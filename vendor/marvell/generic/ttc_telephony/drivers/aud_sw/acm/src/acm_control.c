/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code (?Material? are owned
   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
   transmitted, distributed, or disclosed in any way without Intel?s prior express written permission.

   No license under any patent, copyright, trade secret or other intellectual property right is granted to or
   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
   Intel in writing.
   -------------------------------------------------------------------------------------------------------------------*/

/******************************************************************************
*               MODULE IMPLEMENTATION FILE
*******************************************************************************
* Title: ACM (Audio Component Manager)
*
* Filename: acm_control.c
*
* Target, platform: Common Platform, SW platform
*
* Authors: Paul Shen
*
* Description: Controls the h/w access of all audio devices.
*
* Last Updated: Jun 11 2009
*
* Notes:
******************************************************************************/

/*----------- External include files -----------------------------------------*/
#include <stdlib.h>
#include "acm.h"
#include "acmtypes.h"
#include "acm_nvm.h"
#include "acm_device.h"
#include "acm_stub.h"
#include "audio_stub_api_ttc.h"
#include <pxa_dbg.h>

AudioControlReq control_req;
AudioControlCnf control_cnf;


/*----------- Local macro definitions ----------------------------------------*/
#define MASK8(a)        ((unsigned char)((a) & 0x000000FF))

#define ACM_NOT_CALL_ACMMUTEPATH  (0xff)       //0:not mute; 1: mute; other value: no call to ACMMutePath in AP

#define BIT_9_INDICATE_RECORD_RATE_CHANGE  (0x200)    /*bit 9 indicates record sample rate has been changed*/
#define BIT_8_INDICATE_PLAYBACK_RATE_CHANGE  (0x100)  /*bit 8 indicates playback sample rate has been changed*/

/*----------- Global constant definitions ------------------------------------*/
/*----------- Local variable definitions -------------------------------------*/
static ACM_DeviceHandleLocal *_deviceTable;
static ACM_DeviceHandle*     _deviceNvmTable;
static unsigned int _deviceNvmTableSize;
static ACM_AudioCalibrationStatus _calibrationstatus; /* 0 = normal status, 1 = calibration status*/

/*hard coding*/
ACM_Component ACMAPPSComponent = LEVANTE_COMPONENT;

const ACM_ComponentHandle* componentHandles[] = //Must be according to Component enum
{
	&HWNullHandle,
	&HWNullHandle,
	&HWNullHandle,
	&HWNullHandle,
	&HWNullHandle,
	&HWBlueToothHandle,
	&HWNullHandle,
	&HWNullHandle,
	&HWNullHandle,
	&HWLevanteHandle,
	&HWNullHandle
};
/*----------- Global extrenal variable definition -------------------------------------*/
ACM_SspAudioConfiguration *componentSspConfig[] =
{
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

short *ditherGenConfig[] =
{
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};


/*----------- Extern function definition -------------------------------------*/
extern void StatusCallBackFunc(unsigned long appHandle, unsigned int primitive, unsigned status, unsigned int value);

/*----------  Internal function definition -----------------------------------*/
static ACM_AudioCalibrationStatus ACMAudioGetCalibrationStatus(void);

/*******************************************************************************
* Function: ACMBspGetBoardTables
*******************************************************************************
* Description: Get ACM Tables according to BSP board type
*
* Parameters: none
*
* Return value: void
*
* Notes:
*******************************************************************************/
static void ACMBspGetBoardTables(void)
{
	DPRINTF("ACMBspGetBoardTables\n");
#if 0
	_deviceNvmTable = (ACM_DeviceHandle *)deviceTableMicco_TTCEVB;
	_deviceNvmTableSize = sizeof(deviceTableMicco_TTCEVB);
#else
	_deviceNvmTable = (ACM_DeviceHandle *)deviceTableLevante_TTCDKB;
	_deviceNvmTableSize = sizeof(deviceTableLevante_TTCDKB);

#endif
}



ACMAudioComponentRegisterReq regReq;
ACMAudioComponentRegisterRsp regRsp;
/*******************************************************************************
* Function: ACMControlInit
*******************************************************************************
* Description: Read ACM Tables from FDI
*
* Parameters: none
*
* Return value: void
*
* Notes:
*******************************************************************************/
static void ACMControlInit(void)
{
	ACM_DeviceHandleLocal *localHandle_p;
	ACM_DeviceHandleLocal *localOrgHandle_p;
	int initLocalComponent[NULL_COMPONENT];
	int tablelist_size;
	int i;

	DPRINTF("ACMControlInit\n");
	//get specific board device mapping default tables
	ACMBspGetBoardTables();
	tablelist_size = _deviceNvmTableSize / sizeof(ACM_DeviceHandle);

	//Init the device table here
	localOrgHandle_p =  (ACM_DeviceHandleLocal *)malloc(tablelist_size * sizeof(ACM_DeviceHandleLocal) );
	localHandle_p = localOrgHandle_p;

	for (i = 0; i < tablelist_size; i++)
	{
		//_deviceNvmTable[i].component, _deviceNvmTable[i].path);
		memcpy(&localHandle_p->deviceHandle, &_deviceNvmTable[i], sizeof(ACM_DeviceHandle) );

		localHandle_p++;
	}

	/* Reset the component init flag array */
	memset(initLocalComponent, 0, (NULL_COMPONENT * sizeof(int)));

	localHandle_p = localOrgHandle_p;
	while ( (localHandle_p->deviceHandle.component != NULL_COMPONENT) )
	{
		if (initLocalComponent[localHandle_p->deviceHandle.component] == 0 )
		{
			componentHandles[localHandle_p->deviceHandle.component]->ACMComponentInit(0, &componentSspConfig[localHandle_p->deviceHandle.component], &ditherGenConfig[localHandle_p->deviceHandle.component]);
			initLocalComponent[localHandle_p->deviceHandle.component] = 1;
		}

		localHandle_p++;
	}

	_deviceTable = localOrgHandle_p;
	_calibrationstatus = ACM_CALIBRATION_OFF;

	LevanteFrequencySettingsData frequencySettingsData = { HIFI, PLAY, BOTH_SIDES, LEVANTE_BIT_RATE_44_1_KHZ };
	LevanteAdditionalSettingsSet(FREQUENCY_SETTINGS, (void *)&frequencySettingsData);
} /* End of ACMControlInit */


/*******************************************************************************
* Function: ACMSwitchingSampleRate
*******************************************************************************
* Description: dynamically switch sample rate, betwen 44.1k and 8k
*
* Parameters: int rate
*
* Return value: ACM_ReturnCode
*
* Notes:
*******************************************************************************/
ACM_ReturnCode ACMSwitchingSampleRate(int rate)
{
	// before switching sample rate, check whether all Levante audio path has
	// been closed, if not, forbid sample rate switch
	unsigned int pathNum = componentHandles[LEVANTE_COMPONENT]->ACMGetActivePathNum();
	int high, low;

	high = rate & 0x00000300; /*the bit 9 means if record sample rate is changed, the bit 8 means if playback sample rate is changed */
	low = rate & 0x000000FF;

	DPRINTF("ACMSwitchSampleRate, high = 0x%x, low = 0x%x\n", high, low);

	if (high & BIT_9_INDICATE_RECORD_RATE_CHANGE)             /*record sample rate has been changed*/
	{
		if (((low & 0xF0) >> 4) == LEVANTE_BIT_RATE_8_KHZ)
		{
			LevanteFrequencySettingsData frequencySettingsData = { LOFI, RECORD, BOTH_SIDES, LEVANTE_BIT_RATE_8_KHZ };
			LevanteAdditionalSettingsSet(FREQUENCY_SETTINGS, (void *)&frequencySettingsData);
		}
		else if (((low & 0xF0) >> 4) == LEVANTE_BIT_RATE_16_KHZ)
		{
			LevanteFrequencySettingsData frequencySettingsData = { LOFI, RECORD, BOTH_SIDES, LEVANTE_BIT_RATE_16_KHZ };
			LevanteAdditionalSettingsSet(FREQUENCY_SETTINGS, (void *)&frequencySettingsData);
		}
		else if (((low & 0xF0) >> 4) == LEVANTE_BIT_RATE_32_KHZ)
		{
			LevanteFrequencySettingsData frequencySettingsData = { LOFI, RECORD, BOTH_SIDES, LEVANTE_BIT_RATE_32_KHZ };
			LevanteAdditionalSettingsSet(FREQUENCY_SETTINGS, (void *)&frequencySettingsData);
		}
		else if (((low & 0xF0) >> 4) == LEVANTE_BIT_RATE_48_KHZ)
		{
			LevanteFrequencySettingsData frequencySettingsData = { LOFI, RECORD, BOTH_SIDES, LEVANTE_BIT_RATE_48_KHZ };
			LevanteAdditionalSettingsSet(FREQUENCY_SETTINGS, (void *)&frequencySettingsData);
		}
		else
			DPRINTF("ACMSwitchSampleRate, don't support other sample rate except 8k, 16k, 32k, 48k for ADC\n");
	}

	if (high & BIT_8_INDICATE_PLAYBACK_RATE_CHANGE)    /*play back sample rate has been changed*/
	{
		if (pathNum != 0)
		{
			DPRINTF("Disable all Levante audio path before switching playback sample rate, active path num %d!\n", pathNum);
			return ACM_RC_NEED_DISABLE_PATH;
		}

		if ((low & 0xF) == LEVANTE_BIT_RATE_8_KHZ)
		{
			LevanteFrequencySettingsData frequencySettingsData = { HIFI, PLAY, BOTH_SIDES, LEVANTE_BIT_RATE_8_KHZ };
			LevanteAdditionalSettingsSet(FREQUENCY_SETTINGS, (void *)&frequencySettingsData);
		}
		else if ((low & 0xF) == LEVANTE_BIT_RATE_16_KHZ)
		{
			LevanteFrequencySettingsData frequencySettingsData = { HIFI, PLAY, BOTH_SIDES, LEVANTE_BIT_RATE_16_KHZ };
			LevanteAdditionalSettingsSet(FREQUENCY_SETTINGS, (void *)&frequencySettingsData);
		}
		else if ((low & 0xF) == LEVANTE_BIT_RATE_44_1_KHZ)
		{
			LevanteFrequencySettingsData frequencySettingsData = { HIFI, PLAY, BOTH_SIDES, LEVANTE_BIT_RATE_44_1_KHZ };
			LevanteAdditionalSettingsSet(FREQUENCY_SETTINGS, (void *)&frequencySettingsData);
		}
		else if ((low & 0xF) == LEVANTE_BIT_RATE_48_KHZ)
		{
			LevanteFrequencySettingsData frequencySettingsData = { HIFI, PLAY, BOTH_SIDES, LEVANTE_BIT_RATE_48_KHZ };
			LevanteAdditionalSettingsSet(FREQUENCY_SETTINGS, (void *)&frequencySettingsData);
		}
		else
			DPRINTF("ACMSwitchSampleRate, don't support other sample rate except 8k, 16k, 44.1k, 48k for DAC\n");
	}

	return ACM_RC_OK;
}


/*******************************************************************************
* Function: ACMRegisterComponent
*******************************************************************************
* Description: Register ACM component to CP, audio stub will call this function after CP restarts
*
* Parameters: none
*
* Return value: void
*
* Notes:
*******************************************************************************/
void ACMRegisterComponent(void)
{
	STUB_ReturnCode stubReturn;

	regReq.component = LEVANTE_COMPONENT;
	stubReturn = RegisterHwComponent(&regReq, &regRsp);

	if (stubReturn != STUB_SUCCESS)
	{
		DPRINTF("ACMRegisterComponent failed\n");
	}
	DPRINTF("ACMRegisterComponent done\n");
} /* End of ACMControlInit */


/*******************************************************************************
* Function: ACMSyncGainDefaultSetting
*******************************************************************************
* Description: Sync audiolevante.c {slope, offset} with CP side
*
* Parameters: none
*
* Return value: void
*
* Notes:
*******************************************************************************/
void ACMSyncGainDefaultSetting(void)
{
	STUB_ReturnCode stubReturn;

	ACM_SYNC_Gain *pACMGain = NULL;
	int len = 0;

	LevanteGetGainData((void **)&pACMGain, &len);

	stubReturn = SyncGainSetting(pACMGain, len * sizeof(ACM_SYNC_Gain));

	if (stubReturn != STUB_SUCCESS)
	{
		DPRINTF("ACMSyncGainDefaultSetting failed\n");
	}
	free(pACMGain);

	DPRINTF("ACMSyncGainDefaultSetting done\n");
} /* End of ACMControlInit */


/*******************************************************************************
* Function: ACMAudioDeviceMute
*******************************************************************************
* Description: Analog mutes or un-mutes the device passed.
*               In case of ACM_MSA_PCM format:
*                   un-mute ==> perform it.
*                   mute ==> perform it only if the device is the only active
*			form it.
*                   mute ==> perform it only if the device is the only active
*                       non-muted device on the relevant path (in / out).
*
* Parameters: ACM_AudioDevice device
*             ACM_AudioFormat format
*             ACM_AudioMute   mute
*
* Return value: ACM_ReturnCode
*
* Notes:
*******************************************************************************/
AudioDeviceMuteReq audioMuteReq;
AudioDeviceMuteRsp audioMuteRsp;

ACM_ReturnCode ACMAudioDeviceMute(ACM_AudioDevice device,
				  ACM_AudioFormat format,
				  ACM_AudioMute mute)
{
	ACM_DeviceHandleLocal   *handle_p;
	ACM_ReturnCode acmReturnCode = ACM_RC_DEVICE_FORMAT_NOT_FOUND;

	STUB_ReturnCode stubReturn;

	DPRINTF("ACMAudioDeviceMute API called: device= %d, format= %d, mute= %d\n", (int)device, (int)format, (int)mute);

	ACM_Component ACMComponent = NULL_COMPONENT;
	handle_p = _deviceTable;

	if (format != ACM_MSA_PCM)
	{
		DPRINTF("ACMAudioDeviceMute: not PCM path so do not notify CP\n\n");
		handle_p = _deviceTable;

		if (ACMAudioGetCalibrationStatus() == ACM_CALIBRATION_ON)
		{
			DPRINTF("Calibration status: forward hifi path to CP\n\n");
			audioMuteReq.device = device;
			audioMuteReq.format = format;
			audioMuteReq.mute		= mute;
			stubReturn = HwComponentMute(&audioMuteReq, &audioMuteRsp);
		}

		while (handle_p->deviceHandle.device != ACM_NOT_CONNECTED)
		{
			if ( (handle_p->deviceHandle.device == device) && (handle_p->deviceHandle.format == format) )
			{
				componentHandles[handle_p->deviceHandle.component]->ACMMutePath(
					MASK8(handle_p->deviceHandle.path),
					mute);
			}

			handle_p++;
		}

		return ACM_RC_OK;
	}

	while (handle_p->deviceHandle.device != ACM_NOT_CONNECTED)
	{
		if ( (handle_p->deviceHandle.device == device) && (handle_p->deviceHandle.format == format) )
		{
			ACMComponent = handle_p->deviceHandle.component;
			break;
		}

		handle_p++;
	}
	if (ACMComponent == NULL_COMPONENT)
	{
		DPRINTF("ACMAudioDeviceMute API : device not found\n");
		return acmReturnCode;
	}

	audioMuteReq.device = device;
	audioMuteReq.format = format;
	audioMuteReq.mute   = mute;
	stubReturn = HwComponentMute(&audioMuteReq, &audioMuteRsp);

	if (stubReturn == STUB_SUCCESS)
	{
		ACM_ReturnCode acmReturnCode = ACM_RC_DEVICE_FORMAT_NOT_FOUND;
		ACM_AudioDevice device;
		ACM_AudioFormat format;
		GAIN_RECORD PGAGains[ACM_MAXPGACNT_PERPATH];
		UINT32 PGAGainIndex, PGAGainCnt;

		//fill audioMuteRsp data
		acmReturnCode = (ACM_ReturnCode)audioMuteRsp.status;
		device = audioMuteRsp.device;
		format = audioMuteRsp.format;
		PGAGainCnt = audioMuteRsp.gains.gain_nums;
		memcpy((unsigned char *)PGAGains, (unsigned char *)&(audioMuteRsp.gains.gain_record[0]), sizeof(PGAGains));

		for (PGAGainIndex = 0; PGAGainIndex < PGAGainCnt; PGAGainIndex++)
		{
			/********************************************************************************
			   For each path returned, a single DIGITAL_GAIN_MAGICNUM will be returned from ACM_COMM.
			   And there may be more than one PGA gain for each path.

			   So, we can check whether gain_id==DIGITAL_GAIN_MAGICNUM, to get the path.
			   Then we can configure codec using the path.
			 *********************************************************************************/
			if (DIGITAL_GAIN_MAGICNUM == PGAGains[PGAGainIndex].gain_id)
			{
				/**********************************************************
					Set PMIC path returned from ACM_COMM.

					Because PMIC path is maintained by ACM_COMM, ACMAPPS APIs
					such as ACMAPPSAudioDeviceEnable will not know PMIC path.
				 ***********************************************************/
				/*********************************************************************************
				   Not all codec paths need to mute.

				   In acm_nvm.h, ACM_MUTE_IN_MSA means not need to mute codec path in AP.
				   AM_COMM manage the table, when it found ACM_MUTE_IN_COMPONENT/ACM_MUTE_IN_COMPONENT_AND_MSA
				   0/1 will be returned, which means unmute/mute.
				   Otherwise,ACM_NOT_CALL_ACMMUTEPATH will be returned.
				 **********************************************************************************/
				if (PGAGains[PGAGainIndex].mutePath != ACM_NOT_CALL_ACMMUTEPATH)
					componentHandles[ACMComponent]->ACMMutePath(PGAGains[PGAGainIndex].path, (ACM_AudioMute)PGAGains[PGAGainIndex].mutePath);
			}
			else
			{
				//DPRINTF("First mute path, then set gain from CP, to ensure not to replace the gain with AP side again\n");
			}

		}

		for (PGAGainIndex = 0; PGAGainIndex < PGAGainCnt; PGAGainIndex++)
		{
			if (DIGITAL_GAIN_MAGICNUM == PGAGains[PGAGainIndex].gain_id)
			{
			}
			else
			{
				DPRINTF("HwComponentMute, DONOT need to set gain, (path,gainId,gain,regIndex)= (%lu, %lu, %lu, %d)\n", PGAGains[PGAGainIndex].path, PGAGains[PGAGainIndex].gain_id, PGAGains[PGAGainIndex].gain_val, PGAGains[PGAGainIndex].regIndex);
				//componentHandles[ACMComponent]->ACMSetPathAnalogGain(PGAGains[PGAGainIndex].regIndex, PGAGains[PGAGainIndex].gain_val, PGAGains[PGAGainIndex].gainMask, PGAGains[PGAGainIndex].gainShift);
			}

		}

	}
	else if (stubReturn == STUB_TIMEOUT)
	{
		DPRINTF("HwComponentMute timeout, AP do default setting\n\n");
		handle_p = _deviceTable;

		while (handle_p->deviceHandle.device != ACM_NOT_CONNECTED)
		{
			if ( (handle_p->deviceHandle.device == device) && (handle_p->deviceHandle.format == format) )
			{
				componentHandles[handle_p->deviceHandle.component]->ACMMutePath(
					MASK8(handle_p->deviceHandle.path),
					mute);
			}

			handle_p++;
		}
	}
	else
	{
		DPRINTF("HwComponentMute error, return %d\n\n", stubReturn);
		handle_p = _deviceTable;

		while (handle_p->deviceHandle.device != ACM_NOT_CONNECTED)
		{
			if ( (handle_p->deviceHandle.device == device) && (handle_p->deviceHandle.format == format) )
			{
				componentHandles[handle_p->deviceHandle.component]->ACMMutePath(
					MASK8(handle_p->deviceHandle.path),
					mute);
			}

			handle_p++;
		}
	}


	acmReturnCode = ACM_RC_OK;
	return acmReturnCode;
} /* End of ACMAudioDeviceMute */



/*******************************************************************************
* Function: ACMAudioDeviceVolumeSet
*******************************************************************************
* Description: Sets the analog volume of the device.
*               In case of ACM_MSA_PCM format, the digital volume is set by the
*               function digitalVolumeSet() according to certain specifications
*               (see description of digitalVolumeSet() above).
*
* Parameters: ACMAudioDevice device
*             ACMAudioFormat format
*             ACMAudioVolume volume
*
* Return value: ACM_ReturnCode
*
* Notes:
*******************************************************************************/

AudioDeviceVolumeSetReq audioVolReq;
AudioDeviceVolumeSetRsp audioVolRsp;

ACM_ReturnCode ACMAudioDeviceVolumeSet(ACM_AudioDevice device,
				       ACM_AudioFormat format,
				       ACM_AudioVolume volume)
{
	ACM_DeviceHandleLocal  *handle_p;
	ACM_ReturnCode acmReturnCode = ACM_RC_DEVICE_FORMAT_NOT_FOUND;
	STUB_ReturnCode stubReturn;

	DPRINTF("ACMAudioDeviceVolumeSet API called: device=%d,format=%d,volume=%d\n", (int)device, (int)format, volume);

	ACM_Component ACMComponent = NULL_COMPONENT;
	handle_p = _deviceTable;

	if (format != ACM_MSA_PCM)
	{
		DPRINTF("ACMAudioDeviceVolumeSet: not PCM path so do not notify CP\n\n");
		handle_p = _deviceTable;

		if (ACMAudioGetCalibrationStatus() == ACM_CALIBRATION_ON)
		{
			DPRINTF("Calibration status: forward hifi path to CP\n\n");
			audioVolReq.device = device;
			audioVolReq.format = format;
			audioVolReq.volume = volume;
			stubReturn = SetComponentVolume(&audioVolReq, &audioVolRsp);
		}

		while (handle_p->deviceHandle.device != ACM_NOT_CONNECTED)
		{
			if ( (handle_p->deviceHandle.device == device) && (handle_p->deviceHandle.format == format) )
			{
				componentHandles[handle_p->deviceHandle.component]->ACMVolumeSetPath(
					MASK8(handle_p->deviceHandle.path),
					volume);
			}

			handle_p++;
		}

		return ACM_RC_OK;
	}

	while (handle_p->deviceHandle.device != ACM_NOT_CONNECTED)
	{
		if ( (handle_p->deviceHandle.device == device) && (handle_p->deviceHandle.format == format) )
		{
			ACMComponent = handle_p->deviceHandle.component;
			break;
		}

		handle_p++;
	}
	if (ACMComponent == NULL_COMPONENT)
	{
		DPRINTF("ACMAudioDeviceVolumeSet API : device not found\n");
		return acmReturnCode;
	}

	audioVolReq.device = device;
	audioVolReq.format = format;
	audioVolReq.volume = volume;
	stubReturn = SetComponentVolume(&audioVolReq, &audioVolRsp);

	if (stubReturn == STUB_SUCCESS)
	{
		ACM_ReturnCode acmReturnCode = ACM_RC_DEVICE_FORMAT_NOT_FOUND;
		ACM_AudioDevice device;
		ACM_AudioFormat format;
		GAIN_RECORD PGAGains[ACM_MAXPGACNT_PERPATH];
		UINT32 PGAGainIndex, PGAGainCnt;

		//fill audioVolRsp data
		acmReturnCode = (ACM_ReturnCode)audioVolRsp.status;
		device = audioVolRsp.device;
		format = audioVolRsp.format;
		PGAGainCnt = audioVolRsp.gains.gain_nums;
		memcpy((unsigned char *)PGAGains, (unsigned char *)&(audioVolRsp.gains.gain_record[0]), sizeof(PGAGains));

		for (PGAGainIndex = 0; PGAGainIndex < PGAGainCnt; PGAGainIndex++)
		{
			/********************************************************************************
			   For each path returned, a single DIGITAL_GAIN_MAGICNUM will be returned from ACM_COMM.
			   And there may be more than one PGA gain for each path.

			   So, we can check whether gain_id==DIGITAL_GAIN_MAGICNUM, to get the path.
			   Then we can configure codec using the path.
			 *********************************************************************************/
			if (DIGITAL_GAIN_MAGICNUM == PGAGains[PGAGainIndex].gain_id)
			{
				/**********************************************************
					Set PMIC path returned from ACM_COMM.

					Because PMIC path is maintained by ACM_COMM, ACMAPPS APIs
					such as ACMAPPSAudioDeviceEnable will not know PMIC path.
				 ***********************************************************/
				componentHandles[ACMComponent]->ACMVolumeSetPath(PGAGains[PGAGainIndex].path, PGAGains[PGAGainIndex].volume);
			}
			else
			{
				DPRINTF("First setvolume, then set gain from CP, to ensure not to replace the gain with AP side again\n");
			}

		}

		for (PGAGainIndex = 0; PGAGainIndex < PGAGainCnt; PGAGainIndex++)
		{
			if (DIGITAL_GAIN_MAGICNUM == PGAGains[PGAGainIndex].gain_id)
			{
			}
			else
			{
				DPRINTF("SetComponentVolume, (path,gainId,gain,regIndex)= (%lu, %lu, %lu, %d)\n", PGAGains[PGAGainIndex].path, PGAGains[PGAGainIndex].gain_id, PGAGains[PGAGainIndex].gain_val, PGAGains[PGAGainIndex].regIndex);
				componentHandles[ACMComponent]->ACMSetPathAnalogGain(PGAGains[PGAGainIndex].regIndex, PGAGains[PGAGainIndex].gain_val, PGAGains[PGAGainIndex].gainMask, PGAGains[PGAGainIndex].gainShift);
			}

		}

	}
	else if (stubReturn == STUB_TIMEOUT)
	{
		DPRINTF("SetComponentVolume timeout, AP do default setting\n\n");
		handle_p = _deviceTable;

		while (handle_p->deviceHandle.device != ACM_NOT_CONNECTED)
		{
			if ( (handle_p->deviceHandle.device == device) && (handle_p->deviceHandle.format == format) )
			{
				componentHandles[handle_p->deviceHandle.component]->ACMVolumeSetPath(
					MASK8(handle_p->deviceHandle.path),
					volume);
			}

			handle_p++;
		}
	}
	else
	{
		DPRINTF("SetComponentVolume error, return %d\n\n", stubReturn);
		handle_p = _deviceTable;

		while (handle_p->deviceHandle.device != ACM_NOT_CONNECTED)
		{
			if ( (handle_p->deviceHandle.device == device) && (handle_p->deviceHandle.format == format) )
			{
				componentHandles[handle_p->deviceHandle.component]->ACMVolumeSetPath(
					MASK8(handle_p->deviceHandle.path),
					volume);
			}

			handle_p++;
		}
	}

	acmReturnCode = ACM_RC_OK;
	return acmReturnCode;
} /* ACMAudioDeviceVolumeSet */


/*******************************************************************************
* Function: ACMAudioDeviceDisable
*******************************************************************************
* Description: Disables the device/format pair only if the pair enabled.
*               In case of ACM_MSA_PCM format, restore the digital volume by calling
*               the function restoreDigitalVolume() (see description of
*               restoreDigitalVolume() above).
*
* Parameters: ACM_AudioDevice device
*             ACM_AudioFormat format
*
* Return value: ACM_ReturnCode
*
* Notes:
*******************************************************************************/
AudioDeviceDisableReq audioDisableReq;
AudioDeviceDisableRsp audioDisableRsp;
ACM_ReturnCode ACMAudioDeviceDisable(ACM_AudioDevice device,
				     ACM_AudioFormat format)
{
	ACM_DeviceHandleLocal *handle_p;
	ACM_ReturnCode acmReturnCode = ACM_RC_DEVICE_FORMAT_NOT_FOUND;
	STUB_ReturnCode stubReturn;

	DPRINTF("ACMAudioDeviceDisable API called: device=%d,format=%d\n", (int)device, (int)format);

	ACM_Component ACMComponent = NULL_COMPONENT;
	handle_p = _deviceTable;

	if (format != ACM_MSA_PCM)
	{
		DPRINTF("ACMAudioDeviceDisable: not PCM path so do not notify CP\n\n");
		handle_p = _deviceTable;

		if (ACMAudioGetCalibrationStatus() == ACM_CALIBRATION_ON)
		{
			DPRINTF("Calibration status: forward hifi path to CP\n\n");
			audioDisableReq.device = device;
			audioDisableReq.format = format;
			stubReturn = DisableHwComponent(&audioDisableReq, &audioDisableRsp);
		}

		while (handle_p->deviceHandle.device != ACM_NOT_CONNECTED)
		{
			if ( (handle_p->deviceHandle.device == device) && (handle_p->deviceHandle.format == format) )
			{
				componentHandles[handle_p->deviceHandle.component]->ACMDisablePath(
					MASK8(handle_p->deviceHandle.path));
			}

			handle_p++;
		}

		return ACM_RC_OK;
	}

	while (handle_p->deviceHandle.device != ACM_NOT_CONNECTED)
	{
		if ( (handle_p->deviceHandle.device == device) && (handle_p->deviceHandle.format == format) )
		{
			ACMComponent = handle_p->deviceHandle.component;
			break;
		}

		handle_p++;
	}
	if (ACMComponent == NULL_COMPONENT)
	{
		DPRINTF("ACMAudioDeviceDisable API : device not found\n");
		return acmReturnCode;
	}

	audioDisableReq.device = device;
	audioDisableReq.format = format;
	stubReturn = DisableHwComponent(&audioDisableReq, &audioDisableRsp);

	if (stubReturn == STUB_SUCCESS)
	{
		ACM_ReturnCode acmReturnCode = ACM_RC_DEVICE_FORMAT_NOT_FOUND;
		ACM_AudioDevice device;
		ACM_AudioFormat format;
		GAIN_RECORD PGAGains[ACM_MAXPGACNT_PERPATH];
		UINT32 PGAGainIndex, PGAGainCnt;

		//fill audioDisableRsp data
		acmReturnCode = (ACM_ReturnCode)audioDisableRsp.status;
		device = audioDisableRsp.device;
		format = audioDisableRsp.format;
		PGAGainCnt = audioDisableRsp.gains.gain_nums;
		memcpy((unsigned char *)PGAGains, (unsigned char *)&(audioDisableRsp.gains.gain_record[0]), sizeof(PGAGains));

		for (PGAGainIndex = 0; PGAGainIndex < PGAGainCnt; PGAGainIndex++)
		{
			/********************************************************************************
			   For each path returned, a single DIGITAL_GAIN_MAGICNUM will be returned from ACM_COMM.
			   And there may be more than one PGA gain for each path.

			   So, we can check whether gain_id==DIGITAL_GAIN_MAGICNUM, to get the path.
			   Then we can configure codec using the path.
			 *********************************************************************************/
			if (DIGITAL_GAIN_MAGICNUM == PGAGains[PGAGainIndex].gain_id)
			{
				/**********************************************************
					Set PMIC path returned from ACM_COMM.

					Because PMIC path is maintained by ACM_COMM, ACMAPPS APIs
					such as ACMAPPSAudioDeviceEnable will not know PMIC path.
				 ***********************************************************/

				/*********************************************************************************
				   ACM_COMM API ACMAudioDeviceDisable will go through the device table, if it get to the end of
				   the table without finding another user of this specific path, ACM_RC_NEED_DISABLE_PATH will
				   be returned to ACM_APPS.

				   ACM_APPS should disable the according path.
				 **********************************************************************************/
				if (ACM_RC_NEED_DISABLE_PATH == acmReturnCode)
					componentHandles[ACMComponent]->ACMDisablePath(PGAGains[PGAGainIndex].path);
			}
			else
			{
				DPRINTF("DisableHwComponent, (path,gainId,gain,regIndex)= (%lu, %lu, %lu, %d)\n", PGAGains[PGAGainIndex].path, PGAGains[PGAGainIndex].gain_id, PGAGains[PGAGainIndex].gain_val, PGAGains[PGAGainIndex].regIndex);
				/*we don't need to set gain here, the default value will be mute or the smallest*/
				//componentHandles[ACMComponent]->ACMSetPathAnalogGain(PGAGains[PGAGainIndex].regIndex, PGAGains[PGAGainIndex].gain_val, PGAGains[PGAGainIndex].gainMask, PGAGains[PGAGainIndex].gainShift);
			}

		}

	}
	else if (stubReturn == STUB_TIMEOUT)
	{
		DPRINTF("DisableHwComponent timeout, AP do default setting\n\n");
		handle_p = _deviceTable;

		while (handle_p->deviceHandle.device != ACM_NOT_CONNECTED)
		{
			if ( (handle_p->deviceHandle.device == device) && (handle_p->deviceHandle.format == format) )
			{
				componentHandles[handle_p->deviceHandle.component]->ACMDisablePath(
					MASK8(handle_p->deviceHandle.path));
			}

			handle_p++;
		}
	}
	else
	{
		DPRINTF("DisableHwComponent error, return %d\n\n", stubReturn);
		handle_p = _deviceTable;

		while (handle_p->deviceHandle.device != ACM_NOT_CONNECTED)
		{
			if ( (handle_p->deviceHandle.device == device) && (handle_p->deviceHandle.format == format) )
			{
				componentHandles[handle_p->deviceHandle.component]->ACMDisablePath(
					MASK8(handle_p->deviceHandle.path));
			}

			handle_p++;
		}
	}

	acmReturnCode = ACM_RC_OK;
	return acmReturnCode;
} /* End of ACMAudioDeviceDisable */


/*******************************************************************************
* Function: ACMAudioDeviceEnable
*******************************************************************************
* Description: Enables the device/format pair and sets the volume.
*               Enable the pair only if  it's not enaled already, and set the
*               analog volume.
*               In case of ACM_MSA_PCM format, the digital volume is set by the
*               function digitalVolumeSet() according to certain specifications
*               (see description of digitalVolumeSet() above).
*
* Parameters: ACM_AudioDevice device
*             ACM_AudioFormat format
*             ACM_AudioVolume volume
*
* Return value: ACM_ReturnCode
*
* Notes:
*******************************************************************************/
AudioDeviceEnableReq audioEnableReq;
AudioDeviceEnableRsp audioEnableRsp;

ACM_ReturnCode ACMAudioDeviceEnable(ACM_AudioDevice device,
				    ACM_AudioFormat format,
				    ACM_AudioVolume volume)
{
	ACM_DeviceHandleLocal *handle_p;
	ACM_ReturnCode acmReturnCode = ACM_RC_DEVICE_FORMAT_NOT_FOUND;
	STUB_ReturnCode stubReturn;

	DPRINTF("ACMAudioDeviceEnable API called: device=%d,format=%d,volume=%d\n", (int)device, (int)format, volume);

	ACM_Component ACMComponent = NULL_COMPONENT;
	handle_p = _deviceTable;

	if (format != ACM_MSA_PCM)
	{
		DPRINTF("ACMAudioDeviceEnable: not PCM path so do not notify CP\n\n");
		handle_p = _deviceTable;

		if (ACMAudioGetCalibrationStatus() == ACM_CALIBRATION_ON)
		{
			DPRINTF("Calibration status: forward hifi path to CP\n\n");
			audioEnableReq.device = device;
			audioEnableReq.format = format;
			audioEnableReq.volume = volume;
			stubReturn = EnableHwComponent(&audioEnableReq, &audioEnableRsp);
		}

		while (handle_p->deviceHandle.device != ACM_NOT_CONNECTED)
		{
			if ( (handle_p->deviceHandle.device == device) && (handle_p->deviceHandle.format == format) )
			{
				componentHandles[handle_p->deviceHandle.component]->ACMEnablePath(
					MASK8(handle_p->deviceHandle.path),
					volume);
			}

			handle_p++;
		}

		return ACM_RC_OK;
	}

	while (handle_p->deviceHandle.device != ACM_NOT_CONNECTED)
	{
		if ( (handle_p->deviceHandle.device == device) && (handle_p->deviceHandle.format == format) )
		{
			ACMComponent = handle_p->deviceHandle.component;
			break;
		}

		handle_p++;
	}
	if (ACMComponent == NULL_COMPONENT)
	{
		DPRINTF("ACMAudioDeviceEnable API : device not found\n");
		return acmReturnCode;
	}

	/*After apply GSSP recording, upper layer may change ADC sample rate frequently, and it doesn't make sure it will be changed to 8Khz*/
	/*When being in voice call, force to change ADC sample rate to 8kHz*/
	LevanteFrequencySettingsData frequencySettingsData = { LOFI, BOTH_DIRECTIONS, BOTH_SIDES, LEVANTE_BIT_RATE_8_KHZ };
	LevanteAdditionalSettingsSet(FREQUENCY_SETTINGS, (void *)&frequencySettingsData);

	audioEnableReq.device = device;
	audioEnableReq.format = format;
	audioEnableReq.volume = volume;
	stubReturn = EnableHwComponent(&audioEnableReq, &audioEnableRsp);

	if (stubReturn == STUB_SUCCESS)
	{
		ACM_ReturnCode acmReturnCode = ACM_RC_DEVICE_FORMAT_NOT_FOUND;
		ACM_AudioDevice device;
		ACM_AudioFormat format;
		GAIN_RECORD PGAGains[ACM_MAXPGACNT_PERPATH];
		UINT32 PGAGainIndex, PGAGainCnt;

		//fill audioEnableRsp data
		acmReturnCode = (ACM_ReturnCode)audioEnableRsp.status;
		device = audioEnableRsp.device;
		format = audioEnableRsp.format;
		PGAGainCnt = audioEnableRsp.gains.gain_nums;
		memcpy((unsigned char *)PGAGains, (unsigned char *)&(audioEnableRsp.gains.gain_record[0]), sizeof(PGAGains));

		for (PGAGainIndex = 0; PGAGainIndex < PGAGainCnt; PGAGainIndex++)
		{
			/********************************************************************************
			   For each path returned, a single DIGITAL_GAIN_MAGICNUM will be returned from ACM_COMM.
			   And there may be more than one PGA gain for each path.

			   So, we can check whether gain_id==DIGITAL_GAIN_MAGICNUM, to get the path.
			   Then we can configure codec using the path.
			 *********************************************************************************/
			if (DIGITAL_GAIN_MAGICNUM == PGAGains[PGAGainIndex].gain_id)
			{
				/**********************************************************
					Set PMIC path returned from ACM_COMM.

					Because PMIC path is maintained by ACM_COMM, ACMAPPS APIs
					such as ACMAPPSAudioDeviceEnable will not know PMIC path.
				 ***********************************************************/
				if (ACM_RC_OK == acmReturnCode)
				{
					componentHandles[ACMComponent]->ACMEnablePath(PGAGains[PGAGainIndex].path, PGAGains[PGAGainIndex].volume);

					//0:not mute; 1: mute; other value: no call to ACMMutePath
					if (PGAGains[PGAGainIndex].mutePath == 0)
						componentHandles[ACMComponent]->ACMMutePath(PGAGains[PGAGainIndex].path, ACM_MUTE_OFF);
					else if (PGAGains[PGAGainIndex].mutePath == 1)
						componentHandles[ACMComponent]->ACMMutePath(PGAGains[PGAGainIndex].path, ACM_MUTE_ON);
					else if (PGAGains[PGAGainIndex].mutePath == ACM_NOT_CALL_ACMMUTEPATH)
					{
						DPRINTF("Rx AUDIO_ENABLEHWCPNT_CONFIRM : Not call ACMMutePath\n");
					}
				}
			}
			else
			{
				DPRINTF("First enable path, then set gain from CP, to ensure not to replace the gain with AP side again\n");
			}

		}

		for (PGAGainIndex = 0; PGAGainIndex < PGAGainCnt; PGAGainIndex++)
		{
			if (DIGITAL_GAIN_MAGICNUM == PGAGains[PGAGainIndex].gain_id)
			{
			}
			else
			{
				DPRINTF("EnableHwComponent, set gain, (path,gainId,gain,regIndex)= (%lu, %lu, %lu, %d)\n", PGAGains[PGAGainIndex].path, PGAGains[PGAGainIndex].gain_id, PGAGains[PGAGainIndex].gain_val, PGAGains[PGAGainIndex].regIndex);
				componentHandles[ACMComponent]->ACMSetPathAnalogGain(PGAGains[PGAGainIndex].regIndex, PGAGains[PGAGainIndex].gain_val, PGAGains[PGAGainIndex].gainMask, PGAGains[PGAGainIndex].gainShift);
			}

		}

	}
	else if (stubReturn == STUB_TIMEOUT)
	{
		DPRINTF("\nEnableHwComponent timeout, AP do default setting\n\n");
		handle_p = _deviceTable;

		while (handle_p->deviceHandle.device != ACM_NOT_CONNECTED)
		{
			if ( (handle_p->deviceHandle.device == device) && (handle_p->deviceHandle.format == format) )
			{
				componentHandles[handle_p->deviceHandle.component]->ACMEnablePath(
					MASK8(handle_p->deviceHandle.path),
					volume);
			}

			handle_p++;
		}
	}
	else
	{
		DPRINTF("\nEnableHwComponent error, return %d\n\n", stubReturn);
		handle_p = _deviceTable;

		while (handle_p->deviceHandle.device != ACM_NOT_CONNECTED)
		{
			if ( (handle_p->deviceHandle.device == device) && (handle_p->deviceHandle.format == format) )
			{
				componentHandles[handle_p->deviceHandle.component]->ACMEnablePath(
					MASK8(handle_p->deviceHandle.path),
					volume);
			}

			handle_p++;
		}
	}

	printf("EnableHwComponent in ACMAudioDeviceEnable done\n");

	acmReturnCode = ACM_RC_OK;

	return acmReturnCode;


} /* End of ACMAudioDeviceEnable */

/************************************************************************
 * Function: ACMAudioSetCalibrationStatus
 *************************************************************************
 * Description:  set if it enters calibration test for using ACAT
 *
 * Parameters:   none
 *
 * Return value: ACM_AudioCalibrationStatus
 *
 * Notes:
 *************************************************************************/
//ICAT EXPORTED FUNCTION - Audio, ACM, ACMAudioSetCalibrationStatus
ACM_ReturnCode ACMAudioSetCalibrationStatus(ACM_AudioCalibrationStatus calstatus)
{
	DPRINTF("\nCalibration state is %s\n\n", calstatus?"ACM_CALIBRATION_ON":"ACM_CALIBRATION_OFF");
	if (calstatus)
	{
		_calibrationstatus = ACM_CALIBRATION_ON;
	}
	else

		_calibrationstatus = ACM_CALIBRATION_OFF;
	return ACM_RC_OK;
}

/************************************************************************
 * Function: ACMAudioGetCalibrationStatus
 *************************************************************************
 * Description:  indicate if it escape from calibration test
 *
 * Parameters:   none
 *
 * Return value: ACM_AudioCalibrationStatus
 *
 * Notes:
 *************************************************************************/
static ACM_AudioCalibrationStatus ACMAudioGetCalibrationStatus(void)
{
	return _calibrationstatus;
}

/*******************************************************************************
* Function: ACMAudioPathEnable
*******************************************************************************
* Description: Enable path according to PHS requirments
*
* Parameters: ACM_Component, path, volume
*
* Return value: ACM_ReturnCode
*
* Notes:
*******************************************************************************/
ACM_ReturnCode ACMAudioPathEnable(ACM_Component component, unsigned char path, ACM_AudioVolume volume)
{
	DPRINTF("ACMAudioPathEnable, component_num = %d, path_num = %d, volume = %d\n", component, path, volume);
	/*it enater loopback test status*/
	componentHandles[component]->ACMSetLoopbackTestStatus(1);
	componentHandles[component]->ACMEnablePath(path, volume);
	/*exit from loopback test mode*/
	componentHandles[component]->ACMSetLoopbackTestStatus(0);
	return ACM_RC_OK;
}

/*******************************************************************************
* Function: ACMAudioPathDisable
*******************************************************************************
* Description: Enable path according to PHS requirments
*
* Parameters: ACM_Component, path
*
* Return value: ACM_ReturnCode
*
* Notes:
*******************************************************************************/
ACM_ReturnCode ACMAudioPathDisable(ACM_Component component, unsigned char path)
{
	DPRINTF("ACMAudioPathDisable, component_num = %d, path_num = %d\n", component, path);
	/*it enater loopback test status*/
	componentHandles[component]->ACMSetLoopbackTestStatus(1);
	componentHandles[component]->ACMDisablePath(path);
	/*exit from loopback test mode*/
	componentHandles[component]->ACMSetLoopbackTestStatus(0);
	return ACM_RC_OK;
}

void  ACMStreamInit(void);
/*******************************************************************************
* Function: ACMInit
*******************************************************************************
* Description: ACM initialization function.
*
* Parameters: none
*
* Return value: void
*
* Notes:
*******************************************************************************/
void ACMInit(void)
{
	DPRINTF("InitAudioStub\n");
	//for cp
	InitAudioStub();

	DPRINTF("ACMControlInit\n");
	ACMControlInit();

	DPRINTF("ACMStreamInit\n");
	ACMStreamInit();

}

/*******************************************************************************
* Function: ACMDeinit
*******************************************************************************
* Description: ACM Deinitialization function.
*
* Parameters: none
*
* Return value: void
*
* Notes:
*******************************************************************************/
void ACMDeinit()
{
}


ACMAudioDSPSettings audioDSPSettingsReq;
ACMAudioDSPSettings audioDSPSettingsRsp;

/*******************************************************************************
* Function: ACMAudioSetMSASettings
*******************************************************************************
* Description: Send audio MSA settings to CP side.
*
* Parameters: data
*                  Caller should give the setting data in data, and get the response setting from CP side in data.
*
* Return value: void
*
* Notes:
*******************************************************************************/
ACM_ReturnCode ACMAudioSetMSASettings(const char *data)
{
	ACM_ReturnCode acmReturnCode = ACM_RC_BUFFER_GET_FUNC_INVALID;

	STUB_ReturnCode stubReturn;

	DPRINTF("ACMAudioSetMSASettings API called\n");

	memcpy((char *)audioDSPSettingsReq.data, data, MSASETTINGDATALEN);
	stubReturn = SetMSASetting(&audioDSPSettingsReq, &audioDSPSettingsRsp);

	if (stubReturn == STUB_SUCCESS)
	{
		DPRINTF("ACMAudioSetMSASettings success\n");
		//The response will be sent separately, we won't get the response from here
		//memcpy(data, (char *)audioDSPSettingsReq.data, MSASETTINGDATALEN);
	}
	else if (stubReturn == STUB_TIMEOUT)
	{
		DPRINTF("ACMAudioSetMSASettings timeout\n\n");
		return acmReturnCode;
	}
	else
	{
		DPRINTF("ACMAudioSetMSASettings error, return %d\n\n", stubReturn);
		return acmReturnCode;
	}

	acmReturnCode = ACM_RC_OK;
	return acmReturnCode;
}

