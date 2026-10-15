/**
 ********************************************************************
 * @file    test_flight_controller_entry.cpp
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
#include <flight_control/test_flight_control.h>
#include "test_flight_controller_entry.h"
#include "dji_logger.h"
#include "test_flight_controller_command_flying.h"
#include <waypoint_v2/test_waypoint_v2.h>
#include <waypoint_v3/test_waypoint_v3.h>
#include <interest_point/test_interest_point.h>

/* Private constants ---------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private values -------------------------------------------------------------*/

/* Private functions declaration ---------------------------------------------*/

/* Exported functions definition ---------------------------------------------*/
void DjiUser_RunFlightControllerSample(void)
{
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    char inputSelectSample;

start:
    osalHandler->TaskSleepMs(100);

    std::cout
        << "\n"
        << "| Available commands:                                                                                            |\n"
        << "| [0] Flight controller sample - control flying with keyboard                                                    |\n"
        << "| [1] Flight controller sample - take off landing                                                                |\n"
        << "| [2] Flight controller sample - take off position ctrl landing                                                  |\n"
        << "| [3] Flight controller sample - take off go home force landing                                                  |\n"
        << "| [4] Flight controller sample - take off velocity ctrl landing                                                  |\n"
        << "| [5] Flight controller sample - arrest flying                                                                   |\n"
        << "| [6] Flight controller sample - set get parameters                                                              |\n"
        << "| [7] Waypoint 2.0 sample - run airline mission by settings (only support on M300 RTK)                           |\n"
        << "| [8] Waypoint 3.0 sample - run airline mission by kmz file (not support on M300 RTK)                            |\n"
        << "| [9] Interest point sample - run interest point mission by settings (only support on M3E/M3T)                   |\n"
        << "| [a] EU-C6 FTS trigger sample - receive fts callback to trigger parachute function (only support on M3D/M3DT)   |\n"
        << "| [b] Slow rotate blade sample, only support on M400                                                             |\n"
        << "| [c] Select the FTS PWM trigger on E-Port or E-Port Lite (either one). Supported on M4/M4T/M4D/M4TD.            |\n"
        << "| [d] Select the FTS PWM trigger on Port 4 (M400 only). Not supported on Manifold 3.                             |\n"
        << "| [e] Flight controller sample - set get perception parameters, support on M400                                  |\n"
        << "| [f] Flight controller sample - set cmd start mission, support on M400                                          |\n"
        << "| [g] Flight controller sample - get battery capacity gohome                                                     |\n"
        << "| [h] Flight controller sample - register osd info, get relative high                                            |\n"
        << "| [i] Flight controller sample - stealth mode control                                                            |\n"
        << "| [j] Flight controller sample - spz approach control                                                            |\n"
        << "| [k] Flight controller sample - no gnss mode, heading go home, support on M4D                                   |\n"
        << "| [l] Flight controller sample - flight control no gps mode and function                                         |\n"
        << "| [m] Flight controller sample - extreme battery endurance mode control                                           |\n"
        << std::endl;

    std::cin >> inputSelectSample;
    switch (inputSelectSample) {
        case '0':
            DjiUser_RunFlightControllerCommandFlyingSample();
            goto start;
        case '1':
            DjiTest_FlightControlRunSample(E_DJI_TEST_FLIGHT_CTRL_SAMPLE_SELECT_TAKE_OFF_LANDING);
            goto start;
        case '2':
            DjiTest_FlightControlRunSample(E_DJI_TEST_FLIGHT_CTRL_SAMPLE_SELECT_TAKE_OFF_POSITION_CTRL_LANDING);
            goto start;
        case '3':
            DjiTest_FlightControlRunSample(E_DJI_TEST_FLIGHT_CTRL_SAMPLE_SELECT_TAKE_OFF_GO_HOME_FORCE_LANDING);
            goto start;
        case '4':
            DjiTest_FlightControlRunSample(E_DJI_TEST_FLIGHT_CTRL_SAMPLE_SELECT_TAKE_OFF_VELOCITY_CTRL_LANDING);
            goto start;
        case '5':
            DjiTest_FlightControlRunSample(E_DJI_TEST_FLIGHT_CTRL_SAMPLE_SELECT_ARREST_FLYING);
            goto start;
        case '6':
            DjiTest_FlightControlRunSample(E_DJI_TEST_FLIGHT_CTRL_SAMPLE_SELECT_SET_GET_PARAM);
            goto start;
        case '7':
            DjiTest_WaypointV2RunSample();
            break;
        case '8':
            DjiTest_WaypointV3RunSample();
            break;
        case '9':
            DjiTest_InterestPointRunSample();
            break;
        case 'a':
            DjiTest_FlightControlRunSample(E_DJI_TEST_FLIGHT_CTRL_SAMPLE_SELECT_FTS_TRIGGER);
            break;
        case 'b':
            DjiTest_FlightControlRunSample(E_DJI_TEST_FLIGHT_CTRL_SAMPLE_SELECT_SLOW_ROTATE_BLADE);
            break;
        case 'c':
            DjiTest_FtsPwmTriggerSample(DJI_MOUNT_POSITION_EXTENSION_PORT, "DJI_MOUNT_POSITION_EXTENSION_PORT");
            // or DJI_MOUNT_POSITION_EXTENSION_LITE_PORT
            DjiTest_FtsPwmTriggerSample(DJI_MOUNT_POSITION_EXTENSION_LITE_PORT, "DJI_MOUNT_POSITION_EXTENSION_LITE_PORT");
            break;
        case 'd': // for m400
            DjiTest_FtsPwmTriggerSample(DJI_MOUNT_POSITION_EXTENSION_PORT_V2_NO4, "DJI_MOUNT_POSITION_EXTENSION_PORT_V2_NO4");
            break;
        case 'e' : // for m400
            DjiTest_FlightControlRunSample(E_DJI_TEST_FLIGHT_CTRL_SAMPLE_SELECT_SET_GET_PERCEPTION_PARAM);
            goto start;
        case 'f' : // for m400
            DjiTest_FlightControlRunSample(E_DJI_TEST_FLIGHT_CTRL_SAMPLE_SELECT_SET_CMD_START_MISSION);
            goto start;
        case 'g':
            DjiTest_FlightControlRunSample(E_DJI_TEST_FLIGHT_CTRL_SAMPLE_SELECT_GET_BATTERY_CAPACITY_GOHOME);
            goto start;
        case 'h':
            DjiTest_FlightControlRunSample(E_DJI_TEST_FLIGHT_CTRL_SAMPLE_SELECT_REGISTER_OSD_INFO);
            goto start;
        case 'i':
            DjiTest_FlightControlRunSample(E_DJI_TEST_FLIGHT_CTRL_SAMPLE_SELECT_STEALTH_MODE);
            goto start;
        case 'j': {
            uint16_t cx, cy, w, h;
            float speed, angle;
            std::cout << "Enter SPZ box params (cx cy w h, range 0~10000): ";
            std::cin >> cx >> cy >> w >> h;
            std::cout << "Enter approach params (speed_ms angle_deg): ";
            std::cin >> speed >> angle;
            DjiTest_SpzSetBoxParam(cx, cy, w, h);
            DjiTest_SpzSetApproachParam(speed, angle);
            DjiTest_SpzApproachRunSample();
            goto start;
        }
        case 'k':
            DjiTest_FlightControlRunSample(E_DJI_TEST_FLIGHT_CTRL_SAMPLE_SELECT_NO_GNSS_MODE);
            goto start;
        case 'l':
            DjiTest_FlightControlRunSample(E_DJI_TEST_FLIGHT_CTRL_SAMPLE_SELECT_NO_GPS_MODE);
            goto start;
        case 'm':
            DjiTest_FlightControlRunSample(E_DJI_TEST_FLIGHT_CTRL_SAMPLE_SELECT_EXTREME_BATTERY_ENDURANCE_MODE);
            goto start;
        case 'q':
            break;
        default:
            USER_LOG_ERROR("Input command is invalid");
            goto start;
    }
}

/* Private functions definition-----------------------------------------------*/

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
