/*!
    \file    gd32f5xx_it.c
    \brief   interrupt service routines

    \version 2024-02-19, V1.1.0, demo for GD32F5xx
*/

/*
    Copyright (c) 2024, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#include "gd32f5xx.h"
#include "gd32f5xx_it.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stdint.h"
#include "semphr.h"
#include "queue.h"
#include "gd32f5xx_it.h"
#include "drv_usbd_int.h"

#ifdef USE_USB_FS
extern usb_core_driver cdc_acm;
#endif

extern void usb_timer_irq (void);
static void resume_mcu_clk(void);

#define MULTI_ECC_ERROR_HANDLE(s)    do{}while(1)

/*!
    \brief    this function handles NMI exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void NMI_Handler(void)
{
    if((SET == syscfg_interrupt_flag_get(SYSCFG_INT_FLAG_ECCME0)) ||
            (SET == syscfg_interrupt_flag_get(SYSCFG_INT_FLAG_ECCME1)) ||
            (SET == syscfg_interrupt_flag_get(SYSCFG_INT_FLAG_ECCME2)) ||
            (SET == syscfg_interrupt_flag_get(SYSCFG_INT_FLAG_ECCME3)) ||
            (SET == syscfg_interrupt_flag_get(SYSCFG_INT_FLAG_ECCME4)) ||
            (SET == syscfg_interrupt_flag_get(SYSCFG_INT_FLAG_ECCME5)) ||
            (SET == syscfg_interrupt_flag_get(SYSCFG_INT_FLAG_ECCME6))) {
        MULTI_ECC_ERROR_HANDLE("SRAM or FLASH multi-bits non-correction ECC error\r\n");
    } else {
        /* if NMI exception occurs, go to infinite loop */
        /* HXTAL clock monitor NMI error */
        while(1) {
        }
    }
}

/*!
    \brief      this function handles HardFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void HardFault_Handler(void)
{
    /* if Hard Fault exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles MemManage exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void MemManage_Handler(void)
{
    /* if Memory Manage exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles BusFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void BusFault_Handler(void)
{
    /* if Bus Fault exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles UsageFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void UsageFault_Handler(void)
{
    /* if Usage Fault exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles DebugMon exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DebugMon_Handler(void)
{
    /* if DebugMon exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles timer2 interrupt Handler
    \param[in]  none
    \param[out] none
    \retval     none
*/
void TIMER2_IRQHandler(void)
{
    usb_timer_irq();
}

#ifdef USE_USB_FS

/*!
    \brief      this function handles USBFS wakeup interrupt handler
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USBFS_WKUP_IRQHandler(void)
{
    if (cdc_acm.bp.low_power) {
        resume_mcu_clk();

        rcu_pll48m_clock_config(RCU_PLL48MSRC_PLLQ);
        rcu_ck48m_clock_config(RCU_CK48MSRC_PLL48M);

        rcu_periph_clock_enable(RCU_USBFS);

        usb_clock_active(&cdc_acm);
    }

    exti_interrupt_flag_clear(EXTI_18);
}

#elif defined(USE_USB_HS)

/*!
    \brief      this function handles USBHS wakeup interrupt handler
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USBHS_WKUP_IRQHandler(void)
{
    if (cdc_acm.bp.low_power) {
        resume_mcu_clk();

        #ifdef USE_EMBEDDED_PHY
            rcu_pll48m_clock_config(RCU_PLL48MSRC_PLLQ);
            rcu_ck48m_clock_config(RCU_CK48MSRC_PLL48M);
        #elif defined(USE_ULPI_PHY)
            rcu_periph_clock_enable(RCU_USBHSULPI);
        #endif

        rcu_periph_clock_enable(RCU_USBHS);

        usb_clock_active(&cdc_acm);
    }

    exti_interrupt_flag_clear(EXTI_20);
}

#endif /* USE_USBFS */

#ifdef USE_USB_FS

/*!
    \brief      this function handles USBFS IRQ Handler
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USBFS_IRQHandler(void)
{
    usbd_isr(&cdc_acm);
}

#elif defined(USE_USB_HS)

/*!
    \brief      this function handles USBHS IRQ Handler
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USBHS_IRQHandler(void)
{
    usbd_isr(&cdc_acm);
}

#endif /* USE_USBFS */

/*!
    \brief      resume mcu clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void resume_mcu_clk(void)
{
    /* enable HXTAL */
    rcu_osci_on(RCU_HXTAL);

    /* wait till HXTAL is ready */
    while(RESET == rcu_flag_get(RCU_FLAG_HXTALSTB)){
    }

    /* enable PLL */
    rcu_osci_on(RCU_PLL_CK);

    /* wait till PLL is ready */
    while(RESET == rcu_flag_get(RCU_FLAG_PLLSTB)){
    }

    /* select PLL as system clock source */
    rcu_system_clock_source_config(RCU_CKSYSSRC_PLLP);

    /* wait till PLL is used as system clock source */
    while(RCU_SCSS_PLLP != rcu_system_clock_source_get()){
    }
}
