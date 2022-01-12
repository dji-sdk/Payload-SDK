/**
 ********************************************************************
 * @file    test_upgrade_platform_opt.c
 * @version V1.0.0
 * @date    2019/01/01
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
#include "test_upgrade_platform_opt.h"
#include <psdk_logger.h>

/* Private constants ---------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private values -------------------------------------------------------------*/
static T_PsdkTestUpgradePlatformOpt s_upgradePlatformOpt = {0};

/* Private functions declaration ---------------------------------------------*/

/* Exported functions definition ---------------------------------------------*/
T_PsdkReturnCode PsdkTest_RegUpgradePlatformOpt(const T_PsdkTestUpgradePlatformOpt *upgradePlatformOpt)
{
    if (upgradePlatformOpt->rebootSystem == NULL) {
        PsdkLogger_UserLogError("rebootSystem callback can't be NULL");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }
    if (upgradePlatformOpt->cleanUpgradeProgramFileStoreArea == NULL) {
        PsdkLogger_UserLogError("cleanUpgradeProgramFileStoreArea callback can't be NULL");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }
    if (upgradePlatformOpt->createUpgradeProgramFile == NULL) {
        PsdkLogger_UserLogError("createUpgradeProgramFile callback can't be NULL");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }
    if (upgradePlatformOpt->readUpgradeProgramFile == NULL) {
        PsdkLogger_UserLogError("readUpgradeProgramFile callback can't be NULL");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }
    if (upgradePlatformOpt->writeUpgradeProgramFile == NULL) {
        PsdkLogger_UserLogError("writeUpgradeProgramFile callback can't be NULL");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }
    if (upgradePlatformOpt->closeUpgradeProgramFile == NULL) {
        PsdkLogger_UserLogError("closeUpgradeProgramFile callback can't be NULL");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }
    if (upgradePlatformOpt->replaceOldProgram == NULL) {
        PsdkLogger_UserLogError("replaceOldProgram callback can't be NULL");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }
    if (upgradePlatformOpt->setUpgradeRebootState == NULL) {
        PsdkLogger_UserLogError("setUpgradeRebootState callback can't be NULL");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }
    if (upgradePlatformOpt->getUpgradeRebootState == NULL) {
        PsdkLogger_UserLogError("getUpgradeRebootState callback can't be NULL");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }
    if (upgradePlatformOpt->cleanUpgradeRebootState == NULL) {
        PsdkLogger_UserLogError("cleanUpgradeRebootState callback can't be NULL");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    s_upgradePlatformOpt = *upgradePlatformOpt;

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_PsdkReturnCode PsdkTest_RebootSystem(void)
{
    return s_upgradePlatformOpt.rebootSystem();
}

T_PsdkReturnCode PsdkTest_CleanUpgradeProgramFileStoreArea(void)
{
    return s_upgradePlatformOpt.cleanUpgradeProgramFileStoreArea();
}

T_PsdkReturnCode PsdkTest_CreateUpgradeProgramFile(const T_PsdkUpgradeFileInfo *fileInfo)
{
    return s_upgradePlatformOpt.createUpgradeProgramFile(fileInfo);
}

T_PsdkReturnCode PsdkTest_WriteUpgradeProgramFile(uint32_t offset, const uint8_t *data, uint16_t dataLen)
{
    return s_upgradePlatformOpt.writeUpgradeProgramFile(offset, data, dataLen);
}

T_PsdkReturnCode PsdkTest_ReadUpgradeProgramFile(uint32_t offset, uint16_t readDataLen, uint8_t *data,
                                                 uint16_t *realLen)
{
    return s_upgradePlatformOpt.readUpgradeProgramFile(offset, readDataLen, data, realLen);
}

T_PsdkReturnCode PsdkTest_CloseUpgradeProgramFile(void)
{
    return s_upgradePlatformOpt.closeUpgradeProgramFile();
}

T_PsdkReturnCode PsdkTest_ReplaceOldProgram(void)
{
    return s_upgradePlatformOpt.replaceOldProgram();
}

T_PsdkReturnCode PsdkTest_SetUpgradeRebootState(const T_PsdkUpgradeEndInfo *upgradeEndInfo)
{
    return s_upgradePlatformOpt.setUpgradeRebootState(upgradeEndInfo);
}

T_PsdkReturnCode PsdkTest_GetUpgradeRebootState(bool *isUpgradeReboot, T_PsdkUpgradeEndInfo *upgradeEndInfo)
{
    return s_upgradePlatformOpt.getUpgradeRebootState(isUpgradeReboot, upgradeEndInfo);
}

T_PsdkReturnCode PsdkTest_CleanUpgradeRebootState(void)
{
    return s_upgradePlatformOpt.cleanUpgradeRebootState();
}

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
