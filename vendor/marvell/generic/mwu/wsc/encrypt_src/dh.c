/* @file  dh.c
 * @brief This file contains function for Diffie-Hellman Key
 *          which utilize the functions of modular exponentiation
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

#include "includes.h"
#include "common.h"
#include "crypto.h"
#include "dh.h"
#include "mwu_timer.h"

/********************************************************
        Local Functions
********************************************************/
static int
generate_random_bytes(u8 * block, u32 block_len)
{
    u32 available = 0;
    struct mwu_timeval tv;
    u32 ut;

    ENTER();

    mwu_get_time(&tv);
    ut = (u32) tv.sec * 1000000 + (u32) tv.usec;
    srand(ut);

    while (block_len > available)
        block[available++] = rand() % 0x100;

    LEAVE();
    return (0);
}

/********************************************************
        Global Functions
********************************************************/
/**
 *  @brief  Sets up Diffie-Hellman key agreement.
 *
 *  @param public_key       Pointer to public key generated
 *  @param public_len       Length of public key
 *  @param private_key      Pointer to private key generated randomly
 *  @param private_len      Length of private key
 *  @param dh_params        Parameters for DH algorithm
 *  @return                 0 on success, -1 on failure
 */
int
setup_dh_agreement(u8 * public_key,
                   u32 public_len,
                   u8 * private_key, u32 private_len, DH_PG_PARAMS * dh_params)
{
    size_t generate_len;

    ENTER();

    if (generate_random_bytes(private_key, private_len) != 0) {
        LEAVE();
        return (-1);
    }

    generate_len = dh_params->primeLen;
    if (crypto_mod_exp(dh_params->generator, dh_params->generatorLen,
                       private_key, private_len,
                       dh_params->prime, dh_params->primeLen,
                       public_key, &generate_len) < 0) {
        printf("setup_dh_agreement: crypto_mod_exp failed\n");
        LEAVE();
        return (-1);
    }

    LEAVE();
    return (0);
}

/**
 *  @brief  Computes agreed shared key from the public value,
 *          private value, and Diffie-Hellman parameters.
 *
 *  @param shared_key       Pointer to agreed shared key generated
 *  @param public_key       Pointer to public key
 *  @param public_len       Length of public key
 *  @param private_key      Pointer to private key
 *  @param private_len      Length of private key
 *  @param dh_params        Parameters for DH algorithm
 *  @return                 0 on success, -1 on failure
 */
int
compute_dh_agreed_key(u8 * shared_key,
                      u8 * public_key,
                      u32 public_len,
                      u8 * private_key,
                      u32 private_len, DH_PG_PARAMS * dh_params)
{
    size_t shared_len;

    ENTER();

    if (shared_key == NULL || public_key == NULL || private_key == NULL) {
        LEAVE();
        return (-1);
    }

    shared_len = dh_params->primeLen;
    memset(shared_key, 0, 192);

    if (crypto_mod_exp(public_key, public_len,
                       private_key, private_len,
                       dh_params->prime, dh_params->primeLen,
                       shared_key, &shared_len) < 0) {
        printf("compute_dh_agreed_key: crypto_mod_exp failed\n");
        LEAVE();
        return (-1);
    }

    LEAVE();
    return (0);
}
