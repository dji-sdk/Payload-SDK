/**
 ********************************************************************
 * @file    test_xport.c
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
#include <dji_gimbal.h>
#include "test_payload_xport.h"
#include "dji_logger.h"
#include "dji_platform.h"
#include "utils/util_misc.h"
#include "dji_aircraft_info.h"
#include "camera_emu/test_payload_cam_emu_base.h"

/* Private constants ---------------------------------------------------------*/
#define XPORT_TASK_FREQ             (10)
#define XPORT_TASK_STACK_SIZE       (2048)

/* Private types -------------------------------------------------------------*/


/* Private functions declaration ---------------------------------------------*/
static void *UserXPort_Task(void *arg);
static T_DjiReturnCode ReceiveXPortSystemState(T_DjiGimbalSystemState systemState);
static T_DjiReturnCode ReceiveXPortAttitudeInformation(T_DjiGimbalAttitudeInformation attitudeInformation);

/* Private variables ---------------------------------------------------------*/
static T_DjiTaskHandle s_userXPortThread;
static T_DjiMutexHandle s_userXPortMutex;
static T_DjiGimbalSystemState s_userXPortSystemState = {0};
static bool s_isUserXPortInited = false;
static bool s_isUserXPortSystemStateVaild = false;

/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode DjiTest_XPortStartService(void)
{
    T_DjiReturnCode djiStat;
    T_DjiXPortLimitAngle limitAngle = {0};
    T_DjiAircraftInfoBaseInfo aircraftInfoBaseInfo = {0};
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    djiStat = DjiXPort_Init();
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("XPort init error: 0x%08llX.", djiStat);
        return djiStat;
    }

    s_isUserXPortInited = true;

    djiStat = osalHandler->MutexCreate(&s_userXPortMutex);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("user XPort mutex create error: 0x%08llX.", djiStat);
        return djiStat;
    }

    djiStat = DjiXPort_RegReceiveSystemStateCallback(ReceiveXPortSystemState);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("register receive XPort system state callback function error: 0x%08llX.", djiStat);
        return djiStat;
    }

    djiStat = DjiXPort_RegReceiveAttitudeInformationCallback(ReceiveXPortAttitudeInformation);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("register receive XPort attitude information callback function error: 0x%08llX.",
                       djiStat);
        return djiStat;
    }

    limitAngle.upperLimit = 300;
    limitAngle.lowerLimit = -1000;
    USER_LOG_INFO("Set joint angle limit of pitch axis, upperLimit %d, lowerLimit %d", limitAngle.upperLimit, limitAngle.lowerLimit);
    djiStat = DjiXPort_SetLimitAngleSync(DJI_XPORT_LIMIT_ANGLE_CATEGORY_PITCH_JOINT_ANGLE, limitAngle);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("set pitch joint angle limit angle for XPort error: 0x%08llX.", djiStat);
        return djiStat;
    }

    limitAngle.upperLimit = 300;
    limitAngle.lowerLimit = -800;
    USER_LOG_INFO("Set euler angle limit of pitch axis, upperLimit %d, lowerLimit %d", limitAngle.upperLimit, limitAngle.lowerLimit);
    djiStat = DjiXPort_SetLimitAngleSync(DJI_XPORT_LIMIT_ANGLE_CATEGORY_PITCH_EULER_ANGLE, limitAngle);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("set pitch euler angle limit angle for XPort error: 0x%08llX.", djiStat);
        return djiStat;
    }

    limitAngle.upperLimit = 300;
    limitAngle.lowerLimit = -1000;
    USER_LOG_INFO("Set extended euler angle limit of pitch axis, upperLimit %d, lowerLimit %d", limitAngle.upperLimit, limitAngle.lowerLimit);
    djiStat = DjiXPort_SetLimitAngleSync(DJI_XPORT_LIMIT_ANGLE_CATEGORY_PITCH_EULER_ANGLE_EXTENSION, limitAngle);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("set pitch extension euler angle limit angle for XPort error: 0x%08llX.", djiStat);
        return djiStat;
    }

    djiStat = DjiAircraftInfo_GetBaseInfo(&aircraftInfoBaseInfo);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("get aircraft base information error: 0x%08llX.", djiStat);
        return djiStat;
    }

    if (aircraftInfoBaseInfo.mountPosition == DJI_MOUNT_POSITION_PAYLOAD_PORT_NO1) {
        limitAngle.upperLimit = 300;
        limitAngle.lowerLimit = -1500;
    } else if (aircraftInfoBaseInfo.mountPosition == DJI_MOUNT_POSITION_PAYLOAD_PORT_NO2) {
        limitAngle.upperLimit = 1500;
        limitAngle.lowerLimit = -300;
    } else if (aircraftInfoBaseInfo.mountPosition == DJI_MOUNT_POSITION_PAYLOAD_PORT_NO3) {
        limitAngle.upperLimit = 1500;
        limitAngle.lowerLimit = -1500;
    } else {
        USER_LOG_WARN("unknown mounposition.");
        limitAngle.upperLimit = 300;
        limitAngle.lowerLimit = -1500;
    }

    USER_LOG_INFO("Set joint angle limit of yaw axis, upperLimit %d, lowerLimit %d", limitAngle.upperLimit, limitAngle.lowerLimit);
    djiStat = DjiXPort_SetLimitAngleSync(DJI_XPORT_LIMIT_ANGLE_CATEGORY_YAW_JOINT_ANGLE, limitAngle);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("set yaw joint angle limit angle for XPort error: 0x%08llX.", djiStat);
        return djiStat;
    }

    djiStat = DjiXPort_SetGimbalModeSync(DJI_GIMBAL_MODE_FREE);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("set XPort gimbal mode error: 0x%08llX.", djiStat);
        return djiStat;
    }

    djiStat = DjiXPort_ResetSync(DJI_GIMBAL_RESET_MODE_PITCH_AND_YAW);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("reset XPort gimbal error: 0x%08llX.", djiStat);
        return djiStat;
    }

    if (osalHandler->TaskCreate("user_xport_task", UserXPort_Task, XPORT_TASK_STACK_SIZE, NULL, &s_userXPortThread) !=
        DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("user XPort task create error.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiTest_XPortDeInit(void)
{
    T_DjiReturnCode djiStat;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    djiStat = osalHandler->TaskDestroy(s_userXPortThread);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Destroy test xport thread error: 0x%08llX.", djiStat);
        return djiStat;
    }

    djiStat = osalHandler->MutexDestroy(s_userXPortMutex);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Destroy test xport mutex error: 0x%08llX.", djiStat);
        return djiStat;
    }

    djiStat = DjiXPort_DeInit();
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("XPort de-init error: 0x%08llX.", djiStat);
        return djiStat;
    }

    s_isUserXPortInited = false;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiTest_XPortGetSystemState(T_DjiGimbalSystemState *systemState)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    if (s_isUserXPortInited != true || s_isUserXPortSystemStateVaild != true) {
        USER_LOG_ERROR("user XPort has not inited.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    returnCode = osalHandler->MutexLock(s_userXPortMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("user XPort mutex lock error: 0x%08llX.", returnCode);
        return returnCode;
    }

    memcpy(systemState, &s_userXPortSystemState, sizeof(T_DjiGimbalSystemState));

    returnCode = osalHandler->MutexUnlock(s_userXPortMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("user XPort mutex unlock error: 0x%08llX.", returnCode);
        return returnCode;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/* Private functions definition-----------------------------------------------*/
#ifndef __CC_ARM
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wreturn-type"
#endif

static void *UserXPort_Task(void *arg)
{
    T_DjiReturnCode djiStat;
    T_DjiXPortLimitAngle limitAngle = {0};
    float opticalZoomFactor = 1.0f;
    float digitalZoomFactor = 1.0f;
    int step = 0;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    USER_UTIL_UNUSED(arg);

    while (1) {
        osalHandler->TaskSleepMs(1000 / XPORT_TASK_FREQ);
        step++;

        if (USER_UTIL_IS_WORK_TURN(step, 1, XPORT_TASK_FREQ)) {
            djiStat = DjiXPort_GetLimitAngleSync(DJI_XPORT_LIMIT_ANGLE_CATEGORY_ROLL_JOINT_ANGLE, &limitAngle);
            if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("get roll joint angle limit angle from XPort error: 0x%08llX.", djiStat);
                continue;
            }

            USER_LOG_DEBUG("roll joint angle limit angle of XPort: upper limit %d, lower limit %d.",
                           limitAngle.upperLimit, limitAngle.lowerLimit);

            djiStat = DjiXPort_GetLimitAngleSync(DJI_XPORT_LIMIT_ANGLE_CATEGORY_PITCH_JOINT_ANGLE, &limitAngle);
            if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("get pitch joint angle limit angle from XPort error: 0x%08llX.", djiStat);
                continue;
            }

            USER_LOG_DEBUG("pitch joint angle limit angle of XPort: upper limit %d, lower limit %d.",
                           limitAngle.upperLimit, limitAngle.lowerLimit);

            djiStat = DjiXPort_GetLimitAngleSync(DJI_XPORT_LIMIT_ANGLE_CATEGORY_PITCH_EULER_ANGLE, &limitAngle);
            if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("get pitch euler angle limit angle from XPort error: 0x%08llX.", djiStat);
                continue;
            }

            USER_LOG_DEBUG("pitch euler angle limit angle of XPort: upper limit %d, lower limit %d.",
                           limitAngle.upperLimit, limitAngle.lowerLimit);

            djiStat = DjiXPort_GetLimitAngleSync(DJI_XPORT_LIMIT_ANGLE_CATEGORY_PITCH_EULER_ANGLE_EXTENSION,
                                                 &limitAngle);
            if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("get pitch extension euler angle limit angle from XPort error: 0x%08llX.",
                               djiStat);
                continue;
            }

            USER_LOG_DEBUG("pitch extension euler angle limit angle of XPort: upper limit %d, lower limit %d.",
                           limitAngle.upperLimit, limitAngle.lowerLimit);

            djiStat = DjiXPort_GetLimitAngleSync(DJI_XPORT_LIMIT_ANGLE_CATEGORY_YAW_JOINT_ANGLE, &limitAngle);
            if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("get yaw joint angle limit angle from XPort error: 0x%08llX.", djiStat);
                continue;
            }

            USER_LOG_DEBUG("yaw joint angle limit angle of XPort: upper limit %d, lower limit %d.",
                           limitAngle.upperLimit, limitAngle.lowerLimit);
        }

        if (USER_UTIL_IS_WORK_TURN(step, 10, XPORT_TASK_FREQ)) {
            if (DjiTest_CameraIsInited()) {
                djiStat = DjiTest_CameraGetOpticalZoomFactor(&opticalZoomFactor);
                if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    USER_LOG_ERROR("get camera optical zoom factor error: %d.", djiStat);
                    continue;
                }

                djiStat = DjiTest_CameraGetDigitalZoomFactor(&digitalZoomFactor);
                if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    USER_LOG_ERROR("get camera digital zoom factor error: %d.", djiStat);
                    continue;
                }

                djiStat = DjiXPort_SetSpeedConversionFactor(1 / (opticalZoomFactor * digitalZoomFactor));
                if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    USER_LOG_ERROR("set speed conversion factor error: %d.", djiStat);
                    continue;
                }
            }
        }
    }
}

#ifndef __CC_ARM
#pragma GCC diagnostic pop
#endif

static T_DjiReturnCode ReceiveXPortSystemState(T_DjiGimbalSystemState systemState)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    returnCode = osalHandler->MutexLock(s_userXPortMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("user XPort mutex lock error: 0x%08llX.", returnCode);
        return returnCode;
    }

    s_isUserXPortSystemStateVaild = true;
    memcpy(&s_userXPortSystemState, &systemState, sizeof(T_DjiGimbalSystemState));

    returnCode = osalHandler->MutexUnlock(s_userXPortMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("user XPort mutex unlock error: 0x%08llX.", returnCode);
        return returnCode;
    }

    USER_LOG_DEBUG("receive XPort system state: mounted upward flag %d, gimbal mode %d.",
                   systemState.mountedUpward, systemState.gimbalMode);

    USER_LOG_DEBUG("XPort fine tune: %d %d %d.", systemState.fineTuneAngle.pitch,
                   systemState.fineTuneAngle.roll, systemState.fineTuneAngle.yaw);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode ReceiveXPortAttitudeInformation(T_DjiGimbalAttitudeInformation attitudeInformation)
{
    USER_LOG_DEBUG("receive XPort attitude information:");
    USER_LOG_DEBUG("XPort attitude: pitch %d, roll %d, yaw %d.", attitudeInformation.attitude.pitch,
                   attitudeInformation.attitude.roll, attitudeInformation.attitude.yaw);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
