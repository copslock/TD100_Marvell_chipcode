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
 * Filename: diag_comm_EXTif.c                                        *
 *                                                                    *
 * Target, platform: Common Platform, SW platform                     *
 *                                                                    *
 * Authors:Yossi Gabay, Yaeli Karni                                   *
 *                                                                    *
 * Description: handles the interaction with the USB                  *
 *                                                                    *
 * Notes:                                                             *
 *  Yaeli Karni, July 2007 - rearrange to support multi OS			 *
 *			initially it was only used in RTOS..					 *
 *                                                                    *
 *  Yaeli Karni, Aug 2007 - set USB to be gerneral ext-if,            *
 *			add general APIs for specific OS						 *
 *                                                                    *
 *  Yaeli Karni, Nov 2007 - change file name to diag_comm_EXTif.c     *
 *			split the OS specific parts to .c files		             *
 *           Each OS should implement the following fucntions:        *
 *				diagCommExtIFosSpecificInit							 *
 *				diagCommExtIfConnInit								 *
 *				diag_os_TransmitToExtIfMulti						 *
 *				diag_os_TransmitToExtIf								 *
 *				diagExtIfcon_discon									 *
 *			In addition, OS that needs rx-task to get info from      *
 *			extIF will startup the task (and implement it) in its	 *
 *			diagCommExtIfConnInit function							 *
 *                                                                    *                                                                    *
 **********************************************************************/

/************* General include definitions ****************************/
#include "global_types.h"
//#include "ICAT_config.h"
#include "diag_config.h"
#include "diag_osif.h"
#include INCLUDE_ASSERT_OR_DUMMY
//#include "diag_mem.h"
#include "diag_pdu.h"
#include "diag_buff.h"

// the following ifdef should wrap the whole file since this file is included by in the build BUT has same function name as in diag_comm_ssp.c

#include "diag.h"
//#include "diag_ram.h"
//#include "diag_comm.h"
#include "diag_comm_EXTif.h"
#include "diag_header_external.h"
#include "diag_rx.h"
#include "osa.h"
#include "diag_init.h"

/*************** OS specific includes *********************/
#if defined (OSA_LINUX)
#include <stdlib.h> //rand, srand
#include <time.h>
#include "diag_mmi_if_OSA_LINUX.h"
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#endif //defined (OSA_LINUX)
#if defined (OSA_WINCE)
#include <stdlib.h> //rand, srand
#include "diag_mmi_if_OSA_WINCE.h"
#endif //defined (OSA_WINCE)
/*************** OS specific includes end *********************/

#include "pxa_dbg.h"

#define DIAGPRINT DPRINTF

//Definitions for Transmit Task
#define DIAG_TX_DONE_FLAG_BIT                         0x1
#define DIAG_TX_FLAG_TIMER_EXPIRED_BIT            0x2
#define DIAG_TX_FLAG_QUEUE_ADD_BIT                0x4
#define DIAG_SINGLE_TX_DONE                                               0x8
#define DIAG_TX_DISCONNECT_BIT                    0x10
#define DIAG_TX_CONNECT_BIT                       0x20
#define DIAG_TX_RESET_QUEUE                                               0x40


// Disable the debug functionality: RAM consumption - works on Nucleus!!!
//#define DIAG_COM_USB_DEBUG_ENABLED 1

/*************** OS specific globals *********************/

/*************** OS specific globals end *********************/

extern GlobalIfData dataExtIf;
UINT32 OtherCoreReady; //We can call diagExtIFStatusDiscNotify only when the other core already got the ICATReadyNotify Msg
		       //Don't disconnect it when alreay disconnected.

int sdl_signature;

//configuration for tx task (DMA-multi transmit, or regular tx)
//ICAT EXPORTED STRUCT
typedef struct
{
	UINT8 MIN_BUFFERS_NUM;
	//wait in the queue until sent - the worst case of task loop is diag_comm_extIF_tx_config.TASK_TIMER_PERIOD
	UINT8 TASK_TIMER_PERIOD;
	UINT8 task_priority;
} DiagTransmitTxConfig;
#if 0
static DiagTransmitTxConfig diag_comm_extIF_tx_config = {
	0,              // MIN_BUFFERS_NUM, // 0 when we send single buffer, 2 when we send out mutilBuffs (like USB with DMA mode or SSP)
	3,              // BBBBBBBBBBBBBB UUUUUUUUUUUUUUUU GGGGGGGGGGGGGG if set this to zero TASK_TIMER_PERIOD
	DIAG_EXT_IF_TASK_PRIORITY
};                      //task_priority
#endif
void DiagCommExtIfTxTimerCallback(UINT32 param);

extern void diagDisconnectIcatInt(UINT32 bExtDisconnected);
#if (defined (OSA_LINUX) || defined (OSA_WINCE))
UINT32 diag_over_fs_mmi_start = 0; //0==stop; 1==start
extern void diagOverFScfg(const char *cfg_file);
extern void diagOverFSfilter(const char *filter_file);
extern void connectFS();
extern char log_file_rel_path[256];
#endif //(defined (OSA_LINUX) || defined (OSA_WINCE))

#if defined (OSA_LINUX)
extern BOOL FS_Diag_Assert_Received; //Workaround for assure that the queue is empty
#endif

/***************** specific OS implementation APIs decalartions ***********/

/***********************************************************************
* Function: diagCommTransmitToExtIf                                    *
************************************************************************
* Description: TACHLES transmit to the external interface              *
*                                                                      *
* Parameters:                                                          *
*      nOfBuffers - number of buffers to send                          *
*      PlaceInReportsListArray - location of first buffer in the cyclic queue *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
// globals for statistics collection (on multi tx...) TS of start, #-of-bytes
#if defined (DO_APPS_STATS)
UINT32 StartTS;
UINT32 totBytesMutiTx;
#endif
extern void dispatchServiceForSAP(UINT8 *Ori_data);
extern UINT32 diagCommSetChunk(DIAG_COM_RX_Packet_Info  *RxPacket,
			       CHAR *msgData, UINT32 length, UINT32 bRemoveHeader);

/***********************************************************************
* Function: diagExtIFstatusConnectNotify								   *
************************************************************************
* Description: handling the connect status                             *
*                                                                      *
* Parameters:                                                          *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
void diagExtIFstatusConnectNotify(void)
{

	DIAGPRINT(">>Enter diagExtIFstatusConnectNotify --diag_over_fs_mmi_start:%d\n",diag_over_fs_mmi_start);

#if (defined (OSA_LINUX) || defined (OSA_WINCE))
	if (diagIntData.m_bIsClient == 0 && (diagIntData.m_eLLtype == tLL_FS || diagIntData.m_eLLtype == tLL_SC) && diag_over_fs_mmi_start)
	{
		MMI_Report(MMI_REP_ALREADY_START, 0);
		return;
	}
	diag_over_fs_mmi_start = 1;
#endif //(defined (OSA_LINUX) || defined (OSA_WINCE))

	//os_status = OSAFlagSet (dataExtIf.TxFlgRef, DIAG_TX_CONNECT_BIT, OSA_FLAG_OR);
	//DIAG_ASSERT(os_status == OS_SUCCESS);

#if (defined (OSA_LINUX) || defined (OSA_WINCE))
	////////////////////////
	//Diag over FS support//
	////////////////////////
	if (diagIntData.m_bIsClient == 0 && (diagIntData.m_eLLtype == tLL_FS || diagIntData.m_eLLtype == tLL_SC))
	{
		UINT32 seed;
		CHAR usb_comm_ver_id[] = { 0x10, 0x00, 0x00, 0x00, 0x00, DB_VERSION_SERVICE, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
		CHAR usb_apps_ver_id[] = { 0x10, 0x00, 0x00, 0x00, 0x80, DB_VERSION_SERVICE, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
		//CHAR udp_comm_ver_id[]={0x7E, 0x7E, 0x06, 0x14, 0x02, 0x01, 0x00, 0x00, 0x01, 0x00, 0x0C, 0x00, 0x00, 0x00,
		//	0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		//	0x81, 0x81};
		CHAR usb_comm_acat_ready[] = { 0x10, 0x00, 0x00, 0x00, 0x00, ICAT_READY_NOTIFY, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
		CHAR usb_apps_acat_ready[] = { 0x10, 0x00, 0x00, 0x00, 0x80, ICAT_READY_NOTIFY, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

		//CHAR usb_comm_atomic[]={0x10, 0x00, 0x00, 0x00, 0x00, ATOMIC_READY_NOTIFY_AND_DB_VER, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
		//CHAR usb_apps_atomic[]={0x10, 0x00, 0x00, 0x00, 0x80, ATOMIC_READY_NOTIFY_AND_DB_VER, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
		char filename[256];

		//fprintf(stderr,"***** DIAG over FS: START diagExtIFstatusConnectNotify *****\r\n");

		//Signature for *.sdl file headers (ver 2 and above)
#if defined (OSA_LINUX)
		seed = time(NULL); //this doesn't work ??? (UINT32)diagGetTS_LINUX;
#endif
#if defined (OSA_WINCE)
		seed = 123; //TBD based on time sample
#endif
		fprintf(stderr, "***** DIAG over FS: sdl_signature seed=%d *****\r\n", seed);
		srand(seed);
		sdl_signature = rand();
		fprintf(stderr, "***** DIAG over FS: sdl_signature=%d *****\r\n", sdl_signature);

#if defined (OSA_LINUX)
		{
			/****************************/
			/* Copying /tel/ApDiagDB.gz */
			/****************************/
			char buff[256];
			size_t nbytes;
			ssize_t bytes_read, bytes_write;
			int read_fd, write_fd;

			fprintf(stderr, "***** DIAG over FS: Copying /tel/ApDiagDB.gz to %s *****\r\n", log_file_rel_path);
			read_fd = open("/tel/ApDiagDB.gz", O_RDONLY);
			if (read_fd < 0)
			{
				fprintf(stderr, "***** DIAG over FS: /tel/ApDiagDB.gz was not opened. Error: %s. *****\r\n", strerror(errno));
			}
			else
			{
				sprintf(filename, "%s/ApDiagDB.gz", log_file_rel_path);
				write_fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0x666);
				if (write_fd < 0)
				{
					fprintf(stderr, "***** DIAG over FS: %s was not opened. Error: %s. *****\r\n", filename, strerror(errno));
				}
				else
				{
					nbytes = sizeof(buff);
					while ((bytes_read = read(read_fd, buff, nbytes)) > 0)
					{
						if ((bytes_write = write(write_fd, buff, bytes_read)) != bytes_read)
						{
							if (bytes_write == -1)
							{
								fprintf(stderr, "***** DIAG over FS: Writing %s error: %s. *****\r\n", filename, strerror(errno));
							}
							fprintf(stderr, "***** DIAG over FS: Error when writing to %s. bytes_read=%d bytes_write=%d *****\r\n", filename, (int)bytes_read, (int)bytes_write);
							break;
						}
					}
					if (bytes_read == -1)
					{
						fprintf(stderr, "***** DIAG over FS: Reading /tel/ApDiagDB.gz error: %s. *****\r\n", strerror(errno));
					}
					close(write_fd);
				}
				close(read_fd);
			}
		}
		sprintf(filename, "%s/../diag_fs.cfg", log_file_rel_path);
#endif
#if defined (OSA_WINCE)
		sprintf(filename, "%s\\diag_fs.cfg", log_file_rel_path);
#endif
		//fprintf(stderr,"***** DIAG over FS: diag_fs.cfg location is:'%s' *****\r\n",filename);
		diagOverFScfg(filename); //OS specific
		connectFS();

		diagUsbConnectNotify();
		DIAGPRINT(">>>diagConnect , diagUsbConnectNotifyForSD\n");

		if (diagIntData.m_bWorkMultiCore)
		{
			/*
			   diagCommSetChunk(dataExtIf.RxPacket, usb_comm_atomic, 16, 1);
			   diagReceiveCommand(dataExtIf.RxPacket);
			 */
			diagCommSetChunk(dataExtIf.RxPacket, usb_comm_acat_ready, 16, 1);
			dispatchServiceForSAP((UINT8 *)dataExtIf.RxPacket->buffer);
			diagCommSetChunk(dataExtIf.RxPacket, usb_comm_ver_id, 16, 1); //Linux runs on the apps side and we need the comm ver id in the log file
			dispatchServiceForSAP((UINT8 *)dataExtIf.RxPacket->buffer);
			diagCommSetChunk(dataExtIf.RxPacket, usb_apps_ver_id, 16, 1);
			dispatchServiceForSAP((UINT8 *)dataExtIf.RxPacket->buffer);
		}
		else
		{
			diagCommSetChunk(dataExtIf.RxPacket, usb_apps_acat_ready, 16, 1);
			dispatchServiceForSAP((UINT8 *)dataExtIf.RxPacket->buffer);
			diagCommSetChunk(dataExtIf.RxPacket, usb_apps_ver_id, 16, 1);
			dispatchServiceForSAP((UINT8 *)dataExtIf.RxPacket->buffer);
		}
#if defined (OSA_LINUX)
		sprintf(filename, "%s/../filter.bin", log_file_rel_path);
#endif
#if defined (OSA_WINCE)
		sprintf(filename, "%s\\filter.bin", log_file_rel_path);
#endif
		fprintf(stderr, "***** DIAG over FS: invoking diagOverFSfilter with filter file %s. *****\r\n", filename);
		diagOverFSfilter(filename); //OS specific
	}
#endif //(defined (OSA_LINUX) || defined (OSA_WINCE))
}

/*************** OS specific logic  *********************/


/***********************************************************************
* Function: diagExtIFStatusDiscNotify                                  *
************************************************************************
* Description: handling status                                         *
*                                                                      *
* Parameters:                                                          *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
void diagExtIFStatusDiscNotify(void)
{
	extern void disconnectCOMM();
#if (defined (OSA_LINUX) || defined (OSA_WINCE))
	if (diagIntData.m_bIsClient == 0 && ((diagIntData.m_eLLtype == tLL_FS || diagIntData.m_eLLtype == tLL_SC) && diag_over_fs_mmi_start == 0))
	{
		MMI_Report(MMI_REP_ALREADY_STOP, 0);
		return;
	}
#if defined (OSA_LINUX)
	if (FS_Diag_Assert_Received == FALSE) ////Workaround for assure that the queue is empty
#endif
	{
		diag_over_fs_mmi_start = 0;
		//flush cached data
		disconnectCOMM();
		DIAGPRINT(">>Leave diagExtIFstatusDiscNotify --diag_over_fs_mmi_start:%d\n",diag_over_fs_mmi_start);
	}

#endif //(defined (OSA_LINUX) || defined (OSA_WINCE))

}


