/* ===========================================================================
   File        : osa_sys.c
   Description : Implementation file for the system interface
	      of the os/osa package.

   Notes       :

   Copyright (c) 2001 Intel CCD. All Rights Reserved
   =========================================================================== */

#include "osa_sys.h"

static char osaVersionStr[] = "PK_OSA_3.4.2";

/* ---------------------------------------------------------------------------
   Function    : OsaGetVersion
   Description : Returns the version of the osa package
   Parameters  : none
   Returns     : pointer to a null-terminated char array
   Notes       :
   --------------------------------------------------------------------------- */
char *OsaGetVersion ( void )
{
	return(osaVersionStr);
}

/* ---------------------------------------------------------------------------
   Function    : OsaInit
   Description : Initialization function for system interface
	      the osa package.
   Parameters  :
   Returns     :
   Notes       :
   --------------------------------------------------------------------------- */
void OsaInit ( void )
{
	return;
}


/*                   end of osa_sys.c
   --------------------------------------------------------------------------- */






