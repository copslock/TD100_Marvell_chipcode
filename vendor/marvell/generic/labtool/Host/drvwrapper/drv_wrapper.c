/** @file drv_wrapper.c
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdlib.h>
#include <poll.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <linux/if.h>
#include <linux/ioctl.h>
#include <linux/wireless.h>
#ifdef NONPLUG_SUPPORT
#ifndef MARVELL_BT_STACK 
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#endif
#endif
#include "drv_wrapper.h"

/********************************************************
        Local Variables
********************************************************/
#define MAXBUF 1024
#define IW_MAX_PRIV_DEF   128

static struct ifreq userdata;
static struct iw_priv_args priv_args[IW_MAX_PRIV_DEF];
static int sockfd=0, ioctl_val, subioctl_val;
#ifdef NONPLUG_SUPPORT
static int hci_sock = 0;
static char *hci_addr = NULL; 
static char addr[18];
static char *hci_intf = "hci0";
#endif
/********************************************************
        Local Functions
********************************************************/
/** 
 *  @brief Get private info.
 *   
 *  @param ifname   A pointer to net name
 *  @return         Number of private IOCTLs, -1 on failure
 */
static int
get_private_info(const char *ifname)
{
    struct iwreq iwr;
    int ret = 0;
    struct iw_priv_args *pPriv = priv_args;

    memset(&iwr, 0, sizeof(iwr));
    strncpy(iwr.ifr_name, ifname, IFNAMSIZ);
    iwr.u.data.pointer = (caddr_t) pPriv;
    iwr.u.data.length = IW_MAX_PRIV_DEF;
    iwr.u.data.flags = 0;

    if (ioctl(sockfd, SIOCGIWPRIV, &iwr) < 0) {
        perror("ioctl[SIOCGIWPRIV]");
        ret = -1;
    } else {
        /* Return the number of private ioctls */
        ret = iwr.u.data.length;
    }

    return ret;
}

/** 
 *  @brief Get Sub command ioctl number
 *   
 *  @param i            command index
 *  @param priv_cnt     Total number of private ioctls availabe in driver
 *  @param ioctl_val    A pointer to return ioctl number
 *  @param subioctl_val A pointer to return sub-ioctl number
 *  @return             0 on success, otherwise -1
 */
static int
get_subioctl_no(int i, int priv_cnt, int *ioctl_val, int *subioctl_val)
{
    int j;

    if (priv_args[i].cmd >= SIOCDEVPRIVATE) {
        *ioctl_val = priv_args[i].cmd;
        *subioctl_val = 0;
        return 0;
    }

    j = -1;
    while ((++j < priv_cnt)
           && ((priv_args[j].name[0] != '\0') ||
               (priv_args[j].set_args != priv_args[i].set_args) ||
               (priv_args[j].get_args != priv_args[i].get_args))) {
    }

    /* If not found... */
    if (j == priv_cnt) {
        printf("Invalid private ioctl definition for: 0x%x\n",
               priv_args[i].cmd);
        return -1;
    }

    /* Save ioctl numbers */
    *ioctl_val = priv_args[j].cmd;
    *subioctl_val = priv_args[i].cmd;

    return 0;
}

/** 
 *  @brief Get ioctl number
 *   
 *  @param ifname       Interface name
 *  @param priv_cmd     Private command name
 *  @param ioctl_val    A pointer to return ioctl number
 *  @param subioctl_val A pointer to return sub-ioctl number
 *  @return             0 on success, otherwise -1
 */
static int
get_ioctl_no(const char *ifname, 
             const char *priv_cmd, int *ioctl_val, int *subioctl_val)
{
    int i, priv_cnt, ret = -1;

    priv_cnt = get_private_info(ifname);

    /* Are there any private ioctls? */
    if (priv_cnt <= 0 || priv_cnt > IW_MAX_PRIV_DEF) {
        /* Could skip this message ? */
        printf("%-8.8s  no private ioctls.\n", ifname);
    } else {
        for (i = 0; i < priv_cnt; i++) {
            if (priv_args[i].name[0] && !strcmp(priv_args[i].name, priv_cmd)) {
                ret = get_subioctl_no(i, priv_cnt, ioctl_val, subioctl_val);
                break;
            }
        }
    }
    return ret;
}

#ifdef NONPLUG_SUPPORT
static int get_hci_dev_info(int s, int dev_id, long arg)
{
#if defined (_BT_) || defined (_FM_)
#ifndef MARVELL_BT_STACK
    struct hci_dev_info di = { dev_id: dev_id };

	if (ioctl(s, HCIGETDEVINFO, (void *) &di))
	      return 0;

	if (!strcmp(di.name, hci_intf)) {
	    ba2str(&di.bdaddr, addr);
	    hci_addr = addr;
	}
#endif
#endif	// #if defined (_BT_) || defined (_FM_)
	return 0;
}

/** 
 *  @brief Get HCI driver info
 *   
 *  @return  0 --- if already loadded, otherwise -1
 */
int drv_wrapper_get_hci_info(char *cmdname)
{
    int ret = 0;
#if defined (_BT_) || defined (_FM_)
#ifndef MARVELL_BT_STACK
    if (!cmdname) {
	    printf("Interface name is not present\n");
		return -1;
	}
	hci_intf = cmdname;
	hci_for_each_dev(HCI_UP, get_hci_dev_info, 0);

	ret = (hci_addr == NULL) ? -1 : 0;
#endif
#endif	// #if defined (_BT_) || defined (_FM_)

    return ret;
}

/** 
 *  @brief drv wrapper initialize HCI
 *   
 *  @param intfname Interface name 
 *  @return  0 on success, otherwise -1
 */
int drv_wrapper_init_hci(char *cmdname)
{
    int ret = 0;
#if defined (_BT_) || defined (_FM_)

#ifdef MARVELL_BT_STACK
    char dev_name[20];
    
    strcpy(dev_name, "/dev/mbtchar0");
    hci_sock = open(dev_name, O_RDWR | O_NOCTTY);
    if (hci_sock >= 0) {
        printf("Use device %s\n", dev_name);    
    } else {
        printf("Cannot open device %s", dev_name);
        ret = -1;
    }        
#else
    if (!cmdname) {
        printf("Interface name is not present\n");
        return -1;
    }
    hci_intf = cmdname;
    hci_addr = NULL;
    printf("Initialize drvwrapper for BT ....\n");
    hci_for_each_dev(HCI_UP, get_hci_dev_info, 0);
    if (hci_addr == NULL) {
        printf("BT interface is not present\n");
        ret = -1;
    } else {
        int hci_dev_id;
        hci_dev_id = hci_get_route((bdaddr_t *)hci_addr);
        hci_sock = hci_open_dev(hci_dev_id);
        ret = 0;
    }
#endif    
#endif	// #if defined (_BT_) || defined (_FM_)
    return ret;
}

/** 
 *  @brief drv wrapper de-initialize HCI
 *   
 *  @return  0 on success, otherwise -1
 */
int drv_wrapper_deinit_hci()
{
    int ret = 0;
#if defined (_BT_) || defined (_FM_)

    printf("De-Initialize drvwrapper for BT....\n");
    if (hci_sock) 
        close(hci_sock);
	hci_addr = NULL;	
#endif	// #if defined (_BT_) || defined (_FM_)
    return ret;
}

/** 
 *  @brief drv wrapper send HCI command
 *   
 *  @return  0 on success, otherwise error code
 */
int drv_wrapper_send_hci_command(short ogf, short ocf,  unsigned char *in_buf, int in_buf_len,
		unsigned char *out_buf, int *out_buf_len)
{
#define EVENT_BUF_SIZE 400
    int status = 0;

#if defined (_BT_) || defined (_FM_)
#ifdef MARVELL_BT_STACK    
    int len;
#else
    struct hci_filter flt;
#endif
    int dummy_buf[512];
    int avail = 0;

#ifdef MARVELL_BT_STACK
	len = write(hci_sock, in_buf, in_buf_len);
    if (len != in_buf_len) {
        printf("Failed to write %d bytes (written %d)\n", in_buf_len, len);
        status = -1;
    }
#else
   /* Setup filter */
    hci_filter_clear(&flt);
    hci_filter_set_ptype(HCI_EVENT_PKT, &flt);
    hci_filter_all_events(&flt);
    if (setsockopt(hci_sock, SOL_HCI, HCI_FILTER, &flt, sizeof(flt)) < 0) {
        printf("HCI filter setup failed\n");
        return (-1);
    }

	status = hci_send_cmd(hci_sock, ogf, ocf, in_buf_len - 4, in_buf + 4);
#endif    
	if (status < 0) {
	    printf("Failed to send command (OGF = %04x OCF = %04x)\n", ogf, ocf);
		return status;
	}

    do {
        *out_buf_len = read(hci_sock, out_buf, EVENT_BUF_SIZE);
        printf("Rx Event %X for %02X %02X \n", out_buf[1], out_buf[4], out_buf[5]);
        printf("In Cmd %02X %02X \n", in_buf[1], in_buf[2]);	
	} while((out_buf[1] != 0xFF) &&  !(out_buf[1] != 0xF && out_buf[4] == in_buf[1] && out_buf[5] == in_buf[2]));

    if (out_buf[1] == 0xFF)
        avail = read(hci_sock, dummy_buf, EVENT_BUF_SIZE);
#endif	// #if defined (_BT_) || defined (_FM_)
    return status;
}
#endif /* NONPLUG_SUPPORT */

/** 
 *  @brief Get driver info
 *   
 *  @return  0 --- if already loadded, otherwise -1
 */
int drv_wrapper_get_info(char *cmdname)
{
    struct ifreq ifr;
    int sk, ret=0;

    /* Open socket to communicate with driver */
    sk = socket(AF_INET, SOCK_STREAM, 0);
    if (sk < 0) {
        printf("Failed to create socket\n");
        return -1;
    }
    strncpy(ifr.ifr_name, cmdname, IFNAMSIZ);
    if(ioctl(sk, SIOCGIFFLAGS, &ifr) < 0)
        ret = -1;
    close(sk);
    return ret;
}

/** 
 *  @brief drv wrapper initialize
 *   
 *  @return  0 on success, otherwise -1
 */
int drv_wrapper_init(char *cmdname)
{
    int ret=0;

    printf("Initialize drvwrapper ....\n");
    /* Open socket to communicate with driver */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("Invalid sockfd, function return -1\n");
        return -1;
    }
    if (get_ioctl_no(cmdname, "hostcmd", &ioctl_val, &subioctl_val)
            == -1) {
        printf("No private IOCTL present for command \"hostcmd\"\n");
		close(sockfd);
		sockfd = 0;
        return -1;
    }

    return ret;
}

/** 
 *  @brief drv wrapper de-initialize
 *   
 *  @return  0 on success, otherwise -1
 */
int drv_wrapper_deinit()
{
    int ret=0;

    printf("De-Initialize drvwrapper ....\n");
    if (sockfd) 
        close(sockfd);
    ioctl_val = 0;
    subioctl_val = 0;
    sockfd = 0;
    return ret;
}

/** 
 *  @brief drv wrapper hostcmd ioctl
 *   
 *  @return  0 on success, otherwise error code
 */
int drv_wrapper_ioctl(char *cmdname, unsigned char *p_buf, int *resp_len)
{
    int status = -1;
    HostCmd_DS_Gen *hostcmd_hdr = (HostCmd_DS_Gen *)p_buf;

    *resp_len = 0;
    strncpy(userdata.ifr_name, cmdname, strlen(cmdname));
    userdata.ifr_data = (char *) p_buf;
    status = ioctl(sockfd, ioctl_val, &userdata);
    *resp_len = hostcmd_hdr->size;

    return status;
}

/** 
 *  @brief drv wrapper bridge command handling
 *   
 *  @return  0 on success, otherwise error code
 */
int
drv_wrapper_bridge_command(char *cmdname, unsigned char *buf, int *length, char *script)
{
    int ret;
    unsigned char pInfo[MAXBUF];
    char command[MAXBUF];
#ifdef NONPLUG_SUPPORT
    int RetBufSize;
    short real_ogf;
    short real_ocf;
    unsigned char EventData[MAXBUF];
#endif
    cmd_header *cmd_hd = (cmd_header *) buf;

    /* Check command type and subtype */
    switch (cmd_hd->type) {
    case TYPE_LOCAL:
        switch (cmd_hd->sub_type) {
        case SUB_TYPE_LOAD_DRV:
            if (!script) {
                printf("Load script is not provided\n");
                break;
            }
            if (!drv_wrapper_get_info(cmdname)) {
                printf("Driver already loaded\n");
            } else {
                sprintf(command,"sh %s",script);
                printf("Load driver ......\n");
                if (system(command) != 0) {
                    printf("Failed to run the script\n");
                } else {
                    drv_wrapper_init(cmdname);
                }
            }
            break;
        case SUB_TYPE_UNLOAD_DRV:
            if (!script) {
                printf("Unload script is not provided\n");
                break;
            }
            if (!drv_wrapper_get_info(cmdname)) {
                sprintf(command,"sh %s",script);
                drv_wrapper_deinit();
                printf("Unload driver ......\n");
                ret = system(command);
                if (ret)
                    printf("Failed to run the script\n");
            } else {
                printf("No such device\n");
            }
            break;
        case SUB_TYPE_GET_BRIDGE_CFG:
            break;
        case SUB_TYPE_SET_BRIDGE_CFG:
            break;
        default:
            printf("Unknown local command subtype: %d\n", cmd_hd->sub_type);
            ret = -1;
        }
        break;

    case TYPE_WLAN:
        printf("WLAN Command\n");
        memcpy(pInfo, (buf + sizeof(cmd_header)), *length - sizeof(cmd_header));
        ret = drv_wrapper_ioctl(cmdname, pInfo, length);
        /* Update command header information */
        cmd_hd->status = ret;
        cmd_hd->length = *length;
        /* Copy the pInfo back to the receive buffer */
        memcpy((buf + sizeof(cmd_header)), pInfo, *length);
        *length += sizeof(cmd_header);
        break;
#ifdef NONPLUG_SUPPORT
#if defined (_BT_) || defined (_FM_)
    case TYPE_HCI:
        memcpy(pInfo, (buf + sizeof(cmd_header)), *length - sizeof(cmd_header));
        real_ogf = pInfo[2] >> 2;
        real_ocf =  ((pInfo[2] & 0x03) << 8) + pInfo[1];
        printf("HCI Command (OGF = %04x OCF = %04x)\n", real_ogf, real_ocf);
        ret = drv_wrapper_send_hci_command(real_ogf, real_ocf, pInfo, (*length - sizeof(cmd_header)), EventData, &RetBufSize);
        /* Update command header information */
        cmd_hd->status = ret;
        cmd_hd->length = RetBufSize + sizeof(cmd_header);
        /* Copy the pInfo back to the receive buffer */
        memcpy((buf + sizeof(cmd_header)), EventData, RetBufSize);
        *length = RetBufSize  + sizeof(cmd_header);
        break;
#endif	// #if defined (_BT_) || defined (_FM_)
#endif
    default:
        printf("Unknown command type: %d\n", cmd_hd->type);
        break;
    }
    return ret;
}
