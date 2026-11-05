/**
 ********************************************************************
 * @file    test_sei_geo_converter.h
 * @brief   Sample header for AR SEI + AI SEI → GPS coordinate conversion.
 ********************************************************************
 */

#ifndef TEST_SEI_GEO_CONVERTER_H
#define TEST_SEI_GEO_CONVERTER_H

#include "dji_liveview.h"
#include "dji_camera_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Select which PHYSICAL camera source the AI target-to-GPS sample uses.
 *
 * Must be called BEFORE DjiUser_RunSeiGeoConverterSample(). If never called, the
 * sample keeps the current camera (DEFAULT) and does not touch CameraManager.
 *
 * On H30, wide/zoom is switched via DjiCameraManager_SetStreamSource (NOT via a
 * separate liveview data_idx). The liveview subscription always uses DEFAULT.
 *   DEFAULT_CAM(0) = keep current, WIDE_CAM(1), ZOOM_CAM(2).
 *
 * @param source: physical camera source, see E_DjiCameraManagerStreamSource.
 */
void DjiUser_SeiGeoConverterSetSource(E_DjiCameraManagerStreamSource source);

/**
 * @brief Select which payload port (mount position) the camera is on.
 *
 * Must be called BEFORE DjiUser_RunSeiGeoConverterSample(). Defaults to
 * DJI_MOUNT_POSITION_PAYLOAD_PORT_NO1. On M400 the payload may be on port 1 or 2.
 *
 * @param position: payload port, see E_DjiMountPosition (PAYLOAD_PORT_NO1/NO2).
 */
void DjiUser_SeiGeoConverterSetPosition(E_DjiMountPosition position);

/**
 * @brief Run the SEI geo converter sample.
 *
 * This sample demonstrates:
 *   1. Subscribing to H264 stream with AR + AI SEI parsing
 *   2. Obtaining camera focal length via PSDK camera API
 *   3. Converting AI detection target pixel coordinates to GPS
 *
 * Prerequisites:
 *   - Aircraft connected and powered on
 *   - AR Info enabled (DJIFlightAssistantKey.KeyIsARInfoOn = true)
 *   - AI detection active on the camera
 */
void DjiUser_RunSeiGeoConverterSample(void);

/**
 * @brief Interactive entry: prompt (stdin) for payload port + camera source,
 *        then run the AI target-to-GPS sample. Use this from a menu / main().
 *
 * Do NOT call this from a background thread (it blocks on stdin).
 */
void DjiUser_RunSeiGeoConverterSampleInteractive(void);

#ifdef __cplusplus
}
#endif

#endif /* TEST_SEI_GEO_CONVERTER_H */
