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
 * **  FILENAME:       audio_stub_api.h
 * **
 * **  PURPOSE:        stub api functions used by the application to make Asynchronous calls
 * ******************************************************************************/
#ifndef _AUDIO_STUB_API_H_
#define _AUDIO_STUB_API_H_
//#include "audio_stub_ttc.h"
//pro IDs

#include <acm.h>
#include <acm_stub.h>

#define ACMAPPS_STREAM_SIZE 160

enum
{
	/* AudioStub procId */

	AUDIO_STUB_START = 1,
	AUDIO_STUB_CONFIRM,
	AUDIO_CLIENT_DATA_REQ,
	AUDIO_CLIENT_SERVER_DATA_CNF_IND = 5,
};

enum
{
	/* ACM ProcId */
	AUDIO_STUB_START_ID = 100,
	AUDIO_REGCPNT_ID,
	AUDIO_REGCPNT_CONFIRM,

	AUDIO_ENABLEHWCPNT_ID,
	AUDIO_ENABLEHWCPNT_CONFIRM,

	AUDIO_DISABLEHWCPNT_ID,
	AUDIO_DISABLEHWCPNT_CONFIRM,

	AUDIO_SETHWCPNTVOL_ID,
	AUDIO_SETHWCPNTVOL_CONFIRM,

	AUDIO_HWCPNTMUTE_ID,
	AUDIO_HWCPNTMUTE_CONFIRM,

//	AUDIO_REGISTERCALLBACK_ID,
//	AUDIO_REGISTERCALLBACK_CONFIRM,

	AUDIO_STREAMINDICATE_ID,
	AUDIO_STREAMRESPONSE,

	AUDIO_ENABLEOUTSTREAM_ID,
	AUDIO_ENABLEOUTSTREAM_CONFIRM,

	AUDIO_DISABLEOUTSTREAM_ID,
	AUDIO_DISABLEOUTSTREAM_CONFIRM,

	AUDIO_ENABLEINSTREAM_ID,
	AUDIO_ENABLEINSTREAM_CONFIRM,

	AUDIO_DISABLEINSTREAM_ID,
	AUDIO_DISABLEINSTREAM_CONFIRM,

	/*Jackie, for enabling/disabling the audio at the begining/ending of a call */
	AUDIO_CONVERSATIONSTART_ID,        //From COMM to APPS
	AUDIO_CONVERSATIONSTOP_ID,	      //From COMM to APPS

	/*
	*Jackie, 2010-0518
	* APPS audio send AUDIO_SETMSASETTING_ID to control MSA
	* COMM audio send AUDIO_GETMSASETTING_ID to APPS audio, APPS can refresh its audio calibration UI
	*/
	AUDIO_SETMSASETTING_ID,
	AUDIO_GETMSASETTING_ID,

	// Sync gain information of codec chip
	AUDIO_SYNC_CODEC_ID,

	AUDIO_PROC_ID_NEXTAVAIL,
};

#if 0
STUB_ReturnCode registerAudioComponent(ACM_Component component, void *componentCB);
STUB_ReturnCode EnableHwComponent(AudioControlReq *audioCtlReq, void *controlCB);
STUB_ReturnCode DisableHwComponent(AudioControlReq *audioCtlReq, void *controlCB);
STUB_ReturnCode SetComponentVolume(AudioControlReq *audioCtlReq, void *controlCB);
STUB_ReturnCode HwComponentMute(AudioControlReq *audioCtlReq, void *controlCB);
STUB_ReturnCode EnableOutStream(AudioStreamInOutReq *streamInOutReq, void *controlCB);
STUB_ReturnCode DisableOutStream(AudioStreamInOutReq *streamInOutReq, void *controlCB);
STUB_ReturnCode EnableInStream(AudioStreamInOutReq *streamInOutReq, void *controlCB);
STUB_ReturnCode DisableInStream(AudioStreamInOutReq *streamInOutReq, void *controlCB);
#endif

STUB_ReturnCode EnableHwComponent(AudioDeviceEnableReq *audioEnableReq, AudioDeviceEnableRsp *audioEnableRsp);
STUB_ReturnCode DisableHwComponent(AudioDeviceDisableReq *audioDisableReq, AudioDeviceDisableRsp *audioDisableRsp);
STUB_ReturnCode SetComponentVolume(AudioDeviceVolumeSetReq *audioVolReq, AudioDeviceVolumeSetRsp *audioVolRsp);
STUB_ReturnCode HwComponentMute(AudioDeviceMuteReq *audioMuteReq, AudioDeviceMuteRsp *audioMuteRsp);
STUB_ReturnCode EnableOutStream(AudioStreamOutStartReq *streamOutStartReq, AudioStreamOutStartRsp *streamOutStartRsp);
STUB_ReturnCode DisableOutStream(AudioStreamOutStopReq *streamOutStopReq, AudioStreamOutStopRsp *streamOutStopRsp);
STUB_ReturnCode EnableInStream(AudioStreamInStartReq *streamInStartReq, AudioStreamInStartRsp *streamInStartRsp);
STUB_ReturnCode DisableInStream(AudioStreamInStopReq *streamInStopReq, AudioStreamInStopRsp *streamInStopRsp);
STUB_ReturnCode StreamIndicate(UPLINKSTREAM_REQUEST *uplinkReq);
STUB_ReturnCode RegisterHwComponent(ACMAudioComponentRegisterReq * registerReq, ACMAudioComponentRegisterRsp * registerRsp);
STUB_ReturnCode InitAudioStub();
STUB_ReturnCode SetMSASetting(ACMAudioDSPSettings *audioMSASettingsReq, ACMAudioDSPSettings *audioMSASettingsRsp);
STUB_ReturnCode SyncGainSetting(ACM_SYNC_Gain *pACMGains, int len);
STUB_ReturnCode StreamIndResp(DOWNLINKSTREAM_REQUEST *downlinkReq);

//call back functions

//void*  gAudioConfirmFuncPtr[AUDIO_PROC_ID_NEXTAVAIL];
//static AudioStreamInd  gAudioStreamIndFuncPtr;

//void clientAudioConfirmCallback(unsigned short procId, AudioControlCnf* pArg, void *paras);
//void clientAudioStreamInOutCallback(unsigned short procId, AudioStreamInOutCnf* pArg, void *paras);
#endif
