/**
 ********************************************************************
 * @file    psdk_upgrade.h
 * @brief   This is the header file for "psdk_upgrade.c", defining the structure and
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
#ifndef PSDK_UPGRADE_H
#define PSDK_UPGRADE_H

/* Includes ------------------------------------------------------------------*/
#include "legacy_psdk2.x/psdk_typedef.h"
#include <dji_typedef.h>
#include <dji_upgrade.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef E_DjiFirmwareTransferType E_PsdkPayloadFirmwareTransferType;
/*!
 * @brief FTP firmware transfer type.
 * @note This transfer type only support linux platform and use network port. Users need to deploy FTP service on
 * payload. The ftp user info used to transfer upgrade firmware is :
 * username:psdk_payload_ftp
 * password:DJi_#$31
 * You can get guide about FTP service deployment on https://developer.dji.com/payload-sdk/documentation
 */
#define PSDK_PAYLOAD_FIRMWARE_TRANSFER_TYPE_FTP     DJI_FIRMWARE_TRANSFER_TYPE_FTP
/*!
 * @brief DCFTP firmware transfer type.
 * @details DCFTP (DJI Common File Transfer Protocol) is a private protocol used to transfer file on DJI Product.
 * Users can get file data by command callbacks, see :: T_PsdkUpgradeDcftpFileTransferOpt.
 * @note This transfer type is used to support RTOS platform or payload don't have network port. The transfer speed
 * is very slow compare to FTP because it uses low speed transfer channel.
 */
#define PSDK_PAYLOAD_FIRMWARE_TRANSFER_TYPE_DCFTP   DJI_FIRMWARE_TRANSFER_TYPE_DCFTP

typedef E_DjiUpgradeStage E_PsdkUpgradeStage;
#define PSDK_UPGRADE_STAGE_IDLE             DJI_UPGRADE_STAGE_IDLE /*!< Idle stage means not in upgrade mode. */
#define PSDK_UPGRADE_STAGE_ONGOING          DJI_UPGRADE_STAGE_ONGOING /*!< Ongoing stage means payload is upgrading. */
#define PSDK_UPGRADE_STAGE_DEVICE_REBOOT    DJI_UPGRADE_STAGE_DEVICE_REBOOT /*!< Device reboot stage means device is rebooting. */
#define PSDK_UPGRADE_STAGE_END              DJI_UPGRADE_STAGE_END /*!< End Stage means upgrade finish and reporting upgrade result to the terminal APP. */

typedef E_DjiUpgradeEndState E_PsdkUpgradeEndState;
#define PSDK_UPGRADE_END_STATE_SUCCESS          DJI_UPGRADE_END_STATE_SUCCESS /*!< Upgrade success. */
#define PSDK_UPGRADE_END_STATE_UNKNOWN_ERROR    DJI_UPGRADE_END_STATE_UNKNOWN_ERROR /*!< Upgrade failure due to unknown reason. */

/**
 * @brief The firmware version of payload.
 * @note If majorVersion = AA, minorVersion = BB, modifyVersion = CC, debugVersion = DD, The version show in
 * terminal APP is AA.BB.CC.DD
 */
typedef T_DjiFirmwareVersion T_PsdkPayloadFirmwareVersion;

typedef T_DjiUpgradeOngoingInfo T_PsdkUpgradeOngoingInfo;

typedef T_DjiUpgradeRebootInfo T_PsdkUpgradeRebootInfo;

typedef T_DjiUpgradeEndInfo T_PsdkUpgradeEndInfo;

typedef T_DjiUpgradeFtpFileTransferInfo T_PsdkUpgradeFtpFileTransferInfo;

typedef T_DjiUpgradeFileInfo T_PsdkUpgradeFileInfo;

typedef T_DjiUpgradeDcftpFileTransferOpt T_PsdkUpgradeDcftpFileTransferOpt;

typedef T_DjiFirmwareTransferInfo T_PsdkPayloadFirmwareTransferInfo;

typedef T_DjiUpgradeState T_PsdkUpgradeState;

typedef T_DjiUpgradeConfig T_PsdkUpgradeConfig;

typedef T_DjiUpgradeHandler T_PsdkUpgradeHandler;

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Initialise upgrade module, and user should call this function before using upgrade features.
 * @param upgradeConfig: upgrade init configuration.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkUpgrade_Init(const T_PsdkUpgradeConfig *upgradeConfig);

/**
 * @brief Enable local upgrade mode.
 * @details After enable local upgrade mode, DJI Assistant 2 will display payload upgrade interface. Users can choose
 * firmware file to upgrade payload.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkUpgrade_EnableLocalUpgrade(void);

/**
 * @brief Register the handler for upgrade process.
 * @param upgradeHandler: pointer to structure of handler functions for upgrade process.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkUpgrade_RegHandler(const T_PsdkUpgradeHandler *upgradeHandler);

/**
 * @brief Push upgrade state to terminal app.
 * @param upgradeState: upgrade state in upgrade process.
 * @note When start upgrade, you need push upgrade state until finish upgrade. The upgrade state push logic is :
 * 1. StartUpgrade callback called;
 * 2. The upgrade stage change to :: PSDK_UPGRADE_STAGE_ONGOING. Push upgrade state upgradeOngoingInfo when upgradeOngoingInfo changed;
 * 3. After upgrade ongoing, change upgrade stage to PSDK_UPGRADE_STAGE_DEVICE_REBOOT and push upgrade state upgradeRebootInfo;
 * 4. Device reboot;
 * 4. After device reboot, change upgrade stage to PSDK_UPGRADE_STAGE_END and continuous push upgrade state upgradeEndInfo (recommended 1Hz);
 * 5. FinishUpgrade Callback called, stop push upgrade state. The upgrade process finished.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkUpgrade_PushUpgradeState(const T_PsdkUpgradeState *upgradeState);

#ifdef __cplusplus
}
#endif

#endif // PSDK_UPGRADE_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
