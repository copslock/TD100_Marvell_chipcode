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

#ifndef _MWPAMOD_OS_H_
#define _MWPAMOD_OS_H_

#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/wireless.h>

struct rtnl_handle
{
    int         fd;
    struct sockaddr_nl  local;
    struct sockaddr_nl  peer;
    __u32           seq;
    __u32           dump;
};


void mwpamod_wext_event_raise(int ifindex, char *data, int len);

void mwpamod_wext_event_catch(struct nlmsghdr *nlh);

void mwpamod_wext_event_receive(int fd, void *rth);


inline void rtnl_close(struct rtnl_handle *rth);
inline int rtnl_open(struct rtnl_handle *rth, unsigned subscriptions);

/*Please note, these are the internal events received from wext and passed to STA SM */
enum sta_kernel_event {
    STA_KERNEL_EVENT_ASSOCIATED,
    STA_KERNEL_EVENT_AUTHENTICATED,
    STA_KERNEL_EVENT_LINK_LOST,
};

#endif
