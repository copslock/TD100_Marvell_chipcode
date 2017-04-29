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

/* mwpamod.c: implementation of the wifi direct module
 */

#define UTIL_LOG_TAG "MWPAMOD"
#include "wps_msg.h"
#include "mwpamod.h"
#include "mwu_ioctl.h"
#include "util.h"
#include "mwu.h"
#include "wlan_hostcmd.h"
#include "wlan_wifidir.h" /* for event ids */
#include "mwpamod_os.h"
#include "wps_wlan.h"
#include "mwu_log.h"

extern PWPS_INFO gpwps_info;

#define MWPAMOD_MAX_SSID_LENGTH 32

/* A list of current active modules */
SLIST_HEAD(module_list, module);

/*Finally, we maintain the list of modules which are currently in use or
 * initialized. */
static struct module_list modules;

/* modules_num keeps track of the initialized modules, we don't really make any
 * sensible use of it at present except to know when to use SLIST_INIT for the
 * active modules list. Still this can come handy when we want to restrict the
 * number of active modules at some instance of time */
static int modules_num = 0;

#define MWPAMOD_CHECK_MODULE(mod) do {                                  \
        if (mod == NULL || mod->iface[0] == 0)                          \
            return MWPAMOD_ERR_INVAL;                                   \
        mwpamod = get_active_module(mod->iface);                        \
        if (!mwpamod) {                                                 \
            ERR("Module not present in active module list");            \
            return MWPAMOD_ERR_NOTREADY;                                \
        }                                                               \
    } while (0)

#define MWPAMOD_CHECK_MODULE_INIT(mod) do {                             \
        if (get_active_module(mod->iface)) {                               \
            ERR("Module already initialized. Deinit first.\n");         \
            return MWPAMOD_ERR_BUSY;                                    \
        }                                                               \
        if (mod == NULL || mod->iface[0] == 0)                          \
            return MWPAMOD_ERR_INVAL;                                   \
       /* mwpamod = mod;*/                                                  \
    } while (0)

int mwpamod_set_passphrase(struct module *mod,
                           struct mwpamod_net_info *net_info);
int mwpamod_sta_associate(struct module *mod, char *ssid);

void mwpamod_sta_kernel_event(struct event *e);

static enum mwpamod_error mwpamod_sta_state_machine(struct module *mod, int es, void *buffer, u16 size);

/* Internal enums and functions */
#define PRINT_CASE(i) case i: return #i

struct module *get_active_module(char *iface)
{
    struct module *cur = NULL, *found = NULL;
    int ret = 0;

    SLIST_FOREACH(cur, &modules, list_item) {
        if (!strncmp(cur->iface, iface, IFNAMSIZ)) {
            found = cur;
            break;
        }
    }
    return found;
}
/* HELPER FUNCTION FOR SENDIGN EVENTS */
static void emit_sta_event(struct module *mwpamod, int event_type, int status) {

   struct event *event;
   /* construct event and pass to event handler */
   event = (struct event *)malloc(sizeof(struct event));

   if (!event) {
       ERR("NO MEMORY FOR EVENT");
       goto fail;
   }

   event->type   = event_type;
   event->status = status;
   event->len    = 0;
   /* We send interface info up as a part of event */
   memcpy(event->iface, mwpamod->iface, IFNAMSIZ + 1);

   MODULE_DO_CALLBACK(mwpamod, event);

   free(event);
   return;

fail:
    ERR("Failed to prepare mwpamod event\n");
    return;
}


/* The following INTERNAL data structures are used by the STA state machine.
 */
enum sta_states {
    STA_IDLE,
    STA_READY,
    STA_ASSOCIATED,
    STA_AUTHENTICATED,
};

static char *sta_state_to_str(enum sta_states state)
{
    switch (state) {
       PRINT_CASE(STA_IDLE);
       PRINT_CASE(STA_READY);
       PRINT_CASE(STA_ASSOCIATED);
       PRINT_CASE(STA_AUTHENTICATED);
    }
    return "UNKNOWN";
}

static enum sta_states sta_state = STA_IDLE;

/* events processed by the state machine can either come from the kernel or the
 * API function calls.
 */
enum sta_event_space {
    STA_EVENT_SPACE_KERNEL,
    STA_EVENT_SPACE_API,
};

enum sta_api_event {
    STA_API_EVENT_INIT,
    STA_API_EVENT_DEINIT,
    STA_API_EVENT_CONNECT,
    STA_API_EVENT_DISCONNECT,
};

static char *sta_event_to_str(enum sta_api_event e)
{
    switch (e) {
        PRINT_CASE(STA_API_EVENT_INIT);
        PRINT_CASE(STA_API_EVENT_DEINIT);
        PRINT_CASE(STA_API_EVENT_CONNECT);
        PRINT_CASE(STA_API_EVENT_DISCONNECT);
    }
    return "UNKNOWN";
}

static char *kernel_event_to_str(enum sta_kernel_event e)
{
    switch (e) {
        PRINT_CASE(STA_KERNEL_EVENT_ASSOCIATED);
        PRINT_CASE(STA_KERNEL_EVENT_AUTHENTICATED);
        PRINT_CASE(STA_KERNEL_EVENT_LINK_LOST);
    }
    return "UNKNOWN";
}


enum mwpamod_error mwpamod_launch(void)
{
    return MWPAMOD_ERR_SUCCESS;
}

void mwpamod_halt(void)
{
    return;
}

enum mwpamod_error _mwpamod_sta_init(struct module *mod,
                                     struct mwpamod_sta_config *cfg)
{
    struct mwpamod_sta_info *sta_info = (struct mwpamod_sta_info *) mod->modpriv;
    /* Open netlink channel */
    if(rtnl_open(&sta_info->rth, RTMGRP_LINK) < 0)
    {
        ERR("Can't initialize rtnetlink socket");
        return MWPAMOD_ERR_COM;
    }

    /* Init IOCTL Sock des */
    sta_info->ioctl_sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sta_info->ioctl_sock < 0) {
        ERR("socket[PF_INET,SOCK_DGRAM]");
        return MWPAMOD_ERR_COM;
    }




    return MWPAMOD_ERR_SUCCESS;
}

enum mwpamod_error mwpamod_sta_init(struct module *mod,
                                    struct mwpamod_sta_config *cfg)
{
    struct event *e = NULL;
    enum mwpamod_error ret = MWPAMOD_ERR_NOMEM;
    struct module *mwpamod = NULL;
    struct mwpamod_sta_info *sta_info = NULL;

    MWPAMOD_CHECK_MODULE_INIT(mod);

    mwpamod = (struct module *)malloc(sizeof(struct module));
    if (!mwpamod) {
        ERR("Error allocation new module mwpamod.");
        ret = MWPAMOD_ERR_NOMEM;
        goto module_error;
    }

    memcpy(mwpamod, mod, sizeof(struct module));

    /* Allocate the sta_info here, we save this info into the priv member of
     * module struct. */
    sta_info = malloc(sizeof(struct mwpamod_sta_info));
    if (!sta_info) {
        ERR("Can't allocate memory for module sta config");
        ret = MWPAMOD_ERR_NOMEM;
        goto sta_info_error;
    }

    memset(sta_info, 0, sizeof(struct mwpamod_sta_info));

    /* Now assign the sta_info to the module_priv;
     * so we can track the run time info with the modules */
    mwpamod->modpriv = sta_info;

    e = malloc(sizeof(struct event) + sizeof(struct mwpamod_sta_config));
    if (!e) {
        ERR("No memory for api init event");
        ret = MWPAMOD_ERR_NOMEM;
        goto evt_error;
    }

    e->type = STA_API_EVENT_INIT;
    e->len = 0;
    memcpy(e->val, cfg, sizeof(struct mwpamod_sta_config));
    ret = mwpamod_sta_state_machine(mwpamod, STA_EVENT_SPACE_API, e, 0);
    if (ret == MWPAMOD_ERR_SUCCESS) {
        /* add the module to the active module list, if this is the first module
         * getting initilized, init the list first.*/
        if (!modules_num)
            SLIST_INIT(&modules);
        SLIST_INSERT_HEAD(&modules, mwpamod, list_item);
        ++modules_num;
        free(e);
        return ret; /* success */
    }

/* failure */
    free(e);
evt_error:
    free(sta_info);
sta_info_error:
    free(mwpamod);
module_error:
    return ret;
}

void mwpamod_sta_deinit(struct module *mod)
{
    int bss_type;

    struct event e;
    enum mwpamod_error ret;
    struct mwpamod_sta_info *sta_info;
    struct module *mwpamod;

    mwpamod = get_active_module(mod->iface);
    if (!mwpamod)
        return;

    INFO("Checking BSS type...\n");
    if (mwu_get_bss_type(mod->iface, &bss_type) == MWU_ERR_SUCCESS) {
        INFO("bss_type: %d\n", bss_type);
    } else {
        INFO("WARNING: Failed to get BSS type\n");
    }
    if (bss_type == BSS_TYPE_STA) {
        mwpamod_sta_disconnect(mod);
    } else {
        INFO("BSS type is not STA.");
        INFO("Setting BSS type back to STA...\n");
        if (mwu_set_bss_type(mod->iface, BSS_TYPE_STA) != MWU_ERR_SUCCESS)
            WARN("WARNING: failed to reset to STA mode\n");
    }

    e.type = STA_API_EVENT_DEINIT;
    e.len = 0;
    ret = mwpamod_sta_state_machine(mwpamod, STA_EVENT_SPACE_API, &e, 0);
    if (ret == MWPAMOD_ERR_SUCCESS) {
        /* Remove the module from active module list */
        if (get_active_module(mod->iface)) {
            SLIST_REMOVE(&modules, mwpamod, module, list_item) ;
            --modules_num;
            /*free the module and private data assoicated with it. */
            sta_info = (struct mwpamod_sta_info *) mwpamod->modpriv;
            free(sta_info);
            free(mwpamod);
        }
    }
}


void _mwpamod_sta_deinit(struct module *mod)
{

    struct mwpamod_sta_info *sta_info = NULL;
    PWPS_DATA wps_s = (WPS_DATA *) &gpwps_info->wps_data;

    sta_info = (struct mwpamod_sta_info *)mod->modpriv;

    mwpamod_sta_disconnect(mod);

    rtnl_close(&(sta_info->rth));

    wps_s->bss_type = BSS_TYPE_STA;
    INFO("Done.\n");
    /* Close the ioctl sock des opened at init time */
    close(sta_info->ioctl_sock);
}

enum mwpamod_error mwpamod_sta_connect(struct module *mod,
                                       struct mwpamod_net_info *net_info)
{
    enum mwpamod_error ret;
    struct event *e;
    struct module *mwpamod;                                         \
    MWPAMOD_CHECK_MODULE(mod);

    e = malloc(sizeof(struct event) + sizeof(struct mwpamod_net_info));
    if (!e) {
        ERR("Failed to allocate memory for init command\n");
        return MWPAMOD_ERR_NOMEM;
    }
    e->type = STA_API_EVENT_CONNECT;
    e->len = 0;
    memcpy(e->val, net_info, sizeof(struct mwpamod_net_info));
    /* event size is ignored for api events because the size is in the event
     * struct
     */
    ret = mwpamod_sta_state_machine(mwpamod, STA_EVENT_SPACE_API, e, 0);
    free(e);
    return ret;
}

enum mwpamod_error _mwpamod_sta_connect(struct module *mod, struct mwpamod_net_info *net_info)
{
    int ret = 0;

    /* Ideally this is not required here. But some devices in market including
     * Samsung Galaxy SII do not invalidate the previous association happened
     * during the wps handshake. If we not deauth the previous association
     * happened for wps exchange here, firmware will automatically send
     * a deauth before association and 4 way hs happened, making sta
     * state machine think like it did get a deauth after association is
     * initiated and this deauth event will trigger a sta_connect event
     * with status MWPAMOD_ERR_NAK i.e key mismatch

    if (mwu_set_deauth(mod->iface) != MWU_ERR_SUCCESS)
        ERR("Failed to deauth.");
    else
        WARN("Blind deauth issued/");
    */
    ret = mwpamod_set_passphrase(mod, net_info);
    if (ret != MWPAMOD_ERR_SUCCESS)
        return ret;

    ret = mwpamod_sta_associate(mod, net_info->ssid);
    return ret;
}

/* drop the association
 *
 * params :
 *      none
 *
 * return :
 *      none
 */
void mwpamod_sta_disconnect(struct module *mod)
{
    if (!get_active_module(mod->iface))
        return;

    if (mwu_set_deauth(mod->iface) != MWU_ERR_SUCCESS)
        ERR("Failed to deauth.");
}

/* Associate client to the specified ssid
 *
 * params:
 *      ssid        - pointer to ssid str
 *
 * return
 *      MWPAMOD_ERR_SUCCESS - Success
 *      MWPAMOD_ERR_COM     - Lower layer failure (eg. ioctl failed)
 */
int mwpamod_sta_associate(struct module *mod, char *ssid)
{
    int ret = MWPAMOD_ERR_SUCCESS;

    if (mwu_set_ssid(mod->iface, (unsigned char*) ssid, strlen(ssid), 0) != MWU_ERR_SUCCESS)
        ret = MWPAMOD_ERR_COM;

    return ret;
}

/**
 *  sets ssid and psk in fw using embedded supplicant
 *
 *  params :
 *          ssid  - ssid to set
 *          key   - key to set
 *
 *  return
 *      MWPAMOD_ERR_SUCCESS - Success
 *      MWPAMOD_ERR_COM     - Lower layer failure (eg. ioctl failed)
 */

#define PASSPHRASE_CMD_BUF_SZ 128

int mwpamod_set_passphrase(struct module *mod,
                           struct mwpamod_net_info *net_info)
{
    char *buf;
    struct iwreq iwr;
    int ret = MWPAMOD_ERR_SUCCESS;
    int ioctl_val, sub_ioctlval, buf_len, i;

    buf = calloc(PASSPHRASE_CMD_BUF_SZ, sizeof(char));
    if (buf == NULL) {
        ERR("Can not allocate passphrase cmd buff");
        return MWPAMOD_ERR_NOMEM;
    }

    /* Form the passphrase command for setting ssid, and psk */
    if (strcmp((char *)net_info->key, "none") == 0) {
        /* 2 disables embedded supplicant
         * This is useful if application sends key for open nw
         * and later realizes the mistake and want to clear the set key
         * This makes the api stateless */
        strcpy(buf, "2");
    } else {
        strcpy(buf, "1;ssid=");
        strcat(buf, net_info->ssid);
        if (net_info->key_len >= 8 && net_info->key_len <= 63) {
            INFO("Length of passphrase is %d", net_info->key_len);
            strcat(buf, ";passphrase=");
        } else if (net_info->key_len == MWPAMOD_MAX_KEY) {
            INFO("Length of pmk is %d", net_info->key_len);
            strcat(buf, ";psk=");
        } else {
            ERR("Invalid Key. Key should be 8...63 ASCII or 64 HEX Char long...");
            free(buf);
            return MWPAMOD_ERR_INVAL;
        }
        strcat(buf, (char *)net_info->key);
    }

    buf_len = strlen(buf);

    /* Strange!! I already used str* function!
     * Fix for this is already in driver, we should remove this with next driver
     * build
    buf[buf_len] = '\0';
    buf_len += 1;
    */

    INFO("\nCommand is %s, len %d\n", buf, buf_len);

    if (mwu_set_passphrase(mod->iface, buf, buf_len) != MWU_ERR_SUCCESS) {
        ERR("Set passphrase failed.\n");
        ret = MWPAMOD_ERR_COM;
    }

    free(buf);
    return ret;
}

void mwpamod_sta_kernel_event(struct event *e) {

    /* Check if the iface receieved in the event is a part of active module or
     * not */
    struct module *mwpamod = NULL;
    mwpamod = get_active_module(e->val);
    if (!mwpamod) {
        INFO("%s: Interface not initialized. Droping wext event", e->val);
    } else {
        mwpamod_sta_state_machine(mwpamod, STA_EVENT_SPACE_KERNEL, e, e->len);
    }

    return;
}


static enum mwpamod_error mwpamod_sta_state_machine(struct module *mod, int es, void *buffer, u16 size)
{
    struct event *apie = NULL;
    struct event *ke = NULL;
    struct mwpamod_sta_info *sta_info;
    enum mwpamod_error ret;

    struct module *mwpamod = mod;

    sta_info = (struct mwpamod_sta_info *) mwpamod->modpriv;
    sta_state = sta_info->sta_state;

    if (es == STA_EVENT_SPACE_KERNEL) {
        ke = (struct event *)buffer;
        INFO("STA got kernel event %s in state %s\n", kernel_event_to_str(ke->type),
             sta_state_to_str(sta_state));
    } else if (es == STA_EVENT_SPACE_API) {
        apie = (struct event *)buffer;
        INFO("STA got event %s in state %s\n", sta_event_to_str(apie->type),
             sta_state_to_str(sta_state));
    }



    switch (sta_state) {

    case STA_IDLE:
        if (es == STA_EVENT_SPACE_API && apie->type == STA_API_EVENT_INIT) {
            /* Note the internal function called by state machine */
            ERR("=> Idle->Init");
            ret = _mwpamod_sta_init(mwpamod, (struct mwpamod_sta_config *)apie->val);
            if (ret == MWPAMOD_ERR_SUCCESS) {
                sta_state = STA_READY;
                sta_info->sta_state = sta_state;
            }
            return ret;
        } else if (es == STA_EVENT_SPACE_API && apie->type == STA_API_EVENT_DEINIT) {
            return MWPAMOD_ERR_SUCCESS;
        }
        break;

    case STA_READY:
        if (es == STA_EVENT_SPACE_API && apie->type == STA_API_EVENT_INIT) {
                return MWPAMOD_ERR_BUSY;

        } else if(es == STA_EVENT_SPACE_API && apie->type == STA_API_EVENT_DEINIT) {
            _mwpamod_sta_deinit(mwpamod);
            sta_state = STA_IDLE;
            sta_info->sta_state = sta_state;
            return MWPAMOD_ERR_SUCCESS;
        } else if (es == STA_EVENT_SPACE_API && apie->type == STA_API_EVENT_CONNECT) {
            ret = _mwpamod_sta_connect(mwpamod, (struct mwpamod_net_info *)apie->val);
            if (ret == MWPAMOD_ERR_SUCCESS) {
                sta_state = STA_ASSOCIATED;
                sta_info->sta_state = sta_state;
            } else {
                ERR("Failed to connect.  Going IDLE\n");
                emit_sta_event(mwpamod, MWPAMOD_EVENT_STA_CONNECT, MWPAMOD_ERR_COM);
                _mwpamod_sta_deinit(mwpamod);
                sta_state = STA_IDLE;
                sta_info->sta_state = sta_state;
            }
            return ret;
        }
        break;

    case STA_ASSOCIATED:
        if (es == STA_EVENT_SPACE_API && apie->type == STA_API_EVENT_INIT) {
                return MWPAMOD_ERR_BUSY;
        } else if (es == STA_EVENT_SPACE_API && apie->type == STA_API_EVENT_DEINIT) {
            _mwpamod_sta_deinit(mwpamod);
            sta_state = STA_IDLE;
            sta_info->sta_state = sta_state;
            return MWPAMOD_ERR_SUCCESS;
        } else if (es == STA_EVENT_SPACE_API && apie->type == STA_API_EVENT_CONNECT) {
            ERR("Previous connection already in progress. Disconnect first.");
            return MWPAMOD_ERR_BUSY;
        } else if (es == STA_EVENT_SPACE_API && apie->type == STA_API_EVENT_DISCONNECT) {
            mwpamod_sta_disconnect(mwpamod);
            sta_state = STA_READY;
            sta_info->sta_state = sta_state;
            return MWPAMOD_ERR_SUCCESS;
        } else if (es == STA_EVENT_SPACE_KERNEL && ke->type == STA_KERNEL_EVENT_LINK_LOST) {
            // We disable detecting key mis-match util we find out
            // some noble way to detect key mis-match
            // This change facilitates intraop with Samsung Galaxy SII

            /* ERR("Link Lost. Perhaps incorrect network key");
            sta_state = STA_READY;
            sta_info->sta_state = sta_state;
            emit_sta_event(mwpamod, MWPAMOD_EVENT_STA_CONNECT, MWPAMOD_ERR_NAK); */
            return MWPAMOD_ERR_SUCCESS;
        } else if (es == STA_EVENT_SPACE_KERNEL && ke->type == STA_KERNEL_EVENT_AUTHENTICATED) {
            INFO("Connection with AP successfull");
            sta_state = STA_AUTHENTICATED;
            sta_info->sta_state = sta_state;
            emit_sta_event(mwpamod, MWPAMOD_EVENT_STA_CONNECT, MWPAMOD_ERR_SUCCESS);
            return MWPAMOD_ERR_SUCCESS;
        }
        break;

    case STA_AUTHENTICATED:
        if (es == STA_EVENT_SPACE_API && apie->type == STA_API_EVENT_INIT) {
                return MWPAMOD_ERR_BUSY;
        } else if (es == STA_EVENT_SPACE_API && apie->type == STA_API_EVENT_DEINIT) {
            _mwpamod_sta_deinit(mwpamod);
            sta_state = STA_IDLE;
            sta_info->sta_state = sta_state;
            return MWPAMOD_ERR_SUCCESS;
        } else if (es == STA_EVENT_SPACE_API && apie->type == STA_API_EVENT_CONNECT) {
            ERR("Already connected. Disconnect first.");
            return MWPAMOD_ERR_BUSY;
        } else if (es == STA_EVENT_SPACE_API && apie->type == STA_API_EVENT_DISCONNECT) {
            sta_state = STA_READY;
            sta_info->sta_state = sta_state;
            return MWPAMOD_ERR_SUCCESS;
        } else if (es == STA_EVENT_SPACE_KERNEL && ke->type == STA_KERNEL_EVENT_LINK_LOST) {
            ERR("Link Lost. Perhaps we moved out of BSS or AP went off");
            sta_state = STA_READY;
            sta_info->sta_state = sta_state;
            emit_sta_event(mwpamod, MWPAMOD_EVENT_STA_LINK_LOST, MWPAMOD_ERR_NAK);
            return MWPAMOD_ERR_SUCCESS;
        }
        break;
    }

    return MWPAMOD_ERR_SUCCESS;
}

/* The following INTERNAL data structures are used by the AP state machine.
 */
enum ap_states {
    AP_IDLE,
    AP_READY,
    AP_ACTIVE,
};

static char *ap_state_to_str(enum ap_states state)
{
    switch (state) {
        PRINT_CASE(AP_IDLE);
        PRINT_CASE(AP_READY);
        PRINT_CASE(AP_ACTIVE);
    }
    return "UNKNOWN";
}

static enum ap_states ap_state = AP_IDLE;

/* events processed by the state machine can either come from the driver or the
 * API function calls.
 */
enum ap_event_space {
    AP_EVENT_SPACE_DRIVER,
    AP_EVENT_SPACE_API,
};

enum ap_api_event {
    AP_API_EVENT_INIT,
    AP_API_EVENT_DEINIT,
    AP_API_EVENT_START,
    AP_API_EVENT_STOP,
};

static char *ap_event_to_str(enum ap_api_event e)
{
    switch (e) {
        PRINT_CASE(AP_API_EVENT_INIT);
        PRINT_CASE(AP_API_EVENT_DEINIT);
        PRINT_CASE(AP_API_EVENT_START);
        PRINT_CASE(AP_API_EVENT_STOP);
    }
    return "UNKNOWN";
}

static enum mwpamod_error do_init(struct module *mwpamod, struct mwpamod_ap_config *cfg) {

    bss_config_t bss_config;
    PWPS_DATA wps_s = (WPS_DATA *) &gpwps_info->wps_data;

    int bss_type;
    int ret;

    ret = mwu_get_bss_type(mwpamod->iface, &bss_type);
    if (ret != MWU_ERR_SUCCESS) {
        ERR("Failed to get BSS type\n");
        return ret;
    }
    if (bss_type == BSS_TYPE_UAP) {
        ERR("Already configured as an AP.  Call mwpamod_ap_deinit\n");
        return MWPAMOD_ERR_BUSY;
    }

    if (cfg->net_info.key_len == 64)
        WARN("WARNING: 32-byte psk is untested.");

    /* Make us an AP. */
    if ((ret = mwu_set_bss_type(mwpamod->iface, BSS_TYPE_UAP)
        != MWU_ERR_SUCCESS)) {
        ERR("Failed to go into uAP mode.\n");
        return ret;
    }
    wps_s->bss_type = BSS_TYPE_UAP;

    if (apcmd_get_bss_config(&bss_config) != WPS_STATUS_SUCCESS) {
        ERR("Failed to get BSS Config.\n");
        return MWPAMOD_ERR_COM;
    }

    WARN("WARNING: Hard coding WPA2PSK AES!");
    bss_config.protocol = UAP_PROTO_WPA2;
#define KEY_MGMT_PSK 0x02
    bss_config.key_mgmt = KEY_MGMT_PSK;
    bss_config.key_mgmt_operation = 0;
    bss_config.wpa_cfg.pairwise_cipher_wpa2 = UAP_CIPH_AES;
    bss_config.wpa_cfg.group_cipher = UAP_CIPH_AES;
    bss_config.wpa_cfg.length = cfg->net_info.key_len;
    memcpy(&bss_config.wpa_cfg.passphrase, cfg->net_info.key,
           cfg->net_info.key_len);
    /*
     * net_info.ssid is defined as a char array of length MWPAMOD_MAX_SSID + 1
     * where MWPAMOD_MAX_SSID is equal to MAX_SSID_LENGTH. As bss_config_t.ssid
     * is defined as a u8 array of length MAX_SSID_LENGTH, we can simply set
     * net_info.ssid[MAX_SSID_LENGTH] to 0 so that strlen will never exceed
     * MAX_SSID_LENGTH.
     */
    cfg->net_info.ssid[MAX_SSID_LENGTH] = 0;
    strncpy((char *) bss_config.ssid.ssid, cfg->net_info.ssid, MAX_SSID_LENGTH);
    bss_config.ssid.ssid_len = strlen(cfg->net_info.ssid);

    if (apcmd_set_bss_config(&bss_config) != WPS_STATUS_SUCCESS) {
        ERR("Failed to set BSS Config.\n");
        return MWPAMOD_ERR_COM;
    }

    INFO("Successfully set GO security params");

    /* TODO: this should be in the registrar, not the AP */
    load_cred_info(&gpwps_info->wps_data, gpwps_info, &bss_config);
    wlan_wifidir_generate_psk(gpwps_info);
    INFO("Got credential info\n");

    return MWPAMOD_ERR_SUCCESS;
}

static void do_deinit(struct module *mwpamod)
{
    int bss_type;
    PWPS_DATA wps_s = (WPS_DATA *) &gpwps_info->wps_data;

    INFO("AP deinit called.\n");
    INFO("Checking BSS type...\n");
    if (mwu_get_bss_type(mwpamod->iface, &bss_type) == MWU_ERR_SUCCESS) {
        INFO("bss_type: %d\n", bss_type);
    } else {
        INFO("WARNING: Failed to get BSS type\n");
    }
    if (bss_type == BSS_TYPE_STA) {
        INFO("BSS type is already STA");
    } else {
        INFO("Stopping BSS...\n");
        if (mwu_apcmd_stop_bss(mwpamod->iface) != MWU_ERR_SUCCESS)
            WARN("WARNING: failed to stop BSS\n");

        /* Things break if we don't reset as an STA.  Most notably, we won't be
         * able to call mwpamod_ap_init again.
         */
        INFO("Setting BSS type back to STA...\n");
        if (mwu_set_bss_type(mwpamod->iface, BSS_TYPE_STA) != MWU_ERR_SUCCESS)
            WARN("WARNING: failed to reset to STA mode\n");

        INFO("Checking BSS type...\n");
        if (mwu_get_bss_type(mwpamod->iface, &bss_type) == MWU_ERR_SUCCESS) {
            if (bss_type != BSS_TYPE_STA)
                WARN("WARNING: Failed to switch back to STA mode");
        } else {
            WARN("WARNING: Failed to get BSS type\n");
        }
    }
    wps_s->bss_type = BSS_TYPE_STA;

    INFO("Done.\n");
}

enum mwpamod_error do_start(struct module *mwpamod)
{
    if (mwu_apcmd_start_bss(mwpamod->iface) != MWU_ERR_SUCCESS) {
        ERR("Failed to start AP.\n");
        return MWPAMOD_ERR_COM;
    }
    INFO("Started AP\n");

    return MWPAMOD_ERR_SUCCESS;
}

static void emit_ap_connection_event(struct module *mwpamod,
                                     eventbuf_station_connect *eb,
                                     enum mwpamod_event_type et,
                                     enum mwpamod_error status)
{
   struct event *event;

   /* construct event and pass to event handler */
   event = (struct event *)malloc(sizeof(struct event) + ETH_ALEN);

   if (!event) {
       ERR("Failed to allocate connection event!\n");
       return;
   }

   event->type   = et;
   event->len    = ETH_ALEN;
   event->status = status;
   memcpy(event->val, eb->sta_mac, ETH_ALEN);
   /* We send interface info up as a part of event */
   memcpy(event->iface, mwpamod->iface, IFNAMSIZ + 1);

   MODULE_DO_CALLBACK(mwpamod, event);

   free(event);
   return;
}

static enum mwpamod_error mwpamod_ap_state_machine(struct module *mod, int es, void *buffer, u16 size)
{
    struct event *apie = NULL;
    event_header *de = NULL;
    enum mwpamod_error ret;
    struct mwpamod_ap_info *ap_info = NULL;

    struct module *mwpamod = mod;
    ap_info = (struct mwpamod_ap_info *) mwpamod->modpriv;
    ap_state = ap_info->ap_state;

    if (es == AP_EVENT_SPACE_DRIVER) {
        de = (event_header *)buffer;
        INFO("AP got driver event %s in state %s\n", nl_event_id_to_str(de->event_id),
             ap_state_to_str(ap_state));
    } else if (es == AP_EVENT_SPACE_API) {
        apie = (struct event *)buffer;
        INFO("AP got event %s in state %s\n", ap_event_to_str(apie->type),
             ap_state_to_str(ap_state));
    }
    switch (ap_info->ap_state) {

    case AP_IDLE:
        if (es == AP_EVENT_SPACE_API && apie->type == AP_API_EVENT_INIT) {
            ret = do_init(mwpamod, (struct mwpamod_ap_config *)apie->val);
            if (ret == MWPAMOD_ERR_SUCCESS)
                ap_info->ap_state = AP_READY;
            return ret;
        } else if (es == AP_EVENT_SPACE_API && apie->type == AP_API_EVENT_DEINIT) {
            return MWPAMOD_ERR_SUCCESS;
        }
        break;

    case AP_READY:
        if (es == AP_EVENT_SPACE_API && apie->type == AP_API_EVENT_INIT) {
            return MWPAMOD_ERR_BUSY;

        } else if(es == AP_EVENT_SPACE_API && apie->type == AP_API_EVENT_DEINIT) {
            do_deinit(mwpamod);
            ap_info->ap_state = AP_IDLE;
            return MWPAMOD_ERR_SUCCESS;
        } else if (es == AP_EVENT_SPACE_API && apie->type == AP_API_EVENT_START) {
            ret = do_start(mwpamod);
            if (ret == MWPAMOD_ERR_SUCCESS) {
                ap_info->ap_state = AP_ACTIVE;
            } else {
                ERR("Failed to start AP.  Going IDLE\n");
                do_deinit(mwpamod);
                ap_info->ap_state = AP_IDLE;
            }
            return ret;
        }
        break;

    case AP_ACTIVE:
        /* Once we are active, our only real job is informing the upper layer
         * when STAs connect and disconnect from us.  For now, we only report
         * this for RSN networks, not for open networks.  Further, we don't
         * actually track individual STAs yet.  So we don't actually implement
         * the proper notification API.  For example, if an STA fails to
         * authenticate, we get a DEAUTH event from the firmware and just emit
         * a DISCONNECT event.  But really in this case we should emit a
         * CONNECT event with a suitable status code.
         */
        if (es == AP_EVENT_SPACE_API && apie->type == AP_API_EVENT_INIT) {
            return MWPAMOD_ERR_BUSY;

        } else if (es == AP_EVENT_SPACE_API && apie->type == AP_API_EVENT_DEINIT) {
            do_deinit(mwpamod);
            ap_info->ap_state = AP_IDLE;
            return MWPAMOD_ERR_SUCCESS;
        } else if (es == AP_EVENT_SPACE_DRIVER &&
                   de->event_id == EV_ID_UAP_EV_RSN_CONNECT) {
            emit_ap_connection_event(mwpamod,
                                     (eventbuf_station_connect *)de->event_data,
                                     MWPAMOD_EVENT_AP_CONNECT,
                                     MWPAMOD_ERR_SUCCESS);
        } else if (es == AP_EVENT_SPACE_DRIVER &&
                   de->event_id == EV_ID_UAP_EV_ID_STA_DEAUTH) {
            emit_ap_connection_event(mwpamod,
                                     (eventbuf_station_connect *)de->event_data,
                                     MWPAMOD_EVENT_AP_DISCONNECT,
                                     MWPAMOD_ERR_SUCCESS);
        }
        break;
    }
    return MWPAMOD_ERR_SUCCESS;
}

/* recevive and event from the driver and send it to the state machine */
void mwpamod_ap_driver_event(char *ifname, u8 *buffer, u16 size)
{

    /* Check if the iface receieved in the event is a part of active module or
     * not */
    struct module *mwpamod = NULL;
    mwpamod = get_active_module(ifname);
    if (!mwpamod) {
        INFO("%s: Interface not initialized. Droping ap driver event", ifname);
    } else {
        mwpamod_ap_state_machine(mwpamod, AP_EVENT_SPACE_DRIVER, buffer, size);
    }

    return;

}

enum mwpamod_error mwpamod_ap_init(struct module *mod,
                                   struct mwpamod_ap_config *cfg)
{
    struct event *e = NULL;
    enum mwpamod_error ret = MWPAMOD_ERR_NOMEM;
    struct module *mwpamod = NULL;
    struct mwpamod_ap_info *ap_info = NULL;

    MWPAMOD_CHECK_MODULE_INIT(mod);

    mwpamod = (struct module *)malloc(sizeof(struct module));
    if(!mwpamod) {
        ERR("Failed to allocte ap module for init");
        ret = MWPAMOD_ERR_NOMEM;
        goto module_error;
    }

    memcpy(mwpamod, mod, sizeof(struct module));

    /* We allocate the ap info here, we keep track of this runtime
     * information using the module private member of module structure.
     */
    ap_info = malloc(sizeof(struct mwpamod_ap_info));
    if (!ap_info) {
        ERR("Can not allocate ap runtime config");
        ret = MWPAMOD_ERR_NOMEM;
        goto ap_info_error;
    }

    memset(ap_info, 0, sizeof(struct mwpamod_ap_info));

    /* asign the ap_info pointer to private modpriv*/
    mwpamod->modpriv = ap_info;

    e = malloc(sizeof(struct event) + sizeof(struct mwpamod_ap_config));
    if (!e) {
        ERR("Failed to allocate memory for init command\n");
        ret = MWPAMOD_ERR_NOMEM;
        goto evt_error;
    }
    e->type = AP_API_EVENT_INIT;
    e->len = 0;
    memcpy(e->val, cfg, sizeof(struct mwpamod_ap_config));
    /* event size is ignored for api events because the size is in the event
     * struct
     */
    ret = mwpamod_ap_state_machine(mwpamod, AP_EVENT_SPACE_API, e, 0);
    if (ret == MWPAMOD_ERR_SUCCESS) {
            /* add the module to the active module list */
            SLIST_INSERT_HEAD(&modules, mwpamod, list_item);
            free(e);
            return ret; /* success */
    }

/* failure */
    free(e);
evt_error:
    free(ap_info);
ap_info_error:
    free(mwpamod);
module_error:

    return ret;
}

void mwpamod_ap_deinit(struct module *mod)
{
    struct event e;
    int ret;
    struct module *mwpamod;
    struct mwpamod_sta_info *sta_info = NULL;

    mwpamod = get_active_module(mod->iface);
    if (!mwpamod) {
        ERR("Module not present in active module list. Init first.");
        return;
    }

    e.type = AP_API_EVENT_DEINIT;
    e.len = 0;
    ret = mwpamod_ap_state_machine(mwpamod, AP_EVENT_SPACE_API, &e, 0);
    if (ret == MWPAMOD_ERR_SUCCESS) {
        /* Remove the module from active module list */
        if (get_active_module(mwpamod->iface)) {
            SLIST_REMOVE(&modules, mwpamod, module, list_item) ;
            /*free the module and private data assoicated with it. */
            sta_info = (struct mwpamod_sta_info *) mwpamod->modpriv;
            free(sta_info);
            free(mwpamod);
        }
    }

}

enum mwpamod_error mwpamod_ap_start(struct module *mod)
{
    struct event e;
    struct module *mwpamod;
    MWPAMOD_CHECK_MODULE(mod);

    e.type = AP_API_EVENT_START;
    e.len = 0;
    return mwpamod_ap_state_machine(mwpamod, AP_EVENT_SPACE_API, &e, 0);
}

enum mwpamod_error mwpamod_ap_deauth(struct module *mod, unsigned char *sta_mac)
{
    struct module *mwpamod;
    MWPAMOD_CHECK_MODULE(mod);

    if (!sta_mac)
        return MWPAMOD_ERR_INVAL;

    return mwu_set_ap_deauth(mwpamod->iface, sta_mac);
}

enum mwpamod_error mwpamod_ap_get_info(struct module *mod,
                                       struct mwpamod_ap_config *cfg,
                                       struct mwpamod_ap_info *ap_info)
{
    struct module *mwpamod;
    int ret = 0;
    MWPAMOD_CHECK_MODULE(mod);

    /* we should populate the complete
     * 1. mwpamod_ap_config
     * 2. mwpamod_ap_info
     * here. Currently we do need only sta_list part of the ap_info so we
     * populate that here, othere structure elements should be populated as
     * needed in future. */

    ret = mwu_get_ap_sta_list(mwpamod->iface, &ap_info->sta_list);
    INFO("%d STA connected to GO", ap_info->sta_list.sta_count);
    if (ret != MWPAMOD_ERR_SUCCESS) {
        ERR("Unable to get sta_list");
        return ret ;
    }

    return MWPAMOD_ERR_SUCCESS;
}
