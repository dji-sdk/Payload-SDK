/**
 ********************************************************************
 * @file    main.cpp
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
#include <liveview/test_liveview_entry.hpp>
#include <perception/test_perception_entry.hpp>
#include <perception/test_lidar_entry.hpp>
#include <perception/test_radar_entry.hpp>
#include <flight_control/test_flight_control.h>
#include <gimbal/test_gimbal_entry.hpp>
#include "application.hpp"
#include "fc_subscription/test_fc_subscription.h"
#include <gimbal_emu/test_payload_gimbal_emu.h>
#include <camera_emu/test_payload_cam_emu_media.h>
#include <camera_emu/test_payload_cam_emu_base.h>
#include <dji_logger.h>
#include "widget/test_widget.h"
#include "widget/test_widget_speaker.h"
#include <widget_manager/test_widget_manager.hpp>
#include <power_management/test_power_management.h>
#include "data_transmission/test_data_transmission.h"
#include <flight_controller/test_flight_controller_entry.h>
#include <positioning/test_positioning.h>
#include <hms_manager/hms_manager_entry.h>
#include "camera_manager/test_camera_manager_entry.h"
#include <hms_manager/hms_manager_entry.h>
#include <liveview/dji_liveview_object_detection.hpp>
#include "liveview/test_liveview.h"
#include <signal.h>
/* Private constants ---------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private values -------------------------------------------------------------*/

/* Private functions declaration ---------------------------------------------*/

/* Exported functions definition ---------------------------------------------*/
int main(int argc, char **argv)
{
    signal(SIGINT, [](int signalNum) -> void { exit(0); });
    Application application(argc, argv);
    char inputChar;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    T_DjiReturnCode returnCode;
    E_DjiMountPosition mountPosition = DJI_MOUNT_POSITION_PAYLOAD_PORT_NO1;

start:
    std::cout
        << "\n"
        << "| Available commands:                                                                              |\n"
        << "| [0] Fc subscribe sample - subscribe quaternion and gps data                                      |\n"
        << "| [1] Flight controller sample - you can control flying by PSDK                                    |\n"
        << "| [2] Hms info manager sample - get health manger system info by language                          |\n"
        << "| [a] Gimbal manager sample - you can control gimbal by PSDK                                       |\n"
        << "| [d] Stereo vision view sample - display the stereo image                                         |\n"
        << "| [e] Run camera manager sample - you can test camera's functions interactively                    |\n"
        << "| [f] Start rtk positioning sample - you can receive rtk rtcm data when rtk signal is ok           |\n"
        << "| [g] Request Lidar data sample - Request Lidar data and store the point cloud data as pcd files   |\n"
        << "| [h] Request Radar data sample - Request radar data                                               |\n"
        << "| [i] Run manifold3 AI sample - request h.264 bitstream data, codec it and display it on pilot     |\n"
        << "| [j] Run Hms Enhance sample - shake motor and play sound on pilot                                 |\n"
        << "| [l] Run widget states manager sample, control widget states on other payload                     |\n"
        << "| [m] Run Open Ar sample - draw ar gragh                                                           |\n"
        << "| [n] Run H.264 liveview sample - save H.264 files in the current directory                        |\n"
        << std::endl;

    std::cin >> inputChar;
    switch (inputChar) {
        case '0':
            DjiTest_FcSubscriptionRunSample();
            break;
        case '1':
            DjiUser_RunFlightControllerSample();
            break;
        case '2':
            DjiUser_RunHmsManagerSample();
            break;
        case 'a':
            DjiUser_RunGimbalManagerSample();
            break;
        case 'd':
            DjiUser_RunStereoVisionViewSample();
            break;
        case 'e':
            DjiUser_RunCameraManagerSample();
            break;
        case 'f':
            returnCode = DjiTest_PositioningStartService();
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                USER_LOG_ERROR("rtk positioning sample init error");
                break;
            }
            USER_LOG_INFO("Start rtk positioning sample successfully");
            break;
        case 'g':
            DjiUser_RunLidarDataSubscriptionSample();
            break;
        case 'h':
            DjiUser_RunRadarDataSubscriptionSample();
            break;
        case 'i':
            DjiUser_RunCameraAiDetectionSample();
            break;
        case 'j':
            DjiUser_RunHmsEnhanceSample();
            break;
        case 'l':
            DjiTest_WidgetMannagerStart();
            break;
        case 'm':
            DjiUser_RunOpenArSample();
            break;
        case 'n':
            DjiTest_LiveviewRunSample(mountPosition);
            break;
        case 'q':
            break;
        default:
            break;
    }

    osalHandler->TaskSleepMs(2000);

    goto start;
}

/* Private functions definition-----------------------------------------------*/

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
