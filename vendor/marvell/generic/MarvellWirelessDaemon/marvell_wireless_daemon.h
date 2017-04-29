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

#ifndef _MARVELL_WIRELESS_DAEMON
#define _MARVELL_WIRELESS_DAEMON

void handle_thread(int clifd);
int cmd_handler(char* buffer);
int wifi_enable(void);
int uap_enable(void);
int wifiDirect_enable(void);
int bt_enable(void);
int fm_enable(void);
int wifi_uap_enable(const char* driver_module_arg);
int bt_fm_enable(void);

int wifi_disable(void);
int uap_disable(void);
int wifiDirect_disable(void);
int bt_disable(void);
int fm_disable(void);
int wifi_uap_disable(void);
int bt_fm_disable(void);

int set_power(int on);
int get_power(void);

int insmod(const char *filename, const char *args);
int rmmod(const char *modname);
int check_driver_loaded(const char *modname);

int enable_bt_8787();
int hci_is_up();
int up_hci_device();
int down_hci_device();
int down_all_hci_devices(void);
int create_hci_sock();
int wait_interface_ready (const char* interface_path);
static void kill_handler(int sig);
int bt_calibrate(void);
int wifi_calibrate(void);    

int init_module (void *, unsigned long, const char *);
int delete_module (const char*, unsigned int);
void* load_file(const char* filename, unsigned* size);

int serv_listen (const char* name);
int serv_accept (int listenfd);

#define SOCKERR_IO          -1
#define SOCKERR_CLOSED      -2
#define SOCKERR_INVARG      -3
#define SOCKERR_TIMEOUT     -4
#define SOCKERR_OK          0
#define MAX_BUFFER_SIZE    256


#endif
