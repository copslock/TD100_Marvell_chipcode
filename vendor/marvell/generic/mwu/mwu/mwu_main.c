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
/** mwu_main.c (derived from wps_main.c)
 *
 * mwu is a daemon that manages wifi-direct, wps, wpa connectivity.  After
 * launch, it is controlled via a socket interface.  The control application
 * that sends commands to and receives events from the socket interface is
 * usually written custom for the system.  See mwu_cli for an example of such
 * an application.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <signal.h>

#define UTIL_LOG_TAG "WIFIDIR SM"
#include "version.h"
#include "wps_def.h"
#include "wps_msg.h"
#include "mwu_log.h"
#include "mwu_ioctl.h"
#include "lockf.h"
#include "wps_os.h"
#include "wps_wlan.h"
#include "wlan_hostcmd.h"
#include "wlan_wifidir.h"
#include "util.h"
#include "mwu_internal.h"
#include "mwu_test.h"
#include "wifidir_mwu.h"
#include "wifidir.h"
#include "mwpamod_mwu.h"
#include "mwpamod.h"
#include "mwpsmod_mwu.h"
#include "mwpsmod.h"
#include "queue.h"

#define PID_FILE "wifidir.pid"
#ifdef ANDROID
#define RUN_DIR "/tmp"
#define DEFAULT_WPA_OUTFILE "/data/wpas_wifidir.conf"
#else
#define RUN_DIR "/var/run"
#define DEFAULT_WPA_OUTFILE "/tmp/wpas_wifidir.conf"
#endif

/* location of output file where wpa credential will be written. */
char *wpa_outfile = DEFAULT_WPA_OUTFILE;

/** Configuration file for initialization */
char *init_cfg_file;

/** WIFIDIR Config file for initial download */
extern char *wifidir_cfg_file;

/* The following functions and variables come from Marvell's wps_main.c and
 * other.  Clearly, these functions and data should be available through a
 * proper API, or replaced with suitable local alternatives.
 */
int marvell_get_ioctl_no(const s8 * ifname, const s8 * priv_cmd,
                         int *ioctl_val, int *subioctl_val);
int wps_init(WPS_DATA * wps_s, char *init_cfg_file);
int wps_start(WPS_DATA * wps_s);
extern short ie_index;
int wps_stop(WPS_DATA * wps_s);
void wps_deinit(void);
int wps_clear_running_instance(void);

PWPS_INFO gpwps_info;

static void wifidir_signal_handler(int sig_num)
{
    static int flag = 0;

    ENTER();

    if ((sig_num == SIGINT || sig_num == SIGTERM) && !flag) {
        flag++;

        wps_main_loop_shutdown();

        if (ie_index != -1) {
            /* clear IE buffer */
            wps_wlan_ie_config(CLEAR_WPS_IE, &ie_index);
            ie_index = -1;
        }

        gpwps_info->wps_data.userAbort = WPS_SET;

        wps_clear_running_instance();

    }

    LEAVE();
}

static void wifidir_help_menu(char *program)
{

    printf("mwu: Marvell Wireless Utility Daemon\n");
    printf("Usage: mwu [options]\n\n");
    printf("Options:\n");
    printf("-h                   Print this help message\n\n");
    printf("-v                   Print version and exit\n\n");
    printf("-c <wps_cfg_file>    Configuration file for WPS behavior\n");
    printf("                     Default is %s\n\n", FILE_WPSINIT_CONFIG_NAME);
    printf("-d <wifidir_cfg_file>    Configuration file for WIFIDIR behavior.\n");
    printf("                     Default is none.\n\n");
    printf("-i <iface>           Interface to operate on.\n\n");
    printf("-p <pin>             8-digit pin to use for wps registration.\n");
    printf("                     If 00000000 is supplied, PBC mode will\n");
    printf("                     be used.\n\n");
    printf("-B                   Run in the background (i.e., daemonize)\n\n");
    printf("-l <logfile>         Print to logfile instead of stdout\n\n");
    printf("-V                   Be verbose.\n\n");
    return;
}

static enum mwu_error
mwu_private_info_allocate(PWPS_INFO * pwps_info)
{
    PWPS_INFO temp;

    if (!(temp = calloc(sizeof(WPS_INFO), 1))) {
        mwu_printf(MSG_ERROR, "Allocate buffer for WPS_INFO failed!\n");
        LEAVE();
        return MWU_ERR_COM;
    }
    *pwps_info = temp;

    return MWU_ERR_SUCCESS;
}

/**
 *  @brief Get WPS_INFO structure
 *
 *  @return             A pointer of global WPS_INFO structure
 */
PWPS_INFO
wps_get_private_info(void)
{
    return gpwps_info;
}


extern char *optarg;
extern int optind, opterr, optopt;
int main(int argc, char *argv[])
{
    int exitcode;
    WPS_DATA *wps_s;
    PWPS_INFO pwps_info;
    int opt;
    char *wps_pin = NULL;
    int i;
    s8 tmp_iface[IFNAMSIZ + 1];
    int ifnum;
    short index;
    int ret;
    char *logfile = NULL;
    int background = 0;
    int lock;
    char str[sizeof(pid_t)*2 + 2]; /* just for sprintfing the PID\n\0 */

    exitcode = WPS_STATUS_SUCCESS;

    INFO("**********************************\n");
    INFO("          VERSION %s\n", VERSION);
    INFO("**********************************\n");
    /* initialize default arguments */
    wifidir_cfg_file = NULL;
    init_cfg_file = FILE_WPSINIT_CONFIG_NAME;

    /* Allocate memory for global wps_info data struct */
    ret = mwu_private_info_allocate(&gpwps_info);
	if (ret != MWU_ERR_SUCCESS) {
		ERR("Failed to allocate global data for wps\n");
		return ret;
	}

	/* Show me the info! */
	pwps_info = wps_get_private_info();
	wps_s = (WPS_DATA *) & pwps_info->wps_data;

    memset(wps_s->ifname, 0, IFNAMSIZ + 1);

    while ((opt = getopt(argc, argv, "hac:d:i:p:Bl:V")) != -1) {
        switch ((char)opt) {
        case 'h':
            wifidir_help_menu(argv[0]);
            return WPS_STATUS_SUCCESS;

        case 'c':
            init_cfg_file = optarg;
            break;

        case 'd':
            wifidir_cfg_file = optarg;
            break;

        case 'i':
            if (sscanf(optarg, "wfd%d", &ifnum) != 1) {
                ERR("Invalid iface %s.  Only wfd ifaces are supported.\n",
                    optarg);
                return WPS_STATUS_FAIL;
            }
            strncpy(wps_s->ifname, optarg, IFNAMSIZ);
            break;

        case 'p':
            wps_pin = optarg;
            break;

        case 'B':
            background = 1;
            break;

        case 'l':
            logfile = optarg;
            break;

        case 'V':
            mwu_set_debug_level(DEBUG_ALL);
            break;

        default: /* '?' */
            ERR("Unsupported option: -%c\n", (char)opt);
            return WPS_STATUS_FAIL;
        }
    }

    /* Never run more than one instance of mwu */
    chdir(RUN_DIR);
    lock = open(PID_FILE, O_RDWR|O_CREAT, 0640);
    if (lock < 0) {
        ERR("Failed to create PID file %s\n", PID_FILE);
        return errno;
    }
    ret = lockf(lock, F_TEST, 0);
    if (-1 == ret && (errno == EACCES || errno == EAGAIN)) {
        ERR("pid file is locked.  If mwu is not running, rm %s\n", PID_FILE);
        return WPS_STATUS_FAIL;
    }
    if (ret != 0) {
        perror("failed to check lock file");
        return errno;
    }

    INFO("Initializing interface '%s'", wps_s->ifname);

    /* start us off in STA mode. UAP mode have no deepsleep ioctl */
    mwu_set_bss_type(wps_s->ifname, 0);

    /*
     * 1. Initialize L2 packet interface for receiving EAP packet.
     * 2. Get L2 MAC address and store to application global structure
     * 3. Create socket for wlan driver ioctl
     */
    if (wps_loop_init(wps_s) != 0) {
        exitcode = WPS_STATUS_FAIL;
        ERR("Fail to initialize layer 2 socket !\n");
        goto exit;
    }

    if (exitcode == WPS_STATUS_SUCCESS)
        exitcode = wps_set_signal_handler();

    /* Clear all custom IEs for all interfaces, starting with the specified
     * interface, which is required to be wfdX.
     */
    index = 0;
    if (wps_wlan_ie_config(CLEAR_WPS_IE, &index))
        WARN("Failed to clear IEs 0-1 on %s.\n",
                   wps_s->ifname);
    index = 2;
    if (wps_wlan_ie_config(CLEAR_WPS_IE, &index))
        WARN("Failed to clear IEs 2-3 on %s.\n",
                   wps_s->ifname);

    /*
     * 1. Read the config file
     * 2. Initialize parameter needed in state machine
     */
    if (wps_init(wps_s, init_cfg_file) != WPS_STATUS_SUCCESS) {
        exitcode = WPS_STATUS_FAIL;
        ERR("Fail to initialize WPS !\n");
        goto exit;
    }

    if (wps_s->bss_type == BSS_TYPE_STA)
        pwps_info->frag_thres = EAP_FRAG_THRESHOLD_DEF;
    else
        pwps_info->frag_thres = EAP_FRAG_THRESHOLD_MAX;

    /* This version is broken for anything but the WIFIDIR role */
    if (pwps_info->role != WIFIDIR_ROLE) {
        ERR("This is an experimental build of wpswfd.\n"
            "It is broken for everything except the WIFIDIR role.\n");
        return WPS_STATUS_FAIL;
    }

    /*
     * Initialize netlink socket interface for receiving events.
     */
    if (wps_event_init(wps_s) != 0) {
        exitcode = WPS_STATUS_FAIL;
        ERR("Fail to initialize event socket !\n");
        goto exit;
    }


    /* initialize the socket interface for the mwu interface */
    ret = mwu_internal_init();
    if (ret != MWU_ERR_SUCCESS)
        return ret;

    /* initialize the mwu test module */
    ret = test_init();
    if (ret != MWU_ERR_SUCCESS)
        return ret;

    /* initialize the wifi direct module with the mwu interface */
    ret = wifidir_mwu_launch();
    if (ret != MWU_ERR_SUCCESS)
        return ret;

    ret = wifidir_launch();
    if (ret != WIFIDIR_ERR_SUCCESS)
        return ret;

    /* initialize the wps module with the mwu interface */
    ret = mwpsmod_mwu_launch();
    if (ret != MWU_ERR_SUCCESS)
        return ret;
    ret = mwpsmod_launch();
    if (ret != MWPSMOD_ERR_SUCCESS)
        return ret;

    /*Initilize the mwpamod modue with the mwu interface */
    ret = mwpamod_mwu_launch();
    if (ret != WIFIDIR_ERR_SUCCESS)
        return ret;

    /* Launch mwpamod module Initialization of the module can be done by higher
     * layer after calling mwpamod_sta_init() or mwpamod_ap_init
     */
    ret = mwpamod_launch();
    if (ret != MWPAMOD_ERR_SUCCESS)
        return ret;

    /* Go into the background if necessary */
    if (background) {
        ret = fork();
        if (ret < 0) {
            perror("failed to fork mwu");
            return errno;
        } else if (ret > 0) {
            /* parent process. */
            INFO("Launched mwu\n");
            return 0;
        } else {
            /* child process */
            setsid();

            /* redirect stdin, out, and err */
            freopen("/dev/null", "r", stdin);
            if (logfile) {
                if (freopen(logfile, "w", stdout) == NULL) {
                    return errno;
                }
            } else {
                freopen("/dev/null", "w", stdout);
            }
            umask(027);
            ret = lockf(lock, F_TLOCK, 0);
            if (ret < 0) {
                return errno;
            }
            sprintf(str, "%d\n", getpid());
            ret = write(lock, str, strlen(str));
            if (0 > ret) {
                return errno;
            }

            signal(SIGCHLD, SIG_IGN);
            signal(SIGTSTP, SIG_IGN);
            signal(SIGTTOU, SIG_IGN);
            signal(SIGTTIN, SIG_IGN);
            signal(SIGHUP, wifidir_signal_handler);
            signal(SIGTERM, wifidir_signal_handler);
        }
    }

    /*
     * 1. Scan and associate to AP if role is Enrollee
     * 2. Wait for WPS connection if role is Registrar
     */
    if (wps_start(wps_s) == WPS_STATUS_SUCCESS) {

        /* Enable main loop procedure */
        wps_main_loop_enable();

        /* Main loop for socket read and timeout function */
        wps_main_loop_proc();
    }

    if (wps_s->bss_type == BSS_TYPE_STA) {
        /* Disconnect with AP if needed */
        wps_stop(wps_s);
    } else if (wps_s->bss_type == BSS_TYPE_UAP) {
        /* Clear WPS IE */
        if (ie_index != -1)
            wps_wlan_ie_config(CLEAR_WPS_IE, &ie_index);
    }
    /* free functions */
    if (pwps_info->role == WIFIDIR_ROLE) {
        /* Clear WPS IE */
        if (ie_index != -1)
            wps_wlan_ie_config(CLEAR_WPS_IE, &ie_index);
    }

    /* For unconditionally, the function has checks for NULL, 0 */
    wps_event_deinit(wps_s);
    wps_deinit();

  exit:
    mwpsmod_halt();
    mwpsmod_mwu_halt();

    wifidir_halt();
    wifidir_mwu_halt();

    mwpamod_halt();
    mwpamod_mwu_halt();

    test_deinit();
    mwu_internal_deinit();
    wps_loop_deinit(wps_s);
    wps_intf_deinit(wps_s);
    return exitcode;
}
