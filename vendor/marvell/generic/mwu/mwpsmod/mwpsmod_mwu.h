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

/* mwu module "mwpsmod"
 *
 * The mwpsmod module provides wps enrollee and registrar functionality.  This
 * header file defines the command/response/event message interface offered by
 * the mwpsmod module via mwu.  For the internal wps API used to actually
 * operate on the wps state machine, see mwpsmod.h.  Developers
 * creating control programs in C or C++ that use the mwu interface may wish to
 * include mwpsmod.h to define various constants that appear in the
 * messages.
 *
 * All messages sent to the mwpsmod must have iface=<iface> as their second
 * key-value pair and cmd=<cmd> as their third.  <iface> must be a suitable
 * system interface.  <cmd> must be one of the commands specified below under
 * "mwpsmod COMMANDS".  The response messages to these commands varies
 * depending on the <cmd>.  All messages emitted by the mwpsmod will have
 * module=mwpsmod as their first key-value pair, iface=<iface> as their second,
 * and event=<event> as their third key-value pair.  Events are described below
 * under "mwpsmod EVENTS".
 *
 * See the OVERVIEW documentation in mwpsmod.h for more details regarding the
 * use of mwpsmod.
 *
 * mwpsmod COMMANDS:
 *
 * enrollee_init: initialize mwpsmod as an enrollee.  In addition to the three
 * required key-value pairs, this command must contain the following key-value
 * pairs in order:
 *
 *     device_name=<name>: The null-terminated ascii name to be used for wps.
 *     This should be no longer than MAX_DEVICE_NAME_LEN.
 *
 *     model_name=<name>: The null-terminated ascii model name to be used for
 *     wps.  This should be no longer than MAX_DEVICE_NAME_LEN.
 *
 *     model_number=<number>: The null-terminated ascii module number to be
 *     used for wps.  This should be no longer than MAX_DEVICE_NAME_LEN.
 *
 *     methods=<methods>: The supported WPS configuration methods.  This is an
 *     upper-case hexadecimal number that is to be interpreted as a bitmask.
 *     The bitmap is the logical OR of all of the supported methods.  See enum
 *     module_config_method for the list of methods.
 *
 * The command response for this command contains a single status=<status>
 * key-value pair.  <status> is a decimal number representing the result of the
 * command.  See the documentation for the mwpsmod_enrollee_init() function in
 * mwpsmod.h for details on the meaning of the status and other
 * details.
 *
 * enrollee_deinit: deinitialize the enrollee.  This command does not require
 * any extra key-value pairs.  In response to this command, all wps activity
 * will be halted and the wps module will return to its uninitialized state.
 * This command does not send a response message.  See the documentation for
 * the mwpsmod_enrollee_deinit() function for additional detail on this
 * command.
 *
 * enrollee_start: start wps enrollee.  This command requires two additional
 * key-value pairs:
 *
 *     mac=<mac>: <mac> is the mac address of the registrar with whom wps
 *     should be initiated formatted as hexadecimal digits like so:
 *     00:11:22:33:44:55.
 *
 *     pin=<pin>: The pin to be used for wps encryption.  <pin> must contain
 *     exactly 8 decimal digits represented as ascii characters.  If PBC
 *     authentication is desired, <pin> should be "".
 *
 * The means by which the caller of this API learns the mac address of the
 * registrar is not specified by this module.  The command response contains
 * the usual status=<status> key-value pair.  See the documentation for the
 * mwpsmod_enrollee_start() function in mwpsmod.h for details on the
 * meaning of the status and other details.
 *
 * enrollee_stop: stop the enrollee.  This command does not require any extra
 * key-value pairs.  In response to this command, all wps activity will be
 * halted such that enrollee_start can be sent again.  This command does not
 * send a response message.  See the documentation for the
 * mwpsmod_enrollee_stop() function for additional detail on this command.
 *
 * registrar_init: initialize mwpsmod as a registrar.  In addition to the three
 * required key-value pairs, this command must contain the following key-value
 * pairs in order:
 *
 *     device_name=<name>: The null-terminated ascii name to be used for wps.
 *     This should be no longer than MAX_DEVICE_NAME_LEN.
 *
 *     model_name=<name>: The null-terminated ascii model name to be used for
 *     wps.  This should be no longer than MAX_DEVICE_NAME_LEN.
 *
 *     model_number=<number>: The null-terminated ascii module number to be
 *     used for wps.  This should be no longer than MAX_DEVICE_NAME_LEN.
 *
 *     methods=<methods>: The supported WPS configuration methods.  This is an
 *     upper-case hexadecimal number that is to be interpreted as a bitmask.
 *     The bitmap is the logical OR of all of the supported methods.  See enum
 *     module_config_method for the list of methods.
 *
 *     ssid=<ssid>: The ssid of the network for which credentials are being
 *     shared.  It must be no longer than MAX_SSID_LEN.
 *
 *     auth=<auth>: The authentication type supported by the network for which
 *     credentials are being shared.  This is a 4-character upper-case
 *     hexadecimal number.  It is the logical OR of any number of "enum
 *     mwpsmod_auth"s specified in mwpsmod.h.
 *
 *     encrypt=<encrypt>: The encryption types supported by the network for
 *     which credentials are being shared.  <encrypt> is a 4-character
 *     upper-case hexadecimal number.  It is the logical OR of any number of
 *     "enum mwpsmod_encrypt"s specified in mwpsmod.h.
 *
 *     key=<key>: If auth is MWPSMOD_AUTH_OPEN and encrypt is
 *     MWPSMOD_ENCRYPT_NONE, <key> is ignored and should be "".  Otherwise,
 *     <key> is either the 64-character null-terminated hexadecimal psk or an
 *     ascii passphrase between 8 and 63 characters.
 *
 * The command response for the init command contains a single status=<status>
 * key-value pair.  <status> is a decimal number representing the result of the
 * command.  See the documentation for the mwpsmod_registrar_init() function in
 * mwpsmod.h for details on the meaning of the status and other
 * details.
 *
 * registrar_deinit: deinitialize the registrar.  This command does not require
 * any extra key-value pairs.  In response to this command, all wps activity
 * will be halted and mwpsmod will return to its uninitialized state.  This
 * command does not send a response message.  See the documentation for the
 * mwpsmod_registrar_deinit() function for additional detail on this command.
 *
 * registrar_start: start wps registrar.  This command does not require any
 * additional key-value pairs.  The command response contains the usual
 * status=<status> key-value pair.  See the documentation for the
 * mwpsmod_registrar_start() function in mwpsmod.h for details on the
 * meaning of the status and other details.
 *
 * registrar_stop: stop the wps registrar.  This command does not require any
 * additional key-value pairs and does not send a command response.  See the
 * documentation for the mwpsmod_registrar_stop() function in mwpsmod.h for
 * more detail.
 *
 * registrar_set_pin: set the registrar pin to be used for wps.  This command
 * requires the pin=<pin> key-value pair as described along with the
 * enrollee_start command above.  The command response contains the usual
 * status=<status> key-value pair.  See the documentation for the
 * mwpsmod_registrar_set_pin() function in mwpsmod.h for details on the
 * meaning of the status.
 *
 * get_random_pin: get a random 8-digit pin suitable for wps.  This command
 * does not require any additional key-value pairs.  The response message for
 * this command contains a single key-value pair:
 *
 *     pin=<pin>: the randomly generated null-terminated pin.  <pin> will
 *     contain 8 decimal digits represented as ascii characters.
 *
 * See the documentation for the mwpsmod_random_pin() function for additional
 * details regarding this command.
 *
 * mwpsmod EVENTS
 *
 * enrollee_done: wps negotiation that was initiated by a successful
 * enrollee_start command has completed.  This event contains at least one
 * key-value pair in addition to the required module and event key-value pairs:
 *
 *     status=<status>: Set to the decimal value of MWPSMOD_ERR_SUCCESS if the
 *     negotiation succeeded.  See the documentation for the
 *     MWPSMOD_EVENT_ENROLLEE_DONE in mwpsmod.h for details on other
 *     values of <status>.
 *
 * If <status> is MWPSMOD_ERR_SUCCESS, the following additional key-value pairs
 * will follow:
 *
 *     ssid=<ssid>: As described in the registrar_init command above.
 *
 *     auth=<auth>: As described in the registrar_init command above.
 *
 *     encrypt=<encrypt>: As described in the registrar_init command above.
 *
 *     key=<key>: As described in the registrar_init command above.
 *
 * If <status> is MWPSMOD_ERR_NOTREADY, the following additional key-value
 * pairs will follow:
 *
 *     device_name=<name>: As described in the registrar_init command above.
 *
 *     model_name=<name>: As described in the registrar_init command above.
 *
 *     model_number=<number>: As described in the registrar_init command above.
 *
 *     methods=<methods>: As described in the registrar_init command above.
 *
 * registrar_done: registration has completed with the enrollee specified by
 * the last registrar_started event.  This event is accompanied by at least
 * one key-value pair:
 *
 *     status=<status>: MWPSMOD_ERR_SUCCESS if the enrollee successfully
 *     registered.  See the documentation for the MWPSMOD_EVENT_REGISTRAR_DONE
 *     in mwpsmod.h for details on other values of <status>.
 *
 * If <status> is MWPSMOD_ERR_SUCCESS or MWPSMOD_ERR_NOTREADY, the following
 * additional key-value pairs will follow:
 *
 *     mac=<mac>: mac address of the enrollee
 *
 *     device_name=<name>: As described in the registrar_init command above.
 *
 *     model_name=<name>: As described in the registrar_init command above.
 *
 *     model_number=<number>: As described in the registrar_init command above.
 *
 *     methods=<methods>: As described in the registrar_init command above.
 */

#ifndef __MWU_WPS__
#define __MWU_WPS__

#include "mwu_internal.h"
#include "mwpsmod.h"

/* wps_mwu_launch: launch the mwpsmod mwu interface
 *
 * returns: whatever return code comes back from
 * mwu_internal_register_module().
 */
enum mwu_error mwpsmod_mwu_launch(void);

/* wps_mwu_halt: halt the mwu interface for wps
 */
void mwpsmod_mwu_halt(void);

/* wps_mwu_event_cb: mwu handler for mwpsmod events
 *
 * This callback should be passed to wps_launch() at boot time.
 */
void mwpsmod_mwu_event_cb(struct event *event, void *priv);

#endif
