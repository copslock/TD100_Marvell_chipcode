/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/

/************************************************************************//*                                                                      */
/* Title:    diag       (os specific file )		                            */
/* Filename: diag_osif.c                                                */
/*                                                                      */
/* Author:   Yaeli Karni                                                */
/*                                                                      */
/* Project, Target, subsystem: Tavor, Boerne, HAL                                       */
/*																		*/
/*                                                                                                                      */
/* Created: May 2007                                                  */
/*                                                                                                                      */
/************************************************************************/


/*----------- External include files ----------------------------------------*/


/*----------- Local include files -------------------------------------------*/
#include "linux_types.h"
#include "osa.h"
//#include "ICAT_config.h"
#include "diag_osif.h"
#include "diag_init.h"
#include "pxa_dbg.h"
//#include "diag_comm.h"

#if defined (OSA_WINCE)
#include <Winsock2.h> //for inet_addr,htons
#include "args.h"               //for BSP_ARGS
#include <regext.h>             //for NOTIFICATIONCONDITION
#include "xllp_ost.h"           //for XLLP_OST_T
#include <ceddk.h>              //for PHYSICAL_ADDRESS
#include "tavorP_base_regs.h"   //for MONAHANS_BASE_REG_PA_OST

extern void diagExtIFstatusConnectNotify(void);
extern void diagExtIFStatusDiscNotify(void);
extern void diagPreDefCMMInit(void);

extern UINT32 hUsbPortFunctional;
extern UINT32 lastUSBevent;                             // 0-none, 1-USB connect, 2-USB disconnect, 3-IPconnected,4-IPdisconnected,10-various-error
extern UINT32 lastUDPevent;                             // 0-none, 1-UDP connected, 2-UDP disconnected, 10 -errorr

DWORD USBUseRndisOrDiag;
DWORD dwDiagConnectionMode = BSP_NO_RTR;                // replace with BSP_NO_DIAG

#define USB_PLUG_CHECK_TASK_PRIORITY 5                  // MUST be higher than GPC task
#define USB_PLUG_CHECK_TASK_STACK OSA_MIN_STACK_SIZE    //1000	// like in sicat (not 256)
#define CHANGE_TO_DIAG          0xfefdfa11              //TBDIY ask changing SICAT to DIAG: C:\WM603\PLATFORM\TavorEVB\Apps\USBSWITCH_1\UsbSwitch.cpp(514):		SetEventData(UsbSwitchEvent,CHANGE_TO_SICAT);
#define CHANGE_TO_RNDIS         0xaba1a3a6

#define TAVOR_INTSHARE_STATIC_IP        "192.168.0.1"
#define ACAT_INTSHARE_STATIC_IP         "192.168.0.102"
#define AS_NOTIFY_USER_DATA             0
#define IS_NOTIFY_USER_DATA             1

static UINT32 taskUsbPlugCheckStack[USB_PLUG_CHECK_TASK_STACK / 4];       // align at 4 bytes
BOOL InitSocketNotifications();
void CheckUsbPlug(LPVOID pParam);
DWORD SockEventThread(LPVOID);

OSTaskRef m_CheckUsbPlugStatusThread;

HANDLE ReInitSockEvent = NULL;
HANDLE hSockEventThread = NULL;

//ICAT EXPORTED STRUCT
typedef struct {
	UINT indx;
	DWORD dwUserData;
	DWORD pData;
	UINT cbData;
} reg_event;
#define RE_LOG_SIZE 32 //must be 1<<x
reg_event re[RE_LOG_SIZE]; //global is init to zero by default
UINT re_indx;

typedef struct __RTR_UDP_SOCK_INFO {
	ULONG ulBoardIpAddress;
	ULONG ulACATIpAddress;
}RTR_UDP_SOCK_INFO, *pRTR_UDP_SOCK_INFO;

//SD/FS Related Global Variables
UINT32 log_auto_start = 1; //automatic start of logging to the log file


#endif //defined (OSA_WINCE)

#if defined (OSA_NUCLEUS)
#include "utils.h"
#include "timer.h"
#include "timer_config.h"
#endif

#if defined (OSA_LINUX)
#include <pthread.h>
/* header file for gettimeofday */
#include <sys/time.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h> //toupper
#include <stdlib.h> //getenv
#include <arpa/inet.h> //inet_addr, htons
#include "osa_types.h"
#include "utilities.h"

extern void diagPreDefCMMInit(void);
#if defined (DIAG_TS_FREE_RUNNING)
unsigned long volatile * p_global_ts;
UINT32 ts_fd;
#endif

// Initial values to diag Internal Data variables.
diagInternalDataS diagIntData = {
	NULL,                                           //diagMsgQRef
	tNoConnection,                                  //m_eConnectionType	 - default - no External connection
	tLL_none,                                       //m_eLLtype - default none
	tNoConnection,                                  //m_eIntConnectionType - default no Internal connection
	tLL_none,                                       //m_eIntLLtype  - default none
	0,                                              //m_extIfTxMulti
	1,                                              //m_bWorkMultiCore - work mode - with multi core (via MSL, internal-bus/ram) (=1)or single-core only (=0)
	0,                                              // m_bMasterClientMode;	//  Masgter-client
	0,                                              // m_bIsMaster;			// is master
	0,                                              // m_bIsClient;			// is client
	1,		// m_bIsRawDiag;		// is RawDiag
	NULL, //diagICATReayNotifyEventDefaultFunc,	//diagICATReadyNotifyEventFunc
#if defined (DIAG_L1GKI)
	NULL, //diagStubPsCheckOKfn,		//_diagSigPsCheckFn
#else
	NULL, //diagStubPsCheckNotValidfn,	//_diagSigPsCheckFn
#endif

#if defined  _HERMON_                   // to be removed when platform register (Jun 2007) YKYK
	NULL, //EnterBootloaderMode,		//diagEnterBootLoaderCBFunc
#else
	NULL,                                           //diagEnterBootLoaderCBFunc
#endif
	{ 0, 0, 0 },                                    //DiagBSPFSConfigS diagBSPFScfg
	{ 0, 3 },                                        //DiagCachedWriteS diagCachedWrite
	0                                                //nReTriesToPerform, number of retry to perform on failure of diagRecieveCommand
};


OS_Mutex_t _diag_lock;

#define DIAG_USB_BSPFLAG                1
#define DIAG_UART_BSPFLAG               2

static const char *DIAG_CFG_FILE_PATH = "/data/data/com.marvell.usbsetting/";
static const char *DIAG_NOGUI_CFG_FILE_PATH = "/marvell/tel/";

UINT32 diagGetTS_LINUX(void)
{
	/* code to get the clock */
	struct timeval tv;
	struct timezone tz;
	UINT32 ret_value = 0;

	gettimeofday(&tv, &tz);
	ret_value = ((UINT32)((tv.tv_sec & 0x1ffff) << 15) + (tv.tv_usec >> 5));
	//There are 2^15 (32,768) ticks per second.
	//For performance we do an approximation of 1,000,000 uSec as 1,048,576 (2^20) uSec. Anyway, Linux is not that accurate.
	//(tv.tv_usec*32,768)/1,000,000 ~= (tv.tv_usec>>5)
	//ret_value is 32bits. We have to implement a wraparound when translating to ticks (&0x1ffff) by throwing the most significant bits.
	return ret_value;
}
void * DiagOpenHWAcc(UINT32 hwaddr, UINT32 maplen, UINT32 * fid )

{

	unsigned long addrAligned;
	volatile unsigned long * pa;
	void *vpa;
	int lenAligned;
	int ret;

	if ((ret = open("/dev/hwmap", O_RDWR)) >= 0)
	{
		*fid = ret;
		int pageSize = sysconf(_SC_PAGESIZE);
		addrAligned = hwaddr & PAGE_MASK_BITS(pageSize);
		lenAligned = ((hwaddr + maplen - addrAligned) + pageSize - 1) & PAGE_MASK_BITS(pageSize);


		if ((vpa = mmap(0, lenAligned, PROT_READ | PROT_WRITE, MAP_SHARED, *fid, addrAligned)) != MAP_FAILED)
		{

			pa = (volatile unsigned long*)vpa;
			pa += (hwaddr & PAGE_OFFS_BITS(pageSize)) >> 2;
			return (void*)pa;

		}

	}

	return NULL;

}


void  DiagCloseHWAcc(void * vpa, UINT32 hwaddr, UINT32 maplen, UINT32 fid)

{
	volatile unsigned long * pa;
	int pageSize = sysconf(_SC_PAGESIZE);

	pa = (volatile unsigned long*)vpa;
	//Unalign the VPA
	pa -= (hwaddr & PAGE_OFFS_BITS(pageSize)) >> 2;
	munmap((void*)pa, maplen);
	close(fid);

}

static char sIPaddr[256]; //ACAT IP address string from cfg file
static char sTavorIPaddr[256]; //Tavor IP address string from cfg file

//This folder -environment variable s shoud be set and created by the user for integarte with their environment
//It assures that there is a place that is readable/writeable in their environmenn
char *diag_marvell_rw_dir;
char *diag_socket_rw_dir;

char *SOCKET_PATH_MASTER; //The final socket path
#endif /* OSA_LINUX */

DIAG_IMPORT const UINT32 _FirstCommandID;
int diag_server = 0;
extern unsigned long m_lAddress;
extern unsigned long m_lTavorAddress;
extern unsigned short m_sUDP_Port;

#if defined (TESTUSBNODMA_YK)
#undef DIAG_TX_USB_DMA // temp for test USB without
#endif

/*----------- Global Variables ----------------------------------------------*/
#if defined (OSA_NUCLEUS)
UINT32 oldInterruptsMode;               // global variable for CPSR of diag_lock_G
#endif

#if defined (OSA_WINCE)
HANDLE hDiagLock;                               //Mutex for locking in multiple modules
#endif

/************************************************************************
* Function:     * diag_lock
**************************************************************************
* Description:  * implements lock for diag
*               *
* Parameters:   * Pointer to output value that holds the CPSR value.
*               *
* Return value: * TRUE
*               *
* Notes:        *
************************************************************************/
UINT32  diag_lock(UINT32 *cpsr_ptr)
{

#if defined (OSA_NUCLEUS)
	*cpsr_ptr = disableInterrupts();
#endif
#if defined (OSA_LINUX)
	DIAG_UNUSED_PARAM(cpsr_ptr)
	OSA_MutexLock(&_diag_lock, OSA_SUSPEND);
#endif
#if defined (OSA_WINCE)
	//OSContextLock();
	WaitForSingleObject(hDiagLock, INFINITE);
#endif
	return TRUE;
}


/*************************************************************************
 * Function:     * diag_unlock
 **************************************************************************
 * Description:  * release the lock
 *               *
 *               *
 * Parameters	: New CPSR value to set
 *               *
 * Return value: *
 *               *
 * Notes:        * None
 ************************************************************************/
void            diag_unlock(UINT32 cpsr_val)
{
#if defined (OSA_NUCLEUS)
	restoreInterrupts(cpsr_val);
#endif
#if defined (OSA_LINUX)
	DIAG_UNUSED_PARAM(cpsr_val)
	OSA_MutexUnlock(&_diag_lock);
#endif
#if defined (OSA_WINCE)
	//OSAContextUnlock();
	ReleaseMutex(hDiagLock);
#endif
	return;
}

/************************************************************************
* Function:     * diag_lock_L
**************************************************************************
* Description:  * implements lock for diag
*               *
* Parameters:   * Pointer to output value that holds the CPSR value.
*               *
* Return value: * TRUE
*               *
* Notes:        *
************************************************************************/
UINT32  diag_lock_L(UINT32 *cpsr_ptr, OS_Mutex_t * lock)
{

#if defined (OSA_NUCLEUS)
	*cpsr_ptr = disableInterrupts();
#endif
#if defined (OSA_LINUX)
	DIAG_UNUSED_PARAM(cpsr_ptr)
	OSA_MutexLock(lock, OSA_SUSPEND);
#endif
#if defined (OSA_WINCE)
	//OSContextLock();
	WaitForSingleObject(hDiagLock, INFINITE);
#endif
	return TRUE;
}

/*************************************************************************
 * Function:     * diag_unlock_L
 **************************************************************************
 * Description:  * release the lock
 *               *
 *               *
 * Parameters	: New CPSR value to set
 *               *
 * Return value: *
 *               *
 * Notes:        * None
 ************************************************************************/
void  diag_unlock_L(UINT32 cpsr_val, OS_Mutex_t * lock)
{
#if defined (OSA_NUCLEUS)
	restoreInterrupts(cpsr_val);
#endif
#if defined (OSA_LINUX)
	DIAG_UNUSED_PARAM(cpsr_val)
	OSA_MutexUnlock(lock);
#endif
#if defined (OSA_WINCE)
	//OSAContextUnlock();
	ReleaseMutex(hDiagLock);
#endif
	return;
}
/************************************************************************
* Function:     * diag_lock_G
**************************************************************************
* Description:  * implements lock for diag - with global variable
*               *
* Parameters:   * none
*               *
* Return value: * none
*               *
* Notes:        *
************************************************************************/
void    diag_lock_G(void)
{

#if defined (OSA_NUCLEUS)
	oldInterruptsMode = disableInterrupts();
#endif
#if defined (OSA_LINUX)
	OSA_MutexLock(&_diag_lock, OSA_SUSPEND);
#endif
#if defined (OSA_WINCE) //WinCE code
	//OSContextLock();
	WaitForSingleObject(hDiagLock, INFINITE);
#endif
	return;
}

/*************************************************************************
 * Function:     * diag_unlock_G
 **************************************************************************
 * Description:  * release the lock - by using global variable (in NU)
 *               *
 *               *
 * Parameters	: none
 *               *
 * Return value: *
 *               *
 * Notes:        * None
 ************************************************************************/
void    diag_unlock_G(void)
{
#if defined (OSA_NUCLEUS)
	restoreInterrupts(oldInterruptsMode);
#endif
#if defined (OSA_LINUX)
	OSA_MutexUnlock(&_diag_lock);
#endif
#if defined (OSA_WINCE) //WinCE code
	//OSAContextUnlock();
	ReleaseMutex(hDiagLock);
#endif
	return;
}

/*************************************************************************
 * Function:     * osTimeClockRateGet
 **************************************************************************
 * Description:  * returns the core clock rate
 *               *
 *               *
 * Parameters	: None
 *               *
 * Return value: * clock rate - on Platform that is supported, otherwise zero
 *               *
 * Notes:        * None
 ************************************************************************/
UINT32  osTimeClockRateGet(void)
{
#if defined (OSA_NUCLEUS)
	return timerClockRateGet(OS_TIMER_ID);
#else
	return 0;
#endif
}
/*************************************************************************
 * Function:     * diagBSPconfig
 **************************************************************************
 * Description:  * Configures Diag according to the BSP parameters
 *               *
 * Parameters	: s_diagBSPconfig *cfg - BSP parameters
 *               *
 * Return value: * 1 - success
 *               * 0 - failure due to wrong configuration
 *               *
 * Notes:        * New BSP parameters can be add at the end of the
 *               * s_diagBSPconfig structure. Each parameter should be 4
 *               * bytes size. When set to 0, the default for this
 *               * parameter is used.
 ************************************************************************/
s_diagBSPconfig BSPcfg = { 0 };
UINT32 diagBSPconfig(s_diagBSPconfig *cfg)
{
	BSPcfg.extIfTxMulti = cfg->extIfTxMulti;
	return 1;
}

#if defined (OSA_LINUX)
/***********************************************************************
* Function: diagLinuxBSPcfgFile                                        *
************************************************************************
* Description:  Used to read and parse the Diag BSP configuration file.*
*                                                                      *
* Parameters:   const char *cfg_file - FS configuration file name.     *
		BOOL create_file  create new file
*                                                                      *
* Return value: Fail or Succes                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
BOOL diagLinuxBSPcfgFile(const char *cfg_file, BOOL create_file )
{
	FILE *stream;
	char s[256 + 10];
	char s1[266];
	char s2[266];
	BOOL rc = TRUE;
	BOOL b_write_unit_size_set = FALSE;
	//default values
	//Internal interface: intr=0-sal (default), 1-gpc, 2-shm
	UINT32 intr = 0; //sal
	//External interface: extr=0-usb (default), 1-tcp, 2-udp, 3-uart, 4-ssp, 5-fs_usb, 6-storage_card, 7-virtio
	UINT32 extr = 0; //usb

	diagIntData.m_eConnectionType = tUSBConnection;
	diagIntData.m_eLLtype = tLL_USB;
	diagIntData.m_eIntConnectionType = tMSLConnection;
	diagIntData.m_eIntLLtype = tLL_SAL;
	//Distributed (split) Diag: split=1 (default), 0
	diagIntData.m_bMasterClientMode = 1;
	//Multi core (or apps only): multi_core=1 (default), 0
	diagIntData.m_bWorkMultiCore = 1;
	//USB transmit multi (DMA): usb_tx_multi=0 (default), 1
	diagIntData.m_extIfTxMulti = 0;
	//TCP Server/Client : client = 0 (default), 1
	diagIntData.m_bIsClient = 0;
	//Raw Diag:raw_diag = 1 (default), 0
	diagIntData.m_bIsRawDiag = 1;
	//SD/FS Related Parameters
	diagIntData.diagBSPFScfg.log_auto_start = 1;

	diagIntData.diagBSPFScfg.FS_Inc_Reset = 1;

	diagIntData.diagBSPFScfg.SD_FSType = FSTYPE_VFAT;

	//Secret Cached Write mechanism
	diagIntData.diagCachedWrite.write_unit_size = DIAG_COMM_EXTIF_ETH_MAX_BUFF_SIZE; //undoc1=0-disabled (default)
	//TBDIY be careful!!! In UDP max Tx unit is 4096, so please use max 2048 (2*2048=4096)
	diagIntData.diagCachedWrite.time_out = 3;                         //undoc2=3 (default)

	strcpy(sIPaddr,         "192.168.1.100");
	strcpy(sTavorIPaddr, "192.168.1.101");

	stream = fopen(cfg_file, "r");
	//Only create new file if it does not exist and create new file is allowed.
	if ((stream == NULL) && (TRUE == create_file))
	{
		fprintf(stderr, "***** DIAG BSP configuration file: %s was not opened. Error: %s. Trying to create a default one. *****\r\n", cfg_file, strerror(errno));
		stream = fopen(cfg_file, "w");
		if (stream == NULL)
		{
			fprintf(stderr, "***** DIAG BSP configuration file: The default %s was not opened. Error: %s. Using default values. *****\r\n", cfg_file, strerror(errno));
			rc = FALSE;
		}
		else
		{
			fprintf(stream, "#External interface: extr=0-usb (default), 1-tcp, 2-udp, 3-uart, 4-ssp, 5-fs_usb, 6-storage_card, 7-virtio\r\n");
			fprintf(stream, "extr=%d\r\n", extr);

			fprintf(stream, "#Internal interface: intr=0-sal (default), 1-gpc, 2-shm\r\n");
			fprintf(stream, "intr=%d\r\n", intr);

			fprintf(stream, "#Distributed (split) Diag: split=1 (default), 0\r\n");
			fprintf(stream, "split=%d\r\n", diagIntData.m_bMasterClientMode);

			fprintf(stream, "#Multi core (or apps only): multi_core=1 (default), 0\r\n");
			fprintf(stream, "multi_core=%d\r\n", diagIntData.m_bWorkMultiCore);

			fprintf(stream, "#USB transmit multi (DMA): usb_tx_multi=0 (default), 1\r\n");
			fprintf(stream, "usb_tx_multi=%d\r\n", diagIntData.m_extIfTxMulti);

			fprintf(stream, "#TCP Server/Client : client = 0 (default), 1\r\n");
			fprintf(stream, "client=%d\r\n", diagIntData.m_bIsClient);

			fprintf(stream, "#Raw Diag:raw_diag = 1 (default), 0\r\n");
			fprintf(stream, "raw_diag=%d\r\n",diagIntData.m_bIsRawDiag);

			fprintf(stream, "#==============FS specific variables - Start ============\r\n");

			fprintf(stream, "#Log auto start: auto=1 (default), 0\r\n");
			fprintf(stream, "auto=%d\r\n", diagIntData.diagBSPFScfg.log_auto_start);

			fprintf(stream, "#Enable Log Incremental Reset: increset=1 (default), 0\r\n");
			fprintf(stream, "increset=%d\r\n", diagIntData.diagBSPFScfg.FS_Inc_Reset);

			fprintf(stream, "#Mounting FS Type: fstype=0-no_mount, 1-vfat (default), 2-ext2, 3-ext3, 4-msdos\r\n");
			fprintf(stream, "fstype=%d\r\n", diagIntData.diagBSPFScfg.SD_FSType);




			fprintf(stream, "#==============FS specific variables - End ============\r\n");


			fprintf(stream, "#ACAT IP address: acat_ip=%s (default) valid for TCP/UDP mode (extr=1 or 2)\r\n", sIPaddr);
			fprintf(stream, "acat_ip=%s\r\n", sIPaddr);

			fprintf(stream, "#Tavor IP address: tavor_ip=%s (default) valid for TCP/UDP mode (extr=1 or 2)\r\n", sTavorIPaddr);
			fprintf(stream, "tavor_ip=%s\r\n", sTavorIPaddr);

			if (fclose(stream) != 0)
			{
				fprintf(stderr, "***** DIAG BSP configuration file: fclose() failure. Error: %s. *****\r\n", strerror(errno));
			}
			fprintf(stderr, "***** DIAG BSP configuration file: The default %s is ready. *****\r\n", cfg_file);
		}
	}
	else   //File does not exist or can not be created
	{
		//File does not exists
		if (stream == NULL)
		{
			fprintf(stderr, "***** DIAG BSP configuration file: The default %s was not opened.\r\n", cfg_file);
			//I will return here in this case to avoid exessive print
			return FALSE;
		}
		else //File exists and is ready to be read.
		{
			int i;
			fprintf(stderr, "***** DIAG BSP configuration file: Analyzing the %s file ... *****\r\n", cfg_file);
			i = fscanf(stream, "%[][- =a-zA-Z0-9_.~\\/#%%?+|{}()<>,:;*&^$@!`']\r\n", s);
			fprintf(stderr, "***** DIAG BSP configuration file: %s *****\r\n", s);
			while (i != EOF)
			{
				char *tmp;
				int t;

				if ((*s == '#') || (*s == '/') || (*s == '\\'))
				{
					i = fscanf(stream, "%[][- =a-zA-Z0-9_.~\\/#%%?+|{}()<>,:;*&^$@!`']\r\n", s);
					continue; // # and /  and \ are a remark line
				}

				tmp = s;
				while (*tmp != 0)
				{
					*tmp = toupper(*tmp);
					tmp++;
				}
				t = sscanf(s, "%[^=]%*[=]%s", s1, s2);
				if (t == 2)
				{
					if (strcmp("EXTR", s1) == 0)
					{
						sscanf(s2, "%d", &extr);
						switch (extr)
						{ //External interface: extr=0-usb (default), 1-tcp, 2-udp, 3-uart, 4-ssp, 5-fs_usb, 6-storage_card, 7-virtio
						case 0:
							diagIntData.m_eConnectionType = tUSBConnection;
							diagIntData.m_eLLtype = tLL_USB;
							fprintf(stderr, "***** DIAG BSP configuration file: extr=usb *****\r\n");
							break;
						case 1:
							diagIntData.m_eConnectionType = tTCPConnection;
							diagIntData.m_eLLtype = tLL_ETHERNET;
							fprintf(stderr, "***** DIAG BSP configuration file: extr=tcp *****\r\n");
							break;
						case 2:
							diagIntData.m_eConnectionType = tUDPConnection;
							diagIntData.m_eLLtype = tLL_ETHERNET;
							fprintf(stderr, "***** DIAG BSP configuration file: extr=udp *****\r\n");
							break;
						case 3:
							diagIntData.m_eConnectionType = tUARTConnection;
							diagIntData.m_eLLtype = tLL_UART;
							fprintf(stderr, "***** DIAG BSP configuration file: extr=uart *****\r\n");
							break;
						case 4:
							diagIntData.m_eConnectionType = tSSPConnection;
							diagIntData.m_eLLtype = tLL_SSP;
							fprintf(stderr, "***** DIAG BSP configuration file: extr=ssp *****\r\n");
							break;
						case 5:
							diagIntData.m_eConnectionType = tUSBConnection;
							diagIntData.m_eLLtype = tLL_FS;
							fprintf(stderr, "***** DIAG BSP configuration file: extr=fs_usb *****\r\n");
							break;
						case 6:
							diagIntData.m_eConnectionType = tUSBConnection;
							diagIntData.m_eLLtype = tLL_SC;
							fprintf(stderr, "***** DIAG BSP configuration file: extr=storage_card *****\r\n");
							break;
						case 7:
							diagIntData.m_eConnectionType = tVIRTIOConnection;
							diagIntData.m_eLLtype = tLL_ETHERNET;
							fprintf(stderr, "***** DIAG BSP configuration file: extr=virtio *****\r\n");
							break;
						default:
							diagIntData.m_eConnectionType = tUSBConnection;
							diagIntData.m_eLLtype = tLL_USB;
							fprintf(stderr, "***** DIAG BSP configuration file: '%s' is a wrong parameter! using extr=usb (default) *****\r\n", s2);
							break;
						}
					}
					else if (strcmp("INTR", s1) == 0)
					{
						sscanf(s2, "%d", &intr);
						switch (intr)
						{ //Internal interface: intr=0-sal (default), 1-gpc, 2-shm
						case 0:
							diagIntData.m_eIntConnectionType = tMSLConnection;
							diagIntData.m_eIntLLtype = tLL_SAL;
							fprintf(stderr, "***** DIAG BSP configuration file: intr=sal *****\r\n");
							break;
						case 1:
							diagIntData.m_eIntConnectionType = tMSLConnection; //????????
							diagIntData.m_eIntLLtype = tLL_GPC; //????????
							fprintf(stderr, "***** DIAG BSP configuration file: intr=gpc *****\r\n");
							break;
						case 2:
							diagIntData.m_eIntConnectionType = tSHMConnection;
							diagIntData.m_eIntLLtype = tLL_ACIPC;
							fprintf(stderr, "***** DIAG BSP configuration file: intr=shm *****\r\n");
							break;
						default:
							diagIntData.m_eIntConnectionType = tMSLConnection;
							diagIntData.m_eIntLLtype = tLL_SAL;
							fprintf(stderr, "***** DIAG BSP configuration file: '%s' is a wrong parameter! using intr=sal (default) *****\r\n", s2);
							break;
						}
					}
					else if (strcmp("SPLIT", s1) == 0)
					{
						sscanf(s2, "%d", &(diagIntData.m_bMasterClientMode));
						fprintf(stderr, "***** DIAG BSP configuration file: split=%d *****\r\n", diagIntData.m_bMasterClientMode);
					}
					else if (strcmp("MULTI_CORE", s1) == 0)
					{
						sscanf(s2, "%d", &(diagIntData.m_bWorkMultiCore));
						fprintf(stderr, "***** DIAG BSP configuration file: multi_core=%d *****\r\n", diagIntData.m_bWorkMultiCore);
					}
					else if (strcmp("USB_TX_MULTI", s1) == 0)
					{
						sscanf(s2, "%d", &(diagIntData.m_extIfTxMulti));
						fprintf(stderr, "***** DIAG BSP configuration file: usb_tx_multi=%d *****\r\n", diagIntData.m_extIfTxMulti);
					}
					else if(strcmp("CLIENT", s1) == 0)
					{
						sscanf(s2,"%d", &(diagIntData.m_bIsClient));
					}
					else if(strcmp("RAW_DIAG", s1) == 0)
					{
						sscanf(s2,"%d", &(diagIntData.m_bIsRawDiag));
					}
					else if (strcmp("AUTO", s1) == 0)
					{
						sscanf(s2, "%d", &diagIntData.diagBSPFScfg.log_auto_start);
						fprintf(stderr, "***** DIAG BSP configuration file: AUTO='%d' *****\r\n", diagIntData.diagBSPFScfg.log_auto_start);
					}
					else if (strcmp("INCRESET", s1) == 0)
					{
						sscanf(s2, "%d", &diagIntData.diagBSPFScfg.FS_Inc_Reset);
						fprintf(stderr, "***** DIAG BSP configuration file: IncReset='%d' *****\r\n", diagIntData.diagBSPFScfg.FS_Inc_Reset);
					}
					else if (strcmp("FSTYPE", s1) == 0)
					{
						sscanf(s2, "%d", (int *)&diagIntData.diagBSPFScfg.SD_FSType);
						fprintf(stderr, "***** DIAG BSP configuration file: fstype='%d' *****\r\n", diagIntData.diagBSPFScfg.SD_FSType);
					}
					else if (strcmp("ACAT_IP", s1) == 0)
					{
						strcpy(sIPaddr, s2);
						fprintf(stderr, "***** DIAG over FS: ACAT_IP='%s' *****\r\n", sIPaddr);
					}
					else if (strcmp("TAVOR_IP", s1) == 0)
					{
						strcpy(sTavorIPaddr, s2);
						fprintf(stderr, "***** DIAG over FS: TAVOR_IP='%s' *****\r\n", sTavorIPaddr);
					}
					else if (strcmp("UNDOC1", s1) == 0)
					{
						b_write_unit_size_set = TRUE;
						sscanf(s2, "%d", (int *)&diagIntData.diagCachedWrite.write_unit_size);
						fprintf(stderr, "***** DIAG BSP configuration file: undoc1='%d' *****\r\n", diagIntData.diagCachedWrite.write_unit_size);
					}
					else if (strcmp("UNDOC2", s1) == 0)
					{
						sscanf(s2, "%d", (int *)&diagIntData.diagCachedWrite.time_out);
						fprintf(stderr, "***** DIAG BSP configuration file: undoc2='%d' *****\r\n", diagIntData.diagCachedWrite.time_out);
					}
					else
					{
						fprintf(stderr, "***** DIAG BSP configuration file: parser error: '%s' *****\r\n", s);
					}
				}
				else
				{
					fprintf(stderr, "***** DIAG BSP configuration file: parser error: '%s' *****\r\n", s);
				}
				i = fscanf(stream, "%[][- =a-zA-Z0-9_.~\\/#%%?+|{}()<>,:;*&^$@!`']\r\n", s);
			}
			if (fclose(stream) != 0)
			{
				fprintf(stderr, "***** DIAG BSP configuration file %s: fclose() failure. Error: %s. *****\r\n", cfg_file, strerror(errno));
				rc = FALSE;
			}
		}
	}
	/* Output data read: */
	fprintf(stderr, "***** DIAG BSP configuration file: External interface: extr=0-usb (default), 1-tcp, 2-udp, 3-uart, 4-ssp, 5-fs_usb, 6-storage_card, 7-virtio *****\r\n");
	fprintf(stderr, "***** DIAG BSP configuration file: extr='%d' *****\r\n", extr);
	fprintf(stderr, "***** DIAG BSP configuration file: Internal interface: intr=0-sal (default), 1-gpc, 2-shm *****\r\n");
	fprintf(stderr, "***** DIAG BSP configuration file: intr='%d' *****\r\n", intr);
	fprintf(stderr, "***** DIAG BSP configuration file: Distributed (split) Diag: split=1 (default), 0 *****\r\n");
	fprintf(stderr, "***** DIAG BSP configuration file: split='%d' *****\r\n", diagIntData.m_bMasterClientMode);
	fprintf(stderr, "***** DIAG BSP configuration file: Multi core (or apps only): multi_core=1 (default), 0 *****\r\n");
	fprintf(stderr, "***** DIAG BSP configuration file: multi_core='%d' *****\r\n", diagIntData.m_bWorkMultiCore);
	fprintf(stderr, "***** DIAG BSP configuration file: USB transmit multi (DMA) {may be later overwritten by diagBSPconfig}: usb_tx_multi=0 (default), 1 *****\r\n");
	fprintf(stderr, "***** DIAG BSP configuration file: usb_tx_multi='%d' *****\r\n", diagIntData.m_extIfTxMulti);
	fprintf(stderr, "***** DIAG BSP configuration file: log_auto_start  ='%d' *****\r\n", diagIntData.diagBSPFScfg.log_auto_start);
	fprintf(stderr, "***** DIAG BSP configuration file: IncReset='%d' *****\r\n", diagIntData.diagBSPFScfg.FS_Inc_Reset);
	fprintf(stderr, "***** DIAG BSP configuration file: fstype='%d' (default)\r\n", diagIntData.diagBSPFScfg.SD_FSType);
	fprintf(stderr, "***** DIAG BSP configuration file: ACAT_IP='%s' *****\r\n", sIPaddr);
	fprintf(stderr, "***** DIAG BSP configuration file: TAVOR_IP='%s' *****\r\n", sTavorIPaddr);

	m_lAddress = inet_addr(sIPaddr);
	m_lTavorAddress = inet_addr(sTavorIPaddr);
	return rc;
}


#endif //defined (OSA_LINUX)

/*************************************************************************
 * Function:     * diagOSspecificPhase1Init
 **************************************************************************
 * Description:  * OS specific init - from DEFINES to global variables etc.
 *               *
 * Parameters	: None
 *               *
 * Return value: * None
 *               *
 * Notes:        * None
 ************************************************************************/
void diagOSspecificPhase1Init(void)
{
	DPRINTF(">>>enter diagOSspecificPhase1Init\n");
#if defined (OSA_WINCE)
	/////////////////////////////
	// Client Master Interface //
	/////////////////////////////
	if (diagIntData.m_bMasterClientMode) //m_bMasterClientMode already set from GetNSetConnectionType
	{
		NKDbgPrintfW(TEXT("***** DIAG Running in Distributed Client Master Mode *****\r\n"));
		diagPreDefCMMInit();
		if (_FirstCommandID == 0)
		{
			NKDbgPrintfW(TEXT("***** DIAG Running as CMI Master *****\r\n"));
			diagIntData.m_bIsMaster = 1;
			diagIntData.m_bIsClient = 0;
		}
		else
		{
			NKDbgPrintfW(TEXT("***** DIAG Running as CMI Client *****\r\n"));
			diagIntData.m_bIsMaster = 0;
			diagIntData.m_bIsClient = 1;
		}

	}
	else
	{
		NKDbgPrintfW(TEXT("***** DIAG Running in Non-Distributed Mode *****\r\n"));
		diagIntData.m_bIsMaster = 0;
		diagIntData.m_bIsClient = 0;
	}

	/*TBDIY patch for ACAT running on target
	   diagIntData.m_eConnectionType   =tUDPConnection;
	   dwDiagConnectionMode=BSP_USE_DIAG_ACAT_COLLOCATED;*/

	if (diagIntData.m_bIsClient)
	{
		//Overwrite the Ext If settings when in client distributed mode
		diagIntData.m_eConnectionType = tUDPConnection; //do we want to do this from here?
		diagIntData.m_bWorkMultiCore = 0;
		m_lAddress     = inet_addr("127.0.0.1");
		m_lTavorAddress = inet_addr("127.0.0.1");
		m_sUDP_Port = htons(CMI_MASTER_UDP_PORT);
		NKDbgPrintfW(TEXT("***** DIAG RtrInitBody - Distributed Mode CMI Client *****\r\n"));
	}
	else if (diagIntData.m_eConnectionType == tUDPConnection)
	{
		m_sUDP_Port = htons(ACAT_UDP_PORT);
		DIAGPRINT("******DIAG UDP PORT:%x. %x", ACAT_UDP_PORT, m_sUDP_Port);
		if (dwDiagConnectionMode == BSP_USB_USE_DIAG_UDP_ON_VMINI)
		{
			NKDbgPrintfW(TEXT("***** DIAG connectUDP() - dwDiagConnectionMode==BSP_USB_USE_DIAG_UDP_ON_VMINI *****\r\n"));
			m_lAddress     = inet_addr("192.168.80.3");
			m_lTavorAddress = inet_addr("192.168.80.254");
		}
		if (dwDiagConnectionMode == BSP_USE_DIAG_ACAT_COLLOCATED)
		{
			NKDbgPrintfW(TEXT("***** DIAG connectUDP() - dwDiagConnectionMode==BSP_USE_DIAG_ACAT_COLLOCATED *****\r\n"));
			m_lAddress     = inet_addr("127.0.0.1");
			m_lTavorAddress = inet_addr("127.0.0.1");
		}
	}

	///////////
	// Mutex //
	///////////
	hDiagLock = CreateMutex(NULL, FALSE, L"DiagGLock"); //Mutex for locking in multiple modules
	DIAG_ASSERT(hDiagLock != NULL);

#endif

#if defined (OSA_NUCLEUS)

	// EXTERNAL INTERFACE SETTING

	// Default connection is USB
	diagIntData.m_eConnectionType = tUSBConnection;
	diagIntData.m_eLLtype = tLL_USB;
	diagIntData.m_extIfTxMulti = 0;

	#if defined (DIAG_TX_USB_DMA)
	diagIntData.m_extIfTxMulti = 1;                         // DMA USB allows mutil buff tx in one call
	#endif
	if (BSPcfg.extIfTxMulti == USB_EXT_IF_TX_MULTI) diagIntData.m_extIfTxMulti = 1;
	if (BSPcfg.extIfTxMulti == USB_EXT_IF_TX_SINGLE) diagIntData.m_extIfTxMulti = 0;

	#if defined (_DIAG_USE_VIRTIO_)
	diagIntData.m_eConnectionType = tVIRTIOConnection;
	diagIntData.m_eLLtype = tLL_ETHERNET;
	diagCommVirtioPhase1Init();
	#endif
	#if defined (_DIAG_USE_SSP_)
	diagIntData.m_eConnectionType = tSSPConnection;
	diagIntData.m_eLLtype = tLL_SSP;
	#endif
	#if defined (_DIAG_USE_COMMSTACK_)       // OLD and New
	diagIntData.m_eConnectionType = tUARTConnection;
	diagIntData.m_eLLtype = tLL_UART;
	#endif

	// internal interface (mutil core setting) -
	diagIntData.m_bWorkMultiCore = 0;
	#if defined (DIAG_INT_IF)
	diagIntData.m_bWorkMultiCore = 1;
	diagIntData.m_eIntConnectionType = tMSLConnection;
	diagIntData.m_eIntLLtype = tLL_SAL;
		#if defined (DIAG_OVER_SHMEM)
	diagIntData.m_eIntConnectionType = tSHMConnection;
	diagIntData.m_eIntLLtype = tLL_ACIPC;
		#endif
	#endif
#endif          // (OSA_NUCLEUS)

#if defined (OSA_LINUX)
	int ret;
	OSA_STATUS status;
	status = OSA_MutexCreate(&_diag_lock, NULL);
	DIAG_ASSERT(status == OS_SUCCESS);
	//This folder -environment variable  shoud be set and created by the user forintegarte with their environment
	//It assures that there is a place that is readable/writeable in their environmenn
	//diag_marvell_rw_dir = getenv("MARVELL_RW_DIR");
	//diag_socket_rw_dir = getenv("MARVELL_SOCKET_DIR");
	diag_socket_rw_dir = NULL;

	BOOL cfg_file_exist;
	char bspCfgFname[256];
	char diagBspCfgPath[256];

	SOCKET_PATH_MASTER = (char *)malloc(sizeof(char) * 256);

	DPRINTF("***** DIAG Phase1Init MARVELL_RW_DIR = %s *****\r\n", diag_marvell_rw_dir);
	//Chode the socket path depending on the environment variable
	if (diag_socket_rw_dir)
	{

		sprintf(SOCKET_PATH_MASTER, "/tmp/%s", LOCAL_SOCKET_MASTER_NAME);
	}
	else
	{
		strcpy(SOCKET_PATH_MASTER, LOCAL_SOCKET_PATH_MASTER);
	}

	DPRINTF("***** DIAG Phase1Init SOCKET_PATH_MASTER = %s *****\r\n", SOCKET_PATH_MASTER);

	/* check android.bsp in kernel cmdline,
	   if value == 2, Diag over UART,
	   if value == 1, Diag over USB,
	   if others, default.
	*/
	ret = get_bspflag_from_kernel_cmdline();
	if (DIAG_UART_BSPFLAG ==  ret)
	{
		strcpy(diagBspCfgPath, DIAG_NOGUI_CFG_FILE_PATH);
		sprintf(bspCfgFname, "%s/diag_uart.cfg", diagBspCfgPath);
	}
	else if (DIAG_USB_BSPFLAG == ret)
	{
                strcpy(diagBspCfgPath, DIAG_NOGUI_CFG_FILE_PATH);
                sprintf(bspCfgFname, "%s/diag_usb.cfg", diagBspCfgPath);
	}
	else
	{
		strcpy(diagBspCfgPath, DIAG_CFG_FILE_PATH);
		sprintf(bspCfgFname, "%s/diag_bsp.cfg", diagBspCfgPath);
	}

	DPRINTF("***** DIAG Phase1Init BSP_CFG_PATH = %s *****\r\n", bspCfgFname);

	//Does not create new file if the original does not exists.
	cfg_file_exist = diagLinuxBSPcfgFile(bspCfgFname, FALSE); //This file is accessed from the master and the clients: (needs readers writers lock?)
	DPRINTF("***** DIAG Phase1Init BSP_CFG_PATH = %s , exist:%d!*****\r\n", bspCfgFname,cfg_file_exist);

	if (FALSE == cfg_file_exist)
	{
		//fprintf(stderr, "***** DIAG Phase1Init - NO CFG File in /tmp folder *****\r\n");

#if defined(DIAG_ALLOW_CFGFILE_REPLACEMENT)
		if (diag_marvell_rw_dir)
		{
			strcpy(diagBspCfgPath, diag_marvell_rw_dir);
		}
		else
#endif
		{
			//strcpy(diagBspCfgPath,"/etc");
			strcpy(diagBspCfgPath, DIAG_CFG_FILE_PATH);
		}
		//DIAGPRINT("******DIAG UDP PORT:%x. %x", ACAT_UDP_PORT, m_sUDP_Port);
		sprintf(bspCfgFname, "%s/diag_bsp.cfg", diagBspCfgPath);

		fprintf(stderr, "***** DIAG Phase1Init BSP_CFG_PATH = %s *****\r\n", bspCfgFname);

		//Create new file if the original does not exists.
		diagLinuxBSPcfgFile(bspCfgFname, TRUE); //This file is accessed from the master and the clients: (needs readers writers lock?)
	}
	else // We are working in production mode
	{
		fprintf(stderr, "***** DIAG Phase1Init - File in /tmp folder - Production Mode *****\r\n");
	}


#if 0
	/////////////////////////////
	// Client Master Interface //
	/////////////////////////////
	if (diagIntData.m_bMasterClientMode) //m_bMasterClientMode already set from GetNSetConnectionType
	{
		fprintf(stderr, "***** DIAG Running in Distributed Client Master Mode *****\r\n");
		diagPreDefCMMInit();
		if (_FirstCommandID == 0)
		{
			fprintf(stderr, "***** DIAG Running as CMI Master *****\r\n");
			diagIntData.m_bIsMaster = 1;
			diagIntData.m_bIsClient = 0;
		}
		else
		{
			fprintf(stderr, "***** DIAG Running as CMI Client *****\r\n");
			diagIntData.m_bIsMaster = 0;
			diagIntData.m_bIsClient = 1;
		}
	}
	else
	{
		fprintf(stderr, "***** DIAG Running in Non-Distributed Mode *****\r\n");
		diagIntData.m_bIsMaster = 0;
		diagIntData.m_bIsClient = 0;
	}

	if (diagIntData.m_bIsClient)
	{
		//Overwrite the Ext If settings when in client distributed mode
		diagIntData.m_eConnectionType = tUDPConnection; //do we want to do this from here?
		diagIntData.m_eLLtype = tLL_ETHERNET;
		diagIntData.m_bWorkMultiCore = 0;
		m_lAddress     = inet_addr(/*"192.168.1.101"*/ "127.0.0.1");
		m_lTavorAddress = inet_addr(/*"192.168.1.101"*/ "127.0.0.1");
		m_sUDP_Port = htons(CMI_MASTER_UDP_PORT);
		fprintf(stderr, "***** DIAG RtrInitBody - Distributed Mode CMI Client *****\r\n");
	}
	else if (diagIntData.m_eConnectionType == tUDPConnection)
	{
		m_sUDP_Port = htons(ACAT_UDP_PORT);
		//m_lAddress     =inet_addr("192.168.80.3");
		//m_lTavorAddress=inet_addr("192.168.80.254");
		m_lAddress     = inet_addr(sIPaddr /*"192.168.1.100"*/);
		m_lTavorAddress = inet_addr(sTavorIPaddr /*"192.168.1.101"*/);
	}
#endif
DPRINTF("***** DIAG Phase1Init FirstCMD is %d \n",_FirstCommandID);
if (_FirstCommandID == 0)
{
	diag_server = 1;
}
else
{
	diag_server = 0;
	diagIntData.m_eLLtype = tLL_LocalIP;
	diagIntData.m_eConnectionType = tLocalConnection;
}
#if defined (DIAG_TS_FREE_RUNNING)
	p_global_ts = DiagOpenHWAcc(DIAG_TS_FREE_RUNNING, 4, &ts_fd );
	if (p_global_ts == NULL)
	{
		DPRINTF("***** DIAG TS - Can Not open HWAcc to Time Stamp - NO TS Available *****\r\n");
	}
	else
	{
		DPRINTF("***** DIAG TS - p_ts=0x%xl, init_value=0x%xl *****\r\n", p_global_ts, *p_global_ts);
	}
#endif
#endif  // OSA_LINUX

	return;
}

/*************************************************************************
 * Function:     * diagOSspecificPhase2Init
 **************************************************************************
 * Description:  * OS specific init - from DEFINES to global variables etc.
 *               *
 * Parameters	: None
 *               *
 * Return value: * None
 *               *
 * Notes:        * None
 ************************************************************************/
void diagOSspecificPhase2Init(void)
{
#if defined (OSA_WINCE)
	static const GUID guidDIAG =
	{ 0x6BDD1FC1, 0x810F, 0x11d0, { 0xBE, 0xC7, 0x08, 0x00, 0x2B, 0xE2, 0x12, 0x34 } };

	AdvertiseInterface(&guidDIAG, L"RTR", TRUE);

	// only after interfaces were initialized and event groups/queues set - we should init the notification

	if (diagIntData.m_bIsClient == 0 && diagIntData.m_eConnectionType == tUDPConnection) //TBDIY not relevant for a client in distributed mode
	{
		if (dwDiagConnectionMode == BSP_USB_USE_DIAG_UDP_ON_RNDIS)
		{
			//Ido G's code
			NKDbgPrintfW(TEXT("***** DIAG calling InitSocketNotifications() *****\r\n"));
			InitSocketNotifications();
		}
	}

	if ((diagIntData.m_eConnectionType == tUSBConnection) && (diagIntData.m_eLLtype == tLL_USB))
	{
		OS_STATUS status;

		//We want to get UsbPlugInEvent, UsbPlugOutEvent, UsbSwitchEvent and BulEndpointEvent events
		NKDbgPrintfW(TEXT("***** DIAG Init Check USB Plug Thread. *****\r\n"));

		status = OSTaskCreate(&m_CheckUsbPlugStatusThread, (VOID *)taskUsbPlugCheckStack, USB_PLUG_CHECK_TASK_STACK, USB_PLUG_CHECK_TASK_PRIORITY,
				      "CheckUsbPlug", CheckUsbPlug, NULL);
		if (status != OS_SUCCESS) return;
		OSATaskResume(m_CheckUsbPlugStatusThread);
	}
#endif
}

#if defined (OSA_WINCE)

DWORD SockEventThread(LPVOID lp_param)
{
	DWORD rc;
	pRTR_UDP_SOCK_INFO pSI;

	CeSetThreadPriority(GetCurrentThread(), 240);

	while (1)
	{
		rc = WaitForSingleObject(ReInitSockEvent, INFINITE);
		if (rc == WAIT_OBJECT_0)
		{
			pSI = (pRTR_UDP_SOCK_INFO)GetEventData(ReInitSockEvent);
			m_lAddress = pSI->ulACATIpAddress;
			m_lTavorAddress = pSI->ulBoardIpAddress;
			free(pSI);
			if (m_lAddress)
			{
				NKDbgPrintfW(TEXT("***** DIAG ReAssignHostIP TavorIP=%08X ACATIP=%08X *****\r\n"), m_lTavorAddress, m_lAddress);
				if (diagIntData.m_eConnectionType == tUDPConnection)
				{
					lastUDPevent = 1;        // connected
					diagExtIFStatusDiscNotify();
					diagExtIFstatusConnectNotify();
					//No need to disc/connect CMI
				}
			}
			else
			{
				NKDbgPrintfW(TEXT("***** DIAG CloseSocket - No connection *****\r\n"));
				if (diagIntData.m_eConnectionType == tUDPConnection)
				{
					if (lastUDPevent != 2)          //Only if we are not disconnected, because we can get several disconnect events.
					{
						lastUDPevent = 2;       // dis-connected
						diagExtIFStatusDiscNotify();
						//disconnect client in distributed mode using CMM
					}
					else
					{
						NKDbgPrintfW(TEXT("***** DIAG CloseSocket - Already Disconnected!!! *****\r\n"));
					}
				}
			}
		}
		else
		{
//			extern "C" UINT32 lastUDPevent;		// 0-none, 1-UDP connected, 2-UDP disconnected, 10 -errorr
			lastUDPevent = 10;       // error
			NKDbgPrintfW(TEXT("***** DIAG Error unexpected Timeout on event *****\r\n"));
		}
	}

}

//ICAT EXPORTED FUNCTION - Diag, Debug, RegEventLog
void diagRegEventLog(void)
{
	char re_str[100];
	UINT i;

	for (i = 0; i < RE_LOG_SIZE; i++)
	{
		if (re[i].indx)
		{
			sprintf(re_str, "***** DIAG Indx=0x%x IS=%d data=0x%x len=0x%x *****\r\n", re[i].indx, re[i].dwUserData, re[i].pData, re[i].cbData);
			DIAGM_TRACE_1S(HWPLAT, DIAG, RegEventLog, "%s", re_str);
			NKDbgPrintfW(TEXT("%s"), re_str);
		}
	}
}

void ASyncStateNotifyCallback(HREGNOTIFY hNotify, DWORD dwUserData, const PBYTE pData, const UINT cbData)
{
	static BOOL AsyncIsConnected = FALSE;
	static BOOL ISIsConnected = FALSE;
	static UINT indx = 0;
	DWORD dwState = *(DWORD*)pData;
	pRTR_UDP_SOCK_INFO pSI;
	TCHAR wIPAddrString[64];
	char IPAddrString[64];
	DWORD dwISState;

	re[indx].indx           = ++re_indx;
	NKDbgPrintfW(TEXT("***** DIAG NotifyCallback Indx=0x%x IS=%d [0=AS 1=IS] data=0x%x len=0x%x *****\r\n"), re_indx, dwUserData, *(DWORD*)pData, cbData);
	re[indx].dwUserData     = dwUserData;
	re[indx].pData          = *(DWORD*)pData;
	re[indx++].cbData       = cbData;
	indx &= (RE_LOG_SIZE - 1); //cyclic buffer

	//NKDbgPrintfW(TEXT("***** DIAG *S_NOTIFY_USER_DATA 0x%x len=0x%x *****\r\n"),dwState,cbData);

	pSI = (pRTR_UDP_SOCK_INFO)malloc(sizeof(RTR_UDP_SOCK_INFO));

	if (!pSI)
	{
		NKDbgPrintfW(TEXT("***** DIAG ERROR ASyncStateNotifyCallback - OOM *****\r\n"));
		return;
	}
	switch (dwUserData)
	{
	case AS_NOTIFY_USER_DATA:
		//NKDbgPrintfW(TEXT("***** DIAG AS_NOTIFY_USER_DATA 0x%x *****\r\n"),dwState);
		if (dwState)
		{
			// ASYNC connected:
			if (AsyncIsConnected)
			{
				NKDbgPrintfW(TEXT("***** DIAG please investigate this!!! AS is already connected *****\r\n"));
			}
			RegistryGetString(HKEY_LOCAL_MACHINE, L"COMM\\DTPT", L"DTPTCLI_ADDR", wIPAddrString, sizeof(IPAddrString));
			wcstombs(IPAddrString, wIPAddrString, sizeof(IPAddrString));
			pSI->ulBoardIpAddress = inet_addr((const char*)IPAddrString);
			RegistryGetString(HKEY_LOCAL_MACHINE, L"COMM\\DTPT", L"DTPTSRV_ADDR", wIPAddrString, sizeof(IPAddrString));
			wcstombs(IPAddrString, wIPAddrString, sizeof(IPAddrString));
			pSI->ulACATIpAddress = inet_addr((const char*)IPAddrString);
			NKDbgPrintfW(TEXT("***** DIAG Async Is Connected (before %x) *****\r\n"), AsyncIsConnected);
			NKDbgPrintfW(TEXT("***** DIAG Target IP=%x Registry=%x *****\r\n"), m_lTavorAddress, pSI->ulBoardIpAddress);
			NKDbgPrintfW(TEXT("***** DIAG PC     IP=%x Registry=%x *****\r\n"), m_lAddress, pSI->ulACATIpAddress );
			AsyncIsConnected = TRUE;
			if (ISIsConnected)
			{
				NKDbgPrintfW(TEXT("***** DIAG please investigate this!!! IS is connected when AS is connecting *****\r\n"));
				ISIsConnected = FALSE;
			}
		}
		else if (AsyncIsConnected)
		{
			// ASYNC Disconnected
			if (ISIsConnected)
			{
				NKDbgPrintfW(TEXT("***** DIAG please investigate this!!! IS is connected when AS is disconnecting *****\r\n"));
			}
			dwISState = 0;
			RegistryGetDWORD(HKEY_LOCAL_MACHINE, L"\\Comm\\RndisFn\\InternetSharing", L"Enabled",     &dwISState);
			if (dwISState)
			{
				// intersharing is active
				NKDbgPrintfW(TEXT("***** DIAG please investigate this!!! In registery IS is connected when AS is disconnecting *****\r\n"));
				RegistryGetString(HKEY_LOCAL_MACHINE, L"Comm\\ConnectionSharing\\InternalExposedHost\\LastDHCPAddress",
						  L"InternalName", wIPAddrString, sizeof(IPAddrString));

				wcstombs(IPAddrString, wIPAddrString, sizeof(IPAddrString));
				pSI->ulACATIpAddress = inet_addr((const char*)IPAddrString);
				pSI->ulBoardIpAddress = inet_addr(TAVOR_INTSHARE_STATIC_IP);
				//compare the IP addresses in registry with the ones currently in use by DIAG
				if (m_lTavorAddress != pSI->ulBoardIpAddress)
					NKDbgPrintfW(TEXT("***** DIAG please investigate this!!! Target IP=%x TAVOR_INTSHARE_STATIC_IP=%x *****\r\n"), m_lTavorAddress, pSI->ulBoardIpAddress);
				if (m_lAddress != pSI->ulACATIpAddress)
					NKDbgPrintfW(TEXT("***** DIAG please investigate this!!! PC     IP=%x Registry=%x *****\r\n"), m_lAddress, pSI->ulACATIpAddress );
			}
			else
			{
				NKDbgPrintfW(TEXT("***** DIAG IS is disconnected when AS is disconnecting. Reset the IP addresses to 0 *****\r\n"));
				pSI->ulACATIpAddress = pSI->ulBoardIpAddress = 0;
			}
			NKDbgPrintfW(TEXT("***** DIAG Async Is Disconnected *****\r\n"));
			AsyncIsConnected = FALSE;
		}
		else
		{
			NKDbgPrintfW(TEXT("***** DIAG Async Disconnect when already disconnected *****\r\n"));
			free(pSI); //IS maybe connected at this time and we don't want to reset the IP addresses
			return;
		}
		break;
	case IS_NOTIFY_USER_DATA:
		/*return this TBDIY if (AsyncIsConnected){
			free(pSI); // IGNORE this case we will get - ASYNC Disconnected in a better time
			return;
		   }*/

		//NKDbgPrintfW(TEXT("***** DIAG IS_NOTIFY_USER_DATA 0x%x ISIsConnected=0x%x *****\r\n"),dwState,ISIsConnected);
		if ((dwState & 0x1F) == 0x1F)
		{

			//UINT32 cnt;//patch for special case when the registry is not updated with a valid IP address for IS connected state

			if (ISIsConnected)
			{
				NKDbgPrintfW(TEXT("***** DIAG please investigate this!!! IS is already connected *****\r\n"));
			}
			NKDbgPrintfW(TEXT("***** DIAG (dwState & 0x1F) == 0x1F IS_NOTIFY_USER_DATA *****\r\n"));
			RegistryGetString(HKEY_LOCAL_MACHINE, L"Comm\\ConnectionSharing\\InternalExposedHost\\LastDHCPAddress",
					  L"InternalName", wIPAddrString, sizeof(IPAddrString));

			wcstombs(IPAddrString, wIPAddrString, sizeof(IPAddrString));
			pSI->ulACATIpAddress = inet_addr((const char*)IPAddrString);
			pSI->ulBoardIpAddress = inet_addr(TAVOR_INTSHARE_STATIC_IP);
			NKDbgPrintfW(TEXT("***** DIAG Target IP=%x TAVOR_INTSHARE_STATIC_IP=%x *****\r\n"), m_lTavorAddress, pSI->ulBoardIpAddress);
			NKDbgPrintfW(TEXT("***** DIAG PC     IP=%x Registry=%x *****\r\n"), m_lAddress, pSI->ulACATIpAddress );

			/*TBD better solution required: patch for special case when the registry is not updated with a valid IP address for IS connected state
			   cnt=0;
			   while (cnt++<100 && (pSI->ulACATIpAddress==0 || pSI->ulACATIpAddress==0xFFFFFFFF))
			   {
				NKDbgPrintfW(TEXT("***** DIAG please investigate this!!! IS is connected when ACATIpAddress==0 *****\r\n"));
				Sleep(100);
				RegistryGetString(HKEY_LOCAL_MACHINE,L"Comm\\ConnectionSharing\\InternalExposedHost\\LastDHCPAddress",
				L"InternalName",wIPAddrString,sizeof(IPAddrString));
				wcstombs(IPAddrString,wIPAddrString,sizeof(IPAddrString));
				pSI->ulACATIpAddress=inet_addr((const char*)IPAddrString);
			   }*/
			if (pSI->ulACATIpAddress == 0 || pSI->ulACATIpAddress == 0xFFFFFFFF)
			{
				pSI->ulACATIpAddress = inet_addr(ACAT_INTSHARE_STATIC_IP);
				NKDbgPrintfW(TEXT("***** DIAG IS is connected when ACATIpAddress is 0 or FFFFFFFF, overriding to %x *****\r\n"), pSI->ulACATIpAddress);
			}

			ISIsConnected = TRUE;
			if (AsyncIsConnected)
			{
				NKDbgPrintfW(TEXT("***** DIAG please investigate this!!! AS is connected when IS is connecting *****\r\n"));
				AsyncIsConnected = FALSE;
			}
		}
		else if (ISIsConnected)
		{
			// InternetSharing Disconnected
			if (AsyncIsConnected)
			{
				NKDbgPrintfW(TEXT("***** DIAG please investigate this!!! AS is connected when IS is disconnecting (should we reset the IP addresses to 0?) *****\r\n"));
				RegistryGetString(HKEY_LOCAL_MACHINE, L"Comm\\ConnectionSharing\\InternalExposedHost\\LastDHCPAddress",
						  L"InternalName", wIPAddrString, sizeof(IPAddrString));
				wcstombs(IPAddrString, wIPAddrString, sizeof(IPAddrString));
				pSI->ulACATIpAddress = inet_addr((const char*)IPAddrString);
				pSI->ulBoardIpAddress = inet_addr(TAVOR_INTSHARE_STATIC_IP);
				NKDbgPrintfW(TEXT("***** DIAG Target IP=%x TAVOR_INTSHARE_STATIC_IP=%x *****\r\n"), m_lTavorAddress, pSI->ulBoardIpAddress);
				NKDbgPrintfW(TEXT("***** DIAG PC     IP=%x Registry=%x *****\r\n"), m_lAddress, pSI->ulACATIpAddress );

				NKDbgPrintfW(TEXT("***** DIAG IS Is Disconnected. Don't reset the IP addresses to 0 *****\r\n"));
				free(pSI);
				return;
			}
			NKDbgPrintfW(TEXT("***** DIAG AS is disconnected when IS is disconnecting. Reset the IP addresses to 0 *****\r\n"));
			pSI->ulACATIpAddress = pSI->ulBoardIpAddress = 0;
			ISIsConnected = FALSE;
		}
		else
		{
			//NKDbgPrintfW(TEXT("***** DIAG IS Disconnect when already disconnected) *****\r\n"));
			free(pSI); // IGNORE this case we will get - ASYNC Disconnected in a better time
			return;
		}

		break;

	}

	//NKDbgPrintfW(TEXT("***** DIAG Reg notify set event TavorIP=%08X ACATIP=%08X *****\r\n"),pSI->ulBoardIpAddress,pSI->ulACATIpAddress);
	SetEventData(ReInitSockEvent, (DWORD)pSI);
	SetEvent(ReInitSockEvent);
}

BOOL InitSocketNotifications()
{
	NOTIFICATIONCONDITION notificationCondition;
	HREGNOTIFY hNotifyAS = NULL;
	HREGNOTIFY hNotifyIS = NULL;
	HRESULT hr;


	notificationCondition.dwMask = 0xFFFFFFFF;
	notificationCondition.ctComparisonType = REG_CT_ANYCHANGE;
	notificationCondition.TargetValue.dw = 0;

	if ((ReInitSockEvent = CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL)
		return FALSE;



	//YKYK  1. why create thread (not osa task?) priority?  last argument should be address ... (in sicat)
	if ((hSockEventThread = CreateThread(NULL, 0, SockEventThread, NULL, 0, NULL)) == NULL)
	{
		CloseHandle(ReInitSockEvent);
		return FALSE;
	}

	// TBD: Currently assume the following two will never really fail
	hr = RegistryNotifyCallback(HKEY_LOCAL_MACHINE,
				    L"\\System\\State\\Hardware",
				    L"Cradled",
				    ASyncStateNotifyCallback,
				    AS_NOTIFY_USER_DATA,
				    &notificationCondition,
				    &hNotifyAS);

	if (hr != S_OK)
	{
		NKDbgPrintfW(TEXT("***** DIAG Failed to register AS registry Callback *****\r\n"));
		return FALSE;
	}

	hr = RegistryNotifyCallback(HKEY_LOCAL_MACHINE,
				    L"\\System\\State\\Connectivity",
				    L"InternetSharing",
				    ASyncStateNotifyCallback,
				    IS_NOTIFY_USER_DATA,
				    &notificationCondition,
				    &hNotifyIS);

	if (hr != S_OK)
	{
		NKDbgPrintfW(TEXT("***** DIAG Failed to register AS registry Callback *****\r\n"));
		return FALSE;
	}

	return TRUE;
}

void CheckUsbPlug (LPVOID pParam)
{
	DWORD dwEvent;
	HANDLE phWaitHandles[] = { NULL, NULL, NULL, NULL };

	//HANDLE  UsbSwitchConfirmEvent=CreateEvent (NULL, FALSE, FALSE, _T("UsbSwitchConfirmEvent"));

	phWaitHandles[0] = CreateEvent(NULL, FALSE, FALSE, _T("UsbPlugInEvent"));  //TBD check the returned values
	phWaitHandles[1] = CreateEvent(NULL, FALSE, FALSE, _T("UsbPlugOutEvent"));
	phWaitHandles[2] = CreateEvent(NULL, FALSE, FALSE, _T("UsbSwitchEvent"));
	phWaitHandles[3] = CreateEvent(NULL, FALSE, FALSE, _T("BulEndpointEvent"));

	while (TRUE)
	{
		dwEvent = WaitForMultipleObjects(4, phWaitHandles, FALSE, INFINITE);

		switch (dwEvent)
		{
		//Usb Plug In event
		case WAIT_OBJECT_0:
			NKDbgPrintfW(TEXT("***** DIAG Usb Plug in event! *****\r\n"));
			if (/*diagIntData.m_eConnectionType==tUDPConnection || we have Ido's notification for UDP*/ diagIntData.m_eConnectionType == tUSBConnection)
			{
				//extern "C" UINT32 lastUSBevent;				// 0-none, 1-USB connect, 2-USB disconnect,10-various-error
				lastUSBevent = 1;
				diagExtIFstatusConnectNotify();
			}
			break;
		//Usb Plug Out event
		case WAIT_OBJECT_0 + 1:
			NKDbgPrintfW(TEXT("***** DIAG Usb Plug out event! *****\r\n"));
			if (/*diagIntData.m_eConnectionType==tUDPConnection || we have Ido's notification for UDP*/ diagIntData.m_eConnectionType == tUSBConnection)
			{
				//extern "C" UINT32 lastUSBevent;				// 0-none, 1-USB connect, 2-USB disconnect, 3-IPconnected,4-IPdisconnected,10-various-error
				lastUSBevent = 2;
				hUsbPortFunctional = 0;   // lets mark for us - disconnected
				diagExtIFStatusDiscNotify();
			}
			break;
		// UsbSwithcEvent
		case WAIT_OBJECT_0 + 2:
		{
			DWORD EvData = GetEventData(phWaitHandles[2]);
			//lastUSBevent = 10;
			if (EvData == CHANGE_TO_DIAG)        //TBDIY ask changing SICAT to DIAG: C:\WM603\PLATFORM\TavorEVB\Apps\USBSWITCH_1\UsbSwitch.cpp(514):		SetEventData(UsbSwitchEvent,CHANGE_TO_SICAT);
			{
				NKDbgPrintfW(TEXT("***** DIAG USB Switch event -> Serial USB *****\r\n"));
			}
			else if (EvData == CHANGE_TO_RNDIS)
			{
				NKDbgPrintfW(TEXT("***** DIAG USB Switch event -> RNDIS *****\r\n"));
			}
			else
			{
				NKDbgPrintfW(TEXT("***** DIAG USB Switch event unknown data *****\r\n"));
			}
		}
		break;
		case WAIT_OBJECT_0 + 3: //BulEndpointEvent
			//lastUSBevent = 10;
			NKDbgPrintfW(TEXT("***** DIAG USB BulEndpoint event! *****\r\n"));
			break;
		case WAIT_TIMEOUT:
			//lastUSBevent = 10;
			NKDbgPrintfW(TEXT("***** DIAG USB event timeout *****\r\n"));
			break;
		default:
			//lastUSBevent = 10;
			NKDbgPrintfW(TEXT("***** DIAG Unknown USB event *****\r\n"));
			break;
		}
	}
}

// specific API to get Time Stamp of WM
/************************************************************************
*                                                                      *
* Function: diagGetTS_WM				                                *
*                                                                      *
* Description:	Get TIMESTAMP from WM BEN -		                        *
*                                                                      *
*  Note: Returns the current time stamp								*
*        Read the Time Stamp from free-running-timer                   *
*                                                                      *
************************************************************************/
UINT32 diagGetTS_WM(void)
{
	// BRN TAVOR P/PV implementation for Time Stamp
	PHYSICAL_ADDRESS OST_PH;
	static volatile XLLP_OST_T  *pOSTReg = NULL;
	UINT32 oscr4_value;

	if (pOSTReg == NULL)
	{
		OST_PH.QuadPart = MONAHANS_BASE_REG_PA_OST;
		pOSTReg = ( P_XLLP_OST_T)MmMapIoSpace(OST_PH, sizeof(XLLP_OST_T), FALSE);
	}
	oscr4_value = pOSTReg->oscr4;
	//NKDbgPrintfW(L"***** DIAG TS, value =0x%x (%lu) *****\r\n", oscr4_value, oscr4_value);
	return oscr4_value;
}

#endif //defined (OSA_WINCE)
