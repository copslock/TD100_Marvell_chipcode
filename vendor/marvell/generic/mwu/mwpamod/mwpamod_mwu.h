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

/* mwu module "mwpamod"
 *
 * The mwpamod module provides wpa capabilities for AP and STA.  This header
 * file defines the command/response/event message interface offered by the
 * mwpamod module via mwu.  For the internal API used to access mwpamod, see
 * mwpamod.h.  Developers creating control programs in C or C++ that use the
 * mwu interface may wish to include mwpamod.h to define various constants that
 * appear in the messages.
 *
 * All messages sent to the mwpamod must have iface=<iface> as their second
 * key-value pair, and cmd=<cmd> and as their third.  <iface> must be the valid
 * network interface string on which wpamod will operate.  <cmd> must be one of
 * the commands specified below under "MWPAMOD COMMANDS".  The response
 * messages to these commands varies depending on the <cmd>.  All messages
 * emitted by the mwpamod will have module=mwpamod as their first key-value
 * pair, iface=<iface> as their second key-value pair, and event=<event> as
 * their third.  Events are described below under "MWPAMOD EVENTS".
 *
 * See the OVERVIEW documentation in mwpamod.h for more details regarding the
 * use of mwpamod.
 *
 * MWPAMOD COMMANDS:
 *
 * sta_init: initialize an instance of mwpamod as an STA on the specified
 * interface.  No other STA commands can be issued on the specified interface
 * until this one is issued.  This command does not require any key-value pairs
 * beyond the three required ones.
 *
 * The command response for the sta_init command contains a single
 * status=<status> key-value pair.  <status> is a decimal number representing
 * the result of the command.  See the documentation for the mwpamod_sta_init()
 * function in mwpamod.h for details on the meaning of the status.
 *
 * sta_connect: connect to a network.  This command must be accompanied by the
 * following key value pairs in addition to the three required ones:
 *
 *     ssid=<ssid>: The ssid of the network to connect to.
 *
 *     key=<key>.  If the network to connect to is an open network, <key>
 *     should be "".  If the network to connect to is WPA2, key can either be a
 *     64-byte (i.e., 128-char) hexadecimal null-terminated pre-shared key
 *     (psk), or a null-terminated ascii passphrase of no more than 63
 *     characters.
 *
 * The command response for this command contains a single status=<status>
 * key-value pair.  <status> is a decimal number representing the result of the
 * command.  See the documentation for the mwpamod_sta_connect() function in
 * mwpamod.h for details on the meaning of the status and other details.
 *
 * sta_disconnect: disconnect the STA on the specified interface from its
 * network.  This command does not require any additional key-value pairs and
 * does not return a command response.  See the documentation for
 * mwpamod_sta_disconnect() in mwpamod.h for more detail.
 *
 * sta_deinit: deinitialize the STA on the specified interface.  This command
 * does not require any additional key-value pairs and does not return a
 * command response.  See the documentation for mwpamod_sta_deinit() in
 * mwpamod.h for more detail.
 *
 * ap_init: initialize an instance of mwpamod as an AP on the specified
 * interface.  No other AP commands can be issued on the specified interface
 * until this one is issued.  In addition to the three required key-value pairs,
 * this command requires:
 *
 *     ssid=<ssid>: The ssid of the network for the AP to use.
 *
 *     key=<key>.  If the network managed by the AP is to be an open network,
 *     <key> should be "".  If the network is to be WPA2, key can either be a
 *     64-byte (i.e., 128-char) hexadecimal null-terminated pre-shared key
 *     (psk), or a null-terminated ascii passphrase of no more than 63
 *     characters.
 *
 * The command response for the ap_init command contains a single
 * status=<status> key-value pair.  <status> is a decimal number representing
 * the result of the command.  See the documentation for the mwpamod_ap_init()
 * function in mwpamod.h for details on the meaning of the status.
 *
 * ap_start: Start the AP.  No additional key-value pairs are required for this
 * command.  The command response for the ap_start command contains a single
 * status=<status> key-value pair.  <status> is a decimal number representing
 * the result of the command.  See the documentation for the mwpamod_ap_start()
 * function in mwpamod.h for details on the meaning of the status.
 *
 * ap_stop: stop the AP on the specified interface.  This command does not
 * require any additional key-value pairs and does not return a command
 * response.  See the documentation for mwpamod_ap_stop() in mwpamod.h for more
 * detail.
 *
 * ap_disconnect: disconnect a specified STA from the network.  This command
 * requires one additional key-value pairs:
 *
 *     mac=<mac>: mac address of the STA to disconnect formatted as upper case
 *     hexadecimal like so: 00:11:33:55:77:AA.
 *
 * This command does not return a command response.  See the documentation for
 * mwpamod_ap_disconnect() in mwpamod.h for more detail.
 *
 * ap_deinit: deinitialize the AP on the specified interface.  This command
 * does not require any additional key-value pairs and does not return a
 * command response.  See the documentation for mwpamod_ap_deinit() in
 * mwpamod.h for more detail.
 *
 * MWPAMOD EVENTS:
 *
 * sta_connect: The results from a successful sta_connect command.  This event
 * contains one additional key-value pair:
 *
 *     status=<status>: Set to the decimal value of MWPAMOD_ERR_SUCCESS if the
 *     connection succeeded.  See the documentation for the
 *     MWPAMOD_EVENT_STA_CONNECT in mwpamod.h for details on other values of
 *     <status>.
 *
 * sta_link_lost: The link has been lost.  The network is no longer available.
 * This event has the usual status=<status> key-value pair.  See the
 * documentation for the MWPAMOD_EVENT_LINK_LOST event in mwpamod.h for more
 * detail.
 *
 * ap_connect: This occurs when a module is configured as an AP and an STA
 * attempts to connect.  It is accompanied by the following key-value pair:
 *
 *     mac=<mac>: The mac address of the STA that made the attempt formatted in
 *     upper-case hex.
 *
 *     status=<status>: Code indicating whether or not the attempt was
 *     successful.  See mwpamod.h for more detail on the meaning of status.
 *
 * ap_disconnect: This occurs when a module is configured as an AP and an STA
 * leaves the network.  This event has a single additional key-value pair: the
 * mac of the disconnected STA.
 */

#ifndef __MWU_MWPAMOD__
#define __MWU_MWPAMOD__

#include "mwu_internal.h"
#include "mwpamod.h"

/* mwpamod_mwu_launch: launch the mwpamod mwu interface
 *
 * returns: whatever return code comes back from
 * mwu_internal_register_module().
 */
enum mwu_error mwpamod_mwu_launch(void);

/* mwpamod_mwu_halt: halt the mwu interface for mwpamod
 */
void mwpamod_mwu_halt(void);

/* TODO: This callback should be specified by mwpamod_mwu.h in response to the
 * various init commands, not at launch time as we do now.  But the init
 * commands are not implemented, so for now we need this function available at
 * launch time.
 */
void mwpamod_mwu_event_cb(struct event *event, void *priv);

#endif
