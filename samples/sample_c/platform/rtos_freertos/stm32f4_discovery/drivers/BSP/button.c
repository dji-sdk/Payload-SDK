/**
 ********************************************************************
 * @file    button.c
 * @version V1.0.0
 * @date    2019/01/01
 * @brief
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
 *********************************************************************
 */


/* Includes ------------------------------------------------------------------*/
#include "button.h"
#include <stm32f4xx.h>
#include <stm32f4xx_hal_rcc_ex.h>

/* Private constants ---------------------------------------------------------*/
#define KEY1_BUTTON_PIN                       GPIO_PIN_4
#define KEY1_BUTTON_GPIO_PORT                 GPIOA
#define KEY1_BUTTON_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOA_CLK_ENABLE()

/* Private types -------------------------------------------------------------*/
#define BUTTONx_GPIO_CLK_ENABLE(__INDEX__)  do{ if((__INDEX__) == 0) KEY1_BUTTON_GPIO_CLK_ENABLE(); \
                                               }while(0)

GPIO_TypeDef *BUTTON_PORT[BUTTON_NUM] = {KEY1_BUTTON_GPIO_PORT};
const uint16_t BUTTON_PIN[BUTTON_NUM] = {KEY1_BUTTON_PIN};

/* Private functions declaration ---------------------------------------------*/


/* Exported functions definition ---------------------------------------------*/
void Button_Init(Button_TypeDef Button, ButtonMode_TypeDef Button_Mode)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    /* Enable the BUTTON clock */
    BUTTONx_GPIO_CLK_ENABLE(Button);

    if (Button_Mode == BUTTON_MODE_GPIO) {
        /* Configure Button pin as input */
        GPIO_InitStruct.Pin = BUTTON_PIN[Button];
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FAST;

        HAL_GPIO_Init(BUTTON_PORT[Button], &GPIO_InitStruct);
    }
}

uint32_t Button_GetState(Button_TypeDef Button)
{
    return HAL_GPIO_ReadPin(BUTTON_PORT[Button], BUTTON_PIN[Button]);
}

/* Private functions definition-----------------------------------------------*/


/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
