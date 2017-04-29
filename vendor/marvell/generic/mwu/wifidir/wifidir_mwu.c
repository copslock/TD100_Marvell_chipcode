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

/* wifidir_mwu.c: implementation of mwu control interface to the wifidirect
 * module.
 */
#define UTIL_LOG_TAG "MWU WIFIDIR"
#include "util.h"
#include "wifidir_mwu.h"
#include "module.h"
#include "mwu_log.h"
#include "wps_msg.h"  // for a2hex

/* we currently only support one module instance at a time */
static struct module wifidir_mod;

/* The max event and command names are actually less than this.  But leave some
 * room to grow
 */
#define WIFIDIR_MAX_CMD 16
#define WIFIDIR_MAX_EVENT_NAME 24
#define WIFIDIR_MOD_NAME "wifidirect"
#define WIFIDIR_MOD_KV MWU_KV("module", WIFIDIR_MOD_NAME)

/* note: max size of is_go is 5 for "false" */
#define WIFIDIR_PEER_FMT                                         \
    MWU_KV_FMT_MAC("device_address")                                  \
    MWU_KV_FMT_STR("device_name", MAX_DEVICE_NAME_LEN)              \
    MWU_KV_FMT_STR("is_go", 5)                                   \
    MWU_KV_FMT_STR("go_ssid", MAX_SSID_LEN)                  \
    MWU_KV_FMT_HEX16("methods")                                  \
    MWU_KV_FMT_MAC("interface_address")

#define WIFIDIR_PEER_SZ                                           \
    (MWU_KV_SZ_MAC("device_address") +                                 \
     MWU_KV_SZ_STR("device_name", MAX_DEVICE_NAME_LEN) +             \
     MWU_KV_SZ_STR("is_go", 5) +                                  \
     MWU_KV_SZ_STR("go_ssid", MAX_SSID_LEN) +                 \
     MWU_KV_SZ_HEX16("methods") +                                 \
     MWU_KV_SZ_MAC("interface_address"))

#define WIFIDIF_DUMP_PEER_CMD_FMT                       \
    WIFIDIR_MOD_KV                                      \
    MWU_KV_SCAN_STR("iface", IFNAMSIZ)                  \
    MWU_KV("cmd", "dump_peers")                         \
    MWU_KV_FMT_MAC("device_address")

/* convert a peer to a string as expected by the mwu wifidir interface.  dest
 * must be at least WIFIDIR_PEER_STRING_SIZE bytes long.
 */
static unsigned int peer_to_string(char *dest, struct wifidir_peer *peer)
{
    unsigned int len;

    len = snprintf(dest, WIFIDIR_PEER_SZ + 1,
                   WIFIDIR_PEER_FMT,
                   UTIL_MAC2STR(peer->device_address),
                   peer->device_name,
                   peer->group_capability & WIFIDIR_GROUP_CAP_GROUP_OWNER ? \
                   "true" : "false",
                   peer->go_ssid,
                   peer->methods,
                   UTIL_MAC2STR(peer->interface_address));
    if (len >= WIFIDIR_PEER_SZ) {
        /* This is unexpected */
        WARN("Truncated a peer string!");
        return WIFIDIR_PEER_SZ;
    }
    return len;
}

#define WIFIDIR_PEER_EVENT_HDR_FMT                        \
    WIFIDIR_MOD_KV                                        \
    MWU_KV_FMT_STR("iface", IFNAMSIZ)                     \
    MWU_KV_FMT_STR("event", WIFIDIR_MAX_EVENT_NAME)

#define WIFIDIR_PEER_EVENT_HDR_SZ                         \
    (sizeof(WIFIDIR_MOD_KV) +                             \
     MWU_KV_SZ_STR("iface", IFNAMSIZ) +                   \
     MWU_KV_SZ_STR("event", WIFIDIR_MAX_EVENT_NAME))

/* Many of our mwu commands return a peer object.
 */
static struct mwu_msg *prepare_peer_event(char *event_name,
                                          struct wifidir_peer *peer)
{
    struct mwu_msg *msg;
    int offset;

    msg = malloc(WIFIDIR_PEER_EVENT_HDR_SZ + WIFIDIR_PEER_SZ + 1);
    if (msg == NULL)
        return NULL;

    /* to make use of our peer_to_string function, we populate manually */
    offset = sprintf(msg->data, WIFIDIR_PEER_EVENT_HDR_FMT,
                     wifidir_mod.iface, event_name);
    msg->len = offset + peer_to_string(&msg->data[offset], peer) + 1;
    return msg;
}

#define WIFIDIR_PDI_FMT                                 \
    MWU_KV_FMT_MAC("device_address")                         \
    MWU_KV_FMT_HEX16("methods")

#define WIFIDIR_PDI_SZ                                  \
    MWU_KV_SZ_MAC("device_address") +                        \
    MWU_KV_SZ_HEX16("methods")

#define WIFIDIR_PD_EVENT_FMT                            \
    WIFIDIR_MOD_KV                                      \
    MWU_KV_FMT_STR("iface", IFNAMSIZ)                   \
    MWU_KV_FMT_STR("event", WIFIDIR_MAX_EVENT_NAME)     \
    WIFIDIR_PDI_FMT

#define WIFIDIR_PD_EVENT_SZ                             \
    (sizeof(WIFIDIR_MOD_KV) +                           \
     MWU_KV_SZ_STR("iface", IFNAMSIZ) +                 \
     MWU_KV_SZ_STR("event", WIFIDIR_MAX_EVENT_NAME) +   \
     WIFIDIR_PDI_SZ)

#define WIFIDIR_PD_CMD_FMT(cmd)                         \
    WIFIDIR_MOD_KV                                      \
    MWU_KV_SCAN_STR("iface", IFNAMSIZ)                  \
    MWU_KV("cmd", cmd)                                  \
    WIFIDIR_PDI_FMT

static struct mwu_msg *prepare_pdi_event(char *event_name,
                                         struct wifidir_pd_info *pdi)
{
    struct mwu_msg *msg;
    int ret;

    ALLOC_MSG_OR_FAIL(msg, WIFIDIR_PD_EVENT_SZ + 1,
                      WIFIDIR_PD_EVENT_FMT,
                      wifidir_mod.iface, event_name,
                      UTIL_MAC2STR(pdi->device_address),
                      pdi->methods);
    return msg;

fail:
    return NULL;
}

#define WIFIDIR_NEGRES_FMT                              \
    MWU_KV_FMT_STATUS                                   \
    MWU_KV_FMT_MAC("device_address")                         \
    MWU_KV_FMT_STR("is_go", 5)                          \
    MWU_KV_FMT_STR("go_ssid", MAX_SSID_LEN)

#define WIFIDIR_NEGRES_SZ                               \
    (MWU_KV_SZ_STATUS +                                 \
     MWU_KV_SZ_MAC("device_address") +                       \
     MWU_KV_SZ_STR("is_go", 5) +                        \
     MWU_KV_SZ_STR("go_ssid", MAX_SSID_LEN))

#define WIFIDIR_NEGRES_EVENT_FMT                        \
    WIFIDIR_MOD_KV                                      \
    MWU_KV_FMT_STR("iface", IFNAMSIZ)                   \
    MWU_KV("event", "neg_result")                       \
    WIFIDIR_NEGRES_FMT

#define WIFIDIR_NEGRES_EVENT_SZ                         \
    (sizeof(WIFIDIR_MOD_KV) +                           \
     MWU_KV_SZ_STR("iface", IFNAMSIZ) +                 \
     MWU_KV_SZ("event", "neg_result") +                 \
     WIFIDIR_NEGRES_SZ)

struct mwu_msg *prepare_neg_result_event(struct wifidir_neg_result *neg_result)
{
    struct mwu_msg *msg;
    int ret;

    ALLOC_MSG_OR_FAIL(msg, WIFIDIR_NEGRES_EVENT_SZ + 1,
                      WIFIDIR_NEGRES_EVENT_FMT,
                      wifidir_mod.iface,
                      neg_result->status,
                      UTIL_MAC2STR(neg_result->device_address),
                      neg_result->is_go ? "true" : "false",
                      neg_result->go_ssid);
    return msg;

fail:
    return NULL;
}


#define WIFIDIR_PEER_UNFOUND_EVENT_FMT                  \
    WIFIDIR_MOD_KV                                      \
    MWU_KV_FMT_STR("iface", IFNAMSIZ)                   \
    MWU_KV("event", "peer_unfound")                     \
    MWU_KV_FMT_MAC("device_address")

#define WIFIDIR_PEER_UNFOUND_EVENT_SZ                   \
    (sizeof(WIFIDIR_MOD_KV) +                           \
     MWU_KV_SZ_STR("iface", IFNAMSIZ) +                 \
     MWU_KV_SZ("event", "peer_unfound") +               \
     MWU_KV_SZ_MAC("device_address"))

struct mwu_msg *prepare_peer_unfound_event(unsigned char *peer_device_address)
{
    struct mwu_msg *msg;
    int ret;

    ALLOC_MSG_OR_FAIL(msg, WIFIDIR_PEER_UNFOUND_EVENT_SZ + 1,
                      WIFIDIR_PEER_UNFOUND_EVENT_FMT,
                      wifidir_mod.iface,
                      UTIL_MAC2STR(peer_device_address));
    return msg;

fail:
    return NULL;
}




#define WIFIDIR_CMD_HDR_SCAN                                        \
    WIFIDIR_MOD_KV                                                  \
    MWU_KV_SCAN_STR("iface", IFNAMSIZ)                              \
    MWU_KV_SCAN_STR("cmd", WIFIDIR_MAX_CMD)

#define WIFIDIR_CMD_INIT_SCAN                                       \
    WIFIDIR_CMD_HDR_SCAN                                            \
    MWU_KV_SCAN_STR("name", MAX_DEVICE_NAME_LEN)                    \
    MWU_KV_FMT_DEC32("intent")                                      \
    MWU_KV_FMT_DEC32("op_chan")                                     \
    MWU_KV_FMT_DEC32("listen_chan")                                 \
    MWU_KV_SCAN_STR("methods", MAX_BITMAP_LEN)

#define WIFIDIR_CMD_START_GROUP_SCAN                                       \
    WIFIDIR_CMD_HDR_SCAN                                            \
    MWU_KV_ONE_LINE_STR("ssid", MAX_SSID_LEN)

enum mwu_error wifidir_handle_mwu(struct mwu_msg *msg, struct mwu_msg **resp)
{
    int ret, status;
    char iface[IFNAMSIZ + 1];
    char cmd[WIFIDIR_MAX_CMD];

    *resp = NULL;

    INFO("*********************************\n");
    INFO("WIFIDIR module receives message:\n");
    INFO("\n%s", msg->data);
    INFO("*********************************\n\n");
    /* parse the message */
    ret = sscanf(msg->data, WIFIDIR_CMD_HDR_SCAN, iface, cmd);
    if (ret != 2) {
        ERR("Failed to parse cmd message: ret = %d.\n", ret);
        ALLOC_STATUS_MSG_OR_FAIL(*resp, WIFIDIR_ERR_INVAL);
        return MWU_ERR_SUCCESS;
    }

    /* save some code by checking the interface early */

    if (strcmp(cmd, "deinit") == 0 &&
        strcmp(iface, wifidir_mod.iface) != 0) {
        /* sometimes, deinit is called automatically when the state of the
         * module is unknown.  Don't fail in this case, just return silently.
         */
        return MWU_ERR_SUCCESS;
    }

    if (strcmp(cmd, "init") != 0 &&
        strcmp(iface, wifidir_mod.iface) != 0) {
            ERR("Got command for uninitialized iface %s\n", iface);
            return MWU_ERR_INVAL;
    }

    /* process command */
    if (strcmp(cmd, "init") == 0) {
        char config_methods_str[MAX_BITMAP_LEN + 1];
        struct wifidir_cfg cfg;
        memset(config_methods_str, 0, MAX_BITMAP_LEN + 1);
        memset(&cfg, 0, sizeof(cfg));

        /* currently, we only support one module instance at a time */
        if (wifidir_mod.iface[0] != 0) {
            ERR("Module already initialized\n");
            ALLOC_STATUS_MSG_OR_FAIL(*resp, WIFIDIR_ERR_BUSY);
            return MWU_ERR_SUCCESS;
        }

        ret = sscanf(msg->data, WIFIDIR_CMD_INIT_SCAN, iface, cmd,
                     cfg.name, &cfg.intent, &cfg.operating_chan,
                     &cfg.listen_chan, config_methods_str);
        cfg.config_methods = (unsigned short) a2hex(config_methods_str)
                             & CFG_METHODS_MASK;
        if (ret != 7) {
            ERR("init command was missing some key-value pairs.\n");
            ALLOC_STATUS_MSG_OR_FAIL(*resp, WIFIDIR_ERR_INVAL);
            return MWU_ERR_SUCCESS;
        }

        /* prepare the module struct */
        strcpy(wifidir_mod.iface, iface);
        wifidir_mod.cb = wifidir_mwu_event_cb;
        wifidir_mod.cbpriv = &wifidir_mod;

        status = wifidir_init(&wifidir_mod, &cfg);
        if (status != WIFIDIR_ERR_SUCCESS)
            wifidir_mod.iface[0] = 0;
        ALLOC_STATUS_MSG_OR_FAIL(*resp, status);

    } else if (strcmp(cmd, "deinit") == 0) {
        wifidir_deinit(&wifidir_mod);
        wifidir_mod.iface[0] = 0;

    } else if (strcmp(cmd, "dump_peers") == 0) {
        struct wifidir_peer *cursor = NULL;
        unsigned char device_address[ETH_ALEN];
        unsigned int count = 0, max_len, len = 0, offset = 0;
        const struct peer_list *peers = wifidir_get_peers(&wifidir_mod);

        INFO("Got dump_peers command");
        if (peers == NULL) {
            /* no peers.  quit early. */
            return MWU_ERR_SUCCESS;
        }

        ret = sscanf(msg->data, WIFIDIF_DUMP_PEER_CMD_FMT, iface, UTIL_MAC2SCAN(device_address));
        if ((memcmp(device_address, "\x00\x00\x00\x00\x00\x00", ETH_ALEN)) && ret == 7) {
            struct wifidir_peer *found = NULL;
            SLIST_FOREACH(cursor, peers, list_item) {
                if (!memcmp(cursor->device_address, device_address, ETH_ALEN))
                    found = cursor;
            }
            /* No such peer in list, quit*/
            if (!found)
                return MWU_ERR_SUCCESS;

            *resp = malloc(sizeof(struct mwu_msg) + WIFIDIR_PEER_SZ + 1);
            if (*resp == NULL) {
                ERR("Failed to allocate dump_peers resp for specific peer");
                ret = MWU_ERR_NOMEM;
                goto fail;
            }
            len = peer_to_string((*resp)->data, found);
            /* never forget the the evil \0 */
            (*resp)->len = len + 1;
        } else {
            ERR("==Ret : %d", ret);
            /* we can't use the ALLOC_MSG_OR_FAIL macro here because we populate
             * our response in a list.
             */
            SLIST_COUNT(cursor, peers, list_item, count);
            max_len = count * WIFIDIR_PEER_SZ;
            *resp = malloc(sizeof(struct mwu_msg) + max_len + 1);
            if (*resp == NULL) {
                ERR("Failed to allocate dump_peers response message");
                ret = MWU_ERR_NOMEM;
                goto fail;
            }

            SLIST_FOREACH(cursor, peers, list_item) {
                len = peer_to_string(&(*resp)->data[offset], cursor);
                offset += len;
            }
            (*resp)->len = offset + 1;
        }
        wifidir_release_peers(&wifidir_mod, peers);
    } else if (strcmp(cmd, "pd_req") == 0) {
        struct wifidir_pd_info pdi;

        ret = sscanf(msg->data, WIFIDIR_PD_CMD_FMT("pd_req"),
                     iface,
                     UTIL_MAC2SCAN(pdi.device_address),
                     &pdi.methods);
        if (ret != 8) {
            ERR("pd_req command was missing some key-value pairs (%d).\n", ret);
            ALLOC_STATUS_MSG_OR_FAIL(*resp, WIFIDIR_ERR_INVAL);
            return MWU_ERR_SUCCESS;
        }
        status = wifidir_pd_request(&wifidir_mod, &pdi);
        ALLOC_STATUS_MSG_OR_FAIL(*resp, status);

    } else if (strcmp(cmd, "start_find") == 0) {
        status = wifidir_start_find(&wifidir_mod, NULL);
        ALLOC_STATUS_MSG_OR_FAIL(*resp, status);
        INFO("Find started");

    } else if (strcmp(cmd, "stop_find") == 0) {
        INFO("Got stop_find command");
        wifidir_stop_find(&wifidir_mod);

    } else if (strcmp(cmd, "negotiate_group") == 0) {
        struct wifidir_pd_info pdi;

        ret = sscanf(msg->data, WIFIDIR_PD_CMD_FMT("negotiate_group"),
                     iface,
                     UTIL_MAC2SCAN(pdi.device_address),
                     &pdi.methods);
        if (ret != 8) {
            ERR("negotiate_group command was missing some key-value pairs.\n");
            ALLOC_STATUS_MSG_OR_FAIL(*resp, WIFIDIR_ERR_INVAL);
            return MWU_ERR_SUCCESS;
        }
        status = wifidir_negotiate_group(&wifidir_mod, &pdi);
        ALLOC_STATUS_MSG_OR_FAIL(*resp, status);

    } else if (strcmp(cmd, "allow") == 0) {
        struct wifidir_pd_info pdi;

        ret = sscanf(msg->data, WIFIDIR_PD_CMD_FMT("allow"),
                     iface,
                     UTIL_MAC2SCAN(pdi.device_address),
                     &pdi.methods);
        if (ret != 8) {
            ERR("allow command was missing some key-value pairs (%d).\n", ret);
            ALLOC_STATUS_MSG_OR_FAIL(*resp, WIFIDIR_ERR_INVAL);
            return MWU_ERR_SUCCESS;
        }
        status = wifidir_allow(&wifidir_mod, &pdi);
        ALLOC_STATUS_MSG_OR_FAIL(*resp, status);

    } else if (strcmp(cmd, "start_group") == 0) {
        struct wifidir_group_info info;
        ret = sscanf(msg->data, WIFIDIR_CMD_START_GROUP_SCAN,
                 iface, cmd, info.go_ssid);
        if (ret == 2) {
            /* No SSID given. FW should auto generate it - legal*/
            info.go_ssid[0] = 0;
        } else if (ret != 3) {
            ERR("%d: start_group command was missing some key-value pairs.\n", ret);
            ALLOC_STATUS_MSG_OR_FAIL(*resp, WIFIDIR_ERR_INVAL);
            return MWU_ERR_SUCCESS;
        }
        status = wifidir_start_group(&wifidir_mod, &info);
        ALLOC_STATUS_MSG_OR_FAIL(*resp, status);

    } else if (strcmp(cmd, "get_wpspin") == 0) {
        PWPS_INFO pwps_info;
        pwps_info = wps_get_private_info();
        INFO("Got get_wpspin command");

	 *resp = malloc(sizeof(struct mwu_msg) + 9);
        if (*resp == NULL) {
            ERR("Failed to allocate get_wpspin response message");
            ret = MWU_ERR_NOMEM;
            goto fail;
        }
        sprintf((*resp)->data,"%d",get_wps_pin(pwps_info));
	 INFO("get_wpspin cmd resp : %s ",(*resp)->data);
        (*resp)->len = 9;
    }  else {
        ERR("Got unknown command: %s\n", cmd);
        ALLOC_STATUS_MSG_OR_FAIL(*resp, WIFIDIR_ERR_UNSUPPORTED);
    }
    return MWU_ERR_SUCCESS;

fail:
    if (*resp != NULL)
        free(*resp);
    return ret;
}

static struct mwu_module wifidir_mwu_mod = {
    .name = WIFIDIR_MOD_NAME,
    .msg_cb = wifidir_handle_mwu,
    .msg_free = NULL,
};

enum mwu_error wifidir_mwu_launch(void)
{
    int ret;

    if (wifidir_mod.iface[0] != 0)
        return MWU_ERR_BUSY;

    return mwu_internal_register_module(&wifidir_mwu_mod);
}

void wifidir_mwu_halt(void)
{
    mwu_internal_unregister_module(&wifidir_mwu_mod);
    wifidir_mod.iface[0] = 0;
}

void wifidir_mwu_event_cb(struct event *event, void *priv)
{
    int type, ret;
    struct mwu_msg *msg_event = NULL;
    struct module *mod = (struct module *)priv;

    if (!event) {
        ERR("event pointer is NULL\n");
        return;
    }

    type = event->type;
    switch (type) {
    case WIFIDIR_EVENT_PD_REQ:
        msg_event = prepare_pdi_event("pd_req",
                                      (struct wifidir_pd_info *)event->val);
        if (msg_event == NULL)
            ERR("Failed to allocate pd_req event.\n");
        break;

    case WIFIDIR_EVENT_PD_RESP:
        msg_event = prepare_pdi_event("pd_resp",
                                      (struct wifidir_pd_info *)event->val);
        if (msg_event == NULL)
            ERR("Failed to allocate pd_resp event.\n");
        break;

    case WIFIDIR_EVENT_PEER_FOUND:
        msg_event = prepare_peer_event("peer_found",
                                       (struct wifidir_peer *)event->val);
        if (msg_event == NULL)
            ERR("Failed to prepare PEER_FOUND event\n");
        break;

    case WIFIDIR_EVENT_PEER_UPDATED:
        msg_event = prepare_peer_event("peer_updated",
                                       (struct wifidir_peer *)event->val);
        if (msg_event == NULL)
            ERR("Failed to prepare PEER_UPDATED event\n");
        break;

    case WIFIDIR_EVENT_NEGOTIATION_RESULT:

        msg_event = prepare_neg_result_event(
            (struct wifidir_neg_result *)event->val);
        if (msg_event == NULL)
            ERR("Failed to prepare negotiation result event\n");
        break;

    case WIFIDIR_EVENT_PEER_UNFOUND:
        msg_event = prepare_peer_unfound_event((unsigned char *)event->val);
        if (msg_event == NULL)
            ERR("failed to prepare peer unfound event\n");
        break;

    default:
        ERR("Event type %d is not supported.\n", type);
        break;
    }

    if (msg_event) {
        mwu_internal_send(msg_event);
        free(msg_event);
    }

    return;
}
