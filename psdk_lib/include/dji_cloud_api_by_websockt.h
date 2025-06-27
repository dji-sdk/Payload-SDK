/**
 ********************************************************************
 * @file    dji_cloud_api_by_websockt.h
 * @brief   This is the header file for "dji_cloud_api_by_websockt.h", defining the structure and
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
#ifndef DJI_CLOUD_API_BY_WEEBSOCKET_H
#define DJI_CLOUD_API_BY_WEEBSOCKET_H

/* Includes ------------------------------------------------------------------*/
#include "dji_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Send data into cloud api channel over pilot2 websocket.
 * @note This interface should be called only on manifold3 application. It is recommended
 * to send more bytes of data at a time to improve read and write efficiency. Need to determine whether the send is
 * successful according to the return code and the actual sent data length.
 * @param data: pointer to data to be sent.
 * @param len: length of data to be sented to pilot2, unit: byte.
 * @param realLen: pointer to real length of data that already sent.
 * @return Execution result.
 */
T_DjiReturnCode DjiCloudApi_SendDataByWebSocket(uint8_t *data, uint32_t len, uint32_t *realLen);
#ifdef __cplusplus
}
#endif

#endif

/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/

