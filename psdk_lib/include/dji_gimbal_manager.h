/**
 ********************************************************************
 * @file    dji_gimbal_manager.h
 * @brief   This is the header file for "dji_gimbal_manager.c", defining the structure and
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
#ifndef DJI_GIMBAL_MANAGER_H
#define DJI_GIMBAL_MANAGER_H

/* Includes ------------------------------------------------------------------*/
#include <dji_gimbal.h>
#include "dji_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/**
 * @brief Gimbal manager rotation command property.
 */
typedef struct {
    E_DjiGimbalRotationMode rotationMode; /*!< Rotation gimbal mode. */
    dji_f32_t pitch; /*!< Pitch angle in degree, unit: deg */
    dji_f32_t roll; /*!< Roll angle in degree, unit: deg */
    dji_f32_t yaw; /*!< Yaw angle in degree, unit: deg */
    dji_f64_t time; /*!< Expected execution time for gimbal rotation, in seconds. */
} T_DjiGimbalManagerRotation;

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Initialize the gimbal manager module.
 * @note This interface must be initialized after DjiCore_Init.
 * @return Execution result.
 */
T_DjiReturnCode DjiGimbalManager_Init(void);

/**
 * @brief DeInitialize the gimbal manager module.
 * @return Execution result.
 */
T_DjiReturnCode DjiGimbalManager_Deinit(void);

/**
 * @brief Set the work mode of the gimbal.
 * @param mountPosition: gimbal mount position, input limit see enum E_DjiMountPosition
 * @param mode: gimbal work mode, input limit see enum E_DjiGimbalMode
 * @return Execution result.
 */
T_DjiReturnCode DjiGimbalManager_SetMode(E_DjiMountPosition mountPosition, E_DjiGimbalMode mode);

/**
 * @brief Reset the pitch and yaw of the gimbal.
 * @param mountPosition: gimbal mount position, input limit see enum E_DjiMountPosition
 * @param mode: reset mode, input limit see enum E_DjiGimbalResetMode
 * @return Execution result.
 */
T_DjiReturnCode DjiGimbalManager_Reset(E_DjiMountPosition mountPosition, E_DjiGimbalResetMode resetMode);

/**
 * @brief Rotate the angle of the gimbal.
 * @param mountPosition: gimbal mount position, input limit see enum E_DjiMountPosition
 * @param rotation: the rotation parameters to be executed on the target gimbal, including the rotation mode, target
 * angle value and executed time, refer to T_DjiGimbalManagerRotation
 * @return Execution result.
 */
T_DjiReturnCode DjiGimbalManager_Rotate(E_DjiMountPosition mountPosition, T_DjiGimbalManagerRotation rotation);

/*!
 * @brief Prototype of callback function used to enable or disable extended pitch axis angle range.
 * @note Switching the gimbal limit euler angle of pitch axis to the extended limit angle or the default limit
 * angle.
 * @param mountPosition: gimbal mount position, input limit see enum E_DjiMountPosition
 * @param enabledFlag: flag specifying whether to enable or disable the extended pitch axis angle range..
 * @return Execution result.
 */
T_DjiReturnCode DjiGimbalManager_SetPitchRangeExtensionEnabled(E_DjiMountPosition mountPosition, bool enabledFlag);

/**
 * @brief Set max speed percentage for gimbal controller.
 * @param mountPosition: gimbal mount position, input limit see enum E_DjiMountPosition
 * @param axis: axis to be set.
 * @param maxSpeedPercentage: max speed value. Recommended calculation formula is "spd = default_max_spd * x / 100",
 * where 'x' is the default maximum speed value. Range from 1 to 100.
 * @return Execution result.
 */
T_DjiReturnCode DjiGimbalManager_SetControllerMaxSpeedPercentage(E_DjiMountPosition mountPosition, E_DjiGimbalAxis axis,
                                                                 uint8_t maxSpeedPercentage);

/**
 * @brief Set smooth factor for gimbal controller, using for smooth control.
 * @param mountPosition: gimbal mount position, input limit see enum E_DjiMountPosition
 * @param axis: axis to be set.
 * @param smoothingFactor: smooth factor. A larger value results in slower gimbal acceleration. Recommended
 * calculation formula is "acc = 10000 * (0.8 ^ (1 + x)) deg/s^2", x is smooth factor. Range from 0 to 30.
 * @return Execution result.
 */
T_DjiReturnCode DjiGimbalManager_SetControllerSmoothFactor(E_DjiMountPosition mountPosition, E_DjiGimbalAxis axis,
                                                           uint8_t smoothingFactor);

/**
 * @brief Restore factory settings of gimbal, including fine tune angle, pitch angle extension enable flag and max
 * speed etc.
 * @param mountPosition: gimbal mount position, input limit see enum E_DjiMountPosition
 * @return Execution result.
 */
T_DjiReturnCode DjiGimbalManager_RestoreFactorySettings(E_DjiMountPosition mountPosition);

#ifdef __cplusplus
}
#endif

#endif // DJI_GIMBAL_MANAGER_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
