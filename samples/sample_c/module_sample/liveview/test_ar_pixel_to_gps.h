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

#ifdef __cplusplus
extern "C" {
#endif

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
 * @brief Request the running AR pixel-to-GPS sample to stop.
 * Thread-safe. Called from widget interaction or other threads.
 */
void DjiUser_ArPixelGpsRequestStop(void);

#ifdef __cplusplus
}
#endif

#endif /* TEST_AR_PIXEL_TO_GPS_H */
