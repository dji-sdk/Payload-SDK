/**
 ********************************************************************
 * @file    test_mop_channel.h
 * @version V2.0.0
 * @date    1/14/20
 * @brief   This is the header file for "test_mop_channel.c", defining the structure and
 * (exported) function prototypes.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef TEST_MOP_CHANNEL_H
#define TEST_MOP_CHANNEL_H

/* Includes ------------------------------------------------------------------*/
#include "psdk_typedef.h"

#if PSDK_ARCH_SYS_LINUX

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef enum {
    PSDK_MOP_CHANNEL_FILE_TRANSFOR_CMD_REQUEST = 0x50,
    PSDK_MOP_CHANNEL_FILE_TRANSFOR_CMD_ACK = 0x51,
    PSDK_MOP_CHANNEL_FILE_TRANSFOR_CMD_RESULT = 0x52,
    PSDK_MOP_CHANNEL_FILE_TRANSFOR_CMD_FILE_INFO = 0x60,
    PSDK_MOP_CHANNEL_FILE_TRANSFOR_CMD_FILE_DOWNLOAD_REQ = 0x61,
    PSDK_MOP_CHANNEL_FILE_TRANSFOR_CMD_FILE_DATA = 0x62,
    PSDK_MOP_CHANNEL_FILE_TRANSFOR_CMD_STOP_REQUEST = 0x63,
    PSDK_MOP_CHANNEL_FILE_TRANSFOR_CMD_STOP_ACK = 0x64,
} E_PsdkMopChannel_FileTransforCmd;

typedef enum {
    PSDK_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_REQUEST_UPLOAD = 0x00,
    PSDK_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_REQUEST_DOWNLOAD = 0x01,
} E_PsdkMopChannel_FileTransforRequestSubCmd;

typedef enum {
    PSDK_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_ACK_OK = 0x00,
    PSDK_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_ACK_REJECTED = 0x01,
} E_PsdkMopChannel_FileTransforAckSubCmd;

typedef enum {
    PSDK_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_RESULT_OK = 0x00,
    PSDK_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_RESULT_FAILED = 0x01,
} E_PsdkMopChannel_FileTransforResultSubCmd;

typedef enum {
    PSDK_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_FILE_INFO_DEFAULT = 0xFF,
} E_PsdkMopChannel_FileTransforFileInfoSubCmd;

typedef enum {
    PSDK_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_DOWNLOAD_REQUEST = 0xFF,
} E_PsdkMopChannel_FileTransforFileDownloadRequestSubCmd;

typedef enum {
    PSDK_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_FILE_DATA_NORMAL = 0x00,
    PSDK_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_FILE_DATA_END = 0x01,
} E_PsdkMopChannel_FileTransforFileDataSubCmd;

typedef enum {
    PSDK_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_STOP_UPLOAD = 0x00,
    PSDK_MOP_CHANNEL_FILE_TRANSFOR_SUBCMD_STOP_DOWNLOAD = 0x01,
} E_PsdkMopChannel_FileTransforStopSubCmd;

#pragma pack(1)

typedef struct {
    bool isExist;
    uint32_t fileLength;
    char fileName[32];
    uint8_t md5Buf[16];
} T_PsdkMopChannel_FileInfo;

typedef struct {
    char fileName[32];
} T_PsdkMopChannel_DwonloadReq;

typedef struct {
    uint8_t cmd;
    uint8_t subcmd;
    uint16_t seqNum;
    uint32_t dataLen;
    union dataType {
        T_PsdkMopChannel_FileInfo fileInfo;
        T_PsdkMopChannel_DwonloadReq dwonloadReq;
        uint8_t fileData[0];
    } data;
} T_PsdkMopChannel_FileTransfor;

#pragma pack()

/* Exported functions --------------------------------------------------------*/
T_PsdkReturnCode PsdkTest_MopChannelInit(void);


#ifdef __cplusplus
}
#endif

#endif

#endif // TEST_MOP_CHANNEL_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
