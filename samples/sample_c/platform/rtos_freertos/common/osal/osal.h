/**
 ********************************************************************
 * @file    osal.h
 * @brief   This is the header file for "osal.c", defining the structure and
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
#ifndef OSAL_H
#define OSAL_H

/* Includes ------------------------------------------------------------------*/
#include "dji_platform.h"
#include "dji_typedef.h"


#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
T_DjiReturnCode Osal_TaskCreate(const char *name, void *(*taskFunc)(void *), uint32_t stackSize,
                                void *arg, T_DjiTaskHandle *task);
T_DjiReturnCode Osal_TaskDestroy(T_DjiTaskHandle task);
T_DjiReturnCode Osal_TaskSleepMs(uint32_t timeMs);
T_DjiReturnCode Osal_MutexCreate(T_DjiMutexHandle *mutex);
T_DjiReturnCode Osal_MutexDestroy(T_DjiMutexHandle mutex);
T_DjiReturnCode Osal_MutexLock(T_DjiMutexHandle mutex);
T_DjiReturnCode Osal_MutexUnlock(T_DjiMutexHandle mutex);
T_DjiReturnCode Osal_SemaphoreCreate(uint32_t initValue, T_DjiSemaHandle *semaphore);
T_DjiReturnCode Osal_SemaphoreDestroy(T_DjiSemaHandle semaphore);
T_DjiReturnCode Osal_SemaphoreTimedWait(T_DjiSemaHandle semaphore, uint32_t waitTimeMs);
T_DjiReturnCode Osal_SemaphoreWait(T_DjiSemaHandle semaphore);
T_DjiReturnCode Osal_SemaphorePost(T_DjiSemaHandle semaphore);
T_DjiReturnCode Osal_GetTimeMs(uint32_t *ms);
T_DjiReturnCode Osal_GetTimeUs(uint64_t *us);
T_DjiReturnCode Osal_GetRandomNum(uint16_t *randomNum);
void *Osal_Malloc(uint32_t size);
void Osal_Free(void *ptr);

#ifdef __cplusplus
}
#endif

#endif // OSAL_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
