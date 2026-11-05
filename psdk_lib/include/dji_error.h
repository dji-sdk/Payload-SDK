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
/* flight controller joystick module error message */\
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
{DJI_ERROR_FC_ACTION_MODULE_ERROR_CMD_IS_RUNNING, "Command is running.", "Please wait for command to finish."}, \
{DJI_ERROR_FC_ACTION_MODULE_ERROR_IS_NOT_RUNNING, "Command is not running.", "Please start command first."}, \
{DJI_ERROR_FC_ACTION_MODULE_ERROR_CMD_INVALID, "Command is invalid.", "Please check command validity."}, \
{DJI_ERROR_FC_ACTION_MODULE_ERROR_NO_GEAR, "No landing gear.", "Please check landing gear."}, \
{DJI_ERROR_FC_ACTION_MODULE_ERROR_PACK_GIMBAL_MOUNTED, "Folding mode: gimbal is mounted.", "Please remove gimbal before folding."}, \
{DJI_ERROR_FC_ACTION_MODULE_ERROR_PACK_US_NOT_HEALTHY, "Folding mode: aircraft is not healthy.", "Please check aircraft health."}, \
{DJI_ERROR_FC_ACTION_MODULE_ERROR_PACK_HAS_PACKED, "Already in folded state.", NULL}, \
{DJI_ERROR_FC_ACTION_MODULE_ERROR_NOT_PACKED, "Not in folded state.", NULL}, \
{DJI_ERROR_FC_ACTION_MODULE_ERROR_PACK_NOT_SUPPORTED, "Folding is not supported.", NULL}, \
{DJI_ERROR_FC_ACTION_MODULE_TRIGGER_ERROR_CANNOT_START_MOTOR, "can not start motor", "Please check motor status"} ,\
{DJI_ERROR_FC_ACTION_MODULE_TRIGGER_ERROR_LOW_VOLTAGE, "low voltage", "Please change battery"} ,\
{DJI_ERROR_FC_ACTION_MODULE_ERROR_IN_HOVER, "Aircraft is hovering.", NULL}, \
{DJI_ERROR_FC_ACTION_MODULE_TRIGGER_ERROR_SPEED_TOO_LARGE, "speed too large", "Please slow down"} ,\
{DJI_ERROR_FC_ACTION_MODULE_ERROR_NO_HORIZ_VEL_INFO, "No horizontal velocity information.", NULL}, \
{DJI_ERROR_FC_ACTION_MODULE_ERROR_ANG_NORMAL, "Flip recovery: attitude is normal.", NULL}, \
{DJI_ERROR_FC_ACTION_MODULE_ERROR_CRAFT_NEVER_CRASH, "Flip recovery: aircraft has not crashed.", NULL}, \
{DJI_ERROR_FC_ACTION_MODULE_ERROR_DRONE_UNCTRLABLE, "Aircraft is uncontrollable.", "Please check aircraft status."}, \
{DJI_ERROR_FC_ACTION_MODULE_ERROR_CRAFT_NO_AVAILABLE_ESC, "Flip recovery: all ESCs are abnormal.", "Please check ESC status."}, \
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
{DJI_ERROR_FC_HOME_LOCATION_MODULE_RTK_ENABLE_BUT_NOT_READY, "RTK is enabled but not converged.", "Please wait for RTK to converge."}, \
{DJI_ERROR_FC_HOME_LOCATION_MODULE_RC_NOT_READY, "Beacon/RC positioning accuracy is too poor.", "Please check beacon/RC positioning."}, \
{DJI_ERROR_FC_HOME_LOCATION_MODULE_RC_IN_NFZ, "Home point is in a no-fly zone.", "Please set home point outside of no-fly zones."}, \
{DJI_ERROR_FC_HOME_LOCATION_MODULE_NONSUPPORT_CUR_HP_TYPE, "Current home point type is not supported.", "Please check home point type."}, \
\
/* flight controller emergency stop motor module error message */\
{DJI_ERROR_FC_EMERGENCY_STOP_MOTOR_MODULE_VERSION_NOT_MATCH, "Emergency stop motor failed", "Please check aircraft status and motor state."} ,\
{DJI_ERROR_FC_EMERGENCY_STOP_MOTOR_MODULE_CMD_INVALID, "emergency stop motor cmd invalid", NULL} ,\
/* camera manager module error message */\
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_ACK, "ACK message.", NULL}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_CANNOT_ENTER_PLAYBACK_IN_CUR_STATE, "Cannot enter playback mode in current camera state.", "Please check camera state."}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_FILE_TRANSFER_BUSY, "File transfer is busy.", "Please wait for current transfer to finish."}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_INVALID_TASK_INFORMATION, "No valid task information.", "Please check task information."}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_TASK_UPLOAD_ERROR, "Task information upload error.", "Please retry task upload."}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_INVALID_REQUEST_PARAMETER, "Invalid request parameter.", "Please double-check requested parameters."}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_MAY_CROSS_RESTRICTED_AREA, "Task may cross restricted area.", "Please check flight route."}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_EXCEEDED_INPUT_TIME_LIMIT, "Task duration may exceed input time limit.", "Please adjust task parameters."}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_EXECUTING_HIGHER_PRIORITY_TASK, "A higher priority task is being executed.", "Please stop the higher priority task or try again later."}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_CANNOT_START_TASK_WEAK_GPS, "Cannot start task due to weak GPS signal.", "Please fly to an area with better GPS signal."}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_CANNOT_START_TASK_VOLTAGE_ALARM, "Cannot start task due to voltage alarm.", "Please check battery voltage."}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_MISMATCH, "Operation does not match.", "Please check operation validity."}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_DATA, "Returning data.", NULL}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_TRY_LATER, "Resource is occupied, please try again later.", "Please try again later."}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_NOT_IMPLEMENTED, "Feature is not implemented.", "Please contact DJI for help."}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_CANCELLED, "Operation cancelled by user.", NULL}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_NOT_FOUND, "Resource not found.", "Please check resource availability."}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND, "Command not supported", "Check the firmware or command validity"}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_TIMEOUT, "Camera's execution of this action has timed out", "Try again or check the firmware or command"}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_RAM_ALLOCATION_FAILED, "Camera's execution of this action is out of memory", "Please contact <dev@dji.com> for help."}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_INVALID_COMMAND_PARAMETER, "Camera received invalid parameters", "Check the validity of the parameter"}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND_IN_CUR_STATE, "Camera is busy or the command is not supported in the Camera's current state", "Check current camera state is if appropriate for the CMD"}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_CAMERA_TIME_NOT_SYNCHRONIZED, "The time stamp of the camera is not sync", "Please contact <dev@dji.com> for help."}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_PARAMETER_SET_FAILED, "Camera failed to set the parameters it received", "Please check the parameter to set is if supported in your devices."}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_PARAMETER_GET_FAILED, "Camera param get failed", "Please check the parameter to get is if supported in your devices."}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_SD_CARD_MISSING, "Camera has no SD Card", "Please install SD card."}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_SD_CARD_FULL, "The Camera's SD Card is full", "Please make sure the SD card has enough space."}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_SD_CARD_ERROR, "Error accessing the SD Card", "Please check the validity of the SD card."}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_SENSOR_ERROR, "Camera sensor error", "Please contact <dev@dji.com> for help."}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_SYSTEM_ERROR, "Camera system error", "Please recheck all the running conditions or contact <dev@dji.com> for help."}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_PARAMETER_TOTAL_TOO_LONG, "Camera param get failed", "Please check the validity of the parameter length"}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_MODULE_INACTIVATED, "Camera module is not activated", "Please activate the module first."}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_USER_UNBOUND, "User is not bound.", "Please bind the user first."}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_FIRMWARE_DATA_NUM_DISCONTINUOUS, "The seq number of Firmware data is invalid", "Please check the validity of the camera firmware."}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_FIRMWARE_DATA_OVERLOAD_FLASH, "Firmware data exceeds Flash space.", "Please check firmware size."}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_FIRMWARE_VERIFICATION_ERROR, "Firmware check error", "Please check the validity of the camera firmware."}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_FLASH_ERASE_ERROR, "Flash erase error.", "Please contact DJI for help."}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_FLASH_WRITE_ERROR, "Camera flash write error", "Please contact <dev@dji.com> for help."}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UPGRADE_STATUS_ERROR, "Upgrade status error.", "Please check upgrade status."}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_FIRMWARE_TYPE_MISMATCH, "Firmware type is invalid", "Please check the validity of the camera firmware."}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_WAITING_CLIENT_UPGRADE_STATUS, "Upgrade host needs to wait for client upgrade status push.", "Please wait for client upgrade status."}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_REMOTE_CONTROL_UNCONNECTED, "Remote Control is disconnected now", "Please check the connection with remote control is if OK."}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_MOTOR_NOT_STOPPED, "Motor is not stopped.", "Please stop motor first."}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_HARDWARE_ERROR, "Camera hardware error", "Please contact <dev@dji.com> for help."}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_INSUFFICIENT_ELECTRICITY, "Insufficient battery.", "Please replace battery."}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_AIRCRAFT_UNCONNECTED, "Disconnect with aircraft", "Please check the connection with aircraft is if OK."}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_FLASH_IS_ERASING, "Flash is erasing.", "Please wait for erase to complete."}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_CANNOT_UPGRADE_IN_CUR_STATE, "Camera cannot not upgrade in current status", "Please contact <dev@dji.com> for help."}, \
{DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNDEFINE_ERROR, "Undefined error", "Please contact <dev@dji.com> for help."}, \
\
/* gimbal manager module error message */\
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_ACK, "ACK message.", NULL}, \
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_INVALID_REQUEST_PARAMETER, "Invalid request parameter.", "Please double-check requested parameters."}, \
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_MISMATCH, "Operation does not match.", "Please check operation validity."}, \
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_DATA, "Returning data.", NULL}, \
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_TRY_LATER, "Resource is occupied, please try again later.", "Please try again later."}, \
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_NOT_IMPLEMENTED, "Feature is not implemented.", "Please contact DJI for help."}, \
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_CANCELLED, "Operation cancelled by user.", NULL}, \
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_NOT_FOUND, "Resource not found.", "Please check resource availability."}, \
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND, "Command not supported", "Check the firmware or command validity"}, \
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_TIMEOUT, "Execution timeout.", "Please try again or check the firmware or command."}, \
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_RAM_ALLOCATION_FAILED, "Memory allocation failed.", "Please contact DJI for help."}, \
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_INVALID_COMMAND_PARAMETER, "Invalid command parameter.", "Please check the validity of the parameter."}, \
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND_IN_CUR_STATE, "Command is not supported in current state.", "Please check current state is appropriate for the command."}, \
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_CAMERA_TIME_NOT_SYNCHRONIZED, "Time is not synchronized.", "Please wait for time sync."}, \
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_PARAMETER_SET_FAILED, "Set parameter failed.", "Please check the parameter to set is supported in your device."}, \
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_PARAMETER_GET_FAILED, "Get parameter failed.", "Please check the parameter to get is supported in your device."}, \
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_SD_CARD_MISSING, "SD card is not inserted.", "Please install SD card."}, \
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_SD_CARD_FULL, "SD card is full.", "Please make sure the SD card has enough space."}, \
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_SD_CARD_ERROR, "SD card error.", "Please check the validity of the SD card."}, \
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_SENSOR_ERROR, "Sensor error.", "Please contact DJI for help."}, \
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_SYSTEM_ERROR, "System error.", "Please recheck all the running conditions or contact DJI for help."}, \
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_PARAMETER_TOTAL_TOO_LONG, "Parameter length is too long.", "Please check the validity of the parameter length."}, \
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_MODULE_INACTIVATED, "Module is not activated.", "Please activate the module first."}, \
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_USER_UNBOUND, "User is not bound.", "Please bind the user first."}, \
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_FIRMWARE_DATA_NUM_DISCONTINUOUS, "The seq number of Firmware data is invalid", "Please check the validity of the camera firmware."}, \
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_FIRMWARE_DATA_OVERLOAD_FLASH, "Firmware data exceeds Flash space.", "Please check firmware size."}, \
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_FIRMWARE_VERIFICATION_ERROR, "Firmware check error", "Please check the validity of the camera firmware."}, \
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_FLASH_ERASE_ERROR, "Flash erase error.", "Please contact DJI for help."}, \
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_FLASH_WRITE_ERROR, "Flash write error.", "Please contact DJI for help."}, \
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_UPGRADE_STATUS_ERROR, "Upgrade status error.", "Please check upgrade status."}, \
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_FIRMWARE_TYPE_MISMATCH, "Firmware type is invalid", "Please check the validity of the camera firmware."}, \
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_WAITING_CLIENT_UPGRADE_STATUS, "Upgrade host needs to wait for client upgrade status push.", "Please wait for client upgrade status."}, \
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_REMOTE_CONTROL_UNCONNECTED, "Remote Control is disconnected now", "Please check the connection with remote control is if OK."}, \
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_MOTOR_NOT_STOPPED, "Motor is not stopped.", "Please stop motor first."}, \
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_HARDWARE_ERROR, "Hardware error.", "Please contact DJI for help."}, \
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_INSUFFICIENT_ELECTRICITY, "Insufficient battery.", "Please replace battery."}, \
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_AIRCRAFT_UNCONNECTED, "Disconnect with aircraft", "Please check the connection with aircraft is if OK."}, \
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_FLASH_IS_ERASING, "Flash is erasing.", "Please wait for erase to complete."}, \
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_CANNOT_UPGRADE_IN_CUR_STATE, "Cannot upgrade in current state.", "Please contact DJI for help."}, \
{DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_UNDEFINE_ERROR, "Undefined error", "Please contact <dev@dji.com> for help."}, \
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
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_GIMBAL_INVALID_RPY_ANGLE_CTRL_CMD, "Gimbal roll	pitch yaw angle ctrl cmd param invalid	", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_GIMBAL_INVALID_DURATION_CMD, "Gimbal duration param invalid	 unable to exec. ", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_GIMBAL_FAIL_TO_ARRIVE_TGT_ANGLE, "Gimbal fail to arrive target angle . ", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_GIMBAL_FAIL_TO_SEND_CMD_TO_GIMBAL, "Fail to send cmd to gimbal for gimbal is busy or no gimbal. ", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_GIMBAL_THIS_INDEX_OF_GIMBAL_NOT_DOING_UNIFORM_CTRL , "Fail to stop gimbal uniform ctrl because index error. ", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_FLIGHT_RESV, "Reserved error code", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_FLIGHT_YAW_INVALID_YAW_ANGLE, "Yaw angle is lager max yaw angle. ", NULL}, \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_FLIGHT_YAW_TO_TGT_ANGLE_TIMEOUT, "Failed to target yaw angle because of timeout. ", NULL},    \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_FLIGHT_ACTION_YAW_OCCUPIED, "Failed to target yaw angle because of timeout. ", NULL},    \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_FLIGHT_CUR_AND_TGT_VEL_CLE_STATUE_EQUAL, "Failed to current and target vel not equal. ", NULL},    \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_PAYLOAD_RESV, "Failed to payload reserved error code. ", NULL},    \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_PAYLOAD_FAIL_TO_SEND_CMD_TO_PAYLOAD, "Failed to sned cmd to payload. ", NULL},    \
{DJI_ERROR_WAYPOINT_V2_MODULE_CODE_ACTUATOR_PAYLOAD_EXEC_FAILED, "Failed to execute payload actuator. ", NULL},    \
/* waypoint v3 module error message */   \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_COMMON_SUCCESS, "Execute waypoint v3 cmd successfully. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_MISSION_ID_NOT_EXIST, "Execute waypoint v3 mission id not match. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_WAYLINE_INFO_ERROR , "Execute waypoint v3 wayline info error. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_WPMZ_FILE_VERSION_NOT_MATCH, "Execute waypoint v3 wpmz file not match. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_WPMZ_FILE_LOAD_ERROR, "Load waypoint v3 wpmz file error. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_NO_BREAK_INFO, "Execute waypoint v3 no break info error. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_CMD_INVALID, "Execute waypoint v3 cmd invalid. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_CANNOT_START_WAYLINE_WHEN_WAYLINE_RUNNING, "Cannot start wayline when wayline running. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_CANNOT_BREAK_WAYLINE_IN_CUR_STATE, "Cannot break wayline in current state. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_CANNOT_STOP_WAYLINE_WHEN_WAYLINE_NOT_RUNNING, "Cannot stop wayline when wayline not running. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_CANNOT_BREAK_WAYLINE_WHEN_WAYLINE_NOT_RUNNING, "Cannot break wayline when wayline not running. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_CANNOT_REQUEST_DRONE_CONTROL, "Flight mission conflict, unable to obtain control auth of the aircraft. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_CANNOT_RESUME_WAYLINE_IN_CUR_STATE, "Failed to resume wayline in current state. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_CANNOT_TAKEOFF_ON_SHIP, "Cannot takeoff on ship.", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_HEIGHT_LIMIT, "Execute waypoint v3 failed due to height limit. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_RADIUS_LIMIT, "Execute waypoint v3 failed due to radius limit. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_CROSS_FLYLIMIT_AERA, "Execute waypoint v3 failed due to cross flylimit area. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_LOW_LIMIT, "Execute waypoint v3 failed due to low limit. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_OBSTACAL_STOP, "Execute waypoint v3 failed due to obstacal stop. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_RTK_DISCONNECT, "Execute waypoint v3 failed due to rtk disconnect. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_BOUNDARY_LIMIT, "Execute waypoint v3 failed due to boundary limit. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_RC_PITCH_ROLL_BREAK, "Execute waypoint v3 failed due to rc pitch roll break. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_AIRPORT_HEIGHT_LIMIT, "Execute waypoint v3 failed due to airport height limit. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_REQUEST_TAKEOFF_FAIL, "Failed to request take off. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_AUTOTAKEOFF_RUN_FAIL, "Failed to run auto take off. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_REQUEST_WAYLINE_FAIL, "Failed to request wayline. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_AGRO_PLAN_FAIL, "Failed to agro plan. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_REQUEST_QUICK_TAKEOFF_ASSIST_FAIL, "Failed to request quick take off assist. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_QUICK_TAKEOFF_ASSIST_RUN_FAIL, "Failed to run quick takeoff assist. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_VFENCE_LIMIT, "Execute waypoint v3 failed due to vfence limit. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_DJI_TARGET_NOT_REACHABLE, "Target point is unreachable, may have no-fly zone or obstacle.", "Please check target location."},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_DJI_TRAJ_PLAN_FAILED, "Trajectory planning failed.", "Please check wayline parameters."},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_DEPARTURE_WAYLINE_FAIL, "Departure wayline execution failed.", "Please retry the mission."},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_DEPARTURE_WAYLINE_INIT_FAIL, "Departure wayline initialization failed.", "Please retry the mission."},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_DEPARTURE_WAYLINE_INVALID, "Departure wayline is unavailable.", "Please retry the mission."},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_GPS_INVALID, "Execute waypoint v3 failed due to gps invalid. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_CANNOT_START_AT_CURRENT_RC_MODE, "Failed to start at current rc mode. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_HOME_POINT_NOT_RECORDED, "Execute waypoint v3 failed due to homepoint not recorded. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_LOWER_BATTERY, "Execute waypoint v3 failed due to lower battery. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_RETURN_HOME, "Execute waypoint v3 failed due to return home. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ADSB_ERROR, "Execute waypoint v3 failed due to adsb error. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_RC_LOST, "Execute waypoint v3 failed due to rc lost. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_RTK_NOT_READY, "Execute waypoint v3 failed due to rtk not ready. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_DRONE_IS_MOVING, "Execute waypoint v3 failed due to drone is moving. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_DRONE_ON_GROUND_MOTOR_ON, "Execute waypoint v3 failed due to drone on ground motor on. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_SURFACE_FOLLOW_CAMERA_INVALID, "Execute waypoint v3 failed due to surface follow camera invalid. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_SURFACE_FOLLOW_HEIGHT_INVALID, "Execute waypoint v3 failed due to surface follow height invalid.", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_SURFACE_FOLLOW_MAP_WRONG, "Execute waypoint v3 failed due to surface follow map wrong. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_HOMEPOINT_NOT_MATCH_RTK, "Execute waypoint v3 failed due to homepoint not match rtk. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRANSPORT_DIVERT, "Transport aircraft divert.", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_STRONG_WIND_GOHOME, "Execute waypoint v3 failed due to strong wind gohome. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_DOCK_MOVE_GOHOME, "Dock move gohome.", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_HEADING_LEVEL_POOR, "Heading level too poor to reach waypoint.", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_JAMMING_GOHOME, "Signal interference or spoofing gohome.", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_WIND_SPD, "Wind speed too high, exit wayline.", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_BUSY_IMU_CHANGED, "Primary IMU changed.", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_RAIN_DETECT_VALID, "Rain detected.", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_WP_ARM1_ERROR, "Arm 1 not in position.", "Please check arm 1 status."},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_WP_ARM2_ERROR, "Arm 2 not in position.", "Please check arm 2 status."},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_WP_ARM3_ERROR, "Arm 3 not in position.", "Please check arm 3 status."},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_WP_ARM4_ERROR, "Arm 4 not in position.", "Please check arm 4 status."},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_WP_RADAR_PEDEX_WARNING, "Radar obstacle detection abnormal.", "Please check radar status."},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_WP_OVERLOAD, "Aircraft overloaded.", "Please reduce payload."},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_WP_HOIST_NOT_READY, "Hoist not ready.", "Please check hoist status."},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_WP_HOIST_SELF_CHECK_ERR, "Hoist self-check error.", "Please check hoist cable."},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_WP_PREVENT_MOTOR_START, "Other reason prevents motor start, wayline failed.", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_WP_EST_BAT_OVERTEMP_ERROR, "Estimated battery overtemperature during wayline flight.", "Please check battery temperature."},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_DRONE_CRITICAL_ERROR, "Execute waypoint v3 failed due to drone critical error. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_CANNOT_FIND_PAYLOAD, "Execute waypoint v3 failed due to cannot find payload. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_EXECUTION_FAILED, "Execute waypoint v3 failed due to action execution failed. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_FARM_NO_PESTICIDE, "Execute waypoint v3 failed due to farm no pesticide. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_RADAR_DISCONNECT, "Execute waypoint v3 failed due to radar disconnect. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_PAYLOAD_NOT_HOIST, "Payload type is not hoist.", "Please check payload type."},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_USER_EXIT, "Execute waypoint v3 failed due to user exit. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_USER_BREAK, "Execute waypoint v3 failed due to user break. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_USER_SET_GOHOME, "Execute waypoint v3 failed due to user set gohome. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_USER_AGRO_PLANNER_STATE_CHANGE, "Execute waypoint v3 failed due to user agro panner state change. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_USER_SWITCH_RC_MODE, "Execute waypoint v3 failed due to user switch rc mode. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_INIT_FAIL, "Execute waypoint v3 failed due to traj init failed. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_JOB_EXIT_BUT_MIS_RUNNING, "Execute waypoint v3 failed due to traj job exit but mis running. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_ON_GROUND_MOTOR_ON_CANNOT_GO, "Execute waypoint v3 failed due to traj on ground motor is turn on. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_INVALID_START_INDEX_OR_PROG, "Execute waypoint v3 failed due to traj invalid start index. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_INVALID_CSYS_MODE, "Execute waypoint v3 failed due to traj invalid csys mode. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_INVALID_HEIGHT_MODE, "Execute waypoint v3 failed due to traj invalid height mode. ", NULL},\
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_INVALID_FLY_WP_MODE, "Execute waypoint v3 failed due to traj invalid fly wp mode. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_INVALID_YAW_MODE, "Execute waypoint v3 failed due to traj invalid yaw mode. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_INVALID_TURN_DIR_MODE, "Execute waypoint v3 failed due to traj invalid turn dir mode. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_INVALID_WP_TYPE, "Execute waypoint v3 failed due to traj invalid wp type. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_FIR_LAS_WP_TYPE_ERROR, "Execute waypoint v3 failed due to fir las wp type error. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_GLOB_VEL_OUT_OF_RANGE, "Execute waypoint v3 failed due to traj wp global vel out of range. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_WP_NUM_OUT_OF_RANGE, "Execute waypoint v3 failed due to traj wp num out of range. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_LAT_LONG_OUT_OF_RANGE, "Execute waypoint v3 failed due to traj lat or lon out of range. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_DAMP_DIS_OUT_OF_RANGE, "Execute waypoint v3 failed due to traj damp dis out of range. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_MAX_VEL_OUT_OF_RANGE, "Execute waypoint v3 failed due to traj max vel out of range. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_VEL_OUT_OF_RANGE, "Execute waypoint v3 failed due to traj vel out of range. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_WP_YAW_OUT_OF_RANGE, "Execute waypoint v3 failed due to traj wp yaw out of range. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_INVALID_YAW_MODE_IN_VERT_SEGM, "Execute waypoint v3 failed due to traj invalid yaw mode in vert segm. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_WP_BREAK_INFO_MISSION_ID_CHANGED, "Execute waypoint v3 failed due to traj wp break info mission id changed. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_WP_BREAK_INFO_PROGRESS_OUT_OF_RANGE, "Execute waypoint v3 failed due to traj wp break info progress out of range. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_WP_BREAK_INFO_INVALID_MISSION_STATE, "Execute waypoint v3 failed due to traj wp break info invalid mission state. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_WP_BREAK_INFO_WP_INDEX_OUT_OF_RANGE, "Execute waypoint v3 failed due to traj wp break info wp index out of range. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_BREAK_LAT_LONG_OUT_OF_RANGE, "Execute waypoint v3 failed due to traj break lat or lon out of range. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_BREAK_INFO_WP_YAW_OUT_OF_RANGE, "Execute waypoint v3 failed due to traj break info wp yaw out of range. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_INVALID_BREAK_INFO_FLAG , "Execute waypoint v3 failed due to traj invalid break info flag. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_GET_TRAJ_INFO_FAILED, "Execute waypoint v3 failed due to traj get info failed. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_GENERATE_FAIL, "Execute waypoint v3 failed due to traj generate failed. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_LIB_RUN_FAIL, "Execute waypoint v3 failed due to traj lib run failed. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_LIB_EMERGENCY_BRAKE, "Execute waypoint v3 failed due to traj lib emergency brake. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_ACTION_NOT_FOUND, "Execute waypoint v3 failed due to action not found. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_ACTION_INDEX_REPEATED, "Execute waypoint v3 failed due to action index repeated. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_ACTION_INFO_SIZE_TOO_LONG_OR_TOO_SHORT, "Execute waypoint v3 failed due to size too long or too short. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_ACTION_TREE_EMPTY, "Execute waypoint v3 failed due to action tree empty. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_ACTION_TREE_LAYER_EMPTY, "Execute waypoint v3 failed due to action tree layer empty. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_ACTION_ID_REPEATED, "Execute waypoint v3 failed due to action repeated. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_ACTION_NODE_CHILDREN_NUM_LT_2, "Execute waypoint v3 failed due to action mode children num less than 2. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_ACTION_INDEX_OUT_OF_RANGE, "Execute waypoint v3 failed due to action index out of range. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_ACTION_ID_IS_65535, "Execute waypoint v3 failed due to action id is 65535. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_ACTION_NODE_CHILDNUM_SUM_NOT_EQ_NEXT_LAYER_SIZE, "Execute waypoint v3 failed due to action node child sum not equal next layer size. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_ACTION_TREE_LAYER_NUM_TOO_MORE, "Execute waypoint v3 failed due to action tree layer num too more. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_ACTION_TREE_LAYER_NUM_TOO_LESS, "Execute waypoint v3 failed due to action tree layer num too less. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_ACTION_GROUP_NUM_OUT_OF_RANGE, "Execute waypoint v3 failed due to action group num out of range. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_ACTION_GROUP_VALID_RANGE_ERROR, "Execute waypoint v3 failed due to action group valid range error. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_ACTION_TREE_ROOT_STATUS_INVALID, "Execute waypoint v3 failed due to action tree root status invalid. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_ACTION_TREE_NODE_STATUS_INVALID, "Execute waypoint v3 failed due to action tree mode status invalid. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_BREAK_INFO_ACTION_GROUP_ID_OUT_OF_RANGE, "Execute waypoint v3 failed due to break info action group id out of range. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_ACTION_STATUS_TREE_SIZE_ERROR, "Execute waypoint v3 failed due to action status tree size error. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_BREAK_INFO_TRIGGER_RUN_RESULT_INVALID, "Execute waypoint v3 failed due to break info trigger run result invalid. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_BREAK_INFO_ACTION_GROUP_ID_REPEATED, "Execute waypoint v3 failed due to break info action group id repeated. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_BREAK_INFO_ACTION_LOCATION_REPEATED, "Execute waypoint v3 failed due to break info action location repeated. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_BREAK_INFO_ACTION_LOCATION_OUT_OF_RANGE, "Execute waypoint v3 failed due to break info action location out of range. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_RESUME_ID_NOT_IN_BREAK_INFO, "Execute waypoint v3 failed due to resume id not in break info. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_RESUME_INFO_MODIFY_ACTION_STATUS_FROM_NO_INTERRUPT_TO_INTERRUPT, "Execute waypoint v3 failed due to modify action status from no interrupt to interrupt. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_ACTION_RESUME_FAIL_FOR_INVALID_RESUME_INFO, "Execute waypoint v3 failed due to action resume failed for invalid resume info. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTUATOR_COMMON_ACTUATOR_NOT_FOUND, "Execute waypoint v3 failed due to actuator not found. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRIGGER_NOT_FOUND, "Execute waypoint v3 failed due to trigger not found. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRIGGER_SINGLE_TIME_CHECK_FAIL, "Execute waypoint v3 failed due to single time check failed. ", NULL},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_DJI_INNER_ERROR, "DJI internal error.", "Please contact DJI for help."},    \
{DJI_ERROR_WAYPOINT_V3_MODULE_CODE_DJI_UNKNOWN_ERROR, "Unknown internal error.", "Please contact DJI for help."},    \
{DJI_ERROR_WAYPOINT_V3_BREAKPOINT_QUERY_CODE_MISSION_NOT_EXIST, "Breakpoint query: mission does not exist.", "Please check mission ID."},    \
{DJI_ERROR_WAYPOINT_V3_BREAKPOINT_QUERY_CODE_NO_BREAKPOINT, "Breakpoint query: no breakpoint found.", NULL},    \
/* spotlight zoom module error message */ \
{DJI_ERROR_SPZ_MODULE_CODE_OK,                          "SpotlightZoom status normal.", NULL}, \
{DJI_ERROR_SPZ_MODULE_CODE_TAR_TOO_NEAR,                "SpotlightZoom: target too near.", "Please increase distance to target."}, \
{DJI_ERROR_SPZ_MODULE_CODE_TAR_TOO_FAR,                 "SpotlightZoom: target too far.", "Please decrease distance to target."}, \
{DJI_ERROR_SPZ_MODULE_CODE_LOW_LIMIT,                   "SpotlightZoom: reached lower altitude limit.", "Please fly higher."}, \
{DJI_ERROR_SPZ_MODULE_CODE_HEIGHT_LIMIT,                "SpotlightZoom: reached upper altitude limit.", "Please fly lower."}, \
{DJI_ERROR_SPZ_MODULE_CODE_RADIUS_LIMIT,                "SpotlightZoom: reached radius limit.", NULL}, \
{DJI_ERROR_SPZ_MODULE_CODE_BOUNDARY_LIMIT,              "SpotlightZoom: approaching flight restriction zone.", "Please steer away from restricted area."}, \
{DJI_ERROR_SPZ_MODULE_CODE_GIMBAL_LIMITED,              "SpotlightZoom: gimbal reached tracking composition limit.", NULL}, \
{DJI_ERROR_SPZ_MODULE_CODE_YAW_LIMITED,                 "SpotlightZoom: yaw reached surround mode max limit (±80°).", NULL}, \
{DJI_ERROR_SPZ_MODULE_CODE_TOO_CLOSE_TO_OBSTACLE,       "SpotlightZoom: too close to obstacle.", "Please fly away from the obstacle."}, \
{DJI_ERROR_SPZ_MODULE_CODE_GPS_TO_RTK,                  "SpotlightZoom: positioning switched GPS->RTK, surround paused.", NULL}, \
{DJI_ERROR_SPZ_MODULE_CODE_RTK_TO_GPS,                  "SpotlightZoom: positioning switched RTK->GPS.", NULL}, \
{DJI_ERROR_SPZ_MODULE_CODE_OBSTACLE_AVOID_INVALID,      "SpotlightZoom: obstacle avoidance invalid.", "Please check obstacle avoidance sensors."}, \
{DJI_ERROR_SPZ_MODULE_CODE_ZOOM_TRACKING_LIMITED,       "SpotlightZoom: reached zoom range limit.", NULL}, \
{DJI_ERROR_SPZ_MODULE_CODE_TARGET_LOST,                 "SpotlightZoom: tracking target lost.", "Please reselect the target."}, \
{DJI_ERROR_SPZ_MODULE_CODE_GPS_WEAK,                    "SpotlightZoom: GPS signal weak.", "Please fly to an area with better GPS signal."}, \
{DJI_ERROR_SPZ_MODULE_CODE_EXIT_NO_ERROR,               "SpotlightZoom exited normally.", NULL}, \
{DJI_ERROR_SPZ_MODULE_CODE_EXIT_UNSUPPORTED_PAYLOAD,    "SpotlightZoom exit: unsupported payload type.", "Please check payload compatibility."}, \
{DJI_ERROR_SPZ_MODULE_CODE_EXIT_UNSUPPORTED_CAMERA_MODE,"SpotlightZoom exit: unsupported camera mode.", "Please switch to a compatible camera mode."}, \
{DJI_ERROR_SPZ_MODULE_CODE_EXIT_INVALID_CMD,            "SpotlightZoom exit: invalid command.", "Please check command validity."}, \
{DJI_ERROR_SPZ_MODULE_CODE_EXIT_GPS_INVALID,            "SpotlightZoom exit: positioning invalid.", "Please wait for GPS to be ready."}, \
{DJI_ERROR_SPZ_MODULE_CODE_EXIT_NO_IN_AIR,              "SpotlightZoom exit: aircraft not airborne.", "Please take off first."}, \
{DJI_ERROR_SPZ_MODULE_CODE_EXIT_UNSUPPORTED_CTRL_MODE,  "SpotlightZoom exit: wrong flight control mode.", "Please switch to the correct flight mode."}, \
{DJI_ERROR_SPZ_MODULE_CODE_EXIT_UNSUPPORTED_FLIGHT_MODE,"SpotlightZoom exit: unavailable in current flight mode (RTH/landing).", NULL}, \
{DJI_ERROR_SPZ_MODULE_CODE_EXIT_RC_LOST,                "SpotlightZoom exit: RC or video link lost.", "Please check RC and video link connection."}, \
{DJI_ERROR_SPZ_MODULE_CODE_EXIT_STATUS_ERR,             "SpotlightZoom exit: internal error.", "Please contact DJI for help."}, \
{DJI_ERROR_SPZ_MODULE_CODE_EXIT_INNER_UNKNOW_ERROR,     "SpotlightZoom exit: unknown internal error.", "Please contact DJI for help."}, \
\
/* FlyTo (SetModeStartMission) module error message */ \
{DJI_ERROR_FC_FLYTO_MODULE_CODE_INTERNAL_ERROR,     "FlyTo failed: internal aircraft communication error.", "Please contact DJI for help."}, \
{DJI_ERROR_FC_FLYTO_MODULE_CODE_BAD_GPS_SIGNAL,       "FlyTo failed: GPS signal too weak.", "Please fly to an area with better GPS signal and retry."}, \
{DJI_ERROR_FC_FLYTO_MODULE_CODE_ABSPOS_INVALID,       "FlyTo failed: positioning completely unavailable.", "Please wait for GPS/RTK to be ready before starting the mission."}, \
{DJI_ERROR_FC_FLYTO_MODULE_CODE_PLANNING_FAIL,        "FlyTo failed: route planning failed.", "Please check the target coordinates and retry."}, \
{DJI_ERROR_FC_FLYTO_MODULE_CODE_HOME_NOTSET,          "FlyTo failed: home point not set.", "Please set the home point before starting the mission."}, \
{DJI_ERROR_FC_FLYTO_MODULE_CODE_RC_LOST,              "FlyTo failed: remote controller signal lost.", "Please check the RC link and retry."}, \
{DJI_ERROR_FC_FLYTO_MODULE_CODE_TAKEOFF_ON_SHIP,      "FlyTo failed: auto takeoff on a vessel is prohibited.", NULL}, \
{DJI_ERROR_FC_FLYTO_MODULE_CODE_SOMEWAYRUNOUT,        "FlyTo warning: insufficient energy to complete the mission.", "Please check battery level or shorten the flight distance."}, \
{DJI_ERROR_FC_FLYTO_MODULE_CODE_SWITCH_PLANNING_ALGO, "FlyTo warning: planning algorithm was switched during the mission.", NULL}, \
{DJI_ERROR_FC_FLYTO_MODULE_CODE_HEIGHT_EXCEED_LIMIT,  "FlyTo warning: target altitude exceeds flight altitude limit.", "Please lower the target altitude or raise the altitude limit."}, \
{DJI_ERROR_FC_FLYTO_MODULE_CODE_GOAL_MEETHORLIM,        "FlyTo failed: target point is inside a horizontal no-fly zone.", "Please choose a target outside of no-fly zones."}, \
{DJI_ERROR_FC_FLYTO_MODULE_CODE_GOAL_MEETHORLIM_RADIUS, "FlyTo failed: target point exceeds user-defined distance limit.", "Please increase the max distance limit or choose a closer target."}, \
{DJI_ERROR_FC_FLYTO_MODULE_CODE_GOAL_MEETVERLIM,        "FlyTo failed: target point is inside a vertical no-fly zone.", "Please choose a target outside of no-fly zones."}, \
{DJI_ERROR_FC_FLYTO_MODULE_CODE_GOAL_MEETVERLIM_TOP,    "FlyTo failed: target altitude exceeds user-defined altitude ceiling.", "Please lower the target altitude or raise the altitude limit."}, \
{DJI_ERROR_FC_FLYTO_MODULE_CODE_GOAL_MEETVERLIM_BOTTOM, "FlyTo failed: target altitude is below minimum flight altitude.", "Please raise the target altitude."}, \
{DJI_ERROR_FC_FLYTO_MODULE_CODE_DEPARTURE_WAYLINE_ERROR,          "FlyTo failed: departure wayline execution failed.", "Please retry the mission."}, \
{DJI_ERROR_FC_FLYTO_MODULE_CODE_DEPARTURE_WAYLINE_INIT,           "FlyTo failed: departure wayline initialization failed.", "Please retry the mission."}, \
{DJI_ERROR_FC_FLYTO_MODULE_CODE_DEPARTURE_WAYLINE_INVALID,        "FlyTo failed: departure wayline is unavailable.", "Please retry the mission."}, \
{DJI_ERROR_FC_FLYTO_MODULE_CODE_DEPARTURE_WAYLINE_NOT_REACHABLE,  "FlyTo failed: departure point unreachable due to no-fly zone or obstacle.", "Please check for obstacles or restricted airspace near the takeoff point."}, \
{DJI_ERROR_FC_FLYTO_MODULE_CODE_DEPARTURE_WAYLINE_PLANNING_FAIL,  "FlyTo failed: departure wayline planning failed.", "Please retry the mission."}, \
{DJI_ERROR_FC_FLYTO_MODULE_CODE_DEPARTURE_WAYLINE_RC_SWITCH_MODE, "FlyTo failed: RC is not in P-mode.", "Please switch the RC to P-mode and retry."}, \
{DJI_ERROR_FC_FLYTO_MODULE_CODE_DEPARTURE_WAYLINE_RTK_TYPE_ERR,   "FlyTo failed: BeiDou-only aircraft does not support departure wayline.", "Please use an aircraft with GPS+BeiDou support."}, \
{DJI_ERROR_FC_FLYTO_MODULE_CODE_PREVENT_MOTOR_ERROR,    "FlyTo failed: motor pre-arm check failed.", "Please check motor status before starting the mission."}, \
{DJI_ERROR_FC_FLYTO_MODULE_CODE_GOALINVALID,             "FlyTo failed: target coordinates are invalid or out of planning range.", "Please verify the target latitude/longitude values."}, \
{DJI_ERROR_FC_FLYTO_MODULE_CODE_MAXFLYVEL_INVALID,       "FlyTo failed: maximum flight speed parameter is invalid.", "Please check the fly_vel parameter."}, \
{DJI_ERROR_FC_FLYTO_MODULE_CODE_VERSION_INVALID,         "FlyTo failed: protocol version mismatch.", "Please update firmware and SDK to matching versions."}, \
{DJI_ERROR_FC_FLYTO_MODULE_CODE_STATEMACHINE_INVALID,    "FlyTo failed: mission state machine busy.", "Please wait for the current task to finish and retry."}, \
{DJI_ERROR_FC_FLYTO_MODULE_CODE_MEA_INVALID,             "FlyTo failed: minimum enroute altitude (MEA) is too low.", "Please set MEA to at least 20 m."}, \
{DJI_ERROR_FC_FLYTO_MODULE_CODE_UAV_MEETHORLIM,          "FlyTo exit: aircraft entered horizontal no-fly zone.", "Please land and resume flight in an unrestricted area."}, \
{DJI_ERROR_FC_FLYTO_MODULE_CODE_UAV_MEETHORLIM_RADIUS,   "FlyTo exit: aircraft exceeded user-defined distance limit.", "Please increase the max distance limit."}, \
{DJI_ERROR_FC_FLYTO_MODULE_CODE_UAV_MEETVERLIM,          "FlyTo exit: aircraft entered vertical no-fly zone.", "Please land and resume flight in an unrestricted area."}, \
{DJI_ERROR_FC_FLYTO_MODULE_CODE_UAV_MEETVERLIM_TOP,      "FlyTo exit: aircraft reached user-defined altitude ceiling.", "Please raise the altitude limit."}, \
{DJI_ERROR_FC_FLYTO_MODULE_CODE_UAV_MEETVERLIM_BOTTOM,   "FlyTo exit: aircraft reached minimum flight altitude.", NULL}, \
{DJI_ERROR_FC_FLYTO_MODULE_CODE_TAKEOFF_FAIL,            "FlyTo exit: takeoff failed.", "Please check motor and sensor status."}, \
{DJI_ERROR_FC_FLYTO_MODULE_CODE_GOAL_BLOCKEDBY_OBSTACLE, "FlyTo exit: target point blocked by obstacle.", "Please choose a target in an unobstructed area."}, \
{DJI_ERROR_FC_FLYTO_MODULE_CODE_OBSTACLETOOCLOSE,        "FlyTo exit: obstacle too close, emergency brake applied.", "Please fly away from the obstacle."}, \
{DJI_ERROR_FC_FLYTO_MODULE_CODE_ROUTE_BLOCKED,           "FlyTo exit: route blocked, replanning failed.", "Please choose an alternative target or clear the path."}, \
{DJI_ERROR_FC_FLYTO_MODULE_CODE_UAV_MEETVFENCE,          "FlyTo exit: aircraft hit virtual fence boundary.", "Please adjust the virtual fence or the target location."}, \
{DJI_ERROR_FC_FLYTO_MODULE_CODE_UAV_APPROACH_GEOCAGE,    "FlyTo exit: aircraft approaching GeoFence boundary.", "Please adjust the GeoFence or choose a target within the boundary."}, \
{DJI_ERROR_FC_FLYTO_MODULE_CODE_USER_CONTROL_RC,               "FlyTo exit: operator took over control via RC.", NULL}, \
{DJI_ERROR_FC_FLYTO_MODULE_CODE_USER_REQUESTSTOP,              "FlyTo exit: mission stopped by user request.", NULL}, \
{DJI_ERROR_FC_FLYTO_MODULE_CODE_PREEMPTBY_LOWBATHOME,          "FlyTo exit: low battery, returning to home.", "Please replace the battery."}, \
{DJI_ERROR_FC_FLYTO_MODULE_CODE_PREEMPTBY_LOWBATLAND,          "FlyTo exit: low battery, forced landing.", "Please replace the battery."}, \
{DJI_ERROR_FC_FLYTO_MODULE_CODE_PREEMPTBY_ADSB,                "FlyTo exit: mission preempted by ADS-B collision avoidance.", "Please check surrounding airspace for traffic."}, \
{DJI_ERROR_FC_FLYTO_MODULE_CODE_PREEMPTBY_HIGHERPRIORITY_MIS,  "FlyTo exit: preempted by a higher-priority mission.", NULL}, \
/* FC motor slow rotation error message */\
{DJI_ERROR_FC_MOTOR_MODULE_CODE_SLOW_ROTATE_FAIL, "Motor slow rotation failed.", "Please check motor status."}, \
\
/* gimbal setparams module error message */ \
{DJI_ERROR_GIMBAL_SETPARAMS_MODULE_CODE_NO_CONTROL_SRC, "Gimbal control input source not set.", "Please set the control input source first."}, \
{DJI_ERROR_GIMBAL_SETPARAMS_MODULE_CODE_PARAM_NOT_FOUND, "Parameter ID does not exist.", "Please check the parameter ID validity."}, \
{DJI_ERROR_GIMBAL_SETPARAMS_MODULE_CODE_PARAM_LEN_ERR, "Parameter length is incorrect.", "Please check the parameter length."}, \
{DJI_ERROR_GIMBAL_SETPARAMS_MODULE_CODE_VALUE_BELOW_MIN, "Parameter value is below the minimum limit.", "Please increase the value to within the valid range."}, \
{DJI_ERROR_GIMBAL_SETPARAMS_MODULE_CODE_VALUE_ABOVE_MAX, "Parameter value exceeds the maximum limit.", "Please decrease the value to within the valid range."}, \
{DJI_ERROR_GIMBAL_SETPARAMS_MODULE_CODE_RESP_DATA_TOO_MANY, "Returned data exceeds buffer capacity.", "Please increase the buffer size."}, \
{DJI_ERROR_GIMBAL_SETPARAMS_MODULE_CODE_PARAM_INDEX_ERR, "Parameter table index is incorrect.", "Please check the parameter index validity."}, \
{DJI_ERROR_GIMBAL_SETPARAMS_MODULE_CODE_MUTEX_FAILED, "Memory mutex operation failed.", "Please try again."}, \

#define DJI_RETURN_CODE_OK                 DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS             /*!< Payload SDK return code represents as status is ok. */
#define DJI_RETURN_CODE_ERR_ALLOC          DJI_ERROR_SYSTEM_MODULE_CODE_MEMORY_ALLOC_FAILED /*!< Payload SDK return code represents as status alloc error. */
#define DJI_RETURN_CODE_ERR_TIMEOUT        DJI_ERROR_SYSTEM_MODULE_CODE_TIMEOUT             /*!< Payload SDK return code represents as status timeout error. */
#define DJI_RETURN_CODE_ERR_NOT_FOUND      DJI_ERROR_SYSTEM_MODULE_CODE_NOT_FOUND           /*!< Payload SDK return code represents as status not found error. */
#define DJI_RETURN_CODE_ERR_OUT_OF_RANGE   DJI_ERROR_SYSTEM_MODULE_CODE_OUT_OF_RANGE        /*!< Payload SDK return code represents as status out of range error. */
#define DJI_RETURN_CODE_ERR_PARAM          DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER   /*!< Payload SDK return code represents as status parameter error. */
#define DJI_RETURN_CODE_ERR_SYSTEM         DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR        /*!< Payload SDK return code represents as status system error. */
#define DJI_RETURN_CODE_ERR_BUSY           DJI_ERROR_SYSTEM_MODULE_CODE_BUSY                /*!< Payload SDK return code represents as status busy error. */
#define DJI_RETURN_CODE_ERR_UNSUPPORT      DJI_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT          /*!< Payload SDK return code represents as status nonsupport error. */
#define DJI_RETURN_CODE_ERR_UNKNOWN        DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN             /*!< Payload SDK return code represents as status unknown error. */

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
    DJI_ERROR_MODULE_WAYPOINT_V3,
    DJI_ERROR_MODULE_ERROR,
    DJI_ERROR_MODULE_SPZ,    /*!< Spotlight zoom module */
    DJI_ERROR_MODULE_FC_FLYTO, /*!< Flight controller FlyTo mission module */
    DJI_ERROR_MODULE_FC_MOTOR, /*!< Flight controller motor rotation module */
    DJI_ERROR_MODULE_PERCEPTION, /*!< Perception module */
    DJI_ERROR_MODULE_FILE_TRANSFER, /*!< File transfer module */
    DJI_ERROR_MODULE_CENTER_SUB, /*!< Center board subscription module */
    DJI_ERROR_MODULE_TM_PUSH, /*!< TM push subscription module */
    DJI_ERROR_MODULE_GIMBAL_SETPARAMS, /*!< Gimbal setparams module */
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
    DJI_ERROR_SYSTEM_MODULE_RAW_CODE_MODULE_INACTIVATED = 0xEE, /*!< Module is too not activated yet */
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
    DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_ERROR_CMD_IS_RUNNING = 0x08,
    DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_ERROR_IS_NOT_RUNNING = 0x09,
    DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_ERROR_CMD_INVALID = 0x0A,
    DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_ERROR_NO_GEAR = 0x0B,
    DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_ERROR_PACK_GIMBAL_MOUNTED = 0x0C,
    DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_ERROR_PACK_US_NOT_HEALTHY = 0x0D,
    DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_ERROR_PACK_HAS_PACKED = 0x0E,
    DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_ERROR_NOT_PACKED = 0x0F,
    DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_ERROR_PACK_NOT_SUPPORTED = 0x10,
    DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_ERROR_IN_HOVER = 0x13,
    DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_ERROR_NO_HORIZ_VEL_INFO = 0x15,
    DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_ERROR_ANG_NORMAL = 0x16,
    DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_ERROR_CRAFT_NEVER_CRASH = 0x17,
    DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_ERROR_DRONE_UNCTRLABLE = 0x18,
    DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_ERROR_CRAFT_NO_AVAILABLE_ESC = 0x19,
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
    DJI_ERROR_FC_HOME_LOCATION_MODULE_RAW_CODE_RTK_ENABLE_BUT_NOT_READY = 0x06,
    DJI_ERROR_FC_HOME_LOCATION_MODULE_RAW_CODE_RC_NOT_READY = 0x07,
    DJI_ERROR_FC_HOME_LOCATION_MODULE_RAW_CODE_RC_IN_NFZ = 0x08,
    DJI_ERROR_FC_HOME_LOCATION_MODULE_RAW_CODE_NONSUPPORT_CUR_HP_TYPE = 0x09,
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
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_ACK = 0x01,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_CANNOT_ENTER_PLAYBACK_IN_CUR_STATE = 0xB0,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_FILE_TRANSFER_BUSY = 0xC0,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_INVALID_TASK_INFORMATION = 0xD2,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_TASK_UPLOAD_ERROR = 0xD3,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_INVALID_REQUEST_PARAMETER = 0xD4,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_MAY_CROSS_RESTRICTED_AREA = 0xD5,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_EXCEEDED_INPUT_TIME_LIMIT = 0xD6,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_EXECUTING_HIGHER_PRIORITY_TASK = 0xD7,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_CANNOT_START_TASK_WEAK_GPS = 0xD8,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_CANNOT_START_TASK_VOLTAGE_ALARM = 0xD9,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_MISMATCH = 0xDA,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_DATA = 0xDB,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_TRY_LATER = 0xDC,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_NOT_IMPLEMENTED = 0xDD,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_CANCELLED = 0xDE,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_NOT_FOUND = 0xDF,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_UNSUPPORTED_COMMAND = 0xE0,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_TIMEOUT = 0xE1,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_RAM_ALLOCATION_FAILED = 0xE2,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_INVALID_COMMAND_PARAMETER = 0xE3,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_UNSUPPORTED_COMMAND_IN_CUR_STATE = 0xE4,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_CAMERA_TIME_NOT_SYNCHRONIZED = 0xE5,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_PARAMETER_SET_FAILED = 0xE6,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_PARAMETER_GET_FAILED = 0xE7,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_SD_CARD_MISSING = 0xE8,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_SD_CARD_FULL = 0xE9,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_SD_CARD_ERROR = 0xEA,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_SENSOR_ERROR = 0xEB,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_SYSTEM_ERROR = 0xEC,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_PARAMETER_TOTAL_TOO_LONG = 0xED,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_MODULE_INACTIVATED = 0xEE,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_USER_UNBOUND = 0xEF,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_FIRMWARE_DATA_NUM_DISCONTINUOUS = 0xF0,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_FIRMWARE_DATA_OVERLOAD_FLASH = 0xF1,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_FIRMWARE_VERIFICATION_ERROR = 0xF2,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_FLASH_ERASE_ERROR = 0xF3,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_FLASH_WRITE_ERROR = 0xF4,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_UPGRADE_STATUS_ERROR = 0xF5,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_FIRMWARE_TYPE_MISMATCH = 0xF6,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_WAITING_CLIENT_UPGRADE_STATUS = 0xF7,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_REMOTE_CONTROL_UNCONNECTED = 0xF8,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_MOTOR_NOT_STOPPED = 0xF9,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_HARDWARE_ERROR = 0xFA,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_INSUFFICIENT_ELECTRICITY = 0xFB,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_AIRCRAFT_UNCONNECTED = 0xFC,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_FLASH_IS_ERASING = 0xFD,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_CANNOT_UPGRADE_IN_CUR_STATE = 0xFE,
    DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_UNDEFINE_ERROR = 0xFF,
} E_DjiErrorCameraManagerModuleRawCode;

/**
 * @brief Raw error code of gimbal manager
 */
typedef enum {
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_ACK = 0x01,
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_INVALID_REQUEST_PARAMETER = 0xD4,
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_MISMATCH = 0xDA,
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_DATA = 0xDB,
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_TRY_LATER = 0xDC,
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_NOT_IMPLEMENTED = 0xDD,
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_CANCELLED = 0xDE,
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_NOT_FOUND = 0xDF,
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_UNSUPPORTED_COMMAND = 0xE0,
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_TIMEOUT = 0xE1,
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_RAM_ALLOCATION_FAILED = 0xE2,
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_INVALID_COMMAND_PARAMETER = 0xE3,
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_UNSUPPORTED_COMMAND_IN_CUR_STATE = 0xE4,
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_CAMERA_TIME_NOT_SYNCHRONIZED = 0xE5,
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_PARAMETER_SET_FAILED = 0xE6,
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_PARAMETER_GET_FAILED = 0xE7,
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_SD_CARD_MISSING = 0xE8,
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_SD_CARD_FULL = 0xE9,
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_SD_CARD_ERROR = 0xEA,
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_SENSOR_ERROR = 0xEB,
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_SYSTEM_ERROR = 0xEC,
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_PARAMETER_TOTAL_TOO_LONG = 0xED,
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_MODULE_INACTIVATED = 0xEE,
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_USER_UNBOUND = 0xEF,
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_FIRMWARE_DATA_NUM_DISCONTINUOUS = 0xF0,
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_FIRMWARE_DATA_OVERLOAD_FLASH = 0xF1,
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_FIRMWARE_VERIFICATION_ERROR = 0xF2,
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_FLASH_ERASE_ERROR = 0xF3,
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_FLASH_WRITE_ERROR = 0xF4,
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_UPGRADE_STATUS_ERROR = 0xF5,
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_FIRMWARE_TYPE_MISMATCH = 0xF6,
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_WAITING_CLIENT_UPGRADE_STATUS = 0xF7,
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_REMOTE_CONTROL_UNCONNECTED = 0xF8,
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_MOTOR_NOT_STOPPED = 0xF9,
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_HARDWARE_ERROR = 0xFA,
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_INSUFFICIENT_ELECTRICITY = 0xFB,
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_AIRCRAFT_UNCONNECTED = 0xFC,
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_FLASH_IS_ERASING = 0xFD,
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_CANNOT_UPGRADE_IN_CUR_STATE = 0xFE,
    DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_UNDEFINE_ERROR = 0xFF,
} E_DjiErrorGimbalManagerModuleRawCode;

/**
 * @brief Raw error code of gimbal setparams module.
 */
typedef enum {
    DJI_ERROR_GIMBAL_SETPARAMS_MODULE_RAW_CODE_NO_CONTROL_SRC = 0x01,
    DJI_ERROR_GIMBAL_SETPARAMS_MODULE_RAW_CODE_PARAM_NOT_FOUND = 0x03,
    DJI_ERROR_GIMBAL_SETPARAMS_MODULE_RAW_CODE_PARAM_LEN_ERR = 0x04,
    DJI_ERROR_GIMBAL_SETPARAMS_MODULE_RAW_CODE_VALUE_BELOW_MIN = 0x05,
    DJI_ERROR_GIMBAL_SETPARAMS_MODULE_RAW_CODE_VALUE_ABOVE_MAX = 0x06,
    DJI_ERROR_GIMBAL_SETPARAMS_MODULE_RAW_CODE_RESP_DATA_TOO_MANY = 0x07,
    DJI_ERROR_GIMBAL_SETPARAMS_MODULE_RAW_CODE_PARAM_INDEX_ERR = 0x09,
    DJI_ERROR_GIMBAL_SETPARAMS_MODULE_RAW_CODE_MUTEX_FAILED = 0x0A,
} E_DjiErrorGimbalSetparamsModuleRawCode;

/**
 * @brief Raw error code of spotlight zoom (SPZ) module.
 * Corresponds to E_DjiSpotlightZoomStatusMessage defined in dji_fc_define.h.
 */
typedef enum {
    DJI_ERROR_SPZ_MODULE_RAW_CODE_OK                       = 0,   /*!< Normal status */
    DJI_ERROR_SPZ_MODULE_RAW_CODE_TAR_TOO_NEAR             = 1,   /*!< Target too near */
    DJI_ERROR_SPZ_MODULE_RAW_CODE_TAR_TOO_FAR              = 2,   /*!< Target too far */
    DJI_ERROR_SPZ_MODULE_RAW_CODE_LOW_LIMIT                = 3,   /*!< Reached lower altitude limit */
    DJI_ERROR_SPZ_MODULE_RAW_CODE_HEIGHT_LIMIT             = 4,   /*!< Reached upper altitude limit */
    DJI_ERROR_SPZ_MODULE_RAW_CODE_RADIUS_LIMIT             = 5,   /*!< Reached radius limit */
    DJI_ERROR_SPZ_MODULE_RAW_CODE_BOUNDARY_LIMIT           = 6,   /*!< Approaching flight restriction zone */
    DJI_ERROR_SPZ_MODULE_RAW_CODE_GIMBAL_LIMITED           = 7,   /*!< Gimbal reached tracking composition limit */
    DJI_ERROR_SPZ_MODULE_RAW_CODE_YAW_LIMITED              = 8,   /*!< Yaw reached surround mode max limit (±80°) */
    DJI_ERROR_SPZ_MODULE_RAW_CODE_TOO_CLOSE_TO_OBSTACLE    = 9,   /*!< Too close to obstacle */
    DJI_ERROR_SPZ_MODULE_RAW_CODE_GPS_TO_RTK               = 10,  /*!< Positioning switched GPS->RTK, surround paused */
    DJI_ERROR_SPZ_MODULE_RAW_CODE_RTK_TO_GPS               = 11,  /*!< Positioning switched RTK->GPS */
    DJI_ERROR_SPZ_MODULE_RAW_CODE_OBSTACLE_AVOID_INVALID   = 12,  /*!< Obstacle avoidance invalid */
    DJI_ERROR_SPZ_MODULE_RAW_CODE_ZOOM_TRACKING_LIMITED    = 13,  /*!< Reached zoom range limit */
    DJI_ERROR_SPZ_MODULE_RAW_CODE_TARGET_LOST              = 14,  /*!< Tracking target lost */
    DJI_ERROR_SPZ_MODULE_RAW_CODE_GPS_WEAK                 = 15,  /*!< GPS signal weak */
    DJI_ERROR_SPZ_MODULE_RAW_CODE_EXIT_NO_ERROR            = 160, /*!< Exited normally */
    DJI_ERROR_SPZ_MODULE_RAW_CODE_EXIT_UNSUPPORTED_PAYLOAD = 161, /*!< Unsupported payload type */
    DJI_ERROR_SPZ_MODULE_RAW_CODE_EXIT_UNSUPPORTED_CAMERA_MODE = 162, /*!< Unsupported camera mode */
    DJI_ERROR_SPZ_MODULE_RAW_CODE_EXIT_INVALID_CMD         = 163, /*!< Invalid command */
    DJI_ERROR_SPZ_MODULE_RAW_CODE_EXIT_GPS_INVALID         = 164, /*!< Positioning invalid */
    DJI_ERROR_SPZ_MODULE_RAW_CODE_EXIT_NO_IN_AIR           = 165, /*!< Aircraft not airborne */
    DJI_ERROR_SPZ_MODULE_RAW_CODE_EXIT_UNSUPPORTED_CTRL_MODE = 166, /*!< Wrong flight control mode */
    DJI_ERROR_SPZ_MODULE_RAW_CODE_EXIT_UNSUPPORTED_FLIGHT_MODE = 167, /*!< Unavailable in current flight mode */
    DJI_ERROR_SPZ_MODULE_RAW_CODE_EXIT_RC_LOST             = 168, /*!< RC or video link lost */
    DJI_ERROR_SPZ_MODULE_RAW_CODE_EXIT_STATUS_ERR          = 254, /*!< Internal error (dbus/IMU/gimbal/camera) */
    DJI_ERROR_SPZ_MODULE_RAW_CODE_EXIT_INNER_UNKNOW_ERROR  = 255, /*!< Unknown internal error */
} E_DjiErrorSpzModuleRawCode;

/**
 * @brief Raw error code of flight controller FlyTo (SetModeStartMission) module.
 * These are the error_code values returned in T_DjiFlightControllerStartMissionRsp.
 * CMDER_MODE_DBUS_NULLPTR (0x0001) is sanitized as "internal aircraft communication error".
 */
typedef enum {
    /* Common check errors — reject/exit mission */
    DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_INTERNAL_ERROR     = 0x0001, /*!< Internal aircraft communication error */
    DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_BAD_GPS_SIGNAL       = 0x0002, /*!< GPS signal too weak to execute mission */
    DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_ABSPOS_INVALID       = 0x0003, /*!< Positioning completely unavailable */
    DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_PLANNING_FAIL        = 0x0004, /*!< Route planning failed */
    DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_HOME_NOTSET          = 0x0005, /*!< Home point not set */
    DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_RC_LOST              = 0x0006, /*!< Remote controller signal lost */
    DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_TAKEOFF_ON_SHIP      = 0x0007, /*!< Auto takeoff on a vessel is prohibited */

    /* Common check warnings */
    DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_SOMEWAYRUNOUT        = 0x0011, /*!< Insufficient energy to complete mission */
    DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_SWITCH_PLANNING_ALGO = 0x0012, /*!< Planning algorithm was switched during mission */
    DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_HEIGHT_EXCEED_LIMIT  = 0x0013, /*!< Target altitude exceeds flight altitude limit */

    /* Static check warnings */
    DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_GOAL_MEETHORLIM        = 0x0201, /*!< Target point is inside a horizontal no-fly zone */
    DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_GOAL_MEETHORLIM_RADIUS = 0x0202, /*!< Target point exceeds user-defined distance limit */
    DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_GOAL_MEETVERLIM        = 0x0203, /*!< Target point is inside a vertical no-fly zone */
    DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_GOAL_MEETVERLIM_TOP    = 0x0204, /*!< Target altitude exceeds user-defined altitude ceiling */
    DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_GOAL_MEETVERLIM_BOTTOM = 0x0205, /*!< Target altitude is below minimum flight altitude */

    /* Departure wayline errors */
    DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_DEPARTURE_WAYLINE_ERROR          = 0x0300, /*!< Departure wayline execution failed */
    DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_DEPARTURE_WAYLINE_INIT           = 0x0301, /*!< Departure wayline initialization failed */
    DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_DEPARTURE_WAYLINE_INVALID        = 0x0302, /*!< Departure wayline is unavailable */
    DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_DEPARTURE_WAYLINE_NOT_REACHABLE  = 0x0303, /*!< Departure point unreachable (no-fly zone or obstacle) */
    DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_DEPARTURE_WAYLINE_PLANNING_FAIL  = 0x0304, /*!< Departure wayline planning failed */
    DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_DEPARTURE_WAYLINE_RC_SWITCH_MODE = 0x0305, /*!< RC is not in P-mode */
    DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_DEPARTURE_WAYLINE_RTK_TYPE_ERR   = 0x0306, /*!< BeiDou-only aircraft does not support departure wayline */

    /* Static check errors — reject mission */
    DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_PREVENT_MOTOR_ERROR    = 0x0401, /*!< Motor pre-arm check failed */
    DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_GOALINVALID             = 0x0402, /*!< Target point out of planning range or invalid coordinates */
    DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_MAXFLYVEL_INVALID       = 0x0403, /*!< Maximum flight speed parameter is invalid */
    DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_VERSION_INVALID         = 0x0404, /*!< Protocol version mismatch */
    DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_STATEMACHINE_INVALID    = 0x0405, /*!< Mission state machine busy; retry after current task finishes */
    DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_MEA_INVALID             = 0x0406, /*!< Minimum enroute altitude (MEA) too low (must be >=20 m for M220) */

    /* Dynamic check errors — exit mission in flight */
    DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_UAV_MEETHORLIM          = 0x0601, /*!< Aircraft entered horizontal no-fly zone */
    DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_UAV_MEETHORLIM_RADIUS   = 0x0602, /*!< Aircraft exceeded user-defined distance limit */
    DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_UAV_MEETVERLIM          = 0x0603, /*!< Aircraft entered vertical no-fly zone */
    DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_UAV_MEETVERLIM_TOP      = 0x0604, /*!< Aircraft reached user-defined altitude ceiling */
    DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_UAV_MEETVERLIM_BOTTOM   = 0x0605, /*!< Aircraft reached minimum flight altitude floor */
    DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_TAKEOFF_FAIL            = 0x0606, /*!< Takeoff failed */
    DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_GOAL_BLOCKEDBY_OBSTACLE = 0x0607, /*!< Target point blocked by obstacle */
    DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_OBSTACLETOOCLOSE        = 0x0608, /*!< Obstacle too close; emergency brake */
    DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_ROUTE_BLOCKED           = 0x0609, /*!< Route blocked; replanning failed */
    DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_UAV_MEETVFENCE          = 0x060A, /*!< Aircraft hit virtual fence boundary */
    DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_UAV_APPROACH_GEOCAGE    = 0x060B, /*!< Aircraft approaching GeoFence boundary */

    /* External module interruptions */
    DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_USER_CONTROL_RC               = 0x0801, /*!< Operator took over control via RC */
    DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_USER_REQUESTSTOP              = 0x0802, /*!< Mission stopped by user request */
    DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_PREEMPTBY_LOWBATHOME          = 0x0803, /*!< Low battery: returning to home */
    DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_PREEMPTBY_LOWBATLAND          = 0x0804, /*!< Low battery: forced landing */
    DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_PREEMPTBY_ADSB                = 0x0805, /*!< Mission preempted by ADS-B collision avoidance */
    DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_PREEMPTBY_HIGHERPRIORITY_MIS  = 0x0806, /*!< Mission preempted by a higher-priority mission */
} E_DjiErrorFcFlytoModuleRawCode;

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

typedef enum {
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_SUCCESS = 0x0000,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_MISSION_ID_NOT_EXIST = 1,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_WAYLINE_INFO_ERROR = 2,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_WPMZ_FILE_VERSION_NOT_MATCH = 3,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_WPMZ_FILE_LOAD_ERROR = 4,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_NO_BREAK_INFO = 5,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_CMD_INVALID = 6,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_CANNOT_START_WAYLINE_WHEN_WAYLINE_RUNNING = 257,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_CANNOT_BREAK_WAYLINE_IN_CUR_STATE = 258,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_CANNOT_STOP_WAYLINE_WHEN_WAYLINE_NOT_RUNNING = 259,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_CANNOT_BREAK_WAYLINE_WHEN_WAYLINE_NOT_RUNNING = 260,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_CANNOT_REQUEST_DRONE_CONTROL = 261,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_CANNOT_RESUME_WAYLINE_IN_CUR_STATE = 262,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_HEIGHT_LIMIT = 513,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_RADIUS_LIMIT = 514,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_CROSS_FLYLIMIT_AERA = 515,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_LOW_LIMIT = 516,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_OBSTACAL_STOP = 517,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_RTK_DISCONNECT = 518,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_BOUNDARY_LIMIT = 519,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_RC_PITCH_ROLL_BREAK = 520,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_AIRPORT_HEIGHT_LIMIT = 521,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_REQUEST_TAKEOFF_FAIL = 522,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_AUTOTAKEOFF_RUN_FAIL = 523,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_REQUEST_WAYLINE_FAIL = 524,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_AGRO_PLAN_FAIL = 525,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_REQUEST_QUICK_TAKEOFF_ASSIST_FAIL = 526,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_QUICK_TAKEOFF_ASSIST_RUN_FAIL = 527,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_VFENCE_LIMIT = 528,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_GPS_INVALID = 769,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_CANNOT_START_AT_CURRENT_RC_MODE = 770,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_HOME_POINT_NOT_RECORDED = 771,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_LOWER_BATTERY = 772,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_RETURN_HOME = 773,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ADSB_ERROR = 774,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_RC_LOST = 775,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_RTK_NOT_READY = 776,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_DRONE_IS_MOVING = 777,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_DRONE_ON_GROUND_MOTOR_ON = 778,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_SURFACE_FOLLOW_CAMERA_INVALID = 779,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_SURFACE_FOLLOW_HEIGHT_INVALID = 780,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_SURFACE_FOLLOW_MAP_WRONG = 781,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_HOMEPOINT_NOT_MATCH_RTK = 782,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_STRONG_WIND_GOHOME = 784,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_DRONE_CRITICAL_ERROR = 1023,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_CANNOT_FIND_PAYLOAD = 1025,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_EXECUTION_FAILED = 1026,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_FARM_NO_PESTICIDE = 1027,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_RADAR_DISCONNECT = 1028,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_USER_EXIT = 1281,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_USER_BREAK = 1282,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_USER_SET_GOHOME = 1283,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_USER_AGRO_PLANNER_STATE_CHANGE = 1284,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_USER_SWITCH_RC_MODE = 1285,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_INIT_FAIL = 1536,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_JOB_EXIT_BUT_MIS_RUNNING = 1537,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_ON_GROUND_MOTOR_ON_CANNOT_GO = 1538,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_INVALID_START_INDEX_OR_PROG = 1539,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_INVALID_CSYS_MODE = 1540,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_INVALID_HEIGHT_MODE = 1541,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_INVALID_FLY_WP_MODE = 1542,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_INVALID_YAW_MODE = 1543,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_INVALID_TURN_DIR_MODE = 1544,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_INVALID_WP_TYPE = 1545,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_FIR_LAS_WP_TYPE_ERROR = 1546,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_GLOB_VEL_OUT_OF_RANGE = 1547,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_WP_NUM_OUT_OF_RANGE = 1548,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_LAT_LONG_OUT_OF_RANGE = 1549,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_DAMP_DIS_OUT_OF_RANGE = 1550,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_MAX_VEL_OUT_OF_RANGE = 1551,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_VEL_OUT_OF_RANGE = 1552,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_WP_YAW_OUT_OF_RANGE = 1553,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_INVALID_YAW_MODE_IN_VERT_SEGM = 1554,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_WP_BREAK_INFO_MISSION_ID_CHANGED = 1555,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_WP_BREAK_INFO_PROGRESS_OUT_OF_RANGE = 1556,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_WP_BREAK_INFO_INVALID_MISSION_STATE = 1557,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_WP_BREAK_INFO_WP_INDEX_OUT_OF_RANGE = 1558,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_BREAK_LAT_LONG_OUT_OF_RANGE = 1559,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_BREAK_INFO_WP_YAW_OUT_OF_RANGE = 1560,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_INVALID_BREAK_INFO_FLAG = 1561,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_GET_TRAJ_INFO_FAILED = 1562,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_GENERATE_FAIL = 1563,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_LIB_RUN_FAIL = 1564,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_LIB_EMERGENCY_BRAKE = 1565,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_ACTION_NOT_FOUND = 1588,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_ACTION_INDEX_REPEATED = 1591,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_ACTION_INFO_SIZE_TOO_LONG_OR_TOO_SHORT = 1592,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_ACTION_TREE_EMPTY = 1593,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_ACTION_TREE_LAYER_EMPTY = 1594,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_ACTION_ID_REPEATED = 1595,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_ACTION_NODE_CHILDREN_NUM_LT_2 = 1596,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_ACTION_INDEX_OUT_OF_RANGE = 1597,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_ACTION_ID_IS_65535 = 1598,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_ACTION_NODE_CHILDNUM_SUM_NOT_EQ_NEXT_LAYER_SIZE = 1599,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_ACTION_TREE_LAYER_NUM_TOO_MORE = 1600,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_ACTION_TREE_LAYER_NUM_TOO_LESS = 1601,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_ACTION_GROUP_NUM_OUT_OF_RANGE = 1602,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_ACTION_GROUP_VALID_RANGE_ERROR = 1603,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_ACTION_TREE_ROOT_STATUS_INVALID = 1604,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_ACTION_TREE_NODE_STATUS_INVALID = 1605,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_BREAK_INFO_ACTION_GROUP_ID_OUT_OF_RANGE = 1606,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_ACTION_STATUS_TREE_SIZE_ERROR = 1607,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_BREAK_INFO_TRIGGER_RUN_RESULT_INVALID = 1608,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_BREAK_INFO_ACTION_GROUP_ID_REPEATED = 1609,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_BREAK_INFO_ACTION_LOCATION_REPEATED = 1610,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_BREAK_INFO_ACTION_LOCATION_OUT_OF_RANGE = 1611,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_RESUME_ID_NOT_IN_BREAK_INFO = 1612,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_RESUME_INFO_MODIFY_ACTION_STATUS_FROM_NO_INTERRUPT_TO_INTERRUPT = 1613,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_ACTION_RESUME_FAIL_FOR_INVALID_RESUME_INFO = 1614,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTUATOR_COMMON_ACTUATOR_NOT_FOUND = 1634,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRIGGER_NOT_FOUND = 1649,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRIGGER_SINGLE_TIME_CHECK_FAIL = 1650,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_CANNOT_TAKEOFF_ON_SHIP = 263,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_DJI_TARGET_NOT_REACHABLE = 529,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_DJI_TRAJ_PLAN_FAILED = 530,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_DEPARTURE_WAYLINE_FAIL = 531,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_DEPARTURE_WAYLINE_INIT_FAIL = 532,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_DEPARTURE_WAYLINE_INVALID = 533,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRANSPORT_DIVERT = 783,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_DOCK_MOVE_GOHOME = 786,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_HEADING_LEVEL_POOR = 787,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_JAMMING_GOHOME = 788,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_WIND_SPD = 789,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_BUSY_IMU_CHANGED = 790,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_RAIN_DETECT_VALID = 791,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_WP_ARM1_ERROR = 800,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_WP_ARM2_ERROR = 801,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_WP_ARM3_ERROR = 802,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_WP_ARM4_ERROR = 803,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_WP_RADAR_PEDEX_WARNING = 804,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_WP_OVERLOAD = 805,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_WP_HOIST_NOT_READY = 806,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_WP_HOIST_SELF_CHECK_ERR = 807,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_WP_PREVENT_MOTOR_START = 808,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_WP_EST_BAT_OVERTEMP_ERROR = 816,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_PAYLOAD_NOT_HOIST = 1040,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_DJI_INNER_ERROR = 65534,
    DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_DJI_UNKNOWN_ERROR = 65535,
} E_DjiErrorWaypoint3ModuleRawCode;

/**
 * @brief Raw error code of waypoint v3 breakpoint query (cmd_set=0x22, cmd_id=0xAF).
 * error_code field in dji_fc2_get_wp3_query_breakpoint_info_rsp.
 * Values from duss description (no enum defined, magic numbers documented inline).
 */
typedef enum {
    DJI_ERROR_WAYPOINT_V3_BREAKPOINT_QUERY_RAW_CODE_HAS_BREAKPOINT = 0x00000000,
    DJI_ERROR_WAYPOINT_V3_BREAKPOINT_QUERY_RAW_CODE_MISSION_NOT_EXIST = 0x00010001,
    DJI_ERROR_WAYPOINT_V3_BREAKPOINT_QUERY_RAW_CODE_NO_BREAKPOINT = 0x00010004,
} E_DjiErrorWaypointV3BreakPointQueryRawCode;

/**
 * @brief Raw error code of FC motor slow rotation (cmd_set=0x22, cmd_id=0x5E).
 */
typedef enum {
    DJI_ERROR_FC_MOTOR_MODULE_RAW_CODE_SLOW_ROTATE_OK = 0x00,
    DJI_ERROR_FC_MOTOR_MODULE_RAW_CODE_SLOW_ROTATE_FAIL = 0x01,
} E_DjiErrorFcMotorModuleRawCode;

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
    DJI_ERROR_FC_ACTION_MODULE_ERROR_CMD_IS_RUNNING= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_ACTION, DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_ERROR_CMD_IS_RUNNING),
    DJI_ERROR_FC_ACTION_MODULE_ERROR_IS_NOT_RUNNING= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_ACTION, DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_ERROR_IS_NOT_RUNNING),
    DJI_ERROR_FC_ACTION_MODULE_ERROR_CMD_INVALID= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_ACTION, DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_ERROR_CMD_INVALID),
    DJI_ERROR_FC_ACTION_MODULE_ERROR_NO_GEAR= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_ACTION, DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_ERROR_NO_GEAR),
    DJI_ERROR_FC_ACTION_MODULE_ERROR_PACK_GIMBAL_MOUNTED= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_ACTION, DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_ERROR_PACK_GIMBAL_MOUNTED),
    DJI_ERROR_FC_ACTION_MODULE_ERROR_PACK_US_NOT_HEALTHY= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_ACTION, DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_ERROR_PACK_US_NOT_HEALTHY),
    DJI_ERROR_FC_ACTION_MODULE_ERROR_PACK_HAS_PACKED= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_ACTION, DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_ERROR_PACK_HAS_PACKED),
    DJI_ERROR_FC_ACTION_MODULE_ERROR_NOT_PACKED= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_ACTION, DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_ERROR_NOT_PACKED),
    DJI_ERROR_FC_ACTION_MODULE_ERROR_PACK_NOT_SUPPORTED= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_ACTION, DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_ERROR_PACK_NOT_SUPPORTED),
    DJI_ERROR_FC_ACTION_MODULE_ERROR_IN_HOVER= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_ACTION, DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_ERROR_IN_HOVER),
    DJI_ERROR_FC_ACTION_MODULE_ERROR_NO_HORIZ_VEL_INFO= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_ACTION, DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_ERROR_NO_HORIZ_VEL_INFO),
    DJI_ERROR_FC_ACTION_MODULE_ERROR_ANG_NORMAL= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_ACTION, DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_ERROR_ANG_NORMAL),
    DJI_ERROR_FC_ACTION_MODULE_ERROR_CRAFT_NEVER_CRASH= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_ACTION, DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_ERROR_CRAFT_NEVER_CRASH),
    DJI_ERROR_FC_ACTION_MODULE_ERROR_DRONE_UNCTRLABLE= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_ACTION, DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_ERROR_DRONE_UNCTRLABLE),
    DJI_ERROR_FC_ACTION_MODULE_ERROR_CRAFT_NO_AVAILABLE_ESC= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_ACTION, DJI_ERROR_FC_ACTION_MODULE_RAW_CODE_ERROR_CRAFT_NO_AVAILABLE_ESC),

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
    DJI_ERROR_FC_HOME_LOCATION_MODULE_RTK_ENABLE_BUT_NOT_READY= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_HOME_LOCATION, DJI_ERROR_FC_HOME_LOCATION_MODULE_RAW_CODE_RTK_ENABLE_BUT_NOT_READY),
    DJI_ERROR_FC_HOME_LOCATION_MODULE_RC_NOT_READY= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_HOME_LOCATION, DJI_ERROR_FC_HOME_LOCATION_MODULE_RAW_CODE_RC_NOT_READY),
    DJI_ERROR_FC_HOME_LOCATION_MODULE_RC_IN_NFZ= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_HOME_LOCATION, DJI_ERROR_FC_HOME_LOCATION_MODULE_RAW_CODE_RC_IN_NFZ),
    DJI_ERROR_FC_HOME_LOCATION_MODULE_NONSUPPORT_CUR_HP_TYPE= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_HOME_LOCATION, DJI_ERROR_FC_HOME_LOCATION_MODULE_RAW_CODE_NONSUPPORT_CUR_HP_TYPE),

    /* Flight controller emergency stop motor errors */
    DJI_ERROR_FC_EMERGENCY_STOP_MOTOR_MODULE_VERSION_NOT_MATCH= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_EMERGENCY_STOP_MOTOR, DJI_ERROR_FC_EMERGENCY_STOP_MOTOR_MODULE_RAW_CODE_VERSION_NOT_MATCH),
    DJI_ERROR_FC_EMERGENCY_STOP_MOTOR_MODULE_CMD_INVALID= DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_EMERGENCY_STOP_MOTOR, DJI_ERROR_FC_EMERGENCY_STOP_MOTOR_MODULE_RAW_CODE_CMD_INVALID),

    /* Camera manager errors */
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_ACK = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_ACK),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_CANNOT_ENTER_PLAYBACK_IN_CUR_STATE = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_CANNOT_ENTER_PLAYBACK_IN_CUR_STATE),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_FILE_TRANSFER_BUSY = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_FILE_TRANSFER_BUSY),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_INVALID_TASK_INFORMATION = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_INVALID_TASK_INFORMATION),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_TASK_UPLOAD_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_TASK_UPLOAD_ERROR),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_INVALID_REQUEST_PARAMETER = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_INVALID_REQUEST_PARAMETER),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_MAY_CROSS_RESTRICTED_AREA = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_MAY_CROSS_RESTRICTED_AREA),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_EXCEEDED_INPUT_TIME_LIMIT = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_EXCEEDED_INPUT_TIME_LIMIT),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_EXECUTING_HIGHER_PRIORITY_TASK = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_EXECUTING_HIGHER_PRIORITY_TASK),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_CANNOT_START_TASK_WEAK_GPS = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_CANNOT_START_TASK_WEAK_GPS),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_CANNOT_START_TASK_VOLTAGE_ALARM = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_CANNOT_START_TASK_VOLTAGE_ALARM),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_MISMATCH = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_MISMATCH),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_DATA = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_DATA),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_TRY_LATER = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_TRY_LATER),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_NOT_IMPLEMENTED = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_NOT_IMPLEMENTED),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_CANCELLED = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_CANCELLED),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_NOT_FOUND = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_NOT_FOUND),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_UNSUPPORTED_COMMAND),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_TIMEOUT = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_TIMEOUT),
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
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_USER_UNBOUND = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_USER_UNBOUND),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_FIRMWARE_DATA_NUM_DISCONTINUOUS = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_FIRMWARE_DATA_NUM_DISCONTINUOUS),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_FIRMWARE_DATA_OVERLOAD_FLASH = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_FIRMWARE_DATA_OVERLOAD_FLASH),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_FIRMWARE_VERIFICATION_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_FIRMWARE_VERIFICATION_ERROR),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_FLASH_ERASE_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_FLASH_ERASE_ERROR),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_FLASH_WRITE_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_FLASH_WRITE_ERROR),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UPGRADE_STATUS_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_UPGRADE_STATUS_ERROR),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_FIRMWARE_TYPE_MISMATCH = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_FIRMWARE_TYPE_MISMATCH),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_WAITING_CLIENT_UPGRADE_STATUS = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_WAITING_CLIENT_UPGRADE_STATUS),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_REMOTE_CONTROL_UNCONNECTED = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_REMOTE_CONTROL_UNCONNECTED),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_MOTOR_NOT_STOPPED = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_MOTOR_NOT_STOPPED),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_HARDWARE_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_HARDWARE_ERROR),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_INSUFFICIENT_ELECTRICITY = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_INSUFFICIENT_ELECTRICITY),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_AIRCRAFT_UNCONNECTED = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_AIRCRAFT_UNCONNECTED),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_FLASH_IS_ERASING = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_FLASH_IS_ERASING),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_CANNOT_UPGRADE_IN_CUR_STATE = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_CANNOT_UPGRADE_IN_CUR_STATE),
    DJI_ERROR_CAMERA_MANAGER_MODULE_CODE_UNDEFINE_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_CAMERA_MANAGER, DJI_ERROR_CAMERA_MANAGER_MODULE_RAW_CODE_UNDEFINE_ERROR),


    /* Gimbal manager errors */
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_ACK = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_ACK),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_INVALID_REQUEST_PARAMETER = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_INVALID_REQUEST_PARAMETER),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_MISMATCH = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_MISMATCH),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_DATA = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_DATA),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_TRY_LATER = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_TRY_LATER),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_NOT_IMPLEMENTED = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_NOT_IMPLEMENTED),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_CANCELLED = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_CANCELLED),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_NOT_FOUND = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_NOT_FOUND),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_UNSUPPORTED_COMMAND = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_UNSUPPORTED_COMMAND),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_TIMEOUT = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_TIMEOUT),
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
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_USER_UNBOUND = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_USER_UNBOUND),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_FIRMWARE_DATA_NUM_DISCONTINUOUS = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_FIRMWARE_DATA_NUM_DISCONTINUOUS),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_FIRMWARE_DATA_OVERLOAD_FLASH = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_FIRMWARE_DATA_OVERLOAD_FLASH),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_FIRMWARE_VERIFICATION_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_FIRMWARE_VERIFICATION_ERROR),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_FLASH_ERASE_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_FLASH_ERASE_ERROR),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_FLASH_WRITE_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_FLASH_WRITE_ERROR),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_UPGRADE_STATUS_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_UPGRADE_STATUS_ERROR),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_FIRMWARE_TYPE_MISMATCH = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_FIRMWARE_TYPE_MISMATCH),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_WAITING_CLIENT_UPGRADE_STATUS = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_WAITING_CLIENT_UPGRADE_STATUS),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_REMOTE_CONTROL_UNCONNECTED = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_REMOTE_CONTROL_UNCONNECTED),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_MOTOR_NOT_STOPPED = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_MOTOR_NOT_STOPPED),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_HARDWARE_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_HARDWARE_ERROR),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_INSUFFICIENT_ELECTRICITY = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_INSUFFICIENT_ELECTRICITY),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_AIRCRAFT_UNCONNECTED = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_AIRCRAFT_UNCONNECTED),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_FLASH_IS_ERASING = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_FLASH_IS_ERASING),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_CANNOT_UPGRADE_IN_CUR_STATE = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_CANNOT_UPGRADE_IN_CUR_STATE),
    DJI_ERROR_GIMBAL_MANAGER_MODULE_CODE_UNDEFINE_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_MANAGER, DJI_ERROR_GIMBAL_MANAGER_MODULE_RAW_CODE_UNDEFINE_ERROR),

	/* gimbal setparams module error code */
	DJI_ERROR_GIMBAL_SETPARAMS_MODULE_CODE_VALUE_ABOVE_MAX = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_SETPARAMS, DJI_ERROR_GIMBAL_SETPARAMS_MODULE_RAW_CODE_VALUE_ABOVE_MAX),
	DJI_ERROR_GIMBAL_SETPARAMS_MODULE_CODE_VALUE_BELOW_MIN = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_SETPARAMS, DJI_ERROR_GIMBAL_SETPARAMS_MODULE_RAW_CODE_VALUE_BELOW_MIN),
	DJI_ERROR_GIMBAL_SETPARAMS_MODULE_CODE_PARAM_NOT_FOUND = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_SETPARAMS, DJI_ERROR_GIMBAL_SETPARAMS_MODULE_RAW_CODE_PARAM_NOT_FOUND),
	DJI_ERROR_GIMBAL_SETPARAMS_MODULE_CODE_PARAM_LEN_ERR = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_SETPARAMS, DJI_ERROR_GIMBAL_SETPARAMS_MODULE_RAW_CODE_PARAM_LEN_ERR),
	DJI_ERROR_GIMBAL_SETPARAMS_MODULE_CODE_NO_CONTROL_SRC = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_SETPARAMS, DJI_ERROR_GIMBAL_SETPARAMS_MODULE_RAW_CODE_NO_CONTROL_SRC),
	DJI_ERROR_GIMBAL_SETPARAMS_MODULE_CODE_RESP_DATA_TOO_MANY = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_SETPARAMS, DJI_ERROR_GIMBAL_SETPARAMS_MODULE_RAW_CODE_RESP_DATA_TOO_MANY),
	DJI_ERROR_GIMBAL_SETPARAMS_MODULE_CODE_PARAM_INDEX_ERR = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_SETPARAMS, DJI_ERROR_GIMBAL_SETPARAMS_MODULE_RAW_CODE_PARAM_INDEX_ERR),
	DJI_ERROR_GIMBAL_SETPARAMS_MODULE_CODE_MUTEX_FAILED = DJI_ERROR_CODE(DJI_ERROR_MODULE_GIMBAL_SETPARAMS, DJI_ERROR_GIMBAL_SETPARAMS_MODULE_RAW_CODE_MUTEX_FAILED),

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

    /* Waypoint v3 total errors */
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_COMMON_SUCCESS = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_SUCCESS),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_MISSION_ID_NOT_EXIST = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_MISSION_ID_NOT_EXIST),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_WAYLINE_INFO_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_WAYLINE_INFO_ERROR),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_WPMZ_FILE_VERSION_NOT_MATCH = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_WPMZ_FILE_VERSION_NOT_MATCH),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_WPMZ_FILE_LOAD_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_WPMZ_FILE_LOAD_ERROR),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_NO_BREAK_INFO = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_NO_BREAK_INFO),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_CMD_INVALID = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_CMD_INVALID),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_CANNOT_START_WAYLINE_WHEN_WAYLINE_RUNNING = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_CANNOT_START_WAYLINE_WHEN_WAYLINE_RUNNING),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_CANNOT_BREAK_WAYLINE_IN_CUR_STATE = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_CANNOT_BREAK_WAYLINE_IN_CUR_STATE),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_CANNOT_STOP_WAYLINE_WHEN_WAYLINE_NOT_RUNNING = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_CANNOT_STOP_WAYLINE_WHEN_WAYLINE_NOT_RUNNING),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_CANNOT_BREAK_WAYLINE_WHEN_WAYLINE_NOT_RUNNING = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_CANNOT_BREAK_WAYLINE_WHEN_WAYLINE_NOT_RUNNING),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_CANNOT_REQUEST_DRONE_CONTROL = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_CANNOT_REQUEST_DRONE_CONTROL),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_CANNOT_RESUME_WAYLINE_IN_CUR_STATE = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_CANNOT_RESUME_WAYLINE_IN_CUR_STATE),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_HEIGHT_LIMIT = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_HEIGHT_LIMIT),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_RADIUS_LIMIT = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_RADIUS_LIMIT),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_CROSS_FLYLIMIT_AERA = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_CROSS_FLYLIMIT_AERA),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_LOW_LIMIT = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_LOW_LIMIT),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_OBSTACAL_STOP = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_OBSTACAL_STOP),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_RTK_DISCONNECT = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_RTK_DISCONNECT),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_BOUNDARY_LIMIT = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_BOUNDARY_LIMIT),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_RC_PITCH_ROLL_BREAK = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_RC_PITCH_ROLL_BREAK),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_AIRPORT_HEIGHT_LIMIT = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_AIRPORT_HEIGHT_LIMIT),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_REQUEST_TAKEOFF_FAIL = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_REQUEST_TAKEOFF_FAIL),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_AUTOTAKEOFF_RUN_FAIL = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_AUTOTAKEOFF_RUN_FAIL),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_REQUEST_WAYLINE_FAIL = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_REQUEST_WAYLINE_FAIL),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_AGRO_PLAN_FAIL = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_AGRO_PLAN_FAIL),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_REQUEST_QUICK_TAKEOFF_ASSIST_FAIL = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_REQUEST_QUICK_TAKEOFF_ASSIST_FAIL),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_QUICK_TAKEOFF_ASSIST_RUN_FAIL = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_QUICK_TAKEOFF_ASSIST_RUN_FAIL),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_VFENCE_LIMIT = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_VFENCE_LIMIT),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_GPS_INVALID = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_GPS_INVALID),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_CANNOT_START_AT_CURRENT_RC_MODE = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_CANNOT_START_AT_CURRENT_RC_MODE),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_HOME_POINT_NOT_RECORDED = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_HOME_POINT_NOT_RECORDED),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_LOWER_BATTERY = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_LOWER_BATTERY),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_RETURN_HOME = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_RETURN_HOME),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ADSB_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ADSB_ERROR),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_RC_LOST = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_RC_LOST),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_RTK_NOT_READY = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_RTK_NOT_READY),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_DRONE_IS_MOVING = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_DRONE_IS_MOVING),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_DRONE_ON_GROUND_MOTOR_ON = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_DRONE_ON_GROUND_MOTOR_ON),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_SURFACE_FOLLOW_CAMERA_INVALID = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_SURFACE_FOLLOW_CAMERA_INVALID),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_SURFACE_FOLLOW_HEIGHT_INVALID = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_SURFACE_FOLLOW_HEIGHT_INVALID),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_SURFACE_FOLLOW_MAP_WRONG = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_SURFACE_FOLLOW_MAP_WRONG),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_HOMEPOINT_NOT_MATCH_RTK = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_HOMEPOINT_NOT_MATCH_RTK),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_STRONG_WIND_GOHOME = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_STRONG_WIND_GOHOME),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_DRONE_CRITICAL_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_DRONE_CRITICAL_ERROR),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_CANNOT_FIND_PAYLOAD = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_CANNOT_FIND_PAYLOAD),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_EXECUTION_FAILED = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_EXECUTION_FAILED),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_FARM_NO_PESTICIDE = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_FARM_NO_PESTICIDE),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_RADAR_DISCONNECT = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_RADAR_DISCONNECT),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_USER_EXIT = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_USER_EXIT),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_USER_BREAK = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_USER_BREAK),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_USER_SET_GOHOME = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_USER_SET_GOHOME),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_USER_AGRO_PLANNER_STATE_CHANGE = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_USER_AGRO_PLANNER_STATE_CHANGE),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_USER_SWITCH_RC_MODE = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_USER_SWITCH_RC_MODE),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_INIT_FAIL = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_INIT_FAIL),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_JOB_EXIT_BUT_MIS_RUNNING = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_JOB_EXIT_BUT_MIS_RUNNING),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_ON_GROUND_MOTOR_ON_CANNOT_GO = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_ON_GROUND_MOTOR_ON_CANNOT_GO),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_INVALID_START_INDEX_OR_PROG = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_INVALID_START_INDEX_OR_PROG),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_INVALID_CSYS_MODE = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_INVALID_CSYS_MODE),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_INVALID_HEIGHT_MODE = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_INVALID_HEIGHT_MODE),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_INVALID_FLY_WP_MODE = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_INVALID_FLY_WP_MODE),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_INVALID_YAW_MODE = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_INVALID_YAW_MODE),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_INVALID_TURN_DIR_MODE = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_INVALID_TURN_DIR_MODE),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_INVALID_WP_TYPE = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_INVALID_WP_TYPE),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_FIR_LAS_WP_TYPE_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_FIR_LAS_WP_TYPE_ERROR),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_GLOB_VEL_OUT_OF_RANGE = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_GLOB_VEL_OUT_OF_RANGE),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_WP_NUM_OUT_OF_RANGE = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_WP_NUM_OUT_OF_RANGE),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_LAT_LONG_OUT_OF_RANGE = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_LAT_LONG_OUT_OF_RANGE),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_DAMP_DIS_OUT_OF_RANGE = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_DAMP_DIS_OUT_OF_RANGE),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_MAX_VEL_OUT_OF_RANGE = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_MAX_VEL_OUT_OF_RANGE),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_VEL_OUT_OF_RANGE = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_VEL_OUT_OF_RANGE),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_WP_YAW_OUT_OF_RANGE = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_WP_YAW_OUT_OF_RANGE),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_INVALID_YAW_MODE_IN_VERT_SEGM = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_INVALID_YAW_MODE_IN_VERT_SEGM),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_WP_BREAK_INFO_MISSION_ID_CHANGED = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_WP_BREAK_INFO_MISSION_ID_CHANGED),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_WP_BREAK_INFO_PROGRESS_OUT_OF_RANGE = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_WP_BREAK_INFO_PROGRESS_OUT_OF_RANGE),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_WP_BREAK_INFO_INVALID_MISSION_STATE = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_WP_BREAK_INFO_INVALID_MISSION_STATE),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_WP_BREAK_INFO_WP_INDEX_OUT_OF_RANGE = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_WP_BREAK_INFO_WP_INDEX_OUT_OF_RANGE),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_BREAK_LAT_LONG_OUT_OF_RANGE = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_BREAK_LAT_LONG_OUT_OF_RANGE),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_BREAK_INFO_WP_YAW_OUT_OF_RANGE = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_BREAK_INFO_WP_YAW_OUT_OF_RANGE),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_INVALID_BREAK_INFO_FLAG = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_INVALID_BREAK_INFO_FLAG),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_GET_TRAJ_INFO_FAILED = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_GET_TRAJ_INFO_FAILED),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_GENERATE_FAIL = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_GENERATE_FAIL),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_LIB_RUN_FAIL = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_LIB_RUN_FAIL),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRAJ_LIB_EMERGENCY_BRAKE = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRAJ_LIB_EMERGENCY_BRAKE),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_ACTION_NOT_FOUND = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_ACTION_NOT_FOUND),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_ACTION_INDEX_REPEATED = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_ACTION_INDEX_REPEATED),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_ACTION_INFO_SIZE_TOO_LONG_OR_TOO_SHORT = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_ACTION_INFO_SIZE_TOO_LONG_OR_TOO_SHORT),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_ACTION_TREE_EMPTY = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_ACTION_TREE_EMPTY),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_ACTION_TREE_LAYER_EMPTY = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_ACTION_TREE_LAYER_EMPTY),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_ACTION_ID_REPEATED = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_ACTION_ID_REPEATED),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_ACTION_NODE_CHILDREN_NUM_LT_2 = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_ACTION_NODE_CHILDREN_NUM_LT_2),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_ACTION_INDEX_OUT_OF_RANGE = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_ACTION_INDEX_OUT_OF_RANGE),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_ACTION_ID_IS_65535 = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_ACTION_ID_IS_65535),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_ACTION_NODE_CHILDNUM_SUM_NOT_EQ_NEXT_LAYER_SIZE = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_ACTION_NODE_CHILDNUM_SUM_NOT_EQ_NEXT_LAYER_SIZE),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_ACTION_TREE_LAYER_NUM_TOO_MORE = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_ACTION_TREE_LAYER_NUM_TOO_MORE),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_ACTION_TREE_LAYER_NUM_TOO_LESS = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_ACTION_TREE_LAYER_NUM_TOO_LESS),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_ACTION_GROUP_NUM_OUT_OF_RANGE = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_ACTION_GROUP_NUM_OUT_OF_RANGE),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_ACTION_GROUP_VALID_RANGE_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_ACTION_GROUP_VALID_RANGE_ERROR),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_ACTION_TREE_ROOT_STATUS_INVALID = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_ACTION_TREE_ROOT_STATUS_INVALID),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_ACTION_TREE_NODE_STATUS_INVALID = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_ACTION_TREE_NODE_STATUS_INVALID),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_BREAK_INFO_ACTION_GROUP_ID_OUT_OF_RANGE = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_BREAK_INFO_ACTION_GROUP_ID_OUT_OF_RANGE),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_ACTION_STATUS_TREE_SIZE_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_ACTION_STATUS_TREE_SIZE_ERROR),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_BREAK_INFO_TRIGGER_RUN_RESULT_INVALID = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_BREAK_INFO_TRIGGER_RUN_RESULT_INVALID),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_BREAK_INFO_ACTION_GROUP_ID_REPEATED = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_BREAK_INFO_ACTION_GROUP_ID_REPEATED),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_BREAK_INFO_ACTION_LOCATION_REPEATED = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_BREAK_INFO_ACTION_LOCATION_REPEATED),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_BREAK_INFO_ACTION_LOCATION_OUT_OF_RANGE = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_BREAK_INFO_ACTION_LOCATION_OUT_OF_RANGE),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_RESUME_ID_NOT_IN_BREAK_INFO = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_RESUME_ID_NOT_IN_BREAK_INFO),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_RESUME_INFO_MODIFY_ACTION_STATUS_FROM_NO_INTERRUPT_TO_INTERRUPT = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_RESUME_INFO_MODIFY_ACTION_STATUS_FROM_NO_INTERRUPT_TO_INTERRUPT),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTION_COMMON_ACTION_RESUME_FAIL_FOR_INVALID_RESUME_INFO = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTION_COMMON_ACTION_RESUME_FAIL_FOR_INVALID_RESUME_INFO),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_ACTUATOR_COMMON_ACTUATOR_NOT_FOUND = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_ACTUATOR_COMMON_ACTUATOR_NOT_FOUND),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRIGGER_NOT_FOUND = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRIGGER_NOT_FOUND),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRIGGER_SINGLE_TIME_CHECK_FAIL = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRIGGER_SINGLE_TIME_CHECK_FAIL),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_CANNOT_TAKEOFF_ON_SHIP = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_CANNOT_TAKEOFF_ON_SHIP),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_DJI_TARGET_NOT_REACHABLE = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_DJI_TARGET_NOT_REACHABLE),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_DJI_TRAJ_PLAN_FAILED = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_DJI_TRAJ_PLAN_FAILED),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_DEPARTURE_WAYLINE_FAIL = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_DEPARTURE_WAYLINE_FAIL),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_DEPARTURE_WAYLINE_INIT_FAIL = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_DEPARTURE_WAYLINE_INIT_FAIL),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_DEPARTURE_WAYLINE_INVALID = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_DEPARTURE_WAYLINE_INVALID),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_TRANSPORT_DIVERT = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_TRANSPORT_DIVERT),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_DOCK_MOVE_GOHOME = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_DOCK_MOVE_GOHOME),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_HEADING_LEVEL_POOR = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_HEADING_LEVEL_POOR),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_JAMMING_GOHOME = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_JAMMING_GOHOME),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_WIND_SPD = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_WIND_SPD),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_BUSY_IMU_CHANGED = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_BUSY_IMU_CHANGED),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_RAIN_DETECT_VALID = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_RAIN_DETECT_VALID),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_WP_ARM1_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_WP_ARM1_ERROR),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_WP_ARM2_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_WP_ARM2_ERROR),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_WP_ARM3_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_WP_ARM3_ERROR),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_WP_ARM4_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_WP_ARM4_ERROR),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_WP_RADAR_PEDEX_WARNING = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_WP_RADAR_PEDEX_WARNING),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_WP_OVERLOAD = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_WP_OVERLOAD),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_WP_HOIST_NOT_READY = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_WP_HOIST_NOT_READY),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_WP_HOIST_SELF_CHECK_ERR = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_WP_HOIST_SELF_CHECK_ERR),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_WP_PREVENT_MOTOR_START = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_WP_PREVENT_MOTOR_START),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_WP_EST_BAT_OVERTEMP_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_WP_EST_BAT_OVERTEMP_ERROR),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_PAYLOAD_NOT_HOIST = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_PAYLOAD_NOT_HOIST),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_DJI_INNER_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_DJI_INNER_ERROR),
    DJI_ERROR_WAYPOINT_V3_MODULE_CODE_DJI_UNKNOWN_ERROR = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_MODULE_RAW_CODE_DJI_UNKNOWN_ERROR),

    /* Waypoint v3 breakpoint query errors */
    DJI_ERROR_WAYPOINT_V3_BREAKPOINT_QUERY_CODE_MISSION_NOT_EXIST = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_BREAKPOINT_QUERY_RAW_CODE_MISSION_NOT_EXIST),
    DJI_ERROR_WAYPOINT_V3_BREAKPOINT_QUERY_CODE_NO_BREAKPOINT = DJI_ERROR_CODE(DJI_ERROR_MODULE_WAYPOINT_V3, DJI_ERROR_WAYPOINT_V3_BREAKPOINT_QUERY_RAW_CODE_NO_BREAKPOINT),

    /* Spotlight zoom (SPZ) module errors */
    DJI_ERROR_SPZ_MODULE_CODE_OK                            = DJI_ERROR_CODE(DJI_ERROR_MODULE_SPZ, DJI_ERROR_SPZ_MODULE_RAW_CODE_OK),
    DJI_ERROR_SPZ_MODULE_CODE_TAR_TOO_NEAR                  = DJI_ERROR_CODE(DJI_ERROR_MODULE_SPZ, DJI_ERROR_SPZ_MODULE_RAW_CODE_TAR_TOO_NEAR),
    DJI_ERROR_SPZ_MODULE_CODE_TAR_TOO_FAR                   = DJI_ERROR_CODE(DJI_ERROR_MODULE_SPZ, DJI_ERROR_SPZ_MODULE_RAW_CODE_TAR_TOO_FAR),
    DJI_ERROR_SPZ_MODULE_CODE_LOW_LIMIT                     = DJI_ERROR_CODE(DJI_ERROR_MODULE_SPZ, DJI_ERROR_SPZ_MODULE_RAW_CODE_LOW_LIMIT),
    DJI_ERROR_SPZ_MODULE_CODE_HEIGHT_LIMIT                  = DJI_ERROR_CODE(DJI_ERROR_MODULE_SPZ, DJI_ERROR_SPZ_MODULE_RAW_CODE_HEIGHT_LIMIT),
    DJI_ERROR_SPZ_MODULE_CODE_RADIUS_LIMIT                  = DJI_ERROR_CODE(DJI_ERROR_MODULE_SPZ, DJI_ERROR_SPZ_MODULE_RAW_CODE_RADIUS_LIMIT),
    DJI_ERROR_SPZ_MODULE_CODE_BOUNDARY_LIMIT                = DJI_ERROR_CODE(DJI_ERROR_MODULE_SPZ, DJI_ERROR_SPZ_MODULE_RAW_CODE_BOUNDARY_LIMIT),
    DJI_ERROR_SPZ_MODULE_CODE_GIMBAL_LIMITED                = DJI_ERROR_CODE(DJI_ERROR_MODULE_SPZ, DJI_ERROR_SPZ_MODULE_RAW_CODE_GIMBAL_LIMITED),
    DJI_ERROR_SPZ_MODULE_CODE_YAW_LIMITED                   = DJI_ERROR_CODE(DJI_ERROR_MODULE_SPZ, DJI_ERROR_SPZ_MODULE_RAW_CODE_YAW_LIMITED),
    DJI_ERROR_SPZ_MODULE_CODE_TOO_CLOSE_TO_OBSTACLE         = DJI_ERROR_CODE(DJI_ERROR_MODULE_SPZ, DJI_ERROR_SPZ_MODULE_RAW_CODE_TOO_CLOSE_TO_OBSTACLE),
    DJI_ERROR_SPZ_MODULE_CODE_GPS_TO_RTK                    = DJI_ERROR_CODE(DJI_ERROR_MODULE_SPZ, DJI_ERROR_SPZ_MODULE_RAW_CODE_GPS_TO_RTK),
    DJI_ERROR_SPZ_MODULE_CODE_RTK_TO_GPS                    = DJI_ERROR_CODE(DJI_ERROR_MODULE_SPZ, DJI_ERROR_SPZ_MODULE_RAW_CODE_RTK_TO_GPS),
    DJI_ERROR_SPZ_MODULE_CODE_OBSTACLE_AVOID_INVALID        = DJI_ERROR_CODE(DJI_ERROR_MODULE_SPZ, DJI_ERROR_SPZ_MODULE_RAW_CODE_OBSTACLE_AVOID_INVALID),
    DJI_ERROR_SPZ_MODULE_CODE_ZOOM_TRACKING_LIMITED         = DJI_ERROR_CODE(DJI_ERROR_MODULE_SPZ, DJI_ERROR_SPZ_MODULE_RAW_CODE_ZOOM_TRACKING_LIMITED),
    DJI_ERROR_SPZ_MODULE_CODE_TARGET_LOST                   = DJI_ERROR_CODE(DJI_ERROR_MODULE_SPZ, DJI_ERROR_SPZ_MODULE_RAW_CODE_TARGET_LOST),
    DJI_ERROR_SPZ_MODULE_CODE_GPS_WEAK                      = DJI_ERROR_CODE(DJI_ERROR_MODULE_SPZ, DJI_ERROR_SPZ_MODULE_RAW_CODE_GPS_WEAK),
    DJI_ERROR_SPZ_MODULE_CODE_EXIT_NO_ERROR                 = DJI_ERROR_CODE(DJI_ERROR_MODULE_SPZ, DJI_ERROR_SPZ_MODULE_RAW_CODE_EXIT_NO_ERROR),
    DJI_ERROR_SPZ_MODULE_CODE_EXIT_UNSUPPORTED_PAYLOAD      = DJI_ERROR_CODE(DJI_ERROR_MODULE_SPZ, DJI_ERROR_SPZ_MODULE_RAW_CODE_EXIT_UNSUPPORTED_PAYLOAD),
    DJI_ERROR_SPZ_MODULE_CODE_EXIT_UNSUPPORTED_CAMERA_MODE  = DJI_ERROR_CODE(DJI_ERROR_MODULE_SPZ, DJI_ERROR_SPZ_MODULE_RAW_CODE_EXIT_UNSUPPORTED_CAMERA_MODE),
    DJI_ERROR_SPZ_MODULE_CODE_EXIT_INVALID_CMD              = DJI_ERROR_CODE(DJI_ERROR_MODULE_SPZ, DJI_ERROR_SPZ_MODULE_RAW_CODE_EXIT_INVALID_CMD),
    DJI_ERROR_SPZ_MODULE_CODE_EXIT_GPS_INVALID              = DJI_ERROR_CODE(DJI_ERROR_MODULE_SPZ, DJI_ERROR_SPZ_MODULE_RAW_CODE_EXIT_GPS_INVALID),
    DJI_ERROR_SPZ_MODULE_CODE_EXIT_NO_IN_AIR                = DJI_ERROR_CODE(DJI_ERROR_MODULE_SPZ, DJI_ERROR_SPZ_MODULE_RAW_CODE_EXIT_NO_IN_AIR),
    DJI_ERROR_SPZ_MODULE_CODE_EXIT_UNSUPPORTED_CTRL_MODE    = DJI_ERROR_CODE(DJI_ERROR_MODULE_SPZ, DJI_ERROR_SPZ_MODULE_RAW_CODE_EXIT_UNSUPPORTED_CTRL_MODE),
    DJI_ERROR_SPZ_MODULE_CODE_EXIT_UNSUPPORTED_FLIGHT_MODE  = DJI_ERROR_CODE(DJI_ERROR_MODULE_SPZ, DJI_ERROR_SPZ_MODULE_RAW_CODE_EXIT_UNSUPPORTED_FLIGHT_MODE),
    DJI_ERROR_SPZ_MODULE_CODE_EXIT_RC_LOST                  = DJI_ERROR_CODE(DJI_ERROR_MODULE_SPZ, DJI_ERROR_SPZ_MODULE_RAW_CODE_EXIT_RC_LOST),
    DJI_ERROR_SPZ_MODULE_CODE_EXIT_STATUS_ERR               = DJI_ERROR_CODE(DJI_ERROR_MODULE_SPZ, DJI_ERROR_SPZ_MODULE_RAW_CODE_EXIT_STATUS_ERR),
    DJI_ERROR_SPZ_MODULE_CODE_EXIT_INNER_UNKNOW_ERROR       = DJI_ERROR_CODE(DJI_ERROR_MODULE_SPZ, DJI_ERROR_SPZ_MODULE_RAW_CODE_EXIT_INNER_UNKNOW_ERROR),

    /* FlyTo (SetModeStartMission) errors */
    DJI_ERROR_FC_FLYTO_MODULE_CODE_INTERNAL_ERROR     = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_FLYTO, DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_INTERNAL_ERROR),
    DJI_ERROR_FC_FLYTO_MODULE_CODE_BAD_GPS_SIGNAL       = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_FLYTO, DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_BAD_GPS_SIGNAL),
    DJI_ERROR_FC_FLYTO_MODULE_CODE_ABSPOS_INVALID       = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_FLYTO, DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_ABSPOS_INVALID),
    DJI_ERROR_FC_FLYTO_MODULE_CODE_PLANNING_FAIL        = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_FLYTO, DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_PLANNING_FAIL),
    DJI_ERROR_FC_FLYTO_MODULE_CODE_HOME_NOTSET          = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_FLYTO, DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_HOME_NOTSET),
    DJI_ERROR_FC_FLYTO_MODULE_CODE_RC_LOST              = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_FLYTO, DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_RC_LOST),
    DJI_ERROR_FC_FLYTO_MODULE_CODE_TAKEOFF_ON_SHIP      = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_FLYTO, DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_TAKEOFF_ON_SHIP),
    DJI_ERROR_FC_FLYTO_MODULE_CODE_SOMEWAYRUNOUT        = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_FLYTO, DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_SOMEWAYRUNOUT),
    DJI_ERROR_FC_FLYTO_MODULE_CODE_SWITCH_PLANNING_ALGO = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_FLYTO, DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_SWITCH_PLANNING_ALGO),
    DJI_ERROR_FC_FLYTO_MODULE_CODE_HEIGHT_EXCEED_LIMIT  = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_FLYTO, DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_HEIGHT_EXCEED_LIMIT),
    DJI_ERROR_FC_FLYTO_MODULE_CODE_GOAL_MEETHORLIM        = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_FLYTO, DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_GOAL_MEETHORLIM),
    DJI_ERROR_FC_FLYTO_MODULE_CODE_GOAL_MEETHORLIM_RADIUS = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_FLYTO, DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_GOAL_MEETHORLIM_RADIUS),
    DJI_ERROR_FC_FLYTO_MODULE_CODE_GOAL_MEETVERLIM        = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_FLYTO, DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_GOAL_MEETVERLIM),
    DJI_ERROR_FC_FLYTO_MODULE_CODE_GOAL_MEETVERLIM_TOP    = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_FLYTO, DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_GOAL_MEETVERLIM_TOP),
    DJI_ERROR_FC_FLYTO_MODULE_CODE_GOAL_MEETVERLIM_BOTTOM = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_FLYTO, DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_GOAL_MEETVERLIM_BOTTOM),
    DJI_ERROR_FC_FLYTO_MODULE_CODE_DEPARTURE_WAYLINE_ERROR          = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_FLYTO, DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_DEPARTURE_WAYLINE_ERROR),
    DJI_ERROR_FC_FLYTO_MODULE_CODE_DEPARTURE_WAYLINE_INIT           = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_FLYTO, DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_DEPARTURE_WAYLINE_INIT),
    DJI_ERROR_FC_FLYTO_MODULE_CODE_DEPARTURE_WAYLINE_INVALID        = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_FLYTO, DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_DEPARTURE_WAYLINE_INVALID),
    DJI_ERROR_FC_FLYTO_MODULE_CODE_DEPARTURE_WAYLINE_NOT_REACHABLE  = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_FLYTO, DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_DEPARTURE_WAYLINE_NOT_REACHABLE),
    DJI_ERROR_FC_FLYTO_MODULE_CODE_DEPARTURE_WAYLINE_PLANNING_FAIL  = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_FLYTO, DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_DEPARTURE_WAYLINE_PLANNING_FAIL),
    DJI_ERROR_FC_FLYTO_MODULE_CODE_DEPARTURE_WAYLINE_RC_SWITCH_MODE = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_FLYTO, DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_DEPARTURE_WAYLINE_RC_SWITCH_MODE),
    DJI_ERROR_FC_FLYTO_MODULE_CODE_DEPARTURE_WAYLINE_RTK_TYPE_ERR   = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_FLYTO, DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_DEPARTURE_WAYLINE_RTK_TYPE_ERR),
    DJI_ERROR_FC_FLYTO_MODULE_CODE_PREVENT_MOTOR_ERROR    = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_FLYTO, DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_PREVENT_MOTOR_ERROR),
    DJI_ERROR_FC_FLYTO_MODULE_CODE_GOALINVALID             = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_FLYTO, DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_GOALINVALID),
    DJI_ERROR_FC_FLYTO_MODULE_CODE_MAXFLYVEL_INVALID       = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_FLYTO, DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_MAXFLYVEL_INVALID),
    DJI_ERROR_FC_FLYTO_MODULE_CODE_VERSION_INVALID         = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_FLYTO, DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_VERSION_INVALID),
    DJI_ERROR_FC_FLYTO_MODULE_CODE_STATEMACHINE_INVALID    = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_FLYTO, DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_STATEMACHINE_INVALID),
    DJI_ERROR_FC_FLYTO_MODULE_CODE_MEA_INVALID             = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_FLYTO, DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_MEA_INVALID),
    DJI_ERROR_FC_FLYTO_MODULE_CODE_UAV_MEETHORLIM          = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_FLYTO, DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_UAV_MEETHORLIM),
    DJI_ERROR_FC_FLYTO_MODULE_CODE_UAV_MEETHORLIM_RADIUS   = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_FLYTO, DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_UAV_MEETHORLIM_RADIUS),
    DJI_ERROR_FC_FLYTO_MODULE_CODE_UAV_MEETVERLIM          = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_FLYTO, DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_UAV_MEETVERLIM),
    DJI_ERROR_FC_FLYTO_MODULE_CODE_UAV_MEETVERLIM_TOP      = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_FLYTO, DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_UAV_MEETVERLIM_TOP),
    DJI_ERROR_FC_FLYTO_MODULE_CODE_UAV_MEETVERLIM_BOTTOM   = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_FLYTO, DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_UAV_MEETVERLIM_BOTTOM),
    DJI_ERROR_FC_FLYTO_MODULE_CODE_TAKEOFF_FAIL            = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_FLYTO, DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_TAKEOFF_FAIL),
    DJI_ERROR_FC_FLYTO_MODULE_CODE_GOAL_BLOCKEDBY_OBSTACLE = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_FLYTO, DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_GOAL_BLOCKEDBY_OBSTACLE),
    DJI_ERROR_FC_FLYTO_MODULE_CODE_OBSTACLETOOCLOSE        = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_FLYTO, DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_OBSTACLETOOCLOSE),
    DJI_ERROR_FC_FLYTO_MODULE_CODE_ROUTE_BLOCKED           = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_FLYTO, DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_ROUTE_BLOCKED),
    DJI_ERROR_FC_FLYTO_MODULE_CODE_UAV_MEETVFENCE          = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_FLYTO, DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_UAV_MEETVFENCE),
    DJI_ERROR_FC_FLYTO_MODULE_CODE_UAV_APPROACH_GEOCAGE    = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_FLYTO, DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_UAV_APPROACH_GEOCAGE),
    DJI_ERROR_FC_FLYTO_MODULE_CODE_USER_CONTROL_RC               = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_FLYTO, DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_USER_CONTROL_RC),
    DJI_ERROR_FC_FLYTO_MODULE_CODE_USER_REQUESTSTOP              = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_FLYTO, DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_USER_REQUESTSTOP),
    DJI_ERROR_FC_FLYTO_MODULE_CODE_PREEMPTBY_LOWBATHOME          = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_FLYTO, DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_PREEMPTBY_LOWBATHOME),
    DJI_ERROR_FC_FLYTO_MODULE_CODE_PREEMPTBY_LOWBATLAND          = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_FLYTO, DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_PREEMPTBY_LOWBATLAND),
    DJI_ERROR_FC_FLYTO_MODULE_CODE_PREEMPTBY_ADSB                = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_FLYTO, DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_PREEMPTBY_ADSB),
    DJI_ERROR_FC_FLYTO_MODULE_CODE_PREEMPTBY_HIGHERPRIORITY_MIS  = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_FLYTO, DJI_ERROR_FC_FLYTO_MODULE_RAW_CODE_PREEMPTBY_HIGHERPRIORITY_MIS),
    DJI_ERROR_FC_MOTOR_MODULE_CODE_SLOW_ROTATE_FAIL = DJI_ERROR_CODE(DJI_ERROR_MODULE_FC_MOTOR, DJI_ERROR_FC_MOTOR_MODULE_RAW_CODE_SLOW_ROTATE_FAIL),
};
//@formatter:on

/* Exported functions --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif // DJI_ERROR_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
