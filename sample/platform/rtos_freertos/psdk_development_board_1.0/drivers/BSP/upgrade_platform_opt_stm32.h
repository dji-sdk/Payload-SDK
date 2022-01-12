/**
 ********************************************************************
 * @file    upgrade_platform_opt_stm32.h
 * @version V2.0.0
 * @date    3/25/20
 * @brief   This is the header file for "upgrade_platform_opt_stm32.c", defining the structure and
 * (exported) function prototypes.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef UPGRADE_PLATFORM_OPT_STM32_H
#define UPGRADE_PLATFORM_OPT_STM32_H

/* Includes ------------------------------------------------------------------*/
#include <psdk_typedef.h>
#include <psdk_upgrade.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
T_PsdkReturnCode PsdkUpgradePlatformStm32_RebootSystem(void);

T_PsdkReturnCode PsdkUpgradePlatformStm32_CleanUpgradeProgramFileStoreArea(void);
T_PsdkReturnCode PsdkUpgradePlatformStm32_CreateUpgradeProgramFile(const T_PsdkUpgradeFileInfo *fileInfo);
T_PsdkReturnCode PsdkUpgradePlatformStm32_WriteUpgradeProgramFile(uint32_t offset, const uint8_t *data,
                                                                  uint16_t dataLen);
T_PsdkReturnCode PsdkUpgradePlatformStm32_ReadUpgradeProgramFile(uint32_t offset, uint16_t readDataLen, uint8_t *data,
                                                                 uint16_t *realLen);
T_PsdkReturnCode PsdkUpgradePlatformStm32_CloseUpgradeProgramFile(void);

T_PsdkReturnCode PsdkUpgradePlatformStm32_ReplaceOldProgram(void);

T_PsdkReturnCode PsdkUpgradePlatformStm32_SetUpgradeRebootState(const T_PsdkUpgradeEndInfo *upgradeEndInfo);
T_PsdkReturnCode PsdkUpgradePlatformStm32_GetUpgradeRebootState(bool *isUpgradeReboot,
                                                                T_PsdkUpgradeEndInfo *upgradeEndInfo);
T_PsdkReturnCode PsdkUpgradePlatformStm32_CleanUpgradeRebootState(void);


#ifdef __cplusplus
}
#endif

#endif // UPGRADE_PLATFORM_OPT_STM32_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
