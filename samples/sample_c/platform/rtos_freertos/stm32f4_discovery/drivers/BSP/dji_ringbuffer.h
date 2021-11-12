/**
 ******************************************************************************
 * @file    dji_ringbuffer.h
 * @version V1.0.0
 * @date    2017/11/10
 * @brief   This is the header file for "dji_ringbuffer.c".
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
#ifndef _DJI_RING_BUFFER_H_
#define _DJI_RING_BUFFER_H_

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

//Note: not need lock for just one producer / one consumer
//need mutex to protect for multi-producer / multi-consumer

typedef struct _ringBuffer {
    uint8_t *bufferPtr;
    uint16_t bufferSize;

    uint16_t readIndex;
    uint16_t writeIndex;
} T_RingBuffer;

/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

void RingBuf_Init(T_RingBuffer *pthis, uint8_t *pBuf, uint16_t bufSize);
uint16_t RingBuf_Put(T_RingBuffer *pthis, const uint8_t *pData, uint16_t dataLen);
uint16_t RingBuf_Get(T_RingBuffer *pthis, uint8_t *pData, uint16_t dataLen);
uint16_t RingBuf_GetUnusedSize(T_RingBuffer *pthis);

/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#endif
