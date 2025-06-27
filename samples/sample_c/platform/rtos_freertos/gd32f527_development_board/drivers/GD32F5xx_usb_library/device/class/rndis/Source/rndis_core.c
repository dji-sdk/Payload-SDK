/*!
    \file    rndis_core.c
    \brief   rndis driver

    \version 2023-12-20, V1.0.0, firmware for GD32F5xx
*/

/*
    Copyright (c) 2023, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "rndis_core.h"
#include "usbd_transc.h"
#include "rndis_oid.h"
#include <string.h>
#include "ethernetif.h"

#define USBD_VID                          0x28E9U
#define USBD_PID                          0x0801U

/* note:it should use the C99 standard when compiling the below codes */
/* USB standard device descriptor */
const usb_desc_dev rndis_dev_desc =
{
    .header = 
     {
         .bLength          = USB_DEV_DESC_LEN, 
         .bDescriptorType  = USB_DESCTYPE_DEV,
     },
    .bcdUSB                = 0x0200U,
    .bDeviceClass          = USB_CLASS_WIRELESS_CTR,
    .bDeviceSubClass       = 0x00U,
    .bDeviceProtocol       = 0x00U,
    .bMaxPacketSize0       = USB_FS_EP0_MAX_LEN,
    .idVendor              = USBD_VID,
    .idProduct             = USBD_PID,
    .bcdDevice             = 0x0100U,
    .iManufacturer         = STR_IDX_MFC,
    .iProduct              = STR_IDX_PRODUCT,
    .iSerialNumber         = STR_IDX_SERIAL,
    .bNumberConfigurations = USBD_CFG_MAX_NUM,
};

/* USB device configuration descriptor */
const usb_rndis_desc_config_set rndis_config_desc = 
{
    .config = 
    {
        .header = 
         {
             .bLength         = sizeof(usb_desc_config), 
             .bDescriptorType = USB_DESCTYPE_CONFIG,
         },
        .wTotalLength         = USB_RNDIS_CONFIG_DESC_SIZE,
        .bNumInterfaces       = 0x02U,
        .bConfigurationValue  = 0x01U,
        .iConfiguration       = 0x00U,
        .bmAttributes         = 0x40U,
        .bMaxPower            = 0x01U
    },
    
    .cmd_itf = 
    {
        .header = 
         {
             .bLength         = sizeof(usb_desc_itf), 
             .bDescriptorType = USB_DESCTYPE_ITF 
         },
        .bInterfaceNumber     = 0x00U,
        .bAlternateSetting    = 0x00U,
        .bNumEndpoints        = 0x01U,
        .bInterfaceClass      = USB_CLASS_WIRELESS_CTR,
        .bInterfaceSubClass   = 0x01U,
        .bInterfaceProtocol   = 0X03U,
        .iInterface           = 0x00U
    },

    .rndis_header = 
    {
        .header =
         {
            .bLength         = sizeof(usb_desc_header_func), 
            .bDescriptorType = USB_DESCTYPE_CS_INTERFACE
         },
        .bDescriptorSubtype  = 0x00U,
        .bcdCDC              = 0x0110U
    },

    .rndis_call_managment = 
    {
        .header = 
         {
            .bLength         = sizeof(usb_desc_call_managment_func), 
            .bDescriptorType = USB_DESCTYPE_CS_INTERFACE
         },
        .bDescriptorSubtype  = 0x01U,
        .bmCapabilities      = 0x00U,
        .bDataInterface      = 0x01U
    },

    .rndis_acm = 
    {
        .header = 
         {
            .bLength         = sizeof(usb_desc_acm_func), 
            .bDescriptorType = USB_DESCTYPE_CS_INTERFACE
         },
        .bDescriptorSubtype  = 0x02U,
        .bmCapabilities      = 0x00U,
    },

    .rndis_union = 
    {
        .header = 
         {
            .bLength         = sizeof(usb_desc_union_func), 
            .bDescriptorType = USB_DESCTYPE_CS_INTERFACE
         },
        .bDescriptorSubtype  = 0x06U,
        .bMasterInterface    = 0x00U,
        .bSlaveInterface0    = 0x01U,
    },

    .rndis_cmd_endpoint = 
    {
        .header = 
         {
            .bLength         = sizeof(usb_desc_ep), 
            .bDescriptorType = USB_DESCTYPE_EP,
         },
        .bEndpointAddress    = RNDIS_CMD_EP,
        .bmAttributes        = USB_EP_ATTR_INT,
        .wMaxPacketSize      = USB_RNDIS_CMD_PACKET_SIZE,
        .bInterval           = 0x01U
    },

    .rndis_data_interface = 
    {
        .header = 
         {
            .bLength         = sizeof(usb_desc_itf), 
            .bDescriptorType = USB_DESCTYPE_ITF,
         },
        .bInterfaceNumber    = 0x01U,
        .bAlternateSetting   = 0x00U,
        .bNumEndpoints       = 0x02U,
        .bInterfaceClass     = USB_CLASS_DATA,
        .bInterfaceSubClass  = 0x00U,
        .bInterfaceProtocol  = 0x00U,
        .iInterface          = 0x00U
    },

    .rndis_out_endpoint = 
    {
        .header = 
         {
             .bLength         = sizeof(usb_desc_ep), 
             .bDescriptorType = USB_DESCTYPE_EP, 
         },
        .bEndpointAddress     = RNDIS_DATA_OUT_EP,
        .bmAttributes         = USB_EP_ATTR_BULK,
        .wMaxPacketSize       = USB_RNDIS_DATA_PACKET_SIZE,
        .bInterval            = 0x00U
    },

    .rndis_in_endpoint = 
    {
        .header = 
         {
             .bLength         = sizeof(usb_desc_ep), 
             .bDescriptorType = USB_DESCTYPE_EP 
         },
        .bEndpointAddress     = RNDIS_DATA_IN_EP,
        .bmAttributes         = USB_EP_ATTR_BULK,
        .wMaxPacketSize       = USB_RNDIS_DATA_PACKET_SIZE,
        .bInterval            = 0x00U
    }
};

/* USB language ID Descriptor */
static const usb_desc_LANGID usbd_language_id_desc = 
{
    .header = 
     {
         .bLength         = sizeof(usb_desc_LANGID), 
         .bDescriptorType = USB_DESCTYPE_STR,
     },
    .wLANGID              = ENG_LANGID
};

/* USB manufacture string */
static const usb_desc_str manufacturer_string = 
{
    .header = 
     {
         .bLength         = USB_STRING_LEN(10), 
         .bDescriptorType = USB_DESCTYPE_STR,
     },
    .unicode_string = {'G', 'i', 'g', 'a', 'D', 'e', 'v', 'i', 'c', 'e'}
};

/* USB product string */
static const usb_desc_str product_string = 
{
    .header = 
     {
         .bLength         = USB_STRING_LEN(14), 
         .bDescriptorType = USB_DESCTYPE_STR,
     },
    .unicode_string = {'G', 'D', '3', '2', '-', 'U', 'S', 'B', '_', 'R', 'N', 'D', 'I', 'S'}
};

/* USBD serial string */
static usb_desc_str serial_string = 
{
    .header = 
     {
         .bLength         = USB_STRING_LEN(12), 
         .bDescriptorType = USB_DESCTYPE_STR,
     }
};

/* USB string descriptor */
void *const usbd_rndis_strings[] = 
{
    [STR_IDX_LANGID]  = (uint8_t *)&usbd_language_id_desc,
    [STR_IDX_MFC]     = (uint8_t *)&manufacturer_string,
    [STR_IDX_PRODUCT] = (uint8_t *)&product_string,
    [STR_IDX_SERIAL]  = (uint8_t *)&serial_string
};

usb_desc rndis_desc = {
    .dev_desc    = (uint8_t *)&rndis_dev_desc,
    .config_desc = (uint8_t *)&rndis_config_desc,
    .strings     = usbd_rndis_strings
};

/* local function prototypes ('static') */
static uint8_t rndis_init   (usb_dev *udev, uint8_t config_index);
static uint8_t rndis_deinit (usb_dev *udev, uint8_t config_index);
static uint8_t rndis_req    (usb_dev *udev, usb_req *req);
static uint8_t rndis_in     (usb_dev *udev, uint8_t ep_num);
static uint8_t rndis_out    (usb_dev *udev, uint8_t ep_num);

static void rndis_msg_parser (usb_dev *udev);
static void rndis_data_msg_handle (usb_dev *udev);

/* USB CDC device class callbacks structure */
usb_class_core rndis_class =
{
    .command   = NO_CMD,
    .alter_set = 0,

    .init      = rndis_init,
    .deinit    = rndis_deinit,

    .req_proc  = rndis_req,

    .data_in   = rndis_in,
    .data_out  = rndis_out
};

/*!
    \brief      initialize the CDC ACM device
    \param[in]  udev: pointer to USB device instance
    \param[in]  config_index: configuration index
    \param[out] none
    \retval     USB device operation status
*/
static uint8_t rndis_init (usb_dev *udev, uint8_t config_index)
{
    static usbd_rndis_handler rndis_handler;

    memset((void *)&rndis_handler, 0, sizeof(usbd_rndis_handler));

    /* initialize the data Tx endpoint */
    usbd_ep_setup (udev, &(rndis_config_desc.rndis_in_endpoint));

    /* initialize the data Rx endpoint */
    usbd_ep_setup (udev, &(rndis_config_desc.rndis_out_endpoint));

    /* initialize the command Tx endpoint */
    usbd_ep_setup (udev, &(rndis_config_desc.rndis_cmd_endpoint));

    rndis_handler.usb_enet_cmd.usb_enet_response_available_buffer[0] = 0x01;
    rndis_handler.usb_enet_cmd.cmd_send = 1;

    rndis_handler.init_cmplt.MessageType = 0x80000002;
    rndis_handler.keepalive_cmplt.MessageType = 0x80000008;
    rndis_handler.query_cmplt.MessageType = 0x80000004;
    rndis_handler.set_cmplt.MessageType = 0x80000005;
    rndis_handler.package_return_msg.MessageType = 0x00000001;
    rndis_handler.package_return_msg.DataOffset = 0x00000024;

    udev->dev.class_data[USBD_RNDIS_INTERFACE] = (void *)&rndis_handler;

    return USBD_OK;
}

/*!
    \brief      de-initialize the CDC ACM device
    \param[in]  udev: pointer to USB device instance
    \param[in]  config_index: configuration index
    \param[out] none
    \retval     USB device operation status
*/
static uint8_t rndis_deinit (usb_dev *udev, uint8_t config_index)
{
    /* deinitialize the data Tx/Rx endpoint */
    usbd_ep_clear (udev, RNDIS_DATA_IN_EP);
    usbd_ep_clear (udev, RNDIS_DATA_OUT_EP);

    /* deinitialize the command Tx endpoint */
    usbd_ep_clear (udev, RNDIS_CMD_EP);

    return USBD_OK;
}

static void rndis_msg_parser (usb_dev *udev)
{
    usbd_rndis_handler *rndis = (usbd_rndis_handler *)udev->dev.class_data[USBD_RNDIS_INTERFACE];

    if (rndis->usb_enet_cmd.usb_enet_cmd_buffer[0] == 0x02) {
        rndis->init_msg = (rndis_initialize_msg_struct *)rndis->usb_enet_cmd.usb_enet_cmd_buffer;
    } else if (rndis->usb_enet_cmd.usb_enet_cmd_buffer[0] == 0x04) {
        rndis->query_msg = (rndis_query_msg_struct *)rndis->usb_enet_cmd.usb_enet_cmd_buffer;
    } else if (rndis->usb_enet_cmd.usb_enet_cmd_buffer[0] == 0x05) {
        rndis->set_msg = (rndis_set_msg_struct *)rndis->usb_enet_cmd.usb_enet_cmd_buffer;
    } else if (rndis->usb_enet_cmd.usb_enet_cmd_buffer[0] == 0x08) {
        rndis->keepalive_msg = (rndis_keepalive_msg_struct *)rndis->usb_enet_cmd.usb_enet_cmd_buffer;
    }
}

/*!
    \brief      handle the CDC ACM class-specific requests
    \param[in]  udev: pointer to USB device instance
    \param[in]  req: device class-specific request
    \param[out] none
    \retval     USB device operation status
*/
static uint8_t rndis_req (usb_dev *udev, usb_req *req)
{
    usb_transc *transc = &udev->dev.transc_in[0];

    usbd_rndis_handler *rndis = (usbd_rndis_handler *)udev->dev.class_data[USBD_RNDIS_INTERFACE];

    switch (req->bRequest) {
    case SEND_ENCAPSULATED_COMMAND:
        
        transc = &udev->dev.transc_out[0];
        transc->remain_len = req->wLength;
        transc->xfer_buf = rndis->usb_enet_cmd.usb_enet_cmd_buffer;
        usbd_ctl_recev (udev);
    
        rndis->usb_enet_cmd.cmd_send = 1;
    
        break;

    case GET_ENCAPSULATED_RESPONSE:
        rndis_msg_parser(udev);

        if (rndis->init_msg->MessageType == 0x00000002) {
            rndis->init_cmplt.MessageLength         = 0x00000034;
            rndis->init_cmplt.RequestId             = rndis->init_msg->RequestId;
            rndis->init_cmplt.Status                = 0x00000000;
            rndis->init_cmplt.MajorVersion          = 0x00000001;
            rndis->init_cmplt.MinorVersion          = 0x00000000;
            rndis->init_cmplt.DeviceFlags           = 0x00000001;
            rndis->init_cmplt.Medium                = 0x00000000;
            rndis->init_cmplt.MaxPacketsPerMessage  = 0x00000001;
            rndis->init_cmplt.MaxTransferSize       = 0x00000616;
            rndis->init_cmplt.PacketAlignmentFactor = 0x00000000;
            rndis->init_cmplt.AFListOffset          = 0x00000000;
            rndis->init_cmplt.AFListSize            = 0x00000000;
            
            transc->remain_len = 52;
            transc->xfer_buf = (uint8_t *)&rndis->init_cmplt;
            usbd_ctl_send (udev);
        } else if (rndis->query_msg->Oid == OID_GEN_SUPPORTED_LIST) {
            rndis->query_cmplt.MessageLength           = 0x00000070;
            rndis->query_cmplt.RequestId               = rndis->query_msg->RequestId;
            rndis->query_cmplt.Status                  = 0x00000000;
            rndis->query_cmplt.InformationBufferLength = 0x00000058;
            rndis->query_cmplt.InformationBufferOffset = 0x00000010;
            rndis->query_cmplt.Oid_Num[0]  = OID_GEN_SUPPORTED_LIST;
            rndis->query_cmplt.Oid_Num[1]  = OID_GEN_HARDWARE_STATUS;
            rndis->query_cmplt.Oid_Num[2]  = OID_GEN_MEDIA_SUPPORTED;
            rndis->query_cmplt.Oid_Num[3]  = OID_GEN_MEDIA_IN_USE;
            rndis->query_cmplt.Oid_Num[4]  = OID_GEN_MAXIMUM_FRAME_SIZE;
            rndis->query_cmplt.Oid_Num[5]  = OID_GEN_LINK_SPEED;
            rndis->query_cmplt.Oid_Num[6]  = OID_GEN_TRANSMIT_BLOCK_SIZE;
            rndis->query_cmplt.Oid_Num[7]  = OID_GEN_RECEIVE_BLOCK_SIZE;
            rndis->query_cmplt.Oid_Num[8]  = OID_GEN_VENDOR_ID;
            rndis->query_cmplt.Oid_Num[9]  = OID_GEN_VENDOR_DESCRIPTION;
            rndis->query_cmplt.Oid_Num[10] = OID_GEN_VENDOR_DRIVER_VERSION;
            rndis->query_cmplt.Oid_Num[11] = OID_GEN_CURRENT_PACKET_FILTER;
            rndis->query_cmplt.Oid_Num[12] = OID_GEN_MAXIMUM_TOTAL_SIZE;
            rndis->query_cmplt.Oid_Num[13] = OID_GEN_PROTOCOL_OPTIONS;
            rndis->query_cmplt.Oid_Num[14] = OID_GEN_MAC_OPTIONS;          /* Optional */
            rndis->query_cmplt.Oid_Num[15] = OID_GEN_MEDIA_CONNECT_STATUS;
            rndis->query_cmplt.Oid_Num[16] = OID_GEN_MAXIMUM_SEND_PACKETS;
            rndis->query_cmplt.Oid_Num[17] = OID_802_3_PERMANENT_ADDRESS;
            rndis->query_cmplt.Oid_Num[18] = OID_802_3_CURRENT_ADDRESS;
            rndis->query_cmplt.Oid_Num[19] = OID_802_3_MULTICAST_LIST;
            rndis->query_cmplt.Oid_Num[20] = OID_802_3_MAXIMUM_LIST_SIZE;
            rndis->query_cmplt.Oid_Num[21] = OID_802_3_MAC_OPTIONS;
            
            transc->remain_len = 112;
            transc->xfer_buf = (uint8_t *)&rndis->query_cmplt;
            usbd_ctl_send (udev);
        } else if (rndis->query_msg->Oid == OID_GEN_VENDOR_DRIVER_VERSION) {
            rndis->query_cmplt.MessageLength           = 0x0000001C;
            rndis->query_cmplt.RequestId               = rndis->query_msg->RequestId;
            rndis->query_cmplt.Status                  = 0x00000000;
            rndis->query_cmplt.InformationBufferLength = 0x00000004;
            rndis->query_cmplt.InformationBufferOffset = 0x00000010;
            rndis->query_cmplt.Oid_Num[0]              = 0x00001000;
            
            transc->remain_len = 28;
            transc->xfer_buf = (uint8_t *)&rndis->query_cmplt;
            usbd_ctl_send (udev);
        } else if (rndis->query_msg->Oid == OID_GEN_MAXIMUM_FRAME_SIZE) {
            rndis->query_cmplt.MessageLength           = 0x0000001C;
            rndis->query_cmplt.RequestId               = rndis->query_msg->RequestId;
            rndis->query_cmplt.Status                  = 0x00000000;
            rndis->query_cmplt.InformationBufferLength = 0x00000004;
            rndis->query_cmplt.InformationBufferOffset = 0x00000010;
            rndis->query_cmplt.Oid_Num[0]              = 0x000005DC;
            
            transc->remain_len = 28;
            transc->xfer_buf = (uint8_t *)&rndis->query_cmplt;
            usbd_ctl_send (udev);
        } else if (rndis->query_msg->Oid == OID_GEN_LINK_SPEED) {
            rndis->query_cmplt.MessageLength           = 0x0000001C;
            rndis->query_cmplt.RequestId               = rndis->query_msg->RequestId;
            rndis->query_cmplt.Status                  = 0x00000000;
            rndis->query_cmplt.InformationBufferLength = 0x00000004;
            rndis->query_cmplt.InformationBufferOffset = 0x00000010;
            rndis->query_cmplt.Oid_Num[0]              = 0x0001D4C0;
            
            transc->remain_len = 28;
            transc->xfer_buf = (uint8_t *)&rndis->query_cmplt;
            usbd_ctl_send (udev);
        } else if (rndis->query_msg->Oid == OID_802_3_MAXIMUM_LIST_SIZE) {
            rndis->query_cmplt.MessageLength           = 0x0000001C;
            rndis->query_cmplt.RequestId               = rndis->query_msg->RequestId;
            rndis->query_cmplt.Status                  = 0x00000000;
            rndis->query_cmplt.InformationBufferLength = 0x00000004;
            rndis->query_cmplt.InformationBufferOffset = 0x00000010;
            rndis->query_cmplt.Oid_Num[0]              = 0x00000001;
            
            transc->remain_len = 28;
            transc->xfer_buf = (uint8_t *)&rndis->query_cmplt;
            usbd_ctl_send (udev);
        } else if (rndis->query_msg->Oid == OID_GEN_MAXIMUM_SEND_PACKETS) {
            rndis->query_cmplt.MessageLength           = 0x00000018;
            rndis->query_cmplt.RequestId               = rndis->query_msg->RequestId;
            rndis->query_cmplt.Status                  = 0xC0000001;
            rndis->query_cmplt.InformationBufferLength = 0x00000000;
            rndis->query_cmplt.InformationBufferOffset = 0x00000010;
            
            transc->remain_len = 24;
            transc->xfer_buf = (uint8_t *)&rndis->query_cmplt;
            usbd_ctl_send (udev);
        }else if (rndis->query_msg->Oid == OID_802_3_CURRENT_ADDRESS) {
            rndis->query_cmplt.MessageLength           = 0x0000001E;
            rndis->query_cmplt.RequestId               = rndis->query_msg->RequestId;
            rndis->query_cmplt.Status                  = 0x00000000;
            rndis->query_cmplt.InformationBufferLength = 0x00000006;
            rndis->query_cmplt.InformationBufferOffset = 0x00000010;
            rndis->query_cmplt.Oid_Num[0]              = 0x0E0F0A02;         // Low four bits of MAC address 
            rndis->query_cmplt.Oid_Num[1]              = 0x0000060D;         // Hight two bits of MAC address 
            
            transc->remain_len = 30;
            transc->xfer_buf = (uint8_t *)&rndis->query_cmplt;
            usbd_ctl_send (udev);
        } else if (rndis->query_msg->Oid == OID_802_3_PERMANENT_ADDRESS) {
            rndis->query_cmplt.MessageLength           = 0x0000001E;
            rndis->query_cmplt.RequestId               = rndis->query_msg->RequestId;
            rndis->query_cmplt.Status                  = 0x00000000;
            rndis->query_cmplt.InformationBufferLength = 0x00000006;
            rndis->query_cmplt.InformationBufferOffset = 0x00000010;
            rndis->query_cmplt.Oid_Num[0]              = 0x0E0F0A02;         // Low four bits of MAC address 
            rndis->query_cmplt.Oid_Num[1]              = 0x0000060D;         // Hight two bits of MAC address 
            
            transc->remain_len = 30;
            transc->xfer_buf = (uint8_t *)&rndis->query_cmplt;
            usbd_ctl_send (udev);
        } else if (rndis->query_msg->Oid == OID_GEN_PHYSICAL_MEDIUM) {
            rndis->query_cmplt.MessageLength           = 0x0000001C;
            rndis->query_cmplt.RequestId               = rndis->query_msg->RequestId;
            rndis->query_cmplt.Status                  = 0x00000000;
            rndis->query_cmplt.InformationBufferLength = 0x00000004;
            rndis->query_cmplt.InformationBufferOffset = 0x00000010;
            rndis->query_cmplt.Oid_Num[0]              = 0x00000000;
            
            transc->remain_len = 28;
            transc->xfer_buf = (uint8_t *)&rndis->query_cmplt;
            usbd_ctl_send (udev);
        } else if (rndis->query_msg->Oid == OID_GEN_MEDIA_CONNECT_STATUS) {
            rndis->query_cmplt.MessageLength           = 0x0000001C;
            rndis->query_cmplt.RequestId               = rndis->query_msg->RequestId;
            rndis->query_cmplt.Status                  = 0x00000000;
            rndis->query_cmplt.InformationBufferLength = 0x00000004;
            rndis->query_cmplt.InformationBufferOffset = 0x00000010;
            rndis->query_cmplt.Oid_Num[0]              = 0x00000000;
            
            transc->remain_len = 28;
            transc->xfer_buf = (uint8_t *)&rndis->query_cmplt;
            usbd_ctl_send (udev);
        } else if (rndis->query_msg->Oid == OID_GEN_MAC_OPTIONS) {
            rndis->query_cmplt.MessageLength           = 0x0000001C;
            rndis->query_cmplt.RequestId               = rndis->query_msg->RequestId;
            rndis->query_cmplt.Status                  = 0x00000000;
            rndis->query_cmplt.InformationBufferLength = 0x00000004;
            rndis->query_cmplt.InformationBufferOffset = 0x00000010;
            rndis->query_cmplt.Oid_Num[0]              = 0x00000000;
            
            transc->remain_len = 28;
            transc->xfer_buf = (uint8_t *)&rndis->query_cmplt;
            usbd_ctl_send (udev);
        } else if (rndis->query_msg->Oid == OID_GEN_MAXIMUM_TOTAL_SIZE) {
            rndis->query_cmplt.MessageLength           = 0x0000001C;
            rndis->query_cmplt.RequestId               = rndis->query_msg->RequestId;
            rndis->query_cmplt.Status                  = 0x00000000;
            rndis->query_cmplt.InformationBufferLength = 0x00000004;
            rndis->query_cmplt.InformationBufferOffset = 0x00000010;
            rndis->query_cmplt.Oid_Num[0]              = 0x000005EA;
            
            transc->remain_len = 28;
            transc->xfer_buf = (uint8_t *)&rndis->query_cmplt;
            usbd_ctl_send (udev);
        } else if (rndis->query_msg->Oid == OID_GEN_XMIT_OK) {
            rndis->query_cmplt.MessageLength           = 0x00000048;
            rndis->query_cmplt.RequestId               = rndis->query_msg->RequestId;
            rndis->query_cmplt.Status                  = 0x00000000;
            rndis->query_cmplt.InformationBufferLength = 0x00000004;
            rndis->query_cmplt.InformationBufferOffset = 0x00000010;
            rndis->query_cmplt.Oid_Num[0]              = 0x00000001;         /* ?????? */
            rndis->query_cmplt.Oid_Num[1]              = 0x00000000;
            rndis->query_cmplt.Oid_Num[2]              = 0x05809990;
            rndis->query_cmplt.Oid_Num[3]              = 0xFFFFF800;
            rndis->query_cmplt.Oid_Num[4]              = 0x07C70DA0;
            rndis->query_cmplt.Oid_Num[5]              = 0xFFFFFA80;
            rndis->query_cmplt.Oid_Num[6]              = 0x0000000A;
            rndis->query_cmplt.Oid_Num[7]              = 0x00000000;
            rndis->query_cmplt.Oid_Num[8]              = 0x00000001;
            rndis->query_cmplt.Oid_Num[9]              = 0x00000001;
            rndis->query_cmplt.Oid_Num[10]             = 0x07C70DB8;
            rndis->query_cmplt.Oid_Num[11]             = 0xFFFFFA80;
            
            transc->remain_len = 72;
            transc->xfer_buf = (uint8_t *)&rndis->query_cmplt;
            usbd_ctl_send (udev);
        } else if (rndis->query_msg->Oid == OID_GEN_RCV_OK) {
            rndis->query_cmplt.MessageLength           = 0x00000048;
            rndis->query_cmplt.RequestId               = rndis->query_msg->RequestId;
            rndis->query_cmplt.Status                  = 0x00000000;
            rndis->query_cmplt.InformationBufferLength = 0x00000004;
            rndis->query_cmplt.InformationBufferOffset = 0x00000010;
            rndis->query_cmplt.Oid_Num[0]              = 0x00000001;
            rndis->query_cmplt.Oid_Num[1]              = 0x00000000;
            rndis->query_cmplt.Oid_Num[2]              = 0x05809990;
            rndis->query_cmplt.Oid_Num[3]              = 0xFFFFF800;
            rndis->query_cmplt.Oid_Num[4]              = 0x07C70DA0;
            rndis->query_cmplt.Oid_Num[5]              = 0xFFFFFA80;
            rndis->query_cmplt.Oid_Num[6]              = 0x0000000A;
            rndis->query_cmplt.Oid_Num[7]              = 0x00000000;
            rndis->query_cmplt.Oid_Num[8]              = 0x00000001;
            rndis->query_cmplt.Oid_Num[9]              = 0x00000001;
            rndis->query_cmplt.Oid_Num[10]             = 0x07C70DB8;
            rndis->query_cmplt.Oid_Num[11]             = 0xFFFFFA80;
            
            transc->remain_len = 72;
            transc->xfer_buf = (uint8_t *)&rndis->query_cmplt;
            usbd_ctl_send (udev);
        } else if (rndis->set_msg->Oid == OID_GEN_CURRENT_PACKET_FILTER) {
            rndis->set_cmplt.MessageLength           = 0x00000010;
            rndis->set_cmplt.RequestId               = rndis->set_msg->RequestId;
            rndis->set_cmplt.Status                  = 0x00000000;
            
            transc->remain_len = 16;
            transc->xfer_buf = (uint8_t *)&rndis->set_cmplt;
            usbd_ctl_send (udev);
            
            rndis_data_receive(udev);
            
        } else if (rndis->set_msg->Oid == OID_802_3_MULTICAST_LIST) {
            rndis->set_cmplt.MessageLength           = 0x00000010;
            rndis->set_cmplt.RequestId               = rndis->set_msg->RequestId;
            rndis->set_cmplt.Status                  = 0x00000000;
            
            transc->remain_len = 16;
            transc->xfer_buf = (uint8_t *)&rndis->set_cmplt;
            usbd_ctl_send (udev);

//            rndis_data_receive(udev);

        } else if (rndis->keepalive_msg->MessageType == 0x00000008) {
            rndis->keepalive_cmplt.MessageLength           = 0x00000010;
            rndis->keepalive_cmplt.RequestId               = rndis->keepalive_msg->RequestId;
            rndis->keepalive_cmplt.Status                  = 0x00000000;
            
            transc->remain_len = 16;
            transc->xfer_buf = (uint8_t *)&rndis->keepalive_cmplt;
            usbd_ctl_send (udev);
        } 
        
        memset(rndis->usb_enet_cmd.usb_enet_cmd_buffer , 0x00, 20);
        
        break;

    case SET_COMM_FEATURE:
        /* no operation for this driver */
        break;

    case GET_COMM_FEATURE:
        /* no operation for this driver */
        break;

    case CLEAR_COMM_FEATURE:
        /* no operation for this driver */
        break;

    case SET_LINE_CODING:
        /* no operation for this driver */
        break;

    case GET_LINE_CODING:
        /* no operation for this driver */
        break;

    case SET_CONTROL_LINE_STATE:
        /* no operation for this driver */
        break;

    case SEND_BREAK:
        /* no operation for this driver */
        break;

    default:
        break;
    }

    return USBD_OK;
}

/*!
    \brief      handle CDC ACM data
    \param[in]  udev: pointer to USB device instance
    \param[in]  ep_num: endpoint identifier
    \param[out] none
    \retval     USB device operation status
*/
static uint8_t rndis_in (usb_dev *udev, uint8_t ep_num)
{
    usb_transc *transc = &udev->dev.transc_in[EP_ID(ep_num)];
    usbd_rndis_handler *rndis = (usbd_rndis_handler *)udev->dev.class_data[USBD_RNDIS_INTERFACE];
    
    if ((transc->xfer_len % transc->max_len == 0) && (transc->xfer_len != 0)) {
        usbd_ep_send (udev, ep_num, NULL, 0);
    } else {
        rndis->usb_to_enet.packet_sent = 1;
    }

    return USBD_OK;
}

/*!
    \brief      handle CDC ACM data
    \param[in]  udev: pointer to USB device instance
    \param[in]  ep_num: endpoint identifier
    \param[out] none
    \retval     USB device operation status
*/
static uint8_t rndis_out (usb_dev *udev, uint8_t ep_num)
{
    usbd_rndis_handler *rndis = (usbd_rndis_handler *)udev->dev.class_data[USBD_RNDIS_INTERFACE];

    rndis->usb_to_enet.receive_length = ((usb_core_driver *)udev)->dev.transc_out[ep_num].xfer_count;

    rndis_data_receive(udev);

    return USBD_OK;
}

/*!
    \brief      send RNDIS Command
    \param[in]  udev: pointer to USB device instance
    \param[out] none
    \retval     USB device operation status
*/
void rndis_cmd_send (usb_dev *udev, uint32_t len)
{
    usbd_rndis_handler *rndis = (usbd_rndis_handler *)udev->dev.class_data[USBD_RNDIS_INTERFACE];

    if (len <= USB_RNDIS_DATA_PACKET_SIZE) {
        usbd_ep_send (udev, RNDIS_CMD_EP, (uint8_t*)(rndis->usb_enet_cmd.usb_enet_response_available_buffer), len);
    }
}

/*!
    \brief      send CDC ACM data
    \param[in]  udev: pointer to USB device instance
    \param[out] none
    \retval     USB device operation status
*/
void rndis_data_send (usb_dev *udev, uint32_t len)
{
    usbd_rndis_handler *rndis = (usbd_rndis_handler *)udev->dev.class_data[USBD_RNDIS_INTERFACE];
    
    rndis->usb_to_enet.packet_sent = 0;

    usbd_ep_send (udev, RNDIS_DATA_IN_EP, rndis->enet_to_usb.ENET_USB_data_buffer, len);
}

static void rndis_data_msg_handle (usb_dev *udev)
{
    usbd_rndis_handler *rndis = (usbd_rndis_handler *)udev->dev.class_data[USBD_RNDIS_INTERFACE];

    if (rndis->usb_to_enet.receive_length > 0) {
        if ((rndis->usb_to_enet.usb_data_buffer[0] == 0x01) && (rndis->usb_to_enet.usb_data_buffer[8] == 0x24)) {
             rndis->package_msg = (rndis_package_msg_struct *)rndis->usb_to_enet.usb_data_buffer;
            
            if ((rndis->package_msg->MessageLength - rndis->package_msg->DataLength == 0x2C)){
                rndis->usb_to_enet.usb_data_len = rndis->package_msg->DataLength;

                rndis->usb_to_enet.USB_enet_data_length = rndis->usb_to_enet.receive_length - 44;

                memcpy(rndis->usb_to_enet.USB_enet_data_buffer, &rndis->usb_to_enet.usb_data_buffer[44], rndis->usb_to_enet.USB_enet_data_length);
            }
        } else {
            memcpy(rndis->usb_to_enet.USB_enet_data_buffer + rndis->usb_to_enet.USB_enet_data_length, rndis->usb_to_enet.usb_data_buffer, rndis->usb_to_enet.receive_length);

            rndis->usb_to_enet.USB_enet_data_length += rndis->usb_to_enet.receive_length;
        }
    }

    
    if ((rndis->usb_to_enet.USB_enet_data_length == rndis->usb_to_enet.usb_data_len) && (rndis->usb_to_enet.USB_enet_data_length > 0)) {
        low_level_output(rndis->usb_to_enet.USB_enet_data_buffer, rndis->usb_to_enet.USB_enet_data_length);

        rndis->usb_to_enet.USB_enet_data_length = 0;

        rndis->usb_to_enet.receive_length = 0;

    }
}

/*!
    \brief      receive CDC ACM data
    \param[in]  udev: pointer to USB device instance
    \param[out] none
    \retval     USB device operation status
*/
void rndis_data_receive (usb_dev *udev)
{
    usbd_rndis_handler *rndis = (usbd_rndis_handler *)udev->dev.class_data[USBD_RNDIS_INTERFACE];

    usbd_ep_recev(udev, RNDIS_DATA_OUT_EP, (uint8_t*)(rndis->usb_to_enet.usb_data_buffer), USB_RNDIS_DATA_PACKET_SIZE);
    
    rndis_data_msg_handle(udev);
}
