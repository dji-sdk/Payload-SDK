/**
 ********************************************************************
 * @file    hal_network.c
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
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "hal_network.h"
#include "dji_logger.h"

/* Private constants ---------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private values -------------------------------------------------------------*/

/* Private functions declaration ---------------------------------------------*/

/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode HalNetWork_Init(const char *ipAddr, const char *netMask, T_DjiNetworkHandle *halObj)
{
    int32_t ret;
    char cmdStr[LINUX_CMD_STR_MAX_SIZE];
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    int32_t routeIp[4] = {0};
    int32_t genMask[4] = {0};

    if (ipAddr == NULL || netMask == NULL) {
        USER_LOG_ERROR("hal network config param error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    sscanf(ipAddr, "%d.%d.%d.%d", &routeIp[0], &routeIp[1], &routeIp[2], &routeIp[3]);
    sscanf(netMask, "%d.%d.%d.%d", &genMask[0], &genMask[1], &genMask[2], &genMask[3]);
    routeIp[0] &= genMask[0];
    routeIp[1] &= genMask[1];
    routeIp[2] &= genMask[2];
    routeIp[3] &= genMask[3];

    //Attention: need root permission to config ip addr and netmask.
    memset(cmdStr, 0, sizeof(cmdStr));

    snprintf(cmdStr, sizeof(cmdStr), "ifconfig %s up", LINUX_NETWORK_DEV);
    USER_LOG_DEBUG("%s", cmdStr);
    ret = system(cmdStr);
    if (ret != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Can't open the network."
                       "Probably the program not execute with root permission."
                       "Please use the root permission to execute the program.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    snprintf(cmdStr, sizeof(cmdStr), "ifconfig %s %s netmask %s", LINUX_NETWORK_DEV, ipAddr, netMask);
    USER_LOG_DEBUG("%s", cmdStr);
    ret = system(cmdStr);
    if (ret != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("cmd failed: %s", cmdStr);
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    osalHandler->TaskSleepMs(50);

    snprintf(cmdStr, sizeof(cmdStr), "ip route flush dev %s", LINUX_NETWORK_DEV);
    USER_LOG_DEBUG("%s", cmdStr);
    ret = system(cmdStr);
    if (ret != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("cmd failed: %s", cmdStr);
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    snprintf(cmdStr, sizeof(cmdStr), "route add -net %d.%d.%d.%d netmask %s dev %s",
        routeIp[0], routeIp[1], routeIp[2], routeIp[3], netMask, LINUX_NETWORK_DEV);
    USER_LOG_DEBUG("%s", cmdStr);
    ret = system(cmdStr);
    if (ret != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("cmd failed: %s", cmdStr);
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    osalHandler->TaskSleepMs(50);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode HalNetWork_DeInit(T_DjiNetworkHandle halObj)
{
    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode HalNetWork_GetDeviceInfo(T_DjiHalNetworkDeviceInfo *deviceInfo)
{
    deviceInfo->usbNetAdapter.vid = USB_NET_ADAPTER_VID;
    deviceInfo->usbNetAdapter.pid = USB_NET_ADAPTER_PID;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/* Private functions definition-----------------------------------------------*/

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
