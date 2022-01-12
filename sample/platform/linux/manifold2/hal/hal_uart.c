/**
 ********************************************************************
 * @file    psdk_hal.c
 * @version V2.0.0
 * @date    2019/07/01
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
#include "hal_uart.h"

/* Private constants ---------------------------------------------------------*/
#define LINUX_UART_DEV    "/dev/ttyUSB0"

/* Private types -------------------------------------------------------------*/
static int s_psdkUartFd = -1;

/* Private functions declaration ---------------------------------------------*/

/* Exported functions definition ---------------------------------------------*/
T_PsdkReturnCode Hal_UartInit(void)
{
    struct termios options;
    struct flock lock;
    T_PsdkReturnCode returnCode = PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;

    s_psdkUartFd = open(LINUX_UART_DEV, O_RDWR | O_NOCTTY | O_NDELAY);
    if (s_psdkUartFd == -1) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    // Forbid multiple psdk programs to access the serial port
    lock.l_type = F_WRLCK;
    lock.l_pid = getpid();
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    if (fcntl(s_psdkUartFd, F_GETLK, &lock) < 0) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }
    if (lock.l_type != F_UNLCK) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }
    lock.l_type = F_WRLCK;
    lock.l_pid = getpid();
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    if (fcntl(s_psdkUartFd, F_SETLKW, &lock) < 0) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    if (tcgetattr(s_psdkUartFd, &options) != 0) {
        close(s_psdkUartFd);
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    cfsetispeed(&options, B460800);
    cfsetospeed(&options, B460800);

    options.c_cflag |= CLOCAL;
    options.c_cflag |= CREAD;
    options.c_cflag &= ~CRTSCTS;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_cflag &= ~PARENB;
    options.c_iflag &= ~INPCK;
    options.c_cflag &= ~CSTOPB;
    options.c_oflag &= ~OPOST;
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    options.c_cc[VTIME] = 0;
    options.c_cc[VMIN] = 0;

    tcflush(s_psdkUartFd, TCIFLUSH);

    if (tcsetattr(s_psdkUartFd, TCSANOW, &options) != 0) {
        close(s_psdkUartFd);
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    return returnCode;
}

T_PsdkReturnCode Hal_UartSendData(const uint8_t *pBuf, uint16_t bufLen)
{
    int32_t realLen;

    if (s_psdkUartFd == -1) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    realLen = write(s_psdkUartFd, pBuf, bufLen);
    if (realLen == bufLen) {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    } else {
        return PSDK_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }
}

T_PsdkReturnCode Hal_UartReadData(uint8_t *pBuf, uint16_t bufLen, uint16_t *realBufLen)
{
    int result;
    T_PsdkReturnCode returnCode;

    result = read(s_psdkUartFd, pBuf, bufLen);
    if (result >= 0) {
        *realBufLen = result;
        returnCode = PSDK_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    } else {
        *realBufLen = 0;
        returnCode = PSDK_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    return returnCode;
}

/* Private functions definition-----------------------------------------------*/

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
