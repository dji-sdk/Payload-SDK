/**
 ********************************************************************
 * @file    psdk_mop_channel.h
 * @brief   This is the header file for "psdk_mop_channel.c", defining the structure and
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
#ifndef PSDK_MOP_CHANNEL_H
#define PSDK_MOP_CHANNEL_H

/* Includes ------------------------------------------------------------------*/
#include "legacy_psdk2.x/psdk_typedef.h"
#include "dji_mop_channel.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/**
 * @brief Mop channel handle.
 */
typedef T_DjiMopChannelHandle T_PsdkMopChannelHandle;

/**
 * @brief Mop channel transmission type.
 */
typedef E_DjiMopChannelTransType E_PsdkMopChannelTransType;
#define PSDK_MOP_CHANNEL_TRANS_RELIABLE      DJI_MOP_CHANNEL_TRANS_RELIABLE
#define PSDK_MOP_CHANNEL_TRANS_UNRELIABLE    DJI_MOP_CHANNEL_TRANS_UNRELIABLE

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Initialize the mop channel for operation about mop channel handle later.
 * @note This interface needs to be called before calling other mop channel interfaces. Please make sure the network port
 * is connected properly before calling this interface. Mop channel feature currently only supports linux platform.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkMopChannel_Init(void);

/**
 * @brief Create the mop channel handle by specified transmission type.
 * @note After calling this interface, you should follow below cases:
 * 1. When mop channel handle created successfully, you need bind channel id to this created mop channel handle and
 * accept other client device connections, such as MSDK or OSDK device;
 * 2. When mop channel accept successfully, you can use send or receive data interface to write or read data by accepted
 * output channel handle;
 * 3. When the mop channel handle do not used, you can use close or destroy interface to release the resource;
 * @param channelHandle: pointer to the created mop channel.
 * @param transType: the transmission type for mop channel ::E_PsdkMopChannelTransType.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkMopChannel_Create(T_PsdkMopChannelHandle *channelHandle, E_PsdkMopChannelTransType transType);

/**
 * @brief Destroy the created mop channel and release the resource that referenced by the channel handle.
 * @note After calling this interface, we can not do any operation about this channel handle expect create channel handle.
 * @param channelHandle: pointer to the created mop channel.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkMopChannel_Destroy(T_PsdkMopChannelHandle channelHandle);

/**
 * @brief Bind the channel id to the created mop channel handle.
 * @note When mop channel handle created successfully, you can bind channel id to this created mop handle and calling
 * interface ::PsdkMopChannel_Accept, then other client device can use this binded channel id to connect created mop handle.
 * @param channelHandle: pointer to the created mop channel handle.
 * @param channelId: the channel id of mop handle for accepting client device connection.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkMopChannel_Bind(T_PsdkMopChannelHandle channelHandle,
                                     uint16_t channelId);

/**
 * @brief Accept the connection by binded channel id for created mop channel.
 * @note The mop accept channel extracts the connection request on the queue of pennding connections for listening channel
 * handle ::channelHandle. Create a new connected channel handle and return to user ::outChannelHandle by referrng to a new
 * connection. Payload can be a server that allow multiple client connections by binded channel id.
 * @param channelHandle: pointer to the created mop channel.
 * @param outChannelHandle: pointer to the accepted output mop channel.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkMopChannel_Accept(T_PsdkMopChannelHandle channelHandle,
                                       T_PsdkMopChannelHandle *outChannelHandle);

/**
 * @brief Close the created mop channel.
 * @note After calling this interface, we can not do any operation about this channel handle expect destroy channel handle.
 * @param channelHandle: pointer to the created mop channel.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkMopChannel_Close(T_PsdkMopChannelHandle channelHandle);

/**
 * @brief Send data by the accepted output mop channel.
 * @note This interface should be called after successfully calling the interface ::PsdkMopChannel_Accept. It is recommended
 * to send more bytes of data at a time to improve read and write efficiency. Need to determine whether the send is
 * successful according to the return code and the actual sent data length.
 * @param channelHandle: pointer to accepted output mop handle ::outChannelHandle by calling interface ::PsdkMopChannel_Accept.
 * @param data: pointer to data to be sent.
 * @param len: length of data to be sent via accepted output mop handle, unit: byte.
 * @param realLen: pointer to real length of data that already sent.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkMopChannel_SendData(T_PsdkMopChannelHandle channelHandle,
                                         uint8_t *data,
                                         uint32_t len,
                                         uint32_t *realLen);

/**
 * @brief Receive data from the accepted output mop channel.
 * @note This interface should be called after successfully calling the interface ::PsdkMopChannel_Accept. It is recommended
 * to receive more bytes of data at a time to improve read and write efficiency. Need to determine whether the receive
 * is successful according to the return code and the actual received data length.
 * @param channelHandle: pointer to accepted output mop handle ::outChannelHandle by calling interface ::PsdkMopChannel_Accept.
 * @param data: pointer to data to store the received data.
 * @param len: length of data to be received via accepted output mop handle, unit: byte.
 * @param realLen: pointer to real length of data that already received.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkMopChannel_RecvData(T_PsdkMopChannelHandle channelHandle,
                                         uint8_t *data,
                                         uint32_t len,
                                         uint32_t *realLen);
#ifdef __cplusplus
}
#endif

#endif // PSDK_MOP_CHANNEL_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
