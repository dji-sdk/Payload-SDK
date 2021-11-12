/**
 ********************************************************************
 * @file    test_upgrade_platform_opt.c
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
#include "test_upgrade_platform_opt.h"
#include <dji_logger.h>

/* Private constants ---------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private values -------------------------------------------------------------*/
static T_DjiTestUpgradePlatformOpt s_upgradePlatformOpt = {0};

/* Private functions declaration ---------------------------------------------*/

/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode DjiTest_RegUpgradePlatformOpt(const T_DjiTestUpgradePlatformOpt *upgradePlatformOpt)
{
    if (upgradePlatformOpt->rebootSystem == NULL) {
        USER_LOG_ERROR("rebootSystem callback can't be NULL");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }
    if (upgradePlatformOpt->cleanUpgradeProgramFileStoreArea == NULL) {
        USER_LOG_ERROR("cleanUpgradeProgramFileStoreArea callback can't be NULL");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }
    if (upgradePlatformOpt->createUpgradeProgramFile == NULL) {
        USER_LOG_ERROR("createUpgradeProgramFile callback can't be NULL");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }
    if (upgradePlatformOpt->readUpgradeProgramFile == NULL) {
        USER_LOG_ERROR("readUpgradeProgramFile callback can't be NULL");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }
    if (upgradePlatformOpt->writeUpgradeProgramFile == NULL) {
        USER_LOG_ERROR("writeUpgradeProgramFile callback can't be NULL");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }
    if (upgradePlatformOpt->closeUpgradeProgramFile == NULL) {
        USER_LOG_ERROR("closeUpgradeProgramFile callback can't be NULL");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }
    if (upgradePlatformOpt->replaceOldProgram == NULL) {
        USER_LOG_ERROR("replaceOldProgram callback can't be NULL");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }
    if (upgradePlatformOpt->setUpgradeRebootState == NULL) {
        USER_LOG_ERROR("setUpgradeRebootState callback can't be NULL");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }
    if (upgradePlatformOpt->getUpgradeRebootState == NULL) {
        USER_LOG_ERROR("getUpgradeRebootState callback can't be NULL");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }
    if (upgradePlatformOpt->cleanUpgradeRebootState == NULL) {
        USER_LOG_ERROR("cleanUpgradeRebootState callback can't be NULL");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    s_upgradePlatformOpt = *upgradePlatformOpt;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiTest_RebootSystem(void)
{
    return s_upgradePlatformOpt.rebootSystem();
}

T_DjiReturnCode DjiTest_CleanUpgradeProgramFileStoreArea(void)
{
    return s_upgradePlatformOpt.cleanUpgradeProgramFileStoreArea();
}

T_DjiReturnCode DjiTest_CreateUpgradeProgramFile(const T_DjiUpgradeFileInfo *fileInfo)
{
    return s_upgradePlatformOpt.createUpgradeProgramFile(fileInfo);
}

T_DjiReturnCode DjiTest_WriteUpgradeProgramFile(uint32_t offset, const uint8_t *data, uint16_t dataLen)
{
    return s_upgradePlatformOpt.writeUpgradeProgramFile(offset, data, dataLen);
}

T_DjiReturnCode DjiTest_ReadUpgradeProgramFile(uint32_t offset, uint16_t readDataLen, uint8_t *data,
                                                 uint16_t *realLen)
{
    return s_upgradePlatformOpt.readUpgradeProgramFile(offset, readDataLen, data, realLen);
}

T_DjiReturnCode DjiTest_CloseUpgradeProgramFile(void)
{
    return s_upgradePlatformOpt.closeUpgradeProgramFile();
}

T_DjiReturnCode DjiTest_ReplaceOldProgram(void)
{
    return s_upgradePlatformOpt.replaceOldProgram();
}

T_DjiReturnCode DjiTest_SetUpgradeRebootState(const T_DjiUpgradeEndInfo *upgradeEndInfo)
{
    return s_upgradePlatformOpt.setUpgradeRebootState(upgradeEndInfo);
}

T_DjiReturnCode DjiTest_GetUpgradeRebootState(bool *isUpgradeReboot, T_DjiUpgradeEndInfo *upgradeEndInfo)
{
    return s_upgradePlatformOpt.getUpgradeRebootState(isUpgradeReboot, upgradeEndInfo);
}

T_DjiReturnCode DjiTest_CleanUpgradeRebootState(void)
{
    return s_upgradePlatformOpt.cleanUpgradeRebootState();
}

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
