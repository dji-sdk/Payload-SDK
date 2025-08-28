/**
 ********************************************************************
 * @file    test_cloud_api_by_web_socket.c
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
#include <stdio.h>
#include <utils/util_md5.h>
#include "dji_cloud_api_by_websockt.h"
#include "dji_logger.h"
#include "dji_platform.h"
#include "test_cloud_api_by_web_socket.h"

/* Private constants ---------------------------------------------------------*/
#define TEST_MOP_CHANNEL_NORMAL_TRANSFOR_SEND_BUFFER             (64 * 1024)

/* Private types -------------------------------------------------------------*/

/* Private values -------------------------------------------------------------*/

/* Private functions declaration ---------------------------------------------*/
static T_DjiTaskHandle s_testCloudApiByWebSocketSendTask;

/* Private functions definition-----------------------------------------------*/
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wreturn-type"

static void *DjiTest_CloudApiByWebSocketSendTask(void *arg)
{
    uint8_t *sendBuf = NULL;
    uint32_t realLen = 0;
    T_DjiReturnCode returnCode;
    uint32_t sendDataCount = 0;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    USER_UTIL_UNUSED(arg);

    sendBuf = osalHandler->Malloc(TEST_MOP_CHANNEL_NORMAL_TRANSFOR_SEND_BUFFER);
    if (sendBuf == NULL) {
        USER_LOG_ERROR("cloud_api over mop malloc send buffer error");
        return NULL;
    }

    while (1) {
        sendDataCount++;
        memset(sendBuf, 'A' - 1 + (sendDataCount % 26), TEST_MOP_CHANNEL_NORMAL_TRANSFOR_SEND_BUFFER);
        returnCode = DjiCloudApi_SendDataByWebSocket(sendBuf, TEST_MOP_CHANNEL_NORMAL_TRANSFOR_SEND_BUFFER, &realLen);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_WARN("cloud_api over mop send data to channel error,stat:0x%08llX", returnCode);
        } else {
            USER_LOG_INFO("cloud_api over mop send data to channel length:%d count:%d", realLen, sendDataCount);
        }

        osalHandler->TaskSleepMs(1000);
    }
}

#pragma GCC diagnostic pop

/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode DjiTest_CloudApiByWebSocketStartService(void)
{
    T_DjiReturnCode returnCode = DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    returnCode = osalHandler->TaskCreate("CloudApiByWebSocket", DjiTest_CloudApiByWebSocketSendTask,
                                         2048, NULL, &s_testCloudApiByWebSocketSendTask);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("cloud_api over mop send task create error, stat:0x%08llX.", returnCode);
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
