/**
 ********************************************************************
 * @file    osal.c
 * @version V2.0.0
 * @date    2019/08/30
 * @brief
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

/* Includes ------------------------------------------------------------------*/
#include "limits.h"
#include "osal/osal.h"
#include "utils/util_misc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Private constants ---------------------------------------------------------*/
#define SEM_MUTEX_WAIT_FOREVER      0xFFFFFFFF
#define TASK_PRIORITY_NORMAL        0

/* Private types -------------------------------------------------------------*/

/* Private functions declaration ---------------------------------------------*/

/* Exported functions definition ---------------------------------------------*/

/* Private functions definition-----------------------------------------------*/
T_PsdkReturnCode
Osal_TaskCreate(T_PsdkTaskHandle *task, void *(*taskFunc)(void *), const char *name, uint32_t stackSize, void *arg)
{
    uint32_t stackDepth;
    char nameDealed[16] = {0};

    //attention :  freertos use stack depth param, stack size = (stack depth) * sizeof(StackType_t)
    if (stackSize % sizeof(StackType_t) == 0) {
        stackDepth = stackSize / sizeof(StackType_t);
    } else {
        stackDepth = stackSize / sizeof(StackType_t) + 1;
    }

    if (name != NULL)
        strncpy(nameDealed, name, sizeof(nameDealed) - 1);
    if (xTaskCreate((TaskFunction_t) taskFunc, nameDealed, stackDepth, arg, TASK_PRIORITY_NORMAL, task) != pdPASS) {
        *task = NULL;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_PsdkReturnCode Osal_TaskDestroy(T_PsdkTaskHandle task)
{
    vTaskDelete(task);

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_PsdkReturnCode Osal_TaskSleepMs(uint32_t timeMs)
{
    TickType_t ticks;

    ticks = timeMs / portTICK_PERIOD_MS;

    /* Minimum delay = 1 tick */
    vTaskDelay(ticks ? ticks : 1);

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_PsdkReturnCode Osal_MutexCreate(T_PsdkMutexHandle *mutex)
{
    *mutex = xSemaphoreCreateMutex();
    if (*mutex == NULL) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_PsdkReturnCode Osal_MutexDestroy(T_PsdkMutexHandle mutex)
{
    vQueueDelete((SemaphoreHandle_t) mutex);

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_PsdkReturnCode Osal_MutexLock(T_PsdkMutexHandle mutex)
{
    TickType_t ticks;

    if (mutex == NULL) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    ticks = portMAX_DELAY;

    if (xSemaphoreTake(mutex, ticks) != pdTRUE) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_PsdkReturnCode Osal_MutexUnlock(T_PsdkMutexHandle mutex)
{
    if (xSemaphoreGive(mutex) != pdTRUE) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_PsdkReturnCode Osal_SemaphoreCreate(T_PsdkSemHandle *semaphore, uint32_t initValue)
{
    uint32_t maxCount = UINT_MAX;

    *semaphore = xSemaphoreCreateCounting(maxCount, initValue);

    if (*semaphore == NULL) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_PsdkReturnCode Osal_SemaphoreDestroy(T_PsdkSemHandle semaphore)
{
    vSemaphoreDelete(semaphore);

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_PsdkReturnCode Osal_SemaphoreWait(T_PsdkSemHandle semaphore)
{
    Osal_SemaphoreTimedWait(semaphore, SEM_MUTEX_WAIT_FOREVER);

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_PsdkReturnCode Osal_SemaphoreTimedWait(T_PsdkSemHandle semaphore, uint32_t waitTimeMs)
{
    TickType_t ticks;

    if (semaphore == NULL) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    ticks = 0;
    if (waitTimeMs == SEM_MUTEX_WAIT_FOREVER) {
        ticks = portMAX_DELAY;
    } else if (waitTimeMs != 0) {
        ticks = waitTimeMs / portTICK_PERIOD_MS;
        if (ticks == 0) {
            ticks = 1;
        }
    }
    if (xSemaphoreTake(semaphore, ticks) != pdTRUE) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_PsdkReturnCode Osal_SemaphorePost(T_PsdkSemHandle semaphore)
{
    if (xSemaphoreGive(semaphore) != pdTRUE) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_PsdkReturnCode Osal_GetTimeMs(uint32_t *ms)
{
    *ms = xTaskGetTickCount();

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

void *Osal_Malloc(uint32_t size)
{
    return pvPortMalloc(size);
}

void Osal_Free(void *ptr)
{
    vPortFree(ptr);
}

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
