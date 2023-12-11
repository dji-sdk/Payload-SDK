/**
 ********************************************************************
 * @file    test_liveview.cpp
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
#include "test_liveview.hpp"

/* Private constants ---------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private values -------------------------------------------------------------*/
std::map<::E_DjiLiveViewCameraPosition, DJICameraStreamDecoder *> streamDecoder;

/* Private functions declaration ---------------------------------------------*/
static void LiveviewConvertH264ToRgbCallback(E_DjiLiveViewCameraPosition position, const uint8_t *buf, uint32_t bufLen);

/* Exported functions definition ---------------------------------------------*/
LiveviewSample::LiveviewSample()
{
    T_DjiReturnCode returnCode;

    returnCode = DjiLiveview_Init();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        throw ("Liveview init failed");
    }

    streamDecoder = {
        {DJI_LIVEVIEW_CAMERA_POSITION_FPV,  (new DJICameraStreamDecoder())},
        {DJI_LIVEVIEW_CAMERA_POSITION_NO_1, (new DJICameraStreamDecoder())},
        {DJI_LIVEVIEW_CAMERA_POSITION_NO_2, (new DJICameraStreamDecoder())},
        {DJI_LIVEVIEW_CAMERA_POSITION_NO_3, (new DJICameraStreamDecoder())},
    };
}

LiveviewSample::~LiveviewSample()
{
    T_DjiReturnCode returnCode;

    returnCode = DjiLiveview_Deinit();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        perror("Liveview deinit failed");
    }

    for (auto pair : streamDecoder) {
        if (pair.second) {
            delete pair.second;
        }
    }
}

T_DjiReturnCode LiveviewSample::StartFpvCameraStream(CameraImageCallback callback, void *userData)
{
    auto deocder = streamDecoder.find(DJI_LIVEVIEW_CAMERA_POSITION_FPV);

    if ((deocder != streamDecoder.end()) && deocder->second) {
        deocder->second->init();
        deocder->second->registerCallback(callback, userData);

        return DjiLiveview_StartH264Stream(DJI_LIVEVIEW_CAMERA_POSITION_FPV, DJI_LIVEVIEW_CAMERA_SOURCE_DEFAULT,
                                           LiveviewConvertH264ToRgbCallback);
    } else {
        return DJI_ERROR_SYSTEM_MODULE_CODE_NOT_FOUND;
    }
}

T_DjiReturnCode LiveviewSample::StartMainCameraStream(CameraImageCallback callback, void *userData)
{
    auto deocder = streamDecoder.find(DJI_LIVEVIEW_CAMERA_POSITION_NO_1);

    if ((deocder != streamDecoder.end()) && deocder->second) {
        deocder->second->init();
        deocder->second->registerCallback(callback, userData);

        return DjiLiveview_StartH264Stream(DJI_LIVEVIEW_CAMERA_POSITION_NO_1, DJI_LIVEVIEW_CAMERA_SOURCE_DEFAULT,
                                           LiveviewConvertH264ToRgbCallback);
    } else {
        return DJI_ERROR_SYSTEM_MODULE_CODE_NOT_FOUND;
    }
}

T_DjiReturnCode LiveviewSample::StartViceCameraStream(CameraImageCallback callback, void *userData)
{
    auto deocder = streamDecoder.find(DJI_LIVEVIEW_CAMERA_POSITION_NO_2);

    if ((deocder != streamDecoder.end()) && deocder->second) {
        deocder->second->init();
        deocder->second->registerCallback(callback, userData);

        return DjiLiveview_StartH264Stream(DJI_LIVEVIEW_CAMERA_POSITION_NO_2, DJI_LIVEVIEW_CAMERA_SOURCE_DEFAULT,
                                           LiveviewConvertH264ToRgbCallback);
    } else {
        return DJI_ERROR_SYSTEM_MODULE_CODE_NOT_FOUND;
    }
}

T_DjiReturnCode LiveviewSample::StartTopCameraStream(CameraImageCallback callback, void *userData)
{
    auto deocder = streamDecoder.find(DJI_LIVEVIEW_CAMERA_POSITION_NO_3);

    if ((deocder != streamDecoder.end()) && deocder->second) {
        deocder->second->init();
        deocder->second->registerCallback(callback, userData);

        return DjiLiveview_StartH264Stream(DJI_LIVEVIEW_CAMERA_POSITION_NO_3, DJI_LIVEVIEW_CAMERA_SOURCE_DEFAULT,
                                           LiveviewConvertH264ToRgbCallback);
    } else {
        return DJI_ERROR_SYSTEM_MODULE_CODE_NOT_FOUND;
    }
}

T_DjiReturnCode LiveviewSample::StopFpvCameraStream()
{
    T_DjiReturnCode returnCode;

    returnCode = DjiLiveview_StopH264Stream(DJI_LIVEVIEW_CAMERA_POSITION_FPV, DJI_LIVEVIEW_CAMERA_SOURCE_DEFAULT);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        return returnCode;
    }

    auto deocder = streamDecoder.find(DJI_LIVEVIEW_CAMERA_POSITION_FPV);
    if ((deocder != streamDecoder.end()) && deocder->second) {
        deocder->second->cleanup();
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode LiveviewSample::StopMainCameraStream()
{
    T_DjiReturnCode returnCode;

    returnCode = DjiLiveview_StopH264Stream(DJI_LIVEVIEW_CAMERA_POSITION_NO_1, DJI_LIVEVIEW_CAMERA_SOURCE_DEFAULT);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        return returnCode;
    }

    auto deocder = streamDecoder.find(DJI_LIVEVIEW_CAMERA_POSITION_NO_1);
    if ((deocder != streamDecoder.end()) && deocder->second) {
        deocder->second->cleanup();
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode LiveviewSample::StopViceCameraStream()
{
    T_DjiReturnCode returnCode;

    returnCode = DjiLiveview_StopH264Stream(DJI_LIVEVIEW_CAMERA_POSITION_NO_2, DJI_LIVEVIEW_CAMERA_SOURCE_DEFAULT);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        return returnCode;
    }

    auto deocder = streamDecoder.find(DJI_LIVEVIEW_CAMERA_POSITION_NO_2);
    if ((deocder != streamDecoder.end()) && deocder->second) {
        deocder->second->cleanup();
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode LiveviewSample::StopTopCameraStream()
{
    T_DjiReturnCode returnCode;

    returnCode = DjiLiveview_StopH264Stream(DJI_LIVEVIEW_CAMERA_POSITION_NO_3, DJI_LIVEVIEW_CAMERA_SOURCE_DEFAULT);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        return returnCode;
    }

    auto deocder = streamDecoder.find(DJI_LIVEVIEW_CAMERA_POSITION_NO_3);
    if ((deocder != streamDecoder.end()) && deocder->second) {
        deocder->second->cleanup();
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/* Private functions definition-----------------------------------------------*/
static void LiveviewConvertH264ToRgbCallback(E_DjiLiveViewCameraPosition position, const uint8_t *buf, uint32_t bufLen)
{
    auto deocder = streamDecoder.find(position);
    if ((deocder != streamDecoder.end()) && deocder->second) {
        deocder->second->decodeBuffer(buf, bufLen);
    }
}

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/