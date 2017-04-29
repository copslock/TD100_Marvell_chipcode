/*------------------------------------------------------------
(C) Copyright [2006-2008] Marvell International Ltd.
All Rights Reserved
------------------------------------------------------------*/

/******************************************************************************
*               MODULE IMPLEMENTATION FILE
*******************************************************************************
* Title: audio sample
*
* Filename: IPPEqWrapper.h
*
* Target, platform: Common Platform, SW platform
*
* Authors: Isar Ariel, Israel Davidenko
*
* Description: Header file for sample code for NVM access.
*
* Last Updated:
*
* Notes:
******************************************************************************/
#ifndef _IPPEQWRAPPER_H_
#define _IPPEQWRAPPER_H_

#define NVM_AUDIO_DIR                       "/data/Linux/Marvell/NVM/"

#define AUDIO_EQ_HIFI_TABLE_FILENAME        "audio_hifi_eq.nvm"
#define AUDIO_HIFI_VOLUME_TABLE_FILENAME    "audio_hifi_volume.nvm"

typedef enum
{
	SPEAKER_PHONE,
	HANDSET,
	HEADSET,
	CAR_KIT,
	BT_HEADSET,
        SPEAKER_PHONE_HP,
        MAX_APPLIANCE_VALUE,

	APPLIANCE_TYPE_ENUM_32_BIT = 0x7FFFFFFF  /* 32bit enum compiling enforcement */
} Appliance;

typedef enum
{
	ON,
	OFF,

	ON_OFF_ENUM_32_BIT = 0x7FFFFFFF  /* 32bit enum compiling enforcement */
} On_Off;

typedef enum
{
	MP3,
	AAC,
	OGG,
	MP4,
	AVI,

	CONTAINER_TYPE_ENUM_32_BIT = 0x7FFFFFFF  /* 32bit enum compiling enforcement */
} Container_Type;

typedef enum
{
	SYSTEM,
	RINGTONE,
	MUSIC,
	ALARM,

	STREAM_TYPE_ENUM_32_BIT = 0x7FFFFFFF  /* 32bit enum compiling enforcement */
} Stream_Type;

typedef struct
{
	unsigned short noOfTaps;
	signed short   params[6];
	Appliance      appliance;
	On_Off         onOff;
	unsigned char  description[32];
} AUDIO_HiFi_EQ_Params;

typedef struct
{
    Container_Type containerType;
    Stream_Type    streamType;
    unsigned short volumeOffset;
} AUDIO_HiFi_Volume_Params;

#ifdef __cplusplus
extern "C" {
#endif

int EqTransformPacket(void* bufferInOut, size_t bytesIn, size_t channel);
void EqReset();
void reloadFDI();

#ifdef __cplusplus
};
#endif

#endif  /* _IPPEQWRAPPER_H_*/


