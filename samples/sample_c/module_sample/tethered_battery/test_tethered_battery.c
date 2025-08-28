/**
 ********************************************************************
 * @file    test_tethered_battery.c
 * @brief
 *
 * @copyright (c) 2024 DJI. All rights reserved.
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
#include "test_tethered_battery.h"
#include "dji_logger.h"
#include "dji_tethered_battery.h"
#include "utils/util_misc.h"

/* Private constants ---------------------------------------------------------*/
#define TETHERED_BATTERY_TASK_STACK_SIZE           (2048)
#define TETHERED_BATTERY_TETHER_LINE_MAX_LENGTH    (150)

/* Private types -------------------------------------------------------------*/

/* Private values -------------------------------------------------------------*/
static T_DjiTaskHandle s_tetheredBatteryTestThread;
static bool s_isTetherRetrieval = false;

/* Private functions declaration ---------------------------------------------*/
static void *DjiTest_TetheredBatteryTask(void *arg);

/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode DjiTest_TetheredBatteryStartService(void)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    returnCode = DjiTetheredBattery_Init();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Dji test tethered battery init failed, errno = 0x%08llX", returnCode);
        return returnCode;
    }

    returnCode = osalHandler->TaskCreate("user_tethered_battery_task", DjiTest_TetheredBatteryTask,
                                         TETHERED_BATTERY_TASK_STACK_SIZE, NULL,
                                         &s_tetheredBatteryTestThread);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Dji tethered battery test task create failed, errno = 0x%08llX", returnCode);
        return returnCode;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/* Private functions definition-----------------------------------------------*/

static void *DjiTest_TetheredBatteryTask(void *arg)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    T_DjiTetherLineStatus tetherLineStatus = {0};

    USER_UTIL_UNUSED(arg);

    tetherLineStatus.usedLength = 0;
    tetherLineStatus.totalLength = TETHERED_BATTERY_TETHER_LINE_MAX_LENGTH;

    while (1) {
        if (tetherLineStatus.usedLength >= tetherLineStatus.totalLength) {
            s_isTetherRetrieval = true;
        }

        if (tetherLineStatus.usedLength <= 0.01f) {
            s_isTetherRetrieval = false;
        }

        if (s_isTetherRetrieval == false) {
            tetherLineStatus.usedLength += 1.00f;
        } else {
            tetherLineStatus.usedLength -= 1.00f;
        }

        returnCode = DjiTetheredBattery_PushTetherLineStatus(tetherLineStatus);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Test push tether line status error, stat = 0x%08llX", returnCode);
        }

        USER_LOG_DEBUG("Push tether line status, total: %.2f, used: %.2f", tetherLineStatus.totalLength,
                       tetherLineStatus.usedLength);

        osalHandler->TaskSleepMs(100);
    }

    return NULL;
}

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
