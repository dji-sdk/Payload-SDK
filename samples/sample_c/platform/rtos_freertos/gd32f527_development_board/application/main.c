/**
 ********************************************************************
 * @file    main.c
 * @version V2.0.0
 * @date    2019/8/23
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
#include "gd32f5xx.h"
#include "stdio.h"
#include "application.h"
#include "led.h"
#include "button.h"
#include "uart.h"
#include "pwm.h"
#include "pps.h"
#include "apply_high_power.h"
#include "gd32f5xx.h"
#include "stdint.h"
#include "FreeRTOS.h"
#include "task.h"
#include "usb_device.h"
#include "osal.h"
#include "flash_if.h"

/* Private constants ---------------------------------------------------------*/
#define BSP_INIT_TASK_STACK_SIZE                512
#define BSP_INIT_TASK_PRIORITY                  0
#define USER_START_TASK_STACK_SIZE              2048
#define USER_START_TASK_PRIORITY                0
#define USER_RUN_INDICATE_TASK_STACK_SIZE       256
#define USER_RUN_INDICATE_TASK_PRIORITY         0

/* Private types -------------------------------------------------------------*/

/* Private values -------------------------------------------------------------*/
static TaskHandle_t bspInitTask;
static TaskHandle_t startTask;

/* Private functions declaration ---------------------------------------------*/
static void DjiUser_BspInitTask(void const *argument);

/* Exported functions definition ---------------------------------------------*/

int main(void)
{
    /* configure 4 bits pre-emption priority */
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);

    /* init task */
    xTaskCreate((TaskFunction_t)DjiUser_BspInitTask, "bsp_init_task", BSP_INIT_TASK_STACK_SIZE,
                NULL, BSP_INIT_TASK_PRIORITY, (TaskHandle_t *)bspInitTask);

    /* Create start task */
    xTaskCreate((TaskFunction_t)DjiUser_StartTask, "start_task", USER_START_TASK_STACK_SIZE,
                NULL, USER_START_TASK_PRIORITY, (TaskHandle_t *)startTask);

    /* start scheduler */
    vTaskStartScheduler();

    /*Taken by the scheduler */
    for (;;)
        ;
}

void DjiUser_BspInitTask(void const *argument)
{
    // LED Init
    Led_Init(LED_RED);
    Led_Init(LED_GREEN);
    Led_Init(LED_BLUE);

    // Debug UART Init
    UART_Init(DJI_CONSOLE_UART_NUM, DJI_CONSOLE_UART_BAUD);

    printf("\r\n========Payload sdk app firmware start!========\r\n");

    // Force Loader button Init
    Button_Init(BUTTON_KEY1, BUTTON_MODE_GPIO);

    // FTS Pin output Init
    Pwm_Init(PWM_FTS, 400, 0);

    // PPS Init
    DjiTest_PpsSignalResponseInit();

    // USBD CDC Init
    USBD_CDC_Init();

    // Flash Option Config Init
    FLASH_If_Init();

    for (;;)
    {
        Osal_TaskSleepMs(1);
        USBD_CDC_Handle();
    }
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(UART3, (uint8_t)ch);
    while (RESET == usart_flag_get(UART3, USART_FLAG_TBE));

    return ch;
}

int __io_putchar(int ch)
{
    usart_data_transmit(UART3, (uint8_t)ch);
    while (RESET == usart_flag_get(UART3, USART_FLAG_TBE));
    return 1;
}

int __io_getchar(void)
{
    return 0;
}
