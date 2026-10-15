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

#include "dji_logger.h"
#include "dji_typedef.h"
#include "dji_liveview.h"
#include "dji_sei_parser.h"
#include "dji_high_speed_data_channel.h"
#include "dji_platform.h"

#include "dji_geo_converter.h"
#include "test_sei_geo_converter.h"

/* Private constants ---------------------------------------------------------*/
#define TAG "AiGpsSample"
#define LOG_INTERVAL            60   /* Print summary every N frames */

#define EARTH_RADIUS            6378137.0
#define DEG_TO_RAD              (M_PI / 180.0)
#define RAD_TO_DEG              (180.0 / M_PI)

/* Private variables ---------------------------------------------------------*/
static T_DjiGeoCameraIntrinsics s_intrinsics = {0};
static uint32_t s_frameCount = 0;
static volatile bool s_isRunning = false;

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
 * @brief Stream callback with parsed SEI data (AR + AI + FOV).
 */
static void StreamWithSeiCallback(E_DjiLiveViewCameraPosition position,
                                   const uint8_t *buf, uint32_t len,
                                   const T_DjiSeiInfo *seiInfo)
{
    (void)position;
    (void)buf;
    (void)len;

    s_frameCount++;

    /* Update intrinsics from FOV SEI whenever available */
    if (seiInfo != NULL && seiInfo->hasFov) {
        UpdateIntrinsicsFromFov(&seiInfo->fov);
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
            USER_LOG_INFO("[AiGPS] Target: id=%u type=%-7s (no distance, skipped)",
                          target->targetId, TargetTypeName(target->targetType));
            continue;
        }

        T_DjiGeoGpsResult gps = {0};
        T_DjiReturnCode ret = DjiGeoConverter_AiTargetToGps(
            arInfo, &s_intrinsics, &target->bbox, &gps);

        if (ret == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_INFO("[AiGPS] Target: id=%u type=%-7s dist=%.1fm "
                          "-> (%.7f, %.7f, %.2fm)",
                          target->targetId,
                          TargetTypeName(target->targetType),
                          target->bbox.distance / 1000.0f,
                          gps.latitude, gps.longitude, gps.altitude);
        }
    }
}

/* Exported functions --------------------------------------------------------*/

void DjiUser_RunSeiGeoConverterSample(void)
{
    T_DjiReturnCode returnCode;

    s_frameCount = 0;
    memset(&s_intrinsics, 0, sizeof(s_intrinsics));

    USER_LOG_INFO("================================================");
    USER_LOG_INFO("  AI Target-to-GPS Sample (option 'o')");
    USER_LOG_INFO("  Converts AI detected targets to GPS coordinates");
    USER_LOG_INFO("  Requires: AR + AI + FOV SEI data");
    USER_LOG_INFO("================================================");

    /* Step 1: Set bandwidth proportion for high-speed channel */
    const T_DjiDataChannelBandwidthProportionOfHighspeedChannel bandwidthProportion = {10, 60, 30};
    returnCode = DjiHighSpeedDataChannel_SetBandwidthProportion(bandwidthProportion);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Set bandwidth proportion failed: 0x%08llX", returnCode);
        return;
    }

    /* Step 2: Initialize liveview */
    returnCode = DjiLiveview_Init();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Liveview init failed: 0x%08llX", returnCode);
        return;
    }
    USER_LOG_INFO("Liveview initialized, waiting for AR + AI + FOV SEI data...");

    /* Step 3: Start stream with SEI parsing */
    returnCode = DjiLiveview_StartStreamWithSei(
        DJI_LIVEVIEW_CAMERA_POSITION_NO_1,
        DJI_LIVEVIEW_CAMERA_SOURCE_DEFAULT,
        StreamWithSeiCallback);

    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Start stream with SEI failed: 0x%08llX", returnCode);
        goto cleanup;
    }

    /* Step 4: Main loop */
    s_isRunning = true;
    while (s_isRunning) {
        usleep(100000); /* 100ms */
    }

    USER_LOG_INFO("Stopping...");

    /* Step 5: Cleanup */
    returnCode = DjiLiveview_StopStreamWithSei(
        DJI_LIVEVIEW_CAMERA_POSITION_NO_1,
        DJI_LIVEVIEW_CAMERA_SOURCE_DEFAULT);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Stop stream failed: 0x%08llX", returnCode);
    }

cleanup:
    returnCode = DjiLiveview_Deinit();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Liveview deinit failed: 0x%08llX", returnCode);
    }

    USER_LOG_INFO("AI Target-to-GPS Sample finished. Total frames: %u", s_frameCount);
}
