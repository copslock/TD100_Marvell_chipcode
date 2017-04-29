#include "MarvellAmixer.h"
#include "acmtypes.h"
#include "acm.h"
#include "diag.h"
#include "diag_aud_eq.h"

/*******************************************************************************
* Function: ACMAudioPathEnable_Env
*******************************************************************************
* Description: ACMAudioPathEnable wrapper
*
* Parameters: none
*
* Return value: void
*
* Notes:
*******************************************************************************/
//ICAT EXPORTED FUNCTION - Audio, HW, AUDIO_HW_PathEnable
void ACMAudioPathEnable_Env(void *data)
{
	int client, value = 0;
	unsigned char component = 0, path = 0;

	DIAG_FILTER(AUDIO, ACM, ACMAudioPathEnable, DIAG_ALL)
	diagPrintf("ACMAudioPathEnable\n");
	/*
	 *data[0] indicate componentId, data[1] indicate pathID,
	 *data[2] indicate path direction, data[3] indicate volume
	 */
	value |= (ACM_AudioVolume)(*((unsigned char*)data + 3));/*volume*/
	path = (unsigned char)(*((unsigned char *)data + 1));  /*pathnum*/
	component = (ACM_Component)(*((unsigned char *)data));  /*component number*/

	client = PHONE_CLIENT;
	value |= (path << 16) | (component << 24);

	audio_route_control(client, PLUGIN_CTL_WRITE, PATH_ENABLE, 0, value);
}

/*******************************************************************************
* Function: ACMAudioPathDisable_Env
*******************************************************************************
* Description: ACMAudioPathDisable wrapper
*
* Parameters: none
*
* Return value: void
*
* Notes:
*******************************************************************************/
//ICAT EXPORTED FUNCTION - Audio, HW, AUDIO_HW_PathDisable
void ACMAudioPathDisable_Env(void *data)
{
	int client, value = 0;
	unsigned char component = 0, path = 0;

	DIAG_FILTER(AUDIO, ACM, ACMAudioPathDisable, DIAG_ALL)
	diagPrintf("ACMAudioPathDisable\n");
	/*
	 *data[0] indicate componentId, data[1] indicate pathID,
	 *data[2] indicate path direction, data[3] indicate volume
	 */
	path = (unsigned char)(*((unsigned short *)data + 1));  //pathnum
	component = (ACM_Component)(*((unsigned short *)data));  //component number

	client = PHONE_CLIENT;
	value |= (path << 16) | (component << 24);

	audio_route_control(client, PLUGIN_CTL_WRITE, PATH_DISABLE, 0, value);
}

/*******************************************************************************
* Function: ACMAudioDeviceEnable_Env
*******************************************************************************
* Description: ACMAudioDeviceEnable wrapper
*
* Parameters: none
*
* Return value: void
*
* Notes:
*******************************************************************************/
//ICAT EXPORTED FUNCTION - Audio, ACM, ACMAudioDeviceEnable
void ACMAudioDeviceEnable_Env(void *data)
{
	int client, device, value = 0;

	DIAG_FILTER(AUDIO, ACM, ACMAudioDeviceEnable, DIAG_ALL)
	diagPrintf("ACMAudioDeviceEnable\n");

	value |= (ACM_AudioVolume)(*((unsigned short*)data + 2));//volume

	switch ((ACM_AudioFormat)(*((unsigned short*)data + 1))) {//client
	case ACM_MSA_PCM:
		client = PHONE_CLIENT;
		break;
	case ACM_I2S:
		client = HIFI_CLIENT;
		break;
	case ACM_AUX_FM:
		client = FMRADIO_CLINET;
		break;
	default:
		return;
	}

	switch ((ACM_AudioDevice)(*((unsigned short*)data))) {
	case ACM_MAIN_SPEAKER:
		device = EARPIECE_DEVICE;//device
		value |= (OUTPUT << 16);//direction
		break;
	case ACM_AUX_SPEAKER:
		device = SPEAKER_DEVICE;
		value |= (OUTPUT << 16);
		break;
	case ACM_HEADSET_SPEAKER:
		device = HEADSET_DEVICE;
		value |= (OUTPUT << 16);
		break;
	case ACM_BLUETOOTH_SPEAKER:
		device = BLUETOOTH_DEVICE;
		value |= (OUTPUT << 16);
		break;
	case ACM_MIC:
		device = SPEAKER_DEVICE;
		value |= (INPUT << 16);
		break;
	case ACM_HEADSET_MIC:
		device = HEADSET_DEVICE;
		value |= (INPUT << 16);
		break;
	case ACM_BLUETOOTH_MIC:
		device = BLUETOOTH_DEVICE;
		value |= (INPUT << 16);
		break;
	case ACM_MIC_EC:
		device = SPEAKER_DEVICE;
		value |= (INPUT << 16);
		value |= (ECENABLE << 16);
		break;
	case ACM_HEADSET_MIC_EC:
		device = HEADSET_DEVICE;
		value |= (INPUT << 16);
		value |= (ECENABLE << 16);
		break;
	case ACM_AUX_MIC:
		if (client != FMRADIO_CLINET)
			return;
		device = ACM_AUX_SPEAKER;
		value |= (INPUT << 16);
		break;
	case ACM_MAIN_SPEAKER__LOOP:
		device = EARPIECE_DEVICE;//device
		value |= (OUTPUT << 16);//direction
		value |= (LOOPBACKTEST << 16);//loopback test
		break;
	case ACM_AUX_SPEAKER__LOOP:
		device = SPEAKER_DEVICE;
		value |= (OUTPUT << 16);
		value |= (LOOPBACKTEST << 16);//loopback test
		break;
	case ACM_HEADSET_SPEAKER__LOOP:
		device = HEADSET_DEVICE;
		value |= (OUTPUT << 16);
		value |= (LOOPBACKTEST << 16);//loopback test
		break;
	case ACM_MIC__LOOP:
		device = SPEAKER_DEVICE;
		value |= (INPUT << 16);
		value |= (LOOPBACKTEST << 16);//loopback test
	case ACM_AUX_MIC__LOOP:
		device = ACM_AUX_SPEAKER;
		value |= (INPUT << 16);
		value |= (LOOPBACKTEST << 16);//loopback test
		break;
	case ACM_HEADSET_MIC__LOOP:
		device = HEADSET_DEVICE;
		value |= (INPUT << 16);
		value |= (LOOPBACKTEST << 16);//loopback test
		break;

	default:
		return;
	}

	audio_route_control(client, PLUGIN_CTL_WRITE, METHOD_ENABLE, device, value);
}


/*******************************************************************************
* Function: ACMAudioDeviceDisable_Env
*******************************************************************************
* Description: ACMAudioDeviceDisable wrapper
*
* Parameters: none
*
* Return value: void
*
* Notes:
*******************************************************************************/
//ICAT EXPORTED FUNCTION - Audio, ACM, ACMAudioDeviceDisable
void ACMAudioDeviceDisable_Env(void *data)
{
	int client, device, value = 0;

	DIAG_FILTER(AUDIO, ACM, ACMAudioDeviceDisable, DIAG_ALL)
	diagPrintf("ACMAudioDeviceDisable\n");

	switch ((ACM_AudioFormat)(*((unsigned short*)data + 1))) {//client
	case ACM_MSA_PCM:
		client = PHONE_CLIENT;
		break;
	case ACM_I2S:
		client = HIFI_CLIENT;
		break;
	case ACM_AUX_FM:
		client = FMRADIO_CLINET;
		break;
	default:
		return;
	}

	switch ((ACM_AudioDevice)(*((unsigned short*)data))) {
	case ACM_MAIN_SPEAKER:
		device = EARPIECE_DEVICE;//device
		value |= (OUTPUT << 16);//direction
		break;
	case ACM_AUX_SPEAKER:
		device = SPEAKER_DEVICE;
		value |= (OUTPUT << 16);
		break;
	case ACM_HEADSET_SPEAKER:
		device = HEADSET_DEVICE;
		value |= (OUTPUT << 16);
		break;
	case ACM_BLUETOOTH_SPEAKER:
		device = BLUETOOTH_DEVICE;
		value |= (OUTPUT << 16);
		break;
	case ACM_MIC:
		device = SPEAKER_DEVICE;
		value |= (INPUT << 16);
		break;
	case ACM_HEADSET_MIC:
		device = HEADSET_DEVICE;
		value |= (INPUT << 16);
		break;
	case ACM_BLUETOOTH_MIC:
		device = BLUETOOTH_DEVICE;
		value |= (INPUT << 16);
		break;
	case ACM_MIC_EC:
		device = SPEAKER_DEVICE;
		value |= (INPUT << 16);
		value |= (ECENABLE << 16);
		break;
	case ACM_HEADSET_MIC_EC:
		device = HEADSET_DEVICE;
		value |= (INPUT << 16);
		value |= (ECENABLE << 16);
		break;
	case ACM_AUX_MIC:
		if (client != FMRADIO_CLINET)
			return;
		device = ACM_AUX_SPEAKER;
		value |= (INPUT << 16);
		break;
	case ACM_MAIN_SPEAKER__LOOP:
		device = EARPIECE_DEVICE;//device
		value |= (OUTPUT << 16);//direction
		value |= (LOOPBACKTEST << 16);//loopback test
		break;
	case ACM_AUX_SPEAKER__LOOP:
		device = SPEAKER_DEVICE;
		value |= (OUTPUT << 16);
		value |= (LOOPBACKTEST << 16);//loopback test
		break;
	case ACM_HEADSET_SPEAKER__LOOP:
		device = HEADSET_DEVICE;
		value |= (OUTPUT << 16);
		value |= (LOOPBACKTEST << 16);//loopback test
		break;
	case ACM_MIC__LOOP:
		device = SPEAKER_DEVICE;
		value |= (INPUT << 16);
		value |= (LOOPBACKTEST << 16);//loopback test
	case ACM_AUX_MIC__LOOP:
		device = ACM_AUX_SPEAKER;
		value |= (INPUT << 16);
		value |= (LOOPBACKTEST << 16);//loopback test
		break;
	case ACM_HEADSET_MIC__LOOP:
		device = HEADSET_DEVICE;
		value |= (INPUT << 16);
		value |= (LOOPBACKTEST << 16);//loopback test
		break;
	default:
		return;
	}

	audio_route_control(client, PLUGIN_CTL_WRITE, METHOD_DISABLE, device, value);
}


/*******************************************************************************
* Function: ACMAudioDeviceMute_Env
*******************************************************************************
* Description: ACMAudioDeviceMute wrapper
*
* Parameters: none
*
* Return value: void
*
* Notes:
*******************************************************************************/
//ICAT EXPORTED FUNCTION - Audio, ACM, ACMAudioDeviceMute
void ACMAudioDeviceMute_Env(void *data)
{
	int client, device, value = 0;

	DIAG_FILTER(AUDIO, ACM, ACMAudioDeviceMute, DIAG_ALL)
	diagPrintf("ACMAudioDeviceMute\n");

	value |= (ACM_AudioVolume)(*((unsigned short*)data + 2));//mute

	switch ((ACM_AudioFormat)(*((unsigned short*)data + 1))) {//client
	case ACM_MSA_PCM:
		client = PHONE_CLIENT;
		break;
	case ACM_I2S:
		client = HIFI_CLIENT;
		break;
	case ACM_AUX_FM:
		client = FMRADIO_CLINET;
		break;
	default:
		return;
	}

	switch ((ACM_AudioDevice)(*((unsigned short*)data))) {
	case ACM_MAIN_SPEAKER:
		device = EARPIECE_DEVICE;//device
		value |= (OUTPUT << 16);//direction
		break;
	case ACM_AUX_SPEAKER:
		device = SPEAKER_DEVICE;
		value |= (OUTPUT << 16);
		break;
	case ACM_HEADSET_SPEAKER:
		device = HEADSET_DEVICE;
		value |= (OUTPUT << 16);
		break;
	case ACM_BLUETOOTH_SPEAKER:
		device = BLUETOOTH_DEVICE;
		value |= (OUTPUT << 16);
		break;
	case ACM_MIC:
		device = SPEAKER_DEVICE;
		value |= (INPUT << 16);
		break;
	case ACM_HEADSET_MIC:
		device = HEADSET_DEVICE;
		value |= (INPUT << 16);
		break;
	case ACM_BLUETOOTH_MIC:
		device = BLUETOOTH_DEVICE;
		value |= (INPUT << 16);
		break;
	case ACM_MIC_EC:
		device = SPEAKER_DEVICE;
		value |= (INPUT << 16);
		value |= (ECENABLE << 16);
		break;
	case ACM_HEADSET_MIC_EC:
		device = HEADSET_DEVICE;
		value |= (INPUT << 16);
		value |= (ECENABLE << 16);
		break;
	case ACM_AUX_MIC:
		if (client != FMRADIO_CLINET)
			return;
		device = ACM_AUX_SPEAKER;
		value |= (INPUT << 16);
		break;
	case ACM_MAIN_SPEAKER__LOOP:
		device = EARPIECE_DEVICE;//device
		value |= (OUTPUT << 16);//direction
		value |= (LOOPBACKTEST << 16);//loopback test
		break;
	case ACM_AUX_SPEAKER__LOOP:
		device = SPEAKER_DEVICE;
		value |= (OUTPUT << 16);
		value |= (LOOPBACKTEST << 16);//loopback test
		break;
	case ACM_HEADSET_SPEAKER__LOOP:
		device = HEADSET_DEVICE;
		value |= (OUTPUT << 16);
		value |= (LOOPBACKTEST << 16);//loopback test
		break;
	case ACM_MIC__LOOP:
		device = SPEAKER_DEVICE;
		value |= (INPUT << 16);
		value |= (LOOPBACKTEST << 16);//loopback test
	case ACM_AUX_MIC__LOOP:
		device = ACM_AUX_SPEAKER;
		value |= (INPUT << 16);
		value |= (LOOPBACKTEST << 16);//loopback test
		break;
	case ACM_HEADSET_MIC__LOOP:
		device = HEADSET_DEVICE;
		value |= (INPUT << 16);
		value |= (LOOPBACKTEST << 16);//loopback test
		break;
	default:
		return;
	}

	audio_route_control(client, PLUGIN_CTL_WRITE, METHOD_MUTE, device, value);
}


/*******************************************************************************
* Function: ACMAudioDeviceVolumeSet_Env
*******************************************************************************
* Description: ACMAudioDeviceVolumeSet wrapper
*
* Parameters: none
*
* Return value: void
*
* Notes:
*******************************************************************************/
//ICAT EXPORTED FUNCTION - Audio, ACM, ACMAudioDeviceVolumeSet
void ACMAudioDeviceVolumeSet_Env(void *data)
{
	int client, device, value = 0;

	DIAG_FILTER(AUDIO, ACM, AACMAudioDeviceVolumeSet, DIAG_ALL)
	diagPrintf("ACMAudioDeviceVolumeSet\n");

	value |= (ACM_AudioVolume)(*((unsigned short*)data + 2));//volume

	switch ((ACM_AudioFormat)(*((unsigned short*)data + 1))) {//client
	case ACM_MSA_PCM:
		client = PHONE_CLIENT;
		break;
	case ACM_I2S:
		client = HIFI_CLIENT;
		break;
	case ACM_AUX_FM:
		client = FMRADIO_CLINET;
		break;
	default:
		return;
	}

	switch ((ACM_AudioDevice)(*((unsigned short*)data))) {
	case ACM_MAIN_SPEAKER:
		device = EARPIECE_DEVICE;//device
		value |= (OUTPUT << 16);//direction
		break;
	case ACM_AUX_SPEAKER:
		device = SPEAKER_DEVICE;
		value |= (OUTPUT << 16);
		break;
	case ACM_HEADSET_SPEAKER:
		device = HEADSET_DEVICE;
		value |= (OUTPUT << 16);
		break;
	case ACM_BLUETOOTH_SPEAKER:
		device = BLUETOOTH_DEVICE;
		value |= (OUTPUT << 16);
		break;
	case ACM_MIC:
		device = SPEAKER_DEVICE;
		value |= (INPUT << 16);
		break;
	case ACM_HEADSET_MIC:
		device = HEADSET_DEVICE;
		value |= (INPUT << 16);
		break;
	case ACM_BLUETOOTH_MIC:
		device = BLUETOOTH_DEVICE;
		value |= (INPUT << 16);
		break;
	case ACM_MIC_EC:
		device = SPEAKER_DEVICE;
		value |= (INPUT << 16);
		value |= (ECENABLE << 16);
		break;
	case ACM_HEADSET_MIC_EC:
		device = HEADSET_DEVICE;
		value |= (INPUT << 16);
		value |= (ECENABLE << 16);
		break;
	case ACM_AUX_MIC:
		if (client != FMRADIO_CLINET)
			return;
		device = ACM_AUX_SPEAKER;
		value |= (INPUT << 16);
		break;
	case ACM_MAIN_SPEAKER__LOOP:
		device = EARPIECE_DEVICE;//device
		value |= (OUTPUT << 16);//direction
		value |= (LOOPBACKTEST << 16);//loopback test
		break;
	case ACM_AUX_SPEAKER__LOOP:
		device = SPEAKER_DEVICE;
		value |= (OUTPUT << 16);
		value |= (LOOPBACKTEST << 16);//loopback test
		break;
	case ACM_HEADSET_SPEAKER__LOOP:
		device = HEADSET_DEVICE;
		value |= (OUTPUT << 16);
		value |= (LOOPBACKTEST << 16);//loopback test
		break;
	case ACM_MIC__LOOP:
		device = SPEAKER_DEVICE;
		value |= (INPUT << 16);
		value |= (LOOPBACKTEST << 16);//loopback test
	case ACM_AUX_MIC__LOOP:
		device = ACM_AUX_SPEAKER;
		value |= (INPUT << 16);
		value |= (LOOPBACKTEST << 16);//loopback test
		break;
	case ACM_HEADSET_MIC__LOOP:
		device = HEADSET_DEVICE;
		value |= (INPUT << 16);
		value |= (LOOPBACKTEST << 16);//loopback test
		break;
	default:
		return;
	}

	audio_route_control(client, PLUGIN_CTL_WRITE, METHOD_VOLUME_SET, device, value);
}

/*******************************************************************************
* Function: ACMAudioCalibrationStateSet_Env
*******************************************************************************
* Description: Set the phone work state.
*
* Parameters: 0 indicate it will enater normal state,
* 	      1 indicate it will enater calibration state
*
* Return value: void
*
* Notes:
*******************************************************************************/
//ICAT EXPORTED FUNCTION - Audio, ACM, ACMAudioCalibrationStateSet
void ACMAudioCalibrationStateSet_Env(void *data)
{
	int client, value = 0;

	value |= (ACM_AudioCalibrationStatus)(*((unsigned short*)data));/*calibtation state*/

	/*This diag command isn't relate with client type, and we select PHONE_CLIENT to switch the calibration state*/
	client = PHONE_CLIENT;

	DIAG_FILTER(AUDIO, ACM, ACMAudioSetCalibrationStateSet, DIAG_ALL)
	diagPrintf("calibration state: %s\n", value?"ACM_CALIBRATION_ON":"ACM_CALIBRATION_OFF");

	value |= (CALSTATESWITCH << 16);
	audio_route_control(client, PLUGIN_CTL_WRITE, 0, 0, value);
}
