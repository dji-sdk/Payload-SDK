/**
 ********************************************************************
 * @file    osal.h
 * @version V2.0.0
 * @date    2019/8/28
 * @brief   This is the header file for "osal.c", defining the structure and
 * (exported) function prototypes.
 *
 * @copyright (c) 2018-2019 DJI. All rights reserved.
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
#ifndef OSAL_H
#define OSAL_H

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <unistd.h>
#include "psdk_typedef.h"

#include "psdk_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
T_PsdkReturnCode
Osal_TaskCreate(T_PsdkTaskHandle *task, void *(*taskFunc)(void *), const char *name, uint32_t stackSize, void *arg);
T_PsdkReturnCode Osal_TaskDestroy(T_PsdkTaskHandle task);
T_PsdkReturnCode Osal_TaskSleepMs(uint32_t timeMs);

T_PsdkReturnCode Osal_MutexCreate(T_PsdkMutexHandle *mutex);
T_PsdkReturnCode Osal_MutexDestroy(T_PsdkMutexHandle mutex);
T_PsdkReturnCode Osal_MutexLock(T_PsdkMutexHandle mutex);
T_PsdkReturnCode Osal_MutexUnlock(T_PsdkMutexHandle mutex);

T_PsdkReturnCode Osal_SemaphoreCreate(T_PsdkSemHandle *semaphore, uint32_t initValue);
T_PsdkReturnCode Osal_SemaphoreDestroy(T_PsdkSemHandle semaphore);
T_PsdkReturnCode Osal_SemaphoreWait(T_PsdkSemHandle semaphore);
T_PsdkReturnCode Osal_SemaphoreTimedWait(T_PsdkSemHandle semaphore, uint32_t waitTime);
T_PsdkReturnCode Osal_SemaphorePost(T_PsdkSemHandle semaphore);

T_PsdkReturnCode Osal_GetTimeMs(uint32_t *ms);
void *Osal_Malloc(uint32_t size);
void Osal_Free(void *ptr);

#ifdef __cplusplus
}
#endif

#endif // OSAL_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
