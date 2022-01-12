/**
 ********************************************************************
 * @file    psdk_error.h
 * @brief   This is the header file for "psdk_error.c", defining the structure and
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
#ifndef PSDK_ERROR_H
#define PSDK_ERROR_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/
#define PSDK_ERROR_MODULE_INDEX_OFFSET  12u
#define PSDK_ERROR_MODULE_INDEX_MASK    0x00000000000FF000u
#define PSDK_ERROR_RAW_CODE_OFFSET      0u
#define PSDK_ERROR_RAW_CODE_MASK        0x0000000000000FFFu

#define PSDK_ERROR_CODE(moduleIndex, rawErrCode) \
((uint64_t)0 | \
((((uint64_t)(moduleIndex)) << (PSDK_ERROR_MODULE_INDEX_OFFSET)) & (PSDK_ERROR_MODULE_INDEX_MASK)) | \
((((uint64_t)(rawErrCode)) << (PSDK_ERROR_RAW_CODE_OFFSET)) & (PSDK_ERROR_RAW_CODE_MASK)))

/**
 * @brief Error objects, specifying error code, error description, error reason and error recovery suggestion below.
 * @attention Require arrange by error code from small to large.
 */
#define PSDK_ERROR_OBJECTS \
/* system module error message */ \
{PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS, "Execution successfully.", NULL, NULL}, \
{PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_REQUEST_PARAMETER, "Request parameters are invalid.", NULL, "Please double-check requested parameters."}, \
{PSDK_ERROR_SYSTEM_MODULE_CODE_EXECUTING_HIGHER_PRIORITY_TASK, "A higher priority task is being executed.", NULL, "Please stop the higher priority task or try again later."}, \
{PSDK_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT, "Operation is not supported.", NULL, "Please check input parameters or contact DJI for help."}, \
{PSDK_ERROR_SYSTEM_MODULE_CODE_TIMEOUT, "Execution timeout.", NULL, "Please contact DJI for help."}, \
{PSDK_ERROR_SYSTEM_MODULE_CODE_MEMORY_ALLOC_FAILED, "Memory allocation failed.", NULL, "Please check system configure."}, \
{PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER, "Input parameters are invalid.", NULL, "Please double-check requested parameters."}, \
{PSDK_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT_IN_CURRENT_STATE, "Operation is not supported in current state.", NULL, "Please try again later."}, \
{PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR, "System error.", NULL, "Please contact DJI for help."}, \
{PSDK_ERROR_SYSTEM_MODULE_CODE_HARDWARE_ERR, "Hardware error.", NULL, "Please contact DJI for help."}, \
{PSDK_ERROR_SYSTEM_MODULE_CODE_INSUFFICIENT_ELECTRICITY, "Low battery.", NULL, "Please replacement battery for machine and try again."}, \
{PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN, "Unknown error.", NULL, NULL}, \
{PSDK_ERROR_SYSTEM_MODULE_CODE_NOT_FOUND, "Parameters are not found.", NULL, NULL}, \
{PSDK_ERROR_SYSTEM_MODULE_CODE_OUT_OF_RANGE, "Out of range.", NULL, "Please check parameters."}, \
{PSDK_ERROR_SYSTEM_MODULE_CODE_BUSY, "System is busy.", NULL, "Please try again later."}, \
{PSDK_ERROR_SYSTEM_MODULE_CODE_DUPLICATE, "Have existed the same object.", NULL, "Please input valid parameters."}, \
{PSDK_ERROR_SYSTEM_MODULE_CODE_ADAPTER_NOT_MATCH, "PSDK adapter do not meet requirements.", NULL, "Please try again after replacing PSDK adapter."}, \
 \
/* gimbal module error message */ \
{PSDK_ERROR_GIMBAL_MODULE_CODE_PITCH_REACH_POSITIVE_LIMIT, "Pitch axis gimbal reach positive limit.", NULL, "Please do not rotate towards positive direction."}, \
{PSDK_ERROR_GIMBAL_MODULE_CODE_PITCH_REACH_NEGATIVE_LIMIT, "Pitch axis gimbal reach negative limit.", NULL, "Please do not rotate towards negative direction."}, \
{PSDK_ERROR_GIMBAL_MODULE_CODE_ROLL_REACH_POSITIVE_LIMIT, "Roll axis gimbal reach positive limit.", NULL, "Please do not rotate towards positive direction."}, \
{PSDK_ERROR_GIMBAL_MODULE_CODE_ROLL_REACH_NEGATIVE_LIMIT, "Roll axis gimbal reach negative limit.", NULL, "Please do not rotate towards negative direction."}, \
{PSDK_ERROR_GIMBAL_MODULE_CODE_YAW_REACH_POSITIVE_LIMIT, "Yaw axis gimbal reach positive limit.", NULL, "Please do not rotate towards positive direction."}, \
{PSDK_ERROR_GIMBAL_MODULE_CODE_YAW_REACH_NEGATIVE_LIMIT, "Yaw axis gimbal reach negative limit.", NULL, "Please do not rotate towards negative direction."}, \
{PSDK_ERROR_GIMBAL_MODULE_CODE_NON_CONTROL_AUTHORITY, "Current device do not have control authority of the gimbal.", NULL, "Please do not control gimbal with other devices that have high control priority simultaneously."}, \
 \
/* payload collaboration module error message */ \
{PSDK_ERROR_PAYLOAD_COLLABORATION_MODULE_CODE_POSITION_NOT_MATCH, "Payload mount position do not meet requirements.", NULL, "Please read API and user documentation, ensuring input parameters satisfy requirements."}, \
 \
/* subscription module error message */ \
{PSDK_ERROR_SUBSCRIPTION_MODULE_CODE_INVALID_TOPIC_FREQ, "Frequency of topic is invalid.", NULL, "Please read API and user documentation, ensuring input parameters satisfy requirements."}, \
{PSDK_ERROR_SUBSCRIPTION_MODULE_CODE_TOPIC_DUPLICATE, "Topic is subscribed repeatedly.", NULL, "Please do not subscribe a topic repeatedly."}, \
{PSDK_ERROR_SUBSCRIPTION_MODULE_CODE_TOPIC_NOT_SUBSCRIBED, "Requested topic have not been subscribed.", NULL, "Please try to get value after subscribing topic."}, \
{PSDK_ERROR_SUBSCRIPTION_MODULE_CODE_TIMESTAMP_NOT_ENABLE, "Requested topic do not have timestamp data.", "Did not enable timestamp when subscribe topic.", "Please subscribe topic enabled timestamp."}, \
/* mop channel module error message */ \
{PSDK_ERROR_MOP_CHANNEL_MODULE_CODE_CONNECTION_CLOSE, "Connection of channel is closed.", "The peer channel do not work or abnormally be closed.", "Please confirm state of the peer channel and reaccept the connection request of MSDK/OSDK"}, \

#define PSDK_RETURN_CODE_OK                 PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS /*!< Payload SDK return code represents as status is ok. */
#define PSDK_RETURN_CODE_ERR_ALLOC          PSDK_ERROR_SYSTEM_MODULE_CODE_MEMORY_ALLOC_FAILED /*!< Payload SDK return code represents as status alloc error. */
#define PSDK_RETURN_CODE_ERR_TIMEOUT        PSDK_ERROR_SYSTEM_MODULE_CODE_TIMEOUT /*!< Payload SDK return code represents as status timeout error. */
#define PSDK_RETURN_CODE_ERR_NOT_FOUND      PSDK_ERROR_SYSTEM_MODULE_CODE_NOT_FOUND /*!< Payload SDK return code represents as status not found error. */
#define PSDK_RETURN_CODE_ERR_OUT_OF_RANGE   PSDK_ERROR_SYSTEM_MODULE_CODE_OUT_OF_RANGE /*!< Payload SDK return code represents as status out of range error. */
#define PSDK_RETURN_CODE_ERR_PARAM          PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER /*!< Payload SDK return code represents as status parameter error. */
#define PSDK_RETURN_CODE_ERR_SYSTEM         PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR /*!< Payload SDK return code represents as status system error. */
#define PSDK_RETURN_CODE_ERR_BUSY           PSDK_ERROR_SYSTEM_MODULE_CODE_BUSY /*!< Payload SDK return code represents as status busy error. */
#define PSDK_RETURN_CODE_ERR_UNSUPPORT      PSDK_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT /*!< Payload SDK return code represents as status nonsupport error. */
#define PSDK_RETURN_CODE_ERR_UNKNOWN        PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN /*!< Payload SDK return code represents as status unknown error. */

/* Exported types ------------------------------------------------------------*/
/**
 * @brief PSDK module enum for defining error code.
 */
typedef enum {
    PSDK_ERROR_MODULE_SYSTEM = 0,
    PSDK_ERROR_MODULE_PLATFORM,
    PSDK_ERROR_MODULE_LOGGER,
    PSDK_ERROR_MODULE_TIME_SYNC,
    PSDK_ERROR_MODULE_COMMAND,
    PSDK_ERROR_MODULE_CAMERA,
    PSDK_ERROR_MODULE_GIMBAL,
    PSDK_ERROR_MODULE_XPORT,
    PSDK_ERROR_MODULE_PAYLOAD_COLLABORATION,
    PSDK_ERROR_MODULE_WIDGET,
    PSDK_ERROR_MODULE_CORE,
    PSDK_ERROR_MODULE_IDENTITY,
    PSDK_ERROR_MODULE_TRANSMISSION,
    PSDK_ERROR_MODULE_DATA_CHANNEL,
    PSDK_ERROR_MODULE_SUBSCRIPTION,
    PSDK_ERROR_MODULE_MOP_CHANNEL,
    PSDK_ERROR_MODULE_POSITIONING,
    PSDK_ERROR_MODULE_POWER_MANAGEMENT,
    PSDK_ERROR_MODULE_AIRCRAFTINFO,
    PSDK_ERROR_MODULE_PRODUCTINFO,
    PSDK_ERROR_MODULE_FLOWCONTROLLER,
    PSDK_ERROR_MODULE_DOWNLOADER,
    PSDK_ERROR_MODULE_PARAMETER,
    PSDK_ERROR_MODULE_UTIL,
    PSDK_ERROR_MODULE_USER,
    PSDK_ERROR_MODULE_NEGOTIATE,
    PSDK_ERROR_MODULE_UPGRADE,
    PSDK_ERROR_MODULE_ERROR,
} E_PsdkErrorModule;

/**
 * @brief Raw error code of system module.
 */
typedef enum {
    PSDK_ERROR_SYSTEM_MODULE_RAW_CODE_SUCCESS = 0x000,
    PSDK_ERROR_SYSTEM_MODULE_RAW_CODE_INVALID_REQUEST_PARAMETER = 0x0D4,
    PSDK_ERROR_SYSTEM_MODULE_RAW_CODE_EXECUTING_HIGHER_PRIORITY_TASK = 0x0D7,
    PSDK_ERROR_SYSTEM_MODULE_RAW_CODE_NONSUPPORT = 0x0E0,
    PSDK_ERROR_SYSTEM_MODULE_RAW_CODE_TIMEOUT = 0x0E1,
    PSDK_ERROR_SYSTEM_MODULE_RAW_CODE_MEMORY_ALLOC_FAILED = 0x0E2,
    PSDK_ERROR_SYSTEM_MODULE_RAW_CODE_INVALID_PARAMETER = 0x0E3,
    PSDK_ERROR_SYSTEM_MODULE_RAW_CODE_NONSUPPORT_IN_CURRENT_STATE = 0x0E4,
    PSDK_ERROR_SYSTEM_MODULE_RAW_CODE_SYSTEM_ERROR = 0x0EC,
    PSDK_ERROR_SYSTEM_MODULE_RAW_CODE_HARDWARE_ERR = 0x0FA,
    PSDK_ERROR_SYSTEM_MODULE_RAW_CODE_INSUFFICIENT_ELECTRICITY = 0x0FB,
    PSDK_ERROR_SYSTEM_MODULE_RAW_CODE_UNKNOWN = 0x0FF,
    PSDK_ERROR_SYSTEM_MODULE_RAW_CODE_NOT_FOUND = 0x100,
    PSDK_ERROR_SYSTEM_MODULE_RAW_CODE_OUT_OF_RANGE = 0x101,
    PSDK_ERROR_SYSTEM_MODULE_RAW_CODE_BUSY = 0x102,
    PSDK_ERROR_SYSTEM_MODULE_RAW_CODE_DUPLICATE = 0x103,
    PSDK_ERROR_SYSTEM_MODULE_RAW_CODE_ADAPTER_NOT_MATCH = 0x104,
} E_PsdkErrorSystemModuleRawCode;

/**
 * @brief Raw error code of gimbal module.
 */
typedef enum {
    PSDK_ERROR_GIMBAL_MODULE_RAW_CODE_PITCH_REACH_POSITIVE_LIMIT = 0x000,
    PSDK_ERROR_GIMBAL_MODULE_RAW_CODE_PITCH_REACH_NEGATIVE_LIMIT = 0x001,
    PSDK_ERROR_GIMBAL_MODULE_RAW_CODE_ROLL_REACH_POSITIVE_LIMIT = 0x002,
    PSDK_ERROR_GIMBAL_MODULE_RAW_CODE_ROLL_REACH_NEGATIVE_LIMIT = 0x003,
    PSDK_ERROR_GIMBAL_MODULE_RAW_CODE_YAW_REACH_POSITIVE_LIMIT = 0x004,
    PSDK_ERROR_GIMBAL_MODULE_RAW_CODE_YAW_REACH_NEGATIVE_LIMIT = 0x005,
    PSDK_ERROR_GIMBAL_MODULE_RAW_CODE_NON_CONTROL_AUTHORITY = 0x006,
} E_PsdkErrorGimbalModuleRawCode;

/**
 * @brief Raw error code of payload collaboration module.
 */
typedef enum {
    PSDK_ERROR_PAYLOAD_COLLABORATION_MODULE_RAW_CODE_POSITION_NOT_MATCH = 0x000,
} E_PsdkErrorPayloadCollaborationModuleRawCode;

/**
 * @brief Raw error code of subscription module.
 */
typedef enum {
    PSDK_ERROR_SUBSCRIPTION_MODULE_RAW_CODE_INVALID_TOPIC_FREQ = 0x000,
    PSDK_ERROR_SUBSCRIPTION_MODULE_RAW_CODE_TOPIC_DUPLICATE = 0x001,
    PSDK_ERROR_SUBSCRIPTION_MODULE_RAW_CODE_TOPIC_NOT_SUBSCRIBED = 0x002,
    PSDK_ERROR_SUBSCRIPTION_MODULE_RAW_CODE_TIMESTAMP_NOT_ENABLE = 0x003,
} E_PsdkErrorSubscriptionModuleRawCode;

/**
 * @brief Raw error code of mop channel module.
 */
typedef enum {
    PSDK_ERROR_MOP_CHANNEL_MODULE_RAW_CODE_CONNECTION_CLOSE = 0x00B,
} E_PsdkErrorMopChannelModuleRawCode;

//@formatter:off
/**
 * @brief PSDK error code complete works. Users can search all error messages here.
 * @details Please get error description, error reason and error recovery suggestion of every error code from
 * ::PSDK_ERROR_OBJECTS macro.
 */
enum PsdkErrorCode {
    /* system module error code, including some common error code */
    PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS = PSDK_ERROR_CODE(PSDK_ERROR_MODULE_SYSTEM, PSDK_ERROR_SYSTEM_MODULE_RAW_CODE_SUCCESS),
    PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_REQUEST_PARAMETER = PSDK_ERROR_CODE(PSDK_ERROR_MODULE_SYSTEM, PSDK_ERROR_SYSTEM_MODULE_RAW_CODE_INVALID_REQUEST_PARAMETER),
    PSDK_ERROR_SYSTEM_MODULE_CODE_EXECUTING_HIGHER_PRIORITY_TASK = PSDK_ERROR_CODE(PSDK_ERROR_MODULE_SYSTEM, PSDK_ERROR_SYSTEM_MODULE_RAW_CODE_EXECUTING_HIGHER_PRIORITY_TASK),
    PSDK_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT = PSDK_ERROR_CODE(PSDK_ERROR_MODULE_SYSTEM, PSDK_ERROR_SYSTEM_MODULE_RAW_CODE_NONSUPPORT),
    PSDK_ERROR_SYSTEM_MODULE_CODE_TIMEOUT = PSDK_ERROR_CODE(PSDK_ERROR_MODULE_SYSTEM, PSDK_ERROR_SYSTEM_MODULE_RAW_CODE_TIMEOUT),
    PSDK_ERROR_SYSTEM_MODULE_CODE_MEMORY_ALLOC_FAILED = PSDK_ERROR_CODE(PSDK_ERROR_MODULE_SYSTEM, PSDK_ERROR_SYSTEM_MODULE_RAW_CODE_MEMORY_ALLOC_FAILED),
    PSDK_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER = PSDK_ERROR_CODE(PSDK_ERROR_MODULE_SYSTEM, PSDK_ERROR_SYSTEM_MODULE_RAW_CODE_INVALID_PARAMETER),
    PSDK_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT_IN_CURRENT_STATE = PSDK_ERROR_CODE(PSDK_ERROR_MODULE_SYSTEM, PSDK_ERROR_SYSTEM_MODULE_RAW_CODE_NONSUPPORT_IN_CURRENT_STATE),
    PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR = PSDK_ERROR_CODE(PSDK_ERROR_MODULE_SYSTEM, PSDK_ERROR_SYSTEM_MODULE_RAW_CODE_SYSTEM_ERROR),
    PSDK_ERROR_SYSTEM_MODULE_CODE_HARDWARE_ERR = PSDK_ERROR_CODE(PSDK_ERROR_MODULE_SYSTEM, PSDK_ERROR_SYSTEM_MODULE_RAW_CODE_HARDWARE_ERR),
    PSDK_ERROR_SYSTEM_MODULE_CODE_INSUFFICIENT_ELECTRICITY = PSDK_ERROR_CODE(PSDK_ERROR_MODULE_SYSTEM, PSDK_ERROR_SYSTEM_MODULE_RAW_CODE_INSUFFICIENT_ELECTRICITY),
    PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN = PSDK_ERROR_CODE(PSDK_ERROR_MODULE_SYSTEM, PSDK_ERROR_SYSTEM_MODULE_RAW_CODE_UNKNOWN),
    PSDK_ERROR_SYSTEM_MODULE_CODE_NOT_FOUND = PSDK_ERROR_CODE(PSDK_ERROR_MODULE_SYSTEM, PSDK_ERROR_SYSTEM_MODULE_RAW_CODE_NOT_FOUND),
    PSDK_ERROR_SYSTEM_MODULE_CODE_OUT_OF_RANGE = PSDK_ERROR_CODE(PSDK_ERROR_MODULE_SYSTEM, PSDK_ERROR_SYSTEM_MODULE_RAW_CODE_OUT_OF_RANGE),
    PSDK_ERROR_SYSTEM_MODULE_CODE_BUSY = PSDK_ERROR_CODE(PSDK_ERROR_MODULE_SYSTEM, PSDK_ERROR_SYSTEM_MODULE_RAW_CODE_BUSY),
    PSDK_ERROR_SYSTEM_MODULE_CODE_DUPLICATE = PSDK_ERROR_CODE(PSDK_ERROR_MODULE_SYSTEM, PSDK_ERROR_SYSTEM_MODULE_RAW_CODE_DUPLICATE),
    PSDK_ERROR_SYSTEM_MODULE_CODE_ADAPTER_NOT_MATCH = PSDK_ERROR_CODE(PSDK_ERROR_MODULE_SYSTEM, PSDK_ERROR_SYSTEM_MODULE_RAW_CODE_ADAPTER_NOT_MATCH),

    /* gimbal module error code */
    PSDK_ERROR_GIMBAL_MODULE_CODE_PITCH_REACH_POSITIVE_LIMIT = PSDK_ERROR_CODE(PSDK_ERROR_MODULE_GIMBAL, PSDK_ERROR_GIMBAL_MODULE_RAW_CODE_PITCH_REACH_POSITIVE_LIMIT),
    PSDK_ERROR_GIMBAL_MODULE_CODE_PITCH_REACH_NEGATIVE_LIMIT = PSDK_ERROR_CODE(PSDK_ERROR_MODULE_GIMBAL, PSDK_ERROR_GIMBAL_MODULE_RAW_CODE_PITCH_REACH_NEGATIVE_LIMIT),
    PSDK_ERROR_GIMBAL_MODULE_CODE_ROLL_REACH_POSITIVE_LIMIT = PSDK_ERROR_CODE(PSDK_ERROR_MODULE_GIMBAL, PSDK_ERROR_GIMBAL_MODULE_RAW_CODE_ROLL_REACH_POSITIVE_LIMIT),
    PSDK_ERROR_GIMBAL_MODULE_CODE_ROLL_REACH_NEGATIVE_LIMIT = PSDK_ERROR_CODE(PSDK_ERROR_MODULE_GIMBAL, PSDK_ERROR_GIMBAL_MODULE_RAW_CODE_ROLL_REACH_NEGATIVE_LIMIT),
    PSDK_ERROR_GIMBAL_MODULE_CODE_YAW_REACH_POSITIVE_LIMIT = PSDK_ERROR_CODE(PSDK_ERROR_MODULE_GIMBAL, PSDK_ERROR_GIMBAL_MODULE_RAW_CODE_YAW_REACH_POSITIVE_LIMIT),
    PSDK_ERROR_GIMBAL_MODULE_CODE_YAW_REACH_NEGATIVE_LIMIT = PSDK_ERROR_CODE(PSDK_ERROR_MODULE_GIMBAL, PSDK_ERROR_GIMBAL_MODULE_RAW_CODE_YAW_REACH_NEGATIVE_LIMIT),
    PSDK_ERROR_GIMBAL_MODULE_CODE_NON_CONTROL_AUTHORITY = PSDK_ERROR_CODE(PSDK_ERROR_MODULE_GIMBAL, PSDK_ERROR_GIMBAL_MODULE_RAW_CODE_NON_CONTROL_AUTHORITY),

    /* payload collaboration module error code */
    PSDK_ERROR_PAYLOAD_COLLABORATION_MODULE_CODE_POSITION_NOT_MATCH = PSDK_ERROR_CODE(PSDK_ERROR_MODULE_PAYLOAD_COLLABORATION, PSDK_ERROR_PAYLOAD_COLLABORATION_MODULE_RAW_CODE_POSITION_NOT_MATCH),

    /* subscription module error code */
    PSDK_ERROR_SUBSCRIPTION_MODULE_CODE_INVALID_TOPIC_FREQ = PSDK_ERROR_CODE(PSDK_ERROR_MODULE_SUBSCRIPTION, PSDK_ERROR_SUBSCRIPTION_MODULE_RAW_CODE_INVALID_TOPIC_FREQ),
    PSDK_ERROR_SUBSCRIPTION_MODULE_CODE_TOPIC_DUPLICATE = PSDK_ERROR_CODE(PSDK_ERROR_MODULE_SUBSCRIPTION, PSDK_ERROR_SUBSCRIPTION_MODULE_RAW_CODE_TOPIC_DUPLICATE),
    PSDK_ERROR_SUBSCRIPTION_MODULE_CODE_TOPIC_NOT_SUBSCRIBED = PSDK_ERROR_CODE(PSDK_ERROR_MODULE_SUBSCRIPTION, PSDK_ERROR_SUBSCRIPTION_MODULE_RAW_CODE_TOPIC_NOT_SUBSCRIBED),
    PSDK_ERROR_SUBSCRIPTION_MODULE_CODE_TIMESTAMP_NOT_ENABLE = PSDK_ERROR_CODE(PSDK_ERROR_MODULE_SUBSCRIPTION, PSDK_ERROR_SUBSCRIPTION_MODULE_RAW_CODE_TIMESTAMP_NOT_ENABLE),

    /* mop channel module error code */
    PSDK_ERROR_MOP_CHANNEL_MODULE_CODE_CONNECTION_CLOSE = PSDK_ERROR_CODE(PSDK_ERROR_MODULE_MOP_CHANNEL, PSDK_ERROR_MOP_CHANNEL_MODULE_RAW_CODE_CONNECTION_CLOSE),
};
//@formatter:on

/* Exported functions --------------------------------------------------------*/


#ifdef __cplusplus
}
#endif

#endif // PSDK_ERROR_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
