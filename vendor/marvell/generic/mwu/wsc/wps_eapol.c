/** @file wps_eapol.c
 *  @brief This file contains functions for EAPOL packet Tx/Rx.
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

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <signal.h>

#include "wps_def.h"
#include "mwu_log.h"
#include "util.h"
#include "wps_msg.h"
#include "wps_wlan.h"
#include "wps_eapol.h"
#include "wps_l2.h"

/********************************************************
        Local Variables
********************************************************/

/********************************************************
        Global Variables
********************************************************/
extern PWPS_INFO gpwps_info;

extern int auto_go;
/********************************************************
        Local Functions
********************************************************/
/**
 *  @brief Allocate and prepare a EAPOL packet
 *
 *  @param type         Type of EAPOL packet
 *  @param data         A pointer to payload of EAPOL packet
 *  @param data_len     Length of payload
 *  @param msg_len      A pointer to allocated length
 *  @return             A pointer to allocated buffer or NULL if fail on allocation
 */
static u8 *
wps_alloc_eapol(u8 type, const void *data, u16 data_len,
                size_t * msg_len, void **data_pos)
{
    struct ieee802_1x_hdr *hdr;

    ENTER();

    *msg_len = sizeof(*hdr) + data_len;
    hdr = malloc(*msg_len);
    if (hdr == NULL) {
        LEAVE();
        return NULL;
    }
#define DEFAULT_EAPOL_VERSION 1

    hdr->version = DEFAULT_EAPOL_VERSION;
    hdr->type = type;
    hdr->length = mwu_htons(data_len);

    if (data)
        memcpy(hdr + 1, data, data_len);
    else
        memset(hdr + 1, 0, data_len);

    if (data_pos)
        *data_pos = hdr + 1;

    LEAVE();
    return (u8 *) hdr;
}

/********************************************************
        Global Functions
********************************************************/
/**
 *  @brief Send Ethernet frame to layer 2 socket
 *
 *  @param dest         Destination MAC address
 *  @param proto        Ethertype
 *  @param buf          Frame payload starting from IEEE 802.1X header
 *  @param len          Frame payload length
 *  @return             Result of layer 2 send function
 */
int
wps_ether_send(const u8 * dest, u16 proto, const u8 * buf, size_t len)
{
    WPS_DATA *wps_s = (WPS_DATA *) & gpwps_info->wps_data;

    ENTER();

    return wps_l2_send(wps_s->l2, dest, proto, buf, len);
}

/**
 *  @brief Send IEEE 802.1X EAPOL packet to Authenticator
 *         This function adds Ethernet and IEEE 802.1X header
 *         and sends the EAPOL frame to the current Authenticator.
 *
 *  @param type         IEEE 802.1X packet type (IEEE802_1X_TYPE_*)
 *  @param buf          EAPOL payload (after IEEE 802.1X header)
 *  @param len          EAPOL payload length
 *  @return             Result of Ether sending function
 */
int
wps_eapol_send(int type, u8 * buf, size_t len)
{
    u8 *msg, *dst;
    size_t msglen;
    int res;
    WPS_DATA *wps_s = (WPS_DATA *) & gpwps_info->wps_data;

    ENTER();

    /*
     * In IBSS mode, destination mac address is unknown from scan result
     */
    if (wps_s->current_ssid.mode == IEEE80211_MODE_IBSS) {
        if (gpwps_info->role == WPS_ENROLLEE)
            dst = (u8 *) & gpwps_info->registrar.mac_address[0];
        else {

            dst = (u8 *) & gpwps_info->enrollee.mac_address[0];
        }
    } else {
        dst = (u8 *) & gpwps_info->enrollee.mac_address[0];
        if (gpwps_info->role == WPS_ENROLLEE
            || (IS_DISCOVERY_ENROLLEE(gpwps_info))
            ) {
            dst = (u8 *) & gpwps_info->registrar.mac_address[0];
        } else {
            dst = (u8 *) & gpwps_info->enrollee.mac_address[0];
        }

    }

    mwu_printf(DEBUG_EAPOL, "TX EAPOL to " UTIL_MACSTR, UTIL_MAC2STR(dst));

    /* Payload of EAPOL copied in wps_alloc_eapol */
    msg = wps_alloc_eapol(type, buf, len, &msglen, NULL);
    if (msg == NULL) {
        LEAVE();
        return WPS_STATUS_FAIL;
    }
    mwu_hexdump(MSG_MSGDUMP, "TX EAPOL", msg, msglen);

    res = wps_ether_send(dst, ETH_P_EAPOL, msg, msglen);

    free(msg);

    LEAVE();
    return res;
}

/**
 *  @brief Send EAPOL-START packet
 *
 *  @return         Result of EAPOL sending function
 */
int
wps_eapol_txStart(void)
{
    int ret;

    ENTER();

    ret = wps_eapol_send(IEEE802_1X_TYPE_EAPOL_START, (u8 *) "", 0);
    if (ret < 0)
        mwu_printf(DEBUG_EAPOL, "wps_eapol_txStart fail");

    LEAVE();
    return ret;
}

/**
 *  @brief Send EAPOL packet
 *
 *  @param buf      EAP packet payload
 *  @param len      Length of payload
 *  @return         Result of EAPOL sending function
 */
int
wps_eapol_txPacket(u8 * buf, size_t len)
{
    int ret;

    ENTER();

    ret = wps_eapol_send(IEEE802_1X_TYPE_EAP_PACKET, buf, len);
    if (ret < 0)
        mwu_printf(DEBUG_EAPOL, "wps_eapol_txPacket fail");

    LEAVE();
    return ret;
}

/**
 *  @brief EAPOL packet received handler
 *
 *  @param src_addr     Layer 2 source address of EAPOL packet
 *  @param buf          Received packet buffer
 *  @param len          Received packet length
 *  @return             None
 */
void
wps_rx_eapol(const u8 * src_addr, const u8 * buf, size_t len)
{
    WPS_DATA *wps_s = (WPS_DATA *) & gpwps_info->wps_data;

    ENTER();

    /*
     * For autonomous GO,
     * note the STA address when receiving eapol packet
     */
    if (IS_DISCOVERY_REGISTRAR(gpwps_info))
        memcpy(gpwps_info->enrollee.mac_address, src_addr, ETH_ALEN);

    /*
     * For a existing GO, if EAPOL is received from a device, start as GO and
     * then respond to device
     */
    if (gpwps_info->role == WIFIDIR_ROLE &&
        gpwps_info->discovery_role == WPS_CANCEL) {

        memcpy(gpwps_info->enrollee.mac_address, src_addr, ETH_ALEN);
        gpwps_info->discovery_role = WPS_REGISTRAR;

        /* Update Group formation bit when starting as registrar */
        if(!auto_go) {
            if (wps_wlan_update_group_formation_config(WPS_SET) !=
                WPS_STATUS_SUCCESS) {
                mwu_printf(DEBUG_WIFIDIR_DISCOVERY,
                        "Failed to update Group formation bit.\n");
                LEAVE();
                return;
            }
        }
    }

    mwu_printf(DEBUG_EAPOL, "RX EAPOL from " UTIL_MACSTR, UTIL_MAC2STR(src_addr));
    mwu_hexdump(MSG_MSGDUMP, "RX EAPOL", buf, len);

    wps_s->eapol_received++;

    if (gpwps_info->pin_pbc_set == WPS_SET) {
        /* Callback to WPS state machine message handler */
        wps_message_handler((u8 *) buf, (u8 *) src_addr);
    }

    LEAVE();
}
