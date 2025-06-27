/**
 ********************************************************************
 * @file    hal_uart.c
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
#include <dji_logger.h>
#include "hal_uart.h"

/* Private constants ---------------------------------------------------------*/
#define UART_DEV_NAME_STR_SIZE             (128)
#define DJI_SYSTEM_CMD_STR_MAX_SIZE        (64)
#define DJI_SYSTEM_RESULT_STR_MAX_SIZE     (128)

/* Private types -------------------------------------------------------------*/
typedef struct {
    int32_t uartFd;
} T_UartHandleStruct;

/* Private values -------------------------------------------------------------*/

/* Private functions declaration ---------------------------------------------*/

/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode HalUart_Init(E_DjiHalUartNum uartNum, uint32_t baudRate, T_DjiUartHandle *uartHandle)
{
    T_UartHandleStruct *uartHandleStruct = NULL;
    struct termios options;
    struct flock lock;
    T_DjiReturnCode returnCode = DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
    char uartName[UART_DEV_NAME_STR_SIZE];
    char systemCmd[DJI_SYSTEM_CMD_STR_MAX_SIZE];
    char *ret = NULL;
    char lineBuf[DJI_SYSTEM_RESULT_STR_MAX_SIZE] = {0};
    FILE *fp;

    uartHandleStruct = malloc(sizeof(T_UartHandleStruct));
    if (uartHandleStruct == NULL) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_MEMORY_ALLOC_FAILED;
    }

    if (uartNum == DJI_HAL_UART_NUM_0) {
        strcpy(uartName, LINUX_UART_DEV1);
    } else if (uartNum == DJI_HAL_UART_NUM_1) {
        strcpy(uartName, LINUX_UART_DEV2);
    } else {
        goto free_uart_handle;
    }

#ifdef USE_CLION_DEBUG
    sprintf(systemCmd, "ls -l %s", uartName);
    fp = popen(systemCmd, "r");
    if (fp == NULL) {
        goto free_uart_handle;
    }

    ret = fgets(lineBuf, sizeof(lineBuf), fp);
    if (ret == NULL) {
        goto close_fp;
    }

    if (strstr(lineBuf, "crwxrwxrwx") == NULL) {
        USER_LOG_ERROR("Can't operation the device. "
                       "Probably the device has not operation permission. "
                       "Please execute command 'sudo chmod 777 %s' to add permission. ", uartName);
        goto close_fp;
    }
#else
    sprintf(systemCmd, "chmod 777 %s", uartName);
    fp = popen(systemCmd, "r");
    if (fp == NULL) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }
#endif

    uartHandleStruct->uartFd = open(uartName, (unsigned) O_RDWR | (unsigned) O_NOCTTY | (unsigned) O_NDELAY);
    if (uartHandleStruct->uartFd == -1) {
        goto close_fp;
    }

    // Forbid multiple psdk programs to access the serial port
    lock.l_type = F_WRLCK;
    lock.l_pid = getpid();
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    if (fcntl(uartHandleStruct->uartFd, F_GETLK, &lock) < 0) {
        goto close_uart_fd;
    }
    if (lock.l_type != F_UNLCK) {
        goto close_uart_fd;
    }
    lock.l_type = F_WRLCK;
    lock.l_pid = getpid();
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    if (fcntl(uartHandleStruct->uartFd, F_SETLKW, &lock) < 0) {
        goto close_uart_fd;
    }

    if (tcgetattr(uartHandleStruct->uartFd, &options) != 0) {
        goto close_uart_fd;
    }

    switch (baudRate) {
        case 115200:
            cfsetispeed(&options, B115200);
            cfsetospeed(&options, B115200);
            break;
        case 230400:
            cfsetispeed(&options, B230400);
            cfsetospeed(&options, B230400);
            break;
        case 460800:
            cfsetispeed(&options, B460800);
            cfsetospeed(&options, B460800);
            break;
        case 921600:
            cfsetispeed(&options, B921600);
            cfsetospeed(&options, B921600);
            break;
        case 1000000:
            cfsetispeed(&options, B1000000);
            cfsetospeed(&options, B1000000);
            break;
        default:
            goto close_uart_fd;
    }

    options.c_cflag |= (unsigned) CLOCAL;
    options.c_cflag |= (unsigned) CREAD;
    options.c_cflag &= ~(unsigned) CRTSCTS;
    options.c_cflag &= ~(unsigned) CSIZE;
    options.c_cflag |= (unsigned) CS8;
    options.c_cflag &= ~(unsigned) PARENB;
    options.c_iflag &= ~(unsigned) INPCK;
    options.c_cflag &= ~(unsigned) CSTOPB;
    options.c_oflag &= ~(unsigned) OPOST;
    options.c_lflag &= ~((unsigned) ICANON | (unsigned) ECHO | (unsigned) ECHOE | (unsigned) ISIG);
    options.c_iflag &= ~((unsigned) BRKINT | (unsigned) ICRNL | (unsigned) INPCK | (unsigned) ISTRIP | (unsigned) IXON);
    options.c_cc[VTIME] = 0;
    options.c_cc[VMIN] = 0;

    tcflush(uartHandleStruct->uartFd, TCIFLUSH);

    if (tcsetattr(uartHandleStruct->uartFd, TCSANOW, &options) != 0) {
        goto close_uart_fd;
    }

    *uartHandle = uartHandleStruct;
    pclose(fp);

    return returnCode;

close_uart_fd:
    close(uartHandleStruct->uartFd);

close_fp:
    pclose(fp);

free_uart_handle:
    free(uartHandleStruct);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
}

T_DjiReturnCode HalUart_DeInit(T_DjiUartHandle uartHandle)
{
    int32_t ret;
    T_UartHandleStruct *uartHandleStruct = (T_UartHandleStruct *) uartHandle;

    if (uartHandle == NULL) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_UNKNOWN;
    }

    ret = close(uartHandleStruct->uartFd);
    if (ret < 0) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    free(uartHandleStruct);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode HalUart_WriteData(T_DjiUartHandle uartHandle, const uint8_t *buf, uint32_t len, uint32_t *realLen)
{
    int32_t ret;
    T_UartHandleStruct *uartHandleStruct = (T_UartHandleStruct *) uartHandle;

    if (uartHandle == NULL || buf == NULL || len == 0 || realLen == NULL) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    ret = write(uartHandleStruct->uartFd, buf, len);
    if (ret >= 0) {
        *realLen = ret;
    } else {
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode HalUart_ReadData(T_DjiUartHandle uartHandle, uint8_t *buf, uint32_t len, uint32_t *realLen)
{
    int32_t ret;
    T_UartHandleStruct *uartHandleStruct = (T_UartHandleStruct *) uartHandle;

    if (uartHandle == NULL || buf == NULL || len == 0 || realLen == NULL) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    ret = read(uartHandleStruct->uartFd, buf, len);
    if (ret >= 0) {
        *realLen = ret;
    } else {
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode HalUart_GetStatus(E_DjiHalUartNum uartNum, T_DjiUartStatus *status)
{
    if (uartNum == DJI_HAL_UART_NUM_0) {
        status->isConnect = true;
    } else if (uartNum == DJI_HAL_UART_NUM_1) {
        status->isConnect = true;
    } else {
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode HalUart_GetDeviceInfo(T_DjiHalUartDeviceInfo *deviceInfo)
{
    if (deviceInfo == NULL) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    deviceInfo->vid = USB_UART_CONNECTED_TO_UAV_VID;
    deviceInfo->pid = USB_UART_CONNECTED_TO_UAV_PID;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/* Private functions definition-----------------------------------------------*/

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
