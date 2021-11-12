/**
 ******************************************************************************
 * @file    util_buffer.c
 * @brief   The file defines buffer related functions, including initialize, put data to buffer,
 *           get data from buffer and get unused count of bytes of buffer.
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

/* Includes ------------------------------------------------------------------*/
#include "util_buffer.h"
#include <string.h>
#include "util_misc.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief Cut buffer size to power of 2, in order to increase the convenience of get and put operating of buffer.
 * @param bufSize Original buffer size.
 * @return Buffer size after handling.
 */
static uint16_t UtilBuffer_CutBufSizeToPowOfTwo(uint16_t bufSize)
{
    uint16_t i = 0;

    while ((1 << (++i)) <= bufSize);
    return (uint16_t) (1 << (--i));
}

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Buffer initialization.
 * @param pthis Pointer to buffer structure.
 * @param pBuf Pointer to data buffer.
 * @param bufSize Size of data buffer.
 * @return None.
 */
void UtilBuffer_Init(T_UtilBuffer *pthis, uint8_t *pBuf, uint16_t bufSize)
{
    pthis->readIndex = 0;
    pthis->writeIndex = 0;
    pthis->bufferPtr = pBuf;
    pthis->bufferSize = UtilBuffer_CutBufSizeToPowOfTwo(bufSize);
}

/**
 * @brief Put a block of data into buffer.
 * @param pthis Pointer to buffer structure.
 * @param pData Pointer to data to be stored.
 * @param dataLen Length of data to be stored.
 * @return Length of data to be stored.
 */
uint16_t UtilBuffer_Put(T_UtilBuffer *pthis, const uint8_t *pData, uint16_t dataLen)
{
    uint16_t writeUpLen;

    dataLen = USER_UTIL_MIN(dataLen, (uint16_t) (pthis->bufferSize - pthis->writeIndex + pthis->readIndex));

    //fill up data
    writeUpLen = USER_UTIL_MIN(dataLen, (uint16_t) (pthis->bufferSize - (pthis->writeIndex & (pthis->bufferSize - 1))));
    memcpy(pthis->bufferPtr + (pthis->writeIndex & (pthis->bufferSize - 1)), pData, writeUpLen);

    //fill begin data
    memcpy(pthis->bufferPtr, pData + writeUpLen, dataLen - writeUpLen);

    pthis->writeIndex += dataLen;

    return dataLen;
}

/**
 * @brief Get a block of data from buffer.
* @param pthis Pointer to buffer structure.
* @param pData Pointer to data to be read.
* @param dataLen Length of data to be read.
* @return Length of data to be read.
 */
uint16_t UtilBuffer_Get(T_UtilBuffer *pthis, uint8_t *pData, uint16_t dataLen)
{
    uint16_t readUpLen;

    dataLen = USER_UTIL_MIN(dataLen, (uint16_t) (pthis->writeIndex - pthis->readIndex));

    //get up data
    readUpLen = USER_UTIL_MIN(dataLen, (uint16_t) (pthis->bufferSize - (pthis->readIndex & (pthis->bufferSize - 1))));
    memcpy(pData, pthis->bufferPtr + (pthis->readIndex & (pthis->bufferSize - 1)), readUpLen);

    //get begin data
    memcpy(pData + readUpLen, pthis->bufferPtr, dataLen - readUpLen);

    pthis->readIndex += dataLen;

    return dataLen;
}

/**
 * @brief Get unused size of buffer.
 * @param pthis Pointer to buffer structure.
 * @return Unused size of buffer.
 */
uint16_t UtilBuffer_GetUnusedSize(T_UtilBuffer *pthis)
{
    return (uint16_t) (pthis->bufferSize - pthis->writeIndex + pthis->readIndex);
}
