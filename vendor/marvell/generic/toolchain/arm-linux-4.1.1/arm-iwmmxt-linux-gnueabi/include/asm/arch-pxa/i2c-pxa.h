/*
 *  i2c_pxa.h
 *
 *  Copyright (C) 2002 Intrinsyc Software Inc.
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 */
#ifndef _I2C_PXA_H_
#define _I2C_PXA_H_

#include "asm/types.h" 

extern int pxa_i2c_write(u8 slaveaddr, const u8 * bytesbuf, u32 bytescount);
extern int pxa_i2c_read(u8 slaveaddr, u8 * bytesbuf, u32 bytescount);

#endif
