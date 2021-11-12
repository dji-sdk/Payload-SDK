/**
 ********************************************************************
 * @file    test_perception.hpp
 * @brief   This is the header file for "test_perception.cpp", defining the structure and
 * (exported) function prototypes.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef TEST_PERCEPTION_H
#define TEST_PERCEPTION_H

/* Includes ------------------------------------------------------------------*/
#include "dji_perception.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
using namespace std;

class PerceptionSample {
public:
    PerceptionSample();
    ~PerceptionSample();

    T_DjiReturnCode SubscribeFrontImage(DjiPerceptionImageCallback callback);
    T_DjiReturnCode SubscribeRearImage(DjiPerceptionImageCallback callback);
    T_DjiReturnCode SubscribeLeftImage(DjiPerceptionImageCallback callback);
    T_DjiReturnCode SubscribeRightImage(DjiPerceptionImageCallback callback);
    T_DjiReturnCode SubscribeUpImage(DjiPerceptionImageCallback callback);
    T_DjiReturnCode SubscribeDownImage(DjiPerceptionImageCallback callback);

    T_DjiReturnCode UnSubscribeFrontImage();
    T_DjiReturnCode UnSubscribeRearImage();
    T_DjiReturnCode UnSubscribeLeftImage();
    T_DjiReturnCode UnSubscribeRightImage();
    T_DjiReturnCode UnSubscribeUpImage();
    T_DjiReturnCode UnSubscribeDownImage();
private:
};

/* Exported functions --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif // TEST_PERCEPTION_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
