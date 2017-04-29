#include "diag_al.h"
#include "pxa_dbg.h"
#include <pthread.h>
#include "linux_types.h"
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <linux/netlink.h>
#include <sys/un.h> //AF_UNIX
#include <netdb.h> //AF_UNIX
#include <linux/rtnetlink.h>//for RTMGRP_IPV4_IFADDR
#include <linux/version.h>
#include <unistd.h> /* close */
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <errno.h>
#include "diag.h"
#include "diag_buff.h"
#include "diag_rx.h"
#include "diag_tx.h"
#include "osa.h"
#include "osa_types.h"
#include "diag_init.h"
#include "shm_share.h"
#include <ctype.h>

#if defined (BIONIC)
#include <sys/socket.h>
#endif

#include "global_types.h"
#include "diag_comm.h"

#ifdef AT_PRODUCTION_CMNDS
#include "prod_api.h"
#endif

#include <arpa/inet.h> // in_addr_t, inet_addr

#if defined (BIONIC)
#include <sys/reboot.h> // reboot, RB_AUTOBOOT
#endif

#define DIAGPRINT DPRINTF
#define DIAGASSERT ASSERT

#define DIAG_TTY_USB_DEVICE "/dev/ttydiag0"
#define SERIAL_PORT8_NAME   "/dev/citty8"
#define DIAGSTUB_USB_DEVICE "/dev/diagdatastub"
#define DIAG_ETHER_USB_DEVICE "/sys/class/net/usb0"
#define USB_DRV_CONNECTED    "/sys/devices/platform/pxa-u2o/connected"
#define USB_DRV_COMPOSITE    "/sys/devices/platform/pxa-u2o/composite"
#define DIAG_COMPO_KEY      "diag"
#define ETHERNET_COMPO_KEY  "rndis"
#define PXA_GADGET_CABLE_CONNECTED '1'

#define USB_ANDROID_COMPOSITE    "/sys/devices/virtual/android_usb/android0/functions"
#define DIAG__ANDROID_COMPO_KEY      "marvell_diag"

#define DATABITS        CS8
#define BAUD            B115200
#define STOPBITS        0
#define PARITYON        0
#define PARITY          0

#define STRIDE_LEN      1024
#define DiagAlignMalloc malloc
#define DiagAlignFree   free
#define PTHREAD_MUTEXATTR_INITIALIZER 0

pthread_t diagUsbRxTask;
pthread_t diagSCRxTask;
pthread_t diagLocalRxTask;
pthread_t diagSCInit;
pthread_t diagRxTask;
pthread_t diagConnectTcpTask;

int diagNlFd;
//int diagdrvfd;

int hPort = -1;
static int serialfd;
static int mydiagstubfd = -1;
static int mydiagsockfd = -1;
static BOOL bDiagChannelInited = FALSE;
static BOOL bDiagDiscAcked = FALSE;
BOOL bDiagConnAcked = FALSE;
UINT32 diag_count = 0;
GlobalIfData dataExtIf;

UINT32 b_ExportedPtrStat = 0; //Enable or disable the print of the exported function pointer when invoked

DIAG_IMPORT const DiagDBVersion DBversionID;

DIAG_IMPORT const CommandAddress cat3CommandGlobalAddress[]; // only for RTOS (Nu?)
_diagIntData_S _diagInternalData;

// variables defined by pre-pass in its own lib/dll (wince, linux)
//DIAG_IMPORT const int			cat3CommandGlobalNumber;
extern const UINT32 _FirstCommandID;
extern const UINT32 _LastCommandID;
typedef struct{
	UINT32 state;	//0-disconnected, 1-connected, 2-trying to connect
	UINT32 cpsr;
	OS_Mutex_t lock;
}UdpState;
UdpState udp_state = {0,0, {PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEXATTR_INITIALIZER, 0, 0}};
UdpState tcp_state = {0,0, {PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEXATTR_INITIALIZER, 0, 0}};
UdpState local_state = {0,0, {PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEXATTR_INITIALIZER, 0, 0}};
UdpState cmi_state = {0,0, {PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEXATTR_INITIALIZER, 0, 0}};
extern unsigned long m_lTavorAddress;

Diag_Event diag_event;

Diag_Target diag_target;

int diag_server_port = -1;
extern int diag_server;

struct diag_client_struct{
	struct sockaddr_un client_addr;
	UINT32 CMMver; //for backward and forward CMM compatibility
	UINT32 firstCommandID;
	UINT32 lastCommandID;
	UINT32 firstReportID;
	UINT32 lastReportID;
	int connected;
};

#define MAX_DIAG_CLIENT_NUM 10
struct diag_client_struct diag_clients[MAX_DIAG_CLIENT_NUM];

#define SERVER_PORT 1500
#define MAX_MSG 100

#define BUFFSIZE    512

static int SerialClientIsInit = 0;
static char buffer[BUFFSIZE];
static char buffer_out[BUFFSIZE];
static pthread_t InputThread;
static void initUsbConnection();
static void processTCPEvent(int event_fd);
static void ReceiveDataFromDev(void);

extern int msend(int s, const void *buf, int len, int flags);
extern int mrecv(int s, void *buf, int len, int flags);
extern int msocket( int Port );
extern void connectCMI(void);
extern void connectLocal(void);
extern void connectUDP(void);
extern void connectTCP(void);
extern UINT32 SendCMIMsg(UINT8 * msg, UINT32 len, const void * clienaddr);
extern int SendLocalMsg(UINT8 * msg, UINT32 len);
extern UINT32 SendCommMsg(UINT8 *msg, UINT32 len);
extern int SendUDPMsg(UINT8 *, UINT32);
extern int SendTCPMsg(UINT8 *, UINT32);
extern OSA_STATUS InitDiagCachedWrite(void);
extern void diagExtIFStatusDiscNotify(void);
extern void diagCommL2ReceiveL1Packet   (UINT8   *buff , UINT32  length , BOOL isRecursive, COMDEV_NAME rxif);
void diag_target_init(EActiveConnectionLL_Type ltype, EActiveConnectionType etype );
void diagRcvDataFromClientTask(void *data) __attribute__((noreturn));
void DiagCommExtIfLocalRxTask(void *ptr) __attribute__((noreturn));
void DiagCommExtIfTCPRxTask(void *ptr) __attribute__((noreturn));
void diagInitTask(void *data);


#ifdef AT_PRODUCTION_CMNDS
int is_diag_running = 0;

static void init_diag_prod_conn(void);
extern int get_bspflag_from_kernel_cmdline(void);

#endif

static void ReceiveDataATCmdIF(void )
{
	/*
	 *  check if there is response
	 */
	while (SerialClientIsInit)
	{

		int bytes = 0;
		int i, j;
		if ((bytes = read(serialfd, buffer, BUFFSIZE - 1)) < 1)
		{
			//printf("Failed to receive bytes from server");
			sleep(1);
			continue;
		}
		buffer[bytes] = '\0';        /* Assure null terminated string */


		//printf("Data In length (%d).\n", bytes);

		//fprintf(stdout, buffer);
		// Send received indication to to diag
		DIAG_FILTER(VALI_IF, ATCMD_IF, ATOUT_CHAR, 0)
		diagPrintf("%s", buffer);
		for (i = 0, j = 0; i < bytes; i++)
		{
			if ( buffer[i] ==  0x0d )
			{
				i++;
				if (j == 0)
				{
					// If the line is empty send at least one SPACE
					buffer_out[0] =  ' ';
					buffer_out[1] =  '\0';
				}
				else
				{
					buffer_out[j] =  '\0';
				}
				DIAG_FILTER(VALI_IF, ATCMD_IF, ATOUT, 0)
				diagPrintf("%s", buffer_out);
				j = 0;
			}
			else
			{
				buffer_out[j++] = buffer[i];
			}
		}
		DIAGPRINT("%s", buffer_out);
	}

}
static void printRTC(void)
{
	time_t curtime;
	struct tm *curgmtime;

	if (1)
	{
		char sCalanderTime[256];
		time(&curtime);
		curgmtime = gmtime(&curtime);
		sprintf(sCalanderTime, "The RTC Value is: %d/%d/%d, %d:%d:%d",
			curgmtime->tm_mday,
			curgmtime->tm_mon + 1,
			curgmtime->tm_year + 1900,
			curgmtime->tm_hour,
			curgmtime->tm_min,
			curgmtime->tm_sec);
		DIAGPRINT("%s", sCalanderTime);
		DIAG_FILTER(Diag, Utils, Print_RTC, DIAG_INFORMATION)
		diagPrintf("%s", sCalanderTime);
		return;
	}
	else
	{
		DIAG_FILTER(Diag, Utils, ReadRTC_Failure, DIAG_INFORMATION)
		diagPrintf("Failed to read RTC");
	}
}
//ICAT EXPORTED FUNCTION - VALI_IF,ATCMD_IF,StartATCmdIF
void StartATCmdIF()
{
	pthread_attr_t tattr;

	void initCittyConnection();

	if (SerialClientIsInit)
	{
		// Already init - get out!
		DIAG_FILTER(VALI_IF, ATCMD_IF, ALREADYRUNNING, 0)
		diagPrintf("Error  -> Already Running!");
		return;
	}

	/*
	 *  Open the Device
	 */

	initCittyConnection();

	/*
	 *  Create a thread to recieve
	 */
	pthread_attr_init(&tattr);

	pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);

	SerialClientIsInit = 1;

	if ( pthread_create( &InputThread, &tattr, (void *)ReceiveDataATCmdIF, NULL) != 0 )
	{

		perror("pthread_create");
		exit( 1 );
	}

}

//ICAT EXPORTED FUNCTION - VALI_IF,ATCMD_IF,StopATCmdIF
void StopATCmdIF()
{
	if (!SerialClientIsInit)
	{
		// It was not init, nothing to close - get out!
		DIAG_FILTER(VALI_IF, ATCMD_IF, STOPIFNOTSTARTED, 0)
		diagPrintf("Interface was not started jet");
		return;
	}
#if !defined (BIONIC)
	pthread_cancel(InputThread);
#endif
	SerialClientIsInit = 0;
}


//ICAT EXPORTED FUNCTION - VALI_IF,ATCMD_IF,SendATCmd
void SendATCmd(char* Cmd)
{
	unsigned char buf[514];
	int rc, i;
	int recieved;

	DIAGPRINT(">>>>>>>>>Enter SendAT CMD!!!\r\n");
	DIAGPRINT(">>>SerialClientIsInit: %d\r\n", SerialClientIsInit);
	//Check for init
	if (!SerialClientIsInit)
	{
		DIAG_FILTER(VALI_IF, ATCMD_IF, ATCMDIFNOTSTARTEDINSENDAT, 0)
		diagPrintf("Received AT Command but interface not started");
		return;
	}
	recieved = 0;
	for (i = 0; i < (int)sizeof(buf) - 1; i++)
	{
		int symb;
		symb = (int)(*(unsigned char*)Cmd);
		// Check for ASCII
		if (isprint(symb) || (symb == 0x1a))     // Check for printable and cntrl-Z
		{
			buf[i] = (*(unsigned char*)Cmd);
			Cmd++;
			recieved++;
		}
		else
		{
			break;
		}
	}
	if(recieved > 0)
	{
		if (buf[recieved - 1] == '\a' || buf[recieved - 1] == 0xa )
		{
			buf[recieved - 1] = 0xd;
		}
		else
		{
			buf[recieved++] = 0xd;
		}
		buf[recieved] = 0;  //Add NULL termination to the string
		DIAGPRINT(">>>SendATCmd: %s\r\n", buf);
		rc = write(serialfd, buf, recieved);

		if (rc < 0)
		{
			DIAGPRINT("cannot send data ");
			//close(sd);
			//exit(1);

		}
	}
}
//ICAT EXPORTED FUNCTION - VALI_IF,ATCMD_IF,SendATCmdChars
void SendATCmdChars(char* Cmd)
{
	int rc;
	int recieved;

	//Check for init
	if (!SerialClientIsInit)
	{
		DIAG_FILTER(VALI_IF, ATCMD_IF, ATCMDIFNOTSTARTEDINSENDATCHAR, 0)
		diagPrintf("Received AT Command but interface not started");
		return;
	}

	recieved = strlen(Cmd);
	rc = write(serialfd, Cmd, recieved);

	if (rc < 0)
	{
		perror("cannot send data ");
		//close(sd);
		//exit(1);
	}

}

//ICAT EXPORTED FUNCTION - VALI_IF,MISC_IF,SendSystem
void SendSystem(char* Cmd)
{
	char buf[514];
	int i;
	int recieved;

	recieved = 0;
	for (i = 0; i < (int)sizeof(buf) - 1; i++)
	{
		int symb;
		symb = (int)(*(unsigned char*)Cmd);
		// Check for ASCII
		if (isprint(symb) )     // Check for printable
		{
			buf[i] = (*(unsigned char*)Cmd);
			Cmd++;
			recieved++;
		}
		else
		{
			break;
		}
	}

	buf[recieved] = 0;  //Add NULL termination to the string
	DIAG_FILTER(VALI_IF, MISC_IF, ATCMDSYSTEM, 0)
	diagPrintf("Received command: %s", buf);

	DIAGPRINT(">>>SendSystem:%s\r\n", buf);

#if defined (BIONIC)
	if(strncmp(buf, "reboot", strlen("reboot")) == 0)
		reboot(RB_AUTOBOOT);
	else
#endif
		system(buf);

}


/************************************************************************
* Function: diagCommandServer                                          *
************************************************************************
* Description: this function is responsible to invoke the requested    *
*              Non-Response application command.                       *
*                                                                      *
* Parameters: UINT16 moduleID, UINT32 commandID - ACAT IDs to indicated *
*				commnad to activate on target.							*
*                               void *commandData - parameters of the commnad		    *
*				UINT32 dataLength - lenght of parameters				*
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
************************************************************************/
void diagCommandServer(UINT32 commandID, PackedUnPackedData *commandData, UINT32 dataLength)
{
	FunctionPtr ptrToFunction;
	UINT32 fID = /*(((UINT32)moduleID)<<16) + */ commandID; //Ignore the module ID: commandID is flat

	if (fID >= _FirstCommandID && fID <=  _LastCommandID)
	//if((int)fID<cat3CommandGlobalNumber)//Fix the signed/unsigned mismatch
	{
		if (b_ExportedPtrStat)
		{
			DIAG_FILTER(DIAG, Dispatcher, ExportedPtr, DIAG_INFO)
			diagPrintf("Dispatching exported function pointer [0x%x%x]", (((unsigned int)cat3CommandGlobalAddress[fID - _FirstCommandID]) >> 16) & 0xffff, ((unsigned int)cat3CommandGlobalAddress[fID - _FirstCommandID]) & 0xffff);
			fprintf(stderr, "***** DIAG Dispatching exported function pointer [0x%x] *****\r\n", (unsigned int)cat3CommandGlobalAddress[fID - _FirstCommandID]);
		}

		ptrToFunction = (FunctionPtr)cat3CommandGlobalAddress[fID - _FirstCommandID];
		ptrToFunction(commandData, dataLength);
	}
	else
	{
		DIAG_FILTER(SW_PLAT, DIAG, COMMANDID_WRONG, DIAG_ERROR)
		diagPrintf("DIAG - command received with ID out of range: %d", fID);
	}
	return;
} /* End of diagCommandServer*/

static void openNetlink(void)
{
	struct sockaddr_nl snl;

	memset(&snl, 0, sizeof(snl));
	snl.nl_family = AF_NETLINK;
	snl.nl_pid = getpid();
	snl.nl_groups = 1;

	diagNlFd = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
	if (diagNlFd < 0)
	{
		DIAGPRINT("openNetlink socket create error.\r\n");
		return;
	}

	if (bind(diagNlFd, (struct sockaddr *)&snl, sizeof(struct sockaddr_nl)) < 0)
	{
		DIAGPRINT("openNetlink socket bind error.\r\n");
		close(diagNlFd);
	}
	return;

}
static const char *searchNetlink(const char *key, const char *buf, size_t len)
{
	size_t curlen;
	size_t keylen = strlen((char *)key);
	char *cur = (char *)buf;

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

/******************************************************************************
 *****
 *
 *      Name:			FindSoTOffset
 *
 *      Parameters:     void* pHeader -         pointer to the message
 *
 *      Returns:        the offset from message start to the SoT pattern
 *
 *      Descriptiton:	The function runs over the msg until all SoT patterns appears

 *******************************************************************************
 ****/
UINT8 FindSoTOffset(void* pHeader)
{
	UINT8 offset = 0, i = 0;
	UINT8 *pUINT8Header = (UINT8 *)pHeader;

	while (i < 10)
	{

		/* scan the received message until notifying the first byte of the SoT pattern */
		while ((*pUINT8Header != SOT_FIRST_BYTE) && (i < 9))
		{
			pUINT8Header++;
			offset++;
			i++;
		}

		pUINT8Header++;

		/* if next byte is the second byte of SoT pattern, we found the offset */
		if (*pUINT8Header == SOT_SECOND_BYTE)
		{
			break;
		}

		pUINT8Header++;
		offset++;

		i++;
	}

	if (i == 10)
	{
		return 0;
	}
	else
	{
		return offset;
	}
}

/******************************************************************************
 *****
 *
 *      Name:			GetUsbMsgLen
 *
 *      Parameters:     void* pHeader -         pointer to the message
 *
 *      Returns:        the length of the message. See more details in the
   description.
 *
 *      Descriptiton:	The length returned is the num of bytes of the whole message
   , including all its fields.
 *					Pay attention that this value is different than the total length field
   in the header!

 *******************************************************************************
 ****/
UINT32 GetUsbMsgLen (void* pHeader)
{
#if !defined (DIAGNEWHEADER)
	//Assuming PID field in USB header is always zero
	if ((UINT32)pHeader % 4)  // address of lenght (4 bytes) is not aligned. Rearrange data
	{
		UINT32 len = 0;
		UINT8 *lptr = (UINT8 *)pHeader;

		len = (UINT32)(*lptr) | (UINT32)(*(lptr + 1) << 8) | (UINT32)(*(lptr + 2) << 16) | (UINT32)(*(lptr + 3) << 24);
		//DIAG_WM_CONSOLE_PRN( (TEXT("***** DIAG GetUsbMsgLen pHeader is not 32 bits aligned!!! *****\r\n")) );
		return len;
	}
	else
	{
		return *((UINT32 *)pHeader);
	}
#else   /* DIAGNEWHEADER is defined */
	UINT32 totalLength = 0;

	/* find the SoT offset in order to know where total length field is */
	UINT8 sotOffset = FindSoTOffset(pHeader);

	/* the pointer to the header should point to the msg plus the offset found */
	TxPDUHeader *pTxPDUHeader = (TxPDUHeader *)((UINT8 *)pHeader + sotOffset);

	/* if checksum exist, than total length field should be added with SoT, EoT
	   and checksum */
	/* in addition, offset should be added as the num of bytes declared in the
	   header does not include them */
	if (pTxPDUHeader->msgTotalLen & CHECKSUM_BIT)
	{
		totalLength = ((UINT32)pTxPDUHeader->msgTotalLen) & (~CHECKSUM_BIT);
		return totalLength + SIZE_OF_SOT_PATTERN + SIZE_OF_CHECKSUM + SIZE_OF_EOT_PATTERN + sotOffset;
	}
	/* if checksum does not exist, than total length field should be added with SoT and EoT only */
	/* in addition, offset should be added as the num of bytes declared in the header does not include them */
	else
	{
		return ((UINT32)pTxPDUHeader->msgTotalLen) + SIZE_OF_SOT_PATTERN + SIZE_OF_EOT_PATTERN + sotOffset;
	}
#endif  /* DIAG_NEW_HEADER */
}


/***********************************************************************
* Function: diagCommSetChunk                                           *
************************************************************************
* Description: Used to add all USB format messages (coming from USB or *
*              CMI) or new-header messages chunks into the recieiving  *
*              buffer.                                                 *
*                                                                      *
* Parameters:                                                          *
*		msgData       - pointer to buffer.							   *
*		lenght        - lenght of data.								   *
*       bRemoveHeader - When true - sets the header offset to 4 on the *
*                       first chunk.                                   *
*                       In distributed CMI mode the offset is zero.    *
*                       Not relevant to DIAGNEWHEADER.                 *
*                                                                      *
* Return value: 0 - if data was discarded.                             *
*               1 - when data is added to RxPacket                     *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
UINT32 diagCommSetChunk(DIAG_COM_RX_Packet_Info  *RxPacket, CHAR *msgData, UINT32 length, UINT32 bRemoveHeader)
{
#if !defined (DIAGNEWHEADER)
	UINT32 lengthOfRecMsg = 0;
#endif

	DIAGPRINT("Enter diagCommSetChunk\n ");

	RxPacket->buffer                = msgData;
	RxPacket->nReTriesToPerform     = DIAG_RX_COMMAND_RETRIES; // diagRecieveCommand will release its buffer if failed to put-in-q

#if defined (DIAGNEWHEADER)
	// In new header, any fragment of message is ok, the diagReceiveCommand command will handle the state machine
	RxPacket->total_bytes_expected = length;
	RxPacket->total_bytes_received = length;
	RxPacket->handling_state = DIAG_COM_CompletePacketState;
	RxPacket->data_offset = 0; //new header

#else   // Old header

	DIAGPRINT("lengthOfRecMsg:%d, length: %d\r\n", lengthOfRecMsg, length);

	switch (RxPacket->handling_state)
	{
	case  DIAG_COM_PacketLastChunkState:
	case  DIAG_COM_CompletePacketState:
		//The length of the message is only in the first chunk
		lengthOfRecMsg = GetUsbMsgLen((void*)msgData);  //TBDIY potential bug - at least 4 bytes of msgData are needed for lengthOfRecMsg! (length>=4)
	#if !defined (OSA_NUCLEUS)
		//Handle case where USB driver got several messages in one buffer (WinCE, Linux) they are processed one by one
		if (length > lengthOfRecMsg)
			length = lengthOfRecMsg;
	#endif
		#if (!defined (OSA_LINUX) && !defined (OSA_WINCE))
		//In linux and windows the driver can give diag very small chunks...
		//For instance, the filter file (in Diag over FS) is sent in fragments causing some tiny fragments.
		//Handle a specific case - Yan M
		if (  (length < RX_PDU_HEADER_SIZE)
		      || !( (length == lengthOfRecMsg) ||
			    ( (length == DIAG_COM_USB_MAX_PACKET_BYTE_SIZE) && (length < lengthOfRecMsg) ))
		      )
		{
			lengthOfRecMsg = length;
			RxPacket->handling_state       = DIAG_COM_CompletePacketState;
			RxPacket->end_of_packet        = 1;
			RxPacket->data_offset          = (bRemoveHeader ? 4 : 0); //TBDIY Header in USB format 2 bytes Length (includes header size) and 2 bytes PID
			RxPacket->nReTriesToPerform    = 0; //don't execute in the diagCommReceiveHISR()
			return 0; //TBDIY potential bug when returning 0
		}
		#endif //(!defined (OSA_LINUX) && !defined (OSA_WINCE))
		RxPacket->total_bytes_expected  = lengthOfRecMsg;
		if (length == lengthOfRecMsg)
			RxPacket->handling_state                = DIAG_COM_CompletePacketState;
		else
			RxPacket->handling_state                = DIAG_COM_PacketFirstChunkState;
		RxPacket->total_bytes_received  = length;
		RxPacket->data_offset = (bRemoveHeader ? 4 : 0); //TBDIY Header in USB format 2 bytes Length (includes header size) and 2 bytes PID
		break;
	case DIAG_COM_PacketFirstChunkState:
	case DIAG_COM_PacketNextChunkState:
	#if !defined (OSA_NUCLEUS)
		//Handle a case when getting in one chunk the end of the current message and data from the next message
		if (RxPacket->total_bytes_received + length >= RxPacket->total_bytes_expected)
		{
			length = RxPacket->total_bytes_expected - RxPacket->total_bytes_received;
		}
	#endif
		/* accumulate the num of received bytes */
		RxPacket->total_bytes_received += length;
		if (RxPacket->total_bytes_expected == RxPacket->total_bytes_received)
			RxPacket->handling_state = DIAG_COM_PacketLastChunkState;
		else
			RxPacket->handling_state = DIAG_COM_PacketNextChunkState;
		/* in both headers there is no general header before the specific message in that scenario */
		//RxPacket->data_offset = 0;//no header on the 2nd chunk and on
		break;
	default:
		DIAG_ASSERT(0);
	}


#endif  //DIAGNEWHEADER

	RxPacket->current_chunk_bytes   = length;
	//RxPacket->rxIF = DIAG_COMMDEV_EXT;/*TBDIY select CMI client*/ done in DiagCommCMIfInit and DiagCommExtIfInit

	DIAGPRINT("Out diagCommSetChunk \r\n");

	return length;
}

/* The functions below are getters which return each field in the RxPDU msg */
UINT16 GetRxMsgDiagSap (UINT8 *rxMsg)
{
#if !defined (DIAGNEWHEADER)
	RxPDU *pRxPdu = (RxPDU *)rxMsg;

	return (UINT16)pRxPdu->DiagSAP;

#else   /* DIAGNEWHEADER is defined */
	pLongMsg *ppLongMsg = (pLongMsg *)rxMsg;

	return ppLongMsg->rxMsgHolder.messageSAP;
#endif  /* DIAGNEWHEADER */
}
UINT16 GetRxMsgServiceID (UINT8 *rxMsg)
{
#if !defined (DIAGNEWHEADER)
	RxPDU *pRxPdu = (RxPDU *)rxMsg;

	return pRxPdu->serviceID;

#else   /* DIAGNEWHEADER is defined */
	pLongMsg *ppLongMsg = (pLongMsg *)rxMsg;

	return (UINT16)ppLongMsg->rxMsgHolder.sapRelatedData;
#endif  /* DIAGNEWHEADER */
}

UINT16 GetRxMsgModuleID (UINT8 *rxMsg)
{
#if !defined (DIAGNEWHEADER)
	RxPDU *pRxPdu = (RxPDU *)rxMsg;

	return pRxPdu->moduleID;

#else   /* DIAGNEWHEADER is defined */
	pLongMsg *ppLongMsg = (pLongMsg *)rxMsg;

	/* in the new header the module ID is located in the 16 LS bits of the SAP related data */
//	return (UINT16) (ppLongMsg->rxMsgHolder.sapRelatedData & 0x00ff);
	return (ppLongMsg->rxMsgHolder.sapRelatedData >> 16);
#endif  /* DIAGNEWHEADER */
}

UINT32 GetRxMsgCommID (UINT8 *rxMsg)
{
#if !defined (DIAGNEWHEADER)
	RxPDU *pRxPdu = (RxPDU *)rxMsg;

	return pRxPdu->commID;

#else   /* DIAGNEWHEADER is defined */
	pLongMsg *ppLongMsg = (pLongMsg *)rxMsg;

//	return (ppLongMsg->rxMsgHolder.sapRelatedData >> 16);
	return (UINT16)ppLongMsg->rxMsgHolder.sapRelatedData;
#endif  /* DIAGNEWHEADER */
}

UINT32 GetRxMsgSourceID (UINT8 *rxMsg)
{
	RxPDU *pRxPdu = (RxPDU *)rxMsg;

	return pRxPdu->sourceID;
}
UINT8 *GetRxMsgFunctionParams (UINT8 *rxMsg)
{
#if !defined (DIAGNEWHEADER)
	RxPDU *pRxPdu = (RxPDU *)rxMsg;

	return &pRxPdu->functionParams[0];

#else   /* DIAGNEWHEADER is defined */
	pLongMsg *ppLongMsg = (pLongMsg *)rxMsg;

	return ppLongMsg->rxMsgHolder.payloadData;
#endif  /* DIAGNEWHEADER */
}
/************************************************************************
* Function: diagGetDBVersionID                                         *
************************************************************************
* Description: this function takes the Diag DB Version ID from the DB  *
*              and transfer it to the diagSendPDU function that sends  *
*              it to the ICAT.                                         *
*                                                                      *
* Parameters:  none                                                    *
*                                                                      *
* Return value:                                                        *
*                                                                      *
* Notes:                                                               *
************************************************************************/
static void diagGetDBVersionID (void)
{
	DiagDBVersion dbVerID = DBversionID;
	UINT32 numOfDecreasedBytes;
	int size = strlen(dbVerID) + 1;
	TxPDU *buf = (TxPDU *)(diagAlignAndHandleErr(size, 0, &numOfDecreasedBytes,
#if !defined (DIAGNEWHEADER)
						     RETURN_SERVICE
#else
						     SAP_INTERNAL_SERVICE_RESPONSE
#endif  /* DIAGNEWHEADER */
						     ));

	if (!buf)
		return;

	strcpy((char *)(buf->data - numOfDecreasedBytes), (char *)(dbVerID));
#if !defined (DIAGNEWHEADER)
	diagSendPDU(RETURN_SERVICE, RETURN_SERVICE_MODULE_ID, DB_VERSION_SERVICE, buf, size, numOfDecreasedBytes);
#else
	diagSendPDU(SAP_INTERNAL_SERVICE_RESPONSE, RETURN_SERVICE_MODULE_ID, DB_VERSION_SERVICE, buf, size, numOfDecreasedBytes);
#endif  /* DIAGNEWHEADER */
	buf = 0;

	return;

} /* End of diagGetDBVersionID   */

/************************************************************************
* Function: diagInternalServiceHandler                                 *
************************************************************************
* Description: adresses the requested service to the appropriate       *
*              handler according to opcode parameter                   *
*                                                                      *
* Parameters:  serviceID - the ID of the requested service             *
*              data - pointer to the requested service data            *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
************************************************************************/
static void diagInternalServiceHandler (UINT32 rxIF, UINT16 serviceID, PackedUnPackedData *data, UINT16 len)
{
	UNUSEDPARAM(rxIF)
	UNUSEDPARAM(data)
	UNUSEDPARAM(len)

	switch (serviceID)
	{
	case DB_VERSION_SERVICE:
		diagGetDBVersionID();
		break;

	case TRANSFER_DB_SERVICE:
		//diagTransferDiagDB();
		break;

	case GET_REF_CLK_RATE_SERVICE:
		//diagGetReferenceClockRate();
		break;

	case SET_FILTER_LEVEL_SERVICE:
		//diagSetFilter(data, len);
		break;

	case ICAT_READY_NOTIFY:
		//diagICATReady(rxIF);
		break;

	case GET_FILTER_SERVICE:
		//diagGetFilter();
		break;

	case ENTER_BOOTLOADER_MODE:
		//diagSetBootloaderMode();
		break;

	case EXTERNAL_CONNECTION_CEASED:
		//diagDisconnectIcat();
		//diagDisconnectIcatInt(0);
		//ResetDiagBuffers(); // clear accumulated traces / signals, to start from clean once connection is set again.
		break;

	case SEND_FULL_FRAME_NUMBER_SERVICE:
		//diagSendFullFrameNumber();
		break;

	case ATOMIC_READY_NOTIFY_AND_DB_VER:
		//diagICATReady(rxIF);
		//diagGetDBVersionID();
		break;

	default:
		DIAG_FILTER(SW_PLAT, DIAG, RX_SERVICEID_WRONG, DIAG_FATAL_ERROR)
		diagPrintf("DIAG - msg received with wrong RX service ID, the serviceID is %d", serviceID);
		break;
	} /*end of switch */

	return;

} /* End of diagInternalServiceHandler   */

/************************************************************************
* Function: diagSendData2CP                                            *
************************************************************************
* Description: this function gets the message from caller, and adds    *
*              header for it, then translates header+data to msocket   *
*                                                                      *
* Parameters:  msg   : the data to be sent to CP                       *
*              msglen: the length of data                              *
*                                                                      *
* Return value:                                                        *
*                                                                      *
* Notes:                                                               *
************************************************************************/

void diagSendData2CP(unsigned char*msg, int msglen)
{
	UINT8 *pTxbuf;
	DIAGHDR *pHeader;

	pTxbuf = malloc(msglen + sizeof(DIAGHDR));

	pHeader = (DIAGHDR*)pTxbuf;
	pHeader->packetlen = msglen;
	pHeader->seqNo = 0;
	pHeader->reserved = 0;
	memcpy(pTxbuf + sizeof(DIAGHDR), msg, msglen);
	//diagTxRawData(pTxbuf,msglen+sizeof(DIAGHDR));
	msend(mydiagsockfd, pTxbuf, msglen + sizeof(DIAGHDR), 0);

	free(pTxbuf);

}
void diagRcvDataFromClientTask(void *data)
{
	UNUSEDPARAM(data)

	UINT8 * diagDataRecvMsg, * rawPacket;
	TxPDUCMMHeader * cmmHead;
	int dwBytesTransferred = -1;
	UINT32  *pData;
	UINT16   pduLen, received;
	void diagSCInitTask(void *data);
	struct sockaddr_un clintaddr;
	int addr_len = sizeof(struct sockaddr_un);
	int i;
	diagDataRecvMsg = malloc(MAX_DIAG_DATA_RXMSG_LEN + TX_PDU_FOOTER_SIZE);//align?
	DIAGPRINT("*****DIAG Enter diagRcvDataFromClientTask!!!\n");
	received = 0;
	while (1)
	{
		dwBytesTransferred = recvfrom(diag_server_port, diagDataRecvMsg, MAX_DIAG_DATA_RXMSG_LEN, 0,(struct sockaddr*)&clintaddr, (socklen_t*)&addr_len);
		if (dwBytesTransferred <= 0)
			continue;
		cmmHead = (TxPDUCMMHeader *)diagDataRecvMsg;
		if((cmmHead->DiagSAP  == (CLIENT_TO_MASTER_CMM | (DIAG_CORE_ID? DIAG_CORE_SELECTOR_RX_MASK : 0))))
		{
			switch(cmmHead->serviceID)
			{
			case REGISTER_CMM_SER_ID:
			{
				DIAGPRINT("*****DIAG Recive CMM register mesage:%d!!!\n",cmmHead->serviceID);
				UINT32 * param;
				param = (UINT32 *)(diagDataRecvMsg + sizeof(TxPDUCMMHeader));
				for(i=0; i<MAX_DIAG_CLIENT_NUM;i++)
				{
					if(diag_clients[i].connected)
					{
						if( memcmp((char *)&diag_clients[i].client_addr, (char *)&clintaddr, sizeof(struct sockaddr_un)) ==0)
							break;
						else
							continue;
					}
					else
					{
						memcpy((char *)&diag_clients[i].client_addr, (char *)&clintaddr, sizeof(struct sockaddr_un));
						diag_clients[i].connected = 1;
						diag_clients[i].firstCommandID = param[0];
						diag_clients[i].lastCommandID = param[1];
						diag_clients[i].firstReportID = param[2];
						diag_clients[i].lastReportID = param[3];
						break;
					}
				}
				break;
			}
			default:
				break;
			}
			continue;
		}

		rawPacket = diagDataRecvMsg;

		pData = (UINT32*)(diagDataRecvMsg);

		switch (*pData)
		{
		//case: reserved for future use.
		default:
			pduLen = dwBytesTransferred;
			if(hPort >= 0 || diag_target.hPort >=0)
			{
				if(diag_target.raw_diag_support == 1)
				{
					diag_count++;
					UINT8 * pbuff;
					pbuff = (UINT8 *)pData;
					SendCommMsg(pbuff, pduLen);
				}
			}
			break;
		}
	}
	DIAGPRINT("Exit diagRcvDataFromClientTask!!!\n");
}
//ICAT EXPORTED FUNCTION - Diag,timeprint, diagRcvDataFromCPTask
void diagRcvDataFromCPTask(void *data)
{
	UNUSEDPARAM(data)

	UINT8 * diagDataRecvMsg, * rawPacket;
	int dwBytesTransferred = -1;
	UINT32  *pData;
	DIAGHDR *pHeader;
	UINT16   pduLen, received;
	void diagSCInitTask(void *data);
	BOOL Wraped = TRUE;
	struct diag_target_buffer *diag_buffer;

	diag_buffer = malloc(sizeof(struct diag_target_buffer));
	if(!diag_buffer)
	{
		DIAGPRINT("***** DIAG CP Chunk Buff is NULL *****\r\n");
		DIAG_ASSERT(0);
	}
	diag_disc_flush(diag_buffer);
	diagDataRecvMsg = malloc(MAX_DIAG_DATA_RXMSG_LEN + TX_PDU_FOOTER_SIZE);//align?
	if(!diagDataRecvMsg)
	{
		DIAGPRINT("***** DIAG CP RX TransferBuffer is NULL *****\r\n");
		free(diag_buffer);
		DIAG_ASSERT(0);
	}
	DIAGPRINT("*****DIAG Enter diagRcvDataFromCPTask!!!\n");
	received = 0;
	while (1)
	{
		dwBytesTransferred = mrecv(mydiagsockfd, diagDataRecvMsg, MAX_DIAG_DATA_RXMSG_LEN, 0);
		if (dwBytesTransferred <= 0)
			continue;

		pHeader = (DIAGHDR*)diagDataRecvMsg;

		if (pHeader->packetlen == 0)
		{
			DIAGPRINT("Meet Zero packet len !!!\n");
			continue;
		}

		rawPacket = diagDataRecvMsg + sizeof(DIAGHDR);
		pData = (UINT32*)(diagDataRecvMsg + sizeof(DIAGHDR));

		switch (*pData)
		{
		case DiagDataConfirmStartMsg:
			bDiagChannelInited = TRUE;
			DIAGPRINT(">>DiagDataConfirmStartMsg\n");
			printf(">>>DiagDataConfirmStartMsg\n");
			break;
		case DiagExtIfConnectedAckMsg:
			bDiagConnAcked = TRUE;
			DIAGPRINT(">>DiagExtIfConnectedAckMsg\n");
			printf(">>>DiagExtIfConnectedAckMsg\n");
			break;
		case DiagExtIfDisconnectedAckMsg:
			bDiagDiscAcked = TRUE;
			break;
		case MsocketLinkdownProcId:
			DIAGPRINT("%s: received MsocketLinkdownProcId!\n", __FUNCTION__);
			bDiagChannelInited = FALSE;
			bDiagConnAcked = FALSE;
			bDiagDiscAcked = FALSE;
			break;
		case MsocketLinkupProcId:
			DIAGPRINT("%s: received MsocketLinkupProcId!\n", __FUNCTION__);

			//'pthread_cancel' is not implemented in Android libpthread.
			//Fortunately, we don't need pthread_cancel here.
			//
			//if (pthread_cancel(diagSCInit) == -1)
			//{
			//	DIAGPRINT("pthread_cancel return -1\r\n");
			//}

			if( pthread_create( &diagSCInit, NULL, (void *)diagInitTask, NULL) != 0 )
			{
				DIAGPRINT("pthread_create diagRcvDataFromCPTask error");
				free(diag_buffer);
				free(diagDataRecvMsg);
				return;
			}

			break;

		default:
			pduLen = dwBytesTransferred - sizeof(DIAGHDR);
			if(hPort >= 0 || diag_target.hPort >=0)
			{
				if((diag_count % 1000 == 0) && Wraped)
				{
					DIAGPRINT("diag_count:%d, read RTC\n",diag_count);
					printRTC();
				}
				{
					char * pbuff = (char *)pData;
					int ret;
					do
					{
						ret = diag_disc_rx(diag_buffer, pbuff,pduLen);
						if(ret >= 0)
						{
							SendCommMsg((UINT8 *)diag_buffer->chunk,diag_buffer->chunk_size);
							diag_disc_flush(diag_buffer);
							pbuff += (pduLen-ret);
							pduLen = ret;
							diag_count++;
						}

					}while(ret > 0);
				}
			}
			break;
		}
	}
	free(diagDataRecvMsg);
	free(diag_buffer);
	DIAGPRINT("Exit diagRcvDataFromCPTask!!!\n");
}

BOOL IsICATReadyNotify_UM(UINT8* pData)
{
#if !defined (DIAGNEWHEADER)
	RxPDU *pRxPdu;
#else /* DIAGNEWHEADER is defined */
	RxMsgHolder *pRxMsgHolder;
#endif /* DIAGNEWHEADER */
	if (!pData)
	{
		return FALSE;
	}
#if !defined (DIAGNEWHEADER)
	pRxPdu = (RxPDU *) pData;

	return ((pRxPdu->serviceID == ICAT_READY_NOTIFY) || (pRxPdu->serviceID == ATOMIC_READY_NOTIFY_AND_DB_VER));  /* pData[1] is start of serviceID - UINT8 field  */

#else /* DIAGNEWHEADER is defined */
	pRxMsgHolder = (RxMsgHolder *) pData;

	return ((pRxMsgHolder->messageSAP == SAP_INTERNAL_SERVICE_REQUEST) &&
			 (pRxMsgHolder->isSapRelatedData & SAP_RELATED_DATA_BIT) &&
			 ((pRxMsgHolder->sapRelatedData == ICAT_READY_NOTIFY) ||
				(pRxMsgHolder->sapRelatedData == ATOMIC_READY_NOTIFY_AND_DB_VER)));

#endif /* DIAGNEWHEADER */
}

void dispatchServiceForSAP(UINT8 *Ori_data)
{
	UINT16 Sap, ServiceID, dataLen;
	UINT32 MessageID;
	UINT8 *data;
	UINT8 *new_buff;
	UINT16 i;
	UINT32 ori_len, new_len;
	//UINT8 dumpCP[50];
	BOOL bNotifyCP = FALSE;
	ori_len = new_len = 0;
	data = Ori_data + dataExtIf.RxPacket->data_offset;

	Sap = GetRxMsgDiagSap(data);
	dataLen = dataExtIf.RxPacket->total_bytes_received-
	          dataExtIf.RxPacket->data_offset - RX_PDU_HEADER_SIZE;

	DIAGPRINT(">>> bytes: %d, data_offset %d, SAP:%d\r\n",
	          dataExtIf.RxPacket->total_bytes_received,
	          dataExtIf.RxPacket->data_offset, Sap);

	DIAGPRINT(">>>dispatchServiceForSAP dataLen %d\n", dataLen);

	//sprintf(dumpCP,"Data:0-4:%2x,%2x,%2x,%2x",data[0],data[1],data[2],data[3]);
	//DIAG_FILTER(VALI_IF,ATCMD_IF,ATOUT_CHAR,0)
	//diagPrintf("%s", dumpCP);


#if !defined (DIAGNEWHEADER)
	if (Sap & 0x80)  //AP
	{
		switch (Sap & 0x7f)
		{
		case INTERNAL_SERVICE:
			ServiceID = GetRxMsgServiceID(data);
			DIAGPRINT(">>>>>Internal Service: Service %d\r\n", ServiceID);
			diagInternalServiceHandler(DIAG_COMMDEV_EXT,
			                           ServiceID,
			                           (PackedUnPackedData *)(GetRxMsgFunctionParams(data)),
			                           dataLen);
			break;
		case COMMAND_SERVER:
			MessageID = GetRxMsgCommID(data);
			DIAGPRINT(">>>>>Command Server: CommID %d\r\n", MessageID);
			if(MessageID>= _FirstCommandID && MessageID <= _LastCommandID)
			{
				DIAGPRINT(">>>>>Command Server: It's my command, I will process!\r\n");
				diagCommandServer( MessageID,
					(PackedUnPackedData *)(GetRxMsgFunctionParams(data)),
			                   dataLen);
			}
			else
			{
				DIAGPRINT(">>>>>Command Server: It's not my command, Ignore!\r\n");
				if(diag_server)
				{
					int i = 0;
					for(i =0; i<MAX_DIAG_CLIENT_NUM;i++)
					{
						if(diag_clients[i].connected)
						{
							if(MessageID >= diag_clients[i].firstCommandID && MessageID <= diag_clients[i].lastCommandID)
							{
								DIAGPRINT(">>>>>Command Server: transfert to %s!\r\n",diag_clients[i].client_addr.sun_path);
								SendCMIMsg(Ori_data, dataExtIf.RxPacket->total_bytes_received,(void *)& diag_clients[i].client_addr);
							}
						}
					}
				}
			}
			break;
		case FIXUPS_COMMAND_SERVER:
			MessageID = GetRxMsgCommID(data);
			DIAGPRINT(">>>>>Fixups Command Server: CommID %d\r\n", MessageID);
			break;
		}
		//if (diagIntData.m_eConnectionType != tUSBConnection)
		{
			bNotifyCP = IsICATReadyNotify_UM(data);
		}
	}

	if (!(Sap & 0x80) || bNotifyCP)  // for CP
	{
//#ifdef UART_NON_RAW_DIAG
		if (diag_target.raw_diag_support == 0)
		{
			//if (dataExtIf.RxPacket->data_offset == 0)
			{
				ori_len = dataExtIf.RxPacket->total_bytes_received- dataExtIf.RxPacket->data_offset;
				new_len = sizeof(UINT32) + ori_len; // Add length header

				new_buff = (UINT8 *)malloc(new_len);
				if (new_buff == NULL)
					return;

				if ((UINT32)new_buff % 4) // address of lenght (4 bytes) is not aligned. Rearrange data
				{
					UINT8 *lptr = (UINT8 *)new_buff;

					(*lptr)   = new_len & 0xff;
					*(lptr+1) = (new_len >> 8) & 0xff;
					*(lptr+2) = (new_len >> 16) & 0xff;
					*(lptr+3) = (new_len >> 24) & 0xff;
				}
				else
				{
					*((UINT32 *)new_buff) = new_len;
				}

				memcpy(new_buff+sizeof(UINT32), data, ori_len);
				for (i = 0; i < new_len; i += STRIDE_LEN)
				{
					if ((UINT32)(i + STRIDE_LEN) <= new_len)
						diagSendData2CP(new_buff + i, STRIDE_LEN);
					else
						diagSendData2CP(new_buff + i, new_len % STRIDE_LEN);
				}
				free(new_buff);
			}
		}
		else //USB
//#endif
		{
			DIAGPRINT("*****dispatchServiceForSAP sent data to CP***\n");
			new_len = dataExtIf.RxPacket->total_bytes_received;
			new_buff = Ori_data;
			for(i = 0; i < new_len; i+=STRIDE_LEN)
			{
				if((UINT32)(i+STRIDE_LEN) <= new_len)
					diagSendData2CP(new_buff + i, STRIDE_LEN);
				else
					diagSendData2CP(new_buff + i, new_len % STRIDE_LEN);
			}
		}
	}

#else   // DIAG NEW HEADER
	switch (Sap & 0x7f)
	{
	case SAP_INTERNAL_SERVICE_REQUEST:
		DIAGPRINT(">>>>>New Internal Service\n");
		break;
	case SAP_COMMAND_SERVICE_REQUEST:
		DIAGPRINT(">>>>>New Command Server\n");
		break;
	case SAP_FIXUP_COMMAND_SERVICE_REQUEST:
		DIAGPRINT(">>>>>New Fixup Command Server\n");
		break;

	}
#endif
}

int DiagIsUSBCableConnected()
{
	FILE *fp_conn = NULL, *fp_comp = NULL;
	char connect = 0, composite_cfg[50];

	fp_conn = fopen(USB_DRV_CONNECTED, "rb");
	fp_comp = fopen(USB_DRV_COMPOSITE, "rb");

	if (fp_conn && fp_comp &&
	    (fread(&connect,1,1,fp_conn) == 1)  && (connect == (PXA_GADGET_CABLE_CONNECTED)) &&
	    (fread(&composite_cfg, 1, 40, fp_comp) && strstr(composite_cfg, DIAG_COMPO_KEY)))
	{
		fclose(fp_conn);
		fclose(fp_comp);
		DIAGPRINT(" ++++ Diag USB connected \n");
		return 1;    //connected
	}

	if(fp_conn)
	{
		fclose(fp_conn);
		fp_conn = NULL;
	}

	if(fp_comp)
	{
		fclose(fp_comp);
		fp_comp = NULL;
	}

	DIAGPRINT(" ---- Diag USB not connected \n");
	return 0;

}

// This fucntion is for kernel 3.0
int DiagIsInUSBComposite()
{
	FILE *fp_comp = NULL;
	char composite_cfg[50];

	fp_comp = fopen(USB_ANDROID_COMPOSITE, "rb");

	if (fp_comp &&
	    (fread(&composite_cfg, 1, 40, fp_comp) && strstr(composite_cfg, DIAG__ANDROID_COMPO_KEY)))
	{
		fclose(fp_comp);
		DIAGPRINT(" ++++ Diag USB connected \n");
		return 1;    //connected
	}

	if(fp_comp)
	{
		fclose(fp_comp);
		fp_comp = NULL;
	}

	DIAGPRINT(" ---- Diag USB not connected \n");
	return 0;

}

static void ReceiveDataFromDev(void)
{
	ssize_t dwBytesTransferred = -1;
	unsigned char *transferBuffer;
	unsigned char *tmpBuffer;
	unsigned int received =0;
	unsigned int packetLen = 0;
	fd_set readfds;
	struct timeval tv;
	int fdcount;
	const char *keys;
	const char *subsys;
	const char *driver;
	const char *action;
	const char *usb_state;
	int UsbStatus = -1; // -1: invalid, 0: usb out, 1: usb in
	int DiagInComposite = 0; // 0: diag not in usb composite, 1: diag is in composite

	transferBuffer = malloc(ICAT_DATA_LENGTH*4*sizeof(CHAR));
	tmpBuffer = malloc(ICAT_DATA_LENGTH*4*sizeof(CHAR));
	while (1)
	{
		FD_ZERO(&readfds);

		if (hPort >= 0)
		{
			if (diagIntData.m_eConnectionType == tUSBConnection)
			{
				/* load the buffer received before diag init */
				ioctl(hPort, IO_FLUSH_PENDING_BUFFERS, 0);
			}
			FD_SET(hPort, &readfds);
		}

		if (diagNlFd > 0)
		{
			FD_SET(diagNlFd, &readfds);
		}

		tv.tv_sec = 1; //timeout is needed to allow FD_SET updates
		tv.tv_usec = 0;
		fdcount = select(MAX(hPort, diagNlFd) + 1, &readfds, NULL, NULL, &tv);
		if (fdcount < 0)
		{
			DIAGPRINT("select diagNlFd error\n");
			continue;
		}
		else if (fdcount == 0)
		{
			//		printf("select diagNlFd timeout\n");
			continue;
		}
		else
		{
			if (diagNlFd > 0 && FD_ISSET(diagNlFd, &readfds))
			{
				DIAGPRINT("got Netlink Data\n");
				dwBytesTransferred = recv(diagNlFd, transferBuffer, DIAG_COM_USB_MAX_PACKET_BYTE_SIZE, 0);
				if (dwBytesTransferred <= 0)
					continue;
				/* search udev netlink keys from env */
				keys = (char *)(transferBuffer + strlen((char *)transferBuffer) + 1);
				subsys = searchNetlink("SUBSYSTEM", keys, dwBytesTransferred);
				driver = searchNetlink("DRIVER", keys, dwBytesTransferred);
				action = searchNetlink("ACTION", keys, dwBytesTransferred);
				usb_state = searchNetlink("USB_STATE", keys, dwBytesTransferred);

				UsbStatus = -1;
				DiagInComposite = 0;

				if (subsys && driver && action) // for kernel .35 or before
				{
					if ((!strcmp(subsys, "platform")) &&
						!(strcmp(driver, "pxa27x-udc") /* Tavor P USB 1.1 */
						 && strcmp(driver, "pxa3xx-u2d") /* Tavor P USB 2.0 */
						 && strcmp(driver, "pxa-u2o")
						 && strcmp(driver, "pxa9xx-u2o"))) /* Tavor PV USB 2.0 */
					{
						if (!strcmp(action, "add"))
						{
							UsbStatus = 1;
							DiagInComposite = DiagIsUSBCableConnected();
						}
						else if (!strcmp(action, "remove"))
						{
							UsbStatus = 0;
						}
					}
					else
					{
						UsbStatus = -1;
					}
				} else if (subsys && usb_state && action) // for kernel 3.0
				{
					if ((!strcmp(subsys, "android_usb")) &&
						!(strcmp(action, "change")))
					{
						if (!strcmp(usb_state, "CONFIGURED"))
						{
							UsbStatus = 1;
							DiagInComposite = DiagIsInUSBComposite();
						}
						else if (!strcmp(usb_state, "DISCONNECTED"))
						{
							UsbStatus = 0;
						}
					}
					else
					{
						UsbStatus = -1;
					}

				}
				else
				{
					/* wrong event */
					continue;
				}

				/* plug in or out? */
				if (UsbStatus == 1)
				{
					DIAGPRINT("DIAG RX Netlink add \r\n");
					/* FIXME: detect /dev/ttygserial to avoid endless loop */
					if (access(DIAG_TTY_USB_DEVICE, F_OK) < 0) //existence test
					{
						DIAGPRINT("DIAG over FS: TTY device error\r\n");
					}

					if (hPort > 0)
					{
						ioctl(hPort, DIAGSTUB_USBREMOVE, 0);
						close(hPort);
						hPort = -1;
					}

					if (DiagInComposite)
					{
						mydiagstubfd = open(DIAGSTUB_USB_DEVICE,O_RDWR);
						if (mydiagstubfd < 0)
						{
							DIAGPRINT(">>%s,Id:%d\n",DIAGSTUB_USB_DEVICE,mydiagstubfd);
							return;
						}
						if (mydiagstubfd > 0)
							ioctl(mydiagstubfd, DIAGSTUB_USBINIT, 0);

						close(mydiagstubfd);

						initUsbConnection();
						if (hPort > 0)
						{
							ioctl(hPort, DIAGSTUB_USBADD, 0);
						}
					}
					else
					{
						DIAGPRINT("Diag is not in USB composite, Do Nothing\r\n");
					}
				}
				else if (UsbStatus == 0)
				{
					DIAGPRINT("DIAG RX Netlink remove\r\n");
					if (hPort > 0)
						ioctl(hPort, DIAGSTUB_USBREMOVE, 0);
					close(hPort);
					hPort = -1;
				}

			}
			/* is usb gadget serial data? */
			if (hPort > 0 && FD_ISSET(hPort, &readfds))
			{
				if ((dwBytesTransferred = read(hPort, transferBuffer + received, ICAT_DATA_LENGTH )) >= 0)
				{
					if (errno == EIO)
					{
						// when do USB dynamic switch, diag tty port could be closed by kernel
						DIAGPRINT("Close diag port when %s\n",strerror(errno));
						ioctl(hPort, DIAGSTUB_USBREMOVE, 0);
						close(hPort);
						hPort = -1;
					}

				DIAGPRINT("DIAG received from hPort:%x , received:%d  len:%d\n", hPort, received, dwBytesTransferred);

GetNewData:		if(packetLen ==0) // need to get length first!
					{
						if(dwBytesTransferred+received < sizeof(UINT32)) //not enough for read a UINT32!
						{
							received += dwBytesTransferred;
							continue;
						}
						else //enough to get a Length!
						{
							packetLen = GetUsbMsgLen(transferBuffer);
							goto GetNewData; // continue to process this new data!
						}
					}
					else
					{
							received += dwBytesTransferred;

							if(received >= packetLen) // We received next packet!
							{
								memcpy(tmpBuffer, transferBuffer+packetLen, received - packetLen);
								diagCommSetChunk(dataExtIf.RxPacket, (CHAR *)transferBuffer, (UINT32)packetLen, 1/*TBDIY DIAGNEWHEADER*/);
								dispatchServiceForSAP(transferBuffer);

								memcpy(transferBuffer, tmpBuffer, received - packetLen); // just like we get a new data!
								dwBytesTransferred = received - packetLen;
								received = 0;
								packetLen = 0;

								goto GetNewData; // continue to process this new data!
							}
							else
								continue;
					}
				}
				else
				{
					//Handle this error
					DIAGPRINT("***** DIAG EXT COMM RX ReadFile failed:%d,%s. *****\r\n", dwBytesTransferred,strerror(errno));
					sleep(3); //Sleep some time to allow the usb-disconnection event to arrive properly
				}
			}
			else
			{
				DIAGPRINT("****GO TO this wrong path** port:%d*\r\n", hPort);
			}
		}
	}

}

void initUsbConnection()
{
	struct termios tio, oldtio;

	hPort = open(DIAG_TTY_USB_DEVICE, O_RDWR | O_NOCTTY | O_NDELAY);
	if (hPort == -1)
	{
		DIAGPRINT("failed to init USB connection\n");
		return;
	}

	fcntl(hPort, F_SETFL, 0 /*FASYNC*/);

	tcgetattr(hPort, &oldtio); // save current port settings
	DIAGPRINT(">>>hPort C_iflag %x\r\n", oldtio.c_iflag);

	memset(&tio, 0x0, sizeof(struct termios));
	/* Configure termios */
	tio.c_cflag = BAUD | CRTSCTS | CS8 | CLOCAL | CREAD;
	tio.c_iflag = IGNPAR;
	tio.c_oflag = 0;
	tio.c_lflag = 0;
	tio.c_cc[VINTR]    = 0; /* Ctrl-c */
	tio.c_cc[VQUIT]    = 0; /* Ctrl-\ */
	tio.c_cc[VKILL]    = 0; /* @ */
	tio.c_cc[VEOF]     = 0; //4;     /* Ctrl-d */
	tio.c_cc[VTIME]    = 0; /* inter-character timer unused */
	tio.c_cc[VMIN]     = 1; /* blocking read until 1 character arrives */
	tio.c_cc[VSWTC]    = 0; /* '\0' */
	tio.c_cc[VSTART]   = 0; /* Ctrl-q */
	tio.c_cc[VSTOP]    = 0; /* Ctrl-s */
	tio.c_cc[VSUSP]    = 0; /* Ctrl-z */
	tio.c_cc[VREPRINT] = 0; /* Ctrl-r */
	tio.c_cc[VDISCARD] = 0; /* Ctrl-u */
	tio.c_cc[VWERASE]  = 0; /* Ctrl-w */
	tio.c_cc[VLNEXT]   = 0; /* Ctrl-v */
	tio.c_cc[VEOL2]    = 0; /* '\0' */
	tio.c_cc[VERASE] = 0x8; /* del */
	tio.c_cc[VEOL]   = 0xD; /* '\0' */

	tcflush(hPort, TCIFLUSH);
	tcsetattr(hPort, TCSANOW, &tio);

	DIAGPRINT( "DIAG: Initialized USB connection %s.\r\n", DIAG_TTY_USB_DEVICE);
}
void initCittyConnection()
{
	struct termios oldtio, newtio;       //place for old and new port settings for serial port
	memset(&newtio, 0x0, sizeof(newtio));
	/*open the device(com port) to be non-blocking (read will return immediately) */
	serialfd = open(SERIAL_PORT8_NAME, O_RDWR); //| O_NOCTTY

	if (serialfd <= 0)
	{
		printf("Error opening device %s\n", SERIAL_PORT8_NAME);
		return;
	}

	// Make the file descriptor asynchronous (the manual page says only
	// O_APPEND and O_NONBLOCK, will work with F_SETFL...)

	fcntl(serialfd, F_SETFL, 0);

	tcgetattr(serialfd, &oldtio); // save current port settings

	// set new port settings for canonical input processing
	newtio.c_cflag = BAUD | CRTSCTS | DATABITS | STOPBITS | PARITYON | PARITY | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;
	///newtio.c_lflag =  ECHOE | ECHO | ICANON;       //ICANON;
	newtio.c_lflag =  0;       //ICANON;

	newtio.c_cc[VMIN] = 1;
	newtio.c_cc[VTIME] = 0;
	newtio.c_cc[VERASE] = 0x8;
	newtio.c_cc[VEOL] = 0xD;

	newtio.c_cc[VINTR]    = 0;      /* Ctrl-c */
	newtio.c_cc[VQUIT]    = 0;      /* Ctrl-\ */
	newtio.c_cc[VERASE]   = 0;      /* del */
	newtio.c_cc[VKILL]    = 0;      /* @ */
	newtio.c_cc[VEOF]     = 4;      /* Ctrl-d */
	newtio.c_cc[VTIME]    = 0;      /* inter-character timer unused */
	newtio.c_cc[VMIN]     = 1;      /* blocking read until 1 character arrives */
	newtio.c_cc[VSWTC]    = 0;      /* '\0' */
	newtio.c_cc[VSTART]   = 0;      /* Ctrl-q */
	newtio.c_cc[VSTOP]    = 0;      /* Ctrl-s */
	newtio.c_cc[VSUSP]    = 0;      /* Ctrl-z */
	newtio.c_cc[VEOL]     = 0;      /* '\0' */
	newtio.c_cc[VREPRINT] = 0;      /* Ctrl-r */
	newtio.c_cc[VDISCARD] = 0;      /* Ctrl-u */
	newtio.c_cc[VWERASE]  = 0;      /* Ctrl-w */
	newtio.c_cc[VLNEXT]   = 0;      /* Ctrl-v */
	newtio.c_cc[VEOL2]    = 0;      /* '\0' */

	newtio.c_cc[VMIN] = 1;
	newtio.c_cc[VTIME] = 0;
	newtio.c_cc[VERASE] = 0x8;
	newtio.c_cc[VEOL] = 0xD;



	tcflush(serialfd, TCIFLUSH);
	tcsetattr(serialfd, TCSANOW, &newtio);

}
void DiagCommExtIfInit(void)
{
	extern OSSemaRef send_comm_msg_sem;
	extern OSSemaRef connect_fs_sem;
	OSA_STATUS status;
	DIAGPRINT(">>>enter DiagCommExtIfInit\n");
	dataExtIf.RxPacket = malloc(sizeof(DIAG_COM_RX_Packet_Info));

	dataExtIf.RxPacket->handling_state             = DIAG_COM_CompletePacketState;
	dataExtIf.RxPacket->current_chunk_bytes        = 0;
	dataExtIf.RxPacket->data_offset                = 0; //init
	dataExtIf.RxPacket->end_of_packet              = FALSE;
	dataExtIf.RxPacket->total_bytes_expected       = 0;
	dataExtIf.RxPacket->total_bytes_received       = 0;
	dataExtIf.RxPacket->rxIF                       = DIAG_COMMDEV_EXT;
	dataExtIf.bIsIfBusy                            = FALSE;

	// RESTRICT MESSAGES LENGHT.
	_diagInternalData.MsgLimits.diagMaxMsgOutLimit          = 7400; //8000;
	_diagInternalData.MsgLimits.diagMaxMsgOutWarning        = 7200; //5000;
	_diagInternalData.MsgLimits.diagMaxMsgInLimit           = 9400; //12000;
	_diagInternalData.MsgLimits.diagMaxMsgInWarning         = 9200; //5000;

	//Init DIAG configuration
	diagOSspecificPhase1Init();
	DIAGPRINT(">>>diagOSspecificPhase1Init Finished!\n");

	DIAGPRINT(">>>OSASemaphoreCreate: %x, sem :%d !\n", OSASemaphoreCreate, send_comm_msg_sem);

	status = OSASemaphoreCreate(&send_comm_msg_sem, 1, OSA_FIFO);

	if ( status != OS_SUCCESS )
	{
		DIAGPRINT("DiagCommExtIfInit - can't create semaphore!\n");
	}
	status = OSASemaphoreCreate(&connect_fs_sem, 1, OSA_FIFO);
	if ( status != OS_SUCCESS )
	{
		DIAGPRINT("DiagCommExtIfInit - can't create semaphore!\n");
	}

}

void diagUsbConnectNotify()
{
	UINT32 dataId = DiagExtIfConnectedMsg;
	int retrycunt = 3;

	//bDiagHostUsbConn = TRUE;
	bDiagDiscAcked = FALSE;
	/* If we have alreadly received Conn Ack, we can ignore it */
	if (bDiagConnAcked)
		return;

	while (retrycunt > 0)
	{
		if (!bDiagConnAcked)
		{
			diagSendData2CP((unsigned char *)&dataId, sizeof(UINT32));
			retrycunt--;
			sleep(1);
		}
		else
		{
			break;
		}
	}
}

void diagUsbDisConnectNotify()
{
	UINT32 dataId = DiagExtIfDisconnectedMsg;
	int retrycunt = 3;
	bDiagConnAcked = FALSE;
	if(bDiagDiscAcked)
		return;
	while(retrycunt)
	{
		if (!bDiagDiscAcked)
		{
			diagSendData2CP((unsigned char *)&dataId, sizeof(UINT32));
			retrycunt--;
			sleep(1);
		}
		else
		{
			break;
		}
	}
}

void diagSCInitTask(void *data)
{
	UNUSEDPARAM(data)

	UINT32 startId = DiagDataRequestStartMsg;

	while (1)
	{
		if (!bDiagChannelInited)
		{
			diagSendData2CP((unsigned char *)&startId, sizeof(UINT32));
			DIAGPRINT(">>>diagSCInitTask , DiagDataRequestStartMsg\n");
			usleep(100*1000);
		}
		else
		{
			//diagUsbConnectNotifyForSD();
			//DIAGPRINT(">>>diagSCInitTask , diagUsbConnectNotifyForSD\n");
			break;
		}

	}
}

void diagInitTask(void *data)
{
	UNUSEDPARAM(data)

	UINT32 startId = DiagDataRequestStartMsg;

	while (1)
	{
		if (!bDiagChannelInited)
		{
			diagSendData2CP((unsigned char *)&startId, sizeof(UINT32));
			DIAGPRINT(">>>diagInitTask , DiagDataRequestStartMsg\n");
			usleep(100*1000);
		}
		else
		{
			diagUsbConnectNotify();
			DIAGPRINT(">>>diagInitTask , diagUsbConnectNotify\n");
			break;
		}

	}
}

/***********************************************************************
* Function: checkIP                                                    *
************************************************************************
* Description:                                                         *
* Parameters:                                                          *
*                                                                      *
* Return value: 0=valid IP, -1=invalid IP                              *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
static int checkIP(unsigned long ip)
{
	int fd, num, ret, i;
	struct ifconf ifc;
	struct ifreq *ifr;

	ret = -1;

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(fd < 0)
		return -1;

	num = 10;
	ifc.ifc_buf = NULL;
	for (;;) {
		ifc.ifc_len = num * (int)sizeof(struct ifreq);
		ifc.ifc_buf = realloc(ifc.ifc_buf, ifc.ifc_len);

		if (ioctl(fd, SIOCGIFCONF, &ifc) < 0) {
			perror("SIOCGIFCONF");
			goto out;
		}
		if (ifc.ifc_len == num * (int)sizeof(struct ifreq)) {
			/* assume it overflowed and try again */
			num += 10;
			continue;
		}
		break;
	}

	num = ifc.ifc_len / sizeof(struct ifreq);

	for (i = 0, ifr = ifc.ifc_req; i < num; i++, ifr++) {
		if(ip == ((struct sockaddr_in *)&(ifr->ifr_addr))->sin_addr.s_addr) {
			ret = 0;
			break;
		}
	}

out:
	free(ifc.ifc_buf);
	close(fd);
	return ret;
}

void DiagCommExtIfLocalRxTask(void *ptr)
{
	CHAR*	TransferBuffer;
	long	dwBytesTransferred=0;
	UINT32	socError=0;
	unsigned char *tmpBuffer;
	unsigned int received =0;
	unsigned int packetLen = 0;

	fd_set readfds;
	UNUSEDPARAM(ptr)

	DIAGPRINT("***** DIAG DiagCommExtIfUDPRxTask m_bIsClient=%d _FirstCommandID==%d *****\r\n",diagIntData.m_bIsClient,_FirstCommandID);
	TransferBuffer = DiagAlignMalloc(ICAT_DATA_LENGTH*4*sizeof(CHAR)); //it's *4 also in:
	tmpBuffer = DiagAlignMalloc(ICAT_DATA_LENGTH*4*sizeof(CHAR));

	if (!TransferBuffer)
	{
		//handle this error
		DIAGPRINT("***** DIAG UDP RX TransferBuffer is NULL. *****\r\n");
		DIAG_ASSERT(0);
	}
	while(1)
	{
		int	iSocketLastError,n;
		FD_ZERO(&readfds);
		FD_SET(diag_target.hPort, &readfds);

		if(diag_event.event_socket >0)
			FD_SET(diag_event.event_socket, &readfds);

		DIAGPRINT("***** Waiting for local data coming in :%d, %d!!!\n",diag_target.hPort, diag_event.event_socket);
		n = select( MAX(diag_target.hPort, diag_event.event_socket)+1, &readfds, NULL, NULL, NULL);
		DIAGPRINT("***** Waited for local data coming in :%d, %d!!!\n",diag_target.hPort, diag_event.event_socket);
		if((diag_target.hPort != -1)&&FD_ISSET(diag_target.hPort, &readfds))
		{
			dwBytesTransferred = recvfrom(diag_target.hPort, (char *)TransferBuffer+received, ICAT_DATA_LENGTH, 0, NULL, NULL);
			if(dwBytesTransferred <= 0)
			{
				if (dwBytesTransferred < 0)
				{
					DIAGPRINT("***** DIAG EXT recvfrom error dwBytesTransferred < 0 *****\r\n");
					socError=1;
					sleep(1);
					continue;
				}
			}
			else
			{
				socError=0;
				DIAGPRINT("testtestsststsstststst %d\n",dwBytesTransferred);
#if !defined (DIAGNEWHEADER)
				if (diag_target.raw_diag_support == 0)
				{
					diagCommL2ReceiveL1Packet((UINT8 *)TransferBuffer, (UINT32)dwBytesTransferred, FALSE,DIAG_COMMDEV_EXT);
				} else //TBDIY tricky! DiagCommExtIfUDPRxTask expects USB format for CMI clients
#endif
				{
GetNewData:		if(packetLen ==0) // need to get length first!
					{
						if(dwBytesTransferred+received < sizeof(UINT32)) //not enough for read a UINT32!
						{
							received += dwBytesTransferred;
							continue;
						}
						else //enough to get a Length!
						{
							packetLen = GetUsbMsgLen(TransferBuffer);
							goto GetNewData; // continue to process this new data!
						}
					}
					else
					{
							received += dwBytesTransferred;

							if(received >= packetLen) // We received next packet!
							{
								memcpy(tmpBuffer, TransferBuffer+packetLen, received - packetLen);
								diagCommSetChunk(dataExtIf.RxPacket, TransferBuffer, (UINT32)packetLen, 1/*TBDIY DIAGNEWHEADER*/);
								dispatchServiceForSAP((UINT8 *)TransferBuffer);

								memcpy(TransferBuffer, tmpBuffer, received - packetLen); // just like we get a new data!
								dwBytesTransferred = received - packetLen;
								received = 0;
								packetLen = 0;

								goto GetNewData; // continue to process this new data!
							}
							else
								continue;
					}
				}
			}
		}
		else if(diag_target.hPort == -1)
		{
			DIAGPRINT("***** DIAG EXT recvfrom m_EXTsock==-1 *****\r\n");
			sleep(1);
			//break;
		}
		//////////////////////////////////////////////
		// Dynamic IP change
		//////////////////////////////////////////////
		if(diag_event.event_socket != -1 && FD_ISSET(diag_event.event_socket, &readfds))
		{
			DIAGPRINT("*****DIAG EXT get diag event!\n");// reserved for future use.
		}
		if(n==-1)
		{
			iSocketLastError = errno;
			DIAGPRINT("***** DIAG EXT select failed iSocketLastError = %d *****\r\n",iSocketLastError);
			DIAGPRINT("Error: %s\n",strerror(iSocketLastError));
			socError=1;
			continue;
		}
	}
}
/***********************************************************************
* Function: DiagCommExtIfUDPRxTask                                     *
************************************************************************
* Description: Task for UDP DIAG messages recieve                      *
*                                                                      *
* Parameters:                                                          *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
void DiagCommExtIfUDPRxTask(void *ptr)
{
	CHAR*	TransferBuffer;
	long	dwBytesTransferred=0;
	UINT32	socError=0;
	unsigned char *tmpBuffer;
	unsigned int received =0;
	unsigned int packetLen = 0;

	fd_set readfds;
	UNUSEDPARAM(ptr)

	DIAGPRINT("***** DIAG DiagCommExtIfUDPRxTask m_bIsClient=%d _FirstCommandID==%d *****\r\n",diagIntData.m_bIsClient,_FirstCommandID);
	TransferBuffer = DiagAlignMalloc(ICAT_DATA_LENGTH*4*sizeof(CHAR)); //it's *4 also in:
	if (!TransferBuffer)
	{
		//handle this error
		DIAGPRINT("***** DIAG UDP RX TransferBuffer is NULL. *****\r\n");
		DIAG_ASSERT(0);
	}
	tmpBuffer = DiagAlignMalloc(ICAT_DATA_LENGTH*4*sizeof(CHAR));
	if (!tmpBuffer)
	{
		//handle this error
		DIAGPRINT("***** DIAG UDP RX TransferBuffer is NULL. *****\r\n");
		DiagAlignFree(TransferBuffer);
		DIAG_ASSERT(0);
	}
	while(1)
	{
		int	iSocketLastError,n;
		FD_ZERO(&readfds);
		FD_SET(diag_target.hPort, &readfds);

		if(diag_event.event_socket >0)
			FD_SET(diag_event.event_socket, &readfds);

		DIAGPRINT("***** Waiting for UDP data coming in :%d, %d!!!\n",diag_target.hPort, diag_event.event_socket);
		n = select( MAX(diag_target.hPort, diag_event.event_socket)+1, &readfds, NULL, NULL, NULL);
		DIAGPRINT("***** Waited for UDP data coming in :%d, %d!!!\n",diag_target.hPort, diag_event.event_socket);
		if((diag_target.hPort != -1)&&FD_ISSET(diag_target.hPort, &readfds))
		{
			dwBytesTransferred = recvfrom(diag_target.hPort, (char *)TransferBuffer+received, ICAT_DATA_LENGTH, 0, NULL, NULL);
			if(dwBytesTransferred <= 0)
			{
				if (dwBytesTransferred < 0)
				{
					DIAGPRINT("***** DIAG EXT recvfrom error dwBytesTransferred < 0 *****\r\n");
					socError=1;
					sleep(1);
					continue;
				}
			}
			else
			{
				socError=0;
				DIAGPRINT("testtestsststsstststst %d\n",dwBytesTransferred);
#if !defined (DIAGNEWHEADER)
				if (diag_target.raw_diag_support == 0)
				{
					diagCommL2ReceiveL1Packet((UINT8 *)TransferBuffer, (UINT32)dwBytesTransferred, FALSE,DIAG_COMMDEV_EXT);
				} else //TBDIY tricky! DiagCommExtIfUDPRxTask expects USB format for CMI clients
#endif
				{
GetNewData:		if(packetLen ==0) // need to get length first!
					{
						if(dwBytesTransferred+received < sizeof(UINT32)) //not enough for read a UINT32!
						{
							received += dwBytesTransferred;
							continue;
						}
						else //enough to get a Length!
						{
							packetLen = GetUsbMsgLen(TransferBuffer);
							goto GetNewData; // continue to process this new data!
						}
					}
					else
					{
							received += dwBytesTransferred;

							if(received >= packetLen) // We received next packet!
							{
								memcpy(tmpBuffer, TransferBuffer+packetLen, received - packetLen);
								diagCommSetChunk(dataExtIf.RxPacket, TransferBuffer, (UINT32)packetLen, 1/*TBDIY DIAGNEWHEADER*/);
								dispatchServiceForSAP((UINT8 *)TransferBuffer);

								memcpy(TransferBuffer, tmpBuffer, received - packetLen); // just like we get a new data!
								dwBytesTransferred = received - packetLen;
								received = 0;
								packetLen = 0;

								goto GetNewData; // continue to process this new data!
							}
							else
								continue;
					}
				}
			}
		}
		else if(diag_target.hPort == -1)
		{
			DIAGPRINT("***** DIAG EXT recvfrom m_EXTsock==-1 *****\r\n");
			sleep(1);
			//break;
		}
		//////////////////////////////////////////////
		// Dynamic IP change
		//////////////////////////////////////////////
		if(diag_event.event_socket != -1 && FD_ISSET(diag_event.event_socket, &readfds))
		{
			if((diagIntData.m_bIsClient==0)&&(checkIP(m_lTavorAddress)<0))
			{
				DIAGPRINT("***** DIAG Dynamic IP address change detected *****\r\n");
				diagExtIFStatusDiscNotify();
				break;
			}
		}
		if(n==-1)
		{
			iSocketLastError = errno;
			DIAGPRINT("***** DIAG EXT select failed iSocketLastError = %d *****\r\n",iSocketLastError);
			DIAGPRINT("Error: %s\n",strerror(iSocketLastError));
			socError=1;
			continue;
		}
	}
	DiagAlignFree(TransferBuffer);
	DiagAlignFree(tmpBuffer);
}

void DiagCommExtIfTCPRxTask(void *ptr)
{
	CHAR*	TransferBuffer;
	long	dwBytesTransferred=0;
	UINT32	socError=0;
	unsigned char *tmpBuffer;
	unsigned int received =0;
	unsigned int packetLen = 0;

	fd_set readfds;
	UNUSEDPARAM(ptr)
	TransferBuffer = DiagAlignMalloc(ICAT_DATA_LENGTH*4*sizeof(CHAR)); //it's *4 also in:
	if (!TransferBuffer)
	{
		//handle this error
		DIAGPRINT("***** DIAG TCP RX TransferBuffer is NULL. *****\r\n");
		DIAG_ASSERT(0);
	}
	tmpBuffer = DiagAlignMalloc(ICAT_DATA_LENGTH*4*sizeof(CHAR));
	if(!tmpBuffer)
	{
		//handle this error
		DIAGPRINT("***** DIAG TCP RX TransferBuffer is NULL. *****\r\n");
		DiagAlignFree(TransferBuffer);
		DIAG_ASSERT(0);
	}
	while(1)
	{
		int	iSocketLastError,n;
		FD_ZERO(&readfds);
		if(diag_target.hPort >0)
			FD_SET(diag_target.hPort, &readfds);

		if(diag_event.event_socket >0)
			FD_SET(diag_event.event_socket, &readfds);

		if (diag_event.pipefd[0] >0)
		{//Very tricky!!! Clients are always in UDP mode
			FD_SET(diag_event.pipefd[0],&readfds);
		}

		DIAGPRINT("***** Waiting for TCP data coming in :hPort:%d, event_socket:%d, event_pipe:%d!!!\n",diag_target.hPort, diag_event.event_socket, diag_event.pipefd[0]);
		n = select( MAX(MAX(diag_target.hPort, diag_event.event_socket), diag_event.pipefd[0])+1, &readfds, NULL, NULL, NULL);
		DIAGPRINT("***** Waited for TCP data coming in :hPort:%d, event_socket:%d, event_pipe:%d!!!\n",diag_target.hPort, diag_event.event_socket, diag_event.pipefd[0]);

		if( diag_event.pipefd[0] >0 && FD_ISSET(diag_event.pipefd[0], &readfds))
		{
			char buf[10];
			DIAGPRINT("******TCP RxTask !!!!");
			read(diag_event.pipefd[0], buf, 10);
			DIAGPRINT("******TCP RxTask:%s\n", buf);
		}
		if((diag_target.hPort != -1)&&FD_ISSET(diag_target.hPort, &readfds))
		{
			dwBytesTransferred = recv(diag_target.hPort, (char *)TransferBuffer+received, ICAT_DATA_LENGTH, 0);
			if(dwBytesTransferred <= 0)
			{
				if (dwBytesTransferred < 0)
				{
					DIAGPRINT("***** DIAG EXT recvfrom error dwBytesTransferred < 0 *****\r\n");
					socError=1;
					sleep(1);
					continue;
				}
				else
				{
					DIAGPRINT("*****DIAG EXT Connection is closed by peer:%d\r\n",dwBytesTransferred);
					diagExtIFStatusDiscNotify();
					if (diag_target.hPort < 0)
					{
						if( pthread_create(&diagConnectTcpTask, NULL, (void *)connectTCP, NULL) != 0)
						{
							DIAGPRINT("pthread_create diagConnectTcpTask error\n");
						}
					}
					continue;
				}
			}
			else
			{
#if !defined (DIAGNEWHEADER)
				if (diag_target.raw_diag_support == 0)
				{
					diagCommL2ReceiveL1Packet((UINT8 *)TransferBuffer, (UINT32)dwBytesTransferred, FALSE,DIAG_COMMDEV_EXT);
				} else //TBDIY tricky! DiagCommExtIfUDPRxTask expects USB format for CMI clients
#endif
				{
GetNewData:		if(packetLen ==0) // need to get length first!
					{
						if(dwBytesTransferred+received < sizeof(UINT32)) //not enough for read a UINT32!
						{
							received += dwBytesTransferred;
							continue;
						}
						else //enough to get a Length!
						{
							packetLen = GetUsbMsgLen(TransferBuffer);
							goto GetNewData; // continue to process this new data!
						}
					}
					else
					{
							received += dwBytesTransferred;

							if(received >= packetLen) // We received next packet!
							{
								memcpy(tmpBuffer, TransferBuffer+packetLen, received - packetLen);
								diagCommSetChunk(dataExtIf.RxPacket, TransferBuffer, (UINT32)packetLen, 1/*TBDIY DIAGNEWHEADER*/);
								dispatchServiceForSAP((UINT8 *)TransferBuffer);

								memcpy(TransferBuffer, tmpBuffer, received - packetLen); // just like we get a new data!
								dwBytesTransferred = received - packetLen;
								received = 0;
								packetLen = 0;

								goto GetNewData; // continue to process this new data!
							}
							else
								continue;
					}
				}
			}
		}
		else if(diag_target.hPort == -1)
		{
			DIAGPRINT("***** DIAG EXT recv target.hPort==-1 *****\r\n");
			sleep(1);
		}
		//////////////////////////////////////////////
		// Dynamic IP change
		//////////////////////////////////////////////
		if(diag_event.event_socket != -1 && FD_ISSET(diag_event.event_socket, &readfds))
		{
			processTCPEvent(diag_event.event_socket);
		}
		if(n==-1)
		{
			iSocketLastError = errno;
			DIAGPRINT("***** DIAG EXT select failed iSocketLastError = %d *****\r\n",iSocketLastError);
			DIAGPRINT("Error: %s\n",strerror(iSocketLastError));
			socError=1;
			continue;
		}
	}

}

int ifc_init(void);
void ifc_close(void);
int ifc_up(const char *name);
int ifc_set_addr(const char *name, unsigned addr);
int ifc_set_mask(const char *name, unsigned mask);

void ConfigEthernetInterface(void)
{
	// ifconfig usb0 192.168.1.101 netmask 255.255.255.0 up
	const char *ifname = "usb0";
	// const char *ipaddr = "192.168.1.101";
	const char *maskaddr = "255.255.255.0";

	in_addr_t addr, netmask;

	addr = m_lTavorAddress; //inet_addr(ipaddr);
	netmask = inet_addr(maskaddr);

	ifc_init();
	if(ifc_up(ifname))
	{
		DIAGPRINT("failed to turn on interface");
		ifc_close();
		return ;
	}
	if(ifc_set_addr(ifname, addr))
	{
		DIAGPRINT("failed to set ipaddr");
		ifc_close();
		return ;
	}

	if(ifc_set_mask(ifname, netmask))
	{
		DIAGPRINT("failed to set netmask");
		ifc_close();
		return ;
	}

	ifc_close();
}

int diag_init(void)
{
#define DIAG_INIT_USERMODE 1

	extern BOOL InitLog2SDcard(void);
	extern void connectCOMM(void);
	DIAGPRINT(">>>enter diag_init\n");

	DiagCommExtIfInit();
	DIAGPRINT(">>>DiagCommExtIfInit Finished\n");

	DIAGPRINT("*****DIAG m_eLLtype :%d, m_eConnectionType:%d diag_server : %d\n", diagIntData.m_eLLtype, diagIntData.m_eConnectionType,diag_server);
	diag_target_init(diagIntData.m_eLLtype, diagIntData.m_eConnectionType);
	if(diag_server)
	{
		connectCMI();
		DIAGPRINT("*****DIAG over Local: master connected!\n");
		if( pthread_create( &diagLocalRxTask, NULL, (void *)diagRcvDataFromClientTask, NULL) != 0 )
		{
			DIAGPRINT("pthread_create diagRcvDataFromClientTask error");
			return -1;
		}
#ifdef AT_PRODUCTION_CMNDS
		if(get_bspflag_from_kernel_cmdline() == 2)
			init_diag_prod_conn();
#endif

	}
	if (diagIntData.m_eLLtype == tLL_FS || diagIntData.m_eLLtype == tLL_SC)
	{
		//Log to SD card
#if DIAG_INIT_USERMODE
		mydiagsockfd = msocket(DIAG_PORT);

		if ( pthread_create( &diagSCRxTask, NULL, (void *)diagRcvDataFromCPTask, NULL) != 0 )
		{
			DIAGPRINT("pthread_create diagRcvDataFromCPTask error");
			return -1;
		}
		if ( pthread_create( &diagSCInit, NULL, (void *)diagSCInitTask, NULL) != 0 )
		{
			DIAGPRINT("pthread_create diagRcvDataFromCPTask error");
			return -1;
		}
#else
		//1. Init USB connection
		mydiagstubfd = open(DIAGSTUB_USB_DEVICE, O_RDWR);

		if (mydiagstubfd < 0)
		{

			DIAGPRINT(">>%s,Id:%d\n", DIAGSTUB_USB_DEVICE, mydiagstubfd);
			return -1;
		}
		if (mydiagstubfd > 0)
		{
			ioctl(mydiagstubfd, DIAGSTUB_USBINIT, 0);
		}

		close(mydiagstubfd);

		//2. Receive Data from DIAG PORT
		mydiagsockfd = msocket(DIAG_PORT);

		if ( pthread_create( &diagSCRxTask, NULL, (void *)diagRcvDataFromCPTask, NULL) != 0 )
		{
			DIAGPRINT("pthread_create diagRcvDataFromCPTask error");
			return -1;
		}
#endif
		InitLog2SDcard();
	}
	else  //Log to ACAT via USB/UART
	{
		if (diagIntData.m_eConnectionType == tUARTConnection)
		{	//UART

			//Make sure cached size is set to 0 under UART log mode
			diagIntData.diagCachedWrite.write_unit_size = 0;

#ifdef AT_PRODUCTION_CMNDS
			if(get_bspflag_from_kernel_cmdline() != 2)
#endif
			connectCOMM();

#if DIAG_INIT_USERMODE
			mydiagsockfd = msocket(DIAG_PORT);

			if( pthread_create( &diagSCRxTask, NULL, (void *)diagRcvDataFromCPTask, NULL) != 0 )
			{
				DIAGPRINT("pthread_create diagRcvDataFromCPTask error");
				return -1;
			}
			if( pthread_create( &diagSCInit, NULL, (void *)diagSCInitTask, NULL) != 0 )
			{
				DIAGPRINT("pthread_create diagRcvDataFromCPTask error");
				return -1;
			}
#ifdef AT_PRODUCTION_CMNDS
			if(get_bspflag_from_kernel_cmdline() != 2)
#endif
			diagUsbConnectNotify();
#else

			//1. Data request and connection notify for CP
			mydiagstubfd = open(DIAGSTUB_USB_DEVICE, O_RDWR);

			if (mydiagstubfd < 0)
			{
				DIAGPRINT(">>%s,Id:%d\n", DIAGSTUB_USB_DEVICE, mydiagstubfd);
				return -1;
			}
			if (mydiagstubfd > 0)
			{
				ioctl(mydiagstubfd, DIAGSTUB_USBINIT, 0);
			}

			close(mydiagstubfd);

			//2. Receive Data from DIAG PORT
			mydiagsockfd = msocket(DIAG_PORT);

			if( pthread_create( &diagSCRxTask, NULL, (void *)diagRcvDataFromCPTask, NULL) != 0 )
			{
				DIAGPRINT("pthread_create diagRcvDataFromCPTask error");
				return -1;
			}
#endif
			if ( pthread_create( &diagUsbRxTask, NULL, (void *)ReceiveDataFromDev, NULL) != 0 )
			{
				DIAGPRINT("pthread_create diagUsbRxTask error");
				return -1;
			}

		}
		else if(diagIntData.m_eConnectionType == tUDPConnection || diagIntData.m_eConnectionType == tTCPConnection || diagIntData.m_eConnectionType == tLocalConnection)
		{
			DIAGPRINT("*****DIAG:start connect to udp\n");

			if(diagIntData.m_eLLtype == tLL_ETHERNET)
				ConfigEthernetInterface();

			if (diagIntData.m_eLLtype == tLL_ETHERNET || diagIntData.m_eLLtype == tLL_LocalIP)
			{
				int wait_timeout = 5;

				InitDiagCachedWrite();
				if(diag_server)
				{
					mydiagsockfd = msocket(DIAG_PORT);
					if( pthread_create( &diagSCRxTask, NULL, (void *)diagRcvDataFromCPTask, NULL) != 0 )
					{
						DIAGPRINT("pthread_create diagRcvDataFromCPTask error");
						return -1;
					}
					if( pthread_create( &diagSCInit, NULL, (void *)diagSCInitTask, NULL) != 0 )
					{
						DIAGPRINT("pthread_create diagRcvDataFromCPTask error");
						return -1;
					}

					while((!bDiagChannelInited)&&(wait_timeout))
					{
						sleep(1);
						wait_timeout--;
					}
				}

				DIAGPRINT(">>>diag:diag target:%x; diag_event:%x, diag connect:%x, diag send:%x \n", &diag_target, &diag_event, diag_target.connect, diag_target.send_target);
				if(diag_target.connect != NULL)
					diag_target.connect();
				DIAGPRINT("*****DIAG over Net: target connected!\n");

				if(diag_event.open != NULL)
					diag_event.open();
				DIAGPRINT("*****DIAG over Net: event opened!\n");
				if ( pthread_create( &diagRxTask, NULL, (void *)diag_target.read_target_task, NULL) != 0 )
				{
					DIAGPRINT("pthread_create DiagCommExtIfUDPRxTask error");
					return -1;
				}
			}
		}
		else // USB default
		{
			//Make sure cached size is set to 0 under USB log mode
			diagIntData.diagCachedWrite.write_unit_size = 0;
			mydiagsockfd = msocket(DIAG_PORT);
			if(mydiagsockfd < 0)
			{
				DIAGPRINT("Open Diag Port error\n!");
				return -1;
			}

			mydiagstubfd = open(DIAGSTUB_USB_DEVICE,O_RDWR);

			if (mydiagstubfd < 0)
			{

				DIAGPRINT(">>%s,Id:%d\n",DIAGSTUB_USB_DEVICE,mydiagstubfd);
				return -1;
			}
			if (mydiagstubfd > 0)
				ioctl(mydiagstubfd, DIAGSTUB_USBSTART, 0);

			close(mydiagstubfd);

			//initUsbConnection();

			openNetlink();
			if ( pthread_create( &diagUsbRxTask, NULL, (void *)ReceiveDataFromDev, NULL) != 0 )
			{
				DIAGPRINT("pthread_create diagUsbRxTask error");
				return -1;
			}
		}
	}
	if(diag_server == 0)
	{
		DPRINTF("*****DIAG: client notcie master!\n");
		//DIAG_FILTER(DIAG, CMI, DIAG_TEST, DIAG_CONNECT)
		//diagTextPrintf("Client Notice master!!");
	}
	return 0;
}

void diag_sig_action(int signum, siginfo_t * info, void *p)
{
	UNUSEDPARAM(info)
	UNUSEDPARAM(p)
	DIAGPRINT(">>diag:receive sig:%d\n", signum);
	switch(signum)
	{
	case SIGTERM:
		 diagExtIFStatusDiscNotify();
		break;
	case SIGPIPE:
		diagExtIFStatusDiscNotify();
		DIAGPRINT("Reconnecting TCP\n");
		if( pthread_create(&diagConnectTcpTask, NULL, (void *)connectTCP, NULL) != 0)
		{
			DIAGPRINT("pthread_create diagConnectTcpTask error\n");
		}
		break;
	default:
		break;
	}
}

void openRTMGRP_IPV4_IFADDR()
{
	int fd;
	struct sockaddr_nl snl;

	DIAGPRINT("***** DIAG open RTMGRP_IPV4_IFADDR *****\r\n");
	memset(&snl, 0, sizeof(snl));
	snl.nl_family = AF_NETLINK;
	snl.nl_groups = RTMGRP_IPV4_IFADDR;

	fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
	if(fd < 0)
	{
		DIAGPRINT("***** DIAG openRTMGRP_IPV4_IFADDR: socket NETLINK_ROUTE Error: %s *****\r\n",strerror(errno));
		return;
	}

	if(bind(fd, (struct sockaddr *) &snl, sizeof(struct sockaddr_nl)) < 0)
	{
		DIAGPRINT("***** DIAG openRTMGRP_IPV4_IFADDR: bind NETLINK_ROUTE Error: %s *****\r\n",strerror(errno));
		close(fd);
		return;
	}

	diag_event.event_socket = fd;
}
static int openNetEventlink(void)
{
	struct sockaddr_nl snl;
	int fd = -1;
	memset(&snl, 0, sizeof(snl));
	snl.nl_family = AF_NETLINK;
	snl.nl_pid = getpid();
	snl.nl_groups = 1;

	fd = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
	if (fd  < 0)
	{
		DIAGPRINT("openNetEventlink socket create error.\r\n");
		return fd;
	}

	if (bind(fd, (struct sockaddr *)&snl, sizeof(struct sockaddr_nl)) < 0)
	{
		DIAGPRINT("openNetlink socket bind error.\r\n");
		close(fd);
	}

	diag_event.event_socket = fd;
	return fd;

}

int DiagIsUSBEthernetConnected(void)
{
	FILE *fp_conn = NULL, *fp_comp = NULL;
	char connect = 0, composite_cfg[50];

	fp_conn = fopen(USB_DRV_CONNECTED, "rb");
	fp_comp = fopen(USB_DRV_COMPOSITE, "rb");

	if (fp_conn && fp_comp &&
	    (fread(&connect,1,1,fp_conn) == 1)  && (connect == (PXA_GADGET_CABLE_CONNECTED)) &&
	    (fread(&composite_cfg, 1, 40, fp_comp) && strstr(composite_cfg, ETHERNET_COMPO_KEY)))
	{
		fclose(fp_conn);
		fclose(fp_comp);
		DIAGPRINT(" ++++ Diag USB ethernet connected \n");
		return 1;    //connected
	}

	if(fp_conn)
	{
		fclose(fp_conn);
		fp_conn = NULL;
	}

	if(fp_comp)
	{
		fclose(fp_comp);
		fp_comp = NULL;
	}

	DIAGPRINT(" ---- Diag USB ethernet not connected \n");
	return 0;

}

// for kernel 3.0
int RndisisInUSBComposite(void)
{
	FILE *fp_comp = NULL;
	char composite_cfg[50];

	fp_comp = fopen(USB_ANDROID_COMPOSITE, "rb");

	if (fp_comp &&
	    (fread(&composite_cfg, 1, 40, fp_comp) && strstr(composite_cfg, ETHERNET_COMPO_KEY)))
	{
		fclose(fp_comp);
		DIAGPRINT(" ++++ Diag USB ethernet connected \n");
		return 1;    //connected
	}

	if(fp_comp)
	{
		fclose(fp_comp);
		fp_comp = NULL;
	}

	DIAGPRINT(" ---- Diag USB ethernet not connected \n");
	return 0;

}

static void processTCPEvent(int event_fd)
{
	CHAR	transferBuffer[2048];
	long	dwBytesTransferred=0;
	const char *keys;
	const char *subsys;
	const char *driver;
	const char *action;
	const char *usb_state;
	int UsbStatus = -1; // -1: invalid, 0: usb out, 1: usb in
	int RndisInComposite = 0; // 0: rndis not in usb composite, 1: rndis is in composite

	dwBytesTransferred = recv(event_fd, transferBuffer, 2048, 0);
	if (dwBytesTransferred <= 0)
		return;

	/* search udev netlink keys from env */
	keys = (char *)(transferBuffer + strlen((char *)transferBuffer) + 1);
	DIAGPRINT("got Netlink Data:%s\n",keys);
	subsys = searchNetlink("SUBSYSTEM", keys, dwBytesTransferred);
	driver = searchNetlink("DRIVER", keys, dwBytesTransferred);
	action = searchNetlink("ACTION", keys, dwBytesTransferred);
	usb_state = searchNetlink("USB_STATE", keys, dwBytesTransferred);

	UsbStatus = -1;
	RndisInComposite = 0;

	if (subsys && driver && action) // for kernel .35 or before
	{
		if ((!strcmp(subsys, "platform")) &&
			!(strcmp(driver, "pxa27x-udc") /* Tavor P USB 1.1 */
			 && strcmp(driver, "pxa3xx-u2d") /* Tavor P USB 2.0 */
			 && strcmp(driver, "pxa-u2o")
			 && strcmp(driver, "pxa9xx-u2o"))) /* Tavor PV USB 2.0 */
		{
			if (!strcmp(action, "add"))
			{
				UsbStatus = 1;
				RndisInComposite = DiagIsUSBEthernetConnected();
			}
			else if (!strcmp(action, "remove"))
			{
				UsbStatus = 0;
			}
		}
		else
		{
			UsbStatus = -1;
		}
	} else if (subsys && usb_state && action) // for kernel 3.0
	{
		if ((!strcmp(subsys, "android_usb")) &&
			!(strcmp(action, "change")))
		{
			if (!strcmp(usb_state, "CONFIGURED"))
			{
				UsbStatus = 1;
				RndisInComposite = RndisisInUSBComposite();
			}
			else if (!strcmp(usb_state, "DISCONNECTED"))
			{
				UsbStatus = 0;
			}
		}
		else
		{
			UsbStatus = -1;
		}
	}
	else
	{
		/* wrong event */
		DIAGPRINT("DIAG over TCP: Wrong event\r\n");
	}


	/* plug in or out? */
	if (UsbStatus == 1)
	{
		DIAGPRINT("DIAG RX Netlink add \r\n");
		/* FIXME: detect /dev/ttygserial to avoid endless loop */
		if (access(DIAG_ETHER_USB_DEVICE, F_OK) < 0) //existence test
		{
			DIAGPRINT("DIAG over TCP: USB0 device error\r\n");
		}

		if (RndisInComposite && diag_target.hPort < 0 && diag_target.socket < 0)
		{
			if( pthread_create(&diagConnectTcpTask, NULL, (void *)connectTCP, NULL) != 0)
			{
				DIAGPRINT("pthread_create diagConnectTcpTask error\n");
			}
		}
	}
	else if (UsbStatus == 0)
	{
		DIAGPRINT("DIAG RX Netlink remove\r\n");
		diagExtIFStatusDiscNotify();
	}
	return;
}
/***********************************************************************
* Function: diag_target_init                                    *
************************************************************************
* Description: init target struct accordint to each target. All the targets(ACAT over USB, ACAT ovet UDP,
   SDcard and so on) may use unify interface in the future.                       *
*                                                                      *
* Parameters:                                                          *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
void diag_target_init(EActiveConnectionLL_Type ltype, EActiveConnectionType etype )
{
	int i;
	DIAGPRINT(">>>diag_target_int:diag target:%x; diag_event:%x, diag connect:%x, diag send:%x \n", &diag_target, &diag_event, connectUDP, SendUDPMsg);
	diag_target.hPort = -1;	//hport: the file description or socket of the target.
	diag_target.socket = -1;
	diag_event.event_socket = -1; //event source port.
	diag_target.raw_diag_support = 1;
	diag_server_port = -1;
	if(pipe(diag_event.pipefd) < 0)
	{
		DIAGPRINT("create pipe error\n");
		return;
	}

	DIAGPRINT(">>>>diag server:%d\n", diag_server);
	if(diag_server)
	{
		OSA_MutexCreate(&cmi_state.lock,NULL);
		for(i=0;i<MAX_DIAG_CLIENT_NUM;i++)
		{
			memset(&diag_clients[i], 0, sizeof(diag_clients[i]));
		}
	}
	switch(ltype)
	{
	case tLL_SC:
	case tLL_FS:
	//	diag_target.connect = InitLog2SDcard;
	//	diag_target.send_target = write;
	//	diag_target.read_target_task = NULL;
	//	diag_event.open = openNetlink;
		diagIntData.m_bIsClient = 0;
		break;
	case tLL_ETHERNET:
		switch(etype)
		{
		case tUDPConnection:
			diagIntData.m_bIsClient = 0;
			diag_target.connect = connectUDP;	//connect:the fucntion to connect to the target, get the avalible hPort usually.
			diag_target.read_target_task = DiagCommExtIfUDPRxTask; //read_target_task: the fuction to read data form target and send it to CP
			diag_target.send_target = SendUDPMsg;					//send_target: the function to send data to target.
			diag_target.raw_diag_support = diagIntData.m_bIsRawDiag;
			diag_event.open = NULL;
			 OSA_MutexCreate(&udp_state.lock,NULL);
			//agIntData.diagCachedWrite.write_unit_size = 1024;
			break;
		case tTCPConnection:
			diag_target.connect = connectTCP;
			diag_target.read_target_task = DiagCommExtIfTCPRxTask;
			diag_target.send_target = SendTCPMsg;
			diag_target.raw_diag_support = diagIntData.m_bIsRawDiag;
			diag_event.open = openNetEventlink;
			 OSA_MutexCreate(&tcp_state.lock,NULL);
			//diagIntData.diagCachedWrite.write_unit_size = 1024;
		default :
			break;
		}
		break;
	case tLL_LocalIP:
		diagIntData.m_bIsClient = 1;
		diag_target.connect = connectLocal;
		diag_target.read_target_task = DiagCommExtIfLocalRxTask;
		diag_target.send_target = SendLocalMsg;
		diag_target.raw_diag_support = 1;
		diag_event.open = NULL;
		OSA_MutexCreate(&local_state.lock, NULL);
	default:
		break;
	}
	DIAGPRINT(">>>diag_target_int:diag target:%x; diag_event:%x, diag connect:%x, diag send:%x \n", &diag_target, &diag_event, diag_target.connect, diag_target.send_target);
}

#ifdef AT_PRODUCTION_CMNDS
static PROD_CTRL diag_prod_ctrl;

static void process_diag_buf(const unsigned char *buf, int len)
{
	unsigned long code;
	extern void connectCOMM(void);

	if(len != 4)
	{
		return;
	}

	memcpy(&code, buf, sizeof(code));
	if(code == CODE_SWITCH_TO_DIAG)
	{
		if(is_diag_running == 0)
		{
			DIAGPRINT("%s: open diag", __FUNCTION__);

			connectCOMM();
			diagUsbConnectNotify();
		}
		is_diag_running = 1;
	}
}

static void init_diag_prod_conn(void)
{
	diag_prod_ctrl.fd = -1;
	diag_prod_ctrl.fp = process_diag_buf;

	create_prod_conn_task(&diag_prod_ctrl);
}

//ICAT EXPORTED FUNCTION - SYSTEM,PROD,AT_SER
void AT_SER(void)
{
	extern void disconnectCOMM();
	unsigned long code = CODE_SWITCH_TO_AT_CMD_SRV;

	DIAG_FILTER(SYSTEM, PROD, AT_SER_OK, DIAG_INFO)
	diagPrintf("OK");

	DIAGPRINT("%s: close diag", __FUNCTION__);

	if(get_bspflag_from_kernel_cmdline() != 2)
		return;

	// stop diag
	is_diag_running = 0;
	disconnectCOMM();

	// send message to start at command server
	if(diag_prod_ctrl.fd != -1)
		write(diag_prod_ctrl.fd, &code, sizeof(code));
}

#endif

