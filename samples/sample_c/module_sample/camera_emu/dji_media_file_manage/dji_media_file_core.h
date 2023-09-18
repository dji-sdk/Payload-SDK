/**
 ********************************************************************
 * @file    dji_media_file_core.h
 * @brief   This is the header file for "dji_media_file_core.c", defining the structure and
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
#ifndef PSDK_MEDIA_FILE_CORE_H
#define PSDK_MEDIA_FILE_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <dji_typedef.h>
#include <dji_payload_camera.h>

/* Exported constants --------------------------------------------------------*/
#define PSDK_MEDIA_FILE_PATH_LEN_MAX           512             /*max file path len */
#define PSDK_MEDIA_DIR_PATH_LEN_MAX            256             /*max dir path len */

/* Exported types ------------------------------------------------------------*/
typedef struct {
    void *privThm;
} T_DjiMediaFileThm;

typedef struct {
    void *privScr;
} T_DjiMediaFileScr;

struct _DjiMediaFile;

typedef struct {
    E_DjiCameraMediaFileType mediaFileType;
    bool (*isSupportedFunc)(const char *filePath);

    T_DjiReturnCode (*getAttrFunc)(struct _DjiMediaFile *mediaFileHandle, T_DjiCameraMediaFileAttr *mediaFileAttr);

    T_DjiReturnCode (*getDataOrgFunc)(struct _DjiMediaFile *mediaFileHandle, uint32_t offset, uint16_t len,
                                      uint8_t *data, uint32_t *realLen);
    T_DjiReturnCode (*getFileSizeOrgFunc)(struct _DjiMediaFile *mediaFileHandle, uint32_t *fileSize);

    T_DjiReturnCode (*createThmFunc)(struct _DjiMediaFile *mediaFileHandle);
    T_DjiReturnCode (*getFileSizeThmFunc)(struct _DjiMediaFile *mediaFileHandle, uint32_t *fileSize);
    T_DjiReturnCode (*getDataThmFunc)(struct _DjiMediaFile *mediaFileHandle, uint32_t offset, uint16_t len,
                                      uint8_t *data, uint16_t *realLen);
    T_DjiReturnCode (*destroyThmFunc)(struct _DjiMediaFile *mediaFileHandle);

    T_DjiReturnCode (*creatScrFunc)(struct _DjiMediaFile *mediaFileHandle);
    T_DjiReturnCode (*getFileSizeScrFunc)(struct _DjiMediaFile *mediaFileHandle, uint32_t *fileSize);
    T_DjiReturnCode (*getDataScrFunc)(struct _DjiMediaFile *mediaFileHandle, uint32_t offset, uint16_t len,
                                      uint8_t *data, uint16_t *realLen);
    T_DjiReturnCode (*destroyScrFunc)(struct _DjiMediaFile *mediaFileHandle);
} T_DjiMediaFileOptItem;

typedef struct _DjiMediaFile {
    char *filePath;
    T_DjiMediaFileOptItem mediaFileOptItem;
    T_DjiMediaFileThm mediaFileThm;
    T_DjiMediaFileScr mediaFileScr;
} T_DjiMediaFile, *T_DjiMediaFileHandle;

/* Exported functions --------------------------------------------------------*/
bool DjiMediaFile_IsSupported(const char *filePath);
T_DjiReturnCode DjiMediaFile_CreateHandle(const char *filePath, T_DjiMediaFileHandle *pMediaFileHandle);
T_DjiReturnCode DjiMediaFile_DestroyHandle(T_DjiMediaFileHandle mediaFileHandle);

T_DjiReturnCode DjiMediaFile_GetMediaFileType(T_DjiMediaFileHandle mediaFileHandle,
                                              E_DjiCameraMediaFileType *mediaFileType);
T_DjiReturnCode DjiMediaFile_GetMediaFileAttr(T_DjiMediaFileHandle mediaFileHandle,
                                              T_DjiCameraMediaFileAttr *mediaFileAttr);

T_DjiReturnCode DjiMediaFile_GetDataOrg(struct _DjiMediaFile *mediaFileHandle, uint32_t offset, uint16_t len,
                                        uint8_t *data, uint32_t *realLen);
T_DjiReturnCode DjiMediaFile_GetFileSizeOrg(struct _DjiMediaFile *mediaFileHandle, uint32_t *fileSize);

T_DjiReturnCode DjiMediaFile_CreateThm(T_DjiMediaFileHandle mediaFileHandle);
T_DjiReturnCode DjiMediaFile_GetFileSizeThm(T_DjiMediaFileHandle mediaFileHandle, uint32_t *fileSize);
T_DjiReturnCode DjiMediaFile_GetDataThm(T_DjiMediaFileHandle mediaFileHandle, uint32_t offset, uint16_t len,
                                        uint8_t *data, uint16_t *realLen);
T_DjiReturnCode DjiMediaFile_DestoryThm(T_DjiMediaFileHandle mediaFileHandle);

T_DjiReturnCode DjiMediaFile_CreateScr(T_DjiMediaFileHandle mediaFileHandle);
T_DjiReturnCode DjiMediaFile_GetFileSizeScr(T_DjiMediaFileHandle mediaFileHandle, uint32_t *fileSize);
T_DjiReturnCode DjiMediaFile_GetDataScr(T_DjiMediaFileHandle mediaFileHandle, uint32_t offset, uint16_t len,
                                        uint8_t *data, uint16_t *realLen);
T_DjiReturnCode DjiMediaFile_DestroyScr(T_DjiMediaFileHandle mediaFileHandle);

#ifdef __cplusplus
}
#endif

#endif // PSDK_MEDIA_FILE_CORE_H

/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
