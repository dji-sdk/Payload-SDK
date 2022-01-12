/**
 ********************************************************************
 * @file    test_widget.c
 * @version V2.0.0
 * @date    2019/07/01
 * @brief
 *
 * @copyright (c) 2018-2019 DJI. All rights reserved.
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

/* Includes ------------------------------------------------------------------*/
#include "test_widget.h"
#include <psdk_widget.h>
#include <psdk_logger.h>
#include <utils/util_misc.h>
#include <psdk_platform.h>
#include <stdio.h>

#if !PSDK_ARCH_SYS_LINUX

#include "file_binary_array_list_en.h"

#endif

/* Private constants ---------------------------------------------------------*/
#define WIDGET_DIR_PATH_LEN_MAX         (256)
#define WIDGET_TASK_STACK_SIZE          (2048)

/* Private types -------------------------------------------------------------*/

/* Private functions declaration ---------------------------------------------*/
static void *PsdkTest_WidgetTask(void *arg);
static T_PsdkReturnCode PsdkTestWidget_SetWidgetValue(E_PsdkWidgetType widgetType, uint32_t index, int32_t value,
                                                      void *userData);
static T_PsdkReturnCode PsdkTestWidget_GetWidgetValue(E_PsdkWidgetType widgetType, uint32_t index, int32_t *value,
                                                      void *userData);

/* Private values ------------------------------------------------------------*/
static T_PsdkTaskHandle s_widgetTestThread;
#if PSDK_ARCH_SYS_LINUX
static bool s_isWidgetFileDirPathConfigured = false;
static char s_widgetFileDirPath[PSDK_FILE_PATH_SIZE_MAX] = {0};
#endif

static const T_PsdkWidgetHandlerListItem s_widgetHandlerList[] = {
    {0, PSDK_WIDGET_TYPE_BUTTON,        PsdkTestWidget_SetWidgetValue, PsdkTestWidget_GetWidgetValue, NULL},
    {1, PSDK_WIDGET_TYPE_BUTTON,        PsdkTestWidget_SetWidgetValue, PsdkTestWidget_GetWidgetValue, NULL},
    {2, PSDK_WIDGET_TYPE_LIST,          PsdkTestWidget_SetWidgetValue, PsdkTestWidget_GetWidgetValue, NULL},
    {3, PSDK_WIDGET_TYPE_SWITCH,        PsdkTestWidget_SetWidgetValue, PsdkTestWidget_GetWidgetValue, NULL},
    {4, PSDK_WIDGET_TYPE_SCALE,         PsdkTestWidget_SetWidgetValue, PsdkTestWidget_GetWidgetValue, NULL},
    {5, PSDK_WIDGET_TYPE_BUTTON,        PsdkTestWidget_SetWidgetValue, PsdkTestWidget_GetWidgetValue, NULL},
    {6, PSDK_WIDGET_TYPE_SCALE,         PsdkTestWidget_SetWidgetValue, PsdkTestWidget_GetWidgetValue, NULL},
    {7, PSDK_WIDGET_TYPE_INT_INPUT_BOX, PsdkTestWidget_SetWidgetValue, PsdkTestWidget_GetWidgetValue, NULL},
    {8, PSDK_WIDGET_TYPE_SWITCH,        PsdkTestWidget_SetWidgetValue, PsdkTestWidget_GetWidgetValue, NULL},
    {9, PSDK_WIDGET_TYPE_LIST,          PsdkTestWidget_SetWidgetValue, PsdkTestWidget_GetWidgetValue, NULL},
};

static char *s_widgetTypeNameArray[] = {
    "Unknown",
    "Button",
    "Switch",
    "Scale",
    "List",
    "Int input box"
};

static const uint32_t s_widgetHandlerListCount = sizeof(s_widgetHandlerList) / sizeof(T_PsdkWidgetHandlerListItem);
static int32_t s_widgetValueList[sizeof(s_widgetHandlerList) / sizeof(T_PsdkWidgetHandlerListItem)] = {0};

/* Exported functions definition ---------------------------------------------*/
T_PsdkReturnCode PsdkTest_WidgetInit(void)
{
    T_PsdkReturnCode psdkStat;

    //Step 1 : Init PSDK Widget
    psdkStat = PsdkWidget_Init();
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Psdk test widget init error, stat = 0x%08llX", psdkStat);
        return psdkStat;
    }

#if PSDK_ARCH_SYS_LINUX
    //Step 2 : Set UI Config (Linux environment)

    char curFileDirPath[WIDGET_DIR_PATH_LEN_MAX];
    char tempPath[WIDGET_DIR_PATH_LEN_MAX];
    psdkStat = PsdkUserUtil_GetCurrentFileDirPath(__FILE__, WIDGET_DIR_PATH_LEN_MAX, curFileDirPath);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Get file current path error, stat = 0x%08llX", psdkStat);
        return psdkStat;
    }

    if (s_isWidgetFileDirPathConfigured == true) {
        snprintf(tempPath, WIDGET_DIR_PATH_LEN_MAX, "%swidget_file/en_big_screen", s_widgetFileDirPath);
    } else {
        snprintf(tempPath, WIDGET_DIR_PATH_LEN_MAX, "%swidget_file/en_big_screen", curFileDirPath);
    }

    //set default ui config path
    psdkStat = PsdkWidget_RegDefaultUiConfigByDirPath(tempPath);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Add default widget ui config error, stat = 0x%08llX", psdkStat);
        return psdkStat;
    }

    //set ui config for English language
    psdkStat = PsdkWidget_RegUiConfigByDirPath(PSDK_AIRCRAFT_INFO_MOBILE_APP_LANGUAGE_ENGLISH,
                                               PSDK_AIRCRAFT_INFO_MOBILE_APP_SCREEN_TYPE_BIG_SCREEN,
                                               tempPath);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Add widget ui config error, stat = 0x%08llX", psdkStat);
        return psdkStat;
    }

    //set ui config for Chinese language
    if (s_isWidgetFileDirPathConfigured == true) {
        snprintf(tempPath, WIDGET_DIR_PATH_LEN_MAX, "%swidget_file/cn_big_screen", s_widgetFileDirPath);
    } else {
        snprintf(tempPath, WIDGET_DIR_PATH_LEN_MAX, "%swidget_file/cn_big_screen", curFileDirPath);
    }

    psdkStat = PsdkWidget_RegUiConfigByDirPath(PSDK_AIRCRAFT_INFO_MOBILE_APP_LANGUAGE_CHINESE,
                                               PSDK_AIRCRAFT_INFO_MOBILE_APP_SCREEN_TYPE_BIG_SCREEN,
                                               tempPath);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Add widget ui config error, stat = 0x%08llX", psdkStat);
        return psdkStat;
    }
#else
    //Step 2 : Set UI Config (RTOS environment)

    T_PsdkWidgetBinaryArrayConfig enWidgetBinaryArrayConfig = {
        .binaryArrayCount = g_EnBinaryArrayCount,
        .fileBinaryArrayList = g_EnFileBinaryArrayList
    };

    //set default ui config
    psdkStat = PsdkWidget_RegDefaultUiConfigByBinaryArray(&enWidgetBinaryArrayConfig);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Add default widget ui config error, stat = 0x%08llX", psdkStat);
        return psdkStat;
    }

#endif

    //Step 3 : Set widget handler list
    psdkStat = PsdkWidget_RegHandlerList(s_widgetHandlerList, s_widgetHandlerListCount);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Set widget handler list error, stat = 0x%08llX", psdkStat);
        return psdkStat;
    }

    //Step 4 : Run widget api sample task
    if (PsdkOsal_TaskCreate(&s_widgetTestThread, PsdkTest_WidgetTask, "user_widget_task", WIDGET_TASK_STACK_SIZE,
                            NULL) != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("Psdk widget test task create error.");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

#if PSDK_ARCH_SYS_LINUX
T_PsdkReturnCode PsdkTest_WidgetSetConfigFilePath(const char *path)
{
    memset(s_widgetFileDirPath, 0, sizeof(s_widgetFileDirPath));
    memcpy(s_widgetFileDirPath, path, USER_UTIL_MIN(strlen(path), sizeof(s_widgetFileDirPath) - 1));
    s_isWidgetFileDirPathConfigured = true;

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}
#endif

#ifndef __CC_ARM
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wreturn-type"
#pragma GCC diagnostic ignored "-Wformat"
#endif

/* Private functions definition-----------------------------------------------*/
static void *PsdkTest_WidgetTask(void *arg)
{
    char message[PSDK_WIDGET_FLOATING_WINDOW_MSG_MAX_LEN];
    uint32_t sysTimeMs = 0;
    T_PsdkReturnCode psdkStat;

    USER_UTIL_UNUSED(arg);

    while (1) {
        psdkStat = PsdkOsal_GetTimeMs(&sysTimeMs);
        if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            PsdkLogger_UserLogError("Get system time ms error, stat = 0x%08llX", psdkStat);
        }
        snprintf(message, PSDK_WIDGET_FLOATING_WINDOW_MSG_MAX_LEN, "System time : %u ms", sysTimeMs);

        psdkStat = PsdkWidgetFloatingWindow_ShowMessage(message);
        if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            PsdkLogger_UserLogError("Floating window show message error, stat = 0x%08llX", psdkStat);
        }

        PsdkOsal_TaskSleepMs(1000);
    }
}

#ifndef __CC_ARM
#pragma GCC diagnostic pop
#endif

static T_PsdkReturnCode PsdkTestWidget_SetWidgetValue(E_PsdkWidgetType widgetType, uint32_t index, int32_t value,
                                                      void *userData)
{
    USER_UTIL_UNUSED(userData);

    PsdkLogger_UserLogInfo("Set widget value, widgetType = %s, widgetIndex = %d ,widgetValue = %d",
                           s_widgetTypeNameArray[widgetType], index, value);
    s_widgetValueList[index] = value;

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_PsdkReturnCode PsdkTestWidget_GetWidgetValue(E_PsdkWidgetType widgetType, uint32_t index, int32_t *value,
                                                      void *userData)
{
    USER_UTIL_UNUSED(userData);
    USER_UTIL_UNUSED(widgetType);

    *value = s_widgetValueList[index];

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
