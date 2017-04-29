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
* Filename: ACM.c
*
* Authors: Isar Ariel
*
* Description: Header file for ACM.
*
* Last Updated : Jun 11 2009 By Paul Shen
*
* Notes:
******************************************************************************/
#ifndef _ACM_H_
#define _ACM_H_
#include "acmtypes.h"

typedef enum
{
	ACM_RC_MALLOC_ERROR = 0,    //Add by jackie
	ACM_RC_OK = 1,
	ACM_RC_DEVICE_ALREADY_ENABLED,
	ACM_RC_DEVICE_ALREADY_DISABLED,
	ACM_RC_NO_MUTE_CHANGE_NEEDED,
	ACM_RC_INVALID_VOLUME_CHANGE,
	ACM_RC_DEVICE_FORMAT_NOT_FOUND,
	ACM_RC_BUFFER_GET_FUNC_INVALID,
	ACM_RC_STREAM_OUT_NOT_PERFORMED,
	ACM_RC_STREAM_IN_NOT_PERFORMED,
	ACM_RC_STREAM_OUT_TO_BE_STOPPED_NOT_ACTIVE,
	ACM_RC_STREAM_IN_TO_BE_STOPPED_NOT_ACTIVE,
	ACM_RC_I2S_INVALID_DATA_POINTER,
	ACM_RC_I2S_INVALID_DATA_SIZE,
	ACM_RC_I2S_INVALID_NOTIFICATION_THRESHOLD,
	ACM_RC_I2S_MESSAGE_QUEUE_IS_FULL,
	ACM_RC_MEMORY_ALREADY_INITIALISED,

	ACM_RC_NEED_DISABLE_PATH,       //Add by jackie

	ACM_RC_ENUM_32_BIT = 0x7FFFFFFF //32bit enum compiling enforcement
} ACM_ReturnCode;


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
	/* Must be at the end */
	ACM_NO_STREAM_TYPE,
	ACM_NUM_OF_STREAM_TYPES = ACM_NO_STREAM_TYPE,


	ACM_STREAM_TYPE_ENUM_32_BIT = 0x7FFFFFFF //32bit enum compiling enforcement
} ACM_StreamType;


typedef enum
{
	ACM_MSA_PCM = 0,  /* first format must be '0' - used by 'for' loops */
	ACM_XSCALE_PCM,
	ACM_I2S,
	ACM_AUDIO_DATA,   /* For DAI */
	ACM_AUX_FM,
	ACM_AUX_HW_MIDI,
	ACM_AUX_APP,

	/* Must be at the end */
	ACM_NO_FORMAT,
	ACM_NUM_OF_AUDIO_FORMATS = ACM_NO_FORMAT,


	ACM_AUDIO_FORMAT_ENUM_32_BIT = 0x7FFFFFFF //32bit enum compiling enforcement
} ACM_AudioFormat;

typedef enum
{
	/* first device must be '0' - used by 'for' loops */
	ACM_MAIN_SPEAKER     = 0        //TBD as ACM_OUTPUT_DEVICE_1, 0
	, ACM_AUX_SPEAKER               //TBD as ACM_OUTPUT_DEVICE_2, 1
	, ACM_STEREO_SPEAKER            //TBD as ACM_OUTPUT_DEVICE_3, 2
	, ACM_HEADSET_SPEAKER = ACM_STEREO_SPEAKER
	, ACM_MONO_LEFT_SPEAKER         //TBD as ACM_OUTPUT_DEVICE_4, 3
	, ACM_MONO_RIGHT_SPEAKER        //TBD as ACM_OUTPUT_DEVICE_5, 4
	, ACM_BUZZER                    //TBD as ACM_OUTPUT_DEVICE_6, 5
	, ACM_MIC                       //TBD as ACM_INPUT_DEVICE_1, 6
	, ACM_MIC_DIFF                  //TBD as ACM_INPUT_DEVICE_2, 7
	, ACM_AUX_MIC                   //TBD as ACM_INPUT_DEVICE_3, 8
	, ACM_AUX_MIC_DIFF              //TBD as ACM_INPUT_DEVICE_4, 9
	, ACM_BLUETOOTH_SPEAKER         //TBD as ACM_OUTPUT_DEVICE_7, 10
	, ACM_BLUETOOTH_MIC             //TBD as ACM_INPUT_DEVICE_5, 11
	, ACM_DAI_OUT                   //TBD as ACM_OUTPUT_DEVICE_8, 12
	, ACM_DAI_IN                    //TBD as ACM_INPUT_DEVICE_6, 13
	, ACM_CAR_KIT_SPEAKER           //TBD as ACM_OUTPUT_DEVICE_9, 14
	, ACM_CAR_KIT_MIC               //TBD as ACM_INPUT_DEVICE_7, 15
	, ACM_INPUT_DEVICE_8            //former ACM_CUSTOM_MIC1, 16
	, ACM_HEADSET_MIC               //former ACM_CUSTOM_MIC2, 17
	, ACM_INPUT_DEVICE_9 = ACM_HEADSET_MIC //17

	, ACM_MIC_EC			// Main Mic with EC path enabled
	, ACM_AUX_MIC_EC		// Main AUX Mic with EC path enabled
	, ACM_HEADSET_MIC_EC		// Headset Mic with EC path enabled
	, ACM_MIC_LOOP_SPEAKER		// Main Mic loop to main speaker
	, ACM_MIC_LOOP_EARPIECE 	// Main Mic loop to main reiver
	, ACM_HEADSET_MIC_LOOP		// Headset Mic loop to headset

	, ACM_TTY_IN_45
	, ACM_INPUT_DEVICE_10 = ACM_TTY_IN_45      //former ACM_CUSTOM_MIC3

	, ACM_TTY_IN_50
	, ACM_INPUT_DEVICE_11 = ACM_TTY_IN_50           //former ACM_CUSTOM_MIC4

	, ACM_INPUT_DEVICE_12                           //former ACM_CUSTOM_MIC5
	, ACM_INPUT_DEVICE_13                           //former ACM_CUSTOM_MIC6
	, ACM_INPUT_DEVICE_14                           //former ACM_CUSTOM_MIC7
	, ACM_INPUT_DEVICE_15                           //former ACM_CUSTOM_MIC8
	, ACM_INPUT_DEVICE_16                           //former ACM_CUSTOM_MIC9
	, ACM_INPUT_DEVICE_17                           //former ACM_CUSTOM_MIC10
	, ACM_INPUT_DEVICE_18                           //former ACM_CUSTOM_MIC11
	, ACM_INPUT_DEVICE_19                           //former ACM_CUSTOM_MIC12
	, ACM_INPUT_DEVICE_20                           //former ACM_CUSTOM_MIC13
	, ACM_INPUT_TEST_DEVICE = ACM_INPUT_DEVICE_20
	, ACM_INPUT_DEVICE_21                           //former ACM_CUSTOM_MIC14
	, ACM_LINE_OUT = ACM_INPUT_DEVICE_21

	, ACM_INPUT_DEVICE_22           //former ACM_CUSTOM_MIC15
	, ACM_INPUT_DEVICE_23           //former ACM_CUSTOM_MIC16
	, ACM_INPUT_DEVICE_24           //former ACM_CUSTOM_MIC17
	, ACM_INPUT_DEVICE_25           //former ACM_CUSTOM_MIC18
	, ACM_INPUT_DEVICE_26           //former ACM_CUSTOM_MIC19
	, ACM_INPUT_DEVICE_27           //former ACM_CUSTOM_MIC20

	, ACM_TTY_OUT_45
	, ACM_OUTPUT_DEVICE_10 = ACM_TTY_OUT_45     //former ACM_CUSTOM_SPEAKER1

	, ACM_TTY_OUT_50
	, ACM_OUTPUT_DEVICE_11 = ACM_TTY_OUT_50         //former ACM_CUSTOM_SPEAKER2

	, ACM_OUTPUT_DEVICE_12                          //former ACM_CUSTOM_SPEAKER3
	, ACM_OUTPUT_DEVICE_13                          //former ACM_CUSTOM_SPEAKER4
	, ACM_OUTPUT_DEVICE_14                          //former ACM_CUSTOM_SPEAKER5
	, ACM_OUTPUT_DEVICE_15                          //former ACM_CUSTOM_SPEAKER6
	, ACM_OUTPUT_DEVICE_16                          //former ACM_CUSTOM_SPEAKER7
	, ACM_OUTPUT_DEVICE_17                          //former ACM_CUSTOM_SPEAKER8
	, ACM_OUTPUT_DEVICE_18                          //former ACM_CUSTOM_SPEAKER9
	, ACM_OUTPUT_DEVICE_19                          //former ACM_CUSTOM_SPEAKER10
	, ACM_OUTPUT_DEVICE_20                          //former ACM_CUSTOM_SPEAKER11
	, ACM_OUTPUT_TEST_DEVICE = ACM_OUTPUT_DEVICE_20
	, ACM_OUTPUT_DEVICE_21                          //former ACM_CUSTOM_SPEAKER12
	, ACM_OUTPUT_DEVICE_22                          //former ACM_CUSTOM_SPEAKER13
	, ACM_OUTPUT_DEVICE_23                          //former ACM_CUSTOM_SPEAKER14
	, ACM_OUTPUT_DEVICE_24                          //former ACM_CUSTOM_SPEAKER15
	, ACM_OUTPUT_DEVICE_25                          //former ACM_CUSTOM_SPEAKER16
	, ACM_OUTPUT_DEVICE_26                          //former ACM_CUSTOM_SPEAKER17
	, ACM_OUTPUT_DEVICE_27                          //former ACM_CUSTOM_SPEAKER18
	, ACM_OUTPUT_DEVICE_28                          //former ACM_CUSTOM_SPEAKER19
	, ACM_OUTPUT_DEVICE_29                          //former ACM_CUSTOM_SPEAKER20

	, ACM_BLUETOOTH1_SPEAKER
	, ACM_BLUETOOTH1_MIC
	, ACM_BLUETOOTH2_SPEAKER
	, ACM_BLUETOOTH2_MIC
	, ACM_BLUETOOTH3_SPEAKER
	, ACM_BLUETOOTH3_MIC
	, ACM_BLUETOOTH4_SPEAKER
	, ACM_BLUETOOTH4_MIC
	, ACM_BLUETOOTH5_SPEAKER
	, ACM_BLUETOOTH5_MIC
	, ACM_BLUETOOTH6_SPEAKER
	, ACM_BLUETOOTH6_MIC
	, ACM_BLUETOOTH7_SPEAKER
	, ACM_BLUETOOTH7_MIC
	, ACM_BLUETOOTH8_SPEAKER
	, ACM_BLUETOOTH8_MIC
	, ACM_BLUETOOTH9_SPEAKER
	, ACM_BLUETOOTH9_MIC

	//Jackie,2011-0223
	//To keep compatible to old audio_device.nvm, MUST fix ACM_NOT_CONNECTED to 0x50
	//0x50 can not be used as audio device number
	,ACM_NOT_CONNECTED=0x50

	//Add new audio device after 0x50

	//Jackie,2011-0222
	//Loop include codec and MSA
	,ACM_MAIN_SPEAKER__LOOP
	,ACM_AUX_SPEAKER__LOOP
	,ACM_HEADSET_SPEAKER__LOOP
	,ACM_MIC__LOOP
	,ACM_AUX_MIC__LOOP
	,ACM_HEADSET_MIC__LOOP

	//force speaker
	,ACM_FORCE_SPEAKER

	//Customized audio device, start from 100
	,ACM_CUSTOMER_LGE = 100

	// audio device from 0x200 for non-voice-call case
	, ACM_AUDIODEVICE_APP_START=0x200

	/* Must be at the end */
	, ACM_NUM_OF_AUDIO_DEVICES = ACM_NOT_CONNECTED

	, ACM_AUDIO_DEVICE_ENUM_32_BIT = 0x7FFFFFFF //32bit enum compiling enforcement
} ACM_AudioDevice;


typedef enum
{
	ACM_BOTH_ENDS = 0       /* Highest priority */
	, ACM_NEAR_END
	, ACM_FAR_END           /* Lowest priority */
	, ACM_NO_END

	, ACM_SRC_DST_ENUM_32_BIT = 0x7FFFFFFF //32bit enum compiling enforcement
} ACM_SrcDst;

//typedef enum
//{
//	ACM_MUTE_OFF = 0,
//	ACM_MUTE_ON = 1,
//
//       ACM_AUDIO_MUTE_ENUM_32_BIT  = 0x7FFFFFFF //32bit enum compiling enforcement
//} ACM_AudioMute;

typedef enum
{
	ACM_NOT_COMB_WITH_CALL = 0,                     /* Lowest priority */
	ACM_COMB_WITH_CALL = 1,                         /* Highest priority */

	ACM_COMB_WITH_CALL_ENUM_32_BIT  = 0x7FFFFFFF    //32bit enum compiling enforcement
} ACM_CombWithCall;


typedef void (*ACM_PacketTransferFunc)(unsigned int *buff);


ACM_ReturnCode ACMAudioStreamInStop(ACM_PacketTransferFunc packetTransferFunc);
ACM_ReturnCode ACMAudioStreamOutStop(ACM_PacketTransferFunc packetTransferFunc);


ACM_ReturnCode ACMAudioStreamInStart(ACM_StreamType streamType,
				     ACM_SrcDst srcDst,
				     ACM_PacketTransferFunc packetTransferFunc);

ACM_ReturnCode ACMAudioStreamOutStart(ACM_StreamType streamType,
				      ACM_SrcDst srcDst,
				      ACM_CombWithCall combWithCall,
				      ACM_PacketTransferFunc packetTransferFunc);

ACM_ReturnCode ACMAudioDeviceMute(ACM_AudioDevice device,
				  ACM_AudioFormat format,
				  ACM_AudioMute mute);

ACM_ReturnCode ACMAudioDeviceVolumeSet(ACM_AudioDevice device,
				       ACM_AudioFormat format,
				       ACM_AudioVolume volume);
ACM_ReturnCode ACMAudioDeviceDisable(ACM_AudioDevice device,
				     ACM_AudioFormat format);

ACM_ReturnCode ACMAudioDeviceEnable(ACM_AudioDevice device,
				    ACM_AudioFormat format,
				    ACM_AudioVolume volume);

ACM_ReturnCode ACMAudioSetCalibrationStatus(ACM_AudioCalibrationStatus calstatus);

ACM_ReturnCode ACMAudioPathEnable(ACM_Component component, unsigned char path, ACM_AudioVolume volume);

ACM_ReturnCode ACMAudioPathDisable(ACM_Component component, unsigned char path);

#define MSASETTINGDATALEN 256

#define MSGQ_TIMEOUT   1000000000 //OSA_SUSPEND

#define UNUSEDPARAM(param) (void)param;

typedef struct
{
  unsigned char path;
  unsigned char gainID;
  unsigned short slope;
  unsigned short offset;
} ACM_SYNC_Gain;

ACM_ReturnCode ACMAudioSetMSASettings(const char *data);
/* support dynamically switch sample switch */
ACM_ReturnCode ACMSwitchingSampleRate(int rate);

void ACMInit(void);
void ACMDeinit();

#endif  /* _ACM_H_ */


