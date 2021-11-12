/**
 ********************************************************************
 * @file    test_payload_collaboration.c
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
#include <dji_payload_camera.h>
#include "test_payload_collaboration.h"
#include "dji_aircraft_info.h"
#include "dji_typedef.h"
#include "dji_logger.h"
#include "dji_platform.h"
#include "utils/util_misc.h"

/* Private constants ---------------------------------------------------------*/
#define DJI_TEST_PAYLOAD_COLLABORATION_TASK_FREQ                   (1)
#define DJI_TEST_PAYLOAD_COLLABORATION_TASK_STACK_SIZE             (2048)
#define DJI_TEST_PAYLOAD_COLLABORATION_PAYLOADS_IN_DRONE_MAX_COUNT (3)

/* Private types -------------------------------------------------------------*/
typedef struct {
    E_DjiCameraType cameraType;
    bool hasOpticalZoomSpec;
    bool hasHybridZoomFocalLength;
} T_DjiTestPayloadPara;

/* Private functions declaration ---------------------------------------------*/
static void *DjiTest_PayloadCollaborationTask(void *arg);

/* Private variables ---------------------------------------------------------*/
static T_DjiTaskHandle s_payloadCollaborationThread;

static const T_DjiTestPayloadPara s_payloadPara[] = {
    {DJI_CAMERA_TYPE_Z30,  true,  true},
    {DJI_CAMERA_TYPE_XT2,  true,  true},
    {DJI_CAMERA_TYPE_XTS,  false, false},
    {DJI_CAMERA_TYPE_H20,  true,  true},
    {DJI_CAMERA_TYPE_H20T, true,  true},
};
static bool s_userPayloadCollaborationDataShow = false;

/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode DjiTest_PayloadCollaborationStartService(void)
{
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    if (osalHandler->TaskCreate("user_payload_collaboration_task", DjiTest_PayloadCollaborationTask,
                                DJI_TEST_PAYLOAD_COLLABORATION_TASK_STACK_SIZE,
                                NULL, &s_payloadCollaborationThread) !=
        DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("user payload collaboration task create error.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiTest_PayloadCollaborationDataShowTrigger(void)
{
    s_userPayloadCollaborationDataShow = !s_userPayloadCollaborationDataShow;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/* Private functions definition-----------------------------------------------*/
#ifndef __CC_ARM
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wreturn-type"
#endif

static void *DjiTest_PayloadCollaborationTask(void *arg)
{
    int i = 0;
    unsigned int payloadParaIndex = 0;
    T_DjiReturnCode djiStat;
    E_DjiCameraType cameraType = DJI_CAMERA_TYPE_UNKNOWN;
    uint16_t cameraHybridZoomFocalLength = 0;
    T_DjiAircraftInfoBaseInfo aircraftBaseInfo = {0};
    E_DjiMountPosition requestedPayloadMountPosition;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    T_DjiCameraOpticalZoomSpec cameraOpticalZoomSpec = {0};

    USER_UTIL_UNUSED(arg);

    while (1) {
        osalHandler->TaskSleepMs(1000 / DJI_TEST_PAYLOAD_COLLABORATION_TASK_FREQ);

        djiStat = DjiAircraftInfo_GetBaseInfo(&aircraftBaseInfo);
        if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("get aircraft information error: 0x%08llX.", djiStat);
            continue;
        }

        for (i = 0; i < DJI_TEST_PAYLOAD_COLLABORATION_PAYLOADS_IN_DRONE_MAX_COUNT; ++i) {
            requestedPayloadMountPosition =
                (E_DjiMountPosition) (i + DJI_MOUNT_POSITION_PAYLOAD_PORT_NO1);
            if (requestedPayloadMountPosition == aircraftBaseInfo.mountPosition)
                continue;

            djiStat = DjiPayloadCamera_GetCameraTypeOfPayload(requestedPayloadMountPosition, &cameraType);
            if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                continue;
            }
            if (s_userPayloadCollaborationDataShow == true) {
                USER_LOG_INFO("camera type of payload mounted on NO.%d gimbal connector is %d.",
                              requestedPayloadMountPosition, cameraType);
            }

            for (payloadParaIndex = 0; payloadParaIndex < UTIL_ARRAY_SIZE(s_payloadPara); ++payloadParaIndex) {
                if (s_payloadPara[payloadParaIndex].cameraType == cameraType)
                    break;
            }
            if (payloadParaIndex == UTIL_ARRAY_SIZE(s_payloadPara)) {
                USER_LOG_ERROR("Not find payload parameters.");
                continue;
            }

            if (s_payloadPara[payloadParaIndex].hasOpticalZoomSpec) {
                djiStat = DjiPayloadCamera_GetCameraOpticalZoomSpecOfPayload(requestedPayloadMountPosition,
                                                                             &cameraOpticalZoomSpec);
                if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    USER_LOG_ERROR("Get optical zoom specification error.");
                }

                if (s_userPayloadCollaborationDataShow == true) {
                    USER_LOG_INFO(
                        "camera optical zoom specification of payload mounted on NO.%d gimbal connector, maxFocalLength: %d, minFocalLength: %d, focalLengthStep: %d.",
                        requestedPayloadMountPosition, cameraOpticalZoomSpec.maxFocalLength,
                        cameraOpticalZoomSpec.minFocalLength, cameraOpticalZoomSpec.focalLengthStep);
                }
            }

            if (s_payloadPara[payloadParaIndex].hasHybridZoomFocalLength) {
                djiStat = DjiPayloadCamera_GetCameraHybridZoomFocalLengthOfPayload(requestedPayloadMountPosition,
                                                                                   &cameraHybridZoomFocalLength);
                if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    USER_LOG_ERROR("Get hybrid zoom focal length error.");
                }
                if (s_userPayloadCollaborationDataShow == true) {
                    USER_LOG_INFO(
                        "camera hybrid zoom focal length of payload mounted on NO.%d gimbal connector, focalLength: %d.\r\n",
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
