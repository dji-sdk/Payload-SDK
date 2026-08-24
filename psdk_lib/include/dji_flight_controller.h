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
typedef uint16_t E_DjiFlightControllerGoHomeAltitude; /*!< Unit:meter, range 20~1500 */

/**
 * @brief The aircraft's actions when RC is lost.
 */
typedef enum {
    DJI_FLIGHT_CONTROLLER_RC_LOST_ACTION_HOVER = 0,  /*!< Aircraft will execute hover action when RC is lost. */
    DJI_FLIGHT_CONTROLLER_RC_LOST_ACTION_LANDING = 1,  /*!< Aircraft will execute land action when RC is lost. */
    DJI_FLIGHT_CONTROLLER_RC_LOST_ACTION_GOHOME = 2,  /*!< Aircraft will execute go-home action when RC is lost. */
} E_DjiFlightControllerRCLostAction;

/**
 * @brief Enable/Disable RTK position enum
 */
typedef enum {
    DJI_FLIGHT_CONTROLLER_DISABLE_RTK_POSITION = 0, /*!< 0: The aircraft will use GPS data instead of RTK data to execute
                                                     * actions which requires location information(waypoint, go home...)
                                                     */
    DJI_FLIGHT_CONTROLLER_ENABLE_RTK_POSITION = 1, /*!< 1:The aircraft will use RTK data instead of GPS data to execute
                                                    * actions which requires location information(waypoint, go home...)*/
} E_DjiFlightControllerRtkPositionEnableStatus;

/**
 * @brief Enable/Disable obstacle sensing enum
 */
typedef enum {
    DJI_FLIGHT_CONTROLLER_DISABLE_OBSTACLE_AVOIDANCE = 0, /*!< 0: The aircraft will not perform obstacle sensing in
                                                           * the specified direction */
    DJI_FLIGHT_CONTROLLER_ENABLE_OBSTACLE_AVOIDANCE = 1, /*!< 0: The aircraft will perform obstacle sensing in the
                                                          * specified direction */
} E_DjiFlightControllerObstacleAvoidanceEnableStatus;

/**
 * @brief Enable/Disable emergency stop motor function enum
 * @note Enable emergency-stop-motor function is very dangerous in the air. It will make the aircraft crash!!!
 */
typedef enum {
    DJI_FLIGHT_CONTROLLER_ENABLE_EMERGENCY_STOP_MOTOR = 0x01, /*!< Execute emergency-stop-motor action */
} E_DjiFlightControllerEmergencyStopMotor;

/**
 * @brief Obtain/Release joystick control permission command enum
 * @note You have obtained joystick control permission successfully before using joystick.
 */
typedef enum {
    DJI_FLIGHT_CONTROLLER_RELEASE_JOYSTICK_CTRL_AUTHORITY = 0, /*!< Obtain joystick permission */
    DJI_FLIGHT_CONTROLLER_OBTAIN_JOYSTICK_CTRL_AUTHORITY = 1, /*!< Release joystick permission */
} E_DjiFlightControllerJoystickCtrlAuthorityAction;

/**
 * @brief The aircraft's joystick control permission owner enum
 */
typedef enum {
    DJI_FLIGHT_CONTROLLER_JOYSTICK_CTRL_AUTHORITY_RC = 0,  /*!< RC could control aircraft with joystick. */
    DJI_FLIGHT_CONTROLLER_JOYSTICK_CTRL_AUTHORITY_MSDK = 1, /*!< MSDK could control aircraft with joystick. */
    DJI_FLIGHT_CONTROLLER_JOYSTICK_CTRL_AUTHORITY_INTERNAL = 2, /*!< Special Internal modules could control aircraft
                                                                 * with joystick. */
    DJI_FLIGHT_CONTROLLER_JOYSTICK_CTRL_AUTHORITY_OSDK = 4, /*!< PSDK could control aircraft with joystick. */
} E_DjiFlightControllerJoystickCtrlAuthority;

/**
 * @brief The aircraft's joystick control permission switch reason enum
 */
typedef enum {
    DJI_FLIGHT_CONTROLLER_MSDK_GET_JOYSTICK_CTRL_AUTH_EVENT = 1, /*!< MSDK gets the joystick control permission. */
    DJI_FLIGHT_CONTROLLER_INTERNAL_GET_JOYSTICK_CTRL_AUTH_EVENT = 2,  /*!< A specific internal modules gets the joystick control permission. */
    DJI_FLIGHT_CONTROLLER_OSDK_GET_JOYSTICK_CTRL_AUTH_EVENT = 3, /*!< PSDK gets the joystick control permission. */
    DJI_FLIGHT_CONTROLLER_RC_LOST_GET_JOYSTICK_CTRL_AUTH_EVENT = 4, /*!< Reset the joystick control permission to RC when executing RC lost action */
    DJI_FLIGHT_CONTROLLER_RC_NOT_P_MODE_RESET_JOYSTICK_CTRL_AUTH_EVENT = 5,  /*!< Reset the joystick control permission to RC when RC is not in P mode */
    DJI_FLIGHT_CONTROLLER_RC_SWITCH_MODE_GET_JOYSTICK_CTRL_AUTH_EVENT = 6,  /*!< Set the joystick control permission to RC when RC switches control mode(T/APS) */
    DJI_FLIGHT_CONTROLLER_RC_PAUSE_GET_JOYSTICK_CTRL_AUTH_EVENT = 7, /*!< Reset the joystick control permission to RC when RC pauses */
    DJI_FLIGHT_CONTROLLER_RC_REQUEST_GO_HOME_GET_JOYSTICK_CTRL_AUTH_EVENT = 8, /*!< Reset the joystick control permission to RC when RC requests to go home*/
    DJI_FLIGHT_CONTROLLER_LOW_BATTERY_GO_HOME_RESET_JOYSTICK_CTRL_AUTH_EVENT = 9, /*!< Reset the joystick control permission to RC when aircraft is executing low-battery-go-home*/
    DJI_FLIGHT_CONTROLLER_LOW_BATTERY_LANDING_RESET_JOYSTICK_CTRL_AUTH_EVENT = 10, /*!< Reset the joystick control permission to RC when aircraft is executing low-battery-landing*/
    DJI_FLIGHT_CONTROLLER_OSDK_LOST_GET_JOYSTICK_CTRL_AUTH_EVENT = 11, /*!< Reset the joystick control permission to RC when PSDK is lost*/
    DJI_FLIGHT_CONTROLLER_NERA_FLIGHT_BOUNDARY_RESET_JOYSTICK_CTRL_AUTH_EVENT = 12, /*!< Reset the joystick control permission to RC when aircraft is near boundary.*/
    DJI_FLIGHT_CONTROLLER_DOCK_REQUEST_CHANGE_JOYSTICK_CTRL_AUTH_EVENT = 13, /*!< Dock request change the joystick control permission.*/
} E_DjiFlightControllerJoystickCtrlAuthoritySwitchEvent;

/**
 * @brief The aircraft's light status enum
 */
typedef enum {
    DJI_FLIGHT_CONTROLLER_LIGHT_OFF = 0,
    DJI_FLIGHT_CONTROLLER_LIGHT_ON = 1,
    DJI_FLIGHT_CONTROLLER_LIGHT_STATUS_MAX,
} E_DjiFlightControllerLightStatus;

/**
 * @brief The aircraft's joystick control permission switch event info enum
 */
typedef struct {
    E_DjiFlightControllerJoystickCtrlAuthority curJoystickCtrlAuthority; /*!< The aircraft's joystick control permission owner */
    E_DjiFlightControllerJoystickCtrlAuthoritySwitchEvent joystickCtrlAuthoritySwitchEvent; /*!< The aircraft's joystick control permission switch reason */
} T_DjiFlightControllerJoystickCtrlAuthorityEventInfo;

/**
 * @brief Prototype of callback function used to get joystick control permission switch event info.
 * @return Execution result.
 */
typedef T_DjiReturnCode (*JoystickCtrlAuthorityEventCbFunc)(
    T_DjiFlightControllerJoystickCtrlAuthorityEventInfo eventData);

/**
 * @brief Prototype of callback function used to get the trigger FTS event.
 * @return Execution result.
 */
typedef T_DjiReturnCode (*TriggerFtsEventCallback)(void);

/**
 * @brief Horizon control mode in joystick mode enum
 * @note Only when the GPS signal is good (health_flag >=3)，horizontal position control (DJI_FLIGHT_CONTROLLER_HORIZONTAL_POSITION_CONTROL_MODE)
 * related control modes can be used. Only when GPS signal is good (health_flag >=3)，or advanced sensing system is working properly with Autopilot，
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
     * @brief Set the control mode to control horizontal vehicle velocities.
     * @note Need to be referenced to either the ground or body frame by E_DjiFlightControllerHorizontalCoordinate setting
     * Limit: -30m/s to 30 m/s
     */
    DJI_FLIGHT_CONTROLLER_HORIZONTAL_VELOCITY_CONTROL_MODE = 1,
    /**
     * @brief Set the control mode to control position offsets of pitch & roll directions.
     * @note Need to be referenced to either the ground or body frame by E_DjiFlightControllerHorizontalCoordinate setting
     * Limit: N/A
     */
    DJI_FLIGHT_CONTROLLER_HORIZONTAL_POSITION_CONTROL_MODE = 2,
    /**
     * @brief Set the control mode to control rate of change of the vehicle's attitude.
     * @note Need to be referenced to either the ground or body frame by E_DjiFlightControllerHorizontalCoordinate setting
     * Limit: -150deg/s to 150.0 deg/s
     */
    DJI_FLIGHT_CONTROLLER_HORIZONTAL_ANGULAR_RATE_CONTROL_MODE = 3
} E_DjiFlightControllerHorizontalControlMode;

/**
 * @brief Vertical control mode enum in joystick mode
 * @note We don not recommend using VERTICAL_POSITION control mode indoors when the aircraft's flying height exceeds 3 meters.
 * This is because barometer can be inaccurate indoors, and the vertical controller may fail to keep the height of the aircraft.
 */
typedef enum {
    /**
     * @brief Set the control mode to control the vertical speed of aircraft, setting the upward as positive/
     * @note Limit: -5 m/s to 5 m/s
     */
    DJI_FLIGHT_CONTROLLER_VERTICAL_VELOCITY_CONTROL_MODE = 0,

    /**
     * @brief Set the control mode to control the height of aircraft
     * @note Limit: 0 m to 120 m
     */
    DJI_FLIGHT_CONTROLLER_VERTICAL_POSITION_CONTROL_MODE = 1,

    /**
     * @brief Set the control mode to directly control the thrust
     * @note Range: 0 % to 100 %
     */
    DJI_FLIGHT_CONTROLLER_VERTICAL_THRUST_CONTROL_MODE = 2,
} E_DjiFlightControllerVerticalControlMode;

/**
 * @brief Yaw control mode enum in joystick mode
 */
typedef enum {
    /**
     * @brief Set the control mode to control yaw angle.
     * @note Yaw angle is referenced to the ground frame. In this control mode, Ground frame is enforced in Autopilot.
     */
    DJI_FLIGHT_CONTROLLER_YAW_ANGLE_CONTROL_MODE = 0x00,

    /**
     * @brief Set the control-mode to control yaw angular velocity
     * @note Same reference frame as YAW_ANGLE.
     * Limit: -150 deg/s to 150 deg/s
     */
    DJI_FLIGHT_CONTROLLER_YAW_ANGLE_RATE_CONTROL_MODE = 1
} E_DjiFlightControllerYawControlMode;

/**
 * @brief Horizontal coordinate enum in joystick mode
 */
typedef enum {
    DJI_FLIGHT_CONTROLLER_HORIZONTAL_GROUND_COORDINATE = 0, /*!< Set the x-y of ground frame as the horizontal frame (NEU) */
    DJI_FLIGHT_CONTROLLER_HORIZONTAL_BODY_COORDINATE = 1 /*!< Set the x-y of body frame as the horizontal frame (FRU) */
} E_DjiFlightControllerHorizontalCoordinate;

/*!
 * @brief Stable mode enum in joystick mode
 * @note Only works in horizontal velocity control mode. In velocity stable mode, aircraft will brake and hover at one position once
 * the input command is zero. In velocity non-stable mode, aircraft will follow the velocity command and not hover when the command is zero.
 * That means aircraft will drift with the wind.
 */
typedef enum {
    DJI_FLIGHT_CONTROLLER_STABLE_CONTROL_MODE_DISABLE = 0, /*!< Disable the stable mode */
    DJI_FLIGHT_CONTROLLER_STABLE_CONTROL_MODE_ENABLE = 1   /*!< Enable the stable mode */
} E_DjiFlightControllerStableControlMode;

typedef enum {
    DJI_FLIGHT_CONTROLLER_ENABLE_RC_LOST_ACTION = 0,
    DJI_FLIGHT_CONTROLLER_DISABLE_RC_LOST_ACTION = 1,
} E_DjiFlightControllerRCLostActionEnableStatus;


typedef enum {
    DJI_FLIGHT_CONTROLLER_DISABLE_ALL_AVIOD = 0, /*disable all avoid*/
    DJI_FLIGHT_CONTROLLER_ENABLE_ALL_AVIOD = 1,  /*enable all avoid*/
} E_DjiFlightControllerCloseAllAvoidCommand;

typedef enum {
    DJI_FLIGHT_CONTROLLER_NO_MOTOR_IN_SLOW_ROTATE_MODE = 0,
    DJI_FLIGHT_CONTROLLER_SOME_MOTOR_IN_SLOW_ROTATE_MODE = 1,
    DJI_FLIGHT_CONTROLLER_ALL_MOTOR_IN_SLOW_ROTATE_MODE = 2,
} E_DjiFlightControllerElectronicSpeedControllerStatus;

typedef enum {
    DJI_FLIGHT_CONTROLLER_FTS_NOT_TRIGGERD = 0,
    DJI_FLIGHT_CONTROLLER_FTS_TRIGGERD = 1,
} E_DjiFlightControllerFtsStatus;

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
    dji_f32_t x;   /*!< Controls the x axis.*/
    dji_f32_t y;   /*!< Controls the y axis.*/
    dji_f32_t z;   /*!< Controls the z axis, setting upward as positive. */
    dji_f32_t yaw; /*!< Yaw position/velocity control w.r.t. the ground frame.*/
} T_DjiFlightControllerJoystickCommand;// pack(1)

typedef struct {
    dji_f64_t latitude;  /*!< unit: rad */
    dji_f64_t longitude; /*!< unit: rad */
} T_DjiFlightControllerHomeLocation; // pack(1)

typedef struct {
    char serialNum[32];
} T_DjiFlightControllerGeneralInfo;

typedef struct {
    dji_f64_t latitude;  /*!< unit: rad */
    dji_f64_t longitude; /*!< unit: rad */
    uint16_t altitude;
} T_DjiFlightControllerRidInfo;

typedef struct
{
    double lat; /*!< Target point latitude, unit: rad */
    double lon; /*!< Target point longitude, unit: rad */
    float alt;  /*!< Target point altitude, ellipsoidal height unit: meters */
} T_DjiFlightControllerPointInfo;

typedef struct
{
    uint8_t version; /*!< Function version */
    int8_t operation; /*!< New upload of a target point or update of the current task's target point; 0: new order,  1: update goal */
    float mea; /*!< Minimum enroute height, in meters, relative to the takeoff point.
                If the aircraft has not started or is on the ground, it will first climb to this height before executing the task. This height is ignored if the aircraft is in the air. */
    uint8_t fly_vel; /*!< Maximum horizontal flight speed to the target point, in meters per second */
    uint8_t goal_num; /*!< Number of target points, currently only supports one point */
    T_DjiFlightControllerPointInfo cmd_mode_point_info[1]; /*!< Information of the target point */
} T_DjiFlightControllerStartMissionReq;

typedef struct
{
    uint8_t ret_code; /*!< 0: Start task successfully, 1: Start task failed */
    uint16_t error_code; /*!< Instruction flight route version reused route error code */
    uint8_t code_name; /*!< Task code, unique, will be included in the trajectory push to ensure that external modules can correlate the task with its trajectory */
} T_DjiFlightControllerStartMissionRsp;

typedef struct
{
    uint8_t mission_state_machine; /*!< Task state machine, 0 is idle, others are in progress.*/
    uint8_t mission_planning_algo; /*!< Task planning algorithm mode */
    uint8_t goal_index; /*!< Current target point index */
    float distance_remaining; /*!< Remaining task distance */
    float time_remaining; /*!< Remaining task time */
    uint8_t soe_remaining; /*!< Required SOE (State of Energy) */
    uint8_t progress; /*!< Progress, reserved unused */
    uint8_t success_rate; /*!< Task success rate, reserved unused */
} T_DjiFlightControllerOpenMis;

typedef struct
{
    int32_t latitude; /*!< GPS latitude, unit: 1/2^32 * 180°, range: [-90°, 90°) */
    int32_t longitude; /*!< GPS longitude, unit: 1/2^32 * 360°, range: [-180°, 180°) */
    int32_t altitude; /*!< Altitude, unit: mm; Note: The type of altitude depends on the function definition.
                       It could be: relative to the takeoff point, WGS84 absolute ellipsoidal height, EGM96 absolute altitude, barometric height, etc. */
} T_DjiFlightControllerSpotlightZoomGps;

typedef struct
{
    uint8_t code_name; /*!< Unique task code for this trajectory push */
    uint8_t point_num; /*!< Number of key points in the trajectory */
    uint8_t byte_per_point; /*!< Number of bytes per point */
    T_DjiFlightControllerSpotlightZoomGps points[1]; /*!< Array of key points in the trajectory */
    uint8_t last_point_type; /*!< Whether the target point can be reached
                              0: Yes, the last point in the trajectory is the user's target point
                              1: No, the user's target point is in an invalid area (e.g., within an NFZ or building), the last point is the closest point to the target */
} T_DjiFlightControllerCoreTraj;

typedef struct
{
    uint16_t remain_fly_time; /*!< Remaining flight time, unit: second , 0 indicates invalid. Return flight and forced landing are not considered*/
    uint8_t gohome_capacity;  /*!< Returning to home and landing requires power capacity, unit: percent, 0 indicates invalid */
} T_DjiFlightControllerBatteryCapacityGohome;

typedef struct
{
    dji_f64_t longitude; /*!< radian, unit: rad [-180, 180]*/
    dji_f64_t latitude; /*!< radian, unit: rad [-190, 90]*/
    int16_t relative_height; /*!< Relative height to home point, unit: 0.1m */
    int16_t speed_x; /*!< Ground-based X-axis velocity, unit: 0.1m/s */
    int16_t speed_y; /*!< Ground-based Y-axis velocity, unit: 0.1m/s */
    int16_t speed_z; /*!< Ground-based Z-axis velocity, unit: 0.1m/s */
    int16_t pitch; /*!< Pitch axis angle, unit: 0.1 degree [-1800, 1800] */
    int16_t roll; /*!< Roll axis angle, unit: 0.1 degree [-1800, 1800] */
    int16_t yaw; /*!< Yaw axis angle, unit: 0.1 degree [-1800, 1800] */
} T_DjiFlightControllerOsdInfo;

#pragma pack()

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Set all radar avoid action.
 * @note  It only supports FC100.
 * @param rcLostAction: actions when radar action.It inherits from Pilot's param.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_SetAllAvoidAction(E_DjiFlightControllerCloseAllAvoidCommand allAction);

/**
 * @brief Get all radar avoid action.
 * @note It only supports FC100.
 * @param rcLostAction: see reference of E_DjiFlightControllerCloseAllAvoidCommand.It inherits from Pilot's param.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_GetAllAvoidAction(E_DjiFlightControllerCloseAllAvoidCommand *allAction);

/**
 * @brief Initialise flight controller module
 * @param ridInfo: Must report the correct RID information before using PSDK to control the aircraft.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_Init(T_DjiFlightControllerRidInfo ridInfo);

/**
 * @brief DeInitialise flight controller module.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_DeInit(void);

/**
 * @brief Set planning algorithm.
 * @param algo: 0:smart height, 1:Manual height.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_SetPlanningAlgo(uint8_t algo);

/**
 * @brief Set max velocity.
 * @param value: max velocity value, min:1, max:15.
 * @return Execution result.
*/
T_DjiReturnCode DjiFlightController_SetMaxVelocity(uint8_t value);

/**
 * @brief Set min flight height.
 * @param value: min flight height value, min:1.0, max:3000.0, only SetPlanningAlgo 1 effective.
 * @return Execution result.
*/
T_DjiReturnCode DjiFlightController_SetMinFlightHeight(float value);

/**
 * @brief Get exit reason.
 * @param reason: exit reason".
 * @return Execution result.
*/
T_DjiReturnCode DjiFlightController_GetExitReason(uint16_t *reason);

/**
* @brief Enable or disable PSDK control when the aircraft is in Attitude (ATTI) mode.
 * After this setting is applied, it controls whether the payload SDK (PSDK) is allowed to
 * send control commands while the aircraft is in Attitude (ATTI) mode.
 * @param      enabled  Enable flag.
 *                    - true:  Allow PSDK control in Attitude (ATTI) mode.
 *                    - false: Disallow PSDK control in Attitude (ATTI) mode.
 * @return Execution result.
*/
T_DjiReturnCode DjiFlightController_SetControlInAttitudeModeEnabled(bool enabled);

/**
 * @brief brief:Get whether PSDK control is enabled when the aircraft is in Attitude (ATTI) mode.
 * @param value: 0 disable, 1 enable".
 * @return Execution result.
*/
T_DjiReturnCode DjiFlightController_GetControlInAttitudeModeEnabled(bool *enabled);

/**
 * @brief Prototype of callback function used to get osd info.
 * @return Execution result.
*/
typedef T_DjiReturnCode (*FcCmderOsdInfoCbFunc)(T_DjiFlightControllerOsdInfo eventData);

/**
 * @brief Prototype of callback function used to get gohome battery capacity.
 * @return Execution result.
*/
typedef T_DjiReturnCode (*FcCmderBatteryCapacityGohomeCbFunc)(T_DjiFlightControllerBatteryCapacityGohome eventData);

/**
 * @brief Prototype of callback function used to get open mis info.
 * @return Execution result.
 */
typedef T_DjiReturnCode (*FcCmderModeOpenMisEventCbFunc)(T_DjiFlightControllerOpenMis eventData);

/**
 * @brief Prototype of callback function used to get core traj info.
 * @return Execution result.
 */
typedef T_DjiReturnCode (*FcCmderModeCoreTrajEventCbFunc)(T_DjiFlightControllerCoreTraj eventData);

/**
 * @brief Register callback function for the osd info, see reference of T_DjiFlightControllerOsdInfo.
 * @note This API is restricted to specific controlled scenarios and is not enabled for general external use by default.
 *       For information regarding its actual availability, please contact our official support team.
 * @param callback: the callback for the osd info.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_RegisterOsdInfoCallBack(FcCmderOsdInfoCbFunc callback);

/**
 * @brief Register callback function for the gohome battery capacity.
 * @param callback: the callback for the gohome battery capacity.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_RegisterBatteryCapacityGohomeCallBack(FcCmderBatteryCapacityGohomeCbFunc callback);

/**
 * @brief antiregister callback function for the gohome battery capacity.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_AntiRegisterBatteryCapacityGohomeCallBack();

/**
 * @brief Register callback function for the open mis event.
 * @param callback: the callback for the open mis  event.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_RegisterOpenMisInfoCallBack(FcCmderModeOpenMisEventCbFunc callback);

/**
 * @brief Register callback function for the core traj event.
 * @param callback: the callback for the core traj event.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_RegisterCoreTrajCallBack(FcCmderModeCoreTrajEventCbFunc callback);

/**
 * @brief antiregister callback function for the open mis event.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_AntiRegisterOpenMisInfoCallBack(void);

/**
 * @brief antiregister callback function for the core traj event.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_AntiRegisterCoreTrajCallBack(void);

/**
 * @brief set mode start mission.
 * @param command: cmd for start mission.
 * @param rsp: response data for set start mission.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_SetModeStartMission(T_DjiFlightControllerStartMissionReq command,
                                                       T_DjiFlightControllerStartMissionRsp *rsp);

/**
 * @brief Enable/Disable RTK position function.
 * @details Enabling RTK means that RTK data will be used instead of GPS during flight.
 * @param rtkEnableStatus: refer to "E_DjiFlightControllerRtkPositionEnableStatus", inheriting from Pilot.
 * @return Execution result.
 */
T_DjiReturnCode
DjiFlightController_SetRtkPositionEnableStatus(E_DjiFlightControllerRtkPositionEnableStatus rtkEnableStatus);

/**
 * @brief Get RTK enable status.
 * @note Enabling RTK means that RTK data will be used during intelligent flight.
 * @param rtkEnableStatus: refer to "E_DjiFlightControllerRtkPositionEnableStatus", inheriting from Pilot.
 * @return Execution result.
 */
T_DjiReturnCode
DjiFlightController_GetRtkPositionEnableStatus(E_DjiFlightControllerRtkPositionEnableStatus *rtkEnableStatus);

/**
 * @brief Set RC lost action.
 * @note Valid when RC and PSDK are both lost. It only supports M30.
 * @param rcLostAction: actions when RC is lost.(hover/landing/go home).It inherits from Pilot's param.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_SetRCLostAction(E_DjiFlightControllerRCLostAction rcLostAction);

/**
 * @brief Get RC lost action(hover/landing/gohome).
 * @note Valid when RC and PSDK are both lost. It only supports M30.
 * @param rcLostAction: see reference of E_DjiFlightControllerRCLostAction.It inherits from Pilot's param.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_GetRCLostAction(E_DjiFlightControllerRCLostAction *rcLostAction);

/**
 * @brief Enable/Disable horizontal visual(forwards,backwards,left,right) obstacle sensing.
 * @note For detailed parameters of obstacle sensing, it is recommended to read the official user manual in
 * https://www.dji.com.
 * @param horizontalObstacleAvoidanceEnableStatus: see reference of E_DjiFlightControllerObstacleAvoidanceEnableStatus.
 * It inherits from Pilot's param.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_SetHorizontalVisualObstacleAvoidanceEnableStatus(
    E_DjiFlightControllerObstacleAvoidanceEnableStatus horizontalObstacleAvoidanceEnableStatus);

/**
 * @brief Get the switch status of horizontal visual(forwards,backwards,left,right) obstacle sensing.
 * @note For detailed parameters of obstacle sensing, it is recommended to read the official user manual in
 * https://www.dji.com.
 * @param horizontalObstacleAvoidanceEnableStatus: see reference of E_DjiFlightControllerObstacleAvoidanceEnableStatus.
 * It inherits from Pilot's param.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_GetHorizontalVisualObstacleAvoidanceEnableStatus(
    E_DjiFlightControllerObstacleAvoidanceEnableStatus *horizontalObstacleAvoidanceEnableStatus);

/**
 * @brief Enable/Disable horizontal radar obstacle sensing.
 * @note It will be valid only if you install CSM radar successfully.For detailed parameters of obstacle sensing,
 * it is recommended to read the official user manual in https://www.dji.com.
 * @param horizontalObstacleAvoidanceEnableStatus: see reference of E_DjiFlightControllerObstacleAvoidanceEnableStatus.
 * It inherits from Pilot's param.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_SetHorizontalRadarObstacleAvoidanceEnableStatus(
    E_DjiFlightControllerObstacleAvoidanceEnableStatus horizontalObstacleAvoidanceEnableStatus);

/**
 * @brief Get the switch status of horizontal radar obstacle sensing.
 * @note It will be valid only if you install CSM radar successfully.For detailed parameters of obstacle sensing,
 * it is recommended to read the official user manual in https://www.dji.com/uk/matrice-300/downloads.
 * @param horizontalObstacleAvoidanceEnableStatus: see reference of E_DjiFlightControllerObstacleAvoidanceEnableStatus.
 * It inherits from Pilot's param.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_GetHorizontalRadarObstacleAvoidanceEnableStatus(
    E_DjiFlightControllerObstacleAvoidanceEnableStatus *horizontalObstacleAvoidanceEnableStatus);

/**
 * @brief Enable/Disable upwards visual obstacle sensing.
 * @note For detailed parameters of obstacle sensing, it is recommended to read the official user manual in
 * https://www.dji.com.
 * @param upwardsObstacleAvoidanceEnableStatus: see reference of E_DjiFlightControllerObstacleAvoidanceEnableStatus.
 * It inherits from Pilot's param.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_SetUpwardsVisualObstacleAvoidanceEnableStatus(
    E_DjiFlightControllerObstacleAvoidanceEnableStatus upwardsObstacleAvoidanceEnableStatus);

/**
 * @brief Get the switch status of upwards visual obstacle sensing.
 * @note For detailed parameters of obstacle sensing, it is recommended to read the official user manual in
 * https://www.dji.com.
 * @param upwardsObstacleAvoidanceEnableStatus: see reference of E_DjiFlightControllerObstacleAvoidanceEnableStatus.
 * It inherits from Pilot's param.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_GetUpwardsVisualObstacleAvoidanceEnableStatus(
    E_DjiFlightControllerObstacleAvoidanceEnableStatus *upwardsObstacleAvoidanceEnableStatus);

/**
 * @brief Enable/Disable upwards radar obstacle sensing.
 * @note It will be valid only if you install CSM radar successfully.For detailed parameters of obstacle sensing,
 * it is recommended to read the official user manual in https://www.dji.com.
 * @param upwardsObstacleAvoidanceEnableStatus: see reference of E_DjiFlightControllerObstacleAvoidanceEnableStatus.
 * It inherits from Pilot's param.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_SetUpwardsRadarObstacleAvoidanceEnableStatus(
    E_DjiFlightControllerObstacleAvoidanceEnableStatus upwardsObstacleAvoidanceEnableStatus);

/**
 * @brief Get the switch status of upwards radar obstacle sensing.
 * @note For detailed parameters of obstacle sensing, it is recommended to read the official user manual in
 * https://www.dji.com.
 * @param upwardsObstacleAvoidanceEnableStatus: see reference of E_DjiFlightControllerObstacleAvoidanceEnableStatus.
 * It inherits from Pilot's param.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_GetUpwardsRadarObstacleAvoidanceEnableStatus(
    E_DjiFlightControllerObstacleAvoidanceEnableStatus *upwardsObstacleAvoidanceEnableStatus);

/**
 * @brief Enable/Disable downwards visual obstacle sensing.
 * @note For detailed parameters of obstacle sensing, it is recommended to read the official user manual in
 * https://www.dji.com.
 * @param downwardsObstacleAvoidanceEnableStatus: see reference of E_DjiFlightControllerObstacleAvoidanceEnableStatus.
 * It inherits from Pilot's param.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_SetDownwardsVisualObstacleAvoidanceEnableStatus(
    E_DjiFlightControllerObstacleAvoidanceEnableStatus downwardsObstacleAvoidanceEnableStatus);

/**
 * @brief Get the switch status of downwards visual obstacle sensing.
 * @note For detailed parameters of obstacle sensing, it is recommended to read the official user manual in
 * https://www.dji.com.
 * @param downwardsObstacleAvoidanceEnableStatus: see reference of E_DjiFlightControllerObstacleAvoidanceEnableStatus.
 * It inherits from Pilot's param.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_GetDownwardsVisualObstacleAvoidanceEnableStatus(
    E_DjiFlightControllerObstacleAvoidanceEnableStatus *downwardsObstacleAvoidanceEnableStatus);

/**
 * @brief Arrest flying means emergency braking
 * @note When the aircraft is on the ground, it will stop motors and display "hms description" on App. when the aircraft is
 * in the air, it will continue flying and display "hms description" on App only.
 * If you use this interface, you need to use "DjiFlightController_CancelArrestFlying" to quit arrest-flying status, then
 * then the aircraft can fly again.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_ArrestFlying(void);

/**
 * @brief Quit status of arrest-flying.
 * @note The aircraft need to quit status of arrest-flying to continue flying after arresting flying.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_CancelArrestFlying(void);

/**
 * @brief Turn on motors when the aircraft is on the ground.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_TurnOnMotors(void);

/**
 * @brief Turn off motors when the aircraft is on the ground.
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
 * @brief Request taking off action when the aircraft is on the ground.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_StartTakeoff(void);

/**
 * @brief Request landing action when the aircraft is in the air.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_StartLanding(void);

/**
 * @brief Request cancelling landing action when the aircraft is landing
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_CancelLanding(void);

/**
 * @brief Confirm the landing when the aircraft is 0.7 m above the ground.
 * @note When the clearance between the aircraft and the ground is less than 0.7m, the aircraft will pause landing and
 * wait for user's confirmation. This API is for confirm landing. If the ground is not suitable for landing, user
 * must use RC to control it landing manually or force landing.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_StartConfirmLanding(void);

/**
 * @brief Force landing in any case.
 * @note This API will ignore the smart landing function. When using this API, it will landing directly (would not stop
 * at 0.7m and wait user's command). Attention: it may make the aircraft crash!!!
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_StartForceLanding(void);

/**
 * @brief Set customized GPS(not RTK) home location.
 * @note Set customized home location failed reason may as follows:
 * 1. The distance between new home location and last home location is larger than MAX_FLY_RADIUS(20 km).
 * 2. Record initial home location failed after start aircraft.
 * @param homeLocation: homeLocation include latitude and longitude
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_SetHomeLocationUsingGPSCoordinates(T_DjiFlightControllerHomeLocation homeLocation);

/**
 * @brief Set home location using current aircraft GPS (not RTK) location.
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
 * Go home altitude setting is 20-1500 m.
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
 * @brief Get country code.
 * @param countryCode: Pointer of buffer to return country code. The country code indicates the current country or
 * region where the aircraft is located. Please refer to the ISO 3166-1 code table for the specific meaning of the
 * country code.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_GetCountryCode(uint16_t *countryCode);

/**
 * @brief Request go home action when the aircraft is in the air
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_StartGoHome(void);

/**
 * @brief Request cancel go home action when the aircraft is going home
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_CancelGoHome(void);

/**
 * @brief Obtain aircraft's joystick control permission.
 * @note 1.You have to obtain joystick control permission successfully before you using joystick to control aircraft.
 * 2. RC must be in p-mode.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_ObtainJoystickCtrlAuthority(void);

/**
 * @brief Release aircraft's joystick control permission.
 * @note RC must be in p-mode.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_ReleaseJoystickCtrlAuthority(void);

/**
 * @brief Subscribe to joystick control permission switch event with a callback function.
 * @note it will be triggered once the joystick control permission switch event occurs.
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
 * "DjiFlightController_CancelEmergencyBrakeAction()" to allow aircraft to execute aircraft action again.
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

/*! @brief The command decides whether execute RC lost action or not when PSDK is running
  * @note  This setting only affects the behavior of the aircraft when the RC lost and the PSDK is connected.
  *         if the command is enable, the aircraft will not execute RC lost action when RC is lost but PSDK is running;
  *         if the command is disable, the aircraft will execute RC lost action when RC is lost but PSDK is running
  *         the aircraft will execute RC lost action when RC is lost and PSDK is lost whatever the command is.
  *         default command is disable.
  * @param executeRCLostActionOrNotWhenOnboardOn  enable:1;disable:0
  * @return T_DjiReturnCode error code
   */
T_DjiReturnCode
DjiFlightController_SetRCLostActionEnableStatus(E_DjiFlightControllerRCLostActionEnableStatus command);

/*! @brief get RC lost action enable status(enable or disable)
 *  @param command executeRCLostActionOrNotWhenOnboardOn, enable:1;disable:0
 *  @return  T_DjiReturnCode error code
 */
T_DjiReturnCode
DjiFlightController_GetEnableRCLostActionStatus(E_DjiFlightControllerRCLostActionEnableStatus *command);

/**
 * @brief Register callback function for the trigger FTS event.
 * @note The timing of the trigger of the callback function of the FTS is determined by the aircraft, and the trigger
 *       execution action of the FTS needs to be implemented in the callback function and the correct return value
*        must be returned, otherwise the aircraft will always be triggered.
 * @param callback: the callback for the trigger FTS event.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_RegTriggerFtsEventCallback(TriggerFtsEventCallback callback);

/**
 * @brief Start to rotate motors slowly.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_StartSlowRotateMotor(void);

/**
 * @brief Stop to rotate motors slowly.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_StopSlowRotateMotor(void);

/**
 * @brief Get the status of the ESC.
 * @param status: The status of the motor on aircraft.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_GetElectronicSpeedControllerStatus(E_DjiFlightControllerElectronicSpeedControllerStatus *status);

/*!
 * @brief This API is used to get the status of the arm lights.
 * @note
 * 1. This interface is used to obtain the on/off status of the arm lights and is supported only on certain aircraft models.
 * @param lightStatus Pointer to the light status.
 *                    When the arm lights are on, the value of `lightStatus` will be `DJI_FLIGHT_CONTROLLER_LIGHT_ON`.
 *                    Otherwise, it will be `DJI_FLIGHT_CONTROLLER_LIGHT_OFF`.
 *                    The value of `lightStatus` will only be one of the `E_DjiFlightControllerLightStatus` enum values; otherwise, it indicates an abnormal state.
 * @return T_DjiReturnCode error code
 * 1. If the aircraft model does not support this function, the error code `DJI_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT` will be returned.
 */
T_DjiReturnCode DjiFlightController_GetArmLightStatus(E_DjiFlightControllerLightStatus *lightStatus);

/*! @brief This API is used to set the status of the arm lights.
 *  @note
 *  1. This interface is used to set the on/off status of the arm lights and is supported only on certain aircraft models.
 *  2. The status of the arm lights can be set to `DJI_FLIGHT_CONTROLLER_LIGHT_ON` or `DJI_FLIGHT_CONTROLLER_LIGHT_OFF`.
 *  3. After setting, you can call `DjiFlightController_GetArmLightStatus` to query the status.
 * @param lightStatus The light status you wish to set.
 *                     The value of `lightStatus` will only be one of the `E_DjiFlightControllerLightStatus` enum values; otherwise, it indicates an abnormal state.
 *  @return T_DjiReturnCode error code
 *  1. If the aircraft model does not support this function, the error code `DJI_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT` will be returned.
 *  2. After the aircraft restarts, the arm light will retain the state they were in before the aircraft was powered off.
 *     This API is no longer maintained. Whether the status is retained after power loss varies by firmware version and aircraft model,
 *     the actual behavior should be verified through testing on the specific aircraft model.
 */
T_DjiReturnCode DjiFlightController_SetArmLightStatus(E_DjiFlightControllerLightStatus lightStatus);

/*! @brief This API is used to get the status of the bottom lights.
 *  @note
 *  1. This interface is used to obtain the on/off status of the bottom lights and is supported only on certain aircraft models.
 *  @param lightStatus Pointer to the light status.
 *                     When the bottom lights are on, the value of `lightStatus` will be `DJI_FLIGHT_CONTROLLER_LIGHT_ON`.
 *                     Otherwise, it will be `DJI_FLIGHT_CONTROLLER_LIGHT_OFF`.
 *                     The value of `lightStatus` will only be one of the `E_DjiFlightControllerLightStatus` enum values; otherwise, it indicates an abnormal state.
 *  @return T_DjiReturnCode error code
 *  1. If the aircraft model does not support this function, the error code `DJI_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT` will be returned.
 *  2. This API can only get the current on/off status of the light, it cannot get the current mode setting of the light.
 */
T_DjiReturnCode DjiFlightController_GetBottomLightStatus(E_DjiFlightControllerLightStatus *lightStatus);

/*! @brief This API is used to set the status of the bottom lights.
 *  @note
 *  1. This interface is used to set the on/off status of the bottom lights and is supported only on certain aircraft models.
 *  2. The status of the bottom lights can be set to `DJI_FLIGHT_CONTROLLER_LIGHT_ON` or `DJI_FLIGHT_CONTROLLER_LIGHT_OFF`.
 * @param lightStatus The light status you wish to set.
 *                     The value of `lightStatus` will only be one of the `E_DjiFlightControllerLightStatus` enum values; otherwise, it indicates an abnormal state.
 *  @return T_DjiReturnCode error code
 *  1. If the aircraft model does not support this function, the error code `DJI_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT` will be returned.
 *  2. After the aircraft restarts, the bottom light will revert to its default state.
 *     This API is no longer maintained. Whether the status is retained after power loss varies by firmware version and aircraft model,
 *     the actual behavior should be verified through testing on the specific aircraft model.
 *  3. When the bottom light is turned on, this API will set the bottom light to automatic mode.
 *     Note: in automatic mode, the light's on/off status is influenced by ambient light conditions.
 *           The light will automatically turn on at night and turn off during the day.
 */
T_DjiReturnCode DjiFlightController_SetBottomLightStatus(E_DjiFlightControllerLightStatus lightStatus);

/*! @brief This API is used to get the status of the battery lights.
 *  @note
 *  1. This interface is used to obtain the on/off status of the battery lights and is supported only on certain aircraft models.
 *  @param lightStatus Pointer to the light status.
 *                     When the battery lights are on, the value of `lightStatus` will be `DJI_FLIGHT_CONTROLLER_LIGHT_ON`.
 *                     Otherwise, it will be `DJI_FLIGHT_CONTROLLER_LIGHT_OFF`.
 *                     The value of `lightStatus` will only be one of the `E_DjiFlightControllerLightStatus` enum values; otherwise, it indicates an abnormal state.
 *  @return T_DjiReturnCode error code
 *  1. If the aircraft model does not support this function, the error code `DJI_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT` will be returned.
 */
T_DjiReturnCode DjiFlightController_GetBatteryLightStatus(E_DjiFlightControllerLightStatus *lightStatus);

/*! @brief This API is used to set the status of the battery lights.
 *  @note
 *  1. This interface is used to set the on/off status of the battery lights and is supported only on certain aircraft models.
 *  2. The status of the battery lights can be set to `DJI_FLIGHT_CONTROLLER_LIGHT_ON` or `DJI_FLIGHT_CONTROLLER_LIGHT_OFF`.
 *  3. Approximately 3 seconds after setting, you can call `DjiFlightController_GetBatteryLightStatus` to query the status.
 *     If you query the status immediately after setting, you may still get the old status.
 * @param lightStatus The light status you wish to set.
 *                     The value of `lightStatus` will only be one of the `E_DjiFlightControllerLightStatus` enum values; otherwise, it indicates an abnormal state.
 *  @return T_DjiReturnCode error code
 *  1. If the aircraft model does not support this function, the error code `DJI_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT` will be returned.
 *  2. After the aircraft restarts, the battery light will retain the state they were in before the aircraft was powered off.
 *     This API is no longer maintained. Whether the status is retained after power loss varies by firmware version and aircraft model,
 *     the actual behavior should be verified through testing on the specific aircraft model.
 */
T_DjiReturnCode DjiFlightController_SetBatteryLightStatus(E_DjiFlightControllerLightStatus lightStatus);

#ifdef __cplusplus
}
#endif

#endif // DJI_FLIGHT_CONTROLLER_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
