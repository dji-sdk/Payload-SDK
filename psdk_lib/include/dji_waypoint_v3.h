/**
 ********************************************************************
 * @file    dji_waypoint_v3.h
 * @brief   This is the header file for "dji_waypoint_v3.c", defining the structure and
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
#ifndef DJI_WAYPOINT_V3_H
#define DJI_WAYPOINT_V3_H

/* Includes ------------------------------------------------------------------*/
#include "dji_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/**
* @brief Waypoint v3 mission action.
*/
typedef enum {
    DJI_WAYPOINT_V3_ACTION_START = 0, /*!< Waypoint v3 mission start action. */
    DJI_WAYPOINT_V3_ACTION_STOP = 1, /*!< Waypoint v3 mission stop action. */
    DJI_WAYPOINT_V3_ACTION_PAUSE = 2, /*!< Waypoint v3 mission pause action. */
    DJI_WAYPOINT_V3_ACTION_RESUME = 3, /*!< Waypoint v3 mission resume action. */
} E_DjiWaypointV3Action;

/**
*  Waypoint v3 current mission state.
*/
typedef enum {
    DJI_WAYPOINT_V3_MISSION_STATE_IDLE = 0, /*!< Waypoint v3 mission in idle state. */
    DJI_WAYPOINT_V3_MISSION_STATE_PREPARE = 16, /*!< Waypoint v3 mission in prepare state. */
    DJI_WAYPOINT_V3_MISSION_STATE_TRANS_MISSION = 32, /*!< Waypoint v3 mission in trans mission state. */
    DJI_WAYPOINT_V3_MISSION_STATE_MISSION = 48, /*!< Waypoint v3 mission in mission state. */
    DJI_WAYPOINT_V3_MISSION_STATE_BREAK = 64, /*!< Waypoint v3 mission in break state. */
    DJI_WAYPOINT_V3_MISSION_STATE_RESUME = 80, /*!< Waypoint v3 mission in resume state. */
    DJI_WAYPOINT_V3_MISSION_STATE_RETURN_FIRSTPOINT = 98, /*!< Waypoint v3 mission in return first point state. */
} E_DjiWaypointV3MissionState;

/**
*  Waypoint v3 current action state.
*/
typedef enum {
    DJI_WAYPOINT_V3_ACTION_STATE_IDLE = 0, /*!< Waypoint v3 action in idle state. */
    DJI_WAYPOINT_V3_ACTION_STATE_RUNNING = 1, /*!< Waypoint v3 action in idle state. */
    DJI_WAYPOINT_V3_ACTION_STATE_FINISHED = 5, /*!< Waypoint v3 action in idle state. */
} E_DjiWaypointV3ActionState;

/**
 * @brief Waypoint v3 mission exit status category.
 * @details Categorizes how the most recent waypoint v3 mission ended (or whether it is still
 *          running). Used together with #E_DjiWaypointV3ExitReason which further describes
 *          the exit cause.
 */
typedef enum {
    DJI_WAYPOINT_V3_EXIT_STATUS_NORMAL_FINISH = 0,    /*!< Mission finished normally (no breakpoint). */
    DJI_WAYPOINT_V3_EXIT_STATUS_ABNORMAL_EXIT = 1,    /*!< Mission exited abnormally with breakpoint. */
    DJI_WAYPOINT_V3_EXIT_STATUS_USER_EXIT = 2,        /*!< Mission exited by user with breakpoint. */
    DJI_WAYPOINT_V3_EXIT_STATUS_RUNNING = 3,          /*!< Mission is currently running. */
    DJI_WAYPOINT_V3_EXIT_STATUS_NO_MISSION = 4,       /*!< No previous mission record. */
} E_DjiWaypointV3ExitStatus;

/**
 * @brief Waypoint v3 mission exit reason.
 * @details The detailed reason for a waypoint v3 mission exit. When #E_DjiWaypointV3ExitStatus
 *          is #DJI_WAYPOINT_V3_EXIT_STATUS_NORMAL_FINISH or
 *          #DJI_WAYPOINT_V3_EXIT_STATUS_NO_MISSION, this field is
 *          #DJI_WAYPOINT_V3_EXIT_REASON_NO_ERROR / not meaningful.
 */
typedef enum {
    DJI_WAYPOINT_V3_EXIT_REASON_NO_ERROR = 0,                                              /*!< No error. */
    DJI_WAYPOINT_V3_EXIT_REASON_MISSION_ID_NOT_EXIST = 1,                                  /*!< Mission ID does not exist. */
    DJI_WAYPOINT_V3_EXIT_REASON_WAYLINE_INFO_ERROR = 2,                                    /*!< Wayline info error. */
    DJI_WAYPOINT_V3_EXIT_REASON_WPMZ_FILE_VERSION_NOT_MATCH = 3,                           /*!< WPMZ file version mismatch. */
    DJI_WAYPOINT_V3_EXIT_REASON_WPMZ_FILE_LOAD_ERROR = 4,                                  /*!< WPMZ file parse / load error. */
    DJI_WAYPOINT_V3_EXIT_REASON_NO_BREAK_INFO = 5,                                         /*!< Missing breakpoint info. */
    DJI_WAYPOINT_V3_EXIT_REASON_CMD_INVALID = 6,                                           /*!< Invalid command. */
    DJI_WAYPOINT_V3_EXIT_REASON_CANNOT_START_WHEN_RUNNING = 257,                           /*!< Wayline already started, cannot start again. */
    DJI_WAYPOINT_V3_EXIT_REASON_CANNOT_BREAK_IN_CUR_STATE = 258,                           /*!< Cannot break wayline in current state. */
    DJI_WAYPOINT_V3_EXIT_REASON_CANNOT_STOP_WHEN_NOT_RUNNING = 259,                        /*!< Wayline not started, cannot stop. */
    DJI_WAYPOINT_V3_EXIT_REASON_CANNOT_BREAK_WHEN_NOT_RUNNING = 260,                       /*!< Wayline not started, cannot break. */
    DJI_WAYPOINT_V3_EXIT_REASON_CANNOT_REQUEST_DRONE_CONTROL = 261,                        /*!< Flight task conflict, cannot acquire drone control. */
    DJI_WAYPOINT_V3_EXIT_REASON_CANNOT_RESUME_IN_CUR_STATE = 262,                          /*!< Cannot resume wayline in current state. */
    DJI_WAYPOINT_V3_EXIT_REASON_CANNOT_TAKEOFF_ON_SHIP = 263,                              /*!< Cannot take off on ship. */
    DJI_WAYPOINT_V3_EXIT_REASON_HEIGHT_LIMIT = 513,                                        /*!< Reached height limit. */
    DJI_WAYPOINT_V3_EXIT_REASON_RADIUS_LIMIT = 514,                                        /*!< Reached radius (distance) limit. */
    DJI_WAYPOINT_V3_EXIT_REASON_CROSS_FLYLIMIT_AREA = 515,                                 /*!< Crossed fly-limit area. */
    DJI_WAYPOINT_V3_EXIT_REASON_LOW_LIMIT = 516,                                           /*!< Reached low-altitude limit. */
    DJI_WAYPOINT_V3_EXIT_REASON_OBSTACLE_STOP = 517,                                       /*!< Stopped by obstacle avoidance. */
    DJI_WAYPOINT_V3_EXIT_REASON_RTK_DISCONNECT = 518,                                      /*!< Poor RTK signal. */
    DJI_WAYPOINT_V3_EXIT_REASON_BOUNDARY_LIMIT = 519,                                      /*!< Approaching fly-limit area boundary. */
    DJI_WAYPOINT_V3_EXIT_REASON_RC_PITCH_ROLL_BREAK = 520,                                 /*!< RC pitch/roll stick break (agriculture wayline only). */
    DJI_WAYPOINT_V3_EXIT_REASON_AIRPORT_HEIGHT_LIMIT = 521,                                /*!< Exceeded airport fly-limit height. */
    DJI_WAYPOINT_V3_EXIT_REASON_REQUEST_TAKEOFF_FAIL = 522,                                /*!< Request auto-takeoff failed. */
    DJI_WAYPOINT_V3_EXIT_REASON_AUTOTAKEOFF_RUN_FAIL = 523,                                /*!< Auto-takeoff task run failed. */
    DJI_WAYPOINT_V3_EXIT_REASON_REQUEST_WAYLINE_FAIL = 524,                                /*!< Request wayline mission failed. */
    DJI_WAYPOINT_V3_EXIT_REASON_AGRO_PLAN_FAIL = 525,                                      /*!< Agriculture wayline: terrain-follow detour failed, mission paused. */
    DJI_WAYPOINT_V3_EXIT_REASON_REQUEST_QUICK_TAKEOFF_ASSIST_FAIL = 526,                   /*!< Request wayline quick-takeoff assist task failed. */
    DJI_WAYPOINT_V3_EXIT_REASON_QUICK_TAKEOFF_ASSIST_RUN_FAIL = 527,                       /*!< Wayline quick-takeoff assist task run failed. */
    DJI_WAYPOINT_V3_EXIT_REASON_VFENCE_LIMIT = 528,                                        /*!< Approaching user-defined virtual fence. */
    DJI_WAYPOINT_V3_EXIT_REASON_TARGET_NOT_REACHABLE = 529,                                /*!< Target point unreachable (no-fly zone or obstacle). */
    DJI_WAYPOINT_V3_EXIT_REASON_TRAJ_PLAN_FAILED = 530,                                    /*!< Trajectory library trajectory planning failed. */
    DJI_WAYPOINT_V3_EXIT_REASON_DEPARTURE_WAYLINE_FAIL = 531,                              /*!< Departure wayline execution failed. */
    DJI_WAYPOINT_V3_EXIT_REASON_DEPARTURE_WAYLINE_INIT_FAIL = 532,                         /*!< Departure wayline initialization failed. */
    DJI_WAYPOINT_V3_EXIT_REASON_DEPARTURE_WAYLINE_INVALID = 533,                           /*!< Departure wayline invalid. */
    DJI_WAYPOINT_V3_EXIT_REASON_GPS_INVALID = 769,                                         /*!< GPS signal weak. */
    DJI_WAYPOINT_V3_EXIT_REASON_CANNOT_START_AT_CURRENT_RC_MODE = 770,                     /*!< Cannot start at current RC mode (e.g. S mode). */
    DJI_WAYPOINT_V3_EXIT_REASON_HOME_POINT_NOT_RECORDED = 771,                             /*!< Home point not recorded. */
    DJI_WAYPOINT_V3_EXIT_REASON_LOW_BATTERY = 772,                                         /*!< Battery too low to start mission. */
    DJI_WAYPOINT_V3_EXIT_REASON_RETURN_HOME = 773,                                         /*!< Low-battery return-to-home. */
    DJI_WAYPOINT_V3_EXIT_REASON_ADSB_ERROR = 774,                                          /*!< ADSB error. */
    DJI_WAYPOINT_V3_EXIT_REASON_RC_LOST = 775,                                             /*!< RC signal lost. */
    DJI_WAYPOINT_V3_EXIT_REASON_RTK_NOT_READY = 776,                                       /*!< RTK not ready. */
    DJI_WAYPOINT_V3_EXIT_REASON_DRONE_IS_MOVING = 777,                                     /*!< Drone is moving (not hovering), cannot start mission. */
    DJI_WAYPOINT_V3_EXIT_REASON_DRONE_ON_GROUND_MOTOR_ON = 778,                            /*!< Drone on ground with motor idling, wayline not allowed (user not ready). */
    DJI_WAYPOINT_V3_EXIT_REASON_SURFACE_FOLLOW_CAMERA_INVALID = 779,                       /*!< Real-time terrain-follow camera state invalid (too bright/dark/uneven). */
    DJI_WAYPOINT_V3_EXIT_REASON_SURFACE_FOLLOW_HEIGHT_INVALID = 780,                       /*!< Real-time terrain-follow height invalid (>200m or <30m). */
    DJI_WAYPOINT_V3_EXIT_REASON_SURFACE_FOLLOW_MAP_WRONG = 781,                            /*!< Real-time terrain-follow global map computation error. */
    DJI_WAYPOINT_V3_EXIT_REASON_HOMEPOINT_NOT_MATCH_RTK = 782,                             /*!< Home point does not match RTK switch state. */
    DJI_WAYPOINT_V3_EXIT_REASON_TRANSPORT_DIVERT = 783,                                    /*!< Transport drone divert landing. */
    DJI_WAYPOINT_V3_EXIT_REASON_STRONG_WIND_GOHOME = 784,                                  /*!< Strong wind return-to-home. */
    DJI_WAYPOINT_V3_EXIT_REASON_DOCK_MOVE_GOHOME = 786,                                    /*!< Dock moving return-to-home. */
    DJI_WAYPOINT_V3_EXIT_REASON_HEADING_LEVEL_POOR = 787,                                  /*!< Heading level poor, cannot reach waypoint. */
    DJI_WAYPOINT_V3_EXIT_REASON_JAMMING_GOHOME = 788,                                      /*!< Signal jamming/spoofing return-to-home. */
    DJI_WAYPOINT_V3_EXIT_REASON_WIND_SPD = 789,                                            /*!< Wind speed too high, exit wayline. */
    DJI_WAYPOINT_V3_EXIT_REASON_BUSY_IMU_CHANGED = 790,                                    /*!< Main IMU switched. */
    DJI_WAYPOINT_V3_EXIT_REASON_RAIN_DETECT_VALID = 791,                                   /*!< Rain detected. */
    DJI_WAYPOINT_V3_EXIT_REASON_ARM1_ERROR = 800,                                          /*!< Arm 1 not in place. */
    DJI_WAYPOINT_V3_EXIT_REASON_ARM2_ERROR = 801,                                          /*!< Arm 2 not in place. */
    DJI_WAYPOINT_V3_EXIT_REASON_ARM3_ERROR = 802,                                          /*!< Arm 3 not in place. */
    DJI_WAYPOINT_V3_EXIT_REASON_ARM4_ERROR = 803,                                          /*!< Arm 4 not in place. */
    DJI_WAYPOINT_V3_EXIT_REASON_RADAR_PEDEX_WARNING = 804,                                 /*!< Radar obstacle detection abnormal. */
    DJI_WAYPOINT_V3_EXIT_REASON_OVERLOAD = 805,                                            /*!< Drone overloaded. */
    DJI_WAYPOINT_V3_EXIT_REASON_HOIST_NOT_READY = 806,                                     /*!< Transport hoist hook not retracted. */
    DJI_WAYPOINT_V3_EXIT_REASON_HOIST_SELF_CHECK_ERR = 807,                                /*!< Transport hoist cable self-check error (e.g. cable reverse-wound). */
    DJI_WAYPOINT_V3_EXIT_REASON_PREVENT_MOTOR_START = 808,                                 /*!< Other reason prevented motor start, wayline failed. */
    DJI_WAYPOINT_V3_EXIT_REASON_EST_BAT_OVERTEMP_ERROR = 816,                              /*!< Estimated battery will overheat during wayline flight. */
    DJI_WAYPOINT_V3_EXIT_REASON_DRONE_CRITICAL_ERROR = 1023,                               /*!< Drone critical error, cannot execute (HMS level 3 fault). */
    DJI_WAYPOINT_V3_EXIT_REASON_CANNOT_FIND_PAYLOAD = 1025,                                /*!< Cannot find target payload (multi-payload only). */
    DJI_WAYPOINT_V3_EXIT_REASON_ACTION_EXECUTION_FAILED = 1026,                            /*!< Action execution failed. */
    DJI_WAYPOINT_V3_EXIT_REASON_FARM_NO_PESTICIDE = 1027,                                  /*!< Agriculture (field/orchard) wayline: spray tank empty / spreader hopper empty. */
    DJI_WAYPOINT_V3_EXIT_REASON_RADAR_DISCONNECT = 1028,                                   /*!< Agriculture wayline: obstacle avoidance enabled but radar disconnected. */
    DJI_WAYPOINT_V3_EXIT_REASON_PAYLOAD_NOT_HOIST = 1040,                                  /*!< Payload type is not hoist. */
    DJI_WAYPOINT_V3_EXIT_REASON_USER_EXIT = 1281,                                          /*!< User exit. */
    DJI_WAYPOINT_V3_EXIT_REASON_USER_BREAK = 1282,                                         /*!< User break. */
    DJI_WAYPOINT_V3_EXIT_REASON_USER_SET_GOHOME = 1283,                                    /*!< User triggered return-to-home. */
    DJI_WAYPOINT_V3_EXIT_REASON_USER_AGRO_PLANNER_STATE_CHANGE = 1284,                     /*!< Agriculture wayline: user toggled terrain-follow detour switch during flight. */
    DJI_WAYPOINT_V3_EXIT_REASON_USER_SWITCH_RC_MODE = 1285,                                /*!< User switched RC mode. */
    DJI_WAYPOINT_V3_EXIT_REASON_TRAJ_INIT_FAIL = 1536,                                     /*!< Wayline platform initialization failed. */
    DJI_WAYPOINT_V3_EXIT_REASON_TRAJ_JOB_EXIT_BUT_MIS_RUNNING = 1537,                      /*!< Job exited but Mission abnormally still running. */
    DJI_WAYPOINT_V3_EXIT_REASON_TRAJ_ON_GROUND_MOTOR_ON_CANNOT_GO = 1538,                  /*!< Drone propellers already spinning, direct takeoff not allowed (must stop propellers first). */
    DJI_WAYPOINT_V3_EXIT_REASON_TRAJ_INVALID_START_INDEX_OR_PROG = 1539,                   /*!< Start info (waypoint index or progress) error. */
    DJI_WAYPOINT_V3_EXIT_REASON_TRAJ_INVALID_CSYS_MODE = 1540,                             /*!< Unsupported coordinate system. */
    DJI_WAYPOINT_V3_EXIT_REASON_TRAJ_INVALID_HEIGHT_MODE = 1541,                           /*!< Unsupported height mode. */
    DJI_WAYPOINT_V3_EXIT_REASON_TRAJ_INVALID_FLY_WP_MODE = 1542,                           /*!< Unsupported transition wayline mode. */
    DJI_WAYPOINT_V3_EXIT_REASON_TRAJ_INVALID_YAW_MODE = 1543,                              /*!< Unsupported yaw mode. */
    DJI_WAYPOINT_V3_EXIT_REASON_TRAJ_INVALID_TURN_DIR_MODE = 1544,                         /*!< Unsupported yaw turn direction mode. */
    DJI_WAYPOINT_V3_EXIT_REASON_TRAJ_INVALID_WP_TYPE = 1545,                               /*!< Unsupported waypoint type. */
    DJI_WAYPOINT_V3_EXIT_REASON_TRAJ_FIR_LAS_WP_TYPE_ERROR = 1546,                         /*!< First/last waypoint cannot use coordinated turn type. */
    DJI_WAYPOINT_V3_EXIT_REASON_TRAJ_GLOB_VEL_OUT_OF_RANGE = 1547,                         /*!< Global wayline velocity out of reasonable range. */
    DJI_WAYPOINT_V3_EXIT_REASON_TRAJ_WP_NUM_OUT_OF_RANGE = 1548,                           /*!< Waypoint count abnormal. */
    DJI_WAYPOINT_V3_EXIT_REASON_TRAJ_LAT_LONG_OUT_OF_RANGE = 1549,                         /*!< Latitude/longitude data abnormal. */
    DJI_WAYPOINT_V3_EXIT_REASON_TRAJ_DAMP_DIS_OUT_OF_RANGE = 1550,                         /*!< Damping (turn intercept) distance abnormal. */
    DJI_WAYPOINT_V3_EXIT_REASON_TRAJ_MAX_VEL_OUT_OF_RANGE = 1551,                          /*!< Segment max velocity out of reasonable range. */
    DJI_WAYPOINT_V3_EXIT_REASON_TRAJ_VEL_OUT_OF_RANGE = 1552,                              /*!< Segment target velocity out of reasonable range. */
    DJI_WAYPOINT_V3_EXIT_REASON_TRAJ_WP_YAW_OUT_OF_RANGE = 1553,                           /*!< Waypoint yaw angle out of reasonable range. */
    DJI_WAYPOINT_V3_EXIT_REASON_TRAJ_INVALID_YAW_MODE_IN_VERT_SEGM = 1554,                 /*!< Yaw mode invalid in vertical segment. */
    DJI_WAYPOINT_V3_EXIT_REASON_TRAJ_BREAK_INFO_MISSION_ID_CHANGED = 1555,                 /*!< Resume break_info: mission_id input wrong. */
    DJI_WAYPOINT_V3_EXIT_REASON_TRAJ_BREAK_INFO_PROGRESS_OUT_OF_RANGE = 1556,              /*!< Resume break_info: progress input wrong. */
    DJI_WAYPOINT_V3_EXIT_REASON_TRAJ_BREAK_INFO_INVALID_MISSION_STATE = 1557,              /*!< Resume break_info: mission state invalid. */
    DJI_WAYPOINT_V3_EXIT_REASON_TRAJ_BREAK_INFO_WP_INDEX_OUT_OF_RANGE = 1558,              /*!< Resume break_info: waypoint index input wrong. */
    DJI_WAYPOINT_V3_EXIT_REASON_TRAJ_BREAK_LAT_LONG_OUT_OF_RANGE = 1559,                   /*!< Resume break_info: lat/long input wrong. */
    DJI_WAYPOINT_V3_EXIT_REASON_TRAJ_BREAK_INFO_WP_YAW_OUT_OF_RANGE = 1560,                /*!< Resume break_info: waypoint yaw input wrong. */
    DJI_WAYPOINT_V3_EXIT_REASON_TRAJ_INVALID_BREAK_INFO_FLAG = 1561,                       /*!< Resume break_info: flag setting wrong. */
    DJI_WAYPOINT_V3_EXIT_REASON_TRAJ_GET_TRAJ_INFO_FAILED = 1562,                          /*!< Read trajectory info failed. */
    DJI_WAYPOINT_V3_EXIT_REASON_TRAJ_GENERATE_FAIL = 1563,                                 /*!< Trajectory generation failed. */
    DJI_WAYPOINT_V3_EXIT_REASON_TRAJ_LIB_RUN_FAIL = 1564,                                  /*!< Trajectory library run failed. */
    DJI_WAYPOINT_V3_EXIT_REASON_TRAJ_LIB_EMERGENCY_BRAKE = 1565,                           /*!< Wayline obstacle-avoidance emergency brake. */
    DJI_WAYPOINT_V3_EXIT_REASON_ACTION_NOT_FOUND = 1588,                                   /*!< Action not found. */
    DJI_WAYPOINT_V3_EXIT_REASON_ACTION_INDEX_REPEATED = 1591,                              /*!< Action INDEX must not be repeated. */
    DJI_WAYPOINT_V3_EXIT_REASON_ACTION_INFO_SIZE_INVALID = 1592,                           /*!< Action count abnormal (too many or too few). */
    DJI_WAYPOINT_V3_EXIT_REASON_ACTION_TREE_EMPTY = 1593,                                  /*!< Action tree must not be empty. */
    DJI_WAYPOINT_V3_EXIT_REASON_ACTION_TREE_LAYER_EMPTY = 1594,                            /*!< Action tree layer must not be empty. */
    DJI_WAYPOINT_V3_EXIT_REASON_ACTION_ID_REPEATED = 1595,                                 /*!< Action ID must not be repeated. */
    DJI_WAYPOINT_V3_EXIT_REASON_ACTION_NODE_CHILDREN_NUM_LT_2 = 1596,                      /*!< Action node child count must not be less than 2. */
    DJI_WAYPOINT_V3_EXIT_REASON_ACTION_INDEX_OUT_OF_RANGE = 1597,                          /*!< Action INDEX out of reasonable range. */
    DJI_WAYPOINT_V3_EXIT_REASON_ACTION_ID_IS_65535 = 1598,                                 /*!< Action ID value must not be 65535. */
    DJI_WAYPOINT_V3_EXIT_REASON_ACTION_TREE_CHILD_NEXT_LAYER_MISMATCH = 1599,              /*!< Action tree generation error (child count != next-layer node count). */
    DJI_WAYPOINT_V3_EXIT_REASON_ACTION_TREE_LAYER_NUM_TOO_MANY = 1600,                     /*!< Action tree has too many layers. */
    DJI_WAYPOINT_V3_EXIT_REASON_ACTION_TREE_LAYER_NUM_TOO_FEW = 1601,                      /*!< Action tree has too few layers. */
    DJI_WAYPOINT_V3_EXIT_REASON_ACTION_GROUP_NUM_OUT_OF_RANGE = 1602,                      /*!< Action group count out of reasonable range. */
    DJI_WAYPOINT_V3_EXIT_REASON_ACTION_GROUP_VALID_RANGE_ERROR = 1603,                     /*!< Action tree valid range error. */
    DJI_WAYPOINT_V3_EXIT_REASON_ACTION_TREE_ROOT_STATUS_INVALID = 1604,                    /*!< Action tree root node status invalid. */
    DJI_WAYPOINT_V3_EXIT_REASON_ACTION_TREE_NODE_STATUS_INVALID = 1605,                    /*!< Action tree normal node status invalid. */
    DJI_WAYPOINT_V3_EXIT_REASON_BREAK_INFO_ACTION_GROUP_ID_OUT_OF_RANGE = 1606,            /*!< Resume: action index out of reasonable range in break_info. */
    DJI_WAYPOINT_V3_EXIT_REASON_ACTION_STATUS_TREE_SIZE_ERROR = 1607,                      /*!< Action tree count error. */
    DJI_WAYPOINT_V3_EXIT_REASON_BREAK_INFO_TRIGGER_RUN_RESULT_INVALID = 1608,              /*!< Trigger run result invalid in break_info. */
    DJI_WAYPOINT_V3_EXIT_REASON_BREAK_INFO_ACTION_GROUP_ID_REPEATED = 1609,                /*!< Resume: action group ID in break_info must not be repeated. */
    DJI_WAYPOINT_V3_EXIT_REASON_BREAK_INFO_ACTION_LOCATION_REPEATED = 1610,                /*!< Resume: action group location in break_info must not be repeated. */
    DJI_WAYPOINT_V3_EXIT_REASON_BREAK_INFO_ACTION_LOCATION_OUT_OF_RANGE = 1611,            /*!< Resume: action group location in break_info out of reasonable range. */
    DJI_WAYPOINT_V3_EXIT_REASON_RESUME_ID_NOT_IN_BREAK_INFO = 1612,                        /*!< Resume: action id not present in break_info. */
    DJI_WAYPOINT_V3_EXIT_REASON_RESUME_INFO_MODIFY_NO_INTERRUPT_TO_INTERRUPT = 1613,       /*!< Resume: cannot modify action status from no-interrupt to interrupt. */
    DJI_WAYPOINT_V3_EXIT_REASON_ACTION_RESUME_FAIL_FOR_INVALID_RESUME_INFO = 1614,         /*!< Action resume failed due to invalid break_info. */
    DJI_WAYPOINT_V3_EXIT_REASON_ACTUATOR_NOT_FOUND = 1634,                                 /*!< Action actuator not found. */
    DJI_WAYPOINT_V3_EXIT_REASON_TRIGGER_NOT_FOUND = 1649,                                  /*!< Trigger not found. */
    DJI_WAYPOINT_V3_EXIT_REASON_TRIGGER_SINGLE_TIME_CHECK_FAIL = 1650,                     /*!< Single-time-arrival trigger parameter setting error. */
    DJI_WAYPOINT_V3_EXIT_REASON_INNER_ERROR = 65534,                                       /*!< Internal error. */
    DJI_WAYPOINT_V3_EXIT_REASON_UNKNOWN_ERROR = 65535,                                     /*!< Unknown internal error. */
} E_DjiWaypointV3ExitReason;

/**
 * @brief Waypoint v3 mission exit result.
 * @details Returned by #DjiWaypointV3_GetExitReason.
 *          - When #status == #DJI_WAYPOINT_V3_EXIT_STATUS_RUNNING, #reason carries the
 *            current running state (cast to/compare with #E_DjiWaypointV3MissionState
 *            values).
 *          - When #status == #DJI_WAYPOINT_V3_EXIT_STATUS_NORMAL_FINISH or
 *            #DJI_WAYPOINT_V3_EXIT_STATUS_NO_MISSION, #reason is
 *            #DJI_WAYPOINT_V3_EXIT_REASON_NO_ERROR / not meaningful.
 *          - Otherwise, #reason carries the detailed exit reason.
 */
typedef struct {
    E_DjiWaypointV3ExitStatus status; /*!< Exit status category. */
    E_DjiWaypointV3ExitReason reason; /*!< Detailed exit reason. */
} T_DjiWaypointV3ExitResult;

/**
*  Waypoint v3 mission state.
*/
typedef struct {
    E_DjiWaypointV3MissionState state; /*!< Waypoint v3 current mission state, #E_DjiWaypointV3MissionState. */
    uint32_t wayLineId; /*!< Waypoint v3 current way line id. */
    uint16_t currentWaypointIndex; /*!< Waypoint v3 current waypoint index. */
} T_DjiWaypointV3MissionState;

/**
*  Waypoint v3 action state.
*/
typedef struct {
    E_DjiWaypointV3ActionState state; /*!< Waypoint v3 current action state, #E_DjiWaypointV3ActionState. */
    uint32_t wayLineId; /*!< Waypoint v3 current way line id. */
    uint16_t currentWaypointIndex; /*!< Waypoint v3 current waypoint index. */
    uint16_t actionGroupId; /*!< Waypoint v3 current action group index. */
    uint16_t actionId; /*!< Waypoint v3 current action index. */
} T_DjiWaypointV3ActionState;

/**
 * @brief External localization data for ADN mode positioning.
 * @note  The timestamp should be aligned with the 6-DOF data timestamp received by the payload.
 */
typedef struct {
    uint32_t version;       /*!< Protocol version. */
    double   longitude;     /*!< Longitude in radians. */
    double   latitude;      /*!< Latitude in radians. */
    float    altitude;      /*!< Ellipsoidal height in meters. */
    float    heading;       /*!< Heading in degrees. NED frame: north=0°, east=90°, south=180°. */
    float    horizError;    /*!< Horizontal position accuracy in meters. */
    float    vertError;     /*!< Vertical position accuracy in meters. */
    float    headingError;  /*!< Heading accuracy in degrees. */
    uint32_t timestamp;     /*!< Timestamp in milliseconds, aligned with the 6-DOF data timestamp. */
} T_DjiWaypointV3ExternalLocalizationInfo;

/**
 * @brief ADN assist positioning mode status pushed by the flight controller.
 */
typedef struct {
    uint32_t version;       /*!< Protocol version. */
    float    gpsError;      /*!< Deviation between GPS and positioning estimate, in meters. */
    float    sdkError;      /*!< Deviation between SDK and positioning estimate, in meters. */
    float    gpsSdkError;   /*!< Deviation between SDK and GPS, in meters. */
    float    divergeError;  /*!< Divergence reference error, in meters. */
    uint32_t isGnssValid;   /*!< GPS availability: 1 = available, 0 = unavailable. */
} T_DjiWaypointV3AssistPosModeInfo;

/**
 * @brief Prototype of callback function used to receive the waypoint v3 mission state.
 * @warning User can not execute blocking style operations or functions in callback function, because that will block
 * root thread, causing problems such as slow system response, payload disconnection or infinite loop.
 * @param missionState: current waypoint v3 mission state.
 * @return Execution result.
 */
typedef T_DjiReturnCode (*WaypointV3MissionStateCallback)(T_DjiWaypointV3MissionState missionState);

/**
 * @brief Prototype of callback function used to receive the waypoint v3 mission state.
 * @warning User can not execute blocking style operations or functions in callback function, because that will block
 * root thread, causing problems such as slow system response, payload disconnection or infinite loop.
 * @param missionState: current waypoint v3 mission state.
 * @return Execution result.
 */
typedef T_DjiReturnCode (*WaypointV3ActionStateCallback)(T_DjiWaypointV3ActionState missionState);

/**
 * @brief Prototype of callback function used to receive ADN assist positioning mode status.
 * @warning User can not execute blocking style operations or functions in callback function, because that will block
 * root thread, causing problems such as slow system response, payload disconnection or infinite loop.
 * @param info: current assist positioning mode status pushed by the flight controller.
 * @return Execution result.
 */
typedef T_DjiReturnCode (*WaypointV3AssistPosModeInfoCallback)(T_DjiWaypointV3AssistPosModeInfo info);

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Initialise waypoint v3 module, and user should call this function.
 * before using waypoint v3 features.
 * @return Execution result.
 */
T_DjiReturnCode DjiWaypointV3_Init(void);

/**
 * @brief Deinitialise waypoint v3 module.
 * @return Execution result.
 */
T_DjiReturnCode DjiWaypointV3_DeInit(void);

/**
 * @brief Upload kmz file by raw data.
 * @note The size of kmz file is very small, you can use this interface to upload quickly.
 * @return Execution result.
 */
T_DjiReturnCode DjiWaypointV3_UploadKmzFile(const uint8_t *data, uint32_t dataLen);

/**
 * @brief Execute the mission action.
 * @note This action should be called after uploading the kmz file.
 * @return Execution result.
 */
T_DjiReturnCode DjiWaypointV3_Action(E_DjiWaypointV3Action action);

/**
 * @brief Register the mission state callback for waypoint mission.
 * @note If you want to monitor the state of waypoint mission, this interface should be called before uploading kmz
 * file or executing this mission action.
 * @return Execution result.
 */
T_DjiReturnCode DjiWaypointV3_RegMissionStateCallback(WaypointV3MissionStateCallback callback);

/**
 * @brief Register the action state callback for waypoint mission.
 * @note If you want to monitor the state of waypoint action, this interface should be called before uploading kmz
 * file or executing this mission action.
 * @return Execution result.
 */
T_DjiReturnCode DjiWaypointV3_RegActionStateCallback(WaypointV3ActionStateCallback callback);

/**
 * @brief Register the callback for ADN assist positioning mode status.
 * @note  This interface should be called after DjiWaypointV3_Init and before entering ADN mode.
 *        The callback is invoked each time the flight controller pushes an assist positioning
 *        mode status update. Pass NULL to unregister and stop receiving callbacks.
 *        This API is not available in the current version.
 * @return Execution result.
 */
T_DjiReturnCode DjiWaypointV3_RegAssistPosModeInfoCallback(WaypointV3AssistPosModeInfoCallback callback);


/**
 * @brief Send external localization data to the aircraft for ADN mode positioning.
 * @details In ADN mode, the aircraft uses the position data sent via this interface as its
 *          external position reference for no-GPS flight. This interface should be called
 *          continuously at a fixed frequency to maintain stable positioning.
 * @note This API is not available in the current version.
 * @param info Pointer to the localization data to send.
 * @return Execution result.
 */
T_DjiReturnCode DjiWaypointV3_SendExternalLocalizationInfo(
    const T_DjiWaypointV3ExternalLocalizationInfo *info);

/**
 * @brief Query the exit result of the most recent waypoint v3 mission.
 * @details This interface synchronously queries the flight controller for the result of the
 *          most recent waypoint v3 mission, including a status category (normal finish /
 *          abnormal exit / user exit / running / no mission) and the detailed exit reason
 *          (e.g. low battery, RTH, RC takeover, obstacle stop, etc.).
 * @note    This interface should be called after #DjiWaypointV3_Init. It is typically called
 *          after the mission state transitions out of #DJI_WAYPOINT_V3_MISSION_STATE_MISSION
 *          to learn why the mission ended.
 * @param exitResult: Output. Pointer to the variable that receives the exit result. Must
 *                    not be NULL.
 * @return Execution result.
 */
T_DjiReturnCode DjiWaypointV3_GetExitReason(T_DjiWaypointV3ExitResult *exitResult);

#ifdef __cplusplus
}
#endif

#endif // DJI_WAYPOINT_V3_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
