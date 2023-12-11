/**
 ********************************************************************
 * @file    test_perception.cpp
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
#include <stdexcept>
#include "test_perception.hpp"
#include "dji_logger.h"

/* Private constants ---------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private values -------------------------------------------------------------*/

/* Private functions declaration ---------------------------------------------*/

/* Exported functions definition ---------------------------------------------*/
PerceptionSample::PerceptionSample()
{
    T_DjiReturnCode returnCode;

    returnCode = DjiPerception_Init();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        if (returnCode == DJI_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT) {
            USER_LOG_ERROR("Perception feature will support on later version.");
        }

        throw ("Perception init failed");
    }
}

PerceptionSample::~PerceptionSample()
{
    T_DjiReturnCode returnCode;

    returnCode = DjiPerception_Deinit();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        perror("Perception deinit failed");
    }
}

T_DjiReturnCode PerceptionSample::SubscribeFrontImage(DjiPerceptionImageCallback callback)
{
    return DjiPerception_SubscribePerceptionImage(DJI_PERCEPTION_RECTIFY_FRONT, callback);
}

T_DjiReturnCode PerceptionSample::SubscribeRearImage(DjiPerceptionImageCallback callback)
{
    return DjiPerception_SubscribePerceptionImage(DJI_PERCEPTION_RECTIFY_REAR, callback);
}

T_DjiReturnCode PerceptionSample::SubscribeLeftImage(DjiPerceptionImageCallback callback)
{
    return DjiPerception_SubscribePerceptionImage(DJI_PERCEPTION_RECTIFY_LEFT, callback);
}

T_DjiReturnCode PerceptionSample::SubscribeRightImage(DjiPerceptionImageCallback callback)
{
    return DjiPerception_SubscribePerceptionImage(DJI_PERCEPTION_RECTIFY_RIGHT, callback);
}

T_DjiReturnCode PerceptionSample::SubscribeUpImage(DjiPerceptionImageCallback callback)
{
    return DjiPerception_SubscribePerceptionImage(DJI_PERCEPTION_RECTIFY_UP, callback);
}

T_DjiReturnCode PerceptionSample::SubscribeDownImage(DjiPerceptionImageCallback callback)
{
    return DjiPerception_SubscribePerceptionImage(DJI_PERCEPTION_RECTIFY_DOWN, callback);
}

T_DjiReturnCode PerceptionSample::UnSubscribeFrontImage()
{
    return DjiPerception_UnsubscribePerceptionImage(DJI_PERCEPTION_RECTIFY_FRONT);
}

T_DjiReturnCode PerceptionSample::UnSubscribeRearImage()
{
    return DjiPerception_UnsubscribePerceptionImage(DJI_PERCEPTION_RECTIFY_REAR);
}

T_DjiReturnCode PerceptionSample::UnSubscribeLeftImage()
{
    return DjiPerception_UnsubscribePerceptionImage(DJI_PERCEPTION_RECTIFY_LEFT);
}

T_DjiReturnCode PerceptionSample::UnSubscribeRightImage()
{
    return DjiPerception_UnsubscribePerceptionImage(DJI_PERCEPTION_RECTIFY_RIGHT);
}

T_DjiReturnCode PerceptionSample::UnSubscribeUpImage()
{
    return DjiPerception_UnsubscribePerceptionImage(DJI_PERCEPTION_RECTIFY_UP);
}

T_DjiReturnCode PerceptionSample::UnSubscribeDownImage()
{
    return DjiPerception_UnsubscribePerceptionImage(DJI_PERCEPTION_RECTIFY_DOWN);
}

/* Private functions definition-----------------------------------------------*/

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
