/** @file wps_def.h
 *  @brief This file contains definitions for WPS global information.
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

#ifndef _WPS_DEF_H_
#define _WPS_DEF_H_

#include <stdint.h>
#include "mwu_defs.h"

#ifndef MIN
/** Macro to get minimun number */
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif /* MIN */

#ifndef MAX
/** Macro to get maximun number */
#define MAX(a,b)        ((a) > (b) ? (a) : (b))
#endif

#ifndef NELEMENTS
/** Number of elements in aray x */
#define NELEMENTS(x)    (sizeof(x)/sizeof(x[0]))
#endif

/** Configuration file for initialization */
#define FILE_WPSINIT_CONFIG_NAME    "./config/wps_init.conf"
/** Configuration file for multi-instance checking */
#define FILE_WPSRUN_CONFIG_NAME     "/tmp/wpsapp.pid"

/** Size: 1 Byte */
#define SIZE_1_BYTE         1
/** Size: 2 Byte */
#define SIZE_2_BYTES        2
/** Size: 4 Byte */
#define SIZE_4_BYTES        4
/** Size: 6 Byte */
#define SIZE_6_BYTES        6
/** Size: 8 Byte */
#define SIZE_8_BYTES        8
/** Size: 16 Byte */
#define SIZE_16_BYTES       16
/** Size: 20 Byte */
#define SIZE_20_BYTES       20
/** Size: 32 Byte */
#define SIZE_32_BYTES       32
/** Size: 64 Byte */
#define SIZE_64_BYTES       64
/** Size: 80 Byte */
#define SIZE_80_BYTES       80
/** Size: 128 Byte */
#define SIZE_128_BYTES      128
/** Size: 192 Byte */
#define SIZE_192_BYTES      192

/** IEEE 802.11 mode : infra */
#define IEEE80211_MODE_INFRA    0
/** IEEE 802.11 mode : IBSS */
#define IEEE80211_MODE_IBSS     1

/** IEEE 802.11 capability : ESS */
#define IEEE80211_CAP_ESS       0x0001
/** IEEE 802.11 capability : IBSS */
#define IEEE80211_CAP_IBSS      0x0002
/** IEEE 802.11 capability : PRIVACY */
#define IEEE80211_CAP_PRIVACY   0x0010

/** Maximum SSID length */
#define MAX_SSID_LEN         32
/** Scan AP limit */
#define SCAN_AP_LIMIT        64

/** Return type: success */
#define WPS_STATUS_SUCCESS      (0)
/** Return type: failure */
#define WPS_STATUS_FAIL         (-1)

/** WPS Set */
#define WPS_SET                    1
/** WPS Cancel */
#define WPS_CANCEL                 0

/** Discovery phase : Start */
#define WPS_START_REG_DISCOVERY_PHASE   1
/** Discovery phase : End */
#define WPS_END_REG_DISCOVERY_PHASE     0

/** BSS type : STA */
#define BSS_TYPE_STA 0
/** BSS type : UAP */
#define BSS_TYPE_UAP 1

/** Skip scan and start adhoc network */
#define WPS_SKIP_SCAN_ADHOC_START       1
/** scan and join/start adhoc network */
#define WPS_SCAN_ADHOC_JOIN             0

/** Maximum event buffer size */
#define EVENT_MAX_BUF_SIZE            1024

/** Role enrollee after discovery */
#define IS_DISCOVERY_ENROLLEE(gpwps) \
             (((gpwps)->role == WPS_ADHOC_EXTENTION || \
               (gpwps)->role == WIFIDIR_ROLE) && \
               (gpwps)->discovery_role == WPS_ENROLLEE)
/** Role registrar after discovery */
#define IS_DISCOVERY_REGISTRAR(gpwps) \
             (((gpwps)->role == WPS_ADHOC_EXTENTION || \
               (gpwps)->role == WIFIDIR_ROLE) && \
               (gpwps)->discovery_role == WPS_REGISTRAR)


/** Maximum WIFIDIR devices in Find phase */
#define WIFIDIR_MAX_FIND_DEVICES          16
/** Maximum WIFIDIR device name length */
#define MAX_DEVICE_NAME_LEN           32
/** WPS Device Type Length */
#define WPS_DEVICE_TYPE_LEN           8
/** Maximum Bit Map bit length */
#define MAX_BITMAP_LEN                16

/** Config methods mask (bit 8:0) */
#define CFG_METHODS_MASK              0x01FF

/** Action SET */
#define ACTION_GET                      (0)
/** Action GET */
#define ACTION_SET                      (1)

/** Maximum Persistent peers possible in a group */
#define WIFIDIR_MAX_PERSISTENT_PEERS    (2)
/** PSK max len */
#define WIFIDIR_PSK_LEN_MAX             (32)

/** User Persistent record format  */
typedef struct
{
    /* Index of record */
    s8 index;
    /* Device role */
    u8 dev_role;
    /* BSSID */
    u8 bssid[ETH_ALEN];
    /* Group device ID */
    u8 groupdevid[ETH_ALEN];
    /* SSID length */
    u8 ssid_len;
    /* SSID */
    u8 ssid[MAX_SSID_LEN];
    /* PSK length */
    u8 psk_len;
    /* PSK */
    u8 psk[WIFIDIR_PSK_LEN_MAX];
    /* Number of Peers */
    u8 num_peers;
    /* List of Peers */
    u8 peers[WIFIDIR_MAX_PERSISTENT_PEERS][ETH_ALEN];
} wifidir_persistent_record;

/** Read/Write Persistent group record in FW */
int wifidir_cfg_cmd_persistent_group_record(int action,
                                        wifidir_persistent_record * prec);

#endif /* _WPS_DEF_H_ */
