/**
 ********************************************************************
 * @file    dji_interest_point.h
 * @version V1.0.0
 * @date    2020/11/19
 * @brief   This is the header file for "dji_interest_point.c", defining the structure and
 * (exported) function prototypes.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef DJI_INTEREST_POINT_H
#define DJI_INTEREST_POINT_H

/* Includes ------------------------------------------------------------------*/
#include "dji_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef struct {
    dji_f32_t curSpeed;
    dji_f32_t radius;
    uint8_t state;
} T_DjiInterestPointMissionState;

typedef struct {
    dji_f64_t latitude;
    dji_f64_t longitude;
    dji_f32_t speed;
} T_DjiInterestPointSettings;

typedef T_DjiReturnCode (*InterestPointMissionStateCallback)(T_DjiInterestPointMissionState missionState);

/* Exported functions --------------------------------------------------------*/
T_DjiReturnCode DjiInterestPoint_Init(void);

T_DjiReturnCode DjiInterestPoint_DeInit(void);

T_DjiReturnCode DjiInterestPoint_Start(T_DjiInterestPointSettings settings);

T_DjiReturnCode DjiInterestPoint_Stop(void);

T_DjiReturnCode DjiInterestPoint_SetSpeed(dji_f32_t speed);

T_DjiReturnCode DjiInterestPoint_RegMissionStateCallback(InterestPointMissionStateCallback callback);

#ifdef __cplusplus
}
#endif

#endif // DJI_INTEREST_POINT_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
