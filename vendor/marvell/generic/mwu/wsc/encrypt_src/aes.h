/*
 * AES functions
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
 */

#ifndef AES_H
#define AES_H

void *aes_encrypt_init(const u8 * key, size_t len);
void aes_encrypt(void *ctx, const u8 * plain, u8 * crypt);
void aes_encrypt_deinit(void *ctx);
void *aes_decrypt_init(const u8 * key, size_t len);
void aes_decrypt(void *ctx, const u8 * crypt, u8 * plain);
void aes_decrypt_deinit(void *ctx);

#endif /* AES_H */
