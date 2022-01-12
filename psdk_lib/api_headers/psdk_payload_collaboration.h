/**
 ********************************************************************
 * @file    psdk_payload_collaboration.h
 * @brief   This is the header file for "psdk_payload_collaboration.c", defining the structure and
 * (exported) function prototypes.
 *
 * @copyright (c) 2018 DJI. All rights reserved.
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
#include "psdk_typedef.h"
#include "psdk_payload_camera.h"
#include "psdk_aircraft_info.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/
/**
 * @brief Camera type.
 */
typedef enum {
    PSDK_PAYLOAD_COLLABORATION_CAMERA_TYPE_UNKNOWN = 0, /*!< Camera type is unknown. */
    PSDK_PAYLOAD_COLLABORATION_CAMERA_TYPE_XT = 7, /*!< Camera type is XT. */
    PSDK_PAYLOAD_COLLABORATION_CAMERA_TYPE_X4S = 14, /*!< Camera type is X4S. */
    PSDK_PAYLOAD_COLLABORATION_CAMERA_TYPE_X5S = 15, /*!< Camera type is X5S. */
    PSDK_PAYLOAD_COLLABORATION_CAMERA_TYPE_X7 = 17, /*!< Camera type is X7. */
    PSDK_PAYLOAD_COLLABORATION_CAMERA_TYPE_Z30 = 20, /*!< Camera type is Z30. */
    PSDK_PAYLOAD_COLLABORATION_CAMERA_TYPE_XT2 = 26, /*!< Camera type is XT2. */
    PSDK_PAYLOAD_COLLABORATION_CAMERA_TYPE_PSDK = 31, /*!< Camera type is third party camera based on Payload SDK. */
    PSDK_PAYLOAD_COLLABORATION_CAMERA_TYPE_XTS = 41, /*!< Camera type is XT S. */
    PSDK_PAYLOAD_COLLABORATION_CAMERA_TYPE_H20 = 42, /*!< Camera type is H20. */
    PSDK_PAYLOAD_COLLABORATION_CAMERA_TYPE_H20T = 43, /*!< Camera type is H20T. */
} E_PsdkPayloadCollaborationCameraType;

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

#endif // PSDK_PAYLOAD_COLLABORATION_H

/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
