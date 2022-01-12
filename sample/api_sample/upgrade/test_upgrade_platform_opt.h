/**
 ********************************************************************
 * @file    test_upgrade_platform_opt.h
 * @version V0.0.0
 * @date    2019/01/01
 * @brief   This is the header file for "test_upgrade_platform_opt.c", defining the structure and
 * (exported) function prototypes.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef TEST_UPGRADE_PLATFORM_OPT_H
#define TEST_UPGRADE_PLATFORM_OPT_H

#include <psdk_typedef.h>
#include <psdk_upgrade.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/


/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef struct {
    T_PsdkReturnCode (*rebootSystem)(void);

    T_PsdkReturnCode (*cleanUpgradeProgramFileStoreArea)(void);
    T_PsdkReturnCode (*createUpgradeProgramFile)(const T_PsdkUpgradeFileInfo *fileInfo);
    T_PsdkReturnCode (*writeUpgradeProgramFile)(uint32_t offset, const uint8_t *data, uint16_t dataLen);
    T_PsdkReturnCode (*readUpgradeProgramFile)(uint32_t offset, uint16_t readDataLen, uint8_t *data,
                                               uint16_t *realLen);
    T_PsdkReturnCode (*closeUpgradeProgramFile)(void);

    T_PsdkReturnCode (*replaceOldProgram)(void);

    T_PsdkReturnCode (*setUpgradeRebootState)(const T_PsdkUpgradeEndInfo *upgradeEndInfo);
    T_PsdkReturnCode (*getUpgradeRebootState)(bool *isUpgradeReboot, T_PsdkUpgradeEndInfo *upgradeEndInfo);
    T_PsdkReturnCode (*cleanUpgradeRebootState)(void);
} T_PsdkTestUpgradePlatformOpt;

/* Exported functions --------------------------------------------------------*/
T_PsdkReturnCode PsdkTest_RegUpgradePlatformOpt(const T_PsdkTestUpgradePlatformOpt *upgradePlatformOpt);

T_PsdkReturnCode PsdkTest_RebootSystem(void);

T_PsdkReturnCode PsdkTest_CleanUpgradeProgramFileStoreArea(void);
T_PsdkReturnCode PsdkTest_CreateUpgradeProgramFile(const T_PsdkUpgradeFileInfo *fileInfo);
T_PsdkReturnCode PsdkTest_WriteUpgradeProgramFile(uint32_t offset, const uint8_t *data, uint16_t dataLen);
T_PsdkReturnCode PsdkTest_ReadUpgradeProgramFile(uint32_t offset, uint16_t readDataLen, uint8_t *data,
                                                 uint16_t *realLen);
T_PsdkReturnCode PsdkTest_CloseUpgradeProgramFile(void);

T_PsdkReturnCode PsdkTest_ReplaceOldProgram(void);

T_PsdkReturnCode PsdkTest_SetUpgradeRebootState(const T_PsdkUpgradeEndInfo *upgradeEndInfo);
T_PsdkReturnCode PsdkTest_GetUpgradeRebootState(bool *isUpgradeReboot, T_PsdkUpgradeEndInfo *upgradeEndInfo);
T_PsdkReturnCode PsdkTest_CleanUpgradeRebootState(void);

#ifdef __cplusplus
}
#endif

#endif // TEST_UPGRADE_PLATFORM_OPT_H

/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
