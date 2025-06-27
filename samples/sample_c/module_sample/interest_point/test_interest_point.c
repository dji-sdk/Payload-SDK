/**
 ********************************************************************
 * @file    test_interest_point.c
 * @brief
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

/* Includes ------------------------------------------------------------------*/
#include "test_interest_point.h"
#include "dji_interest_point.h"
#include "dji_logger.h"
#include "dji_flight_controller.h"
#include "flight_control/test_flight_control.h"
#include "dji_aircraft_info.h"

/* Private constants ---------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private values -------------------------------------------------------------*/
static T_DjiReturnCode DjiUser_InterestPointMissionStateCallback(T_DjiInterestPointMissionState missionState);

/* Private functions declaration ---------------------------------------------*/

/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode DjiTest_InterestPointRunSample(void)
{
    T_DjiReturnCode returnCode;
    T_DjiInterestPointSettings interestPointSettings = {0};
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    T_DjiFlightControllerRidInfo ridInfo = {0};
    T_DjiAircraftInfoBaseInfo aircraftInfoBaseInfo = {0};

    ridInfo.latitude = 22.542812;
    ridInfo.longitude = 113.958902;
    ridInfo.altitude = 10;

    returnCode = DjiAircraftInfo_GetBaseInfo(&aircraftInfoBaseInfo);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("get aircraft base info error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    returnCode = DjiFlightController_Init(ridInfo);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Flight control init failed, errno=%lld", returnCode);
        return returnCode;
    }

    returnCode = DjiInterestPoint_Init();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Point interest init failed, errno=%lld", returnCode);
        return returnCode;
    }

    osalHandler->TaskSleepMs(1000);
    DjiFlightController_ObtainJoystickCtrlAuthority();

    osalHandler->TaskSleepMs(1000);
    DjiFlightController_StartTakeoff();

    osalHandler->TaskSleepMs(1000);
    DjiTest_FlightControlVelocityAndYawRateCtrl((T_DjiTestFlightControlVector3f) {0, 0, 5}, 0, 10000);

    osalHandler->TaskSleepMs(1000);
    DjiTest_FlightControlVelocityAndYawRateCtrl((T_DjiTestFlightControlVector3f) {3, 0, 0}, 0, 5000);

    interestPointSettings.latitude = 22.57775;
    interestPointSettings.longitude = 113.94265;

    returnCode = DjiInterestPoint_RegMissionStateCallback(DjiUser_InterestPointMissionStateCallback);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Register mission state callback failed, errno=%lld", returnCode);
        return returnCode;
    }

    if (aircraftInfoBaseInfo.aircraftSeries == DJI_AIRCRAFT_SERIES_M400) {
        interestPointSettings.payloadCameraIndex = 1;
        USER_LOG_INFO("The sample try to use payload camera on position %d for poi, please make sure that DJI camera mounted on this position.",
            interestPointSettings.payloadCameraIndex);
    }

    USER_LOG_INFO("Start interest point circle, circumcenter(%f, %f)", interestPointSettings.latitude, interestPointSettings.longitude);
    returnCode = DjiInterestPoint_Start(interestPointSettings);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Point interest start failed, errno=%lld", returnCode);
        return returnCode;
    }

    osalHandler->TaskSleepMs(1000);

    USER_LOG_INFO("set speed to 15");
    DjiInterestPoint_SetSpeed(15.0f);

    for (int i = 0; i < 60; ++i) {
        USER_LOG_INFO("Interest point mission running %d.", i);
        osalHandler->TaskSleepMs(1000);
    }

    USER_LOG_INFO("Stop interest point circle");
    returnCode = DjiInterestPoint_Stop();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Point interest stop failed, errno=%lld", returnCode);
        return returnCode;
    }

    USER_LOG_INFO("Force landing");
    DjiFlightController_StartForceLanding();

    returnCode = DjiInterestPoint_DeInit();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Point interest deinit failed, errno=%lld", returnCode);
        return returnCode;
    }

    returnCode = DjiFlightController_DeInit();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Flight control init failed, errno=%lld", returnCode);
        return returnCode;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/* Private functions definition-----------------------------------------------*/
static T_DjiReturnCode DjiUser_InterestPointMissionStateCallback(T_DjiInterestPointMissionState missionState)
{
    USER_LOG_INFO("Interest point state: %s, radius: %.2f m, speed: %.2f m/s",
                  missionState.state == DJI_INTEREST_POINT_MISSION_ACTION_STATE_NOT_STARTED ? "not started" :
                  missionState.state == DJI_INTEREST_POINT_MISSION_ACTION_STATE_PAUSE ? "pause" :
                  missionState.state == DJI_INTEREST_POINT_MISSION_ACTION_STATE_RUNNING ? "running" : "unknown",
                  missionState.radius,
                  missionState.curSpeed);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
