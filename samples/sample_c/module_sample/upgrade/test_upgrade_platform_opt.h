/**
 ********************************************************************
 * @file    test_upgrade_platform_opt.h
 * @brief   This is the header file for "test_upgrade_platform_opt.c", defining the structure and
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
#ifndef TEST_UPGRADE_PLATFORM_OPT_H
#define TEST_UPGRADE_PLATFORM_OPT_H

#include <dji_typedef.h>
#include <dji_upgrade.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/


/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef struct {
    T_DjiReturnCode (*rebootSystem)(void);

    T_DjiReturnCode (*cleanUpgradeProgramFileStoreArea)(void);
    T_DjiReturnCode (*createUpgradeProgramFile)(const T_DjiUpgradeFileInfo *fileInfo);
    T_DjiReturnCode (*writeUpgradeProgramFile)(uint32_t offset, const uint8_t *data, uint16_t dataLen);
    T_DjiReturnCode (*readUpgradeProgramFile)(uint32_t offset, uint16_t readDataLen, uint8_t *data,
                                               uint16_t *realLen);
    T_DjiReturnCode (*closeUpgradeProgramFile)(void);

    T_DjiReturnCode (*replaceOldProgram)(void);

    T_DjiReturnCode (*setUpgradeRebootState)(const T_DjiUpgradeEndInfo *upgradeEndInfo);
    T_DjiReturnCode (*getUpgradeRebootState)(bool *isUpgradeReboot, T_DjiUpgradeEndInfo *upgradeEndInfo);
    T_DjiReturnCode (*cleanUpgradeRebootState)(void);
} T_DjiTestUpgradePlatformOpt;

/* Exported functions --------------------------------------------------------*/
T_DjiReturnCode DjiTest_RegUpgradePlatformOpt(const T_DjiTestUpgradePlatformOpt *upgradePlatformOpt);

T_DjiReturnCode DjiTest_RebootSystem(void);

T_DjiReturnCode DjiTest_CleanUpgradeProgramFileStoreArea(void);
T_DjiReturnCode DjiTest_CreateUpgradeProgramFile(const T_DjiUpgradeFileInfo *fileInfo);
T_DjiReturnCode DjiTest_WriteUpgradeProgramFile(uint32_t offset, const uint8_t *data, uint16_t dataLen);
T_DjiReturnCode DjiTest_ReadUpgradeProgramFile(uint32_t offset, uint16_t readDataLen, uint8_t *data,
                                                 uint16_t *realLen);
T_DjiReturnCode DjiTest_CloseUpgradeProgramFile(void);

T_DjiReturnCode DjiTest_ReplaceOldProgram(void);

T_DjiReturnCode DjiTest_SetUpgradeRebootState(const T_DjiUpgradeEndInfo *upgradeEndInfo);
T_DjiReturnCode DjiTest_GetUpgradeRebootState(bool *isUpgradeReboot, T_DjiUpgradeEndInfo *upgradeEndInfo);
T_DjiReturnCode DjiTest_CleanUpgradeRebootState(void);

#ifdef __cplusplus
}
#endif

#endif // TEST_UPGRADE_PLATFORM_OPT_H

/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
