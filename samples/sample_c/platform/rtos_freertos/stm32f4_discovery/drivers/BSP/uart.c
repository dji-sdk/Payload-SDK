/**
 ******************************************************************************
 * @file    uart.c
 * @version V1.0.0
 * @date    2017/11/10
 * @brief   The file define UART interface driver functions.
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
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "uart.h"
#include "stm32f4xx_hal.h"
#include "dji_ringbuffer.h"
#include "FreeRTOS.h"
#include "task.h"
#include "osal.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
//uart uart buffer size define
#define UART1_READ_BUF_SIZE      64
#define UART1_WRITE_BUF_SIZE     64
#define UART2_READ_BUF_SIZE      64
#define UART2_WRITE_BUF_SIZE     2048
#define UART3_READ_BUF_SIZE      8192
#define UART3_WRITE_BUF_SIZE     2048

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
#ifdef USING_UART_PORT_1
//UART1 read ring buffer structure
static T_RingBuffer s_uart1ReadRingBuffer;
//USART1 read buffer state
static T_UartBufferState s_uart1ReadBufferState;
//UART1 write ring buffer structure
static T_RingBuffer s_uart1WriteRingBuffer;
//USART1 write buffer state
static T_UartBufferState s_uart1WriteBufferState;
//UART1 read buffer
CCMRAM static uint8_t s_uart1ReadBuf[UART1_READ_BUF_SIZE];
//UART1 write buffer
CCMRAM static uint8_t s_uart1WriteBuf[UART1_WRITE_BUF_SIZE];

//UART1 mutex
static T_DjiMutexHandle s_uart1Mutex;
//USART1 handle
static UART_HandleTypeDef s_uart1Handle;
#endif

#ifdef USING_UART_PORT_2
static T_RingBuffer s_uart2ReadRingBuffer;
static T_UartBufferState s_uart2ReadBufferState;
static T_RingBuffer s_uart2WriteRingBuffer;
static T_UartBufferState s_uart2WriteBufferState;
CCMRAM static uint8_t s_uart2ReadBuf[UART2_READ_BUF_SIZE];
CCMRAM static uint8_t s_uart2WriteBuf[UART2_WRITE_BUF_SIZE];

static T_DjiMutexHandle s_uart2Mutex;
static UART_HandleTypeDef s_uart2Handle;
#endif

#ifdef USING_UART_PORT_3
static T_RingBuffer s_uart3ReadRingBuffer;
static T_UartBufferState s_uart3ReadBufferState;
static T_RingBuffer s_uart3WriteRingBuffer;
static T_UartBufferState s_uart3WriteBufferState;
CCMRAM static uint8_t s_uart3ReadBuf[UART3_READ_BUF_SIZE];
CCMRAM static uint8_t s_uart3WriteBuf[UART3_WRITE_BUF_SIZE];

static T_DjiMutexHandle s_uart3Mutex;
static UART_HandleTypeDef s_uart3Handle;
#endif

/* Exported variables --------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief UART initialization function.
 * @param uartNum UART number to be initialized.
 * @param baudRate UART baudrate.
 * @return None.
 */
void UART_Init(E_UartNum uartNum, uint32_t baudRate)
{
    switch (uartNum) {

#ifdef USING_UART_PORT_1
        case UART_NUM_1: {
            RingBuf_Init(&s_uart1ReadRingBuffer, s_uart1ReadBuf, UART1_READ_BUF_SIZE);
            RingBuf_Init(&s_uart1WriteRingBuffer, s_uart1WriteBuf, UART1_WRITE_BUF_SIZE);

            s_uart1Handle.Instance = USART1;
            s_uart1Handle.Init.BaudRate = baudRate;
            s_uart1Handle.Init.WordLength = UART_WORDLENGTH_8B;
            s_uart1Handle.Init.StopBits = UART_STOPBITS_1;
            s_uart1Handle.Init.Parity = UART_PARITY_NONE;
            s_uart1Handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
            s_uart1Handle.Init.Mode = UART_MODE_TX_RX;
            s_uart1Handle.Init.OverSampling = UART_OVERSAMPLING_16;
            HAL_UART_Init(&s_uart1Handle);
            __HAL_UART_ENABLE_IT(&s_uart1Handle, UART_IT_RXNE);

            Osal_MutexCreate(&s_uart1Mutex);
        }
            break;
#endif

#ifdef USING_UART_PORT_2
        case UART_NUM_2: {
            RingBuf_Init(&s_uart2ReadRingBuffer, s_uart2ReadBuf, UART2_READ_BUF_SIZE);
            RingBuf_Init(&s_uart2WriteRingBuffer, s_uart2WriteBuf, UART2_WRITE_BUF_SIZE);

            s_uart2Handle.Instance = USART2;
            s_uart2Handle.Init.BaudRate = baudRate;
            s_uart2Handle.Init.WordLength = UART_WORDLENGTH_8B;
            s_uart2Handle.Init.StopBits = UART_STOPBITS_1;
            s_uart2Handle.Init.Parity = UART_PARITY_NONE;
            s_uart2Handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
            s_uart2Handle.Init.Mode = UART_MODE_TX_RX;
            s_uart2Handle.Init.OverSampling = UART_OVERSAMPLING_16;
            HAL_UART_Init(&s_uart2Handle);
            __HAL_UART_ENABLE_IT(&s_uart2Handle, UART_IT_RXNE);

            Osal_MutexCreate(&s_uart2Mutex);
        }
            break;
#endif

#ifdef USING_UART_PORT_3
        case UART_NUM_3: {
            RingBuf_Init(&s_uart3ReadRingBuffer, s_uart3ReadBuf, UART3_READ_BUF_SIZE);
            RingBuf_Init(&s_uart3WriteRingBuffer, s_uart3WriteBuf, UART3_WRITE_BUF_SIZE);

            s_uart3Handle.Instance = USART3;
            s_uart3Handle.Init.BaudRate = baudRate;
            s_uart3Handle.Init.WordLength = UART_WORDLENGTH_8B;
            s_uart3Handle.Init.StopBits = UART_STOPBITS_1;
            s_uart3Handle.Init.Parity = UART_PARITY_NONE;
            s_uart3Handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
            s_uart3Handle.Init.Mode = UART_MODE_TX_RX;
            s_uart3Handle.Init.OverSampling = UART_OVERSAMPLING_16;
            HAL_UART_Init(&s_uart3Handle);
            __HAL_UART_ENABLE_IT(&s_uart3Handle, UART_IT_RXNE);

            Osal_MutexCreate(&s_uart3Mutex);
        }
            break;
#endif

        default:
            break;
    }
}

/**
 * @brief Read UART data.
 * @param uartNum UART number.
 * @param buf Pointer to buffer used to store data.
 * @param readSize Size of data to be read.
 * @return Size of data read actually.
 */
int UART_Read(E_UartNum uartNum, uint8_t *buf, uint16_t readSize)
{
    uint16_t readRealSize;

    switch (uartNum) {

#ifdef USING_UART_PORT_1
        case UART_NUM_1: {
            Osal_MutexLock(s_uart1Mutex);
            readRealSize = RingBuf_Get(&s_uart1ReadRingBuffer, buf, readSize);
            Osal_MutexUnlock(s_uart1Mutex);
        }
            break;
#endif

#ifdef USING_UART_PORT_2
        case UART_NUM_2: {
            Osal_MutexLock(s_uart2Mutex);
            readRealSize = RingBuf_Get(&s_uart2ReadRingBuffer, buf, readSize);
            Osal_MutexUnlock(s_uart2Mutex);
        }
            break;
#endif

#ifdef USING_UART_PORT_3
        case UART_NUM_3: {
            Osal_MutexLock(s_uart3Mutex);
            readRealSize = RingBuf_Get(&s_uart3ReadRingBuffer, buf, readSize);
            Osal_MutexUnlock(s_uart3Mutex);
        }
            break;
#endif

        default:
            return UART_ERROR;
    }

    return readRealSize;
}

/**
 * @brief Write UART data.
 * @param uartNum UART number.
 * @param buf Pointer to buffer used to store data.
 * @param writeSize Size of data to be write.
 * @return Size of data wrote actually.
 */
int UART_Write(E_UartNum uartNum, const uint8_t *buf, uint16_t writeSize)
{
    int writeRealLen;
    uint16_t usedCapacityOfBuffer = 0;

    switch (uartNum) {

#ifdef USING_UART_PORT_1
        case UART_NUM_1: {
            Osal_MutexLock(s_uart1Mutex);
            writeRealLen = RingBuf_Put(&s_uart1WriteRingBuffer, buf, writeSize);
            __HAL_UART_ENABLE_IT(&s_uart1Handle, UART_IT_TXE);
            usedCapacityOfBuffer = UART1_WRITE_BUF_SIZE - RingBuf_GetUnusedSize(&s_uart1WriteRingBuffer);
            s_uart1WriteBufferState.maxUsedCapacityOfBuffer =
                usedCapacityOfBuffer > s_uart1WriteBufferState.maxUsedCapacityOfBuffer ? usedCapacityOfBuffer
                                                                                       : s_uart1WriteBufferState.maxUsedCapacityOfBuffer;
            s_uart1WriteBufferState.countOfLostData += writeSize - writeRealLen;
            Osal_MutexUnlock(s_uart1Mutex);
        }
            break;
#endif

#ifdef USING_UART_PORT_2
        case UART_NUM_2: {
            Osal_MutexLock(s_uart2Mutex);
            writeRealLen = RingBuf_Put(&s_uart2WriteRingBuffer, buf, writeSize);
            __HAL_UART_ENABLE_IT(&s_uart2Handle, UART_IT_TXE);
            usedCapacityOfBuffer = UART2_WRITE_BUF_SIZE - RingBuf_GetUnusedSize(&s_uart2WriteRingBuffer);
            s_uart2WriteBufferState.maxUsedCapacityOfBuffer =
                usedCapacityOfBuffer > s_uart2WriteBufferState.maxUsedCapacityOfBuffer ? usedCapacityOfBuffer
                                                                                       : s_uart2WriteBufferState.maxUsedCapacityOfBuffer;
            s_uart2WriteBufferState.countOfLostData += writeSize - writeRealLen;
            Osal_MutexUnlock(s_uart2Mutex);
        }
            break;
#endif

#ifdef USING_UART_PORT_3
        case UART_NUM_3: {
            Osal_MutexLock(s_uart3Mutex);
            writeRealLen = RingBuf_Put(&s_uart3WriteRingBuffer, buf, writeSize);
            __HAL_UART_ENABLE_IT(&s_uart3Handle, UART_IT_TXE);
            usedCapacityOfBuffer = UART3_WRITE_BUF_SIZE - RingBuf_GetUnusedSize(&s_uart3WriteRingBuffer);
            s_uart3WriteBufferState.maxUsedCapacityOfBuffer =
                usedCapacityOfBuffer > s_uart3WriteBufferState.maxUsedCapacityOfBuffer ? usedCapacityOfBuffer
                                                                                       : s_uart3WriteBufferState.maxUsedCapacityOfBuffer;
            s_uart3WriteBufferState.countOfLostData += writeSize - writeRealLen;
            Osal_MutexUnlock(s_uart3Mutex);
        }
            break;
#endif

        default:
            return UART_ERROR;
    }

    return writeRealLen;
}

void UART_GetBufferState(E_UartNum uartNum, T_UartBufferState *readBufferState, T_UartBufferState *writeBufferState)
{
    switch (uartNum) {
#ifdef USING_UART_PORT_1
        case UART_NUM_1:
            memcpy(readBufferState, &s_uart1ReadBufferState, sizeof(T_UartBufferState));
            memcpy(writeBufferState, &s_uart1WriteBufferState, sizeof(T_UartBufferState));
            break;
#endif
#ifdef USING_UART_PORT_2
        case UART_NUM_2:
            memcpy(readBufferState, &s_uart2ReadBufferState, sizeof(T_UartBufferState));
            memcpy(writeBufferState, &s_uart2WriteBufferState, sizeof(T_UartBufferState));
            break;
#endif
#ifdef USING_UART_PORT_3
        case UART_NUM_3:
            memcpy(readBufferState, &s_uart3ReadBufferState, sizeof(T_UartBufferState));
            memcpy(writeBufferState, &s_uart3WriteBufferState, sizeof(T_UartBufferState));
            break;
#endif
        default:
            break;
    }
}

/**
 * @brief UART1 interrupt request handler fucntion.
 */
#ifdef USING_UART_PORT_1

void USART1_IRQHandler(void)
{
    uint8_t data;
    uint16_t usedCapacityOfBuffer = 0;
    uint16_t realCountPutBuffer = 0;

    if (__HAL_UART_GET_IT_SOURCE(&s_uart1Handle, UART_IT_RXNE) != RESET &&
        __HAL_UART_GET_FLAG(&s_uart1Handle, UART_FLAG_RXNE) != RESET) {
        data = (uint8_t) ((uint16_t) (s_uart1Handle.Instance->DR & (uint16_t) 0x01FF) & (uint16_t) 0x00FF);
        realCountPutBuffer = RingBuf_Put(&s_uart1ReadRingBuffer, &data, 1);
        usedCapacityOfBuffer = UART1_READ_BUF_SIZE - RingBuf_GetUnusedSize(&s_uart1ReadRingBuffer);
        s_uart1ReadBufferState.maxUsedCapacityOfBuffer =
            usedCapacityOfBuffer > s_uart1ReadBufferState.maxUsedCapacityOfBuffer ? usedCapacityOfBuffer
                                                                                  : s_uart1ReadBufferState.maxUsedCapacityOfBuffer;
        s_uart1ReadBufferState.countOfLostData += 1 - realCountPutBuffer;
    }

    if (__HAL_UART_GET_IT_SOURCE(&s_uart1Handle, UART_IT_TXE) != RESET &&
        __HAL_UART_GET_FLAG(&s_uart1Handle, UART_FLAG_TXE) != RESET) {
        if (RingBuf_Get(&s_uart1WriteRingBuffer, &data, 1)) {
            /* Transmit Data */
            s_uart1Handle.Instance->DR = ((uint16_t) data & (uint16_t) 0x01FF);
        } else {
            __HAL_UART_DISABLE_IT(&s_uart1Handle, UART_IT_TXE);
        }
    }
}

#endif

/**
 * @brief UART2 interrupt request handler fucntion.
 */
#ifdef USING_UART_PORT_2

void USART2_IRQHandler(void)
{
    uint8_t data;
    uint16_t usedCapacityOfBuffer = 0;
    uint16_t realCountPutBuffer = 0;

    if (__HAL_UART_GET_IT_SOURCE(&s_uart2Handle, UART_IT_RXNE) != RESET &&
        __HAL_UART_GET_FLAG(&s_uart2Handle, UART_FLAG_RXNE) != RESET) {
        data = (uint8_t) ((uint16_t) (s_uart2Handle.Instance->DR & (uint16_t) 0x01FF) & (uint16_t) 0x00FF);
        realCountPutBuffer = RingBuf_Put(&s_uart2ReadRingBuffer, &data, 1);
        usedCapacityOfBuffer = UART2_READ_BUF_SIZE - RingBuf_GetUnusedSize(&s_uart2ReadRingBuffer);
        s_uart2ReadBufferState.maxUsedCapacityOfBuffer =
            usedCapacityOfBuffer > s_uart2ReadBufferState.maxUsedCapacityOfBuffer ? usedCapacityOfBuffer
                                                                                  : s_uart2ReadBufferState.maxUsedCapacityOfBuffer;
        s_uart2ReadBufferState.countOfLostData += 1 - realCountPutBuffer;
    }

    if (__HAL_UART_GET_IT_SOURCE(&s_uart2Handle, UART_IT_TXE) != RESET &&
        __HAL_UART_GET_FLAG(&s_uart2Handle, UART_FLAG_TXE) != RESET) {
        if (RingBuf_Get(&s_uart2WriteRingBuffer, &data, 1)) {
            /* Transmit Data */
            s_uart2Handle.Instance->DR = ((uint16_t) data & (uint16_t) 0x01FF);
        } else {
            __HAL_UART_DISABLE_IT(&s_uart2Handle, UART_IT_TXE);
        }
    }
}

#endif

/**
 * @brief UART3 interrupt request handler fucntion.
 */
#ifdef USING_UART_PORT_3

void USART3_IRQHandler(void)
{
    uint8_t data;
    uint16_t usedCapacityOfBuffer = 0;
    uint16_t realCountPutBuffer = 0;

    if (__HAL_UART_GET_IT_SOURCE(&s_uart3Handle, UART_IT_RXNE) != RESET &&
        __HAL_UART_GET_FLAG(&s_uart3Handle, UART_FLAG_RXNE) != RESET) {
        data = (uint8_t) ((uint16_t) (s_uart3Handle.Instance->DR & (uint16_t) 0x01FF) & (uint16_t) 0x00FF);
        realCountPutBuffer = RingBuf_Put(&s_uart3ReadRingBuffer, &data, 1);
        usedCapacityOfBuffer = UART3_READ_BUF_SIZE - RingBuf_GetUnusedSize(&s_uart3ReadRingBuffer);
        s_uart3ReadBufferState.maxUsedCapacityOfBuffer =
            usedCapacityOfBuffer > s_uart3ReadBufferState.maxUsedCapacityOfBuffer ? usedCapacityOfBuffer
                                                                                  : s_uart3ReadBufferState.maxUsedCapacityOfBuffer;
        s_uart3ReadBufferState.countOfLostData += 1 - realCountPutBuffer;
    }

    if (__HAL_UART_GET_IT_SOURCE(&s_uart3Handle, UART_IT_TXE) != RESET &&
        __HAL_UART_GET_FLAG(&s_uart3Handle, UART_FLAG_TXE) != RESET) {
        if (RingBuf_Get(&s_uart3WriteRingBuffer, &data, 1)) {
            /* Transmit Data */
            s_uart3Handle.Instance->DR = ((uint16_t) data & (uint16_t) 0x01FF);
        } else {
            __HAL_UART_DISABLE_IT(&s_uart3Handle, UART_IT_TXE);
        }
    }
}

#endif

#ifdef __CC_ARM
int fputc(int ch,FILE *f)
{
    if (DJI_CONSOLE_UART_NUM == UART_NUM_1) {
        HAL_UART_Transmit(&s_uart1Handle, (uint8_t *) &ch, 1, 0xFFFF);
    } else if (DJI_CONSOLE_UART_NUM == UART_NUM_2) {
        HAL_UART_Transmit(&s_uart2Handle, (uint8_t *) &ch, 1, 0xFFFF);
    } else if (DJI_CONSOLE_UART_NUM == UART_NUM_3) {
        HAL_UART_Transmit(&s_uart3Handle, (uint8_t *) &ch, 1, 0xFFFF);
    }

    return ch;
}
#else
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

PUTCHAR_PROTOTYPE
{
    if (DJI_CONSOLE_UART_NUM == UART_NUM_1) {
        HAL_UART_Transmit(&s_uart1Handle, (uint8_t *) &ch, 1, 0xFFFF);
    } else if (DJI_CONSOLE_UART_NUM == UART_NUM_2) {
        HAL_UART_Transmit(&s_uart2Handle, (uint8_t *) &ch, 1, 0xFFFF);
    } else if (DJI_CONSOLE_UART_NUM == UART_NUM_3) {
        HAL_UART_Transmit(&s_uart3Handle, (uint8_t *) &ch, 1, 0xFFFF);
    }

    return ch;
}

#endif
