/**
 ********************************************************************
 * @file    psdk_aircraft_info.h
 * @brief   This is the header file for "psdk_aircraft_info.c", defining the structure and
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
#ifndef PSDK_AIRCRAFT_INFO_H
#define PSDK_AIRCRAFT_INFO_H

/* Includes ------------------------------------------------------------------*/
#include "psdk_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/
/**
 * @brief Aircraft type.
 */
typedef enum {
    PSDK_AIRCRAFT_INFO_TYPE_UNKNOWN = 0, /*!< Aircraft type is unknown. */
    PSDK_AIRCRAFT_INFO_TYPE_M200_V2 = 44, /*!< Aircraft type is Matrice 200 V2. */
    PSDK_AIRCRAFT_INFO_TYPE_M210_V2 = 45, /*!< Aircraft type is Matrice 220 V2. */
    PSDK_AIRCRAFT_INFO_TYPE_M210RTK_V2 = 46, /*!< Aircraft type is Matrice 210 RTK V2. */
    PSDK_AIRCRAFT_INFO_TYPE_M300_RTK = 60, /*!< Aircraft type is Matrice 300 RTK. */
} E_PsdkAircraftInfoType;

/**
 * @brief PSDK adapter type.
 */
typedef enum {
    PSDK_AIRCRAFT_INFO_PSDK_ADAPTER_TYPE_UNKNOWN = 0, /*!< PSDK adapter type is unknown. */
    PSDK_AIRCRAFT_INFO_PSDK_ADAPTER_TYPE_SKYPORT_V2 = 1, /*!< PSDK adapter type is Skyport V2. */
    PSDK_AIRCRAFT_INFO_PSDK_ADAPTER_TYPE_XPORT = 2, /*!< PSDK adapter type is X-Port. */
} E_PsdkAircraftInfoPsdkAdapterType;

/**
 * @brief Payload mount position.
 */
typedef enum {
    PSDK_AIRCRAFT_INFO_PAYLOAD_MOUNT_POSITION_UNKNOWN = 0, /*!< Payload mount position is unknown.. */
    PSDK_AIRCRAFT_INFO_PAYLOAD_MOUNT_POSITION_NO1 = 1, /*!< Payload is mounted on the first gimbal connector of aircraft. */
    PSDK_AIRCRAFT_INFO_PAYLOAD_MOUNT_POSITION_NO2 = 2, /*!< Payload is mounted on the second gimbal connector of aircraft. */
    PSDK_AIRCRAFT_INFO_PAYLOAD_MOUNT_POSITION_NO3 = 3, /*!< Payload is mounted on the third gimbal connector of aircraft. */
} E_PsdkAircraftInfoPayloadMountPosition;

/**
 * @brief Mobile APP system language.
 */
typedef enum {
    PSDK_AIRCRAFT_INFO_MOBILE_APP_LANGUAGE_UNKNOWN = 255, /*!< The system language of the mobile app is unknown */
    PSDK_AIRCRAFT_INFO_MOBILE_APP_LANGUAGE_ENGLISH = 0, /*!< The system language of the mobile app is English */
    PSDK_AIRCRAFT_INFO_MOBILE_APP_LANGUAGE_CHINESE = 1, /*!< The system language of the mobile app is Chinese */
    PSDK_AIRCRAFT_INFO_MOBILE_APP_LANGUAGE_JAPANESE = 2, /*!< The system language of the mobile app is Japanese */
    PSDK_AIRCRAFT_INFO_MOBILE_APP_LANGUAGE_FRENCH = 3, /*!< The system language of the mobile app is French */
} E_PsdkAircraftInfoMobileAppLanguage;

/**
 * @brief Mobile APP screen size type.
 */
typedef enum {
    PSDK_AIRCRAFT_INFO_MOBILE_APP_SCREEN_TYPE_UNKNOWN = 255, /*!< Mobile APP screen type is unknown. */
    PSDK_AIRCRAFT_INFO_MOBILE_APP_SCREEN_TYPE_BIG_SCREEN = 0, /*!< The big screen of mobile device refers to a screen
                                                                * size greater than or equal to 6 inches. */
    PSDK_AIRCRAFT_INFO_MOBILE_APP_SCREEN_TYPE_LITTLE_SCREEN = 1, /*!< The little screen of mobile device refers to a
                                                                    * screen size less than 6 inches. */
} E_PsdkAircraftInfoMobileAppScreenType;

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
typedef struct {
    E_PsdkAircraftInfoMobileAppLanguage appLanguage; /*!< Mobile APP system language */
    E_PsdkAircraftInfoMobileAppScreenType appScreenType; /*!< Mobile APP screen size type. */
} T_PsdkAircraftInfoMobileAppInfo;

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
