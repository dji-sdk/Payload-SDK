/**
 ********************************************************************
 * @file    test_high_speed_transmission.c
 * @version V2.2.0
 * @date    7/8/20
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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <pthread.h>
#include <stdbool.h>

/* Private constants ---------------------------------------------------------*/
#pragma clang diagnostic push
#pragma clang diagnostic ignored    "-Wmissing-noreturn"

#define LOG_COLOR_END               "\033[0m"
#define LOG_COLOR_RED               "\033[31m"
#define LOG_COLOR_GREEN             "\033[32m"
#define LOG_COLOR_YELLOW            "\033[33m"
#define LOG_COLOR_WHITE             "\033[37m"

#define PSDK_LOG_DEBUG(fmt, ...)     printf(LOG_COLOR_WHITE "[Debug]-[%s:%d) " fmt LOG_COLOR_END "\r\n", __FUNCTION__, __LINE__ , ##__VA_ARGS__)
#define PSDK_LOG_INFO(fmt, ...)      printf(LOG_COLOR_GREEN "[Info]-[%s:%d) " fmt LOG_COLOR_END "\r\n", __FUNCTION__, __LINE__ , ##__VA_ARGS__)
#define PSDK_LOG_WARN(fmt, ...)      printf(LOG_COLOR_YELLOW "[Warn]-[%s:%d) " fmt LOG_COLOR_END "\r\n", __FUNCTION__, __LINE__ , ##__VA_ARGS__)
#define PSDK_LOG_ERROR(fmt, ...)     printf(LOG_COLOR_RED "[Error]-[%s:%d) " fmt LOG_COLOR_END "\r\n", __FUNCTION__, __LINE__ , ##__VA_ARGS__)

#define VIDEO_STREAM_FILE_NAME      "../../../../../api_sample/camera_media_emu/media_file/PSDK_0006.h264"
#define RECV_UART_BAUDRATE          B115200
#define RECV_UART_DEV               "/dev/ttyUSB0"
#define LINUX_NETWORK_DEV           "enp0s31f6"

#define NETWORK_IP_STR_MAX_SIZE     (15)
#define LINUX_CMD_STR_MAX_SIZE      (128)
#define VIDEO_FRAME_MAX_COUNT       (18000)
#define VIDEO_FRAME_AUD_LEN         (6)
#define VIDEO_FRAME_MAX_LEN         (60000)

/* Private types -------------------------------------------------------------*/
typedef struct {
    bool isVaild;
    char ip[NETWORK_IP_STR_MAX_SIZE];
    uint16_t port;
} T_PsdkRemoteNetworkInfo;
typedef struct {
    bool isVaild;
    char ip[NETWORK_IP_STR_MAX_SIZE];
    char mask[NETWORK_IP_STR_MAX_SIZE];
} T_PsdkLocalNetworkConfig;
typedef struct {
    float durationS;
    uint32_t positionInFile;
    uint32_t size;
} T_TestPayloadCameraVideoFrameInfo;

/* Private values -------------------------------------------------------------*/
static int32_t s_uartFd = -1;
static T_PsdkRemoteNetworkInfo s_videoStreamRemoteNetworkInfo = {0};
static T_PsdkRemoteNetworkInfo s_dataStreamRemoteNetworkInfo = {0};
static T_PsdkLocalNetworkConfig s_psdkLocalNetworkConfigInfo = {0};
static const uint8_t s_frameAudInfo[VIDEO_FRAME_AUD_LEN] = {0x00, 0x00, 0x00, 0x01, 0x09, 0x10};

/* Private functions declaration ---------------------------------------------*/
static int32_t PsdkUser_GetH264Nalu(FILE *pFile, uint8_t *pNalu, uint32_t *packetLength, uint32_t *filePosition);
static int32_t PsdkUser_GetFrameInfoOfVideoFile(const char *path,
                                                T_TestPayloadCameraVideoFrameInfo *frameInfo,
                                                uint32_t frameInfoBufferCount, uint32_t *frameCount);
static int32_t PsdkUser_NetWorkConfig(const char *ipAddr, const char *netMask);
static void PsdkUser_UtilsStringSplit(char *src, const char *separator, char **dest, int32_t *num);
static int32_t PsdkUser_UartSetTermiosAttr(int32_t fd, speed_t speed);
static int32_t PsdkUser_UtilsGetLineString(int32_t fd, char *buf, size_t n);
static void *PsdkUser_UartReceiveDataTask(void *arg);
static void *PsdkUser_SendVideoStreamTask(void *arg);
static void *PsdkUser_SendDataStreamTask(void *arg);

/* Exported functions definition ---------------------------------------------*/
int32_t main(int32_t argc, char **argv)
{
    int32_t ret = 0;
    pthread_t *uartReceiveDataTask = NULL;
    pthread_t *sendVideoStreamTask = NULL;
    pthread_t *sendDataStreamTask = NULL;

    //Attention: need root permission to config uart device, such as "sudo chmod 777 /dev/ttyUSBx"
    s_uartFd = open(RECV_UART_DEV, O_RDWR | O_NOCTTY | O_NDELAY);
    /* O_RDWR   - Read/Write access to serial port                                                          */
    /* O_NOCTTY - No terminal will control the process                                                      */
    /* O_NDELAY - Non Blocking Mode, Does not care about the status of DCD line, Open() returns immediately */
    if (s_uartFd < 0) {
        PSDK_LOG_ERROR("Open recv uart device error");
        exit(1);
    } else {
        PSDK_LOG_INFO("Recv uart device opened successfully");
    }

    PsdkUser_UartSetTermiosAttr(s_uartFd, RECV_UART_BAUDRATE);
    uartReceiveDataTask = malloc(sizeof(pthread_t));
    if (uartReceiveDataTask == NULL) {
        PSDK_LOG_ERROR("Malloc error");
        exit(1);
    }

    ret = pthread_create(uartReceiveDataTask, NULL, PsdkUser_UartReceiveDataTask, NULL);
    if (ret != 0) {
        PSDK_LOG_ERROR("Create uart receive data task error");
        exit(1);
    }

    while (s_psdkLocalNetworkConfigInfo.isVaild != true) {
        PSDK_LOG_INFO("Waiting local network config info");
        sleep(1);
    }

    ret = PsdkUser_NetWorkConfig(s_psdkLocalNetworkConfigInfo.ip, s_psdkLocalNetworkConfigInfo.mask);
    if (ret != 0) {
        PSDK_LOG_ERROR("Config network error");
        exit(1);
    }

    sendVideoStreamTask = malloc(sizeof(pthread_t));
    if (sendVideoStreamTask == NULL) {
        PSDK_LOG_ERROR("Malloc error");
        exit(1);
    }

    ret = pthread_create(sendVideoStreamTask, NULL, PsdkUser_SendVideoStreamTask, NULL);
    if (ret != 0) {
        PSDK_LOG_ERROR("Create send video stream task error");
        exit(1);
    }

    sendDataStreamTask = malloc(sizeof(pthread_t));
    if (sendDataStreamTask == NULL) {
        PSDK_LOG_ERROR("Malloc error");
        exit(1);
    }

    ret = pthread_create(sendDataStreamTask, NULL, PsdkUser_SendDataStreamTask, NULL);
    if (ret != 0) {
        PSDK_LOG_ERROR("Create send data stream task error");
        exit(1);
    }

    while (1) {
        sleep(1);
    }
}

/* Private functions definition-----------------------------------------------*/
static int32_t PsdkUser_GetH264Nalu(FILE *pFile, uint8_t *pNalu, uint32_t *packetLength, uint32_t *filePosition)
{
    int32_t pos = 0;
    int32_t len = 0;

    if (pFile == NULL) {
        return -1;
    }

    if (fread(pNalu, 1, 4, pFile) <= 0) {
        return -1;
    }

    if (pNalu[0] != 0 || pNalu[1] != 0 || pNalu[2] != 0 || pNalu[3] != 1) {
        return -1;
    }

    pos = 4;

    while (1) {
        if (feof(pFile))
            break;

        pNalu[pos] = fgetc(pFile);

        if (pNalu[pos - 3] == 0 && pNalu[pos - 2] == 0 && pNalu[pos - 1] == 0 && pNalu[pos] == 1) {
            fseek(pFile, -4, SEEK_CUR);
            pos -= 4;
            break;
        }

        pos++;
    }

    len = pos + 1;

    *packetLength = len;
    *filePosition = ftell(pFile);

    return len;
}

static int32_t PsdkUser_GetFrameInfoOfVideoFile(const char *path,
                                                T_TestPayloadCameraVideoFrameInfo *frameInfo,
                                                uint32_t frameInfoBufferCount, uint32_t *frameCount)
{
    FILE *pFile = NULL;
    int32_t len;
    unsigned char *pBuf = NULL;
    unsigned char *pNalu = NULL;
    unsigned char naluType;
    bool addSlice = false;
    uint32_t spsLen = 0;
    uint32_t ppsLen = 0;
    uint32_t sliceLen = 0;
    uint32_t packetCount = 0;
    uint32_t length = 0;
    uint32_t position = 0;

    pFile = fopen(path, "rb");
    if (pFile == NULL) {
        PSDK_LOG_ERROR("Open file error");
        return -1;
    }

    pBuf = malloc(1024 * 1024);
    if (pBuf == NULL) {
        PSDK_LOG_ERROR("Malloc error");
        return -1;
    }

    while (1) {
        len = PsdkUser_GetH264Nalu(pFile, pBuf, &length, &position);
        if (len <= 0)
            break;

        if (pBuf[0] != 0 || pBuf[1] != 0 || pBuf[2] != 0 || pBuf[3] != 1)
            continue;

        pNalu = pBuf + 4;
        naluType = pNalu[0] & 0x1F;

        switch (naluType) {
            case 0x07: // SPS
                spsLen = len;
                break;
            case 0x08: // PPS
                ppsLen = len;
                addSlice = true;
                break;
            default:
                if (addSlice == true) {
                    sliceLen = len + spsLen + ppsLen;
                    addSlice = false;
                } else {
                    sliceLen = len;
                }
                frameInfo[packetCount].size = sliceLen;
                frameInfo[packetCount].positionInFile = position - frameInfo[packetCount].size;
                *frameCount = packetCount;
                if (packetCount >= frameInfoBufferCount) {
                    PSDK_LOG_ERROR("frame buffer is full.");
                    goto out;
                }
                packetCount++;
                break;
        }
    }

out:
    free(pBuf);
    fclose(pFile);

    return 0;
}

static int32_t PsdkUser_NetWorkConfig(const char *ipAddr, const char *netMask)
{
    int32_t ret;
    char cmdStr[LINUX_CMD_STR_MAX_SIZE];

    if (ipAddr == NULL || netMask == NULL) {
        PSDK_LOG_ERROR("hal network config param error");
        return -1;
    }

    //Attention: need root permission to config ip addr and netmask.
    memset(cmdStr, 0, sizeof(cmdStr));

    snprintf(cmdStr, sizeof(cmdStr), "ifconfig %s up", LINUX_NETWORK_DEV);
    ret = system(cmdStr);
    if (ret != 0) {
        PSDK_LOG_ERROR("Can't open the network."
                       "Probably the program not execute with root permission."
                       "Please use the root permission to execute the program.");
        return -1;
    }

    snprintf(cmdStr, sizeof(cmdStr), "ifconfig %s %s netmask %s", LINUX_NETWORK_DEV, ipAddr, netMask);
    ret = system(cmdStr);
    if (ret != 0) {
        PSDK_LOG_ERROR("Can't config the ip address of network."
                       "Probably the program not execute with root permission."
                       "Please use the root permission to execute the program.");
        return -1;
    }

    return 0;
}

static void PsdkUser_UtilsStringSplit(char *src, const char *separator, char **dest, int32_t *num)
{
    char *pNext;
    int32_t count = 0;
    if (src == NULL || strlen(src) == 0)
        return;
    if (separator == NULL || strlen(separator) == 0)
        return;
    pNext = (char *) strtok(src, separator);
    while (pNext != NULL) {
        *dest++ = pNext;
        ++count;
        pNext = (char *) strtok(NULL, separator);
    }
    *num = count;
}

static int32_t PsdkUser_UartSetTermiosAttr(int32_t fd, speed_t speed)
{
    /*---------- Setting the Attributes of the serial port using termios structure --------- */

    struct termios SerialPortSettings; /* Create the structure */

    tcgetattr(fd, &SerialPortSettings); /* Get the current attributes of the Serial port */

    cfsetispeed(&SerialPortSettings, speed); /* Set Read Speed */
    cfsetospeed(&SerialPortSettings, speed); /* Set Write Speed */

    SerialPortSettings.c_cflag &= ~PARENB; /* Disables the Parity Enable bit(PARENB),So No Parity */
    SerialPortSettings.c_cflag &= ~CSTOPB; /* CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit */
    SerialPortSettings.c_cflag &= ~CSIZE; /* Clears the mask for setting the data size */
    SerialPortSettings.c_cflag |= CS8; /* Set the data bits = 8 */

    SerialPortSettings.c_cflag &= ~CRTSCTS; /* No Hardware flow Control */
    SerialPortSettings.c_cflag |= CREAD | CLOCAL; /* Enable receiver,Ignore Modem Control lines */

    SerialPortSettings.c_iflag &= ~INPCK;
    SerialPortSettings.c_iflag &= ~(BRKINT | ICRNL | ISTRIP | IXON | IXOFF |
                                    IXANY); /* Disable XON/XOFF flow control both i/p and o/p */

    SerialPortSettings.c_oflag &= ~OPOST; /* No Output Processing */

    SerialPortSettings.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); /* Non Canonical mode */

    /* Setting Time outs */
    SerialPortSettings.c_cc[VMIN] = 10; /* Read at least 10 characters */
    SerialPortSettings.c_cc[VTIME] = 0; /* Wait indefinetly */

    if ((tcsetattr(fd, TCSANOW, &SerialPortSettings)) != 0) { /* Set the attributes to the termios structure */
        PSDK_LOG_ERROR("ERROR in setting attributes");
    } else {
        PSDK_LOG_INFO("\n  BaudRate = %d \n  StopBits = 1 \n  Parity   = none\r\n", speed);
    }

    tcflush(fd, TCIOFLUSH); /* Discards old data in the rx and tx buffer */

    return 0;
}

static int32_t PsdkUser_UtilsGetLineString(int32_t fd, char *buf, size_t n)
{
    char ch = 0;
    int32_t bytes_read = 0; /* Number of bytes read by the read() system call */
    int32_t index = 0;

    while (index < n) {
        bytes_read = read(fd, &ch, 1); /* Read the data */
        if (bytes_read <= 0)
            continue;

        if (ch == '\n') {
            *(buf + index) = '\0';
            break;
        } else if (ch == '\r') {
            continue;
        } else {
            *(buf + index) = ch;
        }

        index++;
    }

    if (index == n) {
        *(buf + n) = '\0';
        return 1;
    }

    return 0;
}

static void *PsdkUser_UartReceiveDataTask(void *arg)
{
    int32_t ret = 0;
    char recvBuf[1024] = {0};
    char *spiltBuf[8] = {0};
    int32_t spiltTotalNum = 0;
    char *ptr;

    while (1) {
        ret = PsdkUser_UtilsGetLineString(s_uartFd, recvBuf, sizeof(recvBuf));
        if (ret != 0) {
            PSDK_LOG_ERROR("Get line string error.");
        }
        PSDK_LOG_INFO("Read data from uart:%s, len:%ld", recvBuf, strlen(recvBuf));
        if (strlen(recvBuf) == 0) {
            continue;
        }

        PsdkUser_UtilsStringSplit(recvBuf, " ", spiltBuf, &spiltTotalNum);
        if (strcmp(spiltBuf[0], "[video_stream]") == 0 && strcmp(spiltBuf[1], "[ip]") == 0 &&
            strcmp(spiltBuf[3], "[port]") == 0) {
            ret = strtol(spiltBuf[4], &ptr, 10);
            if (ret > 0) {
                s_videoStreamRemoteNetworkInfo.port = ret;
            }
            strncpy(s_videoStreamRemoteNetworkInfo.ip, spiltBuf[2], sizeof(s_videoStreamRemoteNetworkInfo.ip));
            if (s_videoStreamRemoteNetworkInfo.port != 0 && s_videoStreamRemoteNetworkInfo.ip[0] != 0) {
                s_videoStreamRemoteNetworkInfo.isVaild = true;
            }
        }

        if (strcmp(spiltBuf[0], "[data_stream]") == 0 && strcmp(spiltBuf[1], "[ip]") == 0 &&
            strcmp(spiltBuf[3], "[port]") == 0) {
            ret = strtol(spiltBuf[4], &ptr, 10);
            if (ret > 0) {
                s_dataStreamRemoteNetworkInfo.port = ret;
            }

            strncpy(s_dataStreamRemoteNetworkInfo.ip, spiltBuf[2], sizeof(s_dataStreamRemoteNetworkInfo.ip));
            if (s_dataStreamRemoteNetworkInfo.port != 0 && s_dataStreamRemoteNetworkInfo.ip[0] != 0) {
                s_dataStreamRemoteNetworkInfo.isVaild = true;
            }
        }

        if (strcmp(spiltBuf[0], "[network_config]") == 0 && strcmp(spiltBuf[1], "[ip]") == 0 &&
            strcmp(spiltBuf[3], "[mask]") == 0) {
            strncpy(s_psdkLocalNetworkConfigInfo.ip, spiltBuf[2], sizeof(s_psdkLocalNetworkConfigInfo.ip));
            strncpy(s_psdkLocalNetworkConfigInfo.mask, spiltBuf[4], sizeof(s_psdkLocalNetworkConfigInfo.ip));
            if (s_psdkLocalNetworkConfigInfo.mask[0] != 0 && s_psdkLocalNetworkConfigInfo.ip[0] != 0) {
                s_psdkLocalNetworkConfigInfo.isVaild = true;
            }
        }

        if (s_psdkLocalNetworkConfigInfo.isVaild == true &&
            s_videoStreamRemoteNetworkInfo.isVaild == true
            && s_dataStreamRemoteNetworkInfo.isVaild == true) {
            break;
        }

        usleep(10 * 1000);
    }

    return NULL;
}

static void *PsdkUser_SendVideoStreamTask(void *arg)
{
    FILE *fpFile = NULL;
    uint32_t frameCount = 0;
    T_TestPayloadCameraVideoFrameInfo *frameInfo = NULL;
    uint8_t *dataBuffer;
    uint32_t dataLength = 0;
    uint32_t frameNumber = 0;
    uint32_t sendPackCount = 0;
    uint32_t sendLastPackSize = 0;
    int32_t ret = 0;
    int32_t s = 0;
    uint32_t j = 0;
    struct sockaddr_in addr;

    while (s_videoStreamRemoteNetworkInfo.isVaild != true) {
        PSDK_LOG_INFO("Waiting remote video stream network info");
        sleep(1);
    }

    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        PSDK_LOG_ERROR("Socket init error");
        exit(1);
    }

    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(s_videoStreamRemoteNetworkInfo.port);
    addr.sin_addr.s_addr = inet_addr(s_videoStreamRemoteNetworkInfo.ip);

    frameInfo = malloc(VIDEO_FRAME_MAX_COUNT * sizeof(T_TestPayloadCameraVideoFrameInfo));
    if (frameInfo == NULL) {
        PSDK_LOG_ERROR("Malloc error");
        exit(1);
    }

    PsdkUser_GetFrameInfoOfVideoFile(VIDEO_STREAM_FILE_NAME, frameInfo, VIDEO_FRAME_MAX_COUNT, &frameCount);
    PSDK_LOG_DEBUG("Decode file has finished, frame count:%d", frameCount);

    while (1) {
        if (fpFile != NULL)
            fclose(fpFile);

        fpFile = fopen(VIDEO_STREAM_FILE_NAME, "rb+");
        if (fpFile == NULL) {
            PSDK_LOG_ERROR("Open video file fail");
            sleep(1);
            continue;
        }

        dataBuffer = calloc(frameInfo[frameNumber].size + VIDEO_FRAME_AUD_LEN, 1);
        if (dataBuffer == NULL) {
            PSDK_LOG_ERROR("Malloc fail");
            goto free;
        }

        ret = fseek(fpFile, frameInfo[frameNumber].positionInFile, SEEK_SET);
        if (ret != 0) {
            PSDK_LOG_ERROR("Seek file fail");
            goto free;
        }

        dataLength = fread(dataBuffer, 1, frameInfo[frameNumber].size, fpFile);
        if (dataLength != frameInfo[frameNumber].size) {
            PSDK_LOG_ERROR("Read data from video file error, frame size:%d, offset:%d  number:%d",
                           frameInfo[frameNumber].size, frameInfo[frameNumber].positionInFile, frameNumber);
        } else {
            PSDK_LOG_DEBUG("Read data from video file success, frame size:%d, offset:%d  number:%d",
                           frameInfo[frameNumber].size, frameInfo[frameNumber].positionInFile, frameNumber);
        }

        memcpy(&dataBuffer[frameInfo[frameNumber].size], &s_frameAudInfo, VIDEO_FRAME_AUD_LEN);
        dataLength = dataLength + VIDEO_FRAME_AUD_LEN;

        if (dataLength > VIDEO_FRAME_MAX_LEN) {
            sendPackCount = dataLength / VIDEO_FRAME_MAX_LEN;
            sendLastPackSize = dataLength % VIDEO_FRAME_MAX_LEN;
            for (j = 0; j < sendPackCount; j++) {
                ret = sendto(s, (const uint8_t *) &dataBuffer[VIDEO_FRAME_MAX_LEN * j], VIDEO_FRAME_MAX_LEN,
                             MSG_NOSIGNAL | MSG_DONTWAIT, (struct sockaddr *) &addr, sizeof(struct sockaddr_in));
                if (ret > 0) {
                    PSDK_LOG_INFO("Send video stream success, len = %d", ret);
                } else {
                    PSDK_LOG_ERROR("Send video stream failed, len = %d", ret);
                }
            }

            ret = sendto(s, (const uint8_t *) &dataBuffer[VIDEO_FRAME_MAX_LEN * sendPackCount], sendLastPackSize,
                         MSG_NOSIGNAL | MSG_DONTWAIT, (struct sockaddr *) &addr, sizeof(struct sockaddr_in));
            if (ret > 0) {
                PSDK_LOG_INFO("Send video stream success, len = %d", ret);
            } else {
                PSDK_LOG_ERROR("Send video stream failed, len = %d", ret);
            }
        } else {
            ret = sendto(s, (const uint8_t *) dataBuffer, dataLength, MSG_NOSIGNAL | MSG_DONTWAIT,
                         (struct sockaddr *) &addr, sizeof(struct sockaddr_in));
            if (ret > 0) {
                PSDK_LOG_INFO("Send video stream success, len = %d", ret);
            } else {
                PSDK_LOG_ERROR("Send video stream failed, len = %d", ret);
            }
        }

        if ((frameNumber++) >= frameCount) {
            PSDK_LOG_WARN("Reach file tail");
            frameNumber = 0;
        }
free:
        free(dataBuffer);

        usleep(33000);
    }
}

static void *PsdkUser_SendDataStreamTask(void *arg)
{
    int32_t s, len;
    struct sockaddr_in addr;
    char buffer[512];
    int32_t testId = 0;

    while (s_dataStreamRemoteNetworkInfo.isVaild != true) {
        PSDK_LOG_INFO("Waiting remote data stream network info");
        sleep(1);
    }

    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        PSDK_LOG_ERROR("Socket init error");
        exit(1);
    }

    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(s_dataStreamRemoteNetworkInfo.port);
    addr.sin_addr.s_addr = inet_addr(s_dataStreamRemoteNetworkInfo.ip);

    while (1) {
        if (testId >= 100) {
            testId = 0;
        }

        sprintf(buffer, "Network Test Data %d", testId++);
        len = sendto(s, buffer, strlen(buffer) + 1, MSG_NOSIGNAL | MSG_DONTWAIT, (struct sockaddr *) &addr,
                     sizeof(struct sockaddr_in));
        if (len > 0) {
            PSDK_LOG_INFO("Send data stream success, len = %d", len);
        } else {
            PSDK_LOG_ERROR("Send data stream failed");
        }

        sleep(1);
    }
}

#pragma clang diagnostic pop

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/