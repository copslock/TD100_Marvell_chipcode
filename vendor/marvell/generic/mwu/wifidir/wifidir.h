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

/* Internal interface for "wifidirect" module
 *
 * OVERVIEW
 *
 * A typical wifidirect use case involves two peers: an initiator and a
 * recipient.  The initiator discovers the recipient and initiates a group
 * negotiation with the recipient.  When the group negotiation completes, one
 * of the peers will end up being the group owner (GO), and the other will be
 * the client.  Note that the initiator may or may not end up being the GO.
 * For the purposes of illustrating the details involved in this transaction,
 * imagine that two nodes are both running this wifidirect module.
 *
 * Our story begins when both initiator and recipient have launched the
 * wifidirect module by calling wifidir_launch(), and have both configured the
 * wifidirect module with a calls to wifidir_init().  After this, the initiator
 * calls wifidir_start_find() to start discovering peers.  As peers are
 * discovered, WIFIDIR_EVENT_PEER_FOUND events will be emitted.  When the peer
 * of interest (i.e., the recipient) appears, the initiator can inspect the
 * peer's supported WPS configuration methods and choose the method that it
 * wishes to use.  Next, it sends a provision discovery request to the
 * recipient using the wifidir_pd_request() function specifying the desired
 * method.
 *
 * Meanwhile, on the recipient, when the provision discovery request is
 * received, a WIFIDIR_EVENT_PD_REQ event will be emitted, and a suitable PD
 * response will be automatically sent.  In response to this event, the
 * recipient should present some sort of UI to the user indicating some details
 * about the initiator.  The recipient should also gather any information that
 * will be required to handle the specified config method.  For example, if the
 * initiator specified a DISPLAY method, the recipient should show a suitable
 * random pin on its display.  If the initiator specified the KEYPAD method,
 * the recipient should present a prompt for the user to enter the pin from the
 * initiator's display.  If the user accepts the provisioning discovery
 * request, the recipient should call wifidir_allow() with the recipient's
 * device_address.
 *
 * Back on the initiator, a WIFIDIR_EVENT_PD_RESP event will be generated when
 * the provisioning discovery response is received.  If the method in the event
 * is the expected method, the initiator should gather any information it needs
 * from the user to proceed with the group negotiation.  For example, if the
 * config method is a DISPLAY method, the initiator should prompt the user to
 * enter the pin from the display of the recipient.  If the method is KEYPAD,
 * the initiator should show a random pin and instruct the user to enter it
 * into the recipient.  Finally, the recipient should proceed by calling
 * wifidir_allow().
 *
 * When group negotiation completes, a WIFIDIR_EVENT_NEGOTIATION_RESULT event
 * will be emitted on both the initiator and the recipient.  Foremost, this
 * event will report whether or not the event succeeded.  If it did succeed,
 * the result will also report whether or not we became the GO and the group
 * ssid.  The sensible thing to do in response to this event depends on whether
 * or not we became the GO.  If so, an AP, WPS registrar, and DHCP server
 * should be launched.  If not, a WPS enrollee should be launched to learn the
 * group's WPA credential information.
 *
 * Before attempting to negotiate with a different peer, the recipient and/or
 * initiator must call wifidir_stop_group().  In order to shutdown cleanly,
 * wifidir_halt() must be called.
 *
 * Variations on this use case abound.  For example, two peers that already
 * know the pin via some other means could entirely skip the provision
 * discovery phase.
 */

#ifndef __WIFIDIR_INTERNAL__
#define __WIFIDIR_INTERNAL__

#include "queue.h"
#include "module.h"
#include "wps_def.h"
#include "mwu_timer.h"

/* WIFIDIR_UUID_MAX_LEN: maximum length of device's UUID. This does not include
 * any null termination.
 */
#define WIFIDIR_UUID_MAX_LEN 16

/* enum wifidir_error
 *
 * These are the return codes that can be returned by the wifidirect module
 * WIFIDIR_ERR_SUCCESS: The operation succeeded.
 * WIFIDIR_ERR_BUSY: The module is busy and cannot handle the operation.
 * WIFIDIR_ERR_INVAL: The arguments were invalid.
 *
 * WIFIDIR_ERR_NOMEM: Insufficient available memory.
 *
 * WIFIDIR_ERR_COM: A system-dependent communication failure has occurred (e.g.,
 * permission denied, failed to create socket, etc.)
 *
 * WIFIDIR_ERR_UNSUPPORTED: The operation is not supported.
 *
 * WIFIDIR_ERR_RANGE: Parameter is out of range.
 * WIFIDIR_ERR_NOENT: No such entity.
 * WIFIDIR_ERR_COM: A low-level driver error occurred.
 * WIFIDIR_ERR_TIMEOUT: the operation timed out
 * WIFIDIR_ERR_NOTREADY: the module is not ready to perform the operation
 */
enum wifidir_error {
    WIFIDIR_ERR_SUCCESS = 0,
    WIFIDIR_ERR_BUSY,
    WIFIDIR_ERR_INVAL,
    WIFIDIR_ERR_NOMEM,
    WIFIDIR_ERR_COM,
    WIFIDIR_ERR_UNSUPPORTED,
    WIFIDIR_ERR_RANGE,
    WIFIDIR_ERR_NOENT,
    WIFIDIR_ERR_TIMEOUT,
    WIFIDIR_ERR_NOTREADY,
};

/* enum wifidir_go_neg_status
 *
 * These are the status of GO negotiation. Please refer to
 * WiFi P2P Technical Specification for details of Status Code definitions.
 *
 * WIFIDIR_GO_NEG_SUCCESS: Success
 * WIFIDIR_GO_NEG_NA: Fail; information is currently unavailable
 * WIFIDIR_GO_NEG_INCOMPARAM: Fail; incompatible parameters
 * WIFIDIR_GO_NEG_LIMIT: Fail; limit reached
 * WIFIDIR_GO_NEG_INVALPARAM: Fail; invalid parameters
 * WIFIDIR_GO_NEG_ACCOMREQUEST: Fail; unable to accommodate request
 * WIFIDIR_GO_NEG_PROTOERROR: Fail; previous protocol error,
 * or disruptive behavior
 *
 * WIFIDIR_GO_NEG_NOCOMCHAN: Fail; no common channels
 * WIFIDIR_GO_NEG_UNGROUP: Fail; unknown P2P group
 * WIFIDIR_GO_NEG_BOTH15: Fail; both P2P devices indicated an intent of 15
 * WIFIDIR_GO_NEG_INCOMPROVISION: Fail; incompatible provisioning method
 * WIFIDIR_GO_NEG_REJECT: fail; rejected by user
 * WIFIDIR_GO_NEG_INTERNAL_ERR: An internal error occured during GO neg.
 * WIFIDIR_GO_NEG_INTERNAL_TIMEOUT: Go negotiation timed out.
 */
enum wifidir_go_neg_status {
    WIFIDIR_GO_NEG_SUCCESS = 0,
    WIFIDIR_GO_NEG_NA,
    WIFIDIR_GO_NEG_INCOMPARAM,
    WIFIDIR_GO_NEG_LIMIT,
    WIFIDIR_GO_NEG_INVALPARAM,
    WIFIDIR_GO_NEG_ACCOMREQUEST,
    WIFIDIR_GO_NEG_PROTOERROR,
    WIFIDIR_GO_NEG_NOCOMCHAN,
    WIFIDIR_GO_NEG_UNGROUP,
    WIFIDIR_GO_NEG_BOTH15,
    WIFIDIR_GO_NEG_INCOMPROVISION,
    WIFIDIR_GO_NEG_REJECT,
    WIFIDIR_GO_NEG_INTERNAL_ERR = -1,
    WIFIDIR_GO_NEG_INTERNAL_TIMEOUT = -2,
};

/* NOTE: These constants are defined elsewhere.  However, the header files in
 * which they are defined require a whole litany of other files to be included
 * first, and this is causing occasional build problems.  So we redefine them
 * here.  If some day these constants are defined in a simple, dedicated header
 * file, these can be replaced.
 */

enum wifidir_group_cap {
    WIFIDIR_GROUP_CAP_GROUP_OWNER = 0x01,
    WIFIDIR_GROUP_CAP_GROUP_PERSIST = 0x02,
    WIFIDIR_GROUP_CAP_GROUP_LIMIT = 0x04,
    WIFIDIR_GROUP_CAP_INTRA_BSS = 0x08,
    WIFIDIR_GROUP_CAP_CROSS_CONNECT = 0x10,
    WIFIDIR_GROUP_CAP_PERSIST_RECONNECT = 0x20,
    WIFIDIR_GROUP_CAP_GROUP_FORMATION = 0x40,
};

/** Maximum no of channels in channel list */
#define MAX_CHANNELS 14

/** chan_list data structure */
typedef struct _channel_list
{
    /** Number of channel */
    int num_of_chan;
    /** Channel number*/
    u16 chan[MAX_CHANNELS];
} channel_list;

/* wifidir_peer: structure representing a wifi-direct peer
 *
 * Note that internally, this structure is also used as a generic container for
 * any event coming from the driver.  That said, every event that comes from
 * the driver may not have every field populated.  So be sure to study each
 * event before assuming that the fields are valid.
 *
 * device_name: The name of this peer device
 *
 * group_capability: group cap bitmask as defined above.
 *
 * list_item: list data used to keep the wifidir_peer in a list.  This data is
 * for internal use only.
 */
struct wifidir_peer {
    char device_name[MAX_DEVICE_NAME_LEN + 1];
    unsigned char device_address[ETH_ALEN];
    unsigned char group_capability;
    unsigned char interface_address[ETH_ALEN];
    char go_ssid[MAX_SSID_LEN + 1];
    unsigned short methods;
    enum wifidir_go_neg_status go_neg_status;
    unsigned short group_cfg_timeout; /* in ms */
    int op_channel;
    channel_list peer_channel_list;

    /* private members */
    SLIST_ENTRY(wifidir_peer) list_item;
    struct mwu_timeval expiry_time;
};

/* wifidir_pd_info: wifidirect provisioning discovery info
 *
 * device_address: the device_address of the peer for which this provisioning info is
 * representative.
 *
 * methods: The methods related to this provisioning discovery info.  In some
 * cases, methods is a bitmask representing the supported methods.  That is, it
 * the logical OR of any number of module_config_method defined in mwpsmod.h.
 * In other cases, methods is the single preferred configuration.
 */
struct wifidir_pd_info {
    unsigned char device_address[ETH_ALEN];
    unsigned short methods;
};

/* wifidir_neg_result: result of a group negotiation
 *
 * status: The status of the negotiation.  If this is WIFIDIR_GO_NEG_SUCCESS,
 * the negotiation has completed successfully and the other members of the
 * struct are valid.  See the documentation for enum wifidir_go_neg_status
 * in wifidir_internal.h for details on other values of <status>.
 *
 * device_address: The device id of the peer with whom the GO negotiation pertains.
 *
 * is_go: 1 or 0 depending on whether we became the GO or not.
 *
 * go_ssid: The ssid of the newly negotiated group.
 *
 */
struct wifidir_neg_result {
    enum wifidir_go_neg_status status;
    unsigned char device_address[ETH_ALEN];
    int is_go;
    char go_ssid[MAX_SSID_LEN + 1];
};

/* wifidir_group_info: group parameters for auto GO
 *
 * go_ssid: The ssid of the newly negotiated group.
 *
 * Please note, this structure currently contains only go_ssid, if required
 * other auto GO parameters should be later accomodated here.
 */
struct wifidir_group_info {
    char go_ssid[MAX_SSID_LEN + 1];
};


/* enum wifidir_event_type
 *
 * These are event types that are emitted from the wifidirect module.
 *
 * WIFIDIR_EVENT_PEER_FOUND: the val member of this event is a struct
 * wifidir_peer representing a peer that has been discovered.
 *
 * WIFIDIR_EVENT_PEER_UPDATED: the val member of this event is a struct
 * wifidir_peer representing a peer that has been discovered.  If the user is
 * maintaining its own list or cache of peers, the peer with the specified
 * device_address should be updated.
 *
 * WIFIDIR_EVENT_PD_REQ: A provision discovery request was received and a
 * suitable PD response has been sent based on the supported configuration
 * methods specified at init time.  The val member of this event is a struct
 * wifidir_pd_info.  The device_address member is the device id of the peer that
 * sent the PD request.  The methods member is set to the single desired
 * configuration method, or MODULE_CONFIG_METHOD_NONE if the request was for
 * an unsupported configuration method.  If the recipient of this event wishes
 * to engage in group negotiation with the specified device, the sensible thing
 * to do is to call the wifidir_allow() function with the device_address of the
 * peer.
 *
 * WIFIDIR_EVENT_PD_RESP: A provision discovery response was received.  The PD
 * response is generally sent in response to a PD request sent with
 * wifidir_pd_request().  Unsolicited PD responses should be ignored.  The val
 * member of this event is a struct wifidir_pd_info.  The device_address member is
 * the device id of the peer that sent the PD response.  The methods member is
 * either set to the single config method that appeared in the PD request, or
 * to MODULE_CONFIG_METHOD_NONE if the peer does not support the desired
 * config method.  The former case indicates that the sending device is
 * prepared to engage in group negotiation and WPS using the specified method.
 *
 * WIFIDIR_EVENT_NEGOTIATION_RESULT: A group negotiation has completed.  In
 * some cases, the group negotiation was initiated with a call to
 * wifidir_negotiate_group().  In other cases, the group negotiation was
 * permitted by calling wifidir_allow(), typically in response to a
 * provisioning discovery request.  The val member of this event is a struct
 * wifidir_neg_result.
 */
enum wifidir_event_type {
    WIFIDIR_EVENT_PEER_FOUND,
    WIFIDIR_EVENT_PEER_UPDATED,
    WIFIDIR_EVENT_PD_REQ,
    WIFIDIR_EVENT_PD_RESP,
    WIFIDIR_EVENT_NEGOTIATION_RESULT,
    WIFIDIR_EVENT_PEER_UNFOUND,
};

/* wifidir_info: runtime stats and parameters for wifidirect module
 */
struct wifidir_info {

};

/* struct wifidir_find_info: details defining how a find should be performed
 */
struct wifidir_find_info {

};

/* wifidir_launch: launch the wifi direct module
 *
 * This function must be called at boot time to start the wifi direct module.
 */
enum wifidir_error wifidir_launch(void);

/* wifidir_halt: halt the wifi direct module
 *
 * Any wifi direct communication underway will be terminated.  This should be
 * called at system shutdown time, presumably by the same entity that called
 * wifidir_launch.
 */
void wifidir_halt(void);

/* wifidir_cfg: wifidirect configuration
 *
 * intent: group-owner intent to be used in GO negotiation.  This must be a
 * number between 0 and 15 inclusive.
 *
 * name: null-terminated device name to be advertised.
 *
 * channel: channel number on which to operate.
 *
 * config_methods: configuration methods supported. It is the caller's
 * responsibility to make sure config_methods the same in WiFi-Direct and WPS.
 *
 * UUID: device's UUID. It is 16 bytes long. Note that it is not a string, so
 * it should not be null-terminated.
 *
 */
struct wifidir_cfg {
    int intent;
    char name[MAX_DEVICE_NAME_LEN + 1];
    int operating_chan;
    int listen_chan;
    unsigned short config_methods;
    unsigned char UUID[WIFIDIR_UUID_MAX_LEN];
};

/* wifidir_init: initialize the wifi direct stack
 *
 * mod: A non-NULL module descriptor.  The interface must be set to the valid
 * system interface string for the interface on which the module should
 * operate.  If the callback is NULL, no events will be emitted.
 *
 * cfg: the wifi direct configuration to use.
 *
 * returns:
 *
 * WIFIDIR_ERR_SUCCESS: The operation succeeded and the wifi direct module is
 * ready to be used with its new configuration.
 *
 * WIFIDIR_ERR_INVAL: The configuration was invalid.
 *
 * WIFIDIR_ERR_BUSY: The wifi direct module has already been initialized.  You
 * must call wifidir_deinit before re-initializing.
 *
 */
int wifidir_init(struct module *mod, struct wifidir_cfg *cfg);

/* wifidir_deinit: de-initialize the wifi direct stack
 *
 * mod: The non-NULL module descriptor that has been initialized by a
 * successful call to wifidir_init().
 *
 * After calling deinit, the wifidirect module will be uninitialized.
 */
void wifidir_deinit(struct module *mod);

/* wifidir_start_find: start the wifidirect find phase
 *
 * mod: The non-NULL module descriptor that has been initialized by a
 * successful call to wifidir_init().
 *
 * find_info: The parameters specifying the find, or NULL if the defaults are
 * to be used.
 *
 * returns:
 *
 * WIFIDIR_ERR_SUCCESS: The operation succeeded.  The caller can expect
 * WIFIDIR_EVENT_PEER_FOUND and WIFIDIR_EVENT_PEER_UPDATED events as peers are
 * discovered and updated.  These events will continue until either: 1) the
 * user calls wifidir_stop_find(), or 2) the user calls
 * wifidir_negotiate_group(), or 3) group negotiation with an allow'd peer
 * commences.
 *
 * WIFIDIR_ERR_COM: A low-level driver error occurred.  This is unexpected
 * and usually represents a bug.
 *
 * WIFIDIR_ERR_BUSY: The wifidirect module is either attempting to negotiate
 * with a peer, or is already a GO or client.  To return to the find phase, the
 * user must call wifidir_deinit() followed by wifidir_init().
 *
 * WIFIDIR_ERR_INVAL: find_info contained invalid parameters.
 *
 * TODO: Is it possible to start the find phase while we're a GO or a client?
 * This would be better so that a user can browse for other peers while
 * connected.
 */
enum wifidir_error wifidir_start_find(struct module *mod,
                                      struct wifidir_find_info *find_info);

/* wifidir_stop_find: stop the find phase
 *
 * mod: The non-NULL module descriptor that has been initialized by a
 * successful call to wifidir_init().
 *
 * After calling this function, the caller can expect peer found and peer
 * updated events to cease.
 */
void wifidir_stop_find(struct module *mod);

/* struct peer_list: a list of wifidir_peers
 *
 * This SLIST_HEAD macro defines the head of a linked list of wifidir_peer
 * structs.  The struct peer_list can be be manipulated using the SLIST_*
 * macros defined in queue.h.  Note that the "list_item" member is the
 * SLIST_ENTRY that is required to navigate the peer list.  See the
 * wifidir_peer_* functions in wifidir.c for examples of using the peer list.
 */
SLIST_HEAD(peer_list, wifidir_peer);

/* wifidir_get_peers: Get a pointer to the current list of wifidir_peers.
 *
 * mod: The non-NULL module descriptor that has been initialized by a
 * successful call to wifidir_init().
 *
 * returns: pointer to a struct peer_list.  This list can be traversed using
 * the SLIST_* macros in queue.h.  If no peers are available, NULL is returned.
 *
 * NOTE: if this function returns a non-NULL value, the caller MUST call
 * wifidir_release_peers() when it is done inspecting the peer list.
 *
 * NOTE: callers should avoid holding the list of peers for very long.  The
 * reason is that the peer list is volatile and likely to change.  Further, the
 * implementation of wifidir_get_peers() may lock the peer list, which will
 * prevent the wifidirect module from updating the peer list.
 */
const struct peer_list *wifidir_get_peers(struct module *mod);

/* wifidir_release_peers: Release the list of peers
 *
 * mod: The non-NULL module descriptor that has been initialized by a
 * successful call to wifidir_init().
 *
 * pl: The peer_list returned by wifidir_get_peers().
 */
void wifidir_release_peers(struct module *mod, const struct peer_list *pl);

/* wifidir_pd_request: send a provisioning discovery request
 *
 * mod: The non-NULL module descriptor that has been initialized by a
 * successful call to wifidir_init().
 *
 * pdi: provisioning discovery info indicating the device id of the peer to
 * whom the PD request should be sent, and the desired config method.
 *
 * returns:
 *
 * WIFIDIR_ERR_SUCCESS: The operation succeeded.  In this case, one of two
 * things can happen.  If the peer receives the provisioning request and sends
 * back a provisioning response, a WIFIDIR_EVENT_PD_RESP event will be
 * generated.  This is the typical case.  If the peer does not receive our
 * request for some reason, or if it fails to send a response, no event will be
 * generated.  It is up to the user to decide how long to wait for the
 * provision discovery response.  The user can invoke wifidir_pd_request
 * repeatedly without any clean-up action between calls if it wishes to re-send
 * the PD request.
 *
 * WIFIDIR_ERR_COM: A low-level driver issue occurred and the PD request was
 * not sent.
 *
 * WIFIDIR_ERR_BUSY: The wifidirect module is either negotiation with a peer,
 * is already a GO, or is already a client.  To send a PD request, the caller
 * must first call wifidir_deinit() then wifidir_init().
 */
enum wifidir_error wifidir_pd_request(struct module *mod,
                                      struct wifidir_pd_info *pdi);

/* wifidir_allow: allow the wifidirect module to engage in group negotiation
 * with the specified peer
 *
 * mod: The non-NULL module descriptor that has been initialized by a
 * successful call to wifidir_init().
 *
 * device_address: the 6-byte device id of the peer to allow
 *
 * pdi: Provisioning info containing the device_address of the peer to engage in
 * negotiation with and the desired method.
 *
 * returns:
 *
 * WIFIDIR_ERR_SUCCESS: The operation succeeded.
 *
 * WIFIDIR_ERR_BUSY: The wifidirect module is either already negotiating with a
 * peer, is already a GO, or is already a client.
 *
 * NOTE: The purpose of this function is to prevent the wifidirect module from
 * engaging in unsolicited group negotiation.  It also prevents group
 * negotiation from proceeding until the recipient is ready (i.e., all of the
 * relevant WPS data has been collected).
 */
enum wifidir_error wifidir_allow(struct module *mod,
                                 struct wifidir_pd_info *pdi);

/* wifidir_get_info: Get current stats and settings for wifidir module
 *
 * mod: The non-NULL module descriptor that has been initialized by a
 * successful call to wifidir_init().
 *
 * cfg: non-NULL structure that will be populated with the current
 * configuration the module.
 *
 * wifidir_info: non-NULL structure that will be populated with the current
 * runtime stats and parameters for this STA.
 *
 * returns:
 *
 * WIFIDIR_ERR_SUCCESS: The command succeeded and wifidir_cfg and wifidir_info
 * have been populated with the current stats and settings.
 *
 * WIFIDIR_ERR_COM: A low-level/driver issue occured.  Data in net_info and
 * wifidir_info is not valid.
 *
 * WIFIDIR_ERR_INVAL: An input parameter was invalid (i.e., NULL).
 *
 * NOTE: If wifidir_init() has been called, the wifidir_cfg struct will contain
 * the same info passed to that function.  If wifidir_deinit() has been called,
 * or if wifidir_init() has never been called, this struct will be set with the
 * default configuration information.
 *
 * NOTE: If wifidir_set_info() has never been called, the parameters in
 * wifidir_info will be populated with their default values.
 */
enum wifidir_error wifidir_get_info(struct module *mod,
                                    struct wifidir_cfg *cfg,
                                    struct wifidir_info *wifidir_info);

/* wifidir_set_info: Set stats and settings for sta
 *
 * mod: The non-NULL module descriptor that has been initialized by a
 * successful call to wifidir_init().
 *
 * wifidir_info: non-NULL settings and stat info to apply to the STA
 *
 * returns:
 *
 * WIFIDIR_ERR_SUCCESS: The command succeeded.  The data in wifidir_info has
 * been applied.
 *
 * WIFIDIR_ERR_COM: A low-level/driver issue occured.  Data in net_info and
 * wifidir_info is not valid.
 *
 * WIFIDIR_ERR_BUSY: The module is currently busy and the data in wifidir_info
 * could not be applied.
 *
 * WIFIDIR_ERR_INVAL: An input parameter in wifidir_info was invalid.
 */
enum wifidir_error wifidir_set_info(struct module *mod,
                                    struct wifidir_info *wifidir_info);

/* wifidir_negotiate_group: initiate group negotiation
 *
 * mod: The non-NULL module descriptor that has been initialized by a
 * successful call to wifidir_init().
 *
 * pdi: Provisioning info containing the device_address of the peer to engage in
 * negotiation with and the desired method.
 *
 * returns:
 *
 * WIFIDIR_ERR_SUCCESS: The operation succeeded.  The user can eventually
 * expect an WIFIDIR_EVENT_NEGOTIATION_RESULT event.  This event will indicate
 * whether or not the group negotiation succeeded or failed.
 *
 * WIFIDIR_ERR_BUSY: The wifidirect module is already engaged in group
 * negotiation with a peer, is already a GO, or is already a client.
 *
 * WIFIDIR_ERR_COM: A low-level driver problem occurred and the group
 * negotiation was not started.
 */
enum wifidir_error wifidir_negotiate_group(struct module *mod,
                                           struct wifidir_pd_info *pdi);

/* wifidir_start_group: start a group (i.e., auto-GO mode)
 *
 * mod: The non-NULL module descriptor that has been initialized by a
 * successful call to wifidir_init().
 *
 * info: non-NULL struct that will be populated with the group parameters
 * (i.e., ssid, etc.) of the new group.
 *
 * returns:
 *
 * WIFIDIR_ERR_SUCCESS: The operation succeeded.  The group has been formed.
 *
 * WIFIDIR_ERR_BUSY: The wifidirect module is already engaged in group
 * negotiation with a peer, is already a GO, or is already a client.
 * wifidir_deinit() must be called before a new group can be formed.
 *
 * WIFIDIR_ERR_COM: A low-level driver problem occurred and the group has not
 * been formed.
 */
enum wifidir_error wifidir_start_group(struct module *mod,
                                       struct wifidir_group_info *info);

/* wifidir_stop_group: stop a group
 *
 * mod: The non-NULL module descriptor that has been initialized by a
 * successful call to wifidir_init().
 *
 * NOTE: this function can be called during GO negotiation, or after the
 * WIFIDIR_EVENT_NEGOTIATION_RESULT, or after a successful call to
 * wifidir_start_group() to return the module to its idle state.  From there,
 * wifidir_start_group() or wifidir_negotiate_group() can be called.
 */
void wifidir_stop_group(struct module *mod);

#endif
