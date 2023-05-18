/**
 ********************************************************************
 * @file    dji_application.hpp
 * @brief   This is the header file for "dji_application.cpp", defining the structure and
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
#ifndef APPLICATION_H
#define APPLICATION_H

/* Includes ------------------------------------------------------------------*/
#include <iostream>
#include <fstream>
#include "dji_typedef.h"
#include "dji_core.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
using namespace std;

class Application {
public:
    Application(int argc, char **argv);
    ~Application();

private:
    static void DjiUser_SetupEnvironment();
    static void DjiUser_ApplicationStart();
    static T_DjiReturnCode DjiUser_PrintConsole(const uint8_t *data, uint16_t dataLen);
    static T_DjiReturnCode DjiUser_LocalWrite(const uint8_t *data, uint16_t dataLen);
    static T_DjiReturnCode DjiUser_FillInUserInfo(T_DjiUserInfo *userInfo);
    static T_DjiReturnCode DjiUser_LocalWriteFsInit(const char *path);
};

/* Exported functions --------------------------------------------------------*/


#ifdef __cplusplus
}
#endif

#endif // APPLICATION_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
