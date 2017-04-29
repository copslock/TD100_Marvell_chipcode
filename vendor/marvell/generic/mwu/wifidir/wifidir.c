/*
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

/* wifidir.c: implementation of the wifi direct module
 *
 * The core of this beast is the state machine implemented in
 * wifidir_state_machine().  See state_machine.jpg for a rough diagram of the
 * state machine.  Be sure to update state_machine.jpg when you alter the state
 * machine.  And don't trust the jpg over the code!  It's probably out of date!
 */

#include "wps_msg.h"
#include "wifidir.h"
#include "wifidir_sm.h"
#include "wifidir_lib.h"
#define UTIL_LOG_TAG "WIFIDIR"
#include "util.h"
#include "wlan_hostcmd.h"
#include "wlan_wifidir.h"
#include "mwu.h"
#include "wps_wlan.h"
#include "mwu_ioctl.h"
#include "mwu_log.h"
#include "wps_os.h" /* for timer functions */
#include "mwu_timer.h"

#define WIFIDIR_PEER_AGEOUT_TIME    30
#define WIFIDIR_GO_NEG_TIMEOUT 120
#define WIFIDIR_GO_UP_TIMER 1
#define MAX_WAIT_GO_COUNT   30

/* The following variables are undesirable globals from elsewhere in the system
 * on which we currently rely.  Please strive only to make this list shorter,
 * not longer!
 */
extern PWPS_INFO gpwps_info;
extern short ie_index;
extern short ap_assocresp_ie_index;

/* Currently, we support only one module instance */
struct module *wifidir_mod = NULL;

/* Wifi direct IE buffer index */
short wifidir_ie_index_base = -1;

/* TODO: A main function that parses a command line may want to set this config
 * file.  Really, that main function should parse the file itself, and all of
 * the data in the file should be stored in the wifidir_cfg struct.  But for
 * now, some parts of the config come from the file, and others from the call
 * to init.
 */
char *wifidir_cfg_file;

/* enum wifidir_state
 *
 * These are all the possible stats of the wifidir state machine.  The state is
 * not to be modified by anybody except the state machine.
 *
 * WIFIDIR_STATE_INIT: The wifidir state machine is initializing.  This is the state
 * immediately after calling wifidir_init.  All of the API functions will return
 * WIFIDIR_ERR_BUSY until the state machine transitions out of this state.
 *
 * WIFIDIR_STATE_IDLE: The wifidir state machine is idle.  wifidir_start_find()
 * can be invoked to start the find.  wifidir_negotiate_group() can be invoked
 * to initiate a connection
 *
 * WIFIDIR_STATE_GO_REQUEST_SENT: After a negotiate group command is initiated in
 * IDLE state, the state machine enters this state and expects to receive
 * EV_TYPE_WIFIDIR_NEGOTIATION_RESPONSE event. If the event is received, it goes
 * into WIFIDIR_STATE_WAIT_FOR_GO_RESULT.
 *
 * WIFIDIR_STATE_WAIT_FOR_GO_RESULT: The state machine expects to receive
 * EV_TYPE_WIFIDIR_NEGOTIATION_RESULT event. If the event is received,
 * WIFIDIR_EVENT_NEGOTIATION_RESULT event will be generated and sent back. Based
 * on the negotiated role, state machine will call wifidir_start_as_go or
 * wifidir_start_as_enrollee to start WPS 8-way handshake. If handshake succeeds,
 * it goes into WIFIDIR_STATE_GO or WIFIDIR_STATE_ENROLLED.
 *
 * WIFIDIR_STATE_WAIT_FOR_INITIATOR_GO_RESULT: The state machine expects to receive
 * EV_TYPE_WIFIDIR_NEGOTIATION_RESULT event as GO negotiation initiator. The reason
 * why this state is needed is that the status attribute of
 * EV_TYPE_WIFIDIR_NEGOTIATION_RESULT event received by initiator is invalid.
 * Initiator will receive valid status attribute in
 * EV_TYPE_WIFIDIR_NEGOTIATION_RESPONSE event.
 *
 * WIFIDIR_STATE_WAIT_FOR_ALLOW: The state machine has received a GO
 * negotiation request from an initiator, but the user has not called
 * wifidir_allow().  A negotiation response with status 1 (Information
 * Unavailable) has ben sent, and the initiator is expecting us to re-initiate
 * within 120 seconds as described in section 3.1.4.2.2 of the standard.
 *
 * WIFIDIR_STATE_EXPECT_GO_REQUEST: The state machine has received a GO
 * negotiation response with status 1 (Information Unavailable).  As described
 * in section 3.1.4.2.2 of the standard, we expect the sender of this GO
 * negotiation response to re-initiate within 120s.
 *
 * WIFIDIR_STATE_GO: The state machine won the GO negotiation.  It is now in a
 * steady state.  Other devices can connect to it via WPA or WPS.
 *
 * WIFIDIR_STATE_WAIT_FOR_GO_UP: The state machine lost the GO negotiation and
 * is waiting for GO up.
 *
 * WIFIDIR_STATE_CLIENT: The GO is up. So now in the CLIENT steady state.
 */
enum wifidir_state {
    WIFIDIR_STATE_INIT,
    WIFIDIR_STATE_IDLE,
    WIFIDIR_STATE_FIND,
    WIFIDIR_STATE_GO_REQUEST_SENT,
    WIFIDIR_STATE_WAIT_FOR_GO_RESULT,
    WIFIDIR_STATE_WAIT_FOR_INITIATOR_GO_RESULT,
    WIFIDIR_STATE_WAIT_FOR_ALLOW,
    WIFIDIR_STATE_EXPECT_GO_REQUEST,
    WIFIDIR_STATE_GO,
    WIFIDIR_STATE_WAIT_FOR_GO_UP,
    WIFIDIR_STATE_CLIENT,
};
enum wifidir_state __state = WIFIDIR_STATE_INIT;

/* allocate a cmdbuf suitable for passing to wifidir_send_cmdbuf() with cmd_len
 * bytes for trailing TLVs and other variable-length cmd data.  Returns NULL on
 * failure.  On success, caller MUST call free() to free the cmdbuf.  This
 * function is factored out from the commands in wfdutl.
 */
static mrvl_cmd_head_buf *wifidir_cmdbuf_alloc(int cmd_len, u16 code)
{
    mrvl_cmd_head_buf *cmd;
    int len;

    len = sizeof(mrvl_cmd_head_buf) + cmd_len;
    cmd = (mrvl_cmd_head_buf *)malloc(len);
    if (!cmd) {
        ERR("Failed to allocate cmdbuf\n");
        return NULL;
    }
    memset(cmd, 0, len);
    cmd->cmd_code = code;
    cmd->size = len;
    return cmd;
}

static enum wifidir_error wifidir_cmdbuf_send(mrvl_cmd_head_buf *cmd)
{
    int ret;
    u16 cmd_len = cmd->size;
    u16 cmd_code_in = cmd->cmd_code;

    ret = wifidir_ioctl(wifidir_mod->iface, (u8 *)cmd, &cmd_len, cmd_len);
    if (ret != WIFIDIR_ERR_SUCCESS) {
        ERR("Failed to send wifidir command to driver.\n");
        return WIFIDIR_ERR_COM;
    }

    if (cmd->cmd_code != (cmd_code_in | WIFIDIRCMD_RESP_CHECK)) {
        ERR("Corrupted response from driver!\n");
        return WIFIDIR_ERR_COM;
    }

    if (cmd->result != 0) {
        ERR("Non-zero result from driver: %d\n", cmd->result);
        return WIFIDIR_ERR_COM;
    }

    return WIFIDIR_ERR_SUCCESS;
}

static inline const char *wifidir_state_to_str(enum wifidir_state state)
{
    switch (state) {
    case WIFIDIR_STATE_INIT:
        return "INIT";
    case WIFIDIR_STATE_IDLE:
        return "IDLE";
    case WIFIDIR_STATE_GO_REQUEST_SENT:
        return "GO_REQUEST_SENT";
    case WIFIDIR_STATE_WAIT_FOR_GO_RESULT:
        return "WAIT_FOR_GO_RESULT";
    case WIFIDIR_STATE_GO:
        return "GO";
    case WIFIDIR_STATE_CLIENT:
        return "CLIENT";
    case WIFIDIR_STATE_FIND:
        return "FIND";
    case WIFIDIR_STATE_WAIT_FOR_INITIATOR_GO_RESULT:
        return "WAIT_FOR_INITIATOR_GO_RESULT";
    case WIFIDIR_STATE_WAIT_FOR_ALLOW:
        return "WIFIDIR_STATE_WAIT_FOR_ALLOW";
    case WIFIDIR_STATE_EXPECT_GO_REQUEST:
        return "WIFIDIR_STATE_EXPECT_GO_REQUEST";
    case WIFIDIR_STATE_WAIT_FOR_GO_UP:
        return "WIFIDIR_STATE_WAIT_FOR_GO_UP";
    default:
        return "UNKNOWN";
    }
}

/* We maintain a list of peers that we know about.  For this we use a BSD SLIST
 * called peers.
 */
static struct peer_list peers;

/* wifidir_peer_init: initialize a peer to its default values
 */
static void wifidir_peer_init(struct wifidir_peer *p)
{
    memset(p, 0, sizeof(struct wifidir_peer));
    p->op_channel = 0;
    p->peer_channel_list.num_of_chan = 0;
    p->go_neg_status = WIFIDIR_GO_NEG_INTERNAL_ERR;
}

/* wifidir_peer_print: print peer data structure
 */
static void wifidir_peer_print(struct wifidir_peer *p)
{
    INFO("device_name: %s\n", p->device_name);
    INFO("device_address: " UTIL_MACSTR "\n", UTIL_MAC2STR(p->device_address));
    INFO("group capability: %X\n", p->group_capability);
    INFO("interface_address: " UTIL_MACSTR "\n", UTIL_MAC2STR(p->interface_address));
    INFO("go_ssid: %s\n", p->go_ssid);
    INFO("methods: %04X\n", p->methods);
    INFO("go_neg_status: %d\n", p->go_neg_status);
    INFO("go_op_channel: %d\n", p->op_channel);
}

/* wifidir_peer_alloc: allocate a new peer
 *
 * returns: the zero-initialized peer or NULL on error.
 */
static struct wifidir_peer *wifidir_peer_alloc(void)
{
    struct wifidir_peer *p;
    p = malloc(sizeof(struct wifidir_peer));
    if (p != NULL)
        wifidir_peer_init(p);
    return p;
}

/* wifidir_peer_free: free a peer that was allocated by wifidir_peer_alloc().
 *
 * The caller must ensure that the peer is not in the peer list prior to
 * calling this function.  That is, either the peer was never added to the list
 * using wifidir_peer_insert(), or it was removed from the list with
 * wifidir_peer_remove().
 *
 * peer: the peer to free
 */
static void wifidir_peer_free(struct wifidir_peer *peer)
{
    if (peer == NULL)
        return;
    free(peer);
}

/* given a pointer to a list of WIFIDIR IEs, parse them and put the data in the
 * supplied peer struct
 */
static enum wifidir_error wifidir_parse_wifidir_tlvs(u8 *ptr, int buflen,
                                                 struct wifidir_peer *peer)
{

    tlvbuf_wifidir_capability *wifidir_tlv1;
    tlvbuf_wifidir_device_info *wifidir_tlv2;
    tlvbuf_wifidir_device_id *wifidir_tlv3;
    tlvbuf_wifidir_group_id *wifidir_tlv4;
    tlvbuf_wifidir_intended_addr *wifidir_tlv5;
    tlvbuf_wifidir_status *wifidir_status_tlv;
    tlvbuf_wifidir_config_timeout *wifidir_cfg_timeout_tlv;
    tlvbuf_wifidir_channel *wifidir_opchannel;
    tlvbuf_wifidir_channel_list *wifidir_chan_list;
    int left_len = buflen, i = 0;
    u16 len;
    u8 type = 0;
    wifidir_device_name_info *array_ptr;
    u16 dev_name_len, len_wifidirect = 0;
    int ssid_len;
    chan_entry *temp_ptr = NULL;

    while (left_len > WIFIDIR_IE_HEADER_LEN) {
        type = *ptr;
        memcpy(&len, ptr + 1, sizeof(u16));
        len = wlan_le16_to_cpu(len);

        /* Sanity check on data length */
        if (left_len < WIFIDIR_IE_HEADER_LEN + len) {
            WARN("Corrupt IE, type = 0x%x, len = %d, left = %d\n",
                 type, len, left_len);
            return WIFIDIR_ERR_INVAL;
        }

        switch (type) {
        case TLV_TYPE_WIFIDIR_CAPABILITY:
            wifidir_tlv1 = (tlvbuf_wifidir_capability *) ptr;
            peer->group_capability = wifidir_tlv1->group_capability;
            break;

        case TLV_TYPE_WIFIDIR_DEVICE_ID:
            wifidir_tlv3 = (tlvbuf_wifidir_device_id *) ptr;
            memcpy(peer->device_address, wifidir_tlv3->dev_mac_address,
                   ETH_ALEN);
            break;

        case TLV_TYPE_WIFIDIR_INTENDED_ADDRESS:
            wifidir_tlv5 = (tlvbuf_wifidir_intended_addr *) ptr;
            memcpy(peer->interface_address, wifidir_tlv5->group_address,
                   ETH_ALEN);
            break;

        case TLV_TYPE_WIFIDIR_GROUP_ID:
            wifidir_tlv4 = (tlvbuf_wifidir_group_id *) ptr;
            ssid_len = len - (sizeof(tlvbuf_wifidir_group_id) - WIFIDIR_IE_HEADER_LEN);
            memcpy(peer->go_ssid, wifidir_tlv4->group_ssid, ssid_len);
            peer->go_ssid[ssid_len] = 0;
            break;

        case TLV_TYPE_WIFIDIR_DEVICE_INFO:
            wifidir_tlv2 = (tlvbuf_wifidir_device_info *) ptr;

            memcpy(peer->device_address, wifidir_tlv2->dev_address,
                   ETH_ALEN);
            /* device name */
            array_ptr = (wifidir_device_name_info *)
                        ((u8 *)&wifidir_tlv2->secondary_dev_count +
                        sizeof(wifidir_tlv2->secondary_dev_count) +
                        wifidir_tlv2->secondary_dev_count * WPS_DEVICE_TYPE_LEN);
            dev_name_len = mwu_ntohs(array_ptr->device_name_len);
            if (dev_name_len > MAX_DEVICE_NAME_LEN) {
                ERR("Device name from peer is too long\n");
                return WIFIDIR_ERR_INVAL;
            }
            memcpy(peer->device_name, array_ptr->device_name, dev_name_len);
            /* config methods */
            peer->methods = mwu_ntohs(wifidir_tlv2->config_methods);
            break;

        case TLV_TYPE_WIFIDIR_STATUS:
            wifidir_status_tlv = (tlvbuf_wifidir_status *)ptr;
            peer->go_neg_status = wifidir_status_tlv->status_code;
            break;

        case TLV_TYPE_WIFIDIR_CONFIG_TIMEOUT:
            wifidir_cfg_timeout_tlv = (tlvbuf_wifidir_config_timeout *)ptr;
            /* The value is in 10s of msecs, convert to msecs */
            peer->group_cfg_timeout =
                wifidir_cfg_timeout_tlv->group_config_timeout * 10;
            break;

        case TLV_TYPE_WIFIDIR_OPCHANNEL:
            wifidir_opchannel = (tlvbuf_wifidir_channel *) ptr;
            peer->op_channel = wifidir_opchannel->channel_number;
            break;

        case TLV_TYPE_WIFIDIR_CHANNEL_LIST:
            wifidir_chan_list = (tlvbuf_wifidir_channel_list *) ptr;
            temp_ptr = (chan_entry *) wifidir_chan_list->wifidir_chan_entry_list;

            len_wifidirect = wlan_le16_to_cpu(wifidir_chan_list->length) -
                (sizeof(tlvbuf_wifidir_channel_list) -
                 WIFIDIR_IE_HEADER_LEN);

            /* Multiple channel entries */
            if (len_wifidirect >
                    (sizeof(chan_entry) + temp_ptr->num_of_channels)) {
                while (len_wifidirect) {
                    if ((int) (temp_ptr->regulatory_class) ==
                            WIFI_REG_CLASS_81) {
                        peer->peer_channel_list.num_of_chan =
                            temp_ptr->num_of_channels;
                        for (i = 0; i < temp_ptr->num_of_channels;
                                i++) {
                            peer->peer_channel_list.chan[i] =
                                *(temp_ptr->chan_list + i);
                        }
                        break;
                    }
                    len_wifidirect -= sizeof(chan_entry) + temp_ptr->num_of_channels;
                    temp_ptr += sizeof(chan_entry) + temp_ptr->num_of_channels;
                }
            } else {
                /* Only one channel entry */
                peer->peer_channel_list.num_of_chan = temp_ptr->num_of_channels;
                for (i = 0; i < temp_ptr->num_of_channels; i++) {
                    peer->peer_channel_list.chan[i] = *(temp_ptr->chan_list + i);
                }
            }
            break;
        default:
            /*ERR("==> 0x%x: tlv not parsed", type); */
            break;
        }
        ptr += len + WIFIDIR_IE_HEADER_LEN;
        left_len -= len + WIFIDIR_IE_HEADER_LEN;
    }

    return WIFIDIR_ERR_SUCCESS;
}


/* wifidir_peer_parse: populate the specified peer with data from the peer
 * discovered event
 *
 * This should be the sole parser used by wifidir.  Please do not add special
 * parsers to extract single specific TLVs from a driver event.  Instead,
 * extend this parser to parse the TLV, and make wifidir_peer_init() set the
 * field where your new TLV will be stored to a known, internal value
 * indicating that it is not valid (e.g., all 0s or -1).
 *
 * wifidir_event: event from driver
 * peer: peer to populate
 *
 * returns:
 *
 * 0: success.  The peer is up to date.
 *
 * -1: error.  The contents of the event were invalid.
 *
 * NOTE: This function is based on the function "wifidir_update_find_table" in
 * wpswifidir.
 *
 * NOTE: This function must not be used to operate on peers that are in the
 * peer list because it memsets the peer to 0.  If you want to update a peer in
 * the table, you must first parse the wifidir_event into a dummy peer and then use
 * wifidir_peer_update() to update the peer in the table.
 */
static int wifidir_peer_parse(apeventbuf_wifidir_generic *wifidir_event,
                       struct wifidir_peer *peer)
{
    const u8 wifi_oui[3] = { 0x50, 0x6F, 0x9A };
    const u8 wps_oui[3] = { 0x00, 0x50, 0xF2 };
    int i = 0;
    u8 *ptr, *array_ptr;
    u8 type = 0;
    s16 left_len = 0;
    u16 len = 0, wifidir_wps_len = 0, wps_len = 0, wps_type = 0;
    wifidir_ie_header *wifidir_wps_header;
    int ret;

    ENTER();

    if (!memcmp(wifidir_event->peer_mac_addr, "\x00\x00\x00\x00\x00\x00", ETH_ALEN)) {
        ERR("Invalid peer mac address in event.  Firmware error?");
        return -1;
    }

    wifidir_peer_init(peer);
    /* We blindly populate this here, It doesn't harm. If the device changes its
     * interface address after negotiation, it will anyway get populated while
     * parsing the p2p tlv TLV_TYPE_WIFIDIR_INTENDED_ADDRESS */
    memcpy(peer->interface_address, wifidir_event->peer_mac_addr, ETH_ALEN);

    wifidir_wps_header = (wifidir_ie_header *) (wifidir_event->entire_ie_list);
    wifidir_wps_len = wlan_le16_to_cpu(wifidir_event->event_length)
        - sizeof(apeventbuf_wifidir_generic);

    while (wifidir_wps_len >= sizeof(wifidir_ie_header)) {
        if (wifidir_wps_len < wifidir_wps_header->ie_length + IE_HEADER_LEN) {
            ERR("Invalid IE in event. Firmware Error?");
            break;
        }

        if (!memcmp(wifidir_wps_header->oui, wifi_oui, sizeof(wifi_oui)) ||
            !memcmp(wifidir_wps_header->oui, wps_oui, sizeof(wps_oui))) {

            switch (wifidir_wps_header->oui_type) {
            case WIFIDIR_OUI_TYPE:
                ptr = wifidir_wps_header->ie_list;
                left_len =
                    wifidir_wps_header->ie_length - sizeof(wifidir_wps_header->oui)
                    - sizeof(wifidir_wps_header->oui_type);

                ret = wifidir_parse_wifidir_tlvs(ptr, left_len, peer);
                if (ret != WIFIDIR_ERR_SUCCESS)
                    return -1;

                break;

            case WIFI_WPS_OUI_TYPE:
                ptr = wifidir_wps_header->ie_list;
                left_len = wifidir_wps_header->ie_length
                           - sizeof(wifidir_wps_header->oui)
                           - sizeof(wifidir_wps_header->oui_type);

                while ((u32)left_len > sizeof(tlvbuf_wps_ie)) {
                    memcpy(&wps_type, ptr, sizeof(u16));
                    wps_type = mwu_ntohs(wps_type);
                    memcpy(&wps_len, ptr + 2, sizeof(u16));
                    wps_len = mwu_ntohs(wps_len);

                    /* Look for WPS Device Name Len */
                    if (wps_type == SC_Device_Name && peer->device_name[0] == 0) {

                        /* sanity check */
                        if (((u32)left_len < sizeof(tlvbuf_wps_ie) + wps_len)
                                || (wps_len > MAX_DEVICE_NAME_LEN)) {
                            ERR("Corrupt WPS IE, type = 0x%x, len %d\n",
                                    wps_type, wps_len);
                            return -1;
                        }
                        tlvbuf_wps_ie *wps_tlv = (tlvbuf_wps_ie *) ptr;
                        array_ptr = wps_tlv->data;
                        memcpy(peer->device_name, array_ptr, wps_len);

                    } else if (wps_type == SC_Config_Methods && peer->methods == 0) {
                        /* Read config methods from wps ie if they are not
                         * parsed from p2p ie.*/
                        tlvbuf_wps_ie *wps_tlv = (tlvbuf_wps_ie *)ptr;
                        array_ptr = wps_tlv->data;
                        memcpy(&peer->methods, array_ptr, wps_len);
                        peer->methods = mwu_ntohs(peer->methods);
                    }

                    ptr += wps_len + sizeof(tlvbuf_wps_ie);
                    left_len -= wps_len + sizeof(tlvbuf_wps_ie);
                }
                break;
            }
        }
        wifidir_wps_len -= wifidir_wps_header->ie_length + IE_HEADER_LEN;
        wifidir_wps_header = (wifidir_ie_header *) (((u8 *) wifidir_wps_header) +
                                            wifidir_wps_header->ie_length +
                                            IE_HEADER_LEN);
    }

    /*
     * We get the discovery events as a result of either a pribe request or
     * probe response. If this event is from a probe response, then there is no
     * problem. TLV_TYPE_WIFIDIR_DEVICE_INFO is a must attribute in probe
     * response, so we get the correct device_address from it.
     * But, if this event is from a probe request, then then spec doesn't
     * mandate it to have TLV_TYPE_WIFIDIR_DEVICE_ID attribute. Now, ths is
     * odd. Why not? How should we set the device_address if it doesn't appear
     * in the event? Or, in another case, if this event is from GO neg result
     * with status 1 (info unavailable), i.e. this device sends out the GO neg
     * result with status 1, then there'll be no P2P TLV in this event and the
     * parsed device_address will be all zero.
     *
     * To workaround, we currently use the peer_mac_addr, which is the address
     * from which packet was received. While this seems to work, it is not
     * strictly correct.
     */
    if (!memcmp(peer->device_address, "\x00\x00\x00\x00\x00\x00", ETH_ALEN))
        memcpy(peer->device_address, wifidir_event->peer_mac_addr, ETH_ALEN);

    return 0;
}


/* wifidir_next_expiring_peer: find a peer in peer table which has least
 * expiry time
 *
 * returns: peer which is next to expire
 */
static struct wifidir_peer *wifidir_next_expiring_peer()
{
    struct wifidir_peer *current = NULL;
    struct wifidir_peer *next = NULL;

    SLIST_FOREACH(current, &peers, list_item) {
        if (next == NULL) {
            next = current;
            continue;
        }
        if (current->expiry_time.sec < next->expiry_time.sec) {
            next = current;
        } else if (current->expiry_time.sec == next->expiry_time.sec &&
                   current->expiry_time.usec < next->expiry_time.usec) {
            next = current;
        }
    }

    return next;
}




/* wifidir_peer_lookup: find a peer in the list by its device_address
 *
 * device_address: device_address of the peer to be looked up.
 *
 * returns: If the peer with that device_address is in the list, it will be
 * returned.  Otherwise, NULL is returned.
 */
static struct wifidir_peer *wifidir_peer_lookup(unsigned char *device_address)
{
    struct wifidir_peer *found = NULL;

    SLIST_FOREACH(found, &peers, list_item) {
        if (memcmp(found->device_address, device_address, ETH_ALEN) == 0)
            return found;
    }
    return NULL;
}

/* wifidir_peer_insert: insert a peer into the list
 *
 * peer: the non-NULL peer to insert.
 *
 * returns: 0: the peer was successfully inserted.  -1: An error occurred.
 * (E.g., a peer with the same device id as the argument peer is already in the
 * list).
 *
 */
static int wifidir_peer_insert(struct wifidir_peer *peer)
{
    struct wifidir_peer *found;

    /* ensure that we're not already monitoring this service */
    found = wifidir_peer_lookup(peer->device_address);
    if (found != NULL) {
        ERR("Peer was already in the list.\n");
        return -1;
    }
    SLIST_INSERT_HEAD(&peers, peer, list_item);

    return 0;
}



/* wifidir_peer_remove: remove the specified peer from the list
 *
 * peer: the peer to remove
 *
 * NOTE: This routine does not free the peer.  The caller is responsible for
 * that.
 */
static void wifidir_peer_remove(struct wifidir_peer *peer)
{
    struct wifidir_peer *found;

    found = wifidir_peer_lookup(peer->device_address);
    if (found == NULL) {
        WARN("Can't remove peer because it's not in the list\n");
        return;
    }
    if (found != peer) {
        ERR("BUG: Detected two peers with same device_address in list!\n");
    }
    SLIST_REMOVE(&peers, found, wifidir_peer, list_item);
}


static void wifidir_emit_peer_unfound(struct wifidir_peer *peer)
{
    struct event *e = malloc(sizeof(struct event) +
                             ETH_ALEN);
    e->type = WIFIDIR_EVENT_PEER_UNFOUND;
    e->len = ETH_ALEN;
    memcpy(e->val, peer->device_address, ETH_ALEN);
    MODULE_DO_CALLBACK(wifidir_mod, e);
    free(e);
}

/* Unfortunately, we can only cancel a timer if we know what private data we
 * passed to it when we set it.  So our peer expiration timer needs to always
 * have the same private data if we want to be able to reset it.  So we
 * introduce a container for it.
 */
struct peer_timeout {
    struct wifidir_peer *peer;
};
static struct peer_timeout pto;

static void wifidir_evict_expired_peer(void *peer_timeout);
static void wifidir_reset_peer_timer(void)
{
    struct wifidir_peer *next_expiring_peer;
    struct mwu_timeval now;
    mwu_time_t sec;
    mwu_time_t usec;

    if (mwu_get_time(&now) != MWU_ERR_SUCCESS) {
        ERR("Unexpected: failed to fetch time.\n");
        return;
    }
    wps_cancel_timer(wifidir_evict_expired_peer, &pto);

    /* reset the timer to the next peer to expire */
    next_expiring_peer = wifidir_next_expiring_peer();

    if (next_expiring_peer != NULL) {
        /* calculate the soonest expiration time */
        sec = next_expiring_peer->expiry_time.sec   - now.sec;
        usec = next_expiring_peer->expiry_time.usec - now.usec;

        /*reset the timer after exp_time*/
        pto.peer = next_expiring_peer;
        wps_start_timer(sec, usec, wifidir_evict_expired_peer, &pto);
    }
}

/* wifidir_evict_expired_peer: removes the expired peer and frees it
 * Also emits an event for higher layer
 *
 * expired_peer: the peer who has been went off
 */

static void wifidir_evict_expired_peer(void *peer_timeout)
{
    struct peer_timeout *__pto = peer_timeout;
    struct wifidir_peer *peer  = NULL;
    int exp_time;

    if (__pto != &pto) {
        /* This is highly unexpected */
        ERR("Got timeout with invalid peer_timeout data!\n");
        return;
    }

    peer = wifidir_peer_lookup(__pto->peer->device_address);
    if (peer == NULL) {
        WARN("Got timeout for non-existant peer.  Already freed?\n");
        return;
    }

    INFO("Peer expired:\n");
    wifidir_peer_print(peer);

    /* emit the event */
    wifidir_emit_peer_unfound(peer);

    /* Remove this peer */
    wifidir_peer_remove(peer);
    wifidir_peer_free(peer);

    wifidir_reset_peer_timer();
    return;
}

/* wifidir_peer_free_all: free all the peers in the list
 *
 * This function is used at tear-down time to completely free all of the peers
 * in the list.
 */
static void wifidir_peer_free_all(void)
{
    struct wifidir_peer *found;

    while (!SLIST_EMPTY(&peers)) {
        found = SLIST_FIRST(&peers);
        SLIST_REMOVE_HEAD(&peers, list_item);
        wifidir_peer_free(found);
    }
}

/* wifidir_peer_equals: check if two peers are equivalent
 *
 * returns 1 if the peers are equal, otherwise 0.
 */
static int wifidir_peer_equals(struct wifidir_peer *p1, struct wifidir_peer *p2)
{
    if (p1 == p2)
        return 1;

    /* two peers are equal if all of their data is equal, not counting the
     * list_item.  So memcmp by itself does not work.  So we compare the
     * relevant members manually:
     */
    if (
        memcmp(p1->device_address, p2->device_address, ETH_ALEN) == 0 &&
        memcmp(p1->device_name, p2->device_name, MAX_DEVICE_NAME_LEN) == 0 &&
        p1->group_capability == p2->group_capability &&
        memcmp(p1->interface_address, p2->interface_address, ETH_ALEN) == 0) {
        return 1;
    }
    return 0;
}

/* wifidir_peer_update: update peer in list with data from the specified peer
 *
 * peer: peer with updated data
 *
 * returns:
 *
 * 0: the peer was updated
 *
 * -1: an error occured (e.g., no peer with specified device_address exists in the
 * list).
 */
static int wifidir_peer_update(struct wifidir_peer *peer)
{
    struct wifidir_peer *found;

    found = wifidir_peer_lookup(peer->device_address);
    if (found == NULL) {
        WARN("Can't update peer because it's not in the list\n");
        return -1;
    }
    if (found == peer)
        return 0;

    /* the easiest way to do this is to remove the peer, update it, then add it
     * back.  This allows us to use a blatant memcpy without messing up the
     * list state.
     */
    SLIST_REMOVE(&peers, found, wifidir_peer, list_item);
    memcpy(found, peer, sizeof(struct wifidir_peer));
    SLIST_INSERT_HEAD(&peers, found, list_item);
    return 0;
}

/* wifidir_handle_peer_detected: update peer list as nec. with wifi direct
 * peer-detected event
 *
 * e: the event from the driver
 * peer: *new_peer will point to updated/new peer after call
 *
 * returns:
 *
 * -1 if there was an error.  In this case, *peer is invalid.  Check logs for
 * output.  This return code indicates a bug in wifidir or an out-of-memory
 * error.
 *
 * 0 if the peer was already in the list.  In this case, *peer will point to
 * the existing peer.
 *
 * 1 if the peer was added to the list.  In this case, *peer will point to the
 * new peer.
 *
 * 2 if the peer was already in the list but has been updated.  In this case,
 * *peer will point to the updated peer.
 */
static int wifidir_handle_peer_detected(apeventbuf_wifidir_generic *e,
                                        struct wifidir_peer **peer)
{
    struct wifidir_peer *existing_peer;
    struct wifidir_peer *next_expiring_peer = NULL;
    int ret = -1, exp_time;

    *peer = wifidir_peer_alloc();
    if (*peer == NULL) {
        ERR("Failed to allocate new peer.\n");
        return -1;
    }
    ret = wifidir_peer_parse(e, *peer);
    if (ret != WIFIDIR_ERR_SUCCESS) {
        ERR("Failed to parse peer detected event.\n");
        goto fail;
    }

    if (!memcmp((*peer)->device_address, "\x00\x00\x00\x00\x00\x00", ETH_ALEN)
        || strlen((*peer)->device_name) == 0) {
        WARN("Ignoring invalid peer whose device_address is all zero"
             " or device_name is empty.\n");
        goto fail;
    }

    existing_peer = wifidir_peer_lookup((*peer)->device_address);
    if (existing_peer == NULL) {
        /* this is a new peer that we haven't seen yet */
        if (mwu_get_time(&(*peer)->expiry_time))
            WARN("Unexpected: failed to fetch time for detected peer.\n");
        (*peer)->expiry_time.sec += WIFIDIR_PEER_AGEOUT_TIME;
        ret = wifidir_peer_insert(*peer);
        if (ret != WIFIDIR_ERR_SUCCESS) {
            ERR("Failed to add new peer to list.\n");
            goto fail;
        }
        ret = 1;
    } else {
        if (wifidir_peer_equals(*peer, existing_peer)) {
            /* This peer is already in the list */
            wifidir_peer_free(*peer);
            *peer = existing_peer;
            if (mwu_get_time(&existing_peer->expiry_time))
                WARN("Unexpected: failed to fetch time for existing peer.\n");
            existing_peer->expiry_time.sec += WIFIDIR_PEER_AGEOUT_TIME;
            ret = 0;
        } else {
            /* This peer has been updated */
            if (mwu_get_time(&(*peer)->expiry_time))
                WARN("Unexpected: failed to fetch time for updated peer.\n");
            (*peer)->expiry_time.sec += WIFIDIR_PEER_AGEOUT_TIME;
            ret = wifidir_peer_update(*peer);
            if (ret != WIFIDIR_ERR_SUCCESS) {
                ERR("Failed to update existing peer.\n");
                goto fail;
            }
            wifidir_peer_free(*peer);
            *peer = existing_peer;
            ret = 2;
        }
    }

    wifidir_reset_peer_timer();

    return ret;
fail:
    if (*peer)
        wifidir_peer_free(*peer);
    return -1;
}

/* API Commands: Often, an API call needs to call the state machine
 * synchronously.  For this, we use API commands.  They are packaged in an
 * "event" as described in module.h.  Note, however, that these API commands
 * are only sent internally.  Outsiders should only invoke the API defined in
 * wifidir.h.
 *
 * WIFIDIR_UI_CMD_INIT: This command is sent with struct wifidir_cfg in
 * the val buffer.
 *
 * WIFIDIR_UI_CMD_DEINIT: This command is sent with an empty val buffer
 *
 * WIFIDIR_UI_CMD_PDREQ: This command is sent with struct wifidir_pd_info in
 * the val buffer.
 *
 * WIFIDIR_UI_CMD_NEGOTIATE_GROUP: This command is sent with struct wifidir_pd_info
 * in the val buffer.
 *
 * WIFIDIR_UI_CMD_START_GROUP: This command is sent with struct
 * wifidir_group_info. This command should start Auto GO.
 */
enum wifidir_ui_cmd_type {
    WIFIDIR_UI_CMD_INIT,
    WIFIDIR_UI_CMD_DEINIT,
    WIFIDIR_UI_CMD_PDREQ,
    WIFIDIR_UI_CMD_NEGOTIATE_GROUP,
    WIFIDIR_UI_CMD_START_FIND,
    WIFIDIR_UI_CMD_STOP_FIND,
    WIFIDIR_UI_CMD_ALLOW,
    WIFIDIR_INTERNAL_EVENT_GO_TIMEOUT,
    WIFIDIR_UI_CMD_START_GROUP,
};

static inline const char *wifidir_ui_cmd_to_str(enum wifidir_ui_cmd_type cmd)
{
    switch (cmd) {
    default:
    case WIFIDIR_UI_CMD_INIT:
        return "INIT";
    case WIFIDIR_UI_CMD_DEINIT:
        return "DEINIT";
    case WIFIDIR_UI_CMD_PDREQ:
        return "PDREQ";
    case WIFIDIR_UI_CMD_NEGOTIATE_GROUP:
        return "NEGOTIATE_GROUP";
    case WIFIDIR_UI_CMD_START_FIND:
        return "START_FIND";
    case WIFIDIR_UI_CMD_STOP_FIND:
        return "STOP_FIND";
    case WIFIDIR_UI_CMD_ALLOW:
        return "ALLOW";
    case WIFIDIR_INTERNAL_EVENT_GO_TIMEOUT:
        return "GO_TIMEOUT";
    }
}

/* change the state */
static void change_state(enum wifidir_state new)
{
    INFO("Changing state from %s to %s\n", wifidir_state_to_str(__state),
         wifidir_state_to_str(new));
    if (new == __state)
        return;
    __state = new;
}

/* NOTE: This function is based on wifidir_wlan_update_mode.  But that function is
 * way too smart for its own good.  It recursively calls itself trying to
 * achieve a certain mode instead of just setting the mode.  This has been the
 * root cause of two freeze bugs.
 */
static enum wifidir_error wifidir_set_mode(u16 mode)
{
    int ret;
    mrvl_cmd_head_buf *cmd = NULL;
    wifidir_mode_config *wifidir_mode = NULL;

    cmd = wifidir_cmdbuf_alloc(sizeof(wifidir_mode_config),
                               HostCmd_CMD_WIFIDIR_MODE_CONFIG);
    if (!cmd)
        return WIFIDIR_ERR_NOMEM;

    wifidir_mode = (wifidir_mode_config *)cmd->cmd_data;
    wifidir_mode->action = wlan_cpu_to_le16(ACTION_SET);
    wifidir_mode->mode = wlan_cpu_to_le16(mode);

    ret = wifidir_cmdbuf_send(cmd);

    free(cmd);
    return ret;
}

/* Internal function to stop the find phase
 *
 * params:
 *      none
 *
 * returns:
 *      WIFIDIR_ERR_SUCCESS -    find phase stopped successfully
 *      WIFIDIR_ERR_COM     -    lower lever ioctl failed
 */
static int wifidir_do_stop_find_phase(void) {
    return wifidir_set_mode(WIFIDIR_MODE_STOP_FIND_PHASE);
}

/* Internal function to start the find phase
 *
 * params:
 *      none
 *
 * return:
 *      WIFIDIR_ERR_SUCCESS -    find phase started successfully
 *      WIFIDIR_ERR_COM     -    lower lever ioctl failed
 */
static int wifidir_do_start_find_phase(void) {
    return wifidir_set_mode(WIFIDIR_MODE_START_FIND_PHASE);
}

static void wifidir_go_timeout(void *data)
{
    struct event *ui_cmd;

    ui_cmd = (struct event *)malloc(sizeof(struct event) +
                                    sizeof(struct wifidir_peer));
    if (!ui_cmd) {
        ERR("Cannot send GO timeout event.  No mem!\n");
        return;
    }
    ui_cmd->type = WIFIDIR_INTERNAL_EVENT_GO_TIMEOUT;
    ui_cmd->len = 0;

    wifidir_state_machine((u8 *)ui_cmd, ui_cmd->len, WIFIDIR_EVENTS_UI);
}

/* 1. Find phase is stopped if started earlier
 * 2. GO is down / WIFIDIR client is idle
 * 3. Disable WIFIDIR
 */
static
void wifidir_do_deinit()
{
    WPS_DATA *wps_s = (WPS_DATA *) &gpwps_info->wps_data;

    char *ifname = NULL;

    if (wifidir_do_stop_find_phase() != WIFIDIR_ERR_SUCCESS) {
        WARN("Failed to stop find phase.\n");
    }

    if (wifidir_set_mode(WIFIDIR_MODE_STOP) != WIFIDIR_ERR_SUCCESS) {
        WARN("Failed to stop WIFIDIR mode.\n");
    }

    ifname = (wifidir_mod) ? wifidir_mod->iface : wps_s->ifname;
    mwu_ie_config(ifname, CLEAR_WPS_IE, (short int *)&wifidir_ie_index_base);

    /* Clear IE indices used by WPS module */
    if(ie_index != -1) {
        wps_wlan_ie_config(CLEAR_WPS_IE, &ie_index);
        ie_index = -1;
    }

    /* Clear Assoc response IE */
    if(ap_assocresp_ie_index != -1) {
        wps_wlan_ie_config(CLEAR_AR_WPS_IE, &ap_assocresp_ie_index);
        ap_assocresp_ie_index = -1;
    }

    wifidir_ie_index_base = -1;

    wps_cancel_timer(wifidir_go_timeout, NULL);
}

/** Borrowed from wfdutl.c
 *
 *  @brief Set wps status (e.g., has PIN bin collected by user?  Has PBC button
 *  been pushed?
 *
 *  @param method the intended provisioning method
 *
 *  @return         enum wifidir_error
 */
static enum wifidir_error
wifidir_cfg_cmd_set_wps_params(enum module_config_method method)
{
    mrvl_cmd_head_buf *cmd;
    wifidir_params_config *wifidir_params;
    tlvbuf_wifidir_wps_params *tlv;
    enum wifidir_error ret = WIFIDIR_ERR_SUCCESS;

    cmd = wifidir_cmdbuf_alloc(sizeof(wifidir_params_config) +
                               sizeof(tlvbuf_wifidir_wps_params),
                               HostCmd_CMD_WIFIDIR_PARAMS_CONFIG);
    if (!cmd)
        return WIFIDIR_ERR_NOMEM;

    wifidir_params = (wifidir_params_config *)cmd->cmd_data;
    wifidir_params->action = wlan_cpu_to_le16(ACTION_SET);

    tlv = (tlvbuf_wifidir_wps_params *)wifidir_params->wifidir_tlv;
    tlv->tag = MRVL_WIFIDIR_WPS_PARAMS_TLV_ID;
    tlv->length = sizeof(tlvbuf_wifidir_wps_params) - TLV_HEADER_SIZE;

    /* these params are hard-coded in wfdutl.  That is the only reference that
     * we have for their meaning.
     */
    if (method == MODULE_CONFIG_METHOD_NONE)
        tlv->action = wlan_cpu_to_le16(0);
    else if(method & MODULE_CONFIG_METHOD_DISPLAY ||
            method & MODULE_CONFIG_METHOD_KEYPAD)
        tlv->action = wlan_cpu_to_le16(1);
    else if(method & MODULE_CONFIG_METHOD_PBC)
        tlv->action = wlan_cpu_to_le16(2);
    else {
        ERR("Config method must be DISPLAY, KEYPAD, or PBC\n");
        ret = WIFIDIR_ERR_INVAL;
        goto done;
     }
    endian_convert_tlv_header_out(tlv);

    ret = wifidir_cmdbuf_send(cmd);

done:
    free(cmd);
    return ret;
}

static
enum wifidir_error wifidir_do_init(struct wifidir_cfg *cfg)
{
    int exitcode = WIFIDIR_ERR_SUCCESS;
    WPS_DATA *wps_s = (WPS_DATA *) &gpwps_info->wps_data;
    int ret, bss_type;

    /*
     * Download WIFIDIR configuration if supplied with -d command line.
     */
    if (wifidir_cfg_file) {
        INFO("WIFIDIR Config file '%s'", wifidir_cfg_file);
        if (wifidircmd_config_download(wifidir_cfg_file, cfg) != WPS_STATUS_SUCCESS) {
            ERR("Fail to download WIFIDIR configuration.!\n");
            exitcode = WIFIDIR_ERR_COM;
            goto fail;
        }
    }
    /* Fetch and store the channel list */
    wifidir_check_self_channel_list();

    ret = mwu_get_bss_type(wifidir_mod->iface, &bss_type);
    if (ret != MWU_ERR_SUCCESS) {
        WARN("Failed to get BSS type\n");
        exitcode = ret;
        goto fail;
    }

    if (bss_type != BSS_TYPE_STA)
        WARN("WARNING: Expected to be in STA mode!  Proceeding anyway.\n");

    /* start WIFIDIR mode */
    exitcode = wifidir_set_mode(WIFIDIR_MODE_START);
    if (exitcode != WIFIDIR_ERR_SUCCESS) {
        ERR("Failed to start wifidirect mode.\n");
        goto fail;
    }

    /* start us off indicating that no config method has been selected.  This
     * forces us to respond to any GO negotiation requests with status 1
     * "Information currently unavailable".
     */
    exitcode = wifidir_cfg_cmd_set_wps_params(MODULE_CONFIG_METHOD_NONE);
    if (exitcode != WIFIDIR_ERR_SUCCESS) {
        ERR("Failed to set wps params to NONE.\n");
        goto fail;
    }

    return exitcode;

fail:
    wifidir_do_deinit();
    return exitcode;
}

/** Borrowed from wfdutl.c
 *  @brief Set wifidir provisioning protocol related parameters
 *  @param pdi pointer of provision discovery info
 *  @return         enum wifidir_error
 */
static
enum wifidir_error wifidir_cfg_cmd_provisioning_params(struct wifidir_pd_info *pdi,
                                                       unsigned short dev_password)
{
    mrvl_cmd_head_buf *cmd;
    wifidir_params_config *wifidir_params = NULL;
    tlvbuf_wifidir_provisioning_params *tlv = NULL;
    int exit_code = WIFIDIR_ERR_SUCCESS;

    cmd = wifidir_cmdbuf_alloc(sizeof(wifidir_params_config) +
                               sizeof(tlvbuf_wifidir_provisioning_params),
                               HostCmd_CMD_WIFIDIR_PARAMS_CONFIG);
    if (!cmd)
        return WIFIDIR_ERR_NOMEM;

    wifidir_params = (wifidir_params_config *)cmd->cmd_data;
    wifidir_params->action = wlan_cpu_to_le16(ACTION_SET);

    tlv = (tlvbuf_wifidir_provisioning_params *)wifidir_params->wifidir_tlv;
    tlv->tag = wlan_cpu_to_le16(WIFIDIR_PROVISIONING_PARAMS_TLV_ID);
    tlv->length = sizeof(tlvbuf_wifidir_provisioning_params) - TLV_HEADER_SIZE;
    tlv->action = wlan_cpu_to_le16(1);
    tlv->config_methods = wlan_cpu_to_le16((u16) pdi->methods);
    tlv->dev_password = wlan_cpu_to_le16(dev_password);
    endian_convert_tlv_header_out(tlv);

    exit_code = wifidir_cmdbuf_send(cmd);
    free(cmd);
    return exit_code;
}

/**
 *  @brief Creates a wifidir_action_frame request and sends to the driver
 *
 *  NOTE: borrowed from wifidircmd_action_frame in wifidir_action.c.
 *
 *  @param mac: mac address of peer to whom action frame is being sent
 *  @param sub_type  Frame Sub type for generic action frame
 *  @return          WIFIDIR_ERR_SUCCESS or WIFIDIR_ERR_COM
 */
static enum wifidir_error send_wifidir_action_frame(unsigned char *mac, int sub_type)
{
    enum wifidir_error ret;
    mrvl_cmd_head_buf *cmd;
    wifidir_action_frame *action_buf = NULL;

    cmd = wifidir_cmdbuf_alloc(sizeof(wifidir_action_frame),
                               HostCmd_CMD_802_11_ACTION_FRAME);
    if (!cmd)
        return WIFIDIR_ERR_NOMEM;

    action_buf = (wifidir_action_frame *)cmd->cmd_data;
    action_buf->category = WIFI_CATEGORY_PUBLIC_ACTION_FRAME;
    memcpy(action_buf->peer_mac_addr, mac, ETH_ALEN);
    action_buf->action = 0;
    action_buf->dialog_taken = WIFI_DIALOG_TOKEN_IGNORE + 1;
    action_buf->oui[0] = 0x50;
    action_buf->oui[1] = 0x6F;
    action_buf->oui[2] = 0x9A;
    action_buf->oui_type = WIFIDIR_OUI_TYPE;
    action_buf->oui_sub_type = sub_type;

    ret = wifidir_cmdbuf_send(cmd);
    free(cmd);
    return ret;
}

static
enum wifidir_error wifidir_do_pd_req(struct wifidir_pd_info *pdi)
{
    int ret;

    if (!pdi ||
        !memcmp(pdi->device_address, "\x00\x00\x00\x00\x00\x00", ETH_ALEN))
        return WIFIDIR_ERR_INVAL;
    /* Currently we send none as dev_passwd */
    ret = wifidir_cfg_cmd_provisioning_params(pdi, 0xffff);
    if (ret != WIFIDIR_ERR_SUCCESS)
        return ret;

    INFO("==> pd_req device_address: " UTIL_MACSTR "\n", UTIL_MAC2STR(pdi->device_address));
    return send_wifidir_action_frame(pdi->device_address,
                                WIFIDIR_PROVISION_DISCOVERY_REQ_SUB_TYPE);
}

static
enum wifidir_error wifidir_handle_pd_req_event(apeventbuf_wifidir_generic *wifidir_event)
{
    struct event *event;
    struct wifidir_pd_info pdi;
    struct wifidir_peer *peer;
    int ret;

    if (!wifidir_event)
        return WIFIDIR_ERR_INVAL;

    /* make sure we put this peer in our table */
    ret = wifidir_handle_peer_detected(wifidir_event, &peer);
    if (ret == -1)
        WARN("Failed to parse complete peer from PD REQ event.\n");

    /* Apparently, the event_sub_type is overloaded here to mean the config
     * methods.  This does not appear to be documented anywhere.  It's
     * particularly ugly because event_sub_type is defined as a bitmask whose
     * fields are obviously meaningless in this case.
     */
    memcpy(&pdi.methods, &wifidir_event->event_sub_type, sizeof(pdi.methods));
    memcpy(pdi.device_address, wifidir_event->peer_mac_addr, ETH_ALEN);

    /* construct event and pass to event handler */
    event = (struct event *)malloc(sizeof(struct event) + sizeof(pdi));
    if (!event)
        return WIFIDIR_ERR_NOMEM;

    event->type = WIFIDIR_EVENT_PD_REQ;
    event->len  = sizeof(pdi);
    memcpy(event->val, &pdi, sizeof(pdi));
    MODULE_DO_CALLBACK(wifidir_mod, event);
    free(event);
    return WIFIDIR_ERR_SUCCESS;
}

static
enum wifidir_error wifidir_handle_pd_resp_event(apeventbuf_wifidir_generic *wifidir_event)
{
    struct event *event;
    struct wifidir_pd_info pdi;

    if (!wifidir_event)
        return WIFIDIR_ERR_INVAL;

    memcpy(&pdi.methods, &wifidir_event->event_sub_type, sizeof(pdi.methods));
    memcpy(pdi.device_address, wifidir_event->peer_mac_addr, ETH_ALEN);

    /* construct event and pass to event handler */
    event = (struct event *) malloc(sizeof(struct event) + sizeof(pdi));
    if (!event)
        return WIFIDIR_ERR_NOMEM;

    event->type = WIFIDIR_EVENT_PD_RESP;
    event->len  = sizeof(pdi);
    memcpy(event->val, &pdi, sizeof(pdi));
    MODULE_DO_CALLBACK(wifidir_mod, event);
    free(event);

    return WIFIDIR_ERR_SUCCESS;
}

/* parse the negotiation status from the event.  Minimally, the device_address and
 * the status in the result must be set.  If the status indicates successful GO
 * negotiation, the other members should be set correctly too.  Note that the
 * parsed_peer argument is the peer that was parsed from the incoming event and
 * the subtype is the subtype of the event.
 */
static
enum wifidir_error
wifidir_parse_neg_result(struct wifidir_peer *parsed_peer,
                      struct wifidir_neg_result *result,
                      u16 subtype)
{
    struct wifidir_peer *peer;

    if (!result || !parsed_peer)
        return WIFIDIR_ERR_INVAL;

    memcpy(result->device_address, parsed_peer->device_address, ETH_ALEN);

    if (subtype == EV_SUBTYPE_WIFIDIR_GO_NEG_FAILED)
        return WIFIDIR_ERR_COM;

    peer = wifidir_peer_lookup(parsed_peer->device_address);
    if (!peer) {
        ERR("Can't find peer in table.\n");
        return WIFIDIR_ERR_NOENT;
    }

    /* populate result */

    /* is_go */
    if (subtype == EV_SUBTYPE_WIFIDIR_GO_ROLE)
        result->is_go = 1;
    else if (subtype == EV_SUBTYPE_WIFIDIR_CLIENT_ROLE)
        result->is_go = 0;

    /* go_ssid */
    if (strlen(parsed_peer->go_ssid)) {
        memcpy(result->go_ssid, parsed_peer->go_ssid, MAX_SSID_LEN + 1);
        result->go_ssid[MAX_SSID_LEN] = 0; /* Make sure end with '\0' */
    }

    result->status = parsed_peer->go_neg_status;
    return WIFIDIR_ERR_SUCCESS;
}

/* After receiving a negotiation result, emit it to the user. */
static
enum wifidir_error
wifidir_emit_neg_result_event(struct wifidir_neg_result *result)
{
    struct event *event;
    int ret;

    if (!result) {
        ret = WIFIDIR_ERR_INVAL;
        goto fail;
    }

    /* construct event and pass to event handler */
    event = (struct event *)malloc(sizeof(struct event) +
                                   sizeof(struct wifidir_neg_result));
    if (!event) {
        ret = WIFIDIR_ERR_NOMEM;
        goto fail;
    }

    event->type = WIFIDIR_EVENT_NEGOTIATION_RESULT;
    event->len  = sizeof(struct wifidir_neg_result);
    memcpy(event->val, result, event->len);
    MODULE_DO_CALLBACK(wifidir_mod, event);
    free(event);

    return WIFIDIR_ERR_SUCCESS;

fail:
    ERR("Failed to send negotiation result to user.\n");
    return ret;
}

/* fetch the wifidirect configuration from the driver and use it to populate a
 * peer struct.  This code is borrowed from wfdutl.
 */
static enum wifidir_error wifidir_get_wifidir_cfg(struct wifidir_peer *peer)
{
    custom_ie *ie_ptr;
    u16 index = 0;
    tlvbuf_custom_ie *tlv;
    enum wifidir_error ret;
    u8 *buf;
    int i;

    buf = (u8 *)malloc(MRVDRV_SIZE_OF_CMD_BUFFER);
    if(!buf) {
        ERR("Failed to allocate buffer to fetch wifidirect config\n");
		return WIFIDIR_ERR_NOMEM;
    }
    memset(buf, 0, MRVDRV_SIZE_OF_CMD_BUFFER);
    ret = wifidir_ie_config(&index, 0, 0, buf);
    if (ret != WPS_STATUS_SUCCESS || index >= 3) {
        ERR("Failed to fetch wifidirect config\n");
        ret = WIFIDIR_ERR_COM;
        goto done;
    }
    tlv = (tlvbuf_custom_ie *)buf;
    if(tlv->tag != MRVL_MGMT_IE_LIST_TLV_ID) {
        ERR("Unexpected tag from driver: %d\n", tlv->tag);
        ret = WIFIDIR_ERR_COM;
        goto done;
    }
    ie_ptr = (custom_ie *)(tlv->ie_data);
    /* Goto appropriate Ie Index */
    for (i=0; i < index; i++) {
        ie_ptr = (custom_ie *)((u8 *)ie_ptr + sizeof(custom_ie) + ie_ptr->ie_length);
    }
    wifidir_peer_init(peer);
    ret = wifidir_parse_wifidir_tlvs(ie_ptr->ie_buffer, ie_ptr->ie_length, peer);

done:
    free(buf);
    return ret;
}

static
enum wifidir_error wifidir_start_as_go(void)
{

    gpwps_info->discovery_role = WPS_REGISTRAR;
#if 0
    /* Update the device password ID here required for WPS provisioning,
     * as specified in table 1 of the P2P technical specification draft
     * v1.0.16(clean) page 38 */
    if (method & MODULE_CONFIG_METHOD_DISPLAY) {
        gpwps_info->enrollee.updated_device_password_id = DEVICE_PASSWORD_ID_PIN;
        gpwps_info->registrar.updated_device_password_id = DEVICE_PASSWORD_ID_PIN;
    } else if (method & MODULE_CONFIG_METHOD_KEYPAD) {
        gpwps_info->enrollee.updated_device_password_id = DEVICE_PASSWORD_REG_SPECIFIED;
        gpwps_info->registrar.updated_device_password_id = DEVICE_PASSWORD_REG_SPECIFIED;
    } else if (method & MODULE_CONFIG_METHOD_PBC) {
        gpwps_info->enrollee.updated_device_password_id = DEVICE_PASSWORD_PUSH_BUTTON;
        gpwps_info->registrar.updated_device_password_id = DEVICE_PASSWORD_PUSH_BUTTON;
    } else {
        ERR("Invalid config method. Should be one of DISPLAY, KEYPAD or PBC");
        return WIFIDIR_ERR_INVAL;
    }
#endif

    /* Set intended address before starting GO
     */
    if (memcmp(gpwps_info->wifidir_intended_addr, "\x00\x00\x00\x00\x00\x00",
            ETH_ALEN)) {
        if (mwu_set_intended_mac_addr(wifidir_mod->iface,
            gpwps_info->wifidir_intended_addr))
            ERR("Failed to set mac address.\n");
        else
            INFO("Intended address is set.\n");

        memcpy(gpwps_info->registrar.mac_address,
            gpwps_info->wifidir_intended_addr, ETH_ALEN);
    }

    if (wifidir_set_mode(WIFIDIR_MODE_START_GROUP_OWNER) != WIFIDIR_ERR_SUCCESS) {
        ERR("Failed to start GO.\n");
        return WIFIDIR_ERR_COM;
    }

    return WIFIDIR_ERR_SUCCESS;
}

static
enum wifidir_error wifidir_start_as_enrollee(void)
{

    gpwps_info->discovery_role = WPS_ENROLLEE;
#if 0
    /* Update the device password ID here required for WPS provisioning,
     * as specified in table 1 of the P2P technical specification draft
     * v1.0.16(clean) page 38 */
    if (method & MODULE_CONFIG_METHOD_DISPLAY) {
        gpwps_info->enrollee.updated_device_password_id = DEVICE_PASSWORD_REG_SPECIFIED;
        gpwps_info->registrar.updated_device_password_id = DEVICE_PASSWORD_REG_SPECIFIED;
    } else if (method & MODULE_CONFIG_METHOD_KEYPAD) {
        gpwps_info->enrollee.updated_device_password_id = DEVICE_PASSWORD_ID_PIN;
        gpwps_info->registrar.updated_device_password_id = DEVICE_PASSWORD_ID_PIN;
    } else if (method & MODULE_CONFIG_METHOD_PBC) {
        gpwps_info->enrollee.updated_device_password_id = DEVICE_PASSWORD_PUSH_BUTTON;
        gpwps_info->registrar.updated_device_password_id = DEVICE_PASSWORD_PUSH_BUTTON;
    } else {
        ERR("Invalid config method. Should be one of DISPLAY, KEYPAD or PBC");
        return WIFIDIR_ERR_INVAL;
    }
#endif

    if (memcmp(gpwps_info->wifidir_intended_addr,
            "\x00\x00\x00\x00\x00\x00", ETH_ALEN)) {
        if (mwu_set_intended_mac_addr(wifidir_mod->iface,
            gpwps_info->wifidir_intended_addr))
            ERR("Failed to set mac address.\n");

        memcpy(gpwps_info->enrollee.mac_address,
            gpwps_info->wifidir_intended_addr, ETH_ALEN);
    }

    if (wifidir_set_mode(WIFIDIR_MODE_START_DEVICE) != WIFIDIR_ERR_SUCCESS) {
        ERR("Failed to start device (client) mode.\n");
        return WIFIDIR_ERR_COM;
    }

    return WIFIDIR_ERR_SUCCESS;
}

static enum wifidir_error wifidir_get_op_channel(int *channel)
{
    enum wifidir_error ret = WIFIDIR_ERR_SUCCESS;
    mrvl_cmd_head_buf *cmd;
    wifidir_params_config *wifidir_params;
    tlvbuf_wifidir_operating_channel *tlv;

    cmd = wifidir_cmdbuf_alloc(sizeof(wifidir_params_config) +
                               sizeof(tlvbuf_wifidir_operating_channel),
                               HostCmd_CMD_WIFIDIR_PARAMS_CONFIG);
    if (!cmd)
        return WIFIDIR_ERR_NOMEM;

    wifidir_params = (wifidir_params_config *)cmd->cmd_data;
    wifidir_params->action = wlan_cpu_to_le16(ACTION_GET);

    tlv = (tlvbuf_wifidir_operating_channel *)wifidir_params->wifidir_tlv;
    tlv->tag = MRVL_WIFIDIR_OPERATING_CHANNEL_TLV_ID;
    tlv->length = sizeof(tlvbuf_wifidir_operating_channel) - TLV_HEADER_SIZE;

    endian_convert_tlv_header_out(tlv);

    ret = wifidir_cmdbuf_send(cmd);
    if (ret != WIFIDIR_ERR_SUCCESS)
        goto done;

    endian_convert_tlv_header_in(tlv);

    *channel = (int) tlv->operating_channel;

done:
    free(cmd);
    return ret;
}

/* a negotiation result has been received and now must be applied.  Prepare an
 * event for the user.  A successful return means that we are ready to advance
 * to our next steady state (either CLIENT or GO).  An error means that there
 * was some trouble getting into our steady state.  In all cases, a result is
 * sent to the user.
 */
static enum wifidir_error wifidir_handle_neg_result(struct wifidir_neg_result *result)
{
    enum wifidir_error ret = WIFIDIR_ERR_SUCCESS;
    struct wifidir_peer *peer, cfg;
    int channel;

    peer = wifidir_peer_lookup(result->device_address);
    if (peer == NULL) {
        ERR("Unexpected: peer not in table!\n");
        result->status = WIFIDIR_GO_NEG_INTERNAL_ERR;
        goto done;
    }

    if (result->status != WIFIDIR_GO_NEG_SUCCESS) {
        goto done;
    }

    if (result->is_go) {
        /* If we get here, GO negotiation is complete and we MUST emit an
         * event to the user, either indicating success or failure.
         */
        INFO("We won the GO negotiation.\n");

        /* Update Group formation bit when starting as registrar */
        if (wps_wlan_update_group_formation_config(WPS_SET) != WPS_STATUS_SUCCESS) {
            ERR("Failed to update Group formation bit.  Going IDLE.\n");
            result->status = WIFIDIR_GO_NEG_INTERNAL_ERR;
            goto done;
        }

        ret = wifidir_start_as_go();
        if (ret != WIFIDIR_ERR_SUCCESS) {
            result->status = WIFIDIR_GO_NEG_INTERNAL_ERR;
            goto done;
        }
        ret = wifidir_get_wifidir_cfg(&cfg);
        if (ret != WIFIDIR_ERR_SUCCESS) {
            result->status = WIFIDIR_GO_NEG_INTERNAL_ERR;
            goto done;
        }
        strncpy(result->go_ssid, cfg.go_ssid, MAX_SSID_LEN +1);

    } else {

        INFO("We lost the GO negotiation.  Becoming client instead.\n");

        ret = wifidir_start_as_enrollee();
        if (ret != WIFIDIR_ERR_SUCCESS)
            result->status = WIFIDIR_GO_NEG_INTERNAL_ERR;
    }

done:
    wifidir_emit_neg_result_event(result);
    return ret;
}

/* return:
 * 1 -- GO is up; 0 -- GO is not up; -1 -- error
 */
static int
wifidir_is_go_up(unsigned char *go_interface_address, unsigned char* go_ssid)
{
    int op_channel = 0, scan_channel_list = 0;
    static int retry = 1;
    WPS_DATA *wps_s = (WPS_DATA *) &gpwps_info->wps_data;
    WIFIDIR_DATA *pwifidir_data = &(gpwps_info->wps_data.wifidir_data);

    ENTER();
    if (!go_interface_address ||
        !memcmp(go_interface_address, "\x00\x00\x00\x00\x00\x00", ETH_ALEN)) {
        ERR("Invalid interface address.\n");
        LEAVE();
        return -1;
    }
    if (!go_ssid || !strlen(go_ssid)) {
        ERR("Invalid go_ssid.\n");
        LEAVE();
        return -1;
    }
    if(pwifidir_data->current_peer == NULL) {
        ERR("Current Peer is not updated yet!!\n");
        LEAVE();
        return -1;
    }

    memset(gpwps_info->registrar.mac_address, 0, ETH_ALEN);
    memcpy(gpwps_info->registrar.mac_address, go_interface_address,
           ETH_ALEN);
    memset(gpwps_info->registrar.go_ssid, 0, MAX_SSID_LEN + 1);
    memcpy(gpwps_info->registrar.go_ssid, go_ssid, MAX_SSID_LEN + 1);

    op_channel = pwifidir_data->current_peer->op_channel;

    if(pwifidir_data->current_peer->peer_channel_list.num_of_chan &&
            pwifidir_data->self_channel_list.num_of_chan)
        scan_channel_list = 1;

    if(op_channel && (retry % 3 == 1)) {
        /* Scan op channel */
        wps_wlan_set_user_scan(pwifidir_data, go_ssid,
                wifidir_mod->iface, SCAN_OP_CHANNEL);
        op_channel = 0;
    } else if(scan_channel_list && (retry % 3 == 2)) {
        /* Scan common channel list*/
        wps_wlan_set_user_scan(pwifidir_data, go_ssid,
                wifidir_mod->iface, SCAN_COMMON_CHANNEL_LIST);
        scan_channel_list = 0;
    } else {
        /* Perform generic scan */
        wps_wlan_set_user_scan(pwifidir_data, go_ssid,
                wifidir_mod->iface, SCAN_GENERIC);
    }

    /* reset dev_found */
    wps_s->wifidir_data.dev_found = -1;
    wps_wlan_scan_results(wps_s, FILTER_NONE);

    if (wps_s->wifidir_data.dev_found != -1) {
        retry = 1;
        INFO("GO is up.\n");
        LEAVE();
        return 1;
    }
    retry++;
    INFO("GO is not found.\n");
    LEAVE();
    return 0;
}

/* The user instructed us to negotiate with a particular peer.  If that peer is
 * already a GO, we just become the client.  Otherwise, we initiate GO
 * negotiation.  Note that this function changes the state.  Normally, this
 * would be done by the state machine.  But we save a bit of code here because
 * both the FIND and IDLE states need this code.  As a convenience to the user,
 * *peer is set to the peer with whom we started negotiation.
 */
static
enum wifidir_error wifidir_do_negotiate_group(struct wifidir_pd_info *pdi,
                                           struct wifidir_peer **peer)
{
    enum wifidir_error ret = WIFIDIR_ERR_SUCCESS;
    unsigned short dev_passwd = 0xffff; /* indicate none */

    if (!pdi)
        return WIFIDIR_ERR_INVAL;

    *peer = wifidir_peer_lookup(pdi->device_address);
    if (!*peer) {
        ERR("Can't negotiate with non-existent peer.\n");
        return WIFIDIR_ERR_NOENT;
    }

    /* There should be only one method in methods element */
    if (pdi->methods & MODULE_CONFIG_METHOD_DISPLAY)
        dev_passwd = DEVICE_PASSWORD_USER_SPECIFIED;
    else if (pdi->methods & MODULE_CONFIG_METHOD_KEYPAD)
        dev_passwd = DEVICE_PASSWORD_REG_SPECIFIED;
    else if (pdi->methods & MODULE_CONFIG_METHOD_PBC)
        dev_passwd = DEVICE_PASSWORD_PUSH_BUTTON;
    else {
        ERR("Invalid config method. Should be one of DISPLAY, KEYPAD or PBC");
        goto fail;
    }

    ret = wifidir_cfg_cmd_provisioning_params(pdi, dev_passwd);
    if (ret != WIFIDIR_ERR_SUCCESS) {
        ERR("Failed to set provisioning params");
        goto fail;
    }

    ret = wifidir_cfg_cmd_set_wps_params(pdi->methods);
    if (ret != WIFIDIR_ERR_SUCCESS) {
        ERR("Failed to set wps params.\n");
        goto fail;
    }

    if ((*peer)->group_capability & WIFIDIR_GROUP_OWNER_MASK) {

        /* If the peer is already GO, no need to do negotiation.
         * Directly start as client.
         */

        /* TODO: We probably arrived here from an API call.  Should we emit a
         * negotiation result to the user? Fail?
         */
        ret = wifidir_start_as_enrollee();
        if (ret == WIFIDIR_ERR_SUCCESS) {
            change_state(WIFIDIR_STATE_CLIENT);
        } else {
            change_state(WIFIDIR_STATE_FIND);
            goto fail;
        }
    } else {
        ret = send_wifidir_action_frame(pdi->device_address,
                                   WIFIDIR_GO_NEG_REQ_ACTION_SUB_TYPE);
        if (ret == WIFIDIR_ERR_SUCCESS) {
            change_state(WIFIDIR_STATE_GO_REQUEST_SENT);
        } else {
            ERR("Failed to send GO Negotiation Request action frame.\n");
            change_state(WIFIDIR_STATE_FIND);
            goto fail;
        }
    }

    return ret;

fail:
    wifidir_cfg_cmd_set_wps_params(MODULE_CONFIG_METHOD_NONE);
    return ret;
}

/* Just print some data about unexpected events for debugging purposes.
 */
#define UNEXPECTED_EVENT(es, e) do {                \
        if (es == WIFIDIR_EVENTS_DRIVER &&          \
            e == EV_TYPE_WIFIDIR_PEER_DETECTED)         \
            break;                                  \
        WARN("Unexpected event %d:%d\n", es, e);    \
    } while (0)

/* deinit is unique among UI commands because it can be invoked from any state,
 * and it always results in transitioning to the INIT state.  So we can have a
 * common handler for this particular event that alters our state.  This saves
 * a bit of copy-paste.  return 1 (true) if we handled the event.  Otherwise,
 * return false (0) so that some other handler can be invoked.  This is a
 * convenience function for handing the very common deinit event.
 */
static int handle_deinit_event(int es, struct event *cmd)
{
    if (es == WIFIDIR_EVENTS_UI && cmd->type == WIFIDIR_UI_CMD_DEINIT) {
        wifidir_do_deinit();
        change_state(WIFIDIR_STATE_INIT);
        return 1;
    }
    return 0;
}

static void wifidir_emit_peer_event(struct wifidir_peer *peer,
                                    enum wifidir_event_type event)
{
    struct event *e = malloc(sizeof(struct event) +
                             sizeof(struct wifidir_peer));
    e->type = event;
    e->len = sizeof(struct wifidir_peer);
    memcpy(e->val, peer, sizeof(struct wifidir_peer));
    MODULE_DO_CALLBACK(wifidir_mod, e);
    free(e);
}

/* allow the peer with the device_address in the specified provisioning info to
 * engage in GO negotiation with us.  As a convenience to the caller, set *peer
 * to the peer that we end up allowing.
 */
static enum wifidir_error wifidir_do_allow(struct wifidir_pd_info *pdi,
                                           struct wifidir_peer **peer)
{
    enum wifidir_error ret =  WIFIDIR_ERR_SUCCESS;

    ret = wifidir_cfg_cmd_set_wps_params(pdi->methods);
    if (ret != WIFIDIR_ERR_SUCCESS) {
        ERR("Failed to set wps params\n");
        return ret;
    }

    if (memcmp(pdi->device_address, "\xff\xff\xff\xff\xff\xff", ETH_ALEN) == 0) {
        /*
         * This special device_address means we want to allow any device. So we
         * directly return here to avoid following device_address checking.
         */
        return ret;
    }

    *peer = wifidir_peer_lookup(pdi->device_address);
    if (*peer == NULL) {
        /* The user has allowed a specific peer, but we have not yet
         * received the expected GO negotiation request.  So just remember
         * the peer that we're dealing with and expect a GO negotiation
         * request.  Typically (but not necessarily), we will have received
         * a PD REQ from this peer and he will be in our table.  But he may
         * not be.  Add him in this case.
         */
        *peer = wifidir_peer_alloc();
        if (*peer == NULL) {
            ERR("Failed to alloc allow()'d peer\n");
            ret = WIFIDIR_ERR_NOMEM;
            goto fail;
        }
        memcpy((*peer)->device_address, pdi->device_address, ETH_ALEN);
        ret = wifidir_peer_insert(*peer);
        if (ret != 0) {
            ERR("Failed to add allow()'d peer to list.\n");
            ret = WIFIDIR_ERR_INVAL;
            goto fail;
        }
    }

    wps_cancel_timer(wifidir_go_timeout, NULL);
    ret = wps_start_timer(WIFIDIR_GO_NEG_TIMEOUT, 0, wifidir_go_timeout, NULL);
    if (ret != WPS_STATUS_SUCCESS) {
        ERR("Failed to set GO timeout.\n");
        ret = WIFIDIR_ERR_COM;
        goto fail;
    }

    return WIFIDIR_ERR_SUCCESS;

fail:
    if (*peer != NULL) {
        free(*peer);
    }
    *peer = NULL;
    wifidir_cfg_cmd_set_wps_params(MODULE_CONFIG_METHOD_NONE);
    return ret;
}

/* the state machine got the GO timeout event.  Alert the user. */
void do_go_error(u8 *mac_addr, enum wifidir_go_neg_status status)
{
    struct wifidir_neg_result neg_result;

    memcpy(neg_result.device_address, mac_addr, ETH_ALEN);
    neg_result.status = status;
    wifidir_emit_neg_result_event(&neg_result);
}

/* In some cases, we need to know the peer that we are currently working with
 * in order to ensure that incoming events pertain to the peer we are working
 * with.  This variably should NOT be used by anybody but the state machine!
 * Ever!
 */
static struct wifidir_peer *current_peer;

int wifidir_state_machine(u8 *buffer, u16 size, int es)
{
    int ret = WIFIDIR_ERR_SUCCESS;
    apeventbuf_wifidir_generic *wifidir_event = NULL;
    int e = 0, sub = 0;
    struct event *cmd = NULL;
    static int wait_go_count = 0;
    struct wifidir_peer *peer = NULL;
    int i;
    static struct wifidir_neg_result neg_result;
    struct wifidir_peer temp_peer;
    unsigned short dev_passwd = 0xffff;
    WIFIDIR_DATA *pwifidir_data = &(gpwps_info->wps_data.wifidir_data);

    ENTER();

    /* determine the nature of the event */
    if (es == WIFIDIR_EVENTS_DRIVER) {
        wifidir_event = (apeventbuf_wifidir_generic *)buffer;
        e = wlan_le16_to_cpu(wifidir_event->event_type);
        sub = wlan_le16_to_cpu(wifidir_event->event_sub_type.device_role);
        /* All driver events contain wifidirect and WPS IEs.  Store their data
         * in a peer data structure.
         */
        if (wifidir_peer_parse(wifidir_event, &temp_peer) != 0) {
            WARN("Failed to parse driver event.  Proceeding anyway.\n");
        } else if (e != EV_TYPE_WIFIDIR_PEER_DETECTED) {
            /* peer detected events come too often.  So don't print them. */
            INFO("Got event %s with subtype %d in state %s with peer data:\n",
                 wifidir_generic_event_to_str(e), sub,
                 wifidir_state_to_str(__state));
            wifidir_peer_print(&temp_peer);
        }

        /* Update the current_peer */
        if(current_peer != NULL) {

            if(memcmp(current_peer->device_address, temp_peer.device_address,
                        ETH_ALEN) == 0 &&
                    memcmp(current_peer->interface_address, temp_peer.interface_address,
                        ETH_ALEN) == 0) {
                /* Update the current peer pointer in global structure */
                pwifidir_data->current_peer = current_peer;

                /*Update the operating channel value in Current Peer */
                if((temp_peer.op_channel != 0) &&
                        current_peer->op_channel != temp_peer.op_channel) {
                    current_peer->op_channel = temp_peer.op_channel;
                }

                /*Update the channel list in Current Peer */
                if((temp_peer.peer_channel_list.num_of_chan != 0) &&
                    memcmp(&current_peer->peer_channel_list, &temp_peer.peer_channel_list,
                        sizeof(temp_peer.peer_channel_list) != 0)) {
                    memcpy(&current_peer->peer_channel_list,
                            &temp_peer.peer_channel_list, sizeof(current_peer->peer_channel_list));
                }
            }
        }
    } else if (es == WIFIDIR_EVENTS_UI) {
        cmd = (struct event *)buffer;
        e = cmd->type;
        INFO("Got UI event %s in state %s\n", wifidir_ui_cmd_to_str(e),
             wifidir_state_to_str(__state));
    } else {
        ERR("Unexpected event space: %d\n", es);
        return WIFIDIR_ERR_INVAL;
    }

    /* handle events that don't affect our state, and fail early when
     * possible
     */
    if (es == WIFIDIR_EVENTS_UI && cmd->type == WIFIDIR_UI_CMD_PDREQ) {
        if (__state == WIFIDIR_STATE_FIND ||
            __state == WIFIDIR_STATE_WAIT_FOR_ALLOW) {
            return wifidir_do_pd_req((struct wifidir_pd_info *)cmd->val);
        } else {
            ERR("Can't send PD request while not in find state or wait_for_allow state.\n");
            return WIFIDIR_ERR_BUSY;
        }
    } else if (es == WIFIDIR_EVENTS_DRIVER &&
        e == EV_TYPE_WIFIDIR_PROVISION_DISCOVERY_REQ) {
        return wifidir_handle_pd_req_event(wifidir_event);
    } else if (es == WIFIDIR_EVENTS_DRIVER &&
        e == EV_TYPE_WIFIDIR_PROVISION_DISCOVERY_RESP) {
        return wifidir_handle_pd_resp_event(wifidir_event);

    } else if (es == WIFIDIR_EVENTS_UI && cmd->type == WIFIDIR_UI_CMD_NEGOTIATE_GROUP) {
        if (__state != WIFIDIR_STATE_FIND && __state != WIFIDIR_STATE_IDLE &&
            __state != WIFIDIR_STATE_EXPECT_GO_REQUEST &&
            __state != WIFIDIR_STATE_WAIT_FOR_ALLOW) {
            INFO("Can't start new negotiation.  Busy.\n");
            return WIFIDIR_ERR_BUSY;
        }
    } else if (es == WIFIDIR_EVENTS_UI && cmd->type == WIFIDIR_UI_CMD_INIT) {
        if (__state != WIFIDIR_STATE_INIT) {
            INFO("Already initialized. To reinit, please deinit first.\n");
            return WIFIDIR_ERR_BUSY;
        }
    } else if (es == WIFIDIR_EVENTS_UI && cmd->type == WIFIDIR_UI_CMD_STOP_FIND) {
        if (__state != WIFIDIR_STATE_FIND) {
            INFO("Find phase not started.. Ignoring stop_find.. \n");
            return WIFIDIR_ERR_INVAL;
        }
    } else if (es == WIFIDIR_EVENTS_UI && cmd->type == WIFIDIR_UI_CMD_ALLOW) {
        if (__state != WIFIDIR_STATE_FIND && __state != WIFIDIR_STATE_IDLE &&
            __state != WIFIDIR_STATE_WAIT_FOR_ALLOW && __state != WIFIDIR_STATE_GO) {
            INFO("A peer has already been allowed\n");
            return WIFIDIR_ERR_INVAL;
        }
    }

    /* now for the actual state machine */
    switch (__state) {
    case WIFIDIR_STATE_INIT:
        if (es == WIFIDIR_EVENTS_UI && cmd->type == WIFIDIR_UI_CMD_INIT) {
            ret = wifidir_do_init((struct wifidir_cfg *)cmd->val);
            if (ret != WIFIDIR_ERR_SUCCESS) {
                ERR("wifidir_do_init error: %d\n", ret);
                return ret;
            }
            change_state(WIFIDIR_STATE_IDLE);
            break;
        }
        if (handle_deinit_event(es, cmd))
            break;

        break;

    case WIFIDIR_STATE_IDLE:
        if (es == WIFIDIR_EVENTS_UI &&
                    cmd->type == WIFIDIR_UI_CMD_NEGOTIATE_GROUP) {
            ret = wifidir_do_negotiate_group((struct wifidir_pd_info *)cmd->val,
                                          &current_peer);
            break;

        } else if (handle_deinit_event(es, cmd)) {
            break;
        } else if (es == WIFIDIR_EVENTS_UI && cmd->type == WIFIDIR_UI_CMD_START_FIND) {
            /* set up background scanning */
            mwu_mlanconfig_bgscan(wifidir_mod->iface);
            ret = wifidir_do_start_find_phase();
            change_state(WIFIDIR_STATE_FIND);
            break;

        } else if (es == WIFIDIR_EVENTS_UI && cmd->type == WIFIDIR_UI_CMD_START_GROUP) {
            ret = wifidir_start_as_go();
            wps_cancel_timer(wifidir_go_timeout, NULL);
            change_state(WIFIDIR_STATE_GO);
            /* Don't evict expired peer in case of auto GO
             * In auto GO we are not in find phase and may not get the peer
             * detected event very frequently. So, we might end up in evicting
             * an peer which is just about to connect but haven't responded to
             * our probe req in a while.*/
            wps_cancel_timer(wifidir_evict_expired_peer, &pto);
            break;

        } else if (es == WIFIDIR_EVENTS_DRIVER &&
                   e == EV_TYPE_WIFIDIR_NEGOTIATION_REQUEST) {
            if (sub == EV_SUBTYPE_WIFIDIR_GO_NEG_FAILED) {
                ERR("Ignoring negotiation request with FAILED subtype.\n");
                break;
            }

            ret = wifidir_handle_peer_detected(wifidir_event, &current_peer);
            if (ret == -1) {
                ERR("Failed to parse complete peer from NEG REQ event.\n");
                break;
            }
            change_state(WIFIDIR_STATE_WAIT_FOR_GO_RESULT);

        } else if (es == WIFIDIR_EVENTS_UI && cmd->type == WIFIDIR_UI_CMD_ALLOW) {
            /* The user has allowed a specific peer, but we have not yet
             * received the expected GO negotiation request.  So just remember
             * the peer that we're dealing with and expect a GO negotiation
             * request.  Typically (but not necessarily), we will have received
             * a PD REQ from this peer and he will be in our table.  But he may
             * not be.  Add him in this case.
             */
            struct wifidir_pd_info *pdi = (struct wifidir_pd_info *)cmd->val;
            peer = wifidir_peer_lookup(pdi->device_address);
            if (peer == NULL) {
                peer = wifidir_peer_alloc();
                if (peer == NULL) {
                    ERR("Failed to alloc allow()'d peer\n");
                    ret = WIFIDIR_ERR_NOMEM;
                    break;
                }
                memcpy(peer->device_address, pdi->device_address, ETH_ALEN);
                ret = wifidir_peer_insert(peer);
                if (ret != 0) {
                    ERR("Failed to add allow()'d peer to list.\n");
                    ret = WIFIDIR_ERR_INVAL;
                    break;
                }
            }
            ret = wifidir_do_allow(pdi, &current_peer);
            if (ret == WIFIDIR_ERR_SUCCESS)
                change_state(WIFIDIR_STATE_EXPECT_GO_REQUEST);
            break;

        } else if (es == WIFIDIR_EVENTS_DRIVER && e == EV_TYPE_WIFIDIR_PEER_DETECTED) {
            ret = wifidir_handle_peer_detected(wifidir_event, &peer);
            if (ret == 1 || ret == 2) {
                wifidir_emit_peer_event(peer,
                                        ret == 1 ? WIFIDIR_EVENT_PEER_FOUND : \
                                        WIFIDIR_EVENT_PEER_UPDATED);
                INFO("Found new/updated peer in IDLE state: \n");
                wifidir_peer_print(peer);
            }
            break;
        }
        break;

    case WIFIDIR_STATE_FIND:
        /* While we're in the FIND state, we expect peers to appear and
         * disappear, and we alert the user of these events.  When we leave the
         * FIND state, we disable the peer exipriation timer.  This prevents us
         * from evicting a peer that we are working with.
         */
        if (es == WIFIDIR_EVENTS_DRIVER && e == EV_TYPE_WIFIDIR_PEER_DETECTED) {
            ret = wifidir_handle_peer_detected(wifidir_event, &peer);
            if (ret == -1 || ret == 0)
                break;
            if (ret == 1 || ret == 2) {
                /* This is a new or updated peer.  Alert the user */
                wifidir_emit_peer_event(peer,
                                        ret == 1 ? WIFIDIR_EVENT_PEER_FOUND : \
                                        WIFIDIR_EVENT_PEER_UPDATED);
                INFO("Found new/updated peer: \n");
                wifidir_peer_print(peer);
            }
            break;

        } else if (es == WIFIDIR_EVENTS_DRIVER &&
                   e == EV_TYPE_WIFIDIR_NEGOTIATION_REQUEST) {
            if (sub == EV_SUBTYPE_WIFIDIR_GO_NEG_FAILED) {
                ERR("Ignoring negotiation request with FAILED status.\n");
                break;
            }

            current_peer = wifidir_peer_lookup(wifidir_event->peer_mac_addr);
            if (current_peer == NULL) {
                ERR("Ignoring GO negotiation request from unknown peer.\n");
                break;
            }
            /* See IDLE state comments on why we need to copy this now */
            memcpy(current_peer->interface_address, temp_peer.interface_address,
                   ETH_ALEN);
            current_peer->group_cfg_timeout = temp_peer.group_cfg_timeout;
            wifidir_emit_peer_event(current_peer, WIFIDIR_EVENT_PEER_UPDATED);
            wps_cancel_timer(wifidir_evict_expired_peer, &pto);
            change_state(WIFIDIR_STATE_WAIT_FOR_GO_RESULT);

        } else if (es == WIFIDIR_EVENTS_UI &&
                    cmd->type == WIFIDIR_UI_CMD_NEGOTIATE_GROUP) {
            wps_cancel_timer(wifidir_evict_expired_peer, &pto);
            ret = wifidir_do_negotiate_group((struct wifidir_pd_info *)cmd->val,
                                          &current_peer);
            break;

        } else if (handle_deinit_event(es, cmd)) {
            wps_cancel_timer(wifidir_evict_expired_peer, &pto);
            break;
        } else if (es == WIFIDIR_EVENTS_UI && cmd->type == WIFIDIR_UI_CMD_START_FIND) {
            /* As bgscan is already started, only set mode to find_phase here
             */
            ret = wifidir_do_start_find_phase();
            break;
        } else if (es == WIFIDIR_EVENTS_UI && cmd->type == WIFIDIR_UI_CMD_STOP_FIND) {
            wps_cancel_timer(wifidir_evict_expired_peer, &pto);
            wifidir_do_stop_find_phase();
            break;

        } else if (es == WIFIDIR_EVENTS_UI && cmd->type == WIFIDIR_UI_CMD_ALLOW) {
            ret = wifidir_do_allow((struct wifidir_pd_info *)cmd->val,
                                   &current_peer);
            if (ret == WIFIDIR_ERR_SUCCESS)
                change_state(WIFIDIR_STATE_EXPECT_GO_REQUEST);
            wps_cancel_timer(wifidir_evict_expired_peer, &pto);
        }

        break;

    case WIFIDIR_STATE_GO_REQUEST_SENT:
        if (handle_deinit_event(es, cmd))
            break;

        if (es == WIFIDIR_EVENTS_DRIVER && e == EV_TYPE_WIFIDIR_NEGOTIATION_RESULT) {
            /* we do not expect the GO negotiation result in this state.  But
             * sometimes we get it if there was a negotiation failure
             */

            /* If this failure is not from connecting peer, just ignore it.
             */
            if (memcmp(current_peer->device_address, wifidir_event->peer_mac_addr, ETH_ALEN))
                break;

            ret = wifidir_parse_neg_result(&temp_peer, &neg_result, sub);
            if (ret != WIFIDIR_ERR_SUCCESS) {
                neg_result.status = temp_peer.go_neg_status;
                if (neg_result.status == WIFIDIR_GO_NEG_INTERNAL_ERR) {
                    WARN("We got time out NEG RESULT. Now just wait for peer to send NEG REQUEST.\n");
                    wps_cancel_timer(wifidir_go_timeout, NULL);
                    wps_start_timer(WIFIDIR_GO_NEG_TIMEOUT, 0, wifidir_go_timeout,
                                    NULL);
                    change_state(WIFIDIR_STATE_EXPECT_GO_REQUEST);
                    break;
                } else {
                    wifidir_emit_neg_result_event(&neg_result);
                }
            } else {
                WARN("Unexpected NEG RESULT with subtype %d and status %d\n",
                     sub, temp_peer.go_neg_status);
            }
            change_state(WIFIDIR_STATE_FIND);
            break;
        }

        if (es != WIFIDIR_EVENTS_DRIVER || e != EV_TYPE_WIFIDIR_NEGOTIATION_RESPONSE) {
            UNEXPECTED_EVENT(es, e);
            break;
        }

        if (memcmp(wifidir_event->peer_mac_addr, current_peer->device_address,
                   ETH_ALEN)) {
            ERR("Ignoring GO Negotiation Response from unknown peer.\n");
            break;
        }

        /* We are the initiator, and we expect a valid status in the RESPONSE
         * event from the driver, but not the RESULT event.  So if we don't get
         * a SUCCESS status now, we're done.  Otherwise, we wait for the result
         * event to send the status to the user.
         */
        ret = wifidir_parse_neg_result(&temp_peer, &neg_result, sub);
        if (ret != WIFIDIR_ERR_SUCCESS) {
            neg_result.status = temp_peer.go_neg_status;
            if (neg_result.status == WIFIDIR_GO_NEG_NA) {
                /* The recipient of our GO negotiation request apparently has not
                 * gathered all of the necessary provisioning info.  The P2P spec
                 * says we should expect him to gather all of that stuff within 120
                 * seconds and then re-initiate GO negotiation.  So we set the
                 * timer and wait.
                 */
                wps_cancel_timer(wifidir_go_timeout, NULL);
                wps_start_timer(WIFIDIR_GO_NEG_TIMEOUT, 0, wifidir_go_timeout,
                                NULL);
                change_state(WIFIDIR_STATE_EXPECT_GO_REQUEST);
                break;
            }
            wifidir_emit_neg_result_event(&neg_result);
            change_state(WIFIDIR_STATE_FIND);
            break;
        }

        /* See IDLE state comments on why we need to copy this now */
        memcpy(current_peer->interface_address, temp_peer.interface_address,
               ETH_ALEN);
        current_peer->group_cfg_timeout = temp_peer.group_cfg_timeout;
        wifidir_emit_peer_event(current_peer, WIFIDIR_EVENT_PEER_UPDATED);
        change_state(WIFIDIR_STATE_WAIT_FOR_INITIATOR_GO_RESULT);
        break;

    case WIFIDIR_STATE_WAIT_FOR_GO_RESULT:

        if (handle_deinit_event(es, cmd))
            break;

        if (es == WIFIDIR_EVENTS_DRIVER &&
            e == EV_TYPE_WIFIDIR_NEGOTIATION_RESPONSE_TX) {
            if (sub == WIFIDIR_GO_NEG_NA) {
                /* We sent a GO negotiation response with the "Info Currently
                 * Unavailable" status.  This means that we are expected to
                 * re-initiate GO negotiation within the next 120s.  But our
                 * user has not called allow() yet indicating 1) that we are
                 * allowed to engage with this peer, and 2) that we have
                 * collected all of the necessary provisioning info (e.g., WPS
                 * PIN).  So we wait for the allow().  If we get it within
                 * 120s, great.  Otherwise, we give up.
                 */
                ret = wifidir_parse_neg_result(&temp_peer, &neg_result, sub);
                if (ret != WIFIDIR_ERR_SUCCESS) {
                    neg_result.status = temp_peer.go_neg_status;
                }
                INFO("temp_peer.go_neg_status = %d\n", neg_result.status);
                /*
                 * Since in this event there is no P2P TLV, the go_neg_status
                 * after parsed is still the default value -1 (indicating
                 * internal error status), which isn't expected. So, to
                 * workaround, we shall manually assign sub (or
                 * WIFIDIR_GO_NEG_NA) to neg_result.status.
                 */
                neg_result.status = sub;
                wifidir_emit_neg_result_event(&neg_result);
                wps_cancel_timer(wifidir_go_timeout, NULL);
                wps_start_timer(WIFIDIR_GO_NEG_TIMEOUT, 0, wifidir_go_timeout,
                                NULL);
                change_state(WIFIDIR_STATE_WAIT_FOR_ALLOW);

            } else if (sub != WIFIDIR_GO_NEG_SUCCESS) {
                ERR("Sent neg response with unsuccessful status %d. Bailing\n.",
                    sub);
                change_state(WIFIDIR_STATE_FIND);
            }
            break;
        }

        if (es != WIFIDIR_EVENTS_DRIVER || e != EV_TYPE_WIFIDIR_NEGOTIATION_RESULT) {
            UNEXPECTED_EVENT(es, e);
            break;
        }

        /* If we get here, we were the recipient of a GO negotiation request
         * and the result is in.  Parse it, apply it, and alert the user.
         */
        ret = wifidir_parse_neg_result(&temp_peer, &neg_result, sub);
        if (ret != WIFIDIR_ERR_SUCCESS) {
            neg_result.status = temp_peer.go_neg_status;
            wifidir_emit_neg_result_event(&neg_result);
            change_state(WIFIDIR_STATE_FIND);
            break;
        }

        neg_result.status = WIFIDIR_GO_NEG_SUCCESS;

        wps_cancel_timer(wifidir_go_timeout, NULL);
        if (neg_result.is_go) {
            ret = wifidir_handle_neg_result(&neg_result);
            if (ret != WIFIDIR_ERR_SUCCESS) {
                change_state(WIFIDIR_STATE_FIND);
                break;
            }
            change_state(WIFIDIR_STATE_GO);
        } else {
            wait_go_count = 0;
            if (current_peer->group_cfg_timeout > 0)
                wps_start_timer(0, current_peer->group_cfg_timeout * 1000,
                                wifidir_go_timeout, NULL);
            else
                wps_start_timer(WIFIDIR_GO_UP_TIMER, 0, wifidir_go_timeout,
                                NULL);
            change_state(WIFIDIR_STATE_WAIT_FOR_GO_UP);
        }
        break;

    case WIFIDIR_STATE_WAIT_FOR_INITIATOR_GO_RESULT:

        if (handle_deinit_event(es, cmd))
            break;

        if (es != WIFIDIR_EVENTS_DRIVER || e != EV_TYPE_WIFIDIR_NEGOTIATION_RESULT) {
            UNEXPECTED_EVENT(es, e);
            break;
        }

        /* If we were the initiator, we get the go negotiation status in the
         * RESPONSE event, not the RESULT event.  So if we've arrived here, and
         * the event subtype does not indicate failure, then GO negotiation
         * succeeded.
         */
        ret = wifidir_parse_neg_result(&temp_peer, &neg_result, sub);
        if (ret != WIFIDIR_ERR_SUCCESS) {
            neg_result.status = temp_peer.go_neg_status;
            wifidir_emit_neg_result_event(&neg_result);
            change_state(WIFIDIR_STATE_FIND);
            break;
        }

        neg_result.status = WIFIDIR_GO_NEG_SUCCESS;

        wps_cancel_timer(wifidir_go_timeout, NULL);
        if (neg_result.is_go) {
            ret = wifidir_handle_neg_result(&neg_result);
            if (ret != WIFIDIR_ERR_SUCCESS) {
                change_state(WIFIDIR_STATE_FIND);
                break;
            }
            change_state(WIFIDIR_STATE_GO);
        } else {
            wait_go_count = 0;
            if (current_peer->group_cfg_timeout > 0)
                wps_start_timer(0, current_peer->group_cfg_timeout * 1000,
                                wifidir_go_timeout, NULL);
            else
                wps_start_timer(WIFIDIR_GO_UP_TIMER, 0, wifidir_go_timeout,
                                NULL);
            change_state(WIFIDIR_STATE_WAIT_FOR_GO_UP);
        }
        break;

    case WIFIDIR_STATE_WAIT_FOR_ALLOW:
        if (handle_deinit_event(es, cmd)) {
            break;

        } else if (es == WIFIDIR_EVENTS_UI &&
                   cmd->type == WIFIDIR_UI_CMD_ALLOW) {

            struct wifidir_pd_info *pdi = (struct wifidir_pd_info *)cmd->val;
            wps_cancel_timer(wifidir_go_timeout, NULL);

            if (memcmp(pdi->device_address, current_peer->device_address, ETH_ALEN) != 0) {
                /* It is possible that we are here because some random node
                 * initiated GO negotiation with us and the user has no idea.
                 * So if they call allow with a different device, we should
                 * honor that and let the random guy just timeout.
                 */
                INFO("Got allow() for new peer.  Switching to that peer\n");
                ret = wifidir_do_allow(pdi, &current_peer);
                if (ret == WIFIDIR_ERR_SUCCESS)
                    change_state(WIFIDIR_STATE_EXPECT_GO_REQUEST);
                else
                    change_state(WIFIDIR_STATE_FIND);
                break;
            }

            /* We are waiting for allow() and we got it.  Re-initiate GO
             * negotiation with the current peer.
             */
            /* There should be only one method in methods element */
            if (pdi->methods & MODULE_CONFIG_METHOD_DISPLAY)
                dev_passwd = DEVICE_PASSWORD_USER_SPECIFIED;
            else if (pdi->methods & MODULE_CONFIG_METHOD_KEYPAD)
                dev_passwd = DEVICE_PASSWORD_REG_SPECIFIED;
            else if (pdi->methods & MODULE_CONFIG_METHOD_PBC)
                dev_passwd = DEVICE_PASSWORD_PUSH_BUTTON;
            else {
                ERR("Invalid config method: [%4x]. Should be one of DISPLAY,"
                    "KEYPAD or PBC", pdi->methods);
                break;
            }

            ret = wifidir_cfg_cmd_provisioning_params(pdi, dev_passwd);
            if (ret != WIFIDIR_ERR_SUCCESS) {
                ERR("Failed to set provisioning params.   Going STATE_FIND\n");
                change_state(WIFIDIR_STATE_FIND);
                break;
            }

            ret = wifidir_cfg_cmd_set_wps_params(pdi->methods);
            if (ret != WIFIDIR_ERR_SUCCESS) {
                ERR("Failed to set wps params.  Going STATE_FIND\n");
                change_state(WIFIDIR_STATE_FIND);
                break;
            }
            ret = send_wifidir_action_frame(current_peer->device_address,
                                       WIFIDIR_GO_NEG_REQ_ACTION_SUB_TYPE);
            if (ret != WIFIDIR_ERR_SUCCESS) {
                ERR("Failed to re-initiate GO negotiation.  Bailing.\n");
                change_state(WIFIDIR_STATE_FIND);
            } else {
                change_state(WIFIDIR_STATE_GO_REQUEST_SENT);
            }

        } else if (es == WIFIDIR_EVENTS_UI &&
                   cmd->type == WIFIDIR_INTERNAL_EVENT_GO_TIMEOUT) {
            /* The user never called allow().  No need to emit a negotiation
             * result.  We only have to do that if they called
             * negotiate_group() or allow().
             */
            ERR("Timed out waiting for allow().  Going STATE_FIND.\n");
            change_state(WIFIDIR_STATE_FIND);

        } else if (es == WIFIDIR_EVENTS_UI &&
                    cmd->type == WIFIDIR_UI_CMD_NEGOTIATE_GROUP) {
            /* We're waiting for allow() because some other peer initiated with
             * us.  We haven't even alerted our user at this point.  So it's
             * possible that our user want to connect to a different peer.
             * Don't preclude them from initiating GO negotiation with a peer.
             */
            wps_cancel_timer(wifidir_go_timeout, NULL);
            ret = wifidir_do_negotiate_group((struct wifidir_pd_info *)cmd->val,
                                          &current_peer);
            break;

        } else {
            UNEXPECTED_EVENT(es, e);
        }
        break;

    case WIFIDIR_STATE_EXPECT_GO_REQUEST:

        if (handle_deinit_event(es, cmd)) {
            wps_cancel_timer(wifidir_go_timeout, NULL);
            break;

        } else if (es == WIFIDIR_EVENTS_UI &&
                    cmd->type == WIFIDIR_UI_CMD_NEGOTIATE_GROUP) {
            /* It's probable that we find ourselves in this state because
             * somebody called allow().  This does not preclude them from
             * initiating GO negotiation with a peer.
             */
            wps_cancel_timer(wifidir_go_timeout, NULL);
            ret = wifidir_do_negotiate_group((struct wifidir_pd_info *)cmd->val,
                                          &current_peer);
            break;

        } else if (es == WIFIDIR_EVENTS_DRIVER &&
                   e == EV_TYPE_WIFIDIR_NEGOTIATION_REQUEST) {

            if (memcmp(wifidir_event->peer_mac_addr, current_peer->device_address,
                       ETH_ALEN) != 0) {
                ERR("Ignoring GO negotiation request from unexpected peer "
                    UTIL_MACSTR "\n", UTIL_MAC2STR(wifidir_event->peer_mac_addr));
                break;
            }

            if (sub == EV_SUBTYPE_WIFIDIR_GO_NEG_FAILED) {
                ERR("GO negotiation failed.\n");
                wps_cancel_timer(wifidir_go_timeout, NULL);
                do_go_error(wifidir_event->peer_mac_addr,
                            WIFIDIR_GO_NEG_INTERNAL_ERR);
                break;
            }

            wps_cancel_timer(wifidir_go_timeout, NULL);
            memcpy(current_peer->interface_address, temp_peer.interface_address,
                   ETH_ALEN);
            current_peer->group_cfg_timeout = temp_peer.group_cfg_timeout;
            wifidir_emit_peer_event(current_peer, WIFIDIR_EVENT_PEER_UPDATED);
            change_state(WIFIDIR_STATE_WAIT_FOR_GO_RESULT);

        } else if (es == WIFIDIR_EVENTS_UI &&
                 cmd->type == WIFIDIR_INTERNAL_EVENT_GO_TIMEOUT) {
            /* The expected GO negotiation request never appeared. Alert the
             * user.
             */
            do_go_error(current_peer->device_address,
                        WIFIDIR_GO_NEG_INTERNAL_TIMEOUT);
            change_state(WIFIDIR_STATE_FIND);
        }
        break;

    case WIFIDIR_STATE_GO:
        if (handle_deinit_event(es, cmd))
            break;

        if (es == WIFIDIR_EVENTS_UI && cmd->type == WIFIDIR_UI_CMD_ALLOW) {
            /* When Auto-GO, we will receive allow command in this state.
             */
            INFO("Get allow command in WIFIDIR_STATE_GO\n");
            ret = wifidir_do_allow((struct wifidir_pd_info *)cmd->val,
                                   &current_peer);
        }

        UNEXPECTED_EVENT(es, e);
        break;

    case WIFIDIR_STATE_WAIT_FOR_GO_UP:
        if (handle_deinit_event(es, cmd))
            break;

        if (es == WIFIDIR_EVENTS_UI &&
            cmd->type == WIFIDIR_INTERNAL_EVENT_GO_TIMEOUT) {
            INFO("WIFIDIR_STATE_WAIT_FOR_GO_UP: %d\n", wait_go_count);

            if (wait_go_count > MAX_WAIT_GO_COUNT) {
                INFO("Fail to wait GO ready.\n");
                neg_result.status = WIFIDIR_GO_NEG_INTERNAL_ERR;
            } else {
                i = wifidir_is_go_up(current_peer->interface_address,
                        neg_result.go_ssid);
                if (i == 0) {
                    /* GO is not up */
                    ++wait_go_count;
                    wps_cancel_timer(wifidir_go_timeout, NULL);
                    if (current_peer->group_cfg_timeout > 0)
                        wps_start_timer(0, current_peer->group_cfg_timeout * 1000,
                                        wifidir_go_timeout, NULL);
                    else
                        wps_start_timer(WIFIDIR_GO_UP_TIMER, 0,
                                        wifidir_go_timeout, NULL);
                    break;
                } else if (i == -1) {
                    /* Error */
                    neg_result.status = WIFIDIR_GO_NEG_INTERNAL_ERR;
                }
            }

            ret = wifidir_handle_neg_result(&neg_result);
            if (ret != WIFIDIR_ERR_SUCCESS) {
                change_state(WIFIDIR_STATE_FIND);
                break;
            }

            change_state(WIFIDIR_STATE_CLIENT);
        }

        break;

    case WIFIDIR_STATE_CLIENT:
        if (handle_deinit_event(es, cmd))
            break;

        UNEXPECTED_EVENT(es, e);
        break;
    }

    if(current_peer != NULL) {
        pwifidir_data->current_peer = current_peer;
    }
    LEAVE();
    return ret;
}

/* API FUNCTIONS START HERE */

enum wifidir_error wifidir_start_group(struct module *mod,
                                      struct wifidir_group_info *group_info)
{
    int ret;
    struct event *ui_cmd;

    if (wifidir_mod == NULL || mod != wifidir_mod) {
        ERR("Invalid module structure passed to %s!", __func__);
        return WIFIDIR_ERR_INVAL;
    }

    ui_cmd = (struct event *) (malloc(sizeof(struct event) +
                sizeof(struct wifidir_group_info)));

    if (!ui_cmd)
        return WIFIDIR_ERR_NOMEM;

    ui_cmd->type = WIFIDIR_UI_CMD_START_GROUP;
    ui_cmd->len = sizeof(struct wifidir_group_info);
    memcpy(ui_cmd->val, group_info, ui_cmd->len);

    ret = wifidir_state_machine((u8 *)ui_cmd, ui_cmd->len, WIFIDIR_EVENTS_UI);
    free(ui_cmd);
    return ret;
}




void wifidir_stop_find(struct module *mod)
{
    int ret;
    struct event *ui_cmd;

    if (wifidir_mod == NULL || mod != wifidir_mod) {
        ERR("Invalid module structure passed to %s!", __func__);
        return;
    }

    ui_cmd = (struct event *)malloc(sizeof(struct event));

    if (!ui_cmd) {
        ERR("Can not allocate memory for ui cmd stop_find.\n");
        /* Shall we consider changing prototype of this func to int */
        return;
    }
    ui_cmd->type = WIFIDIR_UI_CMD_STOP_FIND;
    ui_cmd->len = 0;

    ret = wifidir_state_machine((u8 *)ui_cmd, ui_cmd->len, WIFIDIR_EVENTS_UI);
    free(ui_cmd);
    return;
}


enum wifidir_error wifidir_start_find(struct module *mod,
                                      struct wifidir_find_info *find_info)
{
    int ret;
    struct event *ui_cmd;

    if (wifidir_mod == NULL || mod != wifidir_mod) {
        ERR("Invalid module structure passed to %s!", __func__);
        return WIFIDIR_ERR_INVAL;
    }

    ui_cmd = (struct event *)malloc(sizeof(struct event));

    if (!ui_cmd)
        return WIFIDIR_ERR_NOMEM;

    ui_cmd->type = WIFIDIR_UI_CMD_START_FIND;
    ui_cmd->len = 0;

    ret = wifidir_state_machine((u8 *)ui_cmd, ui_cmd->len, WIFIDIR_EVENTS_UI);
    free(ui_cmd);
    return ret;
}


int wifidir_init(struct module *mod, struct wifidir_cfg *cfg)
{
    int ret;
    struct event *ui_cmd;

    if (mod == NULL || mod->iface[0] == 0)
        return WIFIDIR_ERR_INVAL;

    if (wifidir_mod != NULL) {
        ERR("Only one instance of wifidir is supported at this time\n");
        return WIFIDIR_ERR_BUSY;
    }
    wifidir_mod = mod;

    if (!cfg || cfg->intent < 0 || cfg->intent > MAX_INTENT ||
        !strlen(cfg->name))
        return WIFIDIR_ERR_INVAL;

    /** Start with an empty peer list */
    if (!SLIST_EMPTY(&peers)) {
        ERR("Peer list is not empty.  Apparently deinit was not called.\n");
        return WIFIDIR_ERR_BUSY;
    }
    SLIST_INIT(&peers);

    ui_cmd = (struct event *)malloc(sizeof(struct event) +
            sizeof(struct wifidir_cfg) + 1);
    if (!ui_cmd)
        return WIFIDIR_ERR_NOMEM;

    ui_cmd->type = WIFIDIR_UI_CMD_INIT;
    ui_cmd->len = sizeof(struct wifidir_cfg);
    memcpy(ui_cmd->val, cfg, ui_cmd->len);

    ret = wifidir_state_machine((u8 *)ui_cmd, ui_cmd->len, WIFIDIR_EVENTS_UI);
    free(ui_cmd);
    return ret;
}

void wifidir_deinit(struct module *mod)
{
	int ret;
    struct event *ui_cmd;

    if (wifidir_mod == NULL || mod != wifidir_mod) {
        ERR("Invalid module structure passed to %s!", __func__);
        return;
    }

	/** Clean the peer table */
    wifidir_peer_free_all();

	ui_cmd = (struct event *)malloc(sizeof(struct event));
    if (!ui_cmd)
        return;

    ui_cmd->type = WIFIDIR_UI_CMD_DEINIT;
    ui_cmd->len = 0;

    ret = wifidir_state_machine((u8 *)ui_cmd, ui_cmd->len, WIFIDIR_EVENTS_UI);
    wifidir_mod = NULL;
}

const struct peer_list *wifidir_get_peers(struct module *mod)
{

    if (wifidir_mod == NULL || mod != wifidir_mod) {
        ERR("Invalid module structure passed to %s!", __func__);
        return NULL;
    }

    /* Because we have only a single event loop, there is no need to lock or
     * copy the peers before returning them to the caller.  In the future, if
     * the wifidir module is used in a multi-threaded environment, a lock for
     * the peer table could be taken here and released in
     * wifidir_release_peers.
     */
    return SLIST_EMPTY(&peers) ? NULL : &peers;
}

void wifidir_release_peers(struct module *mod,
                           const struct peer_list *peer_list)
{
    if (wifidir_mod == NULL || mod != wifidir_mod) {
        ERR("Invalid module structure passed to %s!", __func__);
        return;
    }

    return;
}

enum wifidir_error wifidir_launch(void)
{
    /* We should never assume that the last shutdown was clean.  So, call
     * deinit here to get a clean start.
     */
    wifidir_do_deinit();

    return WIFIDIR_ERR_SUCCESS;
}

void wifidir_halt(void)
{
    return;
}

/* many of our API commands take a pdi data structure as an argument.  So add a
 * helper function to handle these.
 */
static enum wifidir_error do_pd_command(enum wifidir_ui_cmd_type cmd,
                                        struct wifidir_pd_info *pdi)
{
    int ret;
    struct event *ui_cmd;

    if (!pdi)
        return WIFIDIR_ERR_INVAL;

    ui_cmd = (struct event *)malloc(sizeof(struct event) +
            sizeof(struct wifidir_pd_info));
    if (!ui_cmd)
        return WIFIDIR_ERR_NOMEM;

    ui_cmd->type = cmd;
    ui_cmd->len = sizeof(struct wifidir_pd_info);
    memcpy(ui_cmd->val, pdi, ui_cmd->len);

    ret = wifidir_state_machine((u8 *)ui_cmd, ui_cmd->len, WIFIDIR_EVENTS_UI);
    free(ui_cmd);
    return ret;
}

enum wifidir_error wifidir_pd_request(struct module *mod,
                                      struct wifidir_pd_info *pdi)
{

    if (wifidir_mod == NULL || mod != wifidir_mod) {
        ERR("Invalid module structure passed to %s!", __func__);
        return WIFIDIR_ERR_INVAL;
    }

    return do_pd_command(WIFIDIR_UI_CMD_PDREQ, pdi);
}

enum wifidir_error wifidir_negotiate_group(struct module *mod,
                                           struct wifidir_pd_info *pdi)
{
    if (wifidir_mod == NULL || mod != wifidir_mod) {
        ERR("Invalid module structure passed to %s!", __func__);
        return WIFIDIR_ERR_INVAL;
    }
    return do_pd_command(WIFIDIR_UI_CMD_NEGOTIATE_GROUP, pdi);
}

enum wifidir_error wifidir_allow(struct module *mod,
                                 struct wifidir_pd_info *pdi)
{
    /* TODO: The driver/firmware does not provide a means of allowing
     * negotiation with just a single device.  So for now, the device_address
     * parameter is ignored.
     */
    if (wifidir_mod == NULL || mod != wifidir_mod) {
        ERR("Invalid module structure passed to %s!", __func__);
        return WIFIDIR_ERR_INVAL;
    }

    return do_pd_command(WIFIDIR_UI_CMD_ALLOW, pdi);
}
