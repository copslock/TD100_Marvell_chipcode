# 1 "src/EE_Postmortem.c"
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/errorhandler//"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "src/EE_Postmortem.c"
/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code (“Material”) are owned
   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
   transmitted, distributed, or disclosed in any way without Intel’s prior express written permission.

   No license under any patent, copyright, trade secret or other intellectual property right is granted to or
   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
   Intel in writing.
   -------------------------------------------------------------------------------------------------------------------*/

/*******************************************************************************
*               MODULE HEADER FILE
********************************************************************************
*
* Filename: EE_Postmortem.c
*
* The BOERNE could access the whole HARBELL's DDR for postmortem debugging
* Let's use this possibility for the fetching HARBELL-DDR dump into BIN-file.
* This is especially usable in case of exceptions.
*
* There are next utilities:
*  - SaveComMIPSbuf     keep the MIPS-RAM   buffer. The Address of buffer is FIXED-PreDefined
*  - SaveComEEbuf       keep the EE-handler buffer. The Address of buffer is FIXED-PreDefined
*  - SaveCom_Addr_Len   make dump of any COMM-DDR area started from with a given address and length.
*                                    The address may be given even in the COMM-addressing format (0xD0Xxxxxx)
*
*******************************************************************************/
# 47 "src/EE_Postmortem.c"
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


# 48 "src/EE_Postmortem.c" 2
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
typedef signed char __int8_t;
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
# 49 "src/EE_Postmortem.c" 2

# 1 "../include/pxa_dbg.h" 1
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
# 12 "../include/pxa_dbg.h" 2
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


# 13 "../include/pxa_dbg.h" 2







extern int eeh_draw_panic_text(const char *panic_text, size_t panic_len, int do_timer);
extern void eeh_log(int level, const char *format, ...);
# 51 "src/EE_Postmortem.c" 2





# 1 "./inc/EE_Postmortem.h" 1
/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code (“Material”) are owned
   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
   transmitted, distributed, or disclosed in any way without Intel’s prior express written permission.

   No license under any patent, copyright, trade secret or other intellectual property right is granted to or
   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
   Intel in writing.
   -------------------------------------------------------------------------------------------------------------------*/

/*******************************************************************************
*               MODULE HEADER FILE
********************************************************************************
*
* Filename: EE_Postmortem.h
*
* Description:
*  Descriptor-Table describing all buffers used by case of Exception or Assert
*  for postmortem debugging.
*
*******************************************************************************/





# 1 "./inc/loadTable.h" 1
/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code (“Material”) are owned
   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
   transmitted, distributed, or disclosed in any way without Intel’s prior express written permission.

   No license under any patent, copyright, trade secret or other intellectual property right is granted to or
   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
   Intel in writing.
   -------------------------------------------------------------------------------------------------------------------*/

/***************************************************************************
 *               MODULE IMPLEMENTATION FILE
 ****************************************************************************
 *
 * Filename: loadTable.h
 *
 * The OBM is responsible to copy image from flash to the DDR.
 * It doesn't know about real image size and always copy the maximum 7MB.
 * The problems:
 *  - long time for copying (about 2 sec),
 *  - all ERR/spy/debug buffers are overwriten.
 *
 * SOLUTION:
 * Put the Image BEGIN and END address onto predefined area - offset_0x1C0 size 0x40
 * Add the the text-signature to recognize are these addresses present in image or not.
 * The signature is following the BEGIN/END and is next ":BEGIN:END:LOAD_TABLE_SIGNATURE"
 * OBM should check signature in flash and if it is present MAY use the size=(END-BEGIN).
 * If signature is invalid the default 7MB is used.
 * The IMAGE_END region added into scatter file
 *
 ******************************************************************************/

/*=======================================================================*/
/*        NOTE: This file may be used by OBM or WIN-CE                   */
/*=======================================================================*/







# 1 "../include/global_types.h" 1
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
# 28 "../include/global_types.h" 2

# 1 "../include/gbl_types.h" 1
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

# 1 "../include/linux_types.h" 1
/* ===========================================================================
   File        : posix_types.h
   Description : Global types file for the POSIX Linux environment.

   Notes       : This file is designed for use in the gnu environment
	      and is referenced from the gbl_types.h file. Use of
			  this file requires ENV_POSIX to be defined in posix_env.mak.

   Copyright 2005, Intel Corporation, All rights reserved.
   =========================================================================== */
# 23 "../include/linux_types.h"
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
# 74 "../include/linux_types.h"
/*                         end of posix_types.h
   --------------------------------------------------------------------------- */
# 55 "../include/gbl_types.h" 2




/*                         end of gbl_types.h
   --------------------------------------------------------------------------- */
# 30 "../include/global_types.h" 2
# 1 "../include/linux_types.h" 1
/* ===========================================================================
   File        : posix_types.h
   Description : Global types file for the POSIX Linux environment.

   Notes       : This file is designed for use in the gnu environment
	      and is referenced from the gbl_types.h file. Use of
			  this file requires ENV_POSIX to be defined in posix_env.mak.

   Copyright 2005, Intel Corporation, All rights reserved.
   =========================================================================== */
# 74 "../include/linux_types.h"
/*                         end of posix_types.h
   --------------------------------------------------------------------------- */
# 31 "../include/global_types.h" 2

///////////////////////////////////////////////////////////////////////////
// UPDATE - Wrap Definition for Linux
///////////////////////////////////////////////////////////////////////////
# 52 "../include/global_types.h"
///////////////////////////////////////////////////////////////////////////
// END OF UPDATE
///////////////////////////////////////////////////////////////////////////


/* Standard typedefs */
//typedef unsigned char   BOOL;         /* Boolean                        */
# 78 "../include/global_types.h"
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
# 54 "./inc/loadTable.h" 2
# 65 "./inc/loadTable.h"
//#if defined (_TAVOR_HARBELL_) || defined (_TAVOR_BOERNE_)
# 80 "./inc/loadTable.h"
//Offfset of the LOAD_TABLE = 1C0 (in the First Flash-block)


typedef struct
{
 UINT32 imageBegin; // image addresses are in HARBELL address space 0xD0??.????
 UINT32 imageEnd; //                 for BOERNE use conversion to 0xBF??.????
 char Signature[16];
 UINT32 sharedFreeBegin;
 UINT32 sharedFreeEnd;
 UINT32 ramRWbegin;
 UINT32 ramRWend;
 char spare[24]; /* This area may be used for data transfer from loader to COMM */
}LoadTableType; /*total size 0x40=64bytes */




extern UINT32 getCommImageBaseAddr(void);
extern void getAppComShareMemAddr(UINT32* begin, UINT32* end);


UINT32 ConvertPhysicalAddrToVirtualAddr(UINT32 PhysicalAddr);
void commImageTableInit();
void commImageTableFree(void);
# 114 "./inc/loadTable.h"
extern LoadTableType loadTable; /* do NOT use "const" in EXTERN prototype */


extern LoadTableType *pLoadTable;





//#endif// defined (_TAVOR_HARBELL_) || defined (_TAVOR_BOERNE_)
# 38 "./inc/EE_Postmortem.h" 2
# 53 "./inc/EE_Postmortem.h"
//   The final FILE-name is "com_"#POSTMORTEM_BUF_NAME_xx#".bin"



//ICAT EXPORTED STRUCT
typedef struct
{
 char name[(7 + 1)]; //8 bytes    Name  for the buffer to be accessed or saved into file
 UINT8* bufAddr; //4 bytes   pointer to the buffer to be accessed or saved into file
 UINT32 bufLen; //4 bytes   length  of the buffer to be accessed or saved into file
} EE_PostmortemDesc_Entry; //=16 bytes

extern EE_PostmortemDesc_Entry EE_PostmortemDesc[(15 + 1)];

//on HARBELL: AVOR_SHARED_MEM_ADDR_0 == (UINT32)&boerne_shared_data
//on BOERNE:  there is no label






//#define PUB_ADDR_OF_PTR2POSTMORTEM_DESC        ADDR_CONVERT (TAVOR_SHARED_MEM_ADDR_2)




/* export ICAT functions */



void SaveComPostmortem(void); //Save all buffers from the Dec-Table into files
void SaveCom_Addr_Len(void* p); //Generic procedure to save into file "comm_"POSTMORTEM_BUF_NAME_GENDUMP".bin"
UINT32 EELOG_shareReadFWrite(void); // Log file-stream





void Linux_EELOG_shareReadFWrite(void); // Log file-stream







//#define EE_POSTMORTEM_EXTEND_ICAT_EXPORT





extern BOOL EE_PostmortemRegistration(char* name, UINT8* bufAddr, UINT32 bufLen);
extern void EE_SaveComDDR_RW(UINT32 sizeLimitBytes);

extern void EE_SaveComDDR_DiagOffline(UINT32 sizeLimitBytes);

extern BOOL GetComEEDescbuf(char *pDesc, int len);
# 57 "src/EE_Postmortem.c" 2

# 1 "./inc/EEHandler.h" 1
/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code (“Material”) are owned
   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
   transmitted, distributed, or disclosed in any way without Intel’s prior express written permission.

   No license under any patent, copyright, trade secret or other intellectual property right is granted to or
   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
   Intel in writing.
   -------------------------------------------------------------------------------------------------------------------*/

/*******************************************************************************
*               MODULE HEADER FILE
********************************************************************************
* Title: Error Handler header
*
* Filename: EEHandler.h
*
* Target, platform: Manitoba
*
* Authors: Yossi Hanin
*
* Description: Error handler component header file
*
* Last Updated:
*
* Notes:
*******************************************************************************/





/*----------- Local include files --------------------------------------------*/


# 1 "../include/global_types.h" 1
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
# 47 "./inc/EEHandler.h" 2
# 1 "./inc/rtc.h" 1
/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

/******************************************************************************
*                MODULE HEADER FILE
*******************************************************************************
*  COPYRIGHT (C) 2001 Intel Corporation.
*
*  This file and the software in it is furnished under
*  license and may only be used or copied in accordance with the terms of the
*  license. The information in this file is furnished for informational use
*  only, is subject to change without notice, and should not be construed as
*  a commitment by Intel Corporation. Intel Corporation assumes no
*  responsibility or liability for any errors or inaccuracies that may appear
*  in this document or any software that may be provided in association with
*  this document.
*  Except as permitted by such license, no part of this document may be
*  reproduced, stored in a retrieval system, or transmitted in any form or by
*  any means without the express written consent of Intel Corporation.
*
*  Title: rtc
*
*  Filename: rtc.h
*
*  Target, subsystem: Common Platform, HAL
*
*  Authors:  Avi Erami, Eilam Ben-Dror
*
*  Description:  rtc header file.
*
*  Last Modified: 24.07.2001
*
*  Notes:
******************************************************************************/




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


# 41 "./inc/rtc.h" 2
# 1 "../include/global_types.h" 1
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
# 42 "./inc/rtc.h" 2



/*------------ Global type definitions --------------------------------------*/

/*
	RTC_CalendarTime structure is used in all API that receive/retrieve the date and the time.
	It may be converted into the ANSI time.h tm structure by using the RTCToANSIConvert service.
 */
//ICAT EXPORTED STRUCT
typedef struct
{
 UINT8 second; //  Seconds after minute: 0 - 59
 UINT8 minute; //  Minutes after hour: 0 - 59
 UINT8 hour; //  Hours after midnight: 0 - 23
 UINT8 day; //  Day of month: 1 - 31
 UINT8 month; //  Month of year: 1 - 12
 UINT16 year; //  Calendar year: e.g 2001
}RTC_CalendarTime;


/*
	RTC_Trim structure is used in the RTCConfigure and RTCConfigurationGet services. It specifies
	the calibrated frequency of the Hz clock.
 */
typedef struct
{
 UINT16 clkDividerCount; //	Number of 32KHz clock cycles +1, per Hz clock cycle.
 UINT16 trimDeleteCount; //	Number of 32KHz clock cycles to delete when trimming begins.
 BOOL locked; //	Indicates whether the value within the trim register can be changed.
}RTC_Trim;


//The handle is received when binding an ISR to the RTC periodic interrupt, and is used for unbinding.
typedef UINT8 RTC_Handle;


//Return Code Definitions:
typedef enum
{
 RTC_RC_OK = 1,

 RTC_RC_MAKETIME_FAILURE = -100,
 RTC_RC_INVALID_PARAMETERS,
 RTC_RC_ALARM_ALREADY_SET,
 RTC_RC_ALARM_TIME_ALREADY_PASSED,
 RTC_RC_NO_ALARM_SET,
 RTC_RC_NO_FREE_HANDLE,
 RTC_RC_INVALID_HANDLE,
 RTC_RC_NO_ISR_BOUND,
 RTC_RC_HW_FAILURE,
 RTC_RC_TRIM_LOCKED,
 RTC_RC_INVALID_BACKUP_REGISTER,
 RTC_RC_BAD_TRIM_PARAMETER,
 RTC_RC_FAIL
}RTC_ReturnCode;


typedef enum
{
 RTC_BACKUP_REG_0 = 0,
 RTC_BACKUP_REG_1 = 1,
 RTC_BACKUP_REG_2 = 2,
 RTC_BACKUP_REG_3 = 3,
 RTC_BACKUP_REG_4 = 4,
 RTC_BACKUP_MAX_REGS = 5
} RTC_BACKUP_REG;

typedef enum
{
 RTC_ALARM_NEGATE = 0x0,
 RTC_ALARM_ASSERT = 0x1
} RTC_ALARM_SIGNAL;


// This is a prototype for an ISR that may be bound to the RTC interrupts
typedef void (*RTC_ISR)(void);

// This is a prototype for a callback function that may be bound to the RTC On Time Set notify service
typedef void (*RTCOnTimeSetNotifyCallback)(RTC_CalendarTime* oldTime, RTC_CalendarTime* newTime);

/*---------------------------------------------------------------------------*/

/*---------- Global variable declarations -----------------------------------*/

/*---------- Global constant declarations -----------------------------------*/




/*---------- Exported function prototypes -------------------------------------*/


void RTCReset(void);
void RTCUnreset(void);
void RTCTrimReset(void);
void RTCTrimUnreset(void);


RTC_ReturnCode RTCFromANSIConvert(const struct tm * ANSIStruct, RTC_CalendarTime *calendarTime);
RTC_ReturnCode RTCToANSIConvert(RTC_CalendarTime *dateAndTime, struct tm * ANSIStruct);
RTC_ReturnCode RTCDateAndTimeSet(RTC_CalendarTime *dateAndTime);
RTC_ReturnCode RTCCurrentDateAndTimeGet(RTC_CalendarTime * dateAndTime);
RTC_ReturnCode RTCAlarmBind(RTC_ISR isr);
RTC_ReturnCode RTCAlarmSet(RTC_CalendarTime *dateAndTime, RTC_ISR isr);
RTC_ReturnCode RTCAlarmGet(RTC_CalendarTime *dateAndTime);
RTC_ReturnCode RTCAlarmReset(void);
RTC_ReturnCode RTCPeriodicBind(RTC_ISR isr, RTC_Handle *handle);
RTC_ReturnCode RTCPeriodicUnbind(RTC_Handle handle);
RTC_ReturnCode RTCHwCheck(void);
RTC_ReturnCode RTCPhase1Init(void);
RTC_ReturnCode RTCPhase2Init(void);
RTC_ReturnCode RTCConfigure(RTC_Trim * trim);
RTC_ReturnCode RTCConfigurationGet(RTC_Trim * trim);
BOOL RTCPowerUpAlarmCheck(void);
BOOL RTCIsAlarmSet(void);
BOOL RTCHasAlarmInterrupted(void);
SwVersion RTCVersionGet(void);

RTC_ReturnCode RTCControlAlarmSignalRead( RTC_ALARM_SIGNAL *alarm_signal );
RTC_ReturnCode RTCControlAlarmSignalWrite( RTC_ALARM_SIGNAL alarm_signal );
RTC_ReturnCode RTCBackupRegisterWrite( RTC_BACKUP_REG backup_reg_num, UINT32 reg_value );
RTC_ReturnCode RTCBackupRegisterRead( RTC_BACKUP_REG backup_reg_num, UINT32 *reg_value );

RTC_ReturnCode RTCNotifyOnTimeSetBind(RTCOnTimeSetNotifyCallback OnTimeSetCallback);
RTC_ReturnCode RTCNotifyOnTimeSetUnBind(RTCOnTimeSetNotifyCallback OnTimeSetCallback);
RTC_ReturnCode RTCNotifyUsers(RTC_CalendarTime *oldRTC, RTC_CalendarTime *newRTC);
void TimeGetTest( void );


//Functions that use time_t type:
time_t time(time_t * retrievedTime);
RTC_ReturnCode timeSet(time_t newTime);
RTC_ReturnCode alarmSet(time_t alarmTime);
time_t alarmGet(time_t * retrievedTime);

time_t read_rtc(time_t *rtc);
int write_rtc(struct tm *tm);
void calib_rtc(void);
# 48 "./inc/EEHandler.h" 2
# 58 "./inc/EEHandler.h"
//////////#define COMPILER_DOES_NOT_SUPPORT_SHORT_ENUMS


/******************************************************************************************/
/*                                 GENERAL                                                */
/******************************************************************************************/

//ICAT EXPORTED ENUM
typedef enum
{
 //enumerated
 EE_SYS_RESET_EN,
 EE_ASSERT_EN,
 EE_EXCEPTION_EN,
 EE_WARNING_EN,
 EE_NUM_ENTRY_TYPES,
 // Codes
 EE_SYS_RESET = 300,
 EE_ASSERT = 350,
 EE_EXCEPTION = 450,
 EE_WARNING = 550



} EE_entry_t_;
typedef UINT16 EE_entry_t;





//ICAT EXPORTED ENUM
typedef enum
{
 EEE_DataAbort,
 EEE_PrefetchAbort,
 EEE_FatalError,
 EEE_SWInterrupt,
 EEE_UndefInst,
 EEE_ReservedInt



} EE_ExceptionType_t_;
typedef UINT8 EE_ExceptionType_t;
# 137 "./inc/EEHandler.h"
//ICAT EXPORTED ENUM
typedef enum
{
 EE_NO_RESET_SOURCE,



 EE_POWER_ON_RESET,

 EE_EXT_MASTER_RESET,



 EE_WDT_RESET




} EE_PMU_t_;
typedef UINT8 EE_PMU_t;





/* secondary exception types */
typedef enum
{
 VERTICAL = 0,
 ALIGNMENT_1 = 1,
 TERMINAL = 2,
 ALIGNMENT_2 = 3,
 EXTERNAL_ABORT_ON_TRANSLATION = 12,
 EXTERNAL_ABORT_ON_TRANSLATION_2 = 14,
 TRANSLATION = 5,
 TRANSLATION_2 = 7,
 EE_DOMAIN = 9,
 EE_DOMAIN_2 = 11,
 PERMISSION = 13,
 PERMISSION_2 = 15,
 IMPRECISE_EXTERNAL_DATA_ABORT = 6,
 LOCK_ABORT = 4,
 DATA_CACHE_PARITY_ERROR = 8,
 INSTRUCTION_MMU = 0,
 EXTERNAL_INSTRUCTION_ERROR = 6,
 INSTRUCTION_CACHE_PARITY_ERROR = 8
} EXCEPTION_TYPE;

/******************************************************************************************/
/*                            CPU register context format                                 */
/******************************************************************************************/

//ICAT EXPORTED STRUCT
typedef struct
{
 UINT32 r0; /* register r0 contents */
 UINT32 r1; /* register r1 contents */
 UINT32 r2; /* register r2 contents */
 UINT32 r3; /* register r3 contents */
 UINT32 r4; /* register r4 contents */
 UINT32 r5; /* register r5 contents */
 UINT32 r6; /* register r6 contents */
 UINT32 r7; /* register r7 contents */
 UINT32 r8; /* register r8 contents */
 UINT32 r9; /* register r9 contents */
 UINT32 r10; /* register r10 contents */
 UINT32 r11; /* register r11 contents */
 UINT32 r12; /* register r12 contents */
 UINT32 SP; /* register r13 contents */
 UINT32 LR; /* register r14 contents (excepted mode)*/
 UINT32 PC; /* PC - excepted instruction */
 UINT32 cpsr; /* saved program status register contents */
 UINT32 FSR; /* Fault status register */
 UINT32 FAR_R; /* Fault address register */
 EE_PMU_t PMU_reg; /* saved reset cause - should be last */

 // UINT32              PESR;  /* Extension */
 // UINT32              XESR;
 // UINT32              PEAR;
 // UINT32              FEAR;
 // UINT32              SEAR;
 // UINT32              GEAR;
} EE_RegInfo_Data_t;

//
// Context types: the two enumerations below must be synchronized
//

//ICAT EXPORTED ENUM
typedef enum
{
 EE_CT_None,
 EE_CT_ExecTrace,
 EE_CT_StackDump



} EE_ContextType_t_; // This enumerates configureable types of contents to be saved in the context buffer
typedef UINT8 EE_ContextType_t;


//ICAT EXPORTED ENUM
typedef enum
{
 EE_CDT_None,
 EE_CDT_ExecTrace,
 EE_CDT_StackDump,
 EE_CDT_UserDefined = 0x10



} EE_ContextDataType_t_; // This enumerates configureable types of contents to be saved in the context buffer
typedef UINT8 EE_ContextDataType_t;



//ICAT EXPORTED STRUCT
typedef struct
{
 UINT32 _PESR;
 UINT32 _XESR;
 UINT32 _PEAR;
 UINT32 _FEAR;
 UINT32 _SEAR;
 UINT32 _GEAR;
} EE_XscGasketRegs;

/******************************************************************************************/
/*                            LOG ENTRY FORMAT                                            */
/******************************************************************************************/
//ICAT EXPORTED STRUCT
typedef struct
{
 UINT16 fileWriteOffset; //DO NOT REMOVE OR CHANGE TYPE!!!(for cyclic file)
 EE_entry_t type;
 RTC_CalendarTime dateAndTime;
 char desc[100]; /* Description string size =ERROR_HANDLER_MAX_DESC_SIZE*/
 EE_RegInfo_Data_t RegInfo;
 EE_ContextDataType_t contextBufferType;
 UINT8 contextBuffer[256 * 2];
 UINT32 CHKPT0;
 UINT32 CHKPT1;
 char taskName[10];
 UINT32 taskStackStart;
 UINT32 taskStackEnd;
 //UP TO HERE 0x1e4 bytes (out of 0x200 allocated by linker control file INT_RAM_EE segment)
 EE_XscGasketRegs xscaleGasketRegs;
 UINT32 warningCntr; //reserved[1];
} EE_Entry_t;


/*
 * Actions the EEHandler may take depending on the event specifics
 */
typedef enum
{
 eehActionFatal,
 eehActionLog,
 eehActionIgnore
}EEHandlerAction;

/******************************************************************************************/
/*            PRIMARY EXCEPTION HANDLERS (ENTRIES CALLED BY LOW LEVEL HANDLERS            */
/*            RETURN CODES LISTED                                                         */
/******************************************************************************************/




UINT32 BranchZeroIntHandler( void );
UINT32 DataAbortHandler( void );
UINT32 PrefetchAbortHandler( void );
UINT32 FatalErrorHandler( void );
UINT32 SWInterruptHandler( void );
UINT32 UndefInstHandler( void );
UINT32 ReservedIntHandler( void );


/******************************************************************************************/
/*                                MAINTENANCE                                             */
/******************************************************************************************/

//for configuration and initiliazation
void eeHandlerPhase1Init(void);
void eeHandlerPhase2Init(void);
void eeHandlerPhase3Init(void);
void eeHandlerWatchdogSet(void);
void eeHandlerWatchdogUpdate(UINT16 wdtConfigTimeMSec);

void errHandlerSaveToFDIPeriodic( void );

void EeHandlerReadFile( void );
void EeHandlerTestDataAbortExcep( void );

/******************************************************************************************/
/*                             ERROR ENTRIES (API)                                        */
/******************************************************************************************/
/* Error string only */
void eeHandlerSaveDescAssert(const char* desc);

/* Error string and user context data */
void eeHandlerSaveDescAssertData(const char* desc, const UINT8* data, UINT32 dataLen);

/* Error string, file name and line number */
void eeHandlerSaveDescFileLineAssert(const char* desc, const char *file, UINT16 line);
void eeWarning(const char *file, int line, int warningMask);

/* Error string, file name and line number, and register context */
void eeHandlerSaveDescFileLineAssertRegs(const char* desc, const char *file, UINT16 line, const EE_RegInfo_Data_t* regs);


//for Warning without a reset
char eeHandlerSaveDescWarning(const char *errorDescription);
char eeHandlerSaveDescFileLineWarning(const char *desc, const char *file, UINT16 line);
char eeHandlerSaveDescFileLineParamWarning(const char*, const char*, UINT16, UINT32, UINT32, UINT32);

//for Warning with DSP reset only
void eeHandlerSaveDescDspResetWarning(const char *desc, char DSPresetOn);

/******************************************************************************************/
/*                       EXTENDED HANDING: ACTION BINDING API                             */
/******************************************************************************************/

/* A limited number of independent handlers can be bound */
typedef void FatalEventAction_fn (const char* errorString, //mandatory, event description string
      const char* file, //optional, may appear as NULL
      int line, //optional, may appear as 0
      EE_entry_t type, //mandatory, event type (enum)
      const EE_RegInfo_Data_t * regContext, //register context
      RTC_CalendarTime * calendarTime); //RTC time

//------- generic "global" function pointer --------
typedef void voidPFuncVoid (void);
typedef enum
{
 EE_ActionAlways = 0,
 EE_ActionSkipBeforeReset = 1, // don't run this action when immediate final action is reset
 EE_ActionDefer = 2, // defer this action if possible and applicable
 EE_ActionDeferredOnly = 4 // run this action only when deferred
}EE_ActionCondition;
BOOL EEHandlerFatalErrorActionBind(FatalEventAction_fn* pFunc, UINT32 mode);

FatalEventAction_fn* EEHandlerExternalFinalActionBind(FatalEventAction_fn* pFunc);
BOOL EELOG_RegisterForLogstream(voidPFuncVoid* pF);

/******************************************************************************************/
/*                       SECONDARY EXCEPTION HANDLERS API                                 */
/******************************************************************************************/

typedef void (*ExceptionHendler)(EE_RegInfo_Data_t*);

void ExceptionDataHandlerReplace(EXCEPTION_TYPE, ExceptionHendler);
void ExceptionDataHandlerRestore(EXCEPTION_TYPE, ExceptionHendler);
void ExceptionPrefetchHandlerReplace(EXCEPTION_TYPE, ExceptionHendler);
void ExceptionPrefetchHandlerRestore(EXCEPTION_TYPE, ExceptionHendler);

// Binds an external handler to track all exceptions
typedef EEHandlerAction (*ExceptionHendlerExt)(EE_ExceptionType_t type, EE_RegInfo_Data_t*);
ExceptionHendlerExt eeExtExceptionHandlerBind(ExceptionHendlerExt newHandler);

// Returns the offset from excepted instruction to the PC saved on exception (positive)
int eeGetPcOffset(EE_ExceptionType_t et, const EE_RegInfo_Data_t* regContext);
int eeGetPcStep(const EE_RegInfo_Data_t* regContext);


EEHandlerAction eehCallSecondaryHandler(UINT8 index, EE_RegInfo_Data_t* pSaved_Registers);



// Return Codes:
typedef enum
{
 WATCHDOG_RC_OK = 1,

 WATCHDOG_RC_NO_HANDLER_REGISTERED = -100,
 WATCHDOG_RC_NULL_POINTER,
 WATCHDOG_RC_INTC_ERROR,
 WATCHDOG_RC_BAD_MATCH_VALUE

}WATCHDOG_ReturnCode;
# 59 "src/EE_Postmortem.c" 2
# 1 "./inc/EEHandler_int.h" 1
/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code (“Material”) are owned
   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
   transmitted, distributed, or disclosed in any way without Intel’s prior express written permission.

   No license under any patent, copyright, trade secret or other intellectual property right is granted to or
   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
   Intel in writing.
   -------------------------------------------------------------------------------------------------------------------*/

/******************************************************************************
*               MODULE IMPLEMENTATION FILE
*******************************************************************************
* Title: Error Handler
*
* Filename: EEHandler_int.h
*
* Target, platform: Hermon
*
* Authors: Anton Eidelman
*
* Description: Implementation file: Error handler configuration and NVM definitions
*
*
*
* Notes:
******************************************************************************/





# 1 "../include/global_types.h" 1
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
# 44 "./inc/EEHandler_int.h" 2
# 52 "./inc/EEHandler_int.h"
# 1 "./inc/fdi_file.h" 1
/******************************************************************************
*(C) Copyright 2008 Marvell International Ltd.
* All Rights Reserved
******************************************************************************/
/*--------------------------------------------------------------------------------------------------------------------
 *  -------------------------------------------------------------------------------------------------------------------
 *
 *  Filename: fdi_file.h
 *
 *  Description: The APIs to operate files.
 *
 *  History:
 *   March, 2009 - Rovin Yu Creation of file
 *
 *  Notes:
 *
 ******************************************************************************/

/******************************************************************************
*    Include files
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
# 23 "./inc/fdi_file.h" 2
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

# 24 "./inc/fdi_file.h" 2
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








# 25 "./inc/fdi_file.h" 2




extern FILE *FDI_fopen(const char *fName, char *openFlags);

extern int FDI_fread(void *buffer, size_t size, size_t count, FILE* stream);

extern int FDI_fwrite(void *buffer, size_t size, size_t count, FILE* stream);

extern int FDI_fseek(FILE *stream, long offset, int position );

extern int FDI_fclose(FILE* stream);

extern int FDI_remove(void* fileName);
# 53 "./inc/EEHandler_int.h" 2
# 61 "./inc/EEHandler_int.h"
# 1 "./inc/EEHandler_config.h" 1
/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code (“Material”) are owned
   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
   transmitted, distributed, or disclosed in any way without Intel’s prior express written permission.

   No license under any patent, copyright, trade secret or other intellectual property right is granted to or
   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
   Intel in writing.
   -------------------------------------------------------------------------------------------------------------------*/




# 1 "../diag/diag_al/diag_API.h" 1
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
# 25 "./inc/EEHandler_config.h" 2


/*----------- Global defines -------------------------------------------------*/






//#define EE_WARNINGS
# 43 "./inc/EEHandler_config.h"
//#define EE_INTERNAL_ASSERT_DIAG






// Enables the "Deferred actions" feature


// Enable to run deferred intermediate actions (e.g. LCD printout) not under the thread callback,
// but under the low priority task (public service). This is recommended, because not all actions
// can always be run under the thread callback, which might be an HISR (LCD print impossible).
// Still, using the low priority task service might be not guaranteed under certain error conditions.




// Set OS current thread to a known task thread before running the deferred final action
// This allows the final action code to use OS calls, which are not allowed under HISR
// (relevant external final action).
// NOTE: these OS calls must not be blocking (de-facto), otherwise scheduling might be resumed causing unpredictable results.


// Enables binding of an extended exception handler to catch all exceptions
// Extended handler allows the system to recognize and ignore certain exceptions


// Synchronize the external memory to allow "post mortem" memory content upload: effective for non-reset final actions
# 62 "./inc/EEHandler_int.h" 2
# 1 "./inc/EEHandler_nvm.h" 1
/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code (“Material<94>) are owned
   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
   transmitted, distributed, or disclosed in any way without Intel’s prior express written permission.

   No license under any patent, copyright, trade secret or other intellectual property right is granted to or
   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
   Intel in writing.
   -------------------------------------------------------------------------------------------------------------------*/

/******************************************************************************
*               MODULE IMPLEMENTATION FILE
*******************************************************************************
* Title: Error Handler
*
* Filename: EEHandler_nvm.h
*
* Target, platform: Hermon
*
* Authors: Anton Eidelman
*
* Description: Implementation file: Error handler configuration and NVM definitions
*
*
*
* Notes:
******************************************************************************/





//#include "global_types.h"
# 53 "./inc/EEHandler_nvm.h"
# 1 "./inc/EEHandler.h" 1
/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code (“Material”) are owned
   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
   transmitted, distributed, or disclosed in any way without Intel’s prior express written permission.

   No license under any patent, copyright, trade secret or other intellectual property right is granted to or
   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
   Intel in writing.
   -------------------------------------------------------------------------------------------------------------------*/

/*******************************************************************************
*               MODULE HEADER FILE
********************************************************************************
* Title: Error Handler header
*
* Filename: EEHandler.h
*
* Target, platform: Manitoba
*
* Authors: Yossi Hanin
*
* Description: Error handler component header file
*
* Last Updated:
*
* Notes:
*******************************************************************************/
# 54 "./inc/EEHandler_nvm.h" 2
# 1 "./inc/EEHandler_config.h" 1
/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code (“Material”) are owned
   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
   transmitted, distributed, or disclosed in any way without Intel’s prior express written permission.

   No license under any patent, copyright, trade secret or other intellectual property right is granted to or
   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
   Intel in writing.
   -------------------------------------------------------------------------------------------------------------------*/
# 55 "./inc/EEHandler_nvm.h" 2
# 65 "./inc/EEHandler_nvm.h"
//ICAT EXPORTED STRUCT
typedef struct NVM_Header
{
 unsigned long StructSize; // the size of the user structure below
 unsigned long NumofStructs; // >1 in case of array of structs (default is 1).
 char StructName[64]; // the name of the user structure below
 char Date[32]; // date updated by the ICAT when the file is saved. Filled by calibration SW.
 char time[32]; // time updated by the ICAT when the file is saved. Filled by calibration SW.
 char Version[64]; // user version - this field is updated by the SW eng. Every time they update the UserStruct.
 char HW_ID[32]; // signifies the board number. Filled by calibration SW.
 char CalibVersion[32]; // signifies the calibration SW version. Filled by calibration SW.
} NVM_Header_ts;


//ICAT EXPORTED ENUM
typedef enum
{
 EE_RESET,
 EE_STALL,
 EE_RESERVED,
 EE_EXTERNAL,
 EE_RESET_START_BASIC,
 EE_RESET_COMM_SILENT



} FinalAct_t_;
typedef UINT8 FinalAct_t;
# 102 "./inc/EEHandler_nvm.h"
//ICAT EXPORTED ENUM
typedef enum
{
 EE_OFF,
 EE_ON



} EE_OnOff_t_;
typedef UINT8 EE_OnOff_t;



//ICAT EXPORTED ENUM
typedef enum
{
 EE_WARN_OFF,
 EE_WARN_ASSERTCONTINUE,
 EE_WARN_ASSERT



} EE_WarningOn_t_;
typedef UINT8 EE_WarningOn_t;




//ICAT EXPORTED ENUM
typedef enum
{
 EE_VER_3 = 3,
 EE_VER



} EE_Version_t_;
typedef UINT8 EE_Version_t;



//ICAT EXPORTED ENUM
typedef enum
{
 EE_HSL_OFF = 0,
 EE_HSL_1_8V = 1,
 EE_HSL_3V = 3



} EE_HSL_V_t_;
typedef UINT8 EE_HSL_V_t;




//ICAT EXPORTED ENUM
typedef enum
{
 EE_WDT_OFF = 0,
 EE_WDT_TIME_6SEC = 6000, //in miliseconds; for max WCDMA/GSM DRX cycle
 EE_WDT_TIME_7SEC = 7000, //in miliseconds
 EE_WDT_TIME_8SEC = 8000, //in miliseconds
 EE_WDT_TIME_10SEC = 10000, //in miliseconds
 EE_WDT_TIME_20SEC = 20000, //in miliseconds
 EE_WDT_TIME_30SEC = 30000, //in miliseconds
 EE_WDT_TIME_MAX = 0xFFFF //UINT16



} EE_WdtTimeCfg_t_;
typedef UINT16 EE_WdtTimeCfg_t;
# 182 "./inc/EEHandler_nvm.h"
//ICAT EXPORTED ENUM
typedef enum
{
 EE_DEFER_NONE,
 EE_DEFER_ASSERTS,
 EE_DEFER_EXCEPTIONS,
 EE_DEFER_ALL



} EE_DeferredMode_t_;
typedef UINT8 EE_DeferredMode_t;



//ICAT EXPORTED STRUCT
typedef struct
{
 EE_DeferredMode_t enable;
 EE_OnOff_t deferIntermediateActions;
 UINT16 limitMs;
 UINT16 limitHits;
 UINT16 reserved2;
}EE_DeferredCfg_t;


//ICAT EXPORTED STRUCT
typedef struct
{
 EE_OnOff_t AssertHandlerOn;
 EE_OnOff_t ExcepHandlerOn;
 EE_WarningOn_t WarningHandlerOn;
 EE_OnOff_t powerUpLogOn;
 EE_OnOff_t extHandlersOn;
 EE_OnOff_t printRecentLogOnStartup;
 FinalAct_t finalAction;
 UINT32 EELogFileSize;
 UINT16 delayOnStartup; // Delay for ICAT log coverage in 5ms units
 EE_ContextType_t assertContextBufType;
 EE_ContextType_t exceptionContextBufType;
 EE_ContextType_t warningContextBufType;

 //-------- version 1+D ----------
 EE_DeferredCfg_t deferredCfg;



 EE_WdtTimeCfg_t wdtConfigTime; //UINT16
 UINT16 sysEeHandlerLimit; //relevant for EE_ASSISTING_MASTER only; ZERO is no limits
 UINT32 dumpDdrSizeBytes; //relevant for EE_ASSISTING_MASTER only
  UINT8 reserved[24];
}EE_Configuration_t;


typedef enum
{
 EE_NVMCONFIG_UNCODITIONAL,
 EE_NVMCONFIG_EARLY, /* used for Non NVM_ON_REMOTE */
 EE_NVMCONFIG_LATEST /* used for NVM_ON_REMOTE     */
}EE_NvmConfigMode_t;


//
// EEHandler Internal services
//

//
BOOL eeReadConfiguration(EE_Configuration_t* pCfgBuf);
void eeSaveConfiguration( void );
void eeSetConfiguration(const EE_Configuration_t* pConfig, int len);
void eePrintConfiguration( void );
void eeInitDfltConfiguration(void);
BOOL eeInitNvmConfigSuccess(EE_NvmConfigMode_t mode);
//#ifdef __cplusplus
void eeInitConfiguration(void);
//#endif

//
// EEHandler Internal data
//

extern EE_Configuration_t eeConfiguration;
# 63 "./inc/EEHandler_int.h" 2

/*----------- Global macro definitions ---------------------------------------*/





/* This is not important error, just print it */
# 83 "./inc/EEHandler_int.h"
/*
 * File names
 */
extern const char* const eehLogFileName;

// INTERNAL PROTOTYPES
void exceptionHandlersBind(void);

void exceptionHandlerPhase1Init( void );
void exceptionHandlerPhase2Init( void );

//Only for the EE
void excepHandlerSaveExceptionToFile( void );
int EeHandlerPrintEntry(const EE_Entry_t* entry);
void errHandlerSaveEntryToFile(EE_Entry_t* entry);
void errHandlerPreFinalAction(FinalAct_t action);
void errHandlerFinalAction(FinalAct_t action);
EEHandlerAction eeExtExceptionHandler(EE_ExceptionType_t type, EE_RegInfo_Data_t* pContext);


void ExceptionInChipTraceBufferCopy(void);

typedef struct
{
 UINT16 logSignature;
 UINT16 logChksum;
 EE_Entry_t log[1];
} EE_LogBuffer;



/*----------- Extern definition ----------------------------------------------*/
/*extern*/
extern EE_RegInfo_Data_t __Saved_Registers;
extern void Final_Action_Reset(void);
# 60 "src/EE_Postmortem.c" 2







//The EE_PostmortemDesc may be printed when ICAT postmortem command sent
//#define PRINT_POSTMORTEM_DESC
# 95 "src/EE_Postmortem.c"
UINT32 EE_AssistingMaster_ASSERT_found = 0;


BOOL EE_PostmortemRegistration(char* name, UINT8* bufAddr, UINT32 bufLen)
{
 (void)name;
 (void)bufAddr;
 (void)bufLen;


 return 1;
# 143 "src/EE_Postmortem.c"
}
# 162 "src/EE_Postmortem.c"
static void SaveToFile(char* fileName, UINT8* pBuf, UINT32 len)
{


 FILE* fdiID;





 UINT32 count;
 char fNameBuf[256] = {0};
 UINT32 nameLen;

 //cut given name if too long
 nameLen = strlen(fileName);
 if (nameLen >= (7 + 1))
  fileName[(7 + 1) - 1] = 0;







 if(!strcmp(fileName, "DDR_RW") /* for DDR_RW, save it in DDR_RW_SAVE_DIR */

  || !strcmp(fileName, "DIAGLOG") /* for DIAG LOG OFFLINE, save it in DDR_RW_SAVE_DIR */

  )
  sprintf(fNameBuf, "%s/com_%s.bin", "/data", fileName);
 else

  sprintf(fNameBuf, "com_%s.bin", fileName);
 //The FDI does NOT update the FILE-time-date attributes. So delete and create with new date
 //if(FStatusIsExist(fNameBuf))


 FDI_remove((char *)fNameBuf);


 if (len == 0)
 {
  DIAG_FILTER(SW_PLAT, PERFORMANCE, SAVE2FILE0, DIAG_INFORMATION)
  diagPrintf("COMM MEM-DUMP File %s: nothing to save", fNameBuf);
 }
 else
 {
  //pBuf = ADDR_CONVERT(pBuf);

  if ((fdiID = FDI_fopen(fNameBuf, "wb")) != 0)





  {

   pBuf = (UINT8*)ConvertPhysicalAddrToVirtualAddr((UINT32)(pBuf));
   count = FDI_fwrite(pBuf, sizeof(char), len, fdiID);
   FDI_fclose(fdiID);






   DIAG_FILTER(SW_PLAT, PERFORMANCE, SAVE2FILE, DIAG_INFORMATION)
   diagPrintf("COMM MEM-DUMP File %s: %ld bytes saved", fNameBuf, count);
  }
 }
}


static EE_PostmortemDesc_Entry* getEntryNext(int getFirst)
{
 void* whellKnownAddr = (void*)(ConvertPhysicalAddrToVirtualAddr((UINT32)(((UINT8*)((UINT32)(getCommImageBaseAddr() + (3 * sizeof(void*))))))));
 static EE_PostmortemDesc_Entry * pCurr;
 static int idx;

 if (getFirst)
 {
  eeh_log(7, "%s: PUB_ADDR_OF_PTR2POSTMORTEM_DESC:0x%x\n", __FUNCTION__, ((UINT8*)((UINT32)(getCommImageBaseAddr() + (3 * sizeof(void*))))));
  eeh_log(7, "%s: wellKnownAddr:0x%x\n", __FUNCTION__, whellKnownAddr);
  pCurr = (EE_PostmortemDesc_Entry*)/*ADDR_CONVERT*/ (*(UINT32*)/*ADDR_CONVERT*/ (whellKnownAddr));
  idx = 0;
 }
 else
 {
  pCurr++;
  idx++;

  pCurr = (EE_PostmortemDesc_Entry *)ConvertPhysicalAddrToVirtualAddr((UINT32)(pCurr));
  if ((((EE_PostmortemDesc_Entry*)/*ADDR_CONVERT*/ (pCurr))->name[0] == 0) || (idx >= (15 + 1)))
   return(((EE_PostmortemDesc_Entry*)0xDEADDEAD));
 }
 eeh_log(7, "%s: pCurr:0x%x\n", __FUNCTION__, pCurr);
 return( (EE_PostmortemDesc_Entry*)/*ADDR_CONVERT*/ (pCurr) );
}


static EE_PostmortemDesc_Entry* getEntryByName(char* name)
{
 int i;

 EE_PostmortemDesc_Entry * retAddr = ((EE_PostmortemDesc_Entry*)0xDEADDEAD);
 void* whellKnownAddr = (void*)ConvertPhysicalAddrToVirtualAddr((UINT32)(((UINT8*)((UINT32)(getCommImageBaseAddr() + (3 * sizeof(void*)))))));
 EE_PostmortemDesc_Entry * p = (EE_PostmortemDesc_Entry*)(*(UINT32*)(whellKnownAddr));

 p = (EE_PostmortemDesc_Entry*)ConvertPhysicalAddrToVirtualAddr((UINT32)(p));

 for (i = 0; i < (15 + 1); i++)
 {
  if (p->name[0] == 0)
   break; //Last END-entry. Just exit
  else
  {
   if (strcmp(p->name, name) == 0)
   {
    retAddr = p;
    break; // found
   }
   p++;
  }
 }
 return( (EE_PostmortemDesc_Entry*)/*ADDR_CONVERT*/ (retAddr) );
}

static void printEEinfo(UINT8* pBuf)
{
 // The pBuf points onto __CurrentExceptionLog.log
 // Keep it in the pEElog after Comm/App address convertion.
 // Since we need also the buffer CheckSum and Signature
 // let's find also the buffer beginning pEE.

 EE_Entry_t * pEElog = ((EE_Entry_t*)/*ADDR_CONVERT*/ (pBuf));
 EE_LogBuffer* pEE;
 UINT32 logOffset;

 logOffset = (UINT32)&( ((EE_LogBuffer*)pEElog)->log );
 logOffset -= (UINT32)pEElog;

 pEE = (EE_LogBuffer*)( (UINT32)pEElog - logOffset );

 EE_AssistingMaster_ASSERT_found = (pEE->logSignature == 0x57E2);
 if (EE_AssistingMaster_ASSERT_found)
 {
  DIAG_FILTER(Diag, comMem, EE_LOG_TXT, DIAG_INFORMATION)
  diagPrintf("COMM EE LOG: %s", pEElog->desc);
  DIAG_FILTER(Diag, comMem, EE_LOG, DIAG_INFORMATION)
  diagStructPrintf("COMM EE LOG:  %S{EE_Entry_t}", (UINT16 *)pEElog, sizeof(EE_Entry_t));
 }
 else
 {
  DIAG_FILTER(Diag, comMem, EE_LOG_NOERR, DIAG_INFORMATION)
  diagPrintf("COMM WDT expired (NOT caused by ASSERT)" );
 }
}

//-----------------------------------------------------------------------------
//ICAT EXPORTED FUNCTION - Diag,comMem,SaveComPostmortem
void SaveComPostmortem(void)
{
 EE_PostmortemDesc_Entry* p; // = getEntryByName( POSTMORTEM_BUF_NAME_EEBUF );

 p = getEntryNext(1 /*getFirst*/);
 while (p != ((EE_PostmortemDesc_Entry*)0xDEADDEAD))
 {




  p = (EE_PostmortemDesc_Entry*)ConvertPhysicalAddrToVirtualAddr((UINT32)(p));
  eeh_log(7, "%s: p: 0x%x\n", __FUNCTION__, p);
  if (strcmp(p->name, "EE_Hbuf") == 0)
  {
   printEEinfo((UINT8*)ConvertPhysicalAddrToVirtualAddr((UINT32)(p->bufAddr)));
  }

  SaveToFile( p->name, /*ADDR_CONVERT*/ (p->bufAddr), p->bufLen);
  p = getEntryNext(0 /*getFirst*/);
 }
}

//ICAT EXPORTED FUNCTION - Diag,comMem,SaveCom_Addr_Len
void SaveCom_Addr_Len(void* p)
{
 UINT32* pVal = (UINT32*)ConvertPhysicalAddrToVirtualAddr((UINT32)(p));
 UINT8* pBuf = (UINT8*)pVal[0];
 UINT32 len = pVal[1];


 SaveToFile( "dumpBuf", pBuf, len);
}

//-----------------------------------------------------------------------------
void EE_SaveComDDR_RW(UINT32 sizeLimitBytes)
{
 LoadTableType *p = (LoadTableType*)ConvertPhysicalAddrToVirtualAddr((UINT32)(pLoadTable));
 UINT8* pBuf;
 UINT32 len;

 if (p != (LoadTableType*)0xBAD0DEAD)
 {
  pBuf = (UINT8*)(p->ramRWbegin);

  if (sizeLimitBytes == 0) //do noting
   return;
  if (sizeLimitBytes <= 1024) //do NOT limit but use default - full dump
   len = p->ramRWend - p->ramRWbegin + 4;
  else
  { //len = MIN(A,B)
   len = ((p->ramRWend - p->ramRWbegin + 4) < sizeLimitBytes) ?
          (p->ramRWend - p->ramRWbegin + 4) : sizeLimitBytes;
  }
  DIAG_FILTER(Diag, comMem, DDR_RW, DIAG_INFORMATION)
  diagPrintf("WAIT 1MIN! Be patient! DDR_RW Dumping 0x%lx bytes of DDR_RW", len);
  SaveToFile( "DDR_RW", pBuf, len); //"com_DDR_RW.bin"
 }
}

//-----------------------------------------------------------------------------
void EE_SaveComDDR_DiagOffline(UINT32 sizeLimitBytes)
{
 UINT8* pBuf = (UINT8*)(0x7000000);
 UINT32 len;

 if (pBuf != ((void *)0))
 {
  if (sizeLimitBytes == 0) //do noting
   return;
  if (sizeLimitBytes <= 1024) //do NOT limit but use default - full dump
   len = 0x73ffffc - 0x7000000 + 4;
  else
  { //len = MIN(A,B)
   len = ((0x73ffffc - 0x7000000 + 4) < sizeLimitBytes) ?
          (0x73ffffc - 0x7000000 + 4) : sizeLimitBytes;
  }
  DIAG_FILTER(Diag, comMem, DDR_DiagOffline, DIAG_INFORMATION)
  diagPrintf("WAIT 30 seconds! Be patient! DDR_DiagOffline Dumping 0x%lx bytes of DIAGLOG", len);
  SaveToFile( "DIAGLOG", pBuf, len); //"com_DIAGLOG.bin"
 }
}


//Get CP Error Description Information
BOOL GetComEEDescbuf(char *pDesc, int len)
{
 BOOL res = 0;
 EE_PostmortemDesc_Entry* p = getEntryByName( "EE_Hbuf" );

 if (p != ((EE_PostmortemDesc_Entry*)0xDEADDEAD))
 {
  EE_Entry_t * pEElog = ((EE_Entry_t*)ConvertPhysicalAddrToVirtualAddr((UINT32)(p->bufAddr)));
  EE_LogBuffer* pEE;
  UINT32 logOffset;

  logOffset = (UINT32)&( ((EE_LogBuffer*)pEElog)->log );
  logOffset -= (UINT32)pEElog;

  pEE = (EE_LogBuffer*)( (UINT32)pEElog - logOffset );

  EE_AssistingMaster_ASSERT_found = (pEE->logSignature == 0x57E2);
  if (EE_AssistingMaster_ASSERT_found)
  {
   snprintf(pDesc, len, "COMM EE LOG: %s", pEElog->desc);
   res = 1;
  }
  else
   snprintf(pDesc, len, "COMM WDT expired (NOT caused by ASSERT)");
 }
 else
  snprintf(pDesc, len, "COMM WDT expired (NOT caused by ASSERT)");
 return res;
}
