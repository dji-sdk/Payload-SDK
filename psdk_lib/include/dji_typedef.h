/**
 ********************************************************************
 * @file    dji_typedef.h
 * @brief   This is the header file for "dji_typedef.c", defining the structure and
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
#ifndef DJI_TYPEDEF_H
#define DJI_TYPEDEF_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "dji_error.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/
#define DJI_PI                                  (3.14159265358979323846f)
#define DJI_FILE_NAME_SIZE_MAX                  256
#define DJI_FILE_PATH_SIZE_MAX                  (DJI_FILE_NAME_SIZE_MAX + 256)
#define DJI_IP_ADDR_STR_SIZE_MAX                15
#define DJI_MD5_BUFFER_LEN                      16

#define DJI_SUBSCRIPTION_MODULE_INDEX_OFFSET    24u
#define DJI_SUBSCRIPTION_MODULE_INDEX_MASK      0xFF000000u
#define DJI_SUBSCRIPTION_TOPIC_CODE_OFFSET      0u
#define DJI_SUBSCRIPTION_TOPIC_CODE_MASK        0x00FFFFFFu

#define DJI_DATA_SUBSCRIPTION_TOPIC(subscriptionModule, topicCode) \
    (uint32_t)\
    (((((uint32_t)(subscriptionModule)) << (DJI_SUBSCRIPTION_MODULE_INDEX_OFFSET)) & (DJI_SUBSCRIPTION_MODULE_INDEX_MASK)) | \
    ((((uint32_t)(topicCode)) << (DJI_SUBSCRIPTION_TOPIC_CODE_OFFSET)) & (DJI_SUBSCRIPTION_TOPIC_CODE_MASK)))

/**
 * @brief Type define double as dji_f64_t.
 */
typedef double dji_f64_t;
/**
 * @brief Type define float as dji_f32_t.
 */
typedef float dji_f32_t;
/**
 * @brief Type define uint64 as T_DjiReturnCode.
 * @details The type can be any value of ::DjiErrorCode.
 */
typedef uint64_t T_DjiReturnCode;

typedef enum {
    DJI_MOUNT_POSITION_TYPE_UNKNOWN             = 0,
    DJI_MOUNT_POSITION_TYPE_PAYLOAD_PORT        = 1,
    DJI_MOUNT_POSITION_TYPE_EXTENSION_PORT      = 2,
    DJI_MOUNT_POSITION_TYPE_EXTENSION_LITE_PORT = 3
} E_DjiMountPositionType;

typedef enum {
    DJI_MOUNT_POSITION_UNKNOWN                  = 0,
    DJI_MOUNT_POSITION_PAYLOAD_PORT_NO1         = 1,
    DJI_MOUNT_POSITION_PAYLOAD_PORT_NO2         = 2,
    DJI_MOUNT_POSITION_PAYLOAD_PORT_NO3         = 3,
    DJI_MOUNT_POSITION_EXTENSION_PORT           = 4,
    DJI_MOUNT_POSITION_EXTENSION_LITE_PORT      = 5,
} E_DjiMountPosition;

typedef enum {
    DJI_AIRCRAFT_SERIES_UNKNOWN                 = 0,
    DJI_AIRCRAFT_SERIES_M200_V2                 = 1,
    DJI_AIRCRAFT_SERIES_M300                    = 2,
    DJI_AIRCRAFT_SERIES_M30                     = 3,
    DJI_AIRCRAFT_SERIES_M3                      = 4,
    DJI_AIRCRAFT_SERIES_M350                    = 5,
    DJI_AIRCRAFT_SERIES_M3D                     = 6,
    DJI_AIRCRAFT_SERIES_FC30                    = 7,
} E_DjiAircraftSeries;

typedef enum {
    DJI_AIRCRAFT_TYPE_UNKNOWN                   = 0, /*!< Aircraft type is unknown. */
    DJI_AIRCRAFT_TYPE_M200_V2                   = 44, /*!< Aircraft type is Matrice 200 V2. */
    DJI_AIRCRAFT_TYPE_M210_V2                   = 45, /*!< Aircraft type is Matrice 220 V2. */
    DJI_AIRCRAFT_TYPE_M210RTK_V2                = 46, /*!< Aircraft type is Matrice 210 RTK V2. */
    DJI_AIRCRAFT_TYPE_M300_RTK                  = 60, /*!< Aircraft type is Matrice 300 RTK. */
    DJI_AIRCRAFT_TYPE_M30                       = 67, /*!< Aircraft type is Matrice 30. */
    DJI_AIRCRAFT_TYPE_M30T                      = 68, /*!< Aircraft type is Matrice 30T. */
    DJI_AIRCRAFT_TYPE_M3E                       = 77, /*!< Aircraft type is Mavic 3E. */
    DJI_AIRCRAFT_TYPE_FC30                      = 78, /* !<Aircraft type is FlyCart 30> */
    DJI_AIRCRAFT_TYPE_M3T                       = 79, /*!< Aircraft type is Mavic 3T. */
    DJI_AIRCRAFT_TYPE_M350_RTK                  = 89, /*!< Aircraft type is Matrice 350 RTK. */
    DJI_AIRCRAFT_TYPE_M3D                       = 91, /*!< Aircraft type is Matrice 3D. */
    DJI_AIRCRAFT_TYPE_M3TD                      = 93, /*!< Aircraft type is Matrice 3TD. */
} E_DjiAircraftType;

/**
 * @brief Camera type.
 */
typedef enum {
    DJI_CAMERA_TYPE_UNKNOWN                     = 0, /*!< Camera type is unknown. */
    DJI_CAMERA_TYPE_Z30                         = 20, /*!< Camera type is Z30. */
    DJI_CAMERA_TYPE_XT2                         = 26, /*!< Camera type is XT2. */
    DJI_CAMERA_TYPE_PSDK                        = 31, /*!< Camera type is third party camera based on Payload SDK. */
    DJI_CAMERA_TYPE_XTS                         = 41, /*!< Camera type is XT S. */
    DJI_CAMERA_TYPE_H20                         = 42, /*!< Camera type is H20. */
    DJI_CAMERA_TYPE_H20T                        = 43, /*!< Camera type is H20T. */
    DJI_CAMERA_TYPE_H20N                        = 61, /*!< Camera type is H20N. */
    DJI_CAMERA_TYPE_P1                          = 50, /*!< Camera type is P1. */
    DJI_CAMERA_TYPE_L1, /*!< Camera type is L1. */
    DJI_CAMERA_TYPE_L2                          = 84, /*!< Camera type is L2. */
    DJI_CAMERA_TYPE_M30                         = 52, /*!< Camera type is M30. */
    DJI_CAMERA_TYPE_M30T                        = 53, /*!< Camera type is M30T. */
    DJI_CAMERA_TYPE_M3E                         = 66, /*!< Camera type is M3E. */
    DJI_CAMERA_TYPE_M3T                         = 67, /*!< Camera type is M3T. */
    DJI_CAMERA_TYPE_M3D                         = 80, /*!< Camera type is Matrice 3D. */
    DJI_CAMERA_TYPE_M3TD                        = 81, /*!< Camera type is Matrice 3TD. */
    DJI_CAMERA_TYPE_H30                         = 82, /*!< Camera type is H30. */
    DJI_CAMERA_TYPE_H30T                        = 83, /*!< Camera type is H30T. */
} E_DjiCameraType;

/**
 * @brief Camera supported media file subtype.
 */
typedef enum
{
    DJI_DOWNLOAD_FILE_ORG                       = 0, /*!< Media sub file origin data type. */
    DJI_DOWNLOAD_FILE_LDR                       = 21, /*!< Media sub file cloud point raw data type. */
    DJI_DOWNLOAD_FILE_SIG                       = 22, /*!< Media sub file point cloud signature type. */
    DJI_DOWNLOAD_FILE_RTK                       = 23, /*!< Media sub file point cloud real-time kinematic type. */
    DJI_DOWNLOAD_FILE_CLC                       = 25, /*!< Media sub file radar-camera external reference type. */
    DJI_DOWNLOAD_FILE_CLI                       = 26, /*!< Media sub file radar-IMU external reference type. */
    DJI_DOWNLOAD_FILE_IMU                       = 27, /*!< Media sub file IMU data type. */
    DJI_DOWNLOAD_FILE_RTL                       = 28, /*!< Media sub file RTK boom data type. */
    DJI_DOWNLOAD_FILE_RTB                       = 29, /*!< Media sub file RTK base station data type. */
    DJI_DOWNLOAD_FILE_RTS                       = 30, /*!< Media sub file RTK secondary antenna data type. */
    DJI_DOWNLOAD_FILE_RPOS                      = 31, /*!< Media sub file real-time fusion of attitude and position data type. */
} E_DjiCameraMediaFileSubType;

/**
 * @brief Camera supported media file type.
 */
typedef enum {
    DJI_CAMERA_FILE_TYPE_JPEG                   = 0, /*!< Media file JPEG type. */
    DJI_CAMERA_FILE_TYPE_DNG                    = 1, /*!< Media file DNG type. */
    DJI_CAMERA_FILE_TYPE_MOV                    = 2, /*!< Media file MOV type. */
    DJI_CAMERA_FILE_TYPE_MP4                    = 3, /*!< Media file MP4 type. */
    DJI_CAMERA_FILE_TYPE_TIFF                   = 5, /*!< Media file TIFF type. */
    DJI_CAMERA_FILE_TYPE_LDRT                   = 24, /*!< Media file LDRT type. */
    DJI_CAMERA_FILE_TYPE_RPT                    = 25, /*!< Media file RPT type. */
    DJI_CAMERA_FILE_TYPE_UNKNOWN                = 255, /*!< Media file unknown type. */
} E_DjiCameraMediaFileType;

/**
 * @brief Camera optical zoom specifies.
 */
typedef struct {
    uint16_t maxFocalLength; /*!< The maximum focal length of the lens, unit: 0.1mm. */
    uint16_t minFocalLength; /*!< The minimum focal length of the lens, unit: 0.1mm. */
    uint16_t focalLengthStep; /*!< The minimum interval of focal length change, unit: 0.1mm. */
} T_DjiCameraOpticalZoomSpec;

/**
 * @brief Gimbal work mode, specifies how gimbal follow aircraft movement.
 */
typedef enum {
    DJI_GIMBAL_MODE_FREE                        = 0, /*!< Free mode, fix gimbal attitude in the ground coordinate, ignoring movement of aircraft. */
    DJI_GIMBAL_MODE_FPV                         = 1, /*!< FPV (First Person View) mode, only control roll and yaw angle of gimbal in the ground coordinate to follow aircraft. */
    DJI_GIMBAL_MODE_YAW_FOLLOW                  = 2, /*!< Yaw follow mode, only control yaw angle of gimbal in the ground coordinate to follow aircraft. */
} E_DjiGimbalMode;

/**
 * @brief Gimbal rotation mode, specifies control style.
 */
typedef enum {
    DJI_GIMBAL_ROTATION_MODE_RELATIVE_ANGLE     = 0, /*!< Relative angle rotation mode, represents rotating gimbal specified angles based on current angles. */
    DJI_GIMBAL_ROTATION_MODE_ABSOLUTE_ANGLE     = 1, /*!< Absolute angle rotation mode, represents rotating gimbal to specified angles in the ground coordinate. */
    DJI_GIMBAL_ROTATION_MODE_SPEED              = 2, /*!< Speed rotation mode, specifies rotation speed of gimbal in the ground coordinate. */
} E_DjiGimbalRotationMode;

/**
 * @brief Mobile APP system language.
 */
typedef enum {
    DJI_MOBILE_APP_LANGUAGE_UNKNOWN             = 255, /*!< The system language of the mobile app is unknown */
    DJI_MOBILE_APP_LANGUAGE_ENGLISH             = 0, /*!< The system language of the mobile app is English */
    DJI_MOBILE_APP_LANGUAGE_CHINESE             = 1, /*!< The system language of the mobile app is Chinese */
    DJI_MOBILE_APP_LANGUAGE_JAPANESE            = 2, /*!< The system language of the mobile app is Japanese */
    DJI_MOBILE_APP_LANGUAGE_FRENCH              = 3, /*!< The system language of the mobile app is French */
} E_DjiMobileAppLanguage;

/**
 * @brief Mobile APP screen size type.
 */
typedef enum {
    DJI_MOBILE_APP_SCREEN_TYPE_UNKNOWN          = 255, /*!< Mobile APP screen type is unknown. */
    DJI_MOBILE_APP_SCREEN_TYPE_BIG_SCREEN       = 0, /*!< Screen size is 6 inches or larger. */
    DJI_MOBILE_APP_SCREEN_TYPE_LITTLE_SCREEN    = 1, /*!< Screen size is less than 6 inches. */
} E_DjiMobileAppScreenType;

/**
 * @brief Subscription frequency type.
 */
typedef enum {
    DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ            = 1,
    DJI_DATA_SUBSCRIPTION_TOPIC_5_HZ            = 5,
    DJI_DATA_SUBSCRIPTION_TOPIC_10_HZ           = 10,
    DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ           = 50,
    DJI_DATA_SUBSCRIPTION_TOPIC_100_HZ          = 100,
    DJI_DATA_SUBSCRIPTION_TOPIC_200_HZ          = 200,
    DJI_DATA_SUBSCRIPTION_TOPIC_400_HZ          = 400,
} E_DjiDataSubscriptionTopicFreq;

/**
 * @brief DJI module enum for defining data subscription module.
 */
typedef enum {
    DJI_DATA_SUBSCRIPTION_MODULE_FC             = 0,
    DJI_DATA_SUBSCRIPTION_MODULE_CAMERA,
    DJI_DATA_SUBSCRIPTION_MODULE_ERROR,
} E_DjiDataSubscriptionModule;

/**
 * @brief SDK adapter type.
 */
typedef enum {
    DJI_SDK_ADAPTER_TYPE_UNKNOWN                = 0, /*!< SDK adapter type is unknown. */
    DJI_SDK_ADAPTER_TYPE_SKYPORT_V2             = 1, /*!< SDK adapter type is Skyport V2. */
    DJI_SDK_ADAPTER_TYPE_XPORT                  = 2, /*!< SDK adapter type is X-Port. */
    DJI_SDK_ADAPTER_TYPE_NONE                   = 3, /*!< No external adapter is connected. */
} E_DjiSdkAdapterType;

typedef enum {
    DJI_CHANNEL_ADDRESS_UNKNOWN                 = 0,
    DJI_CHANNEL_ADDRESS_PAYLOAD_PORT_NO1,
    DJI_CHANNEL_ADDRESS_PAYLOAD_PORT_NO2,
    DJI_CHANNEL_ADDRESS_PAYLOAD_PORT_NO3,
    DJI_CHANNEL_ADDRESS_EXTENSION_PORT,
    DJI_CHANNEL_ADDRESS_MASTER_RC_APP,
    DJI_CHANNEL_ADDRESS_SLAVE_RC_APP,
    DJI_CHANNEL_ADDRESS_CLOUD_API,
} E_DjiChannelAddress;

/**
 * @brief Camera focus target point when in focus mode.
 */
typedef struct {
    dji_f32_t focusX; /*!< Specifies the horizontal coordinate within the zone. Range: 0 to 1.
                            The point [0.0, 0.0] represents the top-left corner of the screen.*/
    dji_f32_t focusY; /*!< Specifies vertical zone coordinate. Range: 0 to 1. */
} T_DjiCameraPointInScreen;

/**
 * @brief Camera time interval settings for interval shoot-photo mode.
 */
typedef struct {
    uint8_t captureCount; /*!< Specifies the total capture count of interval settings.
 *                             0: reserved, 1-254: specific number, 255: continuous capture until stopped. */
    uint16_t timeIntervalSeconds; /*!< Specifies the interval time between two captures, unit: s*/
} T_DjiCameraPhotoTimeIntervalSettings;

/**
 * @brief Camera zoom speeds.
 */
typedef enum {
    DJI_CAMERA_ZOOM_SPEED_SLOWEST               = 72, /*!< Slowest zoom speed. */
    DJI_CAMERA_ZOOM_SPEED_SLOW                  = 73, /*!< Slow zoom speed. */
    DJI_CAMERA_ZOOM_SPEED_MODERATELY_SLOW       = 74, /*!< Slightly slower than normal zoom speed. */
    DJI_CAMERA_ZOOM_SPEED_NORMAL                = 75, /*!< Normal zoom speed. */
    DJI_CAMERA_ZOOM_SPEED_MODERATELY_FAST       = 76, /*!< Slightly faster than normal zoom speed. */
    DJI_CAMERA_ZOOM_SPEED_FAST                  = 77, /*!< Fast zoom speed. */
    DJI_CAMERA_ZOOM_SPEED_FASTEST               = 78, /*!< Fastest zoom speed. */
} E_DjiCameraZoomSpeed;

typedef enum {
    /*! The burst mode can capture 2 pictures per trigger. */
    DJI_CAMERA_BURST_COUNT_2                    = 2,
    /*! The burst mode can capture 3 pictures per trigger. */
    DJI_CAMERA_BURST_COUNT_3                    = 3,
    /*! The burst mode can capture 5 pictures per trigger. */
    DJI_CAMERA_BURST_COUNT_5                    = 5,
    /*! The burst mode can capture 7 pictures per trigger. */
    DJI_CAMERA_BURST_COUNT_7                    = 7,
    /*! Supports capturing 10 pictures per trigger, only supported by X4S, X5S cameras, and Phantom 4 Pro. */
    DJI_CAMERA_BURST_COUNT_10                   = 10,
    /*! Supports capturing 14 pictures per trigger, only supported by X4S, X5S cameras, and Phantom 4 Pro. */
    DJI_CAMERA_BURST_COUNT_14                   = 14,
    /*! Unknown burst count. */
    DJI_CAMERA_BURST_COUNT_KNOWN                = 0xFF,
} E_DjiCameraBurstCount;

/**
 * @brief Camera zoom direction.
 */
typedef enum {
    DJI_CAMERA_ZOOM_DIRECTION_OUT               = 0, /*!< Zooms out, reducing the zoom factor. */
    DJI_CAMERA_ZOOM_DIRECTION_IN                = 1, /*!< Zooms in, increasing the zoom factor. */
} E_DjiCameraZoomDirection;
#pragma pack (1)

/**
 * @brief Data channel state.
 */
typedef struct {
    /*! Realtime bandwidth limitation, varying with link status of aircraft system for some channels, such as data
     * stream, video stream and download stream. Must ensure actual bandwidth of data transmission is less than
     * realtime bandwidth limitation, unit: byte/s. */
    int32_t realtimeBandwidthLimit;

    /*! Realtime actual transmission bandwidth of data transmission channel calculated before flow controller, unit: byte/s. */
    int32_t realtimeBandwidthBeforeFlowController;

    /*! Realtime actual transmission bandwidth of data transmission channel calculated after flow controller, unit:
     * byte/s. If specified channel without flow controller. the value is equal to
     * ::realtimeBandwidthBeforeFlowController. */
    int32_t realtimeBandwidthAfterFlowController;

    /*! State specified whether the channel is busy or not. When data can not be sent to the endpoint directly, instead be
     * sent to buffer of flow controller or discarded, the busy state will be set. At this time, the user should stop
     * transmitting data or decrease the amount of data to be transmitted via this channel. When data bandwidth restore to
     * normal, this state will be clear again. */
    bool busyState;
} T_DjiDataChannelState;

/**
 * @brief Represents a vector using int32 coordinates.
 */
typedef struct Vector3d{
    int32_t x; /*!< X-coordinate of the vector. */
    int32_t y; /*!< Y-coordinate of the vector. */
    int32_t z; /*!< Z-coordinate of the vector. */
} T_DjiVector3d;

/**
 * @brief Represents a vector using floating-point coordinates.
 */
typedef struct Vector3f{
    dji_f32_t x; /*!< X-coordinate of the vector. */
    dji_f32_t y; /*!< Y-coordinate of the vector. */
    dji_f32_t z; /*!< Z-coordinate of the vector. */
} T_DjiVector3f;

/**
 * @brief Represents an attitude using int32 values for pitch, roll, and yaw.
 */
typedef struct {
    int32_t pitch; /*!< Pitch angle in degrees. */
    int32_t roll;  /*!< Roll angle in degrees. */
    int32_t yaw;   /*!< Yaw angle in degrees. */
} T_DjiAttitude3d;

/**
 * @brief Represents an attitude using floating-point values for pitch, roll, and yaw.
 */
typedef struct {
    dji_f32_t pitch; /*!< Pitch angle in degrees. */
    dji_f32_t roll;  /*!< Roll angle in degrees. */
    dji_f32_t yaw;   /*!< Yaw angle in degrees. */
} T_DjiAttitude3f;

/**
 * @brief Represents a quaternion, when converted to a rotation matrix or Euler angles.
 */
typedef struct {
    dji_f32_t q0; /*!< Quaternion component w. */
    dji_f32_t q1; /*!< Quaternion component x. */
    dji_f32_t q2; /*!< Quaternion component y. */
    dji_f32_t q3; /*!< Quaternion component z. */
} T_DjiQuaternion4f;

/**
 * @brief Timestamp data structure.
 */
typedef struct {
    uint32_t millisecond;
    uint32_t microsecond;
} T_DjiDataTimestamp;

/**
 * @brief The firmware version of payload.
 * @note The firmware version is displayed as AA.BB.CC.DD where AA is majorVersion,
 * BB is minorVersion, CC is modifyVersion, and DD is debugVersion.
 */
typedef struct {
    uint8_t majorVersion;   /*!< Major version number, ranging from 0 to 99. */
    uint8_t minorVersion;   /*!< Minor version number, ranging from 0 to 99. */
    uint8_t modifyVersion;  /*!< Modification version number, ranging from 0 to 99. */
    uint8_t debugVersion;   /*!< Debug version number, ranging from 0 to 99. */
} T_DjiFirmwareVersion;

#pragma pack ()
/**
 * @brief Prototype of callback function used to receive data of topic.
 * @warning User can not execute blocking style operations or functions in the callback function, because that will block PSDK
 * root thread, causing problems such as slow system response, payload disconnection or infinite loop.
 * @param data: pointer to data of the topic, user need transfer type of this pointer to the corresponding data structure
 * pointer for getting every item of the topic conveniently.
 * @param dataSize: the size of memory space pointed by data argument, equal to data structure size corresponding to the topic.
 * @param timestamp: pointer to timestamp corresponding this data. Use flight controller power-on timestamp on M300 RTK.
 * Use payload local timestamp on M30/M30T.
 * @return Execution result.
 */
typedef T_DjiReturnCode (*DjiReceiveDataOfTopicCallback)(const uint8_t *data, uint16_t dataSize,
                                                         const T_DjiDataTimestamp *timestamp);

/* Exported functions --------------------------------------------------------*/


#ifdef __cplusplus
}
#endif

#endif // DJI_TYPEDEF_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
