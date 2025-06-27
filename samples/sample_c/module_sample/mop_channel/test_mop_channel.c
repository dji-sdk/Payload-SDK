/**
 ********************************************************************
 * @file    test_mop_channel.c
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
#include "dji_mop_channel.h"
#include "dji_logger.h"
#include "dji_platform.h"
#include "test_mop_channel.h"

/* Private constants ---------------------------------------------------------*/
#define DJI_MOP_CHANNEL_TASK_STACK_SIZE                          2048
#define TEST_MOP_CHANNEL_INIT_TIMEMS                             (3 * 1000)
#define TEST_MOP_CHANNEL_RETRY_TIMEMS                            (3 * 1000)

#define TEST_MOP_CHANNEL_NORMAL_TRANSFOR_CHANNEL_ID              49152
#define TEST_MOP_CHANNEL_NORMAL_TRANSFOR_USING_RELIABLE_TRANS    0
#define TEST_MOP_CHANNEL_NORMAL_TRANSFOR_SEND_TASK_FREQ          1
#define TEST_MOP_CHANNEL_NORMAL_TRANSFOR_SEND_BUFFER             (64 * 1024)
#define TEST_MOP_CHANNEL_NORMAL_TRANSFOR_RECV_BUFFER             (100 * 1024)

#define TEST_MOP_CHANNEL_FILE_SERVICE_CHANNEL_ID                 49153
#define TEST_MOP_CHANNEL_FILE_SERVICE_SEND_BUFFER                (3 * 1024 * 1024)
#define TEST_MOP_CHANNEL_FILE_SERVICE_RECV_BUFFER                (100 * 1024)
#define TEST_MOP_CHANNEL_FILE_SERVICE_CLIENT_MAX_SUPPORT_NUM     10

/* Private types -------------------------------------------------------------*/
typedef enum {
    MOP_FILE_SERVICE_DOWNLOAD_IDEL = 0,
    MOP_FILE_SERVICE_DOWNLOAD_REQUEST_START,
    MOP_FILE_SERVICE_DOWNLOAD_FILE_INFO_SUCCESS,
    MOP_FILE_SERVICE_DOWNLOAD_FILE_INFO_FAILED,
    MOP_FILE_SERVICE_DOWNLOAD_DATA_SENDING,
    MOP_FILE_SERVICE_DOWNLOAD_FINISHED_SUCCESS,
    MOP_FILE_SERVICE_DOWNLOAD_FINISHED_FAILED,
    MOP_FILE_SERVICE_DOWNLOAD_STOP,
} E_MopFileServiceDownloadState;

typedef enum {
    MOP_FILE_SERVICE_UPLOAD_IDEL = 0,
    MOP_FILE_SERVICE_UPLOAD_REQUEST_START,
    MOP_FILE_SERVICE_UPLOAD_FILE_INFO_SUCCESS,
    MOP_FILE_SERVICE_UPLOAD_FILE_INFO_FAILED,
    MOP_FILE_SERVICE_UPLOAD_DATA_SENDING,
    MOP_FILE_SERVICE_UPLOAD_FINISHED_SUCCESS,
    MOP_FILE_SERVICE_UPLOAD_FINISHED_FAILED,
    MOP_FILE_SERVICE_UPLOAD_STOP,
} E_MopFileServiceUploadState;

typedef struct {
    uint8_t index;
    T_DjiTaskHandle clientRecvTask;
    T_DjiTaskHandle clientSendTask;
    T_DjiMopChannelHandle clientHandle;
    E_MopFileServiceDownloadState downloadState;
    uint16_t downloadSeqNum;
    E_MopFileServiceUploadState uploadState;
    uint16_t uploadSeqNum;
} T_MopFileServiceClientContent;

/* Private values -------------------------------------------------------------*/
static T_DjiMopChannelHandle s_testMopChannelNormalHandle;
static T_DjiMopChannelHandle s_testMopChannelNormalOutHandle;
static T_DjiTaskHandle s_testMopChannelNormalSendTask;
static T_DjiTaskHandle s_testMopChannelNormalRecvTask;
static T_DjiSemaHandle s_testMopChannelReadySema;
static bool s_testMopChannelConnected = false;

static T_DjiTaskHandle s_fileServiceMopChannelAcceptTask;
static T_DjiMopChannelHandle s_fileServiceMopChannelHandle;
static T_MopFileServiceClientContent s_fileServiceContent[TEST_MOP_CHANNEL_FILE_SERVICE_CLIENT_MAX_SUPPORT_NUM];

/* Private functions declaration ---------------------------------------------*/
static void *DjiTest_MopChannelSendNormalTask(void *arg);
static void *DjiTest_MopChannelRecvNormalTask(void *arg);
static void *DjiTest_MopChannelFileServiceAcceptTask(void *arg);
static void *DjiTest_MopChannelFileServiceRecvTask(void *arg);
static void *DjiTest_MopChannelFileServiceSendTask(void *arg);

/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode DjiTest_MopChannelStartService(void)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    returnCode = osalHandler->SemaphoreCreate(0, &s_testMopChannelReadySema);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mop channel create msdk sema error, stat:0x%08llX.", returnCode);
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    returnCode = DjiMopChannel_Init();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mop channel init error, stat:0x%08llX.", returnCode);
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    returnCode = osalHandler->TaskCreate("mop_msdk_send_task", DjiTest_MopChannelSendNormalTask,
                                         DJI_MOP_CHANNEL_TASK_STACK_SIZE, NULL, &s_testMopChannelNormalSendTask);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mop channel msdk send task create error, stat:0x%08llX.", returnCode);
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    returnCode = osalHandler->TaskCreate("mop_msdk_recv_task", DjiTest_MopChannelRecvNormalTask,
                                         DJI_MOP_CHANNEL_TASK_STACK_SIZE, NULL, &s_testMopChannelNormalRecvTask);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mop channel msdk recv task create error, stat:0x%08llX.", returnCode);
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    returnCode = osalHandler->TaskCreate("mop_msdk_accept_task", DjiTest_MopChannelFileServiceAcceptTask,
                                         DJI_MOP_CHANNEL_TASK_STACK_SIZE, NULL, &s_fileServiceMopChannelAcceptTask);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mop channel osdk recv task create error, stat:0x%08llX.", returnCode);
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/* Private functions definition-----------------------------------------------*/
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wreturn-type"

static void *DjiTest_MopChannelSendNormalTask(void *arg)
{
    uint8_t *sendBuf = NULL;
    uint32_t realLen = 0;
    T_DjiReturnCode returnCode;
    uint32_t sendDataCount = 0;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    USER_UTIL_UNUSED(arg);

    osalHandler->TaskSleepMs(TEST_MOP_CHANNEL_INIT_TIMEMS);

    sendBuf = osalHandler->Malloc(TEST_MOP_CHANNEL_NORMAL_TRANSFOR_SEND_BUFFER);
    if (sendBuf == NULL) {
        USER_LOG_ERROR("malloc send buffer error");
        return NULL;
    }

REWAIT:
    returnCode = osalHandler->SemaphoreWait(s_testMopChannelReadySema);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mop channel wait sema error, stat:0x%08llX.", returnCode);
        return NULL;
    }

    while (1) {
        if (s_testMopChannelConnected == false) {
            sendDataCount = 0;
            goto REWAIT;
        }

        sendDataCount++;
        memset(sendBuf, 'A' - 1 + (sendDataCount % 26), TEST_MOP_CHANNEL_NORMAL_TRANSFOR_SEND_BUFFER);
        returnCode = DjiMopChannel_SendData(s_testMopChannelNormalOutHandle, sendBuf,
                                            TEST_MOP_CHANNEL_NORMAL_TRANSFOR_SEND_BUFFER, &realLen);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("mop channel send data to channel error,stat:0x%08llX", returnCode);
        } else {
            USER_LOG_INFO("mop channel send data to channel length:%d count:%d", realLen, sendDataCount);
        }

        osalHandler->TaskSleepMs(1000 / TEST_MOP_CHANNEL_NORMAL_TRANSFOR_SEND_TASK_FREQ);
    }
}

#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wreturn-type"

static void *DjiTest_MopChannelRecvNormalTask(void *arg)
{
    USER_UTIL_UNUSED(arg);
    uint8_t *recvBuf = NULL;
    uint32_t realLen;
    T_DjiReturnCode returnCode;
    uint32_t recvDataCount = 0;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    osalHandler->TaskSleepMs(TEST_MOP_CHANNEL_INIT_TIMEMS);

#if TEST_MOP_CHANNEL_NORMAL_TRANSFOR_USING_RELIABLE_TRANS
    returnCode = DjiMopChannel_Create(&s_testMopChannelNormalHandle, DJI_MOP_CHANNEL_TRANS_RELIABLE);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mop channel create send handle error, stat:0x%08llX.", returnCode);
        return NULL;
    }
#else
    returnCode = DjiMopChannel_Create(&s_testMopChannelNormalHandle, DJI_MOP_CHANNEL_TRANS_UNRELIABLE);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mop channel create send handle error, stat:0x%08llX.", returnCode);
        return NULL;
    }
#endif

REBIND:
    returnCode = DjiMopChannel_Bind(s_testMopChannelNormalHandle, TEST_MOP_CHANNEL_NORMAL_TRANSFOR_CHANNEL_ID);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mop bind channel error :0x%08llX", returnCode);
        osalHandler->TaskSleepMs(TEST_MOP_CHANNEL_RETRY_TIMEMS);
        goto REBIND;
    }

REACCEPT:
    returnCode = DjiMopChannel_Accept(s_testMopChannelNormalHandle, &s_testMopChannelNormalOutHandle);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_WARN("mop accept channel error :0x%08llX", returnCode);
        osalHandler->TaskSleepMs(TEST_MOP_CHANNEL_RETRY_TIMEMS);
        goto REACCEPT;
    }

    USER_LOG_INFO("mop channel is connected");

    recvBuf = osalHandler->Malloc(TEST_MOP_CHANNEL_NORMAL_TRANSFOR_RECV_BUFFER);
    if (recvBuf == NULL) {
        USER_LOG_ERROR("malloc recv buffer error");
        return NULL;
    }

    s_testMopChannelConnected = true;

    returnCode = osalHandler->SemaphorePost(s_testMopChannelReadySema);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("mop channel post sema error, stat:0x%08llX.", returnCode);
        return NULL;
    }

    while (1) {
        memset(recvBuf, 0, TEST_MOP_CHANNEL_NORMAL_TRANSFOR_RECV_BUFFER);

        returnCode = DjiMopChannel_RecvData(s_testMopChannelNormalOutHandle, recvBuf,
                                            TEST_MOP_CHANNEL_NORMAL_TRANSFOR_RECV_BUFFER, &realLen);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            if (returnCode == DJI_ERROR_MOP_CHANNEL_MODULE_CODE_CONNECTION_CLOSE) {
                USER_LOG_INFO("mop channel is disconnected");
                s_testMopChannelConnected = false;
                osalHandler->TaskSleepMs(TEST_MOP_CHANNEL_RETRY_TIMEMS);
                DjiMopChannel_Close(s_testMopChannelNormalOutHandle);
                DjiMopChannel_Destroy(s_testMopChannelNormalOutHandle);
                goto REACCEPT;
            }
        } else {
            USER_LOG_INFO("mop channel recv data from channel length:%d count:%d", realLen, recvDataCount++);
        }
    }
}

#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wreturn-type"

static void *DjiTest_MopChannelFileServiceAcceptTask(void *arg)
{
    T_DjiReturnCode returnCode;
    uint8_t currentClientNum = 0;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    USER_UTIL_UNUSED(arg);

    USER_LOG_DEBUG("[File-Service] Start the file service.");

    returnCode = DjiMopChannel_Create(&s_fileServiceMopChannelHandle, DJI_MOP_CHANNEL_TRANS_RELIABLE);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("[File-Service] mop channel create send handle error, stat:0x%08llX.", returnCode);
        return NULL;
    }

REBIND:
    returnCode = DjiMopChannel_Bind(s_fileServiceMopChannelHandle, TEST_MOP_CHANNEL_FILE_SERVICE_CHANNEL_ID);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("[File-Service] mop bind channel error :0x%08llX", returnCode);
        osalHandler->TaskSleepMs(TEST_MOP_CHANNEL_RETRY_TIMEMS);
        goto REBIND;
    }

    while (1) {
REACCEPT:
        returnCode = DjiMopChannel_Accept(s_fileServiceMopChannelHandle,
                                          &s_fileServiceContent[currentClientNum].clientHandle);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_WARN("[File-Service] mop accept channel error :0x%08llX", returnCode);
            osalHandler->TaskSleepMs(TEST_MOP_CHANNEL_RETRY_TIMEMS);
            goto REACCEPT;
        }

        USER_LOG_INFO("[File-Service] [Client:%d] mop channel is connected", currentClientNum);

        s_fileServiceContent[currentClientNum].index = currentClientNum;
        returnCode = osalHandler->TaskCreate("mop_file_service_recv_task",
                                             DjiTest_MopChannelFileServiceRecvTask,
                                             DJI_MOP_CHANNEL_TASK_STACK_SIZE,
                                             &s_fileServiceContent[currentClientNum].index,
                                             &s_fileServiceContent[currentClientNum].clientRecvTask);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("mop channel recv task create error, stat:0x%08llX.", returnCode);
            return NULL;
        }

        returnCode = osalHandler->TaskCreate("mop_file_service_send_task",
                                             DjiTest_MopChannelFileServiceSendTask,
                                             DJI_MOP_CHANNEL_TASK_STACK_SIZE,
                                             &s_fileServiceContent[currentClientNum].index,
                                             &s_fileServiceContent[currentClientNum].clientSendTask);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("mop channel send task create error, stat:0x%08llX.", returnCode);
            return NULL;
        }

        currentClientNum++;
        if (currentClientNum > TEST_MOP_CHANNEL_FILE_SERVICE_CLIENT_MAX_SUPPORT_NUM) {
            currentClientNum = 0;
        }
    }
}

#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wreturn-type"

static void *DjiTest_MopChannelFileServiceSendTask(void *arg)
{
    T_DjiReturnCode returnCode;
    uint8_t clientNum = *(uint8_t *) arg;
    uint32_t sendRealLen = 0;
    uint8_t *sendBuf;
    MD5_CTX downloadFileMd5Ctx;
    FILE *downloadFile = NULL;
    uint8_t downloadFileMd5[DJI_MD5_BUFFER_LEN] = {0};
    uint64_t downloadFileTotalSize = 0;
    uint64_t downloadWriteLen;
    uint16_t downloadPackCount = 0;
    T_DjiMopChannel_FileInfo downloadFileInfo = {0};
    T_DjiMopChannel_FileTransfor transforAck = {0};
    T_DjiMopChannel_FileTransfor fileData = {0};
    T_DjiMopChannel_FileTransfor fileInfo = {0};
    uint32_t downloadStartMs = 0;
    uint32_t downloadEndMs = 0;
    uint32_t downloadDurationMs;
    dji_f32_t downloadRate;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    char curFileDirPath[DJI_FILE_PATH_SIZE_MAX];
    char tempPath[DJI_FILE_PATH_SIZE_MAX];

    sendBuf = osalHandler->Malloc(TEST_MOP_CHANNEL_FILE_SERVICE_SEND_BUFFER);
    if (sendBuf == NULL) {
        USER_LOG_ERROR("[File-Service] [Client:%d] malloc send buffer error", clientNum);
        return NULL;
    }

    while (1) {
        switch (s_fileServiceContent[clientNum].uploadState) {
            case MOP_FILE_SERVICE_UPLOAD_REQUEST_START:
                transforAck.cmd = DJI_MOP_CHANNEL_FILE_TRANSFOR_CMD_ACK;
                transforAck.subcmd = DJI_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_ACK_OK;
                transforAck.seqNum = s_fileServiceContent[clientNum].uploadSeqNum;
                transforAck.dataLen = 0;
                DjiMopChannel_SendData(s_fileServiceContent[clientNum].clientHandle, (uint8_t * ) & transforAck,
                                       UTIL_OFFSETOF(T_DjiMopChannel_FileTransfor, data),
                                       &sendRealLen);
                USER_LOG_DEBUG("[File-Service] [Client:%d] upload request ack", clientNum);
                s_fileServiceContent[clientNum].uploadState = MOP_FILE_SERVICE_UPLOAD_IDEL;
                break;
            case MOP_FILE_SERVICE_UPLOAD_FILE_INFO_SUCCESS:
                transforAck.cmd = DJI_MOP_CHANNEL_FILE_TRANSFOR_CMD_ACK;
                transforAck.subcmd = DJI_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_ACK_OK;
                transforAck.seqNum = s_fileServiceContent[clientNum].uploadSeqNum;
                transforAck.dataLen = 0;
                DjiMopChannel_SendData(s_fileServiceContent[clientNum].clientHandle, (uint8_t * ) & transforAck,
                                       UTIL_OFFSETOF(T_DjiMopChannel_FileTransfor, data),
                                       &sendRealLen);
                USER_LOG_DEBUG("[File-Service] [Client:%d] upload file info success", clientNum);
                s_fileServiceContent[clientNum].uploadState = MOP_FILE_SERVICE_UPLOAD_IDEL;
                break;
            case MOP_FILE_SERVICE_UPLOAD_FILE_INFO_FAILED:
                transforAck.cmd = DJI_MOP_CHANNEL_FILE_TRANSFOR_CMD_ACK;
                transforAck.subcmd = DJI_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_ACK_REJECTED;
                transforAck.seqNum = s_fileServiceContent[clientNum].uploadSeqNum;
                transforAck.dataLen = 0;
                DjiMopChannel_SendData(s_fileServiceContent[clientNum].clientHandle, (uint8_t * ) & transforAck,
                                       UTIL_OFFSETOF(T_DjiMopChannel_FileTransfor, data),
                                       &sendRealLen);
                USER_LOG_ERROR("[File-Service] [Client:%d] upload file info failed", clientNum);
                s_fileServiceContent[clientNum].uploadState = MOP_FILE_SERVICE_UPLOAD_IDEL;
                break;
            case MOP_FILE_SERVICE_UPLOAD_FINISHED_SUCCESS:
                transforAck.cmd = DJI_MOP_CHANNEL_FILE_TRANSFOR_CMD_RESULT;
                transforAck.subcmd = DJI_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_RESULT_OK;
                transforAck.seqNum = s_fileServiceContent[clientNum].uploadSeqNum++;
                transforAck.dataLen = 0;
                DjiMopChannel_SendData(s_fileServiceContent[clientNum].clientHandle,
                                       (uint8_t * ) & transforAck,
                                       UTIL_OFFSETOF(T_DjiMopChannel_FileTransfor, data),
                                       &sendRealLen);
                USER_LOG_DEBUG("[File-Service] [Client:%d] upload finished success", clientNum);
                s_fileServiceContent[clientNum].uploadState = MOP_FILE_SERVICE_UPLOAD_IDEL;

                break;
            case MOP_FILE_SERVICE_UPLOAD_FINISHED_FAILED:
                transforAck.cmd = DJI_MOP_CHANNEL_FILE_TRANSFOR_CMD_RESULT;
                transforAck.subcmd = DJI_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_RESULT_FAILED;
                transforAck.seqNum = s_fileServiceContent[clientNum].uploadSeqNum++;
                transforAck.dataLen = 0;
                DjiMopChannel_SendData(s_fileServiceContent[clientNum].clientHandle,
                                       (uint8_t * ) & transforAck,
                                       UTIL_OFFSETOF(T_DjiMopChannel_FileTransfor, data),
                                       &sendRealLen);
                USER_LOG_ERROR("[File-Service] [Client:%d] upload finished failed", clientNum);
                s_fileServiceContent[clientNum].uploadState = MOP_FILE_SERVICE_UPLOAD_IDEL;
                break;
            case MOP_FILE_SERVICE_UPLOAD_STOP:
                transforAck.cmd = DJI_MOP_CHANNEL_FILE_TRANSFOR_CMD_STOP_ACK;
                transforAck.subcmd = DJI_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_STOP_UPLOAD;
                transforAck.seqNum = s_fileServiceContent[clientNum].uploadSeqNum++;
                transforAck.dataLen = 0;
                DjiMopChannel_SendData(s_fileServiceContent[clientNum].clientHandle,
                                       (uint8_t *) &transforAck,
                                       UTIL_OFFSETOF(T_DjiMopChannel_FileTransfor, data),
                                       &sendRealLen);
                s_fileServiceContent[clientNum].uploadState = MOP_FILE_SERVICE_UPLOAD_IDEL;
                break;
            default:
                break;
        }

        switch (s_fileServiceContent[clientNum].downloadState) {
            case MOP_FILE_SERVICE_DOWNLOAD_REQUEST_START:
                transforAck.cmd = DJI_MOP_CHANNEL_FILE_TRANSFOR_CMD_ACK;
                transforAck.subcmd = DJI_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_ACK_OK;
                transforAck.seqNum = s_fileServiceContent[clientNum].downloadSeqNum;
                transforAck.dataLen = 0;
                DjiMopChannel_SendData(s_fileServiceContent[clientNum].clientHandle, (uint8_t * ) & transforAck,
                                       UTIL_OFFSETOF(T_DjiMopChannel_FileTransfor, data),
                                       &sendRealLen);
                USER_LOG_DEBUG("[File-Service] [Client:%d] download request ack", clientNum);
                s_fileServiceContent[clientNum].downloadState = MOP_FILE_SERVICE_DOWNLOAD_IDEL;

                break;
            case MOP_FILE_SERVICE_DOWNLOAD_STOP:
                transforAck.cmd = DJI_MOP_CHANNEL_FILE_TRANSFOR_CMD_STOP_ACK;
                transforAck.subcmd = DJI_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_STOP_DOWNLOAD;
                transforAck.seqNum = s_fileServiceContent[clientNum].uploadSeqNum++;
                transforAck.dataLen = 0;
                DjiMopChannel_SendData(s_fileServiceContent[clientNum].clientHandle,
                                       (uint8_t *) &transforAck,
                                       UTIL_OFFSETOF(T_DjiMopChannel_FileTransfor, data),
                                       &sendRealLen);
                s_fileServiceContent[clientNum].downloadState = MOP_FILE_SERVICE_DOWNLOAD_IDEL;
                break;
            case MOP_FILE_SERVICE_DOWNLOAD_FILE_INFO_SUCCESS:
                UtilMd5_Init(&downloadFileMd5Ctx);
                osalHandler->GetTimeMs(&downloadStartMs);
                if (downloadFile != NULL) {
                    fclose(downloadFile);
                }

                returnCode = DjiUserUtil_GetCurrentFileDirPath(__FILE__, DJI_FILE_PATH_SIZE_MAX, curFileDirPath);
                if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    USER_LOG_ERROR("Get file current path error, stat = 0x%08llX", returnCode);
                    exit(1);
                }
                snprintf(tempPath, DJI_FILE_PATH_SIZE_MAX, "%smop_channel_test_file/mop_send_test_file.mp4", curFileDirPath);

                downloadFile = fopen(tempPath, "rb");
                if (downloadFile == NULL) {
                    USER_LOG_ERROR("[File-Service] [Client:%d] download open file error",
                                   clientNum);
                    return NULL;
                }

                downloadFileTotalSize = 0;
                while (1) {
                    returnCode = fseek(downloadFile, downloadFileTotalSize, SEEK_SET);
                    if (returnCode != 0) {
                        USER_LOG_ERROR(
                            "[File-Service] [Client:%d] mop channel fseek file data fail.", clientNum);
                    }
                    downloadWriteLen = fread(sendBuf, 1, TEST_MOP_CHANNEL_FILE_SERVICE_SEND_BUFFER,
                                             downloadFile);
                    if (downloadWriteLen > 0) {
                        downloadFileTotalSize += downloadWriteLen;
                        UtilMd5_Update(&downloadFileMd5Ctx, sendBuf, downloadWriteLen);
                        if (downloadWriteLen < TEST_MOP_CHANNEL_FILE_SERVICE_SEND_BUFFER) {
                            break;
                        }
                    }
                }
                UtilMd5_Final(&downloadFileMd5Ctx, downloadFileMd5);

                fileInfo.cmd = DJI_MOP_CHANNEL_FILE_TRANSFOR_CMD_FILE_INFO;
                fileInfo.subcmd = DJI_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_DOWNLOAD_REQUEST;
                fileInfo.seqNum = s_fileServiceContent[clientNum].downloadSeqNum;
                fileInfo.dataLen = sizeof(fileInfo) - UTIL_OFFSETOF(T_DjiMopChannel_FileTransfor, data);

                fileInfo.data.fileInfo.isExist = true;
                downloadFileInfo.fileLength = downloadFileTotalSize;
                fileInfo.data.fileInfo.fileLength = downloadFileTotalSize;
                strcpy(fileInfo.data.fileInfo.fileName, "test.mp4");
                memcpy(&fileInfo.data.fileInfo.md5Buf, &downloadFileMd5, sizeof(downloadFileMd5));
                DjiMopChannel_SendData(s_fileServiceContent[clientNum].clientHandle, (uint8_t *) &fileInfo,
                                       sizeof(T_DjiMopChannel_FileTransfor),
                                       &sendRealLen);
                USER_LOG_DEBUG(
                    "[File-Service] [Client:%d] download ack file info exist:%d length:%d name:%s",
                    clientNum, fileInfo.data.fileInfo.isExist,
                    fileInfo.data.fileInfo.fileLength, fileInfo.data.fileInfo.fileName);

                s_fileServiceContent[clientNum].downloadState = MOP_FILE_SERVICE_DOWNLOAD_DATA_SENDING;
                downloadFileTotalSize = 0;
                downloadPackCount = 0;
                break;
            case MOP_FILE_SERVICE_DOWNLOAD_DATA_SENDING:
                if (downloadFile == NULL) {
                    USER_LOG_ERROR("[File-Service] [Client:%d] download file object is NULL.");
                    break;
                }

                returnCode = fseek(downloadFile, downloadFileTotalSize, SEEK_SET);
                if (returnCode != 0) {
                    USER_LOG_ERROR("[File-Service] [Client:%d] download fseek file data fail.",
                                   clientNum);
                    break;
                }
                downloadWriteLen = fread(&sendBuf[UTIL_OFFSETOF(T_DjiMopChannel_FileTransfor, data)],
                                         1,
                                         (TEST_MOP_CHANNEL_FILE_SERVICE_SEND_BUFFER -
                                          UTIL_OFFSETOF(T_DjiMopChannel_FileTransfor, data)),
                                         downloadFile);

                if (downloadWriteLen > 0) {
                    downloadFileTotalSize += downloadWriteLen;
                    downloadPackCount++;

                    fileData.cmd = DJI_MOP_CHANNEL_FILE_TRANSFOR_CMD_FILE_DATA;
                    fileData.dataLen = downloadWriteLen;
                    fileData.seqNum++;
                    if (downloadWriteLen ==
                        (TEST_MOP_CHANNEL_FILE_SERVICE_SEND_BUFFER -
                         UTIL_OFFSETOF(T_DjiMopChannel_FileTransfor, data))) {
                        fileData.subcmd = DJI_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_FILE_DATA_NORMAL;
                    } else {
                        fileData.subcmd = DJI_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_FILE_DATA_END;
                    }

                    memcpy(sendBuf, &fileData, UTIL_OFFSETOF(T_DjiMopChannel_FileTransfor, data));
                    returnCode = DjiMopChannel_SendData(s_fileServiceContent[clientNum].clientHandle, sendBuf,
                                                        (downloadWriteLen +
                                                         UTIL_OFFSETOF(T_DjiMopChannel_FileTransfor, data)),
                                                        &sendRealLen);
                    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                        USER_LOG_ERROR(
                            "[File-Service] [Client:%d] download send file data error,stat:0x%08llX",
                            clientNum, returnCode);
                        if (returnCode == DJI_ERROR_MOP_CHANNEL_MODULE_CODE_CONNECTION_CLOSE) {
                            break;
                        }
                    } else {
                        USER_LOG_INFO(
                            "[File-Service] [Client:%d] download send file data length:%d count:%d total:%d percent: %.1f %%",
                            clientNum, sendRealLen, downloadPackCount, downloadFileTotalSize,
                            (dji_f32_t)(downloadFileTotalSize) * 100 / (dji_f32_t) downloadFileInfo.fileLength);
                    }

                    if (fileData.subcmd == DJI_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_FILE_DATA_END) {
                        osalHandler->GetTimeMs(&downloadEndMs);
                        downloadDurationMs = downloadEndMs - downloadStartMs;
                        if (downloadDurationMs != 0) {
                            downloadRate = (dji_f32_t) downloadFileInfo.fileLength * 1000 /
                                           (dji_f32_t) (downloadDurationMs);
                            USER_LOG_INFO(
                                "[File-Service] [Client:%d] download finished totalTime:%d, rate:%.2f Byte/s",
                                clientNum, downloadDurationMs, downloadRate);
                        }
                    }
                    break;
                    default:
                        break;
                }
        }
    }
}

#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wreturn-type"

static void *DjiTest_MopChannelFileServiceRecvTask(void *arg)
{
    T_DjiReturnCode returnCode;
    uint8_t clientNum = *(uint8_t *) arg;
    uint32_t recvRealLen;
    uint8_t *recvBuf;
    MD5_CTX uploadFileMd5Ctx;
    FILE *uploadFile = NULL;
    uint8_t uploadFileMd5[DJI_MD5_BUFFER_LEN] = {0};
    uint32_t uploadFileTotalSize = 0;
    int32_t uploadWriteLen;
    T_DjiMopChannel_FileInfo uploadFileInfo = {0};
    uint32_t uploadStartMs = 0;
    uint32_t uploadEndMs = 0;
    dji_f32_t uploadRate;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    recvBuf = osalHandler->Malloc(TEST_MOP_CHANNEL_FILE_SERVICE_RECV_BUFFER);
    if (recvBuf == NULL) {
        USER_LOG_ERROR("[File-Service] [Client:%d] malloc recv buffer error", clientNum);
        return NULL;
    }

    s_fileServiceContent[clientNum].uploadState = MOP_FILE_SERVICE_UPLOAD_IDEL;
    s_fileServiceContent[clientNum].downloadState = MOP_FILE_SERVICE_DOWNLOAD_IDEL;

    while (1) {
        returnCode = DjiMopChannel_RecvData(s_fileServiceContent[clientNum].clientHandle, recvBuf,
                                            TEST_MOP_CHANNEL_FILE_SERVICE_RECV_BUFFER, &recvRealLen);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            osalHandler->TaskSleepMs(1000);
            if (returnCode == DJI_ERROR_MOP_CHANNEL_MODULE_CODE_CONNECTION_CLOSE) {
                USER_LOG_INFO("[File-Service] [Client:%d] mop channel is disconnected", clientNum);
                osalHandler->TaskDestroy(s_fileServiceContent[clientNum].clientRecvTask);
                DjiMopChannel_Close(s_fileServiceContent[clientNum].clientHandle);
                DjiMopChannel_Destroy(s_fileServiceContent[clientNum].clientHandle);
            }
        } else {
            if (&recvRealLen > 0) {
                T_DjiMopChannel_FileTransfor *fileTransfor = (T_DjiMopChannel_FileTransfor *) recvBuf;

                switch (fileTransfor->cmd) {
                    case DJI_MOP_CHANNEL_FILE_TRANSFOR_CMD_REQUEST:
                        if (fileTransfor->subcmd == DJI_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_REQUEST_UPLOAD) {
                            s_fileServiceContent[clientNum].uploadState = MOP_FILE_SERVICE_UPLOAD_REQUEST_START;
                            s_fileServiceContent[clientNum].uploadSeqNum = fileTransfor->seqNum;

                            USER_LOG_DEBUG("[File-Service] [Client:%d] upload request is ok", clientNum);
                            UtilMd5_Init(&uploadFileMd5Ctx);
                            uploadFileTotalSize = 0;
                            osalHandler->GetTimeMs(&uploadStartMs);
                        } else if (fileTransfor->subcmd == DJI_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_REQUEST_DOWNLOAD) {
                            s_fileServiceContent[clientNum].downloadState = MOP_FILE_SERVICE_DOWNLOAD_REQUEST_START;
                            s_fileServiceContent[clientNum].downloadSeqNum = fileTransfor->seqNum;

                            USER_LOG_DEBUG("[File-Service] [Client:%d] download request is ok", clientNum);
                        }
                        break;
                    case DJI_MOP_CHANNEL_FILE_TRANSFOR_CMD_FILE_DOWNLOAD_REQ:
                        USER_LOG_DEBUG("[File-Service] [Client:%d] download request file name:%s", clientNum,
                                       fileTransfor->data.dwonloadReq.fileName);

                        if (strcmp(fileTransfor->data.dwonloadReq.fileName, "test.mp4") == 0) {
                            s_fileServiceContent[clientNum].downloadState = MOP_FILE_SERVICE_DOWNLOAD_FILE_INFO_SUCCESS;
                            s_fileServiceContent[clientNum].downloadSeqNum = fileTransfor->seqNum;
                        } else {
                            s_fileServiceContent[clientNum].downloadState = MOP_FILE_SERVICE_DOWNLOAD_FILE_INFO_FAILED;
                            s_fileServiceContent[clientNum].downloadSeqNum = fileTransfor->seqNum;
                        }
                        break;
                    case DJI_MOP_CHANNEL_FILE_TRANSFOR_CMD_FILE_INFO:
                        USER_LOG_DEBUG(
                            "[File-Service] [Client:%d] upload file info length:%d exist:%d name:%s seq:%d", clientNum,
                            fileTransfor->data.fileInfo.fileLength,
                            fileTransfor->data.fileInfo.isExist,
                            fileTransfor->data.fileInfo.fileName, fileTransfor->seqNum);

                        uploadFileInfo.fileLength = fileTransfor->data.fileInfo.fileLength;
                        memcpy(uploadFileInfo.md5Buf, fileTransfor->data.fileInfo.md5Buf,
                               sizeof(uploadFileInfo.md5Buf));
                        if (uploadFile != NULL) {
                            fclose(uploadFile);
                        }
                        uploadFile = fopen(fileTransfor->data.fileInfo.fileName, "wb");
                        if (uploadFile == NULL) {
                            USER_LOG_ERROR("[File-Service] [Client:%d] open file error", clientNum);
                            return NULL;
                        }

                        s_fileServiceContent[clientNum].uploadState = MOP_FILE_SERVICE_UPLOAD_FILE_INFO_SUCCESS;
                        s_fileServiceContent[clientNum].uploadSeqNum = fileTransfor->seqNum;

                        break;
                    case DJI_MOP_CHANNEL_FILE_TRANSFOR_CMD_FILE_DATA:
                        if (uploadFile == NULL) {
                            USER_LOG_ERROR("[File-Service] [Client:%d] open file error", clientNum);
                            return NULL;
                        }

                        if (fileTransfor->subcmd == DJI_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_FILE_DATA_NORMAL) {
                            s_fileServiceContent[clientNum].uploadState = MOP_FILE_SERVICE_UPLOAD_DATA_SENDING;
                            s_fileServiceContent[clientNum].uploadSeqNum = fileTransfor->seqNum;

                            USER_LOG_INFO("fileTransfor->seqNum  %d", fileTransfor->seqNum);

                            uploadWriteLen = fwrite(&recvBuf[UTIL_OFFSETOF(T_DjiMopChannel_FileTransfor, data)], 1,
                                                    fileTransfor->dataLen, uploadFile);
                            if (uploadWriteLen < 0) {
                                USER_LOG_ERROR(
                                    "[File-Service] [Client:%d] upload write normal data to file error, stat:%d.",
                                    clientNum,
                                    uploadWriteLen);
                            } else {
                                uploadFileTotalSize += uploadWriteLen;
                                UtilMd5_Update(&uploadFileMd5Ctx,
                                               &recvBuf[UTIL_OFFSETOF(T_DjiMopChannel_FileTransfor, data)],
                                               fileTransfor->dataLen);

                                if (uploadFileInfo.fileLength != 0) {
                                    USER_LOG_INFO(
                                        "[File-Service] [Client:%d] upload write data to file success, len:%d  percent:%.1f %%",
                                        clientNum, uploadWriteLen,
                                        (dji_f32_t)(uploadFileTotalSize * 100) /
                                        (dji_f32_t) uploadFileInfo.fileLength);
                                }
                            }
                        } else if (fileTransfor->subcmd == DJI_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_FILE_DATA_END) {
                            s_fileServiceContent[clientNum].uploadState = MOP_FILE_SERVICE_UPLOAD_DATA_SENDING;
                            s_fileServiceContent[clientNum].uploadSeqNum = fileTransfor->seqNum;
                            uploadWriteLen = fwrite(&recvBuf[UTIL_OFFSETOF(T_DjiMopChannel_FileTransfor, data)], 1,
                                                    fileTransfor->dataLen, uploadFile);
                            if (uploadWriteLen < 0) {
                                USER_LOG_ERROR(
                                    "[File-Service] [Client:%d] upload write end data to file error, stat:%d.",
                                    clientNum,
                                    uploadWriteLen);
                            } else {
                                uploadFileTotalSize += uploadWriteLen;
                                UtilMd5_Update(&uploadFileMd5Ctx,
                                               &recvBuf[UTIL_OFFSETOF(T_DjiMopChannel_FileTransfor, data)],
                                               fileTransfor->dataLen);
                                UtilMd5_Final(&uploadFileMd5Ctx, uploadFileMd5);
                                osalHandler->GetTimeMs(&uploadEndMs);

                                if (uploadEndMs - uploadStartMs > 0) {
                                    uploadRate = (dji_f32_t) uploadFileTotalSize * 1000 /
                                                 (dji_f32_t) (uploadEndMs - uploadStartMs);
                                    USER_LOG_INFO(
                                        "[File-Service] [Client:%d] upload write data to file success, len:%d  percent:%.1f %%",
                                        clientNum, uploadWriteLen,
                                        (dji_f32_t)(uploadFileTotalSize * 100) /
                                        (dji_f32_t) uploadFileInfo.fileLength);
                                    USER_LOG_INFO(
                                        "[File-Service] [Client:%d] upload file finished, totalTime:%d ms rate:%.2f Byte/s",
                                        clientNum, (uploadEndMs - uploadStartMs), uploadRate);
                                }

                                fclose(uploadFile);
                                uploadFile = NULL;
                                if (uploadFileInfo.fileLength == uploadFileTotalSize) {
                                    if (memcmp(uploadFileInfo.md5Buf, uploadFileMd5, sizeof(uploadFileMd5)) == 0) {
                                        USER_LOG_DEBUG(
                                            "[File-Service] [Client:%d] upload file md5 check success", clientNum);
                                        s_fileServiceContent[clientNum].uploadState = MOP_FILE_SERVICE_UPLOAD_FINISHED_SUCCESS;
                                        s_fileServiceContent[clientNum].uploadSeqNum = fileTransfor->seqNum;
                                    } else {
                                        USER_LOG_ERROR(
                                            "[File-Service] [Client:%d] upload file md5 check failed", clientNum);
                                        s_fileServiceContent[clientNum].uploadState = MOP_FILE_SERVICE_UPLOAD_FINISHED_FAILED;
                                        s_fileServiceContent[clientNum].uploadSeqNum = fileTransfor->seqNum;
                                    }
                                } else {
                                    USER_LOG_ERROR(
                                        "[File-Service] [Client:%d] upload file check file length error", clientNum);
                                    s_fileServiceContent[clientNum].uploadState = MOP_FILE_SERVICE_UPLOAD_FINISHED_FAILED;
                                    s_fileServiceContent[clientNum].uploadSeqNum = fileTransfor->seqNum;
                                }
                            }
                        }
                        break;
                    case DJI_MOP_CHANNEL_FILE_TRANSFOR_CMD_RESULT:
                        if (fileTransfor->subcmd == DJI_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_RESULT_OK) {
                            s_fileServiceContent[clientNum].downloadState = MOP_FILE_SERVICE_DOWNLOAD_FINISHED_SUCCESS;
                            USER_LOG_DEBUG("[File-Service] [Client:%d] download file result notify success",
                                           clientNum);
                        } else {
                            s_fileServiceContent[clientNum].downloadState = MOP_FILE_SERVICE_DOWNLOAD_FINISHED_FAILED;
                            USER_LOG_ERROR("[File-Service] [Client:%d] download file result notify failed",
                                           clientNum);
                        }
                        break;
                    case DJI_MOP_CHANNEL_FILE_TRANSFOR_CMD_STOP_REQUEST:
                        if (fileTransfor->subcmd == DJI_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_STOP_UPLOAD) {
                            s_fileServiceContent[clientNum].uploadState = MOP_FILE_SERVICE_UPLOAD_STOP;
                            USER_LOG_DEBUG("[File-Service] [Client:%d] upload file stop", clientNum);
                        } else if (fileTransfor->subcmd == DJI_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_STOP_DOWNLOAD) {
                            s_fileServiceContent[clientNum].downloadState = MOP_FILE_SERVICE_DOWNLOAD_STOP;
                            USER_LOG_DEBUG("[File-Service] [Client:%d] download file stop", clientNum);
                        }
                        break;
                    default:
                        USER_LOG_WARN("[File-Service] [Client:%d] recv the unknown command：0x%02X",
                                      clientNum, fileTransfor->cmd);
                        break;
                }
            }
        }
    }
}

#pragma GCC diagnostic pop

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
