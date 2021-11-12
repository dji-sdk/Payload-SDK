/**
 ********************************************************************
 * @file    dji_hms.h
 * @brief   This is the header file for "dji_hms.c", defining the structure and
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
#ifndef DJI_HMS_H
#define DJI_HMS_H

/* Includes ------------------------------------------------------------------*/
#include "dji_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef struct {
    uint32_t errorCode;
    uint8_t componentIndex;
    uint8_t errorLevel;
} T_DjiHmsInfo;

typedef struct {
    T_DjiHmsInfo *hmsInfo;
    uint32_t hmsInfoNum;
} T_DjiHmsInfoTable;

typedef T_DjiReturnCode (*DjiHmsInfoCallback)(T_DjiHmsInfoTable hmsInfoTable);

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Initialise hms module, and user should call this function
 * before using hms features.
 * @return Execution result.
 */
T_DjiReturnCode DjiHms_Init(void);

/**
 * @brief Deinitialise hms module.
 * @return Execution result.
 */
T_DjiReturnCode DjiHms_DeInit(void);

/**
 * @brief Register callback to get hms info.
 * @note: Data is pushed with a frequency of 1Hz.
 * @param callback: see reference of DjiHmsInfoCallback.
 * @return Execution result.
 */
T_DjiReturnCode DjiHms_RegHmsInfoCallback(DjiHmsInfoCallback callback);

#ifdef __cplusplus
}
#endif

#endif // DJI_HMS_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
