/**
 ********************************************************************
 * @file    dji_time_sync.h
 * @brief   This is the header file for "dji_time_sync.c", defining the structure and
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
#ifndef DJI_TIME_SYNC_H
#define DJI_TIME_SYNC_H

/* Includes ------------------------------------------------------------------*/
#include "dji_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/
/**
 * @brief Prototype of callback function used to get the newest PPS triggered timestamp.
 * @warning User can not execute blocking style operations or functions in callback function, because that will block DJI
 * root thread, causing problems such as slow system response, payload disconnection or infinite loop.
 * @param localTimeUs: pointer to memory space used to store PPS triggered timestamp.
 * @return Execution result.
 */
typedef T_DjiReturnCode (*DjiGetNewestPpsTriggerLocalTimeUsCallback)(uint64_t *localTimeUs);

/**
 * @brief Data structure of time in aircraft time system.
 */
typedef struct {
    uint16_t year;          /*!< Specifies year. */
    uint8_t month;          /*!< Specifies month, range from 1 to 12. */
    uint8_t day;            /*!< Specifies day, range from 1 to 31. */
    uint8_t hour;           /*!< Specifies hour, range from 0 to 23. */
    uint8_t minute;         /*!< Specifies minute, range from 0 to 59. */
    uint8_t second;         /*!< Specifies second, range from 0 to 59. */
    uint32_t microsecond;   /*!< Specifies microsecond, range from 0 to 999999. */
} T_DjiTimeSyncAircraftTime;

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Initialise time synchronization module in blocking mode. User should call this function before all other time
 * synchronization operations, just like transferring time.
 * @note Max execution time of this function is slightly larger than 2000ms.
 * @note This function have to be called in user task, rather than main() function, and after scheduler being started.
 * @return Execution result.
 */
T_DjiReturnCode DjiTimeSync_Init(void);

/**
 * @brief Register callback function used to get the newest timestamp in local time system when PPS rising edge signal
 * is detected.
 * @details DJI uses the timestamp information to synchronise time of local time system and RTK navigation and
 * positioning system.
 * @param callback: pointer to the callback function.
 * @return Execution result.
 */
T_DjiReturnCode DjiTimeSync_RegGetNewestPpsTriggerTimeCallback(DjiGetNewestPpsTriggerLocalTimeUsCallback callback);

/**
 * @brief Transfer local time to time in aircraft time system.
 * @note Before calling the interface, users must call DjiTimeSync_RegGetNewestPpsTriggerTimeCallback() to register
 * callback function used to report the latest triggered time of PPS signal.
 * @param localTimeUs: local time, unit: microsecond.
 * @param aircraftTime: pointer to memory space used to store time in aircraft time system.
 * @return Execution result.
 */
T_DjiReturnCode DjiTimeSync_TransferToAircraftTime(uint64_t localTimeUs, T_DjiTimeSyncAircraftTime *aircraftTime);

#ifdef __cplusplus
}
#endif

#endif //DJI_TIME_SYNC_H

/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
