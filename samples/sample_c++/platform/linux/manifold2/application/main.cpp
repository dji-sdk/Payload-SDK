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
#include "application.hpp"

/* Private constants ---------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private values -------------------------------------------------------------*/

/* Private functions declaration ---------------------------------------------*/

/* Exported functions definition ---------------------------------------------*/
int main(int argc, char **argv)
{
    Application application(argc, argv);
    char inputChar;

    std::cout
        << "\n"
        << "| Available commands:                                        |\n"
        << "| [0] Camera stream view sample                              |\n"
        << "| [1] Stereo vision view sample                              |\n"
        << std::endl;

    std::cin >> inputChar;
    switch (inputChar) {
        case '0':
            DjiUser_RunCameraStreamViewSample();
            break;
        case '1':
            DjiUser_RunStereoVisionViewSample();
            break;
        default:
            break;
    }

    return 0;
}

/* Private functions definition-----------------------------------------------*/

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
