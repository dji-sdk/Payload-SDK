/**
 ********************************************************************
 * @file    test_upgrade.h
 * @brief   This is the header file for "test_upgrade.c", defining the structure and
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
#ifndef TEST_UPGRADE_H
#define TEST_UPGRADE_H

/* Includes ------------------------------------------------------------------*/
#include <dji_upgrade.h>
#include "test_upgrade_platform_opt.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef struct {
    T_DjiFirmwareVersion firmwareVersion;
    E_DjiFirmwareTransferType transferType;
    //For linux: need replace program before reboot system
    //For mcu: don't need replace program before reboot system, replace program in loader
    bool needReplaceProgramBeforeReboot;
} T_DjiTestUpgradeConfig;

/* Exported functions --------------------------------------------------------*/
T_DjiReturnCode DjiTest_UpgradeStartService(const T_DjiTestUpgradePlatformOpt *upgradePlatformOpt,
                                            T_DjiTestUpgradeConfig testUpgradeConfig);

#ifdef __cplusplus
}
#endif

#endif
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
