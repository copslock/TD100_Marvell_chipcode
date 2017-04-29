/** @file crypto.c
 *
 * Copyright (c) 2004-2009, Jouni Malinen <j@w1.fi>
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

#include <openssl/opensslv.h>
#include <openssl/err.h>
#include <openssl/dh.h>

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "encrypt.h"

void* mrvl_dh_setup_key(u8 *public_key,  u32 public_len,
                       u8 *private_key, u32 private_len,
                       DH_PG_PARAMS *dh_params)
{
    DH *dh = NULL;
    u8 *priv_key = NULL;
    u8 *pub_key = NULL;
    u32 privkey_len;
    u32 publkey_len;
    dh = DH_new();

    if (dh == NULL)
        return NULL;

    dh->p = BN_bin2bn(dh_params->prime, dh_params->primeLen, NULL);
    if (dh->p == NULL)
        goto error;

    dh->g = BN_bin2bn(dh_params->generator, dh_params->generatorLen, NULL);
    if (dh->g == NULL)
        goto error;

    if (DH_generate_key(dh) != 1)
        goto error;

    /* successfully generate keys */
    publkey_len = BN_num_bytes(dh->pub_key);
    pub_key = malloc(publkey_len);
    if (pub_key == NULL)
        goto error;

    privkey_len = BN_num_bytes(dh->priv_key);
    priv_key = malloc(privkey_len);
    if (priv_key == NULL) {
        free(pub_key);
        goto error;
    }

    BN_bn2bin(dh->pub_key, pub_key);
    BN_bn2bin(dh->priv_key, priv_key);
    memcpy(public_key, pub_key, public_len);
    memcpy(private_key, priv_key, private_len);

    free(pub_key);
    free(priv_key);

    return dh;
error:
    DH_free(dh);


    return NULL;
}

int mrvl_dh_compute_key(void *dh,        u8 *shared_key, u32 shared_len,
                        u8 *public_key,  u32 public_len,
                        u8 *private_key, u32 private_len,
                        DH_PG_PARAMS *dh_params)
{
    BIGNUM  *pub_key = NULL;
    u8* key = NULL;
    int key_len = 0;
    u32 ret = 0;

    /* convert the public key binary to the BIGNUM */
    pub_key = BN_bin2bn(public_key, public_len, NULL);

    if (pub_key == NULL)
        return ret;

    /* get the size of module p */
    key_len = DH_size(dh);
    key = malloc(key_len);
    if (key == NULL)
        goto exit0;

    key_len = DH_compute_key(key, pub_key, dh);
    if (key_len < 0)
        goto exit1;

    if (key_len >= shared_len) {
        memcpy(shared_key, key, shared_len);
        ret = shared_len;
        goto exit1;
    }

    /* zero padding */
    memcpy(shared_key, key, key_len);
    memset(shared_key + key_len, 0, shared_len - key_len);

    ret = key_len;

exit1:
    free(key);
exit0:
    BN_free(pub_key);
    return ret;
}


void mrvl_dh_free(void *dh_context)
{
    DH *dh = (DH *)dh_context;
    if (dh)
        DH_free(dh);
}
