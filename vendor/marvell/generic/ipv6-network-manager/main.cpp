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

#include <linux/capability.h>
#include <linux/prctl.h>
#include <private/android_filesystem_config.h>
#include <stdlib.h>
#include <errno.h>

#define LOG_TAG "ipv6-network-manager"

#include "cutils/log.h"

#include "RouteNetlinkManager.h"

static void switchUser()
{
    prctl(PR_SET_KEEPCAPS, 1, 0, 0, 0);
    setuid(AID_SYSTEM);

    struct __user_cap_header_struct header;
    struct __user_cap_data_struct cap;
    header.version = _LINUX_CAPABILITY_VERSION;
    header.pid = 0;
    cap.effective = cap.permitted = (1 << CAP_NET_RAW) | (1 << CAP_NET_ADMIN);
    cap.inheritable = 0;
    capset(&header, &cap);
}

int main() {
    RouteNetlinkManager *nm;

    LOGI("ipv6-network-manager 1.0 starting");

    switchUser();

    if (!(nm = RouteNetlinkManager::Instance())) {
        LOGE("Unable to create RouteNetlinkManager");
        exit(1);
    };

    if (nm->start()) {
        LOGE("Unable to start RouteNetlinkManager (%s)", strerror(errno));
        exit(1);
    }

    // Eventually we'll become the monitoring thread
    while(1) {
        sleep(1000);
    }

    LOGI("ipv6-network-manager exiting");
    exit(0);
}

