/*
 * AES Algorithm on OpenSSL optimizaed arm code
 *
 * Copyright (c) 2009 Marvell International Ltd.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <crypto/aes.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/crypto.h>
#include <asm/byteorder.h>

#include "aes_core.h"
#include "mv_aes.h"

/**
 * mv_aes_set_key - Set the AES key.
 * @tfm:	The %crypto_tfm that is used in the context.
 * @in_key:	The input key.
 * @key_len:	The size of the key.
 *
 * Returns 0 on success, on failure the %CRYPTO_TFM_RES_BAD_KEY_LEN flag in tfm
 * is set. &mv_aes_ctx _must_ be the private data embedded in @tfm which is
 * retrieved with crypto_tfm_ctx().
 */
int mv_aes_set_key(struct crypto_tfm *tfm, const u8 *in_key,
		unsigned int key_len)
{
	struct mv_aes_ctx *ctx = crypto_tfm_ctx(tfm);
	u32 *flags = &tfm->crt_flags;
	int ret;
	int key_bits;
	AES_KEY *key_enc;
	AES_KEY *key_dec;

	key_bits = key_len << 3;

	key_enc = &(ctx->key_enc);
	key_dec = &(ctx->key_dec);

	ret = AES_set_encrypt_key(in_key, key_bits, key_enc);
	if (!ret) {
	    ret = AES_set_decrypt_key(in_key, key_bits, key_dec);
	}

	if (!ret) {
	    return 0;
	}

	*flags |= CRYPTO_TFM_RES_BAD_KEY_LEN;
	return -EINVAL;
}
EXPORT_SYMBOL_GPL(mv_aes_set_key);

/* encrypt a block of text */

static void mv_aes_encrypt(struct crypto_tfm *tfm, u8 *out, const u8 *in)
{
	const struct mv_aes_ctx *ctx = crypto_tfm_ctx(tfm);
	const AES_KEY *key = &(ctx->key_enc);

	AES_encrypt(in, out, key);
}

/* decrypt a block of text */

static void mv_aes_decrypt(struct crypto_tfm *tfm, u8 *out, const u8 *in)
{
	const struct mv_aes_ctx *ctx = crypto_tfm_ctx(tfm);
	const AES_KEY *key = &(ctx->key_dec);

	AES_decrypt(in, out, key);
}

static struct crypto_alg mv_aes_alg = {
	.cra_name		=	"aes",
	.cra_driver_name	=	"mv-aes-generic",
	.cra_priority		=	MV_AES_CRA_PRIORITY,
	.cra_flags		=	CRYPTO_ALG_TYPE_CIPHER,
	.cra_blocksize		=	AES_BLOCK_SIZE,
	.cra_ctxsize		=	sizeof(struct mv_aes_ctx),
	.cra_alignmask		=	3,
	.cra_module		=	THIS_MODULE,
	.cra_list		=	LIST_HEAD_INIT(mv_aes_alg.cra_list),
	.cra_u			=	{
		.cipher = {
			.cia_min_keysize	=	AES_MIN_KEY_SIZE,
			.cia_max_keysize	=	AES_MAX_KEY_SIZE,
			.cia_setkey		    =	mv_aes_set_key,
			.cia_encrypt		=	mv_aes_encrypt,
			.cia_decrypt		=	mv_aes_decrypt
		}
	}
};

static int __init mv_aes_init(void)
{
	return crypto_register_alg(&mv_aes_alg);
}

static void __exit mv_aes_exit(void)
{
	crypto_unregister_alg(&mv_aes_alg);
}

module_init(mv_aes_init);
module_exit(mv_aes_exit);

MODULE_DESCRIPTION("Rijndael (AES) Cipher Algorithm");
MODULE_LICENSE("GPL");
MODULE_ALIAS("mv_aes");
