/**
 ********************************************************************
 * @file    psdk_product_info.h
 * @brief   This is the header file for "psdk_product_info.c", defining the structure and
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
#ifndef PSDK_PRODUCT_INFO_H
#define PSDK_PRODUCT_INFO_H

/* Includes ------------------------------------------------------------------*/
#include <legacy_psdk2.x/psdk_typedef.h>
#include <dji_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/


/* Exported functions --------------------------------------------------------*/
/**
 * @brief Set an alias that satisfies the condition for PSDK application or product.
 * @details Alias will display in DJI Pilot, if exist.
 * @note Still need to pass in correct PSDK APP name that is obtained from DJI SDK developer website to PsdkCore_Init()
 * interface. The PSDK APP name will be used to bind or verification.
 * @note Alias will be effective after a while, and the max value is 1s.
 * @param productAlias: pointer to product alias string, and alias end with '\0'. The max length of the string is 31. If
 * length of alias string is greater than 31, alias string will be truncated and passed in.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkProductInfo_SetAlias(const char *productAlias);

#ifdef __cplusplus
}
#endif

#endif //PSDK_PRODUCT_INFO_H

/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
