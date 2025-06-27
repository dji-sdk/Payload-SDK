/**
 ********************************************************************
 * @file    dji_widget_states_manager.h
 * @version V1.0.0
 * @date    2020/11/26
 * @brief   This is the header file for "dji_widget_manager.h", defining the structure and
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
#ifndef DJI_WIDGET_STATES_MANNAGER_H
#define DJI_WIDGET_STATES_MANNAGER_H

/* Includes ------------------------------------------------------------------*/
#include "dji_typedef.h"
#include "dji_widget.h"
#include "dji_camera_manager.h"
// #include "downloader/dji_download_file.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/
#define WIDGET_MANAGER_MAX_FILE_NAME_LEN  32
#define WIDGET_MANAGER_MAX_FILE_PATH_LEN  128

/* Exported types ------------------------------------------------------------*/
/**
 * @brief Speaker system states.
 */
typedef enum {
    DJI_WIDGET_MGR_SPEAKER_SYSTEM_STATE_IDEL = 0,
    DJI_WIDGET_MGR_SPEAKER_SYSTEM_STATE_IN_TRANSMISSION = 1,
    DJI_WIDGET_MGR_SPEAKER_SYSTEM_STATE_PLAYING = 2,
    DJI_WIDGET_MGR_SPEAKER_SYSTEM_STATE_EXCEPTION = 3,
    DJI_WIDGET_MGR_SPEAKER_SYSTEM_STATE_IN_TTS_CONVERSION = 4,
} E_DjiWidgetManagerSpeakerStates;

/**
 * @brief Speaker parameters.
 */
typedef enum {
    DJI_WIDGET_MGR_SPEAKER_PARAM_WORK_MODE = 0,     /*!< The working mode fo the speaker */
    DJI_WIDGET_MGR_SPEAKER_PARAM_PALY_ACTION = 1,   /*!< The playing action of the speaker */
    DJI_WIDGET_MGR_SPEAKER_PARAM_VOLUME = 2,        /*!< The volume action of the speaker */
    DJI_WIDGET_MGR_SPEAKER_PARAM_PLAY_MODE = 3,     /*!< The playing mode of the speaker */
    DJI_WIDGET_MGR_SPEAKER_PARAM_PLAY_FILE_NAME = 4, /*!<The name of the file to be played */
} E_DjiWidgetManagerSpeakerPram;

/**
 * @brief Speaker audio bitrate
 */
typedef enum {
    DJI_SPEAKER_WIDGET_AUDIO_DECODE_BITRATE_8000 = 1,
    DJI_SPEAKER_WIDGET_AUDIO_DECODE_BITRATE_16000 = 2,
    DJI_SPEAKER_WIDGET_AUDIO_DECODE_BITRATE_24000 = 3,
    DJI_SPEAKER_WIDGET_AUDIO_DECODE_BITRATE_32000 = 4,
    DJI_SPEAKER_WIDGET_AUDIO_DECODE_BITRATE_48000 = 5,
    DJI_SPEAKER_WIDGET_AUDIO_DECODE_BITRATE_64000 = 6,
} E_DjiWidgetManagerSpeakerAudoBitrate;

/**
 * @brief The type of audio file
 */
typedef enum
{
	DJI_SPEAKER_WIDGET_FILE_TYPE_TTS_TXT = 0,
	DJI_SPEAKER_WIDGET_FILE_TYPE_OPUS = 1,
	DJI_SPEAKER_WIDGET_FILE_TYPE_WAV = 2,
} E_DjiWidgetManagerSpeakerFileType;

/**
 * @brief The states of widget.
 */
typedef struct{
    E_DjiWidgetType widgetType;   /*!< The type fo the widget */
    uint8_t widgetIndex;          /*!< The index fo the widget */
    int32_t widgetValue;          /*!< The value fo the widget */
} T_DjiWidgetStates;

/**
 * @brief The information of audio file.
 */
typedef struct{
    E_DjiWidgetManagerSpeakerFileType fileType;             /*!< The type fo the audio file */
    E_DjiWidgetManagerSpeakerAudoBitrate fileBitrate;       /*!< The bitrate fo the audio file */
    uint8_t uuid[WIDGET_MANAGER_MAX_FILE_NAME_LEN];         /*!< The uuid fo the audio file */
    uint8_t fileName[WIDGET_MANAGER_MAX_FILE_NAME_LEN];     /*!< The fileName fo the audio file */
    uint8_t filePath[WIDGET_MANAGER_MAX_FILE_PATH_LEN];     /*!< The absolute path to the audio file */
} T_DjiSpeakerAudioFileInfo;

/**
 * @brief The states of the speaker's widget
 */
typedef struct {
    E_DjiWidgetSpeakerWorkMode workMode;                    /*!< The working mode of the speaker */
    E_DjiWidgetSpeakerPlayMode playMode;                    /*!< The playing mode of the speaker */
    uint8_t playVloume;                                     /*!< The vloume of the speaker */
    E_DjiWidgetManagerSpeakerStates systemStates;	            /*!< The system states of the speaker */
    uint8_t playFileUuid[WIDGET_MANAGER_MAX_FILE_NAME_LEN]; /*!< The uuid of the playing file */
    uint8_t playFileName[WIDGET_MANAGER_MAX_FILE_NAME_LEN]; /*!< The name of the playing file */
    uint8_t playQuality;                                    /*!< packet loss per second [0-255] mapped to 0%-100% */
    uint8_t actualVolume;                                   /*!< The actual volume of the speaker */
    uint8_t limitVolumeOnTheGround;                         /*!< The limit volume when aircraft is on the ground */
} T_DjiSpeakerWidgetStates;

/**
 * @brief Speaker parameters to be set
 */
typedef struct {
    E_DjiWidgetManagerSpeakerPram paramType;    /*!< Type of speaker parameter to set */
    union {
       uint8_t value;
       uint8_t fileName[32];
    }data;
} T_DjiSpeakerWidgetStatesParam;

/**
 * @brief Widget file information
 */
typedef struct {
    uint32_t fileSize;
    uint32_t fileIndex;
    char fileName[DJI_FILE_NAME_SIZE_MAX];
} T_DjiWidgetManagerFileInfo;

/**
 * @brief The list of widget file information
 */
typedef struct{
    uint8_t totalCount;
    T_DjiWidgetManagerFileInfo *fileListInfo;
} T_DjiWidgetManagerFileList;

/**
 * @brief Widget file download process information
 */
typedef struct {
    E_DjiDownloadFileEvent downloadFileEvent;   /*!< Download Event Type */
    E_DjiMountPosition position;                /*!< The position of the payload of the downloaded file */
    uint32_t fileIndex;
    uint32_t fileSize;
    dji_f32_t progressInPercent;
} T_DjiDownloadWidgetFileInfo;

/**
 * @brief Type of callback function to receive widget states.
 * @param position: The position of the payload from which the widget states comes.
 * @param statesData: widgets states of widgets.
 * @param widgetNum: Number of widgets.
 * @note It is not recommended to perform blocking operations in the callback, as it may lead to data loss.
 * @return Execution result.
 */
typedef void (*RecvWidgetStatesCallback)(E_DjiMountPosition position, T_DjiWidgetStates *statesData, uint8_t widgetNum);

/**
 * @brief Type of callback function to receive speaker widget states.
 * @param position: The position of the speaker from which the widget states comes.
 * @param statesData: widgets states of speaker.
 * @note It is not recommended to perform blocking operations in the callback, as it may lead to data loss.
 * @return Execution result.
 */
typedef void (*RecvSpeakerStatesCallback)(E_DjiMountPosition position, T_DjiSpeakerWidgetStates *speakerStates);

/**
 * @brief Types of callback functions that handle downloading widget file data.
 * @param packetInfo: Widget file download process information.
 * @param data: Download raw data.
 * @param len: length of data.
 * @note It is not recommended to perform blocking operations in the callback, as it may lead to data loss.
 * @return Execution result.
 */
typedef T_DjiReturnCode (*DjiWidgetDownloadFileDataCallback)(T_DjiDownloadWidgetFileInfo packetInfo,
                                                                    const uint8_t *data,
                                                                    uint16_t dataLen);
/* Exported functions --------------------------------------------------------*/
/**
 * @brief Initialize the widget manager module.
 * @note The interface initialization needs to be after DjiCore_Init.
 * @return Execution result.
 */
T_DjiReturnCode DjiWidgetManager_Init();

/**
 * @brief Denitialize the widget manager module.
 * @return Execution result.
 */
T_DjiReturnCode DjiWidgetManager_DeInit();

/**
 * @brief Subscribe to the widget states of a Payload.
 * @param position: the position of the target payload.
 * @param recvStatesCallback: the callback function to recv the widget states.
 * @note Use this interface to be notified when the states of the target payload's widget changes.
 * @return Execution result.
 */
T_DjiReturnCode DjiWidgetManager_SubscribePayloadWidgetStates(E_DjiMountPosition position,
                                                            RecvWidgetStatesCallback  recvStatesCallback);

/**
 * @brief Unsubscribe to the widget states of a Payload.
 * @param position: the position of the target payload.
 * @return Execution result.
 */
T_DjiReturnCode DjiWidgetManager_UnsubscribePayloadWidgetStates(E_DjiMountPosition position);

/**
 * @brief Set the widge state of the target payload.
 * @param position: the position of the target payload.
 * @param states: Widget parameters to be set.
 * @return Execution result.
 */
T_DjiReturnCode DjiWidgetManager_SetWidgetState(E_DjiMountPosition position, T_DjiWidgetStates states);

/**
 * @brief Subscribe to the widget states of a speaker.
 * @param position: the position of the target speaker.
 * @param recvStatesCallback: the callback function to recv the widget states.
 * @note Use this interface to be notified when the states of the target speaker's widget changes.
 * @return Execution result.
 */
T_DjiReturnCode DjiWidgetManager_SubscribeSpeakerStates(E_DjiMountPosition position,
                                                        RecvSpeakerStatesCallback recvStatesCallback);

/**
 * @brief Unsubscribe to the widget states of a speaker.
 * @param position: the position of the target speaker.
 * @return Execution result.
 */
T_DjiReturnCode DjiWidgetManager_UnsubscribeSpeakerStates(E_DjiMountPosition position);

/**
 * @brief Set the widge state of the target speaker.
 * @param position: the position of the target payload.
 * @param states: Widget parameters to be set.
 * @return Execution result.
 */
T_DjiReturnCode DjiWidgetManager_SetSpeakertState(E_DjiMountPosition position,
                                                T_DjiSpeakerWidgetStatesParam *states);

/**
 * @brief Transferring audio files to speaker.
 * @param position: the position of the target speaker.
 * @param audioFileInfo: Audio file description information.
 * @return Execution result.
 */
T_DjiReturnCode DjiWidgetManager_SendSpeakerAudioData(E_DjiMountPosition position,
                                                    T_DjiSpeakerAudioFileInfo *audioFileInfo);

/**
 * @brief Download the list of widget files for the target payload
 * @param position: the position of the target speaker.
 * @param fileList: Information on the list of files obtained from the download.
 * @return Execution result.
 */
T_DjiReturnCode DjiWidgetManager_WidgetDownloadFileList(E_DjiMountPosition position,
                                                        T_DjiWidgetManagerFileList *fileList) ;

/**
 * @brief Registering callback functions for downloading widget files
 * @param position: The position of the target speaker.
 * @param recvFileCallback: The callback to receive the downloaded widget file.
 * @return Execution result.
 */
T_DjiReturnCode DjiWidgetManager_RegDownloadFileDataCallback(E_DjiMountPosition position,
                                                            DjiWidgetDownloadFileDataCallback recvFileCallback);

/**
 * @brief Unregistering callback functions for downloading widget files.
 * @param position: The position of the target speaker.
 * @return Execution result.
 */
T_DjiReturnCode DjiWidgetManager_UnRegDownloadFileDataCallback(E_DjiMountPosition position);

/**
 * @brief Download the corresponding widget file on the target payload according to the file index.
 * @param position: The position of the target speaker.
 * @param fileIndex: The index of the file that will be downloaded
 * @return Execution result.
 */
T_DjiReturnCode DjiWidgetManager_DownloadFileByIndex(E_DjiMountPosition position, uint32_t fileIndex);

#ifdef __cplusplus
}
#endif

#endif // DJI_WIDGET_STATES_MANNAGER_H

/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
