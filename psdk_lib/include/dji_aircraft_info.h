/**
 ********************************************************************
 * @file    dji_aircraft_info.h
 * @brief   This is the header file for "dji_aircraft_info.c", defining the structure and
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
#ifndef DJI_AIRCRAFT_INFO_H
#define DJI_AIRCRAFT_INFO_H

/* Includes ------------------------------------------------------------------*/
#include "dji_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/**
 * @brief Information related to mobile APP.
 */
typedef struct {
    E_DjiMobileAppLanguage appLanguage; /*!< Mobile APP system language */
    E_DjiMobileAppScreenType appScreenType; /*!< Mobile APP screen size type. */
} T_DjiMobileAppInfo;

/**
 * @brief Some base information of aircraft system, mainly including some constant parameters information of system.
 */
typedef struct {
    E_DjiAircraftSeries aircraftSeries; /*!< Aircraft series. */
    E_DjiMountPositionType mountPositionType; /*!< Mount position type. */
    E_DjiAircraftType aircraftType; /*!< Aircraft type. */
    E_DjiSdkAdapterType djiAdapterType; /*!< DJI adapter type. */
    E_DjiMountPosition mountPosition; /*!< Payload mount position. */
} T_DjiAircraftInfoBaseInfo;

/**
 * @brief Aircraft version information.
 */
typedef struct {
    uint8_t debugVersion;
    uint8_t modifyVersion;
    uint8_t minorVersion;
    uint8_t majorVersion;
} T_DjiAircraftVersion;

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Get base information of aircraft system, including aircraft type and DJI adapter type.
 * @param baseInfo: pointer to memory space used to store base information of the aircraft system.
 * @return Execution result.
 */
T_DjiReturnCode DjiAircraftInfo_GetBaseInfo(T_DjiAircraftInfoBaseInfo *baseInfo);

/**
 * @brief Get information related to mobile APP.
 * @note The mobile APP language and screen type is unknown if RC or APP is not connected to the aircraft system.
 * @param mobileAppInfo: pointer to memory space used to store information related to mobile APP.
 * @return Execution result.
 */
T_DjiReturnCode DjiAircraftInfo_GetMobileAppInfo(T_DjiMobileAppInfo *mobileAppInfo);

/**
 * @brief Get connection status of payload and aircraft.
 * @note Update period: 1Hz
 * @param isConnected: pointer to connection status.
 * @return Execution result.
 */
T_DjiReturnCode DjiAircraftInfo_GetConnectionStatus(bool *isConnected);

/**
 * @brief Get version of aircraft.
 * @param aircraftVersion: pointer to aircraft version.
 * @return Execution result.
 */
T_DjiReturnCode DjiAircraftInfo_GetAircraftVersion(T_DjiAircraftVersion *aircraftVersion);

#ifdef __cplusplus
}
#endif

#endif //DJI_AIRCRAFT_INFO_H

/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
