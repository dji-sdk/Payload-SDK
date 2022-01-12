/**
 ********************************************************************
 * @file    test_mop_channel.c
 * @version V2.0.0
 * @date    1/14/20
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
#if PSDK_ARCH_SYS_LINUX

#include <utils/util_misc.h>
#include <stdio.h>
#include <utils/util_md5.h>
#include "psdk_mop_channel.h"
#include "psdk_logger.h"
#include "psdk_platform.h"
#include "test_mop_channel.h"

/* Private constants ---------------------------------------------------------*/
#define PSDK_MOP_CHANNEL_TASK_STACK_SIZE                         2048
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
#define TEST_MOP_CHANNEL_FILE_SERVICE_FILE_PATH                  "../../../../../api_sample/mop_channel/mop_channel_test_file/mop_send_test_file.mp4"

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
    T_PsdkTaskHandle clientRecvTask;
    T_PsdkTaskHandle clientSendTask;
    T_PsdkMopChannelHandle clientHandle;
    E_MopFileServiceDownloadState downloadState;
    uint16_t downloadSeqNum;
    E_MopFileServiceUploadState uploadState;
    uint16_t uploadSeqNum;
} T_MopFileServiceClientContent;

/* Private values -------------------------------------------------------------*/
static T_PsdkMopChannelHandle s_testMopChannelNormalHandle;
static T_PsdkMopChannelHandle s_testMopChannelNormalOutHandle;
static T_PsdkTaskHandle s_testMopChannelNormalSendTask;
static T_PsdkTaskHandle s_testMopChannelNormalRecvTask;
static T_PsdkSemHandle s_testMopChannelReadySema;
static bool s_testMopChannelConnected = false;

static T_PsdkTaskHandle s_fileServiceMopChannelAcceptTask;
static T_PsdkMopChannelHandle s_fileServiceMopChannelHandle;
static T_MopFileServiceClientContent s_fileServiceContent[TEST_MOP_CHANNEL_FILE_SERVICE_CLIENT_MAX_SUPPORT_NUM];

/* Private functions declaration ---------------------------------------------*/
static void *PsdkTest_MopChannelSendNormalTask(void *arg);
static void *PsdkTest_MopChannelRecvNormalTask(void *arg);
static void *PsdkTest_MopChannelFileServiceAcceptTask(void *arg);
static void *PsdkTest_MopChannelFileServiceRecvTask(void *arg);
static void *PsdkTest_MopChannelFileServiceSendTask(void *arg);

/* Exported functions definition ---------------------------------------------*/
T_PsdkReturnCode PsdkTest_MopChannelInit(void)
{
    T_PsdkReturnCode returnCode;

    returnCode = PsdkOsal_SemaphoreCreate(&s_testMopChannelReadySema, 0);
    if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("mop channel create msdk sema error, stat:0x%08llX.", returnCode);
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    returnCode = PsdkMopChannel_Init();
    if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("mop channel init error, stat:0x%08llX.", returnCode);
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    returnCode = PsdkOsal_TaskCreate(&s_testMopChannelNormalSendTask, PsdkTest_MopChannelSendNormalTask,
                                     "mop_msdk_send_task",
                                     PSDK_MOP_CHANNEL_TASK_STACK_SIZE, NULL);
    if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("mop channel msdk send task create error, stat:0x%08llX.", returnCode);
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    returnCode = PsdkOsal_TaskCreate(&s_testMopChannelNormalRecvTask, PsdkTest_MopChannelRecvNormalTask,
                                     "mop_msdk_recv_task", PSDK_MOP_CHANNEL_TASK_STACK_SIZE, NULL);
    if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("mop channel msdk recv task create error, stat:0x%08llX.", returnCode);
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    returnCode = PsdkOsal_TaskCreate(&s_fileServiceMopChannelAcceptTask, PsdkTest_MopChannelFileServiceAcceptTask,
                                     "mop_msdk_accept_task", PSDK_MOP_CHANNEL_TASK_STACK_SIZE, NULL);
    if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("mop channel osdk recv task create error, stat:0x%08llX.", returnCode);
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/* Private functions definition-----------------------------------------------*/
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wreturn-type"

static void *PsdkTest_MopChannelSendNormalTask(void *arg)
{
    uint8_t *sendBuf = NULL;
    uint32_t realLen = 0;
    T_PsdkReturnCode returnCode;
    uint32_t sendDataCount = 0;

    USER_UTIL_UNUSED(arg);

    PsdkOsal_TaskSleepMs(TEST_MOP_CHANNEL_INIT_TIMEMS);

    sendBuf = PsdkOsal_Malloc(TEST_MOP_CHANNEL_NORMAL_TRANSFOR_SEND_BUFFER);
    if (sendBuf == NULL) {
        PsdkLogger_UserLogError("malloc send buffer error");
        return NULL;
    }

REWAIT:
    returnCode = PsdkOsal_SemaphoreWait(s_testMopChannelReadySema);
    if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("mop channel wait sema error, stat:0x%08llX.", returnCode);
        return NULL;
    }

    while (1) {
        if (s_testMopChannelConnected == false) {
            sendDataCount = 0;
            goto REWAIT;
        }

        sendDataCount++;
        memset(sendBuf, sendDataCount, TEST_MOP_CHANNEL_NORMAL_TRANSFOR_SEND_BUFFER);
        returnCode = PsdkMopChannel_SendData(s_testMopChannelNormalOutHandle, sendBuf,
                                             TEST_MOP_CHANNEL_NORMAL_TRANSFOR_SEND_BUFFER, &realLen);
        if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            PsdkLogger_UserLogError("mop channel send data to channel error,stat:0x%08llX", returnCode);
        } else {
            PsdkLogger_UserLogInfo("mop channel send data to channel length:%d count:%d", realLen, sendDataCount);
        }

        PsdkOsal_TaskSleepMs(1000 / TEST_MOP_CHANNEL_NORMAL_TRANSFOR_SEND_TASK_FREQ);
    }
}

#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wreturn-type"

static void *PsdkTest_MopChannelRecvNormalTask(void *arg)
{
    USER_UTIL_UNUSED(arg);
    uint8_t *recvBuf = NULL;
    uint32_t realLen;
    T_PsdkReturnCode returnCode;
    uint32_t recvDataCount = 0;

    PsdkOsal_TaskSleepMs(TEST_MOP_CHANNEL_INIT_TIMEMS);

#if TEST_MOP_CHANNEL_NORMAL_TRANSFOR_USING_RELIABLE_TRANS
    returnCode = PsdkMopChannel_Create(&s_testMopChannelNormalHandle, PSDK_MOP_CHANNEL_TRANS_RELIABLE);
    if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("mop channel create send handle error, stat:0x%08llX.", returnCode);
        return NULL;
    }
#else
    returnCode = PsdkMopChannel_Create(&s_testMopChannelNormalHandle, PSDK_MOP_CHANNEL_TRANS_UNRELIABLE);
    if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("mop channel create send handle error, stat:0x%08llX.", returnCode);
        return NULL;
    }
#endif

REBIND:
    returnCode = PsdkMopChannel_Bind(s_testMopChannelNormalHandle, TEST_MOP_CHANNEL_NORMAL_TRANSFOR_CHANNEL_ID);
    if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("mop bind channel error :0x%08llX", returnCode);
        PsdkOsal_TaskSleepMs(TEST_MOP_CHANNEL_RETRY_TIMEMS);
        goto REBIND;
    }

REACCEPT:
    returnCode = PsdkMopChannel_Accept(s_testMopChannelNormalHandle, &s_testMopChannelNormalOutHandle);
    if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogWarn("mop accept channel error :0x%08llX", returnCode);
        PsdkOsal_TaskSleepMs(TEST_MOP_CHANNEL_RETRY_TIMEMS);
        goto REACCEPT;
    }

    PsdkLogger_UserLogInfo("mop channel is connected");

    recvBuf = PsdkOsal_Malloc(TEST_MOP_CHANNEL_NORMAL_TRANSFOR_RECV_BUFFER);
    if (recvBuf == NULL) {
        PsdkLogger_UserLogError("malloc recv buffer error");
        return NULL;
    }

    s_testMopChannelConnected = true;

    returnCode = PsdkOsal_SemaphorePost(s_testMopChannelReadySema);
    if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("mop channel post sema error, stat:0x%08llX.", returnCode);
        return NULL;
    }

    while (1) {
        memset(recvBuf, 0, TEST_MOP_CHANNEL_NORMAL_TRANSFOR_RECV_BUFFER);

        returnCode = PsdkMopChannel_RecvData(s_testMopChannelNormalOutHandle, recvBuf,
                                             TEST_MOP_CHANNEL_NORMAL_TRANSFOR_RECV_BUFFER, &realLen);
        if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            if (returnCode == PSDK_ERROR_MOP_CHANNEL_MODULE_CODE_CONNECTION_CLOSE) {
                PsdkLogger_UserLogInfo("mop channel is disconnected");
                s_testMopChannelConnected = false;
                PsdkOsal_TaskSleepMs(TEST_MOP_CHANNEL_RETRY_TIMEMS);
                PsdkMopChannel_Close(s_testMopChannelNormalOutHandle);
                PsdkMopChannel_Destroy(s_testMopChannelNormalOutHandle);
                goto REACCEPT;
            }
        } else {
            PsdkLogger_UserLogInfo("mop channel recv data from channel length:%d count:%d", realLen, recvDataCount++);
        }
    }
}

#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wreturn-type"

static void *PsdkTest_MopChannelFileServiceAcceptTask(void *arg)
{
    T_PsdkReturnCode returnCode;
    uint8_t currentClientNum = 0;

    USER_UTIL_UNUSED(arg);

    PsdkLogger_UserLogDebug("[File-Service] Start the file service.");

    returnCode = PsdkMopChannel_Create(&s_fileServiceMopChannelHandle, PSDK_MOP_CHANNEL_TRANS_RELIABLE);
    if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("[File-Service] mop channel create send handle error, stat:0x%08llX.", returnCode);
        return NULL;
    }

REBIND:
    returnCode = PsdkMopChannel_Bind(s_fileServiceMopChannelHandle, TEST_MOP_CHANNEL_FILE_SERVICE_CHANNEL_ID);
    if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("[File-Service] mop bind channel error :0x%08llX", returnCode);
        PsdkOsal_TaskSleepMs(TEST_MOP_CHANNEL_RETRY_TIMEMS);
        goto REBIND;
    }

    while (1) {
REACCEPT:
        returnCode = PsdkMopChannel_Accept(s_fileServiceMopChannelHandle,
                                           &s_fileServiceContent[currentClientNum].clientHandle);
        if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            PsdkLogger_UserLogWarn("[File-Service] mop accept channel error :0x%08llX", returnCode);
            PsdkOsal_TaskSleepMs(TEST_MOP_CHANNEL_RETRY_TIMEMS);
            goto REACCEPT;
        }

        PsdkLogger_UserLogInfo("[File-Service] [Client:%d] mop channel is connected", currentClientNum);

        s_fileServiceContent[currentClientNum].index = currentClientNum;
        returnCode = PsdkOsal_TaskCreate(&s_fileServiceContent[currentClientNum].clientRecvTask,
                                         PsdkTest_MopChannelFileServiceRecvTask,
                                         "mop_file_service_recv_task",
                                         PSDK_MOP_CHANNEL_TASK_STACK_SIZE,
                                         &s_fileServiceContent[currentClientNum].index);
        if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            PsdkLogger_UserLogError("mop channel recv task create error, stat:0x%08llX.", returnCode);
            return NULL;
        }

        returnCode = PsdkOsal_TaskCreate(&s_fileServiceContent[currentClientNum].clientSendTask,
                                         PsdkTest_MopChannelFileServiceSendTask,
                                         "mop_file_service_send_task",
                                         PSDK_MOP_CHANNEL_TASK_STACK_SIZE,
                                         &s_fileServiceContent[currentClientNum].index);
        if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            PsdkLogger_UserLogError("mop channel send task create error, stat:0x%08llX.", returnCode);
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

static void *PsdkTest_MopChannelFileServiceSendTask(void *arg)
{
    T_PsdkReturnCode returnCode;
    uint8_t clientNum = *(uint8_t *) arg;
    uint32_t sendRealLen = 0;
    uint8_t *sendBuf;
    MD5_CTX downloadFileMd5Ctx;
    FILE *downloadFile = NULL;
    uint8_t downloadFileMd5[PSDK_MD5_BUFFER_LEN] = {0};
    uint64_t downloadFileTotalSize = 0;
    uint64_t downloadWriteLen;
    uint16_t downloadPackCount = 0;
    T_PsdkMopChannel_FileInfo downloadFileInfo = {0};
    T_PsdkMopChannel_FileTransfor transforAck = {0};
    T_PsdkMopChannel_FileTransfor fileData = {0};
    T_PsdkMopChannel_FileTransfor fileInfo = {0};
    uint32_t downloadStartMs = 0;
    uint32_t downloadEndMs = 0;
    uint32_t downloadDurationMs;
    psdk_f32_t downloadRate;

    sendBuf = PsdkOsal_Malloc(TEST_MOP_CHANNEL_FILE_SERVICE_SEND_BUFFER);
    if (sendBuf == NULL) {
        PsdkLogger_UserLogError("[File-Service] [Client:%d] malloc send buffer error", clientNum);
        return NULL;
    }

    while (1) {
        switch (s_fileServiceContent[clientNum].uploadState) {
            case MOP_FILE_SERVICE_UPLOAD_REQUEST_START:
                transforAck.cmd = PSDK_MOP_CHANNEL_FILE_TRANSFOR_CMD_ACK;
                transforAck.subcmd = PSDK_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_ACK_OK;
                transforAck.seqNum = s_fileServiceContent[clientNum].uploadSeqNum;
                transforAck.dataLen = 0;
                PsdkMopChannel_SendData(s_fileServiceContent[clientNum].clientHandle, (uint8_t *) &transforAck,
                                        UTIL_OFFSETOF(T_PsdkMopChannel_FileTransfor, data),
                                        &sendRealLen);
                PsdkLogger_UserLogDebug("[File-Service] [Client:%d] upload request ack", clientNum);
                s_fileServiceContent[clientNum].uploadState = MOP_FILE_SERVICE_UPLOAD_IDEL;
                break;
            case MOP_FILE_SERVICE_UPLOAD_FILE_INFO_SUCCESS:
                transforAck.cmd = PSDK_MOP_CHANNEL_FILE_TRANSFOR_CMD_ACK;
                transforAck.subcmd = PSDK_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_ACK_OK;
                transforAck.seqNum = s_fileServiceContent[clientNum].uploadSeqNum;
                transforAck.dataLen = 0;
                PsdkMopChannel_SendData(s_fileServiceContent[clientNum].clientHandle, (uint8_t *) &transforAck,
                                        UTIL_OFFSETOF(T_PsdkMopChannel_FileTransfor, data),
                                        &sendRealLen);
                PsdkLogger_UserLogDebug("[File-Service] [Client:%d] upload file info success", clientNum);
                s_fileServiceContent[clientNum].uploadState = MOP_FILE_SERVICE_UPLOAD_IDEL;
                break;
            case MOP_FILE_SERVICE_UPLOAD_FILE_INFO_FAILED:
                transforAck.cmd = PSDK_MOP_CHANNEL_FILE_TRANSFOR_CMD_ACK;
                transforAck.subcmd = PSDK_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_ACK_REJECTED;
                transforAck.seqNum = s_fileServiceContent[clientNum].uploadSeqNum;
                transforAck.dataLen = 0;
                PsdkMopChannel_SendData(s_fileServiceContent[clientNum].clientHandle, (uint8_t *) &transforAck,
                                        UTIL_OFFSETOF(T_PsdkMopChannel_FileTransfor, data),
                                        &sendRealLen);
                PsdkLogger_UserLogError("[File-Service] [Client:%d] upload file info failed", clientNum);
                s_fileServiceContent[clientNum].uploadState = MOP_FILE_SERVICE_UPLOAD_IDEL;
                break;
            case MOP_FILE_SERVICE_UPLOAD_FINISHED_SUCCESS:
                transforAck.cmd = PSDK_MOP_CHANNEL_FILE_TRANSFOR_CMD_RESULT;
                transforAck.subcmd = PSDK_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_RESULT_OK;
                transforAck.seqNum = s_fileServiceContent[clientNum].uploadSeqNum++;
                transforAck.dataLen = 0;
                PsdkMopChannel_SendData(s_fileServiceContent[clientNum].clientHandle,
                                        (uint8_t *) &transforAck,
                                        UTIL_OFFSETOF(T_PsdkMopChannel_FileTransfor, data),
                                        &sendRealLen);
                PsdkLogger_UserLogDebug("[File-Service] [Client:%d] upload finished success", clientNum);
                s_fileServiceContent[clientNum].uploadState = MOP_FILE_SERVICE_UPLOAD_IDEL;

                break;
            case MOP_FILE_SERVICE_UPLOAD_FINISHED_FAILED:
                transforAck.cmd = PSDK_MOP_CHANNEL_FILE_TRANSFOR_CMD_RESULT;
                transforAck.subcmd = PSDK_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_RESULT_FAILED;
                transforAck.seqNum = s_fileServiceContent[clientNum].uploadSeqNum++;
                transforAck.dataLen = 0;
                PsdkMopChannel_SendData(s_fileServiceContent[clientNum].clientHandle,
                                        (uint8_t *) &transforAck,
                                        UTIL_OFFSETOF(T_PsdkMopChannel_FileTransfor, data),
                                        &sendRealLen);
                PsdkLogger_UserLogError("[File-Service] [Client:%d] upload finished failed", clientNum);
                s_fileServiceContent[clientNum].uploadState = MOP_FILE_SERVICE_UPLOAD_IDEL;
                break;
            case MOP_FILE_SERVICE_UPLOAD_STOP:
                transforAck.cmd = PSDK_MOP_CHANNEL_FILE_TRANSFOR_CMD_STOP_ACK;
                transforAck.subcmd = PSDK_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_STOP_UPLOAD;
                transforAck.seqNum = s_fileServiceContent[clientNum].uploadSeqNum++;
                transforAck.dataLen = 0;
                PsdkMopChannel_SendData(s_fileServiceContent[clientNum].clientHandle,
                                        (uint8_t *) &transforAck,
                                        UTIL_OFFSETOF(T_PsdkMopChannel_FileTransfor, data),
                                        &sendRealLen);
                s_fileServiceContent[clientNum].uploadState = MOP_FILE_SERVICE_UPLOAD_IDEL;
                break;
            default:
                break;
        }

        switch (s_fileServiceContent[clientNum].downloadState) {
            case MOP_FILE_SERVICE_DOWNLOAD_REQUEST_START:
                transforAck.cmd = PSDK_MOP_CHANNEL_FILE_TRANSFOR_CMD_ACK;
                transforAck.subcmd = PSDK_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_ACK_OK;
                transforAck.seqNum = s_fileServiceContent[clientNum].downloadSeqNum;
                transforAck.dataLen = 0;
                PsdkMopChannel_SendData(s_fileServiceContent[clientNum].clientHandle, (uint8_t *) &transforAck,
                                        UTIL_OFFSETOF(T_PsdkMopChannel_FileTransfor, data),
                                        &sendRealLen);
                PsdkLogger_UserLogDebug("[File-Service] [Client:%d] download request ack", clientNum);
                s_fileServiceContent[clientNum].downloadState = MOP_FILE_SERVICE_DOWNLOAD_IDEL;

                break;
            case MOP_FILE_SERVICE_DOWNLOAD_STOP:
                transforAck.cmd = PSDK_MOP_CHANNEL_FILE_TRANSFOR_CMD_STOP_ACK;
                transforAck.subcmd = PSDK_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_STOP_DOWNLOAD;
                transforAck.seqNum = s_fileServiceContent[clientNum].uploadSeqNum++;
                transforAck.dataLen = 0;
                PsdkMopChannel_SendData(s_fileServiceContent[clientNum].clientHandle,
                                        (uint8_t *) &transforAck,
                                        UTIL_OFFSETOF(T_PsdkMopChannel_FileTransfor, data),
                                        &sendRealLen);
                s_fileServiceContent[clientNum].downloadState = MOP_FILE_SERVICE_DOWNLOAD_IDEL;
                break;
            case MOP_FILE_SERVICE_DOWNLOAD_FILE_INFO_SUCCESS:
                UtilMd5_Init(&downloadFileMd5Ctx);
                PsdkOsal_GetTimeMs(&downloadStartMs);
                if (downloadFile != NULL) {
                    fclose(downloadFile);
                }
                downloadFile = fopen(TEST_MOP_CHANNEL_FILE_SERVICE_FILE_PATH, "rb");
                if (downloadFile == NULL) {
                    PsdkLogger_UserLogError("[File-Service] [Client:%d] download open file error",
                                            clientNum);
                    return NULL;
                }

                downloadFileTotalSize = 0;
                while (1) {
                    returnCode = fseek(downloadFile, downloadFileTotalSize, SEEK_SET);
                    if (returnCode != 0) {
                        PsdkLogger_UserLogError(
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

                fileInfo.cmd = PSDK_MOP_CHANNEL_FILE_TRANSFOR_CMD_FILE_INFO;
                fileInfo.subcmd = PSDK_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_DOWNLOAD_REQUEST;
                fileInfo.seqNum = s_fileServiceContent[clientNum].downloadSeqNum;
                fileInfo.dataLen = sizeof(fileInfo) - UTIL_OFFSETOF(T_PsdkMopChannel_FileTransfor, data);

                fileInfo.data.fileInfo.isExist = true;
                downloadFileInfo.fileLength = downloadFileTotalSize;
                fileInfo.data.fileInfo.fileLength = downloadFileTotalSize;
                strcpy(fileInfo.data.fileInfo.fileName, "test.mp4");
                memcpy(&fileInfo.data.fileInfo.md5Buf, &downloadFileMd5, sizeof(downloadFileMd5));
                PsdkMopChannel_SendData(s_fileServiceContent[clientNum].clientHandle, (uint8_t *) &fileInfo,
                                        sizeof(T_PsdkMopChannel_FileTransfor),
                                        &sendRealLen);
                PsdkLogger_UserLogDebug(
                    "[File-Service] [Client:%d] download ack file info exist:%d length:%d name:%s",
                    clientNum, fileInfo.data.fileInfo.isExist,
                    fileInfo.data.fileInfo.fileLength, fileInfo.data.fileInfo.fileName);

                s_fileServiceContent[clientNum].downloadState = MOP_FILE_SERVICE_DOWNLOAD_DATA_SENDING;
                downloadFileTotalSize = 0;
                downloadPackCount = 0;
                break;
            case MOP_FILE_SERVICE_DOWNLOAD_DATA_SENDING:
                if (downloadFile == NULL) {
                    PsdkLogger_UserLogError("[File-Service] [Client:%d] download file object is NULL.");
                    break;
                }

                returnCode = fseek(downloadFile, downloadFileTotalSize, SEEK_SET);
                if (returnCode != 0) {
                    PsdkLogger_UserLogError("[File-Service] [Client:%d] download fseek file data fail.",
                                            clientNum);
                    break;
                }
                downloadWriteLen = fread(&sendBuf[UTIL_OFFSETOF(T_PsdkMopChannel_FileTransfor, data)],
                                         1,
                                         (TEST_MOP_CHANNEL_FILE_SERVICE_SEND_BUFFER -
                                          UTIL_OFFSETOF(T_PsdkMopChannel_FileTransfor, data)),
                                         downloadFile);

                if (downloadWriteLen > 0) {
                    downloadFileTotalSize += downloadWriteLen;
                    downloadPackCount++;

                    fileData.cmd = PSDK_MOP_CHANNEL_FILE_TRANSFOR_CMD_FILE_DATA;
                    fileData.dataLen = downloadWriteLen;
                    fileData.seqNum++;
                    if (downloadWriteLen ==
                        (TEST_MOP_CHANNEL_FILE_SERVICE_SEND_BUFFER -
                         UTIL_OFFSETOF(T_PsdkMopChannel_FileTransfor, data))) {
                        fileData.subcmd = PSDK_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_FILE_DATA_NORMAL;
                    } else {
                        fileData.subcmd = PSDK_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_FILE_DATA_END;
                    }

                    memcpy(sendBuf, &fileData, UTIL_OFFSETOF(T_PsdkMopChannel_FileTransfor, data));
                    returnCode = PsdkMopChannel_SendData(s_fileServiceContent[clientNum].clientHandle, sendBuf,
                                                         (downloadWriteLen +
                                                          UTIL_OFFSETOF(T_PsdkMopChannel_FileTransfor, data)),
                                                         &sendRealLen);
                    if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                        PsdkLogger_UserLogError(
                            "[File-Service] [Client:%d] download send file data error,stat:0x%08llX",
                            clientNum, returnCode);
                        if (returnCode == PSDK_ERROR_MOP_CHANNEL_MODULE_CODE_CONNECTION_CLOSE) {
                            break;
                        }
                    } else {
                        PsdkLogger_UserLogInfo(
                            "[File-Service] [Client:%d] download send file data length:%d count:%d total:%d percent: %.1f %%",
                            clientNum, sendRealLen, downloadPackCount, downloadFileTotalSize,
                            (psdk_f32_t) (downloadFileTotalSize) * 100 / (psdk_f32_t) downloadFileInfo.fileLength);
                    }

                    if (fileData.subcmd == PSDK_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_FILE_DATA_END) {
                        PsdkOsal_GetTimeMs(&downloadEndMs);
                        downloadDurationMs = downloadEndMs - downloadStartMs;
                        if (downloadDurationMs != 0) {
                            downloadRate = (psdk_f32_t) downloadFileInfo.fileLength * 1000 /
                                           (psdk_f32_t) (downloadDurationMs);
                            PsdkLogger_UserLogInfo(
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

static void *PsdkTest_MopChannelFileServiceRecvTask(void *arg)
{
    T_PsdkReturnCode returnCode;
    uint8_t clientNum = *(uint8_t *) arg;
    uint32_t recvRealLen;
    uint8_t *recvBuf;
    MD5_CTX uploadFileMd5Ctx;
    FILE *uploadFile = NULL;
    uint8_t uploadFileMd5[PSDK_MD5_BUFFER_LEN] = {0};
    uint32_t uploadFileTotalSize = 0;
    int32_t uploadWriteLen;
    T_PsdkMopChannel_FileInfo uploadFileInfo = {0};
    uint32_t uploadStartMs = 0;
    uint32_t uploadEndMs = 0;
    psdk_f32_t uploadRate;

    recvBuf = PsdkOsal_Malloc(TEST_MOP_CHANNEL_FILE_SERVICE_RECV_BUFFER);
    if (recvBuf == NULL) {
        PsdkLogger_UserLogError("[File-Service] [Client:%d] malloc recv buffer error", clientNum);
        return NULL;
    }

    s_fileServiceContent[clientNum].uploadState = MOP_FILE_SERVICE_UPLOAD_IDEL;
    s_fileServiceContent[clientNum].downloadState = MOP_FILE_SERVICE_DOWNLOAD_IDEL;

    while (1) {
        returnCode = PsdkMopChannel_RecvData(s_fileServiceContent[clientNum].clientHandle, recvBuf,
                                             TEST_MOP_CHANNEL_FILE_SERVICE_RECV_BUFFER, &recvRealLen);
        if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            PsdkOsal_TaskSleepMs(1000);
            if (returnCode == PSDK_ERROR_MOP_CHANNEL_MODULE_CODE_CONNECTION_CLOSE) {
                PsdkLogger_UserLogInfo("[File-Service] [Client:%d] mop channel is disconnected", clientNum);
                PsdkOsal_TaskDestroy(s_fileServiceContent[clientNum].clientRecvTask);
                PsdkMopChannel_Close(s_fileServiceContent[clientNum].clientHandle);
                PsdkMopChannel_Destroy(s_fileServiceContent[clientNum].clientHandle);
            }
        } else {
            if (&recvRealLen > 0) {
                T_PsdkMopChannel_FileTransfor *fileTransfor = (T_PsdkMopChannel_FileTransfor *) recvBuf;

                switch (fileTransfor->cmd) {
                    case PSDK_MOP_CHANNEL_FILE_TRANSFOR_CMD_REQUEST:
                        if (fileTransfor->subcmd == PSDK_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_REQUEST_UPLOAD) {
                            s_fileServiceContent[clientNum].uploadState = MOP_FILE_SERVICE_UPLOAD_REQUEST_START;
                            s_fileServiceContent[clientNum].uploadSeqNum = fileTransfor->seqNum;

                            PsdkLogger_UserLogDebug("[File-Service] [Client:%d] upload request is ok", clientNum);
                            UtilMd5_Init(&uploadFileMd5Ctx);
                            uploadFileTotalSize = 0;
                            PsdkOsal_GetTimeMs(&uploadStartMs);
                        } else if (fileTransfor->subcmd == PSDK_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_REQUEST_DOWNLOAD) {
                            s_fileServiceContent[clientNum].downloadState = MOP_FILE_SERVICE_DOWNLOAD_REQUEST_START;
                            s_fileServiceContent[clientNum].downloadSeqNum = fileTransfor->seqNum;

                            PsdkLogger_UserLogDebug("[File-Service] [Client:%d] download request is ok", clientNum);
                        }
                        break;
                    case PSDK_MOP_CHANNEL_FILE_TRANSFOR_CMD_FILE_DOWNLOAD_REQ:
                        PsdkLogger_UserLogDebug("[File-Service] [Client:%d] download request file name:%s", clientNum,
                                                fileTransfor->data.dwonloadReq.fileName);

                        if (strcmp(fileTransfor->data.dwonloadReq.fileName, "test.mp4") == 0) {
                            s_fileServiceContent[clientNum].downloadState = MOP_FILE_SERVICE_DOWNLOAD_FILE_INFO_SUCCESS;
                            s_fileServiceContent[clientNum].downloadSeqNum = fileTransfor->seqNum;
                        } else {
                            s_fileServiceContent[clientNum].downloadState = MOP_FILE_SERVICE_DOWNLOAD_FILE_INFO_FAILED;
                            s_fileServiceContent[clientNum].downloadSeqNum = fileTransfor->seqNum;
                        }
                        break;
                    case PSDK_MOP_CHANNEL_FILE_TRANSFOR_CMD_FILE_INFO:
                        PsdkLogger_UserLogDebug(
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
                            PsdkLogger_UserLogError("[File-Service] [Client:%d] open file error", clientNum);
                            return NULL;
                        }

                        s_fileServiceContent[clientNum].uploadState = MOP_FILE_SERVICE_UPLOAD_FILE_INFO_SUCCESS;
                        s_fileServiceContent[clientNum].uploadSeqNum = fileTransfor->seqNum;

                        break;
                    case PSDK_MOP_CHANNEL_FILE_TRANSFOR_CMD_FILE_DATA:
                        if (uploadFile == NULL) {
                            PsdkLogger_UserLogError("[File-Service] [Client:%d] open file error", clientNum);
                            return NULL;
                        }

                        if (fileTransfor->subcmd == PSDK_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_FILE_DATA_NORMAL) {
                            s_fileServiceContent[clientNum].uploadState = MOP_FILE_SERVICE_UPLOAD_DATA_SENDING;
                            s_fileServiceContent[clientNum].uploadSeqNum = fileTransfor->seqNum;

                            uploadWriteLen = fwrite(&recvBuf[UTIL_OFFSETOF(T_PsdkMopChannel_FileTransfor, data)], 1,
                                                    fileTransfor->dataLen, uploadFile);
                            if (uploadWriteLen < 0) {
                                PsdkLogger_UserLogError(
                                    "[File-Service] [Client:%d] upload write normal data to file error, stat:%d.",
                                    clientNum,
                                    uploadWriteLen);
                            } else {
                                uploadFileTotalSize += uploadWriteLen;
                                UtilMd5_Update(&uploadFileMd5Ctx,
                                               &recvBuf[UTIL_OFFSETOF(T_PsdkMopChannel_FileTransfor, data)],
                                               fileTransfor->dataLen);

                                if (uploadFileInfo.fileLength != 0) {
                                    PsdkLogger_UserLogInfo(
                                        "[File-Service] [Client:%d] upload write data to file success, len:%d  percent:%.1f %%",
                                        clientNum, uploadWriteLen,
                                        (psdk_f32_t) (uploadFileTotalSize * 100) /
                                        (psdk_f32_t) uploadFileInfo.fileLength);
                                }
                            }
                        } else if (fileTransfor->subcmd == PSDK_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_FILE_DATA_END) {
                            s_fileServiceContent[clientNum].uploadState = MOP_FILE_SERVICE_UPLOAD_DATA_SENDING;
                            s_fileServiceContent[clientNum].uploadSeqNum = fileTransfor->seqNum;
                            uploadWriteLen = fwrite(&recvBuf[UTIL_OFFSETOF(T_PsdkMopChannel_FileTransfor, data)], 1,
                                                    fileTransfor->dataLen, uploadFile);
                            if (uploadWriteLen < 0) {
                                PsdkLogger_UserLogError(
                                    "[File-Service] [Client:%d] upload write end data to file error, stat:%d.",
                                    clientNum,
                                    uploadWriteLen);
                            } else {
                                uploadFileTotalSize += uploadWriteLen;
                                UtilMd5_Update(&uploadFileMd5Ctx,
                                               &recvBuf[UTIL_OFFSETOF(T_PsdkMopChannel_FileTransfor, data)],
                                               fileTransfor->dataLen);
                                UtilMd5_Final(&uploadFileMd5Ctx, uploadFileMd5);
                                PsdkOsal_GetTimeMs(&uploadEndMs);

                                if (uploadEndMs - uploadStartMs > 0) {
                                    uploadRate = (psdk_f32_t) uploadFileTotalSize * 1000 /
                                                 (psdk_f32_t) (uploadEndMs - uploadStartMs);
                                    PsdkLogger_UserLogInfo(
                                        "[File-Service] [Client:%d] upload write data to file success, len:%d  percent:%.1f %%",
                                        clientNum, uploadWriteLen,
                                        (psdk_f32_t) (uploadFileTotalSize * 100) /
                                        (psdk_f32_t) uploadFileInfo.fileLength);
                                    PsdkLogger_UserLogInfo(
                                        "[File-Service] [Client:%d] upload file finished, totalTime:%d ms rate:%.2f Byte/s",
                                        clientNum, (uploadEndMs - uploadStartMs), uploadRate);
                                }

                                fclose(uploadFile);
                                uploadFile = NULL;
                                if (uploadFileInfo.fileLength == uploadFileTotalSize) {
                                    if (memcmp(uploadFileInfo.md5Buf, uploadFileMd5, sizeof(uploadFileMd5)) == 0) {
                                        PsdkLogger_UserLogDebug(
                                            "[File-Service] [Client:%d] upload file md5 check success", clientNum);
                                        s_fileServiceContent[clientNum].uploadState = MOP_FILE_SERVICE_UPLOAD_FINISHED_SUCCESS;
                                        s_fileServiceContent[clientNum].uploadSeqNum = fileTransfor->seqNum;
                                    } else {
                                        PsdkLogger_UserLogError(
                                            "[File-Service] [Client:%d] upload file md5 check failed", clientNum);
                                        s_fileServiceContent[clientNum].uploadState = MOP_FILE_SERVICE_UPLOAD_FINISHED_FAILED;
                                        s_fileServiceContent[clientNum].uploadSeqNum = fileTransfor->seqNum;
                                    }
                                } else {
                                    PsdkLogger_UserLogError(
                                        "[File-Service] [Client:%d] upload file check file length error", clientNum);
                                    s_fileServiceContent[clientNum].uploadState = MOP_FILE_SERVICE_UPLOAD_FINISHED_FAILED;
                                    s_fileServiceContent[clientNum].uploadSeqNum = fileTransfor->seqNum;
                                }
                            }
                        }
                        break;
                    case PSDK_MOP_CHANNEL_FILE_TRANSFOR_CMD_RESULT:
                        if (fileTransfor->subcmd == PSDK_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_RESULT_OK) {
                            s_fileServiceContent[clientNum].downloadState = MOP_FILE_SERVICE_DOWNLOAD_FINISHED_SUCCESS;
                            PsdkLogger_UserLogDebug("[File-Service] [Client:%d] download file result notify success",
                                                    clientNum);
                        } else {
                            s_fileServiceContent[clientNum].downloadState = MOP_FILE_SERVICE_DOWNLOAD_FINISHED_FAILED;
                            PsdkLogger_UserLogError("[File-Service] [Client:%d] download file result notify failed",
                                                    clientNum);
                        }
                        break;
                    case PSDK_MOP_CHANNEL_FILE_TRANSFOR_CMD_STOP_REQUEST:
                        if (fileTransfor->subcmd == PSDK_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_STOP_UPLOAD) {
                            s_fileServiceContent[clientNum].uploadState = MOP_FILE_SERVICE_UPLOAD_STOP;
                            PsdkLogger_UserLogDebug("[File-Service] [Client:%d] upload file stop", clientNum);
                        } else if (fileTransfor->subcmd == PSDK_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_STOP_DOWNLOAD) {
                            s_fileServiceContent[clientNum].downloadState = MOP_FILE_SERVICE_DOWNLOAD_STOP;
                            PsdkLogger_UserLogDebug("[File-Service] [Client:%d] download file stop", clientNum);
                        }
                        break;
                    default:
                        PsdkLogger_UserLogWarn("[File-Service] [Client:%d] recv the unknown command：0x%02X",
                                               clientNum, fileTransfor->cmd);
                        break;
                }
            }
        }
    }
}

#pragma GCC diagnostic pop

#endif

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
