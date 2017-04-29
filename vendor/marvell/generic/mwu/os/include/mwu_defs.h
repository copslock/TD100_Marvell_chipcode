/** @file mwu_defs.h
 *
 * Copyright (C) 2011 Marvell International Ltd., All Rights Reserved
 *
 * MARVELL CONFIDENTIAL
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Marvell International Ltd or its
 * suppliers or licensors. Title to the Material remains with Marvell International Ltd
 * or its suppliers and licensors. The Material contains trade secrets and
 * proprietary and confidential information of Marvell or its suppliers and
 * licensors. The Material is protected by worldwide copyright and trade secret
 * laws and treaty provisions. No part of the Material may be used, copied,
 * reproduced, modified, published, uploaded, posted, transmitted, distributed,
 * or disclosed in any way without Marvell's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise. Any license under such intellectual property rights must be
 * express and approved by Marvell in writing.
 *
 */

#ifndef __MWU_DEFS_H__
#define __MWU_DEFS_H__

#ifdef OS_LINUX
#include <linux/if.h>  /* for IFNAMSIZ */
#include <asm/types.h>
#include <linux/if_ether.h>

/** Unsigned character, 1 byte */
#define u8     __u8
/** Unsigned character, 2 byte */
#define u16    __u16
/** Unsigned character, 4 byte */
#define u32    __u32
/** Unsigned character, 8 byte */
#define u64    __u64
/** Signed character, 1 byte */
#define s8     __s8
/** Signed character, 2 byte */
#define s16    __s16
/** Signed character, 4 byte */
#define s32    __s32
/** Signed character, 8 byte */
#define s64    __s64

#endif /* OS_LINUX */

#ifdef OS_WINDOWS
#endif

#ifndef IFNAMSIZ
#define IFNAMSIZ 16  /* from Linux if.h. TODO: what is appropriate value in Windows? */
#endif

#ifndef ETH_ALEN
#define ETH_ALEN 6
#endif

/** 16 bits byte swap */
#define swap_byte_16(x) \
  ((u16)((((u16)(x) & 0x00ffU) << 8) | \
         (((u16)(x) & 0xff00U) >> 8)))

/** 32 bits byte swap */
#define swap_byte_32(x) \
  ((u32)((((u32)(x) & 0x000000ffUL) << 24) | \
         (((u32)(x) & 0x0000ff00UL) <<  8) | \
         (((u32)(x) & 0x00ff0000UL) >>  8) | \
         (((u32)(x) & 0xff000000UL) >> 24)))
/*
 *  ctype from older glib installations defines BIG_ENDIAN.  Check it
 *   and undef it if necessary to correctly process the wlan header
 *   files
 */
#if (BYTE_ORDER == LITTLE_ENDIAN)
#undef BIG_ENDIAN
#endif

#ifdef BIG_ENDIAN
#define mwu_ntohl(x) x
#define mwu_htonl(x) x
#define mwu_ntohs(x) x
#define mwu_htons(x) x
/** Convert from 16 bit little endian format to CPU format */
#define wlan_le16_to_cpu(x) swap_byte_16(x)
/** Convert from 32 bit little endian format to CPU format */
#define wlan_le32_to_cpu(x) swap_byte_32(x)
/** Convert to 16 bit little endian format from CPU format */
#define wlan_cpu_to_le16(x) swap_byte_16(x)
/** Convert to 32 bit little endian format from CPU format */
#define wlan_cpu_to_le32(x) swap_byte_32(x)
/** Convert WIFIDIRCMD header to little endian format from CPU format */
#define endian_convert_request_header(x);               \
    {                                                   \
        (x).cmd_code = wlan_cpu_to_le16((x).cmd_code);   \
        (x).size = wlan_cpu_to_le16((x).size);         \
        (x).seq_num = wlan_cpu_to_le16((x).seq_num);     \
        (x).result = wlan_cpu_to_le16((x).result);     \
    }

/** Convert WIFIDIRCMD header from little endian format to CPU format */
#define endian_convert_response_header(x);              \
    {                                                   \
        (x).cmd_code = wlan_le16_to_cpu((x).cmd_code);   \
        (x).size = wlan_le16_to_cpu((x).size);         \
        (x).seq_num = wlan_le16_to_cpu((x).seq_num);     \
        (x).result = wlan_le16_to_cpu((x).result);     \
    }
#else /* BIG_ENDIAN */
#define mwu_ntohl(x) swap_byte_32(x)
#define mwu_htonl(x) swap_byte_32(x)
#define mwu_ntohs(x) swap_byte_16(x)
#define mwu_htons(x) swap_byte_16(x)
/** Do nothing */
#define wlan_le16_to_cpu(x) x
/** Do nothing */
#define wlan_le32_to_cpu(x) x
/** Do nothing */
#define wlan_cpu_to_le16(x) x
/** Do nothing */
#define wlan_cpu_to_le32(x) x
/** Do nothing */
#define endian_convert_request_header(x)
/** Do nothing */
#define endian_convert_response_header(x)

#endif /* BIG_ENDIAN */

#ifdef  TRUE
#undef  TRUE
#endif
#define TRUE 1

#ifdef  FALSE
#undef  FALSE
#endif
#define FALSE 0

#endif
