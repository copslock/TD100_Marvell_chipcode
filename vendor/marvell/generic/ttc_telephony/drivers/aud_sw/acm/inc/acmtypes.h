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
* Title: Audio Component Manager (ACM)
*
* Filename: acmTypes.h
*
* Authors: Isar Ariel
*
* Description: ACM typedefs
*
* Last Updated: Jun 11 2009 By Paul Shen
*
* Notes:
******************************************************************************/


#ifndef _ACM_TYPES_H_
#define _ACM_TYPES_H_

typedef enum
{
	AD6521_COMPONENT = 0,
	BGV_COMPONENT,
	MANDY_COMPONENT,
	CCR_COMPONENT,
	ARAVA_COMPONENT,
	BLUETOOTH_COMPONENT,
	DAI_COMPONENT,
	MICCO_COMPONENT,
	GPO_COMPONENT,
	LEVANTE_COMPONENT,
	NULL_COMPONENT,                                 //must be last one

	ACM_COMPONENTS_ENUM_32_BIT  = 0x7FFFFFFF        //32bit enum compiling enforcement
} ACM_Component;

typedef unsigned char ACM_AudioVolume;  /* (0 - 100%) */
typedef signed char ACM_DigitalGain;
typedef signed char ACM_AnalogGain;

#define INVALID_DIGITAL_GAIN    ((ACM_DigitalGain)0x7F)

#define DIGITAL_GAIN_MAGICNUM    0xdeadbeef

typedef enum
{
	ACM_MUTE_OFF = 0,
	ACM_MUTE_ON = 1,

	ACM_AUDIO_MUTE_ENUM_32_BIT        = 0x7FFFFFFF //32bit enum compiling enforcement
} ACM_AudioMute;


typedef struct
{
	unsigned short shift;
	unsigned short SSCR0_HIGH;
	unsigned short SSCR0_LOW;
	unsigned short SSCR1_HIGH;
	unsigned short SSCR1_LOW;
	unsigned short SSTSA_LOW;
	unsigned short SSRSA_LOW;
	unsigned short SSPSP_HIGH;
	unsigned short SSPSP_LOW;

//Tavor only:
	unsigned short SSACD_LOW;
	unsigned short SSACDD_HIGH;
	unsigned short SSACDD_LOW;

//SSP Port
	unsigned short devicePort;      /* (GSSP0_PORT=0, GSSP1_PORT=1) */
	unsigned short SSP_Rate;        /* (0=8 kHz, 1=16 kHz)    */
	unsigned short SSP_Mode;        /* (0=unpacked, 1=packed) */
} ACM_SspAudioConfiguration;

typedef enum
{
	ACM_CALIBRATION_OFF = 0,
	ACM_CALIBRATION_ON = 1,

	ACM_AUDIO_CALIBRATION_STATUS_ENUM_32_BIT        = 0x7FFFFFFF //32bit enum compiling enforcement
} ACM_AudioCalibrationStatus;

typedef    void (*ACMComponentInit_t)(unsigned char reinit, ACM_SspAudioConfiguration **componentSspConfig, signed short **ditherGenConfig);
typedef    ACM_DigitalGain (*ACMEnablePath_t)(unsigned char path, ACM_AudioVolume volume);
typedef    ACM_DigitalGain (*ACMDisablePath_t)(unsigned char path);
typedef    ACM_DigitalGain (*ACMVolumeSetPath_t)(unsigned char path, ACM_AudioVolume volume);
typedef    ACM_DigitalGain (*ACMMutePath_t)(unsigned char path, ACM_AudioMute mute);
typedef    ACM_DigitalGain (*ACMSetPathAnalogGain_t)(unsigned char regIndex, unsigned char regValue, unsigned char regMask, unsigned char regShift);
typedef    short           (*ACMGetPathsStatus_t) (char* data, short length);
typedef    ACM_AnalogGain  (*ACMGetPathAnalogGain_t)    (unsigned char path);
typedef    unsigned int    (*ACMGetActivePathNum_t)(void);
typedef    unsigned int    (*ACMSetLoopbackTestStatus_t)(unsigned char isloopbacktest);
typedef    unsigned int    (*ACMGetLoopbackTestStatus_t)(void);

typedef struct
{
	ACMComponentInit_t ACMComponentInit;
	ACMEnablePath_t ACMEnablePath;
	ACMDisablePath_t ACMDisablePath;
	ACMVolumeSetPath_t ACMVolumeSetPath;
	ACMMutePath_t ACMMutePath;
	ACMGetPathsStatus_t	 ACMGetPathsStatus;
	ACMGetPathAnalogGain_t	 ACMGetPathAnalogGain;
	ACMSetPathAnalogGain_t ACMSetPathAnalogGain;
	ACMGetActivePathNum_t ACMGetActivePathNum;
	ACMSetLoopbackTestStatus_t ACMSetLoopbackTestStatus;
	ACMGetLoopbackTestStatus_t ACMGetLoopbackTestStatus;
} ACM_ComponentHandle;


#endif  /* _ACM_TYPES_H_ */


