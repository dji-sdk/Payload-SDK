/**
 ********************************************************************
 * @file    upgrade_platform_opt_gd32.c
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
#include "upgrade_platform_opt_gd32.h"
#include <flash_if.h>
#include "dji_logger.h"
#include "gd32f5xx.h"

/* Private constants ---------------------------------------------------------*/
#define DJI_TEST_UPGRADE_FILE_INFO_STORE_ADDR      (APPLICATION_STORE_ADDRESS_END - 1023)
#define DJI_TEST_UPGRADE_REBOOT_KEY                0x11223344

/* Private types -------------------------------------------------------------*/
typedef struct {
    uint32_t upgradeRebootKey;
    T_DjiUpgradeEndInfo upgradeEndInfo;
} T_DjiTestUpgradeRebootParam;

/* Private values -------------------------------------------------------------*/
static T_DjiUpgradeFileInfo s_upgradeFileInfo = {0};

/* Private functions declaration ---------------------------------------------*/

/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode DjiUpgradePlatformGd32_RebootSystem(void)
{
   __disable_irq();
#ifdef __CC_ARM
   __disable_fiq();
#endif
    NVIC_SystemReset();

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiUpgradePlatformGd32_CleanUpgradeProgramFileStoreArea(void)
{
    uint32_t result;

    result = FLASH_If_Erase(APPLICATION_STORE_ADDRESS, APPLICATION_STORE_ADDRESS_END);
    if (result != FLASHIF_OK) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }
    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiUpgradePlatformGd32_CreateUpgradeProgramFile(const T_DjiUpgradeFileInfo *fileInfo)
{
    s_upgradeFileInfo = *fileInfo;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiUpgradePlatformGd32_WriteUpgradeProgramFile(uint32_t offset, const uint8_t *data,
                                                                uint16_t dataLen)
{
    uint32_t result;

    result = FLASH_If_Write(APPLICATION_STORE_ADDRESS + offset, (uint8_t *) data, dataLen);
    if (result != FLASHIF_OK) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiUpgradePlatformGd32_ReadUpgradeProgramFile(uint32_t offset, uint16_t readDataLen, uint8_t *data,
                                                               uint16_t *realLen)
{
    memcpy(data, (const void *) (APPLICATION_STORE_ADDRESS + offset), readDataLen);
    *realLen = readDataLen;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiUpgradePlatformGd32_CloseUpgradeProgramFile(void)
{
    FLASH_If_Write(DJI_TEST_UPGRADE_FILE_INFO_STORE_ADDR, (uint8_t *) &s_upgradeFileInfo,
                   sizeof(T_DjiUpgradeFileInfo));

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiUpgradePlatformGd32_ReplaceOldProgram(void)
{
    uint32_t result;
    T_DjiUpgradeFileInfo *upgradeFileInfo = (T_DjiUpgradeFileInfo *) (DJI_TEST_UPGRADE_FILE_INFO_STORE_ADDR);

    if (upgradeFileInfo->fileSize > APPLICATION_ADDRESS_END - APPLICATION_ADDRESS + 1) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    result = FLASH_If_Erase(APPLICATION_ADDRESS, APPLICATION_ADDRESS_END);
    if (result != FLASHIF_OK) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    result = FLASH_If_Write(APPLICATION_ADDRESS, (uint8_t *) APPLICATION_STORE_ADDRESS, upgradeFileInfo->fileSize);
    if (result != FLASHIF_OK) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiUpgradePlatformGd32_SetUpgradeRebootState(const T_DjiUpgradeEndInfo *upgradeEndInfo)
{
    uint32_t result;
    T_DjiTestUpgradeRebootParam upgradeRebootParam = {0};

    upgradeRebootParam.upgradeRebootKey = DJI_TEST_UPGRADE_REBOOT_KEY;
    upgradeRebootParam.upgradeEndInfo = *upgradeEndInfo;

    result = FLASH_If_Erase(APPLICATION_PARAM_STORE_ADDRESS, APPLICATION_PARAM_STORE_ADDRESS_END);
    if (result != FLASHIF_OK) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    result = FLASH_If_Write(APPLICATION_PARAM_STORE_ADDRESS, (uint8_t *) &upgradeRebootParam,
                            sizeof(T_DjiTestUpgradeRebootParam));
    if (result != FLASHIF_OK) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiUpgradePlatformGd32_GetUpgradeRebootState(bool *isUpgradeReboot,
                                                              T_DjiUpgradeEndInfo *upgradeEndInfo)
{
    T_DjiTestUpgradeRebootParam upgradeRebootParam;

    upgradeRebootParam = *(T_DjiTestUpgradeRebootParam *) (APPLICATION_PARAM_STORE_ADDRESS);

    if (upgradeRebootParam.upgradeRebootKey == DJI_TEST_UPGRADE_REBOOT_KEY) {
        *isUpgradeReboot = true;
        *upgradeEndInfo = upgradeRebootParam.upgradeEndInfo;
    } else {
        *isUpgradeReboot = false;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiUpgradePlatformGd32_CleanUpgradeRebootState(void)
{
    uint32_t result;

    result = FLASH_If_Erase(APPLICATION_PARAM_STORE_ADDRESS, APPLICATION_PARAM_STORE_ADDRESS_END);
    if (result != FLASHIF_OK) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/* Private functions definition-----------------------------------------------*/

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
