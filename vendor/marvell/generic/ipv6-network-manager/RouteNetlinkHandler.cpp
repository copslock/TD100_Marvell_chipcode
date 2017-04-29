/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <cutils/properties.h>

#define LOG_TAG "ipv6-network-manager"

#include <cutils/log.h>

#include "RouteNetlinkHandler.h"
#include "RouteNetlinkManager.h"

#define ND_OPT_PREFIX_INFORMATION       3
#define ND_OPT_RDNSS                    25
#define ND_ROUTER_ADVERT                134     /* router advertisement */

struct nd_opt_hdr {     /* Neighbor discovery option header */
    u_int8_t    nd_opt_type;
    u_int8_t    nd_opt_len;
    /* followed by option specific data*/
} __packed;

struct nd_opt_rdnss {       /* RDNSS option RFC 5006 */
    u_int8_t    nd_opt_rdnss_type;
    u_int8_t    nd_opt_rdnss_len;
    u_int16_t   nd_opt_rdnss_reserved;
    u_int32_t   nd_opt_rdnss_lifetime;
    /* followed by list of IP prefixes */
} __packed;

static bool
process_nduseropt_rdnss (const char *ifname, struct nd_opt_hdr *opt)
{
    size_t opt_len = opt->nd_opt_len; // 8 bytes per unit

    if (opt_len < 3 || (opt_len & 1) == 0)
        return false;

    struct nd_opt_rdnss *rdnss_opt = (struct nd_opt_rdnss *) opt;
    struct in6_addr *addr = (struct in6_addr *) (rdnss_opt + 1);
    int num = (opt_len - 1) >> 1;
    num = num < 2 ? num : num;
    for (int i = 0;  i < num; i++) {
        char dns[INET6_ADDRSTRLEN + 1];
        inet_ntop(AF_INET6, &addr[i], dns, sizeof (dns));
        char prop_name[PROPERTY_KEY_MAX];
        snprintf(prop_name, sizeof(prop_name), "net.%s.dns%d", ifname, i + 1);
        property_set(prop_name, dns);
    }

    return true;
}

static bool
process_nduseropt (struct nlmsghdr *nlh)
{
    struct nduseroptmsg *ndmsg = (struct nduseroptmsg *) NLMSG_DATA (nlh);
    bool changed = false;

    SLOGD ("processing netlink nduseropt message");

    if (ndmsg->nduseropt_family != AF_INET6 ||
        ndmsg->nduseropt_icmp_type != ND_ROUTER_ADVERT ||
        ndmsg->nduseropt_icmp_code != 0) {
        SLOGD ("ignoring non-Router Advertisement message");
        return NULL;
    }
    char ifname[32];
    if_indextoname(ndmsg->nduseropt_ifindex, ifname);
    
    struct nd_opt_hdr *opt = (struct nd_opt_hdr *) (ndmsg + 1);
    size_t opts_len = ndmsg->nduseropt_opts_len;
    
    while (opts_len >= sizeof (struct nd_opt_hdr)) {
        size_t nd_opt_len = opt->nd_opt_len; // 8 bytes per unit

        if (nd_opt_len == 0 || opts_len < (nd_opt_len << 3))
            break;

        switch (opt->nd_opt_type) {
        case ND_OPT_RDNSS:
            changed = process_nduseropt_rdnss (ifname, opt);
            break;
        }

        opts_len -= opt->nd_opt_len << 3;
        opt = (struct nd_opt_hdr *) ((uint8_t *) opt + (opt->nd_opt_len << 3));
    }

    return changed;
}

static int parse_rtattr(struct rtattr *tb[], int max, struct rtattr *rta, int len)
{
    memset(tb, 0, sizeof(struct rtattr *) * (max + 1));
    while (RTA_OK(rta, len)) {
        if (rta->rta_type <= max)
            tb[rta->rta_type] = rta;
        rta = RTA_NEXT(rta,len);
    }
    if (len)
        SLOGE("!!!Deficit %d, rta_len=%d\n", len, rta->rta_len);
    return 0;
}

static int
process_prefix (struct nlmsghdr *n)
{
    struct prefixmsg *prefix = (struct prefixmsg *)NLMSG_DATA(n);
    int len = n->nlmsg_len;
    struct rtattr * tb[RTA_MAX+1];
    int family = AF_INET6;


    len -= NLMSG_LENGTH(sizeof(*prefix));
    if (len < 0) {
        SLOGE("BUG: wrong nlmsg len %d", len);
        return -1;
    }

    if (prefix->prefix_family != AF_INET6) {
        SLOGE("wrong family %d", prefix->prefix_family);
        return 0;
    }
    if (prefix->prefix_type != ND_OPT_PREFIX_INFORMATION) {
        SLOGE("wrong ND type %d", prefix->prefix_type);
        return 0;
    }

    parse_rtattr(tb, RTA_MAX, RTM_RTA(prefix), len);

    if (tb[PREFIX_ADDRESS]) {
        struct in6_addr *pfx = (struct in6_addr *)RTA_DATA(tb[PREFIX_ADDRESS]);
        char abuf[256];
        inet_ntop(AF_INET6, pfx, abuf, sizeof (abuf));
        char ifname[32];
        if_indextoname(prefix->prefix_ifindex, ifname);
        char prop_name[PROPERTY_KEY_MAX];
        snprintf(prop_name, sizeof(prop_name), "net.%s.prefix-address", ifname);
        property_set(prop_name, abuf);
    }

    return 0;
}

RouteNetlinkHandler::RouteNetlinkHandler(RouteNetlinkManager *nm, int listenerSocket) :
                SocketListener(listenerSocket, false){
    mNm = nm;
}

RouteNetlinkHandler::~RouteNetlinkHandler() {
}

int RouteNetlinkHandler::start() {
    return this->startListener();
}

int RouteNetlinkHandler::stop() {
    return this->stopListener();
}

bool RouteNetlinkHandler::onDataAvailable(SocketClient *cli)
{
    int socket = cli->getSocket();
    int count;
    char cred_msg[CMSG_SPACE(sizeof(struct ucred))];
    struct sockaddr_nl snl;
    struct iovec iov = {mBuffer, sizeof(mBuffer)};
    struct msghdr hdr = {&snl, sizeof(snl), &iov, 1, cred_msg, sizeof(cred_msg), 0};

    if ((count = recvmsg(socket, &hdr, 0)) < 0) {
        SLOGE("recvmsg failed (%s)", strerror(errno));
        return false;
    }
    SLOGD("onDataAvailable(%p): Read %d bytes\n", cli, count);
    
    if (snl.nl_pid != 0) {
        SLOGE("ignoring non-kernel netlink multicast message, nl_pid=%d", snl.nl_pid);
        return false;
    }

    struct cmsghdr * cmsg = CMSG_FIRSTHDR(&hdr);

    if (cmsg == NULL || cmsg->cmsg_type != SCM_CREDENTIALS) {
        SLOGE("ignoring message with no sender credentials");
        return false;
    }

    struct ucred * cred = (struct ucred *)CMSG_DATA(cmsg);
    if (cred->uid != 0) {
        SLOGE("ignoring message from non-root UID %d", cred->uid);
        return false;
    }
    
    struct nlmsghdr *nlh = (struct nlmsghdr *) mBuffer;
    while (NLMSG_OK(nlh, count)) {
        SLOGD("onDataAvailable(%p): Processing valid message, group=%d, nlmsg_type = %d", cli, snl.nl_groups, nlh->nlmsg_type);
        if (nlh->nlmsg_type == RTM_NEWPREFIX) {
            //process_prefix(nlh);
        } else if (nlh->nlmsg_type == RTM_NEWNDUSEROPT){
            process_nduseropt(nlh);
        }

        nlh = NLMSG_NEXT(nlh, count);
    }
    
    return true;
}


