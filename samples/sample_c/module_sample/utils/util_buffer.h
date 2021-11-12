/**
 ******************************************************************************
 * @file    util_buffer.h
 * @brief   This is the header file for "util_buffer.c".
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
#ifndef _DJI_UTIL_BUFFER_H_
#define _DJI_UTIL_BUFFER_H_

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

//Note: not need lock for just one producer / one consumer
//need mutex to protect for multi-producer / multi-consumer
typedef struct {
    uint8_t *bufferPtr;
    uint16_t bufferSize;
    uint16_t readIndex;
    uint16_t writeIndex;
} T_UtilBuffer;

/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

void UtilBuffer_Init(T_UtilBuffer *pthis, uint8_t *pBuf, uint16_t bufSize);
uint16_t UtilBuffer_Put(T_UtilBuffer *pthis, const uint8_t *pData, uint16_t dataLen);
uint16_t UtilBuffer_Get(T_UtilBuffer *pthis, uint8_t *pData, uint16_t dataLen);
uint16_t UtilBuffer_GetUnusedSize(T_UtilBuffer *pthis);

/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#endif
