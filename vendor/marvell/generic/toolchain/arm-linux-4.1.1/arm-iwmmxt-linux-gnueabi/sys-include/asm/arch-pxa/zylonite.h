/*
 *  linux/include/asm-arm/arch-pxa/zylonite.h
 *
 *  Author:	Nicolas Pitre
 *  Created:	Nov 14, 2002
 *  Copyright:	MontaVista Software Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef ASM_MACH_ZYLONITE_H
#define ASM_MACH_ZYLONITE_H

#define ZYLONITE_ETH_PHYS	0x14000000
/* CF SOCKET CARD STATE */
#define CF_UNKNOW		0
#define CF_IO_MEM_MOD		1	
#define CF_TRUE_IDE_MOD	2
#define CF_SOCKET_SW1_REG	(*(volatile long *)(0xf5200004))
#define CF_SOCKET_STATE()	( (CF_SOCKET_SW1_REG & 0x10) ? CF_IO_MEM_MOD : CF_TRUE_IDE_MOD)
#endif
