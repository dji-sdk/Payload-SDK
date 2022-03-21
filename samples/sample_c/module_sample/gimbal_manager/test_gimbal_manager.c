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

/* Private constants ---------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private values -------------------------------------------------------------*/

/* Private functions declaration ---------------------------------------------*/

/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode DjiTest_GimbalManagerRunSample(E_DjiMountPosition mountPosition, E_DjiGimbalMode gimbalMode)
{
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    T_DjiReturnCode returnCode;
    T_DjiGimbalManagerRotation rotation;

    USER_LOG_INFO("Gimbal manager sample start");
    DjiTest_WidgetLogAppend("Gimbal manager sample start");

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

    USER_LOG_INFO("--> Step 3: Rotate gimbal to target angle in relative angle mode\r\n");
    DjiTest_WidgetLogAppend("--> Step 3: Rotate gimbal to target angle in relative angle mode\r\n");

    for (int i = 0; i < 3; i++) {
        USER_LOG_INFO("Target gimbal pry = (30, 0, 0) in the body coordinate system");
        rotation = (T_DjiGimbalManagerRotation) {DJI_GIMBAL_ROTATION_MODE_RELATIVE_ANGLE, 30, 0, 0, 0.5};
        returnCode = DjiGimbalManager_Rotate(mountPosition, rotation);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Target gimbal pry = (30, 0, 0) failed, error code: 0x%08X", returnCode);
        }

        osalHandler->TaskSleepMs(1000);

        USER_LOG_INFO("Target gimbal pry = (0, 30, 0) in the body coordinate system");
        rotation = (T_DjiGimbalManagerRotation) {DJI_GIMBAL_ROTATION_MODE_RELATIVE_ANGLE, 0, 30, 0, 0.5};
        returnCode = DjiGimbalManager_Rotate(mountPosition, rotation);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Target gimbal pry = (0, 30, 0) failed, error code: 0x%08X", returnCode);
        }
        osalHandler->TaskSleepMs(1000);

        USER_LOG_INFO("Target gimbal pry = (0, 0, 30) in the body coordinate system");
        rotation = (T_DjiGimbalManagerRotation) {DJI_GIMBAL_ROTATION_MODE_RELATIVE_ANGLE, 0, 0, 30, 0.5};
        returnCode = DjiGimbalManager_Rotate(mountPosition, rotation);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Target gimbal pry = (0, 0, 30) failed, error code: 0x%08X", returnCode);
        }
        osalHandler->TaskSleepMs(1000);

        USER_LOG_INFO("Target gimbal reset.\r\n");
        returnCode = DjiGimbalManager_Reset(mountPosition);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Reset gimbal failed, error code: 0x%08X", returnCode);
        }
        osalHandler->TaskSleepMs(2000);
    }

    USER_LOG_INFO("--> Step 4: Rotate gimbal to target angle in absolute angle mode\r\n");
    DjiTest_WidgetLogAppend("--> Step 4: Rotate gimbal to target angle in absolute angle mode\r\n");

    for (int i = 0; i < 3; i++) {
        USER_LOG_INFO("Target gimbal pry = (30, 0, 0) in the ground coordinate system");
        rotation = (T_DjiGimbalManagerRotation) {DJI_GIMBAL_ROTATION_MODE_ABSOLUTE_ANGLE, 30, 0, 0, 0.5};
        returnCode = DjiGimbalManager_Rotate(mountPosition, rotation);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Target gimbal pry = (30, 0, 0) failed, error code: 0x%08X", returnCode);
        }

        osalHandler->TaskSleepMs(1000);

        USER_LOG_INFO("Target gimbal pry = (0, 30, 0) in the ground coordinate system");
        rotation = (T_DjiGimbalManagerRotation) {DJI_GIMBAL_ROTATION_MODE_ABSOLUTE_ANGLE, 0, 30, 0, 0.5};
        returnCode = DjiGimbalManager_Rotate(mountPosition, rotation);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Target gimbal pry = (0, 30, 0) failed, error code: 0x%08X", returnCode);
        }
        osalHandler->TaskSleepMs(1000);

        USER_LOG_INFO("Target gimbal pry = (0, 0, 30) in the ground coordinate system");
        rotation = (T_DjiGimbalManagerRotation) {DJI_GIMBAL_ROTATION_MODE_ABSOLUTE_ANGLE, 0, 0, 30, 0.5};
        returnCode = DjiGimbalManager_Rotate(mountPosition, rotation);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Target gimbal pry = (0, 0, 30) failed, error code: 0x%08X", returnCode);
        }
        osalHandler->TaskSleepMs(1000);

        USER_LOG_INFO("Target gimbal reset.\r\n");
        returnCode = DjiGimbalManager_Reset(mountPosition);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Reset gimbal failed, error code: 0x%08X", returnCode);
        }
        osalHandler->TaskSleepMs(2000);
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
