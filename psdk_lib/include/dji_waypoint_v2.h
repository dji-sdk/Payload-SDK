/**
 ********************************************************************
 * @file    dji_waypoint_v2.h
 * @brief   This is the header file for "dji_waypoint_v2.c", defining the structure and
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
#ifndef DJI_WAYPOINT_V2_H
#define DJI_WAYPOINT_V2_H

/* Includes ------------------------------------------------------------------*/
#include "dji_typedef.h"
#include "dji_waypoint_v2_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef T_DjiReturnCode (*WaypointV2EventCbFunc)(T_DjiWaypointV2MissionEventPush eventData);
typedef T_DjiReturnCode (*WaypointV2StateCbFunc)(T_DjiWaypointV2MissionStatePush stateData);

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Initialise waypoint v2 module, and user should call this function
 * before using waypoint v2 features.
 * @return Execution result.
 */
T_DjiReturnCode DjiWaypointV2_Init(void);

/**
 * @brief Deinitialise waypoint v2 module.
 * @return Execution result.
 */
T_DjiReturnCode DjiWaypointV2_Deinit(void);

/**
 * @brief Init waypoint v2 mission settings
 * @param info: see reference of T_DjiWayPointV2MissionSettings.
 * @return Execution result.
 */
T_DjiReturnCode DjiWaypointV2_UploadMission(const T_DjiWayPointV2MissionSettings *info);

/**
 * @brief Start execute waypoint v2 mission
 * @return Execution result.
 */
T_DjiReturnCode DjiWaypointV2_Start(void);

/**
 * @brief Stop execute waypoint v2 mission
 * @return Execution result.
 */
T_DjiReturnCode DjiWaypointV2_Stop(void);

/**
 * @brief Pause execute waypoint v2 mission
 * @return Execution result.
 */
T_DjiReturnCode DjiWaypointV2_Pause(void);

/**
 * @brief Resume execute waypoint v2 mission
 * @return Execution result.
 */
T_DjiReturnCode DjiWaypointV2_Resume(void);

/**
 * @brief Get the global cruise speed setting from flight controller
 * @param cruiseSpeed: see references of T_DjiWaypointV2GlobalCruiseSpeed.
 * @return Execution result.
 */
T_DjiReturnCode DjiWaypointV2_GetGlobalCruiseSpeed(T_DjiWaypointV2GlobalCruiseSpeed *cruiseSpeed);

/**
 * @brief Set the global cruise speed setting to flight controller
 * @param cruiseSpeed: see references of T_DjiWaypointV2GlobalCruiseSpeed.
 * @return Execution result.
 */
T_DjiReturnCode DjiWaypointV2_SetGlobalCruiseSpeed(T_DjiWaypointV2GlobalCruiseSpeed cruiseSpeed);

/**
 * @brief Subscribe to waypointV2 event with a callback function.
 * @note There will be data when the event is triggered.
 * @param callback: see references of WaypointV2EventCbFunc.
 * @return Execution result.
 */
T_DjiReturnCode DjiWaypointV2_RegisterMissionEventCallback(WaypointV2EventCbFunc callback);

/**
 * @brief Subscribe to waypointV2 mission state with a callback function.
 * @note Data is pushed with a frequency of 10Hz.
 * @param callback: see references of WaypointV2StateCbFunc.
 * @return Execution result.
 */
T_DjiReturnCode DjiWaypointV2_RegisterMissionStateCallback(WaypointV2StateCbFunc callback);

#ifdef __cplusplus
}
#endif

#endif // DJI_WAYPOINT_V2_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
