/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/

/*********************************************************************
 *                      M O D U L E     B O D Y                       *
 **********************************************************************
 * Title: Diag                                                        *
 *                                                                    *
 * Filename: diag_mmi_if_OSA_LINUX.c                                     *
 *                                                                    *
 * Target, platform: Common Platform, SW platform                     *
 *                                                                    *
 * Authors: Shuki Zanyovka											 *
 *                                                                    *
 * Description: handles Linux NETLINK event for insertion/removal     *
 *              of MMC/SD cards										 *
 *                                                                    *
 * Notes:                                                             *
 **********************************************************************/

/************* General include definitions ****************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <termios.h>
#include <fcntl.h>
#include <errno.h>
#include <asm/types.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/netlink.h> //for Linux sockets
#include <sys/mount.h> //for SD card mount
#include <dirent.h>


#include "pxa_dbg.h"
//#include "osa.h"
//#include "global_types.h"
//#include "linux_types.h"

//#include "diag_osif.h"
#include "diag.h"

//#include "diag_mem.h" //for DiagAlignMalloc declaration
#include "diag_mmi_if_OSA_LINUX.h"
//#include "diag_mmi_if_PROTOCOL.h"
#if defined(K2UEVENT_LIB)
#include "k2uevent.h"
#endif

#if defined (ENABLE_DIAG_MMI_TEST)
extern void diagOverFSfilter(const char *filter_file);
#endif //ENABLE_DIAG_MMI_TEST

#define MAX_PATH_SZ                             512
#define MAX_EEH2DM_MESSAGE_SIZE         256
#define DIAG_MMI_IF_STACK_SIZE          2048
#define DIAG_MMI_IF_TASK_PRIORITY     80                /* set to low priority */

#define TRANSFER_BUFFER_SZ                              1024
#define DEV_NAME_MAX_SZ                                 256

#define MMI_MOUNT_SLEEP_TIME                    (300 * 1000)

#define IO_FLUSH_PENDING_BUFFERS _IOW('T', 99, int)
#define DIAGPRINT DPRINTF

#define MMC_PATH_NAME  "/dev/block"
#define MMC_MOUNT_PATH "/mnt/sdcard"
#define UNUSEDPARAM(param) (void)param;
static const char *MMI_IF_event_name[] =
{
	"NONE",
	"CARD_IN",
	"CARD_OUT",
	"REPORT_AUTO",
	"REPORT_STOP_DONE",
	"USER_START",
	"USER_STOP"
};

static const char *MMI_IF_state_name[] =
{
	"INIT",
	"INSERTED_STOPPED",
	"INSERTED_STARTED",
	"NOT_INSERTED_STOPPED",
	"NOT_INSERTED_STARTED",
	"MOUNTED_STOPPED",
	"MOUNTED_STARTED",
	"MOUNTING_MMI_STOPPED",
	"MOUNTING_MMI_STARTED",
	"UNMOUNTING_MMI_STOPPED",
	"UNMOUNTING_MMI_STARTED",
	"EXCEPTION_MMI_STOPPED",
	"EXCEPTION_MMI_STARTED"
};

static const char *MMI_FS_type_name[] =
{
	"NO_MOUNT",
	"vfat",
	"ext2",
	"ext3",
	"msdos"

};

static void MMI_NetlinkListenerTask(void *ptr) __attribute__((noreturn));
//static MMI_IF_Mount_Event_t MMI_AutodetectAndMountFSCard( char *device_name, const char *devpath );
static MMI_IF_Mount_Event_t MMI_AutodetectAndWaitMountFSCard( char *device_name, const char *devpath );
static void MMI_IF_state_machine( MMI_IF_Event_t event, const char *devpath );
static void MMI_IF_state_machine_low_level( MMI_IF_Event_t event, const char *devpath );
static void initFileLogDirNum();
static void initFSMountDirectory();
static void createCurrentLoggingPath( BOOL isInit );

#if defined(K2UEVENT_LIB)
void AddMMCCallBackfunction( __attribute__((unused)) CHAR* TransferBuffer,
			     __attribute__((unused)) int * ReadLen,
			     __attribute__((unused)) char * subsys,
			     __attribute__((unused)) char* usr_str_val,
			     __attribute__((unused)) char* action);

void RemoveMMCCallBackfunction( __attribute__((unused)) CHAR* TransferBuffer,
				__attribute__((unused)) int * ReadLen,
				__attribute__((unused)) char * subsys,
				__attribute__((unused)) char* driver,
				__attribute__((unused)) char* action);
#endif
static int MMI_control_fd = -1;
#if !defined(K2UEVENT_LIB)
static int diag_extIF_nl_sd = -1;                       /* file descriptor of netlink */
#endif
static int MMI_eeh_fd_read = -1;                        /* file descriptor of eeh */
static int MMI_eeh_fd_write = -1;                       /* file descriptor of eeh */

static MMI_START_CB_Func MMI_Start_CB_FuncPtr = NULL;
static MMI_STOP_CB_Func MMI_Stop_CB_FuncPtr = NULL;

static EActiveConnectionLL_Type File_Type;

static OSATaskRef diag_mmi_task_ref;

static unsigned int     *diag_mmi_if_task_stack;

//static pthread_t diag_SDcardTask;

static unsigned int *TransferBuffer;

static char devname[DEV_NAME_MAX_SZ];

static MMI_IF_state_t current_state = MMI_IF_STATE_INIT;

static OSSemaRef mmi_state_machine_sem = NULL;

static BOOL debug_prints_enabled = FALSE;


//static const char *DIAG_MMI_CARD_MOUNT_POINT = "/mnt/mmc";
//static const char *DIAG_MMI_CARD_MOUNT_POINT = "/marvell/tel/mmc";
static const char *DIAG_MMI_CARD_MOUNT_POINT = "/mnt/sdcard";

static char DIAG_MMI_FS_MOUNT_POINT[256] = "/data/acat";

static char RelPath2Log[MAX_PATH_SZ];
static char dirPath2Log[MAX_PATH_SZ];
static char dirPath2log_assert[MAX_PATH_SZ];
static char EEHTransferBuffer[MAX_EEH2DM_MESSAGE_SIZE];

// MMI private file name
static char FILE_LOGDIR[256];
//static char SOCKET_PATH[256];

//This folder shoud be set and created by the user forintegarte with their environment
//It assures that there is a place that is readable/writeable in their environment
//Since this module should be independent we ddid not import the varaible diag_marvell_rw_dir from diag_osif.c
static char *mmidiag_marvell_rw_dir;  //= getenv("MARVELL_RW_DIR");
static char *mmidiag_marvell_socket_dir;


//Global SD Variable for MMI Diag
static DiagBSPFSConfigS local_SD_FS_Config;

BOOL FS_Diag_Assert_Received = FALSE; //Workaround for assure that the queue is empty
BOOL assert_rename = FALSE;

extern FILE * popen ( const char * command , const char * type );
extern int pclose ( FILE * stream );

/***********************************************************************
* Function: MMI_IF_is_current_state_persistent                         *
************************************************************************
* Description:                                                                                                             *
*                                                                                  *
* Parameters:                                                          *
*                                                                                                                          *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
static BOOL MMI_IF_is_current_state_persistent( void )
{
	BOOL ret_val = FALSE;

	switch ( current_state )
	{
	case MMI_IF_STATE_INIT:
	case MMI_IF_STATE_MOUNTING_MMI_STOPPED:
	case MMI_IF_STATE_MOUNTING_MMI_STARTED:
	case MMI_IF_STATE_UNMOUNTING_MMI_STOPPED:
	case MMI_IF_STATE_UNMOUNTING_MMI_STARTED:
	case MMI_IF_STATE_INSERTED_STOPPED:
	case MMI_IF_STATE_INSERTED_STARTED:
		ret_val = FALSE;
		break;

	case MMI_IF_STATE_MOUNTED_STOPPED:
	case MMI_IF_STATE_MOUNTED_STARTED:
	case MMI_IF_STATE_NOT_INSERTED_STOPPED:
	case MMI_IF_STATE_NOT_INSERTED_STARTED:
	case MMI_IF_STATE_EXCEPTION_MMI_STOPPED:
	case MMI_IF_STATE_EXCEPTION_MMI_STARTED:
		ret_val = TRUE;
		break;

	default:
		ret_val = FALSE;
		break;
	}

	return ret_val;
}

/***********************************************************************
* Function: MMI_IF_state_machine                                       *
************************************************************************
* Description: State machine for the MMI IF                                                *
*                                                                                  *
* Parameters:                                                          *
*                                                                                                                          *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
static void MMI_IF_state_machine( MMI_IF_Event_t event, const char *devpath )
{
	UINT32 retries = 10;
	MMI_IF_state_t prev_state;

	DIAGPRINT(">> Enter MMI_IF_state_machine, event:%d devpath:%s\n",event, devpath);

	 // take semaphore,
	OSASemaphoreAcquire(mmi_state_machine_sem, OSA_SUSPEND);

	/* loop through all non-persistent states */
	do
	{
		prev_state = current_state;

		DIAGPRINT(">> Current State:%d\n", current_state);

		MMI_IF_state_machine_low_level( event, devpath );

		/* set to no event */
		event = MMI_IF_EVENT_NONE;

		/* if previous state == current state => the state machine might be stuck! */
		if ( prev_state == current_state )
		{
			/* retries in order to not get stuck inside a non-persistent state */
			retries--;
		}
	} while ( (MMI_IF_is_current_state_persistent() == FALSE) && (retries != 0) );

	// if all was sent ok - let release semaphore
	OSASemaphoreRelease(mmi_state_machine_sem);
}

/***********************************************************************
* Function: MMI_IF_state_machine_low_level                             *
************************************************************************
* Description: State machine for the MMI IF                                                *
*                                                                                  *
* Parameters:                                                          *
*                                                                                                                          *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
static void MMI_IF_state_machine_low_level( MMI_IF_Event_t event, const char *devpath )
{
	UNUSEDPARAM(devpath)
	DIAGPRINT(">> Enter MMI_IF_state_machine_low_level, current state: %d,event:%d devpath:%s\n",current_state,event, devpath);
	MMI_IF_Mount_Event_t mount_result;

	if ( debug_prints_enabled != FALSE )
	{
		fprintf( stderr, "MMI_IF_state_machine_low_level() ENTRY_STATE: STATE(%d)=%s, EVENT(%d)=%s\n",
			 current_state, MMI_IF_state_name[current_state], event, MMI_IF_event_name[event]);
	}

	switch ( current_state )
	{
	case MMI_IF_STATE_INIT:
		/* Auto detect and mount card */
		if (local_SD_FS_Config.SD_FSType != FSTYPE_NO_MOUNT)
		{
			mount_result = MMI_AutodetectAndWaitMountFSCard( devname, NULL );
		}
		else
		{
			mount_result = MMI_IF_MOUNT_EVENT_CARD_MOUNTED;
		}
		switch (mount_result)
		{
		case MMI_IF_MOUNT_EVENT_CARD_DETECTED:
			current_state = MMI_IF_STATE_INSERTED_STOPPED;
			break;

		case MMI_IF_MOUNT_EVENT_CARD_MOUNTED:
			fprintf(stderr, "DIAG_MMI_IF: MMI card DETECTED at startup\n");
			/* set next state to NOT inserted and STOPPED */
			current_state = MMI_IF_STATE_MOUNTED_STOPPED;
			break;

		case MMI_IF_MOUNT_EVENT_CARD_ERROR:
		case MMI_IF_MOUNT_EVENT_CARD_NOT_DETECTED:
		default:
			fprintf(stderr, "DIAG_MMI_IF: No MMI card at startup\n");
			/* set next state to mounting MMI stopped */
			current_state = MMI_IF_STATE_NOT_INSERTED_STOPPED;
			break;

		}

		break;

	case MMI_IF_STATE_INSERTED_STOPPED:
		/* set next state to mounting MMI stopped */
		current_state = MMI_IF_STATE_MOUNTING_MMI_STOPPED;
		break;

	case MMI_IF_STATE_INSERTED_STARTED:
		/* set next state to mounting MMI started */
		current_state = MMI_IF_STATE_MOUNTING_MMI_STARTED;
		break;

	case MMI_IF_STATE_NOT_INSERTED_STOPPED:
		/* Act according to event */
		switch ( event )
		{
		case MMI_IF_EVENT_CARD_IN:
			/* set next state */
			current_state = MMI_IF_STATE_MOUNTING_MMI_STOPPED;
			break;

		case MMI_IF_EVENT_REPORT_AUTO:
		case MMI_IF_EVENT_USER_START:
			/* set next state */
			current_state = MMI_IF_STATE_NOT_INSERTED_STARTED;
			break;

		default:
			/* don't change state */
			break;
		}

		break;

	case MMI_IF_STATE_NOT_INSERTED_STARTED:
		/* Act according to event */
		switch ( event )
		{
		case MMI_IF_EVENT_CARD_IN:
			/* set next state */
			current_state = MMI_IF_STATE_MOUNTING_MMI_STARTED;
			break;

		case MMI_IF_EVENT_REPORT_STOP_DONE:
		case MMI_IF_EVENT_USER_STOP:
			/* set next state */
			current_state = MMI_IF_STATE_NOT_INSERTED_STOPPED;
			break;

		default:
			/* don't change state */
			break;
		}

		break;

	case MMI_IF_STATE_MOUNTED_STOPPED:
		/* Act according to event */
		switch ( event )
		{
		case MMI_IF_EVENT_CARD_OUT:
			/* set next state */
			current_state = MMI_IF_STATE_UNMOUNTING_MMI_STOPPED;
			break;

		case MMI_IF_EVENT_REPORT_AUTO:
		case MMI_IF_EVENT_USER_START:
			/* Notify DIAG to start logging */
			if ( MMI_Start_CB_FuncPtr != NULL )
			{
				/* create current directory to log to in case it doesn't exist */
				if (local_SD_FS_Config.FS_Inc_Reset)
				{
					createCurrentLoggingPath( FALSE );
				}
				else
				{
					createCurrentLoggingPath( TRUE );
				}

				/* Notify DIAG that it should start logging since card was inserted + logging enabled */
				MMI_Start_CB_FuncPtr(dirPath2Log);
			}

			/* set next state */
			current_state = MMI_IF_STATE_MOUNTED_STARTED;
			break;

		default:
			/* don't change state */
			break;
		}

		break;

	case MMI_IF_STATE_MOUNTED_STARTED:
		/* Act according to event */
		switch ( event )
		{
		case MMI_IF_EVENT_CARD_OUT:
		case MMI_IF_EVENT_REPORT_STOP_DONE:
			if (assert_rename)
			{
				//rename the dirPath2Log to dirPath2Log_assert
				sprintf(dirPath2log_assert, "%s_%s", dirPath2Log, "ASSERT");
				if (rename(dirPath2Log, dirPath2log_assert) == -1)
				{
					fprintf(stderr, "***** DIAG DIAG MMI_IF can not rename folder on ASSERT %s *****\r\n", strerror(errno));
				}
				else
				{
					fprintf(stderr, "***** DIAG DIAG MMI_IF rename folder to %s *****\r\n", dirPath2log_assert);
				}
				assert_rename = FALSE;
			}
			/* set next state */
			if ( event == MMI_IF_EVENT_CARD_OUT )
			{
				current_state = MMI_IF_STATE_UNMOUNTING_MMI_STARTED;
			}
			else
			{
				current_state = MMI_IF_STATE_MOUNTED_STOPPED;
			}
			break;
		case MMI_IF_EVENT_USER_STOP:
			if (FS_Diag_Assert_Received)
			{
				assert_rename = TRUE;
			}
			/* Notify DIAG that it should stop logging into MMI card - no need to check if it's enabled or disabled */
			if ( MMI_Stop_CB_FuncPtr != NULL )
			{
				MMI_Stop_CB_FuncPtr();
			}

			break;

		default:
			/* don't change state */
			break;
		}

		break;

	case MMI_IF_STATE_MOUNTING_MMI_STOPPED:
		/* Auto detect and mount card */
		if (local_SD_FS_Config.SD_FSType != FSTYPE_NO_MOUNT)
		{
			sleep(3);
			mount_result = MMI_AutodetectAndWaitMountFSCard( devname, NULL );
		}
		else
		{
			mount_result = MMI_IF_MOUNT_EVENT_CARD_MOUNTED;
		}

		switch (mount_result)
		{
		case MMI_IF_MOUNT_EVENT_CARD_MOUNTED:
			/* set next state to NOT inserted and STOPPED */
			current_state = MMI_IF_STATE_MOUNTED_STOPPED;
			break;

		case MMI_IF_MOUNT_EVENT_CARD_DETECTED:
		case MMI_IF_MOUNT_EVENT_CARD_ERROR:
		case MMI_IF_MOUNT_EVENT_CARD_NOT_DETECTED:
		default:
			/* unmount MMI card - just in case */
			if (local_SD_FS_Config.SD_FSType != FSTYPE_NO_MOUNT)
			{
				umount(DIAG_MMI_CARD_MOUNT_POINT);
			}

			/* set next state to EXCEPTION */
			current_state = MMI_IF_STATE_EXCEPTION_MMI_STOPPED;
			break;
		}
		break;

	case MMI_IF_STATE_MOUNTING_MMI_STARTED:
		/* Auto detect and mount card */
		if (local_SD_FS_Config.SD_FSType != FSTYPE_NO_MOUNT)
		{
			sleep(3);
			mount_result = MMI_AutodetectAndWaitMountFSCard( devname, NULL );
		}
		else
		{
			mount_result = MMI_IF_MOUNT_EVENT_CARD_MOUNTED;
		}

		switch (mount_result)
		{
		case MMI_IF_MOUNT_EVENT_CARD_MOUNTED:
			/* Notify DIAG to start logging */
			if ( MMI_Start_CB_FuncPtr != NULL )
			{
				/* create current directory to log to in case it doesn't exist */
				if (local_SD_FS_Config.FS_Inc_Reset)
				{
					createCurrentLoggingPath( FALSE );
				}
				else
				{
					createCurrentLoggingPath( TRUE );
				}

				/* Notify DIAG that it should start logging since card was inserted + logging enabled */
				MMI_Start_CB_FuncPtr(dirPath2Log);
			}

			/* set next state to NOT inserted and STOPPED */
			current_state = MMI_IF_STATE_MOUNTED_STARTED;
			break;

		case MMI_IF_MOUNT_EVENT_CARD_DETECTED:
		case MMI_IF_MOUNT_EVENT_CARD_ERROR:
		case MMI_IF_MOUNT_EVENT_CARD_NOT_DETECTED:
		default:
			/* unmount MMI card - just in case */
			if (local_SD_FS_Config.SD_FSType != FSTYPE_NO_MOUNT)
			{
				umount(DIAG_MMI_CARD_MOUNT_POINT);
			}

			/* set next state to mounting MMI stopped */
			current_state = MMI_IF_STATE_EXCEPTION_MMI_STARTED;
			break;
		}
		break;

	case MMI_IF_STATE_UNMOUNTING_MMI_STOPPED:
		/* unmount MMI card */
		if (local_SD_FS_Config.SD_FSType != FSTYPE_NO_MOUNT)
		{
			umount(DIAG_MMI_CARD_MOUNT_POINT);
		}

		/* set next state  */
		current_state = MMI_IF_STATE_NOT_INSERTED_STOPPED;

		break;

	case MMI_IF_STATE_UNMOUNTING_MMI_STARTED:
		/* unmount MMI card */
		if (local_SD_FS_Config.SD_FSType != FSTYPE_NO_MOUNT)
		{
			umount(DIAG_MMI_CARD_MOUNT_POINT);
		}

		/* set next state  */
		current_state = MMI_IF_STATE_NOT_INSERTED_STARTED;

		break;

	case MMI_IF_STATE_EXCEPTION_MMI_STOPPED:
		/* Act according to event */
		switch ( event )
		{
		case MMI_IF_EVENT_CARD_IN:
			/* set next state */
			current_state = MMI_IF_STATE_INSERTED_STOPPED;
			break;

		case MMI_IF_EVENT_CARD_OUT:
			/* set next state */
			current_state = MMI_IF_STATE_NOT_INSERTED_STOPPED;
			break;

		case MMI_IF_EVENT_REPORT_AUTO:
		case MMI_IF_EVENT_USER_START:
			/* set next state */
			current_state = MMI_IF_STATE_EXCEPTION_MMI_STARTED;
			break;

		case MMI_IF_EVENT_REPORT_STOP_DONE:
		case MMI_IF_EVENT_USER_STOP:
		default:
			/* don't change state */
			break;
		}

		break;

	case MMI_IF_STATE_EXCEPTION_MMI_STARTED:
		/* Act according to event */
		switch ( event )
		{
		case MMI_IF_EVENT_CARD_IN:
			/* set next state */
			current_state = MMI_IF_STATE_INSERTED_STARTED;
			break;

		case MMI_IF_EVENT_CARD_OUT:
			/* set next state */
			current_state = MMI_IF_STATE_NOT_INSERTED_STARTED;
			break;

		case MMI_IF_EVENT_REPORT_STOP_DONE:
		case MMI_IF_EVENT_USER_STOP:
			/* set next state */
			current_state = MMI_IF_STATE_EXCEPTION_MMI_STOPPED;
			break;

		case MMI_IF_EVENT_REPORT_AUTO:
		case MMI_IF_EVENT_USER_START:
		default:
			/* don't change state */
			break;
		}

		break;

	default:
		fprintf(stderr, "DIAG_MMI_IF: Unknown state, moving to initial state!\n");

		/* set next state */
		current_state = MMI_IF_STATE_INIT;
		break;
	}


	if ( debug_prints_enabled != FALSE )
	{
		fprintf( stderr, "MMI_IF_state_machine_low_level() EXIT_STATE: STATE(%d)=%s, EVENT(%d)=%s\n",
			 current_state, MMI_IF_state_name[current_state], event, MMI_IF_event_name[event]);
	}
}
#if !defined(K2UEVENT_LIB)
/***********************************************************************
* Function: openNetlink                                                *
************************************************************************
* Description:                                                         *
* Parameters:                                                          *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
static void openNetlink(void)
{
	int fd;
	struct sockaddr_nl snl;

	memset(&snl, 0, sizeof(snl));
	snl.nl_family = AF_NETLINK;
	snl.nl_pid = getpid();
	snl.nl_groups = 1;

	fprintf(stderr, "***** DIAG over FS: MMI openNetlink *****\r\n");
	DIAGPRINT("***** DIAG over FS: MMI openNetlink *****\r\n");
	fd = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
	if (fd < 0)
	{
		fprintf(stderr, "***** DIAG over FS: MMI socket NETLINK_KOBJECT_UEVENT Error: %s *****\r\n", strerror(errno));
		DIAGPRINT("***** DIAG over FS: MMI socket NETLINK_KOBJECT_UEVENT Error: %s *****\r\n", strerror(errno));
		return;
	}

	if (bind(fd, (struct sockaddr *)&snl, sizeof(struct sockaddr_nl)) < 0)
	{
		fprintf(stderr, "***** DIAG over FS: MMI bind NETLINK_KOBJECT_UEVENT Error: %s *****\r\n", strerror(errno));
		DIAGPRINT("***** DIAG over FS: MMI bind NETLINK_KOBJECT_UEVENT Error: %s *****\r\n", strerror(errno));
		close(fd);
		return;
	}

	diag_extIF_nl_sd = fd;
}
#endif


void openEehReadSocket(void)
{
#if 0
	int len, socket_fd1;
	struct sockaddr_un local;

	socket_fd1 = socket(PF_LOCAL, SOCK_DGRAM, 0);
	if ( socket_fd1 == -1)
	{
		fprintf(stderr, "***** DIAG DIAG MMI_IF openEehSocket: Socket Error: %s *****\r\n", strerror(errno));
		return;
	}

	if (mmidiag_marvell_socket_dir)
	{
		sprintf(SOCKET_PATH, "%s/%s", mmidiag_marvell_socket_dir, EEH_LM_SOCKET_NAME_ONLY);
		unlink(SOCKET_PATH);
	}
	else
	{
		/* unlink to make sure the bind operation won't fail */
		unlink(EEH_LM_SOCKET_FILENAME);
	}

	local.sun_family = AF_LOCAL;
	if (mmidiag_marvell_socket_dir)
	{
		sprintf(SOCKET_PATH, "%s/%s", mmidiag_marvell_socket_dir, EEH_LM_SOCKET_NAME_ONLY);
		strcpy(local.sun_path, SOCKET_PATH);
	}
	else
	{

		strcpy(local.sun_path, EEH_LM_SOCKET_FILENAME);
	}

	len = strlen(local.sun_path) + sizeof(local.sun_family);
	if (bind(socket_fd1, (struct sockaddr *)&local, len) == -1)
	{
		fprintf(stderr, "***** DIAG over FS  openEehSocket: Bind Error: %s *****\r\n", strerror(errno));
		close(socket_fd1);
		return;
	}



	fprintf(stderr, "***** DIAG over FS: was able to bind EEH socket ***** \n");
	fprintf(stderr, "***** DIAG over FS: MMI_eeh_fd_read  = %d ***** \n", socket_fd1);

	MMI_eeh_fd_read = socket_fd1;
#endif
}

void openEehwriteSocket(void)
{
#if 0
	int len, socket_fd2;
	struct sockaddr_un local;

	socket_fd2 = socket(PF_LOCAL, SOCK_DGRAM, 0);
	if (socket_fd2 == -1)
	{
		fprintf(stderr, "***** DIAG over FS openEehwriteSocket: Socket Error: %s *****\r\n", strerror(errno));
		return;
	}


	local.sun_family = AF_LOCAL;
	if (mmidiag_marvell_socket_dir)
	{
		sprintf(SOCKET_PATH, "%s/%s", mmidiag_marvell_socket_dir, EEH_LM_SOCKET_NAME_ONLY);
		strcpy(local.sun_path, SOCKET_PATH);
	}
	else
	{
		strcpy(local.sun_path, LM_EEH_SOCKET_FILENAME);
	}

	len = strlen(local.sun_path) + sizeof(local.sun_family);


	if (connect(socket_fd2, (struct sockaddr *)&local, len) == -1)
	{
		fprintf(stderr, "***** DIAG over FS  openEehwriteSocket: Bind Error: %s *****\r\n", strerror(errno));
		close(socket_fd2);
		return;
	}

	MMI_eeh_fd_write = socket_fd2;

	fprintf(stderr, "***** DIAG over FS : was able to bind EEH socket ***** \n");
	fprintf(stderr, "***** DIAG over FS : MMI_eeh_fd_write  = %d ***** \n", socket_fd2);
#endif

}
#if 0
/***********************************************************************
* Function: MMI_AutodetectAndMountFSCard                                   *
************************************************************************
* Description: Initialize the MMI detection code					   *
*                                                                                                                                          *
* Parameters:                                                                                                      *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
static MMI_IF_Mount_Event_t MMI_AutodetectAndMountFSCard( char *device_name, const char *devpath )
{
	DIR *dirp;
	struct dirent *dp;
	MMI_IF_Mount_Event_t ret_val = MMI_IF_MOUNT_EVENT_CARD_NOT_DETECTED;
	int RetryCount = 10;

	DIAGPRINT(">> MMI_AutodetectAndMountFSCard enter!\n");

	if (local_SD_FS_Config.SD_FSType == 2 || local_SD_FS_Config.SD_FSType == 3)
	{
		fprintf(stderr, "*******************************************************************\r\n");
		fprintf(stderr, "***** DIAG over FS: The %s file system is not supported !!! *****\r\n", MMI_FS_type_name[local_SD_FS_Config.SD_FSType]);
		fprintf(stderr, "*******************************************************************\r\n");
		return MMI_IF_MOUNT_EVENT_CARD_ERROR;
	}
	do {
		/* do we know the device path? */
		if ( devpath != NULL )
		{
			int mnt_stat;
			fprintf(stderr, "***** DIAG over FS: Mounting  device: %s *****\r\n", devpath);

			DIAGPRINT(">>Enter known MMC device:%s\n", devpath);

			/* 300 mSec sleep */
			usleep( MMI_MOUNT_SLEEP_TIME );

			/* copy name of detected MMC device */
			if ( (mnt_stat = mount(devpath, DIAG_MMI_CARD_MOUNT_POINT, MMI_FS_type_name[local_SD_FS_Config.SD_FSType], MS_MGC_VAL, "")) < 0 )
			{
				fprintf(stderr, "******************************************************************\r\n");
				fprintf(stderr, "***** DIAG over FS: MMI cannot mount %s!!!                 *****\r\n", MMI_FS_type_name[local_SD_FS_Config.SD_FSType]);
				fprintf(stderr, "***** DIAG over FS: trying to umount /mnt/SD1 first!!!       *****\r\n");
				fprintf(stderr, "***** DIAG over FS: and then will try to mount %s again!!! *****\r\n", MMI_FS_type_name[local_SD_FS_Config.SD_FSType]);
				fprintf(stderr, "******************************************************************\r\n");
				umount(DIAG_MMI_CARD_MOUNT_POINT);
				sleep(1);
				if ( (mnt_stat = mount(devpath, DIAG_MMI_CARD_MOUNT_POINT, MMI_FS_type_name[local_SD_FS_Config.SD_FSType], MS_MGC_VAL, "")) < 0 )
				{
					devname[0] = 0;
					fprintf(stderr, "***** DIAG over FS: MMI mount failed! Error: %d %s *****\r\n", errno, strerror(errno));
					ret_val = MMI_IF_MOUNT_EVENT_CARD_ERROR;
				}
			}
			if (mnt_stat != -1)
			{
				fprintf( stderr, "***** DIAG over FS: Mount successful *****\r\n" );
				strcpy(devname, DIAG_MMI_CARD_MOUNT_POINT);

				/* set card MOUNTED */
				ret_val = MMI_IF_MOUNT_EVENT_CARD_MOUNTED;
			}
		}
		else   /* auto search MMC device */
		{
			DIAGPRINT(">>Enter auto search MMC device\n");

			if ((dirp = opendir(MMC_PATH_NAME)) == NULL)
			{
				fprintf(stderr, "***** DIAG over MMI: MMI could not open /dev Error: %s *****\r\n", strerror(errno));
				return ret_val;
			}
			else
			{
				devname[0] = 0;
				do
				{
					if (( dp = readdir(dirp)) != NULL)
					{
						if (strncmp(dp->d_name, "mmc", 3) != 0)
						{
							//fprintf(stderr,"***** DIAG over FS: MMI could not find /dev/mmc* file: %s *****\r\n", dp->d_name);
							continue;
						}

						fprintf(stderr, "***** DIAG over FS: MMI %s *****\r\n", dp->d_name);

						/* set card DETECTED */
						ret_val = MMI_IF_MOUNT_EVENT_CARD_DETECTED;

						/* copy name of detected MMC device */
						sprintf(device_name, "%s/%s", MMC_PATH_NAME, dp->d_name);
						DIAGPRINT(">> DIAG: deviceName:%s\n", device_name);

						fprintf( stderr, "***** DIAG over FS: device_name=%s *****\r\n", device_name );
						if ( mount(device_name, DIAG_MMI_CARD_MOUNT_POINT, MMI_FS_type_name[local_SD_FS_Config.SD_FSType], MS_MGC_VAL, "") < 0 )
						{
							fprintf(stderr, "******************************************************************\r\n");
							fprintf(stderr, "***** DIAG over FS: MMI cannot mount %s!!!                 *****\r\n", MMI_FS_type_name[local_SD_FS_Config.SD_FSType]);
							fprintf(stderr, "***** DIAG over FS: trying to umount /mnt/SD1 first!!!       *****\r\n");
							fprintf(stderr, "***** DIAG over FS: and then will try to mount %s again!!! *****\r\n", MMI_FS_type_name[local_SD_FS_Config.SD_FSType]);
							fprintf(stderr, "******************************************************************\r\n");
							umount(DIAG_MMI_CARD_MOUNT_POINT);
							sleep(1);
							if ( mount(device_name, DIAG_MMI_CARD_MOUNT_POINT, MMI_FS_type_name[local_SD_FS_Config.SD_FSType], MS_MGC_VAL, "") < 0 )
							{
								devname[0] = 0;
								fprintf(stderr, "***** DIAG over FS: MMI mount failed! Error: %d %s *****\r\n", errno, strerror(errno));
								continue;
							}
						}
						fprintf( stderr, "***** DIAG over FS: Mount successful *****\r\n" );
						DIAGPRINT(">> DIAG: Mount successful\n");
						strcpy(devname, DIAG_MMI_CARD_MOUNT_POINT);

						/* set card MOUNTED */
						ret_val = MMI_IF_MOUNT_EVENT_CARD_MOUNTED;

						break;
					}
				} while (dp != NULL);
				if (dp == NULL) DIAGPRINT(">> Mount failed \n");
				if (closedir(dirp) < 0)
				{
					fprintf(stderr, "***** DIAG over MMI: closedir Error: %s *****\r\n", strerror(errno));
				}
			}
		}
		if (ret_val != MMI_IF_MOUNT_EVENT_CARD_MOUNTED && RetryCount)
			sleep(2);
	} while (ret_val != MMI_IF_MOUNT_EVENT_CARD_MOUNTED && RetryCount--);

	return ret_val;
}
#endif
/***********************************************************************
* Function: MMI_AutodetectAndWaitMountFSCard                                   *
************************************************************************
* Description: Initialize the MMI mount detection code. Execute "mount" cmd and check if the "/mnt/sdcard" is in output list.					   *
*                                                                                                                                          *
* Parameters:                                                                                                      *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
static MMI_IF_Mount_Event_t MMI_AutodetectAndWaitMountFSCard( char *device_name, const char *devpath )
{
	UNUSEDPARAM(device_name)
	UNUSEDPARAM(devpath)

	FILE * fd;
	char buf[256];

	MMI_IF_Mount_Event_t ret_val = MMI_IF_MOUNT_EVENT_CARD_NOT_DETECTED;
	int RetryCount = 60;

	DIAGPRINT(">> MMI_AutodetectAndWaitMountFSCard enter!\n");

	if (local_SD_FS_Config.SD_FSType == 2 || local_SD_FS_Config.SD_FSType == 3)
	{
		fprintf(stderr, "*******************************************************************\r\n");
		fprintf(stderr, "***** DIAG over FS: The %s file system is not supported !!! *****\r\n", MMI_FS_type_name[local_SD_FS_Config.SD_FSType]);
		fprintf(stderr, "*******************************************************************\r\n");
		return MMI_IF_MOUNT_EVENT_CARD_ERROR;
	}
	do {

		DIAGPRINT(">>Enter auto search /mnt/sdcard\n");
		fd = popen("mount","r");
		if(fd== NULL)
		{
			DIAGPRINT("*****DIAG over FS:exec mount failure!\n");
			break;
		}
		while(fgets(buf, 256, fd))
		{
			if(strstr(buf, MMC_MOUNT_PATH) != NULL)
			{
				ret_val = MMI_IF_MOUNT_EVENT_CARD_MOUNTED;
				DIAGPRINT("*****DIAG over FS:/mnt/sdcard has been found!\n");
				break;
			}
		}
		pclose(fd);
		if (ret_val != MMI_IF_MOUNT_EVENT_CARD_MOUNTED && RetryCount)
			sleep(2);
	} while (ret_val != MMI_IF_MOUNT_EVENT_CARD_MOUNTED && RetryCount--);

	return ret_val;
}
/***********************************************************************
* Function: MMI_Init	                                               *
************************************************************************
* Description: Initialize the MMI detection code					   *
*                                                                                                                                          *
* Parameters:                                                                                                      *
*                               MMI_Start_CB - DIAG start logging callback function    *
*                               MMI_Stop_CB - DIAG Stop logging callback function      *
*               LL_Type - tLL_FS (file system) or tLL_SC (storage card)*
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
BOOL MMI_Init(MMI_START_CB_Func MMI_Start_CB, MMI_STOP_CB_Func MMI_Stop_CB, EActiveConnectionLL_Type LL_Type, DiagBSPFSConfigS* p_FSConfig )
{
	OSA_STATUS status = OS_SUCCESS;

	/* initialize state machine */
	current_state = MMI_IF_STATE_INIT;

	fprintf(stderr, "DIAG_MMI_IF init started!\n");
	DIAGPRINT(">>DIAG_MMI_IF init started!\n");

	//Check if MARVELL_RW_DIR variable is set get its vakue
	mmidiag_marvell_rw_dir = getenv("MARVELL_RW_DIR");
	mmidiag_marvell_socket_dir = getenv("MARVELL_SOCKET_DIR");

	//Init global SD variable
	memcpy(&local_SD_FS_Config, p_FSConfig, sizeof(DiagBSPFSConfigS));

	//Debug - Should Remove
	DIAGPRINT("DIAG MMI - Local Config Received at Init : IncReset = %d \n", local_SD_FS_Config.FS_Inc_Reset );
	DIAGPRINT("DIAG MMI - Local Config Received at Init : FsType = %d \n", local_SD_FS_Config.SD_FSType );


	initFileLogDirNum();
	// create the semaphore to control transmit from different tasks (tx-task, hand-shake)
	status = OSASemaphoreCreate(&mmi_state_machine_sem, 1, OSA_FIFO);
	if ( status != OS_SUCCESS )
	{
		fprintf(stderr, "DIAG_MMI_IF init failed - can't create semaphore!\n");

		/* CB registration error */
		return FALSE;
	}

	//Only make the dir if diag mounting is enabled.
	if (LL_Type == tLL_SC)
	{
		fprintf(stderr, "***** DIAG over FS: MMI mkdir *****\r\n");
		if ( mkdir( DIAG_MMI_CARD_MOUNT_POINT, 0x666 ) < 0 )
		{
			fprintf(stderr, "***** DIAG over MMI: MMI mkdir failed! Error: %s *****\r\n", strerror(errno));


		}

	}

	/* initialize Active Connection type */
	File_Type = LL_Type;

	/* sanity check */
	if ( (MMI_Start_CB == NULL) || (MMI_Stop_CB == NULL) )
	{
		fprintf(stderr, "DIAG_MMI_IF init failed - bad callback function pointers!\n");

		/* CB registration error */
		return FALSE;
	}

	/* assign CB function pointers */
	MMI_Start_CB_FuncPtr = MMI_Start_CB;
	MMI_Stop_CB_FuncPtr = MMI_Stop_CB;

	/* start listening on MMI control socket */
	//openControlSocket();

	if ( MMI_control_fd < 0 )
	{
		fprintf(stderr, "***** DIAG over FS: MMI control socket init failed! Error: %s *****\r\n", strerror(errno));
		//return FALSE;
	}

	/* Initialize the state machine */
	/* check file type */
	if ( tLL_SC == File_Type )
	{
#if defined(K2UEVENT_LIB)
		RegisterForNotification( AddMMCCallBackfunction,        KOBJ_ADD,               "DEVPATH",      NULL, "mmc");
		RegisterForNotification( RemoveMMCCallBackfunction,     KOBJ_REMOVE,    "DEVPATH",      NULL, "mmc");
#else
		/* start listening on Netlink*/
		openNetlink();
		DIAGPRINT(">> MMI_Init() diag_extIF_nl_sd:%d\n", diag_extIF_nl_sd);
		if ( diag_extIF_nl_sd < 0 )
		{
			fprintf(stderr, "***** DIAG over FS: MMI Netlink init failed! Error: %s *****\r\n", strerror(errno));
			return FALSE;
		}
#endif
		/* Update current path with FS mount point */
		strcpy( RelPath2Log, DIAG_MMI_CARD_MOUNT_POINT );

		/* Set the current path to log to */
		createCurrentLoggingPath( TRUE );

		/* call state machine with init state and NO event for the initial detection */
		MMI_IF_state_machine( MMI_IF_EVENT_NONE, NULL );
	}
	else
	{

		/* initialize state machine to MOUNTED + STOPPED - since the file system is *always* mounted */
		current_state = MMI_IF_STATE_MOUNTED_STOPPED;

		initFSMountDirectory();
		/* Update current path with FS mount point */
		strcpy( RelPath2Log, DIAG_MMI_FS_MOUNT_POINT );

		/* Set the current path to log to */
		createCurrentLoggingPath( TRUE );

		/* Notify DIAG to start logging */
		//if ( MMI_Start_CB_FuncPtr != NULL )
		//{
		//	MMI_Start_CB_FuncPtr(dirPath2Log);
		//}
	}

	//openEehReadSocket();
	if ( MMI_eeh_fd_read < 0 )
	{
		fprintf(stderr, "***** DIAG over FS: MMI EEH init failed! Error: %s *****\r\n", strerror(errno));
		//return FALSE;
	}

	/* Create the Netlink listener thread */
#if 1
	/* shukiz: For now it's allocated from DIAG. should be allocated from another pool */
	diag_mmi_if_task_stack = (unsigned int *)malloc(DIAG_MMI_IF_STACK_SIZE);
	DIAG_ASSERT(diag_mmi_if_task_stack != 0);

	/* shukiz: For now it's allocated from DIAG. should be allocated from another pool */
	TransferBuffer = (unsigned int *)malloc(TRANSFER_BUFFER_SZ);
	DIAG_ASSERT(TransferBuffer != 0);

	// Rx task - in WinCE, LINUX  (not Nucleus)
	fprintf(stderr, "***** DIAG over FS: MMI MMI_NetlinkListenerTask *****\r\n");
	if (diag_mmi_task_ref == NULL)
	{
		status = OSATaskCreate(
			&diag_mmi_task_ref,                                             /* OS task reference                       */
			diag_mmi_if_task_stack,                                         /* pointer to start of task stack area     */
			DIAG_MMI_IF_STACK_SIZE,                                         /* number of bytes in task stack area      */
			DIAG_MMI_IF_TASK_PRIORITY,                                      /* task priority                           */
			(CHAR *)"diagMMI",                                              /* task name                               */
			MMI_NetlinkListenerTask,                                        /* pointer to task entry point        */
			0                                                               /* task entry argument pointer             */
			);
		if ( status != OS_SUCCESS )
		{
			fprintf(stderr, "***** DIAG over FS: MMI Unable to create task! Error: %s *****\r\n", strerror(errno));
			DIAGPRINT(">> Create diagMMI failed\n");
			return FALSE;
		}
	}
#else

	if ( pthread_create(&diag_SDcardTask, NULL, (void *)MMI_NetlinkListenerTask, NULL) != 0 )
	{
		DIAGPRINT(">> pthread_create diag_SDcardTask error");
		return FALSE;
	}
#endif
	DIAGPRINT(">> Create diagMMI successful\n");

	/* start the thread */
	return TRUE;
}
#if !defined(K2UEVENT_LIB)

/***********************************************************************
* Function: searchNetlink                                              *
************************************************************************
* Description:                                                         *
* Parameters:                                                          *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
static const char *searchNetlink(const char *key, const char *buf, size_t len, BOOL print_debug_en)
{
	size_t curlen;
	size_t keylen = strlen(key);
	char *cur = (char *)buf; //not const

	if ( print_debug_en )
	{
		fprintf(stderr, "DIAG_MMI_IF: receive event:key=%s, buf=%s, keylen=%d, len=%d\r\n", key, buf, keylen, len);
	}

	while (cur < buf + len - keylen)
	{
		curlen = strlen(cur);
		if (curlen == 0)
		{
			break;
		}

		if ( print_debug_en )
		{
			/* log events */
			fprintf(stderr, "****** searchNetlink: %s\r\n", cur );
		}

		if (!strncmp(key, cur, keylen) && cur[keylen] == '=')
		{
			/* log events */
			//fprintf(stderr,"%s\r\n", cur + keylen + 1);

			return (cur + keylen + 1);
		}

		cur += curlen + 1;
	}

	return NULL;
}
#endif

/***********************************************************************
* Function: MMI_process_control_event                                      *
************************************************************************
* Description:                                                                                                             *
*                                                                                  *
* Parameters:                                                          *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
static BOOL MMI_process_control_event( void )
{

	BOOL ret_val = FALSE;

#if 0
	ssize_t dwBytesTransferred = -1 /*0*/;
	MMI_IF_Event_t event = MMI_IF_EVENT_NONE;
	/* read diag MMI control event from pipe */
	dwBytesTransferred = read( MMI_control_fd, (void *)TransferBuffer, TRANSFER_BUFFER_SZ );
	if (dwBytesTransferred <= 0)
	{
		return FALSE;
	}

	TransferBuffer[dwBytesTransferred] = 0x0;
	fprintf(stderr, "DIAG MMI_IF: received control message: %s\r\n", (char *)TransferBuffer);

	/* process event, start/stop? */
	if ( !strncmp((char *)TransferBuffer, DIAG_MMI_CTRL_START_MSG, strlen(DIAG_MMI_CTRL_START_MSG)) )
	{
		fprintf(stderr, "DIAG MMI_IF: Got start \r\n");
		event = MMI_IF_EVENT_USER_START;

		ret_val = TRUE;
	}
	else if ( !strncmp((char *)TransferBuffer, DIAG_MMI_CTRL_STOP_MSG, strlen(DIAG_MMI_CTRL_STOP_MSG) ) )
	{
		fprintf(stderr, "DIAG MMI_IF: Got stop \r\n");
		FS_Diag_Assert_Received = FALSE; //Workaround for assure that the queue is empty
		event = MMI_IF_EVENT_USER_STOP;

		ret_val = TRUE;

#if defined (ENABLE_DIAG_MMI_TEST)
	}
	else if ( !strncmp((char *)TransferBuffer, DIAG_MMI_CTRL_TEST_MSG, strlen(DIAG_MMI_CTRL_TEST_MSG) ) )
	{
		fprintf(stderr, "DIAG MMI_IF: Got test \r\n");
		diagOverFSfilter("/tel/test.bin");
#endif //ENABLE_DIAG_MMI_TEST

	}
	else if ( !strncmp((char *)TransferBuffer, DIAG_MMI_CTRL_DEBUG_PRINTS_EN_MSG, strlen(DIAG_MMI_CTRL_DEBUG_PRINTS_EN_MSG) ) )
	{
		debug_prints_enabled = TRUE;
	}

	/* update state machine with new event */
	MMI_IF_state_machine( event, NULL );
#endif
	return ret_val;
}


#if defined(K2UEVENT_LIB)
void AddMMCCallBackfunction( __attribute__((unused)) CHAR* TransferBuffer,
			     __attribute__((unused)) int * ReadLen,
			     __attribute__((unused)) char * subsys,
			     __attribute__((unused)) char* usr_str_val,
			     __attribute__((unused)) char* action)
{
	fprintf(stderr, "***** AddMMCCallBackfunction(). DIAG MMI_IF Netlink add. DEVPATH is <%s> *****\r\n", usr_str_val);

	/* update state machine with new event */
	MMI_IF_state_machine( MMI_IF_EVENT_CARD_IN, NULL );
}

void RemoveMMCCallBackfunction( __attribute__((unused)) CHAR* TransferBuffer,
				__attribute__((unused)) int * ReadLen,
				__attribute__((unused)) char * subsys,
				__attribute__((unused)) char* usr_str_val,
				__attribute__((unused)) char* action)
{
	fprintf(stderr, "***** RemoveMMCCallBackfunction(). DIAG MMI_IF Netlink remove DEVPATH is <%s> *****\r\n", usr_str_val);

	/* update state machine with new event */
	MMI_IF_state_machine( MMI_IF_EVENT_CARD_OUT, NULL );
}
#else
/***********************************************************************
* Function: MMI_process_card_insertion_event                           *
************************************************************************
* Description:                                                                                                             *
*                                                                                  *
* Parameters:                                                          *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
static BOOL MMI_process_card_insertion_event( void )
{
	ssize_t dwBytesTransferred = -1;
	const char              *keys;
	const char              *subsys;
	const char              *action;
	/*const*/ char          *devpath;
	//char                  buffer[256];
	const char              *null_ptr;
	MMI_IF_Event_t event = MMI_IF_EVENT_NONE;


	//DIAGPRINT(">> Enter MMI_process_card_insertion_event\n");

	/* read Netlink udev event  */
	dwBytesTransferred = recv(diag_extIF_nl_sd, TransferBuffer, TRANSFER_BUFFER_SZ, 0);
	if (dwBytesTransferred <= 0)
	{
		return FALSE;
	}
	//DIAGPRINT(">> Got Netlink data\n");
	/* search udev netlink keys from env */
	keys = (const char *)TransferBuffer;


	if ( debug_prints_enabled != FALSE )
	{
		null_ptr = searchNetlink("xxxx", keys, dwBytesTransferred, TRUE);
	}
	subsys  = searchNetlink("SUBSYSTEM", keys, dwBytesTransferred, FALSE);
	action  = searchNetlink("ACTION", keys, dwBytesTransferred, FALSE);
	devpath = (char *)searchNetlink("DEVPATH", keys, dwBytesTransferred, FALSE);



	/* wrong event */
	if ( (!subsys) || (!action) || (!devpath) )
	{
		return FALSE;
	}


	/* is MMC? */
	if (strcmp(subsys, "mmc") != 0)
	{
		return FALSE;
	}
	DIAGPRINT(">>DIAG SDCard. subsys:%s,action:%s,devpath:%s.\n", subsys, action, devpath);
	/* plug in or out? */
	if (!strcmp(action, "add"))
	{
		fprintf(stderr, "***** DIAG MMI_IF Netlink add *****\r\n");
		DIAGPRINT("***** DIAG MMI_IF Netlink add *****\r\n");

		/* set event */
		event = MMI_IF_EVENT_CARD_IN;
	}
	else if (!strcmp(action, "remove"))
	{
		fprintf(stderr, "***** DIAG MMI_IF Netlink remove *****\r\n");
		DIAGPRINT("***** DIAG MMI_IF Netlink remove *****\r\n");

		/* set event */
		event = MMI_IF_EVENT_CARD_OUT;
	}
	else
	{
		/* unknown event  => Continue */
		return FALSE;
	}

	/* update state machine with new event */
	MMI_IF_state_machine( event, NULL );
	if(current_state == MMI_IF_STATE_MOUNTED_STOPPED)
	{
		if (diagIntData.diagBSPFScfg.log_auto_start)
		{
			fprintf(stderr, "***** DIAG over FS: AUTO START ... *****\r\n");
			MMI_Report(MMI_REP_AUTO, 0);
		}
	}

	return TRUE;
}
#endif
static UINT32 getCurrentLoggingPathNum( void )
{
	int dirNum = 0;
	FILE *fp;

	fp = fopen(FILE_LOGDIR, "r");
	if (fp != NULL)
	{
		if ( fscanf(fp, "DirNum=%d", &dirNum) == EOF )
		{
			dirNum = 0;
		}

		fclose(fp);
	}

	return dirNum;
}

static void setCurrentLoggingPathNum( UINT32 nextDirNum )
{
	FILE *fp;

	fp = fopen(FILE_LOGDIR, "w");
	if (fp != NULL)
	{
		fprintf(fp, "DirNum=%d", nextDirNum);
		fclose(fp);
	}
	else
	{
		fprintf(stderr, " ***** DIAG over FS : fail to write logDir ****** \n");
	}
	sync();

}

/* Socket-free direction to the current log for EEH (eliminate EEH dependency on DIAG, which might be dead at the time assert is being handled) */
static void updateLoggingPathLink(const char* logName)
{
#if defined(LM_EEH_LOG_LOCATION)
	char linkCmd[MAX_PATH_SZ + sizeof(LM_EEH_LOG_LOCATION) + 20];
	/* Note, soft-link would be nice here, but causes some problems, e.g. the actual log-name would be not visible */
	sprintf(linkCmd, "busybox echo %s > %s", logName, LM_EEH_LOG_LOCATION);
	system(linkCmd);
#else
	(void)logName; /* unused */
#endif
}

static void createCurrentLoggingPath( BOOL isInit )
{
	int dirNum = 0, nextDirNum;

	/* get current directory number */
	dirNum = getCurrentLoggingPathNum();
	DIAGPRINT("********* DIAG over FS: creat log Path Init:%d\n",isInit);
	/* if this is not called from module init => need to update current directory */
	if ( isInit == FALSE )
	{
		/* set next directory number */
		nextDirNum = dirNum + 1;
		setCurrentLoggingPathNum(nextDirNum);
		/* Set path name to log to */
		sprintf(dirPath2Log, "%s/Log%03d", RelPath2Log, nextDirNum);

		/* Create the directory to log to */
		/*Always create the new dir when incrementing */
		if ( mkdir(dirPath2Log, 0x666) < 0 )
		{
			fprintf(stderr, "********* DIAG over FS:   mkdir failure: dirPath2Log=%s*********\n", dirPath2Log);
		}
	}
	else
	{
		/* Set path name to log to */
		sprintf(dirPath2Log, "%s/Log%03d", RelPath2Log, dirNum);
		/* Create the directory to log to */
		/*Only create new dir if you are in mode of overwrite reset*/
		/*It will succeed only if the folder does not exist for the first time*/
		if (local_SD_FS_Config.FS_Inc_Reset == 0)
		{
			if ( mkdir(dirPath2Log, 0x666) < 0 )
			{
				fprintf(stderr, "********* DIAG over FS:   mkdir failure: dirPath2Log=%s*********\n", dirPath2Log);
			}
		}
	}

	updateLoggingPathLink(dirPath2Log);
	printf("DIAG over FS : Current Log Path === %s\n", dirPath2Log);
}

void initFSMountDirectory()
{

/*This define allows the file diag_bsp.cfg and FS logs to be moved under diag_marvell_rw_dir
   This is  not always a good decission, specially when supporting old systems, if not defined the file will stay under /tel */
#if defined(DIAG_ALLOW_CFGFILE_REPLACEMENT)

	if (mmidiag_marvell_rw_dir)
	{
		strcpy(DIAG_MMI_FS_MOUNT_POINT, mmidiag_marvell_rw_dir);
	}
	else
#endif
	{
		strcpy(DIAG_MMI_FS_MOUNT_POINT, "/data/acat");
	}

	printf("********* DIAG OVER FS : DIAG_MMI_FS_MOUNT_POINT =%s\n", DIAG_MMI_FS_MOUNT_POINT);
}
void initFileLogDirNum()
{

	//Each SD card has its own logging directory
	if (diagIntData.m_eLLtype == tLL_SC)
	{
		sprintf(FILE_LOGDIR,"%s/DiagCurrentLogDir.cfg",DIAG_MMI_CARD_MOUNT_POINT);
	} else
	{
		sprintf(FILE_LOGDIR,"%s/DiagCurrentLogDir.cfg",DIAG_MMI_FS_MOUNT_POINT);
	}
	printf("********* DIAG OVER FS : FILE_LOGDIR =%s\n", FILE_LOGDIR);
}
void MMI_process_eehcontrol_event_receive(void)
{
	ssize_t dwBytesTransferred = -1;


	fprintf(stderr, "********* DIAG OVER FS : processing EEH receive event *********\n");

	FS_Diag_Assert_Received = TRUE; //Workaround for assure that the queue is empty
	dwBytesTransferred = recv(MMI_eeh_fd_read, EEHTransferBuffer, MAX_EEH2DM_MESSAGE_SIZE, 0);
	if (dwBytesTransferred <= 0)
	{
		fprintf(stderr, "***** DIAG over FS: EEH receive Error: %s *****\r\n", strerror(errno));


	}
	else
	{
		fprintf(stderr, "***** DIAG over FS: EEH receive : %s *****\r\n", EEHTransferBuffer);
		openEehwriteSocket();
		if ( MMI_eeh_fd_write < 0 )
		{
			fprintf(stderr, "***** DIAG over FS: MMI EEH init failed! Error: %s *****\r\n", strerror(errno));
			return;
		}
		else
		{
			MMI_process_eehcontrol_event_send();
		}
	}

}

void MMI_process_eehcontrol_event_send(void)
{
#if 0
	ssize_t len;

	if ( strncmp(EEHTransferBuffer, DIAG_MMI_CTRL_EEH_ASSERT, strlen(DIAG_MMI_CTRL_EEH_ASSERT)) == 0 )
	{
		/* update state machine to STOP LOGGING (ASSERT behavior) */
		MMI_IF_state_machine( MMI_IF_EVENT_USER_STOP, NULL );

		//send the new dir name to error handler
		//Do nothing on auto increset createCurrentLoggingPath( TRUE );
		len = strlen(dirPath2Log);

		//First Send the Path to EEHandler
		if ( send(MMI_eeh_fd_write, dirPath2Log, len + 1, 0) < 0 )
		{
			fprintf(stderr, "***** DIAG over FS: EEH send Error: %s *****\r\n", strerror(errno));
		}

		/* shukiz: TODO - implement ASSERT behavior */
		// create path & store in dirPath2Log
		if (local_SD_FS_Config.FS_Inc_Reset == 0)
		{
			createCurrentLoggingPath( FALSE );
		}


	}
#endif
}

/***********************************************************************
* Function: MMI_NetlinkListenerTask                                    *
************************************************************************
* Description: MMI Netlink listener task entry point				   *
*                                                                                  *
* Parameters:                                                          *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
static void MMI_NetlinkListenerTask(void *ptr)
{
	int fdcount, maxfd = -1;
	fd_set readfds;

	DIAG_UNUSED_PARAM(ptr)

	fprintf(stderr, "***** DIAG MMI_NetlinkListener Task *****\r\n");


	while (1)
	{
		/* zero out all events */
		FD_ZERO(&readfds);
		maxfd = -1;

		if ( debug_prints_enabled != FALSE )
		{
#if defined(K2UEVENT_LIB)
			printf("MMI_control_fd = %d\n MMI_eeh_fd_read=%d\n", MMI_control_fd, MMI_eeh_fd_read);
#else
			printf("MMI_control_fd = %d\n diag_extIF_nl_sd=%d\n MMI_eeh_fd_read=%d\n", MMI_control_fd, diag_extIF_nl_sd, MMI_eeh_fd_read);
#endif
		}
#if !defined(K2UEVENT_LIB)
		/* was event received? */
		if (diag_extIF_nl_sd >= 0)
		{
			FD_SET(diag_extIF_nl_sd, &readfds);
			maxfd = MAX(diag_extIF_nl_sd, maxfd);
		}
#endif
		if ( MMI_control_fd >= 0 )
		{
			/* Add to set */
			FD_SET(MMI_control_fd, &readfds);
			maxfd = MAX(MMI_control_fd, maxfd);

		}

		/* was EEH event received? */
		if (MMI_eeh_fd_read >= 0)
		{
			FD_SET(MMI_eeh_fd_read, &readfds);
			maxfd = MAX(MMI_eeh_fd_read, maxfd);

		}

		if ( debug_prints_enabled != FALSE )
		{
#if defined(K2UEVENT_LIB)
			printf("maxfd = %d \n MMI_control_fd = %d\n MMI_eeh_fd_read = %d\n", maxfd + 1, MMI_control_fd, MMI_eeh_fd_read);
#else
			printf("maxfd = %d \n MMI_control_fd = %d\n diag_extIF_nl_sd = %d\n MMI_eeh_fd_read = %d\n", maxfd + 1, MMI_control_fd, diag_extIF_nl_sd, MMI_eeh_fd_read);

#endif
		}

		/* wait for events */
		fdcount = select( maxfd + 1, &readfds, NULL, NULL, NULL );

		if (fdcount <= 0)
		{
			fprintf( stderr, "***** DIAG Over FS: Select() returned with Error: %s *****\r\n", strerror(errno) );
			continue; /* timeout or error */
		}

		if ( debug_prints_enabled != FALSE )
		{
			fprintf( stderr, "***** DIAG Over FS: Select() Number of socket connected = %d *****\r\n", fdcount);

			fprintf( stderr, "***** DIAG Over FS: Select() returned with no error *****\r\n" );
		}

#if !defined(K2UEVENT_LIB)
		/* is netlink data? */
		if ( (diag_extIF_nl_sd > 0) && (FD_ISSET(diag_extIF_nl_sd, &readfds)) )
		{
			/* check for MMI card insertion event */
			MMI_process_card_insertion_event();
			//DIAGPRINT(">>> Fake Call MMI_process_card_insertion_event()\n");
		} /* if(diag_extIF_nl_sd > 0 && FD_ISSET(diag_extIF_nl_sd, &readfds))*/
#endif
		/* is DIAG MMI control data? */
		if ( (MMI_control_fd > 0) && (FD_ISSET(MMI_control_fd, &readfds)) )
		{
			/* check for MMI card insertion event */
			MMI_process_control_event();
		}
		if ( (MMI_control_fd > 0) && (FD_ISSET(MMI_eeh_fd_read, &readfds)) )
		{
			/* handle EEH events */
			MMI_process_eehcontrol_event_receive();
		}

	}
}

/***********************************************************************
* Function: MMI_Report                                                 *
************************************************************************
* Description: reports Diag over FS errors to the MMI application.	   *
*                                                                                  *
* Parameters:  E_DiagOverFS_MMI_Report rep - error type enumerator.    *
*              int err_code - linux errno.                             *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
void MMI_Report(E_DiagOverFS_MMI_Report rep, int err_code)
{
	switch (rep)
	{
	case MMI_REP_AUTO:
		fprintf(stderr, "***** DIAG DIAG MMI_IF MMI_Report: MMI_REP_AUTO *****\r\n");
		/* update state machine with new event */
		MMI_IF_state_machine( MMI_IF_EVENT_REPORT_AUTO, NULL );
		break;
	case MMI_REP_ALREADY_START:
		fprintf(stderr, "***** DIAG DIAG MMI_IF MMI_Report: MMI_REP_ALREADY_START *****\r\n");
		break;
	case MMI_REP_ALREADY_STOP:
		fprintf(stderr, "***** DIAG DIAG MMI_IF MMI_Report: MMI_REP_ALREADY_STOP *****\r\n");
		break;
	case MMI_REP_STOP_DONE:
		fprintf(stderr, "***** DIAG DIAG MMI_IF MMI_Report: MMI_REP_STOP_DONE *****\r\n");
		/* update state machine with new event */
		MMI_IF_state_machine( MMI_IF_EVENT_REPORT_STOP_DONE, NULL );
		break;
	case MMI_REP_APPS_FILTER_DB_VER_MATCH:
		fprintf(stderr, "***** DIAG DIAG MMI_IF MMI_Report: MMI_REP_APPS_FILTER_DB_VER_MATCH *****\r\n");
		break;
	case MMI_REP_APPS_FILTER_DB_VER_MISMATCH:
		fprintf(stderr, "***** DIAG DIAG MMI_IF MMI_Report: MMI_REP_APPS_FILTER_DB_VER_MISMATCH *****\r\n");
		break;
	case MMI_REP_COMM_FILTER_DB_VER_MATCH:
		fprintf(stderr, "***** DIAG DIAG MMI_IF MMI_Report: MMI_REP_COMM_FILTER_DB_VER_MATCH *****\r\n");
		break;
	case MMI_REP_COMM_FILTER_DB_VER_MISMATCH:
		fprintf(stderr, "***** DIAG DIAG MMI_IF MMI_Report: MMI_REP_COMM_FILTER_DB_VER_MISMATCH *****\r\n");
		break;
	case MMI_REP_LSEEK:
		fprintf(stderr, "***** DIAG DIAG MMI_IF MMI_Report: MMI_REP_LSEEK *****\r\n");
		break;
	case MMI_REP_WRITE:
		fprintf(stderr, "***** DIAG DIAG MMI_IF MMI_Report: MMI_REP_WRITE *****\r\n");
		break;
	case MMI_REP_FOPEN:
		fprintf(stderr, "***** DIAG DIAG MMI_IF MMI_Report: MMI_REP_FOPEN *****\r\n");
		break;
	case MMI_REP_OPEN:
		fprintf(stderr, "***** DIAG DIAG MMI_IF MMI_Report: MMI_REP_OPEN *****\r\n");
		break;
	case MMI_REP_FSYNC:
		fprintf(stderr, "***** DIAG DIAG MMI_IF MMI_Report: MMI_REP_FSYNC *****\r\n");
		break;
	case MMI_REP_CLOSE:
		fprintf(stderr, "***** DIAG DIAG MMI_IF MMI_Report: MMI_REP_CLOSE *****\r\n");
		break;
	case MMI_REP_FCLOSE:
		fprintf(stderr, "***** DIAG DIAG MMI_IF MMI_Report: MMI_REP_FCLOSE *****\r\n");
		break;
	case MMI_REP_STAT:
		fprintf(stderr, "***** DIAG DIAG MMI_IF MMI_Report: MMI_REP_STAT *****\r\n");
		break;
	case MMI_REP_MKDIR:
		fprintf(stderr, "***** DIAG DIAG MMI_IF MMI_Report: MMI_REP_MKDIR *****\r\n");
		break;
	default:
		fprintf(stderr, "***** DIAG DIAG MMI_IF MMI_Report: unknown!!! *****\r\n");
		break;
	}
	fprintf(stderr, "***** DIAG DIAG MMI_IF MMI_Report: Error: %s *****\r\n", strerror(err_code));
	return;
}

