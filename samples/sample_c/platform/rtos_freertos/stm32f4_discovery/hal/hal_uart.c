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
#include "uart.h"
#include "dji_platform.h"

#if USE_USB_HOST_UART

#include "usbh_cdc.h"

#endif

/* Private constants ---------------------------------------------------------*/
#define COMMUNICATION_UART_NUM          UART_NUM_3

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

    uartHandleStruct = pvPortMalloc(sizeof(T_UartHandleStruct));
    if (uartHandleStruct == NULL) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_MEMORY_ALLOC_FAILED;
    }

    if (uartNum == DJI_HAL_UART_NUM_0) {
        UART_Init(COMMUNICATION_UART_NUM, baudRate);
        uartHandleStruct->uartNum = USER_UART_NUM0;
    } else if (uartNum == DJI_HAL_UART_NUM_1) {
        uartHandleStruct->uartNum = USER_UART_NUM1;
    }

    *uartHandle = uartHandleStruct;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode HalUart_DeInit(T_DjiUartHandle uartHandle)
{
    vPortFree(uartHandle);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode HalUart_WriteData(T_DjiUartHandle uartHandle, const uint8_t *buf, uint32_t len, uint32_t *realLen)
{
    T_UartHandleStruct *uartHandleStruct = (T_UartHandleStruct *) uartHandle;
    int32_t ret;

    if (uartHandleStruct->uartNum == USER_UART_NUM0) {
        UART_Write(COMMUNICATION_UART_NUM, buf, len);
    } else if (uartHandleStruct->uartNum == USER_UART_NUM1) {
        USBH_CDC_WriteData(buf, len, realLen);
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode HalUart_ReadData(T_DjiUartHandle uartHandle, uint8_t *buf, uint32_t len, uint32_t *realLen)
{
    T_UartHandleStruct *uartHandleStruct = (T_UartHandleStruct *) uartHandle;
    int32_t ret;

    if (uartHandleStruct->uartNum == USER_UART_NUM0) {
        *realLen = UART_Read(COMMUNICATION_UART_NUM, buf, len);
    } else if (uartHandleStruct->uartNum == USER_UART_NUM1) {
        USBH_CDC_ReadData(buf, len, realLen);
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode HalUart_GetStatus(E_DjiHalUartNum uartNum, T_DjiUartStatus *status)
{
    if (uartNum == DJI_HAL_UART_NUM_0) {
        status->isConnect = true;
    } else if (uartNum == DJI_HAL_UART_NUM_1) {
        status->isConnect = true;
    } else {
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/* Private functions definition-----------------------------------------------*/

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
