/**
 ********************************************************************
 * @file    osal_fs.h
 * @brief   This is the header file for "osal_fs.c", defining the structure and
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
#ifndef OSAL_FS_H
#define OSAL_FS_H

/* Includes ------------------------------------------------------------------*/
#include "dji_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
T_DjiReturnCode Osal_FileOpen(const char *fileName, const char *fileMode, T_DjiFileHandle *fileObj);

T_DjiReturnCode Osal_FileClose(T_DjiFileHandle fileObj);

T_DjiReturnCode Osal_FileWrite(T_DjiFileHandle fileObj, const uint8_t *buf, uint32_t len, uint32_t *realLen);

T_DjiReturnCode Osal_FileRead(T_DjiFileHandle fileObj, uint8_t *buf, uint32_t len, uint32_t *realLen);

T_DjiReturnCode Osal_FileSeek(T_DjiFileHandle fileObj, uint32_t offset);

T_DjiReturnCode Osal_FileSync(T_DjiFileHandle fileObj);

T_DjiReturnCode Osal_DirOpen(const char *filePath, T_DjiDirHandle *dirObj);

T_DjiReturnCode Osal_DirClose(T_DjiDirHandle dirObj);

T_DjiReturnCode Osal_DirRead(T_DjiDirHandle dirObj, T_DjiFileInfo *fileInfo);

T_DjiReturnCode Osal_Mkdir(const char *filePath);

T_DjiReturnCode Osal_Unlink(const char *filePath);

T_DjiReturnCode Osal_Rename(const char *oldFilePath, const char *newFilePath);

T_DjiReturnCode Osal_Stat(const char *filePath, T_DjiFileInfo *fileInfo);

#ifdef __cplusplus
}
#endif

#endif // OSAL_FS_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
