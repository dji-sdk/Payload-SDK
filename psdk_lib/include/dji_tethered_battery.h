/**
 ********************************************************************
 * @file    dji_tethered_battery.h
 * @brief   This is the header file for "dji_tethered_battery.c", defining the structure and
 * (exported) function prototypes.
 *
 * @copyright (c) 2024 DJI. All rights reserved.
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
#ifndef DJI_TETHERED_BATTERY_H
#define DJI_TETHERED_BATTERY_H

/* Includes ------------------------------------------------------------------*/
#include "dji_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/**
 * @brief Tether line status.
 */
typedef struct {
    dji_f32_t totalLength; /*!< total length of tether line, range: 0-300, unit: meters. */
    dji_f32_t usedLength; /*!< used length of tether line, range: 0-300, unit: meters.
                               This value must be less than or equal to the total length of tether line. */
} T_DjiTetherLineStatus;

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Initialise tethered battery module
 * @note User should call this function before using tethered battery features.
 * @return Execution result.
 */
T_DjiReturnCode DjiTetheredBattery_Init(void);

/**
 * @brief DeInitialize tethered battery module.
 * @return Execution result.
 */
T_DjiReturnCode DjiTetheredBattery_DeInit(void);

/**
 * @brief Push the real-time tether line length to the Pilot2 for display.
 * @note It is recommended that the push frequency does not exceed the data update frequency. The maximum push
 * frequency should not exceed 10Hz.
 * @param tetherLineStatus: the tether line status.
 * @return Execution result.
 */
T_DjiReturnCode DjiTetheredBattery_PushTetherLineStatus(T_DjiTetherLineStatus tetherLineStatus);

#ifdef __cplusplus
}
#endif

#endif // DJI_TETHERED_BATTERY_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
