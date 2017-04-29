/*
* All Rights Reserved
*
* MARVELL CONFIDENTIAL
* Copyright 2008 ~ 2010 Marvell International Ltd All Rights Reserved.
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
#define LOG_TAG "marvellWirelessDaemon"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <errno.h>
#include <utils/Log.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <cutils/log.h>
#include <sys/types.h>  
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <stddef.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cutils/properties.h>

#include <sys/prctl.h>
#include <linux/capability.h>
#include <private/android_filesystem_config.h>

#include "marvell_wireless_daemon.h"


#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef HCI_DEV_ID
#define HCI_DEV_ID 0
#endif

#define HCID_START_DELAY_SEC   1
#define HCID_STOP_DELAY_USEC 500000
#define HCIATTACH_STOP_DELAY_SEC 1
#define FM_ENABLE_DELAY_SEC  3

//SD8787 power state 
union POWER_SD8787
{
    unsigned int on; // FALSE, means off, others means ON
    struct 
    {
        unsigned int wifi_on:1;  //TRUE means on, FALSE means OFF
        unsigned int uap_on:1;
        unsigned int bt_on:1;
        unsigned int fm_on:1;
        unsigned int wifiDirect_on:1;
    }type;
} power_sd8787;

//Static paths and args
static const char* WIFI_DRIVER_MODULE_1_PATH = "/system/lib/modules/mlan.ko";
static const char* WIFI_DRIVER_MODULE_1_NAME =     "mlan";
static const char* WIFI_DRIVER_MODULE_1_ARG =      "";

static const char* WIFI_DRIVER_MODULE_2_PATH = "/system/lib/modules/sd8787.ko";
static const char* WIFI_DRIVER_MODULE_2_NAME =    "sd8xxx";
static const char* WIFI_DRIVER_MODULE_2_ARG =       "drv_mode=5";
static const char* WIFI_UAP_DRIVER_MODULE_2_ARG =        "drv_mode=3";
static const char* WIFI_WIFIDIRECT_DRIVER_MODULE_2_ARG =        "drv_mode=5";
static const char* WIFI_DRIVER_IFAC_NAME =         "/sys/class/net/mlan0";
static const char* WIFI_UAP_DRIVER_IFAC_NAME =         "/sys/class/net/uap0";
static const char* WIFI_WIFIDIRECT_DRIVER_IFAC_NAME =         "/sys/class/net/wfd0";


static const char* BT_DRIVER_MODULE_PATH =    "/system/lib/modules/bt8787.ko";
static const char* BT_DRIVER_MODULE_NAME =     "bt8xxx";
static const char* BT_DRIVER_MODULE_ARG =     "";
static const char* BT_DRIVER_IFAC_NAME =         "/sys/class/bluetooth/hci0";

static const char* WIRELESS_UNIX_SOCKET_DIR = "/data/misc/wifi/sockets/socket_daemon";
static const char* WIRELESS_POWER_SET_PATH = "/proc/marvell_wireless";
static const char* MODULE_FILE = "/proc/modules";
static const char DRIVER_PROP_NAME[]    = "wlan.driver.status";

static const char* RFKILL_SD8X_PATH = "/sys/class/rfkill/rfkill0/state";

static int flag_exit = 0;

void android_set_aid_and_cap() {
    int ret = -1;
    prctl(PR_SET_KEEPCAPS, 1, 0, 0, 0);

    gid_t groups[] = {AID_BLUETOOTH, AID_WIFI, AID_NET_BT_ADMIN, AID_NET_BT, AID_INET, AID_NET_RAW, AID_NET_ADMIN};
    if ((ret = setgroups(sizeof(groups)/sizeof(groups[0]), groups)) != 0){
        LOGE("setgroups failed, ret:%d, strerror:%s", ret, strerror(errno));
        return;
    }

    if(setgid(AID_SYSTEM) != 0){
        LOGE("setgid failed, ret:%d, strerror:%s", ret, strerror(errno));
        return;
    }

    if ((ret = setuid(AID_SYSTEM)) != 0){
        LOGE("setuid failed, ret:%d, strerror:%s", ret, strerror(errno));
        return;
    }

    struct __user_cap_header_struct header;
    struct __user_cap_data_struct cap;
    header.version = _LINUX_CAPABILITY_VERSION;
    header.pid = 0;

    cap.effective = cap.permitted = 1 << CAP_NET_RAW |
    1 << CAP_NET_ADMIN |
    1 << CAP_NET_BIND_SERVICE |
    1 << CAP_SYS_MODULE |
    1 << CAP_IPC_LOCK;

    cap.inheritable = 0;
    if ((ret = capset(&header, &cap)) != 0){
        LOGE("capset failed, ret:%d, strerror:%s", ret, strerror(errno));
        return;
    }
    return;
}

//Daemon entry 
int main(void)
{
    int listenfd = -1;
    int clifd = -1;

    power_sd8787.on = FALSE;
      //register SIGINT and SIGTERM, set handler as kill_handler
    struct sigaction sa;
    sa.sa_flags = SA_NOCLDSTOP;
    sa.sa_handler = kill_handler;
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT,  &sa, NULL);

    android_set_aid_and_cap();

    listenfd = serv_listen (WIRELESS_UNIX_SOCKET_DIR);
    if (listenfd < 0) 
    {
        LOGE("serv_listen error.\n");
        return -1;
    }
    LOGI("succeed to create socket and listen.\n");
    while (1) 
    { 
        clifd = serv_accept (listenfd);
        if (clifd < 0) 
        {
            LOGE("serv_accept error. \n");
            continue;
        }
        handle_thread(clifd);
        close (clifd);
    }
    close(listenfd);   
    return 0; 
}

void handle_thread(int clifd)
{
    int nread;
    char buffer[MAX_BUFFER_SIZE];
    int len = 0;
    int ret = 0;
     nread = read(clifd, buffer, sizeof (buffer));
        if (nread == SOCKERR_IO) 
        {
            LOGE("read error on fd %d\n", clifd);
        }
        else if (nread == SOCKERR_CLOSED) 
        {
            LOGE("fd %d has been closed.\n", clifd);
        }
        else 
        {
            LOGI("Got that! the data is %s\n", buffer);
            ret = cmd_handler(buffer);
        }
        if(ret == 0)
            strncpy(buffer, "0,OK", sizeof(buffer));
        else
            strncpy(buffer, "1,FAIL", sizeof(buffer));

        nread = write(clifd, buffer, strlen(buffer));

        if (nread == SOCKERR_IO) 
        {
            LOGE("write error on fd %d\n", clifd);
        }
        else if (nread == SOCKERR_CLOSED) 
        {
            LOGE("fd %d has been closed.\n", clifd);
        }
        else
            LOGI("Wrote %s to client. \n", buffer);
}

//Command Handler
int cmd_handler(char* buffer)
{ 
    int ret = 0;
    LOGD("marvell wireless daemon received cmd: %s\n", buffer);
    if(!strncmp(buffer, "WIFI_DISABLE", strlen("WIFI_DISABLE")))
          ret = wifi_disable();
    else if (!strncmp(buffer, "WIFI_ENABLE", strlen("WIFI_ENABLE")))
        ret = wifi_enable();
    if(!strncmp(buffer, "UAP_DISABLE", strlen("UAP_DISABLE")))
          ret = uap_disable();
    else if (!strncmp(buffer, "UAP_ENABLE", strlen("UAP_ENABLE")))
        ret = uap_enable();
    else if (!strncmp(buffer, "BT_DISABLE", strlen("BT_DISABLE")))
        ret = bt_disable();
    else if (!strncmp(buffer, "BT_ENABLE", strlen("BT_ENABLE")))
        ret = bt_enable();
    else if (!strncmp(buffer, "FM_DISABLE", strlen("FM_DISABLE")))
        ret = fm_disable();
    else if (!strncmp(buffer, "FM_ENABLE", strlen("FM_ENABLE")))
        ret = fm_enable();
    else if (!strncmp(buffer, "WIFIDIRECT_ENABLE", strlen("WIFIDIRECT_ENABLE")))
        ret = wifiDirect_enable();
    else if (!strncmp(buffer, "WIFIDIRECT_DISABLE", strlen("WIFIDIRECT_DISABLE")))
        ret = wifiDirect_disable();
    return ret;
} 



int wifi_enable(void)
{
    int ret = 0;
    power_sd8787.type.wifi_on = TRUE;
    ret = wifi_uap_enable(WIFI_DRIVER_MODULE_2_ARG);
    if(ret < 0)goto out;
    ret = wait_interface_ready(WIFI_DRIVER_IFAC_NAME);
    if(ret < 0)
    {
        property_set(DRIVER_PROP_NAME, "timeout");
        goto out;
    }
#ifdef SD8787_NEED_CALIBRATE
    ret = wifi_calibrate();
#endif
out:
    if(ret == 0)
    {
        property_set(DRIVER_PROP_NAME, "ok");
    }
    else
    {
        property_set(DRIVER_PROP_NAME, "failed");
    }
    return ret;
}

int wifiDirect_enable(void)
{
    int ret = 0;
    power_sd8787.type.wifiDirect_on = TRUE;
    ret = wifi_uap_enable(WIFI_WIFIDIRECT_DRIVER_MODULE_2_ARG);
    if(ret < 0)goto out;
        ret = wait_interface_ready(WIFI_WIFIDIRECT_DRIVER_IFAC_NAME);
    if(ret < 0)goto out;
#ifdef SD8787_NEED_CALIBRATE
    ret = wifi_calibrate();
#endif
out:
    return ret;
}



int wifi_disable(void)
{
    int ret = 0;
    power_sd8787.type.wifi_on = FALSE;
    power_sd8787.type.uap_on = FALSE;
    if(power_sd8787.type.wifiDirect_on == FALSE)
    {
        ret = wifi_uap_disable();
    }

    if(ret == 0)
    {
        property_set(DRIVER_PROP_NAME, "unloaded");
    }
    else
    {
        property_set(DRIVER_PROP_NAME, "failed");
    }
    return ret;
}

int wifiDirect_disable(void)
{
    int ret = 0;
    power_sd8787.type.wifiDirect_on = FALSE;

    if(power_sd8787.type.wifi_on == FALSE)
    {
        ret = wifi_uap_disable();
    }

    return ret;
}

int uap_enable(void)
{
    int ret = 0;
    power_sd8787.type.uap_on = TRUE;
    ret = wifi_uap_enable(WIFI_UAP_DRIVER_MODULE_2_ARG);
    if(ret < 0)goto out;
    ret = wait_interface_ready (WIFI_UAP_DRIVER_IFAC_NAME);
    if(ret < 0)goto out;
#ifdef SD8787_NEED_CALIBRATE
    ret = wifi_calibrate();        
#endif
out:
    return ret;
}

#ifdef SD8787_NEED_CALIBRATE
int wifi_calibrate(void)
{
    int ret = 0;
    int ret2 = system("mlanconfig mlan0 regrdwr 5 0x26 0x30");
    if(ret2 != 0)
    {
        LOGD("calibrate:mlanconfig mlan0 regrdwr 5 0x26 0x30 , ret: 0x%x, strerror: %s", ret2, strerror(errno));
    }
    if(ret2 < 0)
    {
        ret = ret2;
    }
    return ret;
}
#endif

int uap_disable(void)
{
    int ret = 0;
    power_sd8787.type.uap_on = FALSE;
    ret = wifi_uap_disable();
    return ret;
}

int bt_enable(void) 
{
    int ret = 0;
    power_sd8787.type.bt_on = TRUE;
    LOGI("Starting hcid deamon");
    if (property_set("ctl.start", "bluetoothd") < 0) {
        ret = -1;
        LOGE("Failed to start hcid");
        goto out;
    }
    ret = bt_fm_enable();
    sleep(HCID_START_DELAY_SEC);
#ifdef SD8787_NEED_CALIBRATE
    ret = bt_calibrate();
#endif
    if (enable_bt_8787() != 0) {
        ret = -1;
        LOGE("%s: enable_bt_8787 failed", __FUNCTION__);
        goto out;
    }
out:
    return ret;
}


int bt_disable() 
{
    int ret = 0;
    LOGI("Stopping hcid deamon");
    ret = property_set("ctl.stop", "bluetoothd");
    if (ret < 0) {
        LOGE("Error stopping hcid");
        goto out;
    }
    usleep(HCID_STOP_DELAY_USEC);
    power_sd8787.type.bt_on = FALSE;
    if(power_sd8787.type.fm_on == FALSE)
    {
        ret = bt_fm_disable();
    }
out:
    return ret;
}

int fm_enable(void) 
{
    int ret = 0;
    power_sd8787.type.fm_on = TRUE;

    ret = bt_fm_enable();
    if(ret < 0)goto out;
    sleep(FM_ENABLE_DELAY_SEC);
#ifdef SD8787_NEED_CALIBRATE
    ret = bt_calibrate();
#endif
out:
    return ret;
}

int fm_disable() 
{
    int ret = 0;
    power_sd8787.type.fm_on = FALSE;
    if(power_sd8787.type.bt_on == FALSE)
    {
        ret = bt_fm_disable();
    }
    return ret;
}

int wifi_uap_enable(const char* driver_module_arg)
{
    int ret = 0;
    LOGD("wifi_uap_enable");
    ret = set_power(TRUE);
    if (ret < 0) 
    {
        LOGD("wifi_uap_enable, set_power fail");
        goto out;
    }
    if(check_driver_loaded(WIFI_DRIVER_MODULE_1_NAME) == FALSE)
    {
        ret = insmod(WIFI_DRIVER_MODULE_1_PATH, WIFI_DRIVER_MODULE_1_ARG);
        if (ret < 0)
        {
            LOGD("wifi_uap_enable, insmod: %s %s fail", WIFI_DRIVER_MODULE_1_PATH, WIFI_DRIVER_MODULE_1_ARG);
            goto out;
        }
    }
    
    if(check_driver_loaded(WIFI_DRIVER_MODULE_2_NAME) == FALSE)
    {
        ret = insmod(WIFI_DRIVER_MODULE_2_PATH, driver_module_arg);
        if (ret < 0)
        {
            LOGD("wifi_uap_enable, insmod: %s %s fail", WIFI_DRIVER_MODULE_2_PATH, driver_module_arg);
            goto out;
        }
    }
out:
    return ret;
}


int wifi_uap_disable()
{
    int ret = 0;
    ret = set_power(FALSE);
    if(check_driver_loaded(WIFI_DRIVER_MODULE_2_NAME) == TRUE)
    {
        ret = rmmod (WIFI_DRIVER_MODULE_2_NAME);
        if (ret < 0) goto out;
    }
    if(check_driver_loaded(WIFI_DRIVER_MODULE_1_NAME) == TRUE)
    {
        ret = rmmod (WIFI_DRIVER_MODULE_1_NAME);
        if (ret < 0) goto out;
    }
out:
    return ret;
}


int bt_fm_enable(void)
{
    int ret = 0;
    ret = set_power(TRUE);
    if(check_driver_loaded(BT_DRIVER_MODULE_NAME) == FALSE)
    {
        ret = insmod(BT_DRIVER_MODULE_PATH, BT_DRIVER_MODULE_ARG);
        if (ret < 0) goto out;
    }
    if(hci_is_up() <= 0)
    {
        ret = up_hci_device();
    }
    if (ret < 0) goto out;
    ret = wait_interface_ready (BT_DRIVER_IFAC_NAME);
out:
    return ret;
}

int bt_fm_disable(void)
{
    int ret = 0;
    ret = down_all_hci_devices();
    if (ret < 0) goto out;
    if(check_driver_loaded(BT_DRIVER_MODULE_NAME) == TRUE)
    {
        ret = rmmod(BT_DRIVER_MODULE_NAME);
        if (ret < 0) goto out;
    }
    ret = set_power(FALSE);
out:
    return ret;
}

int hci_is_up() {
    LOGV(__FUNCTION__);

    int hci_sock = -1;
    int ret = -1;
    struct hci_dev_info dev_info;
    hci_sock = create_hci_sock();
    if (hci_sock < 0) goto out;

    dev_info.dev_id = HCI_DEV_ID;
    if (ioctl(hci_sock, HCIGETDEVINFO, (void *)&dev_info) < 0) {
        ret = 0;
        goto out;
    }

    ret = hci_test_bit(HCI_UP, &dev_info.flags);

out:
    if (hci_sock >= 0) close(hci_sock);
    return ret;
}

int do_set_power(int on)
{
	int fd;
	int retry = 5;
	char ret = -1;
	char sw = on ? '1' : '0';

	LOGI("rfkill utils:%s: on=%d", __FUNCTION__, on);
	while (retry--) {
		fd = open(RFKILL_SD8X_PATH, O_RDWR);
		if (fd <= 0) {
			LOGE("open %s fail", RFKILL_SD8X_PATH);
			return -1;
		}
		if (write(fd, &sw, sizeof(char)) < 0) {
			LOGE("write error %s\n", strerror(errno));
		}
		close(fd);
		usleep(200000);

		fd = open(RFKILL_SD8X_PATH, O_RDWR);
		if (fd <= 0) {
			LOGE("open %s fail", RFKILL_SD8X_PATH);
			return -1;
		}
		read(fd, &ret, sizeof(char));
		close(fd);

		if (ret == sw) {
			return 0;
		}
	}
	return -1;
}

int set_power(int on)
{
	LOGI("rfkill utils:%s: on=%d", __FUNCTION__, on);

	if((!on) && (!(power_sd8787.on))) {
		// off
		return do_set_power(on);
	} else if (on) {
		// on
		return do_set_power(on);
	}
	return 0;
}

int insmod(const char *filename, const char *args)
{
    void *module = NULL;
    unsigned int size = 0;
    int ret = 0;
    module = load_file(filename, &size);
    if (!module)
    {
        LOGD("loadfile:%s, error: %s", filename, strerror(errno));
        ret = -1;
        goto out;
    }
    ret = init_module(module, size, args);
    free(module);
out:
    LOGD("insmod:%s,args %s, size %d, ret: %d", filename, args, size, ret);
    return ret;
}

int rmmod(const char *modname)
{
    int ret = -1;
    int maxtry = 10;

    while (maxtry-- > 0) {
        ret = delete_module(modname, O_NONBLOCK | O_EXCL);
        if (ret < 0 && errno == EAGAIN)
            usleep(500000);
        else
            break;
    }

    if (ret != 0)
        LOGD("Unable to unload driver module \"%s\": %s\n",
             modname, strerror(errno));
    return ret;
}

int check_driver_loaded(const char *modname) 
{
    FILE *proc = NULL;
    char line[64];
    int ret = FALSE;

    if ((proc = fopen(MODULE_FILE, "r")) == NULL) 
    {
        LOGW("Could not open %s: %s", MODULE_FILE, strerror(errno));
        ret = FALSE;
        goto out;
    }
    while ((fgets(line, sizeof(line), proc)) != NULL) 
    {
        if (strncmp(line, modname, strlen(modname)) == 0) 
        {
            fclose(proc);
            ret = TRUE;    
            goto out;
        }
    }
    fclose(proc);
out:
    LOGD("check_driver_loaded %s: ret:%d", modname, ret);
    return ret;
}

int up_hci_device()
{
    int attempt = 0;
    int hci_sock = -1;
    int ret = -1;

    for (attempt = 1000; attempt > 0;  attempt--) 
    {
        hci_sock = create_hci_sock();
        if (hci_sock > 0)
        {
            if (0 == ioctl(hci_sock, HCIDEVUP, 0))
            {
                ret = 0;
                goto out1;
            }

            if (errno == EALREADY)
            {
                if (hci_is_up() > 0)
                {
                    ret = 0;
                    goto out1;
                }
            }

            close(hci_sock);
        }
        usleep(10000);  // 10 ms retry delay
    }
    if (attempt == 0) 
    {
        LOGD("Timeout waiting for HCI device to come up after retry times");
        ret = -1;
        goto out;
    }
out1:
    close(hci_sock);
out:
    return ret;
}

int down_all_hci_devices(void)
{
    struct hci_dev_list_req *dl;
    struct hci_dev_req *dr;
    int i, sk, err = 0;
    int ret = -1;

    sk = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI);
    if (sk < 0)
    {
        LOGE("socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI) failed, errno:0x%x, err:%s", errno, strerror(errno));
        ret = -1;
        goto err;
    }
    dl = malloc(HCI_MAX_DEV * sizeof(*dr) + sizeof(*dl));
    if (!dl)
    {
        LOGE("down_all_hci_devices malloc  failed, errno:0x%x, err:%s", errno, strerror(errno));
        err = errno;
        goto done;
    }

    memset(dl, 0, HCI_MAX_DEV * sizeof(*dr) + sizeof(*dl));

    dl->dev_num = HCI_MAX_DEV;
    dr = dl->dev_req;
    ret = ioctl(sk, HCIGETDEVLIST, (void *) dl);
    if (ret < 0)
    {
        LOGE("ioctl(sk, HCIGETDEVLIST, (void *) dl) failed, errno:0x%x, err:%s", errno, strerror(errno));
        err = errno;
        goto free;
    }

    for (i = 0; i < dl->dev_num; i++, dr++)
    {
        if (hci_test_bit(HCI_UP, &dr->dev_opt))
        {
            LOGD("hci%d status:0x%x", dr->dev_id, dr->dev_opt);
            ret = ioctl(sk, HCIDEVDOWN, dr->dev_id);
            if (ret < 0)
            {
                LOGE("ioctl(hci_sock, HCIDEVDOWN, dr->dev_id) failed, errno:0x%x, err:%s", errno, strerror(errno));
                err = errno;
                goto free;
            }
        }
    }

free:
    free(dl);

done:
    close(sk);
    errno = err;
err:
    return ret;
}

int down_hci_device()
{
    int attempt;
    int hci_sock = -1;

    hci_sock = create_hci_sock();
    if (hci_sock < 0) 
    {
        LOGD("create hci_sock failed.");
        return -1;
    }

    ioctl(hci_sock, HCIDEVDOWN, 0);
    ioctl(hci_sock, HCIDEVDOWN, 1);
    close(hci_sock);
    return 0;

}

int create_hci_sock() 
{
    int sk = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI);
    if (sk < 0) {
        LOGD("Failed to create bluetooth hci socket: %s (%d)",
             strerror(errno), errno);
    }
    return sk;
}


//to do: don’t use polling mode, use uevent, listen interface added uevent from driver
int wait_interface_ready (const char* interface_path)
{
#define TIMEOUT_VALUE 2000    //at most 2 seconds
    int fd, count = TIMEOUT_VALUE;

    while(count--) {
        fd = open(interface_path, O_RDONLY);
        if (fd >= 0)
        {
            close(fd);
            return 0;
        }
        usleep(1000);
    }
    LOGE("timeout(%dms) to wait %s", TIMEOUT_VALUE, interface_path);
    return -1;
}

#ifdef SD8787_NEED_CALIBRATE
int bt_calibrate()
{
    int ret = 0;
    int count = 2000;
    int dd = hci_open_dev(hci_get_route(NULL));
    char buf[6] = {0x02, 0x26, 00, 00, 00, 0x30};
    if (dd < 0) {
        LOGE("hci Device open failed");
        goto out;
    }

    while(--count) {
        ret = hci_send_cmd(dd, 0x3f, 0x66, 6, (void*)buf);
        if (ret == 0)
           break;

        usleep(1000);
    }

    if (count == 0){
        LOGE("Send hci_cmd failed, ret:%d, %s", ret, strerror(errno)); 
        LOGE("timeout(2000ms) to wait fm calibrate");
    }

out1:
    hci_close_dev(dd);
out:
    return ret;
}
#endif

int enable_bt_8787()
{
    LOGI(" BT 8787 - pcm in master mode");
    int dd = hci_open_dev(hci_get_route(NULL));
    if (dd < 0) {
        LOGE("hci Device open failed");
        return -1;
    }

    char buf1 = 0x01;
    if (hci_send_cmd(dd, 0x3f, 0x1d, 1, (void*)&buf1) < 0) {
        LOGE("Send hci_cmd failed");
        hci_close_dev(dd);
        return -1;
    }
    char buf2[3] = {0x03, 0x00, 0x03};
    if (hci_send_cmd(dd, 0x3f, 0x28, 3, (void*)buf2) < 0) {
        LOGE("Send hci_cmd failed");
        hci_close_dev(dd);
        return -1;
    }
    char buf3 = 0x02;
    if (hci_send_cmd(dd, 0x3f, 0x07, 1, (void*)&buf3) < 0) {
        LOGE("Send hci_cmd failed");
        hci_close_dev(dd);
        return -1;
    }
    hci_close_dev(dd);

    return 0;
}

static void kill_handler(int sig)
{
    LOGI("Received signal %d.", sig);
    power_sd8787.on = FALSE;
    if (down_all_hci_devices() < 0)
    {
        LOGE("down_all_hci_devices failed.");
    }

    if (set_power(FALSE) < 0) 
    {
        LOGE("set_fm_power failed.");
    }
    flag_exit = 1;
}

int serv_listen (const char* name)
{
    int fd,len;
    struct sockaddr_un unix_addr;

    /* Create a Unix domain stream socket */
    if ( (fd = socket (AF_UNIX, SOCK_STREAM, 0)) < 0)
        return (-1);
    unlink (name);
    /* Fill in socket address structure */
    memset (&unix_addr, 0, sizeof (unix_addr));
    unix_addr.sun_family = AF_UNIX;
    strcpy (unix_addr.sun_path, name);
    snprintf(unix_addr.sun_path, sizeof(unix_addr.sun_path), "%s", name);
    len = sizeof (unix_addr.sun_family) + strlen (unix_addr.sun_path);

    /* Bind the name to the descriptor */
    if (bind (fd, (struct sockaddr*)&unix_addr, len) < 0)
    {
        LOGE("bind fd:%d and address:%s error: %s", fd, unix_addr.sun_path, strerror(errno));
        goto error;
    }
    if (chmod (name, 0666) < 0)
    {
        LOGE("change %s mode error: %s", name, strerror(errno));
        goto error;
    }
    if (listen (fd, 5) < 0)
    {
        LOGE("listen fd %d error: %s", fd, strerror(errno));
        goto error;
    }
    return (fd);
   
error:
    close (fd);
    return (-1); 
}

#define    STALE    30    /* client's name can't be older than this (sec) */


/* returns new fd if all OK, < 0 on error */
int serv_accept (int listenfd)
{
    int                clifd, len;
    time_t             staletime;
    struct sockaddr_un unix_addr;
    struct stat        statbuf;
    const char*        pid_str;

    len = sizeof (unix_addr);
    if ( (clifd = accept (listenfd, (struct sockaddr *) &unix_addr, &len)) < 0)
    {
        LOGE("listenfd %d, accept error: %s", listenfd, strerror(errno));
        return (-1);        /* often errno=EINTR, if signal caught */
    }
    return (clifd);
}


