/**
 ********************************************************************
 * @file    hal_network.h
 * @brief   This is the header file for "hal_network.c", defining the structure and
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
#ifndef HAL_NETWORK_H
#define HAL_NETWORK_H

/* Includes ------------------------------------------------------------------*/
#include "dji_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/** @attention  User can config network card name here, if your device is not MF2C/G, please comment below and add your
 * NIC name micro define as #define 'LINUX_NETWORK_DEV   "your NIC name"'.
 */
#ifdef PLATFORM_ARCH_x86_64
#define LINUX_NETWORK_DEV           "enxf8e43b7bbc2c"
#else
#define LINUX_NETWORK_DEV           "l4tbr0"
#endif
/**
 * @attention
 */

#ifdef PLATFORM_ARCH_x86_64
#define USB_NET_ADAPTER_VID                   (0x0B95)
#define USB_NET_ADAPTER_PID                   (0x1790)
#else
#define USB_NET_ADAPTER_VID                   (0x0955)
#define USB_NET_ADAPTER_PID                   (0x7020)
#endif

#define LINUX_CMD_STR_MAX_SIZE      (128)

/* Exported types ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
T_DjiReturnCode HalNetWork_Init(const char *ipAddr, const char *netMask, T_DjiNetworkHandle *halObj);
T_DjiReturnCode HalNetWork_DeInit(T_DjiNetworkHandle halObj);
T_DjiReturnCode HalNetWork_GetDeviceInfo(T_DjiHalNetworkDeviceInfo *deviceInfo);

#ifdef __cplusplus
}
#endif

#endif // HAL_NETWORK_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
