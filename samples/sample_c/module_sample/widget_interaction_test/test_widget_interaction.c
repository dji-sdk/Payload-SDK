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
#ifdef SYSTEM_ARCH_LINUX
#include "interest_point/test_interest_point.h"
#include "liveview/test_ar_pixel_to_gps.h"
#endif
#include "file_binary_array_list_en.h"
#include <stdarg.h>
#include "dji_aircraft_info.h"
#include "dji_core.h"
#include <payload_collaboration/test_payload_collaboration.h>
#include <waypoint_v3/test_waypoint_v3.h>
#include "dji_sdk_config.h"
#include "hms/hms_text_c/en/hms_text_config_json.h"
#include "dji_hms.h"
#include "positioning/test_positioning.h"

/* Private constants ---------------------------------------------------------*/
#define WIDGET_DIR_PATH_LEN_MAX         (256)
#define WIDGET_TASK_STACK_SIZE          (2048)

#define DJI_HMS_ERROR_CODE_VALUE0    0x1E020005
#define DJI_HMS_ERROR_CODE_VALUE1    0x1E020006
#define DJI_HMS_ERROR_CODE_VALUE2    0x1E020007
#define DJI_HMS_ERROR_CODE_VALUE3    0x1E020008
#define DJI_HMS_ERROR_CODE_VALUE4    0x1E020009


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
    E_DJI_SAMPLE_INDEX_ON_BOARD_NETWORK_RTK_START = 28,
    E_DJI_SAMPLE_INDEX_ON_BOARD_NETWORK_RTK_STOP = 29,
    E_DJI_SAMPLE_INDEX_SPZ_APPROACH = 30,
    E_DJI_SAMPLE_INDEX_SPZ_STOP     = 31,
    E_DJI_SAMPLE_INDEX_AR_PIXEL_GPS_START = 32,
    E_DJI_SAMPLE_INDEX_AR_PIXEL_GPS_STOP  = 33,
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

/* Private functions declaration ---------------------------------------------*/
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
static T_DjiAircraftInfoBaseInfo s_aircraftInfoBaseInfo = {0};
static bool s_isAliasChanged = false;

#ifdef SYSTEM_ARCH_LINUX
/* SPZ dedicated thread: keeps the main widget loop unblocked */
static T_DjiTaskHandle s_spzTaskHandle = NULL;

/* AR Pixel-to-GPS dedicated thread */
static T_DjiTaskHandle s_arPixelGpsTaskHandle = NULL;

/* SPZ approach parameters, input sequentially via index=6:
 * 1st→speed(m/s)  2nd→angle(deg) */
static float s_spzSpeedMs  = 5.0f;
static float s_spzAngleDeg = 60.0f;
static int   s_spzParamInputIdx = 0;  /* 0=speed 1=angle */
#endif

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

#ifndef __CC_ARM
#pragma GCC diagnostic pop
#endif

#ifndef __CC_ARM
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wreturn-type"
#pragma GCC diagnostic ignored "-Wformat"
#endif

#ifdef SYSTEM_ARCH_LINUX
static void *DjiUser_SpzTask(void *arg)
{
    USER_UTIL_UNUSED(arg);
    USER_LOG_INFO("[SPZ] Task started");
    DjiTest_SpzApproachRunSample();
    USER_LOG_INFO("[SPZ] Task finished");
    s_spzTaskHandle = NULL;
    return NULL;
}

/* Pre-initialize SPZ modules in the main thread (large stack) before
 * the first task creation, to avoid stack overflow in the task thread. */
static void DjiUser_SpzPreInit(void)
{
    static bool s_preInitDone = false;
    if (s_preInitDone) return;
    USER_LOG_INFO("[SPZ] Pre-initializing modules in main thread...");
    DjiTest_SpzRunPreInit();
    s_preInitDone = true;
    USER_LOG_INFO("[SPZ] Pre-init done");
}

static void *DjiUser_ArPixelGpsTask(void *arg)
{
    USER_UTIL_UNUSED(arg);
    USER_LOG_INFO("[ArPixelGPS] Task started");
    DjiUser_RunArPixelToGpsSample();
    USER_LOG_INFO("[ArPixelGPS] Task finished");
    s_arPixelGpsTaskHandle = NULL;
    return NULL;
}
#endif /* SYSTEM_ARCH_LINUX */

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
            s_aircraftInfoBaseInfo.mountPositionType == DJI_MOUNT_POSITION_TYPE_EXTENSION_PORT_V2 ||
            s_aircraftInfoBaseInfo.mountPositionType == DJI_MOUNT_POSITION_TYPE_EXTENSION_PORT_V3) {
            switch (s_extensionPortSampleIndex) {
                case E_DJI_SAMPLE_INDEX_WAYPOINT_V2:
                    if (s_isallowRunFlightControlSample == true) {
                        DjiTest_WaypointV2RunSample();
                    } else {
                        DjiTest_WidgetLogAppend("turn on 'unlock flight control restrictions!!'");
                        USER_LOG_WARN("Please turn on the 'unlock flight control restrictions' switch.");
                    }
                    break;
                case E_DJI_SAMPLE_INDEX_WAYPOINT_V3:
                    if (s_isallowRunFlightControlSample == true) {
                        DjiTest_WaypointV3RunSample();
                    } else {
                        DjiTest_WidgetLogAppend("turn on 'unlock flight control restrictions!!'");
                        USER_LOG_WARN("Please turn on the 'unlock flight control restrictions' switch.");
                    }
                    break;
                case E_DJI_SAMPLE_INDEX_FLIGHT_CONTROL_TAKE_OFF_LANDING:
                    if (s_isallowRunFlightControlSample == true) {
                        DjiTest_FlightControlRunSample(E_DJI_TEST_FLIGHT_CTRL_SAMPLE_SELECT_TAKE_OFF_LANDING);
                    } else {
                        DjiTest_WidgetLogAppend("should turn on 'unlock flight control restrictions.");
                        USER_LOG_WARN("Please turn on the 'unlock flight control restrictions' switch.");
                    }
                    break;
                case E_DJI_SAMPLE_INDEX_FLIGHT_CONTROL_TAKE_OFF_POSITION_CTRL_LANDING:
                    if (s_isallowRunFlightControlSample == true) {
                        DjiTest_FlightControlRunSample(
                            E_DJI_TEST_FLIGHT_CTRL_SAMPLE_SELECT_TAKE_OFF_POSITION_CTRL_LANDING);
                    } else {
                        DjiTest_WidgetLogAppend("should turn on 'unlock flight control restrictions..");
                        USER_LOG_WARN("Please turn on the 'unlock flight control restrictions' switch.");
                    }
                    break;
                case E_DJI_SAMPLE_INDEX_FLIGHT_CONTROL_TAKE_OFF_GO_HOME_FORCE_LANDING:
                    if (s_isallowRunFlightControlSample == true) {
                        DjiTest_FlightControlRunSample(
                            E_DJI_TEST_FLIGHT_CTRL_SAMPLE_SELECT_TAKE_OFF_GO_HOME_FORCE_LANDING);
                    } else {
                        DjiTest_WidgetLogAppend("should turn on 'unlock flight control restrictions.");
                        USER_LOG_WARN("Please turn on the 'unlock flight control restrictions' switch.");
                    }
                    break;
                case E_DJI_SAMPLE_INDEX_FLIGHT_CONTROL_TAKE_OFF_VELOCITY_CTRL_LANDING:
                    if (s_isallowRunFlightControlSample == true) {
                        DjiTest_FlightControlRunSample(
                            E_DJI_TEST_FLIGHT_CTRL_SAMPLE_SELECT_TAKE_OFF_VELOCITY_CTRL_LANDING);
                    } else {
                        DjiTest_WidgetLogAppend("should turn on 'unlock flight control restrictions.");
                        USER_LOG_WARN("Please turn on the 'unlock flight control restrictions' switch.");
                    }
                    break;
                case E_DJI_SAMPLE_INDEX_FLIGHT_CONTROL_ARREST_FLYING:
                    if (s_isallowRunFlightControlSample == true) {
                        DjiTest_FlightControlRunSample(E_DJI_TEST_FLIGHT_CTRL_SAMPLE_SELECT_ARREST_FLYING);
                    } else {
                        DjiTest_WidgetLogAppend("should turn on 'unlock flight control restrictions.");
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
                case E_DJI_SAMPLE_INDEX_ON_BOARD_NETWORK_RTK_START:
                    DjiTest_NetworkRtkOnBoardService(DJI_TEST_NETWORK_RTK_START);
                    break;
                case E_DJI_SAMPLE_INDEX_ON_BOARD_NETWORK_RTK_STOP:
                    DjiTest_NetworkRtkOnBoardService(DJI_TEST_NETWORK_RTK_STOP);
                    break;
#ifdef SYSTEM_ARCH_LINUX
                case E_DJI_SAMPLE_INDEX_SPZ_APPROACH: {
                    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
                    if (osalHandler == NULL) {
                        USER_LOG_ERROR("Get osal handler failed");
                        break;
                    }
                    DjiUser_SpzPreInit();  /* run heavy init in main thread before task */
                    s_spzParamInputIdx = 0;
                    if (s_spzTaskHandle != NULL) {
                        USER_LOG_INFO("[SPZ] Previous run still active, requesting stop...");
                        DjiTest_SpzRequestStop();
                        /* Poll until task self-clears s_spzTaskHandle (max 3s) */
                        for (int i = 0; i < 30 && s_spzTaskHandle != NULL; i++) {
                            osalHandler->TaskSleepMs(100);
                        }
                        if (s_spzTaskHandle != NULL) {
                            USER_LOG_WARN("[SPZ] Old task did not exit in 3s, skip new trigger");
                            break;
                        }
                    }
                    if (osalHandler->TaskCreate("spz_task", DjiUser_SpzTask,
                                                32768, NULL, &s_spzTaskHandle)
                        != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                        USER_LOG_ERROR("[SPZ] Failed to create SPZ task");
                        s_spzTaskHandle = NULL;
                    }
                    break;
                }
                case E_DJI_SAMPLE_INDEX_SPZ_STOP: {
                    if (s_spzTaskHandle != NULL) {
                        USER_LOG_INFO("[SPZ] Stop requested via widget");
                        DjiTest_SpzRequestStop();
                    } else {
                        USER_LOG_WARN("[SPZ] No active SPZ task to stop");
                    }
                    break;
                }
                case E_DJI_SAMPLE_INDEX_AR_PIXEL_GPS_START: {
                    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
                    if (osalHandler == NULL) {
                        USER_LOG_ERROR("Get osal handler failed");
                        break;
                    }
                    if (s_arPixelGpsTaskHandle != NULL) {
                        USER_LOG_INFO("[ArPixelGPS] Previous run still active, requesting stop...");
                        DjiUser_ArPixelGpsRequestStop();
                        for (int i = 0; i < 30 && s_arPixelGpsTaskHandle != NULL; i++) {
                            osalHandler->TaskSleepMs(100);
                        }
                        if (s_arPixelGpsTaskHandle != NULL) {
                            USER_LOG_WARN("[ArPixelGPS] Old task did not exit in 3s, skip new trigger");
                            break;
                        }
                    }
                    if (osalHandler->TaskCreate("ar_gps_task", DjiUser_ArPixelGpsTask,
                                                 32768, NULL, &s_arPixelGpsTaskHandle)
                        != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                        USER_LOG_ERROR("[ArPixelGPS] Failed to create task");
                        s_arPixelGpsTaskHandle = NULL;
                    }
                    break;
                }
                case E_DJI_SAMPLE_INDEX_AR_PIXEL_GPS_STOP: {
                    if (s_arPixelGpsTaskHandle != NULL) {
                        USER_LOG_INFO("[ArPixelGPS] Stop requested via widget");
                        DjiUser_ArPixelGpsRequestStop();
                    } else {
                        USER_LOG_WARN("[ArPixelGPS] No active task to stop");
                    }
                    break;
                }
#endif /* SYSTEM_ARCH_LINUX */
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

    /* SPZ approach param input via index=6 (INT_INPUT_BOX):
     * 1st input → speed(m/s),  2nd → angle(deg),  then cycles.
     * Can be set anytime before triggering. */
#ifdef SYSTEM_ARCH_LINUX
    if (widgetType == DJI_WIDGET_TYPE_INT_INPUT_BOX && index == 6) {
        const char *names[] = {"speed(m/s)", "angle(deg)"};
        int prevIdx = s_spzParamInputIdx;
        if (prevIdx == 0) {
            s_spzSpeedMs = (float)value;
        } else {
            s_spzAngleDeg = (float)value;
        }
        s_spzParamInputIdx = (prevIdx + 1) % 2;
        if (s_spzTaskHandle != NULL) {
            /* Only log SPZ content when SPZ is active */
            USER_LOG_INFO("[SPZ] %s = %d  next: %s",
                          names[prevIdx], value, names[s_spzParamInputIdx]);
            DjiTest_WidgetLogAppend("[SPZ] %s = %d  next: %s",
                          names[prevIdx], value, names[s_spzParamInputIdx]);
        }
        DjiTest_SpzSetApproachParam(s_spzSpeedMs, s_spzAngleDeg);
    }
#endif /* SYSTEM_ARCH_LINUX */

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
