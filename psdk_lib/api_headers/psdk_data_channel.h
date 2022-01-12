/**
 ********************************************************************
 * @file    psdk_data_channel.h
 * @brief   This is the header file for "psdk_data_channel.c", defining the structure and
 * (exported) function prototypes.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef PSDK_DATA_CHANNEL_H
#define PSDK_DATA_CHANNEL_H

/* Includes ------------------------------------------------------------------*/
#include "psdk_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/
/**
 * @brief Data channel state.
 */
typedef struct {
    /*! Realtime bandwidth limitation, varying with link status of aircraft system for some channels, such as data
     * stream, video stream and download stream. Must ensure actual bandwidth of data transmission is less than
     * realtime bandwidth limitation, unit: byte/s. */
    int32_t realtimeBandwidthLimit;

    /*! Realtime actual transmission bandwidth of data transmission channel calculated before flow controller, unit: byte/s. */
    int32_t realtimeBandwidthBeforeFlowController;

    /*! Realtime actual transmission bandwidth of data transmission channel calculated after flow controller, unit:
     * byte/s. If specified channel without flow controller. the value is equal to
     * ::realtimeBandwidthBeforeFlowController. */
    int32_t realtimeBandwidthAfterFlowController;

    /*! State specified whether the channel is busy or not. When data can not be sent to the endpoint directly, instead be
     * sent to buffer of flow controller or discarded, the busy state will be set. At this time, the user should stop
     * transmitting data or decrease the amount of data to be transmitted via this channel. When data bandwidth restore to
     * normal, this state will be clear again. */
    bool busyState;
} T_PsdkDataChannelState;

/**
 * @brief Percentage of bandwidth proportion of high-speed channel.
 */
typedef struct {
    uint8_t dataStream; /*!< Bandwidth proportion of data stream, range from 0 to 100. */
    uint8_t videoStream; /*!< Bandwidth proportion of video stream, range from 0 to 100. */
    uint8_t downloadStream; /*!< Bandwidth proportion of download stream, range from 0 to 100. */
} T_PsdkDataChannelBandwidthProportionOfHighspeedChannel;

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
T_PsdkReturnCode PsdkDataChannel_SetBandwidthProportionForHighspeedChannel(
    T_PsdkDataChannelBandwidthProportionOfHighspeedChannel bandwidthProportion);

#ifdef __cplusplus
}
#endif

#endif // PSDK_DATA_CHANNEL_H

/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
