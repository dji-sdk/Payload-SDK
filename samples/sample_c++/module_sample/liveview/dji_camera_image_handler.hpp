/**
 ********************************************************************
 * @file    dji_camera_image_handler.hpp
 * @brief   This is the header file for "dji_camera_image_handler.cpp", defining the structure and
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
#ifndef DJI_CAMERA_IMAGE_HANDLER_H
#define DJI_CAMERA_IMAGE_HANDLER_H

/* Includes ------------------------------------------------------------------*/
#include "pthread.h"
#include <cstdint>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
struct CameraRGBImage {
    std::vector<uint8_t> rawData;
    int height;
    int width;
};

typedef void (*CameraImageCallback)(CameraRGBImage pImg, void *userData);

typedef void (*H264Callback)(const uint8_t *buf, int bufLen, void *userData);

class DJICameraImageHandler {
public:
    DJICameraImageHandler();
    ~DJICameraImageHandler();

    void writeNewImageWithLock(uint8_t *buf, int bufSize, int width, int height);
    bool getNewImageWithLock(CameraRGBImage &copyOfImage, int timeoutMilliSec);

private:
    pthread_mutex_t m_mutex;
    pthread_cond_t m_condv;
    CameraRGBImage m_img;
    bool m_newImageFlag;
};

/* Exported functions --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif // DJI_CAMERA_IMAGE_HANDLER_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
