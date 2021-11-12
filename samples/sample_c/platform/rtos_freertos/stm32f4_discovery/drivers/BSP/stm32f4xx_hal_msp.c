/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file         stm32f4xx_hal_msp.c
  * @brief        This file provides code for the MSP Initialization
  *               and de-Initialization codes.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* USER CODE BEGIN Includes */
#define USART1_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USART1_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

#define USART1_FORCE_RESET()             __HAL_RCC_USART1_FORCE_RESET()
#define USART1_RELEASE_RESET()           __HAL_RCC_USART1_RELEASE_RESET()

/* Definition for USART1 Pins */
#define USART1_TX_PIN                    GPIO_PIN_9
#define USART1_TX_GPIO_PORT              GPIOA
#define USART1_TX_AF                     GPIO_AF7_USART1
#define USART1_RX_PIN                    GPIO_PIN_10
#define USART1_RX_GPIO_PORT              GPIOA
#define USART1_RX_AF                     GPIO_AF7_USART1

#define USART1_IRQ_PRIO_PRE      5
#define USART1_IRQ_PRIO_SUB      0

//USART2
#define USART2_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USART2_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

#define USART2_FORCE_RESET()             __HAL_RCC_USART2_FORCE_RESET()
#define USART2_RELEASE_RESET()           __HAL_RCC_USART2_RELEASE_RESET()

/* Definition for USART2 Pins */
#define USART2_TX_PIN                    GPIO_PIN_2
#define USART2_TX_GPIO_PORT              GPIOA
#define USART2_TX_AF                     GPIO_AF7_USART2
#define USART2_RX_PIN                    GPIO_PIN_3
#define USART2_RX_GPIO_PORT              GPIOA
#define USART2_RX_AF                     GPIO_AF7_USART2

#define USART2_IRQ_PRIO_PRE      5
#define USART2_IRQ_PRIO_SUB      0

//USART3
#define USART3_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define USART3_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()

#define USART3_FORCE_RESET()             __HAL_RCC_USART3_FORCE_RESET()
#define USART3_RELEASE_RESET()           __HAL_RCC_USART3_RELEASE_RESET()

/* Definition for USART3 Pins */
#define USART3_TX_PIN                    GPIO_PIN_10
#define USART3_TX_GPIO_PORT              GPIOB
#define USART3_TX_AF                     GPIO_AF7_USART3
#define USART3_RX_PIN                    GPIO_PIN_11
#define USART3_RX_GPIO_PORT              GPIOB
#define USART3_RX_AF                     GPIO_AF7_USART3

#define USART3_IRQ_PRIO_PRE      5
#define USART3_IRQ_PRIO_SUB      0
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Define */

/* USER CODE END Define */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN Macro */

/* USER CODE END Macro */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* External functions --------------------------------------------------------*/
/* USER CODE BEGIN ExternalFunctions */

/* USER CODE END ExternalFunctions */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{
  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  /* System interrupt init*/
  /* PendSV_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(PendSV_IRQn, 15, 0);

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

/**
* @brief UART MSP Initialization
* This function configures the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    if (huart->Instance == USART1) {
        /*##-1- Enable peripherals and GPIO Clocks #################################*/
        /* Enable GPIO TX/RX clock */
        USART1_TX_GPIO_CLK_ENABLE();
        USART1_RX_GPIO_CLK_ENABLE();
        /* Enable USART clock */
        __HAL_RCC_USART1_CLK_ENABLE();

        /*##-2- Configure peripheral GPIO ##########################################*/
        /* UART TX GPIO pin configuration  */
        GPIO_InitStruct.Pin = USART1_TX_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
        GPIO_InitStruct.Alternate = USART1_TX_AF;

        HAL_GPIO_Init(USART1_TX_GPIO_PORT, &GPIO_InitStruct);

        /* UART RX GPIO pin configuration  */
        GPIO_InitStruct.Pin = USART1_RX_PIN;
        GPIO_InitStruct.Alternate = USART1_RX_AF;

        HAL_GPIO_Init(USART1_RX_GPIO_PORT, &GPIO_InitStruct);

        /*##-3- Configure the NVIC for UART ########################################*/
        /* NVIC for USART1 */
        HAL_NVIC_SetPriority(USART1_IRQn, USART1_IRQ_PRIO_PRE, USART1_IRQ_PRIO_SUB);
        HAL_NVIC_EnableIRQ(USART1_IRQn);
    } else if (huart->Instance == USART2) {
        /*##-1- Enable peripherals and GPIO Clocks #################################*/
        /* Enable GPIO TX/RX clock */
        USART2_TX_GPIO_CLK_ENABLE();
        USART2_RX_GPIO_CLK_ENABLE();
        /* Enable USART clock */
        __HAL_RCC_USART2_CLK_ENABLE();

        /*##-2- Configure peripheral GPIO ##########################################*/
        /* UART TX GPIO pin configuration  */
        GPIO_InitStruct.Pin = USART2_TX_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
        GPIO_InitStruct.Alternate = USART2_TX_AF;

        HAL_GPIO_Init(USART2_TX_GPIO_PORT, &GPIO_InitStruct);

        /* UART RX GPIO pin configuration  */
        GPIO_InitStruct.Pin = USART2_RX_PIN;
        GPIO_InitStruct.Alternate = USART2_RX_AF;

        HAL_GPIO_Init(USART2_RX_GPIO_PORT, &GPIO_InitStruct);

        /*##-3- Configure the NVIC for UART ########################################*/
        /* NVIC for USART2 */
        HAL_NVIC_SetPriority(USART2_IRQn, USART2_IRQ_PRIO_PRE, USART2_IRQ_PRIO_SUB);
        HAL_NVIC_EnableIRQ(USART2_IRQn);
    } else if (huart->Instance == USART3) {
        /*##-1- Enable peripherals and GPIO Clocks #################################*/
        /* Enable GPIO TX/RX clock */
        USART3_TX_GPIO_CLK_ENABLE();
        USART3_RX_GPIO_CLK_ENABLE();
        /* Enable USART clock */
        __HAL_RCC_USART3_CLK_ENABLE();

        /*##-2- Configure peripheral GPIO ##########################################*/
        /* UART TX GPIO pin configuration  */
        GPIO_InitStruct.Pin = USART3_TX_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
        GPIO_InitStruct.Alternate = USART3_TX_AF;

        HAL_GPIO_Init(USART3_TX_GPIO_PORT, &GPIO_InitStruct);

        /* UART RX GPIO pin configuration  */
        GPIO_InitStruct.Pin = USART3_RX_PIN;
        GPIO_InitStruct.Alternate = USART3_RX_AF;

        HAL_GPIO_Init(USART3_RX_GPIO_PORT, &GPIO_InitStruct);

        /*##-3- Configure the NVIC for UART ########################################*/
        /* NVIC for USART3 */
        HAL_NVIC_SetPriority(USART3_IRQn, USART3_IRQ_PRIO_PRE, USART3_IRQ_PRIO_SUB);
        HAL_NVIC_EnableIRQ(USART3_IRQn);
    }

}

/**
* @brief UART MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
  if(huart->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();

    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);

  /* USER CODE BEGIN USART2_MspDeInit 1 */

  /* USER CODE END USART2_MspDeInit 1 */
  }

}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
