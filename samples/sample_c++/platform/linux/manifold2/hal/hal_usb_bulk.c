/**
 ********************************************************************
 * @file    hal_usb_bulk.c
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
#include "hal_usb_bulk.h"
#include "dji_logger.h"
#include "utils/dji_config_manager.h"

/* Private constants ---------------------------------------------------------*/
#define LINUX_USB_BULK_TRANSFER_TIMEOUT_MS    (50)
#define LINUX_USB_BULK_TRANSFER_WAIT_FOREVER  (-1)

/* Private types -------------------------------------------------------------*/
typedef struct {
#ifdef LIBUSB_INSTALLED
    libusb_device_handle *handle;
#else
    void *handle;
#endif
    int32_t ep1;
    int32_t ep2;
    uint32_t interfaceNum;
    T_DjiHalUsbBulkInfo usbBulkInfo;
} T_HalUsbBulkObj;

/* Private values -------------------------------------------------------------*/

/* Private functions declaration ---------------------------------------------*/

/* Exported functions definition ---------------------------------------------*/
T_DjiReturnCode HalUsbBulk_Init(T_DjiHalUsbBulkInfo usbBulkInfo, T_DjiUsbBulkHandle *usbBulkHandle)
{
    int32_t ret;
    struct libusb_device_handle *handle = NULL;
    T_DjiUserLinkConfig linkConfig = {0};
    char usbBulk1EpInFd[USER_DEVICE_NAME_STR_MAX_SIZE];
    char usbBulk1EpOutFd[USER_DEVICE_NAME_STR_MAX_SIZE];
    char usbBulk2EpInFd[USER_DEVICE_NAME_STR_MAX_SIZE];
    char usbBulk2EpOutFd[USER_DEVICE_NAME_STR_MAX_SIZE];
    uint8_t usbBulk1InterfaceNum;
    uint8_t usbBulk2InterfaceNum;

    *usbBulkHandle = malloc(sizeof(T_HalUsbBulkObj));
    if (*usbBulkHandle == NULL) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    if (usbBulkInfo.isUsbHost == true) {
#ifdef LIBUSB_INSTALLED
        ret = libusb_init(NULL);
        if (ret < 0) {
            USER_LOG_ERROR("init usb bulk failed, errno = %d", ret);
            return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
        }

        handle = libusb_open_device_with_vid_pid(NULL, usbBulkInfo.vid, usbBulkInfo.pid);
        if (handle == NULL) {
            USER_LOG_ERROR("open usb device failed");
            return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
        }

        ret = libusb_claim_interface(handle, usbBulkInfo.channelInfo.interfaceNum);
        if (ret != LIBUSB_SUCCESS) {
            USER_LOG_ERROR("libusb claim interface failed, errno = %d", ret);
            libusb_close(handle);
            return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
        }

        ((T_HalUsbBulkObj *) *usbBulkHandle)->handle = handle;
        memcpy(&((T_HalUsbBulkObj *) *usbBulkHandle)->usbBulkInfo, &usbBulkInfo, sizeof(usbBulkInfo));
#endif
    } else {
        ((T_HalUsbBulkObj *) *usbBulkHandle)->handle = handle;
        memcpy(&((T_HalUsbBulkObj *) *usbBulkHandle)->usbBulkInfo, &usbBulkInfo, sizeof(usbBulkInfo));
        ((T_HalUsbBulkObj *) *usbBulkHandle)->interfaceNum = usbBulkInfo.channelInfo.interfaceNum;

        if (DjiUserConfigManager_IsEnable()) {
            DjiUserConfigManager_GetLinkConfig(&linkConfig);
            usbBulk1InterfaceNum = linkConfig.usbBulkConfig.usbBulk1InterfaceNum;
            usbBulk2InterfaceNum = linkConfig.usbBulkConfig.usbBulk2InterfaceNum;
            snprintf(usbBulk1EpOutFd, sizeof(usbBulk1EpOutFd), "%s/ep1",
                     linkConfig.usbBulkConfig.usbBulk1DeviceName);
            snprintf(usbBulk1EpInFd, sizeof(usbBulk1EpInFd), "%s/ep2",
                     linkConfig.usbBulkConfig.usbBulk1DeviceName);
            snprintf(usbBulk2EpOutFd, sizeof(usbBulk2EpOutFd), "%s/ep1",
                     linkConfig.usbBulkConfig.usbBulk2DeviceName);
            snprintf(usbBulk2EpInFd, sizeof(usbBulk2EpInFd), "%s/ep2",
                     linkConfig.usbBulkConfig.usbBulk2DeviceName);
        } else {
            usbBulk1InterfaceNum = LINUX_USB_BULK1_INTERFACE_NUM;
            usbBulk2InterfaceNum = LINUX_USB_BULK2_INTERFACE_NUM;
            strcpy(usbBulk1EpInFd, LINUX_USB_BULK1_EP_IN_FD);
            strcpy(usbBulk1EpOutFd, LINUX_USB_BULK1_EP_OUT_FD);
            strcpy(usbBulk2EpInFd, LINUX_USB_BULK2_EP_IN_FD);
            strcpy(usbBulk2EpOutFd, LINUX_USB_BULK2_EP_OUT_FD);
        }

        if (usbBulkInfo.channelInfo.interfaceNum == usbBulk1InterfaceNum) {
            ((T_HalUsbBulkObj *) *usbBulkHandle)->ep1 = open(usbBulk1EpOutFd, O_RDWR);
            if (((T_HalUsbBulkObj *) *usbBulkHandle)->ep1 < 0) {
                return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
            }

            ((T_HalUsbBulkObj *) *usbBulkHandle)->ep2 = open(usbBulk1EpInFd, O_RDWR);
            if (((T_HalUsbBulkObj *) *usbBulkHandle)->ep2 < 0) {
                return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
            }
        } else if (usbBulkInfo.channelInfo.interfaceNum == usbBulk2InterfaceNum) {
            ((T_HalUsbBulkObj *) *usbBulkHandle)->ep1 = open(usbBulk2EpOutFd, O_RDWR);
            if (((T_HalUsbBulkObj *) *usbBulkHandle)->ep1 < 0) {
                return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
            }

            ((T_HalUsbBulkObj *) *usbBulkHandle)->ep2 = open(usbBulk2EpInFd, O_RDWR);
            if (((T_HalUsbBulkObj *) *usbBulkHandle)->ep2 < 0) {
                return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
            }
        }
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode HalUsbBulk_DeInit(T_DjiUsbBulkHandle usbBulkHandle)
{
    struct libusb_device_handle *handle = NULL;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();
    int32_t ret;

    if (usbBulkHandle == NULL) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    handle = ((T_HalUsbBulkObj *) usbBulkHandle)->handle;

    if (((T_HalUsbBulkObj *) usbBulkHandle)->usbBulkInfo.isUsbHost == true) {
#ifdef LIBUSB_INSTALLED
        ret = libusb_release_interface(handle,
                                       ((T_HalUsbBulkObj *) usbBulkHandle)->usbBulkInfo.channelInfo.interfaceNum);
        if (ret != 0) {
            USER_LOG_ERROR("release usb bulk interface failed, errno = %d", ret);
            return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
        }
        libusb_exit(NULL);
#endif
    } else {
        close(((T_HalUsbBulkObj *) usbBulkHandle)->ep1);
        close(((T_HalUsbBulkObj *) usbBulkHandle)->ep2);
    }

    free(usbBulkHandle);

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode HalUsbBulk_WriteData(T_DjiUsbBulkHandle usbBulkHandle, const uint8_t *buf, uint32_t len,
                                     uint32_t *realLen)
{
    int32_t ret;
    int32_t actualLen;
    struct libusb_device_handle *handle = NULL;

    if (usbBulkHandle == NULL) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    handle = ((T_HalUsbBulkObj *) usbBulkHandle)->handle;

    if (((T_HalUsbBulkObj *) usbBulkHandle)->usbBulkInfo.isUsbHost == true) {
#ifdef LIBUSB_INSTALLED
        ret = libusb_bulk_transfer(handle, ((T_HalUsbBulkObj *) usbBulkHandle)->usbBulkInfo.channelInfo.endPointOut,
                                   (uint8_t *) buf, len, &actualLen, LINUX_USB_BULK_TRANSFER_TIMEOUT_MS);
        if (ret < 0) {
            USER_LOG_ERROR("Write usb bulk data failed, errno = %d", ret);
            return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
        }

        *realLen = actualLen;
#endif
    } else {
        *realLen = write(((T_HalUsbBulkObj *) usbBulkHandle)->ep1, buf, len);
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode HalUsbBulk_ReadData(T_DjiUsbBulkHandle usbBulkHandle, uint8_t *buf, uint32_t len,
                                    uint32_t *realLen)
{
    int32_t ret;
    struct libusb_device_handle *handle = NULL;
    int32_t actualLen;

    if (usbBulkHandle == NULL) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
    }

    handle = ((T_HalUsbBulkObj *) usbBulkHandle)->handle;

    if (((T_HalUsbBulkObj *) usbBulkHandle)->usbBulkInfo.isUsbHost == true) {
#ifdef LIBUSB_INSTALLED
        ret = libusb_bulk_transfer(handle, ((T_HalUsbBulkObj *) usbBulkHandle)->usbBulkInfo.channelInfo.endPointIn,
                                   buf, len, &actualLen, LINUX_USB_BULK_TRANSFER_WAIT_FOREVER);
        if (ret < 0) {
            USER_LOG_ERROR("Read usb bulk data failed, errno = %d", ret);
            return DJI_ERROR_SYSTEM_MODULE_CODE_SYSTEM_ERROR;
        }

        *realLen = actualLen;
#endif
    } else {
        *realLen = read(((T_HalUsbBulkObj *) usbBulkHandle)->ep2, buf, len);
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

T_DjiReturnCode HalUsbBulk_GetDeviceInfo(T_DjiHalUsbBulkDeviceInfo *deviceInfo)
{
    T_DjiUserLinkConfig linkConfig = {0};

    if (DjiUserConfigManager_IsEnable()) {
        DjiUserConfigManager_GetLinkConfig(&linkConfig);

        //attention: this interface only be called in usb device mode.
        deviceInfo->vid = linkConfig.usbBulkConfig.usbDeviceVid;
        deviceInfo->pid = linkConfig.usbBulkConfig.usbDevicePid;

        // This bulk channel is used to obtain DJI camera video stream and push 3rd-party camera video stream.
        deviceInfo->channelInfo[DJI_HAL_USB_BULK_NUM_0].interfaceNum = linkConfig.usbBulkConfig.usbBulk1InterfaceNum;
        deviceInfo->channelInfo[DJI_HAL_USB_BULK_NUM_0].endPointIn = linkConfig.usbBulkConfig.usbBulk1EndpointIn;
        deviceInfo->channelInfo[DJI_HAL_USB_BULK_NUM_0].endPointOut = linkConfig.usbBulkConfig.usbBulk1EndpointOut;

        // This bulk channel is used to obtain DJI perception image and download camera media file.
        deviceInfo->channelInfo[DJI_HAL_USB_BULK_NUM_1].interfaceNum = linkConfig.usbBulkConfig.usbBulk2InterfaceNum;
        deviceInfo->channelInfo[DJI_HAL_USB_BULK_NUM_1].endPointIn = linkConfig.usbBulkConfig.usbBulk2EndpointIn;
        deviceInfo->channelInfo[DJI_HAL_USB_BULK_NUM_1].endPointOut = linkConfig.usbBulkConfig.usbBulk2EndpointOut;
    } else {
        //attention: this interface only be called in usb device mode.
        deviceInfo->vid = LINUX_USB_VID;
        deviceInfo->pid = LINUX_USB_PID;

        // This bulk channel is used to obtain DJI camera video stream and push 3rd-party camera video stream.
        deviceInfo->channelInfo[DJI_HAL_USB_BULK_NUM_0].interfaceNum = LINUX_USB_BULK1_INTERFACE_NUM;
        deviceInfo->channelInfo[DJI_HAL_USB_BULK_NUM_0].endPointIn = LINUX_USB_BULK1_END_POINT_IN;
        deviceInfo->channelInfo[DJI_HAL_USB_BULK_NUM_0].endPointOut = LINUX_USB_BULK1_END_POINT_OUT;

        // This bulk channel is used to obtain DJI perception image and download camera media file.
        deviceInfo->channelInfo[DJI_HAL_USB_BULK_NUM_1].interfaceNum = LINUX_USB_BULK2_INTERFACE_NUM;
        deviceInfo->channelInfo[DJI_HAL_USB_BULK_NUM_1].endPointIn = LINUX_USB_BULK2_END_POINT_IN;
        deviceInfo->channelInfo[DJI_HAL_USB_BULK_NUM_1].endPointOut = LINUX_USB_BULK2_END_POINT_OUT;
    }

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/* Private functions definition-----------------------------------------------*/

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
