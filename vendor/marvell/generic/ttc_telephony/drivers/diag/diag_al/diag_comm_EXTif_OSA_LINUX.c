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

/*********************************************************************
 *                      M O D U L E     B O D Y                       *
 **********************************************************************
 * Title: Diag                                                        *
 *                                                                    *
 * Filename: diag_comm_EXTif_OSA_LINUX.c                              *
 *                                                                    *
 * Target, platform: Common Platform, SW platform                     *
 *                                                                    *
 * Authors: Yaeli Karni												 *
 *                                                                    *
 * Description: handles the OS specific implementation for ExtIF      *
 *                                                                    *
 * Notes:                                                             *
 *                                                                    *
 *  Yaeli Karni, NOv 2007 - set OS specific file for ext if           *
 *	implement the following:										 *
 *				diagCommExtIFosSpecificInit							 *
 *				diagCommExtIfConnInit								 *
 *				diag_os_TransmitToExtIfMulti						 *
 *				diag_os_TransmitToExtIf								 *
 *				diagExtIfcon_discon									 *
 *			                                                                                 *
 **********************************************************************/

/************* General include definitions ****************************/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> //for open
#include <unistd.h> //for write
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <termios.h>
#include <fcntl.h>
#include <errno.h>
#include <asm/types.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <sys/un.h> //AF_UNIX
#include <netdb.h> //AF_UNIX
#include <sys/types.h>
#include <linux/netlink.h> //for Linux sockets
#include <net/if.h> //for ifconf
#include <ctype.h> //toupper

//#include "osa.h"
//#include "global_types.h"
//#include "ICAT_config.h"
//#include "diag_config.h"
//#include "diag_osif.h"
//#include "diag_init.h"

#include "global_types.h"

#include "diag_config.h"
#include "osa.h"
#include "diag_types.h"
#include "diag.h"
#include "diag_pdu.h"
#include "diag_tx.h"
#include "diag_header_external.h"
#include "diag_buff.h"

#include "diag_osif.h"


#include INCLUDE_ASSERT_OR_DUMMY
//#include "diag_mem.h"
#include "diag_comm.h"
#include "diag_rx.h"
//#include "diag_comm_L2.h"
#include "diag_comm_L4.h"

//#include "diag.h"
//#include "diag_ram.h"
//#include "diag_buff.h"
//#include "diag_comm_EXTif.h"


#include "diag_mmi_if_OSA_LINUX.h"
#include "diag_al.h"
#include "diag_init.h"
#include "osa_types.h"

#include "pxa_dbg.h"

#define DIAGPRINT DPRINTF
//#define  fprintf( stream, fmt, args ...) DPRINTF(fmt, ## args)

#define UNUSEDPARAM(param) (void)param;
#if defined(K2UEVENT_LIB)
#include "k2uevent.h"
#endif

#define DEFAULT_SDL_HEADER_VER 1
#define ICAT_DATA_LENGTH        0x1000 //4k Message for ICAT		// used by some drivers in WM. diag uses for WinCE in diag_comm_USB.c - (set specific definition)

#define DIAG_TTY_USB_DEVICE "/dev/ttygserial"
#define DIAG_TTY_UART_DEVICE "/dev/ttyS0"

/* termios defaults */
#define BAUD            B115200
#define IO_FLUSH_PENDING_BUFFERS _IOW('T', 99, int)

static UINT32 cacheFlush();
static UINT32 cachedWrite(UINT8 *msg, UINT32 len);

UINT32 SendNoopMsg();                           //for UDP support
int SendUDPMsg(UINT8 *msg, UINT32 len);      //for UDP support
UINT32 SendCommMsg(UINT8 *msg, UINT32 len);
void diagPreDefCMMInit(void);
void diagCMMRegister(void);


extern int hPort;                               //file descriptor for the log file or USB or UART used in write()
extern UINT32 diag_count;
extern int diag_server;
extern const UINT32 _FirstCommandID;
extern const UINT32 _LastCommandID;
extern const UINT32 _FirstReportID;
extern const UINT32 _LastReportID;


///////////////////////////////////////////
//Diag over FS (SD card) global variables//
///////////////////////////////////////////
static UINT32 sdl_header_ver;
static UINT32 updateCommDBverOnClose = 0; //Override the log file header on close when having the comm DB ver.
static UINT32 commDBver = 0;
static UINT32 filterCommDBver = 0;
static UINT32 total_log_tx = 0;                         //number of bytes written to the current log file
static UINT32 log_file_size;                            //maximum log file size
static UINT32 log_file_num = 0;                         //current number in the log file name convention
static UINT32 log_file_index = 0;                       //current index of the log file (for the header)
static UINT32 max_log_file_num;                         //maximum number of files for the log file name convention
//extern UINT32 log_auto_start;			//automatic start of logging to the log file
static char log_file_path[256];                         //log file name convention
char log_file_rel_path[256];                            //log file relative path

#if !defined(K2UEVENT_LIB)
int diag_extIF_nl = -1; /* file descriptor of netlink */
#endif
//UINT32 hUsbPortFunctional=0;	// the USBport is not functional
UINT32 lastUSBevent;                            // 0-none, 1-USB connect, 2-USB disconnect,10-various-error
UINT32 lastUDPevent;                            // 0-none, 1-UDP connected, 2-UDP disconnected, 10 -errorr
//DCB m_dcb; //Global
//int m_EXTsock = -1;                             //for UDP support
UINT32 m_bFirstConnected = FALSE;
//UINT32 udp_state        = 0;                //0-disconnected, 1-connected, 2-trying to connect
UINT32 m_iLostExtTxMessages = 0;
unsigned long m_lAddress;
unsigned long m_lTavorAddress;
unsigned short m_sUDP_Port;
UINT32 bUDPsendICATready = 0;

UINT32 myClientID = 0; //TBDIY updated by the CMI master when a client registers and receives a confirm CMM.

OSSemaRef send_comm_msg_sem = NULL;
OSSemaRef connect_fs_sem = NULL;

UINT32 RegisterCMM[11];//cleared by default
//+------+---+---+----------+---------+-------+---------+-----------+
//|Length|PID|SAP|Service ID|Module ID|Comm ID|Source ID|CMM version|
//|2     |2  |1  |1         |2        |4      |4        |4          |
//+------+---+---+----------+---------+-------+---------+-----------+

void disconnectCOMM(void);
void connectCOMM(void);

extern void diagExtIFstatusConnectNotify(void);
extern void diagExtIFStatusDiscNotify(void);
extern void diagUsbConnectNotify();
extern void diagUsbDisConnectNotify(void);
extern UINT32 diagCommSetChunk(DIAG_COM_RX_Packet_Info  *RxPacket, CHAR *msgData, UINT32 length, UINT32 bRemoveHeader);
void dispatchServiceForSAP(UINT8 *Ori_data);
extern UINT32      diag_lock_L(UINT32 *cpsr_ptr, OS_Mutex_t * lock);
extern  void        diag_unlock_L(UINT32 cpsr_val, OS_Mutex_t * lock);
extern void    diagCommL2Init  (void);
extern int sdl_signature;
extern GlobalIfData dataExtIf;
DIAG_IMPORT const DiagDBVersion DBversionID;
extern char *SOCKET_PATH_MASTER;
extern BOOL bDiagConnAcked;
//Cached Write mechanism
typedef struct {
	UINT32 cachePtr;
	OS_Mutex_t diag_cached_write_mutex;
	OSTimerRef diag_cached_write_timer_ref;
	UINT8      *cachedWriteBuff;
} CachedWriteS;
typedef struct{
	UINT32 state;	//0-disconnected, 1-connected, 2-trying to connect
	UINT32 cpsr;
	OS_Mutex_t lock;
}UdpState;
extern UdpState udp_state;
extern UdpState tcp_state;
extern UdpState local_state;
extern UdpState cmi_state;
extern Diag_Target diag_target;
extern Diag_Event diag_event;
extern int diag_server_port;
DIAG_IMPORT const UINT32 _FirstCommandID;
#if !defined(K2UEVENT_LIB)
#if 0
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
static void openNetlink()
{
	int fd;
	struct sockaddr_nl snl;

	memset(&snl, 0, sizeof(snl));
	snl.nl_family = AF_NETLINK;
	snl.nl_pid = getpid();
	snl.nl_groups = 1;

	fd = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
	if (fd < 0)
	{
		fprintf(stderr, "***** DIAG over FS: socket NETLINK_KOBJECT_UEVENT Error: %s *****\r\n", strerror(errno));
		return;
	}

	if (bind(fd, (struct sockaddr *)&snl, sizeof(struct sockaddr_nl)) < 0)
	{
		fprintf(stderr, "***** DIAG over FS: bind NETLINK_KOBJECT_UEVENT Error: %s *****\r\n", strerror(errno));
		close(fd);
		return;
	}

	diag_extIF_nl = fd;
}
#endif
#endif

/***********************************************************************
* Function: diagOverFSfilter                                           *
************************************************************************
* Description:  Used to read the Diag over FS filter file.             *
*                                                                      *
* Parameters:   const char *filter_file - FS filter file name.         *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
void diagOverFSfilter(const char *filter_file)
{
	UNUSEDPARAM(filter_file)
#if 0
	struct tHeader
	{
		char c[4];                      // = "V00"
		UINT32 header_length;           // = sizeof(tHeader)
		UINT32 com_version;             // db version of comm
		UINT32 app_version;             // db version of app
		UINT32 length;                  // data length
		UINT32 commands;                // number of commands in data
	} sFilterHeader;

	int fd;

	fd = open(filter_file, O_RDONLY);
	if (fd < 0)
	{
		fprintf(stderr, "***** DIAG over FS: filter file %s was not opened. Error: %s. *****\r\n", filter_file, strerror(errno));
	}
	else
	{
		ssize_t i;
		CHAR buff[ICAT_DATA_LENGTH];

		i = read(fd, &sFilterHeader, sizeof(sFilterHeader));
		if (i < (ssize_t)sizeof(sFilterHeader))
		{
			fprintf(stderr, "***** DIAG over FS: diagOverFSfilter read() failure when reading the header. *****\r\n");
		}
		else
		{
			UINT32 cmp_ver;

			filterCommDBver = sFilterHeader.com_version;
			if (commDBver != 0)
			{
				if (commDBver == filterCommDBver)
				{
					fprintf(stderr, "***** DIAG over FS: diagOverFSfilter COMM DB VERSION MATCH=0x%x !!! *****\r\n", filterCommDBver);
					MMI_Report(MMI_REP_COMM_FILTER_DB_VER_MATCH, 0);
				}
				else
				{
					fprintf(stderr, "***** DIAG over FS: diagOverFSfilter COMM DB VERSION DOES NOT MATCH img=0x%x filter=0x%x !!! *****\r\n", commDBver, filterCommDBver);
					MMI_Report(MMI_REP_COMM_FILTER_DB_VER_MISMATCH, 0);
				}
			}
			if (sscanf(DBversionID, "0x%lx", (long unsigned int *)&cmp_ver) == 1)
			{
				if (cmp_ver == sFilterHeader.app_version)
				{
					fprintf(stderr, "***** DIAG over FS: diagOverFSfilter APPS VERSION MATCH=0x%x !!! *****\r\n", sFilterHeader.app_version);
					MMI_Report(MMI_REP_APPS_FILTER_DB_VER_MATCH, 0);
				}
				else
				{
					fprintf(stderr, "***** DIAG over FS: diagOverFSfilter APPS VERSION DOES NOT MATCH img=0x%x filter=0x%x !!! *****\r\n", cmp_ver, sFilterHeader.app_version);
					MMI_Report(MMI_REP_APPS_FILTER_DB_VER_MISMATCH, 0);
				}
			}
			fprintf(stderr, "***** DIAG over FS: diagOverFSfilter header version=%s *****\r\n", sFilterHeader.c);
			fprintf(stderr, "***** DIAG over FS: diagOverFSfilter header length=%d *****\r\n", sFilterHeader.header_length);
			fprintf(stderr, "***** DIAG over FS: diagOverFSfilter com version=0x%x *****\r\n", sFilterHeader.com_version);
			fprintf(stderr, "***** DIAG over FS: diagOverFSfilter app version=0x%x *****\r\n", sFilterHeader.app_version);
			fprintf(stderr, "***** DIAG over FS: diagOverFSfilter length=%d *****\r\n", sFilterHeader.length);
			fprintf(stderr, "***** DIAG over FS: diagOverFSfilter commands=%d *****\r\n", sFilterHeader.commands);
			while ((i = read(fd, buff, ICAT_DATA_LENGTH)) > 0)
			{
				UINT32 lengthOfRecMsg, buffIndex; //support more than one msg in buff
				buffIndex = 0;
				while (i && (lengthOfRecMsg = diagCommSetChunk(dataExtIf.RxPacket, buff + buffIndex, i, 1 /*TBDIY remove the header*/)) <= (UINT32)i)
				{
					if (lengthOfRecMsg == 0) break;  //quit when ignoring the msg and prevent endless loop
					buffIndex += lengthOfRecMsg;
					i -= lengthOfRecMsg;
					diagReceiveCommand(dataExtIf.RxPacket);
				}
			}
		}
		if (close(fd) != 0)
		{
			int err = errno;
			MMI_Report(MMI_REP_CLOSE, err);
			fprintf(stderr, "***** DIAG over FS: diagOverFSfilter close() failure. Error: %s. *****\r\n", strerror(err));
		}
	}
#endif
}

/***********************************************************************
* Function: diagOverFScfg                                              *
************************************************************************
* Description:  Used to read the Diag over FS configuration file.      *
*                                                                      *
* Parameters:   const char *cfg_file - FS configuration file name.     *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
void diagOverFScfg(const char *cfg_file)
{
	FILE *stream;
	char s[256 + 10];
	char s1[266];
	char s2[266];

	log_file_num = 0; //reset when loading a new cfg file
	strcpy(log_file_path, "/log%03d.sdl");
	if(diagIntData.m_eLLtype == tLL_SC) //SD card, about 1G = 200x5M in total
	{
		log_file_size = (5*1024*1024); //default 5MB per file
		max_log_file_num = 200; //default
	}
	else								//Flash, about 16M = 8x2M in total
	{
		log_file_size = (2*1024*1024); //default 2MB per file
		max_log_file_num = 8;
	}
	sdl_header_ver = DEFAULT_SDL_HEADER_VER;

	stream = fopen(cfg_file, "r");
	if (stream == NULL)
	{
		fprintf(stderr, "***** DIAG over FS: %s was not opened. Error: %s. Trying to create a default one. *****\r\n", cfg_file, strerror(errno));
		stream = fopen(cfg_file, "w");
		if (stream == NULL)
		{
			int err = errno;
			MMI_Report(MMI_REP_FOPEN, err);
			fprintf(stderr, "***** DIAG over FS: The default %s was not opened. Error: %s. Using default values. *****\r\n", cfg_file, strerror(err));
		}
		else
		{
			fprintf(stream, "#Log file path: path=%s (default)\r\n", log_file_path);
			fprintf(stream, "path=%s\r\n", log_file_path);
			fprintf(stream, "#Log file size: size=%d (default)\r\n", log_file_size);
			fprintf(stream, "size=%d\r\n", log_file_size);
			fprintf(stream, "#Max log file num: max=%d (default)\r\n", max_log_file_num);
			fprintf(stream, "max=%d\r\n", max_log_file_num);
			fprintf(stream, "#SDL header version: ver=%d (default)\r\n", sdl_header_ver);
			fprintf(stream, "ver=%d\r\n", sdl_header_ver);
			if (fclose(stream) != 0)
			{
				int err = errno;
				MMI_Report(MMI_REP_FCLOSE, err);
				fprintf(stderr, "***** DIAG over FS: file %s fclose() failure. Error: %s. *****\r\n", cfg_file, strerror(err));
			}
			fprintf(stderr, "***** DIAG over FS: The default %s is ready. *****\r\n", cfg_file);
		}
	}
	else
	{
		int i;
		fprintf(stderr, "***** DIAG over FS: Analyzing the %s file ... *****\r\n", cfg_file);
		i = fscanf(stream, "%[][- =a-zA-Z0-9_.~\\/#%%?+|{}()<>,:;*&^$@!`']\r\n", s);
		while (i != EOF)
		{
			char *tmp;
			int t;

			//fprintf(stderr,"***** DIAG over FS: '%d' '%s' *****\r\n",i,s);
			if ((*s == '#') || (*s == '/') || (*s == '\\'))
			{
				i = fscanf(stream, "%[][- =a-zA-Z0-9_.~\\/#%%?+|{}()<>,:;*&^$@!`']\r\n", s);
				continue; // # and /  and \ are a remark line
			}

			tmp = s;
			while (*tmp != 0 && *tmp != '=')
			{
				*tmp = toupper(*tmp);
				tmp++;
			}
			t = sscanf(s, "%[^=]%*[=]%s", s1, s2);
			//fprintf(stderr,"***** DIAG over FS: '%d' '%s' '%s' *****\r\n",t,s1,s2);
			if (t == 2)
			{
				if (strcmp("PATH", s1) == 0)
				{
					strcpy(log_file_path, s2);
					fprintf(stderr, "***** DIAG over FS: PATH='%s' *****\r\n", log_file_path);
				}
				else if (strcmp("SIZE", s1) == 0)
				{
					sscanf(s2, "%d", &log_file_size);
					fprintf(stderr, "***** DIAG over FS: SIZE='%d' *****\r\n", log_file_size);
				}
				else if (strcmp("MAX", s1) == 0)
				{
					sscanf(s2, "%d", &max_log_file_num);
					fprintf(stderr, "***** DIAG over FS: MAX ='%d' *****\r\n", max_log_file_num);
				}
				else if (strcmp("VER", s1) == 0)
				{
					sscanf(s2, "%d", &sdl_header_ver);
					fprintf(stderr, "***** DIAG over FS: VER ='%d' *****\r\n", sdl_header_ver);
					if ((sdl_header_ver > 2) || (sdl_header_ver < 1))
					{
						sdl_header_ver = DEFAULT_SDL_HEADER_VER;
						fprintf(stderr, "***** DIAG over FS: Wrong VER! Using the default VER='%d' *****\r\n", sdl_header_ver);
					}
				}
				else
				{
					fprintf(stderr, "***** DIAG over FS: parser error: '%s' *****\r\n", s);
				}
			}
			else
			{
				fprintf(stderr, "***** DIAG over FS: parser error: '%s' *****\r\n", s);
			}
			i = fscanf(stream, "%[][- =a-zA-Z0-9_.~\\/#%%?+|{}()<>,:;*&^$@!`']\r\n", s);
		}
		if (fclose(stream) != 0)
		{
			int err = errno;
			MMI_Report(MMI_REP_FCLOSE, err);
			fprintf(stderr, "***** DIAG over FS: file %s fclose() failure. Error: %s. *****\r\n", cfg_file, strerror(err));
		}
	}
	/* Output data read: */
	fprintf(stderr, "***** DIAG over FS: log_file_path   ='%s' *****\r\n", log_file_path);
	fprintf(stderr, "***** DIAG over FS: log_file_size   ='%d' *****\r\n", log_file_size);
	fprintf(stderr, "***** DIAG over FS: max_log_file_num='%d' *****\r\n", max_log_file_num);
	fprintf(stderr, "***** DIAG over FS: sdl_header_ver  ='%d' *****\r\n", sdl_header_ver);
}

void connectCMI(void)
{

	DIAGPRINT("*****DIAG enter connectCMI()***\n");
	diag_lock_L(&cmi_state.cpsr,&cmi_state.lock); //mutex on udp_state - maybe checked when USB cable is connected.
	DIAGPRINT("*****DIAG connectCMIState;%d\n", cmi_state.state);
	if(cmi_state.state == 0)
	{
		int rc;
		struct sockaddr_un sin;
		cmi_state.state = 2;
		diag_unlock_L(cmi_state.cpsr,&cmi_state.lock);
		do
		{
			diag_server_port = socket(AF_UNIX, SOCK_DGRAM, 0);
			if(diag_server_port < 0)
				DIAGPRINT("*****DIAG creat soket error:%s(%d)\n", strerror(errno),errno);

		}while(diag_server_port < 0);
		memset(&sin, 0, sizeof(sin));
		sin.sun_family = AF_UNIX;
		strcpy(sin.sun_path, SOCKET_PATH_MASTER);
		DIAGPRINT("*****DIAG set AF_UNIX addr %s\n",SOCKET_PATH_MASTER);

		rc=	unlink(SOCKET_PATH_MASTER);
		if (rc!=0)
		{
			DIAGPRINT("***** DIAG CMI (cId = %d) couldn't unlink Socket rc == %d, Err= %d	*****\r\n",
			_FirstCommandID, rc, errno);

		}

		do
		{
			rc = bind(diag_server_port, (struct sockaddr *)&sin, sizeof(sin));
			if(rc != -1)	//successful
			{
				DIAGPRINT("*****Diag bind unix soket successful\n");
				break;
			}

		}while(1);
		if(rc != 0)
		{
			diag_lock_L(&cmi_state.cpsr, &cmi_state.lock);
			cmi_state.state = 0;
			diag_unlock_L(cmi_state.cpsr, &cmi_state.lock);
		}
		else
		{
			diag_lock_L(&cmi_state.cpsr, &cmi_state.lock);
			cmi_state.state = 1;
			diag_unlock_L(cmi_state.cpsr, &cmi_state.lock);
		}

	}
	else
	{
		UINT32 tmp=cmi_state.state;
		diag_unlock_L(cmi_state.cpsr, &cmi_state.lock);
		DIAGPRINT("*********************************************************************\r\n");
		DIAGPRINT("***** DIAG EXT Already binded/trying to bind !!! cmi_state=%d  *****\r\n",tmp);
		DIAGPRINT("*********************************************************************\r\n");
	}
}
void connectLocal(void)
{
	DIAGPRINT("*****DIAG enter connectLocal()***\n");
	diag_lock_L(&local_state.cpsr,&local_state.lock); //mutex on udp_state - maybe checked when USB cable is connected.
	DIAGPRINT("*****DIAG connectLocalState;%d\n", local_state.state);
	if(local_state.state ==0)
	{
		int rc;
		struct sockaddr_un clntaddr;
		local_state.state = 2;
		diag_unlock_L(local_state.cpsr,&local_state.lock);
		do
		{
			diag_target.hPort = socket(AF_UNIX, SOCK_DGRAM, 0);
			DIAGPRINT("*****DIAG creat a unix socket \n");

			if(diag_target.hPort < 0)
				DIAGPRINT("*****DIAG creat soket error:%s(%d)\n", strerror(errno),errno);

		}while(diag_target.hPort < 0);

		memset(&clntaddr, 0, sizeof(clntaddr));
		clntaddr.sun_family = AF_UNIX;
		sprintf(clntaddr.sun_path, "/tmp/client%d",getpid());
		DIAGPRINT("*****DIAG set AF_UNIX addr\n");

		do
		{
			rc = bind(diag_target.hPort, (struct sockaddr *)&clntaddr, sizeof(clntaddr));
			if(rc != -1)	//successful
			{
				DIAGPRINT("*****Diag bind unix soket successful\n");
				break;
			}

		}while(1);
		if(rc != 0)
		{
			diag_lock_L(&local_state.cpsr, &local_state.lock);
			local_state.state = 0;
			diag_unlock_L(local_state.cpsr, &local_state.lock);
		}
		else
		{
			diag_lock_L(&local_state.cpsr, &local_state.lock);
			local_state.state = 1;
			diag_unlock_L(local_state.cpsr, &local_state.lock);
#if !defined (DIAGNEWHEADER)
			// we have socket, lets reset the L2 rx- state machine
			diagCommL2Init	();
			// as for L4 - handshake - need to review for USB/UDP maybe replace NOOP
			//diagCommL4Init
#endif
			//diagUsbConnectNotify();
			diagPreDefCMMInit();
			diagCMMRegister();

		}
	}
	else
	{
		UINT32 tmp=local_state.state;
		diag_unlock_L(local_state.cpsr, &local_state.lock);
		DIAGPRINT("*********************************************************************\r\n");
		DIAGPRINT("***** DIAG EXT Already binded/trying to bind !!! local_state=%d  *****\r\n",tmp);
		DIAGPRINT("*********************************************************************\r\n");
	}
}
/***********************************************************************
* Function: connectUDP                                                 *
************************************************************************
* Description:  Connect UDP socket                                     *
* Parameters:   none                                                   *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
void connectUDP(void)
{
	DIAGPRINT("*****DIAG enter connectUDP()***\n");
	diag_lock_L(&udp_state.cpsr,&udp_state.lock); //mutex on udp_state - maybe checked when USB cable is connected.
	DIAGPRINT("*****DIAG connectUDP State;%d, bIsClient :%d\n", udp_state.state, diagIntData.m_bIsClient);
	m_sUDP_Port = htons(ACAT_UDP_PORT);
	if(udp_state.state ==0) //mutex on udp_state - maybe checked when USB cable is connected.
	{
		int rc;
		struct sockaddr_in sin;
		struct sockaddr_un clntaddr;

		udp_state.state = 2;
		diag_unlock_L(udp_state.cpsr,&udp_state.lock);
		do
		{
			if(diagIntData.m_bIsClient)
			{
				diag_target.hPort = socket(AF_UNIX, SOCK_DGRAM, 0);
				DIAGPRINT("*****DIAG creat a socket \n");
			}
			else
			{
				diag_target.hPort = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
				DIAGPRINT("*****DIAG creat a socket in IPPROTO_UDP\n");
			}
			if(diag_target.hPort  < 0)
			{
				DIAGPRINT("*****DIAG creat soket error:%s(%d)\n", strerror(errno),errno);
				sleep(1);
			}
		}while(diag_target.hPort < 0);

		if(diagIntData.m_bIsClient)
		{
			memset(&clntaddr, 0, sizeof(clntaddr));
			clntaddr.sun_family = AF_UNIX;
			sprintf(clntaddr.sun_path, "/tmp/client%d",getpid());
			DIAGPRINT("*****DIAG set AF_UNIX addr\n");
		}
		else
		{
			memset(&sin,0,sizeof(sin));
			sin.sin_family=AF_INET;
			//sin.sin_port=0;//Ido G wants to try a fixed port htons(12345) instead of 0
			sin.sin_port = m_sUDP_Port;
			DIAGPRINT("*****DIAG set AF_INET addr :%x\n",m_sUDP_Port);
		}
		do
		{
			int bindeer;
			if(diagIntData.m_bIsClient)
			{
				rc = bind(diag_target.hPort, (struct sockaddr *)&clntaddr, sizeof(clntaddr));
			}
			else
			{
				sin.sin_addr.s_addr = m_lTavorAddress;
				rc = bind(diag_target.hPort,(const struct sockaddr*)&sin,sizeof(sin));
			}
			DIAGPRINT("***** DIAG EXT rc == %d Last Error == %s(%d) *****\r\n",rc,strerror(errno),errno);
			if(rc != -1)	//successful
			{
				DIAGPRINT("*****Diag bind soket successful\n");
				break;
			}
			bindeer = errno;
			if (bindeer == 99 && diagIntData.m_bIsClient==0)
			{
				//close(diag_target.hPort);
				//diag_target.hPort = -1;
				//diagExtIFstatusConnectNotify();
				//break;
			}
			sleep(1);
		}while(1);
		if(rc != 0)
		{
			diag_lock_L(&udp_state.cpsr, &udp_state.lock);
			udp_state.state = 0;
			diag_unlock_L(udp_state.cpsr, &udp_state.lock);
		}
		else
		{
			diag_lock_L(&udp_state.cpsr, &udp_state.lock);
			udp_state.state = 1;
			diag_unlock_L(udp_state.cpsr, &udp_state.lock);
#if !defined (DIAGNEWHEADER)
			// we have socket, lets reset the L2 rx- state machine
			diagCommL2Init	();
			// as for L4 - handshake - need to review for USB/UDP maybe replace NOOP
			//diagCommL4Init
#endif
			diagUsbConnectNotify();
		}
	}
	else
	{
		UINT32 tmp=udp_state.state;
		diag_unlock_L(udp_state.cpsr, &udp_state.lock);
		DIAGPRINT("*********************************************************************\r\n");
		DIAGPRINT("***** DIAG EXT Already binded/trying to bind !!! udp_state=%d  *****\r\n",tmp);
		DIAGPRINT("*********************************************************************\r\n");
	}
}

void connectTCP()
{
	struct sockaddr_in serv_addr;
	struct sockaddr_in peer_addr;
	int ret = 0;
	int addr_size = 0;
	int addr_reuse = 1;
	DIAGPRINT("*****DIAG enter connectTCP()***\n");

	m_sUDP_Port = htons(ACAT_UDP_PORT);
	diag_lock_L(&tcp_state.cpsr,&tcp_state.lock);

	if(tcp_state.state == 1)
	{
		DIAGPRINT("*****DIAG already connect TCP\n");
		diag_unlock_L(tcp_state.cpsr, &tcp_state.lock);
		return ;
	}
	do
	{
		diag_target.socket = socket(AF_INET, SOCK_STREAM, 0);
		DIAGPRINT("*****DIAG creat a socket in TCP\n");
		if(diag_target.socket < 0)
		{
			DIAGPRINT("*****DIAG over TCP creat soket error:%s(%d)\n", strerror(errno),errno);
			sleep(1);
		}
	}while(diag_target.socket < 0);
	if (!diagIntData.m_bIsClient)
	{
		setsockopt(diag_target.socket, SOL_SOCKET, SO_REUSEADDR, &addr_reuse, sizeof(addr_reuse));
		memset(&serv_addr,0,sizeof(serv_addr));
		serv_addr.sin_family      = AF_INET;
		serv_addr.sin_port        = m_sUDP_Port;
		serv_addr.sin_addr.s_addr = m_lTavorAddress;
		do
		{
			ret = bind(diag_target.socket,(const struct sockaddr*)&serv_addr,sizeof(serv_addr));
			if(ret == -1)
				DIAGPRINT("******DIAG over TCP bind socket error:%s\n",strerror(errno));
		}while(ret < 0);
		DIAGPRINT("*******DIAG over TCP bind socket successful!\n");

	} else
	{
		memset(&serv_addr,0,sizeof(serv_addr));
		serv_addr.sin_family      = AF_INET;
		serv_addr.sin_port        = m_sUDP_Port;
		serv_addr.sin_addr.s_addr = m_lAddress;
		//DIAGPRINT("***** DIAG SendUDPMsg to port %x, length:%d *****\n",m_sUDP_Port, len);
	}
	if(!diagIntData.m_bIsClient)
	{
		do
		{
			ret = listen(diag_target.socket, 0);
			if(ret == -1)
				DIAGPRINT("******DIAG over TCP listen socket error:%s\n",strerror(errno));
		}while(ret <0);
		DIAGPRINT("*****DIAG over TCP listen socket successful!\n");
	}
	else
	{
		do
		{
			ret = connect(diag_target.socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
			DIAGPRINT("*****DIAG connect to the server:%d\n", ret);
			if(diag_target.socket == -1)
			{
				DIAGPRINT("*****DIAG socket is closed\n");
				diag_unlock_L(tcp_state.cpsr, &tcp_state.lock);
				return;
			}
			sleep(1);
		}while(ret < 0);
	}

	if(!diagIntData.m_bIsClient)
	{
		do
		{
			addr_size = sizeof(peer_addr);
			ret = accept(diag_target.socket, (struct sockaddr *)&peer_addr, (socklen_t*)&addr_size);
			if(ret == -1)
				DIAGPRINT("******DIAG over TCP accept socket error:%s\n",strerror(errno));

			if(diag_target.socket == -1)
			{
				DIAGPRINT("*****DIAG socket is closed\n");
				if(ret > 0) close(ret);
				diag_unlock_L(tcp_state.cpsr, &tcp_state.lock);
				return;
			}

		}while(ret <0);
		DIAGPRINT("*****DIAG over TCP connected with:****\nport=%d\naddr=%d.%d.%d.%d\n*******************************\n",
					peer_addr.sin_port,
					(peer_addr.sin_addr.s_addr&0x000000FF),
					(peer_addr.sin_addr.s_addr&0x0000FF00)>>8,
					(peer_addr.sin_addr.s_addr&0x00FF0000)>>16,
					(peer_addr.sin_addr.s_addr&0xFF000000)>>24);

		diag_target.hPort = ret;

	}
	else
	{
		diag_target.hPort = diag_target.socket;
	}
	tcp_state.state =1;
	diag_unlock_L(tcp_state.cpsr, &tcp_state.lock);
	DIAGPRINT("*****DIAG over TCP Notice Rcv Task:%d!\n",diag_event.pipefd[1]);
	write(diag_event.pipefd[1], "wake up",10);
	//struct timeval timeout = {1,0};
	//setsockopt(diag_target.hPort, SOL_SOCKET, SO_SNDTIMEO,(char *)&timeout,sizeof(struct timeval));
	//diagExtIFstatusConnectNotify();
	diagCommL2Init	();
	diagUsbConnectNotify();
	return;
}
/***********************************************************************
* Function: connectFS                                                  *
************************************************************************
* Description:  Connect File System (SD card) interface                *
* Parameters:   none                                                   *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
void connectFS()
{
	UINT32 hf; //header field
	char filename[256];
	char format[256];
	int fd;
	int err;
	static CHAR usb_comm_ver_id[] = { 0x10, 0x00, 0x00, 0x00, 0x00, DB_VERSION_SERVICE, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	//static CHAR usb_apps_ver_id[] = { 0x10, 0x00, 0x00, 0x00, 0x80, DB_VERSION_SERVICE, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	//If  O_CREAT  and  O_EXCL  are set, open() shall fail if the file exists.
	OSASemaphoreAcquire(connect_fs_sem, OSA_SUSPEND);
	if (log_file_num >= max_log_file_num) log_file_num = 0;
	sprintf(format, "%s%s", log_file_rel_path, log_file_path);
	sprintf(filename, format, log_file_num);
	DIAGPRINT("***** DIAG over FS: open the log file:%s\n", filename);

	fd = hPort;
	if (diagIntData.diagCachedWrite.write_unit_size)
	{
		cacheFlush();
	}
	diag_count = 0;
	//hPort = open(filename, O_WRONLY|O_CREAT|O_TRUNC|O_SYNC|O_DSYNC, 0x666 /*S_IRWXU|S_IRWXG|S_IRWXO*/);
	hPort = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0x666); //improve performance
	if (hPort == -1)
	{
		err = errno;
		/*---------------------------------*/
		/* Create non-existent directories */
		/*---------------------------------*/
		if ((err == ENOENT) || (err == ENOTDIR))
		{
			struct stat buf;
			char *src = filename;
			char *dst = format;
			while (*src != 0)
			{
				if ((*src == '/') || (*src == '\\'))
				{
					*dst = 0;
					if (*format != 0)
					{
						if (stat(format, &buf) < 0)
						{
							MMI_Report(MMI_REP_STAT, errno);
							fprintf(stderr, "***** DIAG over FS: non-existent directory for the log file: '%s' *****\r\n", format);
							if (mkdir(format, 0x666) < 0)
							{
								MMI_Report(MMI_REP_MKDIR, errno);
								fprintf(stderr, "***** DIAG over FS: failed to create this directory - reason %s. *****\r\n", strerror(err));
							}
						}
						else
						{
							fprintf(stderr, "***** DIAG over FS: '%s' exist *****\r\n", format);
						}
					}
				}
				*dst = *src;
				dst++;
				src++;
			}
			//retry to open now!
			//hPort = open(filename, O_WRONLY|O_CREAT|O_TRUNC|O_SYNC|O_DSYNC, 0x666 /*S_IRWXU|S_IRWXG|S_IRWXO*/);
			hPort = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0x666); //improve performance
			err = errno;
		}
		/*- - - - - - - - - - - - - - - - -*/
	}
	if (hPort == -1)
	{
		MMI_Report(MMI_REP_OPEN, err);
		fprintf(stderr, "***** DIAG over FS: Failed to initialize FS connection - reason %s. *****\r\n", strerror(err));
		if (m_bFirstConnected)
		{
			m_bFirstConnected = 0;
		}
		else
		{
			//DIAG_ASSERT(0);
		}
	}

	if (fd != -1) // We are already connected
	{
/*		fprintf(stderr,"***** DIAG over FS: Request connect, but hPort not INVALID *****\r\n");
		fprintf(stderr,"***** DIAG over FS: Close (before hPort==%d) *****\r\n",fd);
 #if defined ( _POSIX_SYNCHRONIZED_IO)
		fprintf(stderr,"***** DIAG over FS: _POSIX_SYNCHRONIZED_IO is defined *****\r\n");
 #else
		fprintf(stderr,"***** DIAG over FS: _POSIX_SYNCHRONIZED_IO is NOT defined *****\r\n");
 #endif*/

		/* degrades performabce
		   sync();
		   if (fsync(fd)==-1)
		   {
			int err=errno;
			MMI_Report(MMI_REP_FSYNC,err);
			fprintf(stderr,"***** DIAG over FS fsync error: %s *****\r\n",strerror(err));
		   }
		 */
		if (updateCommDBverOnClose)
		{
			updateCommDBverOnClose = 0;
			if (lseek(fd, ((sdl_header_ver == 1) ? 3 : 4) * 4, SEEK_SET) == -1)
			{
				int err = errno;
				MMI_Report(MMI_REP_LSEEK, err);
				fprintf(stderr, "***** DIAG over FS: connectFS: lseek: error: %s *****\r\n", strerror(err));
			}
			if (write(fd, &commDBver, 4) == -1)
			{
				int err = errno;
				MMI_Report(MMI_REP_WRITE, err);
				fprintf(stderr, "***** DIAG over FS: connectFS: commDBver: write: error: %s *****\r\n", strerror(err));
			}
			fprintf(stderr, "***** DIAG over FS: connectFS: updating the log file header Comm DB ver 0x%x *****\r\n", commDBver);
		}
		if (close(fd) == -1)
		{
			int err = errno;
			MMI_Report(MMI_REP_CLOSE, err);
			fprintf(stderr, "***** DIAG over FS: Close error: %s *****\r\n", strerror(err));
		}
	}
	if (hPort == -1) return;

	//fprintf(stderr,"***** DIAG over FS: FS connection Initialized. %s pid=%d hPort=%d *****\r\n",filename,getpid(),hPort);

	//Create the FS log header
	//------------------------

	//Header Ver
	total_log_tx += 4;
	if (diagIntData.diagCachedWrite.write_unit_size)
	{
		cachedWrite((UINT8 *)&sdl_header_ver, 4);
	}
	else
	{
		if (write(hPort, &sdl_header_ver, 4) == -1)
		{
			int err = errno;
			MMI_Report(MMI_REP_WRITE, err);
			fprintf(stderr, "***** DIAG over FS: Header Ver: write: error: %s *****\r\n", strerror(err));
		}
	}

	if (sdl_header_ver > 1)
	{
		//Header Length
		hf = 9 * 4;
		total_log_tx += 4;
		if (diagIntData.diagCachedWrite.write_unit_size)
		{
			cachedWrite((UINT8 *)&hf, 4);
		}
		else
		{
			if (write(hPort, &hf, 4) == -1)
			{
				int err = errno;
				MMI_Report(MMI_REP_WRITE, err);
				fprintf(stderr, "***** DIAG over FS: Header Length: write: error: %s *****\r\n", strerror(err));
			}
		}
	}

	//Data Format: 1-USB, 2-UART(L2/L4), 3 - TBDIY New Header
	if (diagIntData.m_eConnectionType == tUSBConnection)
	{
		hf = 1;
	}
	else
	{
		hf = 2;
	}
	total_log_tx += 4;
	if (diagIntData.diagCachedWrite.write_unit_size)
	{
		cachedWrite((UINT8 *)&hf, 4);
	}
	else
	{
		if (write(hPort, &hf, 4) == -1)
		{
			int err = errno;
			MMI_Report(MMI_REP_WRITE, err);
			fprintf(stderr, "***** DIAG over FS: Header Data Format: write: error: %s *****\r\n", strerror(err));
		}
	}

	//APPS Ver
	if (sscanf(DBversionID, "0x%lx", (long unsigned int *)&hf) != 1)
	{
		hf = 0;
	}
	total_log_tx += 4;
	if (diagIntData.diagCachedWrite.write_unit_size)
	{
		cachedWrite((UINT8 *)&hf, 4);
	}
	else
	{
		if (write(hPort, &hf, 4) == -1)
		{
			int err = errno;
			MMI_Report(MMI_REP_WRITE, err);
			fprintf(stderr, "***** DIAG over FS: Header APPS Ver: write: error: %s *****\r\n", strerror(err));
		}
	}

	//COMM Ver
	total_log_tx += 4;
	if (diagIntData.diagCachedWrite.write_unit_size)
	{
		cachedWrite((UINT8 *)&commDBver, 4);
	}
	else
	{
		if (write(hPort, &commDBver, 4) == -1)
		{
			int err = errno;
			MMI_Report(MMI_REP_WRITE, err);
			fprintf(stderr, "***** DIAG over FS: Header COMM Ver: write: error: %s *****\r\n", strerror(err));
		}
	}
	if (!commDBver)
	{
		updateCommDBverOnClose = 1;
	}

	//Sequence
	log_file_num++;
	hf = log_file_index++;
	total_log_tx += 4;

	if (diagIntData.diagCachedWrite.write_unit_size)
	{
		cachedWrite((UINT8 *)&hf, 4);
	}
	else
	{
		if (write(hPort, &hf, 4) == -1)
		{
			int err = errno;
			MMI_Report(MMI_REP_WRITE, err);
			fprintf(stderr, "***** DIAG over FS: Header Sequence: write: error: %s *****\r\n", strerror(err));
		}
	}

	//Date/Time
	hf = time(NULL);
	total_log_tx += 4;
	if (diagIntData.diagCachedWrite.write_unit_size)
	{
		cachedWrite((UINT8 *)&hf, 4);
	}
	else
	{
		if (write(hPort, &hf, 4) == -1)
		{
			int err = errno;
			MMI_Report(MMI_REP_WRITE, err);
			fprintf(stderr, "***** DIAG over FS: Header Date/Time: write: error: %s *****\r\n", strerror(err));
		}
	}

	//Checksum
	hf = 0;
	total_log_tx += 4;
	if (diagIntData.diagCachedWrite.write_unit_size)
	{
		cachedWrite((UINT8 *)&hf, 4);
	}
	else
	{
		if (write(hPort, &hf, 4) == -1)
		{
			int err = errno;
			MMI_Report(MMI_REP_WRITE, err);
			fprintf(stderr, "***** DIAG over FS: Header Checksum: write: error: %s *****\r\n", strerror(err));
		}
	}

	//Signature
	if (sdl_header_ver > 1)
	{
		total_log_tx += 4;
		if (diagIntData.diagCachedWrite.write_unit_size)
		{
			cachedWrite((UINT8 *)&sdl_signature, 4);
		}
		else
		{
			if (write(hPort, &sdl_signature, 4) == -1)
			{
				int err = errno;
				MMI_Report(MMI_REP_WRITE, err);
				fprintf(stderr, "***** DIAG over FS: Signature: write: error: %s *****\r\n", strerror(err));
			}
		}
	}

	///////////////////////////
	// Request DB version ID //
	///////////////////////////
	if ((diagIntData.m_bWorkMultiCore) && (!commDBver))
	{
		int wait_timeout = 5;
		DIAGPRINT("******DIAG over FS:requeset DB version ID !!!\n");
		while((!bDiagConnAcked)&&(wait_timeout))
		{
			sleep(1);
			wait_timeout--;
		}
		diagCommSetChunk(dataExtIf.RxPacket, usb_comm_ver_id, 16, 1); //Linux runs on the apps side and we need the comm ver id in the log file
		dispatchServiceForSAP((UINT8 *)dataExtIf.RxPacket->buffer);
	}
	OSASemaphoreRelease(connect_fs_sem);
}

/***********************************************************************
* Function: connectCOMM                                                *
************************************************************************
* Description:  Connect USB/UART interface                             *
* Parameters:   none                                                   *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
void connectCOMM()
{
	struct termios tio;
	memset(&tio, 0x0, sizeof(tio));
	//OSA_STATUS status;
#if 0
	if (hPort != -1)		// We are already connected
	{
		fprintf(stderr, "***** DIAG COMM: request connect, but hPort not INVALID *****\r\n");
		return;
	}

	memset(&tio, 0, sizeof(tio));
	if (diagIntData.m_eConnectionType == tUSBConnection)
	{
		UINT32 i;
		for (i = 0; i < 10; i++)
		{
			hPort = open(DIAG_TTY_USB_DEVICE, O_RDWR | O_NOCTTY | O_NDELAY); //TBDIY clear O_NONBLOCK
			if (hPort == -1)
			{
				fprintf(stderr, "***** DIAG: Failed to initialize USB connection %s - reason %s try #%d. *****\r\n",
					DIAG_TTY_USB_DEVICE, strerror(errno), i);
				sleep(1);
			}
			else
			{
				break;
			}
		}
	}
	else  //Diag over UART
	{
		DIAGPRINT("***** DIAG: before open UART *****\r\n");
		hPort = open(DIAG_TTY_UART_DEVICE,O_RDWR);
		DIAGPRINT("***** DIAG: after open UART hPort=%d*****\r\n",hPort);
	}

	/* Initialize tty port */
	if (hPort == -1)
	{
		if (diagIntData.m_eConnectionType != tUSBConnection)
		{
			fprintf(stderr, "***** DIAG: Failed to initialize UART connection %s - reason %s. *****\r\n", DIAG_TTY_UART_DEVICE, strerror(errno));
		}
		// we tried and tried to setup the port - nothing helps!
		// since this is called on first time always (before USB connection is checked)
		if (m_bFirstConnected)
		{
			m_bFirstConnected = 0;
		}
		else
		{
			//DIAG_ASSERT(0);
		}
		return;
	}

	if (diagIntData.m_eConnectionType == tUSBConnection)
	{
		fcntl(hPort, F_SETFL, 0 /*FASYNC*/);

		/* Configure termios */
		tio.c_cflag = BAUD | CRTSCTS | CS8 | CLOCAL | CREAD;
		tio.c_iflag = IGNPAR;
		tio.c_oflag = 0;
		tio.c_lflag = 0;
		tio.c_cc[VINTR]    = 0; 	/* Ctrl-c */
		tio.c_cc[VQUIT]    = 0; 	/* Ctrl-\ */
		tio.c_cc[VKILL]    = 0; 	/* @ */
		tio.c_cc[VEOF]	   = 4; 	/* Ctrl-d */
		tio.c_cc[VTIME]    = 0; 	/* inter-character timer unused */
		tio.c_cc[VMIN]	   = 1; 	/* blocking read until 1 character arrives */
		tio.c_cc[VSWTC]    = 0; 	/* '\0' */
		tio.c_cc[VSTART]   = 0; 	/* Ctrl-q */
		tio.c_cc[VSTOP]    = 0; 	/* Ctrl-s */
		tio.c_cc[VSUSP]    = 0; 	/* Ctrl-z */
		tio.c_cc[VREPRINT] = 0; 	/* Ctrl-r */
		tio.c_cc[VDISCARD] = 0; 	/* Ctrl-u */
		tio.c_cc[VWERASE]  = 0; 	/* Ctrl-w */
		tio.c_cc[VLNEXT]   = 0; 	/* Ctrl-v */
		tio.c_cc[VEOL2]    = 0; 	/* '\0' */
		tio.c_cc[VERASE] = 0x8; 	/* del */
		tio.c_cc[VEOL]	 = 0xD; 	/* '\0' */

		tcflush(hPort, TCIFLUSH);
		tcsetattr(hPort, TCSANOW, &tio);

		fprintf(stderr, "***** DIAG: Initialized USB connection %s. *****\r\n", DIAG_TTY_USB_DEVICE);
	}
	else
	{
		/* Configure termios */
		tio.c_cflag = BAUD | CS8 | CLOCAL | CREAD;
		tio.c_iflag = 0;
		tio.c_oflag = 0;
		tio.c_lflag = 0;
		tio.c_cc[VMIN] = 1;
		tio.c_cc[VTIME] = 0;

		tcflush(hPort, TCIOFLUSH); //flush both data received but not read and data written but not transmitted.
		tcsetattr(hPort, TCSANOW, &tio); //the change shall occur immediately

		fprintf(stderr, "***** DIAG: Initialized UART connection %s. *****\r\n", DIAG_TTY_UART_DEVICE);
	}

	//hUsbPortFunctional=1;
	DIAGPRINT("***** DIAG USB / UART Initialization Complete Successfuly *****\r\n");
#else
	hPort = open(DIAG_TTY_UART_DEVICE, O_RDWR);
	//hPort = open(DIAG_TTY_UART_DEVICE,O_RDWR | O_NOCTTY | O_NDELAY);

	//fcntl(hPort, F_SETFL, 0 /*FASYNC*/);

	/* Configure termios */
	tio.c_cflag = BAUD | CS8 | CLOCAL | CREAD;
	tio.c_iflag = 0;
	//tio.c_iflag = IGNPAR;
	tio.c_oflag = 0;
	tio.c_lflag = 0;
	tio.c_cc[VMIN] = 1;
	tio.c_cc[VTIME]= 0;

	tcflush(hPort, TCIOFLUSH); //flush both data received but not read and data written but not transmitted.
	tcsetattr(hPort, TCSANOW, &tio); //the change shall occur immediately

#endif
#if !defined (DIAGNEWHEADER)
	// we have socket, lets reset the L2 rx- state machine
#ifdef UART_NON_RAW_DIAG
	if (diagIntData.m_eConnectionType == tUARTConnection)
	{
		diagCommL2Init();
		diagL4InitHSprotocolWithCB(DIAG_COMMDEV_EXT, (DiagHSprotocolSendCB)SendCommMsg);
	}
#endif
#endif
}


/***********************************************************************
* Function: disconnectFS                                               *
************************************************************************
* Description:  Disconnect File System interface                       *
* Parameters:   none                                                   *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
void disconnectFS()
{
	if (hPort != -1)
	{
		fprintf(stderr, "***** DIAG over FS: Close (before hPort==%d) *****\r\n", hPort);

		/* degrades performabce
		   sync();
		   if (fsync(hPort)==-1)
		   {
			int err=errno;
			MMI_Report(MMI_REP_FSYNC,err);
			fprintf(stderr,"***** DIAG over FS fsync error: %s *****\r\n",strerror(err));
		   }
		 */

		if (updateCommDBverOnClose)
		{
			updateCommDBverOnClose = 0;
			if (diagIntData.diagCachedWrite.write_unit_size)
			{
				cacheFlush();
			}
			if (lseek(hPort, ((sdl_header_ver == 1) ? 3 : 4)*4, SEEK_SET) == -1)
			{
				int err = errno;
				MMI_Report(MMI_REP_LSEEK, err);
				fprintf(stderr, "***** DIAG over FS: disconnectFS: lseek: error: %s *****\r\n", strerror(err));
			}
			if (write(hPort, &commDBver, 4) == -1)
			{
				int err = errno;
				MMI_Report(MMI_REP_WRITE, err);
				fprintf(stderr, "***** DIAG over FS: disconnectFS: commDBver: write: error: %s *****\r\n", strerror(err));
			}
			fprintf(stderr, "***** DIAG over FS: disconnectFS: updating the log file header Comm DB ver 0x%x *****\r\n", commDBver);
		}
		else
		{
			if (diagIntData.diagCachedWrite.write_unit_size)
			{
				cacheFlush();
			}
		}
		if (close(hPort) == -1)
		{
			int err = errno;
			MMI_Report(MMI_REP_CLOSE, err);
			fprintf(stderr, "***** DIAG over FS: Close error: %s *****\r\n", strerror(err));
		}
		hPort = -1;
	}

	//Reset the log file size counter for the next connectFS()
	total_log_tx = 0;

	///////////////////////////////////////////////////////////////
	//Before we can remove the SD card,                          //
	//ensure that everything in memory is written to the SD card.//
	sync();
	sleep(2); //wait 2 more secs - just to be on the safe side ;-)
	MMI_Report(MMI_REP_STOP_DONE, 0);
	///////////////////////////////////////////////////////////////
	fprintf(stderr, "***** DIAG over FS: Close() *****\r\n");
}

/***********************************************************************
* Function: disconnectNet                                               *
************************************************************************
* Description:  Disconnect Net interface                       *
* Parameters:   none                                                   *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
void disconnectNet()
{
	int fd = 0;
	int ret = -1;
	if(diag_target.hPort != -1)
	{
		fd = diag_target.hPort;
		DIAGPRINT("Closing stream:%d...\n", fd);
		diag_target.hPort = -1;
		diag_lock_L(&tcp_state.cpsr,&tcp_state.lock);
		tcp_state.state = 0;
		diag_unlock_L(tcp_state.cpsr, &tcp_state.lock);
		close(fd);
		DIAGPRINT("Closed stream!\n");
	}
	if(diag_target.socket != -1)
	{
		fd = diag_target.socket;
		DIAGPRINT("Closing socket:%d...\n", fd);
		diag_target.socket = -1;
		if(diagIntData.m_eConnectionType == tTCPConnection)
			ret = shutdown(fd, SHUT_RDWR);
		ret =close(fd);
		DIAGPRINT("Closed socket:%d!\n",ret);
	}
}
/***********************************************************************
* Function: disconnectCOMM                                             *
************************************************************************
* Description:  Disconnect USB/UART if                                 *
* Parameters:   none                                                   *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
void disconnectCOMM()
{
	int fd;
	diagUsbDisConnectNotify();
	DIAGPRINT(">>>diagDisconnect , diagUsbDisConnectNotify\n");
	//Diag over FS (SD card) support
	if (diagIntData.m_eLLtype == tLL_FS || diagIntData.m_eLLtype == tLL_SC)
	{
		return disconnectFS();
	}

	//hUsbPortFunctional=0;
	if (hPort != -1)
	{
		fd = hPort;
		if (diagIntData.diagCachedWrite.write_unit_size)
		{
			cacheFlush();
		}
		hPort = -1;
		fprintf(stderr, "***** DIAG COMM Close(hPort==%d) *****\r\n", fd);
		close(fd);  //Close the Handle from the Create File (Close the port)
	}
	disconnectNet();
	fprintf(stderr, "***** DIAG COMM Close() *****\r\n");
}

#if defined(K2UEVENT_LIB)
void AddUSBCallBackfunction(    __attribute__((unused)) CHAR* TransferBuffer,
				__attribute__((unused)) int * ReadLen,
				__attribute__((unused)) char * subsys,
				__attribute__((unused)) char* usr_str_val,
				__attribute__((unused)) char* action)
{
	/* plug in or out? */
	/* FIXME: detect /dev/ttygserial to avoid endless loop */
	if (access(DIAG_TTY_USB_DEVICE, F_OK) < 0) //existence test
	{
		fprintf(stderr, "***** DIAG over FS: TTY device error: %s *****\r\n", strerror(errno));
		return;
	}

	fprintf(stderr, "***** AddDiagCallBackfunction(). DIAG EXT RX Netlink add *****\r\n");
	lastUSBevent = 1;
	diagExtIFstatusConnectNotify();

}

void RemoveUSBCallBackfunction(         __attribute__((unused)) CHAR* TransferBuffer,
					__attribute__((unused)) int * ReadLen,
					__attribute__((unused)) char * subsys,
					__attribute__((unused)) char* usr_str_val,
					__attribute__((unused)) char* action)
{
	fprintf(stderr, "***** RemoveDiagCallBackfunction(). DIAG EXT RX Netlink remove *****\r\n");
	lastUSBevent = 2;
	diagExtIFStatusDiscNotify();
}
#else
#if 0
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
static const char *searchNetlink(const char *key, const char *buf, size_t len)
{
	size_t curlen;
	size_t keylen = strlen(key);
	char *cur = (char *)buf; //not const

	while (cur < buf + len - keylen)
	{
		curlen = strlen(cur);
		if (curlen == 0)
			break;
		if (!strncmp(key, cur, keylen) && cur[keylen] == '=')
			return cur + keylen + 1;
		cur += curlen + 1;
	}

	return NULL;
}
#endif
#endif

void mmiStartCBfunc(const char* rel_path)
{
	DIAGPRINT(">> Enter mmiStartCBfunc\n");
	strcpy(log_file_rel_path, rel_path);
	DIAGPRINT("***** DIAG over FS: MMI START path='%s' *****\r\n", log_file_rel_path);
	fprintf(stderr, "***** DIAG over FS: MMI START path='%s' *****\r\n", log_file_rel_path);
	diagExtIFstatusConnectNotify();
}

//#define CACHED_WRITE_DBG
CachedWriteS cw; //global is zero by default

OSA_STATUS InitDiagCachedWrite(void)
{
	OSA_STATUS status = OS_SUCCESS;

	if (diagIntData.diagCachedWrite.write_unit_size)
	{
		fprintf(stderr, "***** DIAG Diag Cached Write is Enabled *****\r\n");
		status = OSATimerCreate(&cw.diag_cached_write_timer_ref);
		DIAG_ASSERT(status == OS_SUCCESS);
		cw.cachedWriteBuff = malloc(2 * diagIntData.diagCachedWrite.write_unit_size); //Not free
		DIAG_ASSERT(cw.cachedWriteBuff != 0);
		status = OSA_MutexCreate(&cw.diag_cached_write_mutex, NULL);
		DIAG_ASSERT(status == OS_SUCCESS);
	}

	return status;
}


BOOL InitLog2SDcard(void)
{
	BOOL retVal = FALSE;
	OSA_STATUS status;

	if (diagIntData.m_eLLtype == tLL_FS || diagIntData.m_eLLtype == tLL_SC)
	{
		status = InitDiagCachedWrite();

		if (status != OS_SUCCESS)
		{
			fprintf(stderr, "*****InitDiagCachedWrite eror ... *****\r\n");
			return FALSE;
		}
		if (MMI_Init(mmiStartCBfunc, diagExtIFStatusDiscNotify, diagIntData.m_eLLtype, &diagIntData.diagBSPFScfg))
		{
			fprintf(stderr, "***** DIAG over FS: MMI_Init returned successfully!!! *****\r\n");
		}
		else
		{
			fprintf(stderr, "***** DIAG over FS: MMI_Init failed!!! *****\r\n");
		}

		if (diagIntData.diagBSPFScfg.log_auto_start)
		{
			fprintf(stderr, "***** DIAG over FS: AUTO START ... *****\r\n");
			MMI_Report(MMI_REP_AUTO, 0);
		}

		retVal = TRUE;
	}

	return retVal;
}

/***********************************************************************
* Function: DiagCachedWriteTimerCallback                               *
************************************************************************
* Description: Notifying diag multi tx task                            *
*                                                                      *
* Parameters:                                                          *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
void DiagCachedWriteTimerCallback(UINT32 param)
{
	DIAG_UNUSED_PARAM(param)
	if (cw.cachePtr)
	{
		cacheFlush(); //protected within if (diagIntData.diagCachedWrite.write_unit_size)
	}
	else
	{
#if defined (CACHED_WRITE_DBG)
		fprintf(stderr, "***** DIAG COMM TX DiagCachedWriteTimerCallback expired not dirty *****\r\n");
#endif
	}
}
/***********************************************************************
* Function: cacheFlush				                                   *
************************************************************************
* Description:                                                         *
*                                                                      *
* Parameters:                                                          *
*                                                                      *
* Return value:                                                        *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
static UINT32 cacheFlush()
{
	ssize_t ret;

	OSA_MutexLock(&cw.diag_cached_write_mutex, OSA_SUSPEND);
#if defined (CACHED_WRITE_DBG)
	fprintf(stderr, "***** DIAG COMM TX cacheFlush cachePtr=%d *****\r\n", cw.cachePtr);
#endif
	if (cw.cachePtr > 0)
	{
		if (tLL_ETHERNET == diagIntData.m_eLLtype || tLL_LocalIP == diagIntData.m_eLLtype)
		{
			if(diag_target.send_target != NULL)
				ret = diag_target.send_target(cw.cachedWriteBuff, cw.cachePtr);
		}
		else
		{
			ret = write(hPort, cw.cachedWriteBuff, cw.cachePtr);
		}
		if((ret < 0) || ((UINT32)ret != cw.cachePtr))
		{
			DIAGPRINT("***** DIAG COMM TX cachedWrite loss!. Error: %s *****\r\n", strerror(errno));
		}
		cw.cachePtr = 0; //no retry
		if (ret == -1)
		{
			int err = errno;
			if (diagIntData.m_eLLtype == tLL_FS || diagIntData.m_eLLtype == tLL_SC)
			{
				MMI_Report(MMI_REP_WRITE, err);
			}
			fprintf(stderr, "***** DIAG COMM TX cachedWrite failed. Error: %s *****\r\n", strerror(err));
			OSA_MutexUnlock(&cw.diag_cached_write_mutex);
			return FALSE;
		}
	}
	OSA_MutexUnlock(&cw.diag_cached_write_mutex);
	return TRUE;
}
/***********************************************************************
* Function: cachedWrite				                                   *
************************************************************************
* Description:                                                         *
*                                                                      *
* Parameters:                                                          *
*                                                                      *
* Return value:                                                        *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
static UINT32 cachedWrite(UINT8 *msg, UINT32 len)
{
	ssize_t ret;

	/*
	   1. cachePtr+len>2*write_unit_size => Flush the buffer. cachePtr=0.
	   2. len>2*write_unit_size => If cachePtr>0 {Flush the buffer. cachePtr=0.} Write the new msg with no caching.
	   3. cachePtr+len>=write_unit_size => Cache the new msg and flush the buffer. cachePtr=0.
	   4. else => Cache the new msg. cachePtr+=len.
	 */

	OSA_MutexLock(&cw.diag_cached_write_mutex, OSA_SUSPEND);
	//AGPRINT( "***** DIAG COMM TX cachedWrite cachePtr=%d len=%d *****\r\n", cw.cachePtr, len);
	/*1*/
	if ((cw.cachePtr + len > 2 * diagIntData.diagCachedWrite.write_unit_size) && (cw.cachePtr))
	{
#if defined (CACHED_WRITE_DBG)
		fprintf(stderr, "***** DIAG COMM TX cachedWrite cachePtr=%d len=%d *****\r\n", cw.cachePtr, len);
#endif

		if (tLL_ETHERNET == diagIntData.m_eLLtype || tLL_LocalIP == diagIntData.m_eLLtype)
		{
			if(diag_target.send_target != NULL)
				ret = diag_target.send_target(cw.cachedWriteBuff, cw.cachePtr);
		}
		else
		{
			ret = write(hPort, cw.cachedWriteBuff, cw.cachePtr);
		}
		if((ret < 0) || ((UINT32)ret != cw.cachePtr))
		{
			DIAGPRINT(  "***** DIAG COMM TX cachedWrite loss!. Error: %s *****\r\n", strerror(errno));
		}
		cw.cachePtr = 0; //no retry
		if (ret == -1)
		{
			int err = errno;
			if (diagIntData.m_eLLtype == tLL_FS || diagIntData.m_eLLtype == tLL_SC)
			{
				MMI_Report(MMI_REP_WRITE, err);
			}
			DIAGPRINT(  "***** DIAG COMM TX cachedWrite failed. Error: %s *****\r\n", strerror(err));
			OSA_MutexUnlock(&cw.diag_cached_write_mutex);
			return FALSE;
		}
	}
	/*2*/
	if (len > 2 * diagIntData.diagCachedWrite.write_unit_size)
	{
#if defined (CACHED_WRITE_DBG)
		fprintf(stderr, "***** DIAG COMM TX cachedWrite len=%d *****\r\n", len);
#endif

		if (tLL_ETHERNET == diagIntData.m_eLLtype || tLL_LocalIP == diagIntData.m_eLLtype)
		{

#if defined (CACHED_WRITE_DBG)
			fprintf(stderr, "***** DIAG COMM TX Can not send UDP message ===> len=%d *****\r\n", len);
#endif
			if(diag_target.send_target != NULL)
				ret = diag_target.send_target(msg, len);
		}
		else
		{
			ret = write(hPort, msg, len);
		}
		if((ret < 0) || ((UINT32)ret != len))
		{
			DIAGPRINT(  "***** DIAG COMM TX cachedWrite loss!. Error: %s *****\r\n", strerror(errno));
		}
		if (ret == -1)
		{
			int err = errno;
			if (diagIntData.m_eLLtype == tLL_FS || diagIntData.m_eLLtype == tLL_SC)
			{
				MMI_Report(MMI_REP_WRITE, err);
			}
			fprintf(stderr, "***** DIAG COMM TX cachedWrite failed. Error: %s *****\r\n", strerror(err));
			OSA_MutexUnlock(&cw.diag_cached_write_mutex);
			return FALSE;
		}
		else
		{
			OSA_MutexUnlock(&cw.diag_cached_write_mutex);
			return TRUE;
		}
	}
	/*3+4*/
	/*In the next lines of code we use cw.cachePtr+len and not increasing yet (cw.cachePtr+=len;) for the following 'if (!cw.cachePtr)' logic. */
	memcpy(cw.cachedWriteBuff + cw.cachePtr, msg, (size_t)len);
	if (cw.cachePtr + len > diagIntData.diagCachedWrite.write_unit_size)
	{
#if defined (CACHED_WRITE_DBG)
		fprintf(stderr, "***** DIAG COMM TX cachedWrite cachePtr=%d len=%d *****\r\n", cw.cachePtr + len, len);
#endif
		if (tLL_ETHERNET == diagIntData.m_eLLtype || tLL_LocalIP == diagIntData.m_eLLtype)
		{
			if(diag_target.send_target != NULL)
				ret = diag_target.send_target(cw.cachedWriteBuff, cw.cachePtr + len);
		}
		else
		{
			ret = write(hPort, cw.cachedWriteBuff, cw.cachePtr + len);
		}
		if((ret < 0) || ((UINT32)ret != (cw.cachePtr+ len)))
		{
			DIAGPRINT(  "***** DIAG COMM TX cachedWrite loss!. Error: %s *****\r\n", strerror(errno));
		}
		cw.cachePtr = 0; //no retry
		if (ret == -1)
		{
			int err = errno;
			if (diagIntData.m_eLLtype == tLL_FS || diagIntData.m_eLLtype == tLL_SC)
			{
				MMI_Report(MMI_REP_WRITE, err);
			}
			fprintf(stderr, "***** DIAG COMM TX cachedWrite failed. Error: %s *****\r\n", strerror(err));
			OSA_MutexUnlock(&cw.diag_cached_write_mutex);
			return FALSE;
		}
	}
	else
	{
		/*In the previous lines of code we used cw.cachePtr+len and didn't increas (cw.cachePtr+=len;) for the following 'if (!cw.cachePtr)' logic. */
		if (!cw.cachePtr)
		{
			OSA_STATUS status;
			status = OSATimerStart(cw.diag_cached_write_timer_ref, diagIntData.diagCachedWrite.time_out, 0, DiagCachedWriteTimerCallback, 0);
			DIAG_ASSERT(status == OS_SUCCESS);
		}
		cw.cachePtr += len;
	}
	OSA_MutexUnlock(&cw.diag_cached_write_mutex);
	return TRUE;
}

/***********************************************************************
* Function: SendCommMsg				                                   *
************************************************************************
* Description: Sends a message over UART interface (for L2 handshake)  *
*                                                                      *
* Parameters:                                                          *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
UINT32 SendCommMsg(UINT8 *msg, UINT32 len)
{
	/*UINT32 dwNumBytesWritten;*/
	ssize_t ret;

	/*ret = WriteFile (hPort,						// Port handle
	   _diagExtIfReportsList[pos].virtualReportPtr,	// Pointer to the data to write
	   _diagExtIfReportsList[pos].reportLength,		// Number of bytes to write
	   &dwNumBytesWritten,						// Pointer to the number of bytes written
	   NULL									// Must be NULL for Windows CE
	   );*/
	OSASemaphoreAcquire(send_comm_msg_sem, OSA_SUSPEND);
	if(diag_server ==0)
	{
		ret = diag_target.send_target(msg, len);
		OSASemaphoreRelease(send_comm_msg_sem);
		return ret;
	}
	if (diagIntData.m_eLLtype == tLL_FS || diagIntData.m_eLLtype == tLL_SC)
	{

		if (total_log_tx + len > log_file_size)
		{
			total_log_tx = len;
			connectFS();
		}
		else
		{
			total_log_tx += len;
			//fprintf(stderr,"***** DIAG over FS: total_log_tx=%d pid=%d. *****\r\n",total_log_tx,getpid());
		}
		//////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////
		//// Parse the Comm DB version:                       ////
		//// --------------------------                       ////
		//// This is a critical path, but msg[4]==0x01 has    ////
		//// a minor cost. Most of the times this condition   ////
		//// fails and the other &&s are not checked at all.  ////
		//////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////
		//[4:5] SAP , [8:9] Module ID , [10:11] Msg ID
		//BUG: sometimes msg[9]!=0xff. We found a case where it was zero.
		//if (msg[4]==0x01 && msg[5]==0x00 && msg[8]==0xff && msg[9]==0xff && msg[10]==0x00 && msg[11]==0x00)
		if (msg[4] == 0x01 && msg[5] == 0x00 && msg[8] == 0xff && msg[10] == 0x00 && msg[11] == 0x00)
		{
			if (sscanf((char *)msg + 16, "0x%lx", (long unsigned int *)&commDBver) != 1)
			{
				commDBver = 0;
			}
			DIAGPRINT("*****DIAG over FS**** CB versiong read from msg:0x%lx\n",commDBver);
			//fprintf(stderr, "***** DIAG over FS: Comm DB VER: %s *****\r\n", msg+16);
			if (filterCommDBver != 0)
			{
				if (commDBver == filterCommDBver)
				{
					fprintf(stderr, "***** DIAG over FS: diagOverFSfilter COMM DB VERSION MATCH=0x%x !!! *****\r\n", filterCommDBver);
					MMI_Report(MMI_REP_COMM_FILTER_DB_VER_MATCH, 0);
				}
				else
				{
					fprintf(stderr, "***** DIAG over FS: diagOverFSfilter COMM DB VERSION DOES NOT MATCH img=0x%x filter=0x%x !!! *****\r\n", commDBver, filterCommDBver);
					MMI_Report(MMI_REP_COMM_FILTER_DB_VER_MISMATCH, 0);
				}
			}
		}
		//////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////
	}
	if (diagIntData.m_eConnectionType == tUSBConnection)
	{
//#define DEBUG_USB_MSG_LEN
#if defined (DEBUG_USB_MSG_LEN) //debug corrupted USB messages
		static UINT32 usb_msg_cnt = 0;
		UINT32 parse_msg_len;
		usb_msg_cnt++;
		parse_msg_len = msg[0] | (msg[1] << 8);
		if (len != parse_msg_len)
		{
			fprintf(stderr, "***** ITZIK TEST message length check - counter=0x%x !!! *****\r\n", usb_msg_cnt);
			fprintf(stderr, "***** ITZIK TEST message length check - corrupted message length: header=0x%x msg=0x%x !!! *****\r\n", parse_msg_len, len);
		}
		else
#endif
		//Make sure that the message is 4 Bytes aligned to prevent red messages in ACAT
		/*if ((len & 0x3) != 0)
		{
			len = (len | 0x3) + 1;
			msg[0] = len & 0xff;
			msg[1] = (len & 0xff00) >> 8;
		}*/
	}

	// Add len for UART RAWDIAG head
#ifndef UART_NON_RAW_DIAG
	if (diagIntData.m_eConnectionType == tUARTConnection)
	{
		//Make sure that the message is 4 Bytes aligned to prevent red messages in ACAT
	/*	if ((len & 0x3) != 0)
		{
			len = (len | 0x3) + 1;
			msg[0] = len & 0xff;
			msg[1] = (len & 0xff00) >> 8;
		} */
	}

#endif
	if (diagIntData.diagCachedWrite.write_unit_size)
	{
		ret = cachedWrite(msg, len);
	}
	else
	{
		ret = write(hPort, msg, len);
		if((ret < 0) || ((UINT32)ret != len))
			DIAGPRINT("*****DIAG COMM TX Error: ret(%d) <> len(%d)\n", ret, len);
		if (ret == -1 /*0*/)
		{
			int err = errno;
			if (diagIntData.m_eLLtype == tLL_FS || diagIntData.m_eLLtype == tLL_SC)
			{
				MMI_Report(MMI_REP_WRITE, err);
			}
			//Handle this error UINT32 le=GetLastError();
			fprintf(stderr, "***** DIAG COMM TX WriteFile failed. *****\r\n");
			fprintf(stderr, "Error: %s\n", strerror(err));
			ret = FALSE;
		}
		else
		{
			ret = TRUE;
		}
	}
	OSASemaphoreRelease(send_comm_msg_sem);

	return ret;
}
int SendCMIMsg(UINT8 *msg, UINT32 len, const void *clienaddr)
{

	struct sockaddr_un *clntaddr = (struct sockaddr_un *)clienaddr;
	int					ret = 0;
	UINT32	retry_cnt		= 20;
	int		iSocketLastError;
	if(diag_server_port == -1)
	{
		DIAGPRINT("*****DIAG SendUDPMsg socket = -1!***\n");
		return -1;
	}

	while(retry_cnt--)
	{
		if(cmi_state.state != 1)
		{
			DIAGPRINT("*****DIAG SendlocalMsg while binding... *****\r\n");
			return -1;
		}
		ret = sendto(diag_server_port, (CHAR *)msg, len, 0, (const struct sockaddr*)clntaddr, (socklen_t)sizeof(struct sockaddr_un));

		if (ret < 0)
		{
			iSocketLastError = errno;
			if (iSocketLastError == EAGAIN || iSocketLastError == EWOULDBLOCK /*WSAEWOULDBLOCK*/)
			{
				DPRINTF("************************************************\r\n");
				DPRINTF("***** DIAG SendUDPMsg WSAEWOULDBLOCK (%d) *****\r\n",retry_cnt);
				DPRINTF("************************************************\r\n");
				DPRINTF("Error: %s\n", strerror(iSocketLastError));
				usleep(200);
				continue;
			}
			m_iLostExtTxMessages++;
			DPRINTF("**********************************************************************\r\n");
			DPRINTF("***** DIAG SendUDPMsg failed iSocketLastError = %d, LostMsg#=%d *****\r\n",iSocketLastError,m_iLostExtTxMessages);
			DPRINTF("***** DIAG NOT WSAEWOULDBLOCK...                                *****\r\n");
			DPRINTF("**********************************************************************\r\n");
			DPRINTF("Error: %s\n", strerror(iSocketLastError));
			return -1;
		}
		else if (ret != (int)len)
		{
			DPRINTF("***** DIAG SendUDPMsg error in length *****\r\n");
			return ret;
		}
		else
		{
			if (20-retry_cnt>1)
			{
				DPRINTF("*********************************************\r\n");
				DPRINTF("***** DIAG SendUDPMsg OK after %02d tries *****\r\n",20-retry_cnt);
				DPRINTF("*********************************************\r\n");
			}
			return ret;
		}
	}
	return 0;
}
int SendLocalMsg(UINT8 *msg, UINT32 len)
{
	struct sockaddr_in sin;
	struct sockaddr_un clntaddr;
	int					ret = 0;
	UINT32	retry_cnt		= 20;
	int		iSocketLastError;
	//DIAGPRINT("******SendLocalMsg!len :%d\n",len);
	if(diag_target.hPort == -1)
	{
		DIAGPRINT("*****DIAG SendUDPMsg socket = -1!***\n");
		return -1;
	}
	if (diagIntData.m_bIsClient)
	{
		memset(&clntaddr, 0, sizeof(clntaddr));
		clntaddr.sun_family = AF_UNIX;
		strcpy(clntaddr.sun_path, SOCKET_PATH_MASTER);
	} else
	{
		memset(&sin,0,sizeof(sin));
		sin.sin_family      = AF_INET;
		sin.sin_port        = m_sUDP_Port;
		//DIAGPRINT("***** DIAG SendUDPMsg to port %x, length:%d *****\n",m_sUDP_Port, len);
	}
	while(retry_cnt--)
	{
		if(local_state.state != 1)
		{
			DIAGPRINT("*****DIAG SendlocalMsg while binding... *****\r\n");
			return -1;
		}

		if (diagIntData.m_bIsClient)
		{
			ret = sendto(diag_target.hPort, (CHAR *)msg, len, 0, (const struct sockaddr*)&clntaddr, (socklen_t)sizeof(clntaddr));
		} else
		{
			sin.sin_addr.s_addr = m_lAddress;
			ret = sendto(diag_target.hPort, (CHAR *)msg, len, 0, (const struct sockaddr*)/*(LPSOCKADDR)*/&sin, (socklen_t)sizeof(sin));
			//DIAGPRINT("*****DIAG Send UDP :%d****\n",ret);
		}
		if (ret < 0)
		{
			iSocketLastError = errno;
			if (iSocketLastError == EAGAIN || iSocketLastError == EWOULDBLOCK /*WSAEWOULDBLOCK*/)
			{
				DPRINTF("************************************************\r\n");
				DPRINTF("***** DIAG SendUDPMsg WSAEWOULDBLOCK (%d) *****\r\n",retry_cnt);
				DPRINTF("************************************************\r\n");
				DPRINTF("Error: %s\n", strerror(iSocketLastError));
				usleep(200);
				continue;
			}
			m_iLostExtTxMessages++;
			DPRINTF("**********************************************************************\r\n");
			DPRINTF("***** DIAG SendUDPMsg failed iSocketLastError = %d, LostMsg#=%d *****\r\n",iSocketLastError,m_iLostExtTxMessages);
			DPRINTF("***** DIAG NOT WSAEWOULDBLOCK...                                *****\r\n");
			DPRINTF("**********************************************************************\r\n");
			DPRINTF("Error: %s\n", strerror(iSocketLastError));
			return -1;
		}
		else if (ret != (int)len)
		{
			DPRINTF("***** DIAG SendUDPMsg error in length *****\r\n");
			return ret;
		}
		else
		{
			if (20-retry_cnt>1)
			{
				DPRINTF("*********************************************\r\n");
				DPRINTF("***** DIAG SendUDPMsg OK after %02d tries *****\r\n",20-retry_cnt);
				DPRINTF("*********************************************\r\n");
			}
			return ret;
		}
	}
	return 0;
}
int SendUDPMsg(UINT8 *msg, UINT32 len)
{
	struct sockaddr_in sin;
	struct sockaddr_un clntaddr;
	int					ret = 0;
	UINT32	retry_cnt		= 20;
	int		iSocketLastError;
	if(diag_target.hPort == -1)
	{
		DIAGPRINT("*****DIAG SendUDPMsg socket = -1!***\n");
		return -1;
	}
	if (diagIntData.m_bIsClient)
	{
		memset(&clntaddr, 0, sizeof(clntaddr));
		clntaddr.sun_family = AF_UNIX;
		strcpy(clntaddr.sun_path, SOCKET_PATH_MASTER);
	} else
	{
		memset(&sin,0,sizeof(sin));
		sin.sin_family      = AF_INET;
		sin.sin_port        = m_sUDP_Port;
		//DIAGPRINT("***** DIAG SendUDPMsg to port %x, length:%d *****\n",m_sUDP_Port, len);
	}
	while(retry_cnt--)
	{
		if(udp_state.state != 1)
		{
			DIAGPRINT("*****DIAG SendUDPMsg while binding... *****\r\n");
			return -1;
		}

		if (diagIntData.m_bIsClient)
		{
			ret = sendto(diag_target.hPort, (CHAR *)msg, len, 0, (const struct sockaddr*)&clntaddr, (socklen_t)sizeof(clntaddr));
		} else
		{
			sin.sin_addr.s_addr = m_lAddress;
			ret = sendto(diag_target.hPort, (CHAR *)msg, len, 0, (const struct sockaddr*)/*(LPSOCKADDR)*/&sin, (socklen_t)sizeof(sin));
			//DIAGPRINT("*****DIAG Send UDP :%d****\n",ret);
		}
		if (ret < 0)
		{
			iSocketLastError = errno;
			if (iSocketLastError == EAGAIN || iSocketLastError == EWOULDBLOCK /*WSAEWOULDBLOCK*/)
			{
				DPRINTF("************************************************\r\n");
				DPRINTF("***** DIAG SendUDPMsg WSAEWOULDBLOCK (%d) *****\r\n",retry_cnt);
				DPRINTF("************************************************\r\n");
				DPRINTF("Error: %s\n", strerror(iSocketLastError));
				usleep(200);
				continue;
			}
			m_iLostExtTxMessages++;
			DPRINTF("**********************************************************************\r\n");
			DPRINTF("***** DIAG SendUDPMsg failed iSocketLastError = %d, LostMsg#=%d *****\r\n",iSocketLastError,m_iLostExtTxMessages);
			DPRINTF("***** DIAG NOT WSAEWOULDBLOCK...                                *****\r\n");
			DPRINTF("**********************************************************************\r\n");
			DPRINTF("Error: %s\n", strerror(iSocketLastError));
			return -1;
		}
		else if (ret != (int)len)
		{
			DPRINTF("***** DIAG SendUDPMsg error in length *****\r\n");
			return ret;
		}
		else
		{
			if (20-retry_cnt>1)
			{
				DPRINTF("*********************************************\r\n");
				DPRINTF("***** DIAG SendUDPMsg OK after %02d tries *****\r\n",20-retry_cnt);
				DPRINTF("*********************************************\r\n");
			}
			return ret;
		}
	}
	return 0;
}

int SendTCPMsg(UINT8 *msg, UINT32 len)
{
	int		ret = 0;
	UINT32	retry_cnt = 20;
	int		iSocketLastError;
	if(diag_target.hPort == -1)
	{
		DIAGPRINT("*****DIAG SendTCPMsg socket = -1!***\n");
		return -1;
	}
	while(retry_cnt--)
	{
		if(tcp_state.state != 1)
		{
			DIAGPRINT("*****DIAG SendTCPMsg while connecting... *****\r\n");
			return -1;
		}
		if (diagIntData.m_bIsClient)
		{
			ret = send(diag_target.hPort, (CHAR *)msg, len, 0);
			//TBD
		} else
		{
			ret = send(diag_target.hPort, (CHAR *)msg, len, 0);
			//DIAGPRINT("*****DIAG Send TCP :%d****\n",ret);
		}
		if (ret < 0)
		{
			iSocketLastError = errno;
			if (iSocketLastError == EAGAIN || iSocketLastError == EWOULDBLOCK /*WSAEWOULDBLOCK*/)
			{
				DPRINTF("************************************************\r\n");
				DPRINTF("***** DIAG SendUDPMsg WSAEWOULDBLOCK (%d) *****\r\n",retry_cnt);
				DPRINTF("************************************************\r\n");
				DPRINTF("Error: %s\n", strerror(iSocketLastError));
				usleep(200);
				continue;
			}
			m_iLostExtTxMessages++;
			DPRINTF("**********************************************************************\r\n");
			DPRINTF("***** DIAG SendUDPMsg failed iSocketLastError = %d, LostMsg#=%d *****\r\n",iSocketLastError,m_iLostExtTxMessages);
			DPRINTF("***** DIAG NOT WSAEWOULDBLOCK...                                *****\r\n");
			DPRINTF("**********************************************************************\r\n");
			DPRINTF("Error: %s\n", strerror(iSocketLastError));
			return -1;
		}
		else if (ret != (int)len)
		{
			DPRINTF("***** DIAG SendTCPPMsg error in length :%d / %d*****\n",ret ,len);
			return ret;
		}
		else
		{
			if (20-retry_cnt>1)
			{
				DPRINTF("*********************************************\r\n");
				DPRINTF("***** DIAG SendUDPMsg OK after %02d tries *****\r\n",20-retry_cnt);
				DPRINTF("*********************************************\r\n");
			}
			return ret;
		}
	}
	if(ret<0)
	{
		diag_lock_L(&tcp_state.cpsr,&tcp_state.lock);
		tcp_state.state = 0;
		diagExtIFStatusDiscNotify();
		diag_unlock_L(tcp_state.cpsr, &tcp_state.lock);

	}
	return ret;
}
/***********************************************************************
* Function: diag_os_TransmitToExtIfMulti                               *
************************************************************************
* Description: Perform the specific interface logic to tx a trace      *
*              on the external interface                               *
*                                                                      *
* Parameters:  number of buffers to transmit, position in cyclic queue *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
UINT32 diag_os_TransmitToExtIfMulti(UINT16 nOfBuffers, UINT16 pos)
{
	DIAG_UNUSED_PARAM(nOfBuffers)
	DIAG_UNUSED_PARAM(pos)
	// LINUX , WINCE
	DIAG_ASSERT(0);  // NOT SUPPORTED
	return 0;
}


// ADD ICAT COMMANDS - to control if we initiate ICAT-ready, DBversion on UDB bind

//ICAT EXPORTED FUNCTION - Diag, Debug, udpInternalICATreadyon
void udpInternalICATreadyon(void)
{
	bUDPsendICATready = 1;
	DIAGM_TRACE(Diag, Debug, UDPicatReadyOn_1, "Send ICATready (internal) when binded");

}

//ICAT EXPORTED FUNCTION - Diag, Debug, udpInternalICATreadyoff
void udpInternalICATreadyoff(void)
{
	bUDPsendICATready = 0;
	DIAGM_TRACE(Diag, Debug, UDPicatReadyOff_2, "DO NOT SEND ICATready (internal on bind) ");
}
void diagPreDefCMMInit(void)
{
	*( (UINT16 *)(RegisterCMM+0)     )=44;//Length
	*( (UINT8  *)(RegisterCMM+1)     )=CLIENT_TO_MASTER_CMM | (DIAG_CORE_ID? DIAG_CORE_SELECTOR_RX_MASK : 0) ;//SAP
	*(((UINT8  *)(RegisterCMM+1))+1  )=REGISTER_CMM_SER_ID;//Service ID
	*(            RegisterCMM+4      )=CMM_VER;//CMM version
}
void diagCMMRegister(void)
{
	RegisterCMM[5]=_FirstCommandID;//UINT32 firstCommandID
	RegisterCMM[6]=_LastCommandID;//UINT32 lastCommandID
	RegisterCMM[7]=_FirstReportID;//UINT32 firstReportID
	RegisterCMM[8]=_LastReportID;//UINT32 lastReportID
	RegisterCMM[9]=0; //TBDIY ;//UINT32 procID
	RegisterCMM[10]=0;//TBDIY GetThreadPriority(HANDLE hThread);//UINT32 procPriority
	while(SendLocalMsg((UINT8 *)&RegisterCMM,sizeof(RegisterCMM))== -1)
	{
		DIAGPRINT("************diag register to master %d\n", sizeof(RegisterCMM));
		usleep(100*1000);
	}
	DIAGPRINT("************diag register to master done!\n");
}
