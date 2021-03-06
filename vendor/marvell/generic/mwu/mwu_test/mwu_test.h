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

/* mwu module "test"
 *
 * The test module is meant as a model module to demonstrate the internal
 * module API for developers wishing to add wireless utilities to the suite.
 *
 * Any message sent via mwu_send_message() with "module=test" will be routed to
 * the test module and must have "cmd=<cmd>" as it's next key-value pair.  Any
 * message received via mwu_recv_message() with module=test is an event from
 * the test module and will have event=<event> as its second key-value pair.
 *
 * <cmd> can be one of the following:
 *
 * ping: This is simply a test command that the control program can use to
 * ensure that it is connected to mwu.  The test module always returns a single
 * key-value pair in the response message: status=<status>.  <status> is the
 * base-ten ascii representation of one of the status codes defined below.  If
 * <status> is TEST_SUCCESS, the user can expect a pong event (see below).
 *
 * <event> will be one of the following:
 *
 * pong: Somebody sent a message with "cmd=ping".  This is the response.
 */

#ifndef __MWU_TEST__
#define __MWU_TEST__

/* enum test_status
 *
 * TEST_SUCCESS: The operation was successful
 *
 * TEST_UNSUPPORTED: The command specified by the cmd key-value pair is not
 * supported.
 *
 * TEST_INVALID: The command message was missing required fields, or is
 * otherwise invalid.
 *
 * TEST_INTERNAL: An unexpected internal error occured (e.g., out-of-memory).
 * See log output for details.
 */
enum test_status {
    TEST_SUCCESS = 0,
    TEST_ERR_UNSUPPORTED,
    TEST_ERR_INVALID,
    TEST_ERR_INTERNAL
};

/* INTERNAL API */

/* test_init: initialize the test module */
int test_init(void);

/* test_deinit: tear down the test module */
void test_deinit(void);

#endif
