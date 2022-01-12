/**
 ********************************************************************
 * @file    main.c
 * @version V2.0.0
 * @date    2019/07/01
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
#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include "errno.h"
#include <signal.h>

#include "psdk_logger.h"
#include "psdk_core.h"
#include "psdk_platform.h"

#include "osal/osal.h"
#include "utils/util_misc.h"
#include "hal/hal_uart.h"
#include "hal/hal_network.h"
#include "psdk_config.h"
#include "app_info.h"
#include "psdk_aircraft_info.h"
#include "camera_emu/test_payload_cam_emu.h"
#include "data_transmission/test_data_transmission.h"
#include "data_subscription/test_data_subscription.h"
#include "payload_collaboration/test_payload_collaboration.h"
#include "camera_media_emu/test_payload_cam_media.h"
#include "widget/test_widget.h"
#include "gimbal_emu/test_payload_gimbal_emu.h"
#include "psdk_data_channel.h"
#include "data_channel/test_data_channel.h"
#include "power_management/test_power_management.h"
#include "xport/test_xport.h"
#include "monitor/sys_monitor.h"
#include "psdk_product_info.h"
#include "mop_channel/test_mop_channel.h"
#include "upgrade/test_upgrade.h"
#include "upgrade_platform_opt/upgrade_platform_opt_linux.h"
#include <psdk_payload_camera.h>

/* Private constants ---------------------------------------------------------*/
#define PSDK_LOG_PATH                    "Logs/psdk_local"
#define PSDK_LOG_INDEX_FILE_NAME         "Logs/latest"
#define PSDK_LOG_FOLDER_NAME             "Logs"
#define PSDK_LOG_PATH_MAX_SIZE           (128)
#define PSDK_LOG_FOLDER_NAME_MAX_SIZE    (32)

/* Private types -------------------------------------------------------------*/
typedef struct {
    pid_t tid;
    char name[16];
    float pcpu;
} T_ThreadAttribute;

/* Private functions declaration ---------------------------------------------*/
static T_PsdkReturnCode PsdkUser_FillInUserInfo(T_PsdkUserInfo *userInfo);
static T_PsdkReturnCode PsdkUser_Console(const uint8_t *data, uint16_t dataLen);
static T_PsdkReturnCode PsdkUser_LocalWrite(const uint8_t *data, uint16_t dataLen);
static T_PsdkReturnCode PsdkUser_FileSystemInit(const char *path);
static void *PsdkUser_MonitorTask(void *argument);

/* Exported functions definition ---------------------------------------------*/
static FILE *s_psdkLogFile;
static FILE *s_psdkLogFileCnt;
static pthread_t s_monitorThread = 0;

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
    USER_UTIL_UNUSED(dataLen);

    /*
     * Warning: printf() is a blocking interface. If print speed is too low, PSDK will be blocked and many abnormal
     * situations  occur, such as initialization failure and payload disconnection. Users can raise corresponding
     * console level to INFO or even higher to solve this problem.
     */
    printf("%s", data);

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode PsdkUser_LocalWrite(const uint8_t *data, uint16_t dataLen)
{
    int32_t realLen;

    if (s_psdkLogFile == NULL) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    realLen = fwrite(data, 1, dataLen, s_psdkLogFile);
    fflush(s_psdkLogFile);
    if (realLen == dataLen) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    } else {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }
}

static T_PsdkReturnCode PsdkUser_FileSystemInit(const char *path)
{
    T_PsdkReturnCode psdkStat = PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    char filePath[PSDK_LOG_PATH_MAX_SIZE];
    char folderName[PSDK_LOG_FOLDER_NAME_MAX_SIZE];
    time_t currentTime = time(NULL);
    struct tm *localTime = localtime(&currentTime);
    uint16_t logFileIndex = 0;
    uint16_t currentLogFileIndex = 0;
    uint8_t ret = 0;

    if (localTime == NULL) {
        printf("Get local time error.\r\n");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    if (access(PSDK_LOG_FOLDER_NAME, F_OK) != 0) {
        printf("Log file is not existed, need create new.\r\n");
        sprintf(folderName, "mkdir %s", PSDK_LOG_FOLDER_NAME);
        ret = system(folderName);
        if (ret != 0) {
            printf("Create new log folder error, ret:%d.\r\n", ret);
            return PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
        }
    }

    s_psdkLogFileCnt = fopen(PSDK_LOG_INDEX_FILE_NAME, "rb+");
    if (s_psdkLogFileCnt == NULL) {
        s_psdkLogFileCnt = fopen(PSDK_LOG_INDEX_FILE_NAME, "wb+");
        if (s_psdkLogFileCnt == NULL) {
            return PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
        }
    } else {
        ret = fseek(s_psdkLogFileCnt, 0, SEEK_SET);
        if (ret != 0) {
            printf("Seek log count file error, ret: %d, errno: %d.\r\n", ret, errno);
            return PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
        }

        ret = fread((uint16_t *) &logFileIndex, 1, sizeof(uint16_t), s_psdkLogFileCnt);
        if (ret != sizeof(uint16_t)) {
            printf("Read log file index error.\r\n");
        }
    }

    printf("Get current log index: %d\r\n", logFileIndex);
    currentLogFileIndex = logFileIndex;
    logFileIndex++;

    ret = fseek(s_psdkLogFileCnt, 0, SEEK_SET);
    if (ret != 0) {
        printf("Seek log file error, ret: %d, errno: %d.\r\n", ret, errno);
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    ret = fwrite((uint16_t *) &logFileIndex, 1, sizeof(uint16_t), s_psdkLogFileCnt);
    if (ret != sizeof(uint16_t)) {
        printf("Write log file index error.\r\n");
        fclose(s_psdkLogFileCnt);
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    fclose(s_psdkLogFileCnt);

    sprintf(filePath, "%s_%04d_%04d%02d%02d_%02d-%02d-%02d.log", path, currentLogFileIndex,
            localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday,
            localTime->tm_hour, localTime->tm_min, localTime->tm_sec);

    s_psdkLogFile = fopen(filePath, "wb+");
    if (s_psdkLogFile == NULL) {
        PsdkLogger_UserLogWarn("Open filepath time error.");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    return psdkStat;
}

static void PsdkUser_NormalExitHandler(int signalNum)
{
    USER_UTIL_UNUSED(signalNum);
    exit(0);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wreturn-type"

static void *PsdkUser_MonitorTask(void *argument)
{
    unsigned int i = 0;
    unsigned int threadCount = 0;
    pid_t *tidList = NULL;
    T_ThreadAttribute *threadAttribute = NULL;

    USER_UTIL_UNUSED(argument);

    while (1) {
        threadCount = Monitor_GetThreadCountOfProcess(getpid());
        tidList = PsdkOsal_Malloc(threadCount * sizeof(pid_t));
        if (tidList == NULL) {
            PsdkLogger_UserLogError("malloc fail.");
            goto delay;
        }
        Monitor_GetTidListOfProcess(getpid(), tidList, threadCount);

        threadAttribute = PsdkOsal_Malloc(threadCount * sizeof(T_ThreadAttribute));
        if (threadAttribute == NULL) {
            PsdkLogger_UserLogError("malloc fail.");
            goto freeTidList;
        }
        for (i = 0; i < threadCount; ++i) {
            threadAttribute[i].tid = tidList[i];
        }

        PsdkLogger_UserLogDebug("thread pcpu:");
        PsdkLogger_UserLogDebug("tid\tname\tpcpu");
        for (i = 0; i < threadCount; ++i) {
            threadAttribute[i].pcpu = Monitor_GetPcpuOfThread(getpid(), tidList[i]);
            Monitor_GetNameOfThread(getpid(), tidList[i], threadAttribute[i].name, sizeof(threadAttribute[i].name));
            PsdkLogger_UserLogDebug("%d\t%15s\t%f %%.", threadAttribute[i].tid, threadAttribute[i].name,
                                    threadAttribute[i].pcpu);
        }

        PsdkLogger_UserLogDebug("heap used: %d B.", Monitor_GetHeapUsed(getpid()));
        PsdkLogger_UserLogDebug("stack used: %d B.", Monitor_GetStackUsed(getpid()));

        PsdkOsal_Free(threadAttribute);
freeTidList:
        PsdkOsal_Free(tidList);

delay:
        sleep(10);
    }
}

#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wreturn-type"

int main(void)
{
    T_PsdkUserInfo userInfo;
    const T_PsdkDataChannelBandwidthProportionOfHighspeedChannel bandwidthProportionOfHighspeedChannel =
        {10, 60, 30};
    T_PsdkLoggerConsole printConsole = {
        .consoleLevel = PSDK_LOGGER_CONSOLE_LOG_LEVEL_INFO,
        .func = PsdkUser_Console,
    };

    T_PsdkLoggerConsole localRecordConsole = {
        .consoleLevel = PSDK_LOGGER_CONSOLE_LOG_LEVEL_INFO,
        .func = PsdkUser_LocalWrite,
    };

    T_PsdkHalUartHandler halUartHandler = {
        .UartInit = Hal_UartInit,
        .UartReadData = Hal_UartReadData,
        .UartWriteData = Hal_UartSendData,
    };

    T_PsdkHalNetWorkHandler halNetWorkHandler = {
        .NetWorkConfig = HalNetWork_Config,
    };

    T_PsdkOsalHandler osalHandler = {
        .Malloc = Osal_Malloc,
        .Free = Osal_Free,
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
        .SemaphorePost = Osal_SemaphorePost,
        .SemaphoreTimedWait = Osal_SemaphoreTimedWait,
        .GetTimeMs = Osal_GetTimeMs,
    };

    T_PsdkAircraftInfoBaseInfo aircraftBaseInfo = {0};

    if (PsdkPlatform_RegHalUartHandler(&halUartHandler) != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        printf("psdk register hal uart handler error");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    if (PsdkPlatform_RegHalNetworkHandler(&halNetWorkHandler) != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        printf("psdk register hal network handler error");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    if (PsdkPlatform_RegOsalHandler(&osalHandler) != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        printf("psdk register osal handler error");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    if (PsdkLogger_AddConsole(&printConsole) != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        printf("psdk add console print error");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    if (PsdkUser_FileSystemInit(PSDK_LOG_PATH) != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        printf("psdk file system init error");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    if (PsdkLogger_AddConsole(&localRecordConsole) != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        printf("psdk add console record error");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    if (PsdkUser_FillInUserInfo(&userInfo) != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        printf("psdk fill in user info error");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    if (PsdkCore_Init(&userInfo) != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("psdk instance init error");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    if (PsdkProductInfo_SetAlias("PSDK_APPALIAS") != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("set product alias error.");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    if (PsdkAircraftInfo_GetBaseInfo(&aircraftBaseInfo) != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("get aircraft information error.");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

#ifdef PSDK_USING_POWER_MANAGEMENT
    if (PsdkTest_PowerManagementInit() != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("power management init error");
    }
#endif

#ifdef PSDK_USING_CAMERA_EMU
    if (PsdkTest_CameraInit() != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("psdk camera init error");
    }
#endif

#ifdef PSDK_USING_CAMERA_MEDIA_EMU
#if PSDK_ARCH_SYS_LINUX
    if (PsdkTest_CameraMediaInit() != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("psdk camera media init error");
    }
#endif
#endif

#ifdef PSDK_USING_DATA_TRANSMISSION
    if (PsdkTest_DataTransmissionInit() != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("psdk data transmission init error");
    }
#endif

#ifdef PSDK_USING_DATA_CHANNEL
    if (PsdkTest_DataChannelSetBandwidthProportionForHighspeedChannel(
        bandwidthProportionOfHighspeedChannel) != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("set bandwidth distribution for high-speed channel error");
    }
#endif

#ifdef PSDK_USING_DATA_SUBSCRIPTION
    if (PsdkTest_DataSubscriptionInit() != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("psdk data subscription init error");
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

#ifdef PSDK_USING_WIDGET
    if (PsdkTest_WidgetInit() != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("psdk widget init error");
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
    T_PsdkTestUpgradePlatformOpt linuxUpgradePlatformOpt = {
        .rebootSystem = PsdkUpgradePlatformLinux_RebootSystem,
        .cleanUpgradeProgramFileStoreArea = PsdkUpgradePlatformLinux_CleanUpgradeProgramFileStoreArea,
        .createUpgradeProgramFile = PsdkUpgradePlatformLinux_CreateUpgradeProgramFile,
        .writeUpgradeProgramFile = PsdkUpgradePlatformLinux_WriteUpgradeProgramFile,
        .readUpgradeProgramFile = PsdkUpgradePlatformLinux_ReadUpgradeProgramFile,
        .closeUpgradeProgramFile = PsdkUpgradePlatformLinux_CloseUpgradeProgramFile,
        .replaceOldProgram = PsdkUpgradePlatformLinux_ReplaceOldProgram,
        .setUpgradeRebootState = PsdkUpgradePlatformLinux_SetUpgradeRebootState,
        .getUpgradeRebootState = PsdkUpgradePlatformLinux_GetUpgradeRebootState,
        .cleanUpgradeRebootState = PsdkUpgradePlatformLinux_CleanUpgradeRebootState,
    };
    if (PsdkTest_UpgradeInit(&linuxUpgradePlatformOpt) != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("psdk upgrade init error");
    }
#endif

#ifdef PSDK_USING_MOP_CHANNEL
    if (PsdkTest_MopChannelInit() != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("psdk mop channel init error");
    }
#endif

    if (pthread_create(&s_monitorThread, NULL, PsdkUser_MonitorTask, NULL) != 0) {
        PsdkLogger_UserLogError("create monitor task fail.");
    }

    if (pthread_setname_np(s_monitorThread, "monitor task") != 0) {
        PsdkLogger_UserLogError("set name for monitor task fail.");
    }

    if (PsdkCore_ApplicationStart() != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("psdk application start error");
    }

    signal(SIGTERM, PsdkUser_NormalExitHandler);

    while (1) {
        sleep(1);
    }
}

#pragma GCC diagnostic pop

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/