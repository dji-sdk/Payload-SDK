/**
 ********************************************************************
 * @file    test_waypoint_v3.c
 * @brief
 *
 * @copyright (c) 2018 DJI. All rights reserved.
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
#include <utils/util_file.h>
#include <utils/util_misc.h>
#include "test_waypoint_v3.h"
#include "dji_logger.h"
#include "dji_waypoint_v3.h"
#include "waypoint_file_c/waypoint_v3_test_file_kmz.h"

/* Private constants ---------------------------------------------------------*/
#define DJI_TEST_WAYPOINT_V3_KMZ_FILE_PATH_LEN_MAX         (256)

/* Private types -------------------------------------------------------------*/

/* Private values -------------------------------------------------------------*/
static T_DjiWaypointV3MissionState s_lastWaypointV3MissionState = {0};

/* Private functions declaration ---------------------------------------------*/
static T_DjiReturnCode DjiTest_WaypointV3StateCallback(T_DjiWaypointV3MissionState missionState);

/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode DjiTest_WaypointV3RunSample(void)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
#ifdef SYSTEM_ARCH_LINUX
    FILE *kmzFile;
    uint32_t kmzFileSize = 0;
    uint8_t *kmzFileBuf;
    uint16_t readLen;
    char curFileDirPath[DJI_TEST_WAYPOINT_V3_KMZ_FILE_PATH_LEN_MAX];
    char tempPath[DJI_TEST_WAYPOINT_V3_KMZ_FILE_PATH_LEN_MAX];

    returnCode = DjiWaypointV3_Init();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Waypoint v3 init failed.");
        return returnCode;
    }

    returnCode = DjiWaypointV3_RegMissionStateCallback(DjiTest_WaypointV3StateCallback);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Register waypoint v3 state callback failed.");
        goto out;
    }

    returnCode = DjiUserUtil_GetCurrentFileDirPath(__FILE__, DJI_TEST_WAYPOINT_V3_KMZ_FILE_PATH_LEN_MAX,
                                                   curFileDirPath);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Get file current path error, stat = 0x%08llX", returnCode);
        goto out;
    }

    /*! Attention: suggest use the exported kmz file by DJI pilot. If use this test file, you need set the longitude as
     * 113.94255, latitude as 22.57765 on DJI Assistant 2 simulator */
    snprintf(tempPath, DJI_TEST_WAYPOINT_V3_KMZ_FILE_PATH_LEN_MAX, "%s/waypoint_file/waypoint_v3_test_file.kmz",
             curFileDirPath);

    kmzFile = fopen(tempPath, "r");
    if (kmzFile == NULL) {
        USER_LOG_ERROR("Open kmz file failed.");
        goto out;
    }

    returnCode = UtilFile_GetFileSize(kmzFile, &kmzFileSize);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Get kmz file size failed.");
        goto out;
    }

    kmzFileBuf = osalHandler->Malloc(kmzFileSize);
    if (kmzFileBuf == NULL) {
        USER_LOG_ERROR("Malloc kmz file buf error.");
        goto out;
    }

    readLen = fread(kmzFileBuf, 1, kmzFileSize, kmzFile);
    if (readLen != kmzFileSize) {
        USER_LOG_ERROR("Read kmz file data failed.");
        goto out;
    }

    returnCode = DjiWaypointV3_UploadKmzFile(kmzFileBuf, kmzFileSize);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Upload kmz file failed.");
        goto out;
    }

    osalHandler->Free(kmzFileBuf);
#else
    returnCode = DjiWaypointV3_UploadKmzFile(waypoint_v3_test_file_kmz_fileBinaryArray,
                                             waypoint_v3_test_file_kmz_fileSize);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Upload kmz file failed.");
        goto out;
    }
#endif

    USER_LOG_INFO("Execute start action");
    returnCode = DjiWaypointV3_Action(DJI_WAYPOINT_V3_ACTION_START);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Execute start action failed.");
        goto out;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;

out:
    return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
}

/* Private functions definition-----------------------------------------------*/
static T_DjiReturnCode DjiTest_WaypointV3StateCallback(T_DjiWaypointV3MissionState missionState)
{
    if (s_lastWaypointV3MissionState.state == missionState.state
        && s_lastWaypointV3MissionState.currentWaypointIndex == missionState.currentWaypointIndex
        && s_lastWaypointV3MissionState.wayLineId == missionState.wayLineId) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    }

    USER_LOG_INFO("Waypoint v3 mission state: %d, current waypoint index: %d, wayLine id: %d", missionState.state,
                  missionState.currentWaypointIndex, missionState.wayLineId);

    memcpy(&s_lastWaypointV3MissionState, &missionState, sizeof(T_DjiWaypointV3MissionState));

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}
/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
