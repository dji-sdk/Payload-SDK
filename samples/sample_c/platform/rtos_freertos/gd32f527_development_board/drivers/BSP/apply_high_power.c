/**
 ********************************************************************
 * @file    apply_high_power.c
 * @version V2.0.0
 * @date    2019/9/20
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
#include "apply_high_power.h"
#include "gd32f5xx.h"

/* Private constants ---------------------------------------------------------*/
#define HIGH_POWER_APPLY_PORT     GPIOA
#define HIGH_POWER_APPLY_PIN      GPIO_PIN_7
#define HIGH_POWER_APPLY_CLK      RCU_GPIOA

/* Private types -------------------------------------------------------------*/

/* Private values -------------------------------------------------------------*/

/* Private functions declaration ---------------------------------------------*/

/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode DjiTest_HighPowerApplyPinInit(void)
{
    /* Enable the led clock */
    rcu_periph_clock_enable(HIGH_POWER_APPLY_CLK);

    /* Configure led GPIO port */ 
    gpio_mode_set(HIGH_POWER_APPLY_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, HIGH_POWER_APPLY_PIN);
    gpio_output_options_set(HIGH_POWER_APPLY_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, HIGH_POWER_APPLY_PIN);

    GPIO_BOP(HIGH_POWER_APPLY_PORT) = HIGH_POWER_APPLY_PIN;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiTest_WriteHighPowerApplyPin(E_DjiPowerManagementPinState pinState)
{
    switch (pinState) {
        case DJI_POWER_MANAGEMENT_PIN_STATE_RESET:
            GPIO_BC(HIGH_POWER_APPLY_PORT) = HIGH_POWER_APPLY_PIN;
            break;
        case DJI_POWER_MANAGEMENT_PIN_STATE_SET:
            GPIO_BOP(HIGH_POWER_APPLY_PORT) = HIGH_POWER_APPLY_PIN;
            break;
        default:
            USER_LOG_ERROR("pin state unknown: %d.", pinState);
            return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/* Private functions definition-----------------------------------------------*/

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
