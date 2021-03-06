/** @file wps_l2.c
 *  @brief This file contains functions handling layer 2 socket read/write.
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
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netpacket/packet.h>

#include "wps_def.h"
#include "wps_msg.h"
#include "mwu_log.h"
#include "wps_l2.h"
#include "wps_os.h"

/********************************************************
        Local Variables
********************************************************/

/********************************************************
        Global Variables
********************************************************/

/********************************************************
        Local Functions
********************************************************/
/**
 *  @brief Process Layer 2 socket receive function
 *
 *  @param sock         Socket number for receiving
 *  @param l2_ctx       A pointer to user private information
 *  @return             None
 */
static void
wps_l2_receive(int sock, void *l2_ctx)
{
    WPS_L2_INFO *l2 = l2_ctx;
    u8 buf[2300];
    int res;
    struct sockaddr_ll ll;
    socklen_t fromlen;

    ENTER();

    memset(&ll, 0, sizeof(ll));
    fromlen = sizeof(ll);
    res =
        recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr *) &ll, &fromlen);
    if (res < 0) {
        LEAVE();
        return;
    }

    l2->rx_callback(ll.sll_addr, buf, (size_t) res);

    LEAVE();
}

/********************************************************
        Global Functions
********************************************************/
/**
 *  @brief Get Layer 2 MAC Address
 *
 *  @param l2           A pointer to structure of layer 2 information
 *  @param addr         A pointer to returned buffer
 *  @return             WPS_STATUS_SUCCESS--success, otherwise--fail
 */
int
wps_l2_get_mac(WPS_L2_INFO * l2, u8 * addr)
{
    ENTER();
    memcpy(addr, l2->my_mac_addr, ETH_ALEN);
    LEAVE();
    return WPS_STATUS_SUCCESS;
}

/**
 *  @brief Process Layer 2 socket send function
 *
 *  @param l2           A pointer to structure of layer 2 information
 *  @param dst_addr     Destination address to send
 *  @param proto        Protocol number for layer 2 packet
 *  @param buf          A pointer to sending packet buffer
 *  @param len          Packet length
 *  @return             WPS_STATUS_SUCCESS--success, otherwise--fail
 */
int
wps_l2_send(WPS_L2_INFO * l2, const u8 * dst_addr, u16 proto,
            const u8 * buf, size_t len)
{
    int ret;

    ENTER();

    if (l2 == NULL) {
        LEAVE();
        return WPS_STATUS_FAIL;
    }

    if (l2->l2_hdr) {
        ret = send(l2->fd, buf, len, 0);
        if (ret < 0)
            perror("wps_l2_send - send");
    } else {
        struct sockaddr_ll ll;
        memset(&ll, 0, sizeof(ll));
        ll.sll_family = AF_PACKET;
        ll.sll_ifindex = l2->ifindex;
        ll.sll_protocol = mwu_htons(proto);
        ll.sll_halen = ETH_ALEN;
        memcpy(ll.sll_addr, dst_addr, ETH_ALEN);
        ret = sendto(l2->fd, buf, len, 0, (struct sockaddr *) &ll, sizeof(ll));
        if (ret < 0) {
            if (errno == ENETDOWN)
                printf("Retrying...\n");
            else
                perror("wps_l2_send - sendto");
        }
    }

    LEAVE();
    return ret;
}

/**
 *  @brief Process Layer 2 socket initialization
 *
 *  @param ifname       Interface name
 *  @param protocol     Ethernet protocol number in host byte order
 *  @param rx_callback  Callback function that will be called for each received packet
 *  @param l2_hdr       1 = include layer 2 header, 0 = do not include header
 *  @return             A pointer to l2 structure
 */
WPS_L2_INFO *
wps_l2_init(const char *ifname,
            unsigned short protocol,
            void (*rx_callback) (const u8 * src_addr,
                                 const u8 * buf, size_t len), int l2_hdr)
{
    WPS_L2_INFO *l2;
    struct ifreq ifr;
    struct sockaddr_ll ll;

    ENTER();

    l2 = malloc(sizeof(WPS_L2_INFO));
    if (l2 == NULL) {
        LEAVE();
        return NULL;
    }

    memset(l2, 0, sizeof(*l2));
    strncpy(l2->ifname, ifname, sizeof(l2->ifname));
    l2->rx_callback = rx_callback;
    l2->l2_hdr = l2_hdr;

    l2->fd = socket(PF_PACKET, l2_hdr ? SOCK_RAW : SOCK_DGRAM, mwu_htons(protocol));
    if (l2->fd < 0) {
        perror("socket(PF_PACKET)");
        free(l2);
        LEAVE();
        return NULL;
    }

    strncpy(ifr.ifr_name, l2->ifname, sizeof(ifr.ifr_name));
    if (ioctl(l2->fd, SIOCGIFINDEX, &ifr) < 0) {
        perror("ioctl[SIOCGIFINDEX]");
        close(l2->fd);
        free(l2);
        LEAVE();
        return NULL;
    }
    l2->ifindex = ifr.ifr_ifindex;

    memset(&ll, 0, sizeof(ll));
    ll.sll_family = PF_PACKET;
    ll.sll_ifindex = ifr.ifr_ifindex;
    ll.sll_protocol = mwu_htons(protocol);
    if (bind(l2->fd, (struct sockaddr *) &ll, sizeof(ll)) < 0) {
        perror("bind[PF_PACKET]");
        close(l2->fd);
        free(l2);
        LEAVE();
        return NULL;
    }

    if (ioctl(l2->fd, SIOCGIFHWADDR, &ifr) < 0) {
        perror("ioctl[SIOCGIFHWADDR]");
        close(l2->fd);
        free(l2);
        LEAVE();
        return NULL;
    }
    memcpy(l2->my_mac_addr, ifr.ifr_hwaddr.sa_data, ETH_ALEN);

    /* register the read socket handler for l2 packet */
    wps_register_rdsock_handler(l2->fd, wps_l2_receive, l2);

    LEAVE();
    return l2;
}

/**
 *  @brief Process Layer 2 socket free
 *
 *  @param l2       A pointer to user private information
 *  @return         None
 */
void
wps_l2_deinit(WPS_L2_INFO * l2)
{
    ENTER();

    if (l2 == NULL) {
        LEAVE();
        return;
    }

    if (l2->fd >= 0) {
        wps_unregister_rdsock_handler(l2->fd);
        close(l2->fd);
    }

    free(l2);

    LEAVE();
}
