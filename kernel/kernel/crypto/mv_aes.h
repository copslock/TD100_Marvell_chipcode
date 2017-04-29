/*
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

#ifndef _MV_AES_H_

#include <linux/types.h>
#include <linux/crypto.h>

#define MV_AES_CRA_PRIORITY			300

struct mv_aes_ctx {
	AES_KEY key_enc;
	AES_KEY key_dec;
};

int mv_aes_set_key(struct crypto_tfm *tfm, const u8 *in_key,
		unsigned int key_len);

#endif /* _MV_AES_H_ */

