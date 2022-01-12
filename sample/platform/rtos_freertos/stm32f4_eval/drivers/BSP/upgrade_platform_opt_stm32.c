/**
 ********************************************************************
 * @file    upgrade_platform_opt_stm32.c
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
#include "upgrade_platform_opt_stm32.h"
#include <stm32f4xx_hal.h>
#include <flash_if.h>

/* Private constants ---------------------------------------------------------*/
#define PSDK_TEST_UPGRADE_FILE_INFO_STORE_ADDR      (APPLICATION_STORE_ADDRESS_END - 1023)
#define PSDK_TEST_UPGRADE_REBOOT_KEY                0x11223344

/* Private types -------------------------------------------------------------*/
typedef struct {
    uint32_t upgradeRebootKey;
    T_PsdkUpgradeEndInfo upgradeEndInfo;
} T_PsdkTestUpgradeRebootParam;

/* Private values -------------------------------------------------------------*/
static T_PsdkUpgradeFileInfo s_upgradeFileInfo = {0};

/* Private functions declaration ---------------------------------------------*/

/* Exported functions definition ---------------------------------------------*/
T_PsdkReturnCode PsdkUpgradePlatformStm32_RebootSystem(void)
{
	  __disable_irq();
	  __disable_fiq();
    NVIC_SystemReset();

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_PsdkReturnCode PsdkUpgradePlatformStm32_CleanUpgradeProgramFileStoreArea(void)
{

    uint32_t result;

    result = FLASH_If_Erase(APPLICATION_STORE_ADDRESS, APPLICATION_STORE_ADDRESS_END);
    if (result != FLASHIF_OK) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }
    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_PsdkReturnCode PsdkUpgradePlatformStm32_CreateUpgradeProgramFile(const T_PsdkUpgradeFileInfo *fileInfo)
{
    s_upgradeFileInfo = *fileInfo;

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_PsdkReturnCode PsdkUpgradePlatformStm32_WriteUpgradeProgramFile(uint32_t offset, const uint8_t *data,
                                                                  uint16_t dataLen)
{
    uint32_t result;

    result = FLASH_If_Write(APPLICATION_STORE_ADDRESS + offset, (uint8_t *) data, dataLen);
    if (result != FLASHIF_OK) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_PsdkReturnCode PsdkUpgradePlatformStm32_ReadUpgradeProgramFile(uint32_t offset, uint16_t readDataLen, uint8_t *data,
                                                                 uint16_t *realLen)
{
    memcpy(data, (const void *) (APPLICATION_STORE_ADDRESS + offset), readDataLen);
    *realLen = readDataLen;

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_PsdkReturnCode PsdkUpgradePlatformStm32_CloseUpgradeProgramFile(void)
{
    FLASH_If_Write(PSDK_TEST_UPGRADE_FILE_INFO_STORE_ADDR, (uint8_t *) &s_upgradeFileInfo,
                   sizeof(T_PsdkUpgradeFileInfo));

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_PsdkReturnCode PsdkUpgradePlatformStm32_ReplaceOldProgram(void)
{
    uint32_t result;
    T_PsdkUpgradeFileInfo *upgradeFileInfo = (T_PsdkUpgradeFileInfo *) (PSDK_TEST_UPGRADE_FILE_INFO_STORE_ADDR);

    if (upgradeFileInfo->fileSize > APPLICATION_ADDRESS_END - APPLICATION_ADDRESS + 1)
    {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    result = FLASH_If_Erase(APPLICATION_ADDRESS, APPLICATION_ADDRESS_END);
    if (result != FLASHIF_OK) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    result = FLASH_If_Write(APPLICATION_ADDRESS, (uint8_t *) APPLICATION_STORE_ADDRESS, upgradeFileInfo->fileSize);
    if (result != FLASHIF_OK) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_PsdkReturnCode PsdkUpgradePlatformStm32_SetUpgradeRebootState(const T_PsdkUpgradeEndInfo *upgradeEndInfo)
{
    uint32_t result;
    T_PsdkTestUpgradeRebootParam upgradeRebootParam;

    upgradeRebootParam.upgradeRebootKey = PSDK_TEST_UPGRADE_REBOOT_KEY;
    upgradeRebootParam.upgradeEndInfo = *upgradeEndInfo;

    result = FLASH_If_Erase(APPLICATION_PARAM_STORE_ADDRESS, APPLICATION_PARAM_STORE_ADDRESS_END);
    if (result != FLASHIF_OK) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    result = FLASH_If_Write(APPLICATION_PARAM_STORE_ADDRESS, (uint8_t *) &upgradeRebootParam,
                            sizeof(T_PsdkTestUpgradeRebootParam));
    if (result != FLASHIF_OK) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_PsdkReturnCode PsdkUpgradePlatformStm32_GetUpgradeRebootState(bool *isUpgradeReboot,
                                                                T_PsdkUpgradeEndInfo *upgradeEndInfo)
{
    T_PsdkTestUpgradeRebootParam upgradeRebootParam;

    upgradeRebootParam = *(T_PsdkTestUpgradeRebootParam *) (APPLICATION_PARAM_STORE_ADDRESS);

    if (upgradeRebootParam.upgradeRebootKey == PSDK_TEST_UPGRADE_REBOOT_KEY) {
        *isUpgradeReboot = true;
        *upgradeEndInfo = upgradeRebootParam.upgradeEndInfo;
    } else {
        *isUpgradeReboot = false;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_PsdkReturnCode PsdkUpgradePlatformStm32_CleanUpgradeRebootState(void)
{
    uint32_t result;

    result = FLASH_If_Erase(APPLICATION_PARAM_STORE_ADDRESS, APPLICATION_PARAM_STORE_ADDRESS_END);
    if (result != FLASHIF_OK) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/* Private functions definition-----------------------------------------------*/

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
