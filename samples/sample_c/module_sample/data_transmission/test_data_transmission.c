/**
 ********************************************************************
 * @file    test_data_transmission.c
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
#include "test_data_transmission.h"
#include "dji_logger.h"
#include "dji_platform.h"
#include "utils/util_misc.h"
#include "dji_low_speed_data_channel.h"
#include "dji_high_speed_data_channel.h"
#include "dji_aircraft_info.h"
#include "widget_interaction_test/test_widget_interaction.h"

/* Private constants ---------------------------------------------------------*/
#define DATA_TRANSMISSION_TASK_FREQ         (1)
#define DATA_TRANSMISSION_TASK_STACK_SIZE   (2048)

/* Private types -------------------------------------------------------------*/

/* Private functions declaration ---------------------------------------------*/
static void *UserDataTransmission_Task(void *arg);
static T_DjiReturnCode ReceiveDataFromMobile(const uint8_t *data, uint16_t len);
static T_DjiReturnCode ReceiveDataFromCloud(const uint8_t *data, uint16_t len);
static T_DjiReturnCode ReceiveDataFromExtensionPort(const uint8_t *data, uint16_t len);
static T_DjiReturnCode ReceiveDataFromPayload(const uint8_t *data, uint16_t len);
static T_DjiReturnCode ReceiveDataFromPayload1(const uint8_t *data, uint16_t len);
static T_DjiReturnCode ReceiveDataFromPayload2(const uint8_t *data, uint16_t len);
static T_DjiReturnCode ReceiveDataFromPayload3(const uint8_t *data, uint16_t len);

/* Private variables ---------------------------------------------------------*/
static T_DjiTaskHandle s_userDataTransmissionThread;
static T_DjiAircraftInfoBaseInfo s_aircraftInfoBaseInfo;

/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode DjiTest_DataTransmissionStartService(void)
{
    T_DjiReturnCode djiStat;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    E_DjiChannelAddress channelAddress;
    const T_DjiDataChannelBandwidthProportionOfHighspeedChannel bandwidthProportionOfHighspeedChannel =
        {10, 60, 30};
    char ipAddr[DJI_IP_ADDR_STR_SIZE_MAX];
    uint16_t port;

    djiStat = DjiLowSpeedDataChannel_Init();
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("init data transmission module error.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    djiStat = DjiAircraftInfo_GetBaseInfo(&s_aircraftInfoBaseInfo);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("get aircraft base info error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    channelAddress = DJI_CHANNEL_ADDRESS_MASTER_RC_APP;
    djiStat = DjiLowSpeedDataChannel_RegRecvDataCallback(channelAddress, ReceiveDataFromMobile);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("register receive data from mobile error.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

        if (s_aircraftInfoBaseInfo.aircraftType == DJI_AIRCRAFT_TYPE_M30  ||
            s_aircraftInfoBaseInfo.aircraftType == DJI_AIRCRAFT_TYPE_M30T ||
            s_aircraftInfoBaseInfo.aircraftType == DJI_AIRCRAFT_TYPE_M3D  ||
            s_aircraftInfoBaseInfo.aircraftType == DJI_AIRCRAFT_TYPE_M3TD) {
        channelAddress = DJI_CHANNEL_ADDRESS_CLOUD_API;
        djiStat = DjiLowSpeedDataChannel_RegRecvDataCallback(channelAddress, ReceiveDataFromCloud);
        if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("register receive data from cloud error.");
            return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
        }
    }

    if (s_aircraftInfoBaseInfo.mountPosition == DJI_MOUNT_POSITION_PAYLOAD_PORT_NO1 ||
        s_aircraftInfoBaseInfo.mountPosition == DJI_MOUNT_POSITION_PAYLOAD_PORT_NO2 ||
        s_aircraftInfoBaseInfo.mountPosition == DJI_MOUNT_POSITION_PAYLOAD_PORT_NO3) {
        channelAddress = DJI_CHANNEL_ADDRESS_EXTENSION_PORT;
        djiStat = DjiLowSpeedDataChannel_RegRecvDataCallback(channelAddress, ReceiveDataFromExtensionPort);
        if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("register receive data from extension port error.");
            return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
        }

        djiStat = DjiHighSpeedDataChannel_SetBandwidthProportion(bandwidthProportionOfHighspeedChannel);
        if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Set data channel bandwidth width proportion error.");
            return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
        }

        djiStat = DjiHighSpeedDataChannel_GetDataStreamRemoteAddress(ipAddr, &port);
        if (djiStat == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_DEBUG("Get data stream remote address: %s, port: %d", ipAddr, port);
        } else {
            USER_LOG_ERROR("get data stream remote address error.");
        }

    } else if (s_aircraftInfoBaseInfo.mountPosition == DJI_MOUNT_POSITION_EXTENSION_PORT
                || DJI_MOUNT_POSITION_EXTENSION_LITE_PORT == s_aircraftInfoBaseInfo.mountPosition) {
        channelAddress = DJI_CHANNEL_ADDRESS_PAYLOAD_PORT_NO1;
        djiStat = DjiLowSpeedDataChannel_RegRecvDataCallback(channelAddress, ReceiveDataFromPayload1);
        if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("register receive data from payload NO1 error.");
            return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
        }

        channelAddress = DJI_CHANNEL_ADDRESS_PAYLOAD_PORT_NO2;
        djiStat = DjiLowSpeedDataChannel_RegRecvDataCallback(channelAddress, ReceiveDataFromPayload2);
        if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("register receive data from payload NO1 error.");
            return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
        }

        channelAddress = DJI_CHANNEL_ADDRESS_PAYLOAD_PORT_NO3;
        djiStat = DjiLowSpeedDataChannel_RegRecvDataCallback(channelAddress, ReceiveDataFromPayload3);
        if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("register receive data from payload NO1 error.");
            return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
        }
    } else {
        return DJI_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT;
    }

    if (osalHandler->TaskCreate("user_transmission_task", UserDataTransmission_Task,
                                DATA_TRANSMISSION_TASK_STACK_SIZE, NULL, &s_userDataTransmissionThread) !=
        DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("user data transmission task create error.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiTest_DataTransmissionStopService(void)
{
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    T_DjiReturnCode returnCode;

    if (osalHandler->TaskDestroy(s_userDataTransmissionThread) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("user data transmission task destroy error.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    returnCode = DjiLowSpeedDataChannel_DeInit();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("deinit data transmission module error.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/* Private functions definition-----------------------------------------------*/
#ifndef __CC_ARM
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wreturn-type"
#endif

static void *UserDataTransmission_Task(void *arg)
{
    T_DjiReturnCode djiStat;
    const uint8_t dataToBeSent[] = "DJI Data Transmission Test Data.";
    T_DjiDataChannelState state = {0};
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    E_DjiChannelAddress channelAddress;

    USER_UTIL_UNUSED(arg);

    while (1) {
        osalHandler->TaskSleepMs(1000 / DATA_TRANSMISSION_TASK_FREQ);

        channelAddress = DJI_CHANNEL_ADDRESS_MASTER_RC_APP;
        djiStat = DjiLowSpeedDataChannel_SendData(channelAddress, dataToBeSent, sizeof(dataToBeSent));
        if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
            USER_LOG_ERROR("send data to mobile error.");

        djiStat = DjiLowSpeedDataChannel_GetSendDataState(channelAddress, &state);
        if (djiStat == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_DEBUG(
                "send to mobile state: realtimeBandwidthBeforeFlowController: %d, realtimeBandwidthAfterFlowController: %d, busyState: %d.",
                state.realtimeBandwidthBeforeFlowController, state.realtimeBandwidthAfterFlowController,
                state.busyState);
        } else {
            USER_LOG_ERROR("get send to mobile channel state error.");
        }

        if (s_aircraftInfoBaseInfo.aircraftType == DJI_AIRCRAFT_TYPE_M30  ||
            s_aircraftInfoBaseInfo.aircraftType == DJI_AIRCRAFT_TYPE_M30T ||
            s_aircraftInfoBaseInfo.aircraftType == DJI_AIRCRAFT_TYPE_M3D  ||
            s_aircraftInfoBaseInfo.aircraftType == DJI_AIRCRAFT_TYPE_M3TD) {
            channelAddress = DJI_CHANNEL_ADDRESS_CLOUD_API;
            djiStat = DjiLowSpeedDataChannel_SendData(channelAddress, dataToBeSent, sizeof(dataToBeSent));
            if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
                USER_LOG_ERROR("send data to cloud error.");

            djiStat = DjiLowSpeedDataChannel_GetSendDataState(channelAddress, &state);
            if (djiStat == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_DEBUG(
                    "send to cloud state: realtimeBandwidthBeforeFlowController: %d, realtimeBandwidthAfterFlowController: %d, busyState: %d.",
                    state.realtimeBandwidthBeforeFlowController, state.realtimeBandwidthAfterFlowController,
                    state.busyState);
            } else {
                USER_LOG_ERROR("get send to cloud channel state error.");
            }
        }

        if (s_aircraftInfoBaseInfo.mountPosition == DJI_MOUNT_POSITION_PAYLOAD_PORT_NO1 ||
            s_aircraftInfoBaseInfo.mountPosition == DJI_MOUNT_POSITION_PAYLOAD_PORT_NO2 ||
            s_aircraftInfoBaseInfo.mountPosition == DJI_MOUNT_POSITION_PAYLOAD_PORT_NO3) {
            channelAddress = DJI_CHANNEL_ADDRESS_EXTENSION_PORT;
            djiStat = DjiLowSpeedDataChannel_SendData(channelAddress, dataToBeSent, sizeof(dataToBeSent));
            if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
                USER_LOG_ERROR("send data to extension port error.");

            djiStat = DjiLowSpeedDataChannel_GetSendDataState(channelAddress, &state);
            if (djiStat == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_DEBUG(
                    "send to extension port state: realtimeBandwidthBeforeFlowController: %d, realtimeBandwidthAfterFlowController: %d, busyState: %d.",
                    state.realtimeBandwidthBeforeFlowController, state.realtimeBandwidthAfterFlowController,
                    state.busyState);
            } else {
                USER_LOG_ERROR("get send to extension port channel state error.");
            }

            if (DjiPlatform_GetSocketHandler() != NULL) {
#ifdef SYSTEM_ARCH_LINUX
                djiStat = DjiHighSpeedDataChannel_SendDataStreamData(dataToBeSent, sizeof(dataToBeSent));
                if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
                    USER_LOG_ERROR("send data to data stream error.");

                djiStat = DjiHighSpeedDataChannel_GetDataStreamState(&state);
                if (djiStat == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    USER_LOG_DEBUG(
                        "data stream state: realtimeBandwidthLimit: %d, realtimeBandwidthBeforeFlowController: %d, busyState: %d.",
                        state.realtimeBandwidthLimit, state.realtimeBandwidthBeforeFlowController, state.busyState);
                } else {
                    USER_LOG_ERROR("get data stream state error.");
                }
#endif
            }
        } else if (s_aircraftInfoBaseInfo.mountPosition == DJI_MOUNT_POSITION_EXTENSION_PORT
                    || DJI_MOUNT_POSITION_EXTENSION_LITE_PORT == s_aircraftInfoBaseInfo.mountPosition) {
            channelAddress = DJI_CHANNEL_ADDRESS_PAYLOAD_PORT_NO1;
            djiStat = DjiLowSpeedDataChannel_SendData(channelAddress, dataToBeSent, sizeof(dataToBeSent));
            if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
                USER_LOG_ERROR("send data to extension port error.");

            djiStat = DjiLowSpeedDataChannel_GetSendDataState(channelAddress, &state);
            if (djiStat == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_DEBUG(
                    "send to extension port state: realtimeBandwidthBeforeFlowController: %d, realtimeBandwidthAfterFlowController: %d, busyState: %d.",
                    state.realtimeBandwidthBeforeFlowController, state.realtimeBandwidthAfterFlowController,
                    state.busyState);
            } else {
                USER_LOG_ERROR("get send to extension port channel state error.");
            }
        }
    }
}

#ifndef __CC_ARM
#pragma GCC diagnostic pop
#endif

static T_DjiReturnCode ReceiveDataFromMobile(const uint8_t *data, uint16_t len)
{
    char *printData = NULL;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    printData = osalHandler->Malloc(len + 1);
    if (printData == NULL) {
        USER_LOG_ERROR("malloc memory for printData fail.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_MEMORY_ALLOC_FAILED;
    }

    strncpy(printData, (const char *) data, len);
    printData[len] = '\0';
    USER_LOG_INFO("receive data from mobile: %s, len:%d.", printData, len);
    DjiTest_WidgetLogAppend("receive data: %s, len:%d.", printData, len);

    osalHandler->Free(printData);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode ReceiveDataFromCloud(const uint8_t *data, uint16_t len)
{
    char *printData = NULL;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    printData = osalHandler->Malloc(len + 1);
    if (printData == NULL) {
        USER_LOG_ERROR("malloc memory for printData fail.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_MEMORY_ALLOC_FAILED;
    }

    strncpy(printData, (const char *) data, len);
    printData[len] = '\0';
    USER_LOG_INFO("receive data from cloud: %s, len:%d.", printData, len);
    DjiTest_WidgetLogAppend("receive data: %s, len:%d.", printData, len);

    osalHandler->Free(printData);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode ReceiveDataFromExtensionPort(const uint8_t *data, uint16_t len)
{
    char *printData = NULL;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    printData = osalHandler->Malloc(len + 1);
    if (printData == NULL) {
        USER_LOG_ERROR("malloc memory for printData fail.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_MEMORY_ALLOC_FAILED;
    }

    strncpy(printData, (const char *) data, len);
    printData[len] = '\0';
    USER_LOG_INFO("receive data from extension port: %s, len:%d.", printData, len);
    DjiTest_WidgetLogAppend("receive data: %s, len:%d.", printData, len);

    osalHandler->Free(printData);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode ReceiveDataFromPayload(const uint8_t *data, uint16_t len)
{
    char *printData = NULL;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    printData = osalHandler->Malloc(len + 1);
    if (printData == NULL) {
        USER_LOG_ERROR("malloc memory for printData fail.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_MEMORY_ALLOC_FAILED;
    }

    strncpy(printData, (const char *) data, len);
    printData[len] = '\0';
    USER_LOG_INFO("receive data from payload port: %s, len:%d.", printData, len);
    DjiTest_WidgetLogAppend("receive data: %s, len:%d.", printData, len);

    osalHandler->Free(printData);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode ReceiveDataFromPayload1(const uint8_t *data, uint16_t len)
{
    USER_LOG_INFO("Receive from payload on port 1");
    return ReceiveDataFromPayload(data, len);
}

static T_DjiReturnCode ReceiveDataFromPayload2(const uint8_t *data, uint16_t len)
{
    USER_LOG_INFO("Receive from payload on port 2");
    return ReceiveDataFromPayload(data, len);
}

static T_DjiReturnCode ReceiveDataFromPayload3(const uint8_t *data, uint16_t len)
{
    USER_LOG_INFO("Receive from payload on port 3");
    return ReceiveDataFromPayload(data, len);
}

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
