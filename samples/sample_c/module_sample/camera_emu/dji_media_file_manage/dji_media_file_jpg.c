/**
 ********************************************************************
 * @file    dji_media_file_jpg.c
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
#include "dji_media_file_jpg.h"
#include "dji_media_file_core.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dji_logger.h>
#include <utils/util_misc.h>
#include "dji_platform.h"
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
} T_DjiJPGTempFilePriv;

/* Private functions declaration ---------------------------------------------*/
static T_DjiReturnCode DjiMediaFile_CreateTempFilePriv_JPG(const char *srcFilePath, const char *scaleCfgStr,
                                                           T_DjiJPGTempFilePriv **pTempFilePrivHandle);
static T_DjiReturnCode DjiMediaFile_DestroyTempFilePriv_JPG(T_DjiJPGTempFilePriv *tempFilePrivHandle);

/* Exported functions definition ---------------------------------------------*/
bool DjiMediaFile_IsSupported_JPG(const char *filePath)
{
    if (filePath == NULL) {
        USER_LOG_ERROR("input parameter is null error");
        return false;
    }

    if (strcmp(&filePath[strlen(filePath) - 4], JPG_FILE_SUFFIX) == 0) {
        return true;
    }

    return false;
}

T_DjiReturnCode DjiMediaFile_GetAttrFunc_JPG(struct _DjiMediaFile *mediaFileHandle,
                                             T_DjiCameraMediaFileAttr *mediaFileAttr)
{
    USER_UTIL_UNUSED(mediaFileHandle);

    mediaFileAttr->attrVideoDuration = 0;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiMediaFile_GetDataOrigin_JPG(struct _DjiMediaFile *mediaFileHandle, uint32_t offset, uint16_t len,
                                               uint8_t *data, uint32_t *realLen)
{
    return UtilFile_GetFileDataByPath(mediaFileHandle->filePath, offset, len, data, realLen);
}

T_DjiReturnCode DjiMediaFile_GetFileSizeOrigin_JPG(struct _DjiMediaFile *mediaFileHandle, uint32_t *fileSize)
{
    return UtilFile_GetFileSizeByPath(mediaFileHandle->filePath, fileSize);
}

T_DjiReturnCode DjiMediaFile_CreateThumbNail_JPG(struct _DjiMediaFile *mediaFileHandle)
{
    return DjiMediaFile_CreateTempFilePriv_JPG(mediaFileHandle->filePath, JPG_THM_SCALE_CFG_STR,
                                               (T_DjiJPGTempFilePriv **) &mediaFileHandle->mediaFileThm.privThm);
}

T_DjiReturnCode DjiMediaFile_GetFileSizeThumbNail_JPG(struct _DjiMediaFile *mediaFileHandle, uint32_t *fileSize)
{
    T_DjiJPGTempFilePriv *jpgFileThmPriv = (T_DjiJPGTempFilePriv *) mediaFileHandle->mediaFileThm.privThm;

    return UtilFile_GetFileSize(jpgFileThmPriv->tempFile, fileSize);
}

T_DjiReturnCode
DjiMediaFile_GetDataThumbNail_JPG(struct _DjiMediaFile *mediaFileHandle, uint32_t offset, uint16_t len,
                                  uint8_t *data, uint16_t *realLen)
{
    T_DjiJPGTempFilePriv *jpgFileThmPriv = (T_DjiJPGTempFilePriv *) mediaFileHandle->mediaFileThm.privThm;

    return UtilFile_GetFileData(jpgFileThmPriv->tempFile, offset, len, data, realLen);
}

T_DjiReturnCode DjiMediaFile_DestroyThumbNail_JPG(struct _DjiMediaFile *mediaFileHandle)
{
    return DjiMediaFile_DestroyTempFilePriv_JPG(mediaFileHandle->mediaFileThm.privThm);
}

T_DjiReturnCode DjiMediaFile_CreateScreenNail_JPG(struct _DjiMediaFile *mediaFileHandle)
{
    return DjiMediaFile_CreateTempFilePriv_JPG(mediaFileHandle->filePath, JPG_SCR_SCALE_CFG_STR,
                                               (T_DjiJPGTempFilePriv **) &mediaFileHandle->mediaFileScr.privScr);
}

T_DjiReturnCode DjiMediaFile_GetFileSizeScreenNail_JPG(struct _DjiMediaFile *mediaFileHandle, uint32_t *fileSize)
{
    T_DjiJPGTempFilePriv *jpgFileScrPriv = (T_DjiJPGTempFilePriv *) mediaFileHandle->mediaFileScr.privScr;

    return UtilFile_GetFileSize(jpgFileScrPriv->tempFile, fileSize);
}

T_DjiReturnCode
DjiMediaFile_GetDataScreenNail_JPG(struct _DjiMediaFile *mediaFileHandle, uint32_t offset, uint16_t len,
                                   uint8_t *data, uint16_t *realLen)
{
    T_DjiJPGTempFilePriv *jpgFileScrPriv = (T_DjiJPGTempFilePriv *) mediaFileHandle->mediaFileScr.privScr;

    return UtilFile_GetFileData(jpgFileScrPriv->tempFile, offset, len, data, realLen);
}

T_DjiReturnCode DjiMediaFile_DestroyScreenNail_JPG(struct _DjiMediaFile *mediaFileHandle)
{
    return DjiMediaFile_DestroyTempFilePriv_JPG(mediaFileHandle->mediaFileScr.privScr);
}

/* Private functions definition-----------------------------------------------*/
static T_DjiReturnCode DjiMediaFile_CreateTempFilePriv_JPG(const char *srcFilePath, const char *scaleCfgStr,
                                                           T_DjiJPGTempFilePriv **pTempFilePrivHandle)
{
    char ffmpeg_cmd[FFMPEG_CMD_BUF_SIZE];
    int cmdRet;
    T_DjiRunTimeStamps tiStart, tiEnd;
    T_DjiJPGTempFilePriv *jpgTempFilePriv;
    T_DjiReturnCode psdkStat;
    int tempFd;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    tiStart = DjiUtilTime_GetRunTimeStamps();

    *pTempFilePrivHandle = osalHandler->Malloc(sizeof(T_DjiJPGTempFilePriv));
    if (*pTempFilePrivHandle == NULL) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_MEMORY_ALLOC_FAILED;
    }

    jpgTempFilePriv = *pTempFilePrivHandle;

    //get temp file name
    strcpy(jpgTempFilePriv->tempfilePath, JPG_TEMP_FILE_TEMPLATE_STR);
    tempFd = mkstemps(jpgTempFilePriv->tempfilePath, strlen(JPG_FILE_SUFFIX));
    if (tempFd < 0) {
        USER_LOG_ERROR("JPG Create Temp File Error, tempFd = %d\n", tempFd);
        psdkStat = DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
        goto err_create_temp;
    }
    close(tempFd);
    unlink(jpgTempFilePriv->tempfilePath);

    //ffmpeg cmd send
    snprintf(ffmpeg_cmd, FFMPEG_CMD_BUF_SIZE, "ffmpeg -i \"%s\" -vf %s %s 1>/dev/null 2>&1",
             srcFilePath, scaleCfgStr, jpgTempFilePriv->tempfilePath);

    cmdRet = system(ffmpeg_cmd);

    tiEnd = DjiUtilTime_GetRunTimeStamps();

    USER_LOG_DEBUG("JPG Create TempFile, RealTime = %ld us\n", tiEnd.realUsec - tiStart.realUsec);

    if (cmdRet != 0) {
        USER_LOG_ERROR("JPG ffmpeg cmd call error, ret = %d\n", cmdRet);
        psdkStat = DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
        goto err_system_cmd;
    }

    //open temp file
    jpgTempFilePriv->tempFile = fopen(jpgTempFilePriv->tempfilePath, "rb+");
    if (jpgTempFilePriv->tempFile == NULL) {
        psdkStat = DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
        goto err_file_open;
    }

    //unlink temp file
    unlink(jpgTempFilePriv->tempfilePath);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;

err_file_open:
    unlink(jpgTempFilePriv->tempfilePath);
err_system_cmd:
err_create_temp:
    osalHandler->Free(*pTempFilePrivHandle);

    return psdkStat;
}

static T_DjiReturnCode DjiMediaFile_DestroyTempFilePriv_JPG(T_DjiJPGTempFilePriv *tempFilePrivHandle)
{
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    fclose(tempFilePrivHandle->tempFile);
    osalHandler->Free(tempFilePrivHandle);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
