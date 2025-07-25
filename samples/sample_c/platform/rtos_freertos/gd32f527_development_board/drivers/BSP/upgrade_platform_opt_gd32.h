/**
 ********************************************************************
 * @file    upgrade_platform_opt_gd32.h
 * @version V2.0.0
 * @date    3/25/20
 * @brief   This is the header file for "upgrade_platform_opt_gd32.c", defining the structure and
 * (exported) function prototypes.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef UPGRADE_PLATFORM_OPT_Gd32_H
#define UPGRADE_PLATFORM_OPT_Gd32_H

/* Includes ------------------------------------------------------------------*/
#include "dji_typedef.h"
#include "dji_upgrade.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
T_DjiReturnCode DjiUpgradePlatformGd32_RebootSystem(void);

T_DjiReturnCode DjiUpgradePlatformGd32_CleanUpgradeProgramFileStoreArea(void);
T_DjiReturnCode DjiUpgradePlatformGd32_CreateUpgradeProgramFile(const T_DjiUpgradeFileInfo *fileInfo);
T_DjiReturnCode DjiUpgradePlatformGd32_WriteUpgradeProgramFile(uint32_t offset, const uint8_t *data,
                                                                uint16_t dataLen);
T_DjiReturnCode DjiUpgradePlatformGd32_ReadUpgradeProgramFile(uint32_t offset, uint16_t readDataLen, uint8_t *data,
                                                               uint16_t *realLen);
T_DjiReturnCode DjiUpgradePlatformGd32_CloseUpgradeProgramFile(void);

T_DjiReturnCode DjiUpgradePlatformGd32_ReplaceOldProgram(void);

T_DjiReturnCode DjiUpgradePlatformGd32_SetUpgradeRebootState(const T_DjiUpgradeEndInfo *upgradeEndInfo);
T_DjiReturnCode DjiUpgradePlatformGd32_GetUpgradeRebootState(bool *isUpgradeReboot,
                                                              T_DjiUpgradeEndInfo *upgradeEndInfo);
T_DjiReturnCode DjiUpgradePlatformGd32_CleanUpgradeRebootState(void);


#ifdef __cplusplus
}
#endif

#endif // UPGRADE_PLATFORM_OPT_Gd32_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
