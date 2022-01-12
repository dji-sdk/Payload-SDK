/**
  ******************************************************************************
  * @file    UART/UART_TwoBoards_ComIT/Src/stm32f4xx_hal_msp.c
  * @author  MCD Application Team
  * @brief   HAL MSP module.    
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************  
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/** @addtogroup STM32F4xx_HAL_Examples
  * @{
  */

/** @defgroup UART_TwoBoards_ComIT
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
//USART1
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
#define USART3_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOD_CLK_ENABLE()
#define USART3_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOD_CLK_ENABLE()

#define USART3_FORCE_RESET()             __HAL_RCC_USART3_FORCE_RESET()
#define USART3_RELEASE_RESET()           __HAL_RCC_USART3_RELEASE_RESET()

/* Definition for USART3 Pins */
#define USART3_TX_PIN                    GPIO_PIN_8
#define USART3_TX_GPIO_PORT              GPIOD
#define USART3_TX_AF                     GPIO_AF7_USART3
#define USART3_RX_PIN                    GPIO_PIN_9
#define USART3_RX_GPIO_PORT              GPIOD
#define USART3_RX_AF                     GPIO_AF7_USART3

#define USART3_IRQ_PRIO_PRE      5
#define USART3_IRQ_PRIO_SUB      0

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup HAL_MSP_Private_Functions
  * @{
  */

/**
  * @brief UART MSP Initialization 
  *        This function configures the hardware resources used in this example: 
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration  
  *           - NVIC configuration for UART interrupt request enable
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
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
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO and NVIC configuration to their default state
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) {
        /*##-1- Reset peripherals ##################################################*/
        USART1_FORCE_RESET();
        USART1_RELEASE_RESET();

        /*##-2- Disable peripherals and GPIO Clocks #################################*/
        /* Configure UART Tx as alternate function  */
        HAL_GPIO_DeInit(USART1_TX_GPIO_PORT, USART1_TX_PIN);
        /* Configure UART Rx as alternate function  */
        HAL_GPIO_DeInit(USART1_RX_GPIO_PORT, USART1_RX_PIN);

        /*##-3- Disable the NVIC for UART ##########################################*/
        HAL_NVIC_DisableIRQ(USART1_IRQn);
    } else if (huart->Instance == USART2) {
        /*##-1- Reset peripherals ##################################################*/
        USART2_FORCE_RESET();
        USART2_RELEASE_RESET();

        /*##-2- Disable peripherals and GPIO Clocks #################################*/
        /* Configure UART Tx as alternate function  */
        HAL_GPIO_DeInit(USART2_TX_GPIO_PORT, USART2_TX_PIN);
        /* Configure UART Rx as alternate function  */
        HAL_GPIO_DeInit(USART2_RX_GPIO_PORT, USART2_RX_PIN);

        /*##-3- Disable the NVIC for UART ##########################################*/
        HAL_NVIC_DisableIRQ(USART2_IRQn);
    } else if (huart->Instance == USART3) {
        /*##-1- Reset peripherals ##################################################*/
        USART3_FORCE_RESET();
        USART3_RELEASE_RESET();

        /*##-2- Disable peripherals and GPIO Clocks #################################*/
        /* Configure UART Tx as alternate function  */
        HAL_GPIO_DeInit(USART3_TX_GPIO_PORT, USART3_TX_PIN);
        /* Configure UART Rx as alternate function  */
        HAL_GPIO_DeInit(USART3_RX_GPIO_PORT, USART3_RX_PIN);

        /*##-3- Disable the NVIC for UART ##########################################*/
        HAL_NVIC_DisableIRQ(USART3_IRQn);
    }
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
