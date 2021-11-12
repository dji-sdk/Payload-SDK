/**
 ********************************************************************
 * @file    psdk_core.h
 * @brief   This is the header file for "psdk_core.c", defining the structure and
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
#ifndef PSDK_CORE_H
#define PSDK_CORE_H

/* Includes ------------------------------------------------------------------*/
#include <legacy_psdk2.x/psdk_typedef.h>
#include <dji_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef T_DjiUserInfo T_PsdkUserInfo;

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Initialize the Payload SDK core in blocking mode.
 * @note The call location of this interface requires special attention, The call needs to be completed after the
 * registration of console/OSAL handler functions/HAL handler functions are completed. At the same time, it must be
 * initialized at the beginning of calling other functional module interfaces. You need to fill in the developer
 * information correctly to ensure the initialization is successful. For additional instructions, please refer to the
 * tutorial“PSDK Initialization”.
 * @note This function does not return until the correct aircraft type and PSDK adapter type is obtained. The logic ensures
 * that aircraft and PSDK adapter have been started up normally before PSDK functional module and user's program run.
 * General execution time of this function is 2~4 seconds.
 * @param userInfo: pointer to the PSDK application information.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkCore_Init(const T_PsdkUserInfo *userInfo);

/**
 * @brief Notify that the Payload SDK core application starts.
 * @note The call location of this interface requires special attention, The call needs to be completed after all the
 * module initialize and register interfaces.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkCore_ApplicationStart(void);

#ifdef __cplusplus
}
#endif

#endif //PSDK_CORE_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
