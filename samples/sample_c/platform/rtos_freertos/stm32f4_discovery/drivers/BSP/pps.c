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
#include "stm32f4xx_hal.h"
#include "pps.h"
#include "osal.h"
#include "stdio.h"
#include "dji_logger.h"

/* Private constants ---------------------------------------------------------*/
#define PPS_PORT                GPIOD
#define PPS_PIN                 GPIO_PIN_2
#define PPS_IRQn                EXTI2_IRQn
#define DjiTest_PpsIrqHandler   EXTI2_IRQHandler
#define PPS_IRQ_PRIO_PRE        0x0F
#define PPS_IRQ_PRIO_SUB        0x0F

/* Private types -------------------------------------------------------------*/

/* Private values -------------------------------------------------------------*/
static uint64_t s_ppsNewestTriggerLocalTimeMs = 0;

/* Private functions declaration ---------------------------------------------*/

/* Exported functions definition ---------------------------------------------*/
void DjiTest_PpsIrqHandler(void)
{
    T_DjiReturnCode psdkStat;
    uint32_t timeMs = 0;

    /* EXTI line interrupt detected */
    if (__HAL_GPIO_EXTI_GET_IT(PPS_PIN) != RESET) {
        __HAL_GPIO_EXTI_CLEAR_IT(PPS_PIN);
        psdkStat = Osal_GetTimeMs(&timeMs);
        if (psdkStat == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
            s_ppsNewestTriggerLocalTimeMs = timeMs;
    }
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

    *localTimeUs = (uint64_t) (s_ppsNewestTriggerLocalTimeMs * 1000);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiTest_PpsSignalResponseInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable GPIOD clock */
    __HAL_RCC_GPIOD_CLK_ENABLE();

    /* Configure pin as input floating */
    GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Pin = PPS_PIN;
    HAL_GPIO_Init(PPS_PORT, &GPIO_InitStructure);

    /* Enable and set EXTI Line Interrupt to the lowest priority */
    HAL_NVIC_SetPriority(PPS_IRQn, PPS_IRQ_PRIO_PRE, PPS_IRQ_PRIO_SUB);
    HAL_NVIC_EnableIRQ(PPS_IRQn);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/* Private functions definition-----------------------------------------------*/

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
