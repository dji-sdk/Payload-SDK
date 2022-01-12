/**
 ********************************************************************
 * @file    psdk_typedef.h
 * @brief   PSDK type define file.
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
#ifndef PSDK_TYPEDEF_H
#define PSDK_TYPEDEF_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "psdk_error.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/
#define PI                          (3.14159265358979323846f)
#define PSDK_FILE_NAME_SIZE_MAX     256
#define PSDK_FILE_PATH_SIZE_MAX     (PSDK_FILE_NAME_SIZE_MAX + 256)
#define PSDK_IP_ADDR_STR_SIZE_MAX   15
#define PSDK_MD5_BUFFER_LEN         16

/* Exported types ------------------------------------------------------------*/
/**
 * @brief Type define double as psdk_f64_t.
 */
typedef double psdk_f64_t;
/**
 * @brief Type define float as psdk_f32_t.
 */
typedef float psdk_f32_t;
/**
 * @brief Type define uint64 as T_PsdkReturnCode.
 * @details The type can be any value of ::PsdkErrorCode.
 */
typedef uint64_t T_PsdkReturnCode;

#pragma pack (1)

typedef struct Vector3d {
    int32_t x; /*!< Specifies int32 value of x for vector. */
    int32_t y; /*!< Specifies int32 value of y for vector. */
    int32_t z; /*!< Specifies int32 value of z for vector. */
} T_PsdkVector3d;

typedef struct Vector3f {
    psdk_f32_t x; /*!< Specifies float value of x for vector. */
    psdk_f32_t y; /*!< Specifies float value of y for vector. */
    psdk_f32_t z; /*!< Specifies float value of z for vector. */
} T_PsdkVector3f;

typedef struct {
    int32_t pitch; /*!< Specifies int32 value of pitch for attitude. */
    int32_t roll; /*!< Specifies int32 value of roll for attitude */
    int32_t yaw; /*!< Specifies int32 value of yaw for attitude */
} T_PsdkAttitude3d;

typedef struct {
    psdk_f32_t pitch; /*!< Specifies float value of pitch for attitude. */
    psdk_f32_t roll; /*!< Specifies float value of roll for attitude */
    psdk_f32_t yaw; /*!< Specifies float value of yaw for attitude */
} T_PsdkAttitude3f;

typedef struct {
    psdk_f32_t q0; /*!< w, when converted to a rotation matrix or Euler angles. */
    psdk_f32_t q1; /*!< x, when converted to a rotation matrix or Euler angles. */
    psdk_f32_t q2; /*!< y, when converted to a rotation matrix or Euler angles. */
    psdk_f32_t q3; /*!< z, when converted to a rotation matrix or Euler angles. */
} T_PsdkQuaternion4f;

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
