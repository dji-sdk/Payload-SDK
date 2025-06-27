/**
 ********************************************************************
 * @file    test_time_sync.c
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
#include <fc_subscription/test_fc_subscription.h>
#include "test_time_sync.h"
#include "dji_time_sync.h"
#include "dji_logger.h"
#include "utils/util_misc.h"
#include "dji_platform.h"

/* Private constants ---------------------------------------------------------*/
#define DJI_TEST_TIME_SYNC_TASK_FREQ            (1)
#define DJI_TEST_TIME_SYNC_TASK_STACK_SIZE      (1024)

/* Private types -------------------------------------------------------------*/

/* Private functions declaration ---------------------------------------------*/
static void *DjiTest_TimeSyncTask(void *arg);

/* Private variables ---------------------------------------------------------*/
static T_DjiTestTimeSyncHandler s_timeSyncHandler;
static T_DjiTaskHandle s_timeSyncThread;

/* Exported functions definition ---------------------------------------------*/
/**
 * @brief Register handler function for initialising PPS pin configure and reporting the latest local time when PPS is
 * triggered. This function have to be called before calling DjiTest_TimeSyncInit().
 * @param timeSyncHandler: pointer to handler function for time synchronization.
 * @return Execution result.
 */
T_DjiReturnCode DjiTest_TimeSyncRegHandler(T_DjiTestTimeSyncHandler *timeSyncHandler)
{
    if (timeSyncHandler->PpsSignalResponseInit == NULL) {
        USER_LOG_ERROR("reg time sync handler PpsSignalResponseInit error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    if (timeSyncHandler->GetNewestPpsTriggerLocalTimeUs == NULL) {
        USER_LOG_ERROR("reg time sync handler GetNewestPpsTriggerLocalTimeUs error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    memcpy(&s_timeSyncHandler, timeSyncHandler, sizeof(T_DjiTestTimeSyncHandler));

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiTest_TimeSyncStartService(void)
{
    T_DjiReturnCode djiStat;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    djiStat = DjiTimeSync_Init();
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("time synchronization module init error.");
        return djiStat;
    }

    if (s_timeSyncHandler.PpsSignalResponseInit == NULL) {
        USER_LOG_ERROR("time sync handler PpsSignalResponseInit interface is NULL error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    if (s_timeSyncHandler.GetNewestPpsTriggerLocalTimeUs == NULL) {
        USER_LOG_ERROR("time sync handler GetNewestPpsTriggerLocalTimeUs interface is NULL error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    // users must register getNewestPpsTriggerTime callback function
    djiStat = DjiTimeSync_RegGetNewestPpsTriggerTimeCallback(s_timeSyncHandler.GetNewestPpsTriggerLocalTimeUs);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("register GetNewestPpsTriggerLocalTimeUsCallback error.");
        return djiStat;
    }

    if (osalHandler->TaskCreate("user_time_sync_task", DjiTest_TimeSyncTask,
                                DJI_TEST_TIME_SYNC_TASK_STACK_SIZE, NULL, &s_timeSyncThread) !=
        DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("user time sync task create error.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    djiStat = s_timeSyncHandler.PpsSignalResponseInit();
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("pps signal response init error");
        return djiStat;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiTest_TimeSyncGetNewestPpsTriggerLocalTimeUs(uint64_t *localTimeUs)
{
    if (s_timeSyncHandler.GetNewestPpsTriggerLocalTimeUs == NULL) {
        USER_LOG_ERROR("GetNewestPpsTriggerLocalTimeUs null error.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    return s_timeSyncHandler.GetNewestPpsTriggerLocalTimeUs(localTimeUs);
}

/* Private functions definition-----------------------------------------------*/
#ifndef __CC_ARM
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wreturn-type"
#endif

static void *DjiTest_TimeSyncTask(void *arg)
{
    T_DjiReturnCode djiStat;
    uint32_t currentTimeMs = 0;
    T_DjiTimeSyncAircraftTime aircraftTime = {0};
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    uint8_t totalSatelliteNumber = 0;

    USER_UTIL_UNUSED(arg);

    while (1) {
        osalHandler->TaskSleepMs(1000 / DJI_TEST_TIME_SYNC_TASK_FREQ);

        djiStat = DjiTest_FcSubscriptionGetTotalSatelliteNumber(&totalSatelliteNumber);
        if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("get total satellite number error: 0x%08llX.", djiStat);
            continue;
        }

        djiStat = osalHandler->GetTimeMs(&currentTimeMs);
        if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("get current time error: 0x%08llX.", djiStat);
            continue;
        }

        djiStat = DjiTimeSync_TransferToAircraftTime(currentTimeMs * 1000, &aircraftTime);
        if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("transfer to aircraft time error: 0x%08llX.", djiStat);
            continue;
        }

        if ((aircraftTime.second % 30) == 0) {
            USER_LOG_INFO("current aircraft time is %04d-%02d-%02d %02d:%02d:%02d %d.",
                        aircraftTime.year, aircraftTime.month, aircraftTime.day,
                        aircraftTime.hour, aircraftTime.minute, aircraftTime.second, aircraftTime.microsecond);
        } else {
            USER_LOG_DEBUG("current aircraft time is %04d-%02d-%02d %02d:%02d:%02d %d.",
                        aircraftTime.year, aircraftTime.month, aircraftTime.day,
                        aircraftTime.hour, aircraftTime.minute, aircraftTime.second, aircraftTime.microsecond);
        }

    }
}

#ifndef __CC_ARM
#pragma GCC diagnostic pop
#endif

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
