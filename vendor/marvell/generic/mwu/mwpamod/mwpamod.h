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

/* Internal interface for "mwpamod" module
 *
 * OVERVIEW
 *
 * This module implements wpa-capable STA and AP capabilities.  To illustrate
 * how these capabilities are used, imagine that you have two different wifi
 * nodes that are both using this module.  One will implement the AP and the
 * other the STA.
 *
 * Our story begins when the mwpamod module is launched on both the AP and STA
 * with calls to mwpamod_launch().  Next, the AP is initialized with a call to
 * mwpamod_ap_init() and started with a call to mwpamod_ap_start().  Note that
 * the struct module passed to mwpamod_ap_init() must specify the interface and
 * a callback for receiving events back from the AP.  The config passed to
 * mwpamod_ap_init() must specify the SSID and key information for the AP.  At
 * any time (i.e., before or after calling mwpamod_ap_start()),
 * mwpamod_ap_get_info() and mwpamod_ap_set_info() can be called to get and set
 * various runtime parameters and statistics from the AP.  If the call to
 * mwpamod_ap_start() succeeds, the AP will be up and running and prepared to
 * accept connections.
 *
 * Meanwhile, the STA is initialized with a call to wpamod_sta_init().  This
 * function requires a struct mwpamod_sta that specifies the interface and
 * callback.  After a successful call to this function, the STA must discover
 * the AP.  This happens through some means outside of the scope of this
 * module, the client will discover the SSID of the network to which he wishes
 * to connect, along with any other network information required to connect
 * (i.e., encryption key).  At any time, mwpamod_sta_set_info() and
 * mwpamod_sta_get_info() can be called to get and set various runtime
 * parameters and statistics for the STA.
 *
 * Next, the STA must call mwpamod_sta_connect() with the network info for the
 * AP.  After this, the user can expect a MWPAMOD_EVENT_STA_CONNECT event when
 * the connection has completed.  The status code accompanying this event will
 * reveal whether or not the connection succeeded.  Meanwhile, over on the AP,
 * a MWPAMOD_EVENT_AP_CONNECT will be emitted with information about the STA
 * and a status code indicating if the STA was able to successfully associate
 * and authenticate.  At this point, the nodes are connected.
 *
 * After successfully connecting, the STA can call mwpamod_sta_disconnect() to
 * disconnect from the network cleanly.  This will cause an
 * MWPAMOD_EVENT_AP_DISCONNECT event on the AP.  Then the STA can call
 * mwpamod_sta_connect() again to connect to a different network, or to
 * re-connect to the same network.  Alternatively, to shut down the STA such
 * that mwpamod_sta_init() can be called to restart, the STA can call
 * mwpamod_sta_deinit().
 *
 * On the other side of the connection, the AP can call mwpamod_ap_disconnect()
 * to kick a particular STA off the network cleanly.  Also, the AP can call
 * mwpamod_ap_stop() to stop the AP, or mwpamod_ap_deinit() to shut it down
 * entirely.
 *
 * If the connection is lost for any reason, the STA can expect a
 * MWPAMOD_EVENT_STA_LINK_LOST.  After this event, the client can call
 * mwpamod_sta_connect() again.  On the AP side, if an STA vanishes for any
 * reason (i.e., the link was lost or the STA disconnected), a
 * MWPAMOD_EVENT_AP_DISCONNECT will be emitted with information about the STA.
 *
 * Finally, to shutdown cleanly, mwpamod_halt() must be called.
 *
 * TODO: Develop mwpamod_auth use case.
 *
 */

#ifndef __MWPAMOD_INTERNAL__
#define __MWPAMOD_INTERNAL__

#include "module.h"
#include "mwpamod_os.h" /* for rtnl_handle */
/* enum mwpamod_error
 *
 * These are the return codes that can be returned by the mwpamod module
 *
 * MWPAMOD_ERR_SUCCESS: The operation succeeded.
 * MWPAMOD_ERR_BUSY: The module is busy and cannot handle the operation.
 * MWPAMOD_ERR_INVAL: The arguments were invalid.
 * MWPAMOD_ERR_NOMEM: Insufficient available memory.
 * MWPAMOD_ERR_COM: A system-dependent communication failure has occurred (e.g.,
 * permission denied, failed to create socket, etc.)
 * MWPAMOD_ERR_NOENT: No such entity.
 * MWPAMOD_ERR_TIMEOUT: the operation timed out
 * MWPAMOD_ERR_NOTREADY: the module is not ready to perform the operation
 * MWPAMOD_ERR_NAK: the AP declined to allow us to connect
 */
enum mwpamod_error {
    MWPAMOD_ERR_SUCCESS = 0,
    MWPAMOD_ERR_BUSY,
    MWPAMOD_ERR_INVAL,
    MWPAMOD_ERR_NOMEM,
    MWPAMOD_ERR_COM,
    MWPAMOD_ERR_UNSUPPORTED,
    MWPAMOD_ERR_NOENT,
    MWPAMOD_ERR_TIMEOUT,
    MWPAMOD_ERR_NOTREADY,
    MWPAMOD_ERR_NAK,
};

/* MWPAMOD_MAX_SSID: maximum ssid length.
 */
#define MWPAMOD_MAX_SSID 32

/* MWPAMOD_MAX_KEY: maximum key length. */
#define MWPAMOD_MAX_KEY 64

/* MWPAMOD_MIN_KEY: minimum key length. */
#define MWPAMOD_MIN_KEY 8

/* MWPAMOD_MAX_AP_CLIENTS: Max clients supported by AP */
#define MWPAMOD_MAX_AP_CLIENTS 16

/* enum mwpamod_event_type
 *
 * These are event types that are emitted from the mwpamod module as a struct
 * event.
 *
 * MWPAMOD_EVENT_STA_CONNECT: A module will emit this event after a successful
 * call to mwpamod_sta_connect().  The status member of the event will be
 * MWPAMOD_ERR_SUCCESS if connection with the network succeeded.  After this
 * event, mwpamod_sta_disconnect() must be called to cleanly disconnect from
 * the network.  The val member can be ignored for this event.  The following
 * are the meanings for other status codes:
 *
 *     MWPAMOD_ERR_NAK: We received a NAK from the AP.  Perhaps authentication
 *     failed.
 *
 *     MWPAMOD_ERR_COM: A low-level driver issue occured.
 *
 *     MWPAMOD_ERR_NOENT: The SSID passed to mwpamod_sta_connect() could not be
 *     found.
 *
 *     MWPAMOD_ERR_TIMEOUT: Timed out waiting for the connection to succeed.
 *
 * MWPAMOD_EVENT_LINK_LOST: The caller can expect this event after a
 * MWPAMOD_EVENT_STA_CONNECT with a status of MWPAMOD_ERR_SUCCESS if the link
 * is lost for any reason. mwpamod_sta_connect() may be called again to connect
 * to a new network.  The val member can be ignored for this event.  The status
 * member of this event has the following meaning:
 *
 *     MWPAMOD_ERR_NAK: The AP deauth'd us for some reason.
 *
 *     MWPAMOD_ERR_COM: An unexpected low-level driver issue occured.
 *
 *     MWPAMOD_ERR_TIMEOUT: Failed to reconnect to network automatically after
 *     a number of attempts.  This happens when the AP is suddenly out of
 *     range.
 *
 * MWPAMOD_EVENT_AP_CONNECT: The module is configured as an AP and an STA has
 * attempted to connect.  The val member of this event is set to the mac
 * address of the STA, which is ETH_ALEN bytes long.  The status code has the
 * following meanings:
 *
 *     MWPAMOD_ERR_SUCCESS: The STA has successfully associated and
 *     authenticated with the AP and is now available for communication.
 *
 *     MWPAMOD_ERR_NAK: The STA failed to be authenticated.
 *
 *     MWPAMOD_ERR_TIMEOUT: The STA initiated a connection but never completed
 *     it.
 *
 * MWPAMOD_EVENT_AP_DISCONNECT: An STA for which a MWPAMOD_EVENT_AP_CONNECT was
 * emitted has left the network and is no longer available.  The val member is
 * set to the mac address of the STA, which is ETH_ALEN bytes long.  The status
 * code can be ignored in this case.
 */
enum mwpamod_event_type {
    MWPAMOD_EVENT_STA_CONNECT,
    MWPAMOD_EVENT_STA_LINK_LOST,
    MWPAMOD_EVENT_AP_CONNECT,
    MWPAMOD_EVENT_AP_DISCONNECT,
};

/* mwpamod_net_info: network info specifying encryption, ssid, etc.
 *
 * ssid: The null-terminated SSID of the network.
 *
 * key: The actual encryption key
 *
 * key_len: length of the key.  If len is 64, key is a null-terminated 32-byte
 * PSK repreasented as 64 hex chars for a WPA2 network.  If len is less than
 * 64, key is a null-terminated ascii passphrase for a WPA2 network.  Note that
 * key_len never includes the null-termination byte.  If len is 0, key is
 * ignored and the net_info represents an open network.
 */
struct mwpamod_net_info {
    char ssid[MWPAMOD_MAX_SSID + 1];
    char key[MWPAMOD_MAX_KEY + 1];
    int key_len;
};

/* struct mwpamod_sta_config: init-time configuration for STA
 *
 */
struct mwpamod_sta_config {

};

/* struct mwpamod_sta_info: runtime parameters and stats for an STA
 *
 */
struct mwpamod_sta_info {
    int ioctl_sock;
    /* for events from wext*/
    struct rtnl_handle rth;
    /* private state tracking variable */
    int sta_state;
};

/* struct mwpamod_ap_config: init-time configuration for AP
 *
 */
struct mwpamod_ap_config {
    struct mwpamod_net_info net_info;
};

/* associated station info */
struct mwpamod_ap_sta_info
{
    /** STA MAC address */
    unsigned char mac_address[ETH_ALEN];
    /** Power mfg status */
    unsigned char power_mfg_status;
    /** RSSI */
    char rssi;
};

/* sta_list structure */
struct mwpamod_ap_sta_list
{
    /** station count */
    unsigned short sta_count;
    /** station list */
    struct mwpamod_ap_sta_info sta_info[MWPAMOD_MAX_AP_CLIENTS];
};

/* struct mwpamod_ap_info: runtime parameters and stats for an AP
 *
 */
struct mwpamod_ap_info {
    int basic_rate;
    int ap_state;
    struct mwpamod_ap_sta_list sta_list;
};

/* mwpamod_launch: launch the mwpamod module
 *
 * This function must be called at boot time to start the mwpamod module.  After
 * this call, the other API calls can be issued.
 *
 */
enum mwpamod_error mwpamod_launch(void);

/* mwpamod_halt: halt mwpamod
 *
 * Any wpa communication underway will be terminated.  This should be called at
 * system shutdown time, presumably by the same entity that called
 * mwpamod_launch.
 */
void mwpamod_halt(void);

/* mwpamod_sta_init: initialize an instance of mwpamod STA
 *
 * mod: A non-NULL module descriptor.  The interface must be set to the valid
 * system interface string for the interface on which the module should
 * operate.  If the callback is NULL, no events will be emitted.
 *
 * cfg: Configuration for the STA or NULL to use the default config.
 *
 * returns:
 *
 * MWPAMOD_ERR_SUCCESS: The operation succeeded and the module is available.
 *
 * MWPAMOD_ERR_INVAL: The interface string was not a valid system interface.
 *
 * MWPAMOD_ERR_COM: A low-level driver or communication issue failed.
 *
 * MWPAMOD_ERR_BUSY: The module is too busy.  (e.g., the maximum number of
 * supported modules have already been initialized).
 *
 * MWPAMOD_ERR_NOMEM: Internal memory allocation failed.
 */
enum mwpamod_error mwpamod_sta_init(struct module *mod,
                                    struct mwpamod_sta_config *cfg);

/* mwpamod_sta_deinit: deinitialize an instance of mwpamod STA
 *
 * mod: The non-NULL module descriptor that has been initialized by a
 * successful call to mwpamod_sta_init().
 */
void mwpamod_sta_deinit(struct module *mod);

/* mwpamod_sta_connect: connect an instance of a mwpamod STA to a network
 *
 * mod: The non-NULL module descriptor that has been initialized by a
 * successful call to mwpamod_sta_init().
 *
 * net_info: The network information specifying the network to connect to.
 * Must contain valid ssid and key information in order to successfully
 * connect.
 *
 * returns:
 *
 * MWPAMOD_ERR_SUCCESS: The operation succeeded.  The user can eventually
 * expect MWPAMOD_EVENT_STA_CONNECT when the connection completes or fails.
 *
 * MWPAMOD_ERR_INVAL: Specified net_info is not valid.
 *
 * MWPAMOD_ERR_BUSY: mwpamod is busy (e.g., already connecting to a network or
 * already connected).  mwpamod_sta_disconnect() must be called before calling
 * this function.
 *
 * MWPAMOD_ERR_COM: An internal/driver error has occurred, and mwpamod STA
 * could not be connected.
 */
enum mwpamod_error mwpamod_sta_connect(struct module *mod,
                                       struct mwpamod_net_info *net_info);

/* mwpamod_sta_disconnect: disconnect from a network
 *
 * mod: The non-NULL module descriptor that has been initialized by a
 * successful call to mwpamod_sta_init().
 *
 * After calling this function, mwpamod_sta_connect() can be called again.  If
 * a connection is underway or already established, it will be canceled.
 */
void mwpamod_sta_disconnect(struct module *mod);

/* mwpamod_sta_get_info: Get current stats and settings for sta
 *
 * mod: The non-NULL module descriptor that has been initialized by a
 * successful call to mwpamod_sta_init().
 *
 * cfg: non-NULL structure that will be populated with the configuration of the
 * STA that was passed to mwpamod_sta_init().
 *
 * net_info: non-NULL structure that will be populated with the current network
 * configuration of the STA.
 *
 * sta_info: non-NULL structure that will be populated with the current runtime
 * stats and parameters for this STA.
 *
 * returns:
 *
 * MWPAMOD_ERR_SUCCESS: The command succeeded and net_info and sta_info have
 * been populated with the current stats and settings.
 *
 * MWPAMOD_ERR_COM: A low-level/driver issue occured.  Data in net_info and
 * sta_info is not valid.
 *
 * MWPAMOD_ERR_INVAL: An input parameter was invalid (i.e., NULL).
 *
 * NOTE: If mwpamod_sta_init() was invoked with NULL for the cfg, cfg will be
 * populated with the default cfg.
 *
 * NOTE: If mwpamod_sta_connect() has been called, this struct will contain the
 * same net_info passed to that function.  If mwpamod_sta_disconnect() has been
 * called, or if mwpamod_sta_connect() has never been called, this struct will
 * be set with default configuration information.
 *
 * NOTE: If mwpamod_sta_set_info() has never been called, the parameters in
 * sta_info will be populated with their default values.
 */
enum mwpamod_error mwpamod_sta_get_info(struct module *mod,
                                        struct mwpamod_sta_config *cfg,
                                        struct mwpamod_net_info *net_info,
                                        struct mwpamod_sta_info *sta_info);

/* mwpamod_sta_set_info: Set stats and settings for sta
 *
 * mod: The non-NULL module descriptor that has been initialized by a
 * successful call to mwpamod_sta_init().
 *
 * sta_info: non-NULL settings and stat info to apply to the STA
 *
 * returns:
 *
 * MWPAMOD_ERR_SUCCESS: The command succeeded.  The data in sta_info has been
 * applied.
 *
 * MWPAMOD_ERR_COM: A low-level/driver issue occured.  Data in net_info and
 * sta_info is not valid.
 *
 * MWPAMOD_ERR_BUSY: The module is currently busy and the data in sta_info
 * could not be applied.
 *
 * MWPAMOD_ERR_INVAL: An input parameter in sta_info was invalid.
 */
enum mwpamod_error mwpamod_sta_set_info(struct module *mod,
                                        struct mwpamod_sta_info *sta_info);

/* mwpamod_ap_init: initialize an instance of mwpamod AP
 *
 * mod: A non-NULL module descriptor.  The interface must be set to the valid
 * system interface string for the interface on which the module should
 * operate.  If the callback is NULL, no events will be emitted.
 *
 * cfg: The configuration of the AP (e.g., ssid and key information), or NULL
 * to use the default configuration.
 *
 * returns:
 *
 * MWPAMOD_ERR_SUCCESS: The operation succeeded and the module is available.
 *
 * MWPAMOD_ERR_INVAL: The interface string or configuration was not a valid
 * system interface.
 *
 * MWPAMOD_ERR_COM: A low-level driver or communication issue failed.
 *
 * MWPAMOD_ERR_BUSY: The module is too busy.  (e.g., the maximum number of
 * supported modules have already been initialized).
 *
 * MWPAMOD_ERR_NOMEM: Internal memory allocation failed.
 */
enum mwpamod_error mwpamod_ap_init(struct module *mod,
                                   struct mwpamod_ap_config *cfg);

/* mwpamod_ap_deinit: deinitialize an instance of mwpamod AP
 *
 * mod: The non-NULL module descriptor that has been initialized by a
 * successful call to mwpamod_ap_init().
 */
void mwpamod_ap_deinit(struct module *mod);

/* mwpamod_ap_start: start the AP
 *
 * mod: The non-NULL module descriptor that has been initialized by a
 * successful call to mwpamod_ap_init().
 *
 * returns:
 *
 * MWPAMOD_ERR_SUCCESS: The operation succeeded and the AP is up and running.
 *
 * MWPAMOD_ERR_INVAL: The net_info was not a valid.
 *
 * MWPAMOD_ERR_COM: A low-level driver or communication issue failed.
 *
 * MWPAMOD_ERR_BUSY: The module is busy (e.g., start was already called).
 *
 */
enum mwpamod_error mwpamod_ap_start(struct module *mod);

/* mwpamod_ap_stop: stop the AP
 *
 * After this call, mwpamod_ap_start can be called again to restart the AP.
 *
 * mod: The non-NULL module descriptor that has been initialized by a
 * successful call to mwpamod_ap_init().
 *
 */
void mwpamod_ap_stop(struct module *mod);

/* mwpamod_ap_get_info: Get current stats and settings for AP
 *
 * mod: The non-NULL module descriptor that has been initialized by a
 * successful call to mwpamod_ap_init().
 *
 * cfg: non-NULL structure that will be populated with the current
 * configuration of the AP.
 *
 * ap_info: non-NULL structure that will be populated with the current runtime
 * stats and parameters for this AP.
 *
 * returns:
 *
 * MWPAMOD_ERR_SUCCESS: The command succeeded and net_info and ap_info have
 * been populated with the current stats and settings.
 *
 * MWPAMOD_ERR_COM: A low-level/driver issue occured.  Data in net_info and
 * ap_info is not valid.
 *
 * MWPAMOD_ERR_INVAL: An input parameter was invalid (i.e., NULL).
 *
 * NOTE: The cfg struct will be populated with the same info passed to
 * mwpamod_ap_init().  If NULL was passed, cfg will contain the default
 * configuration.
 *
 * NOTE: If mwpamod_ap_set_info() has never been called, the parameters in
 * ap_info will be populated with their default values.
 */
enum mwpamod_error mwpamod_ap_get_info(struct module *mod,
                                        struct mwpamod_ap_config *cfg,
                                        struct mwpamod_ap_info *ap_info);

/* mwpamod_ap_set_info: Set stats and settings for ap
 *
 * mod: The non-NULL module descriptor that has been initialized by a
 * successful call to mwpamod_ap_init().
 *
 * ap_info: The non-NULL settings and stat info to apply to the AP
 *
 * returns:
 *
 * MWPAMOD_ERR_SUCCESS: The command succeeded.  The data in ap_info has been
 * applied.
 *
 * MWPAMOD_ERR_COM: A low-level/driver issue occured.  Data in net_info and
 * ap_info is not valid.
 *
 * MWPAMOD_ERR_BUSY: The module is currently busy and the data in ap_info
 * could not be applied.
 *
 * MWPAMOD_ERR_INVAL: An input parameter in ap_info was invalid.
 */
enum mwpamod_error mwpamod_ap_set_info(struct module *mod,
                                       struct mwpamod_ap_info *ap_info);

/* mwpamod_ap_disconnect: disconnect a particular STA from the AP
 *
 * mod: The non-NULL module descriptor that has been initialized by a
 * successful call to mwpamod_ap_init().
 *
 * mac: the mac address of the STA to disconnect.  mac must be ETH_ALEN bytes
 * long.
 *
 */
void mwpamod_ap_disconnect(struct module *mod, unsigned char *mac);

#endif
