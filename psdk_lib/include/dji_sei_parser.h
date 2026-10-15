/**
 ********************************************************************
 * @file    dji_sei_parser.h
 * @brief   This is the header file defining the AI SEI recognition data types
 * used in liveview stream callbacks.
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
 * If you receive this source code without DJI's authorization, you may not
 * further disseminate the information, and you must immediately remove the
 * source code and notify DJI of its removal. DJI reserves the right to pursue
 * legal actions against you for any loss(es) or damage(s) caused by your
 * failure to do so.
 *
 *********************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef DJI_SEI_PARSER_H
#define DJI_SEI_PARSER_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/
#define DJI_TARGET_LIMIT_CNT                    128  /*!< Maximum number of AI targets per frame */

/* Exported types ------------------------------------------------------------*/

/* ============ AR SEI data structures (TLV type=4) ============ */

/**
 * @brief Camera attitude in NED coordinate system, parsed from AR SEI.
 */
typedef struct {
    double pitch;       /*!< Pitch angle, unit: degrees */
    double roll;        /*!< Roll angle, unit: degrees */
    double yaw;         /*!< Yaw angle, unit: degrees */
} T_DjiSeiCameraAttitude;

/**
 * @brief Camera position in GND coordinate system, parsed from AR SEI.
 * GND: X=North, Y=East, Z=Down. Origin is gndOrigin GPS location.
 */
typedef struct {
    double x;           /*!< Position north, unit: meters */
    double y;           /*!< Position east, unit: meters */
    double z;           /*!< Position down, unit: meters */
} T_DjiSeiCameraPosition;

/**
 * @brief GND coordinate system origin GPS location.
 */
typedef struct {
    double latitude;    /*!< Latitude, unit: degrees */
    double longitude;   /*!< Longitude, unit: degrees */
    double altitude;    /*!< Altitude, unit: meters */
} T_DjiSeiGndOrigin;

/**
 * @brief Complete AR camera pose information parsed from H264/H265 stream SEI (TLV type=4).
 *
 * Contains camera extrinsic parameters: attitude (NED euler angles),
 * position (GND local coordinates), and the GPS reference origin.
 *
 * @note The pointer is only valid during the callback scope. Do not cache it.
 */
typedef struct {
    uint8_t version;            /*!< Protocol version */
    uint8_t incremental;        /*!< Incremental flag */
    uint8_t incrementalSeq;     /*!< Incremental sequence number */
    uint16_t deltaTimestamp;    /*!< Delta timestamp, unit: ms */
    T_DjiSeiCameraAttitude attitude;  /*!< Camera attitude in NED */
    T_DjiSeiCameraPosition position;  /*!< Camera position in GND */
    T_DjiSeiGndOrigin gndOrigin;      /*!< GND origin GPS coordinate */
} T_DjiSeiArInfo;

/* ============ FOV SEI data structures (TLV type=10) ============ */

/**
 * @brief Camera FOV / intrinsic information parsed from H264/H265 stream SEI (TLV type=10).
 *
 * Provides camera sensor dimensions and focal length in the same unit system,
 * enabling correct pinhole-model ray construction for pixel-to-GPS conversion.
 *
 * @note horizontal, vertical, and focalLength share the same unit.
 *       Their ratio (e.g. vertical / (2 * focalLength)) directly yields tan(vFov/2).
 */
typedef struct {
    uint8_t  version;           /*!< Protocol version */
    uint32_t horizontal;        /*!< Sensor / imaging width (same unit as focalLength) */
    uint32_t vertical;          /*!< Sensor / imaging height (same unit as focalLength) */
    uint32_t focalLength;       /*!< Current actual focal length (same unit as horizontal/vertical) */
} T_DjiSeiFovInfo;

/* ============ AI SEI data structures (TLV type=7) ============ */

/**
 * @brief AI target bounding box information.
 *
 * Coordinate system: normalized to [0-10000] range.
 * - boxCenterX/boxCenterY: unit is 1/10000 * screen width/height
 * - boxWidth/boxHeight: unit is 1/10000 * screen width/height
 * - distance: unit is millimeters (mm)
 */
typedef struct {
    uint16_t boxCenterX;      /*!< Center X coordinate [0-10000] */
    uint16_t boxCenterY;      /*!< Center Y coordinate [0-10000] */
    uint16_t boxWidth;        /*!< Box width */
    uint16_t boxHeight;       /*!< Box height */
    uint32_t distance;        /*!< Distance to target, unit: mm */
} T_DjiSeiAiBbox;

/**
 * @brief Single AI target information.
 */
typedef struct {
    uint16_t targetId;        /*!< Target unique ID */
    uint8_t  targetType;      /*!< Target classification type */
    uint8_t  targetState;     /*!< Recognition state */
    T_DjiSeiAiBbox bbox;      /*!< Target bounding box */
} T_DjiSeiAiTarget;

/**
 * @brief Target type count entry.
 */
typedef struct {
    uint8_t  type;            /*!< Target type */
    uint16_t count;           /*!< Count of this type */
} T_DjiSeiAiTypeCount;

/**
 * @brief Segmentation mask information.
 */
typedef struct {
    uint32_t width;           /*!< Mask width */
    uint32_t height;          /*!< Mask height */
    float    widthScale;      /*!< Width scale factor */
    float    heightScale;     /*!< Height scale factor */
    uint32_t maskSize;        /*!< Compressed mask bytes count */
    const uint8_t *maskData;  /*!< Compressed mask data pointer (points into internal buffer, do not free) */
    uint8_t *decodedMask;     /*!< RLE decoded mask data (do not free, managed by SDK) */
    uint32_t decodedMaskSize; /*!< Decoded mask data size in bytes */
} T_DjiSeiAiMask;

/**
 * @brief Complete AI recognition information parsed from H264/H265 stream SEI.
 *
 * This structure is passed to user callbacks registered via
 * DjiLiveview_StartH264StreamWithSei() or DjiLiveview_StartStreamWithSei().
 *
 * @note The pointer is only valid during the callback scope. Do not cache it.
 */
typedef struct {
    uint8_t  version;         /*!< Protocol version */
    uint32_t timestamp;       /*!< Flight controller clock timestamp, unit: ms */
    uint8_t  frameIdType;     /*!< 0=INVALID, 1=BASE, 3=AIBOX */
    uint8_t  sourceId;        /*!< Stream source: 1=ZOOM, 2=WIDE */
    uint16_t trackId;         /*!< Tracking ID */
    uint8_t  groupCount;      /*!< Number of groups */

    /* group type 10: target boxes */
    uint8_t  targetNum;       /*!< Number of targets */
    T_DjiSeiAiTarget targets[DJI_TARGET_LIMIT_CNT];

    /* group type 12: type counts */
    uint8_t  typeCountNum;    /*!< Number of type count entries */
    T_DjiSeiAiTypeCount typeCounts[DJI_TARGET_LIMIT_CNT];

    /* group type 14: segmentation mask */
    bool     hasMask;         /*!< Whether mask data is present */
    T_DjiSeiAiMask mask;      /*!< Segmentation mask */
} T_DjiSeiAiInfo;

/**
 * @brief Unified SEI information container for all parsed SEI modules.
 *
 * Passed to user callbacks registered via DjiLiveview_StartStreamWithSei().
 * Each sub-module has a boolean flag indicating whether data is present.
 * Future SEI modules (EIS, FOV, etc.) can be added without changing the
 * callback signature.
 *
 * @note The pointer is only valid during the callback scope. Do not cache it.
 */
typedef struct {
    bool hasAi;               /*!< Whether AI recognition data is present */
    T_DjiSeiAiInfo ai;        /*!< AI recognition data (TLV type=7) */

    bool hasAr;               /*!< Whether AR camera pose data is present */
    T_DjiSeiArInfo ar;        /*!< AR camera pose data (TLV type=4) */

    bool hasFov;              /*!< Whether FOV / camera intrinsic data is present */
    T_DjiSeiFovInfo fov;      /*!< FOV camera intrinsic data (TLV type=10) */
} T_DjiSeiInfo;

#ifdef __cplusplus
}
#endif

#endif // DJI_SEI_PARSER_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
