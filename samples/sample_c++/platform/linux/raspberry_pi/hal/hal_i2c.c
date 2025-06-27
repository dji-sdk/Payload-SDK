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
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>

/* Private constants ---------------------------------------------------------*/
#define I2C_DEVICE_RESET_TIME_US       (25 * 1000)
#define I2C_DEVICE_RESET_GPIO_NUM      (4)
#define DJI_SYSTEM_CMD_STR_MAX_SIZE    (64)

/* Private types -------------------------------------------------------------*/
typedef struct {
    int32_t i2cFd;
} T_I2cHandleStruct;

/* Private values -------------------------------------------------------------*/

/* Private functions declaration ---------------------------------------------*/
static void HalI2c_ResetDevice(void);

/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode HalI2c_Init(T_DjiHalI2cConfig i2cConfig, T_DjiI2cHandle *i2cHandle)
{
    T_I2cHandleStruct *i2CHandleStruct = NULL;

    //attention: suggest reset the i2c device before init it.
    HalI2c_ResetDevice();

    i2CHandleStruct = malloc(sizeof(T_I2cHandleStruct));
    if (i2CHandleStruct == NULL) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_MEMORY_ALLOC_FAILED;
    }

    i2CHandleStruct->i2cFd = open(LINUX_I2C_DEV1, O_RDWR);
    if (i2CHandleStruct->i2cFd < 0) {
        printf("Open i2c device failed, fd: %d\r\n", i2CHandleStruct->i2cFd);
        return -1;
    }

    *i2cHandle = i2CHandleStruct;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode HalI2c_DeInit(T_DjiI2cHandle i2cHandle)
{
    T_I2cHandleStruct *i2CHandleStruct = (T_I2cHandleStruct *) i2cHandle;

    close(i2CHandleStruct->i2cFd);
    free(i2CHandleStruct);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode HalI2c_WriteData(T_DjiI2cHandle i2cHandle, uint16_t devAddress, const uint8_t *buf,
                                 uint32_t len, uint32_t *realLen)
{
    struct i2c_rdwr_ioctl_data data;
    struct i2c_msg messages;
    int32_t ret = 0;
    T_I2cHandleStruct *i2CHandleStruct = (T_I2cHandleStruct *) i2cHandle;

    messages.addr = devAddress;
    messages.flags = 0;
    messages.len = len;
    messages.buf = (uint8_t *) buf;

    data.msgs = &messages;
    data.nmsgs = 1;

    ret = ioctl(i2CHandleStruct->i2cFd, I2C_RDWR, &data);
    if (ret < 0) {
        *realLen = 0;
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    *realLen = ret;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode HalI2c_ReadData(T_DjiI2cHandle i2cHandle, uint16_t devAddress, uint8_t *buf,
                                uint32_t len, uint32_t *realLen)
{
    struct i2c_rdwr_ioctl_data data;
    struct i2c_msg messages;
    int32_t ret = 0;
    T_I2cHandleStruct *i2CHandleStruct = (T_I2cHandleStruct *) i2cHandle;

    messages.addr = devAddress;
    messages.flags = I2C_M_RD;
    messages.len = len;
    messages.buf = buf;

    data.msgs = &messages;
    data.nmsgs = 1;

    ret = ioctl(i2CHandleStruct->i2cFd, I2C_RDWR, &data);
    if (ret < 0) {
        *realLen = 0;
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    *realLen = ret;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/* Private functions definition-----------------------------------------------*/
static void HalI2c_ResetDevice(void)
{
    char systemCmd[DJI_SYSTEM_CMD_STR_MAX_SIZE] = {0};
    int32_t ret;

    sprintf(systemCmd, "echo %d > /sys/class/gpio/export", I2C_DEVICE_RESET_GPIO_NUM);
    ret = system(systemCmd);
    if (ret != 0) {
        printf("Export reset gpio failed, %d\r\n", ret);
    }

    sprintf(systemCmd, "echo out > /sys/class/gpio/gpio4/direction");
    ret = system(systemCmd);
    if (ret != 0) {
        printf("Set gpio direction failed, %d\r\n", ret);
    }

    sprintf(systemCmd, "echo 0 > /sys/class/gpio/gpio4/value");
    ret = system(systemCmd);
    if (ret != 0) {
        printf("Set gpio value failed, %d\r\n", ret);
    }

    usleep(I2C_DEVICE_RESET_TIME_US);

    sprintf(systemCmd, "echo 1 > /sys/class/gpio/gpio4/value");
    ret = system(systemCmd);
    if (ret != 0) {
        printf("Set gpio value failed, %d\r\n", ret);
    }

    sprintf(systemCmd, "echo %d > /sys/class/gpio/unexport", I2C_DEVICE_RESET_GPIO_NUM);
    ret = system(systemCmd);
    if (ret != 0) {
        printf("Unexport reset gpio failed, %d\r\n", ret);
    }
}

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
