/**
 ********************************************************************
 * @file    test_radar_entry.cpp
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
#include "test_radar_entry.hpp"
#include "dji_logger.h"
#include <iostream>
#include <ctime>
#include <chrono>
/* Private constants ---------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private values -------------------------------------------------------------*/

/* Private functions declaration ---------------------------------------------*/
static void DjiTest_PerceptionRadarCallback(E_DjiPerceptionRadarPosition radarPosition,
                                             uint8_t *radarDataBuffer, uint32_t bufferLen);
static float parseVelocity(uint16_t velocity);
static float parseBeamAngle(uint16_t beamAngle);
/* Exported functions definition ---------------------------------------------*/
void DjiUser_RunRadarDataSubscriptionSample(void) {
    int subscriptionDuration = 10;
    T_DjiReturnCode returnCode;
    char inputChar;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    E_DjiPerceptionRadarPosition curPosition = MAX_RADAR_NUM;
    returnCode = DjiPerception_Init();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("DjiPerception Init failed");
        return;
    }

inputAgain:
    std::cout
        << "| Available commands:                                          |"
        <<
        std::endl;
    std::cout
        << "| [d] Subscribe to downward millimeter wave radar data.        |"
        <<
        std::endl;
    std::cout
        << "| [u] Subscribe to upward millimeter wave radar data.          |"
        <<
        std::endl;
    std::cout
        << "| [f] Subscribe to forward millimeter wave radar data.         |"
        <<
        std::endl;
    std::cout
        << "| [b] Subscribe to backward millimeter wave radar data.        |"
        <<
        std::endl;
    std::cout
        << "| [l] Subscribe to leftward millimeter wave radar data.        |"
        <<
        std::endl;
    std::cout
        << "| [r] Subscribe to rightward millimeter wave radar data.       |"
        <<
        std::endl;
    std::cout
        << "| [q] quit                                                     |"
        <<
        std::endl;

    std::cin >> inputChar;
    switch (inputChar) {
        case 'd':
            USER_LOG_INFO("Subscribe to downward millimeter wave radar data");
            curPosition = RADAR_POSITION_DOWN;
            break;
        case 'u':
            USER_LOG_INFO("Subscribe to upward millimeter wave radar data.");
            curPosition = RADAR_POSITION_UP;
            break;
        case 'f':
            USER_LOG_INFO("Subscribe to forward millimeter wave radar data.");
            curPosition = RADAR_POSITION_FRONT;
            break;
        case 'b':
            USER_LOG_INFO("Subscribe to backward millimeter wave radar data.");
            curPosition = RADAR_POSITION_BACK;
            break;
        case 'l':
            USER_LOG_INFO("Subscribe to leftward millimeter wave radar data.");
            curPosition = RADAR_POSITION_LEFT;
            break;
        case 'r':
            USER_LOG_INFO("Subscribe to rightward millimeter wave radar data.");
            curPosition = RADAR_POSITION_RIGHT;
            break;
        case 'q':
            goto endOfSample;
        default:
            goto inputAgain;
    }

    returnCode = DjiPerception_SubscribeRadarData(curPosition, DjiTest_PerceptionRadarCallback);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Request to subscribe radar data failed");
        goto subscribeFailed;
    }

    osalHandler->TaskSleepMs(subscriptionDuration * 1000);

subscribeFailed:

    returnCode = DjiPerception_UnsubscribeRadarData(curPosition);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Request to unsubscribe Radar data failed");
    }
    goto inputAgain;

endOfSample:
    returnCode = DjiPerception_Deinit();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("DjiPerception DeInit failed");
        return;
    }
}

/* Private functions definition-----------------------------------------------*/
static void DjiTest_PerceptionRadarCallback(E_DjiPerceptionRadarPosition radarPosition,
                                             uint8_t *radarDataBuffer, uint32_t bufferLen) {
    T_DjiRadarDataFrame* radarData;
    radarData = (T_DjiRadarDataFrame*)radarDataBuffer;

    if (radarDataBuffer == nullptr || bufferLen == 0) {
        USER_LOG_ERROR("Invalid radar data: buffer=%p len=%u", radarDataBuffer, bufferLen);
        return;
    }

    USER_LOG_INFO("RadarData[pos:%d][len:%u][units:%u][pack:%u/%u]",
                  radarPosition,
                  bufferLen,
                  static_cast<unsigned>(radarData->headInfo.dataLen),
                  static_cast<unsigned>(radarData->headInfo.curPack),
                  static_cast<unsigned>(radarData->headInfo.packNum));

    for (uint32_t i = 0; i < radarData->headInfo.dataLen; ++i) {
        const T_DjiRadarCloudUnit* unit = &radarData->data[i];
        float azimuth = unit->azimuth / 1000.0f - 2*3.14;
        float elevation = unit->elevation / 1000.0f - 2*3.14;
        float radius = unit->radius / 100.0f;
        float energy = unit->ene / 100.0f;
        float velocity = parseVelocity(unit->base_info.velocity);
        uint8_t snr = unit->base_info.snr;
        float beamAngle = parseBeamAngle(unit->base_info.beamAngle);

        USER_LOG_INFO(
            "[Unit%d] Azimuth=%.3f(rad), Elevation=%.3f(rad), Radius=%.2f(m), Energy=%.2f, "
            "Velocity=%.2f(m/s), SNR=%u(dB), BeamAngle=%.2f(deg)",
            i, azimuth, elevation, radius, energy, velocity, snr, beamAngle);
    }
}

static float parseVelocity(uint16_t velocity) {
    return (velocity - 32767) / 100.0f;
}
static float parseBeamAngle(uint16_t beamAngle) {
    if (beamAngle <= 450) {
        return beamAngle / 10.0f;
    } else {
        return (beamAngle / 10.0f) - 90.0f;
    }
}
/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
