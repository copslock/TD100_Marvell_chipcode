/******************************************************************************
*               MODULE IMPLEMENTATION FILE
*******************************************************************************
* Title: audioMICCO
*
* Filename: audioMICCO.c
*
* Target, platform: Common Platform, SW platform
*
* Authors: Yossi Gabay (BlueGrove driver), Tal Klap (Modifications for MICCO)
*
* Description: Micco component
*
* Last Updated:
*
* Notes:
******************************************************************************/

#include "acm.h"
#include "global_types.h"
#include "audiomiccoapi.h"
#include "audiomicco.h"

#include "acmtypes.h"
#include "amixer.h"

#include <pxa_dbg.h>

#define INVALID_FIELD_VALUE             0x1F

void MICCOInit(unsigned char reinit, ACM_SspAudioConfiguration **sspAudioConfiguration, signed short **ditherGenConfig);
//BOOL				PMCWriteRegister(UINT8 regToWrite, UINT8 value,BOOL image_ena);
void                                    PMCCodecMicInCntrl( PMC_micIn_t* pMicIn );
void                                    PMCCodecAuxInCntrl( PMC_aux_t* pAux);
void                                    PMCCodecAuxInCntrl( PMC_aux_t* pAux);
void                                    PMCCodecAuxInCntrl( PMC_aux_t* pAux);
void                                    PMCCodecMonoOutCntrl( PMC_mono_t* pMono );
void                                    PMCCodecStereoOutCntrl( PMC_stereo_t* pStereo );
void                                    PMCCodecPcmInCntrl( PMC_pcmIn_t* pPcmIn );
void                                    PMCCodecPcmOutCntrl( PMC_pcmOut_t* pPcmOut,  PMC_pcmIn_t* pPcmIn );
void                                    PMCCodecSideToneCntrl( PMC_pcmOut_t* pPcmOut );
void                                    PMCCodecHiFiCntrl( PMC_hifi_t* pHifi );
void                                    MICCOReadFdiDataAPI(void);



/*===============================================================*/
/*=========== Local variable definitions ========================*/
/*===============================================================*/


ACM_ComponentHandle HWMICCOHandle  =
{
	MICCOInit,
	MICCOPathEnable,
	MICCOPathDisable,
	MICCOPathVolumeSet,
	MICCOPathMute,
	MICCOGetPathsStatus,
	MICCOGetPathAnalogGain
};

//for Semaphore
static int miccoChipID = 0;
static BOOL miccoIsClkMaster = FALSE;


// try to move these def to the h file - klap
/*----------- Local constant definitions ------------------------------------*/

//Gain ID #0
#define MICCO_SPKRL_OUT_GAIN_MAX       18
static MICCO_SpkrLOutGainS gMICCOSpkrLOutGain[MICCO_SPKRL_OUT_GAIN_MAX] = {
	{ MICCO_PCM_TO_SPKRL,	   { 0x0064, 0x0400 } },
	{ MICCO_PCM_TO_SPKR,	   { 0x0064, 0x0400 } },
	{ MICCO_I2S_TO_SPKRL,	   { 0x0064, 0x0400 } },
	{ MICCO_I2S_TO_SPKR,	   { 0xFF44, 0x0F80 } },
	{ MICCO_I2S_TO_SPKR_MONO,  { 0x0064, 0x0400 } },
	{ MICCO_I2S_TO_SPKRL_MONO, { 0x0064, 0x0400 } },
	{ MICCO_AUX1_TO_SPKRL,	   { 0x0064, 0x0400 } },
	{ MICCO_AUX1_TO_SPKRL_PCM, { 0x0064, 0x0400 } },
	{ MICCO_AUX1_TO_SPKR,	   { 0x0064, 0x0400 } },
	{ MICCO_AUX1_TO_SPKR_PCM,  { 0x0064, 0x0400 } },
	{ MICCO_AUX2_TO_SPKRL,	   { 0x0064, 0x0400 } },
	{ MICCO_AUX2_TO_SPKRL_PCM, { 0x0064, 0x0400 } },
	{ MICCO_AUX2_TO_SPKR,	   { 0x0064, 0x0400 } },
	{ MICCO_AUX2_TO_SPKR_PCM,  { 0x0064, 0x0400 } },
	{ MICCO_AUX3_TO_SPKRL,	   { 0x0064, 0x0400 } },
	{ MICCO_AUX3_TO_SPKRL_PCM, { 0x0064, 0x0400 } },
	{ MICCO_AUX3_TO_SPKR,	   { 0x0064, 0x0400 } },
	{ MICCO_AUX3_TO_SPKR_PCM,  { 0x0064, 0x0400 } }
};

//Gain ID #1
#define MICCO_SPKRR_OUT_GAIN_MAX       18
static MICCO_SpkrROutGainS gMICCOSpkrROutGain[MICCO_SPKRR_OUT_GAIN_MAX] = {
	{ MICCO_PCM_TO_SPKRR,	   { 0x0064, 0x0400 } },
	{ MICCO_PCM_TO_SPKR,	   { 0x0064, 0x0400 } },
	{ MICCO_I2S_TO_SPKRR,	   { 0x0064, 0x0400 } },
	{ MICCO_I2S_TO_SPKR,	   { 0xFF44, 0x0F80 } },
	{ MICCO_I2S_TO_SPKR_MONO,  { 0x0064, 0x0400 } },
	{ MICCO_I2S_TO_SPKRR_MONO, { 0x0064, 0x0400 } },
	{ MICCO_AUX1_TO_SPKRR,	   { 0x0064, 0x0400 } },
	{ MICCO_AUX1_TO_SPKRR_PCM, { 0x0064, 0x0400 } },
	{ MICCO_AUX1_TO_SPKR,	   { 0x0064, 0x0400 } },
	{ MICCO_AUX1_TO_SPKR_PCM,  { 0x0064, 0x0400 } },
	{ MICCO_AUX2_TO_SPKRR,	   { 0x0064, 0x0400 } },
	{ MICCO_AUX2_TO_SPKRR_PCM, { 0x0064, 0x0400 } },
	{ MICCO_AUX2_TO_SPKR,	   { 0x0064, 0x0400 } },
	{ MICCO_AUX2_TO_SPKR_PCM,  { 0x0064, 0x0400 } },
	{ MICCO_AUX3_TO_SPKRR,	   { 0x0064, 0x0400 } },
	{ MICCO_AUX3_TO_SPKRR_PCM, { 0x0064, 0x0400 } },
	{ MICCO_AUX3_TO_SPKR,	   { 0x0064, 0x0400 } },
	{ MICCO_AUX3_TO_SPKR_PCM,  { 0x0064, 0x0400 } }
};

//Gain ID #2
#define MICCO_BEAR_OUT_GAIN_MAX       8
static MICCO_BearOutGainS gMICCOBearOutGain[MICCO_BEAR_OUT_GAIN_MAX] = {
	{ MICCO_PCM_TO_BEAR,	  { 0xFF44, 0x0F80 } },
	{ MICCO_I2S_TO_BEAR,	  { 0xFF44, 0x0F80 } },
	{ MICCO_AUX1_TO_BEAR,	  { 0x0064, 0x0400 } },
	{ MICCO_AUX1_TO_BEAR_PCM, { 0x0064, 0x0400 } },
	{ MICCO_AUX2_TO_BEAR,	  { 0x0064, 0x0400 } },
	{ MICCO_AUX2_TO_BEAR_PCM, { 0x0064, 0x0400 } },
	{ MICCO_AUX3_TO_BEAR,	  { 0x0064, 0x0400 } },
	{ MICCO_AUX3_TO_BEAR_PCM, { 0x0064, 0x0400 } }
};

//Gain ID #3
#define MICCO_MONO_OUT_GAIN_MAX       11
static MICCO_MonoOutGainS gMICCOMonoOutGain[MICCO_MONO_OUT_GAIN_MAX] = {
	{ MICCO_PCM_TO_MONO,	   { 0xFF34, 0x0A00 } },
	{ MICCO_I2S_TO_MONO,	   { 0xFF48, 0x0F80 } },
	{ MICCO_I2S_TO_SPKR_MONO,  { 0x0064, 0x0400 } },
	{ MICCO_I2S_TO_SPKRL_MONO, { 0x0064, 0x0400 } },
	{ MICCO_I2S_TO_SPKRR_MONO, { 0x0064, 0x0400 } },
	{ MICCO_AUX1_TO_MONO,	   { 0x0064, 0x0400 } },
	{ MICCO_AUX1_TO_MONO_PCM,  { 0x0064, 0x0400 } },
	{ MICCO_AUX2_TO_MONO,	   { 0x0064, 0x0400 } },
	{ MICCO_AUX2_TO_MONO_PCM,  { 0x0064, 0x0400 } },
	{ MICCO_AUX3_TO_MONO,	   { 0x0064, 0x0400 } },
	{ MICCO_AUX3_TO_MONO_PCM,  { 0x0064, 0x0400 } }
};

//Gain ID #4
#define MICCO_LINE_OUT_GAIN_MAX       12
static MICCO_LineOutGainS gMICCOLineOutGain[MICCO_LINE_OUT_GAIN_MAX] = {
	{ MICCO_PCM_TO_LINEOUT,	     { 0x0064, 0x0400 } },
	{ MICCO_I2S_TO_LINEOUT,	     { 0x0064, 0x0400 } },
	{ MICCO_MIC1_TO_LINEOUT,     { 0x0064, 0x0400 } },
	{ MICCO_MIC1_TO_LINEOUT_PCM, { 0x0064, 0x0400 } },
	{ MICCO_MIC2_TO_LINEOUT,     { 0x0064, 0x0400 } },
	{ MICCO_MIC2_TO_LINEOUT_PCM, { 0x0064, 0x0400 } },
	{ MICCO_AUX1_TO_LINEOUT,     { 0x0064, 0x0400 } },
	{ MICCO_AUX1_TO_LINEOUT_PCM, { 0x0064, 0x0400 } },
	{ MICCO_AUX2_TO_LINEOUT,     { 0x0064, 0x0400 } },
	{ MICCO_AUX2_TO_LINEOUT_PCM, { 0x0064, 0x0400 } },
	{ MICCO_AUX3_TO_LINEOUT,     { 0x0064, 0x0400 } },
	{ MICCO_AUX3_TO_LINEOUT_PCM, { 0x0064, 0x0400 } }
};



//Gain ID #5
#define MICCO_AUX1_IN_GAIN_MAX         13
static MICCO_AuxInGainS gMICCOAux1InGain[MICCO_AUX1_IN_GAIN_MAX] = {

	{ MICCO_AUX1_TO_PCM,	     { 0x0064, 0x0400 } },

	{ MICCO_AUX1_TO_MONO,	     { 0x0064, 0x0400 } },
	{ MICCO_AUX1_TO_MONO_PCM,    { 0x0064, 0x0400 } },
	{ MICCO_AUX1_TO_BEAR,	     { 0x0064, 0x0400 } },
	{ MICCO_AUX1_TO_BEAR_PCM,    { 0x0064, 0x0400 } },
	{ MICCO_AUX1_TO_SPKRL,	     { 0x0064, 0x0400 } },
	{ MICCO_AUX1_TO_SPKRL_PCM,   { 0x0064, 0x0400 } },
	{ MICCO_AUX1_TO_SPKRR,	     { 0x0064, 0x0400 } },
	{ MICCO_AUX1_TO_SPKRR_PCM,   { 0x0064, 0x0400 } },
	{ MICCO_AUX1_TO_SPKR,	     { 0x0064, 0x0400 } },
	{ MICCO_AUX1_TO_SPKR_PCM,    { 0x0064, 0x0400 } },
	{ MICCO_AUX1_TO_LINEOUT,     { 0x0064, 0x0400 } },
	{ MICCO_AUX1_TO_LINEOUT_PCM, { 0x0064, 0x0400 } }
};

//Gain ID #6
#define MICCO_AUX2_IN_GAIN_MAX         13
static MICCO_AuxInGainS gMICCOAux2InGain[MICCO_AUX2_IN_GAIN_MAX] = {

	{ MICCO_AUX2_TO_PCM,	     { 0x0064, 0x0400 } },
	{ MICCO_AUX2_TO_MONO,	     { 0x0064, 0x0400 } },
	{ MICCO_AUX2_TO_MONO_PCM,    { 0x0064, 0x0400 } },
	{ MICCO_AUX2_TO_BEAR,	     { 0x0064, 0x0400 } },
	{ MICCO_AUX2_TO_BEAR_PCM,    { 0x0064, 0x0400 } },
	{ MICCO_AUX2_TO_SPKRL,	     { 0x0064, 0x0400 } },
	{ MICCO_AUX2_TO_SPKRL_PCM,   { 0x0064, 0x0400 } },
	{ MICCO_AUX2_TO_SPKRR,	     { 0x0064, 0x0400 } },
	{ MICCO_AUX2_TO_SPKRR_PCM,   { 0x0064, 0x0400 } },
	{ MICCO_AUX2_TO_SPKR,	     { 0x0064, 0x0400 } },
	{ MICCO_AUX2_TO_SPKR_PCM,    { 0x0064, 0x0400 } },
	{ MICCO_AUX2_TO_LINEOUT,     { 0x0064, 0x0400 } },
	{ MICCO_AUX2_TO_LINEOUT_PCM, { 0x0064, 0x0400 } }
};

//Gain ID #7
#define MICCO_AUX3_IN_GAIN_MAX         13
static MICCO_AuxInGainS gMICCOAux3InGain[MICCO_AUX3_IN_GAIN_MAX] = {

	{ MICCO_AUX3_TO_PCM,	     { 0x0064, 0x0400 } },

	{ MICCO_AUX3_TO_MONO,	     { 0x0064, 0x0400 } },
	{ MICCO_AUX3_TO_MONO_PCM,    { 0x0064, 0x0400 } },
	{ MICCO_AUX3_TO_BEAR,	     { 0x0064, 0x0400 } },
	{ MICCO_AUX3_TO_BEAR_PCM,    { 0x0064, 0x0400 } },
	{ MICCO_AUX3_TO_SPKRL,	     { 0x0064, 0x0400 } },
	{ MICCO_AUX3_TO_SPKRL_PCM,   { 0x0064, 0x0400 } },
	{ MICCO_AUX3_TO_SPKRR,	     { 0x0064, 0x0400 } },
	{ MICCO_AUX3_TO_SPKRR_PCM,   { 0x0064, 0x0400 } },
	{ MICCO_AUX3_TO_SPKR,	     { 0x0064, 0x0400 } },
	{ MICCO_AUX3_TO_SPKR_PCM,    { 0x0064, 0x0400 } },
	{ MICCO_AUX3_TO_LINEOUT,     { 0x0064, 0x0400 } },
	{ MICCO_AUX3_TO_LINEOUT_PCM, { 0x0064, 0x0400 } }
};

//Gain ID #8
#define MICCO_MIC_IN_GAIN_MAX         6
static MICCO_MicInGainS gMICCOMicInGain[MICCO_MIC_IN_GAIN_MAX] = {
	{ MICCO_MIC1_TO_PCM,	     { 0x0000, 0x0200 } },
	{ MICCO_MIC2_TO_PCM,	     { 0x002F, 0x0000 } },
	{ MICCO_MIC1_TO_LINEOUT,     { 0x002F, 0x0000 } },
	{ MICCO_MIC1_TO_LINEOUT_PCM, { 0x002F, 0x0000 } },
	{ MICCO_MIC2_TO_LINEOUT,     { 0x002F, 0x0000 } },
	{ MICCO_MIC2_TO_LINEOUT_PCM, { 0x002F, 0x0000 } }
};

//Gain ID #9
#define MICCO_TXPGA_IN_GAIN_MAX        27
static MICCO_TxPGAInGainS gMICCOTxPgaInGain[MICCO_TXPGA_IN_GAIN_MAX] = {
	{ MICCO_MIC1_TO_PCM,	     { 0x0000, 0x0300 } },
	{ MICCO_MIC2_TO_PCM,	     { 0x008B, 0x0000 } },
	{ MICCO_AUX3_TO_PCM,	     { 0x008B, 0x0000 } },
	{ MICCO_AUX1_TO_PCM,	     { 0x008B, 0x0000 } },
	{ MICCO_AUX2_TO_PCM,	     { 0x008B, 0x0000 } },

	{ MICCO_I2S_DAC1_TO_PCM,     { 0x008B, 0x0000 } },
	{ MICCO_I2S_DAC2_TO_PCM,     { 0x008B, 0x0000 } },
	{ MICCO_MIC1_TO_LINEOUT_PCM, { 0x008B, 0x0000 } },
	{ MICCO_MIC2_TO_LINEOUT_PCM, { 0x008B, 0x0000 } },

	{ MICCO_AUX1_TO_MONO_PCM,    { 0x0064, 0x0400 } },
	{ MICCO_AUX1_TO_BEAR_PCM,    { 0x0064, 0x0400 } },
	{ MICCO_AUX1_TO_SPKRL_PCM,   { 0x0064, 0x0400 } },
	{ MICCO_AUX1_TO_SPKRR_PCM,   { 0x0064, 0x0400 } },
	{ MICCO_AUX1_TO_SPKR_PCM,    { 0x0064, 0x0400 } },
	{ MICCO_AUX1_TO_LINEOUT_PCM, { 0x0064, 0x0400 } },

	{ MICCO_AUX2_TO_MONO_PCM,    { 0x0064, 0x0400 } },
	{ MICCO_AUX2_TO_BEAR_PCM,    { 0x0064, 0x0400 } },
	{ MICCO_AUX2_TO_SPKRL_PCM,   { 0x0064, 0x0400 } },
	{ MICCO_AUX2_TO_SPKRR_PCM,   { 0x0064, 0x0400 } },
	{ MICCO_AUX2_TO_SPKR_PCM,    { 0x0064, 0x0400 } },
	{ MICCO_AUX2_TO_LINEOUT_PCM, { 0x0064, 0x0400 } },

	{ MICCO_AUX3_TO_MONO_PCM,    { 0x0064, 0x0400 } },
	{ MICCO_AUX3_TO_BEAR_PCM,    { 0x0064, 0x0400 } },
	{ MICCO_AUX3_TO_SPKRL_PCM,   { 0x0064, 0x0400 } },
	{ MICCO_AUX3_TO_SPKRR_PCM,   { 0x0064, 0x0400 } },
	{ MICCO_AUX3_TO_SPKR_PCM,    { 0x0064, 0x0400 } },
	{ MICCO_AUX3_TO_LINEOUT_PCM, { 0x0064, 0x0400 } }
};

//Gain ID #10
#define MICCO_TXADC_IN_GAIN_MAX        27
static MICCO_TxADCInGainS gMICCOTxAdcInGain[MICCO_TXADC_IN_GAIN_MAX] = {
	{ MICCO_MIC1_TO_PCM,	     { 0x0000, 0x0100 } },
	{ MICCO_MIC2_TO_PCM,	     { 0x0024, 0x0000 } },
	{ MICCO_AUX3_TO_PCM,	     { 0x0024, 0x0000 } },
	{ MICCO_AUX1_TO_PCM,	     { 0x0024, 0x0000 } },
	{ MICCO_AUX2_TO_PCM,	     { 0x0024, 0x0000 } },
	{ MICCO_I2S_DAC1_TO_PCM,     { 0x0024, 0x0000 } },
	{ MICCO_I2S_DAC2_TO_PCM,     { 0x0024, 0x0000 } },
	{ MICCO_MIC1_TO_LINEOUT_PCM, { 0x0024, 0x0000 } },
	{ MICCO_MIC2_TO_LINEOUT_PCM, { 0x0024, 0x0000 } },

	{ MICCO_AUX1_TO_MONO_PCM,    { 0x0024, 0x0000 } },
	{ MICCO_AUX1_TO_BEAR_PCM,    { 0x0024, 0x0000 } },
	{ MICCO_AUX1_TO_SPKRL_PCM,   { 0x0024, 0x0000 } },
	{ MICCO_AUX1_TO_SPKRR_PCM,   { 0x0024, 0x0000 } },
	{ MICCO_AUX1_TO_SPKR_PCM,    { 0x0024, 0x0000 } },
	{ MICCO_AUX1_TO_LINEOUT_PCM, { 0x0024, 0x0000 } },

	{ MICCO_AUX2_TO_MONO_PCM,    { 0x0024, 0x0000 } },
	{ MICCO_AUX2_TO_BEAR_PCM,    { 0x0024, 0x0000 } },
	{ MICCO_AUX2_TO_SPKRL_PCM,   { 0x0024, 0x0000 } },
	{ MICCO_AUX2_TO_SPKRR_PCM,   { 0x0024, 0x0000 } },
	{ MICCO_AUX2_TO_SPKR_PCM,    { 0x0024, 0x0000 } },
	{ MICCO_AUX2_TO_LINEOUT_PCM, { 0x0024, 0x0000 } },

	{ MICCO_AUX3_TO_MONO_PCM,    { 0x0024, 0x0000 } },
	{ MICCO_AUX3_TO_BEAR_PCM,    { 0x0024, 0x0000 } },
	{ MICCO_AUX3_TO_SPKRL_PCM,   { 0x0024, 0x0000 } },
	{ MICCO_AUX3_TO_SPKRR_PCM,   { 0x0024, 0x0000 } },
	{ MICCO_AUX3_TO_SPKR_PCM,    { 0x0024, 0x0000 } },
	{ MICCO_AUX3_TO_LINEOUT_PCM, { 0x0024, 0x0400 } }
};
//Gain ID #11
#define MICCO_SIDETONE_GAIN_MAX       1
static MICCO_SideToneGainS gMICCOSideToneGain[MICCO_SIDETONE_GAIN_MAX] = {
	{ MICCO_IN_TO_OUT, { 0x007A, 0xFD00 } }
};

//Gain ID #12
#define MICCO_DIGITAL_GAIN_MAX        MICCO_TOTAL_VALID_PATHS
static MICCO_DigitalGainS gMICCODigitalGain[MICCO_DIGITAL_GAIN_MAX] = {
	{ MICCO_PCM_TO_MONO,	     { 0, 0	 } },
	{ MICCO_PCM_TO_BEAR,	     { 0, 0	 } },
	{ MICCO_PCM_TO_SPKRL,	     { 0, 0	 } },
	{ MICCO_PCM_TO_SPKRR,	     { 0, 0	 } },
	{ MICCO_PCM_TO_SPKR,	     { 0, 0	 } },
	{ MICCO_PCM_TO_LINEOUT,	     { 0, 0	 } },
	{ MICCO_I2S_TO_SPKRL,	     { 0, 0	 } },
	{ MICCO_I2S_TO_SPKRR,	     { 0, 0	 } },
	{ MICCO_I2S_TO_SPKR,	     { 0, 0	 } },
	{ MICCO_I2S_TO_MONO,	     { 0, 0	 } },
	{ MICCO_I2S_TO_BEAR,	     { 0, 0	 } },
	{ MICCO_I2S_TO_LINEOUT,	     { 0, 0	 } },
	{ MICCO_I2S_TO_SPKR_MONO,    { 0, 0	 } },
	{ MICCO_I2S_TO_SPKRL_MONO,   { 0, 0	 } },
	{ MICCO_I2S_TO_SPKRR_MONO,   { 0, 0	 } },
	// { MICCO_IN_TO_OUT					, {0,0} },
	{ MICCO_MIC1_TO_PCM,	     { 0, 0xFE80 } },
	{ MICCO_MIC2_TO_PCM,	     { 0, 0	 } },
	{ MICCO_AUX3_TO_PCM,	     { 0, 0	 } },
	{ MICCO_AUX1_TO_PCM,	     { 0, 0	 } },
	{ MICCO_AUX2_TO_PCM,	     { 0, 0	 } },

	{ MICCO_PCM_LOOP,	     { 0, 0	 } },
	{ MICCO_I2S_DAC1_TO_PCM,     { 0, 0	 } },
	{ MICCO_I2S_DAC2_TO_PCM,     { 0, 0	 } },
	//{ MICCO_MIC1_TO_LINEOUT				, {0,0} },
	{ MICCO_MIC1_TO_LINEOUT_PCM, { 0, 0	 } },
	//{ MICCO_MIC2_TO_LINEOUT				, {0,0} },
	{ MICCO_MIC2_TO_LINEOUT_PCM, { 0, 0	 } },

	//{ MICCO_AUX1_TO_MONO                          , {0,0} },
	{ MICCO_AUX1_TO_MONO_PCM,    { 0, 0	 } },
	//{ MICCO_AUX1_TO_BEAR				, {0,0} },
	{ MICCO_AUX1_TO_BEAR_PCM,    { 0, 0	 } },
	//	{ MICCO_AUX1_TO_SPKRL				, {0,0} },
	{ MICCO_AUX1_TO_SPKRL_PCM,   { 0, 0	 } },
	//	{ MICCO_AUX1_TO_SPKRR				, {0,0} },
	{ MICCO_AUX1_TO_SPKRR_PCM,   { 0, 0	 } },
	//	{ MICCO_AUX1_TO_SPKR				, {0,0} },
	{ MICCO_AUX1_TO_SPKR_PCM,    { 0, 0	 } },
	//	{ MICCO_AUX1_TO_LINEOUT				, {0,0} },
	{ MICCO_AUX1_TO_LINEOUT_PCM, { 0, 0	 } },

	//	{ MICCO_AUX2_TO_MONO				, {0,0} },
	{ MICCO_AUX2_TO_MONO_PCM,    { 0, 0	 } },
	//	{ MICCO_AUX2_TO_BEAR				, {0,0} },
	{ MICCO_AUX2_TO_BEAR_PCM,    { 0, 0	 } },
	//	{ MICCO_AUX2_TO_SPKRL				, {0,0} },
	{ MICCO_AUX2_TO_SPKRL_PCM,   { 0, 0	 } },
	//	{ MICCO_AUX2_TO_SPKRR				, {0,0} },
	{ MICCO_AUX2_TO_SPKRR_PCM,   { 0, 0	 } },
	//	{ MICCO_AUX2_TO_SPKR				, {0,0} },
	{ MICCO_AUX2_TO_SPKR_PCM,    { 0, 0	 } },
	//	{ MICCO_AUX2_TO_LINEOUT		                , {0,0} },
	{ MICCO_AUX2_TO_LINEOUT_PCM, { 0, 0	 } },
	//	{ MICCO_AUX3_TO_MONO				, {0,0} },
	{ MICCO_AUX3_TO_MONO_PCM,    { 0, 0	 } },
	//	{ MICCO_AUX3_TO_BEAR		                , {0,0} },
	{ MICCO_AUX3_TO_BEAR_PCM,    { 0, 0	 } },
	//	{ MICCO_AUX3_TO_SPKRL				, {0,0} },
	{ MICCO_AUX3_TO_SPKRL_PCM,   { 0, 0	 } },
	//	{ MICCO_AUX3_TO_SPKRR				, {0,0} },
	{ MICCO_AUX3_TO_SPKRR_PCM,   { 0, 0	 } },
	//	{ MICCO_AUX3_TO_SPKR				, {0,0} },
	{ MICCO_AUX3_TO_SPKR_PCM,    { 0, 0	 } },
	//	{ MICCO_AUX3_TO_LINEOUT				, {0,0} },
	{ MICCO_AUX3_TO_LINEOUT_PCM, { 0, 0	 } }
};

static PMC_micIn_t shadowPmcMicIn = {
	INVALID_FIELD_VALUE,                                    //micAmpSel
	INVALID_FIELD_VALUE,                                    //micAmpEna
	INVALID_FIELD_VALUE
};                                                              //micAmp;

static PMC_aux_t shadowPmcAux1 = {
	INVALID_FIELD_VALUE,                                    //pgaAuxSelect
	INVALID_FIELD_VALUE,                                    //pgaAuxEna
	INVALID_FIELD_VALUE
};                                                              //pgaAuxGain

static PMC_aux_t shadowPmcAux2 = {
	INVALID_FIELD_VALUE,                                    //pgaAuxSelect
	INVALID_FIELD_VALUE,                                    //pgaAuxEna
	INVALID_FIELD_VALUE
};                                                              //pgaAuxGain

static PMC_aux_t shadowPmcAux3 = {
	INVALID_FIELD_VALUE,                                    //pgaAuxSelect
	INVALID_FIELD_VALUE,                                    //pgaAuxEna
	INVALID_FIELD_VALUE
};                                                              //pgaAuxGain

static PMC_mono_t shadowPmcMono = {
	INVALID_FIELD_VALUE,                                                                    //monoSrcSel
	INVALID_FIELD_VALUE,                                                                    //monoEna
	INVALID_FIELD_VALUE,                                                                    //monoGain
	INVALID_FIELD_VALUE,                                                                    //audioLineOutSrcSel
	INVALID_FIELD_VALUE,                                                                    //audioLineOutEna
	INVALID_FIELD_VALUE,                                                                    //audioLineOutGain
	INVALID_FIELD_VALUE,                                                                    //bearSrcSel
	INVALID_FIELD_VALUE,                                                                    //bearEna
	INVALID_FIELD_VALUE
};                                                                                              //bearGain

static PMC_stereo_t shadowPmcStereo = {
	INVALID_FIELD_VALUE,                                    //stereoEna
	INVALID_FIELD_VALUE,                                    //stereoSrcSel_ch1
	INVALID_FIELD_VALUE,                                    //stereoGain_ch1
	INVALID_FIELD_VALUE,                                    //stereoSrcSel_ch2
	INVALID_FIELD_VALUE
};                                                              //stereoGain_ch2

#if 1                                                           // without dsp gain
static PMC_pcmIn_t shadowPcmIn = {
	INVALID_FIELD_VALUE,                                    //pgaSrcSel
	INVALID_FIELD_VALUE,                                    //txPgaEna
	INVALID_FIELD_VALUE,                                    //txPga
	INVALID_FIELD_VALUE,                                    //micAmpDspGain
	INVALID_FIELD_VALUE,                                    //adcEna
	INVALID_FIELD_VALUE,                                    //adcMute
	INVALID_FIELD_VALUE,                                    //adcHpfCntrl
	INVALID_FIELD_VALUE
};                                                              //adcGain
#else // with dsp gain
static PMC_pcmIn_t shadowPcmIn = {
	INVALID_FIELD_VALUE,                                    //pgaSrcSel
	INVALID_FIELD_VALUE,                                    //txPgaEna
	INVALID_FIELD_VALUE,                                    //txPga
	INVALID_FIELD_VALUE,                                    //micAmpDspGain
	INVALID_FIELD_VALUE,                                    //adcEna
	INVALID_FIELD_VALUE,                                    //adcMute
	INVALID_FIELD_VALUE,                                    //adcHpfCntrl
	INVALID_FIELD_VALUE
};                                                              //adcGain
#endif

static PMC_pcmOut_t shadowPcmOut = {
	INVALID_FIELD_VALUE,                                                                    //vdacEna
	INVALID_FIELD_VALUE,                                                                    //vdacCntrl
	INVALID_FIELD_VALUE,                                                                    //vdacMute
	INVALID_FIELD_VALUE,                                                                    //sideToneEna
	INVALID_FIELD_VALUE,                                                                    //sideToneCntrl
	INVALID_FIELD_VALUE
};                                                                                              //pcmLoop

static PMC_hifi_t shadowHifi = {
	INVALID_FIELD_VALUE,                                    //i2sCntrl
	INVALID_FIELD_VALUE
};                                                              //hiFiDacCntrl


//-----------------------------------------------------------------------------------//
#define CLIP_VALUE(g, l, h)  {						  \
		if ((g) > (h))		\
		{					\
			(g) = (h);	\
		}			\
		else if ((g) < (l))	\
		{		    \
			(g) = (l);		\
		}					\
}



//HIGHER NUMBER => HAPPENS TO INDICATE HARDWARE LOWER EDGE IN db =
//do not change anything regarding this clipping section

#define CLIP_DIGITAL_GAIN(g)            CLIP_VALUE((g), -36, 12)        // NEED CHECK

#define CLIP_SPKRL_OUT_GAIN(g)          CLIP_VALUE((g), 0x00, 0x3F)     //Stereo CH1 PGA -60 to 15 dB
#define CLIP_SPKRR_OUT_GAIN(g)          CLIP_VALUE((g), 0x00, 0x3F)     //Stereo CH2 PGA -60 to 15 dB
#define CLIP_BEAR_OUT_GAIN(g)           CLIP_VALUE((g), 0x00, 0x3F)
#define CLIP_MONO_OUT_GAIN(g)           CLIP_VALUE((g), 0x00, 0x3F)
#define CLIP_LINE_OUT_GAIN(g)           CLIP_VALUE((g), 0x00, 0x0F)             //LineOut -24 to 15dB
#define CLIP_AUX_OUT_GAIN(g)            CLIP_VALUE((g), 0x00, 0x03)             //AUX1 0-18
#define CLIP_MIC_IN_GAIN(g)             CLIP_VALUE((g), 0x00, 0x07)             //MIC 0-30
#define CLIP_TXPGA_GAIN(g)                      CLIP_VALUE((g), 0x00, 0x0F)     //TX-PGA -18 to 24
#define CLIP_VADC_GAIN(g)                       CLIP_VALUE((g), 0x00, 0x03)     //VADC -6 to 12 dB
#define CLIP_SIDETONE_GAIN(g)           CLIP_VALUE((g), 0x00, 0x1F)             //SIDETONE	-46.5 to 0 dB

//-----------------------------------------------------------------------------------//


/* Path to Input/Output */
static MICCOPathDescriptionS const gMICCOPathDescription[MICCO_TOTAL_VALID_PATHS] =
{
	{ PCM_OUT,	  MONO_OUT_OUTPUT	  },                                    /*MICCO_PCM_TO_MONO			*/
	{ PCM_OUT,	  BEAR_OUT_OUTPUT	  },                                    /*MICCO_PCM_TO_BEAR			*/
	{ PCM_OUT,	  STEREO_CH1_OUTPUT	  },                                    /*MICCO_PCM_TO_SPKRL		*/
	{ PCM_OUT,	  STEREO_CH2_OUTPUT	  },                                    /*MICCO_PCM_TO_SPKRR        */
	{ PCM_OUT,	  STEREO_BOTH_OUTPUT	  },                                    /*MICCO_PCM_TO_SPKR			*/
	{ PCM_OUT,	  LINE_OUT_OUTPUT	  },                                    /*MICCO_PCM_TO_LINEOUT      */
	{ I2S_CH1_INPUT,  STEREO_CH1_OUTPUT	  },                                    /*MICCO_I2S_TO_SPKRL        */
	{ I2S_CH1_INPUT,  STEREO_CH2_OUTPUT	  },                                    /*MICCO_I2S_TO_SPKRR        */
	{ I2S_BOTH_INPUT, STEREO_BOTH_OUTPUT	  },                                    /*MICCO_I2S_TO_SPKR         */
	{ I2S_BOTH_INPUT, MONO_OUT_OUTPUT	  },                                    /*MICCO_I2S_TO_MONO			*/
	{ I2S_BOTH_INPUT, BEAR_OUT_OUTPUT	  },                                    /*MICCO_I2S_TO_BEAR			*/
	{ I2S_CH1_INPUT,  LINE_OUT_OUTPUT	  },                                    /*MICCO_I2S_TO_LINEOUT		*/
	{ AUX3_INPUT,	  STEREO_BOTH_MONO_OUTPUT },                                    /*MICCO_I2S_TO_SPKR_MONO	*/
	{ I2S_CH1_INPUT,  STEREO_CH1_MONO_OUTPUT  },                                    /*MICCO_I2S_TO_SPKRL_MONO	*/
	{ I2S_CH2_INPUT,  STEREO_CH2_MONO_OUTPUT  },                                    /*MICCO_I2S_TO_SPKRR_MONO	*/
	{ SIDE_TONE_IN,	  SIDE_TONE_OUT		  },                                    /*MICCO_IN_TO_OUT			*/
	{ MIC1_INPUT,	  PCM_IN		  },                                    /*MICCO_MIC1_TO_PCM			*/
	{ MIC2_INPUT,	  PCM_IN		  },                                    /*MICCO_MIC2_TO_PCM			*/
	{ AUX3_INPUT,	  PCM_IN		  },                                    /*MICCO_AUX3_TO_PCM			*/
	{ AUX1_INPUT,	  PCM_IN		  },                                    /*MICCO_AUX1_TO_PCM			*/
	{ AUX2_INPUT,	  PCM_IN		  },                                    /*MICCO_AUX2_TO_PCM			*/
	{ PCM_OUT_LOOP,	  PCM_IN_LOOP		  },                                    /*MICCO_PCM_LOOP			*/

	{ I2S_CH1_INPUT,  PCM_IN		  },                                    /*MICCO_I2S_DAC1_TO_PCM		*/
	{ I2S_CH2_INPUT,  PCM_IN		  },                                    /*MICCO_I2S_DAC2_TO_PCM		*/
	{ MIC1_INPUT,	  LINE_OUT_OUTPUT	  },                                    /*MICCO_MIC1_TO_LINEOUT		*/
	{ MIC1_INPUT,	  LINE_OUT_PCM_OUTPUT	  },                                    /*MICCO_MIC1_TO_LINEOUT_PCM	*/
	{ MIC2_INPUT,	  LINE_OUT_OUTPUT	  },                                    /*MICCO_MIC2_TO_LINEOUT		*/
	{ MIC2_INPUT,	  LINE_OUT_PCM_OUTPUT	  },                                    /*MICCO_MIC2_TO_LINEOUT_PCM	*/

	{ AUX1_INPUT,	  MONO_OUT_OUTPUT	  },                                    /*MICCO_AUX1_TO_MONO		*/
	{ AUX1_INPUT,	  MONO_OUT_PCM_OUTPUT	  },                                    /*MICCO_AUX1_TO_MONO_PCM	*/
	{ AUX1_INPUT,	  BEAR_OUT_OUTPUT	  },                                    /*MICCO_AUX1_TO_BEAR		*/
	{ AUX1_INPUT,	  BEAR_OUT_PCM_OUTPUT	  },                                    /*MICCO_AUX1_TO_BEAR_PCM	*/
	{ AUX1_INPUT,	  STEREO_CH1_OUTPUT	  },                                    /*MICCO_AUX1_TO_SPKRL		*/
	{ AUX1_INPUT,	  STEREO_CH1_PCM_OUTPUT	  },                                    /*MICCO_AUX1_TO_SPKRL_PCM	*/
	{ AUX1_INPUT,	  STEREO_CH2_OUTPUT	  },                                    /*MICCO_AUX1_TO_SPKRR		*/
	{ AUX1_INPUT,	  STEREO_CH2_PCM_OUTPUT	  },                                    /*MICCO_AUX1_TO_SPKRR_PCM	*/
	{ AUX1_INPUT,	  STEREO_BOTH_OUTPUT	  },                                    /*MICCO_AUX1_TO_SPKR		*/
	{ AUX1_INPUT,	  STEREO_BOTH_PCM_OUTPUT  },                                    /*MICCO_AUX1_TO_SPKR_PCM	*/
	{ AUX1_INPUT,	  LINE_OUT_OUTPUT	  },                                    /*MICCO_AUX1_TO_LINEOUT		*/
	{ AUX1_INPUT,	  LINE_OUT_PCM_OUTPUT	  },                                    /*MICCO_AUX1_TO_LINEOUT_PCM	*/

	{ AUX2_INPUT,	  MONO_OUT_OUTPUT	  },                                    /*MICCO_AUX2_TO_MONO		*/
	{ AUX2_INPUT,	  MONO_OUT_PCM_OUTPUT	  },                                    /*MICCO_AUX2_TO_MONO_PCM	*/
	{ AUX2_INPUT,	  BEAR_OUT_OUTPUT	  },                                    /*MICCO_AUX2_TO_BEAR		*/
	{ AUX2_INPUT,	  BEAR_OUT_PCM_OUTPUT	  },                                    /*MICCO_AUX2_TO_BEAR_PCM	*/
	{ AUX2_INPUT,	  STEREO_CH1_OUTPUT	  },                                    /*MICCO_AUX2_TO_SPKRL		*/
	{ AUX2_INPUT,	  STEREO_CH1_PCM_OUTPUT	  },                                    /*MICCO_AUX2_TO_SPKRL_PCM	*/
	{ AUX2_INPUT,	  STEREO_CH2_OUTPUT	  },                                    /*MICCO_AUX2_TO_SPKRR		*/
	{ AUX2_INPUT,	  STEREO_CH2_PCM_OUTPUT	  },                                    /*MICCO_AUX2_TO_SPKRR_PCM	*/
	{ AUX2_INPUT,	  STEREO_BOTH_OUTPUT	  },                                    /*MICCO_AUX2_TO_SPKR		*/
	{ AUX2_INPUT,	  STEREO_BOTH_PCM_OUTPUT  },                                    /*MICCO_AUX2_TO_SPKR_PCM	*/
	{ AUX2_INPUT,	  LINE_OUT_OUTPUT	  },                                    /*MICCO_AUX2_TO_LINEOUT		*/
	{ AUX2_INPUT,	  LINE_OUT_PCM_OUTPUT	  },                                    /*MICCO_AUX2_TO_LINEOUT_PCM	*/

	{ AUX3_INPUT,	  MONO_OUT_OUTPUT	  },                                    /*MICCO_AUX3_TO_MONO		*/
	{ AUX3_INPUT,	  MONO_OUT_PCM_OUTPUT	  },                                    /*MICCO_AUX3_TO_MONO_PCM	*/
	{ AUX3_INPUT,	  BEAR_OUT_OUTPUT	  },                                    /*MICCO_AUX3_TO_BEAR		*/
	{ AUX3_INPUT,	  BEAR_OUT_PCM_OUTPUT	  },                                    /*MICCO_AUX3_TO_BEAR_PCM	*/
	{ AUX3_INPUT,	  STEREO_CH1_OUTPUT	  },                                    /*MICCO_AUX3_TO_SPKRL		*/
	{ AUX3_INPUT,	  STEREO_CH1_PCM_OUTPUT	  },                                    /*MICCO_AUX3_TO_SPKRL_PCM	*/
	{ AUX3_INPUT,	  STEREO_CH2_OUTPUT	  },                                    /*MICCO_AUX3_TO_SPKRR		*/
	{ AUX3_INPUT,	  STEREO_CH2_PCM_OUTPUT	  },                                    /*MICCO_AUX3_TO_SPKRR_PCM	*/
	{ AUX3_INPUT,	  STEREO_BOTH_OUTPUT	  },                                    /*MICCO_AUX3_TO_SPKR		*/
	{ AUX3_INPUT,	  STEREO_BOTH_PCM_OUTPUT  },                                    /*MICCO_AUX3_TO_SPKR_PCM	*/
	{ AUX3_INPUT,	  LINE_OUT_OUTPUT	  },                                    /*MICCO_AUX3_TO_LINEOUT		*/
	{ AUX3_INPUT,	  LINE_OUT_PCM_OUTPUT	  }                                     /*MICCO_AUX3_TO_LINEOUT_PCM	*/
};

static const MICCO_FileGain2StructGainConverter gainConverter[MICCO_TOTAL_GAIN_IDS] =
{
	{ (void *)gMICCOSpkrLOutGain, MICCO_SPKRL_OUT_GAIN_MAX				},      /*id 0*/
	{ (void *)gMICCOSpkrROutGain, MICCO_SPKRR_OUT_GAIN_MAX				},      /*id 1*/
	{ (void *)gMICCOBearOutGain,  MICCO_BEAR_OUT_GAIN_MAX				},      /*id 2*/
	{ (void *)gMICCOMonoOutGain,  MICCO_MONO_OUT_GAIN_MAX				},      /*id 3*/
	{ (void *)gMICCOLineOutGain,  MICCO_LINE_OUT_GAIN_MAX				},      /*id 4*/
	{ (void *)gMICCOAux1InGain,   MICCO_AUX1_IN_GAIN_MAX				},      /*id 5*/
	{ (void *)gMICCOAux2InGain,   MICCO_AUX2_IN_GAIN_MAX				},      /*id 6*/
	{ (void *)gMICCOAux3InGain,   MICCO_AUX3_IN_GAIN_MAX				},      /*id 7*/
	{ (void *)gMICCOMicInGain,    MICCO_MIC_IN_GAIN_MAX				},      /*id 8*/
	{ (void *)gMICCOTxPgaInGain,  MICCO_TXPGA_IN_GAIN_MAX				},      /*id 9*/
	{ (void *)gMICCOTxAdcInGain,  MICCO_TXADC_IN_GAIN_MAX				},      /*id 10*/
	{ (void *)gMICCOSideToneGain, MICCO_SIDETONE_GAIN_MAX				},      /*id 11*/
	{ (void *)gMICCODigitalGain,  MICCO_DIGITAL_GAIN_MAX				} /*id 12*/
};

static MICCOCurrentPathS gMICCOCurrentPaths[MICCO_TOTAL_VALID_PATHS];
static MICCOPaths gMICCOPathsSortedList[MICCO_TOTAL_VALID_PATHS];
static MICCODatabaseS gMICCODatabase;

/*----------- Local defines  ------------------------------------*/

#define MICCO_GET_PATH_INPUT(fULLpATH)        (gMICCOPathDescription[fULLpATH].pathInput)

#define MICCO_GET_PATH_OUTPUT(fULLpATH)       (gMICCOPathDescription[fULLpATH].pathOutput)


#define MICCO_FIND_PATH(iDX, pATH, lIST, nUMiTEMS)   {   int i;			   \
							 for (i = 0; i < nUMiTEMS; i++)	  \
							 {			     \
								 if (lIST[i].path == pATH)										   \
									 break;											\
							 }			     \
							 iDX = (unsigned char)i;     \
}

#define MICCO_GAIN_ZERO_CLIP_8BIT(gAIN)                   (((gAIN) < 0) ? 0 : (unsigned char)(gAIN))
#define MICCO_GAIN_ZERO_CLIP_8BIT_SIGNED(gAIN)            (((gAIN) < 0) ? 0 : ((signed char)(gAIN)))
#define MICCO_DIGITAL_GAIN_CLIP(gAIN)                     (   ((gAIN) < (-36)) ? -36 : \
							      (((gAIN) > ( 12)) ?  12 :	\
							       ((signed char)(gAIN))) )


/******************************************************************************/

/* Bit Control functions */
static unsigned char MICCOGetPCMvdacCntrl(MICCOPaths path, ACM_AudioVolume volume)
{
	return MICCO_PCM_VDAC_CNTRL_DEFAULT;
}

static unsigned char MICCOGetSideToneCntrl(MICCOPaths path, ACM_AudioVolume volume)
{
	return MICCO_PCM_SIDETONE_CNTRL_DEFAULT;
}

static unsigned char MICCOGetI2SCntrl(MICCOPaths path, ACM_AudioVolume volume)
{
	unsigned char i2sCntrl;
	unsigned char rate;

	switch (gMICCODatabase.playBitRate)
	{
	case MICCO_BIT_RATE_8_KHZ:
		//ASSERT(miccoIsClkMaster == FALSE);
		rate = 0x00;
		break;

	case MICCO_BIT_RATE_11_025_KHZ:
		//ASSERT(miccoIsClkMaster == FALSE);
		rate = 0x01;
		break;

	case MICCO_BIT_RATE_12_KHZ:
		// ASSERT(miccoIsClkMaster == FALSE);
		rate = 0x02;
		break;

	case MICCO_BIT_RATE_16_KHZ:
		if (miccoIsClkMaster)
			rate = 0x00;
		else
			rate = 0x03;
		break;

	case MICCO_BIT_RATE_22_05_KHZ:
		if (miccoIsClkMaster)
			rate = 0x01;
		else
			rate = 0x04;
		break;

	case MICCO_BIT_RATE_24_KHZ:
		if (miccoIsClkMaster)
			rate = 0x02;
		else
			rate = 0x05;
		break;

	case MICCO_BIT_RATE_32_KHZ:
		if (miccoIsClkMaster)
			rate = 0x03;
		else
			rate = 0x06;
		break;

	case MICCO_BIT_RATE_44_1_KHZ:
		if (miccoIsClkMaster)
			rate = 0x04;
		else
			rate = 0x07;
		break;

	case MICCO_BIT_RATE_48_KHZ:
		if (miccoIsClkMaster)
			rate = 0x05;
		else
			rate = 0xF;
		break;

	case MICCO_BIT_RATE_OFF:
		rate = 0x00;
		break;

	default:
		//ASSERT(FALSE);
		break;
	}

	i2sCntrl = miccoIsClkMaster ? I2S_CTRL_VAL_MICCO_CLK_MASTER : I2S_CTRL_VAL_MICCO_CLK_SLAVE;

	i2sCntrl |= rate;     /* the rate will actually updated when calling MiccoCodecSetSampleRate() */

	return (i2sCntrl);
}

static unsigned char MICCOGetI2SHiFiDacCntrl(MICCOPaths path, ACM_AudioVolume volume)
{
	return (MICCO_I2S_HIFI_DAC_CNTRL_DEFAULT | 0x80);  /* Always Enable HiFi DAC & PLL */
}

static unsigned char MICCOGetAdcHpfCntrl(MICCOPaths path, ACM_AudioVolume volume)
{

	return MICCO_ADC_HPF_CNTRL_DEFAULT;
}

///////////////////////////* Gain calculation functions *////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// gain ID #0
static unsigned char MICCOGetStereoCh1PgaGain(MICCOPaths path, ACM_AudioVolume volume)
{
	unsigned char pathIdx;
	int gain;

	MICCO_FIND_PATH(pathIdx, path, gMICCOSpkrLOutGain, MICCO_SPKRL_OUT_GAIN_MAX);

	gain = MICCO_GAIN_CALC(volume, gMICCOSpkrLOutGain[pathIdx].pga);

	return MICCO_GAIN_ZERO_CLIP_8BIT(gain);
}

// gain ID #1
static unsigned char MICCOGetStereoCh2PgaGain(MICCOPaths path, ACM_AudioVolume volume)
{
	unsigned char pathIdx;
	int gain;

	MICCO_FIND_PATH(pathIdx, path, gMICCOSpkrROutGain, MICCO_SPKRR_OUT_GAIN_MAX);

	gain = MICCO_GAIN_CALC(volume, gMICCOSpkrROutGain[pathIdx].pga);
	gain = MICCO_GAIN_ZERO_CLIP_8BIT(gain);

	if (miccoChipID >= 0x20)        /* MICCO_C0_ID, MICCO_ID_C1_DB, MICCO_ID_C1_EA or newer */
	{
		gain |= 0x80;           /* Enable ch2 (in B0 it is being done by stereoGain_ch1 bit #7) */
	}

	return gain;
}

// gain ID #2
static unsigned char MICCOGetBearPgaGain(MICCOPaths path, ACM_AudioVolume volume)
{
	unsigned char pathIdx;
	int gain;

	MICCO_FIND_PATH(pathIdx, path, gMICCOBearOutGain, MICCO_LINE_OUT_GAIN_MAX);

	gain = MICCO_GAIN_CALC(volume, gMICCOBearOutGain[pathIdx].pga);

	return MICCO_GAIN_ZERO_CLIP_8BIT(gain);
}

// gain ID #3
static unsigned char MICCOGetMonoPgaGain(MICCOPaths path, ACM_AudioVolume volume)
{
	unsigned char pathIdx;
	int gain;

	MICCO_FIND_PATH(pathIdx, path, gMICCOMonoOutGain, MICCO_MONO_OUT_GAIN_MAX);

	gain = MICCO_GAIN_CALC(volume, gMICCOMonoOutGain[pathIdx].pga);

	return MICCO_GAIN_ZERO_CLIP_8BIT(gain);
}

// gain ID #4
static unsigned char MICCOGetLineOutPgaGain(MICCOPaths path, ACM_AudioVolume volume)
{
	unsigned char pathIdx;
	int gain;

	MICCO_FIND_PATH(pathIdx, path, gMICCOLineOutGain, MICCO_LINE_OUT_GAIN_MAX);

	gain = MICCO_GAIN_CALC(volume, gMICCOLineOutGain[pathIdx].pga);

	return MICCO_GAIN_ZERO_CLIP_8BIT(gain);
}


/* gain ID #5 - Aux1 */
static unsigned char MICCOGetAux1PgaGain(MICCOPaths path, ACM_AudioVolume volume)
{
	unsigned char pathIdx;
	int gain;

	MICCO_FIND_PATH(pathIdx, path, gMICCOAux1InGain, MICCO_AUX1_IN_GAIN_MAX);

	gain = MICCO_GAIN_CALC(volume, gMICCOAux1InGain[pathIdx].mic);

	return MICCO_GAIN_ZERO_CLIP_8BIT(gain);
}

/* gain ID #6 - Aux2 */
static unsigned char MICCOGetAux2PgaGain(MICCOPaths path, ACM_AudioVolume volume)
{
	unsigned char pathIdx;
	int gain;

	MICCO_FIND_PATH(pathIdx, path, gMICCOAux2InGain, MICCO_AUX2_IN_GAIN_MAX);

	gain = MICCO_GAIN_CALC(volume, gMICCOAux2InGain[pathIdx].mic);

	return MICCO_GAIN_ZERO_CLIP_8BIT(gain);
}

/* gain ID #7 - Aux3 */
static unsigned char MICCOGetAux3PgaGain(MICCOPaths path, ACM_AudioVolume volume)
{
	unsigned char pathIdx;
	int gain;

	MICCO_FIND_PATH(pathIdx, path, gMICCOAux3InGain, MICCO_AUX3_IN_GAIN_MAX);

	gain = MICCO_GAIN_CALC(volume, gMICCOAux3InGain[pathIdx].mic);

	return MICCO_GAIN_ZERO_CLIP_8BIT(gain);
}

// gain ID #8
static unsigned char MICCOGetMicGain(MICCOPaths path, ACM_AudioVolume volume)
{
	unsigned char pathIdx;
	int gain;

	MICCO_FIND_PATH(pathIdx, path, gMICCOMicInGain, MICCO_MIC_IN_GAIN_MAX);

	gain = MICCO_GAIN_CALC(volume, gMICCOMicInGain[pathIdx].mic);

	return MICCO_GAIN_ZERO_CLIP_8BIT(gain);
}


/* for 8 bit sampling we need to attenuate 24 dB form Gain 8, gain 9 and gain 10                        */
/* first we will attenuate the max we can from the adcGain (gain 10) and then TxPga (gain 9)*/
/* and the rest will be attenuated from the MicPga (gain8)									*/
/* the 'att' value is the value to substract from gain 8 , (calcuated at gain 9 procedure)	*/

/*
   // gain ID #8 (8 Bit)
   static unsigned char MICCOGetMicGain8Bit(MICCOPaths path, ACM_AudioVolume volume, int )
   {
    unsigned char   pathIdx;
    int             gain;

    MICCO_FIND_PATH(pathIdx, path, gMICCOMicInGain, MICCO_MIC_IN_GAIN_MAX);

    gain = MICCO_GAIN_CALC(volume, gMICCOMicInGain[pathIdx].mic);

	if (gain > 5)
	{
		gain = 5 - (att_div3 >>1);
	}
	else
	{
		gain = gain - (att_div3 >>1);		// the att value is NOT a multipication of 6, but this is a miss acuracy we have to tolerate
	}

    return MICCO_GAIN_ZERO_CLIP_8BIT(gain);
   }


 */

// gain ID #9
static unsigned char MICCOGetPcmPgaGain(MICCOPaths path, ACM_AudioVolume volume)
{
	unsigned char pathIdx;
	int gain;

	MICCO_FIND_PATH(pathIdx, path, gMICCOTxPgaInGain, MICCO_TXPGA_IN_GAIN_MAX);

	gain = MICCO_GAIN_CALC(volume, gMICCOTxPgaInGain[pathIdx].pga);

	return MICCO_GAIN_ZERO_CLIP_8BIT(gain);
}

/*
   // gain ID #9 (8 Bit)
   static unsigned char MICCOGetPcmPgaGain8Bit(MICCOPaths path, ACM_AudioVolume volume, int* att_div3)
   {
    unsigned char   pathIdx;
    int             gain, AnalogGain_div3;

    MICCO_FIND_PATH(pathIdx, path, gMICCOTxPgaInGain, MICCO_TXPGA_IN_GAIN_MAX);

    gain = MICCO_GAIN_CALC(volume, gMICCOTxPgaInGain[pathIdx].pga);

	// TxPGA gain in dB:
	CLIP_TXPGA_GAIN(gain);


	if (gain != 15)
	{
		//AnalogGain = ((gain * (3)) + (-18));   //relating gain Register value to Real DB
		AnalogGain_div3 = (gain+ (-6));			// helping int

		if (AnalogGain_div3 >= *att_div3 - 6)	// in this case we wont need the MicPga to att
		{
			gain = gain - *att_div3;	// att possible values are [6/12/18/24] all divided by 3
 *att_div3 = 0;
		}
		else
		{
			gain = 0;	// the minmum possible
			// to find out how much we attenuated, and how much is missing to complete 24 dB:
 *att_div3 = *att_div3 - (6 + AnalogGain_div3);
		}
	}
	else
	{
		// in case of 24dB gain ( 14 value = 15 value in gain)
		gain = 14 - *att_div3;	// att possible values are [6/12/18/24] all divided by 3
 *att_div3 = 0;
	}

    return MICCO_GAIN_ZERO_CLIP_8BIT(gain);

   }
 */

// gain ID #10
static unsigned char MICCOGetAdcPgaGain(MICCOPaths path, ACM_AudioVolume volume)
{
	unsigned char pathIdx;
	int gain;

	MICCO_FIND_PATH(pathIdx, path, gMICCOTxAdcInGain, MICCO_TXADC_IN_GAIN_MAX);

	gain = MICCO_GAIN_CALC(volume, gMICCOTxAdcInGain[pathIdx].pga);

	return MICCO_GAIN_ZERO_CLIP_8BIT(gain);
}

/*
   // gain ID #10 (8 Bit)
   static unsigned char MICCOGetAdcPgaGain8Bit(MICCOPaths path, ACM_AudioVolume volume, int* att_div3)
   {
    unsigned char   pathIdx;
    int             gain,AnalogGain,AnalogGain_div3;

    MICCO_FIND_PATH(pathIdx, path, gMICCOTxAdcInGain, MICCO_TXADC_IN_GAIN_MAX);

    gain = MICCO_GAIN_CALC(volume, gMICCOTxAdcInGain[pathIdx].pga);

	CLIP_VADC_GAIN(gain);
	AnalogGain = (gain * (-6)) + 12;		// relating gain Register value to Real DB
	AnalogGain_div3 = (gain* (-2)) +4;		// helping int for calcultaions

	gain = 0x3;								// the minimal value
	// to find out how much we attenuated, and how much is missing to complete 24 dB:
   //	att = 24 + (-6 - AnalogGain);			// att value is in the range of [6/12/18/24]
 *att_div3 = 8 + (-2 - AnalogGain_div3);

    return MICCO_GAIN_ZERO_CLIP_8BIT(gain);
   }

 */
//Side Tone Gain ID #11
//static unsigned char MICCOGetSideToneGain(MICCOPaths path, ACM_AudioVolume volume, unsigned char *sideToneEna)
static unsigned char MICCOGetSideToneGain(MICCOPaths path, ACM_AudioVolume volume)
{
	unsigned char pathIdx;
	int gain;

	MICCO_FIND_PATH(pathIdx, path, gMICCOSideToneGain, MICCO_SIDETONE_GAIN_MAX);

//    if(gMICCOSideToneGain[pathIdx].gain.slope == INVALID_INDEX)
//    {
//        *sideToneEna = FALSE;
//        return 0;
//   }

//    *sideToneEna = TRUE;
	gain         = MICCO_GAIN_CALC(volume, gMICCOSideToneGain[pathIdx].gain);

	return MICCO_GAIN_ZERO_CLIP_8BIT(gain);
}


//Digital Gain ID #12
static ACM_DigitalGain MICCOGetDigitalGain(unsigned char path, ACM_AudioVolume volume)
{
	unsigned char pathIdx;
	int gain;

	MICCO_FIND_PATH(pathIdx, path, gMICCODigitalGain, MICCO_DIGITAL_GAIN_MAX);

	gain = MICCO_GAIN_CALC(volume, gMICCODigitalGain[pathIdx].gain);

	DBGMSG("AUDIO_MICCO_DigitalGain (%d)", (ACM_DigitalGain)MICCO_DIGITAL_GAIN_CLIP(gain) );

	return (ACM_DigitalGain)MICCO_DIGITAL_GAIN_CLIP(gain);
}


/* Local services */
static pgaSrcSel_t MICCOInput2PCMSourceSel(MICCOPathInput pathInput)
{
	pgaSrcSel_t srcSel = NO_SOURCE;

	switch (pathInput)
	{
	case AUX1_INPUT:
		srcSel = AUXI1;
		break;

	case AUX2_INPUT:
		srcSel = AUXI2;
		break;

	case AUX3_INPUT:
		srcSel = AUXI3;
		break;
	case MIC1_INPUT:
	case MIC2_INPUT:
		srcSel = MIC_DIFF;
		break;
	case I2S_CH1_INPUT:
		srcSel = I2S_CH1;
		break;
	case I2S_CH2_INPUT:
		srcSel = I2S_CH2;
		break;
	}
	return srcSel;
}
static srcSel_t MICCOInput2SourceSel(MICCOPathInput pathInput)
{
	srcSel_t srcSel = 0x00;

	switch (pathInput)
	{
	case AUX1_INPUT:
		srcSel = AUXI1_SRC;
		break;

	case AUX2_INPUT:
		srcSel = AUXI2_SRC;
		break;

	case AUX3_INPUT:
		srcSel = AUXI3_SRC;
		break;

	case PCM_OUT:
		srcSel = PCM_DAC_SRC;
		break;

	case I2S_CH1_INPUT:
		srcSel = I2S_CH1_DAC_SRC;
		break;

	case I2S_CH2_INPUT:
		srcSel = I2S_CH2_DAC_SRC;
		break;

	case I2S_BOTH_INPUT:
		srcSel = I2S_CH1_DAC_SRC | I2S_CH2_DAC_SRC;
		break;
	case MIC1_INPUT:                                                // special case: for loop Mic1 to LineOut Amp
		srcSel = I2S_CH2_DAC_SRC;
		break;
	}
	return srcSel;
}

//hardware update
static void MICCOUpdateHardware(void)
{
	int sortedPath;
//	int				att_div3;
	// Initialization
	PMC_micIn_t pmcMicIn    = {
		MIC1_DIFF,                                                                              //micAmpSel
		FALSE,                                                                                  //micAmpEna
		0
	};                                                                                              //micAmp;
	PMC_aux_t pmcAux1     = {
		AUXI1_IN,                                                                               //pgaAuxSelect
		FALSE,                                                                                  //pgaAuxEna
		0
	};                                                                                              //pgaAuxGain
	PMC_aux_t pmcAux2     = {
		AUXI2_IN,                                                                               //pgaAuxSelect
		FALSE,                                                                                  //pgaAuxEna
		0
	};                                                                                              //pgaAuxGain
	PMC_aux_t pmcAux3     = {
		AUXI3_IN,                                                                               //pgaAuxSelect
		FALSE,                                                                                  //pgaAuxEna
		0
	};                                                                                              //pgaAuxGain
	PMC_mono_t pmcMono     = {
		0x00,                                                                                   //monoSrcSel
		FALSE,                                                                                  //monoEna
		0x3f,                                                                                   //monoGain
		0x0,                                                                                    //audioLineOutSrcSel
		FALSE,                                                                                  //audioLineOutEna
		0x3f,                                                                                   //audioLineOutGain
		0x0,                                                                                    //bearSrcSel
		FALSE,                                                                                  //bearEna
		0x3f
	};                                                                                              //bearGain
	PMC_stereo_t pmcStereo   = {
		FALSE,                                                                                  //stereoEna
		0x00,                                                                                   //stereoSrcSel_ch1
		0x3F,                                                                                   //stereoGain_ch1
		0x00,                                                                                   //stereoSrcSel_ch2
		0x3F
	};                                                                                              //stereoGain_ch2

#if 1                                                                                                   // without dsp gain
	PMC_pcmIn_t pcmIn       = {
		0,                                                                                      //pgaSrcSel
		FALSE,                                                                                  //txPgaEna
		0,                                                                                      //txPga
		FALSE,                                                                                  //micAmpDspGain
		FALSE,                                                                                  //adcEna
		FALSE,                                                                                  //adcMute
		0,                                                                                      //adcHpfCntrl
		0
	};                                                                                              //adcGain
#else // with dsp gain
	PMC_pcmIn_t pcmIn       = {
		AUXI1,                                  //pgaSrcSel
		FALSE,                                  //txPgaEna
		0,                                      //txPga
		FALSE,                                  //micAmpDspGain
		TRUE,                                   //adcEna
		FALSE,                                  //adcMute
		0,                                      //adcHpfCntrl
		0
	};                                              //adcGain
#endif
	PMC_pcmOut_t pcmOut      = {
		FALSE,                                                                                          //vdacEna
		0,                                                                                              //vdacCntrl
		FALSE,                                                                                          //vdacMute
		FALSE,                                                                                          //sideToneEna
		0,                                                                                              //sideToneCntrl
		0
	};                                                                                                      //pcmLoop

	PMC_hifi_t hifi;

	/* Set initial value for the HiFi - used also when no HiFi activity present (for disable) */
	hifi.i2sCntrl = miccoIsClkMaster ? I2S_CTRL_VAL_MICCO_CLK_MASTER : I2S_CTRL_VAL_MICCO_CLK_SLAVE;

	if (miccoChipID >= 0x30 /* MICCO_ID_C1_EA or newer */)
		hifi.hiFiDacCntrl = MICCO_I2S_HIFI_DAC_CNTRL_DEFAULT;
	else
		hifi.hiFiDacCntrl = (MICCO_I2S_HIFI_DAC_CNTRL_DEFAULT | 0x80);  /* Always Enable HiFi DAC & PLL */

	//loop through all current enabled paths
	for (sortedPath = 0; sortedPath < gMICCODatabase.totalEnables; sortedPath++)
	{
		MICCOPaths path   = gMICCOPathsSortedList[sortedPath];
		unsigned char volume = gMICCOCurrentPaths[path].volume;
		MICCOPathInput pathInput;
		MICCOPathOutput pathOutput;

		pathInput  = MICCO_GET_PATH_INPUT(gMICCOPathsSortedList[sortedPath]);
		pathOutput = MICCO_GET_PATH_OUTPUT(gMICCOPathsSortedList[sortedPath]);

		DBGMSG("AUDIO_MICCO_MICCOUpdateHardware(p=%d,i=%d,o=%d)", (UINT16)path, (UINT16)pathInput, (UINT16)pathOutput);

		switch (pathOutput)
		{
		case LINE_OUT_OUTPUT:
			pmcMono.audioLineOutSrcSel  |= MICCOInput2SourceSel(pathInput);
			pmcMono.audioLineOutEna     = TRUE;
			pmcMono.audioLineOutGain    = MICCOGetLineOutPgaGain(path, volume);

			// adding the sideTone Gain, the volume of output path impacts the sidetone level.
			//pcmOut.sideToneGain         = MICCOGetSideToneGain(path, volume);
			break;

		case MONO_OUT_OUTPUT:
			pmcMono.monoSrcSel          |= MICCOInput2SourceSel(pathInput);
			pmcMono.monoEna             = TRUE;
			pmcMono.monoGain            = MICCOGetMonoPgaGain(path, volume);

			// adding the sideTone Gain, the volume of output path impacts the sidetone level.
			//pcmOut.sideToneGain         = MICCOGetSideToneGain(path, volume);
			break;

		case BEAR_OUT_OUTPUT:
			pmcMono.bearSrcSel          |= MICCOInput2SourceSel(pathInput);
			pmcMono.bearEna             = TRUE;
			pmcMono.bearGain            = MICCOGetBearPgaGain(path, volume);

			// adding the sideTone Gain, the volume of output path impacts the sidetone level.
			//pcmOut.sideToneGain         = MICCOGetSideToneGain(path, volume);
			break;

		case PCM_IN:
			pcmIn.pgaSrcSel             |= MICCOInput2PCMSourceSel(pathInput);
			pcmIn.txPgaEna                          = TRUE;
			pcmIn.txPga                                     = MICCOGetPcmPgaGain(path, volume);
			pcmIn.adcEna                = TRUE;

			if ( gMICCOCurrentPaths[path].muted && (pathInput != I2S_CH1_INPUT) && (pathInput != I2S_CH2_INPUT) )
				pcmIn.adcMute           = TRUE;
			else
				pcmIn.adcMute           = FALSE;

			pcmIn.adcHpfCntrl           = MICCOGetAdcHpfCntrl(path, volume);
			pcmIn.adcGain               = MICCOGetAdcPgaGain(path, volume);

			// adding the sideTone Gain, the volume of output path impacts the sidetone level.
			//pcmOut.sideToneGain         = MICCOGetSideToneGain(path, volume);
			break;

		case PCM_IN_LOOP:
			break;

		case STEREO_CH1_OUTPUT:
			pmcStereo.stereoEna         = TRUE;
			pmcStereo.stereoSrcSel_ch1  |= MICCOInput2SourceSel(pathInput);
			pmcStereo.stereoGain_ch1    = MICCOGetStereoCh1PgaGain(path, volume);

			// adding the sideTone Gain, the volume of output path impacts the sidetone level.
			//pcmOut.sideToneGain         = MICCOGetSideToneGain(path, volume);
			break;

		case STEREO_CH2_OUTPUT:
			/* Setting stereoEna to TRUE will cause enable of ch1, by setting stereoGain_ch1 bit #7 by PMCCodecStereoOutCntrl();
			 * it causes Enable ch1+ch2 in B0, but only ch1 in C0 or newer */
			pmcStereo.stereoEna         = TRUE;
			pmcStereo.stereoSrcSel_ch2  |= MICCOInput2SourceSel(pathInput);
			pmcStereo.stereoGain_ch2    = MICCOGetStereoCh2PgaGain(path, volume);

			// adding the sideTone Gain, the volume of output path impacts the sidetone level.
			//pcmOut.sideToneGain         = MICCOGetSideToneGain(path, volume);
			break;

		case STEREO_BOTH_OUTPUT:
			/* Setting stereoEna to TRUE will cause enable of ch1, by setting stereoGain_ch1 bit #7 by PMCCodecStereoOutCntrl();
			 * it causes Enable ch1+ch2 in B0, but only ch1 in C0 or newer */
			pmcStereo.stereoEna         = TRUE;

			if (pathInput == I2S_BOTH_INPUT)
			{          /* Use I2S_CH1_INPUT for RIGHT and I2S_CH2_INPUT for LEFT */
				pmcStereo.stereoSrcSel_ch1  |= MICCOInput2SourceSel(I2S_CH1_INPUT);
				pmcStereo.stereoSrcSel_ch2  |= MICCOInput2SourceSel(I2S_CH2_INPUT);
			}
			else
			{
				pmcStereo.stereoSrcSel_ch1  |= MICCOInput2SourceSel(pathInput);
				pmcStereo.stereoSrcSel_ch2  |= MICCOInput2SourceSel(pathInput);
			}

			pmcStereo.stereoGain_ch1    = MICCOGetStereoCh1PgaGain(path, volume);
			pmcStereo.stereoGain_ch2    = MICCOGetStereoCh2PgaGain(path, volume);

			// adding the sideTone Gain, the volume of output path impacts the sidetone level.
			//pcmOut.sideToneGain         = MICCOGetSideToneGain(path, volume);
			break;

		case STEREO_BOTH_MONO_OUTPUT:
			/* Setting stereoEna to TRUE will cause enable of ch1, by setting stereoGain_ch1 bit #7 by PMCCodecStereoOutCntrl();
			 * it causes Enable ch1+ch2 in B0, but only ch1 in C0 or newer */
			pmcStereo.stereoEna         = TRUE;

			pmcStereo.stereoSrcSel_ch1  |= MICCOInput2SourceSel(pathInput);
			pmcStereo.stereoGain_ch1    = MICCOGetStereoCh1PgaGain(path, volume);

			pmcStereo.stereoSrcSel_ch2  |= MICCOInput2SourceSel(pathInput);
			pmcStereo.stereoGain_ch2    = MICCOGetStereoCh2PgaGain(path, volume);

			pmcMono.monoSrcSel          |= MICCOInput2SourceSel(pathInput);
			pmcMono.monoEna             = TRUE;
			pmcMono.monoGain            = MICCOGetMonoPgaGain(path, volume);

			// adding the sideTone Gain, the volume of output path impacts the sidetone level.
			//pcmOut.sideToneGain         = MICCOGetSideToneGain(path, volume);
			break;

		case STEREO_CH1_MONO_OUTPUT:
			pmcStereo.stereoEna         = TRUE;

			pmcStereo.stereoSrcSel_ch1  |= MICCOInput2SourceSel(pathInput);
			pmcStereo.stereoGain_ch1    = MICCOGetStereoCh1PgaGain(path, volume);

			pmcMono.monoSrcSel          |= MICCOInput2SourceSel(pathInput);
			pmcMono.monoEna             = TRUE;
			pmcMono.monoGain            = MICCOGetMonoPgaGain(path, volume);

			// adding the sideTone Gain, the volume of output path impacts the sidetone level.
			//pcmOut.sideToneGain         = MICCOGetSideToneGain(path, volume);
			break;
		case STEREO_CH2_MONO_OUTPUT:
			pmcStereo.stereoEna         = TRUE;

			pmcStereo.stereoSrcSel_ch2  |= MICCOInput2SourceSel(pathInput);
			pmcStereo.stereoGain_ch2    = MICCOGetStereoCh2PgaGain(path, volume);

			pmcMono.monoSrcSel          |= MICCOInput2SourceSel(pathInput);
			pmcMono.monoEna             = TRUE;
			pmcMono.monoGain            = MICCOGetMonoPgaGain(path, volume);

			// adding the sideTone Gain, the volume of output path impacts the sidetone level.
			//pcmOut.sideToneGain         = MICCOGetSideToneGain(path, volume);
			break;

		case SIDE_TONE_OUT:
			break;

		case MONO_OUT_PCM_OUTPUT:
			pmcMono.monoSrcSel          |= MICCOInput2SourceSel(pathInput);
			pmcMono.monoEna             = TRUE;
			pmcMono.monoGain            = MICCOGetMonoPgaGain(path, volume);

			pcmIn.pgaSrcSel             |= MICCOInput2PCMSourceSel(pathInput);
			pcmIn.txPgaEna                          = TRUE;
			pcmIn.txPga                                     = MICCOGetPcmPgaGain(path, volume);
			pcmIn.adcEna                = TRUE;

			if ( gMICCOCurrentPaths[path].muted && (pathInput != I2S_CH1_INPUT) && (pathInput != I2S_CH2_INPUT) )
				pcmIn.adcMute           = TRUE;
			else
				pcmIn.adcMute           = FALSE;

			pcmIn.adcHpfCntrl           = MICCOGetAdcHpfCntrl(path, volume);
			pcmIn.adcGain               = MICCOGetAdcPgaGain(path, volume);

			// adding the sideTone Gain, the volume of output path impacts the sidetone level.
			//pcmOut.sideToneGain         = MICCOGetSideToneGain(path, volume);
			break;

		case BEAR_OUT_PCM_OUTPUT:
			pmcMono.bearSrcSel          |= MICCOInput2SourceSel(pathInput);
			pmcMono.bearEna             = TRUE;
			pmcMono.bearGain            = MICCOGetBearPgaGain(path, volume);

			pcmIn.pgaSrcSel             |= MICCOInput2PCMSourceSel(pathInput);
			pcmIn.txPgaEna                          = TRUE;
			pcmIn.txPga                                     = MICCOGetPcmPgaGain(path, volume);
			pcmIn.adcEna                = TRUE;

			if ( gMICCOCurrentPaths[path].muted && (pathInput != I2S_CH1_INPUT) && (pathInput != I2S_CH2_INPUT) )
				pcmIn.adcMute           = TRUE;
			else
				pcmIn.adcMute           = FALSE;

			pcmIn.adcHpfCntrl           = MICCOGetAdcHpfCntrl(path, volume);
			pcmIn.adcGain               = MICCOGetAdcPgaGain(path, volume);

			// adding the sideTone Gain, the volume of output path impacts the sidetone level.
			//pcmOut.sideToneGain         = MICCOGetSideToneGain(path, volume);
			break;

		case STEREO_CH1_PCM_OUTPUT:
			pmcStereo.stereoEna         = TRUE;
			pmcStereo.stereoSrcSel_ch1  |= MICCOInput2SourceSel(pathInput);
			pmcStereo.stereoGain_ch1    = MICCOGetStereoCh1PgaGain(path, volume);

			pcmIn.pgaSrcSel             |= MICCOInput2PCMSourceSel(pathInput);
			pcmIn.txPgaEna                          = TRUE;
			pcmIn.txPga                                     = MICCOGetPcmPgaGain(path, volume);
			pcmIn.adcEna                = TRUE;

			if ( gMICCOCurrentPaths[path].muted && (pathInput != I2S_CH1_INPUT) && (pathInput != I2S_CH2_INPUT) )
				pcmIn.adcMute           = TRUE;
			else
				pcmIn.adcMute           = FALSE;

			pcmIn.adcHpfCntrl           = MICCOGetAdcHpfCntrl(path, volume);
			pcmIn.adcGain               = MICCOGetAdcPgaGain(path, volume);

			// adding the sideTone Gain, the volume of output path impacts the sidetone level.
			//pcmOut.sideToneGain         = MICCOGetSideToneGain(path, volume);
			break;

		case STEREO_CH2_PCM_OUTPUT:
			pmcStereo.stereoEna         = TRUE;
			pmcStereo.stereoSrcSel_ch2  |= MICCOInput2SourceSel(pathInput);
			pmcStereo.stereoGain_ch2    = MICCOGetStereoCh2PgaGain(path, volume);

			pcmIn.pgaSrcSel             |= MICCOInput2PCMSourceSel(pathInput);
			pcmIn.txPgaEna                          = TRUE;
			pcmIn.txPga                                     = MICCOGetPcmPgaGain(path, volume);
			pcmIn.adcEna                = TRUE;

			if ( gMICCOCurrentPaths[path].muted && (pathInput != I2S_CH1_INPUT) && (pathInput != I2S_CH2_INPUT) )
				pcmIn.adcMute           = TRUE;
			else
				pcmIn.adcMute           = FALSE;

			pcmIn.adcHpfCntrl           = MICCOGetAdcHpfCntrl(path, volume);
			pcmIn.adcGain               = MICCOGetAdcPgaGain(path, volume);

			// adding the sideTone Gain, the volume of output path impacts the sidetone level.
			//pcmOut.sideToneGain         = MICCOGetSideToneGain(path, volume);
			break;

		case STEREO_BOTH_PCM_OUTPUT:
			/* Setting stereoEna to TRUE will cause enable of ch1, by setting stereoGain_ch1 bit #7 by PMCCodecStereoOutCntrl();
			 * it causes Enable ch1+ch2 in B0, but only ch1 in C0 or newer */
			pmcStereo.stereoEna         = TRUE;

			pmcStereo.stereoSrcSel_ch1  |= MICCOInput2SourceSel(pathInput);
			pmcStereo.stereoGain_ch1    = MICCOGetStereoCh1PgaGain(path, volume);

			pmcStereo.stereoSrcSel_ch2  |= MICCOInput2SourceSel(pathInput);
			pmcStereo.stereoGain_ch2    = MICCOGetStereoCh2PgaGain(path, volume);

			pcmIn.pgaSrcSel             |= MICCOInput2PCMSourceSel(pathInput);
			pcmIn.txPgaEna                          = TRUE;
			pcmIn.txPga                                     = MICCOGetPcmPgaGain(path, volume);
			pcmIn.adcEna                = TRUE;

			if ( gMICCOCurrentPaths[path].muted && (pathInput != I2S_CH1_INPUT) && (pathInput != I2S_CH2_INPUT) )
				pcmIn.adcMute           = TRUE;
			else
				pcmIn.adcMute           = FALSE;

			pcmIn.adcHpfCntrl           = MICCOGetAdcHpfCntrl(path, volume);
			pcmIn.adcGain               = MICCOGetAdcPgaGain(path, volume);

			// adding the sideTone Gain, the volume of output path impacts the sidetone level.
			//pcmOut.sideToneGain         = MICCOGetSideToneGain(path, volume);
			break;

		case LINE_OUT_PCM_OUTPUT:
			pmcMono.audioLineOutSrcSel  |= MICCOInput2SourceSel(pathInput);
			pmcMono.audioLineOutEna     = TRUE;
			pmcMono.audioLineOutGain    = MICCOGetLineOutPgaGain(path, volume);

			pcmIn.pgaSrcSel             |= MICCOInput2PCMSourceSel(pathInput);
			pcmIn.txPgaEna                          = TRUE;
			pcmIn.txPga                                     = MICCOGetPcmPgaGain(path, volume);
			pcmIn.adcEna                = TRUE;

			if ( gMICCOCurrentPaths[path].muted && (pathInput != I2S_CH1_INPUT) && (pathInput != I2S_CH2_INPUT) )
				pcmIn.adcMute           = TRUE;
			else
				pcmIn.adcMute           = FALSE;

			pcmIn.adcHpfCntrl           = MICCOGetAdcHpfCntrl(path, volume);
			pcmIn.adcGain               = MICCOGetAdcPgaGain(path, volume);

			// adding the sideTone Gain, the volume of output path impacts the sidetone level.
			//pcmOut.sideToneGain         = MICCOGetSideToneGain(path, volume);
			break;

		} //switch (pathOutput)

		switch (pathInput)
		{
		case AUX1_INPUT:
			//AUX1
			//pmcAux1.pgaAuxSelect        = AUXI1_IN;
			pmcAux1.pgaAuxEna           = TRUE;
			pmcAux1.pgaAuxGain          = MICCOGetAux1PgaGain(path, volume);
			break;
		case AUX2_INPUT:
			//AUX2
			//pmcAux2.pgaAuxSelect        = AUXI2_IN;
			pmcAux2.pgaAuxEna           = TRUE;
			pmcAux2.pgaAuxGain          = MICCOGetAux2PgaGain(path, volume);
			break;
		case AUX3_INPUT:
			//AUX3
			//pmcAux3.pgaAuxSelect        = AUXI3_IN;
			pmcAux3.pgaAuxEna           = TRUE;
			pmcAux3.pgaAuxGain          = MICCOGetAux3PgaGain(path, volume);
			break;
		case PCM_OUT:
		{
			pcmOut.vdacEna              = TRUE;
			pcmOut.vdacCntrl            = MICCOGetPCMvdacCntrl(path, volume);
			pcmOut.vdacMute             = FALSE;
			pcmOut.pcmLoop                          = FALSE;                                        // disable PCM loop when using another channel
		}
		break;

		case PCM_OUT_LOOP:
		{
			pcmOut.pcmLoop                          = TRUE;                                 // enable PCM loop
		}
		break;

		case SIDE_TONE_IN:
		{
//				unsigned char sideToneEna;

			pcmOut.sideToneCntrl        = MICCOGetSideToneCntrl(path, volume);
//		        pcmOut.sideToneGain         = MICCOGetSideToneGain(path, volume, &sideToneEna);
			pcmOut.sideToneGain         = MICCOGetSideToneGain(path, volume);
//			    pcmOut.sideToneEna          = sideToneEna;
			pcmOut.sideToneEna                      = TRUE;
		}
		break;
		case I2S_CH1_INPUT:
		case I2S_CH2_INPUT:
		case I2S_BOTH_INPUT:
		{
			hifi.i2sCntrl = MICCOGetI2SCntrl(path, volume);

			if (gMICCOCurrentPaths[path].muted)
			{                  /* I2S muted */
				hifi.hiFiDacCntrl = MICCOGetI2SHiFiDacCntrl(path, volume) | 0x18;
			}
			else
			{                  /* I2S not muted */
				hifi.hiFiDacCntrl = MICCOGetI2SHiFiDacCntrl(path, volume) & 0xFFE7;
			}

			/* Workaround: Only in MICCO C0 & C1_DB, whenever the input is I2S, the stereo PGAs must be enabled always (for example, in mono) */
			if ( (miccoChipID == 0x20) || (miccoChipID == 0x21) )                 /* MICCO_C0_ID and MICCO_ID_C1_DB Only! */
			{
				/* Setting stereoEna to TRUE will cause enable of ch1, by setting stereoGain_ch1 bit #7 by PMCCodecStereoOutCntrl();
				 * it causes Enable ch1+ch2 in B0, but only ch1 in C0 or newer */
				pmcStereo.stereoEna         = TRUE;
				pmcStereo.stereoGain_ch2    |= 0x80;                  /* Enable ch2 (in B0 it is being done by stereoGain_ch1 bit #7) */
			}
			/* End of workaround */
		}
		break;

		case MIC1_INPUT:
			pmcMicIn.micAmpSel          = MIC1_DIFF;
			pmcMicIn.micAmpEna          = TRUE;
			pmcMicIn.micAmp             = MICCOGetMicGain(path, volume);
			break;

		case MIC2_INPUT:
			pmcMicIn.micAmpSel          = MIC2_DIFF;
			pmcMicIn.micAmpEna          = TRUE;
			pmcMicIn.micAmp             = MICCOGetMicGain(path, volume);
			break;
/*
			case MIC1_INPUT_8BIT:
				pmcMicIn.micAmpSel          = MIC1_DIFF;
		pmcMicIn.micAmpEna          = TRUE;
		pmcMicIn.micAmp             = MICCOGetMicGain8Bit(path, volume, att_div3);
 */
		} //switch (pathInput)
	} //for(path...)

	//Call PMC Codec to set values
	if (memcmp((void *)&shadowPmcMicIn, (void *)&pmcMicIn, sizeof(PMC_micIn_t)))
	{
		PMCCodecMicInCntrl(&pmcMicIn);
		memcpy((void *)&shadowPmcMicIn, (void *)&pmcMicIn, sizeof(PMC_micIn_t));
	}

	if (memcmp((void *)&shadowPmcAux1, (void *)&pmcAux1, sizeof(PMC_aux_t)))
	{
		PMCCodecAuxInCntrl(&pmcAux1);
		memcpy((void *)&shadowPmcAux1, (void *)&pmcAux1, sizeof(PMC_aux_t));
	}

	if (memcmp((void *)&shadowPmcAux2, (void *)&pmcAux2, sizeof(PMC_aux_t)))
	{
		PMCCodecAuxInCntrl(&pmcAux2);
		memcpy((void *)&shadowPmcAux2, (void *)&pmcAux2, sizeof(PMC_aux_t));
	}

	if (memcmp((void *)&shadowPmcAux3, (void *)&pmcAux3, sizeof(PMC_aux_t)))
	{
		PMCCodecAuxInCntrl(&pmcAux3);
		memcpy((void *)&shadowPmcAux3, (void *)&pmcAux3, sizeof(PMC_aux_t));
	}

	if (memcmp((void *)&shadowPmcMono, (void *)&pmcMono, sizeof(PMC_mono_t)))
	{
		PMCCodecMonoOutCntrl(&pmcMono);
		memcpy((void *)&shadowPmcMono, (void *)&pmcMono, sizeof(PMC_mono_t));
	}

	if (memcmp((void *)&shadowPmcStereo, (void *)&pmcStereo, sizeof(PMC_stereo_t)))
	{
		PMCCodecStereoOutCntrl(&pmcStereo);
		memcpy((void *)&shadowPmcStereo, (void *)&pmcStereo, sizeof(PMC_stereo_t));
	}

	if ( memcmp((void *)&shadowPcmIn, (void *)&pcmIn, sizeof(PMC_pcmIn_t)) || memcmp((void *)&shadowPcmOut, (void *)&pcmOut, sizeof(PMC_pcmOut_t)) )
	{
		PMCCodecPcmInCntrl(&pcmIn);
		PMCCodecPcmOutCntrl(&pcmOut, &pcmIn);
		PMCCodecSideToneCntrl(&pcmOut);
		memcpy((void *)&shadowPcmIn, (void *)&pcmIn, sizeof(PMC_pcmIn_t));
		memcpy((void *)&shadowPcmOut, (void *)&pcmOut, sizeof(PMC_pcmOut_t));
	}

	if (memcmp((void *)&shadowHifi, (void *)&hifi, sizeof(PMC_hifi_t)))
	{
		PMCCodecHiFiCntrl(&hifi);
		memcpy((void *)&shadowHifi, (void *)&hifi, sizeof(PMC_hifi_t));
	}

	// Debug Plot
//	NKDbgPrintfW(TEXT( "MicEn = %d, Mic1_2 = %d, VdacEn = %d, TxPgaEn = %d, AdcEn = %d, StrEn = %d, MonoEn = %d, BearEn = %d, LineoutEn = %d\r\n"),
//		(UINT8)pmcMicIn.micAmpEna, (UINT8)pmcMicIn.micAmpSel ,(UINT8)pcmOut.vdacEna, (UINT8)pcmIn.txPgaEna,(UINT8)pcmIn.adcEna,(UINT8)pmcStereo.stereoEna,(UINT8)pmcMono.bearEna,(UINT8)pmcMono.monoEna ,(UINT8)pmcMono.audioLineOutEna);
}




static void MICCOSortPathList(void)
{
	int path;

	for (path = 0; path < MICCO_TOTAL_VALID_PATHS; path++)
	{
		if (gMICCOCurrentPaths[path].order != INVALID_INDEX)
		{
			//gMICCOPathsSortedList[gMICCODatabase.totalEnables - gMICCOCurrentPaths[path].order] = path; // reverse
			gMICCOPathsSortedList[gMICCOCurrentPaths[path].order - 1] = path;
		}
	}
}

static void MICCORemovePathFromList(unsigned char path)
{
	int i;
	unsigned char order = gMICCOCurrentPaths[path].order;

	//if already disabled
	if (gMICCOCurrentPaths[path].order == INVALID_INDEX)
		return;
	//remove path from list
	gMICCOCurrentPaths[path].order   = INVALID_INDEX;
	gMICCOCurrentPaths[path].volume  = INVALID_INDEX;
	gMICCOCurrentPaths[path].muted   = ACM_MUTE_OFF;

	//update database
	gMICCODatabase.totalEnables--;

	if (gMICCODatabase.totalEnables != 0)
	{
		for (i = 0; i < MICCO_TOTAL_VALID_PATHS; i++)
		{
			if ((gMICCOCurrentPaths[i].order != INVALID_INDEX) && (gMICCOCurrentPaths[i].order > order))
			{
				gMICCOCurrentPaths[i].order--;
			}
		}
		MICCOSortPathList();
	}
}

static void MICCOAddPathToList(unsigned char path, ACM_AudioVolume volume, ACM_DigitalGain digGain)
{
	//re-enabled ->first remove it's current location
	if (gMICCOCurrentPaths[path].order != INVALID_INDEX)
	{
		MICCORemovePathFromList(path);
	}
	//update database
	gMICCODatabase.totalEnables++;
//  gMICCODatabase.lastEnabled        = path;

	//update path data
	gMICCOCurrentPaths[path].order                      = gMICCODatabase.totalEnables;
	gMICCOCurrentPaths[path].volume                     = volume;
	gMICCOCurrentPaths[path].muted                      = ACM_MUTE_OFF;
	gMICCOCurrentPaths[path].digitalGain    = digGain;

	MICCOSortPathList();
}



/******************************************************************************/

/* API - through Audio structure */
ACM_DigitalGain MICCOPathEnable(unsigned char path, ACM_AudioVolume volume)
{
	ACM_DigitalGain digGain;

	digGain = MICCOGetDigitalGain(path, volume);

	MICCOAddPathToList(path, volume, digGain);

	MICCOUpdateHardware();

	return digGain;
}

ACM_DigitalGain MICCOPathDisable(unsigned char path)
{


	MICCORemovePathFromList(path);
	MICCOUpdateHardware();

	return gMICCOCurrentPaths[path].digitalGain;
}

ACM_DigitalGain MICCOPathVolumeSet(unsigned char path, ACM_AudioVolume volume)
{
	ACM_DigitalGain digGain;

	DBGMSG("AUDIO_MICCO_audioMICCOPathVolumeSet(%d,%d)", path, volume);

	digGain = MICCOGetDigitalGain(path, volume);

	gMICCOCurrentPaths[path].volume   = volume;
	gMICCOCurrentPaths[path].digitalGain    = digGain;

	MICCOUpdateHardware();

	return digGain;

}

ACM_DigitalGain MICCOPathMute(unsigned char path, ACM_AudioMute mute)
{

	DBGMSG("AUDIO_MICCO_audioMICCOPathMute(%d,%d)", path, (UINT8)mute);


	gMICCOCurrentPaths[path].muted = mute;
	MICCOUpdateHardware();

	return gMICCOCurrentPaths[path].digitalGain;
}


short MICCOGetPathsStatus(char* data, short length)
{
	unsigned char path;
	char* ptr = data;

	if (length < 1 + MICCO_TOTAL_VALID_PATHS * 4)
	{
		return -1; //buffer too small
	}

	*(ptr++) = MICCO_TOTAL_VALID_PATHS;

	for (path = 0; path < MICCO_TOTAL_VALID_PATHS; path++)
	{
		*(ptr++) = path;
		*(ptr++) = (gMICCOCurrentPaths[path].order != INVALID_INDEX); //1 = enabled, 0 = disabled
		*(ptr++) = gMICCOCurrentPaths[path].volume;
		*(ptr++) = gMICCOCurrentPaths[path].muted;
	}
	return ptr - data; //bytes written
}



/*******************************************************************************
* Function:		MICCOGetGainIndB
********************************************************************************
* Parameters:
*
* Return value: gain in dB depanding on the amps range in use
* Notes:
*******************************************************************************/
static ACM_AnalogGain MICCOGetGainIndB(unsigned char path, MICCO_Gains gainId, unsigned char pathLoc)
{
	short gain;

	//gain =  MICCO_GAIN_CALC( MiccoCurrentPathsForend[path].volume, calibrationData[path][gainId]);
	switch ( gainId )
	{
	case SpkrL_Out_Gain:                            //SpkrL PGA     -60 to 15 dB
	{
		gain = MICCO_GAIN_CALC(gMICCOCurrentPaths[path].volume, gMICCOSpkrLOutGain[pathLoc].pga);
		CLIP_SPKRL_OUT_GAIN(gain);
		gain =  (short)(15 + ( gain * (-1.5) ));                // treated mute as low gain (-60dB and less...)
		break;
	}
	case    SpkrR_Out_Gain:                         //SpkrR PGA     -60 to 15 dB
	{
		gain = MICCO_GAIN_CALC(gMICCOCurrentPaths[path].volume, gMICCOSpkrROutGain[pathLoc].pga);
		CLIP_SPKRR_OUT_GAIN(gain)
		gain =  (short)(15 + ( gain * (-1.5) ));                // treated mute as low gain (-60dB and less...)
		break;
	}
	case    Bear_Out_Gain:                          //Bear PGA              -60 to 15 dB
	{
		gain = MICCO_GAIN_CALC(gMICCOCurrentPaths[path].volume, gMICCOBearOutGain[pathLoc].pga);
		CLIP_BEAR_OUT_GAIN(gain);
		gain =  (short)(15 + ( gain * (-1.5) ));                // treated mute as low gain (-60dB and less...)
		break;
	}
	case    Mono_Out_Gain:                          //Mono PGA              -60 to 15 dB
	{
		gain = MICCO_GAIN_CALC(gMICCOCurrentPaths[path].volume, gMICCOMonoOutGain[pathLoc].pga);
		CLIP_MONO_OUT_GAIN(gain);
		gain =  (short)(15 + ( gain * (-1.5) ));                // treated mute as low gain (-60dB and less...)
		break;
	}
	case    Line_Out_Gain:                          //LineOut PGA   -30 to 15 dB
	{
		gain = MICCO_GAIN_CALC(gMICCOCurrentPaths[path].volume, gMICCOLineOutGain[pathLoc].pga);
		CLIP_LINE_OUT_GAIN(gain);
		gain = ((gain * (-3)) + 15);
		break;
	}
	case    Aux1_Out_Gain:                          //AUX1 PGA                0 to 18 dB
	{
		gain = MICCO_GAIN_CALC(gMICCOCurrentPaths[path].volume, gMICCOAux1InGain[pathLoc].mic);
		CLIP_AUX_OUT_GAIN(gain);
		gain = (gain * (6));

		break;
	}
	case    Aux2_Out_Gain:                          //AUX1 PGA                0 to 18 dB
	{
		gain = MICCO_GAIN_CALC(gMICCOCurrentPaths[path].volume, gMICCOAux2InGain[pathLoc].mic);
		CLIP_AUX_OUT_GAIN(gain);
		gain = (gain * (6));

		break;
	}
	case    Aux3_Out_Gain:                          //AUX1 PGA                0 to 18 dB
	{
		gain = MICCO_GAIN_CALC(gMICCOCurrentPaths[path].volume, gMICCOAux3InGain[pathLoc].mic);
		CLIP_AUX_OUT_GAIN(gain);
		gain = (gain * (6));

		break;
	}
	case    Mic_In_Gain:                            //Mic PGA		  0 to 30 dB
	{
		gain = MICCO_GAIN_CALC(gMICCOCurrentPaths[path].volume, gMICCOMicInGain[pathLoc].mic);
		CLIP_MIC_IN_GAIN(gain);
		if (gain >  5)
		{
			gain = 30;
		}
		else
		{
			gain = (gain * (6));              //relating gain Register value to Real DB
		}
		break;
	}
	case    TxPGA_In_Gain:                      //TX-PGA		-18 to 24 dB
	{
		gain = MICCO_GAIN_CALC(gMICCOCurrentPaths[path].volume, gMICCOTxPgaInGain[pathLoc].pga);
		CLIP_TXPGA_GAIN(gain);

		if (gain != 15)
		{
			gain = ((gain * (3)) + (-18));           //relating gain Register value to Real DB
		}
		else
		{
			gain = 24;
		}

		break;
	}
	case    VADC_Txdig_Gain:                        //TX-dig		 -6 to 12 dB
	{
		gain = MICCO_GAIN_CALC(gMICCOCurrentPaths[path].volume, gMICCOTxAdcInGain[pathLoc].pga);
		CLIP_VADC_GAIN(gain);
		gain = (gain * (-6)) + 12;                      //relating gain Register value to Real DB

		break;
	}
	case    SideTone_Gain:                                  //SideTone PGA -46.5to  0 dB
	{
		/* Special Case: Side Tone Gain is not part of Rx/Tx paths;
		 * the Tx\Rx paths analog gain is used for EC algorithm therfor this gain should contribute '0' dB */

		//return (gain * -1.5);
		gain = 0;                       //relating gain Register value to Real DB
		break;
	}
	}

	return ((ACM_AnalogGain)(gain & 0xFF));
}

/*******************************************************************************
* Function:		MICCOGetPathAnalogGain
********************************************************************************
* Parameters:
*
* Return value: the analog gain that would have been configured if this path
				is at high priority AKA was just enabled
* Notes:  the analog gain is the WCD in the sense of EC thresholds external gain
*******************************************************************************/
/*static */ ACM_AnalogGain       MICCOGetPathAnalogGain(unsigned char path)
{
	unsigned char j, k;
	unsigned char TotalPathInGain;
	ACM_AnalogGain analogGain       = 0;

	// loop over all the gain, and if the path is included in this gain add to analogGain

	for (j = 0; j < (MICCO_MAX_GAINS - 1); j++)      // the "-1" is to avoid the digital gain
	{
		//IMPORTANT
		//this calculation is based on the fact that this function "MICCOGetPathAnalogGain"
		//is called AFTER "Enable" of that specific path - only then the values returned
		//by MICCOGetGainIndB is the value which the register is configured to.


		TotalPathInGain = (gainConverter[j].totalPaths);
		for (k = 0; k < TotalPathInGain; k++)
		{
			// calculating the gain if the path is included in the gain
			if (gainConverter[j].gainPath[k].path == path)
				analogGain += MICCOGetGainIndB(path, j, k);
		}
	}

	DBGMSG("(AnalogGain = %d)", analogGain );

	return analogGain;
}


/************************************************************************
 * Function: MICCOIsExists
 *************************************************************************
 * Description: HW detection
 *
 * Parameters:
 *
 * Return value:
 *
 * Notes: compatible for MICCO B0 only (No A0 support)
 * *************************************************************************/
int      MICCOIsExists(void)
{
	return TRUE;
}

/************************************************************************
 * Function: MiccoCodecSetSampleRate
 *************************************************************************
 * Description: setting the I2S sampling rate
 *
 * Parameters:
 *
 * Return value:
 *
 * Notes: compatible for MICCO B0 only (No A0 support)
 * *************************************************************************/

void MiccoCodecSetSampleRate(int Port, int Rate)
{
	UINT8 VDacVal = VDAC_CTRL_VAL;
	UINT8 I2sVal;

	I2sVal = miccoIsClkMaster ? I2S_CTRL_VAL_MICCO_CLK_MASTER : I2S_CTRL_VAL_MICCO_CLK_SLAVE;

	switch (Port)
	{
	case 0 /*MICCO_VOICE_PLAYBACK*/:
		switch (Rate)
		{
		case 8000:
			VDacVal &= ~0x8;
			break;
		case 16000:
			VDacVal |= 0x8;
			break;
		default:
			return;
		}
		MICCOI2CWrite((unsigned char)VDAC_CTRL_ADDR, (unsigned char)VDacVal);
		break;

	case 1 /*MICCO_VOICE_RECORD*/:
		break;

	case 2 /*MICCO_HIFI_PLAYBACK*/:
		I2sVal &= 0xF0;
		switch (Rate)
		{
		case 8000:
			//ASSERT(miccoIsClkMaster == FALSE);
			gMICCODatabase.playBitRate = MICCO_BIT_RATE_8_KHZ;
			break;
		case 11025:
			//ASSERT(miccoIsClkMaster == FALSE);
			I2sVal |= 0x01;
			gMICCODatabase.playBitRate = MICCO_BIT_RATE_11_025_KHZ;
			break;
		case 12000:
			//ASSERT(miccoIsClkMaster == FALSE);
			I2sVal |= 0x02;
			gMICCODatabase.playBitRate = MICCO_BIT_RATE_12_KHZ;
			break;
		case 16000:
			if (miccoIsClkMaster)
				I2sVal |= 0x00;
			else
				I2sVal |= 0x03;
			gMICCODatabase.playBitRate = MICCO_BIT_RATE_16_KHZ;
			break;
		case 22050:
			if (miccoIsClkMaster)
				I2sVal |= 0x01;
			else
				I2sVal |= 0x04;
			gMICCODatabase.playBitRate = MICCO_BIT_RATE_22_05_KHZ;
			break;
		case 24000:
			if (miccoIsClkMaster)
				I2sVal |= 0x02;
			else
				I2sVal |= 0x05;
			gMICCODatabase.playBitRate = MICCO_BIT_RATE_24_KHZ;
			break;
		case 32000:
			if (miccoIsClkMaster)
				I2sVal |= 0x03;
			else
				I2sVal |= 0x06;
			gMICCODatabase.playBitRate = MICCO_BIT_RATE_32_KHZ;
			break;
		case 44100:
			if (miccoIsClkMaster)
				I2sVal |= 0x04;
			else
				I2sVal |= 0x07;
			gMICCODatabase.playBitRate = MICCO_BIT_RATE_44_1_KHZ;
			break;
		case 48000:
			if (miccoIsClkMaster)
				I2sVal |= 0x05;
			else
				I2sVal |= 0xF;
			gMICCODatabase.playBitRate = MICCO_BIT_RATE_48_KHZ;
			break;
		case 0xFF:
			gMICCODatabase.playBitRate = MICCO_BIT_RATE_OFF;
			break;
		default:
			return;
		}

		if (shadowHifi.i2sCntrl != I2sVal)
		{
			MICCOI2CWrite((unsigned char)I2S_CTRL_ADDR, (unsigned char)I2sVal);
			shadowHifi.i2sCntrl = I2sVal;
		}
		break;

	case 3 /*MICCO_HIFI_RECORD*/:
		switch (Rate)
		{
		case 8000:
			gMICCODatabase.recBitRate = MICCO_BIT_RATE_8_KHZ;
			break;

		case 11025:
			gMICCODatabase.recBitRate = MICCO_BIT_RATE_11_025_KHZ;
			break;

		case 16000:
			gMICCODatabase.recBitRate = MICCO_BIT_RATE_16_KHZ;
			break;

		case 22050:
			gMICCODatabase.recBitRate = MICCO_BIT_RATE_22_05_KHZ;
			break;

		case 32000:
			gMICCODatabase.recBitRate = MICCO_BIT_RATE_32_KHZ;
			break;

		case 44100:
			gMICCODatabase.recBitRate = MICCO_BIT_RATE_44_1_KHZ;
			break;

		case 0xFF:
			gMICCODatabase.recBitRate = MICCO_BIT_RATE_OFF;
			break;

		default:
			//ASSERT(FALSE);
			return;
		}
		break;

	default:
		return;
	}

}

/****************************** Functions taken from PMChip.c file (bulverde) -------------------------*/

/*************************************************  Codec  ***************************************************************/

// SSP configuration

static const ACM_SspAudioConfiguration _sspAudioMiccoConfiguration =
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
	/* devicePort  */ 0  /*GSSP0_PORT*/ /* DevicePort (GSSP0_PORT=0, GSSP1_PORT=1) */
};


/************************************************************************
 * Function: MICCOInit
 *************************************************************************
 * Description: initialize the codec
 *
 * Parameters:
 *
 * Return value: reinit, sspAudioConfiguration, ditherGenConfig
 *
 * Notes: compatible for MICCO B0 only (No A0 support)
 * *************************************************************************/
void MICCOInit(unsigned char reinit, ACM_SspAudioConfiguration **sspAudioConfiguration, signed short **ditherGenConfig)
{
	int i;
	unsigned char value;

	DBGMSG("MICCOInit\n");

//	ACM_ASSERT(sspAudioConfiguration != NULL);
	*sspAudioConfiguration = (ACM_SspAudioConfiguration *)&_sspAudioMiccoConfiguration;  /* Configure the SSP parameters */

	if (reinit)
	{       /* In case of reinit, update the SSP Configuration only and return! */
		return;
	}

	ditherGenConfig = NULL;  /* No Dither Config needed */


#if defined (OSA_LINUX)

	if (!MICCOI2CRead(0x70 + 0x16, &value))
	{
		miccoChipID = value;
	}
	else
	{
		miccoChipID = 0x10;
	}

#endif

#if 0
	/* Disable MONO */
	MICCOI2CWrite((unsigned char)RX_AUDIO_MUX1_ADDR, (unsigned char)RX_AUDIO_MUX1_VAL);

	/* Disable BEAR */
	MICCOI2CWrite((unsigned char)RX_AUDIO_MUX2_ADDR, (unsigned char)RX_AUDIO_MUX2_VAL);

	/* Disable LINE */
	MICCOI2CWrite((unsigned char)RX_AUDIO_MUX3_ADDR, (unsigned char)RX_AUDIO_MUX3_VAL);

	/* Disable STEREO2 */
	MICCOI2CWrite((unsigned char)RX_AUDIO_MUX4_ADDR, (unsigned char)RX_AUDIO_MUX4_VAL);

	/* Disable STEREO2  */
	MICCOI2CWrite((unsigned char)RX_AUDIO_MUX5_ADDR, (unsigned char)RX_AUDIO_MUX5_VAL);

	MICCOI2CWrite((unsigned char)AUDIO_LINE_AMP_ADDR, (unsigned char)0x82);
	MICCOI2CWrite((unsigned char)STEREO_AMP_CH1_ADDR, (unsigned char)STEREO_AMP_CH1_VAL);
	MICCOI2CWrite((unsigned char)STEREO_AMP_CH2_ADDR, (unsigned char)STEREO_AMP_CH2_VAL);

	if (miccoChipID >= 0x30 /* MICCO_ID_C1_EA or newer */)
		MICCOI2CWrite((unsigned char)HDAC_CTRL_ADDR, (unsigned char)HDAC_CTRL_VAL);
	else
		MICCOI2CWrite((unsigned char)HDAC_CTRL_ADDR, (unsigned char)(HDAC_CTRL_VAL | 0x80));    /* Always Enable HiFi DAC & PLL */

	MICCOI2CWrite((unsigned char)MONO_VOL_ADDR, (unsigned char)MONO_VOL_VAL);
	MICCOI2CWrite((unsigned char)BEAR_VOL_ADDR, (unsigned char)BEAR_VOL_VAL);
	MICCOI2CWrite((unsigned char)I2S_CTRL_ADDR, (unsigned char)I2S_CTRL_VAL_MICCO_CLK_SLAVE);    /* During init, Micco starts as slave */
	MICCOI2CWrite((unsigned char)TX_PGA_ADDR, (unsigned char)TX_PGA_VAL);

	MICCOI2CWrite((unsigned char)MIC_PGA_ADDR, (unsigned char)MIC_PGA_VAL);

	MICCOI2CWrite((unsigned char)TX_PGA_MUX_ADDR, (unsigned char)TX_PGA_MUX_VAL);
	MICCOI2CWrite((unsigned char)VADC_CTRL_ADDR, (unsigned char)VADC_CTRL_VAL);
	MICCOI2CWrite((unsigned char)VDAC_CTRL_ADDR, (unsigned char)VDAC_CTRL_VAL);
	MICCOI2CWrite((unsigned char)SIDE_TONE_ADDR, (unsigned char)SIDETONE_VAL);
	MICCOI2CWrite((unsigned char)PGA_AUXI1_2_ADDR, (unsigned char)PGA_AUXI1_2_VAL);
	MICCOI2CWrite((unsigned char)PGA_AUXI3_ADDR, (unsigned char)PGA_AUXI3_VAL);
	MICCOI2CWrite((unsigned char)PGA_DACS_ADDR, (unsigned char)PGA_DACS_VAL);
	MICCOI2CWrite((unsigned char)SOFT_START_RAMP_ADDR, (unsigned char)SOFT_START_RAMP_VAL);

#endif

	gMICCODatabase.totalEnables = 0;
	gMICCODatabase.playBitRate = MICCO_BIT_RATE_OFF;
	gMICCODatabase.recBitRate = MICCO_BIT_RATE_OFF;

	for (i = 0; i < MICCO_TOTAL_VALID_PATHS; i++)
	{
		gMICCOPathsSortedList[i]      = INVALID_INDEX;

		gMICCOCurrentPaths[i].order   = INVALID_INDEX;
		gMICCOCurrentPaths[i].volume  = INVALID_INDEX;
		gMICCOCurrentPaths[i].muted   = ACM_MUTE_OFF;
	}

}


/************************************************************************
 * Function: PMCCodecMicInCntrl
 *************************************************************************
 * Description:
 * Parameters:
 * typedef struct
 *{
 *     micAmpSel_t       micAmpSel;  - mic1/mic2
 *     BOOL              micAmpEna;  - ena/dis
 *     UINT8             micAmp;     -gain
 *} PMC_micIn_t;
 *
 *
 * Return value: none
 *
 * Notes:

 *************************************************************************/
void PMCCodecMicInCntrl ( PMC_micIn_t* pMicIn )
{
	UINT8 gain = 0, micSel;
	static UINT8 shadowGain = INVALID_FIELD_VALUE;

	if ( pMicIn->micAmpEna)
	{
		if ( pMicIn->micAmp > 0x07)
			pMicIn->micAmp = 0x07;
		micSel = pMicIn->micAmpSel;

		// micSel		//enable         //gain
		gain = (micSel << 4 | (UINT8)0x01 << 3 | pMicIn->micAmp );

		// for Mic1 - internal bias, for Mic2 - external bias
		if (micSel)
		{       // mic1
			gain = gain | 0x40;
		}
		else
		{       // mic2
			gain = gain | 0x20;
		}
	}

#if defined (OSA_WINCE)
/* This part is temporary for MICCO B0 only: in order to get the headset detection working */
	if ( (miccoChipID == 0x10 /* MICCO_ID_B0 */) && (_enableBiasForHeadsetDetection) )
		gain |= (1 << 6);  /* MICBIAS_EXT_ENABLE bit stays ON all the time for MIC Detection */
/* End of temporary part for MICCO B0 only */
#endif

	if (gain != shadowGain)
	{
		MICCOI2CWrite((unsigned char)MIC_PGA_ADDR, (unsigned char)gain);
		shadowGain = gain;
	}
}

/************************************************************************
 * Function: PMCCodecAuxInCntrl
 *************************************************************************
 * Description:
 * Parameters:
 *typedef struct
 *{
 *     srcSel_t          pgaAuxSelect; - choose the AUX input
 *     BOOL              pgaAuxEna;    - ena/dis the input
 *     UINT8             pgaAuxGain;   - gain for that input.
 *}PMC_aux_t;
 *
 * Return value: none
 *
 * Notes: previos setting will not harm other AUX settings.
 *************************************************************************/
void PMCCodecAuxInCntrl ( PMC_aux_t* pAux)
{
	static UINT8 pgaAuxI12 = 0, pgaAuxI3 = 0;
	static UINT8 shadowPgaAuxI12 = INVALID_FIELD_VALUE, shadowPgaAuxI3 = INVALID_FIELD_VALUE;

	switch (pAux->pgaAuxSelect)
	{
	case AUXI1_IN:
		pgaAuxI12 &= 0xF7;
		pgaAuxI12 |= ((UINT8)pAux->pgaAuxEna & 0x01) << 3;
		pgaAuxI12 &= 0xFC;
		if ( pAux->pgaAuxGain > 0x03)
			pAux->pgaAuxGain = 0x03;
		pgaAuxI12 |= (pAux->pgaAuxGain & 0x03);

		if (pgaAuxI12 != shadowPgaAuxI12)
		{
			MICCOI2CWrite((unsigned char)PGA_AUXI1_2_ADDR, (unsigned char)pgaAuxI12);
			shadowPgaAuxI12 = pgaAuxI12;
		}
		break;

	case AUXI2_IN:
		pgaAuxI12 &= 0x7F;
		pgaAuxI12 |= ((UINT8)pAux->pgaAuxEna & 0x01) << 7;
		pgaAuxI12 &= 0xCF;
		if ( pAux->pgaAuxGain > 0x03)
			pAux->pgaAuxGain = 0x03;
		pgaAuxI12 |= (pAux->pgaAuxGain & 0x03) << 4;

		if (pgaAuxI12 != shadowPgaAuxI12)
		{
			MICCOI2CWrite((unsigned char)PGA_AUXI1_2_ADDR, (unsigned char)pgaAuxI12);
			shadowPgaAuxI12 = pgaAuxI12;
		}
		break;

	case AUXI3_IN:
		pgaAuxI3 &= 0xF7;
		pgaAuxI3 |= ((UINT8)pAux->pgaAuxEna & 0x01) << 3;
		pgaAuxI3  &= 0xFC;
		if ( pAux->pgaAuxGain > 0x03)
			pAux->pgaAuxGain = 0x03;
		pgaAuxI3 |= (pAux->pgaAuxGain & 0x03);

		if (pgaAuxI3 != shadowPgaAuxI3)
		{
			MICCOI2CWrite((unsigned char)PGA_AUXI3_ADDR, (unsigned char)pgaAuxI3);
			shadowPgaAuxI3 = pgaAuxI3;
		}
		break;

	default:
		while (1) ;
		break;

	}

}

/************************************************************************
 * Function: PMCCodecMonoOutCntrl
 *************************************************************************
 * Description:
 * Parameters:
 * typedef struct
 *{
 *     srcSel_t          monoSrcSel;  - input source to the module.
 *     BOOL              monoEna;     - ena/dis
 *     UINT8             monoGain;    -gain
 *     BOOL              audioLineOutEna; -audio line out ena/dis
 *     UINT8             audioLineOutGain;  -audio line out gain
 *     BOOL              bearEna; -audio Bear ena/dis
 *     UINT8             bearGain;  -audio Bear gain
 *}PMC_mono_t;
 *
 *
 * Return value: none
 *
 * Notes:
 *************************************************************************/
void PMCCodecMonoOutCntrl( PMC_mono_t* pMono )
{
	if (pMono->monoSrcSel != shadowPmcMono.monoSrcSel)
		MICCOI2CWrite((unsigned char)RX_AUDIO_MUX1_ADDR, (unsigned char)pMono->monoSrcSel);    /* shadow will be updated when returning from this function */

	if (pMono->bearSrcSel != shadowPmcMono.bearSrcSel)
		MICCOI2CWrite((unsigned char)RX_AUDIO_MUX2_ADDR, (unsigned char)pMono->bearSrcSel);    /* shadow will be updated when returning from this function */

	if (pMono->audioLineOutSrcSel != shadowPmcMono.audioLineOutSrcSel)
		MICCOI2CWrite((unsigned char)RX_AUDIO_MUX3_ADDR, (unsigned char)pMono->audioLineOutSrcSel);    /* shadow will be updated when returning from this function */

	//    * * *

	/******** 4 possible routings: A) MONO+BEAR / B) MONO only / C) BEAR only / D) none *********/

	// A) MONO + BEAR
	if ( (pMono->monoEna) & (pMono->bearEna) )
	{
		//if gain beyond set to max gain.
		if (pMono->monoGain > 0x3E)
			pMono->monoGain = 0x3E;

		pMono->monoGain |= ((UINT8)0x01 << 6);

		//if gain beyond set to max gain.
		if (pMono->bearGain > 0x3E)
			pMono->bearGain = 0x3E;

		pMono->bearGain |= ((UINT8)0x01 << 6);

	}
	// B) MONO only
	else if ( (pMono->monoEna) & !(pMono->bearEna) )
	{
		//if gain beyond set to max gain.
		if (pMono->monoGain > 0x3E)
			pMono->monoGain = 0x3E;

		pMono->monoGain |= ((UINT8)0x01 << 6);

		// due to a bug in MICCO, need to enable the bear amp with mute.
		pMono->bearGain = 0x7f;
	}
	// C) BEAR only
	else if ( !(pMono->monoEna) && (pMono->bearEna) )
	{
		//if gain beyond set to max gain.
		if (pMono->bearGain > 0x3E)
			pMono->bearGain = 0x3E;

		pMono->bearGain |= ((UINT8)0x01 << 6);

		// due to a bug in MICCO, need to enable the mono amp with mute.
		pMono->monoGain = 0x7f;

	}
	else //disable  - 0x3F reset value
	{
		pMono->monoGain = 0x3F;
		pMono->bearGain = 0x3F;
	}

	//    * * *

	//if gain beyong then set max, 0x0F is mute.
	if (pMono->audioLineOutGain > 0xF)
		pMono->audioLineOutGain = 0xF;

	if (pMono->audioLineOutEna)
	{
		pMono->audioLineOutGain |= ((UINT8)0x01 << 4);
	}
	else //disable - 0x05 reset value = 0dB.
	{
		pMono->audioLineOutGain = 0x05;
	}


//	Add all 3 gains FAST-STARTUP option
	pMono->monoGain                 |= ((UINT8)0x01 << 7);
	pMono->bearGain                 |= ((UINT8)0x01 << 7);
	pMono->audioLineOutGain |= ((UINT8)0x01 << 5);

	if (pMono->monoGain != shadowPmcMono.monoGain)
		MICCOI2CWrite((unsigned char)MONO_VOL_ADDR, (unsigned char)pMono->monoGain);    /* shadow will be updated when returning from this function */

	if (pMono->bearGain != shadowPmcMono.bearGain)
		MICCOI2CWrite((unsigned char)BEAR_VOL_ADDR, (unsigned char)pMono->bearGain);    /* shadow will be updated when returning from this function */

	if (pMono->audioLineOutGain != shadowPmcMono.audioLineOutGain)
		MICCOI2CWrite((unsigned char)AUDIO_LINE_AMP_ADDR, (unsigned char)pMono->audioLineOutGain);    /* shadow will be updated when returning from this function */
}

/************************************************************************
 * Function: PMCCodecStereoOutCntrl
 *************************************************************************
 * Description:
 * Parameters:
 *
 *    BOOL                    stereoEna; //disable is for the stereo amplifier for both channels.
 *    srcSel_t                stereoSrcSel_ch1;
 *    UINT8                   stereoGain_ch1;
 *    srcSel_t                stereoSrcSel_ch2;
 *    UINT8                   stereoGain_ch2;
 *
 *
 * Return value:
 *
 * Notes:  disable is for the stereo amplifier -> for both channels.
 *        !!! the user must send full mux configuration for both channels.
 *************************************************************************/
void PMCCodecStereoOutCntrl ( PMC_stereo_t* pStereo )
{
	if (pStereo->stereoSrcSel_ch1 != shadowPmcStereo.stereoSrcSel_ch1)
		MICCOI2CWrite((unsigned char)RX_AUDIO_MUX4_ADDR, (unsigned char)pStereo->stereoSrcSel_ch1);    /* shadow will be updated when returning from this function */

	if (pStereo->stereoSrcSel_ch2 != shadowPmcStereo.stereoSrcSel_ch2)
		MICCOI2CWrite((unsigned char)RX_AUDIO_MUX5_ADDR, (unsigned char)pStereo->stereoSrcSel_ch2);    /* shadow will be updated when returning from this function */

	if ( pStereo->stereoEna)
	{

		//if beyond the gain set max gain.
		if (pStereo->stereoGain_ch1 > 0x3F)
			pStereo->stereoGain_ch1 = 0x3F;
		if ((pStereo->stereoGain_ch2 & ~0x80) > 0x3F) /* Check the value of the gain without the value of bit #7 */
			pStereo->stereoGain_ch2 = 0x3F;

		//0xBF setting for separat gains.
		pStereo->stereoGain_ch1 &= 0xBF;

		//enable both stereo amplifiers. In MICCO C0 or newer, it enables only channel 1 amplifier.
		pStereo->stereoGain_ch1 |= 0x80;
	}
	else //disable - 0x3F reset value.
	{
		// defualt gain, not enabled.
		pStereo->stereoGain_ch1 = 0x3F;
		pStereo->stereoGain_ch2 = 0x3F;
	}
	// gain setting is only separately.!!!

//	Add stereo gains SOFT-STARTUP option
	pStereo->stereoGain_ch2 &=   ~((UINT8)0x01 << 6);



	//ch1 gain
	if (pStereo->stereoGain_ch1 != shadowPmcStereo.stereoGain_ch1)
		MICCOI2CWrite((unsigned char)STEREO_AMP_CH1_ADDR, (unsigned char)pStereo->stereoGain_ch1);    /* shadow will be updated when returning from this function */

	//ch2 gain
	if (pStereo->stereoGain_ch2 != shadowPmcStereo.stereoGain_ch2)
		MICCOI2CWrite((unsigned char)STEREO_AMP_CH2_ADDR, (unsigned char)pStereo->stereoGain_ch2);    /* shadow will be updated when returning from this function */
}

/************************************************************************
 * Function: PMCCodecPcmInCntrl
 *************************************************************************
 * Description:
 * Parameters:
 *typedef struct
 *{
 *    pgaSrcSel_t         pgaSrcSel;
 *    BOOL                txPgaEna;
 *    UINT8               txPga; - gain
 *    BOOL                micAmpDspGain;
 *    BOOL                adcEna;
 *    BOOL                adcMute;
 *    UINT8               adcHpfCntrl;
 *    UINT8               adcGain;
 *}PMC_pcmIn_t;
 *
 *
 * Return value: none
 *
 * Notes:
 *************************************************************************/
void PMCCodecPcmInCntrl ( PMC_pcmIn_t* pPcmIn )
{
	UINT8 txPga = 0, adcVal = 0;
	static UINT8 shadowTxPga = INVALID_FIELD_VALUE, shadowAdcVal = INVALID_FIELD_VALUE;

	if (pPcmIn->txPgaEna)
	{
		pPcmIn->txPga &= 0x0F; //gain (bit1-bit4)
		// if to much gain set the max gain avalible.
		if (pPcmIn->txPga > 0x0F)
			pPcmIn->txPga = 0x0F;
		// do we want dsp gain?
		if (pPcmIn->micAmpDspGain)      //bit5=enable//bit0=1 dsp gain 0db
			txPga = ( pPcmIn->txPga << 1 ) | 0x01;
		else                            //bit5=enable//bit0=0 dsp gain -1.5db
			txPga = ( pPcmIn->txPga << 1 ) & 0xFE;
	}
	else // disable PGA a, amplifier (bit5=0) - 0x00 reset value..
	{
		// should take an image for now 0x00 = reset value.
		txPga = 0x00;
	}

	if (pPcmIn->pgaSrcSel != shadowPcmIn.pgaSrcSel)
		MICCOI2CWrite((unsigned char)TX_PGA_MUX_ADDR, (unsigned char)pPcmIn->pgaSrcSel);    /* shadow will be updated when returning from this function */

	//write the gain to the MICAMP_PGA
	if (txPga != shadowTxPga)
	{
		MICCOI2CWrite((unsigned char)TX_PGA_ADDR, (unsigned char)txPga);
		shadowTxPga = txPga;
	}

	if (pPcmIn->adcEna)
	{
		//ADC ena/dia
		adcVal =  (UINT8)pPcmIn->adcEna & 0x01;
		//Hpf cntrl
		adcVal |=  ((UINT8)pPcmIn->adcHpfCntrl & 0x03) << 1;

		/* Set the record rate */
		adcVal &=  0xE7; /* Set bits 3-4  to 0b00 */

		switch (gMICCODatabase.recBitRate)
		{                                       /* Set bits 3-4 */
		case MICCO_BIT_RATE_8_KHZ:
		case MICCO_BIT_RATE_11_025_KHZ:         /* Set value of 0b00 (like 8 KHz) */
			/* Value stays 0b00 */
			break;

		case MICCO_BIT_RATE_16_KHZ:
			adcVal |=  0x08; /* Set value of 0b01 */
			break;

		case MICCO_BIT_RATE_22_05_KHZ:
			adcVal |=  0x08; /* Set value of 0b01 (like 16 KHz) */
			break;

		case MICCO_BIT_RATE_32_KHZ:
		case MICCO_BIT_RATE_44_1_KHZ:   /* Set value of 0b10 (like 32 KHz) */
			adcVal |=  0x10;        /* Set value of 0b10 */
			break;

		case MICCO_BIT_RATE_OFF:
			/* Do nothing! If HiFi record is not active, getting here is a normal thing! */
			break;

		default:
			//ASSERT(FALSE);
			break;
		}

		//ADC gain
		if ( pPcmIn->adcGain > 0x03)
			pPcmIn->adcGain = 0x03;

		adcVal |=  ((UINT8)pPcmIn->adcGain & 0x03) << 5;

		//mute on/off
		adcVal |= ((UINT8)pPcmIn->adcMute & 0x01 ) << 7;

	}
	else
	{
		//should take an image for now 0x00 = reset value
		adcVal = VADC_CTRL_VAL;
	}

	if (adcVal != shadowAdcVal)
	{
		MICCOI2CWrite((unsigned char)VADC_CTRL_ADDR, (unsigned char)adcVal);
		shadowAdcVal = adcVal;
	}
}


/************************************************************************
 * Function: PMCCodecPcmOutCntrl
 *************************************************************************
 * Description:
 * Parameters:
 * typedef struct
 *{
 *    BOOL                vdacEna;
 *    UINT8               vdacCntrl;
 *    BOOL                vdacMute;
 *}PMC_pcmOut_t;
 *
 *typedef struct
 *{
 *    pgaSrcSel_t         pgaSrcSel;
 *    BOOL                txPgaEna;
 *    UINT8               txPga; - gain
 *    BOOL                micAmpDspGain;
 *    BOOL                adcEna;
 *    BOOL                adcMute;
 *    UINT8               adcHpfCntrl;
 *    UINT8               adcGain;
 *}PMC_pcmIn_t;
 *
 * Return value:
 *
 * Notes:   none
 *************************************************************************/
void PMCCodecPcmOutCntrl( PMC_pcmOut_t* pPcmOut,  PMC_pcmIn_t* pPcmIn )
{
	UINT8 dacVal = 0;
	static UINT8 shadowDacVal = INVALID_FIELD_VALUE;

	if (pPcmOut->vdacEna)
	{
		dacVal  =  (UINT8)pPcmOut->vdacCntrl & 0x36;
		dacVal |= ((UINT8)pPcmOut->vdacEna & 0x01) << 3;

		if (pPcmOut->vdacMute)
			dacVal |= 0x01;
		else
			dacVal &= 0xFE;
	}
	else //disable  -reset value
	{
		dacVal = 0x40;                  // to set the HiZ bit - HiZ enabled
	}

	if (pPcmOut->pcmLoop)
		dacVal = dacVal | 0x80;         // to set the PCM loop bit - with PCM loop
	else
		dacVal = dacVal & 0x7F;         // to reset the PCM Loop bit - no PCM loop

	if (pPcmIn->adcEna)
	{
		// SDO pin control - if the ADC is'nt on then put the SDO in HiZ (to allow bluetooth input).
		dacVal = dacVal & 0xBF; // to reset the HiZ bit - HiZ not enabled
	}
	else
	{
		dacVal = dacVal | 0x40; // to set the HiZ bit - HiZ  enabled
	}

	if (dacVal != shadowDacVal)
	{
		MICCOI2CWrite((unsigned char)VDAC_CTRL_ADDR, (unsigned char)dacVal);
		shadowDacVal = dacVal;
	}

}
/************************************************************************
 * Function: PMCCodecSideToneCntrl
 *************************************************************************
 * Description:
 * Parameters:
 * typedef struct
 *{

 *    BOOL                sideToneEna;
 *    UINT8               sideToneCntrl;
 *    UINT8               sideToneGain;
 *}PMC_pcmOut_t;
 *
 *
 * Return value:
 *
 * Notes:   none
 *************************************************************************/
void PMCCodecSideToneCntrl( PMC_pcmOut_t* pPcmOut )
{
	if (pPcmOut->sideToneEna)
	{
		pPcmOut->sideToneCntrl |= 0x80;
		pPcmOut->sideToneCntrl = (pPcmOut->sideToneCntrl & ~0x1F) | (pPcmOut->sideToneGain & 0x1F);
	}
	else //reset value.
	{
		pPcmOut->sideToneCntrl = 0x00;
	}

	if (pPcmOut->sideToneCntrl != shadowPcmOut.sideToneCntrl)
		MICCOI2CWrite((unsigned char)SIDE_TONE_ADDR, (unsigned char)pPcmOut->sideToneCntrl);    /* shadow will be updated when returning from this function */
}

/************************************************************************
 * Function: PMCCodecHiFiCntrl
 *************************************************************************
 * Description:
 * Parameters:
 *   typedef struct
 *{
 *    UINT8       i2sCntrl;
 *    UINT8       hiFiDacCntrl;
 *}PMC_hifi_t;
 *
 * Return value:
 *
 * Notes:

 *************************************************************************/
void PMCCodecHiFiCntrl( PMC_hifi_t* pHifi )
{
	if (pHifi->hiFiDacCntrl != shadowHifi.hiFiDacCntrl)
		MICCOI2CWrite((unsigned char)HDAC_CTRL_ADDR, (unsigned char)pHifi->hiFiDacCntrl);    /* shadow will be updated when returning from this function */

	if (pHifi->i2sCntrl != shadowHifi.i2sCntrl)
		MICCOI2CWrite((unsigned char)I2S_CTRL_ADDR, (unsigned char)(pHifi->i2sCntrl));    /* shadow will be updated when returning from this function */
}


#if defined (OSA_WINCE)
/* This part is temporary for MICCO B0 only: in order to get the headset detection working */
void enableBiasForHeadsetDetectionSet(BOOL enableBiasForHeadsetDetection)
{
	NKDbgPrintfW(TEXT("*** MICCO: dwEnableBiasForHeadsetDetection= %d ***\r\n"), enableBiasForHeadsetDetection);

	_enableBiasForHeadsetDetection = enableBiasForHeadsetDetection;
}
#endif  /* OSA_WINCE */
/* End of temporary part for MICCO B0 only */

void MiccoClkMasterSet(BOOL isMaster)
{
	unsigned char i2sCntrl = isMaster ? I2S_CTRL_VAL_MICCO_CLK_MASTER : I2S_CTRL_VAL_MICCO_CLK_SLAVE;

	//NKDbgPrintfW(TEXT("*** MiccoClkMasterSet; isMaster= %d ***\r\n"), isMaster);

	/* Preserve all current bits, except the CLK Master/Slave bits */
	i2sCntrl |= (shadowHifi.i2sCntrl & ~(I2S_CTRL_VAL_MICCO_CLK_MASTER | I2S_CTRL_VAL_MICCO_CLK_SLAVE));

	miccoIsClkMaster = isMaster;

	if (shadowHifi.i2sCntrl != i2sCntrl)
	{
		MICCOI2CWrite((unsigned char)I2S_CTRL_ADDR, i2sCntrl);

		shadowHifi.i2sCntrl = i2sCntrl;
	}
}

