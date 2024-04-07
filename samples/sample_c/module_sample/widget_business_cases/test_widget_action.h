/**
 ********************************************************************
 * @file    test_widget_action.h
 * @brief   This is the header file for "test_widget_action.c", defining the structure and
 * (exported) function prototypes.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef TEST_WIDGET_ACTION_H
#define TEST_WIDGET_ACTION_H

/* Includes ------------------------------------------------------------------*/
#include <dji_typedef.h>
#include <dji_widget.h>

#ifdef TEST_WIDGET_ACTION_C
    #define TEST_WIDGET_ACTION_EXT
#else
    #define TEST_WIDGET_ACTION_EXT      extern
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/
#define TEST_WIDGET_GANG_MAGIC_BYTE     (0x5A)
#define TEST_WIDGET_IS_GANGED(index)    ( ( TEST_WIDGET_GANG_MAGIC_BYTE == ((uint32_t)index >> 24) ) ? true : false )
#define TEST_WIDGET_IS_GANGED_LEADER(index)     ( { \
    /* (TEST_WIDGET_GANG_MAGIC_BYTE == coupledIndex->gang_magic) && (coupledIndex->unique_idx == coupledIndex->leader_idx) */ \
    ( TEST_WIDGET_IS_GANGED(index) && ( (((uint32_t)index >> 8) & 0xFF) == ((uint32_t)index & 0xFF) ) ) ? true : false ; \
} )
#define TEST_WIDGET_IS_GANGED_JUNIOR(index)      ( { \
    /* (TEST_WIDGET_GANG_MAGIC_BYTE == coupledIndex->gang_magic) && (coupledIndex->unique_idx != coupledIndex->leader_idx) */ \
    ( TEST_WIDGET_IS_GANGED(index) && ( (((uint32_t)index >> 8) & 0xFF) != ((uint32_t)index & 0xFF) ) ) ? true : false ; \
} )
#define TEST_WIDGET_IS_VALID_GANGED(index, type)    ( \
    /* for master (coupledIndex->major_type == type) */ \
    ( TEST_WIDGET_IS_GANGED_LEADER(index) && ( type == (((uint32_t)index >> 20) & 0xF) ) ) \
    /* or for junior (coupledIndex->minor_type == type) */ \
    || ( TEST_WIDGET_IS_GANGED_JUNIOR(index) && ( type == (((uint32_t)index >> 16) & 0xF) ) ) \
)

/* Exported types ------------------------------------------------------------*/
typedef enum {
    USER_WIDGET_SWITCH_COUPLE_TO_LIST   = (DJI_WIDGET_TYPE_LIST << 4) | (DJI_WIDGET_TYPE_SWITCH),
    USER_WIDGET_LIST_COUPLE_TO_LIST     = (DJI_WIDGET_TYPE_LIST << 4) | (DJI_WIDGET_TYPE_LIST),
    USER_WIDGET_INT_INPUT_COUPLE_TO_INT = (DJI_WIDGET_TYPE_INT_INPUT_BOX << 4) | (DJI_WIDGET_TYPE_INT_INPUT_BOX),
} E_UserWidgetGangType;

#if __BYTE_ORDER == __LITTLE_ENDIAN
    typedef struct {
        uint8_t                         unique_idx;     // index of this widget
        uint8_t                         leader_idx;     // index of this gang leader
        union {
            uint8_t                     gang_type;
            struct {
                uint8_t                 minor_type:4;   // ganged widget type of widget gang
                uint8_t                 major_type:4;   // leader widget type of widget gang
            };
        };
        uint8_t                         gang_magic;     // magic of gang widget
    } T_CoupledInfo;
#elif __BYTE_ORDER == __BIG_ENDIAN
    typedef struct {
        uint8_t                         gang_magic;     // magic of gang widget
        union {
            uint8_t                     gang_type;
            struct {
                uint8_t                 major_type:4;   // leader widget type of widget gang
                uint8_t                 minor_type:4;   // ganged widget type of widget gang
            };
        };
        uint8_t                         leader_idx;     // index of this gang leader
        uint8_t                         unique_idx;     // index of this widget
    } T_CoupledInfo;
#else
    #ERROR "No byte order is declared!"
#endif

typedef T_DjiReturnCode (*T_UserWorkHandle)(void *userData);

typedef struct tagT_UserWidgetBasicData {
    union {
        T_CoupledInfo                   coupledInfo;
        uint32_t                        gangedIndex;
    };
    struct tagT_UserWidgetBasicData     *leaderData;

    T_UserWorkHandle                    action;
    int32_t                             value;

    int32_t                             pDataLen;
    int8_t                              pData[0];
} T_UserWidgetBasicData;

typedef struct tagT_UserWidgetListPrivateData {
    int32_t                             itemCount;
    int32_t                             itemStatus[0];  // TODO: maybe should expand to bitmap
} T_UserWidgetListPrivateData;

// typedef struct tagT_UserWidgetScaleData {
//     T_UserWidgetBasicData               widgetUserData;
//     T_UtilLinkList                      scaleSurfaceStack;
// } T_UserWidgetScaleData;

// typedef struct tagT_UserWidgetGangedPrivateData {
//     T_UserWidgetBasicData               *leaderData;
//     int8_t                              selfPrivate[0];
// } T_UserWidgetGangedPrivateData;

/* Exported functions --------------------------------------------------------*/
TEST_WIDGET_ACTION_EXT T_DjiReturnCode DjiTestWidget_FcSubscriptionTopicCoupledIntInputBoxAction(void *userData);
TEST_WIDGET_ACTION_EXT T_DjiReturnCode DjiTestWidget_FcSubscriptionBatchCoupledSwitchAction(void *userData);
TEST_WIDGET_ACTION_EXT T_DjiReturnCode DjiTestWidget_CameraSimulatorButtonAction(void *userData);
TEST_WIDGET_ACTION_EXT T_DjiReturnCode DjiTestWidget_LiveViewCoupledSwitchAction(void *userData);
TEST_WIDGET_ACTION_EXT T_DjiReturnCode DjiTestWidget_DepthImageCoupledSwitchAction(void *userData);
TEST_WIDGET_ACTION_EXT T_DjiReturnCode DjiTestWidget_HMSIntInputBoxAction(void *userData);

#ifdef __cplusplus
}
#endif

#endif // TEST_WIDGET_ACTION_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
