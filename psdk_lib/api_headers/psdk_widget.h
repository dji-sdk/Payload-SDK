/**
 ********************************************************************
 * @file    psdk_widget.h
 * @brief   This is the header file for "psdk_widget.c", defining the structure and
 * (exported) function prototypes.
 *
 * @copyright (c) 2018 DJI. All rights reserved.
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
#ifndef PSDK_WIDGET_H
#define PSDK_WIDGET_H

/* Includes ------------------------------------------------------------------*/
#include <psdk_typedef.h>
#include "psdk_data_transmission.h"
#include "psdk_aircraft_info.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/
/*! The maximum length of a message that can be displayed by the mobile app floating window */
#define PSDK_WIDGET_FLOATING_WINDOW_MSG_MAX_LEN         255

/* Exported types ------------------------------------------------------------*/
/**
 * @brief Widget types.
 */
typedef enum {
    PSDK_WIDGET_TYPE_BUTTON = 1, /*!< button widget type */
    PSDK_WIDGET_TYPE_SWITCH = 2, /*!< switch widget type */
    PSDK_WIDGET_TYPE_SCALE = 3, /*!< scale widget type */
    PSDK_WIDGET_TYPE_LIST = 4, /*!< list widget type */
    PSDK_WIDGET_TYPE_INT_INPUT_BOX = 5, /*!< integer input box widget type */
} E_PsdkWidgetType;

/**
 * @brief Button widget state.
 */
typedef enum {
    PSDK_WIDGET_BUTTON_STATE_PRESS_DOWN = 1, /*!< Button is pressed down */
    PSDK_WIDGET_BUTTON_STATE_RELEASE_UP = 0, /*!< Button is released up */
} E_PsdkWidgetButtonState;

/**
 * @brief Switch widget state.
 */
typedef enum {
    PSDK_WIDGET_SWITCH_STATE_OFF = 0, /*!< Switch is turned off */
    PSDK_WIDGET_SWITCH_STATE_ON = 1 /*!< Switch is turned on */
} E_PsdkWidgetSwitchState;

/**
 * @brief Widget file binary array.
 */
typedef struct {
    char *fileName; /*!< The file name of the widget file */
    uint32_t fileSize; /*!< The file size of the widget file, uint : byte */
    const uint8_t *fileBinaryArray; /*!< The binary C array of the widget file */
} T_PsdkWidgetFileBinaryArray;

/**
 * @brief Widget binary array config.
 */
typedef struct {
    uint16_t binaryArrayCount; /*!< Binary array count. */
    T_PsdkWidgetFileBinaryArray *fileBinaryArrayList; /*!< Pointer to binary array list */
} T_PsdkWidgetBinaryArrayConfig;

/**
 * @brief Widget handler item.
 */
typedef struct {
    /*! The index of widget, the index can be numbered starting from 0 and cannot be repeated */
    uint32_t widgetIndex;

    /*! The type of widget, refer to ::E_PsdkWidgetType */
    E_PsdkWidgetType widgetType;

    /**
     * @brief Prototype of callback function used to set widget value, the function will be call when the user triggers
     *        the widget.
     * @param widgetType: the type of widget, refer to ::E_PsdkWidgetType.
     * @param index: the index of widget.
     * @param value: the value of widget, need be set.
     * if the widget type is PSDK_WIDGET_TYPE_BUTTON, the value is refer to ::E_PsdkWidgetButtonState;
     * if the widget type is PSDK_WIDGET_TYPE_SWITCH, the value is refer to ::E_PsdkWidgetSwitchState;
     * if the widget type is PSDK_WIDGET_TYPE_SCALE, the value is range from 0 to 100, which represents the percentage
     * of the scale slider;
     * if the Widget type is PSDK_WIDGET_TYPE_LIST, the value is range from 0 to N-1 (N is the value of list item
     * count), which represents which item is chosen;
     * if the widget type is PSDK_WIDGET_TYPE_INT_INPUT_BOX, the value is the input value of int input box widget.
     * @param userData: the user data need used in callback.
     * @return Execution result.
     */
    T_PsdkReturnCode (*SetWidgetValue)(E_PsdkWidgetType widgetType, uint32_t index, int32_t value, void *userData);

    /**
     * @brief Prototype of callback function used to get widget value.
     * @param widgetType: the type of widget, refer to ::E_PsdkWidgetType.
     * @param index
     * @param value: the value of widget, need be set.
     * if the widget type is PSDK_WIDGET_TYPE_BUTTON, the value is refer to ::E_PsdkWidgetButtonState;
     * if the widget type is PSDK_WIDGET_TYPE_SWITCH, the value is refer to ::E_PsdkWidgetSwitchState;
     * if the widget type is PSDK_WIDGET_TYPE_SCALE, the value is range from 0 to 100, which represents the percentage
     * of the scale slider;
     * if the Widget type is PSDK_WIDGET_TYPE_LIST, the value is range from 0 to N-1 (N is the value of list item
     * count), which represents which item is chosen;
     * if the widget type is PSDK_WIDGET_TYPE_INT_INPUT_BOX, the value is the input value of int input box widget.
     * @param userData: the user data need used in callback function.
     * @return Execution result.
     */
    T_PsdkReturnCode (*GetWidgetValue)(E_PsdkWidgetType widgetType, uint32_t index, int32_t *value, void *userData);

    /*! the user data need used in SetWidgetValue and GetWidgetValue callback function. */
    void *userData;
} T_PsdkWidgetHandlerListItem;

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Initialise widget module, and user should call this function before using widget features.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkWidget_Init(void);

#if PSDK_ARCH_SYS_LINUX

/**
 * @brief  Register default widget UI configuration file directory path.
 * @note Under Linux system, there are two functions to set the custom widget configuration directory path, function
 *       PsdkWidget_RegDefaultConfigByDirPath and PsdkWidget_RegUiConfigByDirPath. When you don't need multi-language
 *       and multi-screen size support, you can just use PsdkWidget_RegDefaultUiConfigByDirPath function set widget UI
 *       Config directory path. If you need support multi-language and multi-screen size support, you can use function
 *       PsdkWidget_RegUiConfigByDirPath to specify widget configuration. When the language and screen size is not
 *       cover in your setting by PsdkWidget_RegUiConfigByDirPath, the widget UI configuration uses setting by
 *       PsdkWiget_RegDefaultUiConfigByDirPath function.
 * @param widgetConfigDirPath: the widget UI configuration directory path.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkWidget_RegDefaultUiConfigByDirPath(const char *widgetConfigDirPath);

/**
 * @brief Register widget UI configuration file directory path.
 * @note Different widget UI configurations for several language and screen size require the same widget type, index and
 * count.
 * @param appLanguage: mobile app language type.
 * @param appScreenType: mobile app screen type.
 * @param widgetConfigDirPath: the widget UI configuration directory path.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkWidget_RegUiConfigByDirPath(E_PsdkAircraftInfoMobileAppLanguage appLanguage,
                                                 E_PsdkAircraftInfoMobileAppScreenType appScreenType,
                                                 const char *widgetConfigDirPath);
#endif

/**
 * @brief Register default widget UI config by binary array configuration.
 * @note In RTOS, most likely there is no file system. The widget config file content can use C array express. Use this
 * function and PsdkWidget_RegDefaultUiConfigBinaryArray set widget UI configuration. When the language and screen size
 * is not cover in your setting by PsdkWidget_RegUiConfigByBinaryArray, the widget UI configuration uses setting by this
 * function.
 * @param binaryArrayConfig: the binary array config for widget UI configuration.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkWidget_RegDefaultUiConfigByBinaryArray(const T_PsdkWidgetBinaryArrayConfig *binaryArrayConfig);

/**
 * @brief Register widget UI config by binary array configuration.
 * @note Different widget UI configurations for several language and screen size require the same widget type, index and
 * count.
 * @param appLanguage: mobile app language type.
 * @param screenType: mobile app screen type.
 * @param binaryArrayConfig: the binary array config for widget UI configuration.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkWidget_RegUiConfigByBinaryArray(E_PsdkAircraftInfoMobileAppLanguage appLanguage,
                                                     E_PsdkAircraftInfoMobileAppScreenType screenType,
                                                     const T_PsdkWidgetBinaryArrayConfig *binaryArrayConfig);

/**
 * @brief Register handler list for widgets.
 * @param widgetHandlerList: widget handler list for widgets.
 * @param itemCount: the item count of widget handler list.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkWidget_RegHandlerList(const T_PsdkWidgetHandlerListItem *widgetHandlerList, uint32_t itemCount);

/**
 * @brief Send message to mobile app floating window.
 * @note the message length can't more than PSDK_WIDGET_FLOATING_WINDOW_MSG_MAX_LEN. The max data bandwidth of floating
 * windows message is 2KB/s.
 * @param str: pointer to message string.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkWidgetFloatingWindow_ShowMessage(const char *str);

/**
 * @brief Get data transmission state of floating window channel. User can use the state as base for controlling
 * floating windows message send.
 * @param state: pointer to floating window channel state.
 * @return Execution result.
 */
T_PsdkReturnCode PsdkWidgetFloatingWindow_GetChannelState(T_PsdkDataChannelState *state);

#ifdef __cplusplus
}
#endif

#endif // PSDK_WIDGET_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
