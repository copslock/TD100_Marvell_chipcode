/*
 * AES-based functions
 *
 * - AES Key Wrap Algorithm (128-bit KEK) (RFC3394)
 * - One-Key CBC MAC (OMAC1) hash with AES-128
 * - AES-128 CTR mode encryption
 * - AES-128 EAX mode encryption/decryption
 * - AES-128 CBC
 *
 * Copyright (C) 2003-2005, Jouni Malinen <jkmaline@cc.hut.fi>
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
 */

#ifndef AES_WRAP_H
#define AES_WRAP_H

int aes_wrap(const u8 * kek, int n, const u8 * plain, u8 * cipher);
int aes_unwrap(const u8 * kek, int n, const u8 * cipher, u8 * plain);
int omac1_aes_128(const u8 * key, const u8 * data, size_t data_len, u8 * mac);
int aes_128_encrypt_block(const u8 * key, const u8 * in, u8 * out);
int aes_128_ctr_encrypt(const u8 * key, const u8 * nonce,
                        u8 * data, size_t data_len);
int aes_128_eax_encrypt(const u8 * key, const u8 * nonce, size_t nonce_len,
                        const u8 * hdr, size_t hdr_len,
                        u8 * data, size_t data_len, u8 * tag);
int aes_128_eax_decrypt(const u8 * key, const u8 * nonce, size_t nonce_len,
                        const u8 * hdr, size_t hdr_len,
                        u8 * data, size_t data_len, const u8 * tag);
int aes_128_cbc_encrypt(const u8 * key, const u8 * iv, u8 * data,
                        size_t data_len);
int aes_128_cbc_decrypt(const u8 * key, const u8 * iv, u8 * data,
                        size_t data_len);

#endif /* AES_WRAP_H */
