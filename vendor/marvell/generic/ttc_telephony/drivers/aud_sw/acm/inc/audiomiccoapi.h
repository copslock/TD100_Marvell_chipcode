/*******************************************************************************
*               MODULE HEADER FILE
********************************************************************************
* Title: audioMICCOApi
*
* Filename: audioMICCOApi.h
*
* Target, platform: Common Platform, SW platform
*
* Authors: Yossi Gabay (BlueGrove driver), Tal Klap (Modifications for MICCO)
*
* Description:
*
* Last Updated:
*
* Notes:
*******************************************************************************/
#ifndef _AUDIO_MICCO_API_H_
#define _AUDIO_MICCO_API_H_

#include "acmtypes.h"

extern ACM_ComponentHandle HWMICCOHandle;

//IN MICCO HW CALLED TRANSMIT PATHS

//ICAT EXPORTED ENUM
typedef enum
{
	// Note: if you change the sequence of the following paths, pls update ccrHWUpdatePackageHandle also
	// ----------------------------------------------------------
	//|   path :    |    number: | default lane:  |	description: |
	// ----------------------------------------------------------
	// ----------------------------------------------------------
	// PCM output
	MICCO_PCM_TO_MONO                               =  0,           // PCM SDIn to SP 8
	MICCO_PCM_TO_BEAR                               =  1,           // PCM SDIn to SP 32
	MICCO_PCM_TO_SPKRL                              =  2,           // PCM SDIn to Mono Headset
	MICCO_PCM_TO_SPKRR                              =  3,           // PCM SDIn to Mono Headset
	MICCO_PCM_TO_SPKR                               =  4,           // PCM SDIn to Stereo Headset
	MICCO_PCM_TO_LINEOUT                    =  5,                   // PCM SDIn to Line out

	// HiFi output
	MICCO_I2S_TO_SPKRL                      =  6,                   // Hifi  I2S to Mono Headset
	MICCO_I2S_TO_SPKRR                              =  7,           // Hifi  I2S to Mono Headset
	MICCO_I2S_TO_SPKR                               =  8,           // Hifi  I2S to Stereo Headset
	MICCO_I2S_TO_MONO                               =  9,           // Hifi  I2S to SP 8
	MICCO_I2S_TO_BEAR                               = 10,           // Hifi  I2S to SP 32
	MICCO_I2S_TO_LINEOUT                    = 11,                   // Hifi  I2S to Line out
	MICCO_I2S_TO_SPKR_MONO                  = 12,                   // Hifi  I2S to Stereo Headset + SP 8
	MICCO_I2S_TO_SPKRL_MONO                 = 13,                   // Hifi  I2S to Mono Headset + SP 8
	MICCO_I2S_TO_SPKRR_MONO                 = 14,                   // Hifi  I2S to Mono Headset + SP 8
	MICCO_IN_TO_OUT                         = 15,                   // Side Tone On

	// TO PCM
	MICCO_MIC1_TO_PCM                               = 16,           // Handset microphone
	MICCO_MIC2_TO_PCM                               = 17,           // Headset microphone
	MICCO_AUX3_TO_PCM                               = 18,           // Carkit record
	MICCO_AUX1_TO_PCM                               = 19,           // FM radio record
	MICCO_AUX2_TO_PCM                               = 20,           // FM radio record

	// ADDITIONAL LOOPS
	MICCO_PCM_LOOP                                  = 21,           // Loop - SDI to SDO connection
	MICCO_I2S_DAC1_TO_PCM                   = 22,                   // Loop - Dac1 (HiFi) to PCM
	MICCO_I2S_DAC2_TO_PCM                   = 23,                   // Loop - Dac2 (HiFi) to PCM
	MICCO_MIC1_TO_LINEOUT                   = 24,                   // Loop - MicInP to LineOut
	MICCO_MIC1_TO_LINEOUT_PCM               = 25,                   // Loop - MicInP to LineOut and to PCM
	MICCO_MIC2_TO_LINEOUT                   = 26,                   // Loop - MicInP to LineOut
	MICCO_MIC2_TO_LINEOUT_PCM               = 27,                   // Loop - MicInP to LineOut and to PCM

	// AUX TO OUT
	MICCO_AUX1_TO_MONO                              = 28,                   // Loop - Aux1 to Mono
	MICCO_AUX1_TO_MONO_PCM                  = 29,                           // Loop - Aux1 to Mono and to PCM
	MICCO_AUX1_TO_BEAR                              = 30,                   // Loop - Aux1 to Bear
	MICCO_AUX1_TO_BEAR_PCM                  = 31,                           // Loop - Aux1 to Bear and to PCM
	MICCO_AUX1_TO_SPKRL                             = 32,                   // Loop - Aux1 to Stereo left
	MICCO_AUX1_TO_SPKRL_PCM                 = 33,                           // Loop - Aux1 to Stereo left and to PCM
	MICCO_AUX1_TO_SPKRR                             = 34,                   // Loop - Aux1 to Stereo right
	MICCO_AUX1_TO_SPKRR_PCM                 = 35,                           // Loop - Aux1 to Stereo right and to PCM
	MICCO_AUX1_TO_SPKR                              = 36,                   // Loop - Aux1 to stereo
	MICCO_AUX1_TO_SPKR_PCM                  = 37,                           // Loop - Aux1 to stereo and to PCM
	MICCO_AUX1_TO_LINEOUT                   = 38,                           // Loop - Aux1 to LineOut
	MICCO_AUX1_TO_LINEOUT_PCM               = 39,                           // Loop - Aux1 to LineOut and to PCM

	MICCO_AUX2_TO_MONO                              = 40,                   // Loop - Aux2 to Mono
	MICCO_AUX2_TO_MONO_PCM                  = 41,                           // Loop - Aux2 to Mono and to PCM
	MICCO_AUX2_TO_BEAR                              = 42,                   // Loop - Aux2 to Bear
	MICCO_AUX2_TO_BEAR_PCM                  = 43,                           // Loop - Aux2 to Bear and to PCM
	MICCO_AUX2_TO_SPKRL                             = 44,                   // Loop - Aux2 to Stereo left
	MICCO_AUX2_TO_SPKRL_PCM                 = 45,                           // Loop - Aux2 to Stereo left and to PCM
	MICCO_AUX2_TO_SPKRR                             = 46,                   // Loop - Aux2 to Stereo right
	MICCO_AUX2_TO_SPKRR_PCM                 = 47,                           // Loop - Aux2 to Stereo right and to PCM
	MICCO_AUX2_TO_SPKR                              = 48,                   // Loop - Aux2 to stereo
	MICCO_AUX2_TO_SPKR_PCM                  = 49,                           // Loop - Aux2 to stereo and to PCM
	MICCO_AUX2_TO_LINEOUT                   = 50,                           // Loop - Aux2 to LineOut
	MICCO_AUX2_TO_LINEOUT_PCM               = 51,                           // Loop - Aux2 to LineOut and to PCM

	MICCO_AUX3_TO_MONO                              = 52,                   // Loop - Aux3 to Mono
	MICCO_AUX3_TO_MONO_PCM                  = 53,                           // Loop - Aux3 to Mono and to PCM
	MICCO_AUX3_TO_BEAR                              = 54,                   // Loop - Aux3 to Bear
	MICCO_AUX3_TO_BEAR_PCM                  = 55,                           // Loop - Aux3 to Bear and to PCM
	MICCO_AUX3_TO_SPKRL                             = 56,                   // Loop - Aux3 to Stereo left
	MICCO_AUX3_TO_SPKRL_PCM                 = 57,                           // Loop - Aux3 to Stereo left and to PCM
	MICCO_AUX3_TO_SPKRR                             = 58,                   // Loop - Aux3 to Stereo right
	MICCO_AUX3_TO_SPKRR_PCM                 = 59,                           // Loop - Aux3 to Stereo right and to PCM
	MICCO_AUX3_TO_SPKR                              = 60,                   // Loop - Aux3 to stereo
	MICCO_AUX3_TO_SPKR_PCM                  = 61,                           // Loop - Aux3 to stereo and to PCM
	MICCO_AUX3_TO_LINEOUT                   = 62,                           // Loop - Aux3 to LineOut
	MICCO_AUX3_TO_LINEOUT_PCM               = 63,                           // Loop - Aux3 to LineOut and to PCM

	MICCO_TOTAL_VALID_PATHS,                                                // Must the be here - counting path amount
	MICCO_DUMMY_PATH,                                                       // for disabling purposes

	MICCO_PATHS_ENUM_32_BIT                 = 0x7FFFFFFF                    //32bit enum compiling enforcement
} MICCOPaths;

typedef enum
{
	MICCO_BIT_RATE_8_KHZ                        = 0,
	MICCO_BIT_RATE_11_025_KHZ,
	MICCO_BIT_RATE_16_KHZ,
	MICCO_BIT_RATE_22_05_KHZ,
	MICCO_BIT_RATE_44_1_KHZ,
	MICCO_BIT_RATE_48_KHZ,

	MICCO_BIT_RATE_12_KHZ,
	MICCO_BIT_RATE_24_KHZ,
	MICCO_BIT_RATE_32_KHZ,

	MICCO_BIT_RATE_OFF
} MICCO_I2SbitRate;


/* API*/
void MICCOI2SBitRateSet(MICCO_I2SbitRate bitRate);

int     MICCOIsExists(void);

#endif /*_AUDIO_MICCO_API_H_*/










