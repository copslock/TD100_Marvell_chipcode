/** @file wps_os.c
 *  @brief This file contains timer and socket read functions.
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

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <ctype.h>
#include <netinet/in.h>
#include <linux/if.h>
#include <linux/netlink.h>
#include  <arpa/inet.h>

#include "wps_msg.h"
#include "wps_def.h"
#include "mwu_log.h"
#include "util.h"
#include "wps_l2.h"
#include "wps_eapol.h"
#include "wps_os.h"
#include "wps_wlan.h"

/** Netlink protocol number */
#define NETLINK_MARVELL         (MAX_LINKS - 1)
/** Netlink multicast group number */
#define NL_MULTICAST_GROUP      1
/** Netlink maximum payload size */
#define NL_MAX_PAYLOAD          1024

/********************************************************
        Local Variables
********************************************************/
/* Data structure definition for main loop */
struct wps_sock_s
{
    /** socket no */
    int sock;
    /** private data for callback */
    void *callback_data;
    /** handler */
    void (*handler) (int sock, void *sock_ctx);
};

struct wps_timeout_s
{
    /** next pointer */
    struct wps_timeout_s *next;
    /** time */
    struct timeval time;
    /** private data for callback */
    void *callback_data;
    /** timeout handler */
    void (*handler) (void *timeout_ctx);
};

typedef struct wps_loop_s
{
    /** terminate */
    int terminate;
    /** max socket number */
    int max_sock;
    /** read count */
    int reader_count;
    /** read socket */
    struct wps_sock_s *readers;
    /** timeout */
    struct wps_timeout_s *timeout;
} WPS_LOOP_S;

static WPS_LOOP_S wps_loop;

/********************************************************
        Global Variables
********************************************************/
/** IE buffer index */
extern short ie_index;
/** IE buffer index */
extern short ap_assocresp_ie_index;

/** Global pwps information */
extern PWPS_INFO gpwps_info;

extern int wps_wlan_init(WPS_DATA * wps_s);
extern int wps_clear_running_instance(void);

/********************************************************
        Local Functions
********************************************************/
/**
 *  @brief Free structure used in main loop function
 *
 *  @return         None
 */
static void
wps_main_loop_free(void)
{
    struct wps_timeout_s *timeout, *prev;

    ENTER();

    timeout = wps_loop.timeout;
    while (timeout != NULL) {
        prev = timeout;
        timeout = timeout->next;
        free(prev);
    }
    free(wps_loop.readers);

    LEAVE();
}

/**
 *  @brief Linux alarm signal handler
 *
 *  @param sig_num      signal number
 *  @return             None
 */
static void
wps_alarm_handler(int sig_num)
{
    ENTER();

    if (sig_num != SIGALRM)
        return;

    printf("\n");
    LEAVE();
    exit(1);
}

/**
 *  @brief Linux signal handler
 *
 *  @param sig_num      signal number
 *  @return             None
 */
static void
wps_signal_handler(int sig_num)
{
    static int flag = 0;
    PWPS_DATA wps_s = (WPS_DATA *) &gpwps_info->wps_data;

    ENTER();

    printf("\n");
    if ((sig_num == SIGINT || sig_num == SIGTERM) && !flag) {
        flag++;

        wps_main_loop_shutdown();

        if (ie_index != -1) {
            /* clear IE buffer */
            wps_wlan_ie_config(CLEAR_WPS_IE, &ie_index);
            ie_index = -1;
        }
#ifdef WSC2
        if (wps_s->bss_type == BSS_TYPE_UAP &&
            gpwps_info->registrar.version >= WPS_VERSION_2DOT0) {
            /* Clear session start IEs for Assoc response */
            wps_wlan_ie_config(CLEAR_AR_WPS_IE, &ap_assocresp_ie_index);
        }
#endif

        wps_s->userAbort = WPS_SET;

        wps_clear_running_instance();

        /* Use SIGALRM to break out from potential busy loops that would not
           allow the program to be killed. */
        signal(SIGALRM, wps_alarm_handler);
        alarm(2);
    }

    LEAVE();
}

/********************************************************
        Global Functions
********************************************************/

/**
 *  @brief Process reception of event from wlan
 *
 *  @param fd           File descriptor for reading
 *  @param context      A pointer to user private information
 *  @return             None
 */
void
wps_event_receive(int fd, void *context)
{
    u8 evt_buffer[EVENT_MAX_BUF_SIZE];
    int count = -1;
    WPS_DATA *wps_s = (WPS_DATA *) & gpwps_info->wps_data;
    char if_name[IFNAMSIZ + 1];
    u32 event_id = 0;

    ENTER();

    count = recvmsg(fd, (struct msghdr *) wps_s->evt_msg_head, 0);

    if (count < 0) {
        mwu_printf(MSG_ERROR, "ERR:NETLINK read failure!\n");
        LEAVE();
        return;
    }
    if (count <= NLMSG_HDRLEN) {
        mwu_printf(MSG_ERROR, "ERR:NETLINK read insufficient data!\n");
        LEAVE();
        return;
    }
    mwu_printf(DEBUG_EVENT, "Received event payload (%d)\n", count);
    if (count > NLMSG_SPACE(NL_MAX_PAYLOAD)) {
        mwu_printf(MSG_INFO, "ERR: Buffer overflow!\n");
        LEAVE();
        return;
    }
    memcpy(evt_buffer, NLMSG_DATA((struct nlmsghdr *) wps_s->evt_nl_head),
           count - NLMSG_HDRLEN);
    mwu_hexdump(DEBUG_EVENT, "New event", evt_buffer, count - NLMSG_HDRLEN);

    memcpy(&event_id, evt_buffer, sizeof(event_id));
    if (((event_id & 0xFF000000) == 0x80000000) ||
        ((event_id & 0xFF000000) == 0)) {
        wps_wlan_event_parser(context, wps_s, (char *) evt_buffer,
                              count - NLMSG_HDRLEN);
    } else {
        memset(if_name, 0, IFNAMSIZ + 1);
        memcpy(if_name, evt_buffer, IFNAMSIZ);
        mwu_printf(DEBUG_EVENT, "EVENT on interface %s\n", if_name);
        /* Drop un-needed events */
        if (strncmp(wps_s->ifname, if_name, strlen(if_name))) {
            mwu_printf(MSG_INFO, "Dropped event on %s\n", if_name);
            LEAVE();
            return;
        }
        wps_wlan_event_parser(context, wps_s, (char *) (evt_buffer + IFNAMSIZ),
                              count - NLMSG_HDRLEN - IFNAMSIZ);
    }
    LEAVE();
    return;
}

/**
 *  @brief Determine the netlink number
 *
 *  @return         Netlink number to use
 */
static int
get_netlink_num(void)
{
    FILE *fp;
    int netlink_num = NETLINK_MARVELL;
    char str[64];
    char *srch = "netlink_num";

    /* Try to open /proc/mwlan/config */
    fp = fopen("/proc/mwlan/config", "r");
    if (fp) {
        while (!feof(fp)) {
            fgets(str, sizeof(str), fp);
            if (strncmp(str, srch, strlen(srch)) == 0) {
                netlink_num = atoi(str + strlen(srch) + 1);
                break;
            }
        }
        fclose(fp);
    }

    mwu_printf(DEBUG_INIT, "Netlink number = %d\n", netlink_num);
    return netlink_num;
}

/**
 *  @brief Process event socket initialization
 *
 *  @param wps_s    A pointer to global WPS structure
 *  @return         WPS_STATUS_SUCCESS--success, otherwise--fail
 */
int
wps_event_init(WPS_DATA * wps_s)
{
    int fd = 0;
    int netlink_num = 0;
    int ret = WPS_STATUS_SUCCESS;
    struct nlmsghdr *nlh = NULL;
    struct sockaddr_nl src_addr, *ptr_dest_addr = NULL;
    struct msghdr *pmsg = NULL;
    struct iovec *piov = NULL;

    ENTER();

    netlink_num = get_netlink_num();

    fd = socket(PF_NETLINK, SOCK_RAW, netlink_num);
    if (fd < 0) {
        mwu_printf(MSG_ERROR, "ERR: netlink socket creation error.\n");
        ret = WPS_STATUS_FAIL;
        goto done;
    }

    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid();
    src_addr.nl_groups = NL_MULTICAST_GROUP;
    if (bind(fd, (struct sockaddr *) &src_addr, sizeof(src_addr)) < 0) {
        mwu_printf(MSG_ERROR, "ERR: Bind netlink socket\n");
        ret = WPS_STATUS_FAIL;
        goto done;
    }

    /* Initialize netlink header & msg header */
    nlh = (struct nlmsghdr *) malloc(NLMSG_SPACE(NL_MAX_PAYLOAD));
    if (!nlh) {
        mwu_printf(MSG_ERROR, "ERR: Could not alloc buffer\n");
        ret = WPS_STATUS_FAIL;
        goto done;
    }
    memset(nlh, 0, NLMSG_SPACE(NL_MAX_PAYLOAD));
    pmsg = (struct msghdr *) malloc(sizeof(struct msghdr));
    if (!pmsg) {
        mwu_printf(MSG_ERROR, "ERR: Could not alloc buffer\n");
        ret = WPS_STATUS_FAIL;
        goto msg_error;
    }
    memset(pmsg, 0, sizeof(*pmsg));

    ptr_dest_addr = (struct sockaddr_nl *) malloc(sizeof(struct sockaddr_nl));
    if (!ptr_dest_addr) {
        mwu_printf(MSG_ERROR, "ERR: Could not alloc buffer\n");
        ret = WPS_STATUS_FAIL;
        goto addr_error;
    }
    memset(ptr_dest_addr, 0, sizeof(*ptr_dest_addr));

    piov = (struct iovec *) malloc(sizeof(struct iovec));
    if (!piov) {
        mwu_printf(MSG_ERROR, "ERR: Could not alloc buffer\n");
        ret = WPS_STATUS_FAIL;
        goto piov_error;
    }
    memset(piov, 0, sizeof(*piov));

    /* Set destination address */
    ptr_dest_addr->nl_family = AF_NETLINK;
    ptr_dest_addr->nl_pid = 0;  /* Kernel */
    ptr_dest_addr->nl_groups = NL_MULTICAST_GROUP;

    /* Initialize I/O vector */
    piov->iov_base = (void *) nlh;
    piov->iov_len = NLMSG_SPACE(NL_MAX_PAYLOAD);

    /* Initialize message header */
    memset(pmsg, 0, sizeof(struct msghdr));
    pmsg->msg_name = (void *) ptr_dest_addr;
    pmsg->msg_namelen = sizeof(*ptr_dest_addr);
    pmsg->msg_iov = piov;
    pmsg->msg_iovlen = 1;

    wps_s->evt_fd = fd;
    wps_s->evt_nl_head = nlh;
    wps_s->evt_msg_head = pmsg;
    wps_s->evt_iov = piov;
    wps_s->evt_dest_addr = ptr_dest_addr;

    /* register the read socket handler for event packet */
    wps_register_rdsock_handler(fd, wps_event_receive, gpwps_info);

    LEAVE();
    return ret; /* success */

  /* failure */
  piov_error:
    free(ptr_dest_addr);
  addr_error:
    free(pmsg);
  msg_error:
    free(nlh);
  done:
    LEAVE();
    return ret;
}

/**
 *  @brief Process main loop initialization
 *
 *  @param wps_s    A pointer to global WPS structure
 *  @return         WPS_STATUS_SUCCESS--success, otherwise--fail
 */
int
wps_loop_init(WPS_DATA * wps_s)
{
    ENTER();

    /* clear data structure for main processing loop */
    memset(&wps_loop, 0, sizeof(wps_loop));

    /* Initial and hook l2 callback function */
    wps_s->l2 = wps_l2_init(wps_s->ifname, ETH_P_EAPOL, wps_rx_eapol, 0);

    if (wps_s->l2 == NULL) {
        mwu_printf(DEBUG_INIT, "Init l2 return NULL !\n");
        LEAVE();
        return WPS_STATUS_FAIL;
    }

    if (wps_s->l2 && wps_l2_get_mac(wps_s->l2, wps_s->my_mac_addr)) {
        mwu_printf(DEBUG_INIT, "Failed to get own L2 address\n");
        LEAVE();
        return WPS_STATUS_FAIL;
    }

    mwu_printf(DEBUG_INIT, "Own MAC address: " UTIL_MACSTR,
               UTIL_MAC2STR(wps_s->my_mac_addr));

    if (wps_wlan_init(wps_s) != WPS_STATUS_SUCCESS) {
        mwu_printf(DEBUG_INIT, "wps_driver_config_init fail\n");
        LEAVE();
        return WPS_STATUS_FAIL;
    }

    wps_s->scan_results = NULL;
    wps_s->num_scan_results = 0;

    LEAVE();
    return WPS_STATUS_SUCCESS;
}

/**
 *  @brief Process event handling free
 *
 *  @param wps_s        A pointer to global WPS structure
 *  @return             None
 */
void
wps_event_deinit(WPS_DATA * wps_s)
{
    ENTER();

    if (wps_s == NULL) {
        LEAVE();
        return;
    }

    if (wps_s->evt_nl_head) {
        free(wps_s->evt_nl_head);
        wps_s->evt_nl_head = NULL;
    }

    if (wps_s->evt_dest_addr) {
        free(wps_s->evt_dest_addr);
        wps_s->evt_dest_addr = NULL;
    }

    if (wps_s->evt_msg_head) {
        free(wps_s->evt_msg_head);
        wps_s->evt_msg_head = NULL;
    }

    if (wps_s->evt_iov) {
        free(wps_s->evt_iov);
        wps_s->evt_iov = NULL;
    }

    if (wps_s->evt_fd > 0) {
        wps_unregister_rdsock_handler(wps_s->evt_fd);
        close(wps_s->evt_fd);
        wps_s->evt_fd = 0;
    }

    LEAVE();
}

/**
 *  @brief Process interface deinit
 *
 *  @param wps_s        A pointer to global WPS structure
 *  @return             None
 */
void
wps_intf_deinit(WPS_DATA * wps_s)
{
    ENTER();

    if (wps_s == NULL) {
        LEAVE();
        return;
    }

    wps_l2_deinit(wps_s->l2);
    wps_wlan_deinit(wps_s);

    LEAVE();
}

/**
 *  @brief Process main loop free
 *
 *  @param wps_s        A pointer to global WPS structure
 *  @return             None
 */
void
wps_loop_deinit(WPS_DATA * wps_s)
{
    ENTER();

    if (wps_s == NULL) {
        LEAVE();
        return;
    }

    if (wps_s->scan_results)
        free(wps_s->scan_results);

    wps_s->scan_results = NULL;
    wps_s->num_scan_results = 0;

    wps_main_loop_free();

    LEAVE();
}

/**
 *  @brief Disable main loop
 *
 *  @return         None
 */
void
wps_main_loop_shutdown(void)
{
    ENTER();
    wps_loop.terminate = WPS_SET;
    LEAVE();
}

/**
 *  @brief Enable main loop
 *
 *  @return         None
 */
void
wps_main_loop_enable(void)
{
    ENTER();
    wps_loop.terminate = WPS_CANCEL;
    LEAVE();
}

/**
 *  @brief Register the signal handler to Linux kernel
 *
 *  @return         WPS_STATUS_SUCCESS--success, otherwise--fail
 */
int
wps_set_signal_handler(void)
{
    ENTER();

    /*
     * register signal handler to OS for pressing CTRL+C to terminate program
     */
    signal(SIGINT, wps_signal_handler);
    signal(SIGTERM, wps_signal_handler);

    LEAVE();
    return WPS_STATUS_SUCCESS;
}

/**
 *  @brief Register a handler to main loop socket read function
 *
 *  @param sock             Socket number
 *  @param handler          A function pointer of read handler
 *  @param callback_data    Private data for callback
 *  @return         WPS_STATUS_SUCCESS--success, otherwise--fail
 */
int
wps_register_rdsock_handler(int sock,
                            void (*handler) (int sock, void *sock_ctx),
                            void *callback_data)
{
    struct wps_sock_s *tmp;

    ENTER();

    tmp = (struct wps_sock_s *) realloc(wps_loop.readers,
                                        (wps_loop.reader_count +
                                         1) * sizeof(struct wps_sock_s));
    if (tmp == NULL)
        return WPS_STATUS_FAIL;

    tmp[wps_loop.reader_count].sock = sock;
    tmp[wps_loop.reader_count].callback_data = callback_data;
    tmp[wps_loop.reader_count].handler = handler;
    wps_loop.reader_count++;
    wps_loop.readers = tmp;
    if (sock > wps_loop.max_sock)
        wps_loop.max_sock = sock;

    LEAVE();
    return WPS_STATUS_SUCCESS;
}

/**
 *  @brief Unregister a handler to main loop socket read function
 *
 *  @param sock     Socket number
 *  @return         None
 */
void
wps_unregister_rdsock_handler(int sock)
{
    int i;

    ENTER();

    if (wps_loop.readers == NULL || wps_loop.reader_count == 0) {
        LEAVE();
        return;
    }

    for (i = 0; i < wps_loop.reader_count; i++) {
        if (wps_loop.readers[i].sock == sock)
            break;
    }
    if (i == wps_loop.reader_count)
        return;
    if (i != wps_loop.reader_count - 1) {
        memmove(&wps_loop.readers[i], &wps_loop.readers[i + 1],
                (wps_loop.reader_count - i - 1) * sizeof(struct wps_sock_s));
    }
    wps_loop.reader_count--;

    LEAVE();
}

/**
 *  @brief Register a time-out handler to main loop timer function
 *
 *  @param secs             Time-out value in seconds
 *  @param usecs            Time-out value in micro-seconds
 *  @param handler          A function pointer of time-out handler
 *  @param callback_data    Private data for callback
 *  @return         WPS_STATUS_SUCCESS--success, otherwise--fail
 */
int
wps_start_timer(unsigned int secs,
                unsigned int usecs,
                void (*handler) (void *user_data), void *callback_data)
{
    struct wps_timeout_s *timeout, *tmp, *prev;

    ENTER();

    timeout = (struct wps_timeout_s *) malloc(sizeof(*timeout));
    if (timeout == NULL)
        return WPS_STATUS_FAIL;

    gettimeofday(&timeout->time, NULL);
    timeout->time.tv_sec += secs;
    timeout->time.tv_usec += usecs;
    while (timeout->time.tv_usec >= 1000000) {
        timeout->time.tv_sec++;
        timeout->time.tv_usec -= 1000000;
    }

    timeout->callback_data = callback_data;
    timeout->handler = handler;
    timeout->next = NULL;

    if (wps_loop.timeout == NULL) {
        wps_loop.timeout = timeout;
        LEAVE();
        return WPS_STATUS_SUCCESS;
    }

    prev = NULL;
    tmp = wps_loop.timeout;
    while (tmp != NULL) {
        if (timercmp(&timeout->time, &tmp->time, <))
            break;
        prev = tmp;
        tmp = tmp->next;
    }

    if (prev == NULL) {
        timeout->next = wps_loop.timeout;
        wps_loop.timeout = timeout;
    } else {
        timeout->next = prev->next;
        prev->next = timeout;
    }

    LEAVE();
    return WPS_STATUS_SUCCESS;
}

/**
 *  @brief Cancel time-out handler to main loop timer function
 *
 *  @param handler          Time-out handler to be canceled
 *  @param callback_data    Private data for callback
 *  @return         Number of timer being removed
 */
int
wps_cancel_timer(void (*handler) (void *timeout_ctx), void *callback_data)
{
    struct wps_timeout_s *timeout, *prev, *next;
    int removed = 0;

    ENTER();

    prev = NULL;
    timeout = wps_loop.timeout;
    while (timeout != NULL) {
        next = timeout->next;

        if ((timeout->handler == handler) &&
            (timeout->callback_data == callback_data)) {
            if (prev == NULL)
                wps_loop.timeout = next;
            else
                prev->next = next;
            free(timeout);
            removed++;
        } else
            prev = timeout;

        timeout = next;
    }

    LEAVE();
    return removed;
}

/**
 *  @brief Main loop procedure for socket read and timer functions
 *
 *  @return             None
 */
void
wps_main_loop_proc(void)
{
    int i, res;
    struct timeval tv, now;
    struct timeval tv_nowait;
    fd_set *rfds;

    ENTER();

    rfds = malloc(sizeof(*rfds));
    if (rfds == NULL) {
        mwu_printf(MSG_ERROR, "wps_main_loop_proc : malloc failed !\n");
        return;
    }

    while (!wps_loop.terminate &&
           (wps_loop.timeout || wps_loop.reader_count > 0)) {
        if (wps_loop.timeout) {
            gettimeofday(&now, NULL);
            if (timercmp(&now, &wps_loop.timeout->time, <))
                timersub(&wps_loop.timeout->time, &now, &tv);
            else
                tv.tv_sec = tv.tv_usec = 0;
        }

        FD_ZERO(rfds);

        for (i = 0; i < wps_loop.reader_count; i++) {
            FD_SET(wps_loop.readers[i].sock, rfds);
        }

        /* Be sure to reset timeout each time because some select
         * implementations alter it.
         */
        tv_nowait.tv_sec = 0;
        tv_nowait.tv_usec = 100000;
        res = select(wps_loop.max_sock + 1, rfds, NULL, NULL,
                     wps_loop.timeout ? &tv : &tv_nowait);

        if (res < 0 && errno != EINTR) {
            mwu_printf(MSG_ERROR, "select loop error\n");
            perror("select()");
            free(rfds);
            LEAVE();
            return;
        }

        /* check if some registered timeouts have occurred */
        if (wps_loop.timeout) {
            struct wps_timeout_s *tmp;

            gettimeofday(&now, NULL);
            if (!timercmp(&now, &wps_loop.timeout->time, <)) {
                tmp = wps_loop.timeout;
                wps_loop.timeout = wps_loop.timeout->next;
                tmp->handler(tmp->callback_data);
                free(tmp);
            }
        }

        if (res <= 0) {
            continue;
        }

        /* call socket read handler function */
        for (i = 0; i < wps_loop.reader_count; i++) {
            if (FD_ISSET(wps_loop.readers[i].sock, rfds)) {
                wps_loop.readers[i].handler(wps_loop.readers[i].sock,
                                            wps_loop.readers[i].callback_data);
            }
        }
    }

    free(rfds);
    LEAVE();
}

/**
 *  @brief Function to download the IPv4 CFG to the interface
 *
 *  @return WPS_STATUS_SUCCESS - Success
 *  @return WPS_STATUS_FAIL    - Failure
 */
int
wps_install_current_ipv4_cfg(char *ifname, u32 ip_addr, u32 subnet_mask)
{

    struct ifreq ifr;
    struct sockaddr_in sin;
    int ret_ip = 0, ret_subnet = 0, skfd = 0;

    ENTER();

    skfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (skfd < 0) {
        perror("socket[AF_INET, SOCK_DGRAM]");
        LEAVE();
        return WPS_STATUS_FAIL;
    }

    strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
    memset(&sin, 0, sizeof(struct sockaddr));
    sin.sin_family = AF_INET;

    sin.sin_addr.s_addr = htonl(ip_addr);
    memcpy(&ifr.ifr_addr, &sin, sizeof(struct sockaddr));
    ret_ip = ioctl(skfd, SIOCSIFADDR, &ifr);
    if (ret_ip < 0)
        perror("SIOCSIFADDR");

    sin.sin_addr.s_addr = htonl(subnet_mask);
    memcpy(&ifr.ifr_addr, &sin, sizeof(struct sockaddr));
    ret_subnet = ioctl(skfd, SIOCSIFNETMASK, &ifr);
    if (ret_subnet < 0)
        perror("SIOCSIFNETMASK");

    if (ret_ip < 0 || ret_subnet < 0) {
        close(skfd);
        LEAVE();
        return WPS_STATUS_FAIL;
    }

    close(skfd);
    LEAVE();
    return WPS_STATUS_SUCCESS;

}

void wps_generate_nonce_16B(u8 * buf)
{
    int i, randNum, seed;
    struct timeval now;

    ENTER();

    gettimeofday(&now, NULL);
    seed = now.tv_sec | now.tv_usec;

    for (i = 0; i < 4; i++) {
        srand(seed);
        randNum = rand();
        memcpy(buf + i * 4, &randNum, sizeof(int));
        seed = seed * 1103515245 + 12345 * i;
    }

    LEAVE();
}

int wps_scan_networks(WPS_DATA * wps_s)
{
    int ret, index;
    char inp[4];
    struct timeval start, now, res;
    struct timeval pbcWalkTime;
    MESSAGE_ENROLLEE_REGISTRAR *en_reg;

    printf("\nStart Active Scan ...\n");

    memset(&pbcWalkTime, 0, sizeof(pbcWalkTime));
    en_reg = ((gpwps_info->role == WPS_ENROLLEE) ? &gpwps_info->enrollee :
              &gpwps_info->registrar);

    if (en_reg->device_password_id == DEVICE_PASSWORD_PUSH_BUTTON &&
        gpwps_info->pbc_auto == WPS_SET) {
        /* Set PBC Walk Timer to 120 second */
        pbcWalkTime.tv_sec = PBC_WALK_TIME;
        pbcWalkTime.tv_usec = 0;

        /* Mark the start of PBC Walk Timer */
        gettimeofday(&start, NULL);
    }

    do {
        wps_wlan_scan(wps_s);
        if (en_reg->device_password_id == DEVICE_PASSWORD_PUSH_BUTTON &&
            gpwps_info->pbc_auto == WPS_SET) {

            /* Compare time of PBC Walk Timer */
            gettimeofday(&now, NULL);
            timersub(&now, &start, &res);
            if (timercmp(&res, &pbcWalkTime, >=)) {
                printf("\n");
                printf("\nPBC walk time expired.\n");
                printf("\n\n");
                ret = WPS_STATUS_FAIL;
                goto done;
            }

            wps_wlan_scan_results(wps_s, FILTER_PBC);
            if (wps_s->num_scan_results > 0) {
                ret = wps_wlan_check_for_PBC_overlap(gpwps_info, wps_s);
                goto done;
            } else {
                sleep(3);
                printf("\nScan for PBC AP Again!\n");
                continue;
            }
        } else {
            wps_wlan_scan_results(wps_s, FILTER_NONE);
        }

        printf("\n");
        printf("Select Target SSID Option\n");
        printf("[0]. Quit\n");
        printf("[1]. Searching Again\n");
        printf("[2]. Connect\n");

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
        } else if (index == 2) {
            break;
        }
    } while (1);

    if (wps_s->num_scan_results == 0) {
        printf("\n\nScan Result is empty !\n");
        ret = WPS_STATUS_FAIL;
    } else {
        ret = WPS_STATUS_SUCCESS;
    }

  done:
    return ret;
}
