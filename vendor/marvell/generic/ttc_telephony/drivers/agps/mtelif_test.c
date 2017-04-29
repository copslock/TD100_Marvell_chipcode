/*------------------------------------------------------------
(C) Copyright [2006-2011] Marvell International Ltd.
All Rights Reserved
------------------------------------------------------------*/


/*
 * mtelif_test.c
 *
 * Use to test telephony interface for application layer
 */

#include <sys/types.h>
#include <sys/socket.h>
#if defined (BIONIC)
	//#include <sys/endian.h>
	#include <linux/in.h>
	#include <linux/ip.h>
	//#include <linux/netdevice.h>
#else
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h> /* close */
#include <pthread.h>
#include <termios.h>
#include <fcntl.h>
#include <stddef.h>
#include <string.h>
//#include "CP2LSM.h"
#include "dlfcn.h"
#include "mtelif_lib.h"

//DECLARE_AGPSMtilLib

extern char *optarg;

static int bTerminate = 0;

/*******************************************************************************\
*	Function:		mySignalHandler
*	Description:	Signal Handler which override the default signal handling.
*					It causes a proper closing of the PDP Context (using a global variable).
*	Parameters:		See below
*	Returns:		None
\*******************************************************************************/
void mySignalHandler(int signum)	/* signal number */
{
	bTerminate = 1;
	printf("[%s]: Received signum %d ... exiting ...\n", __FILE__, signum);
}

/*******************************************************************************\
*	Function:		ReceiveSmsPayload
*	Description:	A callback function that receives an SMS message and prints its conntents.
*	Parameters:		See below
*	Returns:		None
\*******************************************************************************/
void ReceiveSmsPayload(const unsigned char* sms_payload,	/* pointer to SMS payload (user data) (TP_UD) */
					   int dcs,								/* data coding scheme (TP_DCS) */
					   int length)							/* payload (user data) length (in bytes) (TP_UDL) */
{
	int i;

	if (sms_payload == NULL)
	{
		printf("[%s]: Error! Null pointer", __FILE__);
		return;
	}

	printf("[%s]: %s TP_UDL=%d, TP_DCS=0x%x, TP_UD= ", __FILE__, __FUNCTION__, length, dcs);

	for (i = 0; i < length; i++)
	{
		printf("%02x ", sms_payload[i]);
	}

	printf("\r\n");
}

/*******************************************************************************\
*	Function:		main
*	Description:	main routine and entry point of this program
*	Parameters:		See below
*	Returns:		0=OK, <0=Error Code
\*******************************************************************************/
int main(int argc, char *argv[])
{
	int opt;
	char *apn_str;
	AccTechMode acc_tech_mode;
	unsigned char imsi_str[16];
	MCellInfo info;
	AGPS_NITZ_STATUS_MSG NitzStatus;

	//BIND_AGPSMtilLib

	// init signal handling
	signal(SIGINT,  mySignalHandler);	// when CTRL-C is captured on console
	signal(SIGTERM, mySignalHandler);	// when killed from outside
	signal(SIGQUIT, mySignalHandler);	// not really needed
	signal(SIGABRT, mySignalHandler);	// not really needed
	signal(SIGKILL, mySignalHandler);	// not really needed

	if (argc < 2)
	{
		printf("[%s]: usage: %s -a <apn>\n", __FILE__, argv[0]);
		return -1;
	}

	while ( -1 != (opt = getopt(argc, argv, "a:")))
	{
		switch (opt)
		{
			case 'a':
				apn_str = "internet"; //optarg;
				printf("[%s]: apn-str=%s\n", __FILE__, apn_str);
				break;

			default:
// 				printf("[%s]: usage: %s -a <apn> -d <AT device name>\n", __FILE__, argv[0]);
				printf("[%s]: usage: %s -a <apn>\n", __FILE__, argv[0]);
				return -1;
		}
	}

	//mtelif_open(argc, argv);
	MRIL_Client_Init();

/* PLATFORM_SDK_VERSION < 9 means Android version <  2.3 */
#if PLATFORM_SDK_VERSION < 9
	// get International Mobile Subscriber Identity (IMSI)
	GetIMSI(imsi_str);
	printf("[%s]: GetIMSI %s\n", __FILE__, imsi_str);

	// get Cell Info
   	GetCurCellInfo(&info);
   	printf("[%s]: GetCurCellInfo MCC=%d, MNC=%d, LAC=%d, CID=%d\n", __FILE__, info.CountryCode, info.NetworkCode, info.lac, info.cellId);

	// get Access Technology Mode (AcT)
	GetAccTechMode(&acc_tech_mode);
	printf("[%s]: GetAccTechMode %d\n", __FILE__, acc_tech_mode);

	// establish GPRS connection
	if (EstablishPSconnection((unsigned char *) apn_str, 0, 0, 0) == 0)
	{
		printf("[%s]: Error! EstablishGPRSconnection failed\n", __FILE__);
	}

	else
	{
		printf("[%s]: EstablishGPRSconnection OK\n", __FILE__);
	}
	// register for MT SMS callback
	if (RegisterMTSmsPayloadNotification(ReceiveSmsPayload) == 0)
	{
		printf("[%s]: Error! RegisterMTSmsNotification failed\n", __FILE__);
	}

	else
	{
		printf("[%s]: RegisterMTSmsNotification OK\n", __FILE__);
	}
#endif

	UpaClient_GetNitzInfo(&NitzStatus);

	printf("[%s]: %04d/%02d/%02d %02d:%02d:%02d UTC. Local TZ=UTC %02d",
		__FUNCTION__,
		(int) (NitzStatus.year + 2000),
		(int) NitzStatus.month,
		(int) NitzStatus.day,
		(int) NitzStatus.hour,
		(int) NitzStatus.minute,
		(int) NitzStatus.sec,
		(int) (NitzStatus.time_zone / 4));

	// make sure the test program is not killed when MT SMS is in
	while (bTerminate == 0)
	{
		sleep(2);
	}

/* PLATFORM_SDK_VERSION < 9 means Android version <  2.3 */
#if PLATFORM_SDK_VERSION < 9
	if (DisconnectGPRSconnection() == 0)
	{
		printf("[%s]: Error! DisconnectGPRSconnection failed\n", __FILE__);
	}

	else
	{
		printf("[%s]: DisconnectGPRSconnection OK\n", __FILE__);
	}
#endif
	//mtelif_close();
	MRIL_Client_DeInit();

	return 0;
}
