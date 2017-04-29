/** @file wlan_wifidir.h
 *  @brief This file contains definitions for Marvell WLAN driver host command,
 *         for wifidir specific commands.
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

#ifndef _WLAN_WIFIDIR_H_
#define _WLAN_WIFIDIR_H_

/*
 *  ctype from older glib installations defines BIG_ENDIAN.  Check it
 *   and undef it if necessary to correctly process the wlan header
 *   files
 */
#if (BYTE_ORDER == LITTLE_ENDIAN)
#undef BIG_ENDIAN
#endif

#ifdef BIG_ENDIAN

/** Convert TLV header to little endian format from CPU format */
#define endian_convert_tlv_header_out(x);           \
    {                                               \
        (x)->tag = wlan_cpu_to_le16((x)->tag);       \
        (x)->length = wlan_cpu_to_le16((x)->length); \
    }

/** Convert TLV header from little endian format to CPU format */
#define endian_convert_tlv_header_in(x)             \
    {                                               \
        (x)->tag = le16_to_cpu((x)->tag);       \
        (x)->length = le16_to_cpu((x)->length); \
    }

#else /* BIG_ENDIAN */

/** Do nothing */
#define endian_convert_tlv_header_out(x)
/** Do nothing */
#define endian_convert_tlv_header_in(x)

#endif /* BIG_ENDIAN */

/* Event IDs for Netlink socket events.  They are borrowed from mlanevent.c.
 * Note that these are not all WIFIDIR events, so they should probably be defined
 * elsewhere.
 */
#define EV_ID_UAP_EV_WMM_STATUS_CHANGE       0x00000017
#define EV_ID_UAP_EV_ID_STA_DEAUTH           0x0000002c
#define EV_ID_UAP_EV_ID_STA_ASSOC            0x0000002d
#define EV_ID_UAP_EV_ID_BSS_START            0x0000002e
#define EV_ID_UAP_EV_ID_DEBUG                0x00000036
#define EV_ID_UAP_EV_BSS_IDLE                0x00000043
#define EV_ID_UAP_EV_BSS_ACTIVE              0x00000044
#define EV_ID_WIFIDIR_GENERIC                    0x00000049
#define EV_ID_WIFIDIR_SERVICE_DISCOVERY          0x0000004a
#define EV_ID_UAP_EV_RSN_CONNECT             0x00000051

#define PRINT_CASE(i) case i: return #i
static inline char *nl_event_id_to_str(int id)
{
    switch (id) {
        PRINT_CASE(EV_ID_UAP_EV_WMM_STATUS_CHANGE);
        PRINT_CASE(EV_ID_UAP_EV_ID_STA_DEAUTH);
        PRINT_CASE(EV_ID_UAP_EV_ID_STA_ASSOC);
        PRINT_CASE(EV_ID_UAP_EV_ID_BSS_START);
        PRINT_CASE(EV_ID_UAP_EV_ID_DEBUG);
        PRINT_CASE(EV_ID_UAP_EV_BSS_IDLE);
        PRINT_CASE(EV_ID_UAP_EV_BSS_ACTIVE);
        PRINT_CASE(EV_ID_WIFIDIR_GENERIC);
        PRINT_CASE(EV_ID_WIFIDIR_SERVICE_DISCOVERY);
        PRINT_CASE(EV_ID_UAP_EV_RSN_CONNECT);
    }
    return "UNKNOWN";
}

/* enum wifidir_generic_event
 *
 * Events with the id EV_ID_WIFIDIR_GENERIC have one of the following types:
 *
 * EV_TYPE_WIFIDIR_NEGOTIATION_REQUEST:
 *
 * EV_TYPE_WIFIDIR_NEGOTIATION_RESPONSE:
 *
 * EV_TYPE_WIFIDIR_NEGOTIATION_RESULT:
 *
 * EV_TYPE_WIFIDIR_INVITATION_REQ:
 *
 * EV_TYPE_WIFIDIR_INVITATION_RESP:
 *
 * EV_TYPE_WIFIDIR_DISCOVERABILITY_REQUEST:
 *
 * EV_TYPE_WIFIDIR_DISCOVERABILITY_RESPONSE:
 *
 * EV_TYPE_WIFIDIR_PROVISION_DISCOVERY_REQ:
 *
 * EV_TYPE_WIFIDIR_PROVISION_DISCOVERY_RESP:
 *
 * EV_TYPE_WIFIDIR_PEER_DETECTED:
 *
 * EV_TYPE_WIFIDIR_CLIENT_ASSOCIATED_EVENT:
 *
 * EV_TYPE_WIFIDIR_FW_DEBUG_EVENT:
 */
enum wifidir_generic_event {
    EV_TYPE_WIFIDIR_NEGOTIATION_REQUEST = 0,
    EV_TYPE_WIFIDIR_NEGOTIATION_RESPONSE,
    EV_TYPE_WIFIDIR_NEGOTIATION_RESULT,
    EV_TYPE_WIFIDIR_INVITATION_REQ,
    EV_TYPE_WIFIDIR_INVITATION_RESP,
    EV_TYPE_WIFIDIR_DISCOVERABILITY_REQUEST,
    EV_TYPE_WIFIDIR_DISCOVERABILITY_RESPONSE,
    EV_TYPE_WIFIDIR_PROVISION_DISCOVERY_REQ,
    EV_TYPE_WIFIDIR_PROVISION_DISCOVERY_RESP,
    EV_TYPE_WIFIDIR_NEGOTIATION_RESPONSE_TX,
    /* Events 10-13 are not documented or used */
    EV_TYPE_WIFIDIR_PEER_DETECTED = 14,
    EV_TYPE_WIFIDIR_CLIENT_ASSOCIATED,
    EV_TYPE_WIFIDIR_FW_DEBUG,
};

static inline char *wifidir_generic_event_to_str(enum wifidir_generic_event e)
{
    switch (e) {
    case EV_TYPE_WIFIDIR_NEGOTIATION_REQUEST:
        return "WIFIDIR_NEGOTIATION_REQUEST";
    case EV_TYPE_WIFIDIR_NEGOTIATION_RESPONSE:
        return "WIFIDIR_NEGOTIATION_RESPONSE";
    case EV_TYPE_WIFIDIR_NEGOTIATION_RESULT:
        return "WIFIDIR_NEGOTIATION_RESULT";
    case EV_TYPE_WIFIDIR_INVITATION_REQ:
        return "WIFIDIR_INVITATION_REQ";
    case EV_TYPE_WIFIDIR_INVITATION_RESP:
        return "WIFIDIR_INVITATION_RESP";
    case EV_TYPE_WIFIDIR_DISCOVERABILITY_REQUEST:
        return "WIFIDIR_DISCOVERABILITY_REQUEST";
    case EV_TYPE_WIFIDIR_DISCOVERABILITY_RESPONSE:
        return "WIFIDIR_DISCOVERABILITY_RESPONSE";
    case EV_TYPE_WIFIDIR_PROVISION_DISCOVERY_REQ:
        return "WIFIDIR_PROVISION_DISCOVERY_REQ";
    case EV_TYPE_WIFIDIR_PROVISION_DISCOVERY_RESP:
        return "WIFIDIR_PROVISION_DISCOVERY_RESP";
    case EV_TYPE_WIFIDIR_NEGOTIATION_RESPONSE_TX:
        return "EV_TYPE_WIFIDIR_NEGOTIATION_RESPONSE_TX";
    case EV_TYPE_WIFIDIR_PEER_DETECTED:
        return "WIFIDIR_PEER_DETECTED";
    case EV_TYPE_WIFIDIR_CLIENT_ASSOCIATED:
        return "WIFIDIR_CLIENT_ASSOCIATED";
    case EV_TYPE_WIFIDIR_FW_DEBUG:
        return "WIFIDIR_DEBUG";
    default:
        return "UNKNOWN";
        break;
    }
}

/* enum wifidir_generic_subevent
 *
 * wifidir_generic_events may have one of the following subtypes.  These are
 * undocumented, but appear to function more as a result code than an event
 * subtype.  They are documented per-event above.
 */
enum wifidir_generic_subevent {
    EV_SUBTYPE_WIFIDIR_GO_NEG_FAILED = 0,
    EV_SUBTYPE_WIFIDIR_GO_ROLE,
    EV_SUBTYPE_WIFIDIR_CLIENT_ROLE,
};

enum wifidir_reqrecv_subevent {
	/** Event subtype for None */
	EV_SUBTYPE_WIFIDIR_REQRECV_NONE = 0,
	/** Event subtype for Req Recv & Processing */
	EV_SUBTYPE_WIFIDIR_REQRECV_PROCESSING,
	/** Event subtype for Req Dropped Insufficient Information */
	EV_SUBTYPE_WIFIDIR_REQRECV_DROP_INSUFFICIENT_INFO,
	/** Event subtype for Req Recv & Processed successfully */
	EV_SUBTYPE_WIFIDIR_REQRECV_PROCESSING_SUCCESS,
	/** Event subtype for Req Recv & Processing failed */
	EV_SUBTYPE_WIFIDIR_REQRECV_PROCESSING_FAIL,
};

/** Host Command ioctl number */
#define WIFIDIRHOSTCMD          (SIOCDEVPRIVATE + 1)
/** Private command ID to BSS control */
#define UAP_BSS_CTRL                (SIOCDEVPRIVATE + 4)
/** Private command ID for mode config */
#define WIFIDIR_MODE_CONFIG     (SIOCDEVPRIVATE + 5)
/** OUI Type WIFIDIR */
#define WIFIDIR_OUI_TYPE                    9
/** OUI Type WPS  */
#define WIFI_WPS_OUI_TYPE                       4

/** category: Public Action frame */
#define WIFI_CATEGORY_PUBLIC_ACTION_FRAME       4
/** Dialog Token : Ignore */
#define WIFI_DIALOG_TOKEN_IGNORE                0

/** WIFIDIR IE header len */
#define WIFIDIR_IE_HEADER_LEN                       3
/** IE header len */
#define IE_HEADER_LEN                           2

/** command to stop as wifidir device. */
#define WIFIDIR_MODE_STOP                           0
/** command to start as wifidir device. */
#define WIFIDIR_MODE_START                          1
/** command to start as group owner from host. */
#define WIFIDIR_MODE_START_GROUP_OWNER              2
/** command to start as device from host. */
#define WIFIDIR_MODE_START_DEVICE                   3
/** command to start find phase  */
#define WIFIDIR_MODE_START_FIND_PHASE               4
/** command to stop find phase  */
#define WIFIDIR_MODE_STOP_FIND_PHASE                5
/** command to get WIFIDIR mode   */
#define WIFIDIR_MODE_QUERY                  0xFF
/** command to get WIFIDIR GO mode   */
#define WIFIDIR_GO_QUERY                    0xFFFF

/** 4 byte header to store buf len*/
#define BUF_HEADER_SIZE                         4
/** Maximum length of lines in configuration file */
#define MAX_CONFIG_LINE                         1024
/** Host Command ID bit mask (bit 11:0) */
#define HostCmd_CMD_ID_MASK                     0x0fff
/** WIFIDIRCMD response check */
#define WIFIDIRCMD_RESP_CHECK               0x8000

/** Host Command ID:  WIFIDIR_ACTION_FRAME */
#define HostCmd_CMD_802_11_ACTION_FRAME         0x00f4
/** Host Command ID : wifidir mode config */
#define HostCmd_CMD_WIFIDIR_MODE_CONFIG             0x00eb
/** Host Command ID:  WIFIDIR_SET_PARAMS */
#define HostCmd_CMD_WIFIDIR_PARAMS_CONFIG           0x00ea
/** Check Bit GO set by peer Bit 0 */
#define WIFIDIR_GROUP_OWNER_MASK                    0x01
/** Check Bit GO set by peer Bit 0 */
#define WIFIDIR_GROUP_CAP_PERSIST_GROUP_BIT         0x02
/** Check Bit formation Bit 6 */
#define WIFIDIR_GROUP_FORMATION_OR_MASK             0x40
/** Check Bit formation Bit 6 */
#define WIFIDIR_GROUP_FORMATION_AND_MASK            0xBF

/** C Success */
#define SUCCESS                         1
/** C Failure */
#define FAILURE                         0

/** Wifi reg class = 81 */
#define WIFI_REG_CLASS_81               81

/** MAC BROADCAST */
#define WIFIDIR_RET_MAC_BROADCAST   0x1FF
/** MAC MULTICAST */
#define WIFIDIR_RET_MAC_MULTICAST   0x1FE
/**
 * Hex or Decimal to Integer
 * @param   num string to convert into decimal or hex
 */
#define A2HEXDECIMAL(num)  \
    (strncasecmp("0x", (num), 2)?(unsigned int) strtoll((num),NULL,0):a2hex((num)))\
/**
 * Check of decimal or hex string
 * @param   num string
 */
#define IS_HEX_OR_DIGIT(num) \
    (strncasecmp("0x", (num), 2)?ISDIGIT((num)):ishexstring((num)))\

/** Set OPP_PS variable */
#define SET_OPP_PS(x) ((x) << 7)

/** Convert WIFIDIR header to little endian format from CPU format */
#define endian_convert_tlv_wifidir_header_out(x);           \
{                                               \
    (x)->length = wlan_cpu_to_le16((x)->length); \
}

/** Convert WPS TLV header to network order */
#define endian_convert_tlv_wps_header_out(x);           \
{                                               \
    (x)->tag = mwu_htons((x)->tag);       \
    (x)->length = mwu_htons((x)->length); \
}

/** OUI SubType GO NEG REQ action DIRECT */
#define WIFIDIR_GO_NEG_REQ_ACTION_SUB_TYPE        0
/** OUI SubType Provision discovery request */
#define WIFIDIR_PROVISION_DISCOVERY_REQ_SUB_TYPE        7
/** WPS Device Type maximum length */
#define WPS_DEVICE_TYPE_MAX_LEN                 8

/** TLV : WIFIDIR status */
#define TLV_TYPE_WIFIDIR_STATUS             0
/** TLV : WIFIDIR param capability */
#define TLV_TYPE_WIFIDIR_CAPABILITY         2
/** TLV : WIFIDIR param device Id */
#define TLV_TYPE_WIFIDIR_DEVICE_ID          3
/** TLV : WIFIDIR param group owner intent */
#define TLV_TYPE_WIFIDIR_GROUPOWNER_INTENT  4
/** TLV : WIFIDIR param config timeout */
#define TLV_TYPE_WIFIDIR_CONFIG_TIMEOUT     5
/** TLV : WIFIDIR param channel */
#define TLV_TYPE_WIFIDIR_CHANNEL            6
/** TLV : WIFIDIR param group bssId */
#define TLV_TYPE_WIFIDIR_GROUP_BSS_ID       7
/** TLV : WIFIDIR param extended listen time */
#define TLV_TYPE_WIFIDIR_EXTENDED_LISTEN_TIME 8
/** TLV : WIFIDIR param intended address */
#define TLV_TYPE_WIFIDIR_INTENDED_ADDRESS   9
/** TLV : WIFIDIR param manageability */
#define TLV_TYPE_WIFIDIR_MANAGEABILITY      10
/** TLV : WIFIDIR param channel list */
#define TLV_TYPE_WIFIDIR_CHANNEL_LIST       11
/** TLV : WIFIDIR Notice of Absence */
#define TLV_TYPE_WIFIDIR_NOTICE_OF_ABSENCE  12
/** TLV : WIFIDIR param device Info */
#define TLV_TYPE_WIFIDIR_DEVICE_INFO        13
/** TLV : WIFIDIR param Group Info */
#define TLV_TYPE_WIFIDIR_GROUP_INFO         14
/** TLV : WIFIDIR param group Id */
#define TLV_TYPE_WIFIDIR_GROUP_ID           15
/** TLV : WIFIDIR param interface */
#define TLV_TYPE_WIFIDIR_INTERFACE          16
/** TLV : WIFIDIR param operating channel */
#define TLV_TYPE_WIFIDIR_OPCHANNEL          17
/** TLV : WIFIDIR param invitation flag */
#define TLV_TYPE_WIFIDIR_INVITATION_FLAG    18

/** TLV: WIFIDIR Provisioning parameters */
#define WIFIDIR_PROVISIONING_PARAMS_TLV_ID  0x18f

/** TLV: WIFIDIR Discovery Period */
#define MRVL_WIFIDIR_DISC_PERIOD_TLV_ID       (PROPRIETARY_TLV_BASE_ID + 124)
/** TLV: WIFIDIR Scan Enable */
#define MRVL_WIFIDIR_SCAN_ENABLE_TLV_ID       (PROPRIETARY_TLV_BASE_ID + 125)
/** TLV: WIFIDIR Peer Device  */
#define MRVL_WIFIDIR_PEER_DEVICE_TLV_ID       (PROPRIETARY_TLV_BASE_ID + 126)
/** TLV: WIFIDIR Scan Request Peer Device  */
#define MRVL_WIFIDIR_SCAN_REQ_DEVICE_TLV_ID   (PROPRIETARY_TLV_BASE_ID + 127)
/** TLV: WIFIDIR Device State */
#define MRVL_WIFIDIR_DEVICE_STATE_TLV_ID      (PROPRIETARY_TLV_BASE_ID + 128)
/** TLV: WIFIDIR Operating Channel */
#define MRVL_WIFIDIR_OPERATING_CHANNEL_TLV_ID    (PROPRIETARY_TLV_BASE_ID + 0x87)   // 0x0187
/** TLV: WIFIDIR Persistent Group */
#define MRVL_WIFIDIR_PERSISTENT_GROUP_TLV_ID     (PROPRIETARY_TLV_BASE_ID + 0x88)   // 0x0188

#define MRVL_WIFIDIR_WPS_PARAMS_TLV_ID           (PROPRIETARY_TLV_BASE_ID + 0x90)   // 0x0190

/** Max Device capability */
#define MAX_DEV_CAPABILITY                 255
/** Max group capability */
#define MAX_GRP_CAPABILITY                 255
/** Max Intent */
#define MAX_INTENT                        15
/** Max length of Primary device type OUI */
#define MAX_PRIMARY_OUI_LEN               4
/** Min value of Regulatory class */
#define MIN_REG_CLASS                      1
/** Max value of Regulatory class */
#define MAX_REG_CLASS                    255
/** Min Primary Device type category */
#define MIN_PRIDEV_TYPE_CAT                1
/** Max Primary Device type category */
#define MAX_PRIDEV_TYPE_CAT               11
/** Min value of NoA index */
#define MIN_NOA_INDEX                   0
/** Max value of NoA index */
#define MAX_NOA_INDEX                   255
/** Min value of CTwindow */
#define MIN_CTWINDOW                    10
/** Max value of CTwindow */
#define MAX_CTWINDOW                    63
/** Min value of Count/Type */
#define MIN_COUNT_TYPE                  1
/** Max value of Count/Type */
#define MAX_COUNT_TYPE                  255
/** Min Primary Device type subcategory */
#define MIN_PRIDEV_TYPE_SUBCATEGORY        1
/** Max Primary Device type subcategory */
#define MAX_PRIDEV_TYPE_SUBCATEGORY        9
/** Min value of WPS config method */
#define MIN_WPS_CONF_METHODS            0x01
/** Max value of WPS config method */
#define MAX_WPS_CONF_METHODS            0xffff
/** Max length of Advertisement Protocol IE  */
#define MAX_ADPROTOIE_LEN               4
/** Max length of Discovery Information ID  */
#define MAX_INFOID_LEN                  2
/** Max length of OUI  */
#define MAX_OUI_LEN                     3
/** Max count of interface list */
#define MAX_INTERFACE_ADDR_COUNT        41
/** Max count of secondary device types */
#define MAX_SECONDARY_DEVICE_COUNT      15
/** Max count of group secondary device types*/
#define MAX_GROUP_SECONDARY_DEVICE_COUNT  2
/** Maximum length of lines in configuration file */
#define MAX_CONFIG_LINE                 1024
/** Maximum channels */
#define MAX_CHANNELS                    14
/** Maximum number of NoA descriptors */
#define MAX_NOA_DESCRIPTORS            8
/** Maximum number of channel list entries */
#define MAX_CHAN_LIST    8
/** Maximum buffer size for channel entries */
#define MAX_BUFFER_SIZE         64
/** WPS Minimum version number */
#define WPS_MIN_VERSION            0x10
/** WPS Maximum version number */
#define WPS_MAX_VERSION                 0x20
/** WPS Minimum request type */
#define WPS_MIN_REQUESTTYPE        0x00
/** WPS Maximum request type */
#define WPS_MAX_REQUESTTYPE        0x04
/** WPS UUID maximum length */
#define WPS_UUID_MAX_LEN                16
/** WPS Device Type maximum length */
#define WPS_DEVICE_TYPE_MAX_LEN         8
/** WPS Minimum association state */
#define WPS_MIN_ASSOCIATIONSTATE    0x0000
/** WPS Maximum association state */
#define WPS_MAX_ASSOCIATIONSTATE    0x0004
/** WPS Minimum configuration error */
#define WPS_MIN_CONFIGURATIONERROR    0x0000
/** WPS Maximum configuration error */
#define WPS_MAX_CONFIGURATIONERROR    0x0012
/** WPS Minimum Device password ID */
#define WPS_MIN_DEVICEPASSWORD        0x0000
/** WPS Maximum Device password ID */
#define WPS_MAX_DEVICEPASSWORD        0x000f
/** WPS Model maximum length */
#define WPS_MODEL_MAX_LEN               32
/** WPS Serial maximum length */
#define WPS_SERIAL_MAX_LEN              32
/** WPS Manufacturer maximum length */
#define WPS_MANUFACT_MAX_LEN            64
/** WPS Device Info OUI+Type+SubType Length */
#define WPS_DEVICE_TYPE_LEN             8
/** Max size of custom IE buffer */
#define MAX_SIZE_IE_BUFFER              (256)
/** Country string last byte 0x04 */
#define WIFIDIR_COUNTRY_LAST_BYTE           0x04

/** WIFIDIR IE Header */
typedef struct _wifidir_ie_header
{
    /** Element ID */
    u8 element_id;
    /** IE Length */
    u8 ie_length;
    /** OUI */
    u8 oui[3];
    /** OUI type */
    u8 oui_type;
    /** IE List of TLV */
    u8 ie_list[0];
} __ATTRIB_PACK__ wifidir_ie_header;

/** Event : WIFIDIR event subtype fields */
#ifdef BIG_ENDIAN
typedef struct _wifidir_event_subtype
{
    /** Reserved */
    u16 reserved:11;
    /** Packet Process State */
    u16 pkt_process_state:3;
    /** Device Role */
    u16 device_role:2;
} __ATTRIB_PACK__ wifidir_event_subtype;
#else
typedef struct _wifidir_event_subtype
{
    /** Device Role */
    u16 device_role:2;
    /** Packet Process State */
    u16 pkt_process_state:3;
    /** Reserved */
    u16 reserved:11;
} __ATTRIB_PACK__ wifidir_event_subtype;
#endif

/** Event : WIFIDIR Generic event */
typedef struct _apeventbuf_wifidir_generic
{
    /** Event Length */
    u16 event_length;
    /** Event Type */
    u16 event_type;
    /** Event SubType */
    wifidir_event_subtype event_sub_type;
    /** Peer mac address */
    u8 peer_mac_addr[ETH_ALEN];
    /** IE List of TLV */
    u8 entire_ie_list[0];
} __ATTRIB_PACK__ apeventbuf_wifidir_generic;

/** TLV buffer : WIFIDIR IE device Id */
typedef struct _tlvbuf_wifidir_device_id
{
    /** TLV Header tag */
    u8 tag;
    /** TLV Header length */
    u16 length;
    /** WIFIDIR device MAC address */
    u8 dev_mac_address[ETH_ALEN];
} __ATTRIB_PACK__ tlvbuf_wifidir_device_id;

/** TLV buffer : WIFIDIR Status */
typedef struct _tlvbuf_wifidir_status
{
    /** TLV Header tag */
    u8 tag;
    /** TLV Header length */
    u16 length;
    /** WIFIDIR status code */
    u8 status_code;
} __ATTRIB_PACK__ tlvbuf_wifidir_status;

/** TLV buffer : WIFIDIR IE capability */
typedef struct _tlvbuf_wifidir_capability
{
    /** TLV Header tag */
    u8 tag;
    /** TLV Header length */
    u16 length;
    /** WIFIDIR device capability */
    u8 dev_capability;
    /** WIFIDIR group capability */
    u8 group_capability;
} __ATTRIB_PACK__ tlvbuf_wifidir_capability;

/** TLV buffer : WIFIDIR extended listen time */
typedef struct _tlvbuf_wifidir_ext_listen_time
{
    /** TLV Header tag */
    u8 tag;
    /** TLV Header length */
    u16 length;
    /** Availability period */
    u16 availability_period;
    /** Availability interval */
    u16 availability_interval;
} __ATTRIB_PACK__ tlvbuf_wifidir_ext_listen_time;

/** TLV buffer : WIFIDIR Intended Interface Address */
typedef struct _tlvbuf_wifidir_intended_addr
{
    /** TLV Header tag */
    u8 tag;
    /** TLV Header length */
    u16 length;
    /** WIFIDIR Group interface address */
    u8 group_address[ETH_ALEN];
} __ATTRIB_PACK__ tlvbuf_wifidir_intended_addr;

/** TLV buffer : WIFIDIR IE Interface */
typedef struct _tlvbuf_wifidir_interface
{
    /** TLV Header tag */
    u8 tag;
    /** TLV Header length */
    u16 length;
    /** WIFIDIR interface Id */
    u8 interface_id[ETH_ALEN];
    /** WIFIDIR interface count */
    u8 interface_count;
    /** WIFIDIR interface addresslist */
    u8 interface_idlist[0];
} __ATTRIB_PACK__ tlvbuf_wifidir_interface;

/** TLV buffer : WIFIDIR configuration timeout */
typedef struct _tlvbuf_wifidir_config_timeout
{
    /** TLV Header tag */
    u8 tag;
    /** TLV Header length */
    u16 length;
    /** Group configuration timeout */
    u8 group_config_timeout;
    /** Device configuration timeout */
    u8 device_config_timeout;
} __ATTRIB_PACK__ tlvbuf_wifidir_config_timeout;

/** TLV buffer : WIFIDIR IE Group owner intent */
typedef struct _tlvbuf_wifidir_group_owner_intent
{
    /** TLV Header tag */
    u8 tag;
    /** TLV Header length */
    u16 length;
    /** WIFIDIR device group owner intent */
    u8 dev_intent;
} __ATTRIB_PACK__ tlvbuf_wifidir_group_owner_intent;

/** TLV buffer : WIFIDIR IE channel */
typedef struct _tlvbuf_wifidir_channel
{
    /** TLV Header tag */
    u8 tag;
    /** TLV Header length */
    u16 length;
    /** WIFIDIR country string */
    u8 country_string[3];
    /** WIFIDIR regulatory class */
    u8 regulatory_class;
    /** WIFIDIR channel number */
    u8 channel_number;
} __ATTRIB_PACK__ tlvbuf_wifidir_channel;

/** TLV buffer : WIFIDIR IE manageability */
typedef struct _tlvbuf_wifidir_manageability
{
    /** TLV Header tag */
    u8 tag;
    /** TLV Header length */
    u16 length;
    /** WIFIDIR manageability */
    u8 manageability;
} __ATTRIB_PACK__ tlvbuf_wifidir_manageability;

/** TLV buffer : WIFIDIR IE Invitation Flag */
typedef struct _tlvbuf_wifidir_invitation_flag
{
    /** TLV Header tag */
    u8 tag;
    /** TLV Header length */
    u16 length;
    /** WIFIDIR Invitation Flag */
    u8 invitation_flag;
} __ATTRIB_PACK__ tlvbuf_wifidir_invitation_flag;

/** Channel Entry */
typedef struct _chan_entry
{
    /** WIFIDIR regulatory class */
    u8 regulatory_class;
    /** WIFIDIR no of channels */
    u8 num_of_channels;
    /** WIFIDIR channel number */
    u8 chan_list[0];
} __ATTRIB_PACK__ chan_entry;

/** TLV buffer : WIFIDIR IE channel list */
typedef struct _tlvbuf_wifidir_channel_list
{
    /** TLV Header tag */
    u8 tag;
    /** TLV Header length */
    u16 length;
    /** WIFIDIR country string */
    u8 country_string[3];
    /** WIFIDIR channel entry list */
    chan_entry wifidir_chan_entry_list[0];
} __ATTRIB_PACK__ tlvbuf_wifidir_channel_list;

/** NoA Descriptor */
typedef struct _noa_descriptor
{
    /** WIFIDIR count OR type */
    u8 count_type;
    /** WIFIDIR duration */
    u32 duration;
    /** WIFIDIR interval */
    u32 interval;
    /** WIFIDIR start time */
    u32 start_time;
} __ATTRIB_PACK__ noa_descriptor;

/** TLV buffer : WIFIDIR IE Notice of Absence */
typedef struct _tlvbuf_wifidir_notice_of_absence
{
    /** TLV Header tag */
    u8 tag;
    /** TLV Header length */
    u16 length;
    /** WIFIDIR NoA Index */
    u8 noa_index;
    /** WIFIDIR CTWindow and OppPS parameters */
    u8 ctwindow_opp_ps;
    /** WIFIDIR NoA Descriptor list */
    noa_descriptor wifidir_noa_descriptor_list[0];
} __ATTRIB_PACK__ tlvbuf_wifidir_notice_of_absence;

/** TLV buffer : WIFIDIR IE group Id */
typedef struct _tlvbuf_wifidir_group_id
{
    /** TLV Header tag */
    u8 tag;
    /** TLV Header length */
    u16 length;
    /** WIFIDIR group MAC address */
    u8 group_address[ETH_ALEN];
    /** WIFIDIR group SSID */
    u8 group_ssid[0];
} __ATTRIB_PACK__ tlvbuf_wifidir_group_id;

/** TLV buffer : WIFIDIR IE group BSS Id */
typedef struct _tlvbuf_wifidir_group_bss_id
{
    /** TLV Header tag */
    u8 tag;
    /** TLV Header length */
    u16 length;
    /** WIFIDIR group Bss Id */
    u8 group_bssid[ETH_ALEN];
} __ATTRIB_PACK__ tlvbuf_wifidir_group_bss_id;

typedef struct _wifidir_device_name_info
{
    /** WPS Device Name Tag */
    u16 device_name_type;
    /** WPS Device Name Length */
    u16 device_name_len;
    /** Device name */
    u8 device_name[0];
} __ATTRIB_PACK__ wifidir_device_name_info;

/** TLV buffer : WIFIDIR IE device Info */
typedef struct _tlvbuf_wifidir_device_info
{
    /** TLV Header tag */
    u8 tag;
    /** TLV Header length */
    u16 length;
    /** WIFIDIR device address */
    u8 dev_address[ETH_ALEN];
    /** WPS config methods */
    u16 config_methods;
    /** Primary device type : category */
    u16 primary_category;
    /** Primary device type : OUI */
    u8 primary_oui[4];
    /** Primary device type : sub-category */
    u16 primary_subcategory;
    /** Secondary Device Count */
    u8 secondary_dev_count;
    /** Secondary Device Info */
    u8 secondary_dev_info[0];
    /** Device Name Info */
    wifidir_device_name_info device_name_info;
} __ATTRIB_PACK__ tlvbuf_wifidir_device_info;

typedef struct _wifidir_client_dev_info
{
    /** Length of each device */
    u8 dev_length;
    /** WIFIDIR device address */
    u8 wifidir_dev_address[ETH_ALEN];
    /** WIFIDIR Interface  address */
    u8 wifidir_intf_address[ETH_ALEN];
    /** WIFIDIR Device capability*/
    u8 wifidir_dev_capability;
    /** WPS config methods */
    u16 config_methods;
    /** Primary device type : category */
    u16 primary_category;
    /** Primary device type : OUI */
    u8 primary_oui[4];
    /** Primary device type : sub-category */
    u16 primary_subcategory;
    /** Secondary Device Count */
    u8 wifidir_secondary_dev_count;
    /** Secondary Device Info */
    u8 wifidir_secondary_dev_info[0];
    /** WPS WIFIDIR Device Name Tag */
    u16 device_name_type;
    /** WPS WIFIDIR Device Name Length */
    u16 wifidir_device_name_len;
    /** WIFIDIR Device name */
    u8 wifidir_device_name[0];
} __ATTRIB_PACK__ wifidir_client_dev_info;

typedef struct _tlvbuf_wifidir_group_info
{
    /** TLV Header tag */
    u8 tag;
    /** TLV Header length */
    u16 length;
    /** Secondary Device Info */
    u8 wifidir_client_dev_list[0];
} __ATTRIB_PACK__ tlvbuf_wifidir_group_info;

/** TLV buffer : WIFIDIR WPS IE */
typedef struct _tlvbuf_wifidir_wps_ie
{
    /** TLV Header tag */
    u16 tag;
    /** TLV Header length */
    u16 length;
    /** WIFIDIR WPS IE data */
    u8 data[0];
} __ATTRIB_PACK__ tlvbuf_wps_ie;

/** TLV buffer : WIFIDIR Provisioning parameters*/
typedef struct _tlvbuf_wifidir_provisioning_params
{
    /** TLV Header tag */
    u16 tag;
    /** TLV Header length */
    u16 length;
    /** action */
    u16 action;
    /** config methods */
    u16 config_methods;
    /** dev password */
    u16 dev_password;
} __ATTRIB_PACK__ tlvbuf_wifidir_provisioning_params;

typedef struct _tlvbuf_wifidir_wps_params
{
    /** Tag */
    u16 tag;
    /** Length */
    u16 length;
    /** action */
    u16 action;
} __ATTRIB_PACK__ tlvbuf_wifidir_wps_params;

typedef struct _tlvbuf_wifidir_operating_channel
{
    /** Tag */
    u16 tag;
    /** Length */
    u16 length;
    /** Operating Channel */
    u8  operating_channel;
} __ATTRIB_PACK__ tlvbuf_wifidir_operating_channel;

/** WIFIDIRCMD buffer */
typedef struct _wifidircmdbuf
{
    /** Command header */
    mrvl_cmd_head_buf cmd_head;
} __ATTRIB_PACK__ wifidircmdbuf;

/** HostCmd_CMD_WIFIDIR_ACTION_FRAME request */
typedef struct _wifidir_action_frame
{
    /** Peer mac address */
    u8 peer_mac_addr[ETH_ALEN];
    /** Category */
    u8 category;
    /** Action */
    u8 action;
    /** OUI */
    u8 oui[3];
    /** OUI type */
    u8 oui_type;
    /** OUI sub type */
    u8 oui_sub_type;
    /** Dialog taken */
    u8 dialog_taken;
    /** IE List of TLVs */
    u8 ie_list[0];
} __ATTRIB_PACK__ wifidir_action_frame;

/** HostCmd_CMD_WIFIDIR_PARAMS_CONFIG struct */
typedef struct _wifidir_params_config
{
    /** Action */
    u16 action;                 /* 0 = ACT_GET; 1 = ACT_SET; */
    /** TLV data */
    u8 wifidir_tlv[0];
} __ATTRIB_PACK__ wifidir_params_config;

/** HostCmd_CMD_WIFIDIR_MODE_CONFIG structure */
typedef struct _wifidir_mode_config
{
    /** Action */
    u16 action;                 /* 0 = ACT_GET; 1 = ACT_SET; */
    /** wifidir mode data */
    u16 mode;
} __ATTRIB_PACK__ wifidir_mode_config;

/** Valid Input Commands */
typedef enum
{
    SCANCHANNELS,
    CHANNEL,
    WIFIDIR_DEVICECAPABILITY,
    WIFIDIR_GROUPCAPABILITY,
    WIFIDIR_INTENT,
    WIFIDIR_REGULATORYCLASS,
    WIFIDIR_MANAGEABILITY,
    WIFIDIR_INVITATIONFLAG,
    WIFIDIR_COUNTRY,
    WIFIDIR_NO_OF_CHANNELS,
    WIFIDIR_NOA_INDEX,
    WIFIDIR_OPP_PS,
    WIFIDIR_CTWINDOW,
    WIFIDIR_COUNT_TYPE,
    WIFIDIR_DURATION,
    WIFIDIR_INTERVAL,
    WIFIDIR_START_TIME,
    WIFIDIR_PRIDEVTYPECATEGORY,
    WIFIDIR_PRIDEVTYPEOUI,
    WIFIDIR_PRIDEVTYPESUBCATEGORY,
    WIFIDIR_SECONDARYDEVCOUNT,
    WIFIDIR_GROUP_SECONDARYDEVCOUNT,
    WIFIDIR_GROUP_WIFIDIR_DEVICE_NAME,
    WIFIDIR_INTERFACECOUNT,
    WIFIDIR_ATTR_CONFIG_TIMEOUT,
    WIFIDIR_ATTR_EXTENDED_TIME,
    WIFIDIR_WPSCONFMETHODS,
    WIFIDIR_WPSVERSION,
    WIFIDIR_WPSSETUPSTATE,
    WIFIDIR_WPSREQRESPTYPE,
    WIFIDIR_WPSSPECCONFMETHODS,
    WIFIDIR_WPSUUID,
    WIFIDIR_WPSPRIMARYDEVICETYPE,
    WIFIDIR_WPSRFBAND,
    WIFIDIR_WPSASSOCIATIONSTATE,
    WIFIDIR_WPSCONFIGURATIONERROR,
    WIFIDIR_WPSDEVICENAME,
    WIFIDIR_WPSDEVICEPASSWORD,
    WIFIDIR_WPSMANUFACTURER,
    WIFIDIR_WPSMODELNAME,
    WIFIDIR_WPSMODELNUMBER,
    WIFIDIR_WPSSERIALNUMBER,
    WIFIDIR_MINDISCOVERYINT,
    WIFIDIR_MAXDISCOVERYINT,
    WIFIDIR_ENABLE_SCAN,
    WIFIDIR_DEVICE_STATE,
} valid_inputs;

/** Level of wifidir parameters in the wifidir.conf file */
typedef enum
{
    WIFIDIR_PARAMS_CONFIG = 1,
    WIFIDIR_CAPABILITY_CONFIG,
    WIFIDIR_GROUP_OWNER_INTENT_CONFIG,
    WIFIDIR_CHANNEL_CONFIG,
    WIFIDIR_MANAGEABILITY_CONFIG,
    WIFIDIR_INVITATION_FLAG_CONFIG,
    WIFIDIR_CHANNEL_LIST_CONFIG,
    WIFIDIR_NOTICE_OF_ABSENCE,
    WIFIDIR_NOA_DESCRIPTOR,
    WIFIDIR_DEVICE_INFO_CONFIG,
    WIFIDIR_GROUP_INFO_CONFIG,
    WIFIDIR_GROUP_SEC_INFO_CONFIG,
    WIFIDIR_GROUP_CLIENT_INFO_CONFIG,
    WIFIDIR_DEVICE_SEC_INFO_CONFIG,
    WIFIDIR_GROUP_ID_CONFIG,
    WIFIDIR_GROUP_BSS_ID_CONFIG,
    WIFIDIR_DEVICE_ID_CONFIG,
    WIFIDIR_INTERFACE_CONFIG,
    WIFIDIR_TIMEOUT_CONFIG,
    WIFIDIR_EXTENDED_TIME_CONFIG,
    WIFIDIR_INTENDED_ADDR_CONFIG,
    WIFIDIR_OPCHANNEL_CONFIG,
    WIFIDIR_WPSIE,
    WIFIDIR_DISCOVERY_REQUEST_RESPONSE = 0x20,
    WIFIDIR_DISCOVERY_QUERY,
    WIFIDIR_DISCOVERY_SERVICE,
    WIFIDIR_DISCOVERY_VENDOR,
    WIFIDIR_EXTRA,
} wifidir_param_level;

/* Needed for passphrase to PSK conversion */
void pbkdf2_sha1(const char *passphrase, const char *ssid, size_t ssid_len,
                 int iterations, u8 * buf, size_t buflen);

/**  Max Number of Peristent group possible in FW */
#define WIFIDIR_PERSISTENT_GROUP_MAX        (4)

/** TLV buffer : persistent group */
typedef struct _tlvbuf_wifidir_persistent_group
{
    /** Tag */
    u16 tag;
    /** Length */
    u16 length;
    /** Record Index */
    u8 rec_index;
    /** Device Role */
    u8 dev_role;
    /** wifidir group Bss Id */
    u8 group_bssid[ETH_ALEN];
    /** wifidir device MAC address */
    u8 dev_mac_address[ETH_ALEN];
    /** wifidir group SSID length */
    u8 group_ssid_len;
    /** wifidir group SSID */
    u8 group_ssid[0];
    /** wifidir PSK length */
    u8 psk_len;
    /** wifidir PSK */
    u8 psk[0];
    /** Num of PEER MAC Addresses */
    u8 num_peers;
    /** PEER MAC Addresses List */
    u8 peer_mac_addrs[0][ETH_ALEN];
} __ATTRIB_PACK__ tlvbuf_wifidir_persistent_group;

/** Return index of persistent group record */
int wifidir_get_persistent_rec_ind_by_ssid(int ssid_length,
                                       u8 * ssid, s8 * pr_index);

#endif /* _WLAN_WIFIDIR_H_ */
