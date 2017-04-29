/*
 * Big number math
 *
 * Copyright (c) 2006, Jouni Malinen <j@w1.fi>
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
 *   Marvell  05/21/09: add Marvell file header
 */

#ifndef BIGNUM_H
#define BIGNUM_H

struct bignum;

struct bignum *bignum_init(void);
void bignum_deinit(struct bignum *n);
size_t bignum_get_unsigned_bin_len(struct bignum *n);
int bignum_get_unsigned_bin(const struct bignum *n, u8 * buf, size_t * len);
int bignum_set_unsigned_bin(struct bignum *n, const u8 * buf, size_t len);
int bignum_cmp(const struct bignum *a, const struct bignum *b);
int bignum_cmp_d(const struct bignum *a, unsigned long b);
int bignum_add(const struct bignum *a, const struct bignum *b,
               struct bignum *c);
int bignum_sub(const struct bignum *a, const struct bignum *b,
               struct bignum *c);
int bignum_mul(const struct bignum *a, const struct bignum *b,
               struct bignum *c);
int bignum_mulmod(const struct bignum *a, const struct bignum *b,
                  const struct bignum *c, struct bignum *d);
int bignum_exptmod(const struct bignum *a, const struct bignum *b,
                   const struct bignum *c, struct bignum *d);

#endif /* BIGNUM_H */
