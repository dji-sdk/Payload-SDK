/**
 ********************************************************************
 * @file    test_payload_gimbal_emu.c
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
#include "math.h"
#include <dji_gimbal.h>
#include "test_payload_gimbal_emu.h"
#include "dji_fc_subscription.h"
#include "dji_logger.h"
#include "dji_platform.h"
#include "utils/util_misc.h"
#include "widget_interaction_test/test_widget_interaction.h"

/* Private constants ---------------------------------------------------------*/
#define PAYLOAD_GIMBAL_EMU_TASK_STACK_SIZE  (2048)
#define PAYLOAD_GIMBAL_TASK_FREQ            1000
#define PAYLOAD_GIMBAL_CALIBRATION_TIME_MS  2000
#define PAYLOAD_GIMBAL_MIN_ACTION_TIME      5

/* Private types -------------------------------------------------------------*/
typedef enum {
    TEST_GIMBAL_CONTROL_TYPE_UNKNOWN = 0,
    TEST_GIMBAL_CONTROL_TYPE_SPEED = 1,
    TEST_GIMBAL_CONTROL_TYPE_ANGLE = 2,
} E_TestGimbalControlType;

/* Private functions declaration ---------------------------------------------*/
static void *UserGimbal_Task(void *arg);
static T_DjiReturnCode GetSystemState(T_DjiGimbalSystemState *systemState);
static T_DjiReturnCode GetAttitudeInformation(T_DjiGimbalAttitudeInformation *attitudeInformation);
static T_DjiReturnCode GetCalibrationState(T_DjiGimbalCalibrationState *calibrationState);
static T_DjiReturnCode GetRotationSpeed(T_DjiAttitude3d *rotationSpeed);
static T_DjiReturnCode GetJointAngle(T_DjiAttitude3d *jointAngle);
static T_DjiReturnCode StartCalibrate(void);
static T_DjiReturnCode SetControllerSmoothFactor(uint8_t smoothingFactor, E_DjiGimbalAxis axis);
static T_DjiReturnCode SetPitchRangeExtensionEnabled(bool enabledFlag);
static T_DjiReturnCode SetControllerMaxSpeedPercentage(uint8_t maxSpeedPercentage, E_DjiGimbalAxis axis);
static T_DjiReturnCode RestoreFactorySettings(void);
static T_DjiReturnCode SetMode(E_DjiGimbalMode mode);
static T_DjiReturnCode Reset(E_DjiGimbalResetMode mode);
static T_DjiReturnCode FineTuneAngle(T_DjiAttitude3d fineTuneAngle);
static T_DjiReturnCode DjiTest_GimbalAngleLegalization(T_DjiAttitude3f *attitude, T_DjiAttitude3d aircraftAttitude,
                                                       T_DjiGimbalReachLimitFlag *reachLimitFlag);
static T_DjiReturnCode
DjiTest_GimbalCalculateSpeed(T_DjiAttitude3d originalAttitude, T_DjiAttitude3d targetAttitude, uint16_t actionTime,
                             T_DjiAttitude3d *speed);
static void DjiTest_GimbalSpeedLegalization(T_DjiAttitude3d *speed);
static T_DjiReturnCode DjiTest_GimbalCalculateGroundAttitudeBaseQuaternion(T_DjiFcSubscriptionQuaternion quaternion,
                                                                           T_DjiAttitude3d *attitude);

/* Private variables ---------------------------------------------------------*/
static T_DjiTaskHandle s_userGimbalThread;
static T_DjiGimbalCommonHandler s_commonHandler = {0};
static T_DjiGimbalSystemState s_systemState = {0};
static bool s_rotatingFlag = false;
static T_DjiMutexHandle s_commonMutex = {0};

static T_DjiGimbalAttitudeInformation s_attitudeInformation = {0}; // unit: 0.1 degree, ground coordination
static T_DjiAttitude3f s_attitudeHighPrecision = {0}; // unit: 0.1 degree, ground coordination
static T_DjiGimbalCalibrationState s_calibrationState = {0};
static T_DjiAttitude3d s_targetAttitude = {0}; // unit: 0.1 degree, ground coordination
static T_DjiAttitude3d s_speed = {0}; // unit: 0.1 degree/s, ground coordination
static T_DjiAttitude3d s_aircraftAttitude = {0}; // unit: 0.1 degree, ground coordination
static T_DjiAttitude3d s_lastAircraftAttitude = {0}; // unit: 0.1 degree, ground coordination
static E_TestGimbalControlType s_controlType = TEST_GIMBAL_CONTROL_TYPE_UNKNOWN;
static const T_DjiAttitude3d s_jointAngleLimitMin = {-1200, -100, -1800}; // unit: 0.1 degree
static const T_DjiAttitude3d s_jointAngleLimitMax = {300, 100, 1800}; // unit: 0.1 degree
static const T_DjiAttitude3d s_eulerAngleLimitMin = {-900, -100, -1800}; // unit: 0.1 degree
static const T_DjiAttitude3d s_eulerAngleLimitMax = {100, 100, 1800}; // unit: 0.1 degree
static const int32_t s_pitchEulerAngleExtensionMin = -1200; // unit: 0.1 degree
static const int32_t s_pitchEulerAngleExtensionMax = 300; // unit: 0.1 degree
static const T_DjiAttitude3d s_speedLimit = {1800, 1800, 1800}; // unit: 0.1 degree/s
static uint32_t s_calibrationStartTime = 0; // unit: ms
static T_DjiMutexHandle s_attitudeMutex = NULL;
static T_DjiMutexHandle s_calibrationMutex = NULL;

/* Exported functions definition ---------------------------------------------*/
/**
 * @brief
 * @note Gimbal sample rely on aircraft quaternion.
 * @return
 */
T_DjiReturnCode DjiTest_GimbalStartService(void)
{
    T_DjiReturnCode djiStat;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    s_systemState.resettingFlag = false;
    s_systemState.mountedUpward = false;
    s_systemState.blockingFlag = false;
    s_systemState.pitchRangeExtensionEnabledFlag = false;

    s_calibrationState.calibratingFlag = false;
    s_calibrationState.lastCalibrationResult = true;

    s_commonHandler.GetSystemState = GetSystemState;
    s_commonHandler.GetAttitudeInformation = GetAttitudeInformation;
    s_commonHandler.GetCalibrationState = GetCalibrationState;
    s_commonHandler.GetRotationSpeed = GetRotationSpeed;
    s_commonHandler.GetJointAngle = GetJointAngle;
    s_commonHandler.Rotate = DjiTest_GimbalRotate;
    s_commonHandler.StartCalibrate = StartCalibrate;
    s_commonHandler.SetControllerSmoothFactor = SetControllerSmoothFactor;
    s_commonHandler.SetPitchRangeExtensionEnabled = SetPitchRangeExtensionEnabled;
    s_commonHandler.SetControllerMaxSpeedPercentage = SetControllerMaxSpeedPercentage;
    s_commonHandler.RestoreFactorySettings = RestoreFactorySettings;
    s_commonHandler.SetMode = SetMode;
    s_commonHandler.Reset = Reset;
    s_commonHandler.FineTuneAngle = FineTuneAngle;

    djiStat = DjiFcSubscription_Init();
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("init data subscription module error.");
        return djiStat;
    }

    if (osalHandler->MutexCreate(&s_commonMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mutex create error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    if (osalHandler->MutexCreate(&s_attitudeMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mutex create error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    if (osalHandler->MutexCreate(&s_calibrationMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mutex create error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    djiStat = DjiGimbal_Init();
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("init gimbal module error: 0x%08llX", djiStat);
        return djiStat;
    }

    djiStat = DjiGimbal_RegCommonHandler(&s_commonHandler);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("gimbal register common handler error: 0x%08llX", djiStat);
        return djiStat;
    }

    if (osalHandler->TaskCreate("user_gimbal_task", UserGimbal_Task,
                                PAYLOAD_GIMBAL_EMU_TASK_STACK_SIZE, NULL, &s_userGimbalThread) !=
        DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("user gimbal task create error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiTest_GimbalDeInit(void)
{
    T_DjiReturnCode djiStat;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    djiStat = osalHandler->TaskDestroy(s_userGimbalThread);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Destroy test gimbal thread error: 0x%08llX.", djiStat);
        return djiStat;
    }

    djiStat = DjiGimbal_DeInit();
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Deinit gimbal module error: 0x%08llX.", djiStat);
        return djiStat;
    }

    if (osalHandler->MutexDestroy(s_calibrationMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mutex destroy error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    if (osalHandler->MutexDestroy(s_attitudeMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mutex destroy error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    if (osalHandler->MutexDestroy(s_commonMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mutex destroy error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiTest_GimbalRotate(E_DjiGimbalRotationMode rotationMode,
                                     T_DjiGimbalRotationProperty rotationProperty,
                                     T_DjiAttitude3d rotationValue)
{
    T_DjiReturnCode djiStat;
    T_DjiReturnCode returnCode = DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    T_DjiAttitude3d targetAttitudeDTemp = {0};
    T_DjiAttitude3f targetAttitudeFTemp = {0};
    T_DjiAttitude3d speedTemp = {0};
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    USER_LOG_DEBUG("gimbal rotation value invalid flag: pitch %d, roll %d, yaw %d.",
                   rotationProperty.rotationValueInvalidFlag.pitch,
                   rotationProperty.rotationValueInvalidFlag.roll,
                   rotationProperty.rotationValueInvalidFlag.yaw);

    if (osalHandler->MutexLock(s_attitudeMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mutex lock error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    if (osalHandler->MutexLock(s_commonMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mutex lock error");
        returnCode = DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
        goto out2;
    }

    switch (rotationMode) {
        case DJI_GIMBAL_ROTATION_MODE_RELATIVE_ANGLE:
            USER_LOG_INFO("gimbal relative rotate angle: pitch %d, roll %d, yaw %d.", rotationValue.pitch,
                          rotationValue.roll, rotationValue.yaw);
            USER_LOG_DEBUG("gimbal relative rotate action time: %d.",
                           rotationProperty.relativeAngleRotation.actionTime);

            if (s_rotatingFlag == true) {
                USER_LOG_WARN("gimbal is rotating.");
                goto out1;
            }

            targetAttitudeDTemp.pitch =
                rotationProperty.rotationValueInvalidFlag.pitch == true ? s_attitudeInformation.attitude.pitch : (
                    s_attitudeInformation.attitude.pitch + rotationValue.pitch);
            targetAttitudeDTemp.roll =
                rotationProperty.rotationValueInvalidFlag.roll == true ? s_attitudeInformation.attitude.roll : (
                    s_attitudeInformation.attitude.roll + rotationValue.roll);
            targetAttitudeDTemp.yaw =
                rotationProperty.rotationValueInvalidFlag.yaw == true ? s_attitudeInformation.attitude.yaw : (
                    s_attitudeInformation.attitude.yaw + rotationValue.yaw);

            targetAttitudeFTemp.pitch = targetAttitudeDTemp.pitch;
            targetAttitudeFTemp.roll = targetAttitudeDTemp.roll;
            targetAttitudeFTemp.yaw = targetAttitudeDTemp.yaw;
            DjiTest_GimbalAngleLegalization(&targetAttitudeFTemp, s_aircraftAttitude, NULL);
            targetAttitudeDTemp.pitch = targetAttitudeFTemp.pitch;
            targetAttitudeDTemp.roll = targetAttitudeFTemp.roll;
            targetAttitudeDTemp.yaw = targetAttitudeFTemp.yaw;

            s_targetAttitude = targetAttitudeDTemp;
            s_rotatingFlag = true;
            s_controlType = TEST_GIMBAL_CONTROL_TYPE_ANGLE;

            djiStat = DjiTest_GimbalCalculateSpeed(s_attitudeInformation.attitude, s_targetAttitude,
                                                   rotationProperty.relativeAngleRotation.actionTime, &s_speed);
            if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("calculate gimbal rotation speed error: 0x%08llX.", djiStat);
                returnCode = djiStat;
                goto out1;
            }

            break;
        case DJI_GIMBAL_ROTATION_MODE_ABSOLUTE_ANGLE:
            DjiTest_WidgetLogAppend("abs mode (%d  %d %d)", rotationValue.pitch, rotationValue.roll, rotationValue.yaw);
            USER_LOG_INFO("gimbal absolute rotate angle: pitch %d, roll %d, yaw %d.", rotationValue.pitch,
                          rotationValue.roll, rotationValue.yaw);
            USER_LOG_DEBUG("gimbal absolute rotate action time: %d.",
                           rotationProperty.absoluteAngleRotation.actionTime);
            if (rotationProperty.absoluteAngleRotation.jointAngleValid) {
                USER_LOG_DEBUG("gimbal joint angles: pitch %d, roll %d, yaw %d.",
                               rotationProperty.absoluteAngleRotation.jointAngle.pitch,
                               rotationProperty.absoluteAngleRotation.jointAngle.roll,
                               rotationProperty.absoluteAngleRotation.jointAngle.yaw);
            }

            if (s_rotatingFlag == true) {
                USER_LOG_WARN("gimbal is rotating.");
                goto out1;
            }

            targetAttitudeDTemp.pitch =
                rotationProperty.rotationValueInvalidFlag.pitch == true ? s_attitudeInformation.attitude.pitch
                                                                        : rotationValue.pitch;
            targetAttitudeDTemp.roll =
                rotationProperty.rotationValueInvalidFlag.roll == true ? s_attitudeInformation.attitude.roll
                                                                       : rotationValue.roll;
            targetAttitudeDTemp.yaw =
                rotationProperty.rotationValueInvalidFlag.yaw == true ? s_attitudeInformation.attitude.yaw
                                                                      : rotationValue.yaw;

            targetAttitudeFTemp.pitch = targetAttitudeDTemp.pitch;
            targetAttitudeFTemp.roll = targetAttitudeDTemp.roll;
            targetAttitudeFTemp.yaw = targetAttitudeDTemp.yaw;
            DjiTest_GimbalAngleLegalization(&targetAttitudeFTemp, s_aircraftAttitude, NULL);
            targetAttitudeDTemp.pitch = targetAttitudeFTemp.pitch;
            targetAttitudeDTemp.roll = targetAttitudeFTemp.roll;
            targetAttitudeDTemp.yaw = targetAttitudeFTemp.yaw;

            s_targetAttitude = targetAttitudeDTemp;
            s_rotatingFlag = true;
            s_controlType = TEST_GIMBAL_CONTROL_TYPE_ANGLE;

            djiStat = DjiTest_GimbalCalculateSpeed(s_attitudeInformation.attitude, s_targetAttitude,
                                                   rotationProperty.absoluteAngleRotation.actionTime, &s_speed);
            if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("calculate gimbal rotation speed error: 0x%08llX.", djiStat);
                returnCode = djiStat;
                goto out1;
            }

            break;
        case DJI_GIMBAL_ROTATION_MODE_SPEED:
            DjiTest_WidgetLogAppend("speed mode (%d  %d %d)", rotationValue.pitch, rotationValue.roll, rotationValue.yaw);
            USER_LOG_INFO("gimbal rotate speed: pitch %d, roll %d, yaw %d.", rotationValue.pitch,
                          rotationValue.roll, rotationValue.yaw);

            if (s_rotatingFlag == true && s_controlType == TEST_GIMBAL_CONTROL_TYPE_ANGLE) {
                USER_LOG_WARN("gimbal is rotating.");
                goto out1;
            }

            memcpy(&speedTemp, &rotationValue, sizeof(T_DjiAttitude3d));
            DjiTest_GimbalSpeedLegalization(&speedTemp);
            s_speed = speedTemp;

            if (rotationValue.pitch != 0 || rotationValue.roll != 0 || rotationValue.yaw != 0) {
                s_rotatingFlag = true;
                s_controlType = TEST_GIMBAL_CONTROL_TYPE_SPEED;
            } else {
                s_rotatingFlag = false;
            }

            break;
        default:
            USER_LOG_ERROR("gimbal rotation mode invalid: %d.", rotationMode);
            returnCode = DJI_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT;
            goto out1;
    }

out1:
    if (osalHandler->MutexUnlock(s_commonMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mutex unlock error");
        returnCode = DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
        goto out2;
    }

out2:
    if (osalHandler->MutexUnlock(s_attitudeMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mutex unlock error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return returnCode;
}

/* Private functions definition-----------------------------------------------*/
#ifndef __CC_ARM
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wreturn-type"
#endif

static void *UserGimbal_Task(void *arg)
{
    T_DjiReturnCode djiStat;
    static uint32_t step = 0;
    T_DjiFcSubscriptionQuaternion quaternion = {0};
    T_DjiDataTimestamp timestamp = {0};
    T_DjiAttitude3f nextAttitude = {0};
    T_DjiAttitude3f attitudeFTemp = {0};
    uint32_t currentTime = 0;
    uint32_t progressTemp = 0;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    USER_UTIL_UNUSED(arg);

    djiStat = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_QUATERNION, DJI_DATA_SUBSCRIPTION_TOPIC_10_HZ,
                                               NULL);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        if (djiStat == DJI_ERROR_SUBSCRIPTION_MODULE_CODE_TOPIC_DUPLICATE) {
            USER_LOG_WARN("Subscribe topic quaternion duplicate.");
        } else {
            USER_LOG_ERROR("Subscribe topic quaternion error.");
        }
    } else {
        USER_LOG_DEBUG("Subscribe topic quaternion success.");
    }

    while (1) {
        osalHandler->TaskSleepMs(1000 / PAYLOAD_GIMBAL_TASK_FREQ);
        step++;

        if (osalHandler->MutexLock(s_attitudeMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("mutex lock error");
            continue;
        }

        if (osalHandler->MutexLock(s_commonMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("mutex lock error");
            goto out2;
        }

        if (USER_UTIL_IS_WORK_TURN(step, 1, PAYLOAD_GIMBAL_TASK_FREQ)) {
            USER_LOG_DEBUG("gimbal attitude: pitch %d, roll %d, yaw %d.", s_attitudeInformation.attitude.pitch,
                           s_attitudeInformation.attitude.roll, s_attitudeInformation.attitude.yaw);

            USER_LOG_DEBUG("gimbal fine tune: pitch %d, roll %d, yaw %d.", s_systemState.fineTuneAngle.pitch,
                           s_systemState.fineTuneAngle.roll, s_systemState.fineTuneAngle.yaw);
        }

        // update aircraft attitude
        if (USER_UTIL_IS_WORK_TURN(step, 50, PAYLOAD_GIMBAL_TASK_FREQ)) {
            djiStat = DjiFcSubscription_GetLatestValueOfTopic(DJI_FC_SUBSCRIPTION_TOPIC_QUATERNION,
                                                              (uint8_t *) &quaternion,
                                                              sizeof(T_DjiFcSubscriptionQuaternion),
                                                              &timestamp);
            if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("get topic quaternion value error.");
            }

            djiStat = DjiTest_GimbalCalculateGroundAttitudeBaseQuaternion(quaternion, &s_aircraftAttitude);
            if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("calculate and update aircraft attitude error.");
            }
        }

        // stable control
        switch (s_systemState.gimbalMode) {
            case DJI_GIMBAL_MODE_FREE:
                break;
            case DJI_GIMBAL_MODE_FPV:
                s_attitudeInformation.attitude.roll += (s_aircraftAttitude.roll - s_lastAircraftAttitude.roll);
                s_attitudeInformation.attitude.yaw += (s_aircraftAttitude.yaw - s_lastAircraftAttitude.yaw);

                s_attitudeHighPrecision.roll += (float) (s_aircraftAttitude.roll - s_lastAircraftAttitude.roll);
                s_attitudeHighPrecision.yaw += (float) (s_aircraftAttitude.yaw - s_lastAircraftAttitude.yaw);

                if (s_rotatingFlag == true && s_controlType == TEST_GIMBAL_CONTROL_TYPE_ANGLE) {
                    s_targetAttitude.roll += (s_aircraftAttitude.roll - s_lastAircraftAttitude.roll);
                    s_targetAttitude.yaw += (s_aircraftAttitude.yaw - s_lastAircraftAttitude.yaw);
                }
                break;
            case DJI_GIMBAL_MODE_YAW_FOLLOW:
                s_attitudeInformation.attitude.yaw += (s_aircraftAttitude.yaw - s_lastAircraftAttitude.yaw);

                s_attitudeHighPrecision.yaw += (float) (s_aircraftAttitude.yaw - s_lastAircraftAttitude.yaw);

                if (s_rotatingFlag == true && s_controlType == TEST_GIMBAL_CONTROL_TYPE_ANGLE) {
                    s_targetAttitude.yaw += (s_aircraftAttitude.yaw - s_lastAircraftAttitude.yaw);
                }
                break;
            default:
                USER_LOG_ERROR("gimbal mode invalid: %d.", s_systemState.gimbalMode);
        }
        s_lastAircraftAttitude = s_aircraftAttitude;

        attitudeFTemp.pitch = s_attitudeInformation.attitude.pitch;
        attitudeFTemp.roll = s_attitudeInformation.attitude.roll;
        attitudeFTemp.yaw = s_attitudeInformation.attitude.yaw;
        DjiTest_GimbalAngleLegalization(&attitudeFTemp, s_aircraftAttitude, &s_attitudeInformation.reachLimitFlag);
        s_attitudeInformation.attitude.pitch = attitudeFTemp.pitch;
        s_attitudeInformation.attitude.roll = attitudeFTemp.roll;
        s_attitudeInformation.attitude.yaw = attitudeFTemp.yaw;

        DjiTest_GimbalAngleLegalization(&s_attitudeHighPrecision, s_aircraftAttitude, NULL);

        attitudeFTemp.pitch = s_targetAttitude.pitch;
        attitudeFTemp.roll = s_targetAttitude.roll;
        attitudeFTemp.yaw = s_targetAttitude.yaw;
        DjiTest_GimbalAngleLegalization(&attitudeFTemp, s_aircraftAttitude, NULL);
        s_targetAttitude.pitch = attitudeFTemp.pitch;
        s_targetAttitude.roll = attitudeFTemp.roll;
        s_targetAttitude.yaw = attitudeFTemp.yaw;

        // rotation
        if (s_rotatingFlag != true)
            goto out1;

        nextAttitude.pitch =
            (float) s_attitudeHighPrecision.pitch + (float) s_speed.pitch / (float) PAYLOAD_GIMBAL_TASK_FREQ;
        nextAttitude.roll =
            (float) s_attitudeHighPrecision.roll + (float) s_speed.roll / (float) PAYLOAD_GIMBAL_TASK_FREQ;
        nextAttitude.yaw = (float) s_attitudeHighPrecision.yaw + (float) s_speed.yaw / (float) PAYLOAD_GIMBAL_TASK_FREQ;

        if (s_controlType == TEST_GIMBAL_CONTROL_TYPE_ANGLE) {
            nextAttitude.pitch =
                (nextAttitude.pitch - s_targetAttitude.pitch) * s_speed.pitch >= 0 ? s_targetAttitude.pitch
                                                                                   : nextAttitude.pitch;
            nextAttitude.roll = (nextAttitude.roll - s_targetAttitude.roll) * s_speed.roll >= 0 ? s_targetAttitude.roll
                                                                                                : nextAttitude.roll;
            nextAttitude.yaw =
                (nextAttitude.yaw - s_targetAttitude.yaw) * s_speed.yaw >= 0 ? s_targetAttitude.yaw : nextAttitude.yaw;
        }

        DjiTest_GimbalAngleLegalization(&nextAttitude, s_aircraftAttitude, &s_attitudeInformation.reachLimitFlag);
        s_attitudeInformation.attitude.pitch = nextAttitude.pitch;
        s_attitudeInformation.attitude.roll = nextAttitude.roll;
        s_attitudeInformation.attitude.yaw = nextAttitude.yaw;

        s_attitudeHighPrecision.pitch = nextAttitude.pitch;
        s_attitudeHighPrecision.roll = nextAttitude.roll;
        s_attitudeHighPrecision.yaw = nextAttitude.yaw;

        if (s_controlType == TEST_GIMBAL_CONTROL_TYPE_ANGLE) {
            if (memcmp(&s_attitudeInformation.attitude, &s_targetAttitude, sizeof(T_DjiAttitude3d)) == 0) {
                s_rotatingFlag = false;
            }
        } else if (s_controlType == TEST_GIMBAL_CONTROL_TYPE_SPEED) {
            if ((s_attitudeInformation.reachLimitFlag.pitch == true || s_speed.pitch == 0) &&
                (s_attitudeInformation.reachLimitFlag.roll == true || s_speed.roll == 0) &&
                (s_attitudeInformation.reachLimitFlag.yaw == true || s_speed.yaw == 0)) {
                s_rotatingFlag = false;
            }
        }

out1:
        if (osalHandler->MutexUnlock(s_commonMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("mutex unlock error");
            goto out2;
        }

out2:
        if (osalHandler->MutexUnlock(s_attitudeMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("mutex unlock error");
            continue;
        }

        djiStat = osalHandler->GetTimeMs(&currentTime);
        if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("get current time error: 0x%08llX.", djiStat);
            continue;
        }

        if (osalHandler->MutexLock(s_calibrationMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("mutex lock error");
            continue;
        }

        // calibration
        if (s_calibrationState.calibratingFlag != true)
            goto unlockCalibrationMutex;

        progressTemp = (currentTime - s_calibrationStartTime) * 100 / PAYLOAD_GIMBAL_CALIBRATION_TIME_MS;
        if (progressTemp >= 100) {
            s_calibrationState.calibratingFlag = false;
            s_calibrationState.currentCalibrationProgress = 100;
            s_calibrationState.currentCalibrationStage = DJI_GIMBAL_CALIBRATION_STAGE_COMPLETE;
        }

unlockCalibrationMutex:
        if (osalHandler->MutexUnlock(s_calibrationMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("mutex unlock error");
            continue;
        }
    }
}

#ifndef __CC_ARM
#pragma GCC diagnostic pop
#endif

static T_DjiReturnCode GetSystemState(T_DjiGimbalSystemState *systemState)
{
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    if (osalHandler->MutexLock(s_commonMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mutex lock error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    *systemState = s_systemState;

    if (osalHandler->MutexUnlock(s_commonMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mutex unlock error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode GetAttitudeInformation(T_DjiGimbalAttitudeInformation *attitudeInformation)
{
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    if (osalHandler->MutexLock(s_attitudeMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mutex lock error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    *attitudeInformation = s_attitudeInformation;

    if (osalHandler->MutexUnlock(s_attitudeMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mutex unlock error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode GetCalibrationState(T_DjiGimbalCalibrationState *calibrationState)
{
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    if (osalHandler->MutexLock(s_calibrationMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mutex lock error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    *calibrationState = s_calibrationState;

    if (osalHandler->MutexUnlock(s_calibrationMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mutex unlock error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode GetRotationSpeed(T_DjiAttitude3d *rotationSpeed)
{
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    if (osalHandler->MutexLock(s_attitudeMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mutex lock error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    *rotationSpeed = s_speed;

    if (osalHandler->MutexUnlock(s_attitudeMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mutex unlock error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode GetJointAngle(T_DjiAttitude3d *jointAngle)
{
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    if (osalHandler->MutexLock(s_attitudeMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mutex lock error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    jointAngle->pitch = s_attitudeInformation.attitude.pitch - s_aircraftAttitude.pitch;
    jointAngle->roll = s_attitudeInformation.attitude.roll - s_aircraftAttitude.roll;
    jointAngle->yaw = s_attitudeInformation.attitude.yaw - s_aircraftAttitude.yaw;

    if (osalHandler->MutexUnlock(s_attitudeMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mutex unlock error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode StartCalibrate(void)
{
    T_DjiReturnCode djiStat;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    USER_LOG_INFO("start calibrate gimbal.");

    djiStat = osalHandler->GetTimeMs(&s_calibrationStartTime);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("get start time error: 0x%08llX.", djiStat);
    }

    if (osalHandler->MutexLock(s_calibrationMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mutex lock error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    s_calibrationState.calibratingFlag = true;
    s_calibrationState.currentCalibrationProgress = 0;
    s_calibrationState.currentCalibrationStage = DJI_GIMBAL_CALIBRATION_STAGE_PROCRESSING;

    if (osalHandler->MutexUnlock(s_calibrationMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mutex unlock error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode SetControllerSmoothFactor(uint8_t smoothingFactor, E_DjiGimbalAxis axis)
{
    USER_LOG_INFO("set gimbal controller smooth factor: factor %d, axis %d.", smoothingFactor, axis);
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    if (osalHandler->MutexLock(s_commonMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mutex lock error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    if (axis == DJI_GIMBAL_AXIS_PITCH)
        s_systemState.smoothFactor.pitch = smoothingFactor;
    else if (axis == DJI_GIMBAL_AXIS_YAW)
        s_systemState.smoothFactor.yaw = smoothingFactor;
    else
        USER_LOG_ERROR("axis is not supported.");

    if (osalHandler->MutexUnlock(s_commonMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mutex unlock error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode SetPitchRangeExtensionEnabled(bool enabledFlag)
{
    USER_LOG_INFO("set gimbal pitch range extension enable flag: %d.", enabledFlag);
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    if (osalHandler->MutexLock(s_commonMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mutex lock error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    s_systemState.pitchRangeExtensionEnabledFlag = enabledFlag;

    if (osalHandler->MutexUnlock(s_commonMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mutex unlock error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode SetControllerMaxSpeedPercentage(uint8_t maxSpeedPercentage, E_DjiGimbalAxis axis)
{
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    USER_LOG_INFO("set gimbal controller max speed: max speed %d, axis %d.", maxSpeedPercentage, axis);

    if (osalHandler->MutexLock(s_commonMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mutex lock error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    if (axis == DJI_GIMBAL_AXIS_PITCH)
        s_systemState.maxSpeedPercentage.pitch = maxSpeedPercentage;
    else if (axis == DJI_GIMBAL_AXIS_YAW)
        s_systemState.maxSpeedPercentage.yaw = maxSpeedPercentage;
    else
        USER_LOG_ERROR("axis is not supported.");

    if (osalHandler->MutexUnlock(s_commonMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mutex unlock error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode RestoreFactorySettings(void)
{
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    USER_LOG_INFO("restore gimbal factory settings.");

    if (osalHandler->MutexLock(s_commonMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mutex lock error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    s_systemState.pitchRangeExtensionEnabledFlag = false;
    s_systemState.gimbalMode = DJI_GIMBAL_MODE_FREE;
    memset(&s_systemState.fineTuneAngle, 0, sizeof(s_systemState.fineTuneAngle));
    memset(&s_systemState.smoothFactor, 0, sizeof(s_systemState.smoothFactor));
    s_systemState.maxSpeedPercentage.pitch = 1;
    s_systemState.maxSpeedPercentage.yaw = 1;

    if (osalHandler->MutexUnlock(s_commonMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mutex unlock error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode SetMode(E_DjiGimbalMode mode)
{
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    USER_LOG_INFO("set gimbal mode: %d.", mode);

    if (osalHandler->MutexLock(s_commonMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mutex lock error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    s_systemState.gimbalMode = mode;

    if (osalHandler->MutexUnlock(s_commonMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mutex unlock error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode Reset(E_DjiGimbalResetMode mode)
{
    T_DjiReturnCode djiReturnCode = DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    T_DjiAttitude3f attitudeFTemp = {0};
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    USER_LOG_INFO("reset gimbal: %d.", mode);

    if (osalHandler->MutexLock(s_attitudeMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mutex lock error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    if (osalHandler->MutexLock(s_commonMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mutex lock error");
        djiReturnCode = DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
        goto unlock2;
    }

    switch (mode) {
        case DJI_GIMBAL_RESET_MODE_YAW:
            s_attitudeInformation.attitude.yaw = s_aircraftAttitude.yaw + s_systemState.fineTuneAngle.yaw;

            s_attitudeHighPrecision.yaw = s_aircraftAttitude.yaw + s_systemState.fineTuneAngle.yaw;
            break;
        case DJI_GIMBAL_RESET_MODE_PITCH_AND_YAW:
            s_attitudeInformation.attitude.pitch = s_systemState.fineTuneAngle.pitch;
            s_attitudeInformation.attitude.yaw = s_aircraftAttitude.yaw + s_systemState.fineTuneAngle.yaw;

            s_attitudeHighPrecision.pitch = s_systemState.fineTuneAngle.pitch;
            s_attitudeHighPrecision.yaw = s_aircraftAttitude.yaw + s_systemState.fineTuneAngle.yaw;
            break;
        case DJI_GIMBAL_RESET_MODE_PITCH_DOWNWARD_UPWARD_AND_YAW:
            s_attitudeInformation.attitude.pitch =
                s_systemState.fineTuneAngle.pitch + (s_systemState.mountedUpward ? 900 : -900);
            s_attitudeInformation.attitude.yaw = s_aircraftAttitude.yaw + s_systemState.fineTuneAngle.yaw;

            s_attitudeHighPrecision.pitch =
                s_systemState.fineTuneAngle.pitch + (s_systemState.mountedUpward ? 900 : -900);
            s_attitudeHighPrecision.yaw = s_aircraftAttitude.yaw + s_systemState.fineTuneAngle.yaw;
            break;
        case DJI_GIMBAL_RESET_MODE_PITCH_DOWNWARD_UPWARD:
            s_attitudeInformation.attitude.pitch =
                s_systemState.fineTuneAngle.pitch + (s_systemState.mountedUpward ? 900 : -900);

            s_attitudeHighPrecision.pitch =
                s_systemState.fineTuneAngle.pitch + (s_systemState.mountedUpward ? 900 : -900);
            break;
        default:
            USER_LOG_ERROR("reset mode is invalid: %d.", mode);
            djiReturnCode = DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
            goto unlock1;
    }

    attitudeFTemp.pitch = s_attitudeInformation.attitude.pitch;
    attitudeFTemp.roll = s_attitudeInformation.attitude.roll;
    attitudeFTemp.yaw = s_attitudeInformation.attitude.yaw;
    DjiTest_GimbalAngleLegalization(&attitudeFTemp, s_aircraftAttitude, &s_attitudeInformation.reachLimitFlag);
    s_attitudeInformation.attitude.pitch = attitudeFTemp.pitch;
    s_attitudeInformation.attitude.roll = attitudeFTemp.roll;
    s_attitudeInformation.attitude.yaw = attitudeFTemp.yaw;
    DjiTest_GimbalAngleLegalization(&s_attitudeHighPrecision, s_aircraftAttitude, NULL);

    s_rotatingFlag = false;

unlock1:
    if (osalHandler->MutexUnlock(s_commonMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mutex unlock error");
        djiReturnCode = DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
        goto unlock2;
    }

unlock2:
    if (osalHandler->MutexUnlock(s_attitudeMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mutex unlock error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return djiReturnCode;
}

static T_DjiReturnCode FineTuneAngle(T_DjiAttitude3d fineTuneAngle)
{
    T_DjiReturnCode djiReturnCode = DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    T_DjiGimbalReachLimitFlag attitudeReachLimitFlag = {0};
    T_DjiGimbalReachLimitFlag fineTuneAngleReachLimitFlag = {0};
    T_DjiAttitude3d aircraftAttitudeResetted = {0};
    T_DjiAttitude3f attitudeFTemp = {0};
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    USER_LOG_INFO("gimbal fine tune angle: pitch %d, roll %d, yaw %d.", fineTuneAngle.pitch,
                  fineTuneAngle.roll, fineTuneAngle.yaw);

    if (osalHandler->MutexLock(s_attitudeMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mutex lock error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    s_attitudeInformation.attitude.pitch += fineTuneAngle.pitch;
    s_attitudeInformation.attitude.roll += fineTuneAngle.roll;
    s_attitudeInformation.attitude.yaw += fineTuneAngle.yaw;
    attitudeFTemp.pitch = s_attitudeInformation.attitude.pitch;
    attitudeFTemp.roll = s_attitudeInformation.attitude.roll;
    attitudeFTemp.yaw = s_attitudeInformation.attitude.yaw;
    DjiTest_GimbalAngleLegalization(&attitudeFTemp, s_aircraftAttitude, &attitudeReachLimitFlag);
    s_attitudeInformation.attitude.pitch = attitudeFTemp.pitch;
    s_attitudeInformation.attitude.roll = attitudeFTemp.roll;
    s_attitudeInformation.attitude.yaw = attitudeFTemp.yaw;

    s_attitudeHighPrecision.pitch += fineTuneAngle.pitch;
    s_attitudeHighPrecision.roll += fineTuneAngle.roll;
    s_attitudeHighPrecision.yaw += fineTuneAngle.yaw;
    DjiTest_GimbalAngleLegalization(&s_attitudeHighPrecision, s_aircraftAttitude, NULL);

    if (osalHandler->MutexLock(s_commonMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mutex lock error");
        djiReturnCode = DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
        goto unlock;
    }

    s_systemState.fineTuneAngle.pitch += fineTuneAngle.pitch;
    s_systemState.fineTuneAngle.roll += fineTuneAngle.roll;
    s_systemState.fineTuneAngle.yaw += fineTuneAngle.yaw;
    attitudeFTemp.pitch = s_systemState.fineTuneAngle.pitch;
    attitudeFTemp.roll = s_systemState.fineTuneAngle.roll;
    attitudeFTemp.yaw = s_systemState.fineTuneAngle.yaw;
    DjiTest_GimbalAngleLegalization(&attitudeFTemp, aircraftAttitudeResetted, &fineTuneAngleReachLimitFlag);
    s_systemState.fineTuneAngle.pitch = attitudeFTemp.pitch;
    s_systemState.fineTuneAngle.roll = attitudeFTemp.roll;
    s_systemState.fineTuneAngle.yaw = attitudeFTemp.yaw;

    if (osalHandler->MutexUnlock(s_commonMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mutex unlock error");
        djiReturnCode = DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
        goto unlock;
    }

unlock:
    if (osalHandler->MutexUnlock(s_attitudeMutex) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mutex unlock error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    if (((attitudeReachLimitFlag.pitch == true || fineTuneAngleReachLimitFlag.pitch == true) &&
         fineTuneAngle.pitch != 0) ||
        ((attitudeReachLimitFlag.roll == true || fineTuneAngleReachLimitFlag.roll == true) &&
         fineTuneAngle.roll != 0) ||
        ((attitudeReachLimitFlag.yaw == true || fineTuneAngleReachLimitFlag.yaw == true) && fineTuneAngle.yaw != 0)) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_OUT_OF_RANGE;
    }

    return djiReturnCode;
}

/**
 * @brief
 * @param attitude: in ground coordinate
 * @param aircraftAttitude: in ground coordinate
 * @param reachLimitFlag
 * @return
 */
static T_DjiReturnCode DjiTest_GimbalAngleLegalization(T_DjiAttitude3f *attitude, T_DjiAttitude3d aircraftAttitude,
                                                       T_DjiGimbalReachLimitFlag *reachLimitFlag)
{
    T_DjiAttitude3d eulerAngleLimitMin;
    T_DjiAttitude3d eulerAngleLimitMax;
    T_DjiAttitude3d angleLimitInBodyCoordinateFromEulerLimitMin = {0};
    T_DjiAttitude3d angleLimitInBodyCoordinateFromEulerLimitMax = {0};
    T_DjiAttitude3d finalAngleLimitInBodyCoordinateMin = {0};
    T_DjiAttitude3d finalAngleLimitInBodyCoordinateMax = {0};
    T_DjiAttitude3f attitudeInBodyCoordinate = {0};

    if (attitude == NULL) {
        USER_LOG_ERROR("input pointer is null.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    // calculate euler angle limit
    eulerAngleLimitMin = s_eulerAngleLimitMin;
    eulerAngleLimitMax = s_eulerAngleLimitMax;
    if (s_systemState.pitchRangeExtensionEnabledFlag == true) {
        eulerAngleLimitMin.pitch = s_pitchEulerAngleExtensionMin;
        eulerAngleLimitMax.pitch = s_pitchEulerAngleExtensionMax;
    }

    // transfer euler angle limit to angle limit in body coordinate
    angleLimitInBodyCoordinateFromEulerLimitMin.pitch = eulerAngleLimitMin.pitch - aircraftAttitude.pitch;
    angleLimitInBodyCoordinateFromEulerLimitMin.roll = eulerAngleLimitMin.roll - aircraftAttitude.roll;
    angleLimitInBodyCoordinateFromEulerLimitMin.yaw = eulerAngleLimitMin.yaw - aircraftAttitude.yaw;
    angleLimitInBodyCoordinateFromEulerLimitMax.pitch = eulerAngleLimitMax.pitch - aircraftAttitude.pitch;
    angleLimitInBodyCoordinateFromEulerLimitMax.roll = eulerAngleLimitMax.roll - aircraftAttitude.roll;
    angleLimitInBodyCoordinateFromEulerLimitMax.yaw = eulerAngleLimitMax.yaw - aircraftAttitude.yaw;

    // calculate final angle limit in body coordinate based on euler angle limit and joint angle limit
    finalAngleLimitInBodyCoordinateMin.pitch = USER_UTIL_MAX(angleLimitInBodyCoordinateFromEulerLimitMin.pitch,
                                                             s_jointAngleLimitMin.pitch);
    finalAngleLimitInBodyCoordinateMin.roll = USER_UTIL_MAX(angleLimitInBodyCoordinateFromEulerLimitMin.roll,
                                                            s_jointAngleLimitMin.roll);
    finalAngleLimitInBodyCoordinateMin.yaw = USER_UTIL_MAX(angleLimitInBodyCoordinateFromEulerLimitMin.yaw,
                                                           s_jointAngleLimitMin.yaw);
    finalAngleLimitInBodyCoordinateMax.pitch = USER_UTIL_MIN(angleLimitInBodyCoordinateFromEulerLimitMax.pitch,
                                                             s_jointAngleLimitMax.pitch);
    finalAngleLimitInBodyCoordinateMax.roll = USER_UTIL_MIN(angleLimitInBodyCoordinateFromEulerLimitMax.roll,
                                                            s_jointAngleLimitMax.roll);
    finalAngleLimitInBodyCoordinateMax.yaw = USER_UTIL_MIN(angleLimitInBodyCoordinateFromEulerLimitMax.yaw,
                                                           s_jointAngleLimitMax.yaw);

    // calculate gimbal attitude in body coordinate
    attitudeInBodyCoordinate.pitch = attitude->pitch - (float) aircraftAttitude.pitch;
    attitudeInBodyCoordinate.roll = attitude->roll - (float) aircraftAttitude.roll;
    attitudeInBodyCoordinate.yaw = attitude->yaw - (float) aircraftAttitude.yaw;

    // modify attitude based on final angle limit
    attitudeInBodyCoordinate.pitch = USER_UTIL_MIN(attitudeInBodyCoordinate.pitch,
                                                   (float) finalAngleLimitInBodyCoordinateMax.pitch);
    attitudeInBodyCoordinate.pitch = USER_UTIL_MAX(attitudeInBodyCoordinate.pitch,
                                                   (float) finalAngleLimitInBodyCoordinateMin.pitch);
    attitudeInBodyCoordinate.roll = USER_UTIL_MIN(attitudeInBodyCoordinate.roll,
                                                  (float) finalAngleLimitInBodyCoordinateMax.roll);
    attitudeInBodyCoordinate.roll = USER_UTIL_MAX(attitudeInBodyCoordinate.roll,
                                                  (float) finalAngleLimitInBodyCoordinateMin.roll);
    attitudeInBodyCoordinate.yaw = USER_UTIL_MIN(attitudeInBodyCoordinate.yaw,
                                                 (float) finalAngleLimitInBodyCoordinateMax.yaw);
    attitudeInBodyCoordinate.yaw = USER_UTIL_MAX(attitudeInBodyCoordinate.yaw,
                                                 (float) finalAngleLimitInBodyCoordinateMin.yaw);

    // calculate gimbal attitude in ground coordinate
    attitude->pitch = attitudeInBodyCoordinate.pitch + (float) aircraftAttitude.pitch;
    attitude->roll = attitudeInBodyCoordinate.roll + (float) aircraftAttitude.roll;
    attitude->yaw = attitudeInBodyCoordinate.yaw + (float) aircraftAttitude.yaw;

    // calculate reach limit flag, please note reach limit flag only specifies whether gimbal attitude reach joint angle limit
    if (reachLimitFlag != NULL) {
        reachLimitFlag->pitch = (attitudeInBodyCoordinate.pitch >= s_jointAngleLimitMax.pitch ||
                                 attitudeInBodyCoordinate.pitch <= s_jointAngleLimitMin.pitch) ? true : false;
        reachLimitFlag->roll = (attitudeInBodyCoordinate.roll >= s_jointAngleLimitMax.roll ||
                                attitudeInBodyCoordinate.roll <= s_jointAngleLimitMin.roll) ? true : false;
        reachLimitFlag->yaw = (attitudeInBodyCoordinate.yaw >= s_jointAngleLimitMax.yaw ||
                               attitudeInBodyCoordinate.yaw <= s_jointAngleLimitMin.yaw) ? true : false;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * @brief
 * @param originalAttitude: unit: 0.1degree.
 * @param targetAttitude: unit: 0.1degree.
 * @param actionTime: unit: 0.01s.
 * @param speed: unit: 0.1degree/s.
 * @return
 */
static T_DjiReturnCode
DjiTest_GimbalCalculateSpeed(T_DjiAttitude3d originalAttitude, T_DjiAttitude3d targetAttitude, uint16_t actionTime,
                             T_DjiAttitude3d *speed)
{
    float pitchSpeedTemp = 0;
    float rollSpeedTemp = 0;
    float yawSpeedTemp = 0;

    if (speed == NULL) {
        USER_LOG_ERROR("input pointer is null.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    if (actionTime == 0) {
        USER_LOG_WARN("Input action time is zero, now used max speed to rotate.");
        actionTime = PAYLOAD_GIMBAL_MIN_ACTION_TIME;
    }

    pitchSpeedTemp = (float) (targetAttitude.pitch - originalAttitude.pitch) / (float) actionTime * 100;
    rollSpeedTemp = (float) (targetAttitude.roll - originalAttitude.roll) / (float) actionTime * 100;
    yawSpeedTemp = (float) (targetAttitude.yaw - originalAttitude.yaw) / (float) actionTime * 100;

    speed->pitch = pitchSpeedTemp >= 0.0f ? (int32_t) (pitchSpeedTemp + 1) : (int32_t) (pitchSpeedTemp - 1);
    speed->roll = rollSpeedTemp >= 0.0f ? (int32_t) (rollSpeedTemp + 1) : (int32_t) (rollSpeedTemp - 1);
    speed->yaw = yawSpeedTemp >= 0.0f ? (int32_t) (yawSpeedTemp + 1) : (int32_t) (yawSpeedTemp - 1);

    DjiTest_GimbalSpeedLegalization(speed);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static void DjiTest_GimbalSpeedLegalization(T_DjiAttitude3d *speed)
{
    speed->pitch = speed->pitch > s_speedLimit.pitch ? s_speedLimit.pitch : speed->pitch;
    speed->roll = speed->roll > s_speedLimit.roll ? s_speedLimit.roll : speed->roll;
    speed->yaw = speed->yaw > s_speedLimit.yaw ? s_speedLimit.yaw : speed->yaw;

    speed->pitch = speed->pitch < (0 - s_speedLimit.pitch) ? (0 - s_speedLimit.pitch) : speed->pitch;
    speed->roll = speed->roll < (0 - s_speedLimit.roll) ? (0 - s_speedLimit.roll) : speed->roll;
    speed->yaw = speed->yaw < (0 - s_speedLimit.yaw) ? (0 - s_speedLimit.yaw) : speed->yaw;
}

/**
 * @brief
 * @param quaternion
 * @param attitude Unit: 0.1 degree.
 * @return
 */
static T_DjiReturnCode DjiTest_GimbalCalculateGroundAttitudeBaseQuaternion(T_DjiFcSubscriptionQuaternion quaternion,
                                                                           T_DjiAttitude3d *attitude)
{
    double aircraftPitchInRad;
    double aircraftRollInRad;
    double aircraftYawInRad;

    if (attitude == NULL) {
        USER_LOG_ERROR("Input argument is null.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    aircraftPitchInRad = asin(2 * ((double) quaternion.q0 * quaternion.q2 - (double) quaternion.q3 * quaternion.q1));
    attitude->pitch = aircraftPitchInRad * 180 / DJI_PI * 10;

    aircraftRollInRad = atan2(2 * ((double) quaternion.q0 * quaternion.q1 + (double) quaternion.q2 * quaternion.q3),
                              (double) 1 -
                              2 * ((double) quaternion.q1 * quaternion.q1 + (double) quaternion.q2 * quaternion.q2));
    attitude->roll = aircraftRollInRad * 180 / DJI_PI * 10;

    aircraftYawInRad = atan2(2 * ((double) quaternion.q0 * quaternion.q3 + (double) quaternion.q1 * quaternion.q2),
                             (double) 1 -
                             2 * ((double) quaternion.q2 * quaternion.q2 + (double) quaternion.q3 * quaternion.q3));
    attitude->yaw = aircraftYawInRad * 180 / DJI_PI * 10;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
