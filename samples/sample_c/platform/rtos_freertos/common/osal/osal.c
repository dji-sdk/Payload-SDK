/**
 ********************************************************************
 * @file    osal.c
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
#include "limits.h"
#include "osal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "stdlib.h"

/* Private constants ---------------------------------------------------------*/
#define SEM_MUTEX_WAIT_FOREVER      0xFFFFFFFF
#define TASK_PRIORITY_NORMAL        0

/* Private types -------------------------------------------------------------*/

/* Private functions declaration ---------------------------------------------*/

/* Exported functions definition ---------------------------------------------*/

/* Private functions definition-----------------------------------------------*/
T_DjiReturnCode Osal_TaskCreate(const char *name, void *(*taskFunc)(void *), uint32_t stackSize,
                                void *arg, T_DjiTaskHandle *task)
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
    if (xTaskCreate((TaskFunction_t) taskFunc, nameDealed, stackDepth, arg, TASK_PRIORITY_NORMAL, (TaskHandle_t *)task) != pdPASS) {
        *task = NULL;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode Osal_TaskDestroy(T_DjiTaskHandle task)
{
    if (task == NULL) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    vTaskDelete(task);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode Osal_TaskSleepMs(uint32_t timeMs)
{
    TickType_t ticks;

    ticks = timeMs / portTICK_PERIOD_MS;

    /* Minimum delay = 1 tick */
    vTaskDelay(ticks ? ticks : 1);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode Osal_MutexCreate(T_DjiMutexHandle *mutex)
{
    if (mutex == NULL) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    *mutex = xSemaphoreCreateMutex();
    if (*mutex == NULL) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode Osal_MutexDestroy(T_DjiMutexHandle mutex)
{
    if (mutex == NULL) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    vQueueDelete((SemaphoreHandle_t) mutex);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode Osal_MutexLock(T_DjiMutexHandle mutex)
{
    TickType_t ticks;

    if (mutex == NULL) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    ticks = portMAX_DELAY;

    if (xSemaphoreTake(mutex, ticks) != pdTRUE) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode Osal_MutexUnlock(T_DjiMutexHandle mutex)
{
    if (mutex == NULL) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    if (xSemaphoreGive(mutex) != pdTRUE) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode Osal_SemaphoreCreate(uint32_t initValue, T_DjiSemaHandle *semaphore)
{
    uint32_t maxCount = UINT_MAX;

    if (semaphore == NULL) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    *semaphore = xSemaphoreCreateCounting(maxCount, initValue);

    if (*semaphore == NULL) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode Osal_SemaphoreDestroy(T_DjiSemaHandle semaphore)
{
    if (semaphore == NULL) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    vSemaphoreDelete(semaphore);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode Osal_SemaphoreTimedWait(T_DjiSemaHandle semaphore, uint32_t waitTimeMs)
{
    TickType_t ticks;

    if (semaphore == NULL) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
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
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode Osal_SemaphoreWait(T_DjiSemaHandle semaphore)
{
    Osal_SemaphoreTimedWait(semaphore, SEM_MUTEX_WAIT_FOREVER);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode Osal_SemaphorePost(T_DjiSemaHandle semaphore)
{
    if (semaphore == NULL) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    if (xSemaphoreGive(semaphore) != pdTRUE) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode Osal_GetTimeMs(uint32_t *ms)
{
    if (ms == NULL) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    *ms = xTaskGetTickCount();

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode Osal_GetTimeUs(uint64_t *us)
{
    if (us == NULL) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    *us = xTaskGetTickCount() * 1000;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode Osal_GetRandomNum(uint16_t *randomNum)
{
    *randomNum = rand() % 65535;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
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
