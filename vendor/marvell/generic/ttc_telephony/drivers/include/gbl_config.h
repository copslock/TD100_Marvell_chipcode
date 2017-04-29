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

#if !defined(_GBL_CONFIG_H_)
#define _GBL_CONFIG_H_

#ifdef OSA_TASKS
#undef OSA_TASKS
#define OSA_TASKS           32
#endif

#ifdef OSA_TIMERS
#undef OSA_TIMERS
#define OSA_TIMERS          10
#endif

#ifdef OSA_FLAGS
#undef OSA_FLAGS
#define OSA_FLAGS           10
#endif

#ifdef OSA_SEMAPHORES
#undef OSA_SEMAPHORES
#define OSA_SEMAPHORES      16
#endif

#ifdef OSA_MUTEXES
#undef OSA_MUTEXES
#define OSA_MUTEXES         16
#endif

#ifdef OSA_MEM_POOLS
#undef OSA_MEM_POOLS
#define OSA_MEM_POOLS       16
#endif

#ifdef OSA_MSG_QUEUES
#undef OSA_MSG_QUEUES
#define OSA_MSG_QUEUES      16
#endif

#ifdef OSA_MESSAGING_POOL_SIZE
#undef OSA_MESSAGING_POOL_SIZE
#define OSA_MESSAGING_POOL_SIZE     16384
#endif

#ifdef OSA_MBOX_QUEUES
#undef OSA_MBOX_QUEUES
#define OSA_MBOX_QUEUES      1
#endif

#ifdef OSA_MBOX_POOL_SIZE
#undef OSA_MBOX_POOL_SIZE
#define OSA_MBOX_POOL_SIZE     2048
#endif

#ifdef OSA_INTERRUPTS
#undef OSA_INTERRUPTS
#define OSA_INTERRUPTS       0
#endif

#ifdef OSA_TICK_FREQ_IN_MILLISEC
#undef OSA_TICK_FREQ_IN_MILLISEC
#define OSA_TICK_FREQ_IN_MILLISEC   5           /* Tick frequency for the OS.      */
#endif                                          /* Ticks are generated every 5ms   */

#ifdef CI_SYS_INIT_TIME_OUT
#undef CI_SYS_INIT_TIME_OUT
#define CI_SYS_INIT_TIME_OUT   0x00010000
#endif

#endif /* _GBL_CONFIG_H_ */

/*                      end of gbl_config.h
			--------------------------------------------------------------------------- */

