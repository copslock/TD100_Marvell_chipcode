/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

#ifndef __ATCMDSRV_AUDIO_H__
#define __ATCMDSRV_AUDIO_H__

typedef enum
{
	AUD_LOOPBACK_DISABLE = 0,
	AUD_LOOPBACK_ENABLE,

	AUD_LOOPBACK_METHOD_ENUM_32_BIT = 0x7FFFFFFF
} AUD_LoopbackMethod;

typedef enum
{
	AUD_LOOPBACK_EARPIECE = 0,
	AUD_LOOPBACK_SPEAKER,
	AUD_LOOPBACK_HEADSET,

	AUD_LOOPBACK_DEVICE_ENUM_32_BIT = 0x7FFFFFFF
} AUD_LoopbackDevice;

#endif
