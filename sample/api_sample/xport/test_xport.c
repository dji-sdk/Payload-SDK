/**
 ********************************************************************
 * @file    test_xport.c
 * @version V2.0.0
 * @date    2019/10/16
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
#include <psdk_gimbal.h>
#include "test_xport.h"
#include "psdk_logger.h"
#include "psdk_platform.h"
#include "utils/util_misc.h"
#include "psdk_aircraft_info.h"
#include "camera_emu/test_payload_cam_emu.h"

/* Private constants ---------------------------------------------------------*/
#define XPORT_TASK_FREQ             (10)
#define XPORT_TASK_STACK_SIZE       (2048)

/* Private types -------------------------------------------------------------*/


/* Private functions declaration ---------------------------------------------*/
static void *UserXPort_Task(void *arg);
static T_PsdkReturnCode ReceiveXPortSystemState(T_PsdkGimbalSystemState systemState);
static T_PsdkReturnCode ReceiveXPortAttitudeInformation(T_PsdkGimbalAttitudeInformation attitudeInformation);

/* Private variables ---------------------------------------------------------*/
static T_PsdkTaskHandle s_userXPortThread;
static T_PsdkMutexHandle s_userXPortMutex;
static T_PsdkGimbalSystemState s_userXPortSystemState = {0};
static bool s_isUserXPortInited = false;
static bool s_isUserXPortSystemStateVaild = false;

/* Exported functions definition ---------------------------------------------*/
T_PsdkReturnCode PsdkTest_XPortInit(void)
{
    T_PsdkReturnCode psdkStat;
    T_PsdkXPortLimitAngle limitAngle = {0};
    T_PsdkAircraftInfoBaseInfo aircraftInfoBaseInfo = {0};

    psdkStat = PsdkXPort_Init();
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("XPort init error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    s_isUserXPortInited = true;

    psdkStat = PsdkOsal_MutexCreate(&s_userXPortMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("user XPort mutex create error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    psdkStat = PsdkXPort_RegReceiveSystemStateCallback(ReceiveXPortSystemState);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("register receive XPort system state callback function error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    psdkStat = PsdkXPort_RegReceiveAttitudeInformationCallback(ReceiveXPortAttitudeInformation);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("register receive XPort attitude information callback function error: 0x%08llX.",
                                psdkStat);
        return psdkStat;
    }

    limitAngle.upperLimit = 300;
    limitAngle.lowerLimit = -1000;
    psdkStat = PsdkXPort_SetLimitAngleSync(PSDK_XPORT_LIMIT_ANGLE_CATEGORY_PITCH_JOINT_ANGLE, limitAngle);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("set pitch joint angle limit angle for XPort error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    limitAngle.upperLimit = 300;
    limitAngle.lowerLimit = -800;
    psdkStat = PsdkXPort_SetLimitAngleSync(PSDK_XPORT_LIMIT_ANGLE_CATEGORY_PITCH_EULER_ANGLE, limitAngle);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("set pitch euler angle limit angle for XPort error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    limitAngle.upperLimit = 300;
    limitAngle.lowerLimit = -1000;
    psdkStat = PsdkXPort_SetLimitAngleSync(PSDK_XPORT_LIMIT_ANGLE_CATEGORY_PITCH_EULER_ANGLE_EXTENSION, limitAngle);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("set pitch extension euler angle limit angle for XPort error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    psdkStat = PsdkAircraftInfo_GetBaseInfo(&aircraftInfoBaseInfo);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("get aircraft base information error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    if (aircraftInfoBaseInfo.payloadMountPosition == PSDK_AIRCRAFT_INFO_PAYLOAD_MOUNT_POSITION_NO1) {
        limitAngle.upperLimit = 300;
        limitAngle.lowerLimit = -1500;
    } else if (aircraftInfoBaseInfo.payloadMountPosition == PSDK_AIRCRAFT_INFO_PAYLOAD_MOUNT_POSITION_NO2) {
        limitAngle.upperLimit = 1500;
        limitAngle.lowerLimit = -300;
    } else if (aircraftInfoBaseInfo.payloadMountPosition == PSDK_AIRCRAFT_INFO_PAYLOAD_MOUNT_POSITION_NO3) {
        limitAngle.upperLimit = 1500;
        limitAngle.lowerLimit = -1500;
    } else {
        PsdkLogger_UserLogWarn("payload mount position is unknown.");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    psdkStat = PsdkXPort_SetLimitAngleSync(PSDK_XPORT_LIMIT_ANGLE_CATEGORY_YAW_JOINT_ANGLE, limitAngle);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("set yaw joint angle limit angle for XPort error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    psdkStat = PsdkXPort_SetGimbalModeSync(PSDK_GIMBAL_MODE_FREE);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("set XPort gimbal mode error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    psdkStat = PsdkXPort_ResetSync(PSDK_GIMBAL_RESET_MODE_PITCH_AND_YAW);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("reset XPort gimbal error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    if (PsdkOsal_TaskCreate(&s_userXPortThread, UserXPort_Task, "user_xport_task", XPORT_TASK_STACK_SIZE, NULL) !=
        PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("user XPort task create error.");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_PsdkReturnCode PsdkTest_XPortDeInit(void)
{
    T_PsdkReturnCode psdkStat;

    psdkStat = PsdkOsal_TaskDestroy(s_userXPortThread);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Destroy test xport thread error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    psdkStat = PsdkOsal_MutexDestroy(s_userXPortMutex);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Destroy test xport mutex error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    psdkStat = PsdkXPort_DeInit();
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("XPort de-init error: 0x%08llX.", psdkStat);
        return psdkStat;
    }

    s_isUserXPortInited = false;

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_PsdkReturnCode PsdkTest_XPortGetSystemState(T_PsdkGimbalSystemState *systemState)
{
    T_PsdkReturnCode returnCode;

    if (s_isUserXPortInited != true || s_isUserXPortSystemStateVaild != true) {
        PsdkLogger_UserLogWarn("user XPort has not inited.");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT_IN_CURRENT_STATE;
    }

    returnCode = PsdkOsal_MutexLock(s_userXPortMutex);
    if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("user XPort mutex lock error: 0x%08llX.", returnCode);
        return returnCode;
    }

    memcpy(systemState, &s_userXPortSystemState, sizeof(T_PsdkGimbalSystemState));

    returnCode = PsdkOsal_MutexUnlock(s_userXPortMutex);
    if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("user XPort mutex unlock error: 0x%08llX.", returnCode);
        return returnCode;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/* Private functions definition-----------------------------------------------*/
#ifndef __CC_ARM
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wreturn-type"
#endif

static void *UserXPort_Task(void *arg)
{
    T_PsdkReturnCode psdkStat;
    T_PsdkXPortLimitAngle limitAngle = {0};
    float opticalZoomFactor = 1.0f;
    float digitalZoomFactor = 1.0f;
    int step = 0;

    USER_UTIL_UNUSED(arg);

    while (1) {
        PsdkOsal_TaskSleepMs(1000 / XPORT_TASK_FREQ);
        step++;

        if (USER_UTIL_IS_WORK_TURN(step, 1, XPORT_TASK_FREQ)) {
            psdkStat = PsdkXPort_GetLimitAngleSync(PSDK_XPORT_LIMIT_ANGLE_CATEGORY_ROLL_JOINT_ANGLE, &limitAngle);
            if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                PsdkLogger_UserLogError("get roll joint angle limit angle from XPort error: 0x%08llX.", psdkStat);
                continue;
            }

            PsdkLogger_UserLogDebug("roll joint angle limit angle of XPort: upper limit %d, lower limit %d.",
                                    limitAngle.upperLimit, limitAngle.lowerLimit);

            psdkStat = PsdkXPort_GetLimitAngleSync(PSDK_XPORT_LIMIT_ANGLE_CATEGORY_PITCH_JOINT_ANGLE, &limitAngle);
            if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                PsdkLogger_UserLogError("get pitch joint angle limit angle from XPort error: 0x%08llX.", psdkStat);
                continue;
            }

            PsdkLogger_UserLogDebug("pitch joint angle limit angle of XPort: upper limit %d, lower limit %d.",
                                    limitAngle.upperLimit, limitAngle.lowerLimit);

            psdkStat = PsdkXPort_GetLimitAngleSync(PSDK_XPORT_LIMIT_ANGLE_CATEGORY_PITCH_EULER_ANGLE, &limitAngle);
            if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                PsdkLogger_UserLogError("get pitch euler angle limit angle from XPort error: 0x%08llX.", psdkStat);
                continue;
            }

            PsdkLogger_UserLogDebug("pitch euler angle limit angle of XPort: upper limit %d, lower limit %d.",
                                    limitAngle.upperLimit, limitAngle.lowerLimit);

            psdkStat = PsdkXPort_GetLimitAngleSync(PSDK_XPORT_LIMIT_ANGLE_CATEGORY_PITCH_EULER_ANGLE_EXTENSION,
                                                   &limitAngle);
            if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                PsdkLogger_UserLogError("get pitch extension euler angle limit angle from XPort error: 0x%08llX.",
                                        psdkStat);
                continue;
            }

            PsdkLogger_UserLogDebug("pitch extension euler angle limit angle of XPort: upper limit %d, lower limit %d.",
                                    limitAngle.upperLimit, limitAngle.lowerLimit);

            psdkStat = PsdkXPort_GetLimitAngleSync(PSDK_XPORT_LIMIT_ANGLE_CATEGORY_YAW_JOINT_ANGLE, &limitAngle);
            if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                PsdkLogger_UserLogError("get yaw joint angle limit angle from XPort error: 0x%08llX.", psdkStat);
                continue;
            }

            PsdkLogger_UserLogDebug("yaw joint angle limit angle of XPort: upper limit %d, lower limit %d.",
                                    limitAngle.upperLimit, limitAngle.lowerLimit);
        }

        if (USER_UTIL_IS_WORK_TURN(step, 10, XPORT_TASK_FREQ)) {
            if (PsdkTest_CameraIsInited()) {
                psdkStat = PsdkTest_CameraGetOpticalZoomFactor(&opticalZoomFactor);
                if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    PsdkLogger_UserLogError("get camera optical zoom factor error: %d.", psdkStat);
                    continue;
                }

                psdkStat = PsdkTest_CameraGetDigitalZoomFactor(&digitalZoomFactor);
                if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    PsdkLogger_UserLogError("get camera digital zoom factor error: %d.", psdkStat);
                    continue;
                }

                psdkStat = PsdkXPort_SetSpeedConversionFactor(1 / (opticalZoomFactor * digitalZoomFactor));
                if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    PsdkLogger_UserLogError("set speed conversion factor error: %d.", psdkStat);
                    continue;
                }
            }
        }
    }
}

#ifndef __CC_ARM
#pragma GCC diagnostic pop
#endif

static T_PsdkReturnCode ReceiveXPortSystemState(T_PsdkGimbalSystemState systemState)
{
    T_PsdkReturnCode returnCode;

    returnCode = PsdkOsal_MutexLock(s_userXPortMutex);
    if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("user XPort mutex lock error: 0x%08llX.", returnCode);
        return returnCode;
    }

    s_isUserXPortSystemStateVaild = true;
    memcpy(&s_userXPortSystemState, &systemState, sizeof(T_PsdkGimbalSystemState));

    returnCode = PsdkOsal_MutexUnlock(s_userXPortMutex);
    if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("user XPort mutex unlock error: 0x%08llX.", returnCode);
        return returnCode;
    }

    PsdkLogger_UserLogDebug("receive XPort system state: mounted upward flag %d, gimbal mode %d.",
                            systemState.mountedUpward, systemState.gimbalMode);

    PsdkLogger_UserLogDebug("XPort fine tune: %d %d %d.", systemState.fineTuneAngle.pitch,
                            systemState.fineTuneAngle.roll, systemState.fineTuneAngle.yaw);

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode ReceiveXPortAttitudeInformation(T_PsdkGimbalAttitudeInformation attitudeInformation)
{
    PsdkLogger_UserLogDebug("receive XPort attitude information:");
    PsdkLogger_UserLogDebug("XPort attitude: pitch %d, roll %d, yaw %d.", attitudeInformation.attitude.pitch,
                            attitudeInformation.attitude.roll, attitudeInformation.attitude.yaw);

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
