/**
 ********************************************************************
 * @file    test_ar_pixel_to_gps.h
 * @brief   Sample header for AR SEI pixel-to-GPS conversion.
 *          Unlike the AI SEI geo converter sample, this sample does NOT
 *          require AI detection targets. It uses AR SEI camera pose alone
 *          to project user-specified pixel coordinates to world GPS.
 ********************************************************************
 */

#ifndef TEST_AR_PIXEL_TO_GPS_H
#define TEST_AR_PIXEL_TO_GPS_H

#include "dji_liveview.h"
#include "dji_camera_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Select which PHYSICAL camera source the AR pixel-to-GPS sample uses.
 *
 * Must be called BEFORE DjiUser_RunArPixelToGpsSample(). If never called, the
 * sample keeps the current camera (DEFAULT) and does not touch CameraManager.
 *
 * On H30, wide/zoom is switched via DjiCameraManager_SetStreamSource (NOT via a
 * separate liveview data_idx). The liveview subscription always uses DEFAULT.
 *   DEFAULT_CAM(0) = keep current, WIDE_CAM(1), ZOOM_CAM(2).
 *
 * @param source: physical camera source, see E_DjiCameraManagerStreamSource.
 */
void DjiUser_ArPixelGpsSetSource(E_DjiCameraManagerStreamSource source);

/**
 * @brief Select which payload port (mount position) the camera is on.
 *
 * Must be called BEFORE DjiUser_RunArPixelToGpsSample(). If never called, the
 * sample defaults to DJI_MOUNT_POSITION_PAYLOAD_PORT_NO1. On M400 the payload
 * camera may be on port 1 or port 2.
 *
 * @param position: payload port, see E_DjiMountPosition (PAYLOAD_PORT_NO1/NO2).
 */
void DjiUser_ArPixelGpsSetPosition(E_DjiMountPosition position);

/**
 * @brief Run the AR pixel-to-GPS sample.
 *
 * Demonstrates:
 *   1. Subscribing to H264 stream with AR SEI parsing (no AI dependency)
 *   2. Converting screen center and preset test points to GPS coordinates
 *   3. Computing the camera's own GPS position from AR pose
 *   4. Optionally showing AI target GPS if AI SEI is also available
 *
 * Prerequisites:
 *   - Aircraft connected and powered on with GPS fix
 *   - AR Info enabled on the aircraft
 *   - A valid distance must be configured (default: 50m)
 */
void DjiUser_RunArPixelToGpsSample(void);

/**
 * @brief Interactive entry: prompt (stdin) for payload port + camera source,
 *        then run the AR pixel-to-GPS sample. Use this from a menu / main().
 *
 * Do NOT call this from a background thread (it blocks on stdin). Background
 * callers (e.g. widget interaction) should use DjiUser_RunArPixelToGpsSample()
 * directly, optionally after DjiUser_ArPixelGpsSetPosition/SetSource().
 */
void DjiUser_RunArPixelToGpsSampleInteractive(void);

/**
 * @brief Request the running AR pixel-to-GPS sample to stop.
 * Thread-safe. Called from widget interaction or other threads.
 */
void DjiUser_ArPixelGpsRequestStop(void);

#ifdef __cplusplus
}
#endif

#endif /* TEST_AR_PIXEL_TO_GPS_H */
