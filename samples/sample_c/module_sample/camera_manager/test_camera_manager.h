/**
 ********************************************************************
 * @file    test_camera_manager.h
 * @brief   This is the header file for "test_camera_manager.c", defining the structure and
 * (exported) function prototypes.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef TEST_CAMERA_MANAGER_H
#define TEST_CAMERA_MANAGER_H

/* Includes ------------------------------------------------------------------*/
#include "dji_typedef.h"
#include "dji_camera_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef enum {
    E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_CAMERA_SHUTTER_SPEED,
    E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_CAMERA_APERTURE,
    E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_CAMERA_EV,
    E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_CAMERA_ISO,
    E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_CAMERA_FOCUS_POINT,
    E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_CAMERA_TAP_ZOOM_POINT,
    E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_CAMERA_ZOOM_PARAM,
    E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SHOOT_SINGLE_PHOTO,
    E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SHOOT_BURST_PHOTO,
    E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SHOOT_INTERVAL_PHOTO,
    E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_RECORD_VIDEO,
    E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_DOWNLOAD_AND_DELETE_MEDIA_FILE,
    E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_DOWNLOAD_FILE_LIST_BY_SLICES,
    E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_THERMOMETRY,
    E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_GET_LIDAR_RANGING_INFO,
    E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_IR_CAMERA_ZOOM_PARAM,
    E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_NIGHT_SCENE_MODE,
    E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_CAPTURE_RECORDING_STREAMS,
    E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SHOW_STORAGE_INFO,
    E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_FORMAT_SD_CARD,
    E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_LINK_ZOOM,
    E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_USER_CUSTOM_DIR_FILE_NAME,
    E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_RESET_CAMERA_SETTINGS,
    E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_AE_LOCK_MODE,
    E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_FOCUS_RING_VALUE,
    E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_CONNECT_STATUS_TEST,
    E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_GET_PHOTO_VIDEO_PARAM,
    E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_METERING_MODE,
    E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_METERING_POINT,
    E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_FFC_MODE_AND_TRRIGER,
    E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_GAIN_MODE,
    E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_GET_CAMERA_STATUS,
    E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SUBSCRIBE_POINT_CLOUD,
    E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_INDEX_MAX
} E_DjiTestCameraManagerSampleSelect;
/* Exported functions --------------------------------------------------------*/

/*! @brief Sample to set exposure compensation value for camera, using async
 * api
 *
 *  @note In this interface, exposure compensation value will be got then be
 * set.
 *  @param index payload node index, input limit see enum
 * DJI::OSDK::PayloadIndexType
 *  @param dataTarget the target exposure compensation value
 *  @return T_DjiReturnCode error code
 */
T_DjiReturnCode DjiTest_CameraManagerSetEV(E_DjiMountPosition position,
                                           E_DjiCameraManagerExposureCompensation exposureCompensation);

/*! @brief Sample to set exposure mode for camera, using async api
 *
 *  @note In this interface, exposure will be got then be set.
 *  @param index payload node index, input limit see enum
 * DJI::OSDK::PayloadIndexType
 *  @param dataTarget the target exposure mode
 *  @return T_DjiReturnCode error code
 */
T_DjiReturnCode DjiTest_CameraManagerSetExposureMode(E_DjiMountPosition position,
                                                     E_DjiCameraManagerExposureMode exposureMode);

/*! @brief Sample to set ISO value for camera, using async api
 *
 *  @note In this interface, ISO will be got then be set.
 *  @param index payload node index, input limit see enum
 * DJI::OSDK::PayloadIndexType
 *  @param dataTarget the target ISO value
 *  @return T_DjiReturnCode error code
 */
T_DjiReturnCode DjiTest_CameraManagerSetISO(E_DjiMountPosition position,
                                            E_DjiCameraManagerISO isoData);
/*! @brief Sample to set shutter speed for camera, using async api
 *
 *  @note In this interface, shutter speed will be got then be set.
 *  @param index payload node index, input limit see enum
 * DJI::OSDK::PayloadIndexType
 *  @param dataTarget the target shutter speed
 *  @return T_DjiReturnCode error code
 */
T_DjiReturnCode DjiTest_CameraManagerSetShutterSpeed(E_DjiMountPosition position,
                                                     E_DjiCameraManagerShutterSpeed shutterSpeed);

/*! @brief Sample to set shutter aperture value for camera, using async api
 *
 *  @note In this interface, aperture value will be got then be set.
 *  @param index payload node index, input limit see enum
 * DJI::OSDK::PayloadIndexType
 *  @param dataTarget the target aperture value
 *  @return T_DjiReturnCode error code
 */
T_DjiReturnCode DjiTest_CameraManagerSetAperture(E_DjiMountPosition position,
                                                 E_DjiCameraManagerAperture aperture);

/*! @brief Sample to set focus point for camera, using async api
 *
 *  @note In this interface, focus mode will be set to be AUTO. Then the
 * focus point will be set to be (x, y)
 *  @param index payload node index, input limit see enum
 * DJI::OSDK::PayloadIndexType
 *  @param x the x value of target focus point, 0~1
 *  @param y the y value of target focus point, 0~1
 *  @return T_DjiReturnCode error code
 */
T_DjiReturnCode DjiTest_CameraManagerSetFocusPoint(E_DjiMountPosition position,
                                                   T_DjiCameraManagerFocusPosData tapFocusPos);

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
                                                     T_DjiCameraManagerTapZoomPosData tapZoomPosData);
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
                                                         E_DjiCameraZoomSpeed zoomSpeed);

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
                                                 dji_f32_t factor);

/*! @brief Sample to stop continuous zoom on camera, using async api
 *
 *  @note It is only supported by X5, X5R and X5S camera on Osmo with lens
 * Olympus M.Zuiko ED 14-42mm f/3.5-5.6 EZ, Z3 camera, Z30 camera.
 *  @param index payload node index, input limit see enum
 * DJI::OSDK::PayloadIndexType
 *  @return T_DjiReturnCode error code
 */
T_DjiReturnCode DjiTest_CameraManagerStopContinuousZoom(E_DjiMountPosition position);

/*! @brief Sample to shoot single photo, using async api
 *
 *  @note In this interface, camera will be set to be the SHOOT_PHOTO mode
 * then start to shoot a single photo.
 *  @param index payload node index, input limit see enum
 * DJI::OSDK::PayloadIndexType
 *  @return T_DjiReturnCode error code
 */
T_DjiReturnCode DjiTest_CameraManagerStartShootSinglePhoto(E_DjiMountPosition position);

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
                                                          E_DjiCameraBurstCount burstCount);

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
                                                             T_DjiCameraPhotoTimeIntervalSettings intervalData);

/*! @brief Sample to stop shooting, using async api
 *
 *  @note In this interface, camera will stop all the shooting action
 *  @param index payload node index, input limit see enum
 * DJI::OSDK::PayloadIndexType
 *  @return T_DjiReturnCode error code
 */
T_DjiReturnCode DjiTest_CameraManagerStopShootPhoto(E_DjiMountPosition position);

/*! @brief Sample to start record video, using async api
 *
 *  @note In this interface, camera will be set to be the RECORD_VIDEO mode
 * then start to record video.
 *  @param index payload node index, input limit see enum
 * DJI::OSDK::PayloadIndexType
 *  @return T_DjiReturnCode error code
 */
T_DjiReturnCode DjiTest_CameraManagerStartRecordVideo(E_DjiMountPosition position);

/*! @brief Sample to stop record video, using async api
 *
 *  @note In this interface, camera will be set to be the RECORD_VIDEO mode
 * then stop recording video.
 *  @param index payload node index, input limit see enum
 * DJI::OSDK::PayloadIndexType
 *  @return T_DjiReturnCode error code
 */
T_DjiReturnCode DjiTest_CameraManagerStopRecordVideo(E_DjiMountPosition position);

T_DjiReturnCode DjiTest_CameraManagerRunSample(E_DjiMountPosition mountPosition,
                                               E_DjiTestCameraManagerSampleSelect cameraManagerSampleSelect);

#ifdef __cplusplus
}
#endif

#endif // TEST_CAMERA_MANAGER_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
