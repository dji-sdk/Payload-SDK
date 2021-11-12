/**
 ********************************************************************
 * @file    util_misc.c
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
#ifdef SYSTEM_ARCH_LINUX

#include <stdio.h>
#include "util_misc.h"

/* Private constants ---------------------------------------------------------*/
const char *baseStr = "[>>>>>>>>>>>>>---------------------------------------------------------------------------------------]  13%";

/* Private types -------------------------------------------------------------*/

/* Private values ------------------------------------------------------------*/

/* Private functions declaration ---------------------------------------------*/

/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode DjiUserUtil_GetCurrentFileDirPath(const char *filePath, uint32_t pathBufferSize, char *dirPath)
{
    uint32_t i = strlen(filePath) - 1;
    uint32_t dirPathLen;

    while (filePath[i] != '/') {
        i--;
    }

    dirPathLen = i + 1;

    if (dirPathLen + 1 > pathBufferSize) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    memcpy(dirPath, filePath, dirPathLen);
    dirPath[dirPathLen] = 0;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiUserUtil_RunSystemCmd(const char *systemCmdStr)
{
    FILE *fp;

    fp = popen(systemCmdStr, "r");
    if (fp == NULL) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    pclose(fp);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

void DjiUserUtil_PrintProgressBar(uint16_t currentProgress, uint16_t totalProgress, char *userData)
{
    for (int j = 0; j < strlen(baseStr) + strlen(userData) + 4; ++j) {
        printf("\b");
    }

    printf("[");
    for (int j = 0; j < totalProgress; ++j) {
        if (j < currentProgress) {
            printf("%c", '>');
        } else {
            printf("-");
        }
    }

    printf("] ");
    printf("%3d%%", currentProgress);
    printf("%s", userData);

    fflush(stdout);
}

/* Private functions definition-----------------------------------------------*/

#endif

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
