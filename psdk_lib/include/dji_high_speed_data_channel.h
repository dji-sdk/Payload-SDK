
/**
 ********************************************************************
 * @file    dji_high_speed_data_channel.h
 * @brief   This is the header file for "dji_high_speed_data_channel.c", defining the structure and
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
#ifndef DJI_HIGH_SPEED_DATA_CHANNEL_H
#define DJI_HIGH_SPEED_DATA_CHANNEL_H

/* Includes ------------------------------------------------------------------*/
#include "dji_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef struct {
    uint8_t dataStream; /*!< Bandwidth proportion of data stream, range from 0 to 100. */
    uint8_t videoStream; /*!< Bandwidth proportion of video stream, range from 0 to 100. */
    uint8_t downloadStream; /*!< Bandwidth proportion of download stream, range from 0 to 100. */
} T_DjiDataChannelBandwidthProportionOfHighspeedChannel;

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Set bandwidth proportion for the data stream, video stream and download stream of high-speed channel.
 * @details Self-defined bandwidth distribution mechanism allows that user can control bandwidth limitation
 * freely. System multiplies total bandwidth limitation of the high-speed channel by a proportion of the corresponding stream to
 * get realtime bandwidth limitation of the specified stream. Realtime bandwidth limitation of data and video stream can be
 * gotten by PsdkDataTransmission_GetDataStreamState() and PsdkPayloadCamera_GetVideoStreamState() interfaces. Total
 * bandwidth of the high-speed channel is determined by the environment in which the system located and loading of system. If
 * user does not set bandwidth proportion for each stream, the default proportion is 33% (data stream), 33% (video
 * stream) and 34% (download stream).
 * @note The bandwidth proportion will be effective after a while, the max value is 1s. If you want use this interface, should calling
 * PsdkPlatform_RegHalNetworkHandler interface firstly, otherwise this interface will not work.
 * @param bandwidthProportion: bandwidth proportion of each stream of high-speed channel. The Sum of bandwidth proportion
 * must equal 100.
 * @return Execution result.
 */
T_DjiReturnCode DjiHighSpeedDataChannel_SetBandwidthProportion(
    T_DjiDataChannelBandwidthProportionOfHighspeedChannel bandwidthProportion);

/**
 * @brief Get the network remote address for sending data stream.
 * @note The interface is used to get the network remote address for sending data stream. You can get this info for another
 * heterogeneous system to do somethings. This interface should be used after calling PsdkCore_Init function.
 * @param ipAddr: the remote ip address for sending data stream.
 * @param port: the remote port for sending data stream.
 * @return Execution result.
 */
T_DjiReturnCode DjiHighSpeedDataChannel_GetDataStreamRemoteAddress(char *ipAddr, uint16_t *port);

/**
 * @brief Send data to mobile end via data stream of the data channel.
 * @note This function can be used only in Linux operating system.
 * @note Must ensure actual bandwidth is less than bandwidth limitation of corresponding channel or stream, please
 * refer to developer documentation and state of channel/stream for details related to bandwidth limitation. User can
 * get state of "dataStream" channel via PsdkDataTransmission_GetDataStreamState() function. If actual bandwidth
 * exceeds the limitation or busy state is set, the user should stop transmitting data or decrease amount of data to be
 * sent. Otherwise, data may be discarded.
 * @param data: pointer to data to be sent.
 * @param len: length of data to be sent via data stream, and it must be less than or equal to 65000, unit: byte.
 * @return Execution result.
 */
T_DjiReturnCode DjiHighSpeedDataChannel_SendDataStreamData(const uint8_t *data, uint16_t len);

/**
 * @brief Get data transmission state of "dataStream" channel. User can use the state as base for controlling data
 * transmission of data stream. This function exists and can be used only in Linux operation system.
 * @param state: pointer to "dataStream" channel state.
 * @return Execution result.
 */
T_DjiReturnCode DjiHighSpeedDataChannel_GetDataStreamState(T_DjiDataChannelState *state);

#ifdef __cplusplus
}
#endif

#endif // DJI_HIGH_SPEED_DATA_CHANNEL_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
