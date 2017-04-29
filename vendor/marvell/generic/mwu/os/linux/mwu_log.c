/** @file mwu_Log.c
 *  @brief This file contains functions for debugging print.
 *
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <ctype.h>
#include <time.h>

#include "mwu_log.h"
#include "mwu_timer.h"

/********************************************************
        Local Variables
********************************************************/
static int wps_debug_timestamp = 0;

/********************************************************
        Global Variables
********************************************************/
int mwu_debug_level = DEFAULT_MSG;       /* declared in mwu_log.h */

/********************************************************
        Local Functions
********************************************************/

static void
mwu_debug_print_timestamp(void)
{
    struct mwu_timeval tv;
    char buf[16];

    if (!wps_debug_timestamp)
        return;

    mwu_get_time(&tv);
    if (strftime(buf, sizeof(buf), "%b %d %H:%M:%S",
                 localtime((const time_t *) &tv.sec)) <= 0) {
        snprintf(buf, sizeof(buf), "%u", (int) tv.sec);
    }
    printf("%s.%06u: ", buf, (unsigned int) tv.usec);
}


/********************************************************
        Global Functions
********************************************************/

void mwu_set_debug_level(int level)
{
    mwu_debug_level = level;
}

#ifndef ANDROID
void
mwu_printf(int level, char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);

    if (level & mwu_debug_level) {
        mwu_debug_print_timestamp();
        vprintf(fmt, ap);
        printf("\n");
    }
    va_end(ap);
}
#endif

void
mwu_hexdump(int level, const char *title, const unsigned char *buf, size_t len)
{
    int i, j;
    unsigned char *offset;

    /* Android logcat adds newlines to everything.  So print hex to a buffer
     * before dumping it.
     */
    char *ptr, line[3*16 + 1];

    if (!(level & mwu_debug_level))
        return;
    mwu_debug_print_timestamp();

    offset = (unsigned char *) buf;
    mwu_printf(level, "%s - hexdump(len=%lu):\n", title, (unsigned long) len);

    for (i = 0; i < len / 16; i++) {
        ptr = line;
        memset(line, 0, sizeof(line));
        for (j = 0; j < 16; j++)
            ptr += sprintf(ptr, "%02x ", offset[j]);
        mwu_printf(level, "%s\n", line);
        offset += 16;
    }
    i = len % 16;
    ptr = line;
    memset(line, 0, sizeof(line));
    for (j = 0; j < i; j++)
        ptr += sprintf(ptr, "%02x ", offset[j]);
    mwu_printf(level, "%s\n", line);
}
