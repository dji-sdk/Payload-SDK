/**
 ********************************************************************
 * @file    dji_payload_camera.h
 * @brief   This is the header file for "dji_payload_camera.c", defining the structure and
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
#ifndef DJI_PAYLOAD_CAMERA_H
#define DJI_PAYLOAD_CAMERA_H

/* Includes ------------------------------------------------------------------*/
#include "dji_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/**
 * @brief Camera work mode.
 */
typedef enum {
    DJI_CAMERA_MODE_SHOOT_PHOTO = 0, /*!< Shoot photo work mode. */
    DJI_CAMERA_MODE_RECORD_VIDEO = 1, /*!< Record video work mode. */
    DJI_CAMERA_MODE_PLAYBACK = 2, /*!< Media playback work mode. */
} E_DjiCameraMode;

/**
 * @brief Camera shoot photo mode.
 */
typedef enum {
    DJI_CAMERA_SHOOT_PHOTO_MODE_SINGLE = 1, /*!< Single photographing mode. */
    DJI_CAMERA_SHOOT_PHOTO_MODE_BURST = 4, /*!< Burst photographing mode. */
    DJI_CAMERA_SHOOT_PHOTO_MODE_INTERVAL = 6, /*!< Interval photographing mode. */
} E_DjiCameraShootPhotoMode;

/**
 * @brief Camera shooting state when photographing.
 */
typedef enum {
    DJI_CAMERA_SHOOTING_PHOTO_IDLE = 0, /*!< Photographing in idle state. */
    DJI_CAMERA_SHOOTING_SINGLE_PHOTO = 1, /*!< Photographing in single photograph state . */
    DJI_CAMERA_SHOOTING_BURST_PHOTO = 2, /*!< Photographing in burst photograph state. */
    DJI_CAMERA_SHOOTING_INTERVAL_PHOTO = 6, /*!< Photographing in interval photograph state. */
} E_DjiCameraShootingState;

/**
 * @brief Camera metering mode.
 */
typedef enum {
    DJI_CAMERA_METERING_MODE_CENTER = 0, /*!< Center metering mode. */
    DJI_CAMERA_METERING_MODE_AVERAGE = 1, /*!< Average metering mode. */
    DJI_CAMERA_METERING_MODE_SPOT = 2, /*!< Spot metering mode. */
} E_DjiCameraMeteringMode;

/**
 * @brief Camera focus mode.
 */
typedef enum {
    DJI_CAMERA_FOCUS_MODE_MANUAL = 0, /*!< Manual focus mode. */
    DJI_CAMERA_FOCUS_MODE_AUTO = 1, /*!< Auto focus mode. */
} E_DjiCameraFocusMode;

/**
 * @brief Camera playback mode in playback process.
 */
typedef enum {
    DJI_CAMERA_PLAYBACK_MODE_PLAY = 2, /*!< Play playbacking mode. */
    DJI_CAMERA_PLAYBACK_MODE_PAUSE = 3, /*!< Pause playbacking mode. */
    DJI_CAMERA_PLAYBACK_MODE_STOP = 7, /*!< Stop playbacking mode. */
} E_DjiCameraPlaybackMode;

/**
 * @brief Camera supported video frames when working in playback mode.
 */
typedef enum {
    DJI_CAMERA_VIDEO_FRAME_RATE_24_FPS = 13, /*!< The camera's video frame rate is 24fps (frames per second) */
    DJI_CAMERA_VIDEO_FRAME_RATE_25_FPS = 2, /*!< The camera's video frame rate is 25fps (frames per second) */
    DJI_CAMERA_VIDEO_FRAME_RATE_30_FPS = 14, /*!< The camera's video frame rate is 30fps (frames per second) */
    DJI_CAMERA_VIDEO_FRAME_RATE_UNKNOWN = 0, /*!< The camera's video frame rate is unknown (frames per second) */
} E_DjiCameraVideoFrameRate;

/**
 * @brief Camera supported video resolutions when working in playback mode.
 */
typedef enum {
    DJI_CAMERA_VIDEO_RESOLUTION_640x480 = 0, /*!< /The camera's video resolution is 640x480. */
    DJI_CAMERA_VIDEO_RESOLUTION_1280x720 = 4, /*!< /The camera's video resolution is 1280x720. */
    DJI_CAMERA_VIDEO_RESOLUTION_1920x1080 = 10, /*!< /The camera's video resolution is 1920x1080. */
    DJI_CAMERA_VIDEO_RESOLUTION_2048x1080 = 37, /*!< /The camera's video resolution is 2048x1080. */
    DJI_CAMERA_VIDEO_RESOLUTION_3840x2160 = 41, /*!< /The camera's video resolution is 3840x2160. */
    DJI_CAMERA_VIDEO_RESOLUTION_UNKNOWN = 255, /*!< /The camera's video resolution is unknown. */
} E_DjiCameraVideoResolution;

/**
 * @brief Camera zoom state in tap zoom process.
 */
typedef enum {
    DJI_CAMERA_TAP_ZOOM_STATE_IDLE = 0, /*!< Camera is not in tap zoom process. */
    DJI_CAMERA_TAP_ZOOM_STATE_ZOOM_IN = 1, /*!< Camera is zooming in. */
    DJI_CAMERA_TAP_ZOOM_STATE_ZOOM_OUT = 2, /*!< Camera is zooming out. */
    DJI_CAMERA_TAP_ZOOM_STATE_ZOOM_LIMITED = 3, /*!< Camera has reached zoom limit. */
} E_DjiCameraTapZoomState;

/**
 * @brief Camera video stream type.
 */
typedef enum {
    /*! Camera video stream by h264 custom format, which needs to comply with the user document Custom-H264 video stream format standard.
     * When using this format to send a video stream, the bit rate of the video stream needs must not exceed the real-time bandwidth
     * limit of the channel that feedback by interface #DjiPayloadCamera_GetVideoStreamState.*/
    DJI_CAMERA_VIDEO_STREAM_TYPE_H264_CUSTOM_FORMAT = 0,
    /*! Camera video stream by h264 DJI format, which needs to comply with the user document DJI-H264 video stream format standard.
     * When using this format to send a video stream, the video stream will be recoded by aircraft. No need to dynamically adjust
	 * the bit rate to ensure the quality of transmission. But the bit rate of video stream can not exceed 8Mbps. */
    DJI_CAMERA_VIDEO_STREAM_TYPE_H264_DJI_FORMAT = 1,
} E_DjiCameraVideoStreamType;

/**
 * @brief Camera sdcard state.
 */
typedef struct {
    bool isInserted; /*!< Specifies if the SD card is inserted in the camera. This parameter is boolean type. */
    bool isVerified; /*!< Specifies if the SD card is verified as genuine. This parameter is boolean type. */
    bool isInitializing; /*!< Specifies if the SD card is initializing. This parameter is boolean type. */
    bool isReadOnly; /*!< Specifies if the SD card is read only type. This parameter is boolean type. */
    bool isFormatting; /*!< Specifies if the SD card is in formatting process. This parameter is boolean type. */
    bool isFull; /*!< Specifies if the SD card's capacity is full. This parameter is boolean type. */
    bool isInvalidFormat;/*!< Specifies if the SD card is invalid formatted. This parameter is boolean type. */
    bool hasError; /*!< Specifies if the SD card has error. This parameter is boolean type. */
    uint32_t totalSpaceInMB; /*!< SD card total capacity, unit: MB. */
    uint32_t remainSpaceInMB; /*!< SD card remaining capacity, unit: MB. */
    uint32_t availableCaptureCount; /*!< Available capture photo count. */
    uint32_t availableRecordingTimeInSeconds; /*!< Available video recording second time, unit: s. */
} T_DjiCameraSDCardState;

/**
 * @brief Camera metering target when in spot metering mode.
 */
typedef struct {
    uint8_t col; /*!< Specifies column coordinate. This parameter is between 0 and 11. */
    uint8_t row; /*!< Specifies row coordinate. This parameter is between 0 and 7. */
} T_DjiCameraSpotMeteringTarget;

/**
 * @brief Camera system state.
 */
typedef struct {
    E_DjiCameraMode cameraMode; /*!< Specifies the camera current work mode, #E_DjiCameraMode. */
    E_DjiCameraShootingState shootingState; /*!< Specifies the camera state of shooting, #E_DjiCameraShootingState. */
    bool isStoring;/*!< Specifies if the camera is in storing status. This parameter is boolean type. */
    bool isShootingIntervalStart; /*!< Specifies if the camera is in interval shooting start status. This parameter is boolean type. */
    uint16_t currentPhotoShootingIntervalTimeInSeconds; /*!< Specifies the current interval shoot countdown time, the value is decreasing,
                                                         * when the value equals to zero trigger the interval take photo, uint:s. */
    uint16_t currentPhotoShootingIntervalCount; /*!< Specifies the current interval photo count, the value is decreasing step by one from
                                                 * the setted count when interval taking photo */
    bool isRecording; /*!< Specifies if the camera is in recording status. This parameter is boolean type. */
    uint16_t currentVideoRecordingTimeInSeconds; /*!< Specifies the current recording process time, uint:s. */
    bool isOverheating; /*!< Specifies if the camera is in overheating status. This parameter is boolean type. */
    bool hasError; /*!< Specifies if the camera in error status. This parameter is boolean type. */
} T_DjiCameraSystemState;

/**
 * @brief Camera focus assistant settings.
 */
typedef struct {
    bool isEnabledMF; /*!< Specifies if the lens focus assistant is enabled for manual focusing. This parameter is boolean type. */
    bool isEnabledAF; /*!< Specifies if the lens focus assistant is enabled for auto focusing. This parameter is boolean type. */
} T_DjiCameraFocusAssistantSettings;

/**
 * @brief Camera playback status.
 */
typedef struct {
    E_DjiCameraPlaybackMode playbackMode; /*!< Specifies the duration of video media file, #E_DjiCameraPlaybackMode. */
    uint8_t videoPlayProcess; /*!< Specifies the current play process of video media file. This parameter is between 0 and 100. */
    uint32_t videoLengthMs; /*!< Specifies the total duration of video media file, uint:ms. */
    uint32_t playPosMs; /*!< Specifies the current play position of video media file, uint:ms. */
} T_DjiCameraPlaybackStatus;

/**
 * @brief Camera focus assistant settings.
 */
typedef struct {
    uint16_t attrVideoDuration; /*!< Specifies the playback duration of video media file, uint:s. */
    uint16_t attrVideoFrameRate; /*!< Specifies the frame rate of video media file, uint:fps. */
    uint16_t attrVideoResolution; /*!< Specifies the resolution of video media file, uint:px. */
} T_DjiCameraMediaFileAttr;

/**
 * @brief Camera media file info.
 */
typedef struct {
    E_DjiCameraMediaFileType type; /*!< Specifies the type of media file, #E_DjiCameraMediaFileType. */
    T_DjiCameraMediaFileAttr mediaFileAttr; /*!< Specifies the attributes of media file. */
    uint32_t fileSize; /*!< Specifies the size of media file, uint:byte. */
} T_DjiCameraMediaFileInfo;

/**
 * @brief Camera tap zoom state.
 */
typedef struct {
    E_DjiCameraTapZoomState zoomState; /*!< Camera zoom state. */
    bool isGimbalMoving; /*!< Flag that specifies whether gimbal is moving for tap zoom. */
} T_DjiCameraTapZoomState;

/**
 * @brief Camera common features handler.
 * @warning User can not execute blocking style operations or functions in callback function, because that will block dji
 * root thread, causing problems such as slow system response, payload disconnection or infinite loop.
 */
typedef struct {
    /**
     * @brief Prototype of callback function used to get camera system state.
     * @param systemState: pointer to memory space used to store camera system state.
     * @return Execution result.
     */
    T_DjiReturnCode (*GetSystemState)(T_DjiCameraSystemState *systemState);

    /**
     * @brief Prototype of callback function used to set camera work mode.
     * @note Sets the camera's work mode to taking pictures, video, playback. Please note that you cannot change the mode
     * when a certain taskHandle is executing, such as taking photo(s), recording video, or downloading and saving files. Also
     * supported by thermal imaging camera.
     * @param mode: camera work mode.
     * @return Execution result.
     */
    T_DjiReturnCode (*SetMode)(E_DjiCameraMode mode);

    /**
     * @brief Prototype of callback function used to get camera current work mode.
     * @param mode: pointer to memory space used to store camera work mode.
     * @return Execution result.
     */
    T_DjiReturnCode (*GetMode)(E_DjiCameraMode *mode);

    /**
     * @brief Prototype of callback function used to start record video.
     * @return Execution result.
     */
    T_DjiReturnCode (*StartRecordVideo)(void);

    /**
     * @brief Prototype of callback function used to stop record video.
     * @return Execution result.
     */
    T_DjiReturnCode (*StopRecordVideo)(void);

    /**
     * @brief Prototype of callback function used to start shoot photo.
     * @return Execution result.
     */
    T_DjiReturnCode (*StartShootPhoto)(void);

    /**
     * @brief Prototype of callback function used to stop shoot photo.
     * @return Execution result.
     */
    T_DjiReturnCode (*StopShootPhoto)(void);

    /**
     * @brief Prototype of callback function used to set camera shoot photo mode.
     * @param mode: camera shoot photo mode.
     * @return Execution result.
     */
    T_DjiReturnCode (*SetShootPhotoMode)(E_DjiCameraShootPhotoMode mode);

    /**
     * @brief Prototype of callback function used to get camera current shoot photo mode.
     * @param mode: pointer to memory space used to store camera shoot photo mode.
     * @return Execution result.
     */
    T_DjiReturnCode (*GetShootPhotoMode)(E_DjiCameraShootPhotoMode *mode);

    /**
     * @brief Prototype of callback function used to set camera shoot burst count.
     * @param burstCount: camera shoot burst count.
     * @return Execution result.
     */
    T_DjiReturnCode (*SetPhotoBurstCount)(E_DjiCameraBurstCount burstCount);

    /**
     * @brief Prototype of callback function used to get camera current burst count.
     * @param burstCount: pointer to memory space used to store camera shoot burst count.
     * @return Execution result.
     */
    T_DjiReturnCode (*GetPhotoBurstCount)(E_DjiCameraBurstCount *burstCount);

    /**
     * @brief Prototype of callback function used to set camera shoot time interval settings.
     * @note The value range of interval photograph count is [2, 255]. If 255 is selected, then the camera will continue
     * to take pictures until StopShootPhoto is called.
     * @param settings: camera shoot time interval settings.
     * @return Execution result.
     */
    T_DjiReturnCode (*SetPhotoTimeIntervalSettings)(T_DjiCameraPhotoTimeIntervalSettings settings);

    /**
     * @brief Prototype of callback function used to get camera shoot time interval settings.
     * @param settings: pointer to memory space used to store camera shoot time interval settings.
     * @return Execution result.
     */
    T_DjiReturnCode (*GetPhotoTimeIntervalSettings)(T_DjiCameraPhotoTimeIntervalSettings *settings);

    /**
     * @brief Prototype of callback function used to get camera current SDCard state.
     * @param sdCardState: pointer to memory space used to store camera SDCard state.
     * @return Execution result.
     */
    T_DjiReturnCode (*GetSDCardState)(T_DjiCameraSDCardState *sdCardState);

    /**
     * @brief Prototype of callback function used to format the SDCard inserted.
     * @return Execution result.
     */
    T_DjiReturnCode (*FormatSDCard)(void);
} T_DjiCameraCommonHandler;

/**
 * @brief Camera metering feature handler.
 * @warning User can not execute blocking style operations or functions in callback function, because that will block dji
 * root thread, causing problems such as slow system response, payload disconnection or infinite loop.
 */
typedef struct {
    /**
     * @brief Prototype of callback function used to set camera metering mode.
     * @param mode: camera metering mode.
     * @return Execution result.
     */
    T_DjiReturnCode (*SetMeteringMode)(E_DjiCameraMeteringMode mode);

    /**
     * @brief Prototype of callback function used to get camera current metering mode.
     * @param mode: pointer to memory space used to store camera metering mode.
     * @return Execution result.
     */
    T_DjiReturnCode (*GetMeteringMode)(E_DjiCameraMeteringMode *mode);

    /**
     * @brief Prototype of callback function used to set camera spot metering target area.
     * @param target: camera spot metering target area.
     * @return Execution result.
     */
    T_DjiReturnCode (*SetSpotMeteringTarget)(T_DjiCameraSpotMeteringTarget target);

    /**
     * @brief Prototype of callback function used to get camera spot metering target area.
     * @param target: pointer to memory space used to store camera spot metering target area.
     * @return Execution result.
     */
    T_DjiReturnCode (*GetSpotMeteringTarget)(T_DjiCameraSpotMeteringTarget *target);
} T_DjiCameraExposureMeteringHandler;

/**
 * @brief Camera focus feature handler.
 * @warning User can not execute blocking style operations or functions in callback function, because that will block dji
 * root thread, causing problems such as slow system response, payload disconnection or infinite loop.
 */
typedef struct {
    /**
     * @brief Prototype of callback function used to set camera focus mode.
     * @param mode: camera focus mode.
     * @return Execution result.
     */
    T_DjiReturnCode (*SetFocusMode)(E_DjiCameraFocusMode mode);

    /**
     * @brief Prototype of callback function used to get camera current focus mode.
     * @param mode: pointer to memory space used to store camera focus mode.
     * @return Execution result.
     */
    T_DjiReturnCode (*GetFocusMode)(E_DjiCameraFocusMode *mode);

    /**
     * @brief Prototype of callback function used to set camera focus target area.
     * @param target: camera focus target area.
     * @return Execution result.
     */
    T_DjiReturnCode (*SetFocusTarget)(T_DjiCameraPointInScreen target);

    /**
     * @brief Prototype of callback function used to get camera focus target area.
     * @param target: pointer to memory space used to store camera focus target area.
     * @return Execution result.
     */
    T_DjiReturnCode (*GetFocusTarget)(T_DjiCameraPointInScreen *target);

    /**
     * @brief Prototype of callback function used to set camera focus assistant settings.
     * @param settings: camera focus assistant settings.
     * @return Execution result.
     */
    T_DjiReturnCode (*SetFocusAssistantSettings)(T_DjiCameraFocusAssistantSettings settings);

    /**
     * @brief Prototype of callback function used to get camera focus assistant settings.
     * @param settings: pointer to memory space used to store camera focus assistant settings.
     * @return Execution result.
     */
    T_DjiReturnCode (*GetFocusAssistantSettings)(T_DjiCameraFocusAssistantSettings *settings);

    /**
     * @brief Prototype of callback function used to set camera focus ring value.
     * @param value: camera focus ring value.
     * @return Execution result.
     */
    T_DjiReturnCode (*SetFocusRingValue)(uint32_t value);

    /**
     * @brief Prototype of callback function used to get camera focus ring value.
     * @param value: pointer to memory space used to store camera focus ring value.
     * @return Execution result.
     */
    T_DjiReturnCode (*GetFocusRingValue)(uint32_t *value);

    /**
     * @brief Prototype of callback function used to get camera focus upper bound ring value.
     * @param value: pointer to memory space used to store camera focus upper bound value.
     * @return Execution result.
     */
    T_DjiReturnCode (*GetFocusRingValueUpperBound)(uint32_t *value);
} T_DjiCameraFocusHandler;

/**
 * @brief Camera digital zoom feature handler.
 * @warning User can not execute blocking style operations or functions in callback function, because that will block dji
 * root thread, causing problems such as slow system response, payload disconnection or infinite loop.
 */
typedef struct {
    /**
     * @brief Prototype of callback function used to set camera digital zoom factor.
     * @param factor: camera digital zoom factor.
     * @return Execution result.
     */
    T_DjiReturnCode (*SetDigitalZoomFactor)(dji_f32_t factor);

    /**
     * @brief Prototype of callback function used to get camera current digital zoom factor.
     * @param factor: pointer to memory space used to store camera current digital zoom factor.
     * @return Execution result.
     */
    T_DjiReturnCode (*GetDigitalZoomFactor)(dji_f32_t *factor);
} T_DjiCameraDigitalZoomHandler;

/**
 * @brief Camera optical zoom feature handler.
 * @warning User can not execute blocking style operations or functions in callback function, because that will block dji
 * root thread, causing problems such as slow system response, payload disconnection or infinite loop.
 */
typedef struct {
    /**
     * @brief Prototype of callback function used to set camera optical zoom focal length.
     * @param focalLength: camera optical zoom focal length.
     * @return Execution result.
     */
    T_DjiReturnCode (*SetOpticalZoomFocalLength)(uint32_t focalLength);

    /**
     * @brief Prototype of callback function used to get camera optical zoom focal length.
     * @param focalLength: pointer to memory space used to store camera optical zoom focal length.
     * @return Execution result.
     */
    T_DjiReturnCode (*GetOpticalZoomFocalLength)(uint32_t *focalLength);

    /**
     * @brief Prototype of callback function used to get camera optical zoom factor.
     * @param factor: pointer to memory space used to store camera optical zoom factor.
     * @return Execution result.
     */
    T_DjiReturnCode (*GetOpticalZoomFactor)(dji_f32_t *factor);

    /**
     * @brief Prototype of callback function used to get camera optical zoom specifies.
     * @param spec: pointer to memory space used to store camera optical zoom specifies.
     * @return Execution result.
     */
    T_DjiReturnCode (*GetOpticalZoomSpec)(T_DjiCameraOpticalZoomSpec *spec);

    /**
     * @brief Prototype of callback function used to start continuous optical zoom.
     * @param direction: camera zoom direction.
     * @param speed: camera zoom speed.
     * @return Execution result.
     */
    T_DjiReturnCode (*StartContinuousOpticalZoom)(E_DjiCameraZoomDirection direction, E_DjiCameraZoomSpeed speed);

    /**
     * @brief Prototype of callback function used to stop continuous optical zoom.
     * @return Execution result.
     */
    T_DjiReturnCode (*StopContinuousOpticalZoom)(void);
} T_DjiCameraOpticalZoomHandler;

/**
 * @brief Prototype of handler functions for media file download and playback.
 * @warning User can not execute blocking style operations or functions in callback function, because that will block dji
 * root thread, causing problems such as slow system response, payload disconnection or infinite loop.
 */
typedef struct {
    /**
     * @brief Prototype of callback function used to get camera media file path.
     * @warning The maximum length of path that users can pass in is 256, including end character '\0'.
     * @param dirPath: pointer to memory space used to store camera media file path.
     * @return Execution result.
     */
    T_DjiReturnCode (*GetMediaFileDir)(char *dirPath);

    /**
     * @brief Prototype of callback function used to get the file info of the selected origin media file.
     * @param dirPath: pointer to memory space used to store camera current media file path.
     * @param fileInfo: pointer to memory space used to store file info of the selected origin media file.
     * @return Execution result.
     */
    T_DjiReturnCode (*GetMediaFileOriginInfo)(const char *filePath, T_DjiCameraMediaFileInfo *fileInfo);

    /**
     * @brief Prototype of callback function used to get the data of the selected origin media file.
     * @param dirPath: pointer to memory space used to store camera current media file path.
     * @param offset: the offset of origin file data that need get from the selected media file.
     * @param length: the length of origin file data that need get from the selected media file.
     * @param data: pointer to memory space used to store the selected origin media file.
     * @return Execution result.
     */
    T_DjiReturnCode (*GetMediaFileOriginData)(const char *filePath, uint32_t offset, uint32_t length, uint8_t *data);

    /**
      * @brief Prototype of callback function used to create the thumb nail of selected media file.
      * @param dirPath: pointer to memory space used to store camera current media file path.
      * @return Execution result.
      */
    T_DjiReturnCode (*CreateMediaFileThumbNail)(const char *filePath);

    /**
     * @brief Prototype of callback function used to get the file info of the created thumbnail.
     * @param dirPath: pointer to memory space used to store camera current media file path.
     * @param fileInfo: pointer to memory space used to store file info of the created thumbnail.
     * @return Execution result.
     */
    T_DjiReturnCode (*GetMediaFileThumbNailInfo)(const char *filePath, T_DjiCameraMediaFileInfo *fileInfo);

    /**
      * @brief Prototype of callback function used to get the data of the created thumbnail.
      * @param dirPath: pointer to memory space used to store camera current media file path.
      * @param offset: the offset of thumb nail data that need get from the selected media file.
      * @param length: the length of thumb nail data that need get from the selected media file.
      * @param data: pointer to memory space used to store the created thumbnail data.
      * @return Execution result.
      */
    T_DjiReturnCode (*GetMediaFileThumbNailData)(const char *filePath, uint32_t offset, uint32_t length,
                                                 uint8_t *data);

    /**
      * @brief Prototype of callback function used to destroy the created thumb nail of selected media file.
      * @param dirPath: pointer to memory space used to store camera current media file path.
      * @return Execution result.
      */
    T_DjiReturnCode (*DestroyMediaFileThumbNail)(const char *filePath);

    /**
      * @brief Prototype of callback function used to create the screen nail of selected media file.
      * @param dirPath: pointer to memory space used to store camera current media file path.
      * @return Execution result.
      */
    T_DjiReturnCode (*CreateMediaFileScreenNail)(const char *filePath);

    /**
      * @brief Prototype of callback function used to get the data of the created screen nail.
      * @param dirPath: pointer to memory space used to store camera current media file path.
      * @param offset: the offset of screen nail data that need get from selected media file.
      * @param length: the length of screen nail data that need get from selected media file.
      * @param data: pointer to memory space used to store the created screen nail data.
      * @return Execution result.
      */
    T_DjiReturnCode (*GetMediaFileScreenNailData)(const char *filePath, uint32_t offset, uint32_t length,
                                                  uint8_t *data);

    /**
     * @brief Prototype of callback function used to get the file info of the created screen nail.
     * @param dirPath: pointer to memory space used to store camera current media file path.
     * @param fileInfo: pointer to memory space used to store file info of the created screen nail.
     * @return Execution result.
     */
    T_DjiReturnCode (*GetMediaFileScreenNailInfo)(const char *filePath, T_DjiCameraMediaFileInfo *fileInfo);

    /**
      * @brief Prototype of callback function used to destroy the created screen nail of selected media file.
      * @param dirPath: pointer to memory space used to store camera current media file path.
      * @return Execution result.
      */
    T_DjiReturnCode (*DestroyMediaFileScreenNail)(const char *filePath);

    /**
     * @brief Prototype of callback function used to notification when download starting.
     * @note You can adjust the bandwidth proportion of each high speed channel in the notification callback.
     * @return Execution result.
     */
    T_DjiReturnCode (*StartDownloadNotification)(void);

    /**
     * @brief Prototype of callback function used to notification when download stoping.
     * @note You can adjust the bandwidth proportion of each high speed channel in the notification callback.
     * @return Execution result.
     */
    T_DjiReturnCode (*StopDownloadNotification)(void);

    /**
      * @brief Prototype of callback function used to delete the user's media files.
      * @param filePath: pointer to memory space used to store camera current media file path.
      * @return Execution result.
      */
    T_DjiReturnCode (*DeleteMediaFile)(char *filePath);

    /**
      * @brief Prototype of callback function used to get camera media playback status.
      * @param status: pointer to memory space used to store camera media file path.
      * @return Execution result.
      */
    T_DjiReturnCode (*GetMediaPlaybackStatus)(T_DjiCameraPlaybackStatus *status);

    /**
      * @brief Prototype of callback function used to set the file path of media file that need playback.
      * @param filePath: pointer to memory space used to store the file path of media file that need playback.
      * @return Execution result.
      */
    T_DjiReturnCode (*SetMediaPlaybackFile)(const char *filePath);

    /**
      * @brief Prototype of callback function used to start media video playback when camera work in playback mode.
      * @return Execution result.
      */
    T_DjiReturnCode (*StartMediaPlayback)(void);

    /**
      * @brief Prototype of callback function used to stop media video playback when camera work in playback mode.
      * @return Execution result.
      */
    T_DjiReturnCode (*StopMediaPlayback)(void);

    /**
      * @brief Prototype of callback function used to pause media video playback when camera work in playback mode.
      * @return Execution result.
      */
    T_DjiReturnCode (*PauseMediaPlayback)(void);

    /**
      * @brief Prototype of callback function used to seek to the playback position of media video when camera work in playback mode.
      * @param playbackPosition: playback position of video media, unit: ms.
      * @return Execution result.
      */
    T_DjiReturnCode (*SeekMediaPlayback)(uint32_t playbackPosition);
} T_DjiCameraMediaDownloadPlaybackHandler;

/**
 * @brief Prototype of handler functions for tap zooming.
 * @note User can not execute blocking style operations or functions in callback function, like djiXPort_RotateSync()
 * function, because that will block dji root thread, causing problems such as slow system response, payload
 * disconnection or infinite loop.
 */
typedef struct {
    /**
     * @brief Prototype of callback function used to get tap zoom state.
     * @param state: pointer to memory space used to store tap zoom state.
     * @return Execution result.
     */
    T_DjiReturnCode (*GetTapZoomState)(T_DjiCameraTapZoomState *state);

    /**
     * @brief Prototype of callback function used to enable or disable tap zoom function.
     * @details dji application should response tap zoom command only if tap zoom function is enabled.
     * @param enabledFlag: enabled flag.
     * @return Execution result.
     */
    T_DjiReturnCode (*SetTapZoomEnabled)(bool enabledFlag);

    /**
     * @brief Prototype of callback function used to get the flag that specifies whether tap zoom function has been
     * enabled,
     * @param enabledFlag: pointer to memory space used to store enabled flag.
     * @return Execution result.
     */
    T_DjiReturnCode (*GetTapZoomEnabled)(bool *enabledFlag);

    /**
     * @brief Prototype of callback function used to set multiplier of single tap zoom.
     * @param multiplier: multiplier of single tap zoom.
     * @return Execution result.
     */
    T_DjiReturnCode (*SetTapZoomMultiplier)(uint8_t multiplier);

    /**
     * @brief Prototype of callback function used to get multiplier of single tap zoom.
     * @param multiplier: pointer to memory space use to store multiplier of single tap zoom.
     * @return Execution result.
     */
    T_DjiReturnCode (*GetTapZoomMultiplier)(uint8_t *multiplier);

    /**
     * @brief Prototype of callback function used to trigger tap zoom.
     * @details Users trigger tap zoom in screen of mobile end, this callback function will be called. Then, dji
     * application should rotate gimbal to orientation of target point and zoom by specified multiplier.
     * @param target: position information of target point in screen.
     * @return Execution result.
     */
    T_DjiReturnCode (*TapZoomAtTarget)(T_DjiCameraPointInScreen target);
} T_DjiCameraTapZoomHandler;

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Initialize the payload camera module.
 * @note The interface is the total initialization interface of the camera module. The following interfaces are optional
 * functions and can be freely selected according to the actual load device condition of the user. Before registering
 * the optional interface, the interface must be initialized before the system can work normally. Interface initialization
 * needs to be after djiCore_Init.
 * @return Execution result.
 */
T_DjiReturnCode DjiPayloadCamera_Init(void);

/**
 * @brief Register the handler for payload camera common function interfaces.
 * @note This interface registers the camera's basic function interface, including camera camera, video, mode settings,
 * SD card operation, camera status interface. Registration of this interface needs to be after DjiPayloadCamera_Init.
 * @param cameraCommonHandler: pointer to the handler for payload camera common functions.
 * @return Execution result.
 */
T_DjiReturnCode DjiPayloadCamera_RegCommonHandler(const T_DjiCameraCommonHandler *cameraCommonHandler);

/**
 * @brief Register the handler for payload camera exposure and metering function interfaces.
 * @note This interface registers the spot metering and exposure interface of the camera. It currently includes
 * setting and acquiring the metering mode and metering area. Special attention must be paid to the limitations
 * of the interface parameters. Registration of this interface needs to be after DjiPayloadCamera_Init.
 * @param cameraExposureMeteringHandler: pointer to the handler for payload camera exposure and metering functions.
 * @return Execution result.
 */
T_DjiReturnCode DjiPayloadCamera_RegExposureMeteringHandler(const T_DjiCameraExposureMeteringHandler
                                                            *cameraExposureMeteringHandler);

/**
 * @brief Register the handler for payload camera focus function interfaces.
 * @note This interface registers the camera's focus interface, which includes setting and acquiring the focus mode,
 * focus area, and focus settings. Special attention must be paid to the interface parameters. Registration of
 * this interface needs to be after DjiPayloadCamera_Init.
 * @param cameraFocusHandler: pointer to the handler for payload camera focus functions.
 * @return Execution result.
 */
T_DjiReturnCode DjiPayloadCamera_RegFocusHandler(const T_DjiCameraFocusHandler *cameraFocusHandler);

/**
 * @brief Register the handler for payload camera digital zoom function interfaces.
 * @note This interface registers the camera's digital zoom interface, which includes setting and obtaining the digital
 * zoom zoom factor. Registering the load on this interface requires digital zoom. Registration of this interface
 * needs to be after DjiPayloadCamera_Init.
 * @param cameraDigitalZoomHandler: pointer to the handler for payload camera digital zoom functions.
 * @return Execution result.
 */
T_DjiReturnCode DjiPayloadCamera_RegDigitalZoomHandler(const T_DjiCameraDigitalZoomHandler
                                                       *cameraDigitalZoomHandler);

/**
 * @brief Register the handler for payload camera optical zoom function interfaces.
 * @note This interface registers the camera's optical zoom interface, which includes setting and acquiring optical zoom
 * parameters, and a continuous zoom interface. Registering the load on this interface requires support for optical
 * zoom. Registration of this interface needs to be after DjiPayloadCamera_Init.
 * @param cameraOpticalZoomHandler: pointer to the handler for payload camera optical zoom functions.
 * @return Execution result.
 */
T_DjiReturnCode DjiPayloadCamera_RegOpticalZoomHandler(const T_DjiCameraOpticalZoomHandler
                                                       *cameraOpticalZoomHandler);

/**
 * @brief Register handler functions for tap zoom function.
 * @details Registration specifies dji application support tap zoom function.
 * @param cameraTapZoomHandler: pointer to structure of handler functions for tap zoom function.
 * @return Execution result.
 */
T_DjiReturnCode DjiPayloadCamera_RegTapZoomHandler(const T_DjiCameraTapZoomHandler *cameraTapZoomHandler);

/**
 * @brief Set the type of camera video stream.
 * @note The interface is used to set the camera video stream type. Must ensure the format of video stream conforms to
 * the specified type in E_DjiCameraVideoStreamType, please refer to developer documentation for more details.
 * @attention Set video stream type must before calling djiCore_ApplicationStart function, when calling this interface
 * the thread will be blocked, and the maximum blocking time is 10s. If this interface is not called, the default video
 * stream type ::DJI_CAMERA_VIDEO_STREAM_TYPE_H264_CUSTOM_FORMAT will be used.
 * @param videoStreamType: camera video stream type.
 * @return Execution result.
 */
T_DjiReturnCode DjiPayloadCamera_SetVideoStreamType(E_DjiCameraVideoStreamType videoStreamType);

/**
 * @brief Get the network remote address for sending camera video stream.
 * @note The interface is used to get the network remote address for sending camera video stream. You can get this info for another
 * heterogeneous system to do somethings. This interface should be used after calling djiCore_Init function.
 * @attention If you want use this interface, should call djiPlatform_RegHalNetworkHandler interface firstly, otherwise
 * this interface will not work.
 * @param ipAddr: the remote ip address for sending camera video stream.
 * @param port: the remote port for sending camera video stream.
 * @return Execution result.
 */
T_DjiReturnCode DjiPayloadCamera_GetVideoStreamRemoteAddress(char *ipAddr, uint16_t *port);

/**
 * @brief Register the handler for payload camera media download and playback function interfaces.
 * @note Registering the camera playback and downloading related interfaces, mainly able to operate the media files of
 * the user storage device online through the playback interface of the pilot. It can playback and download files
 * of pictures and videos`. Currently, only media files in MP4 and JPG formats are supported.
 * @param cameraMediaHandler: pointer to the handler for payload camera media download and playback functions.
 * @return Execution result.
 */
T_DjiReturnCode DjiPayloadCamera_RegMediaDownloadPlaybackHandler(const T_DjiCameraMediaDownloadPlaybackHandler
                                                                 *cameraMediaHandler);
/**
 * @brief Send the video to mobile end via video stream of the data channel. This function exists and can be used only in Linux.
 * @note Must ensure actual bandwidth is less than bandwidth limitation of corresponding channel or stream, please
 * refer to developer documentation and state of channel/stream for details related to bandwidth limitation. User can
 * get state of "videoStream" channel via DjiPayloadCamera_GetVideoStreamState() function. If actual bandwidth
 * exceeds the limitation or busy state is set, the user should stop transmitting data or decrease amount of data to be
 * sent. Otherwise, data may be discarded.
 * @param data: pointer to data to be sent.
 * @param len: length of data to be sent via data stream, and it must be less than or equal to 65000, unit: byte.
 * @return Execution result.
 */
T_DjiReturnCode DjiPayloadCamera_SendVideoStream(const uint8_t *data, uint16_t len);

/**
 * @brief Get data transmission state of "videoStream" channel. User can use the state as base for controlling data
 * transmission of video stream. This function exists and can be used only in Linux operation system.
 * @param state: pointer to "videoStream" channel state.
 * @return Execution result.
 */
T_DjiReturnCode DjiPayloadCamera_GetVideoStreamState(T_DjiDataChannelState *state);

/**
 * @brief Push added media file information to aircraft system.
 * @details After photography or recording video, users must push information of created media file immediately using
 * this interface.
 * @param filePath: pointer to path of added media file. Guaranteed to end with '\0'. Length of path have to be less
 * than ::dji_FILE_PATH_SIZE_MAX bytes.
 * @param mediaFileInfo: information of added media file.
 * @return Execution result.
 */
T_DjiReturnCode DjiPayloadCamera_PushAddedMediaFileInfo(const char *filePath, T_DjiCameraMediaFileInfo mediaFileInfo);

/**
 * @brief Get camera type of other payloads mounted on aircraft.
 * @note Please refer to DJI documentation in DJI SDK developer website for details of conditions of use.
 * @note If there is empty in requested position or payload do not have camera type, the interface will return error.
 * @param payloadPosition: position where payload mounted on.
 * @param cameraType: Pointer to camera type of payload.
 * @return Execution result.
 */
T_DjiReturnCode DjiPayloadCamera_GetCameraTypeOfPayload(E_DjiMountPosition payloadPosition,
                                                        E_DjiCameraType *cameraType);

/**
 * @brief Get optical zoom specification of other camera payloads mounted on aircraft.
 * @note Please refer to DJI documentation in DJI SDK developer website for details of conditions of use.
 * @note If there is empty in requested position or payload do not have related information, the interface will return
 * error.
 * @param payloadPosition: position where payload mounted on.
 * @param opticalZoomSpec: pointer to optical zoom specification.
 * @return Execution result.
 */
T_DjiReturnCode DjiPayloadCamera_GetCameraOpticalZoomSpecOfPayload(E_DjiMountPosition payloadPosition,
                                                                   T_DjiCameraOpticalZoomSpec *opticalZoomSpec);

/**
 * @brief Get hybrid zoom focal length of other camera payloads mounted on aircraft.
 * @note Please refer to DJI documentation in DJI SDK developer website for details of conditions of use.
 * @note If there is empty in requested position or payload do not have related information, the interface will return
 * error.
 * @param payloadPosition: position where payload mounted on.
 * @param focalLength: Pointer to optical focal length, unit: 0.1mm.
 * @return Execution result.
 */
T_DjiReturnCode DjiPayloadCamera_GetCameraHybridZoomFocalLengthOfPayload(E_DjiMountPosition payloadPosition,
                                                                         uint16_t *focalLength);

#ifdef __cplusplus
}
#endif

#endif // DJI_PAYLOAD_CAMERA_H

/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
