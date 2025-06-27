/**
 ********************************************************************
 * @file    pps.c
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
//#include "Gd32f4xx_hal.h"
#include "pps.h"
#include "osal.h"
#include "stdio.h"
#include "dji_logger.h"
#include "gd32f5xx.h"

/* Private constants ---------------------------------------------------------*/
#define PPS_IRQn                EXTI5_9_IRQn
#define DjiTest_PpsIrqHandler  	EXTI5_9_IRQHandler
#define PPS_IRQ_PRIO_PRE        0x0F
#define PPS_IRQ_PRIO_SUB        0x0F

#define PPS_PORT                GPIOB
#define PPS_PIN                 GPIO_PIN_9
#define PPS_CLK                 RCU_GPIOB
#define PPS_EXTI_LINE           EXTI_9
#define PPS_EXTI_PORT_SOURCE    EXTI_SOURCE_GPIOB
#define PPS_EXTI_PIN_SOURCE     EXTI_SOURCE_PIN9

#define VSYNC_PORT              GPIOE
#define VSYNC_PIN               GPIO_PIN_5
#define VSYNC_CLK               RCU_GPIOE
#define VSYNC_EXTI_LINE         EXTI_5
#define VSYNC_EXTI_PORT_SOURCE  EXTI_SOURCE_GPIOE
#define VSYNC_EXTI_PIN_SOURCE   EXTI_SOURCE_PIN5

/* Private types -------------------------------------------------------------*/

/* Private values -------------------------------------------------------------*/
static uint32_t s_ppsNewestTriggerLocalTimeMs = 0;
static uint32_t s_ppsTriggerTimeMsDiff = 0;
static uint32_t s_vsyncNewestTriggerLocalTimeMs = 0;
static uint32_t s_vsyncTriggerTimeMsDiff = 0;

/* Private functions declaration ---------------------------------------------*/

/* Exported functions definition ---------------------------------------------*/
void DjiTest_PpsIrqHandler(void)
{
    T_DjiReturnCode psdkStat;
    uint32_t timeMs = 0;

    /* EXTI line interrupt detected */
    if(RESET != exti_interrupt_flag_get(PPS_EXTI_LINE)) {
        // USER_LOG_INFO("Pps signal triggered.");

        psdkStat = Osal_GetTimeMs(&timeMs);
        if (psdkStat == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
           s_ppsTriggerTimeMsDiff = timeMs - s_ppsNewestTriggerLocalTimeMs;
           s_ppsNewestTriggerLocalTimeMs = timeMs;
        }
        exti_interrupt_flag_clear(PPS_EXTI_LINE);
    }

     /* EXTI line interrupt detected */
    if(RESET != exti_interrupt_flag_get(VSYNC_EXTI_LINE)) {
        // USER_LOG_INFO("Vsync signal triggered.");

        psdkStat = Osal_GetTimeMs(&timeMs);
        if (psdkStat == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
           s_vsyncTriggerTimeMsDiff = timeMs - s_vsyncNewestTriggerLocalTimeMs;
           s_vsyncNewestTriggerLocalTimeMs = timeMs;
        }
        exti_interrupt_flag_clear(VSYNC_EXTI_LINE);
    }
}

uint32_t DjiTest_GetPpsTriggerTimeMsDiff(void)
{
    return s_ppsTriggerTimeMsDiff;
}

uint32_t DjiTest_GetVsyncTriggerTimeMsDiff(void)
{
    return s_vsyncTriggerTimeMsDiff;
}

T_DjiReturnCode DjiTest_GetNewestPpsTriggerLocalTimeUs(uint64_t *localTimeUs)
{
    if (localTimeUs == NULL) {
        USER_LOG_ERROR("input pointer is null.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    if (s_ppsNewestTriggerLocalTimeMs == 0) {
        USER_LOG_WARN("pps have not been triggered.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_BUSY;
    }

    *localTimeUs = s_ppsNewestTriggerLocalTimeMs * 1000;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiTest_PpsSignalResponseInit(void)
{
    /* Enable the pps clock */
    rcu_periph_clock_enable(PPS_CLK);
    rcu_periph_clock_enable(RCU_SYSCFG);

    /* Configure pps pin as input */
    gpio_mode_set(PPS_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, PPS_PIN);

    /* Enable and set pps EXTI interrupt to the lowest priority */
    nvic_irq_enable(PPS_IRQn, PPS_IRQ_PRIO_PRE, PPS_IRQ_PRIO_SUB);

    /* Connect pps EXTI line to key GPIO pin */
    syscfg_exti_line_config(PPS_EXTI_PORT_SOURCE, PPS_EXTI_PIN_SOURCE);

    /* Configure pps EXTI line */
    exti_init(PPS_EXTI_LINE, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    exti_interrupt_flag_clear(PPS_EXTI_LINE);

    /* Enable the pps clock */
    rcu_periph_clock_enable(VSYNC_CLK);
    /* Configure pps pin as input */
    gpio_mode_set(VSYNC_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, VSYNC_PIN);
    /* Connect pps EXTI line to key GPIO pin */
    syscfg_exti_line_config(VSYNC_EXTI_PORT_SOURCE, VSYNC_EXTI_PIN_SOURCE);
    /* Configure pps EXTI line */
    exti_init(VSYNC_EXTI_LINE, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    exti_interrupt_flag_clear(VSYNC_EXTI_LINE);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}
/* Private functions definition-----------------------------------------------*/

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
