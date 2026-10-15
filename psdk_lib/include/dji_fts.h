/**
 ********************************************************************
 * @file    dji_network_rtk.h
 * @version V1.0.0
 * @date    2025/10/09
 * @brief   This is the header file for "dij_core.c", defining the structure and
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
#ifndef DJI_NETWORK_RTK_H
#define DJI_NETWORK_RTK_H

/* Includes ------------------------------------------------------------------*/
#include "dji_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/
typedef enum {
    DJI_FTS_NOT_TRIGGERD = 0,
    DJI_FTS_TRIGGERD = 1,
} E_DjiFtsStatus;

/* Exported types ------------------------------------------------------------*/

typedef struct {
    E_DjiMountPosition fts_select;
    E_DjiFtsStatus fts_status;
    uint8_t fts_pwm_cnt; /* correct number of PWM signals received */
} T_DjiFtsPwmTriggerStatus;

typedef struct {
    T_DjiFtsPwmTriggerStatus ESC[4]; /* trigger status of the two ESCs */
} T_DjiFtsPwmEscTriggerStatus;

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Select Fts pwm trigger.
 * - Notes:Timing requirement: This API must be called while the aircraft is on the ground (not airborne). Calls made during flight will fail or be rejected.
 * - Function: This call only selects/enables the PWM trigger port on the flight controller side.
 *   It does NOT emit PWM signals nor perform the motor-stop action itself. The actual motor-stop must be triggered by sending PWM signals via external PWM hardware pins.
 * - Recommended flow:
 *   1) Call DjiFlightController_SelectFtsPwmTrigger(position) on ground to enable the port;
 *   2) Send the motor-stop PWM from an external PWM controller to that port;
 * @param position
 * - Supported models/ports:
 *   - M400: only support DJI_MOUNT_POSITION_EXTENSION_PORT_V2_NO4.
 * @return Possible failure reasons include invalid param, aircraft not on ground, hardware unsupported, or module not initialized.
 */
T_DjiReturnCode DjiFts_SelectFtsPwmTrigger(E_DjiMountPosition position);

/**
 * @brief Get Fts pwm trigger status.
 * Notes:This API is deprecated and will be removed in a future release. It is NOT recommended for use. Supported models only: M4 serials.
 * Recommended alternative: To confirm motor-stop (FTS) effects, use DJI_FC_SUBSCRIPTION_TOPIC_ESC_DATA fc subscription
 * @return Execution result.
 */
T_DjiReturnCode DjiFts_GetFtsPwmTriggerStatus(T_DjiFtsPwmEscTriggerStatus* trigger_status);

/**
 * @brief Resolve a mount position to its canonical position.
 * When isFtsPosDupblicate is set, alias positions (e.g. V3_NO3) are remapped
 * to the canonical entry in positionMapTriggerArray (e.g. V3_NO2) so that
 * callers can compare against the value returned by DjiFts_GetFtsPwmTriggerStatus.
 * For products without duplicate positions the input is returned unchanged.
 * @param position: The mount position to resolve.
 * @return Canonical mount position.
 */
E_DjiMountPosition DjiFts_ResolveCanonicalPosition(E_DjiMountPosition position);

#ifdef __cplusplus
}
#endif

#endif // DIJ_CORE_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
