/******************************************************************************
*(C) Copyright 2008 Marvell International Ltd.
* All Rights Reserved
******************************************************************************/
/*--------------------------------------------------------------------------------------------------------------------
 *  -------------------------------------------------------------------------------------------------------------------
 *
 *  Filename: eeh_main.c
 *
 *  Description: The main task to process error & exception of system.
 *
 *  History:
 *   May, 2008 - Rovin Yu Creation of file
 *
 *  Notes:
 *
 ******************************************************************************/

/******************************************************************************
*    Include files
******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <fcntl.h>
#include <stdlib.h>

#if defined (BIONIC)
#include <linux/capability.h>
#include <linux/prctl.h>
#include <private/android_filesystem_config.h>
#endif

#include "osa.h"

#include "pxa_dbg.h"

/* used for EEH NVM IPC */
#include "NVMServer_defs.h"
#include "NVMIPCClient.h"
#include "diag_al.h"
char log_tag[256] = "MARVELL_TEL_SM";
#if defined(ENABLE_EEH)
#include "eeh.h"
#endif

#include "eeh_assert_notify.h"

#if defined (BIONIC)
extern BOOL NVM_Init(void);
#endif

#ifdef __cplusplus
extern "C" {
#endif

void init_stdfiles();
int nanosleep(const struct timespec *requested_time, struct timespec *remaining);
void sig_action(int signum, siginfo_t *info, void *p);
#if !defined (BIONIC)
BOOL NVM_Init(void);
#endif

BOOL InProduction_Mode(void);

#ifdef __cplusplus
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

static void print_usage()
{
	printf(  "Error & Exception Handler Task\n"
		 "CopyRight (c) 2008 Marvell \n"
		 "Usage: eeh  [-h] [-D yes/no] [-M yes/no] [-L loglevel] [-f logfile]"
		 "Options:\n"
		 "    -h        Print this message\n"
		 "    -D		Enable DIAG or not. By default, eeh will not enable Diag\n"
		 "    -M		Dump 6M CP memory or not. By default, eeh will not dump 6M CP memory\n"
		 "    -L        debug level (same as log level:0-7, default: 5) \n"
		 "    -f        Specify a log file to log to\n"
		 "    -t        Heart-beating Timeout value to detect ATCMDSRV and MSL DRV activity(unit:second)\n");
}

static void handle_arg(int argc, char * argv[] )
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
			case 'D':
			{

				if ( (i + 1)  < argc )
				{
					i++;
					if (strcmp("yes", argv[i]) == 0)
					{
						b_launch_diag = 1;
					}
				}
				else
				{
					DPRINTF("Not enough argument.\n");
					exit(-1);
				}
				break;
			}
			case 'M':
			{

				if ( (i + 1)  < argc )
				{
					i++;
					if (strcmp("yes", argv[i]) == 0)
					{
						gDumpHugeMemory = 1;
						DPRINTF("gDumpHugeMemory: %d\n", gDumpHugeMemory);
					}
				}
				else
				{
					DPRINTF("Not enough argument.\n");
					exit(-1);
				}
				break;
			}
			case 'L':
			case 'l':
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
			case '?':
			case 'H':
			case 'U':
			case 'h':
			case 'u':
				print_usage();
				exit(0);
			default:
				DPRINTF("Invalid argument %s.\n", cp);
				exit(-1);
			}       // end switch
		}               // end if
	}                       // end for
}


void sig_action(int signum, siginfo_t *info, void *p)
{
	DPRINTF("eeh[%d]%s(%d, %p, %p)\n", getpid(), __func__, signum, info, p);
	diag_sig_action( signum,  info,  p);
	//exit(-1);
}

#if defined (BIONIC)
/*
 *  Switches UID to system, preserving following capabilities
 *  CAP_NET_RAW: create socket
 *  CAP_FOWNER/CAP_DAC_OVERRIDE/CAP_DAC_READ_SEARCH: enable diag to sdcard
 *	CAP_SYS_TIME : enable set RTC
 *  CAP_NET_ADMIN: enable config usb0 interface
 *  CAP_SYS_BOOT: enable reboot device
 */
static void switchUser()
{
	/*  add extra groups:
	 *  AID_LOG to read system logs
	 */
	gid_t groups[] = {AID_LOG};

	prctl(PR_SET_KEEPCAPS, 1, 0, 0, 0);
	setgroups(sizeof(groups)/sizeof(groups[0]), groups);
	setgid(AID_SYSTEM);
	setuid(AID_SYSTEM);

	struct __user_cap_header_struct header;
	struct __user_cap_data_struct cap;
	header.version = _LINUX_CAPABILITY_VERSION;
	header.pid = 0;
	cap.effective = cap.permitted = ((1 << CAP_NET_RAW) | (1 << CAP_FOWNER) | (1 << CAP_DAC_OVERRIDE) | (1 << CAP_DAC_READ_SEARCH) | (1 << CAP_SYS_TIME)
		| (1 << CAP_NET_ADMIN) | (1 << CAP_SYS_BOOT));
	cap.inheritable = 0;
	capset(&header, &cap);
}
#endif


/* main function */
int main(int argc, char *argv[])
{
#ifndef BIONIC
	int i;
#endif
	struct sigaction act;

#if defined (BIONIC)
	/* switch user to system in GUI mode */
	if(InProduction_Mode() != TRUE)
		switchUser();
#endif

#if defined (BIONIC)
	init_stdfiles();
#endif
	/*
	 *  handle passing in arguments
	 */
	handle_arg(argc, argv);
	/* set up the handler */
	act.sa_sigaction = sig_action;
	act.sa_flags = SA_SIGINFO;
	sigemptyset(&act.sa_mask);
#ifndef BIONIC
	for (i = 1; i <= 31; i++)
		if (i != SIGALRM && i != SIGCHLD)
			sigaction(i, &act, NULL);
#else
	// diag need to capture SIGPIPE and SIGTERM
	sigaction(SIGPIPE, &act, NULL);
	sigaction(SIGTERM, &act, NULL);
#endif

	/*
	 * initialize OSA
	 */
	OSAInitialize();

	if (!NVM_Init())
	{
		ERRMSG("Cannot initialize NVM proxy\n");
		exit(1);
	}

	DPRINTF("NVM initialized\n");

#if defined(ENABLE_EEH)
	if (EehInit() != EEH_SUCCESS)
	{
		DPRINTF("Cannot initialize Error Handler\n");
		exit(-1);
	}

	DPRINTF("EEH Task initialized\n");
#endif

	if (b_launch_diag)
	{
		/* Initialize Diag Services */
		diag_init();
		DPRINTF("DIAG: diag_init() complete\n");
	}

	for (;; )
		sleep(24 * 60 * 60);  //Sleep enough to save power

	return 0;
}

