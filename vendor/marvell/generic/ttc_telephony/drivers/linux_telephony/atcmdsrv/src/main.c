/******************************************************************************

*(C) Copyright 2007 Marvell International Ltd.

* All Rights Reserved

******************************************************************************/
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
   -------------------------------------------------------------------------------------------------------------------
 *
 *  Filename: main.c
 *
 *  Authors:  Vincent Yeung
 *
 *  Description: Entry Point of the AT Command Server
 *
 *  HISTORY:
 *   June 8, 2006 - handle more command line arguments
 *   May 25, 2006 - Change file name from atcmdsrv.c to main.c
 *   Jan 11, 2006 - Integrated ENABLE_CSD_TESTING for CSD
 *
 *		Nov, 2005 - Integrated ENABLE_PSD_TESTING for Network Driver Prototype
 *
 *   Aug 24, 2005 - Initial Version
 *
 *  Notes:
 *
 ******************************************************************************/

/******************************************************************************
*   include files
******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <fcntl.h>

#if defined (BIONIC)
#include <linux/capability.h>
#include <linux/prctl.h>
#include <private/android_filesystem_config.h>
#endif

/* for CI init*/
#include "ci_syswrap.h"         /* define CISW_SUCCESS */
#include "ci_dat.h"

/*
 * used for openning the MSL device
 */
#include "osa.h"
#include "utilities.h"

/* Telephoney Controller Includes*/
#include "telcontroller.h"
#include "telatci.h"
#include "telconfig.h"
#if defined(ENABLE_DIAG)
#include "diag_init.h"
#endif
#include "utlTrace.h"
#include "utlEventHandler.h"
#include "modem_al.h"

/******************************************************************************
*   external variables
******************************************************************************/
extern int loglevel;
char log_tag[256] = "MARVELL_TEL_ATCMD";
extern FILE *logfile;

/******************************************************************************
*   external function prototype
******************************************************************************/
extern void initATCI(void);
extern void CiInitCallBack( CiSysWrapStatus status );
extern UINT32 gServiceGroupRegistered;
#if defined(ENABLE_DIAG)
extern void diagPhase1Init(void);
extern void diagPhase2Init(void);
#endif

#if defined(BIONIC)
extern int init_stdfiles(void);
#endif
extern int diag_init(void);
//extern BOOL InitMsldlAl (void);
/****************************************************************************n
   Internal Variable
 ******************************************************************************/

/******************************************************************************
*   Global Variable
******************************************************************************/
int bTerminated[NUM_OF_TEL_ATP] = { 0 };       /* Indicate if the process is being terminated */
BOOL bLaunchDiag = FALSE;
BOOL bLaunchModem = FALSE;
char acExtSerialPath[255];
char modemSerialPath[255];
BOOL bLocalTest = FALSE;
extern BOOL giUseUlDlOpt;

void printUsage()
{
	printf( "atcmdsrv  --  AT Command Server.  Call with atcmdsrv [inputs [output]]\n"
		"CopyRight (c) 2006 Marvell Canada & Vincent Yeung\n"
		"Usage: atcmdsrv [-D yes/no][-l loglevel ]  [-s external-serial-port]\n"
		"       [-m modem-dev-path][-f logfile] [-h] [-t yes/no]\n"
		"Options:\n"
		"      -D      By default, atcmdsrv will not enable Diag;\n"
		"              Using this option enables Diag.\n\n"
		"      -l      Log level (same as log level:0-7, default: 5) \n"
		"	   -t	   Enable the local test or not(default:no)\n"
		"	   -m	   Modem dev path \n"
		"      -s      External serial port location \n"
		"      -f      Specify a log file to log to\n"
		"      -h      Print this message\n"
		"%s %s %s\n", ATCMDSRV_VERSION_STR, __DATE__, __TIME__ );
}

void handleArg(int argc, char * argv[] )
{
	int i;
	char * cp, opt;

	for (i = 1; i < argc; i++)
	{
		cp = argv[i];
		if (*cp == '-' || *cp == '/')
		{
			opt = *(++cp);

			switch (opt)
			{
			case 'd':
			case 'D':
			{

				if ( (i + 1)  < argc )
				{
					i++;
					if (strcasecmp("yes", argv[i]) == 0)
					{
						bLaunchDiag = TRUE;
					}
				}
				else
				{
					DPRINTF("Not enough argument.\n");
					exit(-1);
				}
				break;
			}
			case 't':
			case 'T':
			{

				if ( (i + 1)  < argc )
				{
					i++;
					if (strcasecmp("yes", argv[i]) == 0)
					{
						bLocalTest = TRUE;
					}
				}
				else
				{
					DPRINTF("Not enough argument.\n");
					exit(-1);
				}
				break;
			}
			case 's':
			case 'S':
			{
				if ( (i + 1)  < argc )
				{
					i++;
					sprintf(acExtSerialPath, "%s", argv[i]);
				}
				else
				{
					DPRINTF("Not enough argument.\n");
					exit(-1);
				}
				break;
			}
			case 'm':
			case 'M':
			{
				if ( (i + 1)  < argc )
				{
					i++;
					if (strcasecmp("yes", argv[i]) == 0)
					{
						bLaunchModem = TRUE;
					}
				}
				else
				{
					DPRINTF("Not enough argument.\n");
					exit(-1);
				}
				break;

			}
			case 'l':
			case 'L':
			{
				if ( (i + 1)  < argc )
				{
					i++;
					loglevel = atoi( argv[i]);
				}
				else
				{
					DPRINTF("Not enough argument.\n");
					exit(-1);
				}
				break;
			}
			case 'f':
			case 'F':
			{
				char filename[255];
				if ( (i + 1)  < argc )
				{
					i++;
					sprintf(filename, "%s", argv[i]);
					logfile = fopen(filename, "w");
					if (logfile == NULL)
					{
						DPRINTF( "can't open logfile `%s'\n", filename);
						exit(-1);
					}
				}
				else
				{
					DPRINTF("Not enough argument.\n");
					exit(-1);
				}
				break;
			}
			case 'q':
			{
				if ( (i + 1) < argc )
				{
					i++;
					if (atoi( argv[i]) == 1)
						giUseUlDlOpt =  TRUE;
				}
				else
				{
					DPRINTF("Not enough argument.\n");
					exit(-1);
				}
				break;
			}

			case '?':
			case 'H':
			case 'U':
			case 'h':
			case 'u':
				printUsage();
				exit(0);
			default:
				DPRINTF("Invalid argument %s.\n", cp);
				exit(-1);
			}       // end switch
		}               // end if
	}                       // end for
}

#ifndef BIONIC
void sig_action(int signum, siginfo_t *info, void *p)
{
	ERRMSG("at[%d]%s(%d, %p, %p)\n", getpid(), __func__, signum, info, p);
	ERRMSG("si_code = %d\n", info->si_code);
	switch (signum)
	{
	case SIGILL:
	case SIGFPE:
	case SIGSEGV:
	case SIGBUS:
		ERRMSG("si_addr = %p\n", info->si_addr);
		break;
	default:
		break;
	}

//	exit(-1);
}
#endif

#if 0
#define SCHED_YIELD     sched_yield()
#else
#define SCHED_YIELD \
	do { \
		struct timespec tm; \
		/* Linux 2.4 will busy loop for delays < 2ms on real time tasks */ \
		tm.tv_sec = 0; \
		tm.tv_nsec = 2000000L + 1; \
		nanosleep(&tm, NULL); \
	} while (0)
#endif

#if defined (BIONIC)
/*
 *  Switches UID to system, preserving following capabilities.
 *  1. CAP_NET_RAW, to use packet socket
 */
void switchUser()
{
	prctl(PR_SET_KEEPCAPS, 1, 0, 0, 0);
	setuid(AID_SYSTEM);

	struct __user_cap_header_struct header;
	struct __user_cap_data_struct cap;
	header.version = _LINUX_CAPABILITY_VERSION;
	header.pid = 0;
	cap.effective = cap.permitted = (1 << CAP_NET_RAW);
	cap.inheritable = 0;
	capset(&header, &cap);
}
#endif

/*******************************************************************
*  FUNCTION: main()
*
*  PARAMETERS:     None
*
*  DESCRIPTION:    Program Entry Point
*
*  RETURNS:
*
*******************************************************************/
int main(int argc, char * argv[] )
{

	BOOL bSuccess = FALSE;
#ifndef BIONIC
	int i;
	struct sigaction act;
#endif

#if defined (BIONIC)
	/* switch user to system in GUI mode */
	if(InProduction_Mode() != TRUE)
		switchUser();
#endif

#ifndef BIONIC
	/* set up the handler */
	act.sa_sigaction = sig_action;
	act.sa_flags = SA_SIGINFO;
	sigemptyset(&act.sa_mask);
	for (i = 1; i <= 31; i++)
		if (i != SIGALRM && i != SIGCHLD)
			sigaction(i, &act, NULL);
#endif

	#if defined (BIONIC)
	init_stdfiles();
	#endif

	// Setting Default values
	sprintf(acExtSerialPath, "%s", DEFAULT_EXT_SERIAL_DEVICE );
	sprintf(modemSerialPath, "%s", DEFAULT_EXT_SERIAL_DEVICE );

	/*
	 *  handle passing in arguments
	 */
	handleArg(argc, argv);
	DBGMSG("External Serial Deivce: %s\n.", acExtSerialPath);

	/*
	 * initialize OSA
	 */
	OSAInitialize();
	DPRINTF("OSA Initialized\n");
	diag_init();

#if 0
	/*
	 *  Initialize MSL
	 */

	if (!InitMsldlAl())
	{
		ASSERT(0);
	}

	if (TRUE != MslUtilsMemInit())
	{
		ASSERT(0);
	}

	INT32 mslStatus;
	MslDlStatusReq(&mslStatus);
	DPRINTF("MslDlStatusReq() return %d\n", mslStatus);
	while (mslStatus != MSLDL_CONNECTED)
	{
		MslDlStatusReq(&mslStatus);
		DBGMSG("MslDlStatusReq() return %d\n", mslStatus);
		SCHED_YIELD;

	}

	DPRINTF("msl connected!\n");
#endif

#if defined(ENABLE_DIAG)
	if (bLaunchDiag)
	{
		/* Initialize Diag Services */
		DBGMSG("DIAG: Initializing Diag Services\n");

		diagPhase1Init(/*DIAG_SERVER*/);
		DPRINTF("DIAG: diagPhase1Init() complete\n");
		diagPhase2Init();
		DPRINTF("DIAG: diagPhase2Init() complete\n");
	}
#endif
	/*
	 *  CI Initialization
	 */
	DBGMSG("Initializing CI Client Stub\n");
	if ((ciClientStubInit((CiClientStubStatusInd)CiInitCallBack)) == CISW_SUCCESS)
	{
		DBGMSG("Waiting initializing CI Client Stub.\n");
	}
	else
	{
		DPRINTF("ciClientStubInit() failed.\n");
	}

	/*
	 *  Initialize Telephony Controller
	 */
	bSuccess = tcInit(acExtSerialPath, modemSerialPath);

	if (bSuccess == FALSE )
	{
		DPRINTF("Problem Initializing TC.\n");
		exit(1);
	}

	unlink("/tmp/atcmdsrv_ok");

	if (bLaunchModem)
	{
		/* Initialize Modem Services */
		DBGMSG("MODEM: Initializing Modem Services\n");
		modem_init();
		DPRINTF("MODEM: Modem Init() complete\n");
	}

	while (gServiceGroupRegistered <= CI_SG_NUMIDS) SCHED_YIELD;

	DPRINTF("AT Commands Server is ready.\n");

	if( utlInitTimers() != utlSUCCESS)
	{
		DPRINTF("%s: utlInitTimers() failed\n", __FUNCTION__);
		exit(1);
	}

	/*--- main event loop ---*/
	if (utlEventLoop(true) != utlSUCCESS)
	{
		DPRINTF("%s: utlEventLoop() failed\n", __FUNCTION__);
	}

	F_LEAVE();

	return 0;
}

