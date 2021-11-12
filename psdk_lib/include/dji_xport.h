/**
 ********************************************************************
 * @file    dji_xport.h
 * @brief   XPort module provides some interfaces used to control XPort and get states of XPort. The module can be used
 * only when the DJI adapter is XPort, rather than Skyport.
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
#ifndef DJI_XPORT_H
#define DJI_XPORT_H

/* Includes ------------------------------------------------------------------*/
#include "dji_typedef.h"
#include "dji_gimbal.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/
/**
 * @brief XPort limit angle category.
 */
typedef enum {
    DJI_XPORT_LIMIT_ANGLE_CATEGORY_ROLL_JOINT_ANGLE = 0, /*!< Joint angle limit of roll axis. */
    DJI_XPORT_LIMIT_ANGLE_CATEGORY_PITCH_JOINT_ANGLE = 1, /*!< Joint angle limit of pitch axis. */
    DJI_XPORT_LIMIT_ANGLE_CATEGORY_PITCH_EULER_ANGLE = 2, /*!< Euler angle limit of pitch axis, just is limitation of angle in ground coordinate. */
    DJI_XPORT_LIMIT_ANGLE_CATEGORY_PITCH_EULER_ANGLE_EXTENSION = 3, /*!< Extended euler angle limit of pitch axis, just is limitation of angle in ground coordinate. Extended euler
                                                                          angle limit is activated when enable extended pitch axis angle limit. */
    DJI_XPORT_LIMIT_ANGLE_CATEGORY_YAW_JOINT_ANGLE = 4, /*!< Joint angle limit of yaw axis. */
} E_DjiXPortLimitAngleCategory;

/**
 * @brief Limit angle data structure.
 */
typedef struct {
    int16_t upperLimit; /*!< Upper limit of limit angle, unit: 0.1degree. */
    int16_t lowerLimit; /*!< Lower limit of limit angle, unit: 0.1degree. */
} T_DjiXPortLimitAngle;

/**
 * @brief Prototype of callback function used to receive system state of XPort.
 * @warning User can not execute blocking style operations or functions in callback function, because that will block DJI
 * root thread, causing problems such as slow system response, payload disconnection or infinite loop.
 * @param systemState: system state of XPort.
 * @return Execution result.
 */
typedef T_DjiReturnCode (*DjiReceiveXPortSystemStateCallback)(T_DjiGimbalSystemState systemState);

/**
 * @brief Prototype of callback function used to receive attitude information of XPort.
 * @warning User can not execute blocking style operations or functions in callback function, because that will block DJI
 * root thread, causing problems such as slow system response, payload disconnection or infinite loop.
 * @param attitudeInformation: attitude information of XPort.
 * @return Execution result.
 */
typedef T_DjiReturnCode (*DjiReceiveXPortAttitudeInformationCallback)(
    T_DjiGimbalAttitudeInformation attitudeInformation);

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Initialise XPort module.
 * @note The function have to be called before other functions related to XPort.
 * @return Execution result.
 */
T_DjiReturnCode DjiXPort_Init(void);

/**
 * @brief Deinitialize XPort module.
 * @return Execution result.
 */
T_DjiReturnCode DjiXPort_DeInit(void);

/**
 * @brief Register callback function used to receive system state of XPort.
 * @note The callback function will be called after registering. The call frequency is 1Hz.
 * @param callback: pointer to the callback function.
 * @return Execution result.
 */
T_DjiReturnCode DjiXPort_RegReceiveSystemStateCallback(DjiReceiveXPortSystemStateCallback callback);

/**
 * @brief Register callback function used to receive attitude information of XPort.
 * @note The callback function will be called after registering. The call frequency is 10Hz.
 * @param callback: pointer to the callback function.
 * @return Execution result.
 */
T_DjiReturnCode DjiXPort_RegReceiveAttitudeInformationCallback(DjiReceiveXPortAttitudeInformationCallback callback);

/**
 * @brief Set gimbal mode of XPort in blocking mode.
 * @note Max execution time of this function is slightly larger than 600ms.
 * @param mode: gimbal mode.
 * @return Execution result.
 */
T_DjiReturnCode DjiXPort_SetGimbalModeSync(E_DjiGimbalMode mode);

/**
 * @brief Rotate gimbal of XPort in blocking mode.
 * @note Max execution time of this function is slightly larger than 600ms.
 * @note If rotation mode is ::DJI_GIMBAL_ROTATION_MODE_RELATIVE_ANGLE or ::DJI_GIMBAL_ROTATION_MODE_ABSOLUTE_ANGLE,
 * range of yaw value in rotation value argument is [-1800, 1800]. Gimbal can rotate to any angle of yaw axis by the
 * range. Symbol of yaw value does not determine the rotation direction of the gimbal.
 * @note Attitude that gimbal can reach is also determined by angle limitation and angle limitation buffer of the gimbal.
 * When the gimbal is upward, angle limitation differs from limitation users set. For angle limitation and angle limitation
 * buffer, please refer to DJI documentation for details. If gimbal attitude is within range limitation buffer and
 * control gimbal move towards limit direction still, or gimbal attitude is outside the angle limitation buffer but
 * control gimbal to the range limitation buffer, the gimbal will return an error. For the former case, the gimbal will not
 * move. In the latter case, the gimbal will try to move towards the direction of angle limit but it will stop at
 * somewhere in angle limitation buffer.
 * @note Rotation speed and rotation duration are also limited by the maximum speed of gimbal and maximum acceleration. The
 * maximum acceleration is determined by the smooth factor of the gimbal controller set by APP. The maximum speed is determined
 * by default maximum speed and maximum speed percentage set by APP, and the default maximum speed of XPort is
 * 90degree/s. For the detailed relationship, please refer to SetControllerSmoothFactor() and
 * SetControllerMaxSpeedPercentage() callback function prototype in dji_gimbal.h file.
 * @note Only pitch and yaw axis of XPort is controllable.
 * @note XPort do not response rotation command in process of control parameters auto-tuning, coaxiality detection,
 * balance detection and resetting. And these operations will interrupt rotation of XPort.
 * @note Effective time of a speed control command is 500ms, that is, if XPort do not receive other control commands
 * within 500ms after speed control, XPort will stop rotation.
 * @param rotationMode: mode of rotation command.
 * @param rotationProperty: property of rotation command.
 * @param rotationValue: value of rotation command, unit: 0.1 degree (if rotation mode is
 * ::DJI_GIMBAL_ROTATION_MODE_RELATIVE_ANGLE or ::DJI_GIMBAL_ROTATION_MODE_ABSOLUTE_ANGLE), 0.1 degree/s
 * (if rotation mode is ::DJI_GIMBAL_ROTATION_MODE_SPEED).
 * @return Execution result.
 */
T_DjiReturnCode DjiXPort_RotateSync(E_DjiGimbalRotationMode rotationMode,
                                    T_DjiGimbalRotationProperty rotationProperty, T_DjiAttitude3d rotationValue);

/**
 * @brief Release gimbal control permission out in blocking mode.
 * @details When DJI application control XPort gimbal, DJI application will capture and gain control permission of
 * gimbal if allowable. After DJI application sending a sequence of the gimbal control command, DJI application should
 * call this interface to release control permission of gimbal, to allow other modules (like APP, flight
 * controller) can control gimbal right now. For example, DJI application can release control permission after tap
 * zooming. Of course, if DJI application does not release control permission after controlling gimbal, the gimbal will also
 * release control permission automatically at some time point later than the completion of control commands. Undoubtedly,
 * this is inefficient.
 * @note Max execution time of this function is slightly larger than 600ms.
 * @note XPort do not response release control permission command in process of control parameters auto-tuning,
 * coaxiality detection and balance detection.
 * @return Execution result.
 */
T_DjiReturnCode DjiXPort_ReleaseControlPermissionSync(void);

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
T_DjiReturnCode DjiXPort_ResetSync(E_DjiGimbalResetMode mode);

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
T_DjiReturnCode DjiXPort_SetLimitAngleSync(E_DjiXPortLimitAngleCategory limitAngleCategory,
                                           T_DjiXPortLimitAngle limitAngle);

/**
 * @brief Get limit angle of XPort in blocking mode.
 * @note Max execution time of this function is slightly larger than 1200ms.
 * @param limitAngleCategory: limit angle category.
 * @param limitAngle: pointer to memory space used to store limit angle.
 * @return Execution result.
 */
T_DjiReturnCode DjiXPort_GetLimitAngleSync(E_DjiXPortLimitAngleCategory limitAngleCategory,
                                           T_DjiXPortLimitAngle *limitAngle);

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
T_DjiReturnCode DjiXPort_SetSpeedConversionFactor(float factor);

#ifdef __cplusplus
}
#endif

#endif // DJI_XPORT_H

/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
