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
#include <errno.h>

#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>

#include <linux/netlink.h>
#include <linux/rtnetlink.h>

#define LOG_TAG "ipv6-network-manager"

#include <cutils/log.h>

#include "RouteNetlinkManager.h"
#include "RouteNetlinkHandler.h"

static int netlink_add_membership(int sock, int group)
{
    int err = setsockopt(sock, SOL_NETLINK, NETLINK_ADD_MEMBERSHIP, &group, sizeof(group));
    if (err < 0) {
        SLOGE("Unable to ADD_MEMBERSHIP for group(%d): %s", group, strerror(errno));
    }
    return err;
}

#define TRY_ADD_MEMBERSHIP(sock, group) \
    do { if (netlink_add_membership(sock, group) < 0) return -1;} while(0)

RouteNetlinkManager *RouteNetlinkManager::sInstance = NULL;

RouteNetlinkManager *RouteNetlinkManager::Instance() {
    if (!sInstance)
        sInstance = new RouteNetlinkManager();
    return sInstance;
}

RouteNetlinkManager::RouteNetlinkManager() {
}

RouteNetlinkManager::~RouteNetlinkManager() {
}

int RouteNetlinkManager::start() {
    struct sockaddr_nl nladdr;
    int sz = 64 * 1024;
    int on = 1;

    memset(&nladdr, 0, sizeof(nladdr));
    nladdr.nl_family = AF_NETLINK;
    nladdr.nl_pid = getpid();
    nladdr.nl_groups = 0;

    if ((mSock = socket(PF_NETLINK,
                        SOCK_RAW,NETLINK_ROUTE)) < 0) {
        LOGE("Unable to create route socket: %s", strerror(errno));
        return -1;
    }

    if (setsockopt(mSock, SOL_SOCKET, SO_RCVBUFFORCE, &sz, sizeof(sz)) < 0) {
        LOGE("Unable to set route socket SO_RCVBUFFORCE option: %s", strerror(errno));
        return -1;
    }

    if (setsockopt(mSock, SOL_SOCKET, SO_PASSCRED, &on, sizeof(on)) < 0) {
        SLOGE("Unable to set route socket SO_PASSCRED option: %s", strerror(errno));
        return -1;
    }
  
    if (bind(mSock, (struct sockaddr *) &nladdr, sizeof(nladdr)) < 0) {
        LOGE("Unable to bind route socket: %s", strerror(errno));
        return -1;
    }
    
    TRY_ADD_MEMBERSHIP(mSock, RTNLGRP_ND_USEROPT);
    //TRY_ADD_MEMBERSHIP(mSock, RTNLGRP_IPV6_IFADDR);
    //TRY_ADD_MEMBERSHIP(mSock, RTNLGRP_IPV6_PREFIX);
    //TRY_ADD_MEMBERSHIP(mSock, RTNLGRP_IPV4_IFADDR);

    mHandler = new RouteNetlinkHandler(this, mSock);
    if (mHandler->start()) {
        LOGE("Unable to start RouteNetlinkHandler: %s", strerror(errno));
        return -1;
    }
    return 0;
}

int RouteNetlinkManager::stop() {
    if (mHandler->stop()) {
        LOGE("Unable to stop RouteNetlinkHandler: %s", strerror(errno));
        return -1;
    }
    delete mHandler;
    mHandler = NULL;

    close(mSock);
    mSock = -1;

    return 0;
}
