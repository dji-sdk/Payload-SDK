/**
 ********************************************************************
 * @file    hal_i2c.c
 * @brief
 *
 * @copyright (c) 2025 DJI. All rights reserved.
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
#include "dji_platform.h"
#include "dji_logger.h"
#include "hal_i2c.h"
#include "gd32f5xx.h"

/* Private constants ---------------------------------------------------------*/
#define HAL_I2C_MASTER_COMM_TIMEOUT_MS     (50)
#define I2C_DEVICE_RESET_TIME_MS           (50)
#define I2C_DEVICE_WAIT_RESET_TIME_MS      (100)

#define RST_GPIO_PIN                      GPIO_PIN_5
#define RST_GPIO_PORT                     GPIOA
#define RST_GPIO_CLK_ENABLE()             rcu_periph_clock_enable(GPIOA)

#define I2C_DEVICE_CLK_ENABLE()           rcu_periph_clock_enable(RCU_I2C2)
#define I2C_DEVICE_NUM                    I2C2

#define I2C_SCL_GPIO_PIN                  GPIO_PIN_8
#define I2C_SCL_GPIO_PORT                 GPIOA
#define I2C_SCL_GPIO_AF                   GPIO_AF_4
#define I2C_SDA_GPIO_PIN                  GPIO_PIN_9
#define I2C_SDA_GPIO_PORT                 GPIOC
#define I2C_SDA_GPIO_AF                   GPIO_AF_4

#define I2C_GPIO_CLK_ENABLE() \
    do { \
        rcu_periph_clock_enable(I2C_SCL_GPIO_PORT); \
        rcu_periph_clock_enable(I2C_SDA_GPIO_PORT); \
    } while(0)

#define USER_UTIL_UNUSED(x)               ((x) = (x))

/* Private types -------------------------------------------------------------*/
#define I2C_TIME_OUT    (uint16_t)(100000)
#define I2C_OK          1
#define I2C_FAIL        0
#define I2C_END         1

typedef enum {
    I2C_START = 0,
    I2C_SEND_ADDRESS,
    I2C_CLEAR_ADDRESS_FLAG,
    I2C_TRANSMIT_DATA,
    I2C_STOP
} i2c_process_enum;


/* Private values -------------------------------------------------------------*/

/* Private functions declaration ---------------------------------------------*/
static T_DjiReturnCode GD32_I2C_Master_Transmit(uint32_t i2cPerph, uint16_t devAddress, const uint8_t *dataBuf, uint32_t len, uint32_t *realLen, const uint32_t timeout);
static T_DjiReturnCode GD32_I2C_Master_Receive(uint32_t i2cPerph, uint16_t devAddress, uint8_t *dataBuf, uint32_t size, const uint32_t timeout);

/* Exported functions definition ---------------------------------------------*/

void i2c_bus_reset(void)
{
    i2c_deinit(I2C_DEVICE_NUM);

    /* configure SDA/SCL for GPIO */
    gpio_bit_reset(I2C_SCL_GPIO_PORT, I2C_SCL_GPIO_PIN);
    gpio_bit_reset(I2C_SDA_GPIO_PORT, I2C_SDA_GPIO_PIN);

    gpio_output_options_set(I2C_SCL_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, I2C_SCL_GPIO_PIN);
    gpio_output_options_set(I2C_SDA_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, I2C_SDA_GPIO_PIN);
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    gpio_bit_set(I2C_SCL_GPIO_PORT, I2C_SCL_GPIO_PIN);
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    gpio_bit_set(I2C_SDA_GPIO_PORT, I2C_SDA_GPIO_PIN);
    /* connect I2C_SCL_PIN to I2C_SCL */
    /* connect I2C_SDA_PIN to I2C_SDA */
    gpio_output_options_set(I2C_SCL_GPIO_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, I2C_SCL_GPIO_PIN);
    gpio_output_options_set(I2C_SDA_GPIO_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, I2C_SDA_GPIO_PIN);
    /* configure the I2CX interface */
    /* enable I2C clock */
    I2C_DEVICE_CLK_ENABLE();
    /* configure I2C clock */
    i2c_clock_config(I2C_DEVICE_NUM, 100000, I2C_DTCY_2);
    i2c_stretch_scl_low_config(I2C_DEVICE_NUM, 1);  // disable stretching
    /* configure I2C address */
    i2c_mode_addr_config(I2C_DEVICE_NUM, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, (0x2A) << 1);
    /* enable I2C_DEVICE_NUM */
    i2c_enable(I2C_DEVICE_NUM);
    /* enable acknowledge */
    i2c_ack_config(I2C_DEVICE_NUM, I2C_ACK_ENABLE);
}

T_DjiReturnCode HalI2c_Init(T_DjiHalI2cConfig i2cConfig, T_DjiI2cHandle *i2cHandle)
{
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    // uint8_t disableStanby[] = {0x80, 0xF7, 0x0C, 0x01, 0x01, 0x01};
    // uint32_t realLen = 0;
    /**
     * I2C2 GPIO Configuration
     * PA8 ------> I2C2_SCL
     * PC9 ------> I2C2_SDA
     * PA5 ------> RST
     */

    RST_GPIO_CLK_ENABLE();
    gpio_mode_set(RST_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, RST_GPIO_PIN);
    gpio_output_options_set(RST_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, RST_GPIO_PIN);

    /* Reset the I2C chip. */
    gpio_bit_reset(RST_GPIO_PORT, RST_GPIO_PIN);
    osalHandler->TaskSleepMs(I2C_DEVICE_RESET_TIME_MS);
    gpio_bit_set(RST_GPIO_PORT, RST_GPIO_PIN);
    osalHandler->TaskSleepMs(I2C_DEVICE_RESET_TIME_MS);

    /* enable I2C GPIO clock */
    I2C_GPIO_CLK_ENABLE();

    /* connect I2C_SCL_PIN to I2C_SCL */
    gpio_af_set(I2C_SCL_GPIO_PORT, I2C_SCL_GPIO_AF, I2C_SCL_GPIO_PIN);
    /* connect I2C_SDA_PIN to I2C_SDA */
    gpio_af_set(I2C_SDA_GPIO_PORT, I2C_SDA_GPIO_AF, I2C_SDA_GPIO_PIN);

    /* configure GPIO pins of I2C */
    gpio_mode_set(I2C_SCL_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, I2C_SCL_GPIO_PIN);
    gpio_output_options_set(I2C_SCL_GPIO_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, I2C_SCL_GPIO_PIN);
    gpio_mode_set(I2C_SDA_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, I2C_SDA_GPIO_PIN);
    gpio_output_options_set(I2C_SDA_GPIO_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, I2C_SDA_GPIO_PIN);

    i2c_deinit(I2C_DEVICE_NUM);

    /* enable I2C clock */
    I2C_DEVICE_CLK_ENABLE();
    /* configure I2C clock */
    i2c_clock_config(I2C_DEVICE_NUM, i2cConfig.i2cSpeed, I2C_DTCY_2);
    i2c_stretch_scl_low_config(I2C_DEVICE_NUM, 1);  // disable stretching
    /* configure I2C address */
    i2c_mode_addr_config(I2C_DEVICE_NUM, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, i2cConfig.devAddress << 1);
    /* enable I2C_DEVICE_NUM */
    i2c_enable(I2C_DEVICE_NUM);
    /* enable acknowledge */
    i2c_ack_config(I2C_DEVICE_NUM, I2C_ACK_ENABLE);

    // GD32_I2C_Master_Transmit(I2C_DEVICE_NUM, i2cConfig.devAddress, (const uint8_t *)disableStanby, sizeof(disableStanby), &realLen, I2C_TIME_OUT);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode HalI2c_DeInit(T_DjiI2cHandle i2cHandle)
{
    USER_UTIL_UNUSED(i2cHandle);
    i2c_deinit(I2C_DEVICE_NUM);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode HalI2c_WriteData(T_DjiI2cHandle i2cHandle, uint16_t devAddress, const uint8_t *buf,
                                 uint32_t len, uint32_t *realLen)
{
    T_DjiReturnCode returnCode = 0;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    char strBuf[128] = {0};
    int32_t charCnt = 0;
    int32_t charIdx = 0;

    for (int32_t i = 0; i < len; i++) {
        charCnt = sprintf(&strBuf[charIdx], " %02x", buf[i]);
        charIdx += charCnt;
    }

    returnCode = GD32_I2C_Master_Transmit(I2C_DEVICE_NUM, devAddress, buf, len, realLen, I2C_TIME_OUT);

    return returnCode;
}

T_DjiReturnCode HalI2c_ReadData(T_DjiI2cHandle i2cHandle, uint16_t devAddress, uint8_t *buf,
                                uint32_t len, uint32_t *realLen)
{
    T_DjiReturnCode returnCode;
    char strBuf[128] = {0};
    int32_t charCnt = 0;
    int32_t charIdx = 0;

    returnCode = GD32_I2C_Master_Receive(I2C_DEVICE_NUM, devAddress, buf, len, I2C_TIME_OUT);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        *realLen = 0;
    } else {
        *realLen = len;
    }

    if (returnCode == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        for (int32_t i = 0; (i < *realLen) && (i < 30); i++) {
            charCnt = sprintf(&strBuf[charIdx], " %02x", buf[i]);
            charIdx += charCnt;
        }

        strBuf[30] = 0;

    }

    return returnCode;
}



/* Private functions definition-----------------------------------------------*/

static T_DjiReturnCode GD32_I2C_Master_Transmit(uint32_t i2cPerph, uint16_t devAddress, const uint8_t *dataBuf, uint32_t len, uint32_t *realLen, const uint32_t timeout)
{
    uint8_t state = I2C_START;
    uint16_t timeCnt = 0;
    uint8_t i2c_timeout_flag = 0;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    *realLen = 0;

    while(!(i2c_timeout_flag)) {
        switch(state) {
            case I2C_START: {
                /* i2c master sends start signal only when the bus is idle */
                timeCnt = 0;
                while(i2c_flag_get(i2cPerph, I2C_FLAG_I2CBSY) && (timeCnt < timeout)) {
                    timeCnt++;
                }
                if(timeCnt < timeout) {
                    i2c_start_on_bus(i2cPerph);
                    timeCnt = 0;
                    state = I2C_SEND_ADDRESS;
                } else {
                    timeCnt = 0;
                    state = I2C_START;
                    USER_LOG_INFO("i2c bus is busy in WRITE!\r\n");
                    while (1) {
                        osalHandler->TaskSleepMs(8000);
                    }
                }
                break;
            }
            case I2C_SEND_ADDRESS: {
                /* i2c master sends START signal successfully */
                while((!i2c_flag_get(i2cPerph, I2C_FLAG_SBSEND)) && (timeCnt < timeout)) {
                    timeCnt++;
                }
                if(timeCnt < timeout) {
                    i2c_master_addressing(i2cPerph, devAddress << 1, I2C_TRANSMITTER);
                    timeCnt = 0;
                    state = I2C_CLEAR_ADDRESS_FLAG;
                } else {
                    timeCnt = 0;
                    state = I2C_START;
                }
                break;
            }
            case I2C_CLEAR_ADDRESS_FLAG: {
                /* address flag set means i2c slave sends ACK */
                timeCnt = 0;
                while((!i2c_flag_get(i2cPerph, I2C_FLAG_ADDSEND)) && (timeCnt < timeout)) {
                    timeCnt++;
                }
                if(timeCnt < timeout) {
                    i2c_flag_clear(i2cPerph, I2C_FLAG_ADDSEND);
                    timeCnt = 0;
                    state = I2C_TRANSMIT_DATA;
                } else {
                    timeCnt = 0;
                    state = I2C_START;
                }
                break;
            }
            case I2C_TRANSMIT_DATA: {
                /* wait until the transmit data buffer is empty */
                while((!i2c_flag_get(i2cPerph, I2C_FLAG_TBE)) && (timeCnt < timeout)) {
                    timeCnt++;
                }
                if(timeCnt < timeout) {
                    timeCnt = 0;
                } else {
                    timeCnt = 0;
                    state = I2C_START;
                    break;
                }

                *realLen = 0;
                while(len--) {
                    i2c_data_transmit(i2cPerph, *dataBuf);
                    /* point to the next byte to be written */
                    dataBuf++;
                    *realLen += 1;
                    /* wait until BTC bit is set */
                    while((!i2c_flag_get(i2cPerph, I2C_FLAG_BTC)) && (timeCnt < timeout)) {
                        timeCnt++;
                    }
                    if(timeCnt < timeout) {
                        timeCnt = 0;
                    } else {
                        timeCnt = 0;
                        state = I2C_START;
                        break;
                    }
                }
                timeCnt = 0;
                state = I2C_STOP;
                break;
            }
            case I2C_STOP: {
                /* send a stop condition to I2C bus */
                i2c_stop_on_bus(i2cPerph);
                /* i2c master sends STOP signal successfully */
                while((I2C_CTL0(i2cPerph) & I2C_CTL0_STOP) && (timeCnt < timeout)) {
                    timeCnt++;
                }
                if(timeCnt < timeout) {
                    timeCnt = 0;
                    state = I2C_END;
                    i2c_timeout_flag = I2C_OK;
                } else {
                    timeCnt = 0;
                    state = I2C_START;
                }
                break;
            }
            default: {
                state = I2C_START;
                i2c_timeout_flag = I2C_OK;
                timeCnt = 0;
                break;
            }
        }
    }
    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode GD32_I2C_Master_Receive(uint32_t i2cPerph, uint16_t devAddress, uint8_t *dataBuf, uint32_t size, uint32_t timeout)
{
    uint8_t state = I2C_START;
    uint8_t read_cycle = 0;
    uint16_t timeCnt = 0;
    uint8_t i2c_timeout_flag = 0;

    /* enable acknowledge */
    i2c_ack_config(i2cPerph, I2C_ACK_ENABLE);
    while(!(i2c_timeout_flag)) {
        switch(state) {
            case I2C_START: {
                if(RESET == read_cycle) {
                    /* i2c master sends start signal only when the bus is idle */
                    while(i2c_flag_get(i2cPerph, I2C_FLAG_I2CBSY) && (timeCnt < timeout)) {
                        timeCnt++;
                    }
                    if(timeCnt < timeout) {
                        /* whether to send ACK or not for the next byte */
                        if(2 == size) {
                            i2c_ackpos_config(i2cPerph, I2C_ACKPOS_NEXT);
                        }
                    } else {
                        // i2c_bus_reset();
                        timeCnt = 0;
                        state = I2C_START;
                    }
                }
                /* send the start signal */
                i2c_start_on_bus(i2cPerph);
                timeCnt = 0;
                state = I2C_SEND_ADDRESS;
                break;
            }
            case I2C_SEND_ADDRESS: {
                /* i2c master sends START signal successfully */
                while((!i2c_flag_get(i2cPerph, I2C_FLAG_SBSEND)) && (timeCnt < timeout)) {
                    timeCnt++;
                }
                if(timeCnt < timeout) {
                        i2c_master_addressing(i2cPerph, devAddress << 1, I2C_RECEIVER);
                        if(size < 3) {
                            /* disable acknowledge */
                            i2c_ack_config(i2cPerph, I2C_ACK_DISABLE);
                        }
                        state = I2C_CLEAR_ADDRESS_FLAG;
                    // }
                    timeCnt = 0;
                } else {
                    timeCnt = 0;
                    state = I2C_START;
                    read_cycle = 0;
                }
                break;
            }
            case I2C_CLEAR_ADDRESS_FLAG: {
                /* address flag set means i2c slave sends ACK */
                while((!i2c_flag_get(i2cPerph, I2C_FLAG_ADDSEND)) && (timeCnt < timeout)) {
                    timeCnt++;
                }
                if(timeCnt < timeout) {
                    i2c_flag_clear(i2cPerph, I2C_FLAG_ADDSEND);
                    if((SET == read_cycle) && (1 == size)) {
                        /* send a stop condition to I2C bus */
                        i2c_stop_on_bus(i2cPerph);
                    }
                    timeCnt = 0;
                    state = I2C_TRANSMIT_DATA;
                } else {
                    timeCnt = 0;
                    state = I2C_START;
                    read_cycle = 0;
                }
                break;
            }
            case I2C_TRANSMIT_DATA: {
                    timeCnt = 0;
                    while(size) {
                        timeCnt++;
                        if(3 == size) {
                            /* wait until BTC bit is set */
                            while(!i2c_flag_get(i2cPerph, I2C_FLAG_BTC));
                            /* disable acknowledge */
                            i2c_ack_config(i2cPerph, I2C_ACK_DISABLE);
                        }
                        if(2 == size) {
                            /* wait until BTC bit is set */
                            while(!i2c_flag_get(i2cPerph, I2C_FLAG_BTC));
                            /* send a stop condition to I2C bus */
                            i2c_stop_on_bus(i2cPerph);
                        }
                        /* wait until RBNE bit is set */
                        if(i2c_flag_get(i2cPerph, I2C_FLAG_RBNE)) {
                            /* read a byte from the EEPROM */
                            *dataBuf = i2c_data_receive(i2cPerph);
                            /* point to the next location where the byte read will be saved */
                            dataBuf++;
                            /* decrement the read bytes counter */
                            size--;
                            timeCnt = 0;
                        }
                        if(timeCnt > timeout) {
                            timeCnt = 0;
                            state = I2C_START;
                            read_cycle = 0;
                        }
                    }
                    timeCnt = 0;
                    state = I2C_STOP;
                break;
            }
            case I2C_STOP: {
                /* i2c master sends STOP signal successfully */
                while((I2C_CTL0(i2cPerph) & I2C_CTL0_STOP) && (timeCnt < timeout)) {
                    timeCnt++;
                }
                if(timeCnt < timeout) {
                    timeCnt = 0;
                    state = I2C_END;
                    i2c_timeout_flag = I2C_OK;
                } else {
                    timeCnt = 0;
                    state = I2C_START;
                    read_cycle = 0;
                }
                break;
            }
            default: {
                state = I2C_START;
                read_cycle = 0;
                i2c_timeout_flag = I2C_OK;
                timeCnt = 0;
                break;
            }
        }
    }
    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
