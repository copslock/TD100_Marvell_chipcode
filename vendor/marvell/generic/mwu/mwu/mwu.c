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
/* mwu.c: marvell wireless utility message channel
 *
 * This is the implementation of the mwu APIs defined in mwu.h and
 * mwu_internal.h.  It implements the actual socket communication between the
 * control app (client) and the daemon (server).  It uses a unix domain socket
 * for IPC.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "wps_msg.h"
#include "wps_os.h"
#include "mwu.h"

#define UTIL_LOG_TAG "MWU"
#include "util.h"
#include "mwu_internal.h"

/* TODO: We need various utility functions (e.g., mwu_printf()) from these
 * include files.  Such functionality should be migrated out of the
 * wps-specific headers and into generic ones.
 */
#include "mwu_log.h"
#include "wps_wlan.h"

#ifdef ANDROID
#define MWU_MSG_FIFO "/tmp/mwu.fifo"
#define MWU_EVENT_FIFO "/tmp/mwu-event.fifo"
#define SUN_LEN(su)  (sizeof(*(su)) - sizeof((su)->sun_path) + strlen((su)->sun_path))
#else
#define MWU_MSG_FIFO "/var/run/mwu.fifo"
#define MWU_EVENT_FIFO "/var/run/mwu-event.fifo"
#endif

#ifdef MWU_SERVER

struct mwu_module *registered_modules[MWU_MAX_MODULES];

static struct mwu_module *lookup_module(char *name)
{
    struct mwu_module *m = NULL;
    int i;

    for (i = 0; i < MWU_MAX_MODULES; i++) {
        m = registered_modules[i];
        if (m != NULL && m->name[0] != 0 && strcmp(m->name, name) == 0)
            return m;
    }
    return NULL;
}

enum mwu_error mwu_internal_register_module(struct mwu_module *mod)
{
    struct mwu_module *m;
    int i;

    if (mod == NULL || mod->name == NULL || mod->name[0] == 0 ||
        mod->msg_cb == NULL) {
        ERR("Can't register module without valid name\n");
        return MWU_ERR_INVAL;
    }

    INFO("Registering module %s\n", mod->name);

    /* don't allow modules with identical names */
    m = lookup_module(mod->name);
    if (m != NULL) {
        ERR("Module named %s already registered\n", m->name);
        return MWU_ERR_INVAL;
    }

    for (i = 0; i < MWU_MAX_MODULES; i++) {
        if (registered_modules[i] == NULL) {
            registered_modules[i] = mod;
            return MWU_ERR_SUCCESS;
        }
    }

    INFO("Registration failed.  Too many modules.\n");
    return MWU_ERR_BUSY;
}

void mwu_internal_unregister_module(struct mwu_module *mod)
{
    int i;

    INFO("Unregistering module %s\n", mod->name);

    if (mod == NULL || mod->name == NULL || mod->name[0] == 0) {
        /* surely, we did not register this module */
        return;
    }

    for (i = 0; i < MWU_MAX_MODULES; i++) {
        if (registered_modules[i] == mod)
            registered_modules[i] = NULL;
    }
}

/* sockets for messages and events */
static int mclient = -1;
static int mlistener = -1;
static int eclient = -1;
static int elistener = -1;

/* The client that we are handling here not only expects us to receive a
 * message, but also expects us to send a response.  If we don't send that
 * response, the client will block forever.  If we can't allocate memory or
 * something else fatal, there's not much we can do.  But for error cases that
 * we can handle, we should send back a status.
 */
static void handle_mclient(int sock, void *sock_ctx)
{
    int ret = 0;
    int status = MWU_ERR_SUCCESS;
    struct mwu_msg *msg = NULL, *resp = NULL;
    char *modname = NULL;
    struct mwu_module *mod = NULL;

    if (sock != mclient) {
        ERR("Internal error.  Socket does not match expected socket.\n");
        return;
    }

    if (mclient == -1) {
        ERR("Got msg on closed socket!\n");
        return;
    }

    msg = malloc(sizeof(struct mwu_msg));
    if (!msg) {
        ERR("Failed to alloc message header.\n");
        goto done;
    }

    ret = recv(sock, msg, sizeof(struct mwu_msg), MSG_PEEK);

    if (ret == -1) {
        /* TODO: when the control program calls mwu_disconnect, we get
         * ECONNRESET here.  But after that we get a zero-length response
         * (i.e., ret=0).  Not sure why.  It would be nice to just get one
         * shut-down event.
         */
        ERR("Failed to recv msg from client: %s.\n", strerror(errno));
        goto done;
    }

    if (ret == 0) {
        /* an empty message means that the client wants to disconnect. */
        INFO("Got empty message.  Disconnecting client.\n");
        wps_unregister_rdsock_handler(mclient);
        close(mclient);
        mclient = -1;
        goto done;
    }

    if (ret < (int)sizeof(struct mwu_msg)) {
        ERR("msg did not contain complete header (ret = %d).\n", ret);
        goto done;
    }

    /* make room for the rest of the msg */
    msg = realloc(msg, sizeof(struct mwu_msg) + msg->len);
    if (!msg) {
        ERR("Failed to alloc space for msg body.\n");
        goto done;
    }
    ret = recv(sock, msg, sizeof(struct mwu_msg) + msg->len, 0);
    if (ret == -1) {
        ERR("Failed to recv msg body from client: %s\n", strerror(errno));
        goto done;
    }

    /* Okay.  We have a message from a control client.  Check for the
     * module=<module>, lookup the registered module, and pass the message.
     */

    /* how long is the module name? */
    modname = strchr(msg->data, '\n');
    if (modname == NULL) {
        ERR("No valid key-value pairs in message.\n");
        status = MWU_ERR_INVAL;
        goto send_response;
    }
    modname = malloc(modname - msg->data + 1);
    if (modname == NULL) {
        ERR("Failed to allocate space for module name.\n");
        status = MWU_ERR_NOMEM;
        goto send_response;
    }
    ret = sscanf(msg->data, "module=%s\n", modname);
    if (ret != 1) {
        ERR("Failed to find module=<module> key-value pair in message\n");
        status = MWU_ERR_INVAL;
        goto send_response;
    }

    mod = lookup_module(modname);
    if (mod == NULL) {
        ERR("No registered module named %s\n", modname);
        status = MWU_ERR_NO_MODULE;
        goto send_response;
    }
    ret = mod->msg_cb(msg, &resp);
    if (ret != MWU_ERR_SUCCESS) {
        /* Don't bother cluttering the log with a message here.  The module
         * should do that for us.
         */
        resp = NULL;
        status = ret;
        goto send_response;
    }

    /* send the response */
send_response:

    if (resp == NULL) {
        msg->len = 0;
        msg->status = status;
        ret = send(sock, msg, sizeof(struct mwu_msg), 0);
    } else {
        /* If the module actually provided a response, we have always
         * succeeded.
         */
        resp->status = status;
        ret = send(sock, resp, sizeof(struct mwu_msg) + resp->len, 0);
    }

    if (ret == -1) {
        ERR("Failed to send response to client: %s\n", strerror(errno));
        goto done;
    }

done:
    if (msg)
        free(msg);

    if (modname)
        free(modname);

    if (resp) {
        if (mod && mod->msg_free)
            mod->msg_free(resp);
        else
            free(resp);
    }
}

/* We should never _get_ events from the client.  We only send them.  So this
 * handler is really just a dummy handler that cleanly shuts down the socket
 * when it reads 0 bytes.
 */
static void handle_eclient(int sock, void *sock_ctx)
{
    int ret;
    char buf[4];

    if (sock != eclient) {
        ERR("Internal error.  Socket does not match expected event socket.\n");
        return;
    }

    ret = read(eclient, buf, sizeof(buf));
    if (ret < 0) {
        ERR("Error reading from event socket: %s\n", strerror(errno));
    } else if (ret > 0) {
        ERR("Unexpected data from event client.\n");
    } else {
        INFO("Closing event client.\n");
        wps_unregister_rdsock_handler(eclient);
        close(eclient);
        eclient = -1;
    }
    return;
}

/* accept a connection from listening sock and return client sock.  Register
 * the handler to handle the client sock.
 */
static int accept_connection(int sock,
                             void (*handler) (int sock, void *sock_ctx),
                             void *data)
{
    struct sockaddr_un name;
    int len;
    int ret, client;

    len = sizeof(name);
    client = accept(sock, (struct sockaddr *)&name, &len);
    if (client == -1) {
        ERR("Failed to accept connection: %s\n", strerror(errno));
        return -1;
    }
    ret = wps_register_rdsock_handler(client, handler, data);
    if (ret != WPS_STATUS_SUCCESS) {
        ERR("Failed to register new connection\n");
        return -1;
    }
    return client;
}

static void mwu_mlistener(int sock, void *sock_ctx)
{
    if (mclient != -1) {
        ERR("Failed to accept connection: busy. %d\n", mclient);
        return;
    }

    mclient = accept_connection(sock, handle_mclient, NULL);
    if (mclient == -1) {
        ERR("Failed to accept control connection.\n");
        return;
    }
}

static void mwu_elistener(int sock, void *sock_ctx)
{

    if (eclient != -1) {
        ERR("Failed to accept event connection: busy.\n");
        return;
    }

    eclient = accept_connection(sock, handle_eclient, NULL);
    if (eclient == -1) {
        ERR("Failed to accept event connection.\n");
        return;
    }
}

/* create a listening socket at path */
static int new_un_listener(char *path)
{
    int s, ret;
    struct sockaddr_un addr;

    /* Create a socket that listens for for connections */
    s = socket(PF_LOCAL, SOCK_SEQPACKET, 0);
    if (s == -1) {
        ERR("Failed to create listening socket: %s\n", strerror(errno));
        return -1;
    }

    /* unlink any stale hangers on.  The caller is responsible for ensuring
     * that there are no other instances of the UI server running.
     */
    unlink(path);
    addr.sun_family = AF_LOCAL;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path));
    ret = bind(s, (struct sockaddr *)&addr, SUN_LEN(&addr));
    if (ret == -1) {
        ERR("Failed to bind listening socket: %s\n", strerror(errno));
        return -1;
    }

    /* Listen for connections. */
    ret = listen(s, 5);
    if (ret == -1) {
        ERR("Failed to listen on socket: %s\n", strerror(errno));
        return -1;
    }
    return s;
}


/* we store the last few events so we can replay them through the
 * "get_next_event" facility.  All it does is send the next cached event from
 * our fifo.  This is really only used for test purposes.  A real system should
 * not depend on this capability for event fetching.
 */

struct cached_event {
    struct mwu_msg *msg;
    STAILQ_ENTRY(cached_event) list_item;
};
STAILQ_HEAD(event_list, cached_event);
static struct event_list events;
#define MAX_CACHED_EVENTS 8
static int num_events;

static void mwu_internal_free_cached_events(void) {
    struct cached_event *e;

    while (!STAILQ_EMPTY(&events)) {
        e = STAILQ_FIRST(&events);
        STAILQ_REMOVE_HEAD(&events, list_item);
        free(e->msg);
        free(e);
    }
    num_events = 0;
}

enum mwu_error handle_mwu_cmd(struct mwu_msg *msg, struct mwu_msg **resp)
{
    int ret;
    struct cached_event *e;
    *resp = NULL;

    if (strcmp(msg->data, "module=mwu\ncmd=get_next_event\n") == 0) {
        /* If there's an event in the queue, pop it out and return it */
        if (num_events == 0)
            return MWU_ERR_SUCCESS;
        e = STAILQ_FIRST(&events);
        if (!e) {
            WARN("event queue unexpectedly empty!");
            return MWU_ERR_SUCCESS;
        }
        STAILQ_REMOVE_HEAD(&events, list_item);
        num_events--;
        *resp = e->msg;
        /* We no longer need the container.  msg will be freed by our caller. */
        free(e);

    } else if (strcmp(msg->data, "module=mwu\ncmd=clear_events\n") == 0) {
        mwu_internal_free_cached_events();
    } else {
        ERR("Ignoring unknown command: %s\n", msg->data);
    }
    return MWU_ERR_SUCCESS;
}

struct mwu_module mwu_mod = {
    .name = "mwu",
    .msg_cb = handle_mwu_cmd,
    .msg_free = NULL,
};

enum mwu_error mwu_internal_init(void)
{
    int ret;

    if (mlistener != -1 || elistener != -1) {
        ERR("Can't launch mwu server.  Busy.\n");
        return MWU_ERR_BUSY;
    }

    mlistener = new_un_listener(MWU_MSG_FIFO);
    if (mlistener == -1) {
        ERR("Failed to create mwu message listener.\n");
        ret = MWU_ERR_COM;
        goto fail;
    }

    ret = wps_register_rdsock_handler(mlistener, mwu_mlistener, NULL);
    if (ret != 0) {
        ret = MWU_ERR_COM;
        goto fail;
    }

    elistener = new_un_listener(MWU_EVENT_FIFO);
    if (elistener == -1) {
        ERR("Failed to create mwu event listener.\n");
        ret = MWU_ERR_COM;
        goto fail;
    }

    ret = wps_register_rdsock_handler(elistener, mwu_elistener, NULL);
    if (ret != 0) {
        ret = MWU_ERR_COM;
        goto fail;
    }

    memset(registered_modules, 0, sizeof(registered_modules));

    /* add ourselves as a module */
    if (!STAILQ_EMPTY(&events))
        WARN("events queue is not empty!  This is a memory leak!\n");

    STAILQ_INIT(&events);
    mwu_internal_register_module(&mwu_mod);

    INFO("Launched mwu server.\n");
    return 0;

fail:
    mwu_internal_deinit();
    return ret;
}

void mwu_internal_deinit(void)
{

    if (mclient != -1) {
        wps_unregister_rdsock_handler(mclient);
        close(mclient);
        mclient = -1;
    }

    if (mlistener != -1) {
        wps_unregister_rdsock_handler(mlistener);
        close(mlistener);
        mlistener = -1;
    }

    if (eclient != -1) {
        wps_unregister_rdsock_handler(eclient);
        close(eclient);
        eclient = -1;
    }

    if (elistener != -1) {
        wps_unregister_rdsock_handler(elistener);
        close(elistener);
        elistener = -1;
    }

    remove(MWU_MSG_FIFO);
    remove(MWU_EVENT_FIFO);
    mwu_internal_free_cached_events();
}

enum mwu_error mwu_internal_send(struct mwu_msg *msg)
{
    int ret = 0;
    struct cached_event *e;

    if (msg == NULL) {
        ERR("Can't send invalid message.\n");
        return MWU_ERR_INVAL;
    }

    /* record the event in our event cache */
    if (num_events == MAX_CACHED_EVENTS) {
        WARN("Non fatal: Event cache is full.  Refusing to cache event\n");
    } else {
        e = malloc(sizeof(struct cached_event));
        if (!e) {
            WARN("Failed to put event into cache (no mem)\n");
        } else {
            e->msg = malloc(sizeof(struct mwu_msg) + msg->len);
            if (!e->msg) {
                WARN("Failed to copy event (no mem)\n");
                free(e);
            } else {
                memcpy(e->msg, msg, sizeof(struct mwu_msg) + msg->len);
                STAILQ_INSERT_TAIL(&events, e, list_item);
                num_events++;
            }
        }
    }

    if (eclient == -1) {
        ERR("Can't send event.  No client connected.\n");
        return MWU_ERR_COM;
    }

    ret = sendto(eclient, msg, sizeof(struct mwu_msg) + msg->len, 0, NULL, 0);
    if (ret !=  (int)sizeof(struct mwu_msg) + msg->len) {
        ERR("Failed to send event to client: %s\n", strerror(errno));
        ret = MWU_ERR_COM;
        goto fail;
    }

    INFO("Sent event to client\n");
    return MWU_ERR_SUCCESS;

fail:
    return ret;
}

#endif

enum mwu_error mwu_connect(struct mwu *mwu)
{

    int ret = 0;
    struct sockaddr_un addr;

    ret = pthread_mutex_init(&mwu->mfd_mutex, NULL);
    if (ret) {
        ERR("Failed to init mutex: %s\n", strerror(errno));
        goto fail;
    }

    /* we need two sockets: one to send messages and receive message responses
     * via mwu_send_message(), and the other to receive asynchronous message
     * (i.e., events) that are passed via mwu_recv_message().  Set up the
     * mwu_send_message() socket first.
     */
    mwu->mfd = socket(PF_LOCAL, SOCK_SEQPACKET, 0);
    if(mwu->mfd == -1) {
        ERR("Failed to create event socket\n");
        ret = MWU_ERR_COM;
        goto fail;
    }

    addr.sun_family = AF_LOCAL;
    strncpy(addr.sun_path, MWU_MSG_FIFO, sizeof(addr.sun_path));
    ret = connect(mwu->mfd, (struct sockaddr *)&addr, SUN_LEN(&addr));
    if (ret == -1) {
        ERR("Failed to connect to mwu: %s\n", strerror(errno));
        ERR("Daemon not running?  Permissions?\n");
        ERR("Try again.\n");
        ret = MWU_ERR_COM;
        goto fail;
    }

    /* ...now set up the mwu_recv_message() socket */
    mwu->efd = socket(PF_LOCAL, SOCK_SEQPACKET, 0);
    if(mwu->efd == -1) {
        ERR("Failed to create event socket: %s\n", strerror(errno));
        return MWU_ERR_COM;
    }

    addr.sun_family = AF_LOCAL;
    strncpy(addr.sun_path, MWU_EVENT_FIFO, sizeof(addr.sun_path));
    ret = connect(mwu->efd, (struct sockaddr *)&addr, SUN_LEN(&addr));
    if (ret == -1) {
        ERR("Failed to connect to mwu: %s\n", strerror(errno));
        ERR("Daemon not running?  Permissions?\n");
        ERR("Try again.\n");
        ret = MWU_ERR_COM;
        goto fail;
    }

    return MWU_ERR_SUCCESS;

fail:
    mwu_disconnect(mwu);
    return ret;
}

void mwu_disconnect(struct mwu *mwu)
{
    INFO("Disconnecting from mwu\n");
    if (mwu->mfd != -1) {
        close(mwu->mfd);
        mwu->mfd = -1;
    }

    if (mwu->efd != -1) {
        close(mwu->efd);
        mwu->efd = -1;
    }

    pthread_mutex_destroy(&mwu->mfd_mutex);
}

enum mwu_error mwu_send_message(struct mwu *mwu, struct mwu_msg *msg,
                                struct mwu_msg **resp)
{

    int ret = 0;
    char module[2];

    *resp = NULL;

    // To avoid this function to be re-entry by another thread
    pthread_mutex_lock(&mwu->mfd_mutex);

    ret = sscanf(msg->data, "module=%1s\n", module);
    if (ret != 1) {
        ERR("Failed to find module=<module> key-value pair in message\n");
        ret = MWU_ERR_INVAL;
        goto done;
    }

    ret = sendto(mwu->mfd, msg, msg->len + sizeof(struct mwu_msg), 0, NULL, 0);
    if (ret != (int)sizeof(struct mwu_msg) + msg->len) {
        ERR("Failed to send message\n");
        ret = MWU_ERR_COM;
        goto done;
    }

    /* get the result */
    *resp = malloc(sizeof(struct mwu_msg));
    if (!*resp) {
        ERR("Failed to alloc response message.\n");
        ret = MWU_ERR_NOMEM;
        goto done;
    }

    /* TODO: this should timeout instead of blocking forever. */

    /* recv the header */
    ret = recv(mwu->mfd, *resp, sizeof(struct mwu_msg), MSG_PEEK);
    if (ret == -1) {
        ERR("Failed to recv response.\n");
        ret = MWU_ERR_COM;
        goto done;
    }

    if (ret < (int)sizeof(struct mwu_msg)) {
        ERR("Failed recv complete message header (ret=%d).\n", ret);
        ret = MWU_ERR_COM;
        goto done;
    }

    /* if we're done, flush the socket and quit early */
    if ((*resp)->len == 0) {
        recv(mwu->mfd, *resp, sizeof(struct mwu_msg), 0);
        ret = (*resp)->status;
        free(*resp);
        *resp = NULL;
        goto done;
    }

    /* make room for the rest of the resp */
    *resp = realloc(*resp, sizeof(struct mwu_msg) + (*resp)->len);
    if (!*resp) {
        ERR("Failed to alloc space for command response data.\n");
        ret = MWU_ERR_NOMEM;
        goto done;
    }

    ret = recv(mwu->mfd, *resp, sizeof(struct mwu_msg) + (*resp)->len, 0);
    if (ret == -1) {
        ERR("Failed to recv resp data.\n");
        ret = MWU_ERR_COM;
        goto done;
    }
    ret = MWU_ERR_SUCCESS;

done:
    pthread_mutex_unlock(&mwu->mfd_mutex);

    if (ret != MWU_ERR_SUCCESS && *resp) {
        free(*resp);
        *resp = NULL;
    }

    return ret;
}

void mwu_free_msg(struct mwu_msg *msg)
{
    free(msg);
}

enum mwu_error mwu_recv_message(struct mwu *mwu, struct mwu_msg **msg)
{
    int ret = MWU_ERR_SUCCESS;

    *msg = malloc(sizeof(struct mwu_msg));
    if (!*msg) {
        ERR("Failed to alloc message.\n");
        ret = MWU_ERR_NOMEM;
        goto done;
    }

    /* recv the header */
    ret = recv(mwu->efd, *msg, sizeof(struct mwu_msg), MSG_PEEK);
    if (ret == -1) {
        ERR("Failed to recv message: %s\n", strerror(errno));
        ret = MWU_ERR_COM;
        goto done;
    }

    /* make room for the rest of the cmdresp */
    *msg = realloc(*msg, sizeof(struct mwu_msg) + (*msg)->len);
    if (!*msg) {
        ERR("Failed to alloc space for message body.\n");
        ret = MWU_ERR_NOMEM;
        goto done;
    }
    ret = recv(mwu->efd, *msg, sizeof(struct mwu_msg) + (*msg)->len, 0);
    if (ret == -1) {
        ERR("Failed to recv msg data: %s\n", strerror(errno));
        ret = MWU_ERR_COM;
        goto done;
    }
    ret = MWU_ERR_SUCCESS;

done:
    if (ret != MWU_ERR_SUCCESS && *msg) {
        free(*msg);
        *msg = NULL;
    }
    return ret;
}
