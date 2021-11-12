/**
 ********************************************************************
 * @file    psdk_xport.h
 * @brief   XPort module provides some interfaces used to control XPort and get states of XPort. The module can be used
 * only when the PSDK adapter is XPort, rather than Skyport.
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
#ifndef PSDK_XPORT_H
#define PSDK_XPORT_H

/* Includes ------------------------------------------------------------------*/
#include "legacy_psdk2.x/psdk_typedef.h"
#include "legacy_psdk2.x/psdk_gimbal.h"
#include "dji_xport.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/
/**
 * @brief XPort limit angle category.
 */
typedef E_DjiXPortLimitAngleCategory E_PsdkXPortLimitAngleCategory;
/*!< Joint angle limit of roll axis. */
#define PSDK_XPORT_LIMIT_ANGLE_CATEGORY_ROLL_JOINT_ANGLE    DJI_XPORT_LIMIT_ANGLE_CATEGORY_ROLL_JOINT_ANGLE
/*!< Joint angle limit of pitch axis. */
#define PSDK_XPORT_LIMIT_ANGLE_CATEGORY_PITCH_JOINT_ANGLE    DJI_XPORT_LIMIT_ANGLE_CATEGORY_PITCH_JOINT_ANGLE
/*!< Euler angle limit of pitch axis, just is limitation of angle in ground coordinate. */
#define PSDK_XPORT_LIMIT_ANGLE_CATEGORY_PITCH_EULER_ANGLE    DJI_XPORT_LIMIT_ANGLE_CATEGORY_PITCH_EULER_ANGLE
/*!< Extended euler angle limit of pitch axis, just is limitation of angle in ground coordinate. Extended euler
angle limit is activated when enable extended pitch axis angle limit. */
#define PSDK_XPORT_LIMIT_ANGLE_CATEGORY_PITCH_EULER_ANGLE_EXTENSION    DJI_XPORT_LIMIT_ANGLE_CATEGORY_PITCH_EULER_ANGLE_EXTENSION
/*!< Joint angle limit of yaw axis. */
#define PSDK_XPORT_LIMIT_ANGLE_CATEGORY_YAW_JOINT_ANGLE    DJI_XPORT_LIMIT_ANGLE_CATEGORY_YAW_JOINT_ANGLE

/**
 * @brief Limit angle data structure.
 */
typedef T_DjiXPortLimitAngle T_PsdkXPortLimitAngle;

/**
 * @brief Prototype of callback function used to receive system state of XPort.
 * @warning User can not execute blocking style operations or functions in callback function, because that will block PSDK
 * root thread, causing problems such as slow system response, payload disconnection or infinite loop.
 * @param systemState: system state of XPort.
 * @return Execution result.
 */
typedef DjiReceiveXPortSystemStateCallback ReceiveXPortSystemStateCallback;

/**
 * @brief Prototype of callback function used to receive attitude information of XPort.
 * @warning User can not execute blocking style operations or functions in callback function, because that will block PSDK
 * root thread, causing problems such as slow system response, payload disconnection or infinite loop.
 * @param attitudeInformation: attitude information of XPort.
 * @return Execution result.
 */
typedef DjiReceiveXPortAttitudeInformationCallback ReceiveXPortAttitudeInformationCallback;

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Initialise XPort module.
 * @note The function have to be called before other functions related to XPort.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkXPort_Init(void);

/**
 * @brief Deinitialize XPort module.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkXPort_DeInit(void);

/**
 * @brief Register callback function used to receive system state of XPort.
 * @note The callback function will be called after registering. The call frequency is 1Hz.
 * @param callback: pointer to the callback function.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkXPort_RegReceiveSystemStateCallback(ReceiveXPortSystemStateCallback callback);

/**
 * @brief Register callback function used to receive attitude information of XPort.
 * @note The callback function will be called after registering. The call frequency is 10Hz.
 * @param callback: pointer to the callback function.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkXPort_RegReceiveAttitudeInformationCallback(ReceiveXPortAttitudeInformationCallback callback);

/**
 * @brief Set gimbal mode of XPort in blocking mode.
 * @note Max execution time of this function is slightly larger than 600ms.
 * @param mode: gimbal mode.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkXPort_SetGimbalModeSync(E_PsdkGimbalMode mode);

/**
 * @brief Rotate gimbal of XPort in blocking mode.
 * @note Max execution time of this function is slightly larger than 600ms.
 * @note If rotation mode is ::PSDK_GIMBAL_ROTATION_MODE_RELATIVE_ANGLE or ::PSDK_GIMBAL_ROTATION_MODE_ABSOLUTE_ANGLE,
 * range of yaw value in rotation value argument is [-1800, 1800]. Gimbal can rotate to any angle of yaw axis by the
 * range. Symbol of yaw value does not determine the rotation direction of the gimbal.
 * @note Attitude that gimbal can reach is also determined by angle limitation and angle limitation buffer of the gimbal.
 * When the gimbal is upward, angle limitation differs from limitation users set. For angle limitation and angle limitation
 * buffer, please refer to PSDK documentation for details. If gimbal attitude is within range limitation buffer and
 * control gimbal move towards limit direction still, or gimbal attitude is outside the angle limitation buffer but
 * control gimbal to the range limitation buffer, the gimbal will return an error. For the former case, the gimbal will not
 * move. In the latter case, the gimbal will try to move towards the direction of angle limit but it will stop at
 * somewhere in angle limitation buffer.
 * @note Rotation speed and rotation duration are also limited by the maximum speed of gimbal and maximum acceleration. The
 * maximum acceleration is determined by the smooth factor of the gimbal controller set by APP. The maximum speed is determined
 * by default maximum speed and maximum speed percentage set by APP, and the default maximum speed of XPort is
 * 90degree/s. For the detailed relationship, please refer to SetControllerSmoothFactor() and
 * SetControllerMaxSpeedPercentage() callback function prototype in psdk_gimbal.h file.
 * @note Only pitch and yaw axis of XPort is controllable.
 * @note XPort do not response rotation command in process of control parameters auto-tuning, coaxiality detection,
 * balance detection and resetting. And these operations will interrupt rotation of XPort.
 * @note Effective time of a speed control command is 500ms, that is, if XPort do not receive other control commands
 * within 500ms after speed control, XPort will stop rotation.
 * @param rotationMode: mode of rotation command.
 * @param rotationProperty: property of rotation command.
 * @param rotationValue: value of rotation command, unit: 0.1 degree (if rotation mode is
 * ::PSDK_GIMBAL_ROTATION_MODE_RELATIVE_ANGLE or ::PSDK_GIMBAL_ROTATION_MODE_ABSOLUTE_ANGLE), 0.1 degree/s
 * (if rotation mode is ::PSDK_GIMBAL_ROTATION_MODE_SPEED).
 * @return Execution result.
 */
T_PsdkReturnCode PsdkXPort_RotateSync(E_PsdkGimbalRotationMode rotationMode,
                                      T_PsdkGimbalRotationProperty rotationProperty, T_PsdkAttitude3d rotationValue);

/**
 * @brief Release gimbal control permission out in blocking mode.
 * @details When PSDK application control XPort gimbal, PSDK application will capture and gain control permission of
 * gimbal if allowable. After PSDK application sending a sequence of the gimbal control command, PSDK application should
 * call this interface to release control permission of gimbal, to allow other modules (like APP, flight
 * controller) can control gimbal right now. For example, PSDK application can release control permission after tap
 * zooming. Of course, if PSDK application does not release control permission after controlling gimbal, the gimbal will also
 * release control permission automatically at some time point later than the completion of control commands. Undoubtedly,
 * this is inefficient.
 * @note Max execution time of this function is slightly larger than 600ms.
 * @note XPort do not response release control permission command in process of control parameters auto-tuning,
 * coaxiality detection and balance detection.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkXPort_ReleaseControlPermissionSync(void);

/**
 * @brief Reset gimbal of XPort in blocking mode.
 * @details The interface reset angle of pitch axis to corresponding fine tune value in ground coordinate, and yaw axis
 * to superimposed value of yaw axis angle of aircraft and corresponding fine tune value in ground coordinate.
 * @note Max execution time of this function is slightly larger than 600ms.
 * @note XPort do not response reset command in process of control parameters auto-tuning, coaxiality detection and
 * balance detection.
 * @param mode: Reset mode.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkXPort_ResetSync(E_PsdkGimbalResetMode mode);

/**
 * @brief Set limit angle for XPort in blocking mode.
 * @details Please refer to XPort part of <a href="https://developer.dji.com/payload-sdk">developer website</a> for
 * details about default limit angle of XPort.
 * @note Max execution time of this function is slightly larger than 1200ms.
 * @note XPort do not response set limit angle command in process of control parameters auto-tuning, coaxiality
 * detection and balance detection.
 * @param limitAngleCategory: limit angle category.
 * @param limitAngle: limit angle.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkXPort_SetLimitAngleSync(E_PsdkXPortLimitAngleCategory limitAngleCategory,
                                             T_PsdkXPortLimitAngle limitAngle);

/**
 * @brief Get limit angle of XPort in blocking mode.
 * @note Max execution time of this function is slightly larger than 1200ms.
 * @param limitAngleCategory: limit angle category.
 * @param limitAngle: pointer to memory space used to store limit angle.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkXPort_GetLimitAngleSync(E_PsdkXPortLimitAngleCategory limitAngleCategory,
                                             T_PsdkXPortLimitAngle *limitAngle);

/**
 * @brief Set speed conversion factor for speed control from joystick and APP.
 * @details XPort convert speed control command from joystick and APP to rotation speed based on the speed conversion
 * factor. The formula is "speed = maximum rotation speed * conversion factor", and maximum rotation speed is a product of
 * default maximum speed and maximum speed percentage. The default maximum rotation speed is 90degree/s. The maximum
 * speed percentage is set by APP. The default speed conversion factor is 1.0.
 * @note The value will be effective after a while, and the max value is 100ms.
 * @param factor: Speed conversion factor and it has to be smaller than or equal to 1.0.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkXPort_SetSpeedConversionFactor(psdk_f32_t factor);

#ifdef __cplusplus
}
#endif

#endif // PSDK_XPORT_H

/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
