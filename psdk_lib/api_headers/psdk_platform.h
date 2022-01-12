/**
 ********************************************************************
 * @file    psdk_platform.h
 * @brief   This is the header file for define PSDK platform interfaces.
 *
 * @copyright (c) 2018 DJI. All rights reserved.
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
#include "psdk_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/
/**
* @brief Platform handle of thread task operation.
*/
typedef void *T_PsdkTaskHandle;
/**
* @brief Platform handle of mutex operation.
*/
typedef void *T_PsdkMutexHandle;
/**
* @brief Platform handle of semaphore operation.
*/
typedef void *T_PsdkSemHandle;

/* Exported types ------------------------------------------------------------*/
typedef struct {
    /**
    * @brief Prototype of the callback function used to init the user uart that connected to the payload device.
    * @note  Users need to implement the initialization interface of their serial device and register it. For the related
    * parameter settings of the serial port, please refer to the document.
    * @return Execution result.
    */
    T_PsdkReturnCode (*UartInit)(void);

    /**
    * @brief Prototype of callback function used to write data by the user uart that connected to the payload device.
    * @note Users need to implement the sending interface of their serial device and register it. Please use the relevant
    * tools to test the interface before registering.
    * @param buf: pointer to memory space used to store the data that needs send by uart.
    * @param len: the length of data that needs send by uart.
    * @return Execution result.
    */
    T_PsdkReturnCode (*UartWriteData)(const uint8_t *buf, uint16_t len);

    /**
    * @brief Prototype of callback function used to read data from the user uart that connected to the payload device..
    * @note Users need to implement the read interface of their serial device and register it. Please use the relevant
    * tools to test the interface before registering.
    * @param buf: pointer to memory space used to store the data that read from uart.
    * @param len: the length of data that need read from uart.
    * @param realLen: pointer to memory space used to store the real size of reading data from uart.
    * @return Execution result.
    */
    T_PsdkReturnCode (*UartReadData)(uint8_t *buf, uint16_t len, uint16_t *realLen);
} T_PsdkHalUartHandler;

typedef struct {
    /**
    * @brief Prototype of callback function used to declare the task container, initialize the task, and create task ID.
    * @note This interface is mainly used to create the task. Users need to adapt according to their platform and system.
    * Also needs test the implemented interface before registration to ensure that the registration can be used normally.
    * For details of the parameters, please refer to the following API interface #PsdkOsal_TaskCreate.
    * @param task: pointer to memory space used to store the created task handle.
    * @param taskFunc: pointer to the created task function.
    * @param name: pointer to task name string, end with '\0'. If user does not specifically name for task, fill in NULL.
    * @param stackSize: the value of task stack size, unit: byte.
    * @param arg: pointer to the user-defined data.
    * @return Execution result.
    */
    T_PsdkReturnCode (*TaskCreate)(T_PsdkTaskHandle *task, void *(*taskFunc)(void *), const char *name,
                                   uint32_t stackSize, void *arg);

    /**
    * @brief Prototype of callback function used to destroy the created task.
    * @note This interface is mainly used to destroy the created task. Users need to adapt according to their platform
    * and system. Also needs test the implemented interface before registration to ensure that the registration can be used
    * normally. For details of the parameters, please refer to the following API interface #PsdkOsal_TaskDestroy.
    * @param task: task handle.
    * @return Execution result.
    */
    T_PsdkReturnCode (*TaskDestroy)(T_PsdkTaskHandle task);

    /**
    * @brief Prototype of callback function used to let task into a sleep state in a set time, uint:ms.
    * @note This interface is mainly used to let task sleep. Users need to adapt according to their platform and system.
    * Also needs test the implemented interface before registration to ensure that the registration can be used normally.
    * For details of the parameters, please refer to the following API interface #PsdkOsal_TaskSleepMs.
    * @param timeMs: the value of time ms for task sleeping.
    * @return Execution result.
    */
    T_PsdkReturnCode (*TaskSleepMs)(uint32_t timeMs);

    /**
    * @brief Prototype of callback function used to declare the mutex container, initialize the mutex, and create mutex ID.
    * @note This interface is mainly used to create a mutex. Users need to adapt according to their platform and system.
    * Also needs test the implemented interface before registration to ensure that the registration can be used normally.
    * For details of the parameters, please refer to the following API interface #PsdkOsal_MutexCreate.
    * @param mutex: pointer to memory space used to store the created mutex ID.
    * @return Execution result.
    */
    T_PsdkReturnCode (*MutexCreate)(T_PsdkMutexHandle *mutex);

    /**
    * @brief Prototype of callback function used to destroy the created mutex.
    * @note This interface is mainly used to destroy the created mutex. Users need to adapt according to their platform
    * and system. Also need test the implemented interface before registration to ensure that the registration can be used
    * normally. For details of the parameters, please refer to the following API interface #PsdkOsal_MutexDestroy.
    * @param mutex: pointer to memory space used to store the created mutex ID.
    * @return Execution result.
    */
    T_PsdkReturnCode (*MutexDestroy)(T_PsdkMutexHandle mutex);

    /**
    * @brief Prototype of callback function used to acquire and lock the mutex when peripheral access is required.
    * @note This interface is mainly used to lock the created mutex. Users need to adapt according to their platform
    * and system. Also needs test the implemented interface before registration to ensure that the registration can be
    * used normally. For details of the parameters, please
    * refer to the following API interface #PsdkOsal_MutexLock.
    * @param mutex: pointer to memory space used to store the created mutex ID.
    * @return Execution result.
    */
    T_PsdkReturnCode (*MutexLock)(T_PsdkMutexHandle mutex);

    /**
    * @brief Prototype of callback function used to unlock and release the mutex when done with the peripheral access.
    * @note This interface is mainly used to unlock the created mutex. Users need to adapt according to their platform
    * and system. Also needs test the implemented interface before registration to ensure that the registration can be used
    * normally. For details of the parameters, please refer to the following API interface #PsdkOsal_MutexUnlock.
    * @param mutex: pointer to memory space used to store the created mutex ID.
    * @return Execution result.
    */
    T_PsdkReturnCode (*MutexUnlock)(T_PsdkMutexHandle mutex);

    /**
    * @brief Prototype of callback function used to declare the semaphore container, initialize the semaphore, and create semaphore ID.
    * @note This interface is mainly used to create semaphore. Users need to adapt according to their platform and system.
    * Also needs test the implemented interface before registration to ensure that the registration can be used normally.
    * For details of the parameters, please refer to the following API interface #PsdkOsal_SemaphoreCreate.
    * @param semaphore: pointer to memory space used to store the created semaphore ID.
    * @param  initValue: init value of semaphore for the created semaphore.
    * @return Execution result.
    */
    T_PsdkReturnCode (*SemaphoreCreate)(T_PsdkSemHandle *semaphore, uint32_t initValue);

    /**
    * @brief Prototype of callback function used to destroy the created semaphore.
    * @note This interface is mainly used to destroy the created semaphore. Users need to adapt according to their own
    * platform and system. Also need test the implemented interface before registration to ensure that the registration
    * can be used normally. For details of the parameters, please refer to the following API interface #PsdkOsal_SemaphoreDestroy.s
    * @param semaphore: pointer to memory space used to store the created semaphore ID.
    * @return Execution result.
    */
    T_PsdkReturnCode (*SemaphoreDestroy)(T_PsdkSemHandle semaphore);

    /**
    * @brief Prototype of callback function used to wait the created semaphore forever.
    * @note This interface is mainly used to wait the created semaphore forever. Users need to adapt according to their
    * own platform and system. Also need to test the implemented interface before registration to ensure that the registration
    * can be used normally. For details of the parameters, please refer to the following API interface #PsdkOsal_SemaphoreWait.
    * @param semaphore: pointer to memory space used to store the created semaphore ID.
    * @return Execution result.
    */
    T_PsdkReturnCode (*SemaphoreWait)(T_PsdkSemHandle semaphore);

    /**
    * @brief Prototype of callback function used to ait the created semaphore and set the value of timeout.
    * @note This interface is mainly used to wait for the created semaphore. Users need to adapt according to their platform
    * and system. Also needs test the implemented interface before registration to ensure that the registration can be used
    * normally. For details of the parameters, please refer to the following API interface #PsdkOsal_SemaphoreTimedWait.
    * @param semaphore: pointer to memory space used to store the created semaphore ID.
    * @param waitTimeMs: the value of timeout for waiting created semaphore.
    * @return Execution result.
    */
    T_PsdkReturnCode (*SemaphoreTimedWait)(T_PsdkSemHandle semaphore, uint32_t waitTimeMs);

    /**
    * @brief Prototype of callback function used to post the created semaphore.
    * @note This interface is mainly used to post the created semaphore. Users need to adapt according to their platform
    * and system. Also needs test the implemented interface before registration to ensure that the registration can be used
    * normally. For details of the parameters, please refer to the following API interface #PsdkOsal_SemaphorePost.
    * @param semaphore: pointer to memory space used to store the created semaphore ID.
    * @return Execution result.
    */
    T_PsdkReturnCode (*SemaphorePost)(T_PsdkSemHandle semaphore);

    /**
     * @brief Prototype of callback function used to get the system millisecond time, uint:ms.
     * @note This interface is mainly used to get system millisecond time. Users need to adapt according to their own
     * platform and system. Also need test the implemented interface before registration to ensure that the registration
     * can be used normally. For details of the parameters, please refer to the following API interface #PsdkOsal_GetTimeMs.
     * @param factor: pointer to memory space used to store the got time ms.
     * @return Execution result.
     */
    T_PsdkReturnCode (*GetTimeMs)(uint32_t *ms);

    /**
     * @brief Prototype of callback function used to allocate size bytes of memory.
     * @note This interface is mainly used to allocate size bytes of memory. Users need to adapt according to their own
     * platform and system. Also need test the implemented interface before registration to ensure that the registration
     * can be used normally. For details of the parameters, please refer to the following API interface #PsdkOsal_Malloc.
     * @param size: uint32_t size that needs allocate.
     * @return a void pointer to the allocated memory, if equal to NULL, please do related processing to avoid null pointer crash.
     */
    void *(*Malloc)(uint32_t size);

    /**
     * @brief Prototype of callback function used to release allocated memory.
     * @note This interface is mainly used to release allocated memory. Users need to adapt according to their platform
     * and system. Also needs test the implemented interface before registration to ensure that the registration can be used
     * normally. For details of the parameters, please refer to the following API interface #PsdkOsal_Free.
     * @param ptr: pointer to the need allocated memory.
     */
    void (*Free)(void *ptr);
} T_PsdkOsalHandler;

typedef struct {
    /**
     * @brief Prototype of the callback function used to config the network device that connected to the payload device.
     * @note  Users need to implement the initialization interface of their network device and register it. For the related
     * parameter settings of the network port, please refer to the document.
     * @return Execution result.
     */
    T_PsdkReturnCode (*NetWorkConfig)(const char *ipAddr, const char *netMask);
} T_PsdkHalNetWorkHandler;

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
