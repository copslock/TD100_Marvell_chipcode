# 1 "diag_al.c"
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/diag/diag_al//"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "diag_al.c"
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



# 2 "diag_al.h" 2
# 12 "diag_al.h"
int diag_init(void);
void diag_sig_action(int signum, siginfo_t * info, void *p);
# 2 "diag_al.c" 2
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
# 3 "diag_al.c" 2
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
# 4 "diag_al.c" 2
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
# 5 "diag_al.c" 2
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
# 6 "diag_al.c" 2
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

# 7 "diag_al.c" 2
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


# 8 "diag_al.c" 2
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
# 9 "diag_al.c" 2
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
# 10 "diag_al.c" 2
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
# 11 "diag_al.c" 2
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


# 12 "diag_al.c" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/rtnetlink.h" 1
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
# 16 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/rtnetlink.h" 2
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
# 17 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/rtnetlink.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/if_link.h" 1
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
# 16 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/if_link.h" 2
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
# 17 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/if_link.h" 2

struct rtnl_link_stats
{
 __u32 rx_packets;
 __u32 tx_packets;
 __u32 rx_bytes;
 __u32 tx_bytes;
 __u32 rx_errors;
 __u32 tx_errors;
 __u32 rx_dropped;
 __u32 tx_dropped;
 __u32 multicast;
 __u32 collisions;

 __u32 rx_length_errors;
 __u32 rx_over_errors;
 __u32 rx_crc_errors;
 __u32 rx_frame_errors;
 __u32 rx_fifo_errors;
 __u32 rx_missed_errors;

 __u32 tx_aborted_errors;
 __u32 tx_carrier_errors;
 __u32 tx_fifo_errors;
 __u32 tx_heartbeat_errors;
 __u32 tx_window_errors;

 __u32 rx_compressed;
 __u32 tx_compressed;
};

struct rtnl_link_ifmap
{
 __u64 mem_start;
 __u64 mem_end;
 __u64 base_addr;
 __u16 irq;
 __u8 dma;
 __u8 port;
};

enum
{
 IFLA_UNSPEC,
 IFLA_ADDRESS,
 IFLA_BROADCAST,
 IFLA_IFNAME,
 IFLA_MTU,
 IFLA_LINK,
 IFLA_QDISC,
 IFLA_STATS,
 IFLA_COST,

 IFLA_PRIORITY,

 IFLA_MASTER,

 IFLA_WIRELESS,

 IFLA_PROTINFO,

 IFLA_TXQLEN,

 IFLA_MAP,

 IFLA_WEIGHT,

 IFLA_OPERSTATE,
 IFLA_LINKMODE,
 IFLA_LINKINFO,

 IFLA_NET_NS_PID,
 IFLA_IFALIAS,
 __IFLA_MAX
};






enum
{
 IFLA_INET6_UNSPEC,
 IFLA_INET6_FLAGS,
 IFLA_INET6_CONF,
 IFLA_INET6_STATS,
 IFLA_INET6_MCAST,
 IFLA_INET6_CACHEINFO,
 IFLA_INET6_ICMP6STATS,
 __IFLA_INET6_MAX
};



struct ifla_cacheinfo
{
 __u32 max_reasm_len;
 __u32 tstamp;
 __u32 reachable_time;
 __u32 retrans_time;
};

enum
{
 IFLA_INFO_UNSPEC,
 IFLA_INFO_KIND,
 IFLA_INFO_DATA,
 IFLA_INFO_XSTATS,
 __IFLA_INFO_MAX,
};



enum
{
 IFLA_VLAN_UNSPEC,
 IFLA_VLAN_ID,
 IFLA_VLAN_FLAGS,
 IFLA_VLAN_EGRESS_QOS,
 IFLA_VLAN_INGRESS_QOS,
 __IFLA_VLAN_MAX,
};



struct ifla_vlan_flags {
 __u32 flags;
 __u32 mask;
};

enum
{
 IFLA_VLAN_QOS_UNSPEC,
 IFLA_VLAN_QOS_MAPPING,
 __IFLA_VLAN_QOS_MAX
};



struct ifla_vlan_qos_mapping
{
 __u32 from;
 __u32 to;
};
# 18 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/rtnetlink.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/if_addr.h" 1
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
# 16 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/if_addr.h" 2
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
# 17 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/if_addr.h" 2

struct ifaddrmsg
{
 __u8 ifa_family;
 __u8 ifa_prefixlen;
 __u8 ifa_flags;
 __u8 ifa_scope;
 __u32 ifa_index;
};

enum
{
 IFA_UNSPEC,
 IFA_ADDRESS,
 IFA_LOCAL,
 IFA_LABEL,
 IFA_BROADCAST,
 IFA_ANYCAST,
 IFA_CACHEINFO,
 IFA_MULTICAST,
 __IFA_MAX,
};
# 53 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/if_addr.h"
struct ifa_cacheinfo
{
 __u32 ifa_prefered;
 __u32 ifa_valid;
 __u32 cstamp;
 __u32 tstamp;
};
# 19 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/rtnetlink.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/neighbour.h" 1
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
# 16 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/neighbour.h" 2
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
# 17 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/neighbour.h" 2

struct ndmsg
{
 __u8 ndm_family;
 __u8 ndm_pad1;
 __u16 ndm_pad2;
 __s32 ndm_ifindex;
 __u16 ndm_state;
 __u8 ndm_flags;
 __u8 ndm_type;
};

enum
{
 NDA_UNSPEC,
 NDA_DST,
 NDA_LLADDR,
 NDA_CACHEINFO,
 NDA_PROBES,
 __NDA_MAX
};
# 56 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/neighbour.h"
struct nda_cacheinfo
{
 __u32 ndm_confirmed;
 __u32 ndm_used;
 __u32 ndm_updated;
 __u32 ndm_refcnt;
};

struct ndt_stats
{
 __u64 ndts_allocs;
 __u64 ndts_destroys;
 __u64 ndts_hash_grows;
 __u64 ndts_res_failed;
 __u64 ndts_lookups;
 __u64 ndts_hits;
 __u64 ndts_rcv_probes_mcast;
 __u64 ndts_rcv_probes_ucast;
 __u64 ndts_periodic_gc_runs;
 __u64 ndts_forced_gc_runs;
};

enum {
 NDTPA_UNSPEC,
 NDTPA_IFINDEX,
 NDTPA_REFCNT,
 NDTPA_REACHABLE_TIME,
 NDTPA_BASE_REACHABLE_TIME,
 NDTPA_RETRANS_TIME,
 NDTPA_GC_STALETIME,
 NDTPA_DELAY_PROBE_TIME,
 NDTPA_QUEUE_LEN,
 NDTPA_APP_PROBES,
 NDTPA_UCAST_PROBES,
 NDTPA_MCAST_PROBES,
 NDTPA_ANYCAST_DELAY,
 NDTPA_PROXY_DELAY,
 NDTPA_PROXY_QLEN,
 NDTPA_LOCKTIME,
 __NDTPA_MAX
};


struct ndtmsg
{
 __u8 ndtm_family;
 __u8 ndtm_pad1;
 __u16 ndtm_pad2;
};

struct ndt_config
{
 __u16 ndtc_key_len;
 __u16 ndtc_entry_size;
 __u32 ndtc_entries;
 __u32 ndtc_last_flush;
 __u32 ndtc_last_rand;
 __u32 ndtc_hash_rnd;
 __u32 ndtc_hash_mask;
 __u32 ndtc_hash_chain_gc;
 __u32 ndtc_proxy_qlen;
};

enum {
 NDTA_UNSPEC,
 NDTA_NAME,
 NDTA_THRESH1,
 NDTA_THRESH2,
 NDTA_THRESH3,
 NDTA_CONFIG,
 NDTA_PARMS,
 NDTA_STATS,
 NDTA_GC_INTERVAL,
 __NDTA_MAX
};
# 20 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/rtnetlink.h" 2

enum {
 RTM_BASE = 16,


 RTM_NEWLINK = 16,

 RTM_DELLINK,

 RTM_GETLINK,

 RTM_SETLINK,


 RTM_NEWADDR = 20,

 RTM_DELADDR,

 RTM_GETADDR,


 RTM_NEWROUTE = 24,

 RTM_DELROUTE,

 RTM_GETROUTE,


 RTM_NEWNEIGH = 28,

 RTM_DELNEIGH,

 RTM_GETNEIGH,


 RTM_NEWRULE = 32,

 RTM_DELRULE,

 RTM_GETRULE,


 RTM_NEWQDISC = 36,

 RTM_DELQDISC,

 RTM_GETQDISC,


 RTM_NEWTCLASS = 40,

 RTM_DELTCLASS,

 RTM_GETTCLASS,


 RTM_NEWTFILTER = 44,

 RTM_DELTFILTER,

 RTM_GETTFILTER,


 RTM_NEWACTION = 48,

 RTM_DELACTION,

 RTM_GETACTION,


 RTM_NEWPREFIX = 52,


 RTM_GETMULTICAST = 58,


 RTM_GETANYCAST = 62,


 RTM_NEWNEIGHTBL = 64,

 RTM_GETNEIGHTBL = 66,

 RTM_SETNEIGHTBL,


 RTM_NEWNDUSEROPT = 68,


 RTM_NEWADDRLABEL = 72,

 RTM_DELADDRLABEL,

 RTM_GETADDRLABEL,


 RTM_GETDCB = 78,

 RTM_SETDCB,


 __RTM_MAX,

};





struct rtattr
{
 unsigned short rta_len;
 unsigned short rta_type;
};
# 144 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/rtnetlink.h"
struct rtmsg
{
 unsigned char rtm_family;
 unsigned char rtm_dst_len;
 unsigned char rtm_src_len;
 unsigned char rtm_tos;

 unsigned char rtm_table;
 unsigned char rtm_protocol;
 unsigned char rtm_scope;
 unsigned char rtm_type;

 unsigned rtm_flags;
};

enum
{
 RTN_UNSPEC,
 RTN_UNICAST,
 RTN_LOCAL,
 RTN_BROADCAST,
 RTN_ANYCAST,
 RTN_MULTICAST,
 RTN_BLACKHOLE,
 RTN_UNREACHABLE,
 RTN_PROHIBIT,
 RTN_THROW,
 RTN_NAT,
 RTN_XRESOLVE,
 __RTN_MAX
};
# 194 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/rtnetlink.h"
enum rt_scope_t
{
 RT_SCOPE_UNIVERSE=0,

 RT_SCOPE_SITE=200,
 RT_SCOPE_LINK=253,
 RT_SCOPE_HOST=254,
 RT_SCOPE_NOWHERE=255
};






enum rt_class_t
{
 RT_TABLE_UNSPEC=0,

 RT_TABLE_COMPAT=252,
 RT_TABLE_DEFAULT=253,
 RT_TABLE_MAIN=254,
 RT_TABLE_LOCAL=255,
 RT_TABLE_MAX=0xFFFFFFFF
};

enum rtattr_type_t
{
 RTA_UNSPEC,
 RTA_DST,
 RTA_SRC,
 RTA_IIF,
 RTA_OIF,
 RTA_GATEWAY,
 RTA_PRIORITY,
 RTA_PREFSRC,
 RTA_METRICS,
 RTA_MULTIPATH,
 RTA_PROTOINFO,
 RTA_FLOW,
 RTA_CACHEINFO,
 RTA_SESSION,
 RTA_MP_ALGO,
 RTA_TABLE,
 __RTA_MAX
};






struct rtnexthop
{
 unsigned short rtnh_len;
 unsigned char rtnh_flags;
 unsigned char rtnh_hops;
 int rtnh_ifindex;
};
# 266 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/rtnetlink.h"
struct rta_cacheinfo
{
 __u32 rta_clntref;
 __u32 rta_lastuse;
 __s32 rta_expires;
 __u32 rta_error;
 __u32 rta_used;


 __u32 rta_id;
 __u32 rta_ts;
 __u32 rta_tsage;
};

enum
{
 RTAX_UNSPEC,

 RTAX_LOCK,

 RTAX_MTU,

 RTAX_WINDOW,

 RTAX_RTT,

 RTAX_RTTVAR,

 RTAX_SSTHRESH,

 RTAX_CWND,

 RTAX_ADVMSS,

 RTAX_REORDERING,

 RTAX_HOPLIMIT,

 RTAX_INITCWND,

 RTAX_FEATURES,

 RTAX_RTO_MIN,

 __RTAX_MAX
};
# 320 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/rtnetlink.h"
struct rta_session
{
 __u8 proto;
 __u8 pad1;
 __u16 pad2;

 union {
 struct {
 __u16 sport;
 __u16 dport;
 } ports;

 struct {
 __u8 type;
 __u8 code;
 __u16 ident;
 } icmpt;

 __u32 spi;
 } u;
};

struct rtgenmsg
{
 unsigned char rtgen_family;
};

struct ifinfomsg
{
 unsigned char ifi_family;
 unsigned char __ifi_pad;
 unsigned short ifi_type;
 int ifi_index;
 unsigned ifi_flags;
 unsigned ifi_change;
};

struct prefixmsg
{
 unsigned char prefix_family;
 unsigned char prefix_pad1;
 unsigned short prefix_pad2;
 int prefix_ifindex;
 unsigned char prefix_type;
 unsigned char prefix_len;
 unsigned char prefix_flags;
 unsigned char prefix_pad3;
};

enum
{
 PREFIX_UNSPEC,
 PREFIX_ADDRESS,
 PREFIX_CACHEINFO,
 __PREFIX_MAX
};



struct prefix_cacheinfo
{
 __u32 preferred_time;
 __u32 valid_time;
};

struct tcmsg
{
 unsigned char tcm_family;
 unsigned char tcm__pad1;
 unsigned short tcm__pad2;
 int tcm_ifindex;
 __u32 tcm_handle;
 __u32 tcm_parent;
 __u32 tcm_info;
};

enum
{
 TCA_UNSPEC,
 TCA_KIND,
 TCA_OPTIONS,
 TCA_STATS,
 TCA_XSTATS,
 TCA_RATE,
 TCA_FCNT,
 TCA_STATS2,
 TCA_STAB,
 __TCA_MAX
};






struct nduseroptmsg
{
 unsigned char nduseropt_family;
 unsigned char nduseropt_pad1;
 unsigned short nduseropt_opts_len;
 int nduseropt_ifindex;
 __u8 nduseropt_icmp_type;
 __u8 nduseropt_icmp_code;
 unsigned short nduseropt_pad2;
 unsigned int nduseropt_pad3;

};

enum
{
 NDUSEROPT_UNSPEC,
 NDUSEROPT_SRCADDR,
 __NDUSEROPT_MAX
};
# 457 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/rtnetlink.h"
enum rtnetlink_groups {
 RTNLGRP_NONE,

 RTNLGRP_LINK,

 RTNLGRP_NOTIFY,

 RTNLGRP_NEIGH,

 RTNLGRP_TC,

 RTNLGRP_IPV4_IFADDR,

 RTNLGRP_IPV4_MROUTE,

 RTNLGRP_IPV4_ROUTE,

 RTNLGRP_IPV4_RULE,

 RTNLGRP_IPV6_IFADDR,

 RTNLGRP_IPV6_MROUTE,

 RTNLGRP_IPV6_ROUTE,

 RTNLGRP_IPV6_IFINFO,

 RTNLGRP_DECnet_IFADDR,

 RTNLGRP_NOP2,
 RTNLGRP_DECnet_ROUTE,

 RTNLGRP_DECnet_RULE,

 RTNLGRP_NOP4,
 RTNLGRP_IPV6_PREFIX,

 RTNLGRP_IPV6_RULE,

 RTNLGRP_ND_USEROPT,

 RTNLGRP_PHONET_IFADDR,

 RTNLGRP_PHONET_ROUTE,

 __RTNLGRP_MAX
};


struct tcamsg
{
 unsigned char tca_family;
 unsigned char tca__pad1;
 unsigned short tca__pad2;
};
# 13 "diag_al.c" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/version.h" 1
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
# 14 "diag_al.c" 2
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








# 15 "diag_al.c" 2
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


# 16 "diag_al.c" 2
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
# 35 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/fcntl.h" 2


# 46 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/fcntl.h"
extern int open(const char* path, int mode, ...);
extern int openat(int fd, const char* path, int mode, ...);
extern int unlinkat(int dirfd, const char *pathname, int flags);
extern int fcntl(int fd, int command, ...);
extern int creat(const char* path, mode_t mode);


# 17 "diag_al.c" 2
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
# 18 "diag_al.c" 2
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


# 19 "diag_al.c" 2
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



# 20 "diag_al.c" 2
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
# 21 "diag_al.c" 2
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
# 22 "diag_al.c" 2
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
# 23 "diag_al.c" 2
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
# 24 "diag_al.c" 2
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
# 25 "diag_al.c" 2
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
# 26 "diag_al.c" 2
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
# 27 "diag_al.c" 2
# 1 "../../include/shm_share.h" 1
# 23 "../../include/shm_share.h"
typedef struct _ShmApiMsg
{
 int svcId;
 int procId;
 int msglen;
}ShmApiMsg;
# 28 "diag_al.c" 2
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

# 29 "diag_al.c" 2


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
# 32 "diag_al.c" 2


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
# 35 "diag_al.c" 2
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
# 36 "diag_al.c" 2


# 1 "../../linux_telephony/atcmdsrv/inc/prod_api.h" 1
# 18 "../../linux_telephony/atcmdsrv/inc/prod_api.h"
typedef void (*PROD_PROCESS_FP)(const unsigned char *, int len);

typedef struct prod_ctrl_tag{
 int fd;
 PROD_PROCESS_FP fp;
}PROD_CTRL;

void create_prod_conn_task(PROD_CTRL *pc);
# 39 "diag_al.c" 2


# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/arpa/inet.h" 1
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
# 32 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/arpa/inet.h" 2
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
# 33 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/arpa/inet.h" 2
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
# 34 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/arpa/inet.h" 2



typedef uint32_t in_addr_t;

extern uint32_t inet_addr(const char *);

extern int inet_aton(const char *, struct in_addr *);
extern char* inet_ntoa(struct in_addr);

extern int inet_pton(int, const char *, void *);
extern const char* inet_ntop(int, const void *, char *, size_t);

extern unsigned int inet_nsap_addr(const char *, unsigned char *, int);
extern char* inet_nsap_ntoa(int, const unsigned char *, char *);


# 42 "diag_al.c" 2


# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/reboot.h" 1
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
# 32 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/reboot.h" 2
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/kernel/common/linux/reboot.h" 1
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
# 33 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/../../../../..//bionic/libc/include/sys/reboot.h" 2



/* use glibc names as well */







extern int reboot(int reboot_type);
extern int __reboot(int, int, int, void *);


# 45 "diag_al.c" 2
# 74 "diag_al.c"
pthread_t diagUsbRxTask;
pthread_t diagSCRxTask;
pthread_t diagLocalRxTask;
pthread_t diagSCInit;
pthread_t diagRxTask;
pthread_t diagConnectTcpTask;

int diagNlFd;
//int diagdrvfd;

int hPort = -1;
static int serialfd;
static int mydiagstubfd = -1;
static int mydiagsockfd = -1;
static BOOL bDiagChannelInited = 0;
static BOOL bDiagDiscAcked = 0;
BOOL bDiagConnAcked = 0;
UINT32 diag_count = 0;
GlobalIfData dataExtIf;

UINT32 b_ExportedPtrStat = 0; //Enable or disable the print of the exported function pointer when invoked

extern const DiagDBVersion DBversionID;

extern const CommandAddress cat3CommandGlobalAddress[]; // only for RTOS (Nu?)
_diagIntData_S _diagInternalData;

// variables defined by pre-pass in its own lib/dll (wince, linux)
//DIAG_IMPORT const int			cat3CommandGlobalNumber;
extern const UINT32 _FirstCommandID;
extern const UINT32 _LastCommandID;
typedef struct{
 UINT32 state; //0-disconnected, 1-connected, 2-trying to connect
 UINT32 cpsr;
 OS_Mutex_t lock;
}UdpState;
UdpState udp_state = {0,0, {{0}, 0, 0, 0}};
UdpState tcp_state = {0,0, {{0}, 0, 0, 0}};
UdpState local_state = {0,0, {{0}, 0, 0, 0}};
UdpState cmi_state = {0,0, {{0}, 0, 0, 0}};
extern unsigned long m_lTavorAddress;

Diag_Event diag_event;

Diag_Target diag_target;

int diag_server_port = -1;
extern int diag_server;

struct diag_client_struct{
 struct sockaddr_un client_addr;
 UINT32 CMMver; //for backward and forward CMM compatibility
 UINT32 firstCommandID;
 UINT32 lastCommandID;
 UINT32 firstReportID;
 UINT32 lastReportID;
 int connected;
};


struct diag_client_struct diag_clients[10];






static int SerialClientIsInit = 0;
static char buffer[512];
static char buffer_out[512];
static pthread_t InputThread;
static void initUsbConnection();
static void processTCPEvent(int event_fd);
static void ReceiveDataFromDev(void);

extern int msend(int s, const void *buf, int len, int flags);
extern int mrecv(int s, void *buf, int len, int flags);
extern int msocket( int Port );
extern void connectCMI(void);
extern void connectLocal(void);
extern void connectUDP(void);
extern void connectTCP(void);
extern UINT32 SendCMIMsg(UINT8 * msg, UINT32 len, const void * clienaddr);
extern int SendLocalMsg(UINT8 * msg, UINT32 len);
extern UINT32 SendCommMsg(UINT8 *msg, UINT32 len);
extern int SendUDPMsg(UINT8 *, UINT32);
extern int SendTCPMsg(UINT8 *, UINT32);
extern OSA_STATUS InitDiagCachedWrite(void);
extern void diagExtIFStatusDiscNotify(void);
extern void diagCommL2ReceiveL1Packet (UINT8 *buff , UINT32 length , BOOL isRecursive, COMDEV_NAME rxif);
void diag_target_init(EActiveConnectionLL_Type ltype, EActiveConnectionType etype );
void diagRcvDataFromClientTask(void *data) __attribute__((noreturn));
void DiagCommExtIfLocalRxTask(void *ptr) __attribute__((noreturn));
void DiagCommExtIfTCPRxTask(void *ptr) __attribute__((noreturn));
void diagInitTask(void *data);



int is_diag_running = 0;

static void init_diag_prod_conn(void);
extern int get_bspflag_from_kernel_cmdline(void);



static void ReceiveDataATCmdIF(void )
{
 /*
	 *  check if there is response
	 */
 while (SerialClientIsInit)
 {

  int bytes = 0;
  int i, j;
  if ((bytes = read(serialfd, buffer, 512 - 1)) < 1)
  {
   //printf("Failed to receive bytes from server");
   sleep(1);
   continue;
  }
  buffer[bytes] = '\0'; /* Assure null terminated string */


  //printf("Data In length (%d).\n", bytes);

  //fprintf(stdout, buffer);
  // Send received indication to to diag
  DIAG_FILTER(VALI_IF, ATCMD_IF, ATOUT_CHAR, 0)
  diagPrintf("%s", buffer);
  for (i = 0, j = 0; i < bytes; i++)
  {
   if ( buffer[i] == 0x0d )
   {
    i++;
    if (j == 0)
    {
     // If the line is empty send at least one SPACE
     buffer_out[0] = ' ';
     buffer_out[1] = '\0';
    }
    else
    {
     buffer_out[j] = '\0';
    }
    DIAG_FILTER(VALI_IF, ATCMD_IF, ATOUT, 0)
    diagPrintf("%s", buffer_out);
    j = 0;
   }
   else
   {
    buffer_out[j++] = buffer[i];
   }
  }
  eeh_log(5, "%s", buffer_out);
 }

}
static void printRTC(void)
{
 time_t curtime;
 struct tm *curgmtime;

 if (1)
 {
  char sCalanderTime[256];
  time(&curtime);
  curgmtime = gmtime(&curtime);
  sprintf(sCalanderTime, "The RTC Value is: %d/%d/%d, %d:%d:%d",
   curgmtime->tm_mday,
   curgmtime->tm_mon + 1,
   curgmtime->tm_year + 1900,
   curgmtime->tm_hour,
   curgmtime->tm_min,
   curgmtime->tm_sec);
  eeh_log(5, "%s", sCalanderTime);
  DIAG_FILTER(Diag, Utils, Print_RTC, DIAG_INFORMATION)
  diagPrintf("%s", sCalanderTime);
  return;
 }
 else
 {
  DIAG_FILTER(Diag, Utils, ReadRTC_Failure, DIAG_INFORMATION)
  diagPrintf("Failed to read RTC");
 }
}
//ICAT EXPORTED FUNCTION - VALI_IF,ATCMD_IF,StartATCmdIF
void StartATCmdIF()
{
 pthread_attr_t tattr;

 void initCittyConnection();

 if (SerialClientIsInit)
 {
  // Already init - get out!
  DIAG_FILTER(VALI_IF, ATCMD_IF, ALREADYRUNNING, 0)
  diagPrintf("Error  -> Already Running!");
  return;
 }

 /*
	 *  Open the Device
	 */

 initCittyConnection();

 /*
	 *  Create a thread to recieve
	 */
 pthread_attr_init(&tattr);

 pthread_attr_setdetachstate(&tattr, 0x00000001);

 SerialClientIsInit = 1;

 if ( pthread_create( &InputThread, &tattr, (void *)ReceiveDataATCmdIF, ((void *)0)) != 0 )
 {

  perror("pthread_create");
  exit( 1 );
 }

}

//ICAT EXPORTED FUNCTION - VALI_IF,ATCMD_IF,StopATCmdIF
void StopATCmdIF()
{
 if (!SerialClientIsInit)
 {
  // It was not init, nothing to close - get out!
  DIAG_FILTER(VALI_IF, ATCMD_IF, STOPIFNOTSTARTED, 0)
  diagPrintf("Interface was not started jet");
  return;
 }



 SerialClientIsInit = 0;
}


//ICAT EXPORTED FUNCTION - VALI_IF,ATCMD_IF,SendATCmd
void SendATCmd(char* Cmd)
{
 unsigned char buf[514];
 int rc, i;
 int recieved;

 eeh_log(5, ">>>>>>>>>Enter SendAT CMD!!!\r\n");
 eeh_log(5, ">>>SerialClientIsInit: %d\r\n", SerialClientIsInit);
 //Check for init
 if (!SerialClientIsInit)
 {
  DIAG_FILTER(VALI_IF, ATCMD_IF, ATCMDIFNOTSTARTEDINSENDAT, 0)
  diagPrintf("Received AT Command but interface not started");
  return;
 }
 recieved = 0;
 for (i = 0; i < (int)sizeof(buf) - 1; i++)
 {
  int symb;
  symb = (int)(*(unsigned char*)Cmd);
  // Check for ASCII
  if (isprint(symb) || (symb == 0x1a)) // Check for printable and cntrl-Z
  {
   buf[i] = (*(unsigned char*)Cmd);
   Cmd++;
   recieved++;
  }
  else
  {
   break;
  }
 }
 if(recieved > 0)
 {
  if (buf[recieved - 1] == '\a' || buf[recieved - 1] == 0xa )
  {
   buf[recieved - 1] = 0xd;
  }
  else
  {
   buf[recieved++] = 0xd;
  }
  buf[recieved] = 0; //Add NULL termination to the string
  eeh_log(5, ">>>SendATCmd: %s\r\n", buf);
  rc = write(serialfd, buf, recieved);

  if (rc < 0)
  {
   eeh_log(5, "cannot send data ");
   //close(sd);
   //exit(1);

  }
 }
}
//ICAT EXPORTED FUNCTION - VALI_IF,ATCMD_IF,SendATCmdChars
void SendATCmdChars(char* Cmd)
{
 int rc;
 int recieved;

 //Check for init
 if (!SerialClientIsInit)
 {
  DIAG_FILTER(VALI_IF, ATCMD_IF, ATCMDIFNOTSTARTEDINSENDATCHAR, 0)
  diagPrintf("Received AT Command but interface not started");
  return;
 }

 recieved = strlen(Cmd);
 rc = write(serialfd, Cmd, recieved);

 if (rc < 0)
 {
  perror("cannot send data ");
  //close(sd);
  //exit(1);
 }

}

//ICAT EXPORTED FUNCTION - VALI_IF,MISC_IF,SendSystem
void SendSystem(char* Cmd)
{
 char buf[514];
 int i;
 int recieved;

 recieved = 0;
 for (i = 0; i < (int)sizeof(buf) - 1; i++)
 {
  int symb;
  symb = (int)(*(unsigned char*)Cmd);
  // Check for ASCII
  if (isprint(symb) ) // Check for printable
  {
   buf[i] = (*(unsigned char*)Cmd);
   Cmd++;
   recieved++;
  }
  else
  {
   break;
  }
 }

 buf[recieved] = 0; //Add NULL termination to the string
 DIAG_FILTER(VALI_IF, MISC_IF, ATCMDSYSTEM, 0)
 diagPrintf("Received command: %s", buf);

 eeh_log(5, ">>>SendSystem:%s\r\n", buf);


 if(strncmp(buf, "reboot", strlen("reboot")) == 0)
  reboot(0x01234567);
 else

  system(buf);

}


/************************************************************************
* Function: diagCommandServer                                          *
************************************************************************
* Description: this function is responsible to invoke the requested    *
*              Non-Response application command.                       *
*                                                                      *
* Parameters: UINT16 moduleID, UINT32 commandID - ACAT IDs to indicated *
*				commnad to activate on target.							*
*                               void *commandData - parameters of the commnad		    *
*				UINT32 dataLength - lenght of parameters				*
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
************************************************************************/
void diagCommandServer(UINT32 commandID, PackedUnPackedData *commandData, UINT32 dataLength)
{
 FunctionPtr ptrToFunction;
 UINT32 fID = /*(((UINT32)moduleID)<<16) + */ commandID; //Ignore the module ID: commandID is flat

 if (fID >= _FirstCommandID && fID <= _LastCommandID)
 //if((int)fID<cat3CommandGlobalNumber)//Fix the signed/unsigned mismatch
 {
  if (b_ExportedPtrStat)
  {
   DIAG_FILTER(DIAG, Dispatcher, ExportedPtr, DIAG_INFO)
   diagPrintf("Dispatching exported function pointer [0x%x%x]", (((unsigned int)cat3CommandGlobalAddress[fID - _FirstCommandID]) >> 16) & 0xffff, ((unsigned int)cat3CommandGlobalAddress[fID - _FirstCommandID]) & 0xffff);
   fprintf((&__sF[2]), "***** DIAG Dispatching exported function pointer [0x%x] *****\r\n", (unsigned int)cat3CommandGlobalAddress[fID - _FirstCommandID]);
  }

  ptrToFunction = (FunctionPtr)cat3CommandGlobalAddress[fID - _FirstCommandID];
  ptrToFunction(commandData, dataLength);
 }
 else
 {
  DIAG_FILTER(SW_PLAT, DIAG, COMMANDID_WRONG, DIAG_ERROR)
  diagPrintf("DIAG - command received with ID out of range: %d", fID);
 }
 return;
} /* End of diagCommandServer*/

static void openNetlink(void)
{
 struct sockaddr_nl snl;

 memset(&snl, 0, sizeof(snl));
 snl.nl_family = 16;
 snl.nl_pid = getpid();
 snl.nl_groups = 1;

 diagNlFd = socket(16, 2, 15);
 if (diagNlFd < 0)
 {
  eeh_log(5, "openNetlink socket create error.\r\n");
  return;
 }

 if (bind(diagNlFd, (struct sockaddr *)&snl, sizeof(struct sockaddr_nl)) < 0)
 {
  eeh_log(5, "openNetlink socket bind error.\r\n");
  close(diagNlFd);
 }
 return;

}
static const char *searchNetlink(const char *key, const char *buf, size_t len)
{
 size_t curlen;
 size_t keylen = strlen((char *)key);
 char *cur = (char *)buf;

 while (cur < buf + len - keylen)
 {
  curlen = strlen(cur);
  if (curlen == 0)
   break;
  if (!strncmp(key, cur, keylen) && cur[keylen] == '=')
   return cur + keylen + 1;
  cur += curlen + 1;
 }

 return ((void *)0);
}

/******************************************************************************
 *****
 *
 *      Name:			FindSoTOffset
 *
 *      Parameters:     void* pHeader -         pointer to the message
 *
 *      Returns:        the offset from message start to the SoT pattern
 *
 *      Descriptiton:	The function runs over the msg until all SoT patterns appears

 *******************************************************************************
 ****/
UINT8 FindSoTOffset(void* pHeader)
{
 UINT8 offset = 0, i = 0;
 UINT8 *pUINT8Header = (UINT8 *)pHeader;

 while (i < 10)
 {

  /* scan the received message until notifying the first byte of the SoT pattern */
  while ((*pUINT8Header != 0x6D) && (i < 9))
  {
   pUINT8Header++;
   offset++;
   i++;
  }

  pUINT8Header++;

  /* if next byte is the second byte of SoT pattern, we found the offset */
  if (*pUINT8Header == 0x6D)
  {
   break;
  }

  pUINT8Header++;
  offset++;

  i++;
 }

 if (i == 10)
 {
  return 0;
 }
 else
 {
  return offset;
 }
}

/******************************************************************************
 *****
 *
 *      Name:			GetUsbMsgLen
 *
 *      Parameters:     void* pHeader -         pointer to the message
 *
 *      Returns:        the length of the message. See more details in the
   description.
 *
 *      Descriptiton:	The length returned is the num of bytes of the whole message
   , including all its fields.
 *					Pay attention that this value is different than the total length field
   in the header!

 *******************************************************************************
 ****/
UINT32 GetUsbMsgLen (void* pHeader)
{

 //Assuming PID field in USB header is always zero
 if ((UINT32)pHeader % 4) // address of lenght (4 bytes) is not aligned. Rearrange data
 {
  UINT32 len = 0;
  UINT8 *lptr = (UINT8 *)pHeader;

  len = (UINT32)(*lptr) | (UINT32)(*(lptr + 1) << 8) | (UINT32)(*(lptr + 2) << 16) | (UINT32)(*(lptr + 3) << 24);
  //DIAG_WM_CONSOLE_PRN( (TEXT("***** DIAG GetUsbMsgLen pHeader is not 32 bits aligned!!! *****\r\n")) );
  return len;
 }
 else
 {
  return *((UINT32 *)pHeader);
 }
# 635 "diag_al.c"
}


/***********************************************************************
* Function: diagCommSetChunk                                           *
************************************************************************
* Description: Used to add all USB format messages (coming from USB or *
*              CMI) or new-header messages chunks into the recieiving  *
*              buffer.                                                 *
*                                                                      *
* Parameters:                                                          *
*		msgData       - pointer to buffer.							   *
*		lenght        - lenght of data.								   *
*       bRemoveHeader - When true - sets the header offset to 4 on the *
*                       first chunk.                                   *
*                       In distributed CMI mode the offset is zero.    *
*                       Not relevant to DIAGNEWHEADER.                 *
*                                                                      *
* Return value: 0 - if data was discarded.                             *
*               1 - when data is added to RxPacket                     *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
UINT32 diagCommSetChunk(DIAG_COM_RX_Packet_Info *RxPacket, CHAR *msgData, UINT32 length, UINT32 bRemoveHeader)
{

 UINT32 lengthOfRecMsg = 0;


 eeh_log(5, "Enter diagCommSetChunk\n ");

 RxPacket->buffer = msgData;
 RxPacket->nReTriesToPerform = 0; // diagRecieveCommand will release its buffer if failed to put-in-q
# 678 "diag_al.c"
 eeh_log(5, "lengthOfRecMsg:%d, length: %d\r\n", lengthOfRecMsg, length);

 switch (RxPacket->handling_state)
 {
 case DIAG_COM_PacketLastChunkState:
 case DIAG_COM_CompletePacketState:
  //The length of the message is only in the first chunk
  lengthOfRecMsg = GetUsbMsgLen((void*)msgData); //TBDIY potential bug - at least 4 bytes of msgData are needed for lengthOfRecMsg! (length>=4)

  //Handle case where USB driver got several messages in one buffer (WinCE, Linux) they are processed one by one
  if (length > lengthOfRecMsg)
   length = lengthOfRecMsg;
# 708 "diag_al.c"
  RxPacket->total_bytes_expected = lengthOfRecMsg;
  if (length == lengthOfRecMsg)
   RxPacket->handling_state = DIAG_COM_CompletePacketState;
  else
   RxPacket->handling_state = DIAG_COM_PacketFirstChunkState;
  RxPacket->total_bytes_received = length;
  RxPacket->data_offset = (bRemoveHeader ? 4 : 0); //TBDIY Header in USB format 2 bytes Length (includes header size) and 2 bytes PID
  break;
 case DIAG_COM_PacketFirstChunkState:
 case DIAG_COM_PacketNextChunkState:

  //Handle a case when getting in one chunk the end of the current message and data from the next message
  if (RxPacket->total_bytes_received + length >= RxPacket->total_bytes_expected)
  {
   length = RxPacket->total_bytes_expected - RxPacket->total_bytes_received;
  }

  /* accumulate the num of received bytes */
  RxPacket->total_bytes_received += length;
  if (RxPacket->total_bytes_expected == RxPacket->total_bytes_received)
   RxPacket->handling_state = DIAG_COM_PacketLastChunkState;
  else
   RxPacket->handling_state = DIAG_COM_PacketNextChunkState;
  /* in both headers there is no general header before the specific message in that scenario */
  //RxPacket->data_offset = 0;//no header on the 2nd chunk and on
  break;
 default:
  do { if (!(0)) { char buffer[512]; sprintf(buffer, "ASSERT FAIL AT FILE %s FUNC %s LINE %d", "diag_al.c", __FUNCTION__, 735); eeh_draw_panic_text(buffer, strlen(buffer), 0); } } while (0);
 }




 RxPacket->current_chunk_bytes = length;
 //RxPacket->rxIF = DIAG_COMMDEV_EXT;/*TBDIY select CMI client*/ done in DiagCommCMIfInit and DiagCommExtIfInit

 eeh_log(5, "Out diagCommSetChunk \r\n");

 return length;
}

/* The functions below are getters which return each field in the RxPDU msg */
UINT16 GetRxMsgDiagSap (UINT8 *rxMsg)
{

 RxPDU *pRxPdu = (RxPDU *)rxMsg;

 return (UINT16)pRxPdu->DiagSAP;






}
UINT16 GetRxMsgServiceID (UINT8 *rxMsg)
{

 RxPDU *pRxPdu = (RxPDU *)rxMsg;

 return pRxPdu->serviceID;






}

UINT16 GetRxMsgModuleID (UINT8 *rxMsg)
{

 RxPDU *pRxPdu = (RxPDU *)rxMsg;

 return pRxPdu->moduleID;
# 791 "diag_al.c"
}

UINT32 GetRxMsgCommID (UINT8 *rxMsg)
{

 RxPDU *pRxPdu = (RxPDU *)rxMsg;

 return pRxPdu->commID;







}

UINT32 GetRxMsgSourceID (UINT8 *rxMsg)
{
 RxPDU *pRxPdu = (RxPDU *)rxMsg;

 return pRxPdu->sourceID;
}
UINT8 *GetRxMsgFunctionParams (UINT8 *rxMsg)
{

 RxPDU *pRxPdu = (RxPDU *)rxMsg;

 return &pRxPdu->functionParams[0];






}
/************************************************************************
* Function: diagGetDBVersionID                                         *
************************************************************************
* Description: this function takes the Diag DB Version ID from the DB  *
*              and transfer it to the diagSendPDU function that sends  *
*              it to the ICAT.                                         *
*                                                                      *
* Parameters:  none                                                    *
*                                                                      *
* Return value:                                                        *
*                                                                      *
* Notes:                                                               *
************************************************************************/
static void diagGetDBVersionID (void)
{
 DiagDBVersion dbVerID = DBversionID;
 UINT32 numOfDecreasedBytes;
 int size = strlen(dbVerID) + 1;
 TxPDU *buf = (TxPDU *)(diagAlignAndHandleErr(size, 0, &numOfDecreasedBytes,

           RETURN_SERVICE



           ));

 if (!buf)
  return;

 strcpy((char *)(buf->data - numOfDecreasedBytes), (char *)(dbVerID));

 diagSendPDU(RETURN_SERVICE, 0xFFFF, 0, buf, size, numOfDecreasedBytes);



 buf = 0;

 return;

} /* End of diagGetDBVersionID   */

/************************************************************************
* Function: diagInternalServiceHandler                                 *
************************************************************************
* Description: adresses the requested service to the appropriate       *
*              handler according to opcode parameter                   *
*                                                                      *
* Parameters:  serviceID - the ID of the requested service             *
*              data - pointer to the requested service data            *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
************************************************************************/
static void diagInternalServiceHandler (UINT32 rxIF, UINT16 serviceID, PackedUnPackedData *data, UINT16 len)
{
 (void)rxIF;
 (void)data;
 (void)len;

 switch (serviceID)
 {
 case 0:
  diagGetDBVersionID();
  break;

 case 1:
  //diagTransferDiagDB();
  break;

 case 2:
  //diagGetReferenceClockRate();
  break;

 case 3:
  //diagSetFilter(data, len);
  break;

 case 4:
  //diagICATReady(rxIF);
  break;

 case 5:
  //diagGetFilter();
  break;

 case 6:
  //diagSetBootloaderMode();
  break;

 case 7:
  //diagDisconnectIcat();
  //diagDisconnectIcatInt(0);
  //ResetDiagBuffers(); // clear accumulated traces / signals, to start from clean once connection is set again.
  break;

 case 9:
  //diagSendFullFrameNumber();
  break;

 case 10:
  //diagICATReady(rxIF);
  //diagGetDBVersionID();
  break;

 default:
  DIAG_FILTER(SW_PLAT, DIAG, RX_SERVICEID_WRONG, DIAG_FATAL_ERROR)
  diagPrintf("DIAG - msg received with wrong RX service ID, the serviceID is %d", serviceID);
  break;
 } /*end of switch */

 return;

} /* End of diagInternalServiceHandler   */

/************************************************************************
* Function: diagSendData2CP                                            *
************************************************************************
* Description: this function gets the message from caller, and adds    *
*              header for it, then translates header+data to msocket   *
*                                                                      *
* Parameters:  msg   : the data to be sent to CP                       *
*              msglen: the length of data                              *
*                                                                      *
* Return value:                                                        *
*                                                                      *
* Notes:                                                               *
************************************************************************/

void diagSendData2CP(unsigned char*msg, int msglen)
{
 UINT8 *pTxbuf;
 DIAGHDR *pHeader;

 pTxbuf = malloc(msglen + sizeof(DIAGHDR));

 pHeader = (DIAGHDR*)pTxbuf;
 pHeader->packetlen = msglen;
 pHeader->seqNo = 0;
 pHeader->reserved = 0;
 memcpy(pTxbuf + sizeof(DIAGHDR), msg, msglen);
 //diagTxRawData(pTxbuf,msglen+sizeof(DIAGHDR));
 msend(mydiagsockfd, pTxbuf, msglen + sizeof(DIAGHDR), 0);

 free(pTxbuf);

}
void diagRcvDataFromClientTask(void *data)
{
 (void)data;

 UINT8 * diagDataRecvMsg, * rawPacket;
 TxPDUCMMHeader * cmmHead;
 int dwBytesTransferred = -1;
 UINT32 *pData;
 UINT16 pduLen, received;
 void diagSCInitTask(void *data);
 struct sockaddr_un clintaddr;
 int addr_len = sizeof(struct sockaddr_un);
 int i;
 diagDataRecvMsg = malloc(2048 + 6);//align?
 eeh_log(5, "*****DIAG Enter diagRcvDataFromClientTask!!!\n");
 received = 0;
 while (1)
 {
  dwBytesTransferred = recvfrom(diag_server_port, diagDataRecvMsg, 2048, 0,(struct sockaddr*)&clintaddr, (socklen_t*)&addr_len);
  if (dwBytesTransferred <= 0)
   continue;
  cmmHead = (TxPDUCMMHeader *)diagDataRecvMsg;
  if((cmmHead->DiagSAP == (CLIENT_TO_MASTER_CMM | (0? 0x80 : 0))))
  {
   switch(cmmHead->serviceID)
   {
   case 48:
   {
    eeh_log(5, "*****DIAG Recive CMM register mesage:%d!!!\n",cmmHead->serviceID);
    UINT32 * param;
    param = (UINT32 *)(diagDataRecvMsg + sizeof(TxPDUCMMHeader));
    for(i=0; i<10;i++)
    {
     if(diag_clients[i].connected)
     {
      if( memcmp((char *)&diag_clients[i].client_addr, (char *)&clintaddr, sizeof(struct sockaddr_un)) ==0)
       break;
      else
       continue;
     }
     else
     {
      memcpy((char *)&diag_clients[i].client_addr, (char *)&clintaddr, sizeof(struct sockaddr_un));
      diag_clients[i].connected = 1;
      diag_clients[i].firstCommandID = param[0];
      diag_clients[i].lastCommandID = param[1];
      diag_clients[i].firstReportID = param[2];
      diag_clients[i].lastReportID = param[3];
      break;
     }
    }
    break;
   }
   default:
    break;
   }
   continue;
  }

  rawPacket = diagDataRecvMsg;

  pData = (UINT32*)(diagDataRecvMsg);

  switch (*pData)
  {
  //case: reserved for future use.
  default:
   pduLen = dwBytesTransferred;
   if(hPort >= 0 || diag_target.hPort >=0)
   {
    if(diag_target.raw_diag_support == 1)
    {
     diag_count++;
     UINT8 * pbuff;
     pbuff = (UINT8 *)pData;
     SendCommMsg(pbuff, pduLen);
    }
   }
   break;
  }
 }
 eeh_log(5, "Exit diagRcvDataFromClientTask!!!\n");
}
//ICAT EXPORTED FUNCTION - Diag,timeprint, diagRcvDataFromCPTask
void diagRcvDataFromCPTask(void *data)
{
 (void)data;

 UINT8 * diagDataRecvMsg, * rawPacket;
 int dwBytesTransferred = -1;
 UINT32 *pData;
 DIAGHDR *pHeader;
 UINT16 pduLen, received;
 void diagSCInitTask(void *data);
 BOOL Wraped = 1;
 struct diag_target_buffer *diag_buffer;

 diag_buffer = malloc(sizeof(struct diag_target_buffer));
 if(!diag_buffer)
 {
  eeh_log(5, "***** DIAG CP Chunk Buff is NULL *****\r\n");
  do { if (!(0)) { char buffer[512]; sprintf(buffer, "ASSERT FAIL AT FILE %s FUNC %s LINE %d", "diag_al.c", __FUNCTION__, 1075); eeh_draw_panic_text(buffer, strlen(buffer), 0); } } while (0);
 }
 diag_disc_flush(diag_buffer);
 diagDataRecvMsg = malloc(2048 + 6);//align?
 if(!diagDataRecvMsg)
 {
  eeh_log(5, "***** DIAG CP RX TransferBuffer is NULL *****\r\n");
  free(diag_buffer);
  do { if (!(0)) { char buffer[512]; sprintf(buffer, "ASSERT FAIL AT FILE %s FUNC %s LINE %d", "diag_al.c", __FUNCTION__, 1083); eeh_draw_panic_text(buffer, strlen(buffer), 0); } } while (0);
 }
 eeh_log(5, "*****DIAG Enter diagRcvDataFromCPTask!!!\n");
 received = 0;
 while (1)
 {
  dwBytesTransferred = mrecv(mydiagsockfd, diagDataRecvMsg, 2048, 0);
  if (dwBytesTransferred <= 0)
   continue;

  pHeader = (DIAGHDR*)diagDataRecvMsg;

  if (pHeader->packetlen == 0)
  {
   eeh_log(5, "Meet Zero packet len !!!\n");
   continue;
  }

  rawPacket = diagDataRecvMsg + sizeof(DIAGHDR);
  pData = (UINT32*)(diagDataRecvMsg + sizeof(DIAGHDR));

  switch (*pData)
  {
  case 0x6d727632:
   bDiagChannelInited = 1;
   eeh_log(5, ">>DiagDataConfirmStartMsg\n");
   printf(">>>DiagDataConfirmStartMsg\n");
   break;
  case 0x6d727634:
   bDiagConnAcked = 1;
   eeh_log(5, ">>DiagExtIfConnectedAckMsg\n");
   printf(">>>DiagExtIfConnectedAckMsg\n");
   break;
  case 0x6d727636:
   bDiagDiscAcked = 1;
   break;
  case 0xFFFE:
   eeh_log(5, "%s: received MsocketLinkdownProcId!\n", __FUNCTION__);
   bDiagChannelInited = 0;
   bDiagConnAcked = 0;
   bDiagDiscAcked = 0;
   break;
  case 0xFFFD:
   eeh_log(5, "%s: received MsocketLinkupProcId!\n", __FUNCTION__);

   //'pthread_cancel' is not implemented in Android libpthread.
   //Fortunately, we don't need pthread_cancel here.
   //
   //if (pthread_cancel(diagSCInit) == -1)
   //{
   //	DIAGPRINT("pthread_cancel return -1\r\n");
   //}

   if( pthread_create( &diagSCInit, ((void *)0), (void *)diagInitTask, ((void *)0)) != 0 )
   {
    eeh_log(5, "pthread_create diagRcvDataFromCPTask error");
    free(diag_buffer);
    free(diagDataRecvMsg);
    return;
   }

   break;

  default:
   pduLen = dwBytesTransferred - sizeof(DIAGHDR);
   if(hPort >= 0 || diag_target.hPort >=0)
   {
    if((diag_count % 1000 == 0) && Wraped)
    {
     eeh_log(5, "diag_count:%d, read RTC\n",diag_count);
     printRTC();
    }
    {
     char * pbuff = (char *)pData;
     int ret;
     do
     {
      ret = diag_disc_rx(diag_buffer, pbuff,pduLen);
      if(ret >= 0)
      {
       SendCommMsg((UINT8 *)diag_buffer->chunk,diag_buffer->chunk_size);
       diag_disc_flush(diag_buffer);
       pbuff += (pduLen-ret);
       pduLen = ret;
       diag_count++;
      }

     }while(ret > 0);
    }
   }
   break;
  }
 }
 free(diagDataRecvMsg);
 free(diag_buffer);
 eeh_log(5, "Exit diagRcvDataFromCPTask!!!\n");
}

BOOL IsICATReadyNotify_UM(UINT8* pData)
{

 RxPDU *pRxPdu;



 if (!pData)
 {
  return 0;
 }

 pRxPdu = (RxPDU *) pData;

 return ((pRxPdu->serviceID == 4) || (pRxPdu->serviceID == 10)); /* pData[1] is start of serviceID - UINT8 field  */
# 1206 "diag_al.c"
}

void dispatchServiceForSAP(UINT8 *Ori_data)
{
 UINT16 Sap, ServiceID, dataLen;
 UINT32 MessageID;
 UINT8 *data;
 UINT8 *new_buff;
 UINT16 i;
 UINT32 ori_len, new_len;
 //UINT8 dumpCP[50];
 BOOL bNotifyCP = 0;
 ori_len = new_len = 0;
 data = Ori_data + dataExtIf.RxPacket->data_offset;

 Sap = GetRxMsgDiagSap(data);
 dataLen = dataExtIf.RxPacket->total_bytes_received-
           dataExtIf.RxPacket->data_offset - ((2 * sizeof(UINT8)) + sizeof(UINT16) + (2 * sizeof(UINT32)));

 eeh_log(5, ">>> bytes: %d, data_offset %d, SAP:%d\r\n", dataExtIf.RxPacket->total_bytes_received, dataExtIf.RxPacket->data_offset, Sap);



 eeh_log(5, ">>>dispatchServiceForSAP dataLen %d\n", dataLen);

 //sprintf(dumpCP,"Data:0-4:%2x,%2x,%2x,%2x",data[0],data[1],data[2],data[3]);
 //DIAG_FILTER(VALI_IF,ATCMD_IF,ATOUT_CHAR,0)
 //diagPrintf("%s", dumpCP);



 if (Sap & 0x80) //AP
 {
  switch (Sap & 0x7f)
  {
  case INTERNAL_SERVICE:
   ServiceID = GetRxMsgServiceID(data);
   eeh_log(5, ">>>>>Internal Service: Service %d\r\n", ServiceID);
   diagInternalServiceHandler(DIAG_COMMDEV_EXT,
                              ServiceID,
                              (PackedUnPackedData *)(GetRxMsgFunctionParams(data)),
                              dataLen);
   break;
  case COMMAND_SERVER:
   MessageID = GetRxMsgCommID(data);
   eeh_log(5, ">>>>>Command Server: CommID %d\r\n", MessageID);
   if(MessageID>= _FirstCommandID && MessageID <= _LastCommandID)
   {
    eeh_log(5, ">>>>>Command Server: It's my command, I will process!\r\n");
    diagCommandServer( MessageID,
     (PackedUnPackedData *)(GetRxMsgFunctionParams(data)),
                      dataLen);
   }
   else
   {
    eeh_log(5, ">>>>>Command Server: It's not my command, Ignore!\r\n");
    if(diag_server)
    {
     int i = 0;
     for(i =0; i<10;i++)
     {
      if(diag_clients[i].connected)
      {
       if(MessageID >= diag_clients[i].firstCommandID && MessageID <= diag_clients[i].lastCommandID)
       {
        eeh_log(5, ">>>>>Command Server: transfert to %s!\r\n",diag_clients[i].client_addr.sun_path);
        SendCMIMsg(Ori_data, dataExtIf.RxPacket->total_bytes_received,(void *)& diag_clients[i].client_addr);
       }
      }
     }
    }
   }
   break;
  case FIXUPS_COMMAND_SERVER:
   MessageID = GetRxMsgCommID(data);
   eeh_log(5, ">>>>>Fixups Command Server: CommID %d\r\n", MessageID);
   break;
  }
  //if (diagIntData.m_eConnectionType != tUSBConnection)
  {
   bNotifyCP = IsICATReadyNotify_UM(data);
  }
 }

 if (!(Sap & 0x80) || bNotifyCP) // for CP
 {
//#ifdef UART_NON_RAW_DIAG
  if (diag_target.raw_diag_support == 0)
  {
   //if (dataExtIf.RxPacket->data_offset == 0)
   {
    ori_len = dataExtIf.RxPacket->total_bytes_received- dataExtIf.RxPacket->data_offset;
    new_len = sizeof(UINT32) + ori_len; // Add length header

    new_buff = (UINT8 *)malloc(new_len);
    if (new_buff == ((void *)0))
     return;

    if ((UINT32)new_buff % 4) // address of lenght (4 bytes) is not aligned. Rearrange data
    {
     UINT8 *lptr = (UINT8 *)new_buff;

     (*lptr) = new_len & 0xff;
     *(lptr+1) = (new_len >> 8) & 0xff;
     *(lptr+2) = (new_len >> 16) & 0xff;
     *(lptr+3) = (new_len >> 24) & 0xff;
    }
    else
    {
     *((UINT32 *)new_buff) = new_len;
    }

    memcpy(new_buff+sizeof(UINT32), data, ori_len);
    for (i = 0; i < new_len; i += 1024)
    {
     if ((UINT32)(i + 1024) <= new_len)
      diagSendData2CP(new_buff + i, 1024);
     else
      diagSendData2CP(new_buff + i, new_len % 1024);
    }
    free(new_buff);
   }
  }
  else //USB
//#endif
  {
   eeh_log(5, "*****dispatchServiceForSAP sent data to CP***\n");
   new_len = dataExtIf.RxPacket->total_bytes_received;
   new_buff = Ori_data;
   for(i = 0; i < new_len; i+=1024)
   {
    if((UINT32)(i+1024) <= new_len)
     diagSendData2CP(new_buff + i, 1024);
    else
     diagSendData2CP(new_buff + i, new_len % 1024);
   }
  }
 }
# 1360 "diag_al.c"
}

int DiagIsUSBCableConnected()
{
 FILE *fp_conn = ((void *)0), *fp_comp = ((void *)0);
 char connect = 0, composite_cfg[50];

 fp_conn = fopen("/sys/devices/platform/pxa-u2o/connected", "rb");
 fp_comp = fopen("/sys/devices/platform/pxa-u2o/composite", "rb");

 if (fp_conn && fp_comp &&
     (fread(&connect,1,1,fp_conn) == 1) && (connect == ('1')) &&
     (fread(&composite_cfg, 1, 40, fp_comp) && strstr(composite_cfg, "diag")))
 {
  fclose(fp_conn);
  fclose(fp_comp);
  eeh_log(5, " ++++ Diag USB connected \n");
  return 1; //connected
 }

 if(fp_conn)
 {
  fclose(fp_conn);
  fp_conn = ((void *)0);
 }

 if(fp_comp)
 {
  fclose(fp_comp);
  fp_comp = ((void *)0);
 }

 eeh_log(5, " ---- Diag USB not connected \n");
 return 0;

}

// This fucntion is for kernel 3.0
int DiagIsInUSBComposite()
{
 FILE *fp_comp = ((void *)0);
 char composite_cfg[50];

 fp_comp = fopen("/sys/devices/virtual/android_usb/android0/functions", "rb");

 if (fp_comp &&
     (fread(&composite_cfg, 1, 40, fp_comp) && strstr(composite_cfg, "marvell_diag")))
 {
  fclose(fp_comp);
  eeh_log(5, " ++++ Diag USB connected \n");
  return 1; //connected
 }

 if(fp_comp)
 {
  fclose(fp_comp);
  fp_comp = ((void *)0);
 }

 eeh_log(5, " ---- Diag USB not connected \n");
 return 0;

}

static void ReceiveDataFromDev(void)
{
 ssize_t dwBytesTransferred = -1;
 unsigned char *transferBuffer;
 unsigned char *tmpBuffer;
 unsigned int received =0;
 unsigned int packetLen = 0;
 fd_set readfds;
 struct timeval tv;
 int fdcount;
 const char *keys;
 const char *subsys;
 const char *driver;
 const char *action;
 const char *usb_state;
 int UsbStatus = -1; // -1: invalid, 0: usb out, 1: usb in
 int DiagInComposite = 0; // 0: diag not in usb composite, 1: diag is in composite

 transferBuffer = malloc(0x1000*4*sizeof(CHAR));
 tmpBuffer = malloc(0x1000*4*sizeof(CHAR));
 while (1)
 {
  (memset (&readfds, 0, sizeof (*(fd_set *)(&readfds))));

  if (hPort >= 0)
  {
   if (diagIntData.m_eConnectionType == tUSBConnection)
   {
    /* load the buffer received before diag init */
    ioctl(hPort, (((1U) << (((0 +8)+8)+14)) | ((('T')) << (0 +8)) | (((99)) << 0) | (((((sizeof(int) == sizeof(int[1]) && sizeof(int) < (1 << 14)) ? sizeof(int) : __invalid_size_argument_for_IOC))) << ((0 +8)+8))), 0);
   }
   (((fd_set *)(&readfds))->fds_bits[(hPort) >> 5] |= (1<<((hPort) & 31)));
  }

  if (diagNlFd > 0)
  {
   (((fd_set *)(&readfds))->fds_bits[(diagNlFd) >> 5] |= (1<<((diagNlFd) & 31)));
  }

  tv.tv_sec = 1; //timeout is needed to allow FD_SET updates
  tv.tv_usec = 0;
  fdcount = select((( (hPort) > (diagNlFd) ) ? (hPort) : (diagNlFd)) + 1, &readfds, ((void *)0), ((void *)0), &tv);
  if (fdcount < 0)
  {
   eeh_log(5, "select diagNlFd error\n");
   continue;
  }
  else if (fdcount == 0)
  {
   //		printf("select diagNlFd timeout\n");
   continue;
  }
  else
  {
   if (diagNlFd > 0 && ((((fd_set *)(&readfds))->fds_bits[(diagNlFd) >> 5] & (1<<((diagNlFd) & 31))) != 0))
   {
    eeh_log(5, "got Netlink Data\n");
    dwBytesTransferred = recv(diagNlFd, transferBuffer, 0x400, 0);
    if (dwBytesTransferred <= 0)
     continue;
    /* search udev netlink keys from env */
    keys = (char *)(transferBuffer + strlen((char *)transferBuffer) + 1);
    subsys = searchNetlink("SUBSYSTEM", keys, dwBytesTransferred);
    driver = searchNetlink("DRIVER", keys, dwBytesTransferred);
    action = searchNetlink("ACTION", keys, dwBytesTransferred);
    usb_state = searchNetlink("USB_STATE", keys, dwBytesTransferred);

    UsbStatus = -1;
    DiagInComposite = 0;

    if (subsys && driver && action) // for kernel .35 or before
    {
     if ((!strcmp(subsys, "platform")) &&
      !(strcmp(driver, "pxa27x-udc") /* Tavor P USB 1.1 */
       && strcmp(driver, "pxa3xx-u2d") /* Tavor P USB 2.0 */
       && strcmp(driver, "pxa-u2o")
       && strcmp(driver, "pxa9xx-u2o"))) /* Tavor PV USB 2.0 */
     {
      if (!strcmp(action, "add"))
      {
       UsbStatus = 1;
       DiagInComposite = DiagIsUSBCableConnected();
      }
      else if (!strcmp(action, "remove"))
      {
       UsbStatus = 0;
      }
     }
     else
     {
      UsbStatus = -1;
     }
    } else if (subsys && usb_state && action) // for kernel 3.0
    {
     if ((!strcmp(subsys, "android_usb")) &&
      !(strcmp(action, "change")))
     {
      if (!strcmp(usb_state, "CONFIGURED"))
      {
       UsbStatus = 1;
       DiagInComposite = DiagIsInUSBComposite();
      }
      else if (!strcmp(usb_state, "DISCONNECTED"))
      {
       UsbStatus = 0;
      }
     }
     else
     {
      UsbStatus = -1;
     }

    }
    else
    {
     /* wrong event */
     continue;
    }

    /* plug in or out? */
    if (UsbStatus == 1)
    {
     eeh_log(5, "DIAG RX Netlink add \r\n");
     /* FIXME: detect /dev/ttygserial to avoid endless loop */
     if (access("/dev/ttydiag0", 0) < 0) //existence test
     {
      eeh_log(5, "DIAG over FS: TTY device error\r\n");
     }

     if (hPort > 0)
     {
      ioctl(hPort, (((1U) << (((0 +8)+8)+14)) | (((249)) << (0 +8)) | (((2)) << 0) | (((((sizeof(int) == sizeof(int[1]) && sizeof(int) < (1 << 14)) ? sizeof(int) : __invalid_size_argument_for_IOC))) << ((0 +8)+8))), 0);
      close(hPort);
      hPort = -1;
     }

     if (DiagInComposite)
     {
      mydiagstubfd = open("/dev/diagdatastub",00000002);
      if (mydiagstubfd < 0)
      {
       eeh_log(5, ">>%s,Id:%d\n","/dev/diagdatastub",mydiagstubfd);
       return;
      }
      if (mydiagstubfd > 0)
       ioctl(mydiagstubfd, (((1U) << (((0 +8)+8)+14)) | (((249)) << (0 +8)) | (((6)) << 0) | (((((sizeof(int) == sizeof(int[1]) && sizeof(int) < (1 << 14)) ? sizeof(int) : __invalid_size_argument_for_IOC))) << ((0 +8)+8))), 0);

      close(mydiagstubfd);

      initUsbConnection();
      if (hPort > 0)
      {
       ioctl(hPort, (((1U) << (((0 +8)+8)+14)) | (((249)) << (0 +8)) | (((1)) << 0) | (((((sizeof(int) == sizeof(int[1]) && sizeof(int) < (1 << 14)) ? sizeof(int) : __invalid_size_argument_for_IOC))) << ((0 +8)+8))), 0);
      }
     }
     else
     {
      eeh_log(5, "Diag is not in USB composite, Do Nothing\r\n");
     }
    }
    else if (UsbStatus == 0)
    {
     eeh_log(5, "DIAG RX Netlink remove\r\n");
     if (hPort > 0)
      ioctl(hPort, (((1U) << (((0 +8)+8)+14)) | (((249)) << (0 +8)) | (((2)) << 0) | (((((sizeof(int) == sizeof(int[1]) && sizeof(int) < (1 << 14)) ? sizeof(int) : __invalid_size_argument_for_IOC))) << ((0 +8)+8))), 0);
     close(hPort);
     hPort = -1;
    }

   }
   /* is usb gadget serial data? */
   if (hPort > 0 && ((((fd_set *)(&readfds))->fds_bits[(hPort) >> 5] & (1<<((hPort) & 31))) != 0))
   {
    if ((dwBytesTransferred = read(hPort, transferBuffer + received, 0x1000 )) >= 0)
    {
     if ((*__errno()) == 5)
     {
      // when do USB dynamic switch, diag tty port could be closed by kernel
      eeh_log(5, "Close diag port when %s\n",strerror((*__errno())));
      ioctl(hPort, (((1U) << (((0 +8)+8)+14)) | (((249)) << (0 +8)) | (((2)) << 0) | (((((sizeof(int) == sizeof(int[1]) && sizeof(int) < (1 << 14)) ? sizeof(int) : __invalid_size_argument_for_IOC))) << ((0 +8)+8))), 0);
      close(hPort);
      hPort = -1;
     }

    eeh_log(5, "DIAG received from hPort:%x , received:%d  len:%d\n", hPort, received, dwBytesTransferred);

GetNewData: if(packetLen ==0) // need to get length first!
     {
      if(dwBytesTransferred+received < sizeof(UINT32)) //not enough for read a UINT32!
      {
       received += dwBytesTransferred;
       continue;
      }
      else //enough to get a Length!
      {
       packetLen = GetUsbMsgLen(transferBuffer);
       goto GetNewData; // continue to process this new data!
      }
     }
     else
     {
       received += dwBytesTransferred;

       if(received >= packetLen) // We received next packet!
       {
        memcpy(tmpBuffer, transferBuffer+packetLen, received - packetLen);
        diagCommSetChunk(dataExtIf.RxPacket, (CHAR *)transferBuffer, (UINT32)packetLen, 1/*TBDIY DIAGNEWHEADER*/);
        dispatchServiceForSAP(transferBuffer);

        memcpy(transferBuffer, tmpBuffer, received - packetLen); // just like we get a new data!
        dwBytesTransferred = received - packetLen;
        received = 0;
        packetLen = 0;

        goto GetNewData; // continue to process this new data!
       }
       else
        continue;
     }
    }
    else
    {
     //Handle this error
     eeh_log(5, "***** DIAG EXT COMM RX ReadFile failed:%d,%s. *****\r\n", dwBytesTransferred,strerror((*__errno())));
     sleep(3); //Sleep some time to allow the usb-disconnection event to arrive properly
    }
   }
   else
   {
    eeh_log(5, "****GO TO this wrong path** port:%d*\r\n", hPort);
   }
  }
 }

}

void initUsbConnection()
{
 struct termios tio, oldtio;

 hPort = open("/dev/ttydiag0", 00000002 | 00000400 | 00004000);
 if (hPort == -1)
 {
  eeh_log(5, "failed to init USB connection\n");
  return;
 }

 fcntl(hPort, 4, 0 /*FASYNC*/);

 tcgetattr(hPort, &oldtio); // save current port settings
 eeh_log(5, ">>>hPort C_iflag %x\r\n", oldtio.c_iflag);

 memset(&tio, 0x0, sizeof(struct termios));
 /* Configure termios */
 tio.c_cflag = 0010002 | 020000000000 | 0000060 | 0004000 | 0000200;
 tio.c_iflag = 0000004;
 tio.c_oflag = 0;
 tio.c_lflag = 0;
 tio.c_cc[0] = 0; /* Ctrl-c */
 tio.c_cc[1] = 0; /* Ctrl-\ */
 tio.c_cc[3] = 0; /* @ */
 tio.c_cc[4] = 0; //4;     /* Ctrl-d */
 tio.c_cc[5] = 0; /* inter-character timer unused */
 tio.c_cc[6] = 1; /* blocking read until 1 character arrives */
 tio.c_cc[7] = 0; /* '\0' */
 tio.c_cc[8] = 0; /* Ctrl-q */
 tio.c_cc[9] = 0; /* Ctrl-s */
 tio.c_cc[10] = 0; /* Ctrl-z */
 tio.c_cc[12] = 0; /* Ctrl-r */
 tio.c_cc[13] = 0; /* Ctrl-u */
 tio.c_cc[14] = 0; /* Ctrl-w */
 tio.c_cc[15] = 0; /* Ctrl-v */
 tio.c_cc[16] = 0; /* '\0' */
 tio.c_cc[2] = 0x8; /* del */
 tio.c_cc[11] = 0xD; /* '\0' */

 tcflush(hPort, 0);
 tcsetattr(hPort, 0x5402, &tio);

 eeh_log(5, "DIAG: Initialized USB connection %s.\r\n", "/dev/ttydiag0");
}
void initCittyConnection()
{
 struct termios oldtio, newtio; //place for old and new port settings for serial port
 memset(&newtio, 0x0, sizeof(newtio));
 /*open the device(com port) to be non-blocking (read will return immediately) */
 serialfd = open("/dev/citty8", 00000002); //| O_NOCTTY

 if (serialfd <= 0)
 {
  printf("Error opening device %s\n", "/dev/citty8");
  return;
 }

 // Make the file descriptor asynchronous (the manual page says only
 // O_APPEND and O_NONBLOCK, will work with F_SETFL...)

 fcntl(serialfd, 4, 0);

 tcgetattr(serialfd, &oldtio); // save current port settings

 // set new port settings for canonical input processing
 newtio.c_cflag = 0010002 | 020000000000 | 0000060 | 0 | 0 | 0 | 0004000 | 0000200;
 newtio.c_iflag = 0000004;
 newtio.c_oflag = 0;
 ///newtio.c_lflag =  ECHOE | ECHO | ICANON;       //ICANON;
 newtio.c_lflag = 0; //ICANON;

 newtio.c_cc[6] = 1;
 newtio.c_cc[5] = 0;
 newtio.c_cc[2] = 0x8;
 newtio.c_cc[11] = 0xD;

 newtio.c_cc[0] = 0; /* Ctrl-c */
 newtio.c_cc[1] = 0; /* Ctrl-\ */
 newtio.c_cc[2] = 0; /* del */
 newtio.c_cc[3] = 0; /* @ */
 newtio.c_cc[4] = 4; /* Ctrl-d */
 newtio.c_cc[5] = 0; /* inter-character timer unused */
 newtio.c_cc[6] = 1; /* blocking read until 1 character arrives */
 newtio.c_cc[7] = 0; /* '\0' */
 newtio.c_cc[8] = 0; /* Ctrl-q */
 newtio.c_cc[9] = 0; /* Ctrl-s */
 newtio.c_cc[10] = 0; /* Ctrl-z */
 newtio.c_cc[11] = 0; /* '\0' */
 newtio.c_cc[12] = 0; /* Ctrl-r */
 newtio.c_cc[13] = 0; /* Ctrl-u */
 newtio.c_cc[14] = 0; /* Ctrl-w */
 newtio.c_cc[15] = 0; /* Ctrl-v */
 newtio.c_cc[16] = 0; /* '\0' */

 newtio.c_cc[6] = 1;
 newtio.c_cc[5] = 0;
 newtio.c_cc[2] = 0x8;
 newtio.c_cc[11] = 0xD;



 tcflush(serialfd, 0);
 tcsetattr(serialfd, 0x5402, &newtio);

}
void DiagCommExtIfInit(void)
{
 extern OSSemaRef send_comm_msg_sem;
 extern OSSemaRef connect_fs_sem;
 OSA_STATUS status;
 eeh_log(5, ">>>enter DiagCommExtIfInit\n");
 dataExtIf.RxPacket = malloc(sizeof(DIAG_COM_RX_Packet_Info));

 dataExtIf.RxPacket->handling_state = DIAG_COM_CompletePacketState;
 dataExtIf.RxPacket->current_chunk_bytes = 0;
 dataExtIf.RxPacket->data_offset = 0; //init
 dataExtIf.RxPacket->end_of_packet = 0;
 dataExtIf.RxPacket->total_bytes_expected = 0;
 dataExtIf.RxPacket->total_bytes_received = 0;
 dataExtIf.RxPacket->rxIF = DIAG_COMMDEV_EXT;
 dataExtIf.bIsIfBusy = 0;

 // RESTRICT MESSAGES LENGHT.
 _diagInternalData.MsgLimits.diagMaxMsgOutLimit = 7400; //8000;
 _diagInternalData.MsgLimits.diagMaxMsgOutWarning = 7200; //5000;
 _diagInternalData.MsgLimits.diagMaxMsgInLimit = 9400; //12000;
 _diagInternalData.MsgLimits.diagMaxMsgInWarning = 9200; //5000;

 //Init DIAG configuration
 diagOSspecificPhase1Init();
 eeh_log(5, ">>>diagOSspecificPhase1Init Finished!\n");

 eeh_log(5, ">>>OSASemaphoreCreate: %x, sem :%d !\n", OSASemaphoreCreate, send_comm_msg_sem);

 status = OSASemaphoreCreate(&send_comm_msg_sem, 1, 11);

 if ( status != OS_SUCCESS )
 {
  eeh_log(5, "DiagCommExtIfInit - can't create semaphore!\n");
 }
 status = OSASemaphoreCreate(&connect_fs_sem, 1, 11);
 if ( status != OS_SUCCESS )
 {
  eeh_log(5, "DiagCommExtIfInit - can't create semaphore!\n");
 }

}

void diagUsbConnectNotify()
{
 UINT32 dataId = 0x6d727633;
 int retrycunt = 3;

 //bDiagHostUsbConn = TRUE;
 bDiagDiscAcked = 0;
 /* If we have alreadly received Conn Ack, we can ignore it */
 if (bDiagConnAcked)
  return;

 while (retrycunt > 0)
 {
  if (!bDiagConnAcked)
  {
   diagSendData2CP((unsigned char *)&dataId, sizeof(UINT32));
   retrycunt--;
   sleep(1);
  }
  else
  {
   break;
  }
 }
}

void diagUsbDisConnectNotify()
{
 UINT32 dataId = 0x6d727635;
 int retrycunt = 3;
 bDiagConnAcked = 0;
 if(bDiagDiscAcked)
  return;
 while(retrycunt)
 {
  if (!bDiagDiscAcked)
  {
   diagSendData2CP((unsigned char *)&dataId, sizeof(UINT32));
   retrycunt--;
   sleep(1);
  }
  else
  {
   break;
  }
 }
}

void diagSCInitTask(void *data)
{
 (void)data;

 UINT32 startId = 0x6d727631;

 while (1)
 {
  if (!bDiagChannelInited)
  {
   diagSendData2CP((unsigned char *)&startId, sizeof(UINT32));
   eeh_log(5, ">>>diagSCInitTask , DiagDataRequestStartMsg\n");
   usleep(100*1000);
  }
  else
  {
   //diagUsbConnectNotifyForSD();
   //DIAGPRINT(">>>diagSCInitTask , diagUsbConnectNotifyForSD\n");
   break;
  }

 }
}

void diagInitTask(void *data)
{
 (void)data;

 UINT32 startId = 0x6d727631;

 while (1)
 {
  if (!bDiagChannelInited)
  {
   diagSendData2CP((unsigned char *)&startId, sizeof(UINT32));
   eeh_log(5, ">>>diagInitTask , DiagDataRequestStartMsg\n");
   usleep(100*1000);
  }
  else
  {
   diagUsbConnectNotify();
   eeh_log(5, ">>>diagInitTask , diagUsbConnectNotify\n");
   break;
  }

 }
}

/***********************************************************************
* Function: checkIP                                                    *
************************************************************************
* Description:                                                         *
* Parameters:                                                          *
*                                                                      *
* Return value: 0=valid IP, -1=invalid IP                              *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
static int checkIP(unsigned long ip)
{
 int fd, num, ret, i;
 struct ifconf ifc;
 struct ifreq *ifr;

 ret = -1;

 fd = socket(2, 2, 0);
 if(fd < 0)
  return -1;

 num = 10;
 ifc.ifc_ifcu.ifcu_buf = ((void *)0);
 for (;;) {
  ifc.ifc_len = num * (int)sizeof(struct ifreq);
  ifc.ifc_ifcu.ifcu_buf = realloc(ifc.ifc_ifcu.ifcu_buf, ifc.ifc_len);

  if (ioctl(fd, 0x8912, &ifc) < 0) {
   perror("SIOCGIFCONF");
   goto out;
  }
  if (ifc.ifc_len == num * (int)sizeof(struct ifreq)) {
   /* assume it overflowed and try again */
   num += 10;
   continue;
  }
  break;
 }

 num = ifc.ifc_len / sizeof(struct ifreq);

 for (i = 0, ifr = ifc.ifc_ifcu.ifcu_req; i < num; i++, ifr++) {
  if(ip == ((struct sockaddr_in *)&(ifr->ifr_ifru.ifru_addr))->sin_addr.s_addr) {
   ret = 0;
   break;
  }
 }

out:
 free(ifc.ifc_ifcu.ifcu_buf);
 close(fd);
 return ret;
}

void DiagCommExtIfLocalRxTask(void *ptr)
{
 CHAR* TransferBuffer;
 long dwBytesTransferred=0;
 UINT32 socError=0;
 unsigned char *tmpBuffer;
 unsigned int received =0;
 unsigned int packetLen = 0;

 fd_set readfds;
 (void)ptr;

 eeh_log(5, "***** DIAG DiagCommExtIfUDPRxTask m_bIsClient=%d _FirstCommandID==%d *****\r\n",diagIntData.m_bIsClient,_FirstCommandID);
 TransferBuffer = malloc(0x1000*4*sizeof(CHAR)); //it's *4 also in:
 tmpBuffer = malloc(0x1000*4*sizeof(CHAR));

 if (!TransferBuffer)
 {
  //handle this error
  eeh_log(5, "***** DIAG UDP RX TransferBuffer is NULL. *****\r\n");
  do { if (!(0)) { char buffer[512]; sprintf(buffer, "ASSERT FAIL AT FILE %s FUNC %s LINE %d", "diag_al.c", __FUNCTION__, 1980); eeh_draw_panic_text(buffer, strlen(buffer), 0); } } while (0);
 }
 while(1)
 {
  int iSocketLastError,n;
  (memset (&readfds, 0, sizeof (*(fd_set *)(&readfds))));
  (((fd_set *)(&readfds))->fds_bits[(diag_target.hPort) >> 5] |= (1<<((diag_target.hPort) & 31)));

  if(diag_event.event_socket >0)
   (((fd_set *)(&readfds))->fds_bits[(diag_event.event_socket) >> 5] |= (1<<((diag_event.event_socket) & 31)));

  eeh_log(5, "***** Waiting for local data coming in :%d, %d!!!\n",diag_target.hPort, diag_event.event_socket);
  n = select( (( (diag_target.hPort) > (diag_event.event_socket) ) ? (diag_target.hPort) : (diag_event.event_socket))+1, &readfds, ((void *)0), ((void *)0), ((void *)0));
  eeh_log(5, "***** Waited for local data coming in :%d, %d!!!\n",diag_target.hPort, diag_event.event_socket);
  if((diag_target.hPort != -1)&&((((fd_set *)(&readfds))->fds_bits[(diag_target.hPort) >> 5] & (1<<((diag_target.hPort) & 31))) != 0))
  {
   dwBytesTransferred = recvfrom(diag_target.hPort, (char *)TransferBuffer+received, 0x1000, 0, ((void *)0), ((void *)0));
   if(dwBytesTransferred <= 0)
   {
    if (dwBytesTransferred < 0)
    {
     eeh_log(5, "***** DIAG EXT recvfrom error dwBytesTransferred < 0 *****\r\n");
     socError=1;
     sleep(1);
     continue;
    }
   }
   else
   {
    socError=0;
    eeh_log(5, "testtestsststsstststst %d\n",dwBytesTransferred);

    if (diag_target.raw_diag_support == 0)
    {
     diagCommL2ReceiveL1Packet((UINT8 *)TransferBuffer, (UINT32)dwBytesTransferred, 0,DIAG_COMMDEV_EXT);
    } else //TBDIY tricky! DiagCommExtIfUDPRxTask expects USB format for CMI clients

    {
GetNewData: if(packetLen ==0) // need to get length first!
     {
      if(dwBytesTransferred+received < sizeof(UINT32)) //not enough for read a UINT32!
      {
       received += dwBytesTransferred;
       continue;
      }
      else //enough to get a Length!
      {
       packetLen = GetUsbMsgLen(TransferBuffer);
       goto GetNewData; // continue to process this new data!
      }
     }
     else
     {
       received += dwBytesTransferred;

       if(received >= packetLen) // We received next packet!
       {
        memcpy(tmpBuffer, TransferBuffer+packetLen, received - packetLen);
        diagCommSetChunk(dataExtIf.RxPacket, TransferBuffer, (UINT32)packetLen, 1/*TBDIY DIAGNEWHEADER*/);
        dispatchServiceForSAP((UINT8 *)TransferBuffer);

        memcpy(TransferBuffer, tmpBuffer, received - packetLen); // just like we get a new data!
        dwBytesTransferred = received - packetLen;
        received = 0;
        packetLen = 0;

        goto GetNewData; // continue to process this new data!
       }
       else
        continue;
     }
    }
   }
  }
  else if(diag_target.hPort == -1)
  {
   eeh_log(5, "***** DIAG EXT recvfrom m_EXTsock==-1 *****\r\n");
   sleep(1);
   //break;
  }
  //////////////////////////////////////////////
  // Dynamic IP change
  //////////////////////////////////////////////
  if(diag_event.event_socket != -1 && ((((fd_set *)(&readfds))->fds_bits[(diag_event.event_socket) >> 5] & (1<<((diag_event.event_socket) & 31))) != 0))
  {
   eeh_log(5, "*****DIAG EXT get diag event!\n");// reserved for future use.
  }
  if(n==-1)
  {
   iSocketLastError = (*__errno());
   eeh_log(5, "***** DIAG EXT select failed iSocketLastError = %d *****\r\n",iSocketLastError);
   eeh_log(5, "Error: %s\n",strerror(iSocketLastError));
   socError=1;
   continue;
  }
 }
}
/***********************************************************************
* Function: DiagCommExtIfUDPRxTask                                     *
************************************************************************
* Description: Task for UDP DIAG messages recieve                      *
*                                                                      *
* Parameters:                                                          *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
void DiagCommExtIfUDPRxTask(void *ptr)
{
 CHAR* TransferBuffer;
 long dwBytesTransferred=0;
 UINT32 socError=0;
 unsigned char *tmpBuffer;
 unsigned int received =0;
 unsigned int packetLen = 0;

 fd_set readfds;
 (void)ptr;

 eeh_log(5, "***** DIAG DiagCommExtIfUDPRxTask m_bIsClient=%d _FirstCommandID==%d *****\r\n",diagIntData.m_bIsClient,_FirstCommandID);
 TransferBuffer = malloc(0x1000*4*sizeof(CHAR)); //it's *4 also in:
 if (!TransferBuffer)
 {
  //handle this error
  eeh_log(5, "***** DIAG UDP RX TransferBuffer is NULL. *****\r\n");
  do { if (!(0)) { char buffer[512]; sprintf(buffer, "ASSERT FAIL AT FILE %s FUNC %s LINE %d", "diag_al.c", __FUNCTION__, 2106); eeh_draw_panic_text(buffer, strlen(buffer), 0); } } while (0);
 }
 tmpBuffer = malloc(0x1000*4*sizeof(CHAR));
 if (!tmpBuffer)
 {
  //handle this error
  eeh_log(5, "***** DIAG UDP RX TransferBuffer is NULL. *****\r\n");
  free(TransferBuffer);
  do { if (!(0)) { char buffer[512]; sprintf(buffer, "ASSERT FAIL AT FILE %s FUNC %s LINE %d", "diag_al.c", __FUNCTION__, 2114); eeh_draw_panic_text(buffer, strlen(buffer), 0); } } while (0);
 }
 while(1)
 {
  int iSocketLastError,n;
  (memset (&readfds, 0, sizeof (*(fd_set *)(&readfds))));
  (((fd_set *)(&readfds))->fds_bits[(diag_target.hPort) >> 5] |= (1<<((diag_target.hPort) & 31)));

  if(diag_event.event_socket >0)
   (((fd_set *)(&readfds))->fds_bits[(diag_event.event_socket) >> 5] |= (1<<((diag_event.event_socket) & 31)));

  eeh_log(5, "***** Waiting for UDP data coming in :%d, %d!!!\n",diag_target.hPort, diag_event.event_socket);
  n = select( (( (diag_target.hPort) > (diag_event.event_socket) ) ? (diag_target.hPort) : (diag_event.event_socket))+1, &readfds, ((void *)0), ((void *)0), ((void *)0));
  eeh_log(5, "***** Waited for UDP data coming in :%d, %d!!!\n",diag_target.hPort, diag_event.event_socket);
  if((diag_target.hPort != -1)&&((((fd_set *)(&readfds))->fds_bits[(diag_target.hPort) >> 5] & (1<<((diag_target.hPort) & 31))) != 0))
  {
   dwBytesTransferred = recvfrom(diag_target.hPort, (char *)TransferBuffer+received, 0x1000, 0, ((void *)0), ((void *)0));
   if(dwBytesTransferred <= 0)
   {
    if (dwBytesTransferred < 0)
    {
     eeh_log(5, "***** DIAG EXT recvfrom error dwBytesTransferred < 0 *****\r\n");
     socError=1;
     sleep(1);
     continue;
    }
   }
   else
   {
    socError=0;
    eeh_log(5, "testtestsststsstststst %d\n",dwBytesTransferred);

    if (diag_target.raw_diag_support == 0)
    {
     diagCommL2ReceiveL1Packet((UINT8 *)TransferBuffer, (UINT32)dwBytesTransferred, 0,DIAG_COMMDEV_EXT);
    } else //TBDIY tricky! DiagCommExtIfUDPRxTask expects USB format for CMI clients

    {
GetNewData: if(packetLen ==0) // need to get length first!
     {
      if(dwBytesTransferred+received < sizeof(UINT32)) //not enough for read a UINT32!
      {
       received += dwBytesTransferred;
       continue;
      }
      else //enough to get a Length!
      {
       packetLen = GetUsbMsgLen(TransferBuffer);
       goto GetNewData; // continue to process this new data!
      }
     }
     else
     {
       received += dwBytesTransferred;

       if(received >= packetLen) // We received next packet!
       {
        memcpy(tmpBuffer, TransferBuffer+packetLen, received - packetLen);
        diagCommSetChunk(dataExtIf.RxPacket, TransferBuffer, (UINT32)packetLen, 1/*TBDIY DIAGNEWHEADER*/);
        dispatchServiceForSAP((UINT8 *)TransferBuffer);

        memcpy(TransferBuffer, tmpBuffer, received - packetLen); // just like we get a new data!
        dwBytesTransferred = received - packetLen;
        received = 0;
        packetLen = 0;

        goto GetNewData; // continue to process this new data!
       }
       else
        continue;
     }
    }
   }
  }
  else if(diag_target.hPort == -1)
  {
   eeh_log(5, "***** DIAG EXT recvfrom m_EXTsock==-1 *****\r\n");
   sleep(1);
   //break;
  }
  //////////////////////////////////////////////
  // Dynamic IP change
  //////////////////////////////////////////////
  if(diag_event.event_socket != -1 && ((((fd_set *)(&readfds))->fds_bits[(diag_event.event_socket) >> 5] & (1<<((diag_event.event_socket) & 31))) != 0))
  {
   if((diagIntData.m_bIsClient==0)&&(checkIP(m_lTavorAddress)<0))
   {
    eeh_log(5, "***** DIAG Dynamic IP address change detected *****\r\n");
    diagExtIFStatusDiscNotify();
    break;
   }
  }
  if(n==-1)
  {
   iSocketLastError = (*__errno());
   eeh_log(5, "***** DIAG EXT select failed iSocketLastError = %d *****\r\n",iSocketLastError);
   eeh_log(5, "Error: %s\n",strerror(iSocketLastError));
   socError=1;
   continue;
  }
 }
 free(TransferBuffer);
 free(tmpBuffer);
}

void DiagCommExtIfTCPRxTask(void *ptr)
{
 CHAR* TransferBuffer;
 long dwBytesTransferred=0;
 UINT32 socError=0;
 unsigned char *tmpBuffer;
 unsigned int received =0;
 unsigned int packetLen = 0;

 fd_set readfds;
 (void)ptr;
 TransferBuffer = malloc(0x1000*4*sizeof(CHAR)); //it's *4 also in:
 if (!TransferBuffer)
 {
  //handle this error
  eeh_log(5, "***** DIAG TCP RX TransferBuffer is NULL. *****\r\n");
  do { if (!(0)) { char buffer[512]; sprintf(buffer, "ASSERT FAIL AT FILE %s FUNC %s LINE %d", "diag_al.c", __FUNCTION__, 2235); eeh_draw_panic_text(buffer, strlen(buffer), 0); } } while (0);
 }
 tmpBuffer = malloc(0x1000*4*sizeof(CHAR));
 if(!tmpBuffer)
 {
  //handle this error
  eeh_log(5, "***** DIAG TCP RX TransferBuffer is NULL. *****\r\n");
  free(TransferBuffer);
  do { if (!(0)) { char buffer[512]; sprintf(buffer, "ASSERT FAIL AT FILE %s FUNC %s LINE %d", "diag_al.c", __FUNCTION__, 2243); eeh_draw_panic_text(buffer, strlen(buffer), 0); } } while (0);
 }
 while(1)
 {
  int iSocketLastError,n;
  (memset (&readfds, 0, sizeof (*(fd_set *)(&readfds))));
  if(diag_target.hPort >0)
   (((fd_set *)(&readfds))->fds_bits[(diag_target.hPort) >> 5] |= (1<<((diag_target.hPort) & 31)));

  if(diag_event.event_socket >0)
   (((fd_set *)(&readfds))->fds_bits[(diag_event.event_socket) >> 5] |= (1<<((diag_event.event_socket) & 31)));

  if (diag_event.pipefd[0] >0)
  {//Very tricky!!! Clients are always in UDP mode
   (((fd_set *)(&readfds))->fds_bits[(diag_event.pipefd[0]) >> 5] |= (1<<((diag_event.pipefd[0]) & 31)));
  }

  eeh_log(5, "***** Waiting for TCP data coming in :hPort:%d, event_socket:%d, event_pipe:%d!!!\n",diag_target.hPort, diag_event.event_socket, diag_event.pipefd[0]);
  n = select( (( ((( (diag_target.hPort) > (diag_event.event_socket) ) ? (diag_target.hPort) : (diag_event.event_socket))) > (diag_event.pipefd[0]) ) ? ((( (diag_target.hPort) > (diag_event.event_socket) ) ? (diag_target.hPort) : (diag_event.event_socket))) : (diag_event.pipefd[0]))+1, &readfds, ((void *)0), ((void *)0), ((void *)0));
  eeh_log(5, "***** Waited for TCP data coming in :hPort:%d, event_socket:%d, event_pipe:%d!!!\n",diag_target.hPort, diag_event.event_socket, diag_event.pipefd[0]);

  if( diag_event.pipefd[0] >0 && ((((fd_set *)(&readfds))->fds_bits[(diag_event.pipefd[0]) >> 5] & (1<<((diag_event.pipefd[0]) & 31))) != 0))
  {
   char buf[10];
   eeh_log(5, "******TCP RxTask !!!!");
   read(diag_event.pipefd[0], buf, 10);
   eeh_log(5, "******TCP RxTask:%s\n", buf);
  }
  if((diag_target.hPort != -1)&&((((fd_set *)(&readfds))->fds_bits[(diag_target.hPort) >> 5] & (1<<((diag_target.hPort) & 31))) != 0))
  {
   dwBytesTransferred = recv(diag_target.hPort, (char *)TransferBuffer+received, 0x1000, 0);
   if(dwBytesTransferred <= 0)
   {
    if (dwBytesTransferred < 0)
    {
     eeh_log(5, "***** DIAG EXT recvfrom error dwBytesTransferred < 0 *****\r\n");
     socError=1;
     sleep(1);
     continue;
    }
    else
    {
     eeh_log(5, "*****DIAG EXT Connection is closed by peer:%d\r\n",dwBytesTransferred);
     diagExtIFStatusDiscNotify();
     if (diag_target.hPort < 0)
     {
      if( pthread_create(&diagConnectTcpTask, ((void *)0), (void *)connectTCP, ((void *)0)) != 0)
      {
       eeh_log(5, "pthread_create diagConnectTcpTask error\n");
      }
     }
     continue;
    }
   }
   else
   {

    if (diag_target.raw_diag_support == 0)
    {
     diagCommL2ReceiveL1Packet((UINT8 *)TransferBuffer, (UINT32)dwBytesTransferred, 0,DIAG_COMMDEV_EXT);
    } else //TBDIY tricky! DiagCommExtIfUDPRxTask expects USB format for CMI clients

    {
GetNewData: if(packetLen ==0) // need to get length first!
     {
      if(dwBytesTransferred+received < sizeof(UINT32)) //not enough for read a UINT32!
      {
       received += dwBytesTransferred;
       continue;
      }
      else //enough to get a Length!
      {
       packetLen = GetUsbMsgLen(TransferBuffer);
       goto GetNewData; // continue to process this new data!
      }
     }
     else
     {
       received += dwBytesTransferred;

       if(received >= packetLen) // We received next packet!
       {
        memcpy(tmpBuffer, TransferBuffer+packetLen, received - packetLen);
        diagCommSetChunk(dataExtIf.RxPacket, TransferBuffer, (UINT32)packetLen, 1/*TBDIY DIAGNEWHEADER*/);
        dispatchServiceForSAP((UINT8 *)TransferBuffer);

        memcpy(TransferBuffer, tmpBuffer, received - packetLen); // just like we get a new data!
        dwBytesTransferred = received - packetLen;
        received = 0;
        packetLen = 0;

        goto GetNewData; // continue to process this new data!
       }
       else
        continue;
     }
    }
   }
  }
  else if(diag_target.hPort == -1)
  {
   eeh_log(5, "***** DIAG EXT recv target.hPort==-1 *****\r\n");
   sleep(1);
  }
  //////////////////////////////////////////////
  // Dynamic IP change
  //////////////////////////////////////////////
  if(diag_event.event_socket != -1 && ((((fd_set *)(&readfds))->fds_bits[(diag_event.event_socket) >> 5] & (1<<((diag_event.event_socket) & 31))) != 0))
  {
   processTCPEvent(diag_event.event_socket);
  }
  if(n==-1)
  {
   iSocketLastError = (*__errno());
   eeh_log(5, "***** DIAG EXT select failed iSocketLastError = %d *****\r\n",iSocketLastError);
   eeh_log(5, "Error: %s\n",strerror(iSocketLastError));
   socError=1;
   continue;
  }
 }

}

int ifc_init(void);
void ifc_close(void);
int ifc_up(const char *name);
int ifc_set_addr(const char *name, unsigned addr);
int ifc_set_mask(const char *name, unsigned mask);

void ConfigEthernetInterface(void)
{
 // ifconfig usb0 192.168.1.101 netmask 255.255.255.0 up
 const char *ifname = "usb0";
 // const char *ipaddr = "192.168.1.101";
 const char *maskaddr = "255.255.255.0";

 in_addr_t addr, netmask;

 addr = m_lTavorAddress; //inet_addr(ipaddr);
 netmask = inet_addr(maskaddr);

 ifc_init();
 if(ifc_up(ifname))
 {
  eeh_log(5, "failed to turn on interface");
  ifc_close();
  return ;
 }
 if(ifc_set_addr(ifname, addr))
 {
  eeh_log(5, "failed to set ipaddr");
  ifc_close();
  return ;
 }

 if(ifc_set_mask(ifname, netmask))
 {
  eeh_log(5, "failed to set netmask");
  ifc_close();
  return ;
 }

 ifc_close();
}

int diag_init(void)
{


 extern BOOL InitLog2SDcard(void);
 extern void connectCOMM(void);
 eeh_log(5, ">>>enter diag_init\n");

 DiagCommExtIfInit();
 eeh_log(5, ">>>DiagCommExtIfInit Finished\n");

 eeh_log(5, "*****DIAG m_eLLtype :%d, m_eConnectionType:%d diag_server : %d\n", diagIntData.m_eLLtype, diagIntData.m_eConnectionType,diag_server);
 diag_target_init(diagIntData.m_eLLtype, diagIntData.m_eConnectionType);
 if(diag_server)
 {
  connectCMI();
  eeh_log(5, "*****DIAG over Local: master connected!\n");
  if( pthread_create( &diagLocalRxTask, ((void *)0), (void *)diagRcvDataFromClientTask, ((void *)0)) != 0 )
  {
   eeh_log(5, "pthread_create diagRcvDataFromClientTask error");
   return -1;
  }

  if(get_bspflag_from_kernel_cmdline() == 2)
   init_diag_prod_conn();


 }
 if (diagIntData.m_eLLtype == tLL_FS || diagIntData.m_eLLtype == tLL_SC)
 {
  //Log to SD card

  mydiagsockfd = msocket(4);

  if ( pthread_create( &diagSCRxTask, ((void *)0), (void *)diagRcvDataFromCPTask, ((void *)0)) != 0 )
  {
   eeh_log(5, "pthread_create diagRcvDataFromCPTask error");
   return -1;
  }
  if ( pthread_create( &diagSCInit, ((void *)0), (void *)diagSCInitTask, ((void *)0)) != 0 )
  {
   eeh_log(5, "pthread_create diagRcvDataFromCPTask error");
   return -1;
  }
# 2478 "diag_al.c"
  InitLog2SDcard();
 }
 else //Log to ACAT via USB/UART
 {
  if (diagIntData.m_eConnectionType == tUARTConnection)
  { //UART

   //Make sure cached size is set to 0 under UART log mode
   diagIntData.diagCachedWrite.write_unit_size = 0;


   if(get_bspflag_from_kernel_cmdline() != 2)

   connectCOMM();


   mydiagsockfd = msocket(4);

   if( pthread_create( &diagSCRxTask, ((void *)0), (void *)diagRcvDataFromCPTask, ((void *)0)) != 0 )
   {
    eeh_log(5, "pthread_create diagRcvDataFromCPTask error");
    return -1;
   }
   if( pthread_create( &diagSCInit, ((void *)0), (void *)diagSCInitTask, ((void *)0)) != 0 )
   {
    eeh_log(5, "pthread_create diagRcvDataFromCPTask error");
    return -1;
   }

   if(get_bspflag_from_kernel_cmdline() != 2)

   diagUsbConnectNotify();
# 2536 "diag_al.c"
   if ( pthread_create( &diagUsbRxTask, ((void *)0), (void *)ReceiveDataFromDev, ((void *)0)) != 0 )
   {
    eeh_log(5, "pthread_create diagUsbRxTask error");
    return -1;
   }

  }
  else if(diagIntData.m_eConnectionType == tUDPConnection || diagIntData.m_eConnectionType == tTCPConnection || diagIntData.m_eConnectionType == tLocalConnection)
  {
   eeh_log(5, "*****DIAG:start connect to udp\n");

   if(diagIntData.m_eLLtype == tLL_ETHERNET)
    ConfigEthernetInterface();

   if (diagIntData.m_eLLtype == tLL_ETHERNET || diagIntData.m_eLLtype == tLL_LocalIP)
   {
    int wait_timeout = 5;

    InitDiagCachedWrite();
    if(diag_server)
    {
     mydiagsockfd = msocket(4);
     if( pthread_create( &diagSCRxTask, ((void *)0), (void *)diagRcvDataFromCPTask, ((void *)0)) != 0 )
     {
      eeh_log(5, "pthread_create diagRcvDataFromCPTask error");
      return -1;
     }
     if( pthread_create( &diagSCInit, ((void *)0), (void *)diagSCInitTask, ((void *)0)) != 0 )
     {
      eeh_log(5, "pthread_create diagRcvDataFromCPTask error");
      return -1;
     }

     while((!bDiagChannelInited)&&(wait_timeout))
     {
      sleep(1);
      wait_timeout--;
     }
    }

    eeh_log(5, ">>>diag:diag target:%x; diag_event:%x, diag connect:%x, diag send:%x \n", &diag_target, &diag_event, diag_target.connect, diag_target.send_target);
    if(diag_target.connect != ((void *)0))
     diag_target.connect();
    eeh_log(5, "*****DIAG over Net: target connected!\n");

    if(diag_event.open != ((void *)0))
     diag_event.open();
    eeh_log(5, "*****DIAG over Net: event opened!\n");
    if ( pthread_create( &diagRxTask, ((void *)0), (void *)diag_target.read_target_task, ((void *)0)) != 0 )
    {
     eeh_log(5, "pthread_create DiagCommExtIfUDPRxTask error");
     return -1;
    }
   }
  }
  else // USB default
  {
   //Make sure cached size is set to 0 under USB log mode
   diagIntData.diagCachedWrite.write_unit_size = 0;
   mydiagsockfd = msocket(4);
   if(mydiagsockfd < 0)
   {
    eeh_log(5, "Open Diag Port error\n!");
    return -1;
   }

   mydiagstubfd = open("/dev/diagdatastub",00000002);

   if (mydiagstubfd < 0)
   {

    eeh_log(5, ">>%s,Id:%d\n","/dev/diagdatastub",mydiagstubfd);
    return -1;
   }
   if (mydiagstubfd > 0)
    ioctl(mydiagstubfd, (((1U) << (((0 +8)+8)+14)) | (((249)) << (0 +8)) | (((5)) << 0) | (((((sizeof(int) == sizeof(int[1]) && sizeof(int) < (1 << 14)) ? sizeof(int) : __invalid_size_argument_for_IOC))) << ((0 +8)+8))), 0);

   close(mydiagstubfd);

   //initUsbConnection();

   openNetlink();
   if ( pthread_create( &diagUsbRxTask, ((void *)0), (void *)ReceiveDataFromDev, ((void *)0)) != 0 )
   {
    eeh_log(5, "pthread_create diagUsbRxTask error");
    return -1;
   }
  }
 }
 if(diag_server == 0)
 {
  eeh_log(5, "*****DIAG: client notcie master!\n");
  //DIAG_FILTER(DIAG, CMI, DIAG_TEST, DIAG_CONNECT)
  //diagTextPrintf("Client Notice master!!");
 }
 return 0;
}

void diag_sig_action(int signum, siginfo_t * info, void *p)
{
 (void)info;
 (void)p;
 eeh_log(5, ">>diag:receive sig:%d\n", signum);
 switch(signum)
 {
 case 15:
   diagExtIFStatusDiscNotify();
  break;
 case 13:
  diagExtIFStatusDiscNotify();
  eeh_log(5, "Reconnecting TCP\n");
  if( pthread_create(&diagConnectTcpTask, ((void *)0), (void *)connectTCP, ((void *)0)) != 0)
  {
   eeh_log(5, "pthread_create diagConnectTcpTask error\n");
  }
  break;
 default:
  break;
 }
}

void openRTMGRP_IPV4_IFADDR()
{
 int fd;
 struct sockaddr_nl snl;

 eeh_log(5, "***** DIAG open RTMGRP_IPV4_IFADDR *****\r\n");
 memset(&snl, 0, sizeof(snl));
 snl.nl_family = 16;
 snl.nl_groups = 0x10;

 fd = socket(16, 3, 0);
 if(fd < 0)
 {
  eeh_log(5, "***** DIAG openRTMGRP_IPV4_IFADDR: socket NETLINK_ROUTE Error: %s *****\r\n",strerror((*__errno())));
  return;
 }

 if(bind(fd, (struct sockaddr *) &snl, sizeof(struct sockaddr_nl)) < 0)
 {
  eeh_log(5, "***** DIAG openRTMGRP_IPV4_IFADDR: bind NETLINK_ROUTE Error: %s *****\r\n",strerror((*__errno())));
  close(fd);
  return;
 }

 diag_event.event_socket = fd;
}
static int openNetEventlink(void)
{
 struct sockaddr_nl snl;
 int fd = -1;
 memset(&snl, 0, sizeof(snl));
 snl.nl_family = 16;
 snl.nl_pid = getpid();
 snl.nl_groups = 1;

 fd = socket(16, 2, 15);
 if (fd < 0)
 {
  eeh_log(5, "openNetEventlink socket create error.\r\n");
  return fd;
 }

 if (bind(fd, (struct sockaddr *)&snl, sizeof(struct sockaddr_nl)) < 0)
 {
  eeh_log(5, "openNetlink socket bind error.\r\n");
  close(fd);
 }

 diag_event.event_socket = fd;
 return fd;

}

int DiagIsUSBEthernetConnected(void)
{
 FILE *fp_conn = ((void *)0), *fp_comp = ((void *)0);
 char connect = 0, composite_cfg[50];

 fp_conn = fopen("/sys/devices/platform/pxa-u2o/connected", "rb");
 fp_comp = fopen("/sys/devices/platform/pxa-u2o/composite", "rb");

 if (fp_conn && fp_comp &&
     (fread(&connect,1,1,fp_conn) == 1) && (connect == ('1')) &&
     (fread(&composite_cfg, 1, 40, fp_comp) && strstr(composite_cfg, "rndis")))
 {
  fclose(fp_conn);
  fclose(fp_comp);
  eeh_log(5, " ++++ Diag USB ethernet connected \n");
  return 1; //connected
 }

 if(fp_conn)
 {
  fclose(fp_conn);
  fp_conn = ((void *)0);
 }

 if(fp_comp)
 {
  fclose(fp_comp);
  fp_comp = ((void *)0);
 }

 eeh_log(5, " ---- Diag USB ethernet not connected \n");
 return 0;

}

// for kernel 3.0
int RndisisInUSBComposite(void)
{
 FILE *fp_comp = ((void *)0);
 char composite_cfg[50];

 fp_comp = fopen("/sys/devices/virtual/android_usb/android0/functions", "rb");

 if (fp_comp &&
     (fread(&composite_cfg, 1, 40, fp_comp) && strstr(composite_cfg, "rndis")))
 {
  fclose(fp_comp);
  eeh_log(5, " ++++ Diag USB ethernet connected \n");
  return 1; //connected
 }

 if(fp_comp)
 {
  fclose(fp_comp);
  fp_comp = ((void *)0);
 }

 eeh_log(5, " ---- Diag USB ethernet not connected \n");
 return 0;

}

static void processTCPEvent(int event_fd)
{
 CHAR transferBuffer[2048];
 long dwBytesTransferred=0;
 const char *keys;
 const char *subsys;
 const char *driver;
 const char *action;
 const char *usb_state;
 int UsbStatus = -1; // -1: invalid, 0: usb out, 1: usb in
 int RndisInComposite = 0; // 0: rndis not in usb composite, 1: rndis is in composite

 dwBytesTransferred = recv(event_fd, transferBuffer, 2048, 0);
 if (dwBytesTransferred <= 0)
  return;

 /* search udev netlink keys from env */
 keys = (char *)(transferBuffer + strlen((char *)transferBuffer) + 1);
 eeh_log(5, "got Netlink Data:%s\n",keys);
 subsys = searchNetlink("SUBSYSTEM", keys, dwBytesTransferred);
 driver = searchNetlink("DRIVER", keys, dwBytesTransferred);
 action = searchNetlink("ACTION", keys, dwBytesTransferred);
 usb_state = searchNetlink("USB_STATE", keys, dwBytesTransferred);

 UsbStatus = -1;
 RndisInComposite = 0;

 if (subsys && driver && action) // for kernel .35 or before
 {
  if ((!strcmp(subsys, "platform")) &&
   !(strcmp(driver, "pxa27x-udc") /* Tavor P USB 1.1 */
    && strcmp(driver, "pxa3xx-u2d") /* Tavor P USB 2.0 */
    && strcmp(driver, "pxa-u2o")
    && strcmp(driver, "pxa9xx-u2o"))) /* Tavor PV USB 2.0 */
  {
   if (!strcmp(action, "add"))
   {
    UsbStatus = 1;
    RndisInComposite = DiagIsUSBEthernetConnected();
   }
   else if (!strcmp(action, "remove"))
   {
    UsbStatus = 0;
   }
  }
  else
  {
   UsbStatus = -1;
  }
 } else if (subsys && usb_state && action) // for kernel 3.0
 {
  if ((!strcmp(subsys, "android_usb")) &&
   !(strcmp(action, "change")))
  {
   if (!strcmp(usb_state, "CONFIGURED"))
   {
    UsbStatus = 1;
    RndisInComposite = RndisisInUSBComposite();
   }
   else if (!strcmp(usb_state, "DISCONNECTED"))
   {
    UsbStatus = 0;
   }
  }
  else
  {
   UsbStatus = -1;
  }
 }
 else
 {
  /* wrong event */
  eeh_log(5, "DIAG over TCP: Wrong event\r\n");
 }


 /* plug in or out? */
 if (UsbStatus == 1)
 {
  eeh_log(5, "DIAG RX Netlink add \r\n");
  /* FIXME: detect /dev/ttygserial to avoid endless loop */
  if (access("/sys/class/net/usb0", 0) < 0) //existence test
  {
   eeh_log(5, "DIAG over TCP: USB0 device error\r\n");
  }

  if (RndisInComposite && diag_target.hPort < 0 && diag_target.socket < 0)
  {
   if( pthread_create(&diagConnectTcpTask, ((void *)0), (void *)connectTCP, ((void *)0)) != 0)
   {
    eeh_log(5, "pthread_create diagConnectTcpTask error\n");
   }
  }
 }
 else if (UsbStatus == 0)
 {
  eeh_log(5, "DIAG RX Netlink remove\r\n");
  diagExtIFStatusDiscNotify();
 }
 return;
}
/***********************************************************************
* Function: diag_target_init                                    *
************************************************************************
* Description: init target struct accordint to each target. All the targets(ACAT over USB, ACAT ovet UDP,
   SDcard and so on) may use unify interface in the future.                       *
*                                                                      *
* Parameters:                                                          *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
void diag_target_init(EActiveConnectionLL_Type ltype, EActiveConnectionType etype )
{
 int i;
 eeh_log(5, ">>>diag_target_int:diag target:%x; diag_event:%x, diag connect:%x, diag send:%x \n", &diag_target, &diag_event, connectUDP, SendUDPMsg);
 diag_target.hPort = -1; //hport: the file description or socket of the target.
 diag_target.socket = -1;
 diag_event.event_socket = -1; //event source port.
 diag_target.raw_diag_support = 1;
 diag_server_port = -1;
 if(pipe(diag_event.pipefd) < 0)
 {
  eeh_log(5, "create pipe error\n");
  return;
 }

 eeh_log(5, ">>>>diag server:%d\n", diag_server);
 if(diag_server)
 {
  OSA_MutexCreate(&cmi_state.lock,((void *)0));
  for(i=0;i<10;i++)
  {
   memset(&diag_clients[i], 0, sizeof(diag_clients[i]));
  }
 }
 switch(ltype)
 {
 case tLL_SC:
 case tLL_FS:
 //	diag_target.connect = InitLog2SDcard;
 //	diag_target.send_target = write;
 //	diag_target.read_target_task = NULL;
 //	diag_event.open = openNetlink;
  diagIntData.m_bIsClient = 0;
  break;
 case tLL_ETHERNET:
  switch(etype)
  {
  case tUDPConnection:
   diagIntData.m_bIsClient = 0;
   diag_target.connect = connectUDP; //connect:the fucntion to connect to the target, get the avalible hPort usually.
   diag_target.read_target_task = DiagCommExtIfUDPRxTask; //read_target_task: the fuction to read data form target and send it to CP
   diag_target.send_target = SendUDPMsg; //send_target: the function to send data to target.
   diag_target.raw_diag_support = diagIntData.m_bIsRawDiag;
   diag_event.open = ((void *)0);
    OSA_MutexCreate(&udp_state.lock,((void *)0));
   //agIntData.diagCachedWrite.write_unit_size = 1024;
   break;
  case tTCPConnection:
   diag_target.connect = connectTCP;
   diag_target.read_target_task = DiagCommExtIfTCPRxTask;
   diag_target.send_target = SendTCPMsg;
   diag_target.raw_diag_support = diagIntData.m_bIsRawDiag;
   diag_event.open = openNetEventlink;
    OSA_MutexCreate(&tcp_state.lock,((void *)0));
   //diagIntData.diagCachedWrite.write_unit_size = 1024;
  default :
   break;
  }
  break;
 case tLL_LocalIP:
  diagIntData.m_bIsClient = 1;
  diag_target.connect = connectLocal;
  diag_target.read_target_task = DiagCommExtIfLocalRxTask;
  diag_target.send_target = SendLocalMsg;
  diag_target.raw_diag_support = 1;
  diag_event.open = ((void *)0);
  OSA_MutexCreate(&local_state.lock, ((void *)0));
 default:
  break;
 }
 eeh_log(5, ">>>diag_target_int:diag target:%x; diag_event:%x, diag connect:%x, diag send:%x \n", &diag_target, &diag_event, diag_target.connect, diag_target.send_target);
}


static PROD_CTRL diag_prod_ctrl;

static void process_diag_buf(const unsigned char *buf, int len)
{
 unsigned long code;
 extern void connectCOMM(void);

 if(len != 4)
 {
  return;
 }

 memcpy(&code, buf, sizeof(code));
 if(code == 0x00000002UL)
 {
  if(is_diag_running == 0)
  {
   eeh_log(5, "%s: open diag", __FUNCTION__);

   connectCOMM();
   diagUsbConnectNotify();
  }
  is_diag_running = 1;
 }
}

static void init_diag_prod_conn(void)
{
 diag_prod_ctrl.fd = -1;
 diag_prod_ctrl.fp = process_diag_buf;

 create_prod_conn_task(&diag_prod_ctrl);
}

//ICAT EXPORTED FUNCTION - SYSTEM,PROD,AT_SER
void AT_SER(void)
{
 extern void disconnectCOMM();
 unsigned long code = 0x00000001UL;

 DIAG_FILTER(SYSTEM, PROD, AT_SER_OK, DIAG_INFO)
 diagPrintf("OK");

 eeh_log(5, "%s: close diag", __FUNCTION__);

 if(get_bspflag_from_kernel_cmdline() != 2)
  return;

 // stop diag
 is_diag_running = 0;
 disconnectCOMM();

 // send message to start at command server
 if(diag_prod_ctrl.fd != -1)
  write(diag_prod_ctrl.fd, &code, sizeof(code));
}
