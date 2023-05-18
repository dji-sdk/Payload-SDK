/**
 ********************************************************************
 * @file    dji_flight_controller.h
 * @brief   This is the header file for "dji_flight_controller.c", defining the structure and
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
#ifndef DJI_FLIGHT_CONTROLLER_H
#define DJI_FLIGHT_CONTROLLER_H

/* Includes ------------------------------------------------------------------*/
#include "dji_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/
#define EMERGENCY_STOP_MOTOR_MSG_MAX_LENGTH 10 /*!<  Max length of emergency stop motor message. */

/* Exported types ------------------------------------------------------------*/
typedef uint16_t E_DjiFlightControllerGoHomeAltitude; /*!< Unit:meter, range 20~500 */

/**
 * @brief The UAV's actions when rc is lost.
 */
typedef enum {
    DJI_FLIGHT_CONTROLLER_RC_LOST_ACTION_HOVER = 0,  /*!< Aircraft will execute hover cation when rc is lost. */
    DJI_FLIGHT_CONTROLLER_RC_LOST_ACTION_LANDING = 1,  /*!< Aircraft will execute land cation when rc is lost. */
    DJI_FLIGHT_CONTROLLER_RC_LOST_ACTION_GOHOME = 2,  /*!< Aircraft will execute go-home cation when rc is lost. */
} E_DjiFlightControllerRCLostAction;

/**
 * @brief Enable/Disable RTK position enum
 */
typedef enum {
    DJI_FLIGHT_CONTROLLER_DISABLE_RTK_POSITION = 0, /*!< 0: The UAV will use GPS data instead of RTK data to execute
                                                     * actions which require location information(waypoint、go home...)
                                                     */
    DJI_FLIGHT_CONTROLLER_ENABLE_RTK_POSITION = 1, /*!< 1:The UAV will use RTK data instead of GPS data to execute
                                                    * actions which require location information(waypoint、go home...)*/
} E_DjiFlightControllerRtkPositionEnableStatus;

/**
 * @brief Enable/Disable obstacle avoidance enum
 */
typedef enum {
    DJI_FLIGHT_CONTROLLER_DISABLE_OBSTACLE_AVOIDANCE = 0, /*!< 0: The UAV will not perform obstacle avoidance in
                                                           * the specified direction */
    DJI_FLIGHT_CONTROLLER_ENABLE_OBSTACLE_AVOIDANCE = 1, /*!< 0: The UAV will perform obstacle avoidance in the
                                                          * specified direction */
} E_DjiFlightControllerObstacleAvoidanceEnableStatus;

/**
 * @brief Enable/Disable emergency stop motor function enum
 * @note Attention:Enable emergency-stop-motor function is very dangerous in the air.it will make the aircraft crash!!!
 */
typedef enum {
    DJI_FLIGHT_CONTROLLER_ENABLE_EMERGENCY_STOP_MOTOR = 0x01, /*!< Execute emergency-stop-motor action */
} E_DjiFlightControllerEmergencyStopMotor;

/**
 * @brief Obtain/Release joystick control authority command enum
 * @note You have obtain joystick control authority successfully before using joystick.
 */
typedef enum {
    DJI_FLIGHT_CONTROLLER_RELEASE_JOYSTICK_CTRL_AUTHORITY = 0, /*!< Obtain joystick authority */
    DJI_FLIGHT_CONTROLLER_OBTAIN_JOYSTICK_CTRL_AUTHORITY = 1, /*!< Release joystick authority */
} E_DjiFlightControllerJoystickCtrlAuthorityAction;

/**
 * @brief The UAV's joystick control authority owner enum
 */
typedef enum {
    DJI_FLIGHT_CONTROLLER_JOYSTICK_CTRL_AUTHORITY_RC = 0,  /*!< Rc could control UAV with joystick. */
    DJI_FLIGHT_CONTROLLER_JOYSTICK_CTRL_AUTHORITY_MSDK = 1, /*!< MSDK could control UAV with joystick. */
    DJI_FLIGHT_CONTROLLER_JOYSTICK_CTRL_AUTHORITY_INTERNAL = 2, /*!< Special Internal modules could control UAV
                                                                 * with joystick. */
    DJI_FLIGHT_CONTROLLER_JOYSTICK_CTRL_AUTHORITY_OSDK = 4, /*!< OSDK could control UAV with joystick. */
} E_DjiFlightControllerJoystickCtrlAuthority;

/**
 * @brief The UAV's joystick control authority switch reason enum
 */
typedef enum {
    DJI_FLIGHT_CONTROLLER_MSDK_GET_JOYSTICK_CTRL_AUTH_EVENT = 1, /*!< MSDK get joystick control authority. */
    DJI_FLIGHT_CONTROLLER_INTERNAL_GET_JOYSTICK_CTRL_AUTH_EVENT = 2,  /*!< Special Internal modules get joystick control authority. */
    DJI_FLIGHT_CONTROLLER_OSDK_GET_JOYSTICK_CTRL_AUTH_EVENT = 3, /*!< OSDK get joystick control authority. */
    DJI_FLIGHT_CONTROLLER_RC_LOST_GET_JOYSTICK_CTRL_AUTH_EVENT = 4, /*!< Reset joystick control authority to rc when executing rc lost action */
    DJI_FLIGHT_CONTROLLER_RC_NOT_P_MODE_RESET_JOYSTICK_CTRL_AUTH_EVENT = 5,  /*!< Reset joystick control authority to rc when rc is not in P mode */
    DJI_FLIGHT_CONTROLLER_RC_SWITCH_MODE_GET_JOYSTICK_CTRL_AUTH_EVENT = 6,  /*!< Set joystick control authority to rc when rc switch control mode(T/APS) */
    DJI_FLIGHT_CONTROLLER_RC_PAUSE_GET_JOYSTICK_CTRL_AUTH_EVENT = 7, /*!< Reset joystick control authority to rc when rc pausing */
    DJI_FLIGHT_CONTROLLER_RC_REQUEST_GO_HOME_GET_JOYSTICK_CTRL_AUTH_EVENT = 8, /*!< Reset joystick control authority to rc when rc requesting go home*/
    DJI_FLIGHT_CONTROLLER_LOW_BATTERY_GO_HOME_RESET_JOYSTICK_CTRL_AUTH_EVENT = 9, /*!< Reset joystick control authority to rc when aircraft is executing low-battery-go-home*/
    DJI_FLIGHT_CONTROLLER_LOW_BATTERY_LANDING_RESET_JOYSTICK_CTRL_AUTH_EVENT = 10, /*!< Reset joystick control authority to rc when aircraft is executing low-battery-landing*/
    DJI_FLIGHT_CONTROLLER_OSDK_LOST_GET_JOYSTICK_CTRL_AUTH_EVENT = 11, /*!< Reset joystick control authority to rc when osdk is lost*/
    DJI_FLIGHT_CONTROLLER_NERA_FLIGHT_BOUNDARY_RESET_JOYSTICK_CTRL_AUTH_EVENT = 12, /*!< Reset joystick control authority to rc when aircraft is near boundary.*/
} E_DjiFlightControllerJoystickCtrlAuthoritySwitchEvent;

/**
 * @brief The UAV's joystick control authority switch event info enum
 */
typedef struct {
    E_DjiFlightControllerJoystickCtrlAuthority curJoystickCtrlAuthority; /*!< The UAV's joystick control authority owner */
    E_DjiFlightControllerJoystickCtrlAuthoritySwitchEvent joystickCtrlAuthoritySwitchEvent; /*!< The UAV's joystick control authority switch reason */
} T_DjiFlightControllerJoystickCtrlAuthorityEventInfo;

/**
 * @brief Prototype of callback function used to get joystick control authority switch event info.
 * @return Execution result.
 */
typedef T_DjiReturnCode (*JoystickCtrlAuthorityEventCbFunc)(
    T_DjiFlightControllerJoystickCtrlAuthorityEventInfo eventData);

/**
 * @brief Horizon control mode in joystick mode enum
 * @note Only when the GPS signal is good (health_flag >=3)，horizontal position control (DJI_FLIGHT_CONTROLLER_HORIZONTAL_POSITION_CONTROL_MODE)
 * related control modes can be used. only when GPS signal is good (health_flag >=3)，or when advanced sensing system is working properly with Autopilot，
 * horizontal velocity control（DJI_FLIGHT_CONTROLLER_HORIZONTAL_VELOCITY_CONTROL_MODE) related control modes can be used.
 */
typedef enum {
    /**
     * @brief Control pitch & roll & angle of the aircraft.
     * @note Need to be referenced to either the ground or body frame by E_DjiFlightControllerHorizontalCoordinate setting.
     * Limit: -35 degree to 35 degree
     */
    DJI_FLIGHT_CONTROLLER_HORIZONTAL_ANGLE_CONTROL_MODE = 0,
    /**
     * @brief Set the control-mode to control horizontal vehicle velocities.
     * @note Need to be referenced to either the ground or body frame by E_DjiFlightControllerHorizontalCoordinate setting
     * Limit: -30m/s to 30 m/s
     */
    DJI_FLIGHT_CONTROLLER_HORIZONTAL_VELOCITY_CONTROL_MODE = 1,
    /**
     * @brief Set the control-mode to control position offsets of pitch & roll directions.
     * @note Need to be referenced to either the ground or body frame by E_DjiFlightControllerHorizontalCoordinate setting
     * Limit: N/A
     */
    DJI_FLIGHT_CONTROLLER_HORIZONTAL_POSITION_CONTROL_MODE = 2,
    /**
     * @brief Set the control-mode to control rate of change of the vehicle's attitude.
     * @note Need to be referenced to either the ground or body frame by E_DjiFlightControllerHorizontalCoordinate setting
     * Limit: -150deg/s to 150.0 deg/s
     */
    DJI_FLIGHT_CONTROLLER_HORIZONTAL_ANGULAR_RATE_CONTROL_MODE = 3
} E_DjiFlightControllerHorizontalControlMode;

/**
 * @brief Vertical control mode in joystick mode enum
 * @note We suggest developers do not use VERTICAL_POSITION control mode indoor when your UAV flight height is larger than 3 meters.
 * This is because in indoor environments, barometer can be inaccurate, and the vertical controller may fail to keep
 * the height of the UAV.
 */
typedef enum {
    /**
     * @brief Set the control-mode to control the vertical speed of UAV, upward is positive/
     * @note Limit: -5m/s to 5 m/s
     */
    DJI_FLIGHT_CONTROLLER_VERTICAL_VELOCITY_CONTROL_MODE = 0,

    /**
     * @brief Set the control-mode to control the height of UAV
     * @note Limit: 0m to 120 m
     */
    DJI_FLIGHT_CONTROLLER_VERTICAL_POSITION_CONTROL_MODE = 1,

    /**
     * @brief Set the control-mode to directly control the thrust
     * @note Range: 0% to 100%
     */
    DJI_FLIGHT_CONTROLLER_VERTICAL_THRUST_CONTROL_MODE = 2,
} E_DjiFlightControllerVerticalControlMode;

/**
 * @brief Yaw control mode in joystick mode enum
 */
typedef enum {
    /**
     * @brief Set the control-mode to control yaw angle.
     * @note Yaw angle is referenced to the ground frame.In this control mode, Ground frame is enforced in Autopilot.
     */
    DJI_FLIGHT_CONTROLLER_YAW_ANGLE_CONTROL_MODE = 0x00,

    /**
     * @brief Set the control-mode to control yaw angular velocity
     * @note Same reference frame as YAW_ANGLE.
     * Limit: -150deg/s to 150 deg/s
     */
    DJI_FLIGHT_CONTROLLER_YAW_ANGLE_RATE_CONTROL_MODE = 1
} E_DjiFlightControllerYawControlMode;

/**
 * @brief Horizontal coordinate in joystick mode enum
 */
typedef enum {
    DJI_FLIGHT_CONTROLLER_HORIZONTAL_GROUND_COORDINATE = 0, /*!< Set the x-y of ground frame as the horizontal frame (NEU) */
    DJI_FLIGHT_CONTROLLER_HORIZONTAL_BODY_COORDINATE = 1 /*!< Set the x-y of body frame as the horizontal frame (FRU) */
} E_DjiFlightControllerHorizontalCoordinate;

/*!
 * @brief Stable mode in joystick mode enum.
 * @note Only works in Horizontal velocity control mode.In velocity stable mode, drone will brake and hover at one position once
 * the input command is zero.In velocity non-stable mode, drone will follow the velocity command and not hover when the command is zero.
 * That means drone will drift with the wind.
 */
typedef enum {
    DJI_FLIGHT_CONTROLLER_STABLE_CONTROL_MODE_DISABLE = 0, /*!< Disable the stable mode */
    DJI_FLIGHT_CONTROLLER_STABLE_CONTROL_MODE_ENABLE = 1   /*!< Enable the stable mode */
} E_DjiFlightControllerStableControlMode;

typedef enum {
    DJI_FLIGHT_CONTROLLER_ENABLE_RC_LOST_ACTION = 0,
    DJI_FLIGHT_CONTROLLER_DISABLE_RC_LOST_ACTION = 1,
} E_DjiFlightControllerRCLostActionEnableStatus;

/**
 * @brief Joystick mode.
 * @note You need to set joystick mode first before start to send joystick command to aircraft.
 */
typedef struct {
    E_DjiFlightControllerHorizontalControlMode horizontalControlMode; /*!< See reference of E_DjiFlightControllerHorizontalControlMode*/
    E_DjiFlightControllerVerticalControlMode verticalControlMode; /*!< See reference of E_DjiFlightControllerVerticalControlMode*/
    E_DjiFlightControllerYawControlMode yawControlMode; /*!< See reference of E_DjiFlightControllerYawControlMode*/
    E_DjiFlightControllerHorizontalCoordinate horizontalCoordinate; /*!< See reference of E_DjiFlightControllerHorizontalCoordinate*/
    E_DjiFlightControllerStableControlMode stableControlMode; /*!< See reference of E_DjiFlightControllerStableControlMode*/
} T_DjiFlightControllerJoystickMode;

#pragma pack(1)
typedef struct {
    dji_f32_t x;   /*!< Control with respect to the x axis.*/
    dji_f32_t y;   /*!< Control with respect to the y axis.*/
    dji_f32_t z;   /*!< Control with respect to the z axis, up is positive. */
    dji_f32_t yaw; /*!< Yaw position/velocity control w.r.t. the ground frame.*/
} T_DjiFlightControllerJoystickCommand;// pack(1)

typedef struct {
    dji_f64_t latitude;  /*!< unit: rad */
    dji_f64_t longitude; /*!< unit: rad */
} T_DjiFlightControllerHomeLocation; // pack(1)

typedef struct {
    char serialNum[32];
} T_DjiFlightControllerGeneralInfo;
#pragma pack()

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Initialise flight controller module, and user should call this function
 * before using flight controller features.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_Init(void);

/**
 * @brief DeInitialise flight controller module.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_Deinit(void);

/**
 * @brief Enable/Disable RTK position function.
 * @details Enabling RTK means that RTK data will be used instead of GPS during flight.
 * @param rtkEnableStatus: see reference of "E_DjiFlightControllerRtkPositionEnableStatus".
 * It keeps in sync with pilot's param.
 * @return Execution result.
 */
T_DjiReturnCode
DjiFlightController_SetRtkPositionEnableStatus(E_DjiFlightControllerRtkPositionEnableStatus rtkEnableStatus);

/**
 * @brief Get RTK enable status.
 * @note Enabling RTK means that RTK data will be used during intelligent flight.
 * @param rtkEnableStatus: see reference of "E_DjiFlightControllerRtkPositionEnableStatus".
 * It keeps in sync with pilot's param.
 * @return Execution result.
 */
T_DjiReturnCode
DjiFlightController_GetRtkPositionEnableStatus(E_DjiFlightControllerRtkPositionEnableStatus *rtkEnableStatus);

/**
 * @brief Set rc lost action.
 * @note It will be valid when rc and osdk is both lost.It only support M30.
 * @param rcLostAction: actions when rc is lost.(hover/landing/go home).It keeps in sync with pilot's param.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_SetRCLostAction(E_DjiFlightControllerRCLostAction rcLostAction);

/**
 * @brief Get rc lost action(hover/landing/gohome).
 * @note It will be valid when rc and osdk is both lost.It only support M30.
 * @param rcLostAction: see reference of E_DjiFlightControllerRCLostAction.It keeps in sync with pilot's param.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_GetRCLostAction(E_DjiFlightControllerRCLostAction *rcLostAction);

/**
 * @brief Enable/Disable horizontal visual(forwards,backwards,left,right) obstacle avoidance.
 * @note For detailed parameters of obstacle avoidance, it is recommended to read the official user manual in
 * https://www.dji.com.
 * @param horizontalObstacleAvoidanceEnableStatus: see reference of E_DjiFlightControllerObstacleAvoidanceEnableStatus.
 * It keeps in sync with pilot's param.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_SetHorizontalVisualObstacleAvoidanceEnableStatus(
    E_DjiFlightControllerObstacleAvoidanceEnableStatus horizontalObstacleAvoidanceEnableStatus);

/**
 * @brief Get status of horizontal visual(forwards,backwards,left,right) obstacle avoidance switch.
 * @note For detailed parameters of obstacle avoidance, it is recommended to read the official user manual in
 * https://www.dji.com.
 * @param horizontalObstacleAvoidanceEnableStatus: see reference of E_DjiFlightControllerObstacleAvoidanceEnableStatus.
 * It keeps in sync with pilot's param.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_GetHorizontalVisualObstacleAvoidanceEnableStatus(
    E_DjiFlightControllerObstacleAvoidanceEnableStatus *horizontalObstacleAvoidanceEnableStatus);

/**
 * @brief Enable/Disable horizontal radar obstacle avoidance.
 * @note It will be valid only if you install CSM radar successfully.For detailed parameters of obstacle avoidance,
 * it is recommended to read the official user manual in https://www.dji.com/uk/matrice-300/downloads.
 * @param horizontalObstacleAvoidanceEnableStatus: see reference of E_DjiFlightControllerObstacleAvoidanceEnableStatus.
 * It keeps in sync with pilot's param.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_SetHorizontalRadarObstacleAvoidanceEnableStatus(
    E_DjiFlightControllerObstacleAvoidanceEnableStatus horizontalObstacleAvoidanceEnableStatus);

/**
 * @brief Get status of horizontal radar obstacle avoidance switch.
 * @note It will be valid only if you install CSM radar successfully.For detailed parameters of obstacle avoidance,
 * it is recommended to read the official user manual in https://www.dji.com/uk/matrice-300/downloads.
 * @param horizontalObstacleAvoidanceEnableStatus: see reference of E_DjiFlightControllerObstacleAvoidanceEnableStatus.
 * It keeps in sync with pilot's param.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_GetHorizontalRadarObstacleAvoidanceEnableStatus(
    E_DjiFlightControllerObstacleAvoidanceEnableStatus *horizontalObstacleAvoidanceEnableStatus);

/**
 * @brief Enable/Disable upwards visual obstacle avoidance.
 * @note For detailed parameters of obstacle avoidance, it is recommended to read the official user manual in
 * https://www.dji.com.
 * @param upwardsObstacleAvoidanceEnableStatus: see reference of E_DjiFlightControllerObstacleAvoidanceEnableStatus.
 * It keeps in sync with pilot's param.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_SetUpwardsVisualObstacleAvoidanceEnableStatus(
    E_DjiFlightControllerObstacleAvoidanceEnableStatus upwardsObstacleAvoidanceEnableStatus);

/**
 * @brief Get status of upwards visual obstacle avoidance switch.
 * @note For detailed parameters of obstacle avoidance, it is recommended to read the official user manual in
 * https://www.dji.com.
 * @param upwardsObstacleAvoidanceEnableStatus: see reference of E_DjiFlightControllerObstacleAvoidanceEnableStatus.
 * It keeps in sync with pilot's param.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_GetUpwardsVisualObstacleAvoidanceEnableStatus(
    E_DjiFlightControllerObstacleAvoidanceEnableStatus *upwardsObstacleAvoidanceEnableStatus);

/**
 * @brief Enable/Disable upwards radar obstacle avoidance.
 * @note It will be valid only if you install CSM radar successfully.For detailed parameters of obstacle avoidance,
 * it is recommended to read the official user manual in https://www.dji.com.
 * @param upwardsObstacleAvoidanceEnableStatus: see reference of E_DjiFlightControllerObstacleAvoidanceEnableStatus.
 * It keeps in sync with pilot's param.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_SetUpwardsRadarObstacleAvoidanceEnableStatus(
    E_DjiFlightControllerObstacleAvoidanceEnableStatus upwardsObstacleAvoidanceEnableStatus);

/**
 * @brief Get status of upwards radar obstacle avoidance switch
 * @note For detailed parameters of obstacle avoidance, it is recommended to read the official user manual in
 * https://www.dji.com.
 * @param upwardsObstacleAvoidanceEnableStatus: see reference of E_DjiFlightControllerObstacleAvoidanceEnableStatus.
 * It keeps in sync with pilot's param.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_GetUpwardsRadarObstacleAvoidanceEnableStatus(
    E_DjiFlightControllerObstacleAvoidanceEnableStatus *upwardsObstacleAvoidanceEnableStatus);

/**
 * @brief Enable/Disable downwards visual obstacle avoidance
 * @note For detailed parameters of obstacle avoidance, it is recommended to read the official user manual in
 * https://www.dji.com.
 * @param downwardsObstacleAvoidanceEnableStatus: see reference of E_DjiFlightControllerObstacleAvoidanceEnableStatus.
 * It keeps in sync with pilot's param.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_SetDownwardsVisualObstacleAvoidanceEnableStatus(
    E_DjiFlightControllerObstacleAvoidanceEnableStatus downwardsObstacleAvoidanceEnableStatus);

/**
 * @brief Get status of downwards visual obstacle avoidance switch
 * @note For detailed parameters of obstacle avoidance, it is recommended to read the official user manual in
 * https://www.dji.com.
 * @param downwardsObstacleAvoidanceEnableStatus: see reference of E_DjiFlightControllerObstacleAvoidanceEnableStatus.
 * It keeps in sync with pilot's param.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_GetDownwardsVisualObstacleAvoidanceEnableStatus(
    E_DjiFlightControllerObstacleAvoidanceEnableStatus *downwardsObstacleAvoidanceEnableStatus);

/**
 * @brief Arrest flying.
 * @note when the UAV is on the ground ,it will stop motors and display "hms description" on APP. when the UAV is
 * in the air, it will continue flying and display "hms description" on APP only.
 * If you use this interface, you need to use "DjiFlightController_CancelArrestFlying" to quit arrest-flying status, then
 * then the UAV can fly again.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_ArrestFlying(void);

/**
 * @brief Quit status of arrest-flying.
 * @note The UAV need to quit status of arrest-flying to continue flying after arresting flying.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_CancelArrestFlying(void);

/**
 * @brief Turn on motors when the UAV is on the ground.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_TurnOnMotors(void);

/**
 * @brief Turn off motors when the UAV is on the ground.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_TurnOffMotors(void);

/**
 * @brief Emergency stop motor in any case.
 * @note If you want to turn on motor after emergency stopping motor, you need to use the interface to send disable
 * command to quit lock-motor status.
 * @param cmd: see reference of E_DjiFlightControllerEmergencyStopMotor
 * @param debugMsg:inject debug message to flight control FW for logging, size limit: 10 bytes
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_EmergencyStopMotor(E_DjiFlightControllerEmergencyStopMotor cmd,
                                                       char debugMsg[EMERGENCY_STOP_MOTOR_MSG_MAX_LENGTH]);
/**
 * @brief Request take off action when the UAV is on the ground.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_StartTakeoff(void);

/**
 * @brief Request landing action when the UAV is in the air.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_StartLanding(void);

/**
 * @brief Request cancel landing action when the UAV is landing
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_CancelLanding(void);

/**
 * @brief Confirm the landing when the UAV is 0.7m above the ground.
 * @note When the clearance between the aircraft and the ground is less than 0.7m, the aircraft will pause landing and
 * wait for user's confirmation.This api use for confirm landing. If the ground is not suitable for landing ,user must
 * use RC to control it landing manually or force landing.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_StartConfirmLanding(void);

/**
 * @brief Force landing in any case.
 * @note This api will ignore the smart landing function,.When using this pi, it will landing directly (would not stop
 * at 0.7m and wait user's  command). Attention:it may make the aircraft crash!!!
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_StartForceLanding(void);

/**
 * @brief Set customized GPS(not RTK) home location.
 * @note Set customized home location failed reason may as follows:
 * 1. The distance between new home location and last home location is larger than MAX_FLY_RADIUS(20km).
 * 2. Record initial home location failed after start aircraft.
 * @param homeLocation: homeLocation include latitude and longitude
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_SetHomeLocationUsingGPSCoordinates(T_DjiFlightControllerHomeLocation homeLocation);

/**
 * @brief Set home location using current aircraft GPS(not RTK) location.
 * @note Set home location failed reasons may as follows:
 * 1. Aircraft's gps level can't reach the condition of recording home location.
 * 2. Record initial home location failed after start aircraft.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_SetHomeLocationUsingCurrentAircraftLocation(void);

/**
 * @brief Set go home altitude.
 * @note If aircraft's current altitude is higher than the setting value of go home altitude, aircraft will go home
 * using current altitude. Otherwise, it will climb to setting of go home altitude ,and then execute go home action.
 * Go home altitude setting is 20m ~ 500m.
 * @param altitude: go home altitude, unit: meter
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_SetGoHomeAltitude(E_DjiFlightControllerGoHomeAltitude altitude);

/**
 * @brief Get go home altitude.
 * @param altitude: go home altitude, unit: meter
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_GetGoHomeAltitude(E_DjiFlightControllerGoHomeAltitude *altitude);

/**
 * @brief Request go home action when the UAV is in the air
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_StartGoHome(void);

/**
 * @brief Request cancel go home action when the UAV is going home
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_CancelGoHome(void);

/**
 * @brief Obtain UAV's joystick control authority.
 * @note 1.You have to obtain joystick control authority successfully before you using joystick to control UAV.
 * 2. RC must be in p-mode.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_ObtainJoystickCtrlAuthority(void);

/**
 * @brief Release UAV's joystick control authority.
 * @note RC must be in p-mode.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_ReleaseJoystickCtrlAuthority(void);

/**
 * @brief Subscribe to joystick control authority switch event with a callback function.
 * @note it will be triggered once the joystick control authority switch event occurs.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_RegJoystickCtrlAuthorityEventCallback(JoystickCtrlAuthorityEventCbFunc callback);

/**
 * @brief Set expected joystick mode before requesting joystick.
 * @param joystickMode: include horizontal/vertical/yaw control mode, stable control mode.
 */
void DjiFlightController_SetJoystickMode(T_DjiFlightControllerJoystickMode joystickMode);

/**
 * @brief Request execute joystick action.
 * @param joystickCommand: include x/y/z/yaw.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_ExecuteJoystickAction(T_DjiFlightControllerJoystickCommand joystickCommand);

/**
 * @brief Request emergency brake action.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_ExecuteEmergencyBrakeAction(void);

/**
 * @brief Request cancel emergency brake action.
 * @note It is only support on M30.If you use DjiFlightController_ExecuteEmergencyBrakeAction(), you need to use
 * "DjiFlightController_CancelEmergencyBrakeAction()" to allow aircraft to execute drone action again.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_CancelEmergencyBrakeAction(void);

/**
 * @brief Get general info of the aircraft.
 * @param generalInfo: the struct stored the serial num which contains a array of chars var in case the user gives an
 * illegal length character pointer
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_GetGeneralInfo(T_DjiFlightControllerGeneralInfo *generalInfo);

/*! @brief The command decides whether execute RC lost action or not when osdk is running
  * @note  This setting only affects the behavior of the drone when the RC lost and the OSDK is connected.
  *         if the command is enable, the drone will not execute rc lost action when rc is lost but OSDK is running;
  *         if the command is disable, the drone will execute rc lost action when rc is lost but OSDK is running
  *         the drone will execute rc lost action when rc is lost and OSDK is lost whatever the command is.
  *         default command is disable.
  * @param executeRCLostActionOrNotWhenOnboardOn  enable:1;disable:0
  * @return T_DjiReturnCode error code
   */
T_DjiReturnCode
DjiFlightController_SetRCLostActionEnableStatus(E_DjiFlightControllerRCLostActionEnableStatus command);

/*! @brief get rc lost action enable status(enable or disable)
 *  @param command executeRCLostActionOrNotWhenOnboardOn, enable:1;disable:0
 *  @return  T_DjiReturnCode error code
 */
T_DjiReturnCode
DjiFlightController_GetEnableRCLostActionStatus(E_DjiFlightControllerRCLostActionEnableStatus *command);

#ifdef __cplusplus
}
#endif

#endif // DJI_FLIGHT_CONTROLLER_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
