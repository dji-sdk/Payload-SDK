#ifndef TEST_MOP_CHANNEL_CLIENT_H
#define TEST_MOP_CHANNEL_CLIENT_H

#include <stdbool.h>
#include "dji_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------------------------------------------------------------------
 * File-Service frame format for psdk-psdk MOP transfer (ch=49153).
 * Must stay in sync with the corresponding definitions in
 * samples/sample_c/module_sample/mop_channel/test_mop_channel.h.
 * --------------------------------------------------------------------------- */
typedef enum {
    MOP_FILE_TRANSFOR_CMD_REQUEST   = 0x50,
    MOP_FILE_TRANSFOR_CMD_ACK       = 0x51,
    MOP_FILE_TRANSFOR_CMD_RESULT    = 0x52,
    MOP_FILE_TRANSFOR_CMD_FILE_INFO = 0x60,
    MOP_FILE_TRANSFOR_CMD_FILE_DATA = 0x62,
} E_MopClientFileTransforCmd;

typedef enum {
    MOP_FILE_TRANSFOR_SUBCMD_REQUEST_UPLOAD   = 0x00,
} E_MopClientFileTransforRequestSubCmd;

typedef enum {
    MOP_FILE_TRANSFOR_SUBCMD_ACK_OK           = 0x00,
    MOP_FILE_TRANSFOR_SUBCMD_ACK_REJECTED     = 0x01,
} E_MopClientFileTransforAckSubCmd;

typedef enum {
    MOP_FILE_TRANSFOR_SUBCMD_RESULT_OK        = 0x00,
    MOP_FILE_TRANSFOR_SUBCMD_RESULT_FAILED    = 0x01,
} E_MopClientFileTransforResultSubCmd;

typedef enum {
    MOP_FILE_TRANSFOR_SUBCMD_FILE_DATA_NORMAL = 0x00,
    MOP_FILE_TRANSFOR_SUBCMD_FILE_DATA_END    = 0x01,
} E_MopClientFileTransforDataSubCmd;

#pragma pack(1)
typedef struct {
    bool     isExist;
    uint32_t fileLength;
    char     fileName[32];
    uint8_t  md5Buf[16];
} T_MopClientFileInfo;

typedef struct {
    uint8_t  cmd;
    uint8_t  subcmd;
    uint16_t seqNum;
    uint32_t dataLen;
    union {
        T_MopClientFileInfo fileInfo;
        uint8_t             fileData[0];
    } data;
} T_MopClientFileTransfor;
#pragma pack()

T_DjiReturnCode DjiTest_MopChannelClientRun(void);

#ifdef __cplusplus
}
#endif

#endif /* TEST_MOP_CHANNEL_CLIENT_H */
