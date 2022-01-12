/**
 ********************************************************************
 * @file    test_time_sync.c
 * @version V1.0.0
 * @date    11/26/19
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
#include "test_time_sync.h"
#include "psdk_time_sync.h"
#include "psdk_logger.h"
#include "utils/util_misc.h"
#include "psdk_platform.h"

/* Private constants ---------------------------------------------------------*/
#define PSDK_TEST_TIME_SYNC_TASK_FREQ       (1)
#define PSDK_TEST_TIME_SYNC_TASK_STACK_SIZE (2048)

/* Private types -------------------------------------------------------------*/


/* Private functions declaration ---------------------------------------------*/
static void *PsdkTest_TimeSyncTask(void *arg);

/* Private variables ---------------------------------------------------------*/
static T_PsdkTestTimeSyncHandler s_timeSyncHandler;
static T_PsdkTaskHandle s_timeSyncThread;

/* Exported functions definition ---------------------------------------------*/
/**
 * @brief Register handler function for initialising PPS pin configure and reporting the latest local time when PPS is
 * triggered. This function have to be called before calling PsdkTest_TimeSyncInit().
 * @param timeSyncHandler: pointer to handler function for time synchronization.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkTest_TimeSyncRegHandler(T_PsdkTestTimeSyncHandler *timeSyncHandler)
{
    if (timeSyncHandler->PpsSignalResponseInit == NULL) {
        PsdkLogger_UserLogError("reg time sync handler PpsSignalResponseInit error");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    if (timeSyncHandler->GetNewestPpsTriggerLocalTimeUs == NULL) {
        PsdkLogger_UserLogError("reg time sync handler GetNewestPpsTriggerLocalTimeUs error");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    memcpy(&s_timeSyncHandler, timeSyncHandler, sizeof(T_PsdkTestTimeSyncHandler));

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_PsdkReturnCode PsdkTest_TimeSyncInit(void)
{
    T_PsdkReturnCode psdkStat;

    psdkStat = PsdkTimeSync_Init();
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("time synchronization module init error.");
        return psdkStat;
    }

    if (s_timeSyncHandler.PpsSignalResponseInit == NULL) {
        PsdkLogger_UserLogError("time sync handler PpsSignalResponseInit interface is NULL error");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    if (s_timeSyncHandler.GetNewestPpsTriggerLocalTimeUs == NULL) {
        PsdkLogger_UserLogError("time sync handler GetNewestPpsTriggerLocalTimeUs interface is NULL error");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    // users must register getNewestPpsTriggerTime callback function
    psdkStat = PsdkTimeSync_RegGetNewestPpsTriggerTimeCallback(s_timeSyncHandler.GetNewestPpsTriggerLocalTimeUs);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("register GetNewestPpsTriggerLocalTimeUsCallback error.");
        return psdkStat;
    }

    if (PsdkOsal_TaskCreate(&s_timeSyncThread, PsdkTest_TimeSyncTask, "user_time_sync_task",
                            PSDK_TEST_TIME_SYNC_TASK_STACK_SIZE, NULL) !=
        PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("user time sync task create error.");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    psdkStat = s_timeSyncHandler.PpsSignalResponseInit();
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("pps signal response init error");
        return psdkStat;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_PsdkReturnCode PsdkTest_TimeSyncGetNewestPpsTriggerLocalTimeUs(uint64_t *localTimeUs)
{
    if (s_timeSyncHandler.GetNewestPpsTriggerLocalTimeUs == NULL) {
        PsdkLogger_UserLogError("GetNewestPpsTriggerLocalTimeUs null error.");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    return s_timeSyncHandler.GetNewestPpsTriggerLocalTimeUs(localTimeUs);
}

/* Private functions definition-----------------------------------------------*/
#ifndef __CC_ARM
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wreturn-type"
#endif

static void *PsdkTest_TimeSyncTask(void *arg)
{
    T_PsdkReturnCode psdkStat;
    uint32_t currentTimeMs = 0;
    T_PsdkTimeSyncAircraftTime aircraftTime = {0};

    USER_UTIL_UNUSED(arg);

    while (1) {
        PsdkOsal_TaskSleepMs(1000 / PSDK_TEST_TIME_SYNC_TASK_FREQ);

        psdkStat = PsdkOsal_GetTimeMs(&currentTimeMs);
        if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            PsdkLogger_UserLogError("get current time error: 0x%08llX.", psdkStat);
            continue;
        }

        psdkStat = PsdkTimeSync_TransferToAircraftTime(currentTimeMs * 1000, &aircraftTime);
        if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            PsdkLogger_UserLogError("transfer to aircraft time error: 0x%08llX.", psdkStat);
            continue;
        }

        PsdkLogger_UserLogDebug("current aircraft time is %d.%d.%d %d:%d %d %d.", aircraftTime.year, aircraftTime.month,
                                aircraftTime.day, aircraftTime.hour, aircraftTime.minute, aircraftTime.second,
                                aircraftTime.microsecond);
    }
}

#ifndef __CC_ARM
#pragma GCC diagnostic pop
#endif

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
