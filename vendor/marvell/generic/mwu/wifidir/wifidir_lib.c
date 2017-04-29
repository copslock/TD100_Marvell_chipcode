/** @file wifidir_lib.c
 *  @brief This file contains WIFIDIR action frame, WLAN commands
 *
 * Copyright (c) 2003-2008, Jouni Malinen <j@w1.fi>
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
#include <errno.h>
#include <ctype.h>

#include "wifidir_lib.h"
#include "wps_msg.h"
#include "mwu_defs.h"
#include "mwu_log.h"
#include "util.h"
#include "wps_def.h"
#include "wps_wlan.h"
#include "wps_os.h"
#include "wlan_hostcmd.h"
#include "wlan_wifidir.h"
#include "wifidir.h"
#include "../mwu/mwu.h"
#include "mwu_ioctl.h"

#define DEFAULT_DEVICE_NAME "MRVL_DEFAULT_NAME"

/********************************************************
        Local Variables
********************************************************/

/********************************************************
        Global Variables
********************************************************/
/** Global pwps information */
extern PWPS_INFO gpwps_info;

extern struct module *wifidir_mod;

/* Wifi direct IE buffer index */
extern short wifidir_ie_index_base;

/********************************************************
  Local Functions
 ********************************************************/
/**
 *  @brief Converts colon separated MAC address to hex value
 *
 *  @param mac      A pointer to the colon separated MAC string
 *  @param raw      A pointer to the hex data buffer
 *  @return         SUCCESS or FAILURE
 *                  WIFIDIR_RET_MAC_BROADCAST  - if broadcast mac
 *                  WIFIDIR_RET_MAC_MULTICAST - if multicast mac
 */
static int
mac2raw(char *mac, u8 * raw)
{
    unsigned int temp_raw[ETH_ALEN];
    int num_tokens = 0;
    int i;
    if (strlen(mac) != ((2 * ETH_ALEN) + (ETH_ALEN - 1))) {
        return FAILURE;
    }
    num_tokens = sscanf(mac, "%2x:%2x:%2x:%2x:%2x:%2x",
                        temp_raw + 0, temp_raw + 1, temp_raw + 2, temp_raw + 3,
                        temp_raw + 4, temp_raw + 5);
    if (num_tokens != ETH_ALEN) {
        return FAILURE;
    }
    for (i = 0; i < num_tokens; i++)
        raw[i] = (u8) temp_raw[i];

    if (memcmp(raw, "\xff\xff\xff\xff\xff\xff", ETH_ALEN) == 0) {
        return WIFIDIR_RET_MAC_BROADCAST;
    } else if (raw[0] & 0x01) {
        return WIFIDIR_RET_MAC_MULTICAST;
    }
    return SUCCESS;
}

/**
 *  @brief Prints a MAC address in colon separated form from raw data
 *
 *  @param raw      A pointer to the hex data buffer
 *  @return         N/A
 */
static void
print_mac(u8 * raw)
{
    printf("%02x:%02x:%02x:%02x:%02x:%02x", (unsigned int) raw[0],
           (unsigned int) raw[1], (unsigned int) raw[2], (unsigned int) raw[3],
           (unsigned int) raw[4], (unsigned int) raw[5]);
    return;
}

/**
 *    @brief isdigit for String.
 *
 *    @param x            Char string
 *    @return             FAILURE for non-digit.
 *                        SUCCESS for digit
 */
static inline int
ISDIGIT(char *x)
{
    unsigned int i;
    for (i = 0; i < strlen(x); i++)
        if (isdigit(x[i]) == 0)
            return FAILURE;
    return SUCCESS;
}

/**
 *  @brief              Check hex string
 *
 *  @param hex          A pointer to hex string
 *  @return             SUCCESS or FAILURE
 */
static int
ishexstring(void *hex)
{
    int i, a;
    char *p = hex;
    int len = strlen(p);
    if (!strncasecmp("0x", p, 2)) {
        p += 2;
        len -= 2;
    }
    for (i = 0; i < len; i++) {
        a = hex2num(*p);
        if (a < 0)
            return FAILURE;
        p++;
    }
    return SUCCESS;
}

/**
 *  @brief  Detects duplicates channel in array of strings
 *
 *  @param  argc    Number of elements
 *  @param  argv    Array of strings
 *  @return FAILURE or SUCCESS
 */
static inline int
has_dup_channel(int argc, char *argv[])
{
    int i, j;
    /* Check for duplicate */
    for (i = 0; i < (argc - 1); i++) {
        for (j = i + 1; j < argc; j++) {
            if (atoi(argv[i]) == atoi(argv[j])) {
                return FAILURE;
            }
        }
    }
    return SUCCESS;
}

/**
 *  @brief Checkes a particular input for validatation.
 *
 *  @param cmd      Type of input
 *  @param argc     Number of arguments
 *  @param argv     Pointer to the arguments
 *  @return         SUCCESS or FAILURE
 */
static int
is_input_valid(valid_inputs cmd, int argc, char *argv[])
{
    int i;
    int ret = SUCCESS;
    char country[6] = { ' ', ' ', 0, 0, 0, 0 };
    char wifidir_dev_name[35];
    unsigned int len = 0;
    if (argc == 0)
        return FAILURE;
    switch (cmd) {
    case WIFIDIR_MINDISCOVERYINT:
        if (argc != 1) {
            printf
                ("ERR:Incorrect number of arguments for MinDiscoveryInterval\n");
            ret = FAILURE;
        } else {
            if ((ISDIGIT(argv[0]) == 0) || (atoi(argv[0]) >= (1 << 16))) {
                printf("ERR:MinDiscoveryInterval must be 2 bytes\n");
                ret = FAILURE;
            }
        }
        break;
    case WIFIDIR_MAXDISCOVERYINT:
        if (argc != 1) {
            printf
                ("ERR:Incorrect number of arguments for MaxDiscoveryInterval\n");
            ret = FAILURE;
        } else {
            if ((ISDIGIT(argv[0]) == 0) || (atoi(argv[0]) >= (1 << 16))) {
                printf("ERR:MaxDiscoveryInterval must be 2 bytes\n");
                ret = FAILURE;
            }
        }
        break;
    case WIFIDIR_DEVICECAPABILITY:
        if (argc != 1) {
            printf("ERR:Incorrect number of arguments for DeviceCapability\n");
            ret = FAILURE;
        } else {
            if ((ISDIGIT(argv[0]) == 0) || (atoi(argv[0]) > MAX_DEV_CAPABILITY)
                || (atoi(argv[0]) < 0)) {
                printf("ERR:DeviceCapabilty must be in the range [0:%d]\n",
                       MAX_DEV_CAPABILITY);
                ret = FAILURE;
            }
        }
        break;
    case WIFIDIR_GROUPCAPABILITY:
        if (argc != 1) {
            printf("ERR:Incorrect number of arguments for GroupCapability\n");
            ret = FAILURE;
        } else {
            if ((ISDIGIT(argv[0]) == 0) || (atoi(argv[0]) > MAX_GRP_CAPABILITY)
                || (atoi(argv[0]) < 0)) {
                printf("ERR:GroupCapabilty must be in the range [0:%d]\n",
                       MAX_GRP_CAPABILITY);
                ret = FAILURE;
            }
        }
        break;
    case CHANNEL:
        if ((argc != 1) && (argc != 2)) {
            printf("ERR: Incorrect arguments for channel.\n");
            ret = FAILURE;
        } else {
            if (argc == 2) {
                if ((ISDIGIT(argv[1]) == 0) || (atoi(argv[1]) < 0) ||
                    (atoi(argv[1]) > 1)) {
                    printf("ERR: MODE must be either 0 or 1\n");
                    ret = FAILURE;
                }
                if ((atoi(argv[1]) == 1) && (atoi(argv[0]) != 0)) {
                    printf("ERR: Channel must be 0 for ACS; MODE = 1.\n");
                    ret = FAILURE;
                }
            }
            if ((argc == 1) || (atoi(argv[1]) == 0)) {
                if ((ISDIGIT(argv[0]) == 0) || (atoi(argv[0]) < 1) ||
                    (atoi(argv[0]) > MAX_CHANNELS)) {
                    printf("ERR: Channel must be in the range of 1 to %d\n",
                           MAX_CHANNELS);
                    ret = FAILURE;
                }
            }
        }
        break;
    case SCANCHANNELS:
        if (argc > MAX_CHANNELS) {
            printf("ERR: Invalid List of Channels\n");
            ret = FAILURE;
        } else {
            for (i = 0; i < argc; i++) {
                if ((ISDIGIT(argv[i]) == 0) || (atoi(argv[i]) < 1) ||
                    (atoi(argv[i]) > MAX_CHANNELS)) {
                    printf("ERR: Channel must be in the range of 1 to %d\n",
                           MAX_CHANNELS);
                    ret = FAILURE;
                    break;
                }
            }
            if ((ret != FAILURE) && (has_dup_channel(argc, argv) != SUCCESS)) {
                printf("ERR: Duplicate channel values entered\n");
                ret = FAILURE;
            }
        }
        break;
    case WIFIDIR_INTENT:
        if (argc != 1) {
            printf("ERR:Incorrect number of arguments for intent\n");
            ret = FAILURE;
        } else {
            if ((ISDIGIT(argv[0]) == 0) || (atoi(argv[0]) > MAX_INTENT) ||
                (atoi(argv[0]) < 0)) {
                printf("ERR:Intent must be in the range [0:%d]\n", MAX_INTENT);
                ret = FAILURE;
            }
        }
        break;
    case WIFIDIR_MANAGEABILITY:
        if (argc != 1) {
            printf("ERR:Incorrect number of arguments for manageability\n");
            ret = FAILURE;
        } else {
            if ((ISDIGIT(argv[0]) == 0) || (atoi(argv[0]) < 0) ||
                (atoi(argv[0]) > 1)) {
                printf("ERR:Manageability must be either 0 or 1\n");
                ret = FAILURE;
            }
        }
        break;
    case WIFIDIR_INVITATIONFLAG:
        if (argc != 1) {
            printf("ERR:Incorrect number of arguments for Invitation Flag\n");
            ret = FAILURE;
        } else {
            if ((ISDIGIT(argv[0]) == 0) || (atoi(argv[0]) < 0) ||
                (atoi(argv[0]) > 1)) {
                printf("ERR:Invitation Flag must be either 0 or 1\n");
                ret = FAILURE;
            }
        }
        break;
    case WIFIDIR_GROUP_WIFIDIR_DEVICE_NAME:
        /* 2 extra characters for quotes around device name */
        len = strlen(argv[0]);
        if (len > 34) {
            printf
                ("ERR:WIFIDIR Device name string length must not be more than 32\n");
            ret = FAILURE;
        } else {
            strncpy(wifidir_dev_name, argv[0], len + 1);
            wifidir_dev_name[len] = '\0';
            if ((wifidir_dev_name[0] != '"') ||
                (wifidir_dev_name[strlen(wifidir_dev_name) - 1] != '"')) {
                printf("ERR:WIFIDIR Device name must be within double quotes!\n");
                ret = FAILURE;
            }
        }
        break;
    case WIFIDIR_COUNTRY:
        len = strlen(argv[0]);
        /* 2 extra characters for quotes around country */
        if ((len > 5) || (len < 4)) {
            printf("ERR:Country string must have length 2 or 3\n");
            ret = FAILURE;
        } else {
            strncpy(country, argv[0], len + 1);
            country[len] = '\0';
            if ((country[0] != '"') || (country[strlen(country) - 1] != '"')) {
                printf("ERR:country code must be within double quotes!\n");
                ret = FAILURE;
            } else {
                for (i = 1; (unsigned int) i < strlen(country) - 2; i++) {
                    if ((toupper(country[i]) < 'A') ||
                        (toupper(country[i]) > 'Z')) {
                        printf("ERR:Invalid Country Code\n");
                        ret = FAILURE;
                    }
                }
            }
        }
        break;
    case WIFIDIR_NO_OF_CHANNELS:
        if (argc != 1) {
            printf("ERR:Incorrect number of arguments for num of channels\n");
            ret = FAILURE;
        } else {
            if ((ISDIGIT(argv[0]) == 0) || (atoi(argv[0]) > MAX_CHANNELS)) {
                printf("ERR:Number of channels should be less than %d\n",
                       MAX_CHANNELS);
                ret = FAILURE;
            }
        }
        break;
    case WIFIDIR_NOA_INDEX:
        if (argc != 1) {
            printf("ERR:Incorrect number of arguments for NoA Index\n");
            ret = FAILURE;
        } else {
            if ((ISDIGIT(argv[0]) == 0) || (atoi(argv[0]) < MIN_NOA_INDEX) ||
                (atoi(argv[0]) > MAX_NOA_INDEX)) {
                printf("ERR:NoA index should be in the range [%d:%d]\n",
                       MIN_NOA_INDEX, MAX_NOA_INDEX);
                ret = FAILURE;
            }
        }
        break;
    case WIFIDIR_OPP_PS:
        if (argc != 1) {
            printf("ERR:Incorrect number of arguments for Opp PS\n");
            ret = FAILURE;
        } else {
            if ((ISDIGIT(argv[0]) == 0) || ((atoi(argv[0]) != 0) &&
                                            (atoi(argv[0]) != 1))) {
                printf("ERR:Opp PS must be either 0 or 1\n");
                ret = FAILURE;
            }
        }
        break;
    case WIFIDIR_CTWINDOW:
        if (argc != 1) {
            printf("ERR:Incorrect number of arguments for CTWindow\n");
            ret = FAILURE;
        } else {
            if ((ISDIGIT(argv[0]) == 0) || (atoi(argv[0]) > MAX_CTWINDOW) ||
                (atoi(argv[0]) < MIN_CTWINDOW)) {
                printf("ERR:CT Window must be in the range [%d:%d]\n",
                       MIN_CTWINDOW, MAX_CTWINDOW);
                ret = FAILURE;
            }
        }
        break;
    case WIFIDIR_COUNT_TYPE:
        if (argc != 1) {
            printf("ERR:Incorrect number of arguments for Count/Type\n");
            ret = FAILURE;
        } else {
            if ((ISDIGIT(argv[0]) == 0) || (atoi(argv[0]) == 0) ||
                (atoi(argv[0]) > MAX_COUNT_TYPE) ||
                (atoi(argv[0]) < MIN_COUNT_TYPE)) {
                printf("ERR:Count/Type must be in the range [%d:%d] or 255\n",
                       MIN_COUNT_TYPE, MAX_COUNT_TYPE);
                ret = FAILURE;
            }
        }
        break;
    case WIFIDIR_DURATION:
        if (argc != 1) {
            printf("ERR:Incorrect number of arguments for Duration\n");
            ret = FAILURE;
        }
        break;
    case WIFIDIR_INTERVAL:
        if (argc != 1) {
            printf("ERR:Incorrect number of arguments for Interval\n");
            ret = FAILURE;
        }
        break;
    case WIFIDIR_START_TIME:
        if (argc != 1) {
            printf("ERR:Incorrect number of arguments for Start Time\n");
            ret = FAILURE;
        }
        break;
    case WIFIDIR_PRIDEVTYPEOUI:
        if (argc > MAX_PRIMARY_OUI_LEN) {
            printf
                ("ERR: Incorrect number of PrimaryDeviceTypeOUI arguments.\n");
            ret = FAILURE;
            break;
        }
        for (i = 0; i < argc; i++) {
            if (IS_HEX_OR_DIGIT(argv[i]) == FAILURE) {
                printf("ERR:Unsupported OUI\n");
                ret = FAILURE;
                break;
            }
        }
        if (!((A2HEXDECIMAL(argv[0]) == 0x00) && (A2HEXDECIMAL(argv[1]) == 0x50)
              && (A2HEXDECIMAL(argv[2]) == 0xF2) &&
              (A2HEXDECIMAL(argv[3]) == 0x04))) {
            printf("ERR:Unsupported OUI\n");
            ret = FAILURE;
            break;
        }
        break;
    case WIFIDIR_REGULATORYCLASS:
        if (argc != 1) {
            printf("ERR:Incorrect number of arguments for RegulatoryClass\n");
            ret = FAILURE;
        } else {
            if ((ISDIGIT(argv[0]) == 0) || (atoi(argv[0]) > MAX_REG_CLASS) ||
                (atoi(argv[0]) < MIN_REG_CLASS)) {
                printf
                    ("ERR:RegulatoryClass must be in the range [%d:%d] or 255\n",
                     MIN_REG_CLASS, MAX_REG_CLASS);
                ret = FAILURE;
            }
        }
        break;
    case WIFIDIR_PRIDEVTYPECATEGORY:
        if (argc != 1) {
            printf
                ("ERR:Incorrect number of arguments for PrimaryDeviceTypeCategory\n");
            ret = FAILURE;
        } else {
            if ((ISDIGIT(argv[0]) == 0) || (atoi(argv[0]) > MAX_PRIDEV_TYPE_CAT)
                || (atoi(argv[0]) < MIN_PRIDEV_TYPE_CAT)) {
                printf
                    ("ERR:PrimaryDeviceTypeCategory must be in the range [%d:%d]\n",
                     MIN_PRIDEV_TYPE_CAT, MAX_PRIDEV_TYPE_CAT);
                ret = FAILURE;
            }
        }
        break;
    case WIFIDIR_SECONDARYDEVCOUNT:
        if (argc != 1) {
            printf
                ("ERR:Incorrect number of arguments for SecondaryDeviceCount\n");
            ret = FAILURE;
        } else {
            if ((ISDIGIT(argv[0]) == 0) ||
                (atoi(argv[0]) > MAX_SECONDARY_DEVICE_COUNT)) {
                printf("ERR:SecondaryDeviceCount must be less than 15.\n");
                ret = FAILURE;
            }
        }
        break;
    case WIFIDIR_GROUP_SECONDARYDEVCOUNT:
        if (argc != 1) {
            printf
                ("ERR:Incorrect number of arguments for SecondaryDeviceCount\n");
            ret = FAILURE;
        } else {
            if ((ISDIGIT(argv[0]) == 0) ||
                (atoi(argv[0]) > MAX_GROUP_SECONDARY_DEVICE_COUNT)) {
                printf("ERR:SecondaryDeviceCount must be less than 2.\n");
                ret = FAILURE;
            }
        }
        break;
    case WIFIDIR_INTERFACECOUNT:
        if (argc != 1) {
            printf("ERR:Incorrect number of arguments for InterfaceCount\n");
            ret = FAILURE;
        } else {
            if ((ISDIGIT(argv[0]) == 0) ||
                (atoi(argv[0]) > MAX_INTERFACE_ADDR_COUNT)) {
                printf("ERR:IntefaceCount must be in range.[0-41]\n");
                ret = FAILURE;
            }
        }
        break;
    case WIFIDIR_ATTR_CONFIG_TIMEOUT:
        if (argc != 1) {
            printf
                ("ERR:Incorrect number of arguments for Timeout Configuration\n");
            ret = FAILURE;
        } else {
            if ((ISDIGIT(argv[0]) == 0) || (atoi(argv[0]) > 255)) {
                printf("ERR:TimeoutConfig must be in the range [0:255]\n");
                ret = FAILURE;
            }
        }
        break;
    case WIFIDIR_ATTR_EXTENDED_TIME:
        if (argc != 1) {
            printf("ERR:Incorrect number of arguments for Extended time.\n");
            ret = FAILURE;
        } else {
            if ((ISDIGIT(argv[0]) == 0) || (atoi(argv[0]) > 65535) ||
                (atoi(argv[0]) < 1)) {
                printf("ERR:Extended Time must be in the range [1:65535]\n");
                ret = FAILURE;
            }
        }
        break;

    case WIFIDIR_PRIDEVTYPESUBCATEGORY:
        if (argc != 1) {
            printf
                ("ERR:Incorrect number of arguments for PrimaryDeviceTypeSubCategory\n");
            ret = FAILURE;
        } else {
            if ((ISDIGIT(argv[0]) == 0) ||
                (atoi(argv[0]) > MAX_PRIDEV_TYPE_SUBCATEGORY) ||
                (atoi(argv[0]) < MIN_PRIDEV_TYPE_SUBCATEGORY)) {
                printf
                    ("ERR:PrimaryDeviceTypeSubCategory must be in the range [%d:%d]\n",
                     MIN_PRIDEV_TYPE_SUBCATEGORY, MAX_PRIDEV_TYPE_SUBCATEGORY);
                ret = FAILURE;
            }
        }
        break;
    case WIFIDIR_WPSCONFMETHODS:
        if (argc != 1) {
            printf("ERR:Incorrect number of arguments for WPSConfigMethods\n");
            ret = FAILURE;
        } else {
            if ((IS_HEX_OR_DIGIT(argv[0]) == 0) ||
                (A2HEXDECIMAL(argv[0]) > MAX_WPS_CONF_METHODS) ||
                (A2HEXDECIMAL(argv[0]) < MIN_WPS_CONF_METHODS)) {
                printf("ERR:WPSConfigMethods must be in the range [%d:%d]\n",
                       MIN_WPS_CONF_METHODS, MAX_WPS_CONF_METHODS);
                ret = FAILURE;
            }
        }
        break;
    case WIFIDIR_WPSVERSION:
        if (argc != 1) {
            printf("ERR:Incorrect number of arguments for WPSVersion\n");
            ret = FAILURE;
        } else {
            if ((A2HEXDECIMAL(argv[0]) < 0x10) &&
                (A2HEXDECIMAL(argv[0]) > 0x20)) {
                printf("ERR:Incorrect WPS Version %s\n", argv[0]);
                ret = FAILURE;
            }
        }
        break;
    case WIFIDIR_WPSSETUPSTATE:
        if (argc != 1) {
            printf("ERR:Incorrect number of arguments for WPSSetupState\n");
            ret = FAILURE;
        } else {
            if ((IS_HEX_OR_DIGIT(argv[0]) == 0) ||
                ((A2HEXDECIMAL(argv[0]) != 0x01)
                 && (A2HEXDECIMAL(argv[0]) != 0x02))) {
                printf("ERR:Incorrect WPSSetupState %s\n", argv[0]);
                ret = FAILURE;
            }
        }
        break;
    case WIFIDIR_WPSREQRESPTYPE:
        if (argc != 1) {
            printf("ERR:Incorrect number of arguments for WPSRequestType\n");
            ret = FAILURE;
        } else {
            if ((IS_HEX_OR_DIGIT(argv[0]) == 0) ||
                (A2HEXDECIMAL(argv[0]) > WPS_MAX_REQUESTTYPE)) {
                printf("ERR:Incorrect WPSRequestType %s\n", argv[0]);
                ret = FAILURE;
            }
        }
        break;
    case WIFIDIR_WPSSPECCONFMETHODS:
        if (argc != 1) {
            printf
                ("ERR:Incorrect number of arguments for WPSSpecConfigMethods\n");
            ret = FAILURE;
        } else {
            if ((IS_HEX_OR_DIGIT(argv[0]) == 0) ||
                ((u16) A2HEXDECIMAL(argv[0]) > MAX_WPS_CONF_METHODS) ||
                ((u16) A2HEXDECIMAL(argv[0]) < MIN_WPS_CONF_METHODS)) {
                printf
                    ("ERR:WPSSpecConfigMethods must be in the range [%d:%d]\n",
                     MIN_WPS_CONF_METHODS, MAX_WPS_CONF_METHODS);
                ret = FAILURE;
            }
        }
        break;
    case WIFIDIR_WPSDEVICENAME:
        if (argc != 1) {
            printf("ERR:Incorrect number of arguments\n");
            ret = FAILURE;
        } else {
            if (strlen(argv[0]) > MAX_DEVICE_NAME_LEN) {
                printf("ERR:Device name should contain"
                       " less than 32 charactors.\n");
                ret = FAILURE;
            }
        }
        break;
    case WIFIDIR_WPSMANUFACTURER:
        if (argc != 1) {
            printf("ERR:Incorrect number of arguments\n");
            ret = FAILURE;
        } else {
            if (strlen(argv[0]) > WPS_MANUFACT_MAX_LEN) {
                printf("ERR:Manufacturer name should contain"
                       "less than 64 charactors.\n");
                ret = FAILURE;
            }
        }
        break;
    case WIFIDIR_WPSMODELNAME:
        if (argc != 1) {
            printf("ERR:Incorrect number of arguments\n");
            ret = FAILURE;
        } else {
            if (strlen(argv[0]) > WPS_MODEL_MAX_LEN) {
                printf("ERR:Model name should contain"
                       " less than 64 charactors.\n");
                ret = FAILURE;
            }
        }
        break;
    case WIFIDIR_WPSUUID:
        if (argc > WPS_UUID_MAX_LEN) {
            printf("ERR: Incorrect number of WPSUUID arguments.\n");
            ret = FAILURE;
        } else {
            for (i = 0; i < argc; i++) {
                if (IS_HEX_OR_DIGIT(argv[i]) == FAILURE) {
                    printf("ERR:Unsupported UUID\n");
                    ret = FAILURE;
                    break;
                }
            }
        }
        break;
    case WIFIDIR_WPSPRIMARYDEVICETYPE:
        if (argc > WPS_DEVICE_TYPE_MAX_LEN) {
            printf
                ("ERR: Incorrect number of WPSPrimaryDeviceType arguments.\n");
            ret = FAILURE;
            break;
        }
        for (i = 0; i < argc; i++) {
            if (IS_HEX_OR_DIGIT(argv[i]) == FAILURE) {
                printf("ERR:Unsupported primary device type\n");
                ret = FAILURE;
                break;
            }
        }
        if (!((A2HEXDECIMAL(argv[2]) == 0x00) && (A2HEXDECIMAL(argv[3]) == 0x50)
              && (A2HEXDECIMAL(argv[4]) == 0xF2) &&
              (A2HEXDECIMAL(argv[5]) == 0x04))) {
            printf("ERR:Unsupported OUI\n");
            ret = FAILURE;
            break;
        }
        break;
    case WIFIDIR_WPSRFBAND:
        if (argc != 1) {
            printf("ERR:Incorrect number of arguments for WPSRFBand\n");
            ret = FAILURE;
        } else {
            if ((IS_HEX_OR_DIGIT(argv[0]) == 0) ||
                ((A2HEXDECIMAL(argv[0]) != 0x01)
                 && (A2HEXDECIMAL(argv[0]) != 0x02))) {
                printf("ERR:Incorrect WPSRFBand %s\n", argv[0]);
                ret = FAILURE;
            }
        }
        break;
    case WIFIDIR_WPSASSOCIATIONSTATE:
        if (argc != 1) {
            printf
                ("ERR:Incorrect number of arguments for WPSAssociationState\n");
            ret = FAILURE;
        } else {
            if ((IS_HEX_OR_DIGIT(argv[0]) == 0) ||
                ((u16) A2HEXDECIMAL(argv[0]) > WPS_MAX_ASSOCIATIONSTATE) ||
                ((u16) A2HEXDECIMAL(argv[0]) < WPS_MIN_ASSOCIATIONSTATE)) {
                printf("ERR:WPSAssociationState must be in the range [%d:%d]\n",
                       WPS_MIN_ASSOCIATIONSTATE, WPS_MAX_ASSOCIATIONSTATE);
                ret = FAILURE;
            }
        }
        break;
    case WIFIDIR_WPSCONFIGURATIONERROR:
        if (argc != 1) {
            printf
                ("ERR:Incorrect number of arguments for WPSConfigurationError\n");
            ret = FAILURE;
        } else {
            if ((IS_HEX_OR_DIGIT(argv[0]) == 0) ||
                ((u16) A2HEXDECIMAL(argv[0]) > WPS_MAX_CONFIGURATIONERROR) ||
                ((u16) A2HEXDECIMAL(argv[0]) < WPS_MIN_CONFIGURATIONERROR)) {
                printf
                    ("ERR:WPSConfigurationError must be in the range [%d:%d]\n",
                     WPS_MIN_CONFIGURATIONERROR, WPS_MAX_CONFIGURATIONERROR);
                ret = FAILURE;
            }
        }
        break;
    case WIFIDIR_WPSDEVICEPASSWORD:
        if (argc != 1) {
            printf("ERR:Incorrect number of arguments for WPSDevicePassword\n");
            ret = FAILURE;
        } else {
            if ((IS_HEX_OR_DIGIT(argv[0]) == 0) ||
                ((u16) A2HEXDECIMAL(argv[0]) > WPS_MAX_DEVICEPASSWORD) ||
                ((u16) A2HEXDECIMAL(argv[0]) < WPS_MIN_DEVICEPASSWORD)) {
                printf("ERR:WPSDevicePassword must be in the range [%d:%d]\n",
                       WPS_MIN_DEVICEPASSWORD, WPS_MAX_DEVICEPASSWORD);
                ret = FAILURE;
            }
        }
        break;
    case WIFIDIR_WPSMODELNUMBER:
        if (argc > WPS_MODEL_MAX_LEN) {
            printf("ERR: Incorrect number of WPSModelNumber arguments.\n");
            ret = FAILURE;
        } else {
            for (i = 0; i < argc; i++) {
                if (IS_HEX_OR_DIGIT(argv[i]) == FAILURE) {
                    printf("ERR:Unsupported WPSModelNumber\n");
                    ret = FAILURE;
                    break;
                }
            }
        }
        break;
    case WIFIDIR_WPSSERIALNUMBER:
        if (argc > WPS_SERIAL_MAX_LEN) {
            printf("ERR: Incorrect number of WPSSerialNumber arguments.\n");
            ret = FAILURE;
        } else {
            for (i = 0; i < argc; i++) {
                if (IS_HEX_OR_DIGIT(argv[i]) == FAILURE) {
                    printf("ERR:Unsupported WPSSerialNumber\n");
                    ret = FAILURE;
                    break;
                }
            }
        }
        break;
    case WIFIDIR_ENABLE_SCAN:
        if (argc != 1) {
            printf("ERR:Incorrect number of arguments for EnableScan\n");
            ret = FAILURE;
        } else {
            if ((ISDIGIT(argv[0]) == 0) || ((atoi(argv[0]) != 0) &&
                                            (atoi(argv[0]) != 1))) {
                printf("ERR:EnableScan must be 0 or 1.\n");
                ret = FAILURE;
            }
        }
        break;

    case WIFIDIR_DEVICE_STATE:
        if (argc != 1) {
            printf("ERR:Incorrect number of arguments for DeviceState\n");
            ret = FAILURE;
        } else {
            if (ISDIGIT(argv[0]) == 0) {
                printf("ERR:Incorrect DeviceState.\n");
                ret = FAILURE;
            }
        }
        break;

    default:
        printf("Parameter %d ignored\n", cmd);
        break;
    }
    return ret;
}

/**
 *  @brief Parses a command line
 *
 *  @param line     The line to parse
 *  @param args     Pointer to the argument buffer to be filled in
 *  @return         Number of arguments in the line or EOF
 */
static int
parse_line(char *line, char *args[])
{
    int arg_num = 0;
    int is_start = 0;
    int is_quote = 0;
    int length = 0;
    int i = 0;

    arg_num = 0;
    length = strlen(line);
    /* Process line */

    /* Find number of arguments */
    is_start = 0;
    is_quote = 0;
    for (i = 0; i < length; i++) {
        /* Ignore leading spaces */
        if (is_start == 0) {
            if (line[i] == ' ') {
                continue;
            } else if (line[i] == '\t') {
                continue;
            } else if (line[i] == '\n') {
                break;
            } else {
                is_start = 1;
                args[arg_num] = &line[i];
                arg_num++;
            }
        }
        if (is_start == 1) {
            /* Ignore comments */
            if (line[i] == '#') {
                if (is_quote == 0) {
                    line[i] = '\0';
                    arg_num--;
                }
                break;
            }
            /* Separate by '=' */
            if (line[i] == '=') {
                line[i] = '\0';
                is_start = 0;
                continue;
            }
            /* Separate by ',' */
            if (line[i] == ',') {
                line[i] = '\0';
                is_start = 0;
                continue;
            }
            /* Change ',' to ' ', but not inside quotes */
            if ((line[i] == ',') && (is_quote == 0)) {
                line[i] = ' ';
                continue;
            }
        }
        /* Remove newlines */
        if (line[i] == '\n') {
            line[i] = '\0';
        }
        /* Check for quotes */
        if (line[i] == '"') {
            is_quote = (is_quote == 1) ? 0 : 1;
            continue;
        }
        if (((line[i] == ' ') || (line[i] == '\t')) && (is_quote == 0)) {
            line[i] = '\0';
            is_start = 0;
            continue;
        }
    }
    return arg_num;
}

/**
 *  @brief      Parse function for a configuration line
 *
 *  @param s        Storage buffer for data
 *  @param size     Maximum size of data
 *  @param stream   File stream pointer
 *  @param line     Pointer to current line within the file
 *  @param _pos     Output string or NULL
 *  @return     String or NULL
 */
static char *
config_get_line(char *s, int size, FILE * stream, int *line, char **_pos)
{
    char *pos, *end, *sstart;
    while (fgets(s, size, stream)) {
        (*line)++;
        s[size - 1] = '\0';
        pos = s;
        /* Skip white space from the beginning of line. */
        while (*pos == ' ' || *pos == '\t' || *pos == '\r')
            pos++;
        /* Skip comment lines and empty lines */
        if (*pos == '#' || *pos == '\n' || *pos == '\0')
            continue;
        /*
         * Remove # comments unless they are within a double quoted
         * string.
         */
        sstart = strchr(pos, '"');
        if (sstart)
            sstart = strrchr(sstart + 1, '"');
        if (!sstart)
            sstart = pos;
        end = strchr(sstart, '#');
        if (end)
            *end-- = '\0';
        else
            end = pos + strlen(pos) - 1;
        /* Remove trailing white space. */
        while (end > pos &&
               (*end == '\n' || *end == ' ' || *end == '\t' || *end == '\r'))
            *end-- = '\0';
        if (*pos == '\0')
            continue;
        if (_pos)
            *_pos = pos;
        return pos;
    }

    if (_pos)
        *_pos = NULL;
    return NULL;
}

int wifidir_ioctl(char *ifname, u8 * cmd, u16 * size, u16 buf_size)
{
    wifidircmdbuf *header = NULL;

    if (buf_size < *size) {
        mwu_printf(MSG_WARNING,
                   "buf_size should not less than cmd buffer size\n");
        return WIFIDIR_ERR_INVAL;
    }

    *(u32 *) cmd = buf_size - BUF_HEADER_SIZE;

    header = (wifidircmdbuf *) cmd;
    header->cmd_head.size = *size - BUF_HEADER_SIZE;
    endian_convert_request_header(header->cmd_head);

    if (mwu_hostcmd(ifname, cmd) == MWU_ERR_SUCCESS) {
        endian_convert_response_header(header->cmd_head);
        header->cmd_head.cmd_code &= HostCmd_CMD_ID_MASK;
        header->cmd_head.cmd_code |= WIFIDIRCMD_RESP_CHECK;
        *size = header->cmd_head.size;

        /* Validate response size */
        if (*size > (buf_size - BUF_HEADER_SIZE)) {
            mwu_printf(MSG_WARNING,
            "ERR:Response size (%d) greater than buffer size (%d)! Aborting!\n",
            *size, buf_size);
            return WIFIDIR_ERR_COM;
        }
    } else {
        return WIFIDIR_ERR_COM;
    }

    return WIFIDIR_ERR_SUCCESS;
}

/**
 *  @brief Process and send ie config command
 *  @param ie_index  A pointer to the IE buffer index
 *  @param data_len_wifidir  Length of P2P data, 0 to get, else set.
 *  @param data_len_wps  Length of WPS data, 0 to get, else set.
 *  @param buf      Pointer to buffer to set.
 *  @return          WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
int
wifidir_ie_config(u16 * pie_index, u16 data_len_wifidir, u16 data_len_wps, u8 * buf)
{
    int i, ret = WPS_STATUS_SUCCESS;
    tlvbuf_custom_ie *tlv = NULL;
    custom_ie *ie_ptr = NULL;

    ENTER();
    tlv = (tlvbuf_custom_ie *) buf;
    tlv->tag = MRVL_MGMT_IE_LIST_TLV_ID;
    /* Locate headers */
    ie_ptr = (custom_ie *) (tlv->ie_data);

    /* Set TLV fields : WIFIDIR IE parameters */
    if (data_len_wifidir) {
        /* Set IE */
#define MASK_WIFIDIR_AUTO 0xFFFF
        ie_ptr->mgmt_subtype_mask = MASK_WIFIDIR_AUTO;
        tlv->length = sizeof(custom_ie) + data_len_wifidir;
        ie_ptr->ie_length = data_len_wifidir;
        ie_ptr->ie_index = *pie_index;
    } else {
        /* Get WPS IE */
        tlv->length = 0;
    }

    (*pie_index)++;

    /* Locate headers */
    ie_ptr =
        (custom_ie *) ((u8 *) (tlv->ie_data) + sizeof(custom_ie) +
                       data_len_wifidir);

    /* Set WPS IE parameters */
    if (data_len_wps) {
        /* Set IE */
        ie_ptr->mgmt_subtype_mask = MASK_WIFIDIR_AUTO;
        tlv->length += sizeof(custom_ie) + data_len_wps;
        ie_ptr->ie_length = data_len_wps;
        ie_ptr->ie_index = *pie_index;
    }

    ret = mwu_custom_ie_config(wifidir_mod->iface, buf);

    /*
     * TODO: eventually all functions should use the right return type.
     */
    if (ret == MWU_ERR_SUCCESS)
        ret = WPS_STATUS_SUCCESS;
    else
        ret = WPS_STATUS_FAIL;

    if (ret == WPS_STATUS_SUCCESS) {
        mwu_printf(DEBUG_WLAN, "custom IE ioctl success.\n");

        if (!data_len_wifidir) {
            /* Get the IE buffer index number for MGMT_IE_LIST_TLV */
            tlv = (tlvbuf_custom_ie *) buf;
            *pie_index = 0xFFFF;
            if (tlv->tag == MRVL_MGMT_IE_LIST_TLV_ID) {
                ie_ptr = (custom_ie *) (tlv->ie_data);
                for (i = 0; i < MAX_MGMT_IE_INDEX; i++) {
                    /* zero mask indicates a wps IE, return previous index */
                    if (ie_ptr->mgmt_subtype_mask == MASK_WIFIDIR_AUTO &&
                        ie_ptr->ie_length) {
                        *pie_index = ie_ptr->ie_index;
                        mwu_printf(DEBUG_WLAN, "Found Index %d\n", i);
                        break;
                    }
                    if (i < (MAX_MGMT_IE_INDEX - 1))
                        ie_ptr =
                            (custom_ie *) ((u8 *) ie_ptr + sizeof(custom_ie) +
                                           ie_ptr->ie_length);
                }
            }
            if (*pie_index == 0xFFFF) {
                mwu_printf(MSG_WARNING, "No free IE buffer available\n");
                ret = WPS_STATUS_FAIL;
            }
        }
    }

    LEAVE();
    return ret;
}

/**
 *  @brief  Check if intended address field is configured.
 *          If yes, update the corresponding device ID.
 *  @param  wifidir_buf      WIFIDIR buffer pointer
 *  @param  wifidir_buf_len  WIFIDIR buffer length
 *
 *  @return  WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
static int
wifidir_check_set_intended_addr(u8 * wifidir_buf, u16 wifidir_buf_len)
{
    tlvbuf_wifidir_intended_addr *wifidir_tlv;
    u8 *ptr = wifidir_buf;
    s16 left_len = wifidir_buf_len;
    u16 len = 0;

    ENTER();

    while (left_len > WIFIDIR_IE_HEADER_LEN) {

        if (*ptr == TLV_TYPE_WIFIDIR_INTENDED_ADDRESS) {
            wifidir_tlv = (tlvbuf_wifidir_intended_addr *) ptr;

            /* Address is found, set it now */
            mwu_printf(DEBUG_WIFIDIR_DISCOVERY, "Intended address. " UTIL_MACSTR,
                       UTIL_MAC2STR(wifidir_tlv->group_address));

            memcpy(gpwps_info->wifidir_intended_addr, wifidir_tlv->group_address,
                   ETH_ALEN);
            LEAVE();
            return WPS_STATUS_SUCCESS;
        }

        memcpy(&len, ptr + 1, sizeof(u16));
        len = wlan_le16_to_cpu(len);

        ptr += len + WIFIDIR_IE_HEADER_LEN;
        left_len -= len + WIFIDIR_IE_HEADER_LEN;
    }

    LEAVE();
    return WPS_STATUS_FAIL;
}

/**
 *  @brief  Fetch and store the self channel list.
 *  The channel list stored in global data structure
 *  gpwps_info->wps_data.wifidir_data
 *
 *  @param  None
 *
 *  @return  WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
int
wifidir_check_self_channel_list()
{
    tlvbuf_wifidir_channel_list *wifidir_tlv;
    u8 *ptr = NULL;
    s16 left_len;
    u16 len = 0, len_wifidirect = 0;
    chan_entry *temp_ptr;
    WIFIDIR_DATA *pwifidir_data = &(gpwps_info->wps_data.wifidir_data);
    int i = 0;
    short ie_index = 0;
    u8 buf[MRVDRV_SIZE_OF_CMD_BUFFER];
    tlvbuf_custom_ie *tlv = NULL;
    custom_ie *ie_ptr = NULL;
    int ret = WPS_STATUS_SUCCESS;

    ENTER();

    memset(buf, 0, MRVDRV_SIZE_OF_CMD_BUFFER);

    ret = wifidir_ie_config((u16 *) &ie_index, 0, 0, buf);

    if (ret == WPS_STATUS_SUCCESS && ie_index < (MAX_MGMT_IE_INDEX - 1)) {
        tlv = (tlvbuf_custom_ie *) buf;

        if (tlv->tag == MRVL_MGMT_IE_LIST_TLV_ID) {

            ie_ptr = (custom_ie *) (tlv->ie_data);

            /* Goto appropriate Ie Index */
            for (i = 0; i < ie_index; i++) {
                ie_ptr = (custom_ie *) ((u8 *) ie_ptr + sizeof(custom_ie) +
                        ie_ptr->ie_length);
            }

            left_len = ie_ptr->ie_length;
            ptr = (u8 *) (ie_ptr->ie_buffer);

            while (left_len > WIFIDIR_IE_HEADER_LEN) {
                if (*ptr == TLV_TYPE_WIFIDIR_CHANNEL_LIST) {

                    wifidir_tlv = (tlvbuf_wifidir_channel_list *) ptr;
                    temp_ptr = (chan_entry *) wifidir_tlv->wifidir_chan_entry_list;

                    len_wifidirect = wlan_le16_to_cpu(wifidir_tlv->length) -
                        (sizeof(tlvbuf_wifidir_channel_list) - WIFIDIR_IE_HEADER_LEN);

                    /* Multiple channel entries */
                    if (len_wifidirect >
                            (sizeof(chan_entry) + temp_ptr->num_of_channels)) {
                        while (len_wifidirect) {
                            if ((int) (temp_ptr->regulatory_class) == WIFI_REG_CLASS_81) {

                                pwifidir_data->self_channel_list.num_of_chan =
                                    temp_ptr->num_of_channels;

                                for (i = 0; i < temp_ptr->num_of_channels; i++) {
                                    pwifidir_data->self_channel_list.chan[i] =
                                        *(temp_ptr->chan_list + i);
                                }
                                break;
                            }

                            len_wifidirect -=
                                sizeof(chan_entry) + temp_ptr->num_of_channels;

                            temp_ptr += sizeof(chan_entry) + temp_ptr->num_of_channels;
                        }
                    } else {
                        /* Only one channel entry */
                        pwifidir_data->self_channel_list.num_of_chan =
                            temp_ptr->num_of_channels;

                        for (i = 0; i < temp_ptr->num_of_channels; i++) {
                            pwifidir_data->self_channel_list.chan[i] =
                                *(temp_ptr->chan_list + i);
                        }
                    }

                    LEAVE();
                    return WPS_STATUS_SUCCESS;
                }

                memcpy(&len, ptr + 1, sizeof(u16));
                len = wlan_le16_to_cpu(len);
                ptr += len + WIFIDIR_IE_HEADER_LEN;
            }
        }
    }

    LEAVE();
    return WPS_STATUS_FAIL;
}



/**
 *  @brief  Check if P2P capability attr is present in the P2P IE.
 *          If yes, return the value of it.
 *  @param  wifidir_buf      WIFIDIR buffer pointer
 *  @param  wifidir_buf_len  WIFIDIR buffer length
 *  @param  dev_cap pointer to device capability
 *  @param  grp_cap pointer to group capability
 *
 *  @return  WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
static int
wifidir_get_p2p_cap_attribute(u8 * wifidir_buf, u16 wifidir_buf_len,
                          u8 * dev_cap, u8 * grp_cap)
{
    tlvbuf_wifidir_capability *wifidir_tlv;
    u8 *ptr = wifidir_buf;
    s16 left_len = wifidir_buf_len;
    u16 len = 0;

    ENTER();

    mwu_hexdump(DEBUG_WLAN, "WIFIDIR IE:", (u8 *) wifidir_buf, wifidir_buf_len);
    while (left_len > WIFIDIR_IE_HEADER_LEN) {

        if (*ptr == TLV_TYPE_WIFIDIR_CAPABILITY) {
            wifidir_tlv = (tlvbuf_wifidir_capability *) ptr;
            *dev_cap = wifidir_tlv->dev_capability;
            *grp_cap = wifidir_tlv->group_capability;

            LEAVE();
            return WPS_STATUS_SUCCESS;
        }

        memcpy(&len, ptr + 1, sizeof(u16));
        len = wlan_le16_to_cpu(len);

        ptr += len + WIFIDIR_IE_HEADER_LEN;
        left_len -= len + WIFIDIR_IE_HEADER_LEN;
    }

    LEAVE();
    return WPS_STATUS_FAIL;
}

/**
 *  @brief  Check if device password is received from FW.
 *          If yes, update the corresponding device ID.
 *  @param  wps_buf      WPS buffer pointer
 *  @param  wps_buf_len  WPS buffer length
 *
 *  @return  WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
static int
wifidir_wps_check_device_password(u8 * wps_buf, u16 wps_buf_len)
{
    u8 *ptr = wps_buf;
    s16 left_len = wps_buf_len;
    u16 wps_len = 0, wps_type = 0, WPS_devicepassword = 0;

    ENTER();

    /* Look for WPS Device Password */
    while (left_len > (s16) sizeof(tlvbuf_wps_ie)) {
        memcpy(&wps_type, ptr, sizeof(u16));
        wps_type = mwu_ntohs(wps_type);
        memcpy(&wps_len, ptr + 2, sizeof(u16));
        wps_len = mwu_ntohs(wps_len);

        if (wps_type == SC_Device_Password_ID) {
            tlvbuf_wps_ie *wps_tlv = (tlvbuf_wps_ie *) ptr;
            WPS_devicepassword = mwu_htons(gpwps_info->enrollee.device_password_id);

            /* mwu_htons done, now do memcpy */
            memcpy(wps_tlv->data, &WPS_devicepassword, sizeof(u16));
            mwu_printf(DEBUG_WIFIDIR_DISCOVERY,
                       "Updated Device Password directly in FW response.\n");
            LEAVE();
            return WPS_STATUS_SUCCESS;
        }
        ptr += wps_len + sizeof(tlvbuf_wps_ie);
        left_len -= wps_len + sizeof(tlvbuf_wps_ie);
    }

    LEAVE();
    return WPS_STATUS_FAIL;
}

/**
 *  @brief  Update the password IE in the device configuration.
 *          First Get the config and update with password.
 *  @param  None
 *
 *  @return  WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
int
wps_wlan_update_password_ie_config(void)
{
    int i, ret = WPS_STATUS_SUCCESS;
    short ie_index = 0;
    u8 buf[MRVDRV_SIZE_OF_CMD_BUFFER];
    u16 WPS_devicepassword, tlv_len;
    u16 ie_len_wifidir, ie_len_wps;
    tlvbuf_wps_ie *wps_ptr;
    tlvbuf_custom_ie *tlv = NULL;
    custom_ie *ie_ptr = NULL;


    ENTER();
    memset(buf, 0, MRVDRV_SIZE_OF_CMD_BUFFER);
    ret = wifidir_ie_config((u16 *) & ie_index, 0, 0, buf);

    if (ret == WPS_STATUS_SUCCESS && (ie_index < (MAX_MGMT_IE_INDEX - 1))) {
        tlv = (tlvbuf_custom_ie *) buf;
        if (tlv->tag == MRVL_MGMT_IE_LIST_TLV_ID) {
            ie_ptr = (custom_ie *) (tlv->ie_data);
            /* Goto appropriate Ie Index */
            for (i = 0; i < ie_index; i++) {
                ie_ptr = (custom_ie *) ((u8 *) ie_ptr + sizeof(custom_ie) +
                                        ie_ptr->ie_length);
            }
            ie_len_wifidir = ie_ptr->ie_length;
            /*
             * Find out if the interface address is configured, it needs to be
             * set using SIOCSIFHWADDR
             */
            wifidir_check_set_intended_addr((u8 *) (ie_ptr->ie_buffer), ie_len_wifidir);

            wifidir_get_p2p_cap_attribute((u8 *) (ie_ptr->ie_buffer), ie_len_wifidir,
                                      &gpwps_info->self_p2p_dev_cap,
                                      &gpwps_info->self_p2p_grp_cap);

            ie_ptr =
                (custom_ie *) ((u8 *) ie_ptr + sizeof(custom_ie) + ie_len_wifidir);
            ie_len_wps = ie_ptr->ie_length;
            wps_ptr =
                (tlvbuf_wps_ie *) ((u8 *) (ie_ptr->ie_buffer) + ie_len_wps);

            /* Search for Device passwork in existing WPS attributes, if not
               found append password IE here, update ie_len_wps */
            ret =
                wifidir_wps_check_device_password((u8 *) (ie_ptr->ie_buffer),
                                              ie_len_wps);

            if (ret != WPS_STATUS_SUCCESS) {
                WPS_devicepassword =
                    mwu_htons(gpwps_info->enrollee.device_password_id);
                tlv_len = sizeof(tlvbuf_wps_ie) + sizeof(WPS_devicepassword);
                wps_ptr->tag = mwu_htons(SC_Device_Password_ID);
                wps_ptr->length = mwu_htons(sizeof(WPS_devicepassword));

                /* mwu_htons done, now do memcpy */
                memcpy(wps_ptr->data, &WPS_devicepassword, sizeof(u16));
                ie_len_wps += tlv_len;
            }

            /* Update New IE now */
            ret =
                wifidir_ie_config((u16 *) & ie_index, ie_len_wifidir, ie_len_wps, buf);
            if (ret != WPS_STATUS_SUCCESS) {
                mwu_printf(MSG_WARNING,
                           "Failed to update WIFIDIR device password.\n");
            }
        }
    }
    LEAVE();
    return ret;
}



/**
 *  @brief Check whether the mac address is all zero
 *  @param dev_addr    the mac address to be checked
 *  @return SUCCESS--all zero, FAILURE--not all zero
 */
static int
wifidir_is_mac_all_zero(u8 *dev_addr)
{
    int i;
    for (i = 0; i < ETH_ALEN; ++i) {
        if (dev_addr[i] != 0)
            return FAILURE;
    }
    return SUCCESS;
}

/**
 *  @brief Check whether the device name is by default
 *  @param name    to be checked
 *  @return SUCCESS--default name, FAILURE--not default name
 */
static int
wifidir_is_default_name(char *name)
{
    return strcmp(name, DEFAULT_DEVICE_NAME) ?
        FAILURE : SUCCESS;
}

/**
 *  @brief create default device name
 *  @param name        buffer for generated string
 *  @param dev_addr    device address
 *  @return SUCCESS--success, FAILURE--fail
 */
static int
wifidir_create_default_name(char *name, u8 *dev_addr)
{
    if (!name || !dev_addr)
        return FAILURE;
    sprintf(name, "mvdroid-%02X", dev_addr[ETH_ALEN-1]);
    return SUCCESS;
}

/**
 *  @brief Read the wifidir parameters and sends to the driver
 *
 *  @param file_name File to open for configuration parameters.
 *  @param cfg       Wifidirect config structure
 *  @param pbuf      Pointer to output buffer
 *  @param ie_len_wifidir Length of p2p parameters to return
 *  @param ie_len_wps Length of WPS parameters to return
 *  @return          SUCCESS or FAILURE
 *
 *  By default, the params_config are pulled from the specified file.
 *  If the file leaves any important details unspecified
 *  (e.g., device name), a sensible default will be used. If a wifidir_cfg
 *  is provided, its values will have the highest precedence, overriding
 *  any values in the config file.
 */
static void
wifidir_file_params_config(char *file_name, struct wifidir_cfg *cfg,
                       u8 * pbuf, u16 * ie_len_wifidir, u16 * ie_len_wps)
{
    FILE *config_file = NULL;
    char *line = NULL;
    int i, li = 0, arg_num = 0, ret = 0, wifidir_level = 0, no_of_chan_entries =
        0, no_of_noa = 0;
    int secondary_index = -1, flag = 1, group_secondary_index = -1;
    char *args[256];
    char *pos = NULL;
    char wifidir_mac[20], country[4], wifidir_ssid[33], wifi_group_direct_ssid[33];
    char WPS_manufacturer[33], WPS_modelname[33], WPS_devicename[33];
    u8 dev_channels[MAX_CHANNELS];
    u8 iface_list[ETH_ALEN * MAX_INTERFACE_ADDR_COUNT];
    u8 dev_address[ETH_ALEN], extra[MAX_CONFIG_LINE],
        group_dev_address[ETH_ALEN];
    u8 *buffer = pbuf;
    u16 cmd_len_wifidir = 0, cmd_len_wps = 0, tlv_len = 0, extra_len = 0, temp;
    u16 temp16 = 0;
    u8 temp8 = 0;
    u16 wps_model_len = 0, wps_serial_len = 0;
    u16 pri_category = 0, pri_sub_category = 0, config_methods = 0;
    u16 sec_category = 0, sec_sub_category = 0, group_sec_category =
        0, group_sec_sub_category = 0;
    u16 avail_period = 0, avail_interval = 0;
    u8 secondary_oui[4], group_secondary_oui[4];
    u16 WPS_specconfigmethods = 0, WPS_associationstate = 0,
        WPS_configurationerror = 0, WPS_devicepassword = 0;
    u8 dev_capability = 0, group_capability = 0, cmd_found = 0,
        group_owner_intent = 0, primary_oui[4], iface_count = 0,
        regulatory_class = 0, channel_number = 0, manageability = 0,
        op_regulatory_class = 0, op_channel_number = 0, invitation_flag = 0;
    u8 WPS_version = 0, WPS_setupstate = 0, WPS_requesttype =
        0, WPS_responsetype =
        0, WPS_UUID[WPS_UUID_MAX_LEN],
        WPS_primarydevicetype[WPS_DEVICE_TYPE_MAX_LEN], WPS_RFband =
        0, WPS_modelnumber[4], WPS_serialnumber[4];
    u8 go_config_timeout = 0, client_config_timeout = 0;
    u8 secondary_dev_count = 0, group_secondary_dev_count = 0;
    u8 secondary_dev_info[WPS_DEVICE_TYPE_LEN * MAX_SECONDARY_DEVICE_COUNT];
    u8 group_secondary_dev_info[WPS_DEVICE_TYPE_LEN *
                                MAX_GROUP_SECONDARY_DEVICE_COUNT];
    u8 wifidir_client_dev_count = 0, wifidir_client_dev_index = 0;
    wifidir_client_dev_info wifidir_client_dev_info_list[MAX_SECONDARY_DEVICE_COUNT];
    u8 wifidir_secondary_dev_count = 0, wifidir_total_secondary_dev_count = 0;
    u8 wifidir_group_total_ssid_len = 0, tlv_offset = 0, temp_dev_size = 0;
    u8 noa_index = 0, opp_ps = 0, ctwindow_opp_ps = 0, count_type = 0;
    u32 duration, interval, start_time;
    u16 total_chan_len = 0;
    u8 chan_entry_regulatory_class = 81, chan_entry_num_of_channels = 0;
    u8 *chan_entry_list = NULL;
    u8 *chan_buf = NULL;
    noa_descriptor noa_descriptor_list[MAX_NOA_DESCRIPTORS];
    struct wifidir_cfg *valid_cfg = cfg;

    if (!cfg || cfg->intent < 0 || cfg->intent > MAX_INTENT ||
        !strlen(cfg->name))
        valid_cfg = NULL;

    /* Check if file exists */
    config_file = fopen(file_name, "r");
    if (config_file == NULL) {
        printf("\nERR:Config file can not open.\n");
        return;
    }
    line = (char *) malloc(MAX_CONFIG_LINE);
    if (!line) {
        printf("ERR:Cannot allocate memory for line\n");
        goto done;
    }
    memset(line, 0, MAX_CONFIG_LINE);

    /* Parse file and process */
    while (config_get_line(line, MAX_CONFIG_LINE, config_file, &li, &pos)) {
        arg_num = parse_line(line, args);
        if (!cmd_found && strncmp(args[0], "wifidir_config", strlen(args[0])))
            continue;
        cmd_found = 1;
        if (strcmp(args[0], "wifidir_config") == 0) {
            wifidir_level = WIFIDIR_PARAMS_CONFIG;
        } else if (strcmp(args[0], "Capability") == 0) {
            wifidir_level = WIFIDIR_CAPABILITY_CONFIG;
        } else if (strcmp(args[0], "GroupOwnerIntent") == 0) {
            wifidir_level = WIFIDIR_GROUP_OWNER_INTENT_CONFIG;
        } else if (strcmp(args[0], "Channel") == 0) {
            wifidir_level = WIFIDIR_CHANNEL_CONFIG;
        } else if (strcmp(args[0], "OperatingChannel") == 0) {
            wifidir_level = WIFIDIR_OPCHANNEL_CONFIG;
        } else if (strcmp(args[0], "InfrastructureManageabilityInfo") == 0) {
            wifidir_level = WIFIDIR_MANAGEABILITY_CONFIG;
        } else if (strcmp(args[0], "InvitationFlagBitmap") == 0) {
            wifidir_level = WIFIDIR_INVITATION_FLAG_CONFIG;
        } else if (strcmp(args[0], "ChannelList") == 0) {
            wifidir_level = WIFIDIR_CHANNEL_LIST_CONFIG;
        } else if (strcmp(args[0], "NoticeOfAbsence") == 0) {
            wifidir_level = WIFIDIR_NOTICE_OF_ABSENCE;
        } else if (strcmp(args[0], "NoA_descriptor") == 0) {
            wifidir_level = WIFIDIR_NOA_DESCRIPTOR;
        } else if (strcmp(args[0], "DeviceInfo") == 0) {
            wifidir_level = WIFIDIR_DEVICE_INFO_CONFIG;
        } else if (strcmp(args[0], "SecondaryDeviceType") == 0) {
            wifidir_level = WIFIDIR_DEVICE_SEC_INFO_CONFIG;
        } else if (strcmp(args[0], "GroupInfo") == 0) {
            wifidir_level = WIFIDIR_GROUP_INFO_CONFIG;
        } else if (strcmp(args[0], "GroupSecondaryDeviceTypes") == 0) {
            wifidir_level = WIFIDIR_GROUP_SEC_INFO_CONFIG;
        } else if (strcmp(args[0], "GroupWifidirDeviceTypes") == 0) {
            wifidir_level = WIFIDIR_GROUP_CLIENT_INFO_CONFIG;
        } else if (strcmp(args[0], "GroupId") == 0) {
            wifidir_level = WIFIDIR_GROUP_ID_CONFIG;
        } else if (strcmp(args[0], "GroupBSSId") == 0) {
            wifidir_level = WIFIDIR_GROUP_BSS_ID_CONFIG;
        } else if (strcmp(args[0], "DeviceId") == 0) {
            wifidir_level = WIFIDIR_DEVICE_ID_CONFIG;
        } else if (strcmp(args[0], "Interface") == 0) {
            wifidir_level = WIFIDIR_INTERFACE_CONFIG;
        } else if (strcmp(args[0], "ConfigurationTimeout") == 0) {
            wifidir_level = WIFIDIR_TIMEOUT_CONFIG;
        } else if (strcmp(args[0], "ExtendedListenTime") == 0) {
            wifidir_level = WIFIDIR_EXTENDED_TIME_CONFIG;
        } else if (strcmp(args[0], "IntendedIntfAddress") == 0) {
            wifidir_level = WIFIDIR_INTENDED_ADDR_CONFIG;
        } else if (strcmp(args[0], "WPSIE") == 0) {
            wifidir_level = WIFIDIR_WPSIE;
        } else if (strcmp(args[0], "Extra") == 0) {
            wifidir_level = WIFIDIR_EXTRA;
        } else if (strcmp(args[0], "WIFIDIR_MAC") == 0 ||
                   strcmp(args[0], "GroupAddr") == 0 ||
                   strcmp(args[0], "GroupBssId") == 0 ||
                   strcmp(args[0], "InterfaceAddress") == 0 ||
                   strcmp(args[0], "GroupInterfaceAddress") == 0 ||
                   strcmp(args[0], "DeviceAddress") == 0) {
            strncpy(wifidir_mac, args[1], 20);
            if ((ret = mac2raw(wifidir_mac, dev_address)) != SUCCESS) {
                printf("ERR: %s Address \n",
                       ret == FAILURE ? "Invalid MAC" : ret ==
                       WIFIDIR_RET_MAC_BROADCAST ? "Broadcast" : "Multicast");
                goto done;
            }
            if (wifidir_is_mac_all_zero(dev_address) == SUCCESS)
                memcpy(dev_address, gpwps_info->wps_data.my_mac_addr, ETH_ALEN);
        } else if (strncmp(args[0], "GroupWifidirDeviceAddress", 21) == 0) {
            strncpy(wifidir_mac, args[1], 20);
            if ((ret = mac2raw(wifidir_mac, group_dev_address)) != SUCCESS) {
                printf("ERR: %s Address \n",
                       ret == FAILURE ? "Invalid MAC" : ret ==
                       WIFIDIR_RET_MAC_BROADCAST ? "Broadcast" : "Multicast");
                goto done;
            }
            wifidir_client_dev_index++;
            if (wifidir_client_dev_index > wifidir_client_dev_count) {
                printf
                    ("ERR: No of Client Dev count is less than no of client dev configs!!\n");
                goto done;
            }
            group_secondary_index = 0;
            wifidir_secondary_dev_count = 0;
            tlv_offset =
                wifidir_group_total_ssid_len +
                wifidir_total_secondary_dev_count * WPS_DEVICE_TYPE_LEN;
            memcpy(wifidir_client_dev_info_list[wifidir_client_dev_index - 1].
                   wifidir_dev_address + tlv_offset, group_dev_address, ETH_ALEN);
        } else if (strncmp(args[0], "GroupWifidirIntfAddress", 19) == 0) {
            strncpy(wifidir_mac, args[1], 20);
            if ((ret = mac2raw(wifidir_mac, group_dev_address)) != SUCCESS) {
                printf("ERR: %s Address \n",
                       ret == FAILURE ? "Invalid MAC" : ret ==
                       WIFIDIR_RET_MAC_BROADCAST ? "Broadcast" : "Multicast");
                goto done;
            }
            memcpy(wifidir_client_dev_info_list[wifidir_client_dev_index - 1].
                   wifidir_intf_address + tlv_offset, group_dev_address, ETH_ALEN);
        } else if (strncmp(args[0], "GroupWifidirDeviceCapab", 19) == 0) {
            if (is_input_valid(WIFIDIR_DEVICECAPABILITY, arg_num - 1, args + 1) !=
                SUCCESS) {
                goto done;
            }
            temp8 = atoi(args[1]);
            memcpy(&wifidir_client_dev_info_list[wifidir_client_dev_index - 1].
                   wifidir_dev_capability + tlv_offset, &temp8, sizeof(temp8));
        } else if (strncmp(args[0], "GroupWifidirWPSConfigMethods", 24) == 0) {
            if (is_input_valid(WIFIDIR_WPSCONFMETHODS, arg_num - 1, args + 1) !=
                SUCCESS) {
                goto done;
            }
            temp16 = (u16) A2HEXDECIMAL(args[1]);
            memcpy(&wifidir_client_dev_info_list[wifidir_client_dev_index - 1].
                   config_methods + tlv_offset, &temp16, sizeof(temp16));
            (u16) A2HEXDECIMAL(args[1]);
        } else if (strncmp(args[0], "GroupPrimaryDeviceTypeCategory", 30) == 0) {
            if (is_input_valid(WIFIDIR_PRIDEVTYPECATEGORY, arg_num - 1, args + 1) !=
                SUCCESS) {
                goto done;
            }
            pri_category = (u16) atoi(args[1]);
            temp16 = mwu_htons(pri_category);
            memcpy(&wifidir_client_dev_info_list[wifidir_client_dev_index - 1].
                   primary_category + tlv_offset, &temp16, sizeof(temp16));
        } else if (strncmp(args[0], "GroupPrimaryDeviceTypeOUI", 25) == 0) {
            if (is_input_valid(WIFIDIR_PRIDEVTYPEOUI, arg_num - 1, args + 1) !=
                SUCCESS) {
                goto done;
            }
            for (i = 0; i < 4; i++) {
                temp8 = (u8) A2HEXDECIMAL(args[i + 1]);
                memcpy(&wifidir_client_dev_info_list[wifidir_client_dev_index - 1].
                       primary_oui[i]
                       + tlv_offset, &temp8, sizeof(temp8));
            }
        } else if (strncmp(args[0], "GroupPrimaryDeviceTypeSubCategory", 33) ==
                   0) {
            if (is_input_valid(WIFIDIR_PRIDEVTYPESUBCATEGORY, arg_num - 1, args + 1)
                != SUCCESS) {
                goto done;
            }
            pri_sub_category = (u16) atoi(args[1]);
            temp16 = mwu_htons(pri_sub_category);
            memcpy(&wifidir_client_dev_info_list[wifidir_client_dev_index - 1].
                   primary_subcategory + tlv_offset, &temp16, sizeof(temp16));
        } else if (strncmp(args[0], "GroupSecondaryDeviceCount", 25) == 0) {
            if (is_input_valid
                (WIFIDIR_GROUP_SECONDARYDEVCOUNT, arg_num - 1,
                 args + 1) != SUCCESS) {
                goto done;
            }
            group_secondary_dev_count = (u8) atoi(args[1]);
            memcpy(&wifidir_client_dev_info_list[wifidir_client_dev_index - 1].
                   wifidir_secondary_dev_count + tlv_offset,
                   &group_secondary_dev_count,
                   sizeof(group_secondary_dev_count));
            wifidir_total_secondary_dev_count += group_secondary_dev_count;
            if (group_secondary_dev_count)
                memset(group_secondary_dev_info, 0,
                       sizeof(group_secondary_dev_info));
        } else if (strncmp(args[0], "GroupSecondaryDeviceTypeCategory", 30) ==
                   0) {
            if (is_input_valid(WIFIDIR_PRIDEVTYPECATEGORY, arg_num - 1, args + 1) !=
                SUCCESS) {
                goto done;
            }
            sec_category = (u16) atoi(args[1]);
            group_sec_category = wlan_cpu_to_le16(sec_category);
            group_secondary_index++;
        } else if (strncmp(args[0], "GroupSecondaryDeviceTypeOUI", 27) == 0) {
            if (is_input_valid(WIFIDIR_PRIDEVTYPEOUI, arg_num - 1, args + 1) !=
                SUCCESS) {
                goto done;
            }
            for (i = 0; i < 4; i++)
                group_secondary_oui[i] = (u8) A2HEXDECIMAL(args[i + 1]);
        } else if (strncmp(args[0], "GroupSecondaryDeviceTypeSubCategory", 35)
                   == 0) {
            if (is_input_valid(WIFIDIR_PRIDEVTYPESUBCATEGORY, arg_num - 1, args + 1)
                != SUCCESS) {
                goto done;
            }
            if (group_secondary_index < 0 ||
                group_secondary_index >= MAX_SECONDARY_DEVICE_COUNT) {
                printf("Error in configuration file %s:%d", file_name, li);
                goto done;
            }
            sec_sub_category = (u16) atoi(args[1]);
            group_sec_sub_category = wlan_cpu_to_le16(group_sec_sub_category);
            if (group_secondary_dev_count) {
                memcpy(&group_secondary_dev_info
                       [(group_secondary_index - 1) * WPS_DEVICE_TYPE_LEN],
                       &group_sec_category, sizeof(u16));
                memcpy(&group_secondary_dev_info
                       [((group_secondary_index - 1) * WPS_DEVICE_TYPE_LEN) +
                        2], group_secondary_oui, sizeof(secondary_oui));
                memcpy(&group_secondary_dev_info
                       [((group_secondary_index - 1) * WPS_DEVICE_TYPE_LEN) +
                        6], &group_sec_sub_category, sizeof(u16));
            }

        } else if (strncmp(args[0], "GroupWifidirDeviceCount", 19) == 0) {
            if (is_input_valid(WIFIDIR_SECONDARYDEVCOUNT, arg_num - 1, args + 1) !=
                SUCCESS) {
                goto done;
            }
            wifidir_client_dev_count = (u8) atoi(args[1]);
        } else if (strncmp(args[0], "GroupWifidirDeviceName", 18) == 0) {

            if (is_input_valid(WIFIDIR_GROUP_WIFIDIR_DEVICE_NAME, arg_num - 1, args + 1)
                != SUCCESS) {
                goto done;
            }

            strncpy(wifi_group_direct_ssid, args[1] + 1, strlen(args[1]) - 2);
            wifi_group_direct_ssid[strlen(args[1]) - 2] = '\0';
            temp = mwu_htons(SC_Device_Name);
            memcpy(((u8 *) & wifidir_client_dev_info_list[wifidir_client_dev_index - 1].
                    device_name_type + tlv_offset), &temp, sizeof(temp));
            temp = mwu_htons(strlen(wifi_group_direct_ssid));
            memcpy(((u8 *) & wifidir_client_dev_info_list[wifidir_client_dev_index - 1].
                    wifidir_device_name_len + tlv_offset), &temp, sizeof(temp));
            memset(((u8 *) & wifidir_client_dev_info_list[wifidir_client_dev_index - 1].
                    wifidir_device_name + tlv_offset), 0,
                   strlen(wifi_group_direct_ssid));
            memcpy(((u8 *) & wifidir_client_dev_info_list[wifidir_client_dev_index - 1].
                    wifidir_device_name + tlv_offset), &wifi_group_direct_ssid,
                   strlen(wifi_group_direct_ssid));
            wifidir_group_total_ssid_len += strlen(wifi_group_direct_ssid);

            if (wifidir_client_dev_index - 1) {
                temp_dev_size =
                    sizeof(wifidir_client_dev_info) +
                    strlen(wifi_group_direct_ssid) +
                    group_secondary_dev_count * WPS_DEVICE_TYPE_LEN;
                memcpy(&wifidir_client_dev_info_list[wifidir_client_dev_index - 1].
                       dev_length + (tlv_offset -
                                     (group_secondary_dev_count *
                                      WPS_DEVICE_TYPE_LEN)), &temp_dev_size,
                       sizeof(temp_dev_size));
            } else {

                temp_dev_size =
                    sizeof(wifidir_client_dev_info) +
                    strlen(wifi_group_direct_ssid) +
                    group_secondary_dev_count * WPS_DEVICE_TYPE_LEN;
                wifidir_client_dev_info_list[wifidir_client_dev_index - 1].dev_length =
                    sizeof(wifidir_client_dev_info) +
                    strlen(wifi_group_direct_ssid) +
                    group_secondary_dev_count * WPS_DEVICE_TYPE_LEN;
            }

        } else if (strcmp(args[0], "DeviceCapability") == 0) {
            if (is_input_valid(WIFIDIR_DEVICECAPABILITY, arg_num - 1, args + 1) !=
                SUCCESS) {
                goto done;
            }
            dev_capability = (u8) atoi(args[1]);
        } else if (strcmp(args[0], "GroupCapability") == 0) {
            if (is_input_valid(WIFIDIR_GROUPCAPABILITY, arg_num - 1, args + 1) !=
                SUCCESS) {
                goto done;
            }
            group_capability = (u8) atoi(args[1]);
        } else if (strcmp(args[0], "Intent") == 0) {
            /* Intent -> 0 - 15 */
            if (valid_cfg)
                group_owner_intent = (u8) valid_cfg->intent;
            else if (is_input_valid(WIFIDIR_INTENT, arg_num - 1, args + 1) == SUCCESS)
                group_owner_intent = (u8) atoi(args[1]);
            else
                goto done;
        } else if (strcmp(args[0], "RegulatoryClass") == 0) {
            if (is_input_valid(WIFIDIR_REGULATORYCLASS, arg_num - 1, args + 1) !=
                SUCCESS) {
                goto done;
            }
            regulatory_class = (u8) atoi(args[1]);
        } else if (strcmp(args[0], "ChannelNumber") == 0) {
            if (valid_cfg && valid_cfg->listen_chan >= 1
                && valid_cfg->listen_chan <= MAX_CHANNELS) {
                channel_number = (u8) valid_cfg->listen_chan;/* Set listen channel */
            } else {
                if (is_input_valid(CHANNEL, arg_num - 1, args + 1) != SUCCESS) {
                    goto done;
                }
                channel_number = (u8) atoi(args[1]);
            }
        } else if (strcmp(args[0], "OpRegulatoryClass") == 0) {
            if (is_input_valid(WIFIDIR_REGULATORYCLASS, arg_num - 1, args + 1) !=
                SUCCESS) {
                goto done;
            }
            op_regulatory_class = (u8) atoi(args[1]);
        } else if (strcmp(args[0], "OpChannelNumber") == 0) {
            if (valid_cfg && valid_cfg->operating_chan >= 1
                && valid_cfg->operating_chan <= MAX_CHANNELS) {
                op_channel_number = (u8) valid_cfg->operating_chan;  /*set the op_chan*/
            } else {
                if (is_input_valid(CHANNEL, arg_num - 1, args + 1) != SUCCESS) {
                    goto done;
                }
                op_channel_number = (u8) atoi(args[1]);
            }
        } else if (strcmp(args[0], "Manageability") == 0) {
            if (is_input_valid(WIFIDIR_MANAGEABILITY, arg_num - 1, args + 1)
                != SUCCESS) {
                goto done;
            }
            manageability = (u8) atoi(args[1]);
        } else if (strcmp(args[0], "InvitationFlag") == 0) {
            if (is_input_valid(WIFIDIR_INVITATIONFLAG, arg_num - 1, args + 1)
                != SUCCESS) {
                goto done;
            }
            invitation_flag = (u8) atoi(args[1]);
        } else if (strcmp(args[0], "CountryString") == 0) {
            if (is_input_valid(WIFIDIR_COUNTRY, arg_num - 1, args + 1) != SUCCESS) {
                goto done;
            }
            strncpy(country, args[1] + 1, 3);
            country[strlen(args[1]) - 2] = '\0';
        } else if (strncmp(args[0], "Regulatory_Class_", 17) == 0) {
            if (is_input_valid(WIFIDIR_REGULATORYCLASS, arg_num - 1, args + 1) !=
                SUCCESS) {
                goto done;
            }
            if (!no_of_chan_entries) {
                chan_entry_list = (u8 *) malloc(MAX_BUFFER_SIZE);
                if (!chan_entry_list) {
                    printf("ERR:cannot allocate memory for chan_entry_list!\n");
                    goto done;
                }
                memset(chan_entry_list, 0, MAX_BUFFER_SIZE);
                chan_buf = chan_entry_list;
            }
            no_of_chan_entries++;
            chan_entry_regulatory_class = (u8) atoi(args[1]);
        } else if (strncmp(args[0], "NumofChannels", 13) == 0) {
            if (is_input_valid(WIFIDIR_NO_OF_CHANNELS, arg_num - 1, args + 1) !=
                SUCCESS) {
                goto done;
            }
            chan_entry_num_of_channels = (u8) atoi(args[1]);
        } else if (strncmp(args[0], "ChanList", 8) == 0) {
            if (chan_entry_num_of_channels != (arg_num - 1)) {
                printf
                    ("ERR: ChanList channels and NumofChannels do not match!\n");
                goto done;
            }
            if (is_input_valid(SCANCHANNELS, arg_num - 1, args + 1) != SUCCESS) {
                goto done;
            }
            for (i = 0; i < chan_entry_num_of_channels; i++)
                dev_channels[i] = (u8) atoi(args[i + 1]);
            total_chan_len += chan_entry_num_of_channels;
            memcpy(chan_buf, &chan_entry_regulatory_class, sizeof(u8));
            memcpy(chan_buf + 1, &chan_entry_num_of_channels, sizeof(u8));
            memcpy(chan_buf + 2, dev_channels, chan_entry_num_of_channels);
            chan_buf += sizeof(u8) + sizeof(u8) + chan_entry_num_of_channels;
        } else if (strcmp(args[0], "NoA_Index") == 0) {
            if (is_input_valid(WIFIDIR_NOA_INDEX, arg_num - 1, args + 1) != SUCCESS) {
                goto done;
            }
            noa_index = (u8) atoi(args[1]);
        } else if (strcmp(args[0], "OppPS") == 0) {
            if (is_input_valid(WIFIDIR_OPP_PS, arg_num - 1, args + 1) != SUCCESS) {
                goto done;
            }
            opp_ps = (u8) atoi(args[1]);
        } else if (strcmp(args[0], "CTWindow") == 0) {
            if (is_input_valid(WIFIDIR_CTWINDOW, arg_num - 1, args + 1) != SUCCESS) {
                goto done;
            }
            ctwindow_opp_ps = (u8) atoi(args[1]) | SET_OPP_PS(opp_ps);
        } else if (strncmp(args[0], "CountType", 9) == 0) {
            if (is_input_valid(WIFIDIR_COUNT_TYPE, arg_num - 1, args + 1) !=
                SUCCESS) {
                goto done;
            }
            no_of_noa++;
            if (no_of_noa > MAX_NOA_DESCRIPTORS) {
                printf("Number of descriptors should not be greater than %d\n",
                       MAX_NOA_DESCRIPTORS);
                goto done;
            }
            count_type = (u8) atoi(args[1]);
            noa_descriptor_list[no_of_noa - 1].count_type = count_type;
        } else if (strncmp(args[0], "GroupWifidirDeviceCount", 19) == 0) {
            if (is_input_valid(WIFIDIR_SECONDARYDEVCOUNT, arg_num - 1, args + 1) !=
                SUCCESS) {
                goto done;
            }
            wifidir_client_dev_count = (u8) atoi(args[1]);
        } else if (strncmp(args[0], "GroupWifidirDeviceName", 18) == 0) {

            memset(wifi_group_direct_ssid, 0, sizeof(wifi_group_direct_ssid));
            strncpy(wifi_group_direct_ssid, args[1],
                    sizeof(wifi_group_direct_ssid));
            temp = mwu_htons(SC_Device_Name);
            memcpy(((u8 *) & wifidir_client_dev_info_list[wifidir_client_dev_index - 1].
                    device_name_type + tlv_offset), &temp, sizeof(temp));
            temp = mwu_htons(strlen(wifi_group_direct_ssid));
            memcpy(((u8 *) & wifidir_client_dev_info_list[wifidir_client_dev_index - 1].
                    wifidir_device_name_len + tlv_offset), &temp, sizeof(temp));
            memset(((u8 *) & wifidir_client_dev_info_list[wifidir_client_dev_index - 1].
                    wifidir_device_name + tlv_offset), 0,
                   strlen(wifi_group_direct_ssid));
            memcpy(((u8 *) & wifidir_client_dev_info_list[wifidir_client_dev_index - 1].
                    wifidir_device_name + tlv_offset), &wifi_group_direct_ssid,
                   strlen(wifi_group_direct_ssid));
            wifidir_group_total_ssid_len += strlen(wifi_group_direct_ssid);

            if (wifidir_client_dev_index - 1) {
                temp_dev_size =
                    sizeof(wifidir_client_dev_info) +
                    strlen(wifi_group_direct_ssid) +
                    group_secondary_dev_count * WPS_DEVICE_TYPE_LEN;
                memcpy(&wifidir_client_dev_info_list[wifidir_client_dev_index - 1].
                       dev_length + (tlv_offset -
                                     (group_secondary_dev_count *
                                      WPS_DEVICE_TYPE_LEN)), &temp_dev_size,
                       sizeof(temp_dev_size));
            } else {

                temp_dev_size =
                    sizeof(wifidir_client_dev_info) +
                    strlen(wifi_group_direct_ssid) +
                    group_secondary_dev_count * WPS_DEVICE_TYPE_LEN;
                wifidir_client_dev_info_list[wifidir_client_dev_index - 1].dev_length =
                    sizeof(wifidir_client_dev_info) +
                    strlen(wifi_group_direct_ssid) +
                    group_secondary_dev_count * WPS_DEVICE_TYPE_LEN;
            }
            interval = (u32) atoi(args[1]);
            interval = wlan_cpu_to_le16(interval);
            noa_descriptor_list[no_of_noa - 1].interval = interval;
        } else if (strncmp(args[0], "StartTime", 9) == 0) {
            if (is_input_valid(WIFIDIR_START_TIME, arg_num - 1, args + 1) !=
                SUCCESS) {
                goto done;
            }
            start_time = (u32) atoi(args[1]);
            start_time = wlan_cpu_to_le16(start_time);
            noa_descriptor_list[no_of_noa - 1].start_time = start_time;
        } else if (strcmp(args[0], "PrimaryDeviceTypeCategory") == 0) {
            if (is_input_valid(WIFIDIR_PRIDEVTYPECATEGORY, arg_num - 1, args + 1) !=
                SUCCESS) {
                goto done;
            }
            pri_category = (u16) atoi(args[1]);
        } else if (strncmp(args[0], "Duration", 8) == 0) {
            if (is_input_valid(WIFIDIR_DURATION, arg_num - 1, args + 1) != SUCCESS) {
                goto done;
            }
            duration = (u32) atoi(args[1]);
            duration = wlan_cpu_to_le16(duration);
            noa_descriptor_list[no_of_noa - 1].duration = duration;
        } else if (strncmp(args[0], "Interval", 8) == 0) {
            if (is_input_valid(WIFIDIR_INTERVAL, arg_num - 1, args + 1) != SUCCESS) {
                goto done;
            }
            interval = (u32) atoi(args[1]);
            interval = wlan_cpu_to_le16(interval);
            noa_descriptor_list[no_of_noa - 1].interval = interval;
        } else if (strncmp(args[0], "StartTime", 9) == 0) {
            if (is_input_valid(WIFIDIR_START_TIME, arg_num - 1, args + 1) !=
                SUCCESS) {
                goto done;
            }
            start_time = (u32) atoi(args[1]);
            start_time = wlan_cpu_to_le16(start_time);
            noa_descriptor_list[no_of_noa - 1].start_time = start_time;
        } else if (strcmp(args[0], "PrimaryDeviceTypeCategory") == 0) {
            if (is_input_valid(WIFIDIR_PRIDEVTYPECATEGORY, arg_num - 1, args + 1) !=
                SUCCESS) {
                goto done;
            }
            pri_category = (u16) atoi(args[1]);
        } else if (strcmp(args[0], "PrimaryDeviceTypeOUI") == 0) {
            if (is_input_valid(WIFIDIR_PRIDEVTYPEOUI, arg_num - 1, args + 1) !=
                SUCCESS) {
                goto done;
            }
            for (i = 0; i < 4; i++)
                primary_oui[i] = (u8) A2HEXDECIMAL(args[i + 1]);
        } else if (strcmp(args[0], "PrimaryDeviceTypeSubCategory") == 0) {
            if (is_input_valid(WIFIDIR_PRIDEVTYPESUBCATEGORY, arg_num - 1, args + 1)
                != SUCCESS) {
                goto done;
            }
            pri_sub_category = (u16) atoi(args[1]);
        } else if (strcmp(args[0], "SecondaryDeviceCount") == 0) {
            if (is_input_valid(WIFIDIR_SECONDARYDEVCOUNT, arg_num - 1, args + 1) !=
                SUCCESS) {
                goto done;
            }
            secondary_dev_count = (u8) atoi(args[1]);
            if (secondary_dev_count)
                memset(secondary_dev_info, 0, sizeof(secondary_dev_info));
        } else if (strncmp(args[0], "SecondaryDeviceTypeCategory", 27) == 0) {
            if (is_input_valid(WIFIDIR_PRIDEVTYPECATEGORY, arg_num - 1, args + 1) !=
                SUCCESS) {
                goto done;
            }
            sec_category = (u16) atoi(args[1]);
            sec_category = mwu_htons(sec_category);
            secondary_index++;
        } else if (strncmp(args[0], "SecondaryDeviceTypeOUI", 22) == 0) {
            if (is_input_valid(WIFIDIR_PRIDEVTYPEOUI, arg_num - 1, args + 1) !=
                SUCCESS) {
                goto done;
            }
            for (i = 0; i < 4; i++)
                secondary_oui[i] = (u8) A2HEXDECIMAL(args[i + 1]);
        } else if (strncmp(args[0], "SecondaryDeviceTypeSubCategory", 30) == 0) {
            if (is_input_valid(WIFIDIR_PRIDEVTYPESUBCATEGORY, arg_num - 1, args + 1)
                != SUCCESS) {
                goto done;
            }
            if (secondary_index < 0 ||
                secondary_index >= MAX_SECONDARY_DEVICE_COUNT) {
                printf("Error in configuration file %s:%d", file_name, li);
                goto done;
            }
            sec_sub_category = (u16) atoi(args[1]);
            sec_sub_category = mwu_htons(sec_sub_category);
            if (secondary_dev_count) {
                memcpy(&secondary_dev_info
                       [secondary_index * WPS_DEVICE_TYPE_LEN], &sec_category,
                       sizeof(u16));
                memcpy(&secondary_dev_info
                       [(secondary_index * WPS_DEVICE_TYPE_LEN) + 2],
                       secondary_oui, sizeof(secondary_oui));
                memcpy(&secondary_dev_info
                       [(secondary_index * WPS_DEVICE_TYPE_LEN) + 6],
                       &sec_sub_category, sizeof(u16));
            }
        } else if (strcmp(args[0], "InterfaceAddressCount") == 0) {
            if (is_input_valid(WIFIDIR_INTERFACECOUNT, arg_num - 1, args + 1) !=
                SUCCESS) {
                goto done;
            }
            iface_count = (u8) atoi(args[1]);
        } else if (strcmp(args[0], "InterfaceAddressList") == 0) {
            if (iface_count != (arg_num - 1)) {
                printf("Incorrect address list for %d entries.\n", iface_count);
                goto done;
            }
            for (i = 0; i < iface_count && i < MAX_INTERFACE_ADDR_COUNT; i++) {
                if ((ret =
                     mac2raw(args[i + 1],
                             &iface_list[i * ETH_ALEN])) != SUCCESS) {
                    printf("ERR: %s Address \n",
                           ret == FAILURE ? "Invalid MAC" : ret ==
                           WIFIDIR_RET_MAC_BROADCAST ? "Broadcast" : "Multicast");
                    goto done;
                }
            }
        } else if (strcmp(args[0], "GroupConfigurationTimeout") == 0) {
            if (is_input_valid(WIFIDIR_ATTR_CONFIG_TIMEOUT, arg_num - 1, args + 1)
                != SUCCESS) {
                goto done;
            }
            go_config_timeout = (u8) atoi(args[1]);
        } else if (strcmp(args[0], "ClientConfigurationTimeout") == 0) {
            if (is_input_valid(WIFIDIR_ATTR_CONFIG_TIMEOUT, arg_num - 1, args + 1)
                != SUCCESS) {
                goto done;
            }
            client_config_timeout = (u8) atoi(args[1]);
        } else if (strcmp(args[0], "AvailabilityPeriod") == 0) {
            if (is_input_valid(WIFIDIR_ATTR_EXTENDED_TIME, arg_num - 1, args + 1) !=
                SUCCESS) {
                goto done;
            }
            avail_period = (u16) atoi(args[1]);
        } else if (strcmp(args[0], "AvailabilityInterval") == 0) {
            if (is_input_valid(WIFIDIR_ATTR_EXTENDED_TIME, arg_num - 1, args + 1) !=
                SUCCESS) {
                goto done;
            }
            avail_interval = (u16) atoi(args[1]);
        } else if (strcmp(args[0], "WPSConfigMethods") == 0) {
            if (valid_cfg && valid_cfg->config_methods >= MIN_WPS_CONF_METHODS
                && valid_cfg->config_methods <= MAX_WPS_CONF_METHODS) {
                config_methods = valid_cfg->config_methods;
            } else {
                if (is_input_valid(WIFIDIR_WPSCONFMETHODS, arg_num - 1, args + 1) !=
                    SUCCESS) {
                    goto done;
                }
                config_methods = (u16) A2HEXDECIMAL(args[1]);
            }
        } else if (strcmp(args[0], "DeviceName") == 0 ||
                   strcmp(args[0], "GroupSsId") == 0) {
            memset(wifidir_ssid, 0, sizeof(wifidir_ssid));
            if (valid_cfg) {
                strncpy(wifidir_ssid, valid_cfg->name, sizeof(wifidir_ssid));
            } else if (wifidir_is_default_name(args[1]) == SUCCESS) {
                if (wifidir_create_default_name(wifidir_ssid,
                        gpwps_info->wps_data.my_mac_addr) != SUCCESS)
                    goto done;
            } else {
                strncpy(wifidir_ssid, args[1], sizeof(wifidir_ssid));
            }
        } else if (strcmp(args[0], "WPSVersion") == 0) {
            if (is_input_valid(WIFIDIR_WPSVERSION, arg_num - 1, args + 1) !=
                SUCCESS) {
                goto done;
            }
            WPS_version = (u8) A2HEXDECIMAL(args[1]);
        } else if (strcmp(args[0], "WPSSetupState") == 0) {
            if (is_input_valid(WIFIDIR_WPSSETUPSTATE, arg_num - 1, args + 1) !=
                SUCCESS) {
                goto done;
            }
            WPS_setupstate = (u8) A2HEXDECIMAL(args[1]);
        } else if (strcmp(args[0], "WPSDeviceName") == 0) {
            if (is_input_valid(WIFIDIR_WPSDEVICENAME, arg_num - 1, args + 1) !=
                SUCCESS) {
                goto done;
            }
            memset(WPS_devicename, 0, sizeof(WPS_devicename));
            if (valid_cfg) {
                strncpy(WPS_devicename, valid_cfg->name, sizeof(WPS_devicename));
            } else if (wifidir_is_default_name(args[1]) == SUCCESS) {
                if (wifidir_create_default_name(WPS_devicename,
                        gpwps_info->wps_data.my_mac_addr) != SUCCESS)
                    goto done;
            } else {
                strncpy(WPS_devicename, args[1], sizeof(WPS_devicename));
            }
        } else if (strcmp(args[0], "WPSRequestType") == 0) {
            if (is_input_valid(WIFIDIR_WPSREQRESPTYPE, arg_num - 1, args + 1) !=
                SUCCESS) {
                goto done;
            }
            WPS_requesttype = (u8) A2HEXDECIMAL(args[1]);
        } else if (strcmp(args[0], "WPSResponseType") == 0) {
            if (is_input_valid(WIFIDIR_WPSREQRESPTYPE, arg_num - 1, args + 1) !=
                SUCCESS) {
                goto done;
            }
            WPS_responsetype = (u8) A2HEXDECIMAL(args[1]);
        } else if (strcmp(args[0], "WPSSpecConfigMethods") == 0) {
            if (valid_cfg && valid_cfg->config_methods >= MIN_WPS_CONF_METHODS
                && valid_cfg->config_methods <= MAX_WPS_CONF_METHODS) {
                WPS_specconfigmethods = valid_cfg->config_methods;
            } else {
                if (is_input_valid(WIFIDIR_WPSSPECCONFMETHODS, arg_num - 1, args + 1) !=
                    SUCCESS) {
                    goto done;
                }
                WPS_specconfigmethods = (u16) A2HEXDECIMAL(args[1]);
            }
        } else if (strcmp(args[0], "WPSUUID") == 0) {
            if (valid_cfg && memcmp(valid_cfg->UUID,
                "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
                WPS_UUID_MAX_LEN)) {
                memcpy(WPS_UUID, valid_cfg->UUID, WPS_UUID_MAX_LEN);
            } else {
                if (is_input_valid(WIFIDIR_WPSUUID, arg_num - 1, args + 1) != SUCCESS) {
                    goto done;
                }
                for (i = 0; i < WPS_UUID_MAX_LEN; i++)
                    WPS_UUID[i] = (u8) A2HEXDECIMAL(args[i + 1]);
            }
        } else if (strcmp(args[0], "WPSPrimaryDeviceType") == 0) {
            if (is_input_valid(WIFIDIR_WPSPRIMARYDEVICETYPE, arg_num - 1, args + 1)
                != SUCCESS) {
                goto done;
            }
            for (i = 0; i < WPS_DEVICE_TYPE_MAX_LEN; i++)
                WPS_primarydevicetype[i] = (u8) A2HEXDECIMAL(args[i + 1]);
        } else if (strcmp(args[0], "WPSRFBand") == 0) {
            if (is_input_valid(WIFIDIR_WPSRFBAND, arg_num - 1, args + 1) != SUCCESS) {
                goto done;
            }
            WPS_RFband = (u8) A2HEXDECIMAL(args[1]);
        } else if (strcmp(args[0], "WPSAssociationState") == 0) {
            if (is_input_valid(WIFIDIR_WPSASSOCIATIONSTATE, arg_num - 1, args + 1)
                != SUCCESS) {
                goto done;
            }
            WPS_associationstate = (u16) A2HEXDECIMAL(args[1]);
        } else if (strcmp(args[0], "WPSConfigurationError") == 0) {
            if (is_input_valid(WIFIDIR_WPSCONFIGURATIONERROR, arg_num - 1, args + 1)
                != SUCCESS) {
                goto done;
            }
            WPS_configurationerror = (u16) A2HEXDECIMAL(args[1]);
        } else if (strcmp(args[0], "WPSDevicePassword") == 0) {
            if (is_input_valid(WIFIDIR_WPSDEVICEPASSWORD, arg_num - 1, args + 1) !=
                SUCCESS) {
                goto done;
            }
            WPS_devicepassword = (u16) A2HEXDECIMAL(args[1]);
        } else if (strcmp(args[0], "WPSManufacturer") == 0) {
            if (is_input_valid(WIFIDIR_WPSMANUFACTURER, arg_num - 1, args + 1) !=
                SUCCESS) {
                goto done;
            }
            memset(WPS_manufacturer, 0, sizeof(WPS_manufacturer));
            strncpy(WPS_manufacturer, args[1], sizeof(WPS_manufacturer));
        } else if (strcmp(args[0], "WPSModelName") == 0) {
            if (is_input_valid(WIFIDIR_WPSMODELNAME, arg_num - 1, args + 1) !=
                SUCCESS) {
                goto done;
            }
            memset(WPS_modelname, 0, sizeof(WPS_modelname));
            strncpy(WPS_modelname, args[1], sizeof(WPS_modelname));
        } else if (strcmp(args[0], "WPSModelNumber") == 0) {
            if (is_input_valid(WIFIDIR_WPSMODELNUMBER, arg_num - 1, args + 1) !=
                SUCCESS) {
                goto done;
            }
            for (i = 0; i < arg_num - 1; i++)
                WPS_modelnumber[i] = (u8) A2HEXDECIMAL(args[i + 1]);
            wps_model_len = arg_num - 1;
        } else if (strcmp(args[0], "WPSSerialNumber") == 0) {
            if (is_input_valid(WIFIDIR_WPSSERIALNUMBER, arg_num - 1, args + 1) !=
                SUCCESS) {
                goto done;
            }
            for (i = 0; i < arg_num - 1; i++)
                WPS_serialnumber[i] = (u8) A2HEXDECIMAL(args[i + 1]);
            wps_serial_len = arg_num - 1;
        } else if (strcmp(args[0], "Buffer") == 0) {
            for (i = 0; i < arg_num - 1; i++)
                extra[i] = (u8) A2HEXDECIMAL(args[i + 1]);
            extra_len = arg_num - 1;
        } else if (strcmp(args[0], "}") == 0) {
            /* Based on level, populate appropriate struct */
            switch (wifidir_level) {
            case WIFIDIR_DEVICE_ID_CONFIG:
                {
                    tlvbuf_wifidir_device_id *tlv = NULL;
                    /* Append a new TLV */
                    tlv_len = sizeof(tlvbuf_wifidir_device_id);
                    tlv = (tlvbuf_wifidir_device_id *) (buffer + cmd_len_wifidir);
                    cmd_len_wifidir += tlv_len;
                    /* Set TLV fields */
                    tlv->tag = TLV_TYPE_WIFIDIR_DEVICE_ID;
                    tlv->length = tlv_len - (sizeof(u8) + sizeof(u16));
                    memcpy(tlv->dev_mac_address, dev_address, ETH_ALEN);
                    endian_convert_tlv_wifidir_header_out(tlv);
                    break;
                }
            case WIFIDIR_CAPABILITY_CONFIG:
                {
                    tlvbuf_wifidir_capability *tlv = NULL;
                    /* Append a new TLV */
                    tlv_len = sizeof(tlvbuf_wifidir_capability);
                    tlv = (tlvbuf_wifidir_capability *) (buffer + cmd_len_wifidir);
                    cmd_len_wifidir += tlv_len;
                    /* Set TLV fields */
                    tlv->tag = TLV_TYPE_WIFIDIR_CAPABILITY;
                    tlv->length = tlv_len - (sizeof(u8) + sizeof(u16));
                    tlv->dev_capability = dev_capability;
                    tlv->group_capability = group_capability;
                    endian_convert_tlv_wifidir_header_out(tlv);
                    break;
                }
            case WIFIDIR_GROUP_OWNER_INTENT_CONFIG:
                {
                    tlvbuf_wifidir_group_owner_intent *tlv = NULL;
                    /* Append a new TLV */
                    tlv_len = sizeof(tlvbuf_wifidir_group_owner_intent);
                    tlv =
                        (tlvbuf_wifidir_group_owner_intent *) (buffer +
                                                           cmd_len_wifidir);
                    cmd_len_wifidir += tlv_len;
                    /* Set TLV fields */
                    tlv->tag = TLV_TYPE_WIFIDIR_GROUPOWNER_INTENT;
                    tlv->length = tlv_len - (sizeof(u8) + sizeof(u16));
                    tlv->dev_intent = group_owner_intent;
                    endian_convert_tlv_wifidir_header_out(tlv);
                    break;
                }
            case WIFIDIR_MANAGEABILITY_CONFIG:
                {
                    tlvbuf_wifidir_manageability *tlv = NULL;
                    /* Append a new TLV */
                    tlv_len = sizeof(tlvbuf_wifidir_manageability);
                    tlv = (tlvbuf_wifidir_manageability *) (buffer + cmd_len_wifidir);
                    cmd_len_wifidir += tlv_len;
                    /* Set TLV fields */
                    tlv->tag = TLV_TYPE_WIFIDIR_MANAGEABILITY;
                    tlv->length = tlv_len - (sizeof(u8) + sizeof(u16));
                    tlv->manageability = manageability;
                    endian_convert_tlv_wifidir_header_out(tlv);
                    break;
                }
            case WIFIDIR_INVITATION_FLAG_CONFIG:
                {
                    tlvbuf_wifidir_invitation_flag *tlv = NULL;
                    /* Append a new TLV */
                    tlv_len = sizeof(tlvbuf_wifidir_invitation_flag);
                    tlv = (tlvbuf_wifidir_invitation_flag *) (buffer + cmd_len_wifidir);
                    cmd_len_wifidir += tlv_len;
                    /* Set TLV fields */
                    tlv->tag = TLV_TYPE_WIFIDIR_INVITATION_FLAG;
                    tlv->length = tlv_len - (sizeof(u8) + sizeof(u16));
                    tlv->invitation_flag |= invitation_flag;
                    endian_convert_tlv_wifidir_header_out(tlv);
                    break;
                }
            case WIFIDIR_CHANNEL_LIST_CONFIG:
                {
                    tlvbuf_wifidir_channel_list *tlv = NULL;
                    /* Append a new TLV */
                    tlv_len = sizeof(tlvbuf_wifidir_channel_list) +
                        no_of_chan_entries * sizeof(chan_entry) +
                        total_chan_len;
                    tlv = (tlvbuf_wifidir_channel_list *) (buffer + cmd_len_wifidir);
                    cmd_len_wifidir += tlv_len;
                    /* Set TLV fields */
                    tlv->tag = TLV_TYPE_WIFIDIR_CHANNEL_LIST;
                    tlv->length = tlv_len - (sizeof(u8) + sizeof(u16));
                    memcpy(tlv->country_string, country, 3);
                    if (tlv->country_string[2] == 0)
                        tlv->country_string[2] = WIFIDIR_COUNTRY_LAST_BYTE;
                    memcpy(tlv->wifidir_chan_entry_list, chan_entry_list,
                           (tlv->length - 3));
                    endian_convert_tlv_wifidir_header_out(tlv);
                    break;
                }
            case WIFIDIR_NOTICE_OF_ABSENCE:
                {
                    tlvbuf_wifidir_notice_of_absence *tlv = NULL;
                    /* Append a new TLV */
                    tlv_len = sizeof(tlvbuf_wifidir_notice_of_absence) +
                        no_of_noa * sizeof(noa_descriptor);
                    tlv =
                        (tlvbuf_wifidir_notice_of_absence *) (buffer + cmd_len_wifidir);
                    cmd_len_wifidir += tlv_len;
                    /* Set TLV fields */
                    tlv->tag = TLV_TYPE_WIFIDIR_NOTICE_OF_ABSENCE;
                    tlv->length = tlv_len - (sizeof(u8) + sizeof(u16));
                    tlv->noa_index = noa_index;
                    tlv->ctwindow_opp_ps = ctwindow_opp_ps;
                    memcpy(tlv->wifidir_noa_descriptor_list,
                           noa_descriptor_list,
                           no_of_noa * sizeof(noa_descriptor));
                    endian_convert_tlv_wifidir_header_out(tlv);
                    flag = 1;
                    break;
                }
            case WIFIDIR_NOA_DESCRIPTOR:
                {
                    wifidir_level = WIFIDIR_NOTICE_OF_ABSENCE;
                    flag = 0;
                    break;
                }
            case WIFIDIR_CHANNEL_CONFIG:
                {
                    tlvbuf_wifidir_channel *tlv = NULL;
                    /* Append a new TLV */
                    tlv_len = sizeof(tlvbuf_wifidir_channel);
                    tlv = (tlvbuf_wifidir_channel *) (buffer + cmd_len_wifidir);
                    cmd_len_wifidir += tlv_len;
                    /* Set TLV fields */
                    tlv->tag = TLV_TYPE_WIFIDIR_CHANNEL;
                    tlv->length = tlv_len - (sizeof(u8) + sizeof(u16));
                    memcpy(tlv->country_string, country, 3);
                    if (tlv->country_string[2] == 0)
                        tlv->country_string[2] = WIFIDIR_COUNTRY_LAST_BYTE;
                    tlv->regulatory_class = regulatory_class;
                    tlv->channel_number = channel_number;
                    endian_convert_tlv_wifidir_header_out(tlv);
                    break;
                }
            case WIFIDIR_OPCHANNEL_CONFIG:
                {
                    tlvbuf_wifidir_channel *tlv = NULL;
                    /* Append a new TLV */
                    tlv_len = sizeof(tlvbuf_wifidir_channel);
                    tlv = (tlvbuf_wifidir_channel *) (buffer + cmd_len_wifidir);
                    cmd_len_wifidir += tlv_len;
                    /* Set TLV fields */
                    tlv->tag = TLV_TYPE_WIFIDIR_OPCHANNEL;
                    tlv->length = tlv_len - (sizeof(u8) + sizeof(u16));
                    memcpy(tlv->country_string, country, 3);
                    if (tlv->country_string[2] == 0)
                        tlv->country_string[2] = WIFIDIR_COUNTRY_LAST_BYTE;
                    tlv->regulatory_class = op_regulatory_class;
                    tlv->channel_number = op_channel_number;
                    endian_convert_tlv_wifidir_header_out(tlv);
                    break;
                }
            case WIFIDIR_DEVICE_INFO_CONFIG:
                {
                    tlvbuf_wifidir_device_info *tlv = NULL;
                    wifidir_device_name_info *array_ptr;
                    /* Append a new TLV */
                    tlv_len = sizeof(tlvbuf_wifidir_device_info) +
                        secondary_dev_count * WPS_DEVICE_TYPE_LEN +
                        strlen(wifidir_ssid);
                    tlv = (tlvbuf_wifidir_device_info *) (buffer + cmd_len_wifidir);
                    cmd_len_wifidir += tlv_len;
                    /* Set TLV fields */
                    tlv->tag = TLV_TYPE_WIFIDIR_DEVICE_INFO;
                    tlv->length = tlv_len - (sizeof(u8) + sizeof(u16));
                    memcpy(tlv->dev_address, dev_address, ETH_ALEN);
                    tlv->config_methods = mwu_htons(config_methods);
                    tlv->primary_category = mwu_htons(pri_category);
                    memcpy(tlv->primary_oui, primary_oui, 4);
                    tlv->primary_subcategory = mwu_htons(pri_sub_category);
                    tlv->secondary_dev_count = secondary_dev_count;
                    endian_convert_tlv_wifidir_header_out(tlv);
                    /* Parameters within secondary_dev_info are already
                       mwu_htons'ed */
                    memcpy(tlv->secondary_dev_info, secondary_dev_info,
                           secondary_dev_count * WPS_DEVICE_TYPE_LEN);
                    array_ptr = (wifidir_device_name_info *)
                        ((u8 *)&tlv->secondary_dev_count +
                        sizeof(tlv->secondary_dev_count) +
                        tlv->secondary_dev_count * WPS_DEVICE_TYPE_LEN);
                    temp = mwu_htons(SC_Device_Name);
                    memcpy((u8 *) (&array_ptr->device_name_type), &temp,
                           sizeof(temp));
                    temp = mwu_htons(strlen(wifidir_ssid));
                    memcpy((u8 *) (&array_ptr->device_name_len), &temp,
                           sizeof(temp));
                    memcpy((u8 *) (&array_ptr->device_name), wifidir_ssid,
                           strlen(wifidir_ssid));
                    flag = 1;
                    break;
                }
            case WIFIDIR_DEVICE_SEC_INFO_CONFIG:
                {
                    wifidir_level = WIFIDIR_DEVICE_INFO_CONFIG;
                    flag = 0;
                    break;
                }
            case WIFIDIR_GROUP_INFO_CONFIG:
                {
                    tlvbuf_wifidir_group_info *tlv = NULL;
                    /* Append a new TLV */
                    tlv_offset = wifidir_group_total_ssid_len
                        + wifidir_total_secondary_dev_count * WPS_DEVICE_TYPE_LEN;
                    tlv_len = sizeof(tlvbuf_wifidir_group_info)
                        + wifidir_client_dev_count * sizeof(wifidir_client_dev_info) +
                        tlv_offset;
                    tlv = (tlvbuf_wifidir_group_info *) (buffer + cmd_len_wifidir);
                    cmd_len_wifidir += tlv_len;
                    /* Set TLV fields */
                    tlv->tag = TLV_TYPE_WIFIDIR_GROUP_INFO;
                    tlv->length = tlv_len;
                    memcpy(tlv->wifidir_client_dev_list, wifidir_client_dev_info_list,
                           wifidir_client_dev_count * sizeof(wifidir_client_dev_info) +
                           tlv_offset);
                    /* Parameters within secondary_dev_info are already
                       mwu_htons'ed */
                    mwu_hexdump(DEBUG_WLAN, "Group Info Hexdump:", (u8 *) tlv,
                                tlv_len);
                    flag = 1;
                    break;
                }
            case WIFIDIR_GROUP_SEC_INFO_CONFIG:
                {
                    wifidir_level = WIFIDIR_GROUP_CLIENT_INFO_CONFIG;

                    if (wifidir_client_dev_index && group_secondary_index) {
                        memset(((u8 *) &
                                wifidir_client_dev_info_list[wifidir_client_dev_index -
                                                         1].
                                wifidir_secondary_dev_info + tlv_offset), 0,
                               group_secondary_index * WPS_DEVICE_TYPE_LEN);
                        memcpy(((u8 *) &
                                wifidir_client_dev_info_list[wifidir_client_dev_index -
                                                         1].
                                wifidir_secondary_dev_info + tlv_offset),
                               &group_secondary_dev_info,
                               group_secondary_index * WPS_DEVICE_TYPE_LEN);
                    }
                    tlv_offset = wifidir_group_total_ssid_len
                        + wifidir_total_secondary_dev_count * WPS_DEVICE_TYPE_LEN;
                    flag = 0;
                    break;
                }
            case WIFIDIR_GROUP_CLIENT_INFO_CONFIG:
                {
                    wifidir_level = WIFIDIR_GROUP_INFO_CONFIG;
                    flag = 0;
                    break;
                }
            case WIFIDIR_GROUP_ID_CONFIG:
                {
                    tlvbuf_wifidir_group_id *tlv = NULL;
                    /* Append a new TLV */
                    tlv_len = sizeof(tlvbuf_wifidir_group_id) + strlen(wifidir_ssid);
                    tlv = (tlvbuf_wifidir_group_id *) (buffer + cmd_len_wifidir);
                    cmd_len_wifidir += tlv_len;
                    /* Set TLV fields */
                    tlv->tag = TLV_TYPE_WIFIDIR_GROUP_ID;
                    tlv->length = tlv_len - (sizeof(u8) + sizeof(u16));
                    memcpy(tlv->group_address, dev_address, ETH_ALEN);
                    memcpy(tlv->group_ssid, wifidir_ssid, strlen(wifidir_ssid));
                    endian_convert_tlv_wifidir_header_out(tlv);
                    break;
                }
            case WIFIDIR_GROUP_BSS_ID_CONFIG:
                {
                    tlvbuf_wifidir_group_bss_id *tlv = NULL;
                    /* Append a new TLV */
                    tlv_len = sizeof(tlvbuf_wifidir_group_bss_id);
                    tlv = (tlvbuf_wifidir_group_bss_id *) (buffer + cmd_len_wifidir);
                    cmd_len_wifidir += tlv_len;
                    /* Set TLV fields */
                    tlv->tag = TLV_TYPE_WIFIDIR_GROUP_BSS_ID;
                    tlv->length = tlv_len - (sizeof(u8) + sizeof(u16));
                    memcpy(tlv->group_bssid, dev_address, ETH_ALEN);
                    endian_convert_tlv_wifidir_header_out(tlv);
                    break;
                }
            case WIFIDIR_INTERFACE_CONFIG:
                {
                    tlvbuf_wifidir_interface *tlv = NULL;
                    /* Append a new TLV */
                    tlv_len = sizeof(tlvbuf_wifidir_interface)
                        + iface_count * ETH_ALEN;
                    tlv = (tlvbuf_wifidir_interface *) (buffer + cmd_len_wifidir);
                    cmd_len_wifidir += tlv_len;
                    /* Set TLV fields */
                    tlv->tag = TLV_TYPE_WIFIDIR_INTERFACE;
                    tlv->length = tlv_len - (sizeof(u8) + sizeof(u16));
                    memcpy(tlv->interface_id, dev_address, ETH_ALEN);
                    tlv->interface_count = iface_count;
                    memcpy(tlv->interface_idlist, iface_list,
                           iface_count * ETH_ALEN);
                    endian_convert_tlv_wifidir_header_out(tlv);
                    break;
                }
            case WIFIDIR_TIMEOUT_CONFIG:
                {
                    tlvbuf_wifidir_config_timeout *tlv = NULL;
                    /* Append a new TLV */
                    tlv_len = sizeof(tlvbuf_wifidir_config_timeout);
                    tlv = (tlvbuf_wifidir_config_timeout *) (buffer + cmd_len_wifidir);
                    cmd_len_wifidir += tlv_len;
                    /* Set TLV fields */
                    tlv->tag = TLV_TYPE_WIFIDIR_CONFIG_TIMEOUT;
                    tlv->length = tlv_len - (sizeof(u8) + sizeof(u16));
                    tlv->group_config_timeout = go_config_timeout;
                    tlv->device_config_timeout = client_config_timeout;
                    endian_convert_tlv_wifidir_header_out(tlv);
                    break;
                }
            case WIFIDIR_EXTENDED_TIME_CONFIG:
                {
                    tlvbuf_wifidir_ext_listen_time *tlv = NULL;
                    /* Append a new TLV */
                    tlv_len = sizeof(tlvbuf_wifidir_ext_listen_time);
                    tlv = (tlvbuf_wifidir_ext_listen_time *) (buffer + cmd_len_wifidir);
                    cmd_len_wifidir += tlv_len;
                    /* Set TLV fields */
                    tlv->tag = TLV_TYPE_WIFIDIR_EXTENDED_LISTEN_TIME;
                    tlv->length = tlv_len - (sizeof(u8) + sizeof(u16));
                    tlv->availability_period = wlan_le16_to_cpu(avail_period);
                    tlv->availability_interval =
                        wlan_le16_to_cpu(avail_interval);
                    endian_convert_tlv_wifidir_header_out(tlv);
                    break;
                }
            case WIFIDIR_INTENDED_ADDR_CONFIG:
                {
                    tlvbuf_wifidir_intended_addr *tlv = NULL;
                    /* Append a new TLV */
                    tlv_len = sizeof(tlvbuf_wifidir_intended_addr);
                    tlv = (tlvbuf_wifidir_intended_addr *) (buffer + cmd_len_wifidir);
                    cmd_len_wifidir += tlv_len;
                    /* Set TLV fields */
                    tlv->tag = TLV_TYPE_WIFIDIR_INTENDED_ADDRESS;
                    tlv->length = tlv_len - (sizeof(u8) + sizeof(u16));
                    memcpy(tlv->group_address, dev_address, ETH_ALEN);
                    endian_convert_tlv_wifidir_header_out(tlv);
                    break;
                }
            case WIFIDIR_WPSIE:
                {
                    /* Append TLV for WPSVersion */
                    tlvbuf_wps_ie *tlv = NULL;
                    tlv_len = sizeof(tlvbuf_wps_ie) + sizeof(WPS_version);
                    tlv = (tlvbuf_wps_ie *) (buffer + cmd_len_wifidir +
                                             sizeof(custom_ie) + cmd_len_wps);
                    tlv->tag = SC_Version;
                    tlv->length = tlv_len - 2 * sizeof(u16);
                    *(tlv->data) = WPS_version;
                    endian_convert_tlv_wps_header_out(tlv);
                    cmd_len_wps += tlv_len;

                    /* Append TLV for WPSSetupState */
                    tlv_len = sizeof(tlvbuf_wps_ie) + sizeof(WPS_setupstate);
                    tlv = (tlvbuf_wps_ie *) (buffer + cmd_len_wifidir +
                                             sizeof(custom_ie) + cmd_len_wps);
                    tlv->tag = SC_Simple_Config_State;
                    tlv->length = tlv_len - 2 * sizeof(u16);
                    *(tlv->data) = WPS_setupstate;
                    endian_convert_tlv_wps_header_out(tlv);
                    cmd_len_wps += tlv_len;

                    /* Append TLV for WPSRequestType */
                    tlv_len = sizeof(tlvbuf_wps_ie) + sizeof(WPS_requesttype);
                    tlv = (tlvbuf_wps_ie *) (buffer + cmd_len_wifidir +
                                             sizeof(custom_ie) + cmd_len_wps);
                    tlv->tag = SC_Request_Type;
                    tlv->length = tlv_len - 2 * sizeof(u16);
                    *(tlv->data) = WPS_requesttype;
                    endian_convert_tlv_wps_header_out(tlv);
                    cmd_len_wps += tlv_len;

                    /* Append TLV for WPSResponseType */
                    tlv_len = sizeof(tlvbuf_wps_ie) + sizeof(WPS_responsetype);
                    tlv = (tlvbuf_wps_ie *) (buffer + cmd_len_wifidir +
                                             sizeof(custom_ie) + cmd_len_wps);
                    tlv->tag = SC_Response_Type;
                    tlv->length = tlv_len - 2 * sizeof(u16);
                    *(tlv->data) = WPS_responsetype;
                    endian_convert_tlv_wps_header_out(tlv);
                    cmd_len_wps += tlv_len;

                    /* Append TLV for WPSSpecConfigMethods */
                    tlv_len =
                        sizeof(tlvbuf_wps_ie) + sizeof(WPS_specconfigmethods);
                    tlv =
                        (tlvbuf_wps_ie *) (buffer + cmd_len_wifidir +
                                           sizeof(custom_ie) + cmd_len_wps);
                    tlv->tag = SC_Config_Methods;
                    tlv->length = tlv_len - 2 * sizeof(u16);
                    temp = mwu_htons(WPS_specconfigmethods);
                    memcpy((u16 *) tlv->data, &temp, sizeof(temp));
                    endian_convert_tlv_wps_header_out(tlv);
                    cmd_len_wps += tlv_len;

                    /* Append TLV for WPSUUID */
                    tlv_len = sizeof(tlvbuf_wps_ie) + sizeof(WPS_UUID);
                    tlv = (tlvbuf_wps_ie *) (buffer + cmd_len_wifidir +
                                             sizeof(custom_ie) + cmd_len_wps);
                    tlv->tag = SC_UUID_E;
                    tlv->length = tlv_len - 2 * sizeof(u16);
                    memcpy(tlv->data, WPS_UUID, WPS_UUID_MAX_LEN);
                    endian_convert_tlv_wps_header_out(tlv);
                    cmd_len_wps += tlv_len;

                    /* Append TLV for WPSPrimaryDeviceType */
                    tlv_len =
                        sizeof(tlvbuf_wps_ie) + sizeof(WPS_primarydevicetype);
                    tlv =
                        (tlvbuf_wps_ie *) (buffer + cmd_len_wifidir +
                                           sizeof(custom_ie) + cmd_len_wps);
                    tlv->tag = SC_Primary_Device_Type;
                    tlv->length = tlv_len - 2 * sizeof(u16);
                    memcpy(tlv->data, WPS_primarydevicetype,
                           WPS_DEVICE_TYPE_MAX_LEN);
                    endian_convert_tlv_wps_header_out(tlv);
                    cmd_len_wps += tlv_len;

                    /* Append TLV for WPSRFBand */
                    tlv_len = sizeof(tlvbuf_wps_ie) + sizeof(WPS_RFband);
                    tlv = (tlvbuf_wps_ie *) (buffer + cmd_len_wifidir +
                                             sizeof(custom_ie) + cmd_len_wps);
                    tlv->tag = SC_RF_Band;
                    tlv->length = tlv_len - 2 * sizeof(u16);
                    *(tlv->data) = WPS_RFband;
                    endian_convert_tlv_wps_header_out(tlv);
                    cmd_len_wps += tlv_len;

                    /* Append TLV for WPSAssociationState */
                    tlv_len =
                        sizeof(tlvbuf_wps_ie) + sizeof(WPS_associationstate);
                    tlv =
                        (tlvbuf_wps_ie *) (buffer + cmd_len_wifidir +
                                           sizeof(custom_ie) + cmd_len_wps);
                    tlv->tag = SC_Association_State;
                    tlv->length = tlv_len - 2 * sizeof(u16);
                    temp = mwu_htons(WPS_associationstate);
                    memcpy((u16 *) tlv->data, &temp, sizeof(temp));
                    endian_convert_tlv_wps_header_out(tlv);
                    cmd_len_wps += tlv_len;

                    /* Append TLV for WPSConfigurationError */
                    tlv_len =
                        sizeof(tlvbuf_wps_ie) + sizeof(WPS_configurationerror);
                    tlv =
                        (tlvbuf_wps_ie *) (buffer + cmd_len_wifidir +
                                           sizeof(custom_ie) + cmd_len_wps);
                    tlv->tag = SC_Configuration_Error;
                    tlv->length = tlv_len - 2 * sizeof(u16);
                    temp = mwu_htons(WPS_configurationerror);
                    memcpy((u16 *) tlv->data, &temp, sizeof(temp));
                    endian_convert_tlv_wps_header_out(tlv);
                    cmd_len_wps += tlv_len;

                    /* Append TLV for WPSDevicePassword */
                    tlv_len =
                        sizeof(tlvbuf_wps_ie) + sizeof(WPS_devicepassword);
                    tlv =
                        (tlvbuf_wps_ie *) (buffer + cmd_len_wifidir +
                                           sizeof(custom_ie) + cmd_len_wps);
                    tlv->tag = SC_Device_Password_ID;
                    tlv->length = tlv_len - 2 * sizeof(u16);
                    temp = mwu_htons(WPS_devicepassword);
                    memcpy((u16 *) tlv->data, &temp, sizeof(temp));
                    endian_convert_tlv_wps_header_out(tlv);
                    cmd_len_wps += tlv_len;

                    /* Append TLV for WPSDeviceName */
                    tlv_len = sizeof(tlvbuf_wps_ie) + strlen(WPS_devicename);
                    tlv = (tlvbuf_wps_ie *) (buffer + cmd_len_wifidir +
                                             sizeof(custom_ie) + cmd_len_wps);
                    tlv->tag = SC_Device_Name;
                    tlv->length = tlv_len - 2 * sizeof(u16);
                    memcpy(tlv->data, WPS_devicename, strlen(WPS_devicename));
                    endian_convert_tlv_wps_header_out(tlv);
                    cmd_len_wps += tlv_len;
                    /*
                     * Update device_name of enrollee and registrar.
                     */
                    strcpy((char *) gpwps_info->enrollee.device_name, WPS_devicename);
                    strcpy((char *) gpwps_info->registrar.device_name, WPS_devicename);
                    gpwps_info->enrollee.device_name_length =
                        strlen((char *) gpwps_info->enrollee.device_name);
                    gpwps_info->registrar.device_name_length =
                        strlen((char *) gpwps_info->registrar.device_name);

                    /* Append TLV for WPSManufacturer */
                    tlv_len = sizeof(tlvbuf_wps_ie) + strlen(WPS_manufacturer);
                    tlv = (tlvbuf_wps_ie *) (buffer + cmd_len_wifidir +
                                             sizeof(custom_ie) + cmd_len_wps);
                    tlv->tag = SC_Manufacturer;
                    tlv->length = tlv_len - 2 * sizeof(u16);
                    memcpy(tlv->data, WPS_manufacturer,
                           strlen(WPS_manufacturer));
                    endian_convert_tlv_wps_header_out(tlv);
                    cmd_len_wps += tlv_len;

                    /* Append TLV for WPSModelName */
                    tlv_len = sizeof(tlvbuf_wps_ie) + strlen(WPS_modelname);
                    tlv = (tlvbuf_wps_ie *) (buffer + cmd_len_wifidir +
                                             sizeof(custom_ie) + cmd_len_wps);
                    tlv->tag = SC_Model_Name;
                    tlv->length = tlv_len - 2 * sizeof(u16);
                    memcpy(tlv->data, WPS_modelname, strlen(WPS_modelname));
                    endian_convert_tlv_wps_header_out(tlv);
                    cmd_len_wps += tlv_len;

                    /* Append TLV for WPSModelNumber */
                    tlv_len = sizeof(tlvbuf_wps_ie) + wps_model_len;
                    tlv = (tlvbuf_wps_ie *) (buffer + cmd_len_wifidir +
                                             sizeof(custom_ie) + cmd_len_wps);
                    tlv->tag = SC_Model_Number;
                    tlv->length = tlv_len - 2 * sizeof(u16);
                    memcpy(tlv->data, WPS_modelnumber, wps_model_len);
                    endian_convert_tlv_wps_header_out(tlv);
                    cmd_len_wps += tlv_len;

                    /* Append TLV for WPSSerialNumber */
                    tlv_len = sizeof(tlvbuf_wps_ie) + wps_serial_len;
                    tlv = (tlvbuf_wps_ie *) (buffer + cmd_len_wifidir +
                                             sizeof(custom_ie) + cmd_len_wps);
                    tlv->tag = SC_Serial_Number;
                    tlv->length = tlv_len - 2 * sizeof(u16);
                    memcpy(tlv->data, WPS_serialnumber, wps_serial_len);
                    endian_convert_tlv_wps_header_out(tlv);
                    cmd_len_wps += tlv_len;
                    break;
                }
            case WIFIDIR_EXTRA:
                {
                    memcpy(buffer + cmd_len_wifidir, extra, extra_len);
                    cmd_len_wifidir += extra_len;
                    break;
                }
            default:
                *ie_len_wifidir = cmd_len_wifidir;
                if (ie_len_wps)
                    *ie_len_wps = cmd_len_wps;
                break;
            }
            memset(country, 0, sizeof(country));
            if (wifidir_level == 0)
                cmd_found = 0;
            if (flag)
                wifidir_level = 0;
        }
    }

  done:
    fclose(config_file);
    if (chan_entry_list)
        free(chan_entry_list);
    if (line)
        free(line);
    return;
}

/**
 *  @brief Creates a wifidir_config request and sends to the driver
 *
 *  Usage: "Usage : wfd_config <CONFIG_FILE>"
 *
 *  @param filename Config file Name
 *  @param cfg      Wifidirect config structure
 *  @return         WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
static int
wifidir_config_download(char *filename, struct wifidir_cfg *cfg)
{
    int ret = WPS_STATUS_FAIL;
    u8 *buf = NULL;
    u16 ie_len_wifidir = 0, ie_len_wps = 0, ie_len;
    u16 ie_index = 0;

    ENTER();
    buf = (u8 *) malloc(MRVDRV_SIZE_OF_CMD_BUFFER);
    if (!buf) {
        printf("ERR:Cannot allocate memory!\n");
        return WPS_STATUS_FAIL;
    }
    memset(buf, 0, MRVDRV_SIZE_OF_CMD_BUFFER);

    /* Read parameters and send command to firmware */
    wifidir_file_params_config(filename, cfg, buf
                           + sizeof(tlvbuf_custom_ie) + sizeof(custom_ie),
                           &ie_len_wifidir, &ie_len_wps);
    if (ie_len_wifidir > MAX_SIZE_IE_BUFFER || ie_len_wps > MAX_SIZE_IE_BUFFER) {
        printf("ERR:IE parameter size exceeds limit in %s.\n", filename);
        free(buf);
        return WPS_STATUS_FAIL;
    }
    ie_len =
        ie_len_wifidir + ie_len_wps + sizeof(tlvbuf_custom_ie) +
        (2 * sizeof(custom_ie));
    if (ie_len >= MRVDRV_SIZE_OF_CMD_BUFFER) {
        printf("ERR:Too much data in configuration file %s.\n", filename);
        free(buf);
        return WPS_STATUS_FAIL;
    }

    mwu_hexdump(DEBUG_WLAN, "Config Download", buf, ie_len);

    ret = wifidir_ie_config(&ie_index, ie_len_wifidir, ie_len_wps, buf);
    if (ret != WPS_STATUS_SUCCESS) {
        printf("ERR:Could not set wifidir parameters\n");
    }

    wifidir_ie_index_base = ie_index;

    mwu_printf(DEBUG_WLAN, "Result of config download %d.\n", ret);

    free(buf);
    LEAVE();
    return ret;
}

/**
 *  @brief Creates a wifidir_params_config request and sends to the driver
 *
 *  Usage: "Usage : wfd_params_config <CONFIG_FILE>"
 *
 *  @param filename Config file Name
 *  @return         WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
static int
wifidir_params_config_download(char *filename)
{
    int ret = WPS_STATUS_FAIL;
    wifidir_params_config *param_buf = NULL;
    tlvbuf_wps_ie *new_tlv = NULL;
    char *line = NULL, wifidir_mac[20];
    FILE *config_file = NULL;
    int i, li = 0, arg_num = 0;
    char *args[30], *pos = NULL;
    u8 dev_address[ETH_ALEN], enable_scan;
    u8 *buffer = NULL, WPS_primarydevicetype[WPS_DEVICE_TYPE_MAX_LEN];
    u16 device_state = 0, tlv_len = 0, max_disc_int, min_disc_int, cmd_len = 0;
    mrvl_cmd_head_buf *cmd = NULL;

    ENTER();
    cmd_len = sizeof(mrvl_cmd_head_buf) + sizeof(wifidir_params_config);
    buffer = (u8 *) malloc(MRVDRV_SIZE_OF_CMD_BUFFER);
    if (!buffer) {
        printf("ERR:Cannot allocate memory!\n");
        return WPS_STATUS_FAIL;
    }
    memset(buffer, 0, MRVDRV_SIZE_OF_CMD_BUFFER);
    cmd = (mrvl_cmd_head_buf *)buffer;
    param_buf = (wifidir_params_config *)cmd->cmd_data;
    cmd->cmd_code = HostCmd_CMD_WIFIDIR_PARAMS_CONFIG;
    cmd->seq_num = 0;
    cmd->result = 0;

    /* Check if file exists */
    config_file = fopen(filename, "r");
    if (config_file == NULL) {
        printf("\nERR:Config file can not be opened.\n");
        goto done;
    }
    line = (char *) malloc(MAX_CONFIG_LINE);
    if (!line) {
        printf("ERR:Cannot allocate memory for line\n");
        goto done;
    }
    memset(line, 0, MAX_CONFIG_LINE);
    param_buf->action = wlan_cpu_to_le16(ACTION_SET);

    /* Parse file and process */
    while (config_get_line(line, MAX_CONFIG_LINE, config_file, &li, &pos)) {
        arg_num = parse_line(line, args);

        if (strcmp(args[0], "EnableScan") == 0) {
            if (is_input_valid(WIFIDIR_ENABLE_SCAN, arg_num - 1, args + 1)
                != SUCCESS) {
                goto done;
            }
            enable_scan = (u8) atoi(args[1]);
            /* Append a new TLV */
            tlv_len = sizeof(tlvbuf_wps_ie) + sizeof(u8);
            new_tlv = (tlvbuf_wps_ie *) (buffer + cmd_len);
            cmd_len += tlv_len;
            /* Set TLV fields */
            new_tlv->tag = MRVL_WIFIDIR_SCAN_ENABLE_TLV_ID;
            new_tlv->length = tlv_len - 2 * sizeof(u16);
            memcpy(new_tlv->data, &enable_scan, sizeof(u8));
            endian_convert_tlv_header_out(new_tlv);

        } else if (strcmp(args[0], "ScanPeerDeviceId") == 0) {
            strncpy(wifidir_mac, args[1], 20);
            if ((ret = mac2raw(wifidir_mac, dev_address)) != SUCCESS) {
                printf("ERR: %s Address \n",
                       ret == WPS_STATUS_FAIL ? "Invalid MAC" : ret ==
                       WIFIDIR_RET_MAC_BROADCAST ? "Broadcast" : "Multicast");
                goto done;
            }
            /* Append a new TLV */
            tlv_len = sizeof(tlvbuf_wps_ie) + ETH_ALEN;
            new_tlv = (tlvbuf_wps_ie *) (buffer + cmd_len);
            cmd_len += tlv_len;
            /* Set TLV fields */
            new_tlv->tag = MRVL_WIFIDIR_PEER_DEVICE_TLV_ID;
            new_tlv->length = tlv_len - 2 * sizeof(u16);
            memcpy(new_tlv->data, dev_address, ETH_ALEN);
            endian_convert_tlv_header_out(new_tlv);

        } else if (strcmp(args[0], "MinDiscoveryInterval") == 0) {
            if (is_input_valid(WIFIDIR_MINDISCOVERYINT, arg_num - 1, args + 1) !=
                SUCCESS) {
                goto done;
            }
            min_disc_int = wlan_cpu_to_le16(atoi(args[1]));
        } else if (strcmp(args[0], "MaxDiscoveryInterval") == 0) {
            if (is_input_valid(WIFIDIR_MAXDISCOVERYINT, arg_num - 1, args + 1) !=
                SUCCESS) {
                goto done;
            }
            max_disc_int = wlan_cpu_to_le16(atoi(args[1]));

            /* Append a new TLV */
            tlv_len = sizeof(tlvbuf_wps_ie) + 2 * sizeof(u16);
            new_tlv = (tlvbuf_wps_ie *) (buffer + cmd_len);
            cmd_len += tlv_len;
            /* Set TLV fields */
            new_tlv->tag = MRVL_WIFIDIR_DISC_PERIOD_TLV_ID;
            new_tlv->length = tlv_len - 2 * sizeof(u16);
            memcpy(&new_tlv->data, &min_disc_int, sizeof(u16));
            memcpy((((u8 *) & new_tlv->data) + sizeof(u16)),
                   &max_disc_int, sizeof(u16));
            endian_convert_tlv_header_out(new_tlv);

        } else if (strcmp(args[0], "ScanRequestDeviceType") == 0) {
            if (is_input_valid(WIFIDIR_WPSPRIMARYDEVICETYPE, arg_num - 1, args + 1)
                != SUCCESS) {
                goto done;
            }
            for (i = 0; i < WPS_DEVICE_TYPE_MAX_LEN; i++)
                WPS_primarydevicetype[i] = (u8) A2HEXDECIMAL(args[i + 1]);

            /* Append a new TLV */
            tlv_len = sizeof(tlvbuf_wps_ie) + WPS_DEVICE_TYPE_MAX_LEN;
            new_tlv = (tlvbuf_wps_ie *) (buffer + cmd_len);
            cmd_len += tlv_len;
            /* Set TLV fields */
            new_tlv->tag = MRVL_WIFIDIR_SCAN_REQ_DEVICE_TLV_ID;
            new_tlv->length = tlv_len - 2 * sizeof(u16);
            memcpy(&new_tlv->data, WPS_primarydevicetype,
                   WPS_DEVICE_TYPE_MAX_LEN);
            endian_convert_tlv_header_out(new_tlv);

        } else if (strcmp(args[0], "DeviceState") == 0) {
            if (is_input_valid(WIFIDIR_DEVICE_STATE, arg_num - 1, args + 1)
                != SUCCESS) {
                goto done;
            }
            device_state = wlan_cpu_to_le16((u16) atoi(args[1]));

            /* Append a new TLV */
            tlv_len = sizeof(tlvbuf_wps_ie) + sizeof(u16);
            new_tlv = (tlvbuf_wps_ie *) (buffer + cmd_len);
            cmd_len += tlv_len;
            /* Set TLV fields */
            new_tlv->tag = MRVL_WIFIDIR_DEVICE_STATE_TLV_ID;
            new_tlv->length = tlv_len - 2 * sizeof(u16);
            memcpy(new_tlv->data, &device_state, sizeof(u16));
            endian_convert_tlv_header_out(new_tlv);
        }
    }
    cmd->size = cmd_len;

    mwu_hexdump(DEBUG_WLAN, "Parameters Download", buffer, cmd_len);

    /* Send collective command */
    ret = wifidir_ioctl(wifidir_mod->iface, (u8 *) buffer, &cmd_len, cmd_len);
    mwu_printf(DEBUG_WLAN, "Result of parameter download %d.\n", ret);

  done:
    if (config_file)
        fclose(config_file);
    if (line)
        free(line);
    if (buffer)
        free(buffer);
    LEAVE();
    return ret;
}

/**
 *  @brief Creates a wifidir_config, wifidir_params_config
 *          request and sends to the driver.
 *
 *         ./wfdutl mlan0 wfd_config wifidir.conf
 *         ./wfdutl mlan0 wfd_params_config wifidir.conf
 *
 *  @param filename Config file Name
 *  @param cfg      Wifidirect config structure
 *  @return         WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
int
wifidircmd_config_download(char *filename, struct wifidir_cfg *cfg)
{
    if (wifidir_config_download(filename, cfg) != WPS_STATUS_SUCCESS) {
        printf("WIFIDIR configuration download failed.\n");
        return WPS_STATUS_FAIL;
    }
    if (wifidir_params_config_download(filename) != WPS_STATUS_SUCCESS) {
        printf("WIFIDIR parameter configuration failed.\n");
        return WPS_STATUS_FAIL;
    }

    return WPS_STATUS_SUCCESS;
}

/*
 *  @brief       Read/Write WIFIDIR Persistent group database in FW
 *  @action      ACTION_GET or ACTION_SET
 *  @param prec  Pointer to wifidir_persistent_record structure
 *  @return      WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
int
wifidir_cfg_cmd_persistent_group_record(int action, wifidir_persistent_record * prec)
{
    mrvl_cmd_head_buf *cmd = NULL;
    wifidir_params_config *cmd_buf = NULL;
    tlvbuf_wifidir_persistent_group *tlv = NULL;
    u8 index = 0, var_len = 0;
    u8 *buffer = NULL;
    u16 cmd_len = 0, buf_len = 0;
    int pi;

    /* error checks */
    if (prec->index >= WIFIDIR_PERSISTENT_GROUP_MAX) {
        mwu_printf(MSG_ERROR, "ERR:wrong index. Value values are [0-3]\n");
        return WPS_STATUS_FAIL;
    }

    cmd_len = sizeof(mrvl_cmd_head_buf) + sizeof(wifidir_params_config);
    buffer = (u8 *) malloc(MRVDRV_SIZE_OF_CMD_BUFFER);
    if (!buffer) {
        mwu_printf(MSG_ERROR, "ERR:Cannot allocate memory!\n");
        return WPS_STATUS_FAIL;
    }

    memset(buffer, 0, MRVDRV_SIZE_OF_CMD_BUFFER);
    cmd = (mrvl_cmd_head_buf *)buffer;
    cmd_buf = (wifidir_params_config *) cmd->cmd_data;
    cmd->cmd_code = HostCmd_CMD_WIFIDIR_PARAMS_CONFIG;
    cmd->seq_num = 0;
    cmd->result = 0;
    tlv = (tlvbuf_wifidir_persistent_group *)cmd_buf->wifidir_tlv;

    tlv->tag = MRVL_WIFIDIR_PERSISTENT_GROUP_TLV_ID;

    if (action == ACTION_GET) {
        cmd_buf->action = ACTION_GET;
        tlv->rec_index = prec->index;
        tlv->length = sizeof(prec->index);
        cmd_len += TLV_HEADER_SIZE + tlv->length;
        cmd->size = cmd_len - BUF_HEADER_SIZE;
        buf_len = MRVDRV_SIZE_OF_CMD_BUFFER;
        goto send_cmd;
    }

    cmd_buf->action = ACTION_SET;
    tlv->rec_index = prec->index;
    tlv->dev_role = prec->dev_role;
    memcpy(tlv->group_bssid, prec->bssid, ETH_ALEN);
    memcpy(tlv->dev_mac_address, prec->groupdevid, ETH_ALEN);

    /* ssid */
    tlv->group_ssid_len = prec->ssid_len;
    var_len += tlv->group_ssid_len;
    memcpy(tlv->group_ssid, prec->ssid, tlv->group_ssid_len);

    /* adjust pointer from here */
    /* psk */
    *(&tlv->psk_len + var_len) = prec->psk_len;
    memcpy(tlv->psk + var_len, prec->psk, prec->psk_len);
    var_len += prec->psk_len;

    tlv->length = sizeof(tlvbuf_wifidir_persistent_group) -
        TLV_HEADER_SIZE + var_len;

    *(&tlv->num_peers + var_len) = prec->num_peers;

    for (pi = 0; pi < prec->num_peers; pi++) {
        memcpy(tlv->peer_mac_addrs[pi] + var_len, prec->peers[pi], ETH_ALEN);
    }

    tlv->length += prec->num_peers * ETH_ALEN;

    cmd_len += TLV_HEADER_SIZE + tlv->length;
    buf_len = cmd_len;
    cmd->size = cmd_len - BUF_HEADER_SIZE;

  send_cmd:
    endian_convert_tlv_header_out(tlv);

    /* Send collective command */
    wifidir_ioctl(wifidir_mod->iface, (u8 *) buffer, &cmd_len, buf_len);

    endian_convert_tlv_header_in(tlv);

    if (action == ACTION_GET) {
        if (tlv->length > (sizeof(tlvbuf_wifidir_persistent_group) - sizeof(u8)))
            printf("Persistent group information =>\n");
        else
            mwu_printf(DEBUG_WIFIDIR_DISCOVERY,
                       "Persistent group information is empty.\n");
        buf_len = tlv->length;
        while (buf_len > (sizeof(tlvbuf_wifidir_persistent_group) - sizeof(u8))) {
            printf("\n\t Index = [%d]\n", tlv->rec_index);
            prec->index = tlv->rec_index;
            printf("\n\t Dev_Role = [%d]\n", tlv->dev_role);
            printf("\t Role  = %s\n",
                   (tlv->dev_role ==
                    EV_SUBTYPE_WIFIDIR_GO_ROLE) ? "Group owner" : "Client");
            prec->dev_role = tlv->dev_role;
            printf("\t GroupBssId - ");
            print_mac(tlv->group_bssid);
            memcpy(prec->bssid, tlv->group_bssid, ETH_ALEN);
            printf("\n\t DeviceId - ");
            print_mac(tlv->dev_mac_address);
            memcpy(prec->groupdevid, tlv->dev_mac_address, ETH_ALEN);
            printf("\n\t SSID = ");
            for (index = 0; index < tlv->group_ssid_len; index++)
                printf("%c", tlv->group_ssid[index]);

            prec->ssid_len = tlv->group_ssid_len;
            memcpy(prec->ssid, tlv->group_ssid, tlv->group_ssid_len);

            var_len = tlv->group_ssid_len;
            printf("\n\t PSK = ");
            for (index = 0; index < *(&tlv->psk_len + var_len); index++) {
                if (index == 16)
                    printf("\n\t       ");
                printf("%02x ", *(&tlv->psk[index] + var_len));
            }

            prec->psk_len = *(&tlv->psk_len + var_len);
            memcpy(prec->psk, tlv->psk + var_len, prec->psk_len);

            var_len += *(&tlv->psk_len + var_len);
            if (tlv->dev_role == EV_SUBTYPE_WIFIDIR_GO_ROLE) {
                prec->num_peers = *(&tlv->num_peers + var_len);
                for (pi = 0; pi < prec->num_peers; pi++) {
                    memcpy(prec->peers[pi],
                           tlv->peer_mac_addrs[pi] + var_len, ETH_ALEN);
                    printf("\n\t Peer Mac address(%d) = ", pi);
                    print_mac(prec->peers[pi]);
                }
            }
            if (action == ACTION_GET)
                printf("\n\t -----------------------------------------");
            if (tlv->dev_role == EV_SUBTYPE_WIFIDIR_GO_ROLE) {
                buf_len -= sizeof(tlvbuf_wifidir_persistent_group) -
                    TLV_HEADER_SIZE + var_len;
                tlv = (tlvbuf_wifidir_persistent_group *) (((u8 *) (tlv)) +
                                                       (sizeof
                                                        (tlvbuf_wifidir_persistent_group)
                                                        - TLV_HEADER_SIZE +
                                                        var_len));
            } else {
                /* num_peer field willnt be present */
                buf_len -= sizeof(tlvbuf_wifidir_persistent_group) -
                    TLV_HEADER_SIZE - sizeof(u8) + var_len;
                tlv = (tlvbuf_wifidir_persistent_group *) (((u8 *) (tlv)) +
                                                       (sizeof
                                                        (tlvbuf_wifidir_persistent_group)
                                                        - TLV_HEADER_SIZE -
                                                        sizeof(u8) + var_len));
            }
        }
        printf("\n");
    } else {
        mwu_printf(DEBUG_WIFIDIR_DISCOVERY,
                   "Setting persistent group information successful!\n");
    }

    if (buffer)
        free(buffer);

    return WPS_STATUS_SUCCESS;
}

/*
 *  @brief Return index of persistent group record matching the given SSID
 *  @param ssid_length Length of ssid to lookup
 *  @param prec        ssid to lookup
 *  @param pr_index    Index of matching record
 *  @return      WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */

int
wifidir_get_persistent_rec_ind_by_ssid(int ssid_length, u8 * ssid, s8 * pr_index)
{
    int index, ret = WPS_STATUS_SUCCESS;
    wifidir_persistent_record prec;

    for (index = 0; index < WIFIDIR_PERSISTENT_GROUP_MAX; index++) {
        memset(&prec, 0, sizeof(wifidir_persistent_record));

        prec.index = index;
        if ((ret =
             wifidir_cfg_cmd_persistent_group_record(ACTION_GET,
                                                 &prec)) !=
            WPS_STATUS_SUCCESS) {
            goto done;
        }

        if ((prec.ssid_len == ssid_length) &&
            !memcmp(prec.ssid, ssid, ssid_length)) {
            /* correct record found */
            *pr_index = index;
            goto done;
        }
    }

    /* record not found */
    ret = WPS_STATUS_FAIL;

  done:
    return ret;
}

/*
 *  @brief Return index of a free persistent group record
 *  @param pr_index    Index of matching record
 *  @return      WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
int
wifidir_get_free_persistent_rec_index(s8 * pr_index)
{
    int index, ret = WPS_STATUS_SUCCESS;
    wifidir_persistent_record prec;

    for (index = 0; index < WIFIDIR_PERSISTENT_GROUP_MAX; index++) {
        memset(&prec, 0, sizeof(wifidir_persistent_record));

        prec.index = index;
        if ((ret =
             wifidir_cfg_cmd_persistent_group_record(ACTION_GET,
                                                 &prec)) !=
            WPS_STATUS_SUCCESS) {
            goto done;
        }

        if (prec.ssid_len == 0) {
            /* free record found */
            *pr_index = index;
            goto done;
        }
    }

    /* record not found */
    ret = WPS_STATUS_FAIL;

  done:
    return ret;
}

/*
 *  @brief    Update a peristent record based on current info
 *  @param pwps_info   A pointer to the PWPS_INFO structure
 *  @return            WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */

int
wifidir_update_persistent_record(PWPS_INFO pwps_info)
{
    int ret = WPS_STATUS_SUCCESS;
    int registrar = (IS_DISCOVERY_REGISTRAR(pwps_info)) ? 1 : 0;
    wifidir_persistent_record prec;
    CREDENTIAL_DATA *reg_cred = (CREDENTIAL_DATA *)
        & pwps_info->registrar.cred_data[0];

//XXX: Check disabled for now
#if 0
    if (!(pwps_info->self_p2p_grp_cap & WIFIDIR_GROUP_CAP_PERSIST_GROUP_BIT)) {
        /* Persistent group not enabled */
        return WPS_STATUS_FAIL;
    }
#endif

    memset(&prec, 0, sizeof(wifidir_persistent_record));

    /* Pick the correct record */
    if ((ret = wifidir_get_persistent_rec_ind_by_ssid(reg_cred->ssid_length,
                                                  reg_cred->ssid,
                                                  &prec.index)) !=
        WPS_STATUS_SUCCESS) {
        /* FW didnt create a Persistent record entry */
        mwu_printf(DEBUG_WIFIDIR_DISCOVERY,
                   "No persistent rec with req ssid found\n");
        prec.index = -1;
        if (pwps_info->discovery_role != WPS_ENROLLEE) {
            /** This is wrong - can occur only when
             *  enrollee connects to existing GO
             **/
            return ret;
        }
    }

    if (prec.index != -1) {
        /* First, read the record from FW */
        mwu_printf(DEBUG_WIFIDIR_DISCOVERY, "Reading Rec from FW\n");
        wifidir_cfg_cmd_persistent_group_record(ACTION_GET, &prec);
        mwu_hexdump(MSG_INFO, "Prec:", (u8 *) & prec, sizeof(prec));
    } else {
        if (wifidir_get_free_persistent_rec_index(&prec.index) == WPS_STATUS_FAIL) {
            /* No free persistent record entry found */
            return ret;
        }
        mwu_printf(DEBUG_WIFIDIR_DISCOVERY,
                   "Creating new Persistent record @ index: %d\n", prec.index);

        prec.dev_role = EV_SUBTYPE_WIFIDIR_CLIENT_ROLE;
        memcpy(prec.groupdevid, pwps_info->registrar.mac_address, ETH_ALEN);
        prec.ssid_len = reg_cred->ssid_length;
        memcpy(prec.ssid, reg_cred->ssid, reg_cred->ssid_length);
    }

    if (!registrar) {
        /* For Enrollee, just update the PSK info */
        if (reg_cred->network_key_len == 64) {
            prec.psk_len = reg_cred->network_key_len / 2;
            hexstr2bin((s8 *) reg_cred->network_key,
                       prec.psk, reg_cred->network_key_len);
        } else {
            prec.psk_len = reg_cred->network_key_len;
            memcpy(prec.psk, reg_cred->network_key, prec.psk_len);
        }
    } else {
        /* For Registrar, update the Client Mac Address */
        prec.num_peers = 1;
        memcpy(prec.peers[0], pwps_info->enrollee.mac_address, ETH_ALEN);
    }

    mwu_printf(DEBUG_WIFIDIR_DISCOVERY, "Update Rec to FW\n");
    /* Write back to the FW */
    wifidir_cfg_cmd_persistent_group_record(ACTION_SET, &prec);

    return ret;
}
