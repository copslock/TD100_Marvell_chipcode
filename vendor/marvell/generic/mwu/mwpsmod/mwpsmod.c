/* mwpsmod.c: wps module
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
 */

#include <ctype.h>

#include "mwpsmod.h"
#define UTIL_LOG_TAG "MWPSMOD"
#include "util.h"
#include "wps_msg.h"
#include "mwu_log.h"
#include "mwu_ioctl.h"
#include "util.h"
#include "mwu.h"

/* TODO: For now, we depend on these wpswfd data structures, which has wps and
 * wifidir data mixed in.  Not sure how we're going to divorce the wifidir
 * features out of this.
 */
#include "wps_os.h"
#include "wps_def.h"
#include "wps_msg.h"
#include "wps_wlan.h"
extern PWPS_INFO gpwps_info;

extern short ie_index;
extern short ap_assocresp_ie_index;
extern short probe_ie_index;
extern short assocreq_ie_index;

/* we currently only support one module instance at a time.  Note that this
 * should be static.  But wpswfd relies on accessing this to perform callbacks
 * at this time.
 */
struct module *mwpsmod;

#define MWPSMOD_CHECK_MODULE(mod) do {                                  \
        if (mod == NULL || mod->iface[0] == 0 || mwpsmod != mod)        \
            return MWPSMOD_ERR_INVAL;                                   \
    } while (0)

#define MWPSMOD_CHECK_MODULE_INIT(mod) do {                             \
        if (mwpsmod != NULL) {                                          \
            ERR("Only one instance of mwpsmod is supported at this time\n"); \
            return MWPSMOD_ERR_BUSY;                                    \
        }                                                               \
        if (mod == NULL || mod->iface[0] == 0)                          \
            return MWPSMOD_ERR_INVAL;                                   \
        mwpsmod = mod;                                                  \
    } while (0)

/**
 *  Check whether pin is valid. A valid pin should be a 8-digit null-terminated
 *  string.
 */
static int is_valid_pin(const char *wps_pin)
{
    int i;

    if (!wps_pin || strlen(wps_pin) != MWPSMOD_PIN_LEN)
        return FALSE;

    for (i = 0; i < MWPSMOD_PIN_LEN; ++i) {
        if (!isdigit(wps_pin[i]))
            return FALSE;
    }

    return TRUE;
}

static void mwpsmod_reset_pin(PWPS_INFO pwps_info)
{
    pwps_info->PINLen = 0;
    memset(pwps_info->PIN, 0, MWPSMOD_PIN_LEN);
    pwps_info->pin_pbc_set = WPS_CANCEL;
}

/* Internal function to set the pin for wps. This code is poached from the
 * wps_set_device_password_id and wps_registrar_input_PIN.  We can't really use
 * those functions because they mix UI state and back end initialization code.
 */
static void mwpsmod_set_pin(PWPS_INFO pwps_info, char *wps_pin)
{
    if (memcmp(wps_pin, "00000000", MWPSMOD_PIN_LEN) == 0) {
        pwps_info->enrollee.device_password_id = DEVICE_PASSWORD_PUSH_BUTTON;
        pwps_info->registrar.device_password_id = DEVICE_PASSWORD_PUSH_BUTTON;
        pwps_info->enrollee.updated_device_password_id =
            DEVICE_PASSWORD_PUSH_BUTTON;
        pwps_info->registrar.updated_device_password_id =
            DEVICE_PASSWORD_PUSH_BUTTON;
    } else {
        pwps_info->enrollee.device_password_id = DEVICE_PASSWORD_ID_PIN;
        pwps_info->registrar.device_password_id = DEVICE_PASSWORD_ID_PIN;
        pwps_info->enrollee.updated_device_password_id =
            DEVICE_PASSWORD_ID_PIN;
        pwps_info->registrar.updated_device_password_id =
            DEVICE_PASSWORD_ID_PIN;
    }
    pwps_info->PINLen = MWPSMOD_PIN_LEN;
    memcpy(pwps_info->PIN, wps_pin, MWPSMOD_PIN_LEN);
    pwps_info->pin_pbc_set = WPS_SET;


    /* This bit is some insurance against the old interactive text UI that is
     * sprinkled throughout the code.
     */
    pwps_info->input_state = WPS_INPUT_STATE_NO_INPUT;
}

enum mwpsmod_error mwpsmod_launch(void)
{
    return MWPSMOD_ERR_SUCCESS;
}

void mwpsmod_halt(void)
{
    return;
}

enum mwpsmod_error mwpsmod_enrollee_init(struct module *mod,
                                         struct mwpsmod_enrollee_config *cfg)
{
    MWPSMOD_CHECK_MODULE_INIT(mod);

    /* TODO: Currently, we depend on some global data that is poked by many
     * other modules (wifidir and underlying wpswfd).
     */
    return MWPSMOD_ERR_SUCCESS;
}

/* This is a common deinit function for both enrollee and registrar.  There
 * does not seem to be a proper API to do this.  We borrowed this code from
 * wps_txTimer_handler and wps_registration_time_handler.
 */
void wps_reset_wps_state(WPS_INFO * pwps_info);
static void mwpsmod_deinit(void)
{
    INFO("Cancelling registration timeout\n");
    wps_cancel_timer(wps_registration_time_handler, gpwps_info);

    INFO("Cancelling re-Tx timeout\n");
    wps_cancel_timer(wps_txTimer_handler, gpwps_info);
    gpwps_info->set_timer = WPS_CANCEL;
    gpwps_info->wps_msg_resent_count = 0;

    gpwps_info->restart_link_lost = WPS_CANCEL;
    gpwps_info->restart_by_M2D = WPS_CANCEL;

    gpwps_info->state = WPS_STATE_A;
    gpwps_info->id = 1;

    /* Reset Public keys and E-Hash, R-Hash */
    INFO("Resetting WPS state\n");
    wps_reset_wps_state(gpwps_info);

    gpwps_info->last_recv_wps_msg = -1;
    gpwps_info->wps_msg_resent_count = 0;
    gpwps_info->wps_data.wifidir_data.current_peer = NULL;
    gpwps_info->eap_msg_sent = WPS_EAPOL_MSG_START;

    mwpsmod_reset_pin(gpwps_info);
}

void mwpsmod_enrollee_deinit(struct module *mod)
{
    if (mwu_set_deauth(mod->iface) != MWU_ERR_SUCCESS)
        ERR("Failed to deauth.");
    mwpsmod_registrar_stop(mod);
    mwpsmod_deinit();
    mwpsmod = NULL;

    /* Clear Probe request IE */
    if(probe_ie_index != -1) {
        wps_wlan_probe_request_ie_config(CLEAR_WPS_IE, &probe_ie_index);
        probe_ie_index = -1;
    }

    /* Clear Assoc request IE */
    if(assocreq_ie_index != -1) {
        wps_wlan_assoc_request_ie_config(CLEAR_WPS_IE, &assocreq_ie_index);
        assocreq_ie_index = -1;
    }

    return;
}

enum mwpsmod_error mwpsmod_enrollee_start(struct module *mod,
                                  struct mwpsmod_enrollee_info *enrollee_info)
{
    int ret = WPS_STATUS_SUCCESS;
    short assocreq_ie_index = -1;

    MWPSMOD_CHECK_MODULE(mod);

    if (!is_valid_pin(enrollee_info->pin)) {
        ERR("Invalid pin.\n");
        return MWPSMOD_ERR_INVAL;
    }

    mwpsmod_set_pin(gpwps_info, enrollee_info->pin);

    /* TODO: This not only launches the state machine.  It blocks until
     * completion making everything unresponsive.  Most of this is scanning for
     * the registrar.  We really want the enrollee state machine to be more
     * asynchronous.
     */
    memcpy(gpwps_info->registrar.mac_address, enrollee_info->registrar_mac,
           ETH_ALEN);

    if (gpwps_info->enrollee.version >= WPS_VERSION_2DOT0) {
        mwu_printf(DEBUG_INIT, "GET_ASSOC_REQ_WPS_IE\n");

        ret = wps_wlan_assoc_request_ie_config(GET_WPS_IE, &assocreq_ie_index);

        if (ret != WPS_STATUS_SUCCESS) {
            mwu_printf(MSG_ERROR, "Failed to set STA Assoc Req IEs\n");
            return MWPSMOD_ERR_COM;
        }

        /* Update Assoc request IE */
        mwu_printf(DEBUG_INIT, "SET_WPS_STA_SESSION_ACTIVE_IE \n");

        ret = wps_wlan_assoc_request_ie_config(SET_WPS_STA_SESSION_ACTIVE_IE,
                        &assocreq_ie_index);

        if (ret != WPS_STATUS_SUCCESS) {
            mwu_printf(MSG_ERROR, "Failed to set STA IE's\n");
            return MWPSMOD_ERR_COM;
        }
    }

    if (wps_enrollee_start(gpwps_info, &gpwps_info->wps_data) != WPS_STATUS_SUCCESS) {
        ERR("Failed to start WPS enrollee.\n");

        /* CLEAR ASSOC REQ IE */
        if (gpwps_info->enrollee.version >= WPS_VERSION_2DOT0) {

            mwu_printf(MSG_ERROR, "CLEAR_WPS_IE \n");

            ret = wps_wlan_assoc_request_ie_config(CLEAR_WPS_IE,
                        &assocreq_ie_index);

            if (ret != WPS_STATUS_SUCCESS) {
                mwu_printf(MSG_ERROR, "Failed to clear Assoc Request IE's\n");
                return MWPSMOD_ERR_COM;
            }
        }
        return MWPSMOD_ERR_COM;
    }

    /* CLEAR ASSOC REQ IE */
    if (gpwps_info->enrollee.version >= WPS_VERSION_2DOT0) {

        mwu_printf(MSG_ERROR, "CLEAR_WPS_IE \n");

        ret = wps_wlan_assoc_request_ie_config(CLEAR_WPS_IE,
                    &assocreq_ie_index);

        if (ret != WPS_STATUS_SUCCESS) {
            mwu_printf(MSG_ERROR, "Failed to clear Assoc Request IE's\n");
            return MWPSMOD_ERR_COM;
        }
    }

    return MWPSMOD_ERR_SUCCESS;
}

void mwpsmod_enrollee_stop(struct module *mod)
{
    return;
}

enum mwpsmod_error mwpsmod_registrar_init(struct module *mod,
                                      struct mwpsmod_registrar_config *cfg)
{
    MWPSMOD_CHECK_MODULE_INIT(mod);

    /* Not all features are implemented yet.  So fail if we don't support
     * something, and warn loudly when we do something unspecified.
     */
    if (!cfg) {
        ERR("Default config not supported yet.  cfg must not be NULL.\n");
        return WIFIDIR_ERR_UNSUPPORTED;
    }

    /* Currently, we depend on some global data that is poked by many other
     * modules (wifidir and underlying wpswfd).  Eliminating the use of these
     * global structures is quite a project.  For now, just warn that we are
     * taking these values from elsewhere.
     */
    WARN("credential info and registrar configuration taken from global config, "
         "not cfg argument\n");

    return MWPSMOD_ERR_SUCCESS;
}

void mwpsmod_registrar_deinit(struct module *mod)
{
    mwpsmod_registrar_stop(mod);
    mwpsmod_deinit();

    /* Set Inactive IE's */
    if(ie_index != -1) {
        wps_wlan_ie_config(SET_WPS_AP_SESSION_INACTIVE_IE, &ie_index);
    }

    if(ap_assocresp_ie_index != -1) {
        wps_wlan_ie_config(SET_WPS_AP_SESSION_INACTIVE_AR_IE, &ap_assocresp_ie_index);
    }

    mwpsmod = NULL;
}

enum mwpsmod_error mwpsmod_registrar_start(struct module *mod)
{
    MWPSMOD_CHECK_MODULE(mod);

    if (wps_registrar_start(gpwps_info, &gpwps_info->wps_data) != WPS_STATUS_SUCCESS) {
        ERR("Failed to start WPS registrar.\n");
        return MWPSMOD_ERR_COM;
    }
    return MWPSMOD_ERR_SUCCESS;
}

void mwpsmod_registrar_stop(struct module *mod)
{
    return;
}

enum mwpsmod_error mwpsmod_registrar_set_pin(struct module *mod, char *pin)
{
    int ret = -1;

    if (pin && strlen(pin) == 0) {
        /* get the free ies */
        if (ie_index == -1) {
            ret = wps_wlan_ie_config(GET_WPS_IE,  &ie_index);
            if (ret != WPS_STATUS_SUCCESS) {
                ERR("Unable to get free ie index");
                return MWPSMOD_ERR_COM;
            }
        }

        /* Set the AP session inactive IE here*/
        ret = wps_wlan_ie_config(SET_WPS_AP_SESSION_INACTIVE_IE, &ie_index);
        if (ret != WPS_STATUS_SUCCESS) {
            ERR("Failed to set WPS AP SESSION ACTIVE IE");
            return MWPSMOD_ERR_COM;
        }

        if (gpwps_info->registrar.version >= WPS_VERSION_2DOT0) {
            if (ap_assocresp_ie_index == -1) {
                /* Get the free IE buffer index */
                ret = wps_wlan_ie_config(GET_WPS_IE, &ap_assocresp_ie_index);
                if (ret != WPS_STATUS_SUCCESS) {
                    ERR("WPS Registrar failed to get assoc resp IE index");
                    return MWPSMOD_ERR_COM;
                }
            }

            /* Set the AP session inactive AR IE here*/
            ret = wps_wlan_ie_config(SET_WPS_AP_SESSION_INACTIVE_AR_IE, &ap_assocresp_ie_index);
            if (ret != WPS_STATUS_SUCCESS) {
                ERR("Failed to set WPS AP SESSION INACTIVE AR IE");
                return MWPSMOD_ERR_COM;
            }
        }

        mwpsmod_reset_pin(gpwps_info);
        return MWPSMOD_ERR_SUCCESS;
    }

    MWPSMOD_CHECK_MODULE(mod);

    if (!is_valid_pin(pin)) {
        ERR("Invalid pin.\n");
        return MWPSMOD_ERR_INVAL;
    }

    mwpsmod_set_pin(gpwps_info, pin);

    /* get the free ies */
    if (ie_index == -1) {
        ret = wps_wlan_ie_config(GET_WPS_IE,  &ie_index);
        if (ret != WPS_STATUS_SUCCESS) {
            ERR("Unable to get free ie index");
            return MWPSMOD_ERR_COM;
        }
    }

    /* Set the AP session active IE here*/
    ret = wps_wlan_ie_config(SET_WPS_AP_SESSION_ACTIVE_IE, &ie_index);
    if (ret != WPS_STATUS_SUCCESS) {
        ERR("Failed to set WPS AP SESSION ACTIVE IE");
        return MWPSMOD_ERR_COM;
    }

    if (gpwps_info->registrar.version >= WPS_VERSION_2DOT0) {
        if (ap_assocresp_ie_index == -1) {
            /* Get the free IE buffer index */
            ret = wps_wlan_ie_config(GET_WPS_IE, &ap_assocresp_ie_index);
            if (ret != WPS_STATUS_SUCCESS) {
                ERR("WPS Registrar failed to get assoc resp IE index");
                return MWPSMOD_ERR_COM;
            }
        }

        /* Set the AP session active AR IE here*/
        ret = wps_wlan_ie_config(SET_WPS_AP_SESSION_ACTIVE_AR_IE, &ap_assocresp_ie_index);
        if (ret != WPS_STATUS_SUCCESS) {
            ERR("Failed to set WPS AP SESSION ACTIVE AR IE");
            return MWPSMOD_ERR_COM;
        }
    }

    return MWPSMOD_ERR_SUCCESS;
}

void mwpsmod_random_pin(char *pin)
{
    return;
}
