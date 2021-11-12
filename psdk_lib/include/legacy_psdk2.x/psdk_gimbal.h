/**
 ********************************************************************
 * @file    psdk_gimbal.h
 * @brief   PSDK gimbal module is used to achieve the third gimbal for developer. The module can be used only when the PSDK
 * adapter is Skyport, rather than XPort.
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
#ifndef PSDK_GIMBAL_H
#define PSDK_GIMBAL_H

/* Includes ------------------------------------------------------------------*/
#include "legacy_psdk2.x/psdk_typedef.h"
#include "dji_gimbal.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/**
 * @brief Gimbal work mode, specifies how gimbal follow aircraft movement.
 */
typedef E_DjiGimbalMode E_PsdkGimbalMode;
/*!< Free mode, fix gimbal attitude in the ground coordinate, ignoring movement of aircraft. */
#define PSDK_GIMBAL_MODE_FREE        DJI_GIMBAL_MODE_FREE
/*!< FPV (First Person View) mode, only control roll and yaw angle of gimbal in the ground coordinate to follow aircraft. */
#define PSDK_GIMBAL_MODE_FPV         DJI_GIMBAL_MODE_FPV
/*!< Yaw follow mode, only control yaw angle of gimbal in the ground coordinate to follow aircraft. */
#define PSDK_GIMBAL_MODE_YAW_FOLLOW  DJI_GIMBAL_MODE_YAW_FOLLOW

/**
 * @brief Gimbal calibration stage and result.
 */
typedef E_DjiGimbalCalibrationStage E_PsdkGimbalCalibrationStage;
/*!< Calibration have been completed and the result is successful. */
#define PSDK_GIMBAL_CALIBRATION_STAGE_COMPLETE      DJI_GIMBAL_CALIBRATION_STAGE_COMPLETE
/*!< Gimbal is calibrating. */
#define PSDK_GIMBAL_CALIBRATION_STAGE_PROCRESSING   DJI_GIMBAL_CALIBRATION_STAGE_PROCRESSING
/*!< Calibration have been completed and the result is failure. */
#define PSDK_GIMBAL_CALIBRATION_STAGE_FAILED        DJI_GIMBAL_CALIBRATION_STAGE_FAILED

/**
 * @brief Gimbal rotation mode, specifies control style.
 */
typedef E_DjiGimbalRotationMode E_PsdkGimbalRotationMode;
#define PSDK_GIMBAL_ROTATION_MODE_RELATIVE_ANGLE   DJI_GIMBAL_ROTATION_MODE_RELATIVE_ANGLE /*!< Relative angle rotation mode, represents rotating gimbal specified angles based on current angles. */
#define PSDK_GIMBAL_ROTATION_MODE_ABSOLUTE_ANGLE   DJI_GIMBAL_ROTATION_MODE_ABSOLUTE_ANGLE /*!< Absolute angle rotation mode, represents rotating gimbal to specified angles in the ground coordinate. */
#define PSDK_GIMBAL_ROTATION_MODE_SPEED            DJI_GIMBAL_ROTATION_MODE_SPEED /*!< Speed rotation mode, specifies rotation speed of gimbal in the ground coordinate. */

/**
 * @brief Axis of gimbal.
 */
typedef E_DjiGimbalAxis E_PsdkGimbalAxis;
#define PSDK_GIMBAL_AXIS_PITCH  DJI_GIMBAL_AXIS_PITCH /*!< Pitch axis. */
#define PSDK_GIMBAL_AXIS_ROLL   DJI_GIMBAL_AXIS_ROLL /*!< Roll axis. */
#define PSDK_GIMBAL_AXIS_YAW    DJI_GIMBAL_AXIS_YAW /*!< Yaw axis. */

/**
 * @brief Reset mode of gimbal.
 */
typedef E_DjiGimbalResetMode E_PsdkGimbalResetMode;
/*! Only reset yaw axis of gimbal. Reset angle of yaw axis to the sum of yaw axis angle of aircraft and fine tune angle
 * of yaw axis of gimbal. */
#define PSDK_GIMBAL_RESET_MODE_YAW                            DJI_GIMBAL_RESET_MODE_YAW
/*! Reset yaw axis and pitch axis of gimbal. Reset angle of yaw axis to sum of yaw axis angle of aircraft and fine tune
 * angle of yaw axis of gimbal, and reset pitch axis angle to the fine tune angle. */
#define PSDK_GIMBAL_RESET_MODE_PITCH_AND_YAW                  DJI_GIMBAL_RESET_MODE_PITCH_AND_YAW
/*! Reset yaw axis and pitch axis of gimbal. Reset angle of yaw axis to sum of yaw axis angle of aircraft and fine tune
 * angle of yaw axis of gimbal, and reset pitch axis angle to sum of -90 degree and fine tune angle if gimbal
 * downward, sum of 90 degree and fine tune angle if upward. */
#define PSDK_GIMBAL_RESET_MODE_PITCH_DOWNWARD_UPWARD_AND_YAW  DJI_GIMBAL_RESET_MODE_PITCH_DOWNWARD_UPWARD_AND_YAW
/*! Reset pitch axis of gimbal. Reset pitch axis angle to sum of -90 degree and fine tune angle if gimbal downward,
 * sum of 90 degree and fine tune angle if upward. */
#define PSDK_GIMBAL_RESET_MODE_PITCH_DOWNWARD_UPWARD          DJI_GIMBAL_RESET_MODE_PITCH_DOWNWARD_UPWARD

/**
 * @brief Smooth factor of gimbal controller.
 */
typedef T_DjiGimbalControllerSmoothFactor T_PsdkGimbalControllerSmoothFactor;

/**
 * @brief Max speed percentage of every axis of gimbal.
 */
typedef T_DjiGimbalControllerMaxSpeedPercentage T_PsdkGimbalControllerMaxSpeedPercentage;

/**
 * @brief Data structure describes gimbal system state.
 */
typedef T_DjiGimbalSystemState T_PsdkGimbalSystemState;

/**
 * @brief Reach limit flag.
 */
typedef T_DjiGimbalReachLimitFlag T_PsdkGimbalReachLimitFlag;

/**
 * @brief Gimbal attitude related information.
 */
typedef T_DjiGimbalAttitudeInformation T_PsdkGimbalAttitudeInformation;

/**
 * @brief Gimbal calibration state.
 */
typedef T_DjiGimbalCalibrationState T_PsdkGimbalCalibrationState;

/**
 * @brief Gimbal rotation command property.
 */
typedef T_DjiGimbalRotationProperty T_PsdkGimbalRotationProperty;

/**
 * @brief Common handler function prototypes of gimbal, that is, some common callback function prototypes of gimbal.
 * @warning User can not execute blocking style operations or functions in callback function, because that will block PSDK
 * root thread, causing problems such as slow system response, payload disconnection or infinite loop.
 */
typedef T_DjiGimbalCommonHandler T_PsdkGimbalCommonHandler;

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Initialise gimbal module, and user should call this function before using gimbal features.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkGimbal_Init(void);

/**
 * @brief Deinitialise gimbal module.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkGimbal_DeInit(void);

/**
 * @brief Register common handler functions/callback functions of gimbal. After registering, callback functions will be
 * called automatically.
 * @param commonHandler: pointer to common handler functions of gimbal.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkGimbal_RegCommonHandler(const T_PsdkGimbalCommonHandler *commonHandler);

#ifdef __cplusplus
}
#endif

#endif // PSDK_GIMBAL_H

/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
