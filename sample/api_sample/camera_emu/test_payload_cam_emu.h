/**
 ********************************************************************
 * @file    test_payload_cam_emu.h
 * @version V2.0.0
 * @date    2019/07/01
 * @brief   This is the header file for "test_payload_cam_emu.c", defining the structure and
 * (exported) function prototypes.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef TEST_PAYLOAD_CAM_EMU_H
#define TEST_PAYLOAD_CAM_EMU_H

/* Includes ------------------------------------------------------------------*/
#include "psdk_typedef.h"
#include <psdk_payload_camera.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
T_PsdkReturnCode PsdkTest_CameraInit(void);
T_PsdkReturnCode PsdkTest_CameraGetDigitalZoomFactor(psdk_f32_t *factor);
T_PsdkReturnCode PsdkTest_CameraGetOpticalZoomFactor(psdk_f32_t *factor);
T_PsdkReturnCode PsdkTest_CameraGetMode(E_PsdkCameraMode *mode);
T_PsdkReturnCode PsdkTest_CameraGetVideoStreamType(E_PsdkCameraVideoStreamType *type);
bool PsdkTest_CameraIsInited(void);

#ifdef __cplusplus
}
#endif

#endif // TEST_PAYLOAD_CAM_EMU_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
