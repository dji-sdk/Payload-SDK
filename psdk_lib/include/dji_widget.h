/**
 ********************************************************************
 * @file    dji_widget.h
 * @brief   This is the header file for "dji_widget.c", defining the structure and
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
#ifndef DJI_WIDGET_H
#define DJI_WIDGET_H

/* Includes ------------------------------------------------------------------*/
#include <dji_typedef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/
/*! The maximum length of a message that can be displayed by the mobile app floating window */
#define DJI_WIDGET_FLOATING_WINDOW_MSG_MAX_LEN         255

/* Exported types ------------------------------------------------------------*/
/**
 * @brief Widget types.
 */
typedef enum {
    DJI_WIDGET_TYPE_BUTTON = 1, /*!< button widget type */
    DJI_WIDGET_TYPE_SWITCH = 2, /*!< switch widget type */
    DJI_WIDGET_TYPE_SCALE = 3, /*!< scale widget type */
    DJI_WIDGET_TYPE_LIST = 4, /*!< list widget type */
    DJI_WIDGET_TYPE_INT_INPUT_BOX = 5, /*!< integer input box widget type */
} E_DjiWidgetType;

/**
 * @brief Button widget state.
 */
typedef enum {
    DJI_WIDGET_BUTTON_STATE_PRESS_DOWN = 1, /*!< Button is pressed down */
    DJI_WIDGET_BUTTON_STATE_RELEASE_UP = 0, /*!< Button is released up */
} E_DjiWidgetButtonState;

/**
 * @brief Switch widget state.
 */
typedef enum {
    DJI_WIDGET_SWITCH_STATE_OFF = 0, /*!< Switch is turned off */
    DJI_WIDGET_SWITCH_STATE_ON = 1 /*!< Switch is turned on */
} E_DjiWidgetSwitchState;

/**
 * @brief Switch widget speaker work mode.
 */
typedef enum {
    DJI_WIDGET_SPEAKER_WORK_MODE_TTS = 0,
    DJI_WIDGET_SPEAKER_WORK_MODE_VOICE = 1,
} E_DjiWidgetSpeakerWorkMode;

/**
 * @brief Switch widget speaker play mode.
 */
typedef enum {
    DJI_WIDGET_SPEAKER_PLAY_MODE_SINGLE_PLAY = 0,
    DJI_WIDGET_SPEAKER_PLAY_MODE_LOOP_PLAYBACK = 1,
} E_DjiWidgetSpeakerPlayMode;

/**
 * @brief Switch widget speaker state.
 */
typedef enum {
    DJI_WIDGET_SPEAKER_STATE_IDEL = 0,
    DJI_WIDGET_SPEAKER_STATE_TRANSMITTING = 1,
    DJI_WIDGET_SPEAKER_STATE_PLAYING = 2,
    DJI_WIDGET_SPEAKER_STATE_ERROR = 3,
    DJI_WIDGET_SPEAKER_STATE_IN_TTS_CONVERSION = 4,
} E_DjiWidgetSpeakerState;

/**
 * @brief Switch widget transmit data event.
 */
typedef enum {
    DJI_WIDGET_TRANSMIT_DATA_EVENT_START,
    DJI_WIDGET_TRANSMIT_DATA_EVENT_TRANSMIT,
    DJI_WIDGET_TRANSMIT_DATA_EVENT_FINISH,
    DJI_WIDGET_TRANSMIT_DATA_EVENT_ABORT,
} E_DjiWidgetTransmitDataEvent;

/**
 * @brief Widget file binary array.
 */
typedef struct {
    char *fileName; /*!< The file name of the widget file */
    uint32_t fileSize; /*!< The file size of the widget file, uint : byte */
    const uint8_t *fileBinaryArray; /*!< The binary C array of the widget file */
} T_DjiWidgetFileBinaryArray;

/**
 * @brief Widget binary array config.
 */
typedef struct {
    uint16_t binaryArrayCount; /*!< Binary array count. */
    T_DjiWidgetFileBinaryArray *fileBinaryArrayList; /*!< Pointer to binary array list */
} T_DjiWidgetBinaryArrayConfig;

/**
 * @brief Widget handler item.
 */
typedef struct {
    /*! The index of widget, the index can be numbered starting from 0 and cannot be repeated */
    uint32_t widgetIndex;

    /*! The type of widget, refer to ::E_DjiWidgetType */
    E_DjiWidgetType widgetType;

    /**
     * @brief Prototype of callback function used to set widget value, the function will be call when the user triggers
     *        the widget.
     * @param widgetType: the type of widget, refer to ::E_DjiWidgetType.
     * @param index: the index of widget.
     * @param value: the value of widget, need be set.
     * if the widget type is DJI_WIDGET_TYPE_BUTTON, the value is refer to ::E_DjiWidgetButtonState;
     * if the widget type is DJI_WIDGET_TYPE_SWITCH, the value is refer to ::E_DjiWidgetSwitchState;
     * if the widget type is DJI_WIDGET_TYPE_SCALE, the value is range from 0 to 100, which represents the percentage
     * of the scale slider;
     * if the Widget type is DJI_WIDGET_TYPE_LIST, the value is range from 0 to N-1 (N is the value of list item
     * count), which represents which item is chosen;
     * if the widget type is DJI_WIDGET_TYPE_INT_INPUT_BOX, the value is the input value of int input box widget.
     * @param userData: the user data need used in callback.
     * @return Execution result.
     */
    T_DjiReturnCode (*SetWidgetValue)(E_DjiWidgetType widgetType, uint32_t index, int32_t value, void *userData);

    /**
     * @brief Prototype of callback function used to get widget value.
     * @param widgetType: the type of widget, refer to ::E_DjiWidgetType.
     * @param index
     * @param value: the value of widget, need be set.
     * if the widget type is DJI_WIDGET_TYPE_BUTTON, the value is refer to ::E_DjiWidgetButtonState;
     * if the widget type is DJI_WIDGET_TYPE_SWITCH, the value is refer to ::E_DjiWidgetSwitchState;
     * if the widget type is DJI_WIDGET_TYPE_SCALE, the value is range from 0 to 100, which represents the percentage
     * of the scale slider;
     * if the Widget type is DJI_WIDGET_TYPE_LIST, the value is range from 0 to N-1 (N is the value of list item
     * count), which represents which item is chosen;
     * if the widget type is DJI_WIDGET_TYPE_INT_INPUT_BOX, the value is the input value of int input box widget.
     * @param userData: the user data need used in callback function.
     * @return Execution result.
     */
    T_DjiReturnCode (*GetWidgetValue)(E_DjiWidgetType widgetType, uint32_t index, int32_t *value, void *userData);

    /*! the user data need used in SetWidgetValue and GetWidgetValue callback function. */
    void *userData;
} T_DjiWidgetHandlerListItem;

typedef struct {
    union {
        /*! When event is 'DJI_WIDGET_TRANSMIT_DATA_EVENT_START', the buf contains file name, uuid and decoder bitrate. */
        struct {
            uint8_t fileName[32];
            uint8_t fileUuid[32];
            uint32_t fileDecodeBitrate;
        } transDataStartContent;

        /*! When event is 'DJI_WIDGET_TRANSMIT_DATA_EVENT_START', the buf contains file md5 sum. */
        struct {
            uint8_t md5Sum[16];
        } transDataEndContent;
    };
} T_DjiWidgetTransDataContent;

typedef struct {
    E_DjiWidgetSpeakerState state;
    E_DjiWidgetSpeakerWorkMode workMode;
    E_DjiWidgetSpeakerPlayMode playMode;
    uint8_t volume;
} T_DjiWidgetSpeakerState;

typedef struct {
    T_DjiReturnCode (*GetSpeakerState)(T_DjiWidgetSpeakerState *speakerState);
    T_DjiReturnCode (*SetWorkMode)(E_DjiWidgetSpeakerWorkMode workMode);
    T_DjiReturnCode (*SetPlayMode)(E_DjiWidgetSpeakerPlayMode playMode);
    T_DjiReturnCode (*SetVolume)(uint8_t volume);

    T_DjiReturnCode (*StartPlay)(void);
    T_DjiReturnCode (*StopPlay)(void);

    T_DjiReturnCode (*ReceiveTtsData)(E_DjiWidgetTransmitDataEvent event,
                                      uint32_t offset, uint8_t *buf, uint16_t size);
    T_DjiReturnCode (*ReceiveVoiceData)(E_DjiWidgetTransmitDataEvent event,
                                        uint32_t offset, uint8_t *buf, uint16_t size);
} T_DjiWidgetSpeakerHandler;

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Initialise widget module, and user should call this function before using widget features.
 * @return Execution result.
 */
T_DjiReturnCode DjiWidget_Init(void);

/**
 * @brief  Register default widget UI configuration file directory path.
 * @note Under Linux system, there are two functions to set the custom widget configuration directory path, function
 *       DjiWidget_RegDefaultConfigByDirPath and DjiWidget_RegUiConfigByDirPath. When you don't need multi-language
 *       and multi-screen size support, you can just use DjiWidget_RegDefaultUiConfigByDirPath function set widget UI
 *       Config directory path. If you need support multi-language and multi-screen size support, you can use function
 *       DjiWidget_RegUiConfigByDirPath to specify widget configuration. When the language and screen size is not
 *       cover in your setting by DjiWidget_RegUiConfigByDirPath, the widget UI configuration uses setting by
 *       DjiWiget_RegDefaultUiConfigByDirPath function.
 * @param widgetConfigDirPath: the widget UI configuration directory path.
 * @return Execution result.
 */
T_DjiReturnCode DjiWidget_RegDefaultUiConfigByDirPath(const char *widgetConfigDirPath);

/**
 * @brief Register widget UI configuration file directory path.
 * @note Different widget UI configurations for several language and screen size require the same widget type, index and
 * count.
 * @param appLanguage: mobile app language type.
 * @param appScreenType: mobile app screen type.
 * @param widgetConfigDirPath: the widget UI configuration directory path.
 * @return Execution result.
 */
T_DjiReturnCode DjiWidget_RegUiConfigByDirPath(E_DjiMobileAppLanguage appLanguage,
                                               E_DjiMobileAppScreenType appScreenType,
                                               const char *widgetConfigDirPath);

/**
 * @brief Register default widget UI config by binary array configuration.
 * @note In RTOS, most likely there is no file system. The widget config file content can use C array express. Use this
 * function and DjiWidget_RegDefaultUiConfigBinaryArray set widget UI configuration. When the language and screen size
 * is not cover in your setting by DjiWidget_RegUiConfigByBinaryArray, the widget UI configuration uses setting by this
 * function.
 * @param binaryArrayConfig: the binary array config for widget UI configuration.
 * @return Execution result.
 */
T_DjiReturnCode DjiWidget_RegDefaultUiConfigByBinaryArray(const T_DjiWidgetBinaryArrayConfig *binaryArrayConfig);

/**
 * @brief Register widget UI config by binary array configuration.
 * @note Different widget UI configurations for several language and screen size require the same widget type, index and
 * count.
 * @param appLanguage: mobile app language type.
 * @param screenType: mobile app screen type.
 * @param binaryArrayConfig: the binary array config for widget UI configuration.
 * @return Execution result.
 */
T_DjiReturnCode DjiWidget_RegUiConfigByBinaryArray(E_DjiMobileAppLanguage appLanguage,
                                                   E_DjiMobileAppScreenType screenType,
                                                   const T_DjiWidgetBinaryArrayConfig *binaryArrayConfig);

/**
 * @brief Register handler list for widgets.
 * @param widgetHandlerList: widget handler list for widgets.
 * @param itemCount: the item count of widget handler list.
 * @return Execution result.
 */
T_DjiReturnCode DjiWidget_RegHandlerList(const T_DjiWidgetHandlerListItem *widgetHandlerList, uint32_t itemCount);

/**
 * @brief Send message to mobile app floating window.
 * @note the message length can't more than DJI_WIDGET_FLOATING_WINDOW_MSG_MAX_LEN. The max data bandwidth of floating
 * windows message is 2KB/s.
 * @param str: pointer to message string.
 * @return Execution result.
 */
T_DjiReturnCode DjiWidgetFloatingWindow_ShowMessage(const char *str);

/**
 * @brief Get data transmission state of floating window channel. User can use the state as base for controlling
 * floating windows message send.
 * @param state: pointer to floating window channel state.
 * @return Execution result.
 */
T_DjiReturnCode DjiWidgetFloatingWindow_GetChannelState(T_DjiDataChannelState *state);

/**
 * @brief Register the handler for widget speaker function interfaces.
 * @note This interface registers the widget speaker function interface, including speaker settings, play operation,
 * speaker status interface.
 * @param widgetSpeakerHandler: pointer to the handler for widget speaker functions.
 * @return Execution result.
 */
T_DjiReturnCode DjiWidget_RegSpeakerHandler(const T_DjiWidgetSpeakerHandler *widgetSpeakerHandler);

#ifdef __cplusplus
}
#endif

#endif // DJI_WIDGET_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
