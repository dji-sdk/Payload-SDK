/**
 ********************************************************************
 * @file    hal_network.c
 * @version V2.0.0
 * @date    3/27/20
 * @brief
 *
 * @copyright (c) 2018-2019 DJI. All rights reserved.
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
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "hal_network.h"
#include "psdk_logger.h"

/* Private constants ---------------------------------------------------------*/
#define LINUX_NETWORK_DEV           "enp0s31f6"
#define LINUX_CMD_STR_MAX_SIZE      (128)

/* Private types -------------------------------------------------------------*/

/* Private values -------------------------------------------------------------*/

/* Private functions declaration ---------------------------------------------*/

/* Exported functions definition ---------------------------------------------*/
T_PsdkReturnCode HalNetWork_Config(const char *ipAddr, const char *netMask)
{
    int32_t ret;
    char cmdStr[LINUX_CMD_STR_MAX_SIZE];

    if (ipAddr == NULL || netMask == NULL) {
        PsdkLogger_UserLogError("hal network config param error");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    //Attention: need root permission to config ip addr and netmask.
    memset(cmdStr, 0, sizeof(cmdStr));

    snprintf(cmdStr, sizeof(cmdStr), "ifconfig | grep %s", LINUX_NETWORK_DEV);
    ret = system(cmdStr);
    if (ret != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Can't find the network device. "
                                "Probably the name of network device is not match or device is not properly initialized. "
                                "Please check the name or status of network device. ");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    snprintf(cmdStr, sizeof(cmdStr), "ifconfig %s up", LINUX_NETWORK_DEV);
    ret = system(cmdStr);
    if (ret != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Can't open the network. "
                                "Probably the program not execute with root permission. "
                                "Please use the root permission to execute the program. ");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    snprintf(cmdStr, sizeof(cmdStr), "ifconfig %s %s netmask %s", LINUX_NETWORK_DEV, ipAddr, netMask);
    ret = system(cmdStr);
    if (ret != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Can't config the ip address of network. "
                                "Probably the program not execute with root permission. "
                                "Please use the root permission to execute the program. ");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/* Private functions definition-----------------------------------------------*/

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
