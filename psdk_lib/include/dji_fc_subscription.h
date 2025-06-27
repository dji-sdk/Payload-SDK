/**
 ********************************************************************
 * @file    dji_fc_subscription.h
 * @brief   This is the header file for "dji_fc_subscription.c", defining the structure and
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
#ifndef DJI_FC_SUBSCRIPTION_H
#define DJI_FC_SUBSCRIPTION_H

/* Includes ------------------------------------------------------------------*/
#include "dji_typedef.h"

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
     * (NED) rotation. Please refer to ::T_DjiFcSubscriptionQuaternion for information about data structure.
     * @details The DJI quaternion follows Hamilton convention (q0 = w, q1 = x, q2 = y, q3 = z).
     * | Angle        | Unit | Accuracy   | Notes                                           |
       |--------------|------|------------|-------------------------------------------------|
       | pitch, roll  | deg  | <1         | in NON-AHRS mode                                |
       | yaw          | deg  | <3         | in well-calibrated compass with fine aligned    |
       | yaw with rtk | deg  | around 1.2 | in RTK heading fixed mode with 1 meter baseline |
     * @datastruct \ref T_DjiFcSubscriptionQuaternion
     */
    DJI_FC_SUBSCRIPTION_TOPIC_QUATERNION = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 0),
    /*!
     * @brief Provides aircraft's acceleration w.r.t a ground-fixed \b NEU frame @ up to 200Hz
     * @warning Please note that this data is not in a conventional right-handed frame of reference.
     * @details This is a fusion output from the flight control system. The output is in a right-handed NED frame, but the
     * sign of the Z-axis acceleration is flipped before publishing to this topic. So if you are looking to get acceleration
     * in an NED frame, simply flip the sign of the z-axis value. Beyond that, you can convert using rotations to
     * any right-handed frame of reference.
     * @units m/s<SUP>2</SUP>
     * @datastruct \ref T_DjiFcSubscriptionAccelerationGround
     */
    DJI_FC_SUBSCRIPTION_TOPIC_ACCELERATION_GROUND = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 1),
    /*!
     * @brief Provides aircraft's acceleration w.r.t a body-fixed \b FRU frame @ up to 200Hz
     * @warning Please note that this data is not in a conventional right-handed frame of reference.
     * @details This is a fusion output from the flight control system.
     * @units m/s<SUP>2</SUP>
     * @datastruct \ref T_DjiFcSubscriptionAccelerationBody
     */
    DJI_FC_SUBSCRIPTION_TOPIC_ACCELERATION_BODY = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 2),
    /*!
     * @brief Provides aircraft's acceleration in an IMU-centered, body-fixed \b FRD frame @ up to 400Hz
     * @details This is a filtered output from the IMU on board the flight control system.
     * @sensors IMU
     * @units m/s<SUP>2</SUP>
     * @datastruct \ref T_DjiFcSubscriptionAccelerationRaw
     */
    DJI_FC_SUBSCRIPTION_TOPIC_ACCELERATION_RAW = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 3),
    /*!
     * @brief Velocity of aircraft topic name. Velocity topic provides aircraft's velocity in a ground-fixed NEU frame.
     * Please refer to ::T_DjiFcSubscriptionVelocity for information about data structure.
     * @warning Please note that this data is not in a conventional right-handed frame of reference.
     * @details This velocity data is a fusion output from the aircraft. Original output is in a right-handed NED frame, but the
     * sign of the Z-axis velocity is flipped before publishing to this topic. So if you are looking to get velocity
     * in an NED frame, simply flip the sign of the z-axis value. Beyond that, you can convert using rotations to
     * any right-handed frame of reference.
     * | Axis     | Unit | Accuracy                                                                                    |
       |----------|------|---------------------------------------------------------------------------------------------|
       | vgx, vgy | m/s  | Around 5cm/s for GNSS navigation. Around 3cm/s with VO at 1 meter height                    |
       | vgz      | m/s  | 10cm/s only with barometer in steady air. 3cm/s with VO at 1 meter height with 8cm baseline |
     * @datastruct \ref T_DjiFcSubscriptionVelocity
     */
    DJI_FC_SUBSCRIPTION_TOPIC_VELOCITY = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 4),
    /*!
     * @brief Provides aircraft's angular velocity in a ground-fixed \b NED frame @ up to 200Hz
     * @details This is a fusion output from the flight control system.
     * @units rad/s
     * @datastruct \ref T_DjiFcSubscriptionAngularRateFusioned
     */
    DJI_FC_SUBSCRIPTION_TOPIC_ANGULAR_RATE_FUSIONED = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 5),
    /*!
     * @brief Provides aircraft's angular velocity in an IMU-centered, body-fixed \b FRD frame @ up to 400Hz
     * @details This is a filtered output from the IMU on board the flight control system.
     * @sensors IMU
     * @units rad/s
     * @datastruct \ref T_DjiFcSubscriptionAngularRateRaw
     */
    DJI_FC_SUBSCRIPTION_TOPIC_ANGULAR_RATE_RAW = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 6),
    /*!
     * @brief Fused altitude of aircraft topic name. Fused altitude topic provides aircraft's fused altitude from sea
     * level. Please refer to ::T_DjiFcSubscriptionAltitudeFused for information about data structure.
     * @units m
     * @datastruct \ref T_DjiFcSubscriptionAltitudeFused
     */
    DJI_FC_SUBSCRIPTION_TOPIC_ALTITUDE_FUSED = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 7),
    /*!
     * @brief Provides aircraft's pressure altitude from sea level using the ICAO model @ up to 200Hz
     * @details
     * This is a filetered output from the barometer without any further fusion.
     *
     * The ICAO model gives an MSL altitude of 1013.25mBar at 15&deg; C and a temperature lapse rate of -6.5&deg; C
     * per 1000m. In your case, it may be possible that the take off altitude of the aircraft is recording a higher pressure
     * than 1013.25mBar. Let's take an example - a weather station shows that SFO (San Francisco International Airport) had
     * recently recorded a pressure of 1027.1mBar. SFO is 4m above MSL, yet, if you calculate the Pressure Altitude using
     * the ICAO model, it relates to -114m. You can use an online calculator to similarly calculate the Pressure Altitude
     * in your area.
     *
     * Another factor that may affect your altitude reading is manufacturing differences in the barometer - it is not
     * uncommon to have a variation of &plusmn;30m readings at the same physical location with two different aircraft. For a given
     * aircraft, these readings will be consistent, so you will need to calibrate the offset of your system if your code
     * relies on the accuracy of the absolute value of altitude.
     * @sensors GPS, Barometer, IMU
     * @units m
     * @datastruct \ref T_DjiFcSubscriptionAltitudeBarometer
     */
    DJI_FC_SUBSCRIPTION_TOPIC_ALTITUDE_BAROMETER = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 8),
    /*!
     * @brief Provides the altitude from sea level when the aircraft last took off.
     * @details
     * This is a fusion output from the flight control system, and also uses the ICAO model.
     *
     * The ICAO model gives an MSL altitude of 1013.25mBar at 15&deg; C and a temperature lapse rate of -6.5&deg; C
     * per 1000m. In your case, it may be possible that the take off altitude of the aircraft is recording a higher pressure
     * than 1013.25mBar. Let's take an example - a weather station shows that SFO (San Francisco International Airport) had
     * recently recorded a pressure of 1027.1mBar. SFO is 4m above MSL, yet, if you calculate the Pressure Altitude using
     * the ICAO model, it relates to -114m. You can use an online calculator to similarly calculate the Pressure Altitude
     * in your area.
     *
     * Another factor that may affect your altitude reading is manufacturing differences in the barometer - it is not
     * uncommon to have a variation of &plusmn;30m readings at the same physical location with two different aircraft. For a given
     * aircraft, these readings will be consistent, so you will need to calibrate the offset of your system if your code
     * relies on the accuracy of the absolute value of altitude.
     *
     * @note This value is updated each time the drone takes off.
     *
     * @units m
     * @datastruct \ref T_DjiFcSubscriptionAltitudeOfHomePoint
     */
    DJI_FC_SUBSCRIPTION_TOPIC_ALTITUDE_OF_HOMEPOINT = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 9),
    /*!
     * @brief Provides the relative height above ground at up to 100Hz.
     * @details
     * This is a fusion output from the flight control system. The height is a direct estimate of the closest large object below the aircraft's ultrasonic sensors.
     * A large object is something that covers the ultrasonic sensor for an extended duration of time.
     *
     * @warning This topic does not come with a 'valid' flag - so if the aircraft is too far from an object for the
     * ultrasonic sensors/VO to provide any meaningful data, the values will latch and there is no way for user code to
     * determine if the data is valid or not. Use with caution.
     * @sensors Visual Odometry, Ultrasonic
     * @units m
     * @datastruct \ref T_DjiFcSubscriptionHeightFusion
     */
    DJI_FC_SUBSCRIPTION_TOPIC_HEIGHT_FUSION = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 10),
    /*!
     * @brief Relative height above ground of aircraft topic name. Please refer to
     * ::T_DjiFcSubscriptionHeightRelative for information about data structure.
     * @details This data is a fusion output from aircraft. The height is a direct estimate of the closest large object
     * below the aircraft's ultrasonic sensors.
     * @warning This topic does not come with a 'valid' flag - so if the aircraft is too far from an object for the
     * ultrasonic sensors/VO to provide any meaningful data, the values will latch and there is no way for user to
     * determine if the data is valid or not. Please use with caution.
     * @datastruct \ref T_DjiFcSubscriptionHeightRelative
     */
    DJI_FC_SUBSCRIPTION_TOPIC_HEIGHT_RELATIVE = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 11),

    /*!
     * Fused position of aircraft topic name. Please refer to ::T_DjiFcSubscriptionPositionFused for information
     * about data structure.
     * @warning Please note that if GPS signal is weak (low visibleSatelliteNumber, see below), the
     * latitude/longitude values won't be updated but the altitude might still be. There is currently no way to know if
     * the lat/lon update is healthy.
     * @details The most important component of this topic is the T_DjiFcSubscriptionPositionFused::visibleSatelliteNumber.
     * Use this to track your GPS satellite coverage and build some heuristics for when you might expect to lose GPS updates.
     *   | Axis | Unit | Position Sensor | Accuracy                                         |
         |------|------|-----------------|--------------------------------------------------|
         | x, y | m    | GPS             | <3m with open sky without multipath              |
         | z    | m    | GPS             | <5m with open sky without multipath              |
     * @datastruct \ref T_DjiFcSubscriptionPositionFused
     */
    DJI_FC_SUBSCRIPTION_TOPIC_POSITION_FUSED = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 12),

    /*!
     * @brief GPS date topic name. Please refer to ::T_DjiFcSubscriptionGpsDate for information about data structure.
     * @datastruct \ref T_DjiFcSubscriptionGpsDate
     */
    DJI_FC_SUBSCRIPTION_TOPIC_GPS_DATE = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 13),

    /*!
     * @brief GPS time topic name. Please refer to ::T_DjiFcSubscriptionGpsTime for information about data structure.
     * @datastruct \ref T_DjiFcSubscriptionGpsTime
     */
    DJI_FC_SUBSCRIPTION_TOPIC_GPS_TIME = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 14),

    /*!
     * @brief GPS position topic name. Please refer to ::T_DjiFcSubscriptionGpsPosition for information about data structure.
     * @details
     *   | Axis | Accuracy                                         |
         |------|--------------------------------------------------|
         | x, y | <3m with open sky without multipath              |
         | z    | <5m with open sky without multipath              |
     * @datastruct \ref T_DjiFcSubscriptionGpsPosition
     */
    DJI_FC_SUBSCRIPTION_TOPIC_GPS_POSITION = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 15),

    /*!
     * @brief GPS velocity topic name. Please refer to ::T_DjiFcSubscriptionGpsVelocity for information about data structure.
     * @datastruct \ref T_DjiFcSubscriptionGpsVelocity
     */
    DJI_FC_SUBSCRIPTION_TOPIC_GPS_VELOCITY = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 16),

    /*!
     * @brief GPS details topic name. GPS details topic provides GPS state and other detail information. Please refer
     * to ::T_DjiFcSubscriptionGpsDetail for information about data structure.
     * @datastruct \ref T_DjiFcSubscriptionGpsDetails
     */
    DJI_FC_SUBSCRIPTION_TOPIC_GPS_DETAILS = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 17),

    /*!
     * @brief GPS signal level topic name. This topic provides a measure of the quality of GPS signal. Please refer to
     * ::T_DjiFcSubscriptionGpsSignalLevel for information about data structure.
     * @datastruct \ref T_DjiFcSubscriptionGpsSignalLevel
     */
    DJI_FC_SUBSCRIPTION_TOPIC_GPS_SIGNAL_LEVEL = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 18),

    /*!
     * @brief RTK position topic name. Please refer to ::T_DjiFcSubscriptionRtkPosition for information about data structure.
     * @details
     *   | Axis | Accuracy                                         |
         |------|--------------------------------------------------|
         | x, y | ~2cm with fine alignment and fix condition       |
         | z    | ~3cm with fine alignment and fix condition       |
     * @datastruct \ref T_DjiFcSubscriptionRtkPosition
     */
    DJI_FC_SUBSCRIPTION_TOPIC_RTK_POSITION = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 19),

    /*!
     * @brief RTK velocity topic name. Please refer to ::T_DjiFcSubscriptionRtkVelocity for information about data structure.
     * @datastruct \ref T_DjiFcSubscriptionRtkVelocity
     */
    DJI_FC_SUBSCRIPTION_TOPIC_RTK_VELOCITY = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 20),

    /*!
     * @brief RTK yaw topic name. Please refer to ::T_DjiFcSubscriptionRtkYaw for information about data structure.
     * @details The RTK yaw will provide the vector from ANT1 to ANT2 as configured in DJI Assistant 2. This
     * means that the value of RTK yaw will be 90deg offset from the yaw of the aircraft.
     * @datastruct \ref T_DjiFcSubscriptionRtkYaw
     */
    DJI_FC_SUBSCRIPTION_TOPIC_RTK_YAW = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 21),

    /*!
     * @brief RTK position information topic name. RTK position information topic provides a state of RTK position
     * solution. Please refer to ::T_DjiFcSubscriptionRtkPositionInfo for information about data structure.
     * @datastruct \ref T_DjiFcSubscriptionRtkPositionInfo
     */
    DJI_FC_SUBSCRIPTION_TOPIC_RTK_POSITION_INFO = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 22),

    /*!
     * @brief RTK yaw topic name. RTK yaw information topic provides a state of RTK yaw solution. Please refer to
     * ::T_DjiFcSubscriptionRtkYawInfo for information about data structure.
     * @datastruct \ref T_DjiFcSubscriptionRtkYawInfo
     */
    DJI_FC_SUBSCRIPTION_TOPIC_RTK_YAW_INFO = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 23),
    /*!
     * @brief Provides aircraft's magnetometer reading, fused with IMU and GPS @ up to 100Hz
     * @details This reading is the magnetic field recorded by the magnetometer in x,y,z axis, calibrated such that
     * 1000 < |m| < 2000, and fused with IMU and GPS for robustness
     * @sensors Magnetometer, IMU, GPS
     * @units N/A
     * @datastruct \ref T_DjiFcSubscriptionCompass
     */
    DJI_FC_SUBSCRIPTION_TOPIC_COMPASS = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 24),
    /*!
     * @brief Provides remote controller stick inputs @ up to 100Hz
     * @details This topic will give you:
     * - Stick inputs (R,P,Y,Thr)
     * - Mode switch (P/A/F)
     * - Landing gear switch (Up/Down)
     *
     * @datastruct \ref T_DjiFcSubscriptionRC
     * @also \ref TOPIC_RC_WITH_FLAG_DATA
     */
    DJI_FC_SUBSCRIPTION_TOPIC_RC = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 25),
    /*!
     * @brief Provides gimbal pitch, roll, yaw @ up to 50Hz
     * @details
     * The reference frame for gimbal angles is a NED frame attached to the gimbal.
     * This topic uses a data structure, Vector3f, that is too generic for the topic. The order of angles is :
     * |Data Structure Element| Meaning|
     * |----------------------|--------|
     * |Vector3f.x            |pitch   |
     * |Vector3f.y            |roll    |
     * |Vector3f.z            |yaw     |
     *
     * @perf
     * 0.1 deg accuracy in all axes
     *
     * @sensors Gimbal Encoder, IMU, Magnetometer
     * @units deg
     * @datastruct \ref T_DjiFcSubscriptionGimbalAngles
     * @also \ref TOPIC_GIMBAL_STATUS, \ref TOPIC_GIMBAL_CONTROL_MODE
     */
    DJI_FC_SUBSCRIPTION_TOPIC_GIMBAL_ANGLES = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 26),
    /*!
     * @brief Provides gimbal status and error codes @ up to 50Hz
     * @details Please see the \ref GimbalStatus struct for the details on what data you can receive.
     *
     * @datastruct \ref T_DjiFcSubscriptionGimbalStatus
     * @also \ref TOPIC_GIMBAL_ANGLES, \ref TOPIC_GIMBAL_CONTROL_MODE
     */
    DJI_FC_SUBSCRIPTION_TOPIC_GIMBAL_STATUS = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 27),
    /*!
     * @brief Flight status topic name. Please refer to ::T_DjiFcSubscriptionFlightStatus for information about data structure.
     * @datastruct \ref T_DjiFcSubscriptionFlightStatus
     */
    DJI_FC_SUBSCRIPTION_TOPIC_STATUS_FLIGHT = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 28),
    /*!
     * @brief Provides a granular state representation for various tasks/flight modes @ up to 50Hz
     * @details Typically, use this topic together with \ref TOPIC_STATUS_FLIGHT to get a
     * better understanding of the overall status of the aircraft.
     *
     * @datastruct \ref T_DjiFcSubscriptionDisplaymode
     * @also \ref TOPIC_STATUS_FLIGHT
     */
    DJI_FC_SUBSCRIPTION_TOPIC_STATUS_DISPLAYMODE = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 29),
    /*!
     * @brief Provides status for the landing gear state @ up to 50Hz
     *
     * @datastruct \ref T_DjiFcSubscriptionLandinggear
     */
    DJI_FC_SUBSCRIPTION_TOPIC_STATUS_LANDINGGEAR = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 30),
    /*!
     * @brief If motors failed to start, this topic provides reasons why. Available @ up to 50Hz
     * @datastruct \ref T_DjiFcSubscriptionMotorStartError
     * \note These enumerations show up in the ErrorCode class because they can also be returned as acknowledgements
     * for APIs that start the motors, such as \ref Control::takeoff "Takeoff" or \ref Control::armMotors "Arm"
     */
    DJI_FC_SUBSCRIPTION_TOPIC_STATUS_MOTOR_START_ERROR = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC,
                                                                                     31),
    /*!
     * @brief Battery information topic name. Please refer to ::T_DjiFcSubscriptionWholeBatteryInfo for information about data structure.
     * @datastruct \ref T_DjiFcSubscriptionWholeBatteryInfo
     */
    DJI_FC_SUBSCRIPTION_TOPIC_BATTERY_INFO = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 32),
    /*!
     * @brief Provides states of the aircraft related to SDK/RC control
     * @details The following information is available in this topic:
     * |Data Structure Element| Meaning|
     * |----------------------|--------|
     * |controlMode           |The modes in which the aircraft is being controlled (control loops being applied on horizontal, vertical and yaw axes of the aircraft)|
     * |deviceStatus          |Which device is controlling the motion of the aircraft: RC (Manual control), MSDK (Missions kicked off through mobile), OSDK (Missions kicked off through onboard/ low-level flight control)    |
     * |flightStatus          |Has the OSDK been granted control authority? Since MSDK and RC have precedence, it is possible that deviceStatus shows RC or MSDK actually controlling the aircraft but this value is 1.     |
     * |vrcStatus             |Deprecated|
     * @datastruct \ref T_DjiFcSubscriptionControlDevice
     */
    DJI_FC_SUBSCRIPTION_TOPIC_CONTROL_DEVICE = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 33),
    /*!
     * @brief Provides IMU and quaternion data time-synced with a hardware clock signal @ up to 400Hz.
     * @details This is the only data which can be synchronized with external software or hardware systems. If you want to
     * fuse an external sensor's data with the aircraft's IMU, this data along with a hardware trigger from the A3/N3's
     * expansion ports is how you would do it. You can see detailed documentation on how this process works in the [Hardware
     * Sync Guide](https://developer.dji.com/onboard-sdk/documentation/guides/component-guide-hardware-sync.html).
     * @sensors IMU, sensor fusion output
     * @units
     * |Data Structure Element| Units|
     * |----------------------|--------|
     * |Timestamp |2.5ms, 1ns (See \ref SyncTimestamp)|
     * |Quaternion |rad (after converting to rotation matrix)|
     * |Acceleration |g|
     * |Gyroscope |rad/sec|
     * @datastruct \ref T_DjiFcSubscriptionHardSync
     */
    DJI_FC_SUBSCRIPTION_TOPIC_HARD_SYNC = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 34),
    /*!
     * @brief Provides a measure of the quality of GPS signal, with a mechanism for guarding against unset homepoint @ up to 50Hz
     * @details The level varies from 0 to 5, with 0 being the worst and 5 the best GPS signal. The key difference between
     * this and TOPIC_GPS_SIGNAL_LEVEL is that this topic always returns 0 if the homepoint is not set. Once the home point is
     * set, the behavior is exactly the same as TOPIC_GPS_SIGNAL_LEVEL.
     * @sensors GPS
     * @datastruct \ref T_DjiFcSubscriptionGpsControlLevel
     * @also \ref TOPIC_GPS_SIGNAL_LEVEL
     */
    DJI_FC_SUBSCRIPTION_TOPIC_GPS_CONTROL_LEVEL = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 35),
    /*!
     * @brief Provides normalized remote controller stick input data, along with connection status @ up to 50Hz
     * @note This topic was added in August 2018. Your aircraft may require a FW update to enable this feature.
     * @details This topic will give you:
     * - Stick inputs (R,P,Y,Thr)
     * - Mode switch (P/A/F)
     * - Landing gear switch (Up/Down)
     * - Connection status for air system, ground system and MSDK apps. The connection status also includes a
     * logicConnected element, which will change to false if either the air system or the ground system radios
     * are disconnected for >3s.
     * - Deadzones near the center of the stick positions are also handled in this topic.
     *
     * @datastruct \ref T_DjiFcSubscriptionRCWithFlagData
     * @also \ref TOPIC_RC
     */
    DJI_FC_SUBSCRIPTION_TOPIC_RC_WITH_FLAG_DATA = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 36),
    /*!
     * @brief Provides raw data from the ESCs @ up to 50Hz
     * @note This topic was added in August 2018. Your aircraft may require a FW update to enable this feature.
     * @details This topic supports reporting data for up to 8 ESCs; note that only DJI Intelligent ESCs are supported
     * for this reporting feature. Use this topic to get data on elements close to the hardware - e.g. motor speeds,
     * ESC current and voltage, error flags at the ESC level etc.
     * @datastruct \ref T_DjiFcSubscriptionEscData
     */
    DJI_FC_SUBSCRIPTION_TOPIC_ESC_DATA = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 37),
    /*!
     * @brief Provides RTK connection status @ up to 50Hz
     * @note This topic was added in August 2018. Your aircraft may require a FW update to enable this feature.
     * @details This topic will update in real time whether the RTK GPS system is connected or not; typical uses
     * include app-level logic to switch between GPS and RTK sources of positioning based on this flag.
     * @datastruct \ref T_DjiFcSubscriptionRTKConnectStatus
     */
    DJI_FC_SUBSCRIPTION_TOPIC_RTK_CONNECT_STATUS = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 38),
    /*!
     * @brief Provides the mode in which the gimbal will interpret control commands @ up to 50Hz
     * @note This topic was added in August 2018. Your aircraft may require a FW update to enable this feature.
     * @details This topic will report the current control mode which can be set in the
     * DJI Go app, MSDK apps, or through Onboard SDK gimbal control APIs (see \ref Gimbal::AngleData "AngleData" struct
     * for more information)
     * @datastruct \ref T_DjiFcSubscriptionGimbalControlMode
     */
    DJI_FC_SUBSCRIPTION_TOPIC_GIMBAL_CONTROL_MODE = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 39),
    /*!
     * @brief Provides a number of flags which report different errors the aircraft may encounter in flight @ up to 50Hz
     * @note This topic was added in August 2018. Your aircraft may require a FW update to enable this feature.
     * @warning Most of the errors reported by this topic are cases where immediate action is required; you can use these
     * as a baseline for implementing safety-related error-handling routines.
     * @datastruct \ref T_DjiFcSubscriptionFlightAnomaly
     */
    DJI_FC_SUBSCRIPTION_TOPIC_FLIGHT_ANOMALY = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 40),
    /*!
     * @brief Provides aircraft's position in a Cartesian frame @ up to 50Hz, without the need for GPS
     * @warning This topic does not follow a standard co-ordinate convention. Please read the details below for usage.
     * @details This is the only topic which can provide positioning information without having a GPS fix; though this
     * can be a big enabler please note the caveats of using this topic:
     * - The topic will use an origin that does not have a global reference, and is not published to the SDK.
     * - The topic uses a combination of VO and compass heading to identify the X-Y axes of its reference frame. This means
     * that if your compass performance is not good in an environment, there is no guarantee the X-Y axes will point to
     * North and East.
     * - The actual directions of the X-Y axes are currently not published to the SDK.
     * - If during a flight the compass performance were to change dramatically, the orientation of the X-Y axes may change
     * to re-align with North-East. The aircraft's position in X and Y may exhibit discontinuities in these cases.
     * - The reference frame is referred to as the Navigation Frame - Cartesian X,Y axes aligned with N,E directions on a best-effort
     * basis, and Z aligned to D (down) direction.
     * - A health flag for each axis provides some granularity on whether this data is valid or not.
     *
     * The key takeaway from these details is that this topic provides a best-effort attempt at providing position
     * information in the absence of absolute references (GPS, compass etc.), without guarantees of consistency if
     * environmental conditions change. So if your application is confined to a stable environment, or if you will
     * have GPS and compass available at all times, this topic can still provide useful data that cannot be otherwise
     * had. If using for control, make sure to have guards checking for the continuity of data.
     *
     * @note Since this topic relies on visual features and/or GPS, if your environment does not provide any of these
     * sources of data, the quality of this topic will reduce significantly. VO data quality will reduce if you are too high
     * above the ground. Make sure that the Vision Positioning System is enabled in DJI Go 4 before using this topic
     * (by default it is enabled).
     * @sensors IMU, VO, GPS(if available), RTK (if available), ultrasonic, magnetometer, barometer
     * @units m
     * @datastruct \ref T_DjiFcSubscriptionPositionVO
     */
    DJI_FC_SUBSCRIPTION_TOPIC_POSITION_VO = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 41),
    /*!
     * @brief Provides obstacle info around the vehicle @ up to 100Hz
     * @datastruct \ref T_DjiFcSubscriptionAvoidData
     */
    DJI_FC_SUBSCRIPTION_TOPIC_AVOID_DATA = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 42),
    /*!
     * @brief Provides status of whether the home point was set or not
     * @datastruct \ref T_DjiFcSubscriptionHomePointSetStatus
     */
    DJI_FC_SUBSCRIPTION_TOPIC_HOME_POINT_SET_STATUS = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 43),
    /*!
     * @brief Provides homepoint information, the valid of the home point infomation can ref to the
     * topic DJI_FC_SUBSCRIPTION_TOPIC_HOME_POINT_SET_STATUS
     * @datastruct \ref T_DjiFcSubscriptionHomePointInfo
     */
    DJI_FC_SUBSCRIPTION_TOPIC_HOME_POINT_INFO = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 44),
    /*!
     * @brief Provides three gimbal information, used for M300
     * @datastruct \ref T_DjiFcSubscriptionThreeGimbalData
     */
    DJI_FC_SUBSCRIPTION_TOPIC_THREE_GIMBAL_DATA = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 45),

    /*!
     * @brief Battery information topic name. Please refer to ::T_DjiFcSubscriptionSingleBatteryInfo for information about data structure.
     * @datastruct \ref T_DjiFcSubscriptionSingleBatteryInfo
     */
    DJI_FC_SUBSCRIPTION_TOPIC_BATTERY_SINGLE_INFO_INDEX1 = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC,
                                                                                       46),
    /*!
     * @brief Battery information topic name. Please refer to ::T_DjiFcSubscriptionSingleBatteryInfo for information about data structure.
     * @datastruct \ref T_DjiFcSubscriptionSingleBatteryInfo
     */
    DJI_FC_SUBSCRIPTION_TOPIC_BATTERY_SINGLE_INFO_INDEX2 = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC,
                                                                                       47),

    /*!
     * @brief Please refer to ::T_DjiFcSubscriptionImuAttiNaviDataWithTimestamp for information about data structure.
     * @datastruct \ref T_DjiFcSubscriptionImuAttiNaviDataWithTimestamp
     */
    DJI_FC_SUBSCRIPTION_TOPIC_IMU_ATTI_NAVI_DATA_WITH_TIMESTAMP = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 48),

    /*!
     * @brief Provides postion NO.1 gimbal pitch, roll, yaw @ up to 50Hz
     * @details
     * The reference frame for gimbal angles is a NED frame attached to the gimbal.
     * This topic uses a data structure, Vector3f, that is too generic for the topic. The order of angles is :
     * |Data Structure Element| Meaning|
     * |----------------------|--------|
     * |Vector3f.x            |pitch   |
     * |Vector3f.y            |roll    |
     * |Vector3f.z            |yaw     |
     *
     * @perf
     * 0.1 deg accuracy in all axes
     *
     * @sensors Gimbal Encoder, IMU, Magnetometer
     * @units deg
     * @datastruct \ref T_DjiFcSubscriptionGimbalAngles
     * @also \ref TOPIC_GIMBAL_STATUS, \ref TOPIC_GIMBAL_CONTROL_MODE
     */
    DJI_FC_SUBSCRIPTION_TOPIC_GIMBAL_ANGLES_ON_POS_NO1 = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 48),

    /*!
     * @brief Provides postion NO.2 gimbal pitch, roll, yaw @ up to 50Hz
     * @details
     * The reference frame for gimbal angles is a NED frame attached to the gimbal.
     * This topic uses a data structure, Vector3f, that is too generic for the topic. The order of angles is :
     * |Data Structure Element| Meaning|
     * |----------------------|--------|
     * |Vector3f.x            |pitch   |
     * |Vector3f.y            |roll    |
     * |Vector3f.z            |yaw     |
     *
     * @perf
     * 0.1 deg accuracy in all axes
     *
     * @sensors Gimbal Encoder, IMU, Magnetometer
     * @units deg
     * @datastruct \ref T_DjiFcSubscriptionGimbalAngles
     * @also \ref TOPIC_GIMBAL_STATUS, \ref TOPIC_GIMBAL_CONTROL_MODE
     */
    DJI_FC_SUBSCRIPTION_TOPIC_GIMBAL_ANGLES_ON_POS_NO2 = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 49),

    /*!
     * @brief Provides postion NO.3 gimbal pitch, roll, yaw @ up to 50Hz
     * @details
     * The reference frame for gimbal angles is a NED frame attached to the gimbal.
     * This topic uses a data structure, Vector3f, that is too generic for the topic. The order of angles is :
     * |Data Structure Element| Meaning|
     * |----------------------|--------|
     * |Vector3f.x            |pitch   |
     * |Vector3f.y            |roll    |
     * |Vector3f.z            |yaw     |
     *
     * @perf
     * 0.1 deg accuracy in all axes
     *
     * @sensors Gimbal Encoder, IMU, Magnetometer
     * @units deg
     * @datastruct \ref T_DjiFcSubscriptionGimbalAngles
     * @also \ref TOPIC_GIMBAL_STATUS, \ref TOPIC_GIMBAL_CONTROL_MODE
     */
    DJI_FC_SUBSCRIPTION_TOPIC_GIMBAL_ANGLES_ON_POS_NO3 = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 50),

    /*!
     * @brief Provides postion NO.4 gimbal pitch, roll, yaw @ up to 50Hz
     * @details
     * The reference frame for gimbal angles is a NED frame attached to the gimbal.
     * This topic uses a data structure, Vector3f, that is too generic for the topic. The order of angles is :
     * |Data Structure Element| Meaning|
     * |----------------------|--------|
     * |Vector3f.x            |pitch   |
     * |Vector3f.y            |roll    |
     * |Vector3f.z            |yaw     |
     *
     * @perf
     * 0.1 deg accuracy in all axes
     *
     * @sensors Gimbal Encoder, IMU, Magnetometer
     * @units deg
     * @datastruct \ref T_DjiFcSubscriptionGimbalAngles
     * @also \ref TOPIC_GIMBAL_STATUS, \ref TOPIC_GIMBAL_CONTROL_MODE
     */
    DJI_FC_SUBSCRIPTION_TOPIC_GIMBAL_ANGLES_ON_POS_NO4 = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 51),

    /*!
     * @brief Provides postion NO.5 gimbal pitch, roll, yaw @ up to 50Hz
     * @details
     * The reference frame for gimbal angles is a NED frame attached to the gimbal.
     * This topic uses a data structure, Vector3f, that is too generic for the topic. The order of angles is :
     * |Data Structure Element| Meaning|
     * |----------------------|--------|
     * |Vector3f.x            |pitch   |
     * |Vector3f.y            |roll    |
     * |Vector3f.z            |yaw     |
     *
     * @perf
     * 0.1 deg accuracy in all axes
     *
     * @sensors Gimbal Encoder, IMU, Magnetometer
     * @units deg
     * @datastruct \ref T_DjiFcSubscriptionGimbalAngles
     * @also \ref TOPIC_GIMBAL_STATUS, \ref TOPIC_GIMBAL_CONTROL_MODE
     */
    DJI_FC_SUBSCRIPTION_TOPIC_GIMBAL_ANGLES_ON_POS_NO5 = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 52),

    /*!
     * @brief Provides postion NO.6 gimbal pitch, roll, yaw @ up to 50Hz
     * @details
     * The reference frame for gimbal angles is a NED frame attached to the gimbal.
     * This topic uses a data structure, Vector3f, that is too generic for the topic. The order of angles is :
     * |Data Structure Element| Meaning|
     * |----------------------|--------|
     * |Vector3f.x            |pitch   |
     * |Vector3f.y            |roll    |
     * |Vector3f.z            |yaw     |
     *
     * @perf
     * 0.1 deg accuracy in all axes
     *
     * @sensors Gimbal Encoder, IMU, Magnetometer
     * @units deg
     * @datastruct \ref T_DjiFcSubscriptionGimbalAngles
     * @also \ref TOPIC_GIMBAL_STATUS, \ref TOPIC_GIMBAL_CONTROL_MODE
     */
    DJI_FC_SUBSCRIPTION_TOPIC_GIMBAL_ANGLES_ON_POS_NO6 = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 53),

    /*!
     * @brief Provides postion NO.7 gimbal pitch, roll, yaw @ up to 50Hz
     * @details
     * The reference frame for gimbal angles is a NED frame attached to the gimbal.
     * This topic uses a data structure, Vector3f, that is too generic for the topic. The order of angles is :
     * |Data Structure Element| Meaning|
     * |----------------------|--------|
     * |Vector3f.x            |pitch   |
     * |Vector3f.y            |roll    |
     * |Vector3f.z            |yaw     |
     *
     * @perf
     * 0.1 deg accuracy in all axes
     *
     * @sensors Gimbal Encoder, IMU, Magnetometer
     * @units deg
     * @datastruct \ref T_DjiFcSubscriptionGimbalAngles
     * @also \ref TOPIC_GIMBAL_STATUS, \ref TOPIC_GIMBAL_CONTROL_MODE
     */
    DJI_FC_SUBSCRIPTION_TOPIC_GIMBAL_ANGLES_ON_POS_NO7 = DJI_DATA_SUBSCRIPTION_TOPIC(DJI_DATA_SUBSCRIPTION_MODULE_FC, 54),

    /*! Total number of topics that can be subscribed. */
    DJI_FC_SUBSCRIPTION_TOPIC_TOTAL_NUMBER,
} E_DjiFcSubscriptionTopic;

/**
 * @brief Health state of data subscribed.
 */
typedef enum {
    DJI_FC_SUBSCRIPTION_DATA_NOT_HEALTH = 0, /*!< Data subscribed is healthy and can be used. */
    DJI_FC_SUBSCRIPTION_DATA_HEALTH = 1, /*!< Data subscribed is not healthy and recommend not to use it. */
} E_DjiFcSubscriptionDataHealthFlag;

/**
 * @brief Position solution property.
 */
typedef enum {
    DJI_FC_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_NOT_AVAILABLE = 0, /*!< Position solution is not available. */
    DJI_FC_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_FIX_POSITION = 1, /*!< Position has been fixed by the FIX POSITION command. */
    DJI_FC_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_FIX_HEIGHT_AUTO = 2, /*!< Position has been fixed by the FIX HEIGHT/AUTO command. */
    DJI_FC_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_INSTANTANEOUS_DOPPLER_COMPUTE_VELOCITY = 8, /*!< Velocity computed using instantaneous Doppler. */
    DJI_FC_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_SINGLE_PNT_SOLUTION = 16, /*!< Single point position solution. */
    DJI_FC_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_PSEUDORANGE_DIFFERENTIAL_SOLUTION = 17, /*!< Pseudorange differential solution. */
    DJI_FC_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_SBAS_CORRECTION_CALCULATED = 18, /*!< Solution calculated using corrections from an SBAS. */
    DJI_FC_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_KALMAN_FILTER_WITHOUT_OBSERVATION_PROPAGATED = 19, /*!< Propagated by a Kalman filter without new observations. */
    DJI_FC_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_OMNISTAR_VBS_POSITION = 20, /*!< OmniSTAR VBS position (L1 sub-metre). */
    DJI_FC_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_FLOAT_L1_AMBIGUITY = 32, /*!< Floating L1 ambiguity solution. */
    DJI_FC_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_FLOAT_IONOSPHERIC_FREE_AMBIGUITY = 33, /*!< Floating ionospheric-free ambiguity solution. */
    DJI_FC_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_FLOAT_SOLUTION = 34, /*!< Float position solution. */
    DJI_FC_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_L1_AMBIGUITY_INT = 48, /*!< Integer L1 ambiguity solution. */
    DJI_FC_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_WIDE_LANE_AMBIGUITY_INT = 49, /*!< Integer wide-lane ambiguity solution. */
    DJI_FC_SUBSCRIPTION_POSITION_SOLUTION_PROPERTY_NARROW_INT = 50, /*!< Narrow fixed point position solution. */
} E_DjiFcSubscriptionPositionSolutionProperty;

/**
 * @brief GPS fix state.
 */
typedef enum {
    DJI_FC_SUBSCRIPTION_GPS_FIX_STATE_NO_FIX = 0, /*!< GPS position has not been fixed. */
    DJI_FC_SUBSCRIPTION_GPS_FIX_STATE_DEAD_RECKONING_ONLY = 1, /*!< GPS position is dead reckoned only. */
    DJI_FC_SUBSCRIPTION_GPS_FIX_STATE_2D_FIX = 2, /*!< The horizontal position with latitude/longitude (or northing/easting or X/Y) is fixed. */
    DJI_FC_SUBSCRIPTION_GPS_FIX_STATE_3D_FIX = 3, /*!< The horizontal and vertical position with latitude/longitude/altitude (northing/easting/altitude or X/Y/Z) is fixed. */
    DJI_FC_SUBSCRIPTION_GPS_FIX_STATE_GPS_PLUS_DEAD_RECKONING = 4, /*!< Position is calculated by GPS and combined with dead reckoning. */
    DJI_FC_SUBSCRIPTION_GPS_FIX_STATE_TIME_ONLY_FIX = 5, /*!< Only time is fixed. */
} E_DjiFcSubscriptionGpsFixState;

/**
 * @brief Flight status of aircraft.
 */
typedef enum {
    DJI_FC_SUBSCRIPTION_FLIGHT_STATUS_STOPED = 0, /*!< Aircraft is on ground and motors are still. */
    DJI_FC_SUBSCRIPTION_FLIGHT_STATUS_ON_GROUND = 1, /*!< Aircraft is on ground but motors are rotating. */
    DJI_FC_SUBSCRIPTION_FLIGHT_STATUS_IN_AIR = 2, /*!< Aircraft is in air. */
} E_DjiFcSubscriptionFlightStatus;

/*!
 * @brief "Mode" of the vehicle's state machine, as displayed on DJI Go
 * @details Available through Telemetry::TOPIC_STATUS_DISPLAYMODE
 */
typedef enum {
    /*! This mode requires the user to manually
     * control the aircraft to remain stable in air. */
    DJI_FC_SUBSCRIPTION_DISPLAY_MODE_MANUAL_CTRL = 0,
    /*! In this mode, the aircraft can keep
     * attitude stabilization and only use the
     * barometer for positioning to control the altitude. <br>
     * The aircraft can not autonomously locate and hover stably.*/
    DJI_FC_SUBSCRIPTION_DISPLAY_MODE_ATTITUDE = 1,
    DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_2 = 2,
    DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_3 = 3,
    DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_4 = 4,
    DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_5 = 5,
    /*! The aircraft is in normal GPS mode. <br>
     * In normal GPS mode, the aircraft can
     * autonomously locate and hover stably. <br>
     *  The sensitivity of the aircraft to the
     *  command response is moderate.
     */
    DJI_FC_SUBSCRIPTION_DISPLAY_MODE_P_GPS = 6,
    DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_7 = 7,
    DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_8 = 8,
    /*! In hotpoint mode */
    DJI_FC_SUBSCRIPTION_DISPLAY_MODE_HOTPOINT_MODE = 9,
    /*! In this mode, user can push the throttle
     * stick to complete stable take-off. */
    DJI_FC_SUBSCRIPTION_DISPLAY_MODE_ASSISTED_TAKEOFF = 10,
    /*! In this mode, the aircraft will autonomously
     * start motor, ascend and finally hover. */
    DJI_FC_SUBSCRIPTION_DISPLAY_MODE_AUTO_TAKEOFF = 11,
    /*! In this mode, the aircraft can land autonomously. */
    DJI_FC_SUBSCRIPTION_DISPLAY_MODE_AUTO_LANDING = 12,
    DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_13 = 13,
    DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_14 = 14,
    /*! In this mode, the aircraft can antonomously return the
     * last recorded Home Point. <br>
     * There are three types of this mode: Smart RTH(Return-to-Home),
     * Low Batterry RTH, and Failsafe RTTH.  */
    DJI_FC_SUBSCRIPTION_DISPLAY_MODE_NAVI_GO_HOME = 15,
    DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_16 = 16,
    /*! In this mode, the aircraft is controled by SDK API. <br>
     * User can directly define the control mode of horizon
     * and vertical directions and send control datas to aircraft. */
    DJI_FC_SUBSCRIPTION_DISPLAY_MODE_NAVI_SDK_CTRL = 17,
    DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_18 = 18,
    DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_19 = 19,
    DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_20 = 20,
    DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_21 = 21,
    DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_22 = 22,
    DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_23 = 23,
    DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_24 = 24,
    DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_25 = 25,
    DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_26 = 26,
    DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_27 = 27,
    DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_28 = 28,
    DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_29 = 29,
    DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_30 = 30,
    DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_31 = 31,
    DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_32 = 32,
    /*! drone is forced to land, might due to low battery */
    DJI_FC_SUBSCRIPTION_DISPLAY_MODE_FORCE_AUTO_LANDING = 33,
    DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_34 = 34,
    DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_35 = 35,
    DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_36 = 36,
    DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_37 = 37,
    DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_38 = 38,
    DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_39 = 39,
    /*! drone will search for the last position where the rc is not lost */
    DJI_FC_SUBSCRIPTION_DISPLAY_MODE_SEARCH_MODE = 40,
    /*! Mode for motor starting. <br>
     * Every time user unlock the motor, this will be the first mode. */
    DJI_FC_SUBSCRIPTION_DISPLAY_MODE_ENGINE_START = 41,
    DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_42 = 42,
    DJI_FC_SUBSCRIPTION_DISPLAY_MODE_RESERVED_43 = 42,
} E_DjiFcSubscriptionDisplayMode;

/**
 * @brief Setting status of homepoint.
 */
typedef enum {
    DJI_FC_SUBSCRIPTION_HOME_POINT_SET_STATUS_FAILED = 0, /*!< The home point was not set yet. */
    DJI_FC_SUBSCRIPTION_HOME_POINT_SET_STATUS_SUCCESS = 1, /*!< The home point was set successfully. */
} E_DjiFcSubscriptionHomePointSetStatus;

/**
 * @brief Smart battery self-check result.
 */
typedef enum {
    DJI_FC_SUBSCRIPTION_BATTERY_SELF_CHECK_NORMAL = 0, /*!< Battery self-check is normal. */
    DJI_FC_SUBSCRIPTION_BATTERY_SELF_CHECK_NTC_ABNORMAL = 1, /*!< Battery self-check NTC is abnormal. */
    DJI_FC_SUBSCRIPTION_BATTERY_SELF_CHECK_MOS_ABNORMAL = 2, /*!< Battery self-check MOS is abnormal. */
    DJI_FC_SUBSCRIPTION_BATTERY_SELF_CHECK_R_ABNORMAL = 3, /*!< Battery self-check sampling resistance is abnormal. */
    DJI_FC_SUBSCRIPTION_BATTERY_SELF_CHECK_CELL_DAMAGE = 4, /*!< Battery cell self-check is damaged. */
    DJI_FC_SUBSCRIPTION_BATTERY_SELF_CHECK_CAL_EXP = 5, /*!< Battery self-check is not calibrated. */
    DJI_FC_SUBSCRIPTION_BATTERY_SELF_CHECK_GAUGE_PARM_EXP = 6, /*!< Battery self-check fuel gauge parameters is abnormal. */
} E_DJIFcSubscriptionBatterySelfCheck;

/**
 * @brief Smart battery closed reason.
 */
typedef enum {
    DJI_FC_SUBSCRIPTION_BATTERY_CLOSED_NORMAL = 0, /*!< Battery closed reason is normal shutdown. */
    DJI_FC_SUBSCRIPTION_BATTERY_CLOSED_FORCED = 1, /*!< Battery closed reason is forced by flight controller shutdown. */
    DJI_FC_SUBSCRIPTION_BATTERY_CLOSED_SCD = 2, /*!< Battery closed reason is discharge short circuit shutdown. */
    DJI_FC_SUBSCRIPTION_BATTERY_CLOSED_OLD = 3, /*!< Battery closed reason is discharge overload shutdown. */
    DJI_FC_SUBSCRIPTION_BATTERY_CLOSED_OCD = 4, /*!< Battery closed reason is discharge over current shutdown. */
    DJI_FC_SUBSCRIPTION_BATTERY_CLOSED_UVD = 5, /*!< Battery closed reason is discharge under voltage shutdown. */
    DJI_FC_SUBSCRIPTION_BATTERY_CLOSED_OTD = 6, /*!< Battery closed reason is discharge over temperature shutdown. */
    DJI_FC_SUBSCRIPTION_BATTERY_CLOSED_SCC = 16, /*!< Battery closed reason is charging short circuit shutdown. */
    DJI_FC_SUBSCRIPTION_BATTERY_CLOSED_OCC = 17, /*!< Battery closed reason is charging over current shutdown. */
    DJI_FC_SUBSCRIPTION_BATTERY_CLOSED_OVC = 18, /*!< Battery closed reason is charging over voltage shutdown. */
    DJI_FC_SUBSCRIPTION_BATTERY_CLOSED_CHARGER_OVC = 19, /*!< Battery closed reason is charger over voltage shutdown. */
    DJI_FC_SUBSCRIPTION_BATTERY_CLOSED_LTC = 20, /*!< Battery closed reason is charging low temperature shutdown. */
    DJI_FC_SUBSCRIPTION_BATTERY_CLOSED_HTC = 21, /*!< Battery closed reason is charging high temperature shutdown. */
} E_DJIFcSubscriptionBatteryClosedReason;

/**
 * @brief Smart battery SOH state.
 */
typedef enum {
    DJI_FC_SUBSCRIPTION_BATTERY_SOH_NORMAL = 0, /*!< Battery SOH state is normal. */
    DJI_FC_SUBSCRIPTION_BATTERY_SOH_ALERT = 1, /*!< Battery SOH state is alert. */
    DJI_FC_SUBSCRIPTION_BATTERY_SOH_SAFE = 2, /*!< Battery SOH state is safe. */
} E_DJIFcSubscriptionBatterySohState;

/**
 * @brief Smart battery heart state.
 */
typedef enum {
    DJI_FC_SUBSCRIPTION_BATTERY_NO_HEAT = 0, /*!< Battery is in no heat state. */
    DJI_FC_SUBSCRIPTION_BATTERY_IN_HEAT = 1, /*!< Battery is in heat state. */
    DJI_FC_SUBSCRIPTION_BATTERY_KEEP_WARM = 2, /*!< Battery is in keep warm state. */
} E_DJIFcSubscriptionBatteryHeatState;

/**
 * @brief Smart battery SOC state.
 */
typedef enum {
    DJI_FC_SUBSCRIPTION_BATTERY_SOC_NORMAL = 0, /*!< Battery SOC state is normal. */
    DJI_FC_SUBSCRIPTION_BATTERY_SOC_ABNORMAL_HIGH = 1, /*!< Battery SOC state is abnormal high. */
    DJI_FC_SUBSCRIPTION_BATTERY_SOC_JUMP_DOWN = 2, /*!< Battery SOC state is jump down. */
    DJI_FC_SUBSCRIPTION_BATTERY_SOC_JUMP_UP = 3, /*!< Battery SOC state is jump up. */
    DJI_FC_SUBSCRIPTION_BATTERY_SOC_INVALID = 4, /*!< Battery SOC state is invalid. */
} E_DJIFcSubscriptionBatterySocState;

/**
 * @brief Flight control mode.
 */
typedef enum {
    DJI_FC_SUBSCRIPTION_CONTROL_MODE_UNKNOWN = 0,
    DJI_FC_SUBSCRIPTION_CONTROL_MODE_HOR_ANG_VER_VEL_YAW_ANG = 1,   /*!< Horizontal mode is angle, vertical mode is velocity, yaw mode is angle. */
    DJI_FC_SUBSCRIPTION_CONTROL_MODE_HOR_ANG_VER_VEL_YAW_RAT = 2,   /*!< Horizontal mode is angle, vertical mode is velocity, yaw mode is rate. */
    DJI_FC_SUBSCRIPTION_CONTROL_MODE_HOR_VEL_VER_VEL_YAW_ANG = 3,   /*!< Horizontal mode is velocity, vertical mode is velocity, yaw mode is angle. */
    DJI_FC_SUBSCRIPTION_CONTROL_MODE_HOR_VEL_VER_VEL_YAW_RAT = 4,   /*!< Horizontal mode is velocity, vertical mode is velocity, yaw mode is rate. */
    DJI_FC_SUBSCRIPTION_CONTROL_MODE_HOR_POS_VER_VEL_YAW_ANG = 5,   /*!< Horizontal mode is position, vertical mode is velocity, yaw mode is angle. */
    DJI_FC_SUBSCRIPTION_CONTROL_MODE_HOR_POS_VER_VEL_YAW_RAT = 6,   /*!< Horizontal mode is position, vertical mode is velocity, yaw mode is rate. */
    DJI_FC_SUBSCRIPTION_CONTROL_MODE_HOR_ANG_VER_POS_YAW_ANG = 7,   /*!< Horizontal mode is angle, vertical mode is position, yaw mode is angle. */
    DJI_FC_SUBSCRIPTION_CONTROL_MODE_HOR_ANG_VER_POS_YAW_RAT = 8,   /*!< Horizontal mode is angle, vertical mode is position, yaw mode is rate. */
    DJI_FC_SUBSCRIPTION_CONTROL_MODE_HOR_VEL_VER_POS_YAW_ANG = 9,   /*!< Horizontal mode is velocity, vertical mode is position, yaw mode is angle. */
    DJI_FC_SUBSCRIPTION_CONTROL_MODE_HOR_VEL_VER_POS_YAW_RAT = 10,  /*!< Horizontal mode is velocity, vertical mode is position, yaw mode is rate. */
    DJI_FC_SUBSCRIPTION_CONTROL_MODE_HOR_POS_VER_POS_YAW_ANG = 11,  /*!< Horizontal mode is position, vertical mode is position, yaw mode is angle. */
    DJI_FC_SUBSCRIPTION_CONTROL_MODE_HOR_POS_VER_POS_YAW_RAT = 12,  /*!< Horizontal mode is position, vertical mode is position, yaw mode is rate. */
    DJI_FC_SUBSCRIPTION_CONTROL_MODE_HOR_ANG_VER_THR_YAW_ANG = 13,  /*!< Horizontal mode is angle, vertical mode is thrust, yaw mode is angle. */
    DJI_FC_SUBSCRIPTION_CONTROL_MODE_HOR_ANG_VER_THR_YAW_RAT = 14,  /*!< Horizontal mode is angle, vertical mode is thrust, yaw mode is rate. */
    DJI_FC_SUBSCRIPTION_CONTROL_MODE_HOR_VEL_VER_THR_YAW_ANG = 15,  /*!< Horizontal mode is velocity, vertical mode is thrust, yaw mode is angle. */
    DJI_FC_SUBSCRIPTION_CONTROL_MODE_HOR_VEL_VER_THR_YAW_RAT = 16,  /*!< Horizontal mode is velocity, vertical mode is thrust, yaw mode is rate. */
    DJI_FC_SUBSCRIPTION_CONTROL_MODE_HOR_POS_VER_THR_YAW_ANG = 17,  /*!< Horizontal mode is position, vertical mode is thrust, yaw mode is angle. */
    DJI_FC_SUBSCRIPTION_CONTROL_MODE_HOR_POS_VER_THR_YAW_RAT = 18,  /*!< Horizontal mode is position, vertical mode is thrust, yaw mode is rate. */
    DJI_FC_SUBSCRIPTION_CONTROL_MODE_HOR_AEL_VER_VEL_YAW_RAT = 19,  /*!< Horizontal mode is angle-rate, vertical mode is velocity, yaw mode is rate. */
    DJI_FC_SUBSCRIPTION_CONTROL_MODE_HOR_AEL_VER_POS_YAW_RAT = 20,  /*!< Horizontal mode is angle-rate, vertical mode is thrust, yaw mode is rate. */
    DJI_FC_SUBSCRIPTION_CONTROL_MODE_HOR_AEL_VER_THR_YAW_RAT = 21,  /*!< Horizontal mode is angle-rate, vertical mode is velocity, yaw mode is rate. */
} E_DJIFcSubscriptionControlMode;

/**
 * @brief Flight control authority.
 */
typedef enum {
    DJI_FC_SUBSCRIPTION_CONTROL_AUTHORITY_RC = 0, /*!< Authority is in remote control */
    DJI_FC_SUBSCRIPTION_CONTROL_AUTHORITY_MSDK = 1, /*!< Authority is in MSDK */
    DJI_FC_SUBSCRIPTION_CONTROL_AUTHORITY_PSDK = 4, /*!< Authority is in PSDK */
    DJI_FC_SUBSCRIPTION_CONTROL_AUTHORITY_DOCK = 5, /*!< Authority is in dock */
} E_DJIFcSubscriptionControlAuthority;

/**
 * @brief Flight control authority change reason.
 */
typedef enum {
    DJI_FC_SUBSCRIPTION_AUTHORITY_CHANGE_REASON_UNKNOWN = 0, /*!< Reason unknown */
    DJI_FC_SUBSCRIPTION_AUTHORITY_CHANGE_REASON_MSDK_REQUEST = 1, /*!< Contro authority changed by MSDK request. */
    DJI_FC_SUBSCRIPTION_AUTHORITY_CHANGE_REASON_USER_REQUEST = 2, /*!< Contro authority changed by user request. */
    DJI_FC_SUBSCRIPTION_AUTHORITY_CHANGE_REASON_PSDK_REQUEST = 3, /*!< Contro authority changed by PSDK request. */
    DJI_FC_SUBSCRIPTION_AUTHORITY_CHANGE_REASON_RC_LOST = 4, /*!< Contro authority changed for remote control lost. */
    DJI_FC_SUBSCRIPTION_AUTHORITY_CHANGE_REASON_RC_NOT_P_MODE = 5, /*!< Contro authority changed for remote control not in P mode. */
    DJI_FC_SUBSCRIPTION_AUTHORITY_CHANGE_REASON_RC_SWITCH = 6, /*!< Contro authority changed for remote control switching mode. */
    DJI_FC_SUBSCRIPTION_AUTHORITY_CHANGE_REASON_RC_PAUSE_STOP = 7, /*!< Contro authority changed for remote control stop key paused. */
    DJI_FC_SUBSCRIPTION_AUTHORITY_CHANGE_REASON_RC_ONE_KEY_GO_HOME = 8, /*!< Contro authority changed for remote control go-home key paused. */
    DJI_FC_SUBSCRIPTION_AUTHORITY_CHANGE_REASON_BATTERY_LOW_GO_HOME = 9, /*!< Contro authority changed for remote control go-home key paused. */
    DJI_FC_SUBSCRIPTION_AUTHORITY_CHANGE_REASON_BATTERY_SUPER_LOW_LANDING = 10, /*!< Contro authority changed for going home caused by low batter power. */
    DJI_FC_SUBSCRIPTION_AUTHORITY_CHANGE_REASON_PSDK_LOST = 11, /*!< Contro authority changed for PSDK lost. */
    DJI_FC_SUBSCRIPTION_AUTHORITY_CHANGE_REASON_NEAR_BOUNDARY = 13, /*!< Contro authority changed for nearing boundary. */
    DJI_FC_SUBSCRIPTION_AUTHORITY_CHANGE_REASON_AIRPORT_REQUEST = 14, /*!< Contro authority changed by airport request. */
} E_DJIFcSubscriptionAuthorityChangeReason;

#pragma pack(1)

/**
 * @brief Quaternion topic data structure.
 */
typedef struct Quaternion {
    dji_f32_t q0; /*!< w, rad (when converted to a rotation matrix or Euler angles). */
    dji_f32_t q1; /*!< x, rad (when converted to a rotation matrix or Euler angles). */
    dji_f32_t q2; /*!< y, rad (when converted to a rotation matrix or Euler angles). */
    dji_f32_t q3; /*!< z, rad (when converted to a rotation matrix or Euler angles). */
} T_DjiFcSubscriptionQuaternion;

/**
 * @brief DJI_FC_SUBSCRIPTION_TOPIC_ACCELERATION_GROUND topic data structure.
 */
typedef T_DjiVector3f T_DjiFcSubscriptionAccelerationGround;

/**
 * @brief DJI_FC_SUBSCRIPTION_TOPIC_ACCELERATION_BODY topic data structure.
 */
typedef T_DjiVector3f T_DjiFcSubscriptionAccelerationBody;

/**
 * @brief DJI_FC_SUBSCRIPTION_TOPIC_ACCELERATION_RAW topic data structure.
 */
typedef T_DjiVector3f T_DjiFcSubscriptionAccelerationRaw;

/**
 * @brief DJI_FC_SUBSCRIPTION_TOPIC_VELOCITY topic data structure.
 */
typedef struct Velocity {
    /*! Velocity of aircraft. */
    T_DjiVector3f data;
    /*! Health state of aircraft velocity data. It can be any value of ::E_DjiFcSubscriptionDataHealthFlag. */
    uint8_t health: 1;
    /*! Reserved. */
    uint8_t reserve: 7;
} T_DjiFcSubscriptionVelocity;

/**
 * @brief DJI_FC_SUBSCRIPTION_TOPIC_ANGULAR_RATE_FUSIONED topic data structure.
 */
typedef T_DjiVector3f T_DjiFcSubscriptionAngularRateFusioned;

/**
 * @brief DJI_FC_SUBSCRIPTION_TOPIC_ANGULAR_RATE_RAW topic data structure.
 */
typedef T_DjiVector3f T_DjiFcSubscriptionAngularRateRaw;

/**
 * @brief DJI_FC_SUBSCRIPTION_TOPIC_ANGULAR_RATE_RAW topic data structure.
 */
typedef dji_f32_t T_DjiFcSubscriptionAltitudeFused;

/**
 * @brief DJI_FC_SUBSCRIPTION_TOPIC_ALTITUDE_BAROMETER topic data structure.
 */
typedef dji_f32_t T_DjiFcSubscriptionAltitudeBarometer;

/**
 * @brief DJI_FC_SUBSCRIPTION_TOPIC_ALTITUDE_OF_HOMEPOINT topic data structure.
 */
typedef dji_f32_t T_DjiFcSubscriptionAltitudeOfHomePoint;

/**
 * @brief DJI_FC_SUBSCRIPTION_TOPIC_HEIGHT_FUSION topic data structure.
 */
typedef dji_f32_t T_DjiFcSubscriptionHeightFusion;

/**
 * @brief Relative height above ground topic data structure, unit: m.
 */
typedef dji_f32_t T_DjiFcSubscriptionHeightRelative;

/**
 * @brief Fused position topic data structure.
 */
typedef struct PositionFused {
    dji_f64_t longitude; /*!< Longitude, unit: rad. */
    dji_f64_t latitude; /*!< Latitude, unit: rad. */
    dji_f32_t altitude; /*!< Altitude, WGS 84 reference ellipsoid, unit: m. */
    uint16_t visibleSatelliteNumber; /*!< Number of visible satellites. */
} T_DjiFcSubscriptionPositionFused;

/**
 * @brief GPS date topic data structure, format: yyyymmdd.
 */
typedef uint32_t T_DjiFcSubscriptionGpsDate;

/**
 * @brief GPS time topic data structure, format: hhmmss.
 */
typedef uint32_t T_DjiFcSubscriptionGpsTime;

/**
 * @brief GPS position topic data structure. x = Longitude, y = Latitude, z = Altitude, unit: deg*10<SUP>-7</SUP>
 * (Lat,Lon), mm (Alt)
 */
typedef T_DjiVector3d T_DjiFcSubscriptionGpsPosition;

/**
 * @brief GPS velocity topic data structure, unit: cm/s.
 */
typedef T_DjiVector3f T_DjiFcSubscriptionGpsVelocity;

/**
 * @brief GPS details topic data structure.
 */
typedef struct GpsDetail {
    dji_f32_t hdop; /*!< Horizontal dilution of precision, unit: 0.01, eg: 100 = 1.00, <1: ideal, 1-2: excellent, 2-5: good, 5-10: moderate, 10-20: fair, >20: poor. */
    dji_f32_t pdop; /*!< Position dilution of precision, unit: 0.01, eg: 100 = 1.00, <1: ideal, 1-2: excellent, 2-5: good, 5-10: moderate, 10-20: fair, >20: poor. */
    dji_f32_t fixState; /*!< GPS fix state, and can be any value of ::E_DjiFcSubscriptionGpsFixState. Value other than ::E_DjiFcSubscriptionGpsFixState is invalid. */
    dji_f32_t vacc; /*!< Vertical position accuracy (mm), the smaller, the better. */
    dji_f32_t hacc; /*!< Horizontal position accuracy (mm), the smaller, the better. */
    dji_f32_t sacc; /*!< Speed accuracy (cm/s), the smaller, the better. */
    uint32_t gpsSatelliteNumberUsed; /*!< Number of GPS satellites used for fixing position. */
    uint32_t glonassSatelliteNumberUsed; /*!< Number of GLONASS satellites used for fixing position. */
    uint16_t totalSatelliteNumberUsed; /*!< Total number of satellites used for fixing position. */
    uint16_t gpsCounter; /*!< Accumulated times of sending GPS data. */
} T_DjiFcSubscriptionGpsDetails;

/**
 * @brief GPS signal level topic data structure. Signal level of GPS. The level varies from 0 to 5, with 0 being the
 * worst and 5 the best GPS signal.
 */
typedef uint8_t T_DjiFcSubscriptionGpsSignalLevel;

/**
 * @brief RTK position topic data structure.
 */
typedef struct PositionData {
    dji_f64_t longitude; /*!< Longitude, unit: deg. */
    dji_f64_t latitude;  /*!< Latitude, unit: deg. */
    dji_f32_t hfsl;      /*!< Height above mean sea level, unit: m. */
} T_DjiFcSubscriptionRtkPosition;

/**
 * @brief RTK velocity topic data structure, unit: cm/s.
 */
typedef T_DjiVector3f T_DjiFcSubscriptionRtkVelocity;

/**
 * @brief RTK yaw topic data structure, unit: deg.
 */
typedef int16_t T_DjiFcSubscriptionRtkYaw;

/**
 * @brief RTK position information topic data structure. Specifies RTK position solution state, it can be any value of
 * ::E_DjiFcSubscriptionPositionSolutionProperty.
 */
typedef uint8_t T_DjiFcSubscriptionRtkPositionInfo;

/**
 * @brief RTK yaw information topic data structure. Specifies RTK yaw solution state, it can be any value of
 * ::E_DjiFcSubscriptionPositionSolutionProperty.
 */
typedef uint8_t T_DjiFcSubscriptionRtkYawInfo;

/*!
 * @brief struct for data broadcast, return magnetometer reading
 *
 * @note returned value is calibrated mag data,
 * 1000 < |mag| < 2000 for normal operation
 */
typedef struct Mag {
    int16_t x;
    int16_t y;
    int16_t z;
} T_DjiFcSubscriptionCompass;

/*!
 * @brief struct for data broadcast and data subscription, return RC reading
 */
typedef struct RC {
    int16_t roll;     /*!< [-10000,10000] */
    int16_t pitch;    /*!< [-10000,10000] */
    int16_t yaw;      /*!< [-10000,10000] */
    int16_t throttle; /*!< [-10000,10000] */
    int16_t mode;     /*!< [-10000,10000] */
    /*!< M100 [P: -8000, A: 0, F: 8000] */
    int16_t gear;     /*!< [-10000,10000] */
    /*!< M100 [Up: -10000, Down: -4545] */
} T_DjiFcSubscriptionRC;

typedef T_DjiVector3f T_DjiFcSubscriptionGimbalAngles;

/*!
 * @brief struct for TOPIC_GIMBAL_STATUS
 */
typedef struct GimbalStatus {
    uint32_t mountStatus: 1; /*!< 1 - gimbal mounted, 0 - gimbal not mounted*/
    uint32_t isBusy: 1;
    uint32_t pitchLimited: 1;           /*!< 1 - axis reached limit, 0 - no */
    uint32_t rollLimited: 1;            /*!< 1 - axis reached limit, 0 - no */
    uint32_t yawLimited: 1;             /*!< 1 - axis reached limit, 0 - no */
    uint32_t calibrating: 1;            /*!< 1 - calibrating, 0 - no */
    uint32_t prevCalibrationgResult: 1; /*!< 1 - success, 0 - fail */
    uint32_t installedDirection: 1;     /*!< 1 - reversed for OSMO, 0 - normal */
    uint32_t disabled_mvo: 1;
    uint32_t gear_show_unable: 1;
    uint32_t gyroFalut: 1;       /*!< 1 - fault, 0 - normal */
    uint32_t escPitchStatus: 1;  /*!< 1 - Pitch data is normal, 0 - fault */
    uint32_t escRollStatus: 1;   /*!< 1 - Roll data is normal, 0 - fault */
    uint32_t escYawStatus: 1;    /*!< 1 - Yaw data is normal , 0 - fault */
    uint32_t droneDataRecv: 1;   /*!< 1 - normal , 0 - at fault */
    uint32_t initUnfinished: 1;  /*!< 1 - init complete, 0 - not complete */
    uint32_t FWUpdating: 1;      /*!< 1 - updating, 0 - not updating */
    uint32_t reserved2: 15;
} T_DjiFcSubscriptionGimbalStatus;

/**
 * @brief Flight status information topic data structure. It can be any value of ::E_DjiFcSubscriptionFlightStatus.
 */
typedef uint8_t T_DjiFcSubscriptionFlightStatus;

typedef uint8_t T_DjiFcSubscriptionDisplaymode;

typedef uint8_t T_DjiFcSubscriptionLandinggear;

typedef uint16_t T_DjiFcSubscriptionMotorStartError;

typedef struct {
    uint32_t reserved: 12;
    uint32_t cellBreak: 5;            /*!< 0: normal; other: under voltage core index(0x01-0x1F). */
    uint32_t selfCheckError: 3;       /*!< enum-type: E_DJIFcSubscriptionBatterySelfCheck. */
    uint32_t reserved1: 7;
    uint32_t batteryClosedReason: 5;  /*!< enum-type: E_DJIFcSubscriptionBatteryClosedReason. */
    uint8_t reserved2: 6;             /*!< 0: CHG state; 1: DSG state; 2: ORING state. */
    uint8_t batSOHState: 2;           /*!< enum-type: E_DJIFcSubscriptionBatterySohState. */
    uint8_t maxCycleLimit: 6;         /*!< APP: cycle_limit*10. */
    uint8_t reserved3: 2;
    uint16_t lessBattery: 1;
    uint16_t batteryCommunicationAbnormal: 1;
    uint16_t reserved4: 3;
    uint16_t hasCellBreak: 1;
    uint16_t reserved5: 4;
    uint16_t isBatteryEmbed: 1;       /*!< 0:embed; 1:not embed. */
    uint16_t heatState: 2;            /*!< enum-type: E_DJIFcSubscriptionBatteryHeatState. */
    uint16_t socState: 3;             /*!< enum-type: E_DJIFcSubscriptionBatterySocState. */
} T_DjiFcSubscriptionSingleBatteryState;

/**
 * @brief Battery information topic data structure.
 */
typedef struct BatteryWholeInfo {
    uint32_t capacity; /*!< Battery capacity, unit: mAh. */
    int32_t voltage; /*!< Battery voltage, unit: mV. */
    int32_t current; /*!< Battery current, unit: mA. */
    uint8_t percentage; /*!< Battery capacity percentage, unit: 1%. */
} T_DjiFcSubscriptionWholeBatteryInfo;

typedef struct BatterySingleInfo {
    uint8_t reserve;
    uint8_t batteryIndex;
    int32_t currentVoltage;          /*!< uint: mV. */
    int32_t currentElectric;         /*!< uint: mA. */
    uint32_t fullCapacity;           /*!< uint: mAh. */
    uint32_t remainedCapacity;       /*!< uint: mAh. */
    int16_t batteryTemperature;      /*!< uint: 0.1℃. */
    uint8_t cellCount;
    uint8_t batteryCapacityPercent;  /*!< uint: %. */
    T_DjiFcSubscriptionSingleBatteryState batteryState;
    uint8_t reserve1;
    uint8_t reserve2;
    uint8_t SOP;                     /*!< Relative power percentage. */
} T_DjiFcSubscriptionSingleBatteryInfo;

/*!
 * @brief struct for TOPIC_CONTROL_DEVICE and data broadcast, return SDK info
 */
typedef struct SDKCtrlInfo {
    union {
        /* Used by M300 & M350 */
        struct {
            uint8_t controlMode;     /*!< enum-type: E_DJIFcSubscriptionControlMode. */
            uint8_t deviceStatus: 3; /*!< 0->rc  1->app  4->psdk */
            uint8_t flightStatus: 1; /*!< 1->open  0->closed */
            uint8_t vrcStatus: 1;
            uint8_t reserved: 3;
        };

        /* Used by other aircrafts */
        struct {
            uint8_t controlAuthority;      /*!< enum-type: E_DJIFcSubscriptionControlAuthority. */
            uint8_t controlAuthorityChangeReason;     /*!< enum-type: E_DJIFcSubscriptionAuthorityChangeReason. */
        };
    };
} T_DjiFcSubscriptionControlDevice; // pack(1)

/*!
 * @brief sub struct for TOPIC_HARD_SYNC
 */
typedef struct SyncTimestamp {
    uint32_t time2p5ms; /*!< clock time in multiples of 2.5ms. Sync timer runs at
                         400Hz, this field increments in integer steps */
    uint32_t time1ns;   /*!< nanosecond time offset from the 2.5ms pulse */
    uint32_t resetTime2p5ms; /*!< clock time in multiple of 2.5ms elapsed since the
                              hardware sync started */
    uint16_t index;   /*!< This is the tag field you filled out when using the
                       setSyncFreq API above; use it to identify the packets that
                       have sync data. This is useful when you call the
                       setSyncFreq API with freqInHz = 0, so you get a single
                       pulse that can be uniquely identified with a tag - allowing
                       you to create your own pulse train with uniquely
                       identifiable pulses. */
    uint8_t flag;     /*!< This is true when the packet corresponds to a hardware
                       pulse and false otherwise. This is useful because you can
                       request the software packet to be sent at a higher frequency
                       that the hardware line.*/
} SyncTimestamp;    // pack(1)

/*!
 * @brief struct for TOPIC_HARD_SYNC
 */
typedef struct HardSyncData {
    SyncTimestamp ts;  /*!< time stamp for the incoming data */
    struct Quaternion q;  /*!< quaternion */
    T_DjiVector3f a;  /*!< accelerometer reading unit: g */
    T_DjiVector3f w;  /*!< gyro reading unit: rad/sec */
} T_DjiFcSubscriptionHardSync;

typedef uint8_t T_DjiFcSubscriptionGpsControlLevel;

/*!
 * @brief struct for TOPIC_RC_WITH_FLAG_DATA
 */
typedef struct RCWithFlagData {
    dji_f32_t pitch;       /*!< down = -0.999, middle = 0.000, up   =0.999 */
    dji_f32_t roll;        /*!< left = -0.999, middle = 0.000, right=0.999 */
    dji_f32_t yaw;         /*!< left = -0.999, middle = 0.000, right=0.999 */
    dji_f32_t throttle;    /*!< down = -0.999, middle = 0.000, up   =0.999 */
    struct {
        uint8_t logicConnected: 1;  /*!< 0 if sky or ground side is disconnected for 3 seconds   */
        uint8_t skyConnected: 1;  /*!< Sky side is connected, i.e., receiver is connected to FC */
        uint8_t groundConnected: 1;  /*!< Ground side is connected, i.e., RC is on and connected to FC */
        uint8_t appConnected: 1;  /*!< Mobile App is connected to RC */
        uint8_t reserved: 4;
    } flag;
} T_DjiFcSubscriptionRCWithFlagData;

/*!
 * @brief struct for status of each individual esc
 */
typedef struct ESCStatusIndividual {
    int16_t current;              /*!< ESC current, unit: mA */
    int16_t speed;                /*!< ESC speed, unit: rpm */
    uint16_t voltage;              /*!< Input power from battery to ESC, unit: mV */
    int16_t temperature;          /*!< ESC temperature, unit: degree C */
    uint16_t stall: 1; /*!< Motor is stall */
    uint16_t empty: 1; /*!< Motor has no load */
    uint16_t unbalanced: 1; /*!< Motor speed is unbalanced */
    uint16_t escDisconnected: 1; /*!< ESC is disconnected */
    uint16_t temperatureHigh: 1; /*!< Temperature is high */
    uint16_t reserved: 11;
} ESCStatusIndividual;

/*!
 * @brief struct for TOPIC_ESC_DATA
 */
typedef struct EscData {
    ESCStatusIndividual esc[8];
} T_DjiFcSubscriptionEscData;

/*!
 * @brief struct indicating RTK GPS Connection
 */
typedef struct RTKConnectStatus {
    uint16_t rtkConnected: 1;
    uint16_t reserve: 15;
} T_DjiFcSubscriptionRTKConnectStatus;

typedef uint8_t T_DjiFcSubscriptionGimbalControlMode;

/*!
 * @brief struct for TOPIC_FLIGHT_ANOMALY
 */
typedef struct FlightAnomaly {
    uint32_t impactInAir: 1;  /*!< 0: No impact,                      1: Impact happens in Air */
    uint32_t randomFly: 1;  /*!< 0: Normal,                         1: Randomly fly in GPS mode without stick input*/
    uint32_t heightCtrlFail: 1;  /*!< 0: Height control normal,          1: Height control failed */
    uint32_t rollPitchCtrlFail: 1;  /*!< 0: Tilt control normal,            1: Tilt control failed */
    uint32_t yawCtrlFail: 1;  /*!< 0: Yaw control normal,             1: Yaw control failed */
    uint32_t aircraftIsFalling: 1;  /*!< 0: Aircraft is not falling,        1: Aircraft is falling */
    uint32_t strongWindLevel1: 1;  /*!< 0: Wind is under big wind level 1, 1: wind is stronger than  big wind level 1*/
    uint32_t strongWindLevel2: 1;  /*!< 0: Wind is under big wind level 2, 1: wind is stronger than  big wind level 2*/
    uint32_t compassInstallationError: 1;  /*!< 0: Compass install right,          1: Compass install error */
    uint32_t imuInstallationError: 1;  /*!< 0: IMU install right,              1: IMU install error */
    uint32_t escTemperatureHigh: 1;  /*!< 0: ESC temperature is normal,      1: ESC temperature is high */
    uint32_t atLeastOneEscDisconnected: 1;  /*!< 0: No ESC disconnected,            1: At least one ESC is disconnected */
    uint32_t gpsYawError: 1;  /*!< 0: No GPS yaw error,               1: GPS yaw error */
    uint32_t reserved: 19;
} T_DjiFcSubscriptionFlightAnomaly;

/*!
 * @brief struct for TOPIC_POSITION_VO
 */
typedef struct PositionVO {
    dji_f32_t x;              /*!< North (best effort), unit: m */
    dji_f32_t y;              /*!< East (best effort),  unit: m */
    dji_f32_t z;              /*!< Down,  unit: m */
    uint8_t xHealth: 1;
    uint8_t yHealth: 1;
    uint8_t zHealth: 1;
    uint8_t reserved: 5;
} T_DjiFcSubscriptionPositionVO;

/*!
 * @brief struct for data broadcast and subscription, return obstacle info around the vehicle
 *
 */
typedef struct RelativePosition {
    dji_f32_t down;            /*!< distance from obstacle (m) */
    dji_f32_t front;           /*!< distance from obstacle (m) */
    dji_f32_t right;           /*!< distance from obstacle (m) */
    dji_f32_t back;            /*!< distance from obstacle (m) */
    dji_f32_t left;            /*!< distance from obstacle (m) */
    dji_f32_t up;              /*!< distance from obstacle (m) */
    uint8_t downHealth: 1;  /*!< Down sensor flag: 0 - not working, 1 - working */
    uint8_t frontHealth: 1; /*!< Front sensor flag: 0 - not working, 1 - working */
    uint8_t rightHealth: 1; /*!< Right sensor flag: 0 - not working, 1 - working */
    uint8_t backHealth: 1;  /*!< Back sensor flag: 0 - not working, 1 - working */
    uint8_t leftHealth: 1;  /*!< Left sensor flag: 0 - not working, 1 - working */
    uint8_t upHealth: 1;    /*!< Up sensor health flag: 0 - not working, 1 - working */
    uint8_t reserved: 2;    /*!< Reserved sensor health flag*/
} T_DjiFcSubscriptionAvoidData;

/**
 * @brief Struct for the topic DJI_FC_SUBSCRIPTION_TOPIC_HOME_POINT_SET_STATUS. Specifies homepoint was set or not,
 * it can be any value of ::E_DjiFcSubscriptionHomePointSetStatus.
 */
typedef uint8_t T_DjiFcSubscriptionHomePointSetStatus;

/**
 * @brief Struct for the topic DJI_FC_SUBSCRIPTION_TOPIC_HOME_POINT_INFO. Specifies homepoint.
 */
typedef struct HomeLocationData {
    dji_f64_t latitude;  /*!< unit: rad */
    dji_f64_t longitude; /*!< unit: rad */
} T_DjiFcSubscriptionHomePointInfo; // pack(1)

typedef struct GimbalSingleData {
    dji_f32_t roll;
    dji_f32_t pitch;
    dji_f32_t yaw;
} GimbalAnglesData;

/**
 * @brief Struct for the topic DJI_FC_SUBSCRIPTION_TOPIC_THREE_GIMBAL_DATA. Used in M300
 */
typedef struct GimbalThreeData {
    GimbalAnglesData anglesData[3];
} T_DjiFcSubscriptionThreeGimbalData;

/**
 * @brief Struct for the topic DJI_FC_SUBSCRIPTION_TOPIC_IMU_ATTI_NAVI_DATA_WITH_TIMESTAMP. Used in M300
 */
typedef struct ImuAttiNaviDataWithTimestamp {
    uint16_t version;
    uint16_t flag;
    dji_f32_t pn_x;
    dji_f32_t pn_y;
    dji_f32_t pn_z;
    dji_f32_t vn_x;
    dji_f32_t vn_y;
    dji_f32_t vn_z;
    dji_f32_t an_x;
    dji_f32_t an_y;
    dji_f32_t an_z;
    dji_f32_t q[4];
    uint16_t resv;
    uint16_t cnt;
    uint32_t timestamp;
} T_DjiFcSubscriptionImuAttiNaviDataWithTimestamp;

#pragma pack()

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Initialise data subscription module in blocking mode. This function has to be called before subscribing any
 * data, to initialize run environment of data subscription module, if need to subscribe data from aircraft.
 * @note Max execution time of this function is slightly larger than 500ms.
 * @note This function has to be called in user task, rather than main() function, and after scheduler being started.
 * @return Execution result.
 */
T_DjiReturnCode DjiFcSubscription_Init(void);

/**
 * @brief Deinitialize data subscription module. When data subscription will no longer be used, can call this function
 * to deinitialize the data subscription module. Deinitialization function will help to release all system resource data
 * subscription has occupied.
 * @return Execution result.
 */
T_DjiReturnCode DjiFcSubscription_DeInit(void);

/**
 * @brief Subscribe a topic in blocking mode. Before subscribing any data from aircraft, DjiFcSubscription_Init()
 * function has to be called.
 * @details User can subscribe a topic by specifying topic name, push frequency and callback function used to receive
 * data of topic (if needed). After subscribing successfully, the user can call
 * DjiFcSubscription_GetLatestValueOfTopic() function to get the latest data of the topic have been
 * subscribed and the corresponding timestamp when aircraft sends the data out, and the callback function will be called to
 * push data of the topic and corresponding timestamp if the callback function is specified.
 * @note Max execution time of this function is slightly larger than 1200ms.
 * @note Topic to be subscribed can not have been subscribed, that is, one topic can not be subscribed repeatedly.
 * @note User must ensure that types of subscription frequency of all topics have been subscribed is less than or
 * equal to 4. Otherwise, the subscribing topic will fail.
 * @note User must ensure that the data length sum of all topics of the same subscription frequency is less than or equal to 242.
 * @param topic: topic name to be subscribed.
 * @param frequency: subscription frequency of topic to be subscribed. Subscription frequency can not beyond max
 * frequency limitation of the topic and must be the value of enum E_DjiFcSubscriptionTopicFreq. And, subscription
 * frequency has to be larger than 0. Users can find max frequency of topics in data subscription part of documentation
 * on developer website (developer.dji.com).
 * @param callback: callback function used to receive data of topic to be subscribed. If the callback function is not needed,
 * this item can be set as NULL.
 * @return Execution result.
 */
T_DjiReturnCode DjiFcSubscription_SubscribeTopic(E_DjiFcSubscriptionTopic topic,
                                                 E_DjiDataSubscriptionTopicFreq frequency,
                                                 DjiReceiveDataOfTopicCallback callback);

/**
 * @brief Unsubscribe a topic in blocking mode. Before unsubscribing any data from aircraft, DjiFcSubscription_Init()
 * and DjiFcSubscription_SubscribeTopic() function has to be called,
 * @details User can unsubscribe a topic by specifying topic name, but the topic must be unsubscribed in the order of subscription.
 * @note Topic to be unsubscribed must have been subscribed.
 * @note Topic to be unsubscribed must be in the order of subscription, similar to a queue, subscription is similar to enqueue,
 * unsubscription is similar to dequeue.
 * @param topic: topic name to be unsubscribed.
 * @return Execution result.
 */
T_DjiReturnCode DjiFcSubscription_UnSubscribeTopic(E_DjiFcSubscriptionTopic topic);

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
 * overflow event. If the user does not need timestamp information, can fill in NULL. Use flight controller power-on
 * timestamp on M300 RTK. Use payload local timestamp on M30/M30T.
 * @return Execution result.
 */
T_DjiReturnCode DjiFcSubscription_GetLatestValueOfTopic(E_DjiFcSubscriptionTopic topic,
                                                        uint8_t *data, uint16_t dataSizeOfTopic,
                                                        T_DjiDataTimestamp *timestamp);
#ifdef __cplusplus
}
#endif

#endif // DJI_FC_SUBSCRIPTION_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
