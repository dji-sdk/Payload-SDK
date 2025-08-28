/**
 ********************************************************************
 * @file    application.c
 * @date    2019/8/23
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
#include <widget/test_widget_speaker.h>
#include <hms/test_hms.h>
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "led.h"
#include "pps.h"
#include "apply_high_power.h"
#include "uart.h"
#include "flash_if.h"
#include "upgrade_platform_opt_gd32.h"

#include "application.h"
#include "hal_uart.h"
#include "hal_i2c.h"
#include "osal.h"
#include "dji_sdk_app_info.h"
#include "dji_sdk_config.h"
#include "dji_core.h"
#include "dji_aircraft_info.h"
#include "dji_logger.h"

#include "utils/util_misc.h"
#include "camera_emu/test_payload_cam_emu_base.h"
#include "fc_subscription/test_fc_subscription.h"
#include "gimbal_emu/test_payload_gimbal_emu.h"
#include "xport/test_payload_xport.h"
#include "payload_collaboration/test_payload_collaboration.h"
#include "widget/test_widget.h"
#include "widget_interaction_test/test_widget_interaction.h"
#include "data_transmission/test_data_transmission.h"
#include "time_sync/test_time_sync.h"
#include "positioning/test_positioning.h"
#include "upgrade/test_upgrade.h"
#include "power_management/test_power_management.h"
#include "tethered_battery/test_tethered_battery.h"

/* Private constants ---------------------------------------------------------*/
#define RUN_INDICATE_TASK_FREQ_1HZ        1
#define RUN_INDICATE_TASK_FREQ_0D1HZ      0.1f

#define DJI_USE_WIDGET_INTERACTION        0

/* Private types -------------------------------------------------------------*/

/* Private values -------------------------------------------------------------*/
static bool s_isApplicationStart = false;

/* Private functions declaration ---------------------------------------------*/
static T_DjiReturnCode DjiUser_PrintConsole(const uint8_t *data, uint16_t dataLen);
static T_DjiReturnCode DjiUser_FillInUserInfo(T_DjiUserInfo *userInfo);

/* Exported functions definition ---------------------------------------------*/
void DjiUser_StartTask(void const *argument)
{
    T_DjiReturnCode returnCode;
    T_DjiUserInfo userInfo;
    T_DjiAircraftInfoBaseInfo aircraftInfoBaseInfo;
    T_DjiOsalHandler osalHandler = {
        .TaskCreate = Osal_TaskCreate,
        .TaskDestroy = Osal_TaskDestroy,
        .TaskSleepMs = Osal_TaskSleepMs,
        .MutexCreate= Osal_MutexCreate,
        .MutexDestroy = Osal_MutexDestroy,
        .MutexLock = Osal_MutexLock,
        .MutexUnlock = Osal_MutexUnlock,
        .SemaphoreCreate = Osal_SemaphoreCreate,
        .SemaphoreDestroy = Osal_SemaphoreDestroy,
        .SemaphoreWait = Osal_SemaphoreWait,
        .SemaphoreTimedWait = Osal_SemaphoreTimedWait,
        .SemaphorePost = Osal_SemaphorePost,
        .Malloc = Osal_Malloc,
        .Free = Osal_Free,
        .GetTimeMs = Osal_GetTimeMs,
        .GetTimeUs = Osal_GetTimeUs,
        .GetRandomNum = Osal_GetRandomNum,
    };
    T_DjiLoggerConsole printConsole = {
        .func = DjiUser_PrintConsole,
        .consoleLevel = DJI_LOGGER_CONSOLE_LOG_LEVEL_INFO,
        .isSupportColor = true,
    };
    T_DjiHalUartHandler uartHandler = {
        .UartInit = HalUart_Init,
        .UartDeInit = HalUart_DeInit,
        .UartWriteData = HalUart_WriteData,
        .UartReadData = HalUart_ReadData,
        .UartGetStatus = HalUart_GetStatus,
        .UartGetDeviceInfo = HalUart_GetDeviceInfo,
    };
    T_DjiHalI2cHandler i2CHandler = {
        .I2cInit = HalI2c_Init,
        .I2cDeInit = HalI2c_DeInit,
        .I2cWriteData = HalI2c_WriteData,
        .I2cReadData = HalI2c_ReadData,
    };
    T_DjiFirmwareVersion firmwareVersion = {
        .majorVersion = USER_FIRMWARE_MAJOR_VERSION,
        .minorVersion = USER_FIRMWARE_MINOR_VERSION,
        .modifyVersion = USER_FIRMWARE_MODIFY_VERSION,
        .debugVersion = USER_FIRMWARE_DEBUG_VERSION,
    };

    returnCode = DjiPlatform_RegOsalHandler(&osalHandler);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        printf("register osal handler error");
        goto out;
    }

    returnCode = DjiPlatform_RegHalUartHandler(&uartHandler);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        printf("register hal uart handler error");
        goto out;
    }

    USER_LOG_INFO("Register hal i2c handler.");
    returnCode = DjiPlatform_RegHalI2cHandler(&i2CHandler);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        printf("register hal i2c handler error");
        goto out;
    }

    returnCode = DjiLogger_AddConsole(&printConsole);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        printf("add printf console error");
        goto out;
    }

    returnCode = DjiUser_FillInUserInfo(&userInfo);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("fill user info error, please check user info config");
        goto out;
    }


    returnCode = DjiCore_SetFirmwareVersion(firmwareVersion);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("set firmware version error");
        goto out;
    }

    returnCode = DjiCore_SetSerialNumber("PSDK12345678XX");
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("set serial number error");
        goto out;
    }

    returnCode = DjiCore_Init(&userInfo);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        osalHandler.TaskSleepMs(200);
        USER_LOG_ERROR("core init error");
        goto out;
    }

    returnCode = DjiAircraftInfo_GetBaseInfo(&aircraftInfoBaseInfo);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("get aircraft base info error");
        goto out;
    }

    returnCode = DjiCore_SetAlias("PSDK_APPALIAS");
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("set alias error");
        goto out;
    }


#ifdef CONFIG_MODULE_SAMPLE_POWER_MANAGEMENT_ON
    T_DjiTestApplyHighPowerHandler applyHighPowerHandler = {
        .pinInit = DjiTest_HighPowerApplyPinInit,
        .pinWrite = DjiTest_WriteHighPowerApplyPin,
    };

    returnCode = DjiTest_RegApplyHighPowerHandler(&applyHighPowerHandler);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("register apply high power handler error");
    }

    returnCode = DjiTest_PowerManagementStartService();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("power management init error");
    }
#endif

#ifdef CONFIG_MODULE_SAMPLE_WIDGET_ON
    returnCode = DjiTest_WidgetStartService();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("widget sample init error");
    }
#endif

#ifdef CONFIG_MODULE_SAMPLE_WIDGET_SPEAKER_ON
    returnCode = DjiTest_WidgetSpeakerStartService();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("widget speaker sample init error");
    }
#endif

#ifdef CONFIG_MODULE_SAMPLE_DATA_TRANSMISSION_ON
    returnCode = DjiTest_DataTransmissionStartService();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("widget sample init error");
    }
#endif

#ifdef CONFIG_MODULE_SAMPLE_FC_SUBSCRIPTION_ON
    returnCode = DjiTest_FcSubscriptionStartService();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("data subscription sample init error\n");
    }
#endif

#ifdef CONFIG_MODULE_SAMPLE_HMS_CUSTOMIZATION_ON
    returnCode = DjiTest_HmsCustomizationStartService();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("hms test init error");
    }
#endif

#ifdef CONFIG_MODULE_SAMPLE_CAMERA_ON
    if (aircraftInfoBaseInfo.aircraftType == DJI_AIRCRAFT_TYPE_M300_RTK
        && aircraftInfoBaseInfo.djiAdapterType == DJI_SDK_ADAPTER_TYPE_NONE) {
        USER_LOG_WARN("Not support camera emu sample.");
    } else {
        returnCode = DjiTest_CameraEmuBaseStartService();
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("camera emu common init error");
        }
    }
#endif

#ifdef CONFIG_MODULE_SAMPLE_GIMBAL_EMU_ON
    if ((aircraftInfoBaseInfo.aircraftType == DJI_AIRCRAFT_TYPE_M300_RTK ||
         aircraftInfoBaseInfo.aircraftType == DJI_AIRCRAFT_TYPE_M350_RTK)
        && aircraftInfoBaseInfo.djiAdapterType == DJI_SDK_ADAPTER_TYPE_NONE) {
        USER_LOG_WARN("Not support gimbal emu sample.");
    } else {
        if (aircraftInfoBaseInfo.djiAdapterType == DJI_SDK_ADAPTER_TYPE_SKYPORT_V2 ||
            aircraftInfoBaseInfo.djiAdapterType == DJI_SDK_ADAPTER_TYPE_EPORT_V2_RIBBON_CABLE ||
            aircraftInfoBaseInfo.djiAdapterType == DJI_SDK_ADAPTER_TYPE_SKYPORT_V3 ||
            aircraftInfoBaseInfo.djiAdapterType == DJI_SDK_ADAPTER_TYPE_NONE) {
            if (DjiTest_GimbalStartService() != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("psdk gimbal init error");
            }
        }
    }
#endif

#ifdef CONFIG_MODULE_SAMPLE_XPORT_ON
    if (aircraftInfoBaseInfo.djiAdapterType == DJI_SDK_ADAPTER_TYPE_XPORT) {
        if (DjiTest_XPortStartService() != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("psdk xport init error");
        }
    }
#endif

#ifdef CONFIG_MODULE_SAMPLE_PAYLOAD_COLLABORATION_ON
    if (aircraftInfoBaseInfo.mountPosition != DJI_MOUNT_POSITION_EXTENSION_PORT) {
        returnCode = DjiTest_PayloadCollaborationStartService();
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Payload collaboration sample init error\n");
        }
    }
#endif

#ifdef CONFIG_MODULE_SAMPLE_TIME_SYNC_ON
    T_DjiTestTimeSyncHandler testTimeSyncHandler = {
        .PpsSignalResponseInit = DjiTest_PpsSignalResponseInit,
        .GetNewestPpsTriggerLocalTimeUs = DjiTest_GetNewestPpsTriggerLocalTimeUs,
    };

    if (DjiTest_TimeSyncRegHandler(&testTimeSyncHandler) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("regsiter time sync handler error");
        goto out;
    }

    if (DjiTest_TimeSyncStartService() != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("psdk time sync init error");
    }
#endif

#ifdef CONFIG_MODULE_SAMPLE_POSITIONING_ON
        if (DjiTest_PositioningStartService() != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("psdk positioning init error");
        }
#endif

#ifdef CONFIG_MODULE_SAMPLE_UPGRADE_ON
    T_DjiTestUpgradePlatformOpt Gd32UpgradePlatformOpt = {
        .rebootSystem = DjiUpgradePlatformGd32_RebootSystem,
        .cleanUpgradeProgramFileStoreArea = DjiUpgradePlatformGd32_CleanUpgradeProgramFileStoreArea,
        .createUpgradeProgramFile = DjiUpgradePlatformGd32_CreateUpgradeProgramFile,
        .writeUpgradeProgramFile = DjiUpgradePlatformGd32_WriteUpgradeProgramFile,
        .readUpgradeProgramFile = DjiUpgradePlatformGd32_ReadUpgradeProgramFile,
        .closeUpgradeProgramFile = DjiUpgradePlatformGd32_CloseUpgradeProgramFile,
        .replaceOldProgram = DjiUpgradePlatformGd32_ReplaceOldProgram,
        .setUpgradeRebootState = DjiUpgradePlatformGd32_SetUpgradeRebootState,
        .getUpgradeRebootState = DjiUpgradePlatformGd32_GetUpgradeRebootState,
        .cleanUpgradeRebootState = DjiUpgradePlatformGd32_CleanUpgradeRebootState,
    };
    T_DjiTestUpgradeConfig testUpgradeConfig = {
        .firmwareVersion = {
            USER_FIRMWARE_MAJOR_VERSION,
            USER_FIRMWARE_MINOR_VERSION,
            USER_FIRMWARE_MODIFY_VERSION,
            USER_FIRMWARE_DEBUG_VERSION
        },
        .transferType = DJI_FIRMWARE_TRANSFER_TYPE_DCFTP,
        .needReplaceProgramBeforeReboot = false
    };
    if (DjiTest_UpgradeStartService(&Gd32UpgradePlatformOpt, testUpgradeConfig) !=
        DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        printf("psdk upgrade init error");
    }
#endif

#ifdef CONFIG_MODULE_SAMPLE_TETHERED_BATTERY_ON
    if (aircraftInfoBaseInfo.djiAdapterType == DJI_SDK_ADAPTER_TYPE_EPORT_V2_RIBBON_CABLE) {
        if (DjiTest_TetheredBatteryStartService() != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("psdk tethered battery sample init error");
        }
    }
#endif

    returnCode = DjiCore_ApplicationStart();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("start sdk application error");
    }

    s_isApplicationStart = true;

#ifdef CONFIG_MODULE_SAMPLE_FC_SUBSCRIPTION_ON
    returnCode = DjiTest_FcSubscriptionRunSample();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Fc subcription run sample failed, 0x%08X", returnCode);
    }
#endif

    while (1) {
        Osal_TaskSleepMs(500);
        Led_Trigger(LED_GREEN);
    }

out:
    while (1) {
        USER_LOG_ERROR("Eport 2.0 Development Board sample init failed.");
        Osal_TaskSleepMs(250);
        Led_Trigger(LED_RED);
    }
    vTaskDelete(xTaskGetCurrentTaskHandle());
}

#ifndef __CC_ARM
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wreturn-type"
#endif

void DjiUser_MonitorTask(void const *argument)
{
   static uint32_t runIndicateTaskStep = 0;
   T_UartBufferState readBufferState = {0};
   T_UartBufferState writeBufferState = {0};
   T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
#if (configUSE_TRACE_FACILITY == 1)
   int32_t i = 0;
   int32_t j = 0;
   TaskStatus_t *lastTaskStatusArray = NULL;
   TaskStatus_t *currentTaskStatusArray = NULL;
   uint8_t lastTaskStatusArraySize = 0;
   uint8_t currentTaskStatusArraySize;
   uint8_t cpuOccupyPercentage;
#endif

   while (1) {
       Osal_TaskSleepMs(1000 / RUN_INDICATE_TASK_FREQ_0D1HZ);

       if (s_isApplicationStart == false) {
           continue;
       }
       // report UART buffer state
#ifdef USING_UART_PORT_1
       UART_GetBufferState(UART_NUM_1, &readBufferState, &writeBufferState);
       USER_LOG_DEBUG("Uart1 read buffer state: countOfLostData %d, maxUsedCapacityOfBuffer %d.",
                      readBufferState.countOfLostData, readBufferState.maxUsedCapacityOfBuffer);
       USER_LOG_DEBUG("Uart1 write buffer state: countOfLostData %d, maxUsedCapacityOfBuffer %d.",
                      writeBufferState.countOfLostData, writeBufferState.maxUsedCapacityOfBuffer);
#endif

#ifdef USING_UART_PORT_2
       UART_GetBufferState(UART_NUM_2, &readBufferState, &writeBufferState);
       USER_LOG_DEBUG("Uart2 read buffer state: countOfLostData %d, maxUsedCapacityOfBuffer %d.",
                      readBufferState.countOfLostData, readBufferState.maxUsedCapacityOfBuffer);
       USER_LOG_DEBUG("Uart2 write buffer state: countOfLostData %d, maxUsedCapacityOfBuffer %d.",
                      writeBufferState.countOfLostData, writeBufferState.maxUsedCapacityOfBuffer);
#endif

#ifdef USING_UART_PORT_3
       UART_GetBufferState(UART_NUM_3, &readBufferState, &writeBufferState);
       USER_LOG_DEBUG("Uart3 read buffer state: countOfLostData %d, maxUsedCapacityOfBuffer %d.",
                      readBufferState.countOfLostData, readBufferState.maxUsedCapacityOfBuffer);
       USER_LOG_DEBUG("Uart3 write buffer state: countOfLostData %d, maxUsedCapacityOfBuffer %d.",
                      writeBufferState.countOfLostData, writeBufferState.maxUsedCapacityOfBuffer);
#endif

       // report system performance information.
       // Attention: report system performance part is not intended for normal application runtime use but as a debug aid.
       if (USER_UTIL_IS_WORK_TURN(runIndicateTaskStep++, RUN_INDICATE_TASK_FREQ_0D1HZ,
                                  RUN_INDICATE_TASK_FREQ_1HZ)) {
#if (configUSE_TRACE_FACILITY == 1)
           currentTaskStatusArraySize = uxTaskGetNumberOfTasks();
           currentTaskStatusArray = osalHandler->Malloc(currentTaskStatusArraySize * sizeof(TaskStatus_t));
           if (currentTaskStatusArray == NULL) {
               continue;
           }

           currentTaskStatusArraySize = uxTaskGetSystemState(currentTaskStatusArray, currentTaskStatusArraySize, NULL);
           USER_LOG_DEBUG("task information:");
           USER_LOG_DEBUG("task name\trun time (%%)\tstack left (byte)\tnumber");
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
               USER_LOG_DEBUG("%-16s\t%u\t%u\t%u", currentTaskStatusArray[i].pcTaskName, cpuOccupyPercentage,
                              (unsigned int) currentTaskStatusArray[i].usStackHighWaterMark * sizeof(StackType_t),
                              (unsigned int) currentTaskStatusArray[i].xTaskNumber);
           }
           osalHandler->Free(lastTaskStatusArray);
           lastTaskStatusArray = currentTaskStatusArray;
           lastTaskStatusArraySize = currentTaskStatusArraySize;
#endif
       }
       USER_LOG_INFO("Used heap size: %d/%d.\r\n", configTOTAL_HEAP_SIZE - xPortGetFreeHeapSize(),
                     configTOTAL_HEAP_SIZE);
   }
}

#ifndef __CC_ARM
#pragma GCC diagnostic pop
#endif

/* Private functions definition-----------------------------------------------*/
static T_DjiReturnCode DjiUser_FillInUserInfo(T_DjiUserInfo *userInfo)
{
    memset(userInfo->appName, 0, sizeof(userInfo->appName));
    memset(userInfo->appId, 0, sizeof(userInfo->appId));
    memset(userInfo->appKey, 0, sizeof(userInfo->appKey));
    memset(userInfo->appLicense, 0, sizeof(userInfo->appLicense));
    memset(userInfo->developerAccount, 0, sizeof(userInfo->developerAccount));
    memset(userInfo->baudRate, 0, sizeof(userInfo->baudRate));

    if (strlen(USER_APP_NAME) >= sizeof(userInfo->appName) ||
        strlen(USER_APP_ID) > sizeof(userInfo->appId) ||
        strlen(USER_APP_KEY) > sizeof(userInfo->appKey) ||
        strlen(USER_APP_LICENSE) > sizeof(userInfo->appLicense) ||
        strlen(USER_DEVELOPER_ACCOUNT) >= sizeof(userInfo->developerAccount) ||
        strlen(USER_BAUD_RATE) > sizeof(userInfo->baudRate)) {
        USER_LOG_ERROR("Length of user information string is beyond limit. Please check.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    if (!strcmp(USER_APP_NAME, "your_app_name") ||
        !strcmp(USER_APP_ID, "your_app_id") ||
        !strcmp(USER_APP_KEY, "your_app_key") ||
        !strcmp(USER_APP_LICENSE, "your_app_license") ||
        !strcmp(USER_DEVELOPER_ACCOUNT, "your_developer_account") ||
        !strcmp(USER_BAUD_RATE, "your_baud_rate")) {
        USER_LOG_ERROR(
            "Please fill in correct user information to 'samples/sample_c/platform/rtos_freertos/Gd32f4_discovery/application/dji_sdk_app_info.h' file.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    strncpy(userInfo->appName, USER_APP_NAME, sizeof(userInfo->appName) - 1);
    memcpy(userInfo->appId, USER_APP_ID, USER_UTIL_MIN(sizeof(userInfo->appId), strlen(USER_APP_ID)));
    memcpy(userInfo->appKey, USER_APP_KEY, USER_UTIL_MIN(sizeof(userInfo->appKey), strlen(USER_APP_KEY)));
    memcpy(userInfo->appLicense, USER_APP_LICENSE,
           USER_UTIL_MIN(sizeof(userInfo->appLicense), strlen(USER_APP_LICENSE)));
    memcpy(userInfo->baudRate, USER_BAUD_RATE, USER_UTIL_MIN(sizeof(userInfo->baudRate), strlen(USER_BAUD_RATE)));
    strncpy(userInfo->developerAccount, USER_DEVELOPER_ACCOUNT, sizeof(userInfo->developerAccount) - 1);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode DjiUser_PrintConsole(const uint8_t *data, uint16_t dataLen)
{
    UART_Write(DJI_CONSOLE_UART_NUM, (uint8_t *) data, dataLen);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

#ifdef __cplusplus
}
#endif
/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
