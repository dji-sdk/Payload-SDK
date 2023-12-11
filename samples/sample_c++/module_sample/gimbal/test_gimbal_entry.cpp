/**
 ********************************************************************
 * @file    test_gimbal_entry.cpp
 * @version V2.0.0
 * @date    2023/3/28
 * @brief
 *
 * @copyright (c) 2018-2023 DJI. All rights reserved.
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
#include <stdexcept>
#include "test_gimbal_entry.hpp"
#include "dji_logger.h"
#include "utils/util_misc.h"
#include "dji_gimbal.h"
#include "dji_gimbal_manager.h"
#include <iostream>
#include "dji_aircraft_info.h"
#include "dji_fc_subscription.h"
#include <math.h>

/* Private constants ---------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/
typedef enum {
    E_DJI_TEST_GIMBAL_MANAGER_SAMPLE_SELECT_ROTATE_GIMBAL_ON_FREE_MODE,
    E_DJI_TEST_GIMBAL_MANAGER_SAMPLE_SELECT_ROTATE_GIMBAL_ON_YAW_FOLLOW_MODE,
    E_DJI_TEST_GIMBAL_MANAGER_SAMPLE_SELECT_SET_PITCH_RANGE_EXTENSION_MODE,
    E_DJI_TEST_GIMBAL_MANAGER_SAMPLE_SELECT_SET_CONTROLLER_MAX_SPEED_PERCENTAGE,
    E_DJI_TEST_GIMBAL_MANAGER_SAMPLE_SELECT_SET_CONTROLLER_SMOOTH_FACTOR,
    E_DJI_TEST_GIMBAL_MANAGER_SAMPLE_SELECT_RESET_GIMBAL_SETTINGS,
    E_DJI_TEST_GIMBAL_MANAGER_SAMPLE_SELECT_ROTATE_GIMBAL_BY_KEYBOARD,
    E_DJI_TEST_GIMBAL_MANAGER_SAMPLE_SELECT_QUIT,
} E_DjiTestGimbalManagerSampleSelect;

/* Private values -------------------------------------------------------------*/
static const char *s_gimbalManagerSampleList[] = {
    [E_DJI_TEST_GIMBAL_MANAGER_SAMPLE_SELECT_ROTATE_GIMBAL_ON_FREE_MODE] =
    "| [0] Gimbal manager sample - Rotate gimbal on free mode                                           |",
    [E_DJI_TEST_GIMBAL_MANAGER_SAMPLE_SELECT_ROTATE_GIMBAL_ON_YAW_FOLLOW_MODE] =
    "| [1] Gimbal manager sample - Rotate gimbal on yaw follow mode                                     |",
    [E_DJI_TEST_GIMBAL_MANAGER_SAMPLE_SELECT_SET_PITCH_RANGE_EXTENSION_MODE] =
    "| [2] Gimbal manager sample - Set pitch range extension mode                                       |",
    [E_DJI_TEST_GIMBAL_MANAGER_SAMPLE_SELECT_SET_CONTROLLER_MAX_SPEED_PERCENTAGE] =
    "| [3] Gimbal manager sample - Set controller max speed percentage                                  |",
    [E_DJI_TEST_GIMBAL_MANAGER_SAMPLE_SELECT_SET_CONTROLLER_SMOOTH_FACTOR] =
    "| [4] Gimbal manager sample - Set controller smooth factor                                         |",
    [E_DJI_TEST_GIMBAL_MANAGER_SAMPLE_SELECT_RESET_GIMBAL_SETTINGS] =
    "| [5] Gimbal manager sample - Reset gimbal settings                                                |",
    [E_DJI_TEST_GIMBAL_MANAGER_SAMPLE_SELECT_ROTATE_GIMBAL_BY_KEYBOARD] =
    "| [6] Gimbal manager sample - Rotate gimbal by keyboard input and read back value                  |",
    [E_DJI_TEST_GIMBAL_MANAGER_SAMPLE_SELECT_QUIT] =
    "| [q] Gimbal manager sample - Quit                                                                 |",
};

/* Private functions declaration ---------------------------------------------*/
void DjiTest_GimbalManagerShowSampleSelectList(const char **SampleList, uint8_t size);

/* Exported functions definition ---------------------------------------------*/
void DjiUser_RunGimbalManagerSample(void)
{
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    char inputTestCase;
    char mountPosition;
    E_DjiMountPosition gimbalMountPosition;
    T_DjiReturnCode returnCode;

start:
    osalHandler->TaskSleepMs(100);
    std::cout
        << "| Available position:                                                                              |"
        <<
        std::endl;
    std::cout
        << "| [1] Select gimbal mount position at NO.1 payload port                                            |"
        <<
        std::endl;
    std::cout
        << "| [2] Select gimbal mount position at NO.2 payload port                                            |"
        <<
        std::endl;
    std::cout
        << "| [3] Select gimbal mount position at NO.3 payload port                                            |"
        <<
        std::endl;
    std::cout
        << "| [q] Quit                                                                                         |"
        <<
        std::endl;

    std::cin >> mountPosition;
    if (mountPosition == 'q') {
        return;
    }

    if (mountPosition > '3' || mountPosition < '1') {
        USER_LOG_ERROR("Input mount position is invalid");
        goto start;
    }
    gimbalMountPosition = E_DjiMountPosition(mountPosition - '0');

    osalHandler->TaskSleepMs(100);
    std::cout
        << "| Available commands:                                                                              |"
        <<
        std::endl;
    DjiTest_GimbalManagerShowSampleSelectList(s_gimbalManagerSampleList, UTIL_ARRAY_SIZE(s_gimbalManagerSampleList));

    std::cin >> inputTestCase;
    switch (inputTestCase) {
        case '0':
            DjiTest_GimbalManagerRunSample(gimbalMountPosition, DJI_GIMBAL_MODE_FREE);
            goto start;
        case '1':
            DjiTest_GimbalManagerRunSample(gimbalMountPosition, DJI_GIMBAL_MODE_YAW_FOLLOW);
            goto start;
        case '2': {
            int32_t enableFlag;

            osalHandler->TaskSleepMs(10);
            printf("Input enable flag: ");
            scanf("%d", &enableFlag);

            returnCode = DjiGimbalManager_Init();
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Init gimbal manager error, return code 0x%08X", returnCode);
                return;
            }

            returnCode = DjiGimbalManager_SetPitchRangeExtensionEnabled(gimbalMountPosition, (bool)enableFlag);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Failed!");
            }

            USER_LOG_INFO("Set gimbal's pitch range extension mode as %d successfully!", enableFlag);

            goto start;
            break;
        }
        case '3': {
            int32_t percentage;

            osalHandler->TaskSleepMs(10);
            printf("Input max speed percentage of yaw axis: ");
            scanf("%d", &percentage);

            returnCode = DjiGimbalManager_Init();
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Init gimbal manager error, return code 0x%08X", returnCode);
                return;
            }

            returnCode = DjiGimbalManager_SetControllerMaxSpeedPercentage(gimbalMountPosition, DJI_GIMBAL_AXIS_YAW, (uint8_t)percentage);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Failed!");
            }

            USER_LOG_INFO("Set yaw axis's max speed percentage to %d successfully!", percentage);

            osalHandler->TaskSleepMs(10);
            printf("Input max speed percentage of pitch axis: ");
            scanf("%d", &percentage);

            returnCode = DjiGimbalManager_SetControllerMaxSpeedPercentage(gimbalMountPosition, DJI_GIMBAL_AXIS_PITCH, (uint8_t)percentage);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Failed!");
            }

            USER_LOG_INFO("Set pitch axis's max speed percentage to %d successfully!", percentage);

            goto start;
            break;
        }
        case '4': {
            int32_t factor;

            osalHandler->TaskSleepMs(10);
            printf("Input yaw axis's smooth factor: ");
            scanf("%d", &factor);

            returnCode = DjiGimbalManager_Init();
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Init gimbal manager error, return code 0x%08X", returnCode);
                return;
            }

            returnCode = DjiGimbalManager_SetControllerSmoothFactor(gimbalMountPosition, DJI_GIMBAL_AXIS_YAW, (uint8_t)factor);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Failed!");
            }

            USER_LOG_INFO("Set yaw axis smooth factor to %d successfully!", factor);

            osalHandler->TaskSleepMs(10);
            printf("Input pitch axis's smooth factor: ");
            scanf("%d", &factor);

            returnCode = DjiGimbalManager_SetControllerSmoothFactor(gimbalMountPosition, DJI_GIMBAL_AXIS_PITCH, (uint8_t)factor);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Failed!");
            }

            USER_LOG_INFO("Set pitch axis smooth factor to %d successfully!", factor);

            goto start;
            break;
        }
        case '5': {

            returnCode = DjiGimbalManager_Init();
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Init gimbal manager error, return code 0x%08X", returnCode);
                return;
            }

            returnCode = DjiGimbalManager_RestoreFactorySettings(gimbalMountPosition);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Failed!");
            }

            USER_LOG_INFO("Reset gimbal factory settings successfully!");

            goto start;
            break;
        }
        case '6': {
            uint32_t gimbalMode;
            uint32_t rotateMode;
            dji_f32_t pitch, roll, yaw;
            T_DjiGimbalManagerRotation rotation;
            T_DjiAircraftInfoBaseInfo baseInfo;
            E_DjiAircraftSeries aircraftSeries;

            returnCode = DjiGimbalManager_Init();
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Init gimbal manager error, return code 0x%08X", returnCode);
                return;
            }

            returnCode = DjiAircraftInfo_GetBaseInfo(&baseInfo);
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("Failed to get aircraft base info, return code 0x%08X", returnCode);
                goto end;
            }

            aircraftSeries = baseInfo.aircraftSeries;

            if (aircraftSeries == DJI_AIRCRAFT_SERIES_M300 || aircraftSeries == DJI_AIRCRAFT_SERIES_M350) {
                returnCode = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_IMU_ATTI_NAVI_DATA_WITH_TIMESTAMP, DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ, NULL);
                if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    USER_LOG_ERROR("DjiFcSubscription_SubscribeTopic %d return %d",
                                   DJI_FC_SUBSCRIPTION_TOPIC_IMU_ATTI_NAVI_DATA_WITH_TIMESTAMP, returnCode);
                    goto end;
                }

                returnCode = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_QUATERNION, DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ, NULL);
                if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    USER_LOG_ERROR("DjiFcSubscription_SubscribeTopic %d return %d", DJI_FC_SUBSCRIPTION_TOPIC_QUATERNION, returnCode);
                    goto end;
                }
            }


            if (aircraftSeries == DJI_AIRCRAFT_SERIES_M300 || aircraftSeries == DJI_AIRCRAFT_SERIES_M350) {
                returnCode = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_THREE_GIMBAL_DATA, DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ, NULL);
                if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    USER_LOG_ERROR("DjiFcSubscription_SubscribeTopic %d return %d", DJI_FC_SUBSCRIPTION_TOPIC_THREE_GIMBAL_DATA, returnCode);
                    goto end;
                }
                USER_LOG_INFO("Subscribe topic DJI_FC_SUBSCRIPTION_TOPIC_THREE_GIMBAL_DATA succefully.");
            }
            else if (aircraftSeries == DJI_AIRCRAFT_SERIES_M30 || aircraftSeries == DJI_AIRCRAFT_SERIES_M3 ||
                     aircraftSeries == DJI_AIRCRAFT_SERIES_M3D) {
                returnCode = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_GIMBAL_ANGLES, DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ, NULL);
                if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    USER_LOG_ERROR("DjiFcSubscription_SubscribeTopic %d return %d", DJI_FC_SUBSCRIPTION_TOPIC_GIMBAL_ANGLES, returnCode);
                    goto end;
                }
                USER_LOG_INFO("Subscribe topic DJI_FC_SUBSCRIPTION_TOPIC_GIMBAL_ANGLES succefully.");
            }


            while (1) {
                T_DjiFcSubscriptionQuaternion quat;
                T_DjiFcSubscriptionThreeGimbalData threeGimbalData = {0};
                T_DjiFcSubscriptionGimbalAngles gimbalAngles = {0};
                T_DjiDataTimestamp timestamp = {0};
                dji_f32_t nPitch, nRoll, nYaw;
                dji_f32_t qPitch, qRoll, qYaw;
                dji_f32_t yawOffset = 0;
                T_DjiFcSubscriptionImuAttiNaviDataWithTimestamp naviData = {0};

                osalHandler->TaskSleepMs(5);
                printf("gimbal mode: 0: free, 1: fpv, 2: yaw-follow, 3: exit sample\n");
                printf("rotate mode: 0: rel, 1: abs\n");
                printf("Input gimbal mode, rotate mode, p, r, y(range in 0 ~ 360 deg if in abs mode): ");

                scanf("%d", &gimbalMode);
                if (gimbalMode == 3) {
                    break;
                }

                scanf("%d %f %f %f", &rotateMode, &pitch, &roll, &yaw);

                printf("gimbale mode %d, rotate mode %d, p %f, r %f, y %f\n",
                       gimbalMode, rotateMode, pitch, roll, yaw);

                returnCode = DjiGimbalManager_SetMode(gimbalMountPosition, (E_DjiGimbalMode)gimbalMode);
                if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    USER_LOG_ERROR("DjiGimbalManager_SetMode return 0x%08X", returnCode);
                    goto end;
                }

                rotation.rotationMode = (E_DjiGimbalRotationMode)rotateMode;
                rotation.pitch = pitch;
                rotation.roll = roll;
                rotation.yaw = yaw;
                rotation.time = 0.5;

                if (aircraftSeries == DJI_AIRCRAFT_SERIES_M300 || aircraftSeries == DJI_AIRCRAFT_SERIES_M350) {
                    osalHandler->TaskSleepMs(20);
                    returnCode = DjiFcSubscription_GetLatestValueOfTopic(DJI_FC_SUBSCRIPTION_TOPIC_IMU_ATTI_NAVI_DATA_WITH_TIMESTAMP,
                                                                        (uint8_t *) &naviData,
                                                                        sizeof(T_DjiFcSubscriptionImuAttiNaviDataWithTimestamp),
                                                                        &timestamp);
                    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                        USER_LOG_ERROR("returnCode = 0x%08X", returnCode);
                    }

                    nYaw = (dji_f64_t) atan2f(2 * naviData.q[1] * naviData.q[2] + 2 * naviData.q[0] * naviData.q[3],
                                            -2 * naviData.q[2] * naviData.q[2] - 2 * naviData.q[3] * naviData.q[3] + 1) * 57.3;

                    returnCode = DjiFcSubscription_GetLatestValueOfTopic(DJI_FC_SUBSCRIPTION_TOPIC_QUATERNION,
                                                                        (uint8_t *) &quat,
                                                                        sizeof(T_DjiFcSubscriptionQuaternion),
                                                                        &timestamp);
                    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                        USER_LOG_ERROR("DjiFcSubscription_GetLatestValueOfTopic return %x%08X", returnCode);
                        goto end;
                    }

                    qYaw = (dji_f64_t) atan2f(2 * quat.q1 * quat.q2 + 2 * quat.q0 * quat.q3,
                                            -2 * quat.q2 * quat.q2 - 2 * quat.q3 * quat.q3 + 1) * 57.3;


                    yawOffset = nYaw - qYaw;

                    if (rotation.rotationMode == DJI_GIMBAL_ROTATION_MODE_ABSOLUTE_ANGLE) {
                        rotation.yaw += yawOffset;
                    }
                }

                returnCode = DjiGimbalManager_Rotate(gimbalMountPosition, rotation);
                if (returnCode == DJI_ERROR_GIMBAL_MODULE_CODE_PITCH_REACH_POSITIVE_LIMIT ||
                    returnCode == DJI_ERROR_GIMBAL_MODULE_CODE_PITCH_REACH_NEGATIVE_LIMIT ||
                    returnCode == DJI_ERROR_GIMBAL_MODULE_CODE_ROLL_REACH_POSITIVE_LIMIT ||
                    returnCode == DJI_ERROR_GIMBAL_MODULE_CODE_ROLL_REACH_NEGATIVE_LIMIT ||
                    returnCode == DJI_ERROR_GIMBAL_MODULE_CODE_YAW_REACH_POSITIVE_LIMIT ||
                    returnCode == DJI_ERROR_GIMBAL_MODULE_CODE_YAW_REACH_NEGATIVE_LIMIT) {
                    USER_LOG_WARN("Reach limitation!");
                }
                else if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    USER_LOG_ERROR("DjiGimbalManager_Rotate return %x%08X", returnCode);
                    goto end;
                }

                osalHandler->TaskSleepMs(2000);

                if (aircraftSeries == DJI_AIRCRAFT_SERIES_M300 || aircraftSeries == DJI_AIRCRAFT_SERIES_M350) {
                    returnCode = DjiFcSubscription_GetLatestValueOfTopic(DJI_FC_SUBSCRIPTION_TOPIC_THREE_GIMBAL_DATA,
                                                                        (uint8_t *) &threeGimbalData,
                                                                        sizeof(T_DjiFcSubscriptionThreeGimbalData),
                                                                        &timestamp);
                    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                        USER_LOG_ERROR("DjiFcSubscription_GetLatestValueOfTopic return %x%08X", returnCode);
                        goto end;
                    }

                    if (threeGimbalData.anglesData[0].yaw < 0) {
                        threeGimbalData.anglesData[0].yaw = 360 + threeGimbalData.anglesData[0].yaw;
                    }

                    USER_LOG_INFO("reab back gimbal's (p, r, y): p=%.4f r=%.4f y=%.4f",
                                  threeGimbalData.anglesData[gimbalMountPosition - 1].pitch,
                                  threeGimbalData.anglesData[gimbalMountPosition - 1].roll,
                                  threeGimbalData.anglesData[gimbalMountPosition - 1].yaw);
                }
                else if (aircraftSeries == DJI_AIRCRAFT_SERIES_M30 || aircraftSeries == DJI_AIRCRAFT_SERIES_M3 ||
                         aircraftSeries == DJI_AIRCRAFT_SERIES_M3D) {
                    returnCode = DjiFcSubscription_GetLatestValueOfTopic(DJI_FC_SUBSCRIPTION_TOPIC_GIMBAL_ANGLES,
                                                                        (uint8_t *) &gimbalAngles,
                                                                        sizeof(T_DjiFcSubscriptionGimbalAngles),
                                                                        &timestamp);
                    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                        USER_LOG_ERROR("DjiFcSubscription_GetLatestValueOfTopic return %x%08X", returnCode);
                        goto end;
                    }

                    if (gimbalAngles.z < 0) {
                        gimbalAngles.z = 360 + gimbalAngles.z;
                    }

                    USER_LOG_INFO("read back gimbal's (p, r, y): p=%.4f r=%.4f y=%.4f",
                                  gimbalAngles.x, gimbalAngles.y, gimbalAngles.z);
                }
            }

            break;
        }
        case 'q':
            break;
        default:
            USER_LOG_ERROR("Input command is invalid");
            goto start;
    }

end:
    returnCode = DjiGimbalManager_Deinit();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Deinit gimbal manager error, return code 0x%08X", returnCode);
        return;
    }

    USER_LOG_INFO("Gimbal sample end");

    return;
}
/* Private functions definition-----------------------------------------------*/
void DjiTest_GimbalManagerShowSampleSelectList(const char **SampleList, uint8_t size) {
    uint8_t i = 0;

    for (i = 0; i < size; i++) {
        std::cout << SampleList[i] << std::endl;
    }
}
/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
