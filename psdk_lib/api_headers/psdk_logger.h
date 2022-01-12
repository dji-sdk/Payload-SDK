/**
 ********************************************************************
 * @file    psdk_logger.h
 * @brief   This is the header file for "psdk_logger.c", defining the structure and
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
#ifndef PSDK_LOGGER_H
#define PSDK_LOGGER_H

/* Includes ------------------------------------------------------------------*/
#include "psdk_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/**
 * @brief The console method that needs to be registered.
 * @note  Before registering the console method, you need to test the methods that need to be registered to ensure
 *        that they can be used normally.
 */
typedef T_PsdkReturnCode (*ConsoleFunc)(const uint8_t *data, uint16_t dataLen);

typedef enum {
    PSDK_LOGGER_CONSOLE_LOG_LEVEL_ERROR = 1, /*!< Logger console error level. The method and level of the console are
                                                  associated with each other. If the level of the registered console
                                                  method is lower than this level, the level interface will not be
                                                  printed successfully. */
    PSDK_LOGGER_CONSOLE_LOG_LEVEL_WARNING = 2, /*!< Logger console warning level.The method and level of the console are
                                                    associated with each other. If the level of the registered console
                                                    method is lower than this level, the level interface will not be
                                                    printed successfully. */
    PSDK_LOGGER_CONSOLE_LOG_LEVEL_INFO = 3, /*!< Logger console info level. The method and level of the console are
                                                 associated with each other. If the level of the registered console
                                                 method is lower than this level, the level interface will not be
                                                 printed successfully. */
    PSDK_LOGGER_CONSOLE_LOG_LEVEL_DEBUG = 4, /*!< Logger console debug level. The method and level of the console are
                                                  associated with each other. If the level of the registered console
                                                  method is lower than this level, the level interface will not be
                                                  printed successfully. */
} E_PsdkLoggerConsoleLogLevel;

typedef struct {
    uint8_t consoleLevel; /*!< Specifies logger console level. The method and level of the console are associated with
                               each other. Calling different levels of the print interface, you can print the log with
                               the method no higher than the level. For example, the registered method level is info,
                               only the log not higher than info can be printed, and the debug log will not be printed.*/
    ConsoleFunc func; /*!< Specifies logger console function. Before registering the console method, you need to
                           test the methods that need to be registered to ensure that they can be used normally.*/
} T_PsdkLoggerConsole;

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Add the console function and level for Payload SDK.
 * @note When registering the console, you need to provide the method of the console and the level corresponding to
 * the method. Log levels from high to low are Debug, Info, Warn, and Error, the log function module can print all 
 * logs not higher than the specified level. Maximum support for registering eight different console methods at the 
 * same time.Before registering the console method, you should test the registration method ensure that all the method
 * are normally. If you registered multiple methods at the same time, all the methods will be print.
 * @param console: pointer to the console function.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkLogger_AddConsole(T_PsdkLoggerConsole *console);

/**
 * @brief Print out the info log of the specified format by the registration method.
 * @note The registered method is printed according to the corresponding level. If the level of the console is lower
 * than the level at which the log needs to be printed, it will not be printed successfully.
 * @param fmt: pointer to the format string that needs print out.
 * @param ...: Variable parameters, consistent with the use of the system interface print out.
 */
void PsdkLogger_UserLogInfo(const char *fmt, ...);

/**
 * @brief Print out the warning log of the specified format by the registration method.
 * @note The registered method is printed according to the corresponding level. If the level of the console is lower
 * than the level at which the log needs to be printed, it will not be printed successfully.
 * @param fmt: pointer to the format string that needs print out.
 * @param ...: Variable parameters, consistent with the use of the system interface print out.
 */
void PsdkLogger_UserLogWarn(const char *fmt, ...);

/**
 * @brief Print out the debug log of the specified format by the registration method.
 * @note The registered method is printed according to the corresponding level. If the level of the console is lower
 * than the level at which the log needs to be printed, it will not be printed successfully.
 * @param fmt: pointer to the format string that needs print out.
 * @param ...: Variable parameters, consistent with the use of the system interface print out.
 */
void PsdkLogger_UserLogDebug(const char *fmt, ...);

/**
 * @brief Print out the error log of the specified format by the registration method.
 * @note The registered method is printed according to the corresponding level. If the level of the console is lower
 * than the level at which the log needs to be printed, it will not be printed successfully.
 * @param fmt: pointer to the format string that needs print out.
 * @param ...: Variable parameters, consistent with the use of the system interface print out.
 */
void PsdkLogger_UserLogError(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif // PSDK_LOGGER_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
