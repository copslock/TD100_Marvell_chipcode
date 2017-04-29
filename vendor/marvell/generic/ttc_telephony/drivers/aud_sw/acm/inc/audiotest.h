/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code (“Material? are owned
   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
   transmitted, distributed, or disclosed in any way without Intel’s prior express written permission.

   No license under any patent, copyright, trade secret or other intellectual property right is granted to or
   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
   Intel in writing.
   -------------------------------------------------------------------------------------------------------------------*/

/******************************************************************************
*               MODULE HEADER FILE
*******************************************************************************
* Title:Audio test
*
* Filename: AudioTest.h
*
* Authors:
*
* Description: Header file for AudioTest.c.
*
* Last Updated:
*
* Notes:
******************************************************************************/
#ifndef _AUDIO_TEST_H
#define _AUDIO_TEST_H_



/*----------- Local include files --------------------------------------------*/

#include "acm.h"
#if !defined (OSA_WINCE)
#include "global_types.h"
#endif // OSA_WINCE

#if defined (OSA_WINCE) && !defined (BSP_NODIAG)
#include "RouterInterface.h"
#endif // OSA_WINCE



/*----------- Local defines --------------------------------------------------*/

#define RESTART (0xfff)    // used for the tones
#define FOREVER (0)
#define STOP (-2)
#define AUDIO_TEST_MAX_TONE_COMPONENTS (3)







/*----------- Local type definitions -----------------------------------------*/




//ICAT EXPORTED ENUM


typedef enum Demo_ToneType
{
	AUDIO_TEST_DTMF_TONE_0 = 0
	, AUDIO_TEST_DTMF_TONE_1
	, AUDIO_TEST_DTMF_TONE_2
	, AUDIO_TEST_DTMF_TONE_3
	, AUDIO_TEST_DTMF_TONE_4
	, AUDIO_TEST_DTMF_TONE_5
	, AUDIO_TEST_DTMF_TONE_6
	, AUDIO_TEST_DTMF_TONE_7
	, AUDIO_TEST_DTMF_TONE_8
	, AUDIO_TEST_DTMF_TONE_9
	, AUDIO_TEST_DTMF_TONE_A
	, AUDIO_TEST_DTMF_TONE_B
	, AUDIO_TEST_DTMF_TONE_C
	, AUDIO_TEST_DTMF_TONE_D
	, AUDIO_TEST_DTMF_TONE_asterisk
	, AUDIO_TEST_DTMF_TONE_numSymb
	, AUDIO_TEST_TONE_DIAL_TONE
	, AUDIO_TEST_TONE_BUSY_SIGNAL
	, AUDIO_TEST_TONE_CONGESTION
	, AUDIO_TEST_TONE_RECORDER
	, AUDIO_TEST_TONE_NORMAL_RINGBACK
	, AUDIO_TEST_TONE_PBX_RINGBACK
	, AUDIO_TEST_NUMBER_OF_TONES

} AUDIO_TEST_Tone_Type;

typedef struct _AUDIO_TEST_Tone_Freq
{
	UINT16 time;
	UINT16 freq1;
	UINT16 freq2;

}
AUDIO_TEST_Tone_Freq;


typedef AUDIO_TEST_Tone_Freq AUDIO_TEST_Tone_List [ AUDIO_TEST_MAX_TONE_COMPONENTS ];

/* declaration of library functions */
/************************************/

void AUDIO_TEST_StreamInInit(void);

void AUDIO_TEST_StartCall(void *buff);

void AUDIO_TEST_EndCall(void);

void AUDIO_TEST_SetMicDevice(void *buff);

void AUDIO_TEST_SetSpeakerDevice(void *buff);

void AUDIO_TEST_StartTone(void *buff);

void AUDIO_TEST_EndTone(void);

void AUDIO_TEST_SetVolume(void *buff);

void AUDIO_TEST_SetMute(void *buff);

void AUDIO_TEST_StreamInStart(void *buff);

void AUDIO_TEST_StreamInStop(void *buff);

void AUDIO_TEST_StreamOutStart(void *buff);

void AUDIO_TEST_StreamOutStop(void *buff);

void AUDIO_TEST_StreamOutInit(void);

void AUDIO_TEST_ACMPcmStartBipPlay(void *buff);

void AUDIO_TEST_ACMPcmStopBipPlay(void);

void AUDIO_TEST_ACMStreamPlayDummyStopTest(void *buff);

void AUDIO_TEST_ACMStreamPlayDummyTest(void *buff);

void AUDIO_TEST_ACMLoopBackTestStop(void *data);

void AUDIO_TEST_ACMLoopBackTestStart(void *data);

#ifndef INTEL_2CHIP_PLAT_BVD   //I2S over SSP - single chip

void AUDIO_TEST_ACMI2STest(void *data);

void AUDIO_TEST_ACMI2STestStop(void);

void AUDIO_TEST_VocoderStreamingRecordStop(unsigned int* buff);

void AUDIO_TEST_VocodrStreamingRecordStart(unsigned int *buff);

void AUDIO_TEST_VocoderStreamingPlayStop(void);

void AUDIO_TEST_VocoderStreamingPlayStart(void *buff);


#else  //INTEL_2CHIP_PLAT_BVD

void AUDIO_TEST_I2SDriverPlayRecordBuff(unsigned int *buff);

void AUDIO_TEST_I2SDriverRecordStart(unsigned int *buff);

void AUDIO_TEST_I2SDriverPlayBuffStartSingle(unsigned int *buff);

void AUDIO_TEST_I2SDriverPlayBuffStart(unsigned int *buff);

void AUDIO_TEST_I2SDriverRecBuffStop(void);

#endif  //INTEL_2CHIP_PLAT_BVD


#endif  /* _DEMO_ACM_H_ */











































































































