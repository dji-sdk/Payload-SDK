/**
 ********************************************************************
 * @file    psdk_upgrade.h
 * @brief   This is the header file for "psdk_upgrade.c", defining the structure and
 * (exported) function prototypes.
 *
 * @copyright (c) 2018 DJI. All rights reserved.
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
#include <psdk_typedef.h>

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
     * username:psdk_payload_ftp
     * password:DJi_#$31
     * You can get guide about FTP service deployment on https://developer.dji.com/payload-sdk/documentation
     */
    PSDK_PAYLOAD_FIRMWARE_TRANSFER_TYPE_FTP = 0,
    /*!
     * @brief DCFTP firmware transfer type.
     * @details DCFTP (DJI Common File Transfer Protocol) is a private protocol used to transfer file on DJI Product.
     * Users can get file data by command callbacks, see :: T_PsdkUpgradeDcftpFileTransferOpt.
     * @note This transfer type is used to support RTOS platform or payload don't have network port. The transfer speed
     * is very slow compare to FTP because it uses low speed transfer channel.
     */
    PSDK_PAYLOAD_FIRMWARE_TRANSFER_TYPE_DCFTP,
} E_PsdkPayloadFirmwareTransferType;

typedef enum {
    PSDK_UPGRADE_STAGE_IDLE = 0, /*!< Idle stage means not in upgrade mode. */
    PSDK_UPGRADE_STAGE_ONGOING = 3, /*!< Ongoing stage means payload is upgrading. */
    PSDK_UPGRADE_STAGE_DEVICE_REBOOT = 6, /*!< Device reboot stage means device is rebooting. */
    PSDK_UPGRADE_STAGE_END = 4, /*!< End Stage means upgrade finish and reporting upgrade result to the terminal APP. */
} E_PsdkUpgradeStage;

typedef enum {
    PSDK_UPGRADE_END_STATE_SUCCESS = 1, /*!< Upgrade success. */
    PSDK_UPGRADE_END_STATE_UNKNOWN_ERROR = 2, /*!< Upgrade failure due to unknown reason. */
} E_PsdkUpgradeEndState;

/**
 * @brief The firmware version of payload.
 * @note If majorVersion = AA, minorVersion = BB, modifyVersion = CC, debugVersion = DD, The version show in
 * terminal APP is AA.BB.CC.DD
 */
typedef struct {
    uint8_t majorVersion; /*!< The major version of firmware, the range is 0 ~ 99. */
    uint8_t minorVersion; /*!< The minor version of firmware, the range is 0 ~ 99. */
    uint8_t modifyVersion; /*!< The modify version of firmware, the range is 0 ~ 99. */
    uint8_t debugVersion; /*!< The debug version of firmware, the range is 0 ~ 99. */
} T_PsdkPayloadFirmwareVersion;

typedef struct {
    uint8_t upgradeProgress; /*!< The upgrade progress, the range is 0 ~ 100. */
} T_PsdkUpgradeOngoingInfo;

typedef struct {
    uint8_t rebootTimeout; /*!< The timeout value of rebooting device. unit: s */
} T_PsdkUpgradeRebootInfo;

typedef struct {
    E_PsdkUpgradeEndState upgradeEndState; /*!< The upgrade end state */
} T_PsdkUpgradeEndInfo;

typedef struct {
    uint32_t port; /*!< FTP port used to transfer file, default is 21 */
} T_PsdkUpgradeFtpFileTransferInfo;

typedef struct {
    uint32_t fileSize; /*! The size of file. */
    char fileName[PSDK_FILE_NAME_SIZE_MAX]; /*! The name of file. */
} T_PsdkUpgradeFileInfo;

typedef struct {
    /**
      * @brief Prototype of callback function used to start file transfer.
      * @param fileInfo: the file info about the file to be transferred.
      * @return Execution result.
      */
    T_PsdkReturnCode (*start)(const T_PsdkUpgradeFileInfo *fileInfo);
    /**
      * @brief Prototype of callback function used to transfer file data.
      * @details After start transfer, this callback function will be called several times to transfer file data sequentially.
      * @param data: pointer to memory space used to store file data.
      * @param dataLen: the data length of data.
      * @return Execution result.
      */
    T_PsdkReturnCode (*transfer)(const uint8_t *data, uint16_t dataLen);
    /**
      * @brief Prototype of callback function used to finish file transfer.
      * @param md5: the md5 value of file, used to check the correctness of the file transfer .
      * @return Execution result.
      */
    T_PsdkReturnCode (*finish)(const uint8_t md5[PSDK_MD5_BUFFER_LEN]);
} T_PsdkUpgradeDcftpFileTransferOpt;

typedef struct {
    /*! The firmware transfer type for upgrade. */
    E_PsdkPayloadFirmwareTransferType transferType;

    /*! If transferType is PSDK_PAYLOAD_FIRMWARE_TRANSFER_TYPE_FTP, need support ftpTransferInfo. */
    T_PsdkUpgradeFtpFileTransferInfo ftpTransferInfo;
    /*! If transferType is PSDK_PAYLOAD_FIRMWARE_TRANSFER_TYPE_DCFTP, need support dcftpFileTransferOpt. */
    T_PsdkUpgradeDcftpFileTransferOpt dcftpFileTransferOpt;
} T_PsdkPayloadFirmwareTransferInfo;

typedef struct {
    /*! The upgrade stage in upgrade process. */
    E_PsdkUpgradeStage upgradeStage;

    union {
        /*! If upgradeStage is PSDK_UPGRADE_STAGE_ONGOING, need support upgradeOngoingInfo. */
        T_PsdkUpgradeOngoingInfo upgradeOngoingInfo;
        /*! If upgradeStage is PSDK_UPGRADE_STAGE_DEVICE_REBOOT, need support upgradeRebootInfo. */
        T_PsdkUpgradeRebootInfo upgradeRebootInfo;
        /*! If upgradeStage is PSDK_UPGRADE_STAGE_END, need support upgradeEndInfo. */
        T_PsdkUpgradeEndInfo upgradeEndInfo;
    };
} T_PsdkUpgradeState;

typedef struct {
    T_PsdkPayloadFirmwareVersion currentFirmwareVersion; /*!< The current firmware version of payload. */
    T_PsdkPayloadFirmwareTransferInfo firmwareTransferInfo; /*!< The firmware transfer info of payload. */
} T_PsdkUpgradeConfig;

typedef struct {
    /**
      * @brief Prototype of callback function used to enter upgrade mode.
      * @param waitTime: the wait time for enter upgrade mode. The terminal APP will wait these time before do other
      * upgrade actions. You can use this time to prepare for firmware upgrade in other task, such as clean firmware
      * store area. unit: s
      * @return Execution result.
      */
    T_PsdkReturnCode (*EnterUpgradeMode)(uint16_t *waitTime);
    /**
      * @brief Prototype of callback function used to check transferred firmware.
      * @details You can verify signature and decrypt firmware in this callback function.
      * @return Execution result.
      */
    T_PsdkReturnCode (*CheckFirmware)(void);
    /**
      * @brief Prototype of callback function used to start firmware upgrade.
      * @note After start upgrade, the upgrade stage need change to ::PSDK_UPGRADE_STAGE_ONGOING
      * @return Execution result.
      */
    T_PsdkReturnCode (*StartUpgrade)(void);
    /**
      * @brief Prototype of callback function used to finish firmware upgrade.
      * @note After call finish upgrade, the upgrade stage need change from ::PSDK_UPGRADE_STAGE_END to ::PSDK_UPGRADE_STAGE_IDLE
      * @return Execution result.
      */
    T_PsdkReturnCode (*FinishUpgrade)(void);
} T_PsdkUpgradeHandler;

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
