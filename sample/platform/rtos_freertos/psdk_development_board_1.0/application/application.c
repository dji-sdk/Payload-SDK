/**
 ********************************************************************
 * @file    application.c
 * @version V2.0.0
 * @date    2019/8/23
 * @brief
 *
 * @copyright (c) 2018-2019 DJI. All rights reserved.
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
#include <stdio.h>
#include <psdk_aircraft_info.h>
#include <upgrade/test_upgrade_platform_opt.h>
#include <upgrade/test_upgrade.h>
#include <upgrade_platform_opt_stm32.h>
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "uart.h"
#include "pps.h"
#include "uart_enable.h"
#include "application.h"
#include "app_info.h"
#include "psdk_config.h"
#include "psdk_core.h"
#include "psdk_logger.h"
#include "psdk_platform.h"
#include "hal/hal_uart.h"
#include "osal/osal.h"
#include "camera_emu/test_payload_cam_emu.h"
#include "positioning/test_positioning.h"
#include "power_management/test_power_management.h"
#include "psdk_power_management.h"
#include "utils/util_misc.h"
#include "data_transmission/test_data_transmission.h"
#include "data_subscription/test_data_subscription.h"
#include "widget/test_widget.h"
#include "payload_collaboration/test_payload_collaboration.h"
#include "gimbal_emu/test_payload_gimbal_emu.h"
#include "xport/test_xport.h"
#include "utils/util_misc.h"
#include "psdk_product_info.h"
#include "time_synchronization/test_time_sync.h"
#include "data_channel/test_data_channel.h"

/* Private constants ---------------------------------------------------------*/
#define PSDK_CONSOLE_UART_NUM             UART_NUM_1
#define PSDK_CONSOLE_UART_BAUD            921600

#define PSDK_COMM_WITH_LINUX_UART_NUM     UART_NUM_3
#define PSDK_COMM_WITH_LINUX_UART_BAUD    115200
#define PSDK_NETWORK_INFO_CMD_MAX_SIZE    64

#define RUN_INDICATE_TASK_FREQ_1HZ        1
#define RUN_INDICATE_TASK_FREQ_0D1HZ      0.1f

/* Private types -------------------------------------------------------------*/

/* Private values -------------------------------------------------------------*/
static char s_networkLocalIpAddress[PSDK_IP_ADDR_STR_SIZE_MAX] = {0};
static char s_networkConfigMask[PSDK_IP_ADDR_STR_SIZE_MAX] = {0};
static bool s_psdkInitFinished = false;

/* Private functions declaration ---------------------------------------------*/
static T_PsdkReturnCode PsdkUser_FillInUserInfo(T_PsdkUserInfo *userInfo);
static T_PsdkReturnCode PsdkUser_Console(const uint8_t *data, uint16_t dataLen);
static T_PsdkReturnCode PsdkUser_NetWorkConfig(const char *ipAddr, const char *netMask);

/* Exported functions definition ---------------------------------------------*/
void PsdkUser_StartTask(void const *argument)
{
    T_PsdkUserInfo userInfo;
    const T_PsdkDataChannelBandwidthProportionOfHighspeedChannel bandwidthProportionOfHighspeedChannel =
        {40, 60, 0};
    T_PsdkLoggerConsole printConsole = {
        .consoleLevel = PSDK_LOGGER_CONSOLE_LOG_LEVEL_INFO,
        .func = PsdkUser_Console,
    };

    T_PsdkHalUartHandler halUartHandler = {
        .UartInit = Hal_UartInit,
        .UartWriteData = Hal_UartSendData,
        .UartReadData = Hal_UartReadData,
    };

    T_PsdkOsalHandler osalHandler = {
        .TaskCreate = Osal_TaskCreate,
        .TaskDestroy = Osal_TaskDestroy,
        .TaskSleepMs = Osal_TaskSleepMs,
        .MutexCreate = Osal_MutexCreate,
        .MutexDestroy = Osal_MutexDestroy,
        .MutexLock = Osal_MutexLock,
        .MutexUnlock = Osal_MutexUnlock,
        .SemaphoreCreate = Osal_SemaphoreCreate,
        .SemaphoreDestroy = Osal_SemaphoreDestroy,
        .SemaphoreWait = Osal_SemaphoreWait,
        .SemaphoreTimedWait = Osal_SemaphoreTimedWait,
        .SemaphorePost = Osal_SemaphorePost,
        .GetTimeMs = Osal_GetTimeMs,
        .Malloc = Osal_Malloc,
        .Free = Osal_Free,
    };

    T_PsdkHalNetWorkHandler networkHandler = {
        .NetWorkConfig = PsdkUser_NetWorkConfig,
    };

#ifdef PSDK_USING_TIME_SYNC
    T_PsdkTestTimeSyncHandler timeSyncHandler = {
        .PpsSignalResponseInit = PsdkTest_PpsSignalResponseInit,
        .GetNewestPpsTriggerLocalTimeUs = PsdkTest_GetNewestPpsTriggerLocalTimeUs,
    };
#endif

#ifdef PSDK_USING_UPGRADE
    T_PsdkTestUpgradePlatformOpt stm32UpgradePlatformOpt = {
        .rebootSystem = PsdkUpgradePlatformStm32_RebootSystem,
        .cleanUpgradeProgramFileStoreArea = PsdkUpgradePlatformStm32_CleanUpgradeProgramFileStoreArea,
        .createUpgradeProgramFile = PsdkUpgradePlatformStm32_CreateUpgradeProgramFile,
        .writeUpgradeProgramFile = PsdkUpgradePlatformStm32_WriteUpgradeProgramFile,
        .readUpgradeProgramFile = PsdkUpgradePlatformStm32_ReadUpgradeProgramFile,
        .closeUpgradeProgramFile = PsdkUpgradePlatformStm32_CloseUpgradeProgramFile,
        .replaceOldProgram = PsdkUpgradePlatformStm32_ReplaceOldProgram,
        .setUpgradeRebootState = PsdkUpgradePlatformStm32_SetUpgradeRebootState,
        .getUpgradeRebootState = PsdkUpgradePlatformStm32_GetUpgradeRebootState,
        .cleanUpgradeRebootState = PsdkUpgradePlatformStm32_CleanUpgradeRebootState,
    };
#endif

    T_PsdkAircraftInfoBaseInfo aircraftBaseInfo = {0};

    USER_UTIL_UNUSED(argument);

    UART_Init(PSDK_CONSOLE_UART_NUM, PSDK_CONSOLE_UART_BAUD);
    UART_Init(PSDK_COMM_WITH_LINUX_UART_NUM, PSDK_COMM_WITH_LINUX_UART_BAUD);

    Uart_SwitchOn();

    if (PsdkPlatform_RegHalUartHandler(&halUartHandler) != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
        goto out;

    if (PsdkPlatform_RegOsalHandler(&osalHandler) != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
        goto out;

    if (PsdkPlatform_RegHalNetworkHandler(&networkHandler) != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
        goto out;

    if (PsdkLogger_AddConsole(&printConsole) != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
        goto out;

    if (PsdkUser_FillInUserInfo(&userInfo) != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("psdk fill in user info error");
        goto out;
    }

    if (PsdkCore_Init(&userInfo) != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("psdk instance init error");
        goto out;
    }

    if (PsdkProductInfo_SetAlias("PSDK_APPALIAS") != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("set product alias error.");
        goto out;
    }

    if (PsdkAircraftInfo_GetBaseInfo(&aircraftBaseInfo) != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("get aircraft information error.");
        goto out;
    }

    if (PsdkTest_DataChannelSetBandwidthProportionForHighspeedChannel(
        bandwidthProportionOfHighspeedChannel) != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("set bandwidth distribution for high-speed channel error");
    }

#ifdef PSDK_USING_CAMERA_EMU
    if (PsdkTest_CameraInit() != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("psdk camera init error");
    }
#endif

#ifdef PSDK_USING_DATA_TRANSMISSION
    if (PsdkTest_DataTransmissionInit() != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("psdk data transmission init error");
    }
#endif

#ifdef PSDK_USING_DATA_SUBSCRIPTION
    if (PsdkTest_DataSubscriptionInit() != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("psdk data subscription init error");
    }
#endif

#ifdef PSDK_USING_WIDGET
    if (PsdkTest_WidgetInit() != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("psdk widget init error");
    }
#endif

#ifdef PSDK_USING_TIME_SYNC
    if (aircraftBaseInfo.aircraftType == PSDK_AIRCRAFT_INFO_TYPE_M210RTK_V2 ||
        aircraftBaseInfo.aircraftType == PSDK_AIRCRAFT_INFO_TYPE_M300_RTK) {
        if (PsdkTest_TimeSyncRegHandler(&timeSyncHandler) != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            PsdkLogger_UserLogError("regsiter time sync handler error");
            goto out;
        }

        if (PsdkTest_TimeSyncInit() != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            PsdkLogger_UserLogError("psdk time sync init error");
        }
    }
#endif

#ifdef PSDK_USING_POSITIONING
    if (aircraftBaseInfo.aircraftType == PSDK_AIRCRAFT_INFO_TYPE_M210RTK_V2 ||
        aircraftBaseInfo.aircraftType == PSDK_AIRCRAFT_INFO_TYPE_M300_RTK) {
        if (PsdkTest_PositioningInit() != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            PsdkLogger_UserLogError("psdk positioning init error");
        }
    }
#endif

#ifdef PSDK_USING_PAYLOAD_COLLABORATION
    if (((aircraftBaseInfo.aircraftType == PSDK_AIRCRAFT_INFO_TYPE_M200_V2 ||
          aircraftBaseInfo.aircraftType == PSDK_AIRCRAFT_INFO_TYPE_M210_V2 ||
          aircraftBaseInfo.aircraftType == PSDK_AIRCRAFT_INFO_TYPE_M210RTK_V2) &&
         aircraftBaseInfo.payloadMountPosition == PSDK_AIRCRAFT_INFO_PAYLOAD_MOUNT_POSITION_NO2) ||
        aircraftBaseInfo.aircraftType == PSDK_AIRCRAFT_INFO_TYPE_M300_RTK) {
        if (PsdkTest_PayloadCollaborationInit() != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            PsdkLogger_UserLogError("psdk payload collaboration init error");
        }
    }
#endif

#ifdef PSDK_USING_GIMBAL_EMU
    if (aircraftBaseInfo.psdkAdapterType == PSDK_AIRCRAFT_INFO_PSDK_ADAPTER_TYPE_SKYPORT_V2) {
        if (PsdkTest_GimbalInit() != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            PsdkLogger_UserLogError("psdk gimbal init error");
        }
    }
#endif

#ifdef PSDK_USING_XPORT
    if (aircraftBaseInfo.psdkAdapterType == PSDK_AIRCRAFT_INFO_PSDK_ADAPTER_TYPE_XPORT) {
        if (PsdkTest_XPortInit() != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            PsdkLogger_UserLogError("psdk XPort init error");
        }
    }
#endif

#ifdef PSDK_USING_UPGRADE
    if (PsdkTest_UpgradeInit(&stm32UpgradePlatformOpt) != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("psdk upgrade init error");
    }
#endif

    if (PsdkCore_ApplicationStart() != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("psdk application start error");
    }

    PsdkLogger_UserLogInfo("psdk init ok");
    s_psdkInitFinished = true;

out:
    vTaskDelete(xTaskGetCurrentTaskHandle());
}

#ifndef __CC_ARM
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wreturn-type"
#endif

void PsdkUser_RunIndicateTask(void const *argument)
{
    int32_t i = 0;
    int32_t j = 0;
    static uint32_t runIndicateTaskStep = 0;
    T_UartBufferState readBufferState = {0};
    T_UartBufferState writeBufferState = {0};
    TaskStatus_t *lastTaskStatusArray = NULL;
    TaskStatus_t *currentTaskStatusArray = NULL;
    uint8_t lastTaskStatusArraySize = 0;
    uint8_t currentTaskStatusArraySize = 0;
    uint8_t cpuOccupyPercentage = 0;

    char networkInfoCmd[PSDK_NETWORK_INFO_CMD_MAX_SIZE] = {0};
    T_PsdkReturnCode returnCode;
    char ipAddr[PSDK_IP_ADDR_STR_SIZE_MAX] = {0};
    uint16_t port = 0;

    USER_UTIL_UNUSED(argument);
    USER_UTIL_UNUSED(readBufferState);
    USER_UTIL_UNUSED(writeBufferState);

    while (1) {
        Osal_TaskSleepMs(1000 / RUN_INDICATE_TASK_FREQ_1HZ);

        // report UART buffer state
#ifdef USING_UART_PORT_1
        UART_GetBufferState(UART_NUM_1, &readBufferState, &writeBufferState);
        PsdkLogger_UserLogDebug("Uart1 read buffer state: countOfLostData %d, maxUsedCapacityOfBuffer %d.",
                               readBufferState.countOfLostData, readBufferState.maxUsedCapacityOfBuffer);
        PsdkLogger_UserLogDebug("Uart1 write buffer state: countOfLostData %d, maxUsedCapacityOfBuffer %d.",
                               writeBufferState.countOfLostData, writeBufferState.maxUsedCapacityOfBuffer);
#endif

#ifdef USING_UART_PORT_2
        UART_GetBufferState(UART_NUM_2, &readBufferState, &writeBufferState);
        PsdkLogger_UserLogDebug("Uart2 read buffer state: countOfLostData %d, maxUsedCapacityOfBuffer %d.",
                               readBufferState.countOfLostData, readBufferState.maxUsedCapacityOfBuffer);
        PsdkLogger_UserLogDebug("Uart2 write buffer state: countOfLostData %d, maxUsedCapacityOfBuffer %d.",
                               writeBufferState.countOfLostData, writeBufferState.maxUsedCapacityOfBuffer);
#endif

#ifdef USING_UART_PORT_3
        UART_GetBufferState(UART_NUM_3, &readBufferState, &writeBufferState);
        PsdkLogger_UserLogDebug("Uart3 read buffer state: countOfLostData %d, maxUsedCapacityOfBuffer %d.",
                               readBufferState.countOfLostData, readBufferState.maxUsedCapacityOfBuffer);
        PsdkLogger_UserLogDebug("Uart3 write buffer state: countOfLostData %d, maxUsedCapacityOfBuffer %d.",
                               writeBufferState.countOfLostData, writeBufferState.maxUsedCapacityOfBuffer);
#endif

        // report system performance information.
        // Attention: report system performance part is not intended for normal application runtime use but as a debug aid.
        if (USER_UTIL_IS_WORK_TURN(runIndicateTaskStep++, RUN_INDICATE_TASK_FREQ_0D1HZ, RUN_INDICATE_TASK_FREQ_1HZ)) {
#if (configUSE_TRACE_FACILITY == 1)
            currentTaskStatusArraySize = uxTaskGetNumberOfTasks();
            currentTaskStatusArray = PsdkOsal_Malloc(currentTaskStatusArraySize * sizeof(TaskStatus_t));
            if (currentTaskStatusArray == NULL) {
                continue;
            }

            currentTaskStatusArraySize = uxTaskGetSystemState(currentTaskStatusArray, currentTaskStatusArraySize, NULL);
            PsdkLogger_UserLogDebug("task information:");
            PsdkLogger_UserLogDebug("task name\trun time (%%)\tstack left (byte)\tnumber");
            for (i = 0; i < currentTaskStatusArraySize; i++) {
                cpuOccupyPercentage = 0;
                for (j = 0; j < lastTaskStatusArraySize; ++j) {
                    if (currentTaskStatusArray[i].xTaskNumber == lastTaskStatusArray[j].xTaskNumber) {
                        cpuOccupyPercentage =
                            (currentTaskStatusArray[i].ulRunTimeCounter - lastTaskStatusArray[j].ulRunTimeCounter) /
                            configTICK_RATE_HZ / RUN_INDICATE_TASK_FREQ_0D1HZ;
                        break;
                    }
                }
                PsdkLogger_UserLogDebug("%s\t%u\t%u\t%u", currentTaskStatusArray[i].pcTaskName, cpuOccupyPercentage,
                                        (unsigned int) currentTaskStatusArray[i].usStackHighWaterMark *
                                        sizeof(StackType_t), (unsigned int) currentTaskStatusArray[i].xTaskNumber);
            }
            PsdkOsal_Free(lastTaskStatusArray);
            lastTaskStatusArray = currentTaskStatusArray;
            lastTaskStatusArraySize = currentTaskStatusArraySize;
#endif

            PsdkLogger_UserLogDebug("used heap size: %d.", configTOTAL_HEAP_SIZE - xPortGetFreeHeapSize());
        }
        // Attention: notify another device the network info by uart message, then the device can config network
        // and send video/data stream by these info.
        if (s_psdkInitFinished != true) {
             continue;
        }
        sprintf(networkInfoCmd, "[network_config] [ip] %s [mask] %s\r\n", s_networkLocalIpAddress,
                        s_networkConfigMask);
        UART_Write(PSDK_COMM_WITH_LINUX_UART_NUM, (uint8_t *) networkInfoCmd, strlen(networkInfoCmd));

        returnCode = PsdkPayloadCamera_GetVideoStreamRemoteAddress(ipAddr, &port);
        if (returnCode == PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                PsdkLogger_UserLogDebug("get video stream remote address:%s_%d", ipAddr, port);
        } else {
                PsdkLogger_UserLogError("get video stream remote address error.");
                continue;
        }
        sprintf(networkInfoCmd, "[video_stream] [ip] %s [port] %d\r\n", ipAddr, port);
        UART_Write(PSDK_COMM_WITH_LINUX_UART_NUM, (uint8_t *) networkInfoCmd, strlen(networkInfoCmd));

        returnCode = PsdkDataTransmission_GetDataStreamRemoteAddress(ipAddr, &port);
        if (returnCode == PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                PsdkLogger_UserLogDebug("get data stream remote address: %s_%d", ipAddr, port);
        } else {
                PsdkLogger_UserLogError("get data stream remote address error.");
                continue;
        }
        sprintf(networkInfoCmd, "[data_stream] [ip] %s [port] %d\r\n", ipAddr, port);
        UART_Write(PSDK_COMM_WITH_LINUX_UART_NUM, (uint8_t *) networkInfoCmd, strlen(networkInfoCmd));
    }
}

#ifndef __CC_ARM
#pragma GCC diagnostic pop
#endif

/* Private functions definition-----------------------------------------------*/
static T_PsdkReturnCode PsdkUser_FillInUserInfo(T_PsdkUserInfo *userInfo)
{
    memset(userInfo->appName, 0, sizeof(userInfo->appName));
    memset(userInfo->appId, 0, sizeof(userInfo->appId));
    memset(userInfo->appKey, 0, sizeof(userInfo->appKey));
    memset(userInfo->developerAccount, 0, sizeof(userInfo->developerAccount));

    if (strlen(USER_APP_NAME) >= sizeof(userInfo->appName) ||
        strlen(USER_APP_ID) > sizeof(userInfo->appId) ||
        strlen(USER_APP_KEY) > sizeof(userInfo->appKey) ||
        strlen(USER_DEVELOPER_ACCOUNT) >= sizeof(userInfo->developerAccount)) {
        PsdkLogger_UserLogError("Length of user information string is beyond limit. Please check.");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    if (!strcmp(USER_APP_NAME, "your_app_name") ||
        !strcmp(USER_APP_ID, "your_app_id") ||
        !strcmp(USER_APP_KEY, "your_app_key") ||
        !strcmp(USER_DEVELOPER_ACCOUNT, "your_developer_account")) {
        PsdkLogger_UserLogError("Please fill in correct user information to psdk_app_info.h file.");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    strncpy(userInfo->appName, USER_APP_NAME, sizeof(userInfo->appName) - 1);
    memcpy(userInfo->appId, USER_APP_ID, USER_UTIL_MIN(sizeof(userInfo->appId), strlen(USER_APP_ID)));
    memcpy(userInfo->appKey, USER_APP_KEY, USER_UTIL_MIN(sizeof(userInfo->appKey), strlen(USER_APP_KEY)));
    strncpy(userInfo->developerAccount, USER_DEVELOPER_ACCOUNT, sizeof(userInfo->developerAccount) - 1);

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode PsdkUser_Console(const uint8_t *data, uint16_t dataLen)
{
    UART_Write(PSDK_CONSOLE_UART_NUM, (uint8_t *) data, dataLen);

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode PsdkUser_NetWorkConfig(const char *ipAddr, const char *netMask)
{
    PsdkLogger_UserLogInfo("Network of payload need config to ipAddr:%s netMask:%s", ipAddr, netMask);
    strncpy(s_networkLocalIpAddress, ipAddr, sizeof(s_networkLocalIpAddress));
    strncpy(s_networkConfigMask, netMask, sizeof(s_networkLocalIpAddress));

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}


/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
