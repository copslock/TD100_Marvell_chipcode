/*--------------------------------------------------------------------------------------------------------------------
 * (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
 * -------------------------------------------------------------------------------------------------------------------*/

/******************************************************************************
 * **
 * **  COPYRIGHT (C) 2001, 2002 Intel Corporation.
 * **
 * **  This software as well as the software described in it is furnished under
 * **  license and may only be used or copied in accordance with the terms of the
 * **  license. The information in this file is furnished for informational use
 * **  only, is subject to change without notice, and should not be construed as
 * **  a commitment by Intel Corporation. Intel Corporation assumes no
 * **  responsibility or liability for any errors or inaccuracies that may appear
 * **  in this document or any software that may be provided in association with
 * **  this document.
 * **  Except as permitted by such license, no part of this document may be
 * **  reproduced, stored in a retrieval system, or transmitted in any form or by
 * **  any means without the express written consent of Intel Corporation.
 * **
 * **  FILENAME:       audio_stub.h
 * **
 * **  PURPOSE:       contains functions for the audio stub functions
 * **
 * **  NOTES:      The following funcs are implemented in this file
 * **
 * **
 * ******************************************************************************/
#include "audio_stub_api_ttc.h"
#include "audio_stub_ttc.h"
#include "audiolevanteapi.h"
#include "linux_types.h"
#include "osa.h"
#include "string.h"
#include "msocket_api.h"
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdlib.h>
#include "shm_share.h"
#include "assert.h"
#include "acm_stub.h"
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

#include <pxa_dbg.h>
/*-------------------------------------------------------------------------*
 *  * Internal Global Variables
 *   *-------------------------------------------------------------------------*/
unsigned int gMsgTimeoutCounts;

/*FIXME: temporary code to support dynamically select PCM master or slave, we can remove the relative code after several months*/
BOOL gCPIsPCMMaster = 1;

#ifndef INT_MAX
#define INT_MAX 0x7fffffff
#endif
#define AUDIO_STUB_MESSAGE_Q_MAX          50

static UINT8 audioClientStubTaskStack[AUDIO_STUB_STACK_SIZE];
static OSTaskRef audioClientStubTaskRef   = NULL;
static UINT8 audioInitTaskStack[AUDIO_STUB_STACK_SIZE];
static OSTaskRef audioInitTaskRef   = NULL;

OSAMsgQRef audioClientStubMsgQ = NULL;
OSAMsgQRef msgQ4Calibration = NULL;
int  gAudioInitStatus = AUDIO_STUB_INIT_NOT_STARTED;
int audiostubsockfd;

#define STUB_INIT_TIMEOUT OSA_SUSPEND

void dump_buffer(char* buf, int len)
{
	DBGMSG("*****************************dump_buffer, pointer %p************************************\n", buf);
	int i;
	char * ptempBuf = malloc(135);
	DBGMSG("status                 device                 format                 gain_nums\n");
	ptempBuf[0]='\0';
	char num_buf[15];

	for (i = 0; i < 16; i++)
	{
		sprintf(num_buf, "%02X ", buf[i]);
		strcat(ptempBuf, num_buf);
		if (((i + 1) % 4) == 0)
			strcat(ptempBuf, "           ");
	}
	strcat(ptempBuf, "\n");
	DBGMSG(ptempBuf);
	DBGMSG("path                   gain_id                gain_val               mutePath volume regIndex gainMask   gainShift\n");
	ptempBuf[0]='\0';
	for (i = 0; i < len - 16; i++)
	{
		if (((i + 1) % 20) == 15)
			sprintf(num_buf, "    %02X       ", (buf[i + 16] < 53 ? (buf[i + 16] + 171) : (buf[i + 16] + 172)));/*change regIndex to reg address*/
		else if ((i + 1) % 20 == 14)
			sprintf(num_buf, "      %03d", buf[i + 16]);
		else
			sprintf(num_buf, "%02X ", buf[i + 16]);
		strcat(ptempBuf, num_buf);
		if (((i + 1) % 4) == 0)
		{
			if (((i + 1) % 20) == 16)
				strcat(ptempBuf, "        ");
			else
				strcat(ptempBuf, "           ");
		}
		if (((i + 1) % 20) == 0)
		{
			strcat(ptempBuf, "\n");
			DBGMSG(ptempBuf);
			ptempBuf[0]='\0';
		}
	}

	if (ptempBuf[0] != '\0')
	{
		strcat(ptempBuf, "\n");
		DBGMSG(ptempBuf);
	}
	free(ptempBuf);
}

int audioSendMsgToServer(int procId, unsigned char *msg, int msglen)
{
	int ret;
	ShmApiMsg *pShmArgs;

	if (msg == NULL)
		DPRINTF("\nassert in (msg == NULL)\n");
	assert(msg != NULL);

	pShmArgs = (ShmApiMsg *)msg;
	pShmArgs->svcId = AudioStubSvc_Id;
	pShmArgs->msglen = msglen;
	pShmArgs->procId = procId;

//	dump_buffer(msg, msglen+SHM_HEADER_SIZE);

	//msend may return -1 by msocket can't contact with CP
	ret = msend(audiostubsockfd, msg, msglen + SHM_HEADER_SIZE, 0);

	return ret;
}

extern void ACMRegisterComponent(void);
extern void ACMSyncGainDefaultSetting(void);
void audioInitTask(void *input)
{
	UNUSEDPARAM(input)

	int audioInitTryTimes = INT_MAX;
	ShmApiMsg startmsg;

	while (audioInitTryTimes > 0)
	{
		if (gAudioInitStatus == AUDIO_STUB_INIT_COMPLETE)
		{
			DPRINTF("\n\naudioInitTask done!\n\n");

			DPRINTF("ACMRegisterComponent in audioInitTask\n");
			ACMRegisterComponent();

			DPRINTF("ACMSyncGainDefaultSetting in audioInitTask\n");
			ACMSyncGainDefaultSetting();

			ioctl(audiostubsockfd, MSOCKET_IOC_PMIC_QUERY, &gCPIsPCMMaster);
			DPRINTF("audioInitTask: gCPIsPCMMaster is %d\n", gCPIsPCMMaster);

			LevanteMasterSlaveModeSettingsData masterSlaveSettingsData = {PCM_TYPE, gCPIsPCMMaster};
			LevanteAdditionalSettingsSet(MASTER_SLAVE_MODE_SETTINGS, (void *)&masterSlaveSettingsData);

			return;
		}
		DPRINTF("\n\naudioSendMsgToServer AUDIO_STUB_START\n\n");
		audioSendMsgToServer(AUDIO_STUB_START, (unsigned char *)&startmsg, 0);

		audioInitTryTimes--;
		OSATaskSleep(100);
	}
	DPRINTF("\n\n\naudioInitTask not done!\n\n\n");
}


UINT8 audioClientStubInit(int pStatusIndCb)
{
	UNUSEDPARAM(pStatusIndCb)
	OS_STATUS status;

	//AUDIO_TRACE1(AUDIO_CLIENT_STUB, AUDIO_CLIENT_TASK, __LINE__, AUDIO_TRACE_API,"audioClientStubInit: The audioClientStubPriority is %d",AUDIO_TASK_PRIORITY);
	DPRINTF("audioClientStubInit: The audioClientStubPriority is %d\n", AUDIO_TASK_PRIORITY);

	/* Check if the client stub has been started already */
	if (gAudioInitStatus != AUDIO_STUB_INIT_NOT_STARTED)
	{
		return STUB_ERROR;
	}

	if (audioClientStubMsgQ == NULL )
	{
		status = OSAMsgQCreate(&audioClientStubMsgQ,
				       //"audioClientStubMsgQ",
				       MAX_AUDIO_STUB_RXMSG_LEN,
				       AUDIO_STUB_MESSAGE_Q_MAX,
				       OSA_PRIORITY);

		if (status != OS_SUCCESS)
		{
			DPRINTF("failed to create stubMsgQ\n");
			return STUB_ERROR;
		}
	}

	if (msgQ4Calibration == NULL )
	{
		status = OSAMsgQCreate(&msgQ4Calibration,
				       MAX_AUDIO_STUB_RXMSG_LEN,
				       AUDIO_STUB_MESSAGE_Q_MAX,
				       OSA_PRIORITY);

		if (status != OS_SUCCESS)
		{
			DPRINTF("failed to create stubMsgQ for calibration\n");
			return STUB_ERROR;
		}
	}

	audiostubsockfd = msocket(AUDIOSTUB_PORT);
	if (audiostubsockfd <= 0)
		DPRINTF("\nassert in (audiostubsockfd>0)\n");
	assert(audiostubsockfd > 0);
	//AUDIO_TRACE1(AUDIO_CLIENT_STUB, AUDIO_CLIENT_TASK, __LINE__, AUDIO_TRACE_ERROR," audioClientStubInit: audiostubsockfd=0x%x",audiostubsockfd);
	DPRINTF("audioClientStubInit: audiostubsockfd=0x%x\n", audiostubsockfd);
	if (audioClientStubTaskRef == NULL)
	{
		status = OSATaskCreate(&audioClientStubTaskRef,
				       (void *)audioClientStubTaskStack,
				       AUDIO_STUB_STACK_SIZE,
				       AUDIO_TASK_PRIORITY,
				       (CHAR*)"audioClientStubTask",
				       audioClientStubTask,
				       NULL);


		if (status != OS_SUCCESS)
		{
			//AUDIO_TRACE(AUDIO_CLIENT_STUB, AUDIO_CLIENT_TASK, __LINE__, AUDIO_TRACE_ERROR," audioClientStubInit: AUDUIO Stub OSA Task Create Failed");
			DPRINTF("audioClientStubInit: audioClientStubTask Create Failed\n");
			return STUB_ERROR;
		}
	}


	//need delete audioInitTask when init handshaking done
	if (audioInitTaskRef == NULL)
	{
		status = OSATaskCreate(&audioInitTaskRef,
				       (void *)audioInitTaskStack,
				       AUDIO_STUB_STACK_SIZE,
				       AUDIO_TASK_PRIORITY,
				       (CHAR*)"audioInitTask",
				       audioInitTask,
				       NULL);


		if (status != OS_SUCCESS)
		{
			//AUDIO_TRACE(AUDIO_CLIENT_STUB, AUDIO_CLIENT_TASK, __LINE__, AUDIO_TRACE_ERROR," audioClientStubInit: AUDIO Stub OSA Task Create Failed");
			DPRINTF("audioClientStubInit: audioInitTask Create Failed\n");
			return STUB_ERROR;
		}
	}
	return STUB_SUCCESS;
}

/************************************************************************************
 *  *
 *  * Name:        audioClientStubDeinit
 *  *
 *  * Description: Delete client stub task and message queue and reset static parameters
 *  *
 *  * Parameters:  None
 *  *
 *  * Returns:     None
 *  *
 *  * Notes:
 *  *
 ***********************************************************************************/
void audioClientStubDeinit(void)
{

	// AUDIO_TRACE(AUDIO_CLIENT_STUB, AUDIO_CLIENT_TASK, __LINE__, AUDIO_TRACE_API,"audioClientStubDeinit:");

	OSATaskDelete(audioClientStubTaskRef);
	OSAMsgQDelete(audioClientStubMsgQ);
	OSAMsgQDelete(msgQ4Calibration);

	/* Reset and clear the task and queue references */
	audioClientStubTaskRef = NULL;
	audioClientStubMsgQ    = NULL;
	msgQ4Calibration       = NULL;

	/* reset the client stub initialization status */
	gAudioInitStatus = AUDIO_STUB_INIT_NOT_STARTED;

	close(audiostubsockfd);

}
/* process message received from audio stub server */
static void audioStubEventHandler (UINT8*  rxmsg, int dataSize)
{
	ShmApiMsg *pAudioStubClientMsg;

	pAudioStubClientMsg = (ShmApiMsg *)rxmsg;
	AudioStubServerSendReq  *recvData;

	UPLINKSTREAM_REQUEST *uplinkReq;
	void *ptr;

	recvData = (AudioStubServerSendReq *)(rxmsg + SHM_HEADER_SIZE);

	StubMsg msgq;

	OS_STATUS status;
	if(rxmsg == NULL || dataSize < 0)
	{
		DPRINTF("audioStubEventhandle param error, dataSize = %d\n", dataSize);
		return;
	}

	// AUDIO_TRACE2(AUDIO_CLIENT_STUB, AUDIO_CLIENT_TASK, __LINE__, AUDIO_TRACE_INFO," audioStubEventHandler, msgid=%d, svcId=%d",pAudioStubClientMsg->procId,pAudioStubClientMsg->svcId);
	if (pAudioStubClientMsg->svcId != AudioStubSvc_Id)
		DPRINTF("\nassert in (pAudioStubClientMsg->svcId == AudioStubSvc_Id)\n");
	assert(pAudioStubClientMsg->svcId == AudioStubSvc_Id);
	switch (pAudioStubClientMsg->procId)
	{
	case AUDIO_STUB_CONFIRM:
		DPRINTF("audioStubEventHandler AUDIO_STUB_CONFIRM\n");
		gAudioInitStatus = AUDIO_STUB_INIT_COMPLETE;
		break;

	case AUDIO_CLIENT_SERVER_DATA_CNF_IND:
		switch (recvData->cbId)
		{
		case AUDIO_REGCPNT_CONFIRM:
			DPRINTF("audioStubEventhandle AUDIO_REGCPNT_CONFIRM\n");
			msgq.procId = AUDIO_REGCPNT_CONFIRM;
			msgq.msglen = sizeof(ACMAudioComponentRegisterRsp);
			/*added by Raul, for rxmsg's content may be modified by next call in this function, before the calling function use it*/
			ptr = malloc(msgq.msglen);
			msgq.pData = (void *)(rxmsg + SHM_HEADER_SIZE + sizeof(AudioStubServerSendReq));
			memcpy(ptr, msgq.pData, msgq.msglen);
			msgq.pData = ptr;
			//dump_buffer(msgq.pData, sizeof(ACMAudioComponentRegisterRsp));
			status = OSAMsgQSend(audioClientStubMsgQ, sizeof(StubMsg),
					     (UINT8*)&msgq, OSA_SUSPEND);
			if (status != OS_SUCCESS)
			{
				DPRINTF("failed to send MsgQ, return code is %d\n", status);
			}
			break;

		case AUDIO_ENABLEHWCPNT_CONFIRM:
			DPRINTF("audioStubEventhandle AUDIO_ENALBEHWCPNT_CONFIRM\n");
			msgq.procId = AUDIO_ENABLEHWCPNT_CONFIRM;
			msgq.msglen = sizeof(AudioDeviceEnableRsp);
			/*added by Raul, for rxmsg's content may be modified by next call in this function, before the calling function use it*/
			ptr = malloc(msgq.msglen);
			msgq.pData = (void *)(rxmsg + SHM_HEADER_SIZE + sizeof(AudioStubServerSendReq));
			memcpy(ptr, msgq.pData, msgq.msglen);
			msgq.pData = ptr;
			dump_buffer(msgq.pData, sizeof(AudioDeviceEnableRsp));
			status = OSAMsgQSend(audioClientStubMsgQ, sizeof(StubMsg),
					     (UINT8*)&msgq, OSA_SUSPEND);
			if (status != OS_SUCCESS)
			{
				DPRINTF("failed to send MsgQ, return code is %d\n", status);
			}
			break;

		case AUDIO_DISABLEHWCPNT_CONFIRM:
			DPRINTF("audioStubEventhandle AUDIO_DISALBEHWCPNT_CONFIRM\n");
			msgq.procId = AUDIO_DISABLEHWCPNT_CONFIRM;
			msgq.msglen = sizeof(AudioDeviceDisableRsp);
			/*added by Raul, for rxmsg's content may be modified by next call in this function, before the calling function use it*/
			ptr = malloc(msgq.msglen);
			msgq.pData = (void *)(rxmsg + SHM_HEADER_SIZE + sizeof(AudioStubServerSendReq));
			memcpy(ptr, msgq.pData, msgq.msglen);
			msgq.pData = ptr;
			dump_buffer(msgq.pData, sizeof(AudioDeviceDisableRsp));
			status = OSAMsgQSend(audioClientStubMsgQ, sizeof(StubMsg),
					     (UINT8*)&msgq, OSA_SUSPEND);
			if (status != OS_SUCCESS)
			{
				DPRINTF("failed to send MsgQ, return code is %d\n", status);
			}
			break;

		case AUDIO_SETHWCPNTVOL_CONFIRM:
			DPRINTF("audioStubEventhandle AUDIO_SETHWCPNTVOL_CONFIRM\n");
			msgq.procId = AUDIO_SETHWCPNTVOL_CONFIRM;
			msgq.msglen = sizeof(AudioDeviceVolumeSetRsp);
			/*added by Raul, for rxmsg's content may be modified by next call in this function, before the calling function use it*/
			ptr = malloc(msgq.msglen);
			msgq.pData = (void *)(rxmsg + SHM_HEADER_SIZE + sizeof(AudioStubServerSendReq));
			memcpy(ptr, msgq.pData, msgq.msglen);
			msgq.pData = ptr;
			dump_buffer(msgq.pData, sizeof(AudioDeviceVolumeSetRsp));
			status = OSAMsgQSend(audioClientStubMsgQ, sizeof(StubMsg),
					     (UINT8*)&msgq, OSA_SUSPEND);
			if (status != OS_SUCCESS)
			{
				DPRINTF("failed to send MsgQ, return code is %d\n", status);
			}
			break;

		case AUDIO_HWCPNTMUTE_CONFIRM:
			DPRINTF("audioStubEventhandle AUDIO_HWCPNTMUTE_CONFIRM\n");
			msgq.procId = AUDIO_HWCPNTMUTE_CONFIRM;
			msgq.msglen = sizeof(AudioDeviceMuteRsp);
			/*added by Raul, for rxmsg's content may be modified by next call in this function, before the calling function use it*/
			ptr = malloc(msgq.msglen);
			msgq.pData = (void *)(rxmsg + SHM_HEADER_SIZE + sizeof(AudioStubServerSendReq));
			memcpy(ptr, msgq.pData, msgq.msglen);
			msgq.pData = ptr;
			dump_buffer(msgq.pData, sizeof(AudioDeviceMuteRsp));
			status = OSAMsgQSend(audioClientStubMsgQ, sizeof(StubMsg),
					     (UINT8*)&msgq, OSA_SUSPEND);
			if (status != OS_SUCCESS)
			{
				DPRINTF("failed to send MsgQ, return code is %d\n", status);
			}
			break;

		case AUDIO_ENABLEOUTSTREAM_CONFIRM:
			DPRINTF("audioStubEventhandle AUDIO_ENABLEOUTSREAM_CONFIRM\n");
			msgq.procId = AUDIO_ENABLEOUTSTREAM_CONFIRM;
			msgq.msglen = sizeof(AudioStreamOutStartRsp);
			/*added by Raul, for rxmsg's content may be modified by next call in this function, before the calling function use it*/
			ptr = malloc(msgq.msglen);
			msgq.pData = (void *)(rxmsg + SHM_HEADER_SIZE + sizeof(AudioStubServerSendReq));
			memcpy(ptr, msgq.pData, msgq.msglen);
			msgq.pData = ptr;
			//dump_buffer(msgq.pData, sizeof(AudioStreamOutStartRsp));
			status = OSAMsgQSend(audioClientStubMsgQ, sizeof(StubMsg),
					     (UINT8*)&msgq, OSA_SUSPEND);
			if (status != OS_SUCCESS)
			{
				DPRINTF("failed to send MsgQ, return code is %d\n", status);
			}
			break;
		case AUDIO_DISABLEOUTSTREAM_CONFIRM:
			DPRINTF("audioStubEventhandle AUDIO_DISABLEOUTSREAM_CONFIRM\n");
			msgq.procId = AUDIO_DISABLEOUTSTREAM_CONFIRM;
			msgq.msglen = sizeof(AudioStreamOutStopRsp);
			/*added by Raul, for rxmsg's content may be modified by next call in this function, before the calling function use it*/
			ptr = malloc(msgq.msglen);
			msgq.pData = (void *)(rxmsg + SHM_HEADER_SIZE + sizeof(AudioStubServerSendReq));
			memcpy(ptr, msgq.pData, msgq.msglen);
			msgq.pData = ptr;
			//dump_buffer(msgq.pData, sizeof(AudioStreamOutStopRsp));
			status = OSAMsgQSend(audioClientStubMsgQ, sizeof(StubMsg),
					     (UINT8*)&msgq, OSA_SUSPEND);
			if (status != OS_SUCCESS)
			{
				DPRINTF("failed to send MsgQ, return code is %d\n", status);
			}
			break;
		case AUDIO_ENABLEINSTREAM_CONFIRM:
			DPRINTF("audioStubEventhandle AUDIO_ENABLEINSREAM_CONFIRM\n");
			msgq.procId = AUDIO_ENABLEINSTREAM_CONFIRM;
			msgq.msglen = sizeof(AudioStreamInStartRsp);
			/*added by Raul, for rxmsg's content may be modified by next call in this function, before the calling function use it*/
			ptr = malloc(msgq.msglen);
			msgq.pData = (void *)(rxmsg + SHM_HEADER_SIZE + sizeof(AudioStubServerSendReq));
			memcpy(ptr, msgq.pData, msgq.msglen);
			msgq.pData = ptr;
			//dump_buffer(msgq.pData, sizeof(AudioStreamInStartRsp));
			status = OSAMsgQSend(audioClientStubMsgQ, sizeof(StubMsg),
					     (UINT8*)&msgq, OSA_SUSPEND);
			if (status != OS_SUCCESS)
			{
				DPRINTF("failed to send MsgQ, return code is %d\n", status);
			}
			break;
		case AUDIO_DISABLEINSTREAM_CONFIRM:
			DPRINTF("audioStubEventhandle AUDIO_DISABLEINSREAM_CONFIRM\n");
			msgq.procId = AUDIO_DISABLEINSTREAM_CONFIRM;
			msgq.msglen = sizeof(AudioStreamInStopRsp);
			/*added by Raul, for rxmsg's content may be modified by next call in this function, before the calling function use it*/
			ptr = malloc(msgq.msglen);
			msgq.pData = (void *)(rxmsg + SHM_HEADER_SIZE + sizeof(AudioStubServerSendReq));
			memcpy(ptr, msgq.pData, msgq.msglen);
			msgq.pData = ptr;
			//dump_buffer(msgq.pData, sizeof(AudioStreamInStopRsp));
			status = OSAMsgQSend(audioClientStubMsgQ, sizeof(StubMsg),
					     (UINT8*)&msgq, OSA_SUSPEND);
			if (status != OS_SUCCESS)
			{
				DPRINTF("failed to send MsgQ, return code is %d\n", status);
			}
			break;

		case AUDIO_STREAMINDICATE_ID:
		{
			int index = 0;
			//DPRINTF("audioStubEventHandle AUDIO_STREAMINDICATE_ID\n");
			uplinkReq = (UPLINKSTREAM_REQUEST *)(rxmsg + SHM_HEADER_SIZE +
							     sizeof(AudioStubServerSendReq));
			//dump_buffer(uplinkReq, sizeof(UPLINKSTREAM_REQUEST));
			/*make data point to voice data*/
			/* Ensure the handle number not larger than the maxima number */
			if (uplinkReq->handle_num > NUM_OF_DB_ENTRIES)  {
				ERRMSG("audioStubEventHandle ERROR: the handle_num is wrong: %d \n", uplinkReq->handle_num);
				uplinkReq->handle_num = NUM_OF_DB_ENTRIES;
			}
			if (uplinkReq->handle_num <= 0) {
				ERRMSG("audioStubEventHandle ERROR: the handle_num is wrong: %d \n", uplinkReq->handle_num);
				return;
			}

			for (index = 0; (index < uplinkReq->handle_num) && (uplinkReq->handle_num > 0); index++)
			{
				if (NULL != uplinkReq->streamInd[index].data)
				{
					/* check the data length to ensure all data are valid, otherwise don't handle it. */
					if ((UINT32)dataSize < (SHM_HEADER_SIZE + sizeof(AudioStubServerSendReq) + sizeof(UPLINKSTREAM_REQUEST) + index * ACMAPPS_STREAM_SIZE * sizeof(unsigned short))) {
						ERRMSG("audioStubEventHandle AUDIO_STREAMINDICATE_ID: invalid packet. the fake num = %d\n", uplinkReq->handle_num);
						return;
					}
					uplinkReq->streamInd[index].data = (UINT16 *)( (rxmsg + SHM_HEADER_SIZE +
											sizeof(AudioStubServerSendReq) +
											sizeof(UPLINKSTREAM_REQUEST) +
											index * ACMAPPS_STREAM_SIZE * sizeof(unsigned short) ));
				}
			}
			StreamIndicate(uplinkReq);
		}
		break;

		case AUDIO_GETMSASETTING_ID:
		{
			DPRINTF("audioStubEventhandle AUDIO_GETMSASETTING_ID\n");
			msgq.procId = AUDIO_GETMSASETTING_ID;
			msgq.msglen = sizeof(ACMAudioDSPSettings);
			ptr = malloc(msgq.msglen);
			msgq.pData = (void *)(rxmsg + SHM_HEADER_SIZE + sizeof(AudioStubServerSendReq));
			memcpy(ptr, msgq.pData, msgq.msglen);
			msgq.pData = ptr;
			//FIXME: if we aren't in UI calibration, we should drop these messages
			status = OSAMsgQSend(msgQ4Calibration, sizeof(StubMsg),
					     (UINT8*)&msgq, OSA_SUSPEND);
			if (status != OS_SUCCESS)
			{
				DPRINTF("failed to send MsgQ, return code is %d\n", status);
			}
		}
		break;

		default:
			DPRINTF("audioStubEventhandle default error, recvData->cbId is %d\n", recvData->cbId);
			msgq.pData = (void *)(rxmsg + SHM_HEADER_SIZE + sizeof(AudioStubServerSendReq));
			dump_buffer(msgq.pData, sizeof(AudioDeviceEnableRsp));
			//assert(0); //TBD: Unexpected message
			//DPRINTF("audioStubEventhandle default error\n");
			break;
		}
		break;
	case MsocketLinkupProcId:
		/*re-init audio stub, handshake with CP*/
		DPRINTF("audioStubEventhandle MsocketLinkupProcId\n");
		gMsgTimeoutCounts = 0;
		if (audioInitTaskRef)
			OSATaskDelete(audioInitTaskRef);
		status = OSATaskCreate(&audioInitTaskRef,
				       (void *)audioInitTaskStack,
				       AUDIO_STUB_STACK_SIZE,
				       AUDIO_TASK_PRIORITY,
				       (CHAR*)"audioInitTask",
				       audioInitTask,
				       NULL);

		if (status != OS_SUCCESS)
		{
			DPRINTF("MsocketLinkupProcId: audioInitTask Create Failed\n");
		}

		break;
	case MsocketLinkdownProcId:
		/*reset some global variable*/
		DPRINTF("audioStubEventhandle MsocketLinkdownProcId\n");
		gAudioInitStatus = AUDIO_STUB_INIT_NOT_STARTED;
		if (audioInitTaskRef)
			OSATaskDelete(audioInitTaskRef);
		audioInitTaskRef=NULL;
		break;
	default:
		DPRINTF("\nassert in TBD: Unexpected message\n");
		assert(0);         //TBD: Unexpected message
		break;
	} // end switch
}
/************************************************************************************
 *  *
 *  * Name:        audioClientStubTask
 *  *
 *  * Description: Client stub task function
 *  *
 *  * Parameters:
 *  *
 *  * Returns:     None
 *  *
 *  * Notes:
 *  *
 *  **********************************************************************************/
UINT8 audioStubRecvMsg[MAX_AUDIO_STUB_RXMSG_LEN];
#if defined (BIONIC)
void __noreturn audioClientStubTask(void *input)
#else
void audioClientStubTask(void *input)
#endif
{
	UNUSEDPARAM(input)

	int ret;

	//	AUDIO_TRACE(AUDIO_CLIENT_STUB, AUDIO_CLIENT_TASK, __LINE__, AUDIO_TRACE_API," Running AudioStubClientTask");

	while (TRUE)
	{
		/* wating AUDIO client event */
		ret = mrecv(audiostubsockfd, audioStubRecvMsg, MAX_AUDIO_STUB_RXMSG_LEN, 0);
		if (ret <= 0)
			continue;

		audioStubEventHandler(audioStubRecvMsg, ret);
	} /* end while */
}

