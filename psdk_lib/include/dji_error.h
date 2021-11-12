/**
 ********************************************************************
 * @file    dji_error.h
 * @brief   This is the header file for "dji_error.c", defining the structure and
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
#ifndef DJI_ERROR_H
#define DJI_ERROR_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/
#define DJI_ERROR_MODULE_INDEX_OFFSET  32u
#define DJI_ERROR_MODULE_INDEX_MASK    0x000000FF00000000u
#define DJI_ERROR_RAW_CODE_OFFSET      0u
#define DJI_ERROR_RAW_CODE_MASK        0x00000000FFFFFFFFu

#define DJI_ERROR_CODE(moduleIndex, rawErrCode) \
((uint64_t)0 | \
((((uint64_t)(moduleIndex)) << (DJI_ERROR_MODULE_INDEX_OFFSET)) & (DJI_ERROR_MODULE_INDEX_MASK)) | \
((((uint64_t)(rawErrCode)) << (DJI_ERROR_RAW_CODE_OFFSET)) & (DJI_ERROR_RAW_CODE_MASK)))

/**
 * @brief Error objects, specifying error code, error description, error reason and error recovery suggestion below.
 * @attention Require arrange by error code from small to large.
 */
#define DJI_ERROR_OBJECTS \
/* system module error message */ \
{DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS, "Execution successfully.", NULL}, \
{DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_REQUEST_PARAMETER, "Request parameters are invalid.", "Please double-check requested parameters."}, \
{DJI_ERROR_SYSTEM_MODULE_CODE_EXECUTING_HIGHER_PRIORITY_TASK, "A higher priority task is being executed.", "Please stop the higher priority task or try again later."}, \
{DJI_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT, "Operation is not supported.", "Please check input parameters or contact DJI for help."}, \
{DJI_ERROR_SYSTEM_MODULE_CODE_TIMEOUT, "Execution timeout.", "Please contact DJI for help."}, \
{DJI_ERROR_SYSTEM_MODULE_CODE_MEMORY_ALLOC_FAILED, "Memory allocation failed.", "Please check system configure."}, \
{DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER, "Input parameters are invalid.", "Please double-check requested parameters."}, \
{DJI_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT_IN_CURRENT_STATE, "Operation is not supported in current state.", "Please try again later."}, \
{DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR, "System error.", "Please contact DJI for help."}, \
{DJI_ERROR_SYSTEM_MODULE_CODE_HARDWARE_ERR, "Hardware error.", "Please contact DJI for help."}, \
{DJI_ERROR_SYSTEM_MODULE_CODE_INSUFFICIENT_ELECTRICITY, "Low battery.", "Please replacement battery for machine and try again."}, \
{DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN, "Unknown error.", NULL}, \
{DJI_ERROR_SYSTEM_MODULE_CODE_NOT_FOUND, "Parameters are not found.", NULL}, \
{DJI_ERROR_SYSTEM_MODULE_CODE_OUT_OF_RANGE, "Out of range.", "Please check parameters."}, \
{DJI_ERROR_SYSTEM_MODULE_CODE_BUSY, "System is busy.", "Please try again later."}, \
{DJI_ERROR_SYSTEM_MODULE_CODE_DUPLICATE, "Have existed the same object.", "Please input valid parameters."}, \
{DJI_ERROR_SYSTEM_MODULE_CODE_ADAPTER_NOT_MATCH, "PSDK adapter do not meet requirements.", "Please try again after replacing PSDK adapter."}, \
 \
/* gimbal module error message */ \
{DJI_ERROR_GIMBAL_MODULE_CODE_PITCH_REACH_POSITIVE_LIMIT, "Pitch axis gimbal reach positive limit.", "Please do not rotate towards positive direction."}, \
{DJI_ERROR_GIMBAL_MODULE_CODE_PITCH_REACH_NEGATIVE_LIMIT, "Pitch axis gimbal reach negative limit.", "Please do not rotate towards negative direction."}, \
{DJI_ERROR_GIMBAL_MODULE_CODE_ROLL_REACH_POSITIVE_LIMIT, "Roll axis gimbal reach positive limit.", "Please do not rotate towards positive direction."}, \
{DJI_ERROR_GIMBAL_MODULE_CODE_ROLL_REACH_NEGATIVE_LIMIT, "Roll axis gimbal reach negative limit.", "Please do not rotate towards negative direction."}, \
{DJI_ERROR_GIMBAL_MODULE_CODE_YAW_REACH_POSITIVE_LIMIT, "Yaw axis gimbal reach positive limit.", "Please do not rotate towards positive direction."}, \
{DJI_ERROR_GIMBAL_MODULE_CODE_YAW_REACH_NEGATIVE_LIMIT, "Yaw axis gimbal reach negative limit.", "Please do not rotate towards negative direction."}, \
{DJI_ERROR_GIMBAL_MODULE_CODE_NON_CONTROL_AUTHORITY, "Current device do not have control authority of the gimbal.", "Please do not control gimbal with other devices that have high control priority simultaneously."}, \
 \
/* payload collaboration module error message */ \
{DJI_ERROR_PAYLOAD_COLLABORATION_MODULE_CODE_POSITION_NOT_MATCH, "Payload mount position do not meet requirements.", "Please read API and user documentation, ensuring input parameters satisfy requirements."}, \
\
/* activation error message of identity module */ \
{DJI_ERROR_IDENTITY_MODULE_CODE_ACTIVATE_PARAMETER_ERROR, "Activation parameter error.", "Please check the validity of the activation parameters."}, \
{DJI_ERROR_IDENTITY_MODULE_CODE_ACTIVATE_ENCODE_ERROR, "Activation encode error", "Please check the validity of the activation parameters"}, \
{DJI_ERROR_IDENTITY_MODULE_CODE_ACTIVATE_NEW_DEVICE_ERROR, "Activate a new device error", \
"\r\n* Double-check your app_id and app_key. Does it match with your DJI developer account?\r\n"\
"* If this is a new device or has been previously activated with another app_id and app_key, you need to activate it through the DJI Assistant 2 with Internet.\r\n"\
"* Please make sure you download the correct version of DJI Assistant 2 for your drone.\r\n"\
}, \
{DJI_ERROR_IDENTITY_MODULE_CODE_ACTIVATE_SOFTWARE_NOT_CONNECTED, "Not connection to DJI Assistant 2.", "Please connect your drone to DJI Assistant 2 while activating the new device at first time."}, \
{DJI_ERROR_IDENTITY_MODULE_CODE_ACTIVATE_NETWORK_ERROR, "DJI Assistant 2 not connected to the internet.", "Please activate through the DJI Assistant 2 with Internet"}, \
{DJI_ERROR_IDENTITY_MODULE_CODE_ACTIVATE_SERVER_ACCESS_REFUSED, "DJI server refuse this activation request.", "Please check the validity of the activation parameters."}, \
{DJI_ERROR_IDENTITY_MODULE_CODE_ACTIVATE_ACCESS_LEVEL_ERROR, "Activate level error.", "Please check your app_id level."}, \
{DJI_ERROR_IDENTITY_MODULE_CODE_ACTIVATE_OSDK_VERSION_ERROR, "Activate a wrong osdk version", "Please check your OSDK version whether match the drone or not."}, \
\
/* subscription module error message */ \
{DJI_ERROR_SUBSCRIPTION_MODULE_CODE_INVALID_TOPIC_FREQ, "Frequency of topic is invalid.", "Please read API and user documentation, ensuring input parameters satisfy requirements."}, \
{DJI_ERROR_SUBSCRIPTION_MODULE_CODE_TOPIC_DUPLICATE, "Topic is subscribed repeatedly.", "Please do not subscribe a topic repeatedly."}, \
{DJI_ERROR_SUBSCRIPTION_MODULE_CODE_TOPIC_NOT_SUBSCRIBED, "Requested topic have not been subscribed.", "Please try to get value after subscribing topic."}, \
{DJI_ERROR_SUBSCRIPTION_MODULE_CODE_TIMESTAMP_NOT_ENABLE, "Requested topic do not have timestamp data. Did not enable timestamp when subscribe topic.", "Please subscribe topic enabled timestamp."},\
{DJI_ERROR_SUBSCRIPTION_MODULE_CODE_TOPIC_NOT_SUPPORTED, "Requested topic is not supported.", "Please check the topic is supported or not on current aircraft version."}, \
\
/* mop channel module error message */ \
{DJI_ERROR_MOP_CHANNEL_MODULE_CODE_CONNECTION_CLOSE, "Connection of channel is closed. The peer channel do not work or abnormally be closed.", "Please confirm state of the peer channel and reaccept the connection request of MSDK/OSDK"}, \
\
/* flight controller module error message */ \
{DJI_ERROR_FC_MODULE_CODE_RC_MODE_ERROR, "RC_MODE_ERROR", "Please check the RC mode"}, \
{DJI_ERROR_FC_MODULE_CODE_RELEASE_CONTROL_SUCCESS, "RELEASE_CONTROL_SUCCESS", NULL} ,\
{DJI_ERROR_FC_MODULE_CODE_OBTAIN_CONTROL_SUCCESS, "OBTAIN_CONTROL_SUCCESS", NULL} ,\
{DJI_ERROR_FC_MODULE_CODE_OBTAIN_CONTROL_IN_PROGRESS, "OBTAIN_CONTROL_IN_PROGRESS", NULL} ,\
{DJI_ERROR_FC_MODULE_CODE_RELEASE_CONTROL_IN_PROGRESS, "RELEASE_CONTROL_IN_PROGRESS", NULL} ,\
{DJI_ERROR_FC_MODULE_CODE_RC_NEED_MODE_P, "RC_NEED_MODE_P", NULL} ,\
{DJI_ERROR_FC_MODULE_CODE_RC_NEED_MODE_F, "RC_NEED_MODE_F", NULL} ,\
{DJI_ERROR_FC_MODULE_CODE_PARAM_READ_WRITE_INVALID_PARAM, "read or write invalid param", "Please check hash value and param value"} ,\
{DJI_ERROR_FC_MODULE_CODE_IOC_OBTAIN_CONTROL_ERROR, "IOC_OBTAIN_CONTROL_ERROR", NULL} ,\
{DJI_ERROR_FC_MODULE_CODE_KEY_ERROR, "Activate key error", NULL} ,\
{DJI_ERROR_FC_MODULE_CODE_NO_AUTHORIZATION_ERROR, "No authorization", "Please finish activation firstly"} ,\
{DJI_ERROR_FC_MODULE_CODE_NO_RIGHTS_ERROR, "No rights error", NULL} ,\
{DJI_ERROR_FC_MODULE_CODE_SYSTEM_ERROR, "Unknown system error", NULL} ,\
\
/* flight controller action module error message */\
{DJI_ERROR_FC_JOYSTICK_MODULE_OBTAIN_RELEASE_JOYSTICK_AUTH_SUCCESS, "Obtain/Release joystick authority success", NULL} ,\
{DJI_ERROR_FC_JOYSTICK_MODULE_DEVICE_NOT_ALLOW, "The requesting device is not allowed to obtain/release joystick control authority, only support OSDK/MSDK", "Please use right devices(OSDK/MSDK)"} ,\
{DJI_ERROR_FC_JOYSTICK_MODULE_TAKING_OFF, "Not allowed to obtain/release joystick control authority when drone is taking off ", "Please do it before or after taking off"} ,\
{DJI_ERROR_FC_JOYSTICK_MODULE_LANDING, "Not allowed to obtain/release joystick control authority when drone is landing", "Please do it before or after landing"} ,\
{DJI_ERROR_FC_JOYSTICK_MODULE_CMD_INVALID, "Invalid input command", "Please check your input command which only support 0/1"} ,\
{DJI_ERROR_FC_JOYSTICK_MODULE_RC_NOT_P_MODE, "Not allowed to obtain/release joystick control authority when rc is not in P_MODE", "Please switch RC to P_MODE"} ,\
{DJI_ERROR_FC_JOYSTICK_MODULE_CMD_LENGTH_ERROR, "Invalid length of input command", "Please input valid length command"} ,\
{DJI_ERROR_FC_JOYSTICK_MODULE_HAS_NO_JOYSTICK_AUTHORITY, "Not allowed to release joystick control authority when drone has no joystick authority", "Please obtain joystick authority first"} ,\
{DJI_ERROR_FC_JOYSTICK_MODULE_IN_RC_LOST_ACTION, "Not allowed to obtain/release joystick control authority when drone is executing rc lost action", "Please change battery"} ,\
\
/* flight controller action module error message */\
{DJI_ERROR_FC_ACTION_MODULE_TRIGGER_ERROR_MOTOR_ON, "now motor is on", "Please check motor status"} ,\
{DJI_ERROR_FC_ACTION_MODULE_TRIGGER_ERROR_MOTOR_OFF, "now motor is off", "Please check motor status"} ,\
{DJI_ERROR_FC_ACTION_MODULE_TRIGGER_ERROR_IN_AIR, "aircraft is in air", "Please check aircraft flight status"} ,\
{DJI_ERROR_FC_ACTION_MODULE_TRIGGER_ERROR_NOT_IN_AIR, "aircraft is not in air", "Please check aircraft flight status"} ,\
{DJI_ERROR_FC_ACTION_MODULE_TRIGGER_ERROR_HOME_POINT_NOT_SET, "home point not set", "Please set home point"} ,\
{DJI_ERROR_FC_ACTION_MODULE_TRIGGER_ERROR_BAD_GPS, "bad GPS", "Please fly where the GPS signal is good"} ,\
{DJI_ERROR_FC_ACTION_MODULE_TRIGGER_ERROR_IN_SIMULATION, "in simulation", "Please exit simulation first"} ,\
{DJI_ERROR_FC_ACTION_MODULE_TRIGGER_ERROR_CANNOT_START_MOTOR, "can not start motor", "Please check motor status"} ,\
{DJI_ERROR_FC_ACTION_MODULE_TRIGGER_ERROR_LOW_VOLTAGE, "low voltage", "Please change battery"} ,\
{DJI_ERROR_FC_ACTION_MODULE_TRIGGER_ERROR_SPEED_TOO_LARGE, "speed too large", "Please slow down"} ,\
                          \
/* flight controller arrest flying module error message */\
{DJI_ERROR_FC_ARREST_FLYING_MODULE_REGISTER_LOGOUT_SET_SUCCESS, "register/logout/execute arrest-flying success", NULL} ,\
{DJI_ERROR_FC_ARREST_FLYING_MODULE_REGISTER_LOGOUT_NULL_POINTER, "null pointer when register/logout arrest-flying", NULL} ,\
{DJI_ERROR_FC_ARREST_FLYING_MODULE_HMS_CODE_HAS_REGISTED, "duplicate hms code", NULL} ,\
{DJI_ERROR_FC_ARREST_FLYING_MODULE_HMS_CODE_NOT_FIND, "invalid hms code", NULL} ,\
{DJI_ERROR_FC_ARREST_FLYING_MODULE_ADD_ITEM_NO_DECRIPTION, "no description when register arrest-flying", NULL} ,\
{DJI_ERROR_FC_ARREST_FLYING_MODULE_REGISTER_ID_INVALID, "invalid request id", NULL} ,\
{DJI_ERROR_FC_ARREST_FLYING_MODULE_STOP_IN_AIR_HMSCODE_NOT_IN_WHITE_TABLE, "hms code is not in white list for allowing stop motor in the air", NULL} ,\
{DJI_ERROR_FC_ARREST_FLYING_MODULE_INVALID_FORMAT_HMSCODE, "invalid hms code format", NULL} ,\
{DJI_ERROR_FC_ARREST_FLYING_MODULE_HMSCODE_NOT_IN_WHITE_TABLE, "hms code is not in arrest-flying white list",NULL} ,\
\
/* flight controller home location module error message */\
{DJI_ERROR_FC_HOME_LOCATION_MODULE_UNKNOWN_FAILED_REASON, "set home location fail, unknown reason", NULL} ,\
{DJI_ERROR_FC_HOME_LOCATION_MODULE_INVALID_GPS_COORDINATE, "invalid GPS coordinate when set APP or RC to be home location", NULL} ,\
{DJI_ERROR_FC_HOME_LOCATION_MODULE_NOT_BE_RECORD, "home location is not inited", "Please waiting for aircraft recording home location"} ,\
{DJI_ERROR_FC_HOME_LOCATION_MODULE_GPS_NOT_READY, "GPS level < 4", NULL} ,\
{DJI_ERROR_FC_HOME_LOCATION_MODULE_DIS_TOO_FAR, "new home location is more than 20km away from current home locaton(APP/RC)", NULL} ,\
\
/* flight controller emergency stop motor module error message */\
{DJI_ERROR_FC_EMERGENCY_STOP_MOTOR_MODULE_VERSION_NOT_MATCH, "emergency stop motor version not match", NULL} ,\
{DJI_ERROR_FC_EMERGENCY_STOP_MOTOR_MODULE_CMD_INVALID, "emergency stop motor cmd invalid", NULL} ,\
/* camera manager module error message */\
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND, "Command not supported", "Check the firmware or command validity"} ,\
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_TIMEOUT, "Camera's execution of this action has timed out", "Try again or check the firmware or command"} ,\
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_RAM_ALLOCATION_FAILED, "Camera's execution of this action is out of memory", "Please contact <dev@dji.com> for help."} ,\
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_INVALID_COMMAND_PARAMETER,  "Camera received invalid parameters", "Check the validity of the parameter"} ,\
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND_IN_CUR_STATE, "Camera is busy or the command is not supported in the Camera's current state", "Check current camera state is if appropriate fot the CMD"} ,\
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_CAMERA_TIME_NOT_SYNCHRONIZED, "The time stamp of the camera is not sync", "Please contact <dev@dji.com> for help."} ,\
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_PARAMETER_SET_FAILED, "Camera failed to set the parameters it received", "Please check the parameter to set is if supported in your devices."} ,\
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_PARAMETER_GET_FAILED, "Camera param get failed", "Please check the parameter to get is if supported in your devices."} ,\
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_SD_CARD_MISSING, "Camera has no SD Card", "Please install SD card."} ,\
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_SD_CARD_FULL, "The Camera's SD Card is full", "Please make sure the SD card has enough space."} ,\
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_SD_CARD_ERROR, "Error accessing the SD Card", "Please check the validity of the SD card."} ,\
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_SENSOR_ERROR, "Camera sensor error", "Please contact <dev@dji.com> for help."} ,\
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_SYSTEM_ERROR, "Camera system error", "Please recheck all the running conditions or contact <dev@dji.com> for help."} ,\
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_PARAMETER_TOTAL_TOO_LONG, "Camera param get failed", "Please check the validity of the parameter length"} ,\
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_MODULE_INACTIVATED, "Camera module is not activated", "Please activate the module first."} ,\
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_FIRMWARE_DATA_NUM_DISCONTINUOUS, "The seq number of Firmware data is invalid", "Please check the validity of the camera firmware."} ,\
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_FIRMWARE_VERIFICATION_ERROR, "Firmware check error", "Please check the validity of the camera firmware."} ,\
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_FLASH_WRITE_ERROR, "Camera flash write error", "Please contact <dev@dji.com> for help."} ,\
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_FIRMWARE_TYPE_MISMATCH, "Firmware type is invalid", "Please check the validity of the camera firmware."} ,\
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_REMOTE_CONTROL_UNCONNECTED, "Remote Control is disconnected now", "Please check the connection with remote control is if OK."} ,\
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_HARDWARE_ERROR, "Camera hardware error", "Please contact <dev@dji.com> for help."} ,\
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_AIRCRAFT_UNCONNECTED, "Disconnect with aircraft", "Please check the connection with aircraft is if OK."} ,\
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_CANNOT_UPGRADE_IN_CUR_STATE, "Camera cannot not upgrade in current status", "Please contact <dev@dji.com> for help."} ,\
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNDEFINE_ERROR, "Undefined error", "Please contact <dev@dji.com> for help."} ,\
\
/* gimbal manager module error message */\
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND, "Command not supported", "Check the firmware or command validity"} ,\
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_TIMEOUT, "Camera's execution of this action has timed out", "Try again or check the firmware or command"} ,\
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_RAM_ALLOCATION_FAILED, "Camera's execution of this action is out of memory", "Please contact <dev@dji.com> for help."} ,\
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_INVALID_COMMAND_PARAMETER,  "Camera received invalid parameters", "Check the validity of the parameter"} ,\
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND_IN_CUR_STATE, "Camera is busy or the command is not supported in the Camera's current state", "Check current camera state is if appropriate fot the CMD"} ,\
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_CAMERA_TIME_NOT_SYNCHRONIZED, "The time stamp of the camera is not sync", "Please contact <dev@dji.com> for help."} ,\
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_PARAMETER_SET_FAILED, "Camera failed to set the parameters it received", "Please check the parameter to set is if supported in your devices."} ,\
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_PARAMETER_GET_FAILED, "Camera param get failed", "Please check the parameter to get is if supported in your devices."} ,\
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_SD_CARD_MISSING, "Camera has no SD Card", "Please install SD card."} ,\
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_SD_CARD_FULL, "The Camera's SD Card is full", "Please make sure the SD card has enough space."} ,\
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_SD_CARD_ERROR, "Error accessing the SD Card", "Please check the validity of the SD card."} ,\
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_SENSOR_ERROR, "Camera sensor error", "Please contact <dev@dji.com> for help."} ,\
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_SYSTEM_ERROR, "Camera system error", "Please recheck all the running conditions or contact <dev@dji.com> for help."} ,\
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_PARAMETER_TOTAL_TOO_LONG, "Camera param get failed", "Please check the validity of the parameter length"} ,\
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_MODULE_INACTIVATED, "Camera module is not activated", "Please activate the module first."} ,\
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_FIRMWARE_DATA_NUM_DISCONTINUOUS, "The seq number of Firmware data is invalid", "Please check the validity of the camera firmware."} ,\
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_FIRMWARE_VERIFICATION_ERROR, "Firmware check error", "Please check the validity of the camera firmware."} ,\
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_FLASH_WRITE_ERROR, "Camera flash write error", "Please contact <dev@dji.com> for help."} ,\
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_FIRMWARE_TYPE_MISMATCH, "Firmware type is invalid", "Please check the validity of the camera firmware."} ,\
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_REMOTE_CONTROL_UNCONNECTED, "Remote Control is disconnected now", "Please check the connection with remote control is if OK."} ,\
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_HARDWARE_ERROR, "Camera hardware error", "Please contact <dev@dji.com> for help."} ,\
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_AIRCRAFT_UNCONNECTED, "Disconnect with aircraft", "Please check the connection with aircraft is if OK."} ,\
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_CANNOT_UPGRADE_IN_CUR_STATE, "Camera cannot not upgrade in current status", "Please contact <dev@dji.com> for help."} ,\
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_UNDEFINE_ERROR, "Undefined error", "Please contact <dev@dji.com> for help."} ,\
\
/* waypoint v2 module error message */   \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_COMMON_SUCCESS, "Execute waypoint v2 cmd successfully", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_COMMON_INVALID_DATA_LENGTH, "The length of the data is illegal based on the protocol ", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_COMMON_INVALD_FLOAT_NUM, "Invalid float number (NAN or INF) ", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_WP_VERSION_NO_MATCH, "Waypoint mission version can't match with firmware ", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_COMMON_UNKNOWN, "Fatal error	 Unexpected result	 ", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_RESV, "Reserved", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_INIT_WP_NUM_TOO_MANY, "Min_initial_waypoint_num is large than permitted_max_waypoint_num ",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_INIT_WP_NUM_TOO_FEW, "Min_initial_waypoint_num is less than permitted_min_waypoint_num ",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_INIT_INVALID_END_INDEX, "Waypoint_end_index is equal or large than total_waypoint_num ",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_UPLOAD_START_ID_GT_END_ID, "The start index is greater than end index of upload wps ",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_UPLOAD_END_ID_GT_TOTAL_NUM, "The end index of uplod wps is greater than inited total numbers ",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_DOWNLOAD_WPS_NOT_IN_STORED_RAGNE, "The index of first and end waypoint expected to download is not in range of stored in FC ",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_CUR_POS_IS_FAR_AWAY_FROM_FIRST_WP, "Current position is far away from the first waypoint. ",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_ADJ_WPS_TOO_CLOSE, "It is too close from two adjacent waypoints",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_ADJ_WPS_TOO_FAR, "The distance betwween two adjacent waypoints is not in[0.5m, 5000m]",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_UPLOAD_MAX_VEL_GT_GLOBAL, "The max vel of uplod wp is greater than global max vel ",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_UPLOAD_LOCAL_CRUISE_VEL_GT_LOCAL_MAX, "The local cruise vel of upload wp is greater than local max vel ",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_UPLOAD_LOCAL_CRUISE_VEL_GT_GLOBAL_MAX, "The local cruise vel of upload wp is greater than global max vel ",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_INIT_INVALID_GLOBAL_MAX_VEL, "Global_max_vel is greater than permitted_max_vel or less than permitted_min_vel ",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_GLOBAL_CRUISE_VEL_GT_MAX_VEL, "Global_cruise_vel is greater than global_max_vel ",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_INIT_INVALID_GOTO_FIRST_FLAG, "Goto_first_point_mode is out of range of waypoint_goto_first_flag_t_enum ",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_INIT_INVALID_FINISHED_ACTION, "Finished_action is out of range of wp_plan_finish_action_t_enum ",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_INIT_INVALID_RC_LOST_ACTION, "Rc_lost_action is out of range of wp_plan_rc_lost_action_t_enum ",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_UPLOAD_YAW_MODE_INVALID, "The yaw mode of upload wp is invalid. reference to waypoint2_yaw_mode_t defined in math_waypoint_planner.h ",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_UPLOAD_YAW_CMD_NOT_IN_RANGE, "The yaw command of upload wp is not in range. the range for MR:[-180 180]",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_UPLOAD_YAW_TURN_DIRECTION_INVALID, "The yaw turn direction of upload wp is invalid. it should be 0:clockwise or 1:anti-clockwise ",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_UPLOAD_WP_TYPE_INVALID, "The wp type of upload wp is invalid. reference to waypoint_type_t defined in math_waypoint_planner.h ",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_GO_STOP_CMD_INVALID, "Go/stop command is invalid. ",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_INVALID_PAUSE_RECOVERY_CMD, "The command of pause/recovery is not equal to any of the command enum ",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_INVALID_BREAK_RESTORE_CMD, "The command of break/restore is not equal to any of the command enum ",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_INIT_INVALID_REF_POINT, "Initial reference point position coordinate exceed set range ",NULL},    \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_DAMPING_DIS_GE_DIS_OF_ADJ_POINTS,  "The damping dis is greater than or equal the distance of adjacent point ",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_UPLOAD_CANNT_SET_WP_LINE_EXIT_TYPE, "Cann't set wp_line_exit type to wp ",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_INIT_INFO_NOT_UPLOADED, "The init info of Ground Station is not uploaded yet ",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_WP_HAS_NOT_UPLOADED, "The wp has not uploaded yet ",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_UPLOADED_WP_NOT_ENOUGH, "Min_initial_waypoint_num is not uploaded. ",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_GS_HAS_STARTED, "Waypoint plan has started when received go command. ",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_GS_NOT_RUNNING, "Waypoint plan not running when received stop command. ",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_GS_NOT_RUNNING_FOR_PAUSE_RECOVERY, "Ground station(GS) is not started(used by pause/recovery) ",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_GS_NOT_RUNNING_FOR_BREAK_RESTORE, "Ground station(GS) is not started(used by break/restore) ",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_NOT_IN_WP_MIS, "Not in the waypoint mission(MIS)(cannot pause/recovery or break/restore) ",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_MIS_HAS_BEEN_PAUSED, "The current status is paused",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_MIS_NOT_PAUSED, "Not in paused status",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_MIS_HAS_BEEN_BROKEN, "The current status is broken",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_MIS_NOT_BROKEN, "Not in break status",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_PAUSE_RECOVERY_NOT_SUPPORTED, "The configuration forbid using pause/recovery API ",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_BREAK_RESTORE_NOT_SUPPORTED, "The configuration forbid using break/restore API ",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_NO_BREAK_POINT, "No break point is recorded for restore ",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_NO_CUR_TRAJ_PROJECT, "No current trajectory project point is recorded for restore ",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_NO_NXT_TRAJ_PROJECT, "No next trajectory project point is recorded for restore ",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_NO_NNT_TRAJ_PROJECT, "No next the next trajectory project point is recorded for restore ",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_UPLOAD_WP_ID_NOT_CONTINUE, "The index of upload wp is not continue after the store wp ",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_WP_LINE_ENTER_NOT_SET_TO_START_WP, "The WP_LINE_ENTER wp_type set to a wp which is not the init start waypoint ",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_INIT_WP_WHEN_PLAN_HAS_STARTED, "The waypoint plan has started when initializing waypoint ",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_DAMPING_DIS_EXCEED_RANGE, "Waypoint damping distance exceed set range ",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_WAYPOINT_COOR_EXCEED_RANGE, "Waypoint position coordinate exceed rational range ",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_FIRST_WP_TYPE_IS_WP_TURN_NO, "First waypoint type error",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_WP_EXCEED_RADIUS_LIMIT, "Waypoint position exceed radius limit ",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_WP_EXCEED_HEIGHT_LIMIT, "Waypoint position exceed height limit ",NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_STATUS_RESV, "Reserved error code", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_STATUS_WP_MIS_CHECK_FAIL, "Head_node is null or atti_not_healthy or gyro_not_healthy or horiz_vel_not healthy or horiz_abs_pos_not_healthy. ", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_STATUS_HOME_NOT_RECORDED, "The home point is no recorded yet	 which will be executed at the first time of GPS level > 3(MR	FW). ", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_STATUS_LOW_LOCATION_ACCURACY, "Current location accuracy is low for bad GPS signal. ", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_STATUS_RTK_CONDITION_IS_NOT_READY, "Use rtk_data	 but rtk is not connected or rtk_data is invalid ", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_SECURE_RESV, "Reserved error code", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_SECURE_CROSS_NFZ, "The trajectory cross the NFZ ", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_SECURE_BAT_LOW, "Current capacity of smart battery or voltage of non-smart battery is lower than level 1 or level 2 threshold ", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTION_COMMON_RESV, "Reserved error code", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTION_COMMON_ACTION_ID_DUPLICATED, "The ID of Action is duplicated. ", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTION_COMMON_ACTION_ITEMS_SPACE_NOT_ENOUGH, "There is no enough memory space for new Action Item ", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTION_COMMON_ACTION_SIZE_GT_BUF_SIZE, "The size of buffer used to get the info of Action is less than the size of Action. Normally users can not get this. ", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTION_COMMON_ACTION_ID_NOT_FOUND, "The ID of Action is not found. ", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTION_COMMON_DOWNLOAD_ACTION_ID_RANGE_ERROR, "The download action start id is bigger than the action end id ", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTION_COMMON_NO_ACTION_ITEMS_STORED, "Can not download or get min-max action ID for no action items stored in action kernel ", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRIGGER_RESV, "Reserved error code", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRIGGER_TYPE_INVALID, "The type ID of Trigger is invalid. It might not defined or the information is empty. ", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRIGGER_REACH_WP_END_INDEX_LT_START_INDEX, "Wp_end_index is less than wp_start_index in reach_waypoint_trigger. ", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRIGGER_REACH_WP_INVALID_INTERVAL_WP_NUM, "Interval_wp_num is large than the difference of wp_start_index and wp_end_index in reach_waypoint_trigger. ", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRIGGER_REACH_WP_INVALID_AUTO_TERMINATE_WP_NUM , "Auto_terminate_wp_num is large than interval_wp_num in reach_waypoint_trigger. ", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRIGGER_ASSOCIATE_INVALID_TYPE, "The associate_type is greater than the maximum value. ", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRIGGER_SIMPLE_INTERVAL_INVALID_TYPE, "The interval type is greater than the maximum value. ", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_COMMON_RESV, "Reserved error code", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_COMMON_ACTUATOR_EXEC_NON_SUPPORTED, "The execution of Actuator is not supported	 e.g.	 try to stop camera shooting. ", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_COMMON_ACTUATOR_TYPE_INVALID, "The type ID of Actuator is invalid. It might not defined or the information is empty. ", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_COMMON_ACTUATOR_FUNC_INVALID, "The Function ID of Actuator is invalid. It might not defined or the information is empty. ", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_CAMERA_RESV, "Reserved error code", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_CAMERA_SEND_SINGLE_SHOT_CMD_TO_CAMERA_FAIL, "Fail to send shot cmd to camera for no camera or camera is busy. ", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_CAMERA_SEND_VIDEO_START_CMD_TO_CAMERA_FAIL, "Fail to send video start cmd to camera for no camera or camera is busy. ", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_CAMERA_SEND_VIDEO_STOP_CMD_TO_CAMERA_FAIL, "Fail to send video stop cmd to camera for no camera or camera is not busy. ", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_CAMERA_FOCUS_PARAM_XY_INVALID, "Camera focus param xy exceed valid range (0	 1). ", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_CAMERA_SEND_FOCUS_CMD_TO_CAMERA_FAIL, "Fail to send focus cmd to camera for no camera or camera is busy. ", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_CAMERA_SEND_FOCALIZE_CMD_TO_CAMERA_FAIL, "Fail to send focalize cmd to camera for no camera or camera is busy. ", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_CAMERA_FOCAL_DISTANCE_INVALID, "Focal distance of camera focalize function exceed valid range. ", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_CAMERA_EXEC_FAIL, "This err code indicate camera fail to exec coressponding cmd	 and the low 8 bit", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_GIMBAL_RESV, "Reserved error code", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_GIMBAL_INVALID_RPY_ANGLE_CTRL_CMD, "Gimbal roll	pitch	yaw angle ctrl cmd param invalid	", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_GIMBAL_INVALID_DURATION_CMD, "Gimbal duration param invalid	 unable to exec. ", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_GIMBAL_FAIL_TO_ARRIVE_TGT_ANGLE, "Gimbal fail to arrive target angle . ", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_GIMBAL_FAIL_TO_SEND_CMD_TO_GIMBAL, "Fail to send cmd to gimbal for gimbal is busy or no gimbal. ", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_GIMBAL_THIS_INDEX_OF_GIMBAL_NOT_DOING_UNIFORM_CTRL , "Fail to stop gimbal uniform ctrl because index error. ", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_FLIGHT_RESV, "Reserved error code", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_FLIGHT_YAW_INVALID_YAW_ANGLE, "Yaw angle is lager max yaw angle. ", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_FLIGHT_YAW_TO_TGT_ANGLE_TIMEOUT, "Failed to target yaw angle because of timeout. ", NULL}, \



#define DJI_RETURN_CODE_OK                 DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS /*!< Payload SDK return code represents as status is ok. */
#define DJI_RETURN_CODE_ERR_ALLOC          DJI_ERROR_SYSTEM_MODULE_CODE_MEMORY_ALLOC_FAILED /*!< Payload SDK return code represents as status alloc error. */
#define DJI_RETURN_CODE_ERR_TIMEOUT        DJI_ERROR_SYSTEM_MODULE_CODE_TIMEOUT /*!< Payload SDK return code represents as status timeout error. */
#define DJI_RETURN_CODE_ERR_NOT_FOUND      DJI_ERROR_SYSTEM_MODULE_CODE_NOT_FOUND /*!< Payload SDK return code represents as status not found error. */
#define DJI_RETURN_CODE_ERR_OUT_OF_RANGE   DJI_ERROR_SYSTEM_MODULE_CODE_OUT_OF_RANGE /*!< Payload SDK return code represents as status out of range error. */
#define DJI_RETURN_CODE_ERR_PARAM          DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER /*!< Payload SDK return code represents as status parameter error. */
#define DJI_RETURN_CODE_ERR_SYSTEM         DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR /*!< Payload SDK return code represents as status system error. */
#define DJI_RETURN_CODE_ERR_BUSY           DJI_ERROR_SYSTEM_MODULE_CODE_BUSY /*!< Payload SDK return code represents as status busy error. */
#define DJI_RETURN_CODE_ERR_UNSUPPORT      DJI_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT /*!< Payload SDK return code represents as status nonsupport error. */
#define DJI_RETURN_CODE_ERR_UNKNOWN        DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN /*!< Payload SDK return code represents as status unknown error. */

/* Exported types ------------------------------------------------------------*/
/**
 * @brief DJI module enum for defining error code.
 */
typedef enum {
    DJI_ERROR_MODULE_SYSTEM = 0,
    DJI_ERROR_MODULE_PLATFORM,
    DJI_ERROR_MODULE_LOGGER,
    DJI_ERROR_MODULE_TIME_SYNC,
    DJI_ERROR_MODULE_COMMAND,
    DJI_ERROR_MODULE_CAMERA,
    DJI_ERROR_MODULE_GIMBAL,
    DJI_ERROR_MODULE_XPORT,
    DJI_ERROR_MODULE_PAYLOAD_COLLABORATION,
    DJI_ERROR_MODULE_WIDGET,
    DJI_ERROR_MODULE_CORE,
    DJI_ERROR_MODULE_IDENTITY,
    DJI_ERROR_MODULE_TRANSMISSION,
    DJI_ERROR_MODULE_DATA_CHANNEL,
    DJI_ERROR_MODULE_SUBSCRIPTION,
    DJI_ERROR_MODULE_MOP_CHANNEL,
    DJI_ERROR_MODULE_POSITIONING,
    DJI_ERROR_MODULE_POWER_MANAGEMENT,
    DJI_ERROR_MODULE_AIRCRAFTINFO,
    DJI_ERROR_MODULE_PRODUCTINFO,
    DJI_ERROR_MODULE_FLOWCONTROLLER,
    DJI_ERROR_MODULE_DOWNLOADER,
    DJI_ERROR_MODULE_PARAMETER,
    DJI_ERROR_MODULE_UTIL,
    DJI_ERROR_MODULE_USER,
    DJI_ERROR_MODULE_NEGOTIATE,
    DJI_ERROR_MODULE_UPGRADE,
    DJI_ERROR_MODULE_FC_BASIC,
    DJI_ERROR_MODULE_FC_JOYSTICK,
    DJI_ERROR_MODULE_FC_ACTION,
    DJI_ERROR_MODULE_FC_ARREST_FLYING,
    DJI_ERROR_MODULE_FC_HOME_LOCATION,
    DJI_ERROR_MODULE_FC_EMERGENCY_STOP_MOTOR,
    DJI_ERROR_MODULE_CAMERA_MANAGER,
    DJI_ERROR_MODULE_GIMBAL_MANAGER,
    DJI_ERROR_MODULE_WAYPOINT_V2,
    DJI_ERROR_MODULE_ERROR,
} E_DjiErrorModule;

/**
 * @brief Raw error code of system module.
 */
typedef enum {
    DJI_ERROR_SYSTEM_MODULE_RAW_CODE_SUCCESS = 0x000,
    DJI_ERROR_SYSTEM_MODULE_RAW_CODE_INVALID_REQUEST_PARAMETER = 0x0D4,
    DJI_ERROR_SYSTEM_MODULE_RAW_CODE_EXECUTING_HIGHER_PRIORITY_TASK = 0x0D7,
    DJI_ERROR_SYSTEM_MODULE_RAW_CODE_NONSUPPORT = 0x0E0,
    DJI_ERROR_SYSTEM_MODULE_RAW_CODE_TIMEOUT = 0x0E1,
    DJI_ERROR_SYSTEM_MODULE_RAW_CODE_MEMORY_ALLOC_FAILED = 0x0E2,
    DJI_ERROR_SYSTEM_MODULE_RAW_CODE_INVALID_PARAMETER = 0x0E3,
    DJI_ERROR_SYSTEM_MODULE_RAW_CODE_NONSUPPORT_IN_CURRENT_STATE = 0x0E4,
    DJI_ERROR_SYSTEM_MODULE_RAW_CODE_SYSTEM_ERROR = 0x0EC,
    DJI_ERROR_SYSTEM_MODULE_RAW_CODE_HARDWARE_ERR = 0x0FA,
    DJI_ERROR_SYSTEM_MODULE_RAW_CODE_INSUFFICIENT_ELECTRICITY = 0x0FB,
    DJI_ERROR_SYSTEM_MODULE_RAW_CODE_UNKNOWN = 0x0FF,
    DJI_ERROR_SYSTEM_MODULE_RAW_CODE_NOT_FOUND = 0x100,
    DJI_ERROR_SYSTEM_MODULE_RAW_CODE_OUT_OF_RANGE = 0x101,
    DJI_ERROR_SYSTEM_MODULE_RAW_CODE_BUSY = 0x102,
    DJI_ERROR_SYSTEM_MODULE_RAW_CODE_DUPLICATE = 0x103,
    DJI_ERROR_SYSTEM_MODULE_RAW_CODE_ADAPTER_NOT_MATCH = 0x104,
} E_DjiErrorSystemModuleRawCode;

/**
 * @brief Raw error code of activation of identity module.
 */
typedef enum {
    DJI_ERROR_IDENTITY_MODULE_RAW_CODE_ACTIVATE_PARAMETER_ERROR = 0x0001,
    DJI_ERROR_IDENTITY_MODULE_RAW_CODE_ACTIVATE_ENCODE_ERROR = 0x0002,
    DJI_ERROR_IDENTITY_MODULE_RAW_CODE_ACTIVATE_NEW_DEVICE_ERROR = 0x0003,
    DJI_ERROR_IDENTITY_MODULE_RAW_CODE_ACTIVATE_SOFTWARE_NOT_CONNECTED = 0x0004,
    DJI_ERROR_IDENTITY_MODULE_RAW_CODE_ACTIVATE_NETWORK_ERROR = 0x0005,
    DJI_ERROR_IDENTITY_MODULE_RAW_CODE_ACTIVATE_SERVER_ACCESS_REFUSED = 0x0006,
    DJI_ERROR_IDENTITY_MODULE_RAW_CODE_ACTIVATE_ACCESS_LEVEL_ERROR = 0x0007,
    DJI_ERROR_IDENTITY_MODULE_RAW_CODE_ACTIVATE_OSDK_VERSION_ERROR = 0x0008,
} E_DjiErrorIdentityModuleActivationRawCode;

/**
 * @brief Raw error code of gimbal module.
 */
typedef enum {
    DJI_ERROR_GIMBAL_MODULE_RAW_CODE_PITCH_REACH_POSITIVE_LIMIT = 0x000,
    DJI_ERROR_GIMBAL_MODULE_RAW_CODE_PITCH_REACH_NEGATIVE_LIMIT = 0x001,
    DJI_ERROR_GIMBAL_MODULE_RAW_CODE_ROLL_REACH_POSITIVE_LIMIT = 0x002,
    DJI_ERROR_GIMBAL_MODULE_RAW_CODE_ROLL_REACH_NEGATIVE_LIMIT = 0x003,
    DJI_ERROR_GIMBAL_MODULE_RAW_CODE_YAW_REACH_POSITIVE_LIMIT = 0x004,
    DJI_ERROR_GIMBAL_MODULE_RAW_CODE_YAW_REACH_NEGATIVE_LIMIT = 0x005,
    DJI_ERROR_GIMBAL_MODULE_RAW_CODE_NON_CONTROL_AUTHORITY = 0x006,
} E_DjiErrorGimbalModuleRawCode;

/**
 * @brief Raw error code of payload collaboration module.
 */
typedef enum {
    DJI_ERROR_PAYLOAD_COLLABORATION_MODULE_RAW_CODE_POSITION_NOT_MATCH = 0x000,
} E_DjiErrorPayloadCollaborationModuleRawCode;

/**
 * @brief Raw error code of subscription module.
 */
typedef enum {
    DJI_ERROR_SUBSCRIPTION_MODULE_RAW_CODE_INVALID_TOPIC_FREQ = 0x000,
    DJI_ERROR_SUBSCRIPTION_MODULE_RAW_CODE_TOPIC_DUPLICATE = 0x001,
    DJI_ERROR_SUBSCRIPTION_MODULE_RAW_CODE_TOPIC_NOT_SUBSCRIBED = 0x002,
    DJI_ERROR_SUBSCRIPTION_MODULE_RAW_CODE_TIMESTAMP_NOT_ENABLE = 0x003,
    DJI_ERROR_SUBSCRIPTION_MODULE_RAW_CODE_TOPIC_NOT_SUPPORTED = 0x004,
} E_DjiErrorSubscriptionModuleRawCode;

/**
 * @brief Raw error code of mop channel module.
 */
typedef enum {
    DJI_ERROR_MOP_CHANNEL_MODULE_RAW_CODE_CONNECTION_CLOSE = 0x00B,
} E_DjiErrorMopChannelModuleRawCode;

/**
 * @brief Raw error code of flight controller module.
 */
typedef enum {
    DJI_ERROR_FC_MODULE_RAW_CODE_RC_MODE_ERROR = 0x0000,
    DJI_ERROR_FC_MODULE_RAW_CODE_RELEASE_CONTROL_SUCCESS = 0x0001,
    DJI_ERROR_FC_MODULE_RAW_CODE_OBTAIN_CONTROL_SUCCESS = 0x0002,
    DJI_ERROR_FC_MODULE_RAW_CODE_OBTAIN_CONTROL_IN_PROGRESS = 0x0003,
    DJI_ERROR_FC_MODULE_RAW_CODE_RELEASE_CONTROL_IN_PROGRESS = 0x0004,
    DJI_ERROR_FC_MODULE_RAW_CODE_RC_NEED_MODE_P = 0x0005,
    DJI_ERROR_FC_MODULE_RAW_CODE_RC_NEED_MODE_F = 0x0006,
    DJI_ERROR_FC_MODULE_RAW_CODE_PARAM_READ_WRITE_INVALID_PARAM = 0x0007,
    DJI_ERROR_FC_MODULE_RAW_CODE_IOC_OBTAIN_CONTROL_ERROR = 0x00C9,
    DJI_ERROR_FC_MODULE_RAW_CODE_KEY_ERROR = 0xFF00,
    DJI_ERROR_FC_MODULE_RAW_CODE_NO_AUTHORIZATION_ERROR = 0xFF01,
    DJI_ERROR_FC_MODULE_RAW_CODE_NO_RIGHTS_ERROR = 0xFF02,
    DJI_ERROR_FC_MODULE_RAW_CODE_SYSTEM_ERROR = 0xFF03,
} E_DjiErrorFCModuleRawCode;

/**
 * @brief Raw error code of flight controller joystick module.
 */
typedef enum {
    DJI_ERROR_FC_JOYSTICK_MODULE_RAW_CODE_SUCCESS = 0x00,
    DJI_ERROR_FC_JOYSTICK_MODULE_RAW_CODE_DEVICE_NOT_ALLOW = 0x01,
    DJI_ERROR_FC_JOYSTICK_MODULE_RAW_CODE_ENGINE_STARTING = 0x02,
    DJI_ERROR_FC_JOYSTICK_MODULE_RAW_CODE_TAKING_OFF = 0x03,
    DJI_ERROR_FC_JOYSTICK_MODULE_RAW_CODE_LANDING = 0x04,
    DJI_ERROR_FC_JOYSTICK_MODULE_RAW_CODE_CMD_INVALID = 0x05,
    DJI_ERROR_FC_JOYSTICK_MODULE_RAW_CODE_RC_NOT_P_MODE = 0x06,
    DJI_ERROR_FC_JOYSTICK_MODULE_RAW_CODE_CMD_LENGTH_ERROR = 0x07,
    DJI_ERROR_FC_JOYSTICK_MODULE_RAW_CODE_HAS_NO_JOYSTICK_AUTHORITY = 0x08,
    DJI_ERROR_FC_JOYSTICK_MODULE_RAW_CODE_IN_RC_LOST_ACTION = 0x09,
} E_DjiErrorFCJoystickModuleRawCode;

/**
 * @brief Raw error code of flight controller action module.
 */
typedef enum {
    DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_TRIGGER_ERROR_MOTOR_ON = 0x01,
    DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_TRIGGER_ERROR_MOTOR_OFF = 0x02,
    DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_TRIGGER_ERROR_IN_AIR = 0x03,
    DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_TRIGGER_ERROR_NOT_IN_AIR = 0x04,
    DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_TRIGGER_ERROR_HOME_POINT_NOT_SET = 0x05,
    DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_TRIGGER_ERROR_BAD_GPS = 0x06,
    DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_TRIGGER_ERROR_IN_SIMULATION = 0x07,
    DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_TRIGGER_ERROR_CANNOT_START_MOTOR = 0x11,
    DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_TRIGGER_ERROR_LOW_VOLTAGE = 0x12,
    DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_TRIGGER_ERROR_SPEED_TOO_LARGE = 0x14,
} E_DjiErrorFCActionModuleRawCode;

/**
 * @brief Raw error code of flight controller arrest flying module.
 */
typedef enum {
    DJI_ERROR_FC_ARREST_FLYING_MODULE_RAW_CODE_REGISTER_LOGOUT_SET_SUCCESS = 0x00,
    DJI_ERROR_FC_ARREST_FLYING_MODULE_RAW_CODE_REGISTER_LOGOUT_NULL_POINTER = 0x01,
    DJI_ERROR_FC_ARREST_FLYING_MODULE_RAW_CODE_HMS_CODE_HAS_REGISTED = 0x02,
    DJI_ERROR_FC_ARREST_FLYING_MODULE_RAW_CODE_HMS_CODE_NOT_FIND = 0x03,
    DJI_ERROR_FC_ARREST_FLYING_MODULE_RAW_CODE_ADD_ITEM_NO_DECRIPTION = 0x04,
    DJI_ERROR_FC_ARREST_FLYING_MODULE_RAW_CODE_REGISTER_ID_INVALID = 0x05,
    DJI_ERROR_FC_ARREST_FLYING_MODULE_RAW_CODE_STOP_IN_AIR_HMSCODE_NOT_IN_WHITE_TABLE = 0x06,
    DJI_ERROR_FC_ARREST_FLYING_MODULE_RAW_CODE_INVALID_FORMAT_HMSCODE = 0x07,
    DJI_ERROR_FC_ARREST_FLYING_MODULE_RAW_CODE_HMSCODE_NOT_IN_WHITE_TABLE = 0x08,
} E_DjiErrorFCArrestFlyingModuleRawCode;

/**
 * @brief Raw error code of flight controller home location module.
 */
typedef enum {
    DJI_ERROR_FC_HOME_LOCATION_MODULE_RAW_CODE_UNKNOWN_FAILED_REASON = 0x01,
    DJI_ERROR_FC_HOME_LOCATION_MODULE_RAW_CODE_INVALID_GPS_COORDINATE = 0x02,
    DJI_ERROR_FC_HOME_LOCATION_MODULE_RAW_CODE_NOT_BE_RECORD = 0x03,
    DJI_ERROR_FC_HOME_LOCATION_MODULE_RAW_CODE_GPS_NOT_READY = 0x04,
    DJI_ERROR_FC_HOME_LOCATION_MODULE_RAW_CODE_DIS_TOO_FAR = 0x05,
} E_DjiErrorFCHomeLocationModuleRawCode;

/**
 * @brief Raw error code of flight controller emergency stop motor module.
 */
typedef enum {
    DJI_ERROR_FC_EMERGENCY_STOP_MOTOR_MODULE_RAW_CODE_VERSION_NOT_MATCH = 0x01,
    DJI_ERROR_FC_EMERGENCY_STOP_MOTOR_MODULE_RAW_CODE_CMD_INVALID = 0x02,
} E_DjiErrorFCKillSwitchModuleRawCode;

/**
 * @brief Raw error code of camera manager
 */
typedef enum {
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_UNSUPPORTED_COMMAND = 0xE0, /*!< Do not support this command */
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_TIMEOUT = 0xE1, /*!< Execution timeout */
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_RAM_ALLOCATION_FAILED = 0xE2, /*!< Memory alloc failed */
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_INVALID_COMMAND_PARAMETER = 0xE3, /*!< Invalid parameter for the command */
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_UNSUPPORTED_COMMAND_IN_CUR_STATE = 0xE4, /*!< Do not support this command in the current state */
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_CAMERA_TIME_NOT_SYNCHRONIZED = 0xE5, /*!< Timestamp of camera is not synchronized */
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_PARAMETER_SET_FAILED = 0xE6, /*!< Setting parameter failed */
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_PARAMETER_GET_FAILED = 0xE7, /*!< Getting parameter failed */
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_SD_CARD_MISSING = 0xE8, /*!< SD card is not installed */
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_SD_CARD_FULL = 0xE9, /*!< SD card is full */
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_SD_CARD_ERROR = 0xEA, /*!< Error accessing the SD Card */
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_SENSOR_ERROR = 0xEB, /*!< Sensor go wrong */
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_SYSTEM_ERROR = 0xEC, /*!< System error */
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_PARAMETER_TOTAL_TOO_LONG = 0xED, /*!< Length of the parameter is too long */
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_MODULE_INACTIVATED = 0xEE, /*!< Module is too not activated yet */
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_FIRMWARE_DATA_NUM_DISCONTINUOUS = 0xF0, /*!< Fireware data number is a discontinuous number */
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_FIRMWARE_VERIFICATION_ERROR = 0xF2, /*!< Error verifying fireware */
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_FLASH_WRITE_ERROR = 0xF4, /*!< Error writing flash */
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_FIRMWARE_TYPE_MISMATCH = 0xF6, /*!< Firmware type don't match */
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_REMOTE_CONTROL_UNCONNECTED = 0xF8, /*!< Not connect remote control yet */
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_HARDWARE_ERROR = 0xFA, /*!< Hardware fault */
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_AIRCRAFT_UNCONNECTED = 0xFC, /*!< Aircraft is not connected yet */
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_CANNOT_UPGRADE_IN_CUR_STATE = 0xFE, /*!< Cannot upgrade in current status (Please reboot or contact with DJI support */
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_UNDEFINE_ERROR = 0xFF, /*!< Undefined error */
} E_DjiErrorCameraManagerModuleRawCode;

/**
 * @brief Raw error code of gimbal manager
 */
typedef enum {
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_UNSUPPORTED_COMMAND = 0xE0, /*!< Do not support this command */
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_TIMEOUT = 0xE1, /*!< Execution timeout */
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_RAM_ALLOCATION_FAILED = 0xE2, /*!< Memory alloc failed */
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_INVALID_COMMAND_PARAMETER = 0xE3, /*!< Invalid parameter for the command */
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_UNSUPPORTED_COMMAND_IN_CUR_STATE = 0xE4, /*!< Do not support this command in the current state */
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_CAMERA_TIME_NOT_SYNCHRONIZED = 0xE5, /*!< Timestamp of camera is not synchronized */
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_PARAMETER_SET_FAILED = 0xE6, /*!< Setting parameter failed */
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_PARAMETER_GET_FAILED = 0xE7, /*!< Getting parameter failed */
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_SD_CARD_MISSING = 0xE8, /*!< SD card is not installed */
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_SD_CARD_FULL = 0xE9, /*!< SD card is full */
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_SD_CARD_ERROR = 0xEA, /*!< Error accessing the SD Card */
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_SENSOR_ERROR = 0xEB, /*!< Sensor go wrong */
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_SYSTEM_ERROR = 0xEC, /*!< System error */
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_PARAMETER_TOTAL_TOO_LONG = 0xED, /*!< Length of the parameter is too long */
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_MODULE_INACTIVATED = 0xEE, /*!< Module is too not activated yet */
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_FIRMWARE_DATA_NUM_DISCONTINUOUS = 0xF0, /*!< Fireware data number is a discontinuous number */
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_FIRMWARE_VERIFICATION_ERROR = 0xF2, /*!< Error verifying fireware */
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_FLASH_WRITE_ERROR = 0xF4, /*!< Error writing flash */
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_FIRMWARE_TYPE_MISMATCH = 0xF6, /*!< Firmware type don't match */
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_REMOTE_CONTROL_UNCONNECTED = 0xF8, /*!< Not connect remote control yet */
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_HARDWARE_ERROR = 0xFA, /*!< Hardware fault */
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_AIRCRAFT_UNCONNECTED = 0xFC, /*!< Aircraft is not connected yet */
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_CANNOT_UPGRADE_IN_CUR_STATE = 0xFE, /*!< Cannot upgrade in current status (Please reboot or contact with DJI support */
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_UNDEFINE_ERROR = 0xFF, /*!< Undefined error */
} E_DjiErrorGimbalManagerModuleRawCode;

/**
 * @brief Waypoint v2 total error raw code.
 */
typedef enum {
    /* Waypoint v2 common error raw code */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_COMMON_SUCCESS = 0x00000,
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_COMMON_INVALID_DATA_LENGTH = 0x00001,   /*!< the length of the data is illegal based on the protocol */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_COMMON_INVALD_FLOAT_NUM = 0x00002,   /*!< invalid float number (NAN or INF) */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_WP_VERSION_NO_MATCH = 0x00003,   /*!< waypoint mission version can't match with firmware*/
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_COMMON_UNKNOWN = 0x0ffff,   /*!< Fatal error! Unexpected result! */

    /* The enum that defines the detail error code of flight-trajectory module */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_RESV = 0x10000,
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_INIT_WP_NUM_TOO_MANY = 0x10001,   /*!< min_initial_waypoint_num is large than permitted_max_waypoint_num */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_INIT_WP_NUM_TOO_FEW = 0x10002,   /*!< min_initial_waypoint_num is less than permitted_min_waypoint_num */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_INIT_INVALID_END_INDEX = 0x10003,   /*!< waypoint_end_index is equal or large than total_waypoint_num */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_UPLOAD_START_ID_GT_END_ID = 0x10004,   /*!< the start index is greater than end index of upload wps */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_UPLOAD_END_ID_GT_TOTAL_NUM = 0x10005,   /*!< the end index of uplod wps is greater than inited total numbers */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_DOWNLOAD_WPS_NOT_IN_STORED_RAGNE = 0x10006,   /*!< the index of first and end waypoint expected to download is not in range of stored in FC */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_CUR_POS_IS_FAR_AWAY_FROM_FIRST_WP = 0x10008,   /*!< current position is far away from the first waypoint. */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_ADJ_WPS_TOO_CLOSE = 0x1000a,   /*!< it is too close from two adjacent waypoints, the value of which might be distinguish from products */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_ADJ_WPS_TOO_FAR = 0x1000b,   /*!< the distance betwween two adjacent waypoints is not in[0.5m, 5000m] the value of which might be distinguish from products */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_UPLOAD_MAX_VEL_GT_GLOBAL = 0x1000c,   /*!< the max vel of uplod wp is greater than global max vel */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_UPLOAD_LOCAL_CRUISE_VEL_GT_LOCAL_MAX = 0x1000d,   /*!< the local cruise vel of upload wp is greater than local max vel */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_UPLOAD_LOCAL_CRUISE_VEL_GT_GLOBAL_MAX = 0x1000e,   /*!< the local cruise vel of upload wp is greater than global max vel */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_INIT_INVALID_GLOBAL_MAX_VEL = 0x1000f,   /*!< global_max_vel is greater than permitted_max_vel or less than permitted_min_vel */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_GLOBAL_CRUISE_VEL_GT_MAX_VEL = 0x10010,   /*!< global_cruise_vel is greater than global_max_vel */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_INIT_INVALID_GOTO_FIRST_FLAG = 0x10011,   /*!< goto_first_point_mode is out of range of waypoint_goto_first_flag_t_enum */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_INIT_INVALID_FINISHED_ACTION = 0x10012,   /*!< finished_action is out of range of wp_plan_finish_action_t_enum */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_INIT_INVALID_RC_LOST_ACTION = 0x10013,   /*!< rc_lost_action is out of range of wp_plan_rc_lost_action_t_enum */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_UPLOAD_YAW_MODE_INVALID = 0x10014,   /*!< the yaw mode of upload wp is invalid. reference to waypoint2_yaw_mode_t defined in math_waypoint_planner.h */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_UPLOAD_YAW_CMD_NOT_IN_RANGE = 0x10015,   /*!< the yaw command of upload wp is not in range. the range for MR:[-180 180],unit:degree */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_UPLOAD_YAW_TURN_DIRECTION_INVALID = 0x10016,   /*!< the yaw turn direction of upload wp is invalid. it should be 0:clockwise or 1:anti-clockwise */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_UPLOAD_WP_TYPE_INVALID = 0x10017,   /*!< the wp type of upload wp is invalid. reference to waypoint_type_t defined in math_waypoint_planner.h */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_GO_STOP_CMD_INVALID = 0x10018,   /*!< go/stop command is invalid. */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_INVALID_PAUSE_RECOVERY_CMD = 0x10019,   /*!< the command of pause/recovery is not equal to any of the command enum */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_INVALID_BREAK_RESTORE_CMD = 0x1001a,   /*!< the command of break/restore is not equal to any of the command enum */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_INIT_INVALID_REF_POINT = 0x1001b,   /*!< initial reference point position coordinate exceed set range */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_DAMPING_DIS_GE_DIS_OF_ADJ_POINTS = 0x1001c,    /*!< the damping dis is greater than or equal the distance of adjacent point */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_UPLOAD_CANNT_SET_WP_LINE_EXIT_TYPE = 0x1001d,   /*!< cann't set wp_line_exit type to wp */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_INIT_INFO_NOT_UPLOADED = 0x1001e,   /*!< the init info of Ground Station is not uploaded yet */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_WP_HAS_NOT_UPLOADED = 0x1001f,   /*!< the wp has not uploaded yet */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_UPLOADED_WP_NOT_ENOUGH = 0x10020,   /*!< min_initial_waypoint_num is not uploaded. */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_GS_HAS_STARTED = 0x10021,   /*!< waypoint plan has started when received go command. */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_GS_NOT_RUNNING = 0x10022,   /*!< waypoint plan not running when received stop command. */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_GS_NOT_RUNNING_FOR_PAUSE_RECOVERY = 0x10023,   /*!< ground station(GS) is not started(used by pause/recovery) */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_GS_NOT_RUNNING_FOR_BREAK_RESTORE = 0x10024,   /*!< ground station(GS) is not started(used by break/restore) */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_NOT_IN_WP_MIS = 0x10025,   /*!< not in the waypoint mission(MIS)(cannot pause/recovery or break/restore) */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_MIS_HAS_BEEN_PAUSED = 0x10026,   /*!< the current status is paused, cannot command pause again */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_MIS_NOT_PAUSED = 0x10027,   /*!< not in paused status, cannot command recovery */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_MIS_HAS_BEEN_BROKEN = 0x10028,   /*!< the current status is broken, cannot command break again */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_MIS_NOT_BROKEN = 0x10029,   /*!< not in break status, cannot command restore */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_PAUSE_RECOVERY_NOT_SUPPORTED = 0x1002a,   /*!< the configuration forbid using pause/recovery API */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_BREAK_RESTORE_NOT_SUPPORTED = 0x1002b,   /*!< the configuration forbid using break/restore API */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_NO_BREAK_POINT = 0x1002c,   /*!< no break point is recorded for restore */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_NO_CUR_TRAJ_PROJECT = 0x1002d,   /*!< no current trajectory project point is recorded for restore */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_NO_NXT_TRAJ_PROJECT = 0x1002e,   /*!< no next trajectory project point is recorded for restore */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_NO_NNT_TRAJ_PROJECT = 0x1002f,   /*!< no next the next trajectory project point is recorded for restore */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_UPLOAD_WP_ID_NOT_CONTINUE = 0x10030,   /*!< the index of upload wp is not continue after the store wp */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_WP_LINE_ENTER_NOT_SET_TO_START_WP = 0x10031,   /*!< the WP_LINE_ENTER wp_type set to a wp which is not the init start waypoint */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_INIT_WP_WHEN_PLAN_HAS_STARTED = 0x10032,   /*!< the waypoint plan has started when initializing waypoint */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_DAMPING_DIS_EXCEED_RANGE = 0x10033,   /*!< waypoint damping distance exceed set range */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_WAYPOINT_COOR_EXCEED_RANGE = 0x10034,   /*!< waypoint position coordinate exceed rational range */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_FIRST_WP_TYPE_IS_WP_TURN_NO = 0x10035,   /*!< first waypoint type error, it can not be WP_TURN_NO */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_WP_EXCEED_RADIUS_LIMIT = 0x10038,   /*!< waypoint position exceed radius limit */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_WP_EXCEED_HEIGHT_LIMIT = 0x10039,   /*!< waypoint position exceed height limit */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_UPLOADED_WP_EXCEED_HEIGHT_LIMIT = 0x10040,   /*!< the uploaded waypoint exceed height limit */

    /* The enum that defines the detail error code of flight-status module */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_STATUS_RESV = 0x20000,
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_STATUS_WP_MIS_CHECK_FAIL = 0x20001,   /*!< head_node is null or atti_not_healthy or gyro_not_healthy or horiz_vel_not healthy or horiz_abs_pos_not_healthy. */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_STATUS_HOME_NOT_RECORDED = 0x20002,   /*!< the home point is no recorded yet, which will be executed at the first time of GPS level > 3(MR/FW). */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_STATUS_LOW_LOCATION_ACCURACY = 0x20003,   /*!< current location accuracy is low for bad GPS signal. */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_STATUS_RTK_CONDITION_IS_NOT_READY = 0x20005,   /*!< use rtk_data, but rtk is not connected or rtk_data is invalid */

    /* The enum that defines the detail error code of flight-secure module */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_SECURE_RESV = 0x30000,
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_SECURE_CROSS_NFZ = 0x30001,   /*!< the trajectory cross the NFZ */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_SECURE_BAT_LOW = 0x30002,   /*!< current capacity of smart battery or voltage of non-smart battery is lower than level 1 or level 2 threshold */

    /* The enum that defines the detail error code of action-common module */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTION_COMMON_RESV = 0x400000,
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTION_COMMON_ACTION_ID_DUPLICATED = 0x400001,   /*!< the ID of Action is duplicated. */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTION_COMMON_ACTION_ITEMS_SPACE_NOT_ENOUGH = 0x400002,   /*!< there is no enough memory space for new Action Item */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTION_COMMON_ACTION_SIZE_GT_BUF_SIZE = 0x400003,   /*!< the size of buffer used to get the info of Action is less than the size of Action. Normally users can not get this. */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTION_COMMON_ACTION_ID_NOT_FOUND = 0x400004,   /*!< the ID of Action is not found. */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTION_COMMON_DOWNLOAD_ACTION_ID_RANGE_ERROR = 0x400005,   /*!< the download action start id is bigger than the action end id */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTION_COMMON_NO_ACTION_ITEMS_STORED = 0x400006,   /*!< can not download or get min-max action ID for no action items stored in action kernel */

    /* The enum that defines the detail error code of trigger module */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRIGGER_RESV = 0x410000,
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRIGGER_TYPE_INVALID = 0x410001,   /*!< the type ID of Trigger is invalid. It might not defined or the information is empty. */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRIGGER_REACH_WP_END_INDEX_LT_START_INDEX = 0x410021,   /*!< wp_end_index is less than wp_start_index in reach_waypoint_trigger. */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRIGGER_REACH_WP_INVALID_INTERVAL_WP_NUM = 0x410022,   /*!< interval_wp_num is large than the difference of wp_start_index and wp_end_index in reach_waypoint_trigger. */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRIGGER_REACH_WP_INVALID_AUTO_TERMINATE_WP_NUM = 0x410023,   /*!< auto_terminate_wp_num is large than interval_wp_num in reach_waypoint_trigger. */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRIGGER_ASSOCIATE_INVALID_TYPE = 0x410041,   /*!< the associate_type is greater than the maximum value.  */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRIGGER_SIMPLE_INTERVAL_INVALID_TYPE = 0x410081,   /*!< the interval type is greater than the maximum value. */

    /* The enum that defines the detail error code of actuator-common module */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_COMMON_RESV = 0x420000,
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_COMMON_ACTUATOR_EXEC_NON_SUPPORTED = 0x420001,   /*!< the execution of Actuator is not supported, e.g., try to stop camera shooting. */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_COMMON_ACTUATOR_TYPE_INVALID = 0x420002,   /*!< the type ID of Actuator is invalid. It might not defined or the information is empty. */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_COMMON_ACTUATOR_FUNC_INVALID = 0x420003,   /*!< the Function ID of Actuator is invalid. It might not defined or the information is empty. */

    /* The enum that defines the detail error code of action-camera module */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_CAMERA_RESV = 0x430000,
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_CAMERA_SEND_SINGLE_SHOT_CMD_TO_CAMERA_FAIL = 0x430001,   /*!< fail to send shot cmd to camera for no camera or camera is busy. */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_CAMERA_SEND_VIDEO_START_CMD_TO_CAMERA_FAIL = 0x430002,    /*!< fail to send video start cmd to camera for no camera or camera is busy. */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_CAMERA_SEND_VIDEO_STOP_CMD_TO_CAMERA_FAIL = 0x430003,    /*!< fail to send video stop cmd to camera for no camera or camera is not busy. */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_CAMERA_FOCUS_PARAM_XY_INVALID = 0x430004,   /*!< camera focus param xy exceed valid range (0, 1). */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_CAMERA_SEND_FOCUS_CMD_TO_CAMERA_FAIL = 0x430005,   /*!< fail to send focus cmd to camera for no camera or camera is busy. */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_CAMERA_SEND_FOCALIZE_CMD_TO_CAMERA_FAIL = 0x430006,   /*!< fail to send focalize cmd to camera for no camera or camera is busy. */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_CAMERA_FOCAL_DISTANCE_INVALID = 0x430007,   /*!< focal distance of camera focalize function exceed valid range. */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_CAMERA_EXEC_FAIL = 0x430100,   /*!< this err code indicate camera fail to exec coressponding cmd, and the low 8 bit
                                                                                  will be replaced by the return code from camera, for example: 0x01E0 means current cmd
                                                                                  is not supported, 0x01E8 means SD card is not inserted and so on, the detailed camera
                                                                                  return code could be found in camera protocal. */

    /* The enum that defines the detail error code of action-gimbal module */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_GIMBAL_RESV = 0x440000,
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_GIMBAL_INVALID_RPY_ANGLE_CTRL_CMD = 0x440001,   /*!< gimbal roll/pitch/yaw angle ctrl cmd param invalid, unable to exec. */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_GIMBAL_INVALID_DURATION_CMD = 0x440002,   /*!< gimbal duration param invalid, unable to exec. */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_GIMBAL_FAIL_TO_ARRIVE_TGT_ANGLE = 0x440003,   /*!< gimbal fail to arrive target angle . */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_GIMBAL_FAIL_TO_SEND_CMD_TO_GIMBAL = 0x440004,   /*!< fail to send cmd to gimbal for gimbal is busy or no gimbal. */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_GIMBAL_THIS_INDEX_OF_GIMBAL_NOT_DOING_UNIFORM_CTRL = 0x440005, /*!< fail to stop gimbal uniform ctrl because index error.*/

    /* The enum that defines the detail error code of action-flight module */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_FLIGHT_RESV = 0x460000,
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_FLIGHT_YAW_INVALID_YAW_ANGLE = 0x460001,   /*!< yaw angle is lager max yaw angle. */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_FLIGHT_YAW_TO_TGT_ANGLE_TIMEOUT = 0x460002,   /*!< faile to target yaw angle, because of timeout.*/
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_FLIGHT_ACTION_YAW_OCCUPIED = 0x460003,
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_FLIGHT_CUR_AND_TGT_VEL_CLE_STATUE_EQUAL = 0x460004,

    /* The enum that defines the detail error code of payload module */
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_PAYLOAD_RESV = 0x470000,
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_PAYLOAD_FAIL_TO_SEND_CMD_TO_PAYLOAD = 0x470001,
    DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_PAYLOAD_EXEC_FAILED = 0x470002,
} E_DjiErrorWaypointV2ModuleRawCode;

//@formatter:off
/**
 * @brief DJI error code complete works. Users can search all error messages here.
 * @details Please get error description, error reason and error recovery suggestion of every error code from
 * ::DJI_ERROR_OBJECTS macro.
 */
enum DjiErrorCode {
    /* system module error code, including some common error code */
    DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS = DJI_ERROR_CODE(DJI_ERROR_MODULE_SYSTEM, DJI_ERROR_SYSTEM_MODULE_RAW_CODE_SUCCESS),
    DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_REQUEST_PARAMETER = DJI_ERROR_CODE(DJI_ERROR_MODULE_SYSTEM, DJI_ERROR_SYSTEM_MODULE_RAW_CODE_INVALID_REQUEST_PARAMETER),
    DJI_ERROR_SYSTEM_MODULE_CODE_EXECUTING_HIGHER_PRIORITY_TASK = DJI_ERROR_CODE(DJI_ERROR_MODULE_SYSTEM, DJI_ERROR_SYSTEM_MODULE_RAW_CODE_EXECUTING_HIGHER_PRIORITY_TASK),
    DJI_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT = DJI_ERROR_CODE(DJI_ERROR_MODULE_SYSTEM, DJI_ERROR_SYSTEM_MODULE_RAW_CODE_NONSUPPORT),
    DJI_ERROR_SYSTEM_MODULE_CODE_TIMEOUT = DJI_ERROR_CODE(DJI_ERROR_MODULE_SYSTEM, DJI_ERROR_SYSTEM_MODULE_RAW_CODE_TIMEOUT),
    DJI_ERROR_SYSTEM_MODULE_CODE_MEMORY_ALLOC_FAILED = DJI_ERROR_CODE(DJI_ERROR_MODULE_SYSTEM, DJI_ERROR_SYSTEM_MODULE_RAW_CODE_MEMORY_ALLOC_FAILED),
    DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER = DJI_ERROR_CODE(DJI_ERROR_MODULE_SYSTEM, DJI_ERROR_SYSTEM_MODULE_RAW_CODE_INVALID_PARAMETER),
    DJI_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT_IN_CURRENT_STATE = DJI_ERROR_CODE(DJI_ERROR_MODULE_SYSTEM, DJI_ERROR_SYSTEM_MODULE_RAW_CODE_NONSUPPORT_IN_CURRENT_STATE),
    DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_SYSTEM, DJI_ERROR_SYSTEM_MODULE_RAW_CODE_SYSTEM_ERROR),
    DJI_ERROR_SYSTEM_MODULE_CODE_HARDWARE_ERR = DJI_ERROR_CODE(DJI_ERROR_MODULE_SYSTEM, DJI_ERROR_SYSTEM_MODULE_RAW_CODE_HARDWARE_ERR),
    DJI_ERROR_SYSTEM_MODULE_CODE_INSUFFICIENT_ELECTRICITY = DJI_ERROR_CODE(DJI_ERROR_MODULE_SYSTEM, DJI_ERROR_SYSTEM_MODULE_RAW_CODE_INSUFFICIENT_ELECTRICITY),
    DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN = DJI_ERROR_CODE(DJI_ERROR_MODULE_SYSTEM, DJI_ERROR_SYSTEM_MODULE_RAW_CODE_UNKNOWN),
    DJI_ERROR_SYSTEM_MODULE_CODE_NOT_FOUND = DJI_ERROR_CODE(DJI_ERROR_MODULE_SYSTEM, DJI_ERROR_SYSTEM_MODULE_RAW_CODE_NOT_FOUND),
    DJI_ERROR_SYSTEM_MODULE_CODE_OUT_OF_RANGE = DJI_ERROR_CODE(DJI_ERROR_MODULE_SYSTEM, DJI_ERROR_SYSTEM_MODULE_RAW_CODE_OUT_OF_RANGE),
    DJI_ERROR_SYSTEM_MODULE_CODE_BUSY = DJI_ERROR_CODE(DJI_ERROR_MODULE_SYSTEM, DJI_ERROR_SYSTEM_MODULE_RAW_CODE_BUSY),
    DJI_ERROR_SYSTEM_MODULE_CODE_DUPLICATE = DJI_ERROR_CODE(DJI_ERROR_MODULE_SYSTEM, DJI_ERROR_SYSTEM_MODULE_RAW_CODE_DUPLICATE),
    DJI_ERROR_SYSTEM_MODULE_CODE_ADAPTER_NOT_MATCH = DJI_ERROR_CODE(DJI_ERROR_MODULE_SYSTEM, DJI_ERROR_SYSTEM_MODULE_RAW_CODE_ADAPTER_NOT_MATCH),

    /* gimbal module error code */
    DJI_ERROR_GIMBAL_MODULE_CODE_PITCH_REACH_POSITIVE_LIMIT = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL, DJI_ERROR_GIMBAL_MODULE_RAW_CODE_PITCH_REACH_POSITIVE_LIMIT),
    DJI_ERROR_GIMBAL_MODULE_CODE_PITCH_REACH_NEGATIVE_LIMIT = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL, DJI_ERROR_GIMBAL_MODULE_RAW_CODE_PITCH_REACH_NEGATIVE_LIMIT),
    DJI_ERROR_GIMBAL_MODULE_CODE_ROLL_REACH_POSITIVE_LIMIT = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL, DJI_ERROR_GIMBAL_MODULE_RAW_CODE_ROLL_REACH_POSITIVE_LIMIT),
    DJI_ERROR_GIMBAL_MODULE_CODE_ROLL_REACH_NEGATIVE_LIMIT = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL, DJI_ERROR_GIMBAL_MODULE_RAW_CODE_ROLL_REACH_NEGATIVE_LIMIT),
    DJI_ERROR_GIMBAL_MODULE_CODE_YAW_REACH_POSITIVE_LIMIT = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL, DJI_ERROR_GIMBAL_MODULE_RAW_CODE_YAW_REACH_POSITIVE_LIMIT),
    DJI_ERROR_GIMBAL_MODULE_CODE_YAW_REACH_NEGATIVE_LIMIT = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL, DJI_ERROR_GIMBAL_MODULE_RAW_CODE_YAW_REACH_NEGATIVE_LIMIT),
    DJI_ERROR_GIMBAL_MODULE_CODE_NON_CONTROL_AUTHORITY = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL, DJI_ERROR_GIMBAL_MODULE_RAW_CODE_NON_CONTROL_AUTHORITY),

    /* payload collaboration module error code */
    DJI_ERROR_PAYLOAD_COLLABORATION_MODULE_CODE_POSITION_NOT_MATCH = DJI_ERROR_CODE(DJI_ERROR_MODULE_PAYLOAD_COLLABORATION, DJI_ERROR_PAYLOAD_COLLABORATION_MODULE_RAW_CODE_POSITION_NOT_MATCH),

    /* activation error code of identity module */
    DJI_ERROR_IDENTITY_MODULE_CODE_ACTIVATE_PARAMETER_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_IDENTITY, DJI_ERROR_IDENTITY_MODULE_RAW_CODE_ACTIVATE_PARAMETER_ERROR),
    DJI_ERROR_IDENTITY_MODULE_CODE_ACTIVATE_ENCODE_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_IDENTITY, DJI_ERROR_IDENTITY_MODULE_RAW_CODE_ACTIVATE_ENCODE_ERROR),
    DJI_ERROR_IDENTITY_MODULE_CODE_ACTIVATE_NEW_DEVICE_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_IDENTITY, DJI_ERROR_IDENTITY_MODULE_RAW_CODE_ACTIVATE_NEW_DEVICE_ERROR),
    DJI_ERROR_IDENTITY_MODULE_CODE_ACTIVATE_SOFTWARE_NOT_CONNECTED = DJI_ERROR_CODE(DJI_ERROR_MODULE_IDENTITY, DJI_ERROR_IDENTITY_MODULE_RAW_CODE_ACTIVATE_SOFTWARE_NOT_CONNECTED),
    DJI_ERROR_IDENTITY_MODULE_CODE_ACTIVATE_NETWORK_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_IDENTITY, DJI_ERROR_IDENTITY_MODULE_RAW_CODE_ACTIVATE_NETWORK_ERROR),
    DJI_ERROR_IDENTITY_MODULE_CODE_ACTIVATE_SERVER_ACCESS_REFUSED = DJI_ERROR_CODE(DJI_ERROR_MODULE_IDENTITY, DJI_ERROR_IDENTITY_MODULE_RAW_CODE_ACTIVATE_SERVER_ACCESS_REFUSED),
    DJI_ERROR_IDENTITY_MODULE_CODE_ACTIVATE_ACCESS_LEVEL_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_IDENTITY, DJI_ERROR_IDENTITY_MODULE_RAW_CODE_ACTIVATE_ACCESS_LEVEL_ERROR),
    DJI_ERROR_IDENTITY_MODULE_CODE_ACTIVATE_OSDK_VERSION_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_IDENTITY, DJI_ERROR_IDENTITY_MODULE_RAW_CODE_ACTIVATE_OSDK_VERSION_ERROR),

    /* subscription module error code */
    DJI_ERROR_SUBSCRIPTION_MODULE_CODE_INVALID_TOPIC_FREQ = DJI_ERROR_CODE(DJI_ERROR_MODULE_SUBSCRIPTION, DJI_ERROR_SUBSCRIPTION_MODULE_RAW_CODE_INVALID_TOPIC_FREQ),
    DJI_ERROR_SUBSCRIPTION_MODULE_CODE_TOPIC_DUPLICATE = DJI_ERROR_CODE(DJI_ERROR_MODULE_SUBSCRIPTION, DJI_ERROR_SUBSCRIPTION_MODULE_RAW_CODE_TOPIC_DUPLICATE),
    DJI_ERROR_SUBSCRIPTION_MODULE_CODE_TOPIC_NOT_SUBSCRIBED = DJI_ERROR_CODE(DJI_ERROR_MODULE_SUBSCRIPTION, DJI_ERROR_SUBSCRIPTION_MODULE_RAW_CODE_TOPIC_NOT_SUBSCRIBED),
    DJI_ERROR_SUBSCRIPTION_MODULE_CODE_TIMESTAMP_NOT_ENABLE = DJI_ERROR_CODE(DJI_ERROR_MODULE_SUBSCRIPTION, DJI_ERROR_SUBSCRIPTION_MODULE_RAW_CODE_TIMESTAMP_NOT_ENABLE),
    DJI_ERROR_SUBSCRIPTION_MODULE_CODE_TOPIC_NOT_SUPPORTED = DJI_ERROR_CODE(DJI_ERROR_MODULE_SUBSCRIPTION, DJI_ERROR_SUBSCRIPTION_MODULE_RAW_CODE_TOPIC_NOT_SUPPORTED),

    /* mop channel module error code */
    DJI_ERROR_MOP_CHANNEL_MODULE_CODE_CONNECTION_CLOSE = DJI_ERROR_CODE(DJI_ERROR_MODULE_MOP_CHANNEL, DJI_ERROR_MOP_CHANNEL_MODULE_RAW_CODE_CONNECTION_CLOSE),

    /* Flight controller basic errors */
    DJI_ERROR_FC_MODULE_CODE_RC_MODE_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_BASIC, DJI_ERROR_FC_MODULE_RAW_CODE_RC_MODE_ERROR),
    DJI_ERROR_FC_MODULE_CODE_RELEASE_CONTROL_SUCCESS= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_BASIC, DJI_ERROR_FC_MODULE_RAW_CODE_RELEASE_CONTROL_SUCCESS),
    DJI_ERROR_FC_MODULE_CODE_OBTAIN_CONTROL_SUCCESS= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_BASIC, DJI_ERROR_FC_MODULE_RAW_CODE_OBTAIN_CONTROL_SUCCESS),
    DJI_ERROR_FC_MODULE_CODE_OBTAIN_CONTROL_IN_PROGRESS= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_BASIC, DJI_ERROR_FC_MODULE_RAW_CODE_OBTAIN_CONTROL_IN_PROGRESS),
    DJI_ERROR_FC_MODULE_CODE_RELEASE_CONTROL_IN_PROGRESS= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_BASIC, DJI_ERROR_FC_MODULE_RAW_CODE_RELEASE_CONTROL_IN_PROGRESS),
    DJI_ERROR_FC_MODULE_CODE_RC_NEED_MODE_P= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_BASIC, DJI_ERROR_FC_MODULE_RAW_CODE_RC_NEED_MODE_P),
    DJI_ERROR_FC_MODULE_CODE_RC_NEED_MODE_F= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_BASIC, DJI_ERROR_FC_MODULE_RAW_CODE_RC_NEED_MODE_F),
    DJI_ERROR_FC_MODULE_CODE_PARAM_READ_WRITE_INVALID_PARAM= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_BASIC, DJI_ERROR_FC_MODULE_RAW_CODE_PARAM_READ_WRITE_INVALID_PARAM),
    DJI_ERROR_FC_MODULE_CODE_IOC_OBTAIN_CONTROL_ERROR= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_BASIC, DJI_ERROR_FC_MODULE_RAW_CODE_IOC_OBTAIN_CONTROL_ERROR),
    DJI_ERROR_FC_MODULE_CODE_KEY_ERROR= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_BASIC, DJI_ERROR_FC_MODULE_RAW_CODE_KEY_ERROR),
    DJI_ERROR_FC_MODULE_CODE_NO_AUTHORIZATION_ERROR= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_BASIC, DJI_ERROR_FC_MODULE_RAW_CODE_NO_AUTHORIZATION_ERROR),
    DJI_ERROR_FC_MODULE_CODE_NO_RIGHTS_ERROR= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_BASIC, DJI_ERROR_FC_MODULE_RAW_CODE_NO_RIGHTS_ERROR),
    DJI_ERROR_FC_MODULE_CODE_SYSTEM_ERROR= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_BASIC, DJI_ERROR_FC_MODULE_RAW_CODE_SYSTEM_ERROR),

    /* Flight controller joystick errors */
    DJI_ERROR_FC_JOYSTICK_MODULE_OBTAIN_RELEASE_JOYSTICK_AUTH_SUCCESS = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_JOYSTICK, DJI_ERROR_FC_JOYSTICK_MODULE_RAW_CODE_SUCCESS),
    DJI_ERROR_FC_JOYSTICK_MODULE_DEVICE_NOT_ALLOW= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_JOYSTICK, DJI_ERROR_FC_JOYSTICK_MODULE_RAW_CODE_DEVICE_NOT_ALLOW),
    DJI_ERROR_FC_JOYSTICK_MODULE_TAKING_OFF= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_JOYSTICK, DJI_ERROR_FC_JOYSTICK_MODULE_RAW_CODE_TAKING_OFF),
    DJI_ERROR_FC_JOYSTICK_MODULE_LANDING= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_JOYSTICK, DJI_ERROR_FC_JOYSTICK_MODULE_RAW_CODE_LANDING),
    DJI_ERROR_FC_JOYSTICK_MODULE_CMD_INVALID = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_JOYSTICK, DJI_ERROR_FC_JOYSTICK_MODULE_RAW_CODE_CMD_INVALID),
    DJI_ERROR_FC_JOYSTICK_MODULE_RC_NOT_P_MODE = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_JOYSTICK, DJI_ERROR_FC_JOYSTICK_MODULE_RAW_CODE_RC_NOT_P_MODE),
    DJI_ERROR_FC_JOYSTICK_MODULE_CMD_LENGTH_ERROR= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_JOYSTICK, DJI_ERROR_FC_JOYSTICK_MODULE_RAW_CODE_CMD_LENGTH_ERROR),
    DJI_ERROR_FC_JOYSTICK_MODULE_HAS_NO_JOYSTICK_AUTHORITY= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_JOYSTICK, DJI_ERROR_FC_JOYSTICK_MODULE_RAW_CODE_HAS_NO_JOYSTICK_AUTHORITY),
    DJI_ERROR_FC_JOYSTICK_MODULE_IN_RC_LOST_ACTION= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_JOYSTICK, DJI_ERROR_FC_JOYSTICK_MODULE_RAW_CODE_IN_RC_LOST_ACTION),

    /* Flight controller action errors */
    DJI_ERROR_FC_ACTION_MODULE_TRIGGER_ERROR_MOTOR_ON= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_ACTION, DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_TRIGGER_ERROR_MOTOR_ON),
    DJI_ERROR_FC_ACTION_MODULE_TRIGGER_ERROR_MOTOR_OFF= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_ACTION, DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_TRIGGER_ERROR_MOTOR_OFF),
    DJI_ERROR_FC_ACTION_MODULE_TRIGGER_ERROR_IN_AIR= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_ACTION, DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_TRIGGER_ERROR_IN_AIR),
    DJI_ERROR_FC_ACTION_MODULE_TRIGGER_ERROR_NOT_IN_AIR= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_ACTION, DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_TRIGGER_ERROR_NOT_IN_AIR),
    DJI_ERROR_FC_ACTION_MODULE_TRIGGER_ERROR_HOME_POINT_NOT_SET= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_ACTION, DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_TRIGGER_ERROR_HOME_POINT_NOT_SET),
    DJI_ERROR_FC_ACTION_MODULE_TRIGGER_ERROR_BAD_GPS= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_ACTION, DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_TRIGGER_ERROR_BAD_GPS),
    DJI_ERROR_FC_ACTION_MODULE_TRIGGER_ERROR_IN_SIMULATION= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_ACTION, DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_TRIGGER_ERROR_IN_SIMULATION),
    DJI_ERROR_FC_ACTION_MODULE_TRIGGER_ERROR_CANNOT_START_MOTOR= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_ACTION, DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_TRIGGER_ERROR_CANNOT_START_MOTOR),
    DJI_ERROR_FC_ACTION_MODULE_TRIGGER_ERROR_LOW_VOLTAGE= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_ACTION, DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_TRIGGER_ERROR_LOW_VOLTAGE),
    DJI_ERROR_FC_ACTION_MODULE_TRIGGER_ERROR_SPEED_TOO_LARGE= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_ACTION, DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_TRIGGER_ERROR_SPEED_TOO_LARGE),

    /* Flight controller arrest flying errors */
    DJI_ERROR_FC_ARREST_FLYING_MODULE_REGISTER_LOGOUT_SET_SUCCESS = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_ARREST_FLYING, DJI_ERROR_FC_ARREST_FLYING_MODULE_RAW_CODE_REGISTER_LOGOUT_SET_SUCCESS),
    DJI_ERROR_FC_ARREST_FLYING_MODULE_REGISTER_LOGOUT_NULL_POINTER= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_ARREST_FLYING, DJI_ERROR_FC_ARREST_FLYING_MODULE_RAW_CODE_REGISTER_LOGOUT_NULL_POINTER),
    DJI_ERROR_FC_ARREST_FLYING_MODULE_HMS_CODE_HAS_REGISTED= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_ARREST_FLYING, DJI_ERROR_FC_ARREST_FLYING_MODULE_RAW_CODE_HMS_CODE_HAS_REGISTED),
    DJI_ERROR_FC_ARREST_FLYING_MODULE_HMS_CODE_NOT_FIND = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_ARREST_FLYING, DJI_ERROR_FC_ARREST_FLYING_MODULE_RAW_CODE_HMS_CODE_NOT_FIND),
    DJI_ERROR_FC_ARREST_FLYING_MODULE_ADD_ITEM_NO_DECRIPTION = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_ARREST_FLYING, DJI_ERROR_FC_ARREST_FLYING_MODULE_RAW_CODE_ADD_ITEM_NO_DECRIPTION),
    DJI_ERROR_FC_ARREST_FLYING_MODULE_REGISTER_ID_INVALID= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_ARREST_FLYING, DJI_ERROR_FC_ARREST_FLYING_MODULE_RAW_CODE_REGISTER_ID_INVALID),
    DJI_ERROR_FC_ARREST_FLYING_MODULE_STOP_IN_AIR_HMSCODE_NOT_IN_WHITE_TABLE= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_ARREST_FLYING, DJI_ERROR_FC_ARREST_FLYING_MODULE_RAW_CODE_STOP_IN_AIR_HMSCODE_NOT_IN_WHITE_TABLE),
    DJI_ERROR_FC_ARREST_FLYING_MODULE_INVALID_FORMAT_HMSCODE= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_ARREST_FLYING, DJI_ERROR_FC_ARREST_FLYING_MODULE_RAW_CODE_INVALID_FORMAT_HMSCODE),
    DJI_ERROR_FC_ARREST_FLYING_MODULE_HMSCODE_NOT_IN_WHITE_TABLE= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_ARREST_FLYING, DJI_ERROR_FC_ARREST_FLYING_MODULE_RAW_CODE_HMSCODE_NOT_IN_WHITE_TABLE),

    /* Flight controller home location errors */
    DJI_ERROR_FC_HOME_LOCATION_MODULE_UNKNOWN_FAILED_REASON= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_HOME_LOCATION, DJI_ERROR_FC_HOME_LOCATION_MODULE_RAW_CODE_UNKNOWN_FAILED_REASON),
    DJI_ERROR_FC_HOME_LOCATION_MODULE_INVALID_GPS_COORDINATE= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_HOME_LOCATION, DJI_ERROR_FC_HOME_LOCATION_MODULE_RAW_CODE_INVALID_GPS_COORDINATE),
    DJI_ERROR_FC_HOME_LOCATION_MODULE_NOT_BE_RECORD = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_HOME_LOCATION, DJI_ERROR_FC_HOME_LOCATION_MODULE_RAW_CODE_NOT_BE_RECORD),
    DJI_ERROR_FC_HOME_LOCATION_MODULE_GPS_NOT_READY = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_HOME_LOCATION, DJI_ERROR_FC_HOME_LOCATION_MODULE_RAW_CODE_GPS_NOT_READY),
    DJI_ERROR_FC_HOME_LOCATION_MODULE_DIS_TOO_FAR = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_HOME_LOCATION, DJI_ERROR_FC_HOME_LOCATION_MODULE_RAW_CODE_DIS_TOO_FAR),

    /* Flight controller emergency stop motor errors */
    DJI_ERROR_FC_EMERGENCY_STOP_MOTOR_MODULE_VERSION_NOT_MATCH= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_EMERGENCY_STOP_MOTOR, DJI_ERROR_FC_EMERGENCY_STOP_MOTOR_MODULE_RAW_CODE_VERSION_NOT_MATCH),
    DJI_ERROR_FC_EMERGENCY_STOP_MOTOR_MODULE_CMD_INVALID= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_EMERGENCY_STOP_MOTOR, DJI_ERROR_FC_EMERGENCY_STOP_MOTOR_MODULE_RAW_CODE_CMD_INVALID),

    /* Camera manager errors */
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_UNSUPPORTED_COMMAND),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_TIMEOUT = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_TIMEOUT ),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_RAM_ALLOCATION_FAILED = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_RAM_ALLOCATION_FAILED),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_INVALID_COMMAND_PARAMETER = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_INVALID_COMMAND_PARAMETER),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND_IN_CUR_STATE = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_UNSUPPORTED_COMMAND_IN_CUR_STATE),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_CAMERA_TIME_NOT_SYNCHRONIZED = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_CAMERA_TIME_NOT_SYNCHRONIZED),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_PARAMETER_SET_FAILED = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_PARAMETER_SET_FAILED),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_PARAMETER_GET_FAILED = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_PARAMETER_GET_FAILED),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_SD_CARD_MISSING = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_SD_CARD_MISSING),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_SD_CARD_FULL = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_SD_CARD_FULL),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_SD_CARD_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_SD_CARD_ERROR),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_SENSOR_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_SENSOR_ERROR),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_SYSTEM_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_SYSTEM_ERROR),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_PARAMETER_TOTAL_TOO_LONG = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_PARAMETER_TOTAL_TOO_LONG),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_MODULE_INACTIVATED = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_MODULE_INACTIVATED),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_FIRMWARE_DATA_NUM_DISCONTINUOUS = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_FIRMWARE_DATA_NUM_DISCONTINUOUS),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_FIRMWARE_VERIFICATION_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_FIRMWARE_VERIFICATION_ERROR),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_FLASH_WRITE_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_FLASH_WRITE_ERROR),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_FIRMWARE_TYPE_MISMATCH = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_FIRMWARE_TYPE_MISMATCH),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_REMOTE_CONTROL_UNCONNECTED = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_REMOTE_CONTROL_UNCONNECTED),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_HARDWARE_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_HARDWARE_ERROR),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_AIRCRAFT_UNCONNECTED = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_AIRCRAFT_UNCONNECTED),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_CANNOT_UPGRADE_IN_CUR_STATE = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_CANNOT_UPGRADE_IN_CUR_STATE),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNDEFINE_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_UNDEFINE_ERROR),

    /* Gimbal manager errors */
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_UNSUPPORTED_COMMAND),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_TIMEOUT = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_TIMEOUT ),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_RAM_ALLOCATION_FAILED = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_RAM_ALLOCATION_FAILED),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_INVALID_COMMAND_PARAMETER = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_INVALID_COMMAND_PARAMETER),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND_IN_CUR_STATE = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_UNSUPPORTED_COMMAND_IN_CUR_STATE),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_CAMERA_TIME_NOT_SYNCHRONIZED = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_CAMERA_TIME_NOT_SYNCHRONIZED),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_PARAMETER_SET_FAILED = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_PARAMETER_SET_FAILED),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_PARAMETER_GET_FAILED = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_PARAMETER_GET_FAILED),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_SD_CARD_MISSING = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_SD_CARD_MISSING),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_SD_CARD_FULL = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_SD_CARD_FULL),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_SD_CARD_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_SD_CARD_ERROR),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_SENSOR_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_SENSOR_ERROR),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_SYSTEM_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_SYSTEM_ERROR),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_PARAMETER_TOTAL_TOO_LONG = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_PARAMETER_TOTAL_TOO_LONG),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_MODULE_INACTIVATED = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_MODULE_INACTIVATED),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_FIRMWARE_DATA_NUM_DISCONTINUOUS = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_FIRMWARE_DATA_NUM_DISCONTINUOUS),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_FIRMWARE_VERIFICATION_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_FIRMWARE_VERIFICATION_ERROR),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_FLASH_WRITE_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_FLASH_WRITE_ERROR),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_FIRMWARE_TYPE_MISMATCH = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_FIRMWARE_TYPE_MISMATCH),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_REMOTE_CONTROL_UNCONNECTED = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_REMOTE_CONTROL_UNCONNECTED),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_HARDWARE_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_HARDWARE_ERROR),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_AIRCRAFT_UNCONNECTED = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_AIRCRAFT_UNCONNECTED),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_CANNOT_UPGRADE_IN_CUR_STATE = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_CANNOT_UPGRADE_IN_CUR_STATE),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_UNDEFINE_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_UNDEFINE_ERROR),

	/* Waypoint v2 total errors */
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_COMMON_SUCCESS = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_COMMON_SUCCESS),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_COMMON_INVALID_DATA_LENGTH = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_COMMON_INVALID_DATA_LENGTH),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_COMMON_INVALD_FLOAT_NUM = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_COMMON_INVALD_FLOAT_NUM),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_WP_VERSION_NO_MATCH = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_WP_VERSION_NO_MATCH),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_COMMON_UNKNOWN = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_COMMON_UNKNOWN),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_RESV = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_RESV),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_INIT_WP_NUM_TOO_MANY = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_INIT_WP_NUM_TOO_MANY),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_INIT_WP_NUM_TOO_FEW = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_INIT_WP_NUM_TOO_FEW  ),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_INIT_INVALID_END_INDEX = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_INIT_INVALID_END_INDEX ),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_UPLOAD_START_ID_GT_END_ID = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_UPLOAD_START_ID_GT_END_ID),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_UPLOAD_END_ID_GT_TOTAL_NUM = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_UPLOAD_END_ID_GT_TOTAL_NUM),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_DOWNLOAD_WPS_NOT_IN_STORED_RAGNE = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_DOWNLOAD_WPS_NOT_IN_STORED_RAGNE),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_CUR_POS_IS_FAR_AWAY_FROM_FIRST_WP = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_CUR_POS_IS_FAR_AWAY_FROM_FIRST_WP),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_ADJ_WPS_TOO_CLOSE = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_ADJ_WPS_TOO_CLOSE ),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_ADJ_WPS_TOO_FAR = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_ADJ_WPS_TOO_FAR  ),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_UPLOAD_MAX_VEL_GT_GLOBAL = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_UPLOAD_MAX_VEL_GT_GLOBAL ),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_UPLOAD_LOCAL_CRUISE_VEL_GT_LOCAL_MAX = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_UPLOAD_LOCAL_CRUISE_VEL_GT_LOCAL_MAX),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_UPLOAD_LOCAL_CRUISE_VEL_GT_GLOBAL_MAX = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_UPLOAD_LOCAL_CRUISE_VEL_GT_GLOBAL_MAX),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_INIT_INVALID_GLOBAL_MAX_VEL = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_INIT_INVALID_GLOBAL_MAX_VEL  ),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_GLOBAL_CRUISE_VEL_GT_MAX_VEL = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_GLOBAL_CRUISE_VEL_GT_MAX_VEL ),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_INIT_INVALID_GOTO_FIRST_FLAG = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_INIT_INVALID_GOTO_FIRST_FLAG ),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_INIT_INVALID_FINISHED_ACTION = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_INIT_INVALID_FINISHED_ACTION ),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_INIT_INVALID_RC_LOST_ACTION = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_INIT_INVALID_RC_LOST_ACTION),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_UPLOAD_YAW_MODE_INVALID = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_UPLOAD_YAW_MODE_INVALID),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_UPLOAD_YAW_CMD_NOT_IN_RANGE = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_UPLOAD_YAW_CMD_NOT_IN_RANGE),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_UPLOAD_YAW_TURN_DIRECTION_INVALID = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_UPLOAD_YAW_TURN_DIRECTION_INVALID),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_UPLOAD_WP_TYPE_INVALID = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_UPLOAD_WP_TYPE_INVALID),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_GO_STOP_CMD_INVALID = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_GO_STOP_CMD_INVALID),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_INVALID_PAUSE_RECOVERY_CMD = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_INVALID_PAUSE_RECOVERY_CMD),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_INVALID_BREAK_RESTORE_CMD = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_INVALID_BREAK_RESTORE_CMD),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_INIT_INVALID_REF_POINT = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_INIT_INVALID_REF_POINT),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_DAMPING_DIS_GE_DIS_OF_ADJ_POINTS = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_DAMPING_DIS_GE_DIS_OF_ADJ_POINTS),
    DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_UPLOAD_CANNT_SET_WP_LINE_EXIT_TYPE = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_UPLOAD_CANNT_SET_WP_LINE_EXIT_TYPE),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_INIT_INFO_NOT_UPLOADED = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_INIT_INFO_NOT_UPLOADED),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_WP_HAS_NOT_UPLOADED = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_WP_HAS_NOT_UPLOADED),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_UPLOADED_WP_NOT_ENOUGH = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_UPLOADED_WP_NOT_ENOUGH),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_GS_HAS_STARTED = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_GS_HAS_STARTED),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_GS_NOT_RUNNING = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_GS_NOT_RUNNING),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_GS_NOT_RUNNING_FOR_PAUSE_RECOVERY = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_GS_NOT_RUNNING_FOR_PAUSE_RECOVERY),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_GS_NOT_RUNNING_FOR_BREAK_RESTORE = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_GS_NOT_RUNNING_FOR_BREAK_RESTORE),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_NOT_IN_WP_MIS = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_NOT_IN_WP_MIS ),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_MIS_HAS_BEEN_PAUSED = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_MIS_HAS_BEEN_PAUSED),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_MIS_NOT_PAUSED = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_MIS_NOT_PAUSED),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_MIS_HAS_BEEN_BROKEN = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_MIS_HAS_BEEN_BROKEN),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_MIS_NOT_BROKEN = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_MIS_NOT_BROKEN),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_PAUSE_RECOVERY_NOT_SUPPORTED = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_PAUSE_RECOVERY_NOT_SUPPORTED),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_BREAK_RESTORE_NOT_SUPPORTED = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_BREAK_RESTORE_NOT_SUPPORTED),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_NO_BREAK_POINT = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_NO_BREAK_POINT ),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_NO_CUR_TRAJ_PROJECT = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_NO_CUR_TRAJ_PROJECT),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_NO_NXT_TRAJ_PROJECT = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_NO_NXT_TRAJ_PROJECT),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_NO_NNT_TRAJ_PROJECT = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_NO_NNT_TRAJ_PROJECT),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_UPLOAD_WP_ID_NOT_CONTINUE = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_UPLOAD_WP_ID_NOT_CONTINUE),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_WP_LINE_ENTER_NOT_SET_TO_START_WP = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_WP_LINE_ENTER_NOT_SET_TO_START_WP ),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_INIT_WP_WHEN_PLAN_HAS_STARTED = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_INIT_WP_WHEN_PLAN_HAS_STARTED ),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_DAMPING_DIS_EXCEED_RANGE = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_DAMPING_DIS_EXCEED_RANGE),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_WAYPOINT_COOR_EXCEED_RANGE = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_WAYPOINT_COOR_EXCEED_RANGE),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_FIRST_WP_TYPE_IS_WP_TURN_NO = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_FIRST_WP_TYPE_IS_WP_TURN_NO),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_WP_EXCEED_RADIUS_LIMIT = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_WP_EXCEED_RADIUS_LIMIT ),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRAJ_WP_EXCEED_HEIGHT_LIMIT = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRAJ_WP_EXCEED_HEIGHT_LIMIT ),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_STATUS_RESV = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_STATUS_RESV),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_STATUS_WP_MIS_CHECK_FAIL = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_STATUS_WP_MIS_CHECK_FAIL),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_STATUS_HOME_NOT_RECORDED = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_STATUS_HOME_NOT_RECORDED),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_STATUS_LOW_LOCATION_ACCURACY = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_STATUS_LOW_LOCATION_ACCURACY),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_STATUS_RTK_CONDITION_IS_NOT_READY = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_STATUS_RTK_CONDITION_IS_NOT_READY),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_SECURE_RESV = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_SECURE_RESV),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_SECURE_CROSS_NFZ = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_SECURE_CROSS_NFZ),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_SECURE_BAT_LOW = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_SECURE_BAT_LOW),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTION_COMMON_RESV = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTION_COMMON_RESV),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTION_COMMON_ACTION_ID_DUPLICATED = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTION_COMMON_ACTION_ID_DUPLICATED),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTION_COMMON_ACTION_ITEMS_SPACE_NOT_ENOUGH = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTION_COMMON_ACTION_ITEMS_SPACE_NOT_ENOUGH),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTION_COMMON_ACTION_SIZE_GT_BUF_SIZE = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTION_COMMON_ACTION_SIZE_GT_BUF_SIZE),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTION_COMMON_ACTION_ID_NOT_FOUND = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTION_COMMON_ACTION_ID_NOT_FOUND),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTION_COMMON_DOWNLOAD_ACTION_ID_RANGE_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTION_COMMON_DOWNLOAD_ACTION_ID_RANGE_ERROR),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTION_COMMON_NO_ACTION_ITEMS_STORED = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTION_COMMON_NO_ACTION_ITEMS_STORED),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRIGGER_RESV = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRIGGER_RESV),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRIGGER_TYPE_INVALID = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRIGGER_TYPE_INVALID),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRIGGER_REACH_WP_END_INDEX_LT_START_INDEX = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRIGGER_REACH_WP_END_INDEX_LT_START_INDEX),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRIGGER_REACH_WP_INVALID_INTERVAL_WP_NUM = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRIGGER_REACH_WP_INVALID_INTERVAL_WP_NUM),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRIGGER_REACH_WP_INVALID_AUTO_TERMINATE_WP_NUM = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRIGGER_REACH_WP_INVALID_AUTO_TERMINATE_WP_NUM),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRIGGER_ASSOCIATE_INVALID_TYPE = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRIGGER_ASSOCIATE_INVALID_TYPE),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_TRIGGER_SIMPLE_INTERVAL_INVALID_TYPE = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_TRIGGER_SIMPLE_INTERVAL_INVALID_TYPE),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_COMMON_RESV = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_COMMON_RESV),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_COMMON_ACTUATOR_EXEC_NON_SUPPORTED = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_COMMON_ACTUATOR_EXEC_NON_SUPPORTED),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_COMMON_ACTUATOR_TYPE_INVALID = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_COMMON_ACTUATOR_TYPE_INVALID),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_COMMON_ACTUATOR_FUNC_INVALID = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_COMMON_ACTUATOR_FUNC_INVALID),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_CAMERA_RESV = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_CAMERA_RESV),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_CAMERA_SEND_SINGLE_SHOT_CMD_TO_CAMERA_FAIL = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_CAMERA_SEND_SINGLE_SHOT_CMD_TO_CAMERA_FAIL),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_CAMERA_SEND_VIDEO_START_CMD_TO_CAMERA_FAIL = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_CAMERA_SEND_VIDEO_START_CMD_TO_CAMERA_FAIL),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_CAMERA_SEND_VIDEO_STOP_CMD_TO_CAMERA_FAIL = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_CAMERA_SEND_VIDEO_STOP_CMD_TO_CAMERA_FAIL),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_CAMERA_FOCUS_PARAM_XY_INVALID = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_CAMERA_FOCUS_PARAM_XY_INVALID),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_CAMERA_SEND_FOCUS_CMD_TO_CAMERA_FAIL = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_CAMERA_SEND_FOCUS_CMD_TO_CAMERA_FAIL),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_CAMERA_SEND_FOCALIZE_CMD_TO_CAMERA_FAIL = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_CAMERA_SEND_FOCALIZE_CMD_TO_CAMERA_FAIL),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_CAMERA_FOCAL_DISTANCE_INVALID = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_CAMERA_FOCAL_DISTANCE_INVALID),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_CAMERA_EXEC_FAIL = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_CAMERA_EXEC_FAIL),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_GIMBAL_RESV = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_GIMBAL_RESV),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_GIMBAL_INVALID_RPY_ANGLE_CTRL_CMD = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_GIMBAL_INVALID_RPY_ANGLE_CTRL_CMD),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_GIMBAL_INVALID_DURATION_CMD = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_GIMBAL_INVALID_DURATION_CMD),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_GIMBAL_FAIL_TO_ARRIVE_TGT_ANGLE = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_GIMBAL_FAIL_TO_ARRIVE_TGT_ANGLE),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_GIMBAL_FAIL_TO_SEND_CMD_TO_GIMBAL = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_GIMBAL_FAIL_TO_SEND_CMD_TO_GIMBAL),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_GIMBAL_THIS_INDEX_OF_GIMBAL_NOT_DOING_UNIFORM_CTRL = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_GIMBAL_THIS_INDEX_OF_GIMBAL_NOT_DOING_UNIFORM_CTRL),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_FLIGHT_RESV = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_FLIGHT_RESV),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_FLIGHT_YAW_INVALID_YAW_ANGLE = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_FLIGHT_YAW_INVALID_YAW_ANGLE),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_FLIGHT_YAW_TO_TGT_ANGLE_TIMEOUT = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_FLIGHT_YAW_TO_TGT_ANGLE_TIMEOUT),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_FLIGHT_ACTION_YAW_OCCUPIED = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_FLIGHT_ACTION_YAW_OCCUPIED),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_FLIGHT_CUR_AND_TGT_VEL_CLE_STATUE_EQUAL = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_FLIGHT_CUR_AND_TGT_VEL_CLE_STATUE_EQUAL),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_PAYLOAD_RESV = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_PAYLOAD_RESV),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_PAYLOAD_FAIL_TO_SEND_CMD_TO_PAYLOAD = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_PAYLOAD_FAIL_TO_SEND_CMD_TO_PAYLOAD),
	DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_PAYLOAD_EXEC_FAILED = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V2, DJI_ERROR_WAYPOINT_V2_MODULE_RAW_CODE_ACTUATOR_PAYLOAD_EXEC_FAILED),
};
//@formatter:on

/* Exported functions --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif // DJI_ERROR_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
