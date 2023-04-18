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
typedef enum {
    DJI_HMS_ERROR_LEVEL_NONE = 0,
    DJI_HMS_ERROR_LEVEL_HINT,
    DJI_HMS_ERROR_LEVEL_WARN,
    DJI_HMS_ERROR_LEVEL_CRITICAL,
    DJI_HMS_ERROR_LEVEL_FATAL,
} E_DjiHmsErrorLevel;

typedef struct {
    uint32_t errorCode;
    uint8_t componentIndex;
    uint8_t errorLevel;
} T_DjiHmsInfo;

typedef struct {
    T_DjiHmsInfo *hmsInfo;
    uint32_t hmsInfoNum;
} T_DjiHmsInfoTable;

typedef struct {
    char *fileName; /*!< The file name of the hms text config file */
    uint32_t fileSize; /*!< The file size of the hms text config file, uint : byte */
    const uint8_t *fileBinaryArray; /*!< The binary C array of the hms text config file */
} T_DjiHmsFileBinaryArray;

typedef struct {
    uint16_t binaryArrayCount; /*!< Binary array count. */
    T_DjiHmsFileBinaryArray *fileBinaryArrayList; /*!< Pointer to binary array list */
} T_DjiHmsBinaryArrayConfig;

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

/**
 * @brief Inject custom hms error code to APP.
 * @note: For the same hms error code, a single call is enough, no need to call repeatedly.
 * @param errorCode: hms error code, value range: [0x1E020000 ~ 0x1E02FFFF].
 * @param errorLevel: hms error level, see reference of E_DjiHmsErrorLevel.
 * @return Execution result.
 */
T_DjiReturnCode DjiHms_InjectHmsErrorCode(uint32_t errorCode, E_DjiHmsErrorLevel errorLevel);

/**
 * @brief Eliminate custom hms error code to APP.
 * @note: For the same hms error code, a single call is enough, no need to call repeatedly.
 * @param errorCode: hms error code, value range: [0x1E020000 ~ 0x1E02FFFF].
 * @return Execution result.
 */
T_DjiReturnCode DjiHms_EliminateHmsErrorCode(uint32_t errorCode);

/**
 * @brief Register default hms text configuration file by directory path.
 * @param configDirPath: the hms text configuration by directory path.
 * @return Execution result.
 */
T_DjiReturnCode DjiHms_RegDefaultHmsTextConfigByDirPath(const char *configDirPath);

/**
 * @brief Register hms text configuration file by directory path.
 * @note Different hms text configurations for several language require the same hms config.
 * @param appLanguage: mobile app language type.
 * @param configDirPath: the hms text configuration by directory path.
 * @return Execution result.
 */
T_DjiReturnCode DjiHms_RegHmsTextConfigByDirPath(E_DjiMobileAppLanguage appLanguage,
                                                 const char *configDirPath);

/**
 * @brief Register default hms text configuration config by binary array.
 * @note In RTOS, most likely there is no file system. The hms text config file content can use C array express. Use this
 * function and DjiHms_RegDefaultHmsTextConfigByBinaryArray set hms text configuration. When the language is not cover in
 * your setting by DjiHms_RegHmsTextConfigByBinaryArray, the hms text configuration uses setting by this function.
 * @param binaryArrayConfig: the binary array config for hms text configuration.
 * @return Execution result.
 */
T_DjiReturnCode DjiHms_RegDefaultHmsTextConfigByBinaryArray(const T_DjiHmsBinaryArrayConfig *binaryArrayConfig);

/**
 * @brief Register hms text config by binary array configuration.
 * @note Different hms text configurations for several language require the same hms config.
 * @param appLanguage: mobile app language type.
 * @param binaryArrayConfig: the binary array config for hms text configuration.
 * @return Execution result.
 */
T_DjiReturnCode DjiHms_RegHmsTextConfigByBinaryArray(E_DjiMobileAppLanguage appLanguage,
                                                     const T_DjiHmsBinaryArrayConfig *binaryArrayConfig);

#ifdef __cplusplus
}
#endif

#endif // DJI_HMS_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
