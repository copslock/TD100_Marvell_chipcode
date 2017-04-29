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

/* mwpamod_mwu.c: Implementation of mwu control interface to mwpamod
 * module
 */

#define UTIL_LOG_TAG "MWU MWPAMOD"
#include "util.h"
#include "mwpamod_mwu.h"
#include "mwu_log.h"

#include "module.h"

#define MWPAMOD_MAX_INT_STR sizeof("2147483647")
#define MWPAMOD_MAX_CMD 128
#define MWPAMOD_MAX_EVENT 32
#define MWPAMOD_STATUS_RESPONSE 20
#define MWPAMOD_MOD_NAME "mwpamod"
#define MWPAMOD_MOD_KV MWU_KV("module", MWPAMOD_MOD_NAME)
#define MWPAMOD_MOD_KV_SZ MWU_KV_SZ("module", MWPAMOD_MOD_NAME)
#define MWPAMOD_EVENT_KV(ev) "event=" ev "\n"
#define MWPAMOD_EVENT_KV_SZ(ev) sizeof("event=") + sizeof(ev) + 1
#define MWPAMOD_IFACE_KV(iface) "iface=" iface "\n"
#define MWPAMOD_IFACE_KV_SZ sizeof("iface=") + 10  + 1

/* we currently only support one module instance at a time.  And we need to
 * know if it's an AP or an STA so we only deinit the one that we init'd.  We
 * use the private member of the module for this purpose.  Later, we may want
 * to make the cbpriv a proper struct with other important state tracking info
 * too.
 */
static struct module mwpamod_mod;
#define AP 1
#define STA 2


#define MWPAMOD_AP_ASSOC_STA_FMT                                         \
    MWU_KV_FMT_MAC("mac")                                  \
    MWU_KV_FMT_DEC32("rssi")                                   \
    MWU_KV_FMT_DEC32("powersave")

#define MWPAMOD_AP_ASSOC_STA_SZ                                           \
    (MWU_KV_SZ_MAC("mac") +                                 \
     MWU_KV_SZ_STR("rssi", 5) +                                  \
     MWU_KV_SZ_DEC32("powersave"))

#define MWPAMOD_CMD_HDR_SCAN                                      \
    MWPAMOD_MOD_KV                                                \
    MWU_KV_SCAN_STR("iface", IFNAMSIZ)                            \
    MWU_KV_SCAN_STR("cmd", MWPAMOD_MAX_CMD)

#define MWPAMOD_CMD_INIT_SCAN                                       \
    MWPAMOD_CMD_HDR_SCAN

#define MWPAMOD_NETINFO_SCAN                    \
    MWU_KV_ONE_LINE_STR("ssid", MWPAMOD_MAX_SSID)   \
    MWU_KV_SCAN_STR("key", MWPAMOD_MAX_KEY)

#define MWPAMOD_CMD_AP_INIT_SCAN                \
    MWPAMOD_CMD_HDR_SCAN                        \
    MWPAMOD_NETINFO_SCAN

#define MWPAMOD_EVENT_HDR_FMT                    \
    MWPAMOD_MOD_KV                               \
    MWU_KV_FMT_STR("iface", IFNAMSIZ)            \
    MWU_KV_FMT_STR("event", MWPAMOD_MAX_EVENT)

#define MWPAMOD_EVENT_HDR_SZ                     \
    (MWPAMOD_MOD_KV_SZ +                         \
     MWU_KV_SZ_STR("iface", IFNAMSIZ) +          \
     MWU_KV_SZ_STR("event", MWPAMOD_MAX_EVENT))

#define MWPAMOD_AP_CONNECT_EVENT_FMT \
    MWPAMOD_EVENT_HDR_FMT            \
    MWU_KV_FMT_MAC("mac")            \
    MWU_KV_FMT_DEC32("status")

#define MWPAMOD_AP_CONNECT_EVENT_SZ       \
    (MWPAMOD_EVENT_HDR_SZ +               \
     MWU_KV_SZ_MAC("mac") +               \
     MWU_KV_SZ_DEC32("status"))

#define MWPAMOD_AP_DISCONNECT_EVENT_FMT \
    MWPAMOD_EVENT_HDR_FMT               \
    MWU_KV_FMT_MAC("mac")

#define MWPAMOD_AP_DISCONNECT_EVENT_SZ \
    (MWPAMOD_EVENT_HDR_SZ +            \
     MWU_KV_SZ_MAC("mac"))

#define MWPAMOD_CMD_STA_CONNECT_SCAN            \
    MWPAMOD_CMD_HDR_SCAN                        \
    MWPAMOD_NETINFO_SCAN

#define MWPAMOD_CMD_AP_DEAUTH_SCAN    \
    MWPAMOD_CMD_HDR_SCAN              \
    MWU_KV_FMT_MAC("interface_address")

/*STA EVENTS */

#define MWPAMOD_STA_CONNECT_EVENT_FMT \
    MWPAMOD_EVENT_HDR_FMT            \
    MWU_KV_FMT_DEC32("status")

#define MWPAMOD_STA_CONNECT_EVENT_SZ       \
    (MWPAMOD_EVENT_HDR_SZ +               \
     MWU_KV_SZ_DEC32("status"))

#define MWPAMOD_STA_LINK_LOST_EVENT_FMT \
    MWPAMOD_EVENT_HDR_FMT            \
    MWU_KV_FMT_DEC32("status")

#define MWPAMOD_STA_LINK_LOST_EVENT_SZ       \
    (MWPAMOD_EVENT_HDR_SZ +               \
     MWU_KV_SZ_DEC32("status"))

/* convert an asscoated sta to a string as expected by the mwu mwpamod interface.  dest
 * must be at least MWPAMOD_AP_ASSOC_STA_SZ bytes long.
 */
static unsigned int associated_sta_to_str(char *dest, struct mwpamod_ap_sta_info *sta)
{
    unsigned int len;
    int rssi = 0;

    /* convert rssi to dBm */
    rssi = (int) sta->rssi;
    if (rssi > 0x7F)
        rssi = -(256 - rssi);

    len = snprintf(dest, MWPAMOD_AP_ASSOC_STA_SZ + 1,
                   MWPAMOD_AP_ASSOC_STA_FMT,
                   UTIL_MAC2STR(sta->mac_address),
                   rssi,
                   (sta->power_mfg_status == 0) ? 0 : 1);
    if (len >= MWPAMOD_AP_ASSOC_STA_SZ) {
        /* This is unexpected */
        WARN("Truncated an associated sta string!");
        return MWPAMOD_AP_ASSOC_STA_SZ;
    }
    return len;
}


enum mwu_error mwpamod_handle_mwu(struct mwu_msg *msg, struct mwu_msg **resp)
{
    int ret, status;
    char module[sizeof(MWPAMOD_MOD_NAME)];
    char cmd[MWPAMOD_MAX_CMD];
    char iface[IFNAMSIZ + 1];

    *resp = NULL;

    INFO("*********************************\n");
    INFO("MWPAMOD module receives message:\n");
    INFO("\n%s", msg->data);
    INFO("*********************************\n\n");
    /* parse the message */
    ret = sscanf(msg->data, MWPAMOD_CMD_HDR_SCAN, iface, cmd);
    if (ret != 2) {
        ERR("Failed to parse cmd message: ret = %d.\n", ret);
        ALLOC_STATUS_MSG_OR_FAIL(*resp, MWPAMOD_ERR_INVAL);
        return MWU_ERR_SUCCESS;
    }

    if (strcmp(cmd, "sta_deinit") == 0 || strcmp(cmd, "ap_deinit") == 0) {
        if (strcmp(iface, mwpamod_mod.iface) != 0) {
            /* sometimes, deinit is called automatically when the state of the
             * module is unknown.  Don't fail in this case, just return
             * silently.
             */
            return MWU_ERR_SUCCESS;
        }
    }

    /*
    if (strcmp(cmd, "sta_init") != 0 && strcmp(cmd, "ap_init") != 0) {
        if (strcmp(iface, mwpamod_mod.iface) != 0) {
            ERR("Got command for uninitialized iface %s\n", iface);
            return MWU_ERR_INVAL;
        }
    }*/

    /* process command */
    if (strcmp(cmd, "sta_init") == 0) {


        /* currently, we only support one module instance at a time
        if (mwpamod_mod.iface[0] != 0) {
            ERR("Module mwpamod already initialized\n");
            ALLOC_STATUS_MSG_OR_FAIL(*resp, MWPAMOD_ERR_BUSY);
            return MWU_ERR_SUCCESS;
        } */

        ret = sscanf(msg->data, MWPAMOD_CMD_INIT_SCAN, iface, cmd);
        if (ret != 2) {
            ERR("init command was missing some key-value pairs.\n");
            ALLOC_STATUS_MSG_OR_FAIL(*resp, MWPAMOD_ERR_INVAL);
            return MWU_ERR_SUCCESS;
        }

        /* prepare the module struct */
        strncpy(mwpamod_mod.iface, iface, IFNAMSIZ + 1);
        mwpamod_mod.cb = mwpamod_mwu_event_cb;
        mwpamod_mod.cbpriv = (void *)STA;

        /* Call init with NULL confid param to start with default */
        status = mwpamod_sta_init(&mwpamod_mod, NULL);
        if (status != MWPAMOD_ERR_SUCCESS)
            mwpamod_mod.iface[0] = 0;
        ALLOC_STATUS_MSG_OR_FAIL(*resp, status);

    } else if (strcmp(cmd, "sta_deinit") == 0) {
        if ((int)mwpamod_mod.cbpriv != STA)
            return MWU_ERR_SUCCESS;
        mwpamod_sta_deinit(&mwpamod_mod);
        mwpamod_mod.iface[0] = 0;

    } else if (strcmp(cmd, "sta_connect") == 0) {
        struct mwpamod_net_info net_info;
        ret = sscanf(msg->data, MWPAMOD_CMD_STA_CONNECT_SCAN,
                     iface, cmd, net_info.ssid, net_info.key);
        if (ret != 4) {
            ERR("sta_connect command was missing some key-value pairs (ret=%d)"
                ".\n", ret);
            ALLOC_STATUS_MSG_OR_FAIL(*resp, MWPAMOD_ERR_INVAL);
            return MWU_ERR_SUCCESS;
        }

        net_info.key_len = strlen(net_info.key);

        status = mwpamod_sta_connect(&mwpamod_mod, &net_info);
        ALLOC_STATUS_MSG_OR_FAIL(*resp, status);

    } else if (strcmp(cmd, "sta_disconnect") == 0) {
        mwpamod_sta_disconnect(&mwpamod_mod);

    } else if (strcmp(cmd, "ap_init") == 0) {

        struct mwpamod_ap_config cfg;

        /* currently, we only support one module instance at a time */
        if (mwpamod_mod.iface[0] != 0) {
            ERR("Module already initialized\n");
            ALLOC_STATUS_MSG_OR_FAIL(*resp, MWPAMOD_ERR_BUSY);
            return MWU_ERR_SUCCESS;
        }

        ret = sscanf(msg->data, MWPAMOD_CMD_AP_INIT_SCAN, iface, cmd,
                     cfg.net_info.ssid, cfg.net_info.key);
        if (ret != 4) {
            ERR("ap_init command was missing some key-value pairs (ret=%d).\n",
                ret);
            ALLOC_STATUS_MSG_OR_FAIL(*resp, MWPAMOD_ERR_INVAL);
            return MWU_ERR_SUCCESS;
        }
        cfg.net_info.key_len = strlen(cfg.net_info.key);

        strncpy(mwpamod_mod.iface, iface, IFNAMSIZ + 1);
        mwpamod_mod.cb = mwpamod_mwu_event_cb;
        mwpamod_mod.cbpriv = (void *)AP;

        status = mwpamod_ap_init(&mwpamod_mod, &cfg);
        if (status != MWPAMOD_ERR_SUCCESS)
            mwpamod_mod.iface[0] = 0;
        ALLOC_STATUS_MSG_OR_FAIL(*resp, status);

    } else if (strcmp(cmd, "ap_deinit") == 0) {
        if ((int)mwpamod_mod.cbpriv != AP)
            return MWU_ERR_SUCCESS;
        mwpamod_ap_deinit(&mwpamod_mod);
        mwpamod_mod.iface[0] = 0;

    } else if (strcmp(cmd, "ap_start") == 0) {
        status = mwpamod_ap_start(&mwpamod_mod);
        ALLOC_STATUS_MSG_OR_FAIL(*resp, status);

    } else if (strcmp(cmd, "ap_deauth") == 0) {
        unsigned char sta_mac[ETH_ALEN];
        memset(sta_mac, 0, ETH_ALEN);

        if ((int)mwpamod_mod.cbpriv != AP)
            return MWU_ERR_SUCCESS;

        ret = sscanf(msg->data, MWPAMOD_CMD_AP_DEAUTH_SCAN, iface, cmd,
                     UTIL_MAC2SCAN(sta_mac));
        if (ret != 8) {
            ERR("ap_deauth command was mssing some key-value pairs (ret=%d).\n",
                ret);
        }
        INFO("Deauthing STA " UTIL_MACSTR "\n", UTIL_MAC2STR(sta_mac));
        status = mwpamod_ap_deauth(&mwpamod_mod, sta_mac);
        ALLOC_STATUS_MSG_OR_FAIL(*resp, status);

    } else if (strcmp(cmd, "ap_sta_list") == 0) {
        if ((int)mwpamod_mod.cbpriv != AP) {
            ERR("Command valid only in AP context.");
            return MWPAMOD_ERR_UNSUPPORTED;
        }

        /* Get associated sta_list here.
         * We actually get the ap_config here, sta_list is a part of ap_config.
         * Note that mwpamod_ap_get_info is a partial implementation at
         * present, future requirements for querying ap configuration/information should only be
         * routed through mwpamod_get_ap_info API call. */
        struct mwpamod_ap_info ap_info;
        struct mwpamod_ap_config ap_config;
        int count = 0, max_len = 0, len = 0, offset = 0, i = 0;
        status = mwpamod_ap_get_info(&mwpamod_mod, &ap_config, &ap_info);
        if (status != MWPAMOD_ERR_SUCCESS) {
            ALLOC_STATUS_MSG_OR_FAIL(*resp, status);
        }

        count = ap_info.sta_list.sta_count;

        /* return early if no peer is connected */
        if (!count)
            return MWPAMOD_ERR_SUCCESS;

        max_len = count * MWPAMOD_AP_ASSOC_STA_SZ;

        *resp = malloc(sizeof(struct mwu_msg) + max_len + 1);
        if (*resp == NULL) {
            ERR("Failed to allocate ap_sta_list response message");
            ret = MWU_ERR_NOMEM;
            goto fail;
        }

        for(i = 0; i < count ; i++) {
            len = associated_sta_to_str(&(*resp)->data[offset], &ap_info.sta_list.sta_info[i]);
            offset += len;
        }
        (*resp)->len = offset + 1;

    } else {
        ERR("Got unknown command: %s\n", cmd);
        ALLOC_MSG_OR_FAIL(*resp, MWPAMOD_STATUS_RESPONSE, "status=%d\n",
                MWPAMOD_ERR_UNSUPPORTED);
    }
    return MWU_ERR_SUCCESS;

fail:
    if (*resp != NULL)
        free(*resp);
    return ret;
}

static struct mwu_module mwpamod_mwu_mod = {
    .name = MWPAMOD_MOD_NAME,
    .msg_cb = mwpamod_handle_mwu,
    .msg_free = NULL,
};

enum mwu_error mwpamod_mwu_launch(void)
{
    int ret;

    if (mwpamod_mod.iface[0] != 0)
        return MWU_ERR_BUSY;

    return mwu_internal_register_module(&mwpamod_mwu_mod);
}

void mwpamod_mwu_halt(void)
{
    mwu_internal_unregister_module(&mwpamod_mwu_mod);
    mwpamod_mod.iface[0] = 0;
}


/* When the events be asynchronous */
void mwpamod_mwu_event_cb(struct event *event, void *priv)
{

    int type, ret;
    struct mwu_msg *msg_event = NULL;
    char iface[IFNAMSIZ + 1];
    if (!event) {
        ERR("mwpamod event pointer is NULL\n");
        return;
    }

    type = event->type;
    switch (type) {
    case MWPAMOD_EVENT_STA_CONNECT:
        INFO("Event MWPAMOD_EVENT_STA_CONNECT");
        ALLOC_MSG_OR_FAIL(msg_event, MWPAMOD_STA_CONNECT_EVENT_SZ + 1,
                MWPAMOD_STA_CONNECT_EVENT_FMT,
                event->iface, "sta_connect", event->status);
        break;

    case MWPAMOD_EVENT_STA_LINK_LOST:
        INFO("Event MWPAMOD_EVENT_STA_LINK_LOST");
        ALLOC_MSG_OR_FAIL(msg_event, MWPAMOD_STA_LINK_LOST_EVENT_SZ + 1,
                MWPAMOD_STA_LINK_LOST_EVENT_FMT,
                event->iface, "link_lost", event->status);
        break;

    case MWPAMOD_EVENT_AP_CONNECT:
        ALLOC_MSG_OR_FAIL(msg_event, MWPAMOD_AP_CONNECT_EVENT_SZ + 1,
                          MWPAMOD_AP_CONNECT_EVENT_FMT,
                          event->iface,
                          "ap_connect",
                          UTIL_MAC2STR(event->val), event->status);
        break;

    case MWPAMOD_EVENT_AP_DISCONNECT:
        ALLOC_MSG_OR_FAIL(msg_event, MWPAMOD_AP_DISCONNECT_EVENT_SZ + 1,
                          MWPAMOD_AP_DISCONNECT_EVENT_FMT,
                          event->iface,
                          "ap_disconnect",
                          UTIL_MAC2STR(event->val));
        break;

    default:
        ERR("Event type %d is not supported.\n", type);
        break;
    }

    if (msg_event)
        mwu_internal_send(msg_event);

fail:
    if (msg_event != NULL)
        free(msg_event);

    return;
}
