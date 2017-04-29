/*--------------------------------------------------------------------------------------------------------------------
 * (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
 * -------------------------------------------------------------------------------------------------------------------*/

/******************************************************************************
 * **
 * **  COPYRIGHT (C) 2000, 2002 Intel Corporation.
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
 * **  FILENAME:       audio_stub_api.c
 * **
 * **  PURPOSE:        stub api functions used by the application to make audio calls
 * ******************************************************************************/
 
#include <stdlib.h>
#include "audio_stub_api_ttc.h"
#include "audio_stub_ttc.h"
#include "linux_types.h"
#include "assert.h"
#include "osa.h"
#include "shm_share.h"
#include "pxa_dbg.h"

#define printf DPRINTF
extern OSAMsgQRef audioClientStubMsgQ;

extern int gAudioInitStatus;
unsigned int gMsgTimeoutCounts = 0;

STUB_ReturnCode InitAudioStub()
{
	STUB_ReturnCode status;

	status = audioClientStubInit(AUDIO_STUB_INIT_NOT_STARTED);
	if (status == STUB_SUCCESS)
	{
		DPRINTF("audio stub init success\n");
		return STUB_SUCCESS;
	}
	else
		return STUB_ERROR;
}


STUB_ReturnCode RegisterHwComponent(ACMAudioComponentRegisterReq * registerReq, ACMAudioComponentRegisterRsp * registerRsp)
{
	OS_STATUS status;
	STUB_ReturnCode ret;
	unsigned char *pbuf;
	AudioStubClientSendReq *reqdata;
	StubMsg msgq;

	DPRINTF("RegisterHwComponent start\n");

	pbuf = (unsigned char *)malloc(SHM_HEADER_SIZE + sizeof(ACM_Component) + sizeof(AudioStubClientSendReq));

	reqdata = (AudioStubClientSendReq *)(pbuf + SHM_HEADER_SIZE);
	reqdata->prodId = AUDIO_REGCPNT_ID;
	reqdata->size = sizeof(ACM_Component);
	reqdata->pData = registerReq;

	memcpy((char *)(pbuf + SHM_HEADER_SIZE + sizeof(AudioStubClientSendReq) ),
	       registerReq, sizeof(ACM_Component));

	audioSendMsgToServer(AUDIO_CLIENT_DATA_REQ, pbuf, sizeof(ACM_Component) + sizeof(AudioStubClientSendReq));
	free(pbuf);

	do
	{
		status = OSAMsgQRecv(audioClientStubMsgQ, (UINT8 *)&msgq,  sizeof(StubMsg), MSGQ_TIMEOUT);
		if (status == OS_SUCCESS)
		{
			if (gMsgTimeoutCounts == 0)
			{
				memcpy(registerRsp, msgq.pData, sizeof(ACMAudioComponentRegisterRsp));
				ret = STUB_SUCCESS;
				free(msgq.pData);
				break;
			}
			else
			{
				/* received MSG is not what we want, ignore it */
				DPRINTF("Msg received dis-match, ignore it. timeout count = %d.\n", gMsgTimeoutCounts);
				if (gMsgTimeoutCounts > 0)
					gMsgTimeoutCounts--;
				free(msgq.pData);
			}
		}
		else if (status == OS_TIMEOUT)
		{
			DPRINTF("CPU/Msocket busy? OSAMsgQRecv timeout, return %d\n", status);
			ret = STUB_TIMEOUT;
			gMsgTimeoutCounts++;
			break;
		}
		else
		{
			DPRINTF("CPU/Msocket busy? OSAMsgQRecv fail, return %d\n", status);
			ret = STUB_ERROR;
			gMsgTimeoutCounts++;
			break;
		}
	} while(1);

	return ret;
}


STUB_ReturnCode EnableHwComponent(AudioDeviceEnableReq *audioEnableReq, AudioDeviceEnableRsp *audioEnableRsp)
{
	OS_STATUS status;
	STUB_ReturnCode ret;
	unsigned char *pbuf;
	AudioStubClientSendReq *reqdata;
	StubMsg msgq;

	DPRINTF("EnableHwComponent start\n");

	pbuf = (unsigned char *)malloc(SHM_HEADER_SIZE + sizeof(AudioDeviceEnableReq) + sizeof(AudioStubClientSendReq));

	reqdata = (AudioStubClientSendReq *)(pbuf + SHM_HEADER_SIZE);
	reqdata->prodId = AUDIO_ENABLEHWCPNT_ID;
	reqdata->size = sizeof(AudioDeviceEnableReq);
	reqdata->pData = audioEnableReq;

	memcpy((char *)(pbuf + SHM_HEADER_SIZE + sizeof(AudioStubClientSendReq) ),
	       audioEnableReq, sizeof(AudioDeviceEnableReq));

	if(gAudioInitStatus!=AUDIO_STUB_INIT_COMPLETE)
	{
		DPRINTF("AUDIO NOT INIT\n");
		free(pbuf);
		return STUB_ERROR;
	}
	audioSendMsgToServer(AUDIO_CLIENT_DATA_REQ, pbuf, sizeof(AudioDeviceEnableReq) + sizeof(AudioStubClientSendReq));
	free(pbuf);

	do
	{
		status = OSAMsgQRecv(audioClientStubMsgQ, (UINT8 *)&msgq,  sizeof(StubMsg), MSGQ_TIMEOUT);
		if (status == OS_SUCCESS)
		{
			if (gMsgTimeoutCounts == 0)
			{
				memcpy(audioEnableRsp, msgq.pData, sizeof(AudioDeviceEnableRsp));
				ret = STUB_SUCCESS;
				free(msgq.pData);
				break;
			}
			else
			{
				/* received MSG is not what we want, ignore it */
				DPRINTF("Msg received dis-match, ignore it. timeout count = %d.\n", gMsgTimeoutCounts);
				if (gMsgTimeoutCounts > 0)
					gMsgTimeoutCounts--;
				free(msgq.pData);
			}
		}
		else if (status == OS_TIMEOUT)
		{
			DPRINTF("CPU/Msocket busy? OSAMsgQRecv timeout, return %d\n", status);
			ret = STUB_TIMEOUT;
			gMsgTimeoutCounts++;
			break;
		}
		else
		{
			DPRINTF("CPU/Msocket busy? OSAMsgQRecv fail, return %d\n", status);
			ret = STUB_ERROR;
			gMsgTimeoutCounts++;
			break;
		}
	} while(1);

	return ret;
}

STUB_ReturnCode DisableHwComponent(AudioDeviceDisableReq *audioDisableReq, AudioDeviceDisableRsp *audioDisableRsp)
{
	OS_STATUS status;
	STUB_ReturnCode ret;
	unsigned char *pbuf;
	AudioStubClientSendReq *reqdata;
	StubMsg msgq;

	DPRINTF("DisableHwComponent start\n");
	pbuf = (unsigned char *)malloc(SHM_HEADER_SIZE + sizeof(AudioDeviceDisableReq) + sizeof(AudioStubClientSendReq));

	reqdata = (AudioStubClientSendReq *)(pbuf + SHM_HEADER_SIZE);
	reqdata->prodId = AUDIO_DISABLEHWCPNT_ID;
	reqdata->size = sizeof(AudioDeviceEnableReq);
	reqdata->pData = audioDisableReq;
	memcpy((char *)(pbuf + SHM_HEADER_SIZE + sizeof(AudioStubClientSendReq) ),
	       audioDisableReq, sizeof(AudioDeviceDisableReq));
	if(gAudioInitStatus!=AUDIO_STUB_INIT_COMPLETE)
	{
		DPRINTF("AUDIO NOT INIT\n");
		free(pbuf);
		return STUB_ERROR;
	}

	audioSendMsgToServer(AUDIO_CLIENT_DATA_REQ, pbuf, sizeof(AudioDeviceDisableReq) + sizeof(AudioStubClientSendReq));
	free(pbuf);

	do
	{
		status = OSAMsgQRecv(audioClientStubMsgQ, (UINT8 *)&msgq,  sizeof(StubMsg), MSGQ_TIMEOUT);
		if (status == OS_SUCCESS)
		{
			if (gMsgTimeoutCounts == 0)
			{
				memcpy(audioDisableRsp, msgq.pData, sizeof(AudioDeviceDisableRsp));
				ret = STUB_SUCCESS;
				free(msgq.pData);
				break;
			}
			else
			{
				/* received MSG is not what we want, ignore it */
				DPRINTF("Msg received dis-match, ignore it. timeout count = %d.\n", gMsgTimeoutCounts);
				if (gMsgTimeoutCounts > 0)
					gMsgTimeoutCounts--;
				free(msgq.pData);
			}
		}
		else if (status == OS_TIMEOUT)
		{
			DPRINTF("CPU/Msocket busy? OSAMsgQRecv timeout, return %d\n", status);
			ret = STUB_TIMEOUT;
			gMsgTimeoutCounts++;
			break;
		}
		else
		{
			DPRINTF("CPU/Msocket busy? OSAMsgQRecv fail, return %d\n", status);
			ret = STUB_ERROR;
			gMsgTimeoutCounts++;
			break;
		}
	} while(1);

	return ret;
}

/************************************************************************************
 *  *
 *  * Name:      SetComponentVolume
 *  *
 *  * Description: This function tells the Comms processor to change the gain in the
 *  *        near end audio (microphone) during a voice call.
 *  *
 *  * Parameters:
 *  *
 *  *
 *  * Returns:     None
 *  *
 *  *
 *  **********************************************************************************/
STUB_ReturnCode SetComponentVolume(AudioDeviceVolumeSetReq *audioVolReq, AudioDeviceVolumeSetRsp *audioVolRsp)
{
	OS_STATUS status;
	STUB_ReturnCode ret;
	unsigned char *pbuf;
	AudioStubClientSendReq *reqdata;
	StubMsg msgq;

	DPRINTF("SetComponentVolume start\n");
	pbuf = (unsigned char *)malloc(SHM_HEADER_SIZE + sizeof(AudioDeviceVolumeSetReq) + sizeof(AudioStubClientSendReq));

	reqdata = (AudioStubClientSendReq *)(pbuf + SHM_HEADER_SIZE);
	reqdata->prodId = AUDIO_SETHWCPNTVOL_ID;
	reqdata->size = sizeof(AudioDeviceEnableReq);
	reqdata->pData = audioVolReq;

	memcpy((char *)(pbuf + SHM_HEADER_SIZE + sizeof(AudioStubClientSendReq) ),
	       audioVolReq, sizeof(AudioDeviceVolumeSetReq));
	if(gAudioInitStatus!=AUDIO_STUB_INIT_COMPLETE)
	{
		DPRINTF("AUDIO NOT INIT\n");
		free(pbuf);
		return STUB_ERROR;
	}
	audioSendMsgToServer(AUDIO_CLIENT_DATA_REQ, pbuf, sizeof(AudioDeviceVolumeSetReq) + sizeof(AudioStubClientSendReq));
	free(pbuf);

	do
	{
		status = OSAMsgQRecv(audioClientStubMsgQ, (UINT8 *)&msgq,  sizeof(StubMsg), MSGQ_TIMEOUT);
		if (status == OS_SUCCESS)
		{
			if (gMsgTimeoutCounts == 0)
			{
				memcpy(audioVolRsp, msgq.pData, sizeof(AudioDeviceVolumeSetRsp));
				ret = STUB_SUCCESS;
				free(msgq.pData);
				break;
			}
			else
			{
				/* received MSG is not what we want, ignore it */
				DPRINTF("Msg received dis-match, ignore it. timeout count = %d.\n", gMsgTimeoutCounts);
				if (gMsgTimeoutCounts > 0)
					gMsgTimeoutCounts--;
				free(msgq.pData);
			}
		}
		else if (status == OS_TIMEOUT)
		{
			DPRINTF("CPU/Msocket busy? OSAMsgQRecv timeout, return %d\n", status);
			ret = STUB_TIMEOUT;
			gMsgTimeoutCounts++;
			break;
		}
		else
		{
			DPRINTF("CPU/Msocket busy? OSAMsgQRecv fail, return %d\n", status);
			ret = STUB_ERROR;
			gMsgTimeoutCounts++;
			break;
		}
	} while(1);

	return ret;
}

/************************************************************************************
 *  *
 *  * Name:      HwComponentMute
 *  *
 *  * Description: This function tells the Comms processor to mute or un-mute the stream
 *  *        coming from the nearside microphone going to the radio (and out to the
 *  *        far side user).  This would be an indication to the Comms processor to
 *  *        start generating comfort noise.
 *  *
 *  * Parameters:
 *  *
 *  * Returns:     None
 *  *
 *  *
 *  **********************************************************************************/
STUB_ReturnCode HwComponentMute(AudioDeviceMuteReq *audioMuteReq, AudioDeviceMuteRsp *audioMuteRsp)
{
	OS_STATUS status;
	STUB_ReturnCode ret;
	unsigned char *pbuf;
	AudioStubClientSendReq *reqdata;
	StubMsg msgq;

	DPRINTF("HwComponentMute start\n");
	pbuf = (unsigned char *)malloc(SHM_HEADER_SIZE + sizeof(AudioDeviceMuteReq) + sizeof(AudioStubClientSendReq));

	reqdata = (AudioStubClientSendReq *)(pbuf + SHM_HEADER_SIZE);
	reqdata->prodId = AUDIO_HWCPNTMUTE_ID;
	reqdata->size = sizeof(AudioDeviceEnableReq);
	reqdata->pData = audioMuteReq;

	memcpy((char *)(pbuf + SHM_HEADER_SIZE + sizeof(AudioStubClientSendReq) ),
	       audioMuteReq, sizeof(AudioDeviceMuteReq));

	if(gAudioInitStatus!=AUDIO_STUB_INIT_COMPLETE)
	{
		DPRINTF("AUDIO NOT INIT\n");
		free(pbuf);
		return STUB_ERROR;
	}
	audioSendMsgToServer( AUDIO_CLIENT_DATA_REQ, pbuf, sizeof(AudioDeviceMuteReq) + sizeof(AudioStubClientSendReq) );
	free(pbuf);

	do
	{
		status = OSAMsgQRecv(audioClientStubMsgQ, (UINT8 *)&msgq,  sizeof(StubMsg), MSGQ_TIMEOUT);
		if (status == OS_SUCCESS)
		{
			if (gMsgTimeoutCounts == 0)
			{
				memcpy(audioMuteRsp, msgq.pData, sizeof(AudioDeviceMuteRsp));
				ret = STUB_SUCCESS;
				free(msgq.pData);
				break;
			}
			else
			{
				/* received MSG is not what we want, ignore it */
				DPRINTF("Msg received dis-match, ignore it. timeout count = %d.\n", gMsgTimeoutCounts);
				if (gMsgTimeoutCounts > 0)
					gMsgTimeoutCounts--;
				free(msgq.pData);
			}
		}
		else if (status == OS_TIMEOUT)
		{
			DPRINTF("CPU/Msocket busy? OSAMsgQRecv timeout, return %d\n", status);
			ret = STUB_TIMEOUT;
			gMsgTimeoutCounts++;
			break;
		}
		else
		{
			DPRINTF("CPU/Msocket busy? OSAMsgQRecv fail, return %d\n", status);
			ret = STUB_ERROR;
			gMsgTimeoutCounts++;
			break;
		}
	} while(1);

	return ret;
}


/************************************************************************************
 *  *
 *  * Name:      EnableOutStream
 *  *
 *  * Description: This function tells the Comms processor to enable streaming audio from
 *  *        a source to a destination.
 *  *
 *  * Parameters:
 *  * scenario:
 *  *
 *  * Returns:     None
 *  *
 *  *
 *  **********************************************************************************/
STUB_ReturnCode EnableOutStream(AudioStreamOutStartReq *streamOutStartReq, AudioStreamOutStartRsp *streamOutStartRsp)
{
	UNUSEDPARAM(streamOutStartRsp)

	OS_STATUS status;
	STUB_ReturnCode ret;
	unsigned char *pbuf;
	AudioStubClientSendReq *reqdata;
	StubMsg msgq;

	DPRINTF("EnableOutStream start\n");
	pbuf = (unsigned char *)malloc(SHM_HEADER_SIZE + sizeof(AudioStreamOutStartReq) + sizeof(AudioStubClientSendReq));

	reqdata = (AudioStubClientSendReq *)(pbuf + SHM_HEADER_SIZE);
	reqdata->prodId = AUDIO_ENABLEOUTSTREAM_ID;
	reqdata->size = sizeof(AudioStreamOutStartReq);
	reqdata->pData = streamOutStartReq;

	memcpy((char *)(pbuf + SHM_HEADER_SIZE + sizeof(AudioStubClientSendReq) ),
	       streamOutStartReq, sizeof(AudioStreamOutStartReq));
	if(gAudioInitStatus!=AUDIO_STUB_INIT_COMPLETE)
	{
		DPRINTF("AUDIO NOT INIT\n");
		free(pbuf);
		return STUB_ERROR;
	}
	audioSendMsgToServer(AUDIO_CLIENT_DATA_REQ, pbuf, sizeof(AudioStreamOutStartReq) + sizeof(AudioStubClientSendReq));
	free(pbuf);

	do
	{
		status = OSAMsgQRecv(audioClientStubMsgQ, (UINT8 *)&msgq,  sizeof(StubMsg), MSGQ_TIMEOUT);
		if (status == OS_SUCCESS)
		{
			if (gMsgTimeoutCounts == 0)
			{
				memcpy(streamOutStartRsp, msgq.pData, sizeof(AudioStreamOutStartRsp));
				ret = STUB_SUCCESS;
				free(msgq.pData);
				break;
			}
			else
			{
				/* received MSG is not what we want, ignore it */
				DPRINTF("Msg received dis-match, ignore it. timeout count = %d.\n", gMsgTimeoutCounts);
				if (gMsgTimeoutCounts > 0)
					gMsgTimeoutCounts--;
				free(msgq.pData);
			}
		}
		else if (status == OS_TIMEOUT)
		{
			DPRINTF("CPU/Msocket busy? OSAMsgQRecv timeout, return %d\n", status);
			ret = STUB_TIMEOUT;
			gMsgTimeoutCounts++;
			break;
		}
		else
		{
			DPRINTF("CPU/Msocket busy? OSAMsgQRecv fail, return %d\n", status);
			ret = STUB_ERROR;
			gMsgTimeoutCounts++;
			break;
		}
	} while(1);

	return ret;
}

/************************************************************************************
 *  *
 *  * Name:      DisableOutStream
 *  *
 *  * Description: This function tells the Comms processor to disable a specific stream.
 *  *
 *  * Parameters:
 *  *
 *  * Returns:     None
 *  *
 *  *
 *  **********************************************************************************/
STUB_ReturnCode DisableOutStream(AudioStreamOutStopReq *streamOutStopReq, AudioStreamOutStopRsp *streamOutStopRsp)
{
	UNUSEDPARAM(streamOutStopRsp)

	OS_STATUS status;
	STUB_ReturnCode ret;
	unsigned char *pbuf;
	AudioStubClientSendReq *reqdata;
	StubMsg msgq;

	DPRINTF("DisableOutStream start\n");

	pbuf = (unsigned char *)malloc(SHM_HEADER_SIZE + sizeof(AudioStreamOutStopReq) + sizeof(AudioStubClientSendReq));

	reqdata = (AudioStubClientSendReq *)(pbuf + SHM_HEADER_SIZE);
	reqdata->prodId = AUDIO_DISABLEOUTSTREAM_ID;
	reqdata->size = sizeof(AudioStreamOutStopReq);
	reqdata->pData = streamOutStopReq;

	memcpy((char *)(pbuf + SHM_HEADER_SIZE + sizeof(AudioStubClientSendReq) ),
	       streamOutStopReq, sizeof(AudioStreamOutStopReq));

	if(gAudioInitStatus!=AUDIO_STUB_INIT_COMPLETE)
	{
		DPRINTF("AUDIO NOT INIT\n");
		free(pbuf);
		return STUB_ERROR;
	}

	audioSendMsgToServer(AUDIO_CLIENT_DATA_REQ, pbuf, sizeof(AudioStreamOutStopReq) + sizeof(AudioStubClientSendReq));
	free(pbuf);

	do
	{
		status = OSAMsgQRecv(audioClientStubMsgQ, (UINT8 *)&msgq,  sizeof(StubMsg), MSGQ_TIMEOUT);
		if (status == OS_SUCCESS)
		{
			if (gMsgTimeoutCounts == 0)
			{
				memcpy(streamOutStopRsp, msgq.pData, sizeof(AudioStreamOutStopRsp));
				ret = STUB_SUCCESS;
				free(msgq.pData);
				break;
			}
			else
			{
				/* received MSG is not what we want, ignore it */
				DPRINTF("Msg received dis-match, ignore it. timeout count = %d.\n", gMsgTimeoutCounts);
				if (gMsgTimeoutCounts > 0)
					gMsgTimeoutCounts--;
				free(msgq.pData);
			}
		}
		else if (status == OS_TIMEOUT)
		{
			DPRINTF("CPU/Msocket busy? OSAMsgQRecv timeout, return %d\n", status);
			ret = STUB_TIMEOUT;
			gMsgTimeoutCounts++;
			break;
		}
		else
		{
			DPRINTF("CPU/Msocket busy? OSAMsgQRecv fail, return %d\n", status);
			ret = STUB_ERROR;
			gMsgTimeoutCounts++;
			break;
		}
	} while(1);

	return ret;
}

/************************************************************************************
 *  *
 *  * Name:      EnableInStream
 *  *
 *  * Description: This function tells the Comms processor to enable streaming audio from
 *  *        a source to a destination.
 *  *
 *  * Parameters:
 *  * scenario:
 *  *
 *  * Returns:     None
 *  *
 *  *
 *  **********************************************************************************/
STUB_ReturnCode EnableInStream(AudioStreamInStartReq *streamInStartReq, AudioStreamInStartRsp *streamInStartRsp)
{
	UNUSEDPARAM(streamInStartRsp)

	OS_STATUS status;
	STUB_ReturnCode ret;
	unsigned char *pbuf;
	AudioStubClientSendReq *reqdata;
	StubMsg msgq;

	DPRINTF("EnableInStream start\n");
	pbuf = (unsigned char *)malloc(SHM_HEADER_SIZE + sizeof(AudioStreamInStartReq) + sizeof(AudioStubClientSendReq));

	reqdata = (AudioStubClientSendReq *)(pbuf + SHM_HEADER_SIZE);
	reqdata->prodId = AUDIO_ENABLEINSTREAM_ID;
	reqdata->size = sizeof(AudioStreamInStartReq);
	reqdata->pData = streamInStartReq;

	memcpy((char *)(pbuf + SHM_HEADER_SIZE + sizeof(AudioStubClientSendReq) ),
	       streamInStartReq, sizeof(AudioStreamInStartReq));
	if(gAudioInitStatus!=AUDIO_STUB_INIT_COMPLETE)
	{
		DPRINTF("AUDIO NOT INIT\n");
		free(pbuf);
		return STUB_ERROR;
	}
	audioSendMsgToServer(AUDIO_CLIENT_DATA_REQ, pbuf, sizeof(AudioStreamInStartReq) + sizeof(AudioStubClientSendReq));
	free(pbuf);

	do
	{
		status = OSAMsgQRecv(audioClientStubMsgQ, (UINT8 *)&msgq,  sizeof(StubMsg), MSGQ_TIMEOUT);
		if (status == OS_SUCCESS)
		{
			if (gMsgTimeoutCounts == 0)
			{
				memcpy(streamInStartRsp, msgq.pData, sizeof(AudioStreamInStartRsp));
				ret = STUB_SUCCESS;
				free(msgq.pData);
				break;
			}
			else
			{
				/* received MSG is not what we want, ignore it */
				DPRINTF("Msg received dis-match, ignore it. timeout count = %d.\n", gMsgTimeoutCounts);
				if (gMsgTimeoutCounts > 0)
					gMsgTimeoutCounts--;
				free(msgq.pData);
			}
		}
		else if (status == OS_TIMEOUT)
		{
			DPRINTF("CPU/Msocket busy? OSAMsgQRecv timeout, return %d\n", status);
			ret = STUB_TIMEOUT;
			gMsgTimeoutCounts++;
			break;
		}
		else
		{
			DPRINTF("CPU/Msocket busy? OSAMsgQRecv fail, return %d\n", status);
			ret = STUB_ERROR;
			gMsgTimeoutCounts++;
			break;
		}
	} while(1);

	return ret;
}

/************************************************************************************
 *  *
 *  * Name:      DisableInStream
 *  *
 *  * Description: This function tells the Comms processor to disable a specific stream.
 *  *
 *  * Parameters:
 *  *
 *  *
 *  * Returns:     None
 *  *
 *  *
 *  **********************************************************************************/
STUB_ReturnCode DisableInStream(AudioStreamInStopReq *streamInStopReq, AudioStreamInStopRsp *streamInStopRsp)
{
	UNUSEDPARAM(streamInStopRsp)

	OS_STATUS status;
	STUB_ReturnCode ret;
	unsigned char *pbuf;
	AudioStubClientSendReq *reqdata;
	StubMsg msgq;

	DPRINTF("DisableInStream start\n");
	pbuf = (unsigned char *)malloc(SHM_HEADER_SIZE + sizeof(AudioStreamInStopReq) + sizeof(AudioStubClientSendReq));

	reqdata = (AudioStubClientSendReq *)(pbuf + SHM_HEADER_SIZE);
	reqdata->prodId = AUDIO_DISABLEINSTREAM_ID;
	reqdata->size = sizeof(AudioStreamInStopReq);
	reqdata->pData = streamInStopReq;

	memcpy((char *)(pbuf + SHM_HEADER_SIZE + sizeof(AudioStubClientSendReq) ),
	       streamInStopReq, sizeof(AudioStreamInStopReq));

	if(gAudioInitStatus!=AUDIO_STUB_INIT_COMPLETE)
	{
		DPRINTF("AUDIO NOT INIT\n");
		free(pbuf);
		return STUB_ERROR;
	}
	audioSendMsgToServer(AUDIO_CLIENT_DATA_REQ, pbuf, sizeof(AudioStreamInStopReq) + sizeof(AudioStubClientSendReq));
	free(pbuf);

	do
	{
		status = OSAMsgQRecv(audioClientStubMsgQ, (UINT8 *)&msgq,  sizeof(StubMsg), MSGQ_TIMEOUT);
		if (status == OS_SUCCESS)
		{
			if (gMsgTimeoutCounts == 0)
			{
				memcpy(streamInStopRsp, msgq.pData, sizeof(AudioStreamInStopRsp));
				ret = STUB_SUCCESS;
				free(msgq.pData);
				break;
			}
			else
			{
				/* received MSG is not what we want, ignore it */
				DPRINTF("Msg received dis-match, ignore it. timeout count = %d.\n", gMsgTimeoutCounts);
				if (gMsgTimeoutCounts > 0)
					gMsgTimeoutCounts--;
				free(msgq.pData);
			}
		}
		else if (status == OS_TIMEOUT)
		{
			DPRINTF("CPU/Msocket busy? OSAMsgQRecv timeout, return %d\n", status);
			ret = STUB_TIMEOUT;
			gMsgTimeoutCounts++;
			break;
		}
		else
		{
			DPRINTF("CPU/Msocket busy? OSAMsgQRecv fail, return %d\n", status);
			ret = STUB_ERROR;
			gMsgTimeoutCounts++;
			break;
		}
	} while(1);

	return ret;
}

STUB_ReturnCode StreamIndResp(DOWNLINKSTREAM_REQUEST *downlinkReq)
{
	unsigned char *pbuf;
	AudioStubClientSendReq *reqdata;

	//DPRINTF("StreamIndResp start, data size is %d\n", downlinkReq->streamInd[0].dataSize);
	pbuf = (unsigned char *)malloc(SHM_HEADER_SIZE + sizeof(DOWNLINKSTREAM_REQUEST)
			      + sizeof(AudioStubClientSendReq) + downlinkReq->streamInd[0].dataSize);

	reqdata = (AudioStubClientSendReq *)(pbuf + SHM_HEADER_SIZE);
	reqdata->prodId = AUDIO_STREAMRESPONSE;
	reqdata->size = sizeof(DOWNLINKSTREAM_REQUEST) + downlinkReq->streamInd[0].dataSize;
	reqdata->pData = downlinkReq;

	memcpy((char *)(pbuf + SHM_HEADER_SIZE + sizeof(AudioStubClientSendReq) ),
	       downlinkReq, sizeof(DOWNLINKSTREAM_REQUEST));
	memcpy((char *)(pbuf + SHM_HEADER_SIZE + sizeof(AudioStubClientSendReq) + sizeof(DOWNLINKSTREAM_REQUEST)),
	       downlinkReq->streamInd[0].data, downlinkReq->streamInd[0].dataSize);
	if(gAudioInitStatus!=AUDIO_STUB_INIT_COMPLETE)
	{
		DPRINTF("AUDIO NOT INIT\n");
		free(pbuf);
		return STUB_ERROR;
	}
	audioSendMsgToServer(AUDIO_CLIENT_DATA_REQ, pbuf, sizeof(DOWNLINKSTREAM_REQUEST) + sizeof(AudioStubClientSendReq) + downlinkReq->streamInd[0].dataSize);
	free(pbuf);

	return STUB_SUCCESS;
}

STUB_ReturnCode StreamIndicate(UPLINKSTREAM_REQUEST *uplinkReq)
{
	int index;
	APPSStreamHandle_t streamHandle;
	UINT32 *data;
	UINT32 dataSize;

	//DPRINTF("StreamIndicate start, handle_num %d\n", uplinkReq->handle_num);
	if (uplinkReq->handle_num > 0)
	{
		for (index = 0; index < uplinkReq->handle_num; index++)
		{
			streamHandle = uplinkReq->streamInd[index].streamHandle;
			dataSize = uplinkReq->streamInd[index].dataSize;
			//DPRINTF("StreamIndicate start, index %d, data %d\n", index, uplinkReq->streamInd[index].data);
			if (NULL == uplinkReq->streamInd[index].data)
			{
				data = malloc(ACMAPPS_STREAM_SIZE * sizeof(unsigned short));
				memset( data, '\0', ACMAPPS_STREAM_SIZE * sizeof(unsigned short));
				AudioPutDownLinkStream(streamHandle, data, dataSize);
				free(data);
			}
			else
			{
				data = (UINT32 *)uplinkReq->streamInd[index].data;
				AudioGetUpLinkStream(streamHandle, data, dataSize);
			}
		}
	}
	return STUB_SUCCESS;
}



/************************************************************************************
 *  *
 *  * Name:      SetMSASetting
 *  *
 *  * Description: This function send the MSA settings to CP side.
 *  *
 *  * Parameters:
 *  * scenario:
 *  *
 *  * Returns:     None
 *  *
 *  *
 *  **********************************************************************************/
STUB_ReturnCode SetMSASetting(ACMAudioDSPSettings *audioMSASettingsReq, ACMAudioDSPSettings *audioMSASettingsRsp)
{
	UNUSEDPARAM(audioMSASettingsRsp)

	unsigned char *pbuf;
	AudioStubClientSendReq *reqdata;

	DPRINTF("SetMSASetting start\n");
	pbuf = (unsigned char *)malloc(SHM_HEADER_SIZE + sizeof(ACMAudioDSPSettings) + sizeof(AudioStubClientSendReq));

	reqdata = (AudioStubClientSendReq *)(pbuf + SHM_HEADER_SIZE);
	reqdata->prodId = AUDIO_SETMSASETTING_ID;
	reqdata->size = sizeof(ACMAudioDSPSettings);
	reqdata->pData = audioMSASettingsReq;

	memcpy((char *)(pbuf + SHM_HEADER_SIZE + sizeof(AudioStubClientSendReq) ),
	       audioMSASettingsReq, sizeof(ACMAudioDSPSettings));
	if(gAudioInitStatus!=AUDIO_STUB_INIT_COMPLETE)
	{
		DPRINTF("AUDIO NOT INIT\n");
		free(pbuf);
		return STUB_ERROR;
	}
	audioSendMsgToServer(AUDIO_CLIENT_DATA_REQ, pbuf, sizeof(ACMAudioDSPSettings) + sizeof(AudioStubClientSendReq));
	free(pbuf);

	return STUB_SUCCESS;
}


/************************************************************************************
 *  *
 *  * Name:      SyncGainSetting
 *  *
 *  * Description: This function send/sync the Gain settings to CP side.
 *  *
 *  * Parameters: pACMGains, The gain settings to sync
 *  * Parameters: len, The length of gain settings, in bytes
 *  * scenario:
 *  *
 *  * Returns:     None
 *  *
 *  *
 *  **********************************************************************************/
STUB_ReturnCode SyncGainSetting(ACM_SYNC_Gain *pACMGains, int len)
{
	unsigned char *pbuf;
	AudioStubClientSendReq *reqdata;

	pbuf = (unsigned char *)malloc(SHM_HEADER_SIZE + len + sizeof(AudioStubClientSendReq));

	reqdata = (AudioStubClientSendReq *)(pbuf + SHM_HEADER_SIZE);
	reqdata->prodId = AUDIO_SYNC_CODEC_ID;
	reqdata->size = len;
	reqdata->pData = pACMGains;

	memcpy((char *)(pbuf + SHM_HEADER_SIZE + sizeof(AudioStubClientSendReq) ),
	       pACMGains, len);
	if(gAudioInitStatus!=AUDIO_STUB_INIT_COMPLETE)
	{
		DBGMSG("AUDIO NOT INIT\n");
		free(pbuf);
		return STUB_ERROR;
	}
	int ret = audioSendMsgToServer(AUDIO_CLIENT_DATA_REQ, pbuf, len + sizeof(AudioStubClientSendReq));
	DBGMSG("SyncGainSetting: len is %d, ret is %d\n", len , ret);
	free(pbuf);

	return STUB_SUCCESS;
}

