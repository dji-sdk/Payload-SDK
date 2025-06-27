/**
********************************************************************
* @file    test_lidar_entry.cpp
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
#include "test_lidar_entry.hpp"
#include <dirent.h>
#include "dji_logger.h"
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <mutex>
#include <fcntl.h>
#include <unistd.h>
#include <mutex>
#include <thread>
#include <iomanip>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <queue>
/* Private constants ---------------------------------------------------------*/
#define PCD_FILE_DEFAULT_LENGTH                 (512)
#define FRAME_BUFFER_LENGTH                     (1024 * 1024)
#define SUBSCRIBE_DATA_TIME_MS                  (1000 * 10)
#define USER_PERCEPTION_LIRDAR_TASK_STACK_SIZE  (2042)
#define PCD_FILE_PATH                           "./DJI_cloud_data"

/* Private types -------------------------------------------------------------*/

/* Private values -------------------------------------------------------------*/
static int lastFrameCnt = 0;
static std::queue<T_DjiLidarFrame *> lidarFrameQueue;
static T_DjiMutexHandle queueMutex;
static T_DjiSemaHandle dataSemaphore;
static bool stopProcessing = false;
static T_DjiSemaHandle taskExitSema;

/* Private functions declaration ---------------------------------------------*/
static void DjiTest_PerceptionLidarCallback(uint8_t *recvBuffer, uint32_t bufferLen);
static std::string DjiTest_getCurrentTimestamp();
static void DjiTest_WriteLidarFrameToBinaryPcdFile(const T_DjiLidarFrame *frame);
static void* DjiTest_ProcessLidarDataTask(void* arg);

/* Exported functions definition ---------------------------------------------*/
void DjiUser_RunLidarDataSubscriptionSample(void) {
    int subscriptionDuration = 10;
    lastFrameCnt = 0;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    osalHandler->MutexCreate(&queueMutex);
    osalHandler->SemaphoreCreate(0, &dataSemaphore);
    osalHandler->SemaphoreCreate(0, &taskExitSema);

    std::cout << "Please ensure that there is enough storage space for the PCD files." << std::endl;

    T_DjiTaskHandle processingThread;
    osalHandler->TaskCreate("LidarProcessingThread", DjiTest_ProcessLidarDataTask, USER_PERCEPTION_LIRDAR_TASK_STACK_SIZE, nullptr, &processingThread);

start:
    T_DjiReturnCode returnCode;
    returnCode = DjiPerception_Init();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        std::cout << "DjiPerception Init failed" << std::endl;
        return;
    }

    std::cout << "start subscribe Lidar data from aircraft" << std::endl;

    returnCode = DjiPerception_SubscribeLidarData(DjiTest_PerceptionLidarCallback);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        std::cout << "Request to subscribe Lidar data failed" << std::endl;
        goto subscribeFailed;
    }

    osalHandler->TaskSleepMs(subscriptionDuration * 1000);

    std::cout << "unsubscribe Lidar data " << std::endl;

    subscribeFailed:
    returnCode = DjiPerception_UnsubscribeLidarData();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        std::cout << "Request to unsubscribe Lidar data failed" << std::endl;
    }

    returnCode = DjiPerception_Deinit();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        std::cout << "DjiPerception DeInit failed" << std::endl;
        return;
    }

    std::cout << "unsubscribe Lidar data success" << std::endl;

    osalHandler->MutexLock(queueMutex);
    stopProcessing = true;
    osalHandler->MutexUnlock(queueMutex);

    osalHandler->SemaphorePost(dataSemaphore);
    osalHandler->SemaphoreWait(taskExitSema);
    osalHandler->TaskDestroy(processingThread);
    osalHandler->MutexDestroy(queueMutex);
    osalHandler->SemaphoreDestroy(dataSemaphore);
    osalHandler->SemaphoreDestroy(taskExitSema);
}

/* Private functions definition-----------------------------------------------*/
static void DjiTest_PerceptionLidarCallback(uint8_t *LidarFrame, uint32_t bufferLen) {
    if (bufferLen != sizeof(T_DjiLidarFrame)) {
        std::cout << "usb recv Lidar length wrong, length = " << bufferLen << std::endl;
        return;
    }

    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    T_DjiLidarFrame * curFrame =  (T_DjiLidarFrame *)osalHandler->Malloc(bufferLen);
    memcpy(curFrame, LidarFrame, bufferLen);

    osalHandler->MutexLock(queueMutex);
    lidarFrameQueue.push(curFrame);
    osalHandler->MutexUnlock(queueMutex);

    osalHandler->SemaphorePost(dataSemaphore);
}
std::string DjiTest_getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();

    std::time_t nowTimeT = std::chrono::system_clock::to_time_t(now);

    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::tm nowTm = *std::localtime(&nowTimeT);
    std::ostringstream oss;
    oss << std::put_time(&nowTm, "%Y%m%d_%H%M%S");
    oss << std::setfill('0') << std::setw(3) << milliseconds.count();

    return oss.str();
}

static void DjiTest_WriteLidarFrameToBinaryPcdFile(const T_DjiLidarFrame *frame) {
    uint32_t totalPoints = 0;
    size_t headerLen = 0;
    size_t pointDataSize = 0;
    size_t bufferSize = 0;
    char *buffer = NULL;
    size_t bufferPos = 0;
    int fd = 0;
    std::string directory = PCD_FILE_PATH;
    std::string filename = directory + "/DJI_cloud_data_" + DjiTest_getCurrentTimestamp() + ".pcd";
    char header[PCD_FILE_DEFAULT_LENGTH];

    if (mkdir(directory.c_str(), 0755) != 0 && errno != EEXIST) {
        fprintf(stderr, "Error creating directory: %s\n", strerror(errno));
        return;
    }

    for (uint16_t i = 0; i < frame->pkgNum; ++i) {
        totalPoints += frame->pkgs[i].header.dotNum;
    }

    snprintf(header, sizeof(header),
             "# .PCD v0.7 - Point Cloud Data file format\n"
             "VERSION 0.7\n"
             "FIELDS x y z intensity label\n"
             "SIZE 4 4 4 1 1\n"
             "TYPE F F F U U\n"
             "COUNT 1 1 1 1 1\n"
             "WIDTH %u\n"
             "HEIGHT 1\n"
             "VIEWPOINT 0 0 0 1 0 0 0\n"
             "POINTS %u\n"
             "DATA binary\n",
             totalPoints, totalPoints);

    // Calculate the size of the buffer needed
    headerLen = strlen(header);
    pointDataSize = totalPoints * (sizeof(float) * 3 + sizeof(uint8_t) * 2);
    bufferSize = headerLen + pointDataSize;

    buffer = (char *)malloc(bufferSize);
    if (buffer == NULL) {
        fprintf(stderr, "Error allocating memory for buffer\n");
        return;
    }

    memcpy(buffer + bufferPos, header, headerLen);
    bufferPos += headerLen;

    for (uint16_t i = 0; i < frame->pkgNum; ++i) {
        const T_DjiPerceptionLidarDecodePkg *pkg = &frame->pkgs[i];
        for (uint16_t j = 0; j < pkg->header.dotNum; ++j) {
            const T_DJIPerceptionLidarPoint *point = &pkg->points[j];
            memcpy(buffer + bufferPos, &point->x, sizeof(float));
            bufferPos += sizeof(float);
            memcpy(buffer + bufferPos, &point->y, sizeof(float));
            bufferPos += sizeof(float);
            memcpy(buffer + bufferPos, &point->z, sizeof(float));
            bufferPos += sizeof(float);
            memcpy(buffer + bufferPos, &point->intensity, sizeof(uint8_t));
            bufferPos += sizeof(uint8_t);
            memcpy(buffer + bufferPos, &point->label, sizeof(uint8_t));
            bufferPos += sizeof(uint8_t);
        }
    }

    fd = open(filename.c_str(), O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd == -1) {
        fprintf(stderr, "Error opening file for writing\n");
        free(buffer);
        return;
    }

    if (write(fd, buffer, bufferSize) == -1) {
        fprintf(stderr, "Error writing buffer to file\n");
    }

    close(fd);
    free(buffer);
}

static void* DjiTest_ProcessLidarDataTask(void* arg) {
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    while(true) {
        osalHandler->SemaphoreWait(dataSemaphore);

        osalHandler->MutexLock(queueMutex);
        bool shouldStop = stopProcessing && lidarFrameQueue.empty();
        if(shouldStop) {
            osalHandler->MutexUnlock(queueMutex);
            break;
        }

        T_DjiLidarFrame *lidarFrame = lidarFrameQueue.front();
        lidarFrameQueue.pop();
        osalHandler->MutexUnlock(queueMutex);

        DjiTest_WriteLidarFrameToBinaryPcdFile(lidarFrame);

        int curFrameCnt = lidarFrame->frameCnt;
        std::cout << "Lidar data : curFrameCnt=" << curFrameCnt << std::endl;
        if(lastFrameCnt != 0 && (curFrameCnt - lastFrameCnt) > 1) {
            std::cout << "The number of lost packets during transmission is: " << curFrameCnt - lastFrameCnt - 1 << std::endl;
        }
        lastFrameCnt = curFrameCnt;

        osalHandler->Free(lidarFrame);
    }

    osalHandler->SemaphorePost(taskExitSema);
    return nullptr;
}
/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
