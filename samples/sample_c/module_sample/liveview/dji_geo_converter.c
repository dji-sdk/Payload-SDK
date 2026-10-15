/**
 ********************************************************************
 * @file    dji_geo_converter.c
 * @brief   Implementation of pixel-to-GPS coordinate conversion.
 *
 *
 * Coordinate systems:
 *   NED (North-East-Down): Flight controller attitude output
 *   GND: Camera position from AR SEI (X=North, Y=East, Z=Down)
 *   EUS (East-Up-South): Internal rendering coordinate system
 *   GPS: WGS84 latitude/longitude/altitude
 *
 ********************************************************************
 */

#include "dji_geo_converter.h"
#include <math.h>
#include <string.h>

/* Constants */
#define EARTH_RADIUS        6378137.0       /* Average earth radius in meters */
#define DEG_TO_RAD          (M_PI / 180.0)
#define RAD_TO_DEG          (180.0 / M_PI)
#define BOX_LEN_FACTOR      10000.0f

/* 3D vector type */
typedef struct {
    double x, y, z;
} Vec3;

/* 3x3 matrix (column-major for consistency with glm) */
typedef struct {
    double m[3][3]; /* m[col][row] */
} Mat3;

/* ---- Vector operations ---- */

static Vec3 vec3_sub(Vec3 a, Vec3 b)
{
    return (Vec3){a.x - b.x, a.y - b.y, a.z - b.z};
}

static Vec3 vec3_add(Vec3 a, Vec3 b)
{
    return (Vec3){a.x + b.x, a.y + b.y, a.z + b.z};
}

static Vec3 vec3_scale(Vec3 v, double s)
{
    return (Vec3){v.x * s, v.y * s, v.z * s};
}

static double vec3_length(Vec3 v)
{
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

static Vec3 vec3_normalize(Vec3 v)
{
    double len = vec3_length(v);
    if (len < 1e-12) return (Vec3){0, 0, 0};
    return vec3_scale(v, 1.0 / len);
}

/* ---- Matrix operations ---- */

static Vec3 mat3_mul_vec3(Mat3 m, Vec3 v)
{
    return (Vec3){
        m.m[0][0] * v.x + m.m[1][0] * v.y + m.m[2][0] * v.z,
        m.m[0][1] * v.x + m.m[1][1] * v.y + m.m[2][1] * v.z,
        m.m[0][2] * v.x + m.m[1][2] * v.y + m.m[2][2] * v.z
    };
}

static Mat3 mat3_mul(Mat3 a, Mat3 b)
{
    Mat3 r;
    for (int c = 0; c < 3; c++) {
        for (int row = 0; row < 3; row++) {
            r.m[c][row] = a.m[0][row] * b.m[c][0]
                        + a.m[1][row] * b.m[c][1]
                        + a.m[2][row] * b.m[c][2];
        }
    }
    return r;
}

/**
 * @brief Build rotation matrix from ZYX euler angles (yaw, pitch, roll).
 * Equivalent to glm::eulerAngleZYX(yaw, pitch, roll).
 */
static Mat3 euler_zyx_to_mat3(double yaw, double pitch, double roll)
{
    double cy = cos(yaw),   sy = sin(yaw);
    double cp = cos(pitch), sp = sin(pitch);
    double cr = cos(roll),  sr = sin(roll);

    Mat3 m;
    /* Column 0 */
    m.m[0][0] = cy * cp;
    m.m[0][1] = sy * cp;
    m.m[0][2] = -sp;
    /* Column 1 */
    m.m[1][0] = cy * sp * sr - sy * cr;
    m.m[1][1] = sy * sp * sr + cy * cr;
    m.m[1][2] = cp * sr;
    /* Column 2 */
    m.m[2][0] = cy * sp * cr + sy * sr;
    m.m[2][1] = sy * sp * cr - cy * sr;
    m.m[2][2] = cp * cr;

    return m;
}

/**
 * @brief NED to EUS transformation matrix.
 *
 * NED: X=North, Y=East, Z=Down
 * EUS: X=East,  Y=Up,   Z=South(-North)
 *
 * EUS.x = NED.y (East)
 * EUS.y = -NED.z (Up = -Down)
 * EUS.z = -NED.x (South = -North)
 *
 * Column-major:
 *   col0(NED.x→): [0, 0, -1]  (NED.x maps to -EUS.z)
 *   col1(NED.y→): [1, 0,  0]  (NED.y maps to EUS.x)
 *   col2(NED.z→): [0, -1, 0]  (NED.z maps to -EUS.y)
 */
static Mat3 get_ned2eus(void)
{
    Mat3 m = {{{0, 0, -1}, {1, 0, 0}, {0, -1, 0}}};
    return m;
}

/**
 * @brief Camera view to camera body transformation.
 * OpenGL view: +X right, +Y up, -Z forward
 * Camera body: +X right, -Y up, -Z forward → flip Y and Z
 */
static Mat3 get_view2cam(void)
{
    Mat3 m = {{{1, 0, 0}, {0, -1, 0}, {0, 0, -1}}};
    return m;
}

/**
 * @brief Convert EUS position to GPS coordinates.
 * Converts an EUS-frame position back to WGS84 GPS coordinates.
 */
static void eus_to_gps(Vec3 eus, double originLat, double originLon, double originAlt,
                        double *outLat, double *outLon, double *outAlt)
{
    double originLatRad = remainder(originLat * DEG_TO_RAD, M_PI);
    double originLonRad = remainder(originLon * DEG_TO_RAD, M_PI * 2.0);

    /* lat: z is south direction, so subtract */
    double latRad = originLatRad - eus.z / EARTH_RADIUS;

    /* lon: x is east direction, divide by cos(lat) for longitude correction */
    double cosLat = cos(latRad);
    double lonRad = 0.0;
    if (fabs(cosLat) > 1e-7) {
        lonRad = eus.x / (EARTH_RADIUS * cosLat) + originLonRad;
    }

    /* alt: y is up */
    double alt = eus.y + originAlt;

    *outLat = latRad * RAD_TO_DEG;
    *outLon = lonRad * RAD_TO_DEG;
    *outAlt = alt;

    /* Normalize ranges */
    while (*outLat > 90.0)  *outLat -= 180.0;
    while (*outLat < -90.0) *outLat += 180.0;
    while (*outLon > 180.0) *outLon -= 360.0;
    while (*outLon < -180.0) *outLon += 360.0;
}

/* ---- Public API ---- */

T_DjiReturnCode DjiGeoConverter_PixelToGps(
    const T_DjiSeiArInfo *arInfo,
    const T_DjiGeoCameraIntrinsics *intrinsics,
    float screenPercentX,
    float screenPercentY,
    float distanceMeters,
    T_DjiGeoGpsResult *result)
{
    if (!arInfo || !intrinsics || !result) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }
    if (intrinsics->focalLength == 0 || intrinsics->imgWidth == 0 || intrinsics->imgHeight == 0) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }
    if (distanceMeters <= 0.0f) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    memset(result, 0, sizeof(*result));

    /* Step 1: Normalized screen → pixel coordinates */
    double imgW = (double)intrinsics->imgWidth;
    double imgH = (double)intrinsics->imgHeight;
    double u = screenPercentX * imgW;
    double v = screenPercentY * imgH;

    /* Step 2: Pixel → camera-frame ray direction (pinhole model)
     * focalLength, imgWidth, and imgHeight are from FOV SEI and share the
     * same unit system. Their ratio directly gives the correct angular FOV:
     *   tan(hFov/2) = (imgWidth/2) / focalLength
     * No unit conversion is needed.
     */
    double focal = (double)intrinsics->focalLength;
    double cx = imgW / 2.0;
    double cy = imgH / 2.0;
    Vec3 dirCam = {(u - cx) / focal, (v - cy) / focal, 1.0};

    /* Step 3: Build NED→EUS rotation for camera direction
     * R = R_ned2eus × R_cam2ned × R_view2cam
     * where R_cam2ned = eulerAngleZYX(yaw, pitch, roll) in radians
     */
    double yawRad   = arInfo->attitude.yaw   * DEG_TO_RAD;
    double pitchRad = arInfo->attitude.pitch * DEG_TO_RAD;
    double rollRad  = arInfo->attitude.roll  * DEG_TO_RAD;

    Mat3 R_cam2ned = euler_zyx_to_mat3(yawRad, pitchRad, rollRad);
    Mat3 R_ned2eus = get_ned2eus();
    Mat3 R_view2cam = get_view2cam();

    Mat3 R = mat3_mul(mat3_mul(R_ned2eus, R_cam2ned), R_view2cam);

    /* Step 4: Camera position GND(NED) → EUS */
    Vec3 camPosNed = {arInfo->position.x, arInfo->position.y, arInfo->position.z};
    Vec3 camPosEus = mat3_mul_vec3(R_ned2eus, camPosNed);

    /* Step 5: Ray casting — target = camPos + normalize(R * dirCam) * distance */
    Vec3 dirWorld = mat3_mul_vec3(R, dirCam);
    Vec3 dirNorm = vec3_normalize(dirWorld);
    Vec3 targetEus = vec3_add(camPosEus, vec3_scale(dirNorm, (double)distanceMeters));

    /* Step 6: EUS → GPS */
    eus_to_gps(targetEus,
               arInfo->gndOrigin.latitude,
               arInfo->gndOrigin.longitude,
               arInfo->gndOrigin.altitude,
               &result->latitude,
               &result->longitude,
               &result->altitude);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiGeoConverter_AiTargetToGps(
    const T_DjiSeiArInfo *arInfo,
    const T_DjiGeoCameraIntrinsics *intrinsics,
    const T_DjiSeiAiBbox *box,
    T_DjiGeoGpsResult *result)
{
    if (!box || box->distance == 0) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    float percentX = (float)box->boxCenterX / BOX_LEN_FACTOR;
    float percentY = (float)box->boxCenterY / BOX_LEN_FACTOR;
    float distMeters = (float)box->distance / 1000.0f;

    return DjiGeoConverter_PixelToGps(arInfo, intrinsics,
                                      percentX, percentY, distMeters, result);
}
