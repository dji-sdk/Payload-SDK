/**
 ********************************************************************
 * @file    psdk_positioning.h
 * @brief   This is the header file for "psdk_positioning.c", defining the structure and
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
#ifndef PSDK_POSITIONING_H
#define PSDK_POSITIONING_H

/* Includes ------------------------------------------------------------------*/
#include "legacy_psdk2.x/psdk_typedef.h"
#include "legacy_psdk2.x/psdk_time_sync.h"
#include "dji_positioning.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/
/**
 * @brief Data structure that describes a positioning event.
 */
typedef T_DjiPositioningEventInfo T_PsdkPositioningEventInfo;

/**
 * @brief Data structure that describes position of a point.
 */
typedef T_DjiPositioningPosition T_PsdkPositioningPosition;

/**
 * @brief Position value standard deviation.
 */
typedef T_DjiPositioningPositionStandardDeviation T_PsdkPositioningPositionStandardDeviation;

/**
 * @brief Position of target point and other details returned by interface of requesting position.
 */
typedef T_DjiPositioningPositionInfo T_PsdkPositioningPositionInfo;

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Initialise positioning module in blocking mode. User should call this function before all other positioning
 * operations, just like setting task index, register callback function and requesting positions.
 * @note Max execution time of this function is slightly larger than 500ms.
 * @note This function has to be called in user task, rather than main() function, and after scheduler being started.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkPositioning_Init(void);

/**
 * @brief Set task index, specifying a sequence number of operations, using to write to mark the file for some post-process
 * flows.
 * @details One operation may be a precise surveying and mapping task or ten exposures of camera. The default task index
 * is 0.
 * @param index: task index.
 */
void PsdkPositioning_SetTaskIndex(uint8_t index);

/**
 * @brief The interface is used to get the position of target points and other information (refer to
 * ::T_PsdkPositioningPositionInfo) based on the timestamp in aircraft time system when some events (positioning events) are
 * triggered. The function uses blocking mode.
 * @details Users can request positions for multiple events (event set) conveniently, such as sync exposure of
 * multiple cameras. User must register callback function used to get the newest PPS triggered timestamp (refer to
 * PsdkPositioning_RegGetNewestPpsTriggerTimeCallback()) before requesting position. When users have set structure
 * parameters of payload and interest points (the points whose position user hope to get, e.g. center point of the camera
 * image sensor), the target points are interest points, otherwise the target points are main gimbal interface center.
 * Users can use position information of gimbal interface center, position offset between gimbal interface center and
 * RTK main antenna, UAV attitude, gimbal structure parameters ,and gimbal attitude to calculate the position of interest
 * points in the payload.
 * @note Max execution time of this function is slightly larger than 600ms.
 * @note All requested timestamp have to be between the time point 2 seconds earlier than the newest synchronized
 * timestamp and the time point 1 seconds earlier than the newest synchronized timestamp.
 * @param eventCount: count of positioning event set specified by eventInfo parameter. Please ensure the count is less
 * than 5.
 * @param eventInfo: pointer to positioning event information array.
 * @param positionInfo: the position of target points. In Matrice 210 RTK V2 and Matrice 300 RTK aircraft system, it is
 * the position of the main gimbal interface always.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkPositioning_GetPositionInformationSync(uint8_t eventCount, T_PsdkPositioningEventInfo *eventInfo,
                                                            T_PsdkPositioningPositionInfo *positionInfo);

#ifdef __cplusplus
}
#endif

#endif //PSDK_POSITIONING_H

/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
