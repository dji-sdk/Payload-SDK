/**
 ********************************************************************
 * @file    test_power_management.c
 * @version V2.0.0
 * @date    2019/8/29
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
#include "test_power_management.h"
#include "psdk_logger.h"

/* Private constants ---------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private functions declaration ---------------------------------------------*/
static T_PsdkReturnCode PsdkTest_PowerOffNotificationCallback(bool *powerOffPreparationFlag);

/* Private variables ---------------------------------------------------------*/
static T_PsdkTestApplyHighPowerHandler s_applyHighPowerHandler;

/* Exported functions definition ---------------------------------------------*/
/**
 * @brief Register handler function for applying high power. This function have to be called before calling
 * PsdkTest_PowerManagementInit(), except for in Linux, because PsdkTest_PowerManagementInit() do not apply high power
 * in Linux OS.
 * @param applyHighPowerHandler: pointer to handler function for applying high power.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkTest_RegApplyHighPowerHandler(T_PsdkTestApplyHighPowerHandler *applyHighPowerHandler)
{
    if (applyHighPowerHandler->pinInit == NULL) {
        PsdkLogger_UserLogError("reg apply high power handler pinInit error");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    if (applyHighPowerHandler->pinWrite == NULL) {
        PsdkLogger_UserLogError("reg apply high power handler pinWrite error");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    memcpy(&s_applyHighPowerHandler, applyHighPowerHandler, sizeof(T_PsdkTestApplyHighPowerHandler));

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/**
 * @brief Initialise power management module, including apply high power (only RTOS) and register power off notification
 * callback function.
 * @note PSDK development board 1.0 can not accept high power, so do not call this function in PSDK development board
 * 1.0 project.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkTest_PowerManagementInit(void)
{
    T_PsdkReturnCode psdkStat;

    // apply high power
#if !PSDK_ARCH_SYS_LINUX
    if (s_applyHighPowerHandler.pinInit == NULL) {
        PsdkLogger_UserLogError("apply high power pin init interface is NULL error");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    if (s_applyHighPowerHandler.pinWrite == NULL) {
        PsdkLogger_UserLogError("apply high power pin write interface is NULL error");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    psdkStat = s_applyHighPowerHandler.pinInit();
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("apply high power pin init error");
        return psdkStat;
    }

    psdkStat = PsdkPowerManagement_RegWriteHighPowerApplyPinCallback(s_applyHighPowerHandler.pinWrite);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("register WriteHighPowerApplyPinCallback error.");
        return psdkStat;
    }

    psdkStat = PsdkPowerManagement_ApplyHighPowerSync();
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("apply high power error");
        return psdkStat;
    }
#endif

    // register power off notification callback function
    psdkStat = PsdkPowerManagement_RegPowerOffNotificationCallback(PsdkTest_PowerOffNotificationCallback);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("register power off notification callback function error");
        return psdkStat;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/* Private functions definition-----------------------------------------------*/
static T_PsdkReturnCode PsdkTest_PowerOffNotificationCallback(bool *powerOffPreparationFlag)
{
    PsdkLogger_UserLogDebug("aircraft will power off soon.");

    *powerOffPreparationFlag = true;

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
