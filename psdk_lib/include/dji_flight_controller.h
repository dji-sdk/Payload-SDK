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

/* Exported types (SPZ tracking) ---------------------------------------------*/
/**
 * @brief Track type for spotlight-zoom target selection.
 */
typedef enum {
    DJI_FLIGHT_CONTROLLER_SPZ_TRACK_BY_TARGET_ID = 0, /*!< Select target by ID from MOT push callback. */
    DJI_FLIGHT_CONTROLLER_SPZ_TRACK_BY_BOX       = 1, /*!< Select target by user-drawn bounding box. */
    DJI_FLIGHT_CONTROLLER_SPZ_TRACK_CONFIRM        = 3, /*!< Confirm target, no additional data (action_type=CONFIRM_TARGET). */
} E_DjiFlightControllerSpzTrackType;

/**
 * @brief Bounding box for spotlight-zoom target selection.
 *        All coordinates are in units of 1/10000 of the frame dimension.
 */
typedef struct {
    uint16_t cx; /*!< Box center X, unit: 1/10000 of frame width. */
    uint16_t cy; /*!< Box center Y, unit: 1/10000 of frame height. */
    uint16_t w;  /*!< Box width,    unit: 1/10000 of frame width. */
    uint16_t h;  /*!< Box height,   unit: 1/10000 of frame height. */
} T_DjiFlightControllerSpzBox;

/**
 * @brief Parameters for spotlight-zoom target tracking selection.
 */
typedef struct {
    E_DjiFlightControllerSpzTrackType trackType;
    union {
        uint32_t                    targetId; /*!< Target ID from MOT push; only lower 16 bits sent on wire. */
        T_DjiFlightControllerSpzBox box;      /*!< Bounding box in 1/10000 frame coordinates. */
    };
} T_DjiFlightControllerSpotlightZoomTrackParam;

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
    DJI_FLIGHT_CONTROLLER_RC_LOST_ACTION_GO_UP = 3,  /*!< Aircraft will ascend to the height configured by DjiFlightController_SetRCLostGoUpHeight and then go home when RC is lost. */
    DJI_FLIGHT_CONTROLLER_RC_LOST_ACTION_RETURN_HOME = 4,  /*!< Aircraft will execute return to home action when RC is lost. */
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
 * @brief Spotlight-zoom approach mode enum.
 *        Controls whether the aircraft enforces a minimum distance to the target.
 *        The setting is saved to EEPROM and persists across power cycles. Default: normal mode (1).
 */
typedef enum {
    DJI_FLIGHT_CONTROLLER_SPZ_APPROACH_MODE  = 0, /*!< Approach mode: no minimum distance limit; aircraft can get close to target. */
    DJI_FLIGHT_CONTROLLER_SPZ_NORMAL_MODE    = 1, /*!< Normal mode (default): minimum distance limit is enforced. */
} E_DjiFlightControllerSpzTooCloseLimitMode;

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
 * @brief Enable/Disable spotlight-zoom auto zoom function enum.
 * @note The setting is saved to EEPROM and persists across power cycles.
 */
typedef enum {
    DJI_FLIGHT_CONTROLLER_SPZ_AUTO_ZOOM_DISABLE = 0, /*!< Disable spotlight-zoom auto zoom. */
    DJI_FLIGHT_CONTROLLER_SPZ_AUTO_ZOOM_ENABLE  = 1, /*!< Enable spotlight-zoom auto zoom (default). */
} E_DjiFlightControllerSpzAutoZoomStatus;

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
    DJI_FLIGHT_CONTROLLER_LIGHT_OFF = 0,       /*!< Light off (bottom light: always off) */
    DJI_FLIGHT_CONTROLLER_LIGHT_ON = 1,         /*!< Light on (bottom light: always on during flight, off after landing) */
    DJI_FLIGHT_CONTROLLER_LIGHT_AUTO = 2,       /*!< Bottom light only: automatic mode controlled by vision system */
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
    DJI_FLIGHT_CONTROLLER_DISABLE_HOIST_OPTION_IN_FLIGHT = 0, /*disable hoist action in flight*/
    DJI_FLIGHT_CONTROLLER_ENABLE_HOIST_OPTION_IN_FLIGHT = 1,  /*enable hoist action in flight*/
} E_DjiFlightControllerHoistActionInFlightCommand;

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
 * @brief Enable/disable status of multiple positioning mode.
 * @note  Must be set to DJI_FLIGHT_CONTROLLER_MULTIPLE_MODE_ENABLE before configuring
 *        assist_positioning_mode to PSDK (ADN mode).
 */
typedef enum {
    DJI_FLIGHT_CONTROLLER_MULTIPLE_MODE_DISABLE = 0, /*!< Multiple positioning mode disabled. */
    DJI_FLIGHT_CONTROLLER_MULTIPLE_MODE_ENABLE  = 2, /*!< Disable Gnss mode. */
} E_DjiFlightControllerMultipleMode;

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

/**
 * @brief Flight controller mode type, from DJI OSD push protocol (cmd_id 0x43).
 *        Values correspond to DJI_MC_MODE_TYPE in the internal protocol definition.
 */
typedef uint8_t E_DjiMcModeType;
#define E_DJI_MC_MODE_TYPE_MANUAL                   ((E_DjiMcModeType)0)
#define E_DJI_MC_MODE_TYPE_ATTI                     ((E_DjiMcModeType)1)
#define E_DJI_MC_MODE_TYPE_GPS_ATTI                 ((E_DjiMcModeType)6)   /*!< GPS / P mode */
#define E_DJI_MC_MODE_TYPE_GO_HOME                  ((E_DjiMcModeType)15)  /*!< Return to home */
#define E_DJI_MC_MODE_TYPE_JOYSTICK                 ((E_DjiMcModeType)17)  /*!< Joystick/SDK control */
#define E_DJI_MC_MODE_TYPE_GPS_SPORT                ((E_DjiMcModeType)31)  /*!< Sport mode */
#define E_DJI_MC_MODE_TYPE_NAVI_SUBMODE_NAVI_POI   ((E_DjiMcModeType)50)  /*!< Navigation POI mode */
#define E_DJI_MC_MODE_TYPE_NAVI_SUBMODE_TA          ((E_DjiMcModeType)51)  /*!< Target Acquisition: SPZ active tracking */
#define E_DJI_MC_MODE_TYPE_MASTERSHOT               ((E_DjiMcModeType)58)
#define E_DJI_MC_MODE_TYPE_SPOTLIGHT_NORMAL         ((E_DjiMcModeType)69)
#define E_DJI_MC_MODE_TYPE_SPOTLIGHT_TRIPOD         ((E_DjiMcModeType)70)
#define E_DJI_MC_MODE_TYPE_SPOTLIGHT_SPORT          ((E_DjiMcModeType)71)
/* Full value list: refer to DJI_MC_MODE_TYPE in dji_ofdm_define.h / DJI_V1_3_67_COMMANDS protocol */

typedef struct
{
    dji_f64_t longitude;        /*!< radian, unit: rad [-180, 180]*/
    dji_f64_t latitude;         /*!< radian, unit: rad [-90, 90]*/
    int16_t relative_height;    /*!< Relative height to home point, unit: 0.1m */
    int16_t speed_x;            /*!< Ground-based X-axis velocity, unit: 0.1m/s */
    int16_t speed_y;            /*!< Ground-based Y-axis velocity, unit: 0.1m/s */
    int16_t speed_z;            /*!< Ground-based Z-axis velocity, unit: 0.1m/s */
    int16_t pitch;              /*!< Pitch axis angle, unit: 0.1 degree [-1800, 1800] */
    int16_t roll;               /*!< Roll axis angle, unit: 0.1 degree [-1800, 1800] */
    int16_t yaw;                /*!< Yaw axis angle, unit: 0.1 degree [-1800, 1800] */
    int16_t yaw_rate;           /*!< Yaw angular rate, unit: 0.1 degree/s */
    int16_t agl_height;         /*!< True height above ground (AGL), unit: 0.1m */
    int32_t hsml;               /*!< GPS absolute altitude (height above sea mean level), unit: 0.001m */
    E_DjiMcModeType mode;       /*!< Flight controller mode, refer to E_DjiMcModeType */
    uint8_t failsafe;           /*!< RC failsafe flag: 1 = RC signal lost */
    uint8_t ground_or_sky;      /*!< Aircraft position: 0 = unknown, 1 = on ground, 2 = in air */
    uint8_t is_motor_up;        /*!< Motor running flag: 1 = motors spinning */
    uint8_t is_gps_valid;       /*!< GPS available flag: 1 = GPS is valid */
    uint8_t gps_num;            /*!< Number of GPS satellites */
    uint8_t positioning_signal_level; /*!< Positioning signal strength level (0-5: GPS, 10: RTK) */
    uint8_t positional_src_type;  /*!< Positioning source: 0=Unknown, 1=MVO, 2=GPS, 3=RTK */
    uint8_t heading_src_type;     /*!< Heading source: 0=Gyroscope, 1=Compass, 2=RTK */
    uint8_t battery_warning;      /*!< Battery warning level: 0=normal, 1=level-1, 2=level-2 */
    uint8_t gohome_landing_reason; /*!< Reason for go-home or landing action */
    uint8_t motor_start_error_code; /*!< Motor start failure code (continuously updated) */
    uint8_t wind_spd_dm;          /*!< Estimated wind speed, unit: dm/s */
    uint8_t wind_dir;             /*!< Wind direction (clockwise): 1=N, 2=NE, 3=E, 4=SE, 5=S, 6=SW, 7=W, 8=NW */
    uint8_t product_type;         /*!< Aircraft type, refer to DJI_DRONE_TYPE */
    uint8_t heading_level_ok;     /*!< Heading fusion quality flag: 1 = heading fusion level >= 6, 0 = heading fusion level < 6 */
} T_DjiFlightControllerOsdInfo;

#pragma pack()

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Set all radar avoid action.
 * @note  It only supports FC100, FC200
 * @param rcLostAction: actions when radar action.It inherits from Pilot's param.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_SetAllAvoidAction(E_DjiFlightControllerCloseAllAvoidCommand allAction);

/**
 * @brief Get all radar avoid action.
 * @note It only supports FC100,FC200
 * @param rcLostAction: see reference of E_DjiFlightControllerCloseAllAvoidCommand.It inherits from Pilot's param.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_GetAllAvoidAction(E_DjiFlightControllerCloseAllAvoidCommand *allAction);

/**
 * @brief Set allow conctrl cable about standard hosit for FC100
 * @note  It only supports FC100.
 * @param rcLostAction: E_DjiFlightControllerHoistActionInFlightCommand,
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_SetHoistActionInFlight(E_DjiFlightControllerHoistActionInFlightCommand isEnable);

/**
 * @brief Get allow conctrl cable about standard hosit for FC100
 * @note It only supports FC100.
 * @param rcLostAction: see reference of E_DjiFlightControllerHoistActionInFlightCommand.It inherits from Pilot's param.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_GetHoistActionInFlight(E_DjiFlightControllerHoistActionInFlightCommand *isEnable);

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
 * @note This API is not available in the current version.
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
 * @note
 *       1.This API is restricted to specific controlled scenarios and is not enabled for general external use by default.
 *       For information regarding its actual availability, please contact our official support team.
 *       2.This API is not available in the current version.
 * @param callback: the callback for the osd info.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_RegisterOsdInfoCallBack(FcCmderOsdInfoCbFunc callback);
/**
 * @brief Unregister callback function for the osd info.
 * @note This API is not available in the current version.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_UnregisterOsdInfoCallBack(void);

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
 * 2. This API is not available in the current version.
 * @param lightStatus Pointer to the light status.
 *                    When the arm lights are on, the value of `lightStatus` will be `DJI_FLIGHT_CONTROLLER_LIGHT_ON`.
 *                    Otherwise, it will be `DJI_FLIGHT_CONTROLLER_LIGHT_OFF`.
 *                    The value of `lightStatus` will only be one of the `E_DjiFlightControllerLightStatus` enum values; otherwise, it indicates an abnormal state.
 * @return T_DjiReturnCode error code
 *  1. If the aircraft model does not support this function, the error code `DJI_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT` will be returned.
 *  2. This API is not available in the current version.
 */
T_DjiReturnCode DjiFlightController_GetArmLightStatus(E_DjiFlightControllerLightStatus *lightStatus);

/*! @brief This API is used to set the status of the arm lights.
 *  @note
 *  1. This interface is used to set the on/off status of the arm lights and is supported only on certain aircraft models.
 *  2. The status of the arm lights can be set to `DJI_FLIGHT_CONTROLLER_LIGHT_ON` or `DJI_FLIGHT_CONTROLLER_LIGHT_OFF`.
 *  3. After setting, you can call `DjiFlightController_GetArmLightStatus` to query the status.
 *  4. This API is not available in the current version.
 * @param lightStatus The light status you wish to set.
 *                     The value of `lightStatus` will only be one of the `E_DjiFlightControllerLightStatus` enum values; otherwise, it indicates an abnormal state.
 *  @return T_DjiReturnCode error code
 *  1. If the aircraft model does not support this function, the error code `DJI_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT` will be returned.
 *  2. After the aircraft restarts, the arm light will retain the state they were in before the aircraft was powered off.
 *     This API is no longer maintained. Whether the status is retained after power loss varies by firmware version and aircraft model,
 *     the actual behavior should be verified through testing on the specific aircraft model.
 *  3. This API is not available in the current version.
 */
T_DjiReturnCode DjiFlightController_SetArmLightStatus(E_DjiFlightControllerLightStatus lightStatus);

/*! @brief This API is used to get the status of the bottom lights.
 *  @note
 *  1. This interface is used to obtain the on/off status of the bottom lights and is supported only on certain aircraft models.
 *  2. This API is not available in the current version.
 *  @param lightStatus Pointer to the light status.
 *                     When the bottom lights are on, the value of `lightStatus` will be `DJI_FLIGHT_CONTROLLER_LIGHT_ON`.
 *                     Otherwise, it will be `DJI_FLIGHT_CONTROLLER_LIGHT_OFF`.
 *                     The value of `lightStatus` will only be one of the `E_DjiFlightControllerLightStatus` enum values; otherwise, it indicates an abnormal state.
 *  @return T_DjiReturnCode error code
 *  1. If the aircraft model does not support this function, the error code `DJI_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT` will be returned.
 *  2. This API can only get the current on/off status of the light, it cannot get the current mode setting of the light.
 *  3. This API is not available in the current version.
 */
T_DjiReturnCode DjiFlightController_GetBottomLightStatus(E_DjiFlightControllerLightStatus *lightStatus);

/*! @brief This API is used to set the status of the bottom lights.
 *  @note
 *  1. This interface is used to set the on/off status of the bottom lights and is supported only on certain aircraft models.
 *  2. The status of the bottom lights can be set to `DJI_FLIGHT_CONTROLLER_LIGHT_ON` or `DJI_FLIGHT_CONTROLLER_LIGHT_OFF`.
 *  3. This API is not available in the current version.
 * @param lightStatus The light status you wish to set.
 *                     The value of `lightStatus` will only be one of the `E_DjiFlightControllerLightStatus` enum values; otherwise, it indicates an abnormal state.
 *  @return T_DjiReturnCode error code
 *  1. If the aircraft model does not support this function, the error code `DJI_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT` will be returned.
 *  2. Supported values: LIGHT_OFF (always off), LIGHT_ON (always on during flight, off after landing),
 *     LIGHT_AUTO (automatic mode controlled by vision system, bottom light only).
 *  3. This API is not available in the current version.
 */
T_DjiReturnCode DjiFlightController_SetBottomLightStatus(E_DjiFlightControllerLightStatus lightStatus);

/*! @brief This API is used to get the status of the battery lights.
 *  @note
 *  1. This interface is used to obtain the on/off status of the battery lights and is supported only on certain aircraft models.
 *  2. This API is not available in the current version.
 *  @param lightStatus Pointer to the light status.
 *                     When the battery lights are on, the value of `lightStatus` will be `DJI_FLIGHT_CONTROLLER_LIGHT_ON`.
 *                     Otherwise, it will be `DJI_FLIGHT_CONTROLLER_LIGHT_OFF`.
 *                     The value of `lightStatus` will only be one of the `E_DjiFlightControllerLightStatus` enum values; otherwise, it indicates an abnormal state.
 *  @return T_DjiReturnCode error code
 *  1. If the aircraft model does not support this function, the error code `DJI_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT` will be returned.
 *  2. This API is not available in the current version.
 */
T_DjiReturnCode DjiFlightController_GetBatteryLightStatus(E_DjiFlightControllerLightStatus *lightStatus);

/*! @brief This API is used to set the status of the battery lights.
 *  @note
 *  1. This interface is used to set the on/off status of the battery lights and is supported only on certain aircraft models.
 *  2. The status of the battery lights can be set to `DJI_FLIGHT_CONTROLLER_LIGHT_ON` or `DJI_FLIGHT_CONTROLLER_LIGHT_OFF`.
 *  3. Approximately 3 seconds after setting, you can call `DjiFlightController_GetBatteryLightStatus` to query the status.
 *     If you query the status immediately after setting, you may still get the old status.
 *  4. This API is not available in the current version.
 * @param lightStatus The light status you wish to set.
 *                     The value of `lightStatus` will only be one of the `E_DjiFlightControllerLightStatus` enum values; otherwise, it indicates an abnormal state.
 *  @return T_DjiReturnCode error code
 *  1. If the aircraft model does not support this function, the error code `DJI_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT` will be returned.
 *  2. After the aircraft restarts, the battery light will retain the state they were in before the aircraft was powered off.
 *     This API is no longer maintained. Whether the status is retained after power loss varies by firmware version and aircraft model,
 *     the actual behavior should be verified through testing on the specific aircraft model.
 *  3. This API is not available in the current version.
 */
T_DjiReturnCode DjiFlightController_SetBatteryLightStatus(E_DjiFlightControllerLightStatus lightStatus);

/**
 * @brief Query the exit reason of the most recent spotlight-zoom mission.
 * @details This function sends a synchronous query command to the flight controller to
 *          retrieve the reason why the last spotlight-zoom (POI surround) mission was exited.
 *          Upon success, the exit reason is stored in `*exitReason` and the corresponding
 *          human-readable description is printed via DJI_ERROR_PRINT_MSG using the SPZ error
 *          code system defined in dji_error.h.
 * @note
 *  - This function can be called after a spotlight-zoom mission has ended to diagnose
 *    the cause of exit (normal exit, error, or external trigger).
 *  - The exit reason is only meaningful after a mission has started and stopped. Calling
 *    this function before any mission has run may return an undefined value.
 *  - This API is not available in the current version.
 * @param exitReason Pointer to a uint8_t that receives the exit reason code on success.
 *                   The value corresponds to E_DjiSpotlightZoomStatusMessage defined in
 *                   dji_fc_define.h. Must not be NULL.
 * @return T_DjiReturnCode
 *  - DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS: Query succeeded; `*exitReason` is valid.
 *  - DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER: `exitReason` is NULL.
 *  - DJI_ERROR_SYSTEM_MODULE_CODE_TIMEOUT: No response from flight controller.
 *  - Other error codes: Refer to dji_error.h for details.
 */
T_DjiReturnCode DjiFlightController_QuerySpotlightZoomExitReason(uint8_t *exitReason);

/**
 * @brief Start a spotlight-zoom mission on the specified payload position.
 * @details This function sends a synchronous command to enable the spotlight-zoom
 *          navigation function via the switch_navigation_function.
 *
 * @note
 *  - The current implementation supports single-payload mode only. Multi-payload
 *    support (DJI_FUNC_SWITCH_CAMERA_POS + payloadPosition) is reserved for future
 *    extension via the param_data field of dji_vision_set_switch_navigation_function_req.
 *  - Before calling this function, ensure the aircraft is airborne and GPS signal is
 *    valid; otherwise the flight controller will reject the command (ret_code != 0).
 *  - After a successful start, use DjiInterestPoint_SetSpeed / SetApproachSpeed /
 *    SetApproachAngle / SetZoomTypeParam to configure mission parameters in real time.
 *  - To stop the mission, call DjiFlightController_SpotlightZoomStop_Common.
 *  - This API is not available in the current version.
 *
 * @param payloadPosition The mount position of the payload camera to be used by the
 *                        spotlight-zoom mission. Currently reserved; pass
 *                        DJI_MOUNT_POSITION_PAYLOAD_PORT_NO1 for single-payload scenarios.
 * @return T_DjiReturnCode
 *  - DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS: Mission started successfully.
 *  - DJI_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT: Current license level does not support this interface.
 *  - DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR: Failed to retrieve license level.
 *  - DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN: Flight controller responded with a non-zero
 *    ret_code (e.g. aircraft not in air, GPS invalid, unsupported flight mode).
 *  - DJI_ERROR_SYSTEM_MODULE_CODE_TIMEOUT: No response from flight controller.
 *  - Other error codes: Refer to dji_error.h for details.
 */
T_DjiReturnCode DjiFlightController_SpotlightZoomStart_Common(E_DjiMountPosition payloadPosition);

/**
 * @brief Stop the currently running spotlight-zoom mission.
 * @details This function sends a synchronous command to disable the spotlight-zoom
 *          navigation function via the switch_navigation_function.
 *
 * @note
 *  - If no spotlight-zoom mission is currently running, the flight controller may still
 *    return success (ret_code = 0); the caller does not need to guard against this case.
 *  - After stopping, call DjiFlightController_QuerySpotlightZoomExitReason to retrieve
 *    the exit reason for diagnostics.
 *  - Multi-payload support (DJI_FUNC_SWITCH_CAMERA_POS + payloadPosition) is reserved
 *    for future extension; see TODO comment in implementation.
 *  - This API is not available in the current version.
 *
 * @param payloadPosition The mount position of the payload camera. Currently reserved;
 *                        pass DJI_MOUNT_POSITION_PAYLOAD_PORT_NO1 for single-payload
 *                        scenarios.
 * @return T_DjiReturnCode
 *  - DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS: Stop command acknowledged successfully.
 *  - DJI_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT: Current license level does not support this interface.
 *  - DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR: Failed to retrieve license level.
 *  - DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN: Flight controller responded with a non-zero
 *    ret_code.
 *  - DJI_ERROR_SYSTEM_MODULE_CODE_TIMEOUT: No response from flight controller.
 *  - Other error codes: Refer to dji_error.h for details.
 */
T_DjiReturnCode DjiFlightController_SpotlightZoomStop_Common(E_DjiMountPosition payloadPosition);

/**
 * @brief Select or confirm a tracking target for the spotlight-zoom mission.
 * @details This function sends a synchronous target-selection command to the flight controller,
 *          using the SPOTLIGHT_ZOOM_SELECT_TARGET protocol. Two selection methods are supported,
 *          controlled by trackParam.trackType:
 *
 *          1. DJI_FLIGHT_CONTROLLER_SPZ_TRACK_BY_TARGET_ID:
 *             Select a candidate target by its ID, obtained from the TM MOT push
 *             (Multi-Object Tracking). Only the lower 16 bits of targetId are sent
 *             on the wire.
 *
 *          2. DJI_FLIGHT_CONTROLLER_SPZ_TRACK_BY_BOX:
 *             Select a target by drawing a bounding box over the live view.
 *             Box coordinates use
 *             1/10000 of the frame dimension as the unit:
 *             - cx / cy : box center X / Y (range:1-10000)
 *             - w       : box width (range:1-10000)
 *             - h       : box height (range:1-10000)
 *
 *          The request also sets submode = DJI_SPOTLIGHT_ZOOM_MODE_FREE_LOCK_YAW,
 *          which keeps the aircraft yaw locked to the target direction while the
 *          gimbal tracks the target. A monotonically increasing sequence number (seq)
 *          is sent with each call to allow the flight controller to discard stale
 *          commands.
 *
 * @note
 *  - This function should be called after DjiFlightController_SpotlightZoomStart_Common
 *    has succeeded and the spotlight-zoom state push shows the mission is in
 *    DJI_SPOTLIGHT_ZOOM_MOT or DJI_SPOTLIGHT_ZOOM_WAITING state.
 *    (not yet wrapped; send directly via the protocol if needed).
 *  - trackType values other than the two above will be rejected before sending.
 *  - This API is not available in the current version.
 *
 * @param payloadPosition Mount position of the payload camera. Currently reserved for
 *                        future multi-payload support; pass
 *                        DJI_MOUNT_POSITION_PAYLOAD_PORT_NO1 for single-payload use.
 * @param trackParam      Target selection parameters. Fill either targetId or box
 *                        according to trackType:
 *                        - trackType = DJI_FLIGHT_CONTROLLER_SPZ_TRACK_BY_TARGET_ID:
 *                          set trackParam.targetId (uint32_t, lower 16 bits used).
 *                        - trackType = DJI_FLIGHT_CONTROLLER_SPZ_TRACK_BY_BOX:
 *                          set trackParam.box.{cx, cy, w, h} in 1/10000 frame units.
 * @return T_DjiReturnCode
 *  - DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS: Command sent and acknowledged (ack=0).
 *  - DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER: trackType is not a recognized value.
 *  - DJI_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT: Current license level does not support this interface.
 *  - DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR: Failed to retrieve license level.
 *  - DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN: Flight controller responded with non-zero ack.
 *  - DJI_ERROR_SYSTEM_MODULE_CODE_TIMEOUT: No response from flight controller.
 *  - Other error codes: Refer to dji_error.h for details.
 */
 T_DjiReturnCode DjiFlightController_SpotlightZoomStartTracking_Common(
    E_DjiMountPosition payloadPosition,
    T_DjiFlightControllerSpotlightZoomTrackParam trackParam);

/**
 * @brief Enable or disable the spotlight-zoom auto zoom function.
 * @details In the SPZ mission flow, this function is an optional step that should be
 *          called before setting the target screen occupancy ratio:
 *
 *          [Optional] Call this function to disable auto zoom before setting
 *          target screen occupancy ratio; then set occupancy ratio via
 *          DjiInterestPoint_SetZoomByPercentage / DjiInterestPoint_SetZoomTypeParam
 * @note
 *  - This API is not available in the current version.
 *
 * @param status Auto zoom switch. Use E_DjiFlightControllerSpzAutoZoomStatus:
 *               - DJI_FLIGHT_CONTROLLER_SPZ_AUTO_ZOOM_DISABLE (0): disable auto zoom;
 *                 call this before manually controlling the target screen occupancy ratio.
 *               - DJI_FLIGHT_CONTROLLER_SPZ_AUTO_ZOOM_ENABLE  (1): enable auto zoom (default);
 *                 the flight controller automatically adjusts zoom to maintain target size.
 * @return T_DjiReturnCode
 *  - DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS: Parameter written successfully.
 *  - Other error codes: Refer to dji_error.h for details.
 */
T_DjiReturnCode DjiFlightController_SetSpzAutoZoomStatus(E_DjiFlightControllerSpzAutoZoomStatus status);

/**
 * @brief Set the spotlight-zoom approach mode.
 * @details - DJI_FLIGHT_CONTROLLER_SPZ_APPROACH_MODE (0): Approach mode. The minimum
 *            distance limit to the target is removed, allowing the aircraft to fly
 *            close to the target during spotlight-zoom approach phase.
 *          - DJI_FLIGHT_CONTROLLER_SPZ_NORMAL_MODE   (1): Normal mode (default). The
 *            flight controller enforces a minimum safe distance to the target.
 * @note
 *  - This API is not available in the current version.
 *
 * @param mode Approach mode switch. Use E_DjiFlightControllerSpzTooCloseLimitMode.
 * @return T_DjiReturnCode
 *  - DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS: Parameter written successfully.
 *  - Other error codes: Refer to dji_error.h for details.
 */
T_DjiReturnCode DjiFlightController_SetSpzTooCloseLimitMode(E_DjiFlightControllerSpzTooCloseLimitMode mode);
T_DjiReturnCode DjiFlightController_GetSpzTooCloseLimitMode(E_DjiFlightControllerSpzTooCloseLimitMode *mode);

/**
 * @brief Set the target ascend height for the RC-lost GO_UP failsafe action.
 * @details Takes effect only when rc_lost_action is set to
 *          DJI_FLIGHT_CONTROLLER_RC_LOST_ACTION_GO_UP: the aircraft ascends to this
 *          height (relative to the home point) and then returns home.
 * @note
 *  - This API is not available in the current version.
 * @param heightMeter Target ascend height in meters.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_SetRCLostGoUpHeight(int32_t heightMeter);

/**
 * @brief Get the target ascend height configured for the RC-lost GO_UP failsafe action.
 * @note
 *  - This API is not available in the current version.
 * @param heightMeter Output - current go-up height in meters.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_GetRCLostGoUpHeight(int32_t *heightMeter);

/**
 * @brief Set whether multiple positioning mode is enabled.
 * @details Must be enabled before setting assist positioning mode to PSDK (ADN mode).
 *          Set to DJI_FLIGHT_CONTROLLER_MULTIPLE_MODE_DISABLE to exit multiple positioning mode.
 * @note
 *  - This API is not available in the current version.
 * @param mode See E_DjiFlightControllerMultipleMode.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_SetMultipleMode(E_DjiFlightControllerMultipleMode mode);

/**
 * @brief Get whether multiple positioning mode is enabled.
 * @note
 *  - This API is not available in the current version.
 * @param mode Output - current multiple positioning mode status.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_GetMultipleMode(E_DjiFlightControllerMultipleMode *mode);

/**
 * @brief Assist positioning mode for no-GPS flight.
 */
typedef enum {
    DJI_FLIGHT_CONTROLLER_ASSIST_POSITIONING_MODE_OFF     = 0, /*!< Assist positioning disabled. */
    DJI_FLIGHT_CONTROLLER_ASSIST_POSITIONING_MODE_HEADING = 1, /*!< Heading reference mode. */
    DJI_FLIGHT_CONTROLLER_ASSIST_POSITIONING_MODE_ANCHOR  = 2, /*!< Anchor point reference mode. */
    DJI_FLIGHT_CONTROLLER_ASSIST_POSITIONING_MODE_MSDK    = 3, /*!< MSDK external position reference mode. */
    DJI_FLIGHT_CONTROLLER_ASSIST_POSITIONING_MODE_PSDK    = 4, /*!< PSDK external position reference mode. */
} E_DjiFlightControllerAssistPositioningMode;

/**
 * @brief Set the assist positioning mode for no-GPS flight.
 * @details Controls how the aircraft obtains position reference when GPS is unavailable.
 *          To enter ADN mode, first call DjiFlightController_SetMultipleMode with
 *          DJI_FLIGHT_CONTROLLER_MULTIPLE_MODE_ENABLE, then call this interface with
 *          DJI_FLIGHT_CONTROLLER_ASSIST_POSITIONING_MODE_PSDK.
 * @note
 *  - This API is not available in the current version.
 * @param mode Assist positioning mode. See E_DjiFlightControllerAssistPositioningMode.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_SetAssistPositioningMode(
    E_DjiFlightControllerAssistPositioningMode mode);

/**
 * @brief Get the current assist positioning mode.
 * @note
 *  - This API is not available in the current version.
 * @param mode Output - current assist positioning mode.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_GetAssistPositioningMode(
    E_DjiFlightControllerAssistPositioningMode *mode);

/**
 * @brief Enable or disable ADN mode (PSDK external position reference mode).
 * @details When enabled, the aircraft uses position data provided by the PSDK payload
 *          as the external position reference for no-GPS flight.
 *          When disabled, the aircraft exits ADN mode and restores default positioning behavior.
 * @note
 *  - This API is not available in the current version.
 * @param enable true to enter ADN mode, false to exit.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_SetADNModeEnable(bool enable);

/**
 * @brief Enable or disable extreme battery endurance mode.
 * @details When enabled, the aircraft disables smart battery go-home and smart battery
 *          landing protection to maximize flight endurance.
 *          When disabled, both protections are restored to their default enabled state.
 * @note Takes effect in flight; writing on ground may be rejected by the flight controller.
 *  - This API is not available in the current version.
 * @param enabled true to enable extreme battery endurance mode, false to disable.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_SetExtremeBatteryEnduranceModeEnabled(bool enabled);

/**
 * @brief Get the current state of extreme battery endurance mode.
 * @note
 *  - This API is not available in the current version.
 * @param enabled Output - true if extreme battery endurance mode is enabled, false otherwise.
 * @return Execution result.
 */
T_DjiReturnCode DjiFlightController_GetExtremeBatteryEnduranceModeEnabled(bool *enabled);

#ifdef __cplusplus
}
#endif

#endif // DJI_FLIGHT_CONTROLLER_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
