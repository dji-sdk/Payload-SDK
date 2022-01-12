/**
 ********************************************************************
 * @file    psdk_util_time.h
 * @version V0.0.0
 * @date    2019/01/01
 * @brief   This is the header file for "psdk_util_time.c", defining the structure and
 * (exported) function prototypes.
 *
 * @copyright (c) 2017-2018 DJI. All rights reserved.
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
#ifndef PSDK_UTIL_TIME_H
#define PSDK_UTIL_TIME_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported constants --------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/
typedef struct {
    uint64_t realUsec;
    uint64_t userUsec;
    uint64_t sysUsec;
} T_PsdkRunTimeStamps;

/* Exported functions --------------------------------------------------------*/
#if PSDK_ARCH_SYS_LINUX
T_PsdkRunTimeStamps PsdkUtilTime_GetRunTimeStamps(void);
#endif

#ifdef __cplusplus
}
#endif

#endif // PSDK_DP_UTILS_H

/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
