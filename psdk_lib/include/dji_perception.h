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
#pragma pack()

/**
 * @bref Callback type to receive stereo camera image
 */
typedef void(*DjiPerceptionImageCallback)(T_DjiPerceptionImageInfo imageInfo, uint8_t *imageRawBuffer,
                                          uint32_t bufferLen);

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
 * @param direction: direction to specify the direction of the subscription. Ref to E_DjiPerceptionDirection
 * @return Execution result.
 */
T_DjiReturnCode DjiPerception_UnsubscribePerceptionImage(E_DjiPerceptionDirection direction);


/**
 * @brief Get the internal and external parameters of all stereo cameras.
 * @return Execution result.
 */
T_DjiReturnCode DjiPerception_GetStereoCameraParameters(T_DjiPerceptionCameraParametersPacket *packet);

#ifdef __cplusplus
}
#endif

#endif // DJI_PERCEPTION_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
