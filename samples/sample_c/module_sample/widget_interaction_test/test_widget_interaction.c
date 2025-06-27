/**
 ********************************************************************
 * @file    test_widget_interaction.c
 * @brief
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

/* Includes ------------------------------------------------------------------*/
#include "test_widget_interaction.h"
#include <dji_widget.h>
#include <dji_logger.h>
#include "../utils/util_misc.h"
#include <dji_platform.h>
#include <stdio.h>
#include <waypoint_v2/test_waypoint_v2.h>
#include <hms/test_hms.h>
#include <gimbal_manager/test_gimbal_manager.h>
#include <camera_manager/test_camera_manager.h>
#include <flight_control/test_flight_control.h>
#include <fc_subscription/test_fc_subscription.h>
#include <dji_perception.h>
#include "liveview/test_liveview.h"
#include "perception/test_perception.h"
#include "file_binary_array_list_en.h"
#include <stdarg.h>
#include "dji_aircraft_info.h"
#include "dji_core.h"
#include <payload_collaboration/test_payload_collaboration.h>
#include <waypoint_v3/test_waypoint_v3.h>
#include "dji_sdk_config.h"
#include "hms/hms_text_c/en/hms_text_config_json.h"
#include "dji_hms.h"

/* Private constants ---------------------------------------------------------*/
#define WIDGET_DIR_PATH_LEN_MAX         (256)
#define WIDGET_TASK_STACK_SIZE          (2048)

#define WIDGET_LOG_STRING_MAX_SIZE      (40)
#define WIDGET_LOG_LINE_MAX_NUM         (5)

#define DJI_HMS_ERROR_CODE_VALUE0    0x1E020000
#define DJI_HMS_ERROR_CODE_VALUE1    0x1E020001
#define DJI_HMS_ERROR_CODE_VALUE2    0x1E020002
#define DJI_HMS_ERROR_CODE_VALUE3    0x1E020003
#define DJI_HMS_ERROR_CODE_VALUE4    0x1E020004


/* Private types -------------------------------------------------------------*/
typedef enum {
    E_DJI_SAMPLE_INDEX_FC_SUBSCRIPTION = 0,
    E_DJI_SAMPLE_INDEX_WAYPOINT_V2 = 1,
    E_DJI_SAMPLE_INDEX_WAYPOINT_V3 = 2,
    E_DJI_SAMPLE_INDEX_FLIGHT_CONTROL_TAKE_OFF_LANDING = 3,
    E_DJI_SAMPLE_INDEX_FLIGHT_CONTROL_TAKE_OFF_POSITION_CTRL_LANDING = 4,
    E_DJI_SAMPLE_INDEX_FLIGHT_CONTROL_TAKE_OFF_GO_HOME_FORCE_LANDING = 5,
    E_DJI_SAMPLE_INDEX_FLIGHT_CONTROL_TAKE_OFF_VELOCITY_CTRL_LANDING = 6,
    E_DJI_SAMPLE_INDEX_FLIGHT_CONTROL_ARREST_FLYING = 7,
    E_DJI_SAMPLE_INDEX_FLIGHT_CONTROL_SET_GET_PARAM = 8,
    E_DJI_SAMPLE_INDEX_HMS_MANAGER = 9,
    E_DJI_SAMPLE_INDEX_GIMBAL_MANAGER_FREE_MODE = 10,
    E_DJI_SAMPLE_INDEX_GIMBAL_MANAGER_YAW_FOLLOW_MODE = 11,
    E_DJI_SAMPLE_INDEX_LIVEVIEW = 12,
    E_DJI_SAMPLE_INDEX_PERCEPTION = 13,
    E_DJI_SAMPLE_INDEX_SWITCH_ALIAS = 14,
    E_DJI_SAMPLE_INDEX_CAMMGR_SHUTTER_SPEED = 15,
    E_DJI_SAMPLE_INDEX_CAMMGR_APERTURE = 16,
    E_DJI_SAMPLE_INDEX_CAMMGR_EV = 17,
    E_DJI_SAMPLE_INDEX_CAMMGR_ISO = 18,
    E_DJI_SAMPLE_INDEX_CAMMGR_FOCUS_POINT = 19,
    E_DJI_SAMPLE_INDEX_CAMMGR_TAP_ZOOM = 20,
    E_DJI_SAMPLE_INDEX_CAMMGR_OPTICAL_ZOOM = 21,
    E_DJI_SAMPLE_INDEX_CAMMGR_SINGLE_PHOTO = 22,
    E_DJI_SAMPLE_INDEX_CAMMGR_BURST_PHOTO = 23,
    E_DJI_SAMPLE_INDEX_CAMMGR_INTERVAL_PHOTO = 24,
    E_DJI_SAMPLE_INDEX_CAMMGR_RECORDER_VIDEO = 25,
    E_DJI_SAMPLE_INDEX_CAMMGR_MEDIA_DOWNLOAD = 26,
    E_DJI_SAMPLE_INDEX_CAMMGR_THERMOMETRY = 27,
    E_DJI_SAMPLE_INDEX_UNKNOWN = 0xFF,
} E_DjiExtensionPortSampleIndex;

typedef enum {
    E_DJI_HMS_ERROR_CODE_INDEX1 = 0,
    E_DJI_HMS_ERROR_CODE_INDEX2,
    E_DJI_HMS_ERROR_CODE_INDEX3,
    E_DJI_HMS_ERROR_CODE_INDEX4,
    E_DJI_HMS_ERROR_CODE_INDEX5,
} E_DjiExtensionPortHmsErrorCodeIndex;

typedef enum {
    E_DJI_HMS_ERROR_LEVEL_INDEX1 = 0,
    E_DJI_HMS_ERROR_LEVEL_INDEX2,
    E_DJI_HMS_ERROR_LEVEL_INDEX3,
    E_DJI_HMS_ERROR_LEVEL_INDEX4,
    E_DJI_HMS_ERROR_LEVEL_INDEX5,
} E_DjiExtensionPortHmsErrorLevelIndex;

typedef struct {
    bool valid;
    char content[WIDGET_LOG_STRING_MAX_SIZE];
} T_DjiTestWidgetLog;

/* Private functions declaration ---------------------------------------------*/
static void *DjiTest_WidgetTask(void *arg);
static void *DjiTest_WidgetInteractionTask(void *arg);
static T_DjiReturnCode DjiTestWidget_SetWidgetValue(E_DjiWidgetType widgetType, uint32_t index, int32_t value,
                                                    void *userData);
static T_DjiReturnCode DjiTestWidget_GetWidgetValue(E_DjiWidgetType widgetType, uint32_t index, int32_t *value,
                                                    void *userData);
static T_DjiReturnCode DjiTestWidget_TriggerChangeAlias(void);

/* Private values ------------------------------------------------------------*/
static T_DjiTaskHandle s_widgetTestThread;
static T_DjiTaskHandle s_widgetInteractionTestThread;
static E_DjiExtensionPortSampleIndex s_extensionPortSampleIndex = E_DJI_SAMPLE_INDEX_FC_SUBSCRIPTION;
static E_DjiExtensionPortHmsErrorCodeIndex s_extensionPortErrcodeIndex = E_DJI_HMS_ERROR_CODE_INDEX1;
static E_DjiExtensionPortHmsErrorLevelIndex s_extensionPortErrLevelIndex = E_DJI_HMS_ERROR_LEVEL_INDEX1;
static bool s_isInjectErrcode = false;
static bool s_isEliminateErrcode = false;
static bool s_isallowRunFlightControlSample = false;
static bool s_isSampleStart = false;
static E_DjiMountPosition s_mountPosition = DJI_MOUNT_POSITION_PAYLOAD_PORT_NO1;
static T_DjiTestWidgetLog s_djiTestWidgetLog[WIDGET_LOG_LINE_MAX_NUM] = {0};
static T_DjiAircraftInfoBaseInfo s_aircraftInfoBaseInfo = {0};
static bool s_isAliasChanged = false;

static const T_DjiWidgetHandlerListItem s_widgetHandlerList[] = {
    {0,  DJI_WIDGET_TYPE_BUTTON,        DjiTestWidget_SetWidgetValue, DjiTestWidget_GetWidgetValue, NULL},
    {1,  DJI_WIDGET_TYPE_LIST,          DjiTestWidget_SetWidgetValue, DjiTestWidget_GetWidgetValue, NULL},
    {2,  DJI_WIDGET_TYPE_SWITCH,        DjiTestWidget_SetWidgetValue, DjiTestWidget_GetWidgetValue, NULL},
    {3,  DJI_WIDGET_TYPE_SCALE,         DjiTestWidget_SetWidgetValue, DjiTestWidget_GetWidgetValue, NULL},
    {4,  DJI_WIDGET_TYPE_BUTTON,        DjiTestWidget_SetWidgetValue, DjiTestWidget_GetWidgetValue, NULL},
    {5,  DJI_WIDGET_TYPE_SCALE,         DjiTestWidget_SetWidgetValue, DjiTestWidget_GetWidgetValue, NULL},
    {6,  DJI_WIDGET_TYPE_INT_INPUT_BOX, DjiTestWidget_SetWidgetValue, DjiTestWidget_GetWidgetValue, NULL},
    {7,  DJI_WIDGET_TYPE_SWITCH,        DjiTestWidget_SetWidgetValue, DjiTestWidget_GetWidgetValue, NULL},
    {8,  DJI_WIDGET_TYPE_LIST,          DjiTestWidget_SetWidgetValue, DjiTestWidget_GetWidgetValue, NULL},
    {9,  DJI_WIDGET_TYPE_LIST,          DjiTestWidget_SetWidgetValue, DjiTestWidget_GetWidgetValue, NULL},
    {10, DJI_WIDGET_TYPE_BUTTON,        DjiTestWidget_SetWidgetValue, DjiTestWidget_GetWidgetValue, NULL},
    {11, DJI_WIDGET_TYPE_LIST,          DjiTestWidget_SetWidgetValue, DjiTestWidget_GetWidgetValue, NULL},
    {12, DJI_WIDGET_TYPE_LIST,          DjiTestWidget_SetWidgetValue, DjiTestWidget_GetWidgetValue, NULL},
    {13, DJI_WIDGET_TYPE_BUTTON,        DjiTestWidget_SetWidgetValue, DjiTestWidget_GetWidgetValue, NULL},
    {14, DJI_WIDGET_TYPE_BUTTON,        DjiTestWidget_SetWidgetValue, DjiTestWidget_GetWidgetValue, NULL},
};

static const char *s_widgetTypeNameArray[] = {
    "Unknown",
    "Button",
    "Switch",
    "Scale",
    "List",
    "Int input box"
};

static const uint32_t s_widgetHandlerListCount = sizeof(s_widgetHandlerList) / sizeof(T_DjiWidgetHandlerListItem);
static int32_t s_widgetValueList[sizeof(s_widgetHandlerList) / sizeof(T_DjiWidgetHandlerListItem)] = {0};
static bool s_isWidgetFileDirPathConfigured = false;
static char s_widgetFileDirPath[DJI_FILE_PATH_SIZE_MAX] = {0};

/* Exported functions definition ---------------------------------------------*/
void DjiTest_WidgetLogAppend(const char *fmt, ...)
{
    va_list args;
    char string[WIDGET_LOG_STRING_MAX_SIZE];
    int32_t i;

    va_start(args, fmt);
    vsnprintf(string, WIDGET_LOG_STRING_MAX_SIZE, fmt, args);
    va_end(args);

    for (i = 0; i < WIDGET_LOG_LINE_MAX_NUM; ++i) {
        if (s_djiTestWidgetLog[i].valid == false) {
            s_djiTestWidgetLog[i].valid = true;
            strcpy(s_djiTestWidgetLog[i].content, string);
            break;
        }
    }

    if (i == WIDGET_LOG_LINE_MAX_NUM) {
        for (i = 0; i < WIDGET_LOG_LINE_MAX_NUM - 1; i++) {
            strcpy(s_djiTestWidgetLog[i].content, s_djiTestWidgetLog[i + 1].content);
        }
        strcpy(s_djiTestWidgetLog[WIDGET_LOG_LINE_MAX_NUM - 1].content, string);
    }
}

T_DjiReturnCode DjiTest_WidgetInteractionStartService(void)
{
    T_DjiReturnCode djiStat;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    //Step 1 : Init DJI Widget
    djiStat = DjiWidget_Init();
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Dji test widget init error, stat = 0x%08llX", djiStat);
        return djiStat;
    }

#ifdef SYSTEM_ARCH_LINUX_DISABLE
    //Step 2 : Set UI Config (Linux environment)
    char curFileDirPath[WIDGET_DIR_PATH_LEN_MAX];
    char tempPath[WIDGET_DIR_PATH_LEN_MAX];

    djiStat = DjiUserUtil_GetCurrentFileDirPath(__FILE__, WIDGET_DIR_PATH_LEN_MAX, curFileDirPath);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Get file current path error, stat = 0x%08llX", djiStat);
        return djiStat;
    }

    if (s_isWidgetFileDirPathConfigured == true) {
        snprintf(tempPath, WIDGET_DIR_PATH_LEN_MAX, "%swidget_file/en_big_screen", s_widgetFileDirPath);
    } else {
        snprintf(tempPath, WIDGET_DIR_PATH_LEN_MAX, "%swidget_file/en_big_screen", curFileDirPath);
    }

    USER_LOG_ERROR("Dji test widget set path");
    //set default ui config path
    USER_LOG_INFO("widget file: %s", tempPath);
    djiStat = DjiWidget_RegDefaultUiConfigByDirPath(tempPath);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Add default widget ui config error, stat = 0x%08llX", djiStat);
        return djiStat;
    }

    //set ui config for English language
    djiStat = DjiWidget_RegUiConfigByDirPath(DJI_MOBILE_APP_LANGUAGE_ENGLISH,
                                             DJI_MOBILE_APP_SCREEN_TYPE_BIG_SCREEN,
                                             tempPath);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Add widget ui config error, stat = 0x%08llX", djiStat);
        return djiStat;
    }

    //set ui config for Chinese language
    if (s_isWidgetFileDirPathConfigured == true) {
        snprintf(tempPath, WIDGET_DIR_PATH_LEN_MAX, "%swidget_file/cn_big_screen", s_widgetFileDirPath);
    } else {
        snprintf(tempPath, WIDGET_DIR_PATH_LEN_MAX, "%swidget_file/cn_big_screen", curFileDirPath);
    }

    djiStat = DjiWidget_RegUiConfigByDirPath(DJI_MOBILE_APP_LANGUAGE_CHINESE,
                                             DJI_MOBILE_APP_SCREEN_TYPE_BIG_SCREEN,
                                             tempPath);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Add widget ui config error, stat = 0x%08llX", djiStat);
        return djiStat;
    }
#else
    //Step 2 : Set UI Config (RTOS environment)
    T_DjiWidgetBinaryArrayConfig enWidgetBinaryArrayConfig = {
        .binaryArrayCount = g_testEnBinaryArrayCount,
        .fileBinaryArrayList = g_testEnFileBinaryArrayList
    };

    //set default ui config
    djiStat = DjiWidget_RegDefaultUiConfigByBinaryArray(&enWidgetBinaryArrayConfig);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Add default widget ui config error, stat = 0x%08llX", djiStat);
        return djiStat;
    }
#endif
    //Step 3 : Set widget handler list
    djiStat = DjiWidget_RegHandlerList(s_widgetHandlerList, s_widgetHandlerListCount);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Set widget handler list error, stat = 0x%08llX", djiStat);
        return djiStat;
    }

    //Step 4 : Run widget api sample task
    if (osalHandler->TaskCreate("user_widget_task", DjiTest_WidgetTask, WIDGET_TASK_STACK_SIZE, NULL,
                                &s_widgetTestThread) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Dji widget test task create error.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    if (osalHandler->TaskCreate("user_widget_task", DjiTest_WidgetInteractionTask, WIDGET_TASK_STACK_SIZE, NULL,
                                &s_widgetInteractionTestThread) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Dji widget test task create error.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiTest_WidgetInteractionSetConfigFilePath(const char *path)
{
    memset(s_widgetFileDirPath, 0, sizeof(s_widgetFileDirPath));
    memcpy(s_widgetFileDirPath, path, USER_UTIL_MIN(strlen(path), sizeof(s_widgetFileDirPath) - 1));
    s_isWidgetFileDirPathConfigured = true;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

#ifndef __CC_ARM
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wreturn-type"
#pragma GCC diagnostic ignored "-Wformat"
#endif

/* Private functions definition-----------------------------------------------*/
static void *DjiTest_WidgetTask(void *arg)
{
    char message[DJI_WIDGET_FLOATING_WINDOW_MSG_MAX_LEN];
    uint32_t sysTimeMs = 0;
    T_DjiReturnCode djiStat;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    USER_UTIL_UNUSED(arg);

    while (1) {
        djiStat = osalHandler->GetTimeMs(&sysTimeMs);
        if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Get system time ms error, stat = 0x%08llX", djiStat);
        }
#ifndef USER_FIRMWARE_MAJOR_VERSION
        snprintf(message, DJI_WIDGET_FLOATING_WINDOW_MSG_MAX_LEN,
                 "System time : %u ms \r\n%s \r\n%s \r\n%s \r\n%s \r\n%s \r\n",
                 sysTimeMs,
                 s_djiTestWidgetLog[0].content,
                 s_djiTestWidgetLog[1].content,
                 s_djiTestWidgetLog[2].content,
                 s_djiTestWidgetLog[3].content,
                 s_djiTestWidgetLog[4].content);
#else
        snprintf(message, DJI_WIDGET_FLOATING_WINDOW_MSG_MAX_LEN,
                 "System time : %u ms\r\nVersion: v%02d.%02d.%02d.%02d\r\nBuild time: %s %s", sysTimeMs,
                 USER_FIRMWARE_MAJOR_VERSION, USER_FIRMWARE_MINOR_VERSION,
                 USER_FIRMWARE_MODIFY_VERSION, USER_FIRMWARE_DEBUG_VERSION,
                 __DATE__, __TIME__);
#endif

        djiStat = DjiWidgetFloatingWindow_ShowMessage(message);
        if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Floating window show message error, stat = 0x%08llX", djiStat);
        }

        osalHandler->TaskSleepMs(200);
    }
}

#ifndef __CC_ARM
#pragma GCC diagnostic pop
#endif

#ifndef __CC_ARM
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wreturn-type"
#pragma GCC diagnostic ignored "-Wformat"
#endif

static void *DjiTest_WidgetInteractionTask(void *arg)
{
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    T_DjiReturnCode returnCode;
    uint32_t errorCode;
    E_DjiHmsErrorLevel errorLevel;

    returnCode = DjiAircraftInfo_GetBaseInfo(&s_aircraftInfoBaseInfo);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("get aircraft base info error");
        return NULL;
    }

    while (1) {
        osalHandler->TaskSleepMs(100);

        if (s_isInjectErrcode == true && s_isEliminateErrcode == false) {
            switch (s_extensionPortErrcodeIndex) {
                case E_DJI_HMS_ERROR_CODE_INDEX1:
                    errorCode = DJI_HMS_ERROR_CODE_VALUE0;
                    break;
                case E_DJI_HMS_ERROR_CODE_INDEX2:
                    errorCode = DJI_HMS_ERROR_CODE_VALUE1;
                    break;
                case E_DJI_HMS_ERROR_CODE_INDEX3:
                    errorCode = DJI_HMS_ERROR_CODE_VALUE2;
                    break;
                case E_DJI_HMS_ERROR_CODE_INDEX4:
                    errorCode = DJI_HMS_ERROR_CODE_VALUE3;
                    break;
                case E_DJI_HMS_ERROR_CODE_INDEX5:
                    errorCode = DJI_HMS_ERROR_CODE_VALUE4;
                    break;
                default:
                    break;
            }
            switch (s_extensionPortErrLevelIndex) {
                case E_DJI_HMS_ERROR_LEVEL_INDEX1:
                    errorLevel = DJI_HMS_ERROR_LEVEL_NONE;
                    break;
                case E_DJI_HMS_ERROR_LEVEL_INDEX2:
                    errorLevel = DJI_HMS_ERROR_LEVEL_HINT;
                    break;
                case E_DJI_HMS_ERROR_LEVEL_INDEX3:
                    errorLevel = DJI_HMS_ERROR_LEVEL_WARN;
                    break;
                case E_DJI_HMS_ERROR_LEVEL_INDEX4:
                    errorLevel = DJI_HMS_ERROR_LEVEL_CRITICAL;
                    break;
                case E_DJI_HMS_ERROR_LEVEL_INDEX5:
                    errorLevel = DJI_HMS_ERROR_LEVEL_FATAL;
                    break;
                default:
                    break;
            }
            DjiHmsCustomization_InjectHmsErrorCode(errorCode, errorLevel);
            osalHandler->TaskSleepMs(500);
            s_isInjectErrcode = false;
            s_isEliminateErrcode = false;
            continue;
        }
        if (s_isEliminateErrcode == true && s_isInjectErrcode == false) {
            switch (s_extensionPortErrcodeIndex) {
                case E_DJI_HMS_ERROR_CODE_INDEX1:
                    errorCode = DJI_HMS_ERROR_CODE_VALUE0;
                    break;
                case E_DJI_HMS_ERROR_CODE_INDEX2:
                    errorCode = DJI_HMS_ERROR_CODE_VALUE1;
                    break;
                case E_DJI_HMS_ERROR_CODE_INDEX3:
                    errorCode = DJI_HMS_ERROR_CODE_VALUE2;
                    break;
                case E_DJI_HMS_ERROR_CODE_INDEX4:
                    errorCode = DJI_HMS_ERROR_CODE_VALUE3;
                    break;
                case E_DJI_HMS_ERROR_CODE_INDEX5:
                    errorCode = DJI_HMS_ERROR_CODE_VALUE4;
                    break;
                default:
                    break;
            }
            DjiHmsCustomization_EliminateHmsErrorCode(errorCode);
            osalHandler->TaskSleepMs(500);
            s_isInjectErrcode = false;
            s_isEliminateErrcode = false;
            continue;
        }

        if (s_isSampleStart != true) {
            continue;
        }

        osalHandler->TaskSleepMs(500);

        s_isSampleStart = false;
        printf("\r\n");
        USER_LOG_INFO("--------------------------------------------------------------------------------------------->");
        DjiTest_WidgetLogAppend("-> Sample Start");

        if (s_aircraftInfoBaseInfo.mountPositionType == DJI_MOUNT_POSITION_TYPE_EXTENSION_PORT ||
            s_aircraftInfoBaseInfo.mountPositionType == DJI_MOUNT_POSITION_TYPE_EXTENSION_LITE_PORT ||
            s_aircraftInfoBaseInfo.mountPositionType == DJI_MOUNT_POSITION_TYPE_MANIFOLD3_ONBOARD ||
            s_aircraftInfoBaseInfo.mountPositionType == DJI_MOUNT_POSITION_TYPE_EXTENSION_PORT_V2) {
            switch (s_extensionPortSampleIndex) {
                case E_DJI_SAMPLE_INDEX_WAYPOINT_V2:
                    if (s_isallowRunFlightControlSample == true) {
                        DjiTest_WaypointV2RunSample();
                    } else {
                        DjiTest_WidgetLogAppend("Please turn on the 'unlock flight control restrictions'");
                        USER_LOG_WARN("Please turn on the 'unlock flight control restrictions' switch.");
                    }
                    break;
                case E_DJI_SAMPLE_INDEX_WAYPOINT_V3:
                    if (s_isallowRunFlightControlSample == true) {
                        DjiTest_WaypointV3RunSample();
                    } else {
                        DjiTest_WidgetLogAppend("Please turn on the 'unlock flight control restrictions'");
                        USER_LOG_WARN("Please turn on the 'unlock flight control restrictions' switch.");
                    }
                    break;
                case E_DJI_SAMPLE_INDEX_FLIGHT_CONTROL_TAKE_OFF_LANDING:
                    if (s_isallowRunFlightControlSample == true) {
                        DjiTest_FlightControlRunSample(E_DJI_TEST_FLIGHT_CTRL_SAMPLE_SELECT_TAKE_OFF_LANDING);
                    } else {
                        USER_LOG_WARN("Please turn on the 'unlock flight control restrictions' switch.");
                    }
                    break;
                case E_DJI_SAMPLE_INDEX_FLIGHT_CONTROL_TAKE_OFF_POSITION_CTRL_LANDING:
                    if (s_isallowRunFlightControlSample == true) {
                        DjiTest_FlightControlRunSample(
                            E_DJI_TEST_FLIGHT_CTRL_SAMPLE_SELECT_TAKE_OFF_POSITION_CTRL_LANDING);
                    } else {
                        USER_LOG_WARN("Please turn on the 'unlock flight control restrictions' switch.");
                    }
                    break;
                case E_DJI_SAMPLE_INDEX_FLIGHT_CONTROL_TAKE_OFF_GO_HOME_FORCE_LANDING:
                    if (s_isallowRunFlightControlSample == true) {
                        DjiTest_FlightControlRunSample(
                            E_DJI_TEST_FLIGHT_CTRL_SAMPLE_SELECT_TAKE_OFF_GO_HOME_FORCE_LANDING);
                    } else {
                        USER_LOG_WARN("Please turn on the 'unlock flight control restrictions' switch.");
                    }
                    break;
                case E_DJI_SAMPLE_INDEX_FLIGHT_CONTROL_TAKE_OFF_VELOCITY_CTRL_LANDING:
                    if (s_isallowRunFlightControlSample == true) {
                        DjiTest_FlightControlRunSample(
                            E_DJI_TEST_FLIGHT_CTRL_SAMPLE_SELECT_TAKE_OFF_VELOCITY_CTRL_LANDING);
                    } else {
                        USER_LOG_WARN("Please turn on the 'unlock flight control restrictions' switch.");
                    }
                    break;
                case E_DJI_SAMPLE_INDEX_FLIGHT_CONTROL_ARREST_FLYING:
                    if (s_isallowRunFlightControlSample == true) {
                        DjiTest_FlightControlRunSample(E_DJI_TEST_FLIGHT_CTRL_SAMPLE_SELECT_ARREST_FLYING);
                    } else {
                        USER_LOG_WARN("Please turn on the 'unlock flight control restrictions' switch.");
                    }
                    break;
                case E_DJI_SAMPLE_INDEX_FLIGHT_CONTROL_SET_GET_PARAM:
                    DjiTest_FlightControlRunSample(E_DJI_TEST_FLIGHT_CTRL_SAMPLE_SELECT_SET_GET_PARAM);
                    break;
                case E_DJI_SAMPLE_INDEX_FC_SUBSCRIPTION:
                    DjiTest_FcSubscriptionRunSample();
                    break;
                case E_DJI_SAMPLE_INDEX_GIMBAL_MANAGER_FREE_MODE:
                    DjiTest_GimbalManagerRunSample(s_mountPosition, DJI_GIMBAL_MODE_FREE);
                    break;
                case E_DJI_SAMPLE_INDEX_GIMBAL_MANAGER_YAW_FOLLOW_MODE:
                    DjiTest_GimbalManagerRunSample(s_mountPosition, DJI_GIMBAL_MODE_YAW_FOLLOW);
                    break;
                case E_DJI_SAMPLE_INDEX_HMS_MANAGER:
                    DjiTest_HmsManagerRunSample(DJI_MOBILE_APP_LANGUAGE_ENGLISH);
                    break;
                case E_DJI_SAMPLE_INDEX_LIVEVIEW:
#ifdef SYSTEM_ARCH_LINUX
                    DjiTest_LiveviewRunSample(s_mountPosition);
#else
                    USER_LOG_WARN("This feature does not support RTOS platform.");
#endif
                    break;
                case E_DJI_SAMPLE_INDEX_PERCEPTION:
#ifdef SYSTEM_ARCH_LINUX
                    DjiTest_PerceptionRunSample(DJI_PERCEPTION_RECTIFY_FRONT);
#else
                    USER_LOG_WARN("This feature does not support RTOS platform.");
#endif
                    break;
                case E_DJI_SAMPLE_INDEX_SWITCH_ALIAS:
                    DjiTestWidget_TriggerChangeAlias();
                    break;
                case E_DJI_SAMPLE_INDEX_CAMMGR_SHUTTER_SPEED:
                    DjiTest_CameraManagerRunSample(s_mountPosition,
                                                   E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_CAMERA_SHUTTER_SPEED);
                    break;
                case E_DJI_SAMPLE_INDEX_CAMMGR_APERTURE:
                    DjiTest_CameraManagerRunSample(s_mountPosition,
                                                   E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_CAMERA_APERTURE);
                    break;
                case E_DJI_SAMPLE_INDEX_CAMMGR_EV:
                    DjiTest_CameraManagerRunSample(s_mountPosition,
                                                   E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_CAMERA_EV);
                    break;
                case E_DJI_SAMPLE_INDEX_CAMMGR_ISO:
                    DjiTest_CameraManagerRunSample(s_mountPosition,
                                                   E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_CAMERA_ISO);
                    break;
                case E_DJI_SAMPLE_INDEX_CAMMGR_FOCUS_POINT:
                    DjiTest_CameraManagerRunSample(s_mountPosition,
                                                   E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_CAMERA_FOCUS_POINT);
                    break;
                case E_DJI_SAMPLE_INDEX_CAMMGR_TAP_ZOOM:
                    DjiTest_CameraManagerRunSample(s_mountPosition,
                                                   E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_CAMERA_TAP_ZOOM_POINT);
                    break;
                case E_DJI_SAMPLE_INDEX_CAMMGR_OPTICAL_ZOOM:
                    DjiTest_CameraManagerRunSample(s_mountPosition,
                                                   E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_CAMERA_ZOOM_PARAM);
                    break;
                case E_DJI_SAMPLE_INDEX_CAMMGR_SINGLE_PHOTO:
                    DjiTest_CameraManagerRunSample(s_mountPosition,
                                                   E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SHOOT_SINGLE_PHOTO);
                    break;
                case E_DJI_SAMPLE_INDEX_CAMMGR_BURST_PHOTO:
                    DjiTest_CameraManagerRunSample(s_mountPosition,
                                                   E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SHOOT_BURST_PHOTO);
                    break;
                case E_DJI_SAMPLE_INDEX_CAMMGR_INTERVAL_PHOTO:
                    DjiTest_CameraManagerRunSample(s_mountPosition,
                                                   E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SHOOT_INTERVAL_PHOTO);
                    break;
                case E_DJI_SAMPLE_INDEX_CAMMGR_RECORDER_VIDEO:
                    DjiTest_CameraManagerRunSample(s_mountPosition,
                                                   E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_RECORD_VIDEO);
                    break;
                case E_DJI_SAMPLE_INDEX_CAMMGR_MEDIA_DOWNLOAD:
                    DjiTest_CameraManagerRunSample(s_mountPosition,
                                                   E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_DOWNLOAD_AND_DELETE_MEDIA_FILE);
                    break;
                case E_DJI_SAMPLE_INDEX_CAMMGR_THERMOMETRY:
                    DjiTest_CameraManagerRunSample(s_mountPosition,
                                                   E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_THERMOMETRY);
                    break;
                default:
                    break;
            }
        } else {
            USER_LOG_WARN("Can't support on payload port.");
        }

        USER_LOG_INFO("--------------------------------------------------------------------------------------------->");
        DjiTest_WidgetLogAppend("-> Sample End");
    }
}

#ifndef __CC_ARM
#pragma GCC diagnostic pop
#endif

static T_DjiReturnCode DjiTestWidget_SetWidgetValue(E_DjiWidgetType widgetType, uint32_t index, int32_t value,
                                                    void *userData)
{
    USER_UTIL_UNUSED(userData);

    DjiTest_WidgetLogAppend("SetWidget type:%s index:%d value:%d",
                            s_widgetTypeNameArray[widgetType], index, value);
    USER_LOG_INFO("Set widget value, widgetType = %s, widgetIndex = %d ,widgetValue = %d",
                  s_widgetTypeNameArray[widgetType], index, value);
    s_widgetValueList[index] = value;

    if (widgetType == DJI_WIDGET_TYPE_SWITCH && index == 7) {
        s_isallowRunFlightControlSample = value;
    }

    if (widgetType == DJI_WIDGET_TYPE_LIST && index == 8) {
        s_mountPosition = value + 1;
    }

    if (widgetType == DJI_WIDGET_TYPE_LIST && index == 9) {
        s_extensionPortSampleIndex = value;
    }

    if (widgetType == DJI_WIDGET_TYPE_BUTTON && index == 10) {
        if (value == 1) {
            s_isSampleStart = true;
        }
    }

    if (widgetType == DJI_WIDGET_TYPE_LIST && index == 11) {
        s_extensionPortErrcodeIndex = value;
    }

    if (widgetType == DJI_WIDGET_TYPE_LIST && index == 12) {
        s_extensionPortErrLevelIndex = value;
    }

    if (widgetType == DJI_WIDGET_TYPE_BUTTON && index == 13) {
        if (value == 1) {
            s_isInjectErrcode = true;
            s_isEliminateErrcode = false;
        }
    }

    if (widgetType == DJI_WIDGET_TYPE_BUTTON && index == 14) {
        if (value == 1) {
            s_isInjectErrcode = false;
            s_isEliminateErrcode = true;
        }
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode DjiTestWidget_GetWidgetValue(E_DjiWidgetType widgetType, uint32_t index, int32_t *value,
                                                    void *userData)
{
    USER_UTIL_UNUSED(userData);
    USER_UTIL_UNUSED(widgetType);

    *value = s_widgetValueList[index];

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode DjiTestWidget_TriggerChangeAlias(void)
{
    USER_LOG_INFO("Payload alias sample start");

    if (s_isAliasChanged == false) {
        DjiCore_SetAlias("測試test!@#$%^&*()");
        USER_LOG_INFO("Set payload alias to '測試test!@#$%^&*()' ");
        s_isAliasChanged = true;
    } else {
        DjiCore_SetAlias("PSDK_APPALIAS");
        USER_LOG_INFO("Set payload alias to 'PSDK_APPALIAS' ");
        s_isAliasChanged = false;
    }

    USER_LOG_INFO("Payload alias sample end");

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
