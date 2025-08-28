/**
 ********************************************************************
 * @file    psdk_hal.c
 * @version V2.0.0
 * @date    2019/07/01
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
#include "hal_uart.h"
#include "dji_platform.h"
#include "usb_device.h"
#include "uart.h"
#include "cdc_acm_core.h"

/* Private constants ---------------------------------------------------------*/
#define COMMUNICATION_UART_NUM          UART_NUM_1
#define USE_NATIVE_UART_ON_EPORT_V2     (0)

#define USB_UART_FT232_VID              (0x0403)
#define USB_UART_FT232_PID              (0x6001)

/* Private types -------------------------------------------------------------*/
typedef enum {
    USER_UART_NUM0 = 0,
    USER_UART_NUM1 = 1,
} T_UserUartNum;

typedef struct {
    T_UserUartNum uartNum;
} T_UartHandleStruct;

/* Private functions declaration ---------------------------------------------*/

/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode HalUart_Init(E_DjiHalUartNum uartNum, uint32_t baudRate, T_DjiUartHandle *uartHandle)
{
    T_UartHandleStruct *uartHandleStruct;

    uartHandleStruct = DjiPlatform_GetOsalHandler()->Malloc(sizeof(T_UartHandleStruct));
    if (uartHandleStruct == NULL) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_MEMORY_ALLOC_FAILED;
    }

    if (uartNum == DJI_HAL_UART_NUM_0) {
        uartHandleStruct->uartNum = USER_UART_NUM0;
#if USE_NATIVE_UART_ON_EPORT_V2
        UART_Init(COMMUNICATION_UART_NUM, baudRate);
#endif
    } else if (uartNum == DJI_HAL_UART_NUM_1) {
        uartHandleStruct->uartNum = USER_UART_NUM1;
    }

    *uartHandle = uartHandleStruct;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode HalUart_DeInit(T_DjiUartHandle uartHandle)
{
    DjiPlatform_GetOsalHandler()->Free(uartHandle);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode HalUart_WriteData(T_DjiUartHandle uartHandle, const uint8_t *buf, uint32_t len, uint32_t *realLen)
{
    T_UartHandleStruct *uartHandleStruct = (T_UartHandleStruct *) uartHandle;

    if (uartHandleStruct->uartNum == USER_UART_NUM0) {
#if USE_NATIVE_UART_ON_EPORT_V2
        *realLen = UART_Write(COMMUNICATION_UART_NUM, buf, len);
#else
        USBD_CDC_WriteData(buf, len, realLen);
#endif
    } else if (uartHandleStruct->uartNum == USER_UART_NUM1) {
        //This platform cannot be used on M300 OSDK Port, so no need adapter usb-host cdc drivers.
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode HalUart_ReadData(T_DjiUartHandle uartHandle, uint8_t *buf, uint32_t len, uint32_t *realLen)
{
    T_UartHandleStruct *uartHandleStruct = (T_UartHandleStruct *) uartHandle;

    if (uartHandleStruct->uartNum == USER_UART_NUM0) {
#if USE_NATIVE_UART_ON_EPORT_V2
        *realLen = UART_Read(COMMUNICATION_UART_NUM, buf, len);
#else
        USBD_CDC_ReadData(buf, len, realLen);
#endif
    } else if (uartHandleStruct->uartNum == USER_UART_NUM1) {
        //This platform cannot be used on M300 OSDK Port, so no need adapter usb-host cdc drivers.
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode HalUart_GetStatus(E_DjiHalUartNum uartNum, T_DjiUartStatus *status)
{
    if (uartNum == DJI_HAL_UART_NUM_0) {
        status->isConnect = true;
    } else if (uartNum == DJI_HAL_UART_NUM_1) {
        status->isConnect = false;
    } else {
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode HalUart_GetDeviceInfo(T_DjiHalUartDeviceInfo *deviceInfo)
{

    if (deviceInfo == NULL) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

#if USE_NATIVE_UART_ON_EPORT_V2
    deviceInfo->vid = USB_UART_FT232_VID;
    deviceInfo->pid = USB_UART_FT232_PID;
#else
    deviceInfo->vid = ((usb_desc_dev *)(cdc_desc.dev_desc))->idVendor;
    deviceInfo->pid = ((usb_desc_dev *)(cdc_desc.dev_desc))->idProduct;
#endif

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/* Private functions definition-----------------------------------------------*/

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
