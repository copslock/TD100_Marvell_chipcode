/** @file wps_main.c
 *  @brief This file contains WPS application program entry function
 *           and functions for initialization setting.
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
#include "mwu_log.h"
#include "util.h"
#include "wps_def.h"
#include "wps_wlan.h"
#include "wps_os.h"
#include "wsc/encrypt_src/sha1.h"
#include "wsc/encrypt_src/crypto.h"

/********************************************************
        Local Variables
********************************************************/

/********************************************************
        Global Variables
********************************************************/
/** Global pwps information */
extern PWPS_INFO gpwps_info;

/** IE buffer index */
short ie_index = -1;
/** IE buffer index */
short ap_assocresp_ie_index = -1;
/** IE buffer index assoc request */
short assocreq_ie_index = -1;
/** IE buffer index probe request */
short probe_ie_index = -1;
/** Configuration file for initialization */
char init_cfg_file[100];

/** WIFIDIR Config file for initial download */
char wifidir_cfg_file[100];
/** Autonomous GO flag */
int auto_go = 0;

/** WPS PIN/PBC menu show only once */
int wps_method_menu_shown = 0;

/********************************************************
        Local Functions
********************************************************/
/**
 *  @brief Skip the space in file reading
 *
 *  @param fp       A pointer to FILE stream
 *  @return         None
 */
static void
wps_config_skipspace(FILE * fp)
{
    int c;
    while (!feof(fp)) {
        if (isspace((c = fgetc(fp))))
            continue;
        ungetc(c, fp);
        break;
    }
}

/**
 *  @brief Parsing and saving WPS parameter to global used structure
 *
 *  @param p_line       A pointer to FILE stream
 *  @param pwps_info    A pointer to WPS_INFO structure
 *  @param wps_s        A pointer to global WPS structure
 *  @return             WPS_STATUS_SUCCESS--success, otherwise--fail
 */
static int
wps_config_parser(char *p_line, PWPS_INFO pwps_info, WPS_DATA * wps_s)
{
    char *p = NULL;

    /* Check for comment line */
    if ('#' == *p_line)
        return WPS_STATUS_SUCCESS;

    if (strstr(p_line, "ROLE")) {
        p = strchr(p_line, '=');
        if (!p) {
            return WPS_STATUS_FAIL;
        }

        /* Enrollee: 1, Registrar: 2, WPSE: 3, WIFIDIR: 4 */
        int val = atoi(++p);
        if (val == WPS_ENROLLEE || val == WPS_REGISTRAR ||
            val == WPS_ADHOC_EXTENTION || val == WIFIDIR_ROLE) {
            pwps_info->role = (u8) val;
        } else {
            pwps_info->role = WPS_ENROLLEE;
        }
        mwu_printf(DEBUG_INIT, "pwps_info->role= %d", pwps_info->role);
    } else if (strstr(p_line, "PIN_GENERATOR")) {
        p = strchr(p_line, '=');
        if (!p) {
            return WPS_STATUS_FAIL;
        }

        /* Enrollee: 1, Registrar: 2, WPSE: 3, WIFIDIR: 4 */
        int val = atoi(++p);
        pwps_info->pin_generator = (u8) val;
        if (pwps_info->role == WIFIDIR_ROLE) {
            if (val == WIFIDIR_AUTO_ENTER_PIN) {
                pwps_info->pin_generator = (u8) val;
            } else if (val == WIFIDIR_AUTO_DISPLAY_PIN) {
                pwps_info->pin_generator = (u8) val;
            } else {
                printf
                    ("Invalid PIN generator value. Setting Default Display PIN.\n");
                pwps_info->pin_generator = WIFIDIR_AUTO_DISPLAY_PIN;
            }
        } else {
        }
        mwu_printf(DEBUG_INIT, "pwps_info->pin_generator= %d",
                   pwps_info->pin_generator);
    }

    else if (strstr(p_line, "UUID")) {
        p = strchr(p_line, '=');
        if (!p) {
            return WPS_STATUS_FAIL;
        }

        int i;
        char temp[10];
        memset(&temp, 0, 10);
        temp[0] = '0';
        temp[1] = 'x';
        /* Move past the '0x' */
        p += 1;

        for (i = 0; i <= 15; i++) {
            p += 2;
            strncpy(&temp[2], p, 2);
            pwps_info->enrollee.wps_uuid[i] = (u8) (strtoul(temp, NULL, 16));
            pwps_info->registrar.wps_uuid[i] = (u8) (strtoul(temp, NULL, 16));
        }
        pwps_info->enrollee.wps_uuid_length = WPS_UUID_MAX_LEN;
        pwps_info->registrar.wps_uuid_length = WPS_UUID_MAX_LEN;
        mwu_printf(DEBUG_INIT, "pwps_info->enrollee.wps_uuid_length=%d",
                   pwps_info->enrollee.wps_uuid_length);
        mwu_hexdump(DEBUG_INIT, "pwps_info->enrollee.wps_uuid",
                    pwps_info->enrollee.wps_uuid,
                    pwps_info->enrollee.wps_uuid_length);
        mwu_printf(DEBUG_INIT, "pwps_info->registrar.wps_uuid_length=%d",
                   pwps_info->registrar.wps_uuid_length);
        mwu_hexdump(DEBUG_INIT, "pwps_info->registrar.wps_uuid",
                    pwps_info->registrar.wps_uuid,
                    pwps_info->registrar.wps_uuid_length);
    } else if (strstr(p_line, "VERSION")) {
        p = strchr(p_line, '=');
        if (!p) {
            return WPS_STATUS_FAIL;
        }

        /* currently, only support version 1.0 and 1.1 */
        char version[10];
        memset(&version, 0, 10);
        version[0] = '0';
        version[1] = 'x';
        /* Move past the '0x' */
        p += 3;
        strncpy(&version[2], p, 2);
        pwps_info->enrollee.version = (u8) (strtoul(version, NULL, 16));
        pwps_info->registrar.version = (u8) (strtoul(version, NULL, 16));

        mwu_printf(DEBUG_INIT, "WPS Version= 0x%02x",
                   pwps_info->enrollee.version);

        if (pwps_info->enrollee.version != 0x10 &&
            pwps_info->enrollee.version != 0x11 &&
            pwps_info->enrollee.version != 0x20) {
            mwu_printf(MSG_WARNING, "Using unknown WPS version = 0x%02x",
                       pwps_info->enrollee.version);
        }
    } else if (strstr(p_line, "DEVICE_NAME")) {
        p = strchr(p_line, '=');
        if (!p) {
            return WPS_STATUS_FAIL;
        }

        unsigned int len = strlen(++p);
        len = (len > 32) ? 32 : len;

        strncpy((char *) pwps_info->enrollee.device_name, p, len);
        strncpy((char *) pwps_info->registrar.device_name, p, len);
        pwps_info->enrollee.device_name_length = len;
        pwps_info->registrar.device_name_length = len;

        mwu_printf(DEBUG_INIT, "pwps_info->enrollee.device_name '%s'",
                   pwps_info->enrollee.device_name);
        mwu_printf(DEBUG_INIT, "pwps_info->registrar.device_name '%s'",
                   pwps_info->registrar.device_name);
    } else if (strstr(p_line, "MANUFACTURER")) {
        p = strchr(p_line, '=');
        if (!p) {
            return WPS_STATUS_FAIL;
        }

        unsigned int len = strlen(++p);

        len = (len > 64) ? 64 : len;

        strncpy((char *) pwps_info->enrollee.manufacture, p, len);
        strncpy((char *) pwps_info->registrar.manufacture, p, len);
        pwps_info->enrollee.manufacture_length = len;
        pwps_info->registrar.manufacture_length = len;

        mwu_printf(DEBUG_INIT, "pwps_info->enrollee.manufacture '%s'",
                   pwps_info->enrollee.manufacture);
        mwu_printf(DEBUG_INIT, "pwps_info->registrar.manufacture '%s'",
                   pwps_info->registrar.manufacture);
    } else if (strstr(p_line, "MODEL_NAME")) {
        p = strchr(p_line, '=');
        if (!p) {
            return WPS_STATUS_FAIL;
        }

        unsigned int len = strlen(++p);

        len = (len > 32) ? 32 : len;
        strncpy((char *) pwps_info->enrollee.model_name, p, len);
        strncpy((char *) pwps_info->registrar.model_name, p, len);
        pwps_info->enrollee.model_name_length = len;
        pwps_info->registrar.model_name_length = len;

        mwu_printf(DEBUG_INIT, "pwps_info->enrollee.model_name '%s'",
                   pwps_info->enrollee.model_name);
        mwu_printf(DEBUG_INIT, "pwps_info->registrar.model_name '%s'",
                   pwps_info->registrar.model_name);
    } else if (strstr(p_line, "MODEL_NUMBER")) {
        p = strchr(p_line, '=');
        if (!p) {
            return WPS_STATUS_FAIL;
        }

        unsigned int i;
        unsigned int len = strlen(++p);
        len = (len > MODEL_NUMBER_LEN) ? MODEL_NUMBER_LEN : len;
        for (i = 0; i < len; ++i) {
            pwps_info->enrollee.model_number[i]  = hexc2bin(p[i]);
            pwps_info->registrar.model_number[i] = hexc2bin(p[i]);
        }
        pwps_info->enrollee.model_number_length  = len;
        pwps_info->registrar.model_number_length = len;
    } else if (strstr(p_line, "SERIAL_NUMBER")) {
        p = strchr(p_line, '=');
        if (!p) {
            return WPS_STATUS_FAIL;
        }

        unsigned int i;
        unsigned int len = strlen(++p);
        len = (len > SERIAL_NUMBER_LEN) ? SERIAL_NUMBER_LEN : len;
        for (i = 0; i < len; ++i) {
            pwps_info->enrollee.serial_number[i]  = hexc2bin(p[i]);
            pwps_info->registrar.serial_number[i] = hexc2bin(p[i]);
        }
        pwps_info->enrollee.serial_number_length  = len;
        pwps_info->registrar.serial_number_length = len;
    } else if (strstr(p_line, "CONFIG_METHODS")) {
        p = strchr(p_line, '=');
        if (!p) {
            return WPS_STATUS_FAIL;
        }

        char config[10];
        memset(&config, 0, 10);
        config[0] = '0';
        config[1] = 'x';
        /* Move past the '0x' */
        p += 3;
        strncpy(&config[2], p, 4);
        pwps_info->enrollee.config_methods = (u16) (strtoul(config, NULL, 16));
        pwps_info->registrar.config_methods =
            pwps_info->enrollee.config_methods;
        mwu_printf(DEBUG_INIT, "pwps_info->enrollee.config_methods=0x%04x",
                   pwps_info->enrollee.config_methods);
        mwu_printf(DEBUG_INIT, "pwps_info->registrar.config_methods=0x%04x",
                   pwps_info->registrar.config_methods);
    } else if (strstr(p_line, "RF_BAND")) {
        u8 rfBand;

        p = strchr(p_line, '=');
        if (!p) {
            return WPS_STATUS_FAIL;
        }

        rfBand = atoi(++p);
        if (rfBand == 1 || rfBand == 2) {
            pwps_info->enrollee.rf_bands = (u8) rfBand;
            pwps_info->registrar.rf_bands = (u8) rfBand;
        } else {
            pwps_info->enrollee.rf_bands = RF_24_G;
            pwps_info->registrar.rf_bands = RF_24_G;
        }
        mwu_printf(DEBUG_INIT, "pwps_info->enrollee.rf_bands=%d",
                   pwps_info->enrollee.rf_bands);
    } else if (strstr(p_line, "OS_VER")) {
        p = strchr(p_line, '=');
        if (!p) {
            return WPS_STATUS_FAIL;
        }

        char os_version[20];
        memset(&os_version, 0, 20);
        os_version[0] = '0';
        os_version[1] = 'x';
        /* Move past the '0x' */
        p += 3;
        strncpy(&os_version[2], p, 8);
        pwps_info->enrollee.os_version = (u32) (strtoul(os_version, NULL, 16));
        pwps_info->registrar.os_version = (u32) (strtoul(os_version, NULL, 16));
        mwu_printf(DEBUG_INIT, "pwps_info->enrollee.os_version=0x%08x",
                   pwps_info->enrollee.os_version);
        mwu_printf(DEBUG_INIT, "pwps_info->registrar.os_version=0x%08x",
                   pwps_info->registrar.os_version);
    } else if (strstr(p_line, "AUTHORIZED_ENROLLEE")) {
        p = strchr(p_line, '=');
        if (!p) {
            return WPS_STATUS_FAIL;
        }

        char auth_enrollee1[20], index_char;
        u8 index = 0;
        memset(&auth_enrollee1, 0, 20);
        auth_enrollee1[0] = '0';
        auth_enrollee1[1] = 'x';
        p--;
        index_char = *p;
        index = hexval(index_char);
        /* Move past the '0x' */
        p += 4;
        strncpy(&auth_enrollee1[2], p, 12);
        mwu_printf(DEBUG_INIT, "Authorized Enrollee Mac index %d", index);
        index--;
        a2_mac_addr(p, pwps_info->auth_enrollee_mac_addr + ETH_ALEN * index);
        mwu_printf(DEBUG_INIT, "Authorized Enrollee Mac Address "
                   UTIL_MACSTR "",
                   UTIL_MAC2STR(pwps_info->auth_enrollee_mac_addr +
                           ETH_ALEN * index));
    } else if (strstr(p_line, "MAX_RETRY")) {
        p = strchr(p_line, '=');
        if (!p) {
            return WPS_STATUS_FAIL;
        }

        pwps_info->wps_msg_max_retry = atoi(++p);
        mwu_printf(DEBUG_INIT, "pwps_info->wps_msg_max_retry = %d",
                   pwps_info->wps_msg_max_retry);
    } else if (strstr(p_line, "MSG_TIMEOUT")) {
        p = strchr(p_line, '=');
        if (!p) {
            return WPS_STATUS_FAIL;
        }

        pwps_info->wps_msg_timeout = (u32) (strtoul(++p, NULL, 10));
        mwu_printf(DEBUG_INIT, "pwps_info->wps_msg_timeout = %d",
                   pwps_info->wps_msg_timeout);
    } else if (strstr(p_line, "PIN_LENGTH")) {
        p = strchr(p_line, '=');
        if (!p) {
            return WPS_STATUS_FAIL;
        }

        pwps_info->PINLen = (u32) (strtoul(++p, NULL, 10));
        mwu_printf(DEBUG_INIT, "pwps_info->PINLen = %d", pwps_info->PINLen);

        if ((pwps_info->PINLen != 8) && (pwps_info->PINLen != 4)) {
            mwu_printf(DEBUG_INIT, "Invalid PINLen");
            return WPS_STATUS_FAIL;
        }

    } else if (strstr(p_line, "SSID")) {
        p = strchr(p_line, '=');
        if (!p) {
            return WPS_STATUS_FAIL;
        }

        unsigned int len = strlen(++p);
        len = (len > 32) ? 32 : len;

        strncpy((char *) wps_s->current_ssid.ssid, p, len);
        wps_s->current_ssid.ssid_len = len;

        mwu_printf(DEBUG_INIT, "SSID '%s'", wps_s->current_ssid.ssid);

        pwps_info->registrar.cred_data[0].ssid_length =
            wps_s->current_ssid.ssid_len;
        pwps_info->enrollee.cred_data[0].ssid_length =
            wps_s->current_ssid.ssid_len;
        memcpy(pwps_info->registrar.cred_data[0].ssid, wps_s->current_ssid.ssid,
               pwps_info->registrar.cred_data[0].ssid_length);
        memcpy(pwps_info->enrollee.cred_data[0].ssid, wps_s->current_ssid.ssid,
               pwps_info->enrollee.cred_data[0].ssid_length);
    } else if (strstr(p_line, "AUTHENTICATION_TYPE")) {
        p = strchr(p_line, '=');
        if (!p) {
            return WPS_STATUS_FAIL;
        }

        char auth[10];
        memset(&auth, 0, 10);
        auth[0] = '0';
        auth[1] = 'x';
        /* Move past the '0x' */
        p += 3;
        strncpy(&auth[2], p, 4);
        pwps_info->enrollee.cred_data[0].auth_type =
            pwps_info->registrar.cred_data[0].auth_type =
            (u16) (strtoul(auth, NULL, 16));
        mwu_printf(DEBUG_INIT,
                   "pwps_info->registrar.cred_data[0].auth_type = 0x%04x",
                   pwps_info->registrar.cred_data[0].auth_type);

    } else if (strstr(p_line, "ENCRYPTION_TYPE")) {
        p = strchr(p_line, '=');
        if (!p) {
            return WPS_STATUS_FAIL;
        }

        char encry[10];
        memset(&encry, 0, 10);
        encry[0] = '0';
        encry[1] = 'x';
        /* Move past the '0x' */
        p += 3;
        strncpy(&encry[2], p, 4);
        pwps_info->enrollee.cred_data[0].encry_type =
            pwps_info->registrar.cred_data[0].encry_type =
            (u16) (strtoul(encry, NULL, 16));
        mwu_printf(DEBUG_INIT,
                   "pwps_info->registrar.cred_data[0].encry_type = 0x%04x",
                   pwps_info->registrar.cred_data[0].encry_type);
    } else if (strstr(p_line, "NETWORK_KEY_INDEX")) {
        u8 key_index = 1;
        p = strchr(p_line, '=');
        if (!p) {
            return WPS_STATUS_FAIL;
        }

        key_index = atoi(++p);
        if (key_index <= 0 || key_index >= 5) {

            mwu_printf(DEBUG_INIT,
                       "Invalid key index  %d received! Setting to default 1",
                       key_index);
            key_index = 1;
        }

        pwps_info->enrollee.cred_data[0].network_key_index =
            pwps_info->registrar.cred_data[0].network_key_index = key_index;

        mwu_printf(DEBUG_INIT,
                   "pwps_info->registrar.cred_data[0].network_key_index=%d",
                   pwps_info->registrar.cred_data[0].network_key_index);
    } else if (strstr(p_line, "PRIMARY_DEV_CATEGORY")) {
        u16 primary_dev_category = PRIMARY_DEV_CATEGORY_COMPUTER;
        p = strchr(p_line, '=');
        if (!p) {
            return WPS_STATUS_FAIL;
        }

        primary_dev_category = atoi(++p);
        pwps_info->primary_dev_category = primary_dev_category;

        mwu_printf(DEBUG_INIT, "pwps_info->primary_dev_category=%d",
                   pwps_info->primary_dev_category);
    } else if (strstr(p_line, "PRIMARY_DEV_SUBCATEGORY")) {
        u16 primary_dev_subcategory = PRIMARY_DEV_SUB_CATEGORY_PC;
        p = strchr(p_line, '=');
        if (!p) {
            return WPS_STATUS_FAIL;
        }

        primary_dev_subcategory = atoi(++p);
        pwps_info->primary_dev_subcategory = primary_dev_subcategory;

        mwu_printf(DEBUG_INIT, "pwps_info->primary_dev_subcategory=%d",
                   pwps_info->primary_dev_subcategory);
    } else if (strstr(p_line, "NETWORK_KEY")) {
        p = strchr(p_line, '=');
        if (!p) {
            return WPS_STATUS_FAIL;
        }

        unsigned int len = strlen(++p);
        len = (len > 64) ? 64 : len;

        strncpy((char *) pwps_info->registrar.cred_data[0].network_key, p, len);
        strncpy((char *) pwps_info->enrollee.cred_data[0].network_key, p, len);
        pwps_info->enrollee.cred_data[0].network_key_len =
            pwps_info->registrar.cred_data[0].network_key_len = len;


        mwu_printf(DEBUG_INIT, "NETWORK_KEY '%s'",
                   pwps_info->registrar.cred_data[0].network_key);
    }

    return WPS_STATUS_SUCCESS;
}

/**
 *  @brief Process initialization configuration file
 *
 *  @param pwps_info    A pointer to WPS_INFO structure
 *  @param wps_s        A pointer to global WPS structure
 *  @param init_cfg_file wps initialization config file to read
 *  @return             WPS_STATUS_SUCCESS--success, otherwise--fail
 */
static int
wps_config_read(PWPS_INFO pwps_info, WPS_DATA * wps_s, char *init_cfg_file)
{
    FILE *fp;
    char line[100];
    int ret = WPS_STATUS_SUCCESS;

    ENTER();

    fp = fopen(init_cfg_file, "r");
    if (!fp) {
        ret = WPS_STATUS_FAIL;
    } else {
        while (!feof(fp)) {
            wps_config_skipspace(fp);
            if (feof(fp))
                break;
            fscanf(fp, "%[^\n]", line);
            if (wps_config_parser(line, pwps_info, wps_s) != WPS_STATUS_SUCCESS) {
                ret = WPS_STATUS_FAIL;
                break;
            }
        }

        fclose(fp);
    }

    LEAVE();
    return ret;
}

/**
 *  @brief Set the method of Device Password ID (PIN or PBC)
 *
 *  @param pwps_info    A pointer to WPS_INFO structure
 *  @return             None
 */
static void
wps_set_device_password_id(PWPS_INFO pwps_info)
{
    int index;
    char inp[30];
    int i;
    MESSAGE_ENROLLEE_REGISTRAR *en_reg;
    WPS_DATA *wps_s = (WPS_DATA *) & pwps_info->wps_data;

    ENTER();

    /* Set Default PIN to all '0' */
    for (i = 0; i < 8; i++)
        pwps_info->PIN[i] = 0x30;
    i = 1;
    pwps_info->wifidir_pin_generated = WPS_CANCEL;
    if (pwps_info->role == WPS_ENROLLEE) {
        i++;
    }

    do {

        if (!fgets(inp, sizeof(inp) - 1, stdin))
            continue;
        fflush(stdin);

        if (!isdigit(inp[0])) {
            printf("Invalid input.\n");
            printf("Please input proper Device password ID index.\n");
            printf("Enter Selection : ");
            fflush(stdout);
            continue;
        }

        index = atoi((char *) &inp[0]);
        if (index >= 0 && index <= i) {
            mwu_printf(DEBUG_INIT, "Device Password ID select : [%d]\n", index);
            break;
        } else {
            printf("Invalid Device Password ID index selected.\n");
            printf("Please input proper index.\n");
            printf("Enter Selection : ");
            fflush(stdout);
            continue;
        }
    } while (1);

    if (pwps_info->role == WPS_ENROLLEE) {
        if (index == 1 || index == 2) {
            pwps_info->enrollee.device_password_id =
                DEVICE_PASSWORD_PUSH_BUTTON;
            pwps_info->enrollee.updated_device_password_id =
                DEVICE_PASSWORD_PUSH_BUTTON;
            pwps_info->input_state = WPS_INPUT_STATE_NO_INPUT;
            pwps_info->pin_pbc_set = WPS_SET;
            printf("Device Password ID select : [%d]\n", index);

            if (index == 2)
                pwps_info->pbc_auto = WPS_SET;
        } else if (index == 0) {
            if (wps_s->bss_type == BSS_TYPE_STA) {
                pwps_info->enrollee.device_password_id = DEVICE_PASSWORD_ID_PIN;
                pwps_info->enrollee.updated_device_password_id =
                    DEVICE_PASSWORD_ID_PIN;
                printf("Device Password ID select : [%d]\n", index);
                if (pwps_info->pin_generator == WPS_ENROLLEE) {
                    pwps_info->input_state = WPS_INPUT_STATE_NO_INPUT;
                    pwps_info->pin_pbc_set = WPS_SET;
                } else {
                    pwps_info->input_state = WPS_INPUT_STATE_READ_PIN;
                }
            } else {

                printf("Device Password ID select : [%d]\n", index);
                pwps_info->enrollee.device_password_id = DEVICE_PASSWORD_ID_PIN;
                pwps_info->enrollee.updated_device_password_id =
                    DEVICE_PASSWORD_ID_PIN;
                pwps_info->input_state = WPS_INPUT_STATE_NO_INPUT;
                pwps_info->pin_pbc_set = WPS_SET;

                /* Generate PIN Number */
                wps_generate_PIN(pwps_info);
            }
            fflush(stdout);
        }
    } else if (pwps_info->role == WPS_REGISTRAR) {
        if (index == 1) {
            pwps_info->registrar.device_password_id =
                DEVICE_PASSWORD_PUSH_BUTTON;
            pwps_info->registrar.updated_device_password_id =
                DEVICE_PASSWORD_PUSH_BUTTON;
            pwps_info->pin_pbc_set = WPS_SET;
            pwps_info->input_state = WPS_INPUT_STATE_NO_INPUT;
        } else if (index == 0) {
            pwps_info->registrar.device_password_id = DEVICE_PASSWORD_ID_PIN;
            pwps_info->registrar.updated_device_password_id =
                DEVICE_PASSWORD_ID_PIN;
            pwps_info->input_state = WPS_INPUT_STATE_READ_PIN;
        }
    } else if (pwps_info->role == WIFIDIR_ROLE) {
        if (index == 1) {
            pwps_info->enrollee.device_password_id =
                DEVICE_PASSWORD_PUSH_BUTTON;
            pwps_info->registrar.device_password_id =
                DEVICE_PASSWORD_PUSH_BUTTON;
            pwps_info->enrollee.updated_device_password_id =
                DEVICE_PASSWORD_PUSH_BUTTON;
            pwps_info->registrar.updated_device_password_id =
                DEVICE_PASSWORD_PUSH_BUTTON;
        } else if (index == 0) {
            pwps_info->enrollee.device_password_id = DEVICE_PASSWORD_ID_PIN;
            pwps_info->registrar.device_password_id = DEVICE_PASSWORD_ID_PIN;
            pwps_info->enrollee.updated_device_password_id =
                DEVICE_PASSWORD_ID_PIN;
            pwps_info->registrar.updated_device_password_id =
                DEVICE_PASSWORD_ID_PIN;
            mwu_printf(DEBUG_INIT, "Pin Generator %d. Discovery Role %d",
                       pwps_info->pin_generator, pwps_info->discovery_role);
            switch (pwps_info->discovery_role) {

            case WPS_ENROLLEE:
                if (pwps_info->pin_generator == WIFIDIR_AUTO_DISPLAY_PIN) {
                    /* Generate PIN Number */
                    wps_generate_PIN(pwps_info);
                    pwps_info->input_state = WPS_INPUT_STATE_NO_INPUT;
                    pwps_info->wifidir_pin_generated = WPS_SET;
                } else {
                    pwps_info->input_state = WPS_INPUT_STATE_READ_PIN;
                    pwps_info->enrollee.updated_device_password_id =
                        DEVICE_PASSWORD_REG_SPECIFIED;
                    pwps_info->registrar.updated_device_password_id =
                        DEVICE_PASSWORD_REG_SPECIFIED;
                }
                break;
            case WPS_REGISTRAR:
                if (pwps_info->pin_generator == WIFIDIR_AUTO_DISPLAY_PIN) {
                    pwps_info->input_state = WPS_INPUT_STATE_NO_INPUT;
                    pwps_info->enrollee.updated_device_password_id =
                        DEVICE_PASSWORD_REG_SPECIFIED;
                    pwps_info->registrar.updated_device_password_id =
                        DEVICE_PASSWORD_REG_SPECIFIED;
                    /* Generate PIN Number */
                    wps_generate_PIN(pwps_info);
                    pwps_info->wifidir_pin_generated = WPS_SET;
                } else if (pwps_info->pin_generator == WIFIDIR_AUTO_ENTER_PIN) {
                    pwps_info->input_state = WPS_INPUT_STATE_READ_PIN;
                    /* Input PIN Number */
                    wps_input_PIN(pwps_info);
                }
                break;
            case WPS_CANCEL:
                if (pwps_info->pin_generator == WIFIDIR_AUTO_DISPLAY_PIN) {
                    pwps_info->input_state = WPS_INPUT_STATE_NO_INPUT;
                    /* Generate PIN Number */
                    wps_generate_PIN(pwps_info);
                    pwps_info->wifidir_pin_generated = WPS_SET;
                } else if (pwps_info->pin_generator == WIFIDIR_AUTO_ENTER_PIN) {
                    pwps_info->input_state = WPS_INPUT_STATE_READ_PIN;
                    /* Input PIN Number */
                    wps_input_PIN(pwps_info);
                } else {
                    printf("Invalid PIN genrator value!!!\n");
                }
                break;
            }
        }

        pwps_info->pin_pbc_set = WPS_SET;
    }
    en_reg = ((pwps_info->role == WPS_ENROLLEE) ? &pwps_info->enrollee :
              &pwps_info->registrar);

    if (en_reg->device_password_id == DEVICE_PASSWORD_PUSH_BUTTON) {
        /* Override User's pin length to 8 for PBC */
        pwps_info->PINLen = 8;
    }

    LEAVE();
}

/**
 *  @brief Generate UUID using local MAC address
 *
 *  @param mac_addr     A pointer to local MAC address
 *  @param wps_s        A pointer to UUID
 *  @return             None
 */
static void
wps_generate_uuid_using_mac_addr(const u8 * mac_addr, u8 * uuid)
{
    const u8 *addr[2];
    size_t len[2];
    u8 hash[SHA1_MAC_LEN];
    u8 nsid[16] = {
        0x52, 0x64, 0x80, 0xf8,
        0xc9, 0x9b,
        0x4b, 0xe5,
        0xa6, 0x55,
        0x58, 0xed, 0x5f, 0x5d, 0x60, 0x84
    };

    ENTER();
    addr[0] = nsid;
    len[0] = sizeof(nsid);
    addr[1] = mac_addr;
    len[1] = 6;
    sha1_vector(2, addr, len, hash);
    memcpy(uuid, hash, 16);

    /* Version: 5 = named-based version using SHA-1 */
    uuid[6] = (5 << 4) | (uuid[6] & 0x0f);

    /* Variant specified in RFC 4122 */
    uuid[8] = 0x80 | (uuid[8] & 0x3f);

    LEAVE();
}

/**
 *  @brief Process WPS parameter initialization
 *
 *  @param pwps_info    A pointer to WPS_INFO structure
 *  @param wps_s        A pointer to global WPS structure
 *  @param init_cfg_file     Filename of configuration file to read
 *  @return             WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
static int
wps_parameter_initialization(PWPS_INFO pwps_info, WPS_DATA * wps_s,
                             char *init_cfg_file)
{
    int ret = WPS_STATUS_SUCCESS;
    char input[8];
    u8 zero_uuid[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    int flags;
    bss_config_t bss_config;
    ENTER();

    /* Initial variable used in state machine */
    pwps_info->register_completed = WPS_CANCEL;
    pwps_info->registration_fail = WPS_CANCEL;
    pwps_info->id = 1;
    pwps_info->eap_msg_sent = 0;
    pwps_info->last_recv_wps_msg = -1;
    pwps_info->restart_link_lost = WPS_CANCEL;
    pwps_info->read_ap_config_only = WPS_CANCEL;
    pwps_info->pin_pbc_set = WPS_CANCEL;
    pwps_info->input_state = WPS_INPUT_STATE_METHOD;
    pwps_info->wps_device_state = SC_NOT_CONFIGURED_STATE;
    pwps_info->config_load_by_oob = WPS_CANCEL;
    pwps_info->wps_ap_setup_locked = WPS_CANCEL;
    pwps_info->enrollee_in_authorized_mac = WPS_SET;

    /* Reset ssid here, to read from file or driver */
    memset(&wps_s->current_ssid, 0, sizeof(WPS_SSID));

    memset(&pwps_info->enrollee.wps_uuid, 0, WPS_UUID_MAX_LEN);
    memset(&pwps_info->registrar.wps_uuid, 0, WPS_UUID_MAX_LEN);
    pwps_info->registrar.wps_uuid_length = WPS_UUID_MAX_LEN;
    pwps_info->enrollee.wps_uuid_length = WPS_UUID_MAX_LEN;
    /* Read from config file */
    wps_config_read(pwps_info, wps_s, init_cfg_file);

    pwps_info->registrar.auth_type_flag = AUTHENTICATION_TYPE_ALL;
    pwps_info->registrar.encry_type_flag = ENCRYPTION_TYPE_ALL;
    pwps_info->enrollee.auth_type_flag = AUTHENTICATION_TYPE_ALL;
    pwps_info->enrollee.encry_type_flag = ENCRYPTION_TYPE_ALL;

    mwu_printf(DEBUG_INIT, "Device state SC_NOT_CONFIGURED_STATE !\n");
    if (!(memcmp(pwps_info->enrollee.wps_uuid, zero_uuid, WPS_UUID_MAX_LEN)) ||
        (!memcmp(pwps_info->registrar.wps_uuid, zero_uuid, WPS_UUID_MAX_LEN))) {
        mwu_printf(DEBUG_INIT,
                   "UUID not specified in config file. Generating UUID using local MAC address!");
        wps_generate_uuid_using_mac_addr(wps_s->my_mac_addr,
                                         pwps_info->registrar.wps_uuid);
        memcpy(&pwps_info->enrollee.wps_uuid, &pwps_info->registrar.wps_uuid,
               WPS_UUID_MAX_LEN);
        mwu_hexdump(DEBUG_INIT, "pwps_info->registrar.wps_uuid",
                    pwps_info->registrar.wps_uuid, WPS_UUID_MAX_LEN);
    }

    if (pwps_info->registrar.config_methods & CONFIG_METHOD_KEYPAD) {
        mwu_printf(DEBUG_INIT, "Device is Rich UI device.");
        pwps_info->is_low_ui_device = WPS_CANCEL;
    } else {
        mwu_printf(DEBUG_INIT, "Device is Low UI device.");
        pwps_info->is_low_ui_device = WPS_SET;
    }

    if (wps_s->bss_type == BSS_TYPE_UAP) {
        /* Get current uAP BSS configuration - for both Enrollee/Registrar
           modes */
        if (!wps_s->current_ssid.ssid_len) {
            ret = wps_load_uap_cred(wps_s, pwps_info);
            if (ret != WPS_STATUS_SUCCESS) {
                printf("Incorrect Network configuration. Quitting!\n");
                return ret;
            }
            if (pwps_info->role == WIFIDIR_ROLE) {
                /* For WIFIDIR, convert passpharase into PSK, and store it */
                /* PSK = ssid * passphrase using SHA1 */
                wlan_wifidir_generate_psk(pwps_info);
            }
        }
    } else if (pwps_info->role == WPS_REGISTRAR) {
        /* Wireless STA Registrar */
        ret = wps_load_wsta_registrar_cred(wps_s, pwps_info);
        if (ret != WPS_STATUS_SUCCESS) {
            printf("Incorrect Network configuration. Quitting!\n");
            return ret;
        }
    }

    if (pwps_info->role == WPS_ENROLLEE) {
        printf("Role : WPS_ENROLLEE\n");
        pwps_info->state = WPS_STATE_A;

        wps_print_registration_method_menu(pwps_info);

        if (wps_s->bss_type == BSS_TYPE_STA) {
            /* Ask user to select PIN/PBC */
            wps_set_device_password_id(gpwps_info);
        }
        /* Enrollee MAC Address */
        memcpy(pwps_info->enrollee.mac_address, wps_s->my_mac_addr, ETH_ALEN);

        /* Association State */
        pwps_info->enrollee.association_state = 0x01;

        /* Random Number */
        wps_generate_nonce_16B(pwps_info->enrollee.nonce);      /* Nonce */
        wps_generate_nonce_16B(pwps_info->enrollee.e_s1);       /* E-S1 */
        wps_generate_nonce_16B(pwps_info->enrollee.e_s2);       /* E-S1 */
        wps_generate_nonce_16B(pwps_info->enrollee.IV); /* IV */
    } else if (pwps_info->role == WPS_REGISTRAR) {
        printf("Role : WPS_REGISTRAR\n");

        /* check security mode */
        if ((AUTHENTICATION_TYPE_OPEN ==
             pwps_info->registrar.cred_data[0].auth_type) &&
            (ENCRYPTION_TYPE_NONE ==
             pwps_info->registrar.cred_data[0].encry_type)) {
            /* confirm with the user */
            printf
                ("The AP will be configured to the open mode. Continue? (y/n):");
            fflush(stdout);
            do {
                if (!fgets(input, sizeof(input) - 1, stdin))
                    continue;
                fflush(stdin);
                if (!isalpha(input[0]))
                    continue;

                if ((input[0] == 'y') || (input[0] == 'Y')) {
                    break;
                } else if ((input[0] == 'n') || (input[0] == 'N')) {
                    return WPS_STATUS_FAIL;
                } else {
                    printf("Invalid input. Please input (y/n):");
                }
            } while (1);
        }

        /*
         * Keep inteface UP to receive the EAPOL from enrollee.
         */
        if (wps_wlan_get_ifflags(&flags) != 0 ||
            wps_wlan_set_ifflags(flags | IFF_UP) != 0) {
            mwu_printf(MSG_ERROR, "Could not set interface '%s' UP\n",
                       wps_s->ifname);
            return WPS_STATUS_FAIL;
        }

        wps_print_registration_method_menu(pwps_info);

        /* Registrar MAC Address */
        memcpy(pwps_info->registrar.mac_address, wps_s->my_mac_addr, ETH_ALEN);

        /* Association State */
        pwps_info->registrar.association_state = 0x01;

        /* Random Number */
        wps_generate_nonce_16B(pwps_info->registrar.nonce);     /* Nonce */
        wps_generate_nonce_16B(pwps_info->registrar.r_s1);      /* R-S1 */
        wps_generate_nonce_16B(pwps_info->registrar.r_s2);      /* R-S2 */
        wps_generate_nonce_16B(pwps_info->registrar.IV);        /* IV */
    } else if (pwps_info->role == WIFIDIR_ROLE) {
        printf("Role : Wifidir.\n");

        wps_print_registration_method_menu(pwps_info);

        if (!wps_s->current_ssid.ssid_len && wps_s->bss_type == BSS_TYPE_UAP) {
            if (auto_go)
                apcmd_get_bss_config(&bss_config);

            load_cred_info(wps_s, pwps_info, &bss_config);
        }
        /*
         * Keep inteface UP to receive the EAPOL from enrollee.
         */
        if (wps_wlan_get_ifflags(&flags) != 0 ||
            wps_wlan_set_ifflags(flags | IFF_UP) != 0) {
            mwu_printf(MSG_ERROR, "Could not set interface '%s' UP\n",
                       wps_s->ifname);
            return WPS_STATUS_FAIL;
        }
        if (auto_go)
            pwps_info->discovery_role = WPS_REGISTRAR;

        /* WIFIDIR data initiliazation */
        memset(&wps_s->wifidir_data, 0, sizeof(WIFIDIR_DATA));
        wps_s->wifidir_data.dev_index = -1;
        wps_s->wifidir_data.dev_found = -1;
        wps_s->wifidir_data.current_peer = NULL;

        memcpy(pwps_info->enrollee.mac_address, wps_s->my_mac_addr, ETH_ALEN);
        memcpy(pwps_info->registrar.mac_address, wps_s->my_mac_addr, ETH_ALEN);

        /* Association State */
        pwps_info->enrollee.association_state = 0x01;
        pwps_info->registrar.association_state = 0x01;

        /* Random Number */
        wps_generate_nonce_16B(pwps_info->enrollee.nonce);      /* Nonce */
        wps_generate_nonce_16B(pwps_info->enrollee.e_s1);       /* E-S1 */
        wps_generate_nonce_16B(pwps_info->enrollee.e_s2);       /* E-S1 */
        wps_generate_nonce_16B(pwps_info->enrollee.IV); /* IV */
        wps_generate_nonce_16B(pwps_info->registrar.nonce);     /* Nonce */
        wps_generate_nonce_16B(pwps_info->registrar.r_s1);      /* R-S1 */
        wps_generate_nonce_16B(pwps_info->registrar.r_s2);      /* R-S2 */
        wps_generate_nonce_16B(pwps_info->registrar.IV);        /* IV */
    }

    LEAVE();
    return ret;
}

/********************************************************
        Global Functions
********************************************************/
extern PWPS_INFO wps_get_private_info(void);


/**
 *  @brief Process clear instance flag
 *
 *  @return             WPS_STATUS_SUCCESS--success, otherwise--fail
 */
int
wps_clear_running_instance(void)
{
    FILE *fp = NULL;
    int ret = WPS_STATUS_SUCCESS;
    unsigned int len =0;
    char run_file[50];
    PWPS_INFO pwps_info = wps_get_private_info();
    PWPS_DATA wps_s = (WPS_DATA *) &pwps_info->wps_data;

    len = strlen(FILE_WPSRUN_CONFIG_NAME);
    if (len >= (sizeof(run_file) - (IFNAMSIZ + 1))) {
        mwu_printf(MSG_ERROR, "ERROR - The file name is too long %s !\n",
                   run_file);
        return WPS_STATUS_FAIL;
    }

    strncpy(run_file, FILE_WPSRUN_CONFIG_NAME, len+1);
    strncat(run_file, wps_s->ifname, IFNAMSIZ);

    fp = fopen(run_file, "w+");
    if (!fp) {
        mwu_printf(MSG_ERROR, "ERROR - Fail to open file %s !\n", run_file);
        ret = WPS_STATUS_FAIL;
    } else {
        fseek(fp, 0, SEEK_SET);
        fprintf(fp, "%d\n", 0);
        fclose(fp);
    }

    return ret;
}

/**
 *  @brief WPS Initialization
 *
 *  @param wps_s        A pointer to global WPS structure
 *  @param init_cfg_file     Configuration file to read
 *  @return             WPS_STATUS_SUCCESS--success, otherwise--fail
 */
int wps_init(WPS_DATA * wps_s, char *init_cfg_file)
{
    int ret = WPS_STATUS_SUCCESS;

    ENTER();

    wps_s->userAbort = WPS_CANCEL;

    ret = wps_parameter_initialization(gpwps_info, wps_s, init_cfg_file);

    LEAVE();
    return ret;
}

/**
 *  @brief WPS uAP initialization
 *
 *  @param pwps_info    A pointer to WPS_INFO structure
 *  @param wps_s        A pointer to WPS_DATA structure
 *  @return             WPS_STATUS_SUCCESS--success, otherwise--fail
 */
static int
wps_init_uap_common(PWPS_INFO pwps_info, WPS_DATA * wps_s)
{
    int ret = WPS_STATUS_SUCCESS;

    ENTER();
    /* For UAP mode */
    wps_s->current_ssid.mode = IEEE80211_MODE_INFRA;
    pwps_info->mode = IEEE80211_MODE_INFRA;

    if (ie_index == -1) {
        mwu_printf(DEBUG_INIT, "GET_WPS_IE\n");
        /* Get the free IE buffer index */
        ret = wps_wlan_ie_config(GET_WPS_IE, &ie_index);
        if (ret != WPS_STATUS_SUCCESS) {
            mwu_printf(MSG_ERROR, "WPS Registrar failed to start\n");
            goto _exit_;
        }
    }

    /*
     * Send APCMD_SYS_CONFIGURE command to set WPS IE
     */
    mwu_printf(DEBUG_INIT, "SET_WPS_IE\n");
    ret = wps_wlan_ie_config(SET_WPS_AP_SESSION_INACTIVE_IE, &ie_index);
    if (ret != WPS_STATUS_SUCCESS) {
        mwu_printf(MSG_ERROR, "WPS Registrar failed to start\n");
        goto _exit_;
    }
    if (pwps_info->registrar.version >= WPS_VERSION_2DOT0) {

        if (ap_assocresp_ie_index == -1) {
            /* Get the free IE buffer index */
            ret = wps_wlan_ie_config(GET_WPS_IE, &ap_assocresp_ie_index);
            if (ret != WPS_STATUS_SUCCESS) {
                mwu_printf(MSG_ERROR,"WPS Registrar failed to get assoc resp IE index.\n");
                goto _exit_;
            }
        }

        ret =
            wps_wlan_ie_config(SET_WPS_AP_SESSION_INACTIVE_AR_IE,
                               &ap_assocresp_ie_index);
        if (ret != WPS_STATUS_SUCCESS) {
            mwu_printf(MSG_WARNING, "WPS IE configuration failure.\n");
        }
    }
    pwps_info->input_state = WPS_INPUT_STATE_METHOD;

    LEAVE();
  _exit_:
    return ret;
}

/**
 *  @brief WPS STA initialization
 *
 *  @param wps_s        A pointer to global WPS structure
 *
 *  @return             WPS_STATUS_SUCCESS--success, otherwise--fail
 */
static int
wps_init_sta_common(WPS_DATA * wps_s)
{
    int ret = WPS_STATUS_SUCCESS;
    ENTER();
    LEAVE();
    return ret;
}

/**
 *  @brief Associate with given network.
 *
 *  @param wps_s        A pointer to  WPS_DATA structure
 *  @return             WPS_STATUS_SUCCESS--success, otherwise--fail
 */
/* Scan for the available AP's */
static int
wps_associate(WPS_DATA * wps_s)
{
    int fconnected = 0;
    int retry_cnt = 10, index;
    char inp[4];
    u8 bssid_get[ETH_ALEN];
    u8 ssid_get[32];
    int i;
    int dupl_bssid = 0;
    PWPS_INFO pwps_info;
    pwps_info = wps_get_private_info();
    int flags;
    struct timeval tval;
    MESSAGE_ENROLLEE_REGISTRAR *en_reg;

    en_reg = ((gpwps_info->role == WPS_ENROLLEE) ? &gpwps_info->enrollee :
              &gpwps_info->registrar);

    if ((gpwps_info->role == WPS_ENROLLEE) &&
        (pwps_info->enrollee.device_password_id == DEVICE_PASSWORD_ID_PIN)) {
        /* Generate PIN Number */
        wps_generate_PIN(pwps_info);
        pwps_info->pin_pbc_set = WPS_SET;
    } else if ((gpwps_info->role == WPS_REGISTRAR) &&
               (pwps_info->registrar.device_password_id ==
                DEVICE_PASSWORD_ID_PIN)) {
        /* Input PIN Number */
        wps_input_PIN(pwps_info);
    }

    if (en_reg->device_password_id == DEVICE_PASSWORD_PUSH_BUTTON &&
        gpwps_info->pbc_auto == WPS_SET) {
        /* Connect to the only available AP with PBC session active */
        index = 0;
    } else {
        do {
            printf("\nSelect Index of SSID : ");
            memset(inp, 0, sizeof(inp));
            fgets(inp, sizeof(inp) - 1, stdin);
            fflush(stdin);

            if (!isdigit(inp[0]))
                continue;

            index = atoi((char *) &inp[0]);
            if (index < wps_s->num_scan_results) {
                mwu_printf(DEBUG_INIT, "Index Selected : %d\n", index);
                break;
            }
        } while (1);
    }

    /* mode: 0 = infra/BSS, 1 = adhoc/IBSS */
    if (wps_s->scan_results[index].caps & IEEE80211_CAP_ESS) {
        wps_wlan_set_mode(IEEE80211_MODE_INFRA);
        wps_s->current_ssid.mode = IEEE80211_MODE_INFRA;
        pwps_info->mode = IEEE80211_MODE_INFRA;
    }

    if ((gpwps_info->role == WPS_REGISTRAR) &&
        (wps_s->bss_type == BSS_TYPE_STA)) {
        u8 ap_conf_state = SC_NOT_CONFIGURED_STATE;
        if (wps_s->scan_results[index].wps_ie_len) {
            wps_probe_response_conf_state_parser(wps_s->scan_results[index].
                                                 wps_ie,
                                                 wps_s->scan_results[index].
                                                 wps_ie_len, &ap_conf_state);

            if (ap_conf_state == SC_CONFIGURED_STATE) {
                printf("AP is already configured. Continue (y/n)?:");
                do {
                    fgets(inp, sizeof(inp) - 1, stdin);
                    fflush(stdin);
                    if (strlen(inp) != 0) {
                        if (!isalpha(inp[0]))
                            continue;
                        if (inp[0] == 'n' || inp[0] == 'N') {
                            printf
                                ("\n Registrar will keep AP configuration.\n");
                            pwps_info->read_ap_config_only = WPS_SET;
                            break;
                        } else if (inp[0] == 'y' || inp[0] == 'Y') {
                            printf("\n Registrar will configure AP.\n");
                            pwps_info->read_ap_config_only = WPS_CANCEL;
                            break;
                        } else {
                            printf("Invalid input.Please input (y/n):");
                        }
                    }
                } while (1);
            }
        }
    }

    /* this is a workarond for Atheros 1.0 AP with dual radio and the same
       BSSID */

    /* check a duplicated bssid */
    dupl_bssid = 0;
    for (i = 0; i < wps_s->num_scan_results; i++) {
        if ((i != index) &&
            (memcmp
             (wps_s->scan_results[i].bssid, wps_s->scan_results[index].bssid,
              ETH_ALEN) == 0)) {
            dupl_bssid = 1;
            break;
        }
    }

    fconnected = 0;
    do {
        printf("\nConnecting ..... \n");

        printf("SSID = [%s], BSSID = [%02x:%02x:%02x:%02x:%02x:%02x]\n\n",
               wps_s->scan_results[index].ssid,
               wps_s->scan_results[index].bssid[0],
               wps_s->scan_results[index].bssid[1],
               wps_s->scan_results[index].bssid[2],
               wps_s->scan_results[index].bssid[3],
               wps_s->scan_results[index].bssid[4],
               wps_s->scan_results[index].bssid[5]);

        wps_wlan_set_authentication(AUTHENTICATION_TYPE_OPEN);

        if (!dupl_bssid) {
            /* assoicate AP using bssid */
            wps_wlan_set_wap((u8 *) wps_s->scan_results[index].bssid);
        } else {
            /* assoicate AP using ssid */
            wps_wlan_set_ssid(wps_s->scan_results[index].ssid,
                              (size_t) wps_s->scan_results[i].ssid_len,
                              WPS_SET);
        }

        /* wait for interface up */
        tval.tv_sec = 0;
        tval.tv_usec = 200000;
        select(0, NULL, NULL, NULL, &tval);

        if (!dupl_bssid) {
            memset(bssid_get, 0x00, ETH_ALEN);
            wps_wlan_get_wap(bssid_get);

            if ((memcmp(bssid_get, wps_s->scan_results[index].bssid,
                        ETH_ALEN) == 0)) {
                fconnected = 1;
                break;
            }
        } else {
            memset(ssid_get, 0x00, 32);
            wps_wlan_get_ssid(ssid_get);
            if ((memcmp(ssid_get, wps_s->scan_results[index].ssid,
                        wps_s->scan_results[index].ssid_len) == 0)) {
                fconnected = 1;
                break;
            }
        }

        if (fconnected == 0) {
            mwu_printf(MSG_ERROR, "Connect to AP FAIL ! Retrying ..... ");
            --retry_cnt;
            mwu_printf((DEBUG_WLAN | DEBUG_INIT), "Retry Count = %d", retry_cnt);
        }
    } while (retry_cnt > 0);

    if (!fconnected) {
        /* Abort Connection */
        mwu_printf(MSG_ERROR, "EXIT: fail in connecting to AP.\n");
        return WPS_STATUS_FAIL;
    }

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
    wps_s->current_ssid.ssid_len = wps_s->scan_results[index].ssid_len;
    memcpy(wps_s->current_ssid.ssid, wps_s->scan_results[index].ssid,
           wps_s->current_ssid.ssid_len);

    memcpy(wps_s->current_ssid.bssid, wps_s->scan_results[index].bssid,
           ETH_ALEN);

    /* Store Peer MAC Address */
    if (gpwps_info->role == WPS_ENROLLEE) {
        memcpy(pwps_info->registrar.mac_address,
               wps_s->scan_results[index].bssid, ETH_ALEN);
    } else {
        memcpy(pwps_info->enrollee.mac_address,
               wps_s->scan_results[index].bssid, ETH_ALEN);
    }

    /* Save RF Band information for M1 message */
    pwps_info->enrollee.rf_bands =
        (u8) wps_wlan_freq_to_band(wps_s->scan_results[index].freq);

    if (wps_wlan_get_ifflags(&flags) != 0 ||
        wps_wlan_set_ifflags(flags | IFF_UP) != 0) {
        mwu_printf(MSG_WARNING, "Could not set interface '%s' UP\n",
                   wps_s->ifname);
    }

    return WPS_STATUS_SUCCESS;
}

/**
 *  @brief WPS Start STA functionality.
 *
 *  @param wps_s        A pointer to  WPS_DATA structure
 *  @return             WPS_STATUS_SUCCESS--success, otherwise--fail
 */
/* Scan for the available AP's */
static int
wps_start_sta(WPS_DATA * wps_s)
{
    int ret = WPS_STATUS_SUCCESS;

    ENTER();
    /* XXX: Update/Set STA WPS IE here */

    /* Enable driver WPS session checking */
    wps_wlan_session_control(WPS_SESSION_ON);

    if(probe_ie_index == -1) {
        mwu_printf(DEBUG_INIT, "GET_PROBE_REQ_WPS_IE\n");
        ret = wps_wlan_probe_request_ie_config(GET_WPS_IE, &probe_ie_index);
        if (ret != WPS_STATUS_SUCCESS) {
            mwu_printf(MSG_ERROR, "Failed to set STA Probe Req IEs\n");
            return ret;
        }
    }

    /* configure Probe request IE */
    wps_wlan_probe_request_ie_config(SET_WPS_STA_SESSION_ACTIVE_IE,
                                     &probe_ie_index);
    /* Scan for the available APs, return the index of user selected AP */
    if ((ret = wps_scan_networks(wps_s)) == WPS_STATUS_FAIL) {
        /* Clear Probe request IE */
        wps_wlan_probe_request_ie_config(CLEAR_WPS_IE, &probe_ie_index);
        goto done;
    }
    /* Clear Probe request IE */
    wps_wlan_probe_request_ie_config(CLEAR_WPS_IE, &probe_ie_index);
    /* Associate to the AP selected */
    ret = wps_associate(wps_s);

  done:
    /* Disable driver WPS session checking */
    wps_wlan_session_control(WPS_SESSION_OFF);
    LEAVE();
    return ret;
}

/**
 *  @brief Process WPS start up operations
 *
 *  @param wps_s        A pointer to global WPS structure
 *  @return             WPS_STATUS_SUCCESS--success, otherwise--fail
 */
int
wps_start(WPS_DATA * wps_s)
{
    int ret = WPS_STATUS_SUCCESS;

    ENTER();

    if (wps_s->bss_type == BSS_TYPE_STA) {
        if ((ret = wps_init_sta_common(wps_s)) == WPS_STATUS_FAIL) {
            printf("\nWPS: STA initialization failure.\n");
            goto done;
        }
    }

    if ((wps_s->bss_type == BSS_TYPE_STA && gpwps_info->role == WPS_REGISTRAR)
        || (wps_s->bss_type == BSS_TYPE_UAP &&
            gpwps_info->role == WPS_ENROLLEE)) {
        if (wps_s->bss_type == BSS_TYPE_STA) {

            if ((ret = wps_start_sta(wps_s)) == WPS_STATUS_FAIL) {
                printf("\nWPS: STA startup failure.\n");
                goto done;
            }

        } else {
            if ((ret =
                 wps_init_uap_common(gpwps_info, wps_s)) == WPS_STATUS_FAIL) {
                printf("\nWPS: UAP initialization failure.\n");
                goto done;
            }
        }

        if (gpwps_info->role == WPS_ENROLLEE) {

            wps_enrollee_start(gpwps_info, wps_s);

        } else {

            wps_registrar_start(gpwps_info, wps_s);
        }
    } else {
        if (gpwps_info->role == WPS_ENROLLEE) {

            if (gpwps_info->enrollee.device_password_id ==
                DEVICE_PASSWORD_PUSH_BUTTON &&
                gpwps_info->pbc_auto == WPS_SET) {
                /* Enrollee Process : PBC Auto */
                ret = wps_enrollee_start_PBCAuto(gpwps_info, wps_s);
            } else {
                /* Enrollee Process : PIN mode and PBC user selection */
                ret = wps_enrollee_start(gpwps_info, wps_s);
            }

        } else if (gpwps_info->role == WPS_REGISTRAR) {
            /* Registrar Process */
            ret = wps_registrar_start(gpwps_info, wps_s);
        } else if (gpwps_info->role == WIFIDIR_ROLE) {

            /* In the case of WIFIDIR, the PIN will be set asynchronously by a command
             * client or it has already been supplied at launch time.
             */

            if (auto_go) {
                /* set up intended inteface address and start BSS */
                if (memcmp
                    (gpwps_info->wifidir_intended_addr, "\x00\x00\x00\x00\x00\x00",
                     ETH_ALEN)) {
                    if (wlan_set_intended_mac_addr
                        (gpwps_info->wifidir_intended_addr)) {
                        mwu_printf(DEBUG_WIFIDIR_DISCOVERY,
                                   "Failed to set mac address.\n");
                    }
                    memcpy(gpwps_info->registrar.mac_address,
                           gpwps_info->wifidir_intended_addr, ETH_ALEN);
                }

                if (apcmd_start_bss() == WPS_STATUS_FAIL) {
                    printf
                        ("GO AP is already running, Interface address not set.\n");
                }
            }

            /* Discovery Process */
            /*
             * Discovery is triggred by waiting for peer detected
             * events from FW and connecting to one of the
             * peers.
             */
            printf("\nWIFIDIR: Waiting for discovery events...\n");
        }
    }
  done:
    LEAVE();
    return ret;
}

/**
 *  @brief Print WPS Registration method menu
 *
 *  @param wps_s        A pointer to global WPS structure
 *  @return             None
 */
void
wps_print_registration_method_menu(WPS_INFO * pwps_info)
{
    int i = 2;
    WPS_DATA *wps_s = (WPS_DATA *) & pwps_info->wps_data;

    ENTER();
    printf("\n");
    printf("Select Device Password ID Option\n");
    printf("[0]. PIN\n");
    printf("[1]. PBC\n");
    if (pwps_info->role == WPS_ENROLLEE && wps_s->bss_type != BSS_TYPE_UAP) {
        printf("[2]. PBC Auto\n");
        i++;
    }
    wps_method_menu_shown = 1;
    if (pwps_info->role == WPS_REGISTRAR && wps_s->bss_type == BSS_TYPE_UAP) {
        printf("You may directly input PIN value.\n");
    }
    printf("Enter Selection : ");
    fflush(stdout);

    LEAVE();
}

/**
 *  @brief Process WPS stop operations
 *
 *  @param wps_s        A pointer to global WPS structure
 *  @return             WPS_STATUS_SUCCESS--success, otherwise--fail
 */
int
wps_stop(WPS_DATA * wps_s)
{
    int ret = WPS_STATUS_SUCCESS;
    u8 bssid_get[ETH_ALEN];

    ENTER();

    /* Disconnect if still connect to AP */
    memset(bssid_get, 0x00, ETH_ALEN);
    wps_wlan_get_wap(bssid_get);
    if ((memcmp(bssid_get, wps_s->current_ssid.bssid, ETH_ALEN) == 0)) {
        if (wps_s->current_ssid.mode == IEEE80211_MODE_INFRA)
            ret = wps_wlan_set_deauth();
        else
            ret = WPS_STATUS_FAIL;
    }

    if (wps_s->bss_type == BSS_TYPE_STA && assocreq_ie_index != -1) {
        if (gpwps_info->enrollee.version >= WPS_VERSION_2DOT0) {
            /* Clear (Re)Assoc request IE */
            mwu_printf(MSG_ERROR, "CLEAR_WPS_IE \n");
            ret =
                wps_wlan_assoc_request_ie_config(CLEAR_WPS_IE,
                                                 &assocreq_ie_index);
            if (ret != WPS_STATUS_SUCCESS) {
                mwu_printf(MSG_ERROR, "Failed to clear Assoc Request IE's\n");
            }
        }
    }
    LEAVE();
    return ret;
}

/**
 *  @brief Process WPS free operations
 *
 *  @return             None
 */
void
wps_deinit(void)
{
    ENTER();

    if (gpwps_info == NULL) {
        LEAVE();
        return;
    }

#ifdef OPENSSL
    mrvl_dh_free(gpwps_info->dh);
#endif

    free(gpwps_info);
    gpwps_info = NULL;

    LEAVE();
}
