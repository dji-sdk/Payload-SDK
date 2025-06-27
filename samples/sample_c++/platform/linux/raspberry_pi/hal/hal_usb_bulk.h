/**
 ********************************************************************
 * @file    hal_usb_bulk.h
 * @brief   This is the header file for "hal_usb_bulk.c", defining the structure and
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
#ifndef HAL_USB_BULK_H
#define HAL_USB_BULK_H

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifdef LIBUSB_INSTALLED

#include <libusb-1.0/libusb.h>

#endif

#include "dji_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/
#define LINUX_USB_BULK1_EP_IN_FD                "/dev/usb-ffs/bulk1/ep1"
#define LINUX_USB_BULK1_EP_OUT_FD               "/dev/usb-ffs/bulk1/ep2"

#define LINUX_USB_BULK1_INTERFACE_NUM           (0)
#define LINUX_USB_BULK1_END_POINT_IN            (0x81)
#define LINUX_USB_BULK1_END_POINT_OUT           (0x01)

#define LINUX_USB_BULK2_EP_IN_FD                "/dev/usb-ffs/bulk2/ep1"
#define LINUX_USB_BULK2_EP_OUT_FD               "/dev/usb-ffs/bulk2/ep2"

#define LINUX_USB_BULK2_INTERFACE_NUM           (1)
#define LINUX_USB_BULK2_END_POINT_IN            (0x82)
#define LINUX_USB_BULK2_END_POINT_OUT           (0x02)

#define LINUX_USB_BULK3_EP_IN_FD                "/dev/usb-ffs/bulk3/ep1"
#define LINUX_USB_BULK3_EP_OUT_FD               "/dev/usb-ffs/bulk3/ep2"

#define LINUX_USB_BULK3_INTERFACE_NUM           (2)
#define LINUX_USB_BULK3_END_POINT_IN            (0x83)
#define LINUX_USB_BULK3_END_POINT_OUT           (0x03)

#define LINUX_USB_VID                         (0x2CA3)
#define LINUX_USB_PID                         (0xF001)


/* Exported types ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
T_DjiReturnCode HalUsbBulk_Init(T_DjiHalUsbBulkInfo usbBulkInfo, T_DjiUsbBulkHandle *usbBulkHandle);
T_DjiReturnCode HalUsbBulk_DeInit(T_DjiUsbBulkHandle usbBulkHandle);
T_DjiReturnCode HalUsbBulk_WriteData(T_DjiUsbBulkHandle usbBulkHandle, const uint8_t *buf, uint32_t len,
                                     uint32_t *realLen);
T_DjiReturnCode HalUsbBulk_ReadData(T_DjiUsbBulkHandle usbBulkHandle, uint8_t *buf, uint32_t len, uint32_t *realLen);
T_DjiReturnCode HalUsbBulk_GetDeviceInfo(T_DjiHalUsbBulkDeviceInfo *deviceInfo);

#ifdef __cplusplus
}
#endif

#endif // HAL_USB_BULK_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
