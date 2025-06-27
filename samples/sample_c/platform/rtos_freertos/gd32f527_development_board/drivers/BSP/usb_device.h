/**
 ********************************************************************
 * @file    usb_device.h
 * @version V2.0.0
 * @date    2019/9/20
 * @brief   This is the header file for "usb_device.c", defining the structure and
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
#ifndef USB_DEVICE_H
#define USB_DEVICE_H

/* Includes ------------------------------------------------------------------*/
#include "dji_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef struct {
    uint32_t countOfLostData; /*!< Count of data lost, unit: byte. */
    uint16_t maxUsedCapacityOfBuffer; /*!< Max capacity of buffer that have been used, unit: byte. */
} T_UsbDeviceBufferState;

/* Exported functions --------------------------------------------------------*/
T_DjiReturnCode USBD_CDC_Init(void);
T_DjiReturnCode USBD_CDC_DeInit(void);
T_DjiReturnCode USBD_CDC_WriteData(const uint8_t *buf, uint32_t len, uint32_t *realLen);
T_DjiReturnCode USBD_CDC_ReadData(uint8_t *buf, uint32_t len, uint32_t *realLen);\
void USBD_CDC_Handle(void);

#ifdef __cplusplus
}
#endif

#endif // USB_DEVICE_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
