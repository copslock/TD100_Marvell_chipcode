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
/* util.h: common utilities used by mwu modules
 *
 */
#ifndef __UTIL_H__
#define __UTIL_H__

/* This is defined in wps_util.h, but we need an upper case one.  So we add it
 * here.  We also add UTIL_MAC2STR for completeness.
 */
#define UTIL_MACSTR "%02hhX:%02hhX:%02hhX:%02hhX:%02hhX:%02hhX"
#define UTIL_MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define UTIL_MAC2SCAN(a) &(a)[0], &(a)[1], &(a)[2], &(a)[3], &(a)[4], &(a)[5]

/* convenience macro for couting the elements in a SLIST.  Perhaps an inline
 * function would be better here, but then we have to find a way to pass the
 * type of cursor.
 */
#define SLIST_COUNT(cursor, list, list_item, count) do { \
    count = 0;                                           \
    SLIST_FOREACH(cursor, list, list_item) {             \
        count++;                                         \
    }                                                    \
} while(0)

#endif
