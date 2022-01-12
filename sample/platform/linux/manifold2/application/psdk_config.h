/**
 ********************************************************************
 * @file    psdk_config.h
 * @version V2.0.0
 * @date    2019/9/11
 * @brief   This is the header file for "psdk_config.c", defining the structure and
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
#ifndef PSDK_CONFIG_H
#define PSDK_CONFIG_H

/* Includes ------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/
#define PSDK_USING_POWER_MANAGEMENT

#define PSDK_USING_CAMERA_EMU

#define PSDK_USING_CAMERA_MEDIA_EMU

#define PSDK_USING_DATA_TRANSMISSION

#define PSDK_USING_DATA_CHANNEL

#define PSDK_USING_DATA_SUBSCRIPTION

#define PSDK_USING_PAYLOAD_COLLABORATION

#define PSDK_USING_WIDGET

#define PSDK_USING_GIMBAL_EMU

#define PSDK_USING_XPORT

#define PSDK_USING_UPGRADE

/*!< Attention: This function needs to be used together with OSDK/MSDK mop sample.
 * */
//#define PSDK_USING_MOP_CHANNEL

/* Exported types ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/


#ifdef __cplusplus
}
#endif

#endif // PSDK_CONFIG_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
