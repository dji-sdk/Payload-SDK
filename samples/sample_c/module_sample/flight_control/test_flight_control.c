/**
 ********************************************************************
 * @file    test_flight_control.c
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
#include "dji_flight_controller.h"
#include "test_flight_control.h"
#include "dji_fc_subscription.h"
#include "dji_platform.h"
#include "dji_logger.h"
#include <math.h>
#include <widget_interaction_test/test_widget_interaction.h>
#include <dji_aircraft_info.h>
#include "dji_fts.h"
#include "dji_waypoint_v3.h"
/* Private constants ---------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/
typedef struct {
    E_DjiFcSubscriptionDisplayMode displayMode;
    char *displayModeStr;
} T_DjiTestFlightControlDisplayModeStr;

/* Private values -------------------------------------------------------------*/
static T_DjiOsalHandler *s_osalHandler = NULL;
static const double s_earthCenter = 6378137.0;
static const double s_degToRad = 0.01745329252;
static bool s_isFtsCallbackRegistered = false;
static bool s_isWaypointV3Initialized = false;
static int32_t s_ftsTriggerCount = 0;
static uint8_t s_mission_state_machine = 0;

static const T_DjiTestFlightControlDisplayModeStr s_flightControlDisplayModeStr[] = {
    {.displayMode = DJI_FC_SUBSCRIPTION_DISPLAY_MODE_ATTITUDE, .displayModeStr = "attitude mode"},
    {.displayMode = DJI_FC_SUBSCRIPTION_DISPLAY_MODE_P_GPS, .displayModeStr = "p_gps mode"},
    {.displayMode = DJI_FC_SUBSCRIPTION_DISPLAY_MODE_ASSISTED_TAKEOFF, .displayModeStr = "assisted takeoff mode"},
    {.displayMode = DJI_FC_SUBSCRIPTION_DISPLAY_MODE_AUTO_TAKEOFF, .displayModeStr = "auto takeoff mode"},
    {.displayMode = DJI_FC_SUBSCRIPTION_DISPLAY_MODE_AUTO_LANDING, .displayModeStr = "auto landing mode"},
    {.displayMode = DJI_FC_SUBSCRIPTION_DISPLAY_MODE_NAVI_GO_HOME, .displayModeStr = "go home mode"},
    {.displayMode = DJI_FC_SUBSCRIPTION_DISPLAY_MODE_FORCE_AUTO_LANDING, .displayModeStr = "force landing mode"},
    {.displayMode = DJI_FC_SUBSCRIPTION_DISPLAY_MODE_ENGINE_START, .displayModeStr = "engine start mode"},
    {.displayMode = 0xFF, .displayModeStr = "unknown mode"}
};

/* Private functions declaration ---------------------------------------------*/
static uint8_t DjiTest_FlightControlGetDisplayModeIndex(E_DjiFcSubscriptionDisplayMode displayMode);
static T_DjiFcSubscriptionFlightStatus DjiTest_FlightControlGetValueOfFlightStatus(void);
static T_DjiFcSubscriptionDisplaymode DjiTest_FlightControlGetValueOfDisplayMode(void);
static T_DjiFcSubscriptionHeightFusion DjiTest_FlightControlGetValueOfHeightFusion(void);
static T_DjiFcSubscriptionQuaternion DjiTest_FlightControlGetValueOfQuaternion(void);
static T_DjiFcSubscriptionPositionFused DjiTest_FlightControlGetValueOfPositionFused(void);
static dji_f32_t DjiTest_FlightControlGetValueOfRelativeHeight(void);
static bool DjiTest_FlightControlMotorStartedCheck(void);
static bool DjiTest_FlightControlTakeOffInAirCheck(void);
static bool DjiTest_FlightControlLandFinishedCheck(void);
static bool DjiTest_FlightControlMonitoredTakeoff(void);
static bool DjiTest_FlightControlCheckActionStarted(E_DjiFcSubscriptionDisplayMode mode);
static bool DjiTest_FlightControlMonitoredLanding(void);
static bool DjiTest_FlightControlGoHomeAndConfirmLanding(void);
static T_DjiTestFlightControlVector3f DjiTest_FlightControlQuaternionToEulerAngle(T_DjiFcSubscriptionQuaternion quat);
static T_DjiTestFlightControlVector3f
DjiTest_FlightControlLocalOffsetFromGpsAndFusedHeightOffset(T_DjiFcSubscriptionPositionFused target,
                                                            T_DjiFcSubscriptionPositionFused origin,
                                                            dji_f32_t targetHeight,
                                                            dji_f32_t originHeight);
static T_DjiTestFlightControlVector3f
DjiTest_FlightControlVector3FSub(T_DjiTestFlightControlVector3f vectorA, T_DjiTestFlightControlVector3f vectorB);
static int DjiTest_FlightControlSignOfData(dji_f32_t data);
static void DjiTest_FlightControlHorizCommandLimit(dji_f32_t speedFactor, dji_f32_t *commandX, dji_f32_t *commandY);
static dji_f32_t DjiTest_FlightControlVectorNorm(T_DjiTestFlightControlVector3f v);
static T_DjiReturnCode
DjiTest_FlightControlJoystickCtrlAuthSwitchEventCallback(T_DjiFlightControllerJoystickCtrlAuthorityEventInfo eventData);
static bool DjiTest_FlightControlMoveByPositionOffset(T_DjiTestFlightControlVector3f offsetDesired,
                                                      float yawDesiredInDeg,
                                                      float posThresholdInM,
                                                      float yawThresholdInDeg);
static T_DjiReturnCode DjiTest_FlightControlInit(void);
static T_DjiReturnCode DjiTest_FlightControlDeInit(void);
static void DjiTest_FlightControlTakeOffLandingSample(void);
static void DjiTest_FlightControlPositionControlSample(void);
static void DjiTest_FlightControlGoHomeForceLandingSample(void);
static void DjiTest_FlightControlVelocityControlSample(void);
static void DjiTest_FlightControlArrestFlyingSample(void);
static void DjiTest_FlightControlSetGetParamSample(void);
static void DjiTest_FlightControlSetGetPerceptionParamSample(void);
static void DjiTest_FlightControlPassiveTriggerFtsSample(void);
static void DjiTest_FlightControlSlowRotateMotorSample(void);
static T_DjiReturnCode DjiTest_TriggerFtsEventCallback(void);
static void DjiTest_FlightControlSample(E_DjiTestFlightCtrlSampleSelect flightCtrlSampleSelect);
static void DjiTest_FlightControlSetModeStartMission(void);
static T_DjiReturnCode DjiTest_FlightControlExtremeBatteryEnduranceModeSample(void);

/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode DjiTest_FlightControlRunSample(E_DjiTestFlightCtrlSampleSelect flightCtrlSampleSelect)
{
    T_DjiReturnCode returnCode;

    USER_LOG_DEBUG("Init flight Control Sample");
    DjiTest_WidgetLogAppend("Init flight Control Sample");

    returnCode = DjiTest_FlightControlInit();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Init flight Control sample failed,error code:0x%08llX", returnCode);
        return returnCode;
    }

    DjiTest_FlightControlSample(flightCtrlSampleSelect);

    USER_LOG_DEBUG("Deinit Flight Control Sample");
    DjiTest_WidgetLogAppend("Deinit Flight Control Sample");
    returnCode = DjiTest_FlightControlDeInit();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Deinit Flight Control sample failed,error code:0x%08llX", returnCode);
        return returnCode;
    }

    return returnCode;
}

/* Private functions definition-----------------------------------------------*/
T_DjiReturnCode DjiTest_FlightControlInit(void)
{
    T_DjiReturnCode returnCode;
    T_DjiFlightControllerRidInfo ridInfo = {0};

    s_osalHandler = DjiPlatform_GetOsalHandler();
    if (!s_osalHandler) return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;

    ridInfo.latitude = 22.542812;
    ridInfo.longitude = 113.958902;
    ridInfo.altitude = 10;

    returnCode = DjiFlightController_Init(ridInfo);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Init flight controller module failed, error code:0x%08llX", returnCode);
        return returnCode;
    }

    returnCode = DjiFcSubscription_Init();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Init data subscription module failed, error code:0x%08llX", returnCode);
        return returnCode;
    }

    returnCode = DjiFlightController_SetRCLostActionEnableStatus(DJI_FLIGHT_CONTROLLER_DISABLE_RC_LOST_ACTION);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Set RC lost action failed, error code:0x%08llX", returnCode);
        return returnCode;
    }

    /*! subscribe fc data */
    returnCode = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_STATUS_FLIGHT,
                                                  DJI_DATA_SUBSCRIPTION_TOPIC_10_HZ,
                                                  NULL);

    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Subscribe topic flight status failed, error code:0x%08llX", returnCode);
        return returnCode;
    }

    returnCode = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_STATUS_DISPLAYMODE,
                                                  DJI_DATA_SUBSCRIPTION_TOPIC_10_HZ,
                                                  NULL);

    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Subscribe topic display mode failed, error code:0x%08llX", returnCode);
        return returnCode;
    }

    returnCode = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_HEIGHT_FUSION,
                                                  DJI_DATA_SUBSCRIPTION_TOPIC_10_HZ,
                                                  NULL);

    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Subscribe topic avoid data failed,error code:0x%08llX", returnCode);
        return returnCode;
    }

    returnCode = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_QUATERNION,
                                                  DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ,
                                                  NULL);

    if (returnCode == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
    } else if (returnCode == DJI_ERROR_SUBSCRIPTION_MODULE_CODE_TOPIC_DUPLICATE) {
        USER_LOG_WARN("Subscribe topic quaternion duplicate");
    } else {
        USER_LOG_ERROR("Subscribe topic quaternion failed,error code:0x%08llX", returnCode);
        return returnCode;
    }

    returnCode = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_POSITION_FUSED,
                                                  DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ,
                                                  NULL);

    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Subscribe topic position fused failed,error code:0x%08llX", returnCode);
        return returnCode;
    }

    returnCode = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_ALTITUDE_FUSED,
                                                  DJI_DATA_SUBSCRIPTION_TOPIC_50_HZ,
                                                  NULL);

    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Subscribe topic altitude fused failed,error code:0x%08llX", returnCode);
        return returnCode;
    }

    returnCode = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_ALTITUDE_OF_HOMEPOINT,
                                                  DJI_DATA_SUBSCRIPTION_TOPIC_1_HZ,
                                                  NULL);

    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Subscribe topic altitude of home point failed,error code:0x%08llX", returnCode);
        return returnCode;
    }

    returnCode = DjiFlightController_RegJoystickCtrlAuthorityEventCallback(
        DjiTest_FlightControlJoystickCtrlAuthSwitchEventCallback);

    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS && returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT) {
        USER_LOG_ERROR("Register joystick control authority event callback failed,error code:0x%08llX", returnCode);
        return returnCode;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiTest_FlightControlDeInit(void)
{
    T_DjiReturnCode returnCode;

    returnCode = DjiFlightController_DeInit();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Deinit flight controller module failed, error code:0x%08llX",
                       returnCode);
        return returnCode;
    }

    returnCode = DjiFcSubscription_DeInit();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Deinit data subscription module failed, error code:0x%08llX",
                       returnCode);
        return returnCode;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

void DjiTest_FlightControlTakeOffLandingSample()
{
    T_DjiReturnCode returnCode;

    USER_LOG_INFO("Flight control takeoff-landing sample start");
    DjiTest_WidgetLogAppend("Flight control takeoff-landing sample start");
    USER_LOG_INFO("--> Step 1: Obtain joystick control authority.");
    DjiTest_WidgetLogAppend("--> Step 1: Obtain joystick control authority.");
    returnCode = DjiFlightController_ObtainJoystickCtrlAuthority();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Obtain joystick authority failed, error code: 0x%08X", returnCode);
        goto out;
    }
    s_osalHandler->TaskSleepMs(1000);

    USER_LOG_INFO("--> Step 2: Take off\r\n");
    DjiTest_WidgetLogAppend("--> Step 2: Take off\r\n");
    if (!DjiTest_FlightControlMonitoredTakeoff()) {
        USER_LOG_ERROR("Take off failed");
        goto out;
    }
    USER_LOG_INFO("Successful take off\r\n");
    DjiTest_WidgetLogAppend("Successful take off\r\n");
    s_osalHandler->TaskSleepMs(4000);

    USER_LOG_INFO("--> Step 3: Landing\r\n");
    DjiTest_WidgetLogAppend("--> Step 3: Landing\r\n");
    if (!DjiTest_FlightControlMonitoredLanding()) {
        USER_LOG_ERROR("Landing failed");
        goto out;
    }
    USER_LOG_INFO("Successful landing\r\n");
    DjiTest_WidgetLogAppend("Successful landing\r\n");

    USER_LOG_INFO("--> Step 4: Release joystick authority");
    DjiTest_WidgetLogAppend("--> Step 4: Release joystick authority");
    returnCode = DjiFlightController_ReleaseJoystickCtrlAuthority();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Release joystick authority failed, error code: 0x%08X", returnCode);
        goto out;
    }

out:
    USER_LOG_INFO("Flight control takeoff-landing sample end");
    DjiTest_WidgetLogAppend("Flight control takeoff-landing sample end");
}

void DjiTest_FlightControlPositionControlSample()
{
    T_DjiReturnCode returnCode;

    USER_LOG_INFO("Flight control move-by-position sample start");
    DjiTest_WidgetLogAppend("Flight control move-by-position sample start");

    USER_LOG_INFO("--> Step 1: Obtain joystick control authority.");
    DjiTest_WidgetLogAppend("--> Step 1: Obtain joystick control authority.");
    returnCode = DjiFlightController_ObtainJoystickCtrlAuthority();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Obtain joystick authority failed, error code: 0x%08X", returnCode);
        goto out;
    }
    s_osalHandler->TaskSleepMs(1000);

    USER_LOG_INFO("--> Step 2: Take off\r\n");
    DjiTest_WidgetLogAppend("--> Step 2: Take off\r\n");
    if (!DjiTest_FlightControlMonitoredTakeoff()) {
        USER_LOG_ERROR("Take off failed");
        goto out;
    }
    USER_LOG_INFO("Successful take off\r\n");
    DjiTest_WidgetLogAppend("Successful take off\r\n");

    USER_LOG_INFO("--> Step 3: Move to north:0(m), east:6(m), up:6(m) , yaw:30(degree) from current point");
    DjiTest_WidgetLogAppend("--> Step 3: Move to north:0(m), east:6(m), up:6(m) , yaw:30(degree) from current point");
    if (!DjiTest_FlightControlMoveByPositionOffset((T_DjiTestFlightControlVector3f) {0, 6, 6}, 30, 0.8, 1)) {
        USER_LOG_ERROR("Move to north:0(m), east:6(m), up:6(m) , yaw:30(degree) from current point failed");
        goto out;
    };

    USER_LOG_INFO("--> Step 4: Move to north:6(m), east:0(m), up:-3(m) , yaw:-30(degree) from current point");
    DjiTest_WidgetLogAppend(
        "--> Step 4: Move to north:6(m), east:0(m), up:-3(m) , yaw:-30(degree) from current point");
    if (!DjiTest_FlightControlMoveByPositionOffset((T_DjiTestFlightControlVector3f) {6, 0, -3}, -30, 0.8, 1)) {
        USER_LOG_ERROR("Move to north:6(m), east:0(m), up:-3(m) , yaw:-30(degree) from current point failed");
        goto out;
    };

    USER_LOG_INFO("--> Step 5: Move to north:-6(m), east:-6(m), up:0(m) , yaw:0(degree) from current point");
    DjiTest_WidgetLogAppend("--> Step 5: Move to north:-6(m), east:-6(m), up:0(m) , yaw:0(degree) from current point");
    if (!DjiTest_FlightControlMoveByPositionOffset((T_DjiTestFlightControlVector3f) {-6, -6, 0}, 0, 0.8, 1)) {
        USER_LOG_ERROR("Move to north:-6(m), east:-6(m), up:0(m) , yaw:0(degree) from current point failed");
        goto out;
    }

    USER_LOG_INFO("--> Step 6: Landing\r\n");
    DjiTest_WidgetLogAppend("--> Step 6: Landing\r\n");
    if (!DjiTest_FlightControlMonitoredLanding()) {
        USER_LOG_ERROR("Landing failed");
        goto out;
    }
    USER_LOG_INFO("Successful landing\r\n");
    DjiTest_WidgetLogAppend("Successful landing\r\n");

    USER_LOG_INFO("--> Step 7: Release joystick authority");
    DjiTest_WidgetLogAppend("--> Step 7: Release joystick authority");
    returnCode = DjiFlightController_ReleaseJoystickCtrlAuthority();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Release joystick authority failed, error code: 0x%08X", returnCode);
        goto out;
    }

out:
    USER_LOG_INFO("Flight control move-by-position sample end");
    DjiTest_WidgetLogAppend("Flight control move-by-position sample end");
}

void DjiTest_FlightControlGoHomeForceLandingSample()
{
    T_DjiReturnCode returnCode;

    USER_LOG_INFO("Flight control go-home-force-landing sample start");
    DjiTest_WidgetLogAppend("Flight control go-home-force-landing sample start");

    USER_LOG_INFO("--> Step 1: Obtain joystick control authority");
    DjiTest_WidgetLogAppend("--> Step 1: Obtain joystick control authority");
    returnCode = DjiFlightController_ObtainJoystickCtrlAuthority();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Obtain joystick authority failed, error code: 0x%08X", returnCode);
        goto out;
    }
    s_osalHandler->TaskSleepMs(1000);

    USER_LOG_INFO("--> Step 2: Take off\r\n");
    DjiTest_WidgetLogAppend("--> Step 2: Take off\r\n");
    if (!DjiTest_FlightControlMonitoredTakeoff()) {
        USER_LOG_ERROR("Take off failed");
        goto out;
    }
    USER_LOG_INFO("Successful take off\r\n");
    DjiTest_WidgetLogAppend("Successful take off\r\n");

    USER_LOG_INFO("--> Step 3: Move to north:0(m), east:0(m), up:30(m) , yaw:0(degree) from current point");
    DjiTest_WidgetLogAppend("--> Step 3: Move to north:0(m), east:0(m), up:30(m) , yaw:0(degree) from current point");
    if (!DjiTest_FlightControlMoveByPositionOffset((T_DjiTestFlightControlVector3f) {0, 0, 30}, 0, 0.8, 1)) {
        USER_LOG_ERROR("Move to north:0(m), east:0(m), up:30(m) , yaw:0(degree) from current point failed");
        goto out;
    }

    USER_LOG_INFO("--> Step 4: Move to north:10(m), east:0(m), up:0(m) , yaw:0(degree) from current point");
    DjiTest_WidgetLogAppend("--> Step 4: Move to north:10(m), east:0(m), up:0(m) , yaw:0(degree) from current point");
    if (!DjiTest_FlightControlMoveByPositionOffset((T_DjiTestFlightControlVector3f) {10, 0, 0}, 0, 0.8, 1)) {
        USER_LOG_ERROR("Move to north:10(m), east:0(m), up:0(m) , yaw:0(degree) from current point failed");
        goto out;
    }

    USER_LOG_INFO("--> Step 5: Set aircraft current position as new home location!");
    DjiTest_WidgetLogAppend("--> Step 5: Set aircraft current position as new home location!");
    returnCode = DjiFlightController_SetHomeLocationUsingCurrentAircraftLocation();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Set aircraft current position as new home location failed, error code: 0x%08X", returnCode);
        goto out;
    }

    USER_LOG_INFO("--> Step 6: Set go home altitude to 50(m)\r\n");
    DjiTest_WidgetLogAppend("--> Step 6: Set go home altitude to 50(m)\r\n");
    returnCode = DjiFlightController_SetGoHomeAltitude(50);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Set go home altitude to 50(m) failed, error code: 0x%08X", returnCode);
        goto out;
    }

    /*! get go home altitude */
    E_DjiFlightControllerGoHomeAltitude goHomeAltitude;
    returnCode = DjiFlightController_GetGoHomeAltitude(&goHomeAltitude);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Get go home altitude failed, error code: 0x%08X", returnCode);
        goto out;
    }
    USER_LOG_INFO("Current go home altitude is %d m\r\n", goHomeAltitude);
    DjiTest_WidgetLogAppend("Current go home altitude is %d m\r\n", goHomeAltitude);

    USER_LOG_INFO("--> Step 7: Move to north:20(m), east:0(m), up:0(m) , yaw:0(degree) from current point");
    DjiTest_WidgetLogAppend("--> Step 7: Move to north:20(m), east:0(m), up:0(m) , yaw:0(degree) from current point");
    if (!DjiTest_FlightControlMoveByPositionOffset((T_DjiTestFlightControlVector3f) {20, 0, 0}, 0, 0.8, 1)) {
        USER_LOG_ERROR("Move to north:20(m), east:0(m), up:0(m) , yaw:0(degree) from current point failed");
        goto out;
    }

    USER_LOG_INFO("--> Step 8: Go home and confirm force landing\r\n");
    DjiTest_WidgetLogAppend("--> Step 8: Go home and confirm force landing\r\n");
    if (!DjiTest_FlightControlGoHomeAndConfirmLanding()) {
        USER_LOG_ERROR("Go home and confirm force landing failed");
        goto out;
    }
    USER_LOG_INFO("Successful go home and confirm force landing\r\n");
    DjiTest_WidgetLogAppend("Successful go home and confirm force landing\r\n");

    USER_LOG_INFO("-> Step 9: Release joystick authority");
    s_osalHandler->TaskSleepMs(1000);
    DjiTest_WidgetLogAppend("-> Step 9: Release joystick authority");
    returnCode = DjiFlightController_ReleaseJoystickCtrlAuthority();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Release joystick authority failed, error code: 0x%08X", returnCode);
        goto out;
    }

out:
    USER_LOG_INFO("Flight control go-home-force-landing sample end");
    DjiTest_WidgetLogAppend("Flight control go-home-force-landing sample end");
}

void DjiTest_FlightControlVelocityControlSample()
{
    T_DjiReturnCode returnCode;

    USER_LOG_INFO("Flight control move-by-velocity sample start");
    DjiTest_WidgetLogAppend("Flight control move-by-velocity sample start");

    USER_LOG_INFO("--> Step 1: Obtain joystick control authority");
    DjiTest_WidgetLogAppend("--> Step 1: Obtain joystick control authority");
    returnCode = DjiFlightController_ObtainJoystickCtrlAuthority();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Obtain joystick authority failed, error code: 0x%08X", returnCode);
        goto out;
    }
    s_osalHandler->TaskSleepMs(1000);

    USER_LOG_INFO("--> Step 2: Take off\r\n");
    DjiTest_WidgetLogAppend("--> Step 2: Take off\r\n");
    if (!DjiTest_FlightControlMonitoredTakeoff()) {
        USER_LOG_ERROR("Take off failed");
        goto out;
    }
    USER_LOG_INFO("Successful take off\r\n");
    DjiTest_WidgetLogAppend("Successful take off\r\n");

    USER_LOG_INFO(
        "--> Step 3: Move with north:0(m/s), east:0(m/s), up:5(m/s), yaw:0(deg/s) from current point for 2s!");
    DjiTest_WidgetLogAppend(
        "--> Step 3: Move with north:0(m/s), east:0(m/s), up:5(m/s), yaw:0(deg/s) from current point for 2s!");
    DjiTest_FlightControlVelocityAndYawRateCtrl((T_DjiTestFlightControlVector3f) {0, 0, 5.0}, 0, 2000);

    USER_LOG_INFO("--> Step 4: Emergency brake for 2s");
    DjiTest_WidgetLogAppend("--> Step 4: Emergency brake for 2s");
    returnCode = DjiFlightController_ExecuteEmergencyBrakeAction();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Emergency brake failed, error code: 0x%08X", returnCode);
        goto out;
    }
    s_osalHandler->TaskSleepMs(2000);
    returnCode = DjiFlightController_CancelEmergencyBrakeAction();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Cancel emergency brake action failed, error code: 0x%08X", returnCode);
        goto out;
    }

    USER_LOG_INFO(
        "--> Step 5: Move with north:-1.5(m/s), east:2(m/s), up:0(m/s), yaw:20(deg/s) from current point for 2s!");
    DjiTest_WidgetLogAppend(
        "--> Step 5: Move with north:-1.5(m/s), east:2(m/s), up:0(m/s), yaw:20(deg/s) from current point for 2s!");
    DjiTest_FlightControlVelocityAndYawRateCtrl((T_DjiTestFlightControlVector3f) {-1.5, 2, 0}, 20, 2000);

    USER_LOG_INFO("--> Step 6: Emergency brake for 2s");
    DjiTest_WidgetLogAppend("--> Step 6: Emergency brake for 2s");
    returnCode = DjiFlightController_ExecuteEmergencyBrakeAction();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Emergency brake failed, error code: 0x%08X", returnCode);
        goto out;
    }
    s_osalHandler->TaskSleepMs(2000);
    returnCode = DjiFlightController_CancelEmergencyBrakeAction();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Cancel emergency brake action failed, error code: 0x%08X", returnCode);
        goto out;
    }

    USER_LOG_INFO(
        "--> Step 7: Move with north:3(m/s), east:0(m/s), up:0(m/s), yaw:0(deg/s) from current point for 2.5s!");
    DjiTest_WidgetLogAppend(
        "--> Step 7: Move with north:3(m/s), east:0(m/s), up:0(m/s), yaw:0(deg/s) from current point for 2.5s!");
    DjiTest_FlightControlVelocityAndYawRateCtrl((T_DjiTestFlightControlVector3f) {3, 0, 0}, 0, 2500);

    USER_LOG_INFO("--> Step 8: Emergency brake for 2s");
    DjiTest_WidgetLogAppend("--> Step 8: Emergency brake for 2s");
    returnCode = DjiFlightController_ExecuteEmergencyBrakeAction();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Emergency brake failed, error code: 0x%08X", returnCode);
        goto out;
    }
    s_osalHandler->TaskSleepMs(2000);
    returnCode = DjiFlightController_CancelEmergencyBrakeAction();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Cancel emergency brake action failed, error code: 0x%08X", returnCode);
        goto out;
    }

    USER_LOG_INFO(
        "--> Step 9: Move with north:-1.6(m/s), east:-2(m/s), up:0(m/s), yaw:0(deg/s) from current point for 2.2s!");
    DjiTest_WidgetLogAppend(
        "--> Step 9: Move with north:-1.6(m/s), east:-2(m/s), up:0(m/s), yaw:0(deg/s) from current point for 2.2s!");
    DjiTest_FlightControlVelocityAndYawRateCtrl((T_DjiTestFlightControlVector3f) {-1.6, -2, 0}, 0, 2200);

    USER_LOG_INFO("--> Step 10: Emergency brake for 2s");
    DjiTest_WidgetLogAppend("--> Step 10: Emergency brake for 2s");
    returnCode = DjiFlightController_ExecuteEmergencyBrakeAction();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Emergency brake failed, error code: 0x%08X", returnCode);
        goto out;
    }
    s_osalHandler->TaskSleepMs(2000);
    returnCode = DjiFlightController_CancelEmergencyBrakeAction();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Cancel emergency brake action failed, error code: 0x%08X", returnCode);
        goto out;
    }

    USER_LOG_INFO("--> Step 11: Landing\r\n");
    DjiTest_WidgetLogAppend("--> Step 11: Landing\r\n");
    if (!DjiTest_FlightControlMonitoredLanding()) {
        USER_LOG_ERROR("Landing failed");
        goto out;
    }
    USER_LOG_INFO("Successful landing\r\n");
    DjiTest_WidgetLogAppend("Successful landing\r\n");

    USER_LOG_INFO("--> Step 12: Release joystick authority");
    DjiTest_WidgetLogAppend("--> Step 12: Release joystick authority");
    returnCode = DjiFlightController_ReleaseJoystickCtrlAuthority();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Release joystick authority failed, error code: 0x%08X", returnCode);
        goto out;
    }

out:
    USER_LOG_INFO("Flight control move-by-velocity sample end");
    DjiTest_WidgetLogAppend("Flight control move-by-velocity sample end");
}

void DjiTest_FlightControlArrestFlyingSample()
{
    T_DjiReturnCode returnCode;

    USER_LOG_INFO("Flight control arrest-flying sample start");
    DjiTest_WidgetLogAppend("Flight control arrest-flying sample start");

    USER_LOG_INFO("--> Step 1: Enable arrest-flying");
    DjiTest_WidgetLogAppend("--> Step 1: Enable arrest-flying");
    returnCode = DjiFlightController_ArrestFlying();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Enable arrest-flying failed, error code: 0x%08X", returnCode);
        goto out;
    }
    s_osalHandler->TaskSleepMs(2000);

    //you can replace with takeoff to test in air.
    USER_LOG_INFO("--> Step 2: Turn on motors\r\n");
    DjiTest_WidgetLogAppend("--> Step 2: Turn on motors\r\n");
    returnCode = DjiFlightController_TurnOnMotors();
    if (returnCode == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Turn on motors successfully, but arrest-flying failed");
        s_osalHandler->TaskSleepMs(4000);
        USER_LOG_INFO("--> Step 3: Turn off motors\r\n");
        DjiTest_WidgetLogAppend("--> Step 3: Turn off motors\r\n");
        returnCode = DjiFlightController_TurnOffMotors();
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Turn off motors failed, error code: 0x%08X", returnCode);
        }
        goto out;
    }

    USER_LOG_INFO("Turn on motors failed.Arrest-flying successfully\r\n");
    DjiTest_WidgetLogAppend("Turn on motors failed.Arrest-flying successfully\r\n");
    s_osalHandler->TaskSleepMs(2000);

    USER_LOG_INFO("--> Step 3: Disable arrest-flying");
    DjiTest_WidgetLogAppend("--> Step 3: Disable arrest-flying");
    returnCode = DjiFlightController_CancelArrestFlying();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Disable arrest-flying failed, error code: 0x%08X", returnCode);
        goto out;
    }
    s_osalHandler->TaskSleepMs(2000);

    USER_LOG_INFO("--> Step 4: Turn on motors\r\n");
    DjiTest_WidgetLogAppend("--> Step 4: Turn on motors\r\n");
    returnCode = DjiFlightController_TurnOnMotors();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Turn on motors failed and disable arrest-flying failed, error code: 0x%08X", returnCode);
        goto out;
    } else {
        USER_LOG_INFO("Turn on motors successfully and disable arrest-flying successfully\r\n");
        s_osalHandler->TaskSleepMs(4000);
        USER_LOG_INFO("--> Step 5: Turn off motors");
        DjiTest_WidgetLogAppend("--> Step 5: Turn off motors");
        returnCode = DjiFlightController_TurnOffMotors();
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Turn off motors failed, error code: 0x%08X", returnCode);
        }
    }

out:
    USER_LOG_INFO("Flight control arrest-flying sample end");
    DjiTest_WidgetLogAppend("Flight control arrest-flying sample end");
}

void DjiTest_FlightControlSetGetParamSample()
{
    T_DjiReturnCode returnCode;
    E_DjiFlightControllerObstacleAvoidanceEnableStatus horizontalVisualObstacleAvoidanceStatus;
    E_DjiFlightControllerObstacleAvoidanceEnableStatus horizontalRadarObstacleAvoidanceStatus;
    E_DjiFlightControllerObstacleAvoidanceEnableStatus upwardsVisualObstacleAvoidanceStatus;
    E_DjiFlightControllerObstacleAvoidanceEnableStatus upwardsRadarObstacleAvoidanceStatus;
    E_DjiFlightControllerObstacleAvoidanceEnableStatus downloadsVisualObstacleAvoidanceStatus;
    E_DjiFlightControllerGoHomeAltitude goHomeAltitude;
    E_DjiFlightControllerRtkPositionEnableStatus rtkEnableStatus;
    E_DjiFlightControllerRCLostAction rcLostAction;
    T_DjiAircraftInfoBaseInfo aircraftInfoBaseInfo;
    E_DjiFlightControllerCloseAllAvoidCommand allAvoidStatus;
    uint16_t countryCode;
    E_DjiFlightControllerRCLostActionEnableStatus rcLostActionStatus;
    E_DjiFlightControllerHoistActionInFlightCommand enableHoistActionStatus;

    returnCode = DjiAircraftInfo_GetBaseInfo(&aircraftInfoBaseInfo);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("get aircraft base info error");
    }

    USER_LOG_INFO("Flight control set-get-param sample start");
    DjiTest_WidgetLogAppend("Flight control set-get-param sample start");

    returnCode = DjiFlightController_GetCountryCode(&countryCode);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Get aircraft country code error.");
    }
    USER_LOG_INFO("country code: %hd", countryCode);

    /*! Turn on horizontal vision avoid enable */
    USER_LOG_INFO("--> Step 1: Turn on horizontal visual obstacle avoidance");
    DjiTest_WidgetLogAppend("--> Step 1: Turn on horizontal visual obstacle avoidance");
    returnCode = DjiFlightController_SetHorizontalVisualObstacleAvoidanceEnableStatus(
        DJI_FLIGHT_CONTROLLER_ENABLE_OBSTACLE_AVOIDANCE);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Turn on horizontal visual obstacle avoidance failed, error code: 0x%08X", returnCode);
        goto out;
    };
    s_osalHandler->TaskSleepMs(1000);

    USER_LOG_INFO("--> Step 2: Get horizontal horizontal visual obstacle status\r\n");
    DjiTest_WidgetLogAppend("--> Step 2: Get horizontal horizontal visual obstacle status\r\n");
    returnCode = DjiFlightController_GetHorizontalVisualObstacleAvoidanceEnableStatus(
        &horizontalVisualObstacleAvoidanceStatus);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Get horizontal visual obstacle avoidance failed, error code: 0x%08X", returnCode);
        goto out;
    }
    USER_LOG_INFO("Current horizontal visual obstacle avoidance status is %d\r\n",
                  horizontalVisualObstacleAvoidanceStatus);
    s_osalHandler->TaskSleepMs(1000);

    /*! Turn on horizontal radar avoid enable */
    USER_LOG_INFO("--> Step 3: Turn on horizontal radar obstacle avoidance");
    DjiTest_WidgetLogAppend("--> Step 3: Turn on horizontal radar obstacle avoidance");
    if (aircraftInfoBaseInfo.aircraftType == DJI_AIRCRAFT_TYPE_M300_RTK ||
        aircraftInfoBaseInfo.aircraftType == DJI_AIRCRAFT_TYPE_M30 ||
        aircraftInfoBaseInfo.aircraftType == DJI_AIRCRAFT_TYPE_M30T ||
        aircraftInfoBaseInfo.aircraftType == DJI_AIRCRAFT_TYPE_M350_RTK ||
        aircraftInfoBaseInfo.aircraftType == DJI_AIRCRAFT_TYPE_FC30) {
        returnCode = DjiFlightController_SetHorizontalRadarObstacleAvoidanceEnableStatus(
            DJI_FLIGHT_CONTROLLER_ENABLE_OBSTACLE_AVOIDANCE);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Turn on horizontal radar obstacle avoidance failed, error code: 0x%08X", returnCode);
            goto out;
        };
    }
    s_osalHandler->TaskSleepMs(1000);

    USER_LOG_INFO("--> Step 4: Get horizontal radar obstacle avoidance status\r\n");
    DjiTest_WidgetLogAppend("--> Step 4: Get horizontal radar obstacle avoidance status\r\n");
    if (aircraftInfoBaseInfo.aircraftType == DJI_AIRCRAFT_TYPE_M300_RTK ||
        aircraftInfoBaseInfo.aircraftType == DJI_AIRCRAFT_TYPE_M30 ||
        aircraftInfoBaseInfo.aircraftType == DJI_AIRCRAFT_TYPE_M30T ||
        aircraftInfoBaseInfo.aircraftType == DJI_AIRCRAFT_TYPE_M350_RTK ||
        aircraftInfoBaseInfo.aircraftType == DJI_AIRCRAFT_TYPE_FC30 ) {
        returnCode = DjiFlightController_GetHorizontalRadarObstacleAvoidanceEnableStatus(
            &horizontalRadarObstacleAvoidanceStatus);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Get horizontal radar obstacle avoidance failed, error code: 0x%08X", returnCode);
            goto out;
        }
        USER_LOG_INFO("Current horizontal radar obstacle avoidance status is %d\r\n",
                      horizontalRadarObstacleAvoidanceStatus);
    }
    s_osalHandler->TaskSleepMs(1000);

    /*! Turn on upwards vision avoid enable */
    USER_LOG_INFO("--> Step 5: Turn on upwards visual obstacle avoidance.");
    DjiTest_WidgetLogAppend("--> Step 5: Turn on upwards visual obstacle avoidance.");
    returnCode = DjiFlightController_SetUpwardsVisualObstacleAvoidanceEnableStatus(
        DJI_FLIGHT_CONTROLLER_ENABLE_OBSTACLE_AVOIDANCE);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Turn on upwards visual obstacle avoidance failed, error code: 0x%08X", returnCode);
        goto out;
    };
    s_osalHandler->TaskSleepMs(1000);

    USER_LOG_INFO("--> Step 6: Get upwards visual obstacle avoidance status\r\n");
    DjiTest_WidgetLogAppend("--> Step 6: Get upwards visual obstacle avoidance status\r\n");
    returnCode = DjiFlightController_GetUpwardsVisualObstacleAvoidanceEnableStatus(
        &upwardsVisualObstacleAvoidanceStatus);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Get upwards visual obstacle avoidance failed, error code: 0x%08X", returnCode);
        goto out;
    }
    USER_LOG_INFO("Current upwards visual obstacle avoidance status is %d\r\n", upwardsVisualObstacleAvoidanceStatus);
    s_osalHandler->TaskSleepMs(1000);

    /*! Turn on upwards radar avoid enable */
    USER_LOG_INFO("--> Step 7: Turn on upwards radar obstacle avoidance.");
    DjiTest_WidgetLogAppend("--> Step 7: Turn on upwards radar obstacle avoidance.");
    if (aircraftInfoBaseInfo.aircraftType == DJI_AIRCRAFT_TYPE_M300_RTK ||
        aircraftInfoBaseInfo.aircraftType == DJI_AIRCRAFT_TYPE_M30 ||
        aircraftInfoBaseInfo.aircraftType == DJI_AIRCRAFT_TYPE_M30T ||
        aircraftInfoBaseInfo.aircraftType == DJI_AIRCRAFT_TYPE_M350_RTK ||
        aircraftInfoBaseInfo.aircraftType == DJI_AIRCRAFT_TYPE_FC30) {
        returnCode = DjiFlightController_SetUpwardsRadarObstacleAvoidanceEnableStatus(
            DJI_FLIGHT_CONTROLLER_ENABLE_OBSTACLE_AVOIDANCE);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Turn on upwards radar obstacle avoidance failed, error code: 0x%08X", returnCode);
            goto out;
        }
    }
    s_osalHandler->TaskSleepMs(1000);

    USER_LOG_INFO("--> Step 8: Get upwards radar obstacle avoidance status\r\n");
    DjiTest_WidgetLogAppend("--> Step 8: Get upwards radar obstacle avoidance status\r\n");
    if (aircraftInfoBaseInfo.aircraftType == DJI_AIRCRAFT_TYPE_M300_RTK ||
        aircraftInfoBaseInfo.aircraftType == DJI_AIRCRAFT_TYPE_M30 ||
        aircraftInfoBaseInfo.aircraftType == DJI_AIRCRAFT_TYPE_M30T ||
        aircraftInfoBaseInfo.aircraftType == DJI_AIRCRAFT_TYPE_M350_RTK ||
        aircraftInfoBaseInfo.aircraftType == DJI_AIRCRAFT_TYPE_FC30) {
        returnCode = DjiFlightController_GetUpwardsRadarObstacleAvoidanceEnableStatus(
            &upwardsRadarObstacleAvoidanceStatus);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Get upwards radar obstacle avoidance failed, error code: 0x%08X", returnCode);
            goto out;
        }
        USER_LOG_INFO("Current upwards radar obstacle avoidance status is %d\r\n", upwardsRadarObstacleAvoidanceStatus);
        s_osalHandler->TaskSleepMs(1000);
    }

    /*! Turn on downwards vision avoid enable */
    USER_LOG_INFO("--> Step 9: Turn on downwards visual obstacle avoidance.");
    DjiTest_WidgetLogAppend("--> Step 9: Turn on downwards visual obstacle avoidance.");
    returnCode = DjiFlightController_SetDownwardsVisualObstacleAvoidanceEnableStatus(
        DJI_FLIGHT_CONTROLLER_ENABLE_OBSTACLE_AVOIDANCE);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Turn on downwards visual obstacle avoidance failed, error code: 0x%08X", returnCode);
        goto out;
    }
    s_osalHandler->TaskSleepMs(1000);

    USER_LOG_INFO("--> Step 10: Get downwards visual obstacle avoidance status\r\n");
    DjiTest_WidgetLogAppend("--> Step 10: Get downwards visual obstacle avoidance status\r\n");
    returnCode = DjiFlightController_GetDownwardsVisualObstacleAvoidanceEnableStatus(
        &downloadsVisualObstacleAvoidanceStatus);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Get downwards visual obstacle avoidance failed, error code: 0x%08X", returnCode);
        goto out;
    }
    USER_LOG_INFO("Current downwards visual obstacle avoidance status is %d\r\n",
                  downloadsVisualObstacleAvoidanceStatus);
    s_osalHandler->TaskSleepMs(1000);

    /*! Set new go home altitude */
    USER_LOG_INFO("--> Step 11: Set go home altitude to 50(m)");
    DjiTest_WidgetLogAppend("--> Step 11: Set go home altitude to 50(m)");
    returnCode = DjiFlightController_SetGoHomeAltitude(50);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Set go home altitude to 50(m) failed, error code: 0x%08X", returnCode);
        goto out;
    }
    s_osalHandler->TaskSleepMs(1000);

    /*! get go home altitude */
    USER_LOG_INFO("--> Step 12: Get go home altitude\r\n");
    DjiTest_WidgetLogAppend("--> Step 12: Get go home altitude\r\n");
    returnCode = DjiFlightController_GetGoHomeAltitude(&goHomeAltitude);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Get go home altitude failed, error code: 0x%08X", returnCode);
        goto out;
    }
    USER_LOG_INFO("Current go home altitude is %d m\r\n", goHomeAltitude);
    s_osalHandler->TaskSleepMs(2000);

    /*! Set rtk enable */
    USER_LOG_INFO("--> Step 13: Set rtk enable status");
    DjiTest_WidgetLogAppend("--> Step 13: Set rtk enable status");
    returnCode = DjiFlightController_SetRtkPositionEnableStatus(DJI_FLIGHT_CONTROLLER_ENABLE_RTK_POSITION);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Set rtk enable failed, error code: 0x%08X", returnCode);
        goto out;
    }
    s_osalHandler->TaskSleepMs(1000);

    USER_LOG_INFO("--> Step 14: Get rtk enable status\r\n");
    DjiTest_WidgetLogAppend("--> Step 14: Get rtk enable status\r\n");
    returnCode = DjiFlightController_GetRtkPositionEnableStatus(&rtkEnableStatus);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Get rtk enable failed, error code: 0x%08X", returnCode);
        goto out;
    }
    USER_LOG_INFO("Current rtk enable status is %d\r\n", rtkEnableStatus);
    s_osalHandler->TaskSleepMs(1000);

    /*! Set rc lost action */
    if (aircraftInfoBaseInfo.aircraftType != DJI_AIRCRAFT_TYPE_M300_RTK &&
        aircraftInfoBaseInfo.aircraftType != DJI_AIRCRAFT_TYPE_M350_RTK) {
        USER_LOG_INFO("--> Step 15: Set rc lost action");
        DjiTest_WidgetLogAppend("--> Step 15: Set rc lost action");
        returnCode = DjiFlightController_SetRCLostAction(DJI_FLIGHT_CONTROLLER_RC_LOST_ACTION_GOHOME);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Set rc lost action failed, error code: 0x%08X", returnCode);
            goto out;
        }
        s_osalHandler->TaskSleepMs(1000);

        USER_LOG_INFO("--> Step 16: Get rc lost action\r\n");
        DjiTest_WidgetLogAppend("--> Step 16: Get rc lost action\r\n");
        returnCode = DjiFlightController_GetRCLostAction(&rcLostAction);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Get rc lost action failed, error code: 0x%08X", returnCode);
            goto out;
        }
        USER_LOG_INFO("Current rc lost action is %d\r\n", rcLostAction);
        DjiTest_WidgetLogAppend("Current rc lost action is %d\r\n", rcLostAction);
        s_osalHandler->TaskSleepMs(1000);
    }
    if (aircraftInfoBaseInfo.aircraftType == DJI_AIRCRAFT_TYPE_FC100 ||
        aircraftInfoBaseInfo.aircraftType == DJI_AIRCRAFT_TYPE_FC200){
        returnCode = DjiFlightController_SetAllAvoidAction(DJI_FLIGHT_CONTROLLER_ENABLE_ALL_AVIOD);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS){
            USER_LOG_ERROR("--> Step 17: Turn on set all avoidance for AG: %d failed", returnCode);
            goto out;
        }
        USER_LOG_INFO("--> Step 17: Turn on set all avoidance for return AG: %d success", returnCode);
        s_osalHandler->TaskSleepMs(1000);

        returnCode = DjiFlightController_GetAllAvoidAction(&allAvoidStatus);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS){
            USER_LOG_ERROR("--> Step 17: Turn on get all avoidance for AG: %d failed", returnCode);
            goto out;
        }
        USER_LOG_INFO("--> Step 17: Turn on get all avoidance for result AG: %d success", allAvoidStatus);

        s_osalHandler->TaskSleepMs(1000);
        returnCode = DjiFlightController_SetRCLostActionEnableStatus(DJI_FLIGHT_CONTROLLER_DISABLE_RC_LOST_ACTION);{
            if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS){
                USER_LOG_ERROR("--> Step 18: Turn on set all avoidance for AG: %d failed", returnCode);
                goto out;
            }else{
                USER_LOG_INFO("--> Step 18: Turn on set all avoidance for AG: %d success", returnCode);
                s_osalHandler->TaskSleepMs(1000);
                returnCode = DjiFlightController_GetEnableRCLostActionStatus(&rcLostActionStatus);
                USER_LOG_INFO("--> Step 18: Turn on get GetEnableRCLostActionStatus:%d, ret:%d", rcLostActionStatus), returnCode;
            }
        }

        s_osalHandler->TaskSleepMs(1000);
        returnCode = DjiFlightController_SetHoistActionInFlight(DJI_FLIGHT_CONTROLLER_ENABLE_HOIST_OPTION_IN_FLIGHT);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS){
            USER_LOG_ERROR("--> Step 19: Turn on SetHoistActionInFlight for AG: %d failed", returnCode);
            goto out;
        }
        USER_LOG_INFO("--> Step 19: Turn on SetHoistActionInFlight for return AG success!!!");
        s_osalHandler->TaskSleepMs(1000);

        returnCode = DjiFlightController_GetHoistActionInFlight(&enableHoistActionStatus);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS){
            USER_LOG_ERROR("--> Step 19: Turn on GetHoistActionInFlight for AG: %d failed", returnCode);
            goto out;
        }
        USER_LOG_INFO("--> Step 19: Turn on GetHoistActionInFlight for result AG: %d success", enableHoistActionStatus);
    }

out:
    USER_LOG_INFO("Flight control set-get-param sample end");
    DjiTest_WidgetLogAppend("Flight control set-get-param sample end");
}

void DjiTest_FlightControlSetGetPerceptionParamSample()
{
    T_DjiReturnCode returnCode;
    uint16_t exit_reason;
    bool ctrl_mode;
    T_DjiAircraftInfoBaseInfo aircraftInfoBaseInfo;

    returnCode = DjiAircraftInfo_GetBaseInfo(&aircraftInfoBaseInfo);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("get aircraft base info error");
        return;
    }

    switch (aircraftInfoBaseInfo.aircraftType)
    {
        case DJI_AIRCRAFT_TYPE_M4T:
        case DJI_AIRCRAFT_TYPE_M4E:
        case DJI_AIRCRAFT_TYPE_M4D:
        case DJI_AIRCRAFT_TYPE_M4TD:
        case DJI_AIRCRAFT_TYPE_M400:
            break;

        default:
            USER_LOG_WARN("aircraft type %d not support", aircraftInfoBaseInfo.aircraftType);
            return;
    }
    USER_LOG_INFO("Flight control set-get-perception-param sample start");
    DjiTest_WidgetLogAppend("Flight control set-get-perception-param sample start");

    /* perception parameter test*/
    USER_LOG_INFO("--> Step 1: DjiFlightController_SetPlanningAlgo");
    DjiTest_WidgetLogAppend("--> Step 1: DjiFlightController_SetPlanningAlgo");
    returnCode = DjiFlightController_SetPlanningAlgo(1);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("DjiFlightController_SetPlanningAlgo failed, error code: 0x%08X", returnCode);
    };
    s_osalHandler->TaskSleepMs(1000);

    USER_LOG_INFO("--> Step 2: DjiFlightController_SetMaxVelocity");
    DjiTest_WidgetLogAppend("--> Step 2: DjiFlightController_SetMaxVelocity");
    returnCode = DjiFlightController_SetMaxVelocity(10);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("DjiFlightController_SetMaxVelocity failed, error code: 0x%08X", returnCode);
    };
    s_osalHandler->TaskSleepMs(1000);

    USER_LOG_INFO("--> Step 3: DjiFlightController_SetMinFlightHeight");
    DjiTest_WidgetLogAppend("--> Step 3: DjiFlightController_SetMinFlightHeight");
    returnCode = DjiFlightController_SetMinFlightHeight(50.0f);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("DjiFlightController_SetMinFlightHeight failed, error code: 0x%08X", returnCode);
    };
    s_osalHandler->TaskSleepMs(1000);

    USER_LOG_INFO("--> Step 4: DjiFlightController_GetExitReason");
    DjiTest_WidgetLogAppend("--> Step 4: DjiFlightController_GetExitReason");
    returnCode = DjiFlightController_GetExitReason(&exit_reason);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("DjiFlightController_GetExitReason failed, error code: 0x%08X", returnCode);
    };
    USER_LOG_INFO("DjiFlightController_GetExitReason is %d\r\n", exit_reason);
    s_osalHandler->TaskSleepMs(1000);

    if (aircraftInfoBaseInfo.aircraftType == DJI_AIRCRAFT_TYPE_M4T ||
        aircraftInfoBaseInfo.aircraftType == DJI_AIRCRAFT_TYPE_M4E) {
        USER_LOG_INFO("--> Step 5: DjiFlightController_SetControlInAttitudeModeEnabled mode enable");
        DjiTest_WidgetLogAppend("--> Step 5: DjiFlightController_SetControlInAttitudeModeEnabled mode enable");
        returnCode = DjiFlightController_SetControlInAttitudeModeEnabled(true);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("DjiFlightController_SetControlInAttitudeModeEnabled failed, error code: 0x%08X", returnCode);
        };
        returnCode = DjiFlightController_GetControlInAttitudeModeEnabled(&ctrl_mode);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("DjiFlightController_GetControlInAttitudeModeEnabled failed, error code: 0x%08X", returnCode);
        };
        USER_LOG_INFO("DjiFlightController_GetSdkCtrlModeInAtti is %d", ctrl_mode);
        s_osalHandler->TaskSleepMs(1000);

        USER_LOG_INFO("--> Step 6: DjiFlightController_SetControlInAttitudeModeEnabled mode disable");
        DjiTest_WidgetLogAppend("--> Step 6: DjiFlightController_SetControlInAttitudeModeEnabled mode disable");
        returnCode = DjiFlightController_SetControlInAttitudeModeEnabled(false);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("DjiFlightController_SetControlInAttitudeModeEnabled failed, error code: 0x%08X", returnCode);
        };
        returnCode = DjiFlightController_GetControlInAttitudeModeEnabled(&ctrl_mode);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("DjiFlightController_GetControlInAttitudeModeEnabled failed, error code: 0x%08X", returnCode);
        };
        USER_LOG_INFO("DjiFlightController_GetControlInAttitudeModeEnabled is %d", ctrl_mode);
        s_osalHandler->TaskSleepMs(1000);
    }

out:
    USER_LOG_INFO("Flight control set-get-perception-param sample end");
    DjiTest_WidgetLogAppend("Flight control set-get-perception-param perceptionsample end");

}

T_DjiReturnCode DjiTest_FlightControlOpenMisInfoCallback(T_DjiFlightControllerOpenMis eventData)
{
    s_mission_state_machine = eventData.mission_state_machine;
    USER_LOG_INFO("OpenMisInfoCallback state_machine = %d, planning_algo = %d, goal_index = %d, distance_remaining = %f, time_remaining = %f, soe_remaining = %d",
                  eventData.mission_state_machine, eventData.mission_planning_algo, eventData.goal_index,
                  eventData.distance_remaining, eventData.time_remaining, eventData.soe_remaining);
}

T_DjiReturnCode DjiTest_FlightControlCoreTrajCallback(T_DjiFlightControllerCoreTraj eventData)
{
    USER_LOG_INFO("DjiTest_FlightControlCoreTrajCallback code_name = %d, point_num = %d, byte_per_point = %d",
                  eventData.code_name, eventData.point_num, eventData.byte_per_point);
}
void DjiTest_FlightControlSetModeStartMission()
{
    T_DjiReturnCode returnCode;
    T_DjiFlightControllerStartMissionReq req = {0};
    T_DjiFlightControllerStartMissionRsp rsp = {0};
    T_DjiAircraftInfoBaseInfo aircraftInfoBaseInfo;

    returnCode = DjiAircraftInfo_GetBaseInfo(&aircraftInfoBaseInfo);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("get aircraft base info error");
        return;
    }

    switch (aircraftInfoBaseInfo.aircraftType)
    {
        case DJI_AIRCRAFT_TYPE_M4T:
        case DJI_AIRCRAFT_TYPE_M4E:
        case DJI_AIRCRAFT_TYPE_M4D:
        case DJI_AIRCRAFT_TYPE_M4TD:
        case DJI_AIRCRAFT_TYPE_M400:
            break;

        default:
            USER_LOG_WARN("aircraft type %d not support", aircraftInfoBaseInfo.aircraftType);
            return;
    }

    req.version = 1;
    req.operation = 0;
    req.mea = 10.0f;
    req.fly_vel = 10;
    req.goal_num = 1;
    req.cmd_mode_point_info->lat = 22.578111231;
    req.cmd_mode_point_info->lon = 113.93696;
    req.cmd_mode_point_info->alt = 50.0;

    USER_LOG_INFO("Flight control SetModeStartMission sample start");
    DjiTest_WidgetLogAppend("Flight control SetModeStartMission sample start");

    returnCode = DjiFlightController_SetModeStartMission(req, &rsp);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("DjiTest_FlightControlSetModeStartMission failed, error code: 0x%08X", returnCode);
    } else {
        USER_LOG_INFO("DjiTest_FlightControlSetModeStartMission success ret = %d", returnCode);
    }

    USER_LOG_INFO("Flight control SetModeStartMission sample end");
    DjiTest_WidgetLogAppend("Flight control SetModeStartMission sample end");


    USER_LOG_INFO("Flight control register callback sample start");
    DjiTest_WidgetLogAppend("Flight control register callback sample start");

    returnCode = DjiFlightController_RegisterOpenMisInfoCallBack(DjiTest_FlightControlOpenMisInfoCallback);
    USER_LOG_INFO("RegisterOpenMisInfoCallBack ret = %d", returnCode);
    returnCode = DjiFlightController_RegisterCoreTrajCallBack(DjiTest_FlightControlCoreTrajCallback);
    USER_LOG_INFO("RegisterCoreTrajCallBack ret = %d", returnCode);

    s_osalHandler->TaskSleepMs(10000);

    while (true) {
        if (s_mission_state_machine == 0) {
            USER_LOG_INFO("Mission is finished.");
            break;
        }
        s_osalHandler->TaskSleepMs(1000);
    }

    DjiFlightController_AntiRegisterCoreTrajCallBack();
    DjiFlightController_AntiRegisterOpenMisInfoCallBack();
    USER_LOG_INFO("Flight control register callback sample end");
    DjiTest_WidgetLogAppend("Flight control register callback sample end");

    return;
}

T_DjiReturnCode DjiTest_FlightControlOsdInfoCallback(T_DjiFlightControllerOsdInfo eventData)
{
    USER_LOG_INFO("[OSD] relative_height=%d mode=%u heading_level_ok=%u",
                  eventData.relative_height,
                  eventData.mode,
                  eventData.heading_level_ok);
    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiTest_FlightControlRegisterOsdInfoSample(void)
{
    T_DjiReturnCode returnCode;

    returnCode = DjiFlightController_RegisterOsdInfoCallBack(DjiTest_FlightControlOsdInfoCallback);
    USER_LOG_INFO("RegisterOsdInfoCallBack ret = %d", returnCode);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        return returnCode;
    }

    /* Wait to receive OSD pushes before returning (10 Hz → ~10 callbacks in 1s) */
    USER_LOG_INFO("Waiting 3s for OSD callbacks...");
    s_osalHandler->TaskSleepMs(3000);
    USER_LOG_INFO("OSD sample done, unregistering callback.");
    DjiFlightController_UnregisterOsdInfoCallBack();

    return returnCode;
}

T_DjiReturnCode DjiTest_FlightControlStealthModeSample(void)
{
    T_DjiReturnCode returnCode;
    E_DjiFlightControllerLightStatus status = 0;
    E_DjiFlightControllerLightStatus armModeSelect = 0;
    E_DjiFlightControllerLightStatus bottomModeSelect = DJI_FLIGHT_CONTROLLER_LIGHT_AUTO;
    E_DjiFlightControllerLightStatus bottomModeCurrent = DJI_FLIGHT_CONTROLLER_LIGHT_AUTO;
    E_DjiFlightControllerLightStatus batteryModeSelect = 0;

    USER_LOG_INFO("--> Please select the armlight mode you want to choose");
    USER_LOG_INFO("--> 0. Turn off light  1. Turn on light");
    DjiTest_WidgetLogAppend("--> Please select the armlight mode you want to choose");
    DjiTest_WidgetLogAppend("--> 0. Turn off light  1. Turn on light");

    scanf("%d", &armModeSelect);
    if (armModeSelect >= DJI_FLIGHT_CONTROLLER_LIGHT_STATUS_MAX) {
        USER_LOG_ERROR("Invalid light mode");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    USER_LOG_INFO("--> Please select the bottomlight mode you want to choose");
    USER_LOG_INFO("--> 0. Auto  1. Always on (flight on / land off)  2. Always off");
    DjiTest_WidgetLogAppend("--> Please select the bottomlight mode you want to choose");
    DjiTest_WidgetLogAppend("--> 0. Auto  1. Always on (flight on / land off)  2. Always off");

    scanf("%d", (int *)&bottomModeSelect);
    if ((int)bottomModeSelect < 0 || bottomModeSelect >= DJI_FLIGHT_CONTROLLER_LIGHT_STATUS_MAX) {
        USER_LOG_ERROR("Invalid bottom light mode");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    USER_LOG_INFO("--> Please select the batterylight mode you want to choose");
    USER_LOG_INFO("--> 0. Turn off light  1. Turn on light");
    DjiTest_WidgetLogAppend("--> Please select the batterylight mode you want to choose");
    DjiTest_WidgetLogAppend("--> 0. Turn off light  1. Turn on light");

    scanf("%d", &batteryModeSelect);
    if (batteryModeSelect >= DJI_FLIGHT_CONTROLLER_LIGHT_STATUS_MAX) {
        USER_LOG_ERROR("Invalid light mode");
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    USER_LOG_INFO("---------------------------------------" );
    status = DJI_FLIGHT_CONTROLLER_LIGHT_STATUS_MAX;
    returnCode = DjiFlightController_GetArmLightStatus(&status);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Get arm lights status failed, error code: %llu", returnCode);
    };
    if (status != armModeSelect)
    {
        USER_LOG_INFO("start control arm light, status: %d", armModeSelect);
        returnCode = DjiFlightController_SetArmLightStatus(armModeSelect);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Set arm lights status failed, error code: %llu", returnCode);
        } else {
            USER_LOG_INFO("Set arm lights status success, status: %d", armModeSelect);
        }
    }

    status = DJI_FLIGHT_CONTROLLER_LIGHT_STATUS_MAX;
    returnCode = DjiFlightController_GetBatteryLightStatus(&status);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Set battery lights status failed, error code: %llu", returnCode);
    };
    if (status != batteryModeSelect)
    {
        USER_LOG_INFO("start control battery light, status: %d", batteryModeSelect);
        returnCode = DjiFlightController_SetBatteryLightStatus(batteryModeSelect);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Set battery lights status failed, error code: %llu", returnCode);
        } else {
            USER_LOG_INFO("Set battery lights status success, status: %d", batteryModeSelect);
        }
    }

    returnCode = DjiFlightController_GetBottomLightStatus(&bottomModeCurrent);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Get bottom light status failed, error code: %llu", returnCode);
    };
    if (bottomModeCurrent != bottomModeSelect)
    {
        USER_LOG_INFO("start control bottom light, status: %d", bottomModeSelect);
        returnCode = DjiFlightController_SetBottomLightStatus(bottomModeSelect);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Set bottom light status failed, error code: %llu", returnCode);
        } else {
            USER_LOG_INFO("Set bottom light status success, status: %d", bottomModeSelect);
        }
    }

    return returnCode;
}

 T_DjiReturnCode DjiTest_FlightControlNoGnssHeadingGoHomeSample(void)
{
    T_DjiReturnCode returnCode;
    E_DjiFlightControllerRCLostAction rcLostAction;
    int32_t failsafeHeight = -1;
    int32_t height = 0;

    USER_LOG_INFO("Flight control NoGnss-mode param sample start");

    /*! -----------------------------------------------------------------------
     * Step 1: Set rc_lost_action to go up
     * ----------------------------------------------------------------------- */
    USER_LOG_INFO("--> Step 1: Set rc_lost_action to go up");
    returnCode = DjiFlightController_SetRCLostAction(DJI_FLIGHT_CONTROLLER_RC_LOST_ACTION_GO_UP);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Set rc_lost_action failed, error code: 0x%08X", returnCode);
        goto out;
    }
    s_osalHandler->TaskSleepMs(500);

    /*! -----------------------------------------------------------------------
     * Step 2: Read back rc_lost_action to verify
     * ----------------------------------------------------------------------- */
    USER_LOG_INFO("--> Step 2: Get rc_lost_action to verify");
    returnCode = DjiFlightController_GetRCLostAction(&rcLostAction);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Get rc_lost_action failed, error code: 0x%08X", returnCode);
        goto out;
    }
    USER_LOG_INFO("rc_lost_action readback = %d --> %s",
                  rcLostAction,
                  rcLostAction == DJI_FLIGHT_CONTROLLER_RC_LOST_ACTION_GO_UP ? "PASS" : "FAIL");
    s_osalHandler->TaskSleepMs(500);

    /*! -----------------------------------------------------------------------
     * Step 3: Set RC-lost go-up height
     * ----------------------------------------------------------------------- */
    USER_LOG_INFO("--> Step 3: Set RC-lost go-up height");
    USER_LOG_INFO("Enter height (m): ");
    scanf("%d", &height);
    returnCode = DjiFlightController_SetRCLostGoUpHeight(height);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Set RC-lost go-up height failed, error code: 0x%08X", returnCode);
        goto out;
    }
    s_osalHandler->TaskSleepMs(500);

    /*! -----------------------------------------------------------------------
     * Step 4: Read back RC-lost go-up height to verify
     * ----------------------------------------------------------------------- */
    USER_LOG_INFO("--> Step 4: Get RC-lost go-up height to verify");
    returnCode = DjiFlightController_GetRCLostGoUpHeight(&failsafeHeight);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Get RC-lost go-up height failed, error code: 0x%08X", returnCode);
        goto out;
    }
    USER_LOG_INFO("RC-lost go-up height readback = %d m (expect %d) --> %s",
                  failsafeHeight, height,
                  failsafeHeight == height ? "PASS" : "FAIL");

out:
    USER_LOG_INFO("Flight control NoGnss-mode param sample end");
    return returnCode;
}

static T_DjiReturnCode DjiTest_AssistPosModeInfoCallback(T_DjiWaypointV3AssistPosModeInfo info)
{
    USER_LOG_INFO("[AssistPosModeInfo] gpsErr=%.3fm sdkErr=%.3fm gpsSdkErr=%.3fm divergeErr=%.3fm gnssValid=%u",
                  info.gpsError, info.sdkError, info.gpsSdkError, info.divergeError, info.isGnssValid);
    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiTest_FlightControlNoGpsModeSample(void)
{
    T_DjiReturnCode returnCode;
    E_DjiFlightControllerMultipleMode multipleMode;
    E_DjiFlightControllerAssistPositioningMode assistMode;

    USER_LOG_INFO("Flight control no-GPS mode sample start");

    /*! -----------------------------------------------------------------------
     * Step 1: Init WaypointV3 and register AssistPosModeInfo callback
     * ----------------------------------------------------------------------- */
    USER_LOG_INFO("--> Step 1: Init WaypointV3 and register assist pos mode info callback");
    if (!s_isWaypointV3Initialized) {
        returnCode = DjiWaypointV3_Init();
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("WaypointV3 init failed, error code: 0x%08X", returnCode);
            goto out;
        }
        s_isWaypointV3Initialized = true;
    } else {
        USER_LOG_INFO("WaypointV3 already initialized, skipping Init");
    }

    returnCode = DjiWaypointV3_RegAssistPosModeInfoCallback(DjiTest_AssistPosModeInfoCallback);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Reg AssistPosModeInfo callback failed, error code: 0x%08X", returnCode);
        goto out;
    }
    USER_LOG_INFO("WaypointV3 init and callback registration done");
    s_osalHandler->TaskSleepMs(500);

    /*! -----------------------------------------------------------------------
     * Step 2: Enable ADN mode via high-level interface (sets both params atomically)
     * ----------------------------------------------------------------------- */
    USER_LOG_INFO("--> Step 2: Enable ADN mode via SetADNModeEnable(true)");
    returnCode = DjiFlightController_SetADNModeEnable(true);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("SetADNModeEnable(true) failed, error code: 0x%08X", returnCode);
        goto out;
    }
    USER_LOG_INFO("ADN mode enabled");
    s_osalHandler->TaskSleepMs(1000);

    /*! -----------------------------------------------------------------------
     * Step 3: Get multiple mode and to assist position mode to verify
     * ----------------------------------------------------------------------- */
    USER_LOG_INFO("--> Step 3.1: Get multiple mode to verify");
    returnCode = DjiFlightController_GetMultipleMode(&multipleMode);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Get multiple mode failed, error code: 0x%08X", returnCode);
        goto out;
    }
    USER_LOG_INFO("Multiple mode readback = %d (expect ENABLE) --> %s",
                  multipleMode,
                  multipleMode == DJI_FLIGHT_CONTROLLER_MULTIPLE_MODE_ENABLE ? "PASS" : "FAIL");
    s_osalHandler->TaskSleepMs(500);

    USER_LOG_INFO("--> Step 3.2: Get assist positioning mode to verify");
    returnCode = DjiFlightController_GetAssistPositioningMode(&assistMode);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Get assist positioning mode failed, error code: 0x%08X", returnCode);
        goto out;
    }
    USER_LOG_INFO("Assist positioning mode readback = %d (expect PSDK) --> %s",
                  assistMode,
                  assistMode == DJI_FLIGHT_CONTROLLER_ASSIST_POSITIONING_MODE_PSDK ? "PASS" : "FAIL");
    s_osalHandler->TaskSleepMs(500);

    /*! -----------------------------------------------------------------------
     * Step 4: Send external localization info (continuous push, test 5 frames)
     * ----------------------------------------------------------------------- */
    USER_LOG_INFO("--> Step 4: Send external localization info (5 frames)");
    for (int i = 0; i < 5; i++) {
        T_DjiWaypointV3ExternalLocalizationInfo locInfo = {
            .version      = 1,
            .longitude    = 1.9823756,   /* ~113.6° E, rad */
            .latitude     = 0.3926991,   /* ~22.5° N, rad  */
            .altitude     = 50.0f,       /* m               */
            .heading      = 0.0f,        /* 正北, °         */
            .horizError   = 0.1f,        /* m               */
            .vertError    = 0.2f,        /* m               */
            .headingError = 1.0f,        /* °               */
            .timestamp    = (uint32_t)(i * 100),  /* ms, 模拟100ms间隔 */
        };

        returnCode = DjiWaypointV3_SendExternalLocalizationInfo(&locInfo);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("SendExternalLocalizationInfo frame[%d] failed, error code: 0x%08X", i, returnCode);
            goto out;
        }
        USER_LOG_INFO("SendExternalLocalizationInfo frame[%d] sent --> PASS", i);
        s_osalHandler->TaskSleepMs(100);
    }

    /*! -----------------------------------------------------------------------
     * Step 5: Disable ADN mode via high-level interface (sets both params atomically)
     * ----------------------------------------------------------------------- */
    USER_LOG_INFO("--> Step 5: Disable ADN mode via SetADNModeEnable(false)");
    returnCode = DjiFlightController_SetADNModeEnable(false);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("SetADNModeDisable(false) failed, error code: 0x%08X", returnCode);
        goto out;
    }
    USER_LOG_INFO("ADN mode Disabled");
    s_osalHandler->TaskSleepMs(1000);

    /*! -----------------------------------------------------------------------
     * Step 6: Get multiple mode and to assist position mode to verify
     * ----------------------------------------------------------------------- */
    USER_LOG_INFO("--> Step 6.1: Get multiple mode to verify");
    returnCode = DjiFlightController_GetMultipleMode(&multipleMode);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Get multiple mode failed, error code: 0x%08X", returnCode);
        goto out;
    }
    USER_LOG_INFO("Multiple mode readback = %d (expect DISABLE) --> %s",
                  multipleMode,
                  multipleMode == DJI_FLIGHT_CONTROLLER_MULTIPLE_MODE_DISABLE ? "PASS" : "FAIL");
    s_osalHandler->TaskSleepMs(500);

    USER_LOG_INFO("--> Step 6.2: Get assist positioning mode to verify");
    returnCode = DjiFlightController_GetAssistPositioningMode(&assistMode);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Get assist positioning mode failed, error code: 0x%08X", returnCode);
        goto out;
    }
    USER_LOG_INFO("Assist positioning mode readback = %d (expect OFF) --> %s",
                  assistMode,
                  assistMode == DJI_FLIGHT_CONTROLLER_ASSIST_POSITIONING_MODE_OFF ? "PASS" : "FAIL");
    s_osalHandler->TaskSleepMs(500);

    USER_LOG_INFO("ADN mode disabled");

out:
    DjiWaypointV3_RegAssistPosModeInfoCallback(NULL);
    USER_LOG_INFO("Flight control no-GPS mode sample end");
    return returnCode;
}

T_DjiReturnCode DjiTest_FlightControlGetBatteryCapacityGoHomeCallback(T_DjiFlightControllerBatteryCapacityGohome eventData)
{
    USER_LOG_INFO("DjiTest_FlightControlGetBatteryCapacityGoHomeCallback, remain_fly_time = %d, gohome_capacity = %d",
                   eventData.remain_fly_time, eventData.gohome_capacity);
}
void DjiTest_FlightControlGetBatteryCapacityGoHome()
{
    T_DjiReturnCode returnCode;
    USER_LOG_INFO("Flight control GetBatteryCapacityGoHome sample start");
    returnCode = DjiFlightController_RegisterBatteryCapacityGohomeCallBack(DjiTest_FlightControlGetBatteryCapacityGoHomeCallback);
    USER_LOG_INFO("RegisterBatteryCapacityGohomeCallBack ret = %d", returnCode);

    s_osalHandler->TaskSleepMs(10000);

    USER_LOG_INFO("Flight control GetBatteryCapacityGoHome sample end");
    DjiFlightController_AntiRegisterBatteryCapacityGohomeCallBack();
    return;
}

T_DjiReturnCode DjiTest_TriggerFtsEventCallback(void)
{
    USER_LOG_INFO("Received FTS Trigger event, count = %d.", s_ftsTriggerCount);
    if (s_ftsTriggerCount == 0) {
        USER_LOG_WARN("Note: Simulate a trigger failure scenario and return an error value, this function will be invoked again.");
        s_ftsTriggerCount++;
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    } else {
        USER_LOG_WARN("Note: This is an empty implementation, and the FTS signal needs to be triggered by the PWM signal.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    }
}

void DjiTest_FlightControlPassiveTriggerFtsSample(void)
{
    T_DjiReturnCode returnCode;

    USER_LOG_INFO("Flight control passive trigger FTS sample start.");

    if (s_isFtsCallbackRegistered == false) {
        returnCode = DjiFlightController_RegTriggerFtsEventCallback(DjiTest_TriggerFtsEventCallback);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Register trigger FTS event callback failed.");
            return;
        } else {
            s_isFtsCallbackRegistered = true;
            USER_LOG_INFO("Register trigger FTS event callback successfully."
                          "Please wait for the aircraft to trigger the payload to execute FTS action.");
        }
    } else {
        USER_LOG_WARN("FTS trigger event callback has been registered, no need to register again.");
    }
}

static void DjiTest_FlightControlSlowRotateMotorSample(void)
{
    E_DjiFlightControllerElectronicSpeedControllerStatus escStatus;

    T_DjiReturnCode returnCode = 0;

    DjiTest_WidgetLogAppend("Start rotating.");
    USER_LOG_INFO("Start rotating.");
    returnCode = DjiFlightController_StartSlowRotateMotor();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Start slow rotate blade failed, error code is 0x%08X", returnCode);
        return ;
    }

    returnCode = DjiFlightController_GetElectronicSpeedControllerStatus(&escStatus);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Fail to get ESC status, error code is 0x%08X", returnCode);
    }

    DjiTest_WidgetLogAppend("ESC status: %s motor(s) rotate mode",
        escStatus == DJI_FLIGHT_CONTROLLER_NO_MOTOR_IN_SLOW_ROTATE_MODE ? "no" :
        escStatus == DJI_FLIGHT_CONTROLLER_SOME_MOTOR_IN_SLOW_ROTATE_MODE ? "some" :
        escStatus == DJI_FLIGHT_CONTROLLER_ALL_MOTOR_IN_SLOW_ROTATE_MODE ? "all" : "(error)");
        USER_LOG_INFO("ESC status: %s motor(s) rotate mode",
        escStatus == DJI_FLIGHT_CONTROLLER_NO_MOTOR_IN_SLOW_ROTATE_MODE ? "no" :
        escStatus == DJI_FLIGHT_CONTROLLER_SOME_MOTOR_IN_SLOW_ROTATE_MODE ? "some" :
        escStatus == DJI_FLIGHT_CONTROLLER_ALL_MOTOR_IN_SLOW_ROTATE_MODE ? "all" : "(error)");

    for (int32_t i = 0; i < 8; i++) {
        s_osalHandler->TaskSleepMs(1000);

        returnCode = DjiFlightController_GetElectronicSpeedControllerStatus(&escStatus);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_ERROR("Fail to get ESC status, error code is 0x%08X", returnCode);
        }

        DjiTest_WidgetLogAppend("ESC status: %s motor(s) rotate mode",
            escStatus == DJI_FLIGHT_CONTROLLER_NO_MOTOR_IN_SLOW_ROTATE_MODE ? "no" :
            escStatus == DJI_FLIGHT_CONTROLLER_SOME_MOTOR_IN_SLOW_ROTATE_MODE ? "some" :
            escStatus == DJI_FLIGHT_CONTROLLER_ALL_MOTOR_IN_SLOW_ROTATE_MODE ? "all" : "(error)");
            USER_LOG_INFO("ESC status: %s motor(s) rotate mode",
            escStatus == DJI_FLIGHT_CONTROLLER_NO_MOTOR_IN_SLOW_ROTATE_MODE ? "no" :
            escStatus == DJI_FLIGHT_CONTROLLER_SOME_MOTOR_IN_SLOW_ROTATE_MODE ? "some" :
            escStatus == DJI_FLIGHT_CONTROLLER_ALL_MOTOR_IN_SLOW_ROTATE_MODE ? "all" : "(error)");
    }

    DjiTest_WidgetLogAppend("Stop rotating.");
    USER_LOG_INFO("Stop rotating.");
    returnCode = DjiFlightController_StopSlowRotateMotor();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_INFO("Stop slow rotate blade failed, error code is 0x%08X", returnCode);
        return ;
    }

    s_osalHandler->TaskSleepMs(1000);

    returnCode = DjiFlightController_GetElectronicSpeedControllerStatus(&escStatus);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Fail to get ESC status, error code is 0x%08X", returnCode);
    }

    DjiTest_WidgetLogAppend("ESC status: %s motor(s) rotate mode",
        escStatus == DJI_FLIGHT_CONTROLLER_NO_MOTOR_IN_SLOW_ROTATE_MODE ? "no" :
        escStatus == DJI_FLIGHT_CONTROLLER_SOME_MOTOR_IN_SLOW_ROTATE_MODE ? "some" :
        escStatus == DJI_FLIGHT_CONTROLLER_ALL_MOTOR_IN_SLOW_ROTATE_MODE ? "all" : "(error)");
        USER_LOG_INFO("ESC status: %s motor(s) rotate mode",
        escStatus == DJI_FLIGHT_CONTROLLER_NO_MOTOR_IN_SLOW_ROTATE_MODE ? "no" :
        escStatus == DJI_FLIGHT_CONTROLLER_SOME_MOTOR_IN_SLOW_ROTATE_MODE ? "some" :
        escStatus == DJI_FLIGHT_CONTROLLER_ALL_MOTOR_IN_SLOW_ROTATE_MODE ? "all" : "(error)");
}

void DjiTest_FlightControlSample(E_DjiTestFlightCtrlSampleSelect flightCtrlSampleSelect)
{
    switch (flightCtrlSampleSelect) {
        case E_DJI_TEST_FLIGHT_CTRL_SAMPLE_SELECT_TAKE_OFF_LANDING: {
            DjiTest_FlightControlTakeOffLandingSample();
            break;
        }
        case E_DJI_TEST_FLIGHT_CTRL_SAMPLE_SELECT_TAKE_OFF_POSITION_CTRL_LANDING: {
            DjiTest_FlightControlPositionControlSample();
            break;
        }
        case E_DJI_TEST_FLIGHT_CTRL_SAMPLE_SELECT_TAKE_OFF_GO_HOME_FORCE_LANDING: {
            DjiTest_FlightControlGoHomeForceLandingSample();
            break;
        }
        case E_DJI_TEST_FLIGHT_CTRL_SAMPLE_SELECT_TAKE_OFF_VELOCITY_CTRL_LANDING: {
            DjiTest_FlightControlVelocityControlSample();
            break;
        }
        case E_DJI_TEST_FLIGHT_CTRL_SAMPLE_SELECT_ARREST_FLYING: {
            DjiTest_FlightControlArrestFlyingSample();
            break;
        }
        case E_DJI_TEST_FLIGHT_CTRL_SAMPLE_SELECT_SET_GET_PARAM: {
            DjiTest_FlightControlSetGetParamSample();
            break;
        }
        case E_DJI_TEST_FLIGHT_CTRL_SAMPLE_SELECT_FTS_TRIGGER: {
            DjiTest_FlightControlPassiveTriggerFtsSample();
            break;
        }
        case E_DJI_TEST_FLIGHT_CTRL_SAMPLE_SELECT_SLOW_ROTATE_BLADE: {
            DjiTest_FlightControlSlowRotateMotorSample();
            break;
        }
        case E_DJI_TEST_FLIGHT_CTRL_SAMPLE_SELECT_SET_GET_PERCEPTION_PARAM: {
            DjiTest_FlightControlSetGetPerceptionParamSample();
            break;
        }
        case E_DJI_TEST_FLIGHT_CTRL_SAMPLE_SELECT_SET_CMD_START_MISSION: {
            DjiTest_FlightControlSetModeStartMission();
            break;
        }
        case E_DJI_TEST_FLIGHT_CTRL_SAMPLE_SELECT_GET_BATTERY_CAPACITY_GOHOME: {
            DjiTest_FlightControlGetBatteryCapacityGoHome();
            break;
        }
        case E_DJI_TEST_FLIGHT_CTRL_SAMPLE_SELECT_REGISTER_OSD_INFO: {
            DjiTest_FlightControlRegisterOsdInfoSample();
            break;
        }
        case E_DJI_TEST_FLIGHT_CTRL_SAMPLE_SELECT_STEALTH_MODE: {
            DjiTest_FlightControlStealthModeSample();
            break;
        }
        case E_DJI_TEST_FLIGHT_CTRL_SAMPLE_SELECT_NO_GNSS_MODE: {
            DjiTest_FlightControlNoGnssHeadingGoHomeSample();
            break;
        }
        case E_DJI_TEST_FLIGHT_CTRL_SAMPLE_SELECT_NO_GPS_MODE: {
            DjiTest_FlightControlNoGpsModeSample();
            break;
        }
        case E_DJI_TEST_FLIGHT_CTRL_SAMPLE_SELECT_EXTREME_BATTERY_ENDURANCE_MODE: {
            DjiTest_FlightControlExtremeBatteryEnduranceModeSample();
            break;
        }
        default:
            break;
    }
}

uint8_t DjiTest_FlightControlGetDisplayModeIndex(E_DjiFcSubscriptionDisplayMode displayMode)
{
    uint8_t i;

    for (i = 0; i < sizeof(s_flightControlDisplayModeStr) / sizeof(T_DjiTestFlightControlDisplayModeStr); i++) {
        if (s_flightControlDisplayModeStr[i].displayMode == displayMode) {
            return i;
        }
    }

    return i;
}

T_DjiFcSubscriptionFlightStatus DjiTest_FlightControlGetValueOfFlightStatus(void)
{
    T_DjiReturnCode djiStat;
    T_DjiFcSubscriptionFlightStatus flightStatus;
    T_DjiDataTimestamp flightStatusTimestamp = {0};

    djiStat = DjiFcSubscription_GetLatestValueOfTopic(DJI_FC_SUBSCRIPTION_TOPIC_STATUS_FLIGHT,
                                                      (uint8_t *) &flightStatus,
                                                      sizeof(T_DjiFcSubscriptionFlightStatus),
                                                      &flightStatusTimestamp);

    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Get value of topic flight status error, error code: 0x%08X", djiStat);
        flightStatus = 0;
    } else {
        USER_LOG_DEBUG("Timestamp: millisecond %u microsecond %u.", flightStatusTimestamp.millisecond,
                       flightStatusTimestamp.microsecond);
        USER_LOG_DEBUG("Flight status: %d.", flightStatus);
    }

    return flightStatus;
}

T_DjiFcSubscriptionDisplaymode DjiTest_FlightControlGetValueOfDisplayMode(void)
{
    T_DjiReturnCode djiStat;
    T_DjiFcSubscriptionDisplaymode displayMode;
    T_DjiDataTimestamp displayModeTimestamp = {0};

    djiStat = DjiFcSubscription_GetLatestValueOfTopic(DJI_FC_SUBSCRIPTION_TOPIC_STATUS_DISPLAYMODE,
                                                      (uint8_t *) &displayMode,
                                                      sizeof(T_DjiFcSubscriptionDisplaymode),
                                                      &displayModeTimestamp);

    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Get value of topic display mode error, error code: 0x%08X", djiStat);
        displayMode = 0;
    } else {
        USER_LOG_DEBUG("Timestamp: millisecond %u microsecond %u.", displayModeTimestamp.millisecond,
                       displayModeTimestamp.microsecond);
        USER_LOG_DEBUG("Display mode : %d.", displayMode);
    }

    return displayMode;
}

T_DjiFcSubscriptionHeightFusion DjiTest_FlightControlGetValueOfHeightFusion(void)
{
    T_DjiReturnCode djiStat;
    T_DjiFcSubscriptionHeightFusion heightFusion = {0};
    T_DjiDataTimestamp timestamp = {0};

    djiStat = DjiFcSubscription_GetLatestValueOfTopic(DJI_FC_SUBSCRIPTION_TOPIC_HEIGHT_FUSION,
                                                      (uint8_t *) &heightFusion,
                                                      sizeof(T_DjiFcSubscriptionHeightFusion),
                                                      &timestamp);

    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Get value of topic height fusion error, error code: 0x%08X", djiStat);
    } else {
        USER_LOG_DEBUG("Timestamp: millisecond %u microsecond %u.", timestamp.millisecond, timestamp.microsecond);
        USER_LOG_DEBUG("Relative height fusion is %f m", heightFusion);
    }

    return heightFusion;
}

T_DjiFcSubscriptionQuaternion DjiTest_FlightControlGetValueOfQuaternion(void)
{
    T_DjiReturnCode djiStat;
    T_DjiFcSubscriptionQuaternion quaternion = {0};
    T_DjiDataTimestamp quaternionTimestamp = {0};

    djiStat = DjiFcSubscription_GetLatestValueOfTopic(DJI_FC_SUBSCRIPTION_TOPIC_QUATERNION,
                                                      (uint8_t *) &quaternion,
                                                      sizeof(T_DjiFcSubscriptionQuaternion),
                                                      &quaternionTimestamp);

    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Get value of topic quaternion error, error code: 0x%08X", djiStat);
    } else {
        USER_LOG_DEBUG("Timestamp: millisecond %u microsecond %u.", quaternionTimestamp.millisecond,
                       quaternionTimestamp.microsecond);
        USER_LOG_DEBUG("Quaternion: %f %f %f %f.", quaternion.q0, quaternion.q1, quaternion.q2, quaternion.q3);
    }

    return quaternion;
}

T_DjiFcSubscriptionPositionFused DjiTest_FlightControlGetValueOfPositionFused(void)
{
    T_DjiReturnCode djiStat;
    T_DjiFcSubscriptionPositionFused positionFused = {0};
    T_DjiDataTimestamp positionFusedTimestamp = {0};

    djiStat = DjiFcSubscription_GetLatestValueOfTopic(DJI_FC_SUBSCRIPTION_TOPIC_POSITION_FUSED,
                                                      (uint8_t *) &positionFused,
                                                      sizeof(T_DjiFcSubscriptionPositionFused),
                                                      &positionFusedTimestamp);

    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Get value of topic position fused error, error code: 0x%08X", djiStat);
    } else {
        USER_LOG_DEBUG("Timestamp: millisecond %u microsecond %u.", positionFusedTimestamp.millisecond,
                       positionFusedTimestamp.microsecond);
        USER_LOG_DEBUG("PositionFused: %f, %f,%f,%d.", positionFused.latitude, positionFused.longitude,
                       positionFused.altitude, positionFused.visibleSatelliteNumber);
    }

    return positionFused;
}

dji_f32_t DjiTest_FlightControlGetValueOfRelativeHeight(void)
{
    T_DjiReturnCode djiStat;
    T_DjiFcSubscriptionAltitudeFused altitudeFused = 0;
    T_DjiFcSubscriptionAltitudeOfHomePoint homePointAltitude = 0;
    dji_f32_t relativeHeight = 0;
    T_DjiDataTimestamp relativeHeightTimestamp = {0};

    djiStat = DjiFcSubscription_GetLatestValueOfTopic(DJI_FC_SUBSCRIPTION_TOPIC_ALTITUDE_OF_HOMEPOINT,
                                                      (uint8_t *) &homePointAltitude,
                                                      sizeof(T_DjiFcSubscriptionAltitudeOfHomePoint),
                                                      &relativeHeightTimestamp);

    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Get value of topic altitude of home point error, error code: 0x%08X", djiStat);
        return -1;
    } else {
        USER_LOG_DEBUG("Timestamp: millisecond %u microsecond %u.", relativeHeightTimestamp.millisecond,
                       relativeHeightTimestamp.microsecond);
    }

    djiStat = DjiFcSubscription_GetLatestValueOfTopic(DJI_FC_SUBSCRIPTION_TOPIC_ALTITUDE_FUSED,
                                                      (uint8_t *) &altitudeFused,
                                                      sizeof(T_DjiFcSubscriptionAltitudeFused),
                                                      &relativeHeightTimestamp);

    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Get value of topic altitude fused error, error code: 0x%08X", djiStat);
        return -1;
    } else {
        USER_LOG_DEBUG("Timestamp: millisecond %u microsecond %u.", relativeHeightTimestamp.millisecond,
                       relativeHeightTimestamp.microsecond);
    }

    relativeHeight = altitudeFused - homePointAltitude;

    return relativeHeight;
}

bool DjiTest_FlightControlMotorStartedCheck(void)
{
    int motorsNotStarted = 0;
    int timeoutCycles = 20;

    while (DjiTest_FlightControlGetValueOfFlightStatus() != DJI_FC_SUBSCRIPTION_FLIGHT_STATUS_ON_GROUND &&
           DjiTest_FlightControlGetValueOfDisplayMode() != DJI_FC_SUBSCRIPTION_DISPLAY_MODE_ENGINE_START &&
           motorsNotStarted < timeoutCycles) {
        motorsNotStarted++;
        s_osalHandler->TaskSleepMs(100);
    }
    return motorsNotStarted != timeoutCycles ? true : false;
}

bool DjiTest_FlightControlTakeOffInAirCheck(void)
{
    int stillOnGround = 0;
    int timeoutCycles = 110;

    while (DjiTest_FlightControlGetValueOfFlightStatus() != DJI_FC_SUBSCRIPTION_FLIGHT_STATUS_IN_AIR &&
           (DjiTest_FlightControlGetValueOfDisplayMode() != DJI_FC_SUBSCRIPTION_DISPLAY_MODE_ASSISTED_TAKEOFF ||
            DjiTest_FlightControlGetValueOfDisplayMode() != DJI_FC_SUBSCRIPTION_DISPLAY_MODE_AUTO_TAKEOFF) &&
           stillOnGround < timeoutCycles) {
        stillOnGround++;
        s_osalHandler->TaskSleepMs(100);
    }

    return stillOnGround != timeoutCycles ? true : false;
}

bool takeoffFinishedCheck(void)
{
    while (DjiTest_FlightControlGetValueOfDisplayMode() == DJI_FC_SUBSCRIPTION_DISPLAY_MODE_AUTO_TAKEOFF ||
           DjiTest_FlightControlGetValueOfDisplayMode() == DJI_FC_SUBSCRIPTION_DISPLAY_MODE_ASSISTED_TAKEOFF) {
        s_osalHandler->TaskSleepMs(1000);
    }

    return (DjiTest_FlightControlGetValueOfDisplayMode() == DJI_FC_SUBSCRIPTION_DISPLAY_MODE_P_GPS ||
            DjiTest_FlightControlGetValueOfDisplayMode() == DJI_FC_SUBSCRIPTION_DISPLAY_MODE_ATTITUDE) ? true : false;
}

bool DjiTest_FlightControlLandFinishedCheck(void)
{
    while (DjiTest_FlightControlGetValueOfDisplayMode() == DJI_FC_SUBSCRIPTION_DISPLAY_MODE_AUTO_LANDING ||
           DjiTest_FlightControlGetValueOfFlightStatus() == DJI_FC_SUBSCRIPTION_FLIGHT_STATUS_IN_AIR) {
            if(DjiTest_FlightControlGetValueOfHeightFusion() <= 0.7) {
                if(DjiFlightController_StartConfirmLanding()!= DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
                    USER_LOG_ERROR("Start confirm landing failed");
                }
                s_osalHandler->TaskSleepMs(5000);
                continue;
            }
        s_osalHandler->TaskSleepMs(1000);
    }

    return (DjiTest_FlightControlGetValueOfDisplayMode() != DJI_FC_SUBSCRIPTION_DISPLAY_MODE_P_GPS ||
            DjiTest_FlightControlGetValueOfDisplayMode() != DJI_FC_SUBSCRIPTION_DISPLAY_MODE_ATTITUDE) ? true : false;
}

bool DjiTest_FlightControlCheckActionStarted(E_DjiFcSubscriptionDisplayMode mode)
{
    int actionNotStarted = 0;
    int timeoutCycles = 20;

    while (DjiTest_FlightControlGetValueOfDisplayMode() != mode && actionNotStarted < timeoutCycles) {
        actionNotStarted++;
        s_osalHandler->TaskSleepMs(100);
    }

    if (actionNotStarted == timeoutCycles) {
        USER_LOG_ERROR("%s start failed, now flight is in %s.",
                       s_flightControlDisplayModeStr[DjiTest_FlightControlGetDisplayModeIndex(mode)].displayModeStr,
                       s_flightControlDisplayModeStr[DjiTest_FlightControlGetDisplayModeIndex(
                           DjiTest_FlightControlGetValueOfDisplayMode())].displayModeStr);
        return false;
    } else {
        USER_LOG_INFO("Now flight is in %s.",
                      s_flightControlDisplayModeStr[DjiTest_FlightControlGetDisplayModeIndex(mode)].displayModeStr);
        return true;
    }
}

bool DjiTest_FlightControlMonitoredTakeoff(void)
{
    T_DjiReturnCode djiStat;

    //! Start takeoff
    djiStat = DjiFlightController_StartTakeoff();
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Request to take off failed, error code: 0x%08X", djiStat);
        return false;
    }

    //! Motors start check
    if (!DjiTest_FlightControlMotorStartedCheck()) {
        USER_LOG_ERROR("Takeoff failed. Motors are not spinning.");
        return false;
    } else {
        USER_LOG_INFO("Motors spinning...");
    }
    //! In air check
    if (!DjiTest_FlightControlTakeOffInAirCheck()) {
        USER_LOG_ERROR("Takeoff failed. Aircraft is still on the ground, but the "
                       "motors are spinning");
        return false;
    } else {
        USER_LOG_INFO("Ascending...");
    }
    //! Finished takeoff check
    if (!takeoffFinishedCheck()) {
        USER_LOG_ERROR("Takeoff finished, but the aircraft is in an unexpected mode. "
                       "Please connect DJI GO.");
        return false;
    }

    return true;
}

bool DjiTest_FlightControlMonitoredLanding(void)
{
    T_DjiReturnCode djiStat;
    /*! Step 1: Start landing */
    djiStat = DjiFlightController_StartLanding();
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Start landing failed, error code: 0x%08X", djiStat);
        return false;
    }

    /*! Step 2: check Landing start*/
    if (!DjiTest_FlightControlCheckActionStarted(DJI_FC_SUBSCRIPTION_DISPLAY_MODE_AUTO_LANDING)) {
        USER_LOG_ERROR("Fail to execute Landing action!");
        return false;
    } else {
        /*! Step 3: check Landing finished*/
        if (!DjiTest_FlightControlLandFinishedCheck()) {
            USER_LOG_ERROR("Landing finished, but the aircraft is in an unexpected mode. "
                           "Please connect DJI Assistant.");
            return false;
        }
    }

    return true;
}

bool DjiTest_FlightControlGoHomeAndConfirmLanding(void)
{
    T_DjiReturnCode djiStat;
    T_DjiAircraftInfoBaseInfo aircraftInfoBaseInfo;
    E_DjiFlightControllerObstacleAvoidanceEnableStatus enableStatus;

    djiStat = DjiFlightController_GetDownwardsVisualObstacleAvoidanceEnableStatus(&enableStatus);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("get downwards visual obstacle avoidance enable status error");
    }

    djiStat = DjiAircraftInfo_GetBaseInfo(&aircraftInfoBaseInfo);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("get aircraft base info error");
    }

    /*! Step 1: Start go home */
    USER_LOG_INFO("Start go home action");
    djiStat = DjiFlightController_StartGoHome();
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Start to go home failed, error code: 0x%08X", djiStat);
        return false;;
    }

    if (!DjiTest_FlightControlCheckActionStarted(DJI_FC_SUBSCRIPTION_DISPLAY_MODE_NAVI_GO_HOME)) {
        return false;
    } else {
        while (DjiTest_FlightControlGetValueOfFlightStatus() == DJI_FC_SUBSCRIPTION_FLIGHT_STATUS_IN_AIR &&
               DjiTest_FlightControlGetValueOfDisplayMode() == DJI_FC_SUBSCRIPTION_DISPLAY_MODE_NAVI_GO_HOME) {
            s_osalHandler->TaskSleepMs(1000);// waiting for this action finished
        }
    }

    /*! Step 2: Start landing */
    USER_LOG_INFO("Start landing action");
    if (!DjiTest_FlightControlCheckActionStarted(DJI_FC_SUBSCRIPTION_DISPLAY_MODE_AUTO_LANDING)) {
        USER_LOG_ERROR("Fail to execute Landing action");
        return false;
    } else {
        while (DjiTest_FlightControlGetValueOfDisplayMode() == DJI_FC_SUBSCRIPTION_DISPLAY_MODE_AUTO_LANDING &&
               DjiTest_FlightControlGetValueOfFlightStatus() == DJI_FC_SUBSCRIPTION_FLIGHT_STATUS_IN_AIR) {
            T_DjiFcSubscriptionHeightFusion heightFusion = DjiTest_FlightControlGetValueOfHeightFusion();
            s_osalHandler->TaskSleepMs(1000);
            if (DJI_AIRCRAFT_TYPE_M3E == aircraftInfoBaseInfo.aircraftType || DJI_AIRCRAFT_TYPE_M3T == aircraftInfoBaseInfo.aircraftType
                || DJI_AIRCRAFT_TYPE_M3D == aircraftInfoBaseInfo.aircraftType || DJI_AIRCRAFT_TYPE_M3TD == aircraftInfoBaseInfo.aircraftType
                || DJI_AIRCRAFT_TYPE_M3TA == aircraftInfoBaseInfo.aircraftType
                || DJI_AIRCRAFT_TYPE_M4T == aircraftInfoBaseInfo.aircraftType
                || DJI_AIRCRAFT_TYPE_M4TD == aircraftInfoBaseInfo.aircraftType
                || DJI_AIRCRAFT_TYPE_M4D == aircraftInfoBaseInfo.aircraftType
                || DJI_AIRCRAFT_TYPE_M4E == aircraftInfoBaseInfo.aircraftType
                || DJI_AIRCRAFT_TYPE_M4TD == aircraftInfoBaseInfo.aircraftType
                || DJI_AIRCRAFT_TYPE_M4D == aircraftInfoBaseInfo.aircraftType
            ) {
                if ((dji_f64_t) 0.45 < heightFusion && heightFusion < (dji_f64_t) 0.55) {
                    break;
                }
            } else {
                if ((dji_f64_t) 0.65 < heightFusion && heightFusion < (dji_f64_t) 0.75) {
                    break;
                }
            }
        }
    }

    /*! Step 4: Confirm Landing */
    USER_LOG_INFO("Start confirm Landing and avoid ground action");
    djiStat = DjiFlightController_StartConfirmLanding();
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Fail to execute confirm landing avoid ground action, error code: 0x%08X", djiStat);
        return false;
    }

    if (enableStatus == DJI_FLIGHT_CONTROLLER_ENABLE_OBSTACLE_AVOIDANCE) {
        if (!DjiTest_FlightControlCheckActionStarted(DJI_FC_SUBSCRIPTION_DISPLAY_MODE_FORCE_AUTO_LANDING)) {
            return false;
        } else {
            while (DjiTest_FlightControlGetValueOfFlightStatus() == DJI_FC_SUBSCRIPTION_FLIGHT_STATUS_IN_AIR &&
                   DjiTest_FlightControlGetValueOfDisplayMode() ==
                   DJI_FC_SUBSCRIPTION_DISPLAY_MODE_FORCE_AUTO_LANDING) {
                s_osalHandler->TaskSleepMs(1000);
            }
        }
    } else {
        while (DjiTest_FlightControlGetValueOfFlightStatus() == DJI_FC_SUBSCRIPTION_FLIGHT_STATUS_IN_AIR &&
               DjiTest_FlightControlGetValueOfDisplayMode() ==
               DJI_FC_SUBSCRIPTION_DISPLAY_MODE_FORCE_AUTO_LANDING) {
            s_osalHandler->TaskSleepMs(1000);
        }
    }

    /*! Step 5: Landing finished check*/
    if (DjiTest_FlightControlGetValueOfDisplayMode() != DJI_FC_SUBSCRIPTION_DISPLAY_MODE_P_GPS ||
        DjiTest_FlightControlGetValueOfDisplayMode() != DJI_FC_SUBSCRIPTION_DISPLAY_MODE_ATTITUDE) {
        USER_LOG_INFO("Successful landing");
    } else {
        USER_LOG_ERROR("Landing finished, but the aircraft is in an unexpected mode. "
                       "Please connect DJI Assistant.");
        return false;
    }

    return true;
}

T_DjiTestFlightControlVector3f DjiTest_FlightControlQuaternionToEulerAngle(const T_DjiFcSubscriptionQuaternion quat)
{
    T_DjiTestFlightControlVector3f eulerAngle;
    double q2sqr = quat.q2 * quat.q2;
    double t0 = -2.0 * (q2sqr + quat.q3 * quat.q3) + 1.0;
    double t1 = (dji_f64_t) 2.0 * (quat.q1 * quat.q2 + quat.q0 * quat.q3);
    double t2 = -2.0 * (quat.q1 * quat.q3 - quat.q0 * quat.q2);
    double t3 = (dji_f64_t) 2.0 * (quat.q2 * quat.q3 + quat.q0 * quat.q1);
    double t4 = -2.0 * (quat.q1 * quat.q1 + q2sqr) + 1.0;
    t2 = (t2 > 1.0) ? 1.0 : t2;
    t2 = (t2 < -1.0) ? -1.0 : t2;
    eulerAngle.x = asin(t2);
    eulerAngle.y = atan2(t3, t4);
    eulerAngle.z = atan2(t1, t0);
    return eulerAngle;
}

T_DjiTestFlightControlVector3f
DjiTest_FlightControlLocalOffsetFromGpsAndFusedHeightOffset(const T_DjiFcSubscriptionPositionFused target,
                                                            const T_DjiFcSubscriptionPositionFused origin,
                                                            const dji_f32_t targetHeight,
                                                            const dji_f32_t originHeight)
{
    T_DjiTestFlightControlVector3f deltaNed;
    double deltaLon = target.longitude - origin.longitude;
    double deltaLat = target.latitude - origin.latitude;
    deltaNed.x = deltaLat * s_earthCenter;
    deltaNed.y = deltaLon * s_earthCenter * cos(target.latitude);
    deltaNed.z = targetHeight - originHeight;

    return deltaNed;
}

T_DjiTestFlightControlVector3f DjiTest_FlightControlVector3FSub(const T_DjiTestFlightControlVector3f vectorA,
                                                                const T_DjiTestFlightControlVector3f vectorB)
{
    T_DjiTestFlightControlVector3f result;
    result.x = vectorA.x - vectorB.x;
    result.y = vectorA.y - vectorB.y;
    result.z = vectorA.z - vectorB.z;
    return result;
}

int DjiTest_FlightControlSignOfData(dji_f32_t data)
{
    return data < 0 ? -1 : 1;
}

void DjiTest_FlightControlHorizCommandLimit(dji_f32_t speedFactor, dji_f32_t *commandX, dji_f32_t *commandY)
{
    if (fabs(*commandX) > speedFactor)
        *commandX = speedFactor * DjiTest_FlightControlSignOfData(*commandX);
    if (fabs(*commandY) > speedFactor)
        *commandY = speedFactor * DjiTest_FlightControlSignOfData(*commandY);
}

dji_f32_t DjiTest_FlightControlVectorNorm(T_DjiTestFlightControlVector3f v)
{
    return sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
}

bool
DjiTest_FlightControlMoveByPositionOffset(const T_DjiTestFlightControlVector3f offsetDesired, float yawDesiredInDeg,
                                          float posThresholdInM, float yawThresholdInDeg)
{
    int timeoutInMilSec = 20000;
    int controlFreqInHz = 50;  // Hz
    int cycleTimeInMs = 1000 / controlFreqInHz;
    int outOfControlBoundsTimeLimit = 10 * cycleTimeInMs;    // 10 cycles
    int withinControlBoundsTimeReqmt = 100 * cycleTimeInMs;  // 100 cycles
    int elapsedTimeInMs = 0;
    int withinBoundsCounter = 0;
    int outOfBounds = 0;
    int brakeCounter = 0;
    int speedFactor = 2;

    /* 50 Hz verification: measure actual cycle time and report every 50 cycles */
    uint32_t freqCheckLoopCount = 0;
    uint32_t freqCheckCycleMinMs = UINT32_MAX;
    uint32_t freqCheckCycleMaxMs = 0;
    uint32_t freqCheckLastTimeMs = 0;
    s_osalHandler->GetTimeMs(&freqCheckLastTimeMs);

    //! get origin position and relative height(from home point)of aircraft.
    T_DjiFcSubscriptionPositionFused originGPSPosition = DjiTest_FlightControlGetValueOfPositionFused();
    dji_f32_t originHeightBaseHomePoint = DjiTest_FlightControlGetValueOfRelativeHeight();
    if (originHeightBaseHomePoint == -1) {
        USER_LOG_ERROR("Relative height is invalid!");
        return false;
    }

    T_DjiFlightControllerJoystickMode joystickMode = {
        DJI_FLIGHT_CONTROLLER_HORIZONTAL_POSITION_CONTROL_MODE,
        DJI_FLIGHT_CONTROLLER_VERTICAL_POSITION_CONTROL_MODE,
        DJI_FLIGHT_CONTROLLER_YAW_ANGLE_CONTROL_MODE,
        DJI_FLIGHT_CONTROLLER_HORIZONTAL_GROUND_COORDINATE,
        DJI_FLIGHT_CONTROLLER_STABLE_CONTROL_MODE_ENABLE,
    };
    DjiFlightController_SetJoystickMode(joystickMode);

    while (elapsedTimeInMs < timeoutInMilSec) {
        /* --- 50 Hz verification: measure actual interval from last cycle --- */
        uint32_t freqCheckNowMs = 0;
        s_osalHandler->GetTimeMs(&freqCheckNowMs);
        if (freqCheckLoopCount > 0) {
            uint32_t actualCycleMs = freqCheckNowMs - freqCheckLastTimeMs;
            if (actualCycleMs < freqCheckCycleMinMs) freqCheckCycleMinMs = actualCycleMs;
            if (actualCycleMs > freqCheckCycleMaxMs) freqCheckCycleMaxMs = actualCycleMs;
        }
        freqCheckLastTimeMs = freqCheckNowMs;
        freqCheckLoopCount++;
        /* --- end 50 Hz verification --- */

        T_DjiFcSubscriptionPositionFused currentGPSPosition = DjiTest_FlightControlGetValueOfPositionFused();
        T_DjiFcSubscriptionQuaternion currentQuaternion = DjiTest_FlightControlGetValueOfQuaternion();
        dji_f32_t currentHeight = DjiTest_FlightControlGetValueOfRelativeHeight();
        if (originHeightBaseHomePoint == -1) {
            USER_LOG_ERROR("Relative height is invalid!");
            return false;
        }

        float yawInRad = DjiTest_FlightControlQuaternionToEulerAngle(currentQuaternion).z;
        //! get the vector between aircraft and origin point.

        T_DjiTestFlightControlVector3f localOffset = DjiTest_FlightControlLocalOffsetFromGpsAndFusedHeightOffset(
            currentGPSPosition,
            originGPSPosition,
            currentHeight,
            originHeightBaseHomePoint);
        //! get the vector between aircraft and target point.
        T_DjiTestFlightControlVector3f offsetRemaining = DjiTest_FlightControlVector3FSub(offsetDesired, localOffset);

        T_DjiTestFlightControlVector3f positionCommand = offsetRemaining;
        DjiTest_FlightControlHorizCommandLimit(speedFactor, &positionCommand.x, &positionCommand.y);

        T_DjiFlightControllerJoystickCommand joystickCommand = {positionCommand.x, positionCommand.y,
                                                                offsetDesired.z + originHeightBaseHomePoint,
                                                                yawDesiredInDeg};
        DjiFlightController_ExecuteJoystickAction(joystickCommand);

        if (DjiTest_FlightControlVectorNorm(offsetRemaining) < posThresholdInM &&
            fabs(yawInRad / s_degToRad - yawDesiredInDeg) < yawThresholdInDeg) {
            //! 1. We are within bounds; start incrementing our in-bound counter
            withinBoundsCounter += cycleTimeInMs;
        } else {
            if (withinBoundsCounter != 0) {
                //! 2. Start incrementing an out-of-bounds counter
                outOfBounds += cycleTimeInMs;
            }
        }
        //! 3. Reset withinBoundsCounter if necessary
        if (outOfBounds > outOfControlBoundsTimeLimit) {
            withinBoundsCounter = 0;
            outOfBounds = 0;
        }
        //! 4. If within bounds, set flag and break
        if (withinBoundsCounter >= withinControlBoundsTimeReqmt) {
            break;
        }
        s_osalHandler->TaskSleepMs(cycleTimeInMs);
        elapsedTimeInMs += cycleTimeInMs;
    }

    /* Print one-shot position feedback frequency summary after control loop exits */
    if (elapsedTimeInMs > 0) {
        float avgHzActual = freqCheckLoopCount * 1000.0f / (float)elapsedTimeInMs;
        USER_LOG_INFO("Position feedback actual frequency: %.1f Hz (%u cycles / %d ms)",
                      avgHzActual, freqCheckLoopCount, elapsedTimeInMs);
    }

    while (brakeCounter < withinControlBoundsTimeReqmt) {
        s_osalHandler->TaskSleepMs(cycleTimeInMs);
        brakeCounter += cycleTimeInMs;
    }

    if (elapsedTimeInMs >= timeoutInMilSec) {
        USER_LOG_ERROR("Task timeout!");
        return false;
    }

    return true;
}

void DjiTest_FlightControlVelocityAndYawRateCtrl(const T_DjiTestFlightControlVector3f offsetDesired, float yawRate,
                                                 uint32_t timeMs)
{
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    uint32_t originTime = 0;
    uint32_t currentTime = 0;
    uint32_t elapsedTimeInMs = 0;
    osalHandler->GetTimeMs(&originTime);
    osalHandler->GetTimeMs(&currentTime);
    elapsedTimeInMs = currentTime - originTime;
    T_DjiFlightControllerJoystickMode joystickMode = {
        DJI_FLIGHT_CONTROLLER_HORIZONTAL_VELOCITY_CONTROL_MODE,
        DJI_FLIGHT_CONTROLLER_VERTICAL_VELOCITY_CONTROL_MODE,
        DJI_FLIGHT_CONTROLLER_YAW_ANGLE_RATE_CONTROL_MODE,
        DJI_FLIGHT_CONTROLLER_HORIZONTAL_GROUND_COORDINATE,
        DJI_FLIGHT_CONTROLLER_STABLE_CONTROL_MODE_ENABLE,
    };

    DjiFlightController_SetJoystickMode(joystickMode);
    T_DjiFlightControllerJoystickCommand joystickCommand = {offsetDesired.x, offsetDesired.y, offsetDesired.z,
                                                            yawRate};

    while (elapsedTimeInMs <= timeMs) {
        DjiFlightController_ExecuteJoystickAction(joystickCommand);
        osalHandler->TaskSleepMs(2);
        osalHandler->GetTimeMs(&currentTime);
        elapsedTimeInMs = currentTime - originTime;
    }
}

T_DjiReturnCode
DjiTest_FlightControlJoystickCtrlAuthSwitchEventCallback(T_DjiFlightControllerJoystickCtrlAuthorityEventInfo eventData)
{
    switch (eventData.joystickCtrlAuthoritySwitchEvent) {
        case DJI_FLIGHT_CONTROLLER_MSDK_GET_JOYSTICK_CTRL_AUTH_EVENT: {
            if (eventData.curJoystickCtrlAuthority == DJI_FLIGHT_CONTROLLER_JOYSTICK_CTRL_AUTHORITY_MSDK) {
                DjiTest_WidgetLogAppend("Msdk request ctrl authority!");
                USER_LOG_INFO("[Event]Msdk request to obtain joystick ctrl authority\r\n");
            } else {
                DjiTest_WidgetLogAppend("Msdk release ctrl authority!");
                USER_LOG_INFO("[Event]Msdk request to release joystick ctrl authority\r\n");
            }
            break;
        }
        case DJI_FLIGHT_CONTROLLER_INTERNAL_GET_JOYSTICK_CTRL_AUTH_EVENT: {
            if (eventData.curJoystickCtrlAuthority == DJI_FLIGHT_CONTROLLER_JOYSTICK_CTRL_AUTHORITY_INTERNAL) {
                DjiTest_WidgetLogAppend("Internal request ctrl authority!");
                USER_LOG_INFO("[Event]Internal request to obtain joystick ctrl authority\r\n");
            } else {
                DjiTest_WidgetLogAppend("Internal release ctrl authority!");
                USER_LOG_INFO("[Event]Internal request to release joystick ctrl authority\r\n");
            }
            break;
        }
        case DJI_FLIGHT_CONTROLLER_OSDK_GET_JOYSTICK_CTRL_AUTH_EVENT: {
            if (eventData.curJoystickCtrlAuthority == DJI_FLIGHT_CONTROLLER_JOYSTICK_CTRL_AUTHORITY_OSDK) {
                DjiTest_WidgetLogAppend("request ctrl authority!");
                USER_LOG_INFO("[Event] Request to obtain joystick ctrl authority\r\n");
            } else {
                DjiTest_WidgetLogAppend("release ctrl authority!");
                USER_LOG_INFO("[Event] Request to release joystick ctrl authority\r\n");
            }
            break;
        }
        case DJI_FLIGHT_CONTROLLER_RC_LOST_GET_JOYSTICK_CTRL_AUTH_EVENT :
            DjiTest_WidgetLogAppend("rc lost! reset ctrl authority");
            USER_LOG_INFO("[Event]Current joystick ctrl authority is reset to rc due to rc lost\r\n");
            break;
        case DJI_FLIGHT_CONTROLLER_RC_NOT_P_MODE_RESET_JOYSTICK_CTRL_AUTH_EVENT :
            DjiTest_WidgetLogAppend("rc P mode! rc get authority");
            USER_LOG_INFO("[Event]Current joystick ctrl authority is reset to rc for rc is not in P mode\r\n");
            break;
        case DJI_FLIGHT_CONTROLLER_RC_SWITCH_MODE_GET_JOYSTICK_CTRL_AUTH_EVENT :
            DjiTest_WidgetLogAppend("rc switching mode! rc get authority");
            USER_LOG_INFO("[Event]Current joystick ctrl authority is reset to rc due to rc switching mode\r\n");
            break;
        case DJI_FLIGHT_CONTROLLER_RC_PAUSE_GET_JOYSTICK_CTRL_AUTH_EVENT :
            DjiTest_WidgetLogAppend("rc pausing! rc get authority");
            USER_LOG_INFO("[Event]Current joystick ctrl authority is reset to rc due to rc pausing\r\n");
            break;
        case DJI_FLIGHT_CONTROLLER_RC_REQUEST_GO_HOME_GET_JOYSTICK_CTRL_AUTH_EVENT :
            DjiTest_WidgetLogAppend("rc request return! rc get authority");
            USER_LOG_INFO("[Event]Current joystick ctrl authority is reset to rc due to rc request for return\r\n");
            break;
        case DJI_FLIGHT_CONTROLLER_LOW_BATTERY_GO_HOME_RESET_JOYSTICK_CTRL_AUTH_EVENT :
            DjiTest_WidgetLogAppend("low battery return! rc get authority");
            USER_LOG_INFO("[Event]Current joystick ctrl authority is reset to rc for low battery return\r\n");
            break;
        case DJI_FLIGHT_CONTROLLER_LOW_BATTERY_LANDING_RESET_JOYSTICK_CTRL_AUTH_EVENT :
            DjiTest_WidgetLogAppend("low battery land! rc get authority");
            USER_LOG_INFO("[Event]Current joystick ctrl authority is reset to rc for low battery land\r\n");
            break;
        case DJI_FLIGHT_CONTROLLER_OSDK_LOST_GET_JOYSTICK_CTRL_AUTH_EVENT:
            DjiTest_WidgetLogAppend("sdk lost! rc get authority");
            USER_LOG_INFO("[Event]Current joystick ctrl authority is reset to rc due to sdk lost\r\n");
            break;
        case DJI_FLIGHT_CONTROLLER_NERA_FLIGHT_BOUNDARY_RESET_JOYSTICK_CTRL_AUTH_EVENT :
            DjiTest_WidgetLogAppend("near boundary! rc get authority");
            USER_LOG_INFO("[Event]Current joystick ctrl authority is reset to rc due to near boundary\r\n");
            break;
        case DJI_FLIGHT_CONTROLLER_DOCK_REQUEST_CHANGE_JOYSTICK_CTRL_AUTH_EVENT :
            DjiTest_WidgetLogAppend("dock request! dock get authority");
            USER_LOG_INFO("[Event]Current joystick ctrl authority is change due to the dock request\r\n");
            break;
        default:
            DjiTest_WidgetLogAppend("Unknown ctrl authority event");
            USER_LOG_INFO("[Event]Unknown joystick ctrl authority event\r\n");
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

static T_DjiReturnCode DjiTest_SetFtsTrigger(E_DjiMountPosition position, const char* desc)
{
    T_DjiReturnCode djiStat;
    T_DjiFtsPwmEscTriggerStatus esc_status;

    djiStat = DjiFts_SelectFtsPwmTrigger(position);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("select fts pwm trigger E-PORT error, error code: 0x%08X", djiStat);
        return djiStat;
    }

    djiStat = DjiFts_GetFtsPwmTriggerStatus(&esc_status);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("get pwm trigger status error, error code: 0x%08X", djiStat);
        return djiStat;
    }

    if (esc_status.ESC[0].fts_select != position || esc_status.ESC[1].fts_select != position)
    {
        USER_LOG_ERROR("pwm trigger status incorrect");
        return djiStat;
    }

    if (esc_status.ESC[0].fts_status == DJI_FLIGHT_CONTROLLER_FTS_NOT_TRIGGERD && esc_status.ESC[1].fts_status == DJI_FLIGHT_CONTROLLER_FTS_NOT_TRIGGERD)

    USER_LOG_INFO("Set fts trigger position %s success", desc);

    return djiStat;
}

T_DjiReturnCode DjiTest_FtsPwmTriggerSample(E_DjiMountPosition position, const char* port_name)
{
    T_DjiReturnCode returnCode;

    returnCode = DjiTest_SetFtsTrigger(position, port_name);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Test select %s fts pwm trigger failed", port_name);
        return returnCode;
    }

    return returnCode;
}

T_DjiReturnCode DjiTest_FlightControlExtremeBatteryEnduranceModeSample(void)
{
    T_DjiReturnCode returnCode;
    bool modeEnabled = false;
    bool step1Done = false; /* track whether extreme endurance mode was successfully enabled */

    USER_LOG_INFO("Flight control extreme battery endurance mode sample start");
    DjiTest_WidgetLogAppend("Extreme battery endurance mode sample start");
    USER_LOG_WARN("===============================================");
    USER_LOG_WARN("EXTREME BATTERY ENDURANCE MODE - SAFETY REMINDER");
    USER_LOG_WARN("===============================================");
    USER_LOG_WARN("Enabling this mode disables smart battery go-home and landing protection.");
    USER_LOG_WARN("The aircraft will NOT automatically return home or land when battery is low.");
    USER_LOG_WARN("Please monitor battery level manually and operate with caution.");
    USER_LOG_WARN("===============================================");

    /*! -----------------------------------------------------------------------
     * Step 1: Enable extreme battery endurance mode
     * ----------------------------------------------------------------------- */
    USER_LOG_INFO("--> Step 1: Enable extreme battery endurance mode");
    returnCode = DjiFlightController_SetExtremeBatteryEnduranceModeEnabled(true);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_WARN("Set extreme battery endurance mode failed, error code: 0x%08X"
                      " (may need to be in flight)", returnCode);
        goto out;
    }
    step1Done = true;
    s_osalHandler->TaskSleepMs(500);

    /*! -----------------------------------------------------------------------
     * Step 2: Read back mode state to verify
     * ----------------------------------------------------------------------- */
    USER_LOG_INFO("--> Step 2: Get extreme battery endurance mode to verify");
    returnCode = DjiFlightController_GetExtremeBatteryEnduranceModeEnabled(&modeEnabled);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Get extreme battery endurance mode failed, error code: 0x%08X", returnCode);
        goto out;
    }
    USER_LOG_INFO("Extreme battery endurance mode is %s --> %s",
                  modeEnabled ? "enabled" : "disabled",
                  modeEnabled ? "PASS" : "FAIL");
    s_osalHandler->TaskSleepMs(500);

    /*! -----------------------------------------------------------------------
     * Step 3: Disable extreme battery endurance mode (restore default)
     * ----------------------------------------------------------------------- */
    USER_LOG_INFO("--> Step 3: Disable extreme battery endurance mode (restore default)");
    returnCode = DjiFlightController_SetExtremeBatteryEnduranceModeEnabled(false);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_WARN("Restore extreme battery endurance mode failed, error code: 0x%08X"
                      " (may need to be in flight)", returnCode);
        goto out;
    }
    step1Done = false;
    s_osalHandler->TaskSleepMs(500);

    /*! -----------------------------------------------------------------------
     * Step 4: Read back mode state to verify restore
     * ----------------------------------------------------------------------- */
    USER_LOG_INFO("--> Step 4: Get extreme battery endurance mode to verify restore");
    returnCode = DjiFlightController_GetExtremeBatteryEnduranceModeEnabled(&modeEnabled);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Get extreme battery endurance mode failed, error code: 0x%08X", returnCode);
        goto out;
    }
    USER_LOG_INFO("Extreme battery endurance mode is %s --> %s",
                  modeEnabled ? "enabled" : "disabled",
                  !modeEnabled ? "PASS" : "FAIL");

out:
    /* Safety cleanup: if extreme endurance mode was enabled but not yet restored,
     * attempt to disable it to recover smart battery go-home and landing protection. */
    if (step1Done) {
        USER_LOG_WARN("Extreme battery endurance mode is still active, attempting to restore safety protection...");
        if (DjiFlightController_SetExtremeBatteryEnduranceModeEnabled(false) == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
            USER_LOG_INFO("Safety protection restored successfully.");
        } else {
            USER_LOG_ERROR("Failed to restore safety protection! Please manually disable extreme battery endurance mode.");
        }
    }
    USER_LOG_INFO("Flight control extreme battery endurance mode sample end");
    return returnCode;
}

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
