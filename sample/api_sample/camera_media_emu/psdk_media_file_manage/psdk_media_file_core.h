/**
 ********************************************************************
 * @file    psdk_media_file_core.h
 * @version V0.0.0
 * @date    2019/01/01
 * @brief   This is the header file for "psdk_media_file_core.c", defining the structure and
 * (exported) function prototypes.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef PSDK_MEDIA_FILE_CORE_H
#define PSDK_MEDIA_FILE_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <psdk_typedef.h>
#include <psdk_payload_camera.h>

/* Exported constants --------------------------------------------------------*/
#define PSDK_MEDIA_FILE_PATH_LEN_MAX           512             /*max file path len */
#define PSDK_MEDIA_DIR_PATH_LEN_MAX            256             /*max dir path len */

/* Exported types ------------------------------------------------------------*/
typedef struct {
    void *privThm;
} T_PsdkMediaFileThm;

typedef struct {
    void *privScr;
} T_PsdkMediaFileScr;

struct _PsdkMediaFile;

typedef struct {
    E_PsdkCameraMediaFileType mediaFileType;
    bool (*isSupportedFunc)(const char *filePath);

    T_PsdkReturnCode (*getAttrFunc)(struct _PsdkMediaFile *mediaFileHandle, T_PsdkCameraMediaFileAttr *mediaFileAttr);

    T_PsdkReturnCode (*getDataOrgFunc)(struct _PsdkMediaFile *mediaFileHandle, uint32_t offset, uint16_t len,
                                       uint8_t *data, uint16_t *realLen);
    T_PsdkReturnCode (*getFileSizeOrgFunc)(struct _PsdkMediaFile *mediaFileHandle, uint32_t *fileSize);

    T_PsdkReturnCode (*createThmFunc)(struct _PsdkMediaFile *mediaFileHandle);
    T_PsdkReturnCode (*getFileSizeThmFunc)(struct _PsdkMediaFile *mediaFileHandle, uint32_t *fileSize);
    T_PsdkReturnCode (*getDataThmFunc)(struct _PsdkMediaFile *mediaFileHandle, uint32_t offset, uint16_t len,
                                       uint8_t *data, uint16_t *realLen);
    T_PsdkReturnCode (*destroyThmFunc)(struct _PsdkMediaFile *mediaFileHandle);

    T_PsdkReturnCode (*creatScrFunc)(struct _PsdkMediaFile *mediaFileHandle);
    T_PsdkReturnCode (*getFileSizeScrFunc)(struct _PsdkMediaFile *mediaFileHandle, uint32_t *fileSize);
    T_PsdkReturnCode (*getDataScrFunc)(struct _PsdkMediaFile *mediaFileHandle, uint32_t offset, uint16_t len,
                                       uint8_t *data, uint16_t *realLen);
    T_PsdkReturnCode (*destroyScrFunc)(struct _PsdkMediaFile *mediaFileHandle);
} T_PsdkMediaFileOptItem;

typedef struct _PsdkMediaFile {
    char *filePath;
    T_PsdkMediaFileOptItem mediaFileOptItem;
    T_PsdkMediaFileThm mediaFileThm;
    T_PsdkMediaFileScr mediaFileScr;
} T_PsdkMediaFile, *T_PsdkMediaFileHandle;

/* Exported functions --------------------------------------------------------*/
bool PsdkMediaFile_IsSupported(const char *filePath);
T_PsdkReturnCode PsdkMediaFile_CreateHandle(const char *filePath, T_PsdkMediaFileHandle *pMediaFileHandle);
T_PsdkReturnCode PsdkMediaFile_DestroyHandle(T_PsdkMediaFileHandle mediaFileHandle);

T_PsdkReturnCode PsdkMediaFile_GetMediaFileType(T_PsdkMediaFileHandle mediaFileHandle,
                                                E_PsdkCameraMediaFileType *mediaFileType);
T_PsdkReturnCode PsdkMediaFile_GetMediaFileAttr(T_PsdkMediaFileHandle mediaFileHandle,
                                                T_PsdkCameraMediaFileAttr *mediaFileAttr);

T_PsdkReturnCode PsdkMediaFile_GetDataOrg(struct _PsdkMediaFile *mediaFileHandle, uint32_t offset, uint16_t len,
                                          uint8_t *data, uint16_t *realLen);
T_PsdkReturnCode PsdkMediaFile_GetFileSizeOrg(struct _PsdkMediaFile *mediaFileHandle, uint32_t *fileSize);

T_PsdkReturnCode PsdkMediaFile_CreateThm(T_PsdkMediaFileHandle mediaFileHandle);
T_PsdkReturnCode PsdkMediaFile_GetFileSizeThm(T_PsdkMediaFileHandle mediaFileHandle, uint32_t *fileSize);
T_PsdkReturnCode PsdkMediaFile_GetDataThm(T_PsdkMediaFileHandle mediaFileHandle, uint32_t offset, uint16_t len,
                                          uint8_t *data, uint16_t *realLen);
T_PsdkReturnCode PsdkMediaFile_DestoryThm(T_PsdkMediaFileHandle mediaFileHandle);

T_PsdkReturnCode PsdkMediaFile_CreateScr(T_PsdkMediaFileHandle mediaFileHandle);
T_PsdkReturnCode PsdkMediaFile_GetFileSizeScr(T_PsdkMediaFileHandle mediaFileHandle, uint32_t *fileSize);
T_PsdkReturnCode PsdkMediaFile_GetDataScr(T_PsdkMediaFileHandle mediaFileHandle, uint32_t offset, uint16_t len,
                                          uint8_t *data, uint16_t *realLen);
T_PsdkReturnCode PsdkMediaFile_DestroyScr(T_PsdkMediaFileHandle mediaFileHandle);

#ifdef __cplusplus
}
#endif

#endif // PSDK_MEDIA_FILE_CORE_H

/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
