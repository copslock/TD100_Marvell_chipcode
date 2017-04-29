/** @file wps_wlan.c
 *  @brief This file contains functions for WLAN driver control/command.
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

#include    <stdio.h>
#include    <ctype.h>
#include    <sys/types.h>
#include    <sys/socket.h>
#include    <string.h>
#include    <stdlib.h>
#include    <linux/if.h>
#include    <sys/ioctl.h>
#include    <linux/if_ether.h>
#include    <net/if_arp.h>      /* For ARPHRD_ETHER */
#include    <errno.h>
#include    <linux/wireless.h>
#include    <arpa/inet.h>

#include    "wps_msg.h"
#include    "wps_def.h"
#include    "wps_wlan.h"
#include    "wlan_hostcmd.h"
#include    "mwu_log.h"
#include    "util.h"
#include    "wlan_wifidir.h"
#include    "wifidir_sm.h"
#include    "mwu_timer.h"
#include    "mwu_ioctl.h"
#ifdef ANDROID
#undef perror
#define perror(str) mwu_printf(MSG_ERROR, str ": %s\n", strerror(errno))
#endif

/** Chan-Freq mapping table */
typedef struct _CHANNEL_FREQ_ENTRY
{
    /** Channel Number */
    int Channel;
    /** Frequency of this Channel */
    int Freq;
} CHANNEL_FREQ_ENTRY;

/** Initial number of total private ioctl calls */
#define IW_INIT_PRIV_NUM    128
/** Maximum number of total private ioctl calls supported */
#define IW_MAX_PRIV_NUM     1024
/** MRV Event point off*/
#define MRV_EV_POINT_OFF (((char *) &(((struct iw_point *) NULL)->length)) - \
              (char *) NULL)

/** Convert character to integer */
#define CHAR2INT(x) (((x) >= 'A') ? ((x) - 'A' + 10) : ((x) - '0'))

#define MAX_WEP_KEY_LEN 27

/********************************************************
        Local Variables
********************************************************/
/** Channels for 802.11b/g */
static CHANNEL_FREQ_ENTRY channel_freq_UN_BG[] = {
    {1, 2412},
    {2, 2417},
    {3, 2422},
    {4, 2427},
    {5, 2432},
    {6, 2437},
    {7, 2442},
    {8, 2447},
    {9, 2452},
    {10, 2457},
    {11, 2462},
    {12, 2467},
    {13, 2472},
    {14, 2484}
};

/** Channels for 802.11a/j */
static CHANNEL_FREQ_ENTRY channel_freq_UN_AJ[] = {
    {8, 5040},
    {12, 5060},
    {16, 5080},
    {34, 5170},
    {38, 5190},
    {42, 5210},
    {46, 5230},
    {36, 5180},
    {40, 5200},
    {44, 5220},
    {48, 5240},
    {52, 5260},
    {56, 5280},
    {60, 5300},
    {64, 5320},
    {100, 5500},
    {104, 5520},
    {108, 5540},
    {112, 5560},
    {116, 5580},
    {120, 5600},
    {124, 5620},
    {128, 5640},
    {132, 5660},
    {136, 5680},
    {140, 5700},
    {149, 5745},
    {153, 5765},
    {157, 5785},
    {161, 5805},
    {165, 5825},
/*  {240, 4920},
    {244, 4940},
    {248, 4960},
    {252, 4980},
channels for 11J JP 10M channel gap */
};

static s32 ioctl_sock;
static s8 dev_name[IFNAMSIZ + 1];
static struct iw_priv_args *Priv_args = NULL;
static int we_version_compiled = 0;

/********************************************************
        Global Variables
********************************************************/
/** Global pwps information */
extern PWPS_INFO gpwps_info;
/** IE index*/
extern short ie_index;

/********************************************************
        Local Functions
********************************************************/
/**
 *  @brief convert char to hex integer
 *
 *  @param chr      char to convert
 *  @return         hex integer or 0
 */
int
hexval(s32 chr)
{
    if (chr >= '0' && chr <= '9')
        return chr - '0';
    if (chr >= 'A' && chr <= 'F')
        return chr - 'A' + 10;
    if (chr >= 'a' && chr <= 'f')
        return chr - 'a' + 10;

    return 0;
}

/**
 *  @brief convert char to hex integer
 *
 *  @param chr      char
 *  @return         hex integer
 */
char
hexc2bin(char chr)
{
    if (chr >= '0' && chr <= '9')
        chr -= '0';
    else if (chr >= 'A' && chr <= 'F')
        chr -= ('A' - 10);
    else if (chr >= 'a' && chr <= 'f')
        chr -= ('a' - 10);

    return chr;
}

/**
 *  @brief convert string to hex integer
 *
 *  @param s        A pointer string buffer
 *  @return         hex integer
 */
u32
a2hex(char *s)
{
    u32 val = 0;
    if (!strncasecmp("0x", s, 2)) {
        s += 2;
    }

    while (*s && isxdigit(*s)) {
        val = (val << 4) + hexc2bin(*s++);
    }

    return val;
}

/**
 *  @brief convert string to mac address
 *
 *  @param s                A pointer string buffer
 *  @param mac_addr         pointer to mac address array
 *  @return					none
 */
void
a2_mac_addr(char *s, u8 * mac_addr)
{
    u32 val = 0, count = 0;
    if (!strncasecmp("0x", s, 2)) {
        s += 2;
    }

    while (*s && isxdigit(*s)) {
        val = (val << 4) + hexc2bin(*s++);
        val = (val << 4) + hexc2bin(*s++);
        mac_addr[count] = val;
        count++;
        val = 0;
    }

    return;
}

/**
 *  @brief convert string to integer
 *
 *  @param ptr      A pointer to data buffer
 *  @param chr      A pointer to return integer
 *  @return         A pointer to next data field
 */
s8 *
convert2hex(s8 * ptr, u8 * chr)
{
    u8 val;

    for (val = 0; *ptr && isxdigit(*ptr); ptr++) {
        val = (val * 16) + hexval(*ptr);
    }

    *chr = val;

    return ptr;
}

/**
 *  @brief convert frequency to channel
 *
 *  @param freq     frequency value
 *  @return         channel number
 */
static int
mapping_freq_to_chan(int freq)
{
    int i, table;
    int chan = 0;

    table = sizeof(channel_freq_UN_BG) / sizeof(CHANNEL_FREQ_ENTRY);
    for (i = 0; i < table; i++) {
        if (freq == channel_freq_UN_BG[i].Freq) {
            chan = channel_freq_UN_BG[i].Channel;
            break;
        }
    }

    table = sizeof(channel_freq_UN_AJ) / sizeof(CHANNEL_FREQ_ENTRY);
    for (i = 0; i < table; i++) {
        if (freq == channel_freq_UN_AJ[i].Freq) {
            chan = channel_freq_UN_AJ[i].Channel;
            break;
        }
    }

    return chan;
}

/**
 *  @brief Get private info.
 *
 *  @param ifname   A pointer to net name
 *  @return         The number of private ioctls if success, WPS_STATUS_FAIL if fail
 */
static int
get_private_info(const s8 * ifname)
{
    /* This function sends the SIOCGIWPRIV command which is handled by the
       kernel. and gets the total number of private ioctl's available in the
       host driver. */
    struct iwreq iwr;
    int s, ret = WPS_STATUS_SUCCESS;
    struct iw_priv_args *pPriv = NULL;
    struct iw_priv_args *newPriv;
    int result = 0;
    size_t size = IW_INIT_PRIV_NUM;

    ENTER();

    s = socket(PF_INET, SOCK_DGRAM, 0);
    if (s < 0) {
        perror("socket[PF_INET,SOCK_DGRAM]");
        return WPS_STATUS_FAIL;
    }

    memset(&iwr, 0, sizeof(iwr));
    strncpy(iwr.ifr_name, (char *) ifname, IFNAMSIZ);

    do {
        /* (Re)allocate the buffer */
        newPriv = realloc(pPriv, size * sizeof(pPriv[0]));
        if (newPriv == NULL) {
            mwu_printf(MSG_WARNING, "Error: Buffer allocation failed\n");
            ret = WPS_STATUS_FAIL;
            break;
        }
        pPriv = newPriv;

        iwr.u.data.pointer = (caddr_t) pPriv;
        iwr.u.data.length = size;
        iwr.u.data.flags = 0;

        if (ioctl(s, SIOCGIWPRIV, &iwr) < 0) {
            result = errno;
            ret = WPS_STATUS_FAIL;
            if (result == E2BIG) {
                /* We need a bigger buffer. Check if kernel gave us any hints. */
                if (iwr.u.data.length > size) {
                    /* Kernel provided required size */
                    size = iwr.u.data.length;
                } else {
                    /* No hint from kernel, double the buffer size */
                    size *= 2;
                }
            } else {
                /* ioctl error */
                perror("ioctl[SIOCGIWPRIV]");
                break;
            }
        } else {
            /* Success. Return the number of private ioctls */
            Priv_args = pPriv;
            ret = iwr.u.data.length;
            break;
        }
    } while (size <= IW_MAX_PRIV_NUM);

    if ((ret == WPS_STATUS_FAIL) && (pPriv))
        free(pPriv);

    close(s);

    LEAVE();
    return ret;
}

/**
 *  @brief Get Sub command ioctl number
 *
 *  @param i            command index
 *  @param priv_cnt     Total number of private ioctls available in driver
 *  @param ioctl_val    A pointer to return ioctl number
 *  @param subioctl_val A pointer to return sub-ioctl number
 *  @return             WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
static int
marvell_get_subioctl_no(s32 i, s32 priv_cnt, int *ioctl_val, int *subioctl_val)
{
    s32 j;

    ENTER();

    if (Priv_args[i].cmd >= SIOCDEVPRIVATE) {
        *ioctl_val = Priv_args[i].cmd;
        *subioctl_val = 0;
        LEAVE();
        return WPS_STATUS_SUCCESS;
    }

    j = -1;

    /* Find the matching *real* ioctl */

    while ((++j < priv_cnt)
           && ((Priv_args[j].name[0] != '\0') ||
               (Priv_args[j].set_args != Priv_args[i].set_args) ||
               (Priv_args[j].get_args != Priv_args[i].get_args))) {
    }

    /* If not found... */
    if (j == priv_cnt) {
        mwu_printf(MSG_WARNING,
                   "%s: Invalid private ioctl definition for: 0x%x\n", dev_name,
                   Priv_args[i].cmd);
        LEAVE();
        return WPS_STATUS_FAIL;
    }

    /* Save ioctl numbers */
    *ioctl_val = Priv_args[j].cmd;
    *subioctl_val = Priv_args[i].cmd;

    LEAVE();
    return WPS_STATUS_SUCCESS;
}

/**
 *  @brief Get ioctl number
 *
 *  @param ifname       A pointer to net name
 *  @param priv_cmd     A pointer to priv command buffer
 *  @param ioctl_val    A pointer to return ioctl number
 *  @param subioctl_val A pointer to return sub-ioctl number
 *  @return             WPS_STATUS_SUCCESS--success, otherwise--fail
 */
int
marvell_get_ioctl_no(const s8 * ifname,
                     const s8 * priv_cmd, int *ioctl_val, int *subioctl_val)
{
    s32 i;
    s32 priv_cnt;
    int ret = WPS_STATUS_FAIL;

    ENTER();

    priv_cnt = get_private_info(ifname);

    /* Are there any private ioctls? */
    if (priv_cnt <= 0 || priv_cnt > IW_MAX_PRIV_NUM) {
        /* Could skip this message ? */
        mwu_printf(MSG_WARNING, "%-8.8s  no private ioctls.\n", ifname);
    } else {
        for (i = 0; i < priv_cnt; i++) {
            if (Priv_args[i].name[0] &&
                !strcmp((char *) Priv_args[i].name, (char *) priv_cmd)) {
                ret =
                    marvell_get_subioctl_no(i, priv_cnt, ioctl_val,
                                            subioctl_val);
                break;
            }
        }
    }

    if (Priv_args) {
        free(Priv_args);
        Priv_args = NULL;
    }

    LEAVE();
    return ret;
}

/**
 *  @brief Retrieve the ioctl and sub-ioctl numbers for the given ioctl string
 *
 *  @param ifname       Private IOCTL string name
 *  @param ioctl_val    A pointer to return ioctl number
 *  @param subioctl_val A pointer to return sub-ioctl number
 *
 *  @return             WPS_STATUS_SUCCESS--success, otherwise--fail
 */
static int
get_priv_ioctl(char *ioctl_name, int *ioctl_val, int *subioctl_val)
{
    int retval;

    ENTER();

    retval = marvell_get_ioctl_no((s8 *) dev_name,
                                  (s8 *) ioctl_name, ioctl_val, subioctl_val);

    /* Debug print discovered IOCTL values */
    mwu_printf(DEBUG_WLAN, "ioctl %s: %x, %x\n", ioctl_name, *ioctl_val,
               *subioctl_val);

    LEAVE();
    return retval;
}

/**
 *  @brief  get range
 *
 *  @return         WPS_STATUS_SUCCESS--success, otherwise --fail
 */
static int
get_range(void)
{
    struct iw_range *range;
    struct iwreq iwr;
    size_t buflen;

    ENTER();

    buflen = sizeof(struct iw_range) + 500;
    range = malloc(buflen);
    if (range == NULL)
        return WPS_STATUS_FAIL;
    memset(range, 0, buflen);

    memset(&iwr, 0, sizeof(struct iwreq));
    iwr.u.data.pointer = (caddr_t) range;
    iwr.u.data.length = buflen;
    strncpy(iwr.ifr_name, (char *) dev_name, IFNAMSIZ);

    if ((ioctl(ioctl_sock, SIOCGIWRANGE, &iwr)) < 0) {
        printf("Get Range Results Failed\n");
        free(range);
        LEAVE();
        return WPS_STATUS_FAIL;
    }

    we_version_compiled = range->we_version_compiled;
    mwu_printf(DEBUG_INIT, "Driver build with Wireless Extension %d\n",
               range->we_version_compiled);
    mwu_printf(DEBUG_INIT, "WPS Application build with Wireless Extension %d\n",
               WIRELESS_EXT);

    free(range);

    LEAVE();
    return WPS_STATUS_SUCCESS;
}

/*
 *  @brief convert hex char to integer
 *
 *  @param c        Hex char
 *  @return         Integer value or WPS_STATUS_FAIL
 */
int
hex2num(s8 c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return WPS_STATUS_FAIL;
}

/*
 *  @brief convert integer to hex char
 *
 *  @param c        Integer value or WPS_STATUS_FAIL
 *  @return         Hex char
 */

int
num2hex(s8 c)
{
    if ((c + '0') >= '0' && (c + '0') <= '9')
        return c + '0';
    if ((c - 10 + 'a') >= 'a' && (c - 10 + 'a') <= 'f')
        return c - 10 + 'a';
    if ((c - 10 + 'A') >= 'A' && (c - 10 + 'A') <= 'F')
        return c - 10 + 'A';
    return WPS_STATUS_FAIL;
}

/*
 *  @brief convert hex char to integer
 *
 *  @param c        char
 *  @return         Integer value or WPS_STATUS_FAIL
 */
static int
hex2byte(const s8 * hex)
{
    s32 a, b;
    a = hex2num(*hex++);
    if (a < 0)
        return WPS_STATUS_FAIL;
    b = hex2num(*hex++);
    if (b < 0)
        return WPS_STATUS_FAIL;
    return (a << 4) | b;
}

/*
 *  @brief convert hex char to integer
 *
 *  @param hex      A pointer to hex string
 *  @param buf      buffer to storage the data
 *  @param len
 *  @return         WPS_STATUS_SUCCESS--success, otherwise --fail
 */
int
hexstr2bin(const s8 * hex, u8 * buf, size_t len)
{
    s32 a;
    u32 i;
    const s8 *ipos = hex;
    u8 *opos = buf;

    for (i = 0; i < len; i++) {
        a = hex2byte(ipos);
        if (a < 0)
            return WPS_STATUS_FAIL;
        *opos++ = a;
        ipos += 2;
    }
    return WPS_STATUS_SUCCESS;
}

/*
 *  @brief convert binary data to Hex string
 *
 *  @param bin      A pointer to binary data
 *  @param hex      A pointer to hex string
 *  @param len
 *  @return         WPS_STATUS_SUCCESS--success, otherwise --fail
 */

int
bin2hexstr(const u8 * bin, s8 * hex, size_t len)
{
    u32 i;

    for (i = 0; i < len; i++) {
        hex[2 * i] = num2hex(bin[i] >> 4);
        hex[(2 * i) + 1] = num2hex(bin[i] & 0x0f);
    }

    return WPS_STATUS_SUCCESS;
}

#if WIRELESS_EXT > 14
/**
 *  @brief parse custom info
 *
 *  @param results      A pointer to SCAN_RESULTS buffer
 *  @param data         A pointer to iw_point structure
 *  @param idx          AP index
 *  @return             NA
 */
static void
parse_custom_info(SCAN_RESULTS * results, struct iw_point *data, s32 idx)
{
    ENTER();

    if (!data->pointer || !data->length) {
        mwu_printf(MSG_ERROR, "iw_point: Invalid Pointer/Length\n");
        LEAVE();
        return;
    }

    if (!strncmp(data->pointer, "wpa_ie=", 7)) {
        char *spos;
        int bytes;
        spos = (char *) data->pointer + 7;
        bytes = data->length - 7;
        if (bytes & 1) {
            LEAVE();
            return;
        }
        bytes /= 2;
        if (bytes > SSID_MAX_WPA_IE_LEN) {
            mwu_printf(MSG_INFO, "Too long WPA IE (%d)", bytes);
            LEAVE();
            return;
        }
        hexstr2bin((s8 *) spos, results[idx].wpa_ie, bytes);
        results[idx].wpa_ie_len = bytes;
        mwu_printf(DEBUG_WLAN, "AP index %d", idx);
        mwu_hexdump(DEBUG_WLAN, "WPA IE from driver :",
                    results[idx].wpa_ie, results[idx].wpa_ie_len);
    } else if (!strncmp(data->pointer, "rsn_ie=", 7)) {
        char *spos;
        int bytes;
        spos = (char *) data->pointer + 7;
        bytes = data->length - 7;
        if (bytes & 1) {
            LEAVE();
            return;
        }
        bytes /= 2;
        if (bytes > SSID_MAX_WPA_IE_LEN) {
            mwu_printf(MSG_INFO, "Too long RSN IE (%d)", bytes);
            return;
        }
        hexstr2bin((s8 *) spos, results[idx].rsn_ie, bytes);
        results[idx].rsn_ie_len = bytes;
        mwu_printf(DEBUG_WLAN, "AP index %d", idx);
        mwu_hexdump(DEBUG_WLAN, "RSN IE from driver :",
                    results[idx].rsn_ie, results[idx].rsn_ie_len);
    } else if (!strncmp(data->pointer, "wps_ie=", 7)) {
        char *spos;
        int bytes;
        spos = (char *) data->pointer + 7;
        bytes = data->length - 7;
        if (bytes & 1) {
            LEAVE();
            return;
        }
        bytes /= 2;
        if (bytes > MAX_WPS_IE_LEN) {
            mwu_printf(MSG_INFO, "Too long WPS IE (%d)", bytes);
            LEAVE();
            return;
        }
        hexstr2bin((s8 *) spos, results[idx].wps_ie, bytes);
        results[idx].wps_ie_len = bytes;
        mwu_printf(DEBUG_WLAN, "AP index %d", idx);
        mwu_hexdump(DEBUG_WLAN, "WPS IE from driver: ",
                    results[idx].wps_ie, results[idx].wps_ie_len);
    }
    LEAVE();
}
#endif

#if WIRELESS_EXT >= 18
/**
 *  @brief parse generic ie
 *
 *  @param results      A pointer to SCAN_RESULTS buffer
 *  @param data         A pointer to iw_point structure
 *  @param idx          AP index
 *  @return             NA
 */
static void
parse_generic_ie(SCAN_RESULTS * results, struct iw_point *data, s32 idx)
{
    int offset = 0;
    int buflen;
    unsigned char *buffer;
    unsigned char rsn_oui[3] = { 0x00, 0x0f, 0xac };
#define OUI_LEN 4
    u8 wpa_oui[OUI_LEN] = { 0x00, 0x50, 0xf2, 0x01 };   /* WPA */
    u8 wps_oui[OUI_LEN] = { 0x00, 0x50, 0xf2, 0x04 };   /* WPS */

    ENTER();

    if (!data->pointer || !data->length) {
        mwu_printf(MSG_ERROR, "iw_point: Invalid Pointer/Length\n");
        LEAVE();
        return;
    }

    buffer = data->pointer;
    buflen = data->length;

    /*
     * There might be lots of thing in generic IE.
     * We only process WPA/RSN IE we need
     */

    /* Loop on each IE, each IE is minimum 2 bytes */
    while (offset <= (buflen - 2)) {
        int ielen = buffer[offset + 1] + 2;

        /* Check IE type */
        switch (buffer[offset]) {
        case 0xdd:             /* WPA1 (and other) */
            if (memcmp(&buffer[offset + 2], wpa_oui, 4) == 0) {
                memcpy(results[idx].wpa_ie, buffer + offset, ielen);
                results[idx].wpa_ie_len = ielen;
                mwu_printf(DEBUG_WLAN, "AP index %d", idx);
                mwu_hexdump(DEBUG_WLAN, "WPA IE from driver :",
                            results[idx].wpa_ie, results[idx].wpa_ie_len);
            } else if (memcmp(&buffer[offset + 2], wps_oui, 4) == 0) {
                memcpy(results[idx].wps_ie, buffer + offset, ielen);
                results[idx].wps_ie_len = ielen;
                mwu_printf(DEBUG_WLAN, "AP index %d", idx);
                mwu_hexdump(DEBUG_WLAN, "WPS IE from driver :",
                            results[idx].wps_ie, results[idx].wps_ie_len);
            }
            break;
        case 0x30:             /* WPA2 , Len, 2 bytes of version and rsn_oui */
            if ((memcmp(&buffer[offset + 4], rsn_oui, 3) == 0)) {
                memcpy(results[idx].rsn_ie, buffer + offset, ielen);
                results[idx].rsn_ie_len = ielen;
                mwu_printf(DEBUG_WLAN, "AP index %d", idx);
                mwu_hexdump(DEBUG_WLAN, "RSN IE from driver :",
                            results[idx].rsn_ie, results[idx].rsn_ie_len);
            }
            break;
        default:
            break;
        }
        /* Skip over this IE to the next one in the list. */
        offset += buffer[offset + 1] + 2;
    }

    LEAVE();
}
#endif

/**
 *  @brief          Get scan result from WLAN driver
 *
 *  @param results  A pointer to scan result buffer
 *  @return         Number of scan if success, WPS_STATUS_FAIL if fail
 */
static int
wlan_get_scan_results(SCAN_RESULTS * results)
{
    struct iwreq iwr;
    struct iw_event iwe;
    struct iw_point iwp;
    int buflen = IW_SCAN_MAX_DATA;
    u8 *buffer = NULL;
    u8 *newbuf = NULL;
    u32 ap_index = 0;
    u32 apNum = 0;
    int new_index = 0;
    u32 len = 0;
    u32 length = 0;

    ENTER();

    memset(&iwr, 0, sizeof(struct iwreq));

  _read_result_:
    /* (Re)allocate the buffer - realloc(NULL, len) == malloc(len) */
    newbuf = realloc(buffer, buflen);
    if (newbuf == NULL) {
        if (buffer)
            free(buffer);
        mwu_printf(MSG_ERROR, "Memory allocation failed !\n");
        LEAVE();
        return WPS_STATUS_FAIL;
    }
    buffer = newbuf;

  _read_again_:
    /* Try to read the results */
    iwr.u.data.pointer = buffer;
    iwr.u.data.length = buflen;
    strncpy(iwr.ifr_name, (char *) dev_name, IFNAMSIZ);

    if ((ioctl(ioctl_sock, SIOCGIWSCAN, &iwr)) < 0) {
        /* Check if buffer was too small */
        if (errno == E2BIG) {
            mwu_printf(DEBUG_WLAN, "Buffer too small !");

            /* Check if the driver gave us any hints. */
            if (iwr.u.data.length > buflen)
                buflen = iwr.u.data.length;
            else
                buflen *= 2;

            /* Try again */
            goto _read_result_;
        } else if (errno == EAGAIN) {
            mwu_printf(DEBUG_WLAN, "Reading Scan results again !");

            /* Wait for 100ms similar to WEXT. */
            mwu_sleep(0, 100 * 1000);

            /* Try again */
            goto _read_again_;
        } else {
            mwu_printf(MSG_ERROR, "Get scan results failed !\n");
            if (buffer)
                free(buffer);
            LEAVE();
            return WPS_STATUS_FAIL;
        }
    } else {
        /* We have the results, go to process them */
        mwu_printf(DEBUG_WLAN, "Scan results ready, process them ... ");
    }

    length = iwr.u.data.length;

    while (len + IW_EV_LCP_LEN < length) {
        s32 MRV_EV_POINT_LEN;

        memcpy((s8 *) & iwe, buffer + len, sizeof(struct iw_event));

        if (we_version_compiled > 18)
            MRV_EV_POINT_LEN =
                IW_EV_LCP_LEN + sizeof(struct iw_point) - MRV_EV_POINT_OFF;
        else
            MRV_EV_POINT_LEN = IW_EV_LCP_LEN + sizeof(struct iw_point);

        if ((iwe.cmd == SIOCGIWESSID) || (iwe.cmd == SIOCGIWENCODE) ||
#if WIRELESS_EXT >= 18
            (iwe.cmd == IWEVGENIE) ||
#endif
            (iwe.cmd == IWEVCUSTOM)) {
            if (we_version_compiled > 18)
                memcpy((s8 *) & iwp,
                       buffer + len + IW_EV_LCP_LEN - MRV_EV_POINT_OFF,
                       sizeof(struct iw_point));
            else
                memcpy((s8 *) & iwp, buffer + len + IW_EV_LCP_LEN,
                       sizeof(struct iw_point));
            iwp.pointer = buffer + len + MRV_EV_POINT_LEN;
        }

        switch (iwe.cmd) {
        case SIOCGIWAP:
            if (new_index && ap_index < IW_MAX_AP - 1)
                ap_index++;
            memcpy(results[ap_index].bssid, iwe.u.ap_addr.sa_data, ETH_ALEN);
            new_index = 1;
            break;

        case SIOCGIWESSID:
            if ((iwp.pointer) && (iwp.length)) {
                memcpy(results[ap_index].ssid, (s8 *) iwp.pointer, iwp.length);
                results[ap_index].ssid_len = iwp.length;
            }
            break;

        case SIOCGIWENCODE:
            if (!(iwp.flags & IW_ENCODE_DISABLED)) {
                results[ap_index].caps |= IEEE80211_CAP_PRIVACY;
            }
            break;

        case SIOCGIWMODE:
            if (iwe.u.mode == IW_MODE_ADHOC)
                results[ap_index].caps |= IEEE80211_CAP_IBSS;
            else if (iwe.u.mode == IW_MODE_MASTER ||
                     iwe.u.mode == IW_MODE_INFRA)
                results[ap_index].caps |= IEEE80211_CAP_ESS;
            break;

        case SIOCGIWFREQ:      /* frequencey/channel */
            {
                int divi = 1000000, i;
                if (iwe.u.freq.e > 6) {
                    mwu_printf(DEBUG_WLAN,
                               "Invalid freq in scan results (BSSID=" UTIL_MACSTR
                               ": m=%d e=%d\n",
                               UTIL_MAC2STR(results[ap_index].bssid), iwe.u.freq.m,
                               iwe.u.freq.e);
                    break;
                }
                for (i = 0; i < iwe.u.freq.e; i++)
                    divi /= 10;
                results[ap_index].freq = iwe.u.freq.m / divi;
            }
            break;

#if WIRELESS_EXT > 14
        case IWEVCUSTOM:
            /* iwp point to the start of custom info */
            parse_custom_info(results, &iwp, ap_index);
            break;
#endif

        case IWEVQUAL:         /* signal quality */
            results[ap_index].qual = iwe.u.qual.qual;
            results[ap_index].noise = iwe.u.qual.noise;
            results[ap_index].level = iwe.u.qual.level;
            break;

#if WIRELESS_EXT >= 18
        case IWEVGENIE:
            /* iwp point to the start of generic IE */
            parse_generic_ie(results, &iwp, ap_index);
            break;
#endif
        }

        len += iwe.len;
    }
    if (new_index)
        apNum = ap_index + 1;

    if (buffer)
        free(buffer);

    LEAVE();
    return apNum;
}

/********************************************************
        Global Functions
********************************************************/

/**
 *  @brief Send scan ioctl command to WLAN driver
 *
 *  @return         WPS_STATUS_SUCCESS--success, otherwise--fail
 */
int
wps_wlan_set_scan(void)
{
    struct iwreq iwr;
    int ret = WPS_STATUS_SUCCESS;

    ENTER();

    memset(&iwr, 0, sizeof(iwr));
    strncpy(iwr.ifr_name, (char *) dev_name, IFNAMSIZ);

    iwr.u.data.pointer = NULL;
    iwr.u.data.flags = 0;
    iwr.u.data.length = 0;

    if (ioctl(ioctl_sock, SIOCSIWSCAN, &iwr) < 0) {
        perror("ioctl[SIOCSIWSCAN]");
        ret = WPS_STATUS_FAIL;
    }

    LEAVE();
    return ret;
}

/**
 *  @brief Process scan results get from WLAN driver
 *
 *  @param wps_s       Pointer to WPS global structure
 *  @param filter      Filter AP in scan result
 *  @return         WPS_STATUS_SUCCESS--success, otherwise--fail
 */
int
wps_wlan_scan_results(WPS_DATA * wps_s, u16 filter)
{
    SCAN_RESULTS *results = NULL, *tmp = NULL;
    int num;
    u16 type = 0xffff;
    u32 is_selected_registrar = 0;

    ENTER();

    results = malloc(SCAN_AP_LIMIT * sizeof(SCAN_RESULTS));
    if (results == NULL) {
        mwu_printf(MSG_WARNING, "Failed to allocate memory for scan "
                   "results");
        LEAVE();
        return WPS_STATUS_FAIL;
    }

    memset(results, 0, SCAN_AP_LIMIT * sizeof(SCAN_RESULTS));

    num = wlan_get_scan_results(results);
    mwu_printf(DEBUG_WLAN, "Scan results: %d", num);

    if (num < 0) {
        mwu_printf(DEBUG_WLAN, "Failed to get scan results");
        free(results);
        LEAVE();
        return WPS_STATUS_FAIL;
    }
    if (num > SCAN_AP_LIMIT) {
        mwu_printf(DEBUG_WLAN, "Not enough room for all APs (%d < %d)",
                   num, SCAN_AP_LIMIT);
        num = SCAN_AP_LIMIT;
    }
    if (filter == FILTER_PBC) {
        int k, match = 0;

        tmp = malloc(num * sizeof(SCAN_RESULTS));
        if (tmp == NULL) {
            mwu_printf(MSG_WARNING,
                       "Failed to allocate memory for scan results");
            if (results)
                free(results);
            LEAVE();
            return WPS_STATUS_FAIL;
        }

        for (k = 0; k < num; k++) {
            if (results[k].wps_ie_len > 0) {
                type =
                    wps_probe_response_device_password_id_parser(results[k].
                                                                 wps_ie,
                                                                 results[k].
                                                                 wps_ie_len);

                if (type == DEVICE_PASSWORD_PUSH_BUTTON) {
                    memcpy((void *) &tmp[match], (void *) &results[k],
                           sizeof(SCAN_RESULTS));
                    match++;
                }
            }
        }                       /* end of k loop */

        if (results)
            free(results);

        wps_s->num_scan_results = match;
        if (match > 0) {
            if (wps_s->scan_results)
                free(wps_s->scan_results);
            wps_s->scan_results = tmp;
        } else {
            if (tmp)
                free(tmp);
        }
    } /* PBC */
    else {
        int k, match = 0;

        /* Only save Infra mode AP */
        tmp = malloc(num * sizeof(SCAN_RESULTS));
        if (tmp == NULL) {
            mwu_printf(MSG_WARNING,
                       "Failed to allocate memory for scan results");
            if (results)
                free(results);
            LEAVE();
            return WPS_STATUS_FAIL;
        }

        for (k = 0; k < num; k++) {
            if (results[k].caps & IEEE80211_CAP_ESS &&
                results[k].wps_ie_len > 0) {
                wps_probe_response_selected_registrar_parser(results[k].wps_ie,
                                                             results[k].wps_ie_len,
                                                             &is_selected_registrar);
                /* Only save active registrar */
                if (is_selected_registrar) {
                    memcpy((void *) &tmp[match], (void *) &results[k],
                           sizeof(SCAN_RESULTS));
                    match++;
                }
            }
        }                       /* end of k loop */

        if (results)
            free(results);

        wps_s->num_scan_results = match;
        if (match > 0) {
            if (wps_s->scan_results)
                free(wps_s->scan_results);
            wps_s->scan_results = tmp;
        } else {
            if (tmp)
                free(tmp);
        }
    }

    /* For WIFIDIR, check for matching device Id and send connect request */
    if (gpwps_info->role == WIFIDIR_ROLE) {
        mwu_printf(DEBUG_WIFIDIR_DISCOVERY, "\nScan Result : %d entries.\n",
                   wps_s->num_scan_results);
        int i;
        for (i = 0; i < wps_s->num_scan_results; i++) {
            if (!memcmp(gpwps_info->registrar.mac_address,
                        wps_s->scan_results[i].bssid, ETH_ALEN) &&
                (!strlen(gpwps_info->registrar.go_ssid) ||
                 !memcmp(gpwps_info->registrar.go_ssid,
                         wps_s->scan_results[i].ssid, MAX_SSID_LEN))) {
                wps_s->wifidir_data.dev_found = i;
                if(wps_s->wifidir_data.current_peer != NULL) {
                    wps_s->wifidir_data.current_peer->op_channel =
                        mapping_freq_to_chan(wps_s->scan_results[i].freq);
                }
                mwu_printf(DEBUG_WIFIDIR_DISCOVERY,
                           "\nSelected Device found at %d.\n", i);
                break;
            }
        }
        if (i == wps_s->num_scan_results) {
            /*No peer has been found*/
            mwu_printf(MSG_WARNING,
                       "No Peer has been found!");
            wps_s->wifidir_data.dev_found = -1;
        }
    } else
    {
        int i;
        unsigned int j;

        printf("\nScan Result : %d entry in Scan List\n",
               wps_s->num_scan_results);
        if (wps_s->num_scan_results == 0) {
            switch (gpwps_info->enrollee.device_password_id) {

            case DEVICE_PASSWORD_ID_PIN:
                printf("\nNo device found with Device password ID PIN.\n");
                break;
            case DEVICE_PASSWORD_PUSH_BUTTON:
                printf("\nNo device found with Device password ID PBC.\n");
                break;
            }
        }

        if (wps_s->num_scan_results > 0) {
            printf
                ("-----------------------------------------------------------------------\n");
            printf
                ("  # |  Mode | Chan# | WPA | WPS |       BSSID       |     SSID         \n");
            printf
                ("-----------------------------------------------------------------------\n");

            for (i = 0; i < wps_s->num_scan_results; i++) {
                /* Index Number */
                printf(" %02d |", i);

                /* Mode */
                if (wps_s->scan_results[i].caps & IEEE80211_CAP_IBSS)
                    printf(" Adhoc |");
                else
                    printf(" Infra |");

                /* Channel Number */
                printf("  %03d  |",
                       mapping_freq_to_chan(wps_s->scan_results[i].freq));

                /* WPA Enabled ? */
                if (wps_s->scan_results[i].wpa_ie_len != 0 ||
                    wps_s->scan_results[i].rsn_ie_len != 0) {
                    printf("  Y  |");
                } else {
                    printf("  N  |");
                }

                if (wps_s->scan_results[i].wps_ie_len != 0) {
                    type =
                        wps_probe_response_device_password_id_parser(wps_s->
                                                                     scan_results
                                                                     [i].
                                                                     wps_ie,
                                                                     wps_s->
                                                                     scan_results
                                                                     [i].
                                                                     wps_ie_len);

                    if (type == DEVICE_PASSWORD_PUSH_BUTTON)
                        printf("  Y* |");
                    else
                        printf("  Y  |");

                } else {
                    printf("  N  |");
                }

                /* BSSID */
                printf(" %02x:%02x:%02x:%02x:%02x:%02x |",
                       wps_s->scan_results[i].bssid[0],
                       wps_s->scan_results[i].bssid[1],
                       wps_s->scan_results[i].bssid[2],
                       wps_s->scan_results[i].bssid[3],
                       wps_s->scan_results[i].bssid[4],
                       wps_s->scan_results[i].bssid[5]);

                /* SSID */
                printf(" ");
                for (j = 0; j < wps_s->scan_results[i].ssid_len; j++) {
                    printf("%c", wps_s->scan_results[i].ssid[j]);
                }
                printf("\n");
            }
        }
    }

    LEAVE();
    return WPS_STATUS_SUCCESS;
}

#ifdef WPS_IE_NEEDED
/**
 *  @brief Set WPS IE for foreground scan to WLAN Driver
 *
 *  @return         WPS_STATUS_SUCCESS--success, otherwise--fail
 */
int
wps_wlan_set_fgscan_wps_ie(void)
{
    u8 *buf;
    int wps_ie_size, buf_len;
    int ret = WPS_STATUS_SUCCESS;
    struct iwreq iwr;
    int ioctl_val, subioctl_val;

    ENTER();

    buf = (u8 *) malloc(MRVDRV_SIZE_OF_CMD_BUFFER);
    memset(buf, 0, MRVDRV_SIZE_OF_CMD_BUFFER);

    buf[0] = 221;               /* ElementID of WPS_IE */
    wps_ie_size = wps_sta_probe_request_prepare(&buf[2]);
    buf[1] = (u8) wps_ie_size;  /* IE len */
    buf_len = wps_ie_size + 2;  /* Add IE_ID and IE_Len fileds */

    mwu_hexdump(DEBUG_WLAN, "WPS_IE", (const unsigned char *) buf, buf_len);

    /*
     * Always try private ioctl "setgenie" first
     * If private ioctl failed and if driver build with WE version >= 18,
     * try standard ioctl "SIOCSIWGENIE".
     */
    if (get_priv_ioctl("setgenie", &ioctl_val, &subioctl_val) ==
        WPS_STATUS_FAIL) {
        mwu_printf(MSG_ERROR, "ERROR : ioctl[setgenie] NOT SUPPORT !");
#if (WIRELESS_EXT >= 18)
        if (we_version_compiled < 18) {
            ret = WPS_STATUS_FAIL;
            goto _exit_;
        } else
            goto _we18_;
#else
        ret = WPS_STATUS_FAIL;
        goto _exit_;
#endif
    }

    /*
     * Set up and execute the ioctl call
     */
    memset(&iwr, 0, sizeof(iwr));
    strncpy(iwr.ifr_name, (char *) dev_name, IFNAMSIZ);
    iwr.u.data.pointer = (caddr_t) buf;
    iwr.u.data.length = buf_len;
    iwr.u.data.flags = subioctl_val;

    if (ioctl(ioctl_sock, ioctl_val, &iwr) < 0) {
        perror("ioctl[setgenie]");
#if (WIRELESS_EXT >= 18)
        if (we_version_compiled < 18) {
            ret = WPS_STATUS_FAIL;
            goto _exit_;
        }
#else
        ret = WPS_STATUS_FAIL;
        goto _exit_;
#endif
    } else {
        ret = WPS_STATUS_SUCCESS;
        goto _exit_;
    }

#if (WIRELESS_EXT >= 18)
  _we18_:
    /*
     * If private ioctl failed and if driver build with WE version >= 18,
     * try standard ioctl "SIOCSIWGENIE".
     */
    if (we_version_compiled >= 18) {
        /*
         * Driver WE version >= 18, use standard ioctl
         */
        memset(&iwr, 0, sizeof(iwr));
        strncpy(iwr.ifr_name, (char *) dev_name, IFNAMSIZ);
        iwr.u.data.pointer = (caddr_t) buf;
        iwr.u.data.length = buf_len;

        if (ioctl(ioctl_sock, SIOCSIWGENIE, &iwr) < 0) {
            perror("ioctl[SIOCSIWGENIE]");
            ret = WPS_STATUS_FAIL;
            goto _exit_;
        } else {
            ret = WPS_STATUS_SUCCESS;
            goto _exit_;
        }
    }
#endif /* (WIRELESS_EXT >= 18) */

  _exit_:
    if (buf)
        free(buf);

    LEAVE();
    return ret;
}

/**
 *  @brief Reset WPS IE for foreground scan to WLAN Driver
 *
 *  @return         WPS_STATUS_SUCCESS--success, otherwise--fail
 */
int
wps_wlan_reset_wps_ie(void)
{
    u8 buf[10];
    int buf_len;
    u8 wps_oui[4] = { 0x00, 0x50, 0xF2, 0x04 };
    int ret = WPS_STATUS_SUCCESS;
    struct iwreq iwr;
    int ioctl_val, subioctl_val;

    ENTER();

    buf[0] = 221;               /* ElementID of WPS_IE */
    memcpy(&buf[2], wps_oui, sizeof(wps_oui));
    buf[1] = sizeof(wps_oui);   /* IE len */
    buf_len = sizeof(wps_oui) + 2;      /* Add IE_ID and IE_Len fileds */

    /*
     * Always try private ioctl "setgenie" first
     * If private ioctl failed and if driver build with WE version >= 18,
     * try standard ioctl "SIOCSIWGENIE".
     */
    if (get_priv_ioctl("setgenie", &ioctl_val, &subioctl_val) ==
        WPS_STATUS_FAIL) {
        mwu_printf(MSG_ERROR, "ERROR : ioctl[setgenie] NOT SUPPORT !");
#if (WIRELESS_EXT >= 18)
        if (we_version_compiled < 18) {
            ret = WPS_STATUS_FAIL;
            goto _exit_;
        } else
            goto _we18_;
#else
        ret = WPS_STATUS_FAIL;
        goto _exit_;
#endif
    }

    /*
     * Set up and execute the ioctl call
     */
    memset(&iwr, 0, sizeof(iwr));
    strncpy(iwr.ifr_name, (char *) dev_name, IFNAMSIZ);
    iwr.u.data.pointer = (caddr_t) buf;
    iwr.u.data.length = buf_len;
    iwr.u.data.flags = subioctl_val;

    if (ioctl(ioctl_sock, ioctl_val, &iwr) < 0) {
        perror("ioctl[setgenie]");
#if (WIRELESS_EXT >= 18)
        if (we_version_compiled < 18) {
            ret = WPS_STATUS_FAIL;
            goto _exit_;
        }
#else
        ret = WPS_STATUS_FAIL;
        goto _exit_;
#endif
    } else {
        ret = WPS_STATUS_SUCCESS;
        goto _exit_;
    }

#if (WIRELESS_EXT >= 18)
  _we18_:
    /*
     * If private ioctl failed and if driver build with WE version >= 18,
     * try standard ioctl "SIOCSIWGENIE".
     */
    if (we_version_compiled >= 18) {
        /*
         * Driver WE version >= 18, use standard ioctl
         */
        memset(&iwr, 0, sizeof(iwr));
        strncpy(iwr.ifr_name, (char *) dev_name, IFNAMSIZ);
        iwr.u.data.pointer = (caddr_t) buf;
        iwr.u.data.length = buf_len;

        if (ioctl(ioctl_sock, SIOCSIWGENIE, &iwr) < 0) {
            perror("ioctl[SIOCSIWGENIE]");
            ret = WPS_STATUS_FAIL;
            goto _exit_;
        } else {
            ret = WPS_STATUS_SUCCESS;
            goto _exit_;
        }
    }
#endif /* (WIRELESS_EXT >= 18) */

  _exit_:
    LEAVE();
    return ret;
}
#endif

/**
 *  @brief  Get associated ESSID from WLAN driver (SIOCGIWESSID)
 *
 *  @param ssid       Buffer for the SSID, must be at least 32 bytes long
 *  @return           SSID length on success, WPS_STATUS_FAIL on failure
 */
int
wps_wlan_get_ssid(u8 * ssid)
{
    struct iwreq iwr;
    int ret = 0;

    ENTER();

    memset(&iwr, 0, sizeof(iwr));
    strncpy(iwr.ifr_name, (char *) dev_name, IFNAMSIZ);

    iwr.u.essid.pointer = (caddr_t) ssid;
    iwr.u.essid.length = 32;

    if (ioctl(ioctl_sock, SIOCGIWESSID, &iwr) < 0) {
        perror("ioctl[SIOCGIWESSID]");
        ret = WPS_STATUS_FAIL;
    } else
        ret = iwr.u.essid.length;

    LEAVE();
    return ret;
}

/**
 *  @brief  Set ESSID to associate to WLAN driver (SIOCSIWESSID)
 *
 *  @param ssid       SSID
 *  @param ssid_len   Length of SSID (0..32)
 *  @param skip_scan  Skip scan during association
 *  @return           WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
int
wps_wlan_set_ssid(const u8 * ssid, size_t ssid_len, int skip_scan)
{
    struct iwreq iwr;
    int ret = WPS_STATUS_SUCCESS;
    char buf[33];

    ENTER();

    if (ssid_len > 32) {
        LEAVE();
        return WPS_STATUS_FAIL;
    }

    memset(&iwr, 0, sizeof(iwr));
    strncpy(iwr.ifr_name, (char *) dev_name, IFNAMSIZ);

    /* flags: 1 = ESSID is active, 0 = Any (Promiscuous) */
    if (!skip_scan)
        iwr.u.essid.flags = (ssid_len != 0);
    else
        iwr.u.essid.flags = 0xFFFF;
    memset(buf, 0, sizeof(buf));
    memcpy(buf, ssid, ssid_len);
    iwr.u.essid.pointer = (caddr_t) buf;

    iwr.u.essid.length = ssid_len ? ssid_len : 0;
    mwu_printf(DEBUG_WLAN, "Wireless Ext version %d", WIRELESS_EXT);
    mwu_printf(DEBUG_WLAN, "SSID length %d", iwr.u.essid.length);

    if (ioctl(ioctl_sock, SIOCSIWESSID, &iwr) < 0) {
        perror("ioctl[SIOCSIWESSID]");
        ret = WPS_STATUS_FAIL;
    }

    LEAVE();
    return ret;
}

/**
 *  @brief  Set Auth Mode to WLAN driver (SIOCSIWAUTH)
 *
 *  @param auth_mode  Open or shared auth mode.
 *  @return           WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
int
wps_wlan_set_authentication(int auth_mode)
{
    struct iwreq iwr;
    int ret = WPS_STATUS_SUCCESS;

    ENTER();

    memset(&iwr, 0, sizeof(iwr));
    strncpy(iwr.ifr_name, (char *) dev_name, IFNAMSIZ);
    iwr.u.param.flags = IW_AUTH_80211_AUTH_ALG;

    if (auth_mode == AUTHENTICATION_TYPE_SHARED)
        iwr.u.param.value = IW_AUTH_ALG_SHARED_KEY;
    else
        iwr.u.param.value = IW_AUTH_ALG_OPEN_SYSTEM;

    if (ioctl(ioctl_sock, SIOCSIWAUTH, &iwr) < 0) {
        perror("ioctl[SIOCSIWAUTH]");
        ret = WPS_STATUS_FAIL;
    }

    LEAVE();
    return ret;
}

/**
 *  @brief  Get associated BSSID from WLAN driver (SIOCGIWAP)
 *
 *  @param bssid      Buffer for the BSSID
 *  @return           WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
int
wps_wlan_get_wap(u8 * bssid)
{
    struct iwreq iwr;
    int ret = WPS_STATUS_SUCCESS;

    ENTER();

    memset(&iwr, 0, sizeof(iwr));
    strncpy(iwr.ifr_name, (char *) dev_name, IFNAMSIZ);

    if (ioctl(ioctl_sock, SIOCGIWAP, &iwr) < 0) {
        perror("ioctl[SIOCGIWAP]");
        ret = WPS_STATUS_FAIL;
    }

    if (iwr.u.ap_addr.sa_family == ARPHRD_ETHER) {
        memcpy(bssid, iwr.u.ap_addr.sa_data, ETH_ALEN);
    } else {
        ret = WPS_STATUS_FAIL;
    }

    LEAVE();
    return ret;
}

/**
 *  @brief  Set BSSID to associate to WLAN driver (SIOCSIWAP)
 *
 *  @param bssid      BSSID
 *  @return           WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
int
wps_wlan_set_wap(const u8 * bssid)
{
    struct iwreq iwr;
    int ret = WPS_STATUS_SUCCESS;

    ENTER();

    memset(&iwr, 0, sizeof(iwr));
    strncpy(iwr.ifr_name, (char *) dev_name, IFNAMSIZ);

    iwr.u.ap_addr.sa_family = ARPHRD_ETHER;
    memcpy(iwr.u.ap_addr.sa_data, bssid, ETH_ALEN);

    if (ioctl(ioctl_sock, SIOCSIWAP, &iwr) < 0) {
        perror("ioctl[SIOCSIWAP]");
        ret = WPS_STATUS_FAIL;
    }

    LEAVE();
    return ret;
}

/**
 *  @brief  Set wireless mode to WLAN driver (infra/adhoc) (SIOCSIWMODE)
 *
 *  @param mode       0 = infra/BSS (associate with an AP), 1 = adhoc/IBSS
 *  @return           WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
int
wps_wlan_set_mode(int mode)
{
    struct iwreq iwr;
    int ret = WPS_STATUS_SUCCESS;

    ENTER();

    memset(&iwr, 0, sizeof(iwr));
    strncpy(iwr.ifr_name, (char *) dev_name, IFNAMSIZ);
    iwr.u.mode = mode ? IW_MODE_ADHOC : IW_MODE_INFRA;

    if (ioctl(ioctl_sock, SIOCSIWMODE, &iwr) < 0) {
        perror("ioctl[SIOCSIWMODE]");
        ret = WPS_STATUS_FAIL;
    }

    LEAVE();
    return ret;
}

/**
 *  @brief  Set BSS type (STA/uAP)
 *
 *  @param bss_type   an integer 0: Sta, 1:uAP
 *  @return           WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
int
wps_wlan_set_bss_type(int bss_type)
{
    struct iwreq iwr;
    int ret = WPS_STATUS_SUCCESS;

    int ioctl_val, subioctl_val;

    ENTER();

    if (get_priv_ioctl("bssrole", &ioctl_val, &subioctl_val) == WPS_STATUS_FAIL) {
        mwu_printf(MSG_ERROR, "ERROR : get_priv_ioctl FAIL !");
        LEAVE();
        return WPS_STATUS_FAIL;
    }
    mwu_printf(DEBUG_WLAN, "ioctl_val %x, subioctl_val %d", ioctl_val,
               subioctl_val);

    memset(&iwr, 0, sizeof(iwr));
    strncpy(iwr.ifr_name, (char *) dev_name, IFNAMSIZ);
    iwr.u.data.pointer = (caddr_t) & bss_type;
    iwr.u.data.length = sizeof(bss_type);

    iwr.u.data.flags = subioctl_val;

    if (ioctl(ioctl_sock, ioctl_val, &iwr) < 0) {
        ret = WPS_STATUS_FAIL;
    }

    LEAVE();
    return ret;
}

/**
 *  @brief  Get BSS type (STA/uAP)
 *
 *  @param bss_type   A pointer to an integer
 *  @return           WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
int
wps_wlan_get_bss_type(int *bss_type)
{
    struct iwreq iwr;
    int ret = WPS_STATUS_SUCCESS;

    int ioctl_val, subioctl_val;

    ENTER();

    if (get_priv_ioctl("bssrole",
                &ioctl_val, &subioctl_val) == WPS_STATUS_FAIL) {
        mwu_printf(MSG_ERROR, "ERROR : get_priv_ioctl FAIL !");
        *bss_type = BSS_TYPE_STA;
        LEAVE();
        return WPS_STATUS_FAIL;
    }
    mwu_printf(DEBUG_WLAN, "ioctl_val %x, subioctl_val %d", ioctl_val,
            subioctl_val);

    memset(&iwr, 0, sizeof(iwr));
    strncpy(iwr.ifr_name, (char *) dev_name, IFNAMSIZ);
    iwr.u.data.pointer = (caddr_t) bss_type;

    iwr.u.data.flags = subioctl_val;

    if (ioctl(ioctl_sock, ioctl_val, &iwr) < 0) {
        perror("ioctl[bssrole]");
        ret = WPS_STATUS_FAIL;
     }

    if (ret == WPS_STATUS_FAIL) {
        *bss_type = BSS_TYPE_STA;
    }

    LEAVE();
    return ret;
}

/**
 *  @brief Set the BSS information into the registrar credential structure
 *  @param wps_s     A pointer to the WPS_DATA structure
 *  @param pwps_info A pointer to the PWPS_INFO structure
 *  @param bss       BSS information
 *
 *  @return          WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
int
load_cred_info(WPS_DATA * wps_s, PWPS_INFO pwps_info, bss_config_t * bss)
{
    int i, ret = WPS_STATUS_SUCCESS;
    CREDENTIAL_DATA *reg_cred = NULL;
    MESSAGE_ENROLLEE_REGISTRAR *enr_reg = NULL;
    u8 cipher = 0;

    ENTER();

    if (!bss) {
        mwu_printf(MSG_ERROR, "BSS info is not present");
        return WPS_STATUS_FAIL;
    }

    if ((wps_s->bss_type == BSS_TYPE_STA && pwps_info->role == WPS_REGISTRAR) ||
        (wps_s->bss_type == BSS_TYPE_UAP && pwps_info->role == WPS_ENROLLEE)) {
        enr_reg = &pwps_info->enrollee;
    } else {
        enr_reg = &pwps_info->registrar;
    }

    reg_cred = &enr_reg->cred_data[0];

    memset(reg_cred, 0, sizeof(CREDENTIAL_DATA));
    if (bss->ssid.ssid_len) {
        memcpy(reg_cred->ssid, bss->ssid.ssid, bss->ssid.ssid_len);
        reg_cred->ssid_length = bss->ssid.ssid_len;
        memcpy(wps_s->current_ssid.ssid, bss->ssid.ssid, bss->ssid.ssid_len);
        wps_s->current_ssid.ssid_len = bss->ssid.ssid_len;
        mwu_printf(DEBUG_INIT, "SSID '%s'", wps_s->current_ssid.ssid);
    }
    switch (bss->protocol) {
    case UAP_PROTO_OPEN:
        reg_cred->auth_type = AUTHENTICATION_TYPE_OPEN;
        reg_cred->encry_type = ENCRYPTION_TYPE_NONE;
        break;
    case UAP_PROTO_WEP_STATIC:
        reg_cred->encry_type = ENCRYPTION_TYPE_WEP;
        break;
    case UAP_PROTO_WPA:
        reg_cred->auth_type = AUTHENTICATION_TYPE_WPAPSK;
        break;
    case UAP_PROTO_WPA2:
        reg_cred->auth_type = AUTHENTICATION_TYPE_WPA2PSK;
        break;
    case UAP_PROTO_MIXED:
        reg_cred->auth_type = AUTHENTICATION_TYPE_WPA_MIXED;
        break;
    default:
        break;
    }

    if (reg_cred->encry_type == ENCRYPTION_TYPE_WEP) {
        /* For WEP */
        for (i = 0; i < 4; i++) {
            if (bss->wep_cfg[i].is_default) {
                /* Get the current default key settings */
                memcpy(reg_cred->network_key, bss->wep_cfg[i].key,
                       bss->wep_cfg[i].length);
                reg_cred->network_key_len = bss->wep_cfg[i].length;
                reg_cred->network_key_index = bss->wep_cfg[i].key_index + 1;
                break;
            }
        }
        mwu_printf(DEBUG_INIT, "NETWORK KEY INDEX = %d",
                   reg_cred->network_key_index - 1);
        mwu_hexdump(DEBUG_INIT, "NETWORK_KEY",
                    (const unsigned char *) reg_cred->network_key,
                    reg_cred->network_key_len);
        switch (bss->auth_mode) {
        case UAP_AUTH_MODE_OPEN:
            reg_cred->auth_type = AUTHENTICATION_TYPE_OPEN;
            break;
        case UAP_AUTH_MODE_SHARED:
            reg_cred->auth_type = AUTHENTICATION_TYPE_SHARED;
            break;
        default:
            break;
        }
    }
    mwu_printf(DEBUG_INIT, "AUTHENTICATION TYPE = 0x%04x",
               enr_reg->cred_data[0].auth_type);
    if ((reg_cred->auth_type == AUTHENTICATION_TYPE_WPAPSK) ||
        (reg_cred->auth_type == AUTHENTICATION_TYPE_WPA2PSK) ||
        (reg_cred->auth_type == AUTHENTICATION_TYPE_WPA_MIXED) ||
        (reg_cred->auth_type == AUTHENTICATION_TYPE_ALL)) {
        /* For WPA/WPA2 */

        if (reg_cred->auth_type == AUTHENTICATION_TYPE_WPAPSK)
            cipher = bss->wpa_cfg.pairwise_cipher_wpa;
        else
            cipher = bss->wpa_cfg.pairwise_cipher_wpa2;

        switch (cipher) {
        case UAP_CIPH_NONE:
            reg_cred->encry_type = ENCRYPTION_TYPE_NONE;
            break;
        case UAP_CIPH_TKIP:
            reg_cred->encry_type = ENCRYPTION_TYPE_TKIP;
            break;
        case UAP_CIPH_AES:
            reg_cred->encry_type = ENCRYPTION_TYPE_AES;
            break;
        case UAP_CIPH_AES_TKIP:
            reg_cred->encry_type = ENCRYPTION_TYPE_TKIP_AES_MIXED;
            break;
        default:
            break;
        }
        memcpy(reg_cred->network_key, bss->wpa_cfg.passphrase,
               bss->wpa_cfg.length);
        if (reg_cred->encry_type != ENCRYPTION_TYPE_WEP) {
            reg_cred->network_key_len = bss->wpa_cfg.length;
            reg_cred->network_key_index = 1;
        }
        mwu_printf(DEBUG_INIT, "NETWORK_KEY '%s'",
                   enr_reg->cred_data[0].network_key);
    }
    mwu_printf(DEBUG_INIT, "ENCRYPTION TYPE = 0x%04x",
               enr_reg->cred_data[0].encry_type);

    /* Both auth_type and encry_type can not be ZERO */
    if (!reg_cred->auth_type && !reg_cred->encry_type) {
        reg_cred->auth_type = AUTHENTICATION_TYPE_OPEN;
        mwu_printf(MSG_INFO,
                   "Incorrect Auth encryption types, Using open security\n");
    }
    if (enr_reg->version >= WPS_VERSION_2DOT0) {
        /* Dont go ahead if current security configuration is prohibited */
        switch (reg_cred->encry_type) {

        case ENCRYPTION_TYPE_TKIP:
            printf("WPA-TKIP is deprecated from WSC2.0.\n");
            ret = WPS_STATUS_FAIL;
            break;
        case ENCRYPTION_TYPE_WEP:
            printf("WEP deprecated from WSC2.0.\n");
            ret = WPS_STATUS_FAIL;
            break;
        default:
            break;
        }
    }

    LEAVE();
    return ret;
}

/**
 *  @brief Change AP configuration as per registrar credential structure
 *  @param wps_s     A pointer to the WPS_DATA structure
 *  @param pwps_info A pointer to the PWPS_INFO structure
 *
 *  @return          WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
int
wlan_change_ap_configuration(WPS_DATA * wps_s, PWPS_INFO pwps_info)
{
    MESSAGE_ENROLLEE_REGISTRAR *enr_reg = NULL;
    bss_config_t bss;
    int ret = WPS_STATUS_SUCCESS;
    CREDENTIAL_DATA *pCred = NULL;

    ENTER();

    apcmd_stop_bss();

    enr_reg = &pwps_info->registrar;
    pCred = &enr_reg->cred_data[0];
    mwu_printf(MSG_ERROR, "ap_conf: pCred: %p", pCred);

    memset(&bss, 0, sizeof(bss_config_t));

    /* Read current params for default values */
    if ((ret = apcmd_get_bss_config(&bss)) != WPS_STATUS_SUCCESS) {
        goto done;
    }

    wps_cred_to_bss_config(&bss, pCred, pwps_info->config_load_by_oob);

    mwu_printf(DEBUG_WLAN, "====== new credentials ======\n");

    mwu_printf(DEBUG_WLAN,
               "SSID:%s proto:0x%x pair_cip_wpa:0x%x pair_cip_wpa2:0x%x group_cip:0x%x\n",
               bss.ssid.ssid, bss.protocol, bss.wpa_cfg.pairwise_cipher_wpa,
               bss.wpa_cfg.pairwise_cipher_wpa2, bss.wpa_cfg.group_cipher);

    mwu_hexdump(DEBUG_WLAN, "Net key(PSK)",
                (u8 *) bss.wpa_cfg.passphrase, bss.wpa_cfg.length);

    /* Set updated params */
    ret = apcmd_set_bss_config(&bss);

    apcmd_start_bss();

  done:
    LEAVE();
    return ret;
}

/**
 *  @brief Reset AP configuration for Reversed Role or OOB.
 *
 *  @param pwps_info    A pointer to WPS_INFO structure
 *  @param wps_s        A pointer to WPS_DATA structure
 *  @return             WPS_STATUS_SUCCESS on success, WPS_STATUS_FAIL on fail
 */
int
wps_wlan_reset_ap_config(WPS_DATA * wps_s, PWPS_INFO pwps_info)
{
    int ret = WPS_STATUS_SUCCESS;
    bss_config_t bss;

    ENTER();
    memset(&bss, 0, sizeof(bss_config_t));
    /* Read current params for default values */
    if (wps_s->bss_type == BSS_TYPE_UAP) {
        if ((ret = apcmd_get_bss_config(&bss)) != WPS_STATUS_SUCCESS) {
            LEAVE();
            return WPS_STATUS_FAIL;
        }
    } else {
        LEAVE();
        return WPS_STATUS_SUCCESS;
    }
    if (wps_s->bss_type == BSS_TYPE_UAP && pwps_info->role == WPS_ENROLLEE) {

        mwu_printf(DEBUG_WPS_STATE,
                   "Delay loading new AP config and restart by 200 milli-seconds!");
        mwu_sleep(0, 200000);
        /* Update the new AP's settings received from the Registrar */
        wlan_change_ap_configuration(wps_s, pwps_info);

    } else if (pwps_info->wps_device_state == SC_NOT_CONFIGURED_STATE &&
               (wps_check_for_default_oob_settings(&bss) ==
                WPS_STATUS_SUCCESS)) {

        pwps_info->config_load_by_oob = WPS_SET;
        mwu_printf(DEBUG_WPS_STATE,
                   "Delay loading new AP config and restart by 200 milli-seconds!");
        mwu_sleep(0, 200000);
        /* Update the new Randomly generated AP setting after OOB reset from
           the Registrar */
        wlan_change_ap_configuration(wps_s, pwps_info);
    }

    LEAVE();
    return ret;
}

/**
 *  @brief Copy BSS configuration into PWPS_INFO  structure
 *  @param wps_s     A pointer to the WPS_DATA structure
 *  @param pwps_info A pointer to the PWPS_INFO structure
 *
 *  @return          WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
int
wps_load_uap_cred(WPS_DATA * wps_s, PWPS_INFO pwps_info)
{
    int ret;
    bss_config_t bss;

    ENTER();

    memset(&bss, 0, sizeof(bss_config_t));

    if ((ret = apcmd_get_bss_config(&bss)) != WPS_STATUS_SUCCESS) {
        goto done;
    }

    /* set credentials */
    ret = load_cred_info(wps_s, pwps_info, &bss);

  done:
    LEAVE();
    return ret;
}

/**
 *  @brief Converts a string to hex value
 *
 *  @param str      A pointer to the string
 *  @param raw      A pointer to the raw data buffer
 *  @return         Number of bytes read
 */
int
string2raw(char *str, unsigned char *raw)
{
    int len = (strlen(str) + 1) / 2;

    do {
        if (!isxdigit(*str)) {
            return -1;
        }
        *str = toupper(*str);
        *raw = CHAR2INT(*str) << 4;
        ++str;
        *str = toupper(*str);
        if (*str == '\0')
            break;
        *raw |= CHAR2INT(*str);
        ++raw;
    } while (*++str != '\0');
    return len;
}

/**
 *  @brief Copy Credential data into BSS configuration
 *  @param bss       A pointer to the bss_config_t structure
 *  @param pCred     A pointer to the CREDENTIAL_DATA structure
 *
 *  @return          WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
int
wps_cred_to_bss_config(bss_config_t * bss, PCREDENTIAL_DATA pCred,
                       u8 load_by_oob)
{
    /* Convert cred info to bss-config_t */
    int ret = WPS_STATUS_SUCCESS;
    unsigned char network_key[MAX_WEP_KEY_LEN];

    ENTER();

    /* Copy SSID */
    memset(bss->ssid.ssid, 0, MAX_SSID_LENGTH);
    bss->ssid.ssid_len = pCred->ssid_length;
    memcpy(bss->ssid.ssid, pCred->ssid, pCred->ssid_length);
    /* Enable SSID bcast ctrl */
    bss->bcast_ssid_ctl = 1;

    /* Copy Auth Type */
    switch (pCred->auth_type) {
    case AUTHENTICATION_TYPE_OPEN:
        bss->protocol = UAP_PROTO_OPEN;
        bss->auth_mode = UAP_AUTH_MODE_OPEN;
        break;
    case AUTHENTICATION_TYPE_WPAPSK:
        bss->protocol = UAP_PROTO_WPA;
        bss->auth_mode = UAP_AUTH_MODE_OPEN;
        break;
    case AUTHENTICATION_TYPE_SHARED:
        bss->protocol = UAP_PROTO_WEP_STATIC;
        bss->auth_mode = UAP_AUTH_MODE_SHARED;
        break;
    case AUTHENTICATION_TYPE_WPA:
        bss->protocol = UAP_PROTO_WPA;
        bss->auth_mode = UAP_AUTH_MODE_OPEN;
        break;
    case AUTHENTICATION_TYPE_WPA2:
        bss->protocol = UAP_PROTO_WPA2;
        bss->auth_mode = UAP_AUTH_MODE_OPEN;
        break;
    case AUTHENTICATION_TYPE_WPA2PSK:
        bss->protocol = UAP_PROTO_WPA2;
        bss->auth_mode = UAP_AUTH_MODE_OPEN;
        break;
    case AUTHENTICATION_TYPE_WPA_MIXED:
        bss->protocol = UAP_PROTO_MIXED;
        bss->auth_mode = UAP_AUTH_MODE_OPEN;
        break;
    default:
        if (pCred->auth_type == AUTHENTICATION_TYPE_WPA_MIXED &&
            load_by_oob == WPS_SET) {
            bss->protocol = UAP_PROTO_MIXED;
            bss->auth_mode = UAP_AUTH_MODE_OPEN;
        } else {
            mwu_printf(MSG_ERROR, " Unsupported AUTH type: 0x%x\n",
                       pCred->auth_type);
            ret = WPS_STATUS_FAIL;
        }
        break;
    }
    if (pCred->encry_type == ENCRYPTION_TYPE_WEP) {

        bss->protocol = UAP_PROTO_WEP_STATIC;
        bss->wep_cfg[pCred->network_key_index - 1].is_default = WPS_SET;

        if (pCred->network_key_len == 10 || pCred->network_key_len == 5) {
            bss->wep_cfg[pCred->network_key_index - 1].length = 5;
        } else if (pCred->network_key_len == 13 || pCred->network_key_len == 26) {
            bss->wep_cfg[pCred->network_key_index - 1].length = 13;
        }

        bss->wep_cfg[pCred->network_key_index - 1].key_index =
            pCred->network_key_index - 1;

        if (pCred->network_key_len == 26 || pCred->network_key_len == 10) {
            memset(network_key, 0, MAX_WEP_KEY_LEN);
            string2raw((char *) pCred->network_key, network_key);
        } else {
            memcpy(network_key, pCred->network_key, pCred->network_key_len);
        }

        memcpy(bss->wep_cfg[pCred->network_key_index - 1].key, network_key,
               pCred->network_key_len);
    } else {
        if (bss->protocol == UAP_PROTO_WPA) {
            bss->wpa_cfg.group_cipher = UAP_CIPH_TKIP;
            bss->wpa_cfg.pairwise_cipher_wpa = UAP_CIPH_TKIP;
        } else if (bss->protocol == UAP_PROTO_WPA2) {
            bss->wpa_cfg.group_cipher = UAP_CIPH_AES;
            bss->wpa_cfg.pairwise_cipher_wpa2 = UAP_CIPH_AES;
        } else if (bss->protocol == UAP_PROTO_MIXED) {
            bss->wpa_cfg.group_cipher = UAP_CIPH_TKIP;
            bss->wpa_cfg.pairwise_cipher_wpa2 = UAP_CIPH_AES;
            bss->wpa_cfg.pairwise_cipher_wpa = UAP_CIPH_TKIP;
        }

        /* Copy Network Key */
        bss->wpa_cfg.length = pCred->network_key_len;
        memcpy(bss->wpa_cfg.passphrase, pCred->network_key,
               pCred->network_key_len);
    }

    LEAVE();
    return ret;
}

/**
 *  @brief Copy Credential data into Enrollee/Registrar credential structure
 *  @param wps_s     A pointer to the WPS_DATA structure
 *  @param pwps_info A pointer to the global PWPS_INFO structure
 *
 *  @return          WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
int
wps_load_wsta_registrar_cred(WPS_DATA * wps_s, PWPS_INFO pwps_info)
{
    MESSAGE_ENROLLEE_REGISTRAR *enr_reg = NULL;
    bss_config_t bss;
    int ret = WPS_STATUS_SUCCESS;
    CREDENTIAL_DATA *pCred = NULL;

    ENTER();
    enr_reg = &pwps_info->registrar;
    pCred = &enr_reg->cred_data[0];
    memset(&bss, 0, sizeof(bss_config_t));

    wps_cred_to_bss_config(&bss, pCred, pwps_info->config_load_by_oob);

    /* set credentials */
    ret = load_cred_info(wps_s, pwps_info, &bss);
    LEAVE();

    return ret;
}

/**
 *  @brief Generate the PSK from passphrase and ssid generated.
 *  @param pwps_info A pointer to the PWPS_INFO structure
 *
 *  @return   None
 */
void
wlan_wifidir_generate_psk(PWPS_INFO pwps_info)
{
    int i, len = 0;
    u8 temp[32];
    CREDENTIAL_DATA *reg_cred = NULL;

    ENTER();
    reg_cred = &pwps_info->registrar.cred_data[0];

    /*
     * For WIFIDIR, we need to convert the passphrase to PSK.
     */
    pbkdf2_sha1((char *) reg_cred->network_key, (char *) reg_cred->ssid,
                reg_cred->ssid_length, 4096, temp, 32);
    for (i = 0; i < 32; ++i)
        len += sprintf((char *) reg_cred->network_key + len, "%02x",
                       (temp[i] & 0xff));
    reg_cred->network_key_len = 64;

    mwu_hexdump(DEBUG_WIFIDIR_DISCOVERY, "NETWORK KEY(PSK)",
                (u8 *) pwps_info->registrar.cred_data[0].network_key, 64);

    LEAVE();
}

/**
 *  @brief Creates a get bss_config request and sends to the driver
 *
 *  @param bss       A pointer to the bss_config_t structure
 *
 *  @return            WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
int
apcmd_get_bss_config(bss_config_t * bss)
{
    apcmdbuf_bss_configure *cmd_buf = NULL;
    bss_config_t *bss_temp = NULL;
    u8 *buf = NULL;
    u16 cmd_len;
    u16 buf_len;
    int ret = WPS_STATUS_SUCCESS;
    struct ifreq ifr;

    ENTER();

    /* Alloc buf for command */
    buf_len = MRVDRV_SIZE_OF_CMD_BUFFER;
    buf = (u8 *) malloc(buf_len);
    if (!buf) {
        mwu_printf(MSG_ERROR, "ERR:Cannot allocate buffer from command!");
        return WPS_STATUS_FAIL;
    }
    memset((char *) buf, 0, buf_len);
    memset(bss, 0, sizeof(bss_config_t));

    /* Locate headers */
    cmd_len = sizeof(apcmdbuf_bss_configure);
    cmd_buf = (apcmdbuf_bss_configure *) buf;
    bss_temp = (bss_config_t *) (buf + cmd_len);

    /* Fill the command buffer */
    cmd_buf->Action = ACTION_GET;

    /* Initialize the ifr structure */
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, (char *) dev_name, IFNAMSIZ);
    ifr.ifr_data = (void *) cmd_buf;
    /* Send the command */
    if (ioctl(ioctl_sock, UAP_BSS_CONFIG, &ifr)) {
        mwu_printf(MSG_ERROR, "ERR:UAP_BSS_CONFIG is not supported by %s: %s",
                   dev_name, strerror(errno));
        ret = WPS_STATUS_FAIL;
    } else {
        memcpy(bss, bss_temp, sizeof(bss_config_t));
    }

    free(buf);
    LEAVE();
    return ret;
}

/**
 *  @brief Creates a set bss_config request and sends to the driver
 *
 *  @param bss       A pointer to the bss_config_t structure
 *
 *  @return            WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
int
apcmd_set_bss_config(bss_config_t * bss)
{
    apcmdbuf_bss_configure *cmd_buf = NULL;
    bss_config_t *bss_temp = NULL;
    u8 *buf = NULL;
    u16 cmd_len;
    u16 buf_len;
    int ret = WPS_STATUS_SUCCESS;
    struct ifreq ifr;

    ENTER();

    /* Alloc buf for command */
    buf_len = MRVDRV_SIZE_OF_CMD_BUFFER;
    buf = (u8 *) malloc(buf_len);
    if (!buf) {
        mwu_printf(MSG_ERROR, "ERR:Cannot allocate buffer from command!");
        return WPS_STATUS_FAIL;
    }
    memset((char *) buf, 0, buf_len);

    /* Locate headers */
    cmd_len = sizeof(apcmdbuf_bss_configure);
    cmd_buf = (apcmdbuf_bss_configure *) buf;
    bss_temp = (bss_config_t *) (buf + cmd_len);
    memcpy(bss_temp, bss, sizeof(bss_config_t));

    /* Fill the command buffer */
    cmd_buf->Action = ACTION_SET;

    /* Initialize the ifr structure */
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, (char *) dev_name, IFNAMSIZ);
    ifr.ifr_data = (void *) cmd_buf;
    /* Send the command */
    if (ioctl(ioctl_sock, UAP_BSS_CONFIG, &ifr)) {
        mwu_printf(MSG_ERROR, "ERR:UAP_BSS_CONFIG is not supported by %s: %s",
                   dev_name, strerror(errno));
        ret = WPS_STATUS_FAIL;
    }

    free(buf);
    LEAVE();
    return ret;
}

#define UAP_BSS_CTRL                (SIOCDEVPRIVATE + 4)
/**
 *  @brief Starts the existing AP BSS .
 *
 *  @param wps_s      A pointer to the WPS_DATA structure
 *  @param pwps_info  A pointer to the PWPS_INFO structure
 *
 *  @return            WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
int
apcmd_start_bss(void)
{
    u32 data = 0;
    struct ifreq ifr;

    ENTER();

    /* Initialize the ifr structure */
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, (char *) dev_name, IFNAMSIZ);
    ifr.ifr_data = (void *) &data;
    /* Send the command */
    if (ioctl(ioctl_sock, UAP_BSS_CTRL, &ifr)) {
        mwu_printf(MSG_ERROR, "ERR:UAP_BSS_CTRL is not supported by %s",
                   dev_name);
        return WPS_STATUS_FAIL;
    }

    LEAVE();
    return WPS_STATUS_SUCCESS;
}

/**
 *  @brief Stops the existing AP BSS .
 *
 *  @param      None
 *
 *  @return     WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
int
apcmd_stop_bss(void)
{
    u32 data = 1;
    struct ifreq ifr;

    ENTER();

    /* Initialize the ifr structure */
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, (char *) dev_name, IFNAMSIZ);
    ifr.ifr_data = (void *) &data;
    /* Send the command */
    if (ioctl(ioctl_sock, UAP_BSS_CTRL, &ifr)) {
        mwu_printf(MSG_ERROR, "ERR:UAP_BSS_CTRL is not supported by %s: %s",
                   dev_name, strerror(errno));
        return WPS_STATUS_FAIL;
    }

    LEAVE();
    return WPS_STATUS_SUCCESS;
}

/**
 *  @brief  Get interface flags from WLAN driver (SIOCGIFFLAGS)
 *
 *  @param flags      Pointer to returned flags value
 *  @return           WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
int
wps_wlan_get_ifflags(int *flags)
{
    struct ifreq ifr;

    ENTER();

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, (char *) dev_name, IFNAMSIZ);
    if (ioctl(ioctl_sock, SIOCGIFFLAGS, (caddr_t) & ifr) < 0) {
        perror("ioctl[SIOCGIFFLAGS]");
        LEAVE();
        return WPS_STATUS_FAIL;
    }
    *flags = ifr.ifr_flags & 0xffff;

    LEAVE();
    return WPS_STATUS_SUCCESS;
}

/**
 *  @brief  Set interface flags to WLAN driver (SIOCSIFFLAGS)
 *
 *  @param flags      New value for flags
 *  @return           WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
int
wps_wlan_set_ifflags(int flags)
{
    struct ifreq ifr;

    ENTER();

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, (char *) dev_name, IFNAMSIZ);
    ifr.ifr_flags = flags & 0xffff;
    if (ioctl(ioctl_sock, SIOCSIFFLAGS, (caddr_t) & ifr) < 0) {
        perror("SIOCSIFFLAGS");
        LEAVE();
        return WPS_STATUS_FAIL;
    }

    LEAVE();
    return WPS_STATUS_SUCCESS;
}

/**
 *  @brief  Set uap command "sta_deauth_ext" to WLAN driver
 *
 *  @return           WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
int
wps_wlan_set_ap_deauth(u8* mac, u16 reason_code)
{
    deauth_param param;
    struct ifreq ifr;
    int sockfd;
    int ret;

    ENTER();

    memset(&param, 0, sizeof(deauth_param));

    param.reason_code = reason_code;
    memcpy(&param.mac_addr, mac, ETH_ALEN);

    strncpy(ifr.ifr_ifrn.ifrn_name, (char *)dev_name, IFNAMSIZ);
    ifr.ifr_ifru.ifru_data = (void *) &param;
    /* Perform ioctl */
    if (ioctl(ioctl_sock, UAP_STA_DEAUTH, &ifr)) {
        perror("UAP_STA_DEAUTH ioctl failed.");
        LEAVE();
        return WPS_STATUS_FAIL;
    }
    printf("Station deauth successful\n");
    LEAVE();
    return WPS_STATUS_SUCCESS;

}


/**
 *  @brief  Set private ioctl command "deauth" to WLAN driver
 *
 *  @return           WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
int
wps_wlan_set_deauth(void)
{
    struct iwreq iwr;
    int ret = WPS_STATUS_SUCCESS;
    int ioctl_val, subioctl_val;

    ENTER();

    if (get_priv_ioctl("deauth", &ioctl_val, &subioctl_val) == WPS_STATUS_FAIL) {
        mwu_printf(MSG_ERROR, "ERROR : get_priv_ioctl FAIL !");
        LEAVE();
        return WPS_STATUS_FAIL;
    }
    mwu_printf(DEBUG_WLAN, "ioctl_val %x, subioctl_val %d", ioctl_val,
               subioctl_val);

    memset(&iwr, 0, sizeof(iwr));
    strncpy(iwr.ifr_name, (char *) dev_name, IFNAMSIZ);

    iwr.u.data.pointer = NULL;
    iwr.u.data.flags = subioctl_val;

    if (ioctl(ioctl_sock, ioctl_val, &iwr) < 0) {
        perror("ioctl[deauth]");
        ret = WPS_STATUS_FAIL;
    }

    LEAVE();
    return ret;
}

/**
 *  @brief  Set private ioctl command "deauth" to WLAN driver
 *
 *  @return           WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
int
wps_wlan_set_disassoc(void)
{
    struct iwreq iwr;
    int ret = WPS_STATUS_SUCCESS;
    int ioctl_val, subioctl_val;

    ENTER();

    if (get_priv_ioctl("disassoc",
                       &ioctl_val, &subioctl_val) == WPS_STATUS_FAIL) {
        mwu_printf(MSG_ERROR, "ERROR : get_priv_ioctl FAIL !");
        LEAVE();
        return WPS_STATUS_FAIL;
    }
    mwu_printf(DEBUG_WLAN, "ioctl_val %x, subioctl_val %d", ioctl_val,
               subioctl_val);

    memset(&iwr, 0, sizeof(iwr));
    strncpy(iwr.ifr_name, (char *) dev_name, IFNAMSIZ);

    iwr.u.data.pointer = NULL;
    iwr.u.data.flags = subioctl_val;

    if (ioctl(ioctl_sock, ioctl_val, &iwr) < 0) {
        perror("ioctl[deauth]");
        ret = WPS_STATUS_FAIL;
    }

    LEAVE();
    return ret;
}

/**
 *  @brief  Generates OOB SSID string from device mac address and stroes it in PWPS_INFO structure
 *
 *  @param my_mac_addr      A pointer to device MAC address array
 *  @return                 A pointer to OOB SSID.
 */
void
wps_wlan_get_OOB_ap_ssid(char *wps_oob_ssid, u8 * my_mac_addr)
{

    u8 mac_str[10];

    ENTER();

    memset(mac_str, 0, 10);
    snprintf((char *) mac_str, 5, "%02x%02x", my_mac_addr[4], my_mac_addr[5]);
    strncpy(wps_oob_ssid, "MarvellMicroAP_", 15);
    strncat(wps_oob_ssid, (char *) mac_str, 5);
    LEAVE();
    return;

}

/**
 *  @brief Resets AP configuration to default OOB settings
 *
 *  @param wps_s      A pointer to the WPS_DATA structure
 *  @param pwps_info  A pointer to the PWPS_INFO structure
 *  @return            WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
char *
wps_wlan_proto_to_security_string(int proto)
{
    switch (proto) {
    case UAP_PROTO_OPEN:
        return "Open Security";
        break;
    case UAP_PROTO_WEP_STATIC:
        return "Static WEP";
        break;
    case UAP_PROTO_WPA:
        return "WPA";
        break;
    case UAP_PROTO_WPA2:
        return "WPA2";
        break;
    case UAP_PROTO_MIXED:
        return "WPA/WPA2 Mixed mode";
        break;
    }
    return "Unknown Security";
}

/**
 *  @brief Resets AP configuration to default OOB settings
 *
 *  @param wps_s      A pointer to the WPS_DATA structure
 *  @param pwps_info  A pointer to the PWPS_INFO structure
 *  @return            WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
int
wps_wlan_reset_ap_config_to_OOB(WPS_DATA * wps_s, PWPS_INFO pwps_info)
{
    struct iwreq iwr;
    int ret = WPS_STATUS_SUCCESS;
    bss_config_t *bss;
    u8 wps_oob_ssid[33];
    ENTER();

#if 0
    // TODO: Get Current associated STA list and send deauth.
    /* Deauthenticate all connected stations */
    if (wps_s->current_ssid.mode == IEEE80211_MODE_INFRA)
        wps_wlan_set_deauth();
#endif
    memset(&iwr, 0, sizeof(iwr));
    strncpy(iwr.ifr_name, (char *) dev_name, IFNAMSIZ);

    memset(wps_oob_ssid, 0, sizeof(wps_oob_ssid));
    wps_wlan_get_OOB_ap_ssid((char *) wps_oob_ssid, wps_s->my_mac_addr);

    pwps_info->oob_ssid_len = strlen((char *) wps_oob_ssid);
    memcpy(pwps_info->oob_ssid, wps_oob_ssid, pwps_info->oob_ssid_len);

    bss = (bss_config_t *) malloc(sizeof(bss_config_t));
    memset(bss, 0, sizeof(bss_config_t));
    /* SSID update to credentials & Driver, FW. */

    apcmd_stop_bss();

    /* Read current params for default values */
    if ((ret = apcmd_get_bss_config(bss)) != WPS_STATUS_SUCCESS) {
        goto done;
    }

    bss->ssid.ssid_len = pwps_info->oob_ssid_len =
        strlen((char *) pwps_info->oob_ssid);
    memcpy(&bss->ssid.ssid, pwps_info->oob_ssid, bss->ssid.ssid_len);
    bss->protocol = UAP_PROTO_OPEN;

    mwu_printf(DEBUG_WLAN, "====== WPS OOB credentials ======\n");

    mwu_printf(DEBUG_WLAN, "SSID:%s \nSecurity:%s \n", bss->ssid.ssid,
               wps_wlan_proto_to_security_string(bss->protocol));

    /* Set updated params */
    ret = apcmd_set_bss_config(bss);
    apcmd_start_bss();

  done:
    free(bss);
    LEAVE();

    return ret;
}

/**
 *  @brief Generate Random PSK
 *
 *  @param dest      A pointer to the destination character array
 *  @param length    Destination string legth.
 *  @return          None.
 *  */
void
wps_wlan_generate_random_psk(char *dest, unsigned short len)
{
    int i;
    char charset[] = "0123456789ABCDEFabcdef";

    ENTER();
    srand(time(0));
    for (i = 0; i < len; ++i) {
        dest[i] = charset[rand() % (sizeof(charset) - 1)];
    }
    LEAVE();
    return;
}

 /**
 *  @brief Creates new AP configuration after AP has been reset to OOB settings
 *
 *  @param wps_s      A pointer to the WPS_DATA structure
 *  @param pwps_info  A pointer to the PWPS_INFO structure
 *  @return            WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
int
wps_wlan_create_ap_config_after_OOB(WPS_DATA * wps_s, PWPS_INFO pwps_info)
{
    struct iwreq iwr;
    int ret = WPS_STATUS_SUCCESS;
    bss_config_t *bss = NULL;
    ENTER();

    memset(&iwr, 0, sizeof(iwr));
    strncpy(iwr.ifr_name, (char *) dev_name, IFNAMSIZ);

    bss = (bss_config_t *) malloc(sizeof(bss_config_t));
    memset(bss, 0, sizeof(bss_config_t));
    /*
     * SSID update to credentials & Driver, FW.
     */

    /* Read current params for default values */
    if ((ret = apcmd_get_bss_config(bss)) != WPS_STATUS_SUCCESS) {
        goto done;
    }

    memset(pwps_info->oob_ssid, 0, sizeof(pwps_info->oob_ssid));
    wps_wlan_get_OOB_ap_ssid((char *) pwps_info->oob_ssid, wps_s->my_mac_addr);
    bss->ssid.ssid_len = pwps_info->oob_ssid_len =
        strlen((char *) pwps_info->oob_ssid);
    if (bss->ssid.ssid_len <= MAX_SSID_LENGTH) {
        memcpy(&bss->ssid.ssid, pwps_info->oob_ssid, bss->ssid.ssid_len);
    } else {
        ret = WPS_STATUS_FAIL;
        goto done;
    }


    bss->auth_mode = UAP_AUTH_MODE_OPEN;
    bss->protocol = UAP_PROTO_MIXED;

    bss->wpa_cfg.length = WPA_PASSPHARE_LENGTH;
    wps_wlan_generate_random_psk((char *) bss->wpa_cfg.passphrase,
                                 bss->wpa_cfg.length);

    bss->wpa_cfg.pairwise_cipher_wpa2 = UAP_CIPH_AES;
    bss->wpa_cfg.pairwise_cipher_wpa = UAP_CIPH_TKIP;

    if (bss->protocol == UAP_PROTO_WPA) {
        bss->wpa_cfg.group_cipher = UAP_CIPH_TKIP;
    } else if (bss->protocol == UAP_PROTO_WPA2) {
        bss->wpa_cfg.group_cipher = UAP_CIPH_AES;
    } else if (bss->protocol == UAP_PROTO_MIXED) {
        bss->wpa_cfg.group_cipher = UAP_CIPH_TKIP;
    }

    mwu_printf(DEBUG_WLAN, "====== WPS OOB credentials ======\n");

    mwu_printf(DEBUG_WLAN,
               "SSID: %s proto: 0x%x pair_cip_wpa: 0x%x pair_cip_wpa2: 0x%xi group_cip: 0x%x\n",
               bss->ssid.ssid, bss->protocol, bss->wpa_cfg.pairwise_cipher_wpa,
               bss->wpa_cfg.pairwise_cipher_wpa2, bss->wpa_cfg.group_cipher);
    mwu_hexdump(DEBUG_WLAN, "Random Generated PSK: ", bss->wpa_cfg.passphrase,
                bss->wpa_cfg.length);

    if (load_cred_info(wps_s, pwps_info, bss) != WPS_STATUS_SUCCESS) {

        mwu_printf(MSG_INFO,
                   "Loading new BSS configuration into registrar failed!!!");
        ret = WPS_STATUS_FAIL;
    }
  done:
    if (bss) {
        free(bss);
    }
    LEAVE();

    return ret;
}

/**
 *  @brief Checks if APs configuration is same as default OOB settings
 *
 *  @param bss        A pointer to bss_config_t structure.
 *  @return            WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
int
wps_check_for_default_oob_settings(bss_config_t * bss)
{
    int ret = WPS_STATUS_FAIL;
    u8 wps_oob_ssid[33];

    ENTER();
    memset(wps_oob_ssid, 0, sizeof(wps_oob_ssid));
    wps_wlan_get_OOB_ap_ssid((char *) wps_oob_ssid, bss->mac_addr);

    if (strlen((char *) wps_oob_ssid) == bss->ssid.ssid_len) {
        if ((memcmp(wps_oob_ssid, bss->ssid.ssid, bss->ssid.ssid_len) == 0)
            && (bss->protocol == UAP_PROTO_OPEN)) {
            LEAVE();
            return WPS_STATUS_SUCCESS;
        }
    }
    LEAVE();
    return ret;
}

/* Really, we would like to register this function for the events that he
 * handles.  But for now, we extern it.
 */
void mwpamod_ap_driver_event(char *ifname, u8 *buffer, u16 size);

/**
 *  @brief  Wlan event parser for FW events
 *  @param context    Pointer to Context
 *  @param wps_s      Pointer to WPS global structure
 *  @param evt_buffer Pointer to Event buffer
 *  @param evt_len    Event Length
 *
 *  @return           None
 */
void
wps_wlan_event_parser(void *context, WPS_DATA * wps_s,
                      char *evt_buffer, int evt_len)
{
    event_header *event = NULL;
    PWPS_INFO pwps_info;
    u32 event_id = 0;

    ENTER();
    pwps_info = (PWPS_INFO) context;

    /* event parser */
    event = (event_header *) evt_buffer;

    /* A recent drop of the wlan driver
     * (SD-UAPSTA-BT-FM-8787-LINUX2632-PXA955-14.65.7.p30-M2614164_AX-GPL)
     * changed the layout of the event returned by the driver.  Now, the event
     * may or may not be preceded by the interface name from which the driver
     * was emitted.  The code below is poached from the mlanevent source code
     * that ships with the driver.  Unfortunately, this change pretty much
     * destroys the flexibility of the event structure.
     */
    memcpy(&event_id, evt_buffer, sizeof(event_id));
    if (((event_id & 0xFF000000) == 0x80000000) ||
        ((event_id & 0xFF000000) == 0)) {
        event = (event_header *)evt_buffer;
    } else {
        event = (event_header *)(evt_buffer + IFNAMSIZ);
    }

    switch (event->event_id) {
    case EV_ID_WIFIDIR_GENERIC:
        wifidir_state_machine(event->event_data, evt_len - sizeof(event->event_id),
                              WIFIDIR_EVENTS_DRIVER);
        break;
    case EV_ID_UAP_EV_WMM_STATUS_CHANGE:
    case EV_ID_UAP_EV_ID_STA_DEAUTH:
    case EV_ID_UAP_EV_ID_STA_ASSOC:
    case EV_ID_UAP_EV_ID_BSS_START:
    case EV_ID_UAP_EV_ID_DEBUG:
    case EV_ID_UAP_EV_BSS_IDLE:
    case EV_ID_UAP_EV_BSS_ACTIVE:
    case EV_ID_WIFIDIR_SERVICE_DISCOVERY:
    case EV_ID_UAP_EV_RSN_CONNECT:
        mwpamod_ap_driver_event(wps_s->ifname, (u8 *)event, evt_len);
        break;

    default:
        mwu_printf(MSG_INFO, "Unhandled event %s (%d)\n",
                   nl_event_id_to_str(event->event_id),
                   event->event_id);
        break;
    }
    LEAVE();
}

/**
 *  @brief  Update SET/CLEAR the group formation bit for GO.
 *  @param  set_clear  Set or clear the bit.
 *
 *  @return  WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
int
wps_wlan_update_group_formation_config(int set_clear)
{
    int i, ret = WPS_STATUS_SUCCESS;
    u8 buf[MRVDRV_SIZE_OF_CMD_BUFFER];
    short ie_index = 0;
    u16 len = 0, ie_len_wifidir_org, ie_len_wifidir, ie_len_wps;
    u8 *wifidir_ptr;
    tlvbuf_custom_ie *tlv = NULL;
    custom_ie *ie_ptr = NULL;
    ENTER();

    memset(buf, 0, MRVDRV_SIZE_OF_CMD_BUFFER);
    ret = wifidir_ie_config((u16 *) & ie_index, 0, 0, buf);

    if (ret == WPS_STATUS_SUCCESS && (ie_index < (MAX_MGMT_IE_INDEX -1))) {
        tlv = (tlvbuf_custom_ie *) buf;

        mwu_hexdump(DEBUG_WLAN, "IE Buffer", (unsigned char *) buf,
                    tlv->length);

        if (tlv->tag == MRVL_MGMT_IE_LIST_TLV_ID) {
            ie_ptr = (custom_ie *) (tlv->ie_data);
            /* Goto appropriate Ie Index */
            for (i = 0; i < ie_index; i++) {
                ie_ptr = (custom_ie *) ((u8 *) ie_ptr + sizeof(custom_ie) +
                                        ie_ptr->ie_length);
            }
            ie_len_wifidir_org = ie_len_wifidir = ie_ptr->ie_length;
            wifidir_ptr = ie_ptr->ie_buffer;
            while (ie_len_wifidir > WIFIDIR_IE_HEADER_LEN) {
                memcpy(&len, wifidir_ptr + 1, sizeof(u16));
                len = wlan_le16_to_cpu(len);
                /* check capability type */
                if (*wifidir_ptr == TLV_TYPE_WIFIDIR_CAPABILITY) {
                    tlvbuf_wifidir_capability *wifidir_tlv =
                        (tlvbuf_wifidir_capability *) wifidir_ptr;
                    /* set or clear the bit correctly */
                    if (set_clear)
                        wifidir_tlv->group_capability |=
                            WIFIDIR_GROUP_FORMATION_OR_MASK;
                    else
                        wifidir_tlv->group_capability &=
                            WIFIDIR_GROUP_FORMATION_AND_MASK;
                    break;
                }
                wifidir_ptr += len + WIFIDIR_IE_HEADER_LEN;
                ie_len_wifidir -= len + WIFIDIR_IE_HEADER_LEN;
            }

            /* WPS length */
            ie_ptr =
                (custom_ie *) ((u8 *) ie_ptr + sizeof(custom_ie) +
                               ie_len_wifidir_org);
            ie_len_wps = ie_ptr->ie_length;

            /* Update New IE now */
            ret =
                wifidir_ie_config((u16 *) & ie_index, ie_len_wifidir_org, ie_len_wps,
                              buf);
            if (ret != WPS_STATUS_SUCCESS) {
                printf("Failed to update group formation bit.\n");
            }
        }
    }
    LEAVE();
    return ret;
}

/**
 *  @brief Performs the ioctl operation to set mac address to kernel
 *
 *  @param  mac mac address to set
 *  @return WPS_STATUS_SUCCESS or WPS_STATUS_FAIL
 */
int
wlan_set_intended_mac_addr(u8 mac[ETH_ALEN])
{
    struct ifreq ifr;

    ENTER();

    /* Initialize the ifr structure */
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, (char *) dev_name, IFNAMSIZ);
    ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
    memcpy(ifr.ifr_hwaddr.sa_data, mac, ETH_ALEN);

    /* Send the command */
    if (ioctl(ioctl_sock, SIOCSIFHWADDR, &ifr)) {
        printf("ERR:Set HW address.\n");
        LEAVE();
        return WPS_STATUS_FAIL;
    }

    LEAVE();
    mwu_printf(DEBUG_WLAN, "HW address setting successful\n");
    return WPS_STATUS_SUCCESS;
}

/*
 * Bit 0 : Assoc Req
 * Bit 1 : Assoc Resp
 * Bit 2 : ReAssoc Req
 * Bit 3 : ReAssoc Resp
 * Bit 4 : Probe Req
 * Bit 5 : Probe Resp
 * Bit 8 : Beacon
 */
/** Mask for Assoc request frame */
#define MGMT_MASK_ASSOC_REQ  0x01
/** Mask for ReAssoc request frame */
#define MGMT_MASK_REASSOC_REQ  0x04
/** Mask for Assoc response frame */
#define MGMT_MASK_ASSOC_RESP  0x02
/** Mask for ReAssoc response frame */
#define MGMT_MASK_REASSOC_RESP  0x08
/** Mask for probe request frame */
#define MGMT_MASK_PROBE_REQ  0x10
/** Mask for probe response frame */
#define MGMT_MASK_PROBE_RESP 0x20
/** Mask for beacon frame */
#define MGMT_MASK_BEACON 0x100
/** Mask to clear previous settings */
#define MGMT_MASK_CLEAR 0x000

/**
 *  @brief Process and send ie config command
 *  @param flag   FLAG: Set/clear WPS IE
 *  @param ie_index  A pointer to the IE buffer index
 *  @return            WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
int
wps_wlan_ie_config(int flag, short *pie_index)
{
    u8 *buf, *pos;
    IEEEtypes_Header_t *ptlv_header = NULL;
    struct iwreq iwr;
    int i, ret = WPS_STATUS_SUCCESS;
    u16 ie_len = 0, mgmt_subtype_mask = 0;
    tlvbuf_custom_ie *tlv = NULL;
    custom_ie *ie_ptr = NULL;
    u8 action = 0x00, type = 0x00;

    ENTER();

    type = flag & 0x0f;
    action = flag & 0xf0;

    buf = (u8 *) malloc(MRVDRV_SIZE_OF_CMD_BUFFER);
    if (buf == NULL)
        return WPS_STATUS_FAIL;

    memset(buf, 0, MRVDRV_SIZE_OF_CMD_BUFFER);

    tlv = (tlvbuf_custom_ie *) buf;
    tlv->tag = MRVL_MGMT_IE_LIST_TLV_ID;

    /* Locate headers */
    ie_ptr = (custom_ie *) (tlv->ie_data);
    /* Set TLV fields */

    if (action == SET_WPS_IE) {
        /* Set WPS IE */
        pos = ie_ptr->ie_buffer;
        ptlv_header = (IEEEtypes_Header_t *) pos;
        pos += sizeof(IEEEtypes_Header_t);

        if (type == WPS_AP_SESSION_INACTIVE) {
            ie_len = wps_ap_beacon_prepare(WPS_END_REG_DISCOVERY_PHASE, pos);
            mgmt_subtype_mask = MGMT_MASK_BEACON;
        } else if (type == WPS_AP_SESSION_ACTIVE) {
            ie_len = wps_ap_beacon_prepare(WPS_START_REG_DISCOVERY_PHASE, pos);
            mgmt_subtype_mask = MGMT_MASK_BEACON;
        } else if (type == WPS_STA_SESSION_ACTIVE) {
            ie_len = wps_sta_assoc_request_prepare(pos);
            mgmt_subtype_mask = MGMT_MASK_ASSOC_REQ | MGMT_MASK_REASSOC_REQ;
        } else if (type == WPS_AP_SESSION_ACTIVE_AR) {
            ie_len =
                wps_ap_assoc_response_prepare(WPS_START_REG_DISCOVERY_PHASE,
                                              pos);
            mgmt_subtype_mask = MGMT_MASK_ASSOC_RESP | MGMT_MASK_REASSOC_RESP;
        } else if (type == WPS_AP_SESSION_INACTIVE_AR) {
            ie_len =
                wps_ap_assoc_response_prepare(WPS_END_REG_DISCOVERY_PHASE, pos);
            mgmt_subtype_mask = MGMT_MASK_ASSOC_RESP | MGMT_MASK_REASSOC_RESP;
        } else {
            mwu_printf(MSG_ERROR, "ie_config: unknown SET type!\n");
            ret = WPS_STATUS_FAIL;
            goto _exit_;
        }

        ptlv_header->Type = WPS_IE;
        ptlv_header->Len = ie_len;
        ie_ptr->mgmt_subtype_mask = mgmt_subtype_mask;
        tlv->length = sizeof(custom_ie) + sizeof(IEEEtypes_Header_t) + ie_len;
        ie_ptr->ie_length = sizeof(IEEEtypes_Header_t) + ie_len;
        ie_ptr->ie_index = *pie_index;

        if (type == WPS_AP_SESSION_INACTIVE || type == WPS_AP_SESSION_ACTIVE) {
            ie_ptr = (custom_ie *) (((u8 *) ie_ptr) + sizeof(custom_ie) +
                                    sizeof(IEEEtypes_Header_t) + ie_len);
            pos = ie_ptr->ie_buffer;
            ptlv_header = (IEEEtypes_Header_t *) pos;
            pos += sizeof(IEEEtypes_Header_t);

            if (type == WPS_AP_SESSION_INACTIVE) {
                ie_len =
                    wps_ap_probe_response_prepare(WPS_END_REG_DISCOVERY_PHASE,
                                                  pos);
                mgmt_subtype_mask = MGMT_MASK_PROBE_RESP;
            } else if (type == WPS_AP_SESSION_ACTIVE) {
                ie_len =
                    wps_ap_probe_response_prepare(WPS_START_REG_DISCOVERY_PHASE,
                                                  pos);
                mgmt_subtype_mask = MGMT_MASK_PROBE_RESP;
            }

            ptlv_header->Type = WPS_IE;
            ptlv_header->Len = ie_len;
            ie_ptr->mgmt_subtype_mask = mgmt_subtype_mask;
            tlv->length +=
                sizeof(custom_ie) + sizeof(IEEEtypes_Header_t) + ie_len;
            ie_ptr->ie_length = sizeof(IEEEtypes_Header_t) + ie_len;
            ie_ptr->ie_index = *pie_index + 1;
        }

    } else if (action == CLEAR_WPS_IE) {
        if (*pie_index < 0) {
            mwu_printf(DEBUG_WLAN, "CLEAR IE number should be valid!!\n");
            return WPS_STATUS_FAIL;
        }

        /* Clear WPS IE */
        pos = ie_ptr->ie_buffer;
        ie_ptr->mgmt_subtype_mask = MGMT_MASK_CLEAR;
        ie_ptr->ie_length = 0;
        ie_ptr->ie_index = *pie_index;

        if (ie_ptr->ie_index < 2)
            ie_ptr->mgmt_subtype_mask = 0xFFFF;

        if ((type == CLEAR_WPS_IE) && (*pie_index < (MAX_MGMT_IE_INDEX - 1))) {
            ie_ptr = (custom_ie *) (((u8 *) ie_ptr) + sizeof(custom_ie));
            ie_ptr->mgmt_subtype_mask = MGMT_MASK_CLEAR;
            ie_ptr->ie_length = 0;
            ie_ptr->ie_index = *pie_index + 1;
            if (ie_ptr->ie_index < 2)
                ie_ptr->mgmt_subtype_mask = 0xFFFF;
            tlv->length = 2 * sizeof(custom_ie);
        } else
            tlv->length = sizeof(custom_ie);

    } else {
        /* Get WPS IE */
        tlv->length = 0;
    }

    mwu_hexdump(DEBUG_WLAN, "MGMT_IE", (const unsigned char *) buf,
                tlv->length + TLV_HEADER_SIZE);

    memset(&iwr, 0, sizeof(iwr));
    strncpy(iwr.ifr_name, (char *) dev_name, IFNAMSIZ);
    iwr.u.data.pointer = (void *) buf;
    iwr.u.data.length = tlv->length + TLV_HEADER_SIZE;
    iwr.u.data.flags = 0;

    if (ioctl(ioctl_sock, WLAN_CUSTOM_IE, &iwr)) {
        perror("ioctl[WLAN_CUSTOM_IE]");
        mwu_printf(MSG_ERROR, "Failed to set/get/clear the IE buffer");
        ret = WPS_STATUS_FAIL;
        goto _exit_;
    }

    if (action == GET_WPS_IE) {
        /* Get the free IE buffer index number */
        tlv = (tlvbuf_custom_ie *) buf;
        *pie_index = -1;
        ie_ptr = (custom_ie *) (tlv->ie_data);
        for (i = 0; i < MAX_MGMT_IE_INDEX; i++) {
            if (ie_ptr->mgmt_subtype_mask == 0) {
                *pie_index = ie_ptr->ie_index;
                break;
            }
            if (i < (MAX_MGMT_IE_INDEX - 1))
                ie_ptr =
                    (custom_ie *) ((u8 *) ie_ptr + sizeof(custom_ie) +
                                   ie_ptr->ie_length);
        }
        if (*pie_index == -1 || *pie_index > (MAX_MGMT_IE_INDEX - 2)) {
            /* Two buffers are needed, hence check for > 2 */
            mwu_printf(MSG_ERROR, "No free IE buffer available\n");
            ret = WPS_STATUS_FAIL;
        }
    }
  _exit_:
    if (buf)
        free(buf);

    LEAVE();
    return ret;
}

/**
 *  @brief Process and send ie config command for (re)assoc request.
 *  @param flag   FLAG: Set/clear WPS IE
 *  @param index  A pointer to the IE buffer index
 *  @return            WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
int
wps_wlan_assoc_request_ie_config(int flag, short *pie_index)
{
    u8 *buf, *pos;
    IEEEtypes_Header_t *ptlv_header = NULL;
    struct iwreq iwr;
    int i, ret = WPS_STATUS_SUCCESS;
    u16 ie_len = 0, mgmt_subtype_mask = 0;
    tlvbuf_custom_ie *tlv = NULL;
    custom_ie *ie_ptr = NULL;
    u8 action = 0x00, type = 0x00;

    ENTER();

    type = flag & 0x0f;
    action = flag & 0xf0;

    buf = (u8 *) malloc(MRVDRV_SIZE_OF_CMD_BUFFER);
    if (buf == NULL)
        return WPS_STATUS_FAIL;

    memset(buf, 0, MRVDRV_SIZE_OF_CMD_BUFFER);

    tlv = (tlvbuf_custom_ie *) buf;
    tlv->tag = MRVL_MGMT_IE_LIST_TLV_ID;

    /* Locate headers */
    ie_ptr = (custom_ie *) (tlv->ie_data);
    /* Set TLV fields */

    if (action == SET_WPS_IE) {
        /* Set WPS IE */
        pos = ie_ptr->ie_buffer;
        ptlv_header = (IEEEtypes_Header_t *) pos;
        pos += sizeof(IEEEtypes_Header_t);
        ie_len = wps_sta_assoc_request_prepare(pos);
        mgmt_subtype_mask = MGMT_MASK_ASSOC_REQ | MGMT_MASK_REASSOC_REQ;

        ptlv_header->Type = WPS_IE;
        ptlv_header->Len = ie_len;
        ie_ptr->mgmt_subtype_mask = mgmt_subtype_mask;
        tlv->length += sizeof(custom_ie) + sizeof(IEEEtypes_Header_t) + ie_len;
        ie_ptr->ie_length = sizeof(IEEEtypes_Header_t) + ie_len;
        ie_ptr->ie_index = *pie_index;

    } else if (action == CLEAR_WPS_IE) {
        /* Clear WPS IE */
        pos = ie_ptr->ie_buffer;
        ie_ptr->mgmt_subtype_mask = MGMT_MASK_CLEAR;
        ie_ptr->ie_length = 0;
        ie_ptr->ie_index = *pie_index;   /* index */
        if (ie_ptr->ie_index < 2)
            ie_ptr->mgmt_subtype_mask = 0xFFFF;
        tlv->length = sizeof(custom_ie);

    } else {
        /* Get WPS IE */
        tlv->length = 0;
    }

    mwu_hexdump(DEBUG_WLAN, "MGMT_IE", (const unsigned char *) buf,
                tlv->length + TLV_HEADER_SIZE);

    memset(&iwr, 0, sizeof(iwr));
    strncpy(iwr.ifr_name, (char *) dev_name, IFNAMSIZ);
    iwr.u.data.pointer = (void *) buf;
    iwr.u.data.length = tlv->length + TLV_HEADER_SIZE;
    iwr.u.data.flags = 0;

    if (ioctl(ioctl_sock, WLAN_CUSTOM_IE, &iwr)) {
        perror("ioctl[WLAN_CUSTOM_IE]");
        mwu_printf(MSG_ERROR, "Failed to set/get/clear the IE buffer");
        ret = WPS_STATUS_FAIL;
        goto _exitp_;
    }

    if (flag == GET_WPS_IE) {
        /* Get the free IE buffer index number */
        tlv = (tlvbuf_custom_ie *) buf;
        *pie_index = -1;
        ie_ptr = (custom_ie *) (tlv->ie_data);
        for (i = 0; i < MAX_MGMT_IE_INDEX; i++) {
            if (ie_ptr->mgmt_subtype_mask == 0) {
                *pie_index = ie_ptr->ie_index;
                break;
            }
            if (i < (MAX_MGMT_IE_INDEX - 1))
                ie_ptr =
                    (custom_ie *) ((u8 *) ie_ptr + sizeof(custom_ie) +
                                   ie_ptr->ie_length);
        }
        if (*pie_index == -1 || (*pie_index > (MAX_MGMT_IE_INDEX -1))) {
            /* check for > (MAX_MGMT_IE_INDEX -1) */
            mwu_printf(MSG_ERROR, "No free IE buffer available\n");
            ret = WPS_STATUS_FAIL;
        }
    }
  _exitp_:

    if (buf)
        free(buf);

    LEAVE();
    return ret;
}

/**
 *  @brief Process and send ie config command for probe request.
 *  @param flag   FLAG: Set/clear WPS IE
 *  @param index  A pointer to the IE buffer index
 *  @return            WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
int
wps_wlan_probe_request_ie_config(int flag, short *pie_index)
{
    u8 *buf, *pos;
    IEEEtypes_Header_t *ptlv_header = NULL;
    struct iwreq iwr;
    int i, ret = WPS_STATUS_SUCCESS;
    u16 ie_len = 0, mgmt_subtype_mask = 0;
    tlvbuf_custom_ie *tlv = NULL;
    custom_ie *ie_ptr = NULL;
    u8 action = 0x00, type = 0x00;

    ENTER();

    type = flag & 0x0f;
    action = flag & 0xf0;

    buf = (u8 *) malloc(MRVDRV_SIZE_OF_CMD_BUFFER);
    if (buf == NULL)
        return WPS_STATUS_FAIL;

    memset(buf, 0, MRVDRV_SIZE_OF_CMD_BUFFER);

    tlv = (tlvbuf_custom_ie *) buf;
    tlv->tag = MRVL_MGMT_IE_LIST_TLV_ID;

    /* Locate headers */
    ie_ptr = (custom_ie *) (tlv->ie_data);
    /* Set TLV fields */

    if (action == SET_WPS_IE) {
        /* Set WPS IE */
        pos = ie_ptr->ie_buffer;
        ptlv_header = (IEEEtypes_Header_t *) pos;
        pos += sizeof(IEEEtypes_Header_t);
        ie_len = wps_sta_probe_request_prepare(pos);
        mgmt_subtype_mask = MGMT_MASK_PROBE_REQ;
        ptlv_header->Type = WPS_IE;
        ptlv_header->Len = ie_len;
        ie_ptr->mgmt_subtype_mask = mgmt_subtype_mask;
        tlv->length = sizeof(custom_ie) + sizeof(IEEEtypes_Header_t) + ie_len;
        ie_ptr->ie_length = sizeof(IEEEtypes_Header_t) + ie_len;
        ie_ptr->ie_index = *pie_index;

        ie_ptr = (custom_ie *) (((u8 *) ie_ptr) + sizeof(custom_ie) +
                                sizeof(IEEEtypes_Header_t) + ie_len);
        pos = ie_ptr->ie_buffer;
        ptlv_header = (IEEEtypes_Header_t *) pos;
        pos += sizeof(IEEEtypes_Header_t);

    } else if (action == CLEAR_WPS_IE) {
        /* Clear WPS IE */
        pos = ie_ptr->ie_buffer;
        ie_ptr->mgmt_subtype_mask = MGMT_MASK_CLEAR;
        ie_ptr->ie_length = 0;
        ie_ptr->ie_index = *pie_index;   /* index */
        if (ie_ptr->ie_index < 2)
            ie_ptr->mgmt_subtype_mask = 0xFFFF;
        tlv->length = sizeof(custom_ie);

    } else {
        /* Get WPS IE */
        tlv->length = 0;
    }

    mwu_hexdump(DEBUG_WLAN, "MGMT_IE", (const unsigned char *) buf,
                tlv->length + TLV_HEADER_SIZE);

    memset(&iwr, 0, sizeof(iwr));
    strncpy(iwr.ifr_name, (char *) dev_name, IFNAMSIZ);
    iwr.u.data.pointer = (void *) buf;
    iwr.u.data.length = tlv->length + TLV_HEADER_SIZE;
    iwr.u.data.flags = 0;

    if (ioctl(ioctl_sock, WLAN_CUSTOM_IE, &iwr)) {
        perror("ioctl[WLAN_CUSTOM_IE]");
        mwu_printf(MSG_ERROR, "Failed to set/get/clear the IE buffer");
        ret = WPS_STATUS_FAIL;
        goto _exitp_;
    }

    if (flag == GET_WPS_IE) {
        /* Get the free IE buffer index number */
        tlv = (tlvbuf_custom_ie *) buf;
        *pie_index = -1;
        ie_ptr = (custom_ie *) (tlv->ie_data);
        for (i = 0; i < MAX_MGMT_IE_INDEX; i++) {
            if (ie_ptr->mgmt_subtype_mask == 0) {
                *pie_index = ie_ptr->ie_index;
                break;
            }
            if (i < (MAX_MGMT_IE_INDEX - 1))
                ie_ptr =
                    (custom_ie *) ((u8 *) ie_ptr + sizeof(custom_ie) +
                                   ie_ptr->ie_length);
        }
        if (*pie_index == -1 || (*pie_index > (MAX_MGMT_IE_INDEX -1))) {
            /* check for > (MAX_MGMT_IE_INDEX -1) */
            mwu_printf(MSG_ERROR, "No free IE buffer available\n");
            ret = WPS_STATUS_FAIL;
        }
    }
  _exitp_:

    if (buf)
        free(buf);

    LEAVE();
    return ret;
}

/**
 *  @brief  Set private ioctl command "wpssession" to WLAN driver
 *
 *  @param enable     0 - WPS Session Disable, 1 - WPS Session Enable
 *  @return           WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
int
wps_wlan_session_control(int enable)
{
    struct iwreq iwr;
    int ret = WPS_STATUS_SUCCESS;
    u8 buf[4];
    int ioctl_val, subioctl_val;

    ENTER();

    if (get_priv_ioctl("wpssession",
                       &ioctl_val, &subioctl_val) == WPS_STATUS_FAIL) {
        mwu_printf(MSG_ERROR, "ERROR : get_priv_ioctl FAIL !");
        LEAVE();
        return WPS_STATUS_FAIL;
    }
    mwu_printf(DEBUG_WLAN, "ioctl_val %x, subioctl_val %d", ioctl_val,
               subioctl_val);

    memset(&iwr, 0, sizeof(iwr));
    memset(buf, 0, sizeof(buf));
    strncpy(iwr.ifr_name, (char *) dev_name, IFNAMSIZ);

    if (enable == WPS_SESSION_ON || enable == WPS_SESSION_OFF)
        buf[0] = enable;
    else
        return WPS_STATUS_FAIL;

    iwr.u.data.pointer = buf;
    iwr.u.data.length = sizeof(buf);
    iwr.u.data.flags = subioctl_val;

    if (ioctl(ioctl_sock, ioctl_val, &iwr) < 0) {
        perror("ioctl[wpssession]");
        ret = WPS_STATUS_FAIL;
    }

    LEAVE();
    return ret;
}

/**
 *  @brief  Get power management mode of WLAN driver (SIOCGIWPOWER)
 *
 *  @param enable     Pointer of returned buffer
 *  @return           WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
int
wps_wlan_get_power_mode(u8 * enable)
{
    struct iwreq iwr;
    int ret = WPS_STATUS_SUCCESS;

    ENTER();

    memset(&iwr, 0, sizeof(iwr));
    strncpy(iwr.ifr_name, (char *) dev_name, IFNAMSIZ);

    iwr.u.power.flags = 0;

    if (ioctl(ioctl_sock, SIOCGIWPOWER, &iwr) < 0) {
        perror("ioctl[SIOCGIWPOWER]");
        ret = WPS_STATUS_FAIL;
    }

    if (iwr.u.power.disabled == 1)
        *enable = 0;
    else
        *enable = 1;

    mwu_printf(DEBUG_WLAN, "Get PS Mode : %d", *enable);

    LEAVE();
    return ret;
}

/**
 *  @brief  Set power management mode to WLAN driver (SIOCSIWPOWER)
 *
 *  @param enable     0 = Disable PS mode, 1 = Enable PS mode
 *  @return           WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
int
wps_wlan_set_power_mode(u8 enable)
{
    struct iwreq iwr;
    int ret = WPS_STATUS_SUCCESS;

    ENTER();

    memset(&iwr, 0, sizeof(iwr));
    strncpy(iwr.ifr_name, (char *) dev_name, IFNAMSIZ);

    mwu_printf(DEBUG_WLAN, "Set PS Mode : %d", enable);

    iwr.u.power.flags = 0;
    if (enable == 1)
        iwr.u.power.disabled = 0;
    else
        iwr.u.power.disabled = 1;

    if (ioctl(ioctl_sock, SIOCSIWPOWER, &iwr) < 0) {
        perror("ioctl[SIOCSIWPOWER]");
        ret = WPS_STATUS_FAIL;
    }

    LEAVE();
    return ret;
}

/**
 *  @brief  Get compiled version of wireless extension of WLAN driver
 *
 *  @param version    version
 *  @return           WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
int
wps_wlan_get_WECompiledVersion(int *version)
{
    ENTER();

    *version = we_version_compiled;

    LEAVE();
    return WPS_STATUS_SUCCESS;
}

/**
 *  @brief  Initial ioctl interface to WLAN driver
 *
 *  @param wps_s      Pointer to WPS global structure
 *  @return           WPS_STATUS_SUCCESS--success, WPS_STATUS_FAIL--fail
 */
int
wps_wlan_init(WPS_DATA * wps_s)
{
    int flags;

    ENTER();

    strncpy((char *) dev_name, wps_s->ifname, IFNAMSIZ);

    mwu_printf(DEBUG_WLAN, "Initializing wlan interface : %s\n", dev_name);

    /*
     * create a socket
     */
    if ((ioctl_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "wpsapp: Cannot open socket.\n");
        LEAVE();
        exit(-1);
    }
    /* Get BSS type */
    wps_wlan_get_bss_type(&wps_s->bss_type);

    if (wps_s->bss_type == BSS_TYPE_STA) {
        if (get_range() < 0) {
            fprintf(stderr, "wpsapp: Cannot get range.\n");
            LEAVE();
            exit(-1);
        }

        if (wps_wlan_get_ifflags(&flags) != 0 ||
            wps_wlan_set_ifflags(flags | IFF_UP) != 0) {
            mwu_printf(MSG_WARNING, "Could not set interface '%s' UP\n",
                       wps_s->ifname);
        }
    }
    LEAVE();
    return WPS_STATUS_SUCCESS;
}

/**
 *  @brief  Close ioctl interface to WLAN driver
 *
 *  @param wps_s      Pointer to WPS global structure
 *  @return           None
 */
void
wps_wlan_deinit(WPS_DATA * wps_s)
{
    ENTER();
    /* close existing socket */
    close(ioctl_sock);

    LEAVE();
    return;
}

/**
 *  @brief Send user scan ioctl command to WLAN driver
 *  @param pwifidir_data    Wifi Direct structure
 *  @param go_ssid          ssid of Group Owner
 *  @param ifname           Interface name
 *  @param operation        Scan operation to be performed
 *                          Either SCAN_OP_CHANNEL, SCAN_COMMON_CHANNEL_LIST,
 *                          or SCAN_GENERIC
 *
 *  @return         WPS_STATUS_SUCCESS--success, otherwise--fail
 */
int
wps_wlan_set_user_scan(WIFIDIR_DATA * pwifidir_data, unsigned char *go_ssid,
        char *ifname, int operation)
{
    int op_channel;
    struct wifidir_peer *peer = NULL;
    int all_chan_scan_list[MWU_IOCTL_USER_SCAN_CHAN_MAX];
    int i = 0, j = 0, k = 0;

    ENTER();

    if(pwifidir_data->current_peer == NULL) {
        ERR("Current Peer is not yet updated\n");
        LEAVE();
        return WPS_STATUS_FAIL;
    }

    memset(all_chan_scan_list, 0, sizeof(int)*MWU_IOCTL_USER_SCAN_CHAN_MAX);

    switch(operation) {
        case SCAN_OP_CHANNEL:
            op_channel = pwifidir_data->current_peer->op_channel;
            if(op_channel) {
                /* Scan on the OP channel */
                all_chan_scan_list[0] = op_channel;
            } else {
                LEAVE();
                return WPS_STATUS_FAIL;
            }
            break;

        case SCAN_COMMON_CHANNEL_LIST:
            peer = pwifidir_data->current_peer;
            if(peer->peer_channel_list.num_of_chan &&
                    pwifidir_data->self_channel_list.num_of_chan) {
                /* Scan on the channels that are common between peer and self */

                for(i=0; i < pwifidir_data->self_channel_list.num_of_chan; i++) {
                    for(j=0; j < peer->peer_channel_list.num_of_chan; j++) {
                        if(pwifidir_data->self_channel_list.chan[i] ==
                                peer->peer_channel_list.chan[j]) {

                            all_chan_scan_list[k] = pwifidir_data->self_channel_list.chan[i];
                            k++;
                        }
                    }
                }
            } else {
                LEAVE();
                return WPS_STATUS_FAIL;
            }
            break;

        case SCAN_GENERIC:
            /* Generic scan */
            wps_wlan_set_scan();
            LEAVE();
            return WPS_STATUS_SUCCESS;

        default:
            ERR("Invalid option!!");
            LEAVE();
            return WPS_STATUS_FAIL;
    }

    mwu_set_user_scan(ifname, all_chan_scan_list, go_ssid);
    LEAVE();
    return WPS_STATUS_SUCCESS;
}


/**
 *  @brief  Process scan operation
 *
 *  @param wps_s      Pointer to WPS global structure
 *  @return           None
 */
void
wps_wlan_scan(WPS_DATA * wps_s)
{
    int ret;
    int retry = 5;

    ENTER();

#ifdef WPS_IE_NEEDED
    /* Append the WPS IE in Probe Request */
    wps_wlan_set_fgscan_wps_ie();
#endif

    /* Send the Scan IOCTL Command */
    do {
        ret = wps_wlan_set_scan();
        retry--;
    } while (retry > 0 && ret == WPS_STATUS_FAIL);

#ifdef WPS_IE_NEEDED
    /* Clear the WPS IE in Probe Request */
    wps_wlan_reset_wps_ie();
#endif

    LEAVE();
}

/**
 *  @brief mapping RF band by frequency
 *
 *  @param freq     frequency value
 *  @return         channel number
 */
int
wps_wlan_freq_to_band(int freq)
{
    int i, table;

    table = sizeof(channel_freq_UN_BG) / sizeof(CHANNEL_FREQ_ENTRY);
    for (i = 0; i < table; i++) {
        if (freq == channel_freq_UN_BG[i].Freq) {
            return RF_24_G;
        }
    }

    table = sizeof(channel_freq_UN_AJ) / sizeof(CHANNEL_FREQ_ENTRY);
    for (i = 0; i < table; i++) {
        if (freq == channel_freq_UN_AJ[i].Freq) {
            return RF_50_G;
        }
    }

    return RF_24_G;
}
