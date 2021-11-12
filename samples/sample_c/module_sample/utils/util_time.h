/**
 ********************************************************************
 * @file    util_time.h
 * @brief   This is the header file for "util_time.c", defining the structure and
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
#ifndef DJI_UTIL_TIME_H
#define DJI_UTIL_TIME_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SYSTEM_ARCH_LINUX

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported constants --------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/
typedef struct {
    uint64_t realUsec;
    uint64_t userUsec;
    uint64_t sysUsec;
} T_DjiRunTimeStamps;

/* Exported functions --------------------------------------------------------*/
T_DjiRunTimeStamps DjiUtilTime_GetRunTimeStamps(void);

#ifdef __cplusplus
}
#endif

#endif

#endif // DJI_DP_UTILS_H

/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
