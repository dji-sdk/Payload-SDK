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

#ifdef OPEN_CV_INSTALLED

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"

#endif

/* Private constants ---------------------------------------------------------*/
#define USER_PERCEPTION_TASK_STACK_SIZE    (1024)
#define USER_PERCEPTION_DIRECTION_NUM      (12)
#define FPS_STRING_LEN                     (50)

/* Private types -------------------------------------------------------------*/
typedef struct {
    T_DjiPerceptionImageInfo info;
    uint8_t *imageRawBuffer;
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

/* Private values -------------------------------------------------------------*/
static T_DjiTaskHandle s_stereoImageThread;
static T_DjiTestStereoImagePacket s_stereoImagePacket = {
    .info           = {0},
    .imageRawBuffer = nullptr,
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

/* Private functions declaration ---------------------------------------------*/
static void DjiTest_PerceptionImageCallback(T_DjiPerceptionImageInfo imageInfo, uint8_t *imageRawBuffer,
                                            uint32_t bufferLen);
static void *DjiTest_StereoImagesDisplayTask(void *arg);

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
                perceptionSample->SubscribeDownImage(DjiTest_PerceptionImageCallback);
                break;
            case 'f':
                USER_LOG_INFO("Subscribe front stereo camera pair images.");
                perceptionSample->SubscribeFrontImage(DjiTest_PerceptionImageCallback);
                break;
            case 'r':
                USER_LOG_INFO("Subscribe rear stereo camera pair images.");
                perceptionSample->SubscribeRearImage(DjiTest_PerceptionImageCallback);
                break;
            case 'u':
                USER_LOG_INFO("Subscribe up stereo camera pair images.");
                perceptionSample->SubscribeUpImage(DjiTest_PerceptionImageCallback);
                break;
            case 'l':
                USER_LOG_INFO("Subscribe left stereo camera pair images.");
                perceptionSample->SubscribeLeftImage(DjiTest_PerceptionImageCallback);
                break;
            case 't':
                USER_LOG_INFO("Subscribe right stereo camera pair images.");
                perceptionSample->SubscribeRightImage(DjiTest_PerceptionImageCallback);
                break;
            case 'g':
                USER_LOG_INFO("Do stereo camera parameters subscription");
                break;
            case 'q':
                goto DestroyTask;
            default:
                break;
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
    returnCode = osalHandler->TaskDestroy(s_stereoImageThread);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Destroy task failed, return code:0x%08X", returnCode);
    }

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

    if (imageRawBuffer) {
        osalHandler->MutexLock(s_stereoImagePacket.mutex);
        s_stereoImagePacket.info = imageInfo;
        if (s_stereoImagePacket.imageRawBuffer) osalHandler->Free(s_stereoImagePacket.imageRawBuffer);
        s_stereoImagePacket.imageRawBuffer = (uint8_t *) osalHandler->Malloc(bufferLen);
        memcpy(s_stereoImagePacket.imageRawBuffer, imageRawBuffer, bufferLen);
        s_stereoImagePacket.gotData = true;
        osalHandler->MutexUnlock(s_stereoImagePacket.mutex);
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
        int copySize = pack->info.rawInfo.height * pack->info.rawInfo.width;
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
        osalHandler->TaskSleepMs(1000);
        USER_LOG_WARN("Please install opencv to run this stereo image display sample.");
#endif
    }
}

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
