/**
 ********************************************************************
 * @file    psdk_time_sync.h
 * @brief   This is the header file for "psdk_time_synchronization.c", defining the structure and
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
#ifndef PSDK_TIME_SYNC_H
#define PSDK_TIME_SYNC_H

/* Includes ------------------------------------------------------------------*/
#include "legacy_psdk2.x/psdk_typedef.h"
#include "dji_time_sync.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/
/**
 * @brief Prototype of callback function used to get the newest PPS triggered timestamp.
 * @warning User can not execute blocking style operations or functions in callback function, because that will block PSDK
 * root thread, causing problems such as slow system response, payload disconnection or infinite loop.
 * @param localTimeUs: pointer to memory space used to store PPS triggered timestamp.
 * @return Execution result.
 */
typedef DjiGetNewestPpsTriggerLocalTimeUsCallback GetNewestPpsTriggerLocalTimeUsCallback;

/**
 * @brief Data structure of time in aircraft time system.
 */
typedef T_DjiTimeSyncAircraftTime T_PsdkTimeSyncAircraftTime;

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Initialise time synchronization module in blocking mode. User should call this function before all other time
 * synchronization operations, just like transferring time.
 * @note Max execution time of this function is slightly larger than 2000ms.
 * @note This function have to be called in user task, rather than main() function, and after scheduler being started.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkTimeSync_Init(void);

/**
 * @brief Register callback function used to get the newest timestamp in local time system when PPS rising edge signal
 * is detected.
 * @details PSDK uses the timestamp information to synchronise time of local time system and RTK navigation and
 * positioning system.
 * @param callback: pointer to the callback function.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkTimeSync_RegGetNewestPpsTriggerTimeCallback(GetNewestPpsTriggerLocalTimeUsCallback callback);

/**
 * @brief Transfer local time to time in aircraft time system.
 * @note Before calling the interface, users must call PsdkTimeSync_RegGetNewestPpsTriggerTimeCallback() to register
 * callback function used to report the latest triggered time of PPS signal.
 * @param localTimeUs: local time, unit: microsecond.
 * @param aircraftTime: pointer to memory space used to store time in aircraft time system.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkTimeSync_TransferToAircraftTime(uint64_t localTimeUs, T_PsdkTimeSyncAircraftTime *aircraftTime);

#ifdef __cplusplus
}
#endif

#endif //PSDK_TIME_SYNC_H

/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
