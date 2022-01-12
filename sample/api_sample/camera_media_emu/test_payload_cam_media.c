/**
 ********************************************************************
 * @file    test_payload_cam_media.c
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
#include "test_payload_cam_media.h"

#if PSDK_ARCH_SYS_LINUX

#include <fcntl.h>
#include <stdlib.h>
#include <psdk_platform.h>
#include <camera_emu/test_payload_cam_emu.h>
#include "psdk_logger.h"
#include "camera_media_emu/psdk_media_file_manage/psdk_media_file_core.h"
#include "utils/util_time.h"
#include "utils/util_file.h"
#include "utils/util_misc.h"
#include "utils/util_buffer.h"

/* Private constants ---------------------------------------------------------*/
#define PLAYBACK_TIME_STR_BUF_SIZE       50
#define FFMPEG_CMD_BUF_SIZE              (PSDK_MEDIA_FILE_PATH_LEN_MAX + 256)

#define PSDK_MEDIA_FILE_PATH            "../../../../../api_sample/camera_media_emu/media_file/"

#define SEND_VIDEO_TASK_FREQ            120
#define VIDEO_FRAME_MAX_COUNT           18000 // max video duration 10 minutes
#define VIDEO_FRAME_AUD_LEN             6
#define VIDEO_FILE_PATH                 "../../../../../api_sample/camera_media_emu/media_file/PSDK_0006.h264"

#define DATA_SEND_FROM_VIDEO_STREAM_MAX_LEN 65000

/* Private types -------------------------------------------------------------*/
typedef enum {
    TEST_PAYLOAD_CAMERA_MEDIA_PLAY_COMMAND_STOP = 0,
    TEST_PAYLOAD_CAMERA_MEDIA_PLAY_COMMAND_PAUSE = 1,
    TEST_PAYLOAD_CAMERA_MEDIA_PLAY_COMMAND_START = 2,
} E_TestPayloadCameraPlaybackCommand;

typedef struct {
    uint8_t isInPlayProcess;
    uint16_t videoIndex;
    char filePath[PSDK_MEDIA_FILE_PATH_LEN_MAX];
    uint32_t videoLengthMs;
    uint64_t startPlayTimestampsUs;
    uint64_t playPosMs;
} T_PsdkPlaybackInfo;

typedef struct {
    E_TestPayloadCameraPlaybackCommand command;
    uint32_t timeMs;
    char path[PSDK_MEDIA_FILE_PATH_LEN_MAX];
} T_TestPayloadCameraPlaybackCommand;

typedef struct {
    float durationS;
    uint32_t positionInFile;
    uint32_t size;
} T_TestPayloadCameraVideoFrameInfo;

/* Private functions declaration ---------------------------------------------*/
static T_PsdkReturnCode PsdkPlayback_StopPlay(T_PsdkPlaybackInfo *playbackInfo);
static T_PsdkReturnCode PsdkPlayback_PausePlay(T_PsdkPlaybackInfo *playbackInfo);
static T_PsdkReturnCode PsdkPlayback_SetPlayFile(T_PsdkPlaybackInfo *playbackInfo, const char *filePath,
                                                 uint16_t index);
static T_PsdkReturnCode PsdkPlayback_SeekPlay(T_PsdkPlaybackInfo *playbackInfo, uint32_t seekPos);
static T_PsdkReturnCode PsdkPlayback_StartPlay(T_PsdkPlaybackInfo *playbackInfo);
static T_PsdkReturnCode PsdkPlayback_GetPlaybackStatus(T_PsdkPlaybackInfo *playbackInfo,
                                                       T_PsdkCameraPlaybackStatus *playbackStatus);
static T_PsdkReturnCode PsdkPlayback_GetVideoLengthMs(const char *filePath, uint32_t *videoLengthMs);
static T_PsdkReturnCode PsdkPlayback_StartPlayProcess(const char *filePath, uint32_t playPosMs);
static T_PsdkReturnCode PsdkPlayback_StopPlayProcess(void);
static T_PsdkReturnCode
PsdkPlayback_VideoFileTranscode(const char *inPath, const char *outFormat, char *outPath, uint16_t outPathBufferSize);
static T_PsdkReturnCode
PsdkPlayback_GetFrameInfoOfVideoFile(const char *path, T_TestPayloadCameraVideoFrameInfo *frameInfo,
                                     uint32_t frameInfoBufferCount, uint32_t *frameCount);
static T_PsdkReturnCode PsdkPlayback_GetFrameRateOfVideoFile(const char *path, float *frameRate);
static T_PsdkReturnCode
PsdkPlayback_GetFrameNumberByTime(T_TestPayloadCameraVideoFrameInfo *frameInfo, uint32_t frameCount,
                                  uint32_t *frameNumber, uint32_t timeMs);
static T_PsdkReturnCode GetMediaFileDir(char *dirPath);
static T_PsdkReturnCode GetMediaFileOriginData(const char *filePath, uint32_t offset, uint32_t length,
                                               uint8_t *data);

static T_PsdkReturnCode CreateMediaFileThumbNail(const char *filePath);
static T_PsdkReturnCode GetMediaFileThumbNailInfo(const char *filePath, T_PsdkCameraMediaFileInfo *fileInfo);
static T_PsdkReturnCode GetMediaFileThumbNailData(const char *filePath, uint32_t offset, uint32_t length,
                                                  uint8_t *data);
static T_PsdkReturnCode DestroyMediaFileThumbNail(const char *filePath);

static T_PsdkReturnCode CreateMediaFileScreenNail(const char *filePath);
static T_PsdkReturnCode GetMediaFileScreenNailInfo(const char *filePath, T_PsdkCameraMediaFileInfo *fileInfo);
static T_PsdkReturnCode GetMediaFileScreenNailData(const char *filePath, uint32_t offset, uint32_t length,
                                                   uint8_t *data);
static T_PsdkReturnCode DestroyMediaFileScreenNail(const char *filePath);

static T_PsdkReturnCode DeleteMediaFile(char *filePath);
static T_PsdkReturnCode SetMediaPlaybackFile(const char *filePath);
static T_PsdkReturnCode StartMediaPlayback(void);
static T_PsdkReturnCode StopMediaPlayback(void);
static T_PsdkReturnCode PauseMediaPlayback(void);
static T_PsdkReturnCode SeekMediaPlayback(uint32_t playbackPosition);
static T_PsdkReturnCode GetMediaPlaybackStatus(T_PsdkCameraPlaybackStatus *status);

static T_PsdkReturnCode StartDownloadNotification(void);
static T_PsdkReturnCode StopDownloadNotification(void);

static void *UserCameraMedia_SendVideoTask(void *arg);

/* Private variables -------------------------------------------------------------*/
static T_PsdkCameraMediaDownloadPlaybackHandler s_psdkCameraMedia = {0};
static T_PsdkPlaybackInfo s_playbackInfo = {0};
static T_PsdkTaskHandle s_userSendVideoThread;
static T_UtilBuffer s_mediaPlayCommandBufferHandler = {0};
static T_PsdkMutexHandle s_mediaPlayCommandBufferMutex = {0};
static uint8_t s_mediaPlayCommandBuffer[sizeof(T_TestPayloadCameraPlaybackCommand) * 32] = {0};
static const char *s_frameKeyChar = "[PACKET]";
static const char *s_frameDurationTimeKeyChar = "duration_time";
static const char *s_framePositionKeyChar = "pos";
static const char *s_frameSizeKeyChar = "size";
static T_PsdkMediaFileHandle s_mediaFileThumbNailHandle;
static T_PsdkMediaFileHandle s_mediaFileScreenNailHandle;
static const uint8_t s_frameAudInfo[VIDEO_FRAME_AUD_LEN] = {0x00, 0x00, 0x00, 0x01, 0x09, 0x10};

/* Exported functions definition ---------------------------------------------*/
T_PsdkReturnCode PsdkTest_CameraMediaInit(void)
{
    T_PsdkReturnCode psdkStat;

    s_psdkCameraMedia.GetMediaFileDir = GetMediaFileDir;
    s_psdkCameraMedia.GetMediaFileOriginInfo = PsdkTest_CameraMediaGetFileInfo;
    s_psdkCameraMedia.GetMediaFileOriginData = GetMediaFileOriginData;

    s_psdkCameraMedia.CreateMediaFileThumbNail = CreateMediaFileThumbNail;
    s_psdkCameraMedia.GetMediaFileThumbNailInfo = GetMediaFileThumbNailInfo;
    s_psdkCameraMedia.GetMediaFileThumbNailData = GetMediaFileThumbNailData;
    s_psdkCameraMedia.DestroyMediaFileThumbNail = DestroyMediaFileThumbNail;

    s_psdkCameraMedia.CreateMediaFileScreenNail = CreateMediaFileScreenNail;
    s_psdkCameraMedia.GetMediaFileScreenNailInfo = GetMediaFileScreenNailInfo;
    s_psdkCameraMedia.GetMediaFileScreenNailData = GetMediaFileScreenNailData;
    s_psdkCameraMedia.DestroyMediaFileScreenNail = DestroyMediaFileScreenNail;

    s_psdkCameraMedia.DeleteMediaFile = DeleteMediaFile;

    s_psdkCameraMedia.SetMediaPlaybackFile = SetMediaPlaybackFile;

    s_psdkCameraMedia.StartMediaPlayback = StartMediaPlayback;
    s_psdkCameraMedia.StopMediaPlayback = StopMediaPlayback;
    s_psdkCameraMedia.PauseMediaPlayback = PauseMediaPlayback;
    s_psdkCameraMedia.SeekMediaPlayback = SeekMediaPlayback;
    s_psdkCameraMedia.GetMediaPlaybackStatus = GetMediaPlaybackStatus;

    s_psdkCameraMedia.StartDownloadNotification = StartDownloadNotification;
    s_psdkCameraMedia.StopDownloadNotification = StopDownloadNotification;

    if (PsdkOsal_MutexCreate(&s_mediaPlayCommandBufferMutex) != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("mutex create error");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    UtilBuffer_Init(&s_mediaPlayCommandBufferHandler, s_mediaPlayCommandBuffer, sizeof(s_mediaPlayCommandBuffer));

    psdkStat = PsdkPayloadCamera_RegMediaDownloadPlaybackHandler(&s_psdkCameraMedia);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("psdk camera media function init error.");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    if (PsdkOsal_TaskCreate(&s_userSendVideoThread, UserCameraMedia_SendVideoTask, "user_camera_media_task", 2048,
                            NULL) != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("user send video task create error.");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_PsdkReturnCode PsdkTest_CameraMediaGetFileInfo(const char *filePath, T_PsdkCameraMediaFileInfo *fileInfo)
{
    T_PsdkReturnCode psdkStat;
    T_PsdkMediaFileHandle mediaFileHandle;

    psdkStat = PsdkMediaFile_CreateHandle(filePath, &mediaFileHandle);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Media file create handle error stat:0x%08llX", psdkStat);
        return psdkStat;
    }

    psdkStat = PsdkMediaFile_GetMediaFileType(mediaFileHandle, &fileInfo->type);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Media file get type error stat:0x%08llX", psdkStat);
        goto out;
    }

    psdkStat = PsdkMediaFile_GetMediaFileAttr(mediaFileHandle, &fileInfo->mediaFileAttr);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Media file get attr error stat:0x%08llX", psdkStat);
        goto out;
    }

    psdkStat = PsdkMediaFile_GetFileSizeOrg(mediaFileHandle, &fileInfo->fileSize);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Media file get size error stat:0x%08llX", psdkStat);
        goto out;
    }

out:
    psdkStat = PsdkMediaFile_DestroyHandle(mediaFileHandle);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Media file destroy handle error stat:0x%08llX", psdkStat);
        return psdkStat;
    }

    return psdkStat;
}

/* Private functions definition-----------------------------------------------*/
static T_PsdkReturnCode PsdkPlayback_StopPlay(T_PsdkPlaybackInfo *playbackInfo)
{
    T_PsdkReturnCode psdkStat;

    psdkStat = PsdkPlayback_StopPlayProcess();
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("stop play error ");
    }

    playbackInfo->isInPlayProcess = 0;
    playbackInfo->playPosMs = 0;

    return psdkStat;
}

static T_PsdkReturnCode PsdkPlayback_PausePlay(T_PsdkPlaybackInfo *playbackInfo)
{
    T_PsdkReturnCode psdkStat = PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    T_TestPayloadCameraPlaybackCommand playbackCommand = {0};
    if (playbackInfo->isInPlayProcess) {
        playbackCommand.command = TEST_PAYLOAD_CAMERA_MEDIA_PLAY_COMMAND_PAUSE;

        if (PsdkOsal_MutexLock(s_mediaPlayCommandBufferMutex) != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            PsdkLogger_UserLogError("mutex lock error");
            return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
        }

        if (UtilBuffer_GetUnusedSize(&s_mediaPlayCommandBufferHandler) >= sizeof(T_TestPayloadCameraPlaybackCommand)) {
            UtilBuffer_Put(&s_mediaPlayCommandBufferHandler, (const uint8_t *) &playbackCommand,
                           sizeof(T_TestPayloadCameraPlaybackCommand));
        } else {
            PsdkLogger_UserLogError("Media playback command buffer is full.");
            psdkStat = PSDK_ERROR_SYSTEM_MODULE_CODE_OUT_OF_RANGE;
        }

        if (PsdkOsal_MutexUnlock(s_mediaPlayCommandBufferMutex) != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            PsdkLogger_UserLogError("mutex unlock error");
            return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
        }
    }

    playbackInfo->isInPlayProcess = 0;

    return psdkStat;
}

static T_PsdkReturnCode PsdkPlayback_SetPlayFile(T_PsdkPlaybackInfo *playbackInfo, const char *filePath, uint16_t index)
{
    T_PsdkReturnCode psdkStat;

    if (strlen(filePath) > PSDK_MEDIA_FILE_PATH_LEN_MAX) {
        PsdkLogger_UserLogError("Psdk playback file path out of length range error\n");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    strcpy(playbackInfo->filePath, filePath);
    playbackInfo->videoIndex = index;

    psdkStat = PsdkPlayback_GetVideoLengthMs(filePath, &playbackInfo->videoLengthMs);

    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode PsdkPlayback_SeekPlay(T_PsdkPlaybackInfo *playbackInfo, uint32_t seekPos)
{
    T_PsdkRunTimeStamps ti;
    T_PsdkReturnCode psdkStat;

    psdkStat = PsdkPlayback_PausePlay(playbackInfo);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("pause play error \n");
        return psdkStat;
    }

    playbackInfo->playPosMs = seekPos;
    psdkStat = PsdkPlayback_StartPlayProcess(playbackInfo->filePath, playbackInfo->playPosMs);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("start playback process error \n");
        return psdkStat;
    }

    playbackInfo->isInPlayProcess = 1;
    ti = PsdkUtilTime_GetRunTimeStamps();
    playbackInfo->startPlayTimestampsUs = ti.realUsec - playbackInfo->playPosMs * 1000;

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode PsdkPlayback_StartPlay(T_PsdkPlaybackInfo *playbackInfo)
{
    T_PsdkRunTimeStamps ti;
    T_PsdkReturnCode psdkStat;

    if (playbackInfo->isInPlayProcess == 1) {
        //already in playing, return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    }

    psdkStat = PsdkPlayback_StartPlayProcess(playbackInfo->filePath, playbackInfo->playPosMs);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("start play process error \n");
        return psdkStat;
    }

    playbackInfo->isInPlayProcess = 1;

    ti = PsdkUtilTime_GetRunTimeStamps();
    playbackInfo->startPlayTimestampsUs = ti.realUsec - playbackInfo->playPosMs * 1000;

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode PsdkPlayback_GetPlaybackStatus(T_PsdkPlaybackInfo *playbackInfo,
                                                       T_PsdkCameraPlaybackStatus *playbackStatus)
{
    T_PsdkRunTimeStamps timeStamps;

    memset(playbackStatus, 0, sizeof(T_PsdkCameraPlaybackStatus));

    //update playback pos info
    if (playbackInfo->isInPlayProcess) {
        timeStamps = PsdkUtilTime_GetRunTimeStamps();
        playbackInfo->playPosMs = (timeStamps.realUsec - playbackInfo->startPlayTimestampsUs) / 1000;

        if (playbackInfo->playPosMs >= playbackInfo->videoLengthMs) {
            if (PsdkPlayback_PausePlay(playbackInfo) != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
            }
        }
    }

    //set playback status
    if (playbackInfo->isInPlayProcess == 0 && playbackInfo->playPosMs != 0) {
        playbackStatus->playbackMode = PSDK_CAMERA_PLAYBACK_MODE_PAUSE;
    } else if (playbackInfo->isInPlayProcess) {
        playbackStatus->playbackMode = PSDK_CAMERA_PLAYBACK_MODE_PLAY;
    } else {
        playbackStatus->playbackMode = PSDK_CAMERA_PLAYBACK_MODE_STOP;
    }

    playbackStatus->playPosMs = playbackInfo->playPosMs;
    playbackStatus->videoLengthMs = playbackInfo->videoLengthMs;

    if (playbackInfo->videoLengthMs != 0) {
        playbackStatus->videoPlayProcess =
            (playbackInfo->videoLengthMs - playbackInfo->playPosMs) / playbackInfo->videoLengthMs;
    } else {
        playbackStatus->videoPlayProcess = 0;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode PsdkPlayback_GetVideoLengthMs(const char *filePath, uint32_t *videoLengthMs)
{
    FILE *fp;
    T_PsdkReturnCode psdkStat;
    char ffmpegCmdStr[FFMPEG_CMD_BUF_SIZE];
    float hour, minute, second;
    char tempTailStr[128];
    int ret;

    snprintf(ffmpegCmdStr, FFMPEG_CMD_BUF_SIZE, "ffmpeg -i \"%s\" 2>&1 | grep \"Duration\"", filePath);
    fp = popen(ffmpegCmdStr, "r");

    if (fp == NULL) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    ret = fscanf(fp, "  Duration: %f:%f:%f,%127s", &hour, &minute, &second, tempTailStr);
    if (ret <= 0) {
        PsdkLogger_UserLogError("MP4 File Get Duration Error\n");
        psdkStat = PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
        goto out;
    }

    *videoLengthMs = (uint32_t) ((hour * 3600 + minute * 60 + second) * 1000);
    psdkStat = PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;

out:
    pclose(fp);

    return psdkStat;
}

static T_PsdkReturnCode PsdkPlayback_StartPlayProcess(const char *filePath, uint32_t playPosMs)
{
    T_PsdkReturnCode psdkStat = PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    T_TestPayloadCameraPlaybackCommand mediaPlayCommand = {0};

    mediaPlayCommand.command = TEST_PAYLOAD_CAMERA_MEDIA_PLAY_COMMAND_START;
    mediaPlayCommand.timeMs = playPosMs;

    if (strlen(filePath) >= sizeof(mediaPlayCommand.path)) {
        PsdkLogger_UserLogError("File path is too long.");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_OUT_OF_RANGE;
    }
    memcpy(mediaPlayCommand.path, filePath, strlen(filePath));

    if (PsdkOsal_MutexLock(s_mediaPlayCommandBufferMutex) != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("mutex lock error");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    if (UtilBuffer_GetUnusedSize(&s_mediaPlayCommandBufferHandler) >= sizeof(T_TestPayloadCameraPlaybackCommand)) {
        UtilBuffer_Put(&s_mediaPlayCommandBufferHandler, (const uint8_t *) &mediaPlayCommand,
                       sizeof(T_TestPayloadCameraPlaybackCommand));
    } else {
        PsdkLogger_UserLogError("Media playback command buffer is full.");
        psdkStat = PSDK_ERROR_SYSTEM_MODULE_CODE_OUT_OF_RANGE;
    }

    if (PsdkOsal_MutexUnlock(s_mediaPlayCommandBufferMutex) != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("mutex unlock error");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return psdkStat;
}

static T_PsdkReturnCode PsdkPlayback_StopPlayProcess(void)
{
    T_PsdkReturnCode psdkStat = PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    T_TestPayloadCameraPlaybackCommand playbackCommand = {0};

    playbackCommand.command = TEST_PAYLOAD_CAMERA_MEDIA_PLAY_COMMAND_STOP;

    if (PsdkOsal_MutexLock(s_mediaPlayCommandBufferMutex) != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("mutex lock error");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    if (UtilBuffer_GetUnusedSize(&s_mediaPlayCommandBufferHandler) >= sizeof(T_TestPayloadCameraPlaybackCommand)) {
        UtilBuffer_Put(&s_mediaPlayCommandBufferHandler, (const uint8_t *) &playbackCommand,
                       sizeof(T_TestPayloadCameraPlaybackCommand));
    } else {
        PsdkLogger_UserLogError("Media playback command buffer is full.");
        psdkStat = PSDK_ERROR_SYSTEM_MODULE_CODE_OUT_OF_RANGE;
    }

    if (PsdkOsal_MutexUnlock(s_mediaPlayCommandBufferMutex) != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("mutex unlock error");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return psdkStat;
}

static T_PsdkReturnCode
PsdkPlayback_VideoFileTranscode(const char *inPath, const char *outFormat, char *outPath, uint16_t outPathBufferSize)
{
    T_PsdkReturnCode psdkStat;
    T_PsdkReturnCode returnCode = PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    FILE *fpCommand = NULL;
    char ffmpegCmdStr[FFMPEG_CMD_BUF_SIZE];
    char *directory = NULL;

    directory = PsdkOsal_Malloc(PSDK_MEDIA_FILE_PATH_LEN_MAX);
    if (directory == NULL) {
        PsdkLogger_UserLogError("malloc memory for directory fail.");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_MEMORY_ALLOC_FAILED;
    }

    psdkStat = PsdkUserUtil_GetCurrentFileDirPath(inPath, PSDK_MEDIA_FILE_PATH_LEN_MAX, directory);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("get directory of file error: 0x%08llX.", psdkStat);
        returnCode = psdkStat;
        goto out;
    }

    snprintf(outPath, outPathBufferSize, "%sout.%s", directory, outFormat);
    snprintf(ffmpegCmdStr, FFMPEG_CMD_BUF_SIZE,
             "echo \"y\" | ffmpeg -i \"%s\" -codec copy -f \"%s\" \"%s\" 1>/dev/null 2>&1", inPath,
             outFormat, outPath);
    fpCommand = popen(ffmpegCmdStr, "r");
    if (fpCommand == NULL) {
        PsdkLogger_UserLogError("execute transcode command fail.");
        returnCode = PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
        goto out;
    }

    pclose(fpCommand);

out:
    PsdkOsal_Free(directory);

    return returnCode;
}

static T_PsdkReturnCode
PsdkPlayback_GetFrameInfoOfVideoFile(const char *path, T_TestPayloadCameraVideoFrameInfo *frameInfo,
                                     uint32_t frameInfoBufferCount, uint32_t *frameCount)
{
    long ret;
    T_PsdkReturnCode returnCode = PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    FILE *fpCommand = NULL;
    char ffmpegCmdStr[FFMPEG_CMD_BUF_SIZE];
    char *frameInfoString = NULL;
    char *frameLocation = NULL;
    char frameParameterFormat[50] = {0};
    char *frameDurationTimeLocation = NULL;
    float frameDurationTimeS = 0;
    uint32_t frameNumber = 0;
    char *framePositionLocation = NULL;
    uint32_t framePosition = 0;
    char *frameSizeLocation = NULL;
    uint32_t frameSize = 0;

    frameInfoString = PsdkOsal_Malloc(VIDEO_FRAME_MAX_COUNT * 1024);
    if (frameInfoString == NULL) {
        PsdkLogger_UserLogError("malloc memory for frame info fail.");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_MEMORY_ALLOC_FAILED;
    }
    memset(frameInfoString, 0, VIDEO_FRAME_MAX_COUNT * 1024);

    snprintf(ffmpegCmdStr, FFMPEG_CMD_BUF_SIZE, "ffprobe -show_packets \"%s\" 2>/dev/null", path);
    fpCommand = popen(ffmpegCmdStr, "r");
    if (fpCommand == NULL) {
        PsdkLogger_UserLogError("execute show frames commands fail.");
        returnCode = PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
        goto out2;
    }

    ret = (long) fread(frameInfoString, 1, VIDEO_FRAME_MAX_COUNT * 1024, fpCommand);
    if (ret < 0) {
        PsdkLogger_UserLogError("read show frames commands result error.");
        goto out1;
    }
    frameInfoString[ret] = '\0';

    frameLocation = frameInfoString;
    *frameCount = 0;
    while (1) {
        // find frame
        frameLocation = strstr(frameLocation, s_frameKeyChar);
        if (frameLocation == NULL) {
            PsdkLogger_UserLogDebug("reach file tail.");
            break;
        }

        // find frame duration
        frameDurationTimeLocation = strstr(frameLocation, s_frameDurationTimeKeyChar);
        if (frameDurationTimeLocation == NULL) {
            PsdkLogger_UserLogError("can not find pkt_duration_time.");
            returnCode = PSDK_ERROR_SYSTEM_MODULE_CODE_NOT_FOUND;
            goto out1;
        }

        ret = snprintf(frameParameterFormat, sizeof(frameParameterFormat), "%s=%%f", s_frameDurationTimeKeyChar);
        if (ret < 0) {
            PsdkLogger_UserLogError("snprintf frameParameterFormat fail.");
            returnCode = PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
            goto out1;
        }

        ret = sscanf(frameDurationTimeLocation, frameParameterFormat, &frameDurationTimeS);
        if (ret <= 0) {
            PsdkLogger_UserLogError("can not find pkt_duration_time.");
            returnCode = PSDK_ERROR_SYSTEM_MODULE_CODE_NOT_FOUND;
            goto out1;
        }
        frameInfo[frameNumber].durationS = frameDurationTimeS;

        // find frame position
        framePositionLocation = strstr(frameLocation, s_framePositionKeyChar);
        if (framePositionLocation == NULL) {
            PsdkLogger_UserLogError("can not found pkt_pos.");
            returnCode = PSDK_ERROR_SYSTEM_MODULE_CODE_NOT_FOUND;
            goto out1;
        }

        ret = snprintf(frameParameterFormat, sizeof(frameParameterFormat), "%s=%%d", s_framePositionKeyChar);
        if (ret < 0) {
            PsdkLogger_UserLogError("snprintf frameParameterFormat fail.");
            returnCode = PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
            goto out1;
        }

        ret = sscanf(framePositionLocation, frameParameterFormat, &framePosition);
        if (ret <= 0) {
            PsdkLogger_UserLogError("can not found pkt_pos.");
            returnCode = PSDK_ERROR_SYSTEM_MODULE_CODE_NOT_FOUND;
            goto out1;
        }
        frameInfo[frameNumber].positionInFile = framePosition;

        // find frame size
        frameSizeLocation = strstr(frameLocation, s_frameSizeKeyChar);
        if (frameSizeLocation == NULL) {
            PsdkLogger_UserLogError("can not found pkt_size.");
            returnCode = PSDK_ERROR_SYSTEM_MODULE_CODE_NOT_FOUND;
            goto out1;
        }

        ret = snprintf(frameParameterFormat, sizeof(frameParameterFormat), "%s=%%d", s_frameSizeKeyChar);
        if (ret < 0) {
            PsdkLogger_UserLogError("snprintf frameParameterFormat fail.");
            returnCode = PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
            goto out1;
        }

        ret = sscanf(frameSizeLocation, frameParameterFormat, &frameSize);
        if (ret <= 0) {
            PsdkLogger_UserLogError("can not find pkt_size.");
            returnCode = PSDK_ERROR_SYSTEM_MODULE_CODE_NOT_FOUND;
            goto out1;
        }
        frameInfo[frameNumber].size = frameSize;

        frameLocation += strlen(s_frameKeyChar);
        frameNumber++;
        (*frameCount)++;

        if (frameNumber >= frameInfoBufferCount) {
            PsdkLogger_UserLogError("frame buffer is full.");
            returnCode = PSDK_ERROR_SYSTEM_MODULE_CODE_OUT_OF_RANGE;
            goto out1;
        }
    }

out1:
    pclose(fpCommand);

out2:
    PsdkOsal_Free(frameInfoString);

    return returnCode;
}

static T_PsdkReturnCode PsdkPlayback_GetFrameRateOfVideoFile(const char *path, float *frameRate)
{
    int ret;
    T_PsdkReturnCode psdkStat = PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    FILE *fpCommand = NULL;
    char ffmpegCmdStr[FFMPEG_CMD_BUF_SIZE] = {0};
    int frameRateMolecule = 0;
    int frameRateDenominator = 0;

    snprintf(ffmpegCmdStr, FFMPEG_CMD_BUF_SIZE, "ffprobe -show_streams \"%s\" 2>/dev/null | grep r_frame_rate", path);
    fpCommand = popen(ffmpegCmdStr, "r");
    if (fpCommand == NULL) {
        PsdkLogger_UserLogError("execute show frame rate command fail.");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    ret = fscanf(fpCommand, "r_frame_rate=%d/%d", &frameRateMolecule, &frameRateDenominator);
    if (ret <= 0) {
        PsdkLogger_UserLogError("can not find frame rate.");
        psdkStat = PSDK_ERROR_SYSTEM_MODULE_CODE_NOT_FOUND;
        goto out;
    }
    *frameRate = (float) frameRateMolecule / (float) frameRateDenominator;

out:
    pclose(fpCommand);

    return psdkStat;
}

static T_PsdkReturnCode
PsdkPlayback_GetFrameNumberByTime(T_TestPayloadCameraVideoFrameInfo *frameInfo, uint32_t frameCount,
                                  uint32_t *frameNumber, uint32_t timeMs)
{
    uint32_t i = 0;
    double camulativeTimeS = 0;
    double timeS = (double) timeMs / 1000.0;

    for (i = 0; i < frameCount; ++i) {
        camulativeTimeS += frameInfo[i].durationS;

        if (camulativeTimeS >= timeS) {
            *frameNumber = i;
            return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
        }
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_NOT_FOUND;
}

static T_PsdkReturnCode GetMediaFileDir(char *dirPath)
{
    strcpy(dirPath, PSDK_MEDIA_FILE_PATH);

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode GetMediaFileOriginData(const char *filePath, uint32_t offset, uint32_t length, uint8_t *data)
{
    T_PsdkReturnCode psdkStat;
    uint16_t realLen = 0;
    T_PsdkMediaFileHandle mediaFileHandle;

    psdkStat = PsdkMediaFile_CreateHandle(filePath, &mediaFileHandle);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Media file create handle error stat:0x%08llX", psdkStat);
        return psdkStat;
    }

    psdkStat = PsdkMediaFile_GetDataOrg(mediaFileHandle, offset, length, data, &realLen);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Media file get data error stat:0x%08llX", psdkStat);
        return psdkStat;
    }

    psdkStat = PsdkMediaFile_DestroyHandle(mediaFileHandle);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Media file destroy handle error stat:0x%08llX", psdkStat);
        return psdkStat;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode CreateMediaFileThumbNail(const char *filePath)
{
    T_PsdkReturnCode psdkStat;

    psdkStat = PsdkMediaFile_CreateHandle(filePath, &s_mediaFileThumbNailHandle);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Media file create handle error stat:0x%08llX", psdkStat);
        return psdkStat;
    }

    psdkStat = PsdkMediaFile_CreateThm(s_mediaFileThumbNailHandle);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Media file create thumb nail error stat:0x%08llX", psdkStat);
        return psdkStat;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode GetMediaFileThumbNailInfo(const char *filePath, T_PsdkCameraMediaFileInfo *fileInfo)
{
    T_PsdkReturnCode psdkStat;

    USER_UTIL_UNUSED(filePath);

    if (s_mediaFileThumbNailHandle == NULL) {
        PsdkLogger_UserLogError("Media file thumb nail handle null error");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    psdkStat = PsdkMediaFile_GetMediaFileType(s_mediaFileThumbNailHandle, &fileInfo->type);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Media file get type error stat:0x%08llX", psdkStat);
        return psdkStat;
    }

    psdkStat = PsdkMediaFile_GetMediaFileAttr(s_mediaFileThumbNailHandle, &fileInfo->mediaFileAttr);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Media file get attr error stat:0x%08llX", psdkStat);
        return psdkStat;
    }

    psdkStat = PsdkMediaFile_GetFileSizeThm(s_mediaFileThumbNailHandle, &fileInfo->fileSize);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Media file get size error stat:0x%08llX", psdkStat);
        return psdkStat;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode GetMediaFileThumbNailData(const char *filePath, uint32_t offset, uint32_t length, uint8_t *data)
{
    T_PsdkReturnCode psdkStat;
    uint16_t realLen = 0;

    USER_UTIL_UNUSED(filePath);

    if (s_mediaFileThumbNailHandle == NULL) {
        PsdkLogger_UserLogError("Media file thumb nail handle null error");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    psdkStat = PsdkMediaFile_GetDataThm(s_mediaFileThumbNailHandle, offset, length, data, &realLen);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Media file get data error stat:0x%08llX", psdkStat);
        return psdkStat;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode DestroyMediaFileThumbNail(const char *filePath)
{
    T_PsdkReturnCode psdkStat;

    USER_UTIL_UNUSED(filePath);

    if (s_mediaFileThumbNailHandle == NULL) {
        PsdkLogger_UserLogError("Media file thumb nail handle null error");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    psdkStat = PsdkMediaFile_DestoryThm(s_mediaFileThumbNailHandle);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Media file destroy thumb nail error stat:0x%08llX", psdkStat);
        return psdkStat;
    }

    psdkStat = PsdkMediaFile_DestroyHandle(s_mediaFileThumbNailHandle);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Media file destroy handle error stat:0x%08llX", psdkStat);
        return psdkStat;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode CreateMediaFileScreenNail(const char *filePath)
{
    T_PsdkReturnCode psdkStat;

    psdkStat = PsdkMediaFile_CreateHandle(filePath, &s_mediaFileScreenNailHandle);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Media file create handle error stat:0x%08llX", psdkStat);
        return psdkStat;
    }

    psdkStat = PsdkMediaFile_CreateScr(s_mediaFileScreenNailHandle);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Media file create screen nail error stat:0x%08llX", psdkStat);
        return psdkStat;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode GetMediaFileScreenNailInfo(const char *filePath, T_PsdkCameraMediaFileInfo *fileInfo)
{
    T_PsdkReturnCode psdkStat;

    USER_UTIL_UNUSED(filePath);

    if (s_mediaFileScreenNailHandle == NULL) {
        PsdkLogger_UserLogError("Media file screen nail handle null error");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    psdkStat = PsdkMediaFile_GetMediaFileType(s_mediaFileScreenNailHandle, &fileInfo->type);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Media file get type error stat:0x%08llX", psdkStat);
        return psdkStat;
    }

    psdkStat = PsdkMediaFile_GetMediaFileAttr(s_mediaFileScreenNailHandle, &fileInfo->mediaFileAttr);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Media file get attr error stat:0x%08llX", psdkStat);
        return psdkStat;
    }

    psdkStat = PsdkMediaFile_GetFileSizeScr(s_mediaFileScreenNailHandle, &fileInfo->fileSize);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Media file get size error stat:0x%08llX", psdkStat);
        return psdkStat;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode
GetMediaFileScreenNailData(const char *filePath, uint32_t offset, uint32_t length, uint8_t *data)
{
    T_PsdkReturnCode psdkStat;
    uint16_t realLen = 0;

    USER_UTIL_UNUSED(filePath);

    if (s_mediaFileScreenNailHandle == NULL) {
        PsdkLogger_UserLogError("Media file screen nail handle null error");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    psdkStat = PsdkMediaFile_GetDataScr(s_mediaFileScreenNailHandle, offset, length, data, &realLen);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Media file get size error stat:0x%08llX", psdkStat);
        return psdkStat;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode DestroyMediaFileScreenNail(const char *filePath)
{
    T_PsdkReturnCode psdkStat;

    USER_UTIL_UNUSED(filePath);

    if (s_mediaFileScreenNailHandle == NULL) {
        PsdkLogger_UserLogError("Media file screen nail handle null error");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    psdkStat = PsdkMediaFile_DestroyScr(s_mediaFileScreenNailHandle);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Media file destroy screen nail error stat:0x%08llX", psdkStat);
        return psdkStat;
    }

    psdkStat = PsdkMediaFile_DestroyHandle(s_mediaFileScreenNailHandle);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Media file destroy handle error stat:0x%08llX", psdkStat);
        return psdkStat;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode DeleteMediaFile(char *filePath)
{
    T_PsdkReturnCode psdkStat;

    psdkStat = PsdkFile_Delete(filePath);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Media file delete error stat:0x%08llX", psdkStat);
        return psdkStat;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode SetMediaPlaybackFile(const char *filePath)
{
    PsdkLogger_UserLogDebug("set media playback file:%s", filePath);
    T_PsdkReturnCode psdkStat;

    psdkStat = PsdkPlayback_StopPlay(&s_playbackInfo);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        return psdkStat;
    }

    psdkStat = PsdkPlayback_SetPlayFile(&s_playbackInfo, filePath, 0);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        return psdkStat;
    }

    psdkStat = PsdkPlayback_StartPlay(&s_playbackInfo);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        return psdkStat;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode StartMediaPlayback(void)
{
    T_PsdkReturnCode psdkStat;

    PsdkLogger_UserLogDebug("start media playback");
    psdkStat = PsdkPlayback_StartPlay(&s_playbackInfo);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("start media playback status error, stat:0x%08llX", psdkStat);
        return psdkStat;
    }

    return psdkStat;
}

static T_PsdkReturnCode StopMediaPlayback(void)
{
    T_PsdkReturnCode psdkStat;

    PsdkLogger_UserLogDebug("stop media playback");
    psdkStat = PsdkPlayback_StopPlay(&s_playbackInfo);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("stop media playback error, stat:0x%08llX", psdkStat);
        return psdkStat;
    }

    return psdkStat;
}

static T_PsdkReturnCode PauseMediaPlayback(void)
{
    T_PsdkReturnCode psdkStat;

    PsdkLogger_UserLogDebug("pause media playback");
    psdkStat = PsdkPlayback_PausePlay(&s_playbackInfo);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("pause media playback error, stat:0x%08llX", psdkStat);
        return psdkStat;
    }

    return psdkStat;
}

static T_PsdkReturnCode SeekMediaPlayback(uint32_t playbackPosition)
{
    T_PsdkReturnCode psdkStat;

    PsdkLogger_UserLogDebug("seek media playback:%d", playbackPosition);
    psdkStat = PsdkPlayback_SeekPlay(&s_playbackInfo, playbackPosition);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("seek media playback error, stat:0x%08llX", psdkStat);
        return psdkStat;
    }

    return psdkStat;
}

static T_PsdkReturnCode GetMediaPlaybackStatus(T_PsdkCameraPlaybackStatus *status)
{
    T_PsdkReturnCode psdkStat;

    psdkStat = PsdkPlayback_GetPlaybackStatus(&s_playbackInfo, status);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("get playback status error, stat:0x%08llX", psdkStat);
        return psdkStat;
    }

    status->videoPlayProcess = (uint8_t) (((float) s_playbackInfo.playPosMs / (float) s_playbackInfo.videoLengthMs) *
                                          100);

    PsdkLogger_UserLogDebug("get media playback status %d %d %d %d", status->videoPlayProcess, status->playPosMs,
                            status->videoLengthMs, status->playbackMode);

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode StartDownloadNotification(void)
{
    T_PsdkReturnCode returnCode;
    T_PsdkDataChannelBandwidthProportionOfHighspeedChannel channel = {0};

    PsdkLogger_UserLogDebug("media download start notification.");

    channel.dataStream = 0;
    channel.videoStream = 0;
    channel.downloadStream = 100;

    returnCode = PsdkDataChannel_SetBandwidthProportionForHighspeedChannel(channel);
    if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("set bandwidth proportion for high speed channel error, stat:0x%08llX.", returnCode);
        return returnCode;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode StopDownloadNotification(void)
{
    T_PsdkReturnCode returnCode;
    T_PsdkDataChannelBandwidthProportionOfHighspeedChannel channel = {0};

    PsdkLogger_UserLogDebug("media download stop notification.");

    channel.dataStream = 10;
    channel.videoStream = 60;
    channel.downloadStream = 30;

    returnCode = PsdkDataChannel_SetBandwidthProportionForHighspeedChannel(channel);
    if (returnCode != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("set bandwidth proportion for high speed channel error, stat:0x%08llX.", returnCode);
        return returnCode;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

#ifndef __CC_ARM
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wreturn-type"
#endif

static void *UserCameraMedia_SendVideoTask(void *arg)
{
    int ret;
    T_PsdkReturnCode psdkStat;
    static uint32_t sendVideoStep = 0;
    FILE *fpFile = NULL;
    unsigned long dataLength = 0;
    uint16_t lengthOfDataToBeSent = 0;
    int lengthOfDataHaveBeenSent = 0;
    char *dataBuffer = NULL;
    T_TestPayloadCameraPlaybackCommand playbackCommand = {0};
    uint16_t bufferReadSize = 0;
    char *videoFilePath = NULL;
    char *transcodedFilePath = NULL;
    float frameRate = 1.0f;
    T_TestPayloadCameraVideoFrameInfo *frameInfo = NULL;
    uint32_t frameNumber = 0;
    uint32_t frameCount = 0;
    uint32_t startTimeMs = 0;
    bool sendVideoFlag = true;
    bool sendOneTimeFlag = false;
    T_PsdkDataChannelState videoStreamState = {0};
    E_PsdkCameraMode mode = PSDK_CAMERA_MODE_SHOOT_PHOTO;
    uint32_t frameBufSize = 0;
    E_PsdkCameraVideoStreamType videoStreamType;

    USER_UTIL_UNUSED(arg);

    videoFilePath = PsdkOsal_Malloc(PSDK_MEDIA_FILE_PATH_LEN_MAX);
    if (videoFilePath == NULL) {
        PsdkLogger_UserLogError("malloc memory for video file path fail.");
        exit(1);
    }

    transcodedFilePath = PsdkOsal_Malloc(PSDK_MEDIA_FILE_PATH_LEN_MAX);
    if (transcodedFilePath == NULL) {
        PsdkLogger_UserLogError("malloc memory for transcoded file path fail.");
        exit(1);
    }

    frameInfo = PsdkOsal_Malloc(VIDEO_FRAME_MAX_COUNT * sizeof(T_TestPayloadCameraVideoFrameInfo));
    if (frameInfo == NULL) {
        PsdkLogger_UserLogError("malloc memory for frame info fail.");
        exit(1);
    }
    memset(frameInfo, 0, VIDEO_FRAME_MAX_COUNT * sizeof(T_TestPayloadCameraVideoFrameInfo));

    psdkStat = PsdkPlayback_StopPlayProcess();
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("stop playback and start liveview error: 0x%08llX.", psdkStat);
        exit(1);
    }

    while (1) {
        PsdkOsal_TaskSleepMs(1000 / SEND_VIDEO_TASK_FREQ);

        // response playback command
        if (PsdkOsal_MutexLock(s_mediaPlayCommandBufferMutex) != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            PsdkLogger_UserLogError("mutex lock error");
            continue;
        }

        bufferReadSize = UtilBuffer_Get(&s_mediaPlayCommandBufferHandler, (uint8_t *) &playbackCommand,
                                        sizeof(T_TestPayloadCameraPlaybackCommand));

        if (PsdkOsal_MutexUnlock(s_mediaPlayCommandBufferMutex) != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            PsdkLogger_UserLogError("mutex unlock error");
            continue;
        }

        if (bufferReadSize != sizeof(T_TestPayloadCameraPlaybackCommand))
            goto send;

        switch (playbackCommand.command) {
            case TEST_PAYLOAD_CAMERA_MEDIA_PLAY_COMMAND_STOP:
                snprintf(videoFilePath, PSDK_MEDIA_FILE_PATH_LEN_MAX, "%s", VIDEO_FILE_PATH);
                startTimeMs = 0;
                sendVideoFlag = true;
                sendOneTimeFlag = false;
                break;
            case TEST_PAYLOAD_CAMERA_MEDIA_PLAY_COMMAND_PAUSE:
                sendVideoFlag = false;
                goto send;
            case TEST_PAYLOAD_CAMERA_MEDIA_PLAY_COMMAND_START:
                snprintf(videoFilePath, PSDK_MEDIA_FILE_PATH_LEN_MAX, "%s", playbackCommand.path);
                startTimeMs = playbackCommand.timeMs;
                sendVideoFlag = true;
                sendOneTimeFlag = true;
                break;
            default:
                PsdkLogger_UserLogError("playback command invalid: %d.", playbackCommand.command);
                sendVideoFlag = false;
                goto send;
        }

        // video send preprocess
        psdkStat = PsdkPlayback_VideoFileTranscode(videoFilePath, "h264", transcodedFilePath,
                                                   PSDK_MEDIA_FILE_PATH_LEN_MAX);
        if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            PsdkLogger_UserLogError("transcode video file error: 0x%08llX.", psdkStat);
            continue;
        }

        psdkStat = PsdkPlayback_GetFrameRateOfVideoFile(transcodedFilePath, &frameRate);
        if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            PsdkLogger_UserLogError("get frame rate of video error: 0x%08llX.", psdkStat);
            continue;
        }

        psdkStat = PsdkPlayback_GetFrameInfoOfVideoFile(transcodedFilePath, frameInfo, VIDEO_FRAME_MAX_COUNT,
                                                        &frameCount);
        if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            PsdkLogger_UserLogError("get frame info of video error: 0x%08llX.", psdkStat);
            continue;
        }

        psdkStat = PsdkPlayback_GetFrameNumberByTime(frameInfo, frameCount, &frameNumber,
                                                     startTimeMs);
        if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            PsdkLogger_UserLogError("get start frame number error: 0x%08llX.", psdkStat);
            continue;
        }

        if (fpFile != NULL)
            fclose(fpFile);

        fpFile = fopen(transcodedFilePath, "rb+");
        if (fpFile == NULL) {
            PsdkLogger_UserLogError("open video file fail.");
            continue;
        }

send:
        if (fpFile == NULL) {
            PsdkLogger_UserLogError("open video file fail.");
            continue;
        }

        if (sendVideoFlag != true)
            continue;

        psdkStat = PsdkTest_CameraGetMode(&mode);
        if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            continue;
        }

        psdkStat = PsdkTest_CameraGetVideoStreamType(&videoStreamType);
        if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            continue;
        }

        if (mode == PSDK_CAMERA_MODE_PLAYBACK && s_playbackInfo.isInPlayProcess == false) {
            continue;
        }

        if (!USER_UTIL_IS_WORK_TURN(sendVideoStep++, frameRate, SEND_VIDEO_TASK_FREQ))
            continue;


        frameBufSize = frameInfo[frameNumber].size;
        if (videoStreamType == PSDK_CAMERA_VIDEO_STREAM_TYPE_H264_DJI_FORMAT) {
            frameBufSize = frameBufSize + VIDEO_FRAME_AUD_LEN;
        }

        dataBuffer = calloc(frameBufSize, 1);
        if (dataBuffer == NULL) {
            PsdkLogger_UserLogError("malloc fail.");
            goto free;
        }

        ret = fseek(fpFile, frameInfo[frameNumber].positionInFile, SEEK_SET);
        if (ret != 0) {
            PsdkLogger_UserLogError("fseek fail.");
            goto free;
        }

        dataLength = fread(dataBuffer, 1, frameInfo[frameNumber].size, fpFile);
        if (dataLength != frameInfo[frameNumber].size) {
            PsdkLogger_UserLogError("read data from video file error.");
        } else {
            PsdkLogger_UserLogDebug("read data from video file success, len = %d B\r\n", dataLength);
        }

        if (videoStreamType == PSDK_CAMERA_VIDEO_STREAM_TYPE_H264_DJI_FORMAT) {
            memcpy(&dataBuffer[frameInfo[frameNumber].size], s_frameAudInfo, VIDEO_FRAME_AUD_LEN);
            dataLength = dataLength + VIDEO_FRAME_AUD_LEN;
        }

        lengthOfDataHaveBeenSent = 0;
        while (dataLength - lengthOfDataHaveBeenSent) {
            lengthOfDataToBeSent = USER_UTIL_MIN(DATA_SEND_FROM_VIDEO_STREAM_MAX_LEN,
                                                 dataLength - lengthOfDataHaveBeenSent);
            psdkStat = PsdkPayloadCamera_SendVideoStream((const uint8_t *) dataBuffer + lengthOfDataHaveBeenSent,
                                                         lengthOfDataToBeSent);
            if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                PsdkLogger_UserLogError("send video stream error: 0x%08llX.", psdkStat);
                goto free;
            }
            lengthOfDataHaveBeenSent += lengthOfDataToBeSent;
        }

        if ((frameNumber++) >= frameCount) {
            PsdkLogger_UserLogDebug("reach file tail.");
            frameNumber = 0;

            if (sendOneTimeFlag == true)
                sendVideoFlag = false;
        }

        psdkStat = PsdkPayloadCamera_GetVideoStreamState(&videoStreamState);
        if (psdkStat == PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            PsdkLogger_UserLogDebug(
                "video stream state: realtimeBandwidthLimit: %d, realtimeBandwidthBeforeFlowController: %d, realtimeBandwidthAfterFlowController:%d busyState: %d.",
                videoStreamState.realtimeBandwidthLimit, videoStreamState.realtimeBandwidthBeforeFlowController,
                videoStreamState.realtimeBandwidthAfterFlowController,
                videoStreamState.busyState);
        } else {
            PsdkLogger_UserLogError("get video stream state error.");
        }

free:
        free(dataBuffer);
    }
}

#ifndef __CC_ARM
#pragma GCC diagnostic pop
#endif

#endif

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
