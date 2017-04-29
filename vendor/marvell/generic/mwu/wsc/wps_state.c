/** @file wps_state.c
 *  @brief This file contains functions for WPS state machine.
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
 *
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#define UTIL_LOG_TAG "WPSWIFIDIR"
#include "wps_msg.h"
#include "mwu_log.h"
#include "mwu_timer.h"
#include "wps_def.h"
#include "wps_wlan.h"
#include "wps_os.h"
#include "wps_eapol.h"

#include "mwpsmod/mwpsmod.h"
#include "util.h"

int wps_registrar_state_check(PWPS_INFO pwps_info, u16 msg_type);
int wps_registrar_state_transition(PWPS_INFO pwps_info, u16 msg_type);
void wps_reset_wps_state(WPS_INFO * pwps_info);

extern int wps_install_current_ipv4_cfg(char *ifname, u32 ip_addr,
                                        u32 subnet_mask);

/********************************************************
        Local Variables
********************************************************/
static int (*wps_msg_process[17]) (PWPS_INFO, u8 *, u16) = {
wps_dummy,
        wps_dummy,
        wps_eap_response_identity_process,
        wps_eap_request_start_process,
        wps_eap_M1_frame_process,
        wps_eap_M2_frame_process,
        wps_eap_M2D_frame_process,
        wps_eap_M3_frame_process,
        wps_eap_M4_frame_process,
        wps_eap_M5_frame_process,
        wps_eap_M6_frame_process,
        wps_eap_M7_frame_process,
        wps_eap_M8_frame_process,
        wps_ack_message_process,
        wps_nack_message_process, wps_done_message_process, wps_dummy};

static int (*wps_msg_prepare[18]) (PWPS_INFO) = {
wps_eapol_start_prepare,
        wps_eap_request_identity_prepare,
        wps_eap_response_identity_prepare,
        wps_eap_request_start_prepare,
        wps_eap_M1_frame_prepare,
        wps_eap_M2_frame_prepare,
        wps_eap_M2D_frame_prepare,
        wps_eap_M3_frame_prepare,
        wps_eap_M4_frame_prepare,
        wps_eap_M5_frame_prepare,
        wps_eap_M6_frame_prepare,
        wps_eap_M7_frame_prepare,
        wps_eap_M8_frame_prepare,
        wps_ack_message_prepare,
        wps_nack_message_remap,
        wps_done_message_prepare, wps_eap_fail_frame_prepare,
        wps_eap_frag_ack_frame_prepare};

/********************************************************
        Global Variables
********************************************************/
extern int wps_eapol_txStart(void);
/** IE buffer index */
extern short ie_index;

/** IE buffer index */
extern short ap_assocresp_ie_index;
/** Autonomous GO flag */
extern int auto_go;

/********************************************************
        Local Functions
********************************************************/
/**
 *  @brief Print WPS message type information for debugging
 *
 *  @param str      Print string
 *  @param msg      Message type
 *  @return         None
 */
static void
wps_debug_print_msgtype(const char *str, int msg)
{
    ENTER();

    switch (msg) {
    case WPS_EAPOL_MSG_START:
        printf("%s: EAPOL_START\n", str);
        break;
    case WPS_EAP_MSG_REQUEST_IDENTITY:
        printf("%s: EAP_REQUEST_IDENTITY\n", str);
        break;
    case WPS_EAP_MSG_RESPONSE_IDENTITY:
        printf("%s: EAP_RESPONSE_IDENTITY\n", str);
        break;
    case WPS_EAP_START:
        printf("%s: WPS_START\n", str);
        break;
    case WPS_EAP_M1:
        printf("%s: WPS_M1\n", str);
        break;
    case WPS_EAP_M2:
        printf("%s: WPS_M2\n", str);
        break;
    case WPS_EAP_M2D:
        printf("%s: WPS_M2D\n", str);
        break;
    case WPS_EAP_M3:
        printf("%s: WPS_M3\n", str);
        break;
    case WPS_EAP_M4:
        printf("%s: WPS_M4\n", str);
        break;
    case WPS_EAP_M5:
        printf("%s: WPS_M5\n", str);
        break;
    case WPS_EAP_M6:
        printf("%s: WPS_M6\n", str);
        break;
    case WPS_EAP_M7:
        printf("%s: WPS_M7\n", str);
        break;
    case WPS_EAP_M8:
        printf("%s: WPS_M8\n", str);
        break;
    case WPS_MSG_DONE:
        printf("%s: WPS_DONE\n", str);
        break;
    case WPS_MSG_ACK:
        printf("%s: WPS_ACK\n", str);
        break;
    case WPS_MSG_NACK:
        printf("%s: WPS_NACK\n", str);
        break;
    case WPS_EAP_MSG_FAIL:
        printf("%s: EAP_FAIL\n", str);
        break;
    case WPS_MSG_FRAG_ACK:
        printf("%s: WPS_FRAG_ACK\n", str);
        break;
    default:
        printf("%s: ERROR - UNKNOWN TYPE !\n", str);
        break;
    }

    LEAVE();
}

/**
 *  @brief Print WPS state information for debugging
 *
 *  @param str      Print string
 *  @param state    State information
 *  @return         None
 */
static void
wps_debug_print_state(const char *str, int state)
{
    ENTER();

    switch (state) {
    case WPS_STATE_A:
        mwu_printf(DEBUG_WPS_STATE, "%s = A", str);
        break;
    case WPS_STATE_B:
        mwu_printf(DEBUG_WPS_STATE, "%s = B", str);
        break;
    case WPS_STATE_C:
        mwu_printf(DEBUG_WPS_STATE, "%s = C", str);
        break;
    case WPS_STATE_D:
        mwu_printf(DEBUG_WPS_STATE, "%s = D", str);
        break;
    case WPS_STATE_E:
        mwu_printf(DEBUG_WPS_STATE, "%s = E", str);
        break;
    case WPS_STATE_F:
        mwu_printf(DEBUG_WPS_STATE, "%s = F", str);
        break;
    case WPS_STATE_G:
        mwu_printf(DEBUG_WPS_STATE, "%s = G", str);
        break;
    case WPS_STATE_H:
        mwu_printf(DEBUG_WPS_STATE, "%s = H", str);
        break;
    case WPS_STATE_I:
        mwu_printf(DEBUG_WPS_STATE, "%s = I", str);
        break;
    default:
        mwu_printf(DEBUG_WPS_STATE, "%s = Unknown WPS State", str);
        break;
    }

    LEAVE();
}

/**
 *  @brief Extract the message type field from EAP packet
 *
 *  @param peap     A pointer to EAP frame header
 *  @return         Message type
 */
static int
wps_get_message_type(PEAP_FRAME_HEADER peap)
{
    u8 *buffer;
    int msg_type = 0;
    PTLV_DATA_HEADER ptlv;
    int type, length;

    ENTER();

    switch (peap->op_code) {
    case WPS_Start:
        msg_type = WPS_EAP_START;
        break;
    case WPS_Ack:
    case WPS_Nack:
    case WPS_Msg:
    case WPS_Done:
        buffer = (u8 *) peap;
        buffer += SZ_EAP_WPS_FRAME_HEADER;
        ptlv = (PTLV_DATA_HEADER) buffer;
        length = mwu_ntohs(ptlv->length);
        buffer += (SZ_TLV_HEADER + length);
        ptlv = (PTLV_DATA_HEADER) buffer;
        type = mwu_ntohs(ptlv->type);
        if (type == SC_Message_Type) {
            buffer += SZ_TLV_HEADER;
            msg_type = (u8) * buffer;
        }
        break;
    default:
        mwu_printf(MSG_WARNING, "Unknown opcode: 0x%x", peap->op_code);
        break;
    }

    wps_debug_print_msgtype("RX WPS Message Type", msg_type);

    LEAVE();
    return msg_type;
}

/**
 *  @brief Process Enrollee state machine checking
 *
 *  @param pwps_info    A pointer to WPS_INFO structure
 *  @param msg_type     Message type to be checked
 *  @return             Message type to be sent in current state
 */
static int
wps_enrollee_state_check(PWPS_INFO pwps_info, u16 msg_type)
{
    int msg_next = WPS_EAP_UNKNOWN;
    WPS_DATA *wps_s = (WPS_DATA *) & pwps_info->wps_data;

    ENTER();

    switch (msg_type) {
    case WPS_EAP_MSG_REQUEST_IDENTITY:
        pwps_info->eap_identity_count++;
        if (pwps_info->eap_identity_count < 1)
            msg_next = WPS_EAPOL_MSG_START;
        else if ((pwps_info->state == WPS_STATE_A) ||
                 (pwps_info->state == WPS_STATE_B))
            msg_next = WPS_EAP_MSG_RESPONSE_IDENTITY;
        break;

    case WPS_EAP_MSG_RESPONSE_IDENTITY:
        if (wps_s->bss_type == BSS_TYPE_UAP) {
            if (pwps_info->state == WPS_STATE_B)
                msg_next = WPS_EAP_M1;
        }
        break;

    case WPS_EAP_START:
        pwps_info->eap_identity_count = 0;
        if ((pwps_info->state == WPS_STATE_B) ||
            (pwps_info->state == WPS_STATE_C) ||
            (pwps_info->state == WPS_STATE_D))
            msg_next = WPS_EAP_M1;
        break;
    case WPS_EAP_M2:

        if (wps_s->bss_type == BSS_TYPE_UAP) {
            if (pwps_info->state == WPS_STATE_D)
                msg_next = WPS_EAP_M3;
        } else
            if ((pwps_info->state == WPS_STATE_D) ||
                (pwps_info->state == WPS_STATE_E) ||
                (pwps_info->state == WPS_STATE_F))
            msg_next = WPS_EAP_M3;
        break;
    case WPS_EAP_M2D:
        if ((pwps_info->state == WPS_STATE_D) ||
            (pwps_info->state == WPS_STATE_B) ||
            (pwps_info->state == WPS_STATE_E))
            msg_next = WPS_MSG_ACK;
        break;
    case WPS_EAP_M4:
        if (wps_s->bss_type == BSS_TYPE_UAP) {
            if (pwps_info->state == WPS_STATE_D)
                msg_next = WPS_EAP_M5;
        } else
            if ((pwps_info->state == WPS_STATE_E) ||
                (pwps_info->state == WPS_STATE_F))
            msg_next = WPS_EAP_M5;
        break;
    case WPS_EAP_M6:
        if (wps_s->bss_type == BSS_TYPE_UAP) {
            if (pwps_info->state == WPS_STATE_D)
                msg_next = WPS_EAP_M7;
        } else
            if ((pwps_info->state == WPS_STATE_E) ||
                (pwps_info->state == WPS_STATE_F))
            msg_next = WPS_EAP_M7;
        break;
    case WPS_EAP_M8:
        if (wps_s->bss_type == BSS_TYPE_UAP) {
            if (pwps_info->state == WPS_STATE_D)
                msg_next = WPS_MSG_DONE;
        } else
            if ((pwps_info->state == WPS_STATE_E) ||
                (pwps_info->state == WPS_STATE_F) ||
                (pwps_info->state == WPS_STATE_H))
            msg_next = WPS_MSG_DONE;
        break;
    case WPS_MSG_NACK:
        if (wps_s->bss_type == BSS_TYPE_UAP) {
            if (pwps_info->state == WPS_STATE_F)
                msg_next = WPS_EAP_MSG_FAIL;
        } else if (pwps_info->state == WPS_STATE_I)
            msg_next = WPS_MSG_NACK;
        break;
    case WPS_MSG_ACK:
        if (wps_s->bss_type == BSS_TYPE_UAP) {
            if (pwps_info->state == WPS_STATE_G)
                msg_next = WPS_EAP_MSG_FAIL;
        }
        break;
    case WPS_EAP_MSG_FAIL:
        if ((pwps_info->state == WPS_STATE_A) ||
            (pwps_info->state == WPS_STATE_B))
            msg_next = WPS_EAP_MSG_RESPONSE_IDENTITY;
        break;
    default:
        break;
    }                           /* end of swtich */

    /* wps_debug_print_msgtype("WPS Next Message", msg_next); */

    LEAVE();
    return msg_next;
}

/**
 *  @brief Process Enrollee state machine transition
 *
 *  @param pwps_info    A pointer to WPS_INFO structure
 *  @param msg_type     Message type for triggering state transition
 *  @return             WPS_STATUS_SUCCESS--success, otherwise--fail
 */
static int
wps_enrollee_state_transition(PWPS_INFO pwps_info, u16 msg_type)
{
    WPS_DATA *wps_s = (WPS_DATA *) & pwps_info->wps_data;
    int status = WPS_STATUS_SUCCESS;

    ENTER();

    /* Current WPS State */
    wps_debug_print_state("State Transition Before", pwps_info->state);

    switch (msg_type) {
    case WPS_EAPOL_MSG_START:
        mwu_printf(DEBUG_WPS_STATE, "Event Trigger : EAPOL_START");

        pwps_info->last_recv_wps_msg = -1;

        /* Reset Public keys and E-Hash, R-Hash */
        wps_reset_wps_state(pwps_info);
        break;
    case WPS_EAP_MSG_RESPONSE_IDENTITY:
        mwu_printf(DEBUG_WPS_STATE, "Event Trigger : EAP_RESPONSE_IDENTITY");

        pwps_info->register_completed = WPS_CANCEL;
        pwps_info->registration_in_progress = WPS_SET;
        mwu_printf(DEBUG_WPS_STATE, "Set registration in progress %d",
                   pwps_info->registration_in_progress);

        if (pwps_info->state == WPS_STATE_A) {
            pwps_info->state = WPS_STATE_B;
            mwu_printf(DEBUG_WPS_STATE, "State A->B");
        }
        break;
    case WPS_EAP_START:
        mwu_printf(DEBUG_WPS_STATE, "Event Trigger : WPS_START");

        pwps_info->register_completed = WPS_CANCEL;
        pwps_info->registration_in_progress = WPS_SET;
        mwu_printf(DEBUG_WPS_STATE, "Set registration in progress %d",
                   pwps_info->registration_in_progress);

        if (pwps_info->state == WPS_STATE_A)
            pwps_info->state = WPS_STATE_B;

        if (pwps_info->state == WPS_STATE_B) {
            pwps_info->state = WPS_STATE_C;
            mwu_printf(DEBUG_WPS_STATE, "State B->C");
        }
        break;
    case WPS_EAP_M1:
        mwu_printf(DEBUG_WPS_STATE, "Event Trigger : WPS_M1");
        if (wps_s->bss_type == BSS_TYPE_UAP) {
            if (pwps_info->state == WPS_STATE_B) {
                pwps_info->state = WPS_STATE_C;
                mwu_printf(DEBUG_WPS_STATE, "State B->C");
            }
        } else if (pwps_info->state == WPS_STATE_C) {
            pwps_info->state = WPS_STATE_D;
            mwu_printf(DEBUG_WPS_STATE, "State C->D");
        }
        break;
    case WPS_EAP_M2:
        mwu_printf(DEBUG_WPS_STATE, "Event Trigger : WPS_M2");
        if (wps_s->bss_type == BSS_TYPE_UAP) {
            if (pwps_info->state == WPS_STATE_C) {
                pwps_info->state = WPS_STATE_D;
                mwu_printf(DEBUG_WPS_STATE, "State C->D");
            }
        } else if (pwps_info->state == WPS_STATE_D) {
            pwps_info->state = WPS_STATE_E;
            mwu_printf(DEBUG_WPS_STATE, "State D->E");
        }
        break;
    case WPS_EAP_M2D:
        mwu_printf(DEBUG_WPS_STATE, "Event Trigger : WPS_M2D");
        if (wps_s->bss_type == BSS_TYPE_UAP) {
            if ((pwps_info->state == WPS_STATE_C) ||
                (pwps_info->state == WPS_STATE_D))
                pwps_info->state = WPS_STATE_H;
        } else if ((pwps_info->state == WPS_STATE_D) ||
                   (pwps_info->state == WPS_STATE_B))
                pwps_info->state = WPS_STATE_E;
        break;
    case WPS_EAP_M3:
        mwu_printf(DEBUG_WPS_STATE, "Event Trigger : WPS_M3");
        if (wps_s->bss_type == BSS_TYPE_UAP) {
            if (pwps_info->state == WPS_STATE_D) {
                pwps_info->state = WPS_STATE_E;
                mwu_printf(DEBUG_WPS_STATE, "State D->E");
            }
        } else if (pwps_info->state == WPS_STATE_E) {
            pwps_info->state = WPS_STATE_F;
            mwu_printf(DEBUG_WPS_STATE, "State E->F");
        }
        break;
    case WPS_EAP_M4:
        mwu_printf(DEBUG_WPS_STATE, "Event Trigger : WPS_M4");
        if (wps_s->bss_type == BSS_TYPE_UAP) {
            if (pwps_info->state == WPS_STATE_E) {
                pwps_info->state = WPS_STATE_D;
                mwu_printf(DEBUG_WPS_STATE, "State E->D");
            }
        } else if (pwps_info->state == WPS_STATE_F) {
            pwps_info->state = WPS_STATE_E;
            mwu_printf(DEBUG_WPS_STATE, "State F->E");
        }
        break;
    case WPS_EAP_M5:
        mwu_printf(DEBUG_WPS_STATE, "Event Trigger : WPS_M5");
        if (wps_s->bss_type == BSS_TYPE_UAP) {
            if (pwps_info->state == WPS_STATE_D) {
                pwps_info->state = WPS_STATE_E;
                mwu_printf(DEBUG_WPS_STATE, "State D->E");
            }
        } else if (pwps_info->state == WPS_STATE_E) {
            pwps_info->state = WPS_STATE_F;
            mwu_printf(DEBUG_WPS_STATE, "State E->F");
        }
        break;
    case WPS_EAP_M6:
        mwu_printf(DEBUG_WPS_STATE, "Event Trigger : WPS_M6");
        if (wps_s->bss_type == BSS_TYPE_UAP) {
            if (pwps_info->state == WPS_STATE_E) {
                pwps_info->state = WPS_STATE_D;
                mwu_printf(DEBUG_WPS_STATE, "State E->D");
            }
        } else if (pwps_info->state == WPS_STATE_F) {
            pwps_info->state = WPS_STATE_E;
            mwu_printf(DEBUG_WPS_STATE, "State F->E");
        }
        break;
    case WPS_EAP_M7:
        mwu_printf(DEBUG_WPS_STATE, "Event Trigger : WPS_M7");
        if (wps_s->bss_type == BSS_TYPE_UAP) {
            if (pwps_info->state == WPS_STATE_D) {
                pwps_info->state = WPS_STATE_E;
                mwu_printf(DEBUG_WPS_STATE, "State D->E");
            }
        } else if (pwps_info->state == WPS_STATE_E) {
            pwps_info->state = WPS_STATE_F;
            mwu_printf(DEBUG_WPS_STATE, "State E->F");
        }
        break;
    case WPS_EAP_M8:
        mwu_printf(DEBUG_WPS_STATE, "Event Trigger : WPS_M8");
        pwps_info->register_completed = WPS_SET;
        pwps_info->registration_in_progress = WPS_CANCEL;
        mwu_printf(DEBUG_WPS_STATE, "Set registration in progress %d",
                   pwps_info->registration_in_progress);

        if (wps_s->bss_type == BSS_TYPE_UAP) {
            if (pwps_info->state == WPS_STATE_E) {
                pwps_info->state = WPS_STATE_D;
                mwu_printf(DEBUG_WPS_STATE, "State E->D");
            }
        } else if (pwps_info->state == WPS_STATE_F) {
            pwps_info->state = WPS_STATE_E;
            mwu_printf(DEBUG_WPS_STATE, "State F->E");
        }
        break;
    case WPS_MSG_DONE:
        mwu_printf(DEBUG_WPS_STATE, "Event Trigger : WPS_DONE");
        if (wps_s->bss_type == BSS_TYPE_UAP) {
            if (pwps_info->state == WPS_STATE_D) {
                pwps_info->state = WPS_STATE_E;
                mwu_printf(DEBUG_WPS_STATE, "State D->E");
            }
        } else if (pwps_info->state == WPS_STATE_E) {
            pwps_info->state = WPS_STATE_H;
            mwu_printf(DEBUG_WPS_STATE, "State E->H");
        }
        break;
    case WPS_MSG_ACK:
        mwu_printf(DEBUG_WPS_STATE, "Event Trigger : WPS_ACK");
        if (wps_s->bss_type == BSS_TYPE_UAP) {
            if (pwps_info->state == WPS_STATE_E) {
                pwps_info->state = WPS_STATE_G;
                mwu_printf(DEBUG_WPS_STATE, "State E->G");
            }
        } else if (pwps_info->state == WPS_STATE_E) {
            pwps_info->state = WPS_STATE_D;
            mwu_printf(DEBUG_WPS_STATE, "State E->D");
        }
        break;
    case WPS_MSG_NACK:
        mwu_printf(DEBUG_WPS_STATE, "Event Trigger : WPS_NACK");
        switch (pwps_info->state) {
        case WPS_STATE_D:
            if (wps_s->bss_type == BSS_TYPE_UAP) {
                pwps_info->state = WPS_STATE_H;
                mwu_printf(DEBUG_WPS_STATE, "State D->H");
            }
            break;
        case WPS_STATE_H:
            if (wps_s->bss_type == BSS_TYPE_UAP) {
                pwps_info->state = WPS_STATE_F;
                mwu_printf(DEBUG_WPS_STATE, "State H->F");
            }
            break;
        case WPS_STATE_E:
            if (wps_s->bss_type == BSS_TYPE_UAP) {
                pwps_info->state = WPS_STATE_F;
                mwu_printf(DEBUG_WPS_STATE, "State E->F");
                if (pwps_info->role == WPS_ENROLLEE &&
                    pwps_info->eap_msg_sent == WPS_EAP_M7) {

                    mwu_printf(DEBUG_WPS_MSG,
                               "Setting AP config read only variable.");
                    /* Received Nack after M7. This is AP config read case */
                    pwps_info->read_ap_config_only = WPS_SET;

                }
            } else {
                pwps_info->state = WPS_STATE_G;
                mwu_printf(DEBUG_WPS_STATE, "State E->G");
            }
            break;
        case WPS_STATE_I:
            if (wps_s->bss_type != BSS_TYPE_UAP) {
                pwps_info->state = WPS_STATE_G;
                mwu_printf(DEBUG_WPS_STATE, "State I->G");
            }
            break;
        case WPS_STATE_F:
            if (wps_s->bss_type != BSS_TYPE_UAP) {
                pwps_info->state = WPS_STATE_I;
                mwu_printf(DEBUG_WPS_STATE, "State F->I");
            }
            break;
        default:
            break;
        }
        break;
    case WPS_EAP_MSG_FAIL:
        mwu_printf(DEBUG_WPS_STATE, "Event Trigger : EAP_FAIL");

        pwps_info->last_recv_wps_msg = -1;
        pwps_info->registration_in_progress = WPS_CANCEL;
        mwu_printf(DEBUG_WPS_STATE, "Set registration in progress %d",
                   pwps_info->registration_in_progress);

        switch (pwps_info->state) {
        case WPS_STATE_F:
            if (wps_s->bss_type == BSS_TYPE_UAP) {
                pwps_info->state = WPS_STATE_A;
                mwu_printf(DEBUG_WPS_STATE, "State F->A");
            }
            break;
        case WPS_STATE_D:
            pwps_info->state = WPS_STATE_A;
            mwu_printf(DEBUG_WPS_STATE, "State D->A");
            break;
        case WPS_STATE_G:
            pwps_info->state = WPS_STATE_A;
            mwu_printf(DEBUG_WPS_STATE, "State G->A");
            break;
        case WPS_STATE_H:
            pwps_info->state = WPS_STATE_A;
            mwu_printf(DEBUG_WPS_STATE, "State H->A");
            break;
        case WPS_STATE_I:
            pwps_info->state = WPS_STATE_A;
            mwu_printf(DEBUG_WPS_STATE, "State I->A");
            break;
        default:
            break;
        }

        if (pwps_info->nack_error_count > MAX_NACK_ERROR_COUNT) {
            status = wps_maximum_nack_handler(pwps_info, wps_s);
        }

    default:
        break;

    }                           /* end of switch */

    /* New WPS State */
    wps_debug_print_state("State Transition After", pwps_info->state);

    LEAVE();
    return status;
}

/* This module structure is owned by mwpsmod.  We extern it here for now so we
 * can send callbacks.
 */
extern struct module *mwpsmod;

/* create mwpsmod event and call callback to send user. */
static void send_credential_event(CREDENTIAL_DATA *cred)
{
    struct event *event;
    struct mwpsmod_credential *c;

    event = malloc(sizeof(struct event) +
                   sizeof(struct mwpsmod_credential));
    if (!event) {
        ERR("No memory.  Can't send enrollee done event.\n");
        return;
    }
    memset(event, 0, sizeof(struct event) + sizeof(struct mwpsmod_credential));
    event->type = MWPSMOD_EVENT_ENROLLEE_DONE;
    event->status = MWPSMOD_ERR_SUCCESS;
    event->len = sizeof(struct mwpsmod_credential);
    c = (struct mwpsmod_credential *)event->val;
    memcpy(c->ssid, cred->ssid, cred->ssid_length);
    c->ssid[cred->ssid_length] = 0;
    memcpy(c->key, cred->network_key, cred->network_key_len);
    c->key[cred->network_key_len] = 0;
    c->auth = cred->auth_type;
    c->encrypt = cred->encry_type;
    MODULE_DO_CALLBACK(mwpsmod, event);
    free(event);
}

/* If some error happen during WPS 8-way handshake, send fail event to user.
 * This function is for enrollee.
 */
static void send_enrollee_fail_event()
{
    struct event *event;

    event = malloc(sizeof(struct event) +
                   sizeof(struct mwpsmod_credential));
    if (!event) {
        ERR("No memory.  Can't send enrollee done event.\n");
        return;
    }
    memset(event, 0, sizeof(struct event) + sizeof(struct mwpsmod_credential));
    event->type = MWPSMOD_EVENT_ENROLLEE_DONE;
    event->status = MWPSMOD_ERR_NAK;
    event->len = sizeof(struct mwpsmod_credential);

    MODULE_DO_CALLBACK(mwpsmod, event);
    free(event);
}

/* an enrollee that was attempting to register has finished with the specified
 * status.  For now, we only populate the mac address of the device.
 * Eventually we need the device name, etc., too.
 */
static void send_registrar_done_event(u8 *mac, enum mwpsmod_error status)
{
    struct event *event;
    struct mwpsmod_device *d;

    event = malloc(sizeof(struct event) +
                   sizeof(struct mwpsmod_device));
    if (!event) {
        ERR("No memory.  Can't send registrar done event.\n");
        return;
    }
    memset(event, 0, sizeof(struct event) + sizeof(struct mwpsmod_device));
    event->type = MWPSMOD_EVENT_REGISTRAR_DONE;
    event->status = status;
    event->len = sizeof(struct mwpsmod_device);
    d = (struct mwpsmod_device *)event->val;
    memcpy(d->mac, mac, ETH_ALEN);
    MODULE_DO_CALLBACK(mwpsmod, event);
    free(event);
}

/* If some error happen during WPS 8-way handshake, send fail event to user.
 * This function is for enrollee.
 */
static void send_registrar_fail_event()
{
    struct event *event;

    event = malloc(sizeof(struct event) +
                   sizeof(struct mwpsmod_device));
    if (!event) {
        ERR("No memory.  Can't send registrar done event.\n");
        return;
    }
    memset(event, 0, sizeof(struct event) + sizeof(struct mwpsmod_device));
    event->type = MWPSMOD_EVENT_REGISTRAR_DONE;
    event->status = MWPSMOD_ERR_NAK;
    event->len = sizeof(struct mwpsmod_device);
    MODULE_DO_CALLBACK(mwpsmod, event);
    free(event);
}

/**
 *  @brief Enrollee EAP packet process handling
 *
 *  @param pwps_info    A pointer to WPS_INFO structure
 *  @param peap         A pointer to EAP frame header
 *  @return             WPS_STATUS_SUCCESS--success, otherwise--fail
 */
static int
wps_eap_request_message_handler(PWPS_INFO pwps_info, PEAP_FRAME_HEADER peap)
{
    int status = WPS_STATUS_SUCCESS;
    int msg_next, msg_type;
    WPS_DATA *wps_s = (WPS_DATA *) & pwps_info->wps_data;

    ENTER();

    pwps_info->peer_id = peap->identifier;

    if (peap->type == EAP_TYPE_IDENTITY) {
        mwu_printf(DEBUG_WPS_STATE, "EAP_REQUEST_IDENTITY Received");
        wps_debug_print_msgtype("RX WPS Message Type",
                                WPS_EAP_MSG_REQUEST_IDENTITY);

        if (pwps_info->set_timer == WPS_SET) {
            wps_cancel_timer(wps_txTimer_handler, pwps_info);
            pwps_info->set_timer = WPS_CANCEL;
            pwps_info->wps_msg_resent_count = 0;
        }

        msg_type = WPS_EAP_MSG_REQUEST_IDENTITY;
        pwps_info->last_recv_wps_msg = msg_type;

        if (pwps_info->role == WPS_ENROLLEE
            || (IS_DISCOVERY_ENROLLEE(pwps_info))
            ) {
            msg_next = wps_enrollee_state_check(pwps_info,
                                                WPS_EAP_MSG_REQUEST_IDENTITY);
        } else {
            msg_next = wps_registrar_state_check(pwps_info,
                                                 WPS_EAP_MSG_REQUEST_IDENTITY);
        }
        if (msg_next != WPS_EAP_UNKNOWN)
            status =
                wps_msg_transmit(pwps_info, msg_next,
                                 pwps_info->wps_msg_timeout);
    } else if (peap->type == EAP_TYPE_WPS) {

        switch (peap->op_code) {
        case WPS_Msg:
            {

                msg_type = wps_get_message_type(peap);

                if ((msg_type > pwps_info->last_recv_wps_msg) ||
                    (msg_type == WPS_M2D) ||
                    ((pwps_info->last_recv_wps_msg == WPS_M2D) &&
                     (msg_type == WPS_M2))) {

                    mwu_printf(DEBUG_WPS_STATE, "Last Message received 0x%x",
                               pwps_info->last_recv_wps_msg);
                    mwu_printf(DEBUG_WPS_STATE, "Current message 0x%x",
                               msg_type);

                    if ((msg_type == pwps_info->last_recv_wps_msg)
                        && (msg_type != WPS_M2D)) {
                    /** recevied the retransmitted message
                     *  use the previous transmitted message,
                     *  instead of current tx message
                     */
                        mwu_printf(DEBUG_WPS_STATE,
                                   "Received reTx EAPOL Req message: 0x%x",
                                   msg_type);
                        memcpy(&pwps_info->last_message,
                               &pwps_info->last_tx_message,
                               sizeof(MESSAGE_BUFFER));
                    }

                    pwps_info->last_recv_wps_msg = msg_type;

                } else {
                    printf("rx_msg: Dropping unexpected msg! "
                           "(prev_msg: 0x%x, curr_msg: 0x%x)\n",
                           pwps_info->last_recv_wps_msg, msg_type);
                    break;
                }

                if (pwps_info->set_timer == WPS_SET) {
                    wps_cancel_timer(wps_txTimer_handler, pwps_info);
                    pwps_info->set_timer = WPS_CANCEL;
                    pwps_info->wps_msg_resent_count = 0;
                }

                if (msg_type != WPS_EAP_UNKNOWN)
                    status =
                        (*wps_msg_process[msg_type]) (pwps_info, (u8 *) peap,
                                                      peap->length);
                else
                    status = WPS_STATUS_FAIL;

                if (status == WPS_STATUS_SUCCESS) {

                    if (pwps_info->role == WPS_ENROLLEE
                        || (IS_DISCOVERY_ENROLLEE(pwps_info))
                        ) {
                        wps_enrollee_state_transition(pwps_info, msg_type);
                        msg_next =
                            wps_enrollee_state_check(pwps_info, msg_type);
                    } else {
                        wps_registrar_state_transition(pwps_info, msg_type);
                        msg_next =
                            wps_registrar_state_check(pwps_info, msg_type);
                    }
                    if (msg_next != WPS_EAP_UNKNOWN)
                        status =
                            wps_msg_transmit(pwps_info, msg_next,
                                             pwps_info->wps_msg_timeout);
                    else
                        status =
                            wps_msg_transmit(pwps_info, WPS_MSG_NACK,
                                             pwps_info->wps_msg_timeout);
                } else {
                    status =
                        wps_msg_transmit(pwps_info, WPS_MSG_NACK,
                                         pwps_info->wps_msg_timeout);
                    /* If PINs on Registrar and Enrollee don't match,
                     * no need to keep trying handshake, just send fail
                     * event to user.
                     *
                     * PIN mismatch will be detected when receiving WPS_M4
                     */
                    if (msg_type == WPS_M4)
                        send_enrollee_fail_event();
                }

                break;
            }

        case WPS_Done:
            {
                if (pwps_info->role == WPS_REGISTRAR) {
                    msg_type = wps_get_message_type(peap);
                    status = wps_done_message_process(pwps_info,
                                                      (u8 *) peap,
                                                      peap->length);
                    if (status == WPS_STATUS_SUCCESS) {
                        wps_registrar_state_transition(pwps_info, msg_type);
                        msg_next =
                            wps_registrar_state_check(pwps_info, msg_type);
                        if (msg_next != WPS_EAP_UNKNOWN)
                            status = wps_msg_transmit(pwps_info,
                                                      msg_next,
                                                      pwps_info->
                                                      wps_msg_timeout);
                    } else
                        status = wps_msg_transmit(pwps_info,
                                                  WPS_MSG_NACK,
                                                  pwps_info->wps_msg_timeout);
                }
                break;
            }

        case WPS_Start:
            {
                msg_type = wps_get_message_type(peap);

                if ((msg_type > pwps_info->last_recv_wps_msg) ||
                    (msg_type == WPS_M2D) ||
                    ((pwps_info->last_recv_wps_msg == WPS_M2D) &&
                     (msg_type == WPS_M2))) {

                    mwu_printf(DEBUG_WPS_STATE, "Last Message received 0x%x",
                               pwps_info->last_recv_wps_msg);
                    mwu_printf(DEBUG_WPS_STATE, "Current message 0x%x",
                               msg_type);

                    if ((msg_type == pwps_info->last_recv_wps_msg)
                        && (msg_type != WPS_M2D)) {
                    /** recevied the retransmitted message
                     *  use the previous transmitted message,
                     *  instead of current tx message
                     */
                        mwu_printf(DEBUG_WPS_STATE,
                                   "Received reTx EAPOL Req message: 0x%x",
                                   msg_type);
                        memcpy(&pwps_info->last_message,
                               &pwps_info->last_tx_message,
                               sizeof(MESSAGE_BUFFER));
                    }

                    pwps_info->last_recv_wps_msg = msg_type;

                } else {
                    printf("rx_msg: Dropping unexpected msg! "
                           "(prev_msg: 0x%x, curr_msg: 0x%x)\n",
                           pwps_info->last_recv_wps_msg, msg_type);
                    break;
                }

                if (pwps_info->set_timer == WPS_SET) {
                    wps_cancel_timer(wps_txTimer_handler, pwps_info);
                    pwps_info->set_timer = WPS_CANCEL;
                    pwps_info->wps_msg_resent_count = 0;
                }

                status =
                    wps_eap_request_start_process(pwps_info, (u8 *) peap,
                                                  peap->length);
                if (status == WPS_STATUS_SUCCESS) {

                    wps_enrollee_state_transition(pwps_info, WPS_EAP_START);

                    msg_next =
                        wps_enrollee_state_check(pwps_info, WPS_EAP_START);
                    if (msg_next != WPS_EAP_UNKNOWN)
                        status =
                            wps_msg_transmit(pwps_info, msg_next,
                                             pwps_info->wps_msg_timeout);
                    else
                        status =
                            wps_msg_transmit(pwps_info, WPS_MSG_NACK,
                                             pwps_info->wps_msg_timeout);
                } else {
                    mwu_printf(DEBUG_WPS_STATE, "Process WPS_Start Fail !");
                }

                break;
            }

        case WPS_Ack:
            break;

        case WPS_Nack:
            msg_type = wps_get_message_type(peap);

            status =
                wps_nack_message_process(pwps_info, (u8 *) peap, peap->length);
            if (status == WPS_STATUS_SUCCESS) {

                if (pwps_info->set_timer == WPS_SET) {
                    wps_cancel_timer(wps_txTimer_handler, pwps_info);
                    pwps_info->set_timer = WPS_CANCEL;
                }
                if (pwps_info->role == WPS_ENROLLEE
                    || (IS_DISCOVERY_ENROLLEE(pwps_info))
                    ) {
                    wps_enrollee_state_transition(pwps_info, msg_type);
                    msg_next = wps_enrollee_state_check(pwps_info, msg_type);
                } else {
                    wps_registrar_state_transition(pwps_info, msg_type);
                    msg_next = wps_registrar_state_check(pwps_info, msg_type);
                }
                if (msg_next != WPS_EAP_UNKNOWN)
                    status =
                        wps_msg_transmit(pwps_info, msg_next,
                                         pwps_info->wps_msg_timeout);
            } else
                status =
                    wps_msg_transmit(pwps_info, WPS_MSG_NACK,
                                     pwps_info->wps_msg_timeout);
            break;

        case WPS_Frag_Ack:
            {
                wps_send_next_fragment(pwps_info, wps_s);
                break;
            }
        default:
            break;
        }
    }

    LEAVE();
    return status;
}

/**
 *  @brief Process Registrar state machine checking
 *
 *  @param pwps_info    A pointer to WPS_INFO structure
 *  @param msg_type     Message type to be checked
 *  @return             Message type to be sent in current state
 */
int
wps_registrar_state_check(PWPS_INFO pwps_info, u16 msg_type)
{
    int msg_next = WPS_EAP_UNKNOWN;
    WPS_DATA *wps_s = (WPS_DATA *) & pwps_info->wps_data;

    ENTER();

    switch (msg_type) {

    case WPS_EAP_MSG_REQUEST_IDENTITY:
        if (wps_s->bss_type == BSS_TYPE_STA && pwps_info->role == WPS_REGISTRAR) {
            if ((pwps_info->state == WPS_STATE_A) ||
                (pwps_info->state == WPS_STATE_B))
                msg_next = WPS_EAP_MSG_RESPONSE_IDENTITY;
        }
        break;

    case WPS_EAPOL_MSG_START:
        if (pwps_info->state == WPS_STATE_A)
            msg_next = WPS_EAP_MSG_REQUEST_IDENTITY;
        break;
    case WPS_EAP_MSG_RESPONSE_IDENTITY:
        if ((pwps_info->state == WPS_STATE_A) ||
            (pwps_info->state == WPS_STATE_B) ||
            (pwps_info->state == WPS_STATE_C))
            msg_next = WPS_EAP_START;
        break;
    case WPS_EAP_M1:
        if ((pwps_info->state == WPS_STATE_C) ||
            (pwps_info->state == WPS_STATE_D) ||
            (pwps_info->state == WPS_STATE_E)) {
#ifdef WIFIDIR_SUPPORT
            if (pwps_info->role != WIFIDIR_ROLE) {
#endif
                if ((pwps_info->registrar.updated_device_password_id !=
                     pwps_info->enrollee.device_password_id) ||
                    (pwps_info->pin_pbc_set == WPS_CANCEL)) {
                    mwu_printf(DEBUG_WPS_STATE,
                               "Enrollee and Registrar Device password ID dont match.Registar %d. Enrollee %d",
                               pwps_info->registrar.updated_device_password_id,
                               pwps_info->enrollee.device_password_id);
                    mwu_printf(DEBUG_WPS_STATE, "Pin PBC set value %d",
                               pwps_info->pin_pbc_set);
                    msg_next = WPS_EAP_M2D;
                } else
                    msg_next = WPS_EAP_M2;
#ifdef WIFIDIR_SUPPORT
            } else {
                if (pwps_info->pin_pbc_set == WPS_CANCEL) {

                    mwu_printf(DEBUG_WPS_STATE, "Pin PBC set value %d",
                               pwps_info->pin_pbc_set);
                    msg_next = WPS_EAP_M2D;
                } else {
                    mwu_printf(DEBUG_WPS_STATE,
                               "Ignoring Device password ID check for WIFIDIR");
                    mwu_printf(DEBUG_WPS_STATE,
                               "Enrollee and Registrar Device password ID dont match.Registar %d. Enrollee %d",
                               pwps_info->registrar.updated_device_password_id,
                               pwps_info->enrollee.device_password_id);
                    msg_next = WPS_EAP_M2;
                }
            }
#endif
        }
        break;
    case WPS_EAP_M3:
        if ((pwps_info->state == WPS_STATE_E) ||
            (pwps_info->state == WPS_STATE_F))
            msg_next = WPS_EAP_M4;
        break;
    case WPS_EAP_M5:
        if ((pwps_info->state == WPS_STATE_E) ||
            (pwps_info->state == WPS_STATE_F))
            msg_next = WPS_EAP_M6;
        break;
    case WPS_EAP_M7:
        if (wps_s->bss_type == BSS_TYPE_STA && pwps_info->role == WPS_REGISTRAR) {
            if (pwps_info->state == WPS_STATE_E) {
                if (pwps_info->read_ap_config_only == WPS_SET) {
                    pwps_info->register_completed = WPS_SET;
                    msg_next = WPS_MSG_NACK;
                } else
                    msg_next = WPS_EAP_M8;
            }
        } else {
            if (pwps_info->state == WPS_STATE_F)
                msg_next = WPS_EAP_M8;
        }
        break;
    case WPS_MSG_DONE:
        if (wps_s->bss_type == BSS_TYPE_STA &&
            (pwps_info->role == WPS_REGISTRAR) &&
            pwps_info->state == WPS_STATE_E) {
            msg_next = WPS_MSG_ACK;
        } else if ((pwps_info->state == WPS_STATE_E) ||
                   (pwps_info->state == WPS_STATE_H) ||
                   (pwps_info->state == WPS_STATE_A))
            msg_next = WPS_EAP_MSG_FAIL;
        break;
    case WPS_MSG_ACK:
        if (pwps_info->state == WPS_STATE_D)
            msg_next = WPS_EAP_MSG_FAIL;
        break;
    case WPS_MSG_NACK:
        if ((wps_s->bss_type == BSS_TYPE_STA)
            && (pwps_info->role == WPS_REGISTRAR) &&
            pwps_info->state == WPS_STATE_H) {
            msg_next = WPS_MSG_NACK;
        } else if ((pwps_info->state == WPS_STATE_E) ||
                   (pwps_info->state == WPS_STATE_I) ||
                   (pwps_info->state == WPS_STATE_G))
            msg_next = WPS_EAP_MSG_FAIL;
        break;
    default:
        break;
    }                           /* end of swtich */

    // wps_debug_print_msgtype("WPS Next Message", msg_next);

    LEAVE();
    return msg_next;
}

/**
 *  @brief Process Registrar state machine transition
 *
 *  @param pwps_info    A pointer to WPS_INFO structure
 *  @param msg_type     Message type for triggering state transition
 *  @return             WPS_STATUS_SUCCESS--success, otherwise--fail
 */
int
wps_registrar_state_transition(PWPS_INFO pwps_info, u16 msg_type)
{
    WPS_DATA *wps_s = (WPS_DATA *) & pwps_info->wps_data;
    int status = WPS_STATUS_SUCCESS;

    ENTER();

    /* Current WPS State */
    wps_debug_print_state("State Transition Before", pwps_info->state);

    switch (msg_type) {
    case WPS_EAPOL_MSG_START:
        mwu_printf(DEBUG_WPS_STATE, "Event Trigger : EAPOL_START");

        pwps_info->last_recv_wps_msg = -1;
        /* Reset Public keys and E-Hash, R-Hash */
        wps_reset_wps_state(pwps_info);
        break;
    case WPS_EAP_MSG_RESPONSE_IDENTITY:
        mwu_printf(DEBUG_WPS_STATE, "Event Trigger : EAP_RESPONSE_IDENTITY");

        pwps_info->register_completed = WPS_CANCEL;
        pwps_info->registration_in_progress = WPS_SET;
        mwu_printf(DEBUG_WPS_STATE, "Set registration in progress %d",
                   pwps_info->registration_in_progress);

        if (pwps_info->state == WPS_STATE_A) {
            pwps_info->state = WPS_STATE_B;
            mwu_printf(DEBUG_WPS_STATE, "State A->B");
        }
        break;
    case WPS_EAP_START:
        mwu_printf(DEBUG_WPS_STATE, "Event Trigger : WPS_START");
        if (pwps_info->state == WPS_STATE_B) {
            pwps_info->state = WPS_STATE_C;
            mwu_printf(DEBUG_WPS_STATE, "State B->C");
        }
        break;
    case WPS_EAP_M1:
        mwu_printf(DEBUG_WPS_STATE, "Event Trigger : WPS_M1");

        if (wps_s->bss_type == BSS_TYPE_STA && pwps_info->role == WPS_REGISTRAR) {
            if (pwps_info->state == WPS_STATE_B) {
                pwps_info->state = WPS_STATE_C;
                mwu_printf(DEBUG_WPS_STATE, "State B->C");
            }
        } else if (pwps_info->state == WPS_STATE_C) {
            pwps_info->state = WPS_STATE_D;
            mwu_printf(DEBUG_WPS_STATE, "State C->D");
        }
        break;
    case WPS_EAP_M2:
        mwu_printf(DEBUG_WPS_STATE, "Event Trigger : WPS_M2");
        if (wps_s->bss_type == BSS_TYPE_STA && pwps_info->role == WPS_REGISTRAR) {
            if (pwps_info->state == WPS_STATE_C) {
                pwps_info->state = WPS_STATE_D;
                mwu_printf(DEBUG_WPS_STATE, "State C->D");
            }
        } else if (pwps_info->state == WPS_STATE_D) {
            pwps_info->state = WPS_STATE_E;
            mwu_printf(DEBUG_WPS_STATE, "State D->E");
        }
        break;
    case WPS_EAP_M2D:
        mwu_printf(DEBUG_WPS_STATE, "Event Trigger : WPS_M2D");
        if (wps_s->bss_type == BSS_TYPE_STA && pwps_info->role == WPS_REGISTRAR) {
            if (pwps_info->state == WPS_STATE_C) {
                pwps_info->state = WPS_STATE_D;
                mwu_printf(DEBUG_WPS_STATE, "State C->D");
            }
        } else if (pwps_info->state == WPS_STATE_D)
            pwps_info->state = WPS_STATE_E;
        break;
    case WPS_EAP_M3:
        mwu_printf(DEBUG_WPS_STATE, "Event Trigger : WPS_M3");
        if (wps_s->bss_type == BSS_TYPE_STA && pwps_info->role == WPS_REGISTRAR) {
            if (pwps_info->state == WPS_STATE_D) {
                pwps_info->state = WPS_STATE_E;
                mwu_printf(DEBUG_WPS_STATE, "State D->E");
            }
        } else if (pwps_info->state == WPS_STATE_E) {
            pwps_info->state = WPS_STATE_F;
            mwu_printf(DEBUG_WPS_STATE, "State E->F");
        }
        break;
    case WPS_EAP_M4:
        mwu_printf(DEBUG_WPS_STATE, "Event Trigger : WPS_M4");
        if (wps_s->bss_type == BSS_TYPE_STA && pwps_info->role == WPS_REGISTRAR) {
            if (pwps_info->state == WPS_STATE_E) {
                pwps_info->state = WPS_STATE_D;
                mwu_printf(DEBUG_WPS_STATE, "State E->D");
            }
        } else if (pwps_info->state == WPS_STATE_F) {
            pwps_info->state = WPS_STATE_E;
            mwu_printf(DEBUG_WPS_STATE, "State F->E");
        }
        break;
    case WPS_EAP_M5:
        mwu_printf(DEBUG_WPS_STATE, "Event Trigger : WPS_M5");
        if (wps_s->bss_type == BSS_TYPE_STA && pwps_info->role == WPS_REGISTRAR) {
            if (pwps_info->state == WPS_STATE_D) {
                pwps_info->state = WPS_STATE_E;
                mwu_printf(DEBUG_WPS_STATE, "State D->E");
            }
        } else if (pwps_info->state == WPS_STATE_E) {
            pwps_info->state = WPS_STATE_F;
            mwu_printf(DEBUG_WPS_STATE, "State E->F");
        }
        break;
    case WPS_EAP_M6:
        mwu_printf(DEBUG_WPS_STATE, "Event Trigger : WPS_M6");
        if (wps_s->bss_type == BSS_TYPE_STA && pwps_info->role == WPS_REGISTRAR) {
            if (pwps_info->state == WPS_STATE_E) {
                pwps_info->state = WPS_STATE_D;
                mwu_printf(DEBUG_WPS_STATE, "State E->D");
            }
        } else if (pwps_info->state == WPS_STATE_F) {
            pwps_info->state = WPS_STATE_E;
            mwu_printf(DEBUG_WPS_STATE, "State F->E");
        }
        break;
    case WPS_EAP_M7:
        mwu_printf(DEBUG_WPS_STATE, "Event Trigger : WPS_M7");
        if (wps_s->bss_type == BSS_TYPE_STA && pwps_info->role == WPS_REGISTRAR) {
            if (pwps_info->state == WPS_STATE_D) {
                pwps_info->state = WPS_STATE_E;
                mwu_printf(DEBUG_WPS_STATE, "State D->E");
            }
        } else if (pwps_info->state == WPS_STATE_E) {
            pwps_info->state = WPS_STATE_F;
            mwu_printf(DEBUG_WPS_STATE, "State E->F");
        }
        break;
    case WPS_EAP_M8:
        mwu_printf(DEBUG_WPS_STATE, "Event Trigger : WPS_M8");
        if (wps_s->bss_type == BSS_TYPE_STA && pwps_info->role == WPS_REGISTRAR) {
            if (pwps_info->state == WPS_STATE_E) {
                pwps_info->state = WPS_STATE_D;
                mwu_printf(DEBUG_WPS_STATE, "State E->D");
            }
        } else if (pwps_info->state == WPS_STATE_F) {
            pwps_info->state = WPS_STATE_E;
            mwu_printf(DEBUG_WPS_STATE, "State F->E");
        }
        break;
    case WPS_MSG_DONE:
        mwu_printf(DEBUG_WPS_STATE, "Event Trigger : WPS_DONE");

        pwps_info->register_completed = WPS_SET;
        pwps_info->registration_in_progress = WPS_CANCEL;
        mwu_printf(DEBUG_WPS_STATE, "Set registration in progress %d",
                   pwps_info->registration_in_progress);

        if (wps_s->bss_type == BSS_TYPE_STA && pwps_info->role == WPS_REGISTRAR) {
            if (pwps_info->state == WPS_STATE_D) {
                pwps_info->state = WPS_STATE_E;
                mwu_printf(DEBUG_WPS_STATE, "State D->E");
            }
        } else if (pwps_info->state == WPS_STATE_E) {
            pwps_info->state = WPS_STATE_H;
            mwu_printf(DEBUG_WPS_STATE, "State E->H");
        }
        break;
    case WPS_MSG_ACK:
        mwu_printf(DEBUG_WPS_STATE, "Event Trigger : WPS_ACK");
        if (wps_s->bss_type == BSS_TYPE_STA && pwps_info->role == WPS_REGISTRAR) {
            if (pwps_info->state == WPS_STATE_E) {
                pwps_info->state = WPS_STATE_G;
                mwu_printf(DEBUG_WPS_STATE, "State E->G");
            }
        } else if (pwps_info->state == WPS_STATE_E) {
            pwps_info->state = WPS_STATE_D;
            mwu_printf(DEBUG_WPS_STATE, "State E->D");
        }
        break;
    case WPS_MSG_NACK:
        mwu_printf(DEBUG_WPS_STATE, "Event Trigger : WPS_NACK");
        if (wps_s->bss_type == BSS_TYPE_STA && pwps_info->role == WPS_REGISTRAR) {
            if (pwps_info->state == WPS_STATE_D) {
                pwps_info->state = WPS_STATE_H;
                mwu_printf(DEBUG_WPS_STATE, "State D->H");
            } else if (pwps_info->state == WPS_STATE_E) {
                pwps_info->state = WPS_STATE_F;
                mwu_printf(DEBUG_WPS_STATE, "State E->F");
            }
        } else {

            switch (pwps_info->state) {
            case WPS_STATE_F:
                pwps_info->state = WPS_STATE_I;
                mwu_printf(DEBUG_WPS_STATE, "State F->I");
                break;
            case WPS_STATE_E:
                pwps_info->state = WPS_STATE_G;
                mwu_printf(DEBUG_WPS_STATE, "State E->G");
                break;
            }
        }
        break;
    case WPS_EAP_MSG_FAIL:
        mwu_printf(DEBUG_WPS_STATE, "Event Trigger : EAP_FAIL");

        pwps_info->last_recv_wps_msg = -1;

        /* Reset Public keys and E-Hash, R-Hash */
        wps_reset_wps_state(pwps_info);

        switch (pwps_info->state) {
        case WPS_STATE_D:
            pwps_info->state = WPS_STATE_A;
            mwu_printf(DEBUG_WPS_STATE, "State D->A");
            break;
        case WPS_STATE_G:
            pwps_info->state = WPS_STATE_A;
            mwu_printf(DEBUG_WPS_STATE, "State G->A");
            break;
        case WPS_STATE_H:
            pwps_info->state = WPS_STATE_A;
            mwu_printf(DEBUG_WPS_STATE, "State H->A");
            break;
        case WPS_STATE_I:
            pwps_info->state = WPS_STATE_A;
            mwu_printf(DEBUG_WPS_STATE, "State I->A");
            break;
        case WPS_STATE_A:
            pwps_info->state = WPS_STATE_A;
            mwu_printf(DEBUG_WPS_STATE, "State A->A");
            break;
        case WPS_STATE_F:
            pwps_info->state = WPS_STATE_A;
            mwu_printf(DEBUG_WPS_STATE, "State F->A");
            break;
        default:
            break;
        }
        if (pwps_info->nack_error_count > MAX_NACK_ERROR_COUNT) {
            status = wps_maximum_nack_handler(pwps_info, wps_s);
        }
        break;

    default:
        break;
    }                           /* end of switch */

    /* New WPS State */
    wps_debug_print_state("State Transition After", pwps_info->state);

    LEAVE();
    return status;
}

/**
 *  @brief Maximum NACK error handler
 *
 *  @param pwps_info    A pointer to WPS_INFO structure
 *  @param wps_s        A pointer to WPS_DATA structure
 *  @return             WPS_STATUS_SUCCESS--success, otherwise--fail
 */
int
wps_maximum_nack_handler(PWPS_INFO pwps_info, WPS_DATA * wps_s)
{
    int status = WPS_STATUS_SUCCESS;
    ENTER();

    pwps_info->nack_error_count = 0;

    if (pwps_info->set_timer == WPS_SET) {
        wps_cancel_timer(wps_txTimer_handler, pwps_info);
        pwps_info->set_timer = WPS_CANCEL;
        pwps_info->wps_msg_resent_count = 0;
    }
    if (pwps_info->role == WPS_ENROLLEE) {
        LEAVE();
        return status;
    } else if (pwps_info->role == WPS_REGISTRAR) {

        mwu_printf(DEBUG_WPS_STATE, "Cancelling registration timer!");
        wps_cancel_timer(wps_registration_time_handler, pwps_info);
        /**
         * Read uAP configuration for next connection
         */
        wps_load_uap_cred(wps_s, pwps_info);

        /* For UAP mode */
        wps_s->current_ssid.mode = IEEE80211_MODE_INFRA;
        pwps_info->mode = IEEE80211_MODE_INFRA;

        /*
         * Send APCMD_SYS_CONFIGURE command to set WPS IE
         */
        status = wps_wlan_ie_config(SET_WPS_IE, &ie_index);
        if (status != WPS_STATUS_SUCCESS) {
            mwu_printf(MSG_WARNING, "Setting WPS IE failed!!!");
        }
        if (pwps_info->registrar.version >= WPS_VERSION_2DOT0) {
            status =
                wps_wlan_ie_config(SET_WPS_AP_SESSION_INACTIVE_AR_IE,
                                   &ap_assocresp_ie_index);
            if (status != WPS_STATUS_SUCCESS) {
                mwu_printf(MSG_WARNING, "WPS IE configuration failure.\n");
            }
        }
        pwps_info->input_state = WPS_INPUT_STATE_METHOD;
        pwps_info->pin_pbc_set = WPS_CANCEL;

        wps_print_registration_method_menu(pwps_info);
    }

    LEAVE();
    return status;
}

/**
 *  @brief Registrar EAP packet process handling
 *
 *  @param pwps_info    A pointer to WPS_INFO structure
 *  @param peap         A pointer to EAP frame header
 *  @return             WPS_STATUS_SUCCESS--success, otherwise--fail
 */
static int
wps_eap_response_message_handler(PWPS_INFO pwps_info, PEAP_FRAME_HEADER peap)
{
    int status = WPS_STATUS_SUCCESS;
    u16 msg_next, msg_type = 0;

    ENTER();

    pwps_info->peer_id = peap->identifier + 1;

    if (peap->type == EAP_TYPE_IDENTITY) {
        wps_debug_print_msgtype("RX WPS Message Type",
                                WPS_EAP_MSG_RESPONSE_IDENTITY);
        if (pwps_info->set_timer == WPS_SET) {
            wps_cancel_timer(wps_txTimer_handler, pwps_info);
            pwps_info->set_timer = WPS_CANCEL;
            pwps_info->wps_msg_resent_count = 0;
        }

        status =
            wps_eap_response_identity_process(pwps_info, (u8 *) peap,
                                              peap->length);
        if (status == WPS_STATUS_SUCCESS) {
            msg_type = WPS_EAP_MSG_RESPONSE_IDENTITY;
            pwps_info->last_recv_wps_msg = msg_type;

            if (pwps_info->role == WPS_ENROLLEE
#if defined(WPSE_SUPPORT) || defined(WIFIDIR_SUPPORT)
                || (IS_DISCOVERY_ENROLLEE(pwps_info))
#endif
                ) {
                wps_enrollee_state_transition(pwps_info, msg_type);
                msg_next = wps_enrollee_state_check(pwps_info, msg_type);
            } else {
                wps_registrar_state_transition(pwps_info, msg_type);
                msg_next = wps_registrar_state_check(pwps_info, msg_type);
            }

            if (msg_next != WPS_EAP_UNKNOWN)
                status =
                    wps_msg_transmit(pwps_info, msg_next,
                                     pwps_info->wps_msg_timeout);
        }
    } else if (peap->type == EAP_TYPE_WPS) {
        switch (peap->op_code) {
        case WPS_Msg:

            msg_type = wps_get_message_type(peap);

            if ((msg_type > pwps_info->last_recv_wps_msg) ||
                (msg_type == WPS_M2D) ||
                ((pwps_info->last_recv_wps_msg == WPS_M2D) &&
                 (msg_type == WPS_M2))) {
                mwu_printf(DEBUG_WPS_STATE, "Last Message received 0x%x",
                           pwps_info->last_recv_wps_msg);
                mwu_printf(DEBUG_WPS_STATE, "Current message 0x%x", msg_type);
                if ((msg_type == pwps_info->last_recv_wps_msg)
                    && (msg_type != WPS_M2D)) {
                    /** recevied the retransmitted message
                     *  use the previous transmitted message for auth,
                     *  instead of current tx message
                     */
                    mwu_printf(DEBUG_WPS_STATE,
                               "Received reTx EAPOL Rsp message: 0x%x",
                               msg_type);
                    memcpy(&pwps_info->last_message,
                           &pwps_info->last_tx_message, sizeof(MESSAGE_BUFFER));
                }

                pwps_info->last_recv_wps_msg = msg_type;

            } else {
                printf("rx_msg: Dropping unexpected msg! "
                       "(prev_msg: 0x%x, curr_msg: 0x%x)\n",
                       pwps_info->last_recv_wps_msg, msg_type);
                break;
            }

            if (pwps_info->set_timer == WPS_SET) {
                wps_cancel_timer(wps_txTimer_handler, pwps_info);
                pwps_info->set_timer = WPS_CANCEL;
                pwps_info->wps_msg_resent_count = 0;
            }

            if (msg_type != WPS_EAP_UNKNOWN)
                status =
                    (*wps_msg_process[msg_type]) (pwps_info, (u8 *) peap,
                                                  peap->length);
            else
                status = WPS_STATUS_FAIL;

            if (status == WPS_STATUS_SUCCESS) {

                if (pwps_info->role == WPS_ENROLLEE
#if defined(WPSE_SUPPORT) || defined(WIFIDIR_SUPPORT)
                    || (IS_DISCOVERY_ENROLLEE(pwps_info))
#endif
                    ) {
                    wps_enrollee_state_transition(pwps_info, msg_type);
                    msg_next = wps_enrollee_state_check(pwps_info, msg_type);
                } else {
                    wps_registrar_state_transition(pwps_info, msg_type);
                    msg_next = wps_registrar_state_check(pwps_info, msg_type);
                }

                if (msg_next != WPS_EAP_UNKNOWN)
                    status =
                        wps_msg_transmit(pwps_info, msg_next,
                                         pwps_info->wps_msg_timeout);
                else
                    status =
                        wps_msg_transmit(pwps_info, WPS_MSG_NACK,
                                         pwps_info->wps_msg_timeout);
            } else
                status =
                    wps_msg_transmit(pwps_info, WPS_MSG_NACK,
                                     pwps_info->wps_msg_timeout);

            break;

        case WPS_Done:
            msg_type = wps_get_message_type(peap);

            status =
                wps_done_message_process(pwps_info, (u8 *) peap, peap->length);
            if (status == WPS_STATUS_SUCCESS) {

                wps_registrar_state_transition(pwps_info, msg_type);

                msg_next = wps_registrar_state_check(pwps_info, msg_type);
                if (msg_next != WPS_EAP_UNKNOWN)
                    status =
                        wps_msg_transmit(pwps_info, msg_next,
                                         pwps_info->wps_msg_timeout);
            } else
                status =
                    wps_msg_transmit(pwps_info, WPS_MSG_NACK,
                                     pwps_info->wps_msg_timeout);

            break;

        case WPS_Ack:
            msg_type = wps_get_message_type(peap);

            status =
                wps_ack_message_process(pwps_info, (u8 *) peap, peap->length);
            if (status == WPS_STATUS_SUCCESS) {

                if (pwps_info->role == WPS_ENROLLEE
#if defined(WPSE_SUPPORT) || defined(WIFIDIR_SUPPORT)
                    || (IS_DISCOVERY_ENROLLEE(pwps_info))
#endif
                    ) {
                    wps_enrollee_state_transition(pwps_info, msg_type);
                    msg_next = wps_enrollee_state_check(pwps_info, msg_type);
                } else {
                    wps_registrar_state_transition(pwps_info, msg_type);
                    msg_next = wps_registrar_state_check(pwps_info, msg_type);
                }

                if (msg_next != WPS_EAP_UNKNOWN)
                    status =
                        wps_msg_transmit(pwps_info, msg_next,
                                         pwps_info->wps_msg_timeout);
            } else
                status =
                    wps_msg_transmit(pwps_info, WPS_MSG_NACK,
                                     pwps_info->wps_msg_timeout);

            break;

        case WPS_Nack:
            msg_type = wps_get_message_type(peap);

            status =
                wps_nack_message_process(pwps_info, (u8 *) peap, peap->length);
            if (status == WPS_STATUS_SUCCESS) {

                pwps_info->nack_error_count++;

                if (pwps_info->set_timer == WPS_SET) {
                    wps_cancel_timer(wps_txTimer_handler, pwps_info);
                    pwps_info->set_timer = WPS_CANCEL;
                }
                if (pwps_info->role == WPS_ENROLLEE
#if defined(WPSE_SUPPORT) || defined(WIFIDIR_SUPPORT)
                    || (IS_DISCOVERY_ENROLLEE(pwps_info))
#endif
                    ) {
                    wps_enrollee_state_transition(pwps_info, msg_type);
                    msg_next = wps_enrollee_state_check(pwps_info, msg_type);
                } else {
                    wps_registrar_state_transition(pwps_info, msg_type);
                    msg_next = wps_registrar_state_check(pwps_info, msg_type);
                }
                if (msg_next != WPS_EAP_UNKNOWN)
                    status =
                        wps_msg_transmit(pwps_info, msg_next,
                                         pwps_info->wps_msg_timeout);
                /* If PINs on Registrar and Enrollee don't match, Enrollee will
                 * send NACK, and Registrar will send FAIL. In this case,
                 * we want WPS handshake be stopped ASAP. So, send fail event
                 * to user.
                 * Note: possibly there are other situations where NACK is
                 * followed by FAIL and re-handshake is wanted. If so, this
                 * solution is not appropriate.
                 */
                if (msg_next == WPS_EAP_MSG_FAIL)
                    send_registrar_fail_event();
            } else
                status =
                    wps_msg_transmit(pwps_info, WPS_MSG_NACK,
                                     pwps_info->wps_msg_timeout);

            break;

        default:
            break;
        }
    }

    LEAVE();
    return status;
}

/********************************************************
        Global Functions
********************************************************/

#define EAP_REASSEMBLY_START(winfo, peap) do{   \
    winfo->frag_rx_in_prog=1;                   \
    winfo->frag_msg_tot_len=0;                  \
    winfo->frag_msg_cur_len=0;                  \
    winfo->frag_msg_tot_len=mwu_ntohs(*(u16*)(peap+1))+\
            sizeof(EAP_FRAME_HEADER);\
}while(0)

#define EAP_REASSEMBLY_END(winfo, peap_frag) do{\
    PEAP_FRAME_HEADER peap =                    \
          (PEAP_FRAME_HEADER)winfo->frag_buf;   \
    peap->identifier = peap_frag->identifier;   \
    peap->length = mwu_htons(winfo->frag_msg_tot_len);\
    peap->flags = 0;                            \
    winfo->frag_rx_in_prog=0;                   \
    winfo->frag_msg_tot_len=0;                  \
    winfo->frag_msg_cur_len=0;                  \
}while(0)

#define EAP_REASSEMBLE_FRAG(winfo, first, peapol, peap) do{     \
    int cpsize;                                                 \
    u8 *cpsrc;                                                  \
    if(first){                                                  \
        cpsrc = (u8*)peap;                                      \
        memcpy(&winfo->frag_buf[0], cpsrc, sizeof(EAP_FRAME_HEADER));\
        winfo->frag_msg_cur_len += sizeof(EAP_FRAME_HEADER);    \
        cpsize = mwu_ntohs(peapol->length)-sizeof(EAP_FRAME_HEADER)-sizeof(u16);\
        cpsrc += sizeof(EAP_FRAME_HEADER)+sizeof(u16);          \
    }else{                                                      \
        cpsize = mwu_ntohs(peapol->length)-sizeof(EAP_FRAME_HEADER);\
        cpsrc = (u8*)peap+sizeof(EAP_FRAME_HEADER);             \
    }                                                           \
    if((winfo->frag_msg_cur_len+cpsize)<=EAP_FRAG_BUF_SIZE){    \
        memcpy(&winfo->frag_buf[winfo->frag_msg_cur_len], cpsrc, cpsize);\
        winfo->frag_msg_cur_len += cpsize;                      \
    }else{                                                      \
        /* Overflow */                                          \
        status = WPS_STATUS_FAIL;                               \
    }                                                           \
}while(0);

#define EAP_PKT_NOT_FRAG    (1)
#define EAP_FRAG_PENDING    (2)
#define EAP_FRAG_COMPLETE   (3)

int
wps_reassemble_rx_frag(PWPS_INFO pwps_info, PEAPOL_FRAME_HEADER peapol)
{
    int status = WPS_STATUS_SUCCESS;
    int first_frag = 0;
    ENTER();
    PEAP_FRAME_HEADER peap = (PEAP_FRAME_HEADER)
        ((u8 *) peapol + sizeof(EAPOL_FRAME_HEADER));

    if (!pwps_info->frag_rx_in_prog) {
        /* This is the first fragment */
        first_frag = 1;
        if ((peap->flags & EAP_FLAGS_LEN_FIELD) && peap->length) {
            mwu_printf(DEBUG_WPS_STATE, "Starting RX Reassembly");
            /* Length field is valid */
            EAP_REASSEMBLY_START(pwps_info, peap);
        } else {
            /* Invalid Length field, Ignore frag */
            status = WPS_STATUS_FAIL;
            goto done;
        }
    }

    EAP_REASSEMBLE_FRAG(pwps_info, first_frag, peapol, peap);

  done:
    LEAVE();
    return status;
}

int
wps_check_rx_frag(PWPS_INFO pwps_info, PEAPOL_FRAME_HEADER peapol)
{
    int ret;
    ENTER();
    PEAP_FRAME_HEADER peap_frag = (PEAP_FRAME_HEADER)
        ((u8 *) peapol + sizeof(EAPOL_FRAME_HEADER));

    if (!(peap_frag->flags & EAP_FLAGS_MORE_FRAG) &&
        !pwps_info->frag_rx_in_prog) {
        /* packet Not fragmented */
        ret = EAP_PKT_NOT_FRAG;
        goto done;
    }

    /* Reassemble Rxed Fragment */
    ret = wps_reassemble_rx_frag(pwps_info, peapol);

    if (ret == WPS_STATUS_FAIL) {
        goto done;
    }

    /* Current Fragment Accepted */
    if (pwps_info->frag_msg_tot_len == pwps_info->frag_msg_cur_len) {
        mwu_printf(DEBUG_WPS_STATE, "RX Reassembly Complete");
        /* Reassembly complete */
        EAP_REASSEMBLY_END(pwps_info, peap_frag);
        ret = EAP_FRAG_COMPLETE;
    } else {
        /* some more Fragments Expected */
        mwu_printf(DEBUG_WPS_STATE, "Sending FRAG_ACK");
        ret = wps_msg_transmit(pwps_info,
                               WPS_MSG_FRAG_ACK, WPS_MSG_LONG_TIMEOUT);
        ret = EAP_FRAG_PENDING;
    }

  done:
    LEAVE();
    return ret;
}

/**
 *  @brief EAP frame handling function
 *
 *  @param buffer       A pointer to frame received buffer
 *  @param src_addr     A pointer to source address of received frame
 *  @return             None
 */
void
wps_message_handler(u8 * buffer, u8 * src_addr)
{
    PEAPOL_FRAME_HEADER peapol;
    PEAP_FRAME_HEADER peap;
    WPS_DATA *wps_s;
    PWPS_INFO pwps_info;
    int status = WPS_STATUS_SUCCESS;
    int msg_next;

    ENTER();

    pwps_info = wps_get_private_info();

    wps_s = (WPS_DATA *) & pwps_info->wps_data;

    peapol = (PEAPOL_FRAME_HEADER) buffer;
    switch (peapol->packet_type) {
    case EAP_PACKET:
        {
            peap = (PEAP_FRAME_HEADER) (buffer + sizeof(EAPOL_FRAME_HEADER));

            // XXX: Move this out of here
            pwps_info->peer_id = peap->identifier;

            if (peap->type == EAP_TYPE_WPS) {
                status = wps_check_rx_frag(pwps_info, peapol);

                if ((status == EAP_FRAG_PENDING) || (status == WPS_STATUS_FAIL)) {
                    goto done;
                } else if (status == EAP_FRAG_COMPLETE) {
                    peap = (PEAP_FRAME_HEADER) pwps_info->frag_buf;
                    mwu_hexdump(DEBUG_WPS_MSG, "Reass pkt", (u8 *) peap,
                                mwu_ntohs(peap->length));
                }
            }
            switch (peap->code) {
            case EAP_REQUEST:
                mwu_printf(DEBUG_WPS_STATE, "EAP_REQUEST received");

                {
                    if (wps_s->bss_type == BSS_TYPE_UAP) {
                            if ((pwps_info->role == WPS_REGISTRAR &&
                                 memcmp(pwps_info->enrollee.mac_address, src_addr,
                                        ETH_ALEN)) || (pwps_info->role == WPS_ENROLLEE
                                                       && memcmp(pwps_info->registrar.
                                                                 mac_address, src_addr,
                                                                 ETH_ALEN))) {
                                 mwu_printf(MSG_WARNING,
                                           "EAP_REQUEST received from invalid source "
                                            UTIL_MACSTR " dropped!", UTIL_MAC2STR(src_addr));
                                 break;
                            }
                        }
                    wps_eap_request_message_handler(pwps_info, peap);
                }
                break;

            case EAP_RESPONSE:
                mwu_printf(DEBUG_WPS_STATE, "EAP_RESPONSE received");

                {
                    if (wps_s->bss_type == BSS_TYPE_UAP) {
                            if ((pwps_info->role == WPS_REGISTRAR &&
                                 memcmp(pwps_info->enrollee.mac_address, src_addr,
                                        ETH_ALEN)) || (pwps_info->role == WPS_ENROLLEE
                                                       && memcmp(pwps_info->registrar.
                                                                 mac_address, src_addr,
                                                                 ETH_ALEN))) {
                                   mwu_printf(MSG_WARNING,
                                              "EAP_RESPONSE received from invalid source "
                                              UTIL_MACSTR " dropped!", UTIL_MAC2STR(src_addr));
                                   break;
                            }
                        }
                    wps_eap_response_message_handler(pwps_info, peap);
                }
                break;

            case EAP_FAILURE:
                {
                    mwu_printf(DEBUG_WPS_STATE, "EAP_FAILURE received");

                    if (pwps_info->set_timer == WPS_SET) {
                        wps_cancel_timer(wps_txTimer_handler, pwps_info);
                        pwps_info->set_timer = WPS_CANCEL;
                        pwps_info->wps_msg_resent_count = 0;
                    }

                    if (pwps_info->role == WPS_ENROLLEE
                        || (IS_DISCOVERY_ENROLLEE(pwps_info))
                        ) {
                        wps_enrollee_state_transition(pwps_info,
                                                      WPS_EAP_MSG_FAIL);
                    } else {
                        wps_registrar_state_transition(pwps_info,
                                                       WPS_EAP_MSG_FAIL);
                    }

                    if (pwps_info->invalid_credential == WPS_SET) {
                        if (pwps_info->role == WPS_ENROLLEE
                            || IS_DISCOVERY_ENROLLEE(pwps_info)
                            ) {
                            if (wps_s->bss_type == BSS_TYPE_UAP &&
                                pwps_info->registrar.version >=
                                WPS_VERSION_2DOT0) {
                                /* Clear session start IEs for Assoc response */
                                wps_wlan_ie_config(CLEAR_AR_WPS_IE,
                                                   &ap_assocresp_ie_index);
                            }
                        }

                    }
                    if (pwps_info->state == WPS_STATE_A &&
                        pwps_info->register_completed == WPS_SET) {
                        /*
                         * Registration complete with M8 received.
                         */
                        printf
                            ("WPS Registration Protocol Completed Successfully !\n\n\n");

                        if (pwps_info->role == WPS_ENROLLEE
                            || (IS_DISCOVERY_ENROLLEE(pwps_info))
                            ) {
                            /* WPS handshake is complete, clear the Enrollee
                               IE's */

                            /* Clear WPS IE */
                            if (ie_index != -1) {
                                mwu_printf(MSG_ERROR, "Clearing WSC IE's !\n");
                                wps_wlan_ie_config(CLEAR_WPS_IE, &ie_index);
                                ie_index = -1;
                            }
                        }

                        if (pwps_info->role == WPS_REGISTRAR) {
                            /* Reset Enrollee Mac address to ensure check for
                               multiple EAPOL-START succeeds in EAPOL-STRAT
                               handling */
                            memset(&pwps_info->enrollee.mac_address, 0,
                                   ETH_ALEN);
                        }
                        status = wps_write_to_config_file(pwps_info);
#ifdef WIFIDIR_PERSISTENT_RECORD
                        if (IS_DISCOVERY_ENROLLEE(pwps_info)) {
                            wifidir_update_persistent_record(pwps_info);
                        }
#endif

                        /* message exchange completed */
                        printf("\n");
                        if (status == WPS_STATUS_SUCCESS)
                            printf
                                ("WPS Registration Protocol Configuration file generated  Successfully !\n\n\n");
                        else
                            printf
                                ("Failed to generate WPS Registration configuration file !\n\n\n");

                        send_credential_event(&pwps_info->registrar.cred_data[0]);

                        /* Cancel WPS Registration Timer */
                        mwu_printf(DEBUG_WPS_STATE,
                                   "Cancelling registration timer!");
                        wps_cancel_timer(wps_registration_time_handler,
                                         pwps_info);
                        //mwu_printf(DEBUG_WPS_STATE,
                        //           "Stopping WPS/Clearing AssocReq IEs!");
                        //wps_stop(pwps_info->wps_s);

                        if (wps_s->bss_type == BSS_TYPE_UAP &&
                            pwps_info->registrar.version >= WPS_VERSION_2DOT0) {
                            /* Clear session start IEs for Assoc response */
                            wps_wlan_ie_config(CLEAR_AR_WPS_IE,
                                               &ap_assocresp_ie_index);
                        }

                    } else {
                        u8 bssid_get[ETH_ALEN];
                        int retry_cnt = 50;
                        static u8 eap_fail_count = 0;
                        /*
                         * Registration complete without M8 received especially in
                         * case of external Registrar.
                         * Restart registration protocol.
                         */

                        if (pwps_info->set_timer == WPS_SET) {
                            wps_cancel_timer(wps_txTimer_handler, pwps_info);
                            pwps_info->set_timer = WPS_CANCEL;
                            pwps_info->wps_msg_resent_count = 0;
                        }
                        eap_fail_count++;
                        if (eap_fail_count >= MAX_EAP_FAIL_COUNT) {
                            eap_fail_count = 0;
                            if (wps_s->current_ssid.mode ==
                                IEEE80211_MODE_INFRA)
                                status = wps_wlan_set_deauth();
                            do {
                                memset(bssid_get, 0x00, ETH_ALEN);
                                wps_wlan_get_wap(bssid_get);
                                if ((memcmp
                                     (bssid_get, wps_s->current_ssid.bssid,
                                      ETH_ALEN) != 0)) {
                                    mwu_printf(DEBUG_WPS_STATE,
                                               "\nRe-connecting to AP ..... \n");

                                    /* Enable driver WPS session checking */
                                    wps_wlan_session_control(WPS_SESSION_ON);

                                    wps_wlan_set_wap((u8 *) wps_s->current_ssid.
                                                     bssid);

                                    retry_cnt--;

                                    sleep(1);
                                } else {

                                    /* Disable driver WPS session checking */
                                    wps_wlan_session_control(WPS_SESSION_OFF);

                                    break;
                                }
                            } while (retry_cnt != 0);
                        }
                        /*
                         * Start to exchange WPS Message by sending EAPOL_START packet
                         */
                        msg_next =
                            wps_enrollee_state_check(pwps_info,
                                                     WPS_EAP_MSG_FAIL);
                        pwps_info->eap_msg_sent = WPS_EAPOL_MSG_START;
                        pwps_info->state = WPS_STATE_A;
                        status =
                            wps_msg_transmit(pwps_info, WPS_EAPOL_MSG_START,
                                             WPS_MSG_LONG_TIMEOUT);
                    }
                    break;
                }               /* EAP_FAILURE */

            case EAP_SUCCESS:
                mwu_printf(DEBUG_WPS_STATE, "EAP_SUCCESS received");
                break;

            default:
                mwu_printf(MSG_ERROR, "Unknown Code received");
                break;
            }
            break;
        }                       /* EAP_PACKET */

    case EAPOL_START:
        {
            if (pwps_info->pin_pbc_set == WPS_CANCEL)
                break;

            if (pwps_info->registration_in_progress == WPS_SET) {

                if (wps_s->bss_type == BSS_TYPE_UAP &&
                    pwps_info->role == WPS_ENROLLEE) {
                    if (memcmp
                        (pwps_info->registrar.mac_address, src_addr,
                         ETH_ALEN)) {

                        mwu_printf(MSG_WARNING,
                                   "EAPOL_START received from " UTIL_MACSTR
                                   " while session is in progress with " UTIL_MACSTR,
                                   UTIL_MAC2STR(src_addr),
                                   UTIL_MAC2STR(pwps_info->registrar.mac_address));
                        mwu_printf(MSG_WARNING, "EAPOL_START dropped!!");
                        return;
                    }
                } else {
                    if (memcmp
                        (pwps_info->enrollee.mac_address, src_addr, ETH_ALEN)) {

                        mwu_printf(MSG_WARNING,
                                   "EAPOL_START received from " UTIL_MACSTR
                                   " while session is in progress with " UTIL_MACSTR,
                                   UTIL_MAC2STR(src_addr),
                                   UTIL_MAC2STR(pwps_info->enrollee.mac_address));
                        mwu_printf(MSG_WARNING, "EAPOL_START dropped!!");
                        return;
                    }
                }
            }

            mwu_printf(DEBUG_WPS_STATE, "EAPOL_START received");

            if (pwps_info->role == WPS_REGISTRAR) {
                /* Get Enrollee Peer Address */
                memcpy(pwps_info->enrollee.mac_address, src_addr, ETH_ALEN);
                mwu_printf(DEBUG_WLAN, "Peer Address Get: ");
                mwu_hexdump(DEBUG_WLAN, "pwps_info->enrollee.mac_address",
                            pwps_info->enrollee.mac_address, ETH_ALEN);

                wps_eapol_start_handler(pwps_info);
                wps_eap_request_identity_prepare(pwps_info);

                mwu_printf(DEBUG_WPS_STATE,
                           "eap_msg_sent = WPS_EAP_MSG_REQUEST_IDENTITY");
                pwps_info->eap_msg_sent = WPS_EAP_MSG_REQUEST_IDENTITY;
            } else if (pwps_info->role == WPS_ENROLLEE &&
                       (wps_s->bss_type == BSS_TYPE_UAP)) {
                /* Get Registrar Peer Address */
                memcpy(pwps_info->registrar.mac_address, src_addr, ETH_ALEN);
                mwu_printf(DEBUG_WLAN, "Peer Address Get: ");
                mwu_hexdump(DEBUG_WLAN, "pwps_info->registrar.mac_address",
                            pwps_info->registrar.mac_address, 6);

                wps_eapol_start_handler(pwps_info);
                wps_eap_request_identity_prepare(pwps_info);

                mwu_printf(DEBUG_WPS_STATE,
                           "eap_msg_sent = WPS_EAP_MSG_REQUEST_IDENTITY");
                pwps_info->eap_msg_sent = WPS_EAP_MSG_REQUEST_IDENTITY;
            }
            if (IS_DISCOVERY_REGISTRAR(pwps_info)) {

                /* Get Enrollee Peer Address */
                memcpy(pwps_info->enrollee.mac_address, src_addr, ETH_ALEN);
                mwu_printf(DEBUG_WLAN, "Peer Address Get: ");
                mwu_hexdump(DEBUG_WLAN, "pwps_info->enrollee.mac_address",
                            pwps_info->enrollee.mac_address, 6);

                wps_eapol_start_handler(pwps_info);
                wps_eap_request_identity_prepare(pwps_info);

                mwu_printf(DEBUG_WPS_STATE,
                           "eap_msg_sent = WPS_EAP_MSG_REQUEST_IDENTITY");
                pwps_info->eap_msg_sent = WPS_EAP_MSG_REQUEST_IDENTITY;
            }

            break;
        }                       /* EAPOL_START */

    default:
        mwu_printf(MSG_ERROR, "Unknown Packet Type");
        break;
    }

  done:
    LEAVE();
}

/**
 *  @brief WPS EAP frame transmit function
 *
 *  @param pwps_info    A pointer to PWPS_INFO structure
 *  @param msg_type     message type
 *  @param timeout      timeout
 *  @return             WPS_STATUS_SUCCESS--success, otherwise--fail
 */
int
wps_msg_transmit(PWPS_INFO pwps_info, u16 msg_type, u32 timeout)
{
    int status = WPS_STATUS_SUCCESS;
    u32 sec, usec;
    ENTER();

    wps_debug_print_msgtype("TX WPS Message Type", msg_type);

    status = (*wps_msg_prepare[msg_type]) (pwps_info);

    if (status == WPS_STATUS_SUCCESS) {
        pwps_info->eap_msg_sent = msg_type;

        if (pwps_info->role == WPS_ENROLLEE) {
            wps_enrollee_state_transition(pwps_info, msg_type);
        } else if (pwps_info->role == WPS_REGISTRAR) {
            wps_registrar_state_transition(pwps_info, msg_type);
        } else if (pwps_info->role == WIFIDIR_ROLE) {
            if (pwps_info->discovery_role == WPS_REGISTRAR) {
                wps_registrar_state_transition(pwps_info, msg_type);
            } else {
                wps_enrollee_state_transition(pwps_info, msg_type);
            }
        }
    }

    if (msg_type != WPS_EAP_MSG_FAIL) {
        sec = timeout / 1000;
        usec = (timeout % 1000) * 1000;
        wps_start_timer(sec, usec, wps_txTimer_handler, pwps_info);
        pwps_info->set_timer = WPS_SET;
        pwps_info->wps_msg_resent_count++;
    } else {
        /* registrar send out EAP-FAIL */
        /* WPS process complete */

        printf("\n");
        if (pwps_info->register_completed != WPS_SET
                && pwps_info->restart_by_M2D == WPS_SET
                && pwps_info->pin_generator == WPS_ENROLLEE
                && pwps_info->registrar.device_password_id ==
            DEVICE_PASSWORD_ID_PIN) {
            printf("Please Input 8-digit Numeric PIN :");
        }
        if (pwps_info->set_timer == WPS_SET) {
            wps_cancel_timer(wps_txTimer_handler, pwps_info);
            pwps_info->set_timer = WPS_CANCEL;
            pwps_info->wps_msg_resent_count = 0;
        }

        if (pwps_info->role == WIFIDIR_ROLE &&
            pwps_info->discovery_role == WPS_REGISTRAR) {

            if (pwps_info->register_completed == WPS_SET) {
                printf
                    ("WPS Registration Protocol Completed Successfully !\n\n\n");
                pwps_info->wps_device_state = SC_CONFIGURED_STATE;
                mwu_printf(DEBUG_WPS_STATE,
                           "Device state SC_CONFIGURED_STATE !\n");
                send_registrar_done_event(pwps_info->enrollee.mac_address,
                                          MWPSMOD_ERR_SUCCESS);
            }
            /*
             * Update Group formation bit for registrar
             * send the command on station interface
             */

            if (wps_wlan_update_group_formation_config(WPS_CANCEL) !=
                WPS_STATUS_SUCCESS) {
                mwu_printf(DEBUG_WIFIDIR_DISCOVERY,
                           "Failed to reset Group formation bit.\n");
            }
            /*
             * Note, code below under !RESTART_SM_SUPPORT is mutually
             * excluse to this code.
             */
            /* Cancel WPS Registration Timer */
            mwu_printf(DEBUG_WPS_STATE, "Cancelling registration timer!");
            wps_cancel_timer(wps_registration_time_handler, pwps_info);
        }
        /* Auto-GO registar with role = 2 */
        else if (pwps_info->role == WPS_REGISTRAR) {
            /* Cancel WPS Registration Timer */
            mwu_printf(DEBUG_WPS_STATE, "Cancelling registration timer!");
            wps_cancel_timer(wps_registration_time_handler, pwps_info);

            /* Unregister user abort handler */
            wps_unregister_rdsock_handler(STDIN_FILENO);
        }
    }

    LEAVE();
    return status;
}

#define AP_CONNNECT_RETRY_CNT   (10)

/**
 *  @brief Check WLAN Link Status & Reconnect if disconnected
 *
 *  @param wps_s        A pointer to PWPS_DATA structure
 *  @param reconnected  A pointer to variable to indicate if STA re-connected
 *  @return             1-connected, 0-not connected
 */

int
wps_sta_check_link_active(WPS_DATA * wps_s, int *reconnected)
{
    int link_active = 0;
    u8 retry_count = AP_CONNNECT_RETRY_CNT;
    u8 bssid_get[ETH_ALEN];

    *reconnected = 0;

    memset(bssid_get, 0x00, ETH_ALEN);
    wps_wlan_get_wap(bssid_get);

    if ((memcmp(bssid_get, wps_s->current_ssid.bssid, ETH_ALEN) == 0)) {
        /* Link not lost */
        link_active = 1;
        return link_active;
    }

    *reconnected = 1;

    /* Enable driver WPS session checking */
    wps_wlan_session_control(WPS_SESSION_ON);

    do {
        printf("\nConnection lost, try to re-connect to AP ..... \n");

        if (wps_wlan_set_wap((u8 *) wps_s->current_ssid.bssid)) {
            printf("Re-Connect to AP Failed\n");
        }

        retry_count--;

        memset(bssid_get, 0x00, ETH_ALEN);
        wps_wlan_get_wap(bssid_get);

        if ((memcmp(bssid_get, wps_s->current_ssid.bssid, ETH_ALEN) == 0)) {
            link_active = 1;
            break;
        }

    } while (retry_count != 0);

    /* Disable driver WPS session checking */
    wps_wlan_session_control(WPS_SESSION_OFF);

    return link_active;
}

/**
 *  @brief Re-Transmit the last TX'ed packet
 *
 *  @param pwps_info    A pointer to PWPS_INFO structure
 *  @return             WPS_STATUS_SUCCESS--retransmit success, otherwise--fail
 */

int
retransmit_last_tx_pkt(PWPS_INFO pwps_info)
{
    PEAP_FRAME_HEADER peap_wps_frame = (PEAP_FRAME_HEADER) pwps_info->buffer;
    int pkt_len = mwu_ntohs(peap_wps_frame->length);
    int ret;

    wps_debug_print_msgtype("ReTX WPS Message Type", pwps_info->eap_msg_sent);

    if (pwps_info->eap_msg_sent != WPS_EAPOL_MSG_START) {
        ret = wps_eapol_txPacket((u8 *) pwps_info->buffer, pkt_len);

        wps_start_timer(WPS_MSG_DEFAULT_TIMEOUT / 1000, 0,
                        wps_txTimer_handler, pwps_info);
        pwps_info->set_timer = WPS_SET;
        pwps_info->wps_msg_resent_count++;
    } else {
        ret = wps_msg_transmit(pwps_info, WPS_EAPOL_MSG_START,
                               WPS_MSG_DEFAULT_TIMEOUT);
    }

    return ret;
}

/**
 *  @brief WPS frame transmit time-out handler
 *
 *  @param user_data    User private data
 *  @return             None
 */
void
wps_txTimer_handler(void *user_data)
{
    PWPS_INFO pwps_info;
    WPS_DATA *wps_s;
    int status = WPS_STATUS_SUCCESS;
    int reconnected = 0, link_active = 0;
    u8 zero_bssid[ETH_ALEN] = { 0 };

    ENTER();

    pwps_info = (PWPS_INFO) user_data;
    wps_s = (WPS_DATA *) & pwps_info->wps_data;

    if (pwps_info->set_timer == WPS_SET) {
        wps_cancel_timer(wps_txTimer_handler, pwps_info);
        pwps_info->set_timer = WPS_CANCEL;
    }

    /*
     * Some AP/External Registrar will disconnect after EAP-FAIL
     * packet sent without M2D being sent before that.
     * So, polling link status of AP to restart registration here.
     */
    if (wps_s->bss_type == BSS_TYPE_STA) {

        if (pwps_info->wps_msg_resent_count >= pwps_info->wps_msg_max_retry) {
            /* Maximum retry reached, disconnect & start all over again */
            if (wps_wlan_set_wap((u8 *) zero_bssid)) {
                printf("tx_timer: Disconnect from AP Failed\n");
            }
        }

        link_active = wps_sta_check_link_active(wps_s, &reconnected);

        if (!link_active) {
            printf("\n\nRe-Connect to AP FAIL reached MAX retry count!!!\n\n");
            status = WPS_STATUS_FAIL;
            goto done;
        }

        if (reconnected)
            pwps_info->restart_link_lost = WPS_SET;

        if (pwps_info->restart_link_lost || pwps_info->restart_by_M2D) {
            pwps_info->restart_link_lost = WPS_CANCEL;
            pwps_info->restart_by_M2D = WPS_CANCEL;

            pwps_info->state = WPS_STATE_A;
            pwps_info->id = 1;

            /* Reset Public keys and E-Hash, R-Hash */
            wps_reset_wps_state(pwps_info);

            pwps_info->last_recv_wps_msg = -1;
            pwps_info->wps_msg_resent_count = 0;
            pwps_info->eap_msg_sent = WPS_EAPOL_MSG_START;
            status = wps_msg_transmit(pwps_info, WPS_EAPOL_MSG_START,
                                      WPS_MSG_DEFAULT_TIMEOUT);

            goto done;
        }
    }

    retransmit_last_tx_pkt(pwps_info);

  done:
    if (status != WPS_STATUS_SUCCESS) {
        /* Cancel WPS Registration Timer */
        mwu_printf(DEBUG_WPS_STATE, "Cancelling registration timer!");
        wps_cancel_timer(wps_registration_time_handler, pwps_info);

        if (pwps_info->set_timer == WPS_SET) {
            wps_cancel_timer(wps_txTimer_handler, pwps_info);
            pwps_info->set_timer = WPS_CANCEL;
            pwps_info->wps_msg_resent_count = 0;
        }

        pwps_info->registration_fail = WPS_SET;
    }

    LEAVE();
    return;
}

/**
 *  @brief Start WPS registration timer
 *
 *  @param pwps_info    A pointer to WPS_INFO structure
 *  @return             WPS_STATUS_SUCCESS--success, otherwise--fail
 */
void
wps_start_registration_timer(PWPS_INFO pwps_info)
{
    u32 sec, usec;

    /* Start a timer for WPS registration protocol timeout */
    mwu_printf(DEBUG_WALK_TIME, "Start Timer for WPS Registration ... ");
    sec = WPS_REGISTRATION_TIME;
    usec = 0;
    wps_start_timer(sec, usec, wps_registration_time_handler, pwps_info);

    return;

}

/**
 *  @brief Start WPS state machine
 *
 *  @param pwps_info    A pointer to WPS_INFO structure
 *  @return             WPS_STATUS_SUCCESS--success, otherwise--fail
 */
int
wps_state_machine_start(PWPS_INFO pwps_info)
{
    int status = WPS_STATUS_SUCCESS;
    WPS_DATA *wps_s = (WPS_DATA *) & pwps_info->wps_data;

    if (((pwps_info->role == WPS_ENROLLEE || pwps_info->role == WPS_REGISTRAR)
         && wps_s->bss_type == BSS_TYPE_STA)
        || (IS_DISCOVERY_ENROLLEE(pwps_info))
        ) {
        /*
         * Start to exchange WPS Message by sending EAPOL_START packet
         */
        pwps_info->eap_msg_sent = WPS_EAPOL_MSG_START;
        status =
            wps_msg_transmit(pwps_info, WPS_EAPOL_MSG_START,
                             WPS_MSG_DEFAULT_TIMEOUT);
    }

    return status;
}

/**
 *  @brief Time-out handler of WSC AP Setup locked. R-Hash failure count will
 *  be reset to 0 of this timer is called.
 *
 *  @param user_data    A pointer to user data of timer functions
 *  @return             None
 */
void
wps_r_hash_failure_count_timer_handler(void *user_data)
{

    PWPS_INFO pwps_info;
    ENTER();

    pwps_info = (PWPS_INFO) user_data;

    /* Cancel Hash failure counter Timer */
    wps_cancel_timer(wps_r_hash_failure_count_timer_handler, pwps_info);

    mwu_printf(DEBUG_WALK_TIME, "WSC R-Hash Counter Timeout !");
    pwps_info->wps_r_hash_failure_count = 0;
    LEAVE();
}

/**
 *  @brief Time-out handler of WSC AP Setup locked. R-Hash failure count will
 *  be reset to 0 of this timer is called.
 *
 *  @param user_data    A pointer to user data of timer functions
 *  @return             None
 */
void
wps_ap_setup_locked_timer_handler(void *user_data)
{

    PWPS_INFO pwps_info;
    int ret;
    ENTER();

    pwps_info = (PWPS_INFO) user_data;

    /* Cancel AP Setup Locked Timer */
    wps_cancel_timer(wps_ap_setup_locked_timer_handler, pwps_info);

    mwu_printf(DEBUG_WALK_TIME, "WSC AP Setup Locked Timeout !");
    pwps_info->wps_ap_setup_locked = WPS_CANCEL;
    ret = wps_wlan_ie_config(SET_WPS_AP_SESSION_ACTIVE_IE, &ie_index);
    if (ret != WPS_STATUS_SUCCESS) {
        mwu_printf(MSG_ERROR, "WPS Registrar failed to config\n");
    }
    if (pwps_info->registrar.version >= WPS_VERSION_2DOT0) {
        ret =
            wps_wlan_ie_config(SET_WPS_AP_SESSION_ACTIVE_AR_IE,
                               &ap_assocresp_ie_index);
        if (ret != WPS_STATUS_SUCCESS) {
            mwu_printf(MSG_WARNING, "WPS IE configuration failure.\n");
        }
    }
    LEAVE();
}

/**
 *  @brief Time-out handler of WPS Registration (300 seconds), program will
 *          exit if registration not completed within this time.
 *
 *  @param user_data    A pointer to user data of timer functions
 *  @return             None
 */
void
wps_registration_time_handler(void *user_data)
{
    PWPS_INFO pwps_info;
    int ret = WPS_STATUS_SUCCESS;
    WPS_DATA *wps_s;
    ENTER();

    pwps_info = (PWPS_INFO) user_data;
    wps_s = (WPS_DATA *) & pwps_info->wps_data;

    mwu_printf(DEBUG_WALK_TIME, "WPS Registration Timeout !");

    /* Cancel WPS Registration Timer */
    mwu_printf(DEBUG_WPS_STATE, "Cancelling registration timer!");
    wps_cancel_timer(wps_registration_time_handler, pwps_info);

    /* Cancel packet Tx timer */
    if (pwps_info->set_timer == WPS_SET) {
        wps_cancel_timer(wps_txTimer_handler, pwps_info);
        pwps_info->set_timer = WPS_CANCEL;
        pwps_info->wps_msg_resent_count = 0;
    }

    if (pwps_info->role == WPS_ENROLLEE ||
        (IS_DISCOVERY_ENROLLEE(pwps_info))) {

        printf("\n");
        printf("WPS registration timer time-out.\n");
        printf("WPS Registration Protocol Not Completed !\n\n");
        send_enrollee_fail_event();
    } else if ((wps_s->bss_type == BSS_TYPE_UAP) &&
               (pwps_info->role == WPS_REGISTRAR ||
                pwps_info->role == WPS_ENROLLEE)) {

        /* Shutdown main loop will cause registrar restart */
        printf("WPS Registrartion timeout!!\n");
        printf("Restrating WPS Registrartion.\n");
        wps_print_registration_method_menu(pwps_info);
        pwps_info->input_state = WPS_INPUT_STATE_METHOD;
        pwps_info->pin_pbc_set = WPS_CANCEL;
        ret = wps_wlan_ie_config(SET_WPS_IE, &ie_index);
        if (ret != WPS_STATUS_SUCCESS) {
            mwu_printf(MSG_WARNING, "Setting WPS IE failed!!!");
        }
        if (pwps_info->registrar.version >= WPS_VERSION_2DOT0) {
            ret =
                wps_wlan_ie_config(SET_WPS_AP_SESSION_INACTIVE_AR_IE,
                                   &ap_assocresp_ie_index);
            if (ret != WPS_STATUS_SUCCESS) {
                mwu_printf(MSG_WARNING, "WPS IE configuration failure.\n");
            }
        }
        send_registrar_fail_event();

    } else if (pwps_info->role == WIFIDIR_ROLE) {
        if (pwps_info->discovery_role != WPS_REGISTRAR) {
            printf("\n");
            printf("WPS registration timer time-out.\n");
            printf("WPS Registration Protocol Not Completed !\n\n");
            send_enrollee_fail_event();
        }
        /* Clear WPS IE */
        if (ie_index != -1) {
            wps_wlan_ie_config(CLEAR_WPS_IE, &ie_index);
            ie_index = -1;
        }

    }

    LEAVE();
}

/**
 *  @brief PBC Session Overlap Error handler of WPS Registration
 *  @param user_data    A pointer to user data of timer functions
 *  @return             None
 */
void
wps_registration_pbc_overlap_handler(void *user_data)
{
    PWPS_INFO pwps_info;

    ENTER();

    pwps_info = (PWPS_INFO) user_data;

    mwu_printf(MSG_WARNING,
               "WPS Registration Failed!! PBC Session overlap errror!\n");

    pwps_info->registration_fail = WPS_SET;

    /* Cancel packet Tx timer */
    if (pwps_info->set_timer == WPS_SET) {
        wps_cancel_timer(wps_txTimer_handler, pwps_info);
        pwps_info->set_timer = WPS_CANCEL;
        pwps_info->wps_msg_resent_count = 0;
    }
    if (pwps_info->role == WPS_REGISTRAR) {
        ;
    }
    if (pwps_info->role == WIFIDIR_ROLE) {
        if (pwps_info->discovery_role == WPS_REGISTRAR) {
            printf("\n");
            printf("WPS Registration Protocol Not Completed !\n\n");
        }
        /* Clear WPS IE */
        if (ie_index != -1) {
            wps_wlan_ie_config(CLEAR_WPS_IE, &ie_index);
            ie_index = -1;
        }

    }

    LEAVE();
}

/**
 *  @brief EAPOL-Start handler of WPS Registration
 *  @param pwps_info    A pointer to WPS_INFO structure
 *  @return             None
 */
void
wps_eapol_start_handler(WPS_INFO * pwps_info)
{
    ENTER();

    mwu_printf(MSG_INFO,
               "EAPOL-Start received. Aborting EAPOL State machine...");
    /* Cancel packet Tx timer */
    if (pwps_info->set_timer == WPS_SET) {
        wps_cancel_timer(wps_txTimer_handler, pwps_info);
        pwps_info->set_timer = WPS_CANCEL;
        pwps_info->wps_msg_resent_count = 0;
    }
    mwu_printf(DEBUG_WPS_STATE, "State *->A");
    pwps_info->state = WPS_STATE_A;
    /* Reset Public keys and E-Hash, R-Hash */
    wps_reset_wps_state(pwps_info);
    LEAVE();
}

/**
 *  @brief EAPOL-Start handler of WPS Registration
 *  @param pwps_info    A pointer to WPS_INFO structure
 *  @return             None
 */
void
wps_reset_wps_state(WPS_INFO * pwps_info)
{

    ENTER();
    mwu_printf(MSG_INFO, "Resetting E-HASH, R-HASH and public keys");
    memset(pwps_info->enrollee.e_hash1, 0, 32);
    memset(pwps_info->enrollee.e_hash2, 0, 32);
    memset(pwps_info->enrollee.r_hash1, 0, 32);
    memset(pwps_info->enrollee.r_hash2, 0, 32);
    memset(pwps_info->enrollee.public_key, 0, 192);
    memset(pwps_info->registrar.public_key, 0, 192);
    if (pwps_info->last_message.length) {
        memset(pwps_info->last_message.message, 0,
               pwps_info->last_message.length);
    }
    pwps_info->last_message.length = 0;
    pwps_info->last_recv_wps_msg = 0;
    LEAVE();
}
