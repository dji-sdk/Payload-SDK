/**
 ********************************************************************
 * @file    psdk_platform.h
 * @brief   This is the header file for define PSDK platform interfaces.
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
#ifndef PSDK_PLATFORM_H
#define PSDK_PLATFORM_H

/* Includes ------------------------------------------------------------------*/
#include "legacy_psdk2.x/psdk_typedef.h"
#include "dji_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/
/**
* @brief Platform handle of thread task operation.
*/
typedef T_DjiTaskHandle T_PsdkTaskHandle;
/**
* @brief Platform handle of mutex operation.
*/
typedef T_DjiMutexHandle T_PsdkMutexHandle;
/**
* @brief Platform handle of semaphore operation.
*/
typedef T_DjiSemaHandle T_PsdkSemHandle;

/* Exported types ------------------------------------------------------------*/
typedef T_DjiHalUartHandler T_PsdkHalUartHandler;

typedef T_DjiHalNetworkHandler T_PsdkHalNetWorkHandler;

typedef T_DjiOsalHandler T_PsdkOsalHandler;

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Register the handler for hal uart interfaces by your platform.
 * @note It should be noted that the interface in hal is written and tested well. Users need to implement all the
 * interfaces. Otherwise, the user interface cannot be successfully registered, and then the user interface is registered
 * through the interface. If the registration fails, it needs to be based on the return code. To judge the problem. Make
 * sure that the feature is available after a successful registration. The interface needs to be called at the beginning of
 * the application for registration, otherwise, the subsequent functions will not work properly.
 * @param halUartHandler: pointer to the handler for hal uart interfaces by your platform.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkPlatform_RegHalUartHandler(const T_PsdkHalUartHandler *halUartHandler);

/**
 * @brief Register the handler for osal interfaces by your platform.
 * @note It should be noted that the interface in osal is written and tested well. Users need to implement all the
 * interfaces. Otherwise, the user interface cannot be successfully registered, and then the user interface is registered
 * through the interface. If the registration fails, it needs to be based on the return code. To judge the problem. Make
 * sure that the feature is available after a successful registration. The interface needs to be called at the beginning of
 * the application for registration, otherwise, the subsequent functions will not work properly.
 * @param osalHandler: pointer to the handler for osal interfaces by your platform.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkPlatform_RegOsalHandler(const T_PsdkOsalHandler *osalHandler);

/**
 * @brief Register the handler for hal network interfaces by your platform.
 * @note It should be noted that the interface in hal is written and tested well. Users need to implement all the
 * interfaces. Otherwise, the user interface cannot be successfully registered, and then the user interface is registered
 * through the interface. If the registration fails, it needs to be based on the return code. To judge the problem. Make
 * sure that the feature is available after a successful registration.
 * @attention The interface needs to be called at the beginning of the application for registration, otherwise, the
 * subsequent functions will not work properly.
 * @param osalHandler: pointer to the handler for network handler interfaces by your platform.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkPlatform_RegHalNetworkHandler(const T_PsdkHalNetWorkHandler *halNetWorkHandler);

/**
 * @brief Declare the task container, initialize the task, and create task ID.
 * @param task: pointer to the created task ID.
 * @param taskFunc: pointer to the created task function.
 * @param name: pointer to task name string, end with '\0'. If user do not specify name for task, fill in NULL.
 * @param stackSize: value of task stack size, unit: byte.
 * @param arg: pointer to the user defined data.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkOsal_TaskCreate(T_PsdkTaskHandle *task, void *(*taskFunc)(void *), const char *name,
                                     uint32_t stackSize, void *arg);

/**
 * @brief Destroy the created task.
 * @param task: pointer to the created task ID.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkOsal_TaskDestroy(T_PsdkTaskHandle task);

/**
 * @brief Let task into a sleep state in a set time, uint:ms.
 * @param timeMs: value of time ms for task sleeping.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkOsal_TaskSleepMs(uint32_t timeMs);

/**
 * @brief  Declare the mutex container, initialize the mutex, and create mutex ID.
 * @param  mutex: pointer to the created mutex ID.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkOsal_MutexCreate(T_PsdkMutexHandle *mutex);

/**
 * @brief Destroy the created mutex.
 * @param mutex: pointer to the created mutex ID.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkOsal_MutexDestroy(T_PsdkMutexHandle mutex);

/**
 * @brief Acquire and lock the mutex when peripheral access is required.
 * @param mutex: pointer to the created mutex ID.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkOsal_MutexLock(T_PsdkMutexHandle mutex);

/**
 * @brief Unlock and release the mutex when done with the peripheral access.
 * @param mutex: pointer to the created mutex ID.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkOsal_MutexUnlock(T_PsdkMutexHandle mutex);

/**
 * @brief  Declare the semaphore container, initialize the semaphore, and create semaphore ID.
 * @param  semaphore: pointer to the created semaphore ID.
 * @param  initValue: init value of semaphore for the created semaphore.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkOsal_SemaphoreCreate(T_PsdkSemHandle *semaphore, uint32_t initValue);

/**
 * @brief Destroy the created semaphore.
 * @param semaphore: pointer to the created semaphore ID.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkOsal_SemaphoreDestroy(T_PsdkSemHandle semaphore);

/**
 * @brief Wait the created semaphore forever.
 * @param semaphore: pointer to the created semaphore ID.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkOsal_SemaphoreWait(T_PsdkSemHandle semaphore);

/**
 * @brief Wait the created semaphore and set the value of timeout.
 * @param semaphore: pointer to the created semaphore ID.
 * @param waitTimeMs: value of timeout for waiting created semaphore.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkOsal_SemaphoreTimedWait(T_PsdkSemHandle semaphore, uint32_t waitTimeMs);

/**
 * @brief Post the created semaphore.
 * @param semaphore: pointer to the created semaphore ID.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkOsal_SemaphorePost(T_PsdkSemHandle semaphore);

/**
 * @brief Get the system millisecond time, uint:ms.
 * @param ms: pointer to the got time ms.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkOsal_GetTimeMs(uint32_t *ms);

/**
 * @brief Allocate size bytes of memory.
 * @note Users need to apply for memory according to the actual situation of the platform and application. If you do not
 * use the memory after applying for it, please call #PsdkOsal_Free to release the memory.
 * @param size: uint32_t size that needs allocate.
 * @return a void pointer to the allocated memory, if equal to NULL, please do related processing to avoid null pointer crash.
 */
void *PsdkOsal_Malloc(uint32_t size);

/**
 * @brief Release allocated memory.
 * @note After calling the #PsdkOsal_Malloc interface, if you do not use memory, please remember to use this interface
 * to release the memory.
 * @param ptr: pointer to the need allocated memory.
 */
void PsdkOsal_Free(void *ptr);

#ifdef __cplusplus
}
#endif

#endif // PSDK_PLATFORM_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
