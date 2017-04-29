/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/

/*******************************************************************************
*               MODULE HEADER FILE
********************************************************************************
* Title: audioLevanteApi
*
* Filename: audioLevanteApi.h
*
* Target, platform: Common Platform, SW platform
*
* Authors: Isar Ariel
*
* Description:
*
* Last Updated:
*
* Notes:
*******************************************************************************/
#ifndef _AUDIO_LEVANTE_API_H_
#define _AUDIO_LEVANTE_API_H_

/*----------- Local include files --------------------------------------------*/
#include "acmtypes.h"
#include "linux_types.h"

/*----------- Extern definition ----------------------------------------------*/
extern ACM_ComponentHandle HWLevanteHandle;


/*----------- Global type definitions ----------------------------------------*/
//ICAT EXPORTED ENUM
typedef enum
{
	// PCM 1 output
	LEVANTE_AnaMIC1_TO_PCM_L          = 0,
	LEVANTE_AnaMIC1_LOUD_TO_PCM_L = 1,
	LEVANTE_AnaMIC2_TO_PCM_L          = 2,
	LEVANTE_DigMIC1_TO_PCM_L          = 3,
	LEVANTE_DigMIC1_LOUD_TO_PCM_L = 4,
	LEVANTE_DigMIC2_TO_PCM_R          = 5,
	LEVANTE_AnaMIC3_TO_PCM_R      = 6,
	LEVANTE_AUX1_TO_PCM_L         = 7,
	LEVANTE_AUX2_TO_PCM_R         = 8,

	// PCM 1 output
	LEVANTE_AnaMIC1_TO_PCM_R      = 9,
	LEVANTE_AnaMIC1_LOUD_TO_PCM_R = 10,
	LEVANTE_AnaMIC2_TO_PCM_R      = 11,
	LEVANTE_DigMIC1_TO_PCM_R      = 12,
	LEVANTE_DigMIC1_LOUD_TO_PCM_R = 13,
	LEVANTE_DigMIC2_TO_PCM_L      = 14,
	LEVANTE_AnaMIC3_TO_PCM_L      = 15,
	LEVANTE_AUX1_TO_PCM_R         = 16,
	LEVANTE_AUX2_TO_PCM_L         = 17,

	// I2S 1 output
	LEVANTE_AnaMIC1_TO_I2S_L      = 18,
	LEVANTE_AnaMIC1_LOUD_TO_I2S_L = 19,
	LEVANTE_AnaMIC2_TO_I2S_L      = 20,
	LEVANTE_DigMIC1_TO_I2S_L      = 21,
	LEVANTE_DigMIC1_LOUD_TO_I2S_L = 22,
	LEVANTE_DigMIC2_TO_I2S_R      = 23,
	LEVANTE_AnaMIC3_TO_I2S_R      = 24,
	LEVANTE_AUX1_TO_I2S_L         = 25,
	LEVANTE_AUX2_TO_I2S_R         = 26,

	// I2S 2 output
	LEVANTE_AnaMIC1_TO_I2S_R      = 27,
	LEVANTE_AnaMIC1_LOUD_TO_I2S_R = 28,
	LEVANTE_AnaMIC2_TO_I2S_R      = 29,
	LEVANTE_DigMIC1_TO_I2S_R      = 30,
	LEVANTE_DigMIC1_LOUD_TO_I2S_R = 31,
	LEVANTE_DigMIC2_TO_I2S_L      = 32,
	LEVANTE_AnaMIC3_TO_I2S_L      = 33,
	LEVANTE_AUX1_TO_I2S_R         = 34,
	LEVANTE_AUX2_TO_I2S_L         = 35,

	// From PCM Left
	LEVANTE_PCM_L_TO_EAR          = 36,
	LEVANTE_PCM_L_TO_SPKR         = 37,
	LEVANTE_PCM_L_TO_HS1          = 38,
	LEVANTE_PCM_L_TO_HS2          = 39,
	LEVANTE_PCM_L_TO_HS           = 40,
	LEVANTE_PCM_L_TO_LINEOUT1     = 41,
	LEVANTE_PCM_L_TO_LINEOUT2     = 42,
	LEVANTE_PCM_L_TO_LINEOUT      = 43,

	// From PCM Right
	LEVANTE_PCM_R_TO_EAR          = 44,
	LEVANTE_PCM_R_TO_SPKR         = 45,
	LEVANTE_PCM_R_TO_HS1          = 46,
	LEVANTE_PCM_R_TO_HS2          = 47,
	LEVANTE_PCM_R_TO_HS           = 48,
	LEVANTE_PCM_R_TO_LINEOUT1     = 49,
	LEVANTE_PCM_R_TO_LINEOUT2     = 50,
	LEVANTE_PCM_R_TO_LINEOUT      = 51,

	// From I2S
	LEVANTE_I2S_TO_EAR            = 52,
	LEVANTE_I2S_TO_SPKR           = 53,
	LEVANTE_I2S_TO_HS1            = 54,
	LEVANTE_I2S_TO_HS2            = 55,
	LEVANTE_I2S_TO_HS             = 56,
	LEVANTE_I2S_TO_LINEOUT1       = 57,
	LEVANTE_I2S_TO_LINEOUT2       = 58,
	LEVANTE_I2S_TO_LINEOUT        = 59,

	// From I2S1
	LEVANTE_I2S1_TO_EAR           = 60,
	LEVANTE_I2S1_TO_SPKR          = 61,
	LEVANTE_I2S1_TO_HS1           = 62,
	LEVANTE_I2S1_TO_HS2           = 63,
	LEVANTE_I2S1_TO_HS            = 64,
	LEVANTE_I2S1_TO_LINEOUT1      = 65,
	LEVANTE_I2S1_TO_LINEOUT2      = 66,
	LEVANTE_I2S1_TO_LINEOUT       = 67,

	// Side Tone
	LEVANTE_IN_L_TO_OUT           = 68,
	LEVANTE_IN_R_TO_OUT           = 69,
	LEVANTE_IN_BOTH_TO_OUT        = 70,

	// Digital Loops
	LEVANTE_PCM_INT_LOOP                  = 71,
	LEVANTE_PCM_EXT_LOOP                  = 72,
	LEVANTE_I2S_INT_LOOP                  = 73,
	LEVANTE_I2S_EXT_LOOP                  = 74,
	LEVANTE_DAC1_IN_EC_LOOP               = 75,
	LEVANTE_DAC2_IN_EC_LOOP               = 76,
	LEVANTE_DAC1_DAC2_IN_EC_LOOP          = 77,
	LEVANTE_PCM_L_IN_TO_I2S_L_OUT_LOOP    = 78,
	LEVANTE_PCM_L_IN_TO_I2S_R_OUT_LOOP    = 79,
	LEVANTE_PCM_L_IN_TO_I2S_Both_OUT_LOOP = 80,
	LEVANTE_PCM_R_IN_TO_I2S_L_OUT_LOOP    = 81,
	LEVANTE_PCM_R_IN_TO_I2S_R_OUT_LOOP    = 82,
	LEVANTE_PCM_R_IN_TO_I2S_Both_OUT_LOOP = 83,
	LEVANTE_I2S_IN_TO_I2S_L_OUT_LOOP      = 84,
	LEVANTE_I2S_IN_TO_I2S_R_OUT_LOOP      = 85,
	LEVANTE_I2S_IN_TO_I2S_Both_OUT_LOOP   = 86,
	LEVANTE_I2S1_IN_TO_I2S_L_OUT_LOOP     = 87,
	LEVANTE_I2S1_IN_TO_I2S_R_OUT_LOOP     = 88,
	LEVANTE_I2S1_IN_TO_I2S_Both_OUT_LOOP  = 89,

	// Analog Loops
	LEVANTE_AUX1_TO_HS2           = 90,
	LEVANTE_AUX1_TO_LOUT2         = 91,
	LEVANTE_AUX2_TO_HS1           = 92,
	LEVANTE_AUX2_TO_LOUT1         = 93,

	//Jackie,2011-0222
	//Loop include codec and MSA
	LEVANTE_PCM_L_TO_EAR_LOOP     		= 94,
	LEVANTE_PCM_L_TO_HS_LOOP      		= 95,
	LEVANTE_PCM_L_TO_SPKR_LOOP  		= 96,
	LEVANTE_AnaMIC1_TO_PCM_L_LOOP 		= 97,
	LEVANTE_AnaMIC2_TO_PCM_L_LOOP 		= 98,
	LEVANTE_AnaMIC1_LOUD_TO_PCM_L_LOOP 	= 99,

	//Aux to speaker use sidetone path
	LEVANTE_AUX1_TO_SIDETONE_L	 = 100,
	LEVANTE_AUX2_TO_SIDETONE_R	 = 101,
	LEVANTE_SIDETONE_IN_TO_OUT	 = 102,
	LEVANTE_SIDETONE_TO_SPKR	 = 103,

	// Loud-Speaker with Headset
	LEVANTE_I2S_TO_SPKR_HS = 104,

	// add path for recording by GSSP
	LEVANTE_AnaMIC1_TO_PCM_I2S_L = 105,
	LEVANTE_AnaMIC1_TO_PCM_I2S_R = 106,
	LEVANTE_AnaMIC1_LOUD_TO_PCM_I2S_L = 107,
	LEVANTE_AnaMIC1_LOUD_TO_PCM_I2S_R = 108,
	LEVANTE_AnaMIC2_TO_PCM_I2S_L = 109,
	LEVANTE_AnaMIC2_TO_PCM_I2S_R = 110,
	LEVANTE_AUX1_TO_PCM_I2S_L = 111,
	LEVANTE_AUX2_TO_PCM_I2S_R = 112,

	LEVANTE_TOTAL_VALID_PATHS,                                      /* Must the be here - counting path amount */
	LEVANTE_DUMMY_PATH,                                             /* for disabling purposes */

	LEVANTE_PATHS_ENUM_32_BIT         = 0x7FFFFFFF                  /* 32bit enum compiling enforcement */
} LevantePaths;

typedef enum
{
	FREQUENCY_SETTINGS = 0,
	EQUALIZER_SETTINGS,
	TRI_STATE_SETTINGS,
	DATA_LENGTH_SETTINGS,
	DIG_MIC_INIT_SETTINGS,
	ANALOG_MIC_INIT_SETTINGS,
	LOOPBACK_TEST_SETTINGS,
	MASTER_SLAVE_MODE_SETTINGS,
	LOUD_SPEAKER_SETTINGS,
	CHARGE_PUMP_CLOCK_SETTINGS,
	FORCE_SPEAKER_GAIN_SETTINGS
} LevanteSettingsType;

typedef enum
{
	LEVANTE_BIT_RATE_5_512_KHZ = 0,
	LEVANTE_BIT_RATE_8_KHZ,
	LEVANTE_BIT_RATE_11_025_KHZ,
	LEVANTE_BIT_RATE_16_KHZ,
	LEVANTE_BIT_RATE_22_05_KHZ,
	LEVANTE_BIT_RATE_32_KHZ,
	LEVANTE_BIT_RATE_44_1_KHZ,
	LEVANTE_BIT_RATE_48_KHZ,
	LEVANTE_BIT_RATE_64_KHZ,
	LEVANTE_BIT_RATE_88_2_KHZ,
	LEVANTE_BIT_RATE_96_KHZ,
	LEVANTE_BIT_RATE_176_4_KHZ,
	LEVANTE_BIT_RATE_192_KHZ,

	LEVANTE_BIT_RATE_OFF
} LEVANTE_I2SbitRate;

typedef enum
{
	LOFI = 0,
	HIFI
} LevanteDigitalPathOption;

typedef enum
{
	PLAY = 0,
	RECORD,
	BOTH_DIRECTIONS
} LevanteFreqDirection;

typedef enum
{
	LEFT = 0,
	RIGHT,
	BOTH_SIDES
} LevanteSide;

typedef enum
{
	PCM_TYPE = 0,
	I2S_TYPE
} LevanteInputType;

typedef enum
{
	LENGTH_16_BIT = 0,
	LENGTH_18_BIT
} LevanteDataLength;

typedef enum
{
	OSR_126 = 0,    /* '0' is the offset for the PCM RATE */
	OSR_84  = 1,    /* '1' is the offset for the PCM RATE */
	OSR_72  = 2     /* '2' is the offset for the PCM RATE */
} LevanteOSR;

typedef enum
{
	PCM_INTERFACE = 0,
	I2S_INTERFACE
} LevanteInterfaceType;

typedef enum
{
	INTERNAL_LOOPBACK = 0,
	EXTERNAL_LOOPBACK
} LevanteLoopbackType;

typedef enum
{
	LOAD_4_OHM = 0,
	LOAD_8_OHM
} LevanteSpeakerLoad;

typedef struct
{
	LevanteDigitalPathOption digitalPath;
	LevanteFreqDirection freqDirection;
	LevanteSide side;
	LEVANTE_I2SbitRate bitRate;
} LevanteFrequencySettingsData;

typedef struct
{
	BOOL isEqualizerEnabled;
	signed short N0, N1;
	signed short D1;
} LevanteEqualizerSettingsData;

typedef struct
{
	BOOL isPcmSdoTriStateEnabled;
	BOOL isI2sSdoTriStateEnabled;
} LevanteTriStateSettingsData;

typedef struct
{
	LevanteInputType inputType;
	LevanteDataLength dataLength;
} LevanteFormatDataLengthSettingsData;

typedef struct
{
	UINT8 micIndex;
	LevanteOSR OSR;
	UINT8 clockDelay;
	UINT8 dataDelay;
} LevanteDigMicInitSettingsData;

typedef struct
{
	UINT8 micBiasCurrent_1_2;
	UINT8 micBiasCurrent_3;
} LevanteAnalogMicInitSettingsData;

typedef struct
{
	LevanteInterfaceType interfaceType;
	LevanteLoopbackType loopbackType;
	BOOL isLoopbackEnabled;
} LevanteLoopbackTestSettingsData;

typedef struct
{
	LevanteInputType inputType;
	BOOL isMasterOnClockAndFrame;
} LevanteMasterSlaveModeSettingsData;

typedef struct
{
	LevanteSpeakerLoad speakerLoad;
} LevanteLoudSpeakerSettingsData;

typedef struct
{
	UINT16 fmFrequency;
} LevanteChargePumpClockSettingsData;

typedef struct
{
	UINT8 headsetVolumeIdx;
	UINT8 speakerVolumeIdx;
} LevanteForceSpeakerGainSettingsData;

/*----------- Global API function definition ---------------------------------*/
void LevanteAdditionalSettingsSet(LevanteSettingsType settingsType, void *settingsData);
void LevanteGetGainData(void **ppACMGains, int *pLen);

#endif /*_AUDIO_LEVANTE_API_H_*/
