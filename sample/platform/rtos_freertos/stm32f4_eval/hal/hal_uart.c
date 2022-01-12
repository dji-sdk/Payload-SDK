/**
 ********************************************************************
 * @file    psdk_hal.c
 * @version V2.0.0
 * @date    2019/07/01
 * @brief
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

/* Includes ------------------------------------------------------------------*/
#include "psdk_platform.h"
#include "uart.h"

/* Private constants ---------------------------------------------------------*/
#define COMMUNICATION_UART_NUM          UART_NUM_3
#define COMMUNICATION_UART_BAUDRATE     460800

/* Private types -------------------------------------------------------------*/


/* Private functions declaration ---------------------------------------------*/


/* Exported functions definition ---------------------------------------------*/
T_PsdkReturnCode Hal_UartInit(void)
{
    UART_Init(COMMUNICATION_UART_NUM, COMMUNICATION_UART_BAUDRATE);

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_PsdkReturnCode Hal_UartSendData(const uint8_t *buf, uint16_t len)
{
    UART_Write(COMMUNICATION_UART_NUM, buf, len);

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_PsdkReturnCode Hal_UartReadData(uint8_t *buf, uint16_t len, uint16_t *realLen)
{
    *realLen = UART_Read(COMMUNICATION_UART_NUM, buf, len);

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/* Private functions definition-----------------------------------------------*/

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
