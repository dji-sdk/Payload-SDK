/**
 ********************************************************************
 * @file    psdk_payload_collaboration.h
 * @brief   This is the header file for "psdk_payload_collaboration.c", defining the structure and
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
#ifndef PSDK_PAYLOAD_COLLABORATION_H
#define PSDK_PAYLOAD_COLLABORATION_H

/* Includes ------------------------------------------------------------------*/
#include "legacy_psdk2.x/psdk_typedef.h"
#include "legacy_psdk2.x/psdk_payload_camera.h"
#include "legacy_psdk2.x/psdk_aircraft_info.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/
/**
 * @brief Camera type.
 */
typedef E_DjiCameraType E_PsdkPayloadCollaborationCameraType;
#define PSDK_PAYLOAD_COLLABORATION_CAMERA_TYPE_UNKNOWN  DJI_CAMERA_TYPE_UNKNOWN /*!< Camera type is unknown. */
#define PSDK_PAYLOAD_COLLABORATION_CAMERA_TYPE_XT       DJI_CAMERA_TYPE_XT /*!< Camera type is XT. */
#define PSDK_PAYLOAD_COLLABORATION_CAMERA_TYPE_X4S      DJI_CAMERA_TYPE_X4S /*!< Camera type is X4S. */
#define PSDK_PAYLOAD_COLLABORATION_CAMERA_TYPE_X5S      DJI_CAMERA_TYPE_X5S /*!< Camera type is X5S. */
#define PSDK_PAYLOAD_COLLABORATION_CAMERA_TYPE_X7       DJI_CAMERA_TYPE_X7 /*!< Camera type is X7. */
#define PSDK_PAYLOAD_COLLABORATION_CAMERA_TYPE_Z30      DJI_CAMERA_TYPE_Z30 /*!< Camera type is Z30. */
#define PSDK_PAYLOAD_COLLABORATION_CAMERA_TYPE_XT2      DJI_CAMERA_TYPE_XT2/*!< Camera type is XT2. */
#define PSDK_PAYLOAD_COLLABORATION_CAMERA_TYPE_PSDK     DJI_CAMERA_TYPE_PSDK /*!< Camera type is third party camera based on Payload SDK. */
#define PSDK_PAYLOAD_COLLABORATION_CAMERA_TYPE_XTS      DJI_CAMERA_TYPE_XTS /*!< Camera type is XT S. */
#define PSDK_PAYLOAD_COLLABORATION_CAMERA_TYPE_H20      DJI_CAMERA_TYPE_H20 /*!< Camera type is H20. */
#define PSDK_PAYLOAD_COLLABORATION_CAMERA_TYPE_H20T     DJI_CAMERA_TYPE_H20T /*!< Camera type is H20T. */

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Initialise payload collaboration module. User should call this function before using this module.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkPayloadCollaboration_Init(void);

/**
 * @brief Deinitialise payload collaboration module.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkPayloadCollaboration_DeInit(void);

/**
 * @brief Get camera type of other payloads mounted on aircraft.
 * @note Please refer to PSDK documentation in DJI SDK developer website for details of conditions of use.
 * @note If there is empty in requested position or payload do not have camera type, the interface will return error.
 * @param payloadPosition: position where payload mounted on.
 * @param cameraType: Pointer to camera type of payload.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkPayloadCollaboration_GetCameraTypeOfPayload(
    E_PsdkAircraftInfoPayloadMountPosition payloadPosition,
    E_PsdkPayloadCollaborationCameraType *cameraType);

/**
 * @brief Get optical zoom specification of other camera payloads mounted on aircraft.
 * @note Please refer to PSDK documentation in DJI SDK developer website for details of conditions of use.
 * @note If there is empty in requested position or payload do not have related information, the interface will return
 * error.
 * @param payloadPosition: position where payload mounted on.
 * @param opticalZoomSpec: pointer to optical zoom specification.
 * @return Execution result.
 */
T_PsdkReturnCode
PsdkPayloadCollaboration_GetCameraOpticalZoomSpecOfPayload(E_PsdkAircraftInfoPayloadMountPosition payloadPosition,
                                                           T_PsdkCameraOpticalZoomSpec *opticalZoomSpec);

/**
 * @brief Get hybrid zoom focal length of other camera payloads mounted on aircraft.
 * @note Please refer to PSDK documentation in DJI SDK developer website for details of conditions of use.
 * @note If there is empty in requested position or payload do not have related information, the interface will return
 * error.
 * @param payloadPosition: position where payload mounted on.
 * @param focalLength: Pointer to optical focal length, unit: 0.1mm.
 * @return Execution result.
 */
T_PsdkReturnCode
PsdkPayloadCollaboration_GetCameraHybridZoomFocalLengthOfPayload(E_PsdkAircraftInfoPayloadMountPosition payloadPosition,
                                                                 uint16_t *focalLength);

#ifdef __cplusplus
}
#endif

#endif // `PSDK_PAYLOAD_COLLABORATION`_H

/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
