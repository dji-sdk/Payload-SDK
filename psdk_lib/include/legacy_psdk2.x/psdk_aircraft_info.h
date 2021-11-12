/**
 ********************************************************************
 * @file    psdk_aircraft_info.h
 * @brief   This is the header file for "psdk_aircraft_info.c", defining the structure and
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
#ifndef PSDK_AIRCRAFT_INFO_H
#define PSDK_AIRCRAFT_INFO_H

/* Includes ------------------------------------------------------------------*/
#include "legacy_psdk2.x/psdk_typedef.h"
#include <dji_typedef.h>
#include <dji_aircraft_info.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/
/**
 * @brief Aircraft type.
 */
typedef E_DjiAircraftType E_PsdkAircraftInfoType;
#define PSDK_AIRCRAFT_INFO_TYPE_UNKNOWN     DJI_AIRCRAFT_TYPE_UNKNOWN /*!< Aircraft type is unknown. */
#define PSDK_AIRCRAFT_INFO_TYPE_M200_V2     DJI_AIRCRAFT_TYPE_M200_V2 /*!< Aircraft type is Matrice 200 V2. */
#define PSDK_AIRCRAFT_INFO_TYPE_M210_V2     DJI_AIRCRAFT_TYPE_M210_V2 /*!< Aircraft type is Matrice 220 V2. */
#define PSDK_AIRCRAFT_INFO_TYPE_M210RTK_V2  DJI_AIRCRAFT_TYPE_M210RTK_V2 /*!< Aircraft type is Matrice 210 RTK V2. */
#define PSDK_AIRCRAFT_INFO_TYPE_M300_RTK    DJI_AIRCRAFT_TYPE_M300_RTK /*!< Aircraft type is Matrice 300 RTK. */

/**
 * @brief PSDK adapter type.
 */
typedef E_DjiSdkAdapterType E_PsdkAircraftInfoPsdkAdapterType;
#define PSDK_AIRCRAFT_INFO_PSDK_ADAPTER_TYPE_UNKNOWN       DJI_SDK_ADAPTER_TYPE_UNKNOWN /*!< PSDK adapter type is unknown. */
#define PSDK_AIRCRAFT_INFO_PSDK_ADAPTER_TYPE_SKYPORT_V2    DJI_SDK_ADAPTER_TYPE_SKYPORT_V2 /*!< PSDK adapter type is Skyport V2. */
#define PSDK_AIRCRAFT_INFO_PSDK_ADAPTER_TYPE_XPORT         DJI_SDK_ADAPTER_TYPE_XPORT /*!< PSDK adapter type is X-Port. */

/**
 * @brief Payload mount position.
 */
typedef E_DjiMountPosition E_PsdkAircraftInfoPayloadMountPosition;
#define PSDK_AIRCRAFT_INFO_PAYLOAD_MOUNT_POSITION_UNKNOWN   DJI_MOUNT_POSITION_UNKNOWN /*!< Payload mount position is unknown.. */
#define PSDK_AIRCRAFT_INFO_PAYLOAD_MOUNT_POSITION_NO1       DJI_MOUNT_POSITION_PAYLOAD_PORT_NO1 /*!< Payload is mounted on the first gimbal connector of aircraft. */
#define PSDK_AIRCRAFT_INFO_PAYLOAD_MOUNT_POSITION_NO2       DJI_MOUNT_POSITION_PAYLOAD_PORT_NO2 /*!< Payload is mounted on the second gimbal connector of aircraft. */
#define PSDK_AIRCRAFT_INFO_PAYLOAD_MOUNT_POSITION_NO3       DJI_MOUNT_POSITION_PAYLOAD_PORT_NO3 /*!< Payload is mounted on the third gimbal connector of aircraft. */

/**
 * @brief Mobile APP system language.
 */
typedef E_DjiMobileAppLanguage E_PsdkAircraftInfoMobileAppLanguage;
#define PSDK_AIRCRAFT_INFO_MOBILE_APP_LANGUAGE_UNKNOWN      DJI_MOBILE_APP_LANGUAGE_UNKNOWN /*!< The system language of the mobile app is unknown */
#define PSDK_AIRCRAFT_INFO_MOBILE_APP_LANGUAGE_ENGLISH      DJI_MOBILE_APP_LANGUAGE_ENGLISH /*!< The system language of the mobile app is English */
#define PSDK_AIRCRAFT_INFO_MOBILE_APP_LANGUAGE_CHINESE      DJI_MOBILE_APP_LANGUAGE_CHINESE /*!< The system language of the mobile app is Chinese */
#define PSDK_AIRCRAFT_INFO_MOBILE_APP_LANGUAGE_JAPANESE     DJI_MOBILE_APP_LANGUAGE_JAPANESE /*!< The system language of the mobile app is Japanese */
#define PSDK_AIRCRAFT_INFO_MOBILE_APP_LANGUAGE_FRENCH       DJI_MOBILE_APP_LANGUAGE_FRENCH /*!< The system language of the mobile app is French */

/**
 * @brief Mobile APP screen size type.
 */
typedef E_DjiMobileAppScreenType E_PsdkAircraftInfoMobileAppScreenType;
#define PSDK_AIRCRAFT_INFO_MOBILE_APP_SCREEN_TYPE_UNKNOWN       DJI_MOBILE_APP_SCREEN_TYPE_UNKNOWN /*!< Mobile APP screen type is unknown. */
#define PSDK_AIRCRAFT_INFO_MOBILE_APP_SCREEN_TYPE_BIG_SCREEN    DJI_MOBILE_APP_SCREEN_TYPE_BIG_SCREEN /*!< The big screen of mobile device refers to a screen
                                                                                                       * size greater than or equal to 6 inches. */
#define PSDK_AIRCRAFT_INFO_MOBILE_APP_SCREEN_TYPE_LITTLE_SCREEN DJI_MOBILE_APP_SCREEN_TYPE_LITTLE_SCREEN /*!< The little screen of mobile device refers to a
                                                                                                          * screen size less than 6 inches. */

/**
 * @brief Some base information of aircraft system, mainly including some constant parameters information of system.
 */
typedef struct {
    E_PsdkAircraftInfoType aircraftType; /*!< Aircraft type. */
    E_PsdkAircraftInfoPsdkAdapterType psdkAdapterType; /*!< PSDK adapter type. */
    E_PsdkAircraftInfoPayloadMountPosition payloadMountPosition; /*!< Payload mount position. */
} T_PsdkAircraftInfoBaseInfo;

/**
 * @brief Information related to mobile APP.
 */
typedef T_DjiMobileAppInfo T_PsdkAircraftInfoMobileAppInfo;

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Get base information of aircraft system, including aircraft type and PSDK adapter type.
 * @param baseInfo: pointer to memory space used to store base information of the aircraft system.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkAircraftInfo_GetBaseInfo(T_PsdkAircraftInfoBaseInfo *baseInfo);

/**
 * @brief Get information related to mobile APP.
 * @note The mobile APP language and screen type is unknown if RC or APP is not connected to the aircraft system.
 * @param mobileAppInfo: pointer to memory space used to store information related to mobile APP.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkAircraftInfo_GetMobileAppInfo(T_PsdkAircraftInfoMobileAppInfo *mobileAppInfo);

#ifdef __cplusplus
}
#endif

#endif //PSDK_AIRCRAFT_INFO_H

/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
