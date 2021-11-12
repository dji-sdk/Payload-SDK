/**
 ********************************************************************
 * @file    test_upgrade_common_file_transfer.h
 * @brief   This is the header file for "test_upgrade_common_file_transfer.c", defining the structure and
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
#ifndef TEST_UPGRADE_COMMON_FILE_TRANSFER_H
#define TEST_UPGRADE_COMMON_FILE_TRANSFER_H

/* Includes ------------------------------------------------------------------*/
#include <dji_upgrade.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
T_DjiReturnCode DjiTestCommonFileTransfer_Start(const T_DjiUpgradeFileInfo *fileInfo);
T_DjiReturnCode DjiTestCommonFileTransfer_Transfer(const uint8_t *data, uint16_t dataLen);
T_DjiReturnCode DjiTestCommonFileTransfer_Finish(const uint8_t md5[DJI_MD5_BUFFER_LEN]);

#ifdef __cplusplus
}
#endif

#endif // TEST_UPGRADE_COMMON_FILE_TRANSFER_LINUX_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
