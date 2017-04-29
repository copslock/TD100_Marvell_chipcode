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
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if_arp.h>       /* For ARPHRD_ETHER */
#include <linux/wireless.h>
#define UTIL_LOG_TAG "MWU IOCTL"
#include "util.h"
#include "mwu_log.h"
#include "wps_msg.h"
#include "wlan_hostcmd.h"
#include "wlan_wifidir.h"
#include "wps_wlan.h"
#include "mwu_ioctl.h"
#include "wps_def.h"
#include "mwu_internal.h"
#include "queue.h"

#define MAX_IFACE 16

#define UAP_BSS_CTRL                (SIOCDEVPRIVATE + 4)
#define UAP_GET_STA_LIST                (SIOCDEVPRIVATE + 11)

#define DEFAULT_SSID "DIRECT-*"

/* iface_info: structure including interface name and its ioctl socket
 */
struct iface_info {
    char ifname[IFNAMSIZ + 1];
    int ioctl_sock;
    SLIST_ENTRY(iface_info) list_item;
};

/* iface_list: a list of iface_info items */
SLIST_HEAD(iface_list, iface_info);

/* We maintain a list of iface_info items that are currently in use */
static struct iface_list ifaces;
static int iface_num = 0;

/* If ifname is already in the list, return ioctl_sock;
 * If not, create a new socket and add the new iface_info item into the list.
 */
static int get_ioctl_sock(const char *ifname)
{
    struct iface_info *cur = NULL, *found = NULL;

    if (!ifname)
        return -1;

    SLIST_FOREACH(cur, &ifaces, list_item) {
        if (!strncmp(cur->ifname, ifname, IFNAMSIZ)) {
            found = cur;
            break;
        }
    }

    if (found)
        return found->ioctl_sock;

    struct iface_info *new = NULL;
    new = (struct iface_info *) malloc(sizeof(struct iface_info));
    if (!new)
        return -1;
    if ((new->ioctl_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        ERR("Cannot open socket.");
        free(new);
        return -1;
    }

    if (!iface_num) {
        SLIST_INIT(&ifaces);
    } else if (iface_num == MAX_IFACE) {
        /* If reach MAX_IFACE, remove one item before insertion. */
        close(ifaces.slh_first->ioctl_sock);
        SLIST_REMOVE_HEAD(&ifaces, list_item);
        --iface_num;
    }

    strncpy(new->ifname, ifname, IFNAMSIZ);
    SLIST_INSERT_HEAD(&ifaces, new, list_item);
    ++iface_num;

    return new->ioctl_sock;
}

struct iw_priv_args  *Priv_args = NULL;

static int send_iwreq_ioctl(const char *ifname, struct iwreq *iwr, int ioctl_val)
{
    int ret = MWU_ERR_SUCCESS;
    int ioctl_sock = get_ioctl_sock(ifname);

    if (!ifname || !iwr || ioctl_sock < 0)
        return MWU_ERR_INVAL;

    strncpy(iwr->ifr_name, (char *) ifname, IFNAMSIZ);
    if (ioctl(ioctl_sock, ioctl_val, iwr) < 0)
        ret = MWU_ERR_COM;

    return ret;
}

static int send_ifreq_ioctl(const char *ifname, struct ifreq *ifr, int ioctl_val)
{
    int ret = MWU_ERR_SUCCESS;
    int ioctl_sock = get_ioctl_sock(ifname);

    if (!ifname || !ifr || ioctl_sock < 0)
        return MWU_ERR_INVAL;

    strncpy(ifr->ifr_name, (char *) ifname, IFNAMSIZ);
    if (ioctl(ioctl_sock, ioctl_val, ifr) < 0)
        ret = MWU_ERR_COM;

    return ret;
}

int mwu_get_private_info(char *iface)
{
    /* This function sends the SIOCGIWPRIV command which is
     * handled by the kernel. and gets the total number of
     * private ioctl's available in the host driver.
     */
    struct iwreq iwr;
    int ret = MWU_ERR_SUCCESS;
    struct iw_priv_args *pPriv = NULL;
    struct iw_priv_args *newPriv;
    int result = 0;
    size_t size = IW_INIT_PRIV_NUM;
    int ioctl_sock = get_ioctl_sock(iface);

    memset(&iwr, 0, sizeof(iwr));
    strncpy(iwr.ifr_name,(char *)iface, IFNAMSIZ);

    do {
        /* (Re)allocate the buffer */
        newPriv = realloc(pPriv, size * sizeof(pPriv[0]));
        if (newPriv == NULL) {
            ERR("Error: Buffer allocation failed\n");
            ret = MWU_ERR_COM;
            break;
        }
        pPriv = newPriv;

        iwr.u.data.pointer = (caddr_t) pPriv;
        iwr.u.data.length = size;
        iwr.u.data.flags = 0;

        if (ioctl(ioctl_sock, SIOCGIWPRIV, &iwr) < 0) {
            result = errno;
            ret = MWU_ERR_COM;
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

    if ((ret == MWU_ERR_COM) && (pPriv))
        free(pPriv);

    return ret;
}

int
mwu_get_subioctl_no(int i, int priv_cnt, int *ioctl_val, int *subioctl_val)
{
    int j;

    if (Priv_args[i].cmd >= SIOCDEVPRIVATE) {
        *ioctl_val = Priv_args[i].cmd;
        *subioctl_val = 0;
        ERR("Not a private ioctl.\n");
        return MWU_ERR_SUCCESS;
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
        INFO("Invalid private ioctl definition for: 0x%x\n",
                   Priv_args[i].cmd);
        return MWU_ERR_COM;
    }

    /* Save ioctl numbers */
    *ioctl_val = Priv_args[j].cmd;
    *subioctl_val = Priv_args[i].cmd;

    return MWU_ERR_SUCCESS;
}

int mwu_get_ioctl_no(char * iface, const char * priv_cmd, int *ioctl_val, int *subioctl_val)
{
    int i;
    int priv_cnt;
    int ret = MWU_ERR_COM;

    priv_cnt = mwu_get_private_info(iface);

    /* Are there any private ioctls? */
    if (priv_cnt <= 0 || priv_cnt > IW_MAX_PRIV_NUM) {
        /* Could skip this message ? */
        ERR("%-8.8s  no private ioctls.\n", iface);
    } else {
        for (i = 0; i < priv_cnt; i++) {
            if (Priv_args[i].name[0] &&
                !strcmp((char *) Priv_args[i].name, (char *) priv_cmd)) {
                ret =
                    mwu_get_subioctl_no(i, priv_cnt, ioctl_val,
                                            subioctl_val);
                break;
            }
        }
    }

    if (Priv_args) {
        free(Priv_args);
        Priv_args = NULL;
    }

    return ret;
}

int mwu_get_priv_ioctl(char *iface, char *ioctl_name,
                       int *ioctl_val, int *subioctl_val)
{
    int retval;

    retval = mwu_get_ioctl_no(iface, ioctl_name, ioctl_val,
                              subioctl_val);

    /* Debug print discovered IOCTL values */
    INFO("ioctl %s: %x, %x\n", ioctl_name, *ioctl_val,
               *subioctl_val);

    return retval;
}

int mwu_get_ssid(char *ifname, unsigned char *ssid)
{
    struct iwreq iwr;

    memset(&iwr, 0, sizeof(iwr));
    iwr.u.essid.pointer = (caddr_t) ssid;
    iwr.u.essid.length = 32;

    return (send_iwreq_ioctl(ifname, &iwr, SIOCGIWESSID)
            == MWU_ERR_SUCCESS) ? iwr.u.essid.length : MWU_ERR_COM;
}

int mwu_set_ssid(char *ifname, const unsigned char *ssid,
                 unsigned int ssid_len, int skip_scan)
{
    struct iwreq iwr;
    char buf[33];

    if (ssid_len > 32) {
        ERR("Invalid SSID. SSID should be <= 32 ASCII");
        return MWU_ERR_INVAL;
    }

    memset(&iwr, 0, sizeof(iwr));

    /* flags: 1 = ESSID is active, 0 = Any (Promiscuous) */
    if (!skip_scan)
        iwr.u.essid.flags = (ssid_len != 0);
    else
        iwr.u.essid.flags = 0xFFFF;
    memset(buf, 0, sizeof(buf));
    memcpy(buf, ssid, ssid_len);
    iwr.u.essid.pointer = (caddr_t) buf;

    /*
     * In bionic/libc/kernel/common/linux/wireless.h, WIRELESS_EXT is 20,
     * but in kernel/kernel/include/linux/wireless.h, WIRELESS_EXT is 22.
     * So, there will be mismatch between application and driver when
     * handling ssid_len, which may cause problem in MG1 platform. Thus,
     * temporarily, we ignore WIRELESS_EXT here and assume it is 22.
     * However, to really fix that issue, WIRELESS_EXT should be the same
     * in those two wireless.h files. That inconsistency is a really
     * lurking bug.
     */
    iwr.u.essid.length = ssid_len ? ssid_len : 0;
    INFO("Wireless Ext version %d", WIRELESS_EXT);
    INFO("SSID length %d", iwr.u.essid.length);

    return send_iwreq_ioctl(ifname, &iwr, SIOCSIWESSID);
}

int mwu_set_authentication(char *ifname, int auth_mode)
{
    struct iwreq iwr;

    memset(&iwr, 0, sizeof(iwr));
    iwr.u.param.flags = IW_AUTH_80211_AUTH_ALG;

    if (auth_mode == AUTHENTICATION_TYPE_SHARED)
        iwr.u.param.value = IW_AUTH_ALG_SHARED_KEY;
    else
        iwr.u.param.value = IW_AUTH_ALG_OPEN_SYSTEM;

    return send_iwreq_ioctl(ifname, &iwr, SIOCSIWAUTH);
}

int mwu_get_wap(char *ifname, unsigned char *bssid)
{
    struct iwreq iwr;

    memset(&iwr, 0, sizeof(iwr));

    if (send_iwreq_ioctl(ifname, &iwr, SIOCGIWAP)
        == MWU_ERR_SUCCESS && iwr.u.ap_addr.sa_family == ARPHRD_ETHER) {
        memcpy(bssid, iwr.u.ap_addr.sa_data, ETH_ALEN);
        return MWU_ERR_SUCCESS;
    }

    return MWU_ERR_COM;
}

int mwu_set_wap(char *ifname, const unsigned char *bssid)
{
    struct iwreq iwr;

    memset(&iwr, 0, sizeof(iwr));
    iwr.u.ap_addr.sa_family = ARPHRD_ETHER;
    memcpy(iwr.u.ap_addr.sa_data, bssid, ETH_ALEN);

    return send_iwreq_ioctl(ifname, &iwr, SIOCSIWAP);
}

int mwu_set_mode(char *ifname, int mode)
{
    struct iwreq iwr;

    memset(&iwr, 0, sizeof(iwr));
    iwr.u.mode = mode ? IW_MODE_ADHOC : IW_MODE_INFRA;

    return send_iwreq_ioctl(ifname, &iwr, SIOCSIWMODE);
}

int mwu_get_ifflags(char *ifname, int *flags)
{
    struct ifreq ifr;
    int ret;

    memset(&ifr, 0, sizeof(ifr));
    if ((ret = send_ifreq_ioctl(ifname, &ifr, SIOCGIFFLAGS))
        == MWU_ERR_SUCCESS) {
        *flags = ifr.ifr_flags & 0xffff;
    }

    return ret;
}

int mwu_set_ifflags(char *ifname, int flags)
{
    struct ifreq ifr;

    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = flags & 0xffff;

    return send_ifreq_ioctl(ifname, &ifr, SIOCSIFFLAGS);
}

int mwu_set_deauth(char *ifname)
{
    struct iwreq iwr;
    int ret = MWU_ERR_SUCCESS;
    int ioctl_val, subioctl_val;

    if ((ret = mwu_get_priv_ioctl(ifname, "deauth", &ioctl_val,
        &subioctl_val)) != MWU_ERR_SUCCESS) {
        ERR("mwu_get_priv_ioctl FAIL !");
        return ret;
    }
    INFO("ioctl_val %x, subioctl_val %d", ioctl_val, subioctl_val);

    memset(&iwr, 0, sizeof(iwr));
    iwr.u.data.pointer = NULL;
    iwr.u.data.flags = subioctl_val;

    return send_iwreq_ioctl(ifname, &iwr, ioctl_val);
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

int mwu_ie_config(char *ifname, int flag, short *pie_index)
{
    unsigned char *buf, *pos;
    IEEEtypes_Header_t *ptlv_header = NULL;
    struct iwreq iwr;
    int i, ret = MWU_ERR_SUCCESS;
    unsigned short ie_len = 0, mgmt_subtype_mask = 0;
    tlvbuf_custom_ie *tlv = NULL;
    custom_ie *ie_ptr = NULL;
    unsigned char action = 0x00, type = 0x00;

    type = flag & 0x0f;
    action = flag & 0xf0;

    buf = (unsigned char *) malloc(MRVDRV_SIZE_OF_CMD_BUFFER);
    if (buf == NULL)
        return MWU_ERR_NOMEM;

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
            ERR("ie_config: unknown SET type!\n");
            ret = MWU_ERR_INVAL;
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
        /* Clear WPS IE */
        pos = ie_ptr->ie_buffer;
        ie_ptr->mgmt_subtype_mask = MGMT_MASK_CLEAR;
        ie_ptr->ie_length = 0;
        ie_ptr->ie_index = *pie_index;
        if (ie_ptr->ie_index < 2)
            ie_ptr->mgmt_subtype_mask = 0xFFFF;

        if (*pie_index < (MAX_MGMT_IE_INDEX - 1)) {
            ie_ptr = (custom_ie *) (((unsigned char *) ie_ptr)
                     + sizeof(custom_ie));
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

    memset(&iwr, 0, sizeof(iwr));
    iwr.u.data.pointer = (void *) buf;
    iwr.u.data.length = tlv->length + TLV_HEADER_SIZE;
    iwr.u.data.flags = 0;

    if ((ret = send_iwreq_ioctl(ifname, &iwr, WLAN_CUSTOM_IE))
        != MWU_ERR_SUCCESS) {
        ERR("Failed to set/get/clear the IE buffer");
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
            ERR("No free IE buffer available");
            ret = MWU_ERR_COM;
        }
    }
_exit_:
    if (buf)
        free(buf);

    return ret;
}

int mwu_assoc_request_ie_config(char *ifname, int flag,
                                short *pie_index)
{
    return mwu_ie_config(ifname, flag, pie_index);
}

int mwu_prob_request_ie_config(char *ifname, int flag,
                                short *pie_index)
{
    unsigned char *buf, *pos;
    IEEEtypes_Header_t *ptlv_header = NULL;
    struct iwreq iwr;
    int i, ret = MWU_ERR_SUCCESS;
    unsigned short ie_len = 0, mgmt_subtype_mask = 0;
    tlvbuf_custom_ie *tlv = NULL;
    custom_ie *ie_ptr = NULL;
    unsigned char action = 0x00, type = 0x00;

    type = flag & 0x0f;
    action = flag & 0xf0;

    buf = (unsigned char *) malloc(MRVDRV_SIZE_OF_CMD_BUFFER);
    if (buf == NULL)
        return MWU_ERR_NOMEM;

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

    memset(&iwr, 0, sizeof(iwr));
    iwr.u.data.pointer = (void *) buf;
    iwr.u.data.length = tlv->length + TLV_HEADER_SIZE;
    iwr.u.data.flags = 0;

    if ((ret = send_iwreq_ioctl(ifname, &iwr, WLAN_CUSTOM_IE))
        != MWU_ERR_SUCCESS) {
        ERR("Failed to set/get/clear the IE buffer");
        goto _exit_;
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
                    (custom_ie *) ((unsigned char *) ie_ptr + sizeof(custom_ie) +
                                   ie_ptr->ie_length);
        }
        if (*pie_index == -1 || *pie_index > 3) {
            /* check for > 3 */
            ERR("No free IE buffer available");
            ret = MWU_ERR_COM;
        }
    }

_exit_:
    if (buf)
        free(buf);

    return ret;
}



int mwu_apcmd_get_bss_config(char *ifname, bss_config_t * bss)
{
    apcmdbuf_bss_configure *cmd_buf = NULL;
    bss_config_t *bss_temp = NULL;
    u8 *buf = NULL;
    u16 cmd_len;
    u16 buf_len;
    int ret = WPS_STATUS_SUCCESS;
    struct ifreq ifr;

    /* Alloc buf for command */
    buf_len = MRVDRV_SIZE_OF_CMD_BUFFER;
    buf = (u8 *) malloc(buf_len);
    if (!buf) {
        ERR("Cannot allocate buffer from command!");
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
    ifr.ifr_data = (void *) cmd_buf;
    free(buf);
    return send_ifreq_ioctl(ifname, &ifr, UAP_BSS_CONFIG);
}


int mwu_apcmd_set_bss_config(char *ifname, bss_config_t * bss)
{
    apcmdbuf_bss_configure *cmd_buf = NULL;
    bss_config_t *bss_temp = NULL;
    u8 *buf = NULL;
    u16 cmd_len;
    u16 buf_len;
    int ret = WPS_STATUS_SUCCESS;
    struct ifreq ifr;

    /* Alloc buf for command */
    buf_len = MRVDRV_SIZE_OF_CMD_BUFFER;
    buf =  malloc(buf_len);
    if (!buf) {
        ERR("Cannot allocate buffer from command!");
        return WPS_STATUS_FAIL;
    }
    memset((u8 *)buf, 0, buf_len);
    /* Locate headers */
    cmd_len = sizeof(apcmdbuf_bss_configure);
    cmd_buf = (apcmdbuf_bss_configure *) buf;
    bss_temp = (bss_config_t *) (buf + cmd_len);
    memcpy(bss_temp, bss, sizeof(bss_config_t));

    /* Fill the command buffer */
    cmd_buf->Action = ACTION_SET;

    /* Initialize the ifr structure */
    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_data = (void *) cmd_buf;
    return send_ifreq_ioctl(ifname, &ifr, UAP_BSS_CONFIG);

}

int mwu_apcmd_start_bss(char *ifname)
{
    unsigned int data = 0;
    struct ifreq ifr;

    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_data = (void *) &data;

    return send_ifreq_ioctl(ifname, &ifr, UAP_BSS_CTRL);
}

int mwu_apcmd_stop_bss(char *ifname)
{
    unsigned int data = 1;
    struct ifreq ifr;

    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_data = (void *) &data;

    return send_ifreq_ioctl(ifname, &ifr, UAP_BSS_CTRL);
}

int mwu_set_intended_mac_addr(char *ifname, unsigned char *mac)
{
    struct ifreq ifr;

    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
    memcpy(ifr.ifr_hwaddr.sa_data, mac, ETH_ALEN);

    return send_ifreq_ioctl(ifname, &ifr, SIOCSIFHWADDR);
}

int mwu_custom_ie_config(char *ifname, unsigned char *buf)
{
    struct iwreq iwr;
    int ret = WPS_STATUS_SUCCESS;

    memset(&iwr, 0, sizeof(iwr));
    iwr.u.data.pointer = (void *) buf;
    iwr.u.data.flags = 0;

    return send_iwreq_ioctl(ifname, &iwr, WLAN_CUSTOM_IE);
}

int mwu_hostcmd(char *ifname, unsigned char *buf)
{
    struct ifreq ifr;

    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_ifru.ifru_data = (void *) buf;

    return send_ifreq_ioctl(ifname, &ifr, WIFIDIRHOSTCMD);
}

int mwu_set_bss_type(char *ifname, int bss_type)
{
    struct iwreq iwr;

    int ioctl_val, subioctl_val;

    if (mwu_get_priv_ioctl(ifname, "bssrole", &ioctl_val,
        &subioctl_val) != MWU_ERR_SUCCESS) {
        ERR("mwu_get_priv_ioctl FAIL !");
        return MWU_ERR_COM;
    }
    INFO("ioctl_val %x, subioctl_val %d", ioctl_val, subioctl_val);

    memset(&iwr, 0, sizeof(iwr));
    iwr.u.data.pointer = (caddr_t) & bss_type;
    iwr.u.data.length = sizeof(bss_type);
    iwr.u.data.flags = subioctl_val;

    return send_iwreq_ioctl(ifname, &iwr, ioctl_val);
}

int mwu_get_bss_type(char *ifname, int *bss_type)
{
    struct iwreq iwr;
    int ret = MWU_ERR_SUCCESS;

    int ioctl_val, subioctl_val;

    if ((ret = mwu_get_priv_ioctl(ifname, "bssrole", &ioctl_val,
        &subioctl_val)) != MWU_ERR_SUCCESS) {
        ERR("mwu_get_priv_ioctl FAIL !");
        *bss_type = BSS_TYPE_STA;
        return ret;
    }
    INFO("ioctl_val %x, subioctl_val %d", ioctl_val, subioctl_val);

    memset(&iwr, 0, sizeof(iwr));
    iwr.u.data.pointer = (caddr_t) bss_type;
    iwr.u.data.flags = subioctl_val;

    if ((ret = send_iwreq_ioctl(ifname, &iwr, ioctl_val))
        != MWU_ERR_SUCCESS) {
        if (!strncmp(ifname, "uap", 3))
            *bss_type = BSS_TYPE_UAP;
        else
            *bss_type = BSS_TYPE_STA;
    }

    return ret;
}

int mwu_set_deepsleep(char *ifname, u16 enable)
{
    struct iwreq iwr;
    int ioctl_val, subioctl_val;

    if (mwu_get_priv_ioctl(ifname, "deepsleep", &ioctl_val,
        &subioctl_val) != MWU_ERR_SUCCESS) {
        ERR("mwu_get_priv_ioctl FAIL !");
        return MWU_ERR_COM;
    }
    INFO("ioctl_val %x, subioctl_val %d", ioctl_val, subioctl_val);

    memset(&iwr, 0, sizeof(iwr));
    iwr.u.data.pointer = (caddr_t) & enable;
    iwr.u.data.length = sizeof(enable);
    iwr.u.data.flags = subioctl_val;

    return send_iwreq_ioctl(ifname, &iwr, ioctl_val);
}

int mwu_session_control(char *ifname, int enable)
{
    struct iwreq iwr;
    int ret = MWU_ERR_SUCCESS;
    unsigned char buf[4];
    int ioctl_val, subioctl_val;

    if ((ret = mwu_get_priv_ioctl(ifname, "wpssession",
                       &ioctl_val, &subioctl_val)) != MWU_ERR_SUCCESS) {
        ERR("mwu_get_priv_ioctl FAIL !");
        return ret;
    }
    INFO("ioctl_val %x, subioctl_val %d", ioctl_val, subioctl_val);

    memset(&iwr, 0, sizeof(iwr));
    memset(buf, 0, sizeof(buf));

    if (enable == WPS_SESSION_ON || enable == WPS_SESSION_OFF)
        buf[0] = enable;
    else
        return MWU_ERR_INVAL;

    iwr.u.data.pointer = buf;
    iwr.u.data.length = sizeof(buf);
    iwr.u.data.flags = subioctl_val;

    return send_iwreq_ioctl(ifname, &iwr, ioctl_val);
}

int mwu_get_power_mode(char *ifname, int *enable)
{
    struct iwreq iwr;
    int ret = MWU_ERR_SUCCESS;

    memset(&iwr, 0, sizeof(iwr));
    iwr.u.power.flags = 0;

    if ((ret = send_iwreq_ioctl(ifname, &iwr, SIOCGIWPOWER))
        != MWU_ERR_SUCCESS) {
        return ret;
    }

    if (iwr.u.power.disabled == 1)
        *enable = 0;
    else
        *enable = 1;

    INFO("Get PS Mode : %d", *enable);

    return ret;
}

int mwu_set_power_mode(char *ifname, int enable)
{
    struct iwreq iwr;

    memset(&iwr, 0, sizeof(iwr));

    INFO("Set PS Mode : %d", enable);
    iwr.u.power.flags = 0;
    if (enable == 1)
        iwr.u.power.disabled = 0;
    else
        iwr.u.power.disabled = 1;

    return send_iwreq_ioctl(ifname, &iwr, SIOCSIWPOWER);
}

int mwu_set_passphrase(char *ifname, char *buf, int buf_len)
{
    struct iwreq iwr;
    int ret = MWU_ERR_SUCCESS;
    int ioctl_val, subioctl_val;

    if ((ret = mwu_get_priv_ioctl(ifname, "passphrase", &ioctl_val,
        &subioctl_val)) != MWU_ERR_SUCCESS) {
        ERR("mwu_get_priv_ioctl FAIL !");
        return ret;
    }

    memset(&iwr, 0, sizeof(iwr));
    iwr.u.data.pointer = (caddr_t)buf;
    iwr.u.data.length  = buf_len;
    iwr.u.data.flags   = subioctl_val;

    return send_iwreq_ioctl(ifname, &iwr, ioctl_val);
}

/* borrowed from mlanconfig.c and bg_scan_wifidir.conf.  This function is hard
 * coded for our WIFIDIR set up and may not be suitable for all applications.
 */
int mwu_mlanconfig_bgscan(char *ifname)
{
    unsigned char *buf;
    HostCmd_DS_GEN *hostcmd;
    struct iwreq iwr;
    int ret = MWU_ERR_SUCCESS;
    int ioctl_val, subioctl_val;
    mlanconfig_bgscfg_cmd_hdr *hdr;
    ssid_header_tlv *ssid_tlv;
    probe_header_tlv *probe_tlv;
    channel_header_tlv *chan_tlv;
    snr_header_tlv *snr_tlv;
    start_later_header_tlv *start_later_tlv;

    if ((ret = mwu_get_priv_ioctl(ifname, "hostcmd",
        &ioctl_val, &subioctl_val)) != MWU_ERR_SUCCESS) {
        return ret;
    }

    buf = (unsigned char *)malloc(MRVDRV_SIZE_OF_CMD_BUFFER);
    if (buf == NULL) {
        ERR("allocate memory for hostcmd failed");
        return MWU_ERR_NOMEM;
    }

    /* prepare the bgscan command */
    memset(buf, 0, MRVDRV_SIZE_OF_CMD_BUFFER);
    hostcmd = (HostCmd_DS_GEN *)buf;
    hostcmd->Command = wlan_cpu_to_le16(HostCmd_CMD_BGSCAN_CFG);
    hostcmd->Size = S_DS_GEN;

    /* These are all magic numbers from the bg_scan_wifidir.conf file supplied by
     * Marvell
     */
    hdr = (mlanconfig_bgscfg_cmd_hdr *)(buf + sizeof(HostCmd_DS_GEN));
    hdr->Action = 1;
    hdr->Enable = 1;
    hdr->BssType = 3;
    hdr->ChannelsPerScan = 3;
    hdr->ScanInterval = wlan_cpu_to_le32(1000);
    hdr->StoreCondition = wlan_cpu_to_le32(1);
    hdr->ReportConditions = wlan_cpu_to_le32(1);
    hostcmd->Size += sizeof(mlanconfig_bgscfg_cmd_hdr);

    ssid_tlv = (ssid_header_tlv *)(buf + hostcmd->Size);
    ssid_tlv->type = MLANCONFIG_SSID_HEADER_TYPE;
    ssid_tlv->len = wlan_cpu_to_le16(SIZEOF_VALUE(ssid_header_tlv));
    ssid_tlv->MaxSSIDLen = wlan_cpu_to_le16(0);
    memcpy(ssid_tlv->ssid, "DIRECT-", sizeof(ssid_tlv->ssid));
    hostcmd->Size += sizeof(tlv) + ssid_tlv->len;

    probe_tlv = (probe_header_tlv *)(buf + hostcmd->Size);
    probe_tlv->type = MLANCONFIG_PROBE_HEADER_TYPE;
    probe_tlv->len = wlan_cpu_to_le16(SIZEOF_VALUE(probe_header_tlv));
    probe_tlv->NumProbes = wlan_cpu_to_le16(2);
    hostcmd->Size += sizeof(tlv) + probe_tlv->len;

    chan_tlv = (channel_header_tlv *)(buf + hostcmd->Size);
    chan_tlv->type = MLANCONFIG_CHANNEL_HEADER_TYPE;
    chan_tlv->len = wlan_cpu_to_le16(SIZEOF_VALUE(channel_header_tlv));
    chan_tlv->Chan1_RadioType = 0;
    chan_tlv->Chan1_ChanNumber = 1;
    chan_tlv->Chan1_ScanType = 2;
    chan_tlv->Chan1_MinScanTime = wlan_cpu_to_le16(50);
    chan_tlv->Chan1_ScanTime = wlan_cpu_to_le16(100);
    chan_tlv->Chan2_RadioType = 0;
    chan_tlv->Chan2_ChanNumber = 6;
    chan_tlv->Chan2_ScanType = 2;
    chan_tlv->Chan2_MinScanTime = wlan_cpu_to_le16(50);
    chan_tlv->Chan2_ScanTime = wlan_cpu_to_le16(100);
    chan_tlv->Chan3_RadioType = 0;
    chan_tlv->Chan3_ChanNumber = 11;
    chan_tlv->Chan3_ScanType = 2;
    chan_tlv->Chan3_MinScanTime = wlan_cpu_to_le16(50);
    chan_tlv->Chan3_ScanTime = wlan_cpu_to_le16(100);
    hostcmd->Size += sizeof(tlv) + chan_tlv->len;

    snr_tlv = (snr_header_tlv *)(buf + hostcmd->Size);
    snr_tlv->type = MLANCONFIG_SNR_HEADER_TYPE;
    snr_tlv->len = wlan_cpu_to_le16(SIZEOF_VALUE(snr_header_tlv));
    snr_tlv->SNRValue = 40;
    hostcmd->Size += sizeof(tlv) + snr_tlv->len;

    start_later_tlv = (start_later_header_tlv *)(buf + hostcmd->Size);
    start_later_tlv->type = MLANCONFIG_START_LATER_HEADER_TYPE;
    start_later_tlv->len = wlan_cpu_to_le16(SIZEOF_VALUE(start_later_header_tlv));
    start_later_tlv->StartLaterValue = 0;
    hostcmd->Size += sizeof(tlv) + start_later_tlv->len;

    hostcmd->Size = wlan_cpu_to_le16(hostcmd->Size);

    /* send the bgscan command */
    memset(&iwr, 0, sizeof(iwr));
    iwr.u.data.pointer = (u8 *)hostcmd;
    iwr.u.data.length = wlan_le16_to_cpu(hostcmd->Size);
    iwr.u.data.flags = 0;

    if ((ret = send_iwreq_ioctl(ifname, &iwr, ioctl_val))
        != MWU_ERR_SUCCESS) {
        goto _exit_;
    }

    /* check the response */
    hostcmd->Command = wlan_le16_to_cpu(hostcmd->Command);
    hostcmd->Size = wlan_le16_to_cpu(hostcmd->Size);
    hostcmd->Command &= ~WIFIDIRCMD_RESP_CHECK;
    ret = wlan_le16_to_cpu(hostcmd->Result);

_exit_:
    if (buf)
        free(buf);

    return ret;
}
/* mwu_set_user_scan
 *
 * ifname       - interface name
 * chan_list    - list of scan channel, pls note that caller should init the
 *                unused array elements to 0
 * ssid         - ssid to direct probe req (e.g. WC ssid like DIRECT-*, etc)
 *
 * return       MWU_ERR_SUCCESS if success
 *              MWU_ERR_COM if fail
 */
int mwu_set_user_scan(char *ifname, int chan_list[MWU_IOCTL_USER_SCAN_CHAN_MAX],
                      char *ssid)
{
    mwu_ioctl_user_scan_cfg scan_req;
    int ioctl_val, subioctl_val;
    struct iwreq iwr;
    int i = 0, ret = MWU_ERR_SUCCESS;

    if ((ret = mwu_get_priv_ioctl(ifname, "setuserscan", &ioctl_val,
        &subioctl_val)) != MWU_ERR_SUCCESS) {
        ERR("mwu_get_priv_ioctl FAIL !");
        return ret;
    }

    memset(&scan_req, 0x00, sizeof(scan_req));
    for (i = 0; i < MWU_IOCTL_USER_SCAN_CHAN_MAX; ++i) {
        scan_req.chan_list[i].chan_number = chan_list[i];
        if (scan_req.chan_list[i].chan_number > 14)
            scan_req.chan_list[i].radio_type = 1; /* A Band */
        else
            scan_req.chan_list[i].radio_type = 0; /* BG Band */
    }
    if(ssid != NULL) {
        strncpy(scan_req.ssid_list[0].ssid, strlen(ssid) ? ssid : DEFAULT_SSID,
                MAX_SSID_LEN); /* SSID */
        scan_req.ssid_list[0].max_len = strlen(ssid) ? 0x00 /* Specific scan */
                                                     : 0xFF;
    }

    memset(&iwr, 0, sizeof(iwr));
    iwr.u.data.pointer = (caddr_t)&scan_req;
    iwr.u.data.length  = sizeof(scan_req);
    iwr.u.data.flags   = subioctl_val;

    return send_iwreq_ioctl(ifname, &iwr, ioctl_val);
}

int mwu_set_ap_deauth(char *ifname, unsigned char *sta_mac)
{
    deauth_param param;
    struct ifreq ifr;
    int ret = MWU_ERR_SUCCESS;

    memset(&param, 0, sizeof(deauth_param));
    memset(&ifr, 0, sizeof(ifr));

    /* Assign 0 (reserved) to IEEE reason code. */
    param.reason_code = 0;
    memcpy(&param.mac_addr, sta_mac, ETH_ALEN);

    ifr.ifr_data = (void *) &param;

    return send_ifreq_ioctl(ifname, &ifr, UAP_STA_DEAUTH);
}

int mwu_get_ap_sta_list(char *ifname, ap_sta_list *list)
{
    struct ifreq ifr;

    memset(list, 0, sizeof(list));
    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_ifru.ifru_data = (void *) list;
    return send_ifreq_ioctl(ifname, &ifr, UAP_GET_STA_LIST);
}
