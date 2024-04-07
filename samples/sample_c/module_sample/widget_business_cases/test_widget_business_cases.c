/**
 ********************************************************************
 * @file    test_widget.c
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
#define TEST_WIDGET_BUSINES_CASES_C

/* Includes ------------------------------------------------------------------*/
#include <stdarg.h>
#include "test_widget_business_cases.h"
#include "test_widget_action.h"
#include <dji_logger.h>
#include "../utils/util_misc.h"
#include "utils/util_file.h"
#include "utils/cJSON.h"
#include "utils/util_link_list.h"

// #include "dji_sdk_config.h"
#undef CONFIG_WIDGET_USING_JSON_FILES
#define CONFIG_WIDGET_USING_JSON_FILES true
#if (CONFIG_WIDGET_USING_JSON_FILES == false)
#include "file_binary_array_list_en.h"
#endif

/* Private constants ---------------------------------------------------------*/
#define DJI_WIDGET_CONFIG_FILE_NAME     "widget_config.json"

#define WIDGET_LOG_VER(fmt, ...)        // printf("\n" fmt "\n", ##__VA_ARGS__);
#define WIDGET_LOG_DBG(fmt, ...)        USER_LOG_DEBUG(fmt, ##__VA_ARGS__)
#define WIDGET_LOG_INF(...)             USER_LOG_INFO(__VA_ARGS__)
#define WIDGET_LOG_WAR(...)             USER_LOG_WARN(__VA_ARGS__)
#define WIDGET_LOG_ERR(...)             USER_LOG_ERROR(__VA_ARGS__)
#define WIDGET_ASSERT(expression)       ({(expression) ? (void)0 : WIDGET_LOG_ERR("Assertion:\"%s\" ret is false in %s:%d.", #expression, __FILE__, __LINE__);})

#define WIDGET_DIR_PATH_LEN_MAX         (256)
#define WIDGET_TASK_STACK_SIZE          (2048)
#define TEST_WIDGET_WORK_QUE_MAX        (10)

#define DjiTest_WidgetLogAppend(...)    (void)DjiTest_WidgetBusinessPushLog(__VA_ARGS__)
#define WIDGET_LOG_LINE_MAX_TTL_MS      (30 * 1000)
#define WIDGET_LOG_LINE_MAX_NUM         (4)

/* Private types -------------------------------------------------------------*/
typedef struct tagT_DjiWidgetHandlerList {
    T_DjiWidgetHandlerListItem          *items;
    uint32_t                            count;
} T_DjiWidgetHandlerList;

typedef struct tagT_TestWidgetLogLine {
    uint32_t                            createMs;
    char                                *pStr;
} T_TestWidgetLogLine;

/* Private functions declaration ---------------------------------------------*/
static void *DjiTest_WidgetWorkTask(void *arg);
static T_DjiReturnCode DjiTest_WidgetWaitForWorkWithTimeOut(uint32_t timeMs);
static void DjiTest_WidgetHandlerListDestory(T_DjiWidgetHandlerList *widgetHandlerList);
static T_DjiWidgetHandlerList *DjiTest_WidgetHandlerListCreateByJsonData(const char *widgetConfigFileName, const uint8_t *jsonData);

/* Private values ------------------------------------------------------------*/
static T_DjiTaskHandle s_widgetTestThread;
static bool s_isWidgetFileDirPathConfigured = false;
static char s_widgetFileDirPath[DJI_FILE_PATH_SIZE_MAX] = {0};

// json value string mapping of E_DjiWidgetType
static const char *s_widgetTypeStringMapping[] = {
    "unknown",
    "button",
    "switch",
    "scale",
    "list",
    "int_input_box"
};

static T_DjiTaskHandle                  s_widgetTestWorkThread;
static T_DjiSemaHandle                  s_widgetTestWorkSem = NULL;

static T_UtilLinkList                   s_widgetTestWorkQue = { NULL, NULL, 0};
static T_DjiMutexHandle                 s_widgetTestWorkQueMutex = NULL;

static T_UtilLinkList                   s_widgetTestLogList = { NULL, NULL, 0};
static T_DjiMutexHandle                 s_widgetTestLogListMutex = NULL;

/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode DjiTest_WidgetBusinessStartService(void)
{
    T_DjiOsalHandler                    *osalHandler = DjiPlatform_GetOsalHandler();
    T_DjiWidgetHandlerList              *widgetHandlerList = NULL;
    T_DjiReturnCode                     djiStat = DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;

    // static resource initialization
    {
        if (DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS != osalHandler->SemaphoreCreate(0, &s_widgetTestWorkSem)) {
            WIDGET_LOG_ERR("SemaphoreCreate(\"s_widgetTestWorkSem\") error.");
            return djiStat;
        }

        // DjiUserUtil_InitLinkList(&s_widgetTestWorkQue);
        if (DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS != osalHandler->MutexCreate(&s_widgetTestWorkQueMutex)) {
            WIDGET_LOG_ERR("MutexCreate(\"s_widgetTestWorkQueMutex\") error.");
            return djiStat;
        }

        // DjiUserUtil_InitLinkList(&s_widgetTestLogList);
        if (DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS != osalHandler->MutexCreate(&s_widgetTestLogListMutex)) {
            WIDGET_LOG_ERR("MutexCreate(\"s_widgetTestLogListMutex\") error.");
            return djiStat;
        }
    }

    //Step 1 : Init DJI Widget
    djiStat = DjiWidget_Init();
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Dji test widget init error, stat = 0x%08llX", djiStat);
        return djiStat;
    }

    #if CONFIG_WIDGET_USING_JSON_FILES
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

        // parse DJI_WIDGET_CONFIG_FILE_NAME and create widgetHandlerList
        snprintf(tempPath + strlen(tempPath), WIDGET_DIR_PATH_LEN_MAX - strlen(tempPath), "/%s", DJI_WIDGET_CONFIG_FILE_NAME);
        uint32_t                            readRealSize = 0;
        uint32_t                            fileSize = 0;
        uint8_t                             *jsonData = NULL;
        djiStat = UtilFile_GetFileSizeByPath(tempPath, &fileSize);
        if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            WIDGET_LOG_ERR("Get file:\"%s\" size by path failed, stat = 0x%08llX", tempPath, djiStat);
            return djiStat;
        }

        jsonData = osalHandler->Malloc(fileSize + 1);
        if (jsonData == NULL) {
            WIDGET_LOG_ERR("Malloc jsonData:%d failed.", fileSize);
            return DJI_ERROR_SYSTEM_MODULE_CODE_MEMORY_ALLOC_FAILED;
        }
        memset(jsonData, 0, fileSize);
        UtilFile_GetFileDataByPath(tempPath, 0, fileSize, jsonData, &readRealSize);
        widgetHandlerList = DjiTest_WidgetHandlerListCreateByJsonData(tempPath, jsonData);
        osalHandler->Free(jsonData);
        if (NULL == widgetHandlerList) {
            USER_LOG_ERROR("DjiTest_WidgetHandlerListCreateByJsonData error");
            return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
        }
    #else
        //Step 2 : Set UI Config (RTOS environment)
        // TODO: unsupported
        #ERROR "unsupported to using T_DjiWidgetBinaryArrayConfig"

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

        // parse DJI_WIDGET_CONFIG_FILE_NAME and create widgetHandlerList
        for (int i = 0; i < enWidgetBinaryArrayConfig.binaryArrayCount; i++) {
            if (0 == strcmp(enWidgetBinaryArrayConfig.fileBinaryArrayList[i].fileName, DJI_WIDGET_CONFIG_FILE_NAME)) {
                widgetHandlerList = DjiTest_WidgetHandlerListCreateByJsonData(tempPath, enWidgetBinaryArrayConfig.fileBinaryArrayList[i].fileBinaryArray);
            }
        }
        if (NULL == widgetHandlerList) {
            USER_LOG_ERROR("DjiTest_WidgetHandlerListCreateByJsonData error");
            return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
        }
    #endif

    //Step 3 : Set widget handler list
    djiStat = DjiWidget_RegHandlerList(widgetHandlerList->items, widgetHandlerList->count);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Set widget handler list error, stat = 0x%08llX", djiStat);
        DjiTest_WidgetHandlerListDestory(widgetHandlerList);
        return djiStat;
    }
    widgetHandlerList->count = 0; // ->items[i].userData has registered to DjiWidget, don't free them.
    DjiTest_WidgetHandlerListDestory(widgetHandlerList);

    //Step 4 : Run widget api sample task
    if (osalHandler->TaskCreate("user_widget_task", DjiTest_WidgetWorkTask, WIDGET_TASK_STACK_SIZE, NULL,
                                &s_widgetTestThread) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Dji widget test task create error.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiTest_WidgetBusinessSetConfigFilePath(const char *path)
{
    memset(s_widgetFileDirPath, 0, sizeof(s_widgetFileDirPath));
    memcpy(s_widgetFileDirPath, path, USER_UTIL_MIN(strlen(path), sizeof(s_widgetFileDirPath) - 1));
    s_isWidgetFileDirPathConfigured = true;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode DjiTest_WidgetBusinessGetLog(char *message, uint16_t max_len)
{
    #define SHOW_TS_IN_EACH_WIDGET_LOG  true

    T_UtilListNode                      *logNode = NULL;
    T_TestWidgetLogLine                 *log = NULL;
    uint16_t                            current = 0;

    UTIL_OSAL_MUTEX_LOCK_MAY_RETURN( s_widgetTestLogListMutex );
    logNode = s_widgetTestLogList.first;
    while (NULL != logNode)
    {
        log = logNode->data;
        #if SHOW_TS_IN_EACH_WIDGET_LOG
        current += snprintf(message + current, max_len - current, "-%u.%u %s\n", log->createMs / 1000, log->createMs % 1000, log->pStr);
        #else
        current += snprintf(message + current, max_len - current, "- %s\n", (char *)logNode->data);
        #endif
        if (current >= max_len) {
            // TODO: when log is larger then DJI_WIDGET_FLOATING_WINDOW_MSG_MAX_LEN, it can be called DjiWidgetFloatingWindow_ShowMessage and refreshed immediately in fact.
            // tactics: drop old message.
            break;
        }
        logNode = logNode->next;
    }
    UTIL_OSAL_MUTEX_UNLOCK_MAY_RETURN( s_widgetTestLogListMutex );

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode DjiTest_WidgetBusinessClearLog(uint32_t nowTimeMs)
{
    T_UtilListNode                      *logNode = NULL;
    T_TestWidgetLogLine                 *log = NULL;

    UTIL_OSAL_MUTEX_LOCK_MAY_RETURN( s_widgetTestLogListMutex );
    logNode = s_widgetTestLogList.last;
    while (logNode != s_widgetTestLogList.first) {
        log = logNode->data;

        // if ( WIDGET_LOG_LINE_MAX_NUM < s_widgetTestLogList.count ) {
        //     UTIL_OSAL_MEMRY_FREE(log->pStr);
        //     UTIL_OSAL_MEMRY_FREE(log);
        //     logNode = logNode->prev;
        //     DjiUserUtil_LinkListRemoveNodeOnly( &s_widgetTestLogList, s_widgetTestLogList.last );
        //     continue;
        // }

        if ((nowTimeMs - log->createMs) > WIDGET_LOG_LINE_MAX_TTL_MS) {
            UTIL_OSAL_MEMRY_FREE(log->pStr);
            UTIL_OSAL_MEMRY_FREE(log);
            logNode = logNode->prev;
            DjiUserUtil_LinkListRemoveNodeOnly( &s_widgetTestLogList, s_widgetTestLogList.last );
            continue;
        }
        break;
    }
    UTIL_OSAL_MUTEX_UNLOCK_MAY_RETURN( s_widgetTestLogListMutex );
    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiTest_WidgetBusinessPushLog(const char *fmt, ...)
{
    static T_DjiOsalHandler             *osalHandler = NULL;
    T_TestWidgetLogLine                 *log = NULL;
    va_list                             args;
    char                                string[WIDGET_DIR_PATH_LEN_MAX];

    va_start(args, fmt);
    vsnprintf(string, WIDGET_DIR_PATH_LEN_MAX, fmt, args);
    va_end(args);
    WIDGET_LOG_INF("%s", string);

    if (NULL == osalHandler) {
        osalHandler = DjiPlatform_GetOsalHandler();
        if (NULL == osalHandler) {
            return DJI_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT_IN_CURRENT_STATE;
        }
    }

    // log && pStr will be free by DjiTest_WidgetBusinessClearLog
    log = osalHandler->Malloc(sizeof(T_TestWidgetLogLine));
    if (NULL == log) {
        WIDGET_LOG_ERR("memry alloc %d failed!", sizeof(T_TestWidgetLogLine));
        return DJI_ERROR_SYSTEM_MODULE_CODE_MEMORY_ALLOC_FAILED;
    }
    log->pStr = osalHandler->Malloc(strlen(string) + 1);
    if (NULL == log->pStr) {
        osalHandler->Free(log);
        WIDGET_LOG_ERR("memry alloc %d failed!", sizeof(T_TestWidgetLogLine));
        return DJI_ERROR_SYSTEM_MODULE_CODE_MEMORY_ALLOC_FAILED;
    }
    strcpy(log->pStr, string);
    log->pStr[strlen(string) + 1] = 0;

    (void)osalHandler->GetTimeMs(&log->createMs);

    if( DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS != osalHandler->MutexLock(s_widgetTestLogListMutex) ) {
        osalHandler->Free(log->pStr);
        osalHandler->Free(log);
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }
    DjiUserUtil_LinkListAddNodeFirst( &s_widgetTestLogList, DjiUserUtil_NewListNode( log ));
    if ( WIDGET_LOG_LINE_MAX_NUM < s_widgetTestLogList.count ) {
        log = s_widgetTestLogList.last->data;
        osalHandler->Free(log->pStr);
        osalHandler->Free(log);
        DjiUserUtil_LinkListRemoveNodeOnly( &s_widgetTestLogList, s_widgetTestLogList.last);
    }
    (void)osalHandler->MutexUnlock(s_widgetTestLogListMutex);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

#ifndef __CC_ARM
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wreturn-type"
#pragma GCC diagnostic ignored "-Wformat"
#endif

/* Private functions definition-----------------------------------------------*/
static void *DjiTest_WidgetWorkTask(void *arg)
{
    T_DjiOsalHandler                    *osalHandler = DjiPlatform_GetOsalHandler();
    T_DjiReturnCode                     djiStat;
    char                                message[DJI_WIDGET_FLOATING_WINDOW_MSG_MAX_LEN];

    USER_UTIL_UNUSED(arg);

    while (1) {
        uint32_t                        sysTimeMs = 0;
        uint16_t                        current = 0;

        djiStat = osalHandler->GetTimeMs(&sysTimeMs);
        if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            WIDGET_LOG_ERR("Get system time ms error, stat = 0x%08llX", djiStat);
        }
        current += snprintf(message, DJI_WIDGET_FLOATING_WINDOW_MSG_MAX_LEN, "[%u.%u]\n", sysTimeMs / 1000, sysTimeMs % 1000);

        DjiTest_WidgetBusinessClearLog(sysTimeMs);
        (void)DjiTest_WidgetBusinessGetLog(message + current, DJI_WIDGET_FLOATING_WINDOW_MSG_MAX_LEN - current);
        djiStat = DjiWidgetFloatingWindow_ShowMessage(message);
        if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            WIDGET_LOG_ERR("Floating window show message error, stat = 0x%08llX", djiStat);
        }

        // osalHandler->TaskSleepMs(1000);
        (void)DjiTest_WidgetWaitForWorkWithTimeOut(1000);
    }
}

#ifndef __CC_ARM
#pragma GCC diagnostic pop
#endif

static inline E_DjiWidgetType Dji_Test_WidgetGetTypeEnum(const char *widgetTypeStr)
{
    uint32_t i = sizeof(s_widgetTypeStringMapping)/sizeof(char*) - 1;
    for ( ; i > 0; i --) {
        if (0 == strcmp( s_widgetTypeStringMapping[i], widgetTypeStr)) {
            break;
        }
    }
    return i;
}

static inline const char *Dji_Test_WidgetGetTypeName(E_DjiWidgetType widgetType)
{
    uint32_t i = sizeof(s_widgetTypeStringMapping)/sizeof(char*);
    if (i > widgetType) {
        return s_widgetTypeStringMapping[widgetType];
    }
    return s_widgetTypeStringMapping[0];
}

/* widgets cannot be blocked now,
   so use the general set interface to create background tasks to actually perform actions
*/
static T_DjiReturnCode DjiTest_WidgetWaitForWorkWithTimeOut(uint32_t timeMs)
{
    T_UserWidgetBasicData           *widget = NULL;

    UTIL_OSAL_SEM_WAIT_FOR( s_widgetTestWorkSem, timeMs );
    while ( false == UTIL_LINKLIST_IS_EMPTY(s_widgetTestWorkQue))
    {
        UTIL_OSAL_MUTEX_LOCK_MAY_RETURN( s_widgetTestWorkQueMutex );
        widget = (T_UserWidgetBasicData *)( s_widgetTestWorkQue.first->data );
        DjiUserUtil_LinkListRemoveNodeOnly( &s_widgetTestWorkQue, s_widgetTestWorkQue.first );
        UTIL_OSAL_MUTEX_UNLOCK_MAY_RETURN( s_widgetTestWorkQueMutex );

        (void)widget->action( widget ); // TODO: not enough to thread-safe : widget->value maybe change changed trigger_action is running.
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode DjiTestWidget_SetWidgetValue(E_DjiWidgetType widgetType, uint32_t index, int32_t value,
                                                    void *userData)
{
    T_UserWidgetBasicData               *this = (T_UserWidgetBasicData *)userData;

    DjiTest_WidgetLogAppend("Set widget[%d]<%s> value %d->%d", index, Dji_Test_WidgetGetTypeName(widgetType), this->value, value);

    if (NULL == this) {
        WIDGET_LOG_ERR("Dji test widget set value error, userData is NULL");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    this->value = value; // TODO: not enough to thread-safe : widget->value may be using by trigger_action.
    if (NULL != this->action) {
        UTIL_OSAL_MUTEX_LOCK_MAY_RETURN( s_widgetTestWorkQueMutex );
        DjiUserUtil_LinkListAddNodeLast( &s_widgetTestWorkQue, DjiUserUtil_NewListNode( userData ));
        UTIL_OSAL_MUTEX_UNLOCK_MAY_RETURN( s_widgetTestWorkQueMutex );

        UTIL_OSAL_SEM_POST_MAY_RETURN(s_widgetTestWorkSem);
    }
    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode DjiTestWidget_GetWidgetValue(E_DjiWidgetType widgetType, uint32_t index, int32_t *value,
                                                    void *userData)
{
    T_UserWidgetBasicData               *this = (T_UserWidgetBasicData *)userData;

    if (NULL == this) {
        WIDGET_LOG_ERR("Dji test widget set value error, userData is NULL");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    *value = this->value;
    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode DjiTestWidget_SetWidgetValueForCoupled(E_DjiWidgetType widgetType, uint32_t index, int32_t value,
                                                    void *userData)
{
    T_UserWidgetBasicData               *this = (T_UserWidgetBasicData *)userData;

    DjiTest_WidgetLogAppend("Set ganged[%d(%#08x)]<%s> ->%d", index, this->gangedIndex, Dji_Test_WidgetGetTypeName(widgetType), value);

    if (NULL == this) {
        WIDGET_LOG_ERR("Dji test widget set value error, userData is NULL");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    if (TEST_WIDGET_IS_GANGED_JUNIOR(this->gangedIndex)) {
        // for ganged junior member: action key depend on the leading widget, so actual value should save in action status of leading widget.
        T_UserWidgetBasicData           *leader  = this->leaderData;
        switch ( this->coupledInfo.gang_type ) {
            case USER_WIDGET_SWITCH_COUPLE_TO_LIST:
                {
                    T_UserWidgetListPrivateData *leaderListPri = (T_UserWidgetListPrivateData *)leader->pData;
                    WIDGET_ASSERT(leaderListPri->itemCount > leader->value);
                    // set real status by action: // leaderListPri->itemStatus[leader->value] = (DJI_WIDGET_SWITCH_STATE_ON == value) ? DJI_WIDGET_SWITCH_STATE_ON : DJI_WIDGET_SWITCH_STATE_OFF;
                }
                break;

            case USER_WIDGET_LIST_COUPLE_TO_LIST:
            case USER_WIDGET_INT_INPUT_COUPLE_TO_INT:
            default:
                break;
        }
    }

    // also be necessary to save the operation value of the control normally so that the junior widget can follow the config of leader widget to perform action task.
    this->value = value; // TODO: not enough to thread-safe : widget->value may be using by trigger_action.
    if (NULL != this->action) {
        /* TODO: V1-protocol stack of PSDK can't be pending so we do some action on background, and then we can't get failure results immediately.
            but UI of pilot will changed by set result and can't be refreshed by DjiWidget_PushWidgetState
            for some action like DjiFcSubscription_SubscribeTopic test, it can't be turn off. So we will return failure by special judgment
            This's ugly.
        */
        if (( DjiTestWidget_FcSubscriptionBatchCoupledSwitchAction == this->action)
            && DJI_WIDGET_TYPE_SWITCH == widgetType
            && DJI_WIDGET_SWITCH_STATE_OFF == value ) {
                this->value = DJI_WIDGET_SWITCH_STATE_ON;
            return DJI_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT_IN_CURRENT_STATE;
        }

        UTIL_OSAL_MUTEX_LOCK_MAY_RETURN( s_widgetTestWorkQueMutex );
        if ( TEST_WIDGET_WORK_QUE_MAX > s_widgetTestWorkQue.count ) {
            DjiUserUtil_LinkListAddNodeLast( &s_widgetTestWorkQue, DjiUserUtil_NewListNode( userData ));
        }
        UTIL_OSAL_MUTEX_UNLOCK_MAY_RETURN( s_widgetTestWorkQueMutex );

        UTIL_OSAL_SEM_POST_MAY_RETURN(s_widgetTestWorkSem);
    }
    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode DjiTestWidget_GetWidgetValueForCoupled(E_DjiWidgetType widgetType, uint32_t index, int32_t *value,
                                                    void *userData)
{
    T_UserWidgetBasicData               *this = (T_UserWidgetBasicData *)userData;

    if (NULL == this) {
        WIDGET_LOG_ERR("Dji test widget get value error, userData is NULL");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    if (TEST_WIDGET_IS_GANGED_JUNIOR(this->gangedIndex)) {
        // for ganged junior member: action key depend on the leading widget, so actual value saved in action status of leading widget.
        T_UserWidgetBasicData           *leader = ((T_UserWidgetBasicData *)userData)->leaderData;
        switch ( this->coupledInfo.gang_type ) {
            case USER_WIDGET_SWITCH_COUPLE_TO_LIST:
                {
                    T_UserWidgetListPrivateData *leaderListPri = (T_UserWidgetListPrivateData *)leader->pData;
                    WIDGET_ASSERT(leaderListPri->itemCount > leader->value);
                    *value = leaderListPri->itemStatus[leader->value];
                }
                break;

            case USER_WIDGET_LIST_COUPLE_TO_LIST:
            case USER_WIDGET_INT_INPUT_COUPLE_TO_INT:
            default:
                *value = this->value;
                break;
        }
    } else {
        *value = this->value;
    }
    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}



// TODO: read ELF and parse function symble instead of hard-code table.
struct T_UserWorkCallBackTable {
    const char*                         funcName;
    T_UserWorkHandle                    callBack;
};
#define TEST_WIDGET_WORK_ACTION_ITEM(f) { .funcName = #f, .callBack = f}
static struct T_UserWorkCallBackTable s_widgetUserWorkCallBackTable[] = {
    TEST_WIDGET_WORK_ACTION_ITEM(DjiTestWidget_FcSubscriptionTopicCoupledIntInputBoxAction),
    TEST_WIDGET_WORK_ACTION_ITEM(DjiTestWidget_FcSubscriptionBatchCoupledSwitchAction),
    TEST_WIDGET_WORK_ACTION_ITEM(DjiTestWidget_CameraSimulatorButtonAction),
    TEST_WIDGET_WORK_ACTION_ITEM(DjiTestWidget_LiveViewCoupledSwitchAction),
    TEST_WIDGET_WORK_ACTION_ITEM(DjiTestWidget_DepthImageCoupledSwitchAction),
    TEST_WIDGET_WORK_ACTION_ITEM(DjiTestWidget_HMSIntInputBoxAction),
};
static T_UserWorkHandle Dji_Test_WidgetGetTriggerActionByJson(cJSON *jsonWidgetUserData)
{
    int32_t                             i = sizeof(s_widgetUserWorkCallBackTable)/sizeof(struct T_UserWorkCallBackTable) - 1;
    cJSON                               *jsonValue = NULL;

    jsonValue = cJSON_GetObjectItem(jsonWidgetUserData, "trigger_action");
    if ( false == cJSON_IsString(jsonValue) ) {
        WIDGET_LOG_DBG("cJSON_GetObjectItem \"%s\" failed: need string obj.", "trigger_action");
        return NULL;
    }

    for ( ; i >= 0; i --) {
        if (0 == strcmp( s_widgetUserWorkCallBackTable[i].funcName, jsonValue->valuestring)) {
            return s_widgetUserWorkCallBackTable[i].callBack;
        }
    }
    return NULL;
}

static T_DjiReturnCode Dji_Test_WidgetFillingUserDataByJson(T_UserWidgetBasicData *widgetUserData, cJSON *jsonWidgetUserData)
{
    int32_t                             i = sizeof(s_widgetUserWorkCallBackTable)/sizeof(struct T_UserWorkCallBackTable) - 1;
    cJSON                               *jsonValue = NULL;

    jsonValue = cJSON_GetObjectItem(jsonWidgetUserData, "ganged_index");
    if (cJSON_IsNumber(jsonValue)) {
        widgetUserData->gangedIndex = jsonValue->valueint;
    } else {
        widgetUserData->gangedIndex = 0;
    }

    widgetUserData->action = Dji_Test_WidgetGetTriggerActionByJson(jsonWidgetUserData);
    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static void DjiTestWidget_FormatCoupledWidget(T_DjiWidgetHandlerList *widgetHandlerList)
{
    T_DjiOsalHandler                    *osalHandler = DjiPlatform_GetOsalHandler();
    T_DjiWidgetHandlerListItem          *thisWidget = NULL;
    T_UserWidgetBasicData               *thisData = NULL;

    thisWidget = widgetHandlerList->items;
    for ( int32_t i = 0; i < widgetHandlerList->count; i ++, thisWidget ++) {
        thisData = (T_UserWidgetBasicData *)thisWidget->userData;

        WIDGET_LOG_VER( "widget[%d]<%s>(%#08x) config %s ganged%s%s.\n parse_test={self_index:%d minor_type:\"%s\" leader_idx:%d major_type:\"%s\"}"
                        , thisWidget->widgetIndex
                        , Dji_Test_WidgetGetTypeName(thisWidget->widgetType)
                        , thisData->gangedIndex
                        , TEST_WIDGET_IS_VALID_GANGED(thisData->gangedIndex, thisWidget->widgetType) ? "is" : "isn't"
                        , TEST_WIDGET_IS_GANGED_LEADER(thisData->gangedIndex) ? " leader" : ""
                        , TEST_WIDGET_IS_GANGED_JUNIOR(thisData->gangedIndex) ? " junior" : ""
                        , thisData->coupledInfo.unique_idx
                        , Dji_Test_WidgetGetTypeName(thisData->coupledInfo.minor_type)
                        , thisData->coupledInfo.leader_idx
                        , Dji_Test_WidgetGetTypeName(thisData->coupledInfo.major_type)
        );

        T_DjiWidgetHandlerListItem      *leaderWidget = NULL;
        // widget config inspection by attributes
        if (false == TEST_WIDGET_IS_GANGED(thisData->gangedIndex)) {
            // normal widget : nothing need todo.
            continue;
        } else if (TEST_WIDGET_IS_GANGED_LEADER(thisData->gangedIndex)) {
            leaderWidget = thisWidget;
        } else if (TEST_WIDGET_IS_GANGED_JUNIOR(thisData->gangedIndex)) {
            // find leader widget 
            T_CoupledInfo               leaderInfo = thisData->coupledInfo;
            int32_t                     index;

            leaderInfo.unique_idx = leaderInfo.leader_idx;
            leaderWidget = widgetHandlerList->items;
            for (index = 0; index < widgetHandlerList->count; index ++, leaderWidget ++) {
                if ( *(uint32_t *)&leaderInfo == ((T_UserWidgetBasicData *)leaderWidget->userData)->gangedIndex) {
                    break;
                }
            }
            if (index >= widgetHandlerList->count) {
                WIDGET_LOG_DBG( "Incorrect widget[%d]<%s> ganged[%d(%#08x)] config, can't find leader widget[%d(%#08x)]<%s>."
                                , thisWidget->widgetIndex
                                , Dji_Test_WidgetGetTypeName(thisWidget->widgetType)
                                , thisData->gangedIndex
                                , thisData->gangedIndex
                                , *(uint32_t *)&leaderInfo
                                , *(uint32_t *)&leaderInfo
                                , Dji_Test_WidgetGetTypeName(leaderInfo.major_type)
                );
                // earse ganged configuration and disabled action: ganged widget's action maybe need to use leaderData
                thisData->gangedIndex = thisWidget->widgetIndex;
                thisData->action = NULL;
                continue;
            }
        } else {
            WIDGET_LOG_ERR( "Incorrect widget[%d]<%s> ganged(%#08x) config, want to couple <%s> to leader[%d]<%s>. This is [%d] action@%p."
                            , thisWidget->widgetIndex
                            , Dji_Test_WidgetGetTypeName(thisWidget->widgetType)
                            , thisData->gangedIndex
                            , Dji_Test_WidgetGetTypeName(thisData->coupledInfo.minor_type)
                            , thisData->coupledInfo.leader_idx
                            , Dji_Test_WidgetGetTypeName(thisData->coupledInfo.major_type)
                            , thisData->coupledInfo.unique_idx
                            , thisData->action
            );
            // disabled configruation action: ganged widget's action maybe need to use leaderData
            thisData->action = NULL;
            continue;
        }

        /* correct ganged config and have finded leader widget
        *  recheck ganged type at last.
        */
        WIDGET_ASSERT( thisData->coupledInfo.major_type == leaderWidget->widgetType);
        WIDGET_ASSERT( thisData->coupledInfo.leader_idx == leaderWidget->widgetIndex);
        WIDGET_ASSERT( thisData->coupledInfo.unique_idx == thisWidget->widgetIndex);
        switch ( thisData->coupledInfo.gang_type ) {
            case USER_WIDGET_SWITCH_COUPLE_TO_LIST:
            case USER_WIDGET_LIST_COUPLE_TO_LIST:
            case USER_WIDGET_INT_INPUT_COUPLE_TO_INT:
                {
                    thisData->leaderData = leaderWidget->userData;
                    thisWidget->SetWidgetValue = DjiTestWidget_SetWidgetValueForCoupled;
                    thisWidget->GetWidgetValue = DjiTestWidget_GetWidgetValueForCoupled;
                }
                continue;

            default:
                {
                    WIDGET_LOG_ERR("Incorrect configuration: these type do not supported coupling.");
                    // earse ganged configuration and disabled action: ganged widget's action maybe need to use leaderData
                    thisData->gangedIndex = thisWidget->widgetIndex;
                    thisData->action = NULL;
                }
                continue;
        }
    }

    return;
}

static T_DjiReturnCode DjiTestWidget_FillingWidgetHandle(cJSON *jsonWidgetIterm, T_DjiWidgetHandlerListItem *this_widget)
{
    T_DjiReturnCode                     returnCode;
    cJSON                               *jsonValue = NULL;
    cJSON                               *jsonArray = NULL;
    int                                 itemCount;

    if (NULL == this_widget) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    jsonValue = cJSON_GetObjectItem(jsonWidgetIterm, "widget_index");
    if ( false == cJSON_IsNumber(jsonValue) ) {
        WIDGET_LOG_ERR("cJSON_GetObjectItem \"%s\" failed: need number obj.", "widget_index");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }
    this_widget->widgetIndex = jsonValue->valueint;

    jsonValue = cJSON_GetObjectItem(jsonWidgetIterm, "widget_type");
    if ( false == cJSON_IsString(jsonValue) ) {
        WIDGET_LOG_ERR("cJSON_GetObjectItem \"%s\" failed: need string obj.", "widget_type");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }
    this_widget->widgetType = Dji_Test_WidgetGetTypeEnum(jsonValue->valuestring);

    // Dji_Test_WidgetCreateDefaultUserDataByJson(cJSON *jsonWidgetIterm)
    T_DjiOsalHandler                    *osalHandler = DjiPlatform_GetOsalHandler();
    T_UserWidgetBasicData               *widgetUserData = NULL; // If the interface want to return unsuccessful, widgetUserData must be free at first.
    {
        switch (this_widget->widgetType) {
            case DJI_WIDGET_TYPE_BUTTON:
            case DJI_WIDGET_TYPE_SWITCH:
            case DJI_WIDGET_TYPE_SCALE:
            case DJI_WIDGET_TYPE_INT_INPUT_BOX:
                {
                    widgetUserData = osalHandler->Malloc(sizeof(T_UserWidgetBasicData));
                    if (NULL == widgetUserData) {
                        WIDGET_LOG_ERR("memry alloc dafault widgetUserData for widget:%d(%#08x) failed!", this_widget->widgetIndex, this_widget->widgetIndex);
                        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
                    }
                    memset(widgetUserData, 0, sizeof(T_UserWidgetBasicData));
                    // widgetUserData->pDataLen = 0;
                }
                break;

            case DJI_WIDGET_TYPE_LIST:
                {
                    jsonArray = cJSON_GetObjectItem(jsonWidgetIterm, "list_item");
                    if ( false == cJSON_IsArray(jsonArray) ) {
                        WIDGET_LOG_ERR("cJSON_GetObjectItem \"%s\" failed : need array obj.", "widget_list");
                        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
                    }
                    itemCount = cJSON_GetArraySize(jsonArray);

                    widgetUserData = osalHandler->Malloc(sizeof(T_UserWidgetBasicData) + sizeof(T_UserWidgetListPrivateData) + itemCount * sizeof(uint32_t));
                    if (NULL == widgetUserData) {
                        WIDGET_LOG_ERR("memry alloc dafault widgetUserData for widget:%d(%#08x) failed!", this_widget->widgetIndex, this_widget->widgetIndex);
                        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
                    }
                    memset(widgetUserData, 0, sizeof(T_UserWidgetBasicData) + sizeof(T_UserWidgetListPrivateData) + itemCount * sizeof(uint32_t));
                    widgetUserData->pDataLen = sizeof(T_UserWidgetListPrivateData) + itemCount * sizeof(uint32_t);
                    T_UserWidgetListPrivateData *listPri = (T_UserWidgetListPrivateData *)widgetUserData->pData;
                    listPri->itemCount = itemCount;
                }
                break;

            default:
                WIDGET_LOG_WAR("Json config error: widget type do not supported!");
                return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_REQUEST_PARAMETER;;
        }
        Dji_Test_WidgetFillingUserDataByJson(widgetUserData, cJSON_GetObjectItem(jsonWidgetIterm, "user_data"));
    }
    // WIDGET_ASSERT( widgetUserData->coupledInfo.unique_idx == this_widget->widgetIndex);

    this_widget->userData = widgetUserData;
    this_widget->SetWidgetValue = DjiTestWidget_SetWidgetValue;
    this_widget->GetWidgetValue = DjiTestWidget_GetWidgetValue;
    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static void DjiTest_WidgetHandlerListDestory(T_DjiWidgetHandlerList *widgetHandlerList)
{
    T_DjiOsalHandler                    *osalHandler = DjiPlatform_GetOsalHandler();

    if (NULL != widgetHandlerList) {
        if (NULL != widgetHandlerList->items) {
            for ( uint32_t i = 0; i < widgetHandlerList->count; i ++) {
                WIDGET_LOG_VER("%3d[%p]. userData@%p", i, &widgetHandlerList->items[i], widgetHandlerList->items[i].userData);
                if (NULL != widgetHandlerList->items[i].userData) {
                    osalHandler->Free(widgetHandlerList->items[i].userData);
                    widgetHandlerList->items[i].userData = NULL;
                }
            }
            WIDGET_LOG_VER("          ->%d items @ %p", widgetHandlerList->count, widgetHandlerList->items);
            osalHandler->Free(widgetHandlerList->items);
            widgetHandlerList->items = NULL;
        }
        WIDGET_LOG_VER(" widgetHandlerList @ %p", widgetHandlerList);
        osalHandler->Free(widgetHandlerList);
        widgetHandlerList = NULL;
    }
    return;
}

static T_DjiWidgetHandlerList *DjiTest_WidgetHandlerListCreateByJsonData(const char *widgetConfigFileName, const uint8_t *jsonData)
{
    T_DjiOsalHandler                    *osalHandler = DjiPlatform_GetOsalHandler();
    cJSON                               *jsonRoot = NULL;
    cJSON                               *jsonItem = NULL;
    cJSON                               *jsonArray = NULL;
    cJSON                               *jsonValue = NULL;
    cJSON                               *mainWidgetJsonArray = NULL;
    cJSON                               *configWidgetJsonArray = NULL;

    T_DjiWidgetHandlerList              *widgetHandlerList = NULL;
    T_DjiWidgetHandlerListItem          *widgetHandler = NULL;
    T_UserWidgetBasicData               *widgetUserData = 0;
    T_DjiReturnCode                     returnCode;
    int                                 itemCount = 0, i = 0;

    // cJSON_Hooks hooks = {
    //     .malloc_fn = (void *(*)(size_t)) osalHandler->Malloc,
    //     .free_fn = osalHandler->Free
    // };
    // cJSON_InitHooks(&hooks); // isn't safe for reentry.

    jsonRoot = cJSON_Parse((char *)jsonData);
    if (jsonRoot == NULL) {
        WIDGET_LOG_ERR("cJSON_Parse \"%s\" failed.", widgetConfigFileName);
        return NULL;
    }

    jsonItem = cJSON_GetObjectItem(jsonRoot, "main_interface");
    if (jsonItem == NULL) {
        WIDGET_LOG_ERR("cJSON_GetObjectItem \"%s\" failed.", "main_interface");
        goto ERR_NEED_FREE_JSON_ROOT;
    }
    jsonArray = cJSON_GetObjectItem(jsonItem, "widget_list");
    if ( false == cJSON_IsArray(jsonArray) ) {
        WIDGET_LOG_ERR("cJSON_GetObjectItem \"%s\" failed : need array obj.", "widget_list");
        goto ERR_NEED_FREE_JSON_ROOT;
    }
    mainWidgetJsonArray = jsonArray;

    jsonItem = cJSON_GetObjectItem(jsonRoot, "config_interface");
    if (jsonItem == NULL) {
        WIDGET_LOG_ERR("cJSON_GetObjectItem \"%s\" failed.", "config_interface");
        goto ERR_NEED_FREE_JSON_ROOT;
    }
    jsonArray = cJSON_GetObjectItem(jsonItem, "widget_list");
    if ( false == cJSON_IsArray(jsonArray) ) {
        WIDGET_LOG_ERR("cJSON_GetObjectItem \"%s\" failed : need array obj.", "widget_list");
        goto ERR_NEED_FREE_JSON_ROOT;
    }
    configWidgetJsonArray = jsonArray;

    widgetHandlerList         = osalHandler->Malloc(sizeof(T_DjiWidgetHandlerList));
    memset(widgetHandlerList, 0, sizeof(T_DjiWidgetHandlerList));
    widgetHandlerList->items = osalHandler->Malloc(sizeof(T_DjiWidgetHandlerListItem) * (cJSON_GetArraySize(mainWidgetJsonArray) + cJSON_GetArraySize(configWidgetJsonArray)));
    memset(widgetHandlerList->items, 0, (cJSON_GetArraySize(mainWidgetJsonArray) + cJSON_GetArraySize(configWidgetJsonArray)));
    WIDGET_LOG_VER(" widgetHandlerList @ %p", widgetHandlerList);
    WIDGET_LOG_VER("       ->%3d items @ %p", widgetHandlerList->count, widgetHandlerList->items);

    widgetHandler = &widgetHandlerList->items[widgetHandlerList->count];
    itemCount = cJSON_GetArraySize(mainWidgetJsonArray);
    WIDGET_LOG_VER(" mainWidgetJsonArray itemCount %d begin @ %d[%p] .", itemCount, widgetHandlerList->count, widgetHandler);
    for (i = 0; i < itemCount; i ++) {
        jsonItem = cJSON_GetArrayItem(mainWidgetJsonArray, i);

        returnCode = DjiTestWidget_FillingWidgetHandle(jsonItem, widgetHandler);
        if (DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS != returnCode) {
            WIDGET_LOG_ERR("FillingWidgetHandle for mainWidgetJsonArray[%d] failed!", i);
            goto ERR_NEED_FREE_HANDLE_LIST;
        }
        WIDGET_LOG_VER("%3d[%p]. userData@%p", widgetHandlerList->count, widgetHandler, widgetHandler->userData);
        widgetHandlerList->count ++;
        widgetHandler ++;
    }

    // widgetHandler = &widgetHandlerList->items[widgetHandlerList->count];
    itemCount = cJSON_GetArraySize(configWidgetJsonArray);
    WIDGET_LOG_VER(" configWidgetJsonArray itemCount %d begin @ %d[%p] .", itemCount, widgetHandlerList->count, widgetHandler);
    for (i = 0; i < itemCount; i ++) {
        jsonItem = cJSON_GetArrayItem(configWidgetJsonArray, i);

        returnCode = DjiTestWidget_FillingWidgetHandle(jsonItem, widgetHandler);
        if (DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS != returnCode) {
            WIDGET_LOG_ERR("FillingWidgetHandle for configWidgetJsonArray[%d] failed!", i);
            goto ERR_NEED_FREE_HANDLE_LIST;
        }
        WIDGET_LOG_VER("%3d[%p]. userData@%p", widgetHandlerList->count, widgetHandler, widgetHandler->userData);
        widgetHandlerList->count ++;
        widgetHandler ++;
    }

    WIDGET_LOG_VER("Obtained %d widgets.", widgetHandlerList->count);
    DjiTestWidget_FormatCoupledWidget(widgetHandlerList);
    WIDGET_LOG_VER("Formated %d widgets.", widgetHandlerList->count);
    cJSON_Delete(jsonRoot);
    return widgetHandlerList;

  ERR_NEED_FREE_HANDLE_LIST:
    DjiTest_WidgetHandlerListDestory(widgetHandlerList);
  ERR_NEED_FREE_JSON_ROOT:
    cJSON_Delete(jsonRoot);
    return NULL;
}

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
