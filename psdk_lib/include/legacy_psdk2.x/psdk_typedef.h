/**
 ********************************************************************
 * @file    psdk_typedef.h
 * @brief   PSDK type define file.
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
#ifndef PSDK_TYPEDEF_H
#define PSDK_TYPEDEF_H

/* Includes ------------------------------------------------------------------*/
#include "legacy_psdk2.x/psdk_error.h"
#include "dji_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/
#define PI                              DJI_PI
#define PSDK_FILE_NAME_SIZE_MAX         DJI_FILE_NAME_SIZE_MAX
#define PSDK_FILE_PATH_SIZE_MAX         DJI_FILE_PATH_SIZE_MAX
#define PSDK_IP_ADDR_STR_SIZE_MAX       DJI_IP_ADDR_STR_SIZE_MAX
#define PSDK_MD5_BUFFER_LEN             DJI_MD5_BUFFER_LEN

/* Exported types ------------------------------------------------------------*/
/**
 * @brief Type define double as psdk_f64_t.
 */
typedef dji_f64_t psdk_f64_t;
/**
 * @brief Type define float as psdk_f32_t.
 */
typedef dji_f32_t psdk_f32_t;
/**
 * @brief Type define uint64 as T_PsdkReturnCode.
 * @details The type can be any value of ::PsdkErrorCode.
 */
typedef T_DjiReturnCode T_PsdkReturnCode;

#pragma pack (1)

typedef T_DjiVector3d T_PsdkVector3d;

typedef T_DjiVector3f T_PsdkVector3f;

typedef T_DjiAttitude3d T_PsdkAttitude3d;

typedef T_DjiAttitude3f T_PsdkAttitude3f;

typedef T_DjiQuaternion4f T_PsdkQuaternion4f;

typedef T_DjiDataChannelState T_PsdkDataChannelState;

#pragma pack ()

/* define compiler specific symbols */
#if defined (__ICCARM__)
#elif defined (__CC_ARM)
#pragma anon_unions
#elif defined (__GNUC__)
#elif defined (__TASKING__)
#endif

#ifdef __cplusplus
}
#endif

#endif //PSDK_TYPEDEF_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
