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
 * Title: Audio Test
 *
 * Filename: AudioTest.c
 *
 * Target, platform: Common Platform, SW platform
 *
 * Authors:
 *
 * Description: The APIs in this file demonstrates and tests the capabilities of the audio on the
 *              several hermon platforms.
 *
 * Last Updated:
 *
 * Notes:
 *******************************************************************************/


/*----------- External include files -----------------------------------------*/

/*----------- Extern functions  ----------------------------------------------*/

/*----------- Local include files --------------------------------------------*/

#include "audiotest.h"
#include "acmplatform.h"
#include "acm_fdi_ext.h"
#include "acm_fdi_file.h"
#include "acm_fdi_type.h"
#include "osa.h"
#include "diags.h"

#if defined (OSA_WINCE)
#include "acm_wince.h"
#endif //(OSA_WINCE)

#if defined (OSA_LINUX)
#include "acm_linux.h"
#endif //(OSA_LINUX)

/*----------- Local defines --------------------------------------------------*/

/*----------- Local macro definitions ----------------------------------------*/

#define PCM_BUFF_SIZE                                           160
#define FILE_NAME_OUT                       "AUDIO_TEST_RecordAndPlay.bin"
#define DELAY_FRAMES_IN_LOOPBACK            50
#define NUM_OF_LOOPBACK_FRAMES              (DELAY_FRAMES_IN_LOOPBACK + 2)




/*----------- Local type definitions -----------------------------------------*/

/*----------- Global extrenal variable definition ----------------------------*/

/*----------- Local constant definitions -------------------------------------*/

/*----------- Local functions ------------------------------------------------*/

static void AUDIO_TEST_ACM_TONE_PacketTransferFunc(unsigned int *buff);         // tone packet transfer func
static void AUDIO_TEST_PcmRecord_PacketTransferFunc(unsigned int *buff);        // record to buffer packet transfer func
static void AUDIO_TEST_PcmPlay_PacketTransferFunc(unsigned int *buff);          // play from buffer packet transfer func
static void PcmRecord_TransferFunc(unsigned int *buff);                         // record on FDI transfer func
static void PcmPlay_TransferFunc(unsigned int *buff);                           // play from FDI transfer func
static void AUDIO_TEST_PcmBipPlay(unsigned int *buff);                          //bip play from a buffer transfer func
static void AUDIO_TEST_DummyPlayFunc1(unsigned int *buff);                      //dummy packet transfer func #1
static void AUDIO_TEST_DummyPlayFunc2(unsigned int *buff);                      //dummy packet transfer func #2
static void AUDIO_TEST_pcmPlayForLoop(unsigned int *buff);                      //loop play (with record) call back function
static void AUDIO_TEST_pcmRecordForLoop(unsigned int *buff);                    //loop record (with play) call back function

/************************************** local variables *****************************************************/
static FILE_ID fpOut;

//static UINT16 FirstTimeCall;
static UINT32 dummyCnt1 = 0;    //used for the dummy stream function number 1
static UINT32 dummyCnt2 = 0;    //used for the dummy stream function number 2
static int frameCount;        /*used for the loop record and play - counts how many times the call back function
			       *  was called to record before start playing */
static BOOL startPlayLoop;      //boolean flag - for the loop record and playback. indicates if the playback can start.
static UINT16* _pcmBufLoopPtr;
static UINT16 deviceEnabled = 0; /* if we start record or play during a call then the device is already
				  * enabled, so we indicates it with flag and if so, we dont disable the device
				  * when stopping the record/play */
static UINT16 deviceEnabled2 = 0;
static UINT16 *RecordPlayBuffer = NULL; /*buffer for the FDI record and play. the data is first saved on the buffer before
					 * it is saved on the FDI file */
static UINT32 recCounter = 0;           /*counter for the FDI record - counts how many times the call back function was called */
static UINT32 playCounter = 0;          /*counter for the FDI playback - counts how many times the call back function was called */
static UINT32 recordPlayFlag = 0;
static UINT32 sizeOfFile = 0;

static unsigned short _PcmBuf[PCM_BUFF_SIZE * 101];  //buffer for the record and play on a buffer APIs.
static AUDIO_TEST_Tone_Type Tone;         /* used for the tone type. must be global in order to use it in
					     the packet transfer function */
static UINT16 FirstTimeCall;             /* indicates that this is the first time calling the packet
								     transfer func for the current stream and that the tones data should be
								     initialized */
static ACM_AudioDevice Speaker_device;   /*holds the input speaker device - we save it in a global parameter in order to know
					  * what device should be disabled when we stop the test */
static ACM_AudioDevice Mic_Device;       /*holds the input microphone device - we save it in a global parameter in order to know
					  * what device should be disabled when we stop the test */
#ifndef INTEL_2CHIP_PLAT_BVD
static UINT32 SizeOfFrame = 0;           //for the vocoder stream
#endif


/******************************************    Local constants    ***********************************************************/

/* This constant array is used for the PCM tone stream forever means play the same line forever and restart means
 * start from the beginning - first line */

const static AUDIO_TEST_Tone_List AUDIO_TEST_Tone_Freq_Table [AUDIO_TEST_NUMBER_OF_TONES] =

{
	{
		/* AUDIO_TEST_DTMF_TONE_0 */
		{ FOREVER, 941, 1336 },
	},
	{
		/* AUDIO_TEST_DTMF_TONE_1 */
		{ FOREVER, 697, 1209 },
	},
	{
		/* AUDIO_TEST_DTMF_TONE_2 */
		{ FOREVER, 697, 1336 },

	},
	{
		/* AUDIO_TEST_DTMF_TONE_3 */
		{ FOREVER, 697, 1447 },
	},
	{
		/* AUDIO_TEST_DTMF_TONE_4 */
		{ FOREVER, 770, 1209 },

	},
	{
		/* AUDIO_TEST_DTMF_TONE_5 */
		{ FOREVER, 770, 1336 },

	},
	{
		/* AUDIO_TEST_DTMF_TONE_6 */
		{ FOREVER, 770, 1477 },
	},
	{
		/* AUDIO_TEST_DTMF_TONE_7 */
		{ FOREVER, 852, 1209 },
	},
	{
		/* AUDIO_TEST_DTMF_TONE_8 */
		{ FOREVER, 852, 1336 },
	},
	{
		/* AUDIO_TEST_DTMF_TONE_9 */
		{ FOREVER, 852, 1477 },
	},
	{
		/* AUDIO_TEST_DTMF_TONE_A */
		{ FOREVER, 697, 1633 },
	},
	{
		/* AUDIO_TEST_DTMF_TONE_B */
		{ FOREVER, 770, 1633 },
	},
	{
		/* AUDIO_TEST_DTMF_TONE_C */
		{ FOREVER, 852, 1633 },
	},
	{
		/* AUDIO_TEST_DTMF_TONE_D */
		{ FOREVER, 941, 1633 },
	},
	{
		/* AUDIO_TEST_DTMF_TONE_asterisk */
		{ FOREVER, 941, 1633 },
	},
	{
		/* AUDIO_TEST_DTMF_TONE_numSymb */
		{ FOREVER, 941, 1633 },
	},
	{
		/*AUDIO_TEST_TONE_DIAL_TONE*/
		{ FOREVER, 350, 440  },
	},
	{
		/* AUDIO_TEST_TONE_BUSY_SIGNAL */
		{ 25,	   480, 620  },
		{ 25,	   0,	0    },
		{ RESTART, 0,	0    }
	},
	{
		/* AUDIO_TEST_TONE_CONGESTION */
		{ 10,	   480, 620  },
		{ 15,	   0,	0    },
		{ RESTART, 0,	0    }
	},
	{
		/* AUDIO_TEST_TONE_RECORDER */
		{ 15,	   480, 620  },
		{ 10,	   0,	0    },
		{ RESTART, 0,	0    }
	},
	{
		/* AUDIO_TEST_TONE_NORMAL_RINGBACK */
		{ 100,	   440, 480  },
		{ 200,	   0,	0    },
		{ RESTART, 0,	0    }
	},
	{
		/* AUDIO_TEST_TONE_PBX_RINGBACK */
		{ 50,	   440, 480  },
		{ 150,	   0,	0    },
		{ RESTART, 0,	0    }
	}
};

/* This constant array is for the bip play */
const static __align(32) unsigned short toneBuff[PCM_BUFF_SIZE] = {
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



/*----------- Extern function definition -------------------------------------*/

/*----------- Local variable definitions -------------------------------------*/



/*****************************************************************************
 *							Tone APIs
 ******************************************************************************/

/*******************************************************************************
* Function: AUDIO_TEST_ACM_StartTone
*******************************************************************************
* Description: Start playing a tone with the required volume and device.
*
* Parameters: speaker device, volume, Tone
* Return value: void
*
* Notes:
*******************************************************************************/

//ICAT EXPORTED FUNCTION - Audio, AUDIO_TEST, AUDIO_TEST_StartTone
void AUDIO_TEST_StartTone(void *buff)

{
	ACM_ReturnCode retCode;
	UINT32 *input = (void *)buff;

	ACM_AudioVolume volume = input[1];

	Tone = input[2];
	Speaker_device = input[0];

	/* the flag is initialized to 1 and after the first time the packet transfer function is called,
	   the parameters are initialized and this flag is initialized again to zero */

	FirstTimeCall = 1;

	retCode = ACMAudioDeviceEnable(Speaker_device, ACM_MSA_PCM, volume);   //enabling the device
	if (retCode == ACM_RC_DEVICE_ALREADY_ENABLED)
		deviceEnabled = 1;  /* we indicate that the device is already enabled so we
				     * will not disable it when stopping the record */

	if (retCode == ACM_RC_OK || retCode == ACM_RC_DEVICE_ALREADY_ENABLED)
		retCode = ACMAudioStreamOutStart(ACM_TONE, ACM_NEAR_END,                         //starting the tone stream
						 ACM_NOT_COMB_WITH_CALL,
						 AUDIO_TEST_ACM_TONE_PacketTransferFunc);


	if (retCode == ACM_RC_OK)
	{
		DIAG_FILTER(Audio, AUDIO_TEST, Audio_Test_StartTone_Success, DIAG_INFORMATION);
		diagStructPrintf("Start - Tone success, return code: ", (void *)(&retCode), sizeof(retCode));
	}
	else
	{
		ACMAudioDeviceDisable(Speaker_device, ACM_MSA_PCM);
		DIAG_FILTER(Audio, AUDIO_TEST, Audio_Test_StartTone_error, DIAG_INFORMATION);
		diagStructPrintf("Start - Tone failed, return code: ", (void *)(&retCode), sizeof(retCode));
	}


} /* End of AUDIO_TEST_StartTone */


/*******************************************************************************
* Function: AUDIO_TEST_EndTone
*******************************************************************************
* Description: ending a tone
* Parameters: none
* Return value: void
*
* Notes: The speaker device is a global parameter and therefore, once someone started
*        a call with any device, this device will be the default. therefore, there is
*        no need to send any device as input when ending tone.
*******************************************************************************/

//ICAT EXPORTED FUNCTION - Audio, AUDIO_TEST, AUDIO_TEST_EndTone

void AUDIO_TEST_EndTone(void)

{
	UINT16 retCode;

	retCode = ACMAudioStreamOutStop(AUDIO_TEST_ACM_TONE_PacketTransferFunc);                /* stopping the stream */
	if (retCode == ACM_RC_OK)
		retCode = ACMAudioDeviceDisable(Speaker_device, ACM_MSA_PCM);                   /* disabling the device */

	if (retCode == ACM_RC_OK)
	{
		DIAG_FILTER(Audio, AUDIO_TEST, Audio_Test_EndTone_Success, DIAG_INFORMATION);
		diagStructPrintf("End - Tone success, return code: ", (void *)(&retCode), sizeof(retCode));
	}
	else
	{
		DIAG_FILTER(Audio, AUDIO_TEST, Audio_Test_EndTone_error, DIAG_INFORMATION);
		diagStructPrintf("End - Tone failed, return code: ", (void *)(&retCode), sizeof(retCode));
	}

} /* End of AUDIO_TEST_End_Tone */

/*******************************************************************************
* Function:	AUDIO_TEST_ACM_TONE_PacketTransferFunc
*******************************************************************************
* Description: A call back function for the tone stream
*
* Parameters: the call back function fills the buffer every time it is called with the next
*             tone to be played in the next 20 ms.
* Notes: used for notifying client of data ready in buffer in case of playing tone.
* Called every 20ms, until AUDIO_TEST_End_Tone.
*******************************************************************************/
static void AUDIO_TEST_ACM_TONE_PacketTransferFunc(unsigned int *buff)
{



	static UINT16 CountTime; /* used to count the number of times the function was called
									 for the current tone, in order to know
									 what to send in the buffer */
	static AUDIO_TEST_Tone_Freq *TonePtr; /* pointer to the current tone whitch should be sent */

	if (buff == NULL)
		return;

	if (FirstTimeCall == 1)
	{
		CountTime = 0;                                          /* In case this is the first time
												the function was called for the current stream,
											   the CountTime should be initialized to zero
											   and the tonePtr should be initialized to the
											   correct tone*/
		FirstTimeCall = 0;
		TonePtr = (AUDIO_TEST_Tone_Freq *)(AUDIO_TEST_Tone_Freq_Table [Tone]);
	}


	switch (TonePtr->time)
	{
	case RESTART:  // Starting from the beginning an infinite loop (until stop coomand)
	{
		TonePtr = (AUDIO_TEST_Tone_Freq *)(AUDIO_TEST_Tone_Freq_Table [Tone]);
		CountTime = 0;
		buff [0] = 2;
		buff [1] = TonePtr->freq1;
		buff [2] = 0;
		buff [3] = TonePtr->freq2;
		buff [4] = 0;
		CountTime++;
		break;
	}
	case FOREVER: // Staying on the same tone forever (until stop coomand)
	{
		buff [0] = 2;
		buff [1] = TonePtr->freq1;
		buff [2] = 0;
		buff [3] = TonePtr->freq2;
		buff [4] = 0;
		break;
	}
	case STOP: // Stop playing even if the stop command has not arrived
	{
		AUDIO_TEST_EndTone();
	}

	default:
	{
		if (TonePtr->time == CountTime)           // Reached the end of a tone but not the end of the loop
		{
			buff [0] = 2;
			buff [1] = TonePtr->freq1;
			buff [2] = 0;
			buff [3] = TonePtr->freq2;
			buff [4] = 0;
			TonePtr++;
			CountTime = 0;

		}
		else         // In the middle of the current tone
		{
			buff [0] = 2;
			buff [1] = TonePtr->freq1;
			buff [2] = 0;
			buff [3] = TonePtr->freq2;
			buff [4] = 0;
			CountTime++;
		}
		break;
	}
	}

}    /* End of AUDIO_TEST_ACM_TONE_PacketTransferFunc */


/*********************************************************************************
 *                           dummy stream type
 * ********************************************************************************/

/*******************************************************************************
* Function:	AUDIO_TEST_DummyPlayFunc1
*******************************************************************************
* Description: dummy call back function #1. print to the ICAT the number of times it was called
*
* Parameters: buff - the function doesnt use it.
*
*  Notes:
*******************************************************************************/
static void AUDIO_TEST_DummyPlayFunc1(unsigned int *buff)
{
	/* number of times the call back function was called */
	dummyCnt1++;

	DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_DummyPlayFunc1, DIAG_INFORMATION)
	diagPrintf("Dummy Stream #1 cb called %d times", dummyCnt1);

} //End of AUDIO_TEST_DummyPlayFunc1


/*******************************************************************************
* Function:	AUDIO_TEST_DummyPlayFunc2
*******************************************************************************
* Description: dummy call back function # 2. print to the ICAT viewer the number of times it was called
*
* Parameters: buff - the function doesnt use it.
*
*  Notes:
*******************************************************************************/

static void AUDIO_TEST_DummyPlayFunc2(unsigned int *buff)
{
	/* number of times the function was called */
	dummyCnt2++;

	DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_DummyPlayFunc2, DIAG_INFORMATION)
	diagPrintf("Dummy Stream #2 cb called %d times", dummyCnt1);

} //End of AUDIO_TEST_DummyPlayFunc2


/*******************************************************************************
 * Function:	AUDIO_TEST_ACMStreamPlayDummyStopTest
 *******************************************************************************
 * Description: stop the dummy stream out.
 *
 * Parameters: the number of the dummy call back function to stop.
 * Notes:
 ********************************************************************************/

//ICAT EXPORTED FUNCTION - Audio, AUDIO_TEST, AUDIO_TEST_ACMStreamPlayDummyStopTest
void AUDIO_TEST_ACMStreamPlayDummyStopTest(void *buff)
{
	UINT32 retCode = 0;

	/* first dummy call back function */
	if ((*(UINT16 *)buff) == 0)
	{
		retCode = ACMAudioStreamOutStop(AUDIO_TEST_DummyPlayFunc1);
		dummyCnt1 = 0;
	}
	/* second dummy call back function */
	else if (*(UINT16*)(buff) == 1)
	{
		retCode = ACMAudioStreamOutStop(AUDIO_TEST_DummyPlayFunc2);
		dummyCnt2 = 0;
	}
	if (retCode == ACM_RC_OK)
	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_ACMStreamPlayDummyStopTest_Success, DIAG_INFORMATION);
		diagPrintf("stop Dummy Stream #%d ", *(UINT16*)buff);
	}
	else
	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_ACMStreamPlayDummyStopTest_ERROR, DIAG_INFORMATION);
		diagPrintf("starting Dummy Stream #%d - failed, return code: %d", *(UINT16*)buff, retCode);
	}


} //End of AUDIO_TEST_ACMStreamPlayDummyStopTest


/*******************************************************************************
 * Function:	AUDIO_TEST_ACMStreamPlayDummyTest
 *******************************************************************************
 * Description: start dummy stream out
 *
 * Parameters: the number of dummy call back function to start
 *
 * Notes:
 *********************************************************************************/

//ICAT EXPORTED FUNCTION - Audio, AUDIO_TEST, AUDIO_TEST_ACMStreamPlayDummyTest
void AUDIO_TEST_ACMStreamPlayDummyTest(void *buff)
{
	UINT32 retCode = 0;

	if ((*(UINT16*)buff) == 0)
	{
		retCode = ACMAudioStreamOutStart(ACM_DUMMY, 0, 0, AUDIO_TEST_DummyPlayFunc1);  //starts the dummy stream #1
	}
	else if ((*(UINT16*)buff) == 1)
	{
		retCode = ACMAudioStreamOutStart(ACM_DUMMY, 0, 0, AUDIO_TEST_DummyPlayFunc2);  //starts the dummy stream #2
	}
	if (retCode == ACM_RC_OK)
	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_ACMStreamPlayDummyTest_Success, DIAG_INFORMATION);
		diagPrintf("starting Dummy Stream #%d ", *(UINT16*)buff);
	}
	else
	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_ACMStreamPlayDummyTest_ERROR, DIAG_INFORMATION);
		diagPrintf("starting Dummy Stream #%d - failed, return code: %d", *(UINT16*)buff, retCode);
	}


} //End of AUDIO_TEST_ACMStreamPlayDummyTest


/*****************************************************************************
*                   record and play in one loop
* ***************************************************************************/


/*******************************************************************************
 * Function:	AUDIO_TEST_pcmPlayForLoop
 *******************************************************************************
 * Description: call back function of the loop play. (stream out)
 * after 50 frames the call back function starts copy data from the recorded buffer to the
 * buffer returned from the function. the function starts from the beginning of the buffer every time it reaches the
 * end of it.
 *
 * Parameters: the buffer which is filled with the PCM recorded data. the buffer is filled with zeroes
 * before the record function records 50 frames.
 *
 * Notes: work simultaneously with the record loop function
 *********************************************************************************/

static void AUDIO_TEST_pcmPlayForLoop(unsigned int *buff)
{
	int i;
	static int framePlay;

	if (frameCount >= DELAY_FRAMES_IN_LOOPBACK)
	{
		/* Delay of 50 frames between record and playback */
		framePlay = 0;
		startPlayLoop = TRUE; /* the delay time has expired and now can start playback */
	}

	if (startPlayLoop)
	{
		/* if we passed the delay time  we start copy data to the buffer */
		for (i = 0; i < PCM_BUFF_SIZE; i++)
			((UINT16 *)buff)[i] = _pcmBufLoopPtr[framePlay * PCM_BUFF_SIZE + i];

		if (++framePlay >= NUM_OF_LOOPBACK_FRAMES)
			framePlay = 0;  /* start from the beginning */
	}
	else                            /* write zeroes on the buffer in case we are not ready for the playback yet */
	{
		for (i = 0; i < PCM_BUFF_SIZE; i++)
			((UINT16 *)buff)[i] = 0;
	}

} //End of AUDIO_TEST_pcmPlayForLoop


/*******************************************************************************
 * Function:	AUDIO_TEST_pcmRecordForLoop
 *******************************************************************************
 * Description: call back function of the loop record. (stream in)
 * record every time from the beginning of the buffer when it reaches the end of the buffer.
 *
 *
 * Parameters: buff  - filled with the recorded data and this data is copied by the function to the buffer.
 *
 * Notes: work simultaneously with the play loop function
 *********************************************************************************/

static void AUDIO_TEST_pcmRecordForLoop(unsigned int *buff)
{
	int i;

	if (buff != NULL)
	{
		for (i = 0; i < PCM_BUFF_SIZE; i++)
			_pcmBufLoopPtr[frameCount * PCM_BUFF_SIZE + i] = ((UINT16 *)buff)[i];

		/* reached the end of the buffer */
		if (++frameCount >= NUM_OF_LOOPBACK_FRAMES)
			frameCount = 0;  /* start from the beginning */
	}

} //End of AUDIO_TEST_pcmRecordForLoop

/*******************************************************************************
 * Function:	AUDIO_TEST_ACMLoopBackTestStop
 *******************************************************************************
 * Description: stop the loop back test. disable the devices and stop the streams. (in and out)
 *
 * Parameters: none
 *
 * Notes:
 * *********************************************************************************/

//ICAT EXPORTED FUNCTION -  Audio, AUDIO_TEST, AUDIO_TEST_ACMLoopBackTestStop
void AUDIO_TEST_ACMLoopBackTestStop(void *buff)
{
	UINT32 retCode;

	free((void *)_pcmBufLoopPtr);     //free the recorded buffer

	if (deviceEnabled == 0)
		retCode = ACMAudioDeviceDisable(Mic_Device, ACM_MSA_PCM);   //disable the mic device  (if needed)
	deviceEnabled = 0;


	if (retCode == ACM_RC_OK)
		ACMAudioStreamInStop(AUDIO_TEST_pcmRecordForLoop);     //stop the stream in

	if (retCode == ACM_RC_OK)
	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_ACMLoopBackTestStop_OUT_SUCCESS, DIAG_INFORMATION);
		diagStructPrintf("AUDIO_TEST_ACMLoopBackTest stream out stop SUCCESS - return code: ", (void *)(&retCode), sizeof(retCode));
	}
	else
	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_ACMLoopBackTestStop_OUT_ERROR, DIAG_INFORMATION);
		diagStructPrintf("AUDIO_TEST_ACMLoopBackTest stream out stop failed - return code: ", (void *)(&retCode), sizeof(retCode));
	}


	if (deviceEnabled2 == 0)
		retCode = ACMAudioDeviceDisable(Speaker_device, ACM_MSA_PCM);   //disable the speaker device (if needed)
	deviceEnabled = 0;


	if (retCode == ACM_RC_OK)
		ACMAudioStreamOutStop(AUDIO_TEST_pcmPlayForLoop);          //stop the stream out

	if (retCode == ACM_RC_OK)
	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_ACMLoopBackTestStop_IN_SUCCESS, DIAG_INFORMATION);
		diagStructPrintf("AUDIO_TEST_ACMLoopBackTest stream in stop SUCCESS - return code: ", (void *)(&retCode), sizeof(retCode));
	}
	else
	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_ACMLoopBackTestStop_IN_ERROR, DIAG_INFORMATION);
		diagStructPrintf("AUDIO_TEST_ACMLoopBackTest stream in stop failed - return code: ", (void *)(&retCode), sizeof(retCode));
	}

} //End of AUDIO_TEST_ACMLoopBackTestStop

/*******************************************************************************
 * Function:	AUDIO_TEST_ACMLoopBackTestStart
 *******************************************************************************
 * Description: starts the loop back test. enables the devices and starts the streams. (in and out)
 *
 * Parameters: 3 parameters:
 *             speaker device
 *             microphone device
 *             volume
 *
 * Notes:
 * *********************************************************************************/

//ICAT EXPORTED FUNCTION -  Audio, AUDIO_TEST, AUDIO_TEST_ACMLoopBackTestStart
void AUDIO_TEST_ACMLoopBackTestStart(void *buff)
{
	UINT32 retCode;
	UINT32* input = (UINT32*)buff;

	ACM_AudioVolume volume = input[2];

	Speaker_device = input[0];
	Mic_Device = input[1];
	frameCount = 0;
	startPlayLoop = FALSE;

	/* allocating memory for the record and play buffer */
	_pcmBufLoopPtr = (unsigned short *)malloc((UINT32)(NUM_OF_LOOPBACK_FRAMES * PCM_BUFF_SIZE * sizeof(unsigned short)));
	/* enable the mic device */
	retCode = ACMAudioDeviceEnable(Mic_Device, ACM_MSA_PCM, volume);

	if (retCode == ACM_RC_DEVICE_ALREADY_ENABLED)
		deviceEnabled = 1;  /* we indicate that the device is already enabled so we
				     * will not disable it when stopping the record */

	if (retCode == ACM_RC_OK || retCode == ACM_RC_DEVICE_ALREADY_ENABLED)
		retCode = ACMAudioStreamInStart(ACM_PCM, ACM_NEAR_END, AUDIO_TEST_pcmRecordForLoop);

	if (retCode == ACM_RC_OK)
	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_ACMLoopBackTestStart_OUT_SUCCESS, DIAG_INFORMATION);
		diagStructPrintf("AUDIO_TEST_ACMLoopBackTest stream out start SUCCESS - return code: ", (void *)(&retCode), sizeof(retCode));
	}
	else
	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_ACMLoopBackTestStart_OUT_ERROR, DIAG_INFORMATION);
		diagStructPrintf("AUDIO_TEST_ACMLoopBackTest stream out start failed - return code: ", (void *)(&retCode), sizeof(retCode));
		ACMAudioDeviceDisable(Mic_Device, ACM_MSA_PCM);
		return;
	}



	retCode = ACMAudioDeviceEnable(Speaker_device, ACM_MSA_PCM, volume);
	if (retCode == ACM_RC_DEVICE_ALREADY_ENABLED)
		deviceEnabled2 = 1;  /* we indicate that the device is already enabled so we
				      * will not disable it when stopping the record */

	if (retCode == ACM_RC_OK || retCode == ACM_RC_DEVICE_ALREADY_ENABLED)
		retCode = ACMAudioStreamOutStart(ACM_PCM, ACM_NEAR_END, ACM_NOT_COMB_WITH_CALL, AUDIO_TEST_pcmPlayForLoop);
	if (retCode == ACM_RC_OK)
	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_ACMLoopBackTestStart_IN_SUCCESS, DIAG_INFORMATION);
		diagStructPrintf("AUDIO_TEST_ACMLoopBackTest stream out start SUCCESS - return code: ", (void *)(&retCode), sizeof(retCode));
	}
	else
	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_ACMLoopBackTestStart_IN_ERROR, DIAG_INFORMATION);
		diagStructPrintf("AUDIO_TEST_ACMLoopBackTest stream out start failed - return code: ", (void *)(&retCode), sizeof(retCode));
		ACMAudioDeviceDisable(Speaker_device, ACM_MSA_PCM);
		return;
	}


} //End of AUDIO_TEST_ACMLoopBackTestStart

/***********************************************************************************************
 *                                    bip play
 * ***********************************************************************************************/

/*******************************************************************************
* Function:	AUDIO_TEST_ACMPcmStartBipPlay
*******************************************************************************
* Description: start playing bip from a buffer
*
*
* Parameters: *buff contains the inputs:
*              idx - distance
*              volume
*              speaker device
* Return value: void
*
* Notes:
*******************************************************************************/

//ICAT EXPORTED FUNCTION - Audio, AUDIO_TEST, AUDIO_TEST_ACMPcmStartBipPlay
void AUDIO_TEST_ACMPcmStartBipPlay(void *buff)
{
	UINT32 retCode;
	UINT32* input = (UINT32 *)buff;
	UINT16 idx = input[2];
	ACM_AudioVolume volume = input[1];

	Speaker_device = input[0];

	/* enabling the device */
	retCode = ACMAudioDeviceEnable(Speaker_device, ACM_MSA_PCM, volume);
	if (retCode == ACM_RC_DEVICE_ALREADY_ENABLED)
		deviceEnabled = 1;  /* we indicate that the device is already enabled so we
				     * will not disable it when stopping the record */

	if (retCode == ACM_RC_OK || retCode == ACM_RC_DEVICE_ALREADY_ENABLED)
	{
		switch (idx)
		{
		case 0:             //near end + not comb with call
			retCode = ACMAudioStreamOutStart(ACM_PCM, ACM_NEAR_END, ACM_NOT_COMB_WITH_CALL, AUDIO_TEST_PcmBipPlay);
			break;
		case 1:             //far end + not comb with call
			retCode = ACMAudioStreamOutStart(ACM_PCM, ACM_FAR_END, ACM_NOT_COMB_WITH_CALL, AUDIO_TEST_PcmBipPlay);
			break;
		case 2:             //both ends + not comb with call
			retCode = ACMAudioStreamOutStart(ACM_PCM, ACM_BOTH_ENDS, ACM_NOT_COMB_WITH_CALL, AUDIO_TEST_PcmBipPlay);
			break;
		case 3:             //near end + comb with call
			retCode = ACMAudioStreamOutStart(ACM_PCM, ACM_NEAR_END, ACM_COMB_WITH_CALL, AUDIO_TEST_PcmBipPlay);
			break;
		case 4:             //far end + comb with call
			retCode = ACMAudioStreamOutStart(ACM_PCM, ACM_FAR_END, ACM_COMB_WITH_CALL, AUDIO_TEST_PcmBipPlay);
			break;
		case 5:             //both ends + comb with call
			retCode = ACMAudioStreamOutStart(ACM_PCM, ACM_BOTH_ENDS, ACM_COMB_WITH_CALL, AUDIO_TEST_PcmBipPlay);
			break;
		default:
			DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_ACMPcmStartBipPlay_ERROR1, DIAG_INFORMATION);
			diagPrintf("start bib play failed, invalid distance parameter");
			return;
			break;

		}
	}
	if (retCode == ACM_RC_OK)
	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_ACMPcmStartBipBipPlay_SUCCESS, DIAG_INFORMATION);
		diagStructPrintf("start bip play success, return code: ", (void *)(&retCode), sizeof(retCode));
	}
	else
	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_ACMPcmStartBipPlay_ERROR, DIAG_INFORMATION);
		diagStructPrintf("start bip play failed, return code: ", (void *)(&retCode), sizeof(retCode));
		ACMAudioDeviceDisable(Speaker_device, ACM_MSA_PCM);
	}


} // End of AUDIO_TEST_ACMPcmStartBipPlay

/*******************************************************************************
* Function:	AUDIO_TEST_ACMPcmStopBipPlay
*******************************************************************************
* Description: stop playing bip from a buffer
*
*
* Parameters: void
*
* Return value: void
*
* Notes:
*******************************************************************************/
//ICAT EXPORTED FUNCTION - Audio, AUDIO_TEST, AUDIO_TEST_ACMPcmStopBipPlay
void AUDIO_TEST_ACMPcmStopBipPlay(void)
{
	UINT32 retCode = ACM_RC_OK;

	if (deviceEnabled == 0)
		retCode = ACMAudioDeviceDisable(ACM_STEREO_SPEAKER, ACM_MSA_PCM);  //disable the speaker device
	deviceEnabled = 0;

	if (retCode == ACM_RC_OK)
		retCode = ACMAudioStreamOutStop(AUDIO_TEST_PcmBipPlay);

	if (retCode == ACM_RC_OK)
	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_ACMPcmStopBipPlay_SUCCESS, DIAG_INFORMATION);
		diagStructPrintf("stop bip play success, return code: ", (void *)(&retCode), sizeof(retCode));
	}
	else
	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_ACMPcmStopBipPlay_ERROR, DIAG_INFORMATION);
		diagStructPrintf("stop bip play failed, return code: ", (void *)(&retCode), sizeof(retCode));

	}

} //End of AUDIO_TEST_ACMPcmStopBipPlay

/*******************************************************************************
* Function:	AUDIO_TEST_PcmBipPlay
*******************************************************************************
* Description: call back function for the bip play. every 20 msec copy the whole bip buffer
* to the buffer recieved from the call back func
*
*
* Parameters: buff - 160 * 16 bits
*
* Return value: void
*
* Notes:
*******************************************************************************/

static void AUDIO_TEST_PcmBipPlay(unsigned int *buff)
{
	if ( buff != NULL)
	{
		memcpy(buff, toneBuff, PCM_BUFF_SIZE * sizeof(UINT16));
	}
}




/*****************************************************************************
 *					Record and Play from a buffer APIs
 ******************************************************************************/

/*******************************************************************************
* Function: AUDIO_TEST_ACMPcmStartRecordBuffer
*******************************************************************************
* Description: starting record on a buffer. the buffer is of size 100*(size of pcm buffer) which is 160 words.
*              the record is in a loop. when the counter reaches the end of the buffer it starts from the beginning.
*              There are 3 options of record:
*
* Parameters: distance(ACM_NEAR_END, ACM_FAR_END, ACM_BOTH_END), mic device.
* Return value: void
*
* Notes:
*******************************************************************************/



//ICAT EXPORTED FUNCTION - Audio,AUDIO_TEST, AUDIO_TEST_ACMPcmStartRecordBuffer
void AUDIO_TEST_ACMPcmStartRecordBuffer(void *buff)
{
	UINT16 retCode = 1;
	UINT32* input = (UINT32 *)buff;

	ACM_AudioVolume volume = input[1];
	UINT16 idx = input[2];

	Mic_Device = input[0];

	//enabling the mic device for the record
	retCode = ACMAudioDeviceEnable(Mic_Device, ACM_MSA_PCM, volume);
	/* the device can be already enabled (during a call)*/
	if (retCode == ACM_RC_DEVICE_ALREADY_ENABLED)
		deviceEnabled = 1;  /* we indicate that the device is already enabled so we
				     * will not disable it when stopping the record */
	if (retCode == ACM_RC_OK || retCode == ACM_RC_DEVICE_ALREADY_ENABLED)
	{
		switch (idx) //starting the stream in for the record with the desirable end.
		{
		case 0:
			retCode = ACMAudioStreamInStart(ACM_PCM, ACM_NEAR_END, AUDIO_TEST_PcmRecord_PacketTransferFunc);
			break;
		case 1:
			retCode = ACMAudioStreamInStart(ACM_PCM, ACM_FAR_END, AUDIO_TEST_PcmRecord_PacketTransferFunc);
			break;
		case 2:
			retCode = ACMAudioStreamInStart(ACM_PCM, ACM_BOTH_ENDS, AUDIO_TEST_PcmRecord_PacketTransferFunc);
			break;
		default:
			DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_ACMPcmStartRecordBuffer_ERROR1, DIAG_INFORMATION);
			diagPrintf("Start record - failed - invalid distance parameter");
			return;
			break;
		}
	}
	if (retCode == ACM_RC_OK)

	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_ACMPcmStartRecordBuffer_Success, DIAG_INFORMATION);
		diagStructPrintf("Start record - success, return code: ", (void *)(&retCode), sizeof(retCode));
	}
	else
	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_ACMPcmStartRecordBuffer_ERROR, DIAG_INFORMATION);
		diagStructPrintf("Start record - failed, return code: ", (void *)(&retCode), sizeof(retCode));
	}
}  // End of AUDIO_TEST_ACMPcmStartRecordBuffer



/*******************************************************************************
* Function: AUDIO_TEST_ACMPcmStopRecordBuffer
*******************************************************************************
* Description: stop the record on the buffer.
*              stop the stream in
*              disable the mic device
*
* Parameters:   none
* Return value: void
*
* Notes:
*******************************************************************************/

//ICAT EXPORTED FUNCTION - Audio,AUDIO_TEST,AUDIO_TEST_ACMPcmStopRecordBuffer
void AUDIO_TEST_ACMPcmStopRecordBuffer(void)
{
	UINT16 retCode = ACM_RC_OK;
	UINT16 retCode1 = ACM_RC_OK;
	UINT16 retCode2 = ACM_RC_OK;


	if (deviceEnabled == 0)
		retCode1 = ACMAudioDeviceDisable(Mic_Device, ACM_MSA_PCM);  //disables the microphone device (if needed)
	deviceEnabled = 0;

	retCode2 = ACMAudioStreamInStop(AUDIO_TEST_PcmRecord_PacketTransferFunc); //stops the stream in

	if (retCode1 != ACM_RC_OK)
	{
		retCode = retCode1;
	}
	if (retCode2 != ACM_RC_OK)
	{
		retCode = retCode2;
	}

	if (retCode == ACM_RC_OK)
	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_ACMPcmStopRecord_Success, DIAG_INFORMATION);
		diagStructPrintf("stop record - success, return code: ", (void *)(&retCode), sizeof(retCode));
	}
	else
	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_ACMPcmStopRecord_ERROR, DIAG_INFORMATION);
		diagStructPrintf("stop record - failed, return code: ", (void *)(&retCode), sizeof(retCode));
	}
}   // End of AUDIO_TEST_ACMPcmStopRecord


/*******************************************************************************
* Function: AUDIO_TEST_PcmRecord_PacketTransferFunc
*******************************************************************************
* Description: This call back function is called every 20 msec after starting the buffer record.
* it copy the recorded data from the buffer which is transfered from the ACM and then copies it to the PcmBuf.
* after reaching the end of the buffer it starts from the beginning.
*
* Parameters: buff - filled with 160 words of data recorded.
* Return value: void
*
* Notes:
*******************************************************************************/

static void AUDIO_TEST_PcmRecord_PacketTransferFunc(unsigned int *buff)
{
	static int i = 0;

	if (buff != NULL)
	{
		memcpy(&_PcmBuf[i * PCM_BUFF_SIZE], buff, PCM_BUFF_SIZE * sizeof(unsigned short));
		i++;

		// reaching the end of the buffer
		if (i == 100)
		{
			i = 0;
		}
	}
} // End of AUDIO_TEST_PcmRecord_PacketTransferFunc




/*******************************************************************************
* Function: AUDIO_TEST_ACMPcmStartPlayBuffer
*******************************************************************************
* Description: starting playback from the buffer which we recorded on in AUDIO_TEST_ACMPcmStartRecordBuffer
* the play is on a loop. the start includes: enabling the device, starting pcm stream out with the playback call back function.
*
* Parameters: speaker device
*             distance: (6 options)
*             ACM_NEAR_END + ACM_COMB_WITH_CALL/ACM_NOT_COMB_WITH_CALL
*             ACM_FAR_END + ACM_COMB_WITH_CALL/ACM_NOT_COMB_WITH_CALL
*             ACM_BOTH_END + ACM_COMB_WITH_CALL/ACM_NOT_COMB_WITH_CALL
*
*
* Return value: void
*
* Notes:
*******************************************************************************/

//ICAT EXPORTED FUNCTION -  Audio,AUDIO_TEST,AUDIO_TEST_ACMPcmStartPlayBuffer
void AUDIO_TEST_ACMPcmStartPlayBuffer(void *buff)
{

	UINT32 retCode = 1;
	UINT32* input = (UINT32 *)buff;

	UINT16 idx = input[2];
	ACM_AudioVolume volume = input[1];

	Speaker_device = input[0];

	retCode = ACMAudioDeviceEnable(Speaker_device, ACM_MSA_PCM, volume);
	if (retCode == ACM_RC_DEVICE_ALREADY_ENABLED)
		deviceEnabled = 1;  /* we indicate that the device is already enabled so we
				     * will not disable it when stopping the record */

	if (retCode == ACM_RC_OK || retCode == ACM_RC_DEVICE_ALREADY_ENABLED)
	{
		switch (idx)
		{
		case 0:          //near end + not combined with call
			retCode = ACMAudioStreamOutStart(ACM_PCM, ACM_NEAR_END, ACM_NOT_COMB_WITH_CALL, AUDIO_TEST_PcmPlay_PacketTransferFunc);
			break;
		case 1:          //far end + not combined with call
			retCode = ACMAudioStreamOutStart(ACM_PCM, ACM_FAR_END, ACM_NOT_COMB_WITH_CALL, AUDIO_TEST_PcmPlay_PacketTransferFunc);
			break;
		case 2:          //both ends + not combined with call
			retCode = ACMAudioStreamOutStart(ACM_PCM, ACM_BOTH_ENDS, ACM_NOT_COMB_WITH_CALL, AUDIO_TEST_PcmPlay_PacketTransferFunc);
			break;
		case 3:          //near end + combined with call
			retCode = ACMAudioStreamOutStart(ACM_PCM, ACM_NEAR_END, ACM_COMB_WITH_CALL, AUDIO_TEST_PcmPlay_PacketTransferFunc);
			break;
		case 4:          //far end + combined with call
			retCode = ACMAudioStreamOutStart(ACM_PCM, ACM_FAR_END, ACM_COMB_WITH_CALL, AUDIO_TEST_PcmPlay_PacketTransferFunc);
			break;
		case 5:          //both ends + combined with call
			retCode = ACMAudioStreamOutStart(ACM_PCM, ACM_BOTH_ENDS, ACM_COMB_WITH_CALL, AUDIO_TEST_PcmPlay_PacketTransferFunc);
			break;
		default:         //wrong input
			DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_ACMPcmStartPlayBuffer_ERROR1, DIAG_INFORMATION);
			diagPrintf("stop record to buffer - failed, invalid distance parameter");
			return;
			break;

		}
	}
	if (retCode == ACM_RC_OK)
	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_ACMPcmStartPlayBuffer_Success, DIAG_INFORMATION);
		diagStructPrintf("stop record to buffer - success, return code: ", (void *)(&retCode), sizeof(retCode));
	}
	else
	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_ACMPcmStartPlayBuffer_ERROR, DIAG_INFORMATION);
		diagStructPrintf("stop record to buffer - failed, return code: ", (void *)(&retCode), sizeof(retCode));
	}
} // End of AUDIO_TEST_ACMPcmStartPlayBuffer


/*******************************************************************************
* Function: AUDIO_TEST_ACMPcmStopPlayBuffer
*******************************************************************************
* Description: Stop the playback from the buffer.
*              stop the stream out, disable the speaker device.
*
* Parameters: none
* Return value: void
*
* Notes:
*******************************************************************************/


//ICAT EXPORTED FUNCTION - Audio,AUDIO_TEST,AUDIO_TEST_ACMPcmStopPlayBuffer
void AUDIO_TEST_ACMPcmStopPlayBuffer(void)
{
	UINT16 retCode = ACM_RC_OK;
	UINT16 retCode1 = ACM_RC_OK;
	UINT16 retCode2 = ACM_RC_OK;

	if (deviceEnabled == 0)
		retCode1 = ACMAudioDeviceDisable(Speaker_device, ACM_MSA_PCM);
	deviceEnabled = 0;
	retCode2 = ACMAudioStreamOutStop(AUDIO_TEST_PcmPlay_PacketTransferFunc);

	if (retCode1 != ACM_RC_OK)
	{
		retCode = retCode1;
	}
	if (retCode2 != ACM_RC_OK)
	{
		retCode = retCode2;
	}

	if (retCode == ACM_RC_OK)
	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_ACMPcmStopPlayBuffer_Success, DIAG_INFORMATION);
		diagStructPrintf("stop playback from buffer - Success, return code: ", (void *)(&retCode), sizeof(retCode));
	}
	else
	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_ACMPcmStopPlay_ERROR, DIAG_INFORMATION);
		diagStructPrintf("stop playback from buffer - Failed, return code: ", (void *)(&retCode), sizeof(retCode));
	}
}   // End of AUDIO_TEST_ACMPcmStopRecord




/*******************************************************************************
* Function: AUDIO_TEST_PcmRecord_PacketTransferFunc
*******************************************************************************
* Description: This call back function is called every 20 msec after starting the buffer playback.
* it copies the data from the buffer with the recorded data to the buffer which is transfered to the ACM.
* after reaching the end of the buffer it starts from the beginning.
*
* Parameters: buff - buffer which is filled with the data to be played.
* Return value: void
*
* Notes:
*******************************************************************************/

static void AUDIO_TEST_PcmPlay_PacketTransferFunc(unsigned int *buff)
{
	static int i = 0;

	if (buff != NULL)
	{
		memcpy(buff, &_PcmBuf[i * PCM_BUFF_SIZE], PCM_BUFF_SIZE * sizeof(unsigned short));
		i++;
		// reaching the end of the buffer
		if (i == 100)
		{
			i = 0;
		}
	}
} // End of AUDIO_TEST_PcmRecord_PacketTransferFunc


/************************************************************************************************************************
*                                                                                               record to FDI															*
* **********************************************************************************************************************/

/*******************************************************************************
* Function:	AUDIO_TEST_FDIRecordStart
*******************************************************************************
* Description: starting record to a FDI file. includes: initialize the task and the semaphore.
*
*
* Parameters: mic device, distance (far, near, both)
* Return value: none
*
* Notes:
*******************************************************************************/

//ICAT EXPORTED FUNCTION - Audio, AUDIO_TEST, AUDIO_TEST_FDIRecordStart
void AUDIO_TEST_FDIRecordStart(void *buff)
{
	UINT32* input = (UINT32 *)buff;
	UINT32 retCode = ACM_RC_OK;

	ACM_AudioVolume volume = input[1];
	UINT16 idx = input[2];

	Mic_Device = input[0];

	/* remove the file from flash - if existed */
	FDI_remove((char *)FILE_NAME_OUT);
	/*max size of the FDI file is 100 KB*/
	RecordPlayBuffer = (UINT16*)malloc(1024 * 100);
	if (RecordPlayBuffer == NULL)
	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_RecordStart_ERRORFDI1, DIAG_INFORMATION);
		diagPrintf("start record failed - memory allocation error");
		return;

	}
	recCounter = 0;                                                         //size of recorded data
	recordPlayFlag = 0;                                                     //indicate if we finished record
	retCode = ACMAudioDeviceEnable(Mic_Device, ACM_MSA_PCM, volume);        //starting the mic device
	if (retCode == ACM_RC_DEVICE_ALREADY_ENABLED)
		deviceEnabled = 1;  /* we indicate that the device is already enabled so we
				     * will not disable it when stopping the record */

	if (retCode == ACM_RC_OK || retCode == ACM_RC_DEVICE_ALREADY_ENABLED)
	{
		switch (idx)
		{
		case 0:
		{                   /* near end */
			retCode = ACMAudioStreamInStart(ACM_PCM, ACM_NEAR_END, PcmRecord_TransferFunc);
			break;
		}

		case 1:          /* far end */
		{
			retCode = ACMAudioStreamInStart(ACM_PCM, ACM_FAR_END, PcmRecord_TransferFunc);
			break;
		}
		case 2:          /* both ends */
		{
			retCode = ACMAudioStreamInStart(ACM_PCM, ACM_BOTH_ENDS, PcmRecord_TransferFunc);
			break;
		}
		default:
			DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_RecordStart_ERROR1, DIAG_INFORMATION);
			diagPrintf("start record - failed, invalid distance parameter");
			return;
		}
	}
	if (retCode == ACM_RC_OK)
	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_RecordStart_Success, DIAG_INFORMATION);
		diagStructPrintf("start record - success, return code: ", (void *)(&retCode), sizeof(retCode));
		return;
	}
	else
	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_RecordStart_ERROR, DIAG_INFORMATION);
		diagStructPrintf("start record - failed, return code: ", (void *)(&retCode), sizeof(retCode));
		return;
	}

} /*end of AUDIO_TEST_FDIRecordStart */


/*******************************************************************************
* Function:	AUDIO_TEST_FDIRecordStop
*******************************************************************************
* Description: stopping the record
*
*
* Parameters: Stream type
* Return value:
*
* Notes:
*******************************************************************************/

//ICAT EXPORTED FUNCTION - Audio, AUDIO_TEST, AUDIO_TEST_FDIRecordStop
void AUDIO_TEST_FDIRecordStop(void *buff)
{
	UINT32 retCode = ACM_RC_OK;
	UINT32 retCode1 = ACM_RC_OK;
	UINT32 res;

	recordPlayFlag = 1;
	retCode = ACMAudioStreamInStop(PcmRecord_TransferFunc);
	fpOut = FDI_fopen(FILE_NAME_OUT, "wb"); //creating a new file for the record

	if (!fpOut)
	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_AUDIO_TEST_RecordStart_FileOpenERROR, DIAG_INFORMATION);
		diagStructPrintf("start record - open the file - failed: ", (void *)(&retCode), sizeof(retCode));
		return;
	}

	res = FDI_fwrite(RecordPlayBuffer, sizeof(WORD), recCounter - PCM_BUFF_SIZE, fpOut);
	free((void *)RecordPlayBuffer);

	FDI_fclose(fpOut);
	if (deviceEnabled == 0)
		retCode1 =  ACMAudioDeviceDisable(Mic_Device, ACM_MSA_PCM);
	deviceEnabled = 0;

	if (retCode1 != ACM_RC_OK)
		retCode = retCode1;


	if (retCode == ACM_RC_OK)
	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_FDIRecordStop_Success, DIAG_INFORMATION);
		diagStructPrintf("stop record to FDI - success, return code: ", (void *)(&retCode), sizeof(retCode));
		return;
	}
	else
	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_FDIRecordStop_ERROR, DIAG_INFORMATION);
		diagStructPrintf("stop record to FDI - failed, return code: ", (void *)(&retCode), sizeof(retCode));
		return;
	}


} // end of AUDIO_TEST_FDIRecordStop

/*******************************************************************************
* Function:	PcmRecord_TransferFunc
*******************************************************************************
* Description: call back function for the record.
*
*
* Parameters:
* Return value:
*
* Notes: used for getting the data from the ACM and writing it on the FDI file.
* Called every 20ms, until DEMO_ACM_StreamInStop.
*******************************************************************************/
static void PcmRecord_TransferFunc (unsigned int *buff)
{

	if (buff == NULL)
		return;
	if (recordPlayFlag)
		return;



	recCounter += PCM_BUFF_SIZE;
	if (recCounter * sizeof(UINT16) < 100 * 1024)
		memcpy((UINT16*)RecordPlayBuffer + (recCounter - PCM_BUFF_SIZE), buff, PCM_BUFF_SIZE * sizeof(WORD));
	else
	{                                               /* if we reached 100 KB we stop the record */
		recordPlayFlag = 1;                     /* in order to know not to call the stop function more than once */
		DIAG_FILTER(Audio, AUDIO_TEST, PcmRecord_TransferFunc, DIAG_INFORMATION);
		diagPrintf("the record to the FDI exceeded the buffer bounds - 100 KB. stop the record ");
		return;

	}


}



/*******************************************************************************
 * Function:	AUDIO_TEST_PlayStart
 ********************************************************************************
 * Description: start playing
 *
 *
 * Parameters:
 * Return value:
 *
 * Notes:
 ********************************************************************************/

//ICAT EXPORTED FUNCTION - Audio, AUDIO_TEST, AUDIO_TEST_FDIPlayStart
void AUDIO_TEST_FDIPlayStart(void *buff)

{
	UINT32 retCode;
	UINT32 res;
	FILE_INFO fdiFileInfo;
	UINT32 fdiFileFindError;
	UINT32* input = (UINT32*)buff;
	ACM_SrcDst Dst = input[2];
	ACM_AudioVolume volume = input[1];

	Speaker_device = input[0];




	/* recieve the information about the FDI file */
	fdiFileFindError = FDI_findfirst((char *)FILE_NAME_OUT, &fdiFileInfo);
	if (fdiFileFindError)
	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_FDIPlayStart_FDIERROR1, DIAG_INFORMATION);
		diagPrintf("start play from FDI - failed to find recorded file");
		return;
	}

	fpOut = FDI_fopen((char *)FILE_NAME_OUT, "rb");
	if (!fpOut)
	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_FDIPlayStart_FDIERROR2, DIAG_INFORMATION);
		diagPrintf("start play from FDI - failed to open file");
		return;
	}
	/* allocating the size of the FDI */
	RecordPlayBuffer         = (UINT16*)malloc(fdiFileInfo.size);
	sizeOfFile  = fdiFileInfo.size;
	if (RecordPlayBuffer == NULL)
	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_FDIPlayStart_FDIERROR3, DIAG_INFORMATION);
		diagPrintf("start play from FDI - file is too big");
		return;
	}

	res = FDI_fread(RecordPlayBuffer, sizeof(char), fdiFileInfo.size, fpOut);     // read to the buffer
	if (res != fdiFileInfo.size)
	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_FDIPlayStart_FDIERROR4, DIAG_INFORMATION);
		diagPrintf("start play from FDI - error reading the file");
		return;
	}
	FDI_fclose(fpOut);

	retCode = ACMAudioDeviceEnable(Speaker_device, ACM_MSA_PCM, volume);
	if (retCode == ACM_RC_DEVICE_ALREADY_ENABLED)
		deviceEnabled = 1;  /* we indicate that the device is already enabled so we
				     * will not disable it when stopping the record */
	playCounter = 0;
	recordPlayFlag = 0;
	if (retCode == ACM_RC_OK || retCode == ACM_RC_DEVICE_ALREADY_ENABLED)
	{

		switch (Dst)
		{
		case 0:             /* near end + not combined with call */
		{
			retCode = ACMAudioStreamOutStart(ACM_PCM, ACM_NEAR_END, ACM_NOT_COMB_WITH_CALL, PcmPlay_TransferFunc);
			break;
		}
		case 1:           /* far end + not combined with call */
		{
			retCode = ACMAudioStreamOutStart(ACM_PCM, ACM_FAR_END, ACM_NOT_COMB_WITH_CALL, PcmPlay_TransferFunc);
			break;
		}
		case 2:           /* both ends + not combined with call */
		{
			retCode = ACMAudioStreamOutStart(ACM_PCM, ACM_BOTH_ENDS, ACM_NOT_COMB_WITH_CALL, PcmPlay_TransferFunc);
			break;
		}
		case 3:           /* near end + combined with call */
		{
			retCode = ACMAudioStreamOutStart(ACM_PCM, ACM_NEAR_END, ACM_COMB_WITH_CALL, PcmPlay_TransferFunc);
			break;
		}
		case 4:           /* far end + combined with call */
		{
			retCode = ACMAudioStreamOutStart(ACM_PCM, ACM_FAR_END, ACM_COMB_WITH_CALL, PcmPlay_TransferFunc);
			break;
		}
		case 5:            /* both ends + combined with call */
		{
			retCode = ACMAudioStreamOutStart(ACM_PCM, ACM_BOTH_ENDS, ACM_COMB_WITH_CALL, PcmPlay_TransferFunc);
			break;
		}
		default:          /* wrong parameter */
			DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_FDIPlayStart_ERROR1, DIAG_INFORMATION);
			diagPrintf("start play from FDI - failed, invalid distance parameter");
			return;
		}
	}

	if (retCode == ACM_RC_OK)
	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_FDIPlayStart_Success, DIAG_INFORMATION);
		diagStructPrintf("start play from FDI - success, return code: ", (void *)(&retCode), sizeof(retCode));
		return;
	}
	else
	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_FDIPlayStart_ERROR, DIAG_INFORMATION);
		diagStructPrintf("start play from FDI - failed, return code: ", (void *)(&retCode), sizeof(retCode));
		recordPlayFlag = 1;
		return;
	}

} /* end of AUDIO_TEST_FDIPlayStart */

/*******************************************************************************
 * Function:	AUDIO_TEST_PlayStop
 ********************************************************************************
 * Description: stop playing, after the play task reaches the end of the file the task stops the functions of the play.
 * however, the task will not be deleted until this function is called. there is also an option to call this function during
 * the playback and it will stop everything.
 *
 *
 * Parameters: Stream type
 * Return value:
 *
 * Notes:
 ********************************************************************************/

//ICAT EXPORTED FUNCTION - Audio, AUDIO_TEST,AUDIO_TEST_FDIPlayStop
void AUDIO_TEST_FDIPlayStop(void *buff)
{

	UINT32 retCode = ACM_RC_OK;

	recordPlayFlag = 1;
	if (deviceEnabled == 0)
		retCode =  ACMAudioDeviceDisable(Speaker_device, ACM_MSA_PCM);
	deviceEnabled = 0;

	// the flag of the end task is on therefore no more operations on the semaphore
	if (retCode == ACM_RC_OK)
		retCode = ACMAudioStreamOutStop(PcmPlay_TransferFunc);     // stop the stream out (and the packet transfer func.
	free(RecordPlayBuffer);
	if (retCode == ACM_RC_OK)
	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_FDIPlayStop_Success, DIAG_INFORMATION);
		diagStructPrintf("stop play from FDI - success, return code: ", (void *)(&retCode), sizeof(retCode));
		return;
	}
	else
	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_FDIPlayStop_ERROR, DIAG_INFORMATION);
		diagStructPrintf("Stop play from FDI - failed, return code: ", (void *)(&retCode), sizeof(retCode));
		return;
	}



} /* end of AUDIO_TEST_FDIPlayStop */






/*******************************************************************************
 * Function:	PcmPlay_TransferFunc
 ********************************************************************************
 * Description:
 *
 *
 * Parameters: Stream type
 * Return value:
 *
 * Notes:
 ********************************************************************************/

static void PcmPlay_TransferFunc (unsigned int *buff)
{


	if (buff == NULL)
		return;
	if (recordPlayFlag)
		return;

	playCounter += PCM_BUFF_SIZE;
	if ((playCounter * sizeof(UINT16)) < sizeOfFile)
		memcpy(buff, RecordPlayBuffer + (playCounter - PCM_BUFF_SIZE), PCM_BUFF_SIZE * sizeof(WORD));
	else
	{
		/* reached the end of the buffer */
		recordPlayFlag = 1;
		DIAG_FILTER(Audio, AUDIO_TEST, PcmPlay_TransferFunc, DIAG_INFORMATION);
		diagPrintf("stop playing from FDI - reached the end of the buffer");
		AUDIO_TEST_FDIPlayStop(NULL);
		return;


	}

}

#ifndef INTEL_2CHIP_PLAT_BVD

/***************************** testing vocoder streaming ***************************************/

static void AUDIO_TEST_VocoderPlayDownLinkCBFunc(unsigned int *buff);
static void AUDIO_TEST_VocoderRecordUpLinkCBFunc(unsigned int *buff);


static __align(4) unsigned short _FrameBuf[3 * 50][19] = { 0 };

/*******************************************************************************
 * Function:	AUDIO_TEST_VocoderRecordUpLinkCBFunc
 ********************************************************************************
 * Description:  packet transfer function of the vocoder record stream
 *
 *
 * Parameters: Stream type
 * Return value:
 *
 * Notes:
 ********************************************************************************/

static void AUDIO_TEST_VocoderRecordUpLinkCBFunc(unsigned int *buff)
{
	static unsigned long i = 0;

	memcpy((unsigned char *)&_FrameBuf[i++ % (3 * 50)][0], (unsigned char *)buff, SizeOfFrame * sizeof(unsigned short));
}


/*******************************************************************************
 * Function:	AUDIO_TEST_VocodrStreamingRecordStart
 ********************************************************************************
 * Description: start the record of the vocoder streaming.
 *
 *
 * Parameters: Stream type
 * Return value:
 *
 * Notes:
 ********************************************************************************/

//ICAT EXPORTED FUNCTION - Audio,AUDIO_TEST,AUDIO_TEST_VocodrStreamingRecordStart
void AUDIO_TEST_VocodrStreamingRecordStart(unsigned int *buff)
{
	UINT32* input = (UINT32*)buff;
	UINT32 retCode = ACM_RC_OK;
	ACM_AudioVolume volume = input[1];
	UINT16 streamType = input[2];

	Mic_Device = input[0];



	retCode = ACMAudioDeviceEnable(Mic_Device, ACM_MSA_PCM, volume);
	if (retCode == ACM_RC_DEVICE_ALREADY_ENABLED)
		deviceEnabled = 1;  /* we indicate that the device is already enabled so we
				     * will not disable it when stopping the record */

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

	switch (streamType) //detemines the frame size
	{
	case ACM_HR:
		SizeOfFrame = 8;
		break;
	case ACM_FR:
		SizeOfFrame = 16;
		break;
	case ACM_EFR:
		SizeOfFrame = 16;
		break;
	case ACM_AMR_MR475:
		SizeOfFrame = 7;
		break;
	case ACM_AMR_MR515:
		SizeOfFrame = 7;
		break;
	case ACM_AMR_MR59:
		SizeOfFrame = 8;
		break;
	case  ACM_AMR_MR67:
		SizeOfFrame = 8;
		break;
	case ACM_AMR_MR74:
		SizeOfFrame = 10;
		break;
	case ACM_AMR_MR795:
		SizeOfFrame = 11;
		break;
	case ACM_AMR_MR102:
		SizeOfFrame = 14;
		break;
	case ACM_AMR_MR122:
		SizeOfFrame = 16;
		break;
	default:
		SizeOfFrame = 16;     //treated like 122
		break;
	}

	if (retCode == ACM_RC_OK || retCode == ACM_RC_DEVICE_ALREADY_ENABLED)
	{
		if ((streamType >= ACM_HR) && (streamType <= ACM_AMR_MR122))
		{
			retCode = ACMAudioStreamInStart(streamType, ACM_NEAR_END, AUDIO_TEST_VocoderRecordUpLinkCBFunc);
		}
		else    //treated like 122
		{
			retCode = ACMAudioStreamInStart(ACM_AMR_MR122, ACM_NEAR_END, AUDIO_TEST_VocoderRecordUpLinkCBFunc);
		}
	}
	if (retCode == ACM_RC_OK)
	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_VocodrStreamingRecordStart_Success, DIAG_INFORMATION);
		diagPrintf("start record vocoder streaming - success");
		return;
	}
	else
	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_VocodrStreamingRecordStart_ERROR, DIAG_INFORMATION);
		diagStructPrintf("start record vocoder streaming - failed, return code: ", (void *)(&retCode), sizeof(retCode));
		return;
	}

}


/*******************************************************************************
 * Function:	AUDIO_TEST_VocoderStreamingRecordStop
 ********************************************************************************
 * Description: stop the record of the vocoder streaming.
 *
 *
 * Parameters: Stream type
 * Return value:
 *
 * Notes:
 ********************************************************************************/

//ICAT EXPORTED FUNCTION - Audio,AUDIO_TEST,AUDIO_TEST_VocoderStreamingRecordStop
void AUDIO_TEST_VocoderStreamingRecordStop(unsigned int* buff)
{
	UINT32 retCode = ACM_RC_OK;

	if (deviceEnabled == 0)
		retCode = ACMAudioDeviceDisable(Mic_Device, ACM_MSA_PCM);
	deviceEnabled = 0;


	if (retCode == ACM_RC_OK)
		retCode = ACMAudioStreamInStop(AUDIO_TEST_VocoderRecordUpLinkCBFunc);

	if (retCode == ACM_RC_OK)
	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_ACMVocoderStreamingMemoRecordStop_Success, DIAG_INFORMATION);
		diagPrintf("stop record vocoder streaming - success");
		return;
	}
	else
	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_ACMVocoderStreamingMemoRecordStop_ERROR, DIAG_INFORMATION);
		diagStructPrintf("stop record vocoder streaming - failed, return code: ", (void *)(&retCode), sizeof(retCode));
		return;
	}


}  //End of AUDIO_TEST_ACMVocoderStreamingMemoRecordStop


/***************************** testing play  (using vocoder streaming) ***************************************/

/*******************************************************************************
 * Function:	AUDIO_TEST_VocoderPlayDownLinkCBFunc
 ********************************************************************************
 * Description: call back function of the vocoder streaming - play.
 *
 *
 * Parameters: Stream type
 * Return value:
 *
 * Notes:
 ********************************************************************************/


static void AUDIO_TEST_VocoderPlayDownLinkCBFunc(unsigned int *buff)
{
	static unsigned long i = 0;

	memcpy((unsigned char *)buff, (unsigned char *)&_FrameBuf[i++ % (3 * 50)][0], SizeOfFrame * sizeof(unsigned short));
}



/*******************************************************************************
 * Function:	AUDIO_AUDIO_TEST_ACMVocoderStreamingPlayStart
 ********************************************************************************
 * Description: start playback of the recorded buffer (vocoder stream).
 *
 *
 * Parameters: Stream type
 * Return value:
 *
 * Notes:
 ********************************************************************************/

//ICAT EXPORTED FUNCTION - Audio,AUDIO_TEST,AUDIO_TEST_VocoderStreamingPlayStart
void AUDIO_TEST_VocoderStreamingPlayStart(void *buff)
{

	UINT32* input = (UINT32 *)buff;
	UINT32 retCode = ACM_RC_OK;
	ACM_AudioVolume volume = input[1];
	UINT16 streamType = input[2];

	Speaker_device = input[0];
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

	switch (streamType) //detemines the frame size
	{
	case ACM_HR:
		SizeOfFrame = 8;
		break;
	case ACM_FR:
		SizeOfFrame = 16;
		break;
	case ACM_EFR:
		SizeOfFrame = 16;
		break;
	case ACM_AMR_MR475:
		SizeOfFrame = 7;
		break;
	case ACM_AMR_MR515:
		SizeOfFrame = 7;
		break;
	case ACM_AMR_MR59:
		SizeOfFrame = 8;
		break;
	case  ACM_AMR_MR67:
		SizeOfFrame = 8;
		break;
	case ACM_AMR_MR74:
		SizeOfFrame = 10;
		break;
	case ACM_AMR_MR795:
		SizeOfFrame = 11;
		break;
	case ACM_AMR_MR102:
		SizeOfFrame = 14;
		break;
	case ACM_AMR_MR122:
		SizeOfFrame = 16;
		break;
	default:
		SizeOfFrame = 16;     //treated like 122
		break;
	}


	retCode = ACMAudioDeviceEnable(Speaker_device, ACM_MSA_PCM, volume);
	if (retCode == ACM_RC_DEVICE_ALREADY_ENABLED)
		deviceEnabled = 1;  /* we indicate that the device is already enabled so we
				     * will not disable it when stopping the record */

	if (retCode == ACM_RC_DEVICE_ALREADY_ENABLED || retCode == ACM_RC_OK)
	{
		if ((streamType >= ACM_HR) && (streamType <= ACM_AMR_MR122))
		{
			retCode = ACMAudioStreamOutStart(streamType, ACM_NEAR_END, ACM_NOT_COMB_WITH_CALL, AUDIO_TEST_VocoderPlayDownLinkCBFunc);
		}
		else
		{
			retCode = ACMAudioStreamOutStart(ACM_AMR_MR122, ACM_NEAR_END, ACM_NOT_COMB_WITH_CALL, AUDIO_TEST_VocoderPlayDownLinkCBFunc);
		}
	}
	if (retCode == ACM_RC_OK)
	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_VocoderStreamingPlayStart_Success, DIAG_INFORMATION);
		diagPrintf("start playback vocoder streaming - success");
		return;
	}
	else
	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_VocoderStreamingPlayStart_ERROR, DIAG_INFORMATION);
		diagStructPrintf("start playback vocoder streaming - failed, return code: ", (void *)(&retCode), sizeof(retCode));
		return;
	}
}

/*******************************************************************************
 * Function:	AUDIO_TEST_VocoderStreamingPlayStop
 ********************************************************************************
 * Description: start playback of the recorded buffer (vocoder stream).
 *
 *
 * Parameters: Stream type
 * Return value:
 *
 * Notes:
 ********************************************************************************/

//ICAT EXPORTED FUNCTION - Audio,AUDIO_TEST,AUDIO_TEST_VocoderStreamingPlayStop
void AUDIO_TEST_VocoderStreamingPlayStop(void)
{

	UINT32 retCode = ACM_RC_OK;

	if (deviceEnabled == 0)
		retCode = ACMAudioDeviceDisable(Speaker_device, ACM_MSA_PCM);
	deviceEnabled = 0;

	if (retCode == ACM_RC_OK)
		retCode = ACMAudioStreamOutStop(AUDIO_TEST_VocoderPlayDownLinkCBFunc);

	if (retCode == ACM_RC_OK)
	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_ACMVocoderStreamingPlayStop_Success, DIAG_INFORMATION);
		diagPrintf("stop playback vocoder streaming - success");
		return;
	}
	else
	{
		DIAG_FILTER(Audio, AUDIO_TEST, AUDIO_TEST_ACMVocoderStreamingPlayStop_ERROR, DIAG_INFORMATION);
		diagStructPrintf("stop playback vocoder streaming - failed, return code: ", (void *)(&retCode), sizeof(retCode));
		return;
	}
}   // End of AUDIO_TEST_VocoderStreamingPlayStop

#endif //INTEL_2CHIP_PLAT_BVD

















































































































































































































