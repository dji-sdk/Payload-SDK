/**
 ********************************************************************
 * @file    test_upgrade_common_file_transfer.c
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
#include "test_upgrade_common_file_transfer.h"
#include "dji_logger.h"
#include <utils/util_md5.h>
#include "test_upgrade_platform_opt.h"

/* Private constants ---------------------------------------------------------*/
#define DJI_TEST_FILE_MD5_BUFFER_SIZE              256

/* Private types -------------------------------------------------------------*/

/* Private values -------------------------------------------------------------*/
static T_DjiUpgradeFileInfo s_upgradeFileInfo = {0};
static uint32_t s_alreadyTransferFileSize = 0;

/* Private functions declaration ---------------------------------------------*/
static T_DjiReturnCode DjiTestFile_GetUpgradeFileMd5(uint8_t md5[DJI_MD5_BUFFER_LEN]);

/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode DjiTestCommonFileTransfer_Start(const T_DjiUpgradeFileInfo *fileInfo)
{
    T_DjiReturnCode returnCode;

    s_upgradeFileInfo.fileSize = 0;
    memset(s_upgradeFileInfo.fileName, 0, sizeof(s_upgradeFileInfo.fileName));
    s_alreadyTransferFileSize = 0;

    returnCode = DjiTest_CreateUpgradeProgramFile(fileInfo);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Create upgrade program file error");
        return returnCode;
    }

    s_upgradeFileInfo = *fileInfo;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiTestCommonFileTransfer_Transfer(const uint8_t *data, uint16_t dataLen)
{
    T_DjiReturnCode returnCode;

    if (s_alreadyTransferFileSize >= s_upgradeFileInfo.fileSize) {
        USER_LOG_ERROR("Already transfer file size is more than file real size");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    returnCode = DjiTest_WriteUpgradeProgramFile(s_alreadyTransferFileSize, data, dataLen);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Write upgrade program file error, return code = 0x%08llX", returnCode);
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    s_alreadyTransferFileSize += dataLen;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiTestCommonFileTransfer_Finish(const uint8_t md5[DJI_MD5_BUFFER_LEN])
{
    uint8_t localFileMd5[DJI_MD5_BUFFER_LEN] = {0};
    T_DjiReturnCode returnCode;

    if (s_alreadyTransferFileSize != s_upgradeFileInfo.fileSize) {
        USER_LOG_ERROR("Transfer finish error, transfer file size is not equal");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    returnCode = DjiTestFile_GetUpgradeFileMd5(localFileMd5);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Get file md5 error, return code = 0x%08llX", returnCode);
        goto out;
    }

    if (memcmp(md5, localFileMd5, DJI_MD5_BUFFER_LEN) == 0) {
        returnCode = DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    } else {
        returnCode = DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

out:
    DjiTest_CloseUpgradeProgramFile();
    s_upgradeFileInfo.fileSize = 0;
    memset(s_upgradeFileInfo.fileName, 0, sizeof(s_upgradeFileInfo.fileName));
    s_alreadyTransferFileSize = 0;
    return returnCode;
}

/* Private functions definition-----------------------------------------------*/
static T_DjiReturnCode DjiTestFile_GetUpgradeFileMd5(uint8_t md5[DJI_MD5_BUFFER_LEN])
{
    uint8_t fileBuffer[DJI_TEST_FILE_MD5_BUFFER_SIZE] = {0};
    T_DjiReturnCode returnCode;
    uint32_t offset;
    MD5_CTX fileMd5Ctx;
    uint16_t realLen = 0;

    offset = 0;
    UtilMd5_Init(&fileMd5Ctx);
    while (s_upgradeFileInfo.fileSize - offset > DJI_TEST_FILE_MD5_BUFFER_SIZE) {
        returnCode = DjiTest_ReadUpgradeProgramFile(offset, DJI_TEST_FILE_MD5_BUFFER_SIZE,
                                                     fileBuffer, &realLen);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS || realLen != DJI_TEST_FILE_MD5_BUFFER_SIZE) {
            USER_LOG_ERROR("Get file data error, return code = 0x%08llX", returnCode);
            return returnCode;
        }

        UtilMd5_Update(&fileMd5Ctx, fileBuffer, DJI_TEST_FILE_MD5_BUFFER_SIZE);

        offset += DJI_TEST_FILE_MD5_BUFFER_SIZE;
    }

    returnCode = DjiTest_ReadUpgradeProgramFile(offset, s_upgradeFileInfo.fileSize - offset, fileBuffer, &realLen);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS || realLen != s_upgradeFileInfo.fileSize - offset) {
        USER_LOG_ERROR("Get file data error, return code = 0x%08llX", returnCode);
        return returnCode;
    }

    UtilMd5_Update(&fileMd5Ctx, fileBuffer, s_upgradeFileInfo.fileSize - offset);
    UtilMd5_Final(&fileMd5Ctx, md5);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
