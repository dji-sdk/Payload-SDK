#include "test_mop_channel_client.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <libgen.h>

#include "dji_mop_channel.h"
#include "dji_logger.h"
#include "dji_platform.h"
#include "dji_typedef.h"
#include "utils/util_md5.h"

#define MOP_CLIENT_FILE_TRANSFOR_HEADER_LEN  (offsetof(T_MopClientFileTransfor, data))
#define MOP_CLIENT_FILE_DATA_CHUNK_SIZE      (63 * 1024)
#define MOP_CLIENT_ACK_TIMEOUT_MS            3000

#define MOP_CLIENT_TASK_STACK_SIZE          2048
#define MOP_CLIENT_INIT_WAIT_MS             3000
#define MOP_CLIENT_RETRY_WAIT_MS            3000
#define MOP_CLIENT_SEND_INTERVAL_MS         1000
#define MOP_CLIENT_CHANNEL_ID_UNRELIABLE    49152
#define MOP_CLIENT_CHANNEL_ID_RELIABLE      49153
#define MOP_CLIENT_RECV_BUFFER_SIZE         (64 * 1024)

static volatile bool               s_running      = false;
static T_DjiMopChannelHandle       s_conn_handle  = NULL;
static uint16_t                    s_channel_id   = MOP_CLIENT_CHANNEL_ID_UNRELIABLE;
static E_DjiMopChannelTransType    s_trans_type   = DJI_MOP_CHANNEL_TRANS_UNRELIABLE;
static E_DjiChannelAddress         s_channel_addr = DJI_CHANNEL_ADDRESS_PAYLOAD_PORT_NO1;

static T_DjiTaskHandle s_send_task = NULL;
static T_DjiTaskHandle s_recv_task = NULL;

/* Shared ACK signaling between RecvTask -> SendTask */
static volatile uint8_t  s_ack_cmd    = 0;
static volatile uint8_t  s_ack_subcmd = 0;
static volatile bool     s_ack_ready  = false;

/* Wait for ACK from server, returns subcmd or -1 on timeout */
static int WaitAck(T_DjiOsalHandler *osal, uint8_t expectedCmd, uint32_t timeoutMs)
{
    uint32_t elapsed = 0;
    while (elapsed < timeoutMs) {
        if (s_ack_ready && s_ack_cmd == expectedCmd) {
            s_ack_ready = false;
            return s_ack_subcmd;
        }
        osal->TaskSleepMs(10);
        elapsed += 10;
    }
    return -1;
}

static void *SendTask(void *arg)
{
    (void)arg;
    T_DjiOsalHandler *osal = DjiPlatform_GetOsalHandler();
    T_DjiReturnCode rc;
    uint32_t realLen = 0;

    uint32_t bufLen = MOP_CLIENT_FILE_TRANSFOR_HEADER_LEN + MOP_CLIENT_FILE_DATA_CHUNK_SIZE;
    uint8_t *buf = (uint8_t *)osal->Malloc(bufLen);
    if (!buf) {
        USER_LOG_ERROR("[SendTask] malloc failed");
        return NULL;
    }

    /* ------------------------------------------------------------------ */
    /* Unreliable: simple transparent transfer, no handshake, no ACK      */
    /* ------------------------------------------------------------------ */
    if (s_trans_type == DJI_MOP_CHANNEL_TRANS_UNRELIABLE) {
        int seq = 0;
        while (s_running) {
            T_MopClientFileTransfor *frame = (T_MopClientFileTransfor *)buf;
            frame->cmd    = MOP_FILE_TRANSFOR_CMD_FILE_DATA;
            frame->subcmd = MOP_FILE_TRANSFOR_SUBCMD_FILE_DATA_NORMAL;
            frame->seqNum = (uint16_t)seq;
            int dataLen = snprintf((char *)frame->data.fileData,
                                   MOP_CLIENT_FILE_DATA_CHUNK_SIZE,
                                   "hello seq=%d", seq++);
            if (dataLen < 0) dataLen = 0;
            frame->dataLen = (uint32_t)dataLen;
            rc = DjiMopChannel_SendData(s_conn_handle, buf,
                                        MOP_CLIENT_FILE_TRANSFOR_HEADER_LEN + (uint32_t)dataLen,
                                        &realLen);
            if (rc != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_WARN("[SendTask] unreliable send error: 0x%08llX", rc);
            } else {
                USER_LOG_INFO("[SendTask] unreliable seq=%d sent %u bytes", seq - 1, realLen);
            }
            osal->TaskSleepMs(MOP_CLIENT_SEND_INTERVAL_MS);
        }
        osal->Free(buf);
        return NULL;
    }

    /* ------------------------------------------------------------------ */
    /* Reliable: full File-Service upload handshake                        */
    /* ------------------------------------------------------------------ */
    uint16_t seqNum = 0;

    /* Step 1: Open file — locate relative to executable path via /proc/self/exe */
    char exePath[DJI_FILE_PATH_SIZE_MAX] = {};
    char sendFilePath[DJI_FILE_PATH_SIZE_MAX] = {};
    readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);
    /* binary is at <repo>/build-xxx/bin/, test file is at <repo>/samples/sample_c/module_sample/mop_channel/mop_channel_test_file/ */
    snprintf(sendFilePath, sizeof(sendFilePath),
             "%s/../../samples/sample_c/module_sample/mop_channel/mop_channel_test_file/mop_send_test_file.mp4",
             dirname(exePath));

    FILE *fp = fopen(sendFilePath, "rb");
    if (!fp) {
        USER_LOG_ERROR("[SendTask] open file %s failed", sendFilePath);
        osal->Free(buf);
        return NULL;
    }
    fseek(fp, 0, SEEK_END);
    uint32_t fileLen = (uint32_t)ftell(fp);
    fseek(fp, 0, SEEK_SET);
    USER_LOG_INFO("[SendTask] file size: %u bytes", fileLen);

    /* Pre-calculate md5 over the whole file */
    uint8_t fileMd5[MD5_BLOCK_SIZE] = {0};
    {
        MD5_CTX md5Ctx;
        UtilMd5_Init(&md5Ctx);
        uint8_t *md5Buf = (uint8_t *)osal->Malloc(MOP_CLIENT_FILE_DATA_CHUNK_SIZE);
        if (md5Buf) {
            size_t n;
            while ((n = fread(md5Buf, 1, MOP_CLIENT_FILE_DATA_CHUNK_SIZE, fp)) > 0) {
                UtilMd5_Update(&md5Ctx, md5Buf, n);
            }
            UtilMd5_Final(&md5Ctx, fileMd5);
            osal->Free(md5Buf);
        }
        fseek(fp, 0, SEEK_SET);
    }

    /* Step 2: REQUEST(upload) */
    T_MopClientFileTransfor *frame = (T_MopClientFileTransfor *)buf;
    memset(frame, 0, MOP_CLIENT_FILE_TRANSFOR_HEADER_LEN);
    frame->cmd     = MOP_FILE_TRANSFOR_CMD_REQUEST;
    frame->subcmd  = MOP_FILE_TRANSFOR_SUBCMD_REQUEST_UPLOAD;
    frame->seqNum  = seqNum++;
    frame->dataLen = 0;
    rc = DjiMopChannel_SendData(s_conn_handle, buf, MOP_CLIENT_FILE_TRANSFOR_HEADER_LEN, &realLen);
    if (rc != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("[SendTask] send REQUEST failed: 0x%08llX", rc);
        goto done;
    }
    USER_LOG_INFO("[SendTask] REQUEST(upload) sent, waiting ACK...");
    if (WaitAck(osal, MOP_FILE_TRANSFOR_CMD_ACK, MOP_CLIENT_ACK_TIMEOUT_MS) != MOP_FILE_TRANSFOR_SUBCMD_ACK_OK) {
        USER_LOG_ERROR("[SendTask] REQUEST ACK timeout or rejected");
        goto done;
    }
    USER_LOG_INFO("[SendTask] REQUEST ACK ok");

    /* Step 3: FILE_INFO */
    memset(frame, 0, MOP_CLIENT_FILE_TRANSFOR_HEADER_LEN + sizeof(T_MopClientFileInfo));
    frame->cmd     = MOP_FILE_TRANSFOR_CMD_FILE_INFO;
    frame->subcmd  = 0xFF;
    frame->seqNum  = seqNum++;
    frame->dataLen = sizeof(T_MopClientFileInfo);
    frame->data.fileInfo.isExist    = true;
    frame->data.fileInfo.fileLength = fileLen;
    strncpy(frame->data.fileInfo.fileName, "mop_send_test_file.mp4",
            sizeof(frame->data.fileInfo.fileName) - 1);
    memcpy(frame->data.fileInfo.md5Buf, fileMd5, sizeof(fileMd5));
    rc = DjiMopChannel_SendData(s_conn_handle, buf,
                                MOP_CLIENT_FILE_TRANSFOR_HEADER_LEN + sizeof(T_MopClientFileInfo),
                                &realLen);
    if (rc != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("[SendTask] send FILE_INFO failed: 0x%08llX", rc);
        goto done;
    }
    USER_LOG_INFO("[SendTask] FILE_INFO sent, waiting ACK...");
    if (WaitAck(osal, MOP_FILE_TRANSFOR_CMD_ACK, MOP_CLIENT_ACK_TIMEOUT_MS) != MOP_FILE_TRANSFOR_SUBCMD_ACK_OK) {
        USER_LOG_ERROR("[SendTask] FILE_INFO ACK timeout or rejected");
        goto done;
    }
    USER_LOG_INFO("[SendTask] FILE_INFO ACK ok, start sending data...");

    /* Step 4: FILE_DATA chunks */
    {
        uint32_t totalSent = 0;
        while (s_running) {
            uint32_t chunkLen = (uint32_t)fread(frame->data.fileData, 1,
                                                 MOP_CLIENT_FILE_DATA_CHUNK_SIZE, fp);
            if (chunkLen == 0) break;
            totalSent += chunkLen;
            bool isLast = (totalSent >= fileLen) || feof(fp);
            frame->cmd     = MOP_FILE_TRANSFOR_CMD_FILE_DATA;
            frame->subcmd  = isLast ? MOP_FILE_TRANSFOR_SUBCMD_FILE_DATA_END
                                    : MOP_FILE_TRANSFOR_SUBCMD_FILE_DATA_NORMAL;
            frame->seqNum  = seqNum++;
            frame->dataLen = chunkLen;
            rc = DjiMopChannel_SendData(s_conn_handle, buf,
                                        MOP_CLIENT_FILE_TRANSFOR_HEADER_LEN + chunkLen,
                                        &realLen);
            if (rc != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("[SendTask] send FILE_DATA failed: 0x%08llX", rc);
                goto done;
            }
            USER_LOG_INFO("[SendTask] FILE_DATA seq=%u len=%u (%.1f%%)",
                          frame->seqNum, chunkLen,
                          (float)totalSent * 100.0f / (float)fileLen);
            if (isLast) break;
        }
    }

    /* Step 5: Wait RESULT */
    USER_LOG_INFO("[SendTask] all data sent, waiting RESULT...");
    {
        int result = WaitAck(osal, MOP_FILE_TRANSFOR_CMD_RESULT, MOP_CLIENT_ACK_TIMEOUT_MS * 3);
        if (result == MOP_FILE_TRANSFOR_SUBCMD_RESULT_OK) {
            USER_LOG_INFO("[SendTask] file transfer finished successfully");
        } else {
            USER_LOG_ERROR("[SendTask] file transfer failed or timeout, result=%d", result);
        }
    }

done:
    fclose(fp);
    osal->Free(buf);
    s_running = false;
    return NULL;
}

static void *RecvTask(void *arg)
{
    (void)arg;
    T_DjiOsalHandler *osal = DjiPlatform_GetOsalHandler();
    uint8_t *buf = (uint8_t *)osal->Malloc(MOP_CLIENT_RECV_BUFFER_SIZE);
    if (!buf) {
        USER_LOG_ERROR("[RecvTask] malloc failed");
        return NULL;
    }

    while (s_running) {
        uint32_t realLen = 0;
        T_DjiReturnCode rc = DjiMopChannel_RecvData(s_conn_handle, buf,
                                                     MOP_CLIENT_RECV_BUFFER_SIZE, &realLen);
        if (rc == DJI_ERROR_MOP_CHANNEL_MODULE_CODE_CONNECTION_CLOSE) {
            USER_LOG_INFO("[RecvTask] connection closed by remote");
            s_running = false;
            break;
        }
        if (rc == DJI_ERROR_SYSTEM_MODULE_CODE_TIMEOUT) {
            /* No data available yet, normal during FILE_DATA transfer */
            continue;
        }
        if (rc != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_WARN("[RecvTask] recv error: 0x%08llX", rc);
            osal->TaskSleepMs(100);
            continue;
        }
        if (realLen < MOP_CLIENT_FILE_TRANSFOR_HEADER_LEN) {
            USER_LOG_WARN("[RecvTask] frame too short: %u bytes", realLen);
            continue;
        }

        T_MopClientFileTransfor *frame = (T_MopClientFileTransfor *)buf;

        if (s_trans_type == DJI_MOP_CHANNEL_TRANS_UNRELIABLE) {
            /* Unreliable: just print received data */
            USER_LOG_INFO("[RecvTask] unreliable recv seq=%u dataLen=%u: %.*s",
                          frame->seqNum, frame->dataLen,
                          (int)(frame->dataLen > 64 ? 64 : frame->dataLen),
                          (char *)frame->data.fileData);
        } else {
            /* Reliable: forward ACK/RESULT to SendTask */
            USER_LOG_DEBUG("[RecvTask] cmd=0x%02X subcmd=0x%02X seq=%u",
                           frame->cmd, frame->subcmd, frame->seqNum);
            s_ack_cmd    = frame->cmd;
            s_ack_subcmd = frame->subcmd;
            s_ack_ready  = true;
        }
    }

    osal->Free(buf);
    return NULL;
}

static void *ConnectTask(void *arg)
{
    (void)arg;
    T_DjiOsalHandler *osal = DjiPlatform_GetOsalHandler();
    T_DjiReturnCode rc;

    osal->TaskSleepMs(MOP_CLIENT_INIT_WAIT_MS);

    T_DjiMopChannelHandle handle = NULL;
    rc = DjiMopChannel_Create(&handle, s_trans_type);
    if (rc != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("create channel failed: 0x%08llX", rc);
        return NULL;
    }

    /* Connect loop — exits when connected or s_running becomes false */
    while (s_running) {
        USER_LOG_INFO("connecting to addr=%d channel=%u ...", s_channel_addr, s_channel_id);
        rc = DjiMopChannel_Connect(handle, s_channel_addr, s_channel_id);
        if (rc == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            break;
        }
        USER_LOG_WARN("connect failed: 0x%08llX, retrying...", rc);
        osal->TaskSleepMs(MOP_CLIENT_RETRY_WAIT_MS);
    }

    if (!s_running) {
        DjiMopChannel_Destroy(handle);
        return NULL;
    }

    USER_LOG_INFO("connected to addr=%d channel=%u", s_channel_addr, s_channel_id);
    osal->TaskSleepMs(3000);

    if (!s_running) {
        DjiMopChannel_Destroy(handle);
        return NULL;
    }

    s_conn_handle = handle;

    T_DjiReturnCode rc_send = osal->TaskCreate("mop_client_send", SendTask, MOP_CLIENT_TASK_STACK_SIZE,
                                                NULL, &s_send_task);
    T_DjiReturnCode rc_recv = osal->TaskCreate("mop_client_recv", RecvTask, MOP_CLIENT_TASK_STACK_SIZE,
                                                NULL, &s_recv_task);
    if (rc_send != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS ||
        rc_recv != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("TaskCreate send/recv failed: send=0x%08llX recv=0x%08llX", rc_send, rc_recv);
        s_running = false;
        DjiMopChannel_Close(handle);
        DjiMopChannel_Destroy(handle);
        s_conn_handle = NULL;
        return NULL;
    }

    /* Wait for send/recv tasks to stop before closing handle */
    while (s_running) {
        osal->TaskSleepMs(500);
    }
    osal->TaskSleepMs(200);

    /* ConnectTask is sole owner of handle — close and destroy here */
    DjiMopChannel_Close(handle);
    DjiMopChannel_Destroy(handle);
    s_conn_handle = NULL;
    USER_LOG_INFO("disconnected from channel=%u", s_channel_id);

    return NULL;
}

T_DjiReturnCode DjiTest_MopChannelClientRun(void)
{
    T_DjiOsalHandler *osal = DjiPlatform_GetOsalHandler();
    T_DjiReturnCode rc;

    char cmd;
    printf("\n=== MOP Channel Client ===\n"
           "  [1] Unreliable ch=%u -> server [7], addr=payload1\n"
           "  [2] Reliable   ch=%u -> server [8], addr=payload1\n"
           "  [3] Unreliable ch=%u -> server [7], addr=payload2\n"
           "  [4] Reliable   ch=%u -> server [8], addr=payload2\n"
           "  [q] Cancel\n"
           "> ",
           MOP_CLIENT_CHANNEL_ID_UNRELIABLE,
           MOP_CLIENT_CHANNEL_ID_RELIABLE,
           MOP_CLIENT_CHANNEL_ID_UNRELIABLE,
           MOP_CLIENT_CHANNEL_ID_RELIABLE);
    fflush(stdout);

    if (scanf(" %c", &cmd) != 1) return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;

    switch (cmd) {
        case '1':
            s_trans_type   = DJI_MOP_CHANNEL_TRANS_UNRELIABLE;
            s_channel_id   = MOP_CLIENT_CHANNEL_ID_UNRELIABLE;
            s_channel_addr = DJI_CHANNEL_ADDRESS_PAYLOAD_PORT_NO1;
            break;
        case '2':
            s_trans_type   = DJI_MOP_CHANNEL_TRANS_RELIABLE;
            s_channel_id   = MOP_CLIENT_CHANNEL_ID_RELIABLE;
            s_channel_addr = DJI_CHANNEL_ADDRESS_PAYLOAD_PORT_NO1;
            break;
        case '3':
            s_trans_type   = DJI_MOP_CHANNEL_TRANS_UNRELIABLE;
            s_channel_id   = MOP_CLIENT_CHANNEL_ID_UNRELIABLE;
            s_channel_addr = DJI_CHANNEL_ADDRESS_PAYLOAD_PORT_NO2;
            break;
        case '4':
            s_trans_type   = DJI_MOP_CHANNEL_TRANS_RELIABLE;
            s_channel_id   = MOP_CLIENT_CHANNEL_ID_RELIABLE;
            s_channel_addr = DJI_CHANNEL_ADDRESS_PAYLOAD_PORT_NO2;
            break;
        case 'q':
            return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
        default:
            USER_LOG_ERROR("unknown option '%c'", cmd);
            return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    rc = DjiMopChannel_Init();
    if (rc != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("MopChannel_Init failed: 0x%08llX", rc);
        return rc;
    }

    s_running = true;

    T_DjiTaskHandle connect_task = NULL;
    rc = osal->TaskCreate("mop_client_conn", ConnectTask,
                          MOP_CLIENT_TASK_STACK_SIZE, NULL, &connect_task);
    if (rc != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("TaskCreate failed: 0x%08llX", rc);
        s_running = false;
        return rc;
    }

    USER_LOG_INFO("mop channel client started, press [q] + Enter to disconnect");

    char quit;
    while (true) {
        if (scanf(" %c", &quit) == 1 && quit == 'q') break;
    }

    /* Signal stop — ConnectTask owns handle Close/Destroy */
    s_running = false;

    osal->TaskSleepMs(1000);
    osal->TaskDestroy(connect_task);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}
