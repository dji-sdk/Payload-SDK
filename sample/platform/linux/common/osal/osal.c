/**
 ********************************************************************
 * @file    psdk_osal.c
 * @version V2.0.0
 * @date    2019/07/01
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
#include "osal.h"
#include "utils/util_misc.h"
#include "psdk_logger.h"

/* Private constants ---------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private functions declaration ---------------------------------------------*/

/* Exported functions definition ---------------------------------------------*/

/* Private functions definition-----------------------------------------------*/

/* Private functions declaration ---------------------------------------------*/

/* Exported functions definition ---------------------------------------------*/

T_PsdkReturnCode
Osal_TaskCreate(T_PsdkTaskHandle *task, void *(*taskFunc)(void *), const char *name, uint32_t stackSize, void *arg)
{
    int result;
    char nameDealed[16] = {0};

    USER_UTIL_UNUSED(stackSize);

    *task = malloc(sizeof(pthread_t));
    if (*task == NULL) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_MEMORY_ALLOC_FAILED;
    }

    result = pthread_create(*task, NULL, taskFunc, arg);
    if (result != 0) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    if (name != NULL)
        strncpy(nameDealed, name, sizeof(nameDealed) - 1);
    result = pthread_setname_np(*(pthread_t * ) * task, nameDealed);
    if (result != 0) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_PsdkReturnCode Osal_TaskDestroy(T_PsdkTaskHandle task)
{
    pthread_cancel(*(pthread_t *) task);
    free(task);

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_PsdkReturnCode Osal_TaskSleepMs(uint32_t timeMs)
{
    usleep(1000 * timeMs);

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * @brief Declare the mutex container, initialize the mutex, and
 * create mutex ID.
 * @param mutex:  pointer to the created mutex ID.
 * @return an enum that represents a status of PSDK
 */
T_PsdkReturnCode Osal_MutexCreate(T_PsdkMutexHandle *mutex)
{
    int result;

    *mutex = malloc(sizeof(pthread_mutex_t));
    if (*mutex == NULL) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_MEMORY_ALLOC_FAILED;
    }

    result = pthread_mutex_init(*mutex, NULL);
    if (result != 0) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * @brief Delete the created mutex.
 * @param mutex:  pointer to the created mutex ID.
 * @return an enum that represents a status of PSDK
 */
T_PsdkReturnCode Osal_MutexDestroy(T_PsdkMutexHandle mutex)
{
    int result;

    result = pthread_mutex_destroy(mutex);
    if (result != 0) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }
    free(mutex);

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * @brief Acquire and lock the mutex when peripheral access is required
 * @param mutex:  pointer to the created mutex ID.
 * @return an enum that represents a status of PSDK
 */
T_PsdkReturnCode Osal_MutexLock(T_PsdkMutexHandle mutex)
{
    int result = pthread_mutex_lock(mutex);

    if (result != 0) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * @brief Unlock and release the mutex, when done with the peripheral access.
 * @param mutex:  pointer to the created mutex ID.
 * @return an enum that represents a status of PSDK
 */
T_PsdkReturnCode Osal_MutexUnlock(T_PsdkMutexHandle mutex)
{
    int result = pthread_mutex_unlock(mutex);

    if (result != 0) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * @brief Declare the semaphore container, initialize the semaphore, and
 * create semaphore ID.
 * @param semaphore: pointer to the created semaphore ID.
 * @param initValue: initial value of semaphore.
 * @return an enum that represents a status of PSDK
 */
T_PsdkReturnCode Osal_SemaphoreCreate(T_PsdkSemHandle *semaphore, uint32_t initValue)
{
    int result;

    *semaphore = malloc(sizeof(sem_t));
    if (*semaphore == NULL) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_MEMORY_ALLOC_FAILED;
    }

    result = sem_init(*semaphore, 0, (unsigned int) initValue);
    if (result != 0) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * @brief Delete the created semaphore.
 * @param semaphore: pointer to the created semaphore ID.
 * @return an enum that represents a status of PSDK
 */
T_PsdkReturnCode Osal_SemaphoreDestroy(T_PsdkSemHandle semaphore)
{
    int result;

    result = sem_destroy((sem_t *) semaphore);
    if (result != 0) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    free(semaphore);

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * @brief Wait the semaphore until token becomes available.
 * @param semaphore: pointer to the created semaphore ID.
 * @return an enum that represents a status of PSDK
 */
T_PsdkReturnCode Osal_SemaphoreWait(T_PsdkSemHandle semaphore)
{
    int result;

    result = sem_wait(semaphore);
    if (result != 0) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * @brief Wait the semaphore until token becomes available.
 * @param semaphore: pointer to the created semaphore ID.
 * @param waitTime: timeout value of waiting semaphore, unit: millisecond.
 * @return an enum that represents a status of PSDK
 */
T_PsdkReturnCode Osal_SemaphoreTimedWait(T_PsdkSemHandle semaphore, uint32_t waitTime)
{
    int result;
    struct timespec semaphoreWaitTime;
    struct timeval systemTime;

    gettimeofday(&systemTime, NULL);

    systemTime.tv_usec += waitTime * 1000;
    if (systemTime.tv_usec >= 1000000) {
        systemTime.tv_sec += systemTime.tv_usec / 1000000;
        systemTime.tv_usec %= 1000000;
    }

    semaphoreWaitTime.tv_sec = systemTime.tv_sec;
    semaphoreWaitTime.tv_nsec = systemTime.tv_usec * 1000;

    result = sem_timedwait(semaphore, &semaphoreWaitTime);
    if (result != 0) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * @brief Release the semaphore token.
 * @param semaphore: pointer to the created semaphore ID.
 * @return an enum that represents a status of PSDK
 */
T_PsdkReturnCode Osal_SemaphorePost(T_PsdkSemHandle semaphore)
{
    int result;

    result = sem_post(semaphore);
    if (result != 0) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * @brief Get the system time for ms.
 * @return an uint32 that the time of system, uint:ms
 */
T_PsdkReturnCode Osal_GetTimeMs(uint32_t *ms)
{
    struct timeval time;

    gettimeofday(&time, NULL);
    *ms = (time.tv_sec * 1000 + time.tv_usec / 1000);

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

void *Osal_Malloc(uint32_t size)
{
    return malloc(size);
}

void Osal_Free(void *ptr)
{
    free(ptr);
}

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/