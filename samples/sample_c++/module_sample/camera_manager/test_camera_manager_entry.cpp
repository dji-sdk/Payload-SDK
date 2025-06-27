/**
 ********************************************************************
 * @file    test_camera_manager_entry.cpp
 * @brief
 *
 * @copyright (c) 2018 DJI. All rights reserved.
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
#include <iostream>
#include <string>
#include <dji_logger.h>
#include <dji_typedef.h>
#include "utils/util_misc.h"
#include "dji_camera_manager.h"
#include "camera_manager/test_camera_manager.h"
#include "camera_manager/test_camera_manager_entry.h"
#include "test_camera_manager_entry.h"

using namespace std;

/* Private constants ---------------------------------------------------------*/
static const char *s_cameraManagerSampleSelectList[] = {
    [E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_CAMERA_SHUTTER_SPEED]       = "Set camera shutter speed                            |",
    [E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_CAMERA_APERTURE]            = "Set camera aperture                                 |",
    [E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_CAMERA_EV]                  = "Set camera ev                                       |",
    [E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_CAMERA_ISO]                 = "Set camera iso                                      |",
    [E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_CAMERA_FOCUS_POINT]         = "Set camera focus point                              |",
    [E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_CAMERA_TAP_ZOOM_POINT]      = "Set camera tap zoom point                           |",
    [E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_CAMERA_ZOOM_PARAM]          = "Set camera zoom param                               |",
    [E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SHOOT_SINGLE_PHOTO]             = "Shoot single photo                                  |",
    [E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SHOOT_BURST_PHOTO]              = "Shoot busrt photo                                   |",
    [E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SHOOT_INTERVAL_PHOTO]           = "Shoot interval photo                                |",
    [E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_RECORD_VIDEO]                   = "Record video                                        |",
    [E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_DOWNLOAD_AND_DELETE_MEDIA_FILE] = "Download and delete media file                      |",
    [E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_DOWNLOAD_FILE_LIST_BY_SLICES]   = "Download file list by slices                        |",
    [E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_THERMOMETRY]                    = "Thermometry test                                    |",
    [E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_GET_LIDAR_RANGING_INFO]         = "Get lidar ranging info                              |",
    [E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_IR_CAMERA_ZOOM_PARAM]           = "Set ir camera zoom param                            |",
    [E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_NIGHT_SCENE_MODE]           = "Set night scene mode                                |",
    [E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_CAPTURE_RECORDING_STREAMS]  = "Set capture or recording streams storage type       |",
    [E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SHOW_STORAGE_INFO]              = "Show storage info                                   |",
    [E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_FORMAT_SD_CARD]                 = "Format SD card                                      |",
    [E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_LINK_ZOOM]                  = "Set synchronized split screen zoom                  |",
    [E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_USER_CUSTOM_DIR_FILE_NAME]  = "Set custom directory or file name of media file     |",
    [E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_RESET_CAMERA_SETTINGS]          = "Reset camera settings                               |",
    [E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_AE_LOCK_MODE]               = "Set AE lock mode                                    |",
    [E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_FOCUS_RING_VALUE]           = "Set focus ring value                                |",
    [E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_CONNECT_STATUS_TEST]            = "Camera connect status test                          |",
    [E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_GET_PHOTO_VIDEO_PARAM]      = "Set camera photo and video storage param            |",
    [E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_METERING_MODE]              = "Set camera metering mode                            |",
    [E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_METERING_POINT]             = "Set and get camera metering point                   |",
    [E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_FFC_MODE_AND_TRRIGER]           = "Set FFC mode and trriger a FFC                      |",
    [E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_GAIN_MODE]                  = "Set infrared camera gain mode                       |",
    [E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_GET_CAMERA_STATUS]              = "Get camera status, capturing & recording status etc |",
    [E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SUBSCRIBE_POINT_CLOUD]          = "Subscribe point cloud                               |",
};

/* Private types -------------------------------------------------------------*/

/* Private values -------------------------------------------------------------*/

/* Private functions declaration ---------------------------------------------*/
static void DjiTest_CmareManagerShowStringList(const char **list, int size)
{
    for (int i = 0; i < size; i++) {
        printf("| [%2d] %s\r\n", i, list[i]);
    }
}

static void DjiTest_CameraManagerShowSampleSelectList(void)
{
    DjiTest_CmareManagerShowStringList(s_cameraManagerSampleSelectList,
                                       UTIL_ARRAY_SIZE(s_cameraManagerSampleSelectList));
}

/* Exported functions definition ---------------------------------------------*/
void DjiUser_RunCameraManagerSample(void)
{
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    string mountPositionStr;
    int posNum;
    E_DjiMountPosition cameraMountPosition;
    string sampleSelectStr;
    int sampleSelectNum;
    E_DjiTestCameraManagerSampleSelect cameraSample;

    USER_LOG_INFO("DjiUser_RunCameraManagerSample");

    while (true) {
        osalHandler->TaskSleepMs(10);
        cout
            << "| Available position:                                                                              |"
            <<
            endl;
        std::cout
        << "| [1 ~ 4] Select camera mount position at NO.1~NO.4                                                    |"
        <<
        std::endl;
        cout
            << "| [q] Quit                                                                                         |"
            <<
            endl;

        cout << "Please input number to select position (input q to quit): ";
        cin >> mountPositionStr;
        if (mountPositionStr == "q") {
            return;
        }

        posNum = atoi(mountPositionStr.c_str());

        if (posNum > 4 || posNum < 1) {
            USER_LOG_ERROR("Input mount position is invalid");
            continue;
        } else {
            break;
        }
    }

    cameraMountPosition = E_DjiMountPosition(posNum);

    while (true) {
        osalHandler->TaskSleepMs(10);
        cout << "\nAvailable samples:\n";
        DjiTest_CameraManagerShowSampleSelectList();

        cout << "Please input number to select sample (input q to quit): ";
        cin >> sampleSelectStr;

        if (sampleSelectStr == "q") {
            return;
        }

        sampleSelectNum = atoi(sampleSelectStr.c_str());

        if (sampleSelectNum < 0 ||
            sampleSelectNum >= (int) E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_INDEX_MAX) {
            USER_LOG_ERROR("Input camera sample is invalid");
            continue;
        }

        cameraSample = (E_DjiTestCameraManagerSampleSelect) sampleSelectNum;

        cout << "Start test: position " << cameraMountPosition
             << ", sample " << cameraSample << endl;

        DjiTest_CameraManagerRunSample(cameraMountPosition, cameraSample);
    }

}

/* Private functions definition-----------------------------------------------*/

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
