/**
 ********************************************************************
 * @file    dji_platform.h
 * @brief   This is the header file for "dji_platform.c", defining the structure and
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
#ifndef DJI_PLATFORM_H
#define DJI_PLATFORM_H

/* Includes ------------------------------------------------------------------*/
#include "dji_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/**
* @brief Platform handle of uart operation.
*/
typedef void *T_DjiUartHandle;

/**
* @brief Platform handle of usb bulk operation.
*/
typedef void *T_DjiUsbBulkHandle;

/**
* @brief Platform handle of network operation.
*/
typedef void *T_DjiNetworkHandle;

/**
* @brief Platform handle of i2c device operation.
*/
typedef void *T_DjiI2cHandle;

/**
* @brief Platform handle of thread task operation.
*/
typedef void *T_DjiTaskHandle;

/**
* @brief Platform handle of mutex operation.
*/
typedef void *T_DjiMutexHandle;

/**
* @brief Platform handle of semaphore operation.
*/
typedef void *T_DjiSemaHandle;

/**
* @brief Platform handle of file operation.
*/
typedef void *T_DjiFileHandle;

/**
* @brief Platform handle of dir operation.
*/
typedef void *T_DjiDirHandle;

/**
* @brief Platform handle of socket operation.
*/
typedef void *T_DjiSocketHandle;

typedef enum {
    /**
    * All aircraft type support，users can connect via chip serial port or USB to TTL serial port.
    * Baud rate support list on M300 RTK Payload Port: 115200, 230400, 460800, 921600.
    * Baud rate support list on M300 RTK Extension Port: 115200, 230400, 460800, 921600, 1000000.
    * Baud rate support list on M30/M30T: 115200, 230400, 460800, 921600, 1000000.
    * Baud rate support list on M3E/M3T: 921600.
    * Baud rate support list on M350 RTK Payload Port: 115200, 230400, 460800, 921600.
    * Baud rate support list on M350 RTK Extension Port: 115200, 230400, 460800, 921600, 1000000.
    * */
    DJI_HAL_UART_NUM_0,
    /**
    * Only support on M300/M350 RTK Extension Port by USB virtual serial port, such as /dev/ttyACM0.
    * Baud rate support list on M300 RTK Extension Port: 921600.
    * Baud rate support list on M350 RTK Extension Port: 921600.
    * */
    DJI_HAL_UART_NUM_1,
} E_DjiHalUartNum;

typedef enum {
    DJI_HAL_USB_BULK_NUM_0 = 0,
    DJI_HAL_USB_BULK_NUM_1,
    DJI_HAL_USB_BULK_NUM_2,
    DJI_HAL_USB_BULK_NUM_MAX,
} E_DjiHalUsbBulkNum;

typedef enum {
    DJI_SOCKET_MODE_UDP,
    DJI_SOCKET_MODE_TCP,
} E_DjiSocketMode;

typedef struct {
    bool isConnect;
} T_DjiUartStatus;

typedef struct {
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} T_DjiTime;

typedef struct {
    uint32_t size;
    T_DjiTime createTime;
    T_DjiTime modifyTime;
    char path[DJI_FILE_PATH_SIZE_MAX];
    bool isDir;
} T_DjiFileInfo;

typedef struct {
    uint16_t pid;
    uint16_t vid;
} T_DjiHalUartDeviceInfo;

typedef struct {
    T_DjiReturnCode (*UartInit)(E_DjiHalUartNum uartNum, uint32_t baudRate, T_DjiUartHandle *uartHandle);

    T_DjiReturnCode (*UartDeInit)(T_DjiUartHandle uartHandle);

    T_DjiReturnCode (*UartWriteData)(T_DjiUartHandle uartHandle, const uint8_t *buf, uint32_t len, uint32_t *realLen);

    T_DjiReturnCode (*UartReadData)(T_DjiUartHandle uartHandle, uint8_t *buf, uint32_t len, uint32_t *realLen);

    T_DjiReturnCode (*UartGetStatus)(E_DjiHalUartNum uartNum, T_DjiUartStatus *status);

    /**
     * Get the device info of USB uart (virtual com or TTL-To-USB) which directly connected to UAV.
     * Use for SDK adapter type Eport V2 ribbon cable.
     * When using other types of interface, it is not necessary to implement this member function.
     */
    T_DjiReturnCode (*UartGetDeviceInfo)(T_DjiHalUartDeviceInfo *deviceInfo);
} T_DjiHalUartHandler;

typedef struct {
    uint16_t interfaceNum;
    uint16_t endPointIn;
    uint16_t endPointOut;
} T_DjiHalUsbBulkChannelInfo;

typedef struct {
    bool isUsbHost;
    // attention: if 'isUsbHost' equals false, the following parameters is not valid.
    uint16_t pid;
    uint16_t vid;
    T_DjiHalUsbBulkChannelInfo channelInfo;
} T_DjiHalUsbBulkInfo;

typedef struct {
    uint16_t pid;
    uint16_t vid;
    T_DjiHalUsbBulkChannelInfo channelInfo[DJI_HAL_USB_BULK_NUM_MAX];
} T_DjiHalUsbBulkDeviceInfo;

typedef struct {
    struct {
        uint16_t vid;
        uint16_t pid;
    } usbNetAdapter;
} T_DjiHalNetworkDeviceInfo;

typedef struct {
    uint32_t i2cSpeed;
    uint16_t devAddress;
} T_DjiHalI2cConfig;

typedef struct {
    T_DjiReturnCode (*UsbBulkInit)(T_DjiHalUsbBulkInfo usbBulkInfo, T_DjiUsbBulkHandle *usbBulkHandle);

    T_DjiReturnCode (*UsbBulkDeInit)(T_DjiUsbBulkHandle usbBulkHandle);

    T_DjiReturnCode (*UsbBulkWriteData)(T_DjiUsbBulkHandle usbBulkHandle, const uint8_t *buf,
                                        uint32_t len, uint32_t *realLen);

    T_DjiReturnCode (*UsbBulkReadData)(T_DjiUsbBulkHandle usbBulkHandle, uint8_t *buf,
                                       uint32_t len, uint32_t *realLen);

    T_DjiReturnCode (*UsbBulkGetDeviceInfo)(T_DjiHalUsbBulkDeviceInfo *deviceInfo);
} T_DjiHalUsbBulkHandler;

typedef struct {
    T_DjiReturnCode (*NetworkInit)(const char *ipAddr, const char *netMask, T_DjiNetworkHandle *networkHandle);

    T_DjiReturnCode (*NetworkDeInit)(T_DjiNetworkHandle networkHandle);

    T_DjiReturnCode (*NetworkGetDeviceInfo)(T_DjiHalNetworkDeviceInfo *deviceInfo);
} T_DjiHalNetworkHandler;

typedef struct {
    T_DjiReturnCode (*I2cInit)(T_DjiHalI2cConfig i2cConfig, T_DjiI2cHandle *i2cHandle);

    T_DjiReturnCode (*I2cDeInit)(T_DjiI2cHandle i2cHandle);

    T_DjiReturnCode (*I2cWriteData)(T_DjiI2cHandle i2cHandle, uint16_t devAddress, const uint8_t *buf,
                                    uint32_t len, uint32_t *realLen);

    T_DjiReturnCode (*I2cReadData)(T_DjiI2cHandle i2cHandle, uint16_t devAddress, uint8_t *buf,
                                   uint32_t len, uint32_t *realLen);
} T_DjiHalI2cHandler;

typedef struct {
    T_DjiReturnCode (*TaskCreate)(const char *name, void *(*taskFunc)(void *),
                                  uint32_t stackSize, void *arg, T_DjiTaskHandle *task);

    T_DjiReturnCode (*TaskDestroy)(T_DjiTaskHandle task);

    T_DjiReturnCode (*TaskSleepMs)(uint32_t timeMs);

    T_DjiReturnCode (*MutexCreate)(T_DjiMutexHandle *mutex);

    T_DjiReturnCode (*MutexDestroy)(T_DjiMutexHandle mutex);

    T_DjiReturnCode (*MutexLock)(T_DjiMutexHandle mutex);

    T_DjiReturnCode (*MutexUnlock)(T_DjiMutexHandle mutex);

    T_DjiReturnCode (*SemaphoreCreate)(uint32_t initValue, T_DjiSemaHandle *semaphore);

    T_DjiReturnCode (*SemaphoreDestroy)(T_DjiSemaHandle semaphore);

    T_DjiReturnCode (*SemaphoreWait)(T_DjiSemaHandle semaphore);

    T_DjiReturnCode (*SemaphoreTimedWait)(T_DjiSemaHandle semaphore, uint32_t waitTimeMs);

    T_DjiReturnCode (*SemaphorePost)(T_DjiSemaHandle semaphore);

    T_DjiReturnCode (*GetTimeMs)(uint32_t *ms);

    T_DjiReturnCode (*GetTimeUs)(uint64_t *us);

    T_DjiReturnCode (*GetRandomNum)(uint16_t *randomNum);

    void *(*Malloc)(uint32_t size);

    void (*Free)(void *ptr);
} T_DjiOsalHandler;

typedef struct {
    T_DjiReturnCode (*FileOpen)(const char *fileName, const char *fileMode, T_DjiFileHandle *fileObj);

    T_DjiReturnCode (*FileClose)(T_DjiFileHandle fileObj);

    T_DjiReturnCode (*FileWrite)(T_DjiFileHandle fileObj, const uint8_t *buf, uint32_t len, uint32_t *realLen);

    T_DjiReturnCode (*FileRead)(T_DjiFileHandle fileObj, uint8_t *buf, uint32_t len, uint32_t *realLen);

    T_DjiReturnCode (*FileSeek)(T_DjiFileHandle fileObj, uint32_t offset);

    T_DjiReturnCode (*FileSync)(T_DjiFileHandle fileObj);

    T_DjiReturnCode (*DirOpen)(const char *filePath, T_DjiDirHandle *dirObj);

    T_DjiReturnCode (*DirClose)(T_DjiDirHandle dirObj);

    T_DjiReturnCode (*DirRead)(T_DjiDirHandle dirObj, T_DjiFileInfo *fileInfo);

    T_DjiReturnCode (*Mkdir)(const char *filePath);

    T_DjiReturnCode (*Unlink)(const char *filePath);

    T_DjiReturnCode (*Rename)(const char *oldFilePath, const char *newFilePath);

    T_DjiReturnCode (*Stat)(const char *filePath, T_DjiFileInfo *fileInfo);
} T_DjiFileSystemHandler;

typedef struct {
    T_DjiReturnCode (*Socket)(E_DjiSocketMode mode, T_DjiSocketHandle *socketHandle);

    T_DjiReturnCode (*Close)(T_DjiSocketHandle socketHandle);

    T_DjiReturnCode (*Bind)(T_DjiSocketHandle socketHandle, const char *ipAddr, uint32_t port);

    T_DjiReturnCode (*UdpSendData)(T_DjiSocketHandle socketHandle, const char *ipAddr, uint32_t port,
                                   const uint8_t *buf, uint32_t len, uint32_t *realLen);

    T_DjiReturnCode (*UdpRecvData)(T_DjiSocketHandle socketHandle, char *ipAddr, uint32_t *port,
                                   uint8_t *buf, uint32_t len, uint32_t *realLen);

    T_DjiReturnCode (*TcpListen)(T_DjiSocketHandle socketHandle);

    T_DjiReturnCode (*TcpAccept)(T_DjiSocketHandle socketHandle, char *ipAddr, uint32_t *port,
                                 T_DjiSocketHandle *outSocketHandle);

    T_DjiReturnCode (*TcpConnect)(T_DjiSocketHandle socketHandle, const char *ipAddr, uint32_t port);

    T_DjiReturnCode (*TcpSendData)(T_DjiSocketHandle socketHandle,
                                   const uint8_t *buf, uint32_t len, uint32_t *realLen);

    T_DjiReturnCode (*TcpRecvData)(T_DjiSocketHandle socketHandle,
                                   uint8_t *buf, uint32_t len, uint32_t *realLen);
} T_DjiSocketHandler;

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Register the handler for hal uart interfaces by your platform.
 * @note It should be noted that the interface in hal is written and tested well. Users need to implement all the
 * interfaces. Otherwise, the user interface cannot be successfully registered, and then the user interface is registered
 * through the interface. If the registration fails, it needs to be based on the return code. To judge the problem. Make
 * sure that the feature is available after a successful registration. The interface needs to be called at the beginning of
 * the application for registration, otherwise, the subsequent functions will not work properly.
 * @param halUartHandler: pointer to the handler for hal uart interfaces by your platform.
 * @return Execution result.
 */
T_DjiReturnCode DjiPlatform_RegHalUartHandler(const T_DjiHalUartHandler *halUartHandler);

/**
 * @brief Register the handler for usb bulk interfaces by your platform.
 * @param fileSystemHandler: pointer to the handler for usb bulk interfaces by your platform.
 * @return Execution result.
 */
T_DjiReturnCode DjiPlatform_RegHalUsbBulkHandler(const T_DjiHalUsbBulkHandler *halUsbBulkHandler);

/**
 * @brief Register the handler for hal network interfaces by your platform.
 * @note It should be noted that the interface in hal is written and tested well. Users need to implement all the
 * interfaces. Otherwise, the user interface cannot be successfully registered, and then the user interface is registered
 * through the interface. If the registration fails, it needs to be based on the return code. To judge the problem. Make
 * sure that the feature is available after a successful registration.
 * @attention The interface needs to be called at the beginning of the application for registration, otherwise, the
 * subsequent functions will not work properly.
 * @param halNetworkHandler: pointer to the handler for network handler interfaces by your platform.
 * @return Execution result.
 */
T_DjiReturnCode DjiPlatform_RegHalNetworkHandler(const T_DjiHalNetworkHandler *halNetworkHandler);

/**
 * @brief Register the handler for hal i2c master mode interfaces by your platform.
 * @note It should be noted that the interface in hal is written and tested well. Users need to implement all the
 * interfaces. Otherwise, the user interface cannot be successfully registered, and then the user interface is registered
 * through the interface. If the registration fails, it needs to be based on the return code. To judge the problem. Make
 * sure that the feature is available after a successful registration.
 * @attention The interface needs to be called at the beginning of the application for registration, otherwise, the
 * subsequent functions will not work properly.
 * @param halI2cHandler: pointer to the handler for hal i2c handler interfaces by your platform.
 * @return Execution result.
 */
T_DjiReturnCode DjiPlatform_RegHalI2cHandler(const T_DjiHalI2cHandler *halI2cHandler);

/**
 * @brief Register the handler for osal interfaces by your platform.
 * @note It should be noted that the interface in osal is written and tested well. Users need to implement all the
 * interfaces. Otherwise, the user interface cannot be successfully registered, and then the user interface is registered
 * through the interface. If the registration fails, it needs to be based on the return code. To judge the problem. Make
 * sure that the feature is available after a successful registration. The interface needs to be called at the beginning of
 * the application for registration, otherwise, the subsequent functions will not work properly.
 * @param osalHandler: pointer to the handler for osal interfaces by your platform.
 * @return Execution result.
 */
T_DjiReturnCode DjiPlatform_RegOsalHandler(const T_DjiOsalHandler *osalHandler);

/**
 * @brief Register the handler for file-system interfaces by your platform.
 * @param fileSystemHandler: pointer to the handler for file-system interfaces by your platform.
 * @return Execution result.
 */
T_DjiReturnCode DjiPlatform_RegFileSystemHandler(const T_DjiFileSystemHandler *fileSystemHandler);

/**
 * @brief Register the handler for socket interfaces by your platform.
 * @param fileSystemHandler: pointer to the handler for socket interfaces by your platform.
 * @return Execution result.
 */
T_DjiReturnCode DjiPlatform_RegSocketHandler(const T_DjiSocketHandler *socketHandler);

/**
 * @brief Get the handler of osal interfaces.
 * @return Pointer to osal handler.
 */
T_DjiOsalHandler *DjiPlatform_GetOsalHandler(void);

/**
 * @brief Get the handler of usb bulk interfaces.
 * @return Pointer to usb bulk handler.
 */
T_DjiHalUsbBulkHandler *DjiPlatform_GetHalUsbBulkHandler(void);

/**
 * @brief Get the handler of network interfaces.
 * @return Pointer to network handler.
 */
T_DjiHalNetworkHandler *DjiPlatform_GetHalNetworkHandler(void);

/**
 * @brief Get the handler of i2c interfaces.
 * @return Pointer to i2c handler.
 */
T_DjiHalI2cHandler *DjiPlatform_GetHalI2cHandler(void);

/**
 * @brief Get the handler of file-system interfaces.
 * @return Pointer to file-system handler.
 */
T_DjiFileSystemHandler *DjiPlatform_GetFileSystemHandler(void);

/**
 * @brief Get the handler of socket interfaces.
 * @return Pointer to socket handler.
 */
T_DjiSocketHandler *DjiPlatform_GetSocketHandler(void);

#ifdef __cplusplus
}
#endif

#endif // DJI_PLATFORM_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
