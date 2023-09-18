/**
 ********************************************************************
 * @file    dji_gimbal.h
 * @brief   DJI gimbal module is used to achieve the third gimbal for developer. The module can be used only when the DJI
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
#ifndef DJI_GIMBAL_H
#define DJI_GIMBAL_H

/* Includes ------------------------------------------------------------------*/
#include "dji_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/
/**
 * @brief Gimbal calibration stage and result.
 */
typedef enum {
    DJI_GIMBAL_CALIBRATION_STAGE_COMPLETE = 0, /*!< Calibration have been completed and the result is successful. */
    DJI_GIMBAL_CALIBRATION_STAGE_PROCRESSING = 1, /*!< Gimbal is calibrating. */
    DJI_GIMBAL_CALIBRATION_STAGE_FAILED = 3, /*!< Calibration have been completed and the result is failure. */
} E_DjiGimbalCalibrationStage;

/**
 * @brief Axis of gimbal.
 */
typedef enum {
    DJI_GIMBAL_AXIS_PITCH = 0, /*!< Pitch axis. */
    DJI_GIMBAL_AXIS_ROLL = 1, /*!< Roll axis. */
    DJI_GIMBAL_AXIS_YAW = 2, /*!< Yaw axis. */
} E_DjiGimbalAxis;

/**
 * @brief Reset mode of gimbal.
 */
typedef enum {
    /*! Only reset yaw axis of gimbal. Reset angle of yaw axis to the sum of yaw axis angle of aircraft and fine tune angle
     * of yaw axis of gimbal. */
    DJI_GIMBAL_RESET_MODE_YAW = 1,
    /*! Reset yaw axis and pitch axis of gimbal. Reset angle of yaw axis to sum of yaw axis angle of aircraft and fine tune 
     * angle of yaw axis of gimbal, and reset pitch axis angle to the fine tune angle. */
    DJI_GIMBAL_RESET_MODE_PITCH_AND_YAW = 3,
    /*! Reset only pitch axis of gimbal. */
    DJI_GIMBAL_RESET_MODE_PITCH_ONLY = 4,
    /*! Reset only roll axis of gimbal. */
    DJI_GIMBAL_RESET_MODE_ROLL_ONLY = 5,
    /*! Reset only yaw axis of gimbal. */
    DJI_GIMBAL_RESET_MODE_YAW_ONLY = 6,
    /*! Reset yaw axis and pitch axis of gimbal. Reset angle of yaw axis to sum of yaw axis angle of aircraft and fine tune
     * angle of yaw axis of gimbal, and reset pitch axis angle to sum of -90 degree and fine tune angle if gimbal
     * downward, sum of 90 degree and fine tune angle if upward. */
    DJI_GIMBAL_RESET_MODE_PITCH_DOWNWARD_UPWARD_AND_YAW = 11,
    /*! Reset pitch axis of gimbal. Reset pitch axis angle to sum of -90 degree and fine tune angle if gimbal downward,
     * sum of 90 degree and fine tune angle if upward. */
    DJI_GIMBAL_RESET_MODE_PITCH_DOWNWARD_UPWARD = 12,
} E_DjiGimbalResetMode;

/**
 * @brief Smooth factor of gimbal controller.
 */
typedef struct {
    uint8_t pitch; /*!< Smooth factor of pitch axis, range from 0 to 30. */
    uint8_t yaw; /*!< Smooth factor of yaw axis, range from 0 to 30. */
} T_DjiGimbalControllerSmoothFactor;

/**
 * @brief Max speed percentage of every axis of gimbal.
 */
typedef struct {
    uint8_t pitch; /*!< Max speed percentage of pitch axis, range from 1 to 100. */
    uint8_t yaw; /*!< Max speed percentage of yaw axis, range from 1 to 100. */
} T_DjiGimbalControllerMaxSpeedPercentage;

/**
 * @brief Data structure describes gimbal system state.
 */
typedef struct {
    bool resettingFlag; /*!< Specifies whether gimbal is resetting now or not. */
    bool mountedUpward; /*!< Specifies whether gimbal is upward or not. */
    bool blockingFlag; /*!< Specifies whether gimbal is stuck or not. */
    bool pitchRangeExtensionEnabledFlag; /*!< Specifies whether extended angle range of gimbal pitch axis is enabled or not. */
    E_DjiGimbalMode gimbalMode; /*!< Gimbal mode. */
    T_DjiAttitude3d fineTuneAngle; /*!< Fine tune angles, unit: 0.1 degree. */
    T_DjiGimbalControllerSmoothFactor smoothFactor; /*!< Smooth factor of gimbal controller. */
    T_DjiGimbalControllerMaxSpeedPercentage maxSpeedPercentage; /*!< Max speed percentage of gimbal controller. */
} T_DjiGimbalSystemState;

/**
 * @brief Reach limit flag.
 */
typedef struct {
    bool pitch; /*!< Specifies whether gimbal has reached angle limit of pitch axis, if true, reached the limit. */
    bool roll; /*!< Specifies whether gimbal has reached angle limit of roll axis, if true, reached the limit. */
    bool yaw; /*!< Specifies whether gimbal has reached angle limit of yaw axis, if true, reached the limit. */
} T_DjiGimbalReachLimitFlag;

/**
 * @brief Gimbal attitude related information.
 */
typedef struct {
    T_DjiAttitude3d attitude; /*!< Gimbal attitude in the ground coordinate, unit: 0.1 degree. */
    T_DjiGimbalReachLimitFlag reachLimitFlag; /*!< Reach limit flag, being set when the joint angles of gimbal reach limits. */
} T_DjiGimbalAttitudeInformation;

/**
 * @brief Gimbal calibration state.
 */
typedef struct {
    bool calibratingFlag; /*!< Specifies whether gimbal is calibrating now or not. */
    bool lastCalibrationResult; /*!< Result of the last calibration, and true specifies success. */
    uint8_t currentCalibrationProgress; /*!< Progress percentage of current calibration. If gimbal is calibrating now and fill in real progress percentage, if not, fill in zero. The item ranges from 0 to 100. */
    E_DjiGimbalCalibrationStage currentCalibrationStage; /*!< Current calibration stage and result. If gimbal is calibrating now and fill in ::DJI_GIMBAL_CALIBRATION_STAGE_PROCRESSING, if not, fill in result of the last calibration. */
} T_DjiGimbalCalibrationState;

/**
 * @brief Gimbal rotation command property.
 */
typedef struct {
    struct {
        bool pitch; /*!< Specifies whether ignore rotation command of pitch axis, and true specifies ignoring. */
        bool roll; /*!< Specifies whether ignore rotation command of roll axis, and true specifies ignoring. */
        bool yaw; /*!< Specifies whether ignore rotation command of yaw axis, and true specifies ignoring. */
    } rotationValueInvalidFlag; /*!< Specifies whether ignore rotation command of some axis, including pitch, roll and yaw. */

    union {
        struct {
            uint16_t actionTime; /*!< Action time during which gimbal move to target angles, unit: 0.01s. */
        } relativeAngleRotation; /*!< Property of relative angle rotation command. */
        struct {
            uint16_t actionTime; /*!< Action time during which gimbal move to target angles, unit: 0.01s. */
            /*! Joint angle valid flag, specifying whether
             * T_DjiGimbalRotationProperty::absoluteAngleRotation::jointAngle is valid when absolute angle control. */
            bool jointAngleValid;
            /*! Joint angles of gimbal, unit: 0.1 degree. If
             * T_DjiGimbalRotationProperty::absoluteAngleRotation::jointAngleValid is false, specifying here joint
             * angles are invalid, and please ignore the joint angles. If joint angles are valid, users should ensure
             * that target joint angles of gimbal is approximately the same as the specified value. */
            T_DjiAttitude3d jointAngle;
        } absoluteAngleRotation; /*!< Property of absolute angle rotation command. */
    };
} T_DjiGimbalRotationProperty;

/**
 * @brief Common handler function prototypes of gimbal, that is, some common callback function prototypes of gimbal.
 * @warning User can not execute blocking style operations or functions in callback function, because that will block DJI
 * root thread, causing problems such as slow system response, payload disconnection or infinite loop.
 */
typedef struct {
    /*!
     * @brief Prototype of callback function used to report system state of gimbal.
     * @param systemState: pointer to memory space used to store gimbal system state.
     * @return Execution result.
     */
    T_DjiReturnCode (*GetSystemState)(T_DjiGimbalSystemState *systemState);

    /*!
     * @brief Prototype of callback function used to report attitude information of gimbal.
     * @param attitudeInformation: pointer to memory space used to store gimbal attitude information.
     * @return Execution result.
     */
    T_DjiReturnCode (*GetAttitudeInformation)(T_DjiGimbalAttitudeInformation *attitudeInformation);

    /*!
     * @brief Prototype of callback function used to report calibration state of gimbal.
     * @param calibrationState: pointer to memory space used to store gimbal calibration state.
     * @return Execution result.
     */
    T_DjiReturnCode (*GetCalibrationState)(T_DjiGimbalCalibrationState *calibrationState);

    /*!
     * @brief Prototype of callback function used to rotate gimbal. Rotation callback function will be called when
     * other modules control gimbal.
     * @warning User should control execution time of the callback function to be short, because this callback function
     * may be called at very high frequency in some scenarios, just like 200Hz.
     * @param rotationMode: mode of rotation command.
     * @param rotationProperty: property of rotation command.
     * @param rotationValue: value of rotation command, unit: 0.1 degree (if rotation mode is
     * ::DJI_GIMBAL_ROTATION_MODE_RELATIVE_ANGLE or ::DJI_GIMBAL_ROTATION_MODE_ABSOLUTE_ANGLE), 0.1 degree/s
     * (if rotation mode is ::DJI_GIMBAL_ROTATION_MODE_SPEED).
     * @return Execution result.
     */
    T_DjiReturnCode (*Rotate)(E_DjiGimbalRotationMode rotationMode, T_DjiGimbalRotationProperty rotationProperty,
                              T_DjiAttitude3d rotationValue);

    /*!
     * @brief Prototype of callback function used to start calibrating gimbal.
     * @note Maximum allowable time of gimbal calibration is 120s. If gimbal has not completed calibration during
     * 120s, DJI Pilot believe calibration failed.
     * @return Execution result.
     */
    T_DjiReturnCode (*StartCalibrate)(void);

    /**
     * @brief Set smooth factor for gimbal controller, using to smooth control.
     * @param smoothingFactor: smooth factor. The larger the value, the smaller the acceleration of gimbal. Recommended
     * calculation formula is "acc = 10000 * (0.8 ^ (1 + x)) deg/s^2", x is smooth factor. Range from 0 to 30.
     * @param axis: axis to be set.
     * @return Execution result.
     */
    T_DjiReturnCode (*SetControllerSmoothFactor)(uint8_t smoothingFactor, E_DjiGimbalAxis axis);

    /*!
     * @brief Prototype of callback function used to enable or disable extended pitch axis angle range.
     * @details Switching the gimbal limit euler angle of pitch axis to the extended limit angle or the default limit
     * angle.
     * @param enabledFlag: flag specified whether enable or disable extended pitch axis angle range.
     * @return Execution result.
     */
    T_DjiReturnCode (*SetPitchRangeExtensionEnabled)(bool enabledFlag);

    /**
     * @brief Set max speed percentage for gimbal controller.
     * @param maxSpeedPercentage: max speed value. Recommended calculation formula is "spd = default_max_spd * x / 100",
     * x is default max speed value. Range from 1 to 100.
     * @param axis: axis to be set.
     * @return Execution result.
     */
    T_DjiReturnCode (*SetControllerMaxSpeedPercentage)(uint8_t maxSpeedPercentage, E_DjiGimbalAxis axis);

    /**
     * @brief Restore factory settings of gimbal, including fine tune angle, pitch angle extension enable flag and max
     * speed etc.
     * @return Execution result.
     */
    T_DjiReturnCode (*RestoreFactorySettings)(void);

    /*!
     * @brief Prototype of callback function used to set gimbal work mode.
     * @param mode: gimbal work mode.
     * @return Execution result.
     */
    T_DjiReturnCode (*SetMode)(E_DjiGimbalMode mode);

    /*!
     * @brief Prototype of callback function used to reset gimbal.
     * @note Reset command will interrupt rotation process.
     * @param mode: Reset mode.
     * @return Execution result.
     */
    T_DjiReturnCode (*Reset)(E_DjiGimbalResetMode mode);

    /*!
     * @brief Prototype of callback function used to fine tune angle of gimbal.
     * @param fineTuneAngle: fine tune angle, unit: 0.1 degree. Angle value of greater than 0 specifies rotation
     * towards the positive direction of body coordinate.
     * @return Execution result.
     */
    T_DjiReturnCode (*FineTuneAngle)(T_DjiAttitude3d fineTuneAngle);

    /**
     * @brief Prototype of callback function used to report rotation speed of gimbal.
     * @note The callback function will be called at 50 Hz. Developers should ensure that, the average rotation speed
     * of gimbal within 20 ms after the callback function is called is approximately equal to the reported rotation
     * speed.
     * @param rotationSpeed: pointer to memory space used to store rotation speed, in body coordinate, unit:
     * 0.1 degree/s.
     * @return Execution result.
     */
    T_DjiReturnCode (*GetRotationSpeed)(T_DjiAttitude3d *rotationSpeed);

    /**
     * @brief Prototype of callback function used to report joint angle of gimbal.
     * @param jointAngle: pointer to memory space used to store joint angles, unit: 0.1 degree.
     * @return Execution result.
     */
    T_DjiReturnCode (*GetJointAngle)(T_DjiAttitude3d *jointAngle);
} T_DjiGimbalCommonHandler;

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Initialise gimbal module, and user should call this function before using gimbal features.
 * @return Execution result.
 */
T_DjiReturnCode DjiGimbal_Init(void);

/**
 * @brief Deinitialise gimbal module.
 * @return Execution result.
 */
T_DjiReturnCode DjiGimbal_DeInit(void);

/**
 * @brief Register common handler functions/callback functions of gimbal. After registering, callback functions will be
 * called automatically.
 * @param commonHandler: pointer to common handler functions of gimbal.
 * @return Execution result.
 */
T_DjiReturnCode DjiGimbal_RegCommonHandler(const T_DjiGimbalCommonHandler *commonHandler);

#ifdef __cplusplus
}
#endif

#endif // DJI_GIMBAL_H

/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
