/**
 ********************************************************************
 * @file    test_widget_action.c
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
#define TEST_WIDGET_ACTION_C

/* Includes ------------------------------------------------------------------*/
#include "test_widget_action.h"
#include "dji_sdk_config.h"
#include <dji_logger.h>
#include <dji_platform.h>
#include <dji_fc_subscription.h>
#include "camera_emu/test_payload_cam_emu_media.h"
#include "camera_emu/test_payload_cam_emu_base.h"
#include <dji_liveview.h>
#include <dji_perception.h>
#include <dji_hms.h>
#include "hms/test_hms.h"

/* Private constants ---------------------------------------------------------*/
extern T_DjiReturnCode DjiTest_WidgetBusinessPushLog(const char *fmt, ...);
#define DjiTest_WidgetLogAppend(...)        (void)DjiTest_WidgetBusinessPushLog(__VA_ARGS__)

// #define DJI_STAT_FUNC_NOT_RETURN(func, ...)                {  \
//     if (DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS != func(__VA_ARGS__)) { \
//         USER_LOG_WARN("[ERR]:%s(%s) failed.", #func, #__VA_ARGS__); \
//     } \
// }

#define DJI_STAT_CHECK_MAY_RETURN(djiStat, func, ...)       {  \
    djiStat = func(__VA_ARGS__); \
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) { \
        DjiTest_WidgetLogAppend("[ERR]:%s(%s) ret:%d(%#08X).", #func, #__VA_ARGS__, djiStat, djiStat); \
        return; \
    } \
}

#define DJI_STAT_CHECK_MAY_RETURN_RET(djiStat, func, ...)   {  \
    djiStat = func(__VA_ARGS__); \
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) { \
        DjiTest_WidgetLogAppend("[ERR]:%s ret:%d(%#08X).", #func, djiStat, djiStat); \
        return djiStat; \
    } \
}

#define TEST_ACTION_FC_SUB_MAX_LEVEL        (3)

// mermber count of E_DjiLiveViewCameraPosition
#define TEST_ACTION_LIVEVIEW_SOURCE_CNT     (4)
// max recorder liveview frame count
#define TEST_ACTION_LIVEVIEW_RECORD_MAX     (300)
#define TEST_ACTION_LIVEVIEW_DOWNSAMPLE     (150)

#define TEST_PERCEPTION_SAVE_IMAGE_MAX      (10)
#define TEST_PERCEPTION_IMAGE_DOWNSAMPLE    (100)

#define CONFIG_TEST_WIDGET_ENABLE_RECORDER_FILE false

#if CONFIG_TEST_WIDGET_ENABLE_RECORDER_FILE
    #define TEST_ACTION_FILE_NAME_MAX_SIZE  (256)

    #define TEST_EXECUTED_LOG_ROOT_FOLDER   "./Logs/"

    #define TEST_ACTION_FILE_ROOT_FOLDER    TEST_EXECUTED_LOG_ROOT_FOLDER "test_action/"

    #define TEST_LIVEVIEW_RECORDER_FILE_DIR TEST_ACTION_FILE_ROOT_FOLDER "liveview_record/"
    #define TEST_PERCEPTION_DEPTH_IMAGE_DIR TEST_ACTION_FILE_ROOT_FOLDER "depth_image/"
#endif

#define DJI_HMS_PSDK_DAPARTMENT_ID          (0x1E)
#define DJI_HMS_PSDK_MODULE_ID              (0x02)

/* Private types -------------------------------------------------------------*/
typedef struct tagT_UserTestFcSubTopicAttr {
    E_DjiFcSubscriptionTopic                id;
    E_DjiDataSubscriptionTopicFreq          freq;
    void*                                   feedback;
} T_UserTestFcSubTopicAttr;

typedef struct tagT_UserTestFcSubscriptionBatchLevel {
    uint32_t                                level;
    uint32_t                                count;
    T_UserTestFcSubTopicAttr                *attrs;
} T_UserTestFcSubscriptioBatchnLevel;

typedef struct tagT_UserTestLiveViewRecorder {
    T_DjiMutexHandle                        lock;
    #if CONFIG_TEST_WIDGET_ENABLE_RECORDER_FILE
        T_DjiFileHandle                         fd;
    #endif
    uint32_t                                frameCnt;
} T_UserTestLiveViewRecorder;

typedef struct tagT_TestPerceptionDepthImageRecorder {
    E_DjiPerceptionDirection                direction;
    uint32_t                                count;
    char                                    *name;
} T_TestPerceptionDepthImageRecorder;

typedef union tagT_TestUserHmsServerAlarmId {
    uint32_t                                hmsCode;
    struct {
        #if __BYTE_ORDER == __LITTLE_ENDIAN
            uint8_t                         fault;
            uint8_t                         source;
            uint8_t                         module;
            uint8_t                         department;
        #elif __BYTE_ORDER == __BIG_ENDIAN
            uint8_t                         department;
            uint8_t                         module;
            uint8_t                         source;
            uint8_t                         fault;
        #else
            #ERROR "No byte order is declared!"
        #endif
    };
} T_TestUserHmsServerAlarmId;

/* Private functions declaration ---------------------------------------------*/

/* Private values ------------------------------------------------------------*/
static T_UserTestFcSubTopicAttr             s_fcSubTopicAttrsMoreL2ThenL1[] = {
    // topics of subscriptions that is more high-level subscriptions than mid-level
    { DJI_FC_SUBSCRIPTION_TOPIC_BATTERY_SINGLE_INFO_INDEX2  , DJI_DATA_SUBSCRIPTION_TOPIC_5_HZ  },
    { DJI_FC_SUBSCRIPTION_TOPIC_BATTERY_SINGLE_INFO_INDEX1  , DJI_DATA_SUBSCRIPTION_TOPIC_5_HZ  },
    { DJI_FC_SUBSCRIPTION_TOPIC_HOME_POINT_INFO             , DJI_DATA_SUBSCRIPTION_TOPIC_5_HZ  },
    { DJI_FC_SUBSCRIPTION_TOPIC_HOME_POINT_SET_STATUS       , DJI_DATA_SUBSCRIPTION_TOPIC_5_HZ  },
    { DJI_FC_SUBSCRIPTION_TOPIC_AVOID_DATA                  , DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ },
    { DJI_FC_SUBSCRIPTION_TOPIC_POSITION_VO                 , DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ },
    { DJI_FC_SUBSCRIPTION_TOPIC_FLIGHT_ANOMALY              , DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ },
    { DJI_FC_SUBSCRIPTION_TOPIC_RTK_CONNECT_STATUS          , DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ },
    { DJI_FC_SUBSCRIPTION_TOPIC_STATUS_MOTOR_START_ERROR    , DJI_DATA_SUBSCRIPTION_TOPIC_5_HZ  },
    { DJI_FC_SUBSCRIPTION_TOPIC_STATUS_LANDINGGEAR          , DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ },
    { DJI_FC_SUBSCRIPTION_TOPIC_STATUS_DISPLAYMODE          , DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ },
    { DJI_FC_SUBSCRIPTION_TOPIC_STATUS_FLIGHT               , DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ },
    { DJI_FC_SUBSCRIPTION_TOPIC_RC                          , DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ },
};

static T_UserTestFcSubTopicAttr             s_fcSubTopicAttrsMoreL1ThenL0[] = {
    // topics of subscriptions that is more mid-level subscriptions than low-level
    { DJI_FC_SUBSCRIPTION_TOPIC_COMPASS                     , DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ },
    { DJI_FC_SUBSCRIPTION_TOPIC_RTK_YAW_INFO                , DJI_DATA_SUBSCRIPTION_TOPIC_5_HZ  },
    { DJI_FC_SUBSCRIPTION_TOPIC_RTK_POSITION_INFO           , DJI_DATA_SUBSCRIPTION_TOPIC_5_HZ  },
    { DJI_FC_SUBSCRIPTION_TOPIC_RTK_YAW                     , DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ },
    { DJI_FC_SUBSCRIPTION_TOPIC_RTK_VELOCITY                , DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ },
    { DJI_FC_SUBSCRIPTION_TOPIC_RTK_POSITION                , DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ },
    { DJI_FC_SUBSCRIPTION_TOPIC_GPS_DETAILS                 , DJI_DATA_SUBSCRIPTION_TOPIC_5_HZ  },
    { DJI_FC_SUBSCRIPTION_TOPIC_GPS_VELOCITY                , DJI_DATA_SUBSCRIPTION_TOPIC_5_HZ  },
    { DJI_FC_SUBSCRIPTION_TOPIC_GPS_POSITION                , DJI_DATA_SUBSCRIPTION_TOPIC_5_HZ  },
    { DJI_FC_SUBSCRIPTION_TOPIC_GPS_TIME                    , DJI_DATA_SUBSCRIPTION_TOPIC_5_HZ  },
    { DJI_FC_SUBSCRIPTION_TOPIC_GPS_DATE                    , DJI_DATA_SUBSCRIPTION_TOPIC_5_HZ  },
    { DJI_FC_SUBSCRIPTION_TOPIC_ALTITUDE_OF_HOMEPOINT       , DJI_DATA_SUBSCRIPTION_TOPIC_5_HZ  },
};

static T_UserTestFcSubTopicAttr             s_fcSubTopicAttrsL0[] = {
    // topics of subscriptions that is more low-level subscriptions than least test
    { DJI_FC_SUBSCRIPTION_TOPIC_ESC_DATA                    , DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ },
    { DJI_FC_SUBSCRIPTION_TOPIC_RC_WITH_FLAG_DATA           , DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ },
    { DJI_FC_SUBSCRIPTION_TOPIC_HARD_SYNC                   , DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ },
    { DJI_FC_SUBSCRIPTION_TOPIC_CONTROL_DEVICE              , DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ },
    { DJI_FC_SUBSCRIPTION_TOPIC_BATTERY_INFO                , DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ },
    { DJI_FC_SUBSCRIPTION_TOPIC_GIMBAL_STATUS               , DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ },
    { DJI_FC_SUBSCRIPTION_TOPIC_GIMBAL_ANGLES               , DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ },
    { DJI_FC_SUBSCRIPTION_TOPIC_POSITION_FUSED              , DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ },
    { DJI_FC_SUBSCRIPTION_TOPIC_HEIGHT_RELATIVE             , DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ },
    { DJI_FC_SUBSCRIPTION_TOPIC_ALTITUDE_FUSED              , DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ },
    { DJI_FC_SUBSCRIPTION_TOPIC_ANGULAR_RATE_RAW            , DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ },
    { DJI_FC_SUBSCRIPTION_TOPIC_ANGULAR_RATE_FUSIONED       , DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ },
    { DJI_FC_SUBSCRIPTION_TOPIC_VELOCITY                    , DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ },
    { DJI_FC_SUBSCRIPTION_TOPIC_ACCELERATION_RAW            , DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ },
    { DJI_FC_SUBSCRIPTION_TOPIC_ACCELERATION_BODY           , DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ },
    { DJI_FC_SUBSCRIPTION_TOPIC_ACCELERATION_GROUND         , DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ },
};

T_UserTestFcSubscriptioBatchnLevel          fcSubBatchLevelTable[TEST_ACTION_FC_SUB_MAX_LEVEL] = {
    { 0, sizeof(s_fcSubTopicAttrsL0)/sizeof(T_UserTestFcSubTopicAttr)            , s_fcSubTopicAttrsL0              },
    { 1, sizeof(s_fcSubTopicAttrsMoreL1ThenL0)/sizeof(T_UserTestFcSubTopicAttr)  , s_fcSubTopicAttrsMoreL1ThenL0    },
    { 2, sizeof(s_fcSubTopicAttrsMoreL2ThenL1)/sizeof(T_UserTestFcSubTopicAttr)  , s_fcSubTopicAttrsMoreL2ThenL1    },
};

static T_UserTestFcSubTopicAttr             s_fcSubSingleTopicTest = { DJI_FC_SUBSCRIPTION_TOPIC_QUATERNION, DJI_DATA_SUBSCRIPTION_TOPIC_5_HZ, NULL };

static T_UserTestLiveViewRecorder           s_cameraRecorderBank[TEST_ACTION_LIVEVIEW_SOURCE_CNT]; // initialization by MemZero

static T_TestPerceptionDepthImageRecorder   s_depthImageRecorderBank[] = {
    { .direction = DJI_PERCEPTION_RECTIFY_DOWN   , .count = 0, .name = "down"   },
    { .direction = DJI_PERCEPTION_RECTIFY_FRONT  , .count = 0, .name = "front"  },
    { .direction = DJI_PERCEPTION_RECTIFY_REAR   , .count = 0, .name = "rear"   },
    { .direction = DJI_PERCEPTION_RECTIFY_UP     , .count = 0, .name = "up"     },
    { .direction = DJI_PERCEPTION_RECTIFY_LEFT   , .count = 0, .name = "left"   },
    { .direction = DJI_PERCEPTION_RECTIFY_RIGHT  , .count = 0, .name = "right"  },
};

/* Exported functions definition ---------------------------------------------*/

static T_DjiReturnCode DjiTestWidget_FcSubscriptionFreqStatisFeedCallback(const uint8_t *data, uint16_t dataSize, const T_DjiDataTimestamp *timestamp)
{// TODO: non-reentrant when last topic isn't auto unsub
    static T_DjiDataTimestamp           cut_ts = {0, 0};
    static int32_t                      count = 0;

    T_UserTestFcSubTopicAttr            *topicAttr = &s_fcSubSingleTopicTest;
    int32_t                             *freqFeedBack = topicAttr->feedback;

    if (0 == count) {
        cut_ts.millisecond = timestamp->millisecond + 1000;
        // cut_ts.microsecond = timestamp->microsecond;
        // DjiTest_WidgetLogAppend("->FcSub id:%d 1st@%u(%u)", topicAttr->id, timestamp->millisecond, timestamp->microsecond);
    }
    count ++;

    // TODO: dump msg or parse msg by topicAttr->id.

    if (timestamp->millisecond > cut_ts.millisecond) {
        // DjiTest_WidgetLogAppend("->FcSub id:%d Freq ≈%uHz...", topicAttr->id, count);
        if (NULL != freqFeedBack) {
            *freqFeedBack = count;
        }

        count = 0;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/* work */
T_DjiReturnCode DjiTestWidget_FcSubscriptionTopicCoupledIntInputBoxAction(void *userData)
{// TODO: non-reentrant when last topic isn't auto unsub
    T_DjiOsalHandler                    *osalHandler = DjiPlatform_GetOsalHandler();
    T_UserWidgetBasicData               *this = (T_UserWidgetBasicData *)userData;
    T_UserWidgetBasicData               *leader = this->leaderData;

    T_UserTestFcSubTopicAttr            *topicAttr = &s_fcSubSingleTopicTest;

    T_DjiReturnCode                     djiStat = DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;

    // DJI_STAT_CHECK_MAY_RETURN_RET(djiStat, DjiFcSubscription_Init);

    (void)DjiFcSubscription_UnSubscribeTopic(topicAttr->id);
    osalHandler->TaskSleepMs(400); // lowest frequency DJI_DATA_SUBSCRIPTION_TOPIC_5_HZ

    topicAttr->id = leader->value;
    switch (this->value) {
        case DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ:
        case DJI_DATA_SUBSCRIPTION_TOPIC_5_HZ:
        case DJI_DATA_SUBSCRIPTION_TOPIC_10_HZ:
        case DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ:
        case DJI_DATA_SUBSCRIPTION_TOPIC_100_HZ:
        case DJI_DATA_SUBSCRIPTION_TOPIC_200_HZ:
        case DJI_DATA_SUBSCRIPTION_TOPIC_400_HZ:
            topicAttr->freq = this->value;
            break;

        default:
            {
                topicAttr->freq = 0;
                DjiTest_WidgetLogAppend("->FcSub id:%d freq:%d unsupported, forced to %uHz(unsubscription)", topicAttr->id, this->value, topicAttr->freq);
            }
            break;
    }
    DjiTest_WidgetLogAppend("->FcSub id:%d freq:%d", topicAttr->id, this->value);

    this->value = 0;
    if (topicAttr->freq) {
        // start subscription
        topicAttr->feedback = &this->value;
        DJI_STAT_CHECK_MAY_RETURN_RET(djiStat, DjiFcSubscription_SubscribeTopic, topicAttr->id, topicAttr->freq, DjiTestWidget_FcSubscriptionFreqStatisFeedCallback);
    }

    // DJI_STAT_CHECK_MAY_RETURN_RET(djiStat, DjiFcSubscription_DeInit);
    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiTestWidget_FcSubscriptionBatchCoupledSwitchAction(void *userData)
{
    #define SUPPORTED_FC_SUB_DEINIT     false
    #define SUPPORTED_UNSUBSUBSCRIBE    true
    // tactic: DJI_FC_SUBSCRIPTION_TOPIC_WITHOUT_CALLBACK_AND_NOT_RETURN
    #define DJI_FC_SUBSCRIPTION_TOPIC_TEST(id, freq)     {                                              \
        if (DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS != DjiFcSubscription_SubscribeTopic(id, freq, NULL)) { \
            USER_LOG_WARN("Subscribe topic id:%-3d-%dHz error.", id, freq);                             \
        }                                                                                               \
    }

    T_DjiOsalHandler                    *osalHandler = DjiPlatform_GetOsalHandler();
    T_UserWidgetBasicData               *this = (T_UserWidgetBasicData *)userData;
    T_UserWidgetBasicData               *leader = this->leaderData;
    T_UserWidgetListPrivateData         *leaderListPri = (T_UserWidgetListPrivateData *)leader->pData;

    T_UserTestFcSubTopicAttr            *topic = NULL;
    E_DjiWidgetSwitchState              switchMode = DJI_WIDGET_SWITCH_STATE_ON;
    uint8_t                             subLevel = 0;

    T_DjiReturnCode                     djiStat = DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    static bool                         initFlag = false;
    int32_t                             count = 0, i = 0;

    if (! initFlag) {
        DjiTest_WidgetLogAppend("->FcSub init...");
        DJI_STAT_CHECK_MAY_RETURN_RET(djiStat, DjiFcSubscription_Init);
        initFlag = true;
    }

    // TODO: not enough to thread-safe : leader->value && this->value may is being changed or have changed.
    switchMode = this->value;
    subLevel = leader->value;

    if (DJI_WIDGET_SWITCH_STATE_ON == switchMode) { // coupled switch mode on:
        DjiTest_WidgetLogAppend("->FcSub turn on lv.%d", subLevel);
        // legalization subLevel:
        switch (subLevel) { // range: [0, TEST_ACTION_FC_SUB_MAX_LEVEL)
            case 2:
                { // topics of subscriptions that is more high-level subscriptions than mid-level
                    if (DJI_WIDGET_SWITCH_STATE_OFF == leaderListPri->itemStatus[2]) {
                        count = fcSubBatchLevelTable[2].count;
                        topic = fcSubBatchLevelTable[2].attrs;
                        for ( i = 0; i < count; i ++, topic ++) {
                            DJI_FC_SUBSCRIPTION_TOPIC_TEST(topic->id, topic->freq);
                        }
                        leaderListPri->itemStatus[2] = DJI_WIDGET_SWITCH_STATE_ON;
                    }
                }
            case 1:
                { // topics of subscriptions that is more mid-level subscriptions than low-level
                    if (DJI_WIDGET_SWITCH_STATE_OFF == leaderListPri->itemStatus[1]) {
                        count = fcSubBatchLevelTable[1].count;
                        topic = fcSubBatchLevelTable[1].attrs;
                        for ( i = 0; i < count; i ++, topic ++) {
                            DJI_FC_SUBSCRIPTION_TOPIC_TEST(topic->id, topic->freq);
                        }
                        leaderListPri->itemStatus[1] = DJI_WIDGET_SWITCH_STATE_ON;
                    }
                }
            case 0:
                { // topics of subscriptions that is more low-level subscriptions than least test
                    if (DJI_WIDGET_SWITCH_STATE_OFF == leaderListPri->itemStatus[0]) {
                        count = fcSubBatchLevelTable[0].count;
                        topic = fcSubBatchLevelTable[0].attrs;
                        for ( i = 0; i < count; i ++, topic ++) {
                            DJI_FC_SUBSCRIPTION_TOPIC_TEST(topic->id, topic->freq);
                        }
                        leaderListPri->itemStatus[0] = DJI_WIDGET_SWITCH_STATE_ON;
                    }
                }
            default:
                { // least test
                    DJI_FC_SUBSCRIPTION_TOPIC_TEST(DJI_FC_SUBSCRIPTION_TOPIC_QUATERNION, DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ);
                }
                break;
        }
    } else { // coupled switch mode off:
        DjiTest_WidgetLogAppend("->FcSub %sturn off lv.%d", SUPPORTED_UNSUBSUBSCRIBE ? " " : "can't ", subLevel);
        #if SUPPORTED_UNSUBSUBSCRIBE
            switch (subLevel) {
                case 2:
                    { // topics of subscriptions that is more high-level subscriptions than mid-level
                        count = fcSubBatchLevelTable[2].count;
                        topic = fcSubBatchLevelTable[2].attrs;
                        for ( i = 0; i < count; i ++, topic ++) {
                            (void)DjiFcSubscription_UnSubscribeTopic(topic->id);
                        }
                        leaderListPri->itemStatus[2] = DJI_WIDGET_SWITCH_STATE_OFF;
                    }
                case 1:
                    { // topics of subscriptions that is more mid-level subscriptions than low-level
                        count = fcSubBatchLevelTable[1].count;
                        topic = fcSubBatchLevelTable[1].attrs;
                        for ( i = 0; i < count; i ++, topic ++) {
                            (void)DjiFcSubscription_UnSubscribeTopic(topic->id);
                        }
                        leaderListPri->itemStatus[1] = DJI_WIDGET_SWITCH_STATE_OFF;
                    }
                case 0:
                    { // topics of subscriptions that is more low-level subscriptions than least test
                        count = fcSubBatchLevelTable[0].count;
                        topic = fcSubBatchLevelTable[0].attrs;
                        for ( i = 0; i < count; i ++, topic ++) {
                            (void)DjiFcSubscription_UnSubscribeTopic(topic->id);
                        }
                        leaderListPri->itemStatus[0] = DJI_WIDGET_SWITCH_STATE_OFF;
                    }
                default:
                    { // least test
                        (void)DjiFcSubscription_UnSubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_QUATERNION);
                    }
                    break;
            }
        #endif
    }

    #if SUPPORTED_FC_SUB_DEINIT
        DJI_STAT_CHECK_MAY_RETURN_RET(djiStat, DjiFcSubscription_DeInit);
        initFlag = false;
    #endif

    DjiTest_WidgetLogAppend("->FcSub done");
    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiTestWidget_CameraSimulatorButtonAction(void *userData)
{
    T_UserWidgetBasicData               *this = (T_UserWidgetBasicData *)userData;

    T_DjiReturnCode                     djiStat = DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    #ifdef CONFIG_MODULE_SAMPLE_CAMERA_EMU_ON
        static bool                         simCameraInitFlag = true;
    #else
        static bool                         simCameraInitFlag = false;
    #endif
    #ifdef CONFIG_MODULE_SAMPLE_CAMERA_MEDIA_ON
        static bool                         simMediaInitFlag = true;
    #else
        static bool                         simMediaInitFlag = false;
    #endif

    if (! simCameraInitFlag) {
        DjiTest_WidgetLogAppend("->CameraSim init...");
        DJI_STAT_CHECK_MAY_RETURN_RET(djiStat, DjiTest_CameraEmuBaseStartService);
        simCameraInitFlag = true;
    }
    if (! simMediaInitFlag) {
        DJI_STAT_CHECK_MAY_RETURN_RET(djiStat, DjiTest_CameraEmuMediaStartService);
        simMediaInitFlag = true;
    }

    /* StopMediaPlayback is a static function in test_payload_cam_emu_media.c and not thread-safe.
        so payload_camera video can only oprate by DJI_COMMAND_CMDSET_CAMERA,DJI_CAMERA_CMD_ID_VIDEO_PLAYBACK_CTRL.
        force as DJI_WIDGET_BUTTON_STATE_PRESS_DOWN
    */
    DjiTest_WidgetLogAppend("->CameraSim force as PRESS.");
    this->value = DJI_WIDGET_BUTTON_STATE_PRESS_DOWN;
    return djiStat;
}

static void DjiTestWidget_CameraStreamCallback(E_DjiLiveViewCameraPosition srcCamera, const uint8_t *buf, uint32_t bufLen)
{
    T_DjiOsalHandler                    *osalHandler = DjiPlatform_GetOsalHandler();
    T_UserTestLiveViewRecorder          *cameraRecorder = NULL;
    T_DjiReturnCode                     djiStat = DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;

    switch (srcCamera) {
        case DJI_LIVEVIEW_CAMERA_POSITION_NO_1:
            cameraRecorder = &s_cameraRecorderBank[1];
            break;
        case DJI_LIVEVIEW_CAMERA_POSITION_NO_2:
            cameraRecorder = &s_cameraRecorderBank[2];
            break;
        case DJI_LIVEVIEW_CAMERA_POSITION_NO_3:
            cameraRecorder = &s_cameraRecorderBank[3];
            break;
        case DJI_LIVEVIEW_CAMERA_POSITION_FPV:
        default:
            cameraRecorder = &s_cameraRecorderBank[0];
            break;
    }

    if ( 0 == (cameraRecorder->frameCnt % TEST_ACTION_LIVEVIEW_DOWNSAMPLE)) {
        DjiTest_WidgetLogAppend("->LiveView geted %d frame of camera_%d-src_def.", cameraRecorder->frameCnt, srcCamera);
    }

    DJI_STAT_CHECK_MAY_RETURN(djiStat, osalHandler->MutexLock, cameraRecorder->lock);
    cameraRecorder->frameCnt ++;

    #if CONFIG_TEST_WIDGET_ENABLE_RECORDER_FILE
        T_DjiFileSystemHandler              *fsHandler = DjiPlatform_GetFileSystemHandler();
        uint32_t                            size = 0;

        // check for auto stop...
        if ( TEST_ACTION_LIVEVIEW_RECORD_MAX < cameraRecorder->frameCnt) {
            // file size is too large.
            #if 0 // abandon: auto disable may cause thread race of cameraRecorder
                djiStat = DjiLiveview_StopH264Stream(srcCamera, DJI_LIVEVIEW_CAMERA_SOURCE_DEFAULT);
                if (DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS != djiStat) {
                    DjiTest_WidgetLogAppend("[ERR] DjiLiveview_StopH264Stream(%d, %d) ret:%d.", srcCamera, DJI_LIVEVIEW_CAMERA_SOURCE_DEFAULT, djiStat);
                    DJI_STAT_CHECK_MAY_RETURN(djiStat, osalHandler->MutexUnlock, cameraRecorder->lock);
                    return;
                }
                if (DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS != fsHandler->FileClose(cameraRecorder->fd)) {
                    DjiTest_WidgetLogAppend("[ERR] leakage of fd:%d. Close failed!", cameraRecorder);
                }
                cameraRecorder->fd = NULL;
                cameraRecorder->frameCnt = 0;
            #endif
            DJI_STAT_CHECK_MAY_RETURN(djiStat, osalHandler->MutexUnlock, cameraRecorder->lock);
            return;
        }

        if (DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS != fsHandler->FileWrite(cameraRecorder->fd, buf, bufLen, &size)) {
            DjiTest_WidgetLogAppend("[ERR] fd:%d Write failed!", cameraRecorder->fd);
        } else if (bufLen == size) {
            (void)fsHandler->FileSync(cameraRecorder->fd);
        }
    #endif

    DJI_STAT_CHECK_MAY_RETURN(djiStat, osalHandler->MutexUnlock, cameraRecorder->lock);
    return;
}

T_DjiReturnCode DjiTestWidget_LiveViewCoupledSwitchAction(void *userData)
{
    #ifdef CONFIG_MODULE_SAMPLE_CAMERA_MEDIA_ON
    #else
        T_DjiOsalHandler                    *osalHandler = DjiPlatform_GetOsalHandler();
        T_UserWidgetBasicData               *this = (T_UserWidgetBasicData *)userData;
        T_UserWidgetBasicData               *leader = this->leaderData;
        T_UserWidgetListPrivateData         *leaderListPri = (T_UserWidgetListPrivateData *)leader->pData;

        T_UserTestLiveViewRecorder          *cameraRecorder = NULL;
        #if CONFIG_TEST_WIDGET_ENABLE_RECORDER_FILE
            T_DjiFileSystemHandler              *fsHandler = DjiPlatform_GetFileSystemHandler();
            uint32_t                            sysTimeMs = 0;
            uint16_t                            randSuffix = 0;
            char                                fileName[TEST_ACTION_FILE_NAME_MAX_SIZE] = {0};
        #endif
        E_DjiMountPosition                  position = DJI_MOUNT_POSITION_UNKNOWN;
        E_DjiWidgetSwitchState              switchMode = DJI_WIDGET_SWITCH_STATE_ON;
        E_DjiLiveViewCameraPosition         srcCamera = DJI_LIVEVIEW_CAMERA_POSITION_FPV;

        T_DjiReturnCode                     djiStat = DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
        static bool                         initFlag = false;

        DjiTest_WidgetLogAppend("->LiveView");
        if (false == initFlag) {
            DjiTest_WidgetLogAppend("->LiveView init...");
            #if CONFIG_TEST_WIDGET_ENABLE_RECORDER_FILE
                DJI_STAT_CHECK_MAY_RETURN_RET(djiStat, fsHandler->Mkdir, TEST_LIVEVIEW_RECORDER_FILE_DIR);
            #endif
            DJI_STAT_CHECK_MAY_RETURN_RET(djiStat, DjiLiveview_Init);
            memset(s_cameraRecorderBank, 0, sizeof(s_cameraRecorderBank));
            cameraRecorder = s_cameraRecorderBank;
            while (cameraRecorder != &s_cameraRecorderBank[TEST_ACTION_LIVEVIEW_SOURCE_CNT - 1]) {
                // TODO: the mutex-lock descriptor may is leakage.
                DJI_STAT_CHECK_MAY_RETURN_RET(djiStat, osalHandler->MutexCreate, &cameraRecorder->lock);
                cameraRecorder ++;
            }
            initFlag = true;
        }

        // TODO: not enough to thread-safe : leader->value && this->value may is being changed or have changed.
        switchMode = this->value;
        position = leader->value;

        switch (position) {
            case DJI_MOUNT_POSITION_PAYLOAD_PORT_NO1:
                {
                    srcCamera = DJI_LIVEVIEW_CAMERA_POSITION_NO_1;
                    cameraRecorder = &s_cameraRecorderBank[1];
                }
                break;

            case DJI_MOUNT_POSITION_PAYLOAD_PORT_NO2:
                {
                    srcCamera = DJI_LIVEVIEW_CAMERA_POSITION_NO_2;
                    cameraRecorder = &s_cameraRecorderBank[2];
                }
                break;

            case DJI_MOUNT_POSITION_PAYLOAD_PORT_NO3:
                {
                    srcCamera = DJI_LIVEVIEW_CAMERA_POSITION_NO_3;
                    cameraRecorder = &s_cameraRecorderBank[3];
                }
                break;

            case DJI_MOUNT_POSITION_EXTENSION_PORT:
            case DJI_MOUNT_POSITION_UNKNOWN:
            default:
                {
                    srcCamera = DJI_LIVEVIEW_CAMERA_POSITION_FPV;
                    cameraRecorder = &s_cameraRecorderBank[0];
                }
                break;
        }
        DjiTest_WidgetLogAppend("->LiveView turn %s camera:%d...", (DJI_WIDGET_SWITCH_STATE_ON == switchMode) ? "on" : "off", srcCamera);

        if (switchMode == leaderListPri->itemStatus[position]) {
            DjiTest_WidgetLogAppend("[ERR] camera-%02d_src-def has been geted %d frames!", srcCamera, cameraRecorder->frameCnt);
            return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
        }

        if (DJI_WIDGET_SWITCH_STATE_ON == switchMode) { // coupled switch mode on:
            #if CONFIG_TEST_WIDGET_ENABLE_RECORDER_FILE
                if (NULL != cameraRecorder->fd) {
                    // already started recording!
                    DjiTest_WidgetLogAppend("[ERR] camera-%02d_src-def has been recorded into fd:%d, unbelievable!", srcCamera, cameraRecorder->fd);
                    leaderListPri->itemStatus[position] = DJI_WIDGET_SWITCH_STATE_ON;
                    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
                }

                (void)osalHandler->GetTimeMs(&sysTimeMs);
                (void)osalHandler->GetRandomNum(&randSuffix);
                snprintf(fileName, TEST_ACTION_FILE_NAME_MAX_SIZE, TEST_LIVEVIEW_RECORDER_FILE_DIR "camera-%02d_src-def_%08u-%u.h264", srcCamera , sysTimeMs, randSuffix);
                DJI_STAT_CHECK_MAY_RETURN_RET(djiStat, fsHandler->FileOpen, fileName, "ab+", &cameraRecorder->fd);
            #endif
            cameraRecorder->frameCnt = 0;
            DJI_STAT_CHECK_MAY_RETURN_RET(djiStat, DjiLiveview_StartH264Stream, srcCamera, DJI_LIVEVIEW_CAMERA_SOURCE_DEFAULT, DjiTestWidget_CameraStreamCallback);
        } else { // coupled switch mode off:
            DJI_STAT_CHECK_MAY_RETURN_RET(djiStat, DjiLiveview_StopH264Stream, srcCamera, DJI_LIVEVIEW_CAMERA_SOURCE_DEFAULT);

            #if CONFIG_TEST_WIDGET_ENABLE_RECORDER_FILE
                if (NULL != cameraRecorder->fd) {
                    DJI_STAT_CHECK_MAY_RETURN_RET(djiStat, osalHandler->MutexLock, cameraRecorder->lock);
                    if (DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS != fsHandler->FileClose(cameraRecorder->fd)) {
                        DjiTest_WidgetLogAppend("[ERR] leakage of fd:%d. Close failed!", cameraRecorder->fd);
                    }
                    cameraRecorder->fd = NULL;
                    cameraRecorder->frameCnt = 0;
                    DJI_STAT_CHECK_MAY_RETURN_RET(djiStat, osalHandler->MutexUnlock, cameraRecorder->lock);
                }
            #else
                DJI_STAT_CHECK_MAY_RETURN_RET(djiStat, osalHandler->MutexLock, cameraRecorder->lock);
                cameraRecorder->frameCnt = 0;
                DJI_STAT_CHECK_MAY_RETURN_RET(djiStat, osalHandler->MutexUnlock, cameraRecorder->lock);
            #endif
        }
        // action done, refresh real status.
        leaderListPri->itemStatus[position] = switchMode;
        DjiTest_WidgetLogAppend("->LiveView turn %s camera:%d->%d done.", (DJI_WIDGET_SWITCH_STATE_ON == leaderListPri->itemStatus[position]) ? "on" : "off", position, srcCamera);
    #endif
    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static void DjiTestWidget_PerceptionDepthImageCallback(T_DjiPerceptionImageInfo imageInfo, uint8_t *imageRawBuffer, uint32_t bufLen)
{
    T_TestPerceptionDepthImageRecorder  *depthImageRecorder = &s_depthImageRecorderBank[imageInfo.rawInfo.direction];
    T_DjiReturnCode                     djiStat = DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;

    if (0 == (depthImageRecorder->count % TEST_PERCEPTION_IMAGE_DOWNSAMPLE)) {
        DjiTest_WidgetLogAppend("->DepthIMG geted %d of \"%s\".", depthImageRecorder->count, depthImageRecorder->name);
    }
    depthImageRecorder->count ++;

    #if CONFIG_TEST_WIDGET_ENABLE_RECORDER_FILE
        T_DjiFileSystemHandler              *fsHandler = DjiPlatform_GetFileSystemHandler();
        T_DjiFileHandle                     fd = NULL;
        uint32_t                            size = 0;
        char                                fileName[TEST_ACTION_FILE_NAME_MAX_SIZE] = {0};

        // check for auto stop...
        if (TEST_PERCEPTION_SAVE_IMAGE_MAX > depthImageRecorder->count) {
            #if 0 // abandon: auto disable may cause thread race of depthImageRecorder->count
                if (DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS != DjiPerception_UnsubscribePerceptionImage(imageInfo.rawInfo.direction)) {
                    DjiTest_WidgetLogAppend("[ERR]:%s(%d) failed!", DjiPerception_UnsubscribePerceptionImage, imageInfo.rawInfo.direction);
                    return;
                }
                // can only reset by widget: depthImageRecorder->count = 0;
            #endif
            return;
        }

        snprintf(fileName, TEST_ACTION_FILE_NAME_MAX_SIZE, TEST_PERCEPTION_DEPTH_IMAGE_DIR "%s_%d.raw", depthImageRecorder->name, depthImageRecorder->count);

        if (TEST_PERCEPTION_SAVE_IMAGE_MAX < depthImageRecorder->count) {
            DJI_STAT_CHECK_MAY_RETURN(djiStat, fsHandler->FileOpen, fileName, "ab+", &fd);

            if (DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS != fsHandler->FileWrite(fd, imageRawBuffer, bufLen, &size)) {
                DjiTest_WidgetLogAppend("[ERR] fd:%d Write failed!", fd);
            } else if (bufLen == size) {
                (void)fsHandler->FileSync(fd);
            }

            if (DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS != fsHandler->FileClose(fd)) {
                DjiTest_WidgetLogAppend("[ERR] leakage of fd:%d. Close failed!", fd);
            }
        }
    #endif
    return;
}

T_DjiReturnCode DjiTestWidget_DepthImageCoupledSwitchAction(void *userData)
{
    T_DjiOsalHandler                    *osalHandler = DjiPlatform_GetOsalHandler();

    T_UserWidgetBasicData               *this = (T_UserWidgetBasicData *)userData;
    T_UserWidgetBasicData               *leader = this->leaderData;
    T_UserWidgetListPrivateData         *leaderListPri = (T_UserWidgetListPrivateData *)leader->pData;

    T_TestPerceptionDepthImageRecorder  *depthImageRecorder = NULL;
    E_DjiPerceptionDirection            direction = DJI_PERCEPTION_RECTIFY_FRONT;
    E_DjiWidgetSwitchState              switchMode = DJI_WIDGET_SWITCH_STATE_ON;

    T_DjiReturnCode                     djiStat = DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    static bool                         initFlag = false;
    int32_t                             count = 0, i = 0;

    if (! initFlag) {
        DjiTest_WidgetLogAppend("->DepthIMG init...");
        #if CONFIG_TEST_WIDGET_ENABLE_RECORDER_FILE
            T_DjiFileSystemHandler              *fsHandler = DjiPlatform_GetFileSystemHandler();
            DJI_STAT_CHECK_MAY_RETURN_RET(djiStat, fsHandler->Mkdir, TEST_PERCEPTION_DEPTH_IMAGE_DIR);
        #endif
        DJI_STAT_CHECK_MAY_RETURN_RET(djiStat, DjiPerception_Init);
        initFlag = true;
    }

    // TODO: not enough to thread-safe : leader->value && this->value may is being changed or have changed.
    direction = leader->value;
    switchMode = this->value;

    depthImageRecorder = &s_depthImageRecorderBank[direction];
    DjiTest_WidgetLogAppend("->DepthIMG turn %s \"%s\"...", (DJI_WIDGET_SWITCH_STATE_ON == switchMode) ? "on" : "off", depthImageRecorder->name);

    if (switchMode == leaderListPri->itemStatus[direction]) {
        // impossible: already seted!
        DjiTest_WidgetLogAppend("[ERR] %s_depth has been getd %d images!", depthImageRecorder->name, depthImageRecorder->count);
        return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    }

    if (DJI_WIDGET_SWITCH_STATE_ON == switchMode) { // coupled switch mode on:
        DJI_STAT_CHECK_MAY_RETURN_RET(djiStat, DjiPerception_SubscribePerceptionImage, direction, DjiTestWidget_PerceptionDepthImageCallback);
    } else { // coupled switch mode off:
        DJI_STAT_CHECK_MAY_RETURN_RET(djiStat, DjiPerception_UnsubscribePerceptionImage, direction);
        // reset recorded count: release cpu control at first, not thread-safe enough
        osalHandler->TaskSleepMs(200);
        depthImageRecorder->count = 0;
    }
    // action done, refresh real status.
    leaderListPri->itemStatus[direction] = switchMode;
    DjiTest_WidgetLogAppend("->DepthIMG turn %s \"%s\" done.", (DJI_WIDGET_SWITCH_STATE_ON == leaderListPri->itemStatus[direction]) ? "on" : "off", depthImageRecorder->name);
    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiTestWidget_HMSIntInputBoxAction(void *userData)
{
    T_UserWidgetBasicData               *this = (T_UserWidgetBasicData *)userData;

    T_TestUserHmsServerAlarmId          customizeHMS = { 0 };
    E_DjiHmsErrorLevel                  level = 0;

    T_DjiReturnCode                     djiStat = DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    #ifdef CONFIG_MODULE_SAMPLE_HMS_CUSTOMIZATION_ON
        static bool                         initFlag = true;
    #else
        static bool                         initFlag = false;
    #endif

    if (! initFlag) {
        DjiTest_WidgetLogAppend("->HMS init...");
        DJI_STAT_CHECK_MAY_RETURN_RET(djiStat, DjiTest_HmsCustomizationStartService);
        initFlag = true;
    }

    // TODO: not enough to thread-safe : leader->value && this->value may is being changed or have changed.
    customizeHMS.hmsCode = this->value;
    customizeHMS.department = DJI_HMS_PSDK_DAPARTMENT_ID;
    level = customizeHMS.module;
    customizeHMS.module = DJI_HMS_PSDK_MODULE_ID;
    djiStat = DjiHmsCustomization_InjectHmsErrorCode(customizeHMS.hmsCode, level);
    DjiTest_WidgetLogAppend("->HMS:%#X<%d> Inject ret:%d(%#08X).", customizeHMS.hmsCode, level, djiStat, djiStat);
    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
