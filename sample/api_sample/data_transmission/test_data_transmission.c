/**
 ********************************************************************
 * @file    test_data_transmission.c
 * @version V2.0.0
 * @date    2019/8/3
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
#include "test_data_transmission.h"
#include "psdk_data_transmission.h"
#include "psdk_logger.h"
#include "psdk_platform.h"
#include "utils/util_misc.h"

/* Private constants ---------------------------------------------------------*/
#define DATA_TRANSMISSION_TASK_FREQ         (1)
#define DATA_TRANSMISSION_TASK_STACK_SIZE   (2048)

/* Private types -------------------------------------------------------------*/


/* Private functions declaration ---------------------------------------------*/
static void *UserDataTransmission_Task(void *arg);
static T_PsdkReturnCode ReceiveDataFromMobile(const uint8_t *data, uint16_t len);
static T_PsdkReturnCode ReceiveDataFromOnboardComputer(const uint8_t *data, uint16_t len);

/* Private variables ---------------------------------------------------------*/
static T_PsdkTaskHandle s_userDataTransmissionThread;

/* Exported functions definition ---------------------------------------------*/
T_PsdkReturnCode PsdkTest_DataTransmissionInit(void)
{
    T_PsdkReturnCode psdkStat;
    char ipAddr[PSDK_IP_ADDR_STR_SIZE_MAX] = {0};
    uint16_t port = 0;

    psdkStat = PsdkDataTransmission_Init();
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("init data transmission module error.");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    psdkStat = PsdkDataTransmission_RegReceiveDataFromMobileCallback(ReceiveDataFromMobile);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("register receive data from mobile error.");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    psdkStat = PsdkDataTransmission_RegReceiveDataFromOsdkCallback(ReceiveDataFromOnboardComputer);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("register receive data from onboard coputer error.");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    if (PsdkOsal_TaskCreate(&s_userDataTransmissionThread, UserDataTransmission_Task, "user_transmission_task",
                            DATA_TRANSMISSION_TASK_STACK_SIZE, NULL) !=
        PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("user data transmission task create error.");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    psdkStat = PsdkDataTransmission_GetDataStreamRemoteAddress(ipAddr, &port);
    if (psdkStat == PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogInfo("get data stream remote address: %s_%d", ipAddr, port);
    } else {
        PsdkLogger_UserLogError("get data stream remote address error.");
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/* Private functions definition-----------------------------------------------*/
#ifndef __CC_ARM
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wreturn-type"
#endif

static void *UserDataTransmission_Task(void *arg)
{
    T_PsdkReturnCode psdkStat;
    const uint8_t dataToBeSent[] = "PSDK Data Transmission Test Data.\r\n";
    T_PsdkDataChannelState state = {0};

    USER_UTIL_UNUSED(arg);

    while (1) {
        PsdkOsal_TaskSleepMs(1000 / DATA_TRANSMISSION_TASK_FREQ);

        psdkStat = PsdkDataTransmission_SendDataToMobile(dataToBeSent, sizeof(dataToBeSent));
        if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
            PsdkLogger_UserLogError("send data to mobile error.");

        psdkStat = PsdkDataTransmission_SendDataToOsdk(dataToBeSent, sizeof(dataToBeSent));
        if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
            PsdkLogger_UserLogError("send data to onboard computer error.");

#if PSDK_ARCH_SYS_LINUX
        psdkStat = PsdkDataTransmission_SendDataStream(dataToBeSent, sizeof(dataToBeSent));
        if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
            PsdkLogger_UserLogError("send data to data stream error.");
#endif

        psdkStat = PsdkDataTransmission_GetSendToMobileState(&state);
        if (psdkStat == PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            PsdkLogger_UserLogDebug(
                "send to mobile state: realtimeBandwidthBeforeFlowController: %d, realtimeBandwidthAfterFlowController: %d, busyState: %d.",
                state.realtimeBandwidthBeforeFlowController, state.realtimeBandwidthAfterFlowController,
                state.busyState);
        } else {
            PsdkLogger_UserLogError("get send to mobile channel state error.");
        }

        psdkStat = PsdkDataTransmission_GetSendToOsdkState(&state);
        if (psdkStat == PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            PsdkLogger_UserLogDebug(
                "send to onboard computer state: realtimeBandwidthBeforeFlowController: %d, realtimeBandwidthAfterFlowController: %d, busyState: %d.",
                state.realtimeBandwidthBeforeFlowController, state.realtimeBandwidthAfterFlowController,
                state.busyState);
        } else {
            PsdkLogger_UserLogError("get send to onboard computer channel state error.");
        }

#if PSDK_ARCH_SYS_LINUX
        psdkStat = PsdkDataTransmission_GetDataStreamState(&state);
        if (psdkStat == PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            PsdkLogger_UserLogDebug(
                "data stream state: realtimeBandwidthLimit: %d, realtimeBandwidthBeforeFlowController: %d, busyState: %d.",
                state.realtimeBandwidthLimit, state.realtimeBandwidthBeforeFlowController, state.busyState);
        } else {
            PsdkLogger_UserLogError("get data stream state error.");
        }
#endif
    }
}

#ifndef __CC_ARM
#pragma GCC diagnostic pop
#endif

static T_PsdkReturnCode ReceiveDataFromMobile(const uint8_t *data, uint16_t len)
{
    char *printData = NULL;

    printData = PsdkOsal_Malloc(len + 1);
    if (printData == NULL) {
        PsdkLogger_UserLogError("malloc memory for printData fail.");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_MEMORY_ALLOC_FAILED;
    }

    strncpy(printData, (const char *) data, len);
    printData[len] = '\0';
    PsdkLogger_UserLogInfo("receive data from mobile: %s, len:%d.", printData, len);

    PsdkOsal_Free(printData);

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode ReceiveDataFromOnboardComputer(const uint8_t *data, uint16_t len)
{
    char *printData = NULL;

    printData = PsdkOsal_Malloc(len + 1);
    if (printData == NULL) {
        PsdkLogger_UserLogError("malloc memory for printData fail.");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_MEMORY_ALLOC_FAILED;
    }

    strncpy(printData, (const char *) data, len);
    printData[len] = '\0';
    PsdkLogger_UserLogInfo("receive data from onboard computer: %s, len:%d.", printData, len);

    PsdkOsal_Free(printData);

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
