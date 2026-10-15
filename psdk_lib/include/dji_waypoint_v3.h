/**
 ********************************************************************
 * @file    dji_waypoint_v3.h
 * @brief   This is the header file for "dji_waypoint_v3.c", defining the structure and
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
#ifndef DJI_WAYPOINT_V3_H
#define DJI_WAYPOINT_V3_H

/* Includes ------------------------------------------------------------------*/
#include "dji_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/**
* @brief Waypoint v3 mission action.
*/
typedef enum {
    DJI_WAYPOINT_V3_ACTION_START = 0, /*!< Waypoint v3 mission start action. */
    DJI_WAYPOINT_V3_ACTION_STOP = 1, /*!< Waypoint v3 mission stop action. */
    DJI_WAYPOINT_V3_ACTION_PAUSE = 2, /*!< Waypoint v3 mission pause action. */
    DJI_WAYPOINT_V3_ACTION_RESUME = 3, /*!< Waypoint v3 mission resume action. */
} E_DjiWaypointV3Action;

/**
*  Waypoint v3 current mission state.
*/
typedef enum {
    DJI_WAYPOINT_V3_MISSION_STATE_IDLE = 0, /*!< Waypoint v3 mission in idle state. */
    DJI_WAYPOINT_V3_MISSION_STATE_PREPARE = 16, /*!< Waypoint v3 mission in prepare state. */
    DJI_WAYPOINT_V3_MISSION_STATE_TRANS_MISSION = 32, /*!< Waypoint v3 mission in trans mission state. */
    DJI_WAYPOINT_V3_MISSION_STATE_MISSION = 48, /*!< Waypoint v3 mission in mission state. */
    DJI_WAYPOINT_V3_MISSION_STATE_BREAK = 64, /*!< Waypoint v3 mission in break state. */
    DJI_WAYPOINT_V3_MISSION_STATE_RESUME = 80, /*!< Waypoint v3 mission in resume state. */
    DJI_WAYPOINT_V3_MISSION_STATE_RETURN_FIRSTPOINT = 98, /*!< Waypoint v3 mission in return first point state. */
} E_DjiWaypointV3MissionState;

/**
*  Waypoint v3 current action state.
*/
typedef enum {
    DJI_WAYPOINT_V3_ACTION_STATE_IDLE = 0, /*!< Waypoint v3 action in idle state. */
    DJI_WAYPOINT_V3_ACTION_STATE_RUNNING = 1, /*!< Waypoint v3 action in idle state. */
    DJI_WAYPOINT_V3_ACTION_STATE_FINISHED = 5, /*!< Waypoint v3 action in idle state. */
} E_DjiWaypointV3ActionState;

/**
*  Waypoint v3 mission state.
*/
typedef struct {
    E_DjiWaypointV3MissionState state; /*!< Waypoint v3 current mission state, #E_DjiWaypointV3MissionState. */
    uint32_t wayLineId; /*!< Waypoint v3 current way line id. */
    uint16_t currentWaypointIndex; /*!< Waypoint v3 current waypoint index. */
} T_DjiWaypointV3MissionState;

/**
*  Waypoint v3 action state.
*/
typedef struct {
    E_DjiWaypointV3ActionState state; /*!< Waypoint v3 current action state, #E_DjiWaypointV3ActionState. */
    uint32_t wayLineId; /*!< Waypoint v3 current way line id. */
    uint16_t currentWaypointIndex; /*!< Waypoint v3 current waypoint index. */
    uint16_t actionGroupId; /*!< Waypoint v3 current action group index. */
    uint16_t actionId; /*!< Waypoint v3 current action index. */
} T_DjiWaypointV3ActionState;

/**
 * @brief External localization data for ADN mode positioning.
 * @note  The timestamp should be aligned with the 6-DOF data timestamp received by the payload.
 */
typedef struct {
    uint32_t version;       /*!< Protocol version. */
    double   longitude;     /*!< Longitude in radians. */
    double   latitude;      /*!< Latitude in radians. */
    float    altitude;      /*!< Ellipsoidal height in meters. */
    float    heading;       /*!< Heading in degrees. NED frame: north=0°, east=90°, south=180°. */
    float    horizError;    /*!< Horizontal position accuracy in meters. */
    float    vertError;     /*!< Vertical position accuracy in meters. */
    float    headingError;  /*!< Heading accuracy in degrees. */
    uint32_t timestamp;     /*!< Timestamp in milliseconds, aligned with the 6-DOF data timestamp. */
} T_DjiWaypointV3ExternalLocalizationInfo;

/**
 * @brief ADN assist positioning mode status pushed by the flight controller.
 */
typedef struct {
    uint32_t version;       /*!< Protocol version. */
    float    gpsError;      /*!< Deviation between GPS and positioning estimate, in meters. */
    float    sdkError;      /*!< Deviation between SDK and positioning estimate, in meters. */
    float    gpsSdkError;   /*!< Deviation between SDK and GPS, in meters. */
    float    divergeError;  /*!< Divergence reference error, in meters. */
    uint32_t isGnssValid;   /*!< GPS availability: 1 = available, 0 = unavailable. */
} T_DjiWaypointV3AssistPosModeInfo;

/**
 * @brief Prototype of callback function used to receive the waypoint v3 mission state.
 * @warning User can not execute blocking style operations or functions in callback function, because that will block
 * root thread, causing problems such as slow system response, payload disconnection or infinite loop.
 * @param missionState: current waypoint v3 mission state.
 * @return Execution result.
 */
typedef T_DjiReturnCode (*WaypointV3MissionStateCallback)(T_DjiWaypointV3MissionState missionState);

/**
 * @brief Prototype of callback function used to receive the waypoint v3 mission state.
 * @warning User can not execute blocking style operations or functions in callback function, because that will block
 * root thread, causing problems such as slow system response, payload disconnection or infinite loop.
 * @param missionState: current waypoint v3 mission state.
 * @return Execution result.
 */
typedef T_DjiReturnCode (*WaypointV3ActionStateCallback)(T_DjiWaypointV3ActionState missionState);

/**
 * @brief Prototype of callback function used to receive ADN assist positioning mode status.
 * @warning User can not execute blocking style operations or functions in callback function, because that will block
 * root thread, causing problems such as slow system response, payload disconnection or infinite loop.
 * @param info: current assist positioning mode status pushed by the flight controller.
 * @return Execution result.
 */
typedef T_DjiReturnCode (*WaypointV3AssistPosModeInfoCallback)(T_DjiWaypointV3AssistPosModeInfo info);

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Initialise waypoint v3 module, and user should call this function.
 * before using waypoint v3 features.
 * @return Execution result.
 */
T_DjiReturnCode DjiWaypointV3_Init(void);

/**
 * @brief Deinitialise waypoint v3 module.
 * @return Execution result.
 */
T_DjiReturnCode DjiWaypointV3_DeInit(void);

/**
 * @brief Upload kmz file by raw data.
 * @note The size of kmz file is very small, you can use this interface to upload quickly.
 * @return Execution result.
 */
T_DjiReturnCode DjiWaypointV3_UploadKmzFile(const uint8_t *data, uint32_t dataLen);

/**
 * @brief Execute the mission action.
 * @note This action should be called after uploading the kmz file.
 * @return Execution result.
 */
T_DjiReturnCode DjiWaypointV3_Action(E_DjiWaypointV3Action action);

/**
 * @brief Register the mission state callback for waypoint mission.
 * @note If you want to monitor the state of waypoint mission, this interface should be called before uploading kmz
 * file or executing this mission action.
 * @return Execution result.
 */
T_DjiReturnCode DjiWaypointV3_RegMissionStateCallback(WaypointV3MissionStateCallback callback);

/**
 * @brief Register the action state callback for waypoint mission.
 * @note If you want to monitor the state of waypoint action, this interface should be called before uploading kmz
 * file or executing this mission action.
 * @return Execution result.
 */
T_DjiReturnCode DjiWaypointV3_RegActionStateCallback(WaypointV3ActionStateCallback callback);

/**
 * @brief Register the callback for ADN assist positioning mode status.
 * @note  This interface should be called after DjiWaypointV3_Init and before entering ADN mode.
 *        The callback is invoked each time the flight controller pushes an assist positioning
 *        mode status update. Pass NULL to unregister and stop receiving callbacks.
 * @return Execution result.
 */
T_DjiReturnCode DjiWaypointV3_RegAssistPosModeInfoCallback(WaypointV3AssistPosModeInfoCallback callback);


/**
 * @brief Send external localization data to the aircraft for ADN mode positioning.
 * @details In ADN mode, the aircraft uses the position data sent via this interface as its
 *          external position reference for no-GPS flight. This interface should be called
 *          continuously at a fixed frequency to maintain stable positioning.
 * @param info Pointer to the localization data to send.
 * @return Execution result.
 */
T_DjiReturnCode DjiWaypointV3_SendExternalLocalizationInfo(
    const T_DjiWaypointV3ExternalLocalizationInfo *info);

#ifdef __cplusplus
}
#endif

#endif // DJI_WAYPOINT_V3_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
