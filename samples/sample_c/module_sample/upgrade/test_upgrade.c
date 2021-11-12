/**
 ********************************************************************
 * @file    test_upgrade.c
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
#include <dji_logger.h>
#include <dji_platform.h>
#include <utils/util_misc.h>
#include "test_upgrade_common_file_transfer.h"
#include "test_upgrade_platform_opt.h"
#include "test_upgrade.h"

/* Private constants ---------------------------------------------------------*/
#define UPGRADE_TASK_STACK_SIZE                (2048)
#define DJI_TEST_UPGRADE_TASK_FREQ             (50)
#define DJI_TEST_ENTER_UPGRADE_WAIT_TIME       (10)  //wait 10s for enter upgrade process
#define DJI_TEST_UPGRADE_REBOOT_TIMEOUT        (30)   //reboot timeout 30s

/* Private types -------------------------------------------------------------*/

/* Private values -------------------------------------------------------------*/
static T_DjiUpgradeState s_upgradeState = {0};
static T_DjiMutexHandle s_upgradeStateMutex = {0};
static T_DjiTaskHandle s_upgradeProcessThread;
static T_DjiTaskHandle s_enterUpgradeModeProcessThread;
static bool s_isNeedEnterUpgradeModeProcess = false;
static bool s_isNeedReplaceProgramBeforeReboot = false;

/* Private functions declaration ---------------------------------------------*/
static T_DjiReturnCode DjiTest_EnterUpgradeMode(uint16_t *waitTime);
static T_DjiReturnCode DjiTest_CheckFirmware(void);
static T_DjiReturnCode DjiTest_StartUpgrade(void);
static T_DjiReturnCode DjiTest_FinishUpgrade(void);
static void *DjiTest_UpgradeProcessTask(void *arg);
static void *DjiTest_EnterUpgradeModeProcessTask(void *arg);

/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode
DjiTest_UpgradeStartService(const T_DjiTestUpgradePlatformOpt *upgradePlatformOpt,
                            T_DjiTestUpgradeConfig testUpgradeConfig)
{
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    T_DjiReturnCode returnCode;
    bool isUpgradeReboot = false;
    T_DjiUpgradeEndInfo upgradeEndInfo = {0};
    T_DjiUpgradeConfig upgradeConfig = {
        .currentFirmwareVersion = testUpgradeConfig.firmwareVersion,
        .firmwareTransferInfo = {
            .transferType = testUpgradeConfig.transferType,
            .ftpTransferInfo.port = 21,
            .dcftpFileTransferOpt = {
                .start = DjiTestCommonFileTransfer_Start,
                .transfer = DjiTestCommonFileTransfer_Transfer,
                .finish = DjiTestCommonFileTransfer_Finish,
            }
        }
    };
    s_isNeedReplaceProgramBeforeReboot = testUpgradeConfig.needReplaceProgramBeforeReboot;

    T_DjiUpgradeHandler s_upgradeHandler = {
        .EnterUpgradeMode = DjiTest_EnterUpgradeMode,
        .CheckFirmware = DjiTest_CheckFirmware,
        .StartUpgrade = DjiTest_StartUpgrade,
        .FinishUpgrade = DjiTest_FinishUpgrade
    };

    returnCode = DjiTest_RegUpgradePlatformOpt(upgradePlatformOpt);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Reg upgrade platform opt error, return code = 0x%08llX", returnCode);
        return returnCode;
    }

    returnCode = osalHandler->MutexCreate(&s_upgradeStateMutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Create mutex error");
        return returnCode;
    }

    returnCode = DjiTest_GetUpgradeRebootState(&isUpgradeReboot, &upgradeEndInfo);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Get upgrade reboot state error");
        isUpgradeReboot = false;
    }

    returnCode = DjiTest_CleanUpgradeRebootState();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Clean upgrade reboot state error");
    }

    osalHandler->MutexLock(s_upgradeStateMutex);
    if (isUpgradeReboot == true) {
        s_upgradeState.upgradeStage = DJI_UPGRADE_STAGE_END;
        s_upgradeState.upgradeEndInfo = upgradeEndInfo;
    } else {
        s_upgradeState.upgradeStage = DJI_UPGRADE_STAGE_IDLE;
    }
    osalHandler->MutexUnlock(s_upgradeStateMutex);

    returnCode = DjiUpgrade_Init(&upgradeConfig);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("DjiUpgrade_Init error, return code = %d", returnCode);
        return returnCode;
    }

    returnCode = DjiUpgrade_RegHandler(&s_upgradeHandler);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("DjiUpgrade_RegHandler error, return code = %d", returnCode);
        return returnCode;
    }

    returnCode = DjiUpgrade_EnableLocalUpgrade();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("DjiUpgrade_EnableLocalUpgrade error, return code = %d", returnCode);
        return returnCode;
    }

    if (osalHandler->TaskCreate("upgrade_task", DjiTest_UpgradeProcessTask, UPGRADE_TASK_STACK_SIZE, NULL,
                                &s_upgradeProcessThread) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Dji upgrade test task create error.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    if (osalHandler->TaskCreate("enter_upgrade_mode_task", DjiTest_EnterUpgradeModeProcessTask, UPGRADE_TASK_STACK_SIZE,
                                NULL, &s_enterUpgradeModeProcessThread) !=
        DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Dji upgrade test task create error.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/* Private functions definition-----------------------------------------------*/
static T_DjiReturnCode DjiTest_EnterUpgradeMode(uint16_t *waitTime)
{
    // need 10s for upgrade preprocess work.
    *waitTime = DJI_TEST_ENTER_UPGRADE_WAIT_TIME;
    // enable is need enter upgrade mode process, the process is in  DjiTest_EnterUpgradeModeProcessTask
    s_isNeedEnterUpgradeModeProcess = true;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode DjiTest_CheckFirmware(void)
{
    // you can do decrypt and check firmware in this stage
    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode DjiTest_StartUpgrade(void)
{
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    osalHandler->MutexLock(s_upgradeStateMutex);
    s_upgradeState.upgradeOngoingInfo.upgradeProgress = 0;
    s_upgradeState.upgradeStage = DJI_UPGRADE_STAGE_ONGOING;
    osalHandler->MutexUnlock(s_upgradeStateMutex);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode DjiTest_FinishUpgrade(void)
{
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    osalHandler->MutexLock(s_upgradeStateMutex);
    s_upgradeState.upgradeStage = DJI_UPGRADE_STAGE_IDLE;
    osalHandler->MutexUnlock(s_upgradeStateMutex);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

#ifndef __CC_ARM
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wreturn-type"
#endif

static void *DjiTest_EnterUpgradeModeProcessTask(void *arg)
{
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    T_DjiReturnCode returnCode;

    USER_UTIL_UNUSED(arg);

    while (1) {
        if (s_isNeedEnterUpgradeModeProcess) {
            // prepare enter upgrade mode
            // you can do some thing before enter upgrade mode.
            // clear upgrade program file store area
            returnCode = DjiTest_CleanUpgradeProgramFileStoreArea();
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Clean upgrade file dir error, please check dir permission");
            }

            s_isNeedEnterUpgradeModeProcess = false;
            USER_LOG_INFO("Clean upgrade store area");
        }
        osalHandler->TaskSleepMs(1000 / DJI_TEST_UPGRADE_TASK_FREQ);
    }
}

#ifndef __CC_ARM
#pragma GCC diagnostic pop
#endif

#ifndef __CC_ARM
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wreturn-type"
#endif

static void *DjiTest_UpgradeProcessTask(void *arg)
{
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    T_DjiUpgradeState tempUpgradeState;
    T_DjiUpgradeEndInfo upgradeEndInfo;
    T_DjiReturnCode returnCode;

    USER_UTIL_UNUSED(arg);

    while (1) {
        osalHandler->MutexLock(s_upgradeStateMutex);
        tempUpgradeState = s_upgradeState;
        osalHandler->MutexUnlock(s_upgradeStateMutex);

        if (tempUpgradeState.upgradeStage == DJI_UPGRADE_STAGE_ONGOING) {
            if (s_isNeedReplaceProgramBeforeReboot) {
                // Step 1 : Replace old program
                returnCode = DjiTest_ReplaceOldProgram();
                if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    USER_LOG_ERROR("Replace firmware error, return code = 0x%08llX", returnCode);
                    osalHandler->MutexLock(s_upgradeStateMutex);
                    s_upgradeState.upgradeStage = DJI_UPGRADE_STAGE_END;
                    s_upgradeState.upgradeEndInfo.upgradeEndState = DJI_UPGRADE_END_STATE_UNKNOWN_ERROR;
                    osalHandler->MutexUnlock(s_upgradeStateMutex);
                    continue;
                }

                osalHandler->TaskSleepMs(1000);
                osalHandler->MutexLock(s_upgradeStateMutex);
                s_upgradeState.upgradeStage = DJI_UPGRADE_STAGE_ONGOING;
                s_upgradeState.upgradeOngoingInfo.upgradeProgress = 20;
                DjiUpgrade_PushUpgradeState(&s_upgradeState);
                osalHandler->MutexUnlock(s_upgradeStateMutex);

                // Step 2 : Clean upgrade program file store area
                returnCode = DjiTest_CleanUpgradeProgramFileStoreArea();
                if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    USER_LOG_ERROR("Clean upgrade file dir error, please check dir permission");
                    osalHandler->MutexLock(s_upgradeStateMutex);
                    s_upgradeState.upgradeStage = DJI_UPGRADE_STAGE_END;
                    s_upgradeState.upgradeEndInfo.upgradeEndState = DJI_UPGRADE_END_STATE_UNKNOWN_ERROR;
                    osalHandler->MutexUnlock(s_upgradeStateMutex);
                    continue;
                }

                osalHandler->TaskSleepMs(1000);
                osalHandler->MutexLock(s_upgradeStateMutex);
                s_upgradeState.upgradeStage = DJI_UPGRADE_STAGE_ONGOING;
                s_upgradeState.upgradeOngoingInfo.upgradeProgress = 30;
                DjiUpgrade_PushUpgradeState(&s_upgradeState);
                osalHandler->MutexUnlock(s_upgradeStateMutex);
            }

            //attention emulation upgrade progress, user don't need this process
            do {
                osalHandler->TaskSleepMs(1000);
                osalHandler->MutexLock(s_upgradeStateMutex);
                s_upgradeState.upgradeStage = DJI_UPGRADE_STAGE_ONGOING;
                s_upgradeState.upgradeOngoingInfo.upgradeProgress += 10;
                tempUpgradeState = s_upgradeState;
                DjiUpgrade_PushUpgradeState(&s_upgradeState);
                osalHandler->MutexUnlock(s_upgradeStateMutex);
            } while (tempUpgradeState.upgradeOngoingInfo.upgradeProgress < 100);

            // Step 3 : Reboot device
            osalHandler->MutexLock(s_upgradeStateMutex);
            s_upgradeState.upgradeStage = DJI_UPGRADE_STAGE_DEVICE_REBOOT;
            s_upgradeState.upgradeRebootInfo.rebootTimeout = DJI_TEST_UPGRADE_REBOOT_TIMEOUT;
            DjiUpgrade_PushUpgradeState(&s_upgradeState);
            osalHandler->MutexUnlock(s_upgradeStateMutex);
            osalHandler->TaskSleepMs(1000); // sleep 1000ms to ensure push send terminal.

            upgradeEndInfo.upgradeEndState = DJI_UPGRADE_END_STATE_SUCCESS;
            returnCode = DjiTest_SetUpgradeRebootState(&upgradeEndInfo);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Set Upgrade reboot state error");
                osalHandler->MutexLock(s_upgradeStateMutex);
                s_upgradeState.upgradeStage = DJI_UPGRADE_STAGE_END;
                s_upgradeState.upgradeEndInfo.upgradeEndState = DJI_UPGRADE_END_STATE_UNKNOWN_ERROR;
                osalHandler->MutexUnlock(s_upgradeStateMutex);
                continue;
            }

            returnCode = DjiTest_RebootSystem();
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Reboot system error");
                osalHandler->MutexLock(s_upgradeStateMutex);
                s_upgradeState.upgradeStage = DJI_UPGRADE_STAGE_END;
                s_upgradeState.upgradeEndInfo.upgradeEndState = DJI_UPGRADE_END_STATE_UNKNOWN_ERROR;
                osalHandler->MutexUnlock(s_upgradeStateMutex);
                continue;
            }
            while (1) {
                osalHandler->TaskSleepMs(500);
            }
        } else if (s_upgradeState.upgradeStage == DJI_UPGRADE_STAGE_END) {
            osalHandler->MutexLock(s_upgradeStateMutex);
            DjiUpgrade_PushUpgradeState(&s_upgradeState);
            osalHandler->MutexUnlock(s_upgradeStateMutex);
        }

        osalHandler->TaskSleepMs(500);
    }
}

#ifndef __CC_ARM
#pragma GCC diagnostic pop
#endif

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
