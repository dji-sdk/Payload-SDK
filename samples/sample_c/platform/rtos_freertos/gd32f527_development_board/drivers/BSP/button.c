/**
 ********************************************************************
 * @file    button.c
 * @version V1.0.0
 * @date    2019/01/01
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
#include "button.h"
#include "gd32f5xx.h"

/* Private constants ---------------------------------------------------------*/
#define KEY1_BUTTON_PIN                       GPIO_PIN_2
#define KEY1_BUTTON_GPIO_PORT                 GPIOE
#define KEY1_BUTTON_GPIO_CLK                  RCU_GPIOE

#define FORCE_LOADER_PORT                     GPIOE
#define FORCE_LOADER_PIN                      GPIO_PIN_3
#define FORCE_LOADER_CLK                      RCU_GPIOE

/* Private types -------------------------------------------------------------*/
static uint32_t KEY_PORT[BUTTON_NUM] = {KEY1_BUTTON_GPIO_PORT};
static uint32_t KEY_PIN[BUTTON_NUM] = {KEY1_BUTTON_PIN};
static rcu_periph_enum KEY_CLK[BUTTON_NUM] = {KEY1_BUTTON_GPIO_CLK};

/* Private functions declaration ---------------------------------------------*/
void ForceLoader_PinInit(void)
{
    /* Enable the led clock */
    rcu_periph_clock_enable(FORCE_LOADER_CLK);

    /* Configure led GPIO port */ 
    gpio_mode_set(FORCE_LOADER_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, FORCE_LOADER_PIN);
    gpio_output_options_set(FORCE_LOADER_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, FORCE_LOADER_PIN);

    GPIO_BOP(FORCE_LOADER_PORT) = FORCE_LOADER_PIN;
}

/* Exported functions definition ---------------------------------------------*/
void Button_Init(Button_TypeDef Button, ButtonMode_TypeDef Button_Mode)
{
    /* Configure force loader output pin */
    ForceLoader_PinInit();

    /* Enable the key clock */
    rcu_periph_clock_enable(KEY_CLK[Button]);
    rcu_periph_clock_enable(RCU_SYSCFG);

    /* Configure force loader input pin */
    gpio_mode_set(KEY_PORT[Button], GPIO_MODE_INPUT, GPIO_PUPD_NONE,KEY_PIN[Button]);
}

uint32_t Button_GetState(Button_TypeDef Button)
{
    return gpio_input_bit_get(KEY_PORT[Button], KEY_PIN[Button]);
}

/* Private functions definition-----------------------------------------------*/


/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
