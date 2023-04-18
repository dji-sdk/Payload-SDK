/**
 ********************************************************************
 * @file    dij_core.h
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
#ifndef DJI_CORE_H
#define DJI_CORE_H

/* Includes ------------------------------------------------------------------*/
#include "dji_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef struct {
    char appName[32]; /*!< Specifies DJI SDK app name. This info can be obtained by logging in to the
                           developer website https://developer.dji.com/user/apps/#all. End with '\0'. */
    char appId[16]; /*!< Specifies DJI SDK app ID. This info can be obtained by logging in to the
                         developer website https://developer.dji.com/user/apps/#all. */
    char appKey[32]; /*!< Specifies DJI SDK app key. This info can be obtained by logging in to the
                          developer website https://developer.dji.com/user/apps/#all. */
    char appLicense[512]; /*!< Specifies DJI SDK app license. This info can be obtained by logging in to the
                               developer website https://developer.dji.com/user/apps/#all. */
    char developerAccount[64]; /*!< Specifies DJI SDK developer account email. This info can be obtained by
                                    logging in to the developer website https://developer.dji.com/user/apps/#all.
                                    Developer's account and other related information need to be able to correspond.
                                    End with '\0'. */
    char baudRate[7]; /*!< Specifies DJI SDK communication serial port baud rate. */
} T_DjiUserInfo;

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
T_DjiReturnCode DjiCore_Init(const T_DjiUserInfo *userInfo);

/**
 * @brief Set an alias that satisfies the condition for DJI application or product.
 * @details Alias will display in DJI Pilot, if exist.
 * @note Still need to pass in correct DJI APP name that is obtained from DJI SDK developer website to DjiCore_Init()
 * interface. The DJI APP name will be used to bind or verification.
 * @note Alias will be effective after a while, and the max value is 1s.
 * @param productAlias: pointer to product alias string, and alias end with '\0'. The max length of the string is 31. If
 * length of alias string is greater than 31, alias string will be truncated and passed in.
 * @return Execution result.
 */
T_DjiReturnCode DjiCore_SetAlias(const char *productAlias);

/**
 * @brief Set custom firmware version for DJI application or product.
 * @details Payload firmware version will always display in DJI Pilot payload settings interface.
 * @param version: the custom firmware version to be set.
 * @return Execution result.
 */
T_DjiReturnCode DjiCore_SetFirmwareVersion(T_DjiFirmwareVersion version);

/**
 * @brief Set custom serial number for DJI application or product.
 * @details Payload custom serial number will always display in DJI Pilot payload settings interface.
 * @param productSerialNumber: the custom serial number to be set, needs to be no larger than 32 bytes.
 * @return Execution result.
 */
T_DjiReturnCode DjiCore_SetSerialNumber(const char *productSerialNumber);

/**
 * @brief Notify that the Payload SDK core application starts.
 * @note The call location of this interface requires special attention, The call needs to be completed after all the
 * module initialize and register interfaces.
 * @return Execution result.
 */
T_DjiReturnCode DjiCore_ApplicationStart(void);

/**
 * @brief DeInitialize the Payload SDK core in blocking mode.
 * @return Execution result.
 */
T_DjiReturnCode DjiCore_DeInit(void);

#ifdef __cplusplus
}
#endif

#endif // DIJ_CORE_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
