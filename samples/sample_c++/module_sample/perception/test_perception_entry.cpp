 /**
 ********************************************************************
 * @file    test_perception_entry.cpp
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
#include <dirent.h>
#include "test_perception_entry.hpp"
#include "dji_logger.h"
#include "dji_perception.h"
#include "test_perception.hpp"
#include <iostream>
#include <chrono>
#include <algorithm>
#include <condition_variable>
#include <deque>
#include <thread>
#include <string>
#include <vector>
#include <cstdio>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#ifdef OPEN_CV_INSTALLED

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"

#endif

/* Private constants ---------------------------------------------------------*/
#define USER_PERCEPTION_TASK_STACK_SIZE    (1024)
#define USER_PERCEPTION_DIRECTION_NUM      (12)
#define FPS_STRING_LEN                     (50)
#define PERCEPTION_JPEG_SAMPLE_INTERVAL_MS (2000)
#define PERCEPTION_SAVE_JOB_QUEUE_MAX      (16)
#define PERCEPTION_SAVE_DIR                "./perception_data"

/* Private types -------------------------------------------------------------*/
typedef struct {
    T_DjiPerceptionImageInfo info;
    uint8_t *imageRawBuffer;
    uint32_t bufferLen;
    T_DjiMutexHandle mutex;
    bool gotData;
} T_DjiTestStereoImagePacket;

typedef struct {
    E_DjiPerceptionCameraPosition cameraPosition;
    char const *name;
} T_DjiTestPerceptionCameraPositionName;

typedef struct {
    E_DjiPerceptionDirection direction;
    char const *name;
} T_DjiTestPerceptionDirectionName;

typedef enum {
    DJI_TEST_IMAGE_FORMAT_GRAY = 0,
    DJI_TEST_IMAGE_FORMAT_NV12,
    DJI_TEST_IMAGE_FORMAT_RGB,
} E_DjiTestImageFormat;

typedef struct {
    std::string dirName;
    uint64_t timestamp;
    uint32_t width;
    uint32_t height;
    E_DjiTestImageFormat format;
    std::vector<uint8_t> data;
} T_DjiPerceptionSaveJob;

/* Private values -------------------------------------------------------------*/
static T_DjiTaskHandle s_stereoImageThread;
static T_DjiTestStereoImagePacket s_stereoImagePacket = {
    .info           = {0},
    .imageRawBuffer = nullptr,
    .bufferLen      = 0,
    .mutex          = nullptr,
    .gotData        = false};

static const T_DjiTestPerceptionDirectionName directionName[] = {
    {.direction = DJI_PERCEPTION_RECTIFY_DOWN, .name = "down"},
    {.direction = DJI_PERCEPTION_RECTIFY_FRONT, .name = "front"},
    {.direction = DJI_PERCEPTION_RECTIFY_REAR, .name = "rear"},
    {.direction = DJI_PERCEPTION_RECTIFY_UP, .name = "up"},
    {.direction = DJI_PERCEPTION_RECTIFY_LEFT, .name = "left"},
    {.direction = DJI_PERCEPTION_RECTIFY_RIGHT, .name = "right"},
};

static const T_DjiTestPerceptionCameraPositionName positionName[] = {
    {.cameraPosition = RECTIFY_DOWN_LEFT, .name = "down_l"},
    {.cameraPosition = RECTIFY_DOWN_RIGHT, .name = "down_r"},
    {.cameraPosition = RECTIFY_FRONT_LEFT, .name = "front_l"},
    {.cameraPosition = RECTIFY_FRONT_RIGHT, .name = "front_r"},
    {.cameraPosition = RECTIFY_REAR_LEFT, .name = "rear_l"},
    {.cameraPosition = RECTIFY_REAR_RIGHT, .name = "rear_r"},
    {.cameraPosition = RECTIFY_UP_LEFT, .name = "up_l"},
    {.cameraPosition = RECTIFY_UP_RIGHT, .name = "up_r"},
    {.cameraPosition = RECTIFY_LEFT_LEFT, .name = "left_l"},
    {.cameraPosition = RECTIFY_LEFT_RIGHT, .name = "left_r"},
    {.cameraPosition = RECTIFY_RIGHT_LEFT, .name = "right_l"},
    {.cameraPosition = RECTIFY_RIGHT_RIGHT, .name = "right_r"},
};

static std::mutex s_saveJobMutex;
static std::condition_variable s_saveJobCond;
static std::deque<T_DjiPerceptionSaveJob> s_saveJobQueue;
static std::thread s_saveWorkerThread;
static bool s_saveWorkerStarted = false;
static bool s_saveWorkerStop = false;

/* Private functions declaration ---------------------------------------------*/
static void DjiTest_PerceptionImageCallback(T_DjiPerceptionImageInfo imageInfo, uint8_t *imageRawBuffer,
                                            uint32_t bufferLen);
static void *DjiTest_StereoImagesDisplayTask(void *arg);
static void DjiTest_TrySaveSampledJpeg(T_DjiPerceptionImageInfo imageInfo, const uint8_t *imageRawBuffer,
                                       uint32_t bufferLen);
static void DjiTest_StartSaveWorkerIfNeeded(void);
static void DjiTest_StopSaveWorker(void);
static void DjiTest_StopSaveWorkerAtExit(void);
static void DjiTest_SaveWorkerTask(void);
static void DjiTest_SaveOneJob(const T_DjiPerceptionSaveJob &job);
static bool DjiTest_EnsurePerceptionSaveDir(void);
static const char *DjiTest_GetFormatName(E_DjiTestImageFormat format);

/* Exported functions definition ---------------------------------------------*/
void DjiUser_RunStereoVisionViewSample(void)
{
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    char inputChar;
    char isQuit;
    T_DjiReturnCode returnCode;
    T_DjiPerceptionCameraParametersPacket cameraParametersPacket = {0};

    PerceptionSample *perceptionSample;
    try {
        perceptionSample = new PerceptionSample;
    } catch (...) {
        return;
    }

    returnCode = osalHandler->MutexCreate(&s_stereoImagePacket.mutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Crete mutex failed, return code:0x%08X", returnCode);
        goto DeletePerception;
    }

    returnCode = osalHandler->TaskCreate("user_perception_task", DjiTest_StereoImagesDisplayTask,
                                         USER_PERCEPTION_TASK_STACK_SIZE, &s_stereoImagePacket, &s_stereoImageThread);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Crete task failed, return code:0x%08X", returnCode);
        goto DestroyMutex;
    }

    returnCode = DjiPerception_GetStereoCameraParameters(&cameraParametersPacket);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Get camera parameters failed, return code:0x%08X", returnCode);
        goto DestroyTask;
    }

    for (int i = 0; i < cameraParametersPacket.directionNum; i++) {
        USER_LOG_INFO(" [%-05s] leftIntrinsics = {%f, %f, %f, %f, %f, %f, %f, %f, %f }",
                      directionName[cameraParametersPacket.cameraParameters[i].direction].name,
                      cameraParametersPacket.cameraParameters[i].leftIntrinsics[0],
                      cameraParametersPacket.cameraParameters[i].leftIntrinsics[1],
                      cameraParametersPacket.cameraParameters[i].leftIntrinsics[2],
                      cameraParametersPacket.cameraParameters[i].leftIntrinsics[3],
                      cameraParametersPacket.cameraParameters[i].leftIntrinsics[4],
                      cameraParametersPacket.cameraParameters[i].leftIntrinsics[5],
                      cameraParametersPacket.cameraParameters[i].leftIntrinsics[6],
                      cameraParametersPacket.cameraParameters[i].leftIntrinsics[7],
                      cameraParametersPacket.cameraParameters[i].leftIntrinsics[8]);
        USER_LOG_INFO("[%-05s] rightIntrinsics = {%f, %f, %f, %f, %f, %f, %f, %f, %f }",
                      directionName[cameraParametersPacket.cameraParameters[i].direction].name,
                      cameraParametersPacket.cameraParameters[i].rightIntrinsics[0],
                      cameraParametersPacket.cameraParameters[i].rightIntrinsics[1],
                      cameraParametersPacket.cameraParameters[i].rightIntrinsics[2],
                      cameraParametersPacket.cameraParameters[i].rightIntrinsics[3],
                      cameraParametersPacket.cameraParameters[i].rightIntrinsics[4],
                      cameraParametersPacket.cameraParameters[i].rightIntrinsics[5],
                      cameraParametersPacket.cameraParameters[i].rightIntrinsics[6],
                      cameraParametersPacket.cameraParameters[i].rightIntrinsics[7],
                      cameraParametersPacket.cameraParameters[i].rightIntrinsics[8]);
        USER_LOG_INFO("[%-05s] rotationLeftInRight = {%f, %f, %f, %f, %f, %f, %f, %f, %f }",
                      directionName[cameraParametersPacket.cameraParameters[i].direction].name,
                      cameraParametersPacket.cameraParameters[i].rotationLeftInRight[0],
                      cameraParametersPacket.cameraParameters[i].rotationLeftInRight[1],
                      cameraParametersPacket.cameraParameters[i].rotationLeftInRight[2],
                      cameraParametersPacket.cameraParameters[i].rotationLeftInRight[3],
                      cameraParametersPacket.cameraParameters[i].rotationLeftInRight[4],
                      cameraParametersPacket.cameraParameters[i].rotationLeftInRight[5],
                      cameraParametersPacket.cameraParameters[i].rotationLeftInRight[6],
                      cameraParametersPacket.cameraParameters[i].rotationLeftInRight[7],
                      cameraParametersPacket.cameraParameters[i].rotationLeftInRight[8]);
        USER_LOG_INFO("[%-05s] translationLeftInRight = {%f, %f, %f }\r\n",
                      directionName[cameraParametersPacket.cameraParameters[i].direction].name,
                      cameraParametersPacket.cameraParameters[i].translationLeftInRight[0],
                      cameraParametersPacket.cameraParameters[i].translationLeftInRight[1],
                      cameraParametersPacket.cameraParameters[i].translationLeftInRight[2]);
        osalHandler->TaskSleepMs(100);
    }

    while (true) {
        std::cout
            << "| Available commands:                                            |"
            <<
            std::endl;
        std::cout
            << "| M5TD/M5T only support subscribe options d, f, r:               |"
            <<
            std::endl;
        std::cout
            << "| [d] Subscribe down stereo camera pair images                   |"
            <<
            std::endl;
        std::cout
            << "| [f] Subscribe front stereo camera pair images                  |"
            <<
            std::endl;
        std::cout
            << "| [r] Subscribe rear stereo camera pair images                   |"
            <<
            std::endl;
        std::cout
            << "| [u] Subscribe up stereo camera pair images                     |"
            <<
            std::endl;
        std::cout
            << "| [l] Subscribe left stereo camera pair images                   |"
            <<
            std::endl;
        std::cout
            << "| [t] Subscribe right stereo camera pair images                  |"
            <<
            std::endl;
        std::cout
            << "| [q] quit                                                       |"
            <<
            std::endl;

        std::cin >> inputChar;
        switch (inputChar) {
            case 'd':
                USER_LOG_INFO("Subscribe down stereo camera pair images.");
                returnCode = perceptionSample->SubscribeDownImage(DjiTest_PerceptionImageCallback);
                break;
            case 'f':
                USER_LOG_INFO("Subscribe front stereo camera pair images.");
                returnCode = perceptionSample->SubscribeFrontImage(DjiTest_PerceptionImageCallback);
                break;
            case 'r':
                USER_LOG_INFO("Subscribe rear stereo camera pair images.");
                returnCode = perceptionSample->SubscribeRearImage(DjiTest_PerceptionImageCallback);
                break;
            case 'u':
                USER_LOG_INFO("Subscribe up stereo camera pair images.");
                returnCode = perceptionSample->SubscribeUpImage(DjiTest_PerceptionImageCallback);
                break;
            case 'l':
                USER_LOG_INFO("Subscribe left stereo camera pair images.");
                returnCode = perceptionSample->SubscribeLeftImage(DjiTest_PerceptionImageCallback);
                break;
            case 't':
                USER_LOG_INFO("Subscribe right stereo camera pair images.");
                returnCode = perceptionSample->SubscribeRightImage(DjiTest_PerceptionImageCallback);
                break;
            case 'g':
                USER_LOG_INFO("Do stereo camera parameters subscription");
                break;
            case 'q':
                goto DestroyTask;
            default:
                break;
        }

        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_INFO("Failed to subscribe perception image.");
            goto DestroyTask;
        }

        while (true) {
            cin >> isQuit;
            if (isQuit == 'q' || isQuit == 'Q') {
                break;
            }
        }

        switch (inputChar) {
            case 'd':
                USER_LOG_INFO("Unsubscribe down stereo camera pair images.");
                perceptionSample->UnSubscribeDownImage();
                break;
            case 'f':
                USER_LOG_INFO("Unsubscribe front stereo camera pair images.");
                perceptionSample->UnSubscribeFrontImage();
                break;
            case 'r':
                USER_LOG_INFO("Unsubscribe rear stereo camera pair images.");
                perceptionSample->UnSubscribeRearImage();
                break;
            case 'u':
                USER_LOG_INFO("Unsubscribe up stereo camera pair images.");
                perceptionSample->UnSubscribeUpImage();
                break;
            case 'l':
                USER_LOG_INFO("Unsubscribe left stereo camera pair images.");
                perceptionSample->UnSubscribeLeftImage();
                break;
            case 't':
                USER_LOG_INFO("Unsubscribe right stereo camera pair images.");
                perceptionSample->UnSubscribeRightImage();
                break;
            default:
                break;
        }
#ifdef OPEN_CV_INSTALLED
        cv::destroyAllWindows();
#endif
    }

DestroyTask:
    DjiTest_StopSaveWorker();

    returnCode = osalHandler->TaskDestroy(s_stereoImageThread);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Destroy task failed, return code:0x%08X", returnCode);
    }

#ifdef OPEN_CV_INSTALLED
    cv::destroyAllWindows();
#endif

DestroyMutex:
    returnCode = osalHandler->MutexDestroy(s_stereoImagePacket.mutex);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Destroy mutex failed, return code:0x%08X", returnCode);
    }

DeletePerception:
    delete perceptionSample;
}

/* Private functions definition-----------------------------------------------*/
static void DjiTest_PerceptionImageCallback(T_DjiPerceptionImageInfo imageInfo, uint8_t *imageRawBuffer,
                                            uint32_t bufferLen)
{
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    USER_LOG_INFO("image info : dataId(%d) seq(%d) timestamp(%llu) datatype(%d) index(%d) h(%d) w(%d) dir(%d) "
                  "bpp(%d) bufferlen(%d)", imageInfo.dataId, imageInfo.sequence, imageInfo.timeStamp,
                  imageInfo.dataType,
                  imageInfo.rawInfo.index, imageInfo.rawInfo.height, imageInfo.rawInfo.width,
                  imageInfo.rawInfo.direction,
                  imageInfo.rawInfo.bpp, bufferLen);

    DjiTest_TrySaveSampledJpeg(imageInfo, imageRawBuffer, bufferLen);

    if (imageRawBuffer) {
        osalHandler->MutexLock(s_stereoImagePacket.mutex);
        s_stereoImagePacket.info = imageInfo;
        s_stereoImagePacket.bufferLen = bufferLen;
        if (s_stereoImagePacket.imageRawBuffer) osalHandler->Free(s_stereoImagePacket.imageRawBuffer);
        s_stereoImagePacket.imageRawBuffer = (uint8_t *) osalHandler->Malloc(bufferLen);
        if (s_stereoImagePacket.imageRawBuffer != nullptr) {
            memcpy(s_stereoImagePacket.imageRawBuffer, imageRawBuffer, bufferLen);
            s_stereoImagePacket.gotData = true;
        } else {
            s_stereoImagePacket.gotData = false;
            USER_LOG_WARN("[Perception] Malloc image buffer failed, len=%u", bufferLen);
        }
        osalHandler->MutexUnlock(s_stereoImagePacket.mutex);
    }
}

static void DjiTest_TrySaveSampledJpeg(T_DjiPerceptionImageInfo imageInfo, const uint8_t *imageRawBuffer,
                                       uint32_t bufferLen)
{
    if (imageRawBuffer == nullptr || imageInfo.rawInfo.width == 0 || imageInfo.rawInfo.height == 0) {
        return;
    }

    static uint64_t s_lastSaveTimeMs[USER_PERCEPTION_DIRECTION_NUM] = {0};

    int dirIdx = -1;
    const char *dirName = "unknown";
    for (int i = 0; i < (int) (sizeof(positionName) / sizeof(positionName[0])); ++i) {
        if (positionName[i].cameraPosition == (E_DjiPerceptionCameraPosition) imageInfo.dataType) {
            dirIdx = i;
            dirName = positionName[i].name;
            break;
        }
    }

    if (dirIdx < 0 || dirIdx >= USER_PERCEPTION_DIRECTION_NUM) {
        return;
    }

    uint64_t nowMs = (uint64_t) std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    if (s_lastSaveTimeMs[dirIdx] != 0 && nowMs - s_lastSaveTimeMs[dirIdx] < PERCEPTION_JPEG_SAMPLE_INTERVAL_MS) {
        return;
    }
    s_lastSaveTimeMs[dirIdx] = nowMs;

    const uint32_t width = imageInfo.rawInfo.width;
    const uint32_t height = imageInfo.rawInfo.height;
    const uint32_t graySize = width * height;
    const uint32_t nv12Size = graySize * 3 / 2;

    E_DjiTestImageFormat format;
    if (bufferLen == graySize) {
        format = DJI_TEST_IMAGE_FORMAT_GRAY;
    } else if (bufferLen == nv12Size) {
        format = DJI_TEST_IMAGE_FORMAT_NV12;
    } else if (bufferLen == graySize * 3) {
        format = DJI_TEST_IMAGE_FORMAT_RGB;
    } else {
        USER_LOG_WARN("[Perception] Skip save image, unsupported buffer len=%u, w=%u h=%u type=%u",
                      bufferLen, width, height, imageInfo.dataType);
        return;
    }

    T_DjiPerceptionSaveJob job;
    job.dirName = dirName;
    job.timestamp = imageInfo.timeStamp;
    job.width = width;
    job.height = height;
    job.format = format;
    job.data.assign(imageRawBuffer, imageRawBuffer + bufferLen);

    DjiTest_StartSaveWorkerIfNeeded();

    {
        std::lock_guard<std::mutex> lock(s_saveJobMutex);
        if (s_saveJobQueue.size() >= PERCEPTION_SAVE_JOB_QUEUE_MAX) {
            s_saveJobQueue.pop_front();
            USER_LOG_WARN("[Perception] Save queue full, drop oldest frame.");
        }
        s_saveJobQueue.push_back(std::move(job));
    }
    s_saveJobCond.notify_one();
}

static void DjiTest_StartSaveWorkerIfNeeded(void)
{
    std::lock_guard<std::mutex> lock(s_saveJobMutex);
    if (s_saveWorkerStarted) {
        return;
    }

    s_saveWorkerStop = false;
    s_saveWorkerThread = std::thread(DjiTest_SaveWorkerTask);
    s_saveWorkerStarted = true;
    std::atexit(DjiTest_StopSaveWorkerAtExit);
}

static void DjiTest_StopSaveWorkerAtExit(void)
{
    DjiTest_StopSaveWorker();
}

static void DjiTest_StopSaveWorker(void)
{
    {
        std::lock_guard<std::mutex> lock(s_saveJobMutex);
        if (!s_saveWorkerStarted) {
            return;
        }
        s_saveWorkerStop = true;
    }

    s_saveJobCond.notify_all();

    if (s_saveWorkerThread.joinable()) {
        s_saveWorkerThread.join();
    }

    {
        std::lock_guard<std::mutex> lock(s_saveJobMutex);
        s_saveJobQueue.clear();
        s_saveWorkerStarted = false;
    }
}

static void DjiTest_SaveWorkerTask(void)
{
    while (true) {
        T_DjiPerceptionSaveJob job;

        {
            std::unique_lock<std::mutex> lock(s_saveJobMutex);
            s_saveJobCond.wait(lock, []() {
                return s_saveWorkerStop || !s_saveJobQueue.empty();
            });

            if (s_saveWorkerStop && s_saveJobQueue.empty()) {
                break;
            }

            job = std::move(s_saveJobQueue.front());
            s_saveJobQueue.pop_front();
        }

        DjiTest_SaveOneJob(job);
    }
}

static void DjiTest_SaveOneJob(const T_DjiPerceptionSaveJob &job)
{
    if (!DjiTest_EnsurePerceptionSaveDir()) {
        USER_LOG_WARN("[Perception] Create save dir failed: %s", PERCEPTION_SAVE_DIR);
        return;
    }

    char rawPath[192] = {0};
    snprintf(rawPath, sizeof(rawPath), PERCEPTION_SAVE_DIR "/perception_%s_%llu_%ux%u_%s.raw",
             job.dirName.c_str(),
             (unsigned long long) job.timestamp,
             job.width,
             job.height,
             DjiTest_GetFormatName(job.format));

    FILE *file = fopen(rawPath, "wb");
    if (file == nullptr) {
        USER_LOG_WARN("[Perception] Open raw file failed: %s", rawPath);
        return;
    }

    size_t writeSize = fwrite(job.data.data(), 1, job.data.size(), file);
    fclose(file);

    if (writeSize != job.data.size()) {
        USER_LOG_WARN("[Perception] Save raw image failed: %s, write=%zu expect=%zu",
                      rawPath, writeSize, job.data.size());
        return;
    }

    USER_LOG_INFO("[Perception] Save sampled raw success: %s (len=%zu, interval=%dms)",
                  rawPath, job.data.size(), PERCEPTION_JPEG_SAMPLE_INTERVAL_MS);
}

static bool DjiTest_EnsurePerceptionSaveDir(void)
{
    static bool s_dirChecked = false;
    if (s_dirChecked) {
        return true;
    }

    if (mkdir(PERCEPTION_SAVE_DIR, 0755) == 0 || errno == EEXIST) {
        s_dirChecked = true;
        return true;
    }

    return false;
}

static const char *DjiTest_GetFormatName(E_DjiTestImageFormat format)
{
    switch (format) {
        case DJI_TEST_IMAGE_FORMAT_GRAY:
            return "mono8";
        case DJI_TEST_IMAGE_FORMAT_NV12:
            return "nv12";
        case DJI_TEST_IMAGE_FORMAT_RGB:
            return "rgb888";
        default:
            return "unknown";
    }
}

static void *DjiTest_StereoImagesDisplayTask(void *arg)
{
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    auto *pack = (T_DjiTestStereoImagePacket *) arg;
    char nameStr[32] = {0};
    char fpsStr[20] = "FPS: ";
    int fps = 0;
    double timePrev[USER_PERCEPTION_DIRECTION_NUM] = {0};
    double timeNow[USER_PERCEPTION_DIRECTION_NUM] = {0};
    double timeFess[USER_PERCEPTION_DIRECTION_NUM] = {0};
    int count[USER_PERCEPTION_DIRECTION_NUM] = {1};
    char showFpsString[USER_PERCEPTION_DIRECTION_NUM][FPS_STRING_LEN] = {0};
    int i = 0;

    while (true) {
        osalHandler->TaskSleepMs(1);
#ifdef OPEN_CV_INSTALLED
        /*! Get data here */
        osalHandler->MutexLock(pack->mutex);
        if (!pack->gotData) {
            osalHandler->MutexUnlock(pack->mutex);
            continue;
        }
        cv::Mat cv_img_stereo = cv::Mat(pack->info.rawInfo.height, pack->info.rawInfo.width, CV_8U);
        uint32_t expectedSize = pack->info.rawInfo.height * pack->info.rawInfo.width;
        uint32_t copySize = std::min(expectedSize, pack->bufferLen);
        if (pack->imageRawBuffer) {
            memcpy(cv_img_stereo.data, pack->imageRawBuffer, copySize);
            osalHandler->Free(pack->imageRawBuffer);
            pack->imageRawBuffer = NULL;
        }

        for (i = 0; i < sizeof(positionName) / sizeof(T_DjiTestPerceptionCameraPositionName); ++i) {
            if (positionName[i].cameraPosition == pack->info.dataType) {
                sprintf(nameStr, "Image position: %s", positionName[i].name);
                break;
            }
        }

        pack->gotData = false;
        osalHandler->MutexUnlock(pack->mutex);

        if (i < USER_PERCEPTION_DIRECTION_NUM) {
            /*! Calculate frame rate */
            timeNow[i] = (double) cv::getTickCount();
            if (timePrev[i] != 0) {
                timeFess[i] = (timeNow[i] - timePrev[i]) / cv::getTickFrequency() + timeFess[i];
                count[i]++;
            }
            if (timeFess[i] > 1) {
                memset(&showFpsString[i][0], 0, FPS_STRING_LEN);
                fps = count[i] / timeFess[i];
                timeFess[i] = 0;
                count[i] = 0;
                sprintf(&showFpsString[i][0], "%s%d", fpsStr, fps);
            }
            timePrev[i] = timeNow[i];
            cv::putText(cv_img_stereo, &showFpsString[i][0], cv::Point(5, 20),
                        cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0));
        }
        cv::imshow(nameStr, cv_img_stereo);
        cv::waitKey(1);
#else
        /* OpenCV 未安装，仅做数据释放并提示 */
        static uint32_t s_lastWarnTimeMs = 0;
        uint32_t nowWarnTimeMs = 0;

        osalHandler->GetTimeMs(&nowWarnTimeMs);
        if (nowWarnTimeMs - s_lastWarnTimeMs >= 1000) {
            USER_LOG_WARN("Please install opencv to run this stereo image display sample.");
            s_lastWarnTimeMs = nowWarnTimeMs;
        }

        osalHandler->MutexLock(pack->mutex);
        if (!pack->gotData) {
            osalHandler->MutexUnlock(pack->mutex);
            osalHandler->TaskSleepMs(1000);
            continue;
        }

        if (pack->imageRawBuffer != nullptr) {
            osalHandler->Free(pack->imageRawBuffer);
            pack->imageRawBuffer = nullptr;
        }
        pack->gotData = false;
        osalHandler->MutexUnlock(pack->mutex);
#endif
    }
}

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
