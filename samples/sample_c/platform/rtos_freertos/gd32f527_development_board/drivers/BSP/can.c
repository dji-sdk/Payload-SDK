/**
 ******************************************************************************
 * @file    can.c
 * @version V1.0.0
 * @date    2024/05/22
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
#include "can.h"
#include "dji_ringbuffer.h"
#include "FreeRTOS.h"
#include "task.h"
#include "osal.h"
#include "gd32f5xx.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
// uart uart buffer size define
#define CAN0_READ_BUF_SIZE 1024
#define CAN0_WRITE_BUF_SIZE 4096
#define CAN1_READ_BUF_SIZE 8192
#define CAN1_WRITE_BUF_SIZE 8192

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
#ifdef USING_CAN0
// CAN0 read ring buffer structure
static T_RingBuffer s_uart1ReadRingBuffer;
// CAN0 read buffer state
static T_CanBufferState s_uart1ReadBufferState;
static T_RingBuffer s_uart1WriteRingBuffer;
// CAN0 write buffer state
static T_CanBufferState s_uart1WriteBufferState;
// CAN0 read buffer
static uint8_t s_uart1ReadBuf[CAN0_READ_BUF_SIZE];
// CAN0 write buffer
static uint8_t s_uart1WriteBuf[CAN0_WRITE_BUF_SIZE];
// CAN0 mutex
static T_DjiMutexHandle s_can0Mutex;
#endif

#ifdef USING_CAN1
static T_RingBuffer s_can1ReadRingBuffer;
static T_CanBufferState s_can1ReadBufferState;
static T_RingBuffer s_can1WriteRingBuffer;
static uint8_t s_can1ReadBuf[CAN1_READ_BUF_SIZE];
static uint8_t s_can1WriteBuf[CAN1_WRITE_BUF_SIZE];
static T_DjiMutexHandle s_can1Mutex;
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
void CAN_Init(E_CanNum canNum)
{
    switch (canNum)
    {

#ifdef USING_CAN0
    case UART_NUM_1:
    {
        RingBuf_Init(&s_uart1ReadRingBuffer, s_uart1ReadBuf, UART1_READ_BUF_SIZE);
        RingBuf_Init(&s_uart1WriteRingBuffer, s_uart1WriteBuf, UART1_WRITE_BUF_SIZE);

        rcu_periph_clock_enable(RCU_GPIOA);
        rcu_periph_clock_enable(RCU_USART0);

        gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_9);
        gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_10);

        /* configure USART Tx as alternate function push-pull */
        gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_9);
        gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);

        /* configure USART Rx as alternate function push-pull */
        gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_10);
        gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);

        nvic_irq_enable(USART0_IRQn, 0, 0);

        /* USART configure */
        usart_deinit(USART0);
        usart_baudrate_set(USART0, baudRate);
        usart_receive_config(USART0, USART_RECEIVE_ENABLE);
        usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
        usart_enable(USART0);
        usart_interrupt_enable(USART0, USART_INT_RBNE);

        Osal_MutexCreate(&s_uart1Mutex);
    }
    break;
#endif

#ifdef USING_CAN1
    case CAN_NUM_1:
    {
        RingBuf_Init(&s_can1ReadRingBuffer, s_can1ReadBuf, CAN1_READ_BUF_SIZE);
        RingBuf_Init(&s_can1WriteRingBuffer, s_can1WriteBuf, CAN1_WRITE_BUF_SIZE);

        /* Enable CAN clock */
        rcu_periph_clock_enable(RCU_CAN0);
        rcu_periph_clock_enable(RCU_CAN1);
        rcu_periph_clock_enable(RCU_GPIOB);

        /* Configure CAN1 GPIO */
        gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5);
        gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5);
        gpio_af_set(GPIOB, GPIO_AF_9, GPIO_PIN_5);

        gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
        gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6);
        gpio_af_set(GPIOB, GPIO_AF_9, GPIO_PIN_6);

        can_parameter_struct can_parameter;
        can_struct_para_init(CAN_INIT_STRUCT, &can_parameter);
        can_deinit(CAN1);

        /* Initialize CAN parameters */
        can_parameter.time_triggered = DISABLE;
        can_parameter.auto_bus_off_recovery = ENABLE;
        can_parameter.auto_wake_up = DISABLE;
        can_parameter.auto_retrans = ENABLE;
        can_parameter.rec_fifo_overwrite = DISABLE;
        can_parameter.trans_fifo_order = DISABLE;
        can_parameter.working_mode = CAN_NORMAL_MODE;
        /* Configure CAN0/1 baud rate 1MBps, sample point at 80% */
        can_parameter.resync_jump_width = CAN_BT_SJW_1TQ;
        can_parameter.time_segment_1 = CAN_BT_BS1_7TQ;
        can_parameter.time_segment_2 = CAN_BT_BS2_2TQ;
        can_parameter.prescaler = 5;
        /* Initialize CAN */
        can_init(CAN1, &can_parameter);

        /* initialize filter */
        can1_filter_start_bank(14);

        can_filter_mask_mode_init(0x12, 0x0000, CAN_STANDARD_FIFO0, 15);
        nvic_irq_enable(CAN1_RX0_IRQn, 1, 0);
        can_interrupt_enable(CAN1, CAN_INTEN_RFNEIE0);

        // nvic_irq_enable(CAN1_TX_IRQn, 1, 0);
        // can_interrupt_enable(CAN1, CAN_INTEN_TMEIE);

        Osal_MutexCreate(&s_can1Mutex);
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
int CAN_Read(E_CanNum canNum, uint8_t *buf, uint16_t readSize)
{
    uint16_t readRealSize;

    switch (canNum)
    {
#ifdef USING_CAN0
    case CAN_NUM_0:
    {
        Osal_MutexLock(s_uart1Mutex);
        readRealSize = RingBuf_Get(&s_uart1ReadRingBuffer, buf, readSize);
        Osal_MutexUnlock(s_uart1Mutex);
    }
    break;
#endif

#ifdef USING_CAN1
    case CAN_NUM_1:
    {
        Osal_MutexLock(s_can1Mutex);
        readRealSize = RingBuf_Get(&s_can1ReadRingBuffer, buf, readSize);
        Osal_MutexUnlock(s_can1Mutex);
    }
    break;
#endif

    default:
        return CAN_ERROR;
    }

    return readRealSize;
}

/**
 * @brief Write CAN data.
 * @param canNum CAN number.
 * @param buf Pointer to buffer used to store data.
 * @param writeSize Size of data to be write.
 * @return Size of data wrote actually.
 */
int CAN_Write(E_CanNum canNum, const uint8_t *buf, uint16_t writeSize)
{
    int writeRealLen = 0;
    can_trasnmit_message_struct transmit_message;
    int32_t i = 0;
    int32_t j = 0;
    uint32_t packageCnt = 0;
    uint32_t LastpackageSize = 0;

    switch (canNum)
    {

#ifdef USING_CAN0
    case UART_NUM_1:
    {
        uint16_t usedCapacityOfBuffer = 0;

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

#ifdef USING_CAN1
    case CAN_NUM_1:
    {
        can_struct_para_init(CAN_TX_MESSAGE_STRUCT, &transmit_message);
        transmit_message.tx_efid = 0x00;
        transmit_message.tx_ft = CAN_FT_DATA;
        transmit_message.tx_ff = CAN_FF_STANDARD;

        packageCnt = writeSize / 8;
        LastpackageSize = writeSize % 8;

        for (i = 0; i < packageCnt; i++)
        {
            for (j = 0; j < 8; j++)
            {
                transmit_message.tx_data[j] = buf[i * 8 + j];
            }
            transmit_message.tx_sfid = 0x21;
            transmit_message.tx_dlen = 8;
            can_message_transmit(CAN1, &transmit_message);
            Osal_TaskSleepMs(1);
        }

        if (LastpackageSize > 0)
        {
            for (j = 0; j < LastpackageSize; j++)
            {
                transmit_message.tx_data[j] = buf[packageCnt * 8 + j];
            }

            transmit_message.tx_sfid = 0x21;
            transmit_message.tx_dlen = LastpackageSize;
            can_message_transmit(CAN1, &transmit_message);
            Osal_TaskSleepMs(1);
        }
    }
    break;
#endif

    default:
        return CAN_ERROR;
    }

    return writeRealLen;
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

    if ((RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE)) &&
        (RESET != usart_flag_get(USART0, USART_FLAG_RBNE)))
    {
        data = usart_data_receive(USART0);
        realCountPutBuffer = RingBuf_Put(&s_uart1ReadRingBuffer, &data, 1);
        usedCapacityOfBuffer = UART1_READ_BUF_SIZE - RingBuf_GetUnusedSize(&s_uart1ReadRingBuffer);
        s_uart1ReadBufferState.maxUsedCapacityOfBuffer =
            usedCapacityOfBuffer > s_uart1ReadBufferState.maxUsedCapacityOfBuffer ? usedCapacityOfBuffer
                                                                                  : s_uart1ReadBufferState.maxUsedCapacityOfBuffer;
        s_uart1ReadBufferState.countOfLostData += 1 - realCountPutBuffer;
    }
    if ((RESET != usart_flag_get(USART0, USART_FLAG_TBE)) &&
        (RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_TBE)))
    {
        if (RingBuf_Get(&s_uart1WriteRingBuffer, &data, 1))
        {
            /* Transmit Data */
            usart_data_transmit(USART0, data);
        }
        else
        {
            usart_interrupt_disable(USART0, USART_INT_TBE);
        }
    }

    //    if (__HAL_USART_GET_IT_SOURCE(&s_uart1Handle, USART_IT_RXNE) != RESET &&
    //        __HAL_USART_GET_FLAG(&s_uart1Handle, USART_FLAG_RXNE) != RESET) {
    //        data = (uint8_t) ((uint16_t) (s_uart1Handle.Instance->DR & (uint16_t) 0x01FF) & (uint16_t) 0x00FF);
    //        realCountPutBuffer = RingBuf_Put(&s_uart1ReadRingBuffer, &data, 1);
    //        usedCapacityOfBuffer = UART1_READ_BUF_SIZE - RingBuf_GetUnusedSize(&s_uart1ReadRingBuffer);
    //        s_uart1ReadBufferState.maxUsedCapacityOfBuffer =
    //            usedCapacityOfBuffer > s_uart1ReadBufferState.maxUsedCapacityOfBuffer ? usedCapacityOfBuffer
    //                                                                                  : s_uart1ReadBufferState.maxUsedCapacityOfBuffer;
    //        s_uart1ReadBufferState.countOfLostData += 1 - realCountPutBuffer;
    //    }

    //    if (__HAL_USART_GET_IT_SOURCE(&s_uart1Handle, USART_IT_TXE) != RESET &&
    //        __HAL_USART_GET_FLAG(&s_uart1Handle, USART_FLAG_TXE) != RESET) {
    //        if (RingBuf_Get(&s_uart1WriteRingBuffer, &data, 1)) {
    //            /* Transmit Data */
    //            s_uart1Handle.Instance->DR = ((uint16_t) data & (uint16_t) 0x01FF);
    //        } else {
    //            __HAL_USART_DISABLE_IT(&s_uart1Handle, USART_IT_TXE);
    //        }
    //    }
}

#endif

/**
 * @brief CAN1 interrupt request handler fucntion.
 */
#ifdef USING_CAN1

void CAN1_TX_IRQHandler(void)
{
    // if(RESET != can_interrupt_flag_get(CAN1, CAN_INT_FLAG_MTF0))
    // {
    //     can_interrupt_flag_clear(CAN1, CAN_INT_FLAG_MTF0);
    //     // printf("\r\nMTF0 data transmmit success\r\n");
    // }
    // if(RESET != can_interrupt_flag_get(CAN1, CAN_INT_FLAG_MTF1))
    // {
    //     can_interrupt_flag_clear(CAN1, CAN_INT_FLAG_MTF1);
    //     // printf("\r\nMTF1 data transmmit success\r\n");
    // }
    // if(RESET != can_interrupt_flag_get(CAN1, CAN_INT_FLAG_MTF2))
    // {
    //     can_interrupt_flag_clear(CAN1, CAN_INT_FLAG_MTF2);
    //     // printf("\r\nMTF2 data transmmit success\r\n");
    // }

    // Osal_SemaphorePost(s_can1Sema);

    // uint32_t readLen = 0;
    // uint8_t data[8] = {0};
    // can_trasnmit_message_struct transmit_message;

    // readLen = RingBuf_Get(&s_can1WriteRingBuffer, &data, 8);
    // if (readLen <= 8)
    // {
    //     /* Transmit Data */
    //     can_struct_para_init(CAN_TX_MESSAGE_STRUCT, &transmit_message);
    //     transmit_message.tx_sfid = 0x00;
    //     transmit_message.tx_efid = 0x00;
    //     transmit_message.tx_ft = CAN_FT_DATA;
    //     transmit_message.tx_ff = CAN_FF_STANDARD;
    //     transmit_message.tx_dlen = 8;
    //     /* initialize receive message */
    //     can_struct_para_init(CAN_RX_MESSAGE_STRUCT, &transmit_message);

    //     transmit_message.tx_dlen = readLen;
    //     transmit_message.tx_sfid = 0x21;
    //     can_message_transmit(CAN1, &transmit_message);
    // }
    // else
    // {
    //     can_interrupt_disable(CAN1, CAN_INTEN_TMEIE);
    // }
}

void CAN1_RX0_IRQHandler(void)
{
    can_receive_message_struct receive_message;
    uint16_t usedCapacityOfBuffer = 0;
    uint16_t realCountPutBuffer = 0;

    /* check the receive message */
    can_message_receive(CAN1, CAN_FIFO0, &receive_message);

    realCountPutBuffer = RingBuf_Put(&s_can1ReadRingBuffer, (const uint8_t *)receive_message.rx_data, receive_message.rx_dlen);
    usedCapacityOfBuffer = CAN1_READ_BUF_SIZE - RingBuf_GetUnusedSize(&s_can1ReadRingBuffer);
    s_can1ReadBufferState.maxUsedCapacityOfBuffer =
        usedCapacityOfBuffer > s_can1ReadBufferState.maxUsedCapacityOfBuffer ? usedCapacityOfBuffer
                                                                             : s_can1ReadBufferState.maxUsedCapacityOfBuffer;
    s_can1ReadBufferState.countOfLostData += 1 - realCountPutBuffer;
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

    if ((RESET != usart_interrupt_flag_get(UART3, USART_INT_FLAG_RBNE)) &&
        (RESET != usart_flag_get(UART3, USART_FLAG_RBNE)))
    {
        data = usart_data_receive(UART3);
        realCountPutBuffer = RingBuf_Put(&s_uart3ReadRingBuffer, &data, 1);
        usedCapacityOfBuffer = UART3_READ_BUF_SIZE - RingBuf_GetUnusedSize(&s_uart3ReadRingBuffer);
        s_uart3ReadBufferState.maxUsedCapacityOfBuffer =
            usedCapacityOfBuffer > s_uart3ReadBufferState.maxUsedCapacityOfBuffer ? usedCapacityOfBuffer
                                                                                  : s_uart3ReadBufferState.maxUsedCapacityOfBuffer;
        s_uart3ReadBufferState.countOfLostData += 1 - realCountPutBuffer;
    }
    if ((RESET != usart_flag_get(UART3, USART_FLAG_TBE)) &&
        (RESET != usart_interrupt_flag_get(UART3, USART_INT_FLAG_TBE)))
    {
        if (RingBuf_Get(&s_uart3WriteRingBuffer, &data, 1))
        {
            /* Transmit Data */
            usart_data_transmit(UART3, data);
        }
        else
        {
            usart_interrupt_disable(UART3, USART_INT_TBE);
        }
    }

    //    if (__HAL_USART_GET_IT_SOURCE(&s_uart3Handle, USART_IT_RXNE) != RESET &&
    //        __HAL_USART_GET_FLAG(&s_uart3Handle, USART_FLAG_RXNE) != RESET) {
    //        data = (uint8_t) ((uint16_t) (s_uart3Handle.Instance->DR & (uint16_t) 0x01FF) & (uint16_t) 0x00FF);
    //        realCountPutBuffer = RingBuf_Put(&s_uart3ReadRingBuffer, &data, 1);
    //        usedCapacityOfBuffer = UART3_READ_BUF_SIZE - RingBuf_GetUnusedSize(&s_uart3ReadRingBuffer);
    //        s_uart3ReadBufferState.maxUsedCapacityOfBuffer =
    //            usedCapacityOfBuffer > s_uart3ReadBufferState.maxUsedCapacityOfBuffer ? usedCapacityOfBuffer
    //                                                                                  : s_uart3ReadBufferState.maxUsedCapacityOfBuffer;
    //        s_uart3ReadBufferState.countOfLostData += 1 - realCountPutBuffer;
    //    }

    //    if (__HAL_USART_GET_IT_SOURCE(&s_uart3Handle, USART_IT_TXE) != RESET &&
    //        __HAL_USART_GET_FLAG(&s_uart3Handle, USART_FLAG_TXE) != RESET) {
    //        if (RingBuf_Get(&s_uart3WriteRingBuffer, &data, 1)) {
    //            /* Transmit Data */
    //            s_uart3Handle.Instance->DR = ((uint16_t) data & (uint16_t) 0x01FF);
    //        } else {
    //            __HAL_USART_DISABLE_IT(&s_uart3Handle, USART_IT_TXE);
    //        }
    //    }
}

#endif
