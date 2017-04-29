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

#include <errno.h>
#include <stdio.h>

#define UTIL_LOG_TAG "MWPAMOD"
#include "util.h"
#include "wps_os.h"
#include "mwpamod.h"
#include "mwpamod_os.h"
#include "mwu_log.h"

extern struct module *mwpamod;

extern void mwpamod_sta_kernel_event(struct event *e);

int sock = -1;
static inline int is_zero_mac(const u8 *a) {
    return !(a[0] | a[1] | a[2] | a[3] | a[4] | a[5]);
}



static inline int index2name(int ifindex, char *name) {

    struct ifreq irq;
    int ret = 0;

    memset(name, 0, IFNAMSIZ + 1);

    irq.ifr_ifindex = ifindex;

    if(ioctl(sock, SIOCGIFNAME, &irq) < 0)
        ret -1;
    else
        strncpy(name, irq.ifr_name, IFNAMSIZ);

    return ret;
}

void mwpamod_wext_event_raise(int ifindex, char *data, int len) {

    struct iw_event iwe_buf, *iwe = &iwe_buf;
    char *pos, *end, *custom, *buf = NULL;
    char ifname[IFNAMSIZ + 1];

    /* Event to pass to state machine */
    struct event *e = NULL;

    /* Allocate the event - We send the iface the event came from as the payload
     * of this event to make the state machine aware of the interface event
     * arrieved from. */

    e = malloc(sizeof(struct event) + IFNAMSIZ + 1);
    if (!e) {
        ERR("Failed to allocate memory for event");
        return;
    }


    pos = data;
    end = data + len;

    while (pos + IW_EV_LCP_LEN <= end) {

        memcpy(&iwe_buf, pos, IW_EV_LCP_LEN);

        INFO("Wext Event: cmd = 0x%x, len = 0x%x", iwe->cmd, iwe->len);
        //mwu_hexdump(MSG_ERROR, "EVENT BUFF :", pos, iwe->len);
        if(iwe->len <= IW_EV_LCP_LEN) {
            ERR("Invalid event length");
            goto error;
        }

        /* get the interface */
        if (index2name(ifindex, ifname) < 0 ) {
            ERR("Can't detect interface name from wext event");
            goto error;
        }

        custom = pos + IW_EV_POINT_LEN;

        /* WE-19 removed the pointer from struct iw_point
         * And we _assume_ we will never run into WE < 19
         * If required to detect the WE version, it can be done at the cost of
         * another ioctl,here*/
		if (iwe->cmd == IWEVCUSTOM) {
			char *dpos = (char *) &iwe_buf.u.data.length;
			int dlen = dpos - (char *) &iwe_buf;
			memcpy(dpos, pos + IW_EV_LCP_LEN,
				  sizeof(struct iw_event) - dlen);
		} else {
			memcpy(&iwe_buf, pos, sizeof(struct iw_event));
			custom += IW_EV_POINT_OFF;
		}

        switch (iwe->cmd) {
        case SIOCGIWAP:
            if (is_zero_mac((const u8 *) iwe->u.ap_addr.sa_data)) {
                INFO("%s: Connection lost", ifname);
                e->type = STA_KERNEL_EVENT_LINK_LOST;
                e->len = IFNAMSIZ;
                strncpy(e->val, ifname, e->len);
                mwpamod_sta_kernel_event(e);
            } else {
                INFO("%s: Associated with  " UTIL_MACSTR, ifname, UTIL_MAC2STR((u8 *) iwe->u.ap_addr.sa_data));
                e->type = STA_KERNEL_EVENT_ASSOCIATED;
                e->len = IFNAMSIZ;
                strncpy(e->val, ifname, e->len);
                mwpamod_sta_kernel_event(e);
            }
            break;

        case IWEVCUSTOM:
            if (custom + iwe->u.data.length > end) {
                ERR("%s: Invalid IWCUSTOM length", ifname);
                goto error;
            }
            buf = (char *)malloc(iwe->u.data.length + 1);
            if (buf == NULL) {
                ERR("No memory for custom IWEVENT data allocation");
                goto error;
            }

            memcpy(buf, custom, iwe->u.data.length);
            buf[iwe->u.data.length] = '\0';
            if (strcmp(buf, "EVENT=PORT_RELEASE") == 0) {
                INFO("%s: PORT_RELEASE event received. Authenticated!", ifname);
                e->type = STA_KERNEL_EVENT_AUTHENTICATED;
                e->len = IFNAMSIZ;
                strncpy(e->val, ifname, e->len);
                mwpamod_sta_kernel_event(e);
            }
            free(buf);
            break;

        default:
            INFO("%s: Event 0x%x ignored", ifname, iwe->cmd);
        }
        pos += iwe->len;
    }

error:
    free(e);
    return;
}


void mwpamod_wext_event_catch(struct nlmsghdr *nlh) {

    struct ifinfomsg* ifi;

    /* Check for attributes */
    ifi = NLMSG_DATA(nlh);
    if (nlh->nlmsg_len > NLMSG_ALIGN(sizeof(struct ifinfomsg))) {
        int attrlen = nlh->nlmsg_len - NLMSG_ALIGN(sizeof(struct ifinfomsg));
        struct rtattr *attr = (void *) ((char *) ifi +
                NLMSG_ALIGN(sizeof(struct ifinfomsg)));

        while (RTA_OK(attr, attrlen)) {
            if(attr->rta_type == IFLA_WIRELESS) {
               mwpamod_wext_event_raise(ifi->ifi_index, (char *) attr + RTA_ALIGN(sizeof(struct rtattr)),
                        attr->rta_len - RTA_ALIGN(sizeof(struct rtattr)));
            }
            attr = RTA_NEXT(attr, attrlen);
        }
    }
}

void mwpamod_wext_event_receive(int fd, void *rth) {

    struct sockaddr_nl sanl;
    socklen_t sanllen = sizeof(struct sockaddr_nl);

    struct nlmsghdr *h;
    int amt, size = 0;
    char buf[8192];

    amt = recvfrom(fd, buf, sizeof(buf), MSG_DONTWAIT, (struct sockaddr*)&sanl, &sanllen);
    size = amt;
    if(amt < 0) {
        if(errno != EINTR && errno != EAGAIN) {
            ERR("Error reading netlink: %s.\n", strerror(errno));
        }
        return;
    }

    if(amt == 0) {
        ERR("EOF on netlink??\n");
        return;
    }

    h = (struct nlmsghdr*)buf;
    while(amt >= (int)sizeof(*h)) {
        int len = h->nlmsg_len;
        int l = len - sizeof(*h);

        if(l < 0 || len > amt) {
            ERR("Malformed netlink message: len=%d\n", len);
            break;
        }

        switch(h->nlmsg_type) {
        case RTM_NEWLINK:
        case RTM_DELLINK:
            //mwu_hexdump(MSG_ERROR, "NETLINK EVENT: ", buf, size);
            mwpamod_wext_event_catch(h);
            break;
        default:
            break;
        }

        len = NLMSG_ALIGN(len);
        amt -= len;
        h = (struct nlmsghdr*)((char*)h + len);
    }

    if(amt > 0)
        ERR("Remnant of size %d on netlink\n", amt);

    return;
}


inline void rtnl_close(struct rtnl_handle *rth) {

    wps_unregister_rdsock_handler(rth->fd);
    close(rth->fd);
}

inline int rtnl_open(struct rtnl_handle *rth, unsigned subscriptions) {
    int addr_len;
    void *data = NULL;

    memset(rth, 0, sizeof(rth));

    rth->fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (rth->fd < 0) {
        ERR("Cannot open netlink socket");
        return MWPAMOD_ERR_COM;
    }

    memset(&rth->local, 0, sizeof(rth->local));
    rth->local.nl_family = AF_NETLINK;
    rth->local.nl_groups = subscriptions;

    if (bind(rth->fd, (struct sockaddr*)&rth->local, sizeof(rth->local)) < 0) {
        ERR("Cannot bind netlink socket");
        return MWPAMOD_ERR_COM;
    }
    addr_len = sizeof(rth->local);
    if (getsockname(rth->fd, (struct sockaddr*)&rth->local, &addr_len) < 0) {
        ERR("Cannot getsockname");
        return MWPAMOD_ERR_COM;
    }
    if (addr_len != sizeof(rth->local)) {
        fprintf(stderr, "Wrong address length %d\n", addr_len);
        return MWPAMOD_ERR_COM;
    }
    if (rth->local.nl_family != AF_NETLINK) {
        fprintf(stderr, "Wrong address family %d\n", rth->local.nl_family);
        return MWPAMOD_ERR_COM;
    }
    rth->seq = time(NULL);


    /* open AF_INET socket to send ioctl to extract ifname from event */
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
        ERR("Cant open socket");

    /* Monitor this descriptor */
    wps_register_rdsock_handler(rth->fd, mwpamod_wext_event_receive, data);

    return MWPAMOD_ERR_SUCCESS;
}
