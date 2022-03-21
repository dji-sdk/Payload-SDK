/**
 ********************************************************************
 * @file    test_hms.c
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
#include "test_hms.h"
#include "dji_hms.h"
#include "dji_hms_info_table.h"
#include "dji_logger.h"
#include "dji_platform.h"
#include "dji_fc_subscription.h"

/* Private constants ---------------------------------------------------------*/
#define MAX_HMS_PRINT_COUNT 150
#define MAX_BUFFER_LEN 256
#define MIN_HMS_ERROR_LEVEL 0
#define MID_HMS_ERROR_LEVEL 3
#define MAX_HMS_ERROR_LEVEL 6
/* Private types -------------------------------------------------------------*/

/* Private values -------------------------------------------------------------*/
static const char *oldReplaceAlarmIdStr = "%alarmid";
static const char *oldReplaceIndexStr = "%index";
static const char *oldReplaceComponentIndexStr = "%component_index";

/* Private functions declaration ---------------------------------------------*/
static T_DjiReturnCode DjiTest_HmsInit(void);
static T_DjiReturnCode DjiTest_HmsDeInit(void);
static T_DjiFcSubscriptionFlightStatus DjiTest_GetValueOfFlightStatus(void);
static bool DjiTest_ReplaceStr(char *buffer, uint32_t bufferMaxLen, const char *target, const char *dest);
static bool DjiTest_MarchErrCodeInfoTable(T_DjiHmsInfoTable hmsInfoTable);
static T_DjiReturnCode DjiTest_HmsInfoCallback(T_DjiHmsInfoTable hmsInfoTable);

/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode DjiTest_HmsRunSample(void)
{
    T_DjiReturnCode returnCode;
    T_DjiOsalHandler *osalHandler;

    USER_LOG_INFO("Hms Sample Start");
    DjiTest_WidgetLogAppend("Hms Sample Start");

    USER_LOG_INFO("--> Step 1: Init hms sample");
    DjiTest_WidgetLogAppend("--> Step 1: Init hms sample");
    returnCode = DjiTest_HmsInit();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Hms sample init error, error code:0x%08llX", returnCode);
        goto out;
    }

    osalHandler = DjiPlatform_GetOsalHandler();
    USER_LOG_INFO("--> Step 2: Register callback function of push HMS information");
    DjiTest_WidgetLogAppend("--> Step 2: Register callback function of push HMS information");
    returnCode = DjiHms_RegHmsInfoCallback(DjiTest_HmsInfoCallback);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Register callback function of push HMS information failed, error code:0x%08llX", returnCode);
        goto out;
    }

    osalHandler->TaskSleepMs(10000);

out:
    USER_LOG_INFO("--> Step 3: Deinit hms sample");
    DjiTest_WidgetLogAppend("--> Step 3: Deinit hms sample");
    returnCode = DjiTest_HmsDeInit();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Hms sample deinit error, error code:0x%08llX", returnCode);
    }

    USER_LOG_INFO("Hms Sample End");
    DjiTest_WidgetLogAppend("Hms Sample End");

    return returnCode;
}

/* Private functions definition-----------------------------------------------*/
static T_DjiReturnCode DjiTest_HmsInit(void)
{
    T_DjiReturnCode returnCode;

    returnCode = DjiFcSubscription_Init();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Hms sample init data subscription module failed, error code:0x%08llX", returnCode);
        return returnCode;
    }

    /*! subscribe fc data */
    returnCode = DjiFcSubscription_SubscribeTopic(DJI_FC_SUBSCRIPTION_TOPIC_STATUS_FLIGHT,
                                                  DJI_DATA_SUBSCRIPTION_TOPIC_10_HZ,
                                                  NULL);

    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("HMS sample subscribe topic flight status error, error code:0x%08llX", returnCode);
        return returnCode;
    }

    return DjiHms_Init();
}

static T_DjiReturnCode DjiTest_HmsDeInit(void)
{
    T_DjiReturnCode returnCode;

    returnCode = DjiFcSubscription_DeInit();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Deinit data subscription module failed, error code:0x%08llX",
                       returnCode);
        return returnCode;
    }

    return DjiHms_DeInit();
}

static T_DjiFcSubscriptionFlightStatus DjiTest_GetValueOfFlightStatus(void)
{
    T_DjiReturnCode djiStat;
    T_DjiFcSubscriptionFlightStatus flightStatus;
    T_DjiDataTimestamp flightStatusTimestamp = {0};

    djiStat = DjiFcSubscription_GetLatestValueOfTopic(DJI_FC_SUBSCRIPTION_TOPIC_STATUS_FLIGHT,
                                                      (uint8_t *) &flightStatus,
                                                      sizeof(T_DjiFcSubscriptionFlightStatus),
                                                      &flightStatusTimestamp);

    if (djiStat != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Get value of topic flight status failed, error code:0x%08llX", djiStat);
        flightStatus = 0;
    } else {
        USER_LOG_DEBUG("Timestamp: millisecond %u microsecond %u.", flightStatusTimestamp.millisecond,
                       flightStatusTimestamp.microsecond);
        USER_LOG_DEBUG("Flight status: %d.", flightStatus);
    }

    return flightStatus;
}

static bool DjiTest_ReplaceStr(char *buffer, uint32_t bufferMaxLen, const char *target, const char *dest)
{
    char printBuffHeader[MAX_BUFFER_LEN] = {0};
    uint32_t printBuffHeaderCpyCnt = 0;
    char printBuffTail[MAX_BUFFER_LEN] = {0};
    uint32_t printBuffTailCpyCnt = 0;
    char *targetOffset = NULL;

    targetOffset = strstr(buffer, target);
    if (!targetOffset) {
        return false;
    }

    printBuffHeaderCpyCnt = targetOffset - buffer;
    if (printBuffHeaderCpyCnt > sizeof(printBuffHeader)) {
        printBuffHeaderCpyCnt = sizeof(printBuffHeader);
    }
    memcpy(printBuffHeader, buffer, printBuffHeaderCpyCnt);

    printBuffTailCpyCnt = strlen(targetOffset + strlen(target));
    if (printBuffTailCpyCnt > sizeof(printBuffTail)) {
        printBuffTailCpyCnt = sizeof(printBuffTail);
    }
    memcpy(printBuffTail, targetOffset + strlen(target), printBuffTailCpyCnt);

    snprintf(buffer, bufferMaxLen, "%s%s%s", printBuffHeader, dest, printBuffTail);
    return true;
}


static bool DjiTest_MarchErrCodeInfoTable(T_DjiHmsInfoTable hmsInfoTable)
{
    char alarmIdStr[20] = {0};
    char sensorIdStr[20] = {0};
    char componentIdStr[20] = {0};
    char printBuff[256] = {0};
    const char *originalAlarmInfo = NULL;
    uint8_t hmsCodeMatchFlag = 0;

    if (!hmsInfoTable.hmsInfo) {
        USER_LOG_ERROR("Hms info table is null");
        return false;
    }

    for (int i = 0; i < hmsInfoTable.hmsInfoNum; i++) {
        hmsCodeMatchFlag = 0;
        for (int j = 0; j < sizeof(hmsErrCodeInfoTbl) / sizeof(T_DjiHmsErrCodeInfo); j++) {
            if (hmsInfoTable.hmsInfo[i].errorCode == hmsErrCodeInfoTbl[j].alarmId) {
                hmsCodeMatchFlag = 1;
                snprintf(alarmIdStr, sizeof(alarmIdStr), "%u", hmsInfoTable.hmsInfo[i].errorCode);
                //note:sensor_idx:[0,5].In order to be consistent with the display of pilot, add one.
                snprintf(sensorIdStr, sizeof(sensorIdStr), "%d", hmsInfoTable.hmsInfo[i].componentIndex + 1);
                snprintf(componentIdStr, sizeof(componentIdStr), "0x%02X", hmsInfoTable.hmsInfo[i].componentIndex + 1);
                if (DjiTest_GetValueOfFlightStatus() == DJI_FC_SUBSCRIPTION_FLIGHT_STATUS_IN_AIR) {
                    originalAlarmInfo = hmsErrCodeInfoTbl[j].flyAlarmInfo;
                } else {
                    originalAlarmInfo = hmsErrCodeInfoTbl[j].groundAlarmInfo;
                }
                originalAlarmInfo = hmsErrCodeInfoTbl[j].groundAlarmInfo;
                if (strlen(originalAlarmInfo)) {
                    snprintf(printBuff, sizeof(printBuff), "%s", originalAlarmInfo);
                    DjiTest_ReplaceStr(printBuff, sizeof(printBuff), oldReplaceAlarmIdStr, alarmIdStr);
                    DjiTest_ReplaceStr(printBuff, sizeof(printBuff), oldReplaceIndexStr, sensorIdStr);
                    DjiTest_ReplaceStr(printBuff, sizeof(printBuff), oldReplaceComponentIndexStr, componentIdStr);
                    if (hmsInfoTable.hmsInfo[i].errorLevel > MIN_HMS_ERROR_LEVEL &&
                        hmsInfoTable.hmsInfo[i].errorLevel < MID_HMS_ERROR_LEVEL) {
                        USER_LOG_WARN("[ErrorCode:0x%2x] %s", hmsInfoTable.hmsInfo[i].errorCode, printBuff);
                    } else if (hmsInfoTable.hmsInfo[i].errorLevel >= MID_HMS_ERROR_LEVEL &&
                               hmsInfoTable.hmsInfo[i].errorLevel < MAX_HMS_ERROR_LEVEL) {
                        USER_LOG_ERROR("[ErrorCode:0x%2x] %s", hmsInfoTable.hmsInfo[i].errorCode, printBuff);
                    }
                }
            }
        }
        if (!hmsCodeMatchFlag) {
            USER_LOG_WARN("[ErrorCode:0x%2x] There are no matching documents in the current hmsErrCodeInfoTbl for now.",
                          hmsInfoTable.hmsInfo[i].errorCode);
        }
    }

    return true;
}

static T_DjiReturnCode DjiTest_HmsInfoCallback(T_DjiHmsInfoTable hmsInfoTable)
{
    if (!DjiTest_MarchErrCodeInfoTable(hmsInfoTable)) {
        USER_LOG_ERROR("March HMS Information failed.");
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    if (hmsInfoTable.hmsInfoNum == 0) {
        USER_LOG_INFO("All systems of drone are running well now.");
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
