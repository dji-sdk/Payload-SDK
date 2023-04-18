/**
 ********************************************************************
 * @file    dji_camera_stream_decoder.hpp
 * @brief   This is the header file for "dji_camera_stream_decoder.cpp", defining the structure and
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
#ifndef DJI_CAMERA_STREAM_DECCODER_H
#define DJI_CAMERA_STREAM_DECCODER_H

/* Includes ------------------------------------------------------------------*/
extern "C" {
#ifdef FFMPEG_INSTALLED
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#endif
}

#include "pthread.h"
#include "dji_camera_image_handler.hpp"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
class DJICameraStreamDecoder {
public:
    DJICameraStreamDecoder();
    ~DJICameraStreamDecoder();
    bool init();
    void cleanup();

    void callbackThreadFunc();
    void decodeBuffer(const uint8_t *pBuf, int len);
    static void *callbackThreadEntry(void *p);
    bool registerCallback(CameraImageCallback f, void *param);
    DJICameraImageHandler decodedImageHandler;

private:
    pthread_t callbackThread;
    bool initSuccess;
    bool cbThreadIsRunning;
    int cbThreadStatus;
    CameraImageCallback cb;
    void *cbUserParam;

    pthread_mutex_t decodemutex;

#ifdef FFMPEG_INSTALLED
    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;
    AVCodecParserContext *pCodecParserCtx;
    SwsContext *pSwsCtx;

    AVFrame *pFrameYUV;
    AVFrame *pFrameRGB;
#endif
    uint8_t *rgbBuf;
    size_t bufSize;
};

/* Exported functions --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif // DJI_CAMERA_STREAM_DECCODER_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/









