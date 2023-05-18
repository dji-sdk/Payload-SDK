/**
 ********************************************************************
 * @file    dji_power_management.h
 * @brief   This is the header file for "dji_power_management.c", defining the structure and
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
#ifndef DJI_POWER_MANAGEMENT_H
#define DJI_POWER_MANAGEMENT_H

/* Includes ------------------------------------------------------------------*/
#include "dji_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/**
 * @brief Pin state.
 */
typedef enum {
    DJI_POWER_MANAGEMENT_PIN_STATE_RESET = 0, /*!< Specifies pin is in low level state. */
    DJI_POWER_MANAGEMENT_PIN_STATE_SET = 1, /*!< Specifies pin is in high level state. */
} E_DjiPowerManagementPinState;

/**
 * @brief Prototype of callback function used to set level of high power application pin.
 * @param pinState: level state of pin to be set.
 * @return Execution result.
 */
typedef T_DjiReturnCode (*DjiWriteHighPowerApplyPinCallback)(E_DjiPowerManagementPinState pinState);

/**
 * @brief Prototype of callback function used to notify payload that the aircraft will power off soon and get state
 * whether the payload is ready to power off or not.
 * @warning User can not execute blocking style operations or functions in a callback function, because that will block DJI
 * root thread, causing problems such as slow system response, payload disconnection or infinite loop.
 * @param powerOffPreparationFlag: pointer to memory space used to store power off preparation flag. True represents
 * that payload has completed all work and is ready to power off. False represents that payload is not ready to power
 * off.
 * @return Execution result.
 */
typedef T_DjiReturnCode (*DjiPowerOffNotificationCallback)(bool *powerOffPreparationFlag);

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Initialise power management module, and user should call this function before using power management features.
 * @return Execution result.
 */
T_DjiReturnCode DjiPowerManagement_Init(void);

/**
 * @brief DeInitialise power management module, and user should call this function before using power management features.
 * @return Execution result.
 */
T_DjiReturnCode DjiPowerManagement_DeInit(void);

/**
 * @brief Apply high power from aircraft in blocking mode.
 * @details Before applying, user should register callback function used to set level state of high power application
 * pin using DjiPowerManagement_RegWriteHighPowerApplyPinCallback() function. After applying high power, power pin of
 * DJI adapter will output high power based predetermined specification.
 * @note Max execution time of this function is slightly larger than 600ms.
 * @return Execution result.
 */
T_DjiReturnCode DjiPowerManagement_ApplyHighPowerSync(void);

/**
 * @brief Register callback function used to set level state of high power application pin. Must be called before
 * applying high power.
 * @param callback: pointer to the callback function.
 * @return Execution result.
 */
T_DjiReturnCode DjiPowerManagement_RegWriteHighPowerApplyPinCallback(DjiWriteHighPowerApplyPinCallback callback);

/**
 * @brief Register callback function used to notify payload that the aircraft will power off soon and get state
 * whether the payload is ready to power off or not.
 * @details After registering, the callback function will be called at a fixed frequency when aircraft will power off.
 * User fill in the power off preparation flag, and once the payload is ready to power off, the callback function will not
 * be called. After a specified time, if the payload is not ready to power off, the aircraft will power off immediately. The
 * specified time is 10s.
 * @param callback: pointer to callback function used to notify aircraft power off message and get payload power off
 * preparation flag.
 * @return Execution result.
 */
T_DjiReturnCode DjiPowerManagement_RegPowerOffNotificationCallback(DjiPowerOffNotificationCallback callback);

#ifdef __cplusplus
}
#endif

#endif // DJI_POWER_MANAGEMENT_H

/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
