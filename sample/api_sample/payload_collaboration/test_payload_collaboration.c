/**
 ********************************************************************
 * @file    test_payload_collaboration.c
 * @version V2.0.0
 * @date    2019/9/3
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
#include "test_payload_collaboration.h"
#include "psdk_payload_collaboration.h"
#include "psdk_logger.h"
#include "psdk_platform.h"
#include "utils/util_misc.h"

/* Private constants ---------------------------------------------------------*/
#define PSDK_TEST_PAYLOAD_COLLABORATION_TASK_FREQ                   (1)
#define PSDK_TEST_PAYLOAD_COLLABORATION_TASK_STACK_SIZE             (2048)
#define PSDK_TEST_PAYLOAD_COLLABORATION_PAYLOADS_IN_DRONE_MAX_COUNT (3)

/* Private types -------------------------------------------------------------*/
typedef struct {
    E_PsdkPayloadCollaborationCameraType cameraType;
    bool hasOpticalZoomSpec;
    bool hasHybridZoomFocalLength;
} T_PsdkTestPayloadPara;

/* Private functions declaration ---------------------------------------------*/
static void *PsdkTest_PayloadCollaborationTask(void *arg);

/* Private variables ---------------------------------------------------------*/
static T_PsdkTaskHandle s_payloadCollaborationThread;

static const T_PsdkTestPayloadPara s_payloadPara[] = {
    {PSDK_PAYLOAD_COLLABORATION_CAMERA_TYPE_XT,   false, false},
    {PSDK_PAYLOAD_COLLABORATION_CAMERA_TYPE_X4S,  true,  true},
    {PSDK_PAYLOAD_COLLABORATION_CAMERA_TYPE_X5S,  true,  true},
    {PSDK_PAYLOAD_COLLABORATION_CAMERA_TYPE_X7,   true,  true},
    {PSDK_PAYLOAD_COLLABORATION_CAMERA_TYPE_Z30,  true,  true},
    {PSDK_PAYLOAD_COLLABORATION_CAMERA_TYPE_XT2,  true,  true},
    {PSDK_PAYLOAD_COLLABORATION_CAMERA_TYPE_XTS,  false, false},
    {PSDK_PAYLOAD_COLLABORATION_CAMERA_TYPE_H20,  true,  true},
    {PSDK_PAYLOAD_COLLABORATION_CAMERA_TYPE_H20T, true,  true},
};

/* Exported functions definition ---------------------------------------------*/
T_PsdkReturnCode PsdkTest_PayloadCollaborationInit(void)
{
    if (PsdkPayloadCollaboration_Init() != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("payload collaboration module init error");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    if (PsdkOsal_TaskCreate(&s_payloadCollaborationThread, PsdkTest_PayloadCollaborationTask,
                            "user_payload_collaboration_task", PSDK_TEST_PAYLOAD_COLLABORATION_TASK_STACK_SIZE, NULL) !=
        PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("user payload collaboration task create error.");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/* Private functions definition-----------------------------------------------*/
#ifndef __CC_ARM
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wreturn-type"
#endif

static void *PsdkTest_PayloadCollaborationTask(void *arg)
{
    int i = 0;
    unsigned int payloadParaIndex = 0;
    T_PsdkReturnCode psdkStat;
    E_PsdkPayloadCollaborationCameraType cameraType = PSDK_PAYLOAD_COLLABORATION_CAMERA_TYPE_UNKNOWN;
    T_PsdkCameraOpticalZoomSpec cameraOpticalZoomSpec = {0};
    uint16_t cameraHybridZoomFocalLength = 0;
    T_PsdkAircraftInfoBaseInfo aircraftBaseInfo = {0};
    E_PsdkAircraftInfoPayloadMountPosition requestedPayloadMountPosition;

    USER_UTIL_UNUSED(arg);

    while (1) {
        PsdkOsal_TaskSleepMs(1000 / PSDK_TEST_PAYLOAD_COLLABORATION_TASK_FREQ);

        psdkStat = PsdkAircraftInfo_GetBaseInfo(&aircraftBaseInfo);
        if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            PsdkLogger_UserLogError("get aircraft information error: 0x%08llX.", psdkStat);
            continue;
        }

        for (i = 0; i < PSDK_TEST_PAYLOAD_COLLABORATION_PAYLOADS_IN_DRONE_MAX_COUNT; ++i) {
            requestedPayloadMountPosition =
                (E_PsdkAircraftInfoPayloadMountPosition) (i + PSDK_AIRCRAFT_INFO_PAYLOAD_MOUNT_POSITION_NO1);
            if (requestedPayloadMountPosition == aircraftBaseInfo.payloadMountPosition)
                continue;

            if (aircraftBaseInfo.aircraftType == PSDK_AIRCRAFT_INFO_TYPE_M200_V2 ||
                aircraftBaseInfo.aircraftType == PSDK_AIRCRAFT_INFO_TYPE_M210_V2 ||
                aircraftBaseInfo.aircraftType == PSDK_AIRCRAFT_INFO_TYPE_M210RTK_V2) {
                if (requestedPayloadMountPosition == PSDK_AIRCRAFT_INFO_PAYLOAD_MOUNT_POSITION_NO3) {
                    continue;
                }
            }

            psdkStat = PsdkPayloadCollaboration_GetCameraTypeOfPayload(requestedPayloadMountPosition, &cameraType);
            if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                continue;
            }
            PsdkLogger_UserLogDebug("camera type of payload mounted on NO.%d gimbal connector is %d.",
                                    requestedPayloadMountPosition, cameraType);

            for (payloadParaIndex = 0; payloadParaIndex < UTIL_ARRAY_SIZE(s_payloadPara); ++payloadParaIndex) {
                if (s_payloadPara[payloadParaIndex].cameraType == cameraType)
                    break;
            }
            if (payloadParaIndex == UTIL_ARRAY_SIZE(s_payloadPara)) {
                PsdkLogger_UserLogError("Not find payload parameters.");
                continue;
            }

            if (s_payloadPara[payloadParaIndex].hasOpticalZoomSpec) {
                psdkStat = PsdkPayloadCollaboration_GetCameraOpticalZoomSpecOfPayload(requestedPayloadMountPosition,
                                                                                      &cameraOpticalZoomSpec);
                if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    PsdkLogger_UserLogError("Get optical zoom specification error.");
                } else {
                    PsdkLogger_UserLogDebug(
                        "camera optical zoom specification of payload mounted on NO.%d gimbal connector, maxFocalLength: %d, minFocalLength: %d, focalLengthStep: %d.",
                        requestedPayloadMountPosition, cameraOpticalZoomSpec.maxFocalLength,
                        cameraOpticalZoomSpec.minFocalLength, cameraOpticalZoomSpec.focalLengthStep);
                }
            }

            if (s_payloadPara[payloadParaIndex].hasHybridZoomFocalLength) {
                psdkStat = PsdkPayloadCollaboration_GetCameraHybridZoomFocalLengthOfPayload(
                    requestedPayloadMountPosition,
                    &cameraHybridZoomFocalLength);
                if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    PsdkLogger_UserLogError("Get hybrid zoom focal length error.");
                } else {
                    PsdkLogger_UserLogDebug(
                        "camera hybrid zoom focal length of payload mounted on NO.%d gimbal connector, focalLength: %d.",
                        requestedPayloadMountPosition, cameraHybridZoomFocalLength);
                }
            }
        }
    }
}

#ifndef __CC_ARM
#pragma GCC diagnostic pop
#endif

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
