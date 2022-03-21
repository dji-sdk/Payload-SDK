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

/* Private constants ---------------------------------------------------------*/
#define WIDGET_DIR_PATH_LEN_MAX         (256)
#define WIDGET_TASK_STACK_SIZE          (2048)

#define WIDGET_LOG_STRING_MAX_SIZE      (50)
#define WIDGET_LOG_LINE_MAX_NUM         (5)

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
    E_DJI_SAMPLE_INDEX_HMS = 9,
    E_DJI_SAMPLE_INDEX_CAMERA_MANAGER = 10,
    E_DJI_SAMPLE_INDEX_GIMBAL_MANAGER_FREE_MODE = 11,
    E_DJI_SAMPLE_INDEX_GIMBAL_MANAGER_YAW_FOLLOW_MODE = 12,
    E_DJI_SAMPLE_INDEX_LIVEVIEW = 13,
    E_DJI_SAMPLE_INDEX_PERCEPTION = 14,
    E_DJI_SAMPLE_INDEX_UNKNOWN = 0xFF,
} E_DjiExtensionPortSampleIndex;

typedef enum {
    E_DJI_SAMPLE_INDEX_SWITCH_ALIAS = 0,
    E_DJI_SAMPLE_INDEX_FC_SUB_LOG_ON_OFF = 1,
    E_DJI_SAMPLE_INDEX_COLLABORATION_LOG_ON_OFF = 2,
} E_DjiPayloadPortSampleIndex;

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
static E_DjiPayloadPortSampleIndex s_payloadPortSampleIndex = E_DJI_SAMPLE_INDEX_SWITCH_ALIAS;
static bool s_isallowRunFlightControlSample = false;
static bool s_isSampleStart = false;
static E_DjiMountPosition s_mountPosition = DJI_MOUNT_POSITION_PAYLOAD_PORT_NO1;
static E_DjiPerceptionDirection s_perceptionDirection = DJI_PERCEPTION_RECTIFY_DOWN;
static E_DjiTestCameraManagerSampleSelect s_cameraManagerSampleSelect =
    E_DJI_TEST_CAMERA_MANAGER_SAMPLE_SELECT_SET_CAMERA_SHUTTER_SPEED;
static T_DjiTestWidgetLog s_djiTestWidgetLog[WIDGET_LOG_LINE_MAX_NUM] = {0};
static T_DjiAircraftInfoBaseInfo s_aircraftInfoBaseInfo = {0};
static bool s_isAliasChanged = false;

static const T_DjiWidgetHandlerListItem s_widgetHandlerList[] = {
    {0,  DJI_WIDGET_TYPE_BUTTON,        DjiTestWidget_SetWidgetValue, DjiTestWidget_GetWidgetValue, NULL},
    {1,  DJI_WIDGET_TYPE_BUTTON,        DjiTestWidget_SetWidgetValue, DjiTestWidget_GetWidgetValue, NULL},
    {2,  DJI_WIDGET_TYPE_LIST,          DjiTestWidget_SetWidgetValue, DjiTestWidget_GetWidgetValue, NULL},
    {3,  DJI_WIDGET_TYPE_SWITCH,        DjiTestWidget_SetWidgetValue, DjiTestWidget_GetWidgetValue, NULL},
    {4,  DJI_WIDGET_TYPE_SCALE,         DjiTestWidget_SetWidgetValue, DjiTestWidget_GetWidgetValue, NULL},
    {5,  DJI_WIDGET_TYPE_BUTTON,        DjiTestWidget_SetWidgetValue, DjiTestWidget_GetWidgetValue, NULL},
    {6,  DJI_WIDGET_TYPE_SCALE,         DjiTestWidget_SetWidgetValue, DjiTestWidget_GetWidgetValue, NULL},
    {7,  DJI_WIDGET_TYPE_INT_INPUT_BOX, DjiTestWidget_SetWidgetValue, DjiTestWidget_GetWidgetValue, NULL},
    {8,  DJI_WIDGET_TYPE_BUTTON,        DjiTestWidget_SetWidgetValue, DjiTestWidget_GetWidgetValue, NULL},
    {9,  DJI_WIDGET_TYPE_SWITCH,        DjiTestWidget_SetWidgetValue, DjiTestWidget_GetWidgetValue, NULL},
    {10, DJI_WIDGET_TYPE_LIST,          DjiTestWidget_SetWidgetValue, DjiTestWidget_GetWidgetValue, NULL},
    {11, DJI_WIDGET_TYPE_LIST,          DjiTestWidget_SetWidgetValue, DjiTestWidget_GetWidgetValue, NULL},
    {12, DJI_WIDGET_TYPE_LIST,          DjiTestWidget_SetWidgetValue, DjiTestWidget_GetWidgetValue, NULL},
    {13, DJI_WIDGET_TYPE_LIST,        DjiTestWidget_SetWidgetValue, DjiTestWidget_GetWidgetValue, NULL},
    {14, DJI_WIDGET_TYPE_BUTTON,        DjiTestWidget_SetWidgetValue, DjiTestWidget_GetWidgetValue, NULL},
    {15, DJI_WIDGET_TYPE_BUTTON,        DjiTestWidget_SetWidgetValue, DjiTestWidget_GetWidgetValue, NULL},
    {16, DJI_WIDGET_TYPE_LIST,          DjiTestWidget_SetWidgetValue, DjiTestWidget_GetWidgetValue, NULL},
    {17, DJI_WIDGET_TYPE_BUTTON,        DjiTestWidget_SetWidgetValue, DjiTestWidget_GetWidgetValue, NULL},
};

static char *s_widgetTypeNameArray[] = {
    "Unknown",
    "Button",
    "Switch",
    "Scale",
    "List",
    "Int input box"
};

static const uint32_t s_widgetHandlerListCount = sizeof(s_widgetHandlerList) / sizeof(T_DjiWidgetHandlerListItem);
static int32_t s_widgetValueList[sizeof(s_widgetHandlerList) / sizeof(T_DjiWidgetHandlerListItem)] = {0};

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

#ifdef SYSTEM_ARCH_LINUX
    //Step 2 : Set UI Config (Linux environment)
    char curFileDirPath[WIDGET_DIR_PATH_LEN_MAX];
    char tempPath[WIDGET_DIR_PATH_LEN_MAX];
    djiStat = DjiUserUtil_GetCurrentFileDirPath(__FILE__, WIDGET_DIR_PATH_LEN_MAX, curFileDirPath);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Get file current path error, stat = 0x%08llX", djiStat);
        return djiStat;
    }

    snprintf(tempPath, WIDGET_DIR_PATH_LEN_MAX, "%swidget_file/en_big_screen", curFileDirPath);

    //set default ui config path
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
    snprintf(tempPath, WIDGET_DIR_PATH_LEN_MAX, "%swidget_file/cn_big_screen", curFileDirPath);

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
        snprintf(message, DJI_WIDGET_FLOATING_WINDOW_MSG_MAX_LEN,
                 "System time : %u ms \r\n%s \r\n%s \r\n%s \r\n%s \r\n%s \r\n",
                 sysTimeMs,
                 s_djiTestWidgetLog[0].content,
                 s_djiTestWidgetLog[1].content,
                 s_djiTestWidgetLog[2].content,
                 s_djiTestWidgetLog[3].content,
                 s_djiTestWidgetLog[4].content);

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

    returnCode = DjiAircraftInfo_GetBaseInfo(&s_aircraftInfoBaseInfo);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("get aircraft base info error");
        return NULL;
    }

    while (1) {
        osalHandler->TaskSleepMs(100);

        if (s_isSampleStart != true) {
            continue;
        }

        osalHandler->TaskSleepMs(500);

        s_isSampleStart = false;
        printf("\r\n");
        USER_LOG_INFO("--------------------------------------------------------------------------------------------->");
        DjiTest_WidgetLogAppend("-> Sample Start");

        if (s_aircraftInfoBaseInfo.mountPosition == DJI_MOUNT_POSITION_EXTENSION_PORT) {
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
                case E_DJI_SAMPLE_INDEX_CAMERA_MANAGER:
                    DjiTest_CameraManagerRunSample(s_mountPosition, s_cameraManagerSampleSelect);
                    break;
                case E_DJI_SAMPLE_INDEX_GIMBAL_MANAGER_FREE_MODE:
                    DjiTest_GimbalManagerRunSample(s_mountPosition, DJI_GIMBAL_MODE_FREE);
                    break;
                case E_DJI_SAMPLE_INDEX_GIMBAL_MANAGER_YAW_FOLLOW_MODE:
                    DjiTest_GimbalManagerRunSample(s_mountPosition, DJI_GIMBAL_MODE_YAW_FOLLOW);
                    break;
                case E_DJI_SAMPLE_INDEX_HMS:
                    DjiTest_HmsRunSample();
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
                    DjiTest_PerceptionRunSample(s_perceptionDirection);
#else
                    USER_LOG_WARN("This feature does not support RTOS platform.");
#endif
                    break;
                default:
                    break;
            }
        } else {
            switch (s_payloadPortSampleIndex) {
                case E_DJI_SAMPLE_INDEX_SWITCH_ALIAS:
                    DjiTestWidget_TriggerChangeAlias();
                    break;
                case E_DJI_SAMPLE_INDEX_FC_SUB_LOG_ON_OFF:
                    DjiTest_FcSubscriptionDataShowTrigger();
                    break;
                case E_DJI_SAMPLE_INDEX_COLLABORATION_LOG_ON_OFF:
                    DjiTest_PayloadCollaborationDataShowTrigger();
                    break;
                default:
                    break;
            }
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

    if (widgetType == DJI_WIDGET_TYPE_SWITCH && index == 9) {
        s_isallowRunFlightControlSample = value;
    }

    if (widgetType == DJI_WIDGET_TYPE_LIST && index == 10) {
        s_mountPosition = value + 1;
    }

    if (widgetType == DJI_WIDGET_TYPE_LIST && index == 11) {
        s_perceptionDirection = value;
    }

    if (widgetType == DJI_WIDGET_TYPE_LIST && index == 12) {
        s_cameraManagerSampleSelect = value;
    }

    if (widgetType == DJI_WIDGET_TYPE_LIST && index == 13) {
        s_extensionPortSampleIndex = value;
    }

    if (widgetType == DJI_WIDGET_TYPE_BUTTON && index == 14) {
        if (value == 1) {
            s_isSampleStart = true;
        }
    }

    if (widgetType == DJI_WIDGET_TYPE_LIST && index == 16) {
        s_payloadPortSampleIndex = value;
    }

    if (widgetType == DJI_WIDGET_TYPE_BUTTON && index == 17) {
        if (value == 1) {
            s_isSampleStart = true;
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
    if (s_isAliasChanged == false) {
        DjiCore_SetAlias("測試test!@#$%^&*()");
        s_isAliasChanged = true;
    } else {
        DjiCore_SetAlias("PSDK_APPALIAS");
        s_isAliasChanged = false;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
