/**
 ********************************************************************
 * @file    test_upgrade.c
 * @version V2.0.0
 * @date    3/4/20
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
#include "test_upgrade_common.h"
#include <psdk_logger.h>
#include <psdk_platform.h>
#include <utils/util_misc.h>
#include "test_upgrade_common_file_transfer.h"
#include "test_upgrade_platform_opt.h"

/* Private constants ---------------------------------------------------------*/
#define UPGRADE_TASK_STACK_SIZE                 (2048)
#define PSDK_TEST_UPGRADE_TASK_FREQ             50
#define PSDK_TEST_ENTER_UPGRADE_WAIT_TIME       10  //wait 10s for enter upgrade process
#define PSDK_TEST_UPGRADE_REBOOT_TIMEOUT        30   //reboot timeout 30s

/* Private types -------------------------------------------------------------*/

/* Private values -------------------------------------------------------------*/
static T_PsdkUpgradeState s_upgradeState = {0};
static T_PsdkMutexHandle s_upgradeStateMutex = {0};
static T_PsdkTaskHandle s_upgradeProcessThread;
static T_PsdkTaskHandle s_enterUpgradeModeProcessThread;
static bool s_isNeedEnterUpgradeModeProcess = false;

/* Private functions declaration ---------------------------------------------*/
static T_PsdkReturnCode PsdkTest_EnterUpgradeMode(uint16_t *waitTime);
static T_PsdkReturnCode PsdkTest_CheckFirmware(void);
static T_PsdkReturnCode PsdkTest_StartUpgrade(void);
static T_PsdkReturnCode PsdkTest_FinishUpgrade(void);
static void *PsdkTest_UpgradeProcessTask(void *arg);
static void *PsdkTest_EnterUpgradeModeProcessTask(void *arg);

/* Exported functions definition ---------------------------------------------*/
T_PsdkReturnCode PsdkTest_UpgradeInit(const T_PsdkTestUpgradePlatformOpt *upgradePlatformOpt)
{
    T_PsdkReturnCode returnCode;
    bool isUpgradeReboot = false;
    T_PsdkUpgradeEndInfo upgradeEndInfo = {0};
    T_PsdkUpgradeConfig upgradeConfig = {
        .currentFirmwareVersion = {PSDK_TEST_FIRMWARE_VERSION_MAJOR, PSDK_TEST_FIRMWARE_VERSION_MINOR,
                                   PSDK_TEST_FIRMWARE_VERSION_MODIFY, PSDK_TEST_FIRMWARE_VERSION_DEBUG},
#if PSDK_TEST_UPGRADE_USE_FTP_TRANSFER
        .firmwareTransferInfo = {
            .transferType = PSDK_PAYLOAD_FIRMWARE_TRANSFER_TYPE_FTP,
            .ftpTransferInfo.port = 21, // use default ftp port
        }
#else
    .firmwareTransferInfo = {
        .transferType = PSDK_PAYLOAD_FIRMWARE_TRANSFER_TYPE_DCFTP,
        .dcftpFileTransferOpt = {
            .start = PsdkTestCommonFileTransfer_Start,
            .transfer = PsdkTestCommonFileTransfer_Transfer,
            .finish = PsdkTestCommonFileTransfer_Finish,
        }
    }
#endif
    };

    T_PsdkUpgradeHandler s_upgradeHandler = {
        .EnterUpgradeMode = PsdkTest_EnterUpgradeMode,
        .CheckFirmware = PsdkTest_CheckFirmware,
        .StartUpgrade = PsdkTest_StartUpgrade,
        .FinishUpgrade = PsdkTest_FinishUpgrade
    };

    returnCode = PsdkTest_RegUpgradePlatformOpt(upgradePlatformOpt);
    if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Reg upgrade platform opt error, return code = 0x%08llX", returnCode);
        return returnCode;
    }

    returnCode = PsdkOsal_MutexCreate(&s_upgradeStateMutex);
    if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Create mutex error");
        return returnCode;
    }

    returnCode = PsdkTest_GetUpgradeRebootState(&isUpgradeReboot, &upgradeEndInfo);
    if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Get upgrade reboot state error");
        isUpgradeReboot = false;
    }

    returnCode = PsdkTest_CleanUpgradeRebootState();
    if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Clean upgrade reboot state error");
    }

    PsdkOsal_MutexLock(s_upgradeStateMutex);
    if (isUpgradeReboot == true) {
        s_upgradeState.upgradeStage = PSDK_UPGRADE_STAGE_END;
        s_upgradeState.upgradeEndInfo = upgradeEndInfo;
    } else {
        s_upgradeState.upgradeStage = PSDK_UPGRADE_STAGE_IDLE;
    }
    PsdkOsal_MutexUnlock(s_upgradeStateMutex);

    returnCode = PsdkUpgrade_Init(&upgradeConfig);
    if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("PsdkUpgrade_Init error, return code = %d", returnCode);
        return returnCode;
    }

    returnCode = PsdkUpgrade_RegHandler(&s_upgradeHandler);
    if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("PsdkUpgrade_RegHandler error, return code = %d", returnCode);
        return returnCode;
    }

    returnCode = PsdkUpgrade_EnableLocalUpgrade();
    if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("PsdkUpgrade_EnableLocalUpgrade error, return code = %d", returnCode);
        return returnCode;
    }

    if (PsdkOsal_TaskCreate(&s_upgradeProcessThread, PsdkTest_UpgradeProcessTask, "upgrade_task",
                            UPGRADE_TASK_STACK_SIZE, NULL) != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Psdk upgrade test task create error.");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    if (PsdkOsal_TaskCreate(&s_enterUpgradeModeProcessThread, PsdkTest_EnterUpgradeModeProcessTask,
                            "enter_upgrade_mode_task", UPGRADE_TASK_STACK_SIZE, NULL) !=
        PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Psdk upgrade test task create error.");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/* Private functions definition-----------------------------------------------*/
static T_PsdkReturnCode PsdkTest_EnterUpgradeMode(uint16_t *waitTime)
{
    // need 10s for upgrade preprocess work.
    *waitTime = PSDK_TEST_ENTER_UPGRADE_WAIT_TIME;
    // enable is need enter upgrade mode process, the process is in  PsdkTest_EnterUpgradeModeProcessTask
    s_isNeedEnterUpgradeModeProcess = true;

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode PsdkTest_CheckFirmware(void)
{
    // you can do decrypt and check firmware in this stage
    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode PsdkTest_StartUpgrade(void)
{
    PsdkOsal_MutexLock(s_upgradeStateMutex);
    s_upgradeState.upgradeOngoingInfo.upgradeProgress = 0;
    s_upgradeState.upgradeStage = PSDK_UPGRADE_STAGE_ONGOING;
    PsdkOsal_MutexUnlock(s_upgradeStateMutex);

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode PsdkTest_FinishUpgrade(void)
{
    PsdkOsal_MutexLock(s_upgradeStateMutex);
    s_upgradeState.upgradeStage = PSDK_UPGRADE_STAGE_IDLE;
    PsdkOsal_MutexUnlock(s_upgradeStateMutex);

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

#ifndef __CC_ARM
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wreturn-type"
#endif

static void *PsdkTest_EnterUpgradeModeProcessTask(void *arg)
{
    T_PsdkReturnCode returnCode;

    USER_UTIL_UNUSED(arg);

    while (1) {
        if (s_isNeedEnterUpgradeModeProcess) {
            // prepare enter upgrade mode
            // you can do some thing before enter upgrade mode.
            // clear upgrade program file store area
            returnCode = PsdkTest_CleanUpgradeProgramFileStoreArea();
            if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                PsdkLogger_UserLogError("Clean upgrade file dir error, please check dir permission");
            }

            s_isNeedEnterUpgradeModeProcess = false;
        }
        PsdkOsal_TaskSleepMs(1000 / PSDK_TEST_UPGRADE_TASK_FREQ);
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

static void *PsdkTest_UpgradeProcessTask(void *arg)
{
    T_PsdkUpgradeState tempUpgradeState;
    T_PsdkUpgradeEndInfo upgradeEndInfo;
    T_PsdkReturnCode returnCode;

    USER_UTIL_UNUSED(arg);

    while (1) {
        PsdkOsal_MutexLock(s_upgradeStateMutex);
        tempUpgradeState = s_upgradeState;
        PsdkOsal_MutexUnlock(s_upgradeStateMutex);

        if (tempUpgradeState.upgradeStage == PSDK_UPGRADE_STAGE_ONGOING) {
#if PSDK_TEST_REPLACE_PROGRAM_BEFORE_REBOOT
            // Step 1 : Replace old program
            returnCode = PsdkTest_ReplaceOldProgram();
            if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                PsdkLogger_UserLogError("Replace firmware error, return code = 0x%08llX", returnCode);
                PsdkOsal_MutexLock(s_upgradeStateMutex);
                s_upgradeState.upgradeStage = PSDK_UPGRADE_STAGE_END;
                s_upgradeState.upgradeEndInfo.upgradeEndState = PSDK_UPGRADE_END_STATE_UNKNOWN_ERROR;
                PsdkOsal_MutexUnlock(s_upgradeStateMutex);
                continue;
            }

            PsdkOsal_TaskSleepMs(1000);
            PsdkOsal_MutexLock(s_upgradeStateMutex);
            s_upgradeState.upgradeStage = PSDK_UPGRADE_STAGE_ONGOING;
            s_upgradeState.upgradeOngoingInfo.upgradeProgress = 20;
            PsdkUpgrade_PushUpgradeState(&s_upgradeState);
            PsdkOsal_MutexUnlock(s_upgradeStateMutex);

            // Step 2 : Clean upgrade program file store area
            returnCode = PsdkTest_CleanUpgradeProgramFileStoreArea();
            if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                PsdkLogger_UserLogError("Clean upgrade file dir error, please check dir permission");
                PsdkOsal_MutexLock(s_upgradeStateMutex);
                s_upgradeState.upgradeStage = PSDK_UPGRADE_STAGE_END;
                s_upgradeState.upgradeEndInfo.upgradeEndState = PSDK_UPGRADE_END_STATE_UNKNOWN_ERROR;
                PsdkOsal_MutexUnlock(s_upgradeStateMutex);
                continue;
            }

            PsdkOsal_TaskSleepMs(1000);
            PsdkOsal_MutexLock(s_upgradeStateMutex);
            s_upgradeState.upgradeStage = PSDK_UPGRADE_STAGE_ONGOING;
            s_upgradeState.upgradeOngoingInfo.upgradeProgress = 30;
            PsdkUpgrade_PushUpgradeState(&s_upgradeState);
            PsdkOsal_MutexUnlock(s_upgradeStateMutex);
#endif
            //attention emulation upgrade progress, user don't need this process
            do {
                PsdkOsal_TaskSleepMs(1000);
                PsdkOsal_MutexLock(s_upgradeStateMutex);
                s_upgradeState.upgradeStage = PSDK_UPGRADE_STAGE_ONGOING;
                s_upgradeState.upgradeOngoingInfo.upgradeProgress += 10;
                tempUpgradeState = s_upgradeState;
                PsdkUpgrade_PushUpgradeState(&s_upgradeState);
                PsdkOsal_MutexUnlock(s_upgradeStateMutex);
            } while (tempUpgradeState.upgradeOngoingInfo.upgradeProgress < 100);

            // Step 3 : Reboot device
            PsdkOsal_MutexLock(s_upgradeStateMutex);
            s_upgradeState.upgradeStage = PSDK_UPGRADE_STAGE_DEVICE_REBOOT;
            s_upgradeState.upgradeRebootInfo.rebootTimeout = PSDK_TEST_UPGRADE_REBOOT_TIMEOUT;
            PsdkUpgrade_PushUpgradeState(&s_upgradeState);
            PsdkOsal_MutexUnlock(s_upgradeStateMutex);
            PsdkOsal_TaskSleepMs(1000); // sleep 1000ms to ensure push send terminal.

            upgradeEndInfo.upgradeEndState = PSDK_UPGRADE_END_STATE_SUCCESS;
            returnCode = PsdkTest_SetUpgradeRebootState(&upgradeEndInfo);
            if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                PsdkLogger_UserLogError("Set Upgrade reboot state error");
                PsdkOsal_MutexLock(s_upgradeStateMutex);
                s_upgradeState.upgradeStage = PSDK_UPGRADE_STAGE_END;
                s_upgradeState.upgradeEndInfo.upgradeEndState = PSDK_UPGRADE_END_STATE_UNKNOWN_ERROR;
                PsdkOsal_MutexUnlock(s_upgradeStateMutex);
                continue;
            }

#if PSDK_TEST_UPGRADE_NO_REBOOT_EMU
            bool isUpgradeReboot;
            returnCode = PsdkTest_GetUpgradeRebootState(&isUpgradeReboot, &upgradeEndInfo);
            if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                PsdkLogger_UserLogError("Get upgrade reboot state error");
                PsdkOsal_MutexLock(s_upgradeStateMutex);
                s_upgradeState.upgradeStage = PSDK_UPGRADE_STAGE_END;
                s_upgradeState.upgradeEndInfo.upgradeEndState = PSDK_UPGRADE_END_STATE_UNKNOWN_ERROR;
                PsdkOsal_MutexUnlock(s_upgradeStateMutex);
                continue;
            }

            returnCode = PsdkTest_CleanUpgradeRebootState();
            if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                PsdkLogger_UserLogError("Clean upgrade reboot state error");
                PsdkOsal_MutexLock(s_upgradeStateMutex);
                s_upgradeState.upgradeStage = PSDK_UPGRADE_STAGE_END;
                s_upgradeState.upgradeEndInfo.upgradeEndState = PSDK_UPGRADE_END_STATE_UNKNOWN_ERROR;
                PsdkOsal_MutexUnlock(s_upgradeStateMutex);
                continue;
            }

            PsdkOsal_MutexLock(s_upgradeStateMutex);
            if (isUpgradeReboot == true) {
                s_upgradeState.upgradeStage = PSDK_UPGRADE_STAGE_END;
                s_upgradeState.upgradeEndInfo = upgradeEndInfo;
            } else {
                s_upgradeState.upgradeStage = PSDK_UPGRADE_STAGE_IDLE;
            }
            PsdkOsal_MutexUnlock(s_upgradeStateMutex);
            continue;
#else
            returnCode = PsdkTest_RebootSystem();
            if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                PsdkLogger_UserLogError("Reboot system error");
                PsdkOsal_MutexLock(s_upgradeStateMutex);
                s_upgradeState.upgradeStage = PSDK_UPGRADE_STAGE_END;
                s_upgradeState.upgradeEndInfo.upgradeEndState = PSDK_UPGRADE_END_STATE_UNKNOWN_ERROR;
                PsdkOsal_MutexUnlock(s_upgradeStateMutex);
                continue;
            }
            while (1) {
                PsdkOsal_TaskSleepMs(500);
            }
#endif
        } else if (s_upgradeState.upgradeStage == PSDK_UPGRADE_STAGE_END) {
            PsdkOsal_MutexLock(s_upgradeStateMutex);
            PsdkUpgrade_PushUpgradeState(&s_upgradeState);
            PsdkOsal_MutexUnlock(s_upgradeStateMutex);
        }

        PsdkOsal_TaskSleepMs(500);
    }
}

#ifndef __CC_ARM
#pragma GCC diagnostic pop
#endif

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
