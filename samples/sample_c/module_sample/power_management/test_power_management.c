/**
 ********************************************************************
 * @file    test_power_management.c
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
#include "test_power_management.h"
#include "dji_logger.h"
#include "dji_aircraft_info.h"

/* Private constants ---------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private functions declaration ---------------------------------------------*/
static T_DjiReturnCode DjiTest_PowerOffNotificationCallback(bool *powerOffPreparationFlag);

/* Private variables ---------------------------------------------------------*/
static T_DjiTestApplyHighPowerHandler s_applyHighPowerHandler;

/* Exported functions definition ---------------------------------------------*/
/**
 * @brief Register handler function for applying high power. This function have to be called before calling
 * DjiTest_PowerManagementInit(), except for in Linux, because DjiTest_PowerManagementInit() do not apply high power
 * in Linux OS.
 * @param applyHighPowerHandler: pointer to handler function for applying high power.
 * @return Execution result.
 */
T_DjiReturnCode DjiTest_RegApplyHighPowerHandler(T_DjiTestApplyHighPowerHandler *applyHighPowerHandler)
{
    if (applyHighPowerHandler->pinInit == NULL) {
        USER_LOG_ERROR("reg apply high power handler pinInit error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    if (applyHighPowerHandler->pinWrite == NULL) {
        USER_LOG_ERROR("reg apply high power handler pinWrite error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    memcpy(&s_applyHighPowerHandler, applyHighPowerHandler, sizeof(T_DjiTestApplyHighPowerHandler));

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * @brief Initialise power management module, including apply high power (only RTOS) and register power off notification
 * callback function.
 * @note DJI development board 1.0 can not accept high power, so do not call this function in DJI development board
 * 1.0 project.
 * @return Execution result.
 */
T_DjiReturnCode DjiTest_PowerManagementStartService(void)
{
    T_DjiReturnCode returnCode;
    T_DjiAircraftInfoBaseInfo baseInfo = {0};

    returnCode = DjiPowerManagement_Init();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("power management init error: 0x%08llX.", returnCode);
        return returnCode;
    }

    returnCode = DjiAircraftInfo_GetBaseInfo(&baseInfo);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Get aircraft base info error: 0x%08llX.", returnCode);
        return returnCode;
    }

    if (baseInfo.djiAdapterType == DJI_SDK_ADAPTER_TYPE_SKYPORT_V2 ||
        baseInfo.djiAdapterType == DJI_SDK_ADAPTER_TYPE_XPORT ||
        baseInfo.djiAdapterType == DJI_SDK_ADAPTER_TYPE_SKYPORT_V3 ||
        baseInfo.djiAdapterType == DJI_SDK_ADAPTER_TYPE_EPORT_V2_RIBBON_CABLE ||
     		baseInfo.aircraftType == DJI_AIRCRAFT_TYPE_FC30) {
        // apply high power
        if (s_applyHighPowerHandler.pinInit == NULL) {
            USER_LOG_ERROR("apply high power pin init interface is NULL error");
            return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
        }

        if (s_applyHighPowerHandler.pinWrite == NULL) {
            USER_LOG_ERROR("apply high power pin write interface is NULL error");
            return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
        }

        returnCode = s_applyHighPowerHandler.pinInit();
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("apply high power pin init error");
            return returnCode;
        }

        returnCode = DjiPowerManagement_RegWriteHighPowerApplyPinCallback(s_applyHighPowerHandler.pinWrite);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("register WriteHighPowerApplyPinCallback error.");
            return returnCode;
        }

        if (baseInfo.djiAdapterType == DJI_SDK_ADAPTER_TYPE_SKYPORT_V3 ||
            baseInfo.djiAdapterType == DJI_SDK_ADAPTER_TYPE_EPORT_V2_RIBBON_CABLE) {
            E_DjiHighPowerVoltage voltage = E_DJI_HIGH_POWER_VOLTAGE_17V;

            USER_LOG_INFO("Start to apply high power.");

            returnCode = DjiPowerManagement_ApplyHighPowerSyncV2(voltage);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("apply high power error");
                return returnCode;
            }
            USER_LOG_INFO("Success to apply high power %s.",
                voltage == E_DJI_HIGH_POWER_VOLTAGE_13V6 ? "13V6" :
                voltage == E_DJI_HIGH_POWER_VOLTAGE_17V ? "17V" :
                voltage == E_DJI_HIGH_POWER_VOLTAGE_24V ? "24V" : "???");
        } else {
            USER_LOG_INFO("Start to apply high power.");

            returnCode = DjiPowerManagement_ApplyHighPowerSync();
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("apply high power error");
                return returnCode;
            }
            USER_LOG_INFO("Success to apply high power.");
        }
    }

    // register power off notification callback function
    returnCode = DjiPowerManagement_RegPowerOffNotificationCallback(DjiTest_PowerOffNotificationCallback);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("register power off notification callback function error");
        return returnCode;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiTest_PowerManagementStopService(void)
{
    T_DjiReturnCode returnCode;

    returnCode = DjiPowerManagement_DeInit();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("power management deinit error: 0x%08llX.", returnCode);
        return returnCode;
    }

    return returnCode;
}

/* Private functions definition-----------------------------------------------*/
static T_DjiReturnCode DjiTest_PowerOffNotificationCallback(bool *powerOffPreparationFlag)
{
    USER_LOG_INFO("aircraft will power off soon.");

    *powerOffPreparationFlag = true;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
