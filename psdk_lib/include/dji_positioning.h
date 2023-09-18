/**
 ********************************************************************
 * @file    dji_positioning.h
 * @brief   This is the header file for "dji_positioning.c", defining the structure and
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
#ifndef DJI_POSITIONING_H
#define DJI_POSITIONING_H

/* Includes ------------------------------------------------------------------*/
#include "dji_typedef.h"
#include "dji_time_sync.h"
#include "dji_fc_subscription.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/**
 * @brief Positioning rtcm data type.
 */
typedef enum {
    DJI_POSITIONING_RTCM_DATA_TYPE_RTK_ON_AIRCRAFT = 0,
    DJI_POSITIONING_RTCM_DATA_TYPE_RTK_BASE_STATION = 1,
} E_DjiPositioningRtcmDataType;

/**
 * @brief Data structure that describes a positioning event.
 */
typedef struct {
    /*! Index of event set in which positioning event is located. The item will be written to mark file in aircraft for
     * some post-precess work. If not needed, fill in 0. */
    uint16_t eventSetIndex;
    /*! Index of target point in payload whose position user is requesting. The item will be written to mark file in
     * aircraft for some post-precess work. If not needed, fill in 0. */
    uint8_t targetPointIndex;
    /*! Timestamp in aircraft time system when the positioning event occur. Users should transfer time in local time
     * system to time in aircraft time system by DjiTimeSync_TransferToAircraftTime() interface in time
     * synchronization module. */
    T_DjiTimeSyncAircraftTime eventTime;
} T_DjiPositioningEventInfo;

/**
 * @brief Data structure that describes position of a point.
 */
typedef struct {
    dji_f64_t longitude; /*!< Specifies longitude, unit: degree. */
    dji_f64_t latitude; /*!< Specifies latitude, unit: degree. */
    dji_f64_t height; /*!< Specifies height above sea level, unit: m. */
} T_DjiPositioningPosition;

/**
 * @brief Position value standard deviation.
 */
typedef struct {
    dji_f32_t longitude; /*!< Specifies longitude standard deviation, unit: degree. */
    dji_f32_t latitude; /*!< Specifies latitude standard deviation， unit: degree. */
    dji_f32_t height; /*!< Specifies height standard deviation， unit: m. */
} T_DjiPositioningPositionStandardDeviation;

/**
 * @brief Position of target point and other details returned by interface of requesting position.
 */
typedef struct {
    E_DjiFcSubscriptionPositionSolutionProperty positionSolutionProperty; /*!< Property of position solution. */
    T_DjiAttitude3d uavAttitude; /*!< Specifies UAV attitude, unit: degree. */
    T_DjiVector3d offsetBetweenMainAntennaAndTargetPoint; /*!< Specifies position offset from RTK main antenna to target points in NED coordinate system, unit: mm. */
    T_DjiPositioningPosition targetPointPosition; /*!< Specifies position of target points in GROUND coordinate system. */
    T_DjiPositioningPositionStandardDeviation targetPointPositionStandardDeviation; /*!< Specifies position standard deviation of target points. */
} T_DjiPositioningPositionInfo;

/**
 * @brief Prototype of callback function used to receive the rtk rtcm raw data.
 * @param index: the index of rtcm data package.
 * @param data: pointer to receive rtk rtcm data from aircraft.
 * @param dataLen: receive data length of rtcm data from aircraft.
 * @return Execution result.
 */
typedef T_DjiReturnCode (*DjiReceiveRtkRtcmDataCallback)(uint8_t index, const uint8_t *data, uint16_t dataLen);

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Initialise positioning module in blocking mode. User should call this function before all other positioning
 * operations, just like setting task index, register callback function and requesting positions.
 * @note Max execution time of this function is slightly larger than 500ms.
 * @note This function has to be called in user task, rather than main() function, and after scheduler being started.
 * @return Execution result.
 */
T_DjiReturnCode DjiPositioning_Init(void);

/**
 * @brief Set task index, specifying a sequence number of operations, using to write to mark the file for some post-process
 * flows.
 * @details One operation may be a precise surveying and mapping task or ten exposures of camera. The default task index
 * is 0.
 * @param index: task index.
 */
void DjiPositioning_SetTaskIndex(uint8_t index);

/**
 * @brief The interface is used to get the position of target points and other information (refer to
 * ::T_DjiPositioningPositionInfo) based on the timestamp in aircraft time system when some events (positioning events) are
 * triggered. The function uses blocking mode.
 * @details Users can request positions for multiple events (event set) conveniently, such as sync exposure of
 * multiple cameras. User must register callback function used to get the newest PPS triggered timestamp (refer to
 * DjiPositioning_RegGetNewestPpsTriggerTimeCallback()) before requesting position. When users have set structure
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
T_DjiReturnCode DjiPositioning_GetPositionInformationSync(uint8_t eventCount, T_DjiPositioningEventInfo *eventInfo,
                                                          T_DjiPositioningPositionInfo *positionInfo);

/**
 * @brief Register callback function used to receive rtk rtcm raw data by data type.
 * @note The callback function will be called after registering. The call frequency depends on the number of satellites.
 * @param callback: pointer to the callback function.
 * @return Execution result.
 */
T_DjiReturnCode DjiPositioning_RegReceiveRtcmDataCallback(E_DjiPositioningRtcmDataType dataType,
                                                          DjiReceiveRtkRtcmDataCallback callback);

#ifdef __cplusplus
}
#endif

#endif //DJI_POSITIONING_H

/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
