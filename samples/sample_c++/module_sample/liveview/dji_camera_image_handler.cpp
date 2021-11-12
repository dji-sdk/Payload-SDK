/**
 ********************************************************************
 * @file    dji_camera_image_handler.cpp
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
#include "dji_camera_image_handler.hpp"

/* Private constants ---------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private values -------------------------------------------------------------*/

/* Private functions declaration ---------------------------------------------*/

/* Exported functions definition ---------------------------------------------*/
DJICameraImageHandler::DJICameraImageHandler() : m_newImageFlag(false)
{
    pthread_mutex_init(&m_mutex, NULL);
    pthread_cond_init(&m_condv, NULL);
}

DJICameraImageHandler::~DJICameraImageHandler()
{
    pthread_mutex_destroy(&m_mutex);
    pthread_cond_destroy(&m_condv);
}

bool DJICameraImageHandler::getNewImageWithLock(CameraRGBImage &copyOfImage, int timeoutMilliSec)
{
    int result;

    /*! @note
     * Here result == 0 means successful.
     * Because this is the behavior of pthread_cond_timedwait.
     */
    pthread_mutex_lock(&m_mutex);
    if (m_newImageFlag) {
        /* At this point, a copy of m_img is made, so it is safe to
         * do any modifications to copyOfImage in user code.
         */
        copyOfImage = m_img;
        m_newImageFlag = false;
        result = 0;
    } else {
        struct timespec absTimeout;
        clock_gettime(CLOCK_REALTIME, &absTimeout);
        absTimeout.tv_nsec += timeoutMilliSec * 1e6;
        result = pthread_cond_timedwait(&m_condv, &m_mutex, &absTimeout);

        if (result == 0) {
            copyOfImage = m_img;
            m_newImageFlag = false;
        }
    }
    pthread_mutex_unlock(&m_mutex);
    return (result == 0) ? true : false;
}

void DJICameraImageHandler::writeNewImageWithLock(uint8_t *buf, int bufSize, int width, int height)
{
    pthread_mutex_lock(&m_mutex);

    m_img.rawData.assign(buf, buf + bufSize);
    m_img.height = height;
    m_img.width = width;
    m_newImageFlag = true;

    pthread_cond_signal(&m_condv);
    pthread_mutex_unlock(&m_mutex);
}

/* Private functions definition-----------------------------------------------*/

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
