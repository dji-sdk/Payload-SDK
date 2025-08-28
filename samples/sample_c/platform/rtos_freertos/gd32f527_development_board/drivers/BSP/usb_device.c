/**
 ********************************************************************
 * @file    usb_device.c
 * @version V2.0.0
 * @date    2019/9/20
 * @brief
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

/* Includes ------------------------------------------------------------------*/
#include "usb_device.h"
#include "osal.h"
#include "stdio.h"
#include "dji_logger.h"
#include "gd32f5xx.h"
#include "dji_ringbuffer.h"
#include "FreeRTOS.h"
#include "task.h"
#include "drv_usb_hw.h"
#include "cdc_acm_core.h"

/* Private constants ---------------------------------------------------------*/
#define USB_DEVICE_READ_BUF_SIZE     4096
#define USB_DEVICE_WRITE_BUF_SIZE    4096
#define USB_DEVICE_READ_DATA_SIZE    1024

/* Private types -------------------------------------------------------------*/

/* Private values -------------------------------------------------------------*/
static T_RingBuffer s_usbDeviceReadRingBuffer;
static T_RingBuffer s_usbDeviceWriteRingBuffer;
static uint8_t s_usbDeviceReadBuf[USB_DEVICE_READ_BUF_SIZE];
static uint8_t s_usbDeviceWriteBuf[USB_DEVICE_WRITE_BUF_SIZE];
static T_DjiMutexHandle s_usbDeviceMutex;
usb_core_driver cdc_acm;

/* Private functions declaration ---------------------------------------------*/

/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode USBD_CDC_Init(void)
{
    RingBuf_Init(&s_usbDeviceReadRingBuffer, s_usbDeviceReadBuf, USB_DEVICE_READ_BUF_SIZE);
    RingBuf_Init(&s_usbDeviceWriteRingBuffer, s_usbDeviceWriteBuf, USB_DEVICE_WRITE_BUF_SIZE);

    // USB CDC Init
    usb_gpio_config();
    usb_rcu_config();
    usb_timer_init();

    usbd_init(&cdc_acm,
#ifdef USE_USB_FS
              USB_CORE_ENUM_FS,
#elif defined(USE_USB_HS)
              USB_CORE_ENUM_HS,
#endif /* USE_USB_FS */
              &cdc_desc,
              &cdc_class);

    usb_intr_config();

    Osal_MutexCreate(&s_usbDeviceMutex);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode USBD_CDC_DeInit(void)
{
    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode USBD_CDC_ReadData(uint8_t *buf, uint32_t len, uint32_t *realLen)
{
    Osal_MutexLock(s_usbDeviceMutex);
    *realLen = RingBuf_Get(&s_usbDeviceReadRingBuffer, buf, len);
    Osal_MutexUnlock(s_usbDeviceMutex);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode USBD_CDC_WriteData(const uint8_t *buf, uint32_t len, uint32_t *realLen)
{
    uint32_t blockSize = USB_CDC_DATA_PACKET_SIZE;
    uint32_t blockNum = len / blockSize;
    uint32_t blockRemain = len % blockSize;
    uint32_t sentLen = 0;
    *realLen = 0;

    for (uint32_t i = 0; i < blockNum; i++) {
        cdc_acm_data_send(&cdc_acm, &buf[i * blockSize], blockSize, &sentLen);
        *realLen += sentLen;
    }

    if (blockRemain > 0) {
        cdc_acm_data_send(&cdc_acm, &buf[blockNum * blockSize], blockRemain, &sentLen);
        *realLen += sentLen;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

void USBD_CDC_Handle(void)
{
    uint16_t realCountPutBuffer = 0;
    uint32_t readRealLen = 0;
    uint8_t readBuf[USB_DEVICE_READ_DATA_SIZE] = {0};

    if (USBD_CONFIGURED != cdc_acm.dev.cur_status) {
        return;
    }

    cdc_acm_data_receive(&cdc_acm, readBuf, USB_DEVICE_READ_DATA_SIZE, &readRealLen);
    if (readRealLen > 0) {
        Osal_MutexLock(s_usbDeviceMutex);
        realCountPutBuffer = RingBuf_Put(&s_usbDeviceReadRingBuffer, readBuf, readRealLen);

        Osal_MutexUnlock(s_usbDeviceMutex);
        if (realCountPutBuffer < readRealLen) {
            USER_LOG_ERROR("failed to put data, %u/%u", realCountPutBuffer, readRealLen);
        }
        USER_LOG_DEBUG("Rececive cdc data finished, dataLen: %d", readRealLen);
    }

    return;
}
/* Private functions definition-----------------------------------------------*/

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
