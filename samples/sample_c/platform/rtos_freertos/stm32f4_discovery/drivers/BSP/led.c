/**
 ********************************************************************
 * @file    led.c
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
#include "led.h"

/* Private constants ---------------------------------------------------------*/
#define LED1_GPIO_PIN                      GPIO_PIN_12
#define LED1_GPIO_PORT                     GPIOD
#define LED1_GPIO_CLK_ENABLE()             __HAL_RCC_GPIOD_CLK_ENABLE()

#define LED2_GPIO_PIN                      GPIO_PIN_13
#define LED2_GPIO_PORT                     GPIOD
#define LED2_GPIO_CLK_ENABLE()             __HAL_RCC_GPIOD_CLK_ENABLE()

#define LED3_GPIO_PIN                      GPIO_PIN_14
#define LED3_GPIO_PORT                     GPIOD
#define LED3_GPIO_CLK_ENABLE()             __HAL_RCC_GPIOD_CLK_ENABLE()

#define LED4_GPIO_PIN                      GPIO_PIN_15
#define LED4_GPIO_PORT                     GPIOD
#define LED4_GPIO_CLK_ENABLE()             __HAL_RCC_GPIOD_CLK_ENABLE()

/* Private types -------------------------------------------------------------*/

/* Private values -------------------------------------------------------------*/
GPIO_TypeDef *s_ledPort[LED_NUM] = {LED1_GPIO_PORT, LED2_GPIO_PORT, LED3_GPIO_PORT, LED4_GPIO_PORT};
const uint16_t s_ledPin[LED_NUM] = {LED1_GPIO_PIN, LED2_GPIO_PIN, LED3_GPIO_PIN, LED4_GPIO_PIN};

/* Private functions declaration ---------------------------------------------*/

/* Exported functions definition ---------------------------------------------*/
void Led_Init(E_LedNum ledNum)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    /* Enable the BUTTON clock */
    LED1_GPIO_CLK_ENABLE();
    LED2_GPIO_CLK_ENABLE();
    LED3_GPIO_CLK_ENABLE();
    LED4_GPIO_CLK_ENABLE();

    /* Configure Button pin as input */
    GPIO_InitStruct.Pin = s_ledPin[ledNum];
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FAST;

    HAL_GPIO_Init(s_ledPort[ledNum], &GPIO_InitStruct);
}

void Led_On(E_LedNum ledNum)
{
    return HAL_GPIO_WritePin(s_ledPort[ledNum], s_ledPin[ledNum], 1);
}

void Led_Off(E_LedNum ledNum)
{
    return HAL_GPIO_WritePin(s_ledPort[ledNum], s_ledPin[ledNum], 0);
}

void Led_Trigger(E_LedNum ledNum)
{
    return HAL_GPIO_TogglePin(s_ledPort[ledNum], s_ledPin[ledNum]);
}

/* Private functions definition-----------------------------------------------*/

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
