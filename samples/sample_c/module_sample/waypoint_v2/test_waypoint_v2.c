/**
 ********************************************************************
 * @file    test_waypoint_v2.c
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
#include <widget_interaction_test/test_widget_interaction.h>
#include "test_waypoint_v2.h"
#include "dji_waypoint_v2.h"
#include "dji_fc_subscription.h"
#include "dji_logger.h"
#include "dji_platform.h"
#include "math.h"

/* Private constants ---------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/
typedef struct {
    uint8_t eventID;
    char *eventStr;
} T_DjiTestWaypointV2EventStr;

typedef struct {
    uint8_t missionState;
    char *stateStr;
} T_DjiTestWaypointV2StateStr;

/* Private values -------------------------------------------------------------*/
static T_DjiOsalHandler *osalHandler = NULL;
static const dji_f32_t TEST_EARTH_RADIUS = 6378137.0;
static uint32_t s_missionID = 12345;
//reference note of "T_DjiWaypointV2MissionEventPush"
static const T_DjiTestWaypointV2EventStr s_waypointV2EventStr[] = {
    {.eventID = 0x01, .eventStr = "Interrupt Event"},
    {.eventID = 0x02, .eventStr = "Resume Event"},
    {.eventID = 0x03, .eventStr = "Stop Event"},
    {.eventID = 0x10, .eventStr = "Arrival Event"},
    {.eventID = 0x11, .eventStr = "Finished Event"},
    {.eventID = 0x12, .eventStr = "Avoid Obstacle Event"},
    {.eventID = 0x30, .eventStr = "Action Switch Event"},
    {.eventID = 0xFF, .eventStr = "Unknown"}
};

//reference note of "T_DjiWaypointV2MissionStatePush"
static const T_DjiTestWaypointV2StateStr s_waypointV2StateStr[] = {
    {.missionState = 0x00, .stateStr = "Ground station not start"},
    {.missionState = 0x01, .stateStr = "Mission prepared"},
    {.missionState = 0x02, .stateStr = "Enter mission"},
    {.missionState = 0x03, .stateStr = "Execute mission"},
    {.missionState = 0x04, .stateStr = "Pause Mission"},
    {.missionState = 0x05, .stateStr = "Enter mission after ending pause"},
    {.missionState = 0x06, .stateStr = "Exit mission"},
    {.missionState = 0xFF, .stateStr = "Unknown"}
};

/* Private functions declaration ---------------------------------------------*/
uint8_t DJiTest_WaypointV2GetMissionEventIndex(uint8_t eventID);
uint8_t DjiTest_WaypointV2GetMissionStateIndex(uint8_t state);
static T_DJIWaypointV2Action *DjiTest_WaypointV2GenerateWaypointV2Actions(uint16_t actionNum);
static T_DjiWaypointV2 *DjiTest_WaypointV2GeneratePolygonWaypointV2(dji_f32_t radius, uint16_t polygonNum);
static void DjiTest_WaypointV2SetDefaultSetting(T_DjiWaypointV2 *waypointV2);
static T_DjiReturnCode DjiTest_WaypointV2UploadMission(uint16_t missionNum);
static T_DjiReturnCode DjiTest_WaypointV2EventCallback(T_DjiWaypointV2MissionEventPush eventData);
static T_DjiReturnCode DjiTest_WaypointV2StateCallback(T_DjiWaypointV2MissionStatePush stateData);
static T_DjiReturnCode DjiTest_WaypointV2Init(void);
static T_DjiReturnCode DjiTest_WaypointV2DeInit(void);

/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode DjiTest_WaypointV2RunSample(void)
{
    T_DjiReturnCode returnCode;
    uint32_t timeOutMs = 1000;
    uint16_t missionNum = 8;
    T_DjiWaypointV2GlobalCruiseSpeed setGlobalCruiseSpeed = 0;
    T_DjiWaypointV2GlobalCruiseSpeed getGlobalCruiseSpeed = 0;

    USER_LOG_INFO("Waypoint V2 sample start");
    DjiTest_WidgetLogAppend("Waypoint V2 sample start");

    USER_LOG_INFO("--> Step 1: Init Waypoint V2 sample");
    DjiTest_WidgetLogAppend("--> Step 1: Init Waypoint V2 sample");
    returnCode = DjiTest_WaypointV2Init();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Init waypoint V2 sample failed, error code: 0x%08X", returnCode);
        USER_LOG_INFO("Waypoint V2 sample end");
        return returnCode;
    }

    USER_LOG_INFO("--> Step 2: Subscribe gps fused data");
    DjiTest_WidgetLogAppend("--> Step 2: Subscribe gps fused data");
    returnCode = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_POSITION_FUSED,
                                                  DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ, NULL);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Subscribe gps fused data failed, error code: 0x%08X", returnCode);
        goto out;
    }

    USER_LOG_INFO("--> Step 3: Register waypoint V2 event and state callback\r\n");
    DjiTest_WidgetLogAppend("--> Step 3: Register waypoint V2 event and state callback\r\n");
    returnCode = DjiWaypointV2_RegisterMissionEventCallback(DjiTest_WaypointV2EventCallback);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Register waypoint V2 event failed, error code: 0x%08X", returnCode);
        goto out;
    }
    returnCode = DjiWaypointV2_RegisterMissionStateCallback(DjiTest_WaypointV2StateCallback);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Register waypoint V2 state failed, error code: 0x%08X", returnCode);
        goto out;
    }
    osalHandler->TaskSleepMs(timeOutMs);

    USER_LOG_INFO("--> Step 4: Upload waypoint V2 mission\r\n");
    DjiTest_WidgetLogAppend("--> Step 4: Upload waypoint V2 mission\r\n");
    returnCode = DjiTest_WaypointV2UploadMission(missionNum);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Upload waypoint V2 mission failed, error code: 0x%08X", returnCode);
        goto out;
    }
    osalHandler->TaskSleepMs(timeOutMs);

    USER_LOG_INFO("--> Step 5: Start waypoint V2 mission\r\n");
    DjiTest_WidgetLogAppend("--> Step 5: Start waypoint V2 mission\r\n");
    returnCode = DjiWaypointV2_Start();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Start waypoint V2 mission failed, error code: 0x%08X", returnCode);
        goto out;
    }
    osalHandler->TaskSleepMs(20 * timeOutMs);

    USER_LOG_INFO("--> Step 6: Set global cruise speed\r\n");
    DjiTest_WidgetLogAppend("--> Step 6: Set global cruise speed\r\n");
    setGlobalCruiseSpeed = 1.5;
    returnCode = DjiWaypointV2_SetGlobalCruiseSpeed(setGlobalCruiseSpeed);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Set global cruise speed failed, error code: 0x%08X", returnCode);
        goto out;
    }
    osalHandler->TaskSleepMs(timeOutMs);

    USER_LOG_INFO("--> Step 7: Get global cruise speed\r\n");
    DjiTest_WidgetLogAppend("--> Step 7: Get global cruise speed\r\n");
    returnCode = DjiWaypointV2_GetGlobalCruiseSpeed(&getGlobalCruiseSpeed);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Get global cruise speed failed, error code: 0x%08X", returnCode);
        goto out;
    }
    USER_LOG_INFO("Current global cruise speed is %f m/s", getGlobalCruiseSpeed);
    osalHandler->TaskSleepMs(timeOutMs);

    USER_LOG_INFO("--> Step 8: Pause waypoint V2 for 5 s\r\n");
    DjiTest_WidgetLogAppend("--> Step 8: Pause waypoint V2 for 5 s\r\n");
    returnCode = DjiWaypointV2_Pause();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Pause waypoint V2 failed, error code: 0x%08X", returnCode);
        goto out;
    }
    osalHandler->TaskSleepMs(5 * timeOutMs);

    USER_LOG_INFO("--> Step 9: Resume waypoint V2\r\n");
    DjiTest_WidgetLogAppend("--> Step 9: Resume waypoint V2\r\n");
    returnCode = DjiWaypointV2_Resume();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Resume waypoint V2 failed, error code: 0x%08X", returnCode);
        goto out;
    }
    osalHandler->TaskSleepMs(50 * timeOutMs);

    USER_LOG_INFO("--> Step 10: Deinit Waypoint V2 sample\r\n");
    DjiTest_WidgetLogAppend("--> Step 10: Deinit Waypoint V2 sample\r\n");
out:
    returnCode = DjiTest_WaypointV2DeInit();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Deinit waypoint V2 sample failed, error code: 0x%08X", returnCode);
    }

    USER_LOG_INFO("Waypoint V2 sample end");
    DjiTest_WidgetLogAppend("Waypoint V2 sample end");

    return returnCode;
}

/* Private functions definition-----------------------------------------------*/
static T_DJIWaypointV2Action *DjiTest_WaypointV2GenerateWaypointV2Actions(uint16_t actionNum)
{
    T_DJIWaypointV2Action *actions = NULL;
    uint16_t i;
    T_DJIWaypointV2Trigger trigger = {0};
    T_DJIWaypointV2SampleReachPointTriggerParam sampleReachPointTriggerParam = {0};
    T_DJIWaypointV2Actuator actuator = {0};
    T_DJIWaypointV2Action action = {0};

    actions = osalHandler->Malloc(actionNum * sizeof(T_DJIWaypointV2Action));
    if (actions == NULL) {
        return NULL;
    }

    for (i = 0; i < actionNum; i++) {
        sampleReachPointTriggerParam.waypointIndex = i;
        sampleReachPointTriggerParam.terminateNum = 0;

        trigger.actionTriggerType = DJI_WAYPOINT_V2_ACTION_TRIGGER_TYPE_SAMPLE_REACH_POINT;
        trigger.sampleReachPointTriggerParam.terminateNum = sampleReachPointTriggerParam.terminateNum;
        trigger.sampleReachPointTriggerParam.waypointIndex = sampleReachPointTriggerParam.waypointIndex;

        actuator.actuatorType = DJI_WAYPOINT_V2_ACTION_ACTUATOR_TYPE_CAMERA;
        actuator.actuatorIndex = 0;
        actuator.cameraActuatorParam.operationType = DJI_WAYPOINT_V2_ACTION_ACTUATOR_CAMERA_OPERATION_TYPE_TAKE_PHOTO;

        action.actionId = i;
        memcpy(&action.actuator, &actuator, sizeof(actuator));
        memcpy(&action.trigger, &trigger, sizeof(trigger));

        actions[i] = action;
    }

    return actions;
}

static void DjiTest_WaypointV2SetDefaultSetting(T_DjiWaypointV2 *waypointV2)
{
    waypointV2->waypointType = DJI_WAYPOINT_V2_FLIGHT_PATH_MODE_GO_TO_POINT_IN_STRAIGHT_AND_STOP;
    waypointV2->headingMode = DJI_WAYPOINT_V2_HEADING_MODE_AUTO;
    waypointV2->config.useLocalMaxVel = 0;
    waypointV2->config.useLocalCruiseVel = 0;
    waypointV2->dampingDistance = 40;
    waypointV2->heading = 0;
    waypointV2->turnMode = DJI_WAYPOINT_V2_TURN_MODE_CLOCK_WISE;

    waypointV2->pointOfInterest.positionX = 0;
    waypointV2->pointOfInterest.positionY = 0;
    waypointV2->pointOfInterest.positionZ = 0;
    waypointV2->maxFlightSpeed = 9;
    waypointV2->autoFlightSpeed = 2;
}

static T_DjiWaypointV2 *DjiTest_WaypointV2GeneratePolygonWaypointV2(dji_f32_t radius, uint16_t polygonNum)
{
    // Let's create a vector to store our waypoints in.
    T_DjiReturnCode returnCode;
    T_DjiWaypointV2 *waypointV2List = (T_DjiWaypointV2 *) osalHandler->Malloc(
        (polygonNum + 2) * sizeof(T_DjiWaypointV2));
    T_DjiWaypointV2 startPoint;
    T_DjiWaypointV2 waypointV2;
    T_DjiFcSubscriptionPositionFused positionFused = {0};
    T_DjiDataTimestamp timestamp = {0};

    osalHandler->TaskSleepMs(1000);
    returnCode = DjiFcSubscription_GetLatestValueOfTopic(DJI_FC_SUBSCRIPTION_TOPIC_POSITION_FUSED,
                                                         (uint8_t *) &positionFused,
                                                         sizeof(T_DjiFcSubscriptionPositionFused),
                                                         &timestamp);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Get value of topic GPS Fused error");
    } else {
        USER_LOG_DEBUG("Timestamp: millisecond %u microsecond %u.", timestamp.millisecond,
                       timestamp.microsecond);
        USER_LOG_DEBUG("Position: %f %f %f %d.", positionFused.latitude, positionFused.longitude,
                       positionFused.altitude,
                       positionFused.visibleSatelliteNumber);
    }

    startPoint.latitude = positionFused.latitude;
    startPoint.longitude = positionFused.longitude;
    startPoint.relativeHeight = 15;
    DjiTest_WaypointV2SetDefaultSetting(&startPoint);
    waypointV2List[0] = startPoint;

    // Iterative algorithm
    for (int i = 0; i < polygonNum; i++) {
        dji_f32_t angle = i * 2 * DJI_PI / polygonNum;
        DjiTest_WaypointV2SetDefaultSetting(&waypointV2);
        dji_f32_t X = radius * cos(angle);
        dji_f32_t Y = radius * sin(angle);
        waypointV2.latitude = X / TEST_EARTH_RADIUS + startPoint.latitude;
        waypointV2.longitude = Y / (TEST_EARTH_RADIUS * cos(startPoint.latitude)) + startPoint.longitude;
        waypointV2.relativeHeight = startPoint.relativeHeight;

        waypointV2List[i + 1] = waypointV2;
    }

    waypointV2List[polygonNum + 1] = startPoint;
    return waypointV2List;
}

uint8_t DJiTest_WaypointV2GetMissionEventIndex(uint8_t eventID)
{
    uint8_t i;

    for (i = 0; i < sizeof(s_waypointV2EventStr) / sizeof(T_DjiTestWaypointV2EventStr); i++) {
        if (s_waypointV2EventStr[i].eventID == eventID) {
            return i;
        }
    }

    return i;
}

uint8_t DjiTest_WaypointV2GetMissionStateIndex(uint8_t state)
{
    uint8_t i;

    for (i = 0; i < sizeof(s_waypointV2StateStr) / sizeof(T_DjiTestWaypointV2StateStr); i++) {
        if (s_waypointV2StateStr[i].missionState == state) {
            return i;
        }
    }

    return i;
}

static T_DjiReturnCode DjiTest_WaypointV2EventCallback(T_DjiWaypointV2MissionEventPush eventData)
{
    if (eventData.event == 0x01) {
        USER_LOG_INFO("[%s]: Mission interrupted reason is 0x%x",
                      s_waypointV2EventStr[DJiTest_WaypointV2GetMissionEventIndex(eventData.event)].eventStr,
                      eventData.data.interruptReason);
    } else if (eventData.event == 0x02) {
        USER_LOG_INFO("[%s]: Mission recover reason is 0x%x",
                      s_waypointV2EventStr[DJiTest_WaypointV2GetMissionEventIndex(eventData.event)].eventStr,
                      eventData.data.recoverProcess);
    } else if (eventData.event == 0x03) {
        USER_LOG_INFO("[%s]: Mission exit reason is 0x%x",
                      s_waypointV2EventStr[DJiTest_WaypointV2GetMissionEventIndex(eventData.event)].eventStr,
                      eventData.data.exitReason);
    } else if (eventData.event == 0x10) {
        USER_LOG_INFO("[%s]: Current waypoint index is %d",
                      s_waypointV2EventStr[DJiTest_WaypointV2GetMissionEventIndex(eventData.event)].eventStr,
                      eventData.data.waypointIndex);
    } else if (eventData.event == 0x11) {
        USER_LOG_INFO("[%s]: Current mission execute times is %d",
                      s_waypointV2EventStr[DJiTest_WaypointV2GetMissionEventIndex(eventData.event)].eventStr,
                      eventData.data.T_DjiWaypointV2MissionExecEvent.currentMissionExecTimes);
    } else if (eventData.event == 0x12) {
        USER_LOG_INFO("[%s]: avoid obstacle state:%d",
                      s_waypointV2EventStr[DJiTest_WaypointV2GetMissionEventIndex(eventData.event)].eventStr,
                      eventData.data.avoidState);
    } else if (eventData.event == 0x30) {
        USER_LOG_INFO(
            "[%s]: action id:%d, pre actuator state:%d, current actuator state:%d, result:0x%08llX",
            s_waypointV2EventStr[DJiTest_WaypointV2GetMissionEventIndex(eventData.event)].eventStr,
            eventData.data.T_DjiWaypointV2ActionExecEvent.actionId,
            eventData.data.T_DjiWaypointV2ActionExecEvent.preActuatorState,
            eventData.data.T_DjiWaypointV2ActionExecEvent.curActuatorState,
            eventData.data.T_DjiWaypointV2ActionExecEvent.result
        );
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode DjiTest_WaypointV2StateCallback(T_DjiWaypointV2MissionStatePush stateData)
{
    static uint32_t curMs = 0;
    static uint32_t preMs = 0;
    osalHandler->GetTimeMs(&curMs);
    if (curMs - preMs >= 1000) {
        preMs = curMs;
        USER_LOG_INFO("[Waypoint Index:%d]: State: %s, velocity:%.2f m/s",
                      stateData.curWaypointIndex,
                      s_waypointV2StateStr[DjiTest_WaypointV2GetMissionStateIndex(stateData.state)].stateStr,
                      (dji_f32_t) stateData.velocity / 100);
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode DjiTest_WaypointV2Init(void)
{
    T_DjiReturnCode returnCode;

    osalHandler = DjiPlatform_GetOsalHandler();
    if (!osalHandler) return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;

    returnCode = DjiFcSubscription_Init();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Init waypoint V2 data subscription module error, stat:0x%08llX", returnCode);
        return returnCode;
    }

    returnCode = DjiWaypointV2_Init();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Init waypoint V2 module error, stat:0x%08llX", returnCode);
        return returnCode;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;

}

static T_DjiReturnCode DjiTest_WaypointV2DeInit(void)
{
    T_DjiReturnCode returnCode;

    returnCode = DjiFcSubscription_DeInit();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Deinit waypoint V2 data subscription module error, stat:0x%08llX", returnCode);
        return returnCode;
    }

    returnCode = DjiWaypointV2_Deinit();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Deinit waypoint V2 module error, stat:0x%08llX", returnCode);
        return returnCode;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode DjiTest_WaypointV2UploadMission(uint16_t missionNum)
{
    T_DjiReturnCode returnCode;
    uint16_t polygonNum = missionNum - 2;
    dji_f32_t radius = 6;
    uint16_t actionNum = 5;
    T_DjiWayPointV2MissionSettings missionInitSettings = {0};
    T_DJIWaypointV2ActionList actionList = {NULL, 0};

    /*! Generate actions*/
    actionList.actions = DjiTest_WaypointV2GenerateWaypointV2Actions(actionNum);
    actionList.actionNum = actionNum;

    /*! Init waypoint settings*/
    missionInitSettings.missionID = s_missionID + 10;
    USER_LOG_DEBUG("Generate mission id:%d", missionInitSettings.missionID);
    missionInitSettings.repeatTimes = 1;
    missionInitSettings.finishedAction = DJI_WAYPOINT_V2_FINISHED_GO_HOME;
    missionInitSettings.maxFlightSpeed = 10;
    missionInitSettings.autoFlightSpeed = 2;
    missionInitSettings.actionWhenRcLost = DJI_WAYPOINT_V2_MISSION_KEEP_EXECUTE_WAYPOINT_V2;
    missionInitSettings.gotoFirstWaypointMode = DJI_WAYPOINT_V2_MISSION_GO_TO_FIRST_WAYPOINT_MODE_POINT_TO_POINT;
    missionInitSettings.mission = DjiTest_WaypointV2GeneratePolygonWaypointV2(radius, polygonNum);
    missionInitSettings.missTotalLen = missionNum;
    missionInitSettings.actionList = actionList;

    returnCode = DjiWaypointV2_UploadMission(&missionInitSettings);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Init waypoint V2 mission setting failed, ErrorCode:0x%lX", returnCode);
        goto out;
    }

out:
    osalHandler->Free(actionList.actions);
    return returnCode;
}

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
