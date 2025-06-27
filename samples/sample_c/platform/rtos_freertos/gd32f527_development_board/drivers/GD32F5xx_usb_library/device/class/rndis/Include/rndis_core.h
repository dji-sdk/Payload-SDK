/*!
    \file  rndis_core.h
    \brief the header file of rndis driver

    \version 2018-10-08, V1.0.0, firmware for GD32 USBFS&USBHS
*/

/*
    Copyright (c) 2018, GigaDevice Semiconductor Inc.

    All rights reserved.

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

#ifndef __RNDIS_CORE_H
#define __RNDIS_CORE_H

#include "usbd_enum.h"
#include "usb_rndis.h"

typedef struct
{
    uint32_t MessageType;       /*!< Specifies the type of message being sent. Set to 0x00000002. */
    uint32_t MessageLength;     /*!< Specifies in bytes the total length of this message, from the beginning of the message. */
    uint32_t RequestId;         /*!< Specifies the Remote NDIS message ID value. This value is used to match messages sent by the host with device responses. */
    uint32_t MajorVersion;      /*!< Specifies the Remote NDIS protocol version implemented by the host. The current specification uses RNDIS_MAJOR_VERSION = 1. */
    uint32_t MinorVersion;      /*!< Specifies the Remote NDIS protocol version implemented by the host. The current specification uses RNDIS_MINOR_VERSION = 0. */
    uint32_t MaxTransferSize;   /*!< Specifies the maximum size in bytes of any single bus data transfer that the host expects 
                                     to receive from the device. Typically, each bus data transfer accommodates a single Remote 
                                     NDIS message. However, the device may bundle several Remote NDIS messages that contain data 
                                     packets into a single transfer (see REMOTE_NDIS_PACKET_MSG). */
}rndis_initialize_msg_struct;

typedef struct
{
    uint32_t MessageType;            /*!< Specifies the type of message being sent. Set to 0x80000002. */
    uint32_t MessageLength;          /*!< Specifies in bytes the total length of this message, from the beginning of the message. */
    uint32_t RequestId;              /*!< Specifies the Remote NDIS message ID value. This value is used to match messages sent by the host with device responses. */
    uint32_t Status;                 /*!< Specifies RNDIS_STATUS_SUCCESS if the device initialized successfully; otherwise, it specifies an error code that indicates the failure. */
    uint32_t MajorVersion;           /*!< Specifies the highest Remote NDIS major protocol version supported by the device. */
    uint32_t MinorVersion;           /*!< Specifies the highest Remote NDIS minor protocol version supported by the device. */
    uint32_t DeviceFlags;            /*!< Specifies the miniport driver type as either connectionless or connection-oriented. This value can be one of the following:
                                          RNDIS_DF_CONNECTIONLESS 0x00000001      RNDIS_DF_CONNECTION_ORIENTED 0x00000002 */
    uint32_t Medium;                 /*!< Specifies the medium supported by the device. Set to RNDIS_MEDIUM_802_3 (0x00000000) */
    uint32_t MaxPacketsPerMessage;   /*!< Specifies the maximum number of Remote NDIS data messages that the device can handle in a single transfer to it. This value should be at least one. */
    uint32_t MaxTransferSize;        /*!< Specifies the maximum size in bytes of any single bus data transfer that the device expects to receive from the host. */
    uint32_t PacketAlignmentFactor;  /*!< Specifies the byte alignment that the device expects for each Remote NDIS message 
                                          that is part of a multimessage transfer to it. This value is specified in powers of 2. 
                                          For example, this value is set to three to indicate 8-byte alignment. This value has a 
                                          maximum setting of seven, which specifies 128-byte alignment. */
    uint32_t AFListOffset;           /*!< Reserved for connection-oriented devices. Set value to zero. */
    uint32_t AFListSize;             /*!< Reserved for connection-oriented devices. Set value to zero. */
}rndis_initialize_cmplt_struct;

typedef struct
{
    uint32_t MessageType;              /*!< Specifies the type of message being sent. Set to 0x00000004. */
    uint32_t MessageLength;            /*!< Specifies in bytes the total length of this message, from the beginning of the message. */
    uint32_t RequestId;                /*!< Specifies the Remote NDIS message ID value. This value is used to match messages sent by the host with device responses. */
    uint32_t Oid;                      /*!< Specifies the NDIS OID that identifies the parameter being queried. */
    uint32_t InformationBufferLength;  /*!< Specifies in bytes the length of the input data for the query. Set to zero when there is no OID input buffer. */
    uint32_t InformationBufferOffset;  /*!< Specifies the byte offset, from the beginning of the RequestId field, at which input data for the query is located. Set to zero if there is no OID input buffer. */
    uint32_t DeviceVcHandle;           /*!< Reserved for connection-oriented devices. Set to zero. */
}rndis_query_msg_struct;

typedef struct
{
    uint32_t MessageType;              /*!< Specifies the type of message being sent. Set to 0x80000004. */
    uint32_t MessageLength;            /*!< Specifies in bytes the total length of this message, from the beginning of the message. */
    uint32_t RequestId;                /*!< Specifies the Remote NDIS message ID value. This value is used to match messages sent by the host with device responses. */
    uint32_t Status;                   /*!< Specifies the status of processing the OID query request. */
    uint32_t InformationBufferLength;  /*!< Specifies, in bytes, the length of the response data for the query. Set to zero when there is no OID result buffer. */
    uint32_t InformationBufferOffset;  /*!< Specifies the byte offset, from the beginning of the RequestId field, at which response data for the query is located. Set to zero if there is no response data. */
    uint32_t Oid_Num[28];
}rndis_query_cmplt_struct;

typedef struct
{
    uint32_t MessageType;              /*!< Specifies the type of message being sent. Set to 0x00000005. */
    uint32_t MessageLength;            /*!< Specifies, in bytes, the total length of this message, from the beginning of the message. */
    uint32_t RequestId;                /*!< Specifies the Remote NDIS message ID value. This value is used to match messages sent by the host with device responses. */
    uint32_t Oid;                      /*!< Specifies the NDIS OID that identifies the parameter being set. */
    uint32_t InformationBufferLength;  /*!< Specifies, in bytes, the length of the input data for the request. */
    uint32_t InformationBufferOffset;  /*!< Specifies the byte offset, from the beginning of the RequestId field, at which input data for the request is located. */
    uint32_t DeviceVcHandle;           /*!< Reserved for connection-oriented devices. Set to zero. */
}rndis_set_msg_struct;

typedef struct
{
    uint32_t MessageType;              /*!< Specifies the type of message being sent. Set to 0x80000005. */
    uint32_t MessageLength;            /*!< Specifies, in bytes, the total length of this message, from the beginning of the message. */
    uint32_t RequestId;                /*!< Specifies the Remote NDIS message ID value. This value is copied from the REMOTE_NDIS_SET_MSG being responded to. */
    uint32_t Status;                   /*!< Specifies the status of processing the OID set request. */
    uint32_t Oid_Num[4];
}rndis_set_cmplt_struct;

typedef struct
{
    uint32_t MessageType;              /*!< Specifies the type of message being sent. Set to 0x00000008. */
    uint32_t MessageLength;            /*!< Specifies in bytes the total length of this message, from the beginning of the message. */
    uint32_t RequestId;                /*!< Specifies the Remote NDIS message ID value. This value is used to match messages sent by the host with device responses. */
}rndis_keepalive_msg_struct;

typedef struct
{
    uint32_t MessageType;              /*!< Specifies the type of message being sent. Set to 0x80000008. */
    uint32_t MessageLength;            /*!< Specifies, in bytes, the total length of this message, from the beginning of the message. */
    uint32_t RequestId;                /*!< Specifies the Remote NDIS message ID value. This value is used to match messages sent by the host with device responses. */
    uint32_t Status;                   /*!< Specifies the current status of the device. If the returned Status is not RNDIS_STATUS_SUCCESS, 
                                            the host will send an REMOTE_NDIS_RESET_MSG message to reset the device. */
}rndis_keepalive_cmplt_struct;

typedef struct
{
    uint32_t MessageType;              /*!< Specifies the type of message being sent. Set to 0x1. */
    uint32_t MessageLength;            /*!< Message length in bytes, including appended packet data, OOB data, per-packet information data, and both internal and external padding. */
    uint32_t DataOffset;               /*!< Specifies the offset in bytes from the start of the DataOffset field of this message to the start of the data. This is an integer multiple of 4. */
    uint32_t DataLength;               /*!< Specifies the number of bytes in the data content of this message. */
    uint32_t OOBDataOffset;            /*!< Specifies the offset in bytes of the first OOB data record from the start of the DataOffset field of this message. 
                                            Set to zero if there is no OOB data. Otherwise, this is an integer multiple of 4. */
    uint32_t OOBDataLength;            /*!< Specifies in bytes the total length of the OOB data. */
    uint32_t NumOOBDataElements;       /*!< Specifies the number of OOB records in this message. */
    uint32_t PerPacketInfoOffset;      /*!< Specifies in bytes the offset from the beginning of the DataOffset field in the REMOTE_NDIS_PACKET_MSG data message to the start of the first per-packet information data record. 
                                            Set to zero if there is no per-packet data. Otherwise, this is an integer multiple of 4. */
    uint32_t PerPacketInfoLength;      /*!< Specifies in bytes the total length of the per-packet information contained in this message. */
    uint32_t VcHandle;                 /*!< Reserved for connection-oriented devices. Set to zero. */
    uint32_t Reserved;                 /*!< Reserved. Set to zero. */
}rndis_package_msg_struct;

typedef struct {
    uint8_t  packet_sent;
    uint32_t usb_data_len;
    uint32_t receive_length;
    uint32_t USB_enet_data_length;
    uint8_t  usb_data_buffer[64];
    uint8_t  USB_enet_data_buffer[1000];
} USB_TO_ENET_DATA;

typedef struct {
    uint32_t ENET_receive_length;
    uint8_t ENET_USB_data_buffer[2000];
} ENET_TO_USB_DATA;

typedef struct {
    uint8_t cmd_send;
    uint8_t usb_enet_response_available_buffer[8];
    uint8_t usb_enet_cmd_buffer[100];
} USB_ENET_CMD;

typedef struct _usbd_rndis_handler
{
    USB_ENET_CMD usb_enet_cmd;
    USB_TO_ENET_DATA usb_to_enet;
    ENET_TO_USB_DATA enet_to_usb;

    rndis_initialize_msg_struct   *init_msg;
    rndis_initialize_cmplt_struct  init_cmplt;
    rndis_query_msg_struct        *query_msg;
    rndis_query_cmplt_struct       query_cmplt;
    rndis_set_msg_struct          *set_msg;
    rndis_set_cmplt_struct         set_cmplt;
    rndis_keepalive_msg_struct    *keepalive_msg;
    rndis_keepalive_cmplt_struct   keepalive_cmplt;
    rndis_package_msg_struct      *package_msg;
    rndis_package_msg_struct       package_return_msg;
}usbd_rndis_handler;

extern usb_desc rndis_desc;
extern usb_class_core rndis_class;

/* function declarations */
/* send RNDIS CMD */
void rndis_cmd_send (usb_dev *udev, uint32_t len);

/* receive RNDIS data */
void rndis_data_receive(usb_dev *udev);

/* send RNDIS data */
void rndis_data_send(usb_dev *udev, uint32_t data_len);

#endif  /* __RNDIS_CORE_H */

