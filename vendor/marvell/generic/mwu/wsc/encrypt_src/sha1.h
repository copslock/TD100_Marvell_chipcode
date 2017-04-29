/*
 * SHA1 hash implementation and interface functions
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

#ifndef SHA1_H
#define SHA1_H

#define SHA1_MAC_LEN 20

void hmac_sha1_vector(const u8 * key, size_t key_len, size_t num_elem,
                      const u8 * addr[], const size_t * len, u8 * mac);
void hmac_sha1(const u8 * key, size_t key_len, const u8 * data, size_t data_len,
               u8 * mac);
void sha1_prf(const u8 * key, size_t key_len, const char *label,
              const u8 * data, size_t data_len, u8 * buf, size_t buf_len);
void sha1_t_prf(const u8 * key, size_t key_len, const char *label,
                const u8 * seed, size_t seed_len, u8 * buf, size_t buf_len);
int tls_prf(const u8 * secret, size_t secret_len, const char *label,
            const u8 * seed, size_t seed_len, u8 * out, size_t outlen);
void pbkdf2_sha1(const char *passphrase, const char *ssid, size_t ssid_len,
                 int iterations, u8 * buf, size_t buflen);

#endif /* SHA1_H */
