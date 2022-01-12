/**
 ********************************************************************
 * @file    uart_enable.c
 * @version V2.0.0
 * @date    2019/9/23
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
#include "uart_enable.h"
#include "stm32f4xx_hal.h"

/* Private constants ---------------------------------------------------------*/
#define UART_SWITCH_PORT        GPIOE
#define UART_SWITCH_PIN         GPIO_PIN_7

/* Private types -------------------------------------------------------------*/


/* Private functions declaration ---------------------------------------------*/


/* Private variables ---------------------------------------------------------*/


/* Exported functions definition ---------------------------------------------*/
/**
 * @brief UART enable.
 * @param None.
 * @return None.
 */
void Uart_SwitchOn(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable GPIOE clock */
    __HAL_RCC_GPIOE_CLK_ENABLE();

    /* Configure pin as input floating */
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Pin = UART_SWITCH_PIN;
    HAL_GPIO_Init(UART_SWITCH_PORT, &GPIO_InitStructure);

    // switch on UART
    HAL_GPIO_WritePin(UART_SWITCH_PORT, UART_SWITCH_PIN, GPIO_PIN_SET);
}

/* Private functions definition-----------------------------------------------*/


/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
