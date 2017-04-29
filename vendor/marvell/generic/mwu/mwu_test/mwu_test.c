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
/* mwu_test: test module for mwu control interface */
#include <stdlib.h>
#include "mwu.h"

#define UTIL_LOG_TAG "TEST"
#include "util.h"

#include "mwu_internal.h"
#include "mwu_log.h"
#include "mwu_test.h"

/* ping is our longest cmd at this time of writing.  But leave some room to grow */
#define TEST_MAX_CMD 16

/* status=4294967295\n\0 is our largest cmd response. */
#define TEST_MAX_RESP 20

/* module=test\nevent=pong\n\0 is our largest event at this time of writing.
 * But leave some room to grow.
 */
#define TEST_MAX_EVENT 64

enum mwu_error test_handle_mwu(struct mwu_msg *msg, struct mwu_msg **resp)
{
    int ret;
    char module[5];
    char cmd[TEST_MAX_CMD];
    struct mwu_msg *event = NULL;

    *resp = NULL;

    INFO("Got message:\n");
    INFO("%s", msg->data);

    /* parse the message */
    ret = sscanf(msg->data, "module=%s\ncmd=%s\n", module, cmd);
    if (ret != 2) {
        ERR("Failed to parse cmd message: ret = %d.\n", ret);
        ALLOC_MSG_OR_FAIL(*resp, TEST_MAX_RESP, "status=%d\n",
                          TEST_ERR_INVALID);
        return MWU_ERR_SUCCESS;
    }

    /* process command */
    if (strcmp(cmd, "ping") == 0) {
        /* send the pong event and respond with the status */
        ALLOC_MSG_OR_FAIL(event, TEST_MAX_EVENT, "module=test\nevent=pong\n");
        ret = mwu_internal_send(event);
        free(event);
        event = NULL;
        if (ret == MWU_ERR_SUCCESS) {
            ALLOC_MSG_OR_FAIL(*resp, TEST_MAX_RESP, "status=%d\n",
                              TEST_SUCCESS);
        } else {
            ALLOC_MSG_OR_FAIL(*resp, TEST_MAX_RESP, "status=%d\n",
                              TEST_ERR_INTERNAL);
        }
    } else {
        ERR("Got unknown command: %s\n", cmd);
        ALLOC_MSG_OR_FAIL(*resp, TEST_MAX_RESP, "status=%d\n",
                          TEST_ERR_UNSUPPORTED);
    }
    return MWU_ERR_SUCCESS;

fail:
    if (*resp != NULL)
        free(*resp);
    return ret;
}

static struct mwu_module test_mod = {
    .name = "test",
    .msg_cb = test_handle_mwu,
    .msg_free = NULL,
};

int test_init(void)
{
    int ret;

    ret = mwu_internal_register_module(&test_mod);
    if (ret != MWU_ERR_SUCCESS) {
        ERR("Failed to register with mwu\n");
        return TEST_ERR_INTERNAL;
    }
    return TEST_SUCCESS;
}

void test_deinit(void)
{
    mwu_internal_unregister_module(&test_mod);
    return;
}
