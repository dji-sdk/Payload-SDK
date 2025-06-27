/**
 ********************************************************************
 * @file    application.cpp
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
#include "application.hpp"
#include "dji_sdk_app_info.h"
#include <dji_platform.h>
#include <dji_logger.h>
#include <dji_core.h>
#include <dji_aircraft_info.h>
#include <csignal>
#include "dji_sdk_config.h"

#include "../common/osal/osal.h"
#include "../common/osal/osal_fs.h"
#include "../common/osal/osal_socket.h"
#include "../hal/hal_usb_bulk.h"

/* Private constants ---------------------------------------------------------*/
#define DJI_LOG_PATH                    "logs/DJI"
#define DJI_LOG_INDEX_FILE_NAME         "logs/index"
#define DJI_LOG_FOLDER_NAME             "logs"
#define DJI_LOG_PATH_MAX_SIZE           (128)
#define DJI_LOG_FOLDER_NAME_MAX_SIZE    (32)
#define DJI_SYSTEM_CMD_STR_MAX_SIZE     (64)
#define DJI_LOG_MAX_COUNT               (10)

#define USER_UTIL_UNUSED(x)                                 ((x) = (x))
#define USER_UTIL_MIN(a, b)                                 (((a) < (b)) ? (a) : (b))
#define USER_UTIL_MAX(a, b)                                 (((a) > (b)) ? (a) : (b))

extern "C" {
T_DjiReturnCode DjiTest_WidgetStartService(void);
}
/* Private types -------------------------------------------------------------*/

/* Private values -------------------------------------------------------------*/
static FILE *s_djiLogFile;
static FILE *s_djiLogFileCnt;

/* Private functions declaration ---------------------------------------------*/
static void DjiUser_NormalExitHandler(int signalNum);

/* Exported functions definition ---------------------------------------------*/
Application::Application(int argc, char **argv)
{
    Application::DjiUser_SetupEnvironment();
    Application::DjiUser_ApplicationStart();

    Osal_TaskSleepMs(3000);
}

Application::~Application()
= default;

/* Private functions definition-----------------------------------------------*/
void Application::DjiUser_SetupEnvironment()
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler osalHandler = {0};
    T_DjiHalUsbBulkHandler usbBulkHandler = {0};
    T_DjiLoggerConsole printConsole;
    T_DjiLoggerConsole localRecordConsole;
    T_DjiFileSystemHandler fileSystemHandler = {0};
    T_DjiSocketHandler socketHandler = {0};
    T_DjiHalNetworkHandler networkHandler = {0};

    socketHandler.Socket = Osal_Socket;
    socketHandler.Bind = Osal_Bind;
    socketHandler.Close = Osal_Close;
    socketHandler.UdpSendData = Osal_UdpSendData;
    socketHandler.UdpRecvData = Osal_UdpRecvData;
    socketHandler.TcpListen = Osal_TcpListen;
    socketHandler.TcpAccept = Osal_TcpAccept;
    socketHandler.TcpConnect = Osal_TcpConnect;
    socketHandler.TcpSendData = Osal_TcpSendData;
    socketHandler.TcpRecvData = Osal_TcpRecvData;

    osalHandler.TaskCreate = Osal_TaskCreate;
    osalHandler.TaskDestroy = Osal_TaskDestroy;
    osalHandler.TaskSleepMs = Osal_TaskSleepMs;
    osalHandler.MutexCreate = Osal_MutexCreate;
    osalHandler.MutexDestroy = Osal_MutexDestroy;
    osalHandler.MutexLock = Osal_MutexLock;
    osalHandler.MutexUnlock = Osal_MutexUnlock;
    osalHandler.SemaphoreCreate = Osal_SemaphoreCreate;
    osalHandler.SemaphoreDestroy = Osal_SemaphoreDestroy;
    osalHandler.SemaphoreWait = Osal_SemaphoreWait;
    osalHandler.SemaphoreTimedWait = Osal_SemaphoreTimedWait;
    osalHandler.SemaphorePost = Osal_SemaphorePost;
    osalHandler.Malloc = Osal_Malloc;
    osalHandler.Free = Osal_Free;
    osalHandler.GetTimeMs = Osal_GetTimeMs;
    osalHandler.GetTimeUs = Osal_GetTimeUs;
    osalHandler.GetRandomNum = Osal_GetRandomNum,

    printConsole.func = DjiUser_PrintConsole;
    printConsole.consoleLevel = DJI_LOGGER_CONSOLE_LOG_LEVEL_INFO;
    printConsole.isSupportColor = true;

    localRecordConsole.consoleLevel = DJI_LOGGER_CONSOLE_LOG_LEVEL_DEBUG;
    localRecordConsole.func = DjiUser_LocalWrite;
    localRecordConsole.isSupportColor = false;

    usbBulkHandler.UsbBulkInit = HalUsbBulk_Init;
    usbBulkHandler.UsbBulkDeInit = HalUsbBulk_DeInit;
    usbBulkHandler.UsbBulkWriteData = HalUsbBulk_WriteData;
    usbBulkHandler.UsbBulkReadData = HalUsbBulk_ReadData;
    usbBulkHandler.UsbBulkGetDeviceInfo = HalUsbBulk_GetDeviceInfo;

    fileSystemHandler.FileOpen = Osal_FileOpen,
    fileSystemHandler.FileClose = Osal_FileClose,
    fileSystemHandler.FileWrite = Osal_FileWrite,
    fileSystemHandler.FileRead = Osal_FileRead,
    fileSystemHandler.FileSync = Osal_FileSync,
    fileSystemHandler.FileSeek = Osal_FileSeek,
    fileSystemHandler.DirOpen = Osal_DirOpen,
    fileSystemHandler.DirClose = Osal_DirClose,
    fileSystemHandler.DirRead = Osal_DirRead,
    fileSystemHandler.Mkdir = Osal_Mkdir,
    fileSystemHandler.Unlink = Osal_Unlink,
    fileSystemHandler.Rename = Osal_Rename,
    fileSystemHandler.Stat = Osal_Stat,

    returnCode = DjiPlatform_RegOsalHandler(&osalHandler);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        throw std::runtime_error("Register osal handler error.");
    }

    returnCode = DjiPlatform_RegHalUsbBulkHandler(&usbBulkHandler);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        throw std::runtime_error("Register hal network handler error");
    }

    returnCode = DjiPlatform_RegSocketHandler(&socketHandler);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        throw std::runtime_error("register osal socket handler error");
    }

    returnCode = DjiPlatform_RegFileSystemHandler(&fileSystemHandler);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        throw std::runtime_error("Register osal filesystem handler error.");
    }

    if (DjiUser_LocalWriteFsInit(DJI_LOG_PATH) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        throw std::runtime_error("File system init error.");
    }

    returnCode = DjiLogger_AddConsole(&printConsole);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        throw std::runtime_error("Add printf console error.");
    }

    returnCode = DjiLogger_AddConsole(&localRecordConsole);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        throw std::runtime_error("Add printf console error.");
    }
}

void Application::DjiUser_ApplicationStart()
{
    T_DjiUserInfo userInfo;
    T_DjiReturnCode returnCode;
    T_DjiAircraftInfoBaseInfo aircraftInfoBaseInfo;
    T_DjiFirmwareVersion firmwareVersion = {
        .majorVersion = 1,
        .minorVersion = 0,
        .modifyVersion = 0,
        .debugVersion = 1,
    };

    // attention: when the program is hand up ctrl-c will generate the coredump file
    signal(SIGTERM, DjiUser_NormalExitHandler);

    returnCode = DjiUser_FillInUserInfo(&userInfo);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        throw std::runtime_error("Fill user info error, please check user info config.");
    }

    returnCode = DjiCore_Init(&userInfo);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        throw std::runtime_error("Core init error.");
    }

    returnCode = DjiAircraftInfo_GetBaseInfo(&aircraftInfoBaseInfo);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        throw std::runtime_error("Get aircraft base info error.");
    }

    if (aircraftInfoBaseInfo.mountPosition != DJI_MOUNT_POSITION_EXTENSION_PORT &&
        aircraftInfoBaseInfo.djiAdapterType != DJI_SDK_ADAPTER_TYPE_EPORT_V2_RIBBON_CABLE &&
        aircraftInfoBaseInfo.mountPositionType != DJI_MOUNT_POSITION_TYPE_MANIFOLD3_ONBOARD &&
        aircraftInfoBaseInfo.djiAdapterType != DJI_SDK_ADAPTER_TYPE_SKYPORT_V3) {
        throw std::runtime_error("Please run this sample on extension port or skyport v3.");
    }

    returnCode = DjiCore_SetAlias("PSDK_APPALIAS");
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        throw std::runtime_error("Set alias error.");
    }

    returnCode = DjiCore_SetFirmwareVersion(firmwareVersion);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        throw std::runtime_error("Set firmware version error.");
    }

    returnCode = DjiCore_SetSerialNumber("PSDK12345678XX");
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        throw std::runtime_error("Set serial number error");
    }

    returnCode = DjiTest_WidgetStartService();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("widget sample init error");
    }

    returnCode = DjiCore_ApplicationStart();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        throw std::runtime_error("Start sdk application error.");
    }

    USER_LOG_INFO("Application start.");
}

T_DjiReturnCode Application::DjiUser_PrintConsole(const uint8_t *data, uint16_t dataLen)
{
    printf("%s", data);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode Application::DjiUser_LocalWrite(const uint8_t *data, uint16_t dataLen)
{
    int32_t realLen;

    if (s_djiLogFile == nullptr) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    realLen = fwrite(data, 1, dataLen, s_djiLogFile);
    fflush(s_djiLogFile);
    if (realLen == dataLen) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    } else {
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }
}

T_DjiReturnCode Application::DjiUser_FillInUserInfo(T_DjiUserInfo *userInfo)
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
        !strcmp(USER_BAUD_RATE, "your_app_license") ||
        !strcmp(USER_DEVELOPER_ACCOUNT, "your_developer_account") ||
        !strcmp(USER_BAUD_RATE, "your_baud_rate")) {
        USER_LOG_ERROR(
            "Please fill in correct user information to 'samples/sample_c++/platform/linux/nvidia_jetson/application/dji_sdk_app_info.h' file.");
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

T_DjiReturnCode Application::DjiUser_LocalWriteFsInit(const char *path)
{
    T_DjiReturnCode djiReturnCode = DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    char filePath[DJI_LOG_PATH_MAX_SIZE];
    char systemCmd[DJI_SYSTEM_CMD_STR_MAX_SIZE];
    char folderName[DJI_LOG_FOLDER_NAME_MAX_SIZE];
    time_t currentTime = time(nullptr);
    struct tm *localTime = localtime(&currentTime);
    uint16_t logFileIndex = 0;
    uint16_t currentLogFileIndex;
    uint8_t ret;

    if (localTime == nullptr) {
        printf("Get local time error.\r\n");
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    if (access(DJI_LOG_FOLDER_NAME, F_OK) != 0) {
        sprintf(folderName, "mkdir %s", DJI_LOG_FOLDER_NAME);
        ret = system(folderName);
        if (ret != 0) {
            printf("Create new log folder error, ret:%d.\r\n", ret);
            return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
        }
    }

    s_djiLogFileCnt = fopen(DJI_LOG_INDEX_FILE_NAME, "rb+");
    if (s_djiLogFileCnt == nullptr) {
        s_djiLogFileCnt = fopen(DJI_LOG_INDEX_FILE_NAME, "wb+");
        if (s_djiLogFileCnt == nullptr) {
            return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
        }
    } else {
        ret = fseek(s_djiLogFileCnt, 0, SEEK_SET);
        if (ret != 0) {
            printf("Seek log count file error, ret: %d, errno: %d.\r\n", ret, errno);
            return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
        }

        ret = fread((uint16_t * ) & logFileIndex, 1, sizeof(uint16_t), s_djiLogFileCnt);
        if (ret != sizeof(uint16_t)) {
            printf("Read log file index error.\r\n");
        }
    }

    currentLogFileIndex = logFileIndex;
    logFileIndex++;

    ret = fseek(s_djiLogFileCnt, 0, SEEK_SET);
    if (ret != 0) {
        printf("Seek log file error, ret: %d, errno: %d.\r\n", ret, errno);
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    ret = fwrite((uint16_t * ) & logFileIndex, 1, sizeof(uint16_t), s_djiLogFileCnt);
    if (ret != sizeof(uint16_t)) {
        printf("Write log file index error.\r\n");
        fclose(s_djiLogFileCnt);
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    fclose(s_djiLogFileCnt);

    sprintf(filePath, "%s_%04d_%04d%02d%02d_%02d-%02d-%02d.log", path, currentLogFileIndex,
            localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday,
            localTime->tm_hour, localTime->tm_min, localTime->tm_sec);

    s_djiLogFile = fopen(filePath, "wb+");
    if (s_djiLogFile == nullptr) {
        USER_LOG_ERROR("Open filepath time error.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    if (logFileIndex >= DJI_LOG_MAX_COUNT) {
        sprintf(systemCmd, "rm -rf %s_%04d*.log", path, currentLogFileIndex - DJI_LOG_MAX_COUNT);
        ret = system(systemCmd);
        if (ret != 0) {
            printf("Remove file error, ret:%d.\r\n", ret);
            return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
        }
    }

    sprintf(systemCmd, "ln -sfrv %s " DJI_LOG_FOLDER_NAME "/latest.log", filePath);
    system(systemCmd);
    return djiReturnCode;
}

static void DjiUser_NormalExitHandler(int signalNum)
{
    USER_UTIL_UNUSED(signalNum);
    exit(0);
}

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
