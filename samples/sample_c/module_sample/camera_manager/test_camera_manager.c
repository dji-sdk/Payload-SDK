/**
 ********************************************************************
 * @file    test_camera_manager.c
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
#include <widget_interaction_test/test_widget_interaction.h>
#include <utils/util_misc.h>
#include <time.h>
#include "test_camera_manager.h"
#include "dji_camera_manager.h"
#include "dji_platform.h"
#include "dji_logger.h"
#include "dji_mop_channel.h"

/* Private constants ---------------------------------------------------------*/
#define TEST_CAMERA_MANAGER_MEDIA_FILE_NAME_MAX_SIZE             256
#define TEST_CAMERA_MANAGER_MEDIA_DOWNLOAD_FILE_NUM              5
#define CAMERA_MANAGER_SUBSCRIPTION_FREQ                         5

#define TEST_CAMERA_MAX_INFRARED_ZOOM_FACTOR          8
#define TEST_CAMERA_MIN_INFRARED_ZOOM_FACTOR          2
#define TEST_CAMERA_MOP_CHANNEL_SUBSCRIBE_POINT_CLOUD_CHANNEL_ID         49154
#define TEST_CAMERA_MOP_CHANNEL_SUBSCRIBE_POINT_CLOUD_RECV_BUFFER        (512 * 1024)
#define TEST_CAMERA_MOP_CHANNEL_COLOR_POINTS_BUFFER                      (512 * 1024)
#define TEST_CAMERA_MOP_CHANNEL_WAIT_TIME_MS                             (3 * 1000)
#define TEST_CAMERA_MOP_CHANNEL_MAX_RECV_COUNT                           30
#define TEST_CAMEAR_POINT_CLOUD_FILE_PATH_STR_MAX_SIZE                   256
#define TEST_CAMEAR_MEDIA_SUB_FILE_NOT_FOUND                             -1

/* Private types -------------------------------------------------------------*/
typedef struct {
    E_DjiCameraType cameraType;
    char *cameraTypeStr;
} T_DjiTestCameraTypeStr;

/* Private values -------------------------------------------------------------*/
static const T_DjiTestCameraTypeStr s_cameraTypeStrList[] = {
    {DJI_CAMERA_TYPE_UNKNOWN, "Unknown"},
    {DJI_CAMERA_TYPE_Z30,     "Zenmuse Z30"},
    {DJI_CAMERA_TYPE_XT2,     "Zenmuse XT2"},
    {DJI_CAMERA_TYPE_PSDK,    "Payload Camera"},
    {DJI_CAMERA_TYPE_XTS,     "Zenmuse XTS"},
    {DJI_CAMERA_TYPE_H20,     "Zenmuse H20"},
    {DJI_CAMERA_TYPE_H20T,    "Zenmuse H20T"},
    {DJI_CAMERA_TYPE_P1,      "Zenmuse P1"},
    {DJI_CAMERA_TYPE_L1,      "Zenmuse L1"},
    {DJI_CAMERA_TYPE_L2,      "Zenmuse L2"},
    {DJI_CAMERA_TYPE_H20N,    "Zenmuse H20N"},
    {DJI_CAMERA_TYPE_M30,     "M30 Camera"},
    {DJI_CAMERA_TYPE_M30T,    "M30T Camera"},
    {DJI_CAMERA_TYPE_M3E,     "M3E Camera"},
    {DJI_CAMERA_TYPE_M3T,     "M3T Camera"},
    {DJI_CAMERA_TYPE_M3D,     "M3D Camera"},
    {DJI_CAMERA_TYPE_M3TD,    "M3TD Camera"},
    {DJI_CAMERA_TYPE_H30,     "H30 Camera"},
    {DJI_CAMERA_TYPE_H30T,    "H30T Camera"},
};

static FILE *s_downloadMediaFile = NULL;
static T_DjiCameraManagerFileList s_meidaFileList;
static uint32_t downloadStartMs = 0;
static uint32_t downloadEndMs = 0;
static char downloadFileName[TEST_CAMERA_MANAGER_MEDIA_FILE_NAME_MAX_SIZE] = {0};
static uint32_t s_nextDownloadFileIndex = 0;
static T_DjiMopChannelHandle s_mopChannelHandle;
static char s_pointCloudFilePath[TEST_CAMEAR_POINT_CLOUD_FILE_PATH_STR_MAX_SIZE];

/* Private functions declaration ---------------------------------------------*/
static uint8_t DjiTest_CameraManagerGetCameraTypeIndex(E_DjiCameraType cameraType);
static T_DjiReturnCode DjiTest_CameraManagerMediaDownloadAndDeleteMediaFile(E_DjiMountPosition position);
static T_DjiReturnCode DjiTest_CameraManagerMediaDownloadFileListBySlices(E_DjiMountPosition position);
static T_DjiReturnCode DjiTest_CameraManagerDownloadFileDataCallback(T_DjiDownloadFilePacketInfo packetInfo,
                                                                     const uint8_t *data, uint16_t len);
static T_DjiReturnCode DjiTest_CameraManagerGetAreaThermometryData(E_DjiMountPosition position);
static T_DjiReturnCode DjiTest_CameraManagerGetPointThermometryData(E_DjiMountPosition position);

static T_DjiReturnCode DjiTest_CameraManagerGetLidarRangingInfo(E_DjiMountPosition position);
T_DjiReturnCode DjiTest_CameraManagerSubscribePointCloud(E_DjiMountPosition position);

/* Exported functions definition ---------------------------------------------*/
/*! @brief Sample to set EV for camera, using async api
 *
 *  @note In this interface, ev will be got then be set.
 *  In order to use this function, the camera exposure mode should be
 *  set to be DJI_CAMERA_MANAGER_EXPOSURE_MODE_PROGRAM_AUTO,
 *  DJI_CAMERA_MANAGER_EXPOSURE_MODE_SHUTTER_PRIORITY or
 *  DJI_CAMERA_MANAGER_EXPOSURE_MODE_APERTURE_PRIORITY first
 *  @param index payload node index, input limit see enum
 * DJI::OSDK::PayloadIndexType
 *  @param dataTarget the target exposure mode
 *  @return T_DjiReturnCode error code
 */
T_DjiReturnCode DjiTest_CameraManagerSetEV(E_DjiMountPosition position,
                                           E_DjiCameraManagerExposureCompensation exposureCompensation)
{
    T_DjiReturnCode returnCode;
    E_DjiCameraManagerExposureCompensation exposureCompensationTemp;

    returnCode = DjiCameraManager_GetExposureCompensation(position, &exposureCompensationTemp);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS &&
        returnCode != DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND) {
        USER_LOG_ERROR("Get mounted position %d exposure compensation failed, error code: 0x%08X.",
                       position, returnCode);
    }

    if (exposureCompensationTemp == exposureCompensation) {
        USER_LOG_INFO("The mount position %d camera's exposure compensation is already what you expected.",
                      position);
        return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    }

    returnCode = DjiCameraManager_SetExposureCompensation(position, exposureCompensation);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS &&
        returnCode != DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND) {
        USER_LOG_ERROR("Set mounted position %d camera's exposure compensation %d failed,"
                       "error code: 0x%08X.", position, exposureCompensation, returnCode);
    }

    return returnCode;
}

/*! @brief Sample to set exposure mode for camera, using async api
 *
 *  @note In this interface, exposure will be got then be set.
 *  @param index payload node index, input limit see enum
 * DJI::OSDK::PayloadIndexType
 *  @param dataTarget the target exposure mode
 *  @return T_DjiReturnCode error code
 */
T_DjiReturnCode DjiTest_CameraManagerSetExposureMode(E_DjiMountPosition position,
                                                     E_DjiCameraManagerExposureMode exposureMode)
{
    T_DjiReturnCode returnCode;
    E_DjiCameraManagerExposureMode exposureModeTemp;

    returnCode = DjiCameraManager_GetExposureMode(position, &exposureModeTemp);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS &&
        returnCode != DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND) {
        USER_LOG_ERROR("Get mounted position %d exposure mode failed, error code: 0x%08X",
                       position, returnCode);
        return returnCode;
    }

    if (exposureModeTemp == exposureMode) {
        USER_LOG_INFO("The mounted position %d camera's exposure mode is already what you expected.",
                      position);
        return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    }

    returnCode = DjiCameraManager_SetExposureMode(position, exposureMode);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS &&
        returnCode != DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND) {
        USER_LOG_ERROR("Set mounted position %d camera's exposure mode %d failed, current exposure is %d,"
                       " error code: 0x%08X", position, exposureMode, exposureModeTemp, returnCode);
    }

    return returnCode;
}

/*! @brief Sample to set ISO value for camera, using async api
 *
 *  @note In this interface, ISO will be got then be set.
 *  For the X5, X5R, X4S and X5S, the ISO value can be set for all
 *  modes. For the other cameras, the ISO value can only be set when
 *  the camera exposure mode is in Manual mode
 *  @param index payload node index, input limit see enum
 * DJI::OSDK::PayloadIndexType
 *  @param dataTarget the target ISO value
 *  @return T_DjiReturnCode error code
 */
T_DjiReturnCode DjiTest_CameraManagerSetISO(E_DjiMountPosition position,
                                            E_DjiCameraManagerISO isoData)
{
    T_DjiReturnCode returnCode;
    E_DjiCameraManagerISO isoDataTemp;

    returnCode = DjiCameraManager_GetISO(position, &isoDataTemp);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS &&
        returnCode != DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND) {
        USER_LOG_ERROR("Get mounted position %d camera's iso failed, error code: 0x%08X.",
                       position, returnCode);
        return returnCode;
    }

    if (isoDataTemp == isoData) {
        USER_LOG_INFO("The mounted position %d camera's iso is already what you expected.",
                      position);
        return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    }

    returnCode = DjiCameraManager_SetISO(position, isoData);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS &&
        returnCode != DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND) {
        USER_LOG_ERROR("Set mounted position %d camera's iso %d failed, "
                       "error code: 0x%08X.", position, isoData, returnCode);
    }

    return returnCode;
}

/*! @brief Sample to set shutter speed for camera, using async api
 *
 *  @note In this interface, shutter speed will be got then be set.
 *  The shutter speed can be set only when the camera exposure mode
 *  is Shutter mode or Manual mode. The shutter speed should not be
 *  set slower than the video frame rate when the camera's mode is
 *  RECORD_VIDEO.
 *  @param index payload node index, input limit see enum
 * DJI::OSDK::PayloadIndexType
 *  @param dataTarget the target shutter speed
 *  @return T_DjiReturnCode error code
 */
T_DjiReturnCode DjiTest_CameraManagerSetShutterSpeed(E_DjiMountPosition position,
                                                     E_DjiCameraManagerShutterSpeed shutterSpeed)
{
    T_DjiReturnCode returnCode;
    E_DjiCameraManagerShutterSpeed shutterSpeedTemp;

    returnCode = DjiCameraManager_GetShutterSpeed(position, &shutterSpeedTemp);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS &&
        returnCode != DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND) {
        USER_LOG_ERROR("Get mounted position %d camera's shutter speed failed, "
                       "error code: 0x%08X.", position, returnCode);
        return returnCode;
    }

    if (shutterSpeedTemp == shutterSpeed) {
        USER_LOG_INFO("The mounted position %d camera's shutter speed is already what you expected.",
                      position);
        return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    }

    returnCode = DjiCameraManager_SetShutterSpeed(position, shutterSpeed);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS &&
        returnCode != DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND) {
        USER_LOG_ERROR("Set mounted position %d camera's shutter speed %d failed, "
                       "error code: 0x%08X.", position, shutterSpeed, returnCode);
    }

    return returnCode;
}

/*! @brief Sample to set shutter aperture value for camera, using async api
 *
 *  @note In this interface, aperture value will be got then be set.
 *  In order to use this function, the exposure must be in MANUAL or APERTURE_PRIORITY.
*   Supported only by the X5, X5R, X4S, X5S camera.
 *  @param index payload node index, input limit see enum
 * DJI::OSDK::PayloadIndexType
 *  @param dataTarget the target aperture value
 *  @return T_DjiReturnCode error code
 */
T_DjiReturnCode DjiTest_CameraManagerSetAperture(E_DjiMountPosition position,
                                                 E_DjiCameraManagerAperture aperture)
{
    T_DjiReturnCode returnCode;
    E_DjiCameraManagerAperture apertureTemp;

    returnCode = DjiCameraManager_GetAperture(position, &apertureTemp);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS &&
        returnCode != DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND) {
        USER_LOG_ERROR("Get mounted position %d camera's aperture failed, "
                       "error code: 0x%08X.", position, returnCode);
        return returnCode;
    }

    if (apertureTemp == aperture) {
        USER_LOG_INFO("The mounted position %d camera's aperture is already what you expected.",
                      position);
        return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    }

    returnCode = DjiCameraManager_SetAperture(position, aperture);

    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS &&
        returnCode != DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND) {
        USER_LOG_ERROR("Set mounted position %d camera's aperture %d failed, "
                       "error code: 0x%08X.", position, aperture, returnCode);
    }

    return returnCode;
}

/*! @brief Sample to set focus point for camera, using async api
 *
 *  @note In this interface, focus mode will be set to be AUTO. Then the
 * focus point will be set to be (x, y).
 * When the focus mode is auto, the target point is the focal point.
 * When the focus mode is manual, the target point is the zoom out area
 * if the focus assistant is enabled for the manual mode. Supported only
 * by the X5, X5R, Z3 cameras, Mavic Pro camera, Phantom 4 Pro camera,
 * Mavic 2 Pro, Mavic 2 Zoom Camera, Mavic 2 Enterprise Camera, X5S. "
 * It's should be attention that X4S will keep focus point as (0.5,0.5) "
 * all the time, the setting of focus point to X4S will quickly replaced "
 * by (0.5, 0.5).
 *  @param index payload node index, input limit see enum
 * DJI::OSDK::PayloadIndexType
 *  @param x the x value of target focus point, 0~1
 *  @param y the y value of target focus point, 0~1
 *  @return T_DjiReturnCode error code
 */
T_DjiReturnCode DjiTest_CameraManagerSetFocusPoint(E_DjiMountPosition position,
                                                   T_DjiCameraManagerFocusPosData focusPoint)
{
    T_DjiReturnCode returnCode;

    /*!< set camera focus mode to be CameraModule::FocusMode::AUTO */
    USER_LOG_INFO("Set mounted position %d camera's focus mode to auto mode.",
                  position);
    returnCode = DjiCameraManager_SetFocusMode(position, DJI_CAMERA_MANAGER_FOCUS_MODE_AUTO);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS &&
        returnCode != DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND) {
        USER_LOG_ERROR("Set mounted position %d camera's focus mode(%d) failed,"
                       " error code :0x%08X.", position, DJI_CAMERA_MANAGER_FOCUS_MODE_AUTO,
                       returnCode);
        return returnCode;
    }

    USER_LOG_INFO("Set mounted position %d camera's focus point to (%0.1f, %0.1f).",
                  position, focusPoint.focusX, focusPoint.focusY);
    returnCode = DjiCameraManager_SetFocusTarget(position, focusPoint);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS &&
        returnCode != DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND) {
        USER_LOG_ERROR("Set mounted position %d camera's focus point(%0.1f, %0.1f) failed,"
                       " error code :0x%08X.", position, focusPoint.focusX, focusPoint.focusY,
                       returnCode);
    }

    return returnCode;
}

/*! @brief Sample to set tap-zoom point for camera, using async api
 *
 *  @note In this interface, tap-zoom function will be enable and the
 * multiplier will be set. Then the tap-zoom function will start with the
 * target tap-zoom point (x, y)
 *  @param index payload node index, input limit see enum
 * DJI::OSDK::PayloadIndexType
 *  @param multiplier the zoom multiplier of each tap zoom
 *  @param x the x value of target tap-zoom point, 0~1
 *  @param y the y value of target tap-zoom point, 0~1
 *  @return T_DjiReturnCode error code
 */
T_DjiReturnCode DjiTest_CameraManagerSetTapZoomPoint(E_DjiMountPosition position, uint8_t multiplier,
                                                     T_DjiCameraManagerTapZoomPosData tapZoomPosData)
{
    T_DjiReturnCode returnCode;

    /*!< set camera tap zoom enable parameter to be enable */
    USER_LOG_INFO("Enable mounted position %d camera's tap zoom status.",
                  position);
    returnCode = DjiCameraManager_SetTapZoomEnabled(position, true);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS &&
        returnCode != DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND) {
        USER_LOG_ERROR("Enable mounted position %d camera's tap zoom function failed,"
                       " error code :0x%08X.", position, returnCode);
        return returnCode;
    }

    /*!< set camera tap zoom multiplier parameter */
    USER_LOG_INFO("Set mounted position %d camera's tap zoom multiplier to %d x.",
                  position, multiplier);
    returnCode = DjiCameraManager_SetTapZoomMultiplier(position, multiplier);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS &&
        returnCode != DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND) {
        USER_LOG_ERROR("Set mounted position %d camera's tap zoom multiplier(%d) failed,"
                       " error code :0x%08X.", position, multiplier, returnCode);
        return returnCode;
    }

    USER_LOG_INFO("Set mounted position %d camera's tap zoom point to (%f, %f).",
                  position, tapZoomPosData.focusX, tapZoomPosData.focusY);
    returnCode = DjiCameraManager_TapZoomAtTarget(position, tapZoomPosData);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS &&
        returnCode != DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND) {
        USER_LOG_ERROR("Set mounted position %d camera's tap zoom target point(%f ,%f) failed,"
                       " error code :0x%08X.", position, tapZoomPosData.focusX, tapZoomPosData.focusY,
                       returnCode);
    }

    return returnCode;
}

/*! @brief Sample to execute position zoom on camera, using sync api
 *
 *  @note It is only supported by X5, X5R and X5S camera on Osmo with lens
 * Olympus M.Zuiko ED 14-42mm f/3.5-5.6 EZ, Z3 camera, Z30 camera.
 *  @note In this interface, the zoom will set the zoom factor as the your
 * target value.
 *  @param index payload node index, input limit see enum
 * DJI::OSDK::PayloadIndexType
 *  @param factor target zoom factor
 *  @return T_DjiReturnCode error code
 */
T_DjiReturnCode DjiTest_CameraManagerOpticalZoom(E_DjiMountPosition position,
                                                 E_DjiCameraZoomDirection zoomDirection,
                                                 dji_f32_t factor)
{
    T_DjiReturnCode returnCode;
    T_DjiCameraManagerOpticalZoomParam opticalZoomParam;

    returnCode = DjiCameraManager_GetOpticalZoomParam(position, &opticalZoomParam);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS &&
        returnCode != DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND) {
        USER_LOG_ERROR("Get mounted position %d camera's zoom param failed, error code :0x%08X",
                       position, returnCode);
        return returnCode;
    }

    USER_LOG_INFO("The mounted position %d camera's current optical zoom factor is:%0.1f x, "
                  "max optical zoom factor is :%0.1f x", position, opticalZoomParam.currentOpticalZoomFactor,
                  opticalZoomParam.maxOpticalZoomFactor);

    USER_LOG_INFO("Set mounted position %d camera's zoom factor: %0.1f x.", position, factor);
    returnCode = DjiCameraManager_SetOpticalZoomParam(position, zoomDirection, factor);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS &&
        returnCode != DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND) {
        USER_LOG_INFO("Set mounted position %d camera's zoom factor(%0.1f) failed, error code :0x%08X",
                      position, factor, returnCode);
    }

    return returnCode;
}

/*! @brief Sample to execute continuous zoom on camera, using sync api
 *
 *  @note It is only supported by X5, X5R and X5S camera on Osmo with lens
 * Olympus M.Zuiko ED 14-42mm f/3.5-5.6 EZ, Z3 camera, Z30 camera.
 *  @note In this interface, the zoom will start with the designated direction
 * and speed, and will stop after zoomTimeInSecond second(s).
 *  @param index payload node index, input limit see enum
 * DJI::OSDK::PayloadIndexType
 *  @param direction the choice of zoom out or zoom in
 *  @param speed zooming speed
 *  @return T_DjiReturnCode error code
 */
T_DjiReturnCode DjiTest_CameraManagerStartContinuousZoom(E_DjiMountPosition position,
                                                         E_DjiCameraZoomDirection zoomDirection,
                                                         E_DjiCameraZoomSpeed zoomSpeed)
{
    T_DjiReturnCode returnCode;

    //    USER_LOG_INFO("Mounted position %d camera start continuous optical zoom.\r\n", position);
    returnCode = DjiCameraManager_StartContinuousOpticalZoom(position, zoomDirection, zoomSpeed);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS &&
        returnCode != DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND) {
        USER_LOG_ERROR("Mounted position %d camera start continuous zoom  failed,"
                       " error code :0x%08X.", position, returnCode);
    }

    return returnCode;
}

/*! @brief Sample to stop continuous zoom on camera, using async api
 *
 *  @note It is only supported by X5, X5R and X5S camera on Osmo with lens
 * Olympus M.Zuiko ED 14-42mm f/3.5-5.6 EZ, Z3 camera, Z30 camera.
 *  @param index payload node index, input limit see enum
 * DJI::OSDK::PayloadIndexType
 *  @return T_DjiReturnCode error code
 */
T_DjiReturnCode DjiTest_CameraManagerStopContinuousZoom(E_DjiMountPosition position)
{
    T_DjiReturnCode returnCode;

//    USER_LOG_INFO("Mounted position %d camera stop continuous optical zoom.\r\n", position);
    returnCode = DjiCameraManager_StopContinuousOpticalZoom(position);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS &&
        returnCode != DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND) {
        USER_LOG_ERROR("Mounted position %d camera stop continuous zoom failed,"
                       " error code :0x%08X", position, returnCode);
    }

    return returnCode;
}

/*! @brief Sample to shoot single photo, using async api
 *
 *  @note In this interface, camera will be set to be the SHOOT_PHOTO mode
 * then start to shoot a single photo.
 *  @param index payload node index, input limit see enum
 * DJI::OSDK::PayloadIndexType
 *  @return T_DjiReturnCode error code
 */
T_DjiReturnCode DjiTest_CameraManagerStartShootSinglePhoto(E_DjiMountPosition position)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    E_DjiCameraManagerWorkMode workMode;

    /*!< set camera work mode as shoot photo */
    USER_LOG_INFO("Set mounted position %d camera's work mode as shoot-photo mode", position);
    returnCode = DjiCameraManager_SetMode(position, DJI_CAMERA_MANAGER_WORK_MODE_SHOOT_PHOTO);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS &&
        returnCode != DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND) {
        USER_LOG_ERROR("set mounted position %d camera's work mode as shoot-photo mode failed,"
                       " error code :0x%08X", position, returnCode);
        return returnCode;
    }

    osalHandler->TaskSleepMs(1000);

    returnCode = DjiCameraManager_GetMode(position, &workMode);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS &&
        returnCode != DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND) {
        USER_LOG_ERROR("get mounted position %d camera's work mode failed,"
                       " error code :0x%08X", position, returnCode);
        return returnCode;
    }
    USER_LOG_INFO("Camera current workmode is %d", workMode);

    /*!< set shoot-photo mode */
    USER_LOG_INFO("Set mounted position %d camera's shoot photo mode as single-photo mode", position);
    returnCode = DjiCameraManager_SetShootPhotoMode(position, DJI_CAMERA_MANAGER_SHOOT_PHOTO_MODE_SINGLE);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS &&
        returnCode != DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND) {
        USER_LOG_ERROR("set mounted position %d camera's shoot photo mode as single-photo mode failed,"
                       " error code :0x%08X", position, returnCode);
        return returnCode;
    }

    /*! wait the APP change the shoot-photo mode display */
    USER_LOG_INFO("Sleep 0.5s...");
    osalHandler->TaskSleepMs(500);

    /*!< start to shoot single photo */
    USER_LOG_INFO("Mounted position %d camera start to shoot photo", position);
    returnCode = DjiCameraManager_StartShootPhoto(position, DJI_CAMERA_MANAGER_SHOOT_PHOTO_MODE_SINGLE);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Mounted position %d camera shoot photo failed, "
                       "error code :0x%08X", position, returnCode);
    }

    return returnCode;
}

/*! @brief Sample to shoot burst photo, using async api
 *
 *  @note In this interface, camera will be set to be the SHOOT_PHOTO mode
 * then start to shoot a burst photo.
 *  @param index payload node index, input limit see enum
 * DJI::OSDK::PayloadIndexType
 *  @param count The number of pictures in each burst shooting
 *  @return T_DjiReturnCode error code
 */
T_DjiReturnCode DjiTest_CameraManagerStartShootBurstPhoto(E_DjiMountPosition position,
                                                          E_DjiCameraBurstCount burstCount)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    E_DjiCameraManagerWorkMode workMode;

    /*!< set camera work mode as shoot photo */
    USER_LOG_INFO("set mounted position %d camera's work mode as shoot photo mode.", position);
    returnCode = DjiCameraManager_SetMode(position, DJI_CAMERA_MANAGER_WORK_MODE_SHOOT_PHOTO);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("set mounted position %d camera's work mode as shoot photo mode failed,"
                       " error code :0x%08X.", position, returnCode);
        return returnCode;
    }

    osalHandler->TaskSleepMs(1000);

    returnCode = DjiCameraManager_GetMode(position, &workMode);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS &&
        returnCode != DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND) {
        USER_LOG_ERROR("get mounted position %d camera's work mode failed,"
                       " error code :0x%08X", position, returnCode);
        return returnCode;
    }
    USER_LOG_INFO("Camera current workmode is %d", workMode);

    /*!< set shoot-photo mode */
    USER_LOG_INFO("Set mounted position %d camera's shoot photo mode as burst-photo mode", position);
    returnCode = DjiCameraManager_SetShootPhotoMode(position, DJI_CAMERA_MANAGER_SHOOT_PHOTO_MODE_BURST);
    if (returnCode == DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND) {
        return returnCode;
    }

    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("set mounted position %d camera's shoot photo mode as burst-photo mode failed,"
                       " error code :0x%08X", position, returnCode);
        return returnCode;
    }

    /*! wait the APP change the shoot-photo mode display */
    USER_LOG_INFO("Sleep 0.5s...");
    osalHandler->TaskSleepMs(500);

    /*!< set shoot-photo mode parameter */
    USER_LOG_INFO("Set mounted position %d camera's burst count to %d", position, burstCount);
    returnCode = DjiCameraManager_SetPhotoBurstCount(position, burstCount);

    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS &&
        returnCode != DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND) {
        USER_LOG_ERROR("set mounted position %d camera's burst count(%d) failed,"
                       " error code :0x%08X.", position, burstCount, returnCode);
        return returnCode;
    }
    /*!< start to shoot single photo */
    USER_LOG_INFO("Mounted position %d camera start to shoot photo.", position);
    returnCode = DjiCameraManager_StartShootPhoto(position, DJI_CAMERA_MANAGER_SHOOT_PHOTO_MODE_BURST);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Mounted position %d camera shoot photo failed, "
                       "error code :0x%08X.", position, returnCode);
    }

    return returnCode;
}

/*! @brief Sample to start shooting interval photo, using async api
 *
 *  @note In this interface, camera will be set to be the SHOOT_PHOTO mode
 * then start to shoot a interval photo.
 *  @param index payload node index, input limit see enum
 * DJI::OSDK::PayloadIndexType
 *  @param intervalData the parameter of interval shooting
 *  @return T_DjiReturnCode error code
 */
T_DjiReturnCode DjiTest_CameraManagerStartShootIntervalPhoto(E_DjiMountPosition position,
                                                             T_DjiCameraPhotoTimeIntervalSettings intervalData)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    E_DjiCameraManagerWorkMode workMode;

    /*!< set camera work mode as shoot photo */
    USER_LOG_INFO("set mounted position %d camera's work mode as shoot photo mode.", position);
    returnCode = DjiCameraManager_SetMode(position, DJI_CAMERA_MANAGER_WORK_MODE_SHOOT_PHOTO);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("set mounted position %d camera's work mode as shoot photo mode failed,"
                       " error code :0x%08X.", position, returnCode);
        return returnCode;
    }

    osalHandler->TaskSleepMs(1000);

    returnCode = DjiCameraManager_GetMode(position, &workMode);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS &&
        returnCode != DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND) {
        USER_LOG_ERROR("get mounted position %d camera's work mode failed,"
                       " error code :0x%08X", position, returnCode);
        return returnCode;
    }
    USER_LOG_INFO("Camera current workmode is %d", workMode);

    /*!< set shoot-photo mode */
    USER_LOG_INFO("Set mounted position %d camera's shoot photo mode as interval-photo mode", position);
    returnCode = DjiCameraManager_SetShootPhotoMode(position, DJI_CAMERA_MANAGER_SHOOT_PHOTO_MODE_INTERVAL);
    if (returnCode == DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND) {
        return returnCode;
    }

    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("set mounted position %d camera's shoot photo mode as interval-photo mode failed,"
                       " error code :0x%08X", position, returnCode);
        return returnCode;
    }

    /*! wait the APP change the shoot-photo mode display */
    USER_LOG_INFO("Sleep 0.5s...");
    osalHandler->TaskSleepMs(500);

    /*!< set shoot-photo mode parameter */
    USER_LOG_INFO("Set mounted position %d camera's interval time to %d s", position, intervalData.timeIntervalSeconds);
    returnCode = DjiCameraManager_SetPhotoTimeIntervalSettings(position, intervalData);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS &&
        returnCode != DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND) {
        USER_LOG_ERROR("set mounted position %d camera's time interval parameter"
                       "(photo number:%d, time interval:%d) failed, error code :0x%08X.",
                       position, intervalData.captureCount, intervalData.timeIntervalSeconds, returnCode);
        return returnCode;
    }

    /*! wait the APP change the shoot-photo mode display */
    USER_LOG_INFO("Sleep 0.5s...");
    osalHandler->TaskSleepMs(500);

    /*!< start to shoot single photo */
    USER_LOG_INFO("Mounted position %d camera start to shoot photo.", position);
    returnCode = DjiCameraManager_StartShootPhoto(position, DJI_CAMERA_MANAGER_SHOOT_PHOTO_MODE_INTERVAL);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS &&
        returnCode != DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND) {
        USER_LOG_ERROR("Mounted position %d camera shoot photo failed, "
                       "error code :0x%08X.", position, returnCode);
    }

    return returnCode;
}

/*! @brief Sample to stop shooting, using async api
 *
 *  @note In this interface, camera will stop all the shooting action
 *  @param index payload node index, input limit see enum
 * DJI::OSDK::PayloadIndexType
 *  @return T_DjiReturnCode error code
 */
T_DjiReturnCode DjiTest_CameraManagerStopShootPhoto(E_DjiMountPosition position)
{
    T_DjiReturnCode returnCode;

    USER_LOG_INFO("Mounted position %d camera stop to shoot photo.", position);
    returnCode = DjiCameraManager_StopShootPhoto(position);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS &&
        returnCode != DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND) {
        USER_LOG_ERROR("Mounted position %d camera stop to shoot photo failed,"
                       " error code:0x%08X.", position, returnCode);
    }

    return returnCode;
}

/*! @brief Sample to start record video, using async api
 *
 *  @note In this interface, camera will be set to be the RECORD_VIDEO mode
 * then start to record video.
 *  @param index payload node index, input limit see enum
 * DJI::OSDK::PayloadIndexType
 *  @return T_DjiReturnCode error code
 */
T_DjiReturnCode DjiTest_CameraManagerStartRecordVideo(E_DjiMountPosition position)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    E_DjiCameraManagerWorkMode workMode;

    /*!< set camera work mode as record video */
    USER_LOG_INFO("set mounted position %d camera's work mode as record-video mode", position);
    returnCode = DjiCameraManager_SetMode(position, DJI_CAMERA_MANAGER_WORK_MODE_RECORD_VIDEO);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS &&
        returnCode != DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND) {
        USER_LOG_ERROR("set mounted position %d camera's work mode as record-video mode failed,"
                       " error code :0x%08X", position, returnCode);
        return returnCode;
    }

    osalHandler->TaskSleepMs(1000);

    returnCode = DjiCameraManager_GetMode(position, &workMode);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS &&
        returnCode != DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND) {
        USER_LOG_ERROR("get mounted position %d camera's work mode failed,"
                       " error code :0x%08X", position, returnCode);
        return returnCode;
    }
    USER_LOG_INFO("Camera current workmode is %d", workMode);

    /*!< start to take video */
    USER_LOG_INFO("Mounted position %d camera start to record video.", position);
    returnCode = DjiCameraManager_StartRecordVideo(position);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS &&
        returnCode != DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND) {
        USER_LOG_ERROR("Mounted position %d camera start to record video failed,"
                       " error code:0x%08X.", position, returnCode);
    }

    return returnCode;
}

/*! @brief Sample to stop record video, using async api
 *
 *  @note In this interface, camera will be set to be the RECORD_VIDEO mode
 * then stop recording video.
 *  @param index payload node index, input limit see enum
 * DJI::OSDK::PayloadIndexType
 *  @return T_DjiReturnCode error code
 */
T_DjiReturnCode DjiTest_CameraManagerStopRecordVideo(E_DjiMountPosition position)
{
    T_DjiReturnCode returnCode;
    USER_LOG_INFO("Mounted position %d camera stop to record video.", position);
    returnCode = DjiCameraManager_StopRecordVideo(position);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS &&
        returnCode != DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND) {
        USER_LOG_ERROR("Mounted position %d camera stop to record video failed,"
                       " error code:0x%08X.", position, returnCode);
    }

    return returnCode;
}

T_DjiReturnCode DjiTest_CameraManagerRunSample(E_DjiMountPosition mountPosition,
                                               E_DjiTestCameraManagerSampleSelect cameraManagerSampleSelect)
{
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    T_DjiReturnCode returnCode;
    E_DjiCameraType cameraType;
    T_DjiCameraManagerFirmwareVersion firmwareVersion;
    T_DjiCameraManagerFocusPosData focusPosData;
    T_DjiCameraManagerTapZoomPosData tapZoomPosData;

    USER_LOG_INFO("Camera manager sample start, cameraManagerSampleSelect %d", cameraManagerSampleSelect);
    DjiTest_WidgetLogAppend("Camera manager sample start");

    USER_LOG_INFO("--> Step 1: Init camera manager module");
    DjiTest_WidgetLogAppend("--> Step 1: Init camera manager module");
    returnCode = DjiCameraManager_Init();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Init camera manager failed, error code: 0x%08X\r\n", returnCode);
        goto exitCameraModule;
    }

    USER_LOG_INFO("--> Step 2: Get camera type and version");
    DjiTest_WidgetLogAppend("--> Step 2: Get camera type and version");
    returnCode = DjiCameraManager_GetCameraType(mountPosition, &cameraType);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Get mounted position %d camera's type failed, error code: 0x%08X\r\n",
                       mountPosition, returnCode);
        goto exitCameraModule;
    }
    USER_LOG_INFO("Mounted position %d camera's type is %s",
                  mountPosition,
                  s_cameraTypeStrList[DjiTest_CameraManagerGetCameraTypeIndex(cameraType)].cameraTypeStr);

    returnCode = DjiCameraManager_GetFirmwareVersion(mountPosition, &firmwareVersion);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Get mounted position %d camera's firmware version failed, error code: 0x%08X\r\n",
                       mountPosition, returnCode);
        goto exitCameraModule;
    }
    USER_LOG_INFO("Mounted position %d camera's firmware is V%02d.%02d.%02d.%02d\r\n", mountPosition,
                  firmwareVersion.firmware_version[0], firmwareVersion.firmware_version[1],
                  firmwareVersion.firmware_version[2], firmwareVersion.firmware_version[3]);

    switch (cameraManagerSampleSelect) {
        case E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_CAMERA_SHUTTER_SPEED: {
            USER_LOG_INFO("--> Function a: Set camera shutter speed to 1/100 s");
            DjiTest_WidgetLogAppend("--> Function a: Set camera shutter speed to 1/100 s");
            if (cameraType == DJI_CAMERA_TYPE_H20 || cameraType == DJI_CAMERA_TYPE_H20T ||
                cameraType == DJI_CAMERA_TYPE_H20N || cameraType == DJI_CAMERA_TYPE_M30 ||
                cameraType == DJI_CAMERA_TYPE_M30T || cameraType == DJI_CAMERA_TYPE_M3E ||
                cameraType == DJI_CAMERA_TYPE_M3T || cameraType == DJI_CAMERA_TYPE_M3D ||
                cameraType == DJI_CAMERA_TYPE_M3TD || cameraType == DJI_CAMERA_TYPE_H30 ||
                cameraType == DJI_CAMERA_TYPE_H30T) {
                USER_LOG_INFO("Set mounted position %d camera's exposure mode to manual mode.",
                              mountPosition);
                returnCode = DjiTest_CameraManagerSetExposureMode(mountPosition,
                                                                  DJI_CAMERA_MANAGER_EXPOSURE_MODE_EXPOSURE_MANUAL);
                if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    USER_LOG_ERROR("Set mounted position %d camera's exposure mode failed,"
                                   "error code: 0x%08X\r\n", mountPosition, returnCode);
                    goto exitCameraModule;
                }
            } else {
                USER_LOG_INFO("Set mounted position %d camera's exposure mode to shutter priority mode.",
                              mountPosition);
                returnCode = DjiTest_CameraManagerSetExposureMode(mountPosition,
                                                                  DJI_CAMERA_MANAGER_EXPOSURE_MODE_SHUTTER_PRIORITY);
                if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    USER_LOG_ERROR("Set mounted position %d camera's exposure mode failed,"
                                   "error code: 0x%08X\r\n", mountPosition, returnCode);
                    goto exitCameraModule;
                }
            }

            USER_LOG_INFO("Set mounted position %d camera's shutter speed to 1/100 s.",
                          mountPosition);
            returnCode = DjiTest_CameraManagerSetShutterSpeed(mountPosition,
                                                              DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_100);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Set mounted position %d camera's shutter speed failed,"
                               "error code: 0x%08X\r\n", mountPosition, returnCode);
                goto exitCameraModule;
            }
            break;
        }
        case E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_CAMERA_APERTURE: {
            USER_LOG_INFO("--> Function b: Set camera aperture to 400(F/4)");
            DjiTest_WidgetLogAppend("--> Function b: Set camera aperture to 400(F/4)");
            if (cameraType == DJI_CAMERA_TYPE_H20 || cameraType == DJI_CAMERA_TYPE_H20N
                || cameraType == DJI_CAMERA_TYPE_H20T || cameraType == DJI_CAMERA_TYPE_M30
                || cameraType == DJI_CAMERA_TYPE_M30T || cameraType == DJI_CAMERA_TYPE_M3E
                || cameraType == DJI_CAMERA_TYPE_M3T || cameraType == DJI_CAMERA_TYPE_M3D
                || cameraType == DJI_CAMERA_TYPE_M3TD || cameraType == DJI_CAMERA_TYPE_H30
                || cameraType == DJI_CAMERA_TYPE_H30T) {
                USER_LOG_INFO("Set mounted position %d camera's exposure mode to manual mode.",
                              mountPosition);
                returnCode = DjiTest_CameraManagerSetExposureMode(mountPosition,
                                                                  DJI_CAMERA_MANAGER_EXPOSURE_MODE_EXPOSURE_MANUAL);
                if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    USER_LOG_ERROR("Set mounted position %d camera's exposure mode failed,"
                                   "error code: 0x%08X\r\n", mountPosition, returnCode);
                    goto exitCameraModule;
                }
            } else {
                USER_LOG_INFO("Set mounted position %d camera's exposure mode to aperture priority mode.",
                              mountPosition);
                returnCode = DjiTest_CameraManagerSetExposureMode(mountPosition,
                                                                  DJI_CAMERA_MANAGER_EXPOSURE_MODE_APERTURE_PRIORITY);
                if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    USER_LOG_ERROR("Set mounted position %d camera's exposure mode failed,"
                                   "error code: 0x%08X\r\n", mountPosition, returnCode);
                    goto exitCameraModule;
                }
            }

            USER_LOG_INFO("Set mounted position %d camera's aperture to 400(F/4).",
                          mountPosition);
            returnCode = DjiTest_CameraManagerSetAperture(mountPosition, DJI_CAMERA_MANAGER_APERTURE_F_4);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Set mounted position %d camera's aperture failed,"
                               "error code: 0x%08X\r\n", mountPosition, returnCode);
                goto exitCameraModule;
            }
            break;
        }
        case E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_CAMERA_EV: {
            USER_LOG_INFO("--> Function c: Set camera ev value to +0.3ev");
            DjiTest_WidgetLogAppend("--> Function c: Set camera ev value to +0.3ev");
            USER_LOG_INFO("Set mounted position %d camera's exposure mode to auto mode.",
                          mountPosition);
            returnCode = DjiTest_CameraManagerSetExposureMode(mountPosition,
                                                              DJI_CAMERA_MANAGER_EXPOSURE_MODE_PROGRAM_AUTO);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Set mounted position %d camera's exposure mode failed,"
                               "error code: 0x%08X\r\n", mountPosition, returnCode);
                goto exitCameraModule;
            }

            USER_LOG_INFO("Set mounted position %d camera's ev value to +0.3ev.",
                          mountPosition);
            returnCode = DjiTest_CameraManagerSetEV(mountPosition, DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_P_0_3);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Set mounted position %d camera's EV failed,"
                               "error code: 0x%08X\r\n", mountPosition, returnCode);
                goto exitCameraModule;
            }
            break;
        }
        case E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_CAMERA_ISO: {
            USER_LOG_INFO("--> Function d: Set camera ISO value to 1600");
            DjiTest_WidgetLogAppend("--> Function d: Set camera ISO value to 1600");
            USER_LOG_INFO("Set mounted position %d camera's exposure mode to manual mode.",
                          mountPosition);
            returnCode = DjiTest_CameraManagerSetExposureMode(mountPosition,
                                                              DJI_CAMERA_MANAGER_EXPOSURE_MODE_EXPOSURE_MANUAL);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Set mounted position %d camera's exposure mode failed,"
                               "error code: 0x%08X\r\n", mountPosition, returnCode);
                goto exitCameraModule;
            }

            USER_LOG_INFO("Set mounted position %d camera's ISO value to 1600.",
                          mountPosition);
            returnCode = DjiTest_CameraManagerSetISO(mountPosition, DJI_CAMERA_MANAGER_ISO_1600);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Set mounted position %d camera's iso failed,"
                               "error code: 0x%08X\r\n", mountPosition, returnCode);
                goto exitCameraModule;
            }
            break;
        }
        case E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_CAMERA_FOCUS_POINT: {
            USER_LOG_INFO("--> Function e: Set camera focus point to (0.3,0.4)");
            DjiTest_WidgetLogAppend("--> Function e: Set camera focus point to (0.3,0.4)");
            focusPosData.focusX = 0.3f;
            focusPosData.focusY = 0.4f;
            returnCode = DjiTest_CameraManagerSetFocusPoint(mountPosition, focusPosData);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Set mounted position %d camera's focus point(0.8,0.8) failed,"
                               "error code: 0x%08X\r\n", mountPosition, returnCode);
                goto exitCameraModule;
            }
            break;
        }
        case E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_CAMERA_TAP_ZOOM_POINT: {
            USER_LOG_INFO("--> Function f: Set camera tap zoom point from (5x, 0.3m, 0.3m) to (4x, 0.8m, 0.7m)");
            DjiTest_WidgetLogAppend(
                "--> Function f: Set camera tap zoom point from (5x, 0.3m, 0.3m) to (4x, 0.8m, 0.7m)");
            tapZoomPosData.focusX = 0.3f;
            tapZoomPosData.focusY = 0.3f;
            returnCode = DjiTest_CameraManagerSetTapZoomPoint(mountPosition, 5, tapZoomPosData);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Set mounted position %d camera's tap zoom point(5, 0.3m,0.3m) failed,"
                               "error code: 0x%08X\r\n", mountPosition, returnCode);
                goto exitCameraModule;
            }

            USER_LOG_INFO("Sleep 5s...");
            osalHandler->TaskSleepMs(5000);

            tapZoomPosData.focusX = 0.8f;
            tapZoomPosData.focusY = 0.7f;
            returnCode = DjiTest_CameraManagerSetTapZoomPoint(mountPosition, 4, tapZoomPosData);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Set mounted position %d camera's tap zoom point(4, 0.8m,0.7m) failed,"
                               "error code: 0x%08X\r\n", mountPosition, returnCode);
                goto exitCameraModule;
            }
            break;
        }
        case E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_CAMERA_ZOOM_PARAM: {
            USER_LOG_INFO("--> Function g: Camera zoom from 10x to -5x");
            DjiTest_WidgetLogAppend("--> Function g: Camera zoom from 10x to -5x");
            returnCode = DjiTest_CameraManagerOpticalZoom(mountPosition, DJI_CAMERA_ZOOM_DIRECTION_IN,
                                                          10);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Set mounted position %d camera's optical zoom factor 10x failed,"
                               "error code: 0x%08X\r\n", mountPosition, returnCode);
                goto exitCameraModule;
            }
            USER_LOG_INFO("Sleep 8s...");
            osalHandler->TaskSleepMs(8000);

            returnCode = DjiTest_CameraManagerOpticalZoom(mountPosition, DJI_CAMERA_ZOOM_DIRECTION_OUT, 5);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Set mounted position %d camera's optical zoom factor -5x failed,"
                               "error code: 0x%08X\r\n", mountPosition, returnCode);
                goto exitCameraModule;
            }
            USER_LOG_INFO("Sleep 4s...");
            osalHandler->TaskSleepMs(4000);

            USER_LOG_INFO(
                "Mounted position %d camera start continuous zoom with zoom-out direction and normal zoom speed.",
                mountPosition);
            returnCode = DjiTest_CameraManagerStartContinuousZoom(mountPosition,
                                                                  DJI_CAMERA_ZOOM_DIRECTION_OUT,
                                                                  DJI_CAMERA_ZOOM_SPEED_NORMAL);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Mounted position %d camera start continuous zoom failed,"
                               "error code: 0x%08X\r\n", mountPosition, returnCode);
                goto exitCameraModule;
            }

            USER_LOG_INFO("Sleep 8s...");
            osalHandler->TaskSleepMs(8000);

            USER_LOG_INFO("Mounted position %d camera stop continuous zoom.", mountPosition);
            returnCode = DjiTest_CameraManagerStopContinuousZoom(mountPosition);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Mounted position %d camera stop continuous zoom failed,"
                               "error code: 0x%08X\r\n", mountPosition, returnCode);
                goto exitCameraModule;
            }
            break;
        }
        case E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SHOOT_SINGLE_PHOTO: {
            USER_LOG_INFO("--> Function h: Shoot Single photo");
            DjiTest_WidgetLogAppend("--> Function h: Shoot Single photo");
            returnCode = DjiTest_CameraManagerStartShootSinglePhoto(mountPosition);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Mounted position %d camera shoot single photo failed,"
                               "error code: 0x%08X\r\n", mountPosition, returnCode);
                goto exitCameraModule;
            }
            break;
        }
        case E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SHOOT_BURST_PHOTO: {
            USER_LOG_INFO("--> Function j: Shoot Burst photo with 5 pictures in one time");
            DjiTest_WidgetLogAppend("--> Function j: Shoot Burst photo with 5pictures in one time");
            returnCode = DjiTest_CameraManagerStartShootBurstPhoto(mountPosition,
                                                                   DJI_CAMERA_BURST_COUNT_5);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Mounted position %d camera shoot burst photo failed,"
                               "error code: 0x%08X\r\n", mountPosition, returnCode);
                goto exitCameraModule;
            }
            break;
        }
        case E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SHOOT_INTERVAL_PHOTO: {
            USER_LOG_INFO("--> Function k: Shoot Interval photo with 3s intervals in 15s");
            DjiTest_WidgetLogAppend("--> Function k: Shoot Interval photo with 3s intervals in 15s");
            T_DjiCameraPhotoTimeIntervalSettings intervalData;
            intervalData.captureCount = 255;
            intervalData.timeIntervalSeconds = 3;

            returnCode = DjiTest_CameraManagerStartShootIntervalPhoto(mountPosition, intervalData);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Mounted position %d camera shoot internal photo failed,"
                               "error code: 0x%08X\r\n", mountPosition, returnCode);
                goto exitCameraModule;
            }

            USER_LOG_INFO("Sleep 15s...");
            osalHandler->TaskSleepMs(15000);

            returnCode = DjiTest_CameraManagerStopShootPhoto(mountPosition);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Mounted position %d camera stop shoot photo failed,"
                               "error code: 0x%08X\r\n", mountPosition, returnCode);
                goto exitCameraModule;
            }
            break;
        }
        case E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_RECORD_VIDEO: {
            USER_LOG_INFO("--> Function l: Record video in next 10s");
            DjiTest_WidgetLogAppend("--> Function l: Record video in next 10s");
            returnCode = DjiTest_CameraManagerStartRecordVideo(mountPosition);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Mounted position %d camera start record video failed, error code: 0x%08X\r\n",
                               mountPosition, returnCode);
                goto exitCameraModule;
            }

            USER_LOG_INFO("Sleep 10s...");
            osalHandler->TaskSleepMs(10000);

            returnCode = DjiTest_CameraManagerStopRecordVideo(mountPosition);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Mounted position %d camera stop record video failed, error code: 0x%08X\r\n",
                               mountPosition, returnCode);
                goto exitCameraModule;
            }
            break;
        }
        case E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_DOWNLOAD_AND_DELETE_MEDIA_FILE: {
#ifdef SYSTEM_ARCH_LINUX
            DjiTest_CameraManagerMediaDownloadAndDeleteMediaFile(mountPosition);
#else
            USER_LOG_WARN("This feature does not support RTOS platform.");
#endif
            break;
        }

        case E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_DOWNLOAD_FILE_LIST_BY_SLICES: {
#ifdef SYSTEM_ARCH_LINUX
            DjiTest_CameraManagerMediaDownloadFileListBySlices(mountPosition);
#else
            USER_LOG_WARN("This feature does not support RTOS platform.");
#endif
            break;
        }
        case E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_THERMOMETRY: {
            returnCode = DjiTest_CameraManagerGetPointThermometryData(mountPosition);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Mounted position %d camera start point thermometry failed, error code: 0x%08X\r\n",
                               mountPosition, returnCode);
                goto exitCameraModule;
            }

            returnCode = DjiTest_CameraManagerGetAreaThermometryData(mountPosition);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Mounted position %d camera start area thermometry failed, error code: 0x%08X\r\n",
                               mountPosition, returnCode);
                goto exitCameraModule;
            }
            break;
        }
        case E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_GET_LIDAR_RANGING_INFO: {
            returnCode = DjiTest_CameraManagerGetLidarRangingInfo(mountPosition);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_WARN("Get lidar ranging info failed at position %d, error code: 0x%08X\r\n",
                              mountPosition, returnCode);
            }
            break;
        }
        case E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_IR_CAMERA_ZOOM_PARAM: {
            for (uint16_t factor = TEST_CAMERA_MIN_INFRARED_ZOOM_FACTOR;
                 factor <= TEST_CAMERA_MAX_INFRARED_ZOOM_FACTOR;
                 factor = factor * 2) {

                USER_LOG_INFO("Set infrared zoom factor to %d", factor);
                returnCode = DjiCameraManager_SetInfraredZoomParam(mountPosition, (dji_f32_t) factor);
                if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    USER_LOG_ERROR("Mounted position %d camera infrared zoom factor %d rfailed, error code: 0x%08X\r\n",
                                   mountPosition, factor, returnCode);
                    goto exitCameraModule;
                }
                osalHandler->TaskSleepMs(2000);
            }
            break;
        }
        case E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_NIGHT_SCENE_MODE: {
            E_DjiCameraManagerNightSceneMode nightSceneMode;
            T_DjiCameraManagerRangeList nightSceneModeRange;

            if (cameraType == DJI_CAMERA_TYPE_XT2 || cameraType == DJI_CAMERA_TYPE_XTS ||
                cameraType == DJI_CAMERA_TYPE_H20 || cameraType == DJI_CAMERA_TYPE_P1 ||
                cameraType == DJI_CAMERA_TYPE_L1 || cameraType == DJI_CAMERA_TYPE_L2 ||
                cameraType == DJI_CAMERA_TYPE_M3E || cameraType == DJI_CAMERA_TYPE_M3T ||
                cameraType == DJI_CAMERA_TYPE_M3D || cameraType == DJI_CAMERA_TYPE_M3TD) {
                USER_LOG_INFO("Camera type %s does not support night scene mode!",
                              s_cameraTypeStrList[DjiTest_CameraManagerGetCameraTypeIndex(cameraType)].cameraTypeStr);
                goto exitCameraModule;
            }

            USER_LOG_INFO("Step 1: Change camera stream source to zoom camera.");
            returnCode = DjiCameraManager_SetStreamSource(mountPosition, DJI_CAMERA_MANAGER_SOURCE_ZOOM_CAM);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                if (returnCode == DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND) {
                    USER_LOG_WARN("For camera type %d, it does not need to change stream source.\r\n", cameraType);
                }
                else {
                    goto exitCameraModule;
                }
            }

            USER_LOG_INFO("Step 2: Get night scene mode range.");
            returnCode = DjiCameraManager_GetNightSceneModeRange(mountPosition, &nightSceneModeRange);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Get night scene mode range of camera at position %d failed, return code 0x%08llX",
                                mountPosition, returnCode);
                goto exitCameraModule;
            }

            osalHandler->TaskSleepMs(10);
            printf("Supported night scene mode:");
            for(uint32_t i = 0; i < nightSceneModeRange.size; i++) {
                printf(" %d", nightSceneModeRange.nightSceneMode[i]);
            }
            printf("\n");

            USER_LOG_INFO("Step 3: Set ningh scene mode as enable.");
            returnCode = DjiCameraManager_SetNightSceneMode(mountPosition, DJI_CAMERA_MANAGER_NIGHT_SCENE_MODE_ENABLE);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Set ningh scene mode as enable failed at position %d, error code: 0x%08X\r\n",
                              mountPosition, returnCode);
                goto exitCameraModule;
            }

            returnCode = DjiCameraManager_GetNightSceneMode(mountPosition, &nightSceneMode);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Get night scene mode failed at position %d, error code: 0x%08X\r\n",
                              mountPosition, returnCode);
                goto exitCameraModule;
            }

            USER_LOG_INFO("Now the night scene mode is %d", nightSceneMode);

            USER_LOG_INFO("Sleep 2s...");
            osalHandler->TaskSleepMs(2000);

            USER_LOG_INFO("Step 4: Set night scene mode as auto.");
            if (cameraType == DJI_CAMERA_TYPE_H20T) {
                USER_LOG_WARN("Camera H20T does not support set night scene mode as auto");
            }
            else {
                returnCode = DjiCameraManager_SetNightSceneMode(mountPosition, DJI_CAMERA_MANAGER_NIGHT_SCENE_MODE_AUTO);
                if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    USER_LOG_ERROR("Set ningh scene mode as auto failed at position %d, error code: 0x%08X\r\n",
                                mountPosition, returnCode);
                    goto exitCameraModule;
                }

                returnCode = DjiCameraManager_GetNightSceneMode(mountPosition, &nightSceneMode);
                if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    USER_LOG_ERROR("Get night scene mode failed at position %d, error code: 0x%08X\r\n",
                                mountPosition, returnCode);
                    goto exitCameraModule;
                }

                USER_LOG_INFO("Now the night scene mode is %d", nightSceneMode);

                USER_LOG_INFO("Sleep 2s...");
                osalHandler->TaskSleepMs(2000);
            }

            USER_LOG_INFO("Step 5: Set ningt scene mode as disable.");
            returnCode = DjiCameraManager_SetNightSceneMode(mountPosition, DJI_CAMERA_MANAGER_NIGHT_SCENE_MODE_DISABLE);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Get lidar ranging info failed at position %d, error code: 0x%08X\r\n",
                              mountPosition, returnCode);
                goto exitCameraModule;
            }

            returnCode = DjiCameraManager_GetNightSceneMode(mountPosition, &nightSceneMode);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Get lidar ranging info failed at position %d, error code: 0x%08X\r\n",
                              mountPosition, returnCode);
                goto exitCameraModule;
            }

            USER_LOG_INFO("Now the night scene mode is %d", nightSceneMode);

            break;
        }
        case E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_CAPTURE_RECORDING_STREAMS: {
            T_DjiCameraManagerStreamList streamList;
            T_DjiCameraManagerStreamList getStreamList = {0};
            T_DjiCameraManagerRangeList streamStorageRange = {0};

            if (cameraType == DJI_CAMERA_TYPE_Z30 || cameraType == DJI_CAMERA_TYPE_XT2 ||
                cameraType == DJI_CAMERA_TYPE_XTS || cameraType == DJI_CAMERA_TYPE_P1 ||
                cameraType == DJI_CAMERA_TYPE_L1 || cameraType == DJI_CAMERA_TYPE_M3D) {
                USER_LOG_INFO("Camera type %s does not support set capture or recording stream(s) to storage.",
                                s_cameraTypeStrList[DjiTest_CameraManagerGetCameraTypeIndex(cameraType)].cameraTypeStr);
                goto exitCameraModule;
            }

            returnCode = DjiCameraManager_SetMode(mountPosition, DJI_CAMERA_MANAGER_WORK_MODE_SHOOT_PHOTO);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_WARN("Set camera work mode failed, position %d, error code: 0x%08X\r\n", mountPosition, returnCode);
                goto exitCameraModule;
            }

            returnCode = DjiCameraManager_GetStreamStorageRange(mountPosition,
                                                               &streamStorageRange);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                goto exitCameraModule;
            }

            for (uint32_t i = 0; i < streamStorageRange.size; i++) {
                streamList.streamStorage[i] = streamStorageRange.streamStorage[i];
            }
            streamList.size = streamStorageRange.size;

            USER_LOG_INFO("Step 1: Select all capture stream to be storaged.");
            returnCode = DjiCameraManager_SetCaptureRecordingStreams(mountPosition,
                                                                     DJI_CAMERA_MANAGER_CAPTURE_OR_RECORDING_CAPTURE,
                                                                     &streamList);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_WARN("Set capture storaged streams failed %d, error code: 0x%08X\r\n", mountPosition, returnCode);
                goto exitCameraModule;
            }

            USER_LOG_INFO("Step 2: Read back capture storage streams:");
            returnCode = DjiCameraManager_GetCaptureRecordingStreams(mountPosition,
                                                                     DJI_CAMERA_MANAGER_CAPTURE_OR_RECORDING_CAPTURE,
                                                                     &getStreamList);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_WARN("Get capture storage streams failed %d, error code: 0x%08X\r\n", mountPosition, returnCode);
                goto exitCameraModule;
            }

            for (uint32_t i = 0; i < getStreamList.size; i++) {
                USER_LOG_INFO("    stream %d is %d", i, getStreamList.streamStorage[i]);
            }

            USER_LOG_INFO("Sleep 2s...");
            osalHandler->TaskSleepMs(2000);

            USER_LOG_INFO("Step 3: Select all video stream to be storaged.");
            returnCode = DjiCameraManager_SetMode(mountPosition, DJI_CAMERA_MANAGER_WORK_MODE_RECORD_VIDEO);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_WARN("Failed %d, error code: 0x%08X\r\n", mountPosition, returnCode);
                goto exitCameraModule;
            }

            returnCode = DjiCameraManager_SetCaptureRecordingStreams(mountPosition,
                                                                     DJI_CAMERA_MANAGER_CAPTURE_OR_RECORDING_RECORDING,
                                                                     &streamList);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_WARN("Set recording storage streams failed %d, error code: 0x%08X\r\n", mountPosition, returnCode);
                goto exitCameraModule;
            }

            USER_LOG_INFO("Step 4: Read back video storage streams:");
            returnCode = DjiCameraManager_GetCaptureRecordingStreams(mountPosition,
                                                                     DJI_CAMERA_MANAGER_CAPTURE_OR_RECORDING_RECORDING,
                                                                     &getStreamList);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_WARN("Get capture storage streams failed %d, error code: 0x%08X\r\n", mountPosition, returnCode);
                goto exitCameraModule;
            }

            for (uint32_t i = 0; i < getStreamList.size; i++) {
                USER_LOG_INFO("    stream %d is %d", i, getStreamList.streamStorage[i]);
            }

            streamList.streamStorage[0] = streamStorageRange.streamStorage[0];
            streamList.size = 1;

            USER_LOG_INFO("Sleep 2s...");
            osalHandler->TaskSleepMs(2000);

            USER_LOG_INFO("Step 5: Select default video stream to storage.");
            returnCode = DjiCameraManager_SetMode(mountPosition, DJI_CAMERA_MANAGER_WORK_MODE_RECORD_VIDEO);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_WARN("Failed %d, error code: 0x%08X\r\n", mountPosition, returnCode);
                goto exitCameraModule;
            }

            returnCode = DjiCameraManager_SetCaptureRecordingStreams(mountPosition,
                                                                     DJI_CAMERA_MANAGER_CAPTURE_OR_RECORDING_RECORDING,
                                                                     &streamList);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_WARN("Set recording storage streams failed %d, error code: 0x%08X\r\n", mountPosition, returnCode);
                goto exitCameraModule;
            }

            USER_LOG_INFO("Step 6: Read back video storage streams:");
            returnCode = DjiCameraManager_GetCaptureRecordingStreams(mountPosition,
                                                                     DJI_CAMERA_MANAGER_CAPTURE_OR_RECORDING_RECORDING,
                                                                     &getStreamList);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_WARN("Get capture storage streams failed %d, error code: 0x%08X\r\n", mountPosition, returnCode);
                goto exitCameraModule;
            }

            for (uint32_t i = 0; i < getStreamList.size; i++) {
                USER_LOG_INFO("    stream %d is %d", i, getStreamList.streamStorage[i]);
            }

            break;
        }
        case E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SHOW_STORAGE_INFO: {
            T_DjiCameraManagerStorageInfo storageInfo;

            for (uint32_t i = 0; i < 30; i++) {
                returnCode = DjiCameraManager_GetStorageInfo(mountPosition, &storageInfo);
                if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    USER_LOG_INFO("Get storage info failed!");
                    goto exitCameraModule;
                }

                USER_LOG_INFO("total capacity: %d, remainCapcity: %d", storageInfo.totalCapacity, storageInfo.remainCapacity);

                osalHandler->TaskSleepMs(200);
            }

            break;
        }
        case E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_FORMAT_SD_CARD: {
            returnCode = DjiCameraManager_FormatStorage(mountPosition);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Faile to Format SD card.");
                goto exitCameraModule;
            }
            USER_LOG_INFO("Format SD card successfully!");
            break;
        }
        case E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_LINK_ZOOM: {

            USER_LOG_INFO("Set synchronized split screen zoom enabled");
            returnCode = DjiCameraManager_SetSynchronizedSplitScreenZoomEnabled(mountPosition, true);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                goto exitCameraModule;
            }

            USER_LOG_INFO("Sleep 5s...");
            osalHandler->TaskSleepMs(5000);

            USER_LOG_INFO("Set synchronized split screen zoom disabled");
            returnCode = DjiCameraManager_SetSynchronizedSplitScreenZoomEnabled(mountPosition, false);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                goto exitCameraModule;
            }

            break;
        }
        case E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_USER_CUSTOM_DIR_FILE_NAME: {
            uint8_t dirName[240] = {0};
            uint32_t dirNameSize = 0;
            uint8_t fileName[240] = {0};
            uint32_t fileNameSize = 0;
            uint8_t getNameString[100] = {0};
            uint32_t getNameStringSize;

            osalHandler->TaskSleepMs(5);
            printf("Input expand directory name: ");
            scanf("%s", dirName);
            dirNameSize = strlen((const char *)dirName);

            returnCode = DjiCameraManager_SetCustomExpandName(mountPosition,
                                                              DJI_CAMERA_MANAGER_EXPAND_NAME_TYPE_DIR,
                                                              dirName,
                                                              dirNameSize);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Set Custom expand directory name failed");
                goto exitCameraModule;
            }

            if (cameraType != DJI_CAMERA_TYPE_L1) {
                osalHandler->TaskSleepMs(5);
                printf("Input expand file name: ");
                scanf("%s", fileName);
                fileNameSize = strlen((const char *)fileName);

                returnCode = DjiCameraManager_SetCustomExpandName(mountPosition,
                                                                DJI_CAMERA_MANAGER_EXPAND_NAME_TYPE_FILE,
                                                                fileName,
                                                                fileNameSize);
                if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    USER_LOG_ERROR("Set Custom expand file name failed");
                    goto exitCameraModule;
                }
            }


            getNameStringSize = sizeof(getNameString) - 1;
            returnCode =  DjiCameraManager_GetCustomExpandName(mountPosition,
                                                               DJI_CAMERA_MANAGER_EXPAND_NAME_TYPE_DIR,
                                                               getNameString,
                                                               &getNameStringSize);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Get Custom expand directory name failed, stat = 0x%08llX", returnCode);
                goto exitCameraModule;
            }
            getNameString[getNameStringSize] = '\0';
            USER_LOG_INFO("Latest directory name: %s", getNameString);

            if (cameraType != DJI_CAMERA_TYPE_L1) {
                getNameStringSize = sizeof(getNameString) - 1;
                returnCode =  DjiCameraManager_GetCustomExpandName(mountPosition,
                                                                DJI_CAMERA_MANAGER_EXPAND_NAME_TYPE_FILE,
                                                                getNameString,
                                                                &getNameStringSize);
                if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    USER_LOG_ERROR("Get Custom expand file name failed");
                    goto exitCameraModule;
                }
                getNameString[getNameStringSize] = '\0';
                USER_LOG_INFO("Latest file name: %s", getNameString);
            }

            break;
        }
        case E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_RESET_CAMERA_SETTINGS: {
            returnCode = DjiCameraManager_ResetCameraSettings(mountPosition);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("reset camera at position %d failed", mountPosition);
                goto exitCameraModule;
            }

            USER_LOG_INFO("Reset camera settings success!");

            break;
        }
        case E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_AE_LOCK_MODE: {
            bool enable;

            if (cameraType == DJI_CAMERA_TYPE_H20 || cameraType == DJI_CAMERA_TYPE_H20T ||
                cameraType == DJI_CAMERA_TYPE_H20N || cameraType == DJI_CAMERA_TYPE_M30 ||
                cameraType == DJI_CAMERA_TYPE_M30T || cameraType == DJI_CAMERA_TYPE_M3E ||
                cameraType == DJI_CAMERA_TYPE_M3T || cameraType == DJI_CAMERA_TYPE_H30 ||
                cameraType == DJI_CAMERA_TYPE_H30T) {
                returnCode = DjiCameraManager_SetStreamSource(mountPosition, DJI_CAMERA_MANAGER_SOURCE_ZOOM_CAM);
                if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    USER_LOG_WARN("Change camera stream source to zoom camera failed at position %d, error code: 0x%08X\r\n",
                                mountPosition, returnCode);
                    goto exitCameraModule;
                }
            }

            USER_LOG_INFO("Try to set ae locked...");
            returnCode = DjiCameraManager_SetAELockEnabled(mountPosition, true);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Set camera ae lock mode at position %d failed", mountPosition);
                goto exitCameraModule;
            }

            osalHandler->TaskSleepMs(2000);
            USER_LOG_INFO("Sleep 2s...");

            returnCode = DjiCameraManager_GetAELockEnabled(mountPosition, &enable);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Get camera ae lock mode at position %d failed", mountPosition);
                goto exitCameraModule;
            }

            USER_LOG_INFO("The camera ae lock mode now is %d", enable);

            USER_LOG_INFO("Try to set ae unlocked...");
            returnCode = DjiCameraManager_SetAELockEnabled(mountPosition, false);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Set camera ae lock mode at position %d failed", mountPosition);
                goto exitCameraModule;
            }

            osalHandler->TaskSleepMs(2000);
            USER_LOG_INFO("Sleep 2s...");

            if (cameraType == DJI_CAMERA_TYPE_L1 || cameraType == DJI_CAMERA_TYPE_P1) {
                USER_LOG_INFO("Camera type %d does not support get AE lock mode, please check by yourself.",
                               cameraType);
            }
            else {
                returnCode = DjiCameraManager_GetAELockEnabled(mountPosition, &enable);
                if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    USER_LOG_ERROR("Get camera ae lock mode at position %d failed", mountPosition);
                    goto exitCameraModule;
                }

                USER_LOG_INFO("The camera ae lock mode now is %d", enable);
            }

            break;
        }
        case E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_FOCUS_RING_VALUE: {
            uint16_t focusRingValue;
            T_DjiCameraManagerRangeList focusRingRange;

            if (cameraType == DJI_CAMERA_TYPE_H20 || cameraType == DJI_CAMERA_TYPE_H20N ||
                cameraType == DJI_CAMERA_TYPE_H20T || cameraType == DJI_CAMERA_TYPE_M30 ||
                cameraType == DJI_CAMERA_TYPE_M30T || cameraType == DJI_CAMERA_TYPE_M3E ||
                cameraType == DJI_CAMERA_TYPE_M3T || cameraType == DJI_CAMERA_TYPE_H30 ||
                cameraType == DJI_CAMERA_TYPE_H30T) {
                USER_LOG_INFO("Set camera stream source to zoom camera.");
                returnCode = DjiCameraManager_SetStreamSource(mountPosition, DJI_CAMERA_MANAGER_SOURCE_ZOOM_CAM);
                if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    USER_LOG_WARN("Change camera stream source to zoom camera failed at position %d, error code: 0x%08X\r\n",
                                mountPosition, returnCode);
                    goto exitCameraModule;
                }
            }

            USER_LOG_INFO("Set camera's focus mode to manual mode.");
            returnCode = DjiCameraManager_SetFocusMode(mountPosition, DJI_CAMERA_MANAGER_FOCUS_MODE_MANUAL);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Set mounted position %d camera's focus mode(%d) failed,"
                            " error code :0x%08X.", mountPosition, DJI_CAMERA_MANAGER_FOCUS_MODE_MANUAL,
                            returnCode);
                goto exitCameraModule;
            }

            returnCode = DjiCameraManager_GetFocusRingRange(mountPosition, &focusRingRange);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Get range failed!");
                goto exitCameraModule;
            }

            USER_LOG_INFO("Focus ring vlaue range: %d to %d.", focusRingRange.minValue, focusRingRange.maxValue);

            osalHandler->TaskSleepMs(10);
            printf("Input focus ring value to set: ");
            scanf("%hd", &focusRingValue);

            USER_LOG_INFO("Try to set focus ring value as %hd", focusRingValue);
            returnCode = DjiCameraManager_SetFocusRingValue(mountPosition, focusRingValue);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Set camera focus ring value at position %d failed", mountPosition);
                goto exitCameraModule;
            }

            osalHandler->TaskSleepMs(2000);

            if (cameraType != DJI_CAMERA_TYPE_Z30) {
                returnCode = DjiCameraManager_GetFocusRingValue(mountPosition, &focusRingValue);
                if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    USER_LOG_ERROR("Get camera focus ring value at position %d failed", mountPosition);
                    goto exitCameraModule;
                }
                USER_LOG_INFO("Current focus ring value is %d", focusRingValue);
            }


            break;
        }
        case E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_CONNECT_STATUS_TEST: {
            bool connectStatus;
            uint32_t loopTime = 10;

            osalHandler->TaskSleepMs(10);
            printf("Please input checking time: ");
            scanf("%d", &loopTime);

            do {
                returnCode = DjiCameraManager_GetCameraConnectStatus(mountPosition,
                                                                     &connectStatus);
                if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    goto exitCameraModule;
                }

                USER_LOG_INFO("Camera's connect state is %d", connectStatus);

                osalHandler->TaskSleepMs(1500);
            } while(loopTime--);

            break;
        }
        case E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_GET_PHOTO_VIDEO_PARAM: {
            E_DjiCameraManagerPhotoRatio photoRatio;
            E_DjiCameraManagerPhotoStorageFormat photoFormat;
            E_DjiCameraManagerVideoStorageFormat videoFormat;
            T_DjiCameraManagerRangeList photoRatioRange;
            T_DjiCameraManagerRangeList photoFormatRange;
            T_DjiCameraManagerRangeList videoFormatRange;
            T_DjiCameraManagerVideoFormat videoParam;

            returnCode = DjiCameraManager_GetPhotoRatioRange(mountPosition, &photoRatioRange);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Get range failed!");
                goto exitCameraModule;
            }

            osalHandler->TaskSleepMs(5);
            printf("photo ratio range:");
            for (uint32_t i = 0; i < photoRatioRange.size; i++) {
                printf(" %d", photoRatioRange.photoRatioFormat[i]);
            }
            printf("\n");

            returnCode = DjiCameraManager_GetPhotoStorageFormatRange(mountPosition, &photoFormatRange);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Get range failed!");
                goto exitCameraModule;
            }

            osalHandler->TaskSleepMs(5);
            printf("photo storage format range:");
            for (uint32_t i = 0; i < photoFormatRange.size; i++) {
                printf(" %d", photoFormatRange.photoStorageFormat[i]);
            }
            printf("\n");

            returnCode = DjiCameraManager_GetVideoFormatRange(mountPosition, &videoFormatRange);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Get range failed!");
                goto exitCameraModule;
            }

            osalHandler->TaskSleepMs(5);
            printf("video storage format range:");
            for (uint32_t i = 0; i < videoFormatRange.size; i++) {
                printf(" %d", videoFormatRange.videoStorageFormat[i]);
            }
            printf("\n");

            USER_LOG_INFO("Set camera work mode as shoot photo.");
            returnCode = DjiCameraManager_SetMode(mountPosition, DJI_CAMERA_MANAGER_WORK_MODE_SHOOT_PHOTO);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS &&
                returnCode != DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND) {
                USER_LOG_ERROR("set mounted position %d camera's work mode as shoot-photo mode failed,"
                            " error code :0x%08X", mountPosition, returnCode);
                return returnCode;
            }

            USER_LOG_INFO("Set photo ratio to type %d", photoRatioRange.photoRatioFormat[0]);
            returnCode = DjiCameraManager_SetPhotoRatio(mountPosition,
                                                        photoRatioRange.photoRatioFormat[0]);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
            {
                USER_LOG_ERROR("Set photo ratio failed.");
                goto exitCameraModule;
            }

            if (cameraType == DJI_CAMERA_TYPE_XTS) {
                USER_LOG_INFO("Camera type %d does not support get photo ratio.", cameraType);
            }
            else {
                returnCode = DjiCameraManager_GetPhotoRatio(mountPosition,
                                                            &photoRatio);
                if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    USER_LOG_ERROR("Get camera photo ratio at position %d failed, ret = 0x%08llX",
                                    mountPosition, returnCode);
                    goto exitCameraModule;
                }
                USER_LOG_INFO("Current photo ratio type: %d", photoRatio);
            }

            USER_LOG_INFO("Set photo storage format to type %d", photoFormatRange.photoStorageFormat[0]);
            returnCode = DjiCameraManager_SetPhotoFormat(mountPosition, photoFormatRange.photoStorageFormat[0]);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
            {
                USER_LOG_ERROR("Set photo storage format failed.");
                goto exitCameraModule;
            }

            if (cameraType == DJI_CAMERA_TYPE_XTS) {
                USER_LOG_INFO("Camera type %d does not support get photo storage format.", cameraType);
            }
            else {
                returnCode = DjiCameraManager_GetPhotoFormat(mountPosition, &photoFormat);
                if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    USER_LOG_ERROR("Get camera photo storage format at position %d failed, ret = 0x%08llX",
                                    mountPosition, returnCode);
                    goto exitCameraModule;
                }
                USER_LOG_INFO("Current photo storage format type: %d", photoFormat);
            }

            USER_LOG_INFO("Sleep 2s...");
            osalHandler->TaskSleepMs(2000);

            USER_LOG_INFO("Set camera work mode as record video.");
            returnCode = DjiCameraManager_SetMode(mountPosition, DJI_CAMERA_MANAGER_WORK_MODE_RECORD_VIDEO);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS &&
                returnCode != DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND) {
                USER_LOG_ERROR("set mounted position %d camera's work mode as shoot-photo mode failed,"
                            " error code :0x%08X", mountPosition, returnCode);
                return returnCode;
            }

            USER_LOG_INFO("Set video storage format as type %d", videoFormatRange.videoStorageFormat[0]);
            returnCode = DjiCameraManager_SetVideoStorageFormat(mountPosition, videoFormatRange.videoStorageFormat[0]);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
            {
                USER_LOG_ERROR("Set video storage format failed.");
                goto exitCameraModule;
            }

            if (cameraType == DJI_CAMERA_TYPE_XTS) {
                USER_LOG_INFO("Camera type %d does not support get video storage format, video resolution and frame rate.",
                              cameraType);
            }
            else {
                returnCode = DjiCameraManager_GetVideoFormat(mountPosition, &videoFormat);
                if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    USER_LOG_ERROR("Get camera video storage format at position %d failed, ret = 0x%08llX",
                                    mountPosition, returnCode);
                    goto exitCameraModule;
                }
                USER_LOG_INFO("Current video stroage format: %d", videoFormat);

                returnCode = DjiCameraManager_GetVideoResolutionFrameRate(mountPosition,
                                                                        &videoParam);
                if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    USER_LOG_ERROR("Get camera video resolution and framerate failed at position %d failed",
                                    mountPosition);
                    goto exitCameraModule;
                }
                USER_LOG_INFO("resolution: %d, frame rate: %d", videoParam.videoResolution, videoParam.videoFrameRate);
            }

            break;
        }
        case E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_METERING_MODE: {
            E_DjiCameraManagerMeteringMode meteringMode;

            USER_LOG_INFO("Set metering mode as %d", DJI_CAMERA_MANAGER_METERING_MODE_AVERAGE);
            returnCode = DjiCameraManager_SetMeteringMode(mountPosition, DJI_CAMERA_MANAGER_METERING_MODE_AVERAGE);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Set camera metering mode %d failed", DJI_CAMERA_MANAGER_METERING_MODE_AVERAGE);
                goto exitCameraModule;
            }

            osalHandler->TaskSleepMs(200);

            returnCode = DjiCameraManager_GetMeteringMode(mountPosition, &meteringMode);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Get camera metering mode failed.");
                goto exitCameraModule;
            }
            USER_LOG_INFO("Current metering mode is %d", meteringMode);

            USER_LOG_INFO("Sleep 2s...");
            osalHandler->TaskSleepMs(2000);

            USER_LOG_INFO("Set metering mode as %d", DJI_CAMERA_MANAGER_METERING_MODE_SPOT);
            returnCode = DjiCameraManager_SetMeteringMode(mountPosition, DJI_CAMERA_MANAGER_METERING_MODE_SPOT);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Set camera metering mode %d failed", DJI_CAMERA_MANAGER_METERING_MODE_SPOT);
                goto exitCameraModule;
            }

            osalHandler->TaskSleepMs(200);

            returnCode = DjiCameraManager_GetMeteringMode(mountPosition, &meteringMode);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Get camera metering mode failed.");
                goto exitCameraModule;
            }
            USER_LOG_INFO("Current metering mode is %d", meteringMode);

            USER_LOG_INFO("Sleep 2s...");
            osalHandler->TaskSleepMs(2000);

            USER_LOG_INFO("Set metering mode as %d", DJI_CAMERA_MANAGER_METERING_MODE_CENTRAL);
            returnCode = DjiCameraManager_SetMeteringMode(mountPosition, DJI_CAMERA_MANAGER_METERING_MODE_CENTRAL);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Set camera metering mode %d failed", DJI_CAMERA_MANAGER_METERING_MODE_CENTRAL);
                goto exitCameraModule;
            }

            osalHandler->TaskSleepMs(200);

            returnCode = DjiCameraManager_GetMeteringMode(mountPosition, &meteringMode);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Get camera metering mode failed.");
                goto exitCameraModule;
            }
            USER_LOG_INFO("Current metering mode is %d", meteringMode);

            break;
        }
        case E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_METERING_POINT: {
            int32_t x, y;
            uint8_t getX, getY;
            uint8_t horizonRegionNum;
            uint8_t viticalRegionNum;

            if (cameraType == DJI_CAMERA_TYPE_XTS) {
                USER_LOG_INFO("Camera type %d does not support to set metering point.", cameraType);
                goto exitCameraModule;
            }

            returnCode = DjiCameraManager_GetMeteringPointRegionRange(mountPosition, &horizonRegionNum, &viticalRegionNum);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Get metering point region range failed!");
                goto exitCameraModule;
            }

            USER_LOG_INFO("region range: horizon %d, vitical %d", horizonRegionNum, viticalRegionNum);

            osalHandler->TaskSleepMs(5);
            printf("Input meterting point (x, y) you want to set: ");
            scanf("%d %d", &x, &y);

            USER_LOG_INFO("Try to set metering point as (%d, %d)", (uint8_t)x, (uint8_t)y);
            returnCode = DjiCameraManager_SetMeteringPoint(mountPosition, x, y);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Set metering point failed");
                goto exitCameraModule;
            }

            osalHandler->TaskSleepMs(500);

            returnCode = DjiCameraManager_GetMeteringPoint(mountPosition, &getX, &getY);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Get metering point failed");
                goto exitCameraModule;
            }

            USER_LOG_INFO("Current metering point: (%d, %d)", getX, getY);

            break;
        }
        case E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_FFC_MODE_AND_TRRIGER: {
            uint32_t ffcMode;

            if (cameraType == DJI_CAMERA_TYPE_Z30 || cameraType == DJI_CAMERA_TYPE_XT2 ||
                cameraType == DJI_CAMERA_TYPE_H20 || cameraType == DJI_CAMERA_TYPE_P1 ||
                cameraType == DJI_CAMERA_TYPE_L1 || cameraType == DJI_CAMERA_TYPE_M30 ||
                cameraType == DJI_CAMERA_TYPE_M3E || cameraType == DJI_CAMERA_TYPE_M3D ||
                cameraType == DJI_CAMERA_TYPE_L2 || cameraType == DJI_CAMERA_TYPE_H30) {
                USER_LOG_WARN("Camera type %s don't support FFC function.",
                              s_cameraTypeStrList[DjiTest_CameraManagerGetCameraTypeIndex(cameraType)].cameraTypeStr);
                goto exitCameraModule;
            }

            osalHandler->TaskSleepMs(10);
            printf("Input mode to set FFC (0 manual, 1 auto):");
            scanf("%d", &ffcMode);

            if (cameraType != DJI_CAMERA_TYPE_XTS) {
                USER_LOG_INFO("Set camera stream source to infrared camera.");
                returnCode = DjiCameraManager_SetStreamSource(mountPosition, DJI_CAMERA_MANAGER_SOURCE_IR_CAM);
                if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    USER_LOG_ERROR("Change camera stream source to infrared camera failed at position %d, error code: 0x%08X\r\n",
                                mountPosition, returnCode);
                    goto exitCameraModule;
                }
            }

            returnCode = DjiCameraManager_SetFfcMode(mountPosition, (E_DjiCameraManagerFfcMode)ffcMode);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Set FFC mode %d failed, camera position %d, error code 0x%08llX",
                            ffcMode, mountPosition, returnCode);
                goto exitCameraModule;
            }

            returnCode = DjiCameraManager_TriggerFfc(mountPosition);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Trigger FFC mode, camera position %d, error code 0x%08llX",
                              mountPosition, returnCode);
                goto exitCameraModule;
            }

            USER_LOG_INFO("Trriger FFC successfully!");

            break;
        }
        case E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_GAIN_MODE: {
            uint32_t gainMode;
            T_DjiCameraManagerIrTempMeterRange tempRange = {0};

            if (cameraType == DJI_CAMERA_TYPE_Z30 || cameraType == DJI_CAMERA_TYPE_XT2 ||
                cameraType == DJI_CAMERA_TYPE_H20 || cameraType == DJI_CAMERA_TYPE_P1 ||
                cameraType == DJI_CAMERA_TYPE_L1 || cameraType == DJI_CAMERA_TYPE_M30 ||
                cameraType == DJI_CAMERA_TYPE_M3E || cameraType == DJI_CAMERA_TYPE_M3D ||
                cameraType == DJI_CAMERA_TYPE_L2 || cameraType == DJI_CAMERA_TYPE_H30) {
                USER_LOG_WARN("Camera type %s don't support infrared function.",
                              s_cameraTypeStrList[DjiTest_CameraManagerGetCameraTypeIndex(cameraType)].cameraTypeStr);
                goto exitCameraModule;
            }

            osalHandler->TaskSleepMs(10);
            printf("Input gain mode to set (1 low, 2 high):");
            scanf("%d", &gainMode);

            if (cameraType != DJI_CAMERA_TYPE_XTS) {
                USER_LOG_INFO("Set camera stream source to infrared camera.");
                returnCode = DjiCameraManager_SetStreamSource(mountPosition, DJI_CAMERA_MANAGER_SOURCE_IR_CAM);
                if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    USER_LOG_ERROR("Change camera stream source to infrared camera failed at position %d, error code: 0x%08X\r\n",
                                mountPosition, returnCode);
                    goto exitCameraModule;
                }
            }

            USER_LOG_INFO("Get temperature range of different gain mode...");
            returnCode = DjiCameraManager_GetInfraredCameraGainModeTemperatureRange(mountPosition, &tempRange);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Fail to get infrared mode temperature range. position %d, error code 0x%08llX",
                              mountPosition, returnCode);
                goto exitCameraModule;
            }

            USER_LOG_INFO("low_min: %.2f, low_max: %.2f, high_min: %.2f, high_max: %.2f",
                          tempRange.lowGainTempMin, tempRange.lowGainTempMax,
                          tempRange.highGainTempMin, tempRange.highGainTempMax);

            returnCode = DjiCameraManager_SetInfraredCameraGainMode(mountPosition, (E_DjiCameraManagerIrGainMode)gainMode);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_INFO("Fail to set infrared camera gain mode. position %d, error code 0x%08llX",
                              mountPosition, returnCode);
                goto exitCameraModule;
            }

            USER_LOG_INFO("Set gain mode to %d successfully!", (E_DjiCameraManagerIrGainMode)gainMode);

            break;
        }
        case E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_GET_CAMERA_STATUS: {
            E_DjiCameraManagerCapturingState capturingState;
            E_DjiCameraManagerRecordingState recordingState;
            uint16_t recordingTime;
            uint8_t remainingTime;

            if (cameraType == DJI_CAMERA_TYPE_L1 || cameraType == DJI_CAMERA_TYPE_P1) {
                USER_LOG_INFO("Camera type %d does not support to get camera stauts such as "
                              "capturing state, recording state.", cameraType);
                goto exitCameraModule;
            }

            for (uint32_t i = 0; i < 30; i++) {
                returnCode = DjiCameraManager_GetCapturingState(mountPosition, &capturingState);
                if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    USER_LOG_INFO("Get capturing state failed!");
                    goto exitCameraModule;
                }

                returnCode = DjiCameraManager_GetRecordingState(mountPosition, &recordingState);
                if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    USER_LOG_INFO("Get recording state failed!");
                    goto exitCameraModule;
                }

                returnCode = DjiCameraManager_GetRecordingTime(mountPosition, &recordingTime);
                if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    USER_LOG_INFO("Get recording time failed!");
                    goto exitCameraModule;
                }

                returnCode = DjiCameraManager_GetIntervalShootingRemainTime(mountPosition, &remainingTime);
                if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    USER_LOG_INFO("Get interval shooting remain time failed!");
                    goto exitCameraModule;
                }

                USER_LOG_INFO("cap_state: %d, rec_state: %d, rec_time: %d, cap_remain: %d",
                            capturingState,
                            recordingState,
                            recordingTime,
                            remainingTime);

                osalHandler->TaskSleepMs(200);
            }

            break;
        }
#ifndef SYSTEM_ARCH_RTOS
        case E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SUBSCRIBE_POINT_CLOUD: {
            returnCode = DjiTest_CameraManagerSubscribePointCloud(mountPosition);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Fail to subscribe point cloud. position %d, error code 0x%08llX",
                               mountPosition, returnCode);
                goto exitCameraModule;
            }
            break;
        }
#endif
        default: {
            USER_LOG_ERROR("There is no valid command input!");
            break;
        }
    }

exitCameraModule:
    returnCode = DjiCameraManager_DeInit();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Camera manager deinit failed ,error code :0x%08X", returnCode);
    }

    USER_LOG_INFO("Camera manager sample end");
    DjiTest_WidgetLogAppend("Camera manager sample end");
    return returnCode;
}

/* Private functions definition-----------------------------------------------*/
static uint8_t DjiTest_CameraManagerGetCameraTypeIndex(E_DjiCameraType cameraType)
{
    uint8_t i;

    for (i = 0; i < sizeof(s_cameraTypeStrList) / sizeof(s_cameraTypeStrList[0]); i++) {
        if (s_cameraTypeStrList[i].cameraType == cameraType) {
            return i;
        }
    }

    return 0;
}

static T_DjiReturnCode DjiTest_CameraManagerMediaDownloadAndDeleteMediaFile(E_DjiMountPosition position)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    uint16_t downloadCount = 0;
    uint16_t subFileDownloadCount = 0;
    int i = 0, j= 0;

    s_nextDownloadFileIndex = 0;
    returnCode = DjiCameraManager_RegDownloadFileDataCallback(position, DjiTest_CameraManagerDownloadFileDataCallback);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Register download file data callback failed, error code: 0x%08X.", returnCode);
        return returnCode;
    }

    returnCode = DjiCameraManager_ObtainDownloaderRights(position);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Obtain downloader rights failed, error code: 0x%08X.", returnCode);
    }

    returnCode = DjiCameraManager_DownloadFileList(position, &s_meidaFileList);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Download file list failed, error code: 0x%08X.", returnCode);
        return returnCode;
    }

    if (s_meidaFileList.totalCount > 0) {
        downloadCount = s_meidaFileList.totalCount;
        subFileDownloadCount = s_meidaFileList.fileListInfo->subFileListTotalNum;
        printf(
            "\033[1;33;40m -> Download file list finished, total media file count is %d "
                   "the following %d is list details: \033[0m\r\n",
            s_meidaFileList.totalCount, downloadCount);

        for (i = 0; i < downloadCount; ++i) {
                printf(
                    "\033[1;32;40m ### Media file_%03d name: %s, index: %d, time:%04d-%02d-%02d_%02d:%02d:%02d, size: %.2f KB, type: %d \033[0m\r\n",
                    i, s_meidaFileList.fileListInfo[i].fileName,
                    s_meidaFileList.fileListInfo[i].fileIndex,
                    s_meidaFileList.fileListInfo[i].createTime.year,
                    s_meidaFileList.fileListInfo[i].createTime.month,
                    s_meidaFileList.fileListInfo[i].createTime.day,
                    s_meidaFileList.fileListInfo[i].createTime.hour,
                    s_meidaFileList.fileListInfo[i].createTime.minute,
                    s_meidaFileList.fileListInfo[i].createTime.second,
                    (dji_f32_t) s_meidaFileList.fileListInfo[i].fileSize / (1024),
                    s_meidaFileList.fileListInfo[i].type);
            if (s_meidaFileList.fileListInfo[i].type == DJI_CAMERA_FILE_TYPE_LDRT) {
                for (j = 0; j < s_meidaFileList.fileListInfo[i].subFileListTotalNum; j++) {
                    printf(
                        "\033[1;32;40m ### Media file_%03d, sub_file_%03d,  name: %s, index: %d, time:%04d-%02d-%02d_%02d:%02d:%02d, size: %.2f KB, sub type: %d\033[0m\r\n",
                        i, j, s_meidaFileList.fileListInfo[i].subFileListInfo[j].fileName,
                        s_meidaFileList.fileListInfo[i].subFileListInfo[j].fileIndex,
                        s_meidaFileList.fileListInfo[i].subFileListInfo[j].createTime.year,
                        s_meidaFileList.fileListInfo[i].subFileListInfo[j].createTime.month,
                        s_meidaFileList.fileListInfo[i].subFileListInfo[j].createTime.day,
                        s_meidaFileList.fileListInfo[i].subFileListInfo[j].createTime.hour,
                        s_meidaFileList.fileListInfo[i].subFileListInfo[j].createTime.minute,
                        s_meidaFileList.fileListInfo[i].subFileListInfo[j].createTime.second,
                        (dji_f32_t) s_meidaFileList.fileListInfo[i].subFileListInfo[j].fileSize / (1024),
                        s_meidaFileList.fileListInfo[i].subFileListInfo[j].type);
                }
            }
        }
        printf("\r\n");

        osalHandler->TaskSleepMs(1000);

        downloadCount =  s_meidaFileList.totalCount;

        for (i = 0; i < downloadCount; ++i) {
            redownload:
            if (i != s_nextDownloadFileIndex) {
                i = s_nextDownloadFileIndex;
            }
            returnCode = DjiCameraManager_DownloadFileByIndex(position, s_meidaFileList.fileListInfo[i].fileIndex);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Download media file by index failed, error code: 0x%08X.", returnCode);
                s_nextDownloadFileIndex--;
                goto redownload;
            }
            if (s_meidaFileList.fileListInfo[i].type == DJI_CAMERA_FILE_TYPE_LDRT) {
                for (j = 0; j < s_meidaFileList.fileListInfo[i].subFileListTotalNum; j++) {
                    subFileRedownload:
                    returnCode = DjiCameraManager_DownloadSubFileByIndexAndSubType(position,
                                                                                s_meidaFileList.fileListInfo[i].fileIndex,
                                                                                s_meidaFileList.fileListInfo[i].subFileListInfo[j].type);

                    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                        USER_LOG_ERROR("Download sub media file by index and type failed, error code: 0x%08X.", returnCode);
                        goto subFileRedownload;
                    }
                }
            }
        }

        if (s_meidaFileList.fileListInfo[0].type == DJI_CAMERA_FILE_TYPE_JPEG) {
            returnCode = DjiCameraManager_DeleteFileByIndex(position, s_meidaFileList.fileListInfo[0].fileIndex);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Delete media file by index failed, error code: 0x%08X.", returnCode);
                return returnCode;
            }
        }

        osalHandler->TaskSleepMs(1000);
    } else {
        USER_LOG_WARN("Media file is not existed in sdcard.\r\n");
    }

    returnCode = DjiCameraManager_ReleaseDownloaderRights(position);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Release downloader rights failed, error code: 0x%08X.", returnCode);
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode DjiTest_CameraManagerMediaDownloadFileListBySlices(E_DjiMountPosition position)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    uint16_t downloadCount = 0;
    T_DjiCameraManagerSliceConfig sliceConfig = {0};

    s_nextDownloadFileIndex = 0;
    returnCode = DjiCameraManager_RegDownloadFileDataCallback(position, DjiTest_CameraManagerDownloadFileDataCallback);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Register download file data callback failed, error code: 0x%08X.", returnCode);
        return returnCode;
    }

    returnCode = DjiCameraManager_ObtainDownloaderRights(position);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Obtain downloader rights failed, error code: 0x%08X.", returnCode);
    }

    sliceConfig.countPerSlice = DJI_CAMERA_MANAGER_FILE_LIST_COUNT_ALL_PER_SLICE;
    sliceConfig.sliceStartIndex = 0;

    returnCode = DjiCameraManager_DownloadFileListBySlices(position, sliceConfig, &s_meidaFileList);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Download file list failed, error code: 0x%08X.", returnCode);
        return returnCode;
    }

    if (s_meidaFileList.totalCount > 0) {
        downloadCount = s_meidaFileList.totalCount;
        printf(
            "\033[1;33;40m -> Download file list finished, total file count is %d, the following %d is list details: \033[0m\r\n",
            s_meidaFileList.totalCount, downloadCount);
        for (int i = 0; i < downloadCount; ++i) {
            if (s_meidaFileList.fileListInfo[i].fileSize < 1 * 1024 * 1024) {
                printf(
                    "\033[1;32;40m ### Media file_%03d name: %s, index: %d, time:%04d-%02d-%02d_%02d:%02d:%02d, size: %.2f KB, type: %d \033[0m\r\n",
                    i, s_meidaFileList.fileListInfo[i].fileName,
                    s_meidaFileList.fileListInfo[i].fileIndex,
                    s_meidaFileList.fileListInfo[i].createTime.year,
                    s_meidaFileList.fileListInfo[i].createTime.month,
                    s_meidaFileList.fileListInfo[i].createTime.day,
                    s_meidaFileList.fileListInfo[i].createTime.hour,
                    s_meidaFileList.fileListInfo[i].createTime.minute,
                    s_meidaFileList.fileListInfo[i].createTime.second,
                    (dji_f32_t) s_meidaFileList.fileListInfo[i].fileSize / 1024,
                    s_meidaFileList.fileListInfo[i].type);
            } else {
                printf(
                    "\033[1;32;40m ### Media file_%03d name: %s, index: %d, time:%04d-%02d-%02d_%02d:%02d:%02d, size: %.2f MB, type: %d \033[0m\r\n",
                    i, s_meidaFileList.fileListInfo[i].fileName,
                    s_meidaFileList.fileListInfo[i].fileIndex,
                    s_meidaFileList.fileListInfo[i].createTime.year,
                    s_meidaFileList.fileListInfo[i].createTime.month,
                    s_meidaFileList.fileListInfo[i].createTime.day,
                    s_meidaFileList.fileListInfo[i].createTime.hour,
                    s_meidaFileList.fileListInfo[i].createTime.minute,
                    s_meidaFileList.fileListInfo[i].createTime.second,
                    (dji_f32_t) s_meidaFileList.fileListInfo[i].fileSize / (1024 * 1024),
                    s_meidaFileList.fileListInfo[i].type);
            }
        }
        printf("\r\n");

        returnCode = DjiCameraManager_DownloadFileByIndex(position, s_meidaFileList.fileListInfo[0].fileIndex);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Download media file by index failed, error code: 0x%08X.", returnCode);
        }

    } else {
        USER_LOG_WARN("Media file is not existed in sdcard.\r\n");
    }

    returnCode = DjiCameraManager_ReleaseDownloaderRights(position);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Release downloader rights failed, error code: 0x%08X.", returnCode);
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode DjiTest_CameraManagerDownloadFileDataCallback(T_DjiDownloadFilePacketInfo packetInfo,
                                                                     const uint8_t *data, uint16_t len)
{
    int32_t i =0, j = 0;
    float downloadSpeed = 0.0f;
    int32_t mediaFileIndex = 0, mediaSubFileIndex = -1;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    if (packetInfo.downloadFileEvent == DJI_DOWNLOAD_FILE_EVENT_START) {
        memset(downloadFileName, 0, sizeof(downloadFileName));
        if (packetInfo.fileType == DJI_DOWNLOAD_FILE_ORG) {
            for (i = 0; i < s_meidaFileList.totalCount; ++i) {
                if (s_meidaFileList.fileListInfo[i].fileIndex == packetInfo.fileIndex) {
                    snprintf(downloadFileName, sizeof(downloadFileName), "%s", s_meidaFileList.fileListInfo[i].fileName);
                    mediaFileIndex = i;
                    s_nextDownloadFileIndex = i + 1;
                    break;
                }
            }
        } else {
            for (i = 0; i < s_meidaFileList.totalCount; ++i) {
                for (j = 0; j < s_meidaFileList.fileListInfo[i].subFileListTotalNum; ++j) {
                    if (s_meidaFileList.fileListInfo[i].subFileListInfo[j].fileIndex == packetInfo.fileIndex &&
                        s_meidaFileList.fileListInfo[i].subFileListInfo[j].type == packetInfo.fileType) {
                        snprintf(downloadFileName, sizeof(downloadFileName), "%s",
                                 s_meidaFileList.fileListInfo[i].subFileListInfo[j].fileName);
                        mediaFileIndex = i;
                        mediaSubFileIndex = j;
                        break;
                    }
                }
            }
        }

        osalHandler->GetTimeMs(&downloadStartMs);

        if (mediaSubFileIndex != TEST_CAMEAR_MEDIA_SUB_FILE_NOT_FOUND) {
            USER_LOG_INFO("Start download media sub file, index : %d, media file, index: %d", mediaSubFileIndex, mediaFileIndex);
        } else {
            USER_LOG_INFO("Start download media file, index : %d", mediaFileIndex);
        }

        s_downloadMediaFile = fopen(downloadFileName, "wb+");
        if (s_downloadMediaFile == NULL) {
            return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
        }
        fwrite(data, 1, len, s_downloadMediaFile);
    } else if (packetInfo.downloadFileEvent == DJI_DOWNLOAD_FILE_EVENT_TRANSFER) {
        if (s_downloadMediaFile != NULL) {
            fwrite(data, 1, len, s_downloadMediaFile);
        }
        printf("\033[1;32;40m ### [Complete rate : %0.1f%%] (%s), size: %u, fileIndex: %d\033[0m\r\n",
               packetInfo.progressInPercent, downloadFileName, packetInfo.fileSize, packetInfo.fileIndex);
        printf("\033[1A");
        USER_LOG_DEBUG("Transfer download media file data, len: %d, percent: %.1f", len, packetInfo.progressInPercent);
    } else if (packetInfo.downloadFileEvent == DJI_DOWNLOAD_FILE_EVENT_END) {
        if (s_downloadMediaFile == NULL) {
            return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
        }

        fwrite(data, 1, len, s_downloadMediaFile);

        osalHandler->GetTimeMs(&downloadEndMs);

        downloadSpeed = (float) packetInfo.fileSize / (float) (downloadEndMs - downloadStartMs);
        printf("\033[1;32;40m ### [Complete rate : %0.1f%%] (%s), size: %u, fileIndex: %d\033[0m\r\n",
               packetInfo.progressInPercent, downloadFileName, packetInfo.fileSize, packetInfo.fileIndex);
        printf("\033[1A");
        printf("\r\n");
        USER_LOG_INFO("End download media file, Download Speed %.2f KB/S\r\n\r\n", downloadSpeed);
        fclose(s_downloadMediaFile);
        s_downloadMediaFile = NULL;
    } else if (packetInfo.downloadFileEvent == DJI_DOWNLOAD_FILE_EVENT_START_TRANSFER_END) {

        memset(downloadFileName, 0, sizeof(downloadFileName));
        if (packetInfo.fileType == DJI_DOWNLOAD_FILE_ORG) {
            for (i = 0; i < s_meidaFileList.totalCount; ++i) {
                if (s_meidaFileList.fileListInfo[i].fileIndex == packetInfo.fileIndex) {
                    snprintf(downloadFileName, sizeof(downloadFileName), "%s", s_meidaFileList.fileListInfo[i].fileName);
                    mediaFileIndex = i;
                    s_nextDownloadFileIndex = i + 1;
                    break;
                }
            }
        } else {
            for (i = 0; i < s_meidaFileList.totalCount; ++i) {
                for (j = 0; j < s_meidaFileList.fileListInfo[i].subFileListTotalNum; ++j) {
                    if (s_meidaFileList.fileListInfo[i].subFileListInfo[j].fileIndex == packetInfo.fileIndex &&
                        s_meidaFileList.fileListInfo[i].subFileListInfo[j].type == packetInfo.fileType) {
                        snprintf(downloadFileName, sizeof(downloadFileName), "%s",
                                 s_meidaFileList.fileListInfo[i].subFileListInfo[j].fileName);
                        mediaFileIndex = i;
                        mediaSubFileIndex = j;
                        break;
                    }
                }
            }
        }

        if (mediaSubFileIndex != TEST_CAMEAR_MEDIA_SUB_FILE_NOT_FOUND) {
            USER_LOG_INFO("Start download media sub file, index : %d, media file, index: %d", mediaSubFileIndex, mediaFileIndex);
        } else {
            USER_LOG_INFO("Start download media file, index : %d", mediaFileIndex);
        }

        s_downloadMediaFile = fopen(downloadFileName, "wb+");
        if (s_downloadMediaFile == NULL) {
            return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
        }
        fwrite(data, 1, len, s_downloadMediaFile);

        printf("\033[1;32;40m ### [Complete rate : %0.1f%%] (%s), size: %u, mediaFileIndex: %d\033[0m\r\n",
               packetInfo.progressInPercent, downloadFileName, packetInfo.fileSize, packetInfo.fileIndex);
        printf("\033[1A");

        downloadSpeed = (float) packetInfo.fileSize / 1.0F;
        printf("\033[1;32;40m ### [Complete rate : %0.1f%%] (%s), size: %u, mediaFileIndex: %d\033[0m\r\n",
               packetInfo.progressInPercent, downloadFileName, packetInfo.fileSize, packetInfo.fileIndex);
        printf("\033[1A");
        printf("\r\n");
        USER_LOG_INFO("End download media file, Download Speed %.2f KB/S\r\n\r\n", downloadSpeed);
        fclose(s_downloadMediaFile);
        s_downloadMediaFile = NULL;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode DjiTest_CameraManagerGetPointThermometryData(E_DjiMountPosition position)
{
    T_DjiReturnCode djiStat;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    T_DjiCameraManagerPointThermometryCoordinate pointCoordinate = {0};
    T_DjiCameraManagerPointThermometryData pointThermometryData = {0};

    USER_LOG_INFO("--> Step 1: Set point thermometry coordinate");

    pointCoordinate.pointX = 0.5;
    pointCoordinate.pointY = 0.5;
    djiStat = DjiCameraManager_SetPointThermometryCoordinate(position, pointCoordinate);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Set point thermometry coordinate failed, stat = 0x%08llX", djiStat);
        return djiStat;
    }

    USER_LOG_INFO("--> Step 2: Get data of point thermometry");

    for (int i = 0; i < 30; ++i) {
        osalHandler->TaskSleepMs(1000 / CAMERA_MANAGER_SUBSCRIPTION_FREQ);

        djiStat = DjiCameraManager_GetPointThermometryData(position, &pointThermometryData);
        if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Get data of point thermometry error.");
            return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
        }

        USER_LOG_INFO("received camera point thermometry data, point_x = %.2f, point_y = %.2f "
                      "point temperature = %.2f\r\n", pointThermometryData.pointX,
                      pointThermometryData.pointY, pointThermometryData.pointTemperature);
    }

    return djiStat;
}

static T_DjiReturnCode DjiTest_CameraManagerGetAreaThermometryData(E_DjiMountPosition position)
{
    T_DjiReturnCode djiStat;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    T_DjiCameraManagerAreaThermometryData areaThermometryData = {0};
    T_DjiCameraManagerAreaThermometryCoordinate areaCoordinate = {0};

    USER_LOG_INFO("--> Step 1: Set area thermometry coordinate");
    areaCoordinate.areaTempLtX = 0.2;
    areaCoordinate.areaTempLtY = 0.2;
    areaCoordinate.areaTempRbX = 0.8;
    areaCoordinate.areaTempRbY = 0.8;

    djiStat = DjiCameraManager_SetAreaThermometryCoordinate(position, areaCoordinate);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Set area coordinate failed, stat = 0x%08llX", djiStat);
        return djiStat;
    }

    USER_LOG_INFO("--> Step 2: Get data of area thermometry");

    for (int i = 0; i < 30; ++i) {
        osalHandler->TaskSleepMs(1000 / CAMERA_MANAGER_SUBSCRIPTION_FREQ);

        djiStat = DjiCameraManager_GetAreaThermometryData(position, &areaThermometryData);
        if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Get data of area thermometry error.");
            return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
        }

        USER_LOG_INFO("received camera area thermometry data, lt_point_x = %.2f, lt_point_y = %.2f "
                      "rb_point_x = %.2f, rb_point_y = %.2f, area min temperature = %.2f, area max temperature = %.2f, "
                      "area average temperature = %.2f\r\n", areaThermometryData.areaTempLtX,
                      areaThermometryData.areaTempLtY,
                      areaThermometryData.areaTempRbX, areaThermometryData.areaTempRbY, areaThermometryData.areaMinTemp,
                      areaThermometryData.areaMaxTemp, areaThermometryData.areaAveTemp);
        USER_LOG_INFO("min temperature point_x = %.2f, min temperature point_y = %.2f, max temperature point_x = %.2f, "
                      "max temperature point_y = %.2f\r\n", areaThermometryData.areaMinTempPointX,
                      areaThermometryData.areaMinTempPointY,
                      areaThermometryData.areaMaxTempPointX, areaThermometryData.areaMaxTempPointY);
    }

    return djiStat;
}

static T_DjiReturnCode DjiTest_CameraManagerGetLidarRangingInfo(E_DjiMountPosition position)
{
    T_DjiReturnCode returnCode = 0;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    T_DjiCameraManagerLaserRangingInfo rangingInfo = {0};

    for (int i = 0; i < 5; i++) {

        osalHandler->TaskSleepMs(1000);

        returnCode = DjiCameraManager_GetLaserRangingInfo(position, &rangingInfo);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Get mounted position %d laser ranging info failed, error code: 0x%08X.",
                           position, returnCode);
            return returnCode;
        }

        USER_LOG_INFO(
            "Receive lidar range info, lon:%.6f, lat:%.6f, alt:%.1f, dis:%d, enable:%d, exception:%d, x:%d, y:%d",
            rangingInfo.longitude,
            rangingInfo.latitude,
            (float) rangingInfo.altitude / 10,
            rangingInfo.distance,
            rangingInfo.enable_lidar,
            rangingInfo.exception,
            rangingInfo.screenX,
            rangingInfo.screenY);
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

#ifndef SYSTEM_ARCH_RTOS
T_DjiReturnCode DjiTest_CameraManagerSubscribePointCloud(E_DjiMountPosition position)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    uint8_t recvBuf[TEST_CAMERA_MOP_CHANNEL_SUBSCRIBE_POINT_CLOUD_RECV_BUFFER] = {0};
    uint8_t colorPointsBuf[TEST_CAMERA_MOP_CHANNEL_COLOR_POINTS_BUFFER] = {0};
    uint32_t realLen;
    uint32_t recvDataCount = 0;
    struct tm *localTime = NULL;
    time_t currentTime = time(NULL);
    FILE *fp = NULL;
    size_t size;
    T_DjiCameraManagerColorPointCloud *colorPointCloud;
    uint32_t colorPointCloudDataByte = 0;
    uint32_t colorPointsNum = 0;
    static bool isMopInit = false;

    returnCode = DjiCameraManager_StartRecordPointCloud(position);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Start record point cloud failed, stat:0x%08llX.", returnCode);
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    localTime = localtime(&currentTime);
    sprintf(s_pointCloudFilePath, "payload%d_point_cloud_%04d%02d%02d_%02d-%02d-%02d.ldrt",
            position, localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday,
            localTime->tm_hour, localTime->tm_min, localTime->tm_sec);

    fp = fopen(s_pointCloudFilePath, "ab+");
    if (fp == NULL) {
        USER_LOG_ERROR("fopen point cloud file failed!\n");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    if (isMopInit == false) {
        returnCode = DjiMopChannel_Init();
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            fclose(fp);
            USER_LOG_ERROR("Mop channel init error, stat:0x%08llX.", returnCode);
            return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
        } else {
            isMopInit = true;
        }
    }

    returnCode = DjiMopChannel_Create(&s_mopChannelHandle, DJI_MOP_CHANNEL_TRANS_UNRELIABLE);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        fclose(fp);
        USER_LOG_ERROR("Mop channel create send handle error, stat:0x%08llX.", returnCode);
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

RECONNECT:
    osalHandler->TaskSleepMs(TEST_CAMERA_MOP_CHANNEL_WAIT_TIME_MS);
    returnCode = DjiMopChannel_Connect(s_mopChannelHandle, DJI_CHANNEL_ADDRESS_PAYLOAD_PORT_NO1,
                                       TEST_CAMERA_MOP_CHANNEL_SUBSCRIBE_POINT_CLOUD_CHANNEL_ID);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Connect point cloud mop channel error, stat:0x%08llX.", returnCode);
        goto RECONNECT;
    }

    while (1) {
        if (recvDataCount == TEST_CAMERA_MOP_CHANNEL_MAX_RECV_COUNT) {
            USER_LOG_INFO("Subscribe point cloud success, the point cloud data is stored in %s", s_pointCloudFilePath);
            break;
        }

        memset(recvBuf, 0, TEST_CAMERA_MOP_CHANNEL_SUBSCRIBE_POINT_CLOUD_RECV_BUFFER);

        returnCode = DjiMopChannel_RecvData(s_mopChannelHandle, recvBuf,
                                            TEST_CAMERA_MOP_CHANNEL_SUBSCRIBE_POINT_CLOUD_RECV_BUFFER, &realLen);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            if (returnCode == DJI_ERROR_MOP_CHANNEL_MODULE_CODE_CONNECTION_CLOSE) {
                USER_LOG_INFO("mop channel is disconnected");
                osalHandler->TaskSleepMs(TEST_CAMERA_MOP_CHANNEL_WAIT_TIME_MS);
                goto RECONNECT;
            }
        }

        colorPointCloud = (T_DjiCameraManagerColorPointCloud *) recvBuf;
        colorPointCloudDataByte = (colorPointCloud->pointCloudHeader).dataByte;
        colorPointsNum = colorPointCloudDataByte / sizeof(T_DjiCameraManagerPointXYZRGBInfo);
        memcpy(colorPointsBuf, (uint8_t *)(colorPointCloud->points), colorPointCloudDataByte);
        USER_LOG_INFO("Mop channel recv data from channel length:%d, count:%d, points num = %d, points byte = %d",
                      realLen, recvDataCount++, colorPointsNum, colorPointCloudDataByte);

        size = fwrite(colorPointsBuf, 1, colorPointCloudDataByte, fp);
        if (size != colorPointCloudDataByte) {
            USER_LOG_ERROR("fwrite point cloud file failed!\n");
            fclose(fp);
            return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
        }

        fflush(fp);
    }

    fclose(fp);

    returnCode = DjiMopChannel_Close(s_mopChannelHandle);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Close mop channel error, stat:0x%08llX.", returnCode);
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    returnCode = DjiMopChannel_Destroy(s_mopChannelHandle);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Destroy mop channel error, stat:0x%08llX.", returnCode);
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    returnCode = DjiCameraManager_StopRecordPointCloud(position);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Stop record point cloud failed, stat:0x%08llX.", returnCode);
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}
#endif

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
