/** @file ioctl.h
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#ifndef _usbioctl_h_
#define _usbioctl_h_

// Custom OID's
                           // backward compatible with initial UB-22
#define OID_CUS_DNLD_STATUS      0x01010108
#define OID_CUS_DNLD_SEG         0x01010109
#define OID_CUS_CMD              0x0101010b
#define OID_CUS_CMD_RESP         0x0101010c
#define OID_CUS_RESET_HOST       0x0101010d   // reset usb host
#define OID_CUS_DNLD_BOOT_FBURN  0x0101010a

#define OID_CUS_PEEK             0xffbeef04
#define OID_CUS_POKE             0xffbeef05
#define OID_CUS_DUMP_INT         0xffbeef06
#define OID_CUS_TRACE            0xffbeef07

//          Marvel specific OIDs
#define OID_MRVL_OEM_GET_ULONG                  0xff010201
#define OID_MRVL_OEM_SET_ULONG                  0xff010202
#define OID_MRVL_OEM_GET_STRING	                0xff010203
#define OID_MRVL_OEM_COMMAND                    0xff010204
#ifdef _IF_USB_ 
#define OID_MRVL_MFG_COMMAND					0xfe000001
#else
#define OID_MRVL_MFG_COMMAND					0xff010205
#endif
 

//#define OID_MRVL_USB_REG_GET           0xff010210
//#define OID_MRVL_USB_REG_SET           0xff010211
//#define OID_MRVL_USB_DEBUG_TRACE       0xff010212
//#define OID_MRVL_USB_DEBUG_LEVEL_GET   0xff010213
//#define OID_MRVL_USB_DEBUG_LEVEL_SET   0xff010214
//#define OID_MRVL_USB_DEBUG_DUMP        0xff010215



// Types for debug tracing
#define TRACE_QUERY     '?'
#define TRACE_SET       '='

// segment of firmware file
#define SEGMENT_HDR_SZ  sizeof(DWORD)
#define SEGMENT_BODY_SZ 512
#define SEGMENT_SZ      SEGMENT_HDR_SZ + SEGMENT_BODY_SZ

// segment header field
#define SEGMENT_FIRST   0x00000001
#define SEGMENT_LAST    0x00000002
#define SEGMENT_MODULE  0x0000001c  // mask of module type
/*
typedef struct _seg
{
   DWORD hdr;
   BYTE  body[0];
} seg_t;

// addr, data struct
typedef struct
{
   DWORD addr;
   DWORD data;
} addr_data_t;

// length, byte array struct
typedef struct
{
   DWORD len;
   unsigned char byte[0];
} len_byte_t;

// debug dump: indices of DWORD to be copied over by corresponding variables
typedef enum
{
   idx_ulNumRndisIntialize_g,
   idx_ulNumRndisHalt_g,
   idx_ulNumRndisQuery_g,
   idx_ulNumGenSupportedListQuery_g,
   idx_ulNumGenHardwareStatusQuery_g,
   idx_ulNumQueryUnHandled_g,
   idx_ulNumSetReq_g,
   idx_ulNumUnHandledSetReq_g,
   idx_ulNumRndisReset_g,
   idx_ulNumKpAliveReq_g,
   idx_ulNumMIndicateSts_g,
   idx_total
} dump_int;
*/
#endif   // _ioctl_h_
