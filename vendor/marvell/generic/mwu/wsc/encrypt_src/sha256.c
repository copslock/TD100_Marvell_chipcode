/*
 * SHA-256 hash implementation and interface functions
 *
 * Copyright (C) 2003-2006, Jouni Malinen <jkmaline@cc.hut.fi>
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

/*
 * Change log:
 *   Marvell  09/28/07: add Marvell file header
 *   Marvell  10/11/07: comment out unused function sha256_prf
 */

#include "includes.h"

#include "common.h"
#include "sha256.h"
#include "crypto.h"

/**
 * hmac_sha256_vector - HMAC-SHA256 over data vector (RFC 2104)
 * @key: Key for HMAC operations
 * @key_len: Length of the key in bytes
 * @num_elem: Number of elements in the data vector
 * @addr: Pointers to the data areas
 * @len: Lengths of the data blocks
 * @mac: Buffer for the hash (32 bytes)
 */
void
hmac_sha256_vector(const u8 * key, size_t key_len, size_t num_elem,
                   const u8 * addr[], const size_t * len, u8 * mac)
{
    unsigned char k_pad[64];    /* padding - key XORd with ipad/opad */
    unsigned char tk[32];
    const u8 *_addr[6];
    size_t _len[6], i;

    if (num_elem > 5) {
        /*
         * Fixed limit on the number of fragments to avoid having to
         * allocate memory (which could fail).
         */
        return;
    }

    /* if key is longer than 64 bytes reset it to key = SHA256(key) */
    if (key_len > 64) {
        sha256_vector(1, &key, &key_len, tk);
        key = tk;
        key_len = 32;
    }

    /* the HMAC_SHA256 transform looks like: SHA256(K XOR opad, SHA256(K XOR
       ipad, text)) where K is an n byte key ipad is the byte 0x36 repeated 64
       times opad is the byte 0x5c repeated 64 times and text is the data being
       protected */

    /* start out by storing key in ipad */
    memset(k_pad, 0, sizeof(k_pad));
    memcpy(k_pad, key, key_len);
    /* XOR key with ipad values */
    for (i = 0; i < 64; i++)
        k_pad[i] ^= 0x36;

    /* perform inner SHA256 */
    _addr[0] = k_pad;
    _len[0] = 64;
    for (i = 0; i < num_elem; i++) {
        _addr[i + 1] = addr[i];
        _len[i + 1] = len[i];
    }
    sha256_vector(1 + num_elem, _addr, _len, mac);

    memset(k_pad, 0, sizeof(k_pad));
    memcpy(k_pad, key, key_len);
    /* XOR key with opad values */
    for (i = 0; i < 64; i++)
        k_pad[i] ^= 0x5c;

    /* perform outer SHA256 */
    _addr[0] = k_pad;
    _len[0] = 64;
    _addr[1] = mac;
    _len[1] = SHA256_MAC_LEN;
    sha256_vector(2, _addr, _len, mac);
}

/**
 * hmac_sha256 - HMAC-SHA256 over data buffer (RFC 2104)
 * @key: Key for HMAC operations
 * @key_len: Length of the key in bytes
 * @data: Pointers to the data area
 * @data_len: Length of the data area
 * @mac: Buffer for the hash (20 bytes)
 */
void
hmac_sha256(const u8 * key, size_t key_len, const u8 * data,
            size_t data_len, u8 * mac)
{
    hmac_sha256_vector(key, key_len, 1, &data, &data_len, mac);
}

#if 0
/**
 * sha256_prf - SHA256-based Pseudo-Random Function (IEEE 802.11r, 8.5A.3)
 * @key: Key for PRF
 * @key_len: Length of the key in bytes
 * @label: A unique label for each purpose of the PRF
 * @data: Extra data to bind into the key
 * @data_len: Length of the data
 * @buf: Buffer for the generated pseudo-random key
 * @buf_len: Number of bytes of key to generate
 *
 * This function is used to derive new, cryptographically separate keys from a
 * given key.
 */
void
sha256_prf(const u8 * key, size_t key_len, const char *label,
           const u8 * data, size_t data_len, u8 * buf, size_t buf_len)
{
    u16 counter = 0;
    size_t pos, plen;
    u8 hash[SHA256_MAC_LEN];
    const u8 *addr[3];
    size_t len[3];
    u8 counter_le[2];

    addr[0] = counter_le;
    len[0] = 2;
    addr[1] = (u8 *) label;
    len[1] = strlen(label) + 1;
    addr[2] = data;
    len[2] = data_len;

    pos = 0;
    while (pos < buf_len) {
        plen = buf_len - pos;
        WPA_PUT_LE16(counter_le, counter);
        if (plen >= SHA256_MAC_LEN) {
            hmac_sha256_vector(key, key_len, 3, addr, len, &buf[pos]);
            pos += SHA256_MAC_LEN;
        } else {
            hmac_sha256_vector(key, key_len, 3, addr, len, hash);
            memcpy(&buf[pos], hash, plen);
            break;
        }
        counter++;
    }
}
#endif
