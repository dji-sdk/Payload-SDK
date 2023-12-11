/**
 ********************************************************************
 * @file    test_liveview.c
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
#include <utils/util_misc.h>
#include <widget_interaction_test/test_widget_interaction.h>
#include "test_liveview.h"
#include "dji_liveview.h"
#include "dji_logger.h"
#include "dji_platform.h"
#include "dji_aircraft_info.h"
#include "time.h"

/* Private constants ---------------------------------------------------------*/
#define TEST_LIVEVIEW_STREAM_FILE_PATH_STR_MAX_SIZE             256
#define TEST_LIVEVIEW_STREAM_STROING_TIME_IN_SECONDS            20

#define TEST_LIVEVIEW_STREAM_REQUEST_I_FRAME_ON                 1
#define TEST_LIVEVIEW_STREAM_REQUEST_I_FRAME_TICK_IN_SECONDS    5

/* Private types -------------------------------------------------------------*/

/* Private values -------------------------------------------------------------*/
static char s_fpvCameraStreamFilePath[TEST_LIVEVIEW_STREAM_FILE_PATH_STR_MAX_SIZE];
static char s_payloadCameraStreamFilePath[TEST_LIVEVIEW_STREAM_FILE_PATH_STR_MAX_SIZE];

/* Private functions declaration ---------------------------------------------*/
static void DjiTest_FpvCameraStreamCallback(E_DjiLiveViewCameraPosition position, const uint8_t *buf,
                                            uint32_t bufLen);
static void DjiTest_PayloadCameraStreamCallback(E_DjiLiveViewCameraPosition position, const uint8_t *buf,
                                                uint32_t bufLen);

/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode DjiTest_LiveviewRunSample(E_DjiMountPosition mountPosition)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    time_t currentTime = time(NULL);
    struct tm *localTime = NULL;
    T_DjiAircraftInfoBaseInfo aircraftInfoBaseInfo = {0};

    USER_LOG_INFO("Liveview sample start");
    DjiTest_WidgetLogAppend("Liveview sample start");

    returnCode = DjiAircraftInfo_GetBaseInfo(&aircraftInfoBaseInfo);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("get aircraft base info error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    USER_LOG_INFO("--> Step 1: Init liveview module");
    DjiTest_WidgetLogAppend("--> Step 1: Init liveview module");
    returnCode = DjiLiveview_Init();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Liveview init failed, error code: 0x%08X", returnCode);
        goto out;
    }

    USER_LOG_INFO("--> Step 2: Start h264 stream of the fpv and selected payload\r\n");
    DjiTest_WidgetLogAppend("--> Step 2: Start h264 stream of the fpv and selected payload\r\n");

    if (aircraftInfoBaseInfo.aircraftSeries == DJI_AIRCRAFT_SERIES_M300 ||
        aircraftInfoBaseInfo.aircraftSeries == DJI_AIRCRAFT_SERIES_M350 ||
        aircraftInfoBaseInfo.aircraftSeries == DJI_AIRCRAFT_SERIES_M30) {

        localTime = localtime(&currentTime);
        sprintf(s_fpvCameraStreamFilePath, "fpv_stream_%04d%02d%02d_%02d-%02d-%02d.h264",
                localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday,
                localTime->tm_hour, localTime->tm_min, localTime->tm_sec);

        returnCode = DjiLiveview_StartH264Stream(DJI_LIVEVIEW_CAMERA_POSITION_FPV, DJI_LIVEVIEW_CAMERA_SOURCE_DEFAULT,
                                                 DjiTest_FpvCameraStreamCallback);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Request h264 of fpv failed, error code: 0x%08X", returnCode);
            goto out;
        }
    }

    localTime = localtime(&currentTime);
    sprintf(s_payloadCameraStreamFilePath, "payload%d_vis_stream_%04d%02d%02d_%02d-%02d-%02d.h264",
            mountPosition, localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday,
            localTime->tm_hour, localTime->tm_min, localTime->tm_sec);

    returnCode = DjiLiveview_StartH264Stream((E_DjiLiveViewCameraPosition) mountPosition,
                                             DJI_LIVEVIEW_CAMERA_SOURCE_DEFAULT,
                                             DjiTest_PayloadCameraStreamCallback);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Request h264 of payload %d failed, error code: 0x%08X", mountPosition, returnCode);
    }

    for (int i = 0; i < TEST_LIVEVIEW_STREAM_STROING_TIME_IN_SECONDS; ++i) {
        USER_LOG_INFO("Storing camera h264 stream, second: %d.", i + 1);
#if TEST_LIVEVIEW_STREAM_REQUEST_I_FRAME_ON
        if (i % TEST_LIVEVIEW_STREAM_REQUEST_I_FRAME_TICK_IN_SECONDS == 0) {
            returnCode = DjiLiveview_RequestIntraframeFrameData((E_DjiLiveViewCameraPosition) mountPosition,
                                                                DJI_LIVEVIEW_CAMERA_SOURCE_DEFAULT);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Request stream I frame of payload %d failed, error code: 0x%08X", mountPosition,
                               returnCode);
            }
        }
#endif
        osalHandler->TaskSleepMs(1000);
    }

    USER_LOG_INFO("--> Step 3: Stop h264 stream of the fpv and selected payload\r\n");
    DjiTest_WidgetLogAppend("--> Step 3: Stop h264 stream of the fpv and selected payload");
    if (aircraftInfoBaseInfo.aircraftSeries == DJI_AIRCRAFT_SERIES_M300 ||
        aircraftInfoBaseInfo.aircraftSeries == DJI_AIRCRAFT_SERIES_M350 ||
        aircraftInfoBaseInfo.aircraftSeries == DJI_AIRCRAFT_SERIES_M30) {
        returnCode = DjiLiveview_StopH264Stream(DJI_LIVEVIEW_CAMERA_POSITION_FPV, DJI_LIVEVIEW_CAMERA_SOURCE_DEFAULT);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Request to stop h264 of fpv failed, error code: 0x%08X", returnCode);
            goto out;
        }
    }

    returnCode = DjiLiveview_StopH264Stream((E_DjiLiveViewCameraPosition) mountPosition,
                                            DJI_LIVEVIEW_CAMERA_SOURCE_DEFAULT);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Request to stop h264 of payload %d failed, error code: 0x%08X", mountPosition, returnCode);
        goto out;
    }

    USER_LOG_INFO("Fpv stream is saved to file: %s", s_fpvCameraStreamFilePath);
    USER_LOG_INFO("Payload%d stream is saved to file: %s\r\n", mountPosition, s_payloadCameraStreamFilePath);

    if (aircraftInfoBaseInfo.aircraftType == DJI_AIRCRAFT_TYPE_M3T ||
        aircraftInfoBaseInfo.aircraftType == DJI_AIRCRAFT_TYPE_M3TD) {
        USER_LOG_INFO("--> Start h264 stream of the fpv and selected payload\r\n");

        localTime = localtime(&currentTime);
        sprintf(s_payloadCameraStreamFilePath, "payload%d_ir_stream_%04d%02d%02d_%02d-%02d-%02d.h264",
                mountPosition, localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday,
                localTime->tm_hour, localTime->tm_min, localTime->tm_sec);

        returnCode = DjiLiveview_StartH264Stream((E_DjiLiveViewCameraPosition) mountPosition,
                                                 DJI_LIVEVIEW_CAMERA_SOURCE_M3T_IR,
                                                 DjiTest_PayloadCameraStreamCallback);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Request h264 of payload %d failed, error code: 0x%08X", mountPosition, returnCode);
        }

        for (int i = 0; i < TEST_LIVEVIEW_STREAM_STROING_TIME_IN_SECONDS; ++i) {
            USER_LOG_INFO("Storing camera h264 stream, second: %d.", i + 1);
            osalHandler->TaskSleepMs(1000);
        }

        returnCode = DjiLiveview_StopH264Stream((E_DjiLiveViewCameraPosition) mountPosition,
                                                DJI_LIVEVIEW_CAMERA_SOURCE_M3T_IR);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Request to stop h264 of payload %d failed, error code: 0x%08X", mountPosition, returnCode);
            goto out;
        }
    }

    USER_LOG_INFO("Fpv stream is saved to file: %s", s_fpvCameraStreamFilePath);
    USER_LOG_INFO("Payload%d stream is saved to file: %s\r\n", mountPosition, s_payloadCameraStreamFilePath);

    USER_LOG_INFO("--> Step 4: Deinit liveview module");
    DjiTest_WidgetLogAppend("--> Step 4: Deinit liveview module");
    returnCode = DjiLiveview_Deinit();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Liveview deinit failed, error code: 0x%08X", returnCode);
        goto out;
    }

out:
    USER_LOG_INFO("Liveview sample end");

    return returnCode;
}

/* Private functions definition-----------------------------------------------*/
static void DjiTest_FpvCameraStreamCallback(E_DjiLiveViewCameraPosition position, const uint8_t *buf,
                                            uint32_t bufLen)
{
    FILE *fp = NULL;
    size_t size;

    fp = fopen(s_fpvCameraStreamFilePath, "ab+");
    if (fp == NULL) {
        printf("fopen failed!\n");
        return;
    }

    size = fwrite(buf, 1, bufLen, fp);
    if (size != bufLen) {
        fclose(fp);
        return;
    }

    fflush(fp);
    fclose(fp);
}

static void DjiTest_PayloadCameraStreamCallback(E_DjiLiveViewCameraPosition position, const uint8_t *buf,
                                                uint32_t bufLen)
{
    FILE *fp = NULL;
    size_t size;

    fp = fopen(s_payloadCameraStreamFilePath, "ab+");
    if (fp == NULL) {
        printf("fopen failed!\n");
        return;
    }

    size = fwrite(buf, 1, bufLen, fp);
    if (size != bufLen) {
        fclose(fp);
        return;
    }

    fflush(fp);
    fclose(fp);
}

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
