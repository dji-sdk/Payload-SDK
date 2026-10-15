/**
 ********************************************************************
 * @file    dji_interest_point.h
 * @version V1.0.0
 * @date    2020/11/19
 * @brief   This is the header file for "dji_interest_point.c", defining the structure and
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
#ifndef DJI_INTEREST_POINT_H
#define DJI_INTEREST_POINT_H

/* Includes ------------------------------------------------------------------*/
#include "dji_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef enum {
    DJI_INTEREST_POINT_MISSION_ACTION_STATE_NOT_STARTED = 0,
    DJI_INTEREST_POINT_MISSION_ACTION_STATE_PAUSE = 1,
    DJI_INTEREST_POINT_MISSION_ACTION_STATE_RUNNING = 2,
} E_DjiInterestPointActionState;

/**
 * @brief Spotlight-Zoom overall functional state (field: spzState in T_DjiInterestPointMissionState).
 *        Pushed periodically state push.
 */
typedef enum {
    DJI_SPOTLIGHT_ZOOM_STANDBY = 0, /*!< SPZ function not active. */
    DJI_SPOTLIGHT_ZOOM_MOT     = 1, /*!< Multi-Object Tracking: waiting for user to select target or draw box. */
    DJI_SPOTLIGHT_ZOOM_WAITING = 2, /*!< Single-Object Tracking: waiting for user to confirm target; re-drawing allowed. */
    DJI_SPOTLIGHT_ZOOM_SOT     = 3, /*!< Single-Object Tracking: actively tracking target; re-drawing allowed. */
} E_DjiSpotlightZoomState;

/**
 * @brief Spotlight-zoom mission state pushed periodically from the flight controller.
 * @note  Circle-related fields (radius, velocity, gimbal_pitch_ctrl_state, stick_state,
 *        surround_state) are only valid when submode == DJI_SPOTLIGHT_ZOOM_MODE_CIRCLE.
 */
typedef struct {
    /* ---- functional state ---- */
    uint8_t   spzState;             /*!< Spotlight-Zoom overall state. Refer to DJI_SPOTLIGHT_ZOOM_STATE. */
    uint8_t   submode;              /*!< Current sub-mode. Refer to DJI_TRACKING_MODE. */
    uint8_t   isHandlingCommand;    /*!< Non-zero: FC is handling a command; avoid sending new SPZ commands. */
    uint8_t   statusMessage;        /*!< Status/exit-reason code. Refer to DJI_SPOTLIGHT_ZOOM_STATUS_MESSAGE (E_DjiSpotlightZoomStatusMessage). */

    /* ---- drone & payload control flags ---- */
    uint8_t   isControllingDrone;     /*!< bit[0-2]: drone control mode (0=none,1=assist,2=active). bit[3-4]: speed/avoid mode. */
    uint8_t   isControllingPayload0;  /*!< bit0: gimbal ctrl, bit1: zoom ctrl, bit2: vision tracking (payload 0). */
    uint8_t   isControllingPayload1;  /*!< Same bitmask for payload 1. */
    uint8_t   isControllingPayload2;  /*!< Same bitmask for payload 2. */

    /* ---- focal length range (used by DjiInterestPoint_SetZoomByPercentage) ---- */
    uint16_t  maxEquivalentFocalLength; /*!< Maximum allowed equivalent focal length, unit: mm. */
    uint16_t  minEquivalentFocalLength; /*!< Minimum allowed equivalent focal length, unit: mm. */

    /* ---- circle sub-mode fields (valid only when submode == DJI_SPOTLIGHT_ZOOM_MODE_CIRCLE) ---- */
    dji_f32_t desiredCircleRadius;    /*!< Currently set circle radius, unit: m. */
    dji_f32_t maxCircleRadius;        /*!< Maximum allowed circle radius, unit: m (≤ 1 km). */
    dji_f32_t minCircleRadius;        /*!< Minimum allowed circle radius, unit: m (≥ 5 m). */
    dji_f32_t desiredCircleVelocity;  /*!< Currently set circle velocity, unit: m/s. */
    dji_f32_t maxCircleVelocity;      /*!< Maximum allowed circle velocity, unit: m/s. */
    dji_f32_t curCircleVelocity;      /*!< Current actual circle velocity, unit: m/s. */
    uint8_t   gimbalPitchCtrlState;   /*!< 0: gimbal pitch follows; 1: gimbal pitch locked. */
    uint8_t   stickState;             /*!< 0: no stick input; 1: stick engaged (roll adds to circle speed). */
    uint8_t   surroundState;          /*!< 0: not circling; 1: circling paused; 2: circling. */

    /* ---- backward-compatible aliases ---- */
    uint8_t   state;      /*!< Alias for surroundState. 0=not started, 1=pause, 2=running. */
    dji_f32_t radius;     /*!< Alias for desiredCircleRadius, unit: m. */
    dji_f32_t curSpeed;   /*!< Alias for curCircleVelocity, unit: m/s. */
} T_DjiInterestPointMissionState;

typedef struct {
    dji_f64_t latitude;
    dji_f64_t longitude;
    dji_f32_t speed;
    int8_t payloadCameraIndex; /*!< Used by which aircraft that can mount payload cameras. Range starts from 1.*/
} T_DjiInterestPointSettings;

/**
 * @brief Parameters for spotlight-zoom target size control.
 * @details It configures the target size thresholds used by the flight controller
 *          to decide when to zoom in or out during a spotlight-zoom surround mission.
 *          Total size: 4 * sizeof(float) = 16 bytes.
 */
typedef struct {
    dji_f32_t max_tar_size;       /*!< Maximum target size as a ratio of screen width or height.
                                       Default: 0.7f (70%). If the target exceeds this ratio,
                                       the camera will zoom out. */
    dji_f32_t max_tar_size_area;  /*!< Maximum target area as a ratio of total screen area.
                                       Default: 1.0f/9 (~11.1%). If the target area exceeds
                                       this ratio, the camera will zoom out. */
    dji_f32_t default_ta_size;    /*!< Default target size in pixels used as the zoom reference
                                       baseline. Default: 50.0f px. */
    dji_f32_t min_tar_size;       /*!< Minimum target size in pixels. If the target falls below
                                       this size, the camera will zoom in. Default: 26.0f px. */
} T_DjiInterestPointZoomTypeParam;

typedef T_DjiReturnCode (*InterestPointMissionStateCallback)(T_DjiInterestPointMissionState missionState);

/* Exported functions --------------------------------------------------------*/
T_DjiReturnCode DjiInterestPoint_Init(void);

T_DjiReturnCode DjiInterestPoint_DeInit(void);

T_DjiReturnCode DjiInterestPoint_Start(T_DjiInterestPointSettings settings);

T_DjiReturnCode DjiInterestPoint_Stop(void);

T_DjiReturnCode DjiInterestPoint_SetSpeed(dji_f32_t speed);

/**
 * @brief Set the approach speed for the spotlight-zoom surround mission.
 * @details This function configures the speed magnitude at which the aircraft
 *          approaches the target during the approach phase of a spotlight-zoom mission.
 *          The command is sent synchronously via the MO binocular command channel.
 *          The setting takes effect immediately if a mission is running.
 * @note
 *  - Setting speed to 0.0f will pause the approach phase (aircraft hovers in place).
 *  - This API is not available in the current version.
 * @param speed 3D approach speed magnitude in m/s. Valid range: [0.0f, 20.0f].
 *              - 0.0f:  pause the approach (aircraft holds current position).
 *              - 20.0f: maximum approach speed.
 *              Values outside [0.0f, 20.0f] are rejected before sending the command.
 * @return T_DjiReturnCode
 *  - DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS: Command sent and acknowledged successfully.
 *  - DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER: speed < 0.0f or speed > 20.0f.
 *  - DJI_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT: Current license level does not support this interface.
 *  - DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR: Failed to retrieve license level.
 *  - Other error codes: Refer to dji_error.h for details.
 */
T_DjiReturnCode DjiInterestPoint_SetApproachSpeed(dji_f32_t speed);

/**
 * @brief Set the approach angle for the spotlight-zoom surround mission.
 * @details This function configures the angle between the aircraft's approach velocity
 *          vector and the horizontal plane during the approach phase of a spotlight-zoom
 *          mission. The command is sent synchronously via the MO binocular command channel.
 *          The setting takes effect immediately if a mission is running.
 * @note
 *  - This API is not available in the current version.
 * @param angle Approach angle in degrees. Valid range: [0.0f, 90.0f].
 *              - 0.0f:  horizontal approach (aircraft flies level toward the target).
 *              - 90.0f: vertical approach (aircraft descends straight down toward the target).
 *              Values outside [0.0f, 90.0f] are rejected before sending the command.
 * @return T_DjiReturnCode
 *  - DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS: Command sent and acknowledged successfully.
 *  - DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER: angle < 0.0f or angle > 90.0f.
 *  - DJI_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT: Current license level does not support this interface.
 *  - DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR: Failed to retrieve license level.
 *  - Other error codes: Refer to dji_error.h for details.
 */
T_DjiReturnCode DjiInterestPoint_SetApproachAngle(dji_f32_t angle);


/**
 * @brief Set the camera zoom level by percentage for the spotlight-zoom mission.
 * @details This function calculates the actual equivalent focal length based on the
 *          specified percentage and the focal length range [min, max] received from
 *          the spotlight-zoom state push,
 *          (fields: min_equivalent_focal_length / max_equivalent_focal_length).
 *
 *          The focal length is computed as:
 *          @code
 *          focalLength = minFocal + (maxFocal - minFocal) * (percentage / 100.0f)
 *          @endcode
 *          and sent via set_param.
 *
 * @note
 *  - The focal length range is populated automatically from the periodic state push
 *    . This function will fail with
 *    DJI_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT_IN_CURRENT_STATE if the state push has
 *    not been received yet. Ensure the spotlight-zoom mission is started and at least
 *    one state push has been received before calling this function.
 *  - percentage=0.0f corresponds to the minimum focal length (wide angle).
 *  - percentage=100.0f corresponds to the maximum focal length (telephoto).
 *  - This API is not available in the current version.
 *
 * @param percentage Zoom percentage in the range [0.0f, 100.0f].
 * @return T_DjiReturnCode
 *  - DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS: Command sent and acknowledged successfully.
 *  - DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER: percentage out of [0, 100] range.
 *  - DJI_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT_IN_CURRENT_STATE: Focal length range not
 *    yet received from state push; call after mission start.
 *  - DJI_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT: Current license level does not support this interface.
 *  - Other error codes: Refer to dji_error.h for details.
 */
T_DjiReturnCode DjiInterestPoint_SetZoomByPercentage(dji_f32_t percentage);

T_DjiReturnCode DjiInterestPoint_RegMissionStateCallback(InterestPointMissionStateCallback callback);

/**
 * @brief Get the latest spotlight-zoom mission state cached from the periodic state push.
 * @details The state is updated automatically each time the flight controller sends a
 *          state push. An internal callback is registered
 *          during DjiInterestPoint_Init to keep the cache always current.
 *          This function is non-blocking and safe to call from any context.
 * @note
 *  - This API is not available in the current version.
 * @param state Pointer to a T_DjiInterestPointMissionState that receives the cached state.
 *              Must not be NULL.
 * @return T_DjiReturnCode
 *  - DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS: State copied successfully.
 *  - DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER: state is NULL.
 */
T_DjiReturnCode DjiInterestPoint_GetLatestMissionState(T_DjiInterestPointMissionState *state);

/**
 * @brief Query whether the aircraft is currently in SPZ (Spotlight-Zoom) intelligent
 *        tracking state.
 *
 * @note  **Platform restriction**: The state judgment logic implemented here is
 *        only applicable to the WA345 (Matrice 4 series) aircraft. On other
 *        platforms, the spzState and submode semantics may differ or may not be
 *        supported at all.
 *
 * This function inspects the most recently cached SPZ state push to
 * determine whether two conditions are simultaneously satisfied:
 *
 * **Condition 1 — spzState (field: spzState in T_DjiInterestPointMissionState)**
 *   Derived from `push->state` in the protocol.
 *   Required value: `DJI_SPOTLIGHT_ZOOM_SOT` (3).
 *   - STANDBY (0): SPZ function not active.
 *   - MOT     (1): Multi-Object Tracking — waiting for user to draw a target box.
 *   - WAITING (2): Single-Object Tracking — box received, waiting for user to
 *                  confirm the target.
 *   - SOT     (3): Single-Object Tracking — target confirmed, aircraft is
 *                  actively tracking the target. This is the only state where
 *                  intelligent tracking is truly active.
 *
 * **Condition 2 — submode (field: submode in T_DjiInterestPointMissionState)**
 *   Derived from `push->submode` in the protocol (enum: DJI_TRACKING_MODE).
 *   Required values: `DJI_SPOTLIGHT_ZOOM_MODE_FREE` (12) or
 *                    `DJI_SPOTLIGHT_ZOOM_MODE_FREE_LOCK_YAW` (14).
 *   - FREE (12):          SPZ free mode — only gimbal is controlled, aircraft
 *                         heading is NOT locked to the target.
 *   - FREE_LOCK_YAW (14): SPZ free mode with yaw lock — both gimbal and aircraft
 *                         yaw are controlled to keep the target centered.
 *                         This is the standard mode for approach control.
 *   Other submodes (e.g., CIRCLE=13) indicate a different SPZ sub-function and
 *   do NOT constitute intelligent tracking in the context of approach mode.
 *
 * **Return value**
 *   *isActive is set to true only when BOTH conditions are met. A false result
 *   may mean the SPZ function has not been entered, the target was lost, the
 *   aircraft switched to another flight mode, or status push has not yet
 *   been received.
 *
 * **Additional check (caller's responsibility)**
 *   For complete verification, the caller should also confirm that the OSD
 *   push mode field equals `E_DJI_MC_MODE_TYPE_NAVI_SUBMODE_TA` (51),
 *   which indicates the flight controller has entered the Target-Acquisition
 *   navigation sub-mode. This function does NOT check the OSD mode to avoid
 *   coupling with the flight controller module.
 *  - This API is not available in the current version.
 *
 * @param[out] isActive  Pointer to a bool that receives the result.
 *                       true  = SPZ intelligent tracking is active.
 *                       false = not in tracking state (or state not yet received).
 * @return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS on success.
 *         DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER if isActive is NULL.
 */
/**
 * @brief Feed the latest OSD mode into the interest point module.
 *        Must be called from the OSD callback so that DjiInterestPoint_IsSpzTrackingActive
 *        can include the mode condition in its check.
 * @param mode  The 7-bit mode field from dji_fc_osd_push (enum DJI_MC_MODE_TYPE).
 */
void DjiInterestPoint_UpdateOsdMode(uint8_t mode);
T_DjiReturnCode DjiInterestPoint_IsSpzTrackingActive(bool *isActive);

#ifdef __cplusplus
}
#endif

#endif // DJI_INTEREST_POINT_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
