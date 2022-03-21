/**
 ********************************************************************
 * @file    osal_socket.h
 * @brief   This is the header file for "osal_socket.c", defining the structure and
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
#ifndef OSAL_SOCKET_H
#define OSAL_SOCKET_H

/* Includes ------------------------------------------------------------------*/
#include "dji_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
T_DjiReturnCode Osal_Socket(E_DjiSocketMode mode, T_DjiSocketHandle *socketHandle);

T_DjiReturnCode Osal_Close(T_DjiSocketHandle socketHandle);

T_DjiReturnCode Osal_Bind(T_DjiSocketHandle socketHandle, const char *ipAddr, uint32_t port);

T_DjiReturnCode Osal_UdpSendData(T_DjiSocketHandle socketHandle, const char *ipAddr, uint32_t port,
                                 const uint8_t *buf, uint32_t len, uint32_t *realLen);

T_DjiReturnCode Osal_UdpRecvData(T_DjiSocketHandle socketHandle, char *ipAddr, uint32_t *port,
                                 uint8_t *buf, uint32_t len, uint32_t *realLen);

T_DjiReturnCode Osal_TcpListen(T_DjiSocketHandle socketHandle);

T_DjiReturnCode Osal_TcpAccept(T_DjiSocketHandle socketHandle, char *ipAddr, uint32_t *port,
                               T_DjiSocketHandle *outSocketHandle);

T_DjiReturnCode Osal_TcpConnect(T_DjiSocketHandle socketHandle, const char *ipAddr, uint32_t port);

T_DjiReturnCode Osal_TcpSendData(T_DjiSocketHandle socketHandle,
                                 const uint8_t *buf, uint32_t len, uint32_t *realLen);

T_DjiReturnCode Osal_TcpRecvData(T_DjiSocketHandle socketHandle,
                                 uint8_t *buf, uint32_t len, uint32_t *realLen);

#ifdef __cplusplus
}
#endif

#endif // OSAL_SOCKET_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
