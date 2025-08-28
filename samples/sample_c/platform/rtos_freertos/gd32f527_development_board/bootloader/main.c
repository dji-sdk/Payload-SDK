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
#include <upgrade_platform_opt_gd32.h>
#include "osal.h"
#include "led.h"
#include "stdio.h"
#include "flash_if.h"

/* Exported variables --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define USER_START_TASK_STACK_SIZE          1024
#define USER_START_TASK_PRIORITY            0
#define USER_LED_TASK_STACK_SIZE            1024
#define USER_LED_TASK_PRIORITY              0

#define SRAM_START_ADDR                     ((uint32_t)0x20000000)
#define SRAM_SIZE                           ((uint32_t)0x100000)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern pFunction JumpToApplication;
extern uint32_t JumpAddress;
static T_DjiTaskHandle startTask;
static T_DjiTaskHandle ledTask;

/* Private function prototypes -----------------------------------------------*/
static void IAP_Init(void);
static void SystemClock_Config(void);
static void PsdkUser_StartTask(void const *argument);
static void PsdkUser_LedTask(void const *argument);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    /* Configure 4 bits pre-emption priority */
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);

    /* Create start task */
    xTaskCreate((TaskFunction_t) PsdkUser_StartTask, "start_task", USER_START_TASK_STACK_SIZE,
                NULL, USER_START_TASK_PRIORITY, startTask);

    xTaskCreate((TaskFunction_t) PsdkUser_LedTask, "led_task", USER_LED_TASK_STACK_SIZE,
                NULL, USER_LED_TASK_PRIORITY, ledTask);

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
    extern uint32_t __RAM_START;
    extern uint32_t __RAM_SIZE;

    /* attention : Delay for power on button state check mistake */
    Osal_TaskSleepMs(50);

    /* Initialize Key Button mounted on Gd324xG-EVAL board */
    Button_Init(BUTTON_KEY1, BUTTON_MODE_GPIO);

    /* Initialize flash option config **/
    FLASH_If_Init();

    IAP_Init();

    printf("RAM Start: 0x%08lX\r\n", SRAM_START_ADDR);
    printf("RAM Size: 0x%08lX\r\n", SRAM_SIZE);

   /* Test if Key push-button1 is pressed */
   if (Button_GetState(BUTTON_KEY1) == 1) {
       /* Execute the IAP driver in order to reprogram the Flash */
       /* Display main menu */
       Main_Menu();
   } else {
       returnCode = DjiUpgradePlatformGd32_GetUpgradeRebootState(&isUpgradeReboot, &upgradeEndInfo);
       printf("Reboot from upgrade flag is %d, upgrade end state is 0x%x.\r\n", isUpgradeReboot, upgradeEndInfo.upgradeEndState);
       Osal_TaskSleepMs(5);

       if (returnCode == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS && isUpgradeReboot == true &&
           upgradeEndInfo.upgradeEndState == DJI_UPGRADE_END_STATE_SUCCESS) {
           //replace old program
           printf("Start to replace the old program.\r\n");
           Osal_TaskSleepMs(5);

           returnCode = DjiUpgradePlatformGd32_ReplaceOldProgram();
           if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
               upgradeEndInfo.upgradeEndState = DJI_UPGRADE_END_STATE_UNKNOWN_ERROR;
               DjiUpgradePlatformGd32_SetUpgradeRebootState(&upgradeEndInfo);
           }
       }

       uint32_t spValue =  (*(__IO uint32_t *) APPLICATION_ADDRESS);
       /* Test if user code is programmed starting from address "APPLICATION_ADDRESS" */
       printf("The initial sp of app is 0x%08X\r\n", spValue);
       Osal_TaskSleepMs(5);

       if (spValue >= SRAM_START_ADDR && (spValue < SRAM_START_ADDR + SRAM_SIZE)) {
           printf("Jump to app at 0x%08X\r\n", APPLICATION_ADDRESS);
           Osal_TaskSleepMs(5);
           __disable_irq();
           __disable_fiq();
           /* Jump to user application */
           JumpAddress = *(__IO uint32_t *) (APPLICATION_ADDRESS + 4);
           JumpToApplication = (pFunction) JumpAddress;
           /* Initialize user application's Stack Pointer */
           __set_MSP(*(__IO uint32_t *) APPLICATION_ADDRESS);
           JumpToApplication();
       } else {
           printf("Invalid initial sp value\r\n");
       }
   }

    while (1) {}
}

static void PsdkUser_LedTask(void const *argument)
{
  Led_Init(LED_BLUE);

  while (1)
  {
    Osal_TaskSleepMs(200);
    Led_Trigger(LED_BLUE);
  }
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

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(UART3, (uint8_t)ch);
    while (RESET == usart_flag_get(UART3, USART_FLAG_TBE));

    return ch;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
