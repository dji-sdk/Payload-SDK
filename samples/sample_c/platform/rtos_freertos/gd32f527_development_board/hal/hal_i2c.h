/**
 ********************************************************************
 * @file    hal_i2c.h
 * @brief   This is the header file for "hal_i2c.c", defining the structure and
 * (exported) function prototypes.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef HAL_I2C_H
#define HAL_I2C_H

/* Includes ------------------------------------------------------------------*/
#include "dji_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
T_DjiReturnCode HalI2c_Init(T_DjiHalI2cConfig i2cConfig, T_DjiI2cHandle *i2cHandle);
T_DjiReturnCode HalI2c_DeInit(T_DjiI2cHandle i2cHandle);
T_DjiReturnCode HalI2c_WriteData(T_DjiI2cHandle i2cHandle, uint16_t devAddress,
                                 const uint8_t *buf, uint32_t len, uint32_t *realLen);
T_DjiReturnCode HalI2c_ReadData(T_DjiI2cHandle i2cHandle, uint16_t devAddress,
                                uint8_t *buf, uint32_t len, uint32_t *realLen);

#ifdef __cplusplus
}
#endif

#endif // HAL_I2C_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
