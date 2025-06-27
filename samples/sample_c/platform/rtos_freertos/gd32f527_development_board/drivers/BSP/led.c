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
#include "gd32f5xx.h"

/* Private constants ---------------------------------------------------------*/
#define LED_RED_GPIO_PIN                      GPIO_PIN_7
#define LED_RED_GPIO_PORT                     GPIOC
#define LED_RED_GPIO_CLK                      RCU_GPIOC

#define LED_GREEN_GPIO_PIN                    GPIO_PIN_6
#define LED_GREEN_GPIO_PORT                   GPIOC
#define LED_GREEN_GPIO_CLK                    RCU_GPIOC

#define LED_BLUE_GPIO_PIN                     GPIO_PIN_8
#define LED_BLUE_GPIO_PORT                    GPIOC
#define LED_BLUE_GPIO_CLK                     RCU_GPIOC

/* Private types -------------------------------------------------------------*/

/* Private values -------------------------------------------------------------*/
static uint32_t GPIO_PORT[LED_NUM] = {LED_GREEN_GPIO_PORT, LED_GREEN_GPIO_PORT, LED_GREEN_GPIO_PORT};
static uint32_t GPIO_PIN[LED_NUM] = {LED_RED_GPIO_PIN, LED_GREEN_GPIO_PIN, LED_BLUE_GPIO_PIN};
static rcu_periph_enum GPIO_CLK[LED_NUM] = {LED_RED_GPIO_CLK, LED_GREEN_GPIO_CLK, LED_BLUE_GPIO_CLK};

/* Private functions declaration ---------------------------------------------*/

/* Exported functions definition ---------------------------------------------*/
void Led_Init(E_LedNum ledNum)
{
    /* Enable the led clock */
    rcu_periph_clock_enable(GPIO_CLK[ledNum]);

    /* Configure led GPIO port */ 
    gpio_mode_set(GPIO_PORT[ledNum], GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,GPIO_PIN[ledNum]);
    gpio_output_options_set(GPIO_PORT[ledNum], GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN[ledNum]);

    GPIO_BC(GPIO_PORT[ledNum]) = GPIO_PIN[ledNum];
}

void Led_On(E_LedNum ledNum)
{
    GPIO_BOP(GPIO_PORT[ledNum]) = GPIO_PIN[ledNum];
}

void Led_Off(E_LedNum ledNum)
{
    GPIO_BC(GPIO_PORT[ledNum]) = GPIO_PIN[ledNum];
}

void Led_Trigger(E_LedNum ledNum)
{
    GPIO_TG(GPIO_PORT[ledNum]) = GPIO_PIN[ledNum];
}

/* Private functions definition-----------------------------------------------*/

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
