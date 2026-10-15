/**
 ********************************************************************
 * @file    test_interest_point.c
 * @brief
 *
 * @copyright (c) 2018 DJI. All rights reserved.
 *
 * All information contained herein is, and remains, the property of DJI.
 * The intellectual and technical concepts contained herein are proprietary
 * to DJI and may be covered by U.S. and foreign patents, patents in process,
 * and protected by trade secret or copyright law.  Dissemination of this
 * information, including but not limited to data and other proprietary
 * material(s) incorporated within the information, in any form, is strictly
 * prohibited without the express written consent of DJI.
 *
 * If you receive this source code without DJI’s authorization, you may not
 * further disseminate the information, and you must immediately remove the
 * source code and notify DJI of its removal. DJI reserves the right to pursue
 * legal actions against you for any loss(es) or damage(s) caused by your
 * failure to do so.
 *
 *********************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "test_interest_point.h"
#include "dji_interest_point.h"
#include "dji_logger.h"
#include "dji_flight_controller.h"
#include "flight_control/test_flight_control.h"
#include "dji_aircraft_info.h"
#include "dji_liveview.h"
#include "dji_sei_parser.h"
#include "dji_hms_customization.h"
#include "widget/test_widget.h"
#include "utils/util_misc.h"
#include <string.h>
#include <stdio.h>


/* Private constants ---------------------------------------------------------*/
#define SPZ_APPROACH_SPEED_MS           (5.0f)   /* approach speed, unit: m/s, range [0, 20] */
#define SPZ_APPROACH_ANGLE_DEG          (60.0f)  /* approach angle, unit: degrees, range [0, 90] */
#define SPZ_TARGET_ZOOM_PERCENTAGE      (50.0f)  /* target screen occupancy, unit: %, 0=min focal, 100=max focal */
#define SPZ_WAIT_STATE_TIMEOUT_MS       (3000)  /* timeout for state transition polling */
#define SPZ_WAIT_STATE_POLL_INTERVAL_MS (200)
#define SPZ_WAIT_STATE_FOCAL_INTERVAL_MS (5000)

/* HMS error code for Step 11 prompt (range: 0x1E020000 ~ 0x1E02FFFF) */
#define SPZ_HMS_CODE_SET_APPROACH_PARAM  (0x1E020000U)

/* Private types -------------------------------------------------------------*/

/* Private values -------------------------------------------------------------*/
static T_DjiReturnCode DjiUser_InterestPointMissionStateCallback(T_DjiInterestPointMissionState missionState);

/* SEI shared state (written by liveview callback, read by main flow) */
static volatile bool     s_seiAiValid   = false;
static T_DjiSeiAiInfo    s_lastSeiAiInfo;

/* SPZ track box parameters — written by DjiTest_SpzSetBoxParam(), read at DRAW_BOX time */
static uint16_t s_spzBoxCx = 5000;
static uint16_t s_spzBoxCy = 5000;
static uint16_t s_spzBoxW  = 1000;
static uint16_t s_spzBoxH  = 1000;
static float s_spzApproachSpeedMs  = SPZ_APPROACH_SPEED_MS;
static float s_spzApproachAngleDeg = SPZ_APPROACH_ANGLE_DEG;

/* FlightController init guard: Init registers cmd handlers that survive DeInit,
 * so only init once per process lifetime to avoid duplicate-handler errors. */
static bool s_fcInitialized = false;
static bool s_interestPointInitialized = false;
static bool s_osdCallbackRegistered = false;
static bool s_liveviewInitialized = false;

/* OSD mode shared state (written by OSD callback, read by main flow) */
static volatile E_DjiMcModeType s_osdMode = (E_DjiMcModeType)0xFF;

/* SPZ mode monitor task */
static volatile bool   s_spzMonitorRunning = false;
static volatile bool   s_spzAbortRequested = false;
static T_DjiTaskHandle s_spzMonitorTask    = NULL;

/**
 * @brief Check if the aircraft is in valid SPZ intelligent tracking state.
 *        Directly wraps DjiInterestPoint_IsSpzTrackingActive, which now checks
 *        submode internally.
 */
static bool DjiUser_SpzIsTrackingActive(void)
{
    bool isActive = false;
    DjiInterestPoint_IsSpzTrackingActive(&isActive);
    return isActive;
}

static void *DjiUser_SpzModeMonitorTask(void *arg)
{
    (void)arg;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    USER_LOG_INFO("[SPZ-MONITOR] Started: DjiInterestPoint_IsSpzTrackingActive + mode==TA(51)");

    /* Wait for first valid OSD push (mode != 0xFF) before checking.
     * On ground or slow link, status may not arrive immediately. */
    uint32_t osdWaitElapsed = 0;
    while (s_spzMonitorRunning && s_osdMode == (E_DjiMcModeType)0xFF) {
        osalHandler->TaskSleepMs(200);
        osdWaitElapsed += 200;
        if (osdWaitElapsed >= 5000) {
            USER_LOG_WARN("[SPZ-MONITOR] OSD push not received within 5s, "
                          "mode check disabled, only using spzState+submode.");
            break;
        }
    }
    USER_LOG_INFO("[SPZ-MONITOR] OSD mode=%u (waited %u ms), begin monitoring.",
                  (uint8_t)s_osdMode, osdWaitElapsed);

    while (s_spzMonitorRunning) {
        osalHandler->TaskSleepMs(500);
        if (!s_spzMonitorRunning) break;

        if (!DjiUser_SpzIsTrackingActive()) {
            T_DjiInterestPointMissionState state = {0};
            DjiInterestPoint_GetLatestMissionState(&state);
            USER_LOG_ERROR("[SPZ-MONITOR] Tracking lost! "
                           "mode=%u(expect 51) spzState=%u submode=%u(expect 12 or 14) -> exiting SPZ",
                           (uint8_t)s_osdMode, state.spzState, state.submode);
            DjiTest_WidgetLogAppend("SPZ: tracking lost, exiting");
            s_spzMonitorRunning = false;
            DjiFlightController_SpotlightZoomStop_Common(DJI_MOUNT_POSITION_EXTENSION_PORT);
            break;
        }
    }

    USER_LOG_INFO("[SPZ-MONITOR] Task exiting.");
    return NULL;
}

static void DjiUser_SpzStartModeMonitor(void)
{
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    s_spzMonitorRunning = true;
    T_DjiReturnCode rc = osalHandler->TaskCreate("spz_monitor",
                                                  DjiUser_SpzModeMonitorTask,
                                                  2048, NULL, &s_spzMonitorTask);
    if (rc != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("[SPZ-MONITOR] TaskCreate failed: 0x%08llX", rc);
        s_spzMonitorRunning = false;
    } else {
        USER_LOG_INFO("[SPZ-MONITOR] Monitor task created.");
    }
}

static void DjiUser_SpzStopModeMonitor(void)
{
    if (s_spzMonitorRunning) {
        s_spzMonitorRunning = false;
        USER_LOG_INFO("[SPZ-MONITOR] Monitor stopped.");
    }
}

/* Private functions declaration ---------------------------------------------*/
static T_DjiReturnCode DjiUser_SpzWaitForState(uint8_t targetSpzState, uint32_t timeoutMs);
static void            DjiUser_SpzSeiCallback(E_DjiLiveViewCameraPosition position,
                                               const uint8_t *data, uint32_t len,
                                               const T_DjiSeiInfo *seiInfo);
static T_DjiReturnCode DjiUser_SpzOsdCallback(T_DjiFlightControllerOsdInfo osdInfo);

/* Exported functions definition ---------------------------------------------*/
/**
 * SPZ Approach Mode Sample
 *
 * Correct flow:
 *  1. Request to enter SPZ
 *  2. Poll to confirm entry (spzState == DJI_SPOTLIGHT_ZOOM_MOT)
 *  3. [Placeholder] Obtain target box from colleague's video stream interface
 *  4. Send target box (DRAW_BOX)
 *  5. Poll to confirm state == WAITING, then confirm tracking
 *  6. [Placeholder] Obtain tracking target info from colleague's interface
 *  7. Set target screen occupancy ratio
 *  8. [Optional] Disable auto zoom
 *  9. Switch to approach mode
 * 10. [Placeholder] Obtain tracking state from colleague's interface
 * 11. Set approach speed and angle
 * 12. Request to exit (or abnormal exit)
 * 13. Query exit reason
 */
void DjiTest_SpzRequestStop(void)
{
    USER_LOG_INFO("[SPZ] Stop requested");
    s_spzAbortRequested = true;
    s_spzMonitorRunning = false;
}

void DjiTest_SpzRunPreInit(void)
{
    T_DjiReturnCode returnCode;
    T_DjiFlightControllerRidInfo ridInfo = {0};
    ridInfo.latitude  = 22.542812;
    ridInfo.longitude = 113.958902;
    ridInfo.altitude  = 10;

    if (!s_fcInitialized) {
        USER_LOG_INFO("[SPZ-PreInit] DjiFlightController_Init");
        returnCode = DjiFlightController_Init(ridInfo);
        if (returnCode == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            s_fcInitialized = true;
        } else {
            USER_LOG_WARN("[SPZ-PreInit] FlightController init: 0x%08llX", returnCode);
        }
    }

    if (!s_osdCallbackRegistered) {
        s_osdMode = (E_DjiMcModeType)0xFF;
        returnCode = DjiFlightController_RegisterOsdInfoCallBack(DjiUser_SpzOsdCallback);
        if (returnCode == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            s_osdCallbackRegistered = true;
        }
    }

    if (!s_interestPointInitialized) {
        USER_LOG_INFO("[SPZ-PreInit] DjiInterestPoint_Init");
        returnCode = DjiInterestPoint_Init();
        if (returnCode == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            s_interestPointInitialized = true;
        } else {
            USER_LOG_WARN("[SPZ-PreInit] InterestPoint init: 0x%08llX", returnCode);
        }
    }

    if (!s_liveviewInitialized) {
        USER_LOG_INFO("[SPZ-PreInit] DjiLiveview_Init");
        returnCode = DjiLiveview_Init();
        if (returnCode == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            s_liveviewInitialized = true;
        } else {
            USER_LOG_WARN("[SPZ-PreInit] Liveview init: 0x%08llX", returnCode);
        }
    }
}

void DjiTest_SpzSetBoxParam(uint16_t cx, uint16_t cy, uint16_t w, uint16_t h) {
    s_spzBoxCx = cx;
    s_spzBoxCy = cy;
    s_spzBoxW  = w;
    s_spzBoxH  = h;
    USER_LOG_INFO("[SPZ] box param updated: cx=%u cy=%u w=%u h=%u", cx, cy, w, h);
}

void DjiTest_SpzSetApproachParam(float speedMs, float angleDeg)
{
    s_spzApproachSpeedMs  = speedMs;
    s_spzApproachAngleDeg = angleDeg;
    USER_LOG_INFO("[SPZ] approach param updated: speed=%.1f m/s angle=%.1f deg", speedMs, angleDeg);
}

T_DjiReturnCode DjiTest_SpzApproachRunSample(void)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    T_DjiFlightControllerRidInfo ridInfo = {0};
    T_DjiFlightControllerSpotlightZoomTrackParam trackParam = {0};
    bool primaryFound = false;
    s_spzAbortRequested = false;  /* clear any previous stop request */
    uint8_t exitReason = 0;
    float approachSpeed = s_spzApproachSpeedMs;
    float approachAngle = s_spzApproachAngleDeg;
    const T_DjiSeiAiTarget *primaryTarget = NULL;

    ridInfo.latitude  = 22.542812;
    ridInfo.longitude = 113.958902;
    ridInfo.altitude  = 10;

    /* ---- init ---- */
    if (!s_fcInitialized) {
        USER_LOG_INFO("[SPZ] DjiFlightController_Init");
        DjiTest_WidgetLogAppend("[SPZ] DjiFlightController_Init");
        returnCode = DjiFlightController_Init(ridInfo);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("[SPZ] FlightController init failed: 0x%08llX", returnCode);
            DjiTest_WidgetLogAppend("[SPZ] FlightController init failed: 0x%08llX", returnCode);
            return returnCode;
        }
        s_fcInitialized = true;
    } else {
        DjiTest_WidgetLogAppend("[SPZ] DjiFlightController already initialized, skipping Init");
        USER_LOG_INFO("[SPZ] DjiFlightController already initialized, skipping Init");
    }

    /* ---- Register OSD callback to monitor mode field ---- */
    s_osdMode = (E_DjiMcModeType)0xFF;
    if (!s_osdCallbackRegistered) {
        returnCode = DjiFlightController_RegisterOsdInfoCallBack(DjiUser_SpzOsdCallback);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_WARN("[SPZ] RegisterOsdInfoCallBack failed: 0x%08llX (non-fatal)", returnCode);
        } else {
            s_osdCallbackRegistered = true;
        }
    }

    if (!s_interestPointInitialized) {
        USER_LOG_INFO("[SPZ] DjiInterestPoint_Init");
        returnCode = DjiInterestPoint_Init();
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            DjiTest_WidgetLogAppend("[SPZ] InterestPoint init failed");
            USER_LOG_ERROR("[SPZ] InterestPoint init failed: 0x%08llX", returnCode);
            goto spz_controller_deinit;
        }
        s_interestPointInitialized = true;
    } else {
        USER_LOG_INFO("[SPZ] DjiInterestPoint already initialized, skipping Init");
    }

    /* ---- Subscribe liveview with SEI ---- */
    if (!s_liveviewInitialized) {
        USER_LOG_INFO("[SPZ] DjiLiveview_Init");
        returnCode = DjiLiveview_Init();
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("[SPZ] Liveview init failed: 0x%08llX", returnCode);
            goto spz_deinit;
        }
        s_liveviewInitialized = true;
    } else {
        USER_LOG_INFO("[SPZ] DjiLiveview already initialized, skipping Init");
    }

    USER_LOG_INFO("[SPZ] Subscribe liveview with SEI (position NO_1, source DEFAULT)");
    DjiTest_WidgetLogAppend("[SPZ] Subscribe liveview+SEI");
    s_seiAiValid = false;
    memset(&s_lastSeiAiInfo, 0, sizeof(s_lastSeiAiInfo));
    returnCode = DjiLiveview_StartStreamWithSei(DJI_LIVEVIEW_CAMERA_POSITION_NO_1,
                                                DJI_LIVEVIEW_CAMERA_SOURCE_DEFAULT,
                                                DjiUser_SpzSeiCallback);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("[SPZ] StartStreamWithSei failed: 0x%08llX", returnCode);
        DjiTest_WidgetLogAppend("[SPZ] StartStreamWithSei failed");
        goto spz_liveview_deinit;
    }

    /* ---- Step 1: request to enter SPZ ---- */
    USER_LOG_INFO("[SPZ] Step1: enter SPZ function");
    DjiTest_WidgetLogAppend("SPZ: entering SPZ function");
    returnCode = DjiFlightController_SpotlightZoomStart_Common(DJI_MOUNT_POSITION_EXTENSION_PORT);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("[SPZ] SpotlightZoom start failed: 0x%08llX", returnCode);
        DjiTest_WidgetLogAppend("[SPZ] SpotlightZoom start failed");
        DjiFlightController_QuerySpotlightZoomExitReason(&exitReason);
        goto spz_stop_liveview;
    }

    /* ---- Step 1b: poll to confirm entry (spzState==MOT) and (mode==TA) ---- */
    USER_LOG_INFO("[SPZ] Step1b: wait for SPZ state -> MOT and mode -> TA ");
    returnCode = DjiUser_SpzWaitForState(DJI_SPOTLIGHT_ZOOM_MOT, SPZ_WAIT_STATE_TIMEOUT_MS);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        DjiTest_WidgetLogAppend("[SPZ] Timed out waiting for MOT state");
        USER_LOG_ERROR("[SPZ] Timed out waiting for MOT state");
        DjiFlightController_QuerySpotlightZoomExitReason(&exitReason);
        goto spz_stop;
    }

    /* Check mode*/
    {
        uint32_t elapsed = 0;
        while (s_osdMode != E_DJI_MC_MODE_TYPE_NAVI_SUBMODE_TA && elapsed < SPZ_WAIT_STATE_TIMEOUT_MS
               && !s_spzAbortRequested) {
            osalHandler->TaskSleepMs(SPZ_WAIT_STATE_POLL_INTERVAL_MS);
            elapsed += SPZ_WAIT_STATE_POLL_INTERVAL_MS;
        }
        if (s_spzAbortRequested) goto spz_stop;
        if (s_osdMode == E_DJI_MC_MODE_TYPE_NAVI_SUBMODE_TA) {
            USER_LOG_INFO("[SPZ] Step1b: confirmed mode=TA, SPZ entry verified.");
            DjiTest_WidgetLogAppend("[SPZ] Step1b: confirmed mode=TA, SPZ entry verified");
        } else {
            USER_LOG_WARN("[SPZ] Step1b: mode=%u after timeout, "
                          "may be ground test limitation. Continuing...", s_osdMode);
            DjiTest_WidgetLogAppend("[SPZ] Step1b: mode=%u after timeout", s_osdMode);
        }
    }

    /* ---- Step 2: send a fixed center-frame box to initiate tracking ---- */
    trackParam.trackType = DJI_FLIGHT_CONTROLLER_SPZ_TRACK_BY_BOX;
    trackParam.box.cx    = s_spzBoxCx;
    trackParam.box.cy    = s_spzBoxCy;
    trackParam.box.w     = s_spzBoxW;
    trackParam.box.h     = s_spzBoxH;
    USER_LOG_INFO("[SPZ] Step2: send target box (DRAW_BOX) cx=%u cy=%u w=%u h=%u",
                  trackParam.box.cx, trackParam.box.cy, trackParam.box.w, trackParam.box.h);
    DjiTest_WidgetLogAppend("[SPZ] DRAW_BOX cx=%u cy=%u w=%u h=%u",
                  trackParam.box.cx, trackParam.box.cy, trackParam.box.w, trackParam.box.h);
    returnCode = DjiFlightController_SpotlightZoomStartTracking_Common(
        DJI_MOUNT_POSITION_PAYLOAD_PORT_NO1, trackParam);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("[SPZ] Set target box failed: 0x%08llX", returnCode);
        DjiFlightController_QuerySpotlightZoomExitReason(&exitReason);
        goto spz_stop;
    }

    /* ---- Step 2b: poll state == WAITING ---- */
    /* ---- Start mode monitor: watch mode(TA) + submode(FREE/FREE_LOCK_YAW) ---- */
    USER_LOG_INFO("[SPZ] Step2b: wait for SPZ state -> WAITING");
    DjiTest_WidgetLogAppend("[SPZ] Step2b: wait for SPZ state -> WAITING");
    DjiUser_SpzStartModeMonitor();

    returnCode = DjiUser_SpzWaitForState(DJI_SPOTLIGHT_ZOOM_WAITING, SPZ_WAIT_STATE_TIMEOUT_MS);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("[SPZ] Timed out waiting for WAITING state");
        DjiFlightController_QuerySpotlightZoomExitReason(&exitReason);
        goto spz_stop;
    }

    /* ---- Step 3: confirm tracking via CONFIRM_TARGET (action_type=3, 7 bytes, no box data) ---- */
    USER_LOG_INFO("[SPZ] Step3: confirm tracking");
    DjiTest_WidgetLogAppend("[SPZ] Step3: confirm tracking");
    {
        T_DjiFlightControllerSpotlightZoomTrackParam confirmParam = {0};
        confirmParam.trackType = DJI_FLIGHT_CONTROLLER_SPZ_TRACK_CONFIRM;
        returnCode = DjiFlightController_SpotlightZoomStartTracking_Common(
            DJI_MOUNT_POSITION_PAYLOAD_PORT_NO1, confirmParam);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("[SPZ] Confirm tracking failed: 0x%08llX", returnCode);
            DjiFlightController_QuerySpotlightZoomExitReason(&exitReason);
            goto spz_stop;
        }
    }

    /* ---- Step 3b: wait for state -> SOT (actively tracking) ---- */
    USER_LOG_INFO("[SPZ] Step3b: wait for SPZ state -> SOT (single-object tracking)");
    DjiTest_WidgetLogAppend("[SPZ] Step3b: wait for SOT");
    returnCode = DjiUser_SpzWaitForState(DJI_SPOTLIGHT_ZOOM_SOT, SPZ_WAIT_STATE_TIMEOUT_MS);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_WARN("[SPZ] Step3b: did not enter SOT within timeout (state stayed WAITING). "
                      "Target may not be recognized by vision system (expected on ground). Continuing...");
    } else {
        USER_LOG_INFO("[SPZ] Step3b: SOT confirmed, actively tracking target.");
        DjiTest_WidgetLogAppend("SPZ: tracking confirmed (SOT)");
    }
    osalHandler->TaskSleepMs(SPZ_WAIT_STATE_FOCAL_INTERVAL_MS);

    /* ---- Step 4: log current SEI tracking state ---- */
    USER_LOG_INFO("[SPZ] Step4: SEI tracking info snapshot");
    DjiTest_WidgetLogAppend("[SPZ] Step4: SEI tracking info snapshot");
    if (s_seiAiValid) {
        USER_LOG_INFO("[SPZ]   trackId=%u targetNum=%u typeCountNum=%u hasMask=%d",
                      s_lastSeiAiInfo.trackId,
                      s_lastSeiAiInfo.targetNum,
                      s_lastSeiAiInfo.typeCountNum,
                      s_lastSeiAiInfo.hasMask);
        DjiTest_WidgetLogAppend("[SPZ]   trackId=%u targetNum=%u typeCountNum=%u hasMask=%d",
                      s_lastSeiAiInfo.trackId,
                      s_lastSeiAiInfo.targetNum,
                      s_lastSeiAiInfo.typeCountNum,
                      s_lastSeiAiInfo.hasMask);
    } else {
        USER_LOG_WARN("[SPZ]   No SEI AI data received yet");
        DjiTest_WidgetLogAppend("[SPZ]   No SEI AI data received yet");
    }

    /* ---- Step 5: wait for focal length range, then set zoom percentage ---- */
    USER_LOG_INFO("[SPZ] Step5: waiting for focal length range push...");
    DjiTest_WidgetLogAppend("[SPZ] Step5: waiting for focal length range push...");
    {
        T_DjiInterestPointMissionState focalState;
        uint32_t focalWaitElapsed = 0;
        bool focalReady = false;
        while (focalWaitElapsed < SPZ_WAIT_STATE_TIMEOUT_MS) {
            DjiInterestPoint_GetLatestMissionState(&focalState);
            if (focalState.maxEquivalentFocalLength > 0 && focalState.minEquivalentFocalLength > 0) {
                USER_LOG_INFO("[SPZ] Step7: focal length ready: min=%u max=%u mm",
                              focalState.minEquivalentFocalLength,
                              focalState.maxEquivalentFocalLength);
                focalReady = true;
                break;
            }
            osalHandler->TaskSleepMs(SPZ_WAIT_STATE_POLL_INTERVAL_MS);
            focalWaitElapsed += SPZ_WAIT_STATE_POLL_INTERVAL_MS;
        }

        if (!focalReady) {
            DjiTest_WidgetLogAppend("[SPZ] Step5: no focal range, skip zoom");
            USER_LOG_WARN("[SPZ] Step5: focal length range not available (camera may not support zoom "
                          "or aircraft not airborne), skipping zoom sweep.");
        } else {
            USER_LOG_INFO("[SPZ] Step5: focal range=[%u,%u]mm, starting zoom sweep 10%%→100%%→10%%",
                          focalState.minEquivalentFocalLength, focalState.maxEquivalentFocalLength);

            /*
             * NOTE: The zoom sweep below is purely for testing the SetZoomByPercentage API.
             *
             * In normal SPZ operation, the SPZ mode itself has its own built-in zoom
             * initialization logic that runs automatically after tracking is confirmed.
             * Manually calling SetZoomByPercentage may conflict with SPZ's internal zoom
             * control if invoked too early.
             *
             * If this sweep is re-enabled, add a 5-second delay after SOT confirmation
             * (before starting the sweep) to let SPZ finish its own zoom init first:
             *
             *   osalHandler->TaskSleepMs(5000);  // wait for SPZ internal zoom to settle
             *
             * Then start the sweep.
             */

            /* Sweep up: 10% → 100% in 10% steps */
            /*
            for (int step = 10; step <= 100; step += 10) {
                returnCode = DjiInterestPoint_SetZoomByPercentage((float)step);
                if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    USER_LOG_ERROR("[SPZ] Set zoom %d%% failed: 0x%08llX", step, returnCode);
                    DjiFlightController_QuerySpotlightZoomExitReason(&exitReason);
                    goto spz_stop;
                }
                USER_LOG_INFO("[SPZ] Step5: zoom → %d%%", step);
                osalHandler->TaskSleepMs(1000);
                DjiInterestPoint_GetLatestMissionState(&focalState);
                {
                    uint32_t r = focalState.maxEquivalentFocalLength - focalState.minEquivalentFocalLength;
                    uint32_t cur = focalState.minEquivalentFocalLength + (uint32_t)(r * step / 100);
                    USER_LOG_INFO("[SPZ] Step5:        focal~%umm (range=[%u,%u]mm)",
                                  cur, focalState.minEquivalentFocalLength, focalState.maxEquivalentFocalLength);
                }
            }
            */
            /* Sweep down: 100% → 10% in 10% steps */
            /*
            for (int step = 90; step >= 10; step -= 10) {
                returnCode = DjiInterestPoint_SetZoomByPercentage((float)step);
                if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    USER_LOG_ERROR("[SPZ] Set zoom %d%% failed: 0x%08llX", step, returnCode);
                    DjiFlightController_QuerySpotlightZoomExitReason(&exitReason);
                    goto spz_stop;
                }
                USER_LOG_INFO("[SPZ] Step5: zoom → %d%%", step);
                osalHandler->TaskSleepMs(1000);
                DjiInterestPoint_GetLatestMissionState(&focalState);
                {
                    uint32_t r = focalState.maxEquivalentFocalLength - focalState.minEquivalentFocalLength;
                    uint32_t cur = focalState.minEquivalentFocalLength + (uint32_t)(r * step / 100);
                    USER_LOG_INFO("[SPZ] Step5:        focal~%umm (range=[%u,%u]mm)",
                                  cur, focalState.minEquivalentFocalLength, focalState.maxEquivalentFocalLength);
                }
            }
            */

            USER_LOG_INFO("[SPZ] Step5: zoom sweep done");
        }
    }

    /* ---- Step 6: [Optional] test auto zoom API ---- */
    /*
     * NOTE: The calls below are purely for testing the SetSpzAutoZoomStatus API.
     * Disabling and immediately re-enabling auto zoom has no practical effect in
     * normal use — SPZ manages auto zoom internally. This block is commented out
     * by default to avoid interfering with SPZ's built-in zoom behaviour.
     *
     * DjiFlightController_SetSpzAutoZoomStatus(DJI_FLIGHT_CONTROLLER_SPZ_AUTO_ZOOM_DISABLE);
     * DjiFlightController_SetSpzAutoZoomStatus(DJI_FLIGHT_CONTROLLER_SPZ_AUTO_ZOOM_ENABLE);
     */

    /* ---- Step 6: disable obstacle avoidance before entering approach mode ---- */
    USER_LOG_INFO("[SPZ] Step6: disable obstacle avoidance");
    DjiTest_WidgetLogAppend("[SPZ] Step6: disable obstacle avoidance");
    {
        E_DjiFlightControllerObstacleAvoidanceEnableStatus horizStatus, upStatus, downStatus;

        /* get current status */
        DjiFlightController_GetHorizontalVisualObstacleAvoidanceEnableStatus(&horizStatus);
        DjiFlightController_GetUpwardsVisualObstacleAvoidanceEnableStatus(&upStatus);
        DjiFlightController_GetDownwardsVisualObstacleAvoidanceEnableStatus(&downStatus);
        USER_LOG_INFO("[SPZ]   obstacle avoidance before: horiz=%u up=%u down=%u",
                      horizStatus, upStatus, downStatus);

        /* disable all three */
        returnCode = DjiFlightController_SetHorizontalVisualObstacleAvoidanceEnableStatus(
            DJI_FLIGHT_CONTROLLER_DISABLE_OBSTACLE_AVOIDANCE);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_WARN("[SPZ]   Set horiz avoidance failed: 0x%08llX (non-fatal)", returnCode);
        }
        returnCode = DjiFlightController_SetUpwardsVisualObstacleAvoidanceEnableStatus(
            DJI_FLIGHT_CONTROLLER_DISABLE_OBSTACLE_AVOIDANCE);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_WARN("[SPZ]   Set upward avoidance failed: 0x%08llX (non-fatal)", returnCode);
        }
        returnCode = DjiFlightController_SetDownwardsVisualObstacleAvoidanceEnableStatus(
            DJI_FLIGHT_CONTROLLER_DISABLE_OBSTACLE_AVOIDANCE);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_WARN("[SPZ]   Set downward avoidance failed: 0x%08llX (non-fatal)", returnCode);
        }
    }

    /* ---- Step 7: read tracking state from SEI after entering approach mode ---- */
    USER_LOG_INFO("[SPZ] Step7: SEI tracking state (approach phase)");
    DjiTest_WidgetLogAppend("[SPZ] Step7: SEI tracking state (approach phase)");
    if (s_seiAiValid) {
        USER_LOG_INFO("[SPZ]   trackId=%u targetNum=%u", s_lastSeiAiInfo.trackId, s_lastSeiAiInfo.targetNum);
        DjiTest_WidgetLogAppend("[SPZ]   trackId=%u targetNum=%u", s_lastSeiAiInfo.trackId, s_lastSeiAiInfo.targetNum);
        if (s_lastSeiAiInfo.trackId != 0) {
            primaryFound = true;
        }
        for (uint8_t i = 0; i < s_lastSeiAiInfo.targetNum && i < 3; i++) {
            USER_LOG_INFO("[SPZ]   target[%u] id=%u type=%u state=%u cx=%u cy=%u w=%u h=%u dist=%umm",
                          i,
                          s_lastSeiAiInfo.targets[i].targetId,
                          s_lastSeiAiInfo.targets[i].targetType,
                          s_lastSeiAiInfo.targets[i].targetState,
                          s_lastSeiAiInfo.targets[i].bbox.boxCenterX,
                          s_lastSeiAiInfo.targets[i].bbox.boxCenterY,
                          s_lastSeiAiInfo.targets[i].bbox.boxWidth,
                          s_lastSeiAiInfo.targets[i].bbox.boxHeight,
                          s_lastSeiAiInfo.targets[i].bbox.distance);
            DjiTest_WidgetLogAppend("[SPZ] tgt[%u] id=%u cx=%u cy=%u dist=%umm",
                          i,
                          s_lastSeiAiInfo.targets[i].targetId,
                          s_lastSeiAiInfo.targets[i].bbox.boxCenterX,
                          s_lastSeiAiInfo.targets[i].bbox.boxCenterY,
                          s_lastSeiAiInfo.targets[i].bbox.distance);
        }
    } else {
        USER_LOG_WARN("[SPZ]   No SEI AI data received");
        DjiTest_WidgetLogAppend("[SPZ]   No SEI AI data received");
    }

    if (!primaryFound) {
        USER_LOG_ERROR("[SPZ] Step7: primary target lost (trackId=%u not in SEI), abort approach",
                        s_seiAiValid ? s_lastSeiAiInfo.trackId : 0);
        DjiTest_WidgetLogAppend("SPZ: target lost, abort approach");
        DjiFlightController_QuerySpotlightZoomExitReason(&exitReason);
        goto spz_stop;
    }

    /* ---- Step 8: switch to approach mode ---- */
    USER_LOG_INFO("[SPZ] Step8: switch to approach mode ");
    DjiTest_WidgetLogAppend("[SPZ] Step8: switch to approach mode ");
    returnCode = DjiFlightController_SetSpzTooCloseLimitMode(DJI_FLIGHT_CONTROLLER_SPZ_APPROACH_MODE);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("[SPZ] Set approach mode failed: 0x%08llX", returnCode);
        DjiTest_WidgetLogAppend("[SPZ] Set approach mode failed: 0x%08llX", returnCode);
        DjiFlightController_QuerySpotlightZoomExitReason(&exitReason);
        goto spz_stop;
    }

    /* ---- Step 9: set approach speed and angle (dynamic based on SEI primary target) ---- */
    /* Inject HMS hint to prompt user to set approach speed/angle via widget index=6 */
    USER_LOG_INFO("[SPZ] Step9: injecting HMS hint - please set approach speed/angle now");
    DjiTest_WidgetLogAppend("SPZ: set speed/angle via index=6 now");
    if (DjiHmsCustomization_IsInited()) {
        DjiHmsCustomization_InjectHmsErrorCode(SPZ_HMS_CODE_SET_APPROACH_PARAM, DJI_HMS_ERROR_LEVEL_HINT);
    } else {
        USER_LOG_WARN("[SPZ] HMS not initialized, hint inject skipped, approach continues.");
    }
    osalHandler->TaskSleepMs(2000);

    DjiTest_WidgetLogAppend("[SPZ] SEI tracking state (approach phase)");
    if (s_lastSeiAiInfo.trackId == 0) {
        USER_LOG_ERROR("[SPZ] Step9: primary target lost (trackId=%u not in SEI), abort approach",
                        s_lastSeiAiInfo.trackId);
        DjiTest_WidgetLogAppend("SPZ: target lost, abort approach");
        DjiFlightController_QuerySpotlightZoomExitReason(&exitReason);
        goto spz_stop;
    }

    approachSpeed = s_spzApproachSpeedMs;
    approachAngle = s_spzApproachAngleDeg;
    USER_LOG_INFO("[SPZ] Step10: set approach speed=%.1f m/s angle=%.1f deg", approachSpeed, approachAngle);
    DjiTest_WidgetLogAppend("SPZ: approach speed=%.1f angle=%.1f", approachSpeed, approachAngle);

    /* Clear the HMS hint */
    if (DjiHmsCustomization_IsInited()) {
        DjiHmsCustomization_EliminateHmsErrorCode(SPZ_HMS_CODE_SET_APPROACH_PARAM);
    } else {
        USER_LOG_WARN("[SPZ] HMS not initialized, hint eliminate skipped.");
    }

    returnCode = DjiInterestPoint_SetApproachSpeed(approachSpeed);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("[SPZ] Set approach speed failed: 0x%08llX", returnCode);
        goto spz_stop;
    }

    returnCode = DjiInterestPoint_SetApproachAngle(approachAngle);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("[SPZ] Set approach angle failed: 0x%08llX", returnCode);
        goto spz_stop;
    }

    /* ---- wait: mission running ---- */
    USER_LOG_INFO("[SPZ] Approach mission running, waiting 900s...");
    DjiTest_WidgetLogAppend("[SPZ] Approach mission running, waiting 900s...");
    for (int i = 0; i < 900; ++i) {
        if (s_spzAbortRequested) goto spz_stop;
        DjiTest_WidgetLogAppend("[SPZ] %d...", i);
        osalHandler->TaskSleepMs(1000);
    }

spz_stop:
    DjiUser_SpzStopModeMonitor();
    /* ---- Step 11: exit SPZ ---- */
    USER_LOG_INFO("[SPZ] Step11: exit SPZ function");
    DjiTest_WidgetLogAppend("SPZ: exiting...");
    returnCode = DjiFlightController_SpotlightZoomStop_Common(DJI_MOUNT_POSITION_EXTENSION_PORT);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("[SPZ] SpotlightZoom stop failed: 0x%08llX", returnCode);
    }

    /* ---- Step 12: query exit reason ---- */
    USER_LOG_INFO("[SPZ] Step12: query exit reason");
    DjiFlightController_QuerySpotlightZoomExitReason(&exitReason);

    /* ---- restore normal mode ---- */
    USER_LOG_INFO("[SPZ] Restore normal mode ");
    DjiFlightController_SetSpzTooCloseLimitMode(DJI_FLIGHT_CONTROLLER_SPZ_NORMAL_MODE);

spz_stop_liveview:
    USER_LOG_INFO("[SPZ] Stop liveview SEI stream");
    DjiLiveview_StopStreamWithSei(DJI_LIVEVIEW_CAMERA_POSITION_NO_1,
                                  DJI_LIVEVIEW_CAMERA_SOURCE_DEFAULT);

spz_liveview_deinit:
spz_deinit:
    /* Do NOT call DjiInterestPoint_DeInit() — same reason as FlightController:
     * cmd handler is not cleaned up and causes duplicate on re-entry. */

spz_controller_deinit:
    /* Do NOT call UnregisterOsdInfoCallBack/DeInit — handlers stay registered
     * for the process lifetime to avoid duplicate errors on re-entry. */
    return returnCode;
}

/* Private functions definition-----------------------------------------------*/
static T_DjiReturnCode DjiUser_InterestPointMissionStateCallback(T_DjiInterestPointMissionState missionState)
{
    USER_LOG_INFO("Interest point state: %s, radius: %.2f m, speed: %.2f m/s",
                  missionState.state == DJI_INTEREST_POINT_MISSION_ACTION_STATE_NOT_STARTED ? "not started" :
                  missionState.state == DJI_INTEREST_POINT_MISSION_ACTION_STATE_PAUSE ? "pause" :
                  missionState.state == DJI_INTEREST_POINT_MISSION_ACTION_STATE_RUNNING ? "running" : "unknown",
                  missionState.radius,
                  missionState.curSpeed);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * @brief Poll the SPZ state push until the expected state is reached or timeout.
 */
static T_DjiReturnCode DjiUser_SpzWaitForState(uint8_t targetSpzState, uint32_t timeoutMs)
{
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    T_DjiInterestPointMissionState state;
    uint32_t elapsed = 0;

    while (elapsed < timeoutMs) {
        DjiInterestPoint_GetLatestMissionState(&state);
        if (state.spzState == targetSpzState) {
            USER_LOG_INFO("[SPZ] State reached: %u", targetSpzState);
            return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
        }
        osalHandler->TaskSleepMs(SPZ_WAIT_STATE_POLL_INTERVAL_MS);
        elapsed += SPZ_WAIT_STATE_POLL_INTERVAL_MS;
    }

    USER_LOG_ERROR("[SPZ] WaitForState timeout: expected=%u current=%u", targetSpzState, state.spzState);
    return DJI_ERROR_SYSTEM_MODULE_CODE_TIMEOUT;
}

T_DjiReturnCode DjiTest_InterestPointRunSample(void)
{
    T_DjiReturnCode returnCode;
    T_DjiInterestPointSettings interestPointSettings = {0};
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    T_DjiFlightControllerRidInfo ridInfo = {0};
    T_DjiAircraftInfoBaseInfo aircraftInfoBaseInfo = {0};

    ridInfo.latitude = 22.542812;
    ridInfo.longitude = 113.958902;
    ridInfo.altitude = 10;

    returnCode = DjiAircraftInfo_GetBaseInfo(&aircraftInfoBaseInfo);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("get aircraft base info error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    returnCode = DjiFlightController_Init(ridInfo);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Flight control init failed, errno=%lld", returnCode);
        return returnCode;
    }

    returnCode = DjiInterestPoint_Init();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Point interest init failed, errno=%lld", returnCode);
        goto controller_deinit;
    }

    osalHandler->TaskSleepMs(1000);
    DjiFlightController_ObtainJoystickCtrlAuthority();

    osalHandler->TaskSleepMs(1000);
    DjiFlightController_StartTakeoff();

    osalHandler->TaskSleepMs(1000);
    DjiTest_FlightControlVelocityAndYawRateCtrl((T_DjiTestFlightControlVector3f) {0, 0, 5}, 0, 10000);

    osalHandler->TaskSleepMs(1000);
    DjiTest_FlightControlVelocityAndYawRateCtrl((T_DjiTestFlightControlVector3f) {3, 0, 0}, 0, 5000);

    interestPointSettings.latitude = 22.57775;
    interestPointSettings.longitude = 113.94265;

    returnCode = DjiInterestPoint_RegMissionStateCallback(DjiUser_InterestPointMissionStateCallback);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Register mission state callback failed, errno=%lld", returnCode);
        goto point_deinit;
    }

    if (aircraftInfoBaseInfo.aircraftSeries == DJI_AIRCRAFT_SERIES_M400) {
        interestPointSettings.payloadCameraIndex = 1;
        USER_LOG_INFO("The sample try to use payload camera on position %d for poi, please make sure that DJI camera mounted on this position.",
            interestPointSettings.payloadCameraIndex);
    }

    USER_LOG_INFO("Start interest point circle, circumcenter(%f, %f)", interestPointSettings.latitude, interestPointSettings.longitude);
    returnCode = DjiInterestPoint_Start(interestPointSettings);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Point interest start failed, errno=%lld", returnCode);
        goto point_deinit;
    }

    osalHandler->TaskSleepMs(1000);

    USER_LOG_INFO("set speed to 15");
    DjiInterestPoint_SetSpeed(15.0f);

    for (int i = 0; i < 60; ++i) {
        USER_LOG_INFO("Interest point mission running %d.", i);
        osalHandler->TaskSleepMs(1000);
    }

    USER_LOG_INFO("Stop interest point circle");
    returnCode = DjiInterestPoint_Stop();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Point interest stop failed, errno=%lld", returnCode);
    }

point_deinit:
    USER_LOG_INFO("Force landing");
    DjiFlightController_StartForceLanding();

    returnCode = DjiInterestPoint_DeInit();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Point interest deinit failed, errno=%lld", returnCode);
    }

controller_deinit:
    returnCode = DjiFlightController_DeInit();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Flight control init failed, errno=%lld", returnCode);
    }

    return returnCode;
}

/**
 * @brief OSD info callback.
 *        Monitors mode field to confirm SPZ entry (E_DJI_MC_MODE_TYPE_NAVI_SUBMODE_TA).
 */
static T_DjiReturnCode DjiUser_SpzOsdCallback(T_DjiFlightControllerOsdInfo osdInfo)
{
    static uint8_t s_lastMode = 0xFF;
    s_osdMode = osdInfo.mode;

    /* Feed mode into interest point module for DjiInterestPoint_IsSpzTrackingActive() */
    DjiInterestPoint_UpdateOsdMode((uint8_t)osdInfo.mode);

    if (osdInfo.mode != s_lastMode) {
        USER_LOG_INFO("[OSD] mode changed: %u -> %u%s",
                      s_lastMode, osdInfo.mode,
                      osdInfo.mode == E_DJI_MC_MODE_TYPE_NAVI_SUBMODE_TA ? " (NAVI_SUBMODE_TA)" : "");
        s_lastMode = osdInfo.mode;
    }
    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * @brief Liveview SEI callback.
 *
 * Called for every H264 frame from the subscribed stream.
 * - data/len: raw H264 frame (可用于保存或解码，此处忽略)
 * - aiInfo: NULL 表示本帧没有 AI SEI 数据；非 NULL 时打印所有字段并缓存
 */
static void DjiUser_SpzSeiCallback(E_DjiLiveViewCameraPosition position,
                                    const uint8_t *data, uint32_t len,
                                    const T_DjiSeiInfo *seiInfo)
{
    static uint32_t s_seiCallbackCnt = 0;

    if (seiInfo == NULL || !seiInfo->hasAi) {
        return;  /* no AI SEI */
    }

    const T_DjiSeiAiInfo *aiInfo = &seiInfo->ai;

    s_seiCallbackCnt++;

    /* ---- 每 100 次打印一次，避免刷屏 ---- */
    if (s_seiCallbackCnt % 100 == 1) {
        USER_LOG_INFO("[SEI] pos=%d ver=%u ts=%u frameIdType=%u srcId=%u trackId=%u groupCnt=%u",
                    (int)position,
                    aiInfo->version,
                    aiInfo->timestamp,
                    aiInfo->frameIdType,
                    aiInfo->sourceId,
                    aiInfo->trackId,
                    aiInfo->groupCount);

        USER_LOG_INFO("[SEI] targetNum=%u typeCountNum=%u hasMask=%d",
                    aiInfo->targetNum,
                    aiInfo->typeCountNum,
                    (int)aiInfo->hasMask);

        /* ---- 打印主目标（trackId 对应的目标）---- */
        for (uint8_t i = 0; i < aiInfo->targetNum; i++) {
            const T_DjiSeiAiTarget *t = &aiInfo->targets[i];
            USER_LOG_INFO("[SEI]   target[%u] id=%u type=%u state=%u "
                        "cx=%u cy=%u w=%u h=%u dist=%umm",
                        i, t->targetId, t->targetType, t->targetState,
                        t->bbox.boxCenterX, t->bbox.boxCenterY,
                        t->bbox.boxWidth,   t->bbox.boxHeight,
                        t->bbox.distance);
        }

        for (uint8_t i = 0; i < aiInfo->typeCountNum; i++) {
            USER_LOG_INFO("[SEI]   typeCount[%u] type=%u count=%u",
                          i, aiInfo->typeCounts[i].type, aiInfo->typeCounts[i].count);
        }

        if (aiInfo->hasMask) {
            USER_LOG_INFO("[SEI]   mask: %ux%u scale=(%.3f,%.3f) maskSize=%u decodedSize=%u",
                          aiInfo->mask.width,  aiInfo->mask.height,
                          aiInfo->mask.widthScale, aiInfo->mask.heightScale,
                          aiInfo->mask.maskSize,   aiInfo->mask.decodedMaskSize);
        }
    }

    /* ---- 缓存最新有目标的帧 ---- */
    if (aiInfo->targetNum > 0) {
        /* 先在栈上清零 decodedMask，再整体赋值，避免两步写产生悬空指针的中间状态 */
        T_DjiSeiAiInfo tempInfo = *aiInfo;
        tempInfo.mask.decodedMask = NULL;  /* mask 指针由 SDK 管理，缓存后不可使用 */
        s_lastSeiAiInfo = tempInfo;
        s_seiAiValid = true;
    }
}
/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
