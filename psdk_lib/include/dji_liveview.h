/**
 ********************************************************************
 * @file    dji_liveview.h
 * @brief   This is the header file for "dji_liveview.c", defining the structure and
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
#ifndef DJI_LIVEVIEW_H
#define DJI_LIVEVIEW_H

/* Includes ------------------------------------------------------------------*/
#include "dji_typedef.h"
#include "dji_sei_parser.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/**
 * @brief Liveview camera mount position.
 */
typedef enum {
    DJI_LIVEVIEW_CAMERA_POSITION_NO_1 = DJI_MOUNT_POSITION_PAYLOAD_PORT_NO1,
    DJI_LIVEVIEW_CAMERA_POSITION_NO_2 = DJI_MOUNT_POSITION_PAYLOAD_PORT_NO2,
    DJI_LIVEVIEW_CAMERA_POSITION_NO_3 = DJI_MOUNT_POSITION_PAYLOAD_PORT_NO3,
    DJI_LIVEVIEW_CAMERA_POSITION_FPV = 7
} E_DjiLiveViewCameraPosition;

/**
 * @brief Image format.
 */
typedef enum {
    PIXFMT_NV12 = 3,
    PIXFMT_RGB_PLANAR = 4,
    PIXFMT_RGB_PACKED = 5
} E_DjiLiveViewPixFormate;

/**
 * @brief Liveview camera stream source.
 */
typedef enum {
    DJI_LIVEVIEW_CAMERA_SOURCE_DEFAULT = 0,
    DJI_LIVEVIEW_CAMERA_SOURCE_H20_WIDE = 1,
    DJI_LIVEVIEW_CAMERA_SOURCE_H20_ZOOM = 2,
    DJI_LIVEVIEW_CAMERA_SOURCE_H20T_WIDE = 1,
    DJI_LIVEVIEW_CAMERA_SOURCE_H20T_ZOOM = 2,
    DJI_LIVEVIEW_CAMERA_SOURCE_H20T_IR = 3,
    DJI_LIVEVIEW_CAMERA_SOURCE_H20N_WIDE = 1,
    DJI_LIVEVIEW_CAMERA_SOURCE_H20N_ZOOM = 2,
    DJI_LIVEVIEW_CAMERA_SOURCE_H20N_IR = 3,
    DJI_LIVEVIEW_CAMERA_SOURCE_M30_ZOOM = 1,
    DJI_LIVEVIEW_CAMERA_SOURCE_M30_WIDE = 2,
    DJI_LIVEVIEW_CAMERA_SOURCE_M30T_ZOOM = 1,
    DJI_LIVEVIEW_CAMERA_SOURCE_M30T_WIDE = 2,
    DJI_LIVEVIEW_CAMERA_SOURCE_M30T_IR = 3,
    DJI_LIVEVIEW_CAMERA_SOURCE_M3E_VIS = 1,
    DJI_LIVEVIEW_CAMERA_SOURCE_M3T_VIS = 1,
    DJI_LIVEVIEW_CAMERA_SOURCE_M3T_IR = 2,
    DJI_LIVEVIEW_CAMERA_SOURCE_M3D_VIS = 1,
    DJI_LIVEVIEW_CAMERA_SOURCE_M3TD_VIS = 1,
    DJI_LIVEVIEW_CAMERA_SOURCE_M3TD_IR = 2,
    DJI_LIVEVIEW_CAMERA_SOURCE_H30_ZOOM = 1,
    DJI_LIVEVIEW_CAMERA_SOURCE_H30_4K = 7,
    DJI_LIVEVIEW_CAMERA_SOURCE_M4E_VIS = 1,
    DJI_LIVEVIEW_CAMERA_SOURCE_M4E_4K = 3,
    DJI_LIVEVIEW_CAMERA_SOURCE_M4T_VIS = 1,
    DJI_LIVEVIEW_CAMERA_SOURCE_M4T_IR = 2,
    DJI_LIVEVIEW_CAMERA_SOURCE_M4T_4K = 3,
    DJI_LIVEVIEW_CAMERA_SOURCE_M4D_VIS = 1,
    DJI_LIVEVIEW_CAMERA_SOURCE_M4D_4K = 3,
    DJI_LIVEVIEW_CAMERA_SOURCE_M4TD_VIS = 1,
    DJI_LIVEVIEW_CAMERA_SOURCE_M4TD_IR = 2,
    DJI_LIVEVIEW_CAMERA_SOURCE_M4TD_4K = 3,
    DJI_LIVEVIEW_CAMERA_SOURCE_M5TD_VIS = 1,
    DJI_LIVEVIEW_CAMERA_SOURCE_M5TD_IR  = 2,
    DJI_LIVEVIEW_CAMERA_SOURCE_M5TD_4K  = 3,
} E_DjiLiveViewCameraSource;

/**
 * @brief DJI standard types of target recognition.
 */
typedef enum {
    DJI_LIVEVIEW_OBJ_TYPE_INVALID = 0,
    DJI_LIVEVIEW_OBJ_TYPE_UNKNOWN = 1,
    DJI_LIVEVIEW_OBJ_TYPE_PERSON = 2,
    DJI_LIVEVIEW_OBJ_TYPE_CAR = 3,
    DJI_LIVEVIEW_OBJ_TYPE_BOAT = 4,
    DJI_LIVEVIEW_OBJ_TYPE_HUMAN_FACE = 5,
    DJI_LIVEVIEW_OBJ_TYPE_BIRD = 10,
    DJI_LIVEVIEW_OBJ_TYPE_BEACON = 20,
    DJI_LIVEVIEW_OBJ_TYPE_GPS = 30,
    DJI_LIVEVIEW_OBJ_TYPE_MOVING_TARGET = 34,
} E_DjiLiveViewTargetObjectType;

/**
 * @brief Target Recognition Status.
 */
typedef enum {
    DJI_LIVEVIEW_OBJ_STATE_INVALID = 0,                 /*!< invalid state */
    DJI_LIVEVIEW_OBJ_STATE_TRACKED = 1,                 /*!< steady state following */
    DJI_LIVEVIEW_OBJ_STATE_VISION_BEACON_FUSIONED = 2,  /*!< Vision beacon fusion state */
    DJI_LIVEVIEW_OBJ_STATE_AUXILIARY_TRACKED = 3,       /*!< Loss of trigger source, auxiliary observation following state */
    DJI_LIVEVIEW_OBJ_STATE_NOT_CONFIDENT = 4,           /*!< unstable following state */
    DJI_LIVEVIEW_OBJ_STATE_LOST_WITH_PREDICT = 5,       /*!< Target lost, forecast status maintained */
    DJI_LIVEVIEW_OBJ_STATE_LOST = 6,                    /*!< Target Loss Status */
    DJI_LIVEVIEW_OBJ_STATE_REDETECTED = 7,              /*!< Target Loss Retrieval Status */
} E_DjiLiveViewTargetObjectState;

/**
 * @brief Target Liveview Codec Type.
 */
typedef enum {
    DJI_LIVEVIEW_CODEC_STRATEGY_ONDEMAND_I = 0,   /*!< onmandi codec */
    DJI_LIVEVIEW_CODEC_STRATEGY_GDR = 1,          /*!< gdr codec */
    DJI_LIVEVIEW_CODEC_STRATEGY_PERIOD_I = 2,     /*!< periodi codec */
    DJI_LIVEVIEW_CODEC_STRATEGY_UNKNOWN = 255,    /*!< unknown codec */
} E_DjiLiveViewCodecStrategy;

/**
 * @brief Native Stream Config Status.
 */
typedef enum {
    DJI_LIVEVIEW_HDVT_SDR_MODE_NORMAL = 0,               /*!< normal mode */
    DJI_LIVEVIEW_HDVT_SDR_MODE_SILENCE_ALL = 2,          /*!< 2.4g and 5.8g silenece all */
} E_DjiLiveViewHdvtSdrMode;

/**
 * @brief Picture frame information for target recognition results
 */
typedef struct
{
    uint16_t cx;            /*!< X-coordinate of the center of the frame, unit: 1/10000 * screen width */
    uint16_t cy;            /*!< Y-coordinate of the center of the frame, unit: 1/10000 * screen height */
    uint16_t w;             /*!< Frame width, unit: 1/10000 * screen width */
    uint16_t h;             /*!< Frame height, unit: 1/10000 * screen height */
    uint32_t distance;      /*!< Distance in mm */
} __attribute__((packed)) T_DjiLiveViewTarget2dBox;

/**
 * @brief BoundingBox information for target recognition.
 */
typedef struct {
    uint16_t id;
    uint8_t type;       /*!< type can be an enumerated value of E_DjiLiveViewTargetObjectType or a user-defined value. */
    uint8_t state;      /*!< state should be of type E_DjiLiveViewTargetObjectState enumeration */
    T_DjiLiveViewTarget2dBox box;
} __attribute__((packed)) T_DjiLiveViewBoundingBox;

/**
 * @brief List of boundingBox information.
 */
typedef struct {
    uint8_t boxCount;
    T_DjiLiveViewBoundingBox boxData[1];
} __attribute__((packed)) T_DjiLiveViewStandardMetaData;

/**
 * @brief Image information.
 */
typedef struct {
    E_DjiLiveViewPixFormate pixFmt;
    uint16_t width;
    uint16_t height;
    uint32_t frameId;
} T_DjiLiveviewImageInfo;

/**
 * @brief Liveview Bitrate and Strategy information.
 */
typedef struct{
    E_DjiLiveViewCodecStrategy strategy;	    /*!< state should be of type E_DjiLiveViewCodecStrategy enumeration */
    int16_t bitrate_kbps;                       /*!< set the target bitrate, unit: Kbps */
} T_DjiLiveviewCodecParamItem;

/**
 * @brief Liveview camera h264 stream callback.
 */
typedef void (*DjiLiveview_H264Callback)(E_DjiLiveViewCameraPosition position, const uint8_t *buf, uint32_t len);

/**
 * @brief Liveview camera image data callback.
 */
typedef void (*DjiLiveview_ImageCallback)(E_DjiLiveViewCameraPosition position, const uint8_t *buf,
                                        uint32_t len , T_DjiLiveviewImageInfo ImageInfo);

/**
 * @brief Callback for handling encoded output data
 */
typedef void (*DjiLiveview_EncoderCallback)(const uint8_t *buf, uint32_t len);

/**
 * @brief Liveview camera h264 stream callback with parsed SEI data.
 * @note
 *  - This API is not available in the current version.
 * @param position: Camera mount position.
 * @param buf: H264 frame data buffer.
 * @param len: H264 frame data length.
 * @param seiInfo: Parsed SEI info container. NULL if no SEI in this frame.
 *                 Use seiInfo->hasAi / seiInfo->hasAr to check available data.
 *                 Valid only during the callback scope, do not cache the pointer.
 */
typedef void (*DjiLiveview_H264WithSeiCallback)(E_DjiLiveViewCameraPosition position,
                                                 const uint8_t *buf, uint32_t len,
                                                 const T_DjiSeiInfo *seiInfo);

/**
 * @brief Liveview camera stream callback with parsed SEI data.
 * @note
 *  - This API is not available in the current version.
 * @note Same signature as DjiLiveview_H264WithSeiCallback, used by DjiLiveview_StartStreamWithSei.
 */
typedef void (*DjiLiveview_StreamWithSeiCallback)(E_DjiLiveViewCameraPosition position,
                                                   const uint8_t *buf, uint32_t len,
                                                   const T_DjiSeiInfo *seiInfo);
/* Exported functions --------------------------------------------------------*/
/**
 * @brief Initialize the liveview module.
 * @note Must be called after DjiCore_Init.
 * @return Execution result.
 */
T_DjiReturnCode DjiLiveview_Init(void);

/**
 * @brief Deinitialize the liveview module.
 * @return Execution result.
 */
T_DjiReturnCode DjiLiveview_Deinit(void);


/**
 * @brief Config the camera stream encoding strategy.
 * @note
 * Recommended call sequence:
 * 1. Call DjiLiveview_StartH264Stream() before calling this function.
 * 2. If state callback reports success, call DjiLiveview_SetEncodingStrategy().
 * 3. The configurable bitrate limits for different video streams and resolutions may vary.
 * 4. The actual bitrate may fluctuate by up to 20% above the configured bitrate, and the lower limit of the actual bitrate also depends on the captured image.
 * 5. This API is not available in the current version.
 * @param position: Camera position for the H264 stream output to set the encoding strategy.
 * @param source: sub-camera source for the H264 stream output.
 * @param bitrate_kbps: The target bitrate, unit: Kbps. For the wa345, the recommended bitrate range for H264 at 1440x1080 resolution is 12 Mbps to 20 Mbps.
 * @param strategy: camera codec strategy, state should be of type E_DjiLiveViewSetParamType enumeration
 * @return Execution result. If the model is not supported, `DJI_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT` will be returned.
 */
T_DjiReturnCode DjiLiveview_SetEncodingStrategy(E_DjiLiveViewCameraPosition position, E_DjiLiveViewCameraSource source,
                                                const T_DjiLiveviewCodecParamItem *codec_param);

/**
 * @brief Start the FPV or camera H264 stream from the specified position.
 * @param position: Camera position for the H264 stream output.
 * @param source: sub-camera source for the H264 stream output.
 * @param callback: Callback function in a callback thread when a new h264 frame is received
 * @note This interface will no longer be maintained in future releases.
 * @return Execution result.
 */
T_DjiReturnCode DjiLiveview_StartH264Stream(E_DjiLiveViewCameraPosition position, E_DjiLiveViewCameraSource source,
                                            DjiLiveview_H264Callback callback);

/**
 * @brief Stop the FPV or Camera H264 Stream from the specified position.
 * @param position: Camera position for the H264 stream output.
 * @param source: sub-camera source for the H264 stream output.
 * @note This interface will no longer be maintained in future releases.
 * @return Execution result.
 */
T_DjiReturnCode DjiLiveview_StopH264Stream(E_DjiLiveViewCameraPosition position, E_DjiLiveViewCameraSource source);

/**
 * @brief Start the FPV or camera stream from the specified position.
 * @param position: Camera position for the stream output.
 * @param source: sub-camera source for the stream output.
 * @param callback: Callback function in a callback thread when a new frame is received
 * @note This interface is compatible with the original DjiLiveview_StartH264Stream interface.
 * @return Execution result.
 */
T_DjiReturnCode DjiLiveview_StartStream(E_DjiLiveViewCameraPosition position, E_DjiLiveViewCameraSource source,
                                            DjiLiveview_H264Callback callback);

/**
 * @brief Stop the FPV or Camera Stream from the specified position.
 * @param position: Camera position for the stream output.
 * @param source: sub-camera source for the stream output.
 * @note This interface is compatible with the original DjiLiveview_StopH264Stream interface.
 * @return Execution result.
 */
T_DjiReturnCode DjiLiveview_StopStream(E_DjiLiveViewCameraPosition position, E_DjiLiveViewCameraSource source);

/**
 * @brief Start the camera H264 stream with AI SEI parsing from the specified position.
 * @note This API is not available in the current version.
 * @param position: Camera position for the H264 stream output.
 * @param source: sub-camera source for the H264 stream output.
 * @param callback: Callback function with AI SEI data when a new h264 frame is received.
 *                  The aiInfo parameter will be NULL when the frame has no AI SEI data.
 * @return Execution result.
 */
T_DjiReturnCode DjiLiveview_StartH264StreamWithSei(E_DjiLiveViewCameraPosition position,
                                                    E_DjiLiveViewCameraSource source,
                                                    DjiLiveview_H264WithSeiCallback callback);

/**
 * @brief Stop the camera H264 stream with SEI from the specified position.
 * @note This API is not available in the current version.
 * @param position: Camera position for the H264 stream output.
 * @param source: sub-camera source for the H264 stream output.
 * @return Execution result.
 */
T_DjiReturnCode DjiLiveview_StopH264StreamWithSei(E_DjiLiveViewCameraPosition position,
                                                   E_DjiLiveViewCameraSource source);

/**
 * @brief Start the camera stream with AI SEI parsing from the specified position.
 * @note This API is not available in the current version.
 * @param position: Camera position for the stream output.
 * @param source: sub-camera source for the stream output.
 * @param callback: Callback function with AI SEI data when a new frame is received.
 * @note This interface is compatible with the original DjiLiveview_StartH264StreamWithSei interface.
 * @return Execution result.
 */
T_DjiReturnCode DjiLiveview_StartStreamWithSei(E_DjiLiveViewCameraPosition position,
                                                E_DjiLiveViewCameraSource source,
                                                DjiLiveview_StreamWithSeiCallback callback);

/**
 * @brief Stop the camera stream with SEI from the specified position.
 * @note This API is not available in the current version.
 * @param position: Camera position for the stream output.
 * @param source: sub-camera source for the stream output.
 * @note This interface is compatible with the original DjiLiveview_StopH264StreamWithSei interface.
 * @return Execution result.
 */
T_DjiReturnCode DjiLiveview_StopStreamWithSei(E_DjiLiveViewCameraPosition position,
                                               E_DjiLiveViewCameraSource source);

/**
 * @brief Request the intraframe Frame of camera H264 stream from the specified position.
 * @param position: Camera position for the H264 stream output.
 * @param source: sub-camera source for the H264 stream output.
 * @return Execution result.
 */
T_DjiReturnCode DjiLiveview_RequestIntraframeFrameData(E_DjiLiveViewCameraPosition position,
                                                       E_DjiLiveViewCameraSource source);

/**
 * @brief Request to get the decoded image data from the specified position.
 * @param position: Camera position for the H264 stream.
 * @param source: sub-camera source for the H264 stream.
 * @param pixFmt: Requested Image Format.
 * @param callback: Callback for handling image data.
 * @return Execution result.
 * @note This interface support on DJI manifold3.
 */
T_DjiReturnCode DjiLiveview_StartImageStream(E_DjiLiveViewCameraPosition position, E_DjiLiveViewCameraSource source,
                                            E_DjiLiveViewPixFormate pixFmt, DjiLiveview_ImageCallback callback);

/**
 * @brief Stop the decoded image data from the specified position.
 * @param position: Camera position for the H264 stream.
 * @param source: sub-camera source for the H264 stream.
 * @return Execution result.
 * @note This interface support on DJI manifold3.
 */
T_DjiReturnCode DjiLiveview_StopImageStream(E_DjiLiveViewCameraPosition position, E_DjiLiveViewCameraSource source);

/**
 * @brief Registering encoder callback handlers.
 * @return Execution result.
 * @note This interface support on DJI manifold3.
 */
T_DjiReturnCode DjiLiveview_RegEncoderCallback(DjiLiveview_EncoderCallback callback);

/**
 * @brief Unregistering encoder callback handlers.
 * @return Execution result.
 */
T_DjiReturnCode DjiLiveview_UnregEncoderCallback();

/**
 * @brief Encoding image into h264 streams.
 * @param buf: raw data of image.
 * @param len: length of image data.
 * @param imageInfo: information of image.
 * @param metaData: Resulting information obtained from images for target recognition.
 * @return Execution result.
 * @note If there is no need for information related to target identification, fill in NULL for metaData.
 * @note This interface needs to be used after DjiLiveview_RegEncoderCallback.
 * @note This interface support on DJI manifold3.
 */
T_DjiReturnCode DjiLiveview_EncodeAFrameToH264(const uint8_t *buf, uint32_t len, T_DjiLiveviewImageInfo imageInfo,
                                               T_DjiLiveViewStandardMetaData *metaData);

/**
 * @brief Register user-defined target Lables
 * @param lableCount: Number of lable.
 * @param labels: lable String Array
 * @note The key-value pairs of lable and index will be recorded on the Pilot after registration
 *      and will be used to parse the metaData and display the lable when rendering the boundingbox.
 * @note This interface support on DJI manifold3.
 * @return Execution result.
 */
T_DjiReturnCode DjiLiveview_RegUserAiTargetLableList(uint8_t lableCount,const char *labels[]);

/**
 * @brief Unregister user-defined target Lables
 * @return Execution result.
 * @note This interface support on DJI manifold3.
 */
T_DjiReturnCode DjiLiveview_UnregUserAiTargetLableList();

/**
 * @brief Transmits the result information of the target recognition to the pilot for display.
 * @param metaData: Resulting information obtained from images for target recognition.
 * @note This interface support on DJI manifold3.
 * @return Execution result.
 */
T_DjiReturnCode DjiLiveview_SendAiMetaToPilot(T_DjiLiveViewStandardMetaData *metaData);

/**
 * @brief Stop the native liveview stream.
 * @note This API is not available in the current version.
 * Recommended call sequence:
 * 1. The aircraft will restore to the default open native DJI liveview transmission state after reboot.
 * @param nativeLiveviewConfig: Configuration value for native liveview stream
 *        - 2: Silent mode - Control the sky to silence the liveview transmission,
 *             and disable the DJI native liveview transmission.
 *        - 0: Normal mode - Enable the default DJI native liveview transmission.
 * @return Execution result.
 */
T_DjiReturnCode DjiLiveview_SetHdvtSdrMode(E_DjiLiveViewHdvtSdrMode nativeLiveviewConfig);

#ifdef __cplusplus
}
#endif

#endif // DJI_LIVEVIEW_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
