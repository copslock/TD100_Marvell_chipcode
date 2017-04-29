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
*               MODULE IMPLEMENTATION FILE
*******************************************************************************
* Title: ACM (Audio Component Manager) Test Functions
*
* Filename: acmTest.c
*
* Target, platform: Common Platform, SW platform
*
* Authors: Yishai Gil, Isar Ariel
*
* Description: Includes all ACM test function.
*
* Last Updated:
*
* Notes:
******************************************************************************/

/*----------- External include files -----------------------------------------*/


/*----------- Local include files --------------------------------------------*/
#include "acm.h"
#include "diags.h"
#include "osa.h"

#include "stdlib.h"
#include "acm_fdi_ext.h"
#include "acm_fdi_file.h"
#include "acm_fdi_type.h"
#include "acm_fdi_err.h"

#if defined (OSA_WINCE)
#include "acm_wince.h"
#endif //(OSA_WINCE)

#if defined (OSA_LINUX)
#include "acm_linux.h"
#endif


/*----------- Local defines --------------------------------------------------*/
void playI2SFromFDIOneShot(const char *filename, unsigned short rate);
void playI2SFromFDILoop(const char *filename, unsigned short rate);


/*----------- Local macro definitions ----------------------------------------*/

/*----------- External function definition -----------------------------------*/

//Note: apps should use ACM I2S API only
#if !defined (INTEL_2CHIP_PLAT) && !defined (OSA_WINCE)
extern void  I2SOneShot(void);
extern void  I2SLoop(void);
#endif //2-chip

#if defined (INTEL_2CHIP_PLAT)
extern void StreamingTickFreqSet(UINT32 streamId, UINT16 tickFreq);
#endif

/*----------- Local type definitions -----------------------------------------*/


/*----------- Global constant definitions ------------------------------------*/
#define I2S_TEST_FILENAME                               "audio_i2s_test.bin"
#define PCM_PLAT_TEST_FILENAME                          "audio_pcm_test.bin"
#define PCM_BUFF_SIZE                                           160
#define CHUNK_SIZE_IN_SAMPLES                           2304
#define DELAY_FRAMES_IN_LOOPBACK            50
#define NUM_OF_LOOPBACK_FRAMES              (DELAY_FRAMES_IN_LOOPBACK + 10)  /* Add few frames spare for "80ms tick" change */


/*----------- Local variable definitions -------------------------------------*/

const static unsigned short tonesMelody[5][2] =
{
	{ 697, 1209 },
	{ 770, 1336 },
	{ 852, 1477 },
	{ 941, 1633 },
	{ 0,   0    }
};

static __align(32) unsigned short toneBuff[PCM_BUFF_SIZE] = {
	0x0000, 0x004B, 0x007A, 0x007A, 0x004B, 0x0000, 0xFFB5, 0xFF86, 0xFF86, 0xFFB5,
	0x0000, 0x004B, 0x007A, 0x007A, 0x004B, 0x0000, 0xFFB5, 0xFF86, 0xFF86, 0xFFB5,
	0x0000, 0x004B, 0x007A, 0x007A, 0x004B, 0x0000, 0xFFB5, 0xFF86, 0xFF86, 0xFFB5,
	0x0000, 0x004B, 0x007A, 0x007A, 0x004B, 0x0000, 0xFFB5, 0xFF86, 0xFF86, 0xFFB5,
	0x0000, 0x004B, 0x007A, 0x007A, 0x004B, 0x0000, 0xFFB5, 0xFF86, 0xFF86, 0xFFB5,
	0x0000, 0x004B, 0x007A, 0x007A, 0x004B, 0x0000, 0xFFB5, 0xFF86, 0xFF86, 0xFFB5,
	0x0000, 0x004B, 0x007A, 0x007A, 0x004B, 0x0000, 0xFFB5, 0xFF86, 0xFF86, 0xFFB5,
	0x0000, 0x004B, 0x007A, 0x007A, 0x004B, 0x0000, 0xFFB5, 0xFF86, 0xFF86, 0xFFB5,
	0x0000, 0x004B, 0x007A, 0x007A, 0x004B, 0x0000, 0xFFB5, 0xFF86, 0xFF86, 0xFFB5,
	0x0000, 0x004B, 0x007A, 0x007A, 0x004B, 0x0000, 0xFFB5, 0xFF86, 0xFF86, 0xFFB5,
	0x0000, 0x004B, 0x007A, 0x007A, 0x004B, 0x0000, 0xFFB5, 0xFF86, 0xFF86, 0xFFB5,
	0x0000, 0x004B, 0x007A, 0x007A, 0x004B, 0x0000, 0xFFB5, 0xFF86, 0xFF86, 0xFFB5,
	0x0000, 0x004B, 0x007A, 0x007A, 0x004B, 0x0000, 0xFFB5, 0xFF86, 0xFF86, 0xFFB5,
	0x0000, 0x004B, 0x007A, 0x007A, 0x004B, 0x0000, 0xFFB5, 0xFF86, 0xFF86, 0xFFB5,
	0x0000, 0x004B, 0x007A, 0x007A, 0x004B, 0x0000, 0xFFB5, 0xFF86, 0xFF86, 0xFFB5,
	0x0000, 0x004B, 0x007A, 0x007A, 0x004B, 0x0000, 0xFFB5, 0xFF86, 0xFF86, 0xFFB5
};

static int toneCounter1 = 0, toneCounter2 = 0, dummyCnt1 = 0, dummyCnt2 = 0;
static __align(4) unsigned short _txFrameBuf[19] = { 0 };
static unsigned short *_pcmBufLoopPtr;
//3 sec (50 frames/sec) of memo test buffer
static __align(4) unsigned short _memoFrameBuf[3 * 50][19] = { 0 };
static unsigned short *_PcmBuf = NULL;
static unsigned short *_PcmBuf_1 = NULL, *_PcmBuf_2 = NULL, *_PcmBuf_3 = NULL, *_PcmBuf_4 = NULL;


/*----------- Global extrenal variable definition -------------------------------------*/
/*----------- Local constant definitions -------------------------------------*/
/*----------- Local function definition -------------------------------------*/
/*----------- Global function definition -------------------------------------*/

static int frameCount;

static void pcmPlayForLoop(unsigned int *buff)
{
	int i;
	static BOOL startPlayLoop = FALSE;
	static int framePlay;

	if ( !startPlayLoop && (frameCount >= DELAY_FRAMES_IN_LOOPBACK) )
	{       /* Delay of 50 frames between record and playback */
		framePlay = 0;
		startPlayLoop = TRUE;
	}

	if (startPlayLoop)
	{
		memcpy(buff, &_pcmBufLoopPtr[framePlay * PCM_BUFF_SIZE], PCM_BUFF_SIZE * sizeof(unsigned short));

		if (++framePlay >= NUM_OF_LOOPBACK_FRAMES)
			framePlay = 0;  /* start from the beginning */
	}
	else
	{
		for (i = 0; i < PCM_BUFF_SIZE; i++)
			((UINT16 *)buff)[i] = 0;
	}
}


static void pcmRecordForLoop(unsigned int *buff)
{
	if (buff != NULL)
	{
		memcpy(&_pcmBufLoopPtr[frameCount * PCM_BUFF_SIZE], buff, PCM_BUFF_SIZE * sizeof(unsigned short));

		if (++frameCount >= NUM_OF_LOOPBACK_FRAMES)
			frameCount = 0;  /* start from the beginning */
	}
}


//ICAT EXPORTED FUNCTION - Audio, ACM, ACMLoopBackTestStop
void ACMLoopBackTestStop(void *data)
{
	free((void *)_pcmBufLoopPtr);

	ACMAudioDeviceDisable(ACM_INPUT_TEST_DEVICE, ACM_MSA_PCM);
	ACMAudioStreamInStop(pcmRecordForLoop);

	ACMAudioDeviceDisable(ACM_OUTPUT_TEST_DEVICE, ACM_MSA_PCM);
	ACMAudioStreamOutStop(pcmPlayForLoop);
}


//ICAT EXPORTED FUNCTION - Audio, ACM, ACMLoopBackTestStart
void ACMLoopBackTestStart(void *data)
{
	frameCount = 0;

	_pcmBufLoopPtr = (unsigned short *)malloc((UINT32)(NUM_OF_LOOPBACK_FRAMES * PCM_BUFF_SIZE * sizeof(unsigned short)));

	ACMAudioDeviceEnable(ACM_INPUT_TEST_DEVICE, ACM_MSA_PCM, 75);
	ACMAudioStreamInStart(ACM_PCM, ACM_NEAR_END, pcmRecordForLoop);

	ACMAudioDeviceEnable(ACM_OUTPUT_TEST_DEVICE, ACM_MSA_PCM, 75);
	ACMAudioStreamOutStart(ACM_PCM, ACM_NEAR_END, ACM_NOT_COMB_WITH_CALL, pcmPlayForLoop);
}


static void PcmRecord(unsigned int *buff)
{
	static int i = 0;

	if (( buff != NULL) && (_PcmBuf != NULL))
	{
		memcpy(&_PcmBuf[i * PCM_BUFF_SIZE], buff, PCM_BUFF_SIZE * sizeof(unsigned short));
		i++;
		if (i == 100)
		{
			i = 0;
		}

	}
}

static void PcmBipPlay(unsigned int *buff)
{
	if ( buff != NULL)
	{
		memcpy(buff, toneBuff, PCM_BUFF_SIZE * sizeof(unsigned short));
	}
}


static void PcmRecBufPlay(unsigned int *buff)
{
	static int i = 0;

	if (( buff != NULL) && (_PcmBuf != NULL))
	{
		memcpy(buff, &_PcmBuf[i * PCM_BUFF_SIZE], PCM_BUFF_SIZE * sizeof(unsigned short));
		i++;
		if (i == 100)
		{
			i = 0;
		}
	}
}


//ICAT EXPORTED FUNCTION - Audio,ACM,ACMPcmStartRecord
void ACMPcmStartRecord(void *data)
{
	unsigned short idx = (*(unsigned short *)data);

	if ( (idx & 0x10) == 0)
		ACMAudioDeviceEnable(ACM_INPUT_TEST_DEVICE, ACM_MSA_PCM, 75);

	idx &= ~0x10; /* Turn off the "non enable/disable" bit */

	//allocated once in a sw life time w/o free
	if ( _PcmBuf == NULL)
	{
		_PcmBuf = (unsigned short *)alignMalloc(PCM_BUFF_SIZE * 101 * sizeof(unsigned short));
		memset((unsigned char *)_PcmBuf, '\0', PCM_BUFF_SIZE * 101 * sizeof(unsigned short));
		ACM_ASSERT(_PcmBuf != NULL);
	}

	switch (idx)
	{
	case 0:
		ACMAudioStreamInStart(ACM_PCM, ACM_NEAR_END, PcmRecord);
		break;
	case 1:
		ACMAudioStreamInStart(ACM_PCM, ACM_FAR_END, PcmRecord);
		break;
	case 2:
		ACMAudioStreamInStart(ACM_PCM, ACM_BOTH_ENDS, PcmRecord);
		break;
	}
}

//ICAT EXPORTED FUNCTION - Audio,ACM,ACMPcmStopRecord
void ACMPcmStopRecord(void *data)
{
	unsigned short idx = (*(unsigned short *)data);

	if ( (idx & 0x10) == 0 )
		ACMAudioDeviceDisable(ACM_INPUT_TEST_DEVICE, ACM_MSA_PCM);

	ACMAudioStreamInStop(PcmRecord);
}


//ICAT EXPORTED FUNCTION - Audio,ACM,ACMPcmStartRecBufPlay
void ACMPcmStartRecBufPlay(void *data)
{
	unsigned short idx = (*(unsigned short *)data);

	if ( (idx & 0x10) == 0)
		ACMAudioDeviceEnable(ACM_OUTPUT_TEST_DEVICE, ACM_MSA_PCM, 75);

	idx &= ~0x10; /* Turn off the "non enable/disable" bit */

	//allocated once in a sw life time w/o free
	if ( _PcmBuf == NULL)
	{
		_PcmBuf = (unsigned short *)alignMalloc(PCM_BUFF_SIZE * 101 * sizeof(unsigned short));
		memset((unsigned char *)_PcmBuf, '\0', PCM_BUFF_SIZE * 101 * sizeof(unsigned short));
		ACM_ASSERT(_PcmBuf != NULL);
	}

	switch (idx)
	{
	case 0:
		ACMAudioStreamOutStart(ACM_PCM, ACM_NEAR_END, ACM_NOT_COMB_WITH_CALL, PcmRecBufPlay);
		break;
	case 1:
		ACMAudioStreamOutStart(ACM_PCM, ACM_FAR_END, ACM_NOT_COMB_WITH_CALL, PcmRecBufPlay);
		break;
	case 2:
		ACMAudioStreamOutStart(ACM_PCM, ACM_BOTH_ENDS, ACM_NOT_COMB_WITH_CALL, PcmRecBufPlay);
		break;
	case 3:
		ACMAudioStreamOutStart(ACM_PCM, ACM_NEAR_END, ACM_COMB_WITH_CALL, PcmRecBufPlay);
		break;
	case 4:
		ACMAudioStreamOutStart(ACM_PCM, ACM_FAR_END, ACM_COMB_WITH_CALL, PcmRecBufPlay);
		break;
	case 5:
		ACMAudioStreamOutStart(ACM_PCM, ACM_BOTH_ENDS, ACM_COMB_WITH_CALL, PcmRecBufPlay);
		break;
	}
}

//ICAT EXPORTED FUNCTION - Audio,ACM,ACMPcmStopRecBufPlayPlay
void ACMPcmStopRecBufPlayPlay(void *data)
{
	unsigned short idx = (*(unsigned short *)data);

	if ( (idx & 0x10) == 0 )
		ACMAudioDeviceDisable(ACM_OUTPUT_TEST_DEVICE, ACM_MSA_PCM);

	ACMAudioStreamOutStop(PcmRecBufPlay);
}


//ICAT EXPORTED FUNCTION - Audio,ACM,ACMPcmStartBipPlay
void ACMPcmStartBipPlay(void *data)
{
	unsigned short idx = (*(unsigned short *)data);

	if ( (idx & 0x10) == 0)
		ACMAudioDeviceEnable(ACM_OUTPUT_TEST_DEVICE, ACM_MSA_PCM, 75);

	idx &= ~0x10; /* Turn off the "non enable/disable" bit */

	switch (idx)
	{
	case 0:
		ACMAudioStreamOutStart(ACM_PCM, ACM_NEAR_END, ACM_NOT_COMB_WITH_CALL, PcmBipPlay);
		break;
	case 1:
		ACMAudioStreamOutStart(ACM_PCM, ACM_FAR_END, ACM_NOT_COMB_WITH_CALL, PcmBipPlay);
		break;
	case 2:
		ACMAudioStreamOutStart(ACM_PCM, ACM_BOTH_ENDS, ACM_NOT_COMB_WITH_CALL, PcmBipPlay);
		break;
	case 3:
		ACMAudioStreamOutStart(ACM_PCM, ACM_NEAR_END, ACM_COMB_WITH_CALL, PcmBipPlay);
		break;
	case 4:
		ACMAudioStreamOutStart(ACM_PCM, ACM_FAR_END, ACM_COMB_WITH_CALL, PcmBipPlay);
		break;
	case 5:
		ACMAudioStreamOutStart(ACM_PCM, ACM_BOTH_ENDS, ACM_COMB_WITH_CALL, PcmBipPlay);
		break;
	}

}

//ICAT EXPORTED FUNCTION - Audio,ACM,ACMPcmStopBipPlay
void ACMPcmStopBipPlay(void *data)
{
	unsigned short idx = (*(unsigned short *)data);

	if ( (idx & 0x10) == 0 )
		ACMAudioDeviceDisable(ACM_OUTPUT_TEST_DEVICE, ACM_MSA_PCM);

	ACMAudioStreamOutStop(PcmBipPlay);
}




static void DummyPlayFunc1(unsigned int *buff)
{
	dummyCnt1++;
	DIAG_FILTER(AUDIO, ACM, DummyPlayFunc1, DIAG_ERROR)
	diagPrintf("Dummy Stream #1 cb called %d times", dummyCnt1);
}

static void DummyPlayFunc2(unsigned int *buff)
{
	dummyCnt2++;
	DIAG_FILTER(AUDIO, ACM, DummyPlayFunc2, DIAG_ERROR)
	diagPrintf("Dummy Stream #2 cb called %d times", dummyCnt2);
}



//ICAT EXPORTED FUNCTION - Audio,ACM,ACM_StreamDummyPlayStop
void ACMStreamPlayDummyStopTest(void *data)
{
	if ((*(unsigned short *)data) == 0)
	{
		ACMAudioStreamOutStop(DummyPlayFunc1);
		dummyCnt1 = 0;
	}
	else if (*(unsigned short *)(data) == 1)
	{
		ACMAudioStreamOutStop(DummyPlayFunc2);
		dummyCnt2 = 0;
	}
}



//ICAT EXPORTED FUNCTION - Audio,ACM,ACM_StreamDummyPlay
void ACMStreamPlayDummyTest(void * data)
{
	if ((*(unsigned short *)data) == 0)
	{
		ACMAudioStreamOutStart(ACM_DUMMY, 0, 0, DummyPlayFunc1);
	}
	else if ((*(unsigned short *)data) == 1)
	{
		ACMAudioStreamOutStart(ACM_DUMMY, 0, 0, DummyPlayFunc2);
	}
}




static void TonesPlayFunc1(unsigned int *buff)
{
	static unsigned short tonePlayCnt = 0;
	unsigned int *ptr;

	ptr = (unsigned int *)buff;

	if (buff == NULL)
	{
		DIAG_FILTER(AUDIO, ACM, TonesPlayFunc1Null, DIAG_ERROR)
		diagTextPrintf("TonesPlayFunc1 called with NULL");
		return;
	}
	else
	{

		if (toneCounter1 % 75 == 0)
		{
			*ptr = 2;
			*(ptr + 1) = tonesMelody[tonePlayCnt][0];
			*(ptr + 2) = tonesMelody[tonePlayCnt][1];
			tonePlayCnt++;
			if (tonesMelody[tonePlayCnt][1] == 0)
				tonePlayCnt = 0;

			toneCounter1 = 0;
		}
		toneCounter1++;
	}
}


static void TonesPlayFunc2(unsigned int *buff)
{
	static unsigned short tonePlayCnt = 0;
	unsigned int *ptr;

	ptr = (unsigned int *)buff;

	if (buff == NULL)
	{
		DIAG_FILTER(AUDIO, ACM, TonesPlayFunc2Null, DIAG_ERROR)
		diagTextPrintf("TonesPlayFunc2 called with NULL");
		return;

	}
	else
	{
		if (toneCounter2 % 150 == 0)
		{
			*ptr = 2;
			*(ptr + 1) = tonesMelody[tonePlayCnt][0];
			*(ptr + 2) = tonesMelody[tonePlayCnt][1];
			tonePlayCnt++;
			if (tonesMelody[tonePlayCnt][1] == 0)
				tonePlayCnt = 0;

			toneCounter2 = 0;
		}
		toneCounter2++;
	}
}


//ICAT EXPORTED FUNCTION - Audio,ACM,ACMStreamPlayTonesStopTest
void ACMStreamPlayTonesStopTest(void *data)
{
	if ((*(unsigned short *)data) == 0)
	{
		ACMAudioStreamOutStop(TonesPlayFunc1);
		toneCounter1 = 0;
	}
	else if ((*(unsigned short *)data) == 1)
	{
		ACMAudioStreamOutStop(TonesPlayFunc2);
		toneCounter2 = 0;
	}
}



//ICAT EXPORTED FUNCTION - Audio,ACM,ACMStreamPlayTonesTest
void ACMStreamPlayTonesTest(void *data)
{
	if ((*(unsigned short *)data) == 0)
	{
		ACMAudioStreamOutStart(ACM_TONE, 0, 0, TonesPlayFunc1);
	}
	else if ((*(unsigned short *)data) == 1)
	{
		ACMAudioStreamOutStart(ACM_TONE, 0, 0, TonesPlayFunc2);
	}
}


static void audioToneFunc(unsigned int *buff)
{
	if (buff == NULL)
		return;

	buff[0] = 1;
	buff[1] = 740;  /* freq0 */
	buff[2] = 0;    /* amp0  */
	buff[3] = 0;    /* freq1 */
	buff[4] = 0;    /* amp1  */
}

//ICAT EXPORTED FUNCTION - Audio,ACM,ACMPlayDtmfTestStart
void ACMPlayDtmfTestStart(void *data)
{
	DIAG_FILTER(AUDIO, ACM, ACMPlayDtmfTestStart, DIAG_ERROR)
	diagPrintf("DTMF Streaming Test START");

	ACMAudioDeviceEnable(ACM_OUTPUT_TEST_DEVICE,
			     ACM_MSA_PCM,
			     70);
	ACMAudioStreamOutStart(ACM_TONE, 0, 0, audioToneFunc);
}


//ICAT EXPORTED FUNCTION - Audio,ACM,ACMPlayDtmfTestStop
void ACMPlayDtmfTestStop(void *data)
{
	DIAG_FILTER(AUDIO, ACM, ACMPlayDtmfTestStop, DIAG_ERROR)
	diagPrintf("DTMF Streaming Test STOP");

	ACMAudioDeviceDisable(ACM_OUTPUT_TEST_DEVICE,
			      ACM_MSA_PCM);
	ACMAudioStreamOutStop(audioToneFunc);
}


static void VocoderUpLinkCBFunc(unsigned int *buff)
{
	memcpy((unsigned char *)_txFrameBuf, (unsigned char *)buff, 19 * sizeof(unsigned short));
}

static void VocoderDownLinkCBFunc(unsigned int *buff)
{
	memcpy((unsigned char *)buff, (unsigned char *)_txFrameBuf, 19 * sizeof(unsigned short));
}


//ICAT EXPORTED FUNCTION - Audio,ACM,ACMVocoderStreamingLoopStart
void ACMVocoderStreamingLoopStart(void *p)
{
	unsigned short streamType = *((unsigned short *)p);

	ACMAudioDeviceEnable(ACM_INPUT_TEST_DEVICE, ACM_MSA_PCM, 75);
	ACMAudioDeviceEnable(ACM_OUTPUT_TEST_DEVICE, ACM_MSA_PCM, 75);
	/*
	   typedef enum
	   {
	    ACM_VOICE_CALL,
		ACM_TONE,
	    ACM_PCM,
		ACM_HR,
	    ACM_EFR,
		ACM_FR,
	    ACM_AMR_MR475,
	    ACM_AMR_MR515,
	    ACM_AMR_MR59,
	    ACM_AMR_MR67,
	    ACM_AMR_MR74,
	    ACM_AMR_MR795,
	    ACM_AMR_MR102,
	    ACM_AMR_MR122,
		ACM_DUMMY,

	    ACM_NO_STREAM_TYPE,
	    ACM_NUM_OF_STREAM_TYPES = ACM_NO_STREAM_TYPE
	   } ACM_StreamType;*/

	if ((streamType >= ACM_HR) && (streamType <= ACM_AMR_MR122))
	{
		ACMAudioStreamInStart(streamType, ACM_NEAR_END, VocoderUpLinkCBFunc);
		ACMAudioStreamOutStart(streamType, ACM_NEAR_END, ACM_NOT_COMB_WITH_CALL, VocoderDownLinkCBFunc);
	}
	else
	{
		ACMAudioStreamInStart(ACM_AMR_MR122, ACM_NEAR_END, VocoderUpLinkCBFunc);
		ACMAudioStreamOutStart(ACM_AMR_MR122, ACM_NEAR_END, ACM_NOT_COMB_WITH_CALL, VocoderDownLinkCBFunc);
	}
}

//ICAT EXPORTED FUNCTION - Audio,ACM,ACMVocoderStreamingLoopStop
void ACMVocoderStreamingLoopStop(void)
{
	ACMAudioDeviceDisable(ACM_INPUT_TEST_DEVICE, ACM_MSA_PCM);
	ACMAudioDeviceDisable(ACM_OUTPUT_TEST_DEVICE, ACM_MSA_PCM);

	ACMAudioStreamInStop(VocoderUpLinkCBFunc);
	ACMAudioStreamOutStop(VocoderDownLinkCBFunc);
}




//ICAT EXPORTED FUNCTION - Audio,ACM,ACMI2STest
void ACMI2STest(void *data)
{
	unsigned short *ptr = (unsigned short *)data;
	unsigned long *stopPtr;

	ACMAudioDeviceEnable(ACM_OUTPUT_TEST_DEVICE, ACM_I2S, 75);

	stopPtr = ACMAudioI2SDataSendStop();

	switch ( ptr[0] )       //test case
	{
	case 0:                 //on-shot of stored file (8kHz)
#if !defined (INTEL_2CHIP_PLAT) && !defined (OSA_WINCE)
		//I2SOneShot(); //note: this test uses I2S internal test (apps should use ACM I2S API only!!!)
#endif          //2-chip
		break;
	case 1:          //loop of stored file (8kHz)
#if !defined (INTEL_2CHIP_PLAT) && !defined (OSA_WINCE)
		//I2SLoop();  //note: this test uses I2S internal test  (apps should use ACM I2S API only!!!)
#endif          //2-chip
		break;
	case 2:          //play one-shot of FDI file (file size flash & I2S limitation ~100kB)
		playI2SFromFDIOneShot(I2S_TEST_FILENAME, ptr[1]);
		break;
	case 3:          //play loop of FDI file (file size flash & I2S limitation ~100kB)
		playI2SFromFDILoop(I2S_TEST_FILENAME, ptr[1]);
		break;
	}
}

//ICAT EXPORTED FUNCTION - Audio,ACM,ACMI2STestStop
void ACMI2STestStop(void)
{
	unsigned long *stopPtr;

	stopPtr = ACMAudioI2SDataSendStop();

	DIAG_FILTER(Audio, ACM, ACMI2STestStop, DIAG_ALL)
	diagPrintf("stop I2S pointer: 0x%lx", (unsigned long)stopPtr);
}


/************************************ I2S from FDI tests *******************************************************/

//one shot test
static unsigned long _oneShotFileSizeIn32BitsUnits;
static unsigned long *_oneShotFileBuffer = NULL;
static unsigned long _iOneShot = 0;
static unsigned long _totalSamplesSentOneShot   = 0;


//loop test
static unsigned long *_I2SBuffPtr = NULL;
static unsigned long _fileSizeIn32BitsUnits;
static unsigned long _iLoop = 0;
static unsigned long _totalSamplesSentLoop = 0;


/************************************ one shot test *******************************************************/
static void blockOneShotNotifyFunc(unsigned short progress, unsigned long *ptr)
{
}

static void thresholdOneShotNotifyFunc(void)
{

	_iOneShot++;
	if ((_oneShotFileSizeIn32BitsUnits / CHUNK_SIZE_IN_SAMPLES) >= _iOneShot)
	{
		ACMAudioI2SDataSend((_oneShotFileBuffer + (_iOneShot - 1) * CHUNK_SIZE_IN_SAMPLES),
				    CHUNK_SIZE_IN_SAMPLES,
				    (CHUNK_SIZE_IN_SAMPLES >> 1),
				    thresholdOneShotNotifyFunc,
				    blockOneShotNotifyFunc, 2);
		_totalSamplesSentOneShot += CHUNK_SIZE_IN_SAMPLES;
	}
	else
	{
		_totalSamplesSentOneShot = 0;
		_iOneShot = 0;

		DIAG_FILTER(Audio, ACM, thresholdOneShotNotifyFunc, DIAG_ALL)
		diagTextPrintf("threshold function called --> no data --> ending");
	}

}



void playI2SFromFDIOneShot(const char *filename, unsigned short rate)
{
	FILE_ID fileID;
	FILE_INFO fdiFileInfo;
	ERR_CODE errFdi;
	int fdiFileCloseErr, fdiFileFindError;
	size_t fdiFileBytesRead;

	//one shot test
	_oneShotFileSizeIn32BitsUnits = 0;
	_iOneShot = 0;
	_totalSamplesSentOneShot        = 0;

	if (_oneShotFileBuffer != NULL)
	{
		alignFree(_oneShotFileBuffer);
		_oneShotFileBuffer = NULL;
	}

	if (_I2SBuffPtr != NULL)
	{
		alignFree(_I2SBuffPtr);
		_I2SBuffPtr = NULL;
	}
	fdiFileFindError = FDI_findfirst((char *)filename, &fdiFileInfo);
	if (fdiFileFindError)
		return;

	fileID = FDI_fopen((char *)filename, "rb");
	if (fileID)
	{
		_oneShotFileBuffer = (unsigned long*)alignMalloc(fdiFileInfo.size);

		//too big file
		if ( _oneShotFileBuffer == NULL)
		{
			DIAG_FILTER(AUDIO, ACM, playI2SFromFDIOneShot_Err, DIAG_ERROR)
			diagTextPrintf("File is too big");
			fdiFileCloseErr = FDI_fclose(fileID);
			return;

		}

		fdiFileBytesRead = FDI_fread(_oneShotFileBuffer, sizeof(unsigned char), fdiFileInfo.size, fileID);
		errFdi = FDI_ferror(fileID);
		if (( errFdi == ERR_EOF) || ( errFdi != ERR_NONE) )
		{
			ACM_ASSERT(0);
		}
		fdiFileCloseErr = FDI_fclose(fileID);
		if (fdiFileCloseErr)
			ACM_ASSERT(0);

		_oneShotFileSizeIn32BitsUnits = (fdiFileInfo.size >> 2);
		ACMAudioI2SRateSet((ACM_I2SRate)rate);
		thresholdOneShotNotifyFunc();

	}
}



/************************************ loop test *******************************************************/
static void blockLoopNotifyFunc(unsigned short progress, unsigned long *ptr)
{

}

static void thresholdLoopNotifyFunc(void)
{


	_iLoop++;

	if ((_fileSizeIn32BitsUnits / CHUNK_SIZE_IN_SAMPLES) >= _iLoop)
	{
		_totalSamplesSentLoop += CHUNK_SIZE_IN_SAMPLES;
	}
	else
	{
		_totalSamplesSentLoop = 0;
		_iLoop = 1;
	}

	ACMAudioI2SDataSend((_I2SBuffPtr + (_iLoop - 1) * CHUNK_SIZE_IN_SAMPLES),
			    CHUNK_SIZE_IN_SAMPLES,
			    (CHUNK_SIZE_IN_SAMPLES >> 1),
			    thresholdLoopNotifyFunc,
			    blockLoopNotifyFunc, 2);
}


void  playI2SFromFDILoop(const char *filename, unsigned short rate)
{
	FILE_ID fileID;
	FILE_INFO fdiFileInfo;
	ERR_CODE errFdi;
	int fdiFileCloseErr, fdiFileFindError;
	size_t fdiFileBytesRead;

	//loop test init
	_fileSizeIn32BitsUnits = 0;
	_iLoop = 0;
	_totalSamplesSentLoop = 0;



	if (_I2SBuffPtr != NULL)
	{
		alignFree(_I2SBuffPtr);
		_I2SBuffPtr = NULL;
	}

	if (_oneShotFileBuffer != NULL)
	{
		alignFree(_oneShotFileBuffer);
		_oneShotFileBuffer = NULL;
	}
	fdiFileFindError = FDI_findfirst((char *)filename, &fdiFileInfo);
	if (fdiFileFindError)
		return;

	fileID = FDI_fopen((char *)filename, "rb");
	if (fileID)
	{
		_I2SBuffPtr = (unsigned long*)alignMalloc(fdiFileInfo.size);
		//too big file
		if ( _I2SBuffPtr == NULL)
		{
			DIAG_FILTER(AUDIO, ACM, playI2SFromFDILoop_Err, DIAG_ERROR)
			diagTextPrintf("File is too big");
			fdiFileCloseErr = FDI_fclose(fileID);
			return;

		}

		fdiFileBytesRead = FDI_fread(_I2SBuffPtr, sizeof(unsigned char), fdiFileInfo.size, fileID);
		errFdi = FDI_ferror(fileID);
		if ((errFdi == ERR_EOF) || (errFdi != ERR_NONE))
		{
			ACM_ASSERT(0);
		}
		fdiFileCloseErr = FDI_fclose(fileID);
		if (fdiFileCloseErr)
			ACM_ASSERT(0);

		_fileSizeIn32BitsUnits = (fdiFileInfo.size >> 2);

		ACMAudioI2SRateSet((ACM_I2SRate)rate);
		thresholdLoopNotifyFunc();
	}
}



//ICAT EXPORTED FUNCTION - Audio,ACM,ACMCodecIfChangeTest
void ACMCodecIfChangeTest(void *data)
{
	unsigned short codecIf;

	codecIf = *((unsigned short *)data);

	//for bluetooth i/f testing (changing the PCM link (other SSP))
	switch (codecIf)
	{
	case 0:          //default codec
		ACMAudioDeviceDisable(ACM_BLUETOOTH_SPEAKER, ACM_MSA_PCM);
		ACMAudioDeviceDisable(ACM_BLUETOOTH_MIC, ACM_MSA_PCM);

		ACMAudioDeviceEnable(ACM_INPUT_TEST_DEVICE, ACM_MSA_PCM, 75);
		ACMAudioDeviceEnable(ACM_OUTPUT_TEST_DEVICE, ACM_MSA_PCM, 75);
		break;
	case 1:          //bt codec
		ACMAudioDeviceDisable(ACM_OUTPUT_TEST_DEVICE, ACM_MSA_PCM);
		ACMAudioDeviceDisable(ACM_INPUT_TEST_DEVICE, ACM_MSA_PCM);

		ACMAudioDeviceEnable(ACM_BLUETOOTH_SPEAKER, ACM_MSA_PCM, 75);
		ACMAudioDeviceEnable(ACM_BLUETOOTH_MIC, ACM_MSA_PCM, 75);
		break;

	}
}

/************************************ one shot test *******************************************************/
static unsigned long *_PCMBuffPtr = NULL;
static unsigned long _NumOfPcmFramesInPCMBuff;
static unsigned long _bufCnt;

static void playOneShotPCM(unsigned int *buff)
{

	if ( buff != NULL)
	{
		if (_bufCnt == _NumOfPcmFramesInPCMBuff)
		{       /* When file to play ends, play zeroes */
			memset((unsigned char *)buff, '\0', PCM_BUFF_SIZE * sizeof(unsigned short));
		}
		else
		{
			memcpy((unsigned char *)buff, (unsigned char *)((unsigned short *)_PCMBuffPtr + _bufCnt * PCM_BUFF_SIZE),
			       PCM_BUFF_SIZE * sizeof(unsigned short));
			_bufCnt++;
		}
	}
}


static void playCyclicPCM(unsigned int *buff)
{

	if ( buff != NULL)
	{
		memcpy((unsigned char *)buff, (unsigned char *)((unsigned short *)_PCMBuffPtr + _bufCnt * PCM_BUFF_SIZE),
		       PCM_BUFF_SIZE * sizeof(unsigned short));
		_bufCnt++;

		if (_bufCnt == _NumOfPcmFramesInPCMBuff)
		{       /* When file to play ends, start it again from the beginning */
			_bufCnt = 0;
		}
	}
}

static BOOL _cyclicPlayback = TRUE;

//ICAT EXPORTED FUNCTION - Audio,ACM,ACMPcmPlayStart
void ACMPcmPlayStart(void *data)
{
	FILE_ID fileID;
	FILE_INFO fdiFileInfo;
	ERR_CODE errFdi;
	int fdiFileCloseErr, fdiFileFindError;
	size_t fdiFileBytesRead;
	unsigned short *dataArray = (unsigned short *)data;
	ACM_SrcDst srcDst = ACM_NEAR_END;
	ACM_CombWithCall combWithCall = ACM_NOT_COMB_WITH_CALL;

	if ( _PCMBuffPtr != NULL)
		alignFree(_PCMBuffPtr);

	_bufCnt = 0;


	fdiFileFindError = FDI_findfirst((char *)PCM_PLAT_TEST_FILENAME, &fdiFileInfo);
	if (fdiFileFindError)
		return;

	fileID = FDI_fopen((char *)PCM_PLAT_TEST_FILENAME, "rb");
	if (fileID)
	{

		_NumOfPcmFramesInPCMBuff =  ((fdiFileInfo.size % (PCM_BUFF_SIZE * sizeof(unsigned short))) == 0) ?
					   (fdiFileInfo.size / (PCM_BUFF_SIZE * sizeof(unsigned short))) :
					   ((fdiFileInfo.size / (PCM_BUFF_SIZE * sizeof(unsigned short))) + 1);

		_PCMBuffPtr = (unsigned long*)alignMalloc(_NumOfPcmFramesInPCMBuff * PCM_BUFF_SIZE * sizeof(unsigned short));

		if (_PCMBuffPtr != NULL)
		{
			memset((unsigned char *)_PCMBuffPtr, '\0', _NumOfPcmFramesInPCMBuff * PCM_BUFF_SIZE * sizeof(unsigned short));
			fdiFileBytesRead = FDI_fread(_PCMBuffPtr, sizeof(unsigned char), fdiFileInfo.size, fileID);
			errFdi = FDI_ferror(fileID);
			if (( errFdi == ERR_EOF) || ( errFdi != ERR_NONE) )
			{
				ACM_ASSERT(0);
			}
			fdiFileCloseErr = FDI_fclose(fileID);
			if (fdiFileCloseErr)
				ACM_ASSERT(0);

			if ( (dataArray[0] & 0x10) == 0 )
				ACMAudioDeviceEnable(ACM_OUTPUT_TEST_DEVICE, ACM_MSA_PCM, 75);

			dataArray[0] &= ~0x10;                                  /* Turn off the "non enable/disable" bit */

			srcDst         = (ACM_SrcDst)dataArray[0];              /* ACM_BOTH_ENDS=0, ACM_NEAR_END=1, ACM_FAR_END=2 */
			combWithCall   = (ACM_CombWithCall)dataArray[1];        /* ACM_NOT_COMB_WITH_CALL=0, ACM_COMB_WITH_CALL = 1 */
			_cyclicPlayback = (BOOL)dataArray[2];

			if (_cyclicPlayback)
				ACMAudioStreamOutStart(ACM_PCM, srcDst, combWithCall, playCyclicPCM);
			else
				ACMAudioStreamOutStart(ACM_PCM, srcDst, combWithCall, playOneShotPCM);
		}
	}
}

//ICAT EXPORTED FUNCTION - Audio,ACM,ACMPcmPlayStop
void ACMPcmPlayStop(void *data)
{
	unsigned short idx = (*(unsigned short *)data);

	if (idx != 0x10)
		ACMAudioDeviceDisable(ACM_OUTPUT_TEST_DEVICE, ACM_MSA_PCM);

	if (_cyclicPlayback)
		ACMAudioStreamOutStop(playCyclicPCM);
	else
		ACMAudioStreamOutStop(playOneShotPCM);
}




/***************************** testing record memo (using vocoder streaming) ***************************************/
static void VocoderMemoRecordUpLinkCBFunc(unsigned int *buff)
{
	static unsigned long i = 0;

	memcpy((unsigned char *)&_memoFrameBuf[i++ % (3 * 50)][0], (unsigned char *)buff, 19 * sizeof(unsigned short));
}

//ICAT EXPORTED FUNCTION - Audio,ACM,ACMVocoderStreamingMemoRecordStart
void ACMVocoderStreamingMemoRecordStart(void *p)
{
	unsigned short streamType = *((unsigned short *)p);

	ACMAudioDeviceEnable(ACM_INPUT_TEST_DEVICE, ACM_MSA_PCM, 75);
	ACMAudioDeviceEnable(ACM_OUTPUT_TEST_DEVICE, ACM_MSA_PCM, 75);
	/*
	   typedef enum
	   {
	    ACM_VOICE_CALL,
		ACM_TONE,
	    ACM_PCM,
		ACM_HR,
	    ACM_EFR,
		ACM_FR,
	    ACM_AMR_MR475,
	    ACM_AMR_MR515,
	    ACM_AMR_MR59,
	    ACM_AMR_MR67,
	    ACM_AMR_MR74,
	    ACM_AMR_MR795,
	    ACM_AMR_MR102,
	    ACM_AMR_MR122,
		ACM_DUMMY,

	    ACM_NO_STREAM_TYPE,
	    ACM_NUM_OF_STREAM_TYPES = ACM_NO_STREAM_TYPE
	   } ACM_StreamType;*/

	if ((streamType >= ACM_HR) && (streamType <= ACM_AMR_MR122))
	{
		ACMAudioStreamInStart(streamType, ACM_NEAR_END, VocoderMemoRecordUpLinkCBFunc);
	}
	else
	{
		ACMAudioStreamInStart(ACM_AMR_MR122, ACM_NEAR_END, VocoderMemoRecordUpLinkCBFunc);
	}
}

//ICAT EXPORTED FUNCTION - Audio,ACM,ACMVocoderStreamingMemoRecordStop
void ACMVocoderStreamingMemoRecordStop(void)
{
	ACMAudioDeviceDisable(ACM_INPUT_TEST_DEVICE, ACM_MSA_PCM);
	ACMAudioStreamInStop(VocoderMemoRecordUpLinkCBFunc);
}


/***************************** testing play memo (using vocoder streaming) ***************************************/
static void VocoderMemoPlayDownLinkCBFunc(unsigned int *buff)
{
	static unsigned long i = 0;

	memcpy((unsigned char *)buff, (unsigned char *)&_memoFrameBuf[i++ % (3 * 50)][0], 19 * sizeof(unsigned short));
}


//ICAT EXPORTED FUNCTION - Audio,ACM,ACMVocoderStreamingMemoPlayStart
void ACMVocoderStreamingMemoPlayStart(void *p)
{
	unsigned short streamType = *((unsigned short *)p);

	ACMAudioDeviceEnable(ACM_OUTPUT_TEST_DEVICE, ACM_MSA_PCM, 75);
	/*
	   typedef enum
	   {
	    ACM_VOICE_CALL,
		ACM_TONE,
	    ACM_PCM,
		ACM_HR,
	    ACM_EFR,
		ACM_FR,
	    ACM_AMR_MR475,
	    ACM_AMR_MR515,
	    ACM_AMR_MR59,
	    ACM_AMR_MR67,
	    ACM_AMR_MR74,
	    ACM_AMR_MR795,
	    ACM_AMR_MR102,
	    ACM_AMR_MR122,
		ACM_DUMMY,

	    ACM_NO_STREAM_TYPE,
	    ACM_NUM_OF_STREAM_TYPES = ACM_NO_STREAM_TYPE
	   } ACM_StreamType;*/

	if ((streamType >= ACM_HR) && (streamType <= ACM_AMR_MR122))
	{
		ACMAudioStreamOutStart(streamType, ACM_NEAR_END, ACM_NOT_COMB_WITH_CALL, VocoderMemoPlayDownLinkCBFunc);
	}
	else
	{
		ACMAudioStreamOutStart(ACM_AMR_MR122, ACM_NEAR_END, ACM_NOT_COMB_WITH_CALL, VocoderMemoPlayDownLinkCBFunc);
	}
}

//ICAT EXPORTED FUNCTION - Audio,ACM,ACMVocoderStreamingMemoPlayStop
void ACMVocoderStreamingMemoPlayStop(void)
{
	ACMAudioDeviceDisable(ACM_OUTPUT_TEST_DEVICE, ACM_MSA_PCM);
	ACMAudioStreamOutStop(VocoderMemoPlayDownLinkCBFunc);
}


static void pcmRecord_1(unsigned int *buff)
{
	static int i = 0;

	if ( (buff != NULL) && (_PcmBuf_1 != NULL) )
	{
		memcpy(&_PcmBuf_1[i * PCM_BUFF_SIZE], buff, PCM_BUFF_SIZE * sizeof(unsigned short));
		i++;
		if (i == 100)
		{
			i = 0;

			DIAG_FILTER(AUDIO, ACM, pcmRecord_1, DIAG_ERROR)
			diagPrintf("pcmRecord_1; NE");
		}
	}
}

static void pcmRecord_2(unsigned int *buff)
{
	static int i = 0;

	if ( (buff != NULL) && (_PcmBuf_2 != NULL) )
	{
		memcpy(&_PcmBuf_2[i * PCM_BUFF_SIZE], buff, PCM_BUFF_SIZE * sizeof(unsigned short));
		i++;
		if (i == 100)
		{
			i = 0;

			DIAG_FILTER(AUDIO, ACM, pcmRecord_2, DIAG_ERROR)
			diagPrintf("pcmRecord_2; FE");
		}
	}
}

static void pcmRecord_3(unsigned int *buff)
{
	static int i = 0;

	if ( (buff != NULL) && (_PcmBuf_3 != NULL) )
	{
		memcpy(&_PcmBuf_3[i * PCM_BUFF_SIZE], buff, PCM_BUFF_SIZE * sizeof(unsigned short));
		i++;
		if (i == 100)
		{
			i = 0;

			DIAG_FILTER(AUDIO, ACM, pcmRecord_3, DIAG_ERROR)
			diagPrintf("pcmRecord_2; BOTH");
		}
	}
}

static void pcmRecord_4(unsigned int *buff)
{
	static int i = 0;

	if ( (buff != NULL) && (_PcmBuf_4 != NULL) )
	{
		memcpy(&_PcmBuf_4[i * PCM_BUFF_SIZE], buff, PCM_BUFF_SIZE * sizeof(unsigned short));
		i++;
		if (i == 100)
		{
			i = 0;

			DIAG_FILTER(AUDIO, ACM, pcmRecord_4, DIAG_ERROR)
			diagPrintf("pcmRecord_4; NE");
		}
	}
}

//ICAT EXPORTED FUNCTION - Audio, ACM, ACMPcmRecordManyStreamsStart
void ACMPcmRecordManyStreamsStart(void *data)
{
	unsigned short idx = (*(unsigned short *)data);

	if ( (idx & 0x10) == 0)
		ACMAudioDeviceEnable(ACM_INPUT_TEST_DEVICE, ACM_MSA_PCM, 75);

	idx &= ~0x10; /* Turn off the "non enable/disable" bit */

	switch (idx)
	{
	case 0:
		//allocated once in a sw life time w/o free
		if (_PcmBuf_1 == NULL)
		{
			_PcmBuf_1 = (unsigned short *)alignMalloc(PCM_BUFF_SIZE * 101 * sizeof(unsigned short));
			memset((unsigned char *)_PcmBuf_1, '\0', PCM_BUFF_SIZE * 101 * sizeof(unsigned short));
			ACM_ASSERT(_PcmBuf_1 != NULL);
		}
		ACMAudioStreamInStart(ACM_PCM, ACM_NEAR_END, pcmRecord_1);

		//allocated once in a sw life time w/o free
		if (_PcmBuf_2 == NULL)
		{
			_PcmBuf_2 = (unsigned short *)alignMalloc(PCM_BUFF_SIZE * 101 * sizeof(unsigned short));
			memset((unsigned char *)_PcmBuf_2, '\0', PCM_BUFF_SIZE * 101 * sizeof(unsigned short));
			ACM_ASSERT(_PcmBuf_2 != NULL);
		}
		ACMAudioStreamInStart(ACM_PCM, ACM_FAR_END, pcmRecord_2);

		//allocated once in a sw life time w/o free
		if (_PcmBuf_3 == NULL)
		{
			_PcmBuf_3 = (unsigned short *)alignMalloc(PCM_BUFF_SIZE * 101 * sizeof(unsigned short));
			memset((unsigned char *)_PcmBuf_3, '\0', PCM_BUFF_SIZE * 101 * sizeof(unsigned short));
			ACM_ASSERT(_PcmBuf_3 != NULL);
		}
		ACMAudioStreamInStart(ACM_PCM, ACM_BOTH_ENDS, pcmRecord_3);
		break;

	case 1:
		//allocated once in a sw life time w/o free
		if (_PcmBuf_1 == NULL)
		{
			_PcmBuf_1 = (unsigned short *)alignMalloc(PCM_BUFF_SIZE * 101 * sizeof(unsigned short));
			memset((unsigned char *)_PcmBuf_1, '\0', PCM_BUFF_SIZE * 101 * sizeof(unsigned short));
			ACM_ASSERT(_PcmBuf_1 != NULL);
		}
		ACMAudioStreamInStart(ACM_PCM, ACM_NEAR_END, pcmRecord_1);
		break;

	case 2:
		//allocated once in a sw life time w/o free
		if (_PcmBuf_2 == NULL)
		{
			_PcmBuf_2 = (unsigned short *)alignMalloc(PCM_BUFF_SIZE * 101 * sizeof(unsigned short));
			memset((unsigned char *)_PcmBuf_2, '\0', PCM_BUFF_SIZE * 101 * sizeof(unsigned short));
			ACM_ASSERT(_PcmBuf_2 != NULL);
		}
		ACMAudioStreamInStart(ACM_PCM, ACM_FAR_END, pcmRecord_2);
		break;

	case 3:
		//allocated once in a sw life time w/o free
		if (_PcmBuf_3 == NULL)
		{
			_PcmBuf_3 = (unsigned short *)alignMalloc(PCM_BUFF_SIZE * 101 * sizeof(unsigned short));
			memset((unsigned char *)_PcmBuf_3, '\0', PCM_BUFF_SIZE * 101 * sizeof(unsigned short));
			ACM_ASSERT(_PcmBuf_3 != NULL);
		}
		ACMAudioStreamInStart(ACM_PCM, ACM_BOTH_ENDS, pcmRecord_3);
		break;

	case 4:
		//allocated once in a sw life time w/o free
		if (_PcmBuf_4 == NULL)
		{
			_PcmBuf_4 = (unsigned short *)alignMalloc(PCM_BUFF_SIZE * 101 * sizeof(unsigned short));
			memset((unsigned char *)_PcmBuf_4, '\0', PCM_BUFF_SIZE * 101 * sizeof(unsigned short));
			ACM_ASSERT(_PcmBuf_4 != NULL);
		}
		ACMAudioStreamInStart(ACM_PCM, ACM_NEAR_END, pcmRecord_4);
		break;
	}
}


//ICAT EXPORTED FUNCTION - Audio, ACM, ACMPcmRecordManyStreamsStop
void ACMPcmRecordManyStreamsStop(void *data)
{
	unsigned short idx = (*(unsigned short *)data);

	if ( (idx & 0x10) == 0)
		ACMAudioDeviceDisable(ACM_INPUT_TEST_DEVICE, ACM_MSA_PCM);

	idx &= ~0x10; /* Turn off the "non enable/disable" bit */

	switch (idx)
	{
	case 0:
		if (_PcmBuf_1 != NULL)
			free((void *)_PcmBuf_1);

		ACMAudioStreamInStop(pcmRecord_1);

		if (_PcmBuf_2 != NULL)
			free((void *)_PcmBuf_2);

		ACMAudioStreamInStop(pcmRecord_2);

		if (_PcmBuf_3 != NULL)
			free((void *)_PcmBuf_3);

		ACMAudioStreamInStop(pcmRecord_3);
		break;

	case 1:
		if (_PcmBuf_1 != NULL)
			free((void *)_PcmBuf_1);

		ACMAudioStreamInStop(pcmRecord_1);
		break;

	case 2:
		if (_PcmBuf_2 != NULL)
			free((void *)_PcmBuf_2);

		ACMAudioStreamInStop(pcmRecord_2);
		break;

	case 3:
		if (_PcmBuf_3 != NULL)
			free((void *)_PcmBuf_3);

		ACMAudioStreamInStop(pcmRecord_3);
		break;

	case 4:
		if (_PcmBuf_4 != NULL)
			free((void *)_PcmBuf_4);

		ACMAudioStreamInStop(pcmRecord_4);
		break;
	}
}


static void PcmBipPlay_1(unsigned int *buff)
{
	static int count = 0;

	if (buff != NULL)
	{
		memcpy(buff, toneBuff, PCM_BUFF_SIZE * sizeof(unsigned short));
	}

	if (++count >= 100)
	{
		count = 0;

		DIAG_FILTER(AUDIO, ACM, PcmBipPlay_1, DIAG_ERROR)
		diagPrintf("PcmBipPlay_1; NE, NOT_COMB");
	}
}

static void PcmBipPlay_2(unsigned int *buff)
{
	static int count = 0;

	if (buff != NULL)
	{
		memcpy(buff, toneBuff, PCM_BUFF_SIZE * sizeof(unsigned short));
	}

	if (++count >= 100)
	{
		count = 0;

		DIAG_FILTER(AUDIO, ACM, PcmBipPlay_2, DIAG_ERROR)
		diagPrintf("PcmBipPlay_2; FE, COMB");
	}
}

static void PcmBipPlay_3(unsigned int *buff)
{
	static int count = 0;

	if (buff != NULL)
	{
		memcpy(buff, toneBuff, PCM_BUFF_SIZE * sizeof(unsigned short));
	}

	if (++count >= 100)
	{
		count = 0;

		DIAG_FILTER(AUDIO, ACM, PcmBipPlay_3, DIAG_ERROR)
		diagPrintf("PcmBipPlay_3; NE, COMB");
	}
}

static void PcmBipPlay_4(unsigned int *buff)
{
	static int count = 0;

	if (buff != NULL)
	{
		memcpy(buff, toneBuff, PCM_BUFF_SIZE * sizeof(unsigned short));
	}

	if (++count >= 100)
	{
		count = 0;

		DIAG_FILTER(AUDIO, ACM, PcmBipPlay_4, DIAG_ERROR)
		diagPrintf("PcmBipPlay_4; NE, NOT_COMB");
	}
}

//ICAT EXPORTED FUNCTION - Audio, ACM, ACMPcmPlayManyStreamsStart
void ACMPcmPlayManyStreamsStart(void *data)
{
	UINT16 idx = *((UINT16 *)data);

	ACMAudioDeviceEnable(ACM_OUTPUT_TEST_DEVICE, ACM_MSA_PCM, 75);

	switch (idx)
	{
	case 0:
		ACMAudioStreamOutStart(ACM_PCM, ACM_NEAR_END, ACM_NOT_COMB_WITH_CALL, PcmBipPlay_1);
		ACMAudioStreamOutStart(ACM_PCM, ACM_FAR_END, ACM_COMB_WITH_CALL, PcmBipPlay_2);
		ACMAudioStreamOutStart(ACM_PCM, ACM_NEAR_END, ACM_COMB_WITH_CALL, PcmBipPlay_3);
		break;

	case 1:
		ACMAudioStreamOutStart(ACM_PCM, ACM_NEAR_END, ACM_NOT_COMB_WITH_CALL, PcmBipPlay_1);
		break;

	case 2:
		ACMAudioStreamOutStart(ACM_PCM, ACM_FAR_END, ACM_COMB_WITH_CALL, PcmBipPlay_2);
		break;

	case 3:
		ACMAudioStreamOutStart(ACM_PCM, ACM_NEAR_END, ACM_COMB_WITH_CALL, PcmBipPlay_3);
		break;

	case 4:
		ACMAudioStreamOutStart(ACM_PCM, ACM_NEAR_END, ACM_NOT_COMB_WITH_CALL, PcmBipPlay_4);
		break;
	}
}


//ICAT EXPORTED FUNCTION - Audio, ACM, ACMPcmPlayManyStreamsStop
void ACMPcmPlayManyStreamsStop(void *data)
{
	UINT16 idx = *((UINT16 *)data);

	ACMAudioDeviceDisable(ACM_OUTPUT_TEST_DEVICE, ACM_MSA_PCM);

	switch (idx)
	{
	case 0:
		ACMAudioStreamOutStop(PcmBipPlay_1);
		ACMAudioStreamOutStop(PcmBipPlay_2);
		ACMAudioStreamOutStop(PcmBipPlay_3);
		break;

	case 1:
		ACMAudioStreamOutStop(PcmBipPlay_1);
		break;

	case 2:
		ACMAudioStreamOutStop(PcmBipPlay_2);
		break;

	case 3:
		ACMAudioStreamOutStop(PcmBipPlay_3);
		break;
	}
}


static void ACMTestSuiteDemoRecvFunc(void * buff, unsigned short len)
{
	void *p = malloc(len);

	if (p == NULL)
		return;

	memcpy(p, buff, len);
	ACMStructPrintf(p, len);
	free(p);
}


//ICAT EXPORTED FUNCTION - Audio,ACM,ACMTestSuiteDemo
void ACMTestSuiteDemo(void *p)
{
	unsigned short testType = *((unsigned short *)p);
	char buff[64] = { 'h', 'e', 'l', 'l', 'o', '\0' };

	switch (testType)
	{
	case  0:
		ACMPrintf("ACMPrintf test number %d", testType);
		break;
	case  1:
		ACMStructPrintf((void *)buff, 64);
		break;
	case  2:
		ACMTestSuiteRegisterRecvFromIcat((ACM_TestSuiteRecvFunc)ACMTestSuiteDemoRecvFunc);
		ACMPrintf("ACMTestSuiteRegisterRecvFromIcat registration API called");
		break;
	}
	;

}


//ICAT EXPORTED FUNCTION - Audio, ACM, ACMTickFreqSet
void ACMTickFreqSet(void *data)
{
#if defined (INTEL_2CHIP_PLAT)
	UINT16 tickFreq = *((UINT16 *)data);

	StreamingTickFreqSet(0, tickFreq);
#endif
}

