# 1 "src/eeh_api.c"
# 1 "/home/yanl/marvell.sp6/vendor/marvell/generic/ttc_telephony/drivers/errorhandler//"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "src/eeh_api.c"
/******************************************************************************
*(C) Copyright 2008 Marvell International Ltd.
* All Rights Reserved
******************************************************************************/
/*--------------------------------------------------------------------------------------------------------------------
 *  -------------------------------------------------------------------------------------------------------------------
 *
 *  Filename: eeh_api.c
 *
 *  Description: The APIs to process error & exception of system.
 *
 *  History:
 *   April, 2008 - Rovin Yu Creation of file
 *
 *  Notes:
 *
 ******************************************************************************/

/******************************************************************************
*    Include files
******************************************************************************/

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


# 24 "src/eeh_api.c" 2
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



# 25 "src/eeh_api.c" 2
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
# 164 "/home/yanl/marvell.sp6/kernel/../prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/../lib/gcc/arm-eabi/4.4.3/include/stddef.h" 3 4
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
# 26 "src/eeh_api.c" 2
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








# 27 "src/eeh_api.c" 2
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
# 28 "src/eeh_api.c" 2
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


# 29 "src/eeh_api.c" 2
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
# 30 "src/eeh_api.c" 2
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


# 31 "src/eeh_api.c" 2
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
# 32 "src/eeh_api.c" 2
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




# 33 "src/eeh_api.c" 2
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
# 34 "src/eeh_api.c" 2
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


# 35 "src/eeh_api.c" 2

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


# 37 "src/eeh_api.c" 2


# 1 "../osa/inc/osa.h" 1
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
# 63 "../osa/inc/osa.h"
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
# 64 "../osa/inc/osa.h" 2
# 1 "../osa/inc/osa_config.h" 1
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



# 1 "../include/gbl_config.h" 1
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
# 89 "../include/gbl_config.h"
/*                      end of gbl_config.h
			--------------------------------------------------------------------------- */
# 178 "../osa/inc/osa_config.h" 2

/* Check the Parameter Ranges */
# 215 "../osa/inc/osa_config.h"
/*                      end of osa_config.h
   --------------------------------------------------------------------------- */
# 65 "../osa/inc/osa.h" 2

/*****************************************************************************
 * OSA Constants
 ****************************************************************************/
# 98 "../osa/inc/osa.h"
/* Remain for backwards compatibility */
# 128 "../osa/inc/osa.h"
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

 char *queueName, /* name of message queue                   */

 UINT32 maxSize, /* max message size the queue supports     */
 UINT32 maxNumber, /* max # of messages in the queue          */
 UINT8 waitingMode /* mode of tasks waiting OS_FIFO, OS_PRIORITY */
 );

 extern OSA_STATUS OSAMsgQCreateWithMem(
 OSMsgQRef * msgQRef, /* OS message queue reference              */

 char *queueName, /* name of message queue                   */

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

 char *queueName, /* name of mailbox queue                   */

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
# 545 "../osa/inc/osa.h"
/*****************************************************************************
 * OSA Internal Assert macro definition
 ****************************************************************************/
# 559 "../osa/inc/osa.h"
/* Tracing is disabled so no need for extra trace information */
void OSA_AssertFail(char *file, unsigned line); /* Assert Fail function */
# 586 "../osa/inc/osa.h"
/* Remap old function names to new ones to remain backwards compatibility with
 * old function calls.
 */
# 40 "src/eeh_api.c" 2
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
# 41 "src/eeh_api.c" 2
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
# 59 "../include/gbl_types.h"
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
# 42 "src/eeh_api.c" 2
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
# 43 "src/eeh_api.c" 2
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
# 44 "src/eeh_api.c" 2
# 1 "./inc/eeh.h" 1
/******************************************************************************
*(C) Copyright 2008 Marvell International Ltd.
* All Rights Reserved
******************************************************************************/
/*--------------------------------------------------------------------------------------------------------------------
 *  -------------------------------------------------------------------------------------------------------------------
 *
 *  Filename: eeh.h
 *
 *  Description: API interface for error handler
 *
 *  History:
 *   April 10, 2008 - Rovin Yu Creation of file
 *
 *  Notes:
 *
 ******************************************************************************/





# 1 "./inc/eeh_ioctl.h" 1
/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.

 *(C) Copyright 2007 Marvell International Ltd.
 * All Rights Reserved
 */




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
# 14 "./inc/eeh_ioctl.h" 2

/* The retun value of the EEH API finction calls. */
typedef UINT32 EEH_STATUS;

/* Error code */
# 33 "./inc/eeh_ioctl.h"
typedef enum _EehApiId
{
 _EehInit = 0,
 _EehDeInit,
 _EehInsertComm2Reset,
 _EehReleaseCommFromReset,
 _EehCopyCommImageFromFlash,
 _EehDisableCPUFreq,
 _EehEnableCPUFreq
}EehApiId;

typedef struct _EehApiParams
{
 int eehApiId;
 UINT32 status;
 void *params;
} EehApiParams;

typedef enum _EehMsgInd
{
 EEH_INVALID_MSG = 0,
 EEH_WDT_INT_MSG,
 EEH_AP_ASSERT_MSG,
 EEH_CP_SILENT_RESET_MSG,
}EehMsgInd;
typedef enum _EehAssertType
{
 EEH_AP_ASSERT =0,
 EEH_CP_EXCEPTION =1,
 EEH_AP_EXCEPTION=2,
 EEH_CP_ASSERT,
 EEH_NONE_ASSERT,
}EehAssertType;

typedef struct _EehInsertComm2ResetParam
{
 UINT32 AssertType;
}EehInsertComm2ResetParam;

typedef struct _EehMsgStruct
{
 UINT32 msgId;
 char msgDesc[512]; //for save AP assert description
}EehMsgStruct;

typedef struct _EehAppAssertParam
{
 char msgDesc[512];
}EehAppAssertParam;

typedef struct _EehCpSilentResetParam
{
 char msgDesc[512];
}EehCpSilentResetParam;

/* Communicate the error info to SEH (for RAMDUMP and m.b. more) */
typedef struct _EehErrorInfo
{
 unsigned err;
 char *str;
 unsigned char *regs;
} EehErrorInfo;

/* err: SEH converts these into what defined in kernel ramdump_defs.h file */
# 24 "./inc/eeh.h" 2





EEH_STATUS EehInit(void);
EEH_STATUS EehDeinit(void);
void EehStartHBTest(void);






extern int gDumpHugeMemory;
extern int b_launch_diag;
# 45 "src/eeh_api.c" 2
# 1 "./inc/eeh_assert_notify.h" 1
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * (C) Copyright 2008 Marvell International Ltd.
 * All Rights Reserved
 */


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
# 12 "./inc/eeh_assert_notify.h" 2

extern FILE *logfile;
extern int loglevel;





extern int eeh_draw_panic_text(const char *panic_text, size_t panic_len, int do_timer);
extern int cp_silent_reset_on_req(const char *reset_info);
# 46 "src/eeh_api.c" 2
# 1 "../shmem/wrapper/msocket_api.h" 1
# 11 "../shmem/wrapper/msocket_api.h"
/* Allow the use in C++ code.  */




extern int msocket(int port);
extern int mclose(int sock);
extern int msend(int sock, const void *buf, int len, int flags);
extern int mrecv(int sock, void *buf, int len, int flags);
extern int msocket_linkdown(void);
extern int msocket_linkup(void);
# 47 "src/eeh_api.c" 2
# 1 "../include/NVMServer_api.h" 1



# 1 "../include/gbl_types.h" 1
/* ===========================================================================
   File        : gbl_types.h
   Description : Global types file.

   Notes       : This file includes the types from the correct environment.
	      The environment is set via the ENV_<ENV> macro. This macro
	      is usually set in /env/<host>/build/<env>_env.mak.

   Copyright 2001, Intel Corporation, All rights reserved.
   =========================================================================== */
# 59 "../include/gbl_types.h"
/*                         end of gbl_types.h
   --------------------------------------------------------------------------- */
# 5 "../include/NVMServer_api.h" 2
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
# 6 "../include/NVMServer_api.h" 2
# 1 "../include/nvm_shared.h" 1
// (C) Copyright 2006 Marvell International Ltd.
// All Rights Reserved
//
/******************************************************************************
**
** INTEL CONFIDENTIAL
** Copyright 2000-2005 Intel Corporation All Rights Reserved.
**
** The source code contained or described herein and all documents
** related to the source code (Material) are owned by Intel Corporation
** or its suppliers or licensors.  Title to the Material remains with
** Intel Corporation or its suppliers and licensors. The Material contains
** trade secrets and proprietary and confidential information of Intel
** or its suppliers and licensors. The Material is protected by worldwide
** copyright and trade secret laws and treaty provisions. No part of the
** Material may be used, copied, reproduced, modified, published, uploaded,
** posted, transmitted, distributed, or disclosed in any way without Intel?s
** prior express written permission.
**
** No license under any patent, copyright, trade secret or other intellectual
** property right is granted to or conferred upon you by disclosure or
** delivery of the Materials, either expressly, by implication, inducement,
** estoppel or otherwise. Any license under such intellectual property rights
** must be express and approved by Intel in writing.
**
*****************************************************************************/



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
# 31 "../include/nvm_shared.h" 2
# 1 "../include/shm_share.h" 1
# 23 "../include/shm_share.h"
typedef struct _ShmApiMsg
{
 int svcId;
 int procId;
 int msglen;
}ShmApiMsg;
# 32 "../include/nvm_shared.h" 2

/*
   File Name : nvm_shared.h
   Desc	  : Shared file of NVM Server-Client SW Components, define Protocal Messages
   Both SW Entities should use this file
 */

/*YG - 1) change DEF to enum with base
 *      2) add REMOVE FAILED
 *      3) better to minimize error list (why need winerror?)
 *      4) error handler - not handling properly when no diag - no report (maybe need to loop)
 * */

/* the following 2 files are temporarly exist in the crossplatform\nvmclient\inc the it should use the original files that found under softutil\softutil*/
# 54 "../include/nvm_shared.h"
//#define NVMSRV_PORT 2
# 65 "../include/nvm_shared.h"
typedef enum
{
 NVM_STATUS_SUCCESS = 0x10000, //force to be 4 Bytes wide
 NVM_STATUS_READ_EOF,
 NVM_STATUS_FAILURE,
 NVM_STATUS_API_FAIL_GETDRIVERNAME,
 NVM_STATUS_API_FAIL_OPENDRIVER,
 NVM_STATUS_API_FAIL_IOCTL,
 NVM_STATUS_NON_TRUSTED_CLIENT,
 NVM_STATUS_NO_RESOURCES,
 NVM_STATUS_FAIL_READ_CLIENT_DATA, //client registry data could not be found
 NVM_STATUS_TB_SIZE_MISMATCH_ERR, //size field of translation table does not match nimber of records in table
 NVM_STATUS_TB_ENTRY_ERR, //translation table entry error
 NVM_STATUS_DP_ENTRY_ERR, //default path entry error
 NVM_STATUS_UNSUPPORTED_CLIENT_POLICY, // for example : no TTB and only read-only path exists. against system definition
 NVM_STATUS_OPEN_FAILED_MAPPING, //try to open a file, but mapping to phy path failed.
 NVM_STATUS_OPEN_FILE_NOT_FOUND, //
 NVM_STSTUS_CLIENT_NOT_FOUND,
 NVM_STATUS_INVALID_FILE_HANDLE,
 NVM_STATUS_CLOSE_FILE_FAILED,
 NVM_STATUS_SEEK_FAILED,
 NVM_STATUS_READ_FAILED,
 NVM_STATUS_WRITE_FAILED,
 NVM_STATUS_WRITE_FAILED_READ_ONLY,
 NVM_STATUS_FLUSH_FAILED,
 //NVM_STATUS_GET_FILE_SIZE_FAILED,
 NVM_STATUS_FILE_FIND_FIRST_FAILED,
 NVM_STATUS_FILE_FIND_NEXT_FAILED,
 ////File System Error //////
 ///Those are mapped to file errors in winerror.h
 NVM_STATUS_FS_ERROR,
 NVM_STATUS_FS_INVALID_FUNCTION, //ERROR_INVALID_FUNCTION
 NVM_STATUS_FS_FILE_NOT_FOUND, // ERROR_FILE_NOT_FOUND
 NVM_STATUS_FS_PATH_NOT_FOUND, //ERROR_PATH_NOT_FOUND
 NVM_STATUS_FS_TOO_MANY_OPEN_FILES, //ERROR_TOO_MANY_OPEN_FILES
 NVM_STATUS_FS_ACCESS_DENIED, //ERROR_ACCESS_DENIED
 NVM_STATUS_FS_HANDLE, //ERROR_INVALID_HANDLE
 NVM_STATUS_UNKNOWN,
 NVM_STATUS_ERROR_COUNT

}NVM_STATUS_T;

typedef enum
{
 NVM_SEEK_SET = 0x10000, /* seek to an absolute position */ //force to be 4 Bytes wide
 NVM_SEEK_CUR, /* seek relative to current position */
 NVM_SEEK_END /* seek relative to end of file */

}NVM_SEEK_ORIGIN;





//open defines







typedef UINT32 NVM_OP_CODE; //Describe the message type, should always be first field of message
# 137 "../include/nvm_shared.h"
//#define NVM_OP_CODE_FILE_GET_SIZE	4
# 146 "../include/nvm_shared.h"
/*typedef enum
   {
    NVM_OP_CODE_FILE_OPEN = 0x10000,        //force to be 4 Bytes wide
    NVM_OP_CODE_FILE_CLOSE,
    NVM_OP_CODE_FILE_SEEK,
    NVM_OP_CODE_FILE_GET_SIZE,
    NVM_OP_CODE_FILE_WRITE,
    NVM_OP_CODE_FILE_READ,
    NVM_OP_CODE_FILE_REMOVE,
    NVM_OP_CODE_MAX_COMMAND
   } NVM_OP_CODE;
 */
/* generic file info struct*/
//ICAT EXPORTED STRUCTURE
typedef struct
{
 CHAR file_name[((6) + 60) + 1];
 INT32 time;
 INT32 date;
 UINT32 size;
 UINT32 data_id; /* FDI identifier for file data */
 INT32 dir_id; /* use for winCE handler and FDI directory handler*/
 INT32 plr_date; /* use by FDI */
}NVM_FILE_INFO;

//typedef struct tag_file_info
//{
//   /* filename plus end of string character */
//   FDI_TCHAR file_name[FILE_NAME_SIZE + 1];
//   int       time;          /* updated time stamp when modified */
//   int       date;          /* updated date stamp when modified */
//   DWORD     size;          /* size of file data in bytes */
//   WORD      owner_id;
//   WORD      group_id;
//   WORD      permissions;
//   FILE_ID   data_id;       /* FDI identifier for file data */
//
//   /* the following fields are needed for power loss recovery */
//
//   FILE_ID   plr_id;        /* used for power loss recovery */
//   int       plr_time;      /* used for power loss recovery */
//   int       plr_date;      /* used for power loss recovery */
//   DWORD     plr_size;      /* used for power loss recovery */
//} FILE_INFO;
// Op Code only struct - for "easy" finding/casting the command opcode
typedef struct
{
 NVM_OP_CODE op_code;
} NVM_OP_CODE_ONLY_STRUCT;

// Open Request structures ////////////////////////////////////////////////////
typedef struct
{
 NVM_OP_CODE op_code;
 CHAR szFileName[((6) + 60)];
 CHAR szFileAttributes[(4)];
}NVM_FILE_OPEN_REQUEST_STRUCT; //48 Bytes


typedef struct
{
 NVM_OP_CODE op_code;
 NVM_STATUS_T error_code; //error code of the requested operation
 UINT32 hFile; //contains the UINT32 representation of File handle

}NVM_FILE_OPEN_RESPONSE_STRUCT; //12 Bytes
/////////////////////////////////////////////////////////////////////////////////


// Close Request structures ////////////////////////////////////////////////////
typedef struct
{
 NVM_OP_CODE op_code;
 UINT32 hFile; //contains the UINT32 representation of File handle
}NVM_FILE_CLOSE_REQUEST_STRUCT; //8 Bytes

typedef struct
{
 NVM_OP_CODE op_code;
 NVM_STATUS_T error_code; //error code of the requested operation

}NVM_FILE_CLOSE_RESPONSE_STRUCT; //8 Bytes
/////////////////////////////////////////////////////////////////////////////////


// Seek Request structures ////////////////////////////////////////////////////
typedef struct
{
 NVM_OP_CODE op_code;
 UINT32 hFile; //contains the UINT32 representation of File handle
 UINT32 nOffset; // offset relative to origin indicator
 NVM_SEEK_ORIGIN nOrigin; // where to seek from

}NVM_FILE_SEEK_REQUEST_STRUCT; //16 Bytes

typedef struct
{
 NVM_OP_CODE op_code;
 NVM_STATUS_T error_code; //error code of the requested operation

}NVM_FILE_SEEK_RESPONSE_STRUCT; //8 Bytes
/////////////////////////////////////////////////////////////////////////////////


// Get File Size Request structures ////////////////////////////////////////////////////
typedef struct
{
 NVM_OP_CODE op_code;
 CHAR szFileName[((6) + 60)]; // file to retieve lenght for
}NVM_FILE_FIND_FIRST_REQUEST_STRUCT; //44 Bytes

// ICAT EXPORTED STRUCTURE
typedef struct
{
 NVM_OP_CODE op_code;
 NVM_FILE_INFO fileParms;
 NVM_STATUS_T error_code; //error code of the requested operation
}NVM_FILE_FIND_FIRST_RESPONSE_STRUCT; // 180 Bytes

typedef struct
{
 NVM_OP_CODE op_code;
 NVM_FILE_INFO fileParms;

}NVM_FILE_FIND_NEXT_REQUEST_STRUCT; // 176 Bytes (depend on the FILE_ID size)

// ICAT EXPORTED STRUCTURE
typedef struct
{
 NVM_OP_CODE op_code;
 NVM_FILE_INFO fileParms;
 NVM_STATUS_T error_code; //error code of the requested operation
}NVM_FILE_FIND_NEXT_RESPONSE_STRUCT; // 180 Bytes (depend on the FILE_ID size)


/////////////////////////////////////////////////////////////////////////////////


// Write File Request structures ////////////////////////////////////////////////////
//calculate the size of the data that this request contains


typedef struct
{
 NVM_OP_CODE op_code; //4 byte
 UINT32 hFile; //4 bytes
 UINT32 nNumberOfItems; //number of characters to to write from the given file handle 4 bytes
 UINT32 nItemSize; //size of each item in the buffer we are writing 4 Byte
 UINT8 DataBuffer[((1024) - (4) - 16)]; //this size should be

}NVM_FILE_WRITE_REQUEST_STRUCT; // 1020 Bytes

typedef struct
{
 NVM_OP_CODE op_code;
 NVM_STATUS_T error_code; //error code of the requested operation
 UINT32 nNumberActuallItemsWrite; //number of characters to to write from the given file handle

}NVM_FILE_WRITE_RESPONSE_STRUCT; // 12 Bytes
/////////////////////////////////////////////////////////////////////////////////

// Read File Request structures ////////////////////////////////////////////////////
typedef struct
{
 NVM_OP_CODE op_code;
 UINT32 nItemSize; //size of each item in the buffer we are writing
 UINT32 hFile;
 UINT32 nNumberOfItems; //number of characters to to write from the given file handle

}NVM_FILE_READ_REQUEST_STRUCT; //16 Bytes





typedef struct
{
 NVM_OP_CODE op_code;
 UINT32 nDataBufferLength;
 NVM_STATUS_T error_code; //error code of the requested operation
 UINT32 nNumberActuallItemsRead; //number of characters to to write from the given file handle
 UINT8 DataBuffer[((1024) - (4) - 16)];
}NVM_FILE_READ_RESPONSE_STRUCT;




// Remove File Request structures ////////////////////////////////////////////////////
typedef struct
{
 NVM_OP_CODE op_code;
 CHAR szFileName[((6) + 60)];

}NVM_FILE_REMOVE_REQUEST_STRUCT; //44 Bytes

typedef struct
{
 NVM_OP_CODE op_code;
 NVM_STATUS_T error_code; //error code of the requested operation
}NVM_FILE_REMOVE_RESPONSE_STRUCT; //8 Bytes

typedef struct
{
 NVM_OP_CODE op_code;
 int iTmp;
} GPC_RTC_DATA_REQUEST_STRUCT;

typedef struct
{
 NVM_OP_CODE op_code;
 int iTmp;
} GPC_RTC_DATA_RESPONSE_STRUCT;

/////////////////////////////////////////////////////////////////////////////////
typedef struct
{
 NVM_OP_CODE op_code;
 NVM_STATUS_T error_code; //error code of the requested operation
}RTC_GET_REQUEST_STRUCT;

//ICAT EXPORTED STRUCT
typedef struct
{
 NVM_OP_CODE op_code;
 NVM_STATUS_T error_code; //error code of the requested operation
 UINT8 second; //  Seconds after minute: 0 - 59
 UINT8 minute; //  Minutes after hour: 0 - 59
 UINT8 hour; //  Hours after midnight: 0 - 23
 UINT8 day; //  Day of month: 1 - 31
 UINT8 month; //  Month of year: 1 - 12
 UINT16 year; //  Calendar year: e.g 2001
}RTC_GET_RESPONSE_STRUCT;
# 7 "../include/NVMServer_api.h" 2
# 1 "../NVM_Proxy/include/NVMServer_defs.h" 1
// (C) Copyright 2006 Marvell International Ltd.
// All Rights Reserved
//
/******************************************************************************
**
** INTEL CONFIDENTIAL
** Copyright 2000-2005 Intel Corporation All Rights Reserved.
**
** The source code contained or described herein and all documents
** related to the source code (Material) are owned by Intel Corporation
** or its suppliers or licensors.  Title to the Material remains with
** Intel Corporation or its suppliers and licensors. The Material contains
** trade secrets and proprietary and confidential information of Intel
** or its suppliers and licensors. The Material is protected by worldwide
** copyright and trade secret laws and treaty provisions. No part of the
** Material may be used, copied, reproduced, modified, published, uploaded,
** posted, transmitted, distributed, or disclosed in any way without Intel’s
** prior express written permission.
**
** No license under any patent, copyright, trade secret or other intellectual
** property right is granted to or conferred upon you by disclosure or
** delivery of the Materials, either expressly, by implication, inducement,
** estoppel or otherwise. Any license under such intellectual property rights
** must be express and approved by Intel in writing.
**
*****************************************************************************/
       

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
# 30 "../NVM_Proxy/include/NVMServer_defs.h" 2
# 1 "../include/pxa_dbg.h" 1
/******************************************************************************

*(C) Copyright 2007 Marvell International Ltd.

* All Rights Reserved

******************************************************************************/
# 31 "../NVM_Proxy/include/NVMServer_defs.h" 2
# 40 "../NVM_Proxy/include/NVMServer_defs.h"
//////////////////////////////////////////////////////////////////////////////
# 49 "../NVM_Proxy/include/NVMServer_defs.h"
//HKEY_LOCAL_MACHINE

//registry value\keys name
# 78 "../NVM_Proxy/include/NVMServer_defs.h"
//FSCTL_USER(FILE_DEVICE_FILE_SYSTEM, 0x1, METHOD_BUFFERED, FILE_ANY_ACCESS)





//Entry format is as followed
// <fs id>,<nvm file name>,<fs file name>

// NVM configuration
# 101 "../NVM_Proxy/include/NVMServer_defs.h"
//Translation Table Structure and Defines
typedef struct
{
 short fs_id;
 TCHAR szNVMfileName[64];
 TCHAR szFSfileName[1024 - 64];

}TRANS_TABLE_ENTRY;





//default path structure and defines
typedef struct
{
 short fs_id;
 TCHAR szFSDefaultPath[1024];

}DEFAULT_PATH_ENTRY, *PDEFAULT_PATH_ENTRY;
# 8 "../include/NVMServer_api.h" 2
# 17 "../include/NVMServer_api.h"
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
NVM_STATUS_T NVMS_FileFindClose( DWORD ClientID, LPHANDLE lpSearchHandle );
NVM_STATUS_T NVMS_FileFindFirst(DWORD ClientID,
    LPCTSTR szFileName,
    NVM_FILE_INFO* pFindResults,
    LPHANDLE lpSearchHandle );
NVM_STATUS_T NVMS_FileFindNext( DWORD ClientID,
    NVM_FILE_INFO* pFindResults,
    LPHANDLE lpSearchHandle );

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

/*******************************************************************************
* Function:		NVMS_Register
********************************************************************************
* Description:	API for Client to get a ID.
*
* Parameters:	szClientName - Client's name (in)
*				pClientID - Client's ID (out)
*
* Return value:	Return result after this function called
*
* Notes:
*******************************************************************************/
NVM_STATUS_T NVMS_Register( LPCTSTR szClientName, DWORD *pClientID);


/*******************************************************************************
* Function:		NVMS_GetFileSize
********************************************************************************
* Description:	API for Client to get specified file size in bytes.
*
* Parameters:	ClientID - Client's ID (in)
*				szFileName - File name (in)
*				pdwSizeHight - file size of high order (out)
*				pdwSizeLow - file size of low order (out)
*
* Return value:	Return result after this function called
*
* Notes:
*******************************************************************************/
NVM_STATUS_T NVMS_GetFileSize( DWORD ClientID, LPCTSTR szFileName, PDWORD pdwSizeHigh, PDWORD pdwSizeLow);


/*******************************************************************************
* Function:		NVMS_FileOpen
********************************************************************************
* Description:	API for Client to open specified file
*
* Parameters:	ClientID - Client's ID (in)
*				szFileName - File name (in)
*				szAttrib - Type of access permitted (in)
*				phFile - the pointer to file handle (out)
*
* Return value:	Return result after this function called
*
* Notes:
*******************************************************************************/
NVM_STATUS_T NVMS_FileOpen( DWORD ClientID, LPCTSTR szFileName, LPCTSTR szAttrib, PUINT32 phFile);


/*******************************************************************************
* Function:		NVMS_FileWrite
********************************************************************************
* Description:	API for Client to write data into a file
*
* Parameters:	ClientID - Client's ID (in)
*				hFile - File handle (in)
*				pBuffer - Pointer of data buffer (in)
*				dwBufferLen - Buffer length (in)
*				wItemSize - size of each item in the buffer we are writing (in)
*				dwCount - number of items to write from the given buffer (in)
*				pdwActual  - pointer for number of items actually written (out)
*
* Return value:	Return result after this function called
*
* Notes:
*******************************************************************************/
NVM_STATUS_T NVMS_FileWrite( DWORD ClientID, UINT32 hFile, LPVOID pBuffer, UINT32 dwBufferLen, short wItemSize, UINT32 dwCount, PUINT32 pdwActual);


/*******************************************************************************
* Function:		NVMS_FileFlush
********************************************************************************
* Description:	API for Client to flush a file
*
* Parameters:	ClientID - Client's ID (in)
*				hFile - File handle (in)
*
* Return value:	Return result after this function called
*
* Notes:
*******************************************************************************/
NVM_STATUS_T NVMS_FileFlush( DWORD ClientID, UINT32 hFile);


/*******************************************************************************
* Function:		NVMS_FileSeek
********************************************************************************
* Description:	API for Client to move the file pointer to a specified location
*
* Parameters:	ClientID - Client's ID (in)
*				hFile - File handle (in)
*				dwOffset -  Number of bytes from origin (in)
*				dwOrigin - Initial position (in)
*
* Return value:	Return result after this function called
*
* Notes:
*******************************************************************************/
NVM_STATUS_T NVMS_FileSeek( DWORD ClientID, UINT32 hFile, LONG dwOffset, INT32 dwOrigin);


/*******************************************************************************
* Function:		NVMS_FileRead
********************************************************************************
* Description:	API for Client to read data from a file
*
* Parameters:	ClientID - Client's ID (in)
*				hFile - File handle (in)
*				wItemSize - size of each item in the buffer we are reading (in)
*				dwCount - number of item to read (in)
*				pdwActual  - pointer for number of items actually read (out)
*				pBuffer - Pointer of data buffer (out)
*
* Return value:	Return result after this function called
*
* Notes:
*******************************************************************************/
NVM_STATUS_T NVMS_FileRead( DWORD ClientID, UINT32 hFile, short wItemSize, UINT32 dwCount, PUINT32 pdwActual, LPVOID pBuffer);


/*******************************************************************************
* Function:		NVMS_FileClose
********************************************************************************
* Description:	API for Client to close a file
*
* Parameters:	ClientID - Client's ID (in)
*				hFile - File handle (in)
*
* Return value:	Return result after this function called
*
* Notes:
*******************************************************************************/
NVM_STATUS_T NVMS_FileClose( DWORD ClientID, UINT32 hFile);


/*******************************************************************************
* Function:		NVMS_FileCloseAll
********************************************************************************
* Description:	API for Client to close all files
*
* Parameters:	ClientID - Client's ID (in)
*
* Return value:	Return result after this function called
*
* Notes:
*******************************************************************************/
NVM_STATUS_T NVMS_FileCloseAll( DWORD ClientID);


/*******************************************************************************
* Function:		NVMS_DeRegister
********************************************************************************
* Description:	API for Client to de register
*
* Parameters:	ClientID - Client's ID (in)
*
* Return value:	Return result after this function called
*
* Notes:
*******************************************************************************/
NVM_STATUS_T NVMS_DeRegister( DWORD ClientID);

/*******************************************************************************
* Function:		NVMS_FileDelete
********************************************************************************
* Description:	API for Client to de delete
*
* Parameters:	ClientID - Client's ID (in)
*
* Return value:	Return result after this function called
*
* Notes:
*******************************************************************************/
NVM_STATUS_T NVMS_FileDelete( DWORD ClientID, LPCTSTR szFileName);

/* NVM extension for flash explorer */
NVM_STATUS_T NVMS_FileRename(DWORD ClientID, LPCTSTR szOldFileName, LPCTSTR szNewFileName);
NVM_STATUS_T NVMS_GetFileStat(DWORD ClientID, LPCTSTR szFileName, PDWORD pdwStat);
NVM_STATUS_T NVMS_FileChangeMode(DWORD ClientID, LPCTSTR szFileName, DWORD dwNewMode);
NVM_STATUS_T NVMS_GetAvailSpace( DWORD ClientID, LPCTSTR szVol, PUINT32 pdwSize);
NVM_STATUS_T NVMS_GetTotalSpace( DWORD ClientID, PUINT32 pdwSize);
NVM_STATUS_T NVMS_MkDir(DWORD ClientID, LPCTSTR szDirName, DWORD dwMode);
NVM_STATUS_T NVMS_RmDir(DWORD ClientID, LPCTSTR szDirName);
# 48 "src/eeh_api.c" 2
# 58 "src/eeh_api.c"
static UINT8 EehTaskStack[2048];
static OSTaskRef EehTaskRef = ((void *)0);

int sehdrv_fd = 0;

static UINT32 iSilentRestsCount = 1;
static int currentAssert = EEH_NONE_ASSERT;

int gDumpHugeMemory = 0;
int b_launch_diag = 0;

void SaveSilentResetStatistics(UINT32 msgId);
EEH_STATUS EehInsertCommtoReset(void);
EEH_STATUS EehReleaseCommFromReset(void);
EEH_STATUS EehCopyCommImageFromFlash(void);
static int EehRamdump(void);
static void EehSaveErrorInfo(unsigned err, char *str, unsigned char *context);
EEH_STATUS EehDisableCPUFreq(void);
EEH_STATUS EehEnableCPUFreq(void);


static void EehSaveCommon(const char* buf)
{
 (void)buf;;
//	char *nvm_root_dir = NULL;

//To save more useful logs

//    EehSaveThreadInfo(NULL);

//	EehSaveNvmBackup(nvm_root_dir);
}

static void EehFinalAction(UINT32 action)
{
 sync(); /* prepare for potential system reset */
 switch (action)
 {
 case EE_STALL: ////do nothing - BSP_ARGS_EE_NO_RESET
 {
  eeh_log(3, "[ERROR Handler] ************  No Reset - STALL **************** .\r\n");
  //When CP is asserted, AP should mark the msocket link as down
  msocket_linkdown();
  break;
 }
 case (EE_RESET_START_BASIC + 1):
 {
  eeh_log(3, "[ERROR Handler] ************  Silent Reset **************** .\r\n");

  //Save silent reset statictics
  //SaveSilentResetStatistics(EEH_WDT_INT_MSG);

  time_t tm;
  time(&tm);
  eeh_log(3, "%s: Fail recovery start at %s", __FUNCTION__, ctime(&tm));

  unlink("/tmp/atcmdsrv_ok");

  //Disable CPUFreq
  if ( EehDisableCPUFreq() != 0 )
  {
   eeh_log(3, "[ERROR Handler] ************  Cannot disable CPUFreq now ******************* .\r\n");
   break;
  }

  //Insert Comm subsystem into Reset
  if ( EehInsertCommtoReset() != 0 )
  {
   eeh_log(3, "[ERROR Handler] ************  Cannot reset CP now ******************* .\r\n");
   break;
  }

  msocket_linkdown();

  //Copy CP&DSP image from Flash to DDR

  EehCopyCommImageFromFlash();


  //Release Comm subsystem from Reset
  EehReleaseCommFromReset();

  msocket_linkup();

  //Enable CPUFreq
  if ( EehEnableCPUFreq() != 0 )
  {
   eeh_log(3, "[ERROR Handler] ************  Cannot enable CPUFreq now ******************* .\r\n");
   break;
  }

  break;
 }
 case ((EE_RESET_START_BASIC + 1) + 1):
 {
  eeh_log(3, "[ERROR Handler] ************   GPIO Reset !!! **************** .\r\n");

  sync();

  reboot(0x01234567);




  break;
 }
 case ((((EE_RESET_START_BASIC + 1) + 1) + 1) + 1):
 {
  eeh_log(3, "[ERROR Handler] ************  RAMDUMP Emulate PANIC **************** .\r\n");

  EehRamdump(); /*noreturn*/
  break;
 }
 default:
 {
  eeh_log(3, "[ERROR Handler] ************  Other Reset - unknow id:%d  *************** .\r\n", eeConfiguration.finalAction);
  break;
 }
 }

}
/************************************************************************************
 *
 * EehTask
 *
 * Description: Error handler task function
 *
 * Parameters:
 *
 * Returns:     None
 *
 * Notes:
 *
 **********************************************************************************/
static void EehTask(void* arg)
{
 (void)arg;

 eeh_log(7, "%s: Error Hander Task is running\n", __FUNCTION__);

 for (;; )
 {
  EehMsgStruct msg;
  UINT32 msgId;
  int received = -1;
  static int iCount = 0;
  static int iCountApReq = 0;
  BOOL isCPAssert = 1;
  received = read(sehdrv_fd, &msg, sizeof(EehMsgStruct));
  if ( received == sizeof(EehMsgStruct))
  {
   msgId = msg.msgId;
   switch (msgId)
   {
   case EEH_WDT_INT_MSG:
   {
    eeh_log(3, "EehTask: receiving EEH_WDT_INT_MSG\n");
    eeReadConfiguration(&eeConfiguration);
    NVMS_FileCloseAll(0x01);
    if (currentAssert != EEH_NONE_ASSERT)
    {
     /* There is assert recovery procedure ongoing, we don't need to recover it again */
     eeh_log(3, "%s: there is assert recovery procedure ongoing. Reject for this new EEH_WDT_INT_MSG\n", __FUNCTION__);
     return;
    }

    iCount++;

    commImageTableInit();

    if (getCommImageBaseAddr() != 0xBAD0DEAD)
    {
     char eeBuffer[512];
     char eeString[512+60];
     /* Print out the CP Error Buffer Description Info */
     isCPAssert = GetComEEDescbuf(eeBuffer, sizeof(eeBuffer));

     EehSaveCommon(eeBuffer);
     EehSaveErrorInfo(0, eeBuffer, 0);

     if(eeConfiguration.finalAction == (EE_RESET_START_BASIC + 1))
      sprintf(eeString, "No.%d CP Assert, Cause:%s", iCount, eeBuffer);
     else
      sprintf(eeString, "non silent CP Assert, Cause:%s", eeBuffer);

     eeh_draw_panic_text(eeString, strlen(eeString), (int)EEH_CP_EXCEPTION);

     if(eeConfiguration.finalAction != (EE_RESET_START_BASIC + 1))
     {
      /* Save Comm Post Morten Files */
      SaveComPostmortem();

      /* Output DTCM, DSP & Ram Log */
      Linux_EELOG_shareReadFWrite();
      sync();
     }


    }

    //Sometimes the WDT interrupt is triggerred by CP dead loop, not by CP Asserts.
    //In this case, we need to dump the whole CP DDR for analysis
    if(eeConfiguration.finalAction != (EE_RESET_START_BASIC + 1))
    {
     if (gDumpHugeMemory || !isCPAssert)
     {
      EE_SaveComDDR_RW(1);
      sync();
     }

     EE_SaveComDDR_DiagOffline(1);
     sync();

    }

    commImageTableFree();

    EehFinalAction(eeConfiguration.finalAction);

    break;
   }

   case EEH_AP_ASSERT_MSG:
    eeh_log(3, "EehTask: receiving EEH_AP_ASSERT_MSG\n");
    eeReadConfiguration(&eeConfiguration);

    EehSaveCommon(msg.msgDesc);
    EehSaveErrorInfo(1, msg.msgDesc, 0);

    eeh_draw_panic_text(msg.msgDesc, strlen(msg.msgDesc), (int)EEH_AP_EXCEPTION);

    EehFinalAction(eeConfiguration.finalAction);

    break;

   case EEH_CP_SILENT_RESET_MSG:
   {
    eeh_log(3, "EehTask: receiving EEH_CP_SILENT_RESET_MSG\n");
    eeReadConfiguration(&eeConfiguration);
    NVMS_FileCloseAll(0x01);
    iCountApReq++;
    commImageTableInit();

    if (getCommImageBaseAddr() != 0xBAD0DEAD)
    {
     char eeString[512+60];

     EehSaveCommon(msg.msgDesc);
     EehSaveErrorInfo(0, msg.msgDesc, 0);
     if(eeConfiguration.finalAction == (EE_RESET_START_BASIC + 1))
      sprintf(eeString, "No.%d CP Reset on AP request, Cause:%s", iCountApReq, msg.msgDesc);
     else
      sprintf(eeString, "No.%d non CP Reset on AP request, Cause:%s", iCountApReq, msg.msgDesc);
     eeh_draw_panic_text(eeString, strlen(eeString), (int)EEH_CP_EXCEPTION);
    }
    //dump the whole CP DDR for analysis
    if(eeConfiguration.finalAction != (EE_RESET_START_BASIC + 1))
    {
     if (gDumpHugeMemory)
     {
      EE_SaveComDDR_RW(1);
      sync();
     }

     EE_SaveComDDR_DiagOffline(1);
     sync();

    }

    commImageTableFree();

    EehFinalAction(eeConfiguration.finalAction);

    break;
   }

   default:
    eeh_log(3, "%s: Invalid msg Id: %d\n", __FUNCTION__, msgId);
    break;
   }
  }
  else
  {
   eeh_log(7, "[%d]: %s\n", (*__errno()), strerror((*__errno())));
   if ((*__errno()) == 4)
    continue;
   else
   {
    eeh_log(3, "%s:Exit Error Handler Task!\n", __FUNCTION__);
    break;
   }
  }
 }

}


/*************************************************
 * EehInit
 *
 * Descrption:
 *
 * Initialize the error handler device and make first contact with the error handler device.
 *
 * Notes:
 *
 *
 **************************************************/
EEH_STATUS EehInit (void)
{
 EehApiParams ApiParams;
 INT32 ret;
 OS_STATUS status;

 sehdrv_fd = open("/dev/seh", 00000002 | 00010000);
 if ( sehdrv_fd == -1)
 {
  eeh_log(3, "%s: open seh driver error...\n", __FUNCTION__);
  return (0 - 2);
 }

 eeInitConfiguration(); //create EE file in case not present

 status = OSATaskCreate(&EehTaskRef, EehTaskStack, 2048, 100, (CHAR *)"EehTask", EehTask, ((void *)0));
 if (status != OS_SUCCESS)
 {
  eeh_log(3, "%s: Error to creat the task\n", __FUNCTION__);
  return (0 - 1);
 }

 ApiParams.eehApiId = _EehInit;
 ApiParams.params = ((void *)0);

 eeh_log(7, "%s: _EehInit\n", __FUNCTION__);

 ret = ioctl(sehdrv_fd, (((1U) << (((0 +8)+8)+14)) | ((('Y')) << (0 +8)) | (((1)) << 0) | (((((sizeof(int) == sizeof(int[1]) && sizeof(int) < (1 << 14)) ? sizeof(int) : __invalid_size_argument_for_IOC))) << ((0 +8)+8))), &ApiParams);
 if (ret)
 {
  eeh_log(3, "%s: error=%d\n", __FUNCTION__, (*__errno()));
  return (0 - 1);
 }

 return 0;
}

/*************************************************
 * EehDeinit
 *
 * Descrption:
 * This function terminates all services provided by the EEH.
 *
 * Notes:
 *
 **************************************************/
EEH_STATUS EehDeinit (void)
{
 EehApiParams ApiParams;
 INT32 ret;

 ApiParams.eehApiId = _EehDeInit;
 ApiParams.params = ((void *)0);

 eeh_log(7, "%s: _EehDeInit\n", __FUNCTION__);

 ret = ioctl(sehdrv_fd, (((1U) << (((0 +8)+8)+14)) | ((('Y')) << (0 +8)) | (((1)) << 0) | (((((sizeof(int) == sizeof(int[1]) && sizeof(int) < (1 << 14)) ? sizeof(int) : __invalid_size_argument_for_IOC))) << ((0 +8)+8))), &ApiParams);
 if (ret)
 {
  eeh_log(3, "%s: error=%d\n", __FUNCTION__, (*__errno()));
  return (0 - 1);
 }

 return ApiParams.status;
}

/*************************************************
 * EehInsertCommtoReset
 *
 * Descrption:
 * This function will insert Comm Subsystem to reset.
 *
 * Notes:
 *
 **************************************************/

EEH_STATUS EehInsertCommtoReset(void)
{
 EehApiParams ApiParams;
 INT32 ret;

 ApiParams.eehApiId = _EehInsertComm2Reset;
 ApiParams.params = ((void *)0);

 eeh_log(7, "%s: _EehInsertComm2Reset\n", __FUNCTION__);

 ret = ioctl(sehdrv_fd, (((1U) << (((0 +8)+8)+14)) | ((('Y')) << (0 +8)) | (((1)) << 0) | (((((sizeof(int) == sizeof(int[1]) && sizeof(int) < (1 << 14)) ? sizeof(int) : __invalid_size_argument_for_IOC))) << ((0 +8)+8))), &ApiParams);
 if (ret)
 {
  eeh_log(3, "%s: error=%d\n", __FUNCTION__, (*__errno()));
  return (0 - 1);
 }

 return ApiParams.status;

}

/*************************************************
 * EehReleaseCommFromReset
 *
 * Descrption:
 * This function will release Comm Subsystem from reset.
 *
 * Notes:
 *
 **************************************************/

EEH_STATUS EehReleaseCommFromReset(void)
{
 EehApiParams ApiParams;
 INT32 ret;

 ApiParams.eehApiId = _EehReleaseCommFromReset;
 ApiParams.params = ((void *)0);

 eeh_log(7, "%s: _EehReleaseCommFromReset\n", __FUNCTION__);

 ret = ioctl(sehdrv_fd, (((1U) << (((0 +8)+8)+14)) | ((('Y')) << (0 +8)) | (((1)) << 0) | (((((sizeof(int) == sizeof(int[1]) && sizeof(int) < (1 << 14)) ? sizeof(int) : __invalid_size_argument_for_IOC))) << ((0 +8)+8))), &ApiParams);
 if (ret)
 {
  eeh_log(3, "%s: error=%d\n", __FUNCTION__, (*__errno()));
  return (0 - 1);
 }

 return ApiParams.status;

}

/*************************************************
 * SaveSilentResetStatistics
 *
 * Descrption:
 * This function will save the silent reset statistics.
 *
 * Notes:
 *
 **************************************************/
void SaveSilentResetStatistics(UINT32 msgId)
{
 (void)msgId;

 FILE *fd;
 char fullPath[256] = { 0 };
 char fileName[] = "SilentResetStatistic.log";
 char *nvm_root_dir = getenv("NVM_ROOT_DIR");
 time_t timep;

 if (nvm_root_dir != ((void *)0))
 {
  strcpy(fullPath, nvm_root_dir);
  strcat(fullPath, "/");
 }
 strcat(fullPath, fileName);

 if ((fd = fopen(fullPath, "aw")) != 0)
 {
  time(&timep);
  fprintf(fd, "CPP ERROR causes silent reset No.%d : %s", iSilentRestsCount, ctime(&timep));
  fclose(fd);
  iSilentRestsCount++;
 }
}

/*************************************************
 * EehCopyCommImageFromFlash
 *
 * Descrption:
 * This function will copy Comm image from flash to DDR.
 *
 * Notes:
 *
 **************************************************/

EEH_STATUS EehCopyCommImageFromFlash(void)
{
 EehApiParams ApiParams;
 INT32 ret;

 ApiParams.eehApiId = _EehCopyCommImageFromFlash;
 ApiParams.params = ((void *)0);

 eeh_log(7, "%s: _EehCopyCommImageFromFlash\n", __FUNCTION__);

 ret = ioctl(sehdrv_fd, (((1U) << (((0 +8)+8)+14)) | ((('Y')) << (0 +8)) | (((1)) << 0) | (((((sizeof(int) == sizeof(int[1]) && sizeof(int) < (1 << 14)) ? sizeof(int) : __invalid_size_argument_for_IOC))) << ((0 +8)+8))), &ApiParams);
 if (ret)
 {
  eeh_log(3, "%s: error=%d\n", __FUNCTION__, (*__errno()));
  return (0 - 1);
 }

 return ApiParams.status;
}




static int EehPushRamfile(int dfd, const char* file)
{
 char *vpa = 0;
 FILE* ffd = 0;
 int len, l;
 struct stat fst;
 int ret = -1;

 if ((ffd=fopen(file, "rb"))==((void *)0)) {
  fprintf((&__sF[2]), "RAMFILE: cannot open %s\n", file);
  goto bail;
 }
 if (fstat(((ffd)->_file), &fst)) {
  fprintf((&__sF[2]), "RAMFILE: cannot stat %s\n", file);
  goto bail;
 }

 len = fst.st_size;
 if ((len<=0) || (len>0x400000)) {
  fprintf((&__sF[2]), "RAMFILE: bad length %d file %s\n", len, file);
  goto bail;
 }

 if ((vpa=(char *)mmap(0, len + 32, 0x1|0x2, 0x01, dfd, 0))==((void *)-1)) {
  fprintf((&__sF[2]), "RAMFILE: failed to allocate %d bytes for file %s\n", len, file);
  goto bail;
 }
 printf("RAMFILE vpa=%.8x\n", (unsigned)vpa);

 if ((l = fread((void*)(vpa+32), 1, len, ffd)) != len) {
  fprintf((&__sF[2]), "RAMFILE: failed to read %d bytes from %s, ret=%d\n", len, file, l);
  goto bail;
 }

 fprintf((&__sF[2]), "RAMFILE: pushed %d bytes from %s\n", len, file);
 /* OK, data is in memory */
 ret = 0;
bail:
 if (ffd)
  fclose(ffd);
 if (vpa)
  munmap(vpa, len);
 return ret;
}



static int EehRamdumpPushLogfiles(int fd)
{
 char tmpfilename[256];
 char command_buf[256 +50];
 int ret = -1;
 char *nvm_root_dir = getenv("NVM_ROOT_DIR");

 if (nvm_root_dir == ((void *)0))
  return ret;

 sprintf(tmpfilename, "%s/%s", nvm_root_dir, "ramfiletmp.tgz");
 sprintf(command_buf, "busybox tar cvz -f %s -C %s -T %s/%s",
  tmpfilename, nvm_root_dir, "/marvell/tel", "ramdump_loglist");
 system(command_buf); /* create tgz containing mandatory files */
 ret = EehPushRamfile(fd, tmpfilename);
 unlink(tmpfilename);
 return ret;
}
# 677 "src/eeh_api.c"
static int EehRamdump(void)
{
 int fd;
 int ret;
 fd = open("/dev/ramfile", 00000002 | 00010000);
 if (fd < 0) {
   eeh_log(3, "%s: failed to open ramfile driver, error=%d\n", __FUNCTION__, (*__errno()));
 } else {
  /* Push mandatory files */
  EehRamdumpPushLogfiles(fd);

  /* Push DIAG log files */
  //disabled because in current code Diag log has been put to SDCARD, need not ramdump
//		EehRamdumpPushSdlfiles(fd);
  close(fd);
  }

 /* Emulate panic that triggers ramdump */
 sync();
 ret = ioctl(sehdrv_fd, (((1U) << (((0 +8)+8)+14)) | ((('Y')) << (0 +8)) | (((13)) << 0) | (((((sizeof(int) == sizeof(int[1]) && sizeof(int) < (1 << 14)) ? sizeof(int) : __invalid_size_argument_for_IOC))) << ((0 +8)+8))), ((void *)0));
 if (ret) {
  eeh_log(3, "%s: error=%d\n", __FUNCTION__, (*__errno()));
  return -1;
 }

 return 0;
}

/* Communicate the error info to SEH (for RAMDUMP and m.b. more) */
static void EehSaveErrorInfo(unsigned err, char *str, unsigned char *context)
{
 EehErrorInfo info;
 info.err = err;
 info.str = str;
 info.regs = context;
 ioctl(sehdrv_fd, (((1U) << (((0 +8)+8)+14)) | ((('Y')) << (0 +8)) | (((14)) << 0) | (((((sizeof(int) == sizeof(int[1]) && sizeof(int) < (1 << 14)) ? sizeof(int) : __invalid_size_argument_for_IOC))) << ((0 +8)+8))), &info);
}

/* EehDisableCPUFreq and EehEnableCPUFreq must used in pair, first disable then enable */
EEH_STATUS EehDisableCPUFreq(void)
{
 EehApiParams ApiParams;
 INT32 ret;

 ApiParams.eehApiId = _EehDisableCPUFreq;
 ApiParams.params = ((void *)0);

 eeh_log(7, "%s: _EehDisableCPUFreq\n", __FUNCTION__);

 ret = ioctl(sehdrv_fd, (((1U) << (((0 +8)+8)+14)) | ((('Y')) << (0 +8)) | (((1)) << 0) | (((((sizeof(int) == sizeof(int[1]) && sizeof(int) < (1 << 14)) ? sizeof(int) : __invalid_size_argument_for_IOC))) << ((0 +8)+8))), &ApiParams);
 if (ret)
 {
  eeh_log(3, "%s: error=%d\n", __FUNCTION__, (*__errno()));
  return (0 - 1);
 }

 return ApiParams.status;

}

EEH_STATUS EehEnableCPUFreq(void)
{
 EehApiParams ApiParams;
 INT32 ret;

 ApiParams.eehApiId = _EehEnableCPUFreq;
 ApiParams.params = ((void *)0);

 eeh_log(7, "%s: _EehEnableCPUFreq\n", __FUNCTION__);

 ret = ioctl(sehdrv_fd, (((1U) << (((0 +8)+8)+14)) | ((('Y')) << (0 +8)) | (((1)) << 0) | (((((sizeof(int) == sizeof(int[1]) && sizeof(int) < (1 << 14)) ? sizeof(int) : __invalid_size_argument_for_IOC))) << ((0 +8)+8))), &ApiParams);
 if (ret)
 {
  eeh_log(3, "%s: error=%d\n", __FUNCTION__, (*__errno()));
  return (0 - 1);
 }

 return ApiParams.status;
}
