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
#include "dji_ringbuffer.h"
#include "FreeRTOS.h"
#include "task.h"
#include "osal.h"
#include "gd32f5xx.h"

/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/
//uart uart buffer size define
#define UART1_READ_BUF_SIZE      2048
#define UART1_WRITE_BUF_SIZE     4096
#define UART2_READ_BUF_SIZE      2048
#define UART2_WRITE_BUF_SIZE     2048
#define UART3_READ_BUF_SIZE      1024
#define UART3_WRITE_BUF_SIZE     4096

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
static uint8_t s_uart1ReadBuf[UART1_READ_BUF_SIZE];
//UART1 write buffer
static uint8_t s_uart1WriteBuf[UART1_WRITE_BUF_SIZE];

//UART1 mutex
static T_DjiMutexHandle s_uart1Mutex;
//USART1 handle
//static UART_HandleTypeDef s_uart1Handle;
#endif

#ifdef USING_UART_PORT_2
static T_RingBuffer s_uart2ReadRingBuffer;
static T_UartBufferState s_uart2ReadBufferState;
static T_RingBuffer s_uart2WriteRingBuffer;
static T_UartBufferState s_uart2WriteBufferState;
static uint8_t s_uart2ReadBuf[UART2_READ_BUF_SIZE];
static uint8_t s_uart2WriteBuf[UART2_WRITE_BUF_SIZE];

static T_DjiMutexHandle s_uart2Mutex;
//static UART_HandleTypeDef s_uart2Handle;
#endif

#ifdef USING_UART_PORT_3
static T_RingBuffer s_uart3ReadRingBuffer;
static T_UartBufferState s_uart3ReadBufferState;
static T_RingBuffer s_uart3WriteRingBuffer;
static T_UartBufferState s_uart3WriteBufferState;
static uint8_t s_uart3ReadBuf[UART3_READ_BUF_SIZE];
static uint8_t s_uart3WriteBuf[UART3_WRITE_BUF_SIZE];

static T_DjiMutexHandle s_uart3Mutex;
//static UART_HandleTypeDef s_uart3Handle;
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

            rcu_periph_clock_enable(RCU_GPIOA);
            rcu_periph_clock_enable(RCU_USART0);

            gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_9);
            gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_10);

            /* configure USART Tx as alternate function push-pull */
            gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP,GPIO_PIN_9);
            gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_9);

            /* configure USART Rx as alternate function push-pull */
            gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_10);
            gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);

            nvic_irq_enable(USART0_IRQn, 0, 0);

            /* USART configure */
            usart_deinit(USART0);
            usart_baudrate_set(USART0,baudRate);
            usart_receive_config(USART0, USART_RECEIVE_ENABLE);
            usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
            usart_enable(USART0);
            usart_interrupt_enable(USART0, USART_INT_RBNE);

            Osal_MutexCreate(&s_uart1Mutex);
        }
            break;
#endif

#ifdef USING_UART_PORT_2
        case UART_NUM_2: {
            RingBuf_Init(&s_uart2ReadRingBuffer, s_uart2ReadBuf, UART2_READ_BUF_SIZE);
            RingBuf_Init(&s_uart2WriteRingBuffer, s_uart2WriteBuf, UART2_WRITE_BUF_SIZE);
            Osal_MutexCreate(&s_uart2Mutex);
        }
            break;
#endif

#ifdef USING_UART_PORT_3
        case UART_NUM_3: {
            RingBuf_Init(&s_uart3ReadRingBuffer, s_uart3ReadBuf, UART3_READ_BUF_SIZE);
            RingBuf_Init(&s_uart3WriteRingBuffer, s_uart3WriteBuf, UART3_WRITE_BUF_SIZE);

            rcu_periph_clock_enable(RCU_GPIOA);
            rcu_periph_clock_enable(RCU_UART3);

            gpio_af_set(GPIOA, GPIO_AF_8, GPIO_PIN_0);
            gpio_af_set(GPIOA, GPIO_AF_8, GPIO_PIN_1);

            /* configure USART Tx as alternate function push-pull */
            gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_0);
            gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);

            /* configure USART Rx as alternate function push-pull */
            gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_1);
            gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);

            nvic_irq_enable(UART3_IRQn, 0, 0);

            /* USART configure */
            usart_deinit(UART3);
            usart_baudrate_set(UART3,baudRate);
            usart_receive_config(UART3, USART_RECEIVE_ENABLE);
            usart_transmit_config(UART3, USART_TRANSMIT_ENABLE);
            usart_enable(UART3);
            usart_interrupt_enable(UART3, USART_INT_RBNE);

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

           /* enable USART TBE interrupt */
           usart_interrupt_enable(USART0, USART_INT_TBE);
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
//            Osal_MutexLock(s_uart2Mutex);
//            writeRealLen = RingBuf_Put(&s_uart2WriteRingBuffer, buf, writeSize);
//            __HAL_USART_ENABLE_IT(&s_uart2Handle, USART_IT_TXE);
//            usedCapacityOfBuffer = UART2_WRITE_BUF_SIZE - RingBuf_GetUnusedSize(&s_uart2WriteRingBuffer);
//            s_uart2WriteBufferState.maxUsedCapacityOfBuffer =
//                usedCapacityOfBuffer > s_uart2WriteBufferState.maxUsedCapacityOfBuffer ? usedCapacityOfBuffer
//                                                                                       : s_uart2WriteBufferState.maxUsedCapacityOfBuffer;
//            s_uart2WriteBufferState.countOfLostData += writeSize - writeRealLen;
//            Osal_MutexUnlock(s_uart2Mutex);
        }
            break;
#endif

#ifdef USING_UART_PORT_3
        case UART_NUM_3: {
           Osal_MutexLock(s_uart3Mutex);
           writeRealLen = RingBuf_Put(&s_uart3WriteRingBuffer, buf, writeSize);

           /* enable USART TBE interrupt */
           usart_interrupt_enable(UART3, USART_INT_TBE);
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

void USART0_IRQHandler(void)
{
    uint8_t data;
    uint16_t usedCapacityOfBuffer = 0;
    uint16_t realCountPutBuffer = 0;

    if((RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE)) &&
            (RESET != usart_flag_get(USART0, USART_FLAG_RBNE))) {
       data = usart_data_receive(USART0);
       realCountPutBuffer = RingBuf_Put(&s_uart1ReadRingBuffer, &data, 1);
       usedCapacityOfBuffer = UART1_READ_BUF_SIZE - RingBuf_GetUnusedSize(&s_uart1ReadRingBuffer);
       s_uart1ReadBufferState.maxUsedCapacityOfBuffer =
           usedCapacityOfBuffer > s_uart1ReadBufferState.maxUsedCapacityOfBuffer ? usedCapacityOfBuffer
                                                                                 : s_uart1ReadBufferState.maxUsedCapacityOfBuffer;
       s_uart1ReadBufferState.countOfLostData += 1 - realCountPutBuffer;
    }
    if((RESET != usart_flag_get(USART0, USART_FLAG_TBE)) &&
            (RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_TBE))) {
       if (RingBuf_Get(&s_uart1WriteRingBuffer, &data, 1)) {
           /* Transmit Data */
            usart_data_transmit(USART0, data);
       } else {
            usart_interrupt_disable(USART0, USART_INT_TBE);
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
}

#endif

/**
 * @brief UART3 interrupt request handler fucntion.
 */
#ifdef USING_UART_PORT_3

void UART3_IRQHandler(void)
{
    uint8_t data;
    uint16_t usedCapacityOfBuffer = 0;
    uint16_t realCountPutBuffer = 0;

    if((RESET != usart_interrupt_flag_get(UART3, USART_INT_FLAG_RBNE)) &&
            (RESET != usart_flag_get(UART3, USART_FLAG_RBNE))) {
       data = usart_data_receive(UART3);
       realCountPutBuffer = RingBuf_Put(&s_uart3ReadRingBuffer, &data, 1);
       usedCapacityOfBuffer = UART3_READ_BUF_SIZE - RingBuf_GetUnusedSize(&s_uart3ReadRingBuffer);
       s_uart3ReadBufferState.maxUsedCapacityOfBuffer =
           usedCapacityOfBuffer > s_uart3ReadBufferState.maxUsedCapacityOfBuffer ? usedCapacityOfBuffer
                                                                                 : s_uart3ReadBufferState.maxUsedCapacityOfBuffer;
       s_uart3ReadBufferState.countOfLostData += 1 - realCountPutBuffer;
    }
    if((RESET != usart_flag_get(UART3, USART_FLAG_TBE)) &&
            (RESET != usart_interrupt_flag_get(UART3, USART_INT_FLAG_TBE))) {
       if (RingBuf_Get(&s_uart3WriteRingBuffer, &data, 1)) {
           /* Transmit Data */
            usart_data_transmit(UART3, data);
       } else {
            usart_interrupt_disable(UART3, USART_INT_TBE);
       }
    }
}

#endif
