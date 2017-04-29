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

/* mwpsmod_mwu.c: Implementation of mwu control interface to mwpsmod
 */

#define UTIL_LOG_TAG "MWU MWPSMOD"
#include "util.h"
#include "mwu_log.h"
#include "mwpsmod_mwu.h"
#include "mwu_internal.h"

/* we currently only support one module instance at a time.  And we need to
 * know if it's a registrar or enrolleeg so we only deinit the one that we
 * init'd.  We use the private member of the module for this purpose.  Later,
 * we may want to make the cbpriv a proper struct with other important state
 * tracking info too.
 */
static struct module mwpsmod;
#define REGISTRAR 1
#define ENROLLEE 2

#define MWPSMOD_MAX_CMD 24
#define MWPSMOD_MAX_EVENT_NAME 24
#define MWPSMOD_MOD_NAME "mwpsmod"
#define MWPSMOD_MOD_KV MWU_KV("module", MWPSMOD_MOD_NAME)
#define MWPSMOD_MOD_KV_SZ MWU_KV_SZ("module", MWPSMOD_MOD_NAME)

#define MWPSMOD_CMD_HDR_SCAN                                      \
    MWPSMOD_MOD_KV                                                \
    MWU_KV_SCAN_STR("iface", IFNAMSIZ)                            \
    MWU_KV_SCAN_STR("cmd", MWPSMOD_MAX_CMD)

#define MWPSMOD_CMD_DEVICE_SCAN                       \
    MWU_KV_SCAN_STR("device_name", MAX_DEVICE_NAME_LEN)  \
    MWU_KV_SCAN_STR("model_name", MAX_DEVICE_NAME_LEN)   \
    MWU_KV_SCAN_STR("model_number", MAX_DEVICE_NAME_LEN) \
    MWU_KV_FMT_HEX16("methods")

#define MWPSMOD_DEVICE_FMT                           \
    MWU_KV_FMT_STR("device_name", MAX_DEVICE_NAME_LEN)  \
    MWU_KV_FMT_STR("model_name", MAX_DEVICE_NAME_LEN)   \
    MWU_KV_FMT_STR("model_number", MAX_DEVICE_NAME_LEN) \
    MWU_KV_FMT_HEX16("methods")

#define MWPSMOD_CMD_DEVICE_SZ                           \
    (MWU_KV_SZ_STR("device_name", MAX_DEVICE_NAME_LEN) +   \
     MWU_KV_SZ_STR("model_name", MAX_DEVICE_NAME_LEN) +    \
     MWU_KV_SZ_STR("model_number", MAX_DEVICE_NAME_LEN) +  \
     MWU_KV_SZ_HEX16("methods"))

#define MWPSMOD_CMD_CRED_SCAN                  \
    MWU_KV_ONE_LINE_STR("ssid", MAX_SSID_LEN)  \
    MWU_KV_FMT_HEX16("auth")                   \
    MWU_KV_FMT_HEX16("encrypt")                \
    MWU_KV_SCAN_STR("key", MWPSMOD_MAX_KEY)

#define MWPSMOD_CRED_FMT                       \
    MWU_KV_FMT_STR("ssid", MAX_SSID_LEN)   \
    MWU_KV_FMT_HEX16("auth")                   \
    MWU_KV_FMT_HEX16("encrypt")                \
    MWU_KV_FMT_STR("key", MWPSMOD_MAX_KEY)

#define MWPSMOD_CRED_SZ                                \
    (MWU_KV_SZ_STR("ssid", MAX_SSID_LEN) +         \
     MWU_KV_SZ_HEX16("auth") +                         \
     MWU_KV_SZ_HEX16("encrypt") +                      \
     MWU_KV_SZ_STR("key", MWPSMOD_MAX_KEY))

#define MWPSMOD_CMD_REG_INIT_SCAN \
    MWPSMOD_CMD_HDR_SCAN \
    MWPSMOD_CMD_DEVICE_SCAN \
    MWPSMOD_CMD_CRED_SCAN

#define MWPSMOD_CMD_REG_SET_PIN_SCAN            \
    MWPSMOD_CMD_HDR_SCAN                        \
    MWU_KV_SCAN_STR("pin", MWPSMOD_PIN_LEN)

#define MWPSMOD_CMD_ENROLLEE_INIT_SCAN \
    MWPSMOD_CMD_HDR_SCAN \
    MWPSMOD_CMD_DEVICE_SCAN

#define MWPSMOD_CMD_ENROLLEE_START_SCAN \
    MWPSMOD_CMD_HDR_SCAN                \
    MWU_KV_FMT_MAC("mac")               \
    MWU_KV_SCAN_STR("pin", MWPSMOD_PIN_LEN)

#define MWPSMOD_EVENT_HDR_FMT                                     \
    MWPSMOD_MOD_KV                                                \
    MWU_KV_FMT_STR("iface", IFNAMSIZ)                             \
    MWU_KV_FMT_STR("event", MWPSMOD_MAX_EVENT_NAME)

#define MWPSMOD_EVENT_HDR_SZ                                           \
    (MWPSMOD_MOD_KV_SZ +                                               \
     MWU_KV_SZ_STR("iface", IFNAMSIZ)  +                               \
     MWU_KV_SZ_STR("event", MWPSMOD_MAX_EVENT_NAME))

#define MWPSMOD_EVENT_CRED_FMT                  \
    MWPSMOD_EVENT_HDR_FMT                       \
    MWU_KV_FMT_DEC32("status")                  \
    MWPSMOD_CRED_FMT

#define MWPSMOD_EVENT_CRED_SZ                     \
    (MWPSMOD_EVENT_HDR_SZ +                       \
     MWU_KV_SZ_DEC32("status") +                  \
     MWPSMOD_CRED_SZ)

#define MWPSMOD_EVENT_DEV_FMT                      \
    MWPSMOD_EVENT_HDR_FMT                          \
    MWU_KV_FMT_DEC32("status")                     \
    MWU_KV_FMT_MAC("mac")                          \
    MWPSMOD_DEVICE_FMT

#define MWPSMOD_EVENT_DEV_SZ                         \
    (MWPSMOD_EVENT_HDR_SZ +                          \
     MWU_KV_SZ_DEC32("status") +                     \
     MWU_KV_SZ_MAC("mac") +                          \
     MWPSMOD_CMD_DEVICE_SZ)

enum mwu_error mwpsmod_handle_mwu(struct mwu_msg *msg, struct mwu_msg **resp)
{
    int ret, status;
    char iface[IFNAMSIZ + 1];
    char cmd[MWPSMOD_MAX_CMD + 1];

    *resp = NULL;

    INFO("*********************************\n");
    INFO("MWPSMOD module receives message:\n");
    INFO("\n%s", msg->data);
    INFO("*********************************\n\n");
    /* parse the message */
    ret = sscanf(msg->data, MWPSMOD_CMD_HDR_SCAN, iface, cmd);
    if (ret != 2) {
        ERR("Failed to parse cmd message: ret = %d.\n", ret);
        ALLOC_STATUS_MSG_OR_FAIL(*resp, MWPSMOD_ERR_INVAL);
        return MWU_ERR_SUCCESS;
    }

    if (strcmp(cmd, "enrollee_deinit") == 0 ||
        strcmp(cmd, "registrar_deinit") == 0) {

        if (strcmp(iface, mwpsmod.iface) != 0) {
            /* We have not actually been initialized yet.  So just fail early
             * here.
             */
            return MWU_ERR_SUCCESS;
        }
    }

    if (strcmp(iface, mwpsmod.iface) != 0) {
        if (strcmp(cmd, "enrollee_init") != 0 &&
            strcmp(cmd, "registrar_init") != 0 &&
            strcmp(cmd, "registrar_set_pin") != 0) {
            ERR("Got command for uninitialized iface %s\n", iface);
            return MWU_ERR_INVAL;
        }
    }

    /* process command */
    INFO("Got %s command\n", cmd);
    if (strcmp(cmd, "enrollee_init") == 0) {
        struct mwpsmod_enrollee_config cfg;

        /* currently, we only support one module instance at a time */
        if (mwpsmod.iface[0] != 0) {
            ERR("Module already initialized\n");
            ALLOC_STATUS_MSG_OR_FAIL(*resp, MWPSMOD_ERR_BUSY);
            return MWU_ERR_SUCCESS;
        }

        ret = sscanf(msg->data, MWPSMOD_CMD_ENROLLEE_INIT_SCAN, iface, cmd,
                     cfg.dev.device_name, cfg.dev.model_name,
                     cfg.dev.model_number, &cfg.dev.methods);
        if (ret != 6) {
            ERR("init command was missing some key-value pairs (ret=%d).\n",
                ret);
            ALLOC_STATUS_MSG_OR_FAIL(*resp, MWPSMOD_ERR_INVAL);
            return MWU_ERR_SUCCESS;
        }

        /* prepare the module struct */
        strcpy(mwpsmod.iface, iface);
        mwpsmod.cb = mwpsmod_mwu_event_cb;
        mwpsmod.cbpriv = (void *)ENROLLEE;

        status = mwpsmod_enrollee_init(&mwpsmod, &cfg);
        if (status != MWPSMOD_ERR_SUCCESS)
            mwpsmod.iface[0] = 0;
        ALLOC_STATUS_MSG_OR_FAIL(*resp, status);

    } else if (strcmp(cmd, "enrollee_deinit") == 0) {
        if ((int)mwpsmod.cbpriv != ENROLLEE)
            return MWU_ERR_SUCCESS;
        mwpsmod_enrollee_deinit(&mwpsmod);
        mwpsmod.iface[0] = 0;

    } else if (strcmp(cmd, "enrollee_start") == 0) {
        struct mwpsmod_enrollee_info info;

        ret = sscanf(msg->data, MWPSMOD_CMD_ENROLLEE_START_SCAN, iface, cmd,
                     UTIL_MAC2SCAN(info.registrar_mac), info.pin);
        if (ret == 8) {
            /* pin="".  This is legal. */
            strncpy(info.pin, "00000000", MWPSMOD_PIN_LEN);
            info.pin[MWPSMOD_PIN_LEN] = 0;

        } else if (ret != 9) {
            ERR("start command was missing some key-value pairs (ret=%d).\n",
                ret);
            ALLOC_STATUS_MSG_OR_FAIL(*resp, MWPSMOD_ERR_INVAL);
            return MWU_ERR_SUCCESS;
        }
        INFO("Starting enrollee with " UTIL_MACSTR "\n",
             UTIL_MAC2STR(info.registrar_mac));
        status = mwpsmod_enrollee_start(&mwpsmod, &info);
        ALLOC_STATUS_MSG_OR_FAIL(*resp, status);

    } else if (strcmp(cmd, "registrar_init") == 0) {
        struct mwpsmod_registrar_config cfg;

        /* currently, we only support one module instance at a time */
        if (mwpsmod.iface[0] != 0) {
            ERR("Module already initialized\n");
            ALLOC_STATUS_MSG_OR_FAIL(*resp, MWPSMOD_ERR_BUSY);
            return MWU_ERR_SUCCESS;
        }

        ret = sscanf(msg->data, MWPSMOD_CMD_REG_INIT_SCAN, iface, cmd,
                     cfg.dev.device_name, cfg.dev.model_name,
                     cfg.dev.model_number, &cfg.dev.methods,
                     cfg.cred.ssid, &cfg.cred.auth, &cfg.cred.encrypt,
                     cfg.cred.key);
        if (ret != 10) {
            ERR("init command was missing some key-value pairs (ret=%d).\n",
                ret);
            ALLOC_STATUS_MSG_OR_FAIL(*resp, MWPSMOD_ERR_INVAL);
            return MWU_ERR_SUCCESS;
        }

        /* prepare the module struct */
        strcpy(mwpsmod.iface, iface);
        mwpsmod.cb = mwpsmod_mwu_event_cb;
        mwpsmod.cbpriv = (void *)REGISTRAR;

        status = mwpsmod_registrar_init(&mwpsmod, &cfg);
        if (status != MWPSMOD_ERR_SUCCESS)
            mwpsmod.iface[0] = 0;
        ALLOC_STATUS_MSG_OR_FAIL(*resp, status);

    } else if (strcmp(cmd, "registrar_deinit") == 0) {
        if ((int)mwpsmod.cbpriv != REGISTRAR)
            return MWU_ERR_SUCCESS;
        mwpsmod_registrar_deinit(&mwpsmod);
        mwpsmod.iface[0] = 0;

    } else if (strcmp(cmd, "registrar_start") == 0) {
        status = mwpsmod_registrar_start(&mwpsmod);
        ALLOC_STATUS_MSG_OR_FAIL(*resp, status);

    } else if (strcmp(cmd, "registrar_stop") == 0) {
        mwpsmod_registrar_stop(&mwpsmod);

    } else if (strcmp(cmd, "registrar_set_pin") == 0) {
        char pin[MWPSMOD_PIN_LEN + 1];

        ret = sscanf(msg->data, MWPSMOD_CMD_REG_SET_PIN_SCAN,
                     iface, cmd, pin);
        if (ret == 2) {
            /* pin="".  This means to reset PIN/PBC configuration. */
            pin[0] = 0;
        } else if (ret != 3) {
            ERR("set_pin command was missing some key-value pairs (ret=%d).\n",
                ret);
            ALLOC_STATUS_MSG_OR_FAIL(*resp, MWPSMOD_ERR_INVAL);
            return MWU_ERR_SUCCESS;
        }
        status = mwpsmod_registrar_set_pin(&mwpsmod, pin);
        ALLOC_STATUS_MSG_OR_FAIL(*resp, status);

    } else {
        ERR("Got unknown command: %s\n", cmd);
        ALLOC_STATUS_MSG_OR_FAIL(*resp, MWPSMOD_ERR_INVAL);
    }

    return MWU_ERR_SUCCESS;

fail:
    if (*resp != NULL)
        free(*resp);
    return ret;
}

static struct mwu_module mwpsmod_mod = {
    .name = MWPSMOD_MOD_NAME,
    .msg_cb = mwpsmod_handle_mwu,
    .msg_free = NULL,
};

enum mwu_error mwpsmod_mwu_launch(void)
{
    int ret;
    if (mwpsmod.iface[0] != 0)
        return MWU_ERR_BUSY;

    return mwu_internal_register_module(&mwpsmod_mod);
}

void mwpsmod_mwu_halt(void)
{
    mwu_internal_unregister_module(&mwpsmod_mod);
    mwpsmod.iface[0] = 0;
}

void mwpsmod_mwu_event_cb(struct event *event, void *priv)
{

    struct mwu_msg *msg_event = NULL;
    struct mwpsmod_credential *cred;
    struct mwpsmod_device *dev;
    int ret;

    if (!event) {
        ERR("unexpected NULL event.\n");
        return;
    }

    switch (event->type) {
    case MWPSMOD_EVENT_ENROLLEE_DONE:
        INFO("Got enrollee done event.\n");
        cred = (struct mwpsmod_credential *)event->val;
        ALLOC_MSG_OR_FAIL(msg_event, MWPSMOD_EVENT_CRED_SZ + 1,
                          MWPSMOD_EVENT_CRED_FMT,
                          mwpsmod.iface, "enrollee_done",
                          event->status, cred->ssid, cred->auth, cred->encrypt,
                          cred->key);
        break;

    case MWPSMOD_EVENT_REGISTRAR_DONE:
        INFO("Got registrar done event.\n");
        dev = (struct mwpsmod_device *)event->val;
        ALLOC_MSG_OR_FAIL(msg_event, MWPSMOD_EVENT_DEV_SZ + 1,
                          MWPSMOD_EVENT_DEV_FMT,
                          mwpsmod.iface, "registrar_done",
                          event->status, UTIL_MAC2STR(dev->mac),
                          dev->device_name, dev->model_name, dev->model_number,
                          dev->methods);
        break;

    default:
        ERR("Event type %d is not supported.\n", event->type);
        break;
    }

    if (msg_event) {
        mwu_internal_send(msg_event);
        free(msg_event);
    }
fail:
    return;
}
