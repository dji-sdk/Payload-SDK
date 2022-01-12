/**
 ********************************************************************
 * @file    test_positioning.c
 * @version V2.0.0
 * @date    2019/8/22
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
#include "test_positioning.h"
#include "psdk_positioning.h"
#include "psdk_logger.h"
#include "utils/util_misc.h"
#include "psdk_platform.h"
#include "time_synchronization/test_time_sync.h"

/* Private constants ---------------------------------------------------------*/
#define POSITIONING_TASK_FREQ               (1)
#define POSITIONING_TASK_STACK_SIZE         (2048)
#define TEST_EVENT_COUNT                    (2)
#define TEST_TIME_INTERVAL_AMONG_EVENTS_US  (200000)

/* Private types -------------------------------------------------------------*/


/* Private functions declaration ---------------------------------------------*/
static void *PsdkTest_PositioningTask(void *arg);

/* Private variables ---------------------------------------------------------*/
static T_PsdkTaskHandle s_userPositioningThread;
static int32_t s_eventIndex = 0;

/* Exported functions definition ---------------------------------------------*/
T_PsdkReturnCode PsdkTest_PositioningInit(void)
{
    T_PsdkReturnCode psdkStat;

    psdkStat = PsdkPositioning_Init();
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("positioning module init error.");
        return psdkStat;
    }

    PsdkPositioning_SetTaskIndex(0);

    if (PsdkOsal_TaskCreate(&s_userPositioningThread, PsdkTest_PositioningTask, "user_positioning_task",
                            POSITIONING_TASK_STACK_SIZE, NULL) != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("user positioning task create error.");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/* Private functions definition-----------------------------------------------*/
#ifndef __CC_ARM
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wreturn-type"
#endif

static void *PsdkTest_PositioningTask(void *arg)
{
    int32_t i = 0;
    T_PsdkReturnCode psdkStat;
    uint64_t ppsNewestTriggerTimeUs = 0;
    T_PsdkPositioningEventInfo eventInfo[TEST_EVENT_COUNT] = {0};
    T_PsdkPositioningPositionInfo positionInfo[TEST_EVENT_COUNT] = {0};
    T_PsdkTimeSyncAircraftTime aircraftTime = {0};

    USER_UTIL_UNUSED(arg);

    while (1) {
        PsdkOsal_TaskSleepMs(1000 / POSITIONING_TASK_FREQ);

        psdkStat = PsdkTest_TimeSyncGetNewestPpsTriggerLocalTimeUs(&ppsNewestTriggerTimeUs);
        if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            PsdkLogger_UserLogError("get newest pps trigger time error: 0x%08llX.", psdkStat);
            continue;
        }

        for (i = 0; i < TEST_EVENT_COUNT; ++i) {
            eventInfo[i].eventSetIndex = s_eventIndex;
            eventInfo[i].targetPointIndex = i;

            psdkStat = PsdkTimeSync_TransferToAircraftTime(
                ppsNewestTriggerTimeUs - 1000000 - i * TEST_TIME_INTERVAL_AMONG_EVENTS_US, &aircraftTime);
            if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                PsdkLogger_UserLogError("transfer to aircraft time error: 0x%08llX.", psdkStat);
                continue;
            }

            eventInfo[i].eventTime = aircraftTime;
        }

        psdkStat = PsdkPositioning_GetPositionInformationSync(TEST_EVENT_COUNT, eventInfo, positionInfo);
        if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            PsdkLogger_UserLogError("get position information error.");
            continue;
        }

        PsdkLogger_UserLogDebug("request position of target points success.");
        PsdkLogger_UserLogDebug("detail position information:");
        for (i = 0; i < TEST_EVENT_COUNT; ++i) {
            PsdkLogger_UserLogDebug("position solution property: %d.", positionInfo[i].positionSolutionProperty);
            PsdkLogger_UserLogDebug("pitchAttitudeAngle: %d\trollAttitudeAngle: %d\tyawAttitudeAngle: %d",
                                    positionInfo[i].uavAttitude.pitch, positionInfo[i].uavAttitude.roll,
                                    positionInfo[i].uavAttitude.yaw);
            PsdkLogger_UserLogDebug("northPositionOffset: %d\tearthPositionOffset: %d\tdownPositionOffset: %d",
                                    positionInfo[i].offsetBetweenMainAntennaAndTargetPoint.x,
                                    positionInfo[i].offsetBetweenMainAntennaAndTargetPoint.y,
                                    positionInfo[i].offsetBetweenMainAntennaAndTargetPoint.z);
            PsdkLogger_UserLogDebug("longitude: %.8f\tlatitude: %.8f\theight: %.8f",
                                    positionInfo[i].targetPointPosition.longitude,
                                    positionInfo[i].targetPointPosition.latitude,
                                    positionInfo[i].targetPointPosition.height);
            PsdkLogger_UserLogDebug(
                "longStandardDeviation: %.8f\tlatStandardDeviation: %.8f\thgtStandardDeviation: %.8f",
                positionInfo[i].targetPointPositionStandardDeviation.longitude,
                positionInfo[i].targetPointPositionStandardDeviation.latitude,
                positionInfo[i].targetPointPositionStandardDeviation.height);
        }

        s_eventIndex++;
    }
}

#ifndef __CC_ARM
#pragma GCC diagnostic pop
#endif

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
