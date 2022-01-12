/**
 ********************************************************************
 * @file    psdk_data_subscription.h
 * @brief   This is the header file for "psdk_data_subscription.c", defining the structure and
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
#ifndef PSDK_DATA_SUBSCRIPTION_H
#define PSDK_DATA_SUBSCRIPTION_H

/* Includes ------------------------------------------------------------------*/
#include "psdk_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/
/**
 * @brief Name of topics that can be subscribed. Topic is minimum data item subscribed.
 */
typedef enum {
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
        PSDK_DATA_SUBSCRIPTION_TOPIC_QUATERNION = 0,

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
        PSDK_DATA_SUBSCRIPTION_TOPIC_VELOCITY,

    /*!
     * @brief Fused altitude of aircraft topic name. Fused altitude topic provides aircraft's fused altitude from sea
     * level. Please refer to ::T_PsdkDataSubscriptionAltitudeFused for information about data structure.
     */
        PSDK_DATA_SUBSCRIPTION_TOPIC_ALTITUDE_FUSED,

    /*!
     * @brief Relative height above ground of aircraft topic name. Please refer to
     * ::T_PsdkDataSubscriptionHeightRelative for information about data structure.
     * @details This data is a fusion output from aircraft. The height is a direct estimate of the closest large object
     * below the aircraft's ultrasonic sensors.
     * @warning This topic does not come with a 'valid' flag - so if the aircraft is too far from an object for the
     * ultrasonic sensors/VO to provide any meaningful data, the values will latch and there is no way for user to
     * determine if the data is valid or not. Please use with caution.
     */
        PSDK_DATA_SUBSCRIPTION_TOPIC_HEIGHT_RELATIVE,

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
        PSDK_DATA_SUBSCRIPTION_TOPIC_POSITION_FUSED,

    /*!
     * @brief GPS date topic name. Please refer to ::T_PsdkDataSubscriptionGpsDate for information about data structure.
     */
        PSDK_DATA_SUBSCRIPTION_TOPIC_GPS_DATE,

    /*!
     * @brief GPS time topic name. Please refer to ::T_PsdkDataSubscriptionGpsTime for information about data structure.
     */
        PSDK_DATA_SUBSCRIPTION_TOPIC_GPS_TIME,

    /*!
     * @brief GPS position topic name. Please refer to ::T_PsdkDataSubscriptionGpsPosition for information about data structure.
     * @details
     *   | Axis | Accuracy                                         |
         |------|--------------------------------------------------|
         | x, y | <3m with open sky without multipath              |
         | z    | <5m with open sky without multipath              |
     */
        PSDK_DATA_SUBSCRIPTION_TOPIC_GPS_POSITION,

    /*!
     * @brief GPS velocity topic name. Please refer to ::T_PsdkDataSubscriptionGpsVelocity for information about data structure.
     */
        PSDK_DATA_SUBSCRIPTION_TOPIC_GPS_VELOCITY,

    /*!
     * @brief GPS details topic name. GPS details topic provides GPS state and other detail information. Please refer
     * to ::T_PsdkDataSubscriptionGpsDetail for information about data structure.
     */
        PSDK_DATA_SUBSCRIPTION_TOPIC_GPS_DETAILS,

    /*!
     * @brief GPS signal level topic name. This topic provides a measure of the quality of GPS signal. Please refer to
     * ::T_PsdkDataSubscriptionGpsSignalLevel for information about data structure.
     */
        PSDK_DATA_SUBSCRIPTION_TOPIC_GPS_SIGNAL_LEVEL,

    /*!
     * @brief RTK position topic name. Please refer to ::T_PsdkDataSubscriptionRtkPosition for information about data structure.
     * @details
     *   | Axis | Accuracy                                         |
         |------|--------------------------------------------------|
         | x, y | ~2cm with fine alignment and fix condition       |
         | z    | ~3cm with fine alignment and fix condition       |
     */
        PSDK_DATA_SUBSCRIPTION_TOPIC_RTK_POSITION,

    /*!
     * @brief RTK velocity topic name. Please refer to ::T_PsdkDataSubscriptionRtkVelocity for information about data structure.
     */
        PSDK_DATA_SUBSCRIPTION_TOPIC_RTK_VELOCITY,

    /*!
     * @brief RTK yaw topic name. Please refer to ::T_PsdkDataSubscriptionRtkYaw for information about data structure.
     * @details The RTK yaw will provide the vector from ANT1 to ANT2 as configured in DJI Assistant 2. This
     * means that the value of RTK yaw will be 90deg offset from the yaw of the aircraft.
     */
        PSDK_DATA_SUBSCRIPTION_TOPIC_RTK_YAW,

    /*!
     * @brief RTK position information topic name. RTK position information topic provides a state of RTK position
     * solution. Please refer to ::T_PsdkDataSubscriptionRtkPositionInfo for information about data structure.
     */
        PSDK_DATA_SUBSCRIPTION_TOPIC_RTK_POSITION_INFO,

    /*!
     * @brief RTK yaw topic name. RTK yaw information topic provides a state of RTK yaw solution. Please refer to
     * ::T_PsdkDataSubscriptionRtkYawInfo for information about data structure.
     */
        PSDK_DATA_SUBSCRIPTION_TOPIC_RTK_YAW_INFO,

    /*!
     * @brief Flight status topic name. Please refer to ::T_PsdkDataSubscriptionFlightStatus for information about data structure.
     */
        PSDK_DATA_SUBSCRIPTION_TOPIC_STATUS_FLIGHT,

    /*!
     * @brief Battery information topic name. Please refer to ::T_PsdkDataSubscriptionBatteryInfo for information about data structure.
     */
        PSDK_DATA_SUBSCRIPTION_TOPIC_BATTERY_INFO,

    /*! Total number of topics that can be subscribed. */
        PSDK_DATA_SUBSCRIPTION_TOPIC_TOTAL_NUMBER,
} E_PsdkDataSubscriptionTopicName;

/**
 * @brief Health state of data subscribed.
 */
typedef enum {
    PSDK_DATA_SUBSCRIPTION_DATA_NOT_HEALTH = 0, /*!< Data subscribed is healthy and can be used. */
    PSDK_DATA_SUBSCRIPTION_DATA_HEALTH = 1, /*!< Data subscribed is not healthy and recommend not to use it. */
} E_PsdkDataSubscriptionDataHealthFlag;

/**
 * @brief Position solution property.
 */
typedef enum {
    PSDK_DATA_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_NOT_AVAILABLE = 0, /*!< Position solution is not available. */
    PSDK_DATA_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_FIX_POSITION = 1, /*!< Position has been fixed by the FIX POSITION command. */
    PSDK_DATA_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_FIX_HEIGHT_AUTO = 2, /*!< Position has been fixed by the FIX HEIGHT/AUTO command. */
    PSDK_DATA_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_INSTANTANEOUS_DOPPLER_COMPUTE_VELOCITY = 8, /*!< Velocity computed using instantaneous Doppler. */
    PSDK_DATA_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_SINGLE_PNT_SOLUTION = 16, /*!< Single point position solution. */
    PSDK_DATA_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_PSEUDORANGE_DIFFERENTIAL_SOLUTION = 17, /*!< Pseudorange differential solution. */
    PSDK_DATA_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_SBAS_CORRECTION_CALCULATED = 18, /*!< Solution calculated using corrections from an SBAS. */
    PSDK_DATA_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_KALMAN_FILTER_WITHOUT_OBSERVATION_PROPAGATED = 19, /*!< Propagated by a Kalman filter without new observations. */
    PSDK_DATA_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_OMNISTAR_VBS_POSITION = 20, /*!< OmniSTAR VBS position (L1 sub-metre). */
    PSDK_DATA_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_FLOAT_L1_AMBIGUITY = 32, /*!< Floating L1 ambiguity solution. */
    PSDK_DATA_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_FLOAT_IONOSPHERIC_FREE_AMBIGUITY = 33, /*!< Floating ionospheric-free ambiguity solution. */
    PSDK_DATA_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_FLOAT_SOLUTION = 34, /*!< Float position solution. */
    PSDK_DATA_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_L1_AMBIGUITY_INT = 48, /*!< Integer L1 ambiguity solution. */
    PSDK_DATA_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_WIDE_LANE_AMBIGUITY_INT = 49, /*!< Integer wide-lane ambiguity solution. */
    PSDK_DATA_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_NARROW_INT = 50, /*!< Narrow fixed point position solution. */
} E_PsdkDataSubscriptionPositionSolutionProperty;

/**
 * @brief GPS fix state.
 */
typedef enum {
    PSDK_DATA_SUBSCRIPTION_GPS_FIX_STATE_NO_FIX = 0, /*!< GPS position has not been fixed. */
    PSDK_DATA_SUBSCRIPTION_GPS_FIX_STATE_DEAD_RECKONING_ONLY = 1, /*!< GPS position is dead reckoned only. */
    PSDK_DATA_SUBSCRIPTION_GPS_FIX_STATE_2D_FIX = 2, /*!< The horizontal position with latitude/longitude (or northing/easting or X/Y) is fixed. */
    PSDK_DATA_SUBSCRIPTION_GPS_FIX_STATE_3D_FIX = 3, /*!< The horizontal and vertical position with latitude/longitude/altitude (northing/easting/altitude or X/Y/Z) is fixed. */
    PSDK_DATA_SUBSCRIPTION_GPS_FIX_STATE_GPS_PLUS_DEAD_RECKONING = 4, /*!< Position is calculated by GPS and combined with dead reckoning. */
    PSDK_DATA_SUBSCRIPTION_GPS_FIX_STATE_TIME_ONLY_FIX = 5, /*!< Only time is fixed. */
} E_PsdkDataSubscriptionGpsFixState;

/**
 * @brief Flight status of aircraft.
 */
typedef enum {
    PSDK_DATA_SUBSCRIPTION_FLIGHT_STATUS_STOPED = 0, /*!< Aircraft is on ground and motors are still. */
    PSDK_DATA_SUBSCRIPTION_FLIGHT_STATUS_ON_GROUND = 1, /*!< Aircraft is on ground but motors are rotating. */
    PSDK_DATA_SUBSCRIPTION_FLIGHT_STATUS_IN_AIR = 2, /*!< Aircraft is in air. */
} E_PsdkDataSubscriptionFlightStatus;

#pragma pack(1)

/**
 * @brief Timestamp data structure.
 */
typedef struct {
    uint32_t millisecond; /*!< Millisecond. */
    uint32_t microsecond; /*!< Microsecond. */
} T_PsdkDataSubscriptiontTimestamp;

/**
 * @brief Quaternion topic data structure.
 */
typedef struct Quaternion {
    psdk_f32_t q0; /*!< w, rad (when converted to a rotation matrix or Euler angles). */
    psdk_f32_t q1; /*!< x, rad (when converted to a rotation matrix or Euler angles). */
    psdk_f32_t q2; /*!< y, rad (when converted to a rotation matrix or Euler angles). */
    psdk_f32_t q3; /*!< z, rad (when converted to a rotation matrix or Euler angles). */
} T_PsdkDataSubscriptionQuaternion;

/**
 * @brief Velocity topic data structure.
 */
typedef struct Velocity {
    /*! Velocity of aircraft. */
    T_PsdkVector3f data;
    /*! Health state of aircraft velocity data. It can be any value of ::E_PsdkDataSubscriptionDataHealthFlag. */
    uint8_t health : 1;
    /*! Reserved. */
    uint8_t reserve : 7;
} T_PsdkDataSubscriptionVelocity;

/**
 * @brief Fused altitude topic data structure, unit: m.
 */
typedef psdk_f32_t T_PsdkDataSubscriptionAltitudeFused;

/**
 * @brief Relative height above ground topic data structure, unit: m.
 */
typedef psdk_f32_t T_PsdkDataSubscriptionHeightRelative;

/**
 * @brief Fused position topic data structure.
 */
typedef struct PositionFused {
    psdk_f64_t longitude; /*!< Longitude, unit: rad. */
    psdk_f64_t latitude; /*!< Latitude, unit: rad. */
    psdk_f32_t altitude; /*!< Altitude, WGS 84 reference ellipsoid, unit: m. */
    uint16_t visibleSatelliteNumber; /*!< Number of visible satellites. */
} T_PsdkDataSubscriptionPositionFused;

/**
 * @brief GPS date topic data structure, format: yyyymmdd.
 */
typedef uint32_t T_PsdkDataSubscriptionGpsDate;

/**
 * @brief GPS time topic data structure, format: hhmmss.
 */
typedef uint32_t T_PsdkDataSubscriptionGpsTime;

/**
 * @brief GPS position topic data structure. x = Longitude, y = Latitude, z = Altitude, unit: deg*10<SUP>-7</SUP>
 * (Lat,Lon), mm (Alt)
 */
typedef T_PsdkVector3d T_PsdkDataSubscriptionGpsPosition;

/**
 * @brief GPS velocity topic data structure, unit: cm/s.
 */
typedef T_PsdkVector3f T_PsdkDataSubscriptionGpsVelocity;

/**
 * @brief GPS details topic data structure.
 */
typedef struct GpsDetail {
    psdk_f32_t hdop; /*!< Horizontal dilution of precision, <1: ideal, 1-2: excellent, 2-5: good, 5-10: moderate, 10-20: fair, >20: poor. */
    psdk_f32_t pdop; /*!< Position dilution of precision, <1: ideal, 1-2: excellent, 2-5: good, 5-10: moderate, 10-20: fair, >20: poor. */
    psdk_f32_t fixState; /*!< GPS fix state, and can be any value of ::E_PsdkDataSubscriptionGpsFixState. Value other than ::E_PsdkDataSubscriptionGpsFixState is invalid. */
    psdk_f32_t vacc; /*!< Vertical position accuracy (mm), the smaller, the better. */
    psdk_f32_t hacc; /*!< Horizontal position accuracy (mm), the smaller, the better. */
    psdk_f32_t sacc; /*!< Speed accuracy (cm/s), the smaller, the better. */
    uint32_t gpsSatelliteNumberUsed; /*!< Number of GPS satellites used for fixing position. */
    uint32_t glonassSatelliteNumberUsed; /*!< Number of GLONASS satellites used for fixing position. */
    uint16_t totalSatelliteNumberUsed; /*!< Total number of satellites used for fixing position. */
    uint16_t gpsCounter; /*!< Accumulated times of sending GPS data. */
} T_PsdkDataSubscriptionGpsDetail;

/**
 * @brief GPS signal level topic data structure. Signal level of GPS. The level varies from 0 to 5, with 0 being the
 * worst and 5 the best GPS signal.
 */
typedef uint8_t T_PsdkDataSubscriptionGpsSignalLevel;

/**
 * @brief RTK position topic data structure.
 */
typedef struct PositionData {
    psdk_f64_t longitude; /*!< Longitude, unit: deg. */
    psdk_f64_t latitude;  /*!< Latitude, unit: deg. */
    psdk_f32_t hfsl;      /*!< Height above mean sea level, unit: m. */
} T_PsdkDataSubscriptionRtkPosition;

/**
 * @brief RTK velocity topic data structure, unit: cm/s.
 */
typedef T_PsdkVector3f T_PsdkDataSubscriptionRtkVelocity;

/**
 * @brief RTK yaw topic data structure, unit: deg.
 */
typedef int16_t T_PsdkDataSubscriptionRtkYaw;

/**
 * @brief RTK position information topic data structure. Specifies RTK position solution state, it can be any value of
 * ::E_PsdkDataSubscriptionPositionSolutionProperty.
 */
typedef uint8_t T_PsdkDataSubscriptionRtkPositionInfo;

/**
 * @brief RTK yaw information topic data structure. Specifies RTK yaw solution state, it can be any value of
 * ::E_PsdkDataSubscriptionPositionSolutionProperty.
 */
typedef uint8_t T_PsdkDataSubscriptionRtkYawInfo;

/**
 * @brief Flight status information topic data structure. It can be any value of ::E_PsdkDataSubscriptionFlightStatus.
 */
typedef uint8_t T_PsdkDataSubscriptionFlightStatus;

/**
 * @brief Battery information topic data structure.
 */
typedef struct BatteryInfo {
    uint32_t capacity; /*!< Battery capacity, unit: mAh. */
    int32_t voltage; /*!< Battery voltage, unit: mV. */
    int32_t current; /*!< Battery current, unit: mA. */
    uint8_t percentage; /*!< Battery capacity percentage, unit: 1%. */
} T_PsdkDataSubscriptionBatteryInfo;

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
typedef T_PsdkReturnCode (*ReceiveDataOfTopicCallback)(const uint8_t *data, uint16_t dataSize,
                                                       const T_PsdkDataSubscriptiontTimestamp *timestamp);

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
