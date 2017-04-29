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

#include <stdlib.h>
#include "acm.h"
#include "acmtypes.h"
#include "audiobluetooth.h"
#include <pxa_dbg.h>


/*----------- Local macro definitions ----------------------------------------*/
#define MASK8(a)        ((unsigned char)((a) & 0x000000FF))

#define printf DPRINTF

extern int gCPIsPCMMaster;
static unsigned int gTotalPathEnable = 0;

static void            blueToothInit(unsigned char reinit, ACM_SspAudioConfiguration **sspAudioConfiguration, signed short **ditherGenConfig);
static ACM_DigitalGain blueToothDeviceEnable(unsigned char path, ACM_AudioVolume volume);
static ACM_DigitalGain blueToothDeviceDisable(unsigned char path);
static ACM_DigitalGain blueToothDeviceVolumeSet(unsigned char path, ACM_AudioVolume volume);
static ACM_DigitalGain blueToothDeviceMute(unsigned char path, ACM_AudioMute muteEnable);
static short           blueToothGetPathsStatus(char* data, short length);
static ACM_AnalogGain  blueToothGetPathAnalogGain(unsigned char path);
static ACM_DigitalGain blueToothSetPathsAnalogGain(unsigned char regIndex, unsigned char regValue, unsigned char regMask, unsigned char regShift);
static unsigned int blueToothGetActivePathNum(void);


/*----------- Local type definitions -----------------------------------------*/
typedef signed short BT_SlopeType;
typedef signed short BT_OffsetType;

typedef struct
{
	BT_SlopeType slope;
	BT_OffsetType offset;
} BlueToothSlopeOffset;

//ICAT EXPORTED STRUCT
typedef struct
{
	unsigned long gainId;
	unsigned long path;
	signed short Slope;
	signed short Offset;
} BlueToothNvmRecord;

typedef struct
{
	ACM_AudioMute mute;
	ACM_DigitalGain gain;
	unsigned char enable;
	ACM_AudioVolume volume;
} BTPathsDB;


/*----------- Local macro definitions ----------------------------------------*/
#define BT_GAIN_CALC(vOL, sLOPEoFFSET)   (((((signed long)(sLOPEoFFSET.slope) * (vOL) + ((sLOPEoFFSET.offset) << 3)) >> 9) + 1) >> 1)

#define CLIP_VALUE(g, l, h) {     if ((g) > (h)) (g) = (h);							       \
				  else if ((g) < (l) ) (g) = (l); }

#define SET_STRING(buf, str) strcpy((buf), (str))

#define MASK8(a)        ((unsigned char)((a) & 0x000000FF))


/*----------- Local constant definitions -------------------------------------*/
#define HIGHEST_DIGITAL_GAIN                12
#define LOWEST_DIGITAL_GAIN                -36
#define NOT_USED                       { 0x7FFF, 0x7FFF }


/*----------- Global constant definitions ------------------------------------*/

ACM_ComponentHandle HWBlueToothHandle =
{
	blueToothInit,
	blueToothDeviceEnable,
	blueToothDeviceDisable,
	blueToothDeviceVolumeSet,
	blueToothDeviceMute,
	blueToothGetPathsStatus,
	blueToothGetPathAnalogGain,
	blueToothSetPathsAnalogGain,
	blueToothGetActivePathNum,
	NULL,
	NULL
};


/*----------- Local definitions ------------------------------------*/
static const BlueToothSlopeOffset calibrationData[NUM_OF_BLUETOOTH_PATHS] =
{
	{ 246, -4352 }, 	//BLUETOOTH_OUT
	{ 0,   0     }, 	//BLUETOOTH_IN
	{ 0,   0     }, 	//BLUETOOTH_APP_OUT
	{ 0,   0     }		//BLUETOOTH_APP_IN
};

static const BlueToothSlopeOffset defaultCalibrationData[NUM_OF_BLUETOOTH_PATHS] =
{
	{ 246, -4352 },         //BLUETOOTH_OUT
	{ 0,   0     },         //BLUETOOTH_IN
	{ 0,   0     },         //BLUETOOTH_APP_OUT
	{ 0,   0     }          //BLUETOOTH_APP_IN
};

static const ACM_SspAudioConfiguration _sspAudioBluetoothConfiguration =
{
	/* shift       */ 16,
	/* SSCR0_HIGH  */ 0xC0D0,
	/* SSCR0_LOW   */ 0x001F,
	/* SSCR1_HIGH  */ 0x6000,
	/* SSCR1_LOW   */ 0x1C01,
	/* SSTSA_LOW   */ 0x0001,
	/* SSRSA_LOW   */ 0x0001,
	/* SSPSP_HIGH  */ 0x0000,
	/* SSPSP_LOW   */ 0x0000,
	/* SSACD_LOW   */ 0x00EA,
	/* SSACDD_HIGH */ 0x130B,
	/* SSACDD_LOW  */ 0x0008,
	/* devicePort  */ 0,  /*GSSP0_PORT*/ /* DevicePort (GSSP0_PORT=0, GSSP1_PORT=1) */
	/* SSP_Rate  */ 0,
	/* SSP_Mode  */ 0  /*GSSP0_PORT*/ /* DevicePort (GSSP0_PORT=0, GSSP1_PORT=1) */
};



static void blueToothInit(unsigned char reinit, ACM_SspAudioConfiguration **sspAudioConfiguration, signed short **ditherGenConfig)
{
	UNUSEDPARAM(reinit)
	UNUSEDPARAM(sspAudioConfiguration)
	UNUSEDPARAM(ditherGenConfig)
	printf("blueToothInit\n");

}


static ACM_DigitalGain blueToothDeviceEnable(unsigned char path, ACM_AudioVolume volume)
{
	printf("blueToothDeviceEnable: path %d, volume %d\n", path, volume);
	if (gCPIsPCMMaster == 1)
	{
		system("hcitool -i hci0 cmd 0x3f 0x1d 0x01");
		system("hcitool -i hci0 cmd 0x3f 0x28 0x03 0x00 0x03");
		system("hcitool -i hci0 cmd 0x3f 0x07 0x02");
	}
	else
	{
		system("hcitool -i hci0 cmd 0x3f 0x1d 0x01");
		system("hcitool -i hci0 cmd 0x3f 0x28 0x03 0x00 0x03");
		system("hcitool -i hci0 cmd 0x3f 0x07 0x00");
	}

	gTotalPathEnable = 1;

	return 0;
}


static ACM_DigitalGain blueToothDeviceDisable(unsigned char path)
{
	printf("blueToothDeviceDisable: path %d\n", path);

	gTotalPathEnable = 0;

	return 0;
}


static ACM_DigitalGain blueToothDeviceVolumeSet(unsigned char path, ACM_AudioVolume volume)
{
	printf("blueToothDeviceVolumeSet: path %d, volume %d\n", path, volume);
	return 0;
}


static ACM_DigitalGain blueToothDeviceMute(unsigned char path, ACM_AudioMute muteEnable)
{
	printf("blueToothDeviceMute: path %d, muteEnable %d\n", path, muteEnable);
	return 0;
}


static short blueToothGetPathsStatus(char* data, short length)
{
	UNUSEDPARAM(data)
	UNUSEDPARAM(length)
	printf("blueToothGetPathStatus\n");
	return 0;
}


static ACM_AnalogGain blueToothGetPathAnalogGain(unsigned char path)
{
	UNUSEDPARAM(path)
	printf("blueToothGetAnalogGain\n");
	return 0;
}

static ACM_DigitalGain blueToothSetPathsAnalogGain(unsigned char regIndex, unsigned char regValue, unsigned char regMask, unsigned char regShift)
{
	UNUSEDPARAM(regIndex)
	UNUSEDPARAM(regValue)
	UNUSEDPARAM(regMask)
	UNUSEDPARAM(regShift)
	printf("blueToothSetPathsAnalogGain\n");
	return 0;
}

static unsigned int blueToothGetActivePathNum(void)
{
	return gTotalPathEnable;
}

