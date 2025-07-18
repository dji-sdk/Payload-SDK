/**
 ********************************************************************
 * @file    hal.h
 * @version V2.0.0
 * @date    2019/8/30
 * @brief   This is the header file for "hal.c", defining the structure and
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
#ifndef HAL_H
#define HAL_H

/* Includes ------------------------------------------------------------------*/
//#include "dji_platform.h"
#include "stdint.h"
#include "dji_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
T_DjiReturnCode HalUart_Init(E_DjiHalUartNum uartNum, uint32_t baudRate, T_DjiUartHandle *uartHandle);
T_DjiReturnCode HalUart_DeInit(T_DjiUartHandle uartHandle);
T_DjiReturnCode HalUart_WriteData(T_DjiUartHandle uartHandle, const uint8_t *buf, uint32_t len, uint32_t *realLen);
T_DjiReturnCode HalUart_ReadData(T_DjiUartHandle uartHandle, uint8_t *buf, uint32_t len, uint32_t *realLen);
T_DjiReturnCode HalUart_GetStatus(E_DjiHalUartNum uartNum, T_DjiUartStatus *status);
T_DjiReturnCode HalUart_GetDeviceInfo(T_DjiHalUartDeviceInfo *deviceInfo);

#ifdef __cplusplus
}
#endif

#endif // HAL_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
