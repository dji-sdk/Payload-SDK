/**
  ******************************************************************************
  * @file    IAP/IAP_Main/Src/main.c 
  * @author  MCD Application Team
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  * @statement DJI has modified some symbols' name.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include <button.h>
#include <uart.h>
#include "main.h"
#include "menu.h"
#include "FreeRTOS.h"
#include "task.h"
#include <upgrade_platform_opt_stm32.h>
#include "osal.h"

/** @addtogroup STM32F4xx_IAP_Main
  * @{
  */

/* Exported variables --------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define USER_START_TASK_STACK_SIZE          512
#define USER_START_TASK_PRIORITY            0
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern pFunction JumpToApplication;
extern uint32_t JumpAddress;
static T_DjiTaskHandle startTask;

/* Private function prototypes -----------------------------------------------*/
static void IAP_Init(void);
static void SystemClock_Config(void);
static void PsdkUser_StartTask(void const *argument);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    /* Reset of all peripherals, Initializes the Systick. */
    HAL_Init();

    /* Configure the system clock to have a system clock = 168 Mhz */
    SystemClock_Config();

    /* Create start task */
    xTaskCreate((TaskFunction_t) PsdkUser_StartTask, "start_task", 1024,
                NULL, USER_START_TASK_PRIORITY, startTask);

    /* Start scheduler */
    vTaskStartScheduler();

    /*Taken by the scheduler */
    for (;;);
}

static void PsdkUser_StartTask(void const *argument)
{
    bool isUpgradeReboot;
    T_DjiUpgradeEndInfo upgradeEndInfo;
    T_DjiReturnCode returnCode;

    /* attention : Delay for power on button state check mistake */
    Osal_TaskSleepMs(50);

    /* Initialize Key Button mounted on STM324xG-EVAL board */
    Button_Init(BUTTON_KEY1, BUTTON_MODE_GPIO);

    /* Test if Key push-button1 is pressed */
    if (Button_GetState(BUTTON_KEY1) == GPIO_PIN_RESET) {
        /* Execute the IAP driver in order to reprogram the Flash */
        IAP_Init();
        /* Display main menu */
        Main_Menu();
    } else {
        returnCode = DjiUpgradePlatformStm32_GetUpgradeRebootState(&isUpgradeReboot, &upgradeEndInfo);
        if (returnCode == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS && isUpgradeReboot == true &&
            upgradeEndInfo.upgradeEndState == DJI_UPGRADE_END_STATE_SUCCESS) {
            //replace old program
            returnCode = DjiUpgradePlatformStm32_ReplaceOldProgram();
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                upgradeEndInfo.upgradeEndState = DJI_UPGRADE_END_STATE_UNKNOWN_ERROR;
                DjiUpgradePlatformStm32_SetUpgradeRebootState(&upgradeEndInfo);
            }
        }

        uint32_t addr =  ((*(__IO uint32_t *) APPLICATION_ADDRESS) & 0x2FFE0000);
        /* Test if user code is programmed starting from address "APPLICATION_ADDRESS" */
        if (((*(__IO uint32_t *) APPLICATION_ADDRESS) & 0x2FFE0000) == 0x20000000) {
            __disable_irq();
            __disable_fiq();
            /* Jump to user application */
            JumpAddress = *(__IO uint32_t *) (APPLICATION_ADDRESS + 4);
            JumpToApplication = (pFunction) JumpAddress;
            /* Initialize user application's Stack Pointer */
            __set_MSP(*(__IO uint32_t *) APPLICATION_ADDRESS);
            JumpToApplication();
        }
    }

    while (1) {}
}

/**
  * @brief  Initialize the IAP: Configure USART.
  * @param  None
  * @retval None
  */
static void IAP_Init(void)
{
    UART_Init(PSDK_CONSOLE_UART_NUM, PSDK_CONSOLE_UART_BAUD);
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 168000000
  *            HCLK(Hz)                       = 168000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 336
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;

    /* Enable Power Control clock */
    __HAL_RCC_PWR_CLK_ENABLE();

    /* The voltage scaling allows optimizing the power consumption when the device is
       clocked below the maximum system frequency, to update the voltage scaling value
       regarding system frequency refer to product datasheet.  */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /* Enable HSE Oscillator and activate PLL with HSE as source */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = HSE_VALUE / 1000000;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
       clocks dividers */
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 |
                                   RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);

    /* STM32F405x/407x/415x/417x Revision Z devices: prefetch is supported  */
    if (HAL_GetREVID() == 0x1001) {
        /* Enable the Flash prefetch */
        __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
    }
}

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
