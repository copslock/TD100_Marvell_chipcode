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
#define LOG_TAG "libMarvellWireless"
#include <stdio.h>
#include <utils/Log.h>
#include <stdlib.h>
#include <sys/types.h>  
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <stddef.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include "marvell_wireless.h"

#define SOCKERR_IO          -1
#define SOCKERR_CLOSED      -2
#define SOCKERR_INVARG      -3
#define SOCKERR_TIMEOUT     -4
#define SOCKERR_OK          0
#define MAX_BUFFER_SIZE    256

static int wireless_send_command(const char *cmd);
static int cli_conn (const char *name);

static const char *local_socket_dir = "/data/misc/wifi/sockets/socket_daemon";
/* interfaces implements for wifi,uAP,bluetooth, FMradio*/
int wifi_enable(void)
{
    return wireless_send_command("WIFI_ENABLE");
}

int wifiDirect_enable(void)
{
    return wireless_send_command("WIFIDIRECT_ENABLE");
}

int wifi_disable(void)
{
    return wireless_send_command("WIFI_DISABLE");
}

int wifiDirect_disable(void)
{
    return wireless_send_command("WIFIDIRECT_DISABLE");
}

int uap_enable(void)
{
    return wireless_send_command("UAP_ENABLE");
}
int uap_disable(void)
{
    return wireless_send_command("UAP_DISABLE");
}

int bluetooth_enable(void)
{
    return wireless_send_command("BT_ENABLE");
}
int bluetooth_disable(void)
{
    return wireless_send_command("BT_DISABLE");
}
int fm_enable(void)
{
    return wireless_send_command("FM_ENABLE");
}
int fm_disable(void)
{
    return wireless_send_command("FM_DISABLE");
}
/*send wireless command:wifi,uAP,Bluetooth,FM enable/disable commands to marvell wireless daemon*/
static int wireless_send_command(const char *cmd)
{
    int conn_fd = -1;
    int n = 0;
    int i = 0;
    int data = 0;
    char buffer[256];
    int len = 0;
    int ret = 1;

    conn_fd = cli_conn (local_socket_dir);
    if (conn_fd < 0) {
        LOGE("cli_conn error.\n");
        ret = 1;
        goto out1;
    }
    len = strlen(cmd);
    strncpy(buffer, cmd, len);
    buffer[len++] = '\0';
    n = write(conn_fd, buffer, len);

    if (n == SOCKERR_IO) 
    {
        LOGE("write error on fd %d\n", conn_fd);
        ret = 1;
        goto out;
    }
    else if (n == SOCKERR_CLOSED) 
    {
        LOGE("fd %d has been closed.\n", conn_fd);
        ret = 1;
        goto out;
    }
    else 
        LOGI("Wrote %s to server. \n", buffer);

    n = read(conn_fd, buffer, sizeof (buffer));
    if (n == SOCKERR_IO) 
    {
        LOGE("read error on fd %d\n", conn_fd);
        ret = 1;
        goto out;
    }
    else if (n == SOCKERR_CLOSED) 
    {
        LOGE("fd %d has been closed.\n", conn_fd);
        ret = 1;
        goto out;
    }
    else 
    {
        if(!strncmp(buffer,"0,OK", strlen("0,OK")))
            ret = 0;
        else
            ret = 1;
    }
out:
    close(conn_fd);
out1:
    return ret; 
}

/* returns fd if all OK, -1 on error */
static int cli_conn (const char *name)
{
    int                fd, len;
    int ret = 0;
    struct sockaddr_un unix_addr;
    int value = 0x1;
    /* create a Unix domain stream socket */
    if ( (fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
        LOGE("create socket failed, ret:%d, strerror:%s", fd, strerror(errno));
        return(-1);
    }

    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void*)&value, sizeof(value));
    /* fill socket address structure w/server's addr */
    memset(&unix_addr, 0, sizeof(unix_addr));
    unix_addr.sun_family = AF_UNIX;
    strcpy(unix_addr.sun_path, name);
    len = sizeof(unix_addr.sun_family) + strlen(unix_addr.sun_path);

    if ((ret = connect (fd, (struct sockaddr *) &unix_addr, len)) < 0)
    {
        LOGE("connect failed, ret:%d, strerror:%s", ret, strerror(errno));
        goto error;
    }
    return (fd);

error:
    close (fd);
    return ret;
}

