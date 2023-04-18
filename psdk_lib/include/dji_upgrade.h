/**
 ********************************************************************
 * @file    dji_upgrade.h
 * @brief   This is the header file for "dji_upgrade.c", defining the structure and
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
#ifndef DJI_UPGRADE_H
#define DJI_UPGRADE_H

/* Includes ------------------------------------------------------------------*/
#include <dji_typedef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef enum {
    /*!
     * @brief FTP firmware transfer type.
     * @note This transfer type only support linux platform and use network port. Users need to deploy FTP service on
     * payload. The ftp user info used to transfer upgrade firmware is :
     * username:dji_payload_ftp
     * password:DJi_#$31
     * You can get guide about FTP service deployment on https://developer.dji.com/payload-sdk/documentation
     */
    DJI_FIRMWARE_TRANSFER_TYPE_FTP = 0,
    /*!
     * @brief DCFTP firmware transfer type.
     * @details DCFTP (DJI Common File Transfer Protocol) is a private protocol used to transfer file on DJI Product.
     * Users can get file data by command callbacks, see :: T_DjiUpgradeDcftpFileTransferOpt.
     * @note This transfer type is used to support RTOS platform or payload don't have network port. The transfer speed
     * is very slow compare to FTP because it uses low speed transfer channel.
     */
    DJI_FIRMWARE_TRANSFER_TYPE_DCFTP,
} E_DjiFirmwareTransferType;

typedef enum {
    DJI_UPGRADE_STAGE_IDLE = 0, /*!< Idle stage means not in upgrade mode. */
    DJI_UPGRADE_STAGE_ONGOING = 3, /*!< Ongoing stage means payload is upgrading. */
    DJI_UPGRADE_STAGE_DEVICE_REBOOT = 6, /*!< Device reboot stage means device is rebooting. */
    DJI_UPGRADE_STAGE_END = 4, /*!< End Stage means upgrade finish and reporting upgrade result to the terminal APP. */
} E_DjiUpgradeStage;

typedef enum {
    DJI_UPGRADE_END_STATE_SUCCESS = 1, /*!< Upgrade success. */
    DJI_UPGRADE_END_STATE_UNKNOWN_ERROR = 2, /*!< Upgrade failure due to unknown reason. */
} E_DjiUpgradeEndState;

typedef struct {
    uint8_t upgradeProgress; /*!< The upgrade progress, the range is 0 ~ 100. */
} T_DjiUpgradeOngoingInfo;

typedef struct {
    uint8_t rebootTimeout; /*!< The timeout value of rebooting device. unit: s */
} T_DjiUpgradeRebootInfo;

typedef struct {
    E_DjiUpgradeEndState upgradeEndState; /*!< The upgrade end state */
} T_DjiUpgradeEndInfo;

typedef struct {
    uint32_t port; /*!< FTP port used to transfer file, default is 21 */
} T_DjiUpgradeFtpFileTransferInfo;

typedef struct {
    uint32_t fileSize; /*! The size of file. */
    char fileName[DJI_FILE_NAME_SIZE_MAX]; /*! The name of file. */
} T_DjiUpgradeFileInfo;

typedef struct {
    /**
      * @brief Prototype of callback function used to start file transfer.
      * @param fileInfo: the file info about the file to be transferred.
      * @return Execution result.
      */
    T_DjiReturnCode (*start)(const T_DjiUpgradeFileInfo *fileInfo);
    /**
      * @brief Prototype of callback function used to transfer file data.
      * @details After start transfer, this callback function will be called several times to transfer file data sequentially.
      * @param data: pointer to memory space used to store file data.
      * @param dataLen: the data length of data.
      * @return Execution result.
      */
    T_DjiReturnCode (*transfer)(const uint8_t *data, uint16_t dataLen);
    /**
      * @brief Prototype of callback function used to finish file transfer.
      * @param md5: the md5 value of file, used to check the correctness of the file transfer .
      * @return Execution result.
      */
    T_DjiReturnCode (*finish)(const uint8_t md5[DJI_MD5_BUFFER_LEN]);
} T_DjiUpgradeDcftpFileTransferOpt;

typedef struct {
    /*! The firmware transfer type for upgrade. */
    E_DjiFirmwareTransferType transferType;

    /*! If transferType is DJI_FIRMWARE_TRANSFER_TYPE_FTP, need support ftpTransferInfo. */
    T_DjiUpgradeFtpFileTransferInfo ftpTransferInfo;
    /*! If transferType is DJI_FIRMWARE_TRANSFER_TYPE_DCFTP, need support dcftpFileTransferOpt. */
    T_DjiUpgradeDcftpFileTransferOpt dcftpFileTransferOpt;
} T_DjiFirmwareTransferInfo;

typedef struct {
    /*! The upgrade stage in upgrade process. */
    E_DjiUpgradeStage upgradeStage;

    union {
        /*! If upgradeStage is DJI_UPGRADE_STAGE_ONGOING, need support upgradeOngoingInfo. */
        T_DjiUpgradeOngoingInfo upgradeOngoingInfo;
        /*! If upgradeStage is DJI_UPGRADE_STAGE_DEVICE_REBOOT, need support upgradeRebootInfo. */
        T_DjiUpgradeRebootInfo upgradeRebootInfo;
        /*! If upgradeStage is DJI_UPGRADE_STAGE_END, need support upgradeEndInfo. */
        T_DjiUpgradeEndInfo upgradeEndInfo;
    };
} T_DjiUpgradeState;

typedef struct {
    T_DjiFirmwareVersion currentFirmwareVersion; /*!< The current firmware version of payload. */
    T_DjiFirmwareTransferInfo firmwareTransferInfo; /*!< The firmware transfer info of payload. */
} T_DjiUpgradeConfig;

typedef struct {
    /**
      * @brief Prototype of callback function used to enter upgrade mode.
      * @param waitTime: the wait time for enter upgrade mode. The terminal APP will wait these time before do other
      * upgrade actions. You can use this time to prepare for firmware upgrade in other task, such as clean firmware
      * store area. unit: s
      * @return Execution result.
      */
    T_DjiReturnCode (*EnterUpgradeMode)(uint16_t *waitTime);
    /**
      * @brief Prototype of callback function used to check transferred firmware.
      * @details You can verify signature and decrypt firmware in this callback function.
      * @return Execution result.
      */
    T_DjiReturnCode (*CheckFirmware)(void);
    /**
      * @brief Prototype of callback function used to start firmware upgrade.
      * @note After start upgrade, the upgrade stage need change to ::DJI_UPGRADE_STAGE_ONGOING
      * @return Execution result.
      */
    T_DjiReturnCode (*StartUpgrade)(void);
    /**
      * @brief Prototype of callback function used to finish firmware upgrade.
      * @note After call finish upgrade, the upgrade stage need change from ::DJI_UPGRADE_STAGE_END to ::DJI_UPGRADE_STAGE_IDLE
      * @return Execution result.
      */
    T_DjiReturnCode (*FinishUpgrade)(void);
} T_DjiUpgradeHandler;

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Initialise upgrade module, and user should call this function before using upgrade features.
 * @param upgradeConfig: upgrade init configuration.
 * @return Execution result.
 */
T_DjiReturnCode DjiUpgrade_Init(const T_DjiUpgradeConfig *upgradeConfig);

/**
 * @brief Enable local upgrade mode.
 * @details After enable local upgrade mode, DJI Assistant 2 will display payload upgrade interface. Users can choose
 * firmware file to upgrade payload.
 * @return Execution result.
 */
T_DjiReturnCode DjiUpgrade_EnableLocalUpgrade(void);

/**
 * @brief Register the handler for upgrade process.
 * @param upgradeHandler: pointer to structure of handler functions for upgrade process.
 * @return Execution result.
 */
T_DjiReturnCode DjiUpgrade_RegHandler(const T_DjiUpgradeHandler *upgradeHandler);

/**
 * @brief Push upgrade state to terminal app.
 * @param upgradeState: upgrade state in upgrade process.
 * @note When start upgrade, you need push upgrade state until finish upgrade. The upgrade state push logic is :
 * 1. StartUpgrade callback called;
 * 2. The upgrade stage change to :: DJI_UPGRADE_STAGE_ONGOING. Push upgrade state upgradeOngoingInfo when upgradeOngoingInfo changed;
 * 3. After upgrade ongoing, change upgrade stage to DJI_UPGRADE_STAGE_DEVICE_REBOOT and push upgrade state upgradeRebootInfo;
 * 4. Device reboot;
 * 4. After device reboot, change upgrade stage to DJI_UPGRADE_STAGE_END and continuous push upgrade state upgradeEndInfo (recommended 1Hz);
 * 5. FinishUpgrade Callback called, stop push upgrade state. The upgrade process finished.
 * @return Execution result.
 */
T_DjiReturnCode DjiUpgrade_PushUpgradeState(const T_DjiUpgradeState *upgradeState);

#ifdef __cplusplus
}
#endif

#endif // DJI_UPGRADE_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
