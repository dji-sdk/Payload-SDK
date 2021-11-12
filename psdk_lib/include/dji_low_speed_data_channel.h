/**
 ********************************************************************
 * @file    dji_low_speed_data_channel.h
 * @brief   This is the header file for "dji_low_speed_data_channel.c", defining the structure and
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
#ifndef DJI_LOW_SPEED_DATA_CHANNEL_H
#define DJI_LOW_SPEED_DATA_CHANNEL_H

/* Includes ------------------------------------------------------------------*/
#include "dji_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/**
 * @brief Prototype of callback function used to receive data that come from selected channel address.
 * @warning User can not execute blocking style operations or functions in callback function, because that will block
 * root thread, causing problems such as slow system response, payload disconnection or infinite loop.
 * @param data: pointer to data.
 * @param len: length of data.
 * @return Execution result.
 */
typedef T_DjiReturnCode (*DjiLowSpeedDataChannelRecvDataCallback)(const uint8_t *data, uint16_t len);

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Initialize the low speed data channel module.
 * @note The interface initialization needs to be after DjiCore_Init.
 * @return Execution result.
 */
T_DjiReturnCode DjiLowSpeedDataChannel_Init(void);

/**
 * @brief Deinitialize the low speed data channel module.
 * @return Execution result.
 */
T_DjiReturnCode DjiLowSpeedDataChannel_DeInit(void);

/**
 * @brief Send data to selected channel address end via command channel.
 * @warning If actual bandwidth is below limitation, data can be sent to the endpoint directly. If exceeds the limitation,
 * firstly data will be stored to buffer of the flow controller and be sent to endpoint after a period (an integer multiple of
 * 1s, the exact time depends on bandwidth limitation and buffer size). If the buffer is full, data be will discarded. The
 * capacity of flow controller buffer is 512 bytes.
 * @note Must ensure actual bandwidth is less than bandwidth limitation of corresponding channel or stream, please
 * refer to developer documentation or state of channel/stream for details related to bandwidth limitation. User can
 * get state of "sendDataChannel" command channel via DjiLowSpeedDataChannel_GetSendDataState() function. If actual
 * bandwidth exceeds the limitation or busy state is set, the user should stop transmitting data or decrease amount of data
 * to be sent.
 * @note Max size of data package sent to selected channel address end on a physical link of command channel is 128.
 * If the length of data to be sent is greater than 128, data to be sent will be divided into multiple packages to send,
 * and the user will also receive multiple data packages on the selected channel address end.
 * @param channelAddress: the channel address of the low speed channel
 * @param data: pointer to data to be sent.
 * @param len: length of data to be sent, unit: byte.
 * @return Execution result.
 */
T_DjiReturnCode DjiLowSpeedDataChannel_SendData(E_DjiChannelAddress channelAddress, const uint8_t *data,
                                                uint8_t len);

/**
 * @brief Get data transmission state of "sendToOsdk" command channel. User can use the state as base for controlling data
 * transmission between selected channel address and onboard computer.
 * @param channelAddress: the channel address of the low speed channel
 * @param state: pointer to low speed channel state.
 * @return Execution result.
 */
T_DjiReturnCode DjiLowSpeedDataChannel_GetSendDataState(E_DjiChannelAddress channelAddress,
                                                        T_DjiDataChannelState *state);

/**
 * @brief Register callback function used to receive data from selected channel address. After registering this callback
 * function, callback function will be called automatically when system receive data from selected channel address.
 * @param channelAddress: the channel address of the low speed channel
 * @param callback: pointer to callback function.
 * @return Execution result.
 */
T_DjiReturnCode DjiLowSpeedDataChannel_RegRecvDataCallback(E_DjiChannelAddress channelAddress,
                                                           DjiLowSpeedDataChannelRecvDataCallback callback);

#ifdef __cplusplus
}
#endif

#endif // DJI_LOW_SPEED_DATA_CHANNEL_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
