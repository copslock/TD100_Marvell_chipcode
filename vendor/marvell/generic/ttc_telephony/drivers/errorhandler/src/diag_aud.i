# 1 "diag_aud.c"
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/diag/diag_al//"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "diag_aud.c"
# 1 "../../linux_telephony/atcmdsrv/inc/MarvellAmixer.h" 1



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
# 5 "../../linux_telephony/atcmdsrv/inc/MarvellAmixer.h" 2
# 1 "../../linux_audio/audio_protocol.h" 1
/*
 *
 *  Audio server for tavor
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */




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
# 24 "../../linux_audio/audio_protocol.h" 2

typedef struct {
 int client_type;
 int method_type;
} SOCKETDATA;



typedef struct {
 int client_type;
 int method_type;
} AUDIOIPCHEADER;



// Different types of client plugin for the daemon
# 53 "../../linux_audio/audio_protocol.h"
// Different methods
# 71 "../../linux_audio/audio_protocol.h"
/*low 2 bytes are used for volume control, below are high 2 bytes. When using them, we will shift them to left 16 bits, like "(INPUT << 16)"*/


/*enable voice call playback & capture*/


/*voice call playback & capture to near end/far end, not define means both end*/



/*with EC path enabled*/

/* with sample rate switch enabled */

/* with calibration state switch enabled */


/*loopback test*/

/*test mode, in this mode audio server will drop all the msg from Android, atcmdsrv will control the audio server*/

/*FIXME, notice that in ctl_phone.c, *imax = INT_MAX;*/
/*It means the flags we can use is up to bit 31, the highest 1 bit cannot be used.*/
/*Since in ctl phone plug-in, the int is signed, so we cannot use bit 31 (highest bit) as the bit flag */
# 112 "../../linux_audio/audio_protocol.h"
typedef struct {
 int device_type;
 int method_id;
 int param;
} __attribute__ ((packed)) AUDIOIPCDATA;



typedef struct {
 int volume;
 int status;
} __attribute__ ((packed)) AUDIOCONTROLINFOS;





typedef struct {
 unsigned int format;
 unsigned short rate;
 unsigned char channels;
} __attribute__ ((packed)) AUDIOSTREAMINFOS;





// parameters used to describe device state
typedef struct {
 int volume_phone;
 int status_phone;
 int volume_voice;
 int status_voice;
} __attribute__ ((packed)) AUDIOMIXERDATA;




/* Phone Status */
# 159 "../../linux_audio/audio_protocol.h"
/* Voice Status */
# 6 "../../linux_telephony/atcmdsrv/inc/MarvellAmixer.h" 2
# 15 "../../linux_telephony/atcmdsrv/inc/MarvellAmixer.h"
//--- List of Voice Command OpCodes:
# 45 "../../linux_telephony/atcmdsrv/inc/MarvellAmixer.h"
static const struct
{
 int client;
 int methodType;
 int methodId;
 int device;
} gItemCtlInfo[(2 + 72 + 1)] =
{
 /*dummy*/
 { 0, 0, 0, 0 },
 { 0, 0, 0, 0 },

 /*speaker control*/
 { 0x00000001, 0x00000001, 0x00000001, 0x00000002 },
 { 0x00000001, 0x00000001, 0x00000002, 0x00000002 },
 { 0x00000001, 0x00000001, 0x00000004, 0x00000002 },
 { 0x00000001, 0x00000001, 0x00000003, 0x00000002 },
 { 0x00000003, 0x00000001, 0x00000001, 0x00000002 },
 { 0x00000003, 0x00000001, 0x00000002, 0x00000002 },
 { 0x00000003, 0x00000001, 0x00000004, 0x00000002 },
 { 0x00000003, 0x00000001, 0x00000003, 0x00000002 },
 { 0x00000004, 0x00000001, 0x00000001, 0x00000002 },
 { 0x00000004, 0x00000001, 0x00000002, 0x00000002 },
 { 0x00000004, 0x00000001, 0x00000004, 0x00000002 },
 { 0x00000004, 0x00000001, 0x00000003, 0x00000002 },

 /*earpiece control*/
 { 0x00000001, 0x00000001, 0x00000001, 0x00000001 },
 { 0x00000001, 0x00000001, 0x00000002, 0x00000001 },
 { 0x00000001, 0x00000001, 0x00000004, 0x00000001 },
 { 0x00000001, 0x00000001, 0x00000003, 0x00000001 },
 { 0x00000003, 0x00000001, 0x00000001, 0x00000001 },
 { 0x00000003, 0x00000001, 0x00000002, 0x00000001 },
 { 0x00000003, 0x00000001, 0x00000004, 0x00000001 },
 { 0x00000003, 0x00000001, 0x00000003, 0x00000001 },
 { 0x00000004, 0x00000001, 0x00000001, 0x00000001 },
 { 0x00000004, 0x00000001, 0x00000002, 0x00000001 },
 { 0x00000004, 0x00000001, 0x00000004, 0x00000001 },
 { 0x00000004, 0x00000001, 0x00000003, 0x00000001 },

 /*headset control*/
 { 0x00000001, 0x00000001, 0x00000001, 0x00000004 },
 { 0x00000001, 0x00000001, 0x00000002, 0x00000004 },
 { 0x00000001, 0x00000001, 0x00000004, 0x00000004 },
 { 0x00000001, 0x00000001, 0x00000003, 0x00000004 },
 { 0x00000003, 0x00000001, 0x00000001, 0x00000004 },
 { 0x00000003, 0x00000001, 0x00000002, 0x00000004 },
 { 0x00000003, 0x00000001, 0x00000004, 0x00000004 },
 { 0x00000003, 0x00000001, 0x00000003, 0x00000004 },
 { 0x00000004, 0x00000001, 0x00000001, 0x00000004 },
 { 0x00000004, 0x00000001, 0x00000002, 0x00000004 },
 { 0x00000004, 0x00000001, 0x00000004, 0x00000004 },
 { 0x00000004, 0x00000001, 0x00000003, 0x00000004 },

 /*bluetooth control*/
 { 0x00000001, 0x00000001, 0x00000001, 0x00000003 },
 { 0x00000001, 0x00000001, 0x00000002, 0x00000003 },
 { 0x00000001, 0x00000001, 0x00000004, 0x00000003 },
 { 0x00000001, 0x00000001, 0x00000003, 0x00000003 },
 { 0x00000003, 0x00000001, 0x00000001, 0x00000003 },
 { 0x00000003, 0x00000001, 0x00000002, 0x00000003 },
 { 0x00000003, 0x00000001, 0x00000004, 0x00000003 },
 { 0x00000003, 0x00000001, 0x00000003, 0x00000003 },
 { 0x00000004, 0x00000001, 0x00000001, 0x00000003 },
 { 0x00000004, 0x00000001, 0x00000002, 0x00000003 },
 { 0x00000004, 0x00000001, 0x00000004, 0x00000003 },
 { 0x00000004, 0x00000001, 0x00000003, 0x00000003 },

 /*headphone control*/
 { 0x00000001, 0x00000001, 0x00000001, 0x00000008 },
 { 0x00000001, 0x00000001, 0x00000002, 0x00000008 },
 { 0x00000001, 0x00000001, 0x00000004, 0x00000008 },
 { 0x00000001, 0x00000001, 0x00000003, 0x00000008 },
 { 0x00000003, 0x00000001, 0x00000001, 0x00000008 },
 { 0x00000003, 0x00000001, 0x00000002, 0x00000008 },
 { 0x00000003, 0x00000001, 0x00000004, 0x00000008 },
 { 0x00000003, 0x00000001, 0x00000003, 0x00000008 },
 { 0x00000004, 0x00000001, 0x00000001, 0x00000008 },
 { 0x00000004, 0x00000001, 0x00000002, 0x00000008 },
 { 0x00000004, 0x00000001, 0x00000004, 0x00000008 },
 { 0x00000004, 0x00000001, 0x00000003, 0x00000003 },

 /* for switching sample rate, using 8k in VT case, using 44.1k in HIFI case*/
 {0x00000001, 0x00000001, 0, 0},

 //wrapper enable path as diag command for loopback
 { 0x00000003, 0x00000001, 0, 0},

 //wrapper enable path as diag command for loopback
 { 0x00000003, 0x00000001, 0x00000005, 0},

 //wrapper disable path as diag command for loopback
 { 0x00000003, 0x00000001, 0x00000006, 0},

 //BT NREC
 { 0x00000003, 0x00000001, 0x00000001, 0x00000009 },
 { 0x00000003, 0x00000001, 0x00000002, 0x00000009 },
 { 0x00000003, 0x00000001, 0x00000004, 0x00000009 },
 { 0x00000003, 0x00000001, 0x00000003, 0x00000009 },

 //HIFI Force speaker
 { 0x00000001, 0x00000001, 0x00000001, 0x0000000a },
 { 0x00000001, 0x00000001, 0x00000002, 0x0000000a },
 { 0x00000001, 0x00000001, 0x00000004, 0x0000000a },
 { 0x00000001, 0x00000001, 0x00000003, 0x0000000a },

 /* dummy for UI calibration */
 {0, 0, 0, 0},
};


/*******************************************************************************
* Function: audio_route_control
*******************************************************************************
* Description: Audio route control
*              Through ALSA control interface
*              Write item to ALSA phone control plug-in
*              Ctl plug-in send IPCs to audio server
*
* Parameters: int client (HIFI_CLIENT  VOICE_CLIENT  PHONE_CLIENT  FMRADIO_CLINET)
*             int method_type (PLUGIN_CTL_WRITE  PLUGIN_CTL_READ  PLUGIN_PCM_WRITE  PLUGIN_PCM_READ)
*             int method_id (METHOD_ENABLE  METHOD_DISABLE  METHOD_MUTE  METHOD_VOLUME_SET)
*             int device (EARPIECE_DEVICE  SPEAKER_DEVICE  BLUETOOTH_DEVICE  HEADSET_DEVICE)
*             int value (INPUT | OUTPUT | Volume)
*
* Return value: int (0: Success, -1: Fail)
*
*******************************************************************************/
int audio_route_control(int client, int method_type, int method_id, int device, int value);

/*calibration APIs*/
int UICal_send_MSA_setting(unsigned short *pData);
int UICal_send_MSA_request(unsigned short *pData);
/*loopback test*/
void start_loopback();
void stop_loopback();
# 2 "diag_aud.c" 2
# 1 "../../aud_sw/acm/inc/acmtypes.h" 1
/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code (?Material? are owned
   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
   transmitted, distributed, or disclosed in any way without Intel?s prior express written permission.

   No license under any patent, copyright, trade secret or other intellectual property right is granted to or
   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
   Intel in writing.
   -------------------------------------------------------------------------------------------------------------------*/

/******************************************************************************
*               MODULE IMPLEMENTATION FILE
*******************************************************************************
* Title: Audio Component Manager (ACM)
*
* Filename: acmTypes.h
*
* Authors: Isar Ariel
*
* Description: ACM typedefs
*
* Last Updated: Jun 11 2009 By Paul Shen
*
* Notes:
******************************************************************************/





typedef enum
{
 AD6521_COMPONENT = 0,
 BGV_COMPONENT,
 MANDY_COMPONENT,
 CCR_COMPONENT,
 ARAVA_COMPONENT,
 BLUETOOTH_COMPONENT,
 DAI_COMPONENT,
 MICCO_COMPONENT,
 GPO_COMPONENT,
 LEVANTE_COMPONENT,
 NULL_COMPONENT, //must be last one

 ACM_COMPONENTS_ENUM_32_BIT = 0x7FFFFFFF //32bit enum compiling enforcement
} ACM_Component;

typedef unsigned char ACM_AudioVolume; /* (0 - 100%) */
typedef signed char ACM_DigitalGain;
typedef signed char ACM_AnalogGain;





typedef enum
{
 ACM_MUTE_OFF = 0,
 ACM_MUTE_ON = 1,

 ACM_AUDIO_MUTE_ENUM_32_BIT = 0x7FFFFFFF //32bit enum compiling enforcement
} ACM_AudioMute;


typedef struct
{
 unsigned short shift;
 unsigned short SSCR0_HIGH;
 unsigned short SSCR0_LOW;
 unsigned short SSCR1_HIGH;
 unsigned short SSCR1_LOW;
 unsigned short SSTSA_LOW;
 unsigned short SSRSA_LOW;
 unsigned short SSPSP_HIGH;
 unsigned short SSPSP_LOW;

//Tavor only:
 unsigned short SSACD_LOW;
 unsigned short SSACDD_HIGH;
 unsigned short SSACDD_LOW;

//SSP Port
 unsigned short devicePort; /* (GSSP0_PORT=0, GSSP1_PORT=1) */
 unsigned short SSP_Rate; /* (0=8 kHz, 1=16 kHz)    */
 unsigned short SSP_Mode; /* (0=unpacked, 1=packed) */
} ACM_SspAudioConfiguration;

typedef enum
{
 ACM_CALIBRATION_OFF = 0,
 ACM_CALIBRATION_ON = 1,

 ACM_AUDIO_CALIBRATION_STATUS_ENUM_32_BIT = 0x7FFFFFFF //32bit enum compiling enforcement
} ACM_AudioCalibrationStatus;

typedef void (*ACMComponentInit_t)(unsigned char reinit, ACM_SspAudioConfiguration **componentSspConfig, signed short **ditherGenConfig);
typedef ACM_DigitalGain (*ACMEnablePath_t)(unsigned char path, ACM_AudioVolume volume);
typedef ACM_DigitalGain (*ACMDisablePath_t)(unsigned char path);
typedef ACM_DigitalGain (*ACMVolumeSetPath_t)(unsigned char path, ACM_AudioVolume volume);
typedef ACM_DigitalGain (*ACMMutePath_t)(unsigned char path, ACM_AudioMute mute);
typedef ACM_DigitalGain (*ACMSetPathAnalogGain_t)(unsigned char regIndex, unsigned char regValue, unsigned char regMask, unsigned char regShift);
typedef short (*ACMGetPathsStatus_t) (char* data, short length);
typedef ACM_AnalogGain (*ACMGetPathAnalogGain_t) (unsigned char path);
typedef unsigned int (*ACMGetActivePathNum_t)(void);
typedef unsigned int (*ACMSetLoopbackTestStatus_t)(unsigned char isloopbacktest);
typedef unsigned int (*ACMGetLoopbackTestStatus_t)(void);

typedef struct
{
 ACMComponentInit_t ACMComponentInit;
 ACMEnablePath_t ACMEnablePath;
 ACMDisablePath_t ACMDisablePath;
 ACMVolumeSetPath_t ACMVolumeSetPath;
 ACMMutePath_t ACMMutePath;
 ACMGetPathsStatus_t ACMGetPathsStatus;
 ACMGetPathAnalogGain_t ACMGetPathAnalogGain;
 ACMSetPathAnalogGain_t ACMSetPathAnalogGain;
 ACMGetActivePathNum_t ACMGetActivePathNum;
 ACMSetLoopbackTestStatus_t ACMSetLoopbackTestStatus;
 ACMGetLoopbackTestStatus_t ACMGetLoopbackTestStatus;
} ACM_ComponentHandle;
# 3 "diag_aud.c" 2
# 1 "../../aud_sw/acm/inc/acm.h" 1
/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code (?Material? are owned
   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
   transmitted, distributed, or disclosed in any way without Intel?s prior express written permission.

   No license under any patent, copyright, trade secret or other intellectual property right is granted to or
   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
   Intel in writing.
   -------------------------------------------------------------------------------------------------------------------*/

/******************************************************************************
*               MODULE IMPLEMENTATION FILE
*******************************************************************************
* Title: Audio Component Manager (ACM)
*
* Filename: ACM.c
*
* Authors: Isar Ariel
*
* Description: Header file for ACM.
*
* Last Updated : Jun 11 2009 By Paul Shen
*
* Notes:
******************************************************************************/


# 1 "../../aud_sw/acm/inc/acmtypes.h" 1
/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code (?Material? are owned
   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
   transmitted, distributed, or disclosed in any way without Intel?s prior express written permission.

   No license under any patent, copyright, trade secret or other intellectual property right is granted to or
   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
   Intel in writing.
   -------------------------------------------------------------------------------------------------------------------*/

/******************************************************************************
*               MODULE IMPLEMENTATION FILE
*******************************************************************************
* Title: Audio Component Manager (ACM)
*
* Filename: acmTypes.h
*
* Authors: Isar Ariel
*
* Description: ACM typedefs
*
* Last Updated: Jun 11 2009 By Paul Shen
*
* Notes:
******************************************************************************/
# 39 "../../aud_sw/acm/inc/acm.h" 2

typedef enum
{
 ACM_RC_MALLOC_ERROR = 0, //Add by jackie
 ACM_RC_OK = 1,
 ACM_RC_DEVICE_ALREADY_ENABLED,
 ACM_RC_DEVICE_ALREADY_DISABLED,
 ACM_RC_NO_MUTE_CHANGE_NEEDED,
 ACM_RC_INVALID_VOLUME_CHANGE,
 ACM_RC_DEVICE_FORMAT_NOT_FOUND,
 ACM_RC_BUFFER_GET_FUNC_INVALID,
 ACM_RC_STREAM_OUT_NOT_PERFORMED,
 ACM_RC_STREAM_IN_NOT_PERFORMED,
 ACM_RC_STREAM_OUT_TO_BE_STOPPED_NOT_ACTIVE,
 ACM_RC_STREAM_IN_TO_BE_STOPPED_NOT_ACTIVE,
 ACM_RC_I2S_INVALID_DATA_POINTER,
 ACM_RC_I2S_INVALID_DATA_SIZE,
 ACM_RC_I2S_INVALID_NOTIFICATION_THRESHOLD,
 ACM_RC_I2S_MESSAGE_QUEUE_IS_FULL,
 ACM_RC_MEMORY_ALREADY_INITIALISED,

 ACM_RC_NEED_DISABLE_PATH, //Add by jackie

 ACM_RC_ENUM_32_BIT = 0x7FFFFFFF //32bit enum compiling enforcement
} ACM_ReturnCode;


typedef enum
{
 ACM_VOICE_CALL,
 ACM_TONE,
 ACM_PCM,
 ACM_HR,
 ACM_EFR,
 ACM_FR,
 ACM_AMR_MR475,
 ACM_AMR_MR515,
 ACM_AMR_MR59,
 ACM_AMR_MR67,
 ACM_AMR_MR74,
 ACM_AMR_MR795,
 ACM_AMR_MR102,
 ACM_AMR_MR122,
 ACM_DUMMY,
 /* Must be at the end */
 ACM_NO_STREAM_TYPE,
 ACM_NUM_OF_STREAM_TYPES = ACM_NO_STREAM_TYPE,


 ACM_STREAM_TYPE_ENUM_32_BIT = 0x7FFFFFFF //32bit enum compiling enforcement
} ACM_StreamType;


typedef enum
{
 ACM_MSA_PCM = 0, /* first format must be '0' - used by 'for' loops */
 ACM_XSCALE_PCM,
 ACM_I2S,
 ACM_AUDIO_DATA, /* For DAI */
 ACM_AUX_FM,
 ACM_AUX_HW_MIDI,
 ACM_AUX_APP,

 /* Must be at the end */
 ACM_NO_FORMAT,
 ACM_NUM_OF_AUDIO_FORMATS = ACM_NO_FORMAT,


 ACM_AUDIO_FORMAT_ENUM_32_BIT = 0x7FFFFFFF //32bit enum compiling enforcement
} ACM_AudioFormat;

typedef enum
{
 /* first device must be '0' - used by 'for' loops */
 ACM_MAIN_SPEAKER = 0 //TBD as ACM_OUTPUT_DEVICE_1, 0
 , ACM_AUX_SPEAKER //TBD as ACM_OUTPUT_DEVICE_2, 1
 , ACM_STEREO_SPEAKER //TBD as ACM_OUTPUT_DEVICE_3, 2
 , ACM_HEADSET_SPEAKER = ACM_STEREO_SPEAKER
 , ACM_MONO_LEFT_SPEAKER //TBD as ACM_OUTPUT_DEVICE_4, 3
 , ACM_MONO_RIGHT_SPEAKER //TBD as ACM_OUTPUT_DEVICE_5, 4
 , ACM_BUZZER //TBD as ACM_OUTPUT_DEVICE_6, 5
 , ACM_MIC //TBD as ACM_INPUT_DEVICE_1, 6
 , ACM_MIC_DIFF //TBD as ACM_INPUT_DEVICE_2, 7
 , ACM_AUX_MIC //TBD as ACM_INPUT_DEVICE_3, 8
 , ACM_AUX_MIC_DIFF //TBD as ACM_INPUT_DEVICE_4, 9
 , ACM_BLUETOOTH_SPEAKER //TBD as ACM_OUTPUT_DEVICE_7, 10
 , ACM_BLUETOOTH_MIC //TBD as ACM_INPUT_DEVICE_5, 11
 , ACM_DAI_OUT //TBD as ACM_OUTPUT_DEVICE_8, 12
 , ACM_DAI_IN //TBD as ACM_INPUT_DEVICE_6, 13
 , ACM_CAR_KIT_SPEAKER //TBD as ACM_OUTPUT_DEVICE_9, 14
 , ACM_CAR_KIT_MIC //TBD as ACM_INPUT_DEVICE_7, 15
 , ACM_INPUT_DEVICE_8 //former ACM_CUSTOM_MIC1, 16
 , ACM_HEADSET_MIC //former ACM_CUSTOM_MIC2, 17
 , ACM_INPUT_DEVICE_9 = ACM_HEADSET_MIC //17

 , ACM_MIC_EC // Main Mic with EC path enabled
 , ACM_AUX_MIC_EC // Main AUX Mic with EC path enabled
 , ACM_HEADSET_MIC_EC // Headset Mic with EC path enabled
 , ACM_MIC_LOOP_SPEAKER // Main Mic loop to main speaker
 , ACM_MIC_LOOP_EARPIECE // Main Mic loop to main reiver
 , ACM_HEADSET_MIC_LOOP // Headset Mic loop to headset

 , ACM_TTY_IN_45
 , ACM_INPUT_DEVICE_10 = ACM_TTY_IN_45 //former ACM_CUSTOM_MIC3

 , ACM_TTY_IN_50
 , ACM_INPUT_DEVICE_11 = ACM_TTY_IN_50 //former ACM_CUSTOM_MIC4

 , ACM_INPUT_DEVICE_12 //former ACM_CUSTOM_MIC5
 , ACM_INPUT_DEVICE_13 //former ACM_CUSTOM_MIC6
 , ACM_INPUT_DEVICE_14 //former ACM_CUSTOM_MIC7
 , ACM_INPUT_DEVICE_15 //former ACM_CUSTOM_MIC8
 , ACM_INPUT_DEVICE_16 //former ACM_CUSTOM_MIC9
 , ACM_INPUT_DEVICE_17 //former ACM_CUSTOM_MIC10
 , ACM_INPUT_DEVICE_18 //former ACM_CUSTOM_MIC11
 , ACM_INPUT_DEVICE_19 //former ACM_CUSTOM_MIC12
 , ACM_INPUT_DEVICE_20 //former ACM_CUSTOM_MIC13
 , ACM_INPUT_TEST_DEVICE = ACM_INPUT_DEVICE_20
 , ACM_INPUT_DEVICE_21 //former ACM_CUSTOM_MIC14
 , ACM_LINE_OUT = ACM_INPUT_DEVICE_21

 , ACM_INPUT_DEVICE_22 //former ACM_CUSTOM_MIC15
 , ACM_INPUT_DEVICE_23 //former ACM_CUSTOM_MIC16
 , ACM_INPUT_DEVICE_24 //former ACM_CUSTOM_MIC17
 , ACM_INPUT_DEVICE_25 //former ACM_CUSTOM_MIC18
 , ACM_INPUT_DEVICE_26 //former ACM_CUSTOM_MIC19
 , ACM_INPUT_DEVICE_27 //former ACM_CUSTOM_MIC20

 , ACM_TTY_OUT_45
 , ACM_OUTPUT_DEVICE_10 = ACM_TTY_OUT_45 //former ACM_CUSTOM_SPEAKER1

 , ACM_TTY_OUT_50
 , ACM_OUTPUT_DEVICE_11 = ACM_TTY_OUT_50 //former ACM_CUSTOM_SPEAKER2

 , ACM_OUTPUT_DEVICE_12 //former ACM_CUSTOM_SPEAKER3
 , ACM_OUTPUT_DEVICE_13 //former ACM_CUSTOM_SPEAKER4
 , ACM_OUTPUT_DEVICE_14 //former ACM_CUSTOM_SPEAKER5
 , ACM_OUTPUT_DEVICE_15 //former ACM_CUSTOM_SPEAKER6
 , ACM_OUTPUT_DEVICE_16 //former ACM_CUSTOM_SPEAKER7
 , ACM_OUTPUT_DEVICE_17 //former ACM_CUSTOM_SPEAKER8
 , ACM_OUTPUT_DEVICE_18 //former ACM_CUSTOM_SPEAKER9
 , ACM_OUTPUT_DEVICE_19 //former ACM_CUSTOM_SPEAKER10
 , ACM_OUTPUT_DEVICE_20 //former ACM_CUSTOM_SPEAKER11
 , ACM_OUTPUT_TEST_DEVICE = ACM_OUTPUT_DEVICE_20
 , ACM_OUTPUT_DEVICE_21 //former ACM_CUSTOM_SPEAKER12
 , ACM_OUTPUT_DEVICE_22 //former ACM_CUSTOM_SPEAKER13
 , ACM_OUTPUT_DEVICE_23 //former ACM_CUSTOM_SPEAKER14
 , ACM_OUTPUT_DEVICE_24 //former ACM_CUSTOM_SPEAKER15
 , ACM_OUTPUT_DEVICE_25 //former ACM_CUSTOM_SPEAKER16
 , ACM_OUTPUT_DEVICE_26 //former ACM_CUSTOM_SPEAKER17
 , ACM_OUTPUT_DEVICE_27 //former ACM_CUSTOM_SPEAKER18
 , ACM_OUTPUT_DEVICE_28 //former ACM_CUSTOM_SPEAKER19
 , ACM_OUTPUT_DEVICE_29 //former ACM_CUSTOM_SPEAKER20

 , ACM_BLUETOOTH1_SPEAKER
 , ACM_BLUETOOTH1_MIC
 , ACM_BLUETOOTH2_SPEAKER
 , ACM_BLUETOOTH2_MIC
 , ACM_BLUETOOTH3_SPEAKER
 , ACM_BLUETOOTH3_MIC
 , ACM_BLUETOOTH4_SPEAKER
 , ACM_BLUETOOTH4_MIC
 , ACM_BLUETOOTH5_SPEAKER
 , ACM_BLUETOOTH5_MIC
 , ACM_BLUETOOTH6_SPEAKER
 , ACM_BLUETOOTH6_MIC
 , ACM_BLUETOOTH7_SPEAKER
 , ACM_BLUETOOTH7_MIC
 , ACM_BLUETOOTH8_SPEAKER
 , ACM_BLUETOOTH8_MIC
 , ACM_BLUETOOTH9_SPEAKER
 , ACM_BLUETOOTH9_MIC

 //Jackie,2011-0223
 //To keep compatible to old audio_device.nvm, MUST fix ACM_NOT_CONNECTED to 0x50
 //0x50 can not be used as audio device number
 ,ACM_NOT_CONNECTED=0x50

 //Add new audio device after 0x50

 //Jackie,2011-0222
 //Loop include codec and MSA
 ,ACM_MAIN_SPEAKER__LOOP
 ,ACM_AUX_SPEAKER__LOOP
 ,ACM_HEADSET_SPEAKER__LOOP
 ,ACM_MIC__LOOP
 ,ACM_AUX_MIC__LOOP
 ,ACM_HEADSET_MIC__LOOP

 //force speaker
 ,ACM_FORCE_SPEAKER

 //Customized audio device, start from 100
 ,ACM_CUSTOMER_LGE = 100

 // audio device from 0x200 for non-voice-call case
 , ACM_AUDIODEVICE_APP_START=0x200

 /* Must be at the end */
 , ACM_NUM_OF_AUDIO_DEVICES = ACM_NOT_CONNECTED

 , ACM_AUDIO_DEVICE_ENUM_32_BIT = 0x7FFFFFFF //32bit enum compiling enforcement
} ACM_AudioDevice;


typedef enum
{
 ACM_BOTH_ENDS = 0 /* Highest priority */
 , ACM_NEAR_END
 , ACM_FAR_END /* Lowest priority */
 , ACM_NO_END

 , ACM_SRC_DST_ENUM_32_BIT = 0x7FFFFFFF //32bit enum compiling enforcement
} ACM_SrcDst;

//typedef enum
//{
//	ACM_MUTE_OFF = 0,
//	ACM_MUTE_ON = 1,
//
//       ACM_AUDIO_MUTE_ENUM_32_BIT  = 0x7FFFFFFF //32bit enum compiling enforcement
//} ACM_AudioMute;

typedef enum
{
 ACM_NOT_COMB_WITH_CALL = 0, /* Lowest priority */
 ACM_COMB_WITH_CALL = 1, /* Highest priority */

 ACM_COMB_WITH_CALL_ENUM_32_BIT = 0x7FFFFFFF //32bit enum compiling enforcement
} ACM_CombWithCall;


typedef void (*ACM_PacketTransferFunc)(unsigned int *buff);


ACM_ReturnCode ACMAudioStreamInStop(ACM_PacketTransferFunc packetTransferFunc);
ACM_ReturnCode ACMAudioStreamOutStop(ACM_PacketTransferFunc packetTransferFunc);


ACM_ReturnCode ACMAudioStreamInStart(ACM_StreamType streamType,
         ACM_SrcDst srcDst,
         ACM_PacketTransferFunc packetTransferFunc);

ACM_ReturnCode ACMAudioStreamOutStart(ACM_StreamType streamType,
          ACM_SrcDst srcDst,
          ACM_CombWithCall combWithCall,
          ACM_PacketTransferFunc packetTransferFunc);

ACM_ReturnCode ACMAudioDeviceMute(ACM_AudioDevice device,
      ACM_AudioFormat format,
      ACM_AudioMute mute);

ACM_ReturnCode ACMAudioDeviceVolumeSet(ACM_AudioDevice device,
           ACM_AudioFormat format,
           ACM_AudioVolume volume);
ACM_ReturnCode ACMAudioDeviceDisable(ACM_AudioDevice device,
         ACM_AudioFormat format);

ACM_ReturnCode ACMAudioDeviceEnable(ACM_AudioDevice device,
        ACM_AudioFormat format,
        ACM_AudioVolume volume);

ACM_ReturnCode ACMAudioSetCalibrationStatus(ACM_AudioCalibrationStatus calstatus);

ACM_ReturnCode ACMAudioPathEnable(ACM_Component component, unsigned char path, ACM_AudioVolume volume);

ACM_ReturnCode ACMAudioPathDisable(ACM_Component component, unsigned char path);







typedef struct
{
  unsigned char path;
  unsigned char gainID;
  unsigned short slope;
  unsigned short offset;
} ACM_SYNC_Gain;

ACM_ReturnCode ACMAudioSetMSASettings(const char *data);
/* support dynamically switch sample switch */
ACM_ReturnCode ACMSwitchingSampleRate(int rate);

void ACMInit(void);
void ACMDeinit();
# 4 "diag_aud.c" 2
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
# 5 "diag_aud.c" 2
# 1 "diag_aud_eq.h" 1
/*--------------------------------------------------------------------------------------------------------------------
 *    (C) Copyright 2011 Marvell Technology Group Ltd. All Rights Reserved.
 *-------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
 *    MARVELL CONFIDENTIAL
 *    Copyright 2011 Marvell Corporation All Rights Reserved.
 *    The source code contained or described herein and all documents related to the source code are owned
 *    by MArvell Corporation or its suppliers or licensors. Title to the Material remains with Marvell Corporation or
 *    its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
 *    Marvell or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
 *    treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
 *    transmitted, distributed, or disclosed in any way without Marvell's prior express written permission.
 *
 *    No license under any patent, copyright, trade secret or other intellectual property right is granted to or
 *    conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
 *    estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
 *    MArvell in writing.
 *-------------------------------------------------------------------------------------------------------------------*/

/******************************************************************************
 *               MODULE IMPLEMENTATION FILE
 *******************************************************************************
 * Title:
 *
 * Filename: diag_aud_eq.h
 *
 * Authors: Nenghua Cao
 *
 * Description: In ACAT, in order to open EQ files in AP folder, We must export the definitions in AP diag code.
 *
 * Last Updated :
 *
 * Notes:
 * ******************************************************************************/
# 45 "diag_aud_eq.h"
//ICAT EXPORTED ENUM
typedef enum
{
        ON = 0,
        OFF,

        ON_OFF_ENUM_32_BIT = 0x7FFFFFFF /* 32bit enum compiling enforcement */
} On_Off;

//ICAT EXPORTED ENUM
typedef enum
{
        SPEAKER_PHONE_IIR1,
        HANDSET,
        HEADSET,
        CAR_KIT,
        BT_HEADSET,
        SPEAKER_PHONE_IIR2, //Non voice call filter

    BUILTIN_MIC_IIR_1,
    BUILTIN_MIC_IIR_2,
    MAX_APPLIANCE_VALUE,

    APPLIANCE_TYPE_ENUM_32_BIT = 0x7FFFFFFF /* 32bit enum compiling enforcement */
} Appliance;

//ICAT EXPORTED ENUM
typedef enum
{
        MP3,
        AAC,
        OGG,
        MP4,
        AVI,

        CONTAINER_TYPE_ENUM_32_BIT = 0x7FFFFFFF /* 32bit enum compiling enforcement */
} Container_Type;

//ICAT EXPORTED ENUM
typedef enum
{
        SYSTEM,
        RINGTONE,
        MUSIC,
        ALARM,

        STREAM_TYPE_ENUM_32_BIT = 0x7FFFFFFF /* 32bit enum compiling enforcement */
} Stream_Type;


//ICAT EXPORTED STRUCT
typedef struct
{
        unsigned short nSampleRate; //change from NOofTaps. ;
        signed short params[6];
        Appliance appliance;
        On_Off onOff;
        unsigned char description[32];
} AUDIO_HiFi_EQ_Params;

//ICAT EXPORTED STRUCT
typedef struct
{
    Container_Type containerType;
    Stream_Type streamType;
    unsigned short volumeOffset;
} AUDIO_HiFi_Volume_Params;

//ICAT EXPORTED STRUCT
typedef struct
{
    unsigned short noOfTaps;
    short params[6];
    unsigned long nSampleRate;
    Appliance appliance;
    unsigned long nBand;
    On_Off OnOff;

    unsigned char description[32];
} AUDIO_HiFi_MultiEQ_Params;

//ICAT EXPORTED STRUCT
typedef struct{
        int nOffset; //Q16
        int nThreshold; //Q16
        int nCompressFlag; //0: expand 1:compress
        int nRatio; //Q16
}ae_drc_gain_curve_t;



//DRC

//ICAT EXPORTED STRUCT
typedef struct
{
        //Totally (nNumOfDrcSections-1) threshold, compose nNumOfDrcSections sections,

        //eg: 3 threshold, 4 sections

        //[-100, nThreshold0] [nThreshold0, nThreshold1] [nThreshold1, nThreshold2] [nThreshold2, 0]

        ae_drc_gain_curve_t drcGainMap[5];
        unsigned int nNumOfDrcSections;
        unsigned int nAttackTime; //ms
        unsigned int nReleaseTime; //ms
}AUDIO_HIFI_DRC_Params;
# 6 "diag_aud.c" 2

/*******************************************************************************
* Function: ACMAudioPathEnable_Env
*******************************************************************************
* Description: ACMAudioPathEnable wrapper
*
* Parameters: none
*
* Return value: void
*
* Notes:
*******************************************************************************/
//ICAT EXPORTED FUNCTION - Audio, HW, AUDIO_HW_PathEnable
void ACMAudioPathEnable_Env(void *data)
{
 int client, value = 0;
 unsigned char component = 0, path = 0;

 DIAG_FILTER(AUDIO, ACM, ACMAudioPathEnable, DIAG_ALL)
 diagPrintf("ACMAudioPathEnable\n");
 /*
	 *data[0] indicate componentId, data[1] indicate pathID,
	 *data[2] indicate path direction, data[3] indicate volume
	 */
 value |= (ACM_AudioVolume)(*((unsigned char*)data + 3));/*volume*/
 path = (unsigned char)(*((unsigned char *)data + 1)); /*pathnum*/
 component = (ACM_Component)(*((unsigned char *)data)); /*component number*/

 client = 0x00000003;
 value |= (path << 16) | (component << 24);

 audio_route_control(client, 0x00000001, 0x00000005, 0, value);
}

/*******************************************************************************
* Function: ACMAudioPathDisable_Env
*******************************************************************************
* Description: ACMAudioPathDisable wrapper
*
* Parameters: none
*
* Return value: void
*
* Notes:
*******************************************************************************/
//ICAT EXPORTED FUNCTION - Audio, HW, AUDIO_HW_PathDisable
void ACMAudioPathDisable_Env(void *data)
{
 int client, value = 0;
 unsigned char component = 0, path = 0;

 DIAG_FILTER(AUDIO, ACM, ACMAudioPathDisable, DIAG_ALL)
 diagPrintf("ACMAudioPathDisable\n");
 /*
	 *data[0] indicate componentId, data[1] indicate pathID,
	 *data[2] indicate path direction, data[3] indicate volume
	 */
 path = (unsigned char)(*((unsigned short *)data + 1)); //pathnum
 component = (ACM_Component)(*((unsigned short *)data)); //component number

 client = 0x00000003;
 value |= (path << 16) | (component << 24);

 audio_route_control(client, 0x00000001, 0x00000006, 0, value);
}

/*******************************************************************************
* Function: ACMAudioDeviceEnable_Env
*******************************************************************************
* Description: ACMAudioDeviceEnable wrapper
*
* Parameters: none
*
* Return value: void
*
* Notes:
*******************************************************************************/
//ICAT EXPORTED FUNCTION - Audio, ACM, ACMAudioDeviceEnable
void ACMAudioDeviceEnable_Env(void *data)
{
 int client, device, value = 0;

 DIAG_FILTER(AUDIO, ACM, ACMAudioDeviceEnable, DIAG_ALL)
 diagPrintf("ACMAudioDeviceEnable\n");

 value |= (ACM_AudioVolume)(*((unsigned short*)data + 2));//volume

 switch ((ACM_AudioFormat)(*((unsigned short*)data + 1))) {//client
 case ACM_MSA_PCM:
  client = 0x00000003;
  break;
 case ACM_I2S:
  client = 0x00000001;
  break;
 case ACM_AUX_FM:
  client = 0x00000004;
  break;
 default:
  return;
 }

 switch ((ACM_AudioDevice)(*((unsigned short*)data))) {
 case ACM_MAIN_SPEAKER:
  device = 0x00000001;//device
  value |= (0x00000002 << 16);//direction
  break;
 case ACM_AUX_SPEAKER:
  device = 0x00000002;
  value |= (0x00000002 << 16);
  break;
 case ACM_HEADSET_SPEAKER:
  device = 0x00000004;
  value |= (0x00000002 << 16);
  break;
 case ACM_BLUETOOTH_SPEAKER:
  device = 0x00000003;
  value |= (0x00000002 << 16);
  break;
 case ACM_MIC:
  device = 0x00000002;
  value |= (0x00000001 << 16);
  break;
 case ACM_HEADSET_MIC:
  device = 0x00000004;
  value |= (0x00000001 << 16);
  break;
 case ACM_BLUETOOTH_MIC:
  device = 0x00000003;
  value |= (0x00000001 << 16);
  break;
 case ACM_MIC_EC:
  device = 0x00000002;
  value |= (0x00000001 << 16);
  value |= (0x00000080 << 16);
  break;
 case ACM_HEADSET_MIC_EC:
  device = 0x00000004;
  value |= (0x00000001 << 16);
  value |= (0x00000080 << 16);
  break;
 case ACM_AUX_MIC:
  if (client != 0x00000004)
   return;
  device = ACM_AUX_SPEAKER;
  value |= (0x00000001 << 16);
  break;
 case ACM_MAIN_SPEAKER__LOOP:
  device = 0x00000001;//device
  value |= (0x00000002 << 16);//direction
  value |= (0x00000100 << 16);//loopback test
  break;
 case ACM_AUX_SPEAKER__LOOP:
  device = 0x00000002;
  value |= (0x00000002 << 16);
  value |= (0x00000100 << 16);//loopback test
  break;
 case ACM_HEADSET_SPEAKER__LOOP:
  device = 0x00000004;
  value |= (0x00000002 << 16);
  value |= (0x00000100 << 16);//loopback test
  break;
 case ACM_MIC__LOOP:
  device = 0x00000002;
  value |= (0x00000001 << 16);
  value |= (0x00000100 << 16);//loopback test
 case ACM_AUX_MIC__LOOP:
  device = ACM_AUX_SPEAKER;
  value |= (0x00000001 << 16);
  value |= (0x00000100 << 16);//loopback test
  break;
 case ACM_HEADSET_MIC__LOOP:
  device = 0x00000004;
  value |= (0x00000001 << 16);
  value |= (0x00000100 << 16);//loopback test
  break;

 default:
  return;
 }

 audio_route_control(client, 0x00000001, 0x00000001, device, value);
}


/*******************************************************************************
* Function: ACMAudioDeviceDisable_Env
*******************************************************************************
* Description: ACMAudioDeviceDisable wrapper
*
* Parameters: none
*
* Return value: void
*
* Notes:
*******************************************************************************/
//ICAT EXPORTED FUNCTION - Audio, ACM, ACMAudioDeviceDisable
void ACMAudioDeviceDisable_Env(void *data)
{
 int client, device, value = 0;

 DIAG_FILTER(AUDIO, ACM, ACMAudioDeviceDisable, DIAG_ALL)
 diagPrintf("ACMAudioDeviceDisable\n");

 switch ((ACM_AudioFormat)(*((unsigned short*)data + 1))) {//client
 case ACM_MSA_PCM:
  client = 0x00000003;
  break;
 case ACM_I2S:
  client = 0x00000001;
  break;
 case ACM_AUX_FM:
  client = 0x00000004;
  break;
 default:
  return;
 }

 switch ((ACM_AudioDevice)(*((unsigned short*)data))) {
 case ACM_MAIN_SPEAKER:
  device = 0x00000001;//device
  value |= (0x00000002 << 16);//direction
  break;
 case ACM_AUX_SPEAKER:
  device = 0x00000002;
  value |= (0x00000002 << 16);
  break;
 case ACM_HEADSET_SPEAKER:
  device = 0x00000004;
  value |= (0x00000002 << 16);
  break;
 case ACM_BLUETOOTH_SPEAKER:
  device = 0x00000003;
  value |= (0x00000002 << 16);
  break;
 case ACM_MIC:
  device = 0x00000002;
  value |= (0x00000001 << 16);
  break;
 case ACM_HEADSET_MIC:
  device = 0x00000004;
  value |= (0x00000001 << 16);
  break;
 case ACM_BLUETOOTH_MIC:
  device = 0x00000003;
  value |= (0x00000001 << 16);
  break;
 case ACM_MIC_EC:
  device = 0x00000002;
  value |= (0x00000001 << 16);
  value |= (0x00000080 << 16);
  break;
 case ACM_HEADSET_MIC_EC:
  device = 0x00000004;
  value |= (0x00000001 << 16);
  value |= (0x00000080 << 16);
  break;
 case ACM_AUX_MIC:
  if (client != 0x00000004)
   return;
  device = ACM_AUX_SPEAKER;
  value |= (0x00000001 << 16);
  break;
 case ACM_MAIN_SPEAKER__LOOP:
  device = 0x00000001;//device
  value |= (0x00000002 << 16);//direction
  value |= (0x00000100 << 16);//loopback test
  break;
 case ACM_AUX_SPEAKER__LOOP:
  device = 0x00000002;
  value |= (0x00000002 << 16);
  value |= (0x00000100 << 16);//loopback test
  break;
 case ACM_HEADSET_SPEAKER__LOOP:
  device = 0x00000004;
  value |= (0x00000002 << 16);
  value |= (0x00000100 << 16);//loopback test
  break;
 case ACM_MIC__LOOP:
  device = 0x00000002;
  value |= (0x00000001 << 16);
  value |= (0x00000100 << 16);//loopback test
 case ACM_AUX_MIC__LOOP:
  device = ACM_AUX_SPEAKER;
  value |= (0x00000001 << 16);
  value |= (0x00000100 << 16);//loopback test
  break;
 case ACM_HEADSET_MIC__LOOP:
  device = 0x00000004;
  value |= (0x00000001 << 16);
  value |= (0x00000100 << 16);//loopback test
  break;
 default:
  return;
 }

 audio_route_control(client, 0x00000001, 0x00000002, device, value);
}


/*******************************************************************************
* Function: ACMAudioDeviceMute_Env
*******************************************************************************
* Description: ACMAudioDeviceMute wrapper
*
* Parameters: none
*
* Return value: void
*
* Notes:
*******************************************************************************/
//ICAT EXPORTED FUNCTION - Audio, ACM, ACMAudioDeviceMute
void ACMAudioDeviceMute_Env(void *data)
{
 int client, device, value = 0;

 DIAG_FILTER(AUDIO, ACM, ACMAudioDeviceMute, DIAG_ALL)
 diagPrintf("ACMAudioDeviceMute\n");

 value |= (ACM_AudioVolume)(*((unsigned short*)data + 2));//mute

 switch ((ACM_AudioFormat)(*((unsigned short*)data + 1))) {//client
 case ACM_MSA_PCM:
  client = 0x00000003;
  break;
 case ACM_I2S:
  client = 0x00000001;
  break;
 case ACM_AUX_FM:
  client = 0x00000004;
  break;
 default:
  return;
 }

 switch ((ACM_AudioDevice)(*((unsigned short*)data))) {
 case ACM_MAIN_SPEAKER:
  device = 0x00000001;//device
  value |= (0x00000002 << 16);//direction
  break;
 case ACM_AUX_SPEAKER:
  device = 0x00000002;
  value |= (0x00000002 << 16);
  break;
 case ACM_HEADSET_SPEAKER:
  device = 0x00000004;
  value |= (0x00000002 << 16);
  break;
 case ACM_BLUETOOTH_SPEAKER:
  device = 0x00000003;
  value |= (0x00000002 << 16);
  break;
 case ACM_MIC:
  device = 0x00000002;
  value |= (0x00000001 << 16);
  break;
 case ACM_HEADSET_MIC:
  device = 0x00000004;
  value |= (0x00000001 << 16);
  break;
 case ACM_BLUETOOTH_MIC:
  device = 0x00000003;
  value |= (0x00000001 << 16);
  break;
 case ACM_MIC_EC:
  device = 0x00000002;
  value |= (0x00000001 << 16);
  value |= (0x00000080 << 16);
  break;
 case ACM_HEADSET_MIC_EC:
  device = 0x00000004;
  value |= (0x00000001 << 16);
  value |= (0x00000080 << 16);
  break;
 case ACM_AUX_MIC:
  if (client != 0x00000004)
   return;
  device = ACM_AUX_SPEAKER;
  value |= (0x00000001 << 16);
  break;
 case ACM_MAIN_SPEAKER__LOOP:
  device = 0x00000001;//device
  value |= (0x00000002 << 16);//direction
  value |= (0x00000100 << 16);//loopback test
  break;
 case ACM_AUX_SPEAKER__LOOP:
  device = 0x00000002;
  value |= (0x00000002 << 16);
  value |= (0x00000100 << 16);//loopback test
  break;
 case ACM_HEADSET_SPEAKER__LOOP:
  device = 0x00000004;
  value |= (0x00000002 << 16);
  value |= (0x00000100 << 16);//loopback test
  break;
 case ACM_MIC__LOOP:
  device = 0x00000002;
  value |= (0x00000001 << 16);
  value |= (0x00000100 << 16);//loopback test
 case ACM_AUX_MIC__LOOP:
  device = ACM_AUX_SPEAKER;
  value |= (0x00000001 << 16);
  value |= (0x00000100 << 16);//loopback test
  break;
 case ACM_HEADSET_MIC__LOOP:
  device = 0x00000004;
  value |= (0x00000001 << 16);
  value |= (0x00000100 << 16);//loopback test
  break;
 default:
  return;
 }

 audio_route_control(client, 0x00000001, 0x00000003, device, value);
}


/*******************************************************************************
* Function: ACMAudioDeviceVolumeSet_Env
*******************************************************************************
* Description: ACMAudioDeviceVolumeSet wrapper
*
* Parameters: none
*
* Return value: void
*
* Notes:
*******************************************************************************/
//ICAT EXPORTED FUNCTION - Audio, ACM, ACMAudioDeviceVolumeSet
void ACMAudioDeviceVolumeSet_Env(void *data)
{
 int client, device, value = 0;

 DIAG_FILTER(AUDIO, ACM, AACMAudioDeviceVolumeSet, DIAG_ALL)
 diagPrintf("ACMAudioDeviceVolumeSet\n");

 value |= (ACM_AudioVolume)(*((unsigned short*)data + 2));//volume

 switch ((ACM_AudioFormat)(*((unsigned short*)data + 1))) {//client
 case ACM_MSA_PCM:
  client = 0x00000003;
  break;
 case ACM_I2S:
  client = 0x00000001;
  break;
 case ACM_AUX_FM:
  client = 0x00000004;
  break;
 default:
  return;
 }

 switch ((ACM_AudioDevice)(*((unsigned short*)data))) {
 case ACM_MAIN_SPEAKER:
  device = 0x00000001;//device
  value |= (0x00000002 << 16);//direction
  break;
 case ACM_AUX_SPEAKER:
  device = 0x00000002;
  value |= (0x00000002 << 16);
  break;
 case ACM_HEADSET_SPEAKER:
  device = 0x00000004;
  value |= (0x00000002 << 16);
  break;
 case ACM_BLUETOOTH_SPEAKER:
  device = 0x00000003;
  value |= (0x00000002 << 16);
  break;
 case ACM_MIC:
  device = 0x00000002;
  value |= (0x00000001 << 16);
  break;
 case ACM_HEADSET_MIC:
  device = 0x00000004;
  value |= (0x00000001 << 16);
  break;
 case ACM_BLUETOOTH_MIC:
  device = 0x00000003;
  value |= (0x00000001 << 16);
  break;
 case ACM_MIC_EC:
  device = 0x00000002;
  value |= (0x00000001 << 16);
  value |= (0x00000080 << 16);
  break;
 case ACM_HEADSET_MIC_EC:
  device = 0x00000004;
  value |= (0x00000001 << 16);
  value |= (0x00000080 << 16);
  break;
 case ACM_AUX_MIC:
  if (client != 0x00000004)
   return;
  device = ACM_AUX_SPEAKER;
  value |= (0x00000001 << 16);
  break;
 case ACM_MAIN_SPEAKER__LOOP:
  device = 0x00000001;//device
  value |= (0x00000002 << 16);//direction
  value |= (0x00000100 << 16);//loopback test
  break;
 case ACM_AUX_SPEAKER__LOOP:
  device = 0x00000002;
  value |= (0x00000002 << 16);
  value |= (0x00000100 << 16);//loopback test
  break;
 case ACM_HEADSET_SPEAKER__LOOP:
  device = 0x00000004;
  value |= (0x00000002 << 16);
  value |= (0x00000100 << 16);//loopback test
  break;
 case ACM_MIC__LOOP:
  device = 0x00000002;
  value |= (0x00000001 << 16);
  value |= (0x00000100 << 16);//loopback test
 case ACM_AUX_MIC__LOOP:
  device = ACM_AUX_SPEAKER;
  value |= (0x00000001 << 16);
  value |= (0x00000100 << 16);//loopback test
  break;
 case ACM_HEADSET_MIC__LOOP:
  device = 0x00000004;
  value |= (0x00000001 << 16);
  value |= (0x00000100 << 16);//loopback test
  break;
 default:
  return;
 }

 audio_route_control(client, 0x00000001, 0x00000004, device, value);
}

/*******************************************************************************
* Function: ACMAudioCalibrationStateSet_Env
*******************************************************************************
* Description: Set the phone work state.
*
* Parameters: 0 indicate it will enater normal state,
* 	      1 indicate it will enater calibration state
*
* Return value: void
*
* Notes:
*******************************************************************************/
//ICAT EXPORTED FUNCTION - Audio, ACM, ACMAudioCalibrationStateSet
void ACMAudioCalibrationStateSet_Env(void *data)
{
 int client, value = 0;

 value |= (ACM_AudioCalibrationStatus)(*((unsigned short*)data));/*calibtation state*/

 /*This diag command isn't relate with client type, and we select PHONE_CLIENT to switch the calibration state*/
 client = 0x00000003;

 DIAG_FILTER(AUDIO, ACM, ACMAudioSetCalibrationStateSet, DIAG_ALL)
 diagPrintf("calibration state: %s\n", value?"ACM_CALIBRATION_ON":"ACM_CALIBRATION_OFF");

 value |= (0X00000400 << 16);
 audio_route_control(client, 0x00000001, 0, 0, value);
}
