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
} E_DjiLiveViewCameraSource;

/**
 * @brief Liveview camera h264 stream callback.
 */
typedef void (*DjiLiveview_H264Callback)(E_DjiLiveViewCameraPosition position, const uint8_t *buf, uint32_t len);

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
 * @brief Start the FPV or camera H264 stream from the specified position.
 * @param position: Camera position for the H264 stream output.
 * @param source: sub-camera source for the H264 stream output.
 * @param callback: Callback function in a callback thread when a new h264 frame is received
 * @return Execution result.
 */
T_DjiReturnCode DjiLiveview_StartH264Stream(E_DjiLiveViewCameraPosition position, E_DjiLiveViewCameraSource source,
                                            DjiLiveview_H264Callback callback);

/**
 * @brief Stop the FPV or Camera H264 Stream from the specified position.
 * @param position: Camera position for the H264 stream output.
 * @param source: sub-camera source for the H264 stream output.
 * @return Execution result.
 */
T_DjiReturnCode DjiLiveview_StopH264Stream(E_DjiLiveViewCameraPosition position, E_DjiLiveViewCameraSource source);

/**
 * @brief Request the intraframe Frame of camera H264 stream from the specified position.
 * @param position: Camera position for the H264 stream output.
 * @param source: sub-camera source for the H264 stream output.
 * @return Execution result.
 */
T_DjiReturnCode DjiLiveview_RequestIntraframeFrameData(E_DjiLiveViewCameraPosition position,
                                                       E_DjiLiveViewCameraSource source);

#ifdef __cplusplus
}
#endif

#endif // DJI_LIVEVIEW_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
