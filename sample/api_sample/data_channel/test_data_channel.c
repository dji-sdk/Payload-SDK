/**
 ********************************************************************
 * @file    test_data_channel.c
 * @version V2.0.0
 * @date    2019/9/30
 * @brief
 *
 * @copyright (c) 2018-2019 DJI. All rights reserved.
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
#include "test_data_channel.h"
#include "psdk_data_channel.h"
#include "psdk_logger.h"

/* Private constants ---------------------------------------------------------*/


/* Private types -------------------------------------------------------------*/


/* Private functions declaration ---------------------------------------------*/


/* Private variables ---------------------------------------------------------*/


/* Exported functions definition ---------------------------------------------*/
T_PsdkReturnCode PsdkTest_DataChannelSetBandwidthProportionForHighspeedChannel(
    T_PsdkDataChannelBandwidthProportionOfHighspeedChannel bandwidthProportion)
{
    T_PsdkReturnCode psdkStat;

    psdkStat = PsdkDataChannel_SetBandwidthProportionForHighspeedChannel(bandwidthProportion);
    if (psdkStat != PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        PsdkLogger_UserLogError("set bandwidth proportion for high-speed channel error.");
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/* Private functions definition-----------------------------------------------*/


/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
