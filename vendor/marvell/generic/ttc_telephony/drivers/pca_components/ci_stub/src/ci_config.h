/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/

/* ===========================================================================
   File        : ci_config.h
   Description : Configuration parameters for the
	      ci package.

	      The following are internal ci configuration or tuning parameters
	      that are used to configure the ci services.

   Notes       : These values can be overridden in gbl_config.h
	      The range checks should be updated for each
	      parameter.

   Copyright (c) 2001 Intel CCD. All Rights Reserved
   =========================================================================== */

#if !defined(_CI_CONFIG_H_)
#define _CI_CONFIG_H_

#define PCAC_BASE_TASK_PRIORITY 127
/* ---------------------------------------------------------------------------
   Parameter   : CI_STUB_TASK_PRIORITY
   Description : Default task priority for CI stubs
   Notes       : Based on the PCAC base task priority
   --------------------------------------------------------------------------- */

#define CI_STUB_TASK_PRIORITY           (PCAC_BASE_TASK_PRIORITY        + 5)


/* ---------------------------------------------------------------------------
   Parameters  : CI_SYS_INIT_TIME_OUT
   Description : Default timeout for CI Stub intialization
   Notes       :
   --------------------------------------------------------------------------- */

#define CI_SYS_INIT_TIME_OUT  0x000007D0

/* ---------------------------------------------------------------------------
   Parameters  : CI_SYS_DEINIT_TIME_OUT
   Description : Default timeout for CI Stub deinitialization
   Notes       :
   --------------------------------------------------------------------------- */

#define CI_SYS_DEINIT_TIME_OUT 0x00001000

/* ---------------------------------------------------------------------------
   Parameters  : CI_STUB_DAT_BUFFER_SIZE
   Description : Default buffer size for opt CI Dat primitive
   Notes       :
   --------------------------------------------------------------------------- */
#define CI_STUB_DAT_BUFFER_SIZE 0x800

/* ---------------------------------------------------------------------------
   Parameters  : CI_STUB_DAT_WINDOW_SIZE
   Description : Default window size for opt CI Dat primitive
   Notes       :
   --------------------------------------------------------------------------- */
//#define CI_STUB_DAT_WINDOW_SIZE 7

/* ---------------------------------------------------------------------------*/
#if defined(_GBL_CONFIG_H_)
#undef _GBL_CONFIG_H_
#endif
#include "gbl_config.h"

#endif
