/**
 ********************************************************************
 * @file    dji_hms_customization.h
 * @brief   This is the header file for "dji_hms_customization.c", defining the structure and
 * (exported) function prototypes.
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
#ifndef DJI_HMS_CUSTOMIZATION_H
#define DJI_HMS_CUSTOMIZATION_H

/* Includes ------------------------------------------------------------------*/

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

typedef enum {
    DJI_HMS_ALARM_ENHANCED_TYPE_SHAKE_MOTOR = 1, /*!< Alarm triggered by shaking the motor on the pilot; alerts the user with vibrations indicating a warning or fault. */
    DJI_HMS_ALARM_ENHANCED_PLAY_SOUND = 2, /*!< Alarm triggered by playing sound */
    DJI_HMS_ALARM_ENHANCED_PLAY_SOUND_AND_SHAKE_MOTOR =3, /*!< Alarm that alerts the user on the PILOT app with both sound and vibrations */
} E_DjiHmsAlarmEnhancedType;

typedef enum {
    DJI_HMS_ALARM_ENHANCED_ACTION_STOP = 0, /*!< Action to stop a specific enhanced alarm, shake motor or play sound */
    DJI_HMS_ALARM_ENHANCED_ACTION_START = 1, /*!< Action to stop a specific enhanced alarm */
    DJI_HMS_ALARM_ENHANCED_ACTION_EXIT_ALL =2,/*!< Action to exit all enhanced alarms, both shake motor and play sound; */
} E_DjiHmsAlarmEnhancedAction;

typedef struct {
    char *fileName; /*!< The file name of the hms text config file */
    uint32_t fileSize; /*!< The file size of the hms text config file, uint : byte */
    const uint8_t *fileBinaryArray; /*!< The binary C array of the hms text config file */
} T_DjiHmsFileBinaryArray;

typedef struct {
    uint16_t binaryArrayCount; /*!< Binary array count. */
    T_DjiHmsFileBinaryArray *fileBinaryArrayList; /*!< Pointer to binary array list */
} T_DjiHmsBinaryArrayConfig;

typedef struct {
    E_DjiHmsAlarmEnhancedType type; /*!< The type is used to specify which enhanced alarm to ACTION_STOP or ACTION_START. It is ignored when EXIT_ALL alarms.*/
    int8_t times; /* !< Specifies the number of consecutive times the alarm is to be activated.*/
    int16_t interval; /* !< Indicates the interval (in milliseconds) between consecutive activations of the alarm.*/
} T_DjiHmsAlarmEnhancedSetting;

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Initialise hms customization module, and user should call this function
 * before using hms customization features.
 * @return Execution result.
 */
T_DjiReturnCode DjiHmsCustomization_Init(void);

/**
 * @brief DeInitialize hms customization module.
 * @return Execution result.
 */
T_DjiReturnCode DjiHmsCustomization_DeInit(void);

/**
 * @brief Inject custom hms error code to APP.
 * @note: For the same hms error code, a single call is enough, no need to call repeatedly.
 * @param errorCode: hms error code, value range: [0x1E020000 ~ 0x1E02FFFF].
 * @param errorLevel: hms error level, see reference of E_DjiHmsErrorLevel.
 * @return Execution result.
 */
T_DjiReturnCode DjiHmsCustomization_InjectHmsErrorCode(uint32_t errorCode, E_DjiHmsErrorLevel errorLevel);

/**
 * @brief Eliminate custom hms error code to APP.
 * @note: For the same hms error code, a single call is enough, no need to call repeatedly.
 * @param errorCode: hms error code, value range: [0x1E020000 ~ 0x1E02FFFF].
 * @return Execution result.
 */
T_DjiReturnCode DjiHmsCustomization_EliminateHmsErrorCode(uint32_t errorCode);

/**
 * @brief Register default hms text configuration file by directory path.
 * @param configDirPath: the hms text configuration by directory path.
 * @return Execution result.
 */
T_DjiReturnCode DjiHmsCustomization_RegDefaultHmsTextConfigByDirPath(const char *configDirPath);

/**
 * @brief Register hms text configuration file by directory path.
 * @note Different hms text configurations for several language require the same hms config.
 * @param appLanguage: mobile app language type.
 * @param configDirPath: the hms text configuration by directory path.
 * @return Execution result.
 */
T_DjiReturnCode DjiHmsCustomization_RegHmsTextConfigByDirPath(E_DjiMobileAppLanguage appLanguage,
                                                              const char *configDirPath);

/**
 * @brief Register default hms text configuration config by binary array.
 * @note In RTOS, most likely there is no file system. The hms text config file content can use C array express. Use this
 * function and DjiHms_RegDefaultHmsTextConfigByBinaryArray set hms text configuration. When the language is not cover in
 * your setting by DjiHms_RegHmsTextConfigByBinaryArray, the hms text configuration uses setting by this function.
 * @param binaryArrayConfig: the binary array config for hms text configuration.
 * @return Execution result.
 */
T_DjiReturnCode DjiHmsCustomization_RegDefaultHmsTextConfigByBinaryArray(const T_DjiHmsBinaryArrayConfig
                                                                         *binaryArrayConfig);

/**
 * @brief Register hms text config by binary array configuration.
 * @note Different hms text configurations for several language require the same hms config.
 * @param appLanguage: mobile app language type.
 * @param binaryArrayConfig: the binary array config for hms text configuration.
 * @return Execution result.
 */
T_DjiReturnCode DjiHmsCustomization_RegHmsTextConfigByBinaryArray(E_DjiMobileAppLanguage appLanguage,
                                                                  const T_DjiHmsBinaryArrayConfig *binaryArrayConfig);

/**
 * @brief Contrl the app alram.
 * @note This interface support on DJI manifold3.
 * @param action: The action to start or stop alarm.
 * @param setting: The alarm information.
 * @return Execution result.
 */
T_DjiReturnCode DjiHmsCustomization_AlarmEnhancedCtrl(E_DjiHmsAlarmEnhancedAction action, T_DjiHmsAlarmEnhancedSetting setting);

#ifdef __cplusplus
}
#endif

#endif // DJI_HMS_CUSTOMIZATION_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
