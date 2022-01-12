/**
 ********************************************************************
 * @file    psdk_media_file_JPG.c
 * @version V1.0.0
 * @date    2019/01/01
 * @brief
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


/* Includes ------------------------------------------------------------------*/
#include "psdk_media_file_jpg.h"
#include "psdk_media_file_core.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <psdk_logger.h>
#include <utils/util_misc.h>
#include "psdk_platform.h"
#include "utils/util_time.h"
#include "utils/util_file.h"

/* Private constants ---------------------------------------------------------*/
#define JPG_FILE_SUFFIX                 ".jpg"
#define JPG_TEMP_FILE_TEMPLATE_STR      "JPG_TEMP_XXXXXX.jpg"
#define JPG_TEMP_FILE_PATH_MAX_LEN      50
#define FFMPEG_CMD_BUF_SIZE             1024

#define JPG_THM_SCALE_CFG_STR           "scale=100:-1"
#define JPG_SCR_SCALE_CFG_STR           "scale=600:-1"

/* Private types -------------------------------------------------------------*/
typedef struct {
    FILE *tempFile;
    char tempfilePath[JPG_TEMP_FILE_PATH_MAX_LEN];
} T_PsdkJPGTempFilePriv;

/* Private functions declaration ---------------------------------------------*/
static T_PsdkReturnCode PsdkMediaFile_CreateTempFilePriv_JPG(const char *srcFilePath, const char *scaleCfgStr,
                                                             T_PsdkJPGTempFilePriv **pTempFilePrivHandle);
static T_PsdkReturnCode PsdkMediaFile_DestroyTempFilePriv_JPG(T_PsdkJPGTempFilePriv *tempFilePrivHandle);

/* Exported functions definition ---------------------------------------------*/
bool PsdkMediaFile_IsSupported_JPG(const char *filePath)
{
    if (filePath == NULL) {
        PsdkLogger_UserLogError("input parameter is null error");
        return false;
    }

    if (strcmp(&filePath[strlen(filePath) - 4], JPG_FILE_SUFFIX) == 0) {
        return true;
    }

    return false;
}

T_PsdkReturnCode PsdkMediaFile_GetAttrFunc_JPG(struct _PsdkMediaFile *mediaFileHandle,
                                               T_PsdkCameraMediaFileAttr *mediaFileAttr)
{
    USER_UTIL_UNUSED(mediaFileHandle);

    mediaFileAttr->attrVideoDuration = 0;

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_PsdkReturnCode PsdkMediaFile_GetDataOrigin_JPG(struct _PsdkMediaFile *mediaFileHandle, uint32_t offset, uint16_t len,
                                                 uint8_t *data, uint16_t *realLen)
{
    return UtilFile_GetFileDataByPath(mediaFileHandle->filePath, offset, len, data, realLen);
}

T_PsdkReturnCode PsdkMediaFile_GetFileSizeOrigin_JPG(struct _PsdkMediaFile *mediaFileHandle, uint32_t *fileSize)
{
    return UtilFile_GetFileSizeByPath(mediaFileHandle->filePath, fileSize);
}

T_PsdkReturnCode PsdkMediaFile_CreateThumbNail_JPG(struct _PsdkMediaFile *mediaFileHandle)
{
    return PsdkMediaFile_CreateTempFilePriv_JPG(mediaFileHandle->filePath, JPG_THM_SCALE_CFG_STR,
                                                (T_PsdkJPGTempFilePriv **) &mediaFileHandle->mediaFileThm.privThm);
}

T_PsdkReturnCode PsdkMediaFile_GetFileSizeThumbNail_JPG(struct _PsdkMediaFile *mediaFileHandle, uint32_t *fileSize)
{
    T_PsdkJPGTempFilePriv *jpgFileThmPriv = (T_PsdkJPGTempFilePriv *) mediaFileHandle->mediaFileThm.privThm;

    return UtilFile_GetFileSize(jpgFileThmPriv->tempFile, fileSize);
}

T_PsdkReturnCode
PsdkMediaFile_GetDataThumbNail_JPG(struct _PsdkMediaFile *mediaFileHandle, uint32_t offset, uint16_t len,
                                   uint8_t *data, uint16_t *realLen)
{
    T_PsdkJPGTempFilePriv *jpgFileThmPriv = (T_PsdkJPGTempFilePriv *) mediaFileHandle->mediaFileThm.privThm;

    return UtilFile_GetFileData(jpgFileThmPriv->tempFile, offset, len, data, realLen);
}

T_PsdkReturnCode PsdkMediaFile_DestroyThumbNail_JPG(struct _PsdkMediaFile *mediaFileHandle)
{
    return PsdkMediaFile_DestroyTempFilePriv_JPG(mediaFileHandle->mediaFileThm.privThm);
}

T_PsdkReturnCode PsdkMediaFile_CreateScreenNail_JPG(struct _PsdkMediaFile *mediaFileHandle)
{
    return PsdkMediaFile_CreateTempFilePriv_JPG(mediaFileHandle->filePath, JPG_SCR_SCALE_CFG_STR,
                                                (T_PsdkJPGTempFilePriv **) &mediaFileHandle->mediaFileScr.privScr);
}

T_PsdkReturnCode PsdkMediaFile_GetFileSizeScreenNail_JPG(struct _PsdkMediaFile *mediaFileHandle, uint32_t *fileSize)
{
    T_PsdkJPGTempFilePriv *jpgFileScrPriv = (T_PsdkJPGTempFilePriv *) mediaFileHandle->mediaFileScr.privScr;

    return UtilFile_GetFileSize(jpgFileScrPriv->tempFile, fileSize);
}

T_PsdkReturnCode
PsdkMediaFile_GetDataScreenNail_JPG(struct _PsdkMediaFile *mediaFileHandle, uint32_t offset, uint16_t len,
                                    uint8_t *data, uint16_t *realLen)
{
    T_PsdkJPGTempFilePriv *jpgFileScrPriv = (T_PsdkJPGTempFilePriv *) mediaFileHandle->mediaFileScr.privScr;

    return UtilFile_GetFileData(jpgFileScrPriv->tempFile, offset, len, data, realLen);
}

T_PsdkReturnCode PsdkMediaFile_DestroyScreenNail_JPG(struct _PsdkMediaFile *mediaFileHandle)
{
    return PsdkMediaFile_DestroyTempFilePriv_JPG(mediaFileHandle->mediaFileScr.privScr);
}

/* Private functions definition-----------------------------------------------*/
static T_PsdkReturnCode PsdkMediaFile_CreateTempFilePriv_JPG(const char *srcFilePath, const char *scaleCfgStr,
                                                             T_PsdkJPGTempFilePriv **pTempFilePrivHandle)
{
    char ffmpeg_cmd[FFMPEG_CMD_BUF_SIZE];
    int cmdRet;
    T_PsdkRunTimeStamps tiStart, tiEnd;
    T_PsdkJPGTempFilePriv *jpgTempFilePriv;
    T_PsdkReturnCode psdkStat;
    int tempFd;

    tiStart = PsdkUtilTime_GetRunTimeStamps();

    *pTempFilePrivHandle = PsdkOsal_Malloc(sizeof(T_PsdkJPGTempFilePriv));
    if (*pTempFilePrivHandle == NULL) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_MEMORY_ALLOC_FAILED;
    }

    jpgTempFilePriv = *pTempFilePrivHandle;

    //get temp file name
    strcpy(jpgTempFilePriv->tempfilePath, JPG_TEMP_FILE_TEMPLATE_STR);
    tempFd = mkstemps(jpgTempFilePriv->tempfilePath, strlen(JPG_FILE_SUFFIX));
    if (tempFd < 0) {
        PsdkLogger_UserLogError("JPG Create Temp File Error, tempFd = %d\n", tempFd);
        psdkStat = PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
        goto err_create_temp;
    }
    close(tempFd);
    unlink(jpgTempFilePriv->tempfilePath);

    //ffmpeg cmd send
    snprintf(ffmpeg_cmd, FFMPEG_CMD_BUF_SIZE, "ffmpeg -i \"%s\" -vf %s %s 1>/dev/null 2>&1",
             srcFilePath, scaleCfgStr, jpgTempFilePriv->tempfilePath);

    cmdRet = system(ffmpeg_cmd);

    tiEnd = PsdkUtilTime_GetRunTimeStamps();

    PsdkLogger_UserLogDebug("JPG Create TempFile, RealTime = %ld us\n", tiEnd.realUsec - tiStart.realUsec);

    if (cmdRet != 0) {
        PsdkLogger_UserLogError("JPG ffmpeg cmd call error, ret = %d\n", cmdRet);
        psdkStat = PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
        goto err_system_cmd;
    }

    //open temp file
    jpgTempFilePriv->tempFile = fopen(jpgTempFilePriv->tempfilePath, "rb+");
    if (jpgTempFilePriv->tempFile == NULL) {
        psdkStat = PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
        goto err_file_open;
    }

    //unlink temp file
    unlink(jpgTempFilePriv->tempfilePath);

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;

err_file_open:
    unlink(jpgTempFilePriv->tempfilePath);
err_system_cmd:
err_create_temp:
    PsdkOsal_Free(*pTempFilePrivHandle);

    return psdkStat;
}

static T_PsdkReturnCode PsdkMediaFile_DestroyTempFilePriv_JPG(T_PsdkJPGTempFilePriv *tempFilePrivHandle)
{
    fclose(tempFilePrivHandle->tempFile);
    PsdkOsal_Free(tempFilePrivHandle);

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
