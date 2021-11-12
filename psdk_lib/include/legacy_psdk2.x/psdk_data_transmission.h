/**
 ********************************************************************
 * @file    psdk_data_transmission.h
 * @brief   This is the header file for "psdk_data_transmission.c", defining the structure and
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
#ifndef PSDK_DATA_TRANSMISSION_H
#define PSDK_DATA_TRANSMISSION_H

/* Includes ------------------------------------------------------------------*/
#include "legacy_psdk2.x/psdk_typedef.h"
#include "dji_low_speed_data_channel.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/
/**
 * @brief Prototype of callback function used to receive data that come from mobile end.
 * @warning User can not execute blocking style operations or functions in callback function, because that will block PSDK
 * root thread, causing problems such as slow system response, payload disconnection or infinite loop.
 * @param data: pointer to data.
 * @param len: length of data.
 * @return Execution result.
 */
typedef DjiLowSpeedDataChannelRecvDataCallback ReceiveDataFromMobileCallback;

/**
 * @brief Prototype of callback function used to receive data that come from onboard computer.
 * @warning User can not execute blocking style operations or functions in callback function, because that will block PSDK
 * root thread, causing problems such as slow system response, payload disconnection or infinite loop.
 * @param data: pointer to data.
 * @param len: length of data.
 * @return Execution result.
 */
typedef DjiLowSpeedDataChannelRecvDataCallback ReceiveDataFromOsdkCallback;

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Initialise data transmission module in block mode. Should call this function before sending data to mobile
 * end/onboard computer or receiving data.
 * @note Max execution time of this function is slightly larger than 500ms.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkDataTransmission_Init(void);

/**
 * @brief Deinitialize data transmission module. User can call this function to release all resources that data
 * transmission module have occupied from the system and restore the state of internal variables when functions of data
 * transmission module will no longer be used or exit program.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkDataTransmission_DeInit(void);

/**
 * @brief Register callback function used to receive data from mobile end. After registering this callback function,
 * callback function will be called automatically when system receive data from mobile end.
 * @param callback: pointer to callback function.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkDataTransmission_RegReceiveDataFromMobileCallback(ReceiveDataFromMobileCallback callback);

/**
 * @brief Register callback function used to receive data from onboard computer. After registering this callback
 * function, callback function will be called automatically when system receive data from onboard computer.
 * @param callback: pointer to callback function.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkDataTransmission_RegReceiveDataFromOsdkCallback(ReceiveDataFromOsdkCallback callback);

/**
 * @brief Send data to mobile end via command channel.
 * @warning If actual bandwidth is below limitation, data can be sent to the endpoint directly. If exceeds the limitation,
 * firstly data will be stored to buffer of the flow controller and be sent to endpoint after a period (an integer multiple of
 * 1s, the exact time depends on bandwidth limitation and buffer size). If the buffer is full, data be will discarded. The
 * capacity of flow controller buffer is 512 bytes.
 * @note Must ensure actual bandwidth is less than bandwidth limitation of corresponding channel or stream, please
 * refer to developer documentation or state of channel/stream for details related to bandwidth limitation. User can
 * get state of "sendToMobile" command channel via PsdkDataTransmission_GetSendToMobileState() function. If actual
 * bandwidth exceeds the limitation or busy state is set, the user should stop transmitting data or decrease amount of data
 * to be sent.
 * @note Max size of data package sent to mobile end on a physical link of command channel is 128. If the length of data to
 * be sent is greater than 128, data to be sent will be divided into multiple packages to send, and the user will also
 * receive multiple data packages on the mobile end.
 * @param data: pointer to data to be sent.
 * @param len: length of data to be sent, unit: byte.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkDataTransmission_SendDataToMobile(const uint8_t *data, uint8_t len);

/**
 * @brief Send data to the onboard computer via command channel.
 * @warning If actual bandwidth is below limitation, data can be sent to the endpoint directly. If exceeds the limitation,
 * firstly data will be stored to buffer of the flow controller and be sent to endpoint after a period (an integer multiple of
 * 1s, the exact time depends on bandwidth limitation and buffer size). If the buffer is full, data will be discarded. The
 * capacity of flow controller buffer is 512 bytes.
 * @note Must ensure actual bandwidth is less than bandwidth limitation of corresponding channel or stream, please
 * refer to developer documentation or state of channel/stream for details related to bandwidth limitation. User can
 * get state of "sendToOsdk" command channel via PsdkDataTransmission_GetSendToOsdkState() function. If actual bandwidth
 * exceeds the limitation or busy state is set, the user should stop transmitting data or decrease amount of data to be
 * sent.
 * @note Max size of data package sent to an onboard computer on the physical link of command channel is 255. If the length of data
 * to be sent is greater than 255, data to be sent will be divided into multiple packages to send, and the user will also
 * receive multiple data packages on the onboard computer.
 * @param data: pointer to data to be sent.
 * @param len: length of data to be sent, unit: byte.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkDataTransmission_SendDataToOsdk(const uint8_t *data, uint8_t len);

/**
 * @brief Get data transmission state of "sendToMobile" command channel. User can use the state as base for controlling
 * data transmission between payload and mobile end.
 * @param state: pointer to "sendToMobile" command channel state.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkDataTransmission_GetSendToMobileState(T_PsdkDataChannelState *state);

/**
 * @brief Get data transmission state of "sendToOsdk" command channel. User can use the state as base for controlling data
 * transmission between payload and onboard computer.
 * @param state: pointer to "sendToOsdk" command channel state.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkDataTransmission_GetSendToOsdkState(T_PsdkDataChannelState *state);

/**
 * @brief Get the network remote address for sending data stream.
 * @note The interface is used to get the network remote address for sending data stream. You can get this info for another
 * heterogeneous system to do somethings. This interface should be used after calling PsdkCore_Init function.
 * @param ipAddr: the remote ip address for sending data stream.
 * @param port: the remote port for sending data stream.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkDataTransmission_GetDataStreamRemoteAddress(char *ipAddr, uint16_t *port);

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
T_PsdkReturnCode PsdkDataTransmission_SendDataStream(const uint8_t *data, uint16_t len);

/**
 * @brief Get data transmission state of "dataStream" channel. User can use the state as base for controlling data
 * transmission of data stream. This function exists and can be used only in Linux operation system.
 * @param state: pointer to "dataStream" channel state.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkDataTransmission_GetDataStreamState(T_PsdkDataChannelState *state);

/**
 * @brief Get the network remote address for sending data stream.
 * @note The interface is used to get the network remote address for sending data stream. You can get this info for another
 * heterogeneous system to do somethings. This interface should be used after calling PsdkCore_Init function.
 * @param ipAddr: the remote ip address for sending data stream.
 * @param port: the remote port for sending data stream.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkDataTransmission_GetDataStreamRemoteAddress(char *ipAddr, uint16_t *port);

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
T_PsdkReturnCode PsdkDataTransmission_SendDataStream(const uint8_t *data, uint16_t len);

/**
 * @brief Get data transmission state of "dataStream" channel. User can use the state as base for controlling data
 * transmission of data stream. This function exists and can be used only in Linux operation system.
 * @param state: pointer to "dataStream" channel state.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkDataTransmission_GetDataStreamState(T_PsdkDataChannelState *state);


#ifdef __cplusplus
}
#endif

#endif // PSDK_DATA_TRANSMISSION_H

/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
