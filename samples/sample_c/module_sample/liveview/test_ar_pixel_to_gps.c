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

#include "dji_logger.h"
#include "dji_typedef.h"
#include "dji_liveview.h"
#include "dji_sei_parser.h"
#include "dji_high_speed_data_channel.h"
#include "dji_platform.h"

#include "dji_geo_converter.h"
#include "test_ar_pixel_to_gps.h"

/* Private constants ---------------------------------------------------------*/
#define TAG "ArPixelGpsSample"
#define LOG_INTERVAL            60   /* Log conversion results every N frames */
#define DEFAULT_DISTANCE_M      50.0f  /* Default assumed distance in meters */

#define EARTH_RADIUS            6378137.0
#define DEG_TO_RAD              (M_PI / 180.0)
#define RAD_TO_DEG              (180.0 / M_PI)

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

/* Exported functions (stop interface) ---------------------------------------*/

void DjiUser_ArPixelGpsRequestStop(void)
{
    USER_LOG_INFO("[ArPixelGPS] Stop requested");
    s_stopRequested = true;
    s_isRunning = false;
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
    (void)buf;
    (void)len;

    s_frameCount++;

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

/* Exported functions --------------------------------------------------------*/

void DjiUser_RunArPixelToGpsSample(void)
{
    T_DjiReturnCode returnCode;

    s_stopRequested = false;
    s_frameCount = 0;
    memset(&s_intrinsics, 0, sizeof(s_intrinsics));

    USER_LOG_INFO("========================================");
    USER_LOG_INFO("  AR Pixel-to-GPS Sample Starting");
    USER_LOG_INFO("  Distance: %.1f m", s_distanceMeters);
    USER_LOG_INFO("========================================");

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
    USER_LOG_INFO("Liveview initialized");

    /* Step 3: Intrinsics will be obtained from FOV SEI in the stream callback.
     * No API call needed — FOV SEI provides focal/width/height in consistent units. */
    USER_LOG_INFO("Camera intrinsics: waiting for FOV SEI data from stream...");

    /* Step 4: Start stream with SEI parsing */
    returnCode = DjiLiveview_StartStreamWithSei(
        DJI_LIVEVIEW_CAMERA_POSITION_NO_1,
        DJI_LIVEVIEW_CAMERA_SOURCE_DEFAULT,
        ArStreamCallback);

    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Start stream with SEI failed: 0x%08llX", returnCode);
        goto cleanup;
    }
    USER_LOG_INFO("Stream with SEI started, waiting for AR data...");
    USER_LOG_INFO("This sample only requires AR SEI (no AI detection needed)");
    USER_LOG_INFO("Press 'q' + Enter to quit, 'd' + Enter to change distance");

    /* Step 5: Main loop — exit on stop request or 'q' from stdin */
    s_isRunning = true;
    while (s_isRunning && !s_stopRequested) {
        usleep(100000); /* 100ms */
    }

    USER_LOG_INFO("Stopping...");

    /* Step 6: Cleanup */
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

    USER_LOG_INFO("AR Pixel-to-GPS Sample finished. Total frames: %u", s_frameCount);
}
