/**
 ********************************************************************
 * @file    test_upgrade_common_file_transfer.c
 * @version V2.0.0
 * @date    3/12/20
 * @brief
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

/* Includes ------------------------------------------------------------------*/
#include "test_upgrade_common_file_transfer.h"
#include "psdk_logger.h"
#include <utils/util_md5.h>
#include "test_upgrade_platform_opt.h"

/* Private constants ---------------------------------------------------------*/
#define PSDK_TEST_FILE_MD5_BUFFER_SIZE              256

/* Private types -------------------------------------------------------------*/

/* Private values -------------------------------------------------------------*/
static T_PsdkUpgradeFileInfo s_upgradeFileInfo = {0};
static uint32_t s_alreadyTransferFileSize = 0;

/* Private functions declaration ---------------------------------------------*/
static T_PsdkReturnCode PsdkTestFile_GetUpgradeFileMd5(uint8_t md5[PSDK_MD5_BUFFER_LEN]);

/* Exported functions definition ---------------------------------------------*/
T_PsdkReturnCode PsdkTestCommonFileTransfer_Start(const T_PsdkUpgradeFileInfo *fileInfo)
{
    T_PsdkReturnCode returnCode;

    s_upgradeFileInfo.fileSize = 0;
    memset(s_upgradeFileInfo.fileName, 0, sizeof(s_upgradeFileInfo.fileName));
    s_alreadyTransferFileSize = 0;

    returnCode = PsdkTest_CreateUpgradeProgramFile(fileInfo);
    if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Create upgrade program file error");
        return returnCode;
    }

    s_upgradeFileInfo = *fileInfo;

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_PsdkReturnCode PsdkTestCommonFileTransfer_Transfer(const uint8_t *data, uint16_t dataLen)
{
    T_PsdkReturnCode returnCode;

    if (s_alreadyTransferFileSize >= s_upgradeFileInfo.fileSize) {
        PsdkLogger_UserLogError("Already transfer file size is more than file real size");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    returnCode = PsdkTest_WriteUpgradeProgramFile(s_alreadyTransferFileSize, data, dataLen);
    if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Write upgrade program file error, return code = 0x%08llX", returnCode);
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    s_alreadyTransferFileSize += dataLen;

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_PsdkReturnCode PsdkTestCommonFileTransfer_Finish(const uint8_t md5[PSDK_MD5_BUFFER_LEN])
{
    uint8_t localFileMd5[PSDK_MD5_BUFFER_LEN] = {0};
    T_PsdkReturnCode returnCode;

    if (s_alreadyTransferFileSize != s_upgradeFileInfo.fileSize) {
        PsdkLogger_UserLogError("Transfer finish error, transfer file size is not equal");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    returnCode = PsdkTestFile_GetUpgradeFileMd5(localFileMd5);
    if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Get file md5 error, return code = 0x%08llX", returnCode);
        goto out;
    }

    if (memcmp(md5, localFileMd5, PSDK_MD5_BUFFER_LEN) == 0) {
        returnCode = PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    } else {
        returnCode = PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

out:
    PsdkTest_CloseUpgradeProgramFile();
    s_upgradeFileInfo.fileSize = 0;
    memset(s_upgradeFileInfo.fileName, 0, sizeof(s_upgradeFileInfo.fileName));
    s_alreadyTransferFileSize = 0;
    return returnCode;
}

/* Private functions definition-----------------------------------------------*/
static T_PsdkReturnCode PsdkTestFile_GetUpgradeFileMd5(uint8_t md5[PSDK_MD5_BUFFER_LEN])
{
    uint8_t fileBuffer[PSDK_TEST_FILE_MD5_BUFFER_SIZE] = {0};
    T_PsdkReturnCode returnCode;
    uint32_t offset;
    MD5_CTX fileMd5Ctx;
    uint16_t realLen = 0;

    offset = 0;
    UtilMd5_Init(&fileMd5Ctx);
    while (s_upgradeFileInfo.fileSize - offset > PSDK_TEST_FILE_MD5_BUFFER_SIZE) {
        returnCode = PsdkTest_ReadUpgradeProgramFile(offset, PSDK_TEST_FILE_MD5_BUFFER_SIZE,
                                                     fileBuffer, &realLen);
        if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS || realLen != PSDK_TEST_FILE_MD5_BUFFER_SIZE) {
            PsdkLogger_UserLogError("Get file data error, return code = 0x%08llX", returnCode);
            return returnCode;
        }

        UtilMd5_Update(&fileMd5Ctx, fileBuffer, PSDK_TEST_FILE_MD5_BUFFER_SIZE);

        offset += PSDK_TEST_FILE_MD5_BUFFER_SIZE;
    }

    returnCode = PsdkTest_ReadUpgradeProgramFile(offset, s_upgradeFileInfo.fileSize - offset, fileBuffer, &realLen);
    if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS || realLen != s_upgradeFileInfo.fileSize - offset) {
        PsdkLogger_UserLogError("Get file data error, return code = 0x%08llX", returnCode);
        return returnCode;
    }

    UtilMd5_Update(&fileMd5Ctx, fileBuffer, s_upgradeFileInfo.fileSize - offset);
    UtilMd5_Final(&fileMd5Ctx, md5);

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
