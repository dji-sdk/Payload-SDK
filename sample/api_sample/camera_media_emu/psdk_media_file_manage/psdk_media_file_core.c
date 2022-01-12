/**
 ********************************************************************
 * @file    psdk_media_file_core.c
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
#include <string.h>
#include <psdk_logger.h>

#include "psdk_media_file_core.h"
#include "psdk_media_file_jpg.h"
#include "psdk_media_file_mp4.h"
#include "psdk_platform.h"

/* Private constants ---------------------------------------------------------*/


/* Private types -------------------------------------------------------------*/


/* Private functions declaration ---------------------------------------------*/

/* Private values ------------------------------------------------------------*/
//@formatter:off
static const T_PsdkMediaFileOptItem s_mediaFileOpt[] =
{
    //JPEG File Operation Item
    {
        PSDK_CAMERA_FILE_TYPE_JPEG ,
        PsdkMediaFile_IsSupported_JPG,
        PsdkMediaFile_GetAttrFunc_JPG,
        PsdkMediaFile_GetDataOrigin_JPG,
        PsdkMediaFile_GetFileSizeOrigin_JPG,
        PsdkMediaFile_CreateThumbNail_JPG,
        PsdkMediaFile_GetFileSizeThumbNail_JPG,
        PsdkMediaFile_GetDataThumbNail_JPG,
        PsdkMediaFile_DestroyThumbNail_JPG,
        PsdkMediaFile_CreateScreenNail_JPG,
        PsdkMediaFile_GetFileSizeScreenNail_JPG,
        PsdkMediaFile_GetDataScreenNail_JPG,
        PsdkMediaFile_DestroyScreenNail_JPG,
    },
    //MP4 File Operation Item
    {
        PSDK_CAMERA_FILE_TYPE_MP4 ,
        PsdkMediaFile_IsSupported_MP4,
        PsdkMediaFile_GetAttrFunc_MP4,
        PsdkMediaFile_GetDataOrigin_MP4,
        PsdkMediaFile_GetFileSizeOrigin_MP4,
        PsdkMediaFile_CreateThumbNail_MP4,
        PsdkMediaFile_GetFileSizeThumbNail_MP4,
        PsdkMediaFile_GetDataThumbNail_MP4,
        PsdkMediaFile_DestroyThumbNail_MP4,
        PsdkMediaFile_CreateScreenNail_MP4,
        PsdkMediaFile_GetFileSizeScreenNail_MP4,
        PsdkMediaFile_GetDataScreenNail_MP4,
        PsdkMediaFile_DestroyScreenNail_MP4,
    },
};
static const uint32_t s_mediaFileOptCount = sizeof (s_mediaFileOpt) / sizeof(T_PsdkMediaFileOptItem);
//@formatter:on

/* Exported functions definition ---------------------------------------------*/
bool PsdkMediaFile_IsSupported(const char *filePath)
{
    int i;

    for (i = 0; i < s_mediaFileOptCount; i++) {
        if (s_mediaFileOpt[i].isSupportedFunc(filePath) == true) {
            return true;
        }
    }

    return false;
}

T_PsdkReturnCode PsdkMediaFile_CreateHandle(const char *filePath, T_PsdkMediaFileHandle *pMediaFileHandle)
{
    int optIndex;

    for (optIndex = 0; optIndex < s_mediaFileOptCount; optIndex++) {
        if (s_mediaFileOpt[optIndex].isSupportedFunc(filePath) == true) {
            break;
        }
    }

    if (optIndex == s_mediaFileOptCount) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    *pMediaFileHandle = PsdkOsal_Malloc(sizeof(T_PsdkMediaFile));
    if (*pMediaFileHandle == NULL) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_MEMORY_ALLOC_FAILED;
    }

    (*pMediaFileHandle)->filePath = PsdkOsal_Malloc(strlen(filePath) + 1);
    if ((*pMediaFileHandle)->filePath == NULL) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_MEMORY_ALLOC_FAILED;
    }

    (*pMediaFileHandle)->mediaFileOptItem = s_mediaFileOpt[optIndex];
    (*pMediaFileHandle)->mediaFileThm.privThm = NULL;
    (*pMediaFileHandle)->mediaFileScr.privScr = NULL;

    strcpy((*pMediaFileHandle)->filePath, filePath);

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_PsdkReturnCode PsdkMediaFile_DestroyHandle(T_PsdkMediaFileHandle mediaFileHandle)
{
    PsdkOsal_Free(mediaFileHandle->filePath);

    PsdkOsal_Free(mediaFileHandle);

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_PsdkReturnCode PsdkMediaFile_GetMediaFileType(T_PsdkMediaFileHandle mediaFileHandle,
                                                E_PsdkCameraMediaFileType *mediaFileType)
{
    *mediaFileType = mediaFileHandle->mediaFileOptItem.mediaFileType;

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_PsdkReturnCode PsdkMediaFile_GetMediaFileAttr(T_PsdkMediaFileHandle mediaFileHandle,
                                                T_PsdkCameraMediaFileAttr *mediaFileAttr)
{
    if (mediaFileHandle->mediaFileOptItem.getAttrFunc == NULL) {
        PsdkLogger_UserLogError("Media file handle getAttrFunc null error");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    return mediaFileHandle->mediaFileOptItem.getAttrFunc(mediaFileHandle, mediaFileAttr);
}

T_PsdkReturnCode PsdkMediaFile_GetDataOrg(struct _PsdkMediaFile *mediaFileHandle, uint32_t offset, uint16_t len,
                                          uint8_t *data, uint16_t *realLen)
{
    if (mediaFileHandle->mediaFileOptItem.getDataOrgFunc == NULL) {
        PsdkLogger_UserLogError("Media file handle getDataOrgFunc null error");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    return mediaFileHandle->mediaFileOptItem.getDataOrgFunc(mediaFileHandle, offset, len, data, realLen);
}

T_PsdkReturnCode PsdkMediaFile_GetFileSizeOrg(struct _PsdkMediaFile *mediaFileHandle, uint32_t *fileSize)
{
    if (mediaFileHandle->mediaFileOptItem.getFileSizeOrgFunc == NULL) {
        PsdkLogger_UserLogError("Media file handle getFileSizeOrgFunc null error");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    return mediaFileHandle->mediaFileOptItem.getFileSizeOrgFunc(mediaFileHandle, fileSize);
}

T_PsdkReturnCode PsdkMediaFile_CreateThm(T_PsdkMediaFileHandle mediaFileHandle)
{
    if (mediaFileHandle->mediaFileOptItem.createThmFunc == NULL) {
        PsdkLogger_UserLogError("Media file handle createThmFunc null error");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    return mediaFileHandle->mediaFileOptItem.createThmFunc(mediaFileHandle);
}

T_PsdkReturnCode PsdkMediaFile_GetFileSizeThm(T_PsdkMediaFileHandle mediaFileHandle, uint32_t *fileSize)
{
    if (mediaFileHandle->mediaFileOptItem.getFileSizeThmFunc == NULL) {
        PsdkLogger_UserLogError("Media file handle getFileSizeThmFunc null error");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    return mediaFileHandle->mediaFileOptItem.getFileSizeThmFunc(mediaFileHandle, fileSize);
}

T_PsdkReturnCode PsdkMediaFile_GetDataThm(T_PsdkMediaFileHandle mediaFileHandle, uint32_t offset, uint16_t len,
                                          uint8_t *data, uint16_t *realLen)
{
    if (mediaFileHandle->mediaFileOptItem.getDataThmFunc == NULL) {
        PsdkLogger_UserLogError("Media file handle getDataThmFunc null error");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    return mediaFileHandle->mediaFileOptItem.getDataThmFunc(mediaFileHandle, offset, len, data, realLen);
}

T_PsdkReturnCode PsdkMediaFile_DestoryThm(T_PsdkMediaFileHandle mediaFileHandle)
{
    if (mediaFileHandle->mediaFileOptItem.destroyThmFunc == NULL) {
        PsdkLogger_UserLogError("Media file handle destroyThmFunc null error");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    return mediaFileHandle->mediaFileOptItem.destroyThmFunc(mediaFileHandle);
}

T_PsdkReturnCode PsdkMediaFile_CreateScr(T_PsdkMediaFileHandle mediaFileHandle)
{
    if (mediaFileHandle->mediaFileOptItem.creatScrFunc == NULL) {
        PsdkLogger_UserLogError("Media file handle creatScrFunc null error");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    return mediaFileHandle->mediaFileOptItem.creatScrFunc(mediaFileHandle);
}

T_PsdkReturnCode PsdkMediaFile_GetFileSizeScr(T_PsdkMediaFileHandle mediaFileHandle, uint32_t *fileSize)
{
    if (mediaFileHandle->mediaFileOptItem.getFileSizeScrFunc == NULL) {
        PsdkLogger_UserLogError("Media file handle getFileSizeScrFunc null error");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    return mediaFileHandle->mediaFileOptItem.getFileSizeScrFunc(mediaFileHandle, fileSize);
}

T_PsdkReturnCode PsdkMediaFile_GetDataScr(T_PsdkMediaFileHandle mediaFileHandle, uint32_t offset, uint16_t len,
                                          uint8_t *data, uint16_t *realLen)
{
    if (mediaFileHandle->mediaFileOptItem.getDataScrFunc == NULL) {
        PsdkLogger_UserLogError("Media file handle getDataScrFunc null error");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    return mediaFileHandle->mediaFileOptItem.getDataScrFunc(mediaFileHandle, offset, len, data, realLen);
}

T_PsdkReturnCode PsdkMediaFile_DestroyScr(T_PsdkMediaFileHandle mediaFileHandle)
{
    if (mediaFileHandle->mediaFileOptItem.destroyScrFunc == NULL) {
        PsdkLogger_UserLogError("Media file handle destroyScrFunc null error");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    return mediaFileHandle->mediaFileOptItem.destroyScrFunc(mediaFileHandle);
}

/* Private functions definition-----------------------------------------------*/

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
