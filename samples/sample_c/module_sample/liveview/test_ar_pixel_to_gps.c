/**
 ********************************************************************
 * @file    test_ar_pixel_to_gps.c
 * @brief   Sample demonstrating pixel-to-GPS conversion using ONLY
 *          AR SEI camera pose + camera focal length, without requiring
 *          AI detection targets.
 *
 * This sample differs from the AI SEI geo converter in that it:
 *   - Only requires AR SEI (hasAr), does NOT need AI SEI (hasAi)
 *   - Converts user-specified pixel coordinates with a configurable distance
 *   - Demonstrates multiple conversion scenarios:
 *     a) Screen center → GPS (camera look-at point)
 *     b) Preset test points (corners + center) → GPS
 *     c) Camera's own GPS position from AR pose
 *     d) AI target GPS (bonus, when AI data is available)
 *
 * Flow:
 *   1. Initialize liveview module
 *   2. Register stream callback with SEI parsing
 *   3. On each frame with valid AR + FOV data:
 *      - Update intrinsics from FOV SEI (auto-tracks zoom changes)
 *      - Convert preset pixel points to GPS using configurable distance
 *      - Compute camera GPS from AR position + gndOrigin
 *      - If AI targets also present, convert those too
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
#include "test_ar_pixel_to_gps.h"

/* Private constants ---------------------------------------------------------*/
#define TAG "ArPixelGpsSample"
#define LOG_INTERVAL            60   /* Log conversion results every N frames */
#define DEFAULT_DISTANCE_M      50.0f  /* Default assumed distance in meters */

#define EARTH_RADIUS            6378137.0
#define DEG_TO_RAD              (M_PI / 180.0)
#define RAD_TO_DEG              (180.0 / M_PI)

/* Stream diagnostics / raw dump configuration -------------------------------*/
#define STREAM_STAT_INTERVAL    90      /* Print a stream heartbeat every N frames (~3s @30fps) */
#define SEI_DUMP_REFRESH_INTERVAL 300   /* Refresh full SEI field dump every N frames (~10s @30fps) */
#define STREAM_DUMP_SECONDS     10      /* Dump raw stream for the first N seconds */
#define STREAM_DUMP_PATH_MAX    256
#define FIRST_FRAME_TIMEOUT_SEC 5       /* Warn if no frame arrives within N seconds */

/**
 * @brief Preset screen test points (normalized [0.0, 1.0]).
 * Center + four quadrant sampling points.
 */
typedef struct {
    const char *name;
    float x;        /* normalized screen X [0.0, 1.0] */
    float y;        /* normalized screen Y [0.0, 1.0] */
} T_ScreenTestPoint;

static const T_ScreenTestPoint s_testPoints[] = {
    {"Center",      0.50f, 0.50f},
    {"TopLeft",     0.25f, 0.25f},
    {"TopRight",    0.75f, 0.25f},
    {"BottomLeft",  0.25f, 0.75f},
    {"BottomRight", 0.75f, 0.75f},
};
#define TEST_POINT_COUNT (sizeof(s_testPoints) / sizeof(s_testPoints[0]))

/* Private variables ---------------------------------------------------------*/
static T_DjiGeoCameraIntrinsics s_intrinsics = {0};
static uint32_t s_frameCount = 0;
static volatile bool s_isRunning = false;
static volatile bool s_stopRequested = false;
static float s_distanceMeters = DEFAULT_DISTANCE_M;
/* Physical camera source to switch to via CameraManager (H30: DEFAULT/WIDE/ZOOM).
 * The liveview subscription itself always uses SOURCE_DEFAULT; on H30 the wide/zoom
 * selection is done by DjiCameraManager_SetStreamSource, NOT by a separate data_idx. */
static E_DjiCameraManagerStreamSource s_streamSource = DJI_CAMERA_MANAGER_SOURCE_DEFAULT_CAM;
/* Payload port the camera is mounted on (M400: may be port 1 or port 2).
 * Same numeric value maps to both mount position and liveview camera position. */
static E_DjiMountPosition s_mountPosition = DJI_MOUNT_POSITION_PAYLOAD_PORT_NO1;

/* Stream diagnostics / raw dump state */
static uint64_t s_totalBytes = 0;         /* Total stream bytes received */
static uint32_t s_seiFrameCount = 0;      /* Frames that carried any SEI */
static uint32_t s_arFrameCount = 0;       /* Frames that carried AR SEI */
static FILE    *s_dumpFp = NULL;          /* Raw stream dump file handle */
static char     s_dumpPath[STREAM_DUMP_PATH_MAX] = {0};
static time_t   s_streamStartTime = 0;    /* Wall-clock time of first frame */
static bool     s_dumpFinished = false;   /* Set once the 15s dump completes */
static const char *s_codecName = "unknown";  /* Detected codec (H.264/H.265) */
static bool     s_seenPayloadType[256] = {false}; /* Cumulative: which F5 TLV types ever seen */
static bool     s_cameraManagerInited = false;    /* Track CameraManager init for cleanup */

/* SEI verification dump state: print the full decoded field set the first time
 * each SEI block is seen (then refreshed periodically), so the received stream
 * can be checked field-by-field against the AR/FOV/AI SEI developer guide.
 * Runs independently of the pixel->GPS preconditions. */
static bool     s_dumpedArOnce = false;
static bool     s_dumpedFovOnce = false;
static bool     s_dumpedAiOnce = false;

/* Exported functions (stop interface) ---------------------------------------*/

void DjiUser_ArPixelGpsRequestStop(void)
{
    USER_LOG_INFO("[ArPixelGPS] Stop requested");
    s_stopRequested = true;
    s_isRunning = false;
}

void DjiUser_ArPixelGpsSetSource(E_DjiCameraManagerStreamSource source)
{
    s_streamSource = source;
    USER_LOG_INFO("[ArPixelGPS] Physical camera source set to %d "
                  "(0=DEFAULT/keep, 1=WIDE, 2=ZOOM)", (int)source);
}

void DjiUser_ArPixelGpsSetPosition(E_DjiMountPosition position)
{
    s_mountPosition = position;
    USER_LOG_INFO("[ArPixelGPS] Mount position set to %d (payload port)", (int)position);
}

/* Private functions ---------------------------------------------------------*/

/**
 * @brief Update camera intrinsics from FOV SEI data.
 *
 * FOV SEI (TLV type=10) provides horizontal, vertical, and focalLength
 * in the same unit system. This ensures correct pinhole model geometry.
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
        USER_LOG_INFO("[FOV SEI] focal=%u imgW=%u imgH=%u "
                      "FOV_H=%.2f° FOV_V=%.2f°",
                      fovInfo->focalLength, fovInfo->horizontal, fovInfo->vertical,
                      fovH, fovV);
    }
}

/**
 * @brief Compute camera's own GPS from AR SEI position + gndOrigin.
 *
 * The camera position in AR SEI is in GND frame (NED relative to gndOrigin).
 * Convert NED offset to GPS: lat += north/R, lon += east/(R*cos(lat)).
 */
static void ComputeCameraGps(const T_DjiSeiArInfo *arInfo,
                              double *outLat, double *outLon, double *outAlt)
{
    double originLatRad = arInfo->gndOrigin.latitude * DEG_TO_RAD;

    /* NED: x=North, y=East, z=Down */
    double latRad = originLatRad + arInfo->position.x / EARTH_RADIUS;
    double cosLat = cos(latRad);

    double lonRad = arInfo->gndOrigin.longitude * DEG_TO_RAD;
    if (fabs(cosLat) > 1e-7) {
        lonRad += arInfo->position.y / (EARTH_RADIUS * cosLat);
    }

    *outLat = latRad * RAD_TO_DEG;
    *outLon = lonRad * RAD_TO_DEG;
    *outAlt = arInfo->gndOrigin.altitude - arInfo->position.z; /* z=Down, so subtract */
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
            /* H.265 VPS(32)/SPS(33)/PPS(34) are strong HEVC markers */
            if (h265Type >= 32 && h265Type <= 34) {
                return "H.265/HEVC";
            }
            /* H.264 SPS(7)/PPS(8)/IDR(5) markers */
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
 *   - whether this stream carries AR/AI/FOV SEI (needed for pixel->GPS)
 * Also dumps the first STREAM_DUMP_SECONDS of raw stream to a file for offline
 * analysis by the streaming team.
 */
static void StreamDiagnostics(const uint8_t *buf, uint32_t len,
                              const T_DjiSeiInfo *seiInfo)
{
    /* First frame: init timers, detect codec, open dump file */
    if (s_streamStartTime == 0) {
        s_streamStartTime = time(NULL);
        s_codecName = DetectCodec(buf, len);

        struct tm *lt = localtime(&s_streamStartTime);
        snprintf(s_dumpPath, sizeof(s_dumpPath),
                 "ar_pixel_gps_stream_src%d_%04d%02d%02d_%02d-%02d-%02d.video",
                 (int)s_streamSource,
                 lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday,
                 lt->tm_hour, lt->tm_min, lt->tm_sec);
        s_dumpFp = fopen(s_dumpPath, "wb");
        if (s_dumpFp == NULL) {
            USER_LOG_ERROR("[ArGPS] Open dump file failed: %s", s_dumpPath);
        } else {
            USER_LOG_INFO("[ArGPS] === First frame received! codec=%s len=%u ===",
                          s_codecName, len);
            USER_LOG_INFO("[ArGPS] Dumping first %ds of raw stream to: %s",
                          STREAM_DUMP_SECONDS, s_dumpPath);
        }
    }

    s_totalBytes += len;
    if (seiInfo != NULL) {
        s_seiFrameCount++;
        if (seiInfo->hasAr) s_arFrameCount++;
        /* Record every F5 TLV type ever seen (cumulative), incl. unparsed ones */
        for (int i = 0; i < seiInfo->payloadTypeCount; i++) {
            s_seenPayloadType[seiInfo->payloadTypes[i] & 0xFF] = true;
        }
    }

    /* Write raw stream to dump file for the first STREAM_DUMP_SECONDS */
    if (!s_dumpFinished && s_dumpFp != NULL) {
        if (difftime(time(NULL), s_streamStartTime) < STREAM_DUMP_SECONDS) {
            fwrite(buf, 1, len, s_dumpFp);
        } else {
            fflush(s_dumpFp);
            fclose(s_dumpFp);
            s_dumpFp = NULL;
            s_dumpFinished = true;
            USER_LOG_INFO("[ArGPS] === Raw stream dump complete (%ds): %s ===",
                          STREAM_DUMP_SECONDS, s_dumpPath);
            USER_LOG_INFO("[ArGPS] Send this file to the streaming team for analysis.");
        }
    }

    /* Periodic heartbeat so absence of AR/AI never means total silence */
    if (s_frameCount % STREAM_STAT_INTERVAL == 0) {
        char typeStr[128];
        FormatPayloadTypes(seiInfo, typeStr, sizeof(typeStr));
        USER_LOG_DEBUG("[ArGPS] stream alive: frames=%u bytes=%llu codec=%s | "
                      "SEI: any=%s AR=%s AI=%s FOV=%s (arFrames=%u/%u) | F5 types=[%s]",
                      s_frameCount, (unsigned long long)s_totalBytes, s_codecName,
                      (seiInfo != NULL) ? "Y" : "N",
                      (seiInfo != NULL && seiInfo->hasAr) ? "Y" : "N",
                      (seiInfo != NULL && seiInfo->hasAi) ? "Y" : "N",
                      (seiInfo != NULL && seiInfo->hasFov) ? "Y" : "N",
                      s_arFrameCount, s_frameCount, typeStr);
    }
}

/**
 * @brief Map AI target type number to a short human-readable name.
 * Covers legacy (2/3/4/34) and AI 2.0 (34~73) type values from the SEI guide.
 */
static const char *AiTargetTypeName(uint8_t type)
{
    switch (type) {
        case 0:  return "Invalid";
        case 1:  return "Unknown";
        case 2:  return "Person(old)";
        case 3:  return "Car(old)";
        case 4:  return "Boat(old)";
        case 5:  return "Face";
        case 10: return "Bird";
        case 20: return "Beacon";
        case 30: return "GPS";
        case 34: return "Moving";
        case 35: return "Person";
        case 44: return "Car";
        case 54: return "Ship";
        case 61: return "Truck";
        case 70: return "Fire";
        default:
            if (type >= 36 && type <= 43) return "Person(color)";
            if (type >= 45 && type <= 52) return "Car(color)";
            if (type >= 55 && type <= 60) return "EngVehicle";
            if (type >= 71 && type <= 73) return "Animal";
            if (type >= 128)              return "Custom";
            return "Other";
    }
}

/**
 * @brief Dump the full decoded SEI field set for verification against the
 * AR / FOV / AI SEI developer guide.
 *
 * Runs on EVERY frame, INDEPENDENT of the pixel->GPS preconditions (does not
 * need focalLength, does not need AR). Prints each block's complete field set
 * the first time it is seen, then refreshes every SEI_DUMP_REFRESH_INTERVAL frames. This is
 * the "does the received SEI match the doc" check: every value printed here maps
 * directly to a field/table in the developer guide.
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
                          i, t->targetId, t->targetType, AiTargetTypeName(t->targetType),
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
 * @brief Stream callback with parsed SEI data.
 *
 * Core logic:
 *   - Only requires AR SEI (hasAr) to perform pixel→GPS conversion
 *   - Uses DjiGeoConverter_PixelToGps() with preset test points
 *   - Optionally processes AI targets if also available
 */
static void ArStreamCallback(E_DjiLiveViewCameraPosition position,
                              const uint8_t *buf, uint32_t len,
                              const T_DjiSeiInfo *seiInfo)
{
    (void)position;

    s_frameCount++;

    /* Unconditional stream liveness diagnostics + raw dump (see above) */
    StreamDiagnostics(buf, len, seiInfo);

    /* Unconditional full SEI field dump for verification against the SEI guide.
     * Independent of AR/AI/FOV availability and of the pixel->GPS preconditions. */
    DumpSeiVerification(seiInfo);

    /* Update intrinsics from FOV SEI whenever available (auto-tracks zoom) */
    if (seiInfo != NULL && seiInfo->hasFov) {
        UpdateIntrinsicsFromFov(&seiInfo->fov);
    }

    bool shouldLog = (s_frameCount % LOG_INTERVAL == 0);

    /* --- Need AR + valid intrinsics --- */
    if (seiInfo == NULL || !seiInfo->hasAr || s_intrinsics.focalLength == 0) {
        return;
    }

    if (!shouldLog) {
        return;
    }

    const T_DjiSeiArInfo *arInfo = &seiInfo->ar;

    /* === Periodic log output (every LOG_INTERVAL frames) === */

    /* --- 1. Header + SEI availability --- */
    USER_LOG_INFO("[ArGPS] === frame#%u  AR=Y  AI=%s  FOV=Y ===",
                  s_frameCount, seiInfo->hasAi ? "Y" : "N");

    /* --- 2. FOV intrinsics --- */
    {
        double fovH = 2.0 * atan2((double)s_intrinsics.imgWidth / 2.0,
                                   (double)s_intrinsics.focalLength) * RAD_TO_DEG;
        double fovV = 2.0 * atan2((double)s_intrinsics.imgHeight / 2.0,
                                   (double)s_intrinsics.focalLength) * RAD_TO_DEG;
        USER_LOG_INFO("[ArGPS] FOV: focal=%u sensor=%ux%u FOV=%.1fx%.1f deg",
                      s_intrinsics.focalLength,
                      s_intrinsics.imgWidth, s_intrinsics.imgHeight,
                      fovH, fovV);
    }

    /* --- 3. Camera pose + camera GPS --- */
    {
        double camLat, camLon, camAlt;
        ComputeCameraGps(arInfo, &camLat, &camLon, &camAlt);

        USER_LOG_INFO("[ArGPS] Pose: yaw=%.2f pitch=%.2f roll=%.2f | "
                      "CamGPS=(%.7f, %.7f, %.2fm)",
                      arInfo->attitude.yaw, arInfo->attitude.pitch, arInfo->attitude.roll,
                      camLat, camLon, camAlt);
    }

    /* --- 4. Pixel-to-GPS conversion for preset test points --- */
    USER_LOG_INFO("[ArGPS] PixelToGPS @%.1fm:", s_distanceMeters);
    for (int i = 0; i < (int)TEST_POINT_COUNT; i++) {
        T_DjiGeoGpsResult gps = {0};
        T_DjiReturnCode ret = DjiGeoConverter_PixelToGps(
            arInfo, &s_intrinsics,
            s_testPoints[i].x, s_testPoints[i].y,
            s_distanceMeters, &gps);

        if (ret == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_INFO("[ArGPS]   %-12s (%.2f,%.2f) -> (%.7f, %.7f, %.2fm)",
                          s_testPoints[i].name,
                          s_testPoints[i].x, s_testPoints[i].y,
                          gps.latitude, gps.longitude, gps.altitude);
        }
    }

    /* --- 5. AI targets GPS (when AI SEI is also available) --- */
    if (seiInfo->hasAi && seiInfo->ai.targetNum > 0) {
        const T_DjiSeiAiInfo *aiInfo = &seiInfo->ai;
        for (int i = 0; i < aiInfo->targetNum; i++) {
            const T_DjiSeiAiTarget *target = &aiInfo->targets[i];
            if (target->bbox.distance == 0) {
                continue;
            }

            T_DjiGeoGpsResult gps = {0};
            T_DjiReturnCode ret = DjiGeoConverter_AiTargetToGps(
                arInfo, &s_intrinsics, &target->bbox, &gps);

            if (ret == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_INFO("[ArGPS] AI target: id=%u type=%u dist=%.1fm "
                              "-> (%.7f, %.7f, %.2fm)",
                              target->targetId, target->targetType,
                              target->bbox.distance / 1000.0f,
                              gps.latitude, gps.longitude, gps.altitude);
            }
        }
    }
}

/* Interactive stdin prompts (kept inside the sample module) -----------------*/

/**
 * @brief Prompt (stdin) for the payload port the camera is mounted on.
 * On M400 the payload camera may be on port 1 or port 2.
 */
static E_DjiMountPosition DjiUser_ArPixelGpsPromptPosition(void)
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
 * On H30 wide/zoom is switched via DjiCameraManager_SetStreamSource, then the
 * DEFAULT liveview stream is subscribed.
 */
static E_DjiCameraManagerStreamSource DjiUser_ArPixelGpsPromptSource(void)
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

/* Exported functions --------------------------------------------------------*/

void DjiUser_RunArPixelToGpsSampleInteractive(void)
{
    DjiUser_ArPixelGpsSetPosition(DjiUser_ArPixelGpsPromptPosition());
    DjiUser_ArPixelGpsSetSource(DjiUser_ArPixelGpsPromptSource());
    DjiUser_RunArPixelToGpsSample();
}

void DjiUser_RunArPixelToGpsSample(void)
{
    T_DjiReturnCode returnCode;

    s_stopRequested = false;
    s_frameCount = 0;
    memset(&s_intrinsics, 0, sizeof(s_intrinsics));

    /* Reset stream diagnostics / dump state (safe for re-runs) */
    s_totalBytes = 0;
    s_seiFrameCount = 0;
    s_arFrameCount = 0;
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

    USER_LOG_INFO("========================================");
    USER_LOG_INFO("  AR Pixel-to-GPS Sample Starting");
    USER_LOG_INFO("  Distance: %.1f m", s_distanceMeters);
    USER_LOG_INFO("  Mount position (payload port): %d", (int)s_mountPosition);
    USER_LOG_INFO("  Physical camera source: %d (0=DEFAULT/keep, 1=WIDE, 2=ZOOM)",
                  (int)s_streamSource);
    USER_LOG_INFO("========================================");

    /* Step 1: Set bandwidth proportion for high-speed channel */
    const T_DjiDataChannelBandwidthProportionOfHighspeedChannel bandwidthProportion = {10, 60, 30};
    returnCode = DjiHighSpeedDataChannel_SetBandwidthProportion(bandwidthProportion);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Set bandwidth proportion failed: 0x%08llX", returnCode);
        return;
    }

    /* Step 2: On H30 the wide/zoom selection is done via CameraManager, NOT by
     * subscribing a different liveview data_idx (idx=1/7 pushes no frames).
     * So: switch the physical camera first, then subscribe the DEFAULT stream.
     * Only needed for non-DEFAULT sources. If CameraManager init/switch fails
     * (e.g. license too low), DEGRADE gracefully: keep subscribing DEFAULT. */
    if (s_streamSource != DJI_CAMERA_MANAGER_SOURCE_DEFAULT_CAM) {
        returnCode = DjiCameraManager_Init();
        if (returnCode == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            s_cameraManagerInited = true;

            returnCode = DjiCameraManager_SetStreamSource(s_mountPosition, s_streamSource);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("SetStreamSource(%d) failed: 0x%08llX — does this H30 "
                               "support that source?", (int)s_streamSource, returnCode);
                /* not fatal: continue to subscribe DEFAULT so we still see something */
            } else {
                USER_LOG_INFO("Switched physical camera to source %d, waiting to settle...",
                              (int)s_streamSource);
                DjiPlatform_GetOsalHandler()->TaskSleepMs(1500);
            }
        } else {
            /* not fatal: keep running on the DEFAULT stream */
            s_cameraManagerInited = false;
            USER_LOG_WARN("CameraManager init failed: 0x%08llX — cannot switch to "
                          "source %d; falling back to the DEFAULT stream.",
                          returnCode, (int)s_streamSource);
        }
    }

    /* Step 3: Initialize liveview */
    returnCode = DjiLiveview_Init();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Liveview init failed: 0x%08llX", returnCode);
        goto cleanup;
    }
    USER_LOG_INFO("Liveview initialized");

    /* Step 4: Intrinsics will be obtained from FOV SEI in the stream callback. */
    USER_LOG_INFO("Camera intrinsics: waiting for FOV SEI data from stream...");

    /* Step 5: Start stream with SEI parsing. ALWAYS subscribe DEFAULT stream —
     * the physical camera was already switched via CameraManager above. */
    USER_LOG_INFO("Subscribing liveview: position=%d(payload port) source=DEFAULT "
                  "(physical cam switched to %d via CameraManager)",
                  (int)s_mountPosition, (int)s_streamSource);
    returnCode = DjiLiveview_StartStreamWithSei(
        (E_DjiLiveViewCameraPosition)s_mountPosition,
        DJI_LIVEVIEW_CAMERA_SOURCE_DEFAULT,
        ArStreamCallback);

    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Start stream with SEI failed: 0x%08llX", returnCode);
        goto cleanup;
    }
    USER_LOG_INFO("Stream with SEI started, waiting for stream frames / AR data...");
    USER_LOG_INFO("This sample only requires AR SEI (no AI detection needed)");
    USER_LOG_INFO("Raw stream will be dumped for the first %ds for the streaming team",
                  STREAM_DUMP_SECONDS);

    /* Step 6: Main loop — exit on stop request or 'q' from stdin.
     * Also warn once if no frame arrives within FIRST_FRAME_TIMEOUT_SEC. */
    s_isRunning = true;
    {
        time_t loopStart = time(NULL);
        bool timeoutWarned = false;
        while (s_isRunning && !s_stopRequested) {
            usleep(100000); /* 100ms */
            if (!timeoutWarned && s_streamStartTime == 0 &&
                difftime(time(NULL), loopStart) >= FIRST_FRAME_TIMEOUT_SEC) {
                timeoutWarned = true;
                USER_LOG_WARN("No frame received in %ds! Subscription succeeded but "
                              "this source pushes no stream. On H30, DEFAULT is the "
                              "only real stream; WIDE/ZOOM must be switched via "
                              "CameraManager (this sample already does that).",
                              FIRST_FRAME_TIMEOUT_SEC);
                USER_LOG_WARN("Press 'q'+Enter (widget stop) to exit and try another source.");
            }
        }
    }

    USER_LOG_INFO("Stopping...");

    /* Step 7: Cleanup */
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
        USER_LOG_INFO("[ArGPS] Raw stream partially dumped to: %s", s_dumpPath);
    }

    returnCode = DjiLiveview_Deinit();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Liveview deinit failed: 0x%08llX", returnCode);
    }

    /* Restore DEFAULT source and deinit CameraManager if we switched it */
    if (s_cameraManagerInited) {
        DjiCameraManager_SetStreamSource(s_mountPosition, DJI_CAMERA_MANAGER_SOURCE_DEFAULT_CAM);
        returnCode = DjiCameraManager_DeInit();
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("CameraManager deinit failed: 0x%08llX", returnCode);
        }
        s_cameraManagerInited = false;
    }

    USER_LOG_INFO("AR Pixel-to-GPS Sample finished. frames=%u bytes=%llu "
                  "seiFrames=%u arFrames=%u codec=%s",
                  s_frameCount, (unsigned long long)s_totalBytes,
                  s_seiFrameCount, s_arFrameCount, s_codecName);

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
        USER_LOG_INFO("  -> For pixel->GPS you need type 10(FOV). "
                      "If it's absent here, this source carries no FOV SEI.");
    }

    if (s_dumpPath[0] != '\0') {
        USER_LOG_INFO("Raw stream file for streaming team: %s", s_dumpPath);
    }
    if (s_frameCount == 0) {
        USER_LOG_WARN("No stream frame received at all! Check whether camera 1 "
                      "(position NO_1) actually pushes a stream to PSDK.");
    } else if (s_arFrameCount == 0) {
        USER_LOG_WARN("Stream received but NO AR SEI in any frame. This H30 stream "
                      "likely carries no AR/AI info — confirm with the camera team.");
    }
}
