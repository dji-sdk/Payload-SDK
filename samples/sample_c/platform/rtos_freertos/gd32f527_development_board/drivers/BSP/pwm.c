/**
 ********************************************************************
 * @file    pwm.c
 * @version V1.0.0
 * @date    2024/05/22
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
#include "pwm.h"
#include "gd32f5xx.h"

/* Private constants ---------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/
static uint32_t period = 0;
static uint32_t duty_pluse = 0;

/* Private functions declaration ---------------------------------------------*/

/* Exported functions definition ---------------------------------------------*/
void Pwm_Init(E_PwmNum pwmNum, uint32_t freq, uint32_t duty)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    if (duty > 100)
    {
        return;
    }

    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_2);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);

    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_2);

    rcu_periph_clock_enable(RCU_TIMER1);
    rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);
    timer_deinit(TIMER1);

    /* -----------------------------------------------------------------------
    TIMER1 configuration: generate 3 PWM signals with 3 different duty cycles:
    TIMER1CLK = SystemCoreClock / 200 = 1MHz
    ----------------------------------------------------------------------- */
    period = 1000000 / freq - 1;

    /* TIMER1 configuration */
    timer_initpara.prescaler         = 199;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = period;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER1, &timer_initpara);

    /* CH1,CH2 and CH3 configuration in PWM mode */
    timer_ocintpara.ocpolarity  = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.outputstate = TIMER_CCX_ENABLE;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

    timer_channel_output_config(TIMER1, TIMER_CH_2, &timer_ocintpara);
    
    duty_pluse = (float)((period + 1) / 100) * duty;
    timer_channel_output_pulse_value_config(TIMER1, TIMER_CH_2, duty_pluse);
    timer_channel_output_mode_config(TIMER1, TIMER_CH_2, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER1, TIMER_CH_2, TIMER_OC_SHADOW_DISABLE);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER1);
    /* auto-reload preload enable */
    timer_enable(TIMER1);
}

void Pwm_SetDuty(uint32_t duty)
{
    duty_pluse = (float)((period + 1) / 100) * duty;
    timer_channel_output_pulse_value_config(TIMER1, TIMER_CH_2, duty_pluse);
    timer_channel_output_mode_config(TIMER1, TIMER_CH_2, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER1, TIMER_CH_2, TIMER_OC_SHADOW_DISABLE);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER1);
    /* auto-reload preload enable */
    timer_enable(TIMER1);
}

/* Private functions definition-----------------------------------------------*/


/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
