/**
 ********************************************************************
 * @file    psdk_payload_camera.h
 * @brief   This is the header file for "psdk_payload_camera.c", defining the structure and
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
#ifndef PSDK_PAYLOAD_CAMERA_H
#define PSDK_PAYLOAD_CAMERA_H

/* Includes ------------------------------------------------------------------*/
#include "legacy_psdk2.x/psdk_typedef.h"
#include "dji_payload_camera.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/
/**
 * @brief Camera work mode.
 */
typedef E_DjiCameraMode E_PsdkCameraMode;
#define PSDK_CAMERA_MODE_SHOOT_PHOTO    DJI_CAMERA_MODE_SHOOT_PHOTO /*!< Shoot photo work mode. */
#define PSDK_CAMERA_MODE_RECORD_VIDEO   DJI_CAMERA_MODE_RECORD_VIDEO/*!< Record video work mode. */
#define PSDK_CAMERA_MODE_PLAYBACK       DJI_CAMERA_MODE_PLAYBACK /*!< Media playback work mode. */

/**
 * @brief Camera shoot photo mode.
 */
typedef E_DjiCameraShootPhotoMode E_PsdkCameraShootPhotoMode;
#define PSDK_CAMERA_SHOOT_PHOTO_MODE_SINGLE    DJI_CAMERA_SHOOT_PHOTO_MODE_SINGLE /*!< Single photographing mode. */
#define PSDK_CAMERA_SHOOT_PHOTO_MODE_BURST     DJI_CAMERA_SHOOT_PHOTO_MODE_BURST/*!< Burst photographing mode. */
#define PSDK_CAMERA_SHOOT_PHOTO_MODE_INTERVAL  DJI_CAMERA_SHOOT_PHOTO_MODE_INTERVAL/*!< Interval photographing mode. */

/**
 * @brief Camera shooting state when photographing.
 */
typedef E_DjiCameraShootingState E_PsdkCameraShootingState;
#define PSDK_CAMERA_SHOOTING_PHOTO_IDLE      DJI_CAMERA_SHOOTING_PHOTO_IDLE /*!< Photographing in idle state. */
#define PSDK_CAMERA_SHOOTING_SINGLE_PHOTO    DJI_CAMERA_SHOOTING_SINGLE_PHOTO /*!< Photographing in single photograph state . */
#define PSDK_CAMERA_SHOOTING_BURST_PHOTO     DJI_CAMERA_SHOOTING_BURST_PHOTO /*!< Photographing in burst photograph state. */
#define PSDK_CAMERA_SHOOTING_INTERVAL_PHOTO  DJI_CAMERA_SHOOTING_INTERVAL_PHOTO /*!< Photographing in interval photograph state. */

/**
 * @brief Camera burst count setting when photographing.
 */
typedef E_DjiCameraBurstCount E_PsdkCameraBurstCount;
#define PSDK_CAMERA_BURST_COUNT_2     DJI_CAMERA_BURST_COUNT_2 /*!< Photo number of burst photographing: 2. */
#define PSDK_CAMERA_BURST_COUNT_3     DJI_CAMERA_BURST_COUNT_3 /*!< Photo number of burst photographing: 3. */
#define PSDK_CAMERA_BURST_COUNT_5     DJI_CAMERA_BURST_COUNT_5 /*!< Photo number of burst photographing: 5. */
#define PSDK_CAMERA_BURST_COUNT_7     DJI_CAMERA_BURST_COUNT_7/*!< Photo number of burst photographing: 7. */
#define PSDK_CAMERA_BURST_COUNT_10    DJI_CAMERA_BURST_COUNT_10 /*!< Photo number of burst photographing: 10. */
#define PSDK_CAMERA_BURST_COUNT_14    DJI_CAMERA_BURST_COUNT_14 /*!< Photo number of burst photographing: 14. */

/**
 * @brief Camera metering mode.
 */
typedef E_DjiCameraMeteringMode E_PsdkCameraMeteringMode;
#define PSDK_CAMERA_METERING_MODE_CENTER  DJI_CAMERA_METERING_MODE_CENTER /*!< Center metering mode. */
#define PSDK_CAMERA_METERING_MODE_AVERAGE DJI_CAMERA_METERING_MODE_AVERAGE /*!< Average metering mode. */
#define PSDK_CAMERA_METERING_MODE_SPOT    DJI_CAMERA_METERING_MODE_SPOT /*!< Spot metering mode. */

/**
 * @brief Camera focus mode.
 */
typedef E_DjiCameraFocusMode E_PsdkCameraFocusMode;
#define PSDK_CAMERA_FOCUS_MODE_MANUAL DJI_CAMERA_FOCUS_MODE_MANUAL /*!< Manual focus mode. */
#define PSDK_CAMERA_FOCUS_MODE_AUTO   DJI_CAMERA_FOCUS_MODE_AUTO /*!< Auto focus mode. */

/**
 * @brief Camera zoom direction.
 */
typedef E_DjiCameraZoomDirection E_PsdkCameraZoomDirection;
#define PSDK_CAMERA_ZOOM_DIRECTION_OUT DJI_CAMERA_ZOOM_DIRECTION_OUT /*!< The lens moves in the far direction, the zoom factor becomes smaller. */
#define PSDK_CAMERA_ZOOM_DIRECTION_IN  DJI_CAMERA_ZOOM_DIRECTION_IN /*!< The lens moves in the near direction, the zoom factor becomes larger. */

/**
 * @brief Camera zoom speed.
 */
typedef E_DjiCameraZoomSpeed E_PsdkCameraZoomSpeed;
#define PSDK_CAMERA_ZOOM_SPEED_SLOWEST          DJI_CAMERA_ZOOM_SPEED_SLOWEST/*!< Lens zooms in slowest speed. */
#define PSDK_CAMERA_ZOOM_SPEED_SLOW             DJI_CAMERA_ZOOM_SPEED_SLOW /*!< Lens zooms in slow speed. */
#define PSDK_CAMERA_ZOOM_SPEED_MODERATELY_SLOW  DJI_CAMERA_ZOOM_SPEED_MODERATELY_SLOW /*!< Lens zooms in speed slightly slower than normal speed. */
#define PSDK_CAMERA_ZOOM_SPEED_NORMAL           DJI_CAMERA_ZOOM_SPEED_NORMAL /*!< Lens zooms in normal speed. */
#define PSDK_CAMERA_ZOOM_SPEED_MODERATELY_FAST  DJI_CAMERA_ZOOM_SPEED_MODERATELY_FAST /*!< Lens zooms very in speed slightly faster than normal speed. */
#define PSDK_CAMERA_ZOOM_SPEED_FAST             DJI_CAMERA_ZOOM_SPEED_FAST /*!< Lens zooms very in fast speed. */
#define PSDK_CAMERA_ZOOM_SPEED_FASTEST          DJI_CAMERA_ZOOM_SPEED_FASTEST /*!< Lens zooms very in fastest speed. */

/**
 * @brief Camera supported media file type.
 */
typedef E_DjiCameraMediaFileType E_PsdkCameraMediaFileType;
#define PSDK_CAMERA_FILE_TYPE_JPEG DJI_CAMERA_FILE_TYPE_JPEG /*!< Media file JPEG type. */
#define PSDK_CAMERA_FILE_TYPE_DNG  DJI_CAMERA_FILE_TYPE_DNG /*!< Media file DNG type. */
#define PSDK_CAMERA_FILE_TYPE_MOV  DJI_CAMERA_FILE_TYPE_MOV /*!< Media file MOV type. */
#define PSDK_CAMERA_FILE_TYPE_MP4  DJI_CAMERA_FILE_TYPE_MP4 /*!< Media file MP4 type. */

/**
 * @brief Camera playback mode in playback process.
 */
typedef E_DjiCameraPlaybackMode E_PsdkCameraPlaybackMode;
#define PSDK_CAMERA_PLAYBACK_MODE_PLAY   DJI_CAMERA_PLAYBACK_MODE_PLAY /*!< Play playbacking mode. */
#define PSDK_CAMERA_PLAYBACK_MODE_PAUSE  DJI_CAMERA_PLAYBACK_MODE_PAUSE /*!< Pause playbacking mode. */
#define PSDK_CAMERA_PLAYBACK_MODE_STOP   DJI_CAMERA_PLAYBACK_MODE_STOP /*!< Stop playbacking mode. */

/**
 * @brief Camera supported video frames when working in playback mode.
 */
typedef E_DjiCameraVideoFrameRate E_PsdkCameraVideoFrameRate;
#define PSDK_CAMERA_VIDEO_FRAME_RATE_24_FPS          DJI_CAMERA_VIDEO_FRAME_RATE_24_FPS /*!< The camera's video frame rate is 24fps (frames per second) */
#define PSDK_CAMERA_VIDEO_FRAME_RATE_25_FPS          DJI_CAMERA_VIDEO_FRAME_RATE_25_FPS /*!< The camera's video frame rate is 25fps (frames per second) */
#define PSDK_CAMERA_VIDEO_FRAME_RATE_30_FPS          DJI_CAMERA_VIDEO_FRAME_RATE_30_FPS /*!< The camera's video frame rate is 30fps (frames per second) */
#define PSDK_CAMERA_VIDEO_FRAME_RATE_UNKNOWN         DJI_CAMERA_VIDEO_FRAME_RATE_UNKNOWN /*!< The camera's video frame rate is unknown (frames per second) */

/**
 * @brief Camera supported video resolutions when working in playback mode.
 */
typedef E_DjiCameraVideoResolution E_PsdkCameraVideoResolution;
#define PSDK_CAMERA_VIDEO_RESOLUTION_640x480    DJI_CAMERA_VIDEO_RESOLUTION_640x480 /*!< /The camera's video resolution is 640x480. */
#define PSDK_CAMERA_VIDEO_RESOLUTION_1280x720   DJI_CAMERA_VIDEO_RESOLUTION_1280x720 /*!< /The camera's video resolution is 1280x720. */
#define PSDK_CAMERA_VIDEO_RESOLUTION_1920x1080  DJI_CAMERA_VIDEO_RESOLUTION_1920x1080 /*!< /The camera's video resolution is 1920x1080. */
#define PSDK_CAMERA_VIDEO_RESOLUTION_2048x1080  DJI_CAMERA_VIDEO_RESOLUTION_2048x1080  /*!< /The camera's video resolution is 2048x1080. */
#define PSDK_CAMERA_VIDEO_RESOLUTION_3840x2160  DJI_CAMERA_VIDEO_RESOLUTION_3840x2160 /*!< /The camera's video resolution is 3840x2160. */
#define PSDK_CAMERA_VIDEO_RESOLUTION_UNKNOWN    DJI_CAMERA_VIDEO_RESOLUTION_UNKNOWN /*!< /The camera's video resolution is unknown. */

/**
 * @brief Camera zoom state in tap zoom process.
 */
typedef E_DjiCameraTapZoomState E_PsdkCameraTapZoomState;
#define PSDK_CAMERA_TAP_ZOOM_STATE_IDLE          DJI_CAMERA_TAP_ZOOM_STATE_IDLE /*!< Camera is not in tap zoom process. */
#define PSDK_CAMERA_TAP_ZOOM_STATE_ZOOM_IN       DJI_CAMERA_TAP_ZOOM_STATE_ZOOM_IN /*!< Camera is zooming in. */
#define PSDK_CAMERA_TAP_ZOOM_STATE_ZOOM_OUT      DJI_CAMERA_TAP_ZOOM_STATE_ZOOM_OUT /*!< Camera is zooming out. */
#define PSDK_CAMERA_TAP_ZOOM_STATE_ZOOM_LIMITED  DJI_CAMERA_TAP_ZOOM_STATE_ZOOM_LIMITED /*!< Camera has reached zoom limit. */

/**
 * @brief Camera video stream type.
 */
typedef E_DjiCameraVideoStreamType E_PsdkCameraVideoStreamType;
/*! Camera video stream by h264 custom format, which needs to comply with the user document Custom-H264 video stream format standard.
 * When using this format to send a video stream, the bit rate of the video stream needs must not exceed the real-time bandwidth
 * limit of the channel that feedback by interface #PsdkPayloadCamera_GetVideoStreamState.*/
#define PSDK_CAMERA_VIDEO_STREAM_TYPE_H264_CUSTOM_FORMAT  DJI_CAMERA_VIDEO_STREAM_TYPE_H264_CUSTOM_FORMAT
/*! Camera video stream by h264 DJI format, which needs to comply with the user document DJI-H264 video stream format standard.
 * When using this format to send a video stream, the video stream will be recoded by aircraft. No need to dynamically adjust
 * the bit rate to ensure the quality of transmission. But the bit rate of video stream can not exceed 8Mbps. */
#define PSDK_CAMERA_VIDEO_STREAM_TYPE_H264_DJI_FORMAT     DJI_CAMERA_VIDEO_STREAM_TYPE_H264_DJI_FORMAT

/**
 * @brief Camera sdcard state.
 */
typedef T_DjiCameraSDCardState T_PsdkCameraSDCardState;

/**
 * @brief Camera time interval settings when in interval shootPhoto mode.
 */
typedef T_DjiCameraPhotoTimeIntervalSettings T_PsdkCameraPhotoTimeIntervalSettings;

/**
 * @brief Camera metering target when in spot metering mode.
 */
typedef T_DjiCameraSpotMeteringTarget T_PsdkCameraSpotMeteringTarget;

/**
 * @brief Camera system state.
 */
typedef T_DjiCameraSystemState T_PsdkCameraSystemState;

/**
 * @brief Camera focus target point when in focus mode.
 */
typedef T_DjiCameraPointInScreen T_PsdkCameraPointInScreen;

/**
 * @brief Camera focus assistant settings.
 */
typedef T_DjiCameraFocusAssistantSettings T_PsdkCameraFocusAssistantSettings;

/**
 * @brief Camera playback status.
 */
typedef T_DjiCameraPlaybackStatus T_PsdkCameraPlaybackStatus;

/**
 * @brief Camera focus assistant settings.
 */
typedef T_DjiCameraMediaFileAttr T_PsdkCameraMediaFileAttr;

/**
 * @brief Camera media file info.
 */
typedef T_DjiCameraMediaFileInfo T_PsdkCameraMediaFileInfo;

/**
 * @brief Camera optical zoom specifies.
 */
typedef T_DjiCameraOpticalZoomSpec T_PsdkCameraOpticalZoomSpec;

/**
 * @brief Camera tap zoom state.
 */
typedef T_DjiCameraTapZoomState T_PsdkCameraTapZoomState;

/**
 * @brief Camera common features handler.
 * @warning User can not execute blocking style operations or functions in callback function, because that will block PSDK
 * root thread, causing problems such as slow system response, payload disconnection or infinite loop.
 */
typedef T_DjiCameraCommonHandler T_PsdkCameraCommonHandler;

/**
 * @brief Camera metering feature handler.
 * @warning User can not execute blocking style operations or functions in callback function, because that will block PSDK
 * root thread, causing problems such as slow system response, payload disconnection or infinite loop.
 */
typedef T_DjiCameraExposureMeteringHandler T_PsdkCameraExposureMeteringHandler;

/**
 * @brief Camera focus feature handler.
 * @warning User can not execute blocking style operations or functions in callback function, because that will block PSDK
 * root thread, causing problems such as slow system response, payload disconnection or infinite loop.
 */
typedef T_DjiCameraFocusHandler T_PsdkCameraFocusHandler;

/**
 * @brief Camera digital zoom feature handler.
 * @warning User can not execute blocking style operations or functions in callback function, because that will block PSDK
 * root thread, causing problems such as slow system response, payload disconnection or infinite loop.
 */
typedef T_DjiCameraDigitalZoomHandler T_PsdkCameraDigitalZoomHandler;

/**
 * @brief Camera optical zoom feature handler.
 * @warning User can not execute blocking style operations or functions in callback function, because that will block PSDK
 * root thread, causing problems such as slow system response, payload disconnection or infinite loop.
 */
typedef T_DjiCameraOpticalZoomHandler T_PsdkCameraOpticalZoomHandler;

/**
 * @brief Prototype of handler functions for media file download and playback.
 * @warning User can not execute blocking style operations or functions in callback function, because that will block PSDK
 * root thread, causing problems such as slow system response, payload disconnection or infinite loop.
 */
typedef T_DjiCameraMediaDownloadPlaybackHandler T_PsdkCameraMediaDownloadPlaybackHandler;

/**
 * @brief Prototype of handler functions for tap zooming.
 * @note User can not execute blocking style operations or functions in callback function, like PsdkXPort_RotateSync()
 * function, because that will block PSDK root thread, causing problems such as slow system response, payload
 * disconnection or infinite loop.
 */
typedef T_DjiCameraTapZoomHandler T_PsdkCameraTapZoomHandler;

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Initialize the payload camera module.
 * @note The interface is the total initialization interface of the camera module. The following interfaces are optional
 * functions and can be freely selected according to the actual load device condition of the user. Before registering
 * the optional interface, the interface must be initialized before the system can work normally. Interface initialization
 * needs to be after PsdkCore_Init.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkPayloadCamera_Init(void);

/**
 * @brief Register the handler for payload camera common function interfaces.
 * @note This interface registers the camera's basic function interface, including camera camera, video, mode settings,
 * SD card operation, camera status interface. Registration of this interface needs to be after PsdkPayloadCamera_Init.
 * @param cameraCommonHandler: pointer to the handler for payload camera common functions.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkPayloadCamera_RegCommonHandler(const T_PsdkCameraCommonHandler *cameraCommonHandler);

/**
 * @brief Register the handler for payload camera exposure and metering function interfaces.
 * @note This interface registers the spot metering and exposure interface of the camera. It currently includes
 * setting and acquiring the metering mode and metering area. Special attention must be paid to the limitations
 * of the interface parameters. Registration of this interface needs to be after PsdkPayloadCamera_Init.
 * @param cameraExposureMeteringHandler: pointer to the handler for payload camera exposure and metering functions.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkPayloadCamera_RegExposureMeteringHandler(const T_PsdkCameraExposureMeteringHandler
                                                              *cameraExposureMeteringHandler);

/**
 * @brief Register the handler for payload camera focus function interfaces.
 * @note This interface registers the camera's focus interface, which includes setting and acquiring the focus mode,
 * focus area, and focus settings. Special attention must be paid to the interface parameters. Registration of
 * this interface needs to be after PsdkPayloadCamera_Init.
 * @param cameraFocusHandler: pointer to the handler for payload camera focus functions.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkPayloadCamera_RegFocusHandler(const T_PsdkCameraFocusHandler *cameraFocusHandler);

/**
 * @brief Register the handler for payload camera digital zoom function interfaces.
 * @note This interface registers the camera's digital zoom interface, which includes setting and obtaining the digital
 * zoom zoom factor. Registering the load on this interface requires digital zoom. Registration of this interface
 * needs to be after PsdkPayloadCamera_Init.
 * @param cameraDigitalZoomHandler: pointer to the handler for payload camera digital zoom functions.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkPayloadCamera_RegDigitalZoomHandler(const T_PsdkCameraDigitalZoomHandler
                                                         *cameraDigitalZoomHandler);

/**
 * @brief Register the handler for payload camera optical zoom function interfaces.
 * @note This interface registers the camera's optical zoom interface, which includes setting and acquiring optical zoom
 * parameters, and a continuous zoom interface. Registering the load on this interface requires support for optical
 * zoom. Registration of this interface needs to be after PsdkPayloadCamera_Init.
 * @param cameraOpticalZoomHandler: pointer to the handler for payload camera optical zoom functions.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkPayloadCamera_RegOpticalZoomHandler(const T_PsdkCameraOpticalZoomHandler
                                                         *cameraOpticalZoomHandler);

/**
 * @brief Register handler functions for tap zoom function.
 * @details Registration specifies PSDK application support tap zoom function.
 * @param cameraTapZoomHandler: pointer to structure of handler functions for tap zoom function.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkPayloadCamera_RegTapZoomHandler(const T_PsdkCameraTapZoomHandler *cameraTapZoomHandler);

/**
 * @brief Set the type of camera video stream.
 * @note The interface is used to set the camera video stream type. Must ensure the format of video stream conforms to
 * the specified type in E_PsdkCameraVideoStreamType, please refer to developer documentation for more details.
 * @attention Set video stream type must before calling PsdkCore_ApplicationStart function, when calling this interface
 * the thread will be blocked, and the maximum blocking time is 10s. If this interface is not called, the default video
 * stream type ::PSDK_CAMERA_VIDEO_STREAM_TYPE_H264_CUSTOM_FORMAT will be used.
 * @param videoStreamType: camera video stream type.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkPayloadCamera_SetVideoStreamType(E_PsdkCameraVideoStreamType videoStreamType);

/**
 * @brief Get the network remote address for sending camera video stream.
 * @note The interface is used to get the network remote address for sending camera video stream. You can get this info for another
 * heterogeneous system to do somethings. This interface should be used after calling PsdkCore_Init function.
 * @attention If you want use this interface, should call PsdkPlatform_RegHalNetworkHandler interface firstly, otherwise
 * this interface will not work.
 * @param ipAddr: the remote ip address for sending camera video stream.
 * @param port: the remote port for sending camera video stream.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkPayloadCamera_GetVideoStreamRemoteAddress(char *ipAddr, uint16_t *port);

/**
 * @brief Register the handler for payload camera media download and playback function interfaces.
 * @note Registering the camera playback and downloading related interfaces, mainly able to operate the media files of
 * the user storage device online through the playback interface of the pilot. It can playback and download files
 * of pictures and videos. Currently, only media files in MP4 and JPG formats are supported.
 * @param cameraMediaHandler: pointer to the handler for payload camera media download and playback functions.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkPayloadCamera_RegMediaDownloadPlaybackHandler(const T_PsdkCameraMediaDownloadPlaybackHandler
                                                                   *cameraMediaHandler);

/**
 * @brief Send the video to mobile end via video stream of the data channel. This function exists and can be used only in Linux.
 * @note Must ensure actual bandwidth is less than bandwidth limitation of corresponding channel or stream, please
 * refer to developer documentation and state of channel/stream for details related to bandwidth limitation. User can
 * get state of "videoStream" channel via PsdkPayloadCamera_GetVideoStreamState() function. If actual bandwidth
 * exceeds the limitation or busy state is set, the user should stop transmitting data or decrease amount of data to be
 * sent. Otherwise, data may be discarded.
 * @param data: pointer to data to be sent.
 * @param len: length of data to be sent via data stream, and it must be less than or equal to 65000, unit: byte.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkPayloadCamera_SendVideoStream(const uint8_t *data, uint16_t len);

/**
 * @brief Get data transmission state of "videoStream" channel. User can use the state as base for controlling data
 * transmission of video stream. This function exists and can be used only in Linux operation system.
 * @param state: pointer to "videoStream" channel state.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkPayloadCamera_GetVideoStreamState(T_PsdkDataChannelState *state);

/**
 * @brief Push added media file information to aircraft system.
 * @details After photography or recording video, users must push information of created media file immediately using
 * this interface.
 * @param filePath: pointer to path of added media file. Guaranteed to end with '\0'. Length of path have to be less
 * than ::PSDK_FILE_PATH_SIZE_MAX bytes.
 * @param mediaFileInfo: information of added media file.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkPayloadCamera_PushAddedMediaFileInfo(const char *filePath,
                                                          T_PsdkCameraMediaFileInfo mediaFileInfo);

#ifdef __cplusplus
}
#endif

#endif // PSDK_PAYLOAD_CAMERA_H

/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
