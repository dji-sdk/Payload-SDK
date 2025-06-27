/**
 ********************************************************************
 * @file    test_gimbal_manager.c
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
#include <utils/util_misc.h>
#include <widget_interaction_test/test_widget_interaction.h>
#include "test_gimbal_manager.h"
#include "dji_platform.h"
#include "dji_logger.h"
#include "dji_gimbal_manager.h"
#include "dji_fc_subscription.h"
#include "dji_aircraft_info.h"

/* Private constants ---------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/
typedef enum {
    DJI_TEST_GIMBAL_ROTATION,
    DJI_TEST_GIMBAL_RESET,
} E_DjiTestGimbalAction;

typedef struct {
    E_DjiTestGimbalAction action;
    T_DjiGimbalManagerRotation rotation;
} T_DjiTestGimbalActionList;

/* Private values -------------------------------------------------------------*/
static const T_DjiTestGimbalActionList s_rotationActionList[] =
{
        {.action = DJI_TEST_GIMBAL_RESET},
        {.action = DJI_TEST_GIMBAL_ROTATION, .rotation.rotationMode = DJI_GIMBAL_ROTATION_MODE_RELATIVE_ANGLE, 30,  0,   0,  0.2},
        {.action = DJI_TEST_GIMBAL_ROTATION, .rotation.rotationMode = DJI_GIMBAL_ROTATION_MODE_RELATIVE_ANGLE, -30, 0,   0,  0.2},
        {.action = DJI_TEST_GIMBAL_ROTATION, .rotation.rotationMode = DJI_GIMBAL_ROTATION_MODE_RELATIVE_ANGLE, -30, 0,   0,  0.2},
        {.action = DJI_TEST_GIMBAL_ROTATION, .rotation.rotationMode = DJI_GIMBAL_ROTATION_MODE_RELATIVE_ANGLE, 30,  0,   0,  0.2},
        {.action = DJI_TEST_GIMBAL_ROTATION, .rotation.rotationMode = DJI_GIMBAL_ROTATION_MODE_RELATIVE_ANGLE, 0,   30,  0,  0.2},
        {.action = DJI_TEST_GIMBAL_ROTATION, .rotation.rotationMode = DJI_GIMBAL_ROTATION_MODE_RELATIVE_ANGLE, 0,   -30, 0,  0.2},
        {.action = DJI_TEST_GIMBAL_ROTATION, .rotation.rotationMode = DJI_GIMBAL_ROTATION_MODE_RELATIVE_ANGLE, 0,   -30, 0,  0.2},
        {.action = DJI_TEST_GIMBAL_ROTATION, .rotation.rotationMode = DJI_GIMBAL_ROTATION_MODE_RELATIVE_ANGLE, 0,   30,  0,  0.2},
        {.action = DJI_TEST_GIMBAL_ROTATION, .rotation.rotationMode = DJI_GIMBAL_ROTATION_MODE_RELATIVE_ANGLE, 0,   0,   4,  0.2},
        {.action = DJI_TEST_GIMBAL_ROTATION, .rotation.rotationMode = DJI_GIMBAL_ROTATION_MODE_RELATIVE_ANGLE, 0,   0,   -4, 0.2},
        {.action = DJI_TEST_GIMBAL_RESET},
        {.action = DJI_TEST_GIMBAL_ROTATION, .rotation.rotationMode = DJI_GIMBAL_ROTATION_MODE_ABSOLUTE_ANGLE, 30,  0,   0,  0.2},
        {.action = DJI_TEST_GIMBAL_ROTATION, .rotation.rotationMode = DJI_GIMBAL_ROTATION_MODE_ABSOLUTE_ANGLE, -90, 0,   0,  0.5},
        {.action = DJI_TEST_GIMBAL_ROTATION, .rotation.rotationMode = DJI_GIMBAL_ROTATION_MODE_ABSOLUTE_ANGLE, -60, 0,   0,  0.5},
        {.action = DJI_TEST_GIMBAL_ROTATION, .rotation.rotationMode = DJI_GIMBAL_ROTATION_MODE_ABSOLUTE_ANGLE, -30, 0,   0,  0.5},
        {.action = DJI_TEST_GIMBAL_RESET},
};

/* Private functions declaration ---------------------------------------------*/

/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode DjiTest_GimbalManagerRunSample(E_DjiMountPosition mountPosition, E_DjiGimbalMode gimbalMode)
{
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    T_DjiReturnCode returnCode;
    T_DjiGimbalManagerRotation rotation;
    T_DjiAircraftInfoBaseInfo baseInfo;
    E_DjiAircraftSeries aircraftSeries;
    bool gimbalAnglesSubscribedFlag = false;

    returnCode = DjiAircraftInfo_GetBaseInfo(&baseInfo);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Failed to get aircraft base info, return code 0x%08X", returnCode);
        goto out;
    }

    aircraftSeries = baseInfo.aircraftSeries;

    USER_LOG_INFO("Gimbal manager sample start");
    DjiTest_WidgetLogAppend("Gimbal manager sample start");

    if (DJI_AIRCRAFT_SERIES_M30 == aircraftSeries ||
        DJI_AIRCRAFT_SERIES_M3 == aircraftSeries ||
	    DJI_AIRCRAFT_SERIES_M3D == aircraftSeries ||
        DJI_AIRCRAFT_SERIES_M4 == aircraftSeries ||
        DJI_AIRCRAFT_SERIES_M4D == aircraftSeries) {
        returnCode = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_GIMBAL_ANGLES, DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ, NULL);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Failed to subscribe topic %d, 0x%08X", DJI_FC_SUBSCRIPTION_TOPIC_GIMBAL_ANGLES, returnCode);
            goto out;
        }
        gimbalAnglesSubscribedFlag = true;
    }

    USER_LOG_INFO("--> Step 1: Init gimbal manager module");
    DjiTest_WidgetLogAppend("--> Step 1: Init gimbal manager module");
    returnCode = DjiGimbalManager_Init();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Init gimbal manager failed, error code: 0x%08X", returnCode);
        goto out;
    }

    if (gimbalMode == DJI_GIMBAL_MODE_FREE) {
        USER_LOG_INFO("--> Step 2: Set gimbal to free mode");
        DjiTest_WidgetLogAppend("--> Step 2: Set gimbal to free mode");
    } else if (gimbalMode == DJI_GIMBAL_MODE_YAW_FOLLOW) {
        USER_LOG_INFO("--> Step 2: Set gimbal to yaw follow mode");
        DjiTest_WidgetLogAppend("--> Step 2: Set gimbal to yaw follow mode");
    }
    returnCode = DjiGimbalManager_SetMode(mountPosition, gimbalMode);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Set gimbal mode failed, error code: 0x%08X", returnCode);
        goto out;
    }

    USER_LOG_INFO("--> Step 3: Reset gimbal angles.\r\n");
    returnCode = DjiGimbalManager_Reset(mountPosition, DJI_GIMBAL_RESET_MODE_PITCH_AND_YAW);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Reset gimbal failed, error code: 0x%08X", returnCode);
    }

    USER_LOG_INFO("--> Step 4: Rotate gimbal to target angle by action list\r\n");
    for (int i = 0; i < sizeof(s_rotationActionList) / sizeof(T_DjiTestGimbalActionList); ++i) {
        if (s_rotationActionList[i].action == DJI_TEST_GIMBAL_RESET) {
            USER_LOG_INFO("Target gimbal reset.\r\n");
            returnCode = DjiGimbalManager_Reset(mountPosition, DJI_GIMBAL_RESET_MODE_PITCH_AND_YAW);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Reset gimbal failed, error code: 0x%08X", returnCode);
            }
            osalHandler->TaskSleepMs(2000);
        } else if (s_rotationActionList[i].action == DJI_TEST_GIMBAL_ROTATION) {

            if (gimbalMode == DJI_GIMBAL_MODE_FREE &&
                s_rotationActionList[i].rotation.rotationMode == DJI_GIMBAL_ROTATION_MODE_ABSOLUTE_ANGLE) {
                continue;
            }

            rotation = s_rotationActionList[i].rotation;

            if (DJI_AIRCRAFT_SERIES_M30 == aircraftSeries
                || DJI_AIRCRAFT_SERIES_M3 == aircraftSeries
	            || DJI_AIRCRAFT_SERIES_M3D == aircraftSeries
                || DJI_AIRCRAFT_SERIES_M4 == aircraftSeries
                || DJI_AIRCRAFT_SERIES_M4D == aircraftSeries
            ) {
                if (s_rotationActionList[i].rotation.rotationMode == DJI_GIMBAL_ROTATION_MODE_ABSOLUTE_ANGLE) {
                    T_DjiFcSubscriptionGimbalAngles gimbalAngles = {0};
                    T_DjiDataTimestamp timestamp = {0};
                    returnCode = DjiFcSubscription_GetLatestValueOfTopic(DJI_FC_SUBSCRIPTION_TOPIC_GIMBAL_ANGLES,
                                                                        (uint8_t *) &gimbalAngles,
                                                                        sizeof(T_DjiFcSubscriptionGimbalAngles),
                                                                        &timestamp);
                    rotation.yaw = gimbalAngles.z;
                }
            }

            USER_LOG_INFO("Target gimbal pry = (%.1f, %.1f, %.1f)", rotation.pitch, rotation.roll, rotation.yaw);

            returnCode = DjiGimbalManager_Rotate(mountPosition, rotation);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Target gimbal pry = (%.1f, %.1f, %.1f) failed, error code: 0x%08X",
                               s_rotationActionList[i].rotation.pitch, s_rotationActionList[i].rotation.roll,
                               s_rotationActionList[i].rotation.yaw,
                               returnCode);
            }
            osalHandler->TaskSleepMs(1000);
        }
    }

    if (gimbalAnglesSubscribedFlag) {
        returnCode = DjiFcSubscription_UnSubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_GIMBAL_ANGLES);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Failed to unsubscribe topic %d, 0x%08X", DJI_FC_SUBSCRIPTION_TOPIC_GIMBAL_ANGLES, returnCode);
        }
        gimbalAnglesSubscribedFlag = false;
    }

    USER_LOG_INFO("--> Step 5: Deinit gimbal manager module");
    DjiTest_WidgetLogAppend("--> Step 5: Deinit gimbal manager module");
    returnCode = DjiGimbalManager_Deinit();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Deinit gimbal manager failed, error code: 0x%08X", returnCode);
        goto out;
    }

out:
    USER_LOG_INFO("Gimbal manager sample end");

    return returnCode;
}

/* Private functions definition-----------------------------------------------*/
/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
