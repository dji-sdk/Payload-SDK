/**
 ********************************************************************
 * @file    psdk_util_time.c
 * @version V1.0.0
 * @date    2019/01/01
 * @brief
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


/* Includes ------------------------------------------------------------------*/
#include "util_time.h"
#include <sys/resource.h>
#include <time.h>

/* Private constants ---------------------------------------------------------*/


/* Private types -------------------------------------------------------------*/


/* Private functions declaration ---------------------------------------------*/


/* Private values ------------------------------------------------------------*/


/* Exported functions definition ---------------------------------------------*/
#if PSDK_ARCH_SYS_LINUX
T_PsdkRunTimeStamps PsdkUtilTime_GetRunTimeStamps(void)
{
    T_PsdkRunTimeStamps timeStamps;
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
#endif

/* Private functions definition-----------------------------------------------*/


/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
