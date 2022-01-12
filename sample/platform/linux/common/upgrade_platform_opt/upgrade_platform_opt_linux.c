/**
 ********************************************************************
 * @file    upgrade_platform_opt_linux.c
 * @version V2.0.0
 * @date    3/25/20
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
#include "upgrade_platform_opt_linux.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <psdk_logger.h>
#include <psdk_upgrade.h>

/* Private constants ---------------------------------------------------------*/
#define PSDK_TEST_CMD_CALL_MAX_LEN              (PSDK_FILE_PATH_SIZE_MAX + 256)
#define PSDK_REBOOT_STATE_FILE_NAME             "reboot_state"

/* Private types -------------------------------------------------------------*/

/* Private values -------------------------------------------------------------*/
static FILE *s_upgradeProgramFile = NULL;

/* Private functions declaration ---------------------------------------------*/
static T_PsdkReturnCode PsdkTest_RunSystemCmd(char *systemCmdStr);

/* Exported functions definition ---------------------------------------------*/
T_PsdkReturnCode PsdkUpgradePlatformLinux_RebootSystem(void)
{
    // attention: you need su permission to reboot system
    return PsdkTest_RunSystemCmd("reboot -h now");
}

T_PsdkReturnCode PsdkUpgradePlatformLinux_CleanUpgradeProgramFileStoreArea(void)
{
    char cmdBuffer[PSDK_TEST_CMD_CALL_MAX_LEN];

    snprintf(cmdBuffer, PSDK_TEST_CMD_CALL_MAX_LEN, "rm -rf %s*", PSDK_TEST_UPGRADE_FILE_DIR);

    return PsdkTest_RunSystemCmd(cmdBuffer);
}

T_PsdkReturnCode PsdkUpgradePlatformLinux_ReplaceOldProgram(void)
{
    char cmdBuffer[PSDK_TEST_CMD_CALL_MAX_LEN];

    snprintf(cmdBuffer, PSDK_TEST_CMD_CALL_MAX_LEN, "cp -f %s*_V*.*.*.bin %s", PSDK_TEST_UPGRADE_FILE_DIR,
             PSDK_TEST_UPGRADE_OLD_FIRMWARE_PATH);

    return PsdkTest_RunSystemCmd(cmdBuffer);
}

T_PsdkReturnCode PsdkUpgradePlatformLinux_SetUpgradeRebootState(const T_PsdkUpgradeEndInfo *upgradeEndInfo)
{
    FILE *rebootStateFile;
    size_t res;
    T_PsdkReturnCode returnCode;

    rebootStateFile = fopen(PSDK_REBOOT_STATE_FILE_NAME, "w+");
    if (rebootStateFile == NULL) {
        PsdkLogger_UserLogError("Create reboot state file error");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    res = fwrite((uint8_t *) upgradeEndInfo, 1, sizeof(T_PsdkUpgradeEndInfo), rebootStateFile);
    if (res != sizeof(T_PsdkUpgradeEndInfo)) {
        PsdkLogger_UserLogError("Write data len is not equal");
        returnCode = PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
        goto out;
    }
    returnCode = PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;

out:
    fclose(rebootStateFile);
    return returnCode;
}

T_PsdkReturnCode PsdkUpgradePlatformLinux_GetUpgradeRebootState(bool *isUpgradeReboot,
                                                                T_PsdkUpgradeEndInfo *upgradeEndInfo)
{
    FILE *rebootStateFile;
    size_t res;

    rebootStateFile = fopen(PSDK_REBOOT_STATE_FILE_NAME, "r");
    if (rebootStateFile == NULL) {
        *isUpgradeReboot = false;
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    }

    res = fread(upgradeEndInfo, 1, sizeof(T_PsdkUpgradeEndInfo), rebootStateFile);
    if (res != sizeof(T_PsdkUpgradeEndInfo)) {
        PsdkLogger_UserLogError("Read data len is not equal");
        *isUpgradeReboot = false;
        goto out;
    }
    *isUpgradeReboot = true;

out:
    fclose(rebootStateFile);
    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_PsdkReturnCode PsdkUpgradePlatformLinux_CleanUpgradeRebootState(void)
{
    return PsdkTest_RunSystemCmd("rm -f "PSDK_REBOOT_STATE_FILE_NAME);
}

T_PsdkReturnCode PsdkUpgradePlatformLinux_CreateUpgradeProgramFile(const T_PsdkUpgradeFileInfo *fileInfo)
{
    char filePath[PSDK_FILE_PATH_SIZE_MAX];

    s_upgradeProgramFile = NULL;
    snprintf(filePath, PSDK_FILE_PATH_SIZE_MAX, "%s%s", PSDK_TEST_UPGRADE_FILE_DIR, fileInfo->fileName);
    s_upgradeProgramFile = fopen(filePath, "w+");
    if (s_upgradeProgramFile == NULL) {
        PsdkLogger_UserLogError("Upgrade program file can't create");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_PsdkReturnCode
PsdkUpgradePlatformLinux_WriteUpgradeProgramFile(uint32_t offset, const uint8_t *data, uint16_t dataLen)
{
    size_t writeLen;

    if (s_upgradeProgramFile == NULL) {
        PsdkLogger_UserLogError("upgrade program file can't be NULL");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    if (fseek(s_upgradeProgramFile, offset, SEEK_SET) != 0) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }
    writeLen = fwrite(data, 1, dataLen, s_upgradeProgramFile);
    if (writeLen != dataLen) {
        PsdkLogger_UserLogError("Write upgrade program file error, writeLen = %d, dataLen = %d", writeLen);
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_PsdkReturnCode PsdkUpgradePlatformLinux_ReadUpgradeProgramFile(uint32_t offset, uint16_t readDataLen, uint8_t *data,
                                                                 uint16_t *realLen)
{
    uint32_t readRtn;

    if (s_upgradeProgramFile == NULL) {
        PsdkLogger_UserLogError("upgrade program file can't be NULL");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    if (fseek(s_upgradeProgramFile, offset, SEEK_SET) != 0) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    readRtn = fread(data, 1, readDataLen, s_upgradeProgramFile);
    if (readRtn == 0 || readRtn > readDataLen) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }
    *realLen = readRtn;

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_PsdkReturnCode PsdkUpgradePlatformLinux_CloseUpgradeProgramFile(void)
{
    if (s_upgradeProgramFile == NULL) {
        PsdkLogger_UserLogError("upgrade program file can't be NULL");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    fclose(s_upgradeProgramFile);
    s_upgradeProgramFile = NULL;

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/* Private functions definition-----------------------------------------------*/
static T_PsdkReturnCode PsdkTest_RunSystemCmd(char *systemCmdStr)
{
    int status;

    status = system(systemCmdStr);
    if (status == -1 || WIFEXITED(status) == false) {
        PsdkLogger_UserLogError("Call %s error, status = %d", systemCmdStr, status);
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    if (WEXITSTATUS(status) == 0) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    } else {
        PsdkLogger_UserLogError("Exit status is = %d", WEXITSTATUS(status));
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }
}
/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
