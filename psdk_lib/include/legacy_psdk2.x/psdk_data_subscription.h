/**
 ********************************************************************
 * @file    psdk_data_subscription.h
 * @brief   This is the header file for "psdk_data_subscription.c", defining the structure and
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
#ifndef PSDK_DATA_SUBSCRIPTION_H
#define PSDK_DATA_SUBSCRIPTION_H

/* Includes ------------------------------------------------------------------*/
#include "legacy_psdk2.x/psdk_typedef.h"
#include "dji_fc_subscription.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/
/**
 * @brief Name of topics that can be subscribed. Topic is minimum data item subscribed.
 */
typedef E_DjiFcSubscriptionTopic E_PsdkDataSubscriptionTopicName;
/*!
 * @brief Quaternion of aircraft topic name. Quaternion topic provides aircraft body frame (FRD) to ground frame
 * (NED) rotation. Please refer to ::T_PsdkDataSubscriptionQuaternion for information about data structure.
 * @details The DJI quaternion follows Hamilton convention (q0 = w, q1 = x, q2 = y, q3 = z).
 * | Angle        | Unit | Accuracy   | Notes                                           |
   |--------------|------|------------|-------------------------------------------------|
   | pitch, roll  | deg  | <1         | in NON-AHRS mode                                |
   | yaw          | deg  | <3         | in well-calibrated compass with fine aligned    |
   | yaw with rtk | deg  | around 1.2 | in RTK heading fixed mode with 1 meter baseline |
 */
#define PSDK_DATA_SUBSCRIPTION_TOPIC_QUATERNION  DJI_FC_SUBSCRIPTION_TOPIC_QUATERNION

/*!
 * @brief Velocity of aircraft topic name. Velocity topic provides aircraft's velocity in a ground-fixed NEU frame.
 * Please refer to ::T_PsdkDataSubscriptionVelocity for information about data structure.
 * @warning Please note that this data is not in a conventional right-handed frame of reference.
 * @details This velocity data is a fusion output from the aircraft. Original output is in a right-handed NED frame, but the
 * sign of the Z-axis velocity is flipped before publishing to this topic. So if you are looking to get velocity
 * in an NED frame, simply flip the sign of the z-axis value. Beyond that, you can convert using rotations to
 * any right-handed frame of reference.
 * | Axis     | Unit | Accuracy                                                                                    |
   |----------|------|---------------------------------------------------------------------------------------------|
   | vgx, vgy | m/s  | Around 5cm/s for GNSS navigation. Around 3cm/s with VO at 1 meter height                    |
   | vgz      | m/s  | 10cm/s only with barometer in steady air. 3cm/s with VO at 1 meter height with 8cm baseline |
 */
#define PSDK_DATA_SUBSCRIPTION_TOPIC_VELOCITY  DJI_FC_SUBSCRIPTION_TOPIC_VELOCITY

/*!
 * @brief Fused altitude of aircraft topic name. Fused altitude topic provides aircraft's fused altitude from sea
 * level. Please refer to ::T_PsdkDataSubscriptionAltitudeFused for information about data structure.
 */
#define PSDK_DATA_SUBSCRIPTION_TOPIC_ALTITUDE_FUSED  DJI_FC_SUBSCRIPTION_TOPIC_ALTITUDE_FUSED

/*!
 * @brief Relative height above ground of aircraft topic name. Please refer to
 * ::T_PsdkDataSubscriptionHeightRelative for information about data structure.
 * @details This data is a fusion output from aircraft. The height is a direct estimate of the closest large object
 * below the aircraft's ultrasonic sensors.
 * @warning This topic does not come with a 'valid' flag - so if the aircraft is too far from an object for the
 * ultrasonic sensors/VO to provide any meaningful data, the values will latch and there is no way for user to
 * determine if the data is valid or not. Please use with caution.
 */
#define PSDK_DATA_SUBSCRIPTION_TOPIC_HEIGHT_RELATIVE  DJI_FC_SUBSCRIPTION_TOPIC_HEIGHT_RELATIVE

/*!
 * Fused position of aircraft topic name. Please refer to ::T_PsdkDataSubscriptionPositionFused for information
 * about data structure.
 * @warning Please note that if GPS signal is weak (low visibleSatelliteNumber, see below), the
 * latitude/longitude values won't be updated but the altitude might still be. There is currently no way to know if
 * the lat/lon update is healthy.
 * @details The most important component of this topic is the T_PsdkDataSubscriptionPositionFused::visibleSatelliteNumber.
 * Use this to track your GPS satellite coverage and build some heuristics for when you might expect to lose GPS updates.
 *   | Axis | Unit | Position Sensor | Accuracy                                         |
     |------|------|-----------------|--------------------------------------------------|
     | x, y | m    | GPS             | <3m with open sky without multipath              |
     | z    | m    | GPS             | <5m with open sky without multipath              |
 */
#define PSDK_DATA_SUBSCRIPTION_TOPIC_POSITION_FUSED  DJI_FC_SUBSCRIPTION_TOPIC_POSITION_FUSED

/*!
 * @brief GPS date topic name. Please refer to ::T_PsdkDataSubscriptionGpsDate for information about data structure.
 */
#define PSDK_DATA_SUBSCRIPTION_TOPIC_GPS_DATE  DJI_FC_SUBSCRIPTION_TOPIC_GPS_DATE

/*!
 * @brief GPS time topic name. Please refer to ::T_PsdkDataSubscriptionGpsTime for information about data structure.
 */
#define PSDK_DATA_SUBSCRIPTION_TOPIC_GPS_TIME  DJI_FC_SUBSCRIPTION_TOPIC_GPS_TIME

/*!
 * @brief GPS position topic name. Please refer to ::T_PsdkDataSubscriptionGpsPosition for information about data structure.
 * @details
 *   | Axis | Accuracy                                         |
     |------|--------------------------------------------------|
     | x, y | <3m with open sky without multipath              |
     | z    | <5m with open sky without multipath              |
 */
#define PSDK_DATA_SUBSCRIPTION_TOPIC_GPS_POSITION  DJI_FC_SUBSCRIPTION_TOPIC_GPS_POSITION

/*!
 * @brief GPS velocity topic name. Please refer to ::T_PsdkDataSubscriptionGpsVelocity for information about data structure.
 */
#define PSDK_DATA_SUBSCRIPTION_TOPIC_GPS_VELOCITY  DJI_FC_SUBSCRIPTION_TOPIC_GPS_VELOCITY

/*!
 * @brief GPS details topic name. GPS details topic provides GPS state and other detail information. Please refer
 * to ::T_PsdkDataSubscriptionGpsDetail for information about data structure.
 */
#define PSDK_DATA_SUBSCRIPTION_TOPIC_GPS_DETAILS  DJI_FC_SUBSCRIPTION_TOPIC_GPS_DETAILS

/*!
 * @brief GPS signal level topic name. This topic provides a measure of the quality of GPS signal. Please refer to
 * ::T_PsdkDataSubscriptionGpsSignalLevel for information about data structure.
 */
#define PSDK_DATA_SUBSCRIPTION_TOPIC_GPS_SIGNAL_LEVEL  DJI_FC_SUBSCRIPTION_TOPIC_GPS_SIGNAL_LEVEL

/*!
 * @brief RTK position topic name. Please refer to ::T_PsdkDataSubscriptionRtkPosition for information about data structure.
 * @details
 *   | Axis | Accuracy                                         |
     |------|--------------------------------------------------|
     | x, y | ~2cm with fine alignment and fix condition       |
     | z    | ~3cm with fine alignment and fix condition       |
 */
#define PSDK_DATA_SUBSCRIPTION_TOPIC_RTK_POSITION  DJI_FC_SUBSCRIPTION_TOPIC_RTK_POSITION

/*!
 * @brief RTK velocity topic name. Please refer to ::T_PsdkDataSubscriptionRtkVelocity for information about data structure.
 */
#define PSDK_DATA_SUBSCRIPTION_TOPIC_RTK_VELOCITY  DJI_FC_SUBSCRIPTION_TOPIC_RTK_VELOCITY

/*!
 * @brief RTK yaw topic name. Please refer to ::T_PsdkDataSubscriptionRtkYaw for information about data structure.
 * @details The RTK yaw will provide the vector from ANT1 to ANT2 as configured in DJI Assistant 2. This
 * means that the value of RTK yaw will be 90deg offset from the yaw of the aircraft.
 */
#define PSDK_DATA_SUBSCRIPTION_TOPIC_RTK_YAW  DJI_FC_SUBSCRIPTION_TOPIC_RTK_YAW

/*!
 * @brief RTK position information topic name. RTK position information topic provides a state of RTK position
 * solution. Please refer to ::T_PsdkDataSubscriptionRtkPositionInfo for information about data structure.
 */
#define PSDK_DATA_SUBSCRIPTION_TOPIC_RTK_POSITION_INFO  DJI_FC_SUBSCRIPTION_TOPIC_RTK_POSITION_INFO

/*!
 * @brief RTK yaw topic name. RTK yaw information topic provides a state of RTK yaw solution. Please refer to
 * ::T_PsdkDataSubscriptionRtkYawInfo for information about data structure.
 */
#define PSDK_DATA_SUBSCRIPTION_TOPIC_RTK_YAW_INFO  DJI_FC_SUBSCRIPTION_TOPIC_RTK_YAW_INFO

/*!
 * @brief Flight status topic name. Please refer to ::T_PsdkDataSubscriptionFlightStatus for information about data structure.
 */
#define PSDK_DATA_SUBSCRIPTION_TOPIC_STATUS_FLIGHT  DJI_FC_SUBSCRIPTION_TOPIC_STATUS_FLIGHT

/*!
 * @brief Battery information topic name. Please refer to ::T_PsdkDataSubscriptionBatteryInfo for information about data structure.
 */
#define PSDK_DATA_SUBSCRIPTION_TOPIC_BATTERY_INFO  DJI_FC_SUBSCRIPTION_TOPIC_BATTERY_INFO

/*! Total number of topics that can be subscribed. */
#define PSDK_DATA_SUBSCRIPTION_TOPIC_TOTAL_NUMBER  DJI_FC_SUBSCRIPTION_TOPIC_TOTAL_NUMBER


/**
 * @brief Health state of data subscribed.
 */
typedef E_DjiFcSubscriptionDataHealthFlag E_PsdkDataSubscriptionDataHealthFlag;
#define PSDK_DATA_SUBSCRIPTION_DATA_NOT_HEALTH  DJI_FC_SUBSCRIPTION_DATA_NOT_HEALTH /*!< Data subscribed is healthy and can be used. */
#define PSDK_DATA_SUBSCRIPTION_DATA_HEALTH      DJI_FC_SUBSCRIPTION_DATA_HEALTH /*!< Data subscribed is not healthy and recommend not to use it. */

/**
 * @brief Position solution property.
 */
typedef E_DjiFcSubscriptionPositionSolutionProperty E_PsdkDataSubscriptionPositionSolutionProperty;
#define PSDK_DATA_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_NOT_AVAILABLE                                 DJI_FC_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_NOT_AVAILABLE /*!< Position solution is not available. */
#define PSDK_DATA_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_FIX_POSITION                                  DJI_FC_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_FIX_POSITION /*!< Position has been fixed by the FIX POSITION command. */
#define PSDK_DATA_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_FIX_HEIGHT_AUTO                               DJI_FC_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_FIX_HEIGHT_AUTO /*!< Position has been fixed by the FIX HEIGHT/AUTO command. */
#define PSDK_DATA_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_INSTANTANEOUS_DOPPLER_COMPUTE_VELOCITY        DJI_FC_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_INSTANTANEOUS_DOPPLER_COMPUTE_VELOCITY /*!< Velocity computed using instantaneous Doppler. */
#define PSDK_DATA_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_SINGLE_PNT_SOLUTION                           DJI_FC_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_SINGLE_PNT_SOLUTION /*!< Single point position solution. */
#define PSDK_DATA_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_PSEUDORANGE_DIFFERENTIAL_SOLUTION             DJI_FC_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_PSEUDORANGE_DIFFERENTIAL_SOLUTION /*!< Pseudorange differential solution. */
#define PSDK_DATA_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_SBAS_CORRECTION_CALCULATED                    DJI_FC_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_SBAS_CORRECTION_CALCULATED /*!< Solution calculated using corrections from an SBAS. */
#define PSDK_DATA_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_KALMAN_FILTER_WITHOUT_OBSERVATION_PROPAGATED  DJI_FC_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_KALMAN_FILTER_WITHOUT_OBSERVATION_PROPAGATED /*!< Propagated by a Kalman filter without new observations. */
#define PSDK_DATA_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_OMNISTAR_VBS_POSITION                         DJI_FC_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_OMNISTAR_VBS_POSITION /*!< OmniSTAR VBS position (L1 sub-metre). */
#define PSDK_DATA_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_FLOAT_L1_AMBIGUITY                            DJI_FC_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_FLOAT_L1_AMBIGUITY /*!< Floating L1 ambiguity solution. */
#define PSDK_DATA_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_FLOAT_IONOSPHERIC_FREE_AMBIGUITY              DJI_FC_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_FLOAT_IONOSPHERIC_FREE_AMBIGUITY /*!< Floating ionospheric-free ambiguity solution. */
#define PSDK_DATA_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_FLOAT_SOLUTION                                DJI_FC_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_FLOAT_SOLUTION /*!< Float position solution. */
#define PSDK_DATA_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_L1_AMBIGUITY_INT                              DJI_FC_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_L1_AMBIGUITY_INT /*!< Integer L1 ambiguity solution. */
#define PSDK_DATA_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_WIDE_LANE_AMBIGUITY_INT                       DJI_FC_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_WIDE_LANE_AMBIGUITY_INT /*!< Integer wide-lane ambiguity solution. */
#define PSDK_DATA_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_NARROW_INT                                    DJI_FC_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_NARROW_INT /*!< Narrow fixed point position solution. */

/**
 * @brief GPS fix state.
 */
typedef E_DjiFcSubscriptionGpsFixState E_PsdkDataSubscriptionGpsFixState;
#define PSDK_DATA_SUBSCRIPTION_GPS_FIX_STATE_NO_FIX                   DJI_FC_SUBSCRIPTION_GPS_FIX_STATE_NO_FIX /*!< GPS position has not been fixed. */
#define PSDK_DATA_SUBSCRIPTION_GPS_FIX_STATE_DEAD_RECKONING_ONLY      DJI_FC_SUBSCRIPTION_GPS_FIX_STATE_DEAD_RECKONING_ONLY /*!< GPS position is dead reckoned only. */
#define PSDK_DATA_SUBSCRIPTION_GPS_FIX_STATE_2D_FIX                   DJI_FC_SUBSCRIPTION_GPS_FIX_STATE_2D_FIX /*!< The horizontal position with latitude/longitude (or northing/easting or X/Y) is fixed. */
#define PSDK_DATA_SUBSCRIPTION_GPS_FIX_STATE_3D_FIX                   DJI_FC_SUBSCRIPTION_GPS_FIX_STATE_3D_FIX /*!< The horizontal and vertical position with latitude/longitude/altitude (northing/easting/altitude or X/Y/Z) is fixed. */
#define PSDK_DATA_SUBSCRIPTION_GPS_FIX_STATE_GPS_PLUS_DEAD_RECKONING  DJI_FC_SUBSCRIPTION_GPS_FIX_STATE_GPS_PLUS_DEAD_RECKONING /*!< Position is calculated by GPS and combined with dead reckoning. */
#define PSDK_DATA_SUBSCRIPTION_GPS_FIX_STATE_TIME_ONLY_FIX            DJI_FC_SUBSCRIPTION_GPS_FIX_STATE_TIME_ONLY_FIX /*!< Only time is fixed. */
/**
 * @brief Flight status of aircraft.
 */
typedef E_DjiFcSubscriptionFlightStatus E_PsdkDataSubscriptionFlightStatus;
#define PSDK_DATA_SUBSCRIPTION_FLIGHT_STATUS_STOPED     DJI_FC_SUBSCRIPTION_FLIGHT_STATUS_STOPED /*!< Aircraft is on ground and motors are still. */
#define PSDK_DATA_SUBSCRIPTION_FLIGHT_STATUS_ON_GROUND  DJI_FC_SUBSCRIPTION_FLIGHT_STATUS_ON_GROUND/*!< Aircraft is on ground but motors are rotating. */
#define PSDK_DATA_SUBSCRIPTION_FLIGHT_STATUS_IN_AIR     DJI_FC_SUBSCRIPTION_FLIGHT_STATUS_IN_AIR /*!< Aircraft is in air. */

#pragma pack(1)

/**
 * @brief Timestamp data structure.
 */
typedef T_DjiDataTimestamp T_PsdkDataSubscriptiontTimestamp;

/**
 * @brief Quaternion topic data structure.
 */
typedef T_DjiFcSubscriptionQuaternion T_PsdkDataSubscriptionQuaternion;

/**
 * @brief Velocity topic data structure.
 */
typedef T_DjiFcSubscriptionVelocity T_PsdkDataSubscriptionVelocity;

/**
 * @brief Fused altitude topic data structure, unit: m.
 */
typedef T_DjiFcSubscriptionAltitudeFused T_PsdkDataSubscriptionAltitudeFused;

/**
 * @brief Relative height above ground topic data structure, unit: m.
 */
typedef T_DjiFcSubscriptionHeightRelative T_PsdkDataSubscriptionHeightRelative;

/**
 * @brief Fused position topic data structure.
 */
typedef T_DjiFcSubscriptionPositionFused T_PsdkDataSubscriptionPositionFused;

/**
 * @brief GPS date topic data structure, format: yyyymmdd.
 */
typedef T_DjiFcSubscriptionGpsDate T_PsdkDataSubscriptionGpsDate;

/**
 * @brief GPS time topic data structure, format: hhmmss.
 */
typedef T_DjiFcSubscriptionGpsTime T_PsdkDataSubscriptionGpsTime;

/**
 * @brief GPS position topic data structure. x = Longitude, y = Latitude, z = Altitude, unit: deg*10<SUP>-7</SUP>
 * (Lat,Lon), mm (Alt)
 */
typedef T_DjiFcSubscriptionGpsPosition T_PsdkDataSubscriptionGpsPosition;

/**
 * @brief GPS velocity topic data structure, unit: cm/s.
 */
typedef T_DjiFcSubscriptionGpsVelocity T_PsdkDataSubscriptionGpsVelocity;

/**
 * @brief GPS details topic data structure.
 */
typedef T_DjiFcSubscriptionGpsDetails T_PsdkDataSubscriptionGpsDetail;

/**
 * @brief GPS signal level topic data structure. Signal level of GPS. The level varies from 0 to 5, with 0 being the
 * worst and 5 the best GPS signal.
 */
typedef T_DjiFcSubscriptionGpsSignalLevel T_PsdkDataSubscriptionGpsSignalLevel;

/**
 * @brief RTK position topic data structure.
 */
typedef T_DjiFcSubscriptionRtkPosition T_PsdkDataSubscriptionRtkPosition;

/**
 * @brief RTK velocity topic data structure, unit: cm/s.
 */
typedef T_DjiFcSubscriptionRtkVelocity T_PsdkDataSubscriptionRtkVelocity;

/**
 * @brief RTK yaw topic data structure, unit: deg.
 */
typedef T_DjiFcSubscriptionRtkYaw T_PsdkDataSubscriptionRtkYaw;

/**
 * @brief RTK position information topic data structure. Specifies RTK position solution state, it can be any value of
 * ::E_PsdkDataSubscriptionPositionSolutionProperty.
 */
typedef T_DjiFcSubscriptionRtkPositionInfo T_PsdkDataSubscriptionRtkPositionInfo;

/**
 * @brief RTK yaw information topic data structure. Specifies RTK yaw solution state, it can be any value of
 * ::E_PsdkDataSubscriptionPositionSolutionProperty.
 */
typedef T_DjiFcSubscriptionRtkYawInfo T_PsdkDataSubscriptionRtkYawInfo;

/**
 * @brief Flight status information topic data structure. It can be any value of ::E_PsdkDataSubscriptionFlightStatus.
 */
typedef T_DjiFcSubscriptionFlightStatus T_PsdkDataSubscriptionFlightStatus;

/**
 * @brief Battery information topic data structure.
 */
typedef T_DjiFcSubscriptionWholeBatteryInfo T_PsdkDataSubscriptionBatteryInfo;

#pragma pack()

/**
 * @brief Prototype of callback function used to receive data of topic.
 * @warning User can not execute blocking style operations or functions in the callback function, because that will block PSDK
 * root thread, causing problems such as slow system response, payload disconnection or infinite loop.
 * @param data: pointer to data of the topic, user need transfer type of this pointer to the corresponding data structure
 * pointer for getting every item of the topic conveniently.
 * @param dataSize: the size of memory space pointed by data argument, equal to data structure size corresponding to the topic.
 * @param timestamp: pointer to timestamp corresponding this data.
 * @return Execution result.
 */
typedef DjiReceiveDataOfTopicCallback ReceiveDataOfTopicCallback;

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Initialise data subscription module in blocking mode. This function has to be called before subscribing any
 * data, to initialize run environment of data subscription module, if need to subscribe data from aircraft.
 * @note Max execution time of this function is slightly larger than 500ms.
 * @note This function has to be called in user task, rather than main() function, and after scheduler being started.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkDataSubscription_Init(void);

/**
 * @brief Deinitialize data subscription module. When data subscription will no longer be used, can call this function
 * to deinitialize the data subscription module. Deinitialization function will help to release all system resource data
 * subscription has occupied.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkDataSubscription_DeInit(void);

/**
 * @brief Subscribe a topic in blocking mode. Before subscribing any data from aircraft, PsdkDataSubscription_Init()
 * function has to be called.
 * @details User can subscribe a topic by specifying topic name, push frequency and callback function used to receive
 * data of topic (if needed). After subscribing successfully, the user can call
 * PsdkDataSubscription_GetValueOfTopicWithTimestamp() function to get the latest data of the topic have been
 * subscribed and the corresponding timestamp when aircraft sends the data out, and the callback function will be called to
 * push data of the topic and corresponding timestamp if the callback function is specified.
 * @note Max execution time of this function is slightly larger than 1200ms.
 * @note Topic to be subscribed can not have been subscribed, that is, one topic can not be subscribed repeatedly.
 * @note User must ensure that types of subscription frequency of all topics have been subscribed is less than or
 * equal to 4. Otherwise, the subscribing topic will fail.
 * @note User must ensure that the data length sum of all topics of the same subscription frequency is less than or equal to 242.
 * @param topic: topic name to be subscribed.
 * @param frequency: subscription frequency of topic to be subscribed. Subscription frequency can not beyond max
 * frequency limitation of the topic and must be a divisor of max subscription frequency limitation. And, subscription
 * frequency has to be larger than 0. Users can find max frequency of topics in data subscription part of PSDK
 * documentation on developer website (developer.dji.com).
 * @param callback: callback function used to receive data of topic to be subscribed. If the callback function is not needed,
 * this item can be set as NULL.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkDataSubscription_RegTopicSync(E_PsdkDataSubscriptionTopicName topic, uint16_t frequency,
                                                   ReceiveDataOfTopicCallback callback);

/**
 * @brief Unsubscribe a topic in blocking mode.
 * @details Users must specify frequency and callback function filled in when subscribe the topic. If frequency or
 * callback function is incorrect, unsubscribe operation will fail and the interface return error.
 * @note Max execution time of this function is slightly larger than 600ms.
 * @param topic: topic name to be unsubscribed.
 * @param frequency: subscription frequency filled in when subscribe.
 * @param callback: callback function filled in when subscribe.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkDataSubscription_AntiRegTopicSync(E_PsdkDataSubscriptionTopicName topic, uint16_t frequency,
                                                       ReceiveDataOfTopicCallback callback);

/**
 * @brief Get the latest data value and timestamp in aircraft time system when sending the data from aircraft of specified
 * topic. If the specified topic has not been subscribed successfully, this function will return the error code.
 * @note After calling this function, user need transfer type of data pointer that pointer to data of topic to
 * corresponding data structure pointer for getting every item of the topic conveniently.
 * @param topicName: topic name to be gotten value.
 * @param data: pointer to memory space used to store data of the topic. The memory space used to store data of topic
 * have to have been allocated correctly and should ensure its size is equal to data structure size corresponding to
 * the topic, otherwise, this function will not be able to return data and timestamp (return error code).
 * @param dataSizeOfTopic: the size of memory space used to store data of topic. Normally, this size is equal to data
 * structure size corresponding to the topic. If this size is not equal to the size of the memory space, may cause memory
 * overflow event
 * @param timestamp: pointer to memory space used to store timestamps. The memory space used to store timestamps
 * have to have been allocated correctly, and should ensure its size is equal to data structure size of timestamp,
 * otherwise, this function will not be able to return data and timestamp (return error code) or even cause memory
 * overflow event. If the user does not need timestamp information, can fill in NULL.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkDataSubscription_GetValueOfTopicWithTimestamp(E_PsdkDataSubscriptionTopicName topicName,
                                                                   uint8_t *data, uint16_t dataSizeOfTopic,
                                                                   T_PsdkDataSubscriptiontTimestamp *timestamp);

#ifdef __cplusplus
}
#endif

#endif // PSDK_DATA_SUBSCRIPTION_H

/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
