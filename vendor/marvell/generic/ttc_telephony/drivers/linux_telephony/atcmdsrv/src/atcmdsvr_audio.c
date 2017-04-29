/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/
// #include <audio_stub_api.h>
#include "gbl_types.h"
#include "telconfig.h"
#include "pxa_dbg.h"
#include "audio_ipc.h"
#include "audio_protocol.h"
#include "teldef.h"
#include "MarvellAmixer.h"
#include "diag.h"
#include "diag_buff.h"
#include "atcmdsvr_audio.h"

BOOL audioEnabled = FALSE;
INT32 audioMute = TEL_MUTE_OFF;
AUD_LoopbackMethod methodId = AUD_LOOPBACK_DISABLE;
AUD_LoopbackDevice loop_device = AUD_LOOPBACK_EARPIECE;

static void mute_voicecall_audio(INT32 mute, INT32 device_type)
{
	audio_route_control(PHONE_CLIENT, PLUGIN_CTL_WRITE, METHOD_MUTE, device_type, (INPUT << 16) | mute | (TESTMODE << 16));
}

//Successful: return 0; Otherwise, return -1
int tel_set_voicecall_mute(INT32 mute)
{
	if (mute == audioMute)
	{
		/* The voicecall has been muted on or off, return directly */
		return 0;
	}
	else if ((mute == TEL_MUTE_OFF) || (mute == TEL_MUTE_ON))
	{
		mute_voicecall_audio(mute, EARPIECE_DEVICE);
		mute_voicecall_audio(mute, SPEAKER_DEVICE);
		mute_voicecall_audio(mute, HEADSET_DEVICE);
		mute_voicecall_audio(mute, BLUETOOTH_DEVICE);
		mute_voicecall_audio(mute, BT_NREC_DEVICE);
		audioMute = mute;

		return 0;
	}
	else
		return -1;
}

INT32 tel_get_voicecall_mute(void)
{
	return audioMute;
}

void tel_init_voicecall_mute(void)
{
	if (audioMute != TEL_MUTE_OFF)
	{
		mute_voicecall_audio(TEL_MUTE_OFF, EARPIECE_DEVICE);
		mute_voicecall_audio(TEL_MUTE_OFF, SPEAKER_DEVICE);
		mute_voicecall_audio(TEL_MUTE_OFF, HEADSET_DEVICE);
		mute_voicecall_audio(TEL_MUTE_OFF, BLUETOOTH_DEVICE);
		mute_voicecall_audio(TEL_MUTE_OFF, BT_NREC_DEVICE);
	}
	audioMute = TEL_MUTE_OFF;
	return;
}

void enable_voicecall_audio(void)
{
	if (bLocalTest)
	{
		DPRINTF("*****************atcmd: enable_voicecall_audio, enable speaker and headset***************\n");
		/* Don't need to enable it again */
		if (audioEnabled)
			return;

		audio_route_control(PHONE_CLIENT, PLUGIN_CTL_WRITE, METHOD_ENABLE, HEADSET_DEVICE, ((INPUT | OUTPUT) << 16) | 40 | (TESTMODE << 16));
		audioEnabled = TRUE;
	}
}

void disable_voicecall_audio(void)
{
	if (bLocalTest)
	{
		DPRINTF("*****************atcmd: disable_voicecall_audio***************\n");
		/* Don't disable it again */
		if (!audioEnabled)
			return;

		audio_route_control(PHONE_CLIENT, PLUGIN_CTL_WRITE, METHOD_DISABLE, HEADSET_DEVICE, ((INPUT | OUTPUT) << 16) | (TESTMODE << 16));
		audioEnabled = FALSE;
	}
}

//ICAT EXPORTED FUNCTION - VALI_IF,ATCMD_IF,TestATCmdIF
void Atcmd_HW_test(void *data)
{
	UNUSEDPARAM(data);
	DPRINTF("ATCMD DIAG Test:DO Nothing! ha ha ha\n");
	DIAG_FILTER( VALI_IF, ATCMD, ATCMDTEST, DIAG_TEST)
	diagPrintf("Atcmd_HW_test");
}

INT32 tel_set_loopback_state(INT32 method, INT32 device)
{
	UINT32 audio_device;
	if (method == AUD_LOOPBACK_DISABLE)
	{
		if(methodId == AUD_LOOPBACK_DISABLE)
		{
			DPRINTF("==tel_set_loopback_state: loopback test has been disabled=======\n");
			return 1;
		}
		else if(device != loop_device)
		{
			DPRINTF("==tel_set_loopback_state: this device hasn't been enabled=======\n");
			return 1;
		}
		else
		{
			switch (loop_device)
			{
				case AUD_LOOPBACK_EARPIECE:
					audio_device = EARPIECE_DEVICE;
					break;
				case AUD_LOOPBACK_SPEAKER:
					audio_device = SPEAKER_DEVICE;
					break;
				case AUD_LOOPBACK_HEADSET:
					audio_device = HEADSET_DEVICE;
					break;
				default:
					DPRINTF("==tel_set_loopback_state: don't support this device, loop_device=%d=======\n", loop_device);
					return 1;
			}

			audio_route_control(PHONE_CLIENT, PLUGIN_CTL_WRITE, METHOD_DISABLE, \
					audio_device, ((OUTPUT|INPUT|LOOPBACKTEST)<<16));

			stop_loopback();
			methodId = AUD_LOOPBACK_DISABLE;
			loop_device = AUD_LOOPBACK_EARPIECE;
		}

	}
	else if (method == AUD_LOOPBACK_ENABLE)
	{
		if(methodId == AUD_LOOPBACK_ENABLE)
		{
			DPRINTF("==tel_set_loopback_state: loopback test has been Enabled, please disable it firstly=======\n");
			return 1;
		}
		else
		{
			switch (device)
			{
				case AUD_LOOPBACK_EARPIECE:
					audio_device = EARPIECE_DEVICE;
					break;
				case AUD_LOOPBACK_SPEAKER:
					audio_device = SPEAKER_DEVICE;
					break;
				case AUD_LOOPBACK_HEADSET:
					audio_device = HEADSET_DEVICE;
					break;
				default:
					DPRINTF("==tel_set_loopback_state: don't support this device, device=%d=======\n", device);
					return 1;
			}
			audio_route_control(PHONE_CLIENT, PLUGIN_CTL_WRITE, METHOD_ENABLE, \
					audio_device, ((OUTPUT|INPUT|LOOPBACKTEST)<<16)|90);
			start_loopback();
			methodId = AUD_LOOPBACK_ENABLE;
			loop_device = device;
		}
	}
	else
	{
		DPRINTF("==tel_set_loopback_state: don't support this method, method=%d=======\n", method);
		return 1;
	}

	return 0;
}

INT32 tel_get_loopback_state(INT32 * method, INT32 * device)
{
	*method = methodId;
	*device = loop_device;
	return 0;
}
