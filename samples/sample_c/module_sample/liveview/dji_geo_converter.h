/**
 ********************************************************************
 * @file    dji_geo_converter.h
 * @brief   Pixel-to-GPS coordinate converter using AR SEI camera pose
 *          and camera intrinsics (focal length).
 *
 * Converts normalized screen coordinates + distance into real-world
 * GPS latitude/longitude/altitude using:
 *   - Camera extrinsics from AR SEI (attitude, position, GPS origin)
 *   - Camera intrinsics (focal length, image dimensions)
 *
 * Mathematical basis:
 *   1. Pixel → Camera-frame ray direction (pinhole model)
 *   2. NED euler angles → EUS rotation matrix
 *   3. Ray casting: target = camPos + normalize(dir) * distance
 *   4. EUS local coords → GPS (flat-earth approximation)
 *
 ********************************************************************
 */

#ifndef DJI_GEO_CONVERTER_H
#define DJI_GEO_CONVERTER_H

#include "dji_sei_parser.h"
#include "dji_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Camera intrinsic parameters needed for pixel-to-world projection.
 *
 * @note focalLength, imgWidth, and imgHeight MUST be in the same unit system.
 *       The recommended source is T_DjiSeiFovInfo from FOV SEI (TLV type=10),
 *       where horizontal/vertical/focalLength share the same unit and their
 *       ratio directly yields the correct field-of-view angles.
 *
 *       Usage:  intrinsics.focalLength = fovInfo.focalLength;
 *               intrinsics.imgWidth    = fovInfo.horizontal;
 *               intrinsics.imgHeight   = fovInfo.vertical;
 */
typedef struct {
    uint32_t focalLength;   /*!< Focal length (from FOV SEI, same unit as imgWidth/imgHeight) */
    uint32_t imgWidth;      /*!< Imaging width (from FOV SEI horizontal) */
    uint32_t imgHeight;     /*!< Imaging height (from FOV SEI vertical) */
} T_DjiGeoCameraIntrinsics;

/**
 * @brief GPS coordinate result.
 */
typedef struct {
    double latitude;    /*!< Latitude in degrees */
    double longitude;   /*!< Longitude in degrees */
    double altitude;    /*!< Altitude in meters */
} T_DjiGeoGpsResult;

/**
 * @brief Convert normalized screen coordinates + distance to GPS.
 *
 * @param arInfo: Camera pose from AR SEI (attitude, position, GPS origin).
 * @param intrinsics: Camera intrinsic parameters (focal length, image size).
 * @param screenPercentX: Normalized screen X coordinate [0.0, 1.0].
 * @param screenPercentY: Normalized screen Y coordinate [0.0, 1.0].
 * @param distanceMeters: Distance from camera to target in meters.
 * @param result: Output GPS coordinate.
 * @return Execution result.
 */
T_DjiReturnCode DjiGeoConverter_PixelToGps(
    const T_DjiSeiArInfo *arInfo,
    const T_DjiGeoCameraIntrinsics *intrinsics,
    float screenPercentX,
    float screenPercentY,
    float distanceMeters,
    T_DjiGeoGpsResult *result);

/**
 * @brief Convert an AI detection bounding box to GPS using its center + distance.
 *
 * @param arInfo: Camera pose from AR SEI.
 * @param intrinsics: Camera intrinsic parameters.
 * @param box: AI detection bounding box (normalized [0-10000] + distance in mm).
 * @param result: Output GPS coordinate.
 * @return Execution result. Returns error if box distance is 0 (invalid).
 */
T_DjiReturnCode DjiGeoConverter_AiTargetToGps(
    const T_DjiSeiArInfo *arInfo,
    const T_DjiGeoCameraIntrinsics *intrinsics,
    const T_DjiSeiAiBbox *box,
    T_DjiGeoGpsResult *result);

#ifdef __cplusplus
}
#endif

#endif /* DJI_GEO_CONVERTER_H */
