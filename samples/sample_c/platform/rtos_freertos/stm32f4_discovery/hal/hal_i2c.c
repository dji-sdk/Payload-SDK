/**
 ********************************************************************
 * @file    hal_i2c.c
 * @brief
 *
 * @copyright (c) 2018 DJI. All rights reserved.
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
#include "hal_i2c.h"
#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "osal.h"

/* Private constants ---------------------------------------------------------*/
#define HAL_I2C_MASTER_COMM_TIMEOUT_MS     (50)
#define I2C_DEVICE_RESET_TIME_MS           (50)
#define I2C_DEVICE_WAIT_RESET_TIME_MS      (100)

#define RST_GPIO_PIN                      GPIO_PIN_5
#define RST_GPIO_PORT                     GPIOB
#define RST_GPIO_CLK_ENABLE()             __HAL_RCC_GPIOB_CLK_ENABLE()

#define I2C_DEVICE_CLK()                  __HAL_RCC_I2C1_CLK_ENABLE()
#define I2C_DEVICE_NUM                    I2C1

#define I2C_SCL_GPIO_PIN                  GPIO_PIN_6
#define I2C_SDA_GPIO_PIN                  GPIO_PIN_7
#define I2C_GPIO_PORT                     GPIOB
#define I2C_GPIO_AF                       GPIO_AF4_I2C1
#define I2C_GPIO_CLK_ENABLE()             __HAL_RCC_GPIOB_CLK_ENABLE()

/* Private types -------------------------------------------------------------*/
typedef struct {
    I2C_HandleTypeDef i2CHandle;
} T_I2cHandleStruct;

/* Private values -------------------------------------------------------------*/

/* Private functions declaration ---------------------------------------------*/

/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode HalI2c_Init(T_DjiHalI2cConfig i2cConfig, T_DjiI2cHandle *i2cHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    T_I2cHandleStruct *i2CHandleStruct;

    i2CHandleStruct = pvPortMalloc(sizeof(T_I2cHandleStruct));
    if (i2CHandleStruct == NULL) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_MEMORY_ALLOC_FAILED;
    }

    /* I2C1 GPIO Configuration
      PB6 ------> I2C1_SCL
      PB7 ------> I2C1_SDA
      PB5 ------> RST      */

    RST_GPIO_CLK_ENABLE();
    I2C_GPIO_CLK_ENABLE();

    GPIO_InitStruct.Pin = RST_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(RST_GPIO_PORT, &GPIO_InitStruct);

    HAL_GPIO_WritePin(RST_GPIO_PORT, RST_GPIO_PIN, 0);
    Osal_TaskSleepMs(I2C_DEVICE_RESET_TIME_MS);
    HAL_GPIO_WritePin(RST_GPIO_PORT, RST_GPIO_PIN, 1);
    Osal_TaskSleepMs(I2C_DEVICE_WAIT_RESET_TIME_MS);

    GPIO_InitStruct.Pin = I2C_SCL_GPIO_PIN | I2C_SDA_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = I2C_GPIO_AF;
    HAL_GPIO_Init(I2C_GPIO_PORT, &GPIO_InitStruct);

    /* Peripheral clock enable */
    I2C_DEVICE_CLK();

    i2CHandleStruct->i2CHandle.Instance = I2C_DEVICE_NUM;
    i2CHandleStruct->i2CHandle.Init.ClockSpeed = i2cConfig.i2cSpeed;
    i2CHandleStruct->i2CHandle.Init.DutyCycle = I2C_DUTYCYCLE_2;
    i2CHandleStruct->i2CHandle.Init.OwnAddress1 = (i2cConfig.devAddress << 1);
    i2CHandleStruct->i2CHandle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    i2CHandleStruct->i2CHandle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    i2CHandleStruct->i2CHandle.Init.OwnAddress2 = 0xFF;
    i2CHandleStruct->i2CHandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    i2CHandleStruct->i2CHandle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&i2CHandleStruct->i2CHandle) != HAL_OK) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    *i2cHandle = i2CHandleStruct;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode HalI2c_DeInit(T_DjiI2cHandle i2cHandle)
{
    T_I2cHandleStruct *i2CHandleStruct = (T_I2cHandleStruct *) i2cHandle;

    if (HAL_I2C_DeInit(&i2CHandleStruct->i2CHandle) != HAL_OK) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    vPortFree(i2cHandle);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode HalI2c_WriteData(T_DjiI2cHandle i2cHandle, uint16_t devAddress, const uint8_t *buf,
                                 uint32_t len, uint32_t *realLen)
{
    HAL_StatusTypeDef status;
    T_I2cHandleStruct *i2CHandleStruct = (T_I2cHandleStruct *) i2cHandle;

    status = HAL_I2C_Master_Transmit(&i2CHandleStruct->i2CHandle, (devAddress << 1), (uint8_t *) buf, len,
                                     HAL_I2C_MASTER_COMM_TIMEOUT_MS);
    if (status != HAL_OK) {
        *realLen = 0;
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    *realLen = len;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode HalI2c_ReadData(T_DjiI2cHandle i2cHandle, uint16_t devAddress, uint8_t *buf,
                                uint32_t len, uint32_t *realLen)
{
    HAL_StatusTypeDef status;
    T_I2cHandleStruct *i2CHandleStruct = (T_I2cHandleStruct *) i2cHandle;

    status = HAL_I2C_Master_Receive(&i2CHandleStruct->i2CHandle, (devAddress << 1), buf, len,
                                    HAL_I2C_MASTER_COMM_TIMEOUT_MS);
    if (status != HAL_OK) {
        *realLen = 0;
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    *realLen = len;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/* Private functions definition-----------------------------------------------*/

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
