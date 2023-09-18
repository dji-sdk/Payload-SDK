/**
 ********************************************************************
 * @file    dji_media_file_core.c
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
#include <string.h>
#include <dji_logger.h>

#include "dji_media_file_core.h"
#include "dji_media_file_jpg.h"
#include "dji_media_file_mp4.h"
#include "dji_platform.h"

/* Private constants ---------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private functions declaration ---------------------------------------------*/

/* Private values ------------------------------------------------------------*/
//@formatter:off
static const T_DjiMediaFileOptItem s_mediaFileOpt[] =
{
    //JPEG File Operation Item
    {
        DJI_CAMERA_FILE_TYPE_JPEG ,
        DjiMediaFile_IsSupported_JPG,
        DjiMediaFile_GetAttrFunc_JPG,
        DjiMediaFile_GetDataOrigin_JPG,
        DjiMediaFile_GetFileSizeOrigin_JPG,
        DjiMediaFile_CreateThumbNail_JPG,
        DjiMediaFile_GetFileSizeThumbNail_JPG,
        DjiMediaFile_GetDataThumbNail_JPG,
        DjiMediaFile_DestroyThumbNail_JPG,
        DjiMediaFile_CreateScreenNail_JPG,
        DjiMediaFile_GetFileSizeScreenNail_JPG,
        DjiMediaFile_GetDataScreenNail_JPG,
        DjiMediaFile_DestroyScreenNail_JPG,
    },
    //MP4 File Operation Item
    {
        DJI_CAMERA_FILE_TYPE_MP4 ,
        DjiMediaFile_IsSupported_MP4,
        DjiMediaFile_GetAttrFunc_MP4,
        DjiMediaFile_GetDataOrigin_MP4,
        DjiMediaFile_GetFileSizeOrigin_MP4,
        DjiMediaFile_CreateThumbNail_MP4,
        DjiMediaFile_GetFileSizeThumbNail_MP4,
        DjiMediaFile_GetDataThumbNail_MP4,
        DjiMediaFile_DestroyThumbNail_MP4,
        DjiMediaFile_CreateScreenNail_MP4,
        DjiMediaFile_GetFileSizeScreenNail_MP4,
        DjiMediaFile_GetDataScreenNail_MP4,
        DjiMediaFile_DestroyScreenNail_MP4,
    },
};
static const uint32_t s_mediaFileOptCount = sizeof (s_mediaFileOpt) / sizeof(T_DjiMediaFileOptItem);
//@formatter:on

/* Exported functions definition ---------------------------------------------*/
bool DjiMediaFile_IsSupported(const char *filePath)
{
    int i;

    for (i = 0; i < s_mediaFileOptCount; i++) {
        if (s_mediaFileOpt[i].isSupportedFunc(filePath) == true) {
            return true;
        }
    }

    return false;
}

T_DjiReturnCode DjiMediaFile_CreateHandle(const char *filePath, T_DjiMediaFileHandle *pMediaFileHandle)
{
    int optIndex;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    for (optIndex = 0; optIndex < s_mediaFileOptCount; optIndex++) {
        if (s_mediaFileOpt[optIndex].isSupportedFunc(filePath) == true) {
            break;
        }
    }

    if (optIndex == s_mediaFileOptCount) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    *pMediaFileHandle = osalHandler->Malloc(sizeof(T_DjiMediaFile));
    if (*pMediaFileHandle == NULL) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_MEMORY_ALLOC_FAILED;
    }

    (*pMediaFileHandle)->filePath = osalHandler->Malloc(strlen(filePath) + 1);
    if ((*pMediaFileHandle)->filePath == NULL) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_MEMORY_ALLOC_FAILED;
    }

    (*pMediaFileHandle)->mediaFileOptItem = s_mediaFileOpt[optIndex];
    (*pMediaFileHandle)->mediaFileThm.privThm = NULL;
    (*pMediaFileHandle)->mediaFileScr.privScr = NULL;

    strcpy((*pMediaFileHandle)->filePath, filePath);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiMediaFile_DestroyHandle(T_DjiMediaFileHandle mediaFileHandle)
{
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    osalHandler->Free(mediaFileHandle->filePath);
    osalHandler->Free(mediaFileHandle);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiMediaFile_GetMediaFileType(T_DjiMediaFileHandle mediaFileHandle,
                                              E_DjiCameraMediaFileType *mediaFileType)
{
    *mediaFileType = mediaFileHandle->mediaFileOptItem.mediaFileType;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiMediaFile_GetMediaFileAttr(T_DjiMediaFileHandle mediaFileHandle,
                                              T_DjiCameraMediaFileAttr *mediaFileAttr)
{
    if (mediaFileHandle->mediaFileOptItem.getAttrFunc == NULL) {
        USER_LOG_ERROR("Media file handle getAttrFunc null error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    return mediaFileHandle->mediaFileOptItem.getAttrFunc(mediaFileHandle, mediaFileAttr);
}

T_DjiReturnCode DjiMediaFile_GetDataOrg(struct _DjiMediaFile *mediaFileHandle, uint32_t offset, uint16_t len,
                                        uint8_t *data, uint32_t *realLen)
{
    if (mediaFileHandle->mediaFileOptItem.getDataOrgFunc == NULL) {
        USER_LOG_ERROR("Media file handle getDataOrgFunc null error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    return mediaFileHandle->mediaFileOptItem.getDataOrgFunc(mediaFileHandle, offset, len, data, realLen);
}

T_DjiReturnCode DjiMediaFile_GetFileSizeOrg(struct _DjiMediaFile *mediaFileHandle, uint32_t *fileSize)
{
    if (mediaFileHandle->mediaFileOptItem.getFileSizeOrgFunc == NULL) {
        USER_LOG_ERROR("Media file handle getFileSizeOrgFunc null error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    return mediaFileHandle->mediaFileOptItem.getFileSizeOrgFunc(mediaFileHandle, fileSize);
}

T_DjiReturnCode DjiMediaFile_CreateThm(T_DjiMediaFileHandle mediaFileHandle)
{
    if (mediaFileHandle->mediaFileOptItem.createThmFunc == NULL) {
        USER_LOG_ERROR("Media file handle createThmFunc null error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    return mediaFileHandle->mediaFileOptItem.createThmFunc(mediaFileHandle);
}

T_DjiReturnCode DjiMediaFile_GetFileSizeThm(T_DjiMediaFileHandle mediaFileHandle, uint32_t *fileSize)
{
    if (mediaFileHandle->mediaFileOptItem.getFileSizeThmFunc == NULL) {
        USER_LOG_ERROR("Media file handle getFileSizeThmFunc null error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    return mediaFileHandle->mediaFileOptItem.getFileSizeThmFunc(mediaFileHandle, fileSize);
}

T_DjiReturnCode DjiMediaFile_GetDataThm(T_DjiMediaFileHandle mediaFileHandle, uint32_t offset, uint16_t len,
                                        uint8_t *data, uint16_t *realLen)
{
    if (mediaFileHandle->mediaFileOptItem.getDataThmFunc == NULL) {
        USER_LOG_ERROR("Media file handle getDataThmFunc null error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    return mediaFileHandle->mediaFileOptItem.getDataThmFunc(mediaFileHandle, offset, len, data, realLen);
}

T_DjiReturnCode DjiMediaFile_DestoryThm(T_DjiMediaFileHandle mediaFileHandle)
{
    if (mediaFileHandle->mediaFileOptItem.destroyThmFunc == NULL) {
        USER_LOG_ERROR("Media file handle destroyThmFunc null error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    return mediaFileHandle->mediaFileOptItem.destroyThmFunc(mediaFileHandle);
}

T_DjiReturnCode DjiMediaFile_CreateScr(T_DjiMediaFileHandle mediaFileHandle)
{
    if (mediaFileHandle->mediaFileOptItem.creatScrFunc == NULL) {
        USER_LOG_ERROR("Media file handle creatScrFunc null error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    return mediaFileHandle->mediaFileOptItem.creatScrFunc(mediaFileHandle);
}

T_DjiReturnCode DjiMediaFile_GetFileSizeScr(T_DjiMediaFileHandle mediaFileHandle, uint32_t *fileSize)
{
    if (mediaFileHandle->mediaFileOptItem.getFileSizeScrFunc == NULL) {
        USER_LOG_ERROR("Media file handle getFileSizeScrFunc null error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    return mediaFileHandle->mediaFileOptItem.getFileSizeScrFunc(mediaFileHandle, fileSize);
}

T_DjiReturnCode DjiMediaFile_GetDataScr(T_DjiMediaFileHandle mediaFileHandle, uint32_t offset, uint16_t len,
                                        uint8_t *data, uint16_t *realLen)
{
    if (mediaFileHandle->mediaFileOptItem.getDataScrFunc == NULL) {
        USER_LOG_ERROR("Media file handle getDataScrFunc null error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    return mediaFileHandle->mediaFileOptItem.getDataScrFunc(mediaFileHandle, offset, len, data, realLen);
}

T_DjiReturnCode DjiMediaFile_DestroyScr(T_DjiMediaFileHandle mediaFileHandle)
{
    if (mediaFileHandle->mediaFileOptItem.destroyScrFunc == NULL) {
        USER_LOG_ERROR("Media file handle destroyScrFunc null error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    return mediaFileHandle->mediaFileOptItem.destroyScrFunc(mediaFileHandle);
}

/* Private functions definition-----------------------------------------------*/

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
