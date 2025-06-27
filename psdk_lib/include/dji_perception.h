/**
 ********************************************************************
 * @file    dji_perception.h
 * @brief   This is the header file for "dji_perception.c", defining the structure and
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
#ifndef DJI_PERCEPTION_H
#define DJI_PERCEPTION_H

/* Includes ------------------------------------------------------------------*/
#include "dji_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/
#define IMAGE_MAX_DIRECTION_NUM                     6
#define DJI_PERCEPTION_INTRINSICS_PARAM_ARRAY_NUM   9
#define DJI_PERCEPTION_ROTATION_PARAM_ARRAY_NUM     9
#define DJI_PERCEPTION_TRANSLATION_PARAM_ARRAY_NUM  3
#define DJI_PTS_NUM_PER_PKG                         96
#define DJI_LIDAR_PKG_BUFFER_NUM                    564

/* Exported types ------------------------------------------------------------*/
/**
 * @bref Perception camera direction
 */
typedef enum {
    DJI_PERCEPTION_RECTIFY_DOWN = 0,
    DJI_PERCEPTION_RECTIFY_FRONT = 1,
    DJI_PERCEPTION_RECTIFY_REAR = 2,
    DJI_PERCEPTION_RECTIFY_UP = 3,
    DJI_PERCEPTION_RECTIFY_LEFT = 4,
    DJI_PERCEPTION_RECTIFY_RIGHT = 5
} E_DjiPerceptionDirection;

/**
 * @bref Perception camera design location
 */
typedef enum {
    RECTIFY_DOWN_LEFT = 1,
    RECTIFY_DOWN_RIGHT = 2,
    RECTIFY_FRONT_LEFT = 3,
    RECTIFY_FRONT_RIGHT = 4,
    RECTIFY_REAR_LEFT = 5,
    RECTIFY_REAR_RIGHT = 6,
    RECTIFY_UP_LEFT = 21,
    RECTIFY_UP_RIGHT = 22,
    RECTIFY_LEFT_LEFT = 23,
    RECTIFY_LEFT_RIGHT = 24,
    RECTIFY_RIGHT_LEFT = 25,
    RECTIFY_RIGHT_RIGHT = 26
} E_DjiPerceptionCameraPosition;

/**
 * @bref Perception radar design location
 */
typedef enum {
    RADAR_POSITION_LEFT = 0,
    RADAR_POSITION_RIGHT = 1,
    RADAR_POSITION_DOWN =2,
    RADAR_POSITION_UP = 3,
    RADAR_POSITION_FRONT = 4,
    RADAR_POSITION_BACK =5,
    MAX_RADAR_NUM = 6,
}E_DjiPerceptionRadarPosition;

#pragma pack(1)
/**
 * @bref Perception camera ram image info
 */
typedef struct {
    uint32_t index;
    uint8_t direction;
    uint8_t bpp;
    uint32_t width;
    uint32_t height;
} T_DjiPerceptionRawImageInfo;

/**
 * @bref Perception camera image info
 */
typedef struct {
    T_DjiPerceptionRawImageInfo rawInfo;
    uint16_t dataId;
    uint16_t sequence;
    //see enum E_DjiPerceptionCamPosition
    uint32_t dataType;
    uint64_t timeStamp;
} T_DjiPerceptionImageInfo;

/**
 * @bref Perception camera parameters
 */
typedef struct {
    uint8_t direction;
    float leftIntrinsics[DJI_PERCEPTION_INTRINSICS_PARAM_ARRAY_NUM];
    float rightIntrinsics[DJI_PERCEPTION_INTRINSICS_PARAM_ARRAY_NUM];
    float rotationLeftInRight[DJI_PERCEPTION_ROTATION_PARAM_ARRAY_NUM];
    float translationLeftInRight[DJI_PERCEPTION_TRANSLATION_PARAM_ARRAY_NUM];
} T_DjiPerceptionCameraParameters;

/**
 * @bref Perception camera parameters packet
 */
typedef struct {
    uint32_t timeStamp;
    uint32_t directionNum;
    T_DjiPerceptionCameraParameters cameraParameters[IMAGE_MAX_DIRECTION_NUM];
} T_DjiPerceptionCameraParametersPacket;

/**
 * @bref Perception Lidar data point info
 */
typedef struct{
    float   x; /*!< unit: meters */
    float   y; /*!< unit: meters */
    float   z; /*!< unit: meters */
    uint8_t intensity;
    uint8_t label;  /*!< Noise filtering results (0: obj; 1: noise; 2: unknow; 3: not_retuen ) */
} T_DJIPerceptionLidarPoint;

/**
 * @bref Perception Lidar data header of each pkg
 */
typedef struct{
    uint16_t timeInterval; /*!< The point cloud sampling time (in 0.1us)  */
    uint16_t dotNum; /*!< Current packet data field contains the number of points This field is not valid for non-repeat scans*/
    uint8_t  dataType; /** dataType
                        *
                        * Bit Position (7-4) | Field Name   | Description          | Remarks
                        * ------------------ | ------------ | -------------------- | -------------------------------------------------
                        * 4-7                | echo_mode    | Echo Mode            | 0: Single echo mode
                        *                    |              |                      | 1: Dual echo mode
                        *                    |              |                      | 2: Triple echo mode
                        *                    |              |                      | 3: Quadruple echo mode
                        *                    |              |                      | 4: Quintuple echo mode
                        *
                        * Bit Position (3-0) | Field Name   | Description          | Remarks
                        * ------------------ | ------------ | -------------------- | -------------------------------------------------
                        * 0-3                | data_type    | Data Type            | 40: IMU Data
                        *                    |              |                      | 1: Point cloud data (rectangular, 32-bit)
                        *                    |              |                      | 2: Point cloud data (rectangular, 16-bit default)
                        *                    |              |                      | 3: Point cloud data (spherical)
                        *                    |              |                      | 4: Point cloud data (rectangular, 20-bit, in use)
                        */
    uint8_t  timeType; /** timeType
                        * Timestamp Type | Sync Source Type    | Data Format   | Description
                        * -------------- | ------------------- | ------------- | -------------------------------------------------
                        * 0              | No sync source,     | uint64_t      | Timestamp is radar uptime, unit: ns
                        *                |                     |               |
                        * 1              | gPTP/PTP sync,      | uint64_t      | Timestamp is master clock source time, unit: ns
                        *                |                     |               |
                        * 2              | PPS + ns time sync  | uint64_t      | Unit: ns
                        *                |                     |               |
                        * 3              | PPS + UTC           | uint64_t      | UTC format is:
                        *                |                     | struct        | struct {
                        *                |                     |               |     uint8_t year;
                        *                |                     |               |     uint8_t mon;
                        *                |                     |               |     uint8_t day;
                        *                |                     |               |     uint8_t hour;
                        *                |                     |               |     uint32_t us_offset;
                        *                |                     |               | };
                        */
    uint64_t timeStamp;
}T_DJIPerceptionLidarDataHeader;

/**
 * @bref Perception Lidar data pkg
 */
typedef struct{
    T_DJIPerceptionLidarDataHeader  header;
    T_DJIPerceptionLidarPoint       points[DJI_PTS_NUM_PER_PKG];
}T_DjiPerceptionLidarDecodePkg;

/**
 * @bref Perception Lidar data frame
 */
typedef struct{
    uint64_t  timeStampNs; /*!< Timestamp of the first point of each packet */
    uint32_t  frameCnt; /*!< in increasing order from zero */
    uint16_t  pkgNum;  /*!< Number of valid pkgs per frame */
    T_DjiPerceptionLidarDecodePkg pkgs[DJI_LIDAR_PKG_BUFFER_NUM];
    uint32_t  poseTimeMs;
    uint16_t  naviFlag; /** naviFlag:
                        * Bit Position | Field Name | Description                                      | Remarks
                        * ------------ | ---------- | ------------------------------------------------ | -----------------------------------
                        * 0            | vel_x      | Horizontal x-axis velocity valid bit (1 valid)   | 1 - valid, 0 - invalid
                        * 1            | vel_y      | Horizontal y-axis velocity valid bit (1 valid)   | 1 - valid, 0 - invalid
                        * 2            | vel_z      | Vertical velocity valid bit (1 valid)            | 1 - valid, 0 - invalid
                        * 3            | pos_x      | Horizontal x-axis position valid bit (1 valid)   | 1 - valid, 0 - invalid
                        * 4            | pos_y      | Horizontal y-axis position valid bit (1 valid)   | 1 - valid, 0 - invalid
                        * 5            | pos_z      | Vertical position valid bit (1 valid)            | 1 - valid, 0 - invalid
                        * 6            | dwn_vz     | Ground speed valid bit (1 valid)                 | 1 - valid, 0 - invalid
                        * 7            | dwn_pz     | Ground elevation valid bit (1 valid)             | 1 - valid, 0 - invalid
                        * 8            | rtk_pxy    | RTK horizontal valid flag (1 valid)              | 1 - valid, 0 - invalid
                        * 9            | rtk_pz     | RTK vertical valid flag (1 valid)                | 1 - valid, 0 - invalid
                        * 10           | gns_ll     | GPS horizontal valid flag (1 valid)              | 1 - valid, vertical direction always invalid
                        * 11           | fg_ok      | FG estimate OK flag (1 valid)                    | 1 - valid, 0 - invalid
                        * 12-15        | fg_st      | FG mode (4 bits)                                 | Modes:
                        *              |            |                                                  | 0 - Random initialization
                        *              |            |                                                  | 1 - Initialization with poor compass
                        *              |            |                                                  | 2 - Initialization with good compass
                        *              |            |                                                  | 3 - Magnetic inclination compensation
                        *              |            |                                                  | 4 - Compass fix during takeoff
                        *              |            |                                                  | 5 - Compass fix in the air
                        *              |            |                                                  | 6 - Compass calibration fix
                        *              |            |                                                  | 7 - Accelerometer alignment
                        *              |            |                                                  | 8 - Speed alignment
                        *              |            |                                                  | 9 - RTK heading alignment
                        */
    float     naviPos[3]; /*!< UAV IMU to navigation coordinate system translation vector (xyz) */
    float     naviQuat[4];  /*!<  Quaternions from UAV IMU to navigation coordinate system */
}T_DjiLidarFrame;

/**
 * @bref Radar data frame header
 */
typedef struct {
    uint16_t dataLen;
    uint8_t  curPack;  /*!< The current packet number, in the range of [1, pack_num]. */
    uint8_t  packNum;  /*!< Total number of current circle packets, starting from 1. */
} T_DjiRadarDataHeader;

/**
 * @bref Radar information structure
 */
typedef struct {
    uint64_t velocity  :16; /**
                            * Calculating the actual velocity:
                            *
                            * Actual velocity (m/s) = (Velocity - 32767) / 100
                            *
                            * Interpreting the results:
                            * - If the result is > 0, it indicates that the object is moving closer to the target.
                            * - If the result is < 0, it indicates that the object is moving away from the target.
                            */
    uint64_t snr       :7; /*!< target SNR, in db, ranging from 0 to 127, with 0 being the null point or no echo point
                            (base_noise calculation base_noise = energy / snr) */
    uint64_t beamAngle:10; /**
                            * Beam emission angle value:
                            * - Unit: 0.01 degrees
                            * - Stored range: 0~1023
                            * - Actual value range: [-45°, 45°]
                            *
                            * Conversion method:
                            * - Divide stored value x by 10
                            * - If x ≤ 450, the angle remains unchanged
                            * - If x > 450, the angle is adjusted by subtracting 90
                            *
                            * Examples:
                            * - Stored value of 449 yields an angle of 44.9° (449/10 = 44.9°)
                            * - Stored value of 451 yields an angle of -44.9° (451/10 - 90 = -44.9°)
                            * invalid in M400
                            */

    uint64_t radarType:3; /*!< Radar numbe,is invalid in M400*/

    uint64_t clitterFlag:1; /*!< Flag for clutter point in planar radar: 1 indicates a clutter point, 0 indicates a valid point  */

    uint64_t reserved  :27;
} T_DjiRadarBaseInfo;

/**
 * @bref The single point data structure of millimeter wave radar
 */
typedef struct {
    uint16_t azimuth; /*!
                        * Target azimuth in radar coordinate system:
                        * - Unit: 0.001 radian
                        * - Value range: (0 to 2π) / 0.001
                        * - Calculation method (azimuth/1000 - 2π)
                        */
    uint16_t elevation; /*!
                        * Target pitch angle in radar coordinate system:
                        * - Unit: 0.001 radian
                        * - Value range: (0 to 2π) / 0.001
                        * - Calculation method (elevation/1000 - 2π)
                        */
    uint16_t radius; /*!
                        * Target radial distance in radar coordinate system:
                        * - Unit: 0.01 meters
                        * - Value range: 0 to 65553 (in steps of 0.01 meters)
                        */
    uint16_t ene; /*!
                    * Radar target energy
                    * - Actual value: energy / 100
                    * - invalid in M400
                    */
    T_DjiRadarBaseInfo base_info;
}T_DjiRadarCloudUnit;

/**
 * @bref Perception Radar data frame
 */
typedef struct {
    T_DjiRadarDataHeader headInfo;
    T_DjiRadarCloudUnit data[1];
} T_DjiRadarDataFrame;

#pragma pack()

/**
 * @bref Callback type to receive stereo camera image
 * @note It is not recommended to perform blocking operations in the callback, as it may lead to data loss.
 */
typedef void(*DjiPerceptionImageCallback)(T_DjiPerceptionImageInfo imageInfo, uint8_t *imageRawBuffer,
                                          uint32_t bufferLen);

/**
 * @bref Callback type to receive radar data
 * @note It is not recommended to perform blocking operations in the callback, as it may lead to data loss.
 */
typedef void(*DjiPerceptionRadarCallback)(E_DjiPerceptionRadarPosition radarPosition, uint8_t *radarDataBuffer,
                                          uint32_t bufferLen);
/**
 * @bref Callback type to process Lidar data
 * @note It is not recommended to perform blocking operations in the callback, as it may lead to data loss.
 */
typedef void(*DjiPerceptionLidarDataCallback)(uint8_t* lidarDataBuffer, uint32_t bufferLen);

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Initialize the perception module.
 * @note The interface initialization needs to be after DjiCore_Init.
 * @return Execution result.
 */
T_DjiReturnCode DjiPerception_Init(void);

/**
 * @brief Deinitialize the perception module.
 * @return Execution result.
 */
T_DjiReturnCode DjiPerception_Deinit(void);

/**
 * @brief Subscribe the raw images of both stereo cameras in the same direction. Default frequency at 20 Hz.
 * @param direction: direction to specify the direction of the subscription. Ref to E_DjiPerceptionDirection
 * @param callback callback to observer the stereo camera image and info.
 * @return Execution result.
 */
T_DjiReturnCode DjiPerception_SubscribePerceptionImage(E_DjiPerceptionDirection direction,
                                                       DjiPerceptionImageCallback callback);

/**
 * @brief Unsubscribe the raw image of both stereo cameras in the same direction.
 * @param direction: direction to specify the direction of the subscription. Ref to E_DjiPerceptionDirection.
 * @return Execution result.
 */
T_DjiReturnCode DjiPerception_UnsubscribePerceptionImage(E_DjiPerceptionDirection direction);

/**
 * @brief Get the internal and external parameters of all stereo cameras.
 * @return Execution result.
 */
T_DjiReturnCode DjiPerception_GetStereoCameraParameters(T_DjiPerceptionCameraParametersPacket *packet);

/**
 * @brief Subscribe the lidar data.
 * @param callback: callback to observer the radar data.
 * @return Execution result.
 */
T_DjiReturnCode DjiPerception_SubscribeLidarData(DjiPerceptionLidarDataCallback callback);

/**
 * @brief  Unsubscribe the lidar data.
 *  @return Execution result.
 */
T_DjiReturnCode DjiPerception_UnsubscribeLidarData(void);

/**
 * @brief Subscribe the lidar data of the position.
 * @param position: position the radar monted
 * @param callback callback to observer the radar data.
 * @return Execution result.
 */
T_DjiReturnCode DjiPerception_SubscribeRadarData(E_DjiPerceptionRadarPosition position, DjiPerceptionRadarCallback callback);

/**
 * @brief Unsubscribe the lidar data of the position.
 * @param position: position the radar monted
 * @return Execution result.
 */
T_DjiReturnCode DjiPerception_UnsubscribeRadarData(E_DjiPerceptionRadarPosition position);
#ifdef __cplusplus
}
#endif

#endif // DJI_PERCEPTION_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
