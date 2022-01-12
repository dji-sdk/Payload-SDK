/**
 ******************************************************************************
  * @file    psdk_template.c
  * @version V0.0.0
  * @date    2017/8/10
 * @brief   This file to indicate PSDK coding style and comment style.
 *
 * @copyright (c) 2017-2018 DJI. All rights reserved.
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
#include "psdk_template.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

//Max PSDK data packet size
#define MAX_PACKAGE_LEN  256

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

//A buffer used to store data to be sent
static uint8_t s_SendBuffer[PSDK_TEMPLATE_MAX_BUFFER_SIZE];

/* Exported variables --------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief PSDK low level send function.
 * @param buffer Pointer to send data buffer.
 * @param len Size of data to be sent.
 * @return Size of data to be sent.
 */
static int PsdkTemplate_PrivateSend(const uint8_t *buffer, uint16_t len)
{
    /* NOTE : This function should be implemented in the user file. */
    return len;
}

/* Exported functions --------------------------------------------------------*/

/**
 * @brief PSDK protocol initialization.
 * @param None.
 * @return None.
 */
void PsdkTemplate_Init(void)
{
    /* NOTE : This function should be implemented in the user file. */
}

/**
 * @brief Send data function.
 * @param buffer : the buffer point send data
 * @param len : send data length
 * @return if >= 0, indicate the real send data length; if < 0, indicate the -ErrorCode for send failure reason.
 */
int PsdkTemplate_Send(const uint8_t *buffer, uint16_t len)
{
    uint16_t realLen;

    if (len > MAX_PACKAGE_LEN) {
        realLen = MAX_PACKAGE_LEN;
    } else {
        realLen = len;
    }

    PsdkTemplate_PrivateSend(buffer, len);
    return realLen;
}

/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
