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
#include <iostream>

/* Private constants ---------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/
typedef enum {
    E_DJI_TEST_GIMBAL_MANAGER_SAMPLE_SELECT_ROTATE_GIMBAL_ON_FREE_MODE,
    E_DJI_TEST_GIMBAL_MANAGER_SAMPLE_SELECT_ROTATE_GIMBAL_ON_YAW_FOLLOW_MODE,
    E_DJI_TEST_GIMBAL_MANAGER_SAMPLE_SELECT_QUIT,
} E_DjiTestGimbalManagerSampleSelect;

/* Private values -------------------------------------------------------------*/
static const char *s_gimbalManagerSampleList[] = {
    [E_DJI_TEST_GIMBAL_MANAGER_SAMPLE_SELECT_ROTATE_GIMBAL_ON_FREE_MODE] =
    "| [0] Gimbal manager sample - Rotate gimbal on free mode                                           |",
    [E_DJI_TEST_GIMBAL_MANAGER_SAMPLE_SELECT_ROTATE_GIMBAL_ON_YAW_FOLLOW_MODE] =
    "| [1] Gimbal manager sample - Rotate gimbal on yaw follow mode                                     |",
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
        case 'q':
            break;
        default:
            USER_LOG_ERROR("Input command is invalid");
            goto start;
    }

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
