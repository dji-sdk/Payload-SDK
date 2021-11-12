/**
 ********************************************************************
 * @file    util_time.c
 * @brief
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

/* Includes ------------------------------------------------------------------*/
#ifdef SYSTEM_ARCH_LINUX

#include "util_time.h"
#include <sys/resource.h>
#include <time.h>

/* Private constants ---------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private functions declaration ---------------------------------------------*/

/* Private values ------------------------------------------------------------*/

/* Exported functions definition ---------------------------------------------*/
T_DjiRunTimeStamps DjiUtilTime_GetRunTimeStamps(void)
{
    T_DjiRunTimeStamps timeStamps;
    struct rusage rusage;
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    timeStamps.realUsec = (uint64_t) ts.tv_sec * 1000000 + ts.tv_nsec / 1000;

    getrusage(RUSAGE_SELF, &rusage);
    timeStamps.userUsec =
        (rusage.ru_utime.tv_sec * 1000000LL) + rusage.ru_utime.tv_usec;
    timeStamps.sysUsec =
        (rusage.ru_stime.tv_sec * 1000000LL) + rusage.ru_stime.tv_usec;

    return timeStamps;
}

/* Private functions definition-----------------------------------------------*/

#endif
/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
