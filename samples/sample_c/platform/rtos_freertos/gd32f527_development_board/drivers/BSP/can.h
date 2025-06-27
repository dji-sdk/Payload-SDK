/**
 ******************************************************************************
 * @file    can.h
 * @version V1.0.0
 * @date    2024/05/22
 * @brief   This is the header file for "can.c".
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
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CAN_H__
#define __CAN_H__

/* Includes ------------------------------------------------------------------*/
#include "dji_platform.h"

/* Exported constants --------------------------------------------------------*/
// #define USING_CAN0
#define USING_CAN1

#define CAN_ERROR      (-1)

/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
typedef enum {
    CAN_NUM_0 = 0,
    CAN_NUM_1 = 1,
} E_CanNum;

typedef struct {
    uint32_t countOfLostData; /*!< Count of data lost, unit: byte. */
    uint16_t maxUsedCapacityOfBuffer; /*!< Max capacity of buffer that have been used, unit: byte. */
} T_CanBufferState;

/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void CAN_Init(E_CanNum canNum);
int CAN_Read(E_CanNum canNum, uint8_t *buf, uint16_t readSize);
int CAN_Write(E_CanNum canNum, const uint8_t *buf, uint16_t writeSize);

// void CAN_GetBufferState(E_UartNum uartNum, T_UartBufferState *readBufferState, T_UartBufferState *writeBufferState);

/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#endif
