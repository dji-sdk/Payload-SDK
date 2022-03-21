/**
 ********************************************************************
 * @file    upgrade_platform_opt_linux.c
 * @version V2.0.0
 * @date    3/25/20
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
#include "upgrade_platform_opt_linux.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dji_logger.h>
#include <dji_upgrade.h>

/* Private constants ---------------------------------------------------------*/
#define DJI_TEST_CMD_CALL_MAX_LEN              (DJI_FILE_PATH_SIZE_MAX + 256)
#define DJI_REBOOT_STATE_FILE_NAME             "reboot_state"

/* Private types -------------------------------------------------------------*/

/* Private values -------------------------------------------------------------*/
static FILE *s_upgradeProgramFile = NULL;

/* Private functions declaration ---------------------------------------------*/
static T_DjiReturnCode DjiTest_RunSystemCmd(char *systemCmdStr);

/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode DjiUpgradePlatformLinux_RebootSystem(void)
{
    // attention: you need su permission to reboot system
    return DjiTest_RunSystemCmd("reboot -h now");
}

T_DjiReturnCode DjiUpgradePlatformLinux_CleanUpgradeProgramFileStoreArea(void)
{
    char cmdBuffer[DJI_TEST_CMD_CALL_MAX_LEN];

    snprintf(cmdBuffer, DJI_TEST_CMD_CALL_MAX_LEN, "rm -rf %s*", DJI_TEST_UPGRADE_FILE_DIR);

    return DjiTest_RunSystemCmd(cmdBuffer);
}

T_DjiReturnCode DjiUpgradePlatformLinux_ReplaceOldProgram(void)
{
    char cmdBuffer[DJI_TEST_CMD_CALL_MAX_LEN];

    snprintf(cmdBuffer, DJI_TEST_CMD_CALL_MAX_LEN, "cp -f %s*_V*.*.*.bin %s", DJI_TEST_UPGRADE_FILE_DIR,
             DJI_TEST_UPGRADE_OLD_FIRMWARE_PATH);

    return DjiTest_RunSystemCmd(cmdBuffer);
}

T_DjiReturnCode DjiUpgradePlatformLinux_SetUpgradeRebootState(const T_DjiUpgradeEndInfo *upgradeEndInfo)
{
    FILE *rebootStateFile;
    size_t res;
    T_DjiReturnCode returnCode;

    rebootStateFile = fopen(DJI_REBOOT_STATE_FILE_NAME, "w+");
    if (rebootStateFile == NULL) {
        USER_LOG_ERROR("Create reboot state file error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    res = fwrite((uint8_t *) upgradeEndInfo, 1, sizeof(T_DjiUpgradeEndInfo), rebootStateFile);
    if (res != sizeof(T_DjiUpgradeEndInfo)) {
        USER_LOG_ERROR("Write data len is not equal");
        returnCode = DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
        goto out;
    }
    returnCode = DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;

out:
    fclose(rebootStateFile);
    return returnCode;
}

T_DjiReturnCode DjiUpgradePlatformLinux_GetUpgradeRebootState(bool *isUpgradeReboot,
                                                              T_DjiUpgradeEndInfo *upgradeEndInfo)
{
    FILE *rebootStateFile;
    size_t res;

    rebootStateFile = fopen(DJI_REBOOT_STATE_FILE_NAME, "r");
    if (rebootStateFile == NULL) {
        *isUpgradeReboot = false;
        return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    }

    res = fread(upgradeEndInfo, 1, sizeof(T_DjiUpgradeEndInfo), rebootStateFile);
    if (res != sizeof(T_DjiUpgradeEndInfo)) {
        USER_LOG_ERROR("Read data len is not equal");
        *isUpgradeReboot = false;
        goto out;
    }
    *isUpgradeReboot = true;

out:
    fclose(rebootStateFile);
    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiUpgradePlatformLinux_CleanUpgradeRebootState(void)
{
    return DjiTest_RunSystemCmd("rm -f "DJI_REBOOT_STATE_FILE_NAME);
}

T_DjiReturnCode DjiUpgradePlatformLinux_CreateUpgradeProgramFile(const T_DjiUpgradeFileInfo *fileInfo)
{
    char filePath[DJI_FILE_PATH_SIZE_MAX];

    s_upgradeProgramFile = NULL;
    snprintf(filePath, DJI_FILE_PATH_SIZE_MAX, "%s%s", DJI_TEST_UPGRADE_FILE_DIR, fileInfo->fileName);
    s_upgradeProgramFile = fopen(filePath, "w+");
    if (s_upgradeProgramFile == NULL) {
        USER_LOG_ERROR("Upgrade program file can't create");
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode
DjiUpgradePlatformLinux_WriteUpgradeProgramFile(uint32_t offset, const uint8_t *data, uint16_t dataLen)
{
    size_t writeLen;

    if (s_upgradeProgramFile == NULL) {
        USER_LOG_ERROR("upgrade program file can't be NULL");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    if (fseek(s_upgradeProgramFile, offset, SEEK_SET) != 0) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }
    writeLen = fwrite(data, 1, dataLen, s_upgradeProgramFile);
    if (writeLen != dataLen) {
        USER_LOG_ERROR("Write upgrade program file error, writeLen = %d, dataLen = %d", writeLen);
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiUpgradePlatformLinux_ReadUpgradeProgramFile(uint32_t offset, uint16_t readDataLen, uint8_t *data,
                                                               uint16_t *realLen)
{
    uint32_t readRtn;

    if (s_upgradeProgramFile == NULL) {
        USER_LOG_ERROR("upgrade program file can't be NULL");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    if (fseek(s_upgradeProgramFile, offset, SEEK_SET) != 0) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    readRtn = fread(data, 1, readDataLen, s_upgradeProgramFile);
    if (readRtn == 0 || readRtn > readDataLen) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }
    *realLen = readRtn;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiUpgradePlatformLinux_CloseUpgradeProgramFile(void)
{
    if (s_upgradeProgramFile == NULL) {
        USER_LOG_ERROR("upgrade program file can't be NULL");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    fclose(s_upgradeProgramFile);
    s_upgradeProgramFile = NULL;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/* Private functions definition-----------------------------------------------*/
static T_DjiReturnCode DjiTest_RunSystemCmd(char *systemCmdStr)
{
    int status;

    status = system(systemCmdStr);
    if (status == -1 || WIFEXITED(status) == false) {
        USER_LOG_ERROR("Call %s error, status = %d", systemCmdStr, status);
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    if (WEXITSTATUS(status) == 0) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    } else {
        USER_LOG_ERROR("Exit status is = %d", WEXITSTATUS(status));
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }
}
/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
