/**
 ********************************************************************
 * @file    test_sei_geo_converter.h
 * @brief   Sample header for AR SEI + AI SEI → GPS coordinate conversion.
 ********************************************************************
 */

#ifndef TEST_SEI_GEO_CONVERTER_H
#define TEST_SEI_GEO_CONVERTER_H

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif /* TEST_SEI_GEO_CONVERTER_H */
