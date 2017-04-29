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

/* mwu module "wifidirect"
 *
 * The wifidirect module provides wifi direct functionality including GO
 * negotiation, provisioning requests and responses, peer discovery, etc.  This
 * header file defines the command/response/event message interface offered by
 * the wifi direct module via mwu.  For the internal wifi direct API used to
 * actually operate on the wifidirect state machine, see wifidir.h.  Developers
 * creating control programs in C or C++ that use the mwu interface may wish to
 * include wifidir.h to define various constants that appear in the messages.
 *
 * All messages sent to the wifidirect module must have iface=<iface> as their
 * second key-value pair and cmd=<cmd> as their third.  <iface> must be a
 * suitable system interface capable of wifidirect.  <cmd> must be one of the
 * commands specified below under "wifidirect COMMANDS".  The response messages
 * to these commands varies depending on the <cmd>.  All messages emitted by
 * the wifidirect module will have module=wifidirect as their first key-value
 * pair, iface=<iface> as their second, and event=<event> as their third
 * key-value pair.  Events are described below under "wifidirect EVENTS".
 *
 * See the OVERVIEW documentation in wifidirect_internal.h for more details
 * regarding the use of the wifidirect module.
 *
 * wifidirect COMMANDS:
 *
 * init: initialize the wifidirect module.  No other commands can be issued
 * until this one is issued.  In addition to the three required key-value
 * pairs, this command must contain the following key-value pairs in order:
 *
 *     name=<name>: The desired wifi direct device name.  <name> must be a
 *     string of ascii characters and must not exceed MAX_DEVICE_NAME_LEN
 *     in length and must not contain whitespace.
 *
 *     intent=<intent>: The GO intent for use in group owner negotiation.  This
 *     must be a decimal number between 0 and 15 inclusive.
 *
 * The command response for the init command contains a single status=<status>
 * key-value pair.  <status> is a decimal number representing the result of the
 * command.  See the documentation for the wifidir_init() function in wifidir.h
 * for details on the meaning of the status and other details.
 *
 * deinit: deinitialize the wifidirect module.  This command does not require
 * any extra key-value pairs.  In response to this command, all wifi direct
 * activity will be halted and the wifi direct module will return to its
 * uninitialized state.  This command does not send a response message.  See
 * the documentation for the wifidir_deinit() function for additional detail on
 * this command.
 *
 * start_find: start the find phase.  This command does not require any extra
 * key-value pairs.  It sends a response message with status=<status> in which
 * <status> is the return code from the wifidir_start_find() function
 * documented in wifidir.h.  After sending this command, the user can expect
 * peer_found and peer_updated events to appear via mwu_recv_message().
 *
 * stop_find: stop the find phase.  This command does not require any extra
 * key-value pairs and does not send a response message.  Additional details on
 * the behavior of this command can be found in the documentation of the
 * wifidir_stop_find() function in wifidir.h.
 *
 * dump_peers: dump the current list of known peers.  This command does not
 * require any extra key-value pairs.  If there are no peers in the table,
 * there will be no response message.  Otherwise, the response message contains
 * a list of peers.  Each peer has a fixed number of key value pairs.
 * Specifically:
 *
 *     device_id=<device_id>: the device mac address of the peer formatted as
 *     hexadecimal digits like so: 00:11:22:33:44:55
 *
 *     device_name=<name>: the name of the device formatted as an ascii string
 *     with a maximum length of 32 characters (MAX_DEVICE_NAME_LEN).
 *
 *     is_go=<boolean>: whether or not the peer is a group owner.  <boolean> is
 *     set to the ascii "true" if the peer is a GO, "false" otherwise.
 *
 *     go_ssid=<ssid>: If is_go is set to "true", this is the SSID of the
 *     group.  Otherwise <ssid> is set to the empty string "".  Note that
 *     <ssid> is never longer than 32 characters.
 *
 *     methods=<methods>: The WPS configuration methods supported by this peer.
 *     This is an upper-case hexadecimal number that is to be interpreted as a
 *     bitmask.  The bitmap is the logical OR of all of the supported methods.
 *     See enum module_config_method in mwpsmod.h for the list of all methods.
 *     Note that for wifi direct, only the DISPLAY, PBC, and KEYPAD methods are
 *     allowed.
 *
 * pd_req: send a provision discovery request.  This command requires the
 * following key-value pairs in addition to required module, iface, and cmd
 * key-value pairs:
 *
 *     device_id=<device_id>: the mac address of the peer to whom the
 *     provisioining request should be sent.  Must be formatted as hexadecimal
 *     digits like so: 00:11:22:33:44:55.
 *
 *     methods=<method>: The single desired WPS configuration method.  This is
 *     the 4-digit upper-case hexadecimal representation of the config method
 *     from enum module_config_method.  For example, for
 *     MODULE_CONFIG_METHOD_DISPLAY, methods=0008.
 *
 * The response message for this command contains the single status=<status>
 * key-value pair.  <status> is the return code from the wifidir_pd_request()
 * function.  The meaning of the status code an other behavior are documented
 * alongside that function in wifidir.h.  Also see the documentation on the
 * pd_resp event below, which is emitted if we receive a provision discovery
 * response from the peer.
 *
 * allow: allow the wifidirect module to engage in group negotiation with the
 * specified peer.  This command requires two key-value pairs in addition to
 * the required module, iface, and cmd pairs:
 *
 *     device_id=<device_id>: the mac address of the peer to allow.  Must be
 *     formatted as hexadecimal digits like so: 00:11:22:33:44:55.
 *
 *     methods=<method>: The single desired WPS configuration method.  This is
 *     the 4-digit upper-case hexadecimal representation of the config method
 *     from enum module_config_method.  For example, for
 *     MODULE_CONFIG_METHOD_DISPLAY, methods=0008.
 *
 * The response message for this command contains the usual status=<status>
 * key-value pair.  See the wifidir_allow() for details on the meaning of
 * <status> and other important details regarding this command.
 *
 * negotiate_group: start wifidirect group formation negotiation with the
 * specified peer.  This command requires the following key-value pairs:
 *
 *     device_id=<device_id>: the mac address of the peer to negotiate with.
 *     Must be formatted as hexadecimal digits like so: 00:11:22:33:44:55.
 *
 *     methods=<method>: The single desired WPS configuration method.  This is
 *     the 4-digit upper-case hexadecimal representation of the config method
 *     from enum module_config_method.  For example, for
 *     MODULE_CONFIG_METHOD_DISPLAY, methods=0008.
 *
 * The response message for this command contains the usual status=<status>
 * key-value pair.  For the meaning of <status> and other important details
 * regarding this command, see the documentation for the
 * wifidir_negotiate_group() function in wifidir.h.  Note that if this command
 * returns status=WIFIDIR_ERR_SUCCESS, the user can expect a negotiation_result
 * event (see below).
 *
 * wifidirect EVENTS
 *
 * peer_found: In addition to the required module, iface, and event key-value
 * pairs, this event will have the key-value pairs describing the single peer
 * that has been found.  These key-value pairs are identical to those described
 * under the "dump_peers" command above.
 *
 * peer_updated: This is the same as the peer_found event, except it indicates
 * that an existing peer has been updated as opposed to a new peer being found.
 *
 * pd_resp: A provisioning discovery response has been received.  Presumably
 * this is in response to a pd_req command.  The pd_resp event has the same
 * key-value pairs as the pd_req command described above.  See the
 * documentation for the WIFIDIR_EVENT_PD_RESP in wifidir.h for more detail.
 *
 * pd_req: A provisioning discovery request has been received.  The pd_req
 * event has the same key-value pairs as the pd_req command described above.
 * However, the <method> will be a single MODULE_CONFIG_METHOD_* value, not
 * the logical OR of many such values.  See the documentation in wifidir.h for
 * WIFIDIR_EVENT_PD_REQ for more detail.
 *
 * negotiation_result: Group negotiation that was initiated by a successful
 * negotiate_group command has completed.  This event contains at least two
 * key-value pairs in addition to the required module, iface, and event
 * key-value pairs:
 *
 *     status=<status>: Set to the decimal value of WIFIDIR_GO_NEG_SUCCESS if
 *     the negotiation succeeded.  See the documentation for enum
 *     wifidir_go_neg_status in wifidir.h for details on other values of
 *     <status>.
 *
 *     device_id=<device_id>: the mac address of the peer with whom negotiation
 *     has completed formatted as hexadecimal digits like so:
 *     00:11:22:33:44:55.
 *
 * If <status> is WIFIDIR_GO_NEG_SUCCESS, the following additional key-value pairs
 * will follow:
 *
 *     is_go=<boolean>: <boolean> is either "true" or "false" depending on
 *     whether we became the group owner or not.  If <boolean> is "false", the
 *     user must connect to the GO using the ssid and psk specified below.
 *     This operation is beyond the scope of the wifidirect module.
 *
 *     go_ssid=<ssid>: The SSID of the new group.  Note that <ssid> is never
 *     longer than 32 characters.
 *
 * NOTE: Developers working on the wifidirect module itself: please note that
 * the wifidirect module has no knowledge of mwu string interface.  This way it
 * can be used with different control interfaces in the future.  Please do not
 * call mwu functions or implement mwu commands directly in the wifidirect
 * module.
 */

#ifndef __MWU_WIFIDIR__
#define __MWU_WIFIDIR__

#include "mwu_internal.h"
#include "wifidir.h"

/* wifidir_mwu_launch: launch the wifidir mwu interface
 *
 * returns: whatever return code comes back from
 * mwu_internal_register_module().
 */
enum mwu_error wifidir_mwu_launch(void);

/* wifidir_mwu_halt: halt the mwu interface for wifidirect
 */
void wifidir_mwu_halt(void);

/* wifidir_mwu_event_cb: mwu handler for wifidirect events
 *
 * This callback should be passed in the module data structure passed to
 * wifidir_init().
 */
void wifidir_mwu_event_cb(struct event *event, void *priv);

#endif
