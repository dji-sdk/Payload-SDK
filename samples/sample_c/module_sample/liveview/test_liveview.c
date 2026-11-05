/**
 ********************************************************************
 * @file    test_liveview.c
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
#include <widget_interaction_test/test_widget_interaction.h>
#include "test_liveview.h"
#include "dji_liveview.h"
#include "dji_logger.h"
#include "dji_platform.h"
#include "dji_aircraft_info.h"
#include "time.h"

/* Private constants ---------------------------------------------------------*/
#define TEST_LIVEVIEW_STREAM_FILE_PATH_STR_MAX_SIZE             256
#define TEST_LIVEVIEW_STREAM_STROING_TIME_IN_SECONDS            20

#define TEST_LIVEVIEW_STREAM_REQUEST_I_FRAME_ON                 1
#define TEST_LIVEVIEW_STREAM_REQUEST_I_FRAME_TICK_IN_SECONDS    5
#define TEST_LIVEVIEW_STREAM_WRITER_QUEUE_SIZE                  16
#define TEST_LIVEVIEW_STREAM_WRITER_DRAIN_TIMEOUT_MS            5000
#define TEST_LIVEVIEW_DUAL_STREAM_TMP_DIR                       "/tmp"

/* Private types -------------------------------------------------------------*/
typedef struct {
    uint64_t bytesSinceLog;
    uint32_t framesSinceLog;
    uint32_t openFailCount;
    uint32_t writeFailCount;
    uint32_t dropCount;
    uint32_t maxWriteCostMs;
    uint64_t lastLogMs;
} T_DjiTestLiveviewIoStat;

typedef struct {
    uint8_t *data;
    uint32_t len;
} T_DjiTestLiveviewFrameItem;

typedef struct {
    const char *streamName;
    FILE **streamFile;
    T_DjiTestLiveviewIoStat *stat;
    T_DjiMutexHandle mutex;
    T_DjiSemaHandle sema;
    T_DjiTaskHandle task;
    bool isInitialized;
    bool isExit;
    bool isWriting;
    uint8_t readIndex;
    uint8_t writeIndex;
    uint8_t pendingCount;
    T_DjiTestLiveviewFrameItem queue[TEST_LIVEVIEW_STREAM_WRITER_QUEUE_SIZE];
} T_DjiTestLiveviewWriter;

typedef struct {
    E_DjiLiveViewCameraPosition position;
    E_DjiLiveViewCameraSource source;
    const char *label;
    char *filePath;
    uint32_t filePathSize;
    char *finalFilePath;
    uint32_t finalFilePathSize;
    FILE **streamFile;
    T_DjiTestLiveviewWriter *writer;
    T_DjiTestLiveviewIoStat *stat;
    DjiLiveview_H264Callback callback;
} T_DjiTestDualLiveviewConfig;

/* Private values -------------------------------------------------------------*/
static char s_fpvCameraStreamFilePath[TEST_LIVEVIEW_STREAM_FILE_PATH_STR_MAX_SIZE];
static char s_payloadCameraStreamFilePath[TEST_LIVEVIEW_STREAM_FILE_PATH_STR_MAX_SIZE];
static char s_stream1CameraStreamFilePath[TEST_LIVEVIEW_STREAM_FILE_PATH_STR_MAX_SIZE];
static char s_stream2CameraStreamFilePath[TEST_LIVEVIEW_STREAM_FILE_PATH_STR_MAX_SIZE];
static char s_stream1CameraStreamFinalFilePath[TEST_LIVEVIEW_STREAM_FILE_PATH_STR_MAX_SIZE];
static char s_stream2CameraStreamFinalFilePath[TEST_LIVEVIEW_STREAM_FILE_PATH_STR_MAX_SIZE];
static FILE *s_fpvCameraStreamFile = NULL;
static FILE *s_payloadCameraStreamFile = NULL;
static FILE *s_stream1CameraStreamFile = NULL;
static FILE *s_stream2CameraStreamFile = NULL;
static T_DjiTestLiveviewIoStat s_fpvStreamStat = {0};
static T_DjiTestLiveviewIoStat s_payloadStreamStat = {0};
static T_DjiTestLiveviewIoStat s_stream1StreamStat = {0};
static T_DjiTestLiveviewIoStat s_stream2StreamStat = {0};
static T_DjiTestLiveviewWriter s_fpvWriter = {0};
static T_DjiTestLiveviewWriter s_payloadWriter = {0};
static T_DjiTestLiveviewWriter s_stream1Writer = {0};
static T_DjiTestLiveviewWriter s_stream2Writer = {0};

/* Private functions declaration ---------------------------------------------*/
static void DjiTest_FpvCameraStreamCallback(E_DjiLiveViewCameraPosition position, const uint8_t *buf,
                                            uint32_t bufLen);
static void DjiTest_PayloadCameraStreamCallback(E_DjiLiveViewCameraPosition position, const uint8_t *buf,
                                                uint32_t bufLen);
static void DjiTest_Stream1CameraStreamCallback(E_DjiLiveViewCameraPosition position, const uint8_t *buf,
                                                uint32_t bufLen);
static void DjiTest_Stream2CameraStreamCallback(E_DjiLiveViewCameraPosition position, const uint8_t *buf,
                                                uint32_t bufLen);
static uint64_t DjiTest_LiveviewGetMonotonicMs(void);
static void DjiTest_LiveviewLogIoStat(const char *streamName, T_DjiTestLiveviewIoStat *stat);
static T_DjiReturnCode DjiTest_LiveviewReopenStreamFile(FILE **fp, const char *path);
static void DjiTest_LiveviewCloseStreamFile(FILE **fp);
static void DjiTest_LiveviewPrintSourceHints(E_DjiAircraftType aircraftType);
static void *DjiTest_LiveviewWriterTask(void *arg);
static T_DjiReturnCode DjiTest_LiveviewWriterInit(T_DjiTestLiveviewWriter *writer, const char *streamName,
                                                  FILE **streamFile, T_DjiTestLiveviewIoStat *stat);
static void DjiTest_LiveviewWriterDrain(T_DjiTestLiveviewWriter *writer);
static void DjiTest_LiveviewWriterEnqueue(T_DjiTestLiveviewWriter *writer, const uint8_t *buf, uint32_t bufLen);
static T_DjiReturnCode DjiTest_LiveviewStartDualStream(const T_DjiTestDualLiveviewConfig *config);
static T_DjiReturnCode DjiTest_LiveviewStopDualStream(const T_DjiTestDualLiveviewConfig *config);
static T_DjiReturnCode DjiTest_LiveviewCopyFile(const char *srcPath, const char *dstPath);

/* Exported functions definition ---------------------------------------------*/

static uint64_t DjiTest_LiveviewGetMonotonicMs(void)
{
    uint32_t timeMs = 0;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    if (osalHandler != NULL && osalHandler->GetTimeMs != NULL) {
        osalHandler->GetTimeMs(&timeMs);
    }

    return timeMs;
}

static void DjiTest_LiveviewLogIoStat(const char *streamName, T_DjiTestLiveviewIoStat *stat)
{
    uint64_t nowMs = DjiTest_LiveviewGetMonotonicMs();

    if (stat->lastLogMs == 0) {
        stat->lastLogMs = nowMs;
    }

    if (nowMs - stat->lastLogMs < 1000) {
        return;
    }

    USER_LOG_INFO("[LIVEVIEW][%s][STAT] frames=%u bytes=%llu open_fail=%u write_fail=%u max_write_cost_ms=%u",
                  streamName, stat->framesSinceLog, (unsigned long long) stat->bytesSinceLog,
                  stat->openFailCount, stat->writeFailCount, stat->maxWriteCostMs);

    if (stat->dropCount > 0) {
        USER_LOG_WARN("[LIVEVIEW][%s][DROP] dropped_frames=%u", streamName, stat->dropCount);
    }

    stat->framesSinceLog = 0;
    stat->bytesSinceLog = 0;
    stat->openFailCount = 0;
    stat->writeFailCount = 0;
    stat->dropCount = 0;
    stat->maxWriteCostMs = 0;
    stat->lastLogMs = nowMs;
}

static T_DjiReturnCode DjiTest_LiveviewReopenStreamFile(FILE **fp, const char *path)
{
    if (*fp != NULL) {
        fclose(*fp);
        *fp = NULL;
    }

    *fp = fopen(path, "ab+");
    if (*fp == NULL) {
        USER_LOG_ERROR("open liveview file failed: %s", path);
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode DjiTest_LiveviewCopyFile(const char *srcPath, const char *dstPath)
{
    uint8_t copyBuf[64 * 1024];
    size_t readSize;
    size_t writeSize;
    FILE *srcFile = NULL;
    FILE *dstFile = NULL;

    srcFile = fopen(srcPath, "rb");
    if (srcFile == NULL) {
        USER_LOG_ERROR("open source liveview file failed: %s", srcPath);
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    dstFile = fopen(dstPath, "wb");
    if (dstFile == NULL) {
        fclose(srcFile);
        USER_LOG_ERROR("open destination liveview file failed: %s", dstPath);
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    while ((readSize = fread(copyBuf, 1, sizeof(copyBuf), srcFile)) > 0) {
        writeSize = fwrite(copyBuf, 1, readSize, dstFile);
        if (writeSize != readSize) {
            fclose(srcFile);
            fclose(dstFile);
            USER_LOG_ERROR("copy liveview file failed: %s -> %s", srcPath, dstPath);
            return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
        }
    }

    fflush(dstFile);
    fclose(srcFile);
    fclose(dstFile);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static void DjiTest_LiveviewCloseStreamFile(FILE **fp)
{
    if (*fp != NULL) {
        fflush(*fp);
        fclose(*fp);
        *fp = NULL;
    }
}

static void DjiTest_LiveviewPrintSourceHints(E_DjiAircraftType aircraftType)
{
    printf("Source hint: DEFAULT source is 0.\n");

    switch (aircraftType) {
        case DJI_AIRCRAFT_TYPE_M5T:
            printf("Current aircraft is M5T. Recommended sources: 0=1080P, 1=4K, 3=IR.\n");
            break;
        case DJI_AIRCRAFT_TYPE_M5TD:
            printf("Current aircraft is M5TD. Recommended sources: 1=VIS, 2=IR, 3=4K.\n");
            break;
        case DJI_AIRCRAFT_TYPE_M4T:
        case DJI_AIRCRAFT_TYPE_M4TD:
            printf("Current aircraft is M4T/M4TD. Recommended sources: 1=VIS, 2=IR, 3=4K.\n");
            break;
        case DJI_AIRCRAFT_TYPE_M3T:
        case DJI_AIRCRAFT_TYPE_M3TA:
        case DJI_AIRCRAFT_TYPE_M3TD:
            printf("Current aircraft is M3T family. Recommended sources: 1=VIS, 2=IR.\n");
            break;
        default:
            printf("No aircraft-specific source hint. Use source values supported by current payload.\n");
            break;
    }
}

static void *DjiTest_LiveviewWriterTask(void *arg)
{
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    T_DjiTestLiveviewWriter *writer = (T_DjiTestLiveviewWriter *) arg;
    T_DjiTestLiveviewFrameItem item = {0};

    while (true) {
        osalHandler->SemaphoreWait(writer->sema);

        while (true) {
            osalHandler->MutexLock(writer->mutex);
            if (writer->pendingCount == 0) {
                bool shouldExit = writer->isExit;
                osalHandler->MutexUnlock(writer->mutex);
                if (shouldExit) {
                    return NULL;
                }
                break;
            }

            item = writer->queue[writer->readIndex];
            writer->queue[writer->readIndex].data = NULL;
            writer->queue[writer->readIndex].len = 0;
            writer->readIndex = (writer->readIndex + 1) % TEST_LIVEVIEW_STREAM_WRITER_QUEUE_SIZE;
            writer->pendingCount--;
            writer->isWriting = true;
            osalHandler->MutexUnlock(writer->mutex);

            if (*(writer->streamFile) == NULL) {
                writer->stat->openFailCount++;
                osalHandler->Free(item.data);
                osalHandler->MutexLock(writer->mutex);
                writer->isWriting = false;
                osalHandler->MutexUnlock(writer->mutex);
                DjiTest_LiveviewLogIoStat(writer->streamName, writer->stat);
                continue;
            }

            uint64_t startMs = DjiTest_LiveviewGetMonotonicMs();
            size_t size = fwrite(item.data, 1, item.len, *(writer->streamFile));
            if (size != item.len) {
                writer->stat->writeFailCount++;
            } else {
                uint32_t writeCostMs = (uint32_t) (DjiTest_LiveviewGetMonotonicMs() - startMs);
                writer->stat->framesSinceLog++;
                writer->stat->bytesSinceLog += item.len;
                if (writeCostMs > writer->stat->maxWriteCostMs) {
                    writer->stat->maxWriteCostMs = writeCostMs;
                }
            }

            osalHandler->Free(item.data);
            osalHandler->MutexLock(writer->mutex);
            writer->isWriting = false;
            osalHandler->MutexUnlock(writer->mutex);
            DjiTest_LiveviewLogIoStat(writer->streamName, writer->stat);
        }
    }
}

static T_DjiReturnCode DjiTest_LiveviewWriterInit(T_DjiTestLiveviewWriter *writer, const char *streamName,
                                                  FILE **streamFile, T_DjiTestLiveviewIoStat *stat)
{
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    T_DjiReturnCode returnCode;

    if (writer->isInitialized) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    }

    memset(writer, 0, sizeof(T_DjiTestLiveviewWriter));
    writer->streamName = streamName;
    writer->streamFile = streamFile;
    writer->stat = stat;

    returnCode = osalHandler->MutexCreate(&writer->mutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        return returnCode;
    }

    returnCode = osalHandler->SemaphoreCreate(0, &writer->sema);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        osalHandler->MutexDestroy(writer->mutex);
        writer->mutex = NULL;
        return returnCode;
    }

    returnCode = osalHandler->TaskCreate("liveview_writer", DjiTest_LiveviewWriterTask, 2048, writer, &writer->task);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        osalHandler->SemaphoreDestroy(writer->sema);
        osalHandler->MutexDestroy(writer->mutex);
        writer->sema = NULL;
        writer->mutex = NULL;
        return returnCode;
    }

    writer->isInitialized = true;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static void DjiTest_LiveviewWriterDrain(T_DjiTestLiveviewWriter *writer)
{
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    uint8_t pendingCount;
    bool isWriting;
    uint64_t startMs;

    if (!writer->isInitialized) {
        return;
    }

    startMs = DjiTest_LiveviewGetMonotonicMs();

    while (true) {
        osalHandler->MutexLock(writer->mutex);
        pendingCount = writer->pendingCount;
        isWriting = writer->isWriting;
        osalHandler->MutexUnlock(writer->mutex);

        if (pendingCount == 0 && isWriting == false) {
            break;
        }

        if (DjiTest_LiveviewGetMonotonicMs() - startMs > TEST_LIVEVIEW_STREAM_WRITER_DRAIN_TIMEOUT_MS) {
            USER_LOG_WARN("[LIVEVIEW][%s][DRAIN] timeout pending=%u isWriting=%d",
                          writer->streamName, pendingCount, isWriting);
            break;
        }

        osalHandler->TaskSleepMs(10);
    }
}

static void DjiTest_LiveviewWriterEnqueue(T_DjiTestLiveviewWriter *writer, const uint8_t *buf, uint32_t bufLen)
{
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    uint8_t *dataCopy;

    if (!writer->isInitialized) {
        return;
    }

    dataCopy = osalHandler->Malloc(bufLen);
    if (dataCopy == NULL) {
        writer->stat->writeFailCount++;
        DjiTest_LiveviewLogIoStat(writer->streamName, writer->stat);
        return;
    }

    memcpy(dataCopy, buf, bufLen);

    osalHandler->MutexLock(writer->mutex);
    if (writer->pendingCount >= TEST_LIVEVIEW_STREAM_WRITER_QUEUE_SIZE) {
        writer->stat->dropCount++;
        osalHandler->MutexUnlock(writer->mutex);
        osalHandler->Free(dataCopy);
        DjiTest_LiveviewLogIoStat(writer->streamName, writer->stat);
        return;
    }

    writer->queue[writer->writeIndex].data = dataCopy;
    writer->queue[writer->writeIndex].len = bufLen;
    writer->writeIndex = (writer->writeIndex + 1) % TEST_LIVEVIEW_STREAM_WRITER_QUEUE_SIZE;
    writer->pendingCount++;
    osalHandler->MutexUnlock(writer->mutex);
    osalHandler->SemaphorePost(writer->sema);
}

void DjiTest_LiveviewSample(void)
{
    int pos;

    do {
        printf("Please enter the camera mount position (valid values: 1, 2 or 3): ");
        scanf("%d", &pos);

        if (pos != 1 && pos != 2 && pos != 3) {
            printf("Invalid input, please enter 1, 2 or 3.\n");
        }
    } while (pos != 1 && pos != 2 && pos != 3);

    DjiTest_LiveviewRunSample((E_DjiMountPosition)pos);
}

void DjiTest_LiveviewDualStreamSample(void)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    time_t currentTime = time(NULL);
    struct tm *localTime = NULL;
    T_DjiAircraftInfoBaseInfo aircraftInfoBaseInfo = {0};
    int stream1Pos = 1;
    int stream1Source = 0;
    int stream2Pos = 1;
    int stream2Source = 3;
    int durationSelect = TEST_LIVEVIEW_STREAM_STROING_TIME_IN_SECONDS;
    int durationSeconds = TEST_LIVEVIEW_STREAM_STROING_TIME_IN_SECONDS;
    T_DjiTestDualLiveviewConfig stream1Config;
    T_DjiTestDualLiveviewConfig stream2Config;

    returnCode = DjiAircraftInfo_GetBaseInfo(&aircraftInfoBaseInfo);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("get aircraft base info error");
        return;
    }

    printf("Dual liveview sample on aircraft type enum: %d\n",
           aircraftInfoBaseInfo.aircraftType);
    DjiTest_LiveviewPrintSourceHints(aircraftInfoBaseInfo.aircraftType);

    printf("Please enter stream1 camera position (1, 2, 3 or 7 for FPV): ");
    scanf("%d", &stream1Pos);
    printf("Please enter stream1 source (integer, default source is 0): ");
    scanf("%d", &stream1Source);
    printf("Please enter stream2 camera position (1, 2, 3 or 7 for FPV): ");
    scanf("%d", &stream2Pos);
    printf("Please enter stream2 source (integer, default source is 0): ");
    scanf("%d", &stream2Source);
    printf("Please enter total storage seconds (10 or 20): ");
    scanf("%d", &durationSelect);

    if (durationSelect == 10) {
        durationSeconds = 10;
    } else {
        durationSeconds = TEST_LIVEVIEW_STREAM_STROING_TIME_IN_SECONDS;
    }

    USER_LOG_INFO("Dual liveview sample start, stream1 pos=%d src=%d, stream2 pos=%d src=%d, duration=%d",
                  stream1Pos, stream1Source, stream2Pos, stream2Source, durationSeconds);
    USER_LOG_INFO("Dual liveview temporary output dir: %s", TEST_LIVEVIEW_DUAL_STREAM_TMP_DIR);

    returnCode = DjiLiveview_Init();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Dual liveview init failed, error code: 0x%08X", returnCode);
        return;
    }

    returnCode = DjiTest_LiveviewWriterInit(&s_stream1Writer, "STREAM1", &s_stream1CameraStreamFile,
                                            &s_stream1StreamStat);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Init stream1 writer failed, error code: 0x%08X", returnCode);
        goto out;
    }

    returnCode = DjiTest_LiveviewWriterInit(&s_stream2Writer, "STREAM2", &s_stream2CameraStreamFile,
                                            &s_stream2StreamStat);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Init stream2 writer failed, error code: 0x%08X", returnCode);
        goto out;
    }

    localTime = localtime(&currentTime);
    snprintf(s_stream1CameraStreamFinalFilePath, sizeof(s_stream1CameraStreamFinalFilePath),
             "dual_stream1_pos%d_src%d_%04d%02d%02d_%02d-%02d-%02d.video",
             stream1Pos, stream1Source,
             localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday,
             localTime->tm_hour, localTime->tm_min, localTime->tm_sec);
    snprintf(s_stream2CameraStreamFinalFilePath, sizeof(s_stream2CameraStreamFinalFilePath),
             "dual_stream2_pos%d_src%d_%04d%02d%02d_%02d-%02d-%02d.video",
             stream2Pos, stream2Source,
             localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday,
             localTime->tm_hour, localTime->tm_min, localTime->tm_sec);

    snprintf(s_stream1CameraStreamFilePath, sizeof(s_stream1CameraStreamFilePath),
             "%s/%s", TEST_LIVEVIEW_DUAL_STREAM_TMP_DIR, s_stream1CameraStreamFinalFilePath);
    snprintf(s_stream2CameraStreamFilePath, sizeof(s_stream2CameraStreamFilePath),
             "%s/%s", TEST_LIVEVIEW_DUAL_STREAM_TMP_DIR, s_stream2CameraStreamFinalFilePath);

    stream1Config.position = (E_DjiLiveViewCameraPosition) stream1Pos;
    stream1Config.source = (E_DjiLiveViewCameraSource) stream1Source;
    stream1Config.label = "STREAM1";
    stream1Config.filePath = s_stream1CameraStreamFilePath;
    stream1Config.filePathSize = sizeof(s_stream1CameraStreamFilePath);
    stream1Config.finalFilePath = s_stream1CameraStreamFinalFilePath;
    stream1Config.finalFilePathSize = sizeof(s_stream1CameraStreamFinalFilePath);
    stream1Config.streamFile = &s_stream1CameraStreamFile;
    stream1Config.writer = &s_stream1Writer;
    stream1Config.stat = &s_stream1StreamStat;
    stream1Config.callback = DjiTest_Stream1CameraStreamCallback;

    stream2Config.position = (E_DjiLiveViewCameraPosition) stream2Pos;
    stream2Config.source = (E_DjiLiveViewCameraSource) stream2Source;
    stream2Config.label = "STREAM2";
    stream2Config.filePath = s_stream2CameraStreamFilePath;
    stream2Config.filePathSize = sizeof(s_stream2CameraStreamFilePath);
    stream2Config.finalFilePath = s_stream2CameraStreamFinalFilePath;
    stream2Config.finalFilePathSize = sizeof(s_stream2CameraStreamFinalFilePath);
    stream2Config.streamFile = &s_stream2CameraStreamFile;
    stream2Config.writer = &s_stream2Writer;
    stream2Config.stat = &s_stream2StreamStat;
    stream2Config.callback = DjiTest_Stream2CameraStreamCallback;

    returnCode = DjiTest_LiveviewStartDualStream(&stream1Config);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Start stream1 failed, error code: 0x%08X", returnCode);
        goto out;
    }

    returnCode = DjiTest_LiveviewStartDualStream(&stream2Config);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Start stream2 failed, error code: 0x%08X", returnCode);
        goto out;
    }

    for (int i = 0; i < durationSeconds; ++i) {
        USER_LOG_INFO("Dual stream storing, second: %d/%d.", i + 1, durationSeconds);
        osalHandler->TaskSleepMs(1000);
    }

    DjiTest_LiveviewStopDualStream(&stream1Config);
    DjiTest_LiveviewStopDualStream(&stream2Config);

    returnCode = DjiTest_LiveviewCopyFile(stream1Config.filePath, stream1Config.finalFilePath);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Copy stream1 file failed, error code: 0x%08X", returnCode);
        goto out;
    }

    returnCode = DjiTest_LiveviewCopyFile(stream2Config.filePath, stream2Config.finalFilePath);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Copy stream2 file failed, error code: 0x%08X", returnCode);
        goto out;
    }

    USER_LOG_INFO("Stream1 file is saved to file: %s", s_stream1CameraStreamFinalFilePath);
    USER_LOG_INFO("Stream2 file is saved to file: %s", s_stream2CameraStreamFinalFilePath);

out:
    DjiTest_LiveviewWriterDrain(&s_stream1Writer);
    DjiTest_LiveviewWriterDrain(&s_stream2Writer);
    DjiTest_LiveviewCloseStreamFile(&s_stream1CameraStreamFile);
    DjiTest_LiveviewCloseStreamFile(&s_stream2CameraStreamFile);
    DjiLiveview_Deinit();
    USER_LOG_INFO("Dual liveview sample end");
}

T_DjiReturnCode DjiTest_LiveviewRunSample(E_DjiMountPosition mountPosition)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    time_t currentTime = time(NULL);
    struct tm *localTime = NULL;
    T_DjiAircraftInfoBaseInfo aircraftInfoBaseInfo = {0};

    USER_LOG_INFO("Liveview sample start");
    DjiTest_WidgetLogAppend("Liveview sample start");

    returnCode = DjiAircraftInfo_GetBaseInfo(&aircraftInfoBaseInfo);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("get aircraft base info error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    USER_LOG_INFO("--> Step 1: Init liveview module");
    DjiTest_WidgetLogAppend("--> Step 1: Init liveview module");
    returnCode = DjiLiveview_Init();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Liveview init failed, error code: 0x%08X", returnCode);
        goto out;
    }

    returnCode = DjiTest_LiveviewWriterInit(&s_fpvWriter, "FPV", &s_fpvCameraStreamFile, &s_fpvStreamStat);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Init fpv liveview writer failed, error code: 0x%08X", returnCode);
        goto out;
    }

    returnCode = DjiTest_LiveviewWriterInit(&s_payloadWriter, "PAYLOAD", &s_payloadCameraStreamFile,
                                            &s_payloadStreamStat);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Init payload liveview writer failed, error code: 0x%08X", returnCode);
        goto out;
    }

    USER_LOG_INFO("--> Step 2: Start stream of the fpv and default of selected payload\r\n");
    DjiTest_WidgetLogAppend("--> Step 2: Start stream of the fpv and selected payload\r\n");

    if (aircraftInfoBaseInfo.aircraftSeries == DJI_AIRCRAFT_SERIES_M300 ||
        aircraftInfoBaseInfo.aircraftSeries == DJI_AIRCRAFT_SERIES_M350 ||
        aircraftInfoBaseInfo.aircraftSeries == DJI_AIRCRAFT_SERIES_M30 ||
        aircraftInfoBaseInfo.aircraftSeries == DJI_AIRCRAFT_SERIES_M400) {

        localTime = localtime(&currentTime);
        sprintf(s_fpvCameraStreamFilePath, "fpv_stream_%04d%02d%02d_%02d-%02d-%02d.h264",
                localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday,
                localTime->tm_hour, localTime->tm_min, localTime->tm_sec);

        returnCode = DjiTest_LiveviewReopenStreamFile(&s_fpvCameraStreamFile, s_fpvCameraStreamFilePath);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            goto out;
        }

        returnCode = DjiLiveview_StartH264Stream(DJI_LIVEVIEW_CAMERA_POSITION_FPV, DJI_LIVEVIEW_CAMERA_SOURCE_DEFAULT,
                                                 DjiTest_FpvCameraStreamCallback);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Request h264 of fpv failed, error code: 0x%08X", returnCode);
            goto out;
        }
    }

    if (aircraftInfoBaseInfo.aircraftSeries == DJI_AIRCRAFT_SERIES_M400
        && aircraftInfoBaseInfo.mountPositionType == DJI_MOUNT_POSITION_TYPE_MANIFOLD3_ONBOARD)
    {
        // XXX:  On MANIFOLD3, FPV and MAIN CAMERA SOURCE_DEFAULT streams cannot be subscribed to at the same time.
        for (int i = 0; i < TEST_LIVEVIEW_STREAM_STROING_TIME_IN_SECONDS; ++i) {
            USER_LOG_INFO("Storing camera h264 stream, second: %d.", i + 1);
            osalHandler->TaskSleepMs(1000);
        }
        returnCode = DjiLiveview_StopH264Stream(DJI_LIVEVIEW_CAMERA_POSITION_FPV, DJI_LIVEVIEW_CAMERA_SOURCE_DEFAULT);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Request to stop h264 of fpv failed, error code: 0x%08X", returnCode);
            goto out;
        }

        DjiTest_LiveviewWriterDrain(&s_fpvWriter);
        DjiTest_LiveviewCloseStreamFile(&s_fpvCameraStreamFile);
    }

    localTime = localtime(&currentTime);
    sprintf(s_payloadCameraStreamFilePath, "payload%d_default_stream_%04d%02d%02d_%02d-%02d-%02d.video",
            mountPosition, localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday,
            localTime->tm_hour, localTime->tm_min, localTime->tm_sec);

    returnCode = DjiTest_LiveviewReopenStreamFile(&s_payloadCameraStreamFile, s_payloadCameraStreamFilePath);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        goto out;
    }

    returnCode = DjiLiveview_StartStream((E_DjiLiveViewCameraPosition) mountPosition,
                                             DJI_LIVEVIEW_CAMERA_SOURCE_DEFAULT,
                                             DjiTest_PayloadCameraStreamCallback);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Request video stream of payload %d failed, error code: 0x%08X", mountPosition, returnCode);
    }

    for (int i = 0; i < TEST_LIVEVIEW_STREAM_STROING_TIME_IN_SECONDS; ++i) {
        USER_LOG_INFO("Storing camera video stream, second: %d.", i + 1);
#if TEST_LIVEVIEW_STREAM_REQUEST_I_FRAME_ON
        if (aircraftInfoBaseInfo.mountPositionType != DJI_MOUNT_POSITION_TYPE_MANIFOLD3_ONBOARD)
        {
            if (i % TEST_LIVEVIEW_STREAM_REQUEST_I_FRAME_TICK_IN_SECONDS == 0)
            {
                returnCode = DjiLiveview_RequestIntraframeFrameData((E_DjiLiveViewCameraPosition)mountPosition,
                                                                    DJI_LIVEVIEW_CAMERA_SOURCE_DEFAULT);
                if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
                {
                    USER_LOG_ERROR("Request stream I frame of payload %d failed, error code: 0x%08X", mountPosition,
                                   returnCode);
                }
            }
        }
#endif
        osalHandler->TaskSleepMs(1000);
    }

    USER_LOG_INFO("--> Step 3: Stop video stream of the fpv and selected payload\r\n");
    DjiTest_WidgetLogAppend("--> Step 3: Stop video stream of the fpv and selected payload");
    if (aircraftInfoBaseInfo.aircraftSeries == DJI_AIRCRAFT_SERIES_M300 ||
        aircraftInfoBaseInfo.aircraftSeries == DJI_AIRCRAFT_SERIES_M350 ||
        aircraftInfoBaseInfo.aircraftSeries == DJI_AIRCRAFT_SERIES_M30 ||
        (aircraftInfoBaseInfo.aircraftSeries == DJI_AIRCRAFT_SERIES_M400
         && aircraftInfoBaseInfo.mountPositionType != DJI_MOUNT_POSITION_TYPE_MANIFOLD3_ONBOARD)) {
        returnCode = DjiLiveview_StopH264Stream(DJI_LIVEVIEW_CAMERA_POSITION_FPV, DJI_LIVEVIEW_CAMERA_SOURCE_DEFAULT);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Request to stop h264 of fpv failed, error code: 0x%08X", returnCode);
            goto out;
        }
    }

    returnCode = DjiLiveview_StopStream((E_DjiLiveViewCameraPosition) mountPosition,
                                            DJI_LIVEVIEW_CAMERA_SOURCE_DEFAULT);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Request to stop stream of payload %d failed, error code: 0x%08X", mountPosition, returnCode);
        goto out;
    }

    DjiTest_LiveviewWriterDrain(&s_payloadWriter);
    DjiTest_LiveviewCloseStreamFile(&s_payloadCameraStreamFile);

    if (DJI_AIRCRAFT_TYPE_M3T == aircraftInfoBaseInfo.aircraftType
        || DJI_AIRCRAFT_TYPE_M3TA == aircraftInfoBaseInfo.aircraftType
        || DJI_AIRCRAFT_TYPE_M3TD == aircraftInfoBaseInfo.aircraftType
        || DJI_AIRCRAFT_TYPE_M4T == aircraftInfoBaseInfo.aircraftType
        || DJI_AIRCRAFT_TYPE_M4TD == aircraftInfoBaseInfo.aircraftType
        || DJI_AIRCRAFT_TYPE_M5TD == aircraftInfoBaseInfo.aircraftType
        || DJI_AIRCRAFT_TYPE_M5T == aircraftInfoBaseInfo.aircraftType
    ) {
        E_DjiLiveViewCameraSource irSource;
        if (DJI_AIRCRAFT_TYPE_M5T == aircraftInfoBaseInfo.aircraftType) {
            irSource = DJI_LIVEVIEW_CAMERA_SOURCE_M5T_IR;
        } else if (DJI_AIRCRAFT_TYPE_M5TD == aircraftInfoBaseInfo.aircraftType) {
            irSource = DJI_LIVEVIEW_CAMERA_SOURCE_M5TD_IR;
        } else {
            irSource = DJI_LIVEVIEW_CAMERA_SOURCE_M3T_IR;
        }

        USER_LOG_INFO("--> Start stream of the fpv and selected payload\r\n");

        localTime = localtime(&currentTime);
        sprintf(s_payloadCameraStreamFilePath, "payload%d_ir_stream_%04d%02d%02d_%02d-%02d-%02d.video",
                mountPosition, localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday,
                localTime->tm_hour, localTime->tm_min, localTime->tm_sec);

        returnCode = DjiTest_LiveviewReopenStreamFile(&s_payloadCameraStreamFile, s_payloadCameraStreamFilePath);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            goto out;
        }

        returnCode = DjiLiveview_StartStream((E_DjiLiveViewCameraPosition) mountPosition,
                                                 irSource,
                                                 DjiTest_PayloadCameraStreamCallback);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Request video stream of payload %d failed, error code: 0x%08X", mountPosition, returnCode);
        }

        for (int i = 0; i < TEST_LIVEVIEW_STREAM_STROING_TIME_IN_SECONDS; ++i) {
            USER_LOG_INFO("Storing camera video stream, second: %d.", i + 1);
            osalHandler->TaskSleepMs(1000);
        }

        returnCode = DjiLiveview_StopStream((E_DjiLiveViewCameraPosition) mountPosition,
                                                irSource);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Request to stop stream of payload %d failed, error code: 0x%08X", mountPosition, returnCode);
            goto out;
        }

        DjiTest_LiveviewWriterDrain(&s_payloadWriter);
        DjiTest_LiveviewCloseStreamFile(&s_payloadCameraStreamFile);
    }

    if ( DJI_AIRCRAFT_TYPE_M5T == aircraftInfoBaseInfo.aircraftType
    ) {
        E_DjiLiveViewCameraSource Source4k;
        Source4k = DJI_LIVEVIEW_CAMERA_SOURCE_M5T_4K;

        USER_LOG_INFO("--> Start stream of the fpv and selected payload\r\n");

        localTime = localtime(&currentTime);
        sprintf(s_payloadCameraStreamFilePath, "payload%d_4k_stream_%04d%02d%02d_%02d-%02d-%02d.video",
                mountPosition, localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday,
                localTime->tm_hour, localTime->tm_min, localTime->tm_sec);

        returnCode = DjiTest_LiveviewReopenStreamFile(&s_payloadCameraStreamFile, s_payloadCameraStreamFilePath);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            goto out;
        }

        returnCode = DjiLiveview_StartStream((E_DjiLiveViewCameraPosition) mountPosition,
                                                 Source4k,
                                                 DjiTest_PayloadCameraStreamCallback);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Request video stream of payload %d failed, error code: 0x%08X", mountPosition, returnCode);
        }

        for (int i = 0; i < TEST_LIVEVIEW_STREAM_STROING_TIME_IN_SECONDS; ++i) {
            USER_LOG_INFO("Storing camera video stream, second: %d.", i + 1);
            osalHandler->TaskSleepMs(1000);
        }

        returnCode = DjiLiveview_StopStream((E_DjiLiveViewCameraPosition) mountPosition,
                                                Source4k);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Request to stop stream of payload %d failed, error code: 0x%08X", mountPosition, returnCode);
            goto out;
        }

        DjiTest_LiveviewWriterDrain(&s_payloadWriter);
        DjiTest_LiveviewCloseStreamFile(&s_payloadCameraStreamFile);
    }

    USER_LOG_INFO("Fpv stream is saved to file: %s", s_fpvCameraStreamFilePath);
    USER_LOG_INFO("Payload%d stream is saved to file: %s\r\n", mountPosition, s_payloadCameraStreamFilePath);

    USER_LOG_INFO("--> Step 4: Deinit liveview module");
    DjiTest_WidgetLogAppend("--> Step 4: Deinit liveview module");
    returnCode = DjiLiveview_Deinit();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Liveview deinit failed, error code: 0x%08X", returnCode);
        goto out;
    }

out:
    DjiTest_LiveviewWriterDrain(&s_fpvWriter);
    DjiTest_LiveviewWriterDrain(&s_payloadWriter);
    DjiTest_LiveviewCloseStreamFile(&s_fpvCameraStreamFile);
    DjiTest_LiveviewCloseStreamFile(&s_payloadCameraStreamFile);
    USER_LOG_INFO("Liveview sample end");

    return returnCode;
}

/* Private functions definition-----------------------------------------------*/
static void DjiTest_FpvCameraStreamCallback(E_DjiLiveViewCameraPosition position, const uint8_t *buf,
                                            uint32_t bufLen)
{
    size_t size;
    uint64_t startMs = DjiTest_LiveviewGetMonotonicMs();
    uint32_t writeCostMs;

    USER_UTIL_UNUSED(position);

    if (s_fpvCameraStreamFile == NULL) {
        s_fpvStreamStat.openFailCount++;
        DjiTest_LiveviewLogIoStat("FPV", &s_fpvStreamStat);
        return;
    }

    USER_UTIL_UNUSED(size);
    USER_UTIL_UNUSED(startMs);
    USER_UTIL_UNUSED(writeCostMs);
    DjiTest_LiveviewWriterEnqueue(&s_fpvWriter, buf, bufLen);
}

static void DjiTest_PayloadCameraStreamCallback(E_DjiLiveViewCameraPosition position, const uint8_t *buf,
                                                uint32_t bufLen)
{
    size_t size;
    uint64_t startMs = DjiTest_LiveviewGetMonotonicMs();
    uint32_t writeCostMs;

    USER_UTIL_UNUSED(position);

    if (s_payloadCameraStreamFile == NULL) {
        s_payloadStreamStat.openFailCount++;
        DjiTest_LiveviewLogIoStat("PAYLOAD", &s_payloadStreamStat);
        return;
    }

    USER_UTIL_UNUSED(size);
    USER_UTIL_UNUSED(startMs);
    USER_UTIL_UNUSED(writeCostMs);
    DjiTest_LiveviewWriterEnqueue(&s_payloadWriter, buf, bufLen);
}

static void DjiTest_Stream1CameraStreamCallback(E_DjiLiveViewCameraPosition position, const uint8_t *buf,
                                                uint32_t bufLen)
{
    USER_UTIL_UNUSED(position);
    DjiTest_LiveviewWriterEnqueue(&s_stream1Writer, buf, bufLen);
}

static void DjiTest_Stream2CameraStreamCallback(E_DjiLiveViewCameraPosition position, const uint8_t *buf,
                                                uint32_t bufLen)
{
    USER_UTIL_UNUSED(position);
    DjiTest_LiveviewWriterEnqueue(&s_stream2Writer, buf, bufLen);
}

static T_DjiReturnCode DjiTest_LiveviewStartDualStream(const T_DjiTestDualLiveviewConfig *config)
{
    T_DjiReturnCode returnCode;

    returnCode = DjiTest_LiveviewReopenStreamFile(config->streamFile, config->filePath);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        return returnCode;
    }

    returnCode = DjiLiveview_StartStream(config->position, config->source, config->callback);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Request %s failed, pos=%d source=%d error code: 0x%08X",
                       config->label, config->position, config->source, returnCode);
        return returnCode;
    }

    USER_LOG_INFO("%s started, pos=%d source=%d file=%s",
                  config->label, config->position, config->source, config->filePath);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode DjiTest_LiveviewStopDualStream(const T_DjiTestDualLiveviewConfig *config)
{
    T_DjiReturnCode returnCode;

    returnCode = DjiLiveview_StopStream(config->position, config->source);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Stop %s failed, pos=%d source=%d error code: 0x%08X",
                       config->label, config->position, config->source, returnCode);
        return returnCode;
    }

    DjiTest_LiveviewWriterDrain(config->writer);
    DjiTest_LiveviewCloseStreamFile(config->streamFile);

    USER_LOG_INFO("%s stopped, pos=%d source=%d", config->label, config->position, config->source);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
