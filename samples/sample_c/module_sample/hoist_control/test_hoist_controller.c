/**
 ********************************************************************
 * @file    test_hoist_controller.c
 * @brief
 *
 * @copyright (c) 2025 DJI. All rights reserved.
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
#include "dji_logger.h"
#include "dji_platform.h"
#include "utils/util_misc.h"
#include "dji_aircraft_info.h"
#include "test_hoist_controller.h"

/* Private constants ---------------------------------------------------------*/
#define DATA_TRANSMISSION_TASK_FREQ         (2)
#define DATA_TRANSMISSION_TASK_STACK_SIZE   (2048)
#define DATA_TRANSMISSION_TASK_MSG          (2000)
#define HOOK_EMUN_OFFSET                    (8)
#define DJI_HOIST_CARGO_MAX_CMD             (0xff)
/* Private types -------------------------------------------------------------*/

/* Private functions declaration ---------------------------------------------*/
static void *UsrHoistController_Task(void *arg);

/* Private variables ---------------------------------------------------------*/
static T_DjiTaskHandle s_usrHoistControllerThraed;
static T_DjiAircraftInfoBaseInfo s_aircraftInfoBaseInfo;
static E_DjiHoistControllerCmdStatus s_cmdAction = DJI_HOIST_CARGO_STOP_CMD;
static psdk_receive_hoist_status_t hoist_status = {0};
static psdk_receive_simple_hoist_status_t simple_hoist_status = {0};
static psdk_receive_hook_status_t  hook_status  = {0};
static T_DjiReturnCode  dji_hoist_type  = DJI_EEOR_LOAD;

/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode DjiTest_HoistControllerStartService(void)
{
    T_DjiReturnCode djiStat;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    djiStat = DjiHoistControllerManagement_Init();
    if(djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS){
        USER_LOG_ERROR("sample<hoist controller>,init failed!!!");
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    djiStat = DjiAircraftInfo_GetBaseInfo(&s_aircraftInfoBaseInfo);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("get aircraft base info error");
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    if (osalHandler->TaskCreate("user_transmission_task", UsrHoistController_Task,
                                DATA_TRANSMISSION_TASK_STACK_SIZE, NULL, &s_usrHoistControllerThraed) !=
        DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("user data transmission task create error.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode DjiTest_HoistControllerStopService(void)
{
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    T_DjiReturnCode returnCode;

    s_cmdAction = DJI_HOIST_CARGO_STOP_CMD;
    if (osalHandler->TaskDestroy(s_usrHoistControllerThraed) != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("user hoist controller task destroy error.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    returnCode = DjiHoistControllerManagement_DeInit();
    if (DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS != returnCode){
        USER_LOG_ERROR("user hoist controller deinit error.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/* Private functions definition-----------------------------------------------*/
static T_DjiReturnCode subscriptionHoistAndHookStatusData(void)
{
    T_DjiReturnCode djiStat;
    static uint32_t cnt = 0;

    cnt++;

    switch (dji_hoist_type)
    {
    case DJI_STANDARD_HOIST:
            djiStat = DjiHoistController_hoistStatus(&hoist_status);
            if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS){
                return djiStat;
            }

            if (cnt % 5 == 0){
                USER_LOG_INFO("hoist status:cab_s:%d ,can_s:%d, g_w:%d, mg_w:%d, c_s_s:%d, cc_l:%d",
                    hoist_status.cable_state, hoist_status.can_status, hoist_status.goods_weight, hoist_status.max_goods_weight,
                    hoist_status.cargo_swing_speed, hoist_status.cur_cable_length);
                USER_LOG_INFO("hoist status: c_f:%d, c_an:%d, is_r_t:%d, is_rmr:%d, cdcs:%d, c_s:%d",
                    hoist_status.cable_force, hoist_status.cable_angle, hoist_status.is_reach_top, hoist_status.is_reach_max_round,
                    hoist_status.cable_direction_check_state, hoist_status.cable_speed);
            }

            djiStat = DjiHoistController_hookStatus(&hook_status);
            if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS){
                return djiStat;
            }
            if (cnt % 5 == 0){
                USER_LOG_INFO("hook status: steer_state:%d, weight:%d, warning:%d",
                    hook_status.steer_state, hook_status.cargo_weight, hook_status.cargo_warnig);
                USER_LOG_INFO("hook status: error:%d, ctr_state:%d", hook_status.cargo_error, hook_status.ctrl_state);
            }
        break;
    case DJI_SIMPLE_HOIST:
            djiStat = DjiHoistController_simpleHoistStatus(&simple_hoist_status);
            if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS){
                return djiStat;
            }
            if (cnt % 5 == 0){
                USER_LOG_INFO("simple hoist status: f_kg:%d, angle_z:%d", simple_hoist_status.force_kg,simple_hoist_status.angle_deg);
            }
        break;
    default:
        djiStat = DJI_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT_IN_CURRENT_STATE;
        if (cnt % 5 == 0)
        {
            USER_LOG_INFO("hosit type is error!!!");
        }
        break;
    }


    return djiStat;
}

T_DjiReturnCode sample_psdk_hoistControllerCmd(void)
{
    static uint32_t freqCount = 0;
    static uint32_t loopCount = 0;
    T_DjiReturnCode djiStat;
    static E_DjiHoistControllerCmdStatus pre_cmdAction = DJI_HOIST_CARGO_STOP_CMD;
    static bool AfterHoistControllerStopInit = false;

    if (dji_hoist_type != DJI_STANDARD_HOIST){
        return DJI_ERROR_SYSTEM_MODULE_CODE_NONSUPPORT_IN_CURRENT_STATE;
    }

    if (freqCount++ % 2){ //2s
        return  DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    }

    if (pre_cmdAction != s_cmdAction){
        loopCount = 0;
    }

    pre_cmdAction = s_cmdAction;
    loopCount++;

    switch (s_cmdAction)
    {
        case DJI_HOIST_CARGO_STOP_CMD: {
            djiStat = psdk_hoistControllerCmdAction(DJI_HOIST_CARGO_STOP_CMD);
            if (djiStat == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS){
                if (!AfterHoistControllerStopInit){
                    USER_LOG_INFO("hosit concroller: curent anction cmd is hoist stop is %d!!!", loopCount);
                    s_cmdAction = (loopCount <= 10) ? DJI_HOIST_CARGO_STOP_CMD : DJI_HOIST_CARGO_RELEASE_CMD; //20s stop
                    if (s_cmdAction == DJI_HOIST_CARGO_RELEASE_CMD) {
                        AfterHoistControllerStopInit = true;
                    }
                } else {
                    USER_LOG_INFO("hosit concroller: curent anction cmd is hoist stop!!!");
                    s_cmdAction = (loopCount <= 2) ? DJI_HOIST_CARGO_STOP_CMD : DJI_HOIST_CARGO_RECEIVE_CMD; //4s stop
                }
            }
            break;
        }
        case DJI_HOIST_CARGO_RELEASE_CMD: {
            djiStat = psdk_hoistControllerCmdAction(DJI_HOIST_CARGO_RELEASE_CMD);
            if (djiStat == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS){
                USER_LOG_INFO("hosit concroller: curent anction cmd is hoist release is %d!!!", loopCount);
                s_cmdAction = (loopCount <= 3) ? DJI_HOIST_CARGO_RELEASE_CMD : DJI_HOIST_CARGO_STOP_CMD; //6s release
            }
            break;
        }
        case DJI_HOIST_CARGO_RECEIVE_CMD: {
            djiStat = psdk_hoistControllerCmdAction(DJI_HOIST_CARGO_RECEIVE_CMD);
            if (djiStat == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS){
                USER_LOG_INFO("hosit concroller: curent anction cmd is hoist receive is %d!!!", loopCount);
                s_cmdAction = (loopCount <= 3) ? DJI_HOIST_CARGO_RECEIVE_CMD : (DJI_HOOK_CARGO_OPEN_HOOK + HOOK_EMUN_OFFSET); //6s receive
                if (hoist_status.is_reach_top == 1){
                    s_cmdAction = DJI_HOOK_CARGO_OPEN_HOOK + HOOK_EMUN_OFFSET;
                }
            }
            break;
        }
        case (DJI_HOOK_CARGO_OPEN_HOOK + HOOK_EMUN_OFFSET): {
            djiStat = psdk_hookControllerCmdAction(DJI_HOOK_CARGO_OPEN_HOOK);
            if (djiStat == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS){
                USER_LOG_INFO("hook concroller: curent anction cmd is open hook!!!");
                if (hook_status.steer_state == 2){
                    s_cmdAction = DJI_HOOK_CARGO_OLOSE_HOOK + HOOK_EMUN_OFFSET;
                }
            }
            break;
        }
        case (DJI_HOOK_CARGO_OLOSE_HOOK + HOOK_EMUN_OFFSET): {
            djiStat = psdk_hookControllerCmdAction(DJI_HOOK_CARGO_OLOSE_HOOK);
            if (djiStat == DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS){
                USER_LOG_INFO("hook concroller: curent anction cmd is close hook!!!");
                if (hook_status.steer_state == 1){
                    s_cmdAction = DJI_HOIST_CARGO_MAX_CMD;
                }
            }
            break;
        }
        default:
            break;
    }
    return  djiStat;
}

static void *UsrHoistController_Task(void *arg)
{
    static T_DjiReturnCode djiStat = DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_REQUEST_PARAMETER;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    uint16_t  retry_cnt = 0;

    USER_UTIL_UNUSED(arg);
    // Get avoid data for FC100
    psdk_receive_avoid_status_t avoid_data = {0};

    while (true) {
        osalHandler->TaskSleepMs(DATA_TRANSMISSION_TASK_MSG / DATA_TRANSMISSION_TASK_FREQ); //1000ms

        DjiHoistController_flightAvoidStatus(&avoid_data);
        if (avoid_data.is_avoid_stop_finish || avoid_data.is_cargo_avoid || avoid_data.is_start_avoid){
            USER_LOG_INFO("aircraft start avoid stop_finish:%d, cargo_aviod:%d, start avoid:%d !!!",avoid_data.is_avoid_stop_finish,
                    avoid_data.is_cargo_avoid, avoid_data.is_start_avoid);
        }

        if(djiStat != DJI_STANDARD_HOIST && djiStat != DJI_SIMPLE_HOIST){
            djiStat = psdk_HoistController_NotifyFlight_to_startPushInfo();

            if (djiStat == DJI_STANDARD_HOIST)
                dji_hoist_type = DJI_STANDARD_HOIST;
            else if (djiStat == DJI_SIMPLE_HOIST)
                dji_hoist_type = DJI_SIMPLE_HOIST;
            else {
                dji_hoist_type = DJI_EEOR_LOAD;
                retry_cnt++;
                if (retry_cnt > 2)
                   break;
            }

        } else {
            subscriptionHoistAndHookStatusData();
            sample_psdk_hoistControllerCmd();
        }
    }
}
/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
