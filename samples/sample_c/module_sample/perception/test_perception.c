/**
 ********************************************************************
 * @file    test_perception.c
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
#include "test_perception.h"
#include "dji_logger.h"
#include "dji_platform.h"

/* Private constants ---------------------------------------------------------*/
#define TEST_PERCEPTION_SAVE_IMAGE_MAX_NUM    10

/* Private types -------------------------------------------------------------*/
typedef struct {
    E_DjiPerceptionDirection direction;
    char *name;
} T_DjiTestPerceptionDirectionName;

/* Private values -------------------------------------------------------------*/
static uint16_t s_perceptionImageCount = 0;
static const T_DjiTestPerceptionDirectionName directionName[] = {
    {.direction = DJI_PERCEPTION_RECTIFY_DOWN, .name = "down"},
    {.direction = DJI_PERCEPTION_RECTIFY_FRONT, .name = "front"},
    {.direction = DJI_PERCEPTION_RECTIFY_REAR, .name = "rear"},
    {.direction = DJI_PERCEPTION_RECTIFY_UP, .name = "up"},
    {.direction = DJI_PERCEPTION_RECTIFY_LEFT, .name = "left"},
    {.direction = DJI_PERCEPTION_RECTIFY_RIGHT, .name = "right"},
};

/* Private functions declaration ---------------------------------------------*/
static void DjiTest_PerceptionImageCallback(T_DjiPerceptionImageInfo imageInfo, uint8_t *imageRawBuffer,
                                            uint32_t bufferLen);
static int32_t DjiTest_SaveImageData(char *filePath, const uint8_t *data, uint32_t len);

/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode DjiTest_PerceptionRunSample(E_DjiPerceptionDirection direction)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    T_DjiPerceptionCameraParametersPacket cameraParametersetersPacket = {0};

    USER_LOG_INFO("Perception sample start");
    DjiTest_WidgetLogAppend("Perception sample start");

    USER_LOG_INFO("--> Step 1: Init Perception module");
    DjiTest_WidgetLogAppend("--> Step 1: Init Perception module");
    returnCode = DjiPerception_Init();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Perception init failed, error code: 0x%08X", returnCode);
        goto out;
    }

    s_perceptionImageCount = 0;

    USER_LOG_INFO("--> Step 2: Get stereo camera parameters\r\n");
    DjiTest_WidgetLogAppend("--> Step 2: Get stereo camera parameters\r\n");
    returnCode = DjiPerception_GetStereoCameraParameters(&cameraParametersetersPacket);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Get stereo camera parameters failed, error code: 0x%08X", returnCode);
        goto out;
    }

    if (cameraParametersetersPacket.directionNum <= IMAGE_MAX_DIRECTION_NUM)
        for (int i = 0; i < cameraParametersetersPacket.directionNum; i++) {
            USER_LOG_INFO(" [%-05s] leftIntrinsics = {%f, %f, %f, %f, %f, %f, %f, %f, %f }",
                          directionName[cameraParametersetersPacket.cameraParameters[i].direction].name,
                          cameraParametersetersPacket.cameraParameters[i].leftIntrinsics[0],
                          cameraParametersetersPacket.cameraParameters[i].leftIntrinsics[1],
                          cameraParametersetersPacket.cameraParameters[i].leftIntrinsics[2],
                          cameraParametersetersPacket.cameraParameters[i].leftIntrinsics[3],
                          cameraParametersetersPacket.cameraParameters[i].leftIntrinsics[4],
                          cameraParametersetersPacket.cameraParameters[i].leftIntrinsics[5],
                          cameraParametersetersPacket.cameraParameters[i].leftIntrinsics[6],
                          cameraParametersetersPacket.cameraParameters[i].leftIntrinsics[7],
                          cameraParametersetersPacket.cameraParameters[i].leftIntrinsics[8]);
            USER_LOG_INFO("[%-05s] rightIntrinsics = {%f, %f, %f, %f, %f, %f, %f, %f, %f }",
                          directionName[cameraParametersetersPacket.cameraParameters[i].direction].name,
                          cameraParametersetersPacket.cameraParameters[i].rightIntrinsics[0],
                          cameraParametersetersPacket.cameraParameters[i].rightIntrinsics[1],
                          cameraParametersetersPacket.cameraParameters[i].rightIntrinsics[2],
                          cameraParametersetersPacket.cameraParameters[i].rightIntrinsics[3],
                          cameraParametersetersPacket.cameraParameters[i].rightIntrinsics[4],
                          cameraParametersetersPacket.cameraParameters[i].rightIntrinsics[5],
                          cameraParametersetersPacket.cameraParameters[i].rightIntrinsics[6],
                          cameraParametersetersPacket.cameraParameters[i].rightIntrinsics[7],
                          cameraParametersetersPacket.cameraParameters[i].rightIntrinsics[8]);
            USER_LOG_INFO("[%-05s] rotationLeftInRight = {%f, %f, %f, %f, %f, %f, %f, %f, %f }",
                          directionName[cameraParametersetersPacket.cameraParameters[i].direction].name,
                          cameraParametersetersPacket.cameraParameters[i].rotationLeftInRight[0],
                          cameraParametersetersPacket.cameraParameters[i].rotationLeftInRight[1],
                          cameraParametersetersPacket.cameraParameters[i].rotationLeftInRight[2],
                          cameraParametersetersPacket.cameraParameters[i].rotationLeftInRight[3],
                          cameraParametersetersPacket.cameraParameters[i].rotationLeftInRight[4],
                          cameraParametersetersPacket.cameraParameters[i].rotationLeftInRight[5],
                          cameraParametersetersPacket.cameraParameters[i].rotationLeftInRight[6],
                          cameraParametersetersPacket.cameraParameters[i].rotationLeftInRight[7],
                          cameraParametersetersPacket.cameraParameters[i].rotationLeftInRight[8]);
            USER_LOG_INFO("[%-05s] translationLeftInRight = {%f, %f, %f }\r\n",
                          directionName[cameraParametersetersPacket.cameraParameters[i].direction].name,
                          cameraParametersetersPacket.cameraParameters[i].translationLeftInRight[0],
                          cameraParametersetersPacket.cameraParameters[i].translationLeftInRight[1],
                          cameraParametersetersPacket.cameraParameters[i].translationLeftInRight[2]);
            osalHandler->TaskSleepMs(100);
        }

    USER_LOG_INFO("--> Step 3: Subscribe perception image\r\n");
    DjiTest_WidgetLogAppend("--> Step 3: Subscribe perception image\r\n");
    returnCode = DjiPerception_SubscribePerceptionImage(direction, DjiTest_PerceptionImageCallback);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Subscribe perception image failed, error code: 0x%08X", returnCode);
        goto out;
    }

    osalHandler->TaskSleepMs(5000);

    USER_LOG_INFO("--> Step 4: Unsubscribe perception image");
    DjiTest_WidgetLogAppend("--> Step 4: Unsubscribe perception image");
    returnCode = DjiPerception_UnsubscribePerceptionImage(direction);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Unsubscribe perception image failed, error code: 0x%08X", returnCode);
        goto out;
    }

    USER_LOG_INFO("--> Step 5: Deinit Perception module");
    DjiTest_WidgetLogAppend("--> Step 5: Deinit Perception module");
    returnCode = DjiPerception_Deinit();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Perception deinit failed, error code: 0x%08X", returnCode);
        goto out;
    }

out:
    USER_LOG_INFO("Perception sample end");

    return returnCode;
}

/* Private functions definition-----------------------------------------------*/
static int32_t DjiTest_SaveImageData(char *filePath, const uint8_t *data, uint32_t len)
{
    FILE *fp = NULL;
    size_t size;

    fp = fopen(filePath, "w+");
    if (fp == NULL) {
        return -1;
    }
    size = fwrite(data, 1, len, fp);
    if (size != len) {
        if (fp) {
            fclose(fp);
        }
        return -1;
    }

    if (fp) {
        fclose(fp);
    }

    return 0;
}

static void DjiTest_PerceptionImageCallback(T_DjiPerceptionImageInfo imageInfo, uint8_t *imageRawBuffer,
                                            uint32_t bufferLen)
{
    char fileName[256] = {0};

    snprintf(fileName, sizeof(fileName), "./image_%s_%d.raw",
             directionName[imageInfo.rawInfo.direction].name,
             s_perceptionImageCount);

    if (s_perceptionImageCount < TEST_PERCEPTION_SAVE_IMAGE_MAX_NUM) {
        DjiTest_SaveImageData(fileName, imageRawBuffer, bufferLen);

        USER_LOG_INFO(
            "Save perception image to path: ${binary_execute_path}/image_%s_%d.raw, direction:%s, position:%d, size:%dx%d",
            directionName[imageInfo.rawInfo.direction].name,
            s_perceptionImageCount,
            directionName[imageInfo.rawInfo.direction].name,
            imageInfo.dataType,
            imageInfo.rawInfo.width,
            imageInfo.rawInfo.height);

        s_perceptionImageCount++;
    }
}

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
