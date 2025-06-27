/**
 ********************************************************************
 * @file    test_widget_manager.cpp
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
#include <iostream>
#include "test_widget_manager.hpp"
#include "dji_widget_manager.h"
#include <dji_logger.h>
#include <dji_platform.h>
#include "unistd.h"
#include "utils/util_misc.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
/* Private constants ---------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private values -------------------------------------------------------------*/
T_DjiWidgetManagerFileList s_fileList;
static FILE* s_widgetFileFd;
/* Private functions declaration ---------------------------------------------*/
static void DjiTestWidgetManager_RecvWidgetStatesCallback(E_DjiMountPosition position, T_DjiWidgetStates *statesData, uint8_t widgetNum);
static void DjiTestWidgetManager_RecvSpeakerStatesCallback(E_DjiMountPosition position, T_DjiSpeakerWidgetStates *speakerStates);
static void DjiTestWidgetManager_RunSeachLightManagerSample(E_DjiMountPosition position);
static void  DjiTestWidgetManager_RunSpeakerManagerSample(E_DjiMountPosition position);
static T_DjiReturnCode  DjiWidgetManager_UsrDownloadCallback(T_DjiDownloadWidgetFileInfo packetInfo,
                                                                    const uint8_t *data,
                                                                    uint16_t dataLen);
/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode DjiTest_WidgetMannagerStart(void)
{
    T_DjiReturnCode djiStat;
    E_DjiMountPosition position;
    uint8_t index;
    uint32_t value;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    T_DjiWidgetStates state;
    int widget_type_input;
    int inputPosition = 0;
    int inputIndex = 0;
    bool continueFlag = true;
    int payloadType;
    T_DjiWidgetManagerFileList fileList;

    std::cout << "Please select the position where the payload needs to be managed." << std::endl;
    std::cin >> inputPosition;
    std::cout << "Please select load type (1: third party payload; 2: DJI searchlight; 3: DJI speaker) ";
    std::cin >>payloadType;
    if (inputPosition < 1 || inputPosition > 8) {
        std::cerr << "Invalid input for position. Must be between 1 and 8." << std::endl;
        return -1;
    }

    if (payloadType < 1 || payloadType > 3) {
        std::cerr << "Invalid input for payload type. Must be between 1 and 3." << std::endl;
        return -1;
    }

    position = static_cast<E_DjiMountPosition>(inputPosition);

    djiStat = DjiWidgetManager_Init();
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("Dji test widget manager init error, stat = 0x%08llX", djiStat);
        return djiStat;
    }

    djiStat = DjiWidgetManager_RegDownloadFileDataCallback(position, DjiWidgetManager_UsrDownloadCallback);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("Dji test regist download callback on position:%d error, stat = 0x%08llX", position, djiStat);
        goto finish_management;
    }

    switch(payloadType){
        case 1:
            djiStat = DjiWidgetManager_WidgetDownloadFileList(position, &s_fileList);
            if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
            {
                USER_LOG_ERROR("Dji test download payload widget file list on position:%d error, stat = 0x%08llX", position, djiStat);
                goto finish_management;
            }

            for(int i = 0; i < s_fileList.totalCount; i++) {
                djiStat = DjiWidgetManager_DownloadFileByIndex(position, i);
                if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
                {
                    USER_LOG_ERROR("Dji test download payload widget by index on position:%d error, stat = 0x%08llX", position, djiStat);
                    goto finish_management;
                }
            }

            djiStat = DjiWidgetManager_SubscribePayloadWidgetStates(position, DjiTestWidgetManager_RecvWidgetStatesCallback);
            if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
            {
                USER_LOG_ERROR("Dji test subscribe payload widget state on position:%d error, stat = 0x%08llX", position, djiStat);
                goto finish_management;
            }
             break;
        case 2:
            djiStat = DjiWidgetManager_WidgetDownloadFileList(position, &s_fileList);
            if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
            {
                USER_LOG_ERROR("Dji test download payload widget file list on position:%d error, stat = 0x%08llX", position, djiStat);
                goto finish_management;
            }

            for(int i = 0; i < s_fileList.totalCount; i++) {
                djiStat = DjiWidgetManager_DownloadFileByIndex(position, i);
                if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
                {
                    USER_LOG_ERROR("Dji test download payload widget by index on position:%d error, stat = 0x%08llX", position, djiStat);
                    goto finish_management;
                }
            }

            DjiTestWidgetManager_RunSeachLightManagerSample(position);
            goto finish_management;
        case 3:
            DjiTestWidgetManager_RunSpeakerManagerSample(position);
            goto finish_management;
    }

set_value:
    std::cout << "If you need to change the widget state, enter 1 " << std::endl;
    std::cin >> continueFlag;
    if(continueFlag != 1) goto finish_management;

    std::cout << "Please enter the index and type to be set and the value to be set to :" << std::endl;
    std::cin >> inputIndex >> widget_type_input >> state.widgetValue;

    if (widget_type_input >= 1 && widget_type_input <= 5)
    {
        state.widgetType = static_cast<E_DjiWidgetType>(widget_type_input);
        state.widgetIndex = inputIndex;
    }
    else
    {
        std::cout << "Invalid widget type input." << std::endl;
        goto finish_management;
    }

    djiStat = DjiWidgetManager_SetWidgetState(position, state);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("Dji test set payload widget state on position: error, stat = 0x%08llX", position, djiStat);
        goto finish_management;
    }

    goto set_value;

finish_management:
    djiStat = DjiWidgetManager_DeInit();
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("Dji test deinit widget manager error, stat = 0x%08llX", djiStat);
        return djiStat;
    }

    if(s_fileList.fileListInfo != NULL) {
        free(s_fileList.fileListInfo);
        s_fileList.fileListInfo = NULL;
    }
    return djiStat;
}

/* Private functions definition-----------------------------------------------*/
static void DjiTestWidgetManager_RecvWidgetStatesCallback(E_DjiMountPosition position, T_DjiWidgetStates *statesData, uint8_t widgetNum)
{
    printf("\033[32mrecv widget state form pos: %u\n", position);
    for (int i = 0; i < widgetNum; i++)
    {
        printf("widget index: %u type: %u value: %u\n",
               statesData[i].widgetIndex,
               static_cast<unsigned int>(statesData[i].widgetType),
               statesData[i].widgetValue);
    }
    printf("\033[0m");
}

static void DjiTestWidgetManager_RecvSpeakerStatesCallback(E_DjiMountPosition position, T_DjiSpeakerWidgetStates *speakerStates) {
    printf("\033[0;34mrecv speaker state form pos: %u\n", position);
    printf("speaker playMode = %d\n", speakerStates->playMode);
    printf("speaker workMode = %d\n", speakerStates->workMode);
    printf("speaker playVloume = %u\n", speakerStates->playVloume);
    printf("speaker systemStates = %d\n", speakerStates->systemStates);
    printf("speaker playFileUuid = %s\n", speakerStates->playFileUuid);
    printf("speaker playFileName = %s\n", speakerStates->playFileName);
    printf("speaker playQuality = %u\n", speakerStates->playFileName);
    printf("speaker actualVolume = %u\n", speakerStates->actualVolume);
    printf("speaker limitVolumeOnTheGround = %u\n", speakerStates->limitVolumeOnTheGround);
    printf("\033[0m");
}

void DjiTestWidgetManager_RunSeachLightManagerSample(E_DjiMountPosition position) {
    T_DjiReturnCode djiStat;
    T_DjiWidgetStates state;

    djiStat = DjiWidgetManager_SubscribePayloadWidgetStates(position, DjiTestWidgetManager_RecvWidgetStatesCallback);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("Dji test subscribe search light widget state on position:%d error, stat = 0x%08llX", position, djiStat);
        // return ;
    }

    std::cout<<"step 1: Turn on the searchlight."<<std::endl;
    state = {DJI_WIDGET_TYPE_SWITCH, 3, 1};
    djiStat = DjiWidgetManager_SetWidgetState(position, state);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("Dji test turn on search light widget on position :%d error, stat = 0x%08llX", position, djiStat);
        return ;
    }
    sleep(5);

    std::cout<<"step 2: Setting the searchlight to burst mode"<<std::endl;
    state = {DJI_WIDGET_TYPE_SWITCH, 0, 1};
    djiStat = DjiWidgetManager_SetWidgetState(position, state);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("Dji test setting the searchlight to burst mode on position:%d error, stat = 0x%08llX", position, djiStat);
        return ;
    }
    sleep(5);

    state = {DJI_WIDGET_TYPE_SWITCH, 0, 0};
    djiStat = DjiWidgetManager_SetWidgetState(position, state);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("Dji test setting  up searchlight to roll out burst mode on position:%d error, stat = 0x%08llX", position, djiStat);
        return ;
    }
    sleep(5);

    std::cout<<"step3: Switching the searchlight illumination mode"<<std::endl;
    state = {DJI_WIDGET_TYPE_LIST, 0, 0};
    djiStat = DjiWidgetManager_SetWidgetState(position, state);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("Dji test switching the searchlight illumination mode on position:%d error, stat = 0x%08llX", position, djiStat);
        return ;
    }
    sleep(5);

    state = {DJI_WIDGET_TYPE_SWITCH, 2, 1};
    djiStat = DjiWidgetManager_SetWidgetState(position, state);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("Dji test turn off the searchlight on position:%d error, stat = 0x%08llX", position, djiStat);
        return ;
    }
    sleep(5);

    state = {DJI_WIDGET_TYPE_SWITCH, 2, 2};
    djiStat = DjiWidgetManager_SetWidgetState(position, state);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("Dji test turn off the searchlight on position:%d error, stat = 0x%08llX", position, djiStat);
        return ;
    }
    sleep(5);

    std::cout<<"step4: Turn off the searchlight."<<std::endl;
    state = {DJI_WIDGET_TYPE_SWITCH, 3, 0};
    djiStat = DjiWidgetManager_SetWidgetState(position, state);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("Dji test turn off the searchlight on position:%d error, stat = 0x%08llX", position, djiStat);
        return ;
    }

    djiStat = DjiWidgetManager_UnsubscribePayloadWidgetStates(position);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("Dji test unsubscribe search light widget state on position:%d error, stat = 0x%08llX", position, djiStat);
        // return ;
    }
}

static void  DjiTestWidgetManager_RunSpeakerManagerSample(E_DjiMountPosition position) {
    T_DjiReturnCode djiStat;
    T_DjiSpeakerWidgetStatesParam param;
    T_DjiSpeakerAudioFileInfo audioFileInfo;
    char curFileDirPath[WIDGET_MANAGER_MAX_FILE_PATH_LEN];
    char filePath[WIDGET_MANAGER_MAX_FILE_PATH_LEN];

    djiStat = DjiUserUtil_GetCurrentFileDirPath(__FILE__, WIDGET_MANAGER_MAX_FILE_PATH_LEN, curFileDirPath);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Get file current path error, stat = 0x%08llX", djiStat);
        return ;
    }
    snprintf(filePath, WIDGET_MANAGER_MAX_FILE_PATH_LEN, "%sdata/3K-5K.opus", curFileDirPath);
    const char* fileName = "3K-5K.opus";
    const char* uuid = "abcdef";

    memset(&audioFileInfo, 0, sizeof(audioFileInfo));
    audioFileInfo.fileType = DJI_SPEAKER_WIDGET_FILE_TYPE_OPUS;
    memcpy(audioFileInfo.fileName, fileName, strlen(fileName));
    memcpy(audioFileInfo.filePath, filePath, strlen(filePath));
    memcpy(audioFileInfo.uuid, uuid, strlen(uuid));
    audioFileInfo.fileBitrate = DJI_SPEAKER_WIDGET_AUDIO_DECODE_BITRATE_32000;

    std::cout<<"step 1: subscribe widget state of speaker"<<std::endl;
    djiStat = DjiWidgetManager_SubscribeSpeakerStates(position, DjiTestWidgetManager_RecvSpeakerStatesCallback);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("Dji test subscribe speaker widget state on position:%d error, stat = 0x%08llX", position, djiStat);
        // return ;
    }

    std::cout<<"step 2: set playing param of speaker"<<std::endl;
    param = {DJI_WIDGET_MGR_SPEAKER_PARAM_VOLUME, 20};
    djiStat = DjiWidgetManager_SetSpeakertState(position, &param);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("Dji test setting the speaker volume on position:%d error, stat = 0x%08llX", position, djiStat);
        return ;
    }

    param = {DJI_WIDGET_MGR_SPEAKER_PARAM_PLAY_MODE, 1};
    djiStat = DjiWidgetManager_SetSpeakertState(position, &param);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("Dji test set the speaker play mode on position:%d error, stat = 0x%08llX", position, djiStat);
        return ;
    }

    std::cout<<"step 3: send audio file data to speaker"<<std::endl;
    djiStat = DjiWidgetManager_SendSpeakerAudioData(position, &audioFileInfo);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("Dji test send audio file  to speaker on position:%d error, stat = 0x%08llX", position, djiStat);
        return ;
    }


    std::cout<<"step 4: play the audio file"<<std::endl;
    param = {DJI_WIDGET_MGR_SPEAKER_PARAM_PALY_ACTION, 0};
    djiStat = DjiWidgetManager_SetSpeakertState(position, &param);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("Dji test set the speaker to start play on position:%d error, stat = 0x%08llX", position, djiStat);
        return ;
    }

    sleep(10);

    param = {DJI_WIDGET_MGR_SPEAKER_PARAM_PALY_ACTION, 1};
    djiStat = DjiWidgetManager_SetSpeakertState(position, &param);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("Dji test set the speaker to stop play on position:%d error, stat = 0x%08llX", position, djiStat);
        return ;
    }

    std::cout<<"step 5: stop subscribe speaker state"<<std::endl;
    djiStat = DjiWidgetManager_UnsubscribeSpeakerStates(position);
    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("Dji test unsubscribe speaker widget state on position:%d error, stat = 0x%08llX", position, djiStat);
        // return ;
    }

}

static T_DjiReturnCode DjiWidgetManager_UsrDownloadCallback(T_DjiDownloadWidgetFileInfo packetInfo,
                                                            const uint8_t *data,
                                                            uint16_t dataLen) {
    std::string directory;
    std::string fileName = s_fileList.fileListInfo[packetInfo.fileIndex].fileName;
    std::string path;
    directory = "widget_file_from_position_" + std::to_string(packetInfo.position) ;
    path = directory + '/' + fileName;
    size_t realLen;

    if (mkdir(directory.c_str(), 0755) != 0 && errno != EEXIST) {
        USER_LOG_ERROR("Error creating directory: %s\n", strerror(errno));
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    switch (packetInfo.downloadFileEvent) {
        case DJI_DOWNLOAD_FILE_EVENT_START: {
            s_widgetFileFd = fopen(path.c_str(), "wb+");
            if(s_widgetFileFd == NULL) {
                USER_LOG_ERROR("Dji test Open widget file failed");
                return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
            }
            realLen = fwrite(data, 1, dataLen, s_widgetFileFd);
            if(realLen != dataLen) {
                USER_LOG_ERROR("Dji test write widget data failed");
                return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
            }
            break;
        }
        case DJI_DOWNLOAD_FILE_EVENT_TRANSFER: {
            if(s_widgetFileFd != NULL) {
                realLen = fwrite(data, 1, dataLen, s_widgetFileFd);
                if(realLen != dataLen) {
                    USER_LOG_ERROR("Dji test write widget data failed");
                    return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
                }
            }
            break;
        }
        case DJI_DOWNLOAD_FILE_EVENT_END: {
            if(s_widgetFileFd != NULL) {
                realLen = fwrite(data, 1, dataLen, s_widgetFileFd);
                if(realLen != dataLen) {
                    USER_LOG_ERROR("Dji test write widget data failed");
                    return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
                }
            }
            fclose(s_widgetFileFd);
            s_widgetFileFd = NULL;
            break;
        }
        case DJI_DOWNLOAD_FILE_EVENT_START_TRANSFER_END: {
            s_widgetFileFd = fopen(path.c_str(), "wb+");
            if(s_widgetFileFd == NULL) {
                USER_LOG_ERROR("Dji test Open widget file failed");
                return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
            }
            realLen = fwrite(data, 1, dataLen, s_widgetFileFd);
            if(realLen != dataLen) {
                USER_LOG_ERROR("Dji test write widget data failed");
                return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
            }
            fclose(s_widgetFileFd);
            s_widgetFileFd = NULL;
            break;
        }
        default: {
            USER_LOG_ERROR("Dji test unknown download event");
            break;
        }
    }
    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}
/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
