/**
 ********************************************************************
 * @file    test_payload_cam_emu_common.c
 * @brief
 *
 * @copyright (c) 2021 DJI. All rights reserved.
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
#include "math.h"
#include "test_payload_cam_emu_base.h"
#include "utils/util_misc.h"
#include "dji_logger.h"
#include "dji_platform.h"
#include "dji_payload_camera.h"
#include "dji_aircraft_info.h"
#include "dji_gimbal.h"
#include "dji_xport.h"
#include "gimbal_emu/test_payload_gimbal_emu.h"
#include <widget_interaction_test/test_widget_interaction.h>

/* Private constants ---------------------------------------------------------*/
#define PAYLOAD_CAMERA_EMU_TASK_FREQ            (100)
#define PAYLOAD_CAMERA_EMU_TASK_STACK_SIZE      (2048)
#define SDCARD_TOTAL_SPACE_IN_MB                (32 * 1024)
#define SDCARD_PER_PHOTO_SPACE_IN_MB            (4)
#define SDCARD_PER_SECONDS_RECORD_SPACE_IN_MB   (2)
#define ZOOM_OPTICAL_FOCAL_MAX_LENGTH           (300)
#define ZOOM_OPTICAL_FOCAL_MIN_LENGTH           (10)
#define ZOOM_OPTICAL_FOCAL_LENGTH_STEP          (10)
#define ZOOM_OPTICAL_FOCAL_LENGTH_CTRL_STEP     (5)
#define ZOOM_DIGITAL_BASE_FACTOR                (1.0)
#define ZOOM_DIGITAL_STEP_FACTOR                (0.1)
#define ZOOM_DIGITAL_MAX_FACTOR                 (6.0)
#define FOCUS_MAX_RINGVALUE                     (1000)
#define FOCUS_MID_RINGVALUE                     (500)
#define IMAGE_SENSOR_X_SIZE                     (88.0f) // unit: 0.1mm
#define IMAGE_SENSOR_Y_SIZE                     (66.0f) // unit: 0.1mm
#define CENTER_POINT_IN_SCREEN_X_VALUE          (0.5f)
#define CENTER_POINT_IN_SCREEN_Y_VALUE          (0.5f)
#define TAP_ZOOM_DURATION                       (2000) // unit: ms
#define INTERVAL_PHOTOGRAPH_ALWAYS_COUNT        (255)
#define INTERVAL_PHOTOGRAPH_INTERVAL_INIT_VALUE (1)    // unit: s
#define TAKING_PHOTO_SPENT_TIME_MS_EMU          (500)
#define PHOTO_FILE_PATH                         "../../../../../api_sample/camera_media_emu/media_file/DJI_0001_ORG.jpg"

// Attention: you can select which camera function you want to run, default: all on.
#define USER_CAMERA_EMU_METERING_ON                1
#define USER_CAMERA_EMU_FOCUS_ON                   1
#define USER_CAMERA_EMU_HYBRID_ZOOM_ON             1
#define USER_CAMERA_EMU_TAP_ZOOM_ON                1

/* Private types -------------------------------------------------------------*/
typedef struct {
    E_DjiGimbalRotationMode rotationMode;
    T_DjiGimbalRotationProperty rotationProperty;
    T_DjiAttitude3d rotationValue;
} T_TestCameraGimbalRotationArgument;

/* Private variables ---------------------------------------------------------*/
static bool s_isCamInited = false;

static T_DjiCameraCommonHandler s_commonHandler;
static T_DjiCameraExposureMeteringHandler s_exposureMeteringHandler;
static T_DjiCameraFocusHandler s_focusHandler;
static T_DjiCameraDigitalZoomHandler s_digitalZoomHandler;
static T_DjiCameraOpticalZoomHandler s_opticalZoomHandler;
static T_DjiCameraTapZoomHandler s_tapZoomHandler;

static T_DjiTaskHandle s_userCameraThread;

static T_DjiCameraSystemState s_cameraState = {0};
static E_DjiCameraShootPhotoMode s_cameraShootPhotoMode = DJI_CAMERA_SHOOT_PHOTO_MODE_SINGLE;
static E_DjiCameraBurstCount s_cameraBurstCount = DJI_CAMERA_BURST_COUNT_2;
static T_DjiCameraPhotoTimeIntervalSettings s_cameraPhotoTimeIntervalSettings = {INTERVAL_PHOTOGRAPH_ALWAYS_COUNT,
                                                                                 INTERVAL_PHOTOGRAPH_INTERVAL_INIT_VALUE};
static T_DjiCameraSDCardState s_cameraSDCardState = {0};
static T_DjiMutexHandle s_commonMutex = {0};

static E_DjiCameraMeteringMode s_cameraMeteringMode = DJI_CAMERA_METERING_MODE_CENTER;
static T_DjiCameraSpotMeteringTarget s_cameraSpotMeteringTarget = {0};

static E_DjiCameraFocusMode s_cameraFocusMode = DJI_CAMERA_FOCUS_MODE_AUTO;
static T_DjiCameraPointInScreen s_cameraFocusTarget = {0};
static uint32_t s_cameraFocusRingValue = FOCUS_MID_RINGVALUE;
static T_DjiCameraFocusAssistantSettings s_cameraFocusAssistantSettings = {0};

static E_DjiCameraZoomDirection s_cameraZoomDirection = DJI_CAMERA_ZOOM_DIRECTION_OUT;
static E_DjiCameraZoomSpeed s_cameraZoomSpeed = DJI_CAMERA_ZOOM_SPEED_NORMAL;
static uint32_t s_cameraOpticalZoomFocalLength = ZOOM_OPTICAL_FOCAL_MIN_LENGTH;
static dji_f32_t s_cameraDigitalZoomFactor = ZOOM_DIGITAL_BASE_FACTOR;
static bool s_isStartContinuousOpticalZoom = false;
static bool s_isOpticalZoomReachLimit = false;
static T_DjiMutexHandle s_zoomMutex = {0};

static bool s_isTapZoomEnabled = false;
static T_DjiCameraTapZoomState s_cameraTapZoomState = {0};
static uint8_t s_tapZoomMultiplier = 1;
static uint32_t s_tapZoomStartTime = 0;
static bool s_isStartTapZoom = false;
static bool s_isTapZooming = false;
static T_TestCameraGimbalRotationArgument s_tapZoomNewestGimbalRotationArgument = {0};
static uint32_t s_tapZoomNewestTargetHybridFocalLength = 0; // unit: 0.1mm
static T_DjiMutexHandle s_tapZoomMutex = NULL;
static E_DjiCameraVideoStreamType s_cameraVideoStreamType;
static uint32_t s_currentVideoRecordTimeInSeconds = 0;

/* Private functions declaration ---------------------------------------------*/
static T_DjiReturnCode GetSystemState(T_DjiCameraSystemState *systemState);
static T_DjiReturnCode SetMode(E_DjiCameraMode mode);
static T_DjiReturnCode StartRecordVideo(void);
static T_DjiReturnCode StopRecordVideo(void);
static T_DjiReturnCode StartShootPhoto(void);
static T_DjiReturnCode StopShootPhoto(void);
static T_DjiReturnCode SetShootPhotoMode(E_DjiCameraShootPhotoMode mode);
static T_DjiReturnCode GetShootPhotoMode(E_DjiCameraShootPhotoMode *mode);
static T_DjiReturnCode SetPhotoBurstCount(E_DjiCameraBurstCount burstCount);
static T_DjiReturnCode GetPhotoBurstCount(E_DjiCameraBurstCount *burstCount);
static T_DjiReturnCode SetPhotoTimeIntervalSettings(T_DjiCameraPhotoTimeIntervalSettings settings);
static T_DjiReturnCode GetPhotoTimeIntervalSettings(T_DjiCameraPhotoTimeIntervalSettings *settings);
static T_DjiReturnCode GetSDCardState(T_DjiCameraSDCardState *sdCardState);
static T_DjiReturnCode FormatSDCard(void);

static T_DjiReturnCode SetMeteringMode(E_DjiCameraMeteringMode mode);
static T_DjiReturnCode GetMeteringMode(E_DjiCameraMeteringMode *mode);
static T_DjiReturnCode SetSpotMeteringTarget(T_DjiCameraSpotMeteringTarget target);
static T_DjiReturnCode GetSpotMeteringTarget(T_DjiCameraSpotMeteringTarget *target);

static T_DjiReturnCode SetFocusMode(E_DjiCameraFocusMode mode);
static T_DjiReturnCode GetFocusMode(E_DjiCameraFocusMode *mode);
static T_DjiReturnCode SetFocusTarget(T_DjiCameraPointInScreen target);
static T_DjiReturnCode GetFocusTarget(T_DjiCameraPointInScreen *target);
static T_DjiReturnCode SetFocusAssistantSettings(T_DjiCameraFocusAssistantSettings settings);
static T_DjiReturnCode GetFocusAssistantSettings(T_DjiCameraFocusAssistantSettings *settings);
static T_DjiReturnCode SetFocusRingValue(uint32_t value);
static T_DjiReturnCode GetFocusRingValue(uint32_t *value);
static T_DjiReturnCode GetFocusRingValueUpperBound(uint32_t *value);

static T_DjiReturnCode SetDigitalZoomFactor(dji_f32_t factor);
static T_DjiReturnCode SetOpticalZoomFocalLength(uint32_t focalLength);
static T_DjiReturnCode GetOpticalZoomFocalLength(uint32_t *focalLength);
static T_DjiReturnCode GetOpticalZoomSpec(T_DjiCameraOpticalZoomSpec *spec);
static T_DjiReturnCode StartContinuousOpticalZoom(E_DjiCameraZoomDirection direction, E_DjiCameraZoomSpeed speed);
static T_DjiReturnCode StopContinuousOpticalZoom(void);
static T_DjiReturnCode GetTapZoomState(T_DjiCameraTapZoomState *state);
static T_DjiReturnCode SetTapZoomEnabled(bool enabledFlag);
static T_DjiReturnCode GetTapZoomEnabled(bool *enabledFlag);
static T_DjiReturnCode SetTapZoomMultiplier(uint8_t multiplier);
static T_DjiReturnCode GetTapZoomMultiplier(uint8_t *multiplier);
static T_DjiReturnCode TapZoomAtTarget(T_DjiCameraPointInScreen target);
static T_DjiReturnCode DjiTest_CameraHybridZoom(uint32_t focalLength);
static T_DjiReturnCode DjiTest_CameraRotationGimbal(T_TestCameraGimbalRotationArgument gimbalRotationArgument);

static void *UserCamera_Task(void *arg);

/* Exported functions definition ---------------------------------------------*/

/* Private functions definition-----------------------------------------------*/
static T_DjiReturnCode GetSystemState(T_DjiCameraSystemState *systemState)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    returnCode = osalHandler->MutexLock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("lock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    *systemState = s_cameraState;

    returnCode = osalHandler->MutexUnlock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("unlock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode SetMode(E_DjiCameraMode mode)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    returnCode = osalHandler->MutexLock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("lock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    s_cameraState.cameraMode = mode;
    USER_LOG_INFO("set camera mode:%d", mode);
    DjiTest_WidgetLogAppend("set cam mode %d", mode);

    returnCode = osalHandler->MutexUnlock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("unlock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode StartRecordVideo(void)
{
    T_DjiReturnCode djiStat;
    T_DjiReturnCode returnCode = DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    djiStat = osalHandler->MutexLock(s_commonMutex);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("lock mutex error: 0x%08llX.", djiStat);
        return djiStat;
    }

    if (s_cameraState.isRecording != false) {
        USER_LOG_ERROR("camera is already in recording state");
        returnCode = DJI_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT_IN_CURRENT_STATE;
        goto out;
    }

    s_cameraState.isRecording = true;
    USER_LOG_INFO("start record video");
    DjiTest_WidgetLogAppend("start record video");

out:
    djiStat = osalHandler->MutexUnlock(s_commonMutex);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("unlock mutex error: 0x%08llX.", djiStat);
        return djiStat;
    }

    return returnCode;
}

static T_DjiReturnCode StopRecordVideo(void)
{
    T_DjiReturnCode djiStat;
    T_DjiReturnCode returnCode = DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    djiStat = osalHandler->MutexLock(s_commonMutex);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("lock mutex error: 0x%08llX.", djiStat);
        return djiStat;
    }

    if (s_cameraState.isRecording != true) {
        USER_LOG_ERROR("camera is not in recording state");
        returnCode = DJI_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT_IN_CURRENT_STATE;
        goto out;
    }

    s_cameraState.isRecording = false;
    s_cameraState.currentVideoRecordingTimeInSeconds = 0;
    USER_LOG_INFO("stop record video");
    DjiTest_WidgetLogAppend("stop record video");

out:
    djiStat = osalHandler->MutexUnlock(s_commonMutex);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("unlock mutex error: 0x%08llX.", djiStat);
        return djiStat;
    }

    return returnCode;
}

static T_DjiReturnCode StartShootPhoto(void)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    returnCode = osalHandler->MutexLock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("lock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    USER_LOG_INFO("start shoot photo");
    DjiTest_WidgetLogAppend("start shoot photo");
    s_cameraState.isStoring = true;

    if (s_cameraShootPhotoMode == DJI_CAMERA_SHOOT_PHOTO_MODE_SINGLE) {
        s_cameraState.shootingState = DJI_CAMERA_SHOOTING_SINGLE_PHOTO;
    } else if (s_cameraShootPhotoMode == DJI_CAMERA_SHOOT_PHOTO_MODE_BURST) {
        s_cameraState.shootingState = DJI_CAMERA_SHOOTING_BURST_PHOTO;
    } else if (s_cameraShootPhotoMode == DJI_CAMERA_SHOOT_PHOTO_MODE_INTERVAL) {
        s_cameraState.shootingState = DJI_CAMERA_SHOOTING_INTERVAL_PHOTO;
        s_cameraState.isShootingIntervalStart = true;
        s_cameraState.currentPhotoShootingIntervalTimeInSeconds = s_cameraPhotoTimeIntervalSettings.timeIntervalSeconds;
        s_cameraState.currentPhotoShootingIntervalTimeInMs = s_cameraPhotoTimeIntervalSettings.timeIntervalMilliseconds;
    }

    returnCode = osalHandler->MutexUnlock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("unlock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode StopShootPhoto(void)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    returnCode = osalHandler->MutexLock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("lock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    USER_LOG_INFO("stop shoot photo");
    DjiTest_WidgetLogAppend("stop shoot photo");
    s_cameraState.shootingState = DJI_CAMERA_SHOOTING_PHOTO_IDLE;
    s_cameraState.isStoring = false;
    s_cameraState.isShootingIntervalStart = false;

    returnCode = osalHandler->MutexUnlock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("unlock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode SetShootPhotoMode(E_DjiCameraShootPhotoMode mode)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    returnCode = osalHandler->MutexLock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("lock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    s_cameraShootPhotoMode = mode;
    USER_LOG_INFO("set shoot photo mode:%d", mode);

    returnCode = osalHandler->MutexUnlock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("unlock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode GetShootPhotoMode(E_DjiCameraShootPhotoMode *mode)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    returnCode = osalHandler->MutexLock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("lock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    *mode = s_cameraShootPhotoMode;

    returnCode = osalHandler->MutexUnlock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("unlock mutex error: 0x%08llX.", returnCode);\
        return returnCode;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode SetPhotoBurstCount(E_DjiCameraBurstCount burstCount)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    returnCode = osalHandler->MutexLock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("lock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    s_cameraBurstCount = burstCount;
    USER_LOG_INFO("set photo burst count:%d", burstCount);

    returnCode = osalHandler->MutexUnlock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("unlock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode GetPhotoBurstCount(E_DjiCameraBurstCount *burstCount)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    returnCode = osalHandler->MutexLock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("lock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    *burstCount = s_cameraBurstCount;

    returnCode = osalHandler->MutexUnlock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("unlock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode SetPhotoTimeIntervalSettings(T_DjiCameraPhotoTimeIntervalSettings settings)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    returnCode = osalHandler->MutexLock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("lock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    s_cameraPhotoTimeIntervalSettings.captureCount = settings.captureCount;
    s_cameraPhotoTimeIntervalSettings.timeIntervalSeconds = settings.timeIntervalSeconds;
    s_cameraPhotoTimeIntervalSettings.timeIntervalMilliseconds = settings.timeIntervalMilliseconds;
    USER_LOG_INFO("set photo interval settings count:%d seconds:%d.%d", settings.captureCount,
                  settings.timeIntervalSeconds, settings.timeIntervalMilliseconds / 100);
    s_cameraState.currentPhotoShootingIntervalCount = settings.captureCount;

    returnCode = osalHandler->MutexUnlock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("unlock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode GetPhotoTimeIntervalSettings(T_DjiCameraPhotoTimeIntervalSettings *settings)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    returnCode = osalHandler->MutexLock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("lock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    memcpy(settings, &s_cameraPhotoTimeIntervalSettings, sizeof(T_DjiCameraPhotoTimeIntervalSettings));

    returnCode = osalHandler->MutexUnlock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("unlock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode GetSDCardState(T_DjiCameraSDCardState *sdCardState)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    returnCode = osalHandler->MutexLock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("lock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    memcpy(sdCardState, &s_cameraSDCardState, sizeof(T_DjiCameraSDCardState));

    returnCode = osalHandler->MutexUnlock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("unlock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode FormatSDCard(void)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    returnCode = osalHandler->MutexLock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("lock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    USER_LOG_INFO("format sdcard");

    memset(&s_cameraSDCardState, 0, sizeof(T_DjiCameraSDCardState));
    s_cameraSDCardState.isInserted = true;
    s_cameraSDCardState.isVerified = true;
    s_cameraSDCardState.totalSpaceInMB = SDCARD_TOTAL_SPACE_IN_MB;
    s_cameraSDCardState.remainSpaceInMB = SDCARD_TOTAL_SPACE_IN_MB;
    s_cameraSDCardState.availableCaptureCount = SDCARD_TOTAL_SPACE_IN_MB / SDCARD_PER_PHOTO_SPACE_IN_MB;
    s_cameraSDCardState.availableRecordingTimeInSeconds =
        SDCARD_TOTAL_SPACE_IN_MB / SDCARD_PER_SECONDS_RECORD_SPACE_IN_MB;

    returnCode = osalHandler->MutexUnlock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("unlock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode SetMeteringMode(E_DjiCameraMeteringMode mode)
{
    USER_LOG_INFO("set metering mode:%d", mode);
    s_cameraMeteringMode = mode;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode GetMeteringMode(E_DjiCameraMeteringMode *mode)
{
    *mode = s_cameraMeteringMode;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode SetSpotMeteringTarget(T_DjiCameraSpotMeteringTarget target)
{
    USER_LOG_INFO("set spot metering area col:%d row:%d", target.col, target.row);
    memcpy(&s_cameraSpotMeteringTarget, &target, sizeof(T_DjiCameraSpotMeteringTarget));

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode GetSpotMeteringTarget(T_DjiCameraSpotMeteringTarget *target)
{
    memcpy(target, &s_cameraSpotMeteringTarget, sizeof(T_DjiCameraSpotMeteringTarget));

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode SetFocusMode(E_DjiCameraFocusMode mode)
{
    USER_LOG_INFO("set focus mode:%d", mode);
    s_cameraFocusMode = mode;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode GetFocusMode(E_DjiCameraFocusMode *mode)
{
    *mode = s_cameraFocusMode;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode SetFocusTarget(T_DjiCameraPointInScreen target)
{
    USER_LOG_INFO("set focus target x:%.2f y:%.2f", target.focusX, target.focusY);
    memcpy(&s_cameraFocusTarget, &target, sizeof(T_DjiCameraPointInScreen));

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode GetFocusTarget(T_DjiCameraPointInScreen *target)
{
    memcpy(target, &s_cameraFocusTarget, sizeof(T_DjiCameraPointInScreen));

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode SetFocusAssistantSettings(T_DjiCameraFocusAssistantSettings settings)
{
    USER_LOG_INFO("set focus assistant setting MF:%d AF:%d", settings.isEnabledMF, settings.isEnabledAF);
    memcpy(&s_cameraFocusAssistantSettings, &settings, sizeof(T_DjiCameraFocusAssistantSettings));

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode GetFocusAssistantSettings(T_DjiCameraFocusAssistantSettings *settings)
{
    memcpy(settings, &s_cameraFocusAssistantSettings, sizeof(T_DjiCameraFocusAssistantSettings));

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode SetFocusRingValue(uint32_t value)
{
    USER_LOG_INFO("set focus ring value:%d", value);
    s_cameraFocusRingValue = value;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode GetFocusRingValue(uint32_t *value)
{
    *value = s_cameraFocusRingValue;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode GetFocusRingValueUpperBound(uint32_t *value)
{
    *value = FOCUS_MAX_RINGVALUE;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode SetDigitalZoomFactor(dji_f32_t factor)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    returnCode = osalHandler->MutexLock(s_zoomMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("lock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    USER_LOG_INFO("set digital zoom factor:%.2f", factor);
    s_cameraDigitalZoomFactor = factor;

    returnCode = osalHandler->MutexUnlock(s_zoomMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("unlock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode SetOpticalZoomFocalLength(uint32_t focalLength)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    returnCode = osalHandler->MutexLock(s_zoomMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("lock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    USER_LOG_INFO("set optical zoom focal length:%d", focalLength);
    s_isOpticalZoomReachLimit = false;
    s_cameraDigitalZoomFactor = ZOOM_DIGITAL_BASE_FACTOR;
    s_cameraOpticalZoomFocalLength = ZOOM_OPTICAL_FOCAL_MIN_LENGTH;

    returnCode = osalHandler->MutexUnlock(s_zoomMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("unlock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode GetOpticalZoomFocalLength(uint32_t *focalLength)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    returnCode = osalHandler->MutexLock(s_zoomMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("lock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    *focalLength = s_cameraOpticalZoomFocalLength;

    returnCode = osalHandler->MutexUnlock(s_zoomMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("unlock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode GetOpticalZoomSpec(T_DjiCameraOpticalZoomSpec *spec)
{
    spec->maxFocalLength = ZOOM_OPTICAL_FOCAL_MAX_LENGTH;
    spec->minFocalLength = ZOOM_OPTICAL_FOCAL_MIN_LENGTH;
    spec->focalLengthStep = ZOOM_OPTICAL_FOCAL_LENGTH_STEP;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode StartContinuousOpticalZoom(E_DjiCameraZoomDirection direction, E_DjiCameraZoomSpeed speed)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    returnCode = osalHandler->MutexLock(s_zoomMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("lock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    USER_LOG_INFO("start continuous optical zoom direction:%d speed:%d", direction, speed);
    s_isStartContinuousOpticalZoom = true;
    s_cameraZoomDirection = direction;
    s_cameraZoomSpeed = speed;

    returnCode = osalHandler->MutexUnlock(s_zoomMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("unlock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode StopContinuousOpticalZoom(void)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    returnCode = osalHandler->MutexLock(s_zoomMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("lock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    USER_LOG_INFO("stop continuous optical zoom");
    s_isStartContinuousOpticalZoom = false;
    s_cameraZoomDirection = DJI_CAMERA_ZOOM_DIRECTION_OUT;
    s_cameraZoomSpeed = DJI_CAMERA_ZOOM_SPEED_NORMAL;

    returnCode = osalHandler->MutexUnlock(s_zoomMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("unlock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode GetTapZoomState(T_DjiCameraTapZoomState *state)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    returnCode = osalHandler->MutexLock(s_tapZoomMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("lock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    memcpy(state, &s_cameraTapZoomState, sizeof(T_DjiCameraTapZoomState));

    returnCode = osalHandler->MutexUnlock(s_tapZoomMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("unlock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode SetTapZoomEnabled(bool enabledFlag)
{
    USER_LOG_INFO("set tap zoom enabled flag: %d.", enabledFlag);
    s_isTapZoomEnabled = enabledFlag;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode GetTapZoomEnabled(bool *enabledFlag)
{
    *enabledFlag = s_isTapZoomEnabled;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode SetTapZoomMultiplier(uint8_t multiplier)
{
    USER_LOG_INFO("set tap zoom multiplier: %d.", multiplier);
    s_tapZoomMultiplier = multiplier;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode GetTapZoomMultiplier(uint8_t *multiplier)
{
    *multiplier = s_tapZoomMultiplier;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode TapZoomAtTarget(T_DjiCameraPointInScreen target)
{
    T_DjiReturnCode returnCode;
    E_DjiGimbalRotationMode rotationMode;
    T_DjiGimbalRotationProperty rotationProperty = {0};
    T_DjiAttitude3d rotationValue = {0};
    float hybridFocalLength = 0; // unit: 0.1mm
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    USER_LOG_INFO("tap zoom at target: x %f, y %f.", target.focusX, target.focusY);

    if (s_isTapZoomEnabled != true) {
        USER_LOG_WARN("tap zoom is not enabled.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    if (s_isTapZooming || s_isStartTapZoom) {
        USER_LOG_WARN("The last tap zoom process is not over.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT_IN_CURRENT_STATE;
    }

    rotationMode = DJI_GIMBAL_ROTATION_MODE_RELATIVE_ANGLE;
    rotationProperty.relativeAngleRotation.actionTime = TAP_ZOOM_DURATION / 10;

    returnCode = osalHandler->MutexLock(s_zoomMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("lock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    /* Calculation formula: rotation angle = arctan((coordinate of target in sensor - coordinate of center point in
     * sensor) / hybrid focal length). Here, suppose that images of all pixels of sensor are displayed in screen,
     * and that center of the image sensor coincides with center of rotation of the gimbal, and that optical axis of
     * camera coincides with x-axis of gimbal. */
    hybridFocalLength = (dji_f32_t) s_cameraOpticalZoomFocalLength * s_cameraDigitalZoomFactor;

    returnCode = osalHandler->MutexUnlock(s_zoomMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("unlock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    rotationValue.pitch = (int32_t) (
        atan2f((target.focusY - CENTER_POINT_IN_SCREEN_Y_VALUE) * IMAGE_SENSOR_Y_SIZE, hybridFocalLength) * 1800 /
        DJI_PI);
    rotationValue.yaw = (int32_t) (
        atan2f((target.focusX - CENTER_POINT_IN_SCREEN_X_VALUE) * IMAGE_SENSOR_X_SIZE, hybridFocalLength) * 1800 /
        DJI_PI);

    returnCode = osalHandler->MutexLock(s_tapZoomMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("lock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    s_tapZoomNewestGimbalRotationArgument.rotationMode = rotationMode;
    s_tapZoomNewestGimbalRotationArgument.rotationProperty = rotationProperty;
    s_tapZoomNewestGimbalRotationArgument.rotationValue = rotationValue;
    s_tapZoomNewestTargetHybridFocalLength = (uint32_t) (hybridFocalLength * (float) s_tapZoomMultiplier);

    returnCode = osalHandler->MutexUnlock(s_tapZoomMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("unlock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    s_isStartTapZoom = true;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode DjiTest_CameraHybridZoom(uint32_t focalLength)
{
    if (focalLength >= ZOOM_OPTICAL_FOCAL_MIN_LENGTH && focalLength <= ZOOM_OPTICAL_FOCAL_MAX_LENGTH) {
        s_cameraOpticalZoomFocalLength = focalLength;
        s_cameraDigitalZoomFactor = ZOOM_DIGITAL_BASE_FACTOR;
    } else if (focalLength < ZOOM_OPTICAL_FOCAL_MIN_LENGTH) {
        s_cameraOpticalZoomFocalLength = ZOOM_OPTICAL_FOCAL_MIN_LENGTH;
        s_cameraDigitalZoomFactor = ZOOM_DIGITAL_BASE_FACTOR;

        USER_LOG_ERROR("focal length %d is smaller than min optical focal length %d.", focalLength,
                       ZOOM_OPTICAL_FOCAL_MIN_LENGTH);
        return DJI_ERROR_SYSTEM_MODULE_CODE_OUT_OF_RANGE;
    } else {
        s_cameraOpticalZoomFocalLength = ZOOM_OPTICAL_FOCAL_MAX_LENGTH;
        s_cameraDigitalZoomFactor = (float) focalLength / ZOOM_OPTICAL_FOCAL_MAX_LENGTH;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode DjiTest_CameraRotationGimbal(T_TestCameraGimbalRotationArgument gimbalRotationArgument)
{
    T_DjiReturnCode returnCode;
    T_DjiAircraftInfoBaseInfo aircraftBaseInfo = {0};

    returnCode = DjiAircraftInfo_GetBaseInfo(&aircraftBaseInfo);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("get aircraft information error: 0x%08llX.", returnCode);
        return returnCode;
    }

    if (aircraftBaseInfo.djiAdapterType == DJI_SDK_ADAPTER_TYPE_SKYPORT_V2 ||
        aircraftBaseInfo.djiAdapterType == DJI_SDK_ADAPTER_TYPE_SKYPORT_V3 ||
        aircraftBaseInfo.djiAdapterType == DJI_SDK_ADAPTER_TYPE_EPORT_V2_RIBBON_CABLE ||
        aircraftBaseInfo.djiAdapterType == DJI_SDK_ADAPTER_TYPE_NONE) {
        returnCode = DjiTest_GimbalRotate(gimbalRotationArgument.rotationMode, gimbalRotationArgument.rotationProperty,
                                          gimbalRotationArgument.rotationValue);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("rotate gimbal error: 0x%08llX.", returnCode);
            return returnCode;
        }
    } else if (aircraftBaseInfo.djiAdapterType == DJI_SDK_ADAPTER_TYPE_XPORT) {
        returnCode = DjiXPort_RotateSync(gimbalRotationArgument.rotationMode, gimbalRotationArgument.rotationProperty,
                                         gimbalRotationArgument.rotationValue);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("rotate XPort error: 0x%08llX.", returnCode);
            return returnCode;
        }
    } else {
        USER_LOG_ERROR("PSDK adapter type is invalid: %d.", aircraftBaseInfo.djiAdapterType);
        return DJI_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

#ifndef __CC_ARM
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wreturn-type"
#endif

static void *UserCamera_Task(void *arg)
{
    static uint32_t photoCnt = 0;
    static uint32_t step = 0;
    T_DjiReturnCode returnCode;
    int32_t tempFocalLength = 0;
    dji_f32_t tempDigitalFactor = 0.0f;
    uint32_t currentTime = 0;
    bool isStartIntervalPhotoAction = false;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    uint32_t intervalFreq = 10;

    USER_UTIL_UNUSED(arg);

    while (1) {
        osalHandler->TaskSleepMs(1000 / PAYLOAD_CAMERA_EMU_TASK_FREQ);
        step++;

        returnCode = osalHandler->MutexLock(s_commonMutex);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("lock mutex error: 0x%08llX.", returnCode);
            continue;
        }

        s_cameraSDCardState.availableRecordingTimeInSeconds =
            s_cameraSDCardState.remainSpaceInMB / SDCARD_PER_SECONDS_RECORD_SPACE_IN_MB;
        s_cameraSDCardState.availableCaptureCount = s_cameraSDCardState.remainSpaceInMB / SDCARD_PER_PHOTO_SPACE_IN_MB;

        //realize your photo shoot and storage logic here
        if (s_cameraState.shootingState != DJI_CAMERA_SHOOTING_PHOTO_IDLE &&
            photoCnt++ > TAKING_PHOTO_SPENT_TIME_MS_EMU / (1000 / PAYLOAD_CAMERA_EMU_TASK_FREQ)) {
            photoCnt = 0;

            //store the photo after shooting finished
            if (s_cameraShootPhotoMode == DJI_CAMERA_SHOOT_PHOTO_MODE_SINGLE) {
                s_cameraSDCardState.remainSpaceInMB =
                    s_cameraSDCardState.remainSpaceInMB - SDCARD_PER_PHOTO_SPACE_IN_MB;
                s_cameraState.isStoring = false;
                s_cameraState.shootingState = DJI_CAMERA_SHOOTING_PHOTO_IDLE;
            } else if (s_cameraShootPhotoMode == DJI_CAMERA_SHOOT_PHOTO_MODE_BURST) {
                s_cameraSDCardState.remainSpaceInMB =
                    s_cameraSDCardState.remainSpaceInMB - SDCARD_PER_PHOTO_SPACE_IN_MB * s_cameraBurstCount;
                s_cameraState.isStoring = false;
                s_cameraState.shootingState = DJI_CAMERA_SHOOTING_PHOTO_IDLE;
            } else if (s_cameraShootPhotoMode == DJI_CAMERA_SHOOT_PHOTO_MODE_INTERVAL) {
                if (isStartIntervalPhotoAction == true) {
                    s_cameraState.isStoring = false;
                    s_cameraState.shootingState = DJI_CAMERA_SHOOTING_PHOTO_IDLE;
                    s_cameraSDCardState.remainSpaceInMB =
                        s_cameraSDCardState.remainSpaceInMB - SDCARD_PER_PHOTO_SPACE_IN_MB;
                }
            }

            //check the remain space of sdcard
            if (s_cameraSDCardState.remainSpaceInMB > SDCARD_TOTAL_SPACE_IN_MB) {
                s_cameraSDCardState.remainSpaceInMB = 0;
                s_cameraSDCardState.isFull = true;
            }

#if dji_ARCH_SYS_LINUX
            T_DjiCameraMediaFileInfo mediaFileInfo = {0};

            // push added media file information
            if (s_cameraShootPhotoMode == dji_CAMERA_SHOOT_PHOTO_MODE_SINGLE) {
                if (UtilFile_IsFileExist(PHOTO_FILE_PATH) == true) {
                    psdkStat = PsdkTest_CameraMediaGetFileInfo(PHOTO_FILE_PATH, &mediaFileInfo);
                    if (psdkStat != dji_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                        PsdkLogger_UserLogError("Get media file info error 0x%08llX", psdkStat);
                    }

                    psdkStat = PsdkPayloadCamera_PushAddedMediaFileInfo(PHOTO_FILE_PATH, mediaFileInfo);
                    if (psdkStat != dji_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                        PsdkLogger_UserLogError("Push added media file info error 0x%08llX", psdkStat);
                    }
                } else {
                    PsdkLogger_UserLogWarn("Can't found the media file by path. "
                                           "Probably because media file has not existed. "
                                           "Please check media file if exists. ");
                }
            }
#endif
        }

        returnCode = osalHandler->MutexUnlock(s_commonMutex);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("unlock mutex error: 0x%08llX.", returnCode);
            continue;
        }

        // 10Hz
        if (USER_UTIL_IS_WORK_TURN(step, 10, PAYLOAD_CAMERA_EMU_TASK_FREQ)) {
            returnCode = osalHandler->MutexLock(s_zoomMutex);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("lock mutex error: 0x%08llX.", returnCode);
                continue;
            }

#ifdef USER_CAMERA_EMU_HYBRID_ZOOM_ON
            //Add logic here for zoom camera
            if (s_isStartContinuousOpticalZoom == true) {
                tempDigitalFactor = s_cameraDigitalZoomFactor;
                tempFocalLength = (int32_t) s_cameraOpticalZoomFocalLength;
                if (s_isOpticalZoomReachLimit == false) {
                    if (s_cameraZoomDirection == DJI_CAMERA_ZOOM_DIRECTION_IN) {
                        tempFocalLength += ((int) s_cameraZoomSpeed - DJI_CAMERA_ZOOM_SPEED_SLOWEST + 1) *
                                           ZOOM_OPTICAL_FOCAL_LENGTH_CTRL_STEP;
                    } else if (s_cameraZoomDirection == DJI_CAMERA_ZOOM_DIRECTION_OUT) {
                        tempFocalLength -= ((int) s_cameraZoomSpeed - DJI_CAMERA_ZOOM_SPEED_SLOWEST + 1) *
                                           ZOOM_OPTICAL_FOCAL_LENGTH_CTRL_STEP;
                    }

                    if (tempFocalLength > ZOOM_OPTICAL_FOCAL_MAX_LENGTH) {
                        s_isOpticalZoomReachLimit = true;
                        tempFocalLength = ZOOM_OPTICAL_FOCAL_MAX_LENGTH;
                    }

                    if (tempFocalLength < ZOOM_OPTICAL_FOCAL_MIN_LENGTH) {
                        tempFocalLength = ZOOM_OPTICAL_FOCAL_MIN_LENGTH;
                    }
                } else {
                    if (s_cameraZoomDirection == DJI_CAMERA_ZOOM_DIRECTION_IN) {
                        tempDigitalFactor += (dji_f32_t) ZOOM_DIGITAL_STEP_FACTOR;
                    } else if (s_cameraZoomDirection == DJI_CAMERA_ZOOM_DIRECTION_OUT) {
                        tempDigitalFactor -= (dji_f32_t) ZOOM_DIGITAL_STEP_FACTOR;
                    }

                    if (tempDigitalFactor > (dji_f32_t) ZOOM_DIGITAL_MAX_FACTOR) {
                        tempDigitalFactor = (dji_f32_t) ZOOM_DIGITAL_MAX_FACTOR;
                    }

                    if (tempDigitalFactor < (dji_f32_t) ZOOM_DIGITAL_BASE_FACTOR) {
                        s_isOpticalZoomReachLimit = false;
                        tempDigitalFactor = ZOOM_DIGITAL_BASE_FACTOR;
                    }
                }
                s_cameraOpticalZoomFocalLength = (uint16_t) tempFocalLength;
                s_cameraDigitalZoomFactor = tempDigitalFactor;
            }
#endif

#if USER_CAMERA_EMU_TAP_ZOOM_ON
            returnCode = osalHandler->MutexLock(s_tapZoomMutex);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("lock mutex error: 0x%08llX.", returnCode);
                goto out;
            }

            if (s_isStartTapZoom) {
                s_isStartTapZoom = false;
                s_isTapZooming = true;

                returnCode = osalHandler->GetTimeMs(&s_tapZoomStartTime);
                if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    USER_LOG_ERROR("get start time error: 0x%08llX.", returnCode);
                }

                returnCode = DjiTest_CameraRotationGimbal(s_tapZoomNewestGimbalRotationArgument);
                if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
                    USER_LOG_ERROR("rotate gimbal error: 0x%08llX.", returnCode);
                else
                    s_cameraTapZoomState.isGimbalMoving = true;

                returnCode = DjiTest_CameraHybridZoom(s_tapZoomNewestTargetHybridFocalLength);
                if (returnCode == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    s_cameraTapZoomState.zoomState = (dji_f32_t) s_tapZoomNewestTargetHybridFocalLength >
                                                     ((dji_f32_t) s_cameraOpticalZoomFocalLength *
                                                      s_cameraDigitalZoomFactor)
                                                     ? DJI_CAMERA_TAP_ZOOM_STATE_ZOOM_IN
                                                     : DJI_CAMERA_TAP_ZOOM_STATE_ZOOM_OUT;
                } else if (returnCode == DJI_ERROR_SYSTEM_MODULE_CODE_OUT_OF_RANGE) {
                    USER_LOG_ERROR("hybrid zoom focal length beyond limit.");
                    s_cameraTapZoomState.zoomState = DJI_CAMERA_TAP_ZOOM_STATE_ZOOM_LIMITED;
                } else {
                    USER_LOG_ERROR("hybrid zoom error: 0x%08llX.", returnCode);
                }
            } else if (s_isTapZooming) {
                returnCode = osalHandler->GetTimeMs(&currentTime);
                if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    USER_LOG_ERROR("get start time error: 0x%08llX.", returnCode);
                }

                if ((currentTime - s_tapZoomStartTime) >= TAP_ZOOM_DURATION) {
                    s_cameraTapZoomState.zoomState = DJI_CAMERA_TAP_ZOOM_STATE_IDLE;
                    s_cameraTapZoomState.isGimbalMoving = false;
                    s_isTapZooming = false;
                }
            }

            returnCode = osalHandler->MutexUnlock(s_tapZoomMutex);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("unlock mutex error: 0x%08llX.", returnCode);
                goto out;
            }
#endif

out:
            returnCode = osalHandler->MutexUnlock(s_zoomMutex);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("unlock mutex error: 0x%08llX.", returnCode);
                continue;
            }
        }

        if (USER_UTIL_IS_WORK_TURN(step, intervalFreq, PAYLOAD_CAMERA_EMU_TASK_FREQ)) {
            returnCode = osalHandler->MutexLock(s_commonMutex);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("lock mutex error: 0x%08llX.", returnCode);
                continue;
            }

            if (s_cameraState.isRecording) {
                uint16_t preTimeInSeconds = s_cameraState.currentVideoRecordingTimeInSeconds;

                s_currentVideoRecordTimeInSeconds++;

                s_cameraState.currentVideoRecordingTimeInSeconds = s_currentVideoRecordTimeInSeconds / intervalFreq;
                if (s_cameraState.currentVideoRecordingTimeInSeconds > preTimeInSeconds) {
                    s_cameraSDCardState.remainSpaceInMB =
                        s_cameraSDCardState.remainSpaceInMB - SDCARD_PER_SECONDS_RECORD_SPACE_IN_MB;
                    if (s_cameraSDCardState.remainSpaceInMB > SDCARD_TOTAL_SPACE_IN_MB) {
                        s_cameraSDCardState.remainSpaceInMB = 0;
                        s_cameraSDCardState.isFull = true;
                    }
                }
            } else {
                s_currentVideoRecordTimeInSeconds = 0;
            }

            if (s_cameraState.isShootingIntervalStart == false) {
                isStartIntervalPhotoAction = false;
            }

            if (s_cameraShootPhotoMode == DJI_CAMERA_SHOOT_PHOTO_MODE_INTERVAL
                && s_cameraState.isShootingIntervalStart == true && s_cameraPhotoTimeIntervalSettings.captureCount > 0
                && (s_cameraPhotoTimeIntervalSettings.timeIntervalSeconds > 0 || s_cameraPhotoTimeIntervalSettings.timeIntervalMilliseconds > 0)) {
                uint16_t currentPhotoShootingIntervalTimeInMs = s_cameraState.currentPhotoShootingIntervalTimeInSeconds * 1000 +
                                                                s_cameraState.currentPhotoShootingIntervalTimeInMs;

                currentPhotoShootingIntervalTimeInMs -= 1000 / intervalFreq;

                s_cameraState.currentPhotoShootingIntervalTimeInSeconds = currentPhotoShootingIntervalTimeInMs / 1000;
                s_cameraState.currentPhotoShootingIntervalTimeInMs = currentPhotoShootingIntervalTimeInMs % 1000;

                if ((currentPhotoShootingIntervalTimeInMs == 0 &&
                     s_cameraState.currentPhotoShootingIntervalCount > 0) ||
                    (s_cameraState.isShootingIntervalStart == true && isStartIntervalPhotoAction == false)) {

                    isStartIntervalPhotoAction = true;

                    s_cameraState.shootingState = DJI_CAMERA_SHOOTING_INTERVAL_PHOTO;
                    s_cameraState.isStoring = true;
                    s_cameraState.currentPhotoShootingIntervalTimeInSeconds = s_cameraPhotoTimeIntervalSettings.timeIntervalSeconds;
                    s_cameraState.currentPhotoShootingIntervalTimeInMs = s_cameraPhotoTimeIntervalSettings.timeIntervalMilliseconds;
                    if (s_cameraState.currentPhotoShootingIntervalCount < INTERVAL_PHOTOGRAPH_ALWAYS_COUNT) {
                        USER_LOG_INFO("interval taking photograph count:%d interval_time:%d.%ds",
                                      (s_cameraPhotoTimeIntervalSettings.captureCount -
                                       s_cameraState.currentPhotoShootingIntervalCount + 1),
                                      s_cameraPhotoTimeIntervalSettings.timeIntervalSeconds,
                                      s_cameraPhotoTimeIntervalSettings.timeIntervalMilliseconds / 100);
                        s_cameraState.currentPhotoShootingIntervalCount--;
                        if (s_cameraState.currentPhotoShootingIntervalCount == 0) {
                            s_cameraState.shootingState = DJI_CAMERA_SHOOTING_PHOTO_IDLE;
                            s_cameraState.isStoring = false;
                            s_cameraState.isShootingIntervalStart = false;
                        }
                    } else {
                        USER_LOG_INFO("interval taking photograph always, interval_time:%d.%ds",
                                      s_cameraPhotoTimeIntervalSettings.timeIntervalSeconds,
                                      s_cameraPhotoTimeIntervalSettings.timeIntervalMilliseconds / 100);
                        s_cameraState.currentPhotoShootingIntervalCount--;
                    }
                }
            }

            returnCode = osalHandler->MutexUnlock(s_commonMutex);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("unlock mutex error: 0x%08llX.", returnCode);
                continue;
            }
        }
    }
}

#ifndef __CC_ARM
#pragma GCC diagnostic pop
#endif

/* Private functions definition-----------------------------------------------*/
T_DjiReturnCode DjiTest_CameraEmuBaseStartService(void)
{
    T_DjiReturnCode returnCode;
    char ipAddr[DJI_IP_ADDR_STR_SIZE_MAX] = {0};
    uint16_t port = 0;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    T_DjiAircraftInfoBaseInfo aircraftInfoBaseInfo = {0};

    returnCode = osalHandler->MutexCreate(&s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("create mutex used to lock tap zoom arguments error: 0x%08llX", returnCode);
        return returnCode;
    }

    returnCode = osalHandler->MutexCreate(&s_zoomMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("create mutex used to lock tap zoom arguments error: 0x%08llX", returnCode);
        return returnCode;
    }

    returnCode = osalHandler->MutexCreate(&s_tapZoomMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("create mutex used to lock tap zoom arguments error: 0x%08llX", returnCode);
        return returnCode;
    }

    returnCode = DjiPayloadCamera_Init();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("init payload camera error:0x%08llX", returnCode);
        return returnCode;
    }

    /* Init the SDcard parameters */
    s_cameraSDCardState.isInserted = true;
    s_cameraSDCardState.isVerified = true;
    s_cameraSDCardState.totalSpaceInMB = SDCARD_TOTAL_SPACE_IN_MB;
    s_cameraSDCardState.remainSpaceInMB = SDCARD_TOTAL_SPACE_IN_MB;
    s_cameraSDCardState.availableCaptureCount = SDCARD_TOTAL_SPACE_IN_MB / SDCARD_PER_PHOTO_SPACE_IN_MB;
    s_cameraSDCardState.availableRecordingTimeInSeconds =
        SDCARD_TOTAL_SPACE_IN_MB / SDCARD_PER_SECONDS_RECORD_SPACE_IN_MB;

    /* Register the camera common handler */
    s_commonHandler.GetSystemState = GetSystemState;
    s_commonHandler.SetMode = SetMode;
    s_commonHandler.GetMode = DjiTest_CameraGetMode;
    s_commonHandler.StartRecordVideo = StartRecordVideo;
    s_commonHandler.StopRecordVideo = StopRecordVideo;
    s_commonHandler.StartShootPhoto = StartShootPhoto;
    s_commonHandler.StopShootPhoto = StopShootPhoto;
    s_commonHandler.SetShootPhotoMode = SetShootPhotoMode;
    s_commonHandler.GetShootPhotoMode = GetShootPhotoMode;
    s_commonHandler.SetPhotoBurstCount = SetPhotoBurstCount;
    s_commonHandler.GetPhotoBurstCount = GetPhotoBurstCount;
    s_commonHandler.SetPhotoTimeIntervalSettings = SetPhotoTimeIntervalSettings;
    s_commonHandler.GetPhotoTimeIntervalSettings = GetPhotoTimeIntervalSettings;
    s_commonHandler.GetSDCardState = GetSDCardState;
    s_commonHandler.FormatSDCard = FormatSDCard;

    returnCode = DjiPayloadCamera_RegCommonHandler(&s_commonHandler);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("camera register common handler error:0x%08llX", returnCode);
    }

    /* Register the camera metering handler */
#if USER_CAMERA_EMU_METERING_ON
    s_exposureMeteringHandler.SetMeteringMode = SetMeteringMode;
    s_exposureMeteringHandler.GetMeteringMode = GetMeteringMode;
    s_exposureMeteringHandler.SetSpotMeteringTarget = SetSpotMeteringTarget;
    s_exposureMeteringHandler.GetSpotMeteringTarget = GetSpotMeteringTarget;

    returnCode = DjiPayloadCamera_RegExposureMeteringHandler(&s_exposureMeteringHandler);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("camera register exposure metering handler error:0x%08llX", returnCode);
        return returnCode;
    }
#endif

    /* Register the camera focus handler */
#if USER_CAMERA_EMU_FOCUS_ON
    s_focusHandler.SetFocusMode = SetFocusMode;
    s_focusHandler.GetFocusMode = GetFocusMode;
    s_focusHandler.SetFocusTarget = SetFocusTarget;
    s_focusHandler.GetFocusTarget = GetFocusTarget;
    s_focusHandler.SetFocusAssistantSettings = SetFocusAssistantSettings;
    s_focusHandler.GetFocusAssistantSettings = GetFocusAssistantSettings;
    s_focusHandler.SetFocusRingValue = SetFocusRingValue;
    s_focusHandler.GetFocusRingValue = GetFocusRingValue;
    s_focusHandler.GetFocusRingValueUpperBound = GetFocusRingValueUpperBound;

    returnCode = DjiPayloadCamera_RegFocusHandler(&s_focusHandler);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("camera register adjustable focal point handler error:0x%08llX", returnCode);
        return returnCode;
    }
#endif

    /* Register the camera digital zoom handler */
#if USER_CAMERA_EMU_HYBRID_ZOOM_ON
    s_digitalZoomHandler.SetDigitalZoomFactor = SetDigitalZoomFactor;
    s_digitalZoomHandler.GetDigitalZoomFactor = DjiTest_CameraGetDigitalZoomFactor;

    returnCode = DjiPayloadCamera_RegDigitalZoomHandler(&s_digitalZoomHandler);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("camera register digital zoom handler error:0x%08llX", returnCode);
        return returnCode;
    }

    /* Register the camera optical zoom handler */
    s_opticalZoomHandler.SetOpticalZoomFocalLength = SetOpticalZoomFocalLength;
    s_opticalZoomHandler.GetOpticalZoomFocalLength = GetOpticalZoomFocalLength;
    s_opticalZoomHandler.GetOpticalZoomFactor = DjiTest_CameraGetOpticalZoomFactor;
    s_opticalZoomHandler.GetOpticalZoomSpec = GetOpticalZoomSpec;
    s_opticalZoomHandler.StartContinuousOpticalZoom = StartContinuousOpticalZoom;
    s_opticalZoomHandler.StopContinuousOpticalZoom = StopContinuousOpticalZoom;

    returnCode = DjiPayloadCamera_RegOpticalZoomHandler(&s_opticalZoomHandler);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("camera register optical zoom handler error:0x%08llX", returnCode);
        return returnCode;
    }
#endif

    /* Register the camera tap zoom handler */
#if USER_CAMERA_EMU_TAP_ZOOM_ON
    s_tapZoomHandler.GetTapZoomState = GetTapZoomState;
    s_tapZoomHandler.SetTapZoomEnabled = SetTapZoomEnabled;
    s_tapZoomHandler.GetTapZoomEnabled = GetTapZoomEnabled;
    s_tapZoomHandler.SetTapZoomMultiplier = SetTapZoomMultiplier;
    s_tapZoomHandler.GetTapZoomMultiplier = GetTapZoomMultiplier;
    s_tapZoomHandler.TapZoomAtTarget = TapZoomAtTarget;

    returnCode = DjiPayloadCamera_RegTapZoomHandler(&s_tapZoomHandler);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("camera register tap zoom handler error:0x%08llX", returnCode);
        return returnCode;
    }
#endif

    /* Create the camera emu taskHandle */
    if (osalHandler->TaskCreate("user_camera_task", UserCamera_Task,
                                PAYLOAD_CAMERA_EMU_TASK_STACK_SIZE, NULL, &s_userCameraThread)
        != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("user camera taskHandle create error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    if (DjiAircraftInfo_GetBaseInfo(&aircraftInfoBaseInfo) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("get aircraft information error.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    returnCode = DjiPayloadCamera_SetVideoStreamType(DJI_CAMERA_VIDEO_STREAM_TYPE_H264_DJI_FORMAT);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("DJI camera set video stream error.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }
    s_cameraVideoStreamType = DJI_CAMERA_VIDEO_STREAM_TYPE_H264_DJI_FORMAT;

    returnCode = DjiPayloadCamera_GetVideoStreamRemoteAddress(ipAddr, &port);
    if (returnCode == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_DEBUG("Get video stream remote address: %s, port: %d", ipAddr, port);
    } else {
        USER_LOG_ERROR("get video stream remote address error.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    s_isCamInited = true;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiTest_CameraGetDigitalZoomFactor(dji_f32_t *factor)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    returnCode = osalHandler->MutexLock(s_zoomMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("lock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    *factor = s_cameraDigitalZoomFactor;

    returnCode = osalHandler->MutexUnlock(s_zoomMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("unlock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiTest_CameraGetOpticalZoomFactor(dji_f32_t *factor)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    returnCode = osalHandler->MutexLock(s_zoomMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("lock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    //Formula:factor = currentFocalLength / minFocalLength
    *factor = (dji_f32_t) s_cameraOpticalZoomFocalLength / ZOOM_OPTICAL_FOCAL_MIN_LENGTH;

    returnCode = osalHandler->MutexUnlock(s_zoomMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("unlock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiTest_CameraGetMode(E_DjiCameraMode *mode)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    returnCode = osalHandler->MutexLock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("lock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    *mode = s_cameraState.cameraMode;

    returnCode = osalHandler->MutexUnlock(s_commonMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("unlock mutex error: 0x%08llX.", returnCode);
        return returnCode;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiTest_CameraGetVideoStreamType(E_DjiCameraVideoStreamType *type)
{
    if (s_isCamInited == false) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT_IN_CURRENT_STATE;
    }

    *type = s_cameraVideoStreamType;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

bool DjiTest_CameraIsInited(void)
{
    return s_isCamInited;
}

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
