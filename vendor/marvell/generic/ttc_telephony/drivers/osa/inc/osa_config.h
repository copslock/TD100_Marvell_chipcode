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

#if !defined(_OSA_CONFIG_H_)
#define _OSA_CONFIG_H_

/* ---------------------------------------------------------------------------
   Parameter   : osa TASKS Config Parameter
   Description : number of tasks required in the system
   Notes       :
   --------------------------------------------------------------------------- */
#define OSA_TASKS           32
#define OSA_TASKS_MIN       0
#define OSA_TASKS_STEP      1
#define OSA_TASKS_MAX       OSA_TASKS
#define OS_MAX_TASKS        OSA_TASKS
/* ---------------------------------------------------------------------------
   Parameter   : osa TIMERS Config Parameter
   Description : number of timers required in the system
   Notes       :
   --------------------------------------------------------------------------- */
#define OSA_TIMERS          16
#define OSA_TIMERS_MIN      0
#define OSA_TIMERS_STEP     1
#define OSA_TIMERS_MAX      OSA_TIMERS
#define OS_MAX_TIMERS       OSA_TIMERS
/* ---------------------------------------------------------------------------
   Parameter   : osa FLAGS Config Parameter
   Description : number of event flags required in the system
   Notes       :
   --------------------------------------------------------------------------- */
#define OSA_FLAGS           32
#define OSA_FLAGS_MIN       0
#define OSA_FLAGS_STEP      1
#define OSA_FLAGS_MAX       OSA_FLAGS
#define OS_MAX_FLAG_GROUPS  OSA_FLAGS
/* ---------------------------------------------------------------------------
   Parameter   : osa SEMAPHORES Config Parameter
   Description : number of semaphores required in the system
   Notes       :
   --------------------------------------------------------------------------- */
#define OSA_SEMAPHORES      16
#define OSA_SEMAPHORES_MIN  0
#define OSA_SEMAPHORES_STEP 1
#define OSA_SEMAPHORES_MAX  OSA_SEMAPHORES
#define OS_MAX_SEMAPHORES   OSA_SEMAPHORES
/* ---------------------------------------------------------------------------
   Parameter   : osa MUTEXES Config Parameter
   Description : number of mutexes required in the system
   Notes       :
   --------------------------------------------------------------------------- */
#define OSA_MUTEXES         16
#define OSA_MUTEXES_MIN     0
#define OSA_MUTEXES_STEP    1
#define OSA_MUTEXES_MAX     OSA_MUTEXES
#define OS_MAX_MUTEXES      OSA_MUTEXES
/* ---------------------------------------------------------------------------
   Parameter   : osa MEM_POOLS Config Parameter
   Description : number of memory pools required in the system
   Notes       :
   --------------------------------------------------------------------------- */
#define OSA_MEM_POOLS       16
#define OSA_MEM_POOLS_MIN   0
#define OSA_MEM_POOLS_STEP  1
#define OSA_MEM_POOLS_MAX   OSA_MEM_POOLS
#define OS_MAX_MEM_POOLS    OSA_MEM_POOLS
/* ---------------------------------------------------------------------------
   Parameter   : osa MESSAGING_POOL_SIZE Config Parameter
   Description : number of bytes required for total message Q pools in the system
   Notes       : If there are 5 message queues with max size of 32 bytes with a max
	      number of 100 then you would need 100 * 32 bytes per message * 5 = 16000 bytes.
   --------------------------------------------------------------------------- */
#define OSA_MESSAGING_POOL_SIZE       65536
#define OSA_MESSAGING_POOL_SIZE_MIN   0
#define OSA_MESSAGING_POOL_SIZE_STEP  4
#define OSA_MESSAGING_POOL_SIZE_MAX   OSA_MESSAGING_POOL_SIZE
/* ---------------------------------------------------------------------------
   Parameter   : osa MSG_QUEUES Config Parameter
   Description : number of message queues required in the system
   Notes       :
   --------------------------------------------------------------------------- */
#define OSA_MSG_QUEUES       16
#define OSA_MSG_QUEUES_MIN   0
#define OSA_MSG_QUEUES_STEP  1
#define OSA_MSG_QUEUES_MAX   32 //OSA_MSG_QUEUES
#define OS_MAX_MSG_QUEUES    OSA_MSG_QUEUES
/* ---------------------------------------------------------------------------
   Parameter   : osa MBOX_POOL_SIZE Config Parameter
   Description : number of bytes required for total mailbox Q pools in the system
   Notes       : If there are 5 mailboxes each with a max of 100 messages then
	      you would need 100 * 4 bytes per message * 5 = 2000 bytes.
   --------------------------------------------------------------------------- */
#define OSA_MBOX_POOL_SIZE       2000
#define OSA_MBOX_POOL_SIZE_MIN   0
#define OSA_MBOX_POOL_SIZE_STEP  4
#define OSA_MBOX_POOL_SIZE_MAX   OSA_MBOX_POOL_SIZE
/* ---------------------------------------------------------------------------
   Parameter   : osa MBOX_QUEUES Config Parameter
   Description : number of mailbox queues required in the system
   Notes       :
   --------------------------------------------------------------------------- */
#define OSA_MBOX_QUEUES      16
#define OSA_MBOX_QUEUES_MIN  0
#define OSA_MBOX_QUEUES_STEP 1
#define OSA_MBOX_QUEUES_MAX  OSA_MBOX_QUEUES
#define OS_MAX_MBOX_QUEUES   OSA_MBOX_QUEUES

/* ---------------------------------------------------------------------------
   Parameter   : osa INTERRUPTS Config Parameter
   Description : number of OSA interrupts required in the OSA system
   Notes       :
   --------------------------------------------------------------------------- */
#define OSA_INTERRUPTS       16
#define OSA_INTERRUPTS_MIN   0
#define OSA_INTERRUPTS_STEP  1
#define OSA_INTERRUPTS_MAX   OSA_INTERRUPTS
#define OS_MAX_INTERRUPTS    OSA_INTERRUPTS

/* ---------------------------------------------------------------------------
   Parameter   : osa GLOBAL_MEMORY_SIZE Config Parameter
   Description : number of bytes of global memory required in the system
   Notes       :
   --------------------------------------------------------------------------- */
#define OSA_GLOBAL_MEM_POOL_SIZE       1024
#define OSA_GLOBAL_MEM_POOL_SIZE_MIN   0
// #define OSA_GLOBAL_MEM_POOL_SIZE_STEP  1  // is this needed?
#define OSA_GLOBAL_MEM_POOL_SIZE_MAX   OSA_GLOBAL_MEM_POOL_SIZE

/* ---------------------------------------------------------------------------
   Parameter   : osa SISR_STACK_SIZE Config Parameter
   Description : Size of SISR stack used by all SISR routines using OSA
   Notes       : Configure this setting for the largest amount of stack used by
	      all ISR's in the system, GARSON 512, HERMON 2048
   --------------------------------------------------------------------------- */
#define OSA_SISR_STACK_SIZE  2048

#define OSA_MEM_STACK_FILL_PATERN_SIZE     50
#define OSA_MEM_STACK_FILL_PATERN_VALUE    0xA5A5

/* ----------------------------------------------------------------
   Parameter   : osa TICK_FREQ_IN_MILLISEC Parameter
   Description : Number of milliseconds between each clock tick generated.
   Notes       :
   ---------------------------------------------------------------- */
#define OSA_TICK_FREQ_IN_MILLISEC   5   /* Tick frequency for the OS.      */
					/* Ticks are generated every 5ms   */




/* Include the global configuration file, so these values
   can be overridden */
#if defined(_GBL_CONFIG_H_)
#undef _GBL_CONFIG_H_
#endif
#include "gbl_config.h"

/* Check the Parameter Ranges */
#if (OSA_TASKS < OSA_TASKS_MIN) || (OSA_TASKS > OSA_TASKS_MAX)
#error "Osa Package TASKS parameter out of range in osa_config.h."
#endif
#if (OSA_TIMERS < OSA_TIMERS_MIN) || (OSA_TIMERS > OSA_TIMERS_MAX)
#error "Osa Package TIMERS parameter out of range in osa_config.h."
#endif
#if (OSA_FLAGS < OSA_FLAGS_MIN) || (OSA_FLAGS > OSA_FLAGS_MAX)
#error "Osa Package FLAGS parameter out of range in osa_config.h."
#endif
#if (OSA_SEMAPHORES < OSA_SEMAPHORES_MIN) || (OSA_SEMAPHORES > OSA_SEMAPHORES_MAX)
#error "Osa Package SEMAPHORES parameter out of range in osa_config.h."
#endif
#if (OSA_MUTEXES < OSA_MUTEXES_MIN) || (OSA_MUTEXES > OSA_MUTEXES_MAX)
#error "Osa Package MUTEXES parameter out of range in osa_config.h."
#endif
#if (OSA_MEM_POOLS < OSA_MEM_POOLS_MIN) || (OSA_MEM_POOLS > OSA_MEM_POOLS_MAX)
#error "Osa Package MEM_POOLS parameter out of range in osa_config.h."
#endif
#if (OSA_MSG_QUEUES < OSA_MSG_QUEUES_MIN) || (OSA_MSG_QUEUES > OSA_MSG_QUEUES_MAX)
#error "Osa Package MSG_QUEUES parameter out of range in osa_config.h."
#endif
#if (OSA_MBOX_QUEUES < OSA_MBOX_QUEUES_MIN) || (OSA_MBOX_QUEUES > OSA_MBOX_QUEUES_MAX)
#error "Osa Package MBOX_QUEUES parameter out of range in osa_config.h."
#endif
#if (OSA_GLOBAL_MEM_POOL_SIZE < OSA_GLOBAL_MEM_POOL_SIZE_MIN) || (OSA_GLOBAL_MEM_POOL_SIZE > OSA_GLOBAL_MEM_POOL_SIZE_MAX)
#error "Osa Package GLOBAL_MEMORY_SIZE parameter out of range in osa_config.h."
#endif

#if defined(OSA_NUCLEUS) && !defined(OSA_PC_TEST) && !defined(OSA_NU_TARGET)
#define OSA_NU_TARGET
#endif

#endif /* _OSA_CONFIG_H_ */


/*                      end of osa_config.h
   --------------------------------------------------------------------------- */





