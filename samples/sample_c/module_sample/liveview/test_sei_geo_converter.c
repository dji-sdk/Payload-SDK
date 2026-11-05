/**
 ********************************************************************
 * @file    test_sei_geo_converter.c
 * @brief   Sample: AI detection target pixel-to-GPS conversion.
 *
 * Requires AR SEI (camera pose) + AI SEI (detection targets) + FOV SEI (intrinsics).
 * For each AI target with a valid distance measurement, computes and prints
 * the target's GPS coordinates.
 *
 * Flow:
 *   1. Initialize liveview, start stream with SEI parsing
 *   2. In each callback:
 *      - Update intrinsics from FOV SEI
 *      - When AR + AI + FOV all present, convert each target to GPS
 *   3. Periodically print a summary (every 60 frames)
 *   4. Clean up on exit
 *
 ********************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <unistd.h>
#include <time.h>

#include "dji_logger.h"
#include "dji_typedef.h"
#include "dji_liveview.h"
#include "dji_sei_parser.h"
#include "dji_high_speed_data_channel.h"
#include "dji_platform.h"
#include "dji_camera_manager.h"

#include "dji_geo_converter.h"
#include "test_sei_geo_converter.h"

/* Private constants ---------------------------------------------------------*/
#define TAG "AiGpsSample"
#define LOG_INTERVAL            60   /* Print summary every N frames */

#define EARTH_RADIUS            6378137.0
#define DEG_TO_RAD              (M_PI / 180.0)
#define RAD_TO_DEG              (180.0 / M_PI)

/* Stream diagnostics / raw dump configuration -------------------------------*/
#define STREAM_STAT_INTERVAL    90      /* Print a stream heartbeat every N frames (~3s @30fps) */
#define SEI_DUMP_REFRESH_INTERVAL 300   /* Refresh full SEI field dump every N frames (~10s @30fps) */
#define STREAM_DUMP_SECONDS     10      /* Dump raw stream for the first N seconds */
#define STREAM_DUMP_PATH_MAX    256
#define FIRST_FRAME_TIMEOUT_SEC 5       /* Warn if no frame arrives within N seconds */
#define AI_MOUNT_POSITION       DJI_MOUNT_POSITION_PAYLOAD_PORT_NO1  /* camera 1 */

/* Private variables ---------------------------------------------------------*/
static T_DjiGeoCameraIntrinsics s_intrinsics = {0};
static uint32_t s_frameCount = 0;
static volatile bool s_isRunning = false;
/* Physical camera source to switch to via CameraManager (H30: DEFAULT/WIDE/ZOOM).
 * Liveview subscription always uses SOURCE_DEFAULT; wide/zoom is a CameraManager switch. */
static E_DjiCameraManagerStreamSource s_streamSource = DJI_CAMERA_MANAGER_SOURCE_DEFAULT_CAM;
static bool s_cameraManagerInited = false;  /* Track CameraManager init for cleanup */
/* Payload port the camera is mounted on (M400: may be port 1 or port 2). */
static E_DjiMountPosition s_mountPosition = DJI_MOUNT_POSITION_PAYLOAD_PORT_NO1;

/* Stream diagnostics / raw dump state */
static uint64_t s_totalBytes = 0;         /* Total stream bytes received */
static uint32_t s_seiFrameCount = 0;      /* Frames that carried any SEI */
static uint32_t s_arFrameCount = 0;       /* Frames that carried AR SEI */
static uint32_t s_aiFrameCount = 0;       /* Frames that carried AI SEI */
static FILE    *s_dumpFp = NULL;          /* Raw stream dump file handle */
static char     s_dumpPath[STREAM_DUMP_PATH_MAX] = {0};
static time_t   s_streamStartTime = 0;    /* Wall-clock time of first frame */
static bool     s_dumpFinished = false;   /* Set once the 15s dump completes */
static const char *s_codecName = "unknown";  /* Detected codec (H.264/H.265) */
static bool     s_seenPayloadType[256] = {false}; /* Cumulative: which F5 TLV types ever seen */

/* SEI verification dump state: print the full decoded field set the first time
 * each SEI block is seen (then refreshed periodically), so the received stream
 * can be checked field-by-field against the AR/FOV/AI SEI developer guide.
 * Runs independently of the AR+AI+FOV target->GPS preconditions. */
static bool     s_dumpedArOnce = false;
static bool     s_dumpedFovOnce = false;
static bool     s_dumpedAiOnce = false;

/* Private functions ---------------------------------------------------------*/

/**
 * @brief Map target type number to human-readable name.
 */
static const char *TargetTypeName(uint8_t type)
{
    switch (type) {
        case 0:  return "Invalid";
        case 1:  return "Unknown";
        case 2:  return "Person";
        case 3:  return "Car";
        case 4:  return "Boat";
        case 5:  return "Face";
        case 10: return "Bird";
        case 20: return "Beacon";
        case 30: return "GPS";
        case 34: return "Moving";
        default: return "Other";
    }
}

/**
 * @brief Update intrinsics from FOV SEI.
 */
static void UpdateIntrinsicsFromFov(const T_DjiSeiFovInfo *fovInfo)
{
    if (fovInfo->focalLength == 0 || fovInfo->horizontal == 0 || fovInfo->vertical == 0) {
        return;
    }

    bool changed = (s_intrinsics.focalLength != fovInfo->focalLength ||
                    s_intrinsics.imgWidth    != fovInfo->horizontal  ||
                    s_intrinsics.imgHeight   != fovInfo->vertical);

    s_intrinsics.focalLength = fovInfo->focalLength;
    s_intrinsics.imgWidth    = fovInfo->horizontal;
    s_intrinsics.imgHeight   = fovInfo->vertical;

    if (changed) {
        double fovH = 2.0 * atan2((double)s_intrinsics.imgWidth / 2.0,
                                   (double)s_intrinsics.focalLength) * RAD_TO_DEG;
        double fovV = 2.0 * atan2((double)s_intrinsics.imgHeight / 2.0,
                                   (double)s_intrinsics.focalLength) * RAD_TO_DEG;
        USER_LOG_INFO("[AiGPS] FOV updated: focal=%u sensor=%ux%u FOV=%.1fx%.1f deg",
                      fovInfo->focalLength, fovInfo->horizontal, fovInfo->vertical,
                      fovH, fovV);
    }
}

/**
 * @brief Compute camera's own GPS from AR SEI position + gndOrigin.
 */
static void ComputeCameraGps(const T_DjiSeiArInfo *arInfo,
                              double *outLat, double *outLon, double *outAlt)
{
    double originLatRad = arInfo->gndOrigin.latitude * DEG_TO_RAD;
    double latRad = originLatRad + arInfo->position.x / EARTH_RADIUS;
    double cosLat = cos(latRad);

    double lonRad = arInfo->gndOrigin.longitude * DEG_TO_RAD;
    if (fabs(cosLat) > 1e-7) {
        lonRad += arInfo->position.y / (EARTH_RADIUS * cosLat);
    }

    *outLat = latRad * RAD_TO_DEG;
    *outLon = lonRad * RAD_TO_DEG;
    *outAlt = arInfo->gndOrigin.altitude - arInfo->position.z;
}

/**
 * @brief Best-effort codec detection from the first NAL header (Annex-B).
 *
 * Finds the first start code (00 00 01 or 00 00 00 01) then inspects the
 * NAL type. H.264: type = byte & 0x1F. H.265: type = (byte >> 1) & 0x3F.
 * This is only a hint for the streaming team; the raw dump is codec-agnostic.
 */
static const char *DetectCodec(const uint8_t *buf, uint32_t len)
{
    for (uint32_t i = 0; i + 4 < len && i < 512; i++) {
        if (buf[i] == 0x00 && buf[i + 1] == 0x00 &&
            ((buf[i + 2] == 0x01) || (buf[i + 2] == 0x00 && buf[i + 3] == 0x01))) {
            uint32_t nalPos = (buf[i + 2] == 0x01) ? (i + 3) : (i + 4);
            if (nalPos >= len) break;
            uint8_t h264Type = buf[nalPos] & 0x1F;
            uint8_t h265Type = (buf[nalPos] >> 1) & 0x3F;
            if (h265Type >= 32 && h265Type <= 34) {
                return "H.265/HEVC";
            }
            if (h264Type == 7 || h264Type == 8 || h264Type == 5) {
                return "H.264/AVC";
            }
        }
    }
    return "unknown";
}

/**
 * @brief Map an F5 SEI TLV payload type to a short human-readable name.
 */
static const char *SeiPayloadTypeName(uint16_t type)
{
    switch (type) {
        case 1:  return "ASSIST";
        case 4:  return "AR";
        case 6:  return "ROI";
        case 7:  return "AI";
        case 9:  return "EIS";
        case 10: return "FOV";
        case 155:return "E2E_DELAY";
        default: return "?";
    }
}

/**
 * @brief Format a SEI payload-type list into "4(AR),6(ROI),7(AI)" style text.
 */
static void FormatPayloadTypes(const T_DjiSeiInfo *seiInfo, char *out, size_t outSize)
{
    if (out == NULL || outSize == 0) return;
    out[0] = '\0';
    if (seiInfo == NULL || seiInfo->payloadTypeCount == 0) {
        snprintf(out, outSize, "(none)");
        return;
    }
    size_t used = 0;
    for (int i = 0; i < seiInfo->payloadTypeCount; i++) {
        uint16_t t = seiInfo->payloadTypes[i];
        int n = snprintf(out + used, outSize - used, "%s%u(%s)",
                         (i == 0) ? "" : ",", t, SeiPayloadTypeName(t));
        if (n < 0 || (size_t)n >= outSize - used) break;
        used += n;
    }
}

/**
 * @brief Stream liveness diagnostics + raw dump.
 *
 * Runs UNCONDITIONALLY on every frame (independent of AR/AI availability) so
 * the streaming team can confirm:
 *   - whether any stream data arrives at all (frame count, bytes, codec)
 *   - whether this stream carries AR/AI/FOV SEI (needed for target->GPS)
 * Also dumps the first STREAM_DUMP_SECONDS of raw stream to a file for offline
 * analysis by the streaming team.
 */
static void StreamDiagnostics(const uint8_t *buf, uint32_t len,
                              const T_DjiSeiInfo *seiInfo)
{
    if (s_streamStartTime == 0) {
        s_streamStartTime = time(NULL);
        s_codecName = DetectCodec(buf, len);

        struct tm *lt = localtime(&s_streamStartTime);
        snprintf(s_dumpPath, sizeof(s_dumpPath),
                 "ai_target_gps_stream_src%d_%04d%02d%02d_%02d-%02d-%02d.video",
                 (int)s_streamSource,
                 lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday,
                 lt->tm_hour, lt->tm_min, lt->tm_sec);
        s_dumpFp = fopen(s_dumpPath, "wb");
        if (s_dumpFp == NULL) {
            USER_LOG_ERROR("[AiGPS] Open dump file failed: %s", s_dumpPath);
        } else {
            USER_LOG_INFO("[AiGPS] === First frame received! codec=%s len=%u ===",
                          s_codecName, len);
            USER_LOG_INFO("[AiGPS] Dumping first %ds of raw stream to: %s",
                          STREAM_DUMP_SECONDS, s_dumpPath);
        }
    }

    s_totalBytes += len;
    if (seiInfo != NULL) {
        s_seiFrameCount++;
        if (seiInfo->hasAr) s_arFrameCount++;
        if (seiInfo->hasAi) s_aiFrameCount++;
        /* Record every F5 TLV type ever seen (cumulative), incl. unparsed ones */
        for (int i = 0; i < seiInfo->payloadTypeCount; i++) {
            s_seenPayloadType[seiInfo->payloadTypes[i] & 0xFF] = true;
        }
    }

    if (!s_dumpFinished && s_dumpFp != NULL) {
        if (difftime(time(NULL), s_streamStartTime) < STREAM_DUMP_SECONDS) {
            fwrite(buf, 1, len, s_dumpFp);
        } else {
            fflush(s_dumpFp);
            fclose(s_dumpFp);
            s_dumpFp = NULL;
            s_dumpFinished = true;
            USER_LOG_INFO("[AiGPS] === Raw stream dump complete (%ds): %s ===",
                          STREAM_DUMP_SECONDS, s_dumpPath);
            USER_LOG_INFO("[AiGPS] Send this file to the streaming team for analysis.");
        }
    }

    if (s_frameCount % STREAM_STAT_INTERVAL == 0) {
        char typeStr[128];
        FormatPayloadTypes(seiInfo, typeStr, sizeof(typeStr));
        USER_LOG_DEBUG("[AiGPS] stream alive: frames=%u bytes=%llu codec=%s | "
                      "SEI: any=%s AR=%s AI=%s FOV=%s (ar=%u ai=%u / %u) | F5 types=[%s]",
                      s_frameCount, (unsigned long long)s_totalBytes, s_codecName,
                      (seiInfo != NULL) ? "Y" : "N",
                      (seiInfo != NULL && seiInfo->hasAr) ? "Y" : "N",
                      (seiInfo != NULL && seiInfo->hasAi) ? "Y" : "N",
                      (seiInfo != NULL && seiInfo->hasFov) ? "Y" : "N",
                      s_arFrameCount, s_aiFrameCount, s_frameCount, typeStr);
    }
}

/**
 * @brief Dump the full decoded SEI field set for verification against the
 * AR / FOV / AI SEI developer guide.
 *
 * Runs on EVERY frame, INDEPENDENT of the AR+AI+FOV target->GPS preconditions.
 * Prints each block's complete field set the first time it is seen, then
 * refreshes every SEI_DUMP_REFRESH_INTERVAL frames. Every value printed here maps directly
 * to a field/table in the developer guide, so the received stream can be
 * checked field-by-field.
 */
static void DumpSeiVerification(const T_DjiSeiInfo *seiInfo)
{
    if (seiInfo == NULL) return;

    bool periodic = (s_frameCount % SEI_DUMP_REFRESH_INTERVAL == 0);

    /* ---- FOV SEI (TLV type=10) — doc §4.6 ---- */
    if (seiInfo->hasFov && (!s_dumpedFovOnce || periodic)) {
        s_dumpedFovOnce = true;
        const T_DjiSeiFovInfo *fov = &seiInfo->fov;
        double fovH = (fov->focalLength != 0)
            ? 2.0 * atan2((double)fov->horizontal / 2.0, (double)fov->focalLength) * RAD_TO_DEG : 0.0;
        double fovV = (fov->focalLength != 0)
            ? 2.0 * atan2((double)fov->vertical / 2.0, (double)fov->focalLength) * RAD_TO_DEG : 0.0;
        USER_LOG_INFO("[SEI/FOV] ver=%u horizontal=%u vertical=%u focalLength=%u "
                      "=> FOV_H=%.2f deg FOV_V=%.2f deg",
                      fov->version, fov->horizontal, fov->vertical, fov->focalLength,
                      fovH, fovV);
    }

    /* ---- AR SEI (TLV type=4) — doc §4.2~4.5 ---- */
    if (seiInfo->hasAr && (!s_dumpedArOnce || periodic)) {
        s_dumpedArOnce = true;
        const T_DjiSeiArInfo *ar = &seiInfo->ar;
        USER_LOG_INFO("[SEI/AR ] ver=%u incremental=%u seq=%u deltaTs=%ums",
                      ar->version, ar->incremental, ar->incrementalSeq, ar->deltaTimestamp);
        USER_LOG_INFO("[SEI/AR ] attitude(deg): yaw=%.4f pitch=%.4f roll=%.4f",
                      ar->attitude.yaw, ar->attitude.pitch, ar->attitude.roll);
        USER_LOG_INFO("[SEI/AR ] position(m,NED): N=%.3f E=%.3f D=%.3f",
                      ar->position.x, ar->position.y, ar->position.z);
        USER_LOG_INFO("[SEI/AR ] gndOrigin: lat=%.7f lon=%.7f alt=%.2fm",
                      ar->gndOrigin.latitude, ar->gndOrigin.longitude, ar->gndOrigin.altitude);
    }

    /* ---- AI SEI (TLV type=7) — doc §5 / §8 ---- */
    if (seiInfo->hasAi && (!s_dumpedAiOnce || periodic)) {
        s_dumpedAiOnce = true;
        const T_DjiSeiAiInfo *ai = &seiInfo->ai;
        USER_LOG_INFO("[SEI/AI ] ver=%u ts=%ums frameIdType=%u(%s) sourceId=%u(%s) "
                      "trackId=%u groupCount=%u targetNum=%u typeCountNum=%u hasMask=%d",
                      ai->version, ai->timestamp,
                      ai->frameIdType,
                      ai->frameIdType == 1 ? "BASE" : ai->frameIdType == 3 ? "AIBOX" : "INVALID",
                      ai->sourceId, ai->sourceId == 1 ? "ZOOM" : ai->sourceId == 2 ? "WIDE" : "?",
                      ai->trackId, ai->groupCount, ai->targetNum, ai->typeCountNum,
                      (int)ai->hasMask);

        int showN = ai->targetNum < 8 ? ai->targetNum : 8; /* cap log volume */
        for (int i = 0; i < showN; i++) {
            const T_DjiSeiAiTarget *t = &ai->targets[i];
            USER_LOG_INFO("[SEI/AI ]   [%d] id=%u type=%u(%s) state=%u "
                          "box[0-10000]=(cx=%u,cy=%u,w=%u,h=%u) dist=%umm(%.2fm)",
                          i, t->targetId, t->targetType, TargetTypeName(t->targetType),
                          t->targetState,
                          t->bbox.boxCenterX, t->bbox.boxCenterY,
                          t->bbox.boxWidth, t->bbox.boxHeight,
                          t->bbox.distance, t->bbox.distance / 1000.0f);
        }
        if (ai->targetNum > showN) {
            USER_LOG_INFO("[SEI/AI ]   ... (%u more targets not shown)", ai->targetNum - showN);
        }
        if (ai->hasMask) {
            USER_LOG_INFO("[SEI/AI ]   mask: %ux%u wScale=%.3f hScale=%.3f "
                          "compressed=%uB decoded=%uB",
                          ai->mask.width, ai->mask.height,
                          ai->mask.widthScale, ai->mask.heightScale,
                          ai->mask.maskSize, ai->mask.decodedMaskSize);
        }
    }
}

/**
 * @brief Minimal debug: query the camera's laser ranging (LRF) info directly via
 * CameraManager, independent of the AI SEI bbox.distance.
 *
 * Purpose: when AI targets show dist=0, this tells us whether the camera itself
 * has ANY laser ranging at all (center-screen point), so we can distinguish
 * "camera has no LRF / not aimed" from "LRF works but isn't filled into AI SEI".
 *
 * T_DjiCameraManagerLaserRangingInfo carries: distance (0.1m), the ranged point's
 * GPS (lon/lat/alt), its screen position (0.1%), enable_lidar and an exception code.
 * Throttled to LOG_INTERVAL frames (API max 5Hz). Requires CameraManager inited.
 *
 * On H30 the AI target bbox.distance is NOT filled by the aircraft, but LRF works
 * (center-screen point). So we cache the last VALID LRF fix (exception==0 &&
 * distance>0) and let the AI target loop fall back to its GPS point.
 */
static T_DjiCameraManagerLaserRangingInfo s_lastValidLrf = {0};
static bool s_hasValidLrf = false;

static void DumpLaserRanging(void)
{
    if (!s_cameraManagerInited) {
        /* CameraManager could not be inited (e.g. license too low or unsupported).
         * Print a one-time hint so the tester knows LRF is unavailable. */
        static bool s_hinted = false;
        if (!s_hinted) {
            s_hinted = true;
            USER_LOG_WARN("[AiGPS/LRF] CameraManager not available - laser-ranging "
                          "probe disabled");
        }
        return;
    }

    /* Stop probing after repeated failures so an LRF-less camera doesn't spam logs. */
    static uint32_t s_lrfFailCount = 0;
    if (s_lrfFailCount >= 3) {
        return;
    }

    T_DjiCameraManagerLaserRangingInfo lrf = {0};
    T_DjiReturnCode ret = DjiCameraManager_GetLaserRangingInfo(s_mountPosition, &lrf);
    if (ret != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        s_lrfFailCount++;
        USER_LOG_WARN("[AiGPS/LRF] GetLaserRangingInfo failed: 0x%08llX "
                      "(this camera/source may not support LRF)%s", ret,
                      (s_lrfFailCount >= 3) ? " - stop probing" : "");
        return;
    }
    s_lrfFailCount = 0;  /* recovered */

    /* Cache the last valid fix for the AI-target GPS fallback. */
    if (lrf.exception == 0 && lrf.distance > 0) {
        s_lastValidLrf = lrf;
        s_hasValidLrf = true;
    }

    USER_LOG_INFO("[AiGPS/LRF] distance=%.1fm point=(%.7f, %.7f, %.1fm) "
                  "screen=(%.1f%%, %.1f%%) lidar=%d exception=%u",
                  lrf.distance / 10.0,
                  lrf.longitude, lrf.latitude, lrf.altitude / 10.0,
                  lrf.screenX / 10.0, lrf.screenY / 10.0,
                  (int)lrf.enable_lidar, lrf.exception);
}

/**
 * @brief Stream callback with parsed SEI data (AR + AI + FOV).
 */
static void StreamWithSeiCallback(E_DjiLiveViewCameraPosition position,
                                   const uint8_t *buf, uint32_t len,
                                   const T_DjiSeiInfo *seiInfo)
{
    (void)position;

    s_frameCount++;

    /* Unconditional stream liveness diagnostics + raw dump (see above) */
    StreamDiagnostics(buf, len, seiInfo);

    /* Unconditional full SEI field dump for verification against the SEI guide.
     * Independent of AR/AI/FOV availability and of the target->GPS preconditions. */
    DumpSeiVerification(seiInfo);

    /* Update intrinsics from FOV SEI whenever available */
    if (seiInfo != NULL && seiInfo->hasFov) {
        UpdateIntrinsicsFromFov(&seiInfo->fov);
    }

    /* Minimal LRF debug: probe camera laser ranging directly (independent of AI
     * targets / bbox.distance). Throttled; runs even when no AI target present. */
    if (s_frameCount % LOG_INTERVAL == 0) {
        DumpLaserRanging();
    }

    /* --- Need AR + AI + FOV all present, and at least one target --- */
    if (seiInfo == NULL || !seiInfo->hasAr || !seiInfo->hasAi ||
        seiInfo->ai.targetNum == 0 || s_intrinsics.focalLength == 0) {
        return;
    }

    if (s_frameCount % LOG_INTERVAL != 0) {
        return;
    }

    const T_DjiSeiArInfo *arInfo = &seiInfo->ar;
    const T_DjiSeiAiInfo *aiInfo = &seiInfo->ai;

    /* === Periodic summary output === */

    /* --- 1. Header --- */
    USER_LOG_INFO("[AiGPS] === frame#%u  AR=Y  AI=Y(%u targets)  FOV=Y ===",
                  s_frameCount, aiInfo->targetNum);

    /* --- 2. FOV --- */
    {
        double fovH = 2.0 * atan2((double)s_intrinsics.imgWidth / 2.0,
                                   (double)s_intrinsics.focalLength) * RAD_TO_DEG;
        double fovV = 2.0 * atan2((double)s_intrinsics.imgHeight / 2.0,
                                   (double)s_intrinsics.focalLength) * RAD_TO_DEG;
        USER_LOG_INFO("[AiGPS] FOV: focal=%u sensor=%ux%u FOV=%.1fx%.1f deg",
                      s_intrinsics.focalLength,
                      s_intrinsics.imgWidth, s_intrinsics.imgHeight,
                      fovH, fovV);
    }

    /* --- 3. Camera pose + GPS --- */
    {
        double camLat, camLon, camAlt;
        ComputeCameraGps(arInfo, &camLat, &camLon, &camAlt);

        USER_LOG_INFO("[AiGPS] Pose: yaw=%.2f pitch=%.2f roll=%.2f | "
                      "CamGPS=(%.7f, %.7f, %.2fm)",
                      arInfo->attitude.yaw, arInfo->attitude.pitch, arInfo->attitude.roll,
                      camLat, camLon, camAlt);
    }

    /* --- 4. Each AI target → GPS --- */
    for (int i = 0; i < aiInfo->targetNum; i++) {
        const T_DjiSeiAiTarget *target = &aiInfo->targets[i];

        if (target->bbox.distance == 0) {
            /* H30: AI target carries no distance. Fall back to the last valid LRF
             * fix, whose 'point' is the GPS of the center-screen ranged point.
             * NOTE: this is the LRF center point, only an approximation of THIS
             * target's position (exact only when the target is near screen center). */
            if (s_hasValidLrf) {
                USER_LOG_INFO("[AiGPS] Target: id=%u type=%-7s dist=%.1fm(LRF) "
                              "-> (%.7f, %.7f, %.1fm) [via LRF center point, approx]",
                              target->targetId, TargetTypeName(target->targetType),
                              s_lastValidLrf.distance / 10.0,
                              s_lastValidLrf.latitude, s_lastValidLrf.longitude,
                              s_lastValidLrf.altitude / 10.0);
            } else {
                USER_LOG_INFO("[AiGPS] Target: id=%u type=%-7s (no distance; no LRF fix yet, skipped)",
                              target->targetId, TargetTypeName(target->targetType));
            }
            continue;
        }

        T_DjiGeoGpsResult gps = {0};
        T_DjiReturnCode ret = DjiGeoConverter_AiTargetToGps(
            arInfo, &s_intrinsics, &target->bbox, &gps);

        if (ret == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_INFO("[AiGPS] Target: id=%u type=%-7s dist=%.1fm(SEI) "
                          "-> (%.7f, %.7f, %.2fm)",
                          target->targetId,
                          TargetTypeName(target->targetType),
                          target->bbox.distance / 1000.0f,
                          gps.latitude, gps.longitude, gps.altitude);
        }
    }
}

/* Exported functions --------------------------------------------------------*/

void DjiUser_SeiGeoConverterSetSource(E_DjiCameraManagerStreamSource source)
{
    s_streamSource = source;
    USER_LOG_INFO("[AiGPS] Physical camera source set to %d "
                  "(0=DEFAULT/keep, 1=WIDE, 2=ZOOM)", (int)source);
}

void DjiUser_SeiGeoConverterSetPosition(E_DjiMountPosition position)
{
    s_mountPosition = position;
    USER_LOG_INFO("[AiGPS] Mount position set to %d (payload port)", (int)position);
}

/* Interactive stdin prompts (kept inside the sample module) -----------------*/

/**
 * @brief Prompt (stdin) for the payload port the camera is mounted on.
 * On M400 the payload camera may be on port 1 or port 2.
 */
static E_DjiMountPosition DjiUser_SeiGeoConverterPromptPosition(void)
{
    int sel = 0;
    printf("\n"
           "| Select payload port the camera is mounted on:                     |\n"
           "|   [1] Payload port 1                                               |\n"
           "|   [2] Payload port 2                                               |\n"
           "| Enter 1 / 2: ");
    if (scanf("%d", &sel) != 1) sel = 1;
    if (sel == 2) return DJI_MOUNT_POSITION_PAYLOAD_PORT_NO2;
    if (sel != 1) printf("Invalid input, fallback to port 1.\n");
    return DJI_MOUNT_POSITION_PAYLOAD_PORT_NO1;
}

/**
 * @brief Prompt (stdin) for the physical camera source (H30 = wide + zoom, no IR).
 */
static E_DjiCameraManagerStreamSource DjiUser_SeiGeoConverterPromptSource(void)
{
    int sel = 0;
    printf("\n"
           "| Select PHYSICAL camera source (H30 = wide + zoom, no IR):          |\n"
           "|   [0] DEFAULT - keep current camera                                |\n"
           "|   [1] WIDE  camera                                                 |\n"
           "|   [2] ZOOM  camera                                                 |\n"
           "| Enter 0 / 1 / 2 (used to find which camera carries AR/AI/FOV SEI): ");
    if (scanf("%d", &sel) != 1) sel = 0;
    if (sel == 1) return DJI_CAMERA_MANAGER_SOURCE_WIDE_CAM;
    if (sel == 2) return DJI_CAMERA_MANAGER_SOURCE_ZOOM_CAM;
    if (sel != 0) printf("Invalid input, fallback to DEFAULT(0).\n");
    return DJI_CAMERA_MANAGER_SOURCE_DEFAULT_CAM;
}

void DjiUser_RunSeiGeoConverterSampleInteractive(void)
{
    DjiUser_SeiGeoConverterSetPosition(DjiUser_SeiGeoConverterPromptPosition());
    DjiUser_SeiGeoConverterSetSource(DjiUser_SeiGeoConverterPromptSource());
    DjiUser_RunSeiGeoConverterSample();
}

void DjiUser_RunSeiGeoConverterSample(void)
{
    T_DjiReturnCode returnCode;

    s_frameCount = 0;
    memset(&s_intrinsics, 0, sizeof(s_intrinsics));

    /* Reset LRF fallback cache (safe for re-runs) */
    s_hasValidLrf = false;
    memset(&s_lastValidLrf, 0, sizeof(s_lastValidLrf));

    /* Reset stream diagnostics / dump state (safe for re-runs) */
    s_totalBytes = 0;
    s_seiFrameCount = 0;
    s_arFrameCount = 0;
    s_aiFrameCount = 0;
    s_streamStartTime = 0;
    s_dumpFinished = false;
    s_codecName = "unknown";
    memset(s_seenPayloadType, 0, sizeof(s_seenPayloadType));
    s_dumpedArOnce = false;
    s_dumpedFovOnce = false;
    s_dumpedAiOnce = false;
    if (s_dumpFp != NULL) {
        fclose(s_dumpFp);
        s_dumpFp = NULL;
    }

    USER_LOG_INFO("================================================");
    USER_LOG_INFO("  AI Target-to-GPS Sample (option 'q')");
    USER_LOG_INFO("  Converts AI detected targets to GPS coordinates");
    USER_LOG_INFO("  Requires: AR + AI + FOV SEI data");
    USER_LOG_INFO("  Mount position (payload port): %d", (int)s_mountPosition);
    USER_LOG_INFO("  Physical camera source: %d (0=DEFAULT/keep, 1=WIDE, 2=ZOOM)",
                  (int)s_streamSource);
    USER_LOG_INFO("================================================");

    /* Step 1: Set bandwidth proportion for high-speed channel */
    const T_DjiDataChannelBandwidthProportionOfHighspeedChannel bandwidthProportion = {10, 60, 30};
    returnCode = DjiHighSpeedDataChannel_SetBandwidthProportion(bandwidthProportion);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Set bandwidth proportion failed: 0x%08llX", returnCode);
        return;
    }

    /* Step 2: Init CameraManager (needed for BOTH laser-ranging probe AND
     * wide/zoom source switch). Init is attempted for every source incl. DEFAULT
     * so LRF works on the default stream too. If it fails (e.g. license too low
     * or camera-manager unsupported on this aircraft), we DEGRADE gracefully:
     * LRF + source-switch are disabled, but stream/SEI still run. */
    returnCode = DjiCameraManager_Init();
    if (returnCode == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        s_cameraManagerInited = true;

        /* Only switch the physical camera when a non-DEFAULT source is chosen.
         * On H30 wide/zoom is a CameraManager switch, not a liveview data_idx. */
        if (s_streamSource != DJI_CAMERA_MANAGER_SOURCE_DEFAULT_CAM) {
            returnCode = DjiCameraManager_SetStreamSource(s_mountPosition, s_streamSource);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("SetStreamSource(%d) failed: 0x%08llX — does this H30 "
                               "support that source?", (int)s_streamSource, returnCode);
            } else {
                USER_LOG_INFO("Switched physical camera to source %d, waiting to settle...",
                              (int)s_streamSource);
                DjiPlatform_GetOsalHandler()->TaskSleepMs(1500);
            }
        }
    } else {
        /* Not fatal: keep running with stream + SEI only. */
        s_cameraManagerInited = false;
        USER_LOG_WARN("CameraManager init failed: 0x%08llX — LRF probe and "
                      "wide/zoom switch disabled; stream/SEI still work.", returnCode);
        if (s_streamSource != DJI_CAMERA_MANAGER_SOURCE_DEFAULT_CAM) {
            USER_LOG_WARN("Requested source %d cannot be applied without CameraManager; "
                          "falling back to whatever the DEFAULT stream currently shows.",
                          (int)s_streamSource);
        }
    }

    /* Step 3: Initialize liveview */
    returnCode = DjiLiveview_Init();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Liveview init failed: 0x%08llX", returnCode);
        goto cleanup;
    }
    USER_LOG_INFO("Liveview initialized, waiting for AR + AI + FOV SEI data...");

    /* Step 4: Start stream with SEI parsing. ALWAYS subscribe DEFAULT stream —
     * the physical camera was already switched via CameraManager above. */
    USER_LOG_INFO("Subscribing liveview: position=%d(payload port) source=DEFAULT "
                  "(requested physical cam=%d, cameraMgr=%s)",
                  (int)s_mountPosition, (int)s_streamSource,
                  s_cameraManagerInited ? "on" : "off");
    USER_LOG_INFO("Raw stream will be dumped for the first %ds for the streaming team",
                  STREAM_DUMP_SECONDS);
    returnCode = DjiLiveview_StartStreamWithSei(
        (E_DjiLiveViewCameraPosition)s_mountPosition,
        DJI_LIVEVIEW_CAMERA_SOURCE_DEFAULT,
        StreamWithSeiCallback);

    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Start stream with SEI failed: 0x%08llX", returnCode);
        goto cleanup;
    }

    /* Step 5: Main loop — warn once if no frame arrives within FIRST_FRAME_TIMEOUT_SEC */
    s_isRunning = true;
    {
        time_t loopStart = time(NULL);
        bool timeoutWarned = false;
        while (s_isRunning) {
            usleep(100000); /* 100ms */
            if (!timeoutWarned && s_streamStartTime == 0 &&
                difftime(time(NULL), loopStart) >= FIRST_FRAME_TIMEOUT_SEC) {
                timeoutWarned = true;
                USER_LOG_WARN("No frame received in %ds! Subscription succeeded but "
                              "this source pushes no stream. On H30, DEFAULT is the "
                              "only real stream; WIDE/ZOOM must be switched via "
                              "CameraManager (this sample already does that).",
                              FIRST_FRAME_TIMEOUT_SEC);
            }
        }
    }

    USER_LOG_INFO("Stopping...");

    /* Step 6: Cleanup */
    returnCode = DjiLiveview_StopStreamWithSei(
        (E_DjiLiveViewCameraPosition)s_mountPosition,
        DJI_LIVEVIEW_CAMERA_SOURCE_DEFAULT);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Stop stream failed: 0x%08llX", returnCode);
    }

cleanup:
    /* Close the raw dump file if the sample stopped before 15s elapsed */
    if (s_dumpFp != NULL) {
        fflush(s_dumpFp);
        fclose(s_dumpFp);
        s_dumpFp = NULL;
        s_dumpFinished = true;
        USER_LOG_INFO("[AiGPS] Raw stream partially dumped to: %s", s_dumpPath);
    }

    returnCode = DjiLiveview_Deinit();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Liveview deinit failed: 0x%08llX", returnCode);
    }

    /* Deinit CameraManager if it was inited; restore DEFAULT source only if we
     * had switched to a non-DEFAULT physical camera. */
    if (s_cameraManagerInited) {
        if (s_streamSource != DJI_CAMERA_MANAGER_SOURCE_DEFAULT_CAM) {
            DjiCameraManager_SetStreamSource(s_mountPosition, DJI_CAMERA_MANAGER_SOURCE_DEFAULT_CAM);
        }
        returnCode = DjiCameraManager_DeInit();
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("CameraManager deinit failed: 0x%08llX", returnCode);
        }
        s_cameraManagerInited = false;
    }

    USER_LOG_INFO("AI Target-to-GPS Sample finished. frames=%u bytes=%llu "
                  "seiFrames=%u arFrames=%u aiFrames=%u codec=%s",
                  s_frameCount, (unsigned long long)s_totalBytes,
                  s_seiFrameCount, s_arFrameCount, s_aiFrameCount, s_codecName);

    /* Cumulative list of ALL F5 SEI TLV types seen during the whole run */
    {
        char seenStr[256] = {0};
        size_t used = 0;
        bool any = false;
        for (int t = 0; t < 256; t++) {
            if (!s_seenPayloadType[t]) continue;
            int n = snprintf(seenStr + used, sizeof(seenStr) - used, "%s%d(%s)",
                             any ? "," : "", t, SeiPayloadTypeName((uint16_t)t));
            if (n < 0 || (size_t)n >= sizeof(seenStr) - used) break;
            used += n;
            any = true;
        }
        USER_LOG_INFO("All F5 SEI TLV types seen this run: [%s]", any ? seenStr : "none");
        USER_LOG_INFO("  -> For target->GPS you need type 10(FOV) + 7(AI) + 4(AR). "
                      "If FOV(10) is absent here, this source carries no FOV SEI.");
    }

    if (s_dumpPath[0] != '\0') {
        USER_LOG_INFO("Raw stream file for streaming team: %s", s_dumpPath);
    }
    if (s_frameCount == 0) {
        USER_LOG_WARN("No stream frame received at all! Check whether camera 1 "
                      "(position NO_1) actually pushes a stream to PSDK.");
    } else if (s_arFrameCount == 0 && s_aiFrameCount == 0) {
        USER_LOG_WARN("Stream received but NO AR/AI SEI in any frame. This H30 stream "
                      "likely carries no AR/AI info — confirm with the camera team.");
    }
}
