/** @file Dsdef.h
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#ifndef _DSDEF_H
#define _DSDEF_H
#include <winioctl.h>
#define  IOCTL_GET_DEVICE_DESCRIPTOR CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   0x801,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)
#define  IOCTL_GET_CONFIGURATION_DESCRIPOR CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   0x802,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)
#define  IOCTL_GET_INTERFACE_DESCRIPTOR CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   0x803,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)
#define  IOCTL_GET_ENDPOINT_INFORMATION CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   0x804,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)
#define  IOCTL_VENDOR_COMMAND_READ CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   0x805,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)
#define  IOCTL_VENDOR_COMMAND_WRITE CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   0x806,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)
#define  IOCTL_BULK_READ CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   0x807,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)
#define  IOCTL_BULK_WRITE CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   0x808,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)
#define  IOCTL_GET_ENDPOINT_NUMBER CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   0x809,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)
#define  IOCTL_SET_BULK_INDEX CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   0x80A,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)                                                   

#define  IOCTL_CONTROL_LOOP_BACK_ENABLE CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   0x80B,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)  
#define  IOCTL_CONTROL_LOOP_BACK_DISABLE CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   0x80C,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)  


#define Vendor_Read_MAC_Reg       0
#define Vendor_Read_BBU_Reg       1
#define Vendor_Read_RF_Reg        2
#define Vendor_Write_MAC_Reg      3
#define Vendor_Write_BBU_Reg      4
#define Vendor_Write_RF_Reg       5
#define Vendor_Control_LoopBack_Write  7
#define Vendor_Control_LoopBack_Read   8


#define CR 0x0D
#define LF 0x0A

typedef PVOID USBD_PIPE_HANDLE;

typedef enum _USBD_PIPE_TYPE {
    UsbdPipeTypeControl,
    UsbdPipeTypeIsochronous,
    UsbdPipeTypeBulk,
    UsbdPipeTypeInterrupt
} USBD_PIPE_TYPE;


typedef struct _USB_CONFIGURATION_DESCRIPTOR {
    UCHAR bLength;
    UCHAR bDescriptorType;
    USHORT wTotalLength;
    UCHAR bNumInterfaces;
    UCHAR bConfigurationValue;
    UCHAR iConfiguration;
    UCHAR bmAttributes;
    UCHAR MaxPower;
} USB_CONFIGURATION_DESCRIPTOR, *PUSB_CONFIGURATION_DESCRIPTOR;

typedef struct _USB_INTERFACE_DESCRIPTOR {
    UCHAR bLength;
    UCHAR bDescriptorType;
    UCHAR bInterfaceNumber;
    UCHAR bAlternateSetting;
    UCHAR bNumEndpoints;
    UCHAR bInterfaceClass;
    UCHAR bInterfaceSubClass;
    UCHAR bInterfaceProtocol;
    UCHAR iInterface;
} USB_INTERFACE_DESCRIPTOR, *PUSB_INTERFACE_DESCRIPTOR;

typedef struct _USB_DEVICE_DESCRIPTOR {
    UCHAR bLength;
    UCHAR bDescriptorType;
    USHORT bcdUSB;
    UCHAR bDeviceClass;
    UCHAR bDeviceSubClass;
    UCHAR bDeviceProtocol;
    UCHAR bMaxPacketSize0;
    USHORT idVendor;
    USHORT idProduct;
    USHORT bcdDevice;
    UCHAR iManufacturer;
    UCHAR iProduct;
    UCHAR iSerialNumber;
    UCHAR bNumConfigurations;
} USB_DEVICE_DESCRIPTOR, *PUSB_DEVICE_DESCRIPTOR;

typedef struct _USBD_PIPE_INFORMATION {
    USHORT MaximumPacketSize; 
    UCHAR EndpointAddress;    
    UCHAR Interval;          
    USBD_PIPE_TYPE PipeType; 
    USBD_PIPE_HANDLE PipeHandle;
    ULONG MaximumTransferSize; 
    ULONG PipeFlags;
} USBD_PIPE_INFORMATION, *PUSBD_PIPE_INFORMATION;


typedef struct _USB_ENDPOINT_DESCRIPTOR {
    UCHAR bLength;
    UCHAR bDescriptorType;
    UCHAR bEndpointAddress;
    UCHAR bmAttributes;
    USHORT wMaxPacketSize;
    UCHAR bInterval;
} USB_ENDPOINT_DESCRIPTOR, *PUSB_ENDPOINT_DESCRIPTOR;
//total Packet size is 2048 bytes
typedef struct _TagIO_PACKET
{
 ULONG Length;
 UCHAR  Data[4096];	
}IOPACKET, *PIOPACKET;

typedef struct _tagCONFIGURATION_DESCRIPTOR
{
 ULONG 	Length;
 UCHAR  Index;
 UCHAR  Data[4095];
}CFGPACKET, *PCFGPACKET;

typedef struct _tagINTERFACE_DESCRIPTOR
{
  ULONG Length;
  UCHAR Index;
  UCHAR intindex;
  UCHAR Data[4094];	
}INTFSPACKET, *PINTFSPACKET;

typedef struct _tagENDPOINT_INFORMATION
{
  ULONG Length;
  UCHAR Index;
  UCHAR intindex;
  UCHAR Epindex;
  UCHAR Data[4093];	
}EPPACKET, *PEPPACKET;

typedef struct _tagVENDOR_CMD{
UCHAR Request;
ULONG Offset;
}VENDORCMD,*PVENDORCMD;

typedef struct _tagRegister {
   UCHAR  Type;
   ULONG  RegOffset;
   ULONG  RegVale;
} REGDATA, *PREGDATA;

#endif
