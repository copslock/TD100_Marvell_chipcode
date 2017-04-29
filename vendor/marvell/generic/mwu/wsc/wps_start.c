/** @file wps_start.c
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "wps_msg.h"
#include "wps_eapol.h"
#include "mwu_log.h"
#include "wps_def.h"
#include "wps_wlan.h"
#include "wps_os.h"
#include "mwu_timer.h"
#include "mwu_ioctl.h"
#include "mwu.h"

/********************************************************
        Local Variables
********************************************************/

/********************************************************
        Global Variables
********************************************************/
/** IE buffer index */
extern short ie_index;

/** IE buffer index */
extern short ap_assocresp_ie_index;
/** IE buffer index probe request */
extern short probe_ie_index;

/********************************************************
        Local Functions
********************************************************/
/**
 *  @brief WPS PIN value Generate
 *
 *  @param pwps_info    A pointer to WPS_INFO structure
 *  @return             None
 */
void
wps_generate_PIN(PWPS_INFO pwps_info)
{
    int i;

    ENTER();

    if (pwps_info->enrollee.device_password_id == DEVICE_PASSWORD_ID_PIN
        || pwps_info->registrar.device_password_id == DEVICE_PASSWORD_ID_PIN) {
        /* Generate PIN Number */
        u32 pin_number;
        pin_number = get_wps_pin(pwps_info);
        {
            u32 j, temppin = pin_number;
            u8 quotient;

            j = ((pwps_info->PINLen == 8) ? 10000000 : 1000);

            for (i = 0; i < pwps_info->PINLen; i++) {
                quotient = temppin / j;
                pwps_info->PIN[i] = quotient;
                pwps_info->PIN[i] += 0x30;      /* Convert to numeric digit */
                temppin -= j * quotient;
                j = j / 10;
            }
        }
        mwu_hexdump(DEBUG_INIT, "PIN Number", (u8 *) pwps_info->PIN,
                    pwps_info->PINLen);

        printf("\nPIN Number is : ");
        for (i = 0; i < pwps_info->PINLen; i++)
            printf("%c", pwps_info->PIN[i]);
        printf("\n");
    }

    LEAVE();
}

/**
 *  @brief  Validate checksum of PIN
 *
 *  @param PIN      PIN value
 *  @return         Validation result 1 - Success 0 - Failure
 */
static int
ValidateChecksum(unsigned long int PIN)
{
    u32 accum = 0;

    ENTER();

    accum += 3 * ((PIN / 10000000) % 10);
    accum += 1 * ((PIN / 1000000) % 10);
    accum += 3 * ((PIN / 100000) % 10);
    accum += 1 * ((PIN / 10000) % 10);
    accum += 3 * ((PIN / 1000) % 10);
    accum += 1 * ((PIN / 100) % 10);
    accum += 3 * ((PIN / 10) % 10);
    accum += 1 * ((PIN / 1) % 10);
    LEAVE();

    return (0 == (accum % 10));
}

/**
 *  @brief Process user input PIN value
 *
 *  @param pwps_info    A pointer to WPS_INFO structure
 *  @return             None
 */
void
wps_input_PIN(PWPS_INFO pwps_info)
{
    char inp[10];
    u32 wps_pin;
    int i, len = 0, original_pin_len = 0;

    ENTER();

    original_pin_len = pwps_info->PINLen;

    if (pwps_info->registrar.device_password_id == DEVICE_PASSWORD_ID_PIN
        || pwps_info->enrollee.device_password_id == DEVICE_PASSWORD_ID_PIN) {
        do {

            /* Request User to input PIN */
            printf("\nPlease Input %d-digit Numeric PIN : ", original_pin_len);
            fgets(inp, 10, stdin);
            len = (int) strlen(inp) - 1;
            fflush(stdin);

            if (len == original_pin_len) {
                inp[len] = '\0';
                printf("\nInput PIN : %s", inp);
                for (i = 0; i < len; i++) {
                    pwps_info->PIN[i] = inp[i];
                }

                wps_pin = (u32) (strtoul((char *) pwps_info->PIN, NULL, 10));

                if (ValidateChecksum(wps_pin))
                    mwu_printf(DEBUG_INPUT, "\nChecksum validation successful");
                else {
                    printf("\nChecksum validation failed");
                    continue;

                }
                break;
            } else {
                printf
                    ("Input PIN length and configuration file PIN length doesn't match.\n");
                fflush(stdin);
            }

        } while (1);

    }

    pwps_info->input_state = WPS_INPUT_STATE_NO_INPUT;
    pwps_info->pin_pbc_set = WPS_SET;
    LEAVE();
}

/**
 *  @brief Check if scan result contain any dual band AP entry
 *
 *  @param pwps_info    A pointer to WPS_INFO structure
 *  @param wps_s        A pointer to WPS_DATA structure
 *  @return             WPS_STATUS_SUCCESS--success, otherwise--fail
 */
static int
wps_wlan_check_for_dual_band_ap(PWPS_INFO pwps_info,
                                SCAN_RESULTS * pdual_band_scan_results)
{
#define BYPASS_DUAL_BAND_CHECK 1
    int ret = WPS_STATUS_SUCCESS;
    u8 pbc_registrar_count = 0;
    int i = 0;
    u16 device_password_id = 0xffff;
#if BYPASS_DUAL_BAND_CHECK
    u8 uuid_e[2][16];
    u16 ie_length;
#endif

    ENTER();
#if BYPASS_DUAL_BAND_CHECK

    for (i = 0; i < 2; i++) {

        ie_length = pdual_band_scan_results[i].wps_ie_len;
        if (ie_length > 0) {
            ret =
                wps_probe_response_uuid_parser(pdual_band_scan_results[i].
                                               wps_ie,
                                               pdual_band_scan_results[i].
                                               wps_ie_len, &uuid_e[i][0]);
            if (ret != WPS_STATUS_SUCCESS) {
                printf("\n");
                printf("\nCould not retrieve UUID from scan result!");
                ret = WPS_STATUS_FAIL;
                LEAVE();
                return ret;
            }
        }
    }

    mwu_hexdump(DEBUG_WPS_MSG, "UUID[0]", &uuid_e[0][0], 16);
    mwu_hexdump(DEBUG_WPS_MSG, "UUID[1]", &uuid_e[1][0], 16);

    if (memcmp(&uuid_e[0][0], &uuid_e[1][0], 16) == 0) {
        /* Same UUID, dual-band AP */
        mwu_printf(MSG_INFO, "Dual-band AP detected in PBC auto mode");
    } else {
#endif
        for (i = 0; i < 2; i++) {
            device_password_id =
                wps_probe_response_device_password_id_parser
                (pdual_band_scan_results[i].wps_ie,
                 pdual_band_scan_results[i].wps_ie_len);
            if (device_password_id == pwps_info->enrollee.device_password_id) {

                pbc_registrar_count++;
            }
        }
        if (pbc_registrar_count > 1) {
            switch (pwps_info->enrollee.device_password_id) {
            case DEVICE_PASSWORD_PUSH_BUTTON:
                printf("\nPBC Session Overlap Detected!");
                printf("\n");
                break;
            case DEVICE_PASSWORD_SMPBC:
                printf("\nSMPBC Session Overlap Detected!");
                printf("\n");
                break;
            }
            ret = WPS_STATUS_FAIL;
        }
#if BYPASS_DUAL_BAND_CHECK
    }
#endif
    LEAVE();
    return ret;
}

/**
 * @brief Checks wheather PBC overlap condition has occured during scan
 * @param wps_s        A pointer to global WPS structure
 * @return             WPS_STATUS_SUCCESS--success, otherwise--fail
 *
 * */
int
wps_wlan_check_for_PBC_overlap(PWPS_INFO pwps_info, WPS_DATA * wps_s)
{
    int i = 0, ret = WPS_STATUS_SUCCESS;
    u8 pbc_registrar_count = 0;
    u16 device_password_id = 0xffff;
    SCAN_RESULTS *pscan_results = NULL;
    ENTER();
    if (wps_s->num_scan_results == 1) {
        printf("\nOne AP with Active PBC Session Detected !\n");
        return WPS_STATUS_SUCCESS;
    }

    /* Check for dual-band AP */
    if (wps_s->num_scan_results == 2) {

        pscan_results = (SCAN_RESULTS *) malloc(sizeof(SCAN_RESULTS) * 2);
        if( pscan_results == NULL) {
            return WPS_STATUS_FAIL;
        }
        for (i = 0; i < 2; i++)
            memcpy(&pscan_results[i], &wps_s->scan_results[i],
                   sizeof(SCAN_RESULTS));
        ret = wps_wlan_check_for_dual_band_ap(pwps_info, pscan_results);
    }

    else {
        for (i = 0; i < wps_s->num_scan_results; i++) {
            device_password_id =
                wps_probe_response_device_password_id_parser(wps_s->
                                                             scan_results
                                                             [i].wps_ie,
                                                             wps_s->
                                                             scan_results
                                                             [i].wps_ie_len);

            if (device_password_id == pwps_info->enrollee.device_password_id) {
                pbc_registrar_count++;
                pscan_results = (SCAN_RESULTS *) realloc(pscan_results,
                                    sizeof(SCAN_RESULTS) * pbc_registrar_count);
                memcpy(&pscan_results[pbc_registrar_count-1],
                       &wps_s->scan_results[i], sizeof(SCAN_RESULTS));
            }
        }
        if (pbc_registrar_count > 2) {
            printf("\n");
            printf("PBC Session Overlap Detected!");
            printf("\n");
            ret = WPS_STATUS_FAIL;
        } else if (pbc_registrar_count == 2) {
            ret = wps_wlan_check_for_dual_band_ap(pwps_info, pscan_results);
        }
    }

    if (pscan_results)
        free(pscan_results);
    LEAVE();
    return ret;
}

static int
do_wps_associate(PWPS_INFO pwps_info, WPS_DATA *wps_s)
{
    int index;
    u8 bssid_get[ETH_ALEN];
    int flags;

    index = wps_s->wifidir_data.dev_found;
    /* Reset registrar's go_ssid */
    memset(pwps_info->registrar.go_ssid, 0, MAX_SSID_LEN + 1);

    printf("\nConnecting ..... \n");
    wps_wlan_set_authentication(AUTHENTICATION_TYPE_OPEN);
    if (mwu_set_ssid(wps_s->ifname,
                     (unsigned char*) wps_s->scan_results[index].ssid,
                     wps_s->scan_results[index].ssid_len,
                     0) != MWU_ERR_SUCCESS)
        return WPS_STATUS_FAIL;
    memset(bssid_get, 0x00, ETH_ALEN);
    wps_wlan_get_wap(bssid_get);

    if ((memcmp
        (bssid_get, wps_s->scan_results[index].bssid, ETH_ALEN) == 0)) {

        printf("\nConnected to following BSS (or IBSS) :\n");
        printf("SSID = [%s], BSSID = [%02x:%02x:%02x:%02x:%02x:%02x]\n\n",
             wps_s->scan_results[index].ssid,
             wps_s->scan_results[index].bssid[0],
             wps_s->scan_results[index].bssid[1],
             wps_s->scan_results[index].bssid[2],
             wps_s->scan_results[index].bssid[3],
             wps_s->scan_results[index].bssid[4],
             wps_s->scan_results[index].bssid[5]);

        /* Save information to global structure */
        wps_s->current_ssid.ssid_len =
            wps_s->scan_results[index].ssid_len;
        memcpy(wps_s->current_ssid.ssid,
               wps_s->scan_results[index].ssid,
               wps_s->current_ssid.ssid_len);

        memcpy(wps_s->current_ssid.bssid,
               wps_s->scan_results[index].bssid, ETH_ALEN);

        /* Store Peer MAC Address */
        if (pwps_info->role == WPS_ENROLLEE
#if defined(WIFIDIR_SUPPORT)
            || (IS_DISCOVERY_ENROLLEE(pwps_info))
#endif
            ) {
        memcpy(pwps_info->registrar.mac_address,
               wps_s->scan_results[index].bssid, ETH_ALEN);
        }

        /* Save RF Band information for M1 message */
        pwps_info->enrollee.rf_bands =
            (u8) wps_wlan_freq_to_band(wps_s->scan_results[index].
                                       freq);

        /* Disable driver WPS session checking */
        wps_wlan_session_control(WPS_SESSION_OFF);

        printf("WPS Registration Protocol Starting ..... \n\n");

        if (wps_wlan_get_ifflags(&flags) != 0 ||
            wps_wlan_set_ifflags(flags | IFF_UP) != 0) {
            mwu_printf(MSG_WARNING,
                       "Could not set interface '%s' UP\n",
                       wps_s->ifname);
            printf("Error setting interface UP... Cant proceed.\n");
            return WPS_STATUS_FAIL;
        }

        /* wait for interface up */
        mwu_sleep(2, 0);

        /* Start WPS registration timer */
        wps_start_registration_timer(pwps_info);

        /* Starting WPS Message Exchange Engine */
        wps_state_machine_start(pwps_info);

        return WPS_STATUS_SUCCESS;
    }

    printf("Association failure... Cant proceed.\n");
    return WPS_STATUS_FAIL;
}

/**
 *  @brief Process WPS Enrollee PIN mode and PBC user selection operations
 *
 *  @param pwps_info    A pointer to WPS_INFO structure
 *  @param wps_s        A pointer to global WPS structure
 *  @return             WPS_STATUS_SUCCESS--success, otherwise--fail
 */
int
wps_enrollee_start(PWPS_INFO pwps_info, WPS_DATA * wps_s)
{
    int ret = WPS_STATUS_SUCCESS;
    int retry_cnt = 3;
    struct wifidir_peer *peer = NULL;
    char device_addr[ETH_ALEN];
    WIFIDIR_DATA *pwifidir_data = &(pwps_info->wps_data.wifidir_data);
    int op_channel = 0;
    int scan_channel_list = 0;

    ENTER();

    pwps_info->enrollee.auth_type_flag = AUTHENTICATION_TYPE_ALL;
    pwps_info->enrollee.encry_type_flag = ENCRYPTION_TYPE_ALL;

    if (wps_s->bss_type == BSS_TYPE_UAP && pwps_info->role == WPS_ENROLLEE) {

        /* Starting WPS Message Exchange Engine */
        ret = wps_state_machine_start(pwps_info);
        LEAVE();
        return ret;
    }

    /* Enrollee need to do the scanning function */
    /* Enable driver WPS session checking */
    wps_wlan_session_control(WPS_SESSION_ON);

    if (pwps_info->role == WIFIDIR_ROLE) {
        /* reset dev_found */
        wps_s->wifidir_data.dev_found = -1;
        wps_wlan_scan_results(wps_s, FILTER_NONE);
        /* If registrar is already in driver's scan table, directly associate */
        if (wps_s->wifidir_data.dev_found != -1) {
            printf("\nRegistrar is already in scan table\n");
            LEAVE();
            return do_wps_associate(pwps_info, wps_s);
        }
    }

    printf("\n");
    printf("Start Active Scan ...\n");

    /* Get the information about the current peer */
    if((pwifidir_data->current_peer != NULL) &&
            (memcmp(pwifidir_data->current_peer->interface_address,
                    pwps_info->registrar.mac_address, ETH_ALEN) == 0))
        peer = pwifidir_data->current_peer;
    else
        peer = NULL;

    if(peer == NULL) {
        ERR("Current is not updated yet!!\n");
        return WPS_STATUS_FAIL;
    } else {
        op_channel = peer->op_channel;

        if(peer->peer_channel_list.num_of_chan &&
                pwifidir_data->self_channel_list.num_of_chan)
            scan_channel_list = 1;

        if(strncmp(pwps_info->registrar.go_ssid, peer->go_ssid, MAX_SSID_LEN) != 0){
            memcpy(pwps_info->registrar.go_ssid, peer->go_ssid, MAX_SSID_LEN + 1);
        }
    }

    while (retry_cnt >= 0) {
        if (pwps_info->role == WPS_ADHOC_EXTENTION
#ifdef WSC2
            || (pwps_info->enrollee.version >= WPS_VERSION_2DOT0)
#endif
            ) {

            if(probe_ie_index == -1) {
                mwu_printf(DEBUG_INIT, "GET_PROBE_REQ_WPS_IE\n");
                ret = wps_wlan_probe_request_ie_config(GET_WPS_IE, &probe_ie_index);
                if (ret != WPS_STATUS_SUCCESS) {
                    mwu_printf(MSG_ERROR, "Failed to set STA Probe Req IEs\n");
                    return WPS_STATUS_FAIL;
                }
            }

            /* configure Probe request IE */
            wps_wlan_probe_request_ie_config(SET_WPS_STA_SESSION_ACTIVE_IE,
                                             &probe_ie_index);
        }
        /* If the role is WIFIDIR_ROLE, we will use setuserscan instead of
         * regular scan to save time. Note that setuserscan is triggerred in
         * wifidir module.
         */
        if (pwps_info->role == WIFIDIR_ROLE) {
            if(op_channel) {
                /* Scan op channel */
                wps_wlan_set_user_scan(pwifidir_data, pwps_info->registrar.go_ssid,
                        wps_s->ifname, SCAN_OP_CHANNEL);
                op_channel = 0;
            } else if(scan_channel_list) {
                /* Scan common channel list*/
                wps_wlan_set_user_scan(pwifidir_data, pwps_info->registrar.go_ssid,
                        wps_s->ifname, SCAN_COMMON_CHANNEL_LIST);
                scan_channel_list = 0;
            } else {
                /* Perform generic scan */
                wps_wlan_set_user_scan(pwifidir_data, pwps_info->registrar.go_ssid,
                        wps_s->ifname, SCAN_GENERIC);
            }
        } else
            wps_wlan_scan(wps_s);

        /* reset dev_found */
        wps_s->wifidir_data.dev_found = -1;
        wps_wlan_scan_results(wps_s, FILTER_NONE);

        if (pwps_info->role == WPS_ADHOC_EXTENTION
#ifdef WSC2
            || (pwps_info->enrollee.version >= WPS_VERSION_2DOT0)
#endif
            ) {
            /* De-configure Probe request IE */
            wps_wlan_probe_request_ie_config(CLEAR_WPS_IE, &probe_ie_index);
        }

        /* Go ahead to connect to device found */
        if (pwps_info->role == WIFIDIR_ROLE) {
            if (wps_s->wifidir_data.dev_found != -1) {
                LEAVE();
                return do_wps_associate(pwps_info, wps_s);
            } else {
                /* scan again to see if the device is up now. */
                mwu_printf(DEBUG_WIFIDIR_DISCOVERY,
                           "Device not found in scan list..\n");
                if (retry_cnt) {
                    printf("Scanning again for selected device...%d\n", retry_cnt);
                    mwu_sleep(0, 500000);
                }
            }
        }

        --retry_cnt;
    } /* end of while (retry_cnt >= 0) */

    /* bail out after 10 scan commands */
    printf("Device not found around, bailing out...\n");
    LEAVE();
    return WPS_STATUS_FAIL;
}

/**
 *  @brief Process WPS Enrollee Push Button auto detection and connection operations
 *
 *  @param pwps_info    A pointer to WPS_INFO structure
 *  @param wps_s        A pointer to global WPS structure
 *  @return             WPS_STATUS_SUCCESS--success, otherwise--fail
 */
int
wps_enrollee_start_PBCAuto(PWPS_INFO pwps_info, WPS_DATA * wps_s)
{
    int ret = WPS_STATUS_SUCCESS;
    char inp[30];
    int index = 0;
    int flags;
    int retry_cnt = 50;
    u8 bssid_get[ETH_ALEN];
    struct mwu_timeval start, now, res;
    struct mwu_timeval pbcWalkTime;

    ENTER();

    /*
     * For PBC-auto, STA will maintain 2-min walk time to scan PBC
     *  AP periodically.
     * 1) If no PBC AP found, app exits when 2-min timer timeout.
     * 2) If overlapped PBC found, app prints an error message, cancels
     *     timer and exits.
     * 3) If one PBC AP found (including dual-band AP), app cancels timer
     *     and starts WPS connection/registration automatically.
     */

    pwps_info->enrollee.auth_type_flag = AUTHENTICATION_TYPE_ALL;
    pwps_info->enrollee.encry_type_flag = ENCRYPTION_TYPE_ALL;

    printf("\n");
    printf("Start Active Scan ...\n");

    /* Set PBC Walk Timer to 120 second */
    pbcWalkTime.sec = PBC_WALK_TIME;
    pbcWalkTime.usec = 0;

    /* Mark the start of PBC Walk Timer */
    mwu_get_time(&start);

    /* Enable driver WPS session checking */
    wps_wlan_session_control(WPS_SESSION_ON);

    /* Enrollee need to do the scanning function */
  pbc_scan:

    /* Compare time of PBC Walk Timer */
    mwu_get_time(&now);
    mwu_sub_time(&now, &start, &res);
    if (mwu_cmp_time(&res, &pbcWalkTime) >= 0) {
        printf("\n");
        printf("\nExit program for PBC walk time expired.\n");
        printf("\n\n");

        ret = WPS_STATUS_FAIL;
        goto exit;
    }
    if (pwps_info->role == WPS_ADHOC_EXTENTION
#ifdef WSC2
        || (pwps_info->enrollee.version >= WPS_VERSION_2DOT0)
#endif
        ) {

        if(probe_ie_index == -1) {
            mwu_printf(DEBUG_INIT, "GET_PROBE_REQ_WPS_IE\n");
            ret = wps_wlan_probe_request_ie_config(GET_WPS_IE, &probe_ie_index);
            if (ret != WPS_STATUS_SUCCESS) {
                mwu_printf(MSG_ERROR, "Failed to set STA Probe Req IEs\n");
                goto exit;
            }
        }
        /* configure Probe request IE */
        wps_wlan_probe_request_ie_config(SET_WPS_STA_SESSION_ACTIVE_IE,
                                         &probe_ie_index);
    }
    wps_wlan_scan(wps_s);

    mwu_sleep(0, 500000);
    wps_wlan_scan_results(wps_s, FILTER_PBC);

    if (pwps_info->role == WPS_ADHOC_EXTENTION
#ifdef WSC2
        || (pwps_info->enrollee.version >= WPS_VERSION_2DOT0)
#endif
        ) {
        /* De-configure Probe request IE */
        wps_wlan_probe_request_ie_config(CLEAR_WPS_IE, &probe_ie_index);
    }
    if (wps_s->num_scan_results == 0) {
        /* Scanning again */
        goto pbc_scan;
    } else {

        ret = wps_wlan_check_for_PBC_overlap(pwps_info, wps_s);
        if (WPS_STATUS_SUCCESS != ret) {
            LEAVE();
            return ret;
        }
        /* mode: 0 = infra/BSS, 1 = adhoc/IBSS */
        if (wps_s->scan_results[0].caps & IEEE80211_CAP_ESS) {
            wps_wlan_set_mode(IEEE80211_MODE_INFRA);
            wps_s->current_ssid.mode = IEEE80211_MODE_INFRA;
            pwps_info->mode = IEEE80211_MODE_INFRA;
        } else if (wps_s->scan_results[0].caps & IEEE80211_CAP_IBSS) {
            wps_wlan_set_mode(IEEE80211_MODE_IBSS);
            wps_s->current_ssid.mode = IEEE80211_MODE_IBSS;
            pwps_info->mode = IEEE80211_MODE_IBSS;
        }

        if (wps_s->scan_results[0].caps & IEEE80211_CAP_ESS) {
            if (pwps_info->role == WPS_ENROLLEE &&
                pwps_info->enrollee.version >= WPS_VERSION_2DOT0) {
                /* Parsing Authorized MAC address field for ENROLLEE mac
                   address */
                if (wps_s->scan_results[0].wps_ie_len != 0) {
                    ret =
                        wps_probe_response_authorized_enrollee_mac_parser
                        (pwps_info, (u8 *) & wps_s->scan_results[0].wps_ie,
                         wps_s->scan_results[0].wps_ie_len);
                }
                if (pwps_info->enrollee_in_authorized_mac == WPS_CANCEL) {
                    printf
                        ("Enrollee is not authorized to registrer with this Registrar.\n");
                    do {
                        printf("Select Option to continue\n");
                        printf("[0]. Quit\n");
                        printf("[1]. Searching Again\n");
                        printf("Enter Selection : ");
                        if (!fgets(inp, sizeof(inp) - 1, stdin))
                            continue;
                        fflush(stdin);

                        if (!isdigit(inp[0]))
                            continue;

                        index = atoi((char *) &inp[0]);

                        if (index == 0) {
                            ret = WPS_STATUS_FAIL;
                            LEAVE();
                            return ret;
                        } else if (index == 1) {
                            goto pbc_scan;
                        }
                    } while (1);
                } else {
                    mwu_printf(DEBUG_EAPOL,
                               "Enrollee's Mac Address found in Registrar's Authorized Enrollee list.Continue...");
                }
            }
        }
        do {
            printf("\nConnecting ..... \n");

            wps_wlan_set_authentication(AUTHENTICATION_TYPE_OPEN);

            wps_wlan_set_wap((u8 *) wps_s->scan_results[0].bssid);

            /* wait for interface up */
            mwu_sleep(1, 0);

            memset(bssid_get, 0x00, ETH_ALEN);
            wps_wlan_get_wap(bssid_get);
            if ((memcmp(bssid_get, wps_s->scan_results[0].bssid, ETH_ALEN) ==
                 0)) {

                printf("\nConnected to following BSS (or IBSS) :\n");
                printf
                    ("SSID = [%s], BSSID = [%02x:%02x:%02x:%02x:%02x:%02x]\n\n",
                     wps_s->scan_results[0].ssid,
                     wps_s->scan_results[0].bssid[0],
                     wps_s->scan_results[0].bssid[1],
                     wps_s->scan_results[0].bssid[2],
                     wps_s->scan_results[0].bssid[3],
                     wps_s->scan_results[0].bssid[4],
                     wps_s->scan_results[0].bssid[5]);

                /* Save information to global structure */
                wps_s->current_ssid.ssid_len = wps_s->scan_results[0].ssid_len;
                memcpy(wps_s->current_ssid.ssid, wps_s->scan_results[0].ssid,
                       wps_s->current_ssid.ssid_len);

                memcpy(wps_s->current_ssid.bssid,
                       wps_s->scan_results[0].bssid, ETH_ALEN);

                /* Store Peer MAC Address */
                if (pwps_info->role == WPS_ENROLLEE
                    || (IS_DISCOVERY_ENROLLEE(pwps_info))
                    ) {
                    memcpy(pwps_info->registrar.mac_address,
                           wps_s->scan_results[0].bssid, ETH_ALEN);
                }

                /* Save RF Band information for M1 message */
                pwps_info->enrollee.rf_bands =
                    (u8) wps_wlan_freq_to_band(wps_s->scan_results[0].freq);

                /* Disable driver WPS session checking */
                wps_wlan_session_control(WPS_SESSION_OFF);

                printf("WPS Registration Protocol Starting ..... \n\n");

                if (wps_wlan_get_ifflags(&flags) != 0 ||
                    wps_wlan_set_ifflags(flags | IFF_UP) != 0) {
                    mwu_printf(MSG_WARNING, "Could not set interface '%s' UP\n",
                               wps_s->ifname);
                }

                /* Start WPS registration timer */
                wps_start_registration_timer(pwps_info);

                /* Starting WPS Message Exchange Engine */
                wps_state_machine_start(pwps_info);

                ret = WPS_STATUS_SUCCESS;
                break;
            } else {
                retry_cnt--;
                mwu_printf((DEBUG_WLAN | MSG_ERROR),
                           "Connect to AP FAIL ! Retrying ..... ");
                mwu_printf((DEBUG_WLAN | DEBUG_INIT), "Retry Count = %d",
                           retry_cnt);

                /* Compare time of PBC Walk Timer */
                mwu_get_time(&now);
                mwu_sub_time(&now, &start, &res);
                if (mwu_cmp_time(&res, &pbcWalkTime) >= 0) {
                    ret = WPS_STATUS_FAIL;
                    goto exit;
                }

                /* Enable driver WPS session checking */
                wps_wlan_session_control(WPS_SESSION_ON);
            }
        } while (retry_cnt != 0);

        if (retry_cnt == 0) {
            printf("\n");
            printf("\nTry to connect to AP FAIL !");
            printf("\n\n");

            ret = WPS_STATUS_FAIL;
        }
    }

  exit:
    LEAVE();
    return ret;
}

/**
 *  @brief Process WPS Registrar operations
 *
 *  @param pwps_info    A pointer to WPS_INFO structure
 *  @param wps_s        A pointer to global WPS structure
 *  @return             WPS_STATUS_SUCCESS--success, otherwise--fail
 */
int
wps_registrar_start(PWPS_INFO pwps_info, WPS_DATA * wps_s)
{
    int ret = WPS_STATUS_SUCCESS;
    int flags;

    ENTER();

    pwps_info->registrar.auth_type_flag = AUTHENTICATION_TYPE_ALL;
    pwps_info->registrar.encry_type_flag = ENCRYPTION_TYPE_ALL;

    if (wps_s->bss_type == BSS_TYPE_STA
        && pwps_info->role == WPS_ADHOC_EXTENTION) {
        /* For STA mode */
        wps_s->current_ssid.mode = IEEE80211_MODE_IBSS;
        pwps_info->mode = IEEE80211_MODE_IBSS;

        /* REMOVED ::: Send adhoc registrar config command ADDED ::: MGMT_IE
           command */
        /* Get the free IE buffer index */

        if (ie_index == -1) {
            ret = wps_wlan_ie_config(GET_WPS_IE, &ie_index);
            if (ret != WPS_STATUS_SUCCESS) {
                printf("WPS Registrar failed to start\n");
                goto _exit_;
            }
        }
#if 0
#endif
    }
#ifdef UAP_SUPPORT
    else if (wps_s->bss_type == BSS_TYPE_UAP) {
        /* For UAP mode */
        wps_s->current_ssid.mode = IEEE80211_MODE_INFRA;
        pwps_info->mode = IEEE80211_MODE_INFRA;
    }
#endif

    if (wps_wlan_get_ifflags(&flags) != 0 ||
        wps_wlan_set_ifflags(flags | IFF_UP) != 0) {
        mwu_printf(MSG_WARNING, "Could not set interface '%s' UP\n",
                   wps_s->ifname);
    }

    /* Start registration timer for WIFIDIR registrar */
    if (pwps_info->role == WIFIDIR_ROLE) {
        wps_start_registration_timer(pwps_info);

    }

    /* Starting WPS Message Exchange Engine */
    wps_state_machine_start(pwps_info);

  _exit_:
    LEAVE();
    return ret;
}
