/**
 ********************************************************************
 * @file    test_payload_cam_emu.c
 * @version V2.0.0
 * @date    2019/07/01
 * @brief
 *
 * @copyright (c) 2018-2019 DJI. All rights reserved.
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
#include "test_payload_cam_emu.h"
#include <psdk_logger.h>
#include <psdk_gimbal.h>
#include <utils/util_file.h>
#include "psdk_payload_camera.h"
#include "psdk_platform.h"
#include "utils/util_misc.h"
#include "psdk_aircraft_info.h"
#include "psdk_xport.h"
#include "gimbal_emu/test_payload_gimbal_emu.h"
#include "camera_media_emu/test_payload_cam_media.h"

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
#define PHOTO_FILE_PATH                         "../../../../../api_sample/camera_media_emu/media_file/PSDK_0001_ORG.jpg"

/* Private types -------------------------------------------------------------*/
typedef struct {
    E_PsdkGimbalRotationMode rotationMode;
    T_PsdkGimbalRotationProperty rotationProperty;
    T_PsdkAttitude3d rotationValue;
} T_TestCameraGimbalRotationArgument;

/* Private variables ---------------------------------------------------------*/
static bool s_isCamInited = false;

static T_PsdkCameraCommonHandler s_commonHandler;
static T_PsdkCameraExposureMeteringHandler s_exposureMeteringHandler;
static T_PsdkCameraFocusHandler s_focusHandler;
static T_PsdkCameraDigitalZoomHandler s_digitalZoomHandler;
static T_PsdkCameraOpticalZoomHandler s_opticalZoomHandler;
static T_PsdkCameraTapZoomHandler s_tapZoomHandler;

static T_PsdkTaskHandle s_userCameraThread;

static T_PsdkCameraSystemState s_cameraState = {0};
static E_PsdkCameraShootPhotoMode s_cameraShootPhotoMode = PSDK_CAMERA_SHOOT_PHOTO_MODE_SINGLE;
static E_PsdkCameraBurstCount s_cameraBurstCount = PSDK_CAMERA_BURST_COUNT_2;
static T_PsdkCameraPhotoTimeIntervalSettings s_cameraPhotoTimeIntervalSettings = {INTERVAL_PHOTOGRAPH_ALWAYS_COUNT,
                                                                                  INTERVAL_PHOTOGRAPH_INTERVAL_INIT_VALUE};
static T_PsdkCameraSDCardState s_cameraSDCardState = {0};
static T_PsdkMutexHandle s_commonMutex = {0};

static E_PsdkCameraMeteringMode s_cameraMeteringMode = PSDK_CAMERA_METERING_MODE_CENTER;
static T_PsdkCameraSpotMeteringTarget s_cameraSpotMeteringTarget = {0};

static E_PsdkCameraFocusMode s_cameraFocusMode = PSDK_CAMERA_FOCUS_MODE_AUTO;
static T_PsdkCameraPointInScreen s_cameraFocusTarget = {0};
static uint32_t s_cameraFocusRingValue = FOCUS_MID_RINGVALUE;
static T_PsdkCameraFocusAssistantSettings s_cameraFocusAssistantSettings = {0};

static E_PsdkCameraZoomDirection s_cameraZoomDirection = PSDK_CAMERA_ZOOM_DIRECTION_OUT;
static E_PsdkCameraZoomSpeed s_cameraZoomSpeed = PSDK_CAMERA_ZOOM_SPEED_NORMAL;
static uint32_t s_cameraOpticalZoomFocalLength = ZOOM_OPTICAL_FOCAL_MIN_LENGTH;
static psdk_f32_t s_cameraDigitalZoomFactor = ZOOM_DIGITAL_BASE_FACTOR;
static bool s_isStartContinuousOpticalZoom = false;
static bool s_isOpticalZoomReachLimit = false;
static T_PsdkMutexHandle s_zoomMutex = {0};

static bool s_isTapZoomEnabled = false;
static T_PsdkCameraTapZoomState s_cameraTapZoomState = {0};
static uint8_t s_tapZoomMultiplier = 1;
static uint32_t s_tapZoomStartTime = 0;
static bool s_isStartTapZoom = false;
static bool s_isTapZooming = false;
static T_TestCameraGimbalRotationArgument s_tapZoomNewestGimbalRotationArgument = {0};
static uint32_t s_tapZoomNewestTargetHybridFocalLength = 0; // unit: 0.1mm
static T_PsdkMutexHandle s_tapZoomMutex = NULL;
static E_PsdkCameraVideoStreamType s_cameraVideoStreamType;

/* Private functions declaration ---------------------------------------------*/
static T_PsdkReturnCode GetSystemState(T_PsdkCameraSystemState *systemState);
static T_PsdkReturnCode SetMode(E_PsdkCameraMode mode);
static T_PsdkReturnCode StartRecordVideo(void);
static T_PsdkReturnCode StopRecordVideo(void);
static T_PsdkReturnCode StartShootPhoto(void);
static T_PsdkReturnCode StopShootPhoto(void);
static T_PsdkReturnCode SetShootPhotoMode(E_PsdkCameraShootPhotoMode mode);
static T_PsdkReturnCode GetShootPhotoMode(E_PsdkCameraShootPhotoMode *mode);
static T_PsdkReturnCode SetPhotoBurstCount(E_PsdkCameraBurstCount burstCount);
static T_PsdkReturnCode GetPhotoBurstCount(E_PsdkCameraBurstCount *burstCount);
static T_PsdkReturnCode SetPhotoTimeIntervalSettings(T_PsdkCameraPhotoTimeIntervalSettings settings);
static T_PsdkReturnCode GetPhotoTimeIntervalSettings(T_PsdkCameraPhotoTimeIntervalSettings *settings);
static T_PsdkReturnCode GetSDCardState(T_PsdkCameraSDCardState *sdCardState);
static T_PsdkReturnCode FormatSDCard(void);

static T_PsdkReturnCode SetMeteringMode(E_PsdkCameraMeteringMode mode);
static T_PsdkReturnCode GetMeteringMode(E_PsdkCameraMeteringMode *mode);
static T_PsdkReturnCode SetSpotMeteringTarget(T_PsdkCameraSpotMeteringTarget target);
static T_PsdkReturnCode GetSpotMeteringTarget(T_PsdkCameraSpotMeteringTarget *target);

static T_PsdkReturnCode SetFocusMode(E_PsdkCameraFocusMode mode);
static T_PsdkReturnCode GetFocusMode(E_PsdkCameraFocusMode *mode);
static T_PsdkReturnCode SetFocusTarget(T_PsdkCameraPointInScreen target);
static T_PsdkReturnCode GetFocusTarget(T_PsdkCameraPointInScreen *target);
static T_PsdkReturnCode SetFocusAssistantSettings(T_PsdkCameraFocusAssistantSettings settings);
static T_PsdkReturnCode GetFocusAssistantSettings(T_PsdkCameraFocusAssistantSettings *settings);
static T_PsdkReturnCode SetFocusRingValue(uint32_t value);
static T_PsdkReturnCode GetFocusRingValue(uint32_t *value);
static T_PsdkReturnCode GetFocusRingValueUpperBound(uint32_t *value);

static T_PsdkReturnCode SetDigitalZoomFactor(psdk_f32_t factor);
static T_PsdkReturnCode SetOpticalZoomFocalLength(uint32_t focalLength);
static T_PsdkReturnCode GetOpticalZoomFocalLength(uint32_t *focalLength);
static T_PsdkReturnCode GetOpticalZoomSpec(T_PsdkCameraOpticalZoomSpec *spec);
static T_PsdkReturnCode StartContinuousOpticalZoom(E_PsdkCameraZoomDirection direction, E_PsdkCameraZoomSpeed speed);
static T_PsdkReturnCode StopContinuousOpticalZoom(void);
static T_PsdkReturnCode GetTapZoomState(T_PsdkCameraTapZoomState *state);
static T_PsdkReturnCode SetTapZoomEnabled(bool enabledFlag);
static T_PsdkReturnCode GetTapZoomEnabled(bool *enabledFlag);
static T_PsdkReturnCode SetTapZoomMultiplier(uint8_t multiplier);
static T_PsdkReturnCode GetTapZoomMultiplier(uint8_t *multiplier);
static T_PsdkReturnCode TapZoomAtTarget(T_PsdkCameraPointInScreen target);
static T_PsdkReturnCode PsdkTest_CameraHybridZoom(uint32_t focalLength);
static T_PsdkReturnCode PsdkTest_CameraRotationGimbal(T_TestCameraGimbalRotationArgument gimbalRotationArgument);

static void *UserCamera_Task(void *arg);

/* Exported functions definition ---------------------------------------------*/

/* Private functions definition-----------------------------------------------*/
static T_PsdkReturnCode GetSystemState(T_PsdkCameraSystemState *systemState)
{
    T_PsdkReturnCode psdkStat;

    psdkStat = PsdkOsal_MutexLock(s_commonMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("lock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    *systemState = s_cameraState;

    psdkStat = PsdkOsal_MutexUnlock(s_commonMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("unlock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode SetMode(E_PsdkCameraMode mode)
{
    T_PsdkReturnCode psdkStat;

    psdkStat = PsdkOsal_MutexLock(s_commonMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("lock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    s_cameraState.cameraMode = mode;
    PsdkLogger_UserLogInfo("set camera mode:%d", mode);

    psdkStat = PsdkOsal_MutexUnlock(s_commonMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("unlock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode StartRecordVideo(void)
{
    T_PsdkReturnCode psdkStat;
    T_PsdkReturnCode returnCode = PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;

    psdkStat = PsdkOsal_MutexLock(s_commonMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("lock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    if (s_cameraState.isRecording != false) {
        PsdkLogger_UserLogError("camera is already in recording state");
        returnCode = PSDK_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT_IN_CURRENT_STATE;
        goto out;
    }

    s_cameraState.isRecording = true;
    PsdkLogger_UserLogInfo("start record video");

out:
    psdkStat = PsdkOsal_MutexUnlock(s_commonMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("unlock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    return returnCode;
}

static T_PsdkReturnCode StopRecordVideo(void)
{
    T_PsdkReturnCode psdkStat;
    T_PsdkReturnCode returnCode = PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;

    psdkStat = PsdkOsal_MutexLock(s_commonMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("lock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    if (s_cameraState.isRecording != true) {
        PsdkLogger_UserLogError("camera is not in recording state");
        returnCode = PSDK_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT_IN_CURRENT_STATE;
        goto out;
    }

    s_cameraState.isRecording = false;
    s_cameraState.currentVideoRecordingTimeInSeconds = 0;
    PsdkLogger_UserLogInfo("stop record video");

out:
    psdkStat = PsdkOsal_MutexUnlock(s_commonMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("unlock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    return returnCode;
}

static T_PsdkReturnCode StartShootPhoto(void)
{
    T_PsdkReturnCode psdkStat;

    psdkStat = PsdkOsal_MutexLock(s_commonMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("lock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    PsdkLogger_UserLogInfo("start shoot photo");
    s_cameraState.isStoring = true;

    if (s_cameraShootPhotoMode == PSDK_CAMERA_SHOOT_PHOTO_MODE_SINGLE) {
        s_cameraState.shootingState = PSDK_CAMERA_SHOOTING_SINGLE_PHOTO;
    } else if (s_cameraShootPhotoMode == PSDK_CAMERA_SHOOT_PHOTO_MODE_BURST) {
        s_cameraState.shootingState = PSDK_CAMERA_SHOOTING_BURST_PHOTO;
    } else if (s_cameraShootPhotoMode == PSDK_CAMERA_SHOOT_PHOTO_MODE_INTERVAL) {
        s_cameraState.shootingState = PSDK_CAMERA_SHOOTING_INTERVAL_PHOTO;
        s_cameraState.isShootingIntervalStart = true;
        s_cameraState.currentPhotoShootingIntervalTimeInSeconds = s_cameraPhotoTimeIntervalSettings.timeIntervalSeconds;
    }

    psdkStat = PsdkOsal_MutexUnlock(s_commonMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("unlock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode StopShootPhoto(void)
{
    T_PsdkReturnCode psdkStat;

    psdkStat = PsdkOsal_MutexLock(s_commonMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("lock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    PsdkLogger_UserLogInfo("stop shoot photo");
    s_cameraState.shootingState = PSDK_CAMERA_SHOOTING_PHOTO_IDLE;
    s_cameraState.isStoring = false;
    s_cameraState.isShootingIntervalStart = false;

    psdkStat = PsdkOsal_MutexUnlock(s_commonMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("unlock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode SetShootPhotoMode(E_PsdkCameraShootPhotoMode mode)
{
    T_PsdkReturnCode psdkStat;

    psdkStat = PsdkOsal_MutexLock(s_commonMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("lock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    s_cameraShootPhotoMode = mode;
    PsdkLogger_UserLogInfo("set shoot photo mode:%d", mode);

    psdkStat = PsdkOsal_MutexUnlock(s_commonMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("unlock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode GetShootPhotoMode(E_PsdkCameraShootPhotoMode *mode)
{
    T_PsdkReturnCode psdkStat;

    psdkStat = PsdkOsal_MutexLock(s_commonMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("lock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    *mode = s_cameraShootPhotoMode;

    psdkStat = PsdkOsal_MutexUnlock(s_commonMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("unlock mutex error: 0x%08llX.", psdkStat);\
        return psdkStat;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode SetPhotoBurstCount(E_PsdkCameraBurstCount burstCount)
{
    T_PsdkReturnCode psdkStat;

    psdkStat = PsdkOsal_MutexLock(s_commonMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("lock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    s_cameraBurstCount = burstCount;
    PsdkLogger_UserLogInfo("set photo burst count:%d", burstCount);

    psdkStat = PsdkOsal_MutexUnlock(s_commonMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("unlock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode GetPhotoBurstCount(E_PsdkCameraBurstCount *burstCount)
{
    T_PsdkReturnCode psdkStat;

    psdkStat = PsdkOsal_MutexLock(s_commonMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("lock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    *burstCount = s_cameraBurstCount;

    psdkStat = PsdkOsal_MutexUnlock(s_commonMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("unlock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode SetPhotoTimeIntervalSettings(T_PsdkCameraPhotoTimeIntervalSettings settings)
{
    T_PsdkReturnCode psdkStat;

    psdkStat = PsdkOsal_MutexLock(s_commonMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("lock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    s_cameraPhotoTimeIntervalSettings.captureCount = settings.captureCount;
    s_cameraPhotoTimeIntervalSettings.timeIntervalSeconds = settings.timeIntervalSeconds;
    PsdkLogger_UserLogInfo("set photo interval settings count:%d seconds:%d", settings.captureCount,
                           settings.timeIntervalSeconds);
    s_cameraState.currentPhotoShootingIntervalCount = settings.captureCount;

    psdkStat = PsdkOsal_MutexUnlock(s_commonMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("unlock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode GetPhotoTimeIntervalSettings(T_PsdkCameraPhotoTimeIntervalSettings *settings)
{
    T_PsdkReturnCode psdkStat;

    psdkStat = PsdkOsal_MutexLock(s_commonMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("lock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    memcpy(settings, &s_cameraPhotoTimeIntervalSettings, sizeof(T_PsdkCameraPhotoTimeIntervalSettings));

    psdkStat = PsdkOsal_MutexUnlock(s_commonMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("unlock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode GetSDCardState(T_PsdkCameraSDCardState *sdCardState)
{
    T_PsdkReturnCode psdkStat;

    psdkStat = PsdkOsal_MutexLock(s_commonMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("lock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    memcpy(sdCardState, &s_cameraSDCardState, sizeof(T_PsdkCameraSDCardState));

    psdkStat = PsdkOsal_MutexUnlock(s_commonMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("unlock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode FormatSDCard(void)
{
    T_PsdkReturnCode psdkStat;

    psdkStat = PsdkOsal_MutexLock(s_commonMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("lock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    PsdkLogger_UserLogInfo("format sdcard");

    memset(&s_cameraSDCardState, 0, sizeof(T_PsdkCameraSDCardState));
    s_cameraSDCardState.isInserted = true;
    s_cameraSDCardState.totalSpaceInMB = SDCARD_TOTAL_SPACE_IN_MB;
    s_cameraSDCardState.remainSpaceInMB = SDCARD_TOTAL_SPACE_IN_MB;
    s_cameraSDCardState.availableCaptureCount = SDCARD_TOTAL_SPACE_IN_MB / SDCARD_PER_PHOTO_SPACE_IN_MB;
    s_cameraSDCardState.availableRecordingTimeInSeconds =
        SDCARD_TOTAL_SPACE_IN_MB / SDCARD_PER_SECONDS_RECORD_SPACE_IN_MB;

    psdkStat = PsdkOsal_MutexUnlock(s_commonMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("unlock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode SetMeteringMode(E_PsdkCameraMeteringMode mode)
{
    PsdkLogger_UserLogInfo("set metering mode:%d", mode);
    s_cameraMeteringMode = mode;

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode GetMeteringMode(E_PsdkCameraMeteringMode *mode)
{
    *mode = s_cameraMeteringMode;

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode SetSpotMeteringTarget(T_PsdkCameraSpotMeteringTarget target)
{
    PsdkLogger_UserLogInfo("set spot metering area col:%d row:%d", target.col, target.row);
    memcpy(&s_cameraSpotMeteringTarget, &target, sizeof(T_PsdkCameraSpotMeteringTarget));

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode GetSpotMeteringTarget(T_PsdkCameraSpotMeteringTarget *target)
{
    memcpy(target, &s_cameraSpotMeteringTarget, sizeof(T_PsdkCameraSpotMeteringTarget));

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode SetFocusMode(E_PsdkCameraFocusMode mode)
{
    PsdkLogger_UserLogInfo("set focus mode:%d", mode);
    s_cameraFocusMode = mode;

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode GetFocusMode(E_PsdkCameraFocusMode *mode)
{
    *mode = s_cameraFocusMode;

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode SetFocusTarget(T_PsdkCameraPointInScreen target)
{
    PsdkLogger_UserLogInfo("set focus target x:%.2f y:%.2f", target.focusX, target.focusY);
    memcpy(&s_cameraFocusTarget, &target, sizeof(T_PsdkCameraPointInScreen));

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode GetFocusTarget(T_PsdkCameraPointInScreen *target)
{
    memcpy(target, &s_cameraFocusTarget, sizeof(T_PsdkCameraPointInScreen));

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode SetFocusAssistantSettings(T_PsdkCameraFocusAssistantSettings settings)
{
    PsdkLogger_UserLogInfo("set focus assistant setting MF:%d AF:%d", settings.isEnabledMF, settings.isEnabledAF);
    memcpy(&s_cameraFocusAssistantSettings, &settings, sizeof(T_PsdkCameraFocusAssistantSettings));

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode GetFocusAssistantSettings(T_PsdkCameraFocusAssistantSettings *settings)
{
    memcpy(settings, &s_cameraFocusAssistantSettings, sizeof(T_PsdkCameraFocusAssistantSettings));

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode SetFocusRingValue(uint32_t value)
{
    PsdkLogger_UserLogInfo("set focus ring value:%d", value);
    s_cameraFocusRingValue = value;

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode GetFocusRingValue(uint32_t *value)
{
    *value = s_cameraFocusRingValue;

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode GetFocusRingValueUpperBound(uint32_t *value)
{
    *value = FOCUS_MAX_RINGVALUE;

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode SetDigitalZoomFactor(psdk_f32_t factor)
{
    T_PsdkReturnCode psdkStat;

    psdkStat = PsdkOsal_MutexLock(s_zoomMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("lock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    PsdkLogger_UserLogInfo("set digital zoom factor:%.2f", factor);
    s_cameraDigitalZoomFactor = factor;

    psdkStat = PsdkOsal_MutexUnlock(s_zoomMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("unlock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode SetOpticalZoomFocalLength(uint32_t focalLength)
{
    T_PsdkReturnCode psdkStat;

    psdkStat = PsdkOsal_MutexLock(s_zoomMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("lock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    PsdkLogger_UserLogInfo("set optical zoom focal length:%d", focalLength);
    s_isOpticalZoomReachLimit = false;
    s_cameraDigitalZoomFactor = ZOOM_DIGITAL_BASE_FACTOR;
    s_cameraOpticalZoomFocalLength = ZOOM_OPTICAL_FOCAL_MIN_LENGTH;

    psdkStat = PsdkOsal_MutexUnlock(s_zoomMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("unlock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode GetOpticalZoomFocalLength(uint32_t *focalLength)
{
    T_PsdkReturnCode psdkStat;

    psdkStat = PsdkOsal_MutexLock(s_zoomMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("lock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    *focalLength = s_cameraOpticalZoomFocalLength;

    psdkStat = PsdkOsal_MutexUnlock(s_zoomMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("unlock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode GetOpticalZoomSpec(T_PsdkCameraOpticalZoomSpec *spec)
{
    spec->maxFocalLength = ZOOM_OPTICAL_FOCAL_MAX_LENGTH;
    spec->minFocalLength = ZOOM_OPTICAL_FOCAL_MIN_LENGTH;
    spec->focalLengthStep = ZOOM_OPTICAL_FOCAL_LENGTH_STEP;

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode StartContinuousOpticalZoom(E_PsdkCameraZoomDirection direction, E_PsdkCameraZoomSpeed speed)
{
    T_PsdkReturnCode psdkStat;

    psdkStat = PsdkOsal_MutexLock(s_zoomMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("lock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    PsdkLogger_UserLogInfo("start continuous optical zoom direction:%d speed:%d", direction, speed);
    s_isStartContinuousOpticalZoom = true;
    s_cameraZoomDirection = direction;
    s_cameraZoomSpeed = speed;

    psdkStat = PsdkOsal_MutexUnlock(s_zoomMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("unlock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode StopContinuousOpticalZoom(void)
{
    T_PsdkReturnCode psdkStat;

    psdkStat = PsdkOsal_MutexLock(s_zoomMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("lock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    PsdkLogger_UserLogInfo("stop continuous optical zoom");
    s_isStartContinuousOpticalZoom = false;
    s_cameraZoomDirection = PSDK_CAMERA_ZOOM_DIRECTION_OUT;
    s_cameraZoomSpeed = PSDK_CAMERA_ZOOM_SPEED_NORMAL;

    psdkStat = PsdkOsal_MutexUnlock(s_zoomMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("unlock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode GetTapZoomState(T_PsdkCameraTapZoomState *state)
{
    T_PsdkReturnCode psdkStat;

    psdkStat = PsdkOsal_MutexLock(s_tapZoomMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("lock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    memcpy(state, &s_cameraTapZoomState, sizeof(T_PsdkCameraTapZoomState));

    psdkStat = PsdkOsal_MutexUnlock(s_tapZoomMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("unlock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode SetTapZoomEnabled(bool enabledFlag)
{
    PsdkLogger_UserLogInfo("set tap zoom enabled flag: %d.", enabledFlag);
    s_isTapZoomEnabled = enabledFlag;

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode GetTapZoomEnabled(bool *enabledFlag)
{
    *enabledFlag = s_isTapZoomEnabled;

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode SetTapZoomMultiplier(uint8_t multiplier)
{
    PsdkLogger_UserLogInfo("set tap zoom multiplier: %d.", multiplier);
    s_tapZoomMultiplier = multiplier;

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode GetTapZoomMultiplier(uint8_t *multiplier)
{
    *multiplier = s_tapZoomMultiplier;

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode TapZoomAtTarget(T_PsdkCameraPointInScreen target)
{
    T_PsdkReturnCode psdkStat;
    E_PsdkGimbalRotationMode rotationMode;
    T_PsdkGimbalRotationProperty rotationProperty = {0};
    T_PsdkAttitude3d rotationValue = {0};
    float hybridFocalLength = 0; // unit: 0.1mm

    PsdkLogger_UserLogInfo("tap zoom at target: x %f, y %f.", target.focusX, target.focusY);

    if (s_isTapZoomEnabled != true) {
        PsdkLogger_UserLogWarn("tap zoom is not enabled.");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    if (s_isTapZooming || s_isStartTapZoom) {
        PsdkLogger_UserLogWarn("The last tap zoom process is not over.");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT_IN_CURRENT_STATE;
    }

    rotationMode = PSDK_GIMBAL_ROTATION_MODE_RELATIVE_ANGLE;
    rotationProperty.relativeAngleRotation.actionTime = TAP_ZOOM_DURATION / 10;

    psdkStat = PsdkOsal_MutexLock(s_zoomMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("lock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    /* Calculation formula: rotation angle = arctan((coordinate of target in sensor - coordinate of center point in
     * sensor) / hybrid focal length). Here, suppose that images of all pixels of sensor are displayed in screen,
     * and that center of the image sensor coincides with center of rotation of the gimbal, and that optical axis of
     * camera coincides with x-axis of gimbal. */
    hybridFocalLength = s_cameraOpticalZoomFocalLength * s_cameraDigitalZoomFactor;

    psdkStat = PsdkOsal_MutexUnlock(s_zoomMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("unlock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    rotationValue.pitch = (int32_t) (
        atan2f((target.focusY - CENTER_POINT_IN_SCREEN_Y_VALUE) * IMAGE_SENSOR_Y_SIZE, hybridFocalLength) * 1800 /
        PI);
    rotationValue.yaw = (int32_t) (
        atan2f((target.focusX - CENTER_POINT_IN_SCREEN_X_VALUE) * IMAGE_SENSOR_X_SIZE, hybridFocalLength) * 1800 /
        PI);

    psdkStat = PsdkOsal_MutexLock(s_tapZoomMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("lock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    s_tapZoomNewestGimbalRotationArgument.rotationMode = rotationMode;
    s_tapZoomNewestGimbalRotationArgument.rotationProperty = rotationProperty;
    s_tapZoomNewestGimbalRotationArgument.rotationValue = rotationValue;
    s_tapZoomNewestTargetHybridFocalLength = (uint32_t) (hybridFocalLength * (float) s_tapZoomMultiplier);

    psdkStat = PsdkOsal_MutexUnlock(s_tapZoomMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("unlock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    s_isStartTapZoom = true;

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode PsdkTest_CameraHybridZoom(uint32_t focalLength)
{
    if (focalLength >= ZOOM_OPTICAL_FOCAL_MIN_LENGTH && focalLength <= ZOOM_OPTICAL_FOCAL_MAX_LENGTH) {
        s_cameraOpticalZoomFocalLength = focalLength;
        s_cameraDigitalZoomFactor = ZOOM_DIGITAL_BASE_FACTOR;
    } else if (focalLength < ZOOM_OPTICAL_FOCAL_MIN_LENGTH) {
        s_cameraOpticalZoomFocalLength = ZOOM_OPTICAL_FOCAL_MIN_LENGTH;
        s_cameraDigitalZoomFactor = ZOOM_DIGITAL_BASE_FACTOR;

        PsdkLogger_UserLogError("focal length %d is smaller than min optical focal length %d.", focalLength,
                                ZOOM_OPTICAL_FOCAL_MIN_LENGTH);
        return PSDK_ERROR_SYSTEM_MODULE_CODE_OUT_OF_RANGE;
    } else {
        s_cameraOpticalZoomFocalLength = ZOOM_OPTICAL_FOCAL_MAX_LENGTH;
        s_cameraDigitalZoomFactor = (float) focalLength / ZOOM_OPTICAL_FOCAL_MAX_LENGTH;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode PsdkTest_CameraRotationGimbal(T_TestCameraGimbalRotationArgument gimbalRotationArgument)
{
    T_PsdkReturnCode psdkStat;
    T_PsdkAircraftInfoBaseInfo aircraftBaseInfo = {0};

    psdkStat = PsdkAircraftInfo_GetBaseInfo(&aircraftBaseInfo);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("get aircraft information error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    if (aircraftBaseInfo.psdkAdapterType == PSDK_AIRCRAFT_INFO_PSDK_ADAPTER_TYPE_SKYPORT_V2) {
        psdkStat = PsdkTest_GimbalRotate(gimbalRotationArgument.rotationMode, gimbalRotationArgument.rotationProperty,
                                         gimbalRotationArgument.rotationValue);
        if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            PsdkLogger_UserLogError("rotate gimbal error: 0x%08llX.", psdkStat);
            return psdkStat;
        }
    } else if (aircraftBaseInfo.psdkAdapterType == PSDK_AIRCRAFT_INFO_PSDK_ADAPTER_TYPE_XPORT) {
        psdkStat = PsdkXPort_RotateSync(gimbalRotationArgument.rotationMode, gimbalRotationArgument.rotationProperty,
                                        gimbalRotationArgument.rotationValue);
        if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            PsdkLogger_UserLogError("rotate XPort error: 0x%08llX.", psdkStat);
            return psdkStat;
        }
    } else {
        PsdkLogger_UserLogError("PSDK adapter type is invalid: %d.", aircraftBaseInfo.psdkAdapterType);
        return PSDK_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
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
    T_PsdkReturnCode psdkStat;
    int32_t tempFocalLength = 0;
    psdk_f64_t tempDigitalFactor = 0.0f;
    uint32_t currentTime = 0;
    bool isStartIntervalPhotoAction = false;

    USER_UTIL_UNUSED(arg);

    while (1) {
        PsdkOsal_TaskSleepMs(1000 / PAYLOAD_CAMERA_EMU_TASK_FREQ);
        step++;

        psdkStat = PsdkOsal_MutexLock(s_commonMutex);
        if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            PsdkLogger_UserLogError("lock mutex error: 0x%08llX.", psdkStat);
            continue;
        }

        s_cameraSDCardState.availableRecordingTimeInSeconds =
            s_cameraSDCardState.remainSpaceInMB / SDCARD_PER_SECONDS_RECORD_SPACE_IN_MB;
        s_cameraSDCardState.availableCaptureCount = s_cameraSDCardState.remainSpaceInMB / SDCARD_PER_PHOTO_SPACE_IN_MB;

        //realize your photo shoot and storage logic here
        if (s_cameraState.shootingState != PSDK_CAMERA_SHOOTING_PHOTO_IDLE &&
            photoCnt++ > TAKING_PHOTO_SPENT_TIME_MS_EMU / (1000 / PAYLOAD_CAMERA_EMU_TASK_FREQ)) {
            photoCnt = 0;

            //store the photo after shooting finished
            if (s_cameraShootPhotoMode == PSDK_CAMERA_SHOOT_PHOTO_MODE_SINGLE) {
                s_cameraSDCardState.remainSpaceInMB =
                    s_cameraSDCardState.remainSpaceInMB - SDCARD_PER_PHOTO_SPACE_IN_MB;
                s_cameraState.isStoring = false;
                s_cameraState.shootingState = PSDK_CAMERA_SHOOTING_PHOTO_IDLE;
            } else if (s_cameraShootPhotoMode == PSDK_CAMERA_SHOOT_PHOTO_MODE_BURST) {
                s_cameraSDCardState.remainSpaceInMB =
                    s_cameraSDCardState.remainSpaceInMB - SDCARD_PER_PHOTO_SPACE_IN_MB * s_cameraBurstCount;
                s_cameraState.isStoring = false;
                s_cameraState.shootingState = PSDK_CAMERA_SHOOTING_PHOTO_IDLE;
            } else if (s_cameraShootPhotoMode == PSDK_CAMERA_SHOOT_PHOTO_MODE_INTERVAL) {
                if (isStartIntervalPhotoAction == true) {
                    s_cameraState.isStoring = false;
                    s_cameraState.shootingState = PSDK_CAMERA_SHOOTING_PHOTO_IDLE;
                    s_cameraSDCardState.remainSpaceInMB =
                        s_cameraSDCardState.remainSpaceInMB - SDCARD_PER_PHOTO_SPACE_IN_MB;
                }
            }

            //check the remain space of sdcard
            if (s_cameraSDCardState.remainSpaceInMB > SDCARD_TOTAL_SPACE_IN_MB) {
                s_cameraSDCardState.remainSpaceInMB = 0;
                s_cameraSDCardState.isFull = true;
            }

#if PSDK_ARCH_SYS_LINUX
            T_PsdkCameraMediaFileInfo mediaFileInfo = {0};

            // push added media file information
            if (s_cameraShootPhotoMode == PSDK_CAMERA_SHOOT_PHOTO_MODE_SINGLE) {
                if (UtilFile_IsFileExist(PHOTO_FILE_PATH) == true) {
                    psdkStat = PsdkTest_CameraMediaGetFileInfo(PHOTO_FILE_PATH, &mediaFileInfo);
                    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                        PsdkLogger_UserLogError("Get media file info error 0x%08llX", psdkStat);
                    }

                    psdkStat = PsdkPayloadCamera_PushAddedMediaFileInfo(PHOTO_FILE_PATH, mediaFileInfo);
                    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
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

        psdkStat = PsdkOsal_MutexUnlock(s_commonMutex);
        if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            PsdkLogger_UserLogError("unlock mutex error: 0x%08llX.", psdkStat);
            continue;
        }

        // 10Hz
        if (USER_UTIL_IS_WORK_TURN(step, 10, PAYLOAD_CAMERA_EMU_TASK_FREQ)) {
            psdkStat = PsdkOsal_MutexLock(s_zoomMutex);
            if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                PsdkLogger_UserLogError("lock mutex error: 0x%08llX.", psdkStat);
                continue;
            }

            //Add logic here for zoom camera
            if (s_isStartContinuousOpticalZoom == true) {
                tempDigitalFactor = s_cameraDigitalZoomFactor;
                tempFocalLength = s_cameraOpticalZoomFocalLength;
                if (s_isOpticalZoomReachLimit == false) {
                    if (s_cameraZoomDirection == PSDK_CAMERA_ZOOM_DIRECTION_IN) {
                        tempFocalLength += ((int) s_cameraZoomSpeed - PSDK_CAMERA_ZOOM_SPEED_SLOWEST + 1) *
                                           ZOOM_OPTICAL_FOCAL_LENGTH_CTRL_STEP;
                    } else if (s_cameraZoomDirection == PSDK_CAMERA_ZOOM_DIRECTION_OUT) {
                        tempFocalLength -= ((int) s_cameraZoomSpeed - PSDK_CAMERA_ZOOM_SPEED_SLOWEST + 1) *
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
                    if (s_cameraZoomDirection == PSDK_CAMERA_ZOOM_DIRECTION_IN) {
                        tempDigitalFactor += ZOOM_DIGITAL_STEP_FACTOR;
                    } else if (s_cameraZoomDirection == PSDK_CAMERA_ZOOM_DIRECTION_OUT) {
                        tempDigitalFactor -= ZOOM_DIGITAL_STEP_FACTOR;
                    }

                    if (tempDigitalFactor > ZOOM_DIGITAL_MAX_FACTOR) {
                        tempDigitalFactor = ZOOM_DIGITAL_MAX_FACTOR;
                    }

                    if (tempDigitalFactor < ZOOM_DIGITAL_BASE_FACTOR) {
                        s_isOpticalZoomReachLimit = false;
                        tempDigitalFactor = ZOOM_DIGITAL_BASE_FACTOR;
                    }
                }

                s_cameraOpticalZoomFocalLength = (uint16_t) tempFocalLength;
                s_cameraDigitalZoomFactor = tempDigitalFactor;
            }

            psdkStat = PsdkOsal_MutexLock(s_tapZoomMutex);
            if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                PsdkLogger_UserLogError("lock mutex error: 0x%08llX.", psdkStat);
                goto out;
            }

            if (s_isStartTapZoom) {
                s_isStartTapZoom = false;
                s_isTapZooming = true;

                psdkStat = PsdkOsal_GetTimeMs(&s_tapZoomStartTime);
                if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    PsdkLogger_UserLogError("get start time error: 0x%08llX.", psdkStat);
                }

                psdkStat = PsdkTest_CameraRotationGimbal(s_tapZoomNewestGimbalRotationArgument);
                if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
                    PsdkLogger_UserLogError("rotate gimbal error: 0x%08llX.", psdkStat);
                else
                    s_cameraTapZoomState.isGimbalMoving = true;

                psdkStat = PsdkTest_CameraHybridZoom(s_tapZoomNewestTargetHybridFocalLength);
                if (psdkStat == PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    s_cameraTapZoomState.zoomState = s_tapZoomNewestTargetHybridFocalLength >
                                                     (s_cameraOpticalZoomFocalLength * s_cameraDigitalZoomFactor)
                                                     ? PSDK_CAMERA_TAP_ZOOM_STATE_ZOOM_IN
                                                     : PSDK_CAMERA_TAP_ZOOM_STATE_ZOOM_OUT;
                } else if (psdkStat == PSDK_ERROR_SYSTEM_MODULE_CODE_OUT_OF_RANGE) {
                    PsdkLogger_UserLogError("hybrid zoom focal length beyond limit.");
                    s_cameraTapZoomState.zoomState = PSDK_CAMERA_TAP_ZOOM_STATE_ZOOM_LIMITED;
                } else {
                    PsdkLogger_UserLogError("hybrid zoom error: 0x%08llX.", psdkStat);
                }
            } else if (s_isTapZooming) {
                psdkStat = PsdkOsal_GetTimeMs(&currentTime);
                if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    PsdkLogger_UserLogError("get start time error: 0x%08llX.", psdkStat);
                }

                if ((currentTime - s_tapZoomStartTime) >= TAP_ZOOM_DURATION) {
                    s_cameraTapZoomState.zoomState = PSDK_CAMERA_TAP_ZOOM_STATE_IDLE;
                    s_cameraTapZoomState.isGimbalMoving = false;
                    s_isTapZooming = false;
                }
            }

            psdkStat = PsdkOsal_MutexUnlock(s_tapZoomMutex);
            if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                PsdkLogger_UserLogError("unlock mutex error: 0x%08llX.", psdkStat);
                goto out;
            }

out:
            psdkStat = PsdkOsal_MutexUnlock(s_zoomMutex);
            if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                PsdkLogger_UserLogError("unlock mutex error: 0x%08llX.", psdkStat);
                continue;
            }
        }

        // 1Hz
        if (USER_UTIL_IS_WORK_TURN(step, 1, PAYLOAD_CAMERA_EMU_TASK_FREQ)) {
            psdkStat = PsdkOsal_MutexLock(s_commonMutex);
            if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                PsdkLogger_UserLogError("lock mutex error: 0x%08llX.", psdkStat);
                continue;
            }

            if (s_cameraState.isRecording) {
                s_cameraState.currentVideoRecordingTimeInSeconds++;
                s_cameraSDCardState.remainSpaceInMB =
                    s_cameraSDCardState.remainSpaceInMB - SDCARD_PER_SECONDS_RECORD_SPACE_IN_MB;
                if (s_cameraSDCardState.remainSpaceInMB > SDCARD_TOTAL_SPACE_IN_MB) {
                    s_cameraSDCardState.remainSpaceInMB = 0;
                    s_cameraSDCardState.isFull = true;
                }
            }

            if (s_cameraState.isShootingIntervalStart == false) {
                isStartIntervalPhotoAction = false;
            }

            if (s_cameraShootPhotoMode == PSDK_CAMERA_SHOOT_PHOTO_MODE_INTERVAL
                && s_cameraState.isShootingIntervalStart == true && s_cameraPhotoTimeIntervalSettings.captureCount > 0
                && s_cameraPhotoTimeIntervalSettings.timeIntervalSeconds > 0) {
                s_cameraState.currentPhotoShootingIntervalTimeInSeconds--;

                if ((s_cameraState.currentPhotoShootingIntervalTimeInSeconds == 0 &&
                     s_cameraState.currentPhotoShootingIntervalCount > 0) ||
                    (s_cameraState.isShootingIntervalStart == true && isStartIntervalPhotoAction == false)) {

                    isStartIntervalPhotoAction = true;

                    s_cameraState.shootingState = PSDK_CAMERA_SHOOTING_INTERVAL_PHOTO;
                    s_cameraState.isStoring = true;
                    s_cameraState.currentPhotoShootingIntervalTimeInSeconds
                        = s_cameraPhotoTimeIntervalSettings.timeIntervalSeconds;
                    if (s_cameraState.currentPhotoShootingIntervalCount < INTERVAL_PHOTOGRAPH_ALWAYS_COUNT) {
                        PsdkLogger_UserLogInfo("interval taking photograph count:%d interval_time:%ds",
                                               (s_cameraPhotoTimeIntervalSettings.captureCount -
                                                s_cameraState.currentPhotoShootingIntervalCount + 1),
                                               s_cameraPhotoTimeIntervalSettings.timeIntervalSeconds);
                        s_cameraState.currentPhotoShootingIntervalCount--;
                        if (s_cameraState.currentPhotoShootingIntervalCount == 0) {
                            s_cameraState.shootingState = PSDK_CAMERA_SHOOTING_PHOTO_IDLE;
                            s_cameraState.isStoring = false;
                            s_cameraState.isShootingIntervalStart = false;
                        }
                    } else {
                        PsdkLogger_UserLogInfo("interval taking photograph always, interval_time:%ds",
                                               s_cameraPhotoTimeIntervalSettings.timeIntervalSeconds);
                        s_cameraState.currentPhotoShootingIntervalCount--;
                    }
                }
            }

            psdkStat = PsdkOsal_MutexUnlock(s_commonMutex);
            if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                PsdkLogger_UserLogError("unlock mutex error: 0x%08llX.", psdkStat);
                continue;
            }
        }
    }
}

#ifndef __CC_ARM
#pragma GCC diagnostic pop
#endif

/* Private functions definition-----------------------------------------------*/
T_PsdkReturnCode PsdkTest_CameraInit(void)
{
    T_PsdkReturnCode returnCode;
    char ipAddr[PSDK_IP_ADDR_STR_SIZE_MAX] = {0};
    uint16_t port = 0;
    T_PsdkAircraftInfoBaseInfo aircraftBaseInfo = {0};

    returnCode = PsdkOsal_MutexCreate(&s_commonMutex);
    if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("create mutex used to lock tap zoom arguments error: 0x%08llX", returnCode);
        return returnCode;
    }

    returnCode = PsdkOsal_MutexCreate(&s_zoomMutex);
    if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("create mutex used to lock tap zoom arguments error: 0x%08llX", returnCode);
        return returnCode;
    }

    returnCode = PsdkOsal_MutexCreate(&s_tapZoomMutex);
    if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("create mutex used to lock tap zoom arguments error: 0x%08llX", returnCode);
        return returnCode;
    }

    returnCode = PsdkPayloadCamera_Init();
    if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("payload camera init error:0x%08llX", returnCode);
    }

    /* Init the SDcard parameters */
    s_cameraSDCardState.isInserted = true;
    s_cameraSDCardState.totalSpaceInMB = SDCARD_TOTAL_SPACE_IN_MB;
    s_cameraSDCardState.remainSpaceInMB = SDCARD_TOTAL_SPACE_IN_MB;
    s_cameraSDCardState.availableCaptureCount = SDCARD_TOTAL_SPACE_IN_MB / SDCARD_PER_PHOTO_SPACE_IN_MB;
    s_cameraSDCardState.availableRecordingTimeInSeconds =
        SDCARD_TOTAL_SPACE_IN_MB / SDCARD_PER_SECONDS_RECORD_SPACE_IN_MB;

    /* Register the camera common handler */
    s_commonHandler.GetSystemState = GetSystemState;
    s_commonHandler.SetMode = SetMode;
    s_commonHandler.GetMode = PsdkTest_CameraGetMode;
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

    returnCode = PsdkPayloadCamera_RegCommonHandler(&s_commonHandler);
    if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("camera register common handler error:0x%08llX", returnCode);
    }

    /* Register the camera metering handler */
    s_exposureMeteringHandler.SetMeteringMode = SetMeteringMode;
    s_exposureMeteringHandler.GetMeteringMode = GetMeteringMode;
    s_exposureMeteringHandler.SetSpotMeteringTarget = SetSpotMeteringTarget;
    s_exposureMeteringHandler.GetSpotMeteringTarget = GetSpotMeteringTarget;

    returnCode = PsdkPayloadCamera_RegExposureMeteringHandler(&s_exposureMeteringHandler);
    if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("camera register exposure metering handler error:0x%08llX", returnCode);
        return returnCode;
    }

    /* Register the camera focus handler */
    s_focusHandler.SetFocusMode = SetFocusMode;
    s_focusHandler.GetFocusMode = GetFocusMode;
    s_focusHandler.SetFocusTarget = SetFocusTarget;
    s_focusHandler.GetFocusTarget = GetFocusTarget;
    s_focusHandler.SetFocusAssistantSettings = SetFocusAssistantSettings;
    s_focusHandler.GetFocusAssistantSettings = GetFocusAssistantSettings;
    s_focusHandler.SetFocusRingValue = SetFocusRingValue;
    s_focusHandler.GetFocusRingValue = GetFocusRingValue;
    s_focusHandler.GetFocusRingValueUpperBound = GetFocusRingValueUpperBound;

    returnCode = PsdkPayloadCamera_RegFocusHandler(&s_focusHandler);
    if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("camera register adjustable focal point handler error:0x%08llX", returnCode);
        return returnCode;
    }

    /* Register the camera digital zoom handler */
    s_digitalZoomHandler.SetDigitalZoomFactor = SetDigitalZoomFactor;
    s_digitalZoomHandler.GetDigitalZoomFactor = PsdkTest_CameraGetDigitalZoomFactor;

    returnCode = PsdkPayloadCamera_RegDigitalZoomHandler(&s_digitalZoomHandler);
    if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("camera register digital zoom handler error:0x%08llX", returnCode);
        return returnCode;
    }

    /* Register the camera optical zoom handler */
    s_opticalZoomHandler.SetOpticalZoomFocalLength = SetOpticalZoomFocalLength;
    s_opticalZoomHandler.GetOpticalZoomFocalLength = GetOpticalZoomFocalLength;
    s_opticalZoomHandler.GetOpticalZoomFactor = PsdkTest_CameraGetOpticalZoomFactor;
    s_opticalZoomHandler.GetOpticalZoomSpec = GetOpticalZoomSpec;
    s_opticalZoomHandler.StartContinuousOpticalZoom = StartContinuousOpticalZoom;
    s_opticalZoomHandler.StopContinuousOpticalZoom = StopContinuousOpticalZoom;

    returnCode = PsdkPayloadCamera_RegOpticalZoomHandler(&s_opticalZoomHandler);
    if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("camera register optical zoom handler error:0x%08llX", returnCode);
        return returnCode;
    }

    /* Register the camera tap zoom handler */
    s_tapZoomHandler.GetTapZoomState = GetTapZoomState;
    s_tapZoomHandler.SetTapZoomEnabled = SetTapZoomEnabled;
    s_tapZoomHandler.GetTapZoomEnabled = GetTapZoomEnabled;
    s_tapZoomHandler.SetTapZoomMultiplier = SetTapZoomMultiplier;
    s_tapZoomHandler.GetTapZoomMultiplier = GetTapZoomMultiplier;
    s_tapZoomHandler.TapZoomAtTarget = TapZoomAtTarget;

    returnCode = PsdkPayloadCamera_RegTapZoomHandler(&s_tapZoomHandler);
    if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("camera register tap zoom handler error:0x%08llX", returnCode);
        return returnCode;
    }

    /* Create the camera emu task */
    if (PsdkOsal_TaskCreate(&s_userCameraThread, UserCamera_Task, "user_camera_task",
                            PAYLOAD_CAMERA_EMU_TASK_STACK_SIZE, NULL)
        != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("user camera task create error");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    if (PsdkAircraftInfo_GetBaseInfo(&aircraftBaseInfo) != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("get aircraft information error.");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    if (aircraftBaseInfo.aircraftType == PSDK_AIRCRAFT_INFO_TYPE_M300_RTK) {
        returnCode = PsdkPayloadCamera_SetVideoStreamType(PSDK_CAMERA_VIDEO_STREAM_TYPE_H264_DJI_FORMAT);
        if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            PsdkLogger_UserLogError("psdk camera set video stream error.");
            return PSDK_RETURN_CODE_ERR_UNKNOWN;
        }
        s_cameraVideoStreamType = PSDK_CAMERA_VIDEO_STREAM_TYPE_H264_DJI_FORMAT;
    } else {
        returnCode = PsdkPayloadCamera_SetVideoStreamType(PSDK_CAMERA_VIDEO_STREAM_TYPE_H264_CUSTOM_FORMAT);
        if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            PsdkLogger_UserLogError("psdk camera set video stream error.");
            return PSDK_RETURN_CODE_ERR_UNKNOWN;
        }
        s_cameraVideoStreamType = PSDK_CAMERA_VIDEO_STREAM_TYPE_H264_CUSTOM_FORMAT;
    }

    returnCode = PsdkPayloadCamera_GetVideoStreamRemoteAddress(ipAddr, &port);
    if (returnCode == PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogInfo("get video stream remote address:%s_%d", ipAddr, port);
    } else {
        PsdkLogger_UserLogError("get video stream remote address error.");
        return PSDK_RETURN_CODE_ERR_UNKNOWN;
    }

    s_isCamInited = true;

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_PsdkReturnCode PsdkTest_CameraGetDigitalZoomFactor(psdk_f32_t *factor)
{
    T_PsdkReturnCode psdkStat;

    psdkStat = PsdkOsal_MutexLock(s_zoomMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("lock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    *factor = s_cameraDigitalZoomFactor;

    psdkStat = PsdkOsal_MutexUnlock(s_zoomMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("unlock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_PsdkReturnCode PsdkTest_CameraGetOpticalZoomFactor(psdk_f32_t *factor)
{
    T_PsdkReturnCode psdkStat;

    psdkStat = PsdkOsal_MutexLock(s_zoomMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("lock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    //Formula:factor = currentFocalLength / minFocalLength
    *factor = (psdk_f32_t) s_cameraOpticalZoomFocalLength / ZOOM_OPTICAL_FOCAL_MIN_LENGTH;

    psdkStat = PsdkOsal_MutexUnlock(s_zoomMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("unlock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_PsdkReturnCode PsdkTest_CameraGetMode(E_PsdkCameraMode *mode)
{
    T_PsdkReturnCode psdkStat;

    psdkStat = PsdkOsal_MutexLock(s_commonMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("lock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    *mode = s_cameraState.cameraMode;

    psdkStat = PsdkOsal_MutexUnlock(s_commonMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("unlock mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_PsdkReturnCode PsdkTest_CameraGetVideoStreamType(E_PsdkCameraVideoStreamType *type)
{
    T_PsdkReturnCode psdkStat;

    if (s_isCamInited == false) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT_IN_CURRENT_STATE;
    }

    *type = s_cameraVideoStreamType;

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

bool PsdkTest_CameraIsInited(void)
{
    return s_isCamInited;
}

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
