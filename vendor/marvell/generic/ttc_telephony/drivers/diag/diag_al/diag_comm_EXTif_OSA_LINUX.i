# 1 "diag_comm_EXTif_OSA_LINUX.c"
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/diag/diag_al//"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "diag_comm_EXTif_OSA_LINUX.c"
/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code ("Material") are owned
   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
   transmitted, distributed, or disclosed in any way without Intel's prior express written permission.

   No license under any patent, copyright, trade secret or other intellectual property right is granted to or
   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
   Intel in writing.
   -------------------------------------------------------------------------------------------------------------------*/

/*********************************************************************
 *                      M O D U L E     B O D Y                       *
 **********************************************************************
 * Title: Diag                                                        *
 *                                                                    *
 * Filename: diag_comm_EXTif_OSA_LINUX.c                              *
 *                                                                    *
 * Target, platform: Common Platform, SW platform                     *
 *                                                                    *
 * Authors: Yaeli Karni												 *
 *                                                                    *
 * Description: handles the OS specific implementation for ExtIF      *
 *                                                                    *
 * Notes:                                                             *
 *                                                                    *
 *  Yaeli Karni, NOv 2007 - set OS specific file for ext if           *
 *	implement the following:										 *
 *				diagCommExtIFosSpecificInit							 *
 *				diagCommExtIfConnInit								 *
 *				diag_os_TransmitToExtIfMulti						 *
 *				diag_os_TransmitToExtIf								 *
 *				diagExtIfcon_discon									 *
 *			                                                                                 *
 **********************************************************************/

/************* General include definitions ****************************/
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/types.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */





# 1 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stddef.h" 1 3 4
/* Copyright (C) 1989, 1997, 1998, 1999, 2000, 2002, 2004, 2009
   Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

Under Section 7 of GPL version 3, you are granted additional
permissions described in the GCC Runtime Library Exception, version
3.1, as published by the Free Software Foundation.

You should have received a copy of the GNU General Public License and
a copy of the GCC Runtime Library Exception along with this program;
see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
<http://www.gnu.org/licenses/>.  */

/*
 * ISO C Standard:  7.17  Common definitions  <stddef.h>
 */






/* Any one of these symbols __need_* means that GNU libc
   wants us just to define one data type.  So don't define
   the symbols that indicate this file's entire job has been done.  */
# 49 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stddef.h" 3 4
/* This avoids lossage on SunOS but only if stdtypes.h comes first.
   There's no way to win with the other order!  Sun lossage.  */

/* On 4.3bsd-net2, make sure ansi.h is included, so we have
   one less case to deal with in the following.  */



/* On FreeBSD 5, machine/ansi.h does not exist anymore... */




/* In 4.3bsd-net2, machine/ansi.h defines these symbols, which are
   defined if the corresponding type is *not* defined.
   FreeBSD-2.1 defines _MACHINE_ANSI_H_ instead of _ANSI_H_ */
# 94 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stddef.h" 3 4
/* Sequent's header files use _PTRDIFF_T_ in some conflicting way.
   Just ignore it.  */




/* On VxWorks, <type/vxTypesBase.h> may have defined macros like
   _TYPE_size_t which will typedef size_t.  fixincludes patched the
   vxTypesBase.h so that this macro is only defined if _GCC_SIZE_T is
   not defined, and so that defining this macro defines _GCC_SIZE_T.
   If we find that the macros are still defined at this point, we must
   invoke them so that the type is defined as expected.  */
# 119 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stddef.h" 3 4
/* In case nobody has defined these types, but we aren't running under
   GCC 2.00, make sure that __PTRDIFF_TYPE__, __SIZE_TYPE__, and
   __WCHAR_TYPE__ have reasonable values.  This can happen if the
   parts of GCC is compiled by an older compiler, that actually
   include gstddef.h, such as collect2.  */

/* Signed type of difference of two pointers.  */

/* Define this type if we are doing the whole job,
   or if we want this type in particular.  */
# 149 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stddef.h" 3 4
typedef int ptrdiff_t;
# 159 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stddef.h" 3 4
/* If this symbol has done its job, get rid of it.  */




/* Unsigned type of `sizeof' something.  */

/* Define this type if we are doing the whole job,
   or if we want this type in particular.  */
# 211 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stddef.h" 3 4
typedef unsigned int size_t;
# 237 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stddef.h" 3 4
/* Wide character type.
   Locale-writers should change this as necessary to
   be big enough to hold unique values not between 0 and 127,
   and not (wchar_t) -1, for each defined multibyte character.  */

/* Define this type if we are doing the whole job,
   or if we want this type in particular.  */
# 357 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stddef.h" 3 4
/*  In 4.3bsd-net2, leave these undefined to indicate that size_t, etc.
    are already defined.  */
/*  BSD/OS 3.1 and FreeBSD [23].x require the MACHINE_ANSI_H check here.  */
# 392 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stddef.h" 3 4
/* A null pointer constant.  */
# 34 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/types.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/stdint.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */



# 1 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stddef.h" 1 3 4
/* Copyright (C) 1989, 1997, 1998, 1999, 2000, 2002, 2004, 2009
   Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

Under Section 7 of GPL version 3, you are granted additional
permissions described in the GCC Runtime Library Exception, version
3.1, as published by the Free Software Foundation.

You should have received a copy of the GNU General Public License and
a copy of the GCC Runtime Library Exception along with this program;
see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
<http://www.gnu.org/licenses/>.  */

/*
 * ISO C Standard:  7.17  Common definitions  <stddef.h>
 */






/* Any one of these symbols __need_* means that GNU libc
   wants us just to define one data type.  So don't define
   the symbols that indicate this file's entire job has been done.  */





/* snaroff@next.com says the NeXT needs this.  */

/* Irix 5.1 needs this.  */




/* This avoids lossage on SunOS but only if stdtypes.h comes first.
   There's no way to win with the other order!  Sun lossage.  */

/* On 4.3bsd-net2, make sure ansi.h is included, so we have
   one less case to deal with in the following.  */



/* On FreeBSD 5, machine/ansi.h does not exist anymore... */




/* In 4.3bsd-net2, machine/ansi.h defines these symbols, which are
   defined if the corresponding type is *not* defined.
   FreeBSD-2.1 defines _MACHINE_ANSI_H_ instead of _ANSI_H_ */
# 94 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stddef.h" 3 4
/* Sequent's header files use _PTRDIFF_T_ in some conflicting way.
   Just ignore it.  */




/* On VxWorks, <type/vxTypesBase.h> may have defined macros like
   _TYPE_size_t which will typedef size_t.  fixincludes patched the
   vxTypesBase.h so that this macro is only defined if _GCC_SIZE_T is
   not defined, and so that defining this macro defines _GCC_SIZE_T.
   If we find that the macros are still defined at this point, we must
   invoke them so that the type is defined as expected.  */
# 119 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stddef.h" 3 4
/* In case nobody has defined these types, but we aren't running under
   GCC 2.00, make sure that __PTRDIFF_TYPE__, __SIZE_TYPE__, and
   __WCHAR_TYPE__ have reasonable values.  This can happen if the
   parts of GCC is compiled by an older compiler, that actually
   include gstddef.h, such as collect2.  */

/* Signed type of difference of two pointers.  */

/* Define this type if we are doing the whole job,
   or if we want this type in particular.  */
# 159 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stddef.h" 3 4
/* If this symbol has done its job, get rid of it.  */




/* Unsigned type of `sizeof' something.  */

/* Define this type if we are doing the whole job,
   or if we want this type in particular.  */
# 237 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stddef.h" 3 4
/* Wide character type.
   Locale-writers should change this as necessary to
   be big enough to hold unique values not between 0 and 127,
   and not (wchar_t) -1, for each defined multibyte character.  */

/* Define this type if we are doing the whole job,
   or if we want this type in particular.  */
# 278 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stddef.h" 3 4
/* On BSD/386 1.1, at least, machine/ansi.h defines _BSD_WCHAR_T_
   instead of _WCHAR_T_, and _BSD_RUNE_T_ (which, unlike the other
   symbols in the _FOO_T_ family, stays defined even after its
   corresponding type is defined).  If we define wchar_t, then we
   must undef _WCHAR_T_; for BSD/386 1.1 (and perhaps others), if
   we undef _WCHAR_T_, then we must also define rune_t, since 
   headers like runetype.h assume that if machine/ansi.h is included,
   and _BSD_WCHAR_T_ is not defined, then rune_t is available.
   machine/ansi.h says, "Note that _WCHAR_T_ and _RUNE_T_ must be of
   the same type." */
# 305 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stddef.h" 3 4
/* FreeBSD 5 can't be handled well using "traditional" logic above
   since it no longer defines _BSD_RUNE_T_ yet still desires to export
   rune_t in some cases... */
# 323 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stddef.h" 3 4
typedef unsigned int wchar_t;
# 357 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stddef.h" 3 4
/*  In 4.3bsd-net2, leave these undefined to indicate that size_t, etc.
    are already defined.  */
/*  BSD/OS 3.1 and FreeBSD [23].x require the MACHINE_ANSI_H check here.  */
# 392 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stddef.h" 3 4
/* A null pointer constant.  */
# 410 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stddef.h" 3 4
/* Offset of member MEMBER in a struct of type TYPE. */
# 32 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/stdint.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/_types.h" 1
/*	$OpenBSD: _types.h,v 1.1 2006/01/06 18:53:05 millert Exp $	*/

/*-
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)types.h	8.3 (Berkeley) 1/5/94
 */







# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/arch-arm/include/machine/_types.h" 1
/*	$OpenBSD: _types.h,v 1.3 2006/02/14 18:12:58 miod Exp $	*/

/*-
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)types.h	8.3 (Berkeley) 1/5/94
 *	@(#)ansi.h	8.2 (Berkeley) 1/4/94
 */
# 49 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/arch-arm/include/machine/_types.h"
/* 7.18.1.1 Exact-width integer types */
typedef __signed char __int8_t;
typedef unsigned char __uint8_t;
typedef short __int16_t;
typedef unsigned short __uint16_t;
typedef int __int32_t;
typedef unsigned int __uint32_t;
/* LONGLONG */
typedef long long __int64_t;
/* LONGLONG */
typedef unsigned long long __uint64_t;

/* 7.18.1.2 Minimum-width integer types */
typedef __int8_t __int_least8_t;
typedef __uint8_t __uint_least8_t;
typedef __int16_t __int_least16_t;
typedef __uint16_t __uint_least16_t;
typedef __int32_t __int_least32_t;
typedef __uint32_t __uint_least32_t;
typedef __int64_t __int_least64_t;
typedef __uint64_t __uint_least64_t;

/* 7.18.1.3 Fastest minimum-width integer types */
typedef __int32_t __int_fast8_t;
typedef __uint32_t __uint_fast8_t;
typedef __int32_t __int_fast16_t;
typedef __uint32_t __uint_fast16_t;
typedef __int32_t __int_fast32_t;
typedef __uint32_t __uint_fast32_t;
typedef __int64_t __int_fast64_t;
typedef __uint64_t __uint_fast64_t;

/* 7.18.1.4 Integer types capable of holding object pointers */
typedef int __intptr_t;
typedef unsigned int __uintptr_t;

/* 7.18.1.5 Greatest-width integer types */
typedef __int64_t __intmax_t;
typedef __uint64_t __uintmax_t;

/* Register size */
typedef __int32_t __register_t;

/* VM system types */
typedef unsigned long __vaddr_t;
typedef unsigned long __paddr_t;
typedef unsigned long __vsize_t;
typedef unsigned long __psize_t;

/* Standard system types */
typedef int __clock_t;
typedef int __clockid_t;
typedef long __ptrdiff_t;
typedef int __time_t;
typedef int __timer_t;

typedef __builtin_va_list __va_list;




/* Wide character support types */

typedef int __wchar_t;

typedef int __wint_t;
typedef int __rune_t;
typedef void * __wctrans_t;
typedef void * __wctype_t;
# 41 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/_types.h" 2

typedef unsigned long __cpuid_t; /* CPU id */
typedef __int32_t __dev_t; /* device number */
typedef __uint32_t __fixpt_t; /* fixed point number */
typedef __uint32_t __gid_t; /* group id */
typedef __uint32_t __id_t; /* may contain pid, uid or gid */
typedef __uint32_t __in_addr_t; /* base type for internet address */
typedef __uint16_t __in_port_t; /* IP port type */
typedef __uint32_t __ino_t; /* inode number */
typedef long __key_t; /* IPC key (for Sys V IPC) */
typedef __uint32_t __mode_t; /* permissions */
typedef __uint32_t __nlink_t; /* link count */
typedef __int32_t __pid_t; /* process id */
typedef __uint64_t __rlim_t; /* resource limit */
typedef __uint16_t __sa_family_t; /* sockaddr address family type */
typedef __int32_t __segsz_t; /* segment size */
typedef __uint32_t __socklen_t; /* length type for network syscalls */
typedef __int32_t __swblk_t; /* swap offset */
typedef __uint32_t __uid_t; /* user id */
typedef __uint32_t __useconds_t; /* microseconds */
typedef __int32_t __suseconds_t; /* microseconds (signed) */

/*
 * mbstate_t is an opaque object to keep conversion state, during multibyte
 * stream conversions. The content must not be referenced by user programs.
 */
typedef union {
 char __mbstate8[128];
 __int64_t __mbstateL; /* for alignment */
} __mbstate_t;

/* BIONIC: if we're using non-cleaned up user-level kernel headers, 
 *         this will prevent many type declaration conflicts
 */
# 33 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/stdint.h" 2
# 44 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/stdint.h"
/* the definitions of STDINT_LIMITS depend on those of STDINT_MACROS */
# 53 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/stdint.h"
typedef __int8_t int8_t;
typedef __uint8_t uint8_t;
typedef __int16_t int16_t;
typedef __uint16_t uint16_t;
typedef __int32_t int32_t;
typedef __uint32_t uint32_t;

typedef __int64_t int64_t;
typedef __uint64_t uint64_t;


/*
 * int8_t & uint8_t
 */

typedef int8_t int_least8_t;
typedef int8_t int_fast8_t;

typedef uint8_t uint_least8_t;
typedef uint8_t uint_fast8_t;
# 97 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/stdint.h"
/*
 * int16_t & uint16_t
 */


typedef int16_t int_least16_t;
typedef int32_t int_fast16_t;

typedef uint16_t uint_least16_t;
typedef uint32_t uint_fast16_t;
# 131 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/stdint.h"
/*
 * int32_t & uint32_t
 */

typedef int32_t int_least32_t;
typedef int32_t int_fast32_t;

typedef uint32_t uint_least32_t;
typedef uint32_t uint_fast32_t;
# 165 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/stdint.h"
/*
 *  int64_t
 */
typedef int64_t int_least64_t;
typedef int64_t int_fast64_t;

typedef uint64_t uint_least64_t;
typedef uint64_t uint_fast64_t;
# 207 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/stdint.h"
/*
 * intptr_t & uintptr_t
 */

typedef int intptr_t;
typedef unsigned int uintptr_t;
# 224 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/stdint.h"
/*
 *  intmax_t & uintmax_t
 */



typedef uint64_t uintmax_t;
typedef int64_t intmax_t;
# 255 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/stdint.h"
/* size_t is defined by the GCC-specific <stddef.h> */


typedef long int ssize_t;




/* Keep the kernel from trying to define these types... */
# 35 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/types.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/cdefs.h" 1
/*	$NetBSD: cdefs.h,v 1.58 2004/12/11 05:59:00 christos Exp $	*/

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)cdefs.h	8.8 (Berkeley) 1/9/95
 */




/*
 * Macro to test if we're using a GNU C compiler of a specific vintage
 * or later, for e.g. features that appeared in a particular version
 * of GNU C.  Usage:
 *
 *	#if __GNUC_PREREQ__(major, minor)
 *	...cool feature...
 *	#else
 *	...delete feature...
 *	#endif
 */
# 59 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/cdefs.h"
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/cdefs_elf.h" 1
/*	$NetBSD: cdefs_elf.h,v 1.22 2005/02/26 22:25:34 perry Exp $	*/

/*
 * Copyright (c) 1995, 1996 Carnegie-Mellon University.
 * All rights reserved.
 *
 * Author: Chris G. Demetriou
 *
 * Permission to use, copy, modify and distribute this software and
 * its documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 *
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND
 * FOR ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 *
 * Carnegie Mellon requests users of this software to return to
 *
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 *
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */
# 60 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/cdefs.h" 2
# 71 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/cdefs.h"
/*
 * The __CONCAT macro is used to concatenate parts of symbol names, e.g.
 * with "#define OLD(foo) __CONCAT(old,foo)", OLD(foo) produces oldfoo.
 * The __CONCAT macro is a bit tricky -- make sure you don't put spaces
 * in between its arguments.  __CONCAT can also concatenate double-quoted
 * strings produced by the __STRING macro, but this only works with ANSI C.
 */
# 124 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/cdefs.h"
/*
 * Used for internal auditing of the NetBSD source tree.
 */






/*
 * The following macro is used to remove const cast-away warnings
 * from gcc -Wcast-qual; it should be used with caution because it
 * can hide valid errors; in particular most valid uses are in
 * situations where the API requires it, not to cast away string
 * constants. We don't use *intptr_t on purpose here and we are
 * explicit about unsigned long so that we don't have additional
 * dependencies.
 */


/*
 * GCC2 provides __extension__ to suppress warnings for various GNU C
 * language extensions under "-ansi -pedantic".
 */




/*
 * GCC1 and some versions of GCC2 declare dead (non-returning) and
 * pure (no side effects) functions using "volatile" and "const";
 * unfortunately, these then cause warnings under "-ansi -pedantic".
 * GCC2 uses a new, peculiar __attribute__((attrs)) style.  All of
 * these work for GNU C++ (modulo a slight glitch in the C++ grammar
 * in the distribution version of 2.5.5).
 */
# 168 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/cdefs.h"
/* Delete pseudo-keywords wherever they are not available or needed. */
# 212 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/cdefs.h"
/*
 * C99 defines the restrict type qualifier keyword, which was made available
 * in GCC 2.92.
 */
# 224 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/cdefs.h"
/*
 * C99 defines __func__ predefined identifier, which was made available
 * in GCC 2.95.
 */
# 259 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/cdefs.h"
/*
 * A barrier to stop the optimizer from moving code or assume live
 * register values. This is gcc specific, the version is more or less
 * arbitrary, might work with older compilers.
 */






/*
 * GNU C version 2.96 adds explicit branch prediction so that
 * the CPU back-end can hint the processor and also so that
 * code blocks can be reordered such that the predicted path
 * sees a more linear flow, thus improving cache behavior, etc.
 *
 * The following two macros provide us with a way to use this
 * compiler feature.  Use __predict_true() if you expect the expression
 * to evaluate to true, and __predict_false() if you expect the
 * expression to evaluate to false.
 *
 * A few notes about usage:
 *
 *	* Generally, __predict_false() error condition checks (unless
 *	  you have some _strong_ reason to do otherwise, in which case
 *	  document it), and/or __predict_true() `no-error' condition
 *	  checks, assuming you want to optimize for the no-error case.
 *
 *	* Other than that, if you don't know the likelihood of a test
 *	  succeeding from empirical or other `hard' evidence, don't
 *	  make predictions.
 *
 *	* These are meant to be used in places that are run `a lot'.
 *	  It is wasteful to make predictions in code that is run
 *	  seldomly (e.g. at subsystem initialization time) as the
 *	  basic block reordering that this affects can often generate
 *	  larger code.
 */
# 314 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/cdefs.h"
/*
 * Macros for manipulating "link sets".  Link sets are arrays of pointers
 * to objects, which are gathered up by the linker.
 *
 * Object format-specific code has provided us with the following macros:
 *
 *	__link_set_add_text(set, sym)
 *		Add a reference to the .text symbol `sym' to `set'.
 *
 *	__link_set_add_rodata(set, sym)
 *		Add a reference to the .rodata symbol `sym' to `set'.
 *
 *	__link_set_add_data(set, sym)
 *		Add a reference to the .data symbol `sym' to `set'.
 *
 *	__link_set_add_bss(set, sym)
 *		Add a reference to the .bss symbol `sym' to `set'.
 *
 *	__link_set_decl(set, ptype)
 *		Provide an extern declaration of the set `set', which
 *		contains an array of the pointer type `ptype'.  This
 *		macro must be used by any code which wishes to reference
 *		the elements of a link set.
 *
 *	__link_set_start(set)
 *		This points to the first slot in the link set.
 *
 *	__link_set_end(set)
 *		This points to the (non-existent) slot after the last
 *		entry in the link set.
 *
 *	__link_set_count(set)
 *		Count the number of entries in link set `set'.
 *
 * In addition, we provide the following macros for accessing link sets:
 *
 *	__link_set_foreach(pvar, set)
 *		Iterate over the link set `set'.  Because a link set is
 *		an array of pointers, pvar must be declared as "type **pvar",
 *		and the actual entry accessed as "*pvar".
 *
 *	__link_set_entry(set, idx)
 *		Access the link set entry at index `idx' from set `set'.
 */





/*
 * Some of the recend FreeBSD sources used in Bionic need this.
 * Originally, this is used to embed the rcs versions of each source file
 * in the generated binary. We certainly don't want this in Bionic.
 */


/*-
 * The following definitions are an extension of the behavior originally
 * implemented in <sys/_posix.h>, but with a different level of granularity.
 * POSIX.1 requires that the macros we test be defined before any standard
 * header file is included.
 *
 * Here's a quick run-down of the versions:
 *  defined(_POSIX_SOURCE)		1003.1-1988
 *  _POSIX_C_SOURCE == 1		1003.1-1990
 *  _POSIX_C_SOURCE == 2		1003.2-1992 C Language Binding Option
 *  _POSIX_C_SOURCE == 199309		1003.1b-1993
 *  _POSIX_C_SOURCE == 199506		1003.1c-1995, 1003.1i-1995,
 *					and the omnibus ISO/IEC 9945-1: 1996
 *  _POSIX_C_SOURCE == 200112		1003.1-2001
 *  _POSIX_C_SOURCE == 200809		1003.1-2008
 *
 * In addition, the X/Open Portability Guide, which is now the Single UNIX
 * Specification, defines a feature-test macro which indicates the version of
 * that specification, and which subsumes _POSIX_C_SOURCE.
 *
 * Our macros begin with two underscores to avoid namespace screwage.
 */

/* Deal with IEEE Std. 1003.1-1990, in which _POSIX_C_SOURCE == 1. */





/* Deal with IEEE Std. 1003.2-1992, in which _POSIX_C_SOURCE == 2. */





/* Deal with various X/Open Portability Guides and Single UNIX Spec. */
# 422 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/cdefs.h"
/*
 * Deal with all versions of POSIX.  The ordering relative to the tests above is
 * important.
 */
# 483 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/cdefs.h"
/*
 * Default values.
 */
# 36 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/types.h" 2

# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/posix_types.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/stddef.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/compiler.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 16 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/stddef.h" 2
# 16 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/posix_types.h" 2
# 32 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/posix_types.h"
typedef struct {
 unsigned long fds_bits [(1024/(8 * sizeof(unsigned long)))];
} __kernel_fd_set;

typedef void (*__kernel_sighandler_t)(int);

typedef int __kernel_key_t;
typedef int __kernel_mqd_t;

# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/arch-arm/asm/posix_types.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/



typedef unsigned long __kernel_ino_t;
typedef unsigned short __kernel_mode_t;
typedef unsigned short __kernel_nlink_t;
typedef long __kernel_off_t;
typedef int __kernel_pid_t;
typedef unsigned short __kernel_ipc_pid_t;
typedef unsigned short __kernel_uid_t;
typedef unsigned short __kernel_gid_t;
typedef unsigned int __kernel_size_t;
typedef int __kernel_ssize_t;
typedef int __kernel_ptrdiff_t;
typedef long __kernel_time_t;
typedef long __kernel_suseconds_t;
typedef long __kernel_clock_t;
typedef int __kernel_timer_t;
typedef int __kernel_clockid_t;
typedef int __kernel_daddr_t;
typedef char * __kernel_caddr_t;
typedef unsigned short __kernel_uid16_t;
typedef unsigned short __kernel_gid16_t;
typedef unsigned int __kernel_uid32_t;
typedef unsigned int __kernel_gid32_t;

typedef unsigned short __kernel_old_uid_t;
typedef unsigned short __kernel_old_gid_t;
typedef unsigned short __kernel_old_dev_t;


typedef long long __kernel_loff_t;


typedef struct {



 int __val[2];

} __kernel_fsid_t;
# 42 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/posix_types.h" 2
# 38 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/types.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/arch-arm/asm/types.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/





typedef unsigned short umode_t;

typedef __signed__ char __s8;
typedef unsigned char __u8;

typedef __signed__ short __s16;
typedef unsigned short __u16;

typedef __signed__ int __s32;
typedef unsigned int __u32;


typedef __signed__ long long __s64;
typedef unsigned long long __u64;
# 39 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/types.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/types.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/posix_types.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 16 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/types.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/arch-arm/asm/types.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 17 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/types.h" 2




typedef __u16 __le16;
typedef __u16 __be16;
typedef __u32 __le32;
typedef __u32 __be32;

typedef __u64 __le64;
typedef __u64 __be64;


struct ustat {
 __kernel_daddr_t f_tfree;
 __kernel_ino_t f_tinode;
 char f_fname[6];
 char f_fpack[6];
};
# 40 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/types.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/arch-arm/include/machine/kernel.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */



/* this file contains kernel-specific definitions that were optimized out of
   our processed kernel headers, but still useful nonetheless... */

typedef unsigned long __kernel_blkcnt_t;
typedef unsigned long __kernel_blksize_t;

/* these aren't really defined by the kernel headers though... */
typedef unsigned long __kernel_fsblkcnt_t;
typedef unsigned long __kernel_fsfilcnt_t;
typedef unsigned int __kernel_id_t;
# 41 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/types.h" 2

typedef __u32 __kernel_dev_t;

/* be careful with __kernel_gid_t and __kernel_uid_t
 * these are defined as 16-bit for legacy reason, but
 * the kernel uses 32-bits instead.
 *
 * 32-bit valuea are required for Android, so use
 * __kernel_uid32_t and __kernel_gid32_t
 */

typedef __kernel_blkcnt_t blkcnt_t;
typedef __kernel_blksize_t blksize_t;
typedef __kernel_clock_t clock_t;
typedef __kernel_clockid_t clockid_t;
typedef __kernel_dev_t dev_t;
typedef __kernel_fsblkcnt_t fsblkcnt_t;
typedef __kernel_fsfilcnt_t fsfilcnt_t;
typedef __kernel_gid32_t gid_t;
typedef __kernel_id_t id_t;
typedef __kernel_ino_t ino_t;
typedef __kernel_key_t key_t;
typedef __kernel_mode_t mode_t;
typedef __kernel_nlink_t nlink_t;


typedef __kernel_off_t off_t;

typedef __kernel_loff_t loff_t;
typedef loff_t off64_t; /* GLibc-specific */

typedef __kernel_pid_t pid_t;

/* while POSIX wants these in <sys/types.h>, we
 * declare then in <pthread.h> instead */
# 93 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/types.h"
/* size_t is defined by the GCC-specific <stddef.h> */





typedef __kernel_suseconds_t suseconds_t;
typedef __kernel_time_t time_t;
typedef __kernel_uid32_t uid_t;
typedef signed long useconds_t;

typedef __kernel_daddr_t daddr_t;
typedef __kernel_timer_t timer_t;
typedef __kernel_mqd_t mqd_t;

typedef __kernel_caddr_t caddr_t;
typedef unsigned int uint_t;
typedef unsigned int uint;

/* for some applications */
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/sysmacros.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */



/* some rogue code includes this file directly :-( */




static __inline__ int major(dev_t _dev)
{
  return (_dev >> 8) & 0xfff;
}

static __inline__ int minor(dev_t _dev)
{
  return (_dev & 0xff) | ((_dev >> 12) & 0xfff00);
}

static __inline__ dev_t makedev(int __ma, int __mi)
{
  return ((__ma & 0xfff) << 8) | (__mi & 0xff) | ((__mi & 0xfff00) << 12);
}
# 114 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/types.h" 2


typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int u_int;
typedef unsigned long u_long;

typedef uint32_t u_int32_t;
typedef uint16_t u_int16_t;
typedef uint8_t u_int8_t;
typedef uint64_t u_int64_t;
# 49 "diag_comm_EXTif_OSA_LINUX.c" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/stat.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/cdefs.h" 1
/*	$NetBSD: cdefs.h,v 1.58 2004/12/11 05:59:00 christos Exp $	*/

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)cdefs.h	8.8 (Berkeley) 1/9/95
 */
# 32 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/stat.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/types.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
# 33 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/stat.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/time.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/cdefs.h" 1
/*	$NetBSD: cdefs.h,v 1.58 2004/12/11 05:59:00 christos Exp $	*/

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)cdefs.h	8.8 (Berkeley) 1/9/95
 */
# 32 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/time.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/types.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
# 33 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/time.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/time.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/types.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 16 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/time.h" 2



struct timespec {
 time_t tv_sec;
 long tv_nsec;
};


struct timeval {
 time_t tv_sec;
 suseconds_t tv_usec;
};

struct timezone {
 int tz_minuteswest;
 int tz_dsttime;
};
# 47 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/time.h"
struct itimerspec {
 struct timespec it_interval;
 struct timespec it_value;
};

struct itimerval {
 struct timeval it_interval;
 struct timeval it_value;
};
# 34 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/time.h" 2



extern int gettimeofday(struct timeval *, struct timezone *);
extern int settimeofday(const struct timeval *, const struct timezone *);

extern int getitimer(int, struct itimerval *);
extern int setitimer(int, const struct itimerval *, struct itimerval *);

extern int utimes(const char *, const struct timeval *);
# 76 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/time.h"

# 34 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/stat.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/stat.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 35 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/stat.h" 2

# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/arch-arm/include/endian.h" 1
/*	$OpenBSD: endian.h,v 1.3 2005/12/13 00:35:23 millert Exp $	*/







# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/types.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
# 10 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/arch-arm/include/endian.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/endian.h" 1
/*	$OpenBSD: endian.h,v 1.17 2006/01/06 18:53:05 millert Exp $	*/

/*-
 * Copyright (c) 1997 Niklas Hallqvist.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Generic definitions for little- and big-endian systems.  Other endianesses
 * has to be dealt with in the specific machine/endian.h file for that port.
 *
 * This file is meant to be included from a little- or big-endian port's
 * machine/endian.h after setting _BYTE_ORDER to either 1234 for little endian
 * or 4321 for big..
 */




# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/cdefs.h" 1
/*	$NetBSD: cdefs.h,v 1.58 2004/12/11 05:59:00 christos Exp $	*/

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)cdefs.h	8.8 (Berkeley) 1/9/95
 */
# 40 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/endian.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/arch-arm/include/machine/_types.h" 1
/*	$OpenBSD: _types.h,v 1.3 2006/02/14 18:12:58 miod Exp $	*/

/*-
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)types.h	8.3 (Berkeley) 1/5/94
 *	@(#)ansi.h	8.2 (Berkeley) 1/4/94
 */
# 41 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/endian.h" 2
# 107 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/endian.h"
/*
 * Define MD_SWAP if you provide swap{16,32}md functions/macros that are
 * optimized for your architecture,  These will be used for swap{16,32}
 * unless the argument is a constant and we are using GCC, where we can
 * take advantage of the CSE phase much better by using the generic version.
 */
# 162 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/endian.h"

__uint64_t htobe64(__uint64_t);
__uint32_t htobe32(__uint32_t);
__uint16_t htobe16(__uint16_t);
__uint64_t betoh64(__uint64_t);
__uint32_t betoh32(__uint32_t);
__uint16_t betoh16(__uint16_t);

__uint64_t htole64(__uint64_t);
__uint32_t htole32(__uint32_t);
__uint16_t htole16(__uint16_t);
__uint64_t letoh64(__uint64_t);
__uint32_t letoh32(__uint32_t);
__uint16_t letoh16(__uint16_t);





/* Can be overridden by machine/endian.h before inclusion of this file.  */
# 210 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/endian.h"
/* Bionic additions */
# 10 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/arch-arm/include/endian.h" 2
# 37 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/stat.h" 2



/* really matches stat64 in the kernel, hence the padding
 * Note: The kernel zero's the padded region because glibc might read them
 * in the hope that the kernel has stretched to using larger sizes.
 */
struct stat {
    unsigned long long st_dev;
    unsigned char __pad0[4];

    unsigned long __st_ino;
    unsigned int st_mode;
    unsigned int st_nlink;

    unsigned long st_uid;
    unsigned long st_gid;

    unsigned long long st_rdev;
    unsigned char __pad3[4];

    long long st_size;
    unsigned long st_blksize;
    unsigned long long st_blocks;

    unsigned long st_atime;
    unsigned long st_atime_nsec;

    unsigned long st_mtime;
    unsigned long st_mtime_nsec;

    unsigned long st_ctime;
    unsigned long st_ctime_nsec;

    unsigned long long st_ino;
};

/* For compatibility with GLibc, we provide macro aliases
 * for the non-Posix nano-seconds accessors.
 */




extern int chmod(const char *, mode_t);
extern int fchmod(int, mode_t);
extern int mkdir(const char *, mode_t);

extern int stat(const char *, struct stat *);
extern int fstat(int, struct stat *);
extern int lstat(const char *, struct stat *);
extern int mknod(const char *, mode_t, dev_t);
extern mode_t umask(mode_t);





static __inline__ int mkfifo(const char *__p, mode_t __m)
{
  return mknod(__p, (__m & ~00170000) | 0010000, (dev_t)0);
}

extern int fstatat(int dirfd, const char *path, struct stat *buf, int flags);
extern int mkdirat(int dirfd, const char *pathname, mode_t mode);
extern int fchownat(int dirfd, const char *path, uid_t owner, gid_t group, int flags);
extern int fchmodat(int dirfd, const char *path, mode_t mode, int flags);
extern int renameat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath);


# 50 "diag_comm_EXTif_OSA_LINUX.c" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/fcntl.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/cdefs.h" 1
/*	$NetBSD: cdefs.h,v 1.58 2004/12/11 05:59:00 christos Exp $	*/

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)cdefs.h	8.8 (Berkeley) 1/9/95
 */
# 32 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/fcntl.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/types.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
# 33 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/fcntl.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/fcntl.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/arch-arm/asm/fcntl.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 20 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/arch-arm/asm/fcntl.h"
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/asm-generic/fcntl.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/types.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 16 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/asm-generic/fcntl.h" 2
# 117 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/asm-generic/fcntl.h"
struct flock {
 short l_type;
 short l_whence;
 off_t l_start;
 off_t l_len;
 pid_t l_pid;

};
# 138 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/asm-generic/fcntl.h"
struct flock64 {
 short l_type;
 short l_whence;
 loff_t l_start;
 loff_t l_len;
 pid_t l_pid;

};
# 21 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/arch-arm/asm/fcntl.h" 2
# 16 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/fcntl.h" 2
# 34 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/fcntl.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/unistd.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */



# 1 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stddef.h" 1 3 4
/* Copyright (C) 1989, 1997, 1998, 1999, 2000, 2002, 2004, 2009
   Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

Under Section 7 of GPL version 3, you are granted additional
permissions described in the GCC Runtime Library Exception, version
3.1, as published by the Free Software Foundation.

You should have received a copy of the GNU General Public License and
a copy of the GCC Runtime Library Exception along with this program;
see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
<http://www.gnu.org/licenses/>.  */

/*
 * ISO C Standard:  7.17  Common definitions  <stddef.h>
 */
# 32 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/unistd.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/cdefs.h" 1
/*	$NetBSD: cdefs.h,v 1.58 2004/12/11 05:59:00 christos Exp $	*/

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)cdefs.h	8.8 (Berkeley) 1/9/95
 */
# 33 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/unistd.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/types.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
# 34 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/unistd.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/select.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/cdefs.h" 1
/*	$NetBSD: cdefs.h,v 1.58 2004/12/11 05:59:00 christos Exp $	*/

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)cdefs.h	8.8 (Berkeley) 1/9/95
 */
# 32 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/select.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/time.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
# 33 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/select.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/types.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
# 34 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/select.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/signal.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/cdefs.h" 1
/*	$NetBSD: cdefs.h,v 1.58 2004/12/11 05:59:00 christos Exp $	*/

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)cdefs.h	8.8 (Berkeley) 1/9/95
 */
# 32 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/signal.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/limits.h" 1
/*	$OpenBSD: limits.h,v 1.13 2005/12/31 19:29:38 millert Exp $	*/
/*	$NetBSD: limits.h,v 1.7 1994/10/26 00:56:00 cgd Exp $	*/

/*
 * Copyright (c) 1988 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)limits.h	5.9 (Berkeley) 4/3/91
 */




# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/cdefs.h" 1
/*	$NetBSD: cdefs.h,v 1.58 2004/12/11 05:59:00 christos Exp $	*/

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)cdefs.h	8.8 (Berkeley) 1/9/95
 */
# 39 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/limits.h" 2
# 86 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/limits.h"
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/limits.h" 1
/* $OpenBSD: limits.h,v 1.6 2005/12/13 00:35:23 millert Exp $ */
/*
 * Copyright (c) 2002 Marc Espie.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE OPENBSD PROJECT AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE OPENBSD
 * PROJECT OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/cdefs.h" 1
/*	$NetBSD: cdefs.h,v 1.58 2004/12/11 05:59:00 christos Exp $	*/

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)cdefs.h	8.8 (Berkeley) 1/9/95
 */
# 30 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/limits.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/limits.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 31 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/limits.h" 2

/* Common definitions for limits.h. */

/*
 * <machine/internal_types.h> is meant to describe a specific architecture,
 * but to be a safe include, that doesn't ever define anything that is
 * user-visible (only typedefs and #define names that stays in the __xxx
 * namespace).
 *
 *   __machine_has_unsigned_chars	(default is signed chars)
 *   __FLT_xxx/__DBL_xxx		non standard values for floating
 *   					points limits.
 */
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/arch-arm/include/machine/internal_types.h" 1
/* $OpenBSD: internal_types.h,v 1.2 2004/05/06 15:53:39 drahn Exp $ */
/* Public domain */
# 45 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/limits.h" 2

/* Legacy */
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/arch-arm/include/machine/limits.h" 1
/*	$OpenBSD: limits.h,v 1.3 2006/01/06 22:48:46 millert Exp $	*/
/*	$NetBSD: limits.h,v 1.4 2003/04/28 23:16:18 bjh21 Exp $	*/

/*
 * Copyright (c) 1988 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	from: @(#)limits.h	7.2 (Berkeley) 6/28/90
 */




# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/cdefs.h" 1
/*	$NetBSD: cdefs.h,v 1.58 2004/12/11 05:59:00 christos Exp $	*/

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)cdefs.h	8.8 (Berkeley) 1/9/95
 */
# 39 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/arch-arm/include/machine/limits.h" 2
# 48 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/limits.h" 2
# 86 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/limits.h"
     /* max value for unsigned long long */

     /* max value for a signed long long */

     /* min value for a signed long long */
# 105 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/limits.h"
/* float.h defines these as well */
# 126 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/limits.h"
/* Bionic: the following has been optimized out from our processed kernel headers */




/* Bionic-specific definitions */







/* tell what we implement legacy stuff when appropriate */
# 161 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/limits.h"
/* XXX: TODO: complete and check list here */
# 87 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/limits.h" 2


# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/syslimits.h" 1
# 90 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/limits.h" 2
# 33 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/signal.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/string.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/cdefs.h" 1
/*	$NetBSD: cdefs.h,v 1.58 2004/12/11 05:59:00 christos Exp $	*/

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)cdefs.h	8.8 (Berkeley) 1/9/95
 */
# 32 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/string.h" 2
# 1 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stddef.h" 1 3 4
/* Copyright (C) 1989, 1997, 1998, 1999, 2000, 2002, 2004, 2009
   Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

Under Section 7 of GPL version 3, you are granted additional
permissions described in the GCC Runtime Library Exception, version
3.1, as published by the Free Software Foundation.

You should have received a copy of the GNU General Public License and
a copy of the GCC Runtime Library Exception along with this program;
see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
<http://www.gnu.org/licenses/>.  */

/*
 * ISO C Standard:  7.17  Common definitions  <stddef.h>
 */
# 33 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/string.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/malloc.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/cdefs.h" 1
/*	$NetBSD: cdefs.h,v 1.58 2004/12/11 05:59:00 christos Exp $	*/

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)cdefs.h	8.8 (Berkeley) 1/9/95
 */
# 32 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/malloc.h" 2
# 1 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stddef.h" 1 3 4
/* Copyright (C) 1989, 1997, 1998, 1999, 2000, 2002, 2004, 2009
   Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

Under Section 7 of GPL version 3, you are granted additional
permissions described in the GCC Runtime Library Exception, version
3.1, as published by the Free Software Foundation.

You should have received a copy of the GNU General Public License and
a copy of the GCC Runtime Library Exception along with this program;
see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
<http://www.gnu.org/licenses/>.  */

/*
 * ISO C Standard:  7.17  Common definitions  <stddef.h>
 */
# 33 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/malloc.h" 2



extern __attribute__((malloc)) void* malloc(size_t);
extern __attribute__((malloc)) void* calloc(size_t, size_t);
extern __attribute__((malloc)) void* realloc(void *, size_t);
extern void free(void *);

extern void* memalign(size_t alignment, size_t bytesize);
extern void* valloc(size_t bytesize);
extern void* pvalloc(size_t bytesize);
extern int mallopt(int param_number, int param_value);
extern size_t malloc_footprint(void);
extern size_t malloc_max_footprint(void);

struct mallinfo {
    size_t arena; /* non-mmapped space allocated from system */
    size_t ordblks; /* number of free chunks */
    size_t smblks; /* always 0 */
    size_t hblks; /* always 0 */
    size_t hblkhd; /* space in mmapped regions */
    size_t usmblks; /* maximum total allocated space */
    size_t fsmblks; /* always 0 */
    size_t uordblks; /* total allocated space */
    size_t fordblks; /* total free space */
    size_t keepcost; /* releasable (via malloc_trim) space */
};

extern struct mallinfo mallinfo(void);


/*
  malloc_usable_size(void* p);

  Returns the number of bytes you can actually use in
  an allocated chunk, which may be more than you requested (although
  often not) due to alignment and minimum size constraints.
  You can use this many bytes without worrying about
  overwriting other allocated objects. This is not a particularly great
  programming practice. malloc_usable_size can be more useful in
  debugging and assertions, for example:

  p = malloc(n);
  assert(malloc_usable_size(p) >= 256);
*/
extern size_t malloc_usable_size(void* block);

/*
  malloc_stats();
  Prints on stderr the amount of space obtained from the system (both
  via sbrk and mmap), the maximum amount (which may be more than
  current if malloc_trim and/or munmap got called), and the current
  number of bytes allocated via malloc (or realloc, etc) but not yet
  freed. Note that this is the number of bytes allocated, not the
  number requested. It will be larger than the number requested
  because of alignment and bookkeeping overhead. Because it includes
  alignment wastage as being in use, this figure may be greater than
  zero even when no user-level chunks are allocated.

  The reported current and maximum system memory can be inaccurate if
  a program makes other calls to system memory allocation functions
  (normally sbrk) outside of malloc.

  malloc_stats prints only the most commonly interesting statistics.
  More information can be obtained by calling mallinfo.
*/
extern void malloc_stats(void);


# 34 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/string.h" 2



extern void* memccpy(void *, const void *, int, size_t);
extern void* memchr(const void *, int, size_t);
extern void* memrchr(const void *, int, size_t);
extern int memcmp(const void *, const void *, size_t);
extern void* memcpy(void *, const void *, size_t);
extern void* memmove(void *, const void *, size_t);
extern void* memset(void *, int, size_t);
extern void* memmem(const void *, size_t, const void *, size_t);
extern void memswap(void *, void *, size_t);

extern char* index(const char *, int);
extern char* rindex(const char *, int);
extern char* strchr(const char *, int);
extern char* strrchr(const char *, int);

extern size_t strlen(const char *);
extern int strcmp(const char *, const char *);
extern char* strcpy(char *, const char *);
extern char* strcat(char *, const char *);

extern int strcasecmp(const char *, const char *);
extern int strncasecmp(const char *, const char *, size_t);
extern char* strdup(const char *);

extern char* strstr(const char *, const char *);
extern char* strcasestr(const char *haystack, const char *needle);
extern char* strtok(char *, const char *);
extern char* strtok_r(char *, const char *, char**);

extern char* strerror(int);
extern int strerror_r(int errnum, char *buf, size_t n);

extern size_t strnlen(const char *, size_t);
extern char* strncat(char *, const char *, size_t);
extern char* strndup(const char *, size_t);
extern int strncmp(const char *, const char *, size_t);
extern char* strncpy(char *, const char *, size_t);

extern size_t strlcat(char *, const char *, size_t);
extern size_t strlcpy(char *, const char *, size_t);

extern size_t strcspn(const char *, const char *);
extern char* strpbrk(const char *, const char *);
extern char* strsep(char **, const char *);
extern size_t strspn(const char *, const char *);

extern char* strsignal(int sig);

extern int strcoll(const char *, const char *);
extern size_t strxfrm(char *, const char *, size_t);


# 34 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/signal.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/types.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
# 35 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/signal.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/arch-arm/asm/signal.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/types.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 16 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/arch-arm/asm/signal.h" 2

struct siginfo;



typedef unsigned long sigset_t;
# 83 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/arch-arm/asm/signal.h"
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/asm-generic/signal.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/compiler.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 16 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/asm-generic/signal.h" 2
# 28 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/asm-generic/signal.h"
typedef void __signalfn_t(int);
typedef __signalfn_t *__sighandler_t;

typedef void __restorefn_t(void);
typedef __restorefn_t *__sigrestore_t;
# 84 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/arch-arm/asm/signal.h" 2

struct sigaction {
 union {
 __sighandler_t _sa_handler;
 void (*_sa_sigaction)(int, struct siginfo *, void *);
 } _u;
 sigset_t sa_mask;
 unsigned long sa_flags;
 void (*sa_restorer)(void);
};




typedef struct sigaltstack {
 void *ss_sp;
 int ss_flags;
 size_t ss_size;
} stack_t;
# 36 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/signal.h" 2


# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/arch-arm/asm/siginfo.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/asm-generic/siginfo.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/compiler.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 16 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/asm-generic/siginfo.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/types.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 17 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/asm-generic/siginfo.h" 2

typedef union sigval {
 int sival_int;
 void *sival_ptr;
} sigval_t;
# 42 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/asm-generic/siginfo.h"
typedef struct siginfo {
 int si_signo;
 int si_errno;
 int si_code;

 union {
 int _pad[((128 - (3 * sizeof(int))) / sizeof(int))];

 struct {
 pid_t _pid;
 __kernel_uid32_t _uid;
 } _kill;

 struct {
 timer_t _tid;
 int _overrun;
 char _pad[sizeof( __kernel_uid32_t) - sizeof(int)];
 sigval_t _sigval;
 int _sys_private;
 } _timer;

 struct {
 pid_t _pid;
 __kernel_uid32_t _uid;
 sigval_t _sigval;
 } _rt;

 struct {
 pid_t _pid;
 __kernel_uid32_t _uid;
 int _status;
 clock_t _utime;
 clock_t _stime;
 } _sigchld;

 struct {
 void *_addr;



 } _sigfault;

 struct {
 long _band;
 int _fd;
 } _sigpoll;
 } _sifields;
} siginfo_t;
# 194 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/asm-generic/siginfo.h"
typedef struct sigevent {
 sigval_t sigev_value;
 int sigev_signo;
 int sigev_notify;
 union {
 int _pad[((64 - (sizeof(int) * 2 + sizeof(sigval_t))) / sizeof(int))];
 int _tid;

 struct {
 void (*_function)(sigval_t);
 void *_attribute;
 } _sigev_thread;
 } _sigev_un;
} sigevent_t;
# 16 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/arch-arm/asm/siginfo.h" 2
# 39 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/signal.h" 2




typedef int sig_atomic_t;

/* crepy NIG / _NSIG handling, just to be safe */







extern const char * const sys_siglist[];
extern const char * const sys_signame[];

static __inline__ int sigismember(sigset_t *set, int signum)
{
    unsigned long *local_set = (unsigned long *)set;
    signum--;
    return (int)((local_set[signum/32] >> (signum%32)) & 1);
}


static __inline__ int sigaddset(sigset_t *set, int signum)
{
    unsigned long *local_set = (unsigned long *)set;
    signum--;
    local_set[signum/32] |= 1UL << (signum%32);
    return 0;
}


static __inline__ int sigdelset(sigset_t *set, int signum)
{
    unsigned long *local_set = (unsigned long *)set;
    signum--;
    local_set[signum/32] &= ~(1UL << (signum%32));
    return 0;
}


static __inline__ int sigemptyset(sigset_t *set)
{
    memset(set, 0, sizeof *set);
    return 0;
}

static __inline__ int sigfillset(sigset_t *set)
{
    memset(set, ~0, sizeof *set);
    return 0;
}


/* compatibility types */
typedef void (*sig_t)(int);
typedef sig_t sighandler_t;

/* differentiater between sysv and bsd behaviour 8*/
extern __sighandler_t sysv_signal(int, __sighandler_t);
extern __sighandler_t bsd_signal(int, __sighandler_t);

/* the default is bsd */
static __inline__ __sighandler_t signal(int s, __sighandler_t f)
{
    return bsd_signal(s,f);
}

/* the syscall itself */
extern __sighandler_t __signal(int, __sighandler_t, int);

extern int sigprocmask(int, const sigset_t *, sigset_t *);
extern int sigaction(int, const struct sigaction *, struct sigaction *);

extern int sigpending(sigset_t *);
extern int sigsuspend(const sigset_t *);
extern int sigwait(const sigset_t *set, int *sig);
extern int siginterrupt(int sig, int flag);

extern int raise(int);
extern int kill(pid_t, int);
extern int killpg(int pgrp, int sig);
extern int sigaltstack(const stack_t *ss, stack_t *oss);



# 35 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/select.h" 2



typedef __kernel_fd_set fd_set;

extern int select(int, fd_set *, fd_set *, fd_set *, struct timeval *);
extern int pselect(int n, fd_set *readfds, fd_set *writefds, fd_set *errfds,
                   const struct timespec *timeout, const sigset_t *sigmask);


# 35 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/unistd.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/sysconf.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/cdefs.h" 1
/*	$NetBSD: cdefs.h,v 1.58 2004/12/11 05:59:00 christos Exp $	*/

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)cdefs.h	8.8 (Berkeley) 1/9/95
 */
# 32 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/sysconf.h" 2



/* as listed by Posix sysconf() description */
/* most of these will return -1 and ENOSYS  */
# 131 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/sysconf.h"
extern int sysconf (int name);


# 36 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/unistd.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/capability.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/types.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 16 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/capability.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/compiler.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 17 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/capability.h" 2



typedef struct __user_cap_header_struct {
 __u32 version;
 int pid;
} *cap_user_header_t;

typedef struct __user_cap_data_struct {
 __u32 effective;
 __u32 permitted;
 __u32 inheritable;
} *cap_user_data_t;
# 37 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/unistd.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/pathconf.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/cdefs.h" 1
/*	$NetBSD: cdefs.h,v 1.58 2004/12/11 05:59:00 christos Exp $	*/

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)cdefs.h	8.8 (Berkeley) 1/9/95
 */
# 32 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/pathconf.h" 2



/* constants to be used for the 'name' paremeter of pathconf/fpathconf */
# 58 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/pathconf.h"
extern long fpathconf(int fildes, int name);
extern long pathconf(const char *path, int name);


# 38 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/unistd.h" 2



/* Standard file descriptor numbers. */




/* Values for whence in fseek and lseek */




extern char **environ;
extern __attribute__((__noreturn__)) void _exit(int);

extern pid_t fork(void);
extern pid_t vfork(void);
extern pid_t getpid(void);
extern pid_t gettid(void);
extern pid_t getpgid(pid_t);
extern int setpgid(pid_t, pid_t);
extern pid_t getppid(void);
extern pid_t getpgrp(void);
extern int setpgrp(void);
extern pid_t setsid(void);

extern int execv(const char *, char * const *);
extern int execvp(const char *, char * const *);
extern int execve(const char *, char * const *, char * const *);
extern int execl(const char *, const char *, ...);
extern int execlp(const char *, const char *, ...);
extern int execle(const char *, const char *, ...);
extern int capget(cap_user_header_t hdrp, cap_user_data_t datap);
extern int capset(cap_user_header_t hdrp, const cap_user_data_t datap);
extern int prctl(int option, unsigned long arg2, unsigned long arg3,
                 unsigned long arg4, unsigned long arg5);

extern int nice(int);

extern int setuid(uid_t);
extern uid_t getuid(void);
extern int seteuid(uid_t);
extern uid_t geteuid(void);
extern int setgid(gid_t);
extern gid_t getgid(void);
extern int setegid(gid_t);
extern gid_t getegid(void);
extern int getgroups(int, gid_t *);
extern int setgroups(size_t, const gid_t *);
extern int setreuid(uid_t, uid_t);
extern int setregid(gid_t, gid_t);
extern int setresuid(uid_t, uid_t, uid_t);
extern int setresgid(gid_t, gid_t, gid_t);
extern int getresuid(uid_t *ruid, uid_t *euid, uid_t *suid);
extern int getresgid(gid_t *rgid, gid_t *egid, gid_t *sgid);
extern int issetugid(void);
extern char* getlogin(void);
extern char* getusershell(void);
extern void setusershell(void);
extern void endusershell(void);



/* Macros for access() */





extern int access(const char *, int);
extern int link(const char *, const char *);
extern int unlink(const char *);
extern int chdir(const char *);
extern int fchdir(int);
extern int rmdir(const char *);
extern int pipe(int *);



extern int chroot(const char *);
extern int symlink(const char *, const char *);
extern int readlink(const char *, char *, size_t);
extern int chown(const char *, uid_t, gid_t);
extern int fchown(int, uid_t, gid_t);
extern int lchown(const char *, uid_t, gid_t);
extern int truncate(const char *, off_t);
extern char *getcwd(char *, size_t);

extern int sync(void);

extern int close(int);
extern off_t lseek(int, off_t, int);
extern loff_t lseek64(int, loff_t, int);

extern ssize_t read(int, void *, size_t);
extern ssize_t write(int, const void *, size_t);
extern ssize_t pread(int, void *, size_t, off_t);
extern ssize_t pwrite(int, void *, size_t, off_t);

extern int dup(int);
extern int dup2(int, int);
extern int fcntl(int, int, ...);
extern int ioctl(int, int, ...);
extern int flock(int, int);
extern int fsync(int);
extern int fdatasync(int);
extern int ftruncate(int, off_t);

extern int pause(void);
extern unsigned int alarm(unsigned int);
extern unsigned int sleep(unsigned int);
extern int usleep(unsigned long);

extern int gethostname(char *, size_t);

extern int getdtablesize(void);

extern void *__brk(void *);
extern int brk(void *);
extern void *sbrk(ptrdiff_t);

extern int getopt(int, char * const *, const char *);
extern char *optarg;
extern int optind, opterr, optopt;

extern int isatty(int);
extern char* ttyname(int);
extern int ttyname_r(int, char*, size_t);

extern int acct(const char* filepath);

static __inline__ int getpagesize(void) {
  extern unsigned int __page_size;
  return __page_size;
}
static __inline__ int __getpageshift(void) {
  extern unsigned int __page_shift;
  return __page_shift;
}

extern int sysconf(int name);

extern int daemon(int, int);

/* A special syscall that is only available on the ARM, not x86 function. */
extern int cacheflush(long start, long end, long flags);

extern pid_t tcgetpgrp(int fd);
extern int tcsetpgrp(int fd, pid_t _pid);
# 201 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/unistd.h"
/* Used to retry syscalls that can return EINTR. */








# 35 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/fcntl.h" 2


# 46 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/fcntl.h"
extern int open(const char* path, int mode, ...);
extern int openat(int fd, const char* path, int mode, ...);
extern int unlinkat(int dirfd, const char *pathname, int flags);
extern int fcntl(int fd, int command, ...);
extern int creat(const char* path, mode_t mode);


# 51 "diag_comm_EXTif_OSA_LINUX.c" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/unistd.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
# 52 "diag_comm_EXTif_OSA_LINUX.c" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/stdio.h" 1
/*	$OpenBSD: stdio.h,v 1.35 2006/01/13 18:10:09 miod Exp $	*/
/*	$NetBSD: stdio.h,v 1.18 1996/04/25 18:29:21 jtc Exp $	*/

/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Chris Torek.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)stdio.h	5.17 (Berkeley) 6/3/91
 */




# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/cdefs.h" 1
/*	$NetBSD: cdefs.h,v 1.58 2004/12/11 05:59:00 christos Exp $	*/

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)cdefs.h	8.8 (Berkeley) 1/9/95
 */
# 42 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/stdio.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/_types.h" 1
/*	$OpenBSD: _types.h,v 1.1 2006/01/06 18:53:05 millert Exp $	*/

/*-
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)types.h	8.3 (Berkeley) 1/5/94
 */
# 43 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/stdio.h" 2

/* va_list and size_t must be defined by stdio.h according to Posix */

# 1 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stdarg.h" 1 3 4
/* Copyright (C) 1989, 1997, 1998, 1999, 2000, 2009 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

Under Section 7 of GPL version 3, you are granted additional
permissions described in the GCC Runtime Library Exception, version
3.1, as published by the Free Software Foundation.

You should have received a copy of the GNU General Public License and
a copy of the GCC Runtime Library Exception along with this program;
see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
<http://www.gnu.org/licenses/>.  */

/*
 * ISO C Standard:  7.15  Variable arguments  <stdarg.h>
 */
# 36 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stdarg.h" 3 4
/* Define __gnuc_va_list.  */



typedef __builtin_va_list __gnuc_va_list;


/* Define the standard macros for the user,
   if this invocation was from the user program.  */
# 47 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/stdio.h" 2

/* note that this forces stddef.h to *only* define size_t */

# 1 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stddef.h" 1 3 4
/* Copyright (C) 1989, 1997, 1998, 1999, 2000, 2002, 2004, 2009
   Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

Under Section 7 of GPL version 3, you are granted additional
permissions described in the GCC Runtime Library Exception, version
3.1, as published by the Free Software Foundation.

You should have received a copy of the GNU General Public License and
a copy of the GCC Runtime Library Exception along with this program;
see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
<http://www.gnu.org/licenses/>.  */

/*
 * ISO C Standard:  7.17  Common definitions  <stddef.h>
 */






/* Any one of these symbols __need_* means that GNU libc
   wants us just to define one data type.  So don't define
   the symbols that indicate this file's entire job has been done.  */
# 49 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stddef.h" 3 4
/* This avoids lossage on SunOS but only if stdtypes.h comes first.
   There's no way to win with the other order!  Sun lossage.  */

/* On 4.3bsd-net2, make sure ansi.h is included, so we have
   one less case to deal with in the following.  */



/* On FreeBSD 5, machine/ansi.h does not exist anymore... */




/* In 4.3bsd-net2, machine/ansi.h defines these symbols, which are
   defined if the corresponding type is *not* defined.
   FreeBSD-2.1 defines _MACHINE_ANSI_H_ instead of _ANSI_H_ */
# 94 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stddef.h" 3 4
/* Sequent's header files use _PTRDIFF_T_ in some conflicting way.
   Just ignore it.  */




/* On VxWorks, <type/vxTypesBase.h> may have defined macros like
   _TYPE_size_t which will typedef size_t.  fixincludes patched the
   vxTypesBase.h so that this macro is only defined if _GCC_SIZE_T is
   not defined, and so that defining this macro defines _GCC_SIZE_T.
   If we find that the macros are still defined at this point, we must
   invoke them so that the type is defined as expected.  */
# 119 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stddef.h" 3 4
/* In case nobody has defined these types, but we aren't running under
   GCC 2.00, make sure that __PTRDIFF_TYPE__, __SIZE_TYPE__, and
   __WCHAR_TYPE__ have reasonable values.  This can happen if the
   parts of GCC is compiled by an older compiler, that actually
   include gstddef.h, such as collect2.  */

/* Signed type of difference of two pointers.  */

/* Define this type if we are doing the whole job,
   or if we want this type in particular.  */
# 159 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stddef.h" 3 4
/* If this symbol has done its job, get rid of it.  */




/* Unsigned type of `sizeof' something.  */

/* Define this type if we are doing the whole job,
   or if we want this type in particular.  */
# 237 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stddef.h" 3 4
/* Wide character type.
   Locale-writers should change this as necessary to
   be big enough to hold unique values not between 0 and 127,
   and not (wchar_t) -1, for each defined multibyte character.  */

/* Define this type if we are doing the whole job,
   or if we want this type in particular.  */
# 357 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stddef.h" 3 4
/*  In 4.3bsd-net2, leave these undefined to indicate that size_t, etc.
    are already defined.  */
/*  BSD/OS 3.1 and FreeBSD [23].x require the MACHINE_ANSI_H check here.  */
# 392 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stddef.h" 3 4
/* A null pointer constant.  */
# 410 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stddef.h" 3 4
/* Offset of member MEMBER in a struct of type TYPE. */
# 51 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/stdio.h" 2

# 1 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stddef.h" 1 3 4
/* Copyright (C) 1989, 1997, 1998, 1999, 2000, 2002, 2004, 2009
   Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

Under Section 7 of GPL version 3, you are granted additional
permissions described in the GCC Runtime Library Exception, version
3.1, as published by the Free Software Foundation.

You should have received a copy of the GNU General Public License and
a copy of the GCC Runtime Library Exception along with this program;
see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
<http://www.gnu.org/licenses/>.  */

/*
 * ISO C Standard:  7.17  Common definitions  <stddef.h>
 */
# 53 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/stdio.h" 2


# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/types.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
# 56 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/stdio.h" 2
# 78 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/stdio.h"
typedef off_t fpos_t; /* stdio file position type */

/*
 * NB: to fit things in six character monocase externals, the stdio
 * code uses the prefix `__s' for stdio objects, typically followed
 * by a three-character attempt at a mnemonic.
 */

/* stdio buffers */
struct __sbuf {
 unsigned char *_base;
 int _size;
};

/*
 * stdio state variables.
 *
 * The following always hold:
 *
 *	if (_flags&(__SLBF|__SWR)) == (__SLBF|__SWR),
 *		_lbfsize is -_bf._size, else _lbfsize is 0
 *	if _flags&__SRD, _w is 0
 *	if _flags&__SWR, _r is 0
 *
 * This ensures that the getc and putc macros (or inline functions) never
 * try to write or read from a file that is in `read' or `write' mode.
 * (Moreover, they can, and do, automatically switch from read mode to
 * write mode, and back, on "r+" and "w+" files.)
 *
 * _lbfsize is used only to make the inline line-buffered output stream
 * code as compact as possible.
 *
 * _ub, _up, and _ur are used when ungetc() pushes back more characters
 * than fit in the current _bf, or when ungetc() pushes back a character
 * that does not match the previous one in _bf.  When this happens,
 * _ub._base becomes non-nil (i.e., a stream has ungetc() data iff
 * _ub._base!=NULL) and _up and _ur save the current values of _p and _r.
 *
 * NOTE: if you change this structure, you also need to update the
 * std() initializer in findfp.c.
 */
typedef struct __sFILE {
 unsigned char *_p; /* current position in (some) buffer */
 int _r; /* read space left for getc() */
 int _w; /* write space left for putc() */
 short _flags; /* flags, below; this FILE is free if 0 */
 short _file; /* fileno, if Unix descriptor, else -1 */
 struct __sbuf _bf; /* the buffer (at least 1 byte, if !NULL) */
 int _lbfsize; /* 0 or -_bf._size, for inline putc */

 /* operations */
 void *_cookie; /* cookie passed to io functions */
 int (*_close)(void *);
 int (*_read)(void *, char *, int);
 fpos_t (*_seek)(void *, fpos_t, int);
 int (*_write)(void *, const char *, int);

 /* extension data, to avoid further ABI breakage */
 struct __sbuf _ext;
 /* data for long sequences of ungetc() */
 unsigned char *_up; /* saved _p when _p is doing ungetc data */
 int _ur; /* saved _r when _r is counting ungetc data */

 /* tricks to meet minimum requirements even when malloc() fails */
 unsigned char _ubuf[3]; /* guarantee an ungetc() buffer */
 unsigned char _nbuf[1]; /* guarantee a getc() buffer */

 /* separate buffer for fgetln() when line crosses buffer boundary */
 struct __sbuf _lb; /* buffer for fgetln() */

 /* Unix stdio files get aligned to block boundaries on fseek() */
 int _blksize; /* stat.st_blksize (may be != _bf._size) */
 fpos_t _offset; /* current lseek offset */
} FILE;


extern FILE __sF[];






 /* RD and WR are never simultaneously asserted */
# 174 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/stdio.h"
/*
 * The following three definitions are for ANSI C, which took them
 * from System V, which brilliantly took internal interface macros and
 * made them official arguments to setvbuf(), without renaming them.
 * Hence, these ugly _IOxxx names are *supposed* to appear in user code.
 *
 * Although numbered as their counterparts above, the implementation
 * does not rely on this.
 */
# 191 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/stdio.h"
/*
 * FOPEN_MAX is a minimum maximum, and should be the number of descriptors
 * that the kernel can provide without allocation of a resource that can
 * fail without the process sleeping.  Do not use this for anything.
 */



/* System V/ANSI C; this is the wrong way to do this, do *not* use these. */
# 220 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/stdio.h"
/*
 * Functions defined in ANSI C standard.
 */

void clearerr(FILE *);
int fclose(FILE *);
int feof(FILE *);
int ferror(FILE *);
int fflush(FILE *);
int fgetc(FILE *);
int fgetpos(FILE *, fpos_t *);
char *fgets(char *, int, FILE *);
FILE *fopen(const char *, const char *);
int fprintf(FILE *, const char *, ...);
int fputc(int, FILE *);
int fputs(const char *, FILE *);
size_t fread(void *, size_t, size_t, FILE *);
FILE *freopen(const char *, const char *, FILE *);
int fscanf(FILE *, const char *, ...);
int fseek(FILE *, long, int);
int fseeko(FILE *, off_t, int);
int fsetpos(FILE *, const fpos_t *);
long ftell(FILE *);
off_t ftello(FILE *);
size_t fwrite(const void *, size_t, size_t, FILE *);
int getc(FILE *);
int getchar(void);
char *gets(char *);



extern int sys_nerr; /* perror(3) external variables */
extern char *sys_errlist[];

void perror(const char *);
int printf(const char *, ...);
int putc(int, FILE *);
int putchar(int);
int puts(const char *);
int remove(const char *);
int rename(const char *, const char *);
void rewind(FILE *);
int scanf(const char *, ...);
void setbuf(FILE *, char *);
int setvbuf(FILE *, char *, int, size_t);
int sprintf(char *, const char *, ...);
int sscanf(const char *, const char *, ...);
FILE *tmpfile(void);
char *tmpnam(char *);
int ungetc(int, FILE *);
int vfprintf(FILE *, const char *, __va_list);
int vprintf(const char *, __va_list);
int vsprintf(char *, const char *, __va_list);


int snprintf(char *, size_t, const char *, ...)
  __attribute__((__format__ (printf, 3, 4)))
  __attribute__((__nonnull__ (3)));
int vfscanf(FILE *, const char *, __va_list)
  __attribute__((__format__ (scanf, 2, 0)))
  __attribute__((__nonnull__ (2)));
int vscanf(const char *, __va_list)
  __attribute__((__format__ (scanf, 1, 0)))
  __attribute__((__nonnull__ (1)));
int vsnprintf(char *, size_t, const char *, __va_list)
  __attribute__((__format__ (printf, 3, 0)))
  __attribute__((__nonnull__ (3)));
int vsscanf(const char *, const char *, __va_list)
  __attribute__((__format__ (scanf, 2, 0)))
  __attribute__((__nonnull__ (2)));





/*
 * Functions defined in POSIX 1003.1.
 */









FILE *fdopen(int, const char *);
int fileno(FILE *);
# 331 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/stdio.h"
char *tempnam(const char *, const char *);





/*
 * Routines that are purely local.
 */


int asprintf(char **, const char *, ...)
  __attribute__((__format__ (printf, 2, 3)))
  __attribute__((__nonnull__ (2)));
char *fgetln(FILE *, size_t *);
int fpurge(FILE *);
int getw(FILE *);
int putw(int, FILE *);
void setbuffer(FILE *, char *, int);
int setlinebuf(FILE *);
int vasprintf(char **, const char *, __va_list)
  __attribute__((__format__ (printf, 2, 0)))
  __attribute__((__nonnull__ (2)));


/*
 * Stdio function-access interface.
 */

FILE *funopen(const void *,
  int (*)(void *, char *, int),
  int (*)(void *, const char *, int),
  fpos_t (*)(void *, fpos_t, int),
  int (*)(void *));





/*
 * Functions internal to the implementation.
 */

int __srget(FILE *);
int __swbuf(int, FILE *);


/*
 * The __sfoo macros are here so that we can
 * define function versions in the C library.
 */


static __inline int __sputc(int _c, FILE *_p) {
 if (--_p->_w >= 0 || (_p->_w >= _p->_lbfsize && (char)_c != '\n'))
  return (*_p->_p++ = _c);
 else
  return (__swbuf(_c, _p));
}
# 425 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/stdio.h"
/*
 * The macro implementations of putc and putc_unlocked are not
 * fully POSIX compliant; they do not set errno on failure
 */
# 53 "diag_comm_EXTif_OSA_LINUX.c" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/stdlib.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/cdefs.h" 1
/*	$NetBSD: cdefs.h,v 1.58 2004/12/11 05:59:00 christos Exp $	*/

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)cdefs.h	8.8 (Berkeley) 1/9/95
 */
# 32 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/stdlib.h" 2

/* wchar_t is required in stdlib.h according to POSIX.
 * note that defining __need_wchar_t prevents stddef.h
 * to define all other symbols it does normally */

# 1 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stddef.h" 1 3 4
/* Copyright (C) 1989, 1997, 1998, 1999, 2000, 2002, 2004, 2009
   Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

Under Section 7 of GPL version 3, you are granted additional
permissions described in the GCC Runtime Library Exception, version
3.1, as published by the Free Software Foundation.

You should have received a copy of the GNU General Public License and
a copy of the GCC Runtime Library Exception along with this program;
see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
<http://www.gnu.org/licenses/>.  */

/*
 * ISO C Standard:  7.17  Common definitions  <stddef.h>
 */






/* Any one of these symbols __need_* means that GNU libc
   wants us just to define one data type.  So don't define
   the symbols that indicate this file's entire job has been done.  */
# 49 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stddef.h" 3 4
/* This avoids lossage on SunOS but only if stdtypes.h comes first.
   There's no way to win with the other order!  Sun lossage.  */

/* On 4.3bsd-net2, make sure ansi.h is included, so we have
   one less case to deal with in the following.  */



/* On FreeBSD 5, machine/ansi.h does not exist anymore... */




/* In 4.3bsd-net2, machine/ansi.h defines these symbols, which are
   defined if the corresponding type is *not* defined.
   FreeBSD-2.1 defines _MACHINE_ANSI_H_ instead of _ANSI_H_ */
# 94 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stddef.h" 3 4
/* Sequent's header files use _PTRDIFF_T_ in some conflicting way.
   Just ignore it.  */




/* On VxWorks, <type/vxTypesBase.h> may have defined macros like
   _TYPE_size_t which will typedef size_t.  fixincludes patched the
   vxTypesBase.h so that this macro is only defined if _GCC_SIZE_T is
   not defined, and so that defining this macro defines _GCC_SIZE_T.
   If we find that the macros are still defined at this point, we must
   invoke them so that the type is defined as expected.  */
# 119 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stddef.h" 3 4
/* In case nobody has defined these types, but we aren't running under
   GCC 2.00, make sure that __PTRDIFF_TYPE__, __SIZE_TYPE__, and
   __WCHAR_TYPE__ have reasonable values.  This can happen if the
   parts of GCC is compiled by an older compiler, that actually
   include gstddef.h, such as collect2.  */

/* Signed type of difference of two pointers.  */

/* Define this type if we are doing the whole job,
   or if we want this type in particular.  */
# 159 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stddef.h" 3 4
/* If this symbol has done its job, get rid of it.  */




/* Unsigned type of `sizeof' something.  */

/* Define this type if we are doing the whole job,
   or if we want this type in particular.  */
# 237 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stddef.h" 3 4
/* Wide character type.
   Locale-writers should change this as necessary to
   be big enough to hold unique values not between 0 and 127,
   and not (wchar_t) -1, for each defined multibyte character.  */

/* Define this type if we are doing the whole job,
   or if we want this type in particular.  */
# 357 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stddef.h" 3 4
/*  In 4.3bsd-net2, leave these undefined to indicate that size_t, etc.
    are already defined.  */
/*  BSD/OS 3.1 and FreeBSD [23].x require the MACHINE_ANSI_H check here.  */
# 392 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stddef.h" 3 4
/* A null pointer constant.  */
# 410 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stddef.h" 3 4
/* Offset of member MEMBER in a struct of type TYPE. */
# 38 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/stdlib.h" 2

# 1 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stddef.h" 1 3 4
/* Copyright (C) 1989, 1997, 1998, 1999, 2000, 2002, 2004, 2009
   Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

Under Section 7 of GPL version 3, you are granted additional
permissions described in the GCC Runtime Library Exception, version
3.1, as published by the Free Software Foundation.

You should have received a copy of the GNU General Public License and
a copy of the GCC Runtime Library Exception along with this program;
see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
<http://www.gnu.org/licenses/>.  */

/*
 * ISO C Standard:  7.17  Common definitions  <stddef.h>
 */
# 40 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/stdlib.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/string.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
# 41 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/stdlib.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/alloca.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
# 42 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/stdlib.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/strings.h" 1
/*	$NetBSD: strings.h,v 1.10 2005/02/03 04:39:32 perry Exp $	*/

/*-
 * Copyright (c) 1998 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Klaus Klein.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */




# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/types.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
# 43 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/strings.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/cdefs.h" 1
/*	$NetBSD: cdefs.h,v 1.58 2004/12/11 05:59:00 christos Exp $	*/

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)cdefs.h	8.8 (Berkeley) 1/9/95
 */
# 44 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/strings.h" 2


int bcmp(const void *, const void *, size_t);
void bcopy(const void *, void *, size_t);
void bzero(void *, size_t);
int ffs(int);
char *index(const char *, int);
char *rindex(const char *, int);
int strcasecmp(const char *, const char *);
int strncasecmp(const char *, const char *, size_t);

# 43 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/stdlib.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/memory.h" 1
# 44 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/stdlib.h" 2






extern __attribute__((__noreturn__)) void exit(int);
extern __attribute__((__noreturn__)) void abort(void);
extern int atexit(void (*)(void));

extern char *getenv(const char *);
extern int putenv(const char *);
extern int setenv(const char *, const char *, int);
extern int unsetenv(const char *);
extern int clearenv(void);

extern char *mktemp (char *);
extern int mkstemp (char *);

extern long strtol(const char *, char **, int);
extern long long strtoll(const char *, char **, int);
extern unsigned long strtoul(const char *, char **, int);
extern unsigned long long strtoull(const char *, char **, int);
extern double strtod(const char *nptr, char **endptr);

static __inline__ float strtof(const char *nptr, char **endptr)
{
    return (float)strtod(nptr, endptr);
}

extern int atoi(const char *);
extern long atol(const char *);
extern long long atoll(const char *);

static __inline__ double atof(const char *nptr)
{
    return (strtod(nptr, ((void *)0)));
}

static __inline__ int abs(int __n) {
    return (__n < 0) ? -__n : __n;
}

static __inline__ long labs(long __n) {
    return (__n < 0L) ? -__n : __n;
}

static __inline__ long long llabs(long long __n) {
    return (__n < 0LL) ? -__n : __n;
}

extern char * realpath(const char *path, char *resolved);
extern int system(const char * string);

extern void * bsearch(const void *key, const void *base0,
 size_t nmemb, size_t size,
 int (*compar)(const void *, const void *));

extern void qsort(void *, size_t, size_t, int (*)(const void *, const void *));

extern long jrand48(unsigned short *);
extern long mrand48(void);
extern long nrand48(unsigned short *);
extern long lrand48(void);
extern unsigned short *seed48(unsigned short*);
extern double erand48(unsigned short xsubi[3]);
extern double drand48(void);
extern void srand48(long);
extern unsigned int arc4random(void);
extern void arc4random_stir(void);
extern void arc4random_addrandom(unsigned char *, int);


static __inline__ int rand(void) {
    return (int)lrand48();
}
static __inline__ void srand(unsigned int __s) {
    srand48(__s);
}
static __inline__ long random(void)
{
    return lrand48();
}
static __inline__ void srandom(unsigned int __s)
{
    srand48(__s);
}

/* Basic PTY functions.  These only work if devpts is mounted! */

extern int unlockpt(int);
extern char* ptsname(int);
extern int ptsname_r(int, char*, size_t);
extern int getpt(void);

static __inline__ int grantpt(int __fd __attribute((unused)))
{
  (void)__fd;
  return 0; /* devpts does this all for us! */
}

typedef struct {
    int quot;
    int rem;
} div_t;

extern div_t div(int, int);

typedef struct {
    long int quot;
    long int rem;
} ldiv_t;

extern ldiv_t ldiv(long, long);

typedef struct {
    long long int quot;
    long long int rem;
} lldiv_t;

extern lldiv_t lldiv(long long, long long);


/* make STLPort happy */
extern int mblen(const char *, size_t);
extern size_t mbstowcs(wchar_t *, const char *, size_t);
extern int mbtowc(wchar_t *, const char *, size_t);

/* Likewise, make libstdc++-v3 happy.  */
extern int wctomb(char *, wchar_t);
extern size_t wcstombs(char *, const wchar_t *, size_t);
# 183 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/stdlib.h"

# 54 "diag_comm_EXTif_OSA_LINUX.c" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/unistd.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
# 55 "diag_comm_EXTif_OSA_LINUX.c" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/pthread.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/time.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/cdefs.h" 1
/*	$NetBSD: cdefs.h,v 1.58 2004/12/11 05:59:00 christos Exp $	*/

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)cdefs.h	8.8 (Berkeley) 1/9/95
 */
# 32 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/time.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/time.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
# 33 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/time.h" 2


# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/arch-arm/asm/siginfo.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 36 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/time.h" 2




extern time_t time(time_t *);
extern int nanosleep(const struct timespec *, struct timespec *);

extern char *strtotimeval(const char *str, struct timeval *tv);

struct tm {
   int tm_sec; /* seconds */
   int tm_min; /* minutes */
   int tm_hour; /* hours */
   int tm_mday; /* day of the month */
   int tm_mon; /* month */
   int tm_year; /* year */
   int tm_wday; /* day of the week */
   int tm_yday; /* day in the year */
   int tm_isdst; /* daylight saving time */

   long int tm_gmtoff; /* Seconds east of UTC.  */
   const char *tm_zone; /* Timezone abbreviation.  */

};

/* defining TM_ZONE indicates that we have a "timezone abbreviation" field in
 * struct tm, the value should be the field name
 */


extern char* asctime(const struct tm* a);
extern char* asctime_r(const struct tm* a, char* buf);

/* Return the difference between TIME1 and TIME0.  */
extern double difftime (time_t __time1, time_t __time0);
extern time_t mktime (struct tm *a);

extern struct tm* localtime(const time_t *t);
extern struct tm* localtime_r(const time_t *timep, struct tm *result);

extern struct tm* gmtime(const time_t *timep);
extern struct tm* gmtime_r(const time_t *timep, struct tm *result);

extern char* strptime(const char *buf, const char *fmt, struct tm *tm);
extern size_t strftime(char *s, size_t max, const char *format, const struct tm *tm);

/* ANDROID-BEGIN */
struct strftime_locale {
    const char * mon[12];
    const char * month[12];
    const char * standalone_month[12];
    const char * wday[7];
    const char * weekday[7];
    const char * X_fmt;
    const char * x_fmt;
    const char * c_fmt;
    const char * am;
    const char * pm;
    const char * date_fmt;
};

extern size_t strftime_tz(char *s, size_t max, const char *format, const struct tm *tm, const struct strftime_locale* lc);
/* ANDROID-END */

extern char *ctime(const time_t *timep);
extern char *ctime_r(const time_t *timep, char *buf);

extern void tzset(void);

/* global includes */
extern char* tzname[];
extern int daylight;
extern long int timezone;



extern clock_t clock(void);

/* BIONIC: extra linux clock goodies */
extern int clock_getres(int, struct timespec *);
extern int clock_gettime(int, struct timespec *);
# 125 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/time.h"
extern int timer_create(int, struct sigevent*, timer_t*);
extern int timer_delete(timer_t);
extern int timer_settime(timer_t timerid, int flags, const struct itimerspec *value, struct itimerspec *ovalue);
extern int timer_gettime(timer_t timerid, struct itimerspec *value);
extern int timer_getoverrun(timer_t timerid);


# 32 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/pthread.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/signal.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
# 33 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/pthread.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sched.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/cdefs.h" 1
/*	$NetBSD: cdefs.h,v 1.58 2004/12/11 05:59:00 christos Exp $	*/

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)cdefs.h	8.8 (Berkeley) 1/9/95
 */
# 32 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sched.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/time.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
# 33 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sched.h" 2








struct sched_param {
    int sched_priority;
};

extern int sched_setscheduler(pid_t, int, const struct sched_param *);
extern int sched_getscheduler(pid_t);
extern int sched_yield(void);
extern int sched_get_priority_max(int policy);
extern int sched_get_priority_min(int policy);
extern int sched_setparam(pid_t, const struct sched_param *);
extern int sched_getparam(pid_t, struct sched_param *);
extern int sched_rr_get_interval(pid_t pid, struct timespec *tp);
# 76 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sched.h"

# 34 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/pthread.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/limits.h" 1
/*	$OpenBSD: limits.h,v 1.13 2005/12/31 19:29:38 millert Exp $	*/
/*	$NetBSD: limits.h,v 1.7 1994/10/26 00:56:00 cgd Exp $	*/

/*
 * Copyright (c) 1988 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)limits.h	5.9 (Berkeley) 4/3/91
 */
# 35 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/pthread.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/types.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
# 36 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/pthread.h" 2

/*
 * Types
 */
typedef struct
{
    int volatile value;
} pthread_mutex_t;





enum {
    PTHREAD_MUTEX_NORMAL = 0,
    PTHREAD_MUTEX_RECURSIVE = 1,
    PTHREAD_MUTEX_ERRORCHECK = 2,

    PTHREAD_MUTEX_ERRORCHECK_NP = PTHREAD_MUTEX_ERRORCHECK,
    PTHREAD_MUTEX_RECURSIVE_NP = PTHREAD_MUTEX_RECURSIVE,

    PTHREAD_MUTEX_DEFAULT = PTHREAD_MUTEX_NORMAL
};



typedef struct
{
    int volatile value;
} pthread_cond_t;

typedef struct
{
    uint32_t flags;
    void * stack_base;
    size_t stack_size;
    size_t guard_size;
    int32_t sched_policy;
    int32_t sched_priority;
} pthread_attr_t;

typedef long pthread_mutexattr_t;
typedef long pthread_condattr_t;

typedef int pthread_key_t;
typedef long pthread_t;

typedef volatile int pthread_once_t;

/*
 * Defines
 */
# 103 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/pthread.h"
/*
 * Prototypes
 */




int pthread_attr_init(pthread_attr_t * attr);
int pthread_attr_destroy(pthread_attr_t * attr);

int pthread_attr_setdetachstate(pthread_attr_t * attr, int state);
int pthread_attr_getdetachstate(pthread_attr_t const * attr, int * state);

int pthread_attr_setschedpolicy(pthread_attr_t * attr, int policy);
int pthread_attr_getschedpolicy(pthread_attr_t const * attr, int * policy);

int pthread_attr_setschedparam(pthread_attr_t * attr, struct sched_param const * param);
int pthread_attr_getschedparam(pthread_attr_t const * attr, struct sched_param * param);

int pthread_attr_setstacksize(pthread_attr_t * attr, size_t stack_size);
int pthread_attr_getstacksize(pthread_attr_t const * attr, size_t * stack_size);

int pthread_attr_setstackaddr(pthread_attr_t * attr, void * stackaddr);
int pthread_attr_getstackaddr(pthread_attr_t const * attr, void ** stackaddr);

int pthread_attr_setstack(pthread_attr_t * attr, void * stackaddr, size_t stack_size);
int pthread_attr_getstack(pthread_attr_t const * attr, void ** stackaddr, size_t * stack_size);

int pthread_attr_setguardsize(pthread_attr_t * attr, size_t guard_size);
int pthread_attr_getguardsize(pthread_attr_t const * attr, size_t * guard_size);

int pthread_attr_setscope(pthread_attr_t *attr, int scope);
int pthread_attr_getscope(pthread_attr_t const *attr);

int pthread_getattr_np(pthread_t thid, pthread_attr_t * attr);

int pthread_create(pthread_t *thread, pthread_attr_t const * attr,
                   void *(*start_routine)(void *), void * arg);
void pthread_exit(void * retval);
int pthread_join(pthread_t thid, void ** ret_val);
int pthread_detach(pthread_t thid);

pthread_t pthread_self(void);
int pthread_equal(pthread_t one, pthread_t two);

int pthread_getschedparam(pthread_t thid, int * policy,
                          struct sched_param * param);
int pthread_setschedparam(pthread_t thid, int poilcy,
                          struct sched_param const * param);

int pthread_mutexattr_init(pthread_mutexattr_t *attr);
int pthread_mutexattr_destroy(pthread_mutexattr_t *attr);
int pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *type);
int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type);
int pthread_mutexattr_setpshared(pthread_mutexattr_t *attr, int pshared);
int pthread_mutexattr_getpshared(pthread_mutexattr_t *attr, int *pshared);

int pthread_mutex_init(pthread_mutex_t *mutex,
                       const pthread_mutexattr_t *attr);
int pthread_mutex_destroy(pthread_mutex_t *mutex);
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);
int pthread_mutex_trylock(pthread_mutex_t *mutex);
int pthread_mutex_timedlock(pthread_mutex_t *mutex, struct timespec* ts);

int pthread_condattr_init(pthread_condattr_t *attr);
int pthread_condattr_getpshared(pthread_condattr_t *attr, int *pshared);
int pthread_condattr_setpshared(pthread_condattr_t* attr, int pshared);
int pthread_condattr_destroy(pthread_condattr_t *attr);

int pthread_cond_init(pthread_cond_t *cond,
                      const pthread_condattr_t *attr);
int pthread_cond_destroy(pthread_cond_t *cond);
int pthread_cond_broadcast(pthread_cond_t *cond);
int pthread_cond_signal(pthread_cond_t *cond);
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
int pthread_cond_timedwait(pthread_cond_t *cond,
                           pthread_mutex_t * mutex,
                           const struct timespec *abstime);

/* BIONIC: same as pthread_cond_timedwait, except the 'abstime' given refers
 *         to the CLOCK_MONOTONIC clock instead, to avoid any problems when
 *         the wall-clock time is changed brutally
 */
int pthread_cond_timedwait_monotonic_np(pthread_cond_t *cond,
                                        pthread_mutex_t *mutex,
                                        const struct timespec *abstime);

/* BIONIC: DEPRECATED. same as pthread_cond_timedwait_monotonic_np()
 * unfortunately pthread_cond_timedwait_monotonic has shipped already
 */
int pthread_cond_timedwait_monotonic(pthread_cond_t *cond,
                                     pthread_mutex_t *mutex,
                                     const struct timespec *abstime);



/* BIONIC: same as pthread_cond_timedwait, except the 'reltime' given refers
 *         is relative to the current time.
 */
int pthread_cond_timedwait_relative_np(pthread_cond_t *cond,
                                     pthread_mutex_t *mutex,
                                     const struct timespec *reltime);





int pthread_cond_timeout_np(pthread_cond_t *cond,
                            pthread_mutex_t * mutex,
                            unsigned msecs);

/* same as pthread_mutex_lock(), but will wait up to 'msecs' milli-seconds
 * before returning. same return values than pthread_mutex_trylock though, i.e.
 * returns EBUSY if the lock could not be acquired after the timeout
 * expired.
 */
int pthread_mutex_lock_timeout_np(pthread_mutex_t *mutex, unsigned msecs);

/* read-write lock support */

typedef int pthread_rwlockattr_t;

typedef struct {
    pthread_mutex_t lock;
    pthread_cond_t cond;
    int numLocks;
    int writerThreadId;
    int pendingReaders;
    int pendingWriters;
    void* reserved[4]; /* for future extensibility */
} pthread_rwlock_t;



int pthread_rwlockattr_init(pthread_rwlockattr_t *attr);
int pthread_rwlockattr_destroy(pthread_rwlockattr_t *attr);
int pthread_rwlockattr_setpshared(pthread_rwlockattr_t *attr, int pshared);
int pthread_rwlockattr_getpshared(pthread_rwlockattr_t *attr, int *pshared);

int pthread_rwlock_init(pthread_rwlock_t *rwlock, const pthread_rwlockattr_t *attr);
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);

int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_timedrdlock(pthread_rwlock_t *rwlock, const struct timespec *abs_timeout);

int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_timedwrlock(pthread_rwlock_t *rwlock, const struct timespec *abs_timeout);

int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);


int pthread_key_create(pthread_key_t *key, void (*destructor_function)(void *));
int pthread_key_delete (pthread_key_t);
int pthread_setspecific(pthread_key_t key, const void *value);
void *pthread_getspecific(pthread_key_t key);

int pthread_kill(pthread_t tid, int sig);
int pthread_sigmask(int how, const sigset_t *set, sigset_t *oset);

int pthread_getcpuclockid(pthread_t tid, clockid_t *clockid);

int pthread_once(pthread_once_t *once_control, void (*init_routine)(void));

int pthread_setname_np(pthread_t thid, const char *thname);

typedef void (*__pthread_cleanup_func_t)(void*);

typedef struct __pthread_cleanup_t {
    struct __pthread_cleanup_t* __cleanup_prev;
    __pthread_cleanup_func_t __cleanup_routine;
    void* __cleanup_arg;
} __pthread_cleanup_t;

extern void __pthread_cleanup_push(__pthread_cleanup_t* c,
                                    __pthread_cleanup_func_t routine,
                                    void* arg);

extern void __pthread_cleanup_pop(__pthread_cleanup_t* c,
                                   int execute);

/* Believe or not, the definitions of pthread_cleanup_push and
 * pthread_cleanup_pop below are correct. Posix states that these
 * can be implemented as macros that might introduce opening and
 * closing braces, and that using setjmp/longjmp/return/break/continue
 * between them results in undefined behaviour.
 *
 * And indeed, GLibc and other C libraries use a similar definition
 */
# 307 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/pthread.h"
/************ TO FIX ************/
# 56 "diag_comm_EXTif_OSA_LINUX.c" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/termios.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/cdefs.h" 1
/*	$NetBSD: cdefs.h,v 1.58 2004/12/11 05:59:00 christos Exp $	*/

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)cdefs.h	8.8 (Berkeley) 1/9/95
 */
# 32 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/termios.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/ioctl.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/cdefs.h" 1
/*	$NetBSD: cdefs.h,v 1.58 2004/12/11 05:59:00 christos Exp $	*/

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)cdefs.h	8.8 (Berkeley) 1/9/95
 */
# 32 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/ioctl.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/ioctl.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/arch-arm/asm/ioctl.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/asm-generic/ioctl.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 36 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/asm-generic/ioctl.h"
extern unsigned int __invalid_size_argument_for_IOC;
# 12 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/arch-arm/asm/ioctl.h" 2
# 16 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/ioctl.h" 2
# 33 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/ioctl.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/arch-arm/asm/ioctls.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/arch-arm/asm/ioctl.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/asm-generic/ioctl.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 12 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/arch-arm/asm/ioctl.h" 2
# 16 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/arch-arm/asm/ioctls.h" 2
# 34 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/ioctl.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/arch-arm/asm/termbits.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/



typedef unsigned char cc_t;
typedef unsigned int speed_t;
typedef unsigned int tcflag_t;


struct termios {
 tcflag_t c_iflag;
 tcflag_t c_oflag;
 tcflag_t c_cflag;
 tcflag_t c_lflag;
 cc_t c_line;
 cc_t c_cc[19];
};
# 35 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/ioctl.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/ioctl_compat.h" 1
/*	$NetBSD: ioctl_compat.h,v 1.15 2005/12/03 17:10:46 christos Exp $	*/

/*
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)ioctl_compat.h	8.4 (Berkeley) 1/21/94
 */




/*#include <sys/ttychars.h>*/
/*#include <sys/ttydev.h>*/

struct tchars {
 char t_intrc; /* interrupt */
 char t_quitc; /* quit */
 char t_startc; /* start output */
 char t_stopc; /* stop output */
 char t_eofc; /* end-of-file */
 char t_brkc; /* input delimiter (like nl) */
};

struct ltchars {
 char t_suspc; /* stop process signal */
 char t_dsuspc; /* delayed stop process signal */
 char t_rprntc; /* reprint line */
 char t_flushc; /* flush output (toggles) */
 char t_werasc; /* word erase */
 char t_lnextc; /* literal next character */
};

/*
 * Structure for TIOCGETP and TIOCSETP ioctls.
 */


struct sgttyb {
 char sg_ispeed; /* input speed */
 char sg_ospeed; /* output speed */
 char sg_erase; /* erase character */
 char sg_kill; /* kill character */
 short sg_flags; /* mode flags */
};
# 36 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/ioctl.h" 2



extern int ioctl(int, int, ...);


# 33 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/termios.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/types.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
# 34 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/termios.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/stdint.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
# 35 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/termios.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/termios.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/types.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 16 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/termios.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/arch-arm/asm/termios.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/arch-arm/asm/termbits.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 16 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/arch-arm/asm/termios.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/arch-arm/asm/ioctls.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 17 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/arch-arm/asm/termios.h" 2

struct winsize {
 unsigned short ws_row;
 unsigned short ws_col;
 unsigned short ws_xpixel;
 unsigned short ws_ypixel;
};


struct termio {
 unsigned short c_iflag;
 unsigned short c_oflag;
 unsigned short c_cflag;
 unsigned short c_lflag;
 unsigned char c_line;
 unsigned char c_cc[8];
};
# 17 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/termios.h" 2
# 36 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/termios.h" 2



/* Redefine these to match their ioctl number */
# 49 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/termios.h"
static __inline__ int tcgetattr(int fd, struct termios *s)
{
    return ioctl(fd, 0x5401, s);
}

static __inline__ int tcsetattr(int fd, int __opt, const struct termios *s)
{
    return ioctl(fd, __opt, (void *)s);
}

static __inline__ int tcflow(int fd, int action)
{
    return ioctl(fd, 0x540A, (void *)(intptr_t)action);
}

static __inline__ int tcflush(int fd, int __queue)
{
    return ioctl(fd, 0x540B, (void *)(intptr_t)__queue);
}

static __inline__ pid_t tcgetsid(int fd)
{
    pid_t _pid;
    return ioctl(fd, 0x5429, &_pid) ? (pid_t)-1 : _pid;
}

static __inline__ int tcsendbreak(int fd, int __duration)
{
    return ioctl(fd, 0x5425, (void *)(uintptr_t)__duration);
}

static __inline__ speed_t cfgetospeed(const struct termios *s)
{
    return (speed_t)(s->c_cflag & 0010017);
}

static __inline__ int cfsetospeed(struct termios *s, speed_t speed)
{
    s->c_cflag = (s->c_cflag & ~0010017) | (speed & 0010017);
    return 0;
}

static __inline__ speed_t cfgetispeed(const struct termios *s)
{
    return (speed_t)(s->c_cflag & 0010017);
}

static __inline__ int cfsetispeed(struct termios *s, speed_t speed)
{
    s->c_cflag = (s->c_cflag & ~0010017) | (speed & 0010017);
  return 0;
}

static __inline__ void cfmakeraw(struct termios *s)
{
    s->c_iflag &= ~(0000001|0000002|0000010|0000040|0000100|0000200|0000400|0002000);
    s->c_oflag &= ~0000001;
    s->c_lflag &= ~(0000010|0000100|0000002|0000001|0100000);
    s->c_cflag &= ~(0000060|0000400);
    s->c_cflag |= 0000060;
}


# 57 "diag_comm_EXTif_OSA_LINUX.c" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/fcntl.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
# 58 "diag_comm_EXTif_OSA_LINUX.c" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/errno.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/cdefs.h" 1
/*	$NetBSD: cdefs.h,v 1.58 2004/12/11 05:59:00 christos Exp $	*/

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)cdefs.h	8.8 (Berkeley) 1/9/95
 */
# 32 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/errno.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/errno.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/arch-arm/asm/errno.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/asm-generic/errno.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/asm-generic/errno-base.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 16 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/asm-generic/errno.h" 2
# 16 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/arch-arm/asm/errno.h" 2
# 16 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/errno.h" 2
# 33 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/errno.h" 2



/* on Linux, ENOTSUP and EOPNOTSUPP are defined as the same error code
 * even if 1000.3 states that they should be different
 */




/* internal function that should *only* be called from system calls */
/* use errno = xxxx instead in C code                               */
extern int __set_errno(int error);

/* internal function returning the address of the thread-specific errno */
extern volatile int* __errno(void);

/* a macro expanding to the errno l-value */



# 59 "diag_comm_EXTif_OSA_LINUX.c" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/arch-arm/asm/types.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 60 "diag_comm_EXTif_OSA_LINUX.c" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/ioctl.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
# 61 "diag_comm_EXTif_OSA_LINUX.c" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/select.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
# 62 "diag_comm_EXTif_OSA_LINUX.c" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/time.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
# 63 "diag_comm_EXTif_OSA_LINUX.c" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/netinet/in.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/arch-arm/include/endian.h" 1
/*	$OpenBSD: endian.h,v 1.3 2005/12/13 00:35:23 millert Exp $	*/







# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/types.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
# 10 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/arch-arm/include/endian.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/endian.h" 1
/*	$OpenBSD: endian.h,v 1.17 2006/01/06 18:53:05 millert Exp $	*/

/*-
 * Copyright (c) 1997 Niklas Hallqvist.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Generic definitions for little- and big-endian systems.  Other endianesses
 * has to be dealt with in the specific machine/endian.h file for that port.
 *
 * This file is meant to be included from a little- or big-endian port's
 * machine/endian.h after setting _BYTE_ORDER to either 1234 for little endian
 * or 4321 for big..
 */
# 10 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/arch-arm/include/endian.h" 2
# 32 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/netinet/in.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/socket.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/






struct __kernel_sockaddr_storage {
 unsigned short ss_family;

 char __data[128 - sizeof(unsigned short)];

} __attribute__ ((aligned((__alignof__ (struct sockaddr *)))));



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/arch-arm/asm/socket.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/arch-arm/asm/sockios.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 16 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/arch-arm/asm/socket.h" 2
# 28 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/socket.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/sockios.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/arch-arm/asm/sockios.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 16 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/sockios.h" 2
# 29 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/socket.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/uio.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/compiler.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 16 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/uio.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/types.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 17 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/uio.h" 2

struct iovec
{
 void *iov_base;
 __kernel_size_t iov_len;
};
# 30 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/socket.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/types.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 31 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/socket.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/compiler.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 32 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/socket.h" 2

typedef unsigned short sa_family_t;

struct sockaddr {
 sa_family_t sa_family;
 char sa_data[14];
};

struct linger {
 int l_onoff;
 int l_linger;
};



struct msghdr {
 void * msg_name;
 int msg_namelen;
 struct iovec * msg_iov;
 __kernel_size_t msg_iovlen;
 void * msg_control;
 __kernel_size_t msg_controllen;
 unsigned msg_flags;
};

struct cmsghdr {
 __kernel_size_t cmsg_len;
 int cmsg_level;
 int cmsg_type;
};
# 84 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/socket.h"
static __inline__ struct cmsghdr * __cmsg_nxthdr(void *__ctl, __kernel_size_t __size,
 struct cmsghdr *__cmsg)
{
 struct cmsghdr * __ptr;

 __ptr = (struct cmsghdr*)(((unsigned char *) __cmsg) + ( ((__cmsg->cmsg_len)+sizeof(long)-1) & ~(sizeof(long)-1) ));
 if ((unsigned long)((char*)(__ptr+1) - (char *) __ctl) > __size)
 return (struct cmsghdr *)0;

 return __ptr;
}

static __inline__ struct cmsghdr * cmsg_nxthdr (struct msghdr *__msg, struct cmsghdr *__cmsg)
{
 return __cmsg_nxthdr(__msg->msg_control, __msg->msg_controllen, __cmsg);
}





struct ucred {
 __u32 pid;
 __u32 uid;
 __u32 gid;
};
# 33 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/netinet/in.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/in.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/types.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 16 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/in.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/socket.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 17 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/in.h" 2

enum {
 IPPROTO_IP = 0,
 IPPROTO_ICMP = 1,
 IPPROTO_IGMP = 2,
 IPPROTO_IPIP = 4,
 IPPROTO_TCP = 6,
 IPPROTO_EGP = 8,
 IPPROTO_PUP = 12,
 IPPROTO_UDP = 17,
 IPPROTO_IDP = 22,
 IPPROTO_DCCP = 33,
 IPPROTO_RSVP = 46,
 IPPROTO_GRE = 47,

 IPPROTO_IPV6 = 41,

 IPPROTO_ESP = 50,
 IPPROTO_AH = 51,
 IPPROTO_PIM = 103,

 IPPROTO_COMP = 108,
 IPPROTO_SCTP = 132,

 IPPROTO_RAW = 255,
 IPPROTO_MAX
};

struct in_addr {
 __u32 s_addr;
};
# 98 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/in.h"
struct ip_mreq
{
 struct in_addr imr_multiaddr;
 struct in_addr imr_interface;
};

struct ip_mreqn
{
 struct in_addr imr_multiaddr;
 struct in_addr imr_address;
 int imr_ifindex;
};

struct ip_mreq_source {
 __u32 imr_multiaddr;
 __u32 imr_interface;
 __u32 imr_sourceaddr;
};

struct ip_msfilter {
 __u32 imsf_multiaddr;
 __u32 imsf_interface;
 __u32 imsf_fmode;
 __u32 imsf_numsrc;
 __u32 imsf_slist[1];
};



struct group_req
{
 __u32 gr_interface;
 struct __kernel_sockaddr_storage gr_group;
};

struct group_source_req
{
 __u32 gsr_interface;
 struct __kernel_sockaddr_storage gsr_group;
 struct __kernel_sockaddr_storage gsr_source;
};

struct group_filter
{
 __u32 gf_interface;
 struct __kernel_sockaddr_storage gf_group;
 __u32 gf_fmode;
 __u32 gf_numsrc;
 struct __kernel_sockaddr_storage gf_slist[1];
};



struct in_pktinfo
{
 int ipi_ifindex;
 struct in_addr ipi_spec_dst;
 struct in_addr ipi_addr;
};


struct sockaddr_in {
 sa_family_t sin_family;
 unsigned short int sin_port;
 struct in_addr sin_addr;

 unsigned char __pad[16 - sizeof(short int) -
 sizeof(unsigned short int) - sizeof(struct in_addr)];
};
# 209 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/in.h"
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/arch-arm/asm/byteorder.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/compiler.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 16 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/arch-arm/asm/byteorder.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/arch-arm/asm/types.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 17 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/arch-arm/asm/byteorder.h" 2

static inline __u32 ___arch__swab32(__u32 x)
{
 __u32 t;







 t = x ^ ((x << 16) | (x >> 16));

 x = (x << 24) | (x >> 8);
 t &= ~0x00FF0000;
 x ^= (t >> 8);

 return x;
}
# 47 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/arch-arm/asm/byteorder.h"
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/byteorder/little_endian.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 22 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/byteorder/little_endian.h"
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/types.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 23 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/byteorder/little_endian.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/byteorder/swab.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/compiler.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 16 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/byteorder/swab.h" 2
# 24 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/byteorder/little_endian.h" 2
# 66 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/byteorder/little_endian.h"
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/byteorder/generic.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 67 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/byteorder/little_endian.h" 2
# 48 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/arch-arm/asm/byteorder.h" 2
# 210 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/in.h" 2
# 34 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/netinet/in.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/in6.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/types.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 16 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/in6.h" 2

struct in6_addr
{
 union
 {
 __u8 u6_addr8[16];
 __u16 u6_addr16[8];
 __u32 u6_addr32[4];
 } in6_u;



};



struct sockaddr_in6 {
 unsigned short int sin6_family;
 __u16 sin6_port;
 __u32 sin6_flowinfo;
 struct in6_addr sin6_addr;
 __u32 sin6_scope_id;
};

struct ipv6_mreq {

 struct in6_addr ipv6mr_multiaddr;

 int ipv6mr_ifindex;
};



struct in6_flowlabel_req
{
 struct in6_addr flr_dst;
 __u32 flr_label;
 __u8 flr_action;
 __u8 flr_share;
 __u16 flr_flags;
 __u16 flr_expires;
 __u16 flr_linger;
 __u32 __flr_pad;

};
# 35 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/netinet/in.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/netinet/in6.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/in6.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 32 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/netinet/in6.h" 2
# 63 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/netinet/in6.h"
/* RFC 4193. */
# 36 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/netinet/in.h" 2





extern int bindresvport (int sd, struct sockaddr_in *sin);

static const struct in6_addr in6addr_any = {{{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}}};
static const struct in6_addr in6addr_loopback = { { { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 } } };


# 64 "diag_comm_EXTif_OSA_LINUX.c" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/un.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/un.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/





struct sockaddr_un {
 sa_family_t sun_family;
 char sun_path[108];
};
# 32 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/un.h" 2
# 65 "diag_comm_EXTif_OSA_LINUX.c" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/netdb.h" 1
/*-
 * Copyright (c) 1980, 1983, 1988, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * -
 * Portions Copyright (c) 1993 by Digital Equipment Corporation.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies, and that
 * the name of Digital Equipment Corporation not be used in advertising or
 * publicity pertaining to distribution of the document or software without
 * specific, written prior permission.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND DIGITAL EQUIPMENT CORP. DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS.   IN NO EVENT SHALL DIGITAL EQUIPMENT
 * CORPORATION BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 * -
 * --Copyright--
 */

/*
 *      @(#)netdb.h	8.1 (Berkeley) 6/2/93
 *      From: Id: netdb.h,v 8.9 1996/11/19 08:39:29 vixie Exp $
 * $FreeBSD: /repoman/r/ncvs/src/include/netdb.h,v 1.41 2006/04/15 16:20:26 ume Exp $
 */




# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/cdefs.h" 1
/*	$NetBSD: cdefs.h,v 1.58 2004/12/11 05:59:00 christos Exp $	*/

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)cdefs.h	8.8 (Berkeley) 1/9/95
 */
# 65 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/netdb.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/types.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
# 66 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/netdb.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/socket.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/cdefs.h" 1
/*	$NetBSD: cdefs.h,v 1.58 2004/12/11 05:59:00 christos Exp $	*/

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)cdefs.h	8.8 (Berkeley) 1/9/95
 */
# 32 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/socket.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/types.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
# 33 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/socket.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/socket.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 34 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/socket.h" 2


# 50 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/socket.h"
/* BIONIC: second argument to shutdown() */
enum {
    SHUT_RD = 0, /* no more receptions */

    SHUT_WR, /* no more transmissions */

    SHUT_RDWR /* no more receptions or transmissions */

};


typedef int socklen_t;

extern int socket(int, int, int);
extern int bind(int, const struct sockaddr *, int);
extern int connect(int, const struct sockaddr *, socklen_t);
extern int listen(int, int);
extern int accept(int, struct sockaddr *, socklen_t *);
extern int getsockname(int, struct sockaddr *, socklen_t *);
extern int getpeername(int, struct sockaddr *, socklen_t *);
extern int socketpair(int, int, int, int *);
extern int shutdown(int, int);
extern int setsockopt(int, int, int, const void *, socklen_t);
extern int getsockopt(int, int, int, void *, socklen_t *);
extern int sendmsg(int, const struct msghdr *, unsigned int);
extern int recvmsg(int, struct msghdr *, unsigned int);

extern ssize_t send(int, const void *, size_t, unsigned int);
extern ssize_t recv(int, void *, size_t, unsigned int);

extern ssize_t sendto(int, const void *, size_t, int, const struct sockaddr *, socklen_t);
extern ssize_t recvfrom(int, void *, size_t, unsigned int, const struct sockaddr *, socklen_t *);




# 67 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/netdb.h" 2
# 79 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/netdb.h"
/*
 * Structures returned by network data base library.  All addresses are
 * supplied in host order, and returned in network order (suitable for
 * use in system calls).
 */
struct hostent {
 char *h_name; /* official name of host */
 char **h_aliases; /* alias list */
 int h_addrtype; /* host address type */
 int h_length; /* length of address */
 char **h_addr_list; /* list of addresses from name server */

};

struct netent {
 char *n_name; /* official name of net */
 char **n_aliases; /* alias list */
 int n_addrtype; /* net address type */
 uint32_t n_net; /* network # */
};

struct servent {
 char *s_name; /* official service name */
 char **s_aliases; /* alias list */
 int s_port; /* port # */
 char *s_proto; /* protocol to use */
};

struct protoent {
 char *p_name; /* official protocol name */
 char **p_aliases; /* alias list */
 int p_proto; /* protocol # */
};

struct addrinfo {
 int ai_flags; /* AI_PASSIVE, AI_CANONNAME, AI_NUMERICHOST */
 int ai_family; /* PF_xxx */
 int ai_socktype; /* SOCK_xxx */
 int ai_protocol; /* 0 or IPPROTO_xxx for IPv4 and IPv6 */
 socklen_t ai_addrlen; /* length of ai_addr */
 char *ai_canonname; /* canonical name for hostname */
 struct sockaddr *ai_addr; /* binary address */
 struct addrinfo *ai_next; /* next structure in linked list */
};

/*
 * Error return codes from gethostbyname() and gethostbyaddr()
 * (left in h_errno).
 */
# 137 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/netdb.h"
/*
 * Error return codes from getaddrinfo()
 */
# 159 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/netdb.h"
/*
 * Flag values for getaddrinfo()
 */




/* valid flags for addrinfo (not a standard def, apps should not use it) */
# 175 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/netdb.h"
/* special recommended flags for getipnodebyname */


/*
 * Constants for getnameinfo()
 */



/*
 * Flag values for getnameinfo()
 */
# 196 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/netdb.h"
/*
 * Scope delimit character
 */



/* BIONIC-BEGIN */

int* __get_h_errno(void);
/* BIONIC-END */
void endhostent(void);
void endnetent(void);
void endnetgrent(void);
void endprotoent(void);
void endservent(void);
void freehostent(struct hostent *);
struct hostent *gethostbyaddr(const char *, int, int);
int gethostbyaddr_r(const char *, int, int, struct hostent *, char *, size_t, struct hostent **, int *);
struct hostent *gethostbyname(const char *);
int gethostbyname_r(const char *, struct hostent *, char *, size_t, struct hostent **, int *);
struct hostent *gethostbyname2(const char *, int);
int gethostbyname2_r(const char *, int, struct hostent *, char *, size_t, struct hostent **, int *);
struct hostent *gethostent(void);
int gethostent_r(struct hostent *, char *, size_t, struct hostent **, int *);
struct hostent *getipnodebyaddr(const void *, size_t, int, int *);
struct hostent *getipnodebyname(const char *, int, int, int *);
struct netent *getnetbyaddr(uint32_t, int);
int getnetbyaddr_r(uint32_t, int, struct netent *, char *, size_t, struct netent**, int *);
struct netent *getnetbyname(const char *);
int getnetbyname_r(const char *, struct netent *, char *, size_t, struct netent **, int *);
struct netent *getnetent(void);
int getnetent_r(struct netent *, char *, size_t, struct netent **, int *);
int getnetgrent(char **, char **, char **);
struct protoent *getprotobyname(const char *);
int getprotobyname_r(const char *, struct protoent *, char *, size_t, struct protoent **);
struct protoent *getprotobynumber(int);
int getprotobynumber_r(int, struct protoent *, char *, size_t, struct protoent **);
struct protoent *getprotoent(void);
int getprotoent_r(struct protoent *, char *, size_t, struct protoent **);
struct servent *getservbyname(const char *, const char *);
struct servent *getservbyport(int, const char *);
struct servent *getservent(void);
void herror(const char *);
const char *hstrerror(int);
int innetgr(const char *, const char *, const char *, const char *);
void sethostent(int);
void setnetent(int);
void setprotoent(int);
int getaddrinfo(const char *, const char *, const struct addrinfo *, struct addrinfo **);
int getnameinfo(const struct sockaddr *, socklen_t, char *, size_t, char *, size_t, int);
void freeaddrinfo(struct addrinfo *);
const char *gai_strerror(int);
void setnetgrent(const char *);
void setservent(int);


# 66 "diag_comm_EXTif_OSA_LINUX.c" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/types.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
# 67 "diag_comm_EXTif_OSA_LINUX.c" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/netlink.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/socket.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 16 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/netlink.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/types.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 17 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/netlink.h" 2
# 41 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/netlink.h"
struct net;

struct sockaddr_nl
{
 sa_family_t nl_family;
 unsigned short nl_pad;
 __u32 nl_pid;
 __u32 nl_groups;
};

struct nlmsghdr
{
 __u32 nlmsg_len;
 __u16 nlmsg_type;
 __u16 nlmsg_flags;
 __u32 nlmsg_seq;
 __u32 nlmsg_pid;
};
# 92 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/netlink.h"
struct nlmsgerr
{
 int error;
 struct nlmsghdr msg;
};







struct nl_pktinfo
{
 __u32 group;
};



enum {
 NETLINK_UNCONNECTED = 0,
 NETLINK_CONNECTED,
};

struct nlattr
{
 __u16 nla_len;
 __u16 nla_type;
};
# 68 "diag_comm_EXTif_OSA_LINUX.c" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/net/if.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/if.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/types.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 16 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/if.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/socket.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 17 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/if.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/compiler.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 18 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/if.h" 2


# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/hdlc/ioctl.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/



typedef struct {
 unsigned int clock_rate;
 unsigned int clock_type;
 unsigned short loopback;
} sync_serial_settings;

typedef struct {
 unsigned int clock_rate;
 unsigned int clock_type;
 unsigned short loopback;
 unsigned int slot_map;
} te1_settings;

typedef struct {
 unsigned short encoding;
 unsigned short parity;
} raw_hdlc_proto;

typedef struct {
 unsigned int t391;
 unsigned int t392;
 unsigned int n391;
 unsigned int n392;
 unsigned int n393;
 unsigned short lmi;
 unsigned short dce;
} fr_proto;

typedef struct {
 unsigned int dlci;
} fr_proto_pvc;

typedef struct {
 unsigned int dlci;
 char master[16];
}fr_proto_pvc_info;

typedef struct {
 unsigned int interval;
 unsigned int timeout;
} cisco_proto;
# 21 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/if.h" 2
# 78 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/if.h"
enum {
 IF_OPER_UNKNOWN,
 IF_OPER_NOTPRESENT,
 IF_OPER_DOWN,
 IF_OPER_LOWERLAYERDOWN,
 IF_OPER_TESTING,
 IF_OPER_DORMANT,
 IF_OPER_UP,
};

enum {
 IF_LINK_MODE_DEFAULT,
 IF_LINK_MODE_DORMANT,
};

struct ifmap
{
 unsigned long mem_start;
 unsigned long mem_end;
 unsigned short base_addr;
 unsigned char irq;
 unsigned char dma;
 unsigned char port;

};

struct if_settings
{
 unsigned int type;
 unsigned int size;
 union {

 raw_hdlc_proto *raw_hdlc;
 cisco_proto *cisco;
 fr_proto *fr;
 fr_proto_pvc *fr_pvc;
 fr_proto_pvc_info *fr_pvc_info;

 sync_serial_settings *sync;
 te1_settings *te1;
 } ifs_ifsu;
};

struct ifreq
{

 union
 {
 char ifrn_name[16];
 } ifr_ifrn;

 union {
 struct sockaddr ifru_addr;
 struct sockaddr ifru_dstaddr;
 struct sockaddr ifru_broadaddr;
 struct sockaddr ifru_netmask;
 struct sockaddr ifru_hwaddr;
 short ifru_flags;
 int ifru_ivalue;
 int ifru_mtu;
 struct ifmap ifru_map;
 char ifru_slave[16];
 char ifru_newname[16];
 void * ifru_data;
 struct if_settings ifru_settings;
 } ifr_ifru;
};
# 164 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/if.h"
struct ifconf
{
 int ifc_len;
 union
 {
 char *ifcu_buf;
 struct ifreq *ifcu_req;
 } ifc_ifcu;
};
# 29 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/net/if.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/cdefs.h" 1
/*	$NetBSD: cdefs.h,v 1.58 2004/12/11 05:59:00 christos Exp $	*/

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)cdefs.h	8.8 (Berkeley) 1/9/95
 */
# 30 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/net/if.h" 2






/*
 * Map an interface name into its corresponding index.
 */
extern unsigned int if_nametoindex(const char *);
extern char* if_indextoname(unsigned ifindex, char *ifname);


# 69 "diag_comm_EXTif_OSA_LINUX.c" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/ctype.h" 1
/*	$OpenBSD: ctype.h,v 1.19 2005/12/13 00:35:22 millert Exp $	*/
/*	$NetBSD: ctype.h,v 1.14 1994/10/26 00:55:47 cgd Exp $	*/

/*
 * Copyright (c) 1989 The Regents of the University of California.
 * All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)ctype.h	5.3 (Berkeley) 4/3/91
 */




# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/cdefs.h" 1
/*	$NetBSD: cdefs.h,v 1.58 2004/12/11 05:59:00 christos Exp $	*/

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)cdefs.h	8.8 (Berkeley) 1/9/95
 */
# 44 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/ctype.h" 2
# 54 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/ctype.h"


extern const char *_ctype_;
extern const short *_tolower_tab_;
extern const short *_toupper_tab_;

/* extern __inline is a GNU C extension */
# 72 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/ctype.h"
int isalnum(int);
int isalpha(int);
int iscntrl(int);
int isdigit(int);
int isgraph(int);
int islower(int);
int isprint(int);
int ispunct(int);
int isspace(int);
int isupper(int);
int isxdigit(int);
int tolower(int);
int toupper(int);



int isblank(int);



int isascii(int);
int toascii(int);
int _tolower(int);
int _toupper(int);
# 203 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/ctype.h"

# 70 "diag_comm_EXTif_OSA_LINUX.c" 2

//#include "osa.h"
//#include "global_types.h"
//#include "ICAT_config.h"
//#include "diag_config.h"
//#include "diag_osif.h"
//#include "diag_init.h"

# 1 "../../include/global_types.h" 1
/******************************************************************************

*(C) Copyright 2007 Marvell International Ltd.

* All Rights Reserved

******************************************************************************/
/****************************************************************************
 *
 * Name:          global_types.h
 *
 * Description:   Standard type definitions
 *
 ****************************************************************************
 *
 *
 *
 *
 ****************************************************************************
 *                  Copyright (c) Intel 2000
 ***************************************************************************/





# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/stdio.h" 1
/*	$OpenBSD: stdio.h,v 1.35 2006/01/13 18:10:09 miod Exp $	*/
/*	$NetBSD: stdio.h,v 1.18 1996/04/25 18:29:21 jtc Exp $	*/

/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Chris Torek.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)stdio.h	5.17 (Berkeley) 6/3/91
 */
# 28 "../../include/global_types.h" 2

# 1 "../../include/gbl_types.h" 1
/* ===========================================================================
   File        : gbl_types.h
   Description : Global types file.

   Notes       : This file includes the types from the correct environment.
	      The environment is set via the ENV_<ENV> macro. This macro
	      is usually set in /env/<host>/build/<env>_env.mak.

   Copyright 2001, Intel Corporation, All rights reserved.
   =========================================================================== */






/* Use the Xscale environment types */




/* Use the Manitoba environment types */




/* Use the Arm environment types */




/* Use the Gnu environment types */




/* Use the Microsoft Visual C environment types */




/* Use the Microsoft Smartphone environment types */




/* Use the Symbian environment types */




/* Use the POSIX environment types */






/*                         end of gbl_types.h
   --------------------------------------------------------------------------- */
# 30 "../../include/global_types.h" 2
# 1 "../../include/linux_types.h" 1
/* ===========================================================================
   File        : posix_types.h
   Description : Global types file for the POSIX Linux environment.

   Notes       : This file is designed for use in the gnu environment
	      and is referenced from the gbl_types.h file. Use of
			  this file requires ENV_POSIX to be defined in posix_env.mak.

   Copyright 2005, Intel Corporation, All rights reserved.
   =========================================================================== */
# 23 "../../include/linux_types.h"
typedef unsigned char BOOL;
typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned int UINT32;

typedef signed char CHAR;
typedef signed char INT8;
typedef signed short INT16;
typedef signed int INT32;

typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef int HANDLE;
typedef HANDLE* LPHANDLE;
typedef unsigned char* PUINT8;
typedef long LONG;
typedef char* LPCTSTR;
typedef char* LPTSTR;
typedef void* LPVOID;
typedef unsigned int* LPDWORD;
typedef unsigned int* PDWORD;
typedef unsigned int* PUINT32;
typedef unsigned short TCHAR;
typedef unsigned int UINT;

typedef INT32 *PINT32;
//typedef UINT32  *PUINT32;
typedef INT16 *PINT16;
typedef UINT16 *PUINT16;
typedef INT8 *PINT8;
//typedef UINT8   *PUINT8;
# 74 "../../include/linux_types.h"
/*                         end of posix_types.h
   --------------------------------------------------------------------------- */
# 31 "../../include/global_types.h" 2

///////////////////////////////////////////////////////////////////////////
// UPDATE - Wrap Definition for Linux
///////////////////////////////////////////////////////////////////////////
# 52 "../../include/global_types.h"
///////////////////////////////////////////////////////////////////////////
// END OF UPDATE
///////////////////////////////////////////////////////////////////////////


/* Standard typedefs */
//typedef unsigned char   BOOL;         /* Boolean                        */
# 78 "../../include/global_types.h"
//typedef UINT8           BYTE;         /* Unsigned 8-bit quantity        */
//typedef UINT16          WORD;         /* Unsigned 16-bit quantity       */
//typedef UINT32          DWORD;        /* Unsigned 32-bit quantity       */

typedef const char * SwVersion;

/*  #define FALSE   0
 #define TRUE    1

 #define YES     TRUE
 #define NO      FALSE

 #define ON      1
 #define OFF     0
 */
/* Definition of NULL is required */
//#define NULL    0






/* Handy macros */



/* Bit fields macros */
# 79 "diag_comm_EXTif_OSA_LINUX.c" 2

# 1 "diag_config.h" 1
/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code ("Material") are owned
   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
   transmitted, distributed, or disclosed in any way without Intel's prior express written permission.

   No license under any patent, copyright, trade secret or other intellectual property right is granted to or
   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
   Intel in writing.
   -------------------------------------------------------------------------------------------------------------------*/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*                                                                                 *
*     File name:      Diag_config .h                                              *
*     Programmer:     Shiri Dolev                                                 *
*                                                                                 *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*                                                                                 *
*       Create Date:  November, 2003                                              *
*                                                                                 *
*       Description: DIAG Configuration File.                                                                      *
*                                                                                 *
*       Notes:                                                                    *
*                                                                                 *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */




///#include "osa.h"
# 1 "diag_osif.h" 1
/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/
# 13 "diag_osif.h"
/////#include "osa.h"






//ADD TEMP!!!!!!!!!!!!!!!!!!!!!!!!


// specific OS defines







# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/assert.h" 1
/*	$OpenBSD: assert.h,v 1.12 2006/01/31 10:53:51 hshoexer Exp $	*/
/*	$NetBSD: assert.h,v 1.6 1994/10/26 00:55:44 cgd Exp $	*/

/*-
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)assert.h	8.2 (Berkeley) 1/21/94
 */

/*
 * Unlike other ANSI header files, <assert.h> may usefully be included
 * multiple times, with and without NDEBUG defined.
 */

# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/cdefs.h" 1
/*	$NetBSD: cdefs.h,v 1.58 2004/12/11 05:59:00 christos Exp $	*/

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)cdefs.h	8.8 (Berkeley) 1/9/95
 */
# 46 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/assert.h" 2
# 62 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/assert.h"

 void __assert(const char *, int, const char *);
 void __assert2(const char *, int, const char *, const char *);

# 32 "diag_osif.h" 2


// specific OS (OSA_NUCLEUS) interface defines
# 66 "diag_osif.h"
// align code on byte boundary for nbytes , ASSERT define etc.
/*
   The macro IS_DIAG_BLOCKED translates to checking of global variable in Nucleus, to have
   less impact on runtime when ACAT is not connected (operational, production, field testing)
   In WinCE (and Linux?), the usage of global is a bit difficult due to the required import/export
   definition needed from different dll's. Currently this is left with a function call (more instructions
   are performed before finding out no trace should be sent)
 */
# 95 "diag_osif.h"
// Time stamp for traces
# 112 "diag_osif.h"
//#define DIAG_TS_FREE_RUNNING  0x40A00040



UINT32 diagGetTS_LINUX(void);




/* The DIAG-PRINT is using system calls
 * and therefore should never be called from interrupt context.
 * The wrong using may be found only at run-time.
 **/
# 138 "diag_osif.h"
// Temporary define for LInux testing (Mark B. June 2007)



////////////////////  Extern definitions //////////////////
# 162 "diag_osif.h"
//Macro for trace declaration (WinCE taken from diagdb.h)
# 175 "diag_osif.h"
////////////////////////////////////////
//Diag BSP Configuration
////////////////////////////////////////






typedef enum
{
 DEFAULT_EXT_IF_TX = 0,
 MULTI_EXT_IF_TX,
 SINGLE_EXT_IF_TX,

 //Keeping old enum for backward compatibility. Changing the names to specify it's the USB interface.
 USB_EXT_IF_TX_DEFAULT = DEFAULT_EXT_IF_TX,
 USB_EXT_IF_TX_MULTI = MULTI_EXT_IF_TX,
 USB_EXT_IF_TX_SINGLE = SINGLE_EXT_IF_TX,

 DUMMY_EXT_IF_TX = 0x1FFFFFFF //To enforce UINT32 size
} e_extIfTxMulti;

typedef struct
{
 //Don't change the already used parameters
 e_extIfTxMulti extIfTxMulti;
 //You can always extend this structure. Zero stands for default configuration.
 //...
} s_diagBSPconfig;

extern UINT32 diagBSPconfig(s_diagBSPconfig *cfg);
////////////////////////////////////////
////////////////////////////////////////




//function prototypes
extern UINT32 diag_lock(UINT32 *cpsr_ptr);
extern void diag_unlock(UINT32 cpsr_val);
extern void diag_lock_G(void);
extern void diag_unlock_G(void);
extern UINT32 osTimeClockRateGet(void);
extern void diagOSspecificPhase1Init(void);
extern void diagOSspecificPhase2Init(void);
# 42 "diag_config.h" 2
/************************************************************************* Global definitions
s
\************************************************************************/

/* definition of new header version */


/* definition of USERS_ID */






/************************************************************************* Definitions of SAP values
s
\************************************************************************/
# 98 "diag_config.h"
/* definition of SAPs of old header */

/* RX DiagSAP's */





// 8,9,10 CMI SAPS



/* TX DiagSAP's */





// 8,9,10 CMI SAPS


// SAPs which do not have 'sap related data'
# 81 "diag_comm_EXTif_OSA_LINUX.c" 2
# 1 "../../osa/inc/osa.h" 1
/* ===========================================================================
   File        : osa.h
   Description : This file defines the Intel CCD OS wrapper API and
	      definitions for external application use.

	      The OSA API allows applications to be developed independent of
	      the target microkernel/hardware environment. It provides the
	      facility to add support for multiple independent operating
	      systems from different vendors.

    The OSA API defines a set of interfaces that provide the following
    basic operating system services:

      OS Management     - OSAInitialize(), OSARun()
      Tasks             - OSATaskCreate(), OSATaskDelete(), OSATaskChangePriority(),
			- OSATaskGetPriority(), OSATaskSuspend(), OSATaskResume(),
			- OSATaskSleep(), OSATaskGetCurrentRef()
      Event Flags       - OSAFlagCreate(), OSAFlagDelete(), OSAFlagSet(),
			- OSAFlagWait(), OSAFlagPeek()
      Timers            - OSATimerCreate(), OSATimerDelete(), OSATimerStart(),
			- OSATimerStop(), OSATimerGetStatus()
      Messaging         - OSAMsgQCreate(), OSAMsgQDelete(),
			- OSAMsgQSend(), OSAMsgQRecv(), OSAMsgQPoll()
			- OSAMsgQCreateWithMem()
      Mailboxes         - OSAMailboxQCreate(), OSAMailboxQDelete(),
			- OSAMailboxQSend(), OSAMailboxQRecv(), OSAMailboxQPoll()
      Semaphores        - OSASemaphoreCreate(), OSASemaphoreDelete(),
			- OSASemaphoreAcquire(), OSASemaphoreRelease(),
			- OSASemaphorePoll()
      Mutexes           - OSAMutexCreate(), OSAMutexDelete(),
			- OSAMutexLock(), OSAMutexUnlock(),
      Interrupt Control - OSAIsrCreate(), OSAIsrDelete(),
			- OSAIsrEnable(), OSAIsrDisable(), OSAIsrNotify()
      Real-Time Clock   - OSAGetTicks(), OSAGetClockRate(), OSATick()
			- OSAGetSystemTime()
      Memory            - OSAMemPoolCreate(), OSAMemPoolDelete(),
			- OSAMemPoolAlloc(), OSAMemPoolFree(),
			- OSAMemPoolFixedFree()
      Thread local storage
			- OSATaskSetTls()
			- OSATaskGetTls()
      Symbian Memory    - OSAMemPoolCreateGlobal(), OSAGlobalAlloc(),
			- OSAGlobalRead(), OSAGlobalWrite()

   Copyright (c) 2001 Intel CCD. All Rights Reserved
   =========================================================================== */
# 63 "../../osa/inc/osa.h"
# 1 "../../include/gbl_types.h" 1
/* ===========================================================================
   File        : gbl_types.h
   Description : Global types file.

   Notes       : This file includes the types from the correct environment.
	      The environment is set via the ENV_<ENV> macro. This macro
	      is usually set in /env/<host>/build/<env>_env.mak.

   Copyright 2001, Intel Corporation, All rights reserved.
   =========================================================================== */
# 59 "../../include/gbl_types.h"
/*                         end of gbl_types.h
   --------------------------------------------------------------------------- */
# 64 "../../osa/inc/osa.h" 2
# 1 "../../osa/inc/osa_config.h" 1
/******************************************************************************

*(C) Copyright 2007 Marvell International Ltd.

* All Rights Reserved

******************************************************************************/
/* ===========================================================================
   File        : osa_config.h
   Description : Configuration parameters for the
	      os/osa package.

	      The following are internal OS configuration or tuning parameters
	      that are used to configure the OS services. Each service
	      requires a certain amount of internal memory space to store
	      internal OS data for each instance of the service.

   Notes       : These values can be overridden in gbl_config.h
	      The range checks should be updated for each
	      parameter.

   Copyright (c) 2001 Intel CCD. All Rights Reserved
   =========================================================================== */




/* ---------------------------------------------------------------------------
   Parameter   : osa TASKS Config Parameter
   Description : number of tasks required in the system
   Notes       :
   --------------------------------------------------------------------------- */





/* ---------------------------------------------------------------------------
   Parameter   : osa TIMERS Config Parameter
   Description : number of timers required in the system
   Notes       :
   --------------------------------------------------------------------------- */





/* ---------------------------------------------------------------------------
   Parameter   : osa FLAGS Config Parameter
   Description : number of event flags required in the system
   Notes       :
   --------------------------------------------------------------------------- */





/* ---------------------------------------------------------------------------
   Parameter   : osa SEMAPHORES Config Parameter
   Description : number of semaphores required in the system
   Notes       :
   --------------------------------------------------------------------------- */





/* ---------------------------------------------------------------------------
   Parameter   : osa MUTEXES Config Parameter
   Description : number of mutexes required in the system
   Notes       :
   --------------------------------------------------------------------------- */





/* ---------------------------------------------------------------------------
   Parameter   : osa MEM_POOLS Config Parameter
   Description : number of memory pools required in the system
   Notes       :
   --------------------------------------------------------------------------- */





/* ---------------------------------------------------------------------------
   Parameter   : osa MESSAGING_POOL_SIZE Config Parameter
   Description : number of bytes required for total message Q pools in the system
   Notes       : If there are 5 message queues with max size of 32 bytes with a max
	      number of 100 then you would need 100 * 32 bytes per message * 5 = 16000 bytes.
   --------------------------------------------------------------------------- */




/* ---------------------------------------------------------------------------
   Parameter   : osa MSG_QUEUES Config Parameter
   Description : number of message queues required in the system
   Notes       :
   --------------------------------------------------------------------------- */





/* ---------------------------------------------------------------------------
   Parameter   : osa MBOX_POOL_SIZE Config Parameter
   Description : number of bytes required for total mailbox Q pools in the system
   Notes       : If there are 5 mailboxes each with a max of 100 messages then
	      you would need 100 * 4 bytes per message * 5 = 2000 bytes.
   --------------------------------------------------------------------------- */




/* ---------------------------------------------------------------------------
   Parameter   : osa MBOX_QUEUES Config Parameter
   Description : number of mailbox queues required in the system
   Notes       :
   --------------------------------------------------------------------------- */






/* ---------------------------------------------------------------------------
   Parameter   : osa INTERRUPTS Config Parameter
   Description : number of OSA interrupts required in the OSA system
   Notes       :
   --------------------------------------------------------------------------- */






/* ---------------------------------------------------------------------------
   Parameter   : osa GLOBAL_MEMORY_SIZE Config Parameter
   Description : number of bytes of global memory required in the system
   Notes       :
   --------------------------------------------------------------------------- */


// #define OSA_GLOBAL_MEM_POOL_SIZE_STEP  1  // is this needed?


/* ---------------------------------------------------------------------------
   Parameter   : osa SISR_STACK_SIZE Config Parameter
   Description : Size of SISR stack used by all SISR routines using OSA
   Notes       : Configure this setting for the largest amount of stack used by
	      all ISR's in the system, GARSON 512, HERMON 2048
   --------------------------------------------------------------------------- */





/* ----------------------------------------------------------------
   Parameter   : osa TICK_FREQ_IN_MILLISEC Parameter
   Description : Number of milliseconds between each clock tick generated.
   Notes       :
   ---------------------------------------------------------------- */

     /* Ticks are generated every 5ms   */




/* Include the global configuration file, so these values
   can be overridden */



# 1 "../../include/gbl_config.h" 1
/******************************************************************************

*(C) Copyright 2007 Marvell International Ltd.

* All Rights Reserved

******************************************************************************/
/* ============================================================================
   File        : gbl_config.h
   Description : Global configuration file for testing the
   os/osa package.

   Notes       : This file is only used to test the compilation and
   archiving for the os/osa package.

   Copyright 2001, Intel Corporation, All rights reserved.
   ============================================================================ */
# 89 "../../include/gbl_config.h"
/*                      end of gbl_config.h
			--------------------------------------------------------------------------- */
# 178 "../../osa/inc/osa_config.h" 2

/* Check the Parameter Ranges */
# 215 "../../osa/inc/osa_config.h"
/*                      end of osa_config.h
   --------------------------------------------------------------------------- */
# 65 "../../osa/inc/osa.h" 2

/*****************************************************************************
 * OSA Constants
 ****************************************************************************/
# 98 "../../osa/inc/osa.h"
/* Remain for backwards compatibility */
# 128 "../../osa/inc/osa.h"
/*========================================================================
 *  OSA Return Error Codes
 *========================================================================*/

enum
{
 OS_SUCCESS = 0, /* 0x0 -no errors                                        */
 OS_FAIL, /* 0x1 -operation failed code                            */
 OS_TIMEOUT, /* 0x2 -Timed out waiting for a resource                 */
 OS_NO_RESOURCES, /* 0x3 -Internal OS resources expired                    */
 OS_INVALID_POINTER, /* 0x4 -0 or out of range pointer value                  */
 OS_INVALID_REF, /* 0x5 -invalid reference                                */
 OS_INVALID_DELETE, /* 0x6 -deleting an unterminated task                    */
 OS_INVALID_PTR, /* 0x7 -invalid memory pointer                           */
 OS_INVALID_MEMORY, /* 0x8 -invalid memory pointer                           */
 OS_INVALID_SIZE, /* 0x9 -out of range size argument                       */
 OS_INVALID_MODE, /* 0xA, 10 -invalid mode                                 */
 OS_INVALID_PRIORITY, /* 0xB, 11 -out of range task priority                   */
 OS_UNAVAILABLE, /* 0xC, 12 -Service requested was unavailable or in use  */
 OS_POOL_EMPTY, /* 0xD, 13 -no resources in resource pool                */
 OS_QUEUE_FULL, /* 0xE, 14 -attempt to send to full messaging queue      */
 OS_QUEUE_EMPTY, /* 0xF, 15 -no messages on the queue                     */
 OS_NO_MEMORY, /* 0x10, 16 -no memory left                              */
 OS_DELETED, /* 0x11, 17 -service was deleted                         */
 OS_SEM_DELETED, /* 0x12, 18 -semaphore was deleted                       */
 OS_MUTEX_DELETED, /* 0x13, 19 -mutex was deleted                           */
 OS_MSGQ_DELETED, /* 0x14, 20 -msg Q was deleted                           */
 OS_MBOX_DELETED, /* 0x15, 21 -mailbox Q was deleted                       */
 OS_FLAG_DELETED, /* 0x16, 22 -flag was deleted                            */
 OS_INVALID_VECTOR, /* 0x17, 23 -interrupt vector is invalid                 */
 OS_NO_TASKS, /* 0x18, 24 -exceeded max # of tasks in the system       */
 OS_NO_FLAGS, /* 0x19, 25 -exceeded max # of flags in the system       */
 OS_NO_SEMAPHORES, /* 0x1A, 26 -exceeded max # of semaphores in the system  */
 OS_NO_MUTEXES, /* 0x1B, 27 -exceeded max # of mutexes in the system     */
 OS_NO_QUEUES, /* 0x1C, 28 -exceeded max # of msg queues in the system  */
 OS_NO_MBOXES, /* 0x1D, 29 -exceeded max # of mbox queues in the system */
 OS_NO_TIMERS, /* 0x1E, 30 -exceeded max # of timers in the system      */
 OS_NO_MEM_POOLS, /* 0x1F, 31 -exceeded max # of mem pools in the system   */
 OS_NO_INTERRUPTS, /* 0x20, 32 -exceeded max # of isr's in the system       */
 OS_FLAG_NOT_PRESENT, /* 0x21, 33 -requested flag combination not present      */
 OS_UNSUPPORTED, /* 0x22, 34 -service is not supported by the OS          */
 OS_NO_MEM_CELLS, /* 0x23, 35 -no global memory cells                      */
 OS_DUPLICATE_NAME, /* 0x24, 36 -duplicate global memory cell name           */
 OS_INVALID_PARM /* 0x25, 37 -invalid parameter                           */
};


/*****************************************************************************
 * OSA Data Types
 ****************************************************************************/

typedef void* OSATaskRef;
typedef void* OSASemaRef;
typedef void* OSAMutexRef;
typedef void* OSAMsgQRef;
typedef void* OSAMailboxQRef;
typedef void* OSAPoolRef;
typedef void* OSATimerRef;
typedef void* OSAFlagRef;
typedef UINT8 OSA_STATUS;
/* Remain for backwards compatibility */
typedef void* OSTaskRef;
typedef void* OSSemaRef;
typedef void* OSMutexRef;
typedef void* OSMsgQRef;
typedef void* OSMailboxQRef;
typedef void* OSPoolRef;
typedef void* OSTimerRef;
typedef void* OSFlagRef;
typedef UINT8 OS_STATUS;

/*****************************************************************************
 * OSA Function Prototypes
 ****************************************************************************/

/*========================================================================
 *  OSA Initialization:
 *
 *  Initializes OSA internal structures, tables, and OS specific services.
 *
 *========================================================================*/
 extern OSA_STATUS OSAInitialize(void);

/*========================================================================
 *  OSA Task Management:
 *========================================================================*/
 extern OSA_STATUS OSATaskCreate(
 OSATaskRef * taskRef, /* OS task reference                       */
 void* stackPtr, /* pointer to start of task stack area     */
 UINT32 stackSize, /* number of bytes in task stack area      */
 UINT8 priority, /* task priority 0 - 31                    */
 CHAR * taskName, /* task name                               */
 void (*taskStart)(void*), /* pointer to task entry point        */
 void* argv /* task entry argument pointer             */
 );

 extern OSA_STATUS OSATaskDelete( OSATaskRef taskRef );
 extern OSA_STATUS OSATaskSuspend( OSATaskRef taskRef );
 extern OSA_STATUS OSATaskResume( OSATaskRef taskRef );
 extern void OSATaskSleep( UINT32 ticks ); /* time to sleep in ticks        */

 extern OSA_STATUS OSATaskChangePriority(
 OSATaskRef taskRef, /* OS task reference                       */
 UINT8 newPriority, /* new task priority                       */
 UINT8 *oldPriority /* old task priority                       */
 );

 extern OSA_STATUS OSATaskGetPriority(
 OSATaskRef taskRef, /* OS task reference                       */
 UINT8 *priority /* task priority                           */
 );

 extern void OSATaskYield( void ); /* Allow other ready tasks to run          */
 OSA_STATUS OSATaskGetCurrentRef( OSATaskRef *taskRef ); /* Get current running task ref  */

 extern void OSARun( void );

/*========================================================================
 *  OSA Message Passing
 *========================================================================*/
 extern OSA_STATUS OSAMsgQCreate(
 OSMsgQRef * msgQRef, /* OS message queue reference              */



 UINT32 maxSize, /* max message size the queue supports     */
 UINT32 maxNumber, /* max # of messages in the queue          */
 UINT8 waitingMode /* mode of tasks waiting OS_FIFO, OS_PRIORITY */
 );

 extern OSA_STATUS OSAMsgQCreateWithMem(
 OSMsgQRef * msgQRef, /* OS message queue reference              */



 UINT32 maxSize, /* max message size the queue supports     */
 UINT32 maxNumber, /* max # of messages in the queue          */
 void *qAddr, /* start address of queue memory           */
 UINT8 waitingMode /* mode of tasks waiting OS_FIFO, OS_PRIORITY */
 );

 extern OSA_STATUS OSAMsgQDelete( OSMsgQRef msgQRef );

 extern OSA_STATUS OSAMsgQSend(
 OSMsgQRef msgQRef, /* message queue reference                 */
 UINT32 size, /* size of the message                     */
 UINT8 *msgPtr, /* start address of the data to be sent    */
 UINT32 timeout /* OS_SUSPEND, OS_NO_SUSPEND, or timeout   */
 );

 extern OSA_STATUS OSAMsgQRecv(
 OSMsgQRef msgQRef, /* message queue reference                 */
 UINT8 *recvMsg, /* pointer to the message received         */
 UINT32 size, /* size of the message                     */
 UINT32 timeout /* OS_SUSPEND, OS_NO_SUSPEND, or timeout   */
 );

 extern OSA_STATUS OSAMsgQPoll(
 OSMsgQRef msgQRef, /* message queue reference                 */
 UINT32 *msgCount /* number of messages on the queue         */
 );

/*========================================================================
 *  OSA Mailboxes
 *========================================================================*/
 extern OSA_STATUS OSAMailboxQCreate(
 OSMailboxQRef * mboxQRef, /* OS mailbox queue reference              */



 UINT32 maxNumber, /* max # of messages in the queue          */
 UINT8 waitingMode /* mode of tasks waiting OS_FIFO, OS_PRIORITY */
 );

 extern OSA_STATUS OSAMailboxQDelete( OSMailboxQRef mboxRef );

 extern OSA_STATUS OSAMailboxQSend(
 OSMailboxQRef mboxQRef, /* message queue reference                 */
 void *msgPtr, /* start address of the data to be sent    */
 UINT32 timeout /* OS_SUSPEND, OS_NO_SUSPEND               */
 );

 extern OSA_STATUS OSAMailboxQRecv(
 OSMailboxQRef mboxQRef, /* message queue reference                 */
 void **recvMsg, /* pointer to the message received         */
 UINT32 timeout /* OS_SUSPEND, OS_NO_SUSPEND, or timeout   */
 );

 extern OSA_STATUS OSAMailboxQPoll(
 OSMailboxQRef mboxQRef, /* message queue reference                 */
 UINT32 *msgCount /* number of messages on the queue         */
 );


/*========================================================================
 *  OSA Event Management:
 *========================================================================*/
 extern OSA_STATUS OSAFlagCreate(
 OSFlagRef *flagRef /* OS reference to the flag                */
 );

 extern OSA_STATUS OSAFlagCreateGlobal(
 OSFlagRef *flagRef, /* OS reference to the flag                */
 char *flagName /* name of the event flag                  */
 );

 extern OSA_STATUS OSAFlagDelete( OSFlagRef flagRef );

 extern OSA_STATUS OSAFlagSet(
 OSFlagRef flagRef, /* OS reference to the flag                */
 UINT32 mask, /* flag mask                               */
 UINT32 operation /* OSA_FLAG_AND, OSA_FLAG_OR               */
 );

 extern OSA_STATUS OSAFlagWait(
 OSFlagRef flagRef, /* OS reference to the flag                */
 UINT32 mask, /* flag mask to wait for                   */
 UINT32 operation, /* OSA_FLAG_AND, OSA_FLAG_AND_CLEAR,       */
    /* OSA_FLAG_OR, OSA_FLAG_OR_CLEAR          */
 UINT32 *flags, /* Current value of all flags              */
 UINT32 timeout /* OS_SUSPEND, OS_NO_SUSPEND, or timeout   */
 );

 extern OSA_STATUS OSAFlagPeek(
 OSFlagRef flagRef, /* OS reference to the flag                */
 UINT32 *flags /* returned current value of the flag      */
 );


/*========================================================================
 *  OSA Timer Management:
 *========================================================================*/
/* Timer status information structure */
typedef struct
{
 UINT32 status; /* timer status OS_ENABLED, OS_DISABLED    */
 UINT32 expirations; /* number of expirations for cyclic timers */
} OSATimerStatus;

 extern OSA_STATUS OSATimerStart(
 OSTimerRef timerRef, /* OS supplied timer reference             */
 UINT32 initialTime, /* initial expiration time in ms           */
 UINT32 rescheduleTime, /* timer period after initial expiration   */
 void (*callBackRoutine)(UINT32), /* timer call-back routine     */
 UINT32 timerArgc /* argument to be passed to call-back on expiration */
 );

 extern OSA_STATUS OSATimerCreate( OSTimerRef* timerRef );
 extern OSA_STATUS OSATimerDelete( OSTimerRef timerRef );
 extern OSA_STATUS OSATimerStop( OSTimerRef timerRef );

 extern OSA_STATUS OSATimerGetStatus(
 OSTimerRef timerRef, /* timer reference                     */
 OSATimerStatus *timerStatus /* timer status information            */
 );


/*========================================================================
 *  OSA Semaphore Management
 *========================================================================*/
 extern OSA_STATUS OSASemaphoreCreate(
 OSSemaRef *semaRef, /* OS semaphore reference                  */
 UINT32 initialCount, /* initial count of the semaphore          */
 UINT8 waitingMode /* mode of tasks waiting OS_FIFO, OS_PRIORITY */
 );

 extern OSA_STATUS OSASemaphoreCreateGlobal(
 OSSemaRef *semaRef, /* OS semaphore reference                     */
 char *semaName, /* name of sema to create                     */
 UINT32 initialCount, /* initial count of the semaphore             */
 UINT8 waitingMode /* mode of tasks waiting OS_FIFO, OS_PRIORITY */
 );

 extern OSA_STATUS OSASemaphoreDelete( OSSemaRef semaRef );

 extern OSA_STATUS OSASemaphoreAcquire(
 OSSemaRef semaRef, /* OS semaphore reference                   */
 UINT32 timeout /* OS_SUSPEND, OS_NO_SUSPEND, or timeout    */
 );

 extern OSA_STATUS OSASemaphoreRelease( OSSemaRef semaRef );

 extern OSA_STATUS OSASemaphorePoll(
 OSSemaRef semaRef, /* OS semaphore reference                   */
 UINT32 *count /* Current semaphore count                  */
 );

/*========================================================================
 *  OSA Mutex Management
 *========================================================================*/
 extern OSA_STATUS OSAMutexCreate(
 OSMutexRef *mutexRef, /* OS mutex reference                         */
 UINT8 waitingMode /* mode of tasks waiting OS_FIFO, OS_PRIORITY */
 );

 extern OSA_STATUS OSAMutexDelete( OSSemaRef semaRef );

 extern OSA_STATUS OSAMutexLock(
 OSMutexRef mutexRef, /* OS mutex reference                       */
 UINT32 timeout /* OS_SUSPEND, OS_NO_SUSPEND, or timeout    */
 );

 extern OSA_STATUS OSAMutexUnlock( OSMutexRef mutexRef );


/*========================================================================
 *  OSA Interrupt Control
 *========================================================================*/
 extern OSA_STATUS OSAIsrCreate(
 UINT32 isrNum, /* interrupt to attach routine to          */
 void (*fisrRoutine)(UINT32), /* first level ISR routine to be called    */
 void (*sisrRoutine)(void) /* second level ISR routine to be called   */
 );

 extern OSA_STATUS OSAIsrDelete( UINT32 isrNum );
 extern OSA_STATUS OSAIsrNotify( UINT32 isrNum );

extern UINT32 OSAIsrEnable( UINT32 mask );
extern UINT32 OSAIsrDisable( UINT32 mask );

 extern OSA_STATUS OSAContextLock( void );
 extern OSA_STATUS OSAContextUnlock( void );

/*========================================================================
 *  OSA Sys Context info
 *========================================================================*/
/* Previos Context information structure */
typedef struct
{
 UINT32 prev_task_context;
 UINT32 prev_interrupt_context;
} OSASysContext;


 extern OSA_STATUS OSASysContextLock(OSASysContext *prevContext);
 extern OSA_STATUS OSASysContextUnlock(void);
 extern OSA_STATUS OSASysContextRestore(OSASysContext *prevContext);

/*===========================================================================
 *  OSA Real-Time Access:
 *=========================================================================*/
extern UINT32 OSAGetTicks( void );
 extern UINT32 OSAGetClockRate( void );
extern void OSATick( void );

 extern OSA_STATUS OSAGetSystemTime(
 UINT32 *secsPtr,
 UINT16 *milliSecsPtr
 );

/*========================================================================
 *  OSA Memory Heap Access
 *
 *  Allocating Memory -
 *
 *  Deallocating Memory -
 *
 *========================================================================*/
 extern OSA_STATUS OSAMemPoolCreate(
 OSPoolRef *poolRef, /* OS assigned reference to the pool      */
 UINT32 poolType, /* OSA_FIXED or OS_VARIABLE                */
 UINT8* poolBase, /* pointer to start of pool memory        */
 UINT32 poolSize, /* number of bytes in the memory pool     */
 UINT32 partitionSize, /* size of partitions in fixed pools      */
 UINT8 waitingMode /* mode of tasks waiting OS_FIFO, OS_PRIORITY */
 );

 extern OSA_STATUS OSAMemPoolDelete( OSPoolRef poolRef );

 extern OSA_STATUS OSAMemPoolAlloc(
 OSPoolRef poolRef, /* OS reference to the pool to be used    */
 UINT32 size, /* number of bytes to be allocated        */
 void** mem, /* pointer to start of allocated memory   */
 UINT32 timeout /* OS_SUSPEND, OS_NO_SUSPEND, or timeout  */
 );

 extern OSA_STATUS OSAMemPoolFree(
 OSPoolRef poolRef, /* OS reference to the pool to be used    */
 void* mem /* pointer to start of memory to be freed */
 );
 extern OSA_STATUS OSAMemPoolFixedFree( void *mem ); /* Ptr to fixed pool memory */

 extern OSA_STATUS OSAMemPoolCreateGlobal(
 OSAPoolRef *poolRef, /* OS reference to the partition pool to be used    */
 char *poolName, /* name of global mem pool                */
 UINT8* poolBase, /* pointer to start of pool memory        */
 UINT32 poolSize, /* size of the pool                       */
 UINT32 partitionSize /* size of partitions in the pool         */
 );

 extern OSA_STATUS OSATaskSetTls(void *arg); /* set up the thread local storage (TLS) to the pointer arg*/
 extern OSA_STATUS OSATaskGetTls(void **arg); /* get the TLS pointer and assign it to arg */
# 545 "../../osa/inc/osa.h"
/*****************************************************************************
 * OSA Internal Assert macro definition
 ****************************************************************************/
# 586 "../../osa/inc/osa.h"
/* Remap old function names to new ones to remain backwards compatibility with
 * old function calls.
 */
# 82 "diag_comm_EXTif_OSA_LINUX.c" 2
# 1 "diag_types.h" 1
/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code ("Material") are owned
   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
   transmitted, distributed, or disclosed in any way without Intel's prior express written permission.

   No license under any patent, copyright, trade secret or other intellectual property right is granted to or
   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
   Intel in writing.
   -------------------------------------------------------------------------------------------------------------------*/





////////#include "osa.h"





typedef struct ModuleMessageIDTag
{
 INT16 moduleID; //Need to signed to support Comparison to INVALID_VALUE = (-1)
 UINT16 messageID;
}ModuleMessageID;



/* NOTE: negative logic: bit=0 means trace is sent, 1 - filtered out */
// also, if g_OverridePassFilter is set, indicate that there's no filter




typedef union
{
 struct {
  UINT8 icatNotReady; //BOOL
  UINT8 blockSendPDU; //BOOL
  UINT8 traceProhibited; //BOOL
 }flags;
 UINT32 all;
}DiagBlockReasons;

typedef enum
{
 DIAG_PEND_MSG_TX_OK = 0,
 DIAG_PEND_MSG_TX_ERR
}DIAG_pendMsgSt; // status of pending messages in buffer


typedef enum
{
 DIAG_ERROR_CHANNEL_ALREADY_CLOSED = 0,
 DIAG_CLOSE_CHANNEL_OK
} DiagCloseChStatus;

typedef enum
{
 DIAG_CHAR = 0,
 DIAG_CHAR_BUFFER_SIZE_KNOWN,
 DIAG_CHAR_BUFFER_SIZE_UNKNOWN,
 DIAG_SHORT,
 DIAG_SHORT_BUFFER_SIZE_KNOWN,
 DIAG_SHORT_BUFFER_SIZE_UNKNOWN,
 DIAG_LONG,
 DIAG_LONG_BUFFER_SIZE_KNOWN,
 DIAG_LONG_BUFFER_SIZE_UNKNOWN,
 DIAG_STRING,
 DIAG_ENUM,
 DIAG_ENUM_SHORT,
 DIAG_ENUM_LONG,
 DIAG_ENUM_BUFFER_SIZE_KNOWN,
 DIAG_ENUM_BUFFER_SIZE_UNKNOWN,
 DIAG_ENUM_SHORT_BUFFER_SIZE_KNOWN,
 DIAG_ENUM_SHORT_BUFFER_SIZE_UNKNOWN,
 DIAG_ENUM_LONG_BUFFER_SIZE_KNOWN,
 DIAG_ENUM_LONG_BUFFER_SIZE_UNKNOWN,
 DIAG_ENUM_DUMMY_FOCE_4BYTE_VALUE = 0x00FFFFFF
}ParameterType;

typedef struct {
 ParameterType types; //the parameter type
 UINT16 repititions; //0 - no info about no. of repititions (open brackets), 1 or more - the no. of repititions
}ParameterInfo;

typedef struct {
 UINT16 moduleId;
 UINT16 reportId;
 UINT16 numOfParams;
 ParameterInfo parameters[1]; // variable length array
} DiagTraceDescriptor;

//typedef UINT32 * CommandAddress;	//Manitoba
typedef void (*CommandAddress)(void); //Hermon

typedef char* CommandProto;
typedef const char * DiagDBVersion;

/* This enum defines the protocol types to be used when the diag logger */
/*	communicates with ICAT.Currently only PROTOCOL_TYPE_0 is supported	*/

// ICAT EXPORTED ENUM
typedef enum
{
 PROTOCOL_TYPE_0 = 0,
 MAX_PROTOCOL_TYPES
}ProtocolType;

//ICAT EXPORTED STRUCT
typedef struct
{
 BOOL bEnabled; //enable/disable the trace logging feature
 ProtocolType eProtocolType; //protocol type for communication with ICAT, currently only protocol type 0 is supported
 UINT16 nMaxDataPerTrace; //for each trace, what is the maximum data length to accompany the trace, in protocol type 0, this is relevant only to DSP messages
}DiagLoggerDefs;
// Filler entry value
# 83 "diag_comm_EXTif_OSA_LINUX.c" 2
# 1 "diag.h" 1



# 1 "diag_API.h" 1
/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code ("Material") are owned
   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
   transmitted, distributed, or disclosed in any way without Intel's prior express written permission.

   No license under any patent, copyright, trade secret or other intellectual property right is granted to or
   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
   Intel in writing.
   -------------------------------------------------------------------------------------------------------------------*/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*                                                                                 *
*     File name:      diag_api .h                                                 *
*     Programmer:     Shiri Dolev                                                 *
*                     Ohad Peled, Itzik Yankilevich, Barak Witkowsky, Yaeli Karni *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*                                                                                 *
*       Create Date:  November, 2003                                              *
*                                                                                 *
*       Description: DIAG Tx APIs - MACROs.									   *
*                                                                                 *
*       Notes:  There are 2 files diag_API_var.h and diag_API.h                   *
*               The diag_API_var.h - declaration without MACRO-re-define          *
*               The diag_API.h     - has part which is re-defined by MACROs       *
*                                    and treated differently by PrePassProcessor! *
*                                                                                 *
* July 2007 - fix macros DIAT_TRACEM... (IY)									   *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
# 5 "diag.h" 2
# 1 "diag_API_var.h" 1
/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code ("Material") are owned
   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
   transmitted, distributed, or disclosed in any way without Intel's prior express written permission.

   No license under any patent, copyright, trade secret or other intellectual property right is granted to or
   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
   Intel in writing.
   -------------------------------------------------------------------------------------------------------------------*/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*                                                                                 *
*     File name:      Diag_API_var .h                                             *
*     Programmer:     Shiri Dolev                                                 *
*                     Ohad Peled                                                  *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*                                                                                 *
*       Create Date:  November, 2003                                              *
*                                                                                 *
*       Description: DIAG Tx Multiple Params Definitions.						   *
*                                                                                 *
*       Notes:  There are 2 files diag_API_var.h and diag_API.h                   *
*               The diag_API_var.h - declaration without MACRO-re-define          *
*               The diag_API.h     - has part which is re-defined by MACROs       *
*                                    and treated differently by PrePassProcessor! *
*                                                                                 *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
# 6 "diag.h" 2
# 1 "diag_header_external.h" 1
/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code ("Material") are owned
   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
   transmitted, distributed, or disclosed in any way without Intel's prior express written permission.

   No license under any patent, copyright, trade secret or other intellectual property right is granted to or
   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
   Intel in writing.
   -------------------------------------------------------------------------------------------------------------------*/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*                                                                                 *
*     File name:      diag_header_handler.h                                       *
*     Programmer:     Barak Witkowski                                             *
*                                                                                 *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*                                                                                 *
*       Create Date:  November, 2006                                              *
*                                                                                 *
*       Description:                                                                                                                       *
*                                                                                 *
*       Notes:                                                                    *
*                                                                                 *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */



/* definitions of fields sizes in the new header */
# 54 "diag_header_external.h"
# 1 "diag_header_handler.h" 1
/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code ("Material") are owned
   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
   transmitted, distributed, or disclosed in any way without Intel's prior express written permission.

   No license under any patent, copyright, trade secret or other intellectual property right is granted to or
   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
   Intel in writing.
   -------------------------------------------------------------------------------------------------------------------*/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*                                                                                 *
*     File name:      diag_header_handler.h                                       *
*     Programmer:     Barak Witkowski                                                 *
*                                                                                 *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*                                                                                 *
*       Create Date:  November, 2006                                              *
*                                                                                 *
*       Description:                                                                                                                       *
*                                                                                 *
*       Notes:                                                                    *
*                                                                                 *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */



# 1 "../../include/global_types.h" 1
/******************************************************************************

*(C) Copyright 2007 Marvell International Ltd.

* All Rights Reserved

******************************************************************************/
/****************************************************************************
 *
 * Name:          global_types.h
 *
 * Description:   Standard type definitions
 *
 ****************************************************************************
 *
 *
 *
 *
 ****************************************************************************
 *                  Copyright (c) Intel 2000
 ***************************************************************************/
# 40 "diag_header_handler.h" 2
# 1 "diag_pdu.h" 1
/*------------------------------------------------------------

   (C) Copyright [2006-2008] Marvell International Ltd.

   All Rights Reserved

   ------------------------------------------------------------*/
# 6 "diag_pdu.h"
/*--------------------------------------------------------------------------------------------------------------------

   INTEL CONFIDENTIAL

   Copyright 2006 Intel Corporation All Rights Reserved.

   The source code contained or described herein and all documents related to the source code ("Material") are owned

   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or

   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of

   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and

   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,

   transmitted, distributed, or disclosed in any way without Intel's prior express written permission.



   No license under any patent, copyright, trade secret or other intellectual property right is granted to or

   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,

   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by

   Intel in writing.

   -------------------------------------------------------------------------------------------------------------------*/
# 22 "diag_pdu.h"
/*******************************************************************************

*               MODULE HEADER FILE

********************************************************************************

* Title: diag

*

* Filename: diag_pdu.h

*

* Target, platform: Common Platform, SW platform

*

* Authors:  Yoel Lavi

*

* Description:

*

* Last Updated:

*

* Notes:

*******************************************************************************/
# 43 "diag_pdu.h"
////////////#include "ICAT_config.h"
# 1 "../../include/global_types.h" 1
/******************************************************************************

*(C) Copyright 2007 Marvell International Ltd.

* All Rights Reserved

******************************************************************************/
/****************************************************************************
 *
 * Name:          global_types.h
 *
 * Description:   Standard type definitions
 *
 ****************************************************************************
 *
 *
 *
 *
 ****************************************************************************
 *                  Copyright (c) Intel 2000
 ***************************************************************************/
# 45 "diag_pdu.h" 2

/*-- Header constants in tx/rx buffer ----------------------------------*/
# 60 "diag_pdu.h"
/*-----------------5/29/2005 12:49------------------

 * Support long messages in L2

 * --------------------------------------------------*/





/*-- Type definitions (T type) -----------------------------------------*/
typedef struct
{
 UINT16 STX; // = DIAG_L2L4_PROTOCOL_STX
 UINT8 ProtocolID;
 UINT8 Length;
}TxL2L4PDUHeader; //TxSSPPDUHeader;

typedef struct
{



 UINT16 ETX;

}TxL2L4PDUFooter; //TxSSPPDUFooter;


/* the code below defines sizes of header in footer, depending on old/new header and interfaces defined (relevant for old header only */







/* in old msg structure footer does not exist for USB, but is exist for L2,L4 - we always allocate

   space, not always using it.. */

// size of signal header (we dont have in it SAP related data (no ModuleID, msgID data))


typedef struct
{
 UINT8 txPduHeader[16];
 UINT8 data[1];
}TxPDU;

/*-- Public Functions Prototypes ---------------------------------------*/
# 41 "diag_header_handler.h" 2
# 1 "diag_osif.h" 1
/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/
# 42 "diag_header_handler.h" 2
/****************************************
*                                      *
*                       Constants Definition		*
*                                      *
****************************************/




// #define  SIZE_OF_TX_HEADER sizeof(TxPDUHeader)



/* definition of checksum bit in total length field */
/* MS Bit of total length field is checksumExist bit */


/* ****************************** */
/* options third byte definitions */
/* ****************************** */
/* Bit 0 - Padding LS Bit */


/* Bit 1 - Padding MS Bit */


/*
	OPTIONS appear in the follwoing order (if exist):
	- SAP-related
	- TimeStamp
	- Frame Number
	- Message Counter

	(if the options field contains only SAP-related and Message-counter,
	 then after the 4 bytes of options, there will be four bytes of SAP-related data followed by
	 four bytes of Message-Counter

	 if the options field contains  TimeStamp, Frame Number and Message-counter,
	 then after the 4 bytes of options, there will be:
	 four bytes of TimeStamp followed by
	 four bytes of Frame-Number followed by
	 four bytes of Message-Counter
	)
 */
/* Bit 2 - SAP_RELATED_DATA_BIT */


/* Bit 3 - Timestamp */


/* Bit 4 - Frame number */


/* Bit 5 - Message counter */


/* ***************************************** */
/* options third (and last) byte definitions */
/* ***************************************** */
/* Bit 7 (left most bit) will indicate another option data (4 bytes) */



/****************************************
*                                      *
*                       Struct Definition			*
*                                      *
****************************************/

/* ====================================== *
*       Diag Message Header Struct		  *
* ====================================== */
/*
 * New Header structure. Currently, header size is static
 * ----------------------------------------------------------------------------------------------------------------------
 */
typedef struct
{
 /* SoT pattern */
 UINT16 sot;

 /* Total length of packet data without the SOT, EOT, but including the Padding. That is the total length
	 * is counted from (including) the Total Length field, till (including) the last byte of the Payload-data
	 * and padding.Total Length is not expected to exceed ~6K bytes. */
 UINT16 msgTotalLen;

 /* length of the header. The length is calculated from that field to the end of all options fields,
	 * which means: right before the start of the payload data */
 UINT16 headerTotalLen;

 /* Version of header, anytime more data is added to the header, version should be updated so SW will know
	 * what is relevant (see OPTIONS field) - each addition of option, or new fields should be indicated by
	 * new header version. */
 UINT16 headerVersion;

 /* The sending source (ICAT, Core-n). */
 UINT8 source;

 /* The destination (ICAT, Core-n). */
 UINT8 destination;

 /* Message type (SAP) - indicate how to manipulate the payload data. It can be accompanied with some
	 * relevant data in the first option (for ModueID, CommandID, ServiceID etc). */
 UINT16 msgType_Sap;

 /* Bit field which indicates presence of data (unassigned bits must be zero, when a new bit is assigned,
	 * the header version should be incremented).
	 * There is an option bit (bit 0, LSB) to expand OPTIONS.
	 * Each optional field (declared by the a bit) should be 4 bytes in size, even if there is no current need 4 bytes.
	 * In order to avoid little endians complications the field will be splitted to 4 different bytes */
 UINT8 options[4];

 /* the options data. Each data is 4 bytes. Data appears according to the bits in the options field
	 * The currently available options field are: SAP related data, counter, frame number and time stamp */
 UINT32 optionsData[4];
} TxPDUNewHeader;

/* current TxPDUHeader header */
typedef struct
{
 union {
  TxL2L4PDUHeader L2L4Header;
  struct {
   UINT16 length;
   UINT16 PID;
   } rawHeader;
 } extIfhdr;
 UINT16 DiagSAP;
 UINT16 counter;
 UINT16 moduleID;
 UINT16 msgID;
 UINT32 timeStamp;
}TxPDUOldHeader;

/* define header struct (old or new) according to the suitable switch */





/* current signal Tx Header structure */
typedef struct
{
 union {
  TxL2L4PDUHeader L2L4Header;
  UINT32 length;
 } extIfhdr;
 UINT16 DiagSAP;
 UINT16 counter;
 UINT32 timeStamp;
}TxPDUSigHeader;

typedef struct
{
 UINT8 DiagSAP;
 UINT8 serviceID;
 UINT16 moduleID;
 UINT32 commID;
 UINT32 sourceID;
 UINT8 functionParams[1]; //contains the requested command input parameters
}RxPDU;
typedef struct
{

 struct {
  UINT16 length;
  UINT16 PID;
  } rawHeader;
 UINT8 DiagSAP;
 UINT8 serviceID;
 UINT16 moduleID;
 UINT32 commID;
 UINT32 sourceID;
 UINT32 CMMversion;
}TxPDUCMMHeader;

/* global flags indicating which optional data should be sent in the tx messages */
typedef struct
{
 /* flag indicating whether time stamp should be added to tx messages */
 BOOL TimeStampExist;
 /* flag indicating whether counter should be added to tx messages */
 BOOL MsgCounterExist;
 /* flag indicating whether checksum should be added to tx messages */
 /*^^Barak. in the future this paramter should be per interface */
 BOOL ChecksumExist;
 /* flag indicating whether frame number should be added to tx messages */
 BOOL FrameNumberExist;
} globalFlagsStrct;

/****************************************
*                                      *
*                       Globals Declaration			*
*                                      *
****************************************/
// Interface for FUll Frame number - be placed in the counter field and high byte of module ID.
// Pointer to Frame number - set by L1
// The frame number is stored in the counter field and high byte of ModuleID field
// due to current limitation of the header
extern UINT32* diagFrameNumberPtr;

// Flag is full frame number should be set (also in the module ID field) or only
// in the counter field (for back ward compatibility with ICAT (2.6 and before)
//  - the full FN is set only on ICAT command)
# 55 "diag_header_external.h" 2
typedef void VOID;
/****************************************
*   Definiiton of Core2Core messages    *
****************************************/
/*       YKstat
	C2C_STATISTICS ( March 2008) - used for statistics of COMM
		on internal, and apps on ext apps catches the message and adds its
		statistics. Sends out as trace from COMM (comm send traceID in the message)
 */


/****************************************
*                                      *
*                       Function Declaration		*
*                                      *
****************************************/

VOID setTxDiagTraceHeader(UINT8 *pHeader, UINT16 *pduLen, UINT16 type, UINT16 moduleID, UINT16 msgID);
VOID CreateDiagErrMsgHeader(UINT8 *pHeader, UINT16 pduLen);
VOID InsertCounterAndCheckSumToTxHeader(UINT8 *pHeader, UINT32 MsgCounter);
VOID CreateConnectedDeviceQueryTxHeader(UINT8 *pHeader, UINT16 packetLen);
UINT16 GetMsgIDFromTxHeader(UINT8 *pHeader);
VOID InsertTimeStampToTxHeader(UINT8 *pHeader);
//VOID	ChangeMsgPointerAndLengthDueToDiffInHeaders(UINT8 connectedDev, UINT8 **msg, UINT16 *len, Allocation_Type allocType);
// The 3rd parameter is for 'old header' only  - as the header differes on different interfaces.
extern INT8 GetMessageCoreID(UINT8* pData, UINT32 nLen, UINT32 bIsUSBheader);
extern void SetMessageOtherCoreID(UINT8* pData, UINT32 myCoreID);
INT8 GetMessageCoreIDForIntIf(UINT8* pData);
UINT16 GetRxMsgDiagSap(UINT8 *rxMsg);
UINT16 GetRxMsgServiceID(UINT8 *rxMsg);
UINT16 GetRxMsgModuleID(UINT8 *rxMsg);
UINT32 GetRxMsgCommID(UINT8 *rxMsg);
UINT32 GetRxMsgSourceID(UINT8 *rxMsg);
UINT8 *GetRxMsgFunctionParams(UINT8 *rxMsg);
void SetRxMsgDiagSap(UINT8 *rxMsg, UINT8 newDiagSap);
void SetSapAndServiceIDofCmdToOtherCore(UINT8 *rxMsg, UINT8 diagInternalServiceType);
UINT32 GetUsbMsgLen(void* pHeader);
BOOL IsICATReadyNotify(UINT8* pData);
UINT8 FindSoTOffset(void* pHeader);
BOOL IsC2CStats(UINT8 *pData);


VOID setTxDiagSigHeader(void *pMsg, UINT16 *pduLen);







VOID diagCalcUpdateTxL2L4MessageLength(TxL2L4PDUHeader *header, UINT16 *messageLength, UINT32 *padBytes);

/* This function sets a new a pointer to frame number, which will be inserted
 * in the header counter field (instead of the regular count up) - 2 lower bytes
 * (LSBytes 0 and LSByte 1 of FN) and
 * if Full-Frame-number requested (by ICAT command), then the high byte of
 * the module ID will be set with the 3rd byte (LSByte 3) of the frame number
 */
VOID SetDiagTxMsgFramNumberPtr(const UINT32* const NewFNPtr);
# 7 "diag.h" 2

# 1 "../../include/linux_types.h" 1
/* ===========================================================================
   File        : posix_types.h
   Description : Global types file for the POSIX Linux environment.

   Notes       : This file is designed for use in the gnu environment
	      and is referenced from the gbl_types.h file. Use of
			  this file requires ENV_POSIX to be defined in posix_env.mak.

   Copyright 2005, Intel Corporation, All rights reserved.
   =========================================================================== */
# 74 "../../include/linux_types.h"
/*                         end of posix_types.h
   --------------------------------------------------------------------------- */
# 9 "diag.h" 2
# 24 "diag.h"
//#define DiagDataIndProcId 3
//#define DiagDataReqProcId 4
# 40 "diag.h"
typedef int (*DiagRxCallbackFunc)(char* packet, int len);
typedef struct _diagheader
{
 UINT16 seqNo;
 UINT16 packetlen;
 UINT32 reserved;
}DIAGHDR; //don't change the definition. variable definition order sensitive.


void diagTxRawData(unsigned char *msg, int msglen);
void diagSendUsbData(unsigned char*msg, int msglen);

void InitDiagChannel(void);
void DeInitDiagChannel(void);
void diagTxMsg(int procId, unsigned char *msg, int msglen);
int diagRegisterRxCallBack(DiagRxCallbackFunc callback);
int diagUnregisterRxCallBack(void);
void diagUsbConnectNotify();
void diagUsbDisConnectNotify();
# 84 "diag_comm_EXTif_OSA_LINUX.c" 2
# 1 "diag_pdu.h" 1
/*------------------------------------------------------------

   (C) Copyright [2006-2008] Marvell International Ltd.

   All Rights Reserved

   ------------------------------------------------------------*/
# 6 "diag_pdu.h"
/*--------------------------------------------------------------------------------------------------------------------

   INTEL CONFIDENTIAL

   Copyright 2006 Intel Corporation All Rights Reserved.

   The source code contained or described herein and all documents related to the source code ("Material") are owned

   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or

   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of

   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and

   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,

   transmitted, distributed, or disclosed in any way without Intel's prior express written permission.



   No license under any patent, copyright, trade secret or other intellectual property right is granted to or

   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,

   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by

   Intel in writing.

   -------------------------------------------------------------------------------------------------------------------*/
# 22 "diag_pdu.h"
/*******************************************************************************

*               MODULE HEADER FILE

********************************************************************************

* Title: diag

*

* Filename: diag_pdu.h

*

* Target, platform: Common Platform, SW platform

*

* Authors:  Yoel Lavi

*

* Description:

*

* Last Updated:

*

* Notes:

*******************************************************************************/
# 85 "diag_comm_EXTif_OSA_LINUX.c" 2
# 1 "diag_tx.h" 1
/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code ("Material") are owned
   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
   transmitted, distributed, or disclosed in any way without Intel's prior express written permission.

   No license under any patent, copyright, trade secret or other intellectual property right is granted to or
   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
   Intel in writing.
   -------------------------------------------------------------------------------------------------------------------*/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*                                                                                 *
*     File name:      diag_tx  .h                                                 *
*     Programmer:     Shiri Dolev                                                 *
*                                                                                 *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*                                                                                 *
*       Create Date:  November, 2003                                              *
*                                                                                 *
*       Description: DIAG Tx Definitions - Handle the transmit reports functions. *
*                                                                                 *
*       Notes:                                                                    *
*                                                                                 *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */




# 1 "../../osa/inc/osa.h" 1
/* ===========================================================================
   File        : osa.h
   Description : This file defines the Intel CCD OS wrapper API and
	      definitions for external application use.

	      The OSA API allows applications to be developed independent of
	      the target microkernel/hardware environment. It provides the
	      facility to add support for multiple independent operating
	      systems from different vendors.

    The OSA API defines a set of interfaces that provide the following
    basic operating system services:

      OS Management     - OSAInitialize(), OSARun()
      Tasks             - OSATaskCreate(), OSATaskDelete(), OSATaskChangePriority(),
			- OSATaskGetPriority(), OSATaskSuspend(), OSATaskResume(),
			- OSATaskSleep(), OSATaskGetCurrentRef()
      Event Flags       - OSAFlagCreate(), OSAFlagDelete(), OSAFlagSet(),
			- OSAFlagWait(), OSAFlagPeek()
      Timers            - OSATimerCreate(), OSATimerDelete(), OSATimerStart(),
			- OSATimerStop(), OSATimerGetStatus()
      Messaging         - OSAMsgQCreate(), OSAMsgQDelete(),
			- OSAMsgQSend(), OSAMsgQRecv(), OSAMsgQPoll()
			- OSAMsgQCreateWithMem()
      Mailboxes         - OSAMailboxQCreate(), OSAMailboxQDelete(),
			- OSAMailboxQSend(), OSAMailboxQRecv(), OSAMailboxQPoll()
      Semaphores        - OSASemaphoreCreate(), OSASemaphoreDelete(),
			- OSASemaphoreAcquire(), OSASemaphoreRelease(),
			- OSASemaphorePoll()
      Mutexes           - OSAMutexCreate(), OSAMutexDelete(),
			- OSAMutexLock(), OSAMutexUnlock(),
      Interrupt Control - OSAIsrCreate(), OSAIsrDelete(),
			- OSAIsrEnable(), OSAIsrDisable(), OSAIsrNotify()
      Real-Time Clock   - OSAGetTicks(), OSAGetClockRate(), OSATick()
			- OSAGetSystemTime()
      Memory            - OSAMemPoolCreate(), OSAMemPoolDelete(),
			- OSAMemPoolAlloc(), OSAMemPoolFree(),
			- OSAMemPoolFixedFree()
      Thread local storage
			- OSATaskSetTls()
			- OSATaskGetTls()
      Symbian Memory    - OSAMemPoolCreateGlobal(), OSAGlobalAlloc(),
			- OSAGlobalRead(), OSAGlobalWrite()

   Copyright (c) 2001 Intel CCD. All Rights Reserved
   =========================================================================== */
# 41 "diag_tx.h" 2
# 1 "diag_osif.h" 1
/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/
# 42 "diag_tx.h" 2

typedef enum
{
 Static_Memory_Allocation,
 Pool_Memory_Allocation_Trace,
 Pool_Memory_Allocation_Signal
} Allocation_Type;



struct diag_target_buffer
{
 enum {INSIDE_PACKET, OUT_OF_PACKET} state;
 unsigned int chunk_size;
 unsigned int packet_len;
 char chunk[(16*1024)];
};

/*-- Public Functions Prototypes ---------------------------------------*/
extern void diagSendPDUSignal( void *txPduBuffPtr, UINT16 Length );
void DiagSendFullFN(void);
void setL2Header(UINT8 *pHeader, UINT16 *pduLen);
UINT16 GetOnePacket(UINT8 *RawHeader, UINT8 * NewHeader, UINT16 pduLen, BOOL *Wraped);
int diag_disc_flush(struct diag_target_buffer* target_buff);
int diag_disc_rx(struct diag_target_buffer* target_buff, char * buff, int size);

/*----------------------------------------------------------------------*/
# 86 "diag_comm_EXTif_OSA_LINUX.c" 2
# 1 "diag_header_external.h" 1
/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code ("Material") are owned
   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
   transmitted, distributed, or disclosed in any way without Intel's prior express written permission.

   No license under any patent, copyright, trade secret or other intellectual property right is granted to or
   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
   Intel in writing.
   -------------------------------------------------------------------------------------------------------------------*/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*                                                                                 *
*     File name:      diag_header_handler.h                                       *
*     Programmer:     Barak Witkowski                                             *
*                                                                                 *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*                                                                                 *
*       Create Date:  November, 2006                                              *
*                                                                                 *
*       Description:                                                                                                                       *
*                                                                                 *
*       Notes:                                                                    *
*                                                                                 *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
# 87 "diag_comm_EXTif_OSA_LINUX.c" 2
# 1 "diag_buff.h" 1
/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code ("Material") are owned
   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
   transmitted, distributed, or disclosed in any way without Intel's prior express written permission.

   No license under any patent, copyright, trade secret or other intellectual property right is granted to or
   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
   Intel in writing.
   -------------------------------------------------------------------------------------------------------------------*/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*                                                                                 *
*     File name:      Diag_Buff .h                                                *
*     Programmer:     Shiri Dolev                                                 *
*                                                                                 *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*                                                                                 *
*       Create Date:  November, 2003                                              *
*                                                                                 *
*       Description: DIAG Buffering Definitions								   *
*                                                                                 *
*       Notes:                                                                    *
* Yaeli Karni June 2006	Add definitions for data collection of messages Tx/Rx  *
*                                       sizes. Target - to limit message size to 4000 bytes			   *
*                                                                                 *
* Yaeli Karni Jan 2008  Tx-Q dynamically allocated, size per Q. UPdate macros  *
*                                                                                 *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */




# 1 "../../include/global_types.h" 1
/******************************************************************************

*(C) Copyright 2007 Marvell International Ltd.

* All Rights Reserved

******************************************************************************/
/****************************************************************************
 *
 * Name:          global_types.h
 *
 * Description:   Standard type definitions
 *
 ****************************************************************************
 *
 *
 *
 *
 ****************************************************************************
 *                  Copyright (c) Intel 2000
 ***************************************************************************/
# 45 "diag_buff.h" 2
////#include "diag_tx.h"
////#include "osa.h"

//For APPS+COMM platform the capability of APPS must be greater than COMM.
//So increase APPS-list-size vs COMM-list-size
# 80 "diag_buff.h"
// suggested by Chen Reibach - March 2008 due to FT .. wait for 1/3 of the Queue to be empty before resuming..
# 91 "diag_buff.h"
//NOT used #define LIST_FULL(dps)	(((dps)->diagTotalNumOfItems) >= (((dps)->diagNumOfItemsAllowedOnQ) - 2))



// Lost Diag messages definitions:
// Queu full (ext, INT-if MUST be same size of text !!



// save 10 places for number digits	- all messages same size



// save 12 places for len and msgID



// 13 digits and '\n'

// 13 digits and '\n'







// Diag error messages memory needs to be static (not dynamic):
extern UINT8 diagErrMsgBufQueueOverflow [];
extern UINT8 diagErrMsgBufQueueINTOverflow [];
extern UINT8 diagErrMsgBufMemoryAlloc [];
extern UINT8 diagErrMsgTotalLostMsgsMEM [];
extern UINT8 diagErrMsgTotalLostMsgsEXT [];
extern UINT8 diagErrMsgTotalLostMsgsINT [];
extern UINT8 diagErrMsgTooLong [];
extern UINT8 diagErrMsgWarningLong [];
// type of error condition
//ICAT EXPORTED ENUM
typedef enum {
 diagErrNoMemory,
 diagErrMemoryOK,
 diagErrExtQueueCongest,
 diagErrINTQueueCongest,
 diagErrCMIQueueCongest,
 diagErrExtQueueOK,
 diagErrINTQueueOK,
 diagErrCMIQueueOK,
 diagErrMSGmaxLen,
 diagErrMsgwarningLen
} diagErrStatesE;


typedef struct
{
 UINT8 *virtualReportPtr;
 UINT16 reportLength;
 //UINT16 /*Allocation_Type*/ allocType;	//Set for UINT16 to promise same assignement for all OS
 UINT8 allocType;
 UINT8 clientID; //TBDIY casted from COMDEV_NAME and valid only in CMI mode. For future extend to a general SendMsg function to all interfaces.
}DiagReportItem_S;

// =========================================================================================================================
// Manipulation of the queue variables (which are globals) MUST BE DONE under disable interrupts, since there are tightly
// connected.
// The enqueue (sendPDU()) action manipulates the NextFreeItem pointer (the tail of the queue which points to the
// next free entry in the queue) and the diagTotalNumOfItems (which is the total number of physical items in list, including
// those who were transmitted but not released yet).
// This action is called from many different tasks outside the Diag scope.
//
// The Tx task of each physical connection  use the FirstItemOnList and update the NextItemToSend (which points to the item which
// is the next one to be send on the physical connection) pointers,	and the diagNumOfTxedItems (which is the number of items
// that were already sent but were not released yet) as it dequues items and perform the actual send.
// As part of the removal of items from list, the FirstItemOnList, the diagTotalNumOfItems and diagNumOfTxedItemsis are
// updated for each item that was released.
// These actions must be synchronized with other actions that try to enqueu messages, or reset the queue toatlly,
// THUS all manipouation of the queue pointers (FirstItemOnList, NextFreeItem & NextItemToSend) and items' counters
// (the diagTotalNumOfItems and diagNumOfTxedItemsis) must be protected and atomic (when test and set actions are done).
// =========================================================================================================================
typedef struct
{
 // The head of the queue, from which items are dequeued for transmission and afterwards being removed.
 UINT16 diagFirstItemOnList;

 // The tail of the queue, to which new messages are enqueueud.
 UINT16 diagNextFreeItemOnList;

 // Points to the next message in the queue that needs to be dequeued and transmitted. This item will be equal to the
 // FirstItemOnList before transmission and will change as result of the transmission (while the FirstItemOnList will stiil point
 // the the first item that was transmitted but not released yet). Both indeces will be equal again after the remove of the
 // transmitted items.
 UINT16 NextItemToSend;

 // The number of items currently in the queue (including those who were transmitted but not released yet.
 UINT16 diagTotalNumOfItems;

 // The number of items that were transmitted in the last cycle of transmission, before the release.
 UINT16 diagNumOfTxedItems;
 // total number of items in queue - to allow different Queues with different numbers
 UINT16 diagNumOfItemsAllowedOnQ;
}DiagQPtrData_S;

// A structureand data info of transmitted data from COMM to APPS to ICAT.
// It is sent out any X bytes of traces are produced on the COMM
// It is intended to collect data on the traces path COMM-via-APPS-to-ICAT (not genral
// collection on all paths). Data collected as described in the structure items.
// the driving of data is the COMM collection when number of produced bytes-of-traces e
// exceeded the X (). The APPS collection is done when the message arrives at it over the
// internal interface (thus a letency of MSL/SHM transmit till 'APPS period' statistics are
// added.
// Collection is done on COMM, sent ot APP as C2C message (SAP: CORE_TO_CORE_MESSAGE, service: C2C_STATISTICS). At APPS, it will add
// its data, preper the message as COMM messge (trace, with message ID of the correct C123
// (sent from COMM in the AP_bytes_recieved_from_CP field).




//ICAT EXPORTED STRUCT
typedef struct {
 UINT32 CP_TimeStamp; // time stamp in COMM when X bytes (or more) were produced
 UINT32 CP_bytes_produced; // number of bytes produced in traces in the last period
 UINT32 CP_bytes_dropped; // number of bytes produced but discarded (due to congestion) in the last period
 UINT32 CP_bytes_sent; // number of bytes sent out on the internal channel in the last period
 UINT32 CP_bytes_sent_tot_time; // total time to send all bytes in the period (each send has start/end TS, delta is added to this counter)
 UINT32 CP_max_time_byteOint; // max time to send a byte over internal interface
 UINT32 AP_TimeStamp; // time stamp in APPS when the message arrived over internal interface and processed
 UINT32 AP_bytes_produced; // number of bytes produced in traces (on APPS or coming from COMM) in the last period
 UINT32 AP_bytes_dropped; // number of bytes produced but discarded (due to congestion) in the last period
 UINT32 AP_bytes_sent; // number of bytes sent out on the external channel in the last period
 UINT32 AP_bytes_sent_tot_time; // total time to send all bytes in the period (each send has start/end TS, delta is added to this counter)
 UINT32 AP_max_time_byteOext; // max time to send a byte over external interface
 UINT32 AP_bytes_recieved_from_CP; // total time to send all bytes in the period (each send has start/end TS, delta is added to this counter)
 // Fields added after first diag release of stats (rel 04.28) - not exist in all versions!! must be checked for.
 UINT16 CP_struct_size; // size of the statistics structure on CP side (data allows for backward/forward compatibility)
 UINT16 AP_struct_size; // size of the statistics structure on AP side (data allows for backward/forward compatibility)
 UINT32 CP_bytes_added_INTif; // bytes added for sending over INT if
 UINT32 AP_bytes_removed_INTif; // bytes recieved from CP and removed (used for IntIF protocol only)
 UINT32 AP_bytes_added_ExtIf; // bytes added on external interface
} DiagStats_CP_AP_S;

typedef struct {
 UINT32 CP_trace_bytes_for_statistics_period;
 UINT32 CP_trace_statistics;
 UINT32 Tx_Start_TS;
 UINT32 bytes_in_this_tx;
 UINT32 bytes_INTifOnly_in_this_tx;
} DiagStats_Internal_S;

extern DiagStats_CP_AP_S d_stat_CP_AP;
extern DiagStats_Internal_S d_stat_internal;

/* static declaraion...
   extern DiagReportItem_S	_diagExtIfReportsList[];
   extern DiagQPtrData_S	_diagExtIfQPtrData;     // additional pointers can be maintained by DiagComm
 #if defined (DIAG_INT_IF)
   extern DiagReportItem_S	_diagIntIfReportsList [];
   extern DiagQPtrData_S	_diagIntIfQPtrData;
 #endif
 */
// Q of Tx on interface is dyanmically allocated with different Q size per interface
extern DiagReportItem_S *_diagExtIfReportsList;
extern DiagQPtrData_S *_diagExtIfQPtrData; // additional pointers can be maintained by DiagComm

//CMI support
extern DiagReportItem_S * _diagCMIfReportsList;
extern DiagQPtrData_S * _diagCMIfQPtrData;

//this is used for the internal interface and the data structure is allocated only if the internal interface is working
extern DiagReportItem_S *_diagIntIfReportsList;
extern DiagQPtrData_S *_diagIntIfQPtrData;

// special quick send Q
extern DiagReportItem_S _diagIntIfQQReportsList[10];
extern DiagQPtrData_S _diagIntIfQQPtrData;

extern UINT8 _diagIntIfQQTraceBuffer[10][(((sizeof(DiagStats_CP_AP_S) + 16 + 6 + 3) / 4) * 4)];


//ICAT EXPORTED STRUCT
typedef struct {
 UINT32 diagMaxMsgOutLimit;
 UINT32 diagMaxMsgOutWarning;
 UINT32 diagMaxMsgInLimit;
 UINT32 diagMaxMsgInWarning;
} diagMsgLimitSet_S;

typedef struct {
 UINT32 _diagNumberOfTracesExtQLost;
 UINT32 _diagNumberOfTracesIntQLost;
 UINT32 _diagNumberOfTracesCMIQLost;

 UINT32 _diagNumberOfTracesMEMLost;

 UINT32 diagNumberOfTracesExtQLostTotal;
 UINT32 diagNumberOfTracesExtQLostBytes;
 UINT32 diagNumberOfTracesIntQLostTotal;
 UINT32 diagNumberOfTracesIntQLostBytes;
 UINT32 diagNumberOfTracesCMIQLostTotal;
 UINT32 diagNumberOfTracesCMIQLostBytes;

 UINT32 diagNumberOfTracesMEMLostTotal;
 UINT32 diagNumberOfTracesMEMLostBytes;

 UINT32 diagNumberOfTraceRxTaskLostTotal;
 UINT32 diagNumberOfTraceRxTaskLostBytes;

 UINT32 diagMsgsLost_MemAlloc;

 UINT32 diagMsgsLost_ExtIfQueueIsFull;
 UINT32 diagMsgsLost_IntIfQueueIsFull;
 UINT32 diagMsgsLost_CMIifQueueIsFull; // for CMI interface

 diagMsgLimitSet_S MsgLimits;
 /*
	   UINT32 diagMaxMsgOutLimit;
	   UINT32 diagMaxMsgOutWarning;
	   UINT32 diagMaxMsgInLimit;
	   UINT32 diagMaxMsgInWarning;
	 */
} _diagIntData_S;
extern _diagIntData_S _diagInternalData;

extern UINT32 flagKickExtTx;
extern UINT32 flagKickIntTx;
extern UINT32 flagKickCMITx; //TBDIY CMI support

// Public service invoked by all DiagTX layers
// These functions are re-entrable - Check for space in the Tx-Q (for Ext-if) and add final data (counter,checksum)
//extern void diagBufferPDUExtIf(UINT8 *sendData, UINT16 pduLength, Allocation_Type allocType, UINT32 traceFromLocalCore);
//extern void diagBufferPDUIntIf(UINT8 *sendData, UINT16 pduLength, Allocation_Type allocType, UINT32 traceFromLocalCore);
//extern void diagBufferPDUCMIf (UINT8 *sendData, UINT16 pduLength, Allocation_Type allocType, const UINT8 clientID /*TBDIY no traces via CMI clients - only commands, UINT32 traceFromLocalCore*/);

// This function sets a new Tx message counter pointer, which will not be
// counting up by the diag, and will be sent to ICAT instead of the usual counter.
void SetDiagTxMsgCounterPtr(UINT16* NewCounterPtr);
extern void diagSendErrMsg(UINT32 connectedDev, diagErrStatesE err_state, UINT32 num_lost /* zero - problem started*/);
extern void diagSendErrLenMsg(UINT32 connectedDev, diagErrStatesE err_state, UINT32 length, UINT32 msgID );

// Function to allocated buffer for message + handle error condition
// on allocation failure
//extern void DiagReleaseBufferMemory (Allocation_Type allocType, UINT8* pMem);

extern void updateStatsOnApps(UINT8 *bufData, UINT32 len);
# 88 "diag_comm_EXTif_OSA_LINUX.c" 2

# 1 "diag_osif.h" 1
/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/
# 90 "diag_comm_EXTif_OSA_LINUX.c" 2


# 1 "../../include/pxa_dbg.h" 1
/******************************************************************************

*(C) Copyright 2007 Marvell International Ltd.

* All Rights Reserved

******************************************************************************/



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/stdio.h" 1
/*	$OpenBSD: stdio.h,v 1.35 2006/01/13 18:10:09 miod Exp $	*/
/*	$NetBSD: stdio.h,v 1.18 1996/04/25 18:29:21 jtc Exp $	*/

/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Chris Torek.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)stdio.h	5.17 (Berkeley) 6/3/91
 */
# 12 "../../include/pxa_dbg.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/syslog.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/stdio.h" 1
/*	$OpenBSD: stdio.h,v 1.35 2006/01/13 18:10:09 miod Exp $	*/
/*	$NetBSD: stdio.h,v 1.18 1996/04/25 18:29:21 jtc Exp $	*/

/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Chris Torek.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)stdio.h	5.17 (Berkeley) 6/3/91
 */
# 32 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/syslog.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/cdefs.h" 1
/*	$NetBSD: cdefs.h,v 1.58 2004/12/11 05:59:00 christos Exp $	*/

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)cdefs.h	8.8 (Berkeley) 1/9/95
 */
# 33 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/syslog.h" 2
# 1 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stdarg.h" 1 3 4
/* Copyright (C) 1989, 1997, 1998, 1999, 2000, 2009 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

Under Section 7 of GPL version 3, you are granted additional
permissions described in the GCC Runtime Library Exception, version
3.1, as published by the Free Software Foundation.

You should have received a copy of the GNU General Public License and
a copy of the GCC Runtime Library Exception along with this program;
see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
<http://www.gnu.org/licenses/>.  */

/*
 * ISO C Standard:  7.15  Variable arguments  <stdarg.h>
 */
# 36 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stdarg.h" 3 4
/* Define __gnuc_va_list.  */






/* Define the standard macros for the user,
   if this invocation was from the user program.  */
# 55 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stdarg.h" 3 4
/* Define va_list, if desired, from __gnuc_va_list. */
/* We deliberately do not define va_list when called from
   stdio.h, because ANSI C says that stdio.h is not supposed to define
   va_list.  stdio.h needs to have access to that data type, 
   but must not use that name.  It should use the name __gnuc_va_list,
   which is safe because it is reserved for the implementation.  */
# 89 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stdarg.h" 3 4
/* The macro _VA_LIST_ is the same thing used by this file in Ultrix.
   But on BSD NET2 we must not test or define or undef it.
   (Note that the comments in NET 2's ansi.h
   are incorrect for _VA_LIST_--see stdio.h!)  */

/* The macro _VA_LIST_DEFINED is used in Windows NT 3.5  */

/* The macro _VA_LIST is used in SCO Unix 3.2.  */

/* The macro _VA_LIST_T_H is used in the Bull dpx2  */

/* The macro __va_list__ is used by BeOS.  */

typedef __gnuc_va_list va_list;
# 34 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/syslog.h" 2



/* Alert levels */
# 51 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/syslog.h"
/* Facilities; not actually used */
# 79 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/syslog.h"
/* openlog() flags; only LOG_PID and LOG_PERROR supported */







/* BIONIC: the following definitions are from OpenBSD's sys/syslog.h
 */
struct syslog_data {
 int log_file;
        int connected;
        int opened;
        int log_stat;
        const char *log_tag;
        int log_fac;
        int log_mask;
};





extern void closelog(void);
extern void openlog(const char *, int, int);
extern int setlogmask(int);
extern void syslog(int, const char *, ...);
extern void vsyslog(int, const char *, va_list);
extern void closelog_r(struct syslog_data *);
extern void openlog_r(const char *, int, int, struct syslog_data *);
extern int setlogmask_r(int, struct syslog_data *);
extern void syslog_r(int, struct syslog_data *, const char *, ...);
extern void vsyslog_r(int, struct syslog_data *, const char *, va_list);


# 13 "../../include/pxa_dbg.h" 2







extern int eeh_draw_panic_text(const char *panic_text, size_t panic_len, int do_timer);
extern void eeh_log(int level, const char *format, ...);
# 93 "diag_comm_EXTif_OSA_LINUX.c" 2
//#include "diag_mem.h"
# 1 "diag_comm.h" 1
/*------------------------------------------------------------
(C) Copyright [2006-2008] Marvell International Ltd.
All Rights Reserved
------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
INTEL CONFIDENTIAL
Copyright 2006 Intel Corporation All Rights Reserved.
The source code contained or described herein and all documents related to the source code ("Material") are owned
by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
transmitted, distributed, or disclosed in any way without Intel's prior express written permission.

No license under any patent, copyright, trade secret or other intellectual property right is granted to or
conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
Intel in writing.
-------------------------------------------------------------------------------------------------------------------*/




# 1 "../../include/global_types.h" 1
/******************************************************************************

*(C) Copyright 2007 Marvell International Ltd.

* All Rights Reserved

******************************************************************************/
/****************************************************************************
 *
 * Name:          global_types.h
 *
 * Description:   Standard type definitions
 *
 ****************************************************************************
 *
 *
 *
 *
 ****************************************************************************
 *                  Copyright (c) Intel 2000
 ***************************************************************************/
# 26 "diag_comm.h" 2
//#include "ICAT_config.h"
# 1 "diag_osif.h" 1
/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/
# 28 "diag_comm.h" 2
# 42 "diag_comm.h"
/*This define allows the file diag_bsp.cfg and FS logs to be moved under diag_marvell_rw_dir
This is  not always a good decission, specially when supporting old systems, if not defined the file will stay under /tel or /etc */
//#define DIAG_ALLOW_CFGFILE_REPLACEMENT 1
# 60 "diag_comm.h"
/* DIAG CMM Service IDs */




// MAX tx interfaces


// MAX number of clients for split (distributed) diag mode


// we have maximum of 3 types of interfaces: for ext-interface, internal-interface and CMI-interface
// but in CMI mode, we can recieve data from all clients (should be aligned with DIAG_COMDEV_MAX_CLIENTS)
//ICAT EXPORTED ENUM
typedef enum {
 DIAG_COMMDEV_EXT, //EXT is also for clients that communicate with the master CMI
 DIAG_COMMDEV_INT,
 DIAG_COMMDEV_CMI,
 // for client-master  - up to 5 clients
 DIAG_COMMDEV_CMI1 = DIAG_COMMDEV_CMI,
 DIAG_COMMDEV_CMI2,
 DIAG_COMMDEV_CMI3,
 DIAG_COMMDEV_CMI4,
 DIAG_COMMDEV_CMI5,
 DIAG_COMMDEV_RX_MAX = DIAG_COMMDEV_CMI5,

 //This enum is used also in UINT8 DiagReportItem_S.clientID and we want to limit the size to 8 bits.
 DIAG_COMMDEV_NODEVICE = 0xEF, // large enough value not to be a valid rx interface...
 DIAG_COMMDEV_DUMMY = 0x1FFFFFFF // to keep 4 bytes alignment in structs
} COMDEV_NAME;
# 95 "diag_comm_EXTif_OSA_LINUX.c" 2
# 1 "diag_rx.h" 1
/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code ("Material") are owned
   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
   transmitted, distributed, or disclosed in any way without Intel's prior express written permission.

   No license under any patent, copyright, trade secret or other intellectual property right is granted to or
   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
   Intel in writing.
   -------------------------------------------------------------------------------------------------------------------*/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*                                                                                 *
*     File name:      diag_rx  .h                                                 *
*     Programmer:     Shiri Dolev                                                 *
*                                                                                 *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*                                                                                 *
*       Create Date:  November, 2003                                              *
*                                                                                 *
*       Description: DIAG Rx Definitions - Handle the receive commands functions. *                                                                                 *
*                                                                                 *
*       Notes:                                                                    *
*                                                                                 *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */




/****************************
*			Defines			*
****************************/



/*****************************************/
/***    INTERNAL SERVICES           ******/
/***    (special SAP (0) and serviceID  **/
/***                                    **/
/*****************************************/
# 63 "diag_rx.h"
// DiagComm now passes the length of Diag PDU and the pointer to it (USB 32-bit overhead is not visible)
// used in OLD header... and in extIf to verify input from USB...


// SAP mask is the bits relevant to the SAP value, as in this
// field, some bits are/can-be used for special signals
// ICAT MSG SAP is 8 bits, currently in use 1000 1111






/////////////////////////////////////////////////
/********************************
*			Typedefs			*
********************************/

typedef void (*FunctionPtr)(void *, UINT32); // call by pointer to function
typedef void (*RSVPFunctionPtr)(void *, UINT32, UINT32); // call by pointer to function

typedef union {
 UINT8 packedMsgData[13];
 UINT8 msgData[13];
}PackedUnPackedData;


typedef struct
{
 UINT32 msgLength;
 PackedUnPackedData pdu;
}RxMsgRec;

typedef struct
{
 UINT16 totalLength;
 BOOL isChecksum;
 UINT16 headerLength;
 UINT16 headerVersion;
 UINT8 source;
 UINT8 destination;
 UINT16 messageSAP;
 UINT8 numOfPaddingBytes;
 BOOL isSapRelatedData;
 BOOL isTimeStamp;
 BOOL isFrameNumber;
 BOOL isMsgCounter;
 UINT32 sapRelatedData;
 UINT32 timeStamp;
 UINT32 frameNumber;
 UINT32 msgCounter;
 UINT8 *payloadData;
 UINT8 *msg;
}RxMsgHolder;

typedef enum
{
 waitForSoT,
 waitForTotalLength,
 waitForHeaderLength,
 waitForHeaderVersion,
 waitForSrc,
 waitForDest,
 waitForSAP,
 waitForEndOfData,
 waitForChecksum,
 waitForEoT
} PacketHandlingState;

typedef struct
{
 PacketHandlingState currentState;
 UINT16 remaminingBytes;
 BOOL isChecksum;
 UINT16 checksum;
} PacketHandlingStatus;

typedef struct
{




 UINT32 rxIF;
 UINT32 dataLen;
 UINT8 *data;
} pLongMsg;

typedef struct {
 UINT16 moduleID;
 UINT16 commandID;
 UINT8 isFiltered;
 //int isFiltered;
}reportFilterStatus;

// DiagComm function called by DiagBuff to initiate TX after idle period
typedef enum {
 DIAG_COM_CompletePacketState,
 DIAG_COM_PacketFirstChunkState,
 DIAG_COM_PacketNextChunkState,
 DIAG_COM_PacketLastChunkState
}DIAG_COM_RX_PacketHandlingState;

typedef struct
{
 UINT32 rxIF;
 CHAR * buffer; //_diagUSBRxLisrMsgDataPtr;
 UINT16 data_offset; //bytes offset in buffer
 //TBDIY to remove the header and get a clean Diag Rx PDU
 UINT16 total_bytes_expected; //total packet length in bytes
 UINT16 total_bytes_received; //indicates bytes received at every moment
 UINT16 current_chunk_bytes; //length of last received chcunk of a packet in bytes
 UINT32 nReTriesToPerform; // number of retry to perform on failure of diagRecieveCommand
 // failure can be due to no-memory or diag-rx-queue full
 BOOL end_of_packet;
 DIAG_COM_RX_PacketHandlingState handling_state;
}DIAG_COM_RX_Packet_Info;

typedef struct
{
 BOOL bIsIfBusy;
 UINT32 NBuffTxNotify; // keeps number of tx-notifications bufferes tillhandled in task
 DIAG_COM_RX_Packet_Info *RxPacket;
 void* TxFlgRef;
 void* TxTaskRef;
 void* RxFlgRef;
 void* RxTaskRef;
} GlobalIfData;


/********************************
*			Prototypes			*
********************************/







extern UINT32 b_ExportedPtrStat; //Enable or disable the print of the exported function pointer when invoked
# 96 "diag_comm_EXTif_OSA_LINUX.c" 2
//#include "diag_comm_L2.h"
# 1 "diag_comm_L4.h" 1
/*------------------------------------------------------------
    (C) Copyright [2006-2008] Marvell International Ltd.
    All Rights Reserved
------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
    INTEL CONFIDENTIAL
    Copyright 2006 Intel Corporation All Rights Reserved.
    The source code contained or described herein and all documents related to the source code ("Material") are owned
    by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
    its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
    Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
    treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
    transmitted, distributed, or disclosed in any way without Intel's prior express written permission.

    No license under any patent, copyright, trade secret or other intellectual property right is granted to or
    conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
    estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
    Intel in writing.
-------------------------------------------------------------------------------------------------------------------*/




# 1 "diag_comm.h" 1
/*------------------------------------------------------------
(C) Copyright [2006-2008] Marvell International Ltd.
All Rights Reserved
------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
INTEL CONFIDENTIAL
Copyright 2006 Intel Corporation All Rights Reserved.
The source code contained or described herein and all documents related to the source code ("Material") are owned
by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
transmitted, distributed, or disclosed in any way without Intel's prior express written permission.

No license under any patent, copyright, trade secret or other intellectual property right is granted to or
conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
Intel in writing.
-------------------------------------------------------------------------------------------------------------------*/
# 26 "diag_comm_L4.h" 2

typedef enum
{
    HS_IDLE = 0,
    HS_ACK = 2,
    HS_RESET = 4,
    HS_RESET_ACK = 8

//    HS_ALIGN_16=0xFFFF  // used to make this enum 16 bit in size

}HandshakeMessageEnum;

typedef UINT32 (*DiagHSprotocolSendCB)(UINT8 *, UINT32);

extern void diagCommL4Init (COMDEV_NAME rxif);
extern BOOL diagCommL4ReceiveL2FullMessage (UINT8 *buff , UINT32 length, COMDEV_NAME rxif);
extern BOOL diagCommL4ReceiveL2Packet (UINT8 *buff , UINT32 length, COMDEV_NAME rxif);
extern void HandleHandshakeSequence (UINT8 msgType, COMDEV_NAME rxif);
extern UINT32 SendHandshakeMessage (HandshakeMessageEnum handshake_msg, COMDEV_NAME rxif);
extern UINT32 diagL4InitHSprotocolWithCB(COMDEV_NAME rxif, DiagHSprotocolSendCB cbFunc);
# 98 "diag_comm_EXTif_OSA_LINUX.c" 2

//#include "diag.h"
//#include "diag_ram.h"
//#include "diag_buff.h"
//#include "diag_comm_EXTif.h"


# 1 "diag_mmi_if_OSA_LINUX.h" 1
/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/

/*********************************************************************
 *                      M O D U L E     B O D Y                       *
 **********************************************************************
 * Title: Diag                                                        *
 *                                                                    *
 * Filename: diag_mmi_if_OSA_LINUX.h                                     *
 *                                                                    *
 * Target, platform: Common Platform, SW platform                     *
 *                                                                    *
 * Authors: Shuki Zanyovka											 *
 *                                                                    *
 * Description: handles Linux NETLINK event for insertion/removal     *
 *              of MMC/SD cards										 *
 *                                                                    *
 * Notes:                                                             *
 **********************************************************************/




# 1 "../../osa/inc/osa.h" 1
/* ===========================================================================
   File        : osa.h
   Description : This file defines the Intel CCD OS wrapper API and
	      definitions for external application use.

	      The OSA API allows applications to be developed independent of
	      the target microkernel/hardware environment. It provides the
	      facility to add support for multiple independent operating
	      systems from different vendors.

    The OSA API defines a set of interfaces that provide the following
    basic operating system services:

      OS Management     - OSAInitialize(), OSARun()
      Tasks             - OSATaskCreate(), OSATaskDelete(), OSATaskChangePriority(),
			- OSATaskGetPriority(), OSATaskSuspend(), OSATaskResume(),
			- OSATaskSleep(), OSATaskGetCurrentRef()
      Event Flags       - OSAFlagCreate(), OSAFlagDelete(), OSAFlagSet(),
			- OSAFlagWait(), OSAFlagPeek()
      Timers            - OSATimerCreate(), OSATimerDelete(), OSATimerStart(),
			- OSATimerStop(), OSATimerGetStatus()
      Messaging         - OSAMsgQCreate(), OSAMsgQDelete(),
			- OSAMsgQSend(), OSAMsgQRecv(), OSAMsgQPoll()
			- OSAMsgQCreateWithMem()
      Mailboxes         - OSAMailboxQCreate(), OSAMailboxQDelete(),
			- OSAMailboxQSend(), OSAMailboxQRecv(), OSAMailboxQPoll()
      Semaphores        - OSASemaphoreCreate(), OSASemaphoreDelete(),
			- OSASemaphoreAcquire(), OSASemaphoreRelease(),
			- OSASemaphorePoll()
      Mutexes           - OSAMutexCreate(), OSAMutexDelete(),
			- OSAMutexLock(), OSAMutexUnlock(),
      Interrupt Control - OSAIsrCreate(), OSAIsrDelete(),
			- OSAIsrEnable(), OSAIsrDisable(), OSAIsrNotify()
      Real-Time Clock   - OSAGetTicks(), OSAGetClockRate(), OSATick()
			- OSAGetSystemTime()
      Memory            - OSAMemPoolCreate(), OSAMemPoolDelete(),
			- OSAMemPoolAlloc(), OSAMemPoolFree(),
			- OSAMemPoolFixedFree()
      Thread local storage
			- OSATaskSetTls()
			- OSATaskGetTls()
      Symbian Memory    - OSAMemPoolCreateGlobal(), OSAGlobalAlloc(),
			- OSAGlobalRead(), OSAGlobalWrite()

   Copyright (c) 2001 Intel CCD. All Rights Reserved
   =========================================================================== */
# 27 "diag_mmi_if_OSA_LINUX.h" 2
# 1 "../../include/global_types.h" 1
/******************************************************************************

*(C) Copyright 2007 Marvell International Ltd.

* All Rights Reserved

******************************************************************************/
/****************************************************************************
 *
 * Name:          global_types.h
 *
 * Description:   Standard type definitions
 *
 ****************************************************************************
 *
 *
 *
 *
 ****************************************************************************
 *                  Copyright (c) Intel 2000
 ***************************************************************************/
# 28 "diag_mmi_if_OSA_LINUX.h" 2
# 1 "../../include/linux_types.h" 1
/* ===========================================================================
   File        : posix_types.h
   Description : Global types file for the POSIX Linux environment.

   Notes       : This file is designed for use in the gnu environment
	      and is referenced from the gbl_types.h file. Use of
			  this file requires ENV_POSIX to be defined in posix_env.mak.

   Copyright 2005, Intel Corporation, All rights reserved.
   =========================================================================== */
# 74 "../../include/linux_types.h"
/*                         end of posix_types.h
   --------------------------------------------------------------------------- */
# 29 "diag_mmi_if_OSA_LINUX.h" 2
//#include "diag_comm.h"
# 1 "diag_init.h" 1
/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code ("Material") are owned
   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
   transmitted, distributed, or disclosed in any way without Intel's prior express written permission.

   No license under any patent, copyright, trade secret or other intellectual property right is granted to or
   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
   Intel in writing.
   -------------------------------------------------------------------------------------------------------------------*/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **
 *                                                                                 *
 *     File name:      diag_init.h                                                 *
 *     Programmer:     Shiri Dolev                                                 *
 *                                                                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                 *
 *       Create Date:  November, 2003                                              *
 *                                                                                 *
 *       Description: DIAG Package Inits.                                          *
 *                                                                                 *
 *       Notes:                                                                    *
 *                                                                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */



# 1 "diag_types.h" 1
/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code ("Material") are owned
   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
   transmitted, distributed, or disclosed in any way without Intel's prior express written permission.

   No license under any patent, copyright, trade secret or other intellectual property right is granted to or
   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
   Intel in writing.
   -------------------------------------------------------------------------------------------------------------------*/
# 40 "diag_init.h" 2
# 1 "../../include/global_types.h" 1
/******************************************************************************

*(C) Copyright 2007 Marvell International Ltd.

* All Rights Reserved

******************************************************************************/
/****************************************************************************
 *
 * Name:          global_types.h
 *
 * Description:   Standard type definitions
 *
 ****************************************************************************
 *
 *
 *
 *
 ****************************************************************************
 *                  Copyright (c) Intel 2000
 ***************************************************************************/
# 41 "diag_init.h" 2
//#include "diag_comm.h"
# 1 "diag_API.h" 1
/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code ("Material") are owned
   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
   transmitted, distributed, or disclosed in any way without Intel's prior express written permission.

   No license under any patent, copyright, trade secret or other intellectual property right is granted to or
   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
   Intel in writing.
   -------------------------------------------------------------------------------------------------------------------*/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*                                                                                 *
*     File name:      diag_api .h                                                 *
*     Programmer:     Shiri Dolev                                                 *
*                     Ohad Peled, Itzik Yankilevich, Barak Witkowsky, Yaeli Karni *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*                                                                                 *
*       Create Date:  November, 2003                                              *
*                                                                                 *
*       Description: DIAG Tx APIs - MACROs.									   *
*                                                                                 *
*       Notes:  There are 2 files diag_API_var.h and diag_API.h                   *
*               The diag_API_var.h - declaration without MACRO-re-define          *
*               The diag_API.h     - has part which is re-defined by MACROs       *
*                                    and treated differently by PrePassProcessor! *
*                                                                                 *
* July 2007 - fix macros DIAT_TRACEM... (IY)									   *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
# 43 "diag_init.h" 2
# 60 "diag_init.h"
/*-- Public Functions Prototypes ---------------------------------------*/

/************************
*		Typedefs		*
************************/
typedef void (*DiagICATReadyNotifyEventFunc)(void);
typedef void (*DiagEnterBootLoaderCBFunc)(void);
typedef void (*Diag_Open_Func)(void);
typedef int (*EventSocket_Open_Func)(void);
typedef void (*Diag_Read_Task)(void *);
typedef int (*Diag_Send_Target)(UINT8 *, UINT32);

extern DiagBlockReasons diagBlocked;

/************************
*		Prototypes		*
************************/
extern void diagICATReayNotifyEventDefaultFunc(void);
extern void diagICATReadyNotifyEventBind(DiagICATReadyNotifyEventFunc diagICATReadyFunc);
extern void diagEnterBootLoaderCBFuncBind(DiagEnterBootLoaderCBFunc diagBootLoaderFunc);
extern void diagInit(void);
extern void diagRxInit(void);
extern void diagTxInit(void);
extern void diagTestInit(void);
extern SwVersion diagGetVersion(void);

/*
   enum for defintions of ext-if type. In WM, the ext-if can be USB/UDP/TCP
   when moving to new-header, we can enhance it to include all types of ext-if (
   also UART, SSP..)
 */
//ICAT EXPORTED ENUM
typedef enum
{
 // for external interfaces
 tNoConnection,
 tUSBConnection,
 tTCPConnection,
 tUDPConnection,
 tUARTConnection,
 tSSPConnection,
 tVIRTIOConnection,
 // for internal interfaces
 tMSLConnection,
 tSHMConnection, // shared memory
 tLocalConnection
} EActiveConnectionType;

typedef enum
{
 tLL_none, // not set
 tLL_SSP, // external int. SSP
 tLL_UART, // external int. UART
 tLL_USB, // external int. USB
 tLL_ETHERNET, // external int. over Ethernet port
 tLL_LocalIP, // external int./CMI int. localIP
 tLL_ACIPC, // internal int. SHMEM
 tLL_SAL, // internal int. MSL o SAL
 tLL_GPC, // internal int. MSL o GPC
 tLL_FS, // external int. File System
 tLL_SC // external int. Storage Card (SD card)
} EActiveConnectionLL_Type;

//Global Variable used for configure the Linux SD
typedef enum
{
 FSTYPE_NO_MOUNT = 0,
 FSTYPE_VFAT = 1,
 FSTYPE_EXT2 = 2, //Not supported yet Diag05.00.24
 FSTYPE_EXT3 = 3, //Not supported yet Diag05.00.24
 FSTYPE_MSDOS = 4,

}SDFStypeE;

typedef struct {
 UINT32 log_auto_start; //MBTODO - For now this exist as global variable in Windows should change it
 UINT32 FS_Inc_Reset;
 SDFStypeE SD_FSType;

} DiagBSPFSConfigS;

typedef struct {
 UINT32 write_unit_size; //The minimum chunk size to be gathered before writing (unless writing is enforced by flush or timeout)
    //Cached write mechanism is disabled when write_unit_size==0
 UINT32 time_out; //Used in OSATimerStart
} DiagCachedWriteS;

// global variables to be usded by diag_logic, diag_comm
typedef struct {
 OSMsgQRef diagMsgQRef;

 EActiveConnectionType m_eConnectionType; // Extrernal connection type
 EActiveConnectionLL_Type m_eLLtype; // EXT type L2 of connection
 EActiveConnectionType m_eIntConnectionType; // Internal connection type
 EActiveConnectionLL_Type m_eIntLLtype; // INT type L2 of connection
 UINT32 m_extIfTxMulti; // does the extIf supports muti Tx (in one call to driver, send multi bufs)
 // default is 0 (not supported), currently (Nov 07) only Nu-USB/SSP has mutil mode

 UINT32 m_bWorkMultiCore; // Default, we work multi - core (MSL/Shared Memory, internal-connection is active) =1 or single core only (no router mode) =0
 UINT32 m_bMasterClientMode; //  Masgter-client
 UINT32 m_bIsMaster; // is master
 UINT32 m_bIsClient; // is client
 UINT32 m_bIsRawDiag; //is raw diag
 DiagICATReadyNotifyEventFunc diagICATReadyNotifyEventFunc;
 DiagPSisRunningFn _diagSigPsCheckFn; // PS status CB.
 DiagEnterBootLoaderCBFunc diagEnterBootLoaderCBFunc;

 DiagBSPFSConfigS diagBSPFScfg;
 DiagCachedWriteS diagCachedWrite;
 UINT32 nReTriesToPerform; // number of retry to perform on failure of diagRecieveCommand
} diagInternalDataS;
typedef struct{
 int hPort; //file decription or socket for connetction.
 int socket;
 Diag_Open_Func connect;
 EActiveConnectionLL_Type target_type;
 Diag_Read_Task read_target_task;
 Diag_Send_Target send_target;
 int raw_diag_support;
}Diag_Target;
extern diagInternalDataS diagIntData;
typedef struct{
 int event_socket;// event source
 int pipefd[2];
 EventSocket_Open_Func open;
}Diag_Event;
# 31 "diag_mmi_if_OSA_LINUX.h" 2

typedef enum
{
 MMI_REP_AUTO, //diag.cfg is configured to auto start mode
 MMI_REP_ALREADY_START, //got start when already in start
 MMI_REP_ALREADY_STOP, //got stop when already in stop
 MMI_REP_STOP_DONE, //stop process done
 MMI_REP_APPS_FILTER_DB_VER_MATCH, //the filter file apps DB version match the target
 MMI_REP_APPS_FILTER_DB_VER_MISMATCH, //the filter file apps DB version doesn't match the target
 MMI_REP_COMM_FILTER_DB_VER_MATCH, //the filter file comm DB version match the target
 MMI_REP_COMM_FILTER_DB_VER_MISMATCH, //the filter file comm DB version doesn't match the target
 MMI_REP_LSEEK, //overwriting the comm DB ver in the header
 MMI_REP_WRITE, //writing to a file
 MMI_REP_FOPEN, //opening the configuration file
 MMI_REP_OPEN, //opening a log file
 MMI_REP_FSYNC, //fsync() before closing a log file
 MMI_REP_CLOSE, //closing a log file
 MMI_REP_FCLOSE, //closing the configuration file
 MMI_REP_STAT, //non-existent directory for the log file
 MMI_REP_MKDIR //create the non-existent directory for the log file
} E_DiagOverFS_MMI_Report;


typedef enum
{
 MMI_IF_MOUNT_EVENT_CARD_ERROR,
 MMI_IF_MOUNT_EVENT_CARD_NOT_DETECTED,
 MMI_IF_MOUNT_EVENT_CARD_DETECTED,
 MMI_IF_MOUNT_EVENT_CARD_MOUNTED
} MMI_IF_Mount_Event_t;

typedef enum
{
 MMI_IF_EVENT_NONE,
 MMI_IF_EVENT_CARD_IN,
 MMI_IF_EVENT_CARD_OUT,
 MMI_IF_EVENT_REPORT_AUTO,
 MMI_IF_EVENT_REPORT_STOP_DONE,
 MMI_IF_EVENT_USER_START,
 MMI_IF_EVENT_USER_STOP
} MMI_IF_Event_t;

typedef enum
{
 MMI_IF_STATE_INIT,
 MMI_IF_STATE_INSERTED_STOPPED,
 MMI_IF_STATE_INSERTED_STARTED,
 MMI_IF_STATE_NOT_INSERTED_STOPPED,
 MMI_IF_STATE_NOT_INSERTED_STARTED,
 MMI_IF_STATE_MOUNTED_STOPPED,
 MMI_IF_STATE_MOUNTED_STARTED,
 MMI_IF_STATE_MOUNTING_MMI_STOPPED,
 MMI_IF_STATE_MOUNTING_MMI_STARTED,
 MMI_IF_STATE_UNMOUNTING_MMI_STOPPED,
 MMI_IF_STATE_UNMOUNTING_MMI_STARTED,
 MMI_IF_STATE_EXCEPTION_MMI_STOPPED,
 MMI_IF_STATE_EXCEPTION_MMI_STARTED
} MMI_IF_state_t;

typedef void (*MMI_STOP_CB_Func)(void);
typedef void (*MMI_START_CB_Func)(const char* rel_path);
BOOL MMI_Init(MMI_START_CB_Func MMI_Start_CB, MMI_STOP_CB_Func MMI_Stop_CB, EActiveConnectionLL_Type LL_Type, DiagBSPFSConfigS* p_FSConfig );

void MMI_Report(E_DiagOverFS_MMI_Report rep, int err_code);
void MMI_process_eehcontrol_event_receive(void);
void readPath(void);
void MMI_process_eehcontrol_event_send(void);
void openEehwriteSocket(void);
void openEehReadSocket(void);
# 106 "diag_comm_EXTif_OSA_LINUX.c" 2
# 1 "diag_al.h" 1
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/signal.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
# 2 "diag_al.h" 2
# 12 "diag_al.h"
int diag_init(void);
void diag_sig_action(int signum, siginfo_t * info, void *p);
# 107 "diag_comm_EXTif_OSA_LINUX.c" 2
# 1 "diag_init.h" 1
/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code ("Material") are owned
   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
   transmitted, distributed, or disclosed in any way without Intel's prior express written permission.

   No license under any patent, copyright, trade secret or other intellectual property right is granted to or
   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
   Intel in writing.
   -------------------------------------------------------------------------------------------------------------------*/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **
 *                                                                                 *
 *     File name:      diag_init.h                                                 *
 *     Programmer:     Shiri Dolev                                                 *
 *                                                                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                 *
 *       Create Date:  November, 2003                                              *
 *                                                                                 *
 *       Description: DIAG Package Inits.                                          *
 *                                                                                 *
 *       Notes:                                                                    *
 *                                                                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
# 108 "diag_comm_EXTif_OSA_LINUX.c" 2
# 1 "../../osa/inc/osa_types.h" 1
/* ===========================================================================
   File        : osa_types.h
   Description : Data types file for the os/osa package

   Notes       :

   Copyright (c) 2001 Intel CCD. All Rights Reserved
   =========================================================================== */




# 1 "../../osa/inc/osa_config.h" 1
/******************************************************************************

*(C) Copyright 2007 Marvell International Ltd.

* All Rights Reserved

******************************************************************************/
/* ===========================================================================
   File        : osa_config.h
   Description : Configuration parameters for the
	      os/osa package.

	      The following are internal OS configuration or tuning parameters
	      that are used to configure the OS services. Each service
	      requires a certain amount of internal memory space to store
	      internal OS data for each instance of the service.

   Notes       : These values can be overridden in gbl_config.h
	      The range checks should be updated for each
	      parameter.

   Copyright (c) 2001 Intel CCD. All Rights Reserved
   =========================================================================== */
# 215 "../../osa/inc/osa_config.h"
/*                      end of osa_config.h
   --------------------------------------------------------------------------- */
# 14 "../../osa/inc/osa_types.h" 2
# 23 "../../osa/inc/osa_types.h"
/* Use RTT specific stuff */




/* Use Nucleus specific stuff */




/* Use WinCE specific stuff */




/* Use Symbian specific stuff */




/* Use POSIX specific stuff */

# 1 "../../osa/inc/osa_linux.h" 1
/* ===========================================================================
   File        : osa_linux.h
   Description : Definition of OSA Software Layer data types specific to the
	      POSIX OS.

   Notes       :

   Copyright (c) 2005 Intel CHG. All Rights Reserved
   =========================================================================== */






# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/stdio.h" 1
/*	$OpenBSD: stdio.h,v 1.35 2006/01/13 18:10:09 miod Exp $	*/
/*	$NetBSD: stdio.h,v 1.18 1996/04/25 18:29:21 jtc Exp $	*/

/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Chris Torek.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)stdio.h	5.17 (Berkeley) 6/3/91
 */
# 17 "../../osa/inc/osa_linux.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/stdlib.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
# 18 "../../osa/inc/osa_linux.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/pthread.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
# 19 "../../osa/inc/osa_linux.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/time.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
# 20 "../../osa/inc/osa_linux.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/unistd.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
# 21 "../../osa/inc/osa_linux.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/mman.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/cdefs.h" 1
/*	$NetBSD: cdefs.h,v 1.58 2004/12/11 05:59:00 christos Exp $	*/

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)cdefs.h	8.8 (Berkeley) 1/9/95
 */
# 32 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/mman.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/types.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
# 33 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/mman.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/arch-arm/asm/mman.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/asm-generic/mman.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 16 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/arch-arm/asm/mman.h" 2
# 34 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/mman.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/arch-arm/asm/page.h" 1
/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
# 35 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/mman.h" 2


# 47 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/mman.h"
extern void* mmap(void *, size_t, int, int, int, off_t);
extern int munmap(void *, size_t);
extern int msync(const void *, size_t, int);
extern int mprotect(const void *, size_t, int);
extern void* mremap(void *, size_t, size_t, unsigned long);

extern int mlockall(int);
extern int munlockall(void);
extern int mlock(const void *, size_t);
extern int munlock(const void *, size_t);
extern int madvise(const void *, size_t, int);

extern int mlock(const void *addr, size_t len);
extern int munlock(const void *addr, size_t len);

extern int mincore(void* start, size_t length, unsigned char* vec);


# 22 "../../osa/inc/osa_linux.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/semaphore.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */



# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/cdefs.h" 1
/*	$NetBSD: cdefs.h,v 1.58 2004/12/11 05:59:00 christos Exp $	*/

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)cdefs.h	8.8 (Berkeley) 1/9/95
 */
# 32 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/semaphore.h" 2



typedef struct {
    volatile unsigned int count;
} sem_t;



extern int sem_init(sem_t *sem, int pshared, unsigned int value);

extern int sem_close(sem_t *);
extern int sem_destroy(sem_t *);
extern int sem_getvalue(sem_t *, int *);
extern int sem_init(sem_t *, int, unsigned int);
extern sem_t *sem_open(const char *, int, ...);
extern int sem_post(sem_t *);
extern int sem_trywait(sem_t *);
extern int sem_unlink(const char *);
extern int sem_wait(sem_t *);

struct timespec;
extern int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout);


# 23 "../../osa/inc/osa_linux.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/errno.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
# 24 "../../osa/inc/osa_linux.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/string.h" 1
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
# 25 "../../osa/inc/osa_linux.h" 2
# 1 "../../include/gbl_types.h" 1
/* ===========================================================================
   File        : gbl_types.h
   Description : Global types file.

   Notes       : This file includes the types from the correct environment.
	      The environment is set via the ENV_<ENV> macro. This macro
	      is usually set in /env/<host>/build/<env>_env.mak.

   Copyright 2001, Intel Corporation, All rights reserved.
   =========================================================================== */
# 59 "../../include/gbl_types.h"
/*                         end of gbl_types.h
   --------------------------------------------------------------------------- */
# 26 "../../osa/inc/osa_linux.h" 2
# 1 "../../osa/inc/osa_types.h" 1
/* ===========================================================================
   File        : osa_types.h
   Description : Data types file for the os/osa package

   Notes       :

   Copyright (c) 2001 Intel CCD. All Rights Reserved
   =========================================================================== */
# 490 "../../osa/inc/osa_types.h"
/*                      end of osa_types.h
   --------------------------------------------------------------------------- */
# 27 "../../osa/inc/osa_linux.h" 2

/************************************************************************
 * External Interfaces
 ***********************************************************************/

extern UINT32 OSA_RtcCounter;
//extern CRITICAL_SECTION csOSAContextLock;

/*************************************************************************
* Constants
*************************************************************************/
# 47 "../../osa/inc/osa_linux.h"
/* Remain for backwards compatibility */
# 64 "../../osa/inc/osa_linux.h"
/*************************************************************************
* POSIX Specific Types
*************************************************************************/
typedef UINT32 OS_Hisr_t;
typedef UINT32 OS_MemPool_t;
typedef pid_t OS_Proc_t;

typedef void (*funcPtrType)(void*);
typedef struct
{
 pthread_t task;
 pthread_attr_t attr;
 funcPtrType taskStartPtr;
 void *argv;
 UINT8 priority;
 UINT8 filler[3];
} OS_Task_t;

/* Semaphore reference */
typedef struct
{
 sem_t os_sema;
 UINT32 count;
} OS_Sema_t;

/* Mutex reference */
typedef struct
{
 pthread_mutex_t os_mutex;
 pthread_mutexattr_t attr;
 UINT32 owner;
 UINT32 locked;
} OS_Mutex_t;

/* Flag reference */
typedef struct
{
 UINT32 flag;
 OS_Sema_t semaphore;
} OS_Flag_t;

/* Partition memory pool reference */
typedef struct
{
 struct _OS_PartMemHdr *free; /* Free partition list */
 OS_Mutex_t mutex; /* Mutex to secure access */
 OS_Sema_t sema; /* Sema for blocking when no mem left */
 UINT32 numFree; /* Number of partitions free */
 void *chunk;
} OS_PartitionPool_t;

/* Partition memory pool header */
typedef struct _OS_PartMemHdr
{
 struct _OS_PartMemHdr *next;
 OS_PartitionPool_t *pool;




} OS_PartMemHdr;


/* Message Queue reference */
typedef struct
{
 int os_queue;
 UINT32 id;
 UINT32 maxSize;
 UINT32 maxNum;
 UINT32 msgCount;
} OS_MsgQ_t;

/* Linux Messaging requires a struct to be sent */
typedef struct
{
 UINT32 type;
 char *msg;
} OS_Msg;

/* Mailbox Queue reference */
typedef struct
{
 int os_queue;
 UINT32 id;
 UINT32 maxSize;
 UINT32 maxNum;
 UINT32 msgCount;
} OS_Mbox_t;
# 214 "../../osa/inc/osa_linux.h"
/* timer information */
typedef void (*timerCBPtrType)(UINT32);
typedef struct
{
 UINT32 os_ref;
 UINT32 state;
 UINT32 ticksLeft;
 UINT32 reschedTicks;
 UINT32 expirations;
 timerCBPtrType callback;
 UINT32 timerArgc;
} OS_Timer_t;


/*
** OS System call prototypes
*/
void OSA_Initialize(void*);

OSA_STATUS OSA_TaskCreate( OS_Task_t *, UINT8 *, UINT8, UINT32, void (*)(void*), void *);
OSA_STATUS OSA_TaskDelete( OS_Task_t task );
OSA_STATUS OSA_TaskSuspend( OS_Task_t task );
OSA_STATUS OSA_TaskResume( OS_Task_t task );
OSA_STATUS OSA_TaskGetPriority( OS_Task_t *task, UINT8 *oldPriority);
OSA_STATUS OSA_TaskChangePriority( OS_Task_t *task, UINT8 newPriority, UINT8 *oldPriority);
void* OSA_TaskStartWrapper(void *argv);

OSA_STATUS OSA_QueueCreate( OS_MsgQ_t *msgQRef, char *queueName, UINT32 maxSize, UINT32 maxNumber, void *queueAddr, UINT8 waitingMode );
OSA_STATUS OSA_QueueDelete( OS_MsgQ_t *msgQRef );
OSA_STATUS OSA_QueueSend( OS_MsgQ_t *msgQRef, UINT32 size, UINT8 *msg, UINT32 timeout);
OSA_STATUS OSA_QueueRecv( OS_MsgQ_t *msgQRef, UINT8 *msg, UINT32 size, UINT32 timeout );
OSA_STATUS OSA_QueuePoll( OS_MsgQ_t *msgQRef, UINT32 *msgCount );

OSA_STATUS OSA_FlagCreate( OS_Flag_t *flagRef, char * );
OSA_STATUS OSA_FlagDelete( OS_Flag_t *flagRef);
OSA_STATUS OSA_FlagSetBits( OS_Flag_t *flagRef, UINT32 mask, UINT32 operation);
OSA_STATUS OSA_FlagWait( OS_Flag_t *flagRef, UINT32 mask, UINT32 operation, UINT32* flags, UINT32 timeout );

OSA_STATUS OSA_SemaCreate( OS_Sema_t *sema, char *, UINT32 initialCount, UINT8 waitingMode );
OSA_STATUS OSA_SemaDelete( OS_Sema_t *sema );
OSA_STATUS OSA_SemaAcquire( OS_Sema_t *sema, UINT32 timeout );
OSA_STATUS OSA_SemaRelease( OS_Sema_t *sema );
OSA_STATUS OSA_SemaPoll( OS_Sema_t *sema, UINT32* count );

OSA_STATUS OSA_TimerCreate( OS_Timer_t *timer );
OSA_STATUS OSA_TimerDelete( OS_Timer_t *timer );
OSA_STATUS OSA_TimerStart( OS_Timer_t * timer, UINT32 time, UINT32 resched, void (*handler)(UINT32), UINT32 argc );
OSA_STATUS OSA_TimerStop( OS_Timer_t *timer );
OSA_STATUS OSA_TimerGetStatus( OS_Timer_t *timer, OSATimerStatus* status );

void OSA_ClockTick(void *block);
//UINT32 OSA_GetTicks(void);

OSA_STATUS OSA_MutexCreate( OS_Mutex_t *mutex, char* name );
OSA_STATUS OSA_MutexDelete( OS_Mutex_t *mutex );
OSA_STATUS OSA_MutexLock( OS_Mutex_t *mutex, UINT32 timeout );
OSA_STATUS OSA_MutexUnlock( OS_Mutex_t *mutex );

OSA_STATUS OSA_MemPoolCreate( void *pool, char *name, UINT32 poolType, UINT8 *poolBase, UINT32 poolSize, UINT32 partitionSize);
OSA_STATUS OSA_MemPoolDelete( void *pool, UINT32 poolType );
OSA_STATUS OSA_MemAlloc( void *pool, UINT32 poolType, UINT32 size, UINT32 partSize, void **mem, UINT32 timeout );
OSA_STATUS OSA_MemFree( void *pool, UINT32 poolType, void* mem );

BOOL OSA_GetControlBlock(void**, size_t, BOOL );
//OSA_STATUS OSA_FIsrCreate  ( UINT32, void(*)(UINT8) );
OSA_STATUS OSA_SIsrCreate( UINT32, OS_Hisr_t *, void (*)(void), UINT8, UINT8 *, UINT32 );
OSA_STATUS OSA_SIsrDelete( OS_Hisr_t *);
OSA_STATUS OSA_SIsrNotify( UINT32 isrNum );
OSA_STATUS OSA_TranslateErrorCode(UINT32 osErrCode);
void OSA_Run(void);
void OSA_TaskSleep(UINT32 ticks);
void OSA_ContextLock(void);
void OSA_ContextUnlock(void);

/*
** OS System call macros
*/
# 332 "../../osa/inc/osa_linux.h"
/* Thread Control */
# 392 "../../osa/inc/osa_linux.h"
/* Messaging */
# 415 "../../osa/inc/osa_linux.h"
/* Mailboxes */
# 437 "../../osa/inc/osa_linux.h"
/* Events */
# 463 "../../osa/inc/osa_linux.h"
/* Timers */
# 506 "../../osa/inc/osa_linux.h"
/* Semaphores */
# 529 "../../osa/inc/osa_linux.h"
/* Mutexes */
# 548 "../../osa/inc/osa_linux.h"
/* Context lock */




/*#define OSA_ISR_CREATE_FISR(isrNum, func, rtn)                          {                                                               	rtn = OSA_FIsrCreate(isrNum, func);                             }*/
# 573 "../../osa/inc/osa_linux.h"
/*        INTERRUPTS_ON();                                            \*/




/*        INTERRUPTS_OFF();                                           \*/

/* RTC services */
# 46 "../../osa/inc/osa_types.h" 2


/* Use Win32 specific stuff */





/* For Symbian, we include these definitions elsewhere */
# 71 "../../osa/inc/osa_types.h"
/*=======================================================================
*  OS Memory Pool Structure
*======================================================================*/

typedef struct _OS_Mem
{
 union
 {
  OS_MemPool_t vPoolRef; /* Variable pool ref */
  OS_PartitionPool_t fPoolRef; /* Fixed pool ref */
 }u;
 UINT32 type; /* Pool type OSA_FIXED/OSA_VARIABLE */
 void *refCheck;
 UINT8 *poolBase;
 UINT32 poolSize;
 UINT32 partitionSize;
 struct _OS_Mem *next;
 struct _OS_Mem *prev;
} OS_Mem;

typedef struct
{
 UINT32 numCreated; /* # created memory pools */
 OS_Sema_t poolSemaphore; /* pool semaphore */
 OS_Mem memPools[16]; /* Memory pools */
 OS_Mem *head; /* Active list */
 OS_Mem *free; /* Free list */
} OS_MemPool;


/*=======================================================================
 *  OS Task Structures
 *=====================================================================*/

typedef struct _OS_Task
{
 OS_Task_t os_ref;
 void *refCheck;
 struct _OS_Task* next;
 struct _OS_Task* prev;
 UINT8 state;
 char name[8]; /* Service name */
 void *tlsPtr; /* pointer to the thread local storage (TLS) */
 UINT8 padding[3];
} OS_Task;

typedef struct
{
 UINT32 numCreated;
 UINT32 totalNumCreated;
 OS_Sema_t poolSemaphore;

 OS_Task task[32];

 OS_Task *free; /* Free list */
 OS_Task *head; /* Active list */
} OS_TaskPool;


/*=======================================================================
*  Event Flag Structures
*======================================================================*/

typedef struct _OS_Flag
{
 OS_Flag_t os_ref;
 void *refCheck;
 UINT32 eventMask;
 struct _OS_Flag *next;
 struct _OS_Flag *prev;
} OS_Flag;

typedef struct
{
 UINT32 numCreated;
 OS_Sema_t poolSemaphore;
 UINT32 counter;

 OS_Flag eventGroups[10];

 OS_Flag *head; /* Active queue list */
 OS_Flag *free; /* Free queue list */

 OS_Hisr_t os_HISR;
 UINT32 getIndex;
 UINT8 stack[(256)];
 UINT32 count;

} OS_FlagPool;


/*========================================================================
 *  OS Message Structures
 *========================================================================*/

typedef struct _OS_MsgQueue
{
 OS_MsgQ_t os_ref; /* OS queue reference */
 void *refCheck;
 struct _OS_MsgQueue *next;
 struct _OS_MsgQueue *prev;
 UINT32 msgCount;
# 182 "../../osa/inc/osa_types.h"
} OS_MsgQueue;

typedef struct
{
 UINT32 numCreated;
 OS_Sema_t poolSemaphore;

 OS_MsgQueue queues[32];

 OS_MsgQueue *head; /* Active queue list */
 OS_MsgQueue *free; /* Free queue list */
} OS_MsgQPool;


/*=======================================================================
 *  OS Mailbox Structures
 *=====================================================================*/

typedef struct _OS_Mbox
{
 OS_Mbox_t os_ref;
 void *refCheck;
 struct _OS_Mbox *next;
 struct _OS_Mbox *prev;
 UINT32 msgCount;







} OS_Mbox;

typedef struct
{
 UINT32 numCreated; /* # created mailboxes */
 OS_Sema_t poolSemaphore; /* timer pool semaphore */

 OS_Mbox mboxes[1]; /* Mailboxes */

 OS_Mbox *head; /* Active mailbox list */
 OS_Mbox *free; /* Free mailbox list */
} OS_MboxPool;


/*=======================================================================
 *  OS Timer Structures
 *=====================================================================*/

typedef struct _OS_Timer
{
 OS_Timer_t os_ref;
 void *refCheck;
 UINT32 state;
 struct _OS_Timer *next;
 struct _OS_Timer *prev;
} OS_Timer;

typedef struct
{
 UINT32 numCreated; /* # created timers */
 OS_Sema_t poolSemaphore; /* timer pool semaphore */

 OS_Timer timers[10]; /* Timers */

 OS_Timer *head; /* Active timers list */
 OS_Timer *free; /* Free timers list */
} OS_TimerPool;


/*=======================================================================
 *  Semaphore
 *=====================================================================*/

typedef struct _OS_Sema
{
 OS_Sema_t os_ref;
 void *refCheck;
 struct _OS_Sema *next;
 struct _OS_Sema *prev;
} OS_Sema;


typedef struct
{
 UINT32 numCreated;
 UINT32 counter;
 OS_Sema_t poolSemaphore;

 OS_Sema semaphores[16];

 OS_Sema *head;
 OS_Sema *free;
} OS_SemaPool;


/*=======================================================================
 *  Mutex
 *=====================================================================*/

typedef struct _OS_Mutex
{
 OS_Mutex_t os_ref;
 void *refCheck;
 OS_Task_t *owner;
 struct _OS_Mutex *next;
 struct _OS_Mutex *prev;
 UINT8 basePriority; /* Orig priority for priority inheritance */
 UINT8 currentPriority; /* Highest priority of waiting tasks      */
 UINT8 highestWaiting; /* Max priority of tasks waiting          */
 UINT8 numTasks; /* # tasks in mutex Q including owner     */
} OS_Mutex;


typedef struct
{
 UINT32 numCreated;
 UINT32 counter;
 OS_Sema_t poolSemaphore;

 OS_Mutex mutexes[16];

 OS_Mutex *head;
 OS_Mutex *free;
} OS_MutexPool;
# 332 "../../osa/inc/osa_types.h"
/*=======================================================================
*  OS Statistics
*======================================================================*/

typedef struct
{
 UINT32 sysMsgCount; /* System msg counter */
 UINT32 sysMaxMsgAllowed; /* Max msgs in system */
 UINT32 sysTotalMsgReceived; /* Sys tot msg received */
 UINT32 sysMaxMsgs; /* Max # msgs in sys queue */
 UINT32 sysMboxCount;
 UINT32 sysMaxMboxAllowed;
 UINT32 sysTotalMboxReceived;
 UINT32 sysMaxMboxMsgs;
 UINT32 sysMsgQMemRequired;
 OSSemaRef msgQStatsSema4;

} OS_SystemStats;

/*=======================================================================
*  OS Process Control
*======================================================================*/

typedef struct
{
 OS_Proc_t pid;
 UINT32 numAttached;
} OS_ProcId;

typedef struct
{
 UINT32 numProcAttached;
 OS_Sema_t sema;
 OS_ProcId proc[8];
} OS_ProcessCntrl;


/*=======================================================================
*  OS Control Block
*======================================================================*/

typedef struct _OSA_ControlBlock
{
 OS_Sema_t csOSAContextLock; //Tianbo: global context lock for user
 OS_Sema_t csOSAContextLockInternal; // For internal OSA usage
 OS_Sema_t OSA_TickSemaphore;
 UINT32 OSA_RtcCounter;
 OS_Task_t OSATimerTask;
 OS_Task_t OSATickTask;
 void *globalChunk;
 OS_ProcessCntrl attach;

 OS_MsgQPool msgQPool;


 OS_MboxPool mboxPool;


 OS_TaskPool taskPool;


 OS_TimerPool timerPool;


 OS_SemaPool semaPool;


 OS_MutexPool mutexPool;


 OS_FlagPool eventPool;
# 411 "../../osa/inc/osa_types.h"
 OS_MemPool_t mboxPoolMem;


 OS_MemPool_t heapPool;


 OS_MemPool memPool;


} OSA_ControlBlock;


/*=======================================================================
*  Macros
*======================================================================*/

/* Checks reference for NULL and validates with current stored value */
/*   ref   - api reference tested for NULL                           */
/*   osRef - tested for valid stored pointer to internal reference   */
# 442 "../../osa/inc/osa_types.h"
/* Linked List management macros. Structure must have *next & *prev. */

/* Add the item to the front of the list */
# 455 "../../osa/inc/osa_types.h"
/* Delete the item from the active list */
# 466 "../../osa/inc/osa_types.h"
/* Get the first item from the free list */
# 475 "../../osa/inc/osa_types.h"
/* Add to the front of the free list */
# 490 "../../osa/inc/osa_types.h"
/*                      end of osa_types.h
   --------------------------------------------------------------------------- */
# 109 "diag_comm_EXTif_OSA_LINUX.c" 2

# 1 "../../include/pxa_dbg.h" 1
/******************************************************************************

*(C) Copyright 2007 Marvell International Ltd.

* All Rights Reserved

******************************************************************************/
# 111 "diag_comm_EXTif_OSA_LINUX.c" 2


//#define  fprintf( stream, fmt, args ...) DPRINTF(fmt, ## args)
# 126 "diag_comm_EXTif_OSA_LINUX.c"
/* termios defaults */



static UINT32 cacheFlush();
static UINT32 cachedWrite(UINT8 *msg, UINT32 len);

UINT32 SendNoopMsg(); //for UDP support
int SendUDPMsg(UINT8 *msg, UINT32 len); //for UDP support
UINT32 SendCommMsg(UINT8 *msg, UINT32 len);
void diagPreDefCMMInit(void);
void diagCMMRegister(void);


extern int hPort; //file descriptor for the log file or USB or UART used in write()
extern UINT32 diag_count;
extern int diag_server;
extern const UINT32 _FirstCommandID;
extern const UINT32 _LastCommandID;
extern const UINT32 _FirstReportID;
extern const UINT32 _LastReportID;


///////////////////////////////////////////
//Diag over FS (SD card) global variables//
///////////////////////////////////////////
static UINT32 sdl_header_ver;
static UINT32 updateCommDBverOnClose = 0; //Override the log file header on close when having the comm DB ver.
static UINT32 commDBver = 0;
static UINT32 filterCommDBver = 0;
static UINT32 total_log_tx = 0; //number of bytes written to the current log file
static UINT32 log_file_size; //maximum log file size
static UINT32 log_file_num = 0; //current number in the log file name convention
static UINT32 log_file_index = 0; //current index of the log file (for the header)
static UINT32 max_log_file_num; //maximum number of files for the log file name convention
//extern UINT32 log_auto_start;			//automatic start of logging to the log file
static char log_file_path[256]; //log file name convention
char log_file_rel_path[256]; //log file relative path


int diag_extIF_nl = -1; /* file descriptor of netlink */

//UINT32 hUsbPortFunctional=0;	// the USBport is not functional
UINT32 lastUSBevent; // 0-none, 1-USB connect, 2-USB disconnect,10-various-error
UINT32 lastUDPevent; // 0-none, 1-UDP connected, 2-UDP disconnected, 10 -errorr
//DCB m_dcb; //Global
//int m_EXTsock = -1;                             //for UDP support
UINT32 m_bFirstConnected = 0;
//UINT32 udp_state        = 0;                //0-disconnected, 1-connected, 2-trying to connect
UINT32 m_iLostExtTxMessages = 0;
unsigned long m_lAddress;
unsigned long m_lTavorAddress;
unsigned short m_sUDP_Port;
UINT32 bUDPsendICATready = 0;

UINT32 myClientID = 0; //TBDIY updated by the CMI master when a client registers and receives a confirm CMM.

OSSemaRef send_comm_msg_sem = ((void *)0);
OSSemaRef connect_fs_sem = ((void *)0);

UINT32 RegisterCMM[11];//cleared by default
//+------+---+---+----------+---------+-------+---------+-----------+
//|Length|PID|SAP|Service ID|Module ID|Comm ID|Source ID|CMM version|
//|2     |2  |1  |1         |2        |4      |4        |4          |
//+------+---+---+----------+---------+-------+---------+-----------+

void disconnectCOMM(void);
void connectCOMM(void);

extern void diagExtIFstatusConnectNotify(void);
extern void diagExtIFStatusDiscNotify(void);
extern void diagUsbConnectNotify();
extern void diagUsbDisConnectNotify(void);
extern UINT32 diagCommSetChunk(DIAG_COM_RX_Packet_Info *RxPacket, CHAR *msgData, UINT32 length, UINT32 bRemoveHeader);
void dispatchServiceForSAP(UINT8 *Ori_data);
extern UINT32 diag_lock_L(UINT32 *cpsr_ptr, OS_Mutex_t * lock);
extern void diag_unlock_L(UINT32 cpsr_val, OS_Mutex_t * lock);
extern void diagCommL2Init (void);
extern int sdl_signature;
extern GlobalIfData dataExtIf;
extern const DiagDBVersion DBversionID;
extern char *SOCKET_PATH_MASTER;
extern BOOL bDiagConnAcked;
//Cached Write mechanism
typedef struct {
 UINT32 cachePtr;
 OS_Mutex_t diag_cached_write_mutex;
 OSTimerRef diag_cached_write_timer_ref;
 UINT8 *cachedWriteBuff;
} CachedWriteS;
typedef struct{
 UINT32 state; //0-disconnected, 1-connected, 2-trying to connect
 UINT32 cpsr;
 OS_Mutex_t lock;
}UdpState;
extern UdpState udp_state;
extern UdpState tcp_state;
extern UdpState local_state;
extern UdpState cmi_state;
extern Diag_Target diag_target;
extern Diag_Event diag_event;
extern int diag_server_port;
extern const UINT32 _FirstCommandID;
# 270 "diag_comm_EXTif_OSA_LINUX.c"
/***********************************************************************
* Function: diagOverFSfilter                                           *
************************************************************************
* Description:  Used to read the Diag over FS filter file.             *
*                                                                      *
* Parameters:   const char *filter_file - FS filter file name.         *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
void diagOverFSfilter(const char *filter_file)
{
 (void)filter_file;
# 370 "diag_comm_EXTif_OSA_LINUX.c"
}

/***********************************************************************
* Function: diagOverFScfg                                              *
************************************************************************
* Description:  Used to read the Diag over FS configuration file.      *
*                                                                      *
* Parameters:   const char *cfg_file - FS configuration file name.     *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
void diagOverFScfg(const char *cfg_file)
{
 FILE *stream;
 char s[256 + 10];
 char s1[266];
 char s2[266];

 log_file_num = 0; //reset when loading a new cfg file
 strcpy(log_file_path, "/log%03d.sdl");
 if(diagIntData.m_eLLtype == tLL_SC) //SD card, about 1G = 200x5M in total
 {
  log_file_size = (5*1024*1024); //default 5MB per file
  max_log_file_num = 200; //default
 }
 else //Flash, about 16M = 8x2M in total
 {
  log_file_size = (2*1024*1024); //default 2MB per file
  max_log_file_num = 8;
 }
 sdl_header_ver = 1;

 stream = fopen(cfg_file, "r");
 if (stream == ((void *)0))
 {
  fprintf((&__sF[2]), "***** DIAG over FS: %s was not opened. Error: %s. Trying to create a default one. *****\r\n", cfg_file, strerror((*__errno())));
  stream = fopen(cfg_file, "w");
  if (stream == ((void *)0))
  {
   int err = (*__errno());
   MMI_Report(MMI_REP_FOPEN, err);
   fprintf((&__sF[2]), "***** DIAG over FS: The default %s was not opened. Error: %s. Using default values. *****\r\n", cfg_file, strerror(err));
  }
  else
  {
   fprintf(stream, "#Log file path: path=%s (default)\r\n", log_file_path);
   fprintf(stream, "path=%s\r\n", log_file_path);
   fprintf(stream, "#Log file size: size=%d (default)\r\n", log_file_size);
   fprintf(stream, "size=%d\r\n", log_file_size);
   fprintf(stream, "#Max log file num: max=%d (default)\r\n", max_log_file_num);
   fprintf(stream, "max=%d\r\n", max_log_file_num);
   fprintf(stream, "#SDL header version: ver=%d (default)\r\n", sdl_header_ver);
   fprintf(stream, "ver=%d\r\n", sdl_header_ver);
   if (fclose(stream) != 0)
   {
    int err = (*__errno());
    MMI_Report(MMI_REP_FCLOSE, err);
    fprintf((&__sF[2]), "***** DIAG over FS: file %s fclose() failure. Error: %s. *****\r\n", cfg_file, strerror(err));
   }
   fprintf((&__sF[2]), "***** DIAG over FS: The default %s is ready. *****\r\n", cfg_file);
  }
 }
 else
 {
  int i;
  fprintf((&__sF[2]), "***** DIAG over FS: Analyzing the %s file ... *****\r\n", cfg_file);
  i = fscanf(stream, "%[][- =a-zA-Z0-9_.~\\/#%%?+|{}()<>,:;*&^$@!`']\r\n", s);
  while (i != (-1))
  {
   char *tmp;
   int t;

   //fprintf(stderr,"***** DIAG over FS: '%d' '%s' *****\r\n",i,s);
   if ((*s == '#') || (*s == '/') || (*s == '\\'))
   {
    i = fscanf(stream, "%[][- =a-zA-Z0-9_.~\\/#%%?+|{}()<>,:;*&^$@!`']\r\n", s);
    continue; // # and /  and \ are a remark line
   }

   tmp = s;
   while (*tmp != 0 && *tmp != '=')
   {
    *tmp = toupper(*tmp);
    tmp++;
   }
   t = sscanf(s, "%[^=]%*[=]%s", s1, s2);
   //fprintf(stderr,"***** DIAG over FS: '%d' '%s' '%s' *****\r\n",t,s1,s2);
   if (t == 2)
   {
    if (strcmp("PATH", s1) == 0)
    {
     strcpy(log_file_path, s2);
     fprintf((&__sF[2]), "***** DIAG over FS: PATH='%s' *****\r\n", log_file_path);
    }
    else if (strcmp("SIZE", s1) == 0)
    {
     sscanf(s2, "%d", &log_file_size);
     fprintf((&__sF[2]), "***** DIAG over FS: SIZE='%d' *****\r\n", log_file_size);
    }
    else if (strcmp("MAX", s1) == 0)
    {
     sscanf(s2, "%d", &max_log_file_num);
     fprintf((&__sF[2]), "***** DIAG over FS: MAX ='%d' *****\r\n", max_log_file_num);
    }
    else if (strcmp("VER", s1) == 0)
    {
     sscanf(s2, "%d", &sdl_header_ver);
     fprintf((&__sF[2]), "***** DIAG over FS: VER ='%d' *****\r\n", sdl_header_ver);
     if ((sdl_header_ver > 2) || (sdl_header_ver < 1))
     {
      sdl_header_ver = 1;
      fprintf((&__sF[2]), "***** DIAG over FS: Wrong VER! Using the default VER='%d' *****\r\n", sdl_header_ver);
     }
    }
    else
    {
     fprintf((&__sF[2]), "***** DIAG over FS: parser error: '%s' *****\r\n", s);
    }
   }
   else
   {
    fprintf((&__sF[2]), "***** DIAG over FS: parser error: '%s' *****\r\n", s);
   }
   i = fscanf(stream, "%[][- =a-zA-Z0-9_.~\\/#%%?+|{}()<>,:;*&^$@!`']\r\n", s);
  }
  if (fclose(stream) != 0)
  {
   int err = (*__errno());
   MMI_Report(MMI_REP_FCLOSE, err);
   fprintf((&__sF[2]), "***** DIAG over FS: file %s fclose() failure. Error: %s. *****\r\n", cfg_file, strerror(err));
  }
 }
 /* Output data read: */
 fprintf((&__sF[2]), "***** DIAG over FS: log_file_path   ='%s' *****\r\n", log_file_path);
 fprintf((&__sF[2]), "***** DIAG over FS: log_file_size   ='%d' *****\r\n", log_file_size);
 fprintf((&__sF[2]), "***** DIAG over FS: max_log_file_num='%d' *****\r\n", max_log_file_num);
 fprintf((&__sF[2]), "***** DIAG over FS: sdl_header_ver  ='%d' *****\r\n", sdl_header_ver);
}

void connectCMI(void)
{

 eeh_log(5, "*****DIAG enter connectCMI()***\n");
 diag_lock_L(&cmi_state.cpsr,&cmi_state.lock); //mutex on udp_state - maybe checked when USB cable is connected.
 eeh_log(5, "*****DIAG connectCMIState;%d\n", cmi_state.state);
 if(cmi_state.state == 0)
 {
  int rc;
  struct sockaddr_un sin;
  cmi_state.state = 2;
  diag_unlock_L(cmi_state.cpsr,&cmi_state.lock);
  do
  {
   diag_server_port = socket(1, 2, 0);
   if(diag_server_port < 0)
    eeh_log(5, "*****DIAG creat soket error:%s(%d)\n", strerror((*__errno())),(*__errno()));

  }while(diag_server_port < 0);
  memset(&sin, 0, sizeof(sin));
  sin.sun_family = 1;
  strcpy(sin.sun_path, SOCKET_PATH_MASTER);
  eeh_log(5, "*****DIAG set AF_UNIX addr %s\n",SOCKET_PATH_MASTER);

  rc= unlink(SOCKET_PATH_MASTER);
  if (rc!=0)
  {
   eeh_log(5, "***** DIAG CMI (cId = %d) couldn't unlink Socket rc == %d, Err= %d	*****\r\n", _FirstCommandID, rc, (*__errno()));


  }

  do
  {
   rc = bind(diag_server_port, (struct sockaddr *)&sin, sizeof(sin));
   if(rc != -1) //successful
   {
    eeh_log(5, "*****Diag bind unix soket successful\n");
    break;
   }

  }while(1);
  if(rc != 0)
  {
   diag_lock_L(&cmi_state.cpsr, &cmi_state.lock);
   cmi_state.state = 0;
   diag_unlock_L(cmi_state.cpsr, &cmi_state.lock);
  }
  else
  {
   diag_lock_L(&cmi_state.cpsr, &cmi_state.lock);
   cmi_state.state = 1;
   diag_unlock_L(cmi_state.cpsr, &cmi_state.lock);
  }

 }
 else
 {
  UINT32 tmp=cmi_state.state;
  diag_unlock_L(cmi_state.cpsr, &cmi_state.lock);
  eeh_log(5, "*********************************************************************\r\n");
  eeh_log(5, "***** DIAG EXT Already binded/trying to bind !!! cmi_state=%d  *****\r\n",tmp);
  eeh_log(5, "*********************************************************************\r\n");
 }
}
void connectLocal(void)
{
 eeh_log(5, "*****DIAG enter connectLocal()***\n");
 diag_lock_L(&local_state.cpsr,&local_state.lock); //mutex on udp_state - maybe checked when USB cable is connected.
 eeh_log(5, "*****DIAG connectLocalState;%d\n", local_state.state);
 if(local_state.state ==0)
 {
  int rc;
  struct sockaddr_un clntaddr;
  local_state.state = 2;
  diag_unlock_L(local_state.cpsr,&local_state.lock);
  do
  {
   diag_target.hPort = socket(1, 2, 0);
   eeh_log(5, "*****DIAG creat a unix socket \n");

   if(diag_target.hPort < 0)
    eeh_log(5, "*****DIAG creat soket error:%s(%d)\n", strerror((*__errno())),(*__errno()));

  }while(diag_target.hPort < 0);

  memset(&clntaddr, 0, sizeof(clntaddr));
  clntaddr.sun_family = 1;
  sprintf(clntaddr.sun_path, "/tmp/client%d",getpid());
  eeh_log(5, "*****DIAG set AF_UNIX addr\n");

  do
  {
   rc = bind(diag_target.hPort, (struct sockaddr *)&clntaddr, sizeof(clntaddr));
   if(rc != -1) //successful
   {
    eeh_log(5, "*****Diag bind unix soket successful\n");
    break;
   }

  }while(1);
  if(rc != 0)
  {
   diag_lock_L(&local_state.cpsr, &local_state.lock);
   local_state.state = 0;
   diag_unlock_L(local_state.cpsr, &local_state.lock);
  }
  else
  {
   diag_lock_L(&local_state.cpsr, &local_state.lock);
   local_state.state = 1;
   diag_unlock_L(local_state.cpsr, &local_state.lock);

   // we have socket, lets reset the L2 rx- state machine
   diagCommL2Init ();
   // as for L4 - handshake - need to review for USB/UDP maybe replace NOOP
   //diagCommL4Init

   //diagUsbConnectNotify();
   diagPreDefCMMInit();
   diagCMMRegister();

  }
 }
 else
 {
  UINT32 tmp=local_state.state;
  diag_unlock_L(local_state.cpsr, &local_state.lock);
  eeh_log(5, "*********************************************************************\r\n");
  eeh_log(5, "***** DIAG EXT Already binded/trying to bind !!! local_state=%d  *****\r\n",tmp);
  eeh_log(5, "*********************************************************************\r\n");
 }
}
/***********************************************************************
* Function: connectUDP                                                 *
************************************************************************
* Description:  Connect UDP socket                                     *
* Parameters:   none                                                   *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
void connectUDP(void)
{
 eeh_log(5, "*****DIAG enter connectUDP()***\n");
 diag_lock_L(&udp_state.cpsr,&udp_state.lock); //mutex on udp_state - maybe checked when USB cable is connected.
 eeh_log(5, "*****DIAG connectUDP State;%d, bIsClient :%d\n", udp_state.state, diagIntData.m_bIsClient);
 m_sUDP_Port = __extension__({ __uint16_t __swap16gen_x = (12345); (__uint16_t)((__swap16gen_x & 0xff) << 8 | (__swap16gen_x & 0xff00) >> 8); });
 if(udp_state.state ==0) //mutex on udp_state - maybe checked when USB cable is connected.
 {
  int rc;
  struct sockaddr_in sin;
  struct sockaddr_un clntaddr;

  udp_state.state = 2;
  diag_unlock_L(udp_state.cpsr,&udp_state.lock);
  do
  {
   if(diagIntData.m_bIsClient)
   {
    diag_target.hPort = socket(1, 2, 0);
    eeh_log(5, "*****DIAG creat a socket \n");
   }
   else
   {
    diag_target.hPort = socket(2, 2, IPPROTO_UDP);
    eeh_log(5, "*****DIAG creat a socket in IPPROTO_UDP\n");
   }
   if(diag_target.hPort < 0)
   {
    eeh_log(5, "*****DIAG creat soket error:%s(%d)\n", strerror((*__errno())),(*__errno()));
    sleep(1);
   }
  }while(diag_target.hPort < 0);

  if(diagIntData.m_bIsClient)
  {
   memset(&clntaddr, 0, sizeof(clntaddr));
   clntaddr.sun_family = 1;
   sprintf(clntaddr.sun_path, "/tmp/client%d",getpid());
   eeh_log(5, "*****DIAG set AF_UNIX addr\n");
  }
  else
  {
   memset(&sin,0,sizeof(sin));
   sin.sin_family=2;
   //sin.sin_port=0;//Ido G wants to try a fixed port htons(12345) instead of 0
   sin.sin_port = m_sUDP_Port;
   eeh_log(5, "*****DIAG set AF_INET addr :%x\n",m_sUDP_Port);
  }
  do
  {
   int bindeer;
   if(diagIntData.m_bIsClient)
   {
    rc = bind(diag_target.hPort, (struct sockaddr *)&clntaddr, sizeof(clntaddr));
   }
   else
   {
    sin.sin_addr.s_addr = m_lTavorAddress;
    rc = bind(diag_target.hPort,(const struct sockaddr*)&sin,sizeof(sin));
   }
   eeh_log(5, "***** DIAG EXT rc == %d Last Error == %s(%d) *****\r\n",rc,strerror((*__errno())),(*__errno()));
   if(rc != -1) //successful
   {
    eeh_log(5, "*****Diag bind soket successful\n");
    break;
   }
   bindeer = (*__errno());
   if (bindeer == 99 && diagIntData.m_bIsClient==0)
   {
    //close(diag_target.hPort);
    //diag_target.hPort = -1;
    //diagExtIFstatusConnectNotify();
    //break;
   }
   sleep(1);
  }while(1);
  if(rc != 0)
  {
   diag_lock_L(&udp_state.cpsr, &udp_state.lock);
   udp_state.state = 0;
   diag_unlock_L(udp_state.cpsr, &udp_state.lock);
  }
  else
  {
   diag_lock_L(&udp_state.cpsr, &udp_state.lock);
   udp_state.state = 1;
   diag_unlock_L(udp_state.cpsr, &udp_state.lock);

   // we have socket, lets reset the L2 rx- state machine
   diagCommL2Init ();
   // as for L4 - handshake - need to review for USB/UDP maybe replace NOOP
   //diagCommL4Init

   diagUsbConnectNotify();
  }
 }
 else
 {
  UINT32 tmp=udp_state.state;
  diag_unlock_L(udp_state.cpsr, &udp_state.lock);
  eeh_log(5, "*********************************************************************\r\n");
  eeh_log(5, "***** DIAG EXT Already binded/trying to bind !!! udp_state=%d  *****\r\n",tmp);
  eeh_log(5, "*********************************************************************\r\n");
 }
}

void connectTCP()
{
 struct sockaddr_in serv_addr;
 struct sockaddr_in peer_addr;
 int ret = 0;
 int addr_size = 0;
 int addr_reuse = 1;
 eeh_log(5, "*****DIAG enter connectTCP()***\n");

 m_sUDP_Port = __extension__({ __uint16_t __swap16gen_x = (12345); (__uint16_t)((__swap16gen_x & 0xff) << 8 | (__swap16gen_x & 0xff00) >> 8); });
 diag_lock_L(&tcp_state.cpsr,&tcp_state.lock);

 if(tcp_state.state == 1)
 {
  eeh_log(5, "*****DIAG already connect TCP\n");
  diag_unlock_L(tcp_state.cpsr, &tcp_state.lock);
  return ;
 }
 do
 {
  diag_target.socket = socket(2, 1, 0);
  eeh_log(5, "*****DIAG creat a socket in TCP\n");
  if(diag_target.socket < 0)
  {
   eeh_log(5, "*****DIAG over TCP creat soket error:%s(%d)\n", strerror((*__errno())),(*__errno()));
   sleep(1);
  }
 }while(diag_target.socket < 0);
 if (!diagIntData.m_bIsClient)
 {
  setsockopt(diag_target.socket, 1, 2, &addr_reuse, sizeof(addr_reuse));
  memset(&serv_addr,0,sizeof(serv_addr));
  serv_addr.sin_family = 2;
  serv_addr.sin_port = m_sUDP_Port;
  serv_addr.sin_addr.s_addr = m_lTavorAddress;
  do
  {
   ret = bind(diag_target.socket,(const struct sockaddr*)&serv_addr,sizeof(serv_addr));
   if(ret == -1)
    eeh_log(5, "******DIAG over TCP bind socket error:%s\n",strerror((*__errno())));
  }while(ret < 0);
  eeh_log(5, "*******DIAG over TCP bind socket successful!\n");

 } else
 {
  memset(&serv_addr,0,sizeof(serv_addr));
  serv_addr.sin_family = 2;
  serv_addr.sin_port = m_sUDP_Port;
  serv_addr.sin_addr.s_addr = m_lAddress;
  //DIAGPRINT("***** DIAG SendUDPMsg to port %x, length:%d *****\n",m_sUDP_Port, len);
 }
 if(!diagIntData.m_bIsClient)
 {
  do
  {
   ret = listen(diag_target.socket, 0);
   if(ret == -1)
    eeh_log(5, "******DIAG over TCP listen socket error:%s\n",strerror((*__errno())));
  }while(ret <0);
  eeh_log(5, "*****DIAG over TCP listen socket successful!\n");
 }
 else
 {
  do
  {
   ret = connect(diag_target.socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
   eeh_log(5, "*****DIAG connect to the server:%d\n", ret);
   if(diag_target.socket == -1)
   {
    eeh_log(5, "*****DIAG socket is closed\n");
    diag_unlock_L(tcp_state.cpsr, &tcp_state.lock);
    return;
   }
   sleep(1);
  }while(ret < 0);
 }

 if(!diagIntData.m_bIsClient)
 {
  do
  {
   addr_size = sizeof(peer_addr);
   ret = accept(diag_target.socket, (struct sockaddr *)&peer_addr, (socklen_t*)&addr_size);
   if(ret == -1)
    eeh_log(5, "******DIAG over TCP accept socket error:%s\n",strerror((*__errno())));

   if(diag_target.socket == -1)
   {
    eeh_log(5, "*****DIAG socket is closed\n");
    if(ret > 0) close(ret);
    diag_unlock_L(tcp_state.cpsr, &tcp_state.lock);
    return;
   }

  }while(ret <0);
  eeh_log(5, "*****DIAG over TCP connected with:****\nport=%d\naddr=%d.%d.%d.%d\n*******************************\n", peer_addr.sin_port, (peer_addr.sin_addr.s_addr&0x000000FF), (peer_addr.sin_addr.s_addr&0x0000FF00)>>8, (peer_addr.sin_addr.s_addr&0x00FF0000)>>16, (peer_addr.sin_addr.s_addr&0xFF000000)>>24);






  diag_target.hPort = ret;

 }
 else
 {
  diag_target.hPort = diag_target.socket;
 }
 tcp_state.state =1;
 diag_unlock_L(tcp_state.cpsr, &tcp_state.lock);
 eeh_log(5, "*****DIAG over TCP Notice Rcv Task:%d!\n",diag_event.pipefd[1]);
 write(diag_event.pipefd[1], "wake up",10);
 //struct timeval timeout = {1,0};
 //setsockopt(diag_target.hPort, SOL_SOCKET, SO_SNDTIMEO,(char *)&timeout,sizeof(struct timeval));
 //diagExtIFstatusConnectNotify();
 diagCommL2Init ();
 diagUsbConnectNotify();
 return;
}
/***********************************************************************
* Function: connectFS                                                  *
************************************************************************
* Description:  Connect File System (SD card) interface                *
* Parameters:   none                                                   *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
void connectFS()
{
 UINT32 hf; //header field
 char filename[256];
 char format[256];
 int fd;
 int err;
 static CHAR usb_comm_ver_id[] = { 0x10, 0x00, 0x00, 0x00, 0x00, 0, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
 //static CHAR usb_apps_ver_id[] = { 0x10, 0x00, 0x00, 0x00, 0x80, DB_VERSION_SERVICE, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
 //If  O_CREAT  and  O_EXCL  are set, open() shall fail if the file exists.
 OSASemaphoreAcquire(connect_fs_sem, 0xFFFFFFFF);
 if (log_file_num >= max_log_file_num) log_file_num = 0;
 sprintf(format, "%s%s", log_file_rel_path, log_file_path);
 sprintf(filename, format, log_file_num);
 eeh_log(5, "***** DIAG over FS: open the log file:%s\n", filename);

 fd = hPort;
 if (diagIntData.diagCachedWrite.write_unit_size)
 {
  cacheFlush();
 }
 diag_count = 0;
 //hPort = open(filename, O_WRONLY|O_CREAT|O_TRUNC|O_SYNC|O_DSYNC, 0x666 /*S_IRWXU|S_IRWXG|S_IRWXO*/);
 hPort = open(filename, 00000001 | 00000100 | 00001000, 0x666); //improve performance
 if (hPort == -1)
 {
  err = (*__errno());
  /*---------------------------------*/
  /* Create non-existent directories */
  /*---------------------------------*/
  if ((err == 2) || (err == 20))
  {
   struct stat buf;
   char *src = filename;
   char *dst = format;
   while (*src != 0)
   {
    if ((*src == '/') || (*src == '\\'))
    {
     *dst = 0;
     if (*format != 0)
     {
      if (stat(format, &buf) < 0)
      {
       MMI_Report(MMI_REP_STAT, (*__errno()));
       fprintf((&__sF[2]), "***** DIAG over FS: non-existent directory for the log file: '%s' *****\r\n", format);
       if (mkdir(format, 0x666) < 0)
       {
        MMI_Report(MMI_REP_MKDIR, (*__errno()));
        fprintf((&__sF[2]), "***** DIAG over FS: failed to create this directory - reason %s. *****\r\n", strerror(err));
       }
      }
      else
      {
       fprintf((&__sF[2]), "***** DIAG over FS: '%s' exist *****\r\n", format);
      }
     }
    }
    *dst = *src;
    dst++;
    src++;
   }
   //retry to open now!
   //hPort = open(filename, O_WRONLY|O_CREAT|O_TRUNC|O_SYNC|O_DSYNC, 0x666 /*S_IRWXU|S_IRWXG|S_IRWXO*/);
   hPort = open(filename, 00000001 | 00000100 | 00001000, 0x666); //improve performance
   err = (*__errno());
  }
  /*- - - - - - - - - - - - - - - - -*/
 }
 if (hPort == -1)
 {
  MMI_Report(MMI_REP_OPEN, err);
  fprintf((&__sF[2]), "***** DIAG over FS: Failed to initialize FS connection - reason %s. *****\r\n", strerror(err));
  if (m_bFirstConnected)
  {
   m_bFirstConnected = 0;
  }
  else
  {
   //DIAG_ASSERT(0);
  }
 }

 if (fd != -1) // We are already connected
 {
/*		fprintf(stderr,"***** DIAG over FS: Request connect, but hPort not INVALID *****\r\n");
		fprintf(stderr,"***** DIAG over FS: Close (before hPort==%d) *****\r\n",fd);
 #if defined ( _POSIX_SYNCHRONIZED_IO)
		fprintf(stderr,"***** DIAG over FS: _POSIX_SYNCHRONIZED_IO is defined *****\r\n");
 #else
		fprintf(stderr,"***** DIAG over FS: _POSIX_SYNCHRONIZED_IO is NOT defined *****\r\n");
 #endif*/

  /* degrades performabce
		   sync();
		   if (fsync(fd)==-1)
		   {
			int err=errno;
			MMI_Report(MMI_REP_FSYNC,err);
			fprintf(stderr,"***** DIAG over FS fsync error: %s *****\r\n",strerror(err));
		   }
		 */
  if (updateCommDBverOnClose)
  {
   updateCommDBverOnClose = 0;
   if (lseek(fd, ((sdl_header_ver == 1) ? 3 : 4) * 4, 0) == -1)
   {
    int err = (*__errno());
    MMI_Report(MMI_REP_LSEEK, err);
    fprintf((&__sF[2]), "***** DIAG over FS: connectFS: lseek: error: %s *****\r\n", strerror(err));
   }
   if (write(fd, &commDBver, 4) == -1)
   {
    int err = (*__errno());
    MMI_Report(MMI_REP_WRITE, err);
    fprintf((&__sF[2]), "***** DIAG over FS: connectFS: commDBver: write: error: %s *****\r\n", strerror(err));
   }
   fprintf((&__sF[2]), "***** DIAG over FS: connectFS: updating the log file header Comm DB ver 0x%x *****\r\n", commDBver);
  }
  if (close(fd) == -1)
  {
   int err = (*__errno());
   MMI_Report(MMI_REP_CLOSE, err);
   fprintf((&__sF[2]), "***** DIAG over FS: Close error: %s *****\r\n", strerror(err));
  }
 }
 if (hPort == -1) return;

 //fprintf(stderr,"***** DIAG over FS: FS connection Initialized. %s pid=%d hPort=%d *****\r\n",filename,getpid(),hPort);

 //Create the FS log header
 //------------------------

 //Header Ver
 total_log_tx += 4;
 if (diagIntData.diagCachedWrite.write_unit_size)
 {
  cachedWrite((UINT8 *)&sdl_header_ver, 4);
 }
 else
 {
  if (write(hPort, &sdl_header_ver, 4) == -1)
  {
   int err = (*__errno());
   MMI_Report(MMI_REP_WRITE, err);
   fprintf((&__sF[2]), "***** DIAG over FS: Header Ver: write: error: %s *****\r\n", strerror(err));
  }
 }

 if (sdl_header_ver > 1)
 {
  //Header Length
  hf = 9 * 4;
  total_log_tx += 4;
  if (diagIntData.diagCachedWrite.write_unit_size)
  {
   cachedWrite((UINT8 *)&hf, 4);
  }
  else
  {
   if (write(hPort, &hf, 4) == -1)
   {
    int err = (*__errno());
    MMI_Report(MMI_REP_WRITE, err);
    fprintf((&__sF[2]), "***** DIAG over FS: Header Length: write: error: %s *****\r\n", strerror(err));
   }
  }
 }

 //Data Format: 1-USB, 2-UART(L2/L4), 3 - TBDIY New Header
 if (diagIntData.m_eConnectionType == tUSBConnection)
 {
  hf = 1;
 }
 else
 {
  hf = 2;
 }
 total_log_tx += 4;
 if (diagIntData.diagCachedWrite.write_unit_size)
 {
  cachedWrite((UINT8 *)&hf, 4);
 }
 else
 {
  if (write(hPort, &hf, 4) == -1)
  {
   int err = (*__errno());
   MMI_Report(MMI_REP_WRITE, err);
   fprintf((&__sF[2]), "***** DIAG over FS: Header Data Format: write: error: %s *****\r\n", strerror(err));
  }
 }

 //APPS Ver
 if (sscanf(DBversionID, "0x%lx", (long unsigned int *)&hf) != 1)
 {
  hf = 0;
 }
 total_log_tx += 4;
 if (diagIntData.diagCachedWrite.write_unit_size)
 {
  cachedWrite((UINT8 *)&hf, 4);
 }
 else
 {
  if (write(hPort, &hf, 4) == -1)
  {
   int err = (*__errno());
   MMI_Report(MMI_REP_WRITE, err);
   fprintf((&__sF[2]), "***** DIAG over FS: Header APPS Ver: write: error: %s *****\r\n", strerror(err));
  }
 }

 //COMM Ver
 total_log_tx += 4;
 if (diagIntData.diagCachedWrite.write_unit_size)
 {
  cachedWrite((UINT8 *)&commDBver, 4);
 }
 else
 {
  if (write(hPort, &commDBver, 4) == -1)
  {
   int err = (*__errno());
   MMI_Report(MMI_REP_WRITE, err);
   fprintf((&__sF[2]), "***** DIAG over FS: Header COMM Ver: write: error: %s *****\r\n", strerror(err));
  }
 }
 if (!commDBver)
 {
  updateCommDBverOnClose = 1;
 }

 //Sequence
 log_file_num++;
 hf = log_file_index++;
 total_log_tx += 4;

 if (diagIntData.diagCachedWrite.write_unit_size)
 {
  cachedWrite((UINT8 *)&hf, 4);
 }
 else
 {
  if (write(hPort, &hf, 4) == -1)
  {
   int err = (*__errno());
   MMI_Report(MMI_REP_WRITE, err);
   fprintf((&__sF[2]), "***** DIAG over FS: Header Sequence: write: error: %s *****\r\n", strerror(err));
  }
 }

 //Date/Time
 hf = time(((void *)0));
 total_log_tx += 4;
 if (diagIntData.diagCachedWrite.write_unit_size)
 {
  cachedWrite((UINT8 *)&hf, 4);
 }
 else
 {
  if (write(hPort, &hf, 4) == -1)
  {
   int err = (*__errno());
   MMI_Report(MMI_REP_WRITE, err);
   fprintf((&__sF[2]), "***** DIAG over FS: Header Date/Time: write: error: %s *****\r\n", strerror(err));
  }
 }

 //Checksum
 hf = 0;
 total_log_tx += 4;
 if (diagIntData.diagCachedWrite.write_unit_size)
 {
  cachedWrite((UINT8 *)&hf, 4);
 }
 else
 {
  if (write(hPort, &hf, 4) == -1)
  {
   int err = (*__errno());
   MMI_Report(MMI_REP_WRITE, err);
   fprintf((&__sF[2]), "***** DIAG over FS: Header Checksum: write: error: %s *****\r\n", strerror(err));
  }
 }

 //Signature
 if (sdl_header_ver > 1)
 {
  total_log_tx += 4;
  if (diagIntData.diagCachedWrite.write_unit_size)
  {
   cachedWrite((UINT8 *)&sdl_signature, 4);
  }
  else
  {
   if (write(hPort, &sdl_signature, 4) == -1)
   {
    int err = (*__errno());
    MMI_Report(MMI_REP_WRITE, err);
    fprintf((&__sF[2]), "***** DIAG over FS: Signature: write: error: %s *****\r\n", strerror(err));
   }
  }
 }

 ///////////////////////////
 // Request DB version ID //
 ///////////////////////////
 if ((diagIntData.m_bWorkMultiCore) && (!commDBver))
 {
  int wait_timeout = 5;
  eeh_log(5, "******DIAG over FS:requeset DB version ID !!!\n");
  while((!bDiagConnAcked)&&(wait_timeout))
  {
   sleep(1);
   wait_timeout--;
  }
  diagCommSetChunk(dataExtIf.RxPacket, usb_comm_ver_id, 16, 1); //Linux runs on the apps side and we need the comm ver id in the log file
  dispatchServiceForSAP((UINT8 *)dataExtIf.RxPacket->buffer);
 }
 OSASemaphoreRelease(connect_fs_sem);
}

/***********************************************************************
* Function: connectCOMM                                                *
************************************************************************
* Description:  Connect USB/UART interface                             *
* Parameters:   none                                                   *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
void connectCOMM()
{
 struct termios tio;
 memset(&tio, 0x0, sizeof(tio));
 //OSA_STATUS status;
# 1332 "diag_comm_EXTif_OSA_LINUX.c"
 hPort = open("/dev/ttyS0", 00000002);
 //hPort = open(DIAG_TTY_UART_DEVICE,O_RDWR | O_NOCTTY | O_NDELAY);

 //fcntl(hPort, F_SETFL, 0 /*FASYNC*/);

 /* Configure termios */
 tio.c_cflag = 0010002 | 0000060 | 0004000 | 0000200;
 tio.c_iflag = 0;
 //tio.c_iflag = IGNPAR;
 tio.c_oflag = 0;
 tio.c_lflag = 0;
 tio.c_cc[6] = 1;
 tio.c_cc[5]= 0;

 tcflush(hPort, 2); //flush both data received but not read and data written but not transmitted.
 tcsetattr(hPort, 0x5402, &tio); //the change shall occur immediately



 // we have socket, lets reset the L2 rx- state machine
# 1360 "diag_comm_EXTif_OSA_LINUX.c"
}


/***********************************************************************
* Function: disconnectFS                                               *
************************************************************************
* Description:  Disconnect File System interface                       *
* Parameters:   none                                                   *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
void disconnectFS()
{
 if (hPort != -1)
 {
  fprintf((&__sF[2]), "***** DIAG over FS: Close (before hPort==%d) *****\r\n", hPort);

  /* degrades performabce
		   sync();
		   if (fsync(hPort)==-1)
		   {
			int err=errno;
			MMI_Report(MMI_REP_FSYNC,err);
			fprintf(stderr,"***** DIAG over FS fsync error: %s *****\r\n",strerror(err));
		   }
		 */

  if (updateCommDBverOnClose)
  {
   updateCommDBverOnClose = 0;
   if (diagIntData.diagCachedWrite.write_unit_size)
   {
    cacheFlush();
   }
   if (lseek(hPort, ((sdl_header_ver == 1) ? 3 : 4)*4, 0) == -1)
   {
    int err = (*__errno());
    MMI_Report(MMI_REP_LSEEK, err);
    fprintf((&__sF[2]), "***** DIAG over FS: disconnectFS: lseek: error: %s *****\r\n", strerror(err));
   }
   if (write(hPort, &commDBver, 4) == -1)
   {
    int err = (*__errno());
    MMI_Report(MMI_REP_WRITE, err);
    fprintf((&__sF[2]), "***** DIAG over FS: disconnectFS: commDBver: write: error: %s *****\r\n", strerror(err));
   }
   fprintf((&__sF[2]), "***** DIAG over FS: disconnectFS: updating the log file header Comm DB ver 0x%x *****\r\n", commDBver);
  }
  else
  {
   if (diagIntData.diagCachedWrite.write_unit_size)
   {
    cacheFlush();
   }
  }
  if (close(hPort) == -1)
  {
   int err = (*__errno());
   MMI_Report(MMI_REP_CLOSE, err);
   fprintf((&__sF[2]), "***** DIAG over FS: Close error: %s *****\r\n", strerror(err));
  }
  hPort = -1;
 }

 //Reset the log file size counter for the next connectFS()
 total_log_tx = 0;

 ///////////////////////////////////////////////////////////////
 //Before we can remove the SD card,                          //
 //ensure that everything in memory is written to the SD card.//
 sync();
 sleep(2); //wait 2 more secs - just to be on the safe side ;-)
 MMI_Report(MMI_REP_STOP_DONE, 0);
 ///////////////////////////////////////////////////////////////
 fprintf((&__sF[2]), "***** DIAG over FS: Close() *****\r\n");
}

/***********************************************************************
* Function: disconnectNet                                               *
************************************************************************
* Description:  Disconnect Net interface                       *
* Parameters:   none                                                   *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
void disconnectNet()
{
 int fd = 0;
 int ret = -1;
 if(diag_target.hPort != -1)
 {
  fd = diag_target.hPort;
  eeh_log(5, "Closing stream:%d...\n", fd);
  diag_target.hPort = -1;
  diag_lock_L(&tcp_state.cpsr,&tcp_state.lock);
  tcp_state.state = 0;
  diag_unlock_L(tcp_state.cpsr, &tcp_state.lock);
  close(fd);
  eeh_log(5, "Closed stream!\n");
 }
 if(diag_target.socket != -1)
 {
  fd = diag_target.socket;
  eeh_log(5, "Closing socket:%d...\n", fd);
  diag_target.socket = -1;
  if(diagIntData.m_eConnectionType == tTCPConnection)
   ret = shutdown(fd, SHUT_RDWR);
  ret =close(fd);
  eeh_log(5, "Closed socket:%d!\n",ret);
 }
}
/***********************************************************************
* Function: disconnectCOMM                                             *
************************************************************************
* Description:  Disconnect USB/UART if                                 *
* Parameters:   none                                                   *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
void disconnectCOMM()
{
 int fd;
 diagUsbDisConnectNotify();
 eeh_log(5, ">>>diagDisconnect , diagUsbDisConnectNotify\n");
 //Diag over FS (SD card) support
 if (diagIntData.m_eLLtype == tLL_FS || diagIntData.m_eLLtype == tLL_SC)
 {
  return disconnectFS();
 }

 //hUsbPortFunctional=0;
 if (hPort != -1)
 {
  fd = hPort;
  if (diagIntData.diagCachedWrite.write_unit_size)
  {
   cacheFlush();
  }
  hPort = -1;
  fprintf((&__sF[2]), "***** DIAG COMM Close(hPort==%d) *****\r\n", fd);
  close(fd); //Close the Handle from the Create File (Close the port)
 }
 disconnectNet();
 fprintf((&__sF[2]), "***** DIAG COMM Close() *****\r\n");
}
# 1576 "diag_comm_EXTif_OSA_LINUX.c"
void mmiStartCBfunc(const char* rel_path)
{
 eeh_log(5, ">> Enter mmiStartCBfunc\n");
 strcpy(log_file_rel_path, rel_path);
 eeh_log(5, "***** DIAG over FS: MMI START path='%s' *****\r\n", log_file_rel_path);
 fprintf((&__sF[2]), "***** DIAG over FS: MMI START path='%s' *****\r\n", log_file_rel_path);
 diagExtIFstatusConnectNotify();
}

//#define CACHED_WRITE_DBG
CachedWriteS cw; //global is zero by default

OSA_STATUS InitDiagCachedWrite(void)
{
 OSA_STATUS status = OS_SUCCESS;

 if (diagIntData.diagCachedWrite.write_unit_size)
 {
  fprintf((&__sF[2]), "***** DIAG Diag Cached Write is Enabled *****\r\n");
  status = OSATimerCreate(&cw.diag_cached_write_timer_ref);
  do { if (!(status == OS_SUCCESS)) { char buffer[512]; sprintf(buffer, "ASSERT FAIL AT FILE %s FUNC %s LINE %d", "diag_comm_EXTif_OSA_LINUX.c", __FUNCTION__, 1596); eeh_draw_panic_text(buffer, strlen(buffer), 0); } } while (0);
  cw.cachedWriteBuff = malloc(2 * diagIntData.diagCachedWrite.write_unit_size); //Not free
  do { if (!(cw.cachedWriteBuff != 0)) { char buffer[512]; sprintf(buffer, "ASSERT FAIL AT FILE %s FUNC %s LINE %d", "diag_comm_EXTif_OSA_LINUX.c", __FUNCTION__, 1598); eeh_draw_panic_text(buffer, strlen(buffer), 0); } } while (0);
  status = OSA_MutexCreate(&cw.diag_cached_write_mutex, ((void *)0));
  do { if (!(status == OS_SUCCESS)) { char buffer[512]; sprintf(buffer, "ASSERT FAIL AT FILE %s FUNC %s LINE %d", "diag_comm_EXTif_OSA_LINUX.c", __FUNCTION__, 1600); eeh_draw_panic_text(buffer, strlen(buffer), 0); } } while (0);
 }

 return status;
}


BOOL InitLog2SDcard(void)
{
 BOOL retVal = 0;
 OSA_STATUS status;

 if (diagIntData.m_eLLtype == tLL_FS || diagIntData.m_eLLtype == tLL_SC)
 {
  status = InitDiagCachedWrite();

  if (status != OS_SUCCESS)
  {
   fprintf((&__sF[2]), "*****InitDiagCachedWrite eror ... *****\r\n");
   return 0;
  }
  if (MMI_Init(mmiStartCBfunc, diagExtIFStatusDiscNotify, diagIntData.m_eLLtype, &diagIntData.diagBSPFScfg))
  {
   fprintf((&__sF[2]), "***** DIAG over FS: MMI_Init returned successfully!!! *****\r\n");
  }
  else
  {
   fprintf((&__sF[2]), "***** DIAG over FS: MMI_Init failed!!! *****\r\n");
  }

  if (diagIntData.diagBSPFScfg.log_auto_start)
  {
   fprintf((&__sF[2]), "***** DIAG over FS: AUTO START ... *****\r\n");
   MMI_Report(MMI_REP_AUTO, 0);
  }

  retVal = 1;
 }

 return retVal;
}

/***********************************************************************
* Function: DiagCachedWriteTimerCallback                               *
************************************************************************
* Description: Notifying diag multi tx task                            *
*                                                                      *
* Parameters:                                                          *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
void DiagCachedWriteTimerCallback(UINT32 param)
{
 (void)param;
 if (cw.cachePtr)
 {
  cacheFlush(); //protected within if (diagIntData.diagCachedWrite.write_unit_size)
 }
 else
 {



 }
}
/***********************************************************************
* Function: cacheFlush				                                   *
************************************************************************
* Description:                                                         *
*                                                                      *
* Parameters:                                                          *
*                                                                      *
* Return value:                                                        *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
static UINT32 cacheFlush()
{
 ssize_t ret;

 OSA_MutexLock(&cw.diag_cached_write_mutex, 0xFFFFFFFF);



 if (cw.cachePtr > 0)
 {
  if (tLL_ETHERNET == diagIntData.m_eLLtype || tLL_LocalIP == diagIntData.m_eLLtype)
  {
   if(diag_target.send_target != ((void *)0))
    ret = diag_target.send_target(cw.cachedWriteBuff, cw.cachePtr);
  }
  else
  {
   ret = write(hPort, cw.cachedWriteBuff, cw.cachePtr);
  }
  if((ret < 0) || ((UINT32)ret != cw.cachePtr))
  {
   eeh_log(5, "***** DIAG COMM TX cachedWrite loss!. Error: %s *****\r\n", strerror((*__errno())));
  }
  cw.cachePtr = 0; //no retry
  if (ret == -1)
  {
   int err = (*__errno());
   if (diagIntData.m_eLLtype == tLL_FS || diagIntData.m_eLLtype == tLL_SC)
   {
    MMI_Report(MMI_REP_WRITE, err);
   }
   fprintf((&__sF[2]), "***** DIAG COMM TX cachedWrite failed. Error: %s *****\r\n", strerror(err));
   OSA_MutexUnlock(&cw.diag_cached_write_mutex);
   return 0;
  }
 }
 OSA_MutexUnlock(&cw.diag_cached_write_mutex);
 return 1;
}
/***********************************************************************
* Function: cachedWrite				                                   *
************************************************************************
* Description:                                                         *
*                                                                      *
* Parameters:                                                          *
*                                                                      *
* Return value:                                                        *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
static UINT32 cachedWrite(UINT8 *msg, UINT32 len)
{
 ssize_t ret;

 /*
	   1. cachePtr+len>2*write_unit_size => Flush the buffer. cachePtr=0.
	   2. len>2*write_unit_size => If cachePtr>0 {Flush the buffer. cachePtr=0.} Write the new msg with no caching.
	   3. cachePtr+len>=write_unit_size => Cache the new msg and flush the buffer. cachePtr=0.
	   4. else => Cache the new msg. cachePtr+=len.
	 */

 OSA_MutexLock(&cw.diag_cached_write_mutex, 0xFFFFFFFF);
 //AGPRINT( "***** DIAG COMM TX cachedWrite cachePtr=%d len=%d *****\r\n", cw.cachePtr, len);
 /*1*/
 if ((cw.cachePtr + len > 2 * diagIntData.diagCachedWrite.write_unit_size) && (cw.cachePtr))
 {




  if (tLL_ETHERNET == diagIntData.m_eLLtype || tLL_LocalIP == diagIntData.m_eLLtype)
  {
   if(diag_target.send_target != ((void *)0))
    ret = diag_target.send_target(cw.cachedWriteBuff, cw.cachePtr);
  }
  else
  {
   ret = write(hPort, cw.cachedWriteBuff, cw.cachePtr);
  }
  if((ret < 0) || ((UINT32)ret != cw.cachePtr))
  {
   eeh_log(5, "***** DIAG COMM TX cachedWrite loss!. Error: %s *****\r\n", strerror((*__errno())));
  }
  cw.cachePtr = 0; //no retry
  if (ret == -1)
  {
   int err = (*__errno());
   if (diagIntData.m_eLLtype == tLL_FS || diagIntData.m_eLLtype == tLL_SC)
   {
    MMI_Report(MMI_REP_WRITE, err);
   }
   eeh_log(5, "***** DIAG COMM TX cachedWrite failed. Error: %s *****\r\n", strerror(err));
   OSA_MutexUnlock(&cw.diag_cached_write_mutex);
   return 0;
  }
 }
 /*2*/
 if (len > 2 * diagIntData.diagCachedWrite.write_unit_size)
 {




  if (tLL_ETHERNET == diagIntData.m_eLLtype || tLL_LocalIP == diagIntData.m_eLLtype)
  {




   if(diag_target.send_target != ((void *)0))
    ret = diag_target.send_target(msg, len);
  }
  else
  {
   ret = write(hPort, msg, len);
  }
  if((ret < 0) || ((UINT32)ret != len))
  {
   eeh_log(5, "***** DIAG COMM TX cachedWrite loss!. Error: %s *****\r\n", strerror((*__errno())));
  }
  if (ret == -1)
  {
   int err = (*__errno());
   if (diagIntData.m_eLLtype == tLL_FS || diagIntData.m_eLLtype == tLL_SC)
   {
    MMI_Report(MMI_REP_WRITE, err);
   }
   fprintf((&__sF[2]), "***** DIAG COMM TX cachedWrite failed. Error: %s *****\r\n", strerror(err));
   OSA_MutexUnlock(&cw.diag_cached_write_mutex);
   return 0;
  }
  else
  {
   OSA_MutexUnlock(&cw.diag_cached_write_mutex);
   return 1;
  }
 }
 /*3+4*/
 /*In the next lines of code we use cw.cachePtr+len and not increasing yet (cw.cachePtr+=len;) for the following 'if (!cw.cachePtr)' logic. */
 memcpy(cw.cachedWriteBuff + cw.cachePtr, msg, (size_t)len);
 if (cw.cachePtr + len > diagIntData.diagCachedWrite.write_unit_size)
 {



  if (tLL_ETHERNET == diagIntData.m_eLLtype || tLL_LocalIP == diagIntData.m_eLLtype)
  {
   if(diag_target.send_target != ((void *)0))
    ret = diag_target.send_target(cw.cachedWriteBuff, cw.cachePtr + len);
  }
  else
  {
   ret = write(hPort, cw.cachedWriteBuff, cw.cachePtr + len);
  }
  if((ret < 0) || ((UINT32)ret != (cw.cachePtr+ len)))
  {
   eeh_log(5, "***** DIAG COMM TX cachedWrite loss!. Error: %s *****\r\n", strerror((*__errno())));
  }
  cw.cachePtr = 0; //no retry
  if (ret == -1)
  {
   int err = (*__errno());
   if (diagIntData.m_eLLtype == tLL_FS || diagIntData.m_eLLtype == tLL_SC)
   {
    MMI_Report(MMI_REP_WRITE, err);
   }
   fprintf((&__sF[2]), "***** DIAG COMM TX cachedWrite failed. Error: %s *****\r\n", strerror(err));
   OSA_MutexUnlock(&cw.diag_cached_write_mutex);
   return 0;
  }
 }
 else
 {
  /*In the previous lines of code we used cw.cachePtr+len and didn't increas (cw.cachePtr+=len;) for the following 'if (!cw.cachePtr)' logic. */
  if (!cw.cachePtr)
  {
   OSA_STATUS status;
   status = OSATimerStart(cw.diag_cached_write_timer_ref, diagIntData.diagCachedWrite.time_out, 0, DiagCachedWriteTimerCallback, 0);
   do { if (!(status == OS_SUCCESS)) { char buffer[512]; sprintf(buffer, "ASSERT FAIL AT FILE %s FUNC %s LINE %d", "diag_comm_EXTif_OSA_LINUX.c", __FUNCTION__, 1856); eeh_draw_panic_text(buffer, strlen(buffer), 0); } } while (0);
  }
  cw.cachePtr += len;
 }
 OSA_MutexUnlock(&cw.diag_cached_write_mutex);
 return 1;
}

/***********************************************************************
* Function: SendCommMsg				                                   *
************************************************************************
* Description: Sends a message over UART interface (for L2 handshake)  *
*                                                                      *
* Parameters:                                                          *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
UINT32 SendCommMsg(UINT8 *msg, UINT32 len)
{
 /*UINT32 dwNumBytesWritten;*/
 ssize_t ret;

 /*ret = WriteFile (hPort,						// Port handle
	   _diagExtIfReportsList[pos].virtualReportPtr,	// Pointer to the data to write
	   _diagExtIfReportsList[pos].reportLength,		// Number of bytes to write
	   &dwNumBytesWritten,						// Pointer to the number of bytes written
	   NULL									// Must be NULL for Windows CE
	   );*/
 OSASemaphoreAcquire(send_comm_msg_sem, 0xFFFFFFFF);
 if(diag_server ==0)
 {
  ret = diag_target.send_target(msg, len);
  OSASemaphoreRelease(send_comm_msg_sem);
  return ret;
 }
 if (diagIntData.m_eLLtype == tLL_FS || diagIntData.m_eLLtype == tLL_SC)
 {

  if (total_log_tx + len > log_file_size)
  {
   total_log_tx = len;
   connectFS();
  }
  else
  {
   total_log_tx += len;
   //fprintf(stderr,"***** DIAG over FS: total_log_tx=%d pid=%d. *****\r\n",total_log_tx,getpid());
  }
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //// Parse the Comm DB version:                       ////
  //// --------------------------                       ////
  //// This is a critical path, but msg[4]==0x01 has    ////
  //// a minor cost. Most of the times this condition   ////
  //// fails and the other &&s are not checked at all.  ////
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
  //[4:5] SAP , [8:9] Module ID , [10:11] Msg ID
  //BUG: sometimes msg[9]!=0xff. We found a case where it was zero.
  //if (msg[4]==0x01 && msg[5]==0x00 && msg[8]==0xff && msg[9]==0xff && msg[10]==0x00 && msg[11]==0x00)
  if (msg[4] == 0x01 && msg[5] == 0x00 && msg[8] == 0xff && msg[10] == 0x00 && msg[11] == 0x00)
  {
   if (sscanf((char *)msg + 16, "0x%lx", (long unsigned int *)&commDBver) != 1)
   {
    commDBver = 0;
   }
   eeh_log(5, "*****DIAG over FS**** CB versiong read from msg:0x%lx\n",commDBver);
   //fprintf(stderr, "***** DIAG over FS: Comm DB VER: %s *****\r\n", msg+16);
   if (filterCommDBver != 0)
   {
    if (commDBver == filterCommDBver)
    {
     fprintf((&__sF[2]), "***** DIAG over FS: diagOverFSfilter COMM DB VERSION MATCH=0x%x !!! *****\r\n", filterCommDBver);
     MMI_Report(MMI_REP_COMM_FILTER_DB_VER_MATCH, 0);
    }
    else
    {
     fprintf((&__sF[2]), "***** DIAG over FS: diagOverFSfilter COMM DB VERSION DOES NOT MATCH img=0x%x filter=0x%x !!! *****\r\n", commDBver, filterCommDBver);
     MMI_Report(MMI_REP_COMM_FILTER_DB_VER_MISMATCH, 0);
    }
   }
  }
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////
 }
 if (diagIntData.m_eConnectionType == tUSBConnection)
 {
//#define DEBUG_USB_MSG_LEN
# 1958 "diag_comm_EXTif_OSA_LINUX.c"
  //Make sure that the message is 4 Bytes aligned to prevent red messages in ACAT
  /*if ((len & 0x3) != 0)
		{
			len = (len | 0x3) + 1;
			msg[0] = len & 0xff;
			msg[1] = (len & 0xff00) >> 8;
		}*/
 }

 // Add len for UART RAWDIAG head

 if (diagIntData.m_eConnectionType == tUARTConnection)
 {
  //Make sure that the message is 4 Bytes aligned to prevent red messages in ACAT
 /*	if ((len & 0x3) != 0)
		{
			len = (len | 0x3) + 1;
			msg[0] = len & 0xff;
			msg[1] = (len & 0xff00) >> 8;
		} */
 }


 if (diagIntData.diagCachedWrite.write_unit_size)
 {
  ret = cachedWrite(msg, len);
 }
 else
 {
  ret = write(hPort, msg, len);
  if((ret < 0) || ((UINT32)ret != len))
   eeh_log(5, "*****DIAG COMM TX Error: ret(%d) <> len(%d)\n", ret, len);
  if (ret == -1 /*0*/)
  {
   int err = (*__errno());
   if (diagIntData.m_eLLtype == tLL_FS || diagIntData.m_eLLtype == tLL_SC)
   {
    MMI_Report(MMI_REP_WRITE, err);
   }
   //Handle this error UINT32 le=GetLastError();
   fprintf((&__sF[2]), "***** DIAG COMM TX WriteFile failed. *****\r\n");
   fprintf((&__sF[2]), "Error: %s\n", strerror(err));
   ret = 0;
  }
  else
  {
   ret = 1;
  }
 }
 OSASemaphoreRelease(send_comm_msg_sem);

 return ret;
}
int SendCMIMsg(UINT8 *msg, UINT32 len, const void *clienaddr)
{

 struct sockaddr_un *clntaddr = (struct sockaddr_un *)clienaddr;
 int ret = 0;
 UINT32 retry_cnt = 20;
 int iSocketLastError;
 if(diag_server_port == -1)
 {
  eeh_log(5, "*****DIAG SendUDPMsg socket = -1!***\n");
  return -1;
 }

 while(retry_cnt--)
 {
  if(cmi_state.state != 1)
  {
   eeh_log(5, "*****DIAG SendlocalMsg while binding... *****\r\n");
   return -1;
  }
  ret = sendto(diag_server_port, (CHAR *)msg, len, 0, (const struct sockaddr*)clntaddr, (socklen_t)sizeof(struct sockaddr_un));

  if (ret < 0)
  {
   iSocketLastError = (*__errno());
   if (iSocketLastError == 11 || iSocketLastError == 11 /*WSAEWOULDBLOCK*/)
   {
    eeh_log(5, "************************************************\r\n");
    eeh_log(5, "***** DIAG SendUDPMsg WSAEWOULDBLOCK (%d) *****\r\n",retry_cnt);
    eeh_log(5, "************************************************\r\n");
    eeh_log(5, "Error: %s\n", strerror(iSocketLastError));
    usleep(200);
    continue;
   }
   m_iLostExtTxMessages++;
   eeh_log(5, "**********************************************************************\r\n");
   eeh_log(5, "***** DIAG SendUDPMsg failed iSocketLastError = %d, LostMsg#=%d *****\r\n",iSocketLastError,m_iLostExtTxMessages);
   eeh_log(5, "***** DIAG NOT WSAEWOULDBLOCK...                                *****\r\n");
   eeh_log(5, "**********************************************************************\r\n");
   eeh_log(5, "Error: %s\n", strerror(iSocketLastError));
   return -1;
  }
  else if (ret != (int)len)
  {
   eeh_log(5, "***** DIAG SendUDPMsg error in length *****\r\n");
   return ret;
  }
  else
  {
   if (20-retry_cnt>1)
   {
    eeh_log(5, "*********************************************\r\n");
    eeh_log(5, "***** DIAG SendUDPMsg OK after %02d tries *****\r\n",20-retry_cnt);
    eeh_log(5, "*********************************************\r\n");
   }
   return ret;
  }
 }
 return 0;
}
int SendLocalMsg(UINT8 *msg, UINT32 len)
{
 struct sockaddr_in sin;
 struct sockaddr_un clntaddr;
 int ret = 0;
 UINT32 retry_cnt = 20;
 int iSocketLastError;
 //DIAGPRINT("******SendLocalMsg!len :%d\n",len);
 if(diag_target.hPort == -1)
 {
  eeh_log(5, "*****DIAG SendUDPMsg socket = -1!***\n");
  return -1;
 }
 if (diagIntData.m_bIsClient)
 {
  memset(&clntaddr, 0, sizeof(clntaddr));
  clntaddr.sun_family = 1;
  strcpy(clntaddr.sun_path, SOCKET_PATH_MASTER);
 } else
 {
  memset(&sin,0,sizeof(sin));
  sin.sin_family = 2;
  sin.sin_port = m_sUDP_Port;
  //DIAGPRINT("***** DIAG SendUDPMsg to port %x, length:%d *****\n",m_sUDP_Port, len);
 }
 while(retry_cnt--)
 {
  if(local_state.state != 1)
  {
   eeh_log(5, "*****DIAG SendlocalMsg while binding... *****\r\n");
   return -1;
  }

  if (diagIntData.m_bIsClient)
  {
   ret = sendto(diag_target.hPort, (CHAR *)msg, len, 0, (const struct sockaddr*)&clntaddr, (socklen_t)sizeof(clntaddr));
  } else
  {
   sin.sin_addr.s_addr = m_lAddress;
   ret = sendto(diag_target.hPort, (CHAR *)msg, len, 0, (const struct sockaddr*)/*(LPSOCKADDR)*/&sin, (socklen_t)sizeof(sin));
   //DIAGPRINT("*****DIAG Send UDP :%d****\n",ret);
  }
  if (ret < 0)
  {
   iSocketLastError = (*__errno());
   if (iSocketLastError == 11 || iSocketLastError == 11 /*WSAEWOULDBLOCK*/)
   {
    eeh_log(5, "************************************************\r\n");
    eeh_log(5, "***** DIAG SendUDPMsg WSAEWOULDBLOCK (%d) *****\r\n",retry_cnt);
    eeh_log(5, "************************************************\r\n");
    eeh_log(5, "Error: %s\n", strerror(iSocketLastError));
    usleep(200);
    continue;
   }
   m_iLostExtTxMessages++;
   eeh_log(5, "**********************************************************************\r\n");
   eeh_log(5, "***** DIAG SendUDPMsg failed iSocketLastError = %d, LostMsg#=%d *****\r\n",iSocketLastError,m_iLostExtTxMessages);
   eeh_log(5, "***** DIAG NOT WSAEWOULDBLOCK...                                *****\r\n");
   eeh_log(5, "**********************************************************************\r\n");
   eeh_log(5, "Error: %s\n", strerror(iSocketLastError));
   return -1;
  }
  else if (ret != (int)len)
  {
   eeh_log(5, "***** DIAG SendUDPMsg error in length *****\r\n");
   return ret;
  }
  else
  {
   if (20-retry_cnt>1)
   {
    eeh_log(5, "*********************************************\r\n");
    eeh_log(5, "***** DIAG SendUDPMsg OK after %02d tries *****\r\n",20-retry_cnt);
    eeh_log(5, "*********************************************\r\n");
   }
   return ret;
  }
 }
 return 0;
}
int SendUDPMsg(UINT8 *msg, UINT32 len)
{
 struct sockaddr_in sin;
 struct sockaddr_un clntaddr;
 int ret = 0;
 UINT32 retry_cnt = 20;
 int iSocketLastError;
 if(diag_target.hPort == -1)
 {
  eeh_log(5, "*****DIAG SendUDPMsg socket = -1!***\n");
  return -1;
 }
 if (diagIntData.m_bIsClient)
 {
  memset(&clntaddr, 0, sizeof(clntaddr));
  clntaddr.sun_family = 1;
  strcpy(clntaddr.sun_path, SOCKET_PATH_MASTER);
 } else
 {
  memset(&sin,0,sizeof(sin));
  sin.sin_family = 2;
  sin.sin_port = m_sUDP_Port;
  //DIAGPRINT("***** DIAG SendUDPMsg to port %x, length:%d *****\n",m_sUDP_Port, len);
 }
 while(retry_cnt--)
 {
  if(udp_state.state != 1)
  {
   eeh_log(5, "*****DIAG SendUDPMsg while binding... *****\r\n");
   return -1;
  }

  if (diagIntData.m_bIsClient)
  {
   ret = sendto(diag_target.hPort, (CHAR *)msg, len, 0, (const struct sockaddr*)&clntaddr, (socklen_t)sizeof(clntaddr));
  } else
  {
   sin.sin_addr.s_addr = m_lAddress;
   ret = sendto(diag_target.hPort, (CHAR *)msg, len, 0, (const struct sockaddr*)/*(LPSOCKADDR)*/&sin, (socklen_t)sizeof(sin));
   //DIAGPRINT("*****DIAG Send UDP :%d****\n",ret);
  }
  if (ret < 0)
  {
   iSocketLastError = (*__errno());
   if (iSocketLastError == 11 || iSocketLastError == 11 /*WSAEWOULDBLOCK*/)
   {
    eeh_log(5, "************************************************\r\n");
    eeh_log(5, "***** DIAG SendUDPMsg WSAEWOULDBLOCK (%d) *****\r\n",retry_cnt);
    eeh_log(5, "************************************************\r\n");
    eeh_log(5, "Error: %s\n", strerror(iSocketLastError));
    usleep(200);
    continue;
   }
   m_iLostExtTxMessages++;
   eeh_log(5, "**********************************************************************\r\n");
   eeh_log(5, "***** DIAG SendUDPMsg failed iSocketLastError = %d, LostMsg#=%d *****\r\n",iSocketLastError,m_iLostExtTxMessages);
   eeh_log(5, "***** DIAG NOT WSAEWOULDBLOCK...                                *****\r\n");
   eeh_log(5, "**********************************************************************\r\n");
   eeh_log(5, "Error: %s\n", strerror(iSocketLastError));
   return -1;
  }
  else if (ret != (int)len)
  {
   eeh_log(5, "***** DIAG SendUDPMsg error in length *****\r\n");
   return ret;
  }
  else
  {
   if (20-retry_cnt>1)
   {
    eeh_log(5, "*********************************************\r\n");
    eeh_log(5, "***** DIAG SendUDPMsg OK after %02d tries *****\r\n",20-retry_cnt);
    eeh_log(5, "*********************************************\r\n");
   }
   return ret;
  }
 }
 return 0;
}

int SendTCPMsg(UINT8 *msg, UINT32 len)
{
 int ret = 0;
 UINT32 retry_cnt = 20;
 int iSocketLastError;
 if(diag_target.hPort == -1)
 {
  eeh_log(5, "*****DIAG SendTCPMsg socket = -1!***\n");
  return -1;
 }
 while(retry_cnt--)
 {
  if(tcp_state.state != 1)
  {
   eeh_log(5, "*****DIAG SendTCPMsg while connecting... *****\r\n");
   return -1;
  }
  if (diagIntData.m_bIsClient)
  {
   ret = send(diag_target.hPort, (CHAR *)msg, len, 0);
   //TBD
  } else
  {
   ret = send(diag_target.hPort, (CHAR *)msg, len, 0);
   //DIAGPRINT("*****DIAG Send TCP :%d****\n",ret);
  }
  if (ret < 0)
  {
   iSocketLastError = (*__errno());
   if (iSocketLastError == 11 || iSocketLastError == 11 /*WSAEWOULDBLOCK*/)
   {
    eeh_log(5, "************************************************\r\n");
    eeh_log(5, "***** DIAG SendUDPMsg WSAEWOULDBLOCK (%d) *****\r\n",retry_cnt);
    eeh_log(5, "************************************************\r\n");
    eeh_log(5, "Error: %s\n", strerror(iSocketLastError));
    usleep(200);
    continue;
   }
   m_iLostExtTxMessages++;
   eeh_log(5, "**********************************************************************\r\n");
   eeh_log(5, "***** DIAG SendUDPMsg failed iSocketLastError = %d, LostMsg#=%d *****\r\n",iSocketLastError,m_iLostExtTxMessages);
   eeh_log(5, "***** DIAG NOT WSAEWOULDBLOCK...                                *****\r\n");
   eeh_log(5, "**********************************************************************\r\n");
   eeh_log(5, "Error: %s\n", strerror(iSocketLastError));
   return -1;
  }
  else if (ret != (int)len)
  {
   eeh_log(5, "***** DIAG SendTCPPMsg error in length :%d / %d*****\n",ret ,len);
   return ret;
  }
  else
  {
   if (20-retry_cnt>1)
   {
    eeh_log(5, "*********************************************\r\n");
    eeh_log(5, "***** DIAG SendUDPMsg OK after %02d tries *****\r\n",20-retry_cnt);
    eeh_log(5, "*********************************************\r\n");
   }
   return ret;
  }
 }
 if(ret<0)
 {
  diag_lock_L(&tcp_state.cpsr,&tcp_state.lock);
  tcp_state.state = 0;
  diagExtIFStatusDiscNotify();
  diag_unlock_L(tcp_state.cpsr, &tcp_state.lock);

 }
 return ret;
}
/***********************************************************************
* Function: diag_os_TransmitToExtIfMulti                               *
************************************************************************
* Description: Perform the specific interface logic to tx a trace      *
*              on the external interface                               *
*                                                                      *
* Parameters:  number of buffers to transmit, position in cyclic queue *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
UINT32 diag_os_TransmitToExtIfMulti(UINT16 nOfBuffers, UINT16 pos)
{
 (void)nOfBuffers;
 (void)pos;
 // LINUX , WINCE
 do { if (!(0)) { char buffer[512]; sprintf(buffer, "ASSERT FAIL AT FILE %s FUNC %s LINE %d", "diag_comm_EXTif_OSA_LINUX.c", __FUNCTION__, 2320); eeh_draw_panic_text(buffer, strlen(buffer), 0); } } while (0); // NOT SUPPORTED
 return 0;
}


// ADD ICAT COMMANDS - to control if we initiate ICAT-ready, DBversion on UDB bind

//ICAT EXPORTED FUNCTION - Diag, Debug, udpInternalICATreadyon
void udpInternalICATreadyon(void)
{
 bUDPsendICATready = 1;
 DIAGM_TRACE(Diag, Debug, UDPicatReadyOn_1, "Send ICATready (internal) when binded");

}

//ICAT EXPORTED FUNCTION - Diag, Debug, udpInternalICATreadyoff
void udpInternalICATreadyoff(void)
{
 bUDPsendICATready = 0;
 DIAGM_TRACE(Diag, Debug, UDPicatReadyOff_2, "DO NOT SEND ICATready (internal on bind) ");
}
void diagPreDefCMMInit(void)
{
 *( (UINT16 *)(RegisterCMM+0) )=44;//Length
 *( (UINT8 *)(RegisterCMM+1) )=9 | (0? 0x80 : 0) ;//SAP
 *(((UINT8 *)(RegisterCMM+1))+1 )=48;//Service ID
 *( RegisterCMM+4 )=1;//CMM version
}
void diagCMMRegister(void)
{
 RegisterCMM[5]=_FirstCommandID;//UINT32 firstCommandID
 RegisterCMM[6]=_LastCommandID;//UINT32 lastCommandID
 RegisterCMM[7]=_FirstReportID;//UINT32 firstReportID
 RegisterCMM[8]=_LastReportID;//UINT32 lastReportID
 RegisterCMM[9]=0; //TBDIY ;//UINT32 procID
 RegisterCMM[10]=0;//TBDIY GetThreadPriority(HANDLE hThread);//UINT32 procPriority
 while(SendLocalMsg((UINT8 *)&RegisterCMM,sizeof(RegisterCMM))== -1)
 {
  eeh_log(5, "************diag register to master %d\n", sizeof(RegisterCMM));
  usleep(100*1000);
 }
 eeh_log(5, "************diag register to master done!\n");
}
