/**
 ********************************************************************
 * @file    dji_psdk_hoist_controller.h
 * @brief   This is the header file for "dji_psdk_hoist_controller.c", defining the structure and
 * (exported) function prototypes.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef DJI_PSDK_HOIST_CONTROLLER_H
#define DJI_PSDK_HOIST_CONTROLLER_H

/* Includes ------------------------------------------------------------------*/
#include <dji_typedef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief hoist command status.
 */
typedef enum {
    DJI_HOIST_CARGO_RELEASE_CMD = 0, //relaese goods
    DJI_HOIST_CARGO_STOP_CMD    = 1, //stop relaese or receive goods
    DJI_HOIST_CARGO_RECEIVE_CMD = 2, //receive goods
    DJI_HOIST_CARGO_MAINTENANCE_CMD = 4, //mainteance
    DJI_HOIST_CARGO_ESCAPE_TROUBLE = 6,   //escape rope
} E_DjiHoistControllerCmdStatus;


typedef enum {
    DJI_HOOK_CARGO_OLOSE_HOOK = 1,     //close hook
    DJI_HOOK_CARGO_OPEN_HOOK = 2,       //open hook
} E_DjiHookControllerCmdStatus;

typedef enum {
    DJI_HOIST_CABLE_UP_DONE = 0, //finish taking up
    DJI_HOIST_CABLE_TAKING_UP = 0x1,//taking up
    DJI_HOIST_CABLE_PAUSE   =  0X2, //pause action
    DJI_HOIST_CABLE_TAKING_DOWN = 0x3,//taking down
    DJI_HOIST_CABLE_DOWN_DONE= 0x4,//finish taking down
    DJI_HOIST_CABLE_SYSTEM_PROTECT= 0xf,//system error
    DJI_HOIST_CABLE_UNKONW = 0XFF,
} E_DjiHoistCableStatus;

typedef enum {
    DJI_STANDARD_HOIST = 5,     //standard hoist
    DJI_SIMPLE_HOIST = 4,       //simple hoist
    DJI_EEOR_LOAD = 0,       //error
} E_DjiHoistType;


typedef enum {
    DJI_ESCAPE_CTR_DISABLE = 0, //disable escape
    DJI_ESCAPE_CTR_ENABLE  = 1, //enable escape
    DJI_ROD_CTRL_CMD_OUT_ONLY = 2, //push out ,only apply to rod simple hoist
    DJI_ROD_CTRL_CMD_IN_ONLY = 3,  //push in ,only apply to rod simple hoist
} E_DjiSimpleHoistEscapeCmd;

/**
 * @brief hoist status from hoist system data structure for FC100,FC200
 */
typedef struct {
    /*cerrent staus,0x00: finish taking up; 0x01:taking up; 0x02:pause acion; 0x3:taking down; 0x04:finish taking down; 0x0f: hoist system error*/
    E_DjiHoistCableStatus cable_state;
    /*bit0:can take donw; bit1:can take up; bit7: can controller hoist*/
    uint8_t can_status;
    int32_t goods_weight; /*goods weight,uints:g*/
    uint32_t max_goods_weight;  /*max goods weight,uints:g*/
    uint16_t cargo_swing_speed;
    uint16_t cur_cable_length; /*current cable length,uints:m*/
    uint16_t cable_force;    /*cable force,uints:g*/
    uint16_t cable_angle;    /*cable angle, uints:0.1deg*/
    uint8_t  is_reach_top;   /*goods is reach top, 1:yes; 0:no*/
    uint8_t  is_reach_max_round; /*cable is max round*/
    /*cable check state: 0x00:check_idle; 0x01:cable checking; 0x2:cable check result is true; 0x3:cable check result is false*/
    uint8_t  cable_direction_check_state;
    uint16_t cable_speed;  /*curent cable action speed, uints: cm/s*/
} __attribute__((packed)) psdk_receive_hoist_status_t;

/**
 * @brief smart hook status from hoist system data structure for FC100,FC200.
 */
typedef struct {
   uint8_t  version;
   uint8_t  steer_state;  //0x0:hook unknow;0x1 hook close; 0x2: hook open; 0x3:hook middile
   int16_t  cargo_weight; // uints:0.1 kg
   /* cargo_warnig: value = 0: normal; bit0:bat is not in place; bit1: bat is lowpower; bit2:bat is low voltage
      bit3:steer is not in place;   bit4:steer is stalled;  bit5:disconnect;   bit14: steer is abnormal; bit15: hook is not activate */
   uint16_t cargo_warnig;
   uint16_t cargo_error; // hook faukt,0 is normal
   uint8_t  ctrl_state;  //hook controller status: 0: normal; 1:steer is powering; 2:steer is powered; 3:steer is moving(open); 4:steer stop normal; 5:steer is moving(close)
} __attribute__((packed)) psdk_receive_hook_status_t;

/**
 * @brief smart hook status from hoist system data structure for FC100,FC200.
 */
typedef struct {
   uint8_t  version;
   uint8_t  sensor_damage;  //0x0:OK;0x1 damage
   uint8_t  weight_k_err; // 0x0: OK,other is error;
   int16_t  force_kg;  // z; force/100 = kg
   int16_t  angle_deg; // z; angle/100 = deg
} __attribute__((packed)) psdk_receive_simple_hoist_status_t;


typedef struct {
   uint8_t  is_cargo_avoid:1;  //cargo avoid
   uint8_t  is_avoid_stop_finish:1;  //avoid flight stop
   uint8_t  is_start_avoid:1; //avoid,start reduce speed
   uint8_t  reserve:5;
} __attribute__((packed)) psdk_receive_avoid_status_t;
/**
 * @brief Initialise hoist controller module, and user should call this function before using hoist controller features.
 * @return Execution result.
 */
T_DjiReturnCode DjiHoistControllerManagement_Init(void);

/**
 * @brief DeInitialise hoist controller module, and user should call this function before using hoist controller features.
 * @return Execution result.
 */
T_DjiReturnCode DjiHoistControllerManagement_DeInit(void);

/**
 * @brief notify flightcontorller can control hoist system in FC100,FC200.
 * @note we use it must,befor use DjiHoistControllerManagement_Init().
 * @param void: none
 * @return Execution E_DjiHoistType.
 * typedef enum {
 *  DJI_STANDARD_HOIST = 5,     //standard hoist
 *  DJI_SIMPLE_HOIST = 4,       //simple hoist
 *  DJI_EEOR_LOAD = 0,       //error
 *} E_DjiHoistType;
 */
T_DjiReturnCode psdk_HoistController_NotifyFlight_to_startPushInfo(void);

/**
 * @brief Get hoist stuts for PSDK user in FC100 FC200.
 * @note we use it must,befor use psdk_HoistController_NotifyFlight_to_startPushInfo().
 * @param hoistStatus: return hoist status struction :psdk_receive_hoist_status_t
 * @return Execution result.
 */
T_DjiReturnCode DjiHoistController_hoistStatus(psdk_receive_hoist_status_t *hoistStatus);


/**
 * @brief Get simple hoist stuts for PSDK user in FC100, FC200.
 * @note we use it must,befor use psdk_HoistController_NotifyFlight_to_startPushInfo().
 * @param hoistStatus: return simple hoist status struction :psdk_receive_simple_hoist_status_t
 * @return Execution result.
 */
T_DjiReturnCode DjiHoistController_simpleHoistStatus(psdk_receive_simple_hoist_status_t *simpleHoistStatus);

/**
 * @brief Get hoist stuts for PSDK user in FC100, FC200.
 * @note we use it must,befor use psdk_HoistController_NotifyFlight_to_startPushInfo().
 * @param hookStatus: return hoist status struction :psdk_receive_hook_status_t
 * @return Execution result.
 */
T_DjiReturnCode DjiHoistController_hookStatus(psdk_receive_hook_status_t *hookStatus);


/**
 * @brief Set hoist command for PSDK user in FC100, FC200.
 * @note we use it must,befor use psdk_HoistController_NotifyFlight_to_startPushInfo().
 * @param cmd: set param is E_DjiHoistControllerCmdStatus
 * @return Execution result.
 */
T_DjiReturnCode psdk_hoistControllerCmdAction(E_DjiHoistControllerCmdStatus cmd);

/**
 * @brief Set hook command for PSDK user in FC100, FC200.
 * @note we use it must,befor use psdk_HoistController_NotifyFlight_to_startPushInfo().
 * @param cmd: set param is E_DjiHookControllerCmdStatus
 * @return Execution result.
 */
T_DjiReturnCode psdk_hookControllerCmdAction(E_DjiHookControllerCmdStatus cmd);

/**
 * @brief Set simple hoist escape command for PSDK user in FC100, FC200.
 * @note we use it must,befor use psdk_HoistController_NotifyFlight_to_startPushInfo().
 * @param cmd: set param is E_DjiSimpleHoistEscapeCmd
 * @return Execution result.
 */
T_DjiReturnCode psdk_simpleHoistEscapeCmdAction(E_DjiSimpleHoistEscapeCmd cmd);

/**
 * @brief Get avoid stuts for PSDK user in FC100, FC200.
 * @note we use it must below 10HZ
 * @param cmd: Get param is psdk_receive_avoid_status_t
 * @return Execution result.
 */
T_DjiReturnCode DjiHoistController_flightAvoidStatus(psdk_receive_avoid_status_t *avoidStatus);
#ifdef __cplusplus
}
#endif

#endif // DJI_PSDK_HOIST_CONTROLLER__H
