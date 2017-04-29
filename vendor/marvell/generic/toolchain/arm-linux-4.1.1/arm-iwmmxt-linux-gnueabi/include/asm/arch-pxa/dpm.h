/*
 * include/asm-arm/arch-pxa/dpm.h
 *
 * Monahans-specific definitions for DPM.  If further PXA boards are
 * supported in the future, will split into board-specific files.
 *
 * Based on Bulverde code
 
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * Copyright (C) 2002 MontaVista Software <source@mvista.com>
 *
 * Based on arch/ppc/platforms/ibm405lp_dpm.h by Bishop Brock.
 */

#ifndef __ASM_ARM_PXA_DPM_H__
#define __ASM_ARM_PXA_DPM_H__

#include <asm/arch/cpu-freq-voltage-mhn.h>
#include <asm/arch/ipmc.h>
/*
 * machine dependent operating state
 *
 * An operating state is a cpu execution state that has implications for power
 * management. The DPM will select operating points based largely on the
 * current operating state.
 *
 * DPM_STATES is the number of supported operating states. Valid operating
 * states are from 0 to DPM_STATES-1 but when setting an operating state the
 * kernel should only specify a state from the set of "base states" and should
 * do so by name.  During the context switch the new operating state is simply
 * extracted from current->dpm_state.
 *
 * task states:
 *
 * APIs that reference task states use the range -(DPM_TASK_STATE_LIMIT + 1)
 * through +DPM_TASK_STATE_LIMIT.  This value is added to DPM_TASK_STATE to
 * obtain the downward or upward adjusted task state value. The
 * -(DPM_TASK_STATE_LIMIT + 1) value is interpreted specially, and equates to
 * DPM_NO_STATE.
 *
 * Tasks inherit their task operating states across calls to
 * fork(). DPM_TASK_STATE is the default operating state for all tasks, and is
 * inherited from init.  Tasks can change (or have changed) their tasks states
 * using the DPM_SET_TASK_STATE variant of the sys_dpm() system call.  */

#define DPM_NO_STATE		-1

#define DPM_IDLE_TASK_STATE	0
#define DPM_IDLE_STATE		1
#define DPM_SLEEP_STATE		2
#define DPM_LOWPOWER_STATE	2
#define DPM_BASE_STATES		3

#define DPM_TASK_STATE_LIMIT	2
#define DPM_TASK_STATE		(DPM_BASE_STATES + DPM_TASK_STATE_LIMIT)
#define DPM_STATES		(DPM_TASK_STATE + DPM_TASK_STATE_LIMIT + 1)
#define DPM_TASK_STATES		(DPM_STATES - DPM_BASE_STATES)

/*
 *length of DPM_STATE_NAMES  is DPM_STATES,
 */
#define DPM_STATE_NAMES			\
{ "idle-task", "idle", "sleep",		\
  "task-2", "task-1",			\
  "task",				\
  "task+1", "task+2",			\
}

/* Operating point parameters */
#define DPM_MD_XL		0  /* XL */
#define DPM_MD_XN		1  /* XN */
#define DPM_MD_V_CORE		2  /* Core Voltage */
#define DPM_MD_V_SRAM		3  /* SRAM Voltage */
#define DPM_MD_SMCFS		4  /* Static Memory Controller Freq */
#define DPM_MD_SFLFS		5  /* Internal SRAM Bus Freq */
#define DPM_MD_HSS		6  /* High-speed I/O Bus Freq */
#define DPM_MD_DMCFS		7  /* Dynamic Memory Controller Freq */
#define DPM_MD_DF_CLK		8  /* Data Flash Freq */
#define DPM_MD_EMPI_CLK		9
#define DPM_MD_CPU_MODE		10 /* RUN, RO, STANDBY, SLEEP, ... */

/* this is the number of specifiable operating point parameters,
 * used by arch-independent DPM-core driver
 */
#define DPM_PP_NBR 11
#define DPM_PARAM_NAMES {"l","n","vcc_core","vcc_sram","smcfs","sflfs",	\
			 "hss","dmcfs","df_clk","empi_clk","cpu_mode"};

#if 0
#ifdef CONFIG_DPM_TRACE
typedef __u64 dpm_md_time_t;

extern char *dpm_opp_names[DPM_PP_NBR];
extern inline dpm_md_time_t
dpm_md_time() {
	struct timeval tv;
	u64 time;
	do_gettimeofday(&tv);
	time = ((u64)tv.tv_sec * 1000000) + tv.tv_usec;
	return time;
}
#define DPM_MD_HZ		1000000
#if 0
#define dpm_md_time()	0
#define DPM_MD_HZ		6 * 1000000
#endif
#endif
#endif

#ifndef __ASSEMBLER__

#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/kernel.h>
#include <linux/notifier.h>
#include <asm/hardware.h>
#include <asm/arch/pxa-regs.h>
#include <asm/arch/cpu-freq-voltage-mhn.h>

#define dpm_notifier_entry(ptr, type, member)	container_of(ptr, type, member)

enum
{
	CPUMODE_RUN,
	CPUMODE_RO,
	CPUMODE_LOWPOWER,
	CPUMODE_STANDBY = 2,
	CPUMODE_SLEEP,
	CPUMODE_DEEPSLEEP,
	CPUMODE_RESERVED,
	CPUMODE_RESERVED2,
};

enum
{
	DPM_OPTION_TEMP,
	DPM_OPTION_PERMANENT,
};

struct dpm_md_opt {
	unsigned long xl;
	unsigned long xn;
	unsigned long vcc_core;
	unsigned long vcc_sram;
	unsigned long smcfs;
	unsigned long sflfs;
	unsigned long hss;
	unsigned long dmcfs;
	unsigned long df_clk;
	unsigned long empi_clk;
	/* The cpu_mode properity is different with d0cs.
	 * It indicates low power mode, run mode and RO mode.
	 */
	unsigned long cpu_mode;
	unsigned long lpj; /* New value for loops_per_jiffy */
	unsigned long op_idx;
};

struct dpm_notifier_block {
	char *name;
	void *client;
	int ret_code;
	struct notifier_block nb;
};

#endif /* __ASSEMBLER__ */
#endif /* __ASM_BULVERDE_DPM_H__ */

