/*
 * MD5 hash implementation and interface functions
 * Copyright (c) 2003-2005, Jouni Malinen <jkmaline@cc.hut.fi>
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
 */

#include "includes.h"

#include "common.h"
#include "md5.h"
#include "crypto.h"

/**
 * hmac_md5_vector - HMAC-MD5 over data vector (RFC 2104)
 * @key: Key for HMAC operations
 * @key_len: Length of the key in bytes
 * @num_elem: Number of elements in the data vector
 * @addr: Pointers to the data areas
 * @len: Lengths of the data blocks
 * @mac: Buffer for the hash (16 bytes)
 */
void
hmac_md5_vector(const u8 * key, size_t key_len, size_t num_elem,
                const u8 * addr[], const size_t * len, u8 * mac)
{
    u8 k_pad[64];               /* padding - key XORd with ipad/opad */
    u8 tk[16];
    const u8 *_addr[6];
    size_t i, _len[6];

    if (num_elem > 5) {
        /*
         * Fixed limit on the number of fragments to avoid having to
         * allocate memory (which could fail).
         */
        return;
    }

    /* if key is longer than 64 bytes reset it to key = MD5(key) */
    if (key_len > 64) {
        md5_vector(1, &key, &key_len, tk);
        key = tk;
        key_len = 16;
    }

    /* the HMAC_MD5 transform looks like: MD5(K XOR opad, MD5(K XOR ipad,
       text)) where K is an n byte key ipad is the byte 0x36 repeated 64 times
       opad is the byte 0x5c repeated 64 times and text is the data being
       protected */

    /* start out by storing key in ipad */
    os_memset(k_pad, 0, sizeof(k_pad));
    os_memcpy(k_pad, key, key_len);

    /* XOR key with ipad values */
    for (i = 0; i < 64; i++)
        k_pad[i] ^= 0x36;

    /* perform inner MD5 */
    _addr[0] = k_pad;
    _len[0] = 64;
    for (i = 0; i < num_elem; i++) {
        _addr[i + 1] = addr[i];
        _len[i + 1] = len[i];
    }
    md5_vector(1 + num_elem, _addr, _len, mac);

    os_memset(k_pad, 0, sizeof(k_pad));
    os_memcpy(k_pad, key, key_len);
    /* XOR key with opad values */
    for (i = 0; i < 64; i++)
        k_pad[i] ^= 0x5c;

    /* perform outer MD5 */
    _addr[0] = k_pad;
    _len[0] = 64;
    _addr[1] = mac;
    _len[1] = MD5_MAC_LEN;
    md5_vector(2, _addr, _len, mac);
}

/**
 * hmac_md5 - HMAC-MD5 over data buffer (RFC 2104)
 * @key: Key for HMAC operations
 * @key_len: Length of the key in bytes
 * @data: Pointers to the data area
 * @data_len: Length of the data area
 * @mac: Buffer for the hash (16 bytes)
 */
void
hmac_md5(const u8 * key, size_t key_len, const u8 * data, size_t data_len,
         u8 * mac)
{
    hmac_md5_vector(key, key_len, 1, &data, &data_len, mac);
}
