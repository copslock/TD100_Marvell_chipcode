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


#ifndef __EEH__
#define __EEH__

#include "eeh_ioctl.h"

#ifdef __cplusplus
extern "C" {
#endif

EEH_STATUS EehInit(void);
EEH_STATUS EehDeinit(void);
void EehStartHBTest(void);

#ifdef __cplusplus
}
#endif


extern int gDumpHugeMemory;
extern int b_launch_diag;

#endif


