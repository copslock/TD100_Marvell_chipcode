/*******************************************************************************
*               MODULE HEADER FILE
********************************************************************************
* Title: audioMICCO
*
* Filename: audioMICCO.h
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
#ifndef _AUDIO_MICCO_H_
#define _AUDIO_MICCO_H_

#ifdef __cplusplus
extern "C" {
#endif


/*---------------------------------------------------------------------------*/

// Defaul setting for Micco audio control registers
#define RX_AUDIO_MUX1_VAL                               0x00
#define RX_AUDIO_MUX2_VAL                               0x00
#define RX_AUDIO_MUX3_VAL                               0x00

#define RX_AUDIO_MUX4_VAL                               0x00
#define RX_AUDIO_MUX5_VAL                               0x00

#define AUDIO_LINE_AMP_VAL                              0x20    // Fast Startup
#define STEREO_AMP_CH1_VAL                              0x00
#define STEREO_AMP_CH2_VAL                              0x00    // Soft Startup
#define HDAC_CTRL_VAL                                   0x00
#define MONO_VOL_VAL                                    0x80    // Fast Startup
#define BEAR_VOL_VAL                                    0x80    // Fast Startup
#define I2S_CTRL_VAL_MICCO_CLK_MASTER   0x20                    // Set the Micco as Master for SSP HiFi
#define I2S_CTRL_VAL_MICCO_CLK_SLAVE    0x40                    // Set the Micco as Slave for SSP HiFi
#define TX_PGA_VAL                                              0x00
#define MIC_PGA_VAL                                             0x00
#define TX_PGA_MUX_VAL                                  0x00
#define VADC_CTRL_VAL                                   0x02 // Deafult filter
#define VDAC_CTRL_VAL                                   0x00
#define SIDETONE_VAL                                    0x00
#define PGA_AUXI1_2_VAL                                 0x00
#define PGA_AUXI3_VAL                                   0x00

#define PGA_DACS_VAL                                0x00
#define SOFT_START_RAMP_VAL                         0xFF    //Soft Startup is 500 ms

// control regs
#define MICCO_PCM_VDAC_CNTRL_DEFAULT      0x00
#define MICCO_PCM_SIDETONE_CNTRL_DEFAULT  0x00
#define MICCO_I2S_HIFI_DAC_CNTRL_DEFAULT  HDAC_CTRL_VAL
#define MICCO_ADC_HPF_CNTRL_DEFAULT       0x01

/* ID's are 0 - 10  - regular gain
	    11      - digital gain
	    12      - side tone gain*/
#define MICCO_TOTAL_GAIN_IDS                            13

#define INVALID_INDEX       0xFF

#define MICCO_AUDIO_FDI_FILENAME          "audio_micco.nvm"
#define MICCO_SOFTMUTE_FDI_FILENAME       "audio_micco_sm.nvm"

/* File Structure is:
   unsigned long         pga ID
   unsigned long         path ID
   unsigned long         lane
   signed short          slope
   signed short          offset
 */

#define MICCO_GAIN_CALC(vOL, sLOPEoFFSET)   ((signed char)(((((signed long)(sLOPEoFFSET.slope) * (vOL) + ((sLOPEoFFSET.offset) << 3)) >> 9) + 1) >> 1))




// klap - different definition from MICCO - will need adjustments

/* input */
//IC AT EXPORTED ENUM
typedef enum
{
	AUX1_INPUT          = 0,
	AUX2_INPUT,
	AUX2NEG_INPUT,
	AUX3_INPUT,
	I2S_CH1_INPUT,
	I2S_CH2_INPUT,
	I2S_BOTH_INPUT,
	MIC1_INPUT,
	MIC2_INPUT,
	PCM_OUT,
	PCM_OUT_LOOP,
	SIDE_TONE_IN,
	MICCO_MAX_PATH_INPUT,
	MICCO_PATHS_INPUT_ENUM_32_BIT                   = 0x7FFFFFFF //32bit enum compiling enforcement
}MICCOPathInput;

/* output*/
//IC AT EXPORTED ENUM
typedef enum
{
	LINE_OUT_OUTPUT   = 0,
	MONO_OUT_OUTPUT,
	PCM_IN,
	PCM_IN_LOOP,
	STEREO_CH1_OUTPUT,
	STEREO_CH2_OUTPUT,
	STEREO_BOTH_OUTPUT,
	BEAR_OUT_OUTPUT,
	STEREO_BOTH_MONO_OUTPUT,
	STEREO_CH1_MONO_OUTPUT,
	STEREO_CH2_MONO_OUTPUT,
	SIDE_TONE_OUT,
	MONO_OUT_PCM_OUTPUT,
	BEAR_OUT_PCM_OUTPUT,
	STEREO_CH1_PCM_OUTPUT,
	STEREO_CH2_PCM_OUTPUT,
	STEREO_BOTH_PCM_OUTPUT,
	LINE_OUT_PCM_OUTPUT,
	MICCO_MAX_PATH_OUTPUT,
	MICCO_PATHS_OUTPUT_ENUM_32_BIT                  = 0x7FFFFFFF //32bit enum compiling enforcement


}MICCOPathOutput;

/////////////////////////////////////////////////////////

typedef struct
{
	MICCOPathInput pathInput;
	MICCOPathOutput pathOutput;
}MICCOPathDescriptionS;

typedef struct
{
	unsigned char order;    /* 0 = disabled, else - place in list*/
	unsigned char volume;   /* 0xFF = disabled */
	ACM_AudioMute muted;
	ACM_DigitalGain digitalGain;
}MICCOCurrentPathS;

typedef struct
{
	unsigned char totalEnables;
	MICCO_I2SbitRate playBitRate;
	MICCO_I2SbitRate recBitRate;
}MICCODatabaseS;

typedef signed short MICCO_SlopeType;
typedef signed short MICCO_OffsetType;

typedef struct
{
	MICCO_SlopeType slope;
	MICCO_OffsetType offset;
} MICCO_SlopeOffsetS;

// List of gain's ID for the FDI structure



typedef enum
{
	SpkrL_Out_Gain = 0,             //SpkrL PGA     -60 to 15 dB
	SpkrR_Out_Gain,                 //SpkrR PGA     -60 to 15 dB
	Bear_Out_Gain,                  //Bear PGA              -60 to 15 dB
	Mono_Out_Gain,                  //Mono PGA              -60 to 15 dB
	Line_Out_Gain,                  //LineOut PGA   -30 to 15 dB
	Aux1_Out_Gain,                  //AUX1 PGA                0 to 18 dB
	Aux2_Out_Gain,                  //AUX2 PGA                0 to 18 dB
	Aux3_Out_Gain,                  //AUX3 PGA                0 to 18 dB
	Mic_In_Gain,                    //Mic PGA		  0 to 30 dB
	TxPGA_In_Gain,                  //TX-PGA		-18 to 18 dB
	VADC_Txdig_Gain,                //TX-dig		 -6 to 12 dB
	SideTone_Gain,                  //SideTone PGA -46.5to  0 dB
	DigitalGain,
	MICCO_MAX_GAINS
}MICCO_Gains;


//Gain ID 0
typedef struct
{
	MICCOPaths path;
	MICCO_SlopeOffsetS pga;
}MICCO_SpkrLOutGainS;

//Gain ID 1
typedef struct
{
	MICCOPaths path;
	MICCO_SlopeOffsetS pga;
}MICCO_SpkrROutGainS;

//Gain ID 2
typedef struct
{
	MICCOPaths path;
	MICCO_SlopeOffsetS pga;
}MICCO_BearOutGainS;

//Gain ID 3
typedef struct
{
	MICCOPaths path;
	MICCO_SlopeOffsetS pga;
}MICCO_MonoOutGainS;

//Gain ID 4
typedef struct
{
	MICCOPaths path;
	MICCO_SlopeOffsetS pga;
}MICCO_LineOutGainS;

//Gain ID's 5, 6, 7
typedef struct
{
	MICCOPaths path;
	MICCO_SlopeOffsetS mic;
}MICCO_AuxInGainS;


//Gain ID 8
typedef struct
{
	MICCOPaths path;
	MICCO_SlopeOffsetS mic;
}MICCO_MicInGainS;

//Gain ID 9
typedef struct
{
	MICCOPaths path;
	MICCO_SlopeOffsetS pga;
}MICCO_TxPGAInGainS;

//Gain ID 10
typedef struct
{
	MICCOPaths path;
	MICCO_SlopeOffsetS pga;
}MICCO_TxADCInGainS;

//Dig Gain ID 11
typedef struct
{
	MICCOPaths path;
	MICCO_SlopeOffsetS pga;
}MICCO_TxADCDigInGainS;


//Side Tone Gain ID 12
typedef struct
{
	MICCOPaths path;
	MICCO_SlopeOffsetS gain;
}MICCO_SideToneGainS;

//Digital Gain ID 13
typedef struct
{
	MICCOPaths path;
	MICCO_SlopeOffsetS gain;
}MICCO_DigitalGainS;


/////////////////////////////////////////



//Internal use Gain structure
typedef struct
{
	MICCOPaths path;
	MICCO_SlopeOffsetS pga;
}MICCO_1PgaGainS;

typedef struct
{
	MICCOPaths path;
	//   MICCO_SlopeOffsetS        pgaArray[3]; // klap test
	MICCO_SlopeOffsetS pgaArray;
}MICCO_GainPathU;

typedef struct
{
	MICCO_GainPathU   *gainPath;
	UINT8 totalPaths;
	//   UINT8           numOfPga; // klap test
	//   UINT8           pgaNum; // klap test
}MICCO_FileGain2StructGainConverter;

/////////////////////////////////////////

//Sorce mask.
#define    AUXI1_SRC            (UINT8)(1 << 0)
#define    AUXI2_SRC            (UINT8)(1 << 1)
#define    AUXI2N_SRC           (UINT8)(1 << 6)
#define    AUXI3_SRC            (UINT8)(1 << 2)
#define    PCM_DAC_SRC          (UINT8)(1 << 5)
#define    I2S_CH1_DAC_SRC      (UINT8)(1 << 3)
#define    I2S_CH2_DAC_SRC      (UINT8)(1 << 4)
#define    MIC1_LOOP_SRC            (UINT8)(1 << 7)

//////////////////////////////////////
typedef UINT8 srcSel_t; // klap - no need for UINT32

//ICAT EXPORTED ENUM
typedef enum
{
	MIC1_DIFF,
	MIC2_DIFF
} micAmpSel_t;

//ICAT EXPORTED ENUM
typedef enum
{
	AUXI1_IN,
	AUXI2_IN,
	AUXI3_IN
} auxInSel_t;

//ICAT EXPORTED STRUCT
typedef struct
{
	micAmpSel_t micAmpSel;
	BOOL micAmpEna;
	UINT8 micAmp;
} PMC_micIn_t;

//ICAT EXPORTED STRUCT
typedef struct
{
	auxInSel_t pgaAuxSelect;
	BOOL pgaAuxEna;
	UINT8 pgaAuxGain;
}PMC_aux_t;

//ICAT EXPORTED STRUCT
typedef struct
{
	srcSel_t monoSrcSel;
	BOOL monoEna;
	UINT8 monoGain;
	srcSel_t audioLineOutSrcSel;
	BOOL audioLineOutEna;
	UINT8 audioLineOutGain;
	srcSel_t bearSrcSel;
	BOOL bearEna;
	UINT8 bearGain;
}PMC_mono_t;

//ICAT EXPORTED STRUCT
typedef struct
{
	BOOL stereoEna;                //disable is for the stereo amplifier for both channels.
	srcSel_t stereoSrcSel_ch1;
	UINT8 stereoGain_ch1;
	srcSel_t stereoSrcSel_ch2;
	UINT8 stereoGain_ch2;
}PMC_stereo_t;

//ICAT EXPORTED ENUM
typedef enum
{
	AUXI1 = 0x02,
	MIC_DIFF = 0x0C,
	AUXI2 = 0x11,
	AUXI3 = 0x20,
	I2S_CH1 = 0x40,
	I2S_CH2 = 0x80,
	NO_SOURCE = 0x0
} pgaSrcSel_t;

//ICAT EXPORTED STRUCT
typedef struct
{
	pgaSrcSel_t pgaSrcSel;
	BOOL txPgaEna;
	UINT8 txPga;
	BOOL micAmpDspGain;
	BOOL adcEna;
	BOOL adcMute;
	UINT8 adcHpfCntrl;
	UINT8 adcGain;
}PMC_pcmIn_t;

//ICAT EXPORTED STRUCT
typedef struct
{
	BOOL vdacEna;
	UINT8 vdacCntrl;
	BOOL vdacMute;
	BOOL sideToneEna;
	UINT8 sideToneCntrl;
	UINT8 sideToneGain;
	BOOL pcmLoop;
}PMC_pcmOut_t;

//ICAT EXPORTED STRUCT
typedef struct
{
	UINT8 i2sCntrl;
	UINT8 hiFiDacCntrl;
}PMC_hifi_t;


ACM_DigitalGain  MICCOPathEnable(unsigned char path, unsigned char volume);
ACM_DigitalGain  MICCOPathDisable(unsigned char path);
ACM_DigitalGain  MICCOPathMute(unsigned char path, ACM_AudioMute muteEnable);
ACM_DigitalGain  MICCOPathVolumeSet(unsigned char path, unsigned char volume);
short                    MICCOGetPathsStatus(char* data, short length);
ACM_AnalogGain   MICCOGetPathAnalogGain(unsigned char path);

//sepertion layer built for being able to enable path by API function{auto lane select}
//											   and by calibration function{forced lane)
ACM_DigitalGain  MICCOPathEnableOperation(unsigned char path, unsigned char volume, unsigned char autoLane);
ACM_DigitalGain  MICCOPathDisableOperation(unsigned char path, unsigned char autoLane);
ACM_DigitalGain  MICCOPathMuteOperation(unsigned char path, ACM_AudioMute muteEnable, unsigned char autoLane);
ACM_DigitalGain  MICCOPathVolumeSetOperation(unsigned char path, unsigned char volume, unsigned char autoLane);

void                     MICCOInit(unsigned char reinit, ACM_SspAudioConfiguration **sspAudioConfiguration, signed short **ditherGenConfig);
//void			 MICCOInit(void);


typedef void (*I2CSendNotify)(UINT16);

// Audio MICCO control registers  address */
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define CODEC_BASE_ADDR                                 0x70
#define REG_OFFSET                                              0x05

#define RX_AUDIO_MUX1_ADDR                              (CODEC_BASE_ADDR + 0x00)
#define RX_AUDIO_MUX2_ADDR                              (CODEC_BASE_ADDR + 0x01)
#define RX_AUDIO_MUX3_ADDR                              (CODEC_BASE_ADDR + 0x02)

#define RX_AUDIO_MUX4_ADDR                              (CODEC_BASE_ADDR + 0x03)
#define RX_AUDIO_MUX5_ADDR                              (CODEC_BASE_ADDR + 0x04)

#define AUDIO_LINE_AMP_ADDR                             (CODEC_BASE_ADDR + REG_OFFSET + 0x00)
#define STEREO_AMP_CH1_ADDR                             (CODEC_BASE_ADDR + REG_OFFSET + 0x01)
#define STEREO_AMP_CH2_ADDR                             (CODEC_BASE_ADDR + REG_OFFSET + 0x02)
#define HDAC_CTRL_ADDR                                  (CODEC_BASE_ADDR + REG_OFFSET + 0x03)
#define MONO_VOL_ADDR                                   (CODEC_BASE_ADDR + REG_OFFSET + 0x04)
#define BEAR_VOL_ADDR                                   (CODEC_BASE_ADDR + REG_OFFSET + 0x05)
#define I2S_CTRL_ADDR                                   (CODEC_BASE_ADDR + REG_OFFSET + 0x06)
#define TX_PGA_ADDR                                             (CODEC_BASE_ADDR + REG_OFFSET + 0x07)
#define MIC_PGA_ADDR                                    (CODEC_BASE_ADDR + REG_OFFSET + 0x08)
#define TX_PGA_MUX_ADDR                                 (CODEC_BASE_ADDR + REG_OFFSET + 0x09)
#define VADC_CTRL_ADDR                                  (CODEC_BASE_ADDR + REG_OFFSET + 0x0A)
#define VDAC_CTRL_ADDR                                  (CODEC_BASE_ADDR + REG_OFFSET + 0x0B)
#define SIDE_TONE_ADDR                                  (CODEC_BASE_ADDR + REG_OFFSET + 0x0C)
#define PGA_AUXI1_2_ADDR                                (CODEC_BASE_ADDR + REG_OFFSET + 0x0D)
#define PGA_AUXI3_ADDR                                  (CODEC_BASE_ADDR + REG_OFFSET + 0x0E)
#define PGA_DACS_ADDR                                   (CODEC_BASE_ADDR + REG_OFFSET + 0x0F)
#define SOFT_START_RAMP_ADDR                    (CODEC_BASE_ADDR + REG_OFFSET + 0x10)


// Register settings for Micco codec (MUX)
	#define DAC1_TO_MONO_BIT                                (1 << 3)
	#define DAC2_TO_MONO_BIT                                (1 << 4)
	#define DAC3_TO_MONO_BIT                                (1 << 5)

	#define DAC1_TO_SCH1_BIT                                (1 << 3)
	#define DAC2_TO_SCH1_BIT                                (1 << 4)
	#define DAC3_TO_SCH1_BIT                                (1 << 5)

	#define DAC1_TO_SCH2_BIT                                (1 << 3)
	#define DAC2_TO_SCH2_BIT                                (1 << 4)
	#define DAC3_TO_SCH2_BIT                                (1 << 5)

	#define DAC1_TO_BEAR_BIT                                (1 << 3)
	#define DAC2_TO_BEAR_BIT                                (1 << 4)
	#define DAC3_TO_BEAR_BIT                                (1 << 5)

	#define DAC1_TO_LINE_BIT                                (1 << 3)
	#define DAC2_TO_LINE_BIT                                (1 << 4)
	#define DAC3_TO_LINE_BIT                                (1 << 5)

	#define AUX1_TO_PCM_BIT                                 (1 << 1)
	#define AUX2_TO_PCM_BIT                                 ((1 << 0) | (1 << 4))
	#define AUX3_TO_PCM_BIT                                 (1 << 5)



// General
#define MASK_ALL_ENABLE                                 0xFF
// Micco Gain
#define SPKRL_GAIN_MASK                                 0x3F    //Stereo CH2 PGA
#define SPKRR_GAIN_EN_MASK                              0x80
#define SPKRR_GAIN_MASK                                 0x3F    //Stereo CH1 PGA
#define SPKR_GAIN_EN_MASK                               0xC0

#define MONO_GAIN_MASK                                  0x3F
#define BEAR_GAIN_MASK                                  0x3F
#define BEAR_GAIN_EN_MASK                               0x40
#define LINE_GAIN_MASK                                  0xF

#define SIDETONE_GAIN_MASK                              0x1F
#define AUX3_PGA_GAIN_MASK                              0x03
#define AUX1_PGA_GAIN_MASK                              0x03
#define AUX2_PGA_GAIN_MASK                              0x30

//MIC PGA...
#define MICBIAS_INT_EN_BIT                                      (1 << 5)
#define MICBIAS_EXT_EN_BIT                                      (1 << 6)
#define MIC1_SEL_BIT                                    (0 << 4)
#define MIC2_SEL_BIT                                    (1 << 4)
#define MIC_SEL_MASK                                    0x10
#define MIC_GAIN_EN_BIT                                 (1 << 3)
#define MIC_GAIN_MASK                                   0x07

//TX_PGA_MUX
#define MIC_CONNECTION_BIT                              (3 << 2)

//TX_PGA
#define TX_PGA_EN_BIT                                   (1 << 5)
#define TX_PGA_GAIN_MASK                                0x1E

// HDAC
#define HDAC_ON                                                 0x80
//VADC
#define VADC_EN_BIT                                             (1 << 0)
#define VADC_GAIN_MASK                                  0x30

//MONO_VOL
#define MONO_EN_BIT                                             (1 << 6)
//LINE_OUT_VOL
#define LINE_EN_BIT                                             (1 << 4)
//VDAC
#define VDAC_EN_BIT                                             (1 << 3)
//Side Tone PGA Enable Bit
#define ST_EN_BIT                                               (1 << 7)
//AUX3 PGA Gain Enable Bit
#define AUX3_PGA_EN_BIT                                 (1 << 3)
//AUX1 PGA Gain Enable Bit
#define AUX1_PGA_EN_BIT                                 (1 << 3)
//AUX2 PGA Gain Enable Bit
#define AUX2_PGA_EN_BIT                                 (1 << 7)

/* CODECTRSTN register */                       // activate audio codec internal reset signal
#define RESET_ON_OFF                  0x0001    // audio codec reset : 0 - reset is off, 1 - reset is on

/* CODECNTRL register */                        // activate the blocks of the AFE and audio codec voltage & currents
#define INTERNAL_VLTG_REF_AFE         0x0001    // activation :  0 - block inactive, 1 - active
#define INT_REF_BUFFER_AFE                0x0002
#define CURRENT_REF_AFE                           0x0004
#define REF_GENERATOR                         0x0008

/* CODEC_CONFIG register */                     // configures the operation modes of audio codec
#define AUDIO_CODEC_MODE              0x0001    // select :  0 - PCM mode, 1 - HiFi MODE
//DONT CARE BYTE
#define I2S_INTERFACE_RATE_MASK       0x001C
#define I2S_INTERFACE_RATE_SHIFT           2                                                                                                    // select : 000,001,010,011,100,101,110,111   for differernt rates
#define ADC1_CLOCK_SOURCE_2           0x0020                                                                                                    // select : 0 - I2S_SYSCLK{DEFAULT} , 1 - I2S_SYSCLK2
#define DIGITAL_LOOP_BACK             0x0040                                                                                                    // enable : 0 - loop back disabled , 1 - loop back enabled
#define I2S2_INTERFACE_RATE_MASK      0x0380
#define I2S2_INTERFACE_RATE_SHIFT          7                                                                                                    // select : 000 - 8kHZ,001 - 11.025kHZ,010 - 16kHZ,011 - 22.5kHZ,
#define DAC3_INPUT_CONNECTION_MASK    0x0C00                                                                                                    //          100 - 44.1kHZ, 101,110,111 - 48kHZ
#define DAC3_INPUT_CONNECTION_PCM     0x0C00                                                                                                    // select : 00,11 - pcm ,01 - tone , 10 - tone + pcm
#define AUDIO_CODEC_CONFIG_MODE_MASK  0x3000                                                                                                    // select : 00,11 - single codec mode ,01 - dual codec mode , 10 -  dual codec single clock mode
#define AUDIO_CODEC_DUAL_MODE         0x1000                                                                                                    // 01 - dual codec mode
#define I2S_INTERFACE_TRI_STATE       0x4000                                                                                                    // select : 0 - normal operation , 1 - tri - state
#define I2S2_INTERFACE_TRI_STATE      0x8000                                                                                                    // select : 0 - normal operation , 1 - tri - state

/* CTXCNTRL register */                                                                                                                         // activates the blocks of the audio codec transmit path
#define  MICAMP1                      0x0001                                                                                                    // activation :  0 - block inactive, 1 - active
#define  PGA1_AND_COMB1               0x0002                                                                                                    // activation :  0 - block inactive, 1 - active
#define  PGA2                         0x0004                                                                                                    // activation :  0 - block inactive, 1 - active
#define  ADC1                         0x0008                                                                                                    // activation :  0 - block inactive, 1 - active

/* CTX_CONFIG register */                                                                                                                       // configures the multiplexers of the audio codec  transmit path
#define  TX_1_PIN_CONNECT                     0x0001                                                                                            // configure :  0 -   TX_1 pin is not connected, 1 - TX_1 pin is connected
#define  MIC2_N_PIN_CONNECT                   0x0002                                                                                            // configure :  0 - MIC2_N pin is not connected, 1 - MIC2_N pin is connected
#define  MIC1_N_PIN_CONNECT                   0x0004                                                                                            // configure :  0 - MIC1_N pin is not connected, 1 - MIC1_N pin is connected
#define  MICAMP1_INPUT_CONNECT_TO_MIC1_MASK   0x0018
#define  MICAMP1_INPUT_DIFF_CONNECT_TO_MIC1   0x0008                                                                                            // configure :  00,11 - no connection ,01 - diff input , 10 - single ended input
#define  MICAMP1_INPUT_SNGL_CONNECT_TO_MIC1       0x0010
#define  MICAMP1_INPUT_CONNECT_TO_MIC2_MASK   0x0060
#define  MICAMP1_INPUT_DIFF_CONNECT_TO_MIC2   0x0020    // configure :  00,11 - no connection ,01 - diff input , 10 - single ended input
#define  MICAMP1_INPUT_SNGL_CONNECT_TO_MIC2   0x0040
#define  PGA2_OUT_CONNECT                     0x0080    // configure :  0 - output is not connected, 1 - output is connected

/* CTX_GAIN register */                                 // configures the gain of the audioo codec transmit path
#define PGA1_GAIN_MASK                            0x001F
#define PGA1_GAIN_SHIFT                    0            // select : 5 figures values 00000 to 11111
#define PGA2_GAIN_MASK                0x01E0
#define PGA2_GAIN_SHIFT                    5            // select : 4 figures values 0000 to 1111

/* CHFCNTRL register */                                 // activates the blocks of the audio codec HIFI path
#define  DAC1                         0x0001            // activation :  0 - block inactive, 1 - active
#define  DAC2                         0x0002            // activation :  0 - block inactive, 1 - active
#define  PGA3                         0x0004            // activation :  0 - block inactive, 1 - active

/* CHF_CONFIG */                                        // configures the multiplexers of the audio codec  HIFI path
#define PGA3_OUTPUT_CONNECT_TO_COMB_OR_PGA7   0x0001    // configure :  0 - output is not connected, 1 - output is connected
#define DAC1_OUTPUT_CONNECT                   0x0002    // configure :  0 - output is not connected, 1 - output is connected
#define DAC2_OUTPUT_CONNECT                   0x0004    // configure :  0 - output is not connected, 1 - output is connected
#define AUX_L_PIN_CONNECT                     0x0008    // configure :  0 - AUX_L pin is not connected, 1 - AUX_L pin is connected
#define AUX_R_PIN_CONNECT                     0x0010    // configure :  0 - AUX_R pin is not connected, 1 - AUX_R pin is connected
#define PGA3_OUTPUT_CONNECT_TO_COMB_OR_PGA8   0x0020    // configure :  0 - output is not connected, 1 - output is connected

/* CRXGAIN register */                                  // configures the gain of the audio codec receive path
#define PGA3_GAIN_MASK                0x0007
#define PGA3_GAIN_SHIFT                    0            // select : 3 figures values 000 to 111

/* CRXCNTRL register */                                 // activates the blocks of the audio codec receive path
#define COMB_PGA7                     0x0001            // activation :  0 - block inactive, 1 - active
#define COMB_PGA8                     0x0002            // activation :  0 - block inactive, 1 - active
#define COMB3                         0x0004            // activation :  0 - block inactive, 1 - active
#define PGA4                          0x0008            // activation :  0 - block inactive, 1 - active
#define PGA5                          0x0010            // activation :  0 - block inactive, 1 - active
#define PGA6                          0x0020            // activation :  0 - block inactive, 1 - active
#define AMP1                          0x0040            // activation :  0 - block inactive, 1 - active
#define COMB2                         0x0080            // activation :  0 - block inactive, 1 - active
#define SOFTMUTE                      0x0100            // activation :  0 - block inactive, 1 - active

/* CRX_CONFIG register */                               // configures the multiplexers of the  audio codec receive path
#define _6DB_ATTEN_1_OUTPUT_CONNECT   0x0001            // configure [MUX18]:  0 - output is not connected, 1 - output is connected
#define _6DB_ATTEN_2_OUTPUT_CONNECT   0x0002            // configure [MUX19]:  0 - output is not connected, 1 - output is connected
#define COMB3_BYPASS_CONNECT          0x0004            // configure :  0 - BAYPASS is not connected, 1 - BAYPASS is connected
#define BIT_3                         0x0008            // DOING NOTHING JUST BEING 0
#define PGA4_INPUT_CONNECT_MASK       0x0030
#define PGA4_INPUT_CONNECT_BUS        0x0010            // configure :  00,11 - no connection ,01 - connect to MUX17   10 - connect to COMB/PGA7
#define PGA4_INPUT_CONNECT_PGA7           0x0020
#define PGA5_INPUT_CONNECT_MASK       0x00C0
#define PGA5_INPUT_CONNECT_BUS        0x0040    // configure :  00,11 - no connection ,01 - connect to MUX17   10 - connect to COMB/PGA8
#define PGA5_INPUT_CONNECT_PGA8           0x0080
#define PGA6_INPUT_CONNECT            0x0100    // configure :  0 - input is not connected, 1 - input is connected
#define AMP1_INPUT_CONNECT            0x0200    // configure :  0 - input is not connected, 1 - input is connected
#define COMB2_INPUT_CONNECT           0x0400    // configure :  0 - input is not connected, 1 - input is connected


/* CRXGAIN1 register */                                         // configures the gain for part of the audio codec receive path
#define PGA7_GAIN_MASK                0x001F
#define PGA7_GAIN_SHIFT                    0                    // select : 5 figures values 00000 to 11111
#define PGA8_GAIN_MASK                0x03E0
#define PGA8_GAIN_SHIFT                    5                    // select : 5 figures values 00000 to 11111

/* CRXGAIN2 register */                                         // configures the gain for part of the audio codec receive path
#define PGA4_GAIN_MASK                0x000F
#define PGA4_GAIN_SHIFT                    0                    // select : 4 figures values 0000 to 1111
#define PGA5_GAIN_MASK                            0x00F0
#define PGA5_GAIN_SHIFT                    4                    // select : 4 figures values 0000 to 1111
#define PGA6_GAIN_MASK                            0x0F00
#define PGA6_GAIN_SHIFT                    8                    // select : 4 figures values 0000 to 1111

/* CRXSFMUT register */                                         // configures the soft mute of the audio codec receive path
#define PGA4_MUTE                             0x0001            // enable : 0 - soft mute disabled , 1 - soft mute enabled
#define PGA5_MUTE                                 0x0002        // enable : 0 - soft mute disabled , 1 - soft mute enabled
#define PGA6_MUTE                                 0x0004        // enable : 0 - soft mute disabled , 1 - soft mute enabled
#define AMP1_MUTE                                 0x0008        // enable : 0 - soft mute disabled , 1 - soft mute enabled
#define PGA4_PLUS_INPUT                           0x0010        // enable : 0 - connection to ground , 1 - connection to common mode voltage
#define PGA5_PLUS_INPUT                           0x0020        // enable : 0 - connection to ground , 1 - connection to common mode voltage
#define PGA6_PLUS_INPUT                           0x0040        // enable : 0 - connection to ground , 1 - connection to common mode voltage
#define AMP1_PLUS_INPUT                           0x0080        // enable : 0 - connection to ground , 1 - connection to common mode voltage
// { corrected by  }
#define EXTERNAL_RAMP_CAPACITOR_DISCHARGED    0x0100            // enable : 1 - capacitor  charged , 0 - capacitor is discharged
#define EXTERNAL_RAMP_CAPACITOR_CHARGED       0x0000
#define DIGITAL_SECTION_OPERTION              0x0200            // enable : 1 - digital section is disabled , 0 - digital section enabled

/* CTONECNTRL register */                                       // activates the blocks of the audio codec tone path
#define TONE_GENERATOR                0x0001                    // activation :  0 - block inactive, 1 - active
#define PGA10                         0x0002                    // activation :  0 - block inactive, 1 - active
#define DAC3_AND_PGA9                 0x0004                    // activation :  0 - block inactive, 1 - active

/* CTONE_CONFIG register */                                     // configures the operation modes of the audio codec tone path
#define OPERATION_MODE_TONE_GENERATOR_MASK    0x0007            // select :  000,101,110,111 - melody mode , 001 - DTMF mode
//			 010 - designation tone mode, 011 - warble mode, PWM mode
#define DAC3_OUTPUT_CONNECT_TO_COMB2          0x0008            // select :  0 - output is not connected, 1 - output is connected
#define DAC3_OUTPUT_CONNECT_TO_COMB_PGA7      0x0010            // select :  0 - output is not connected, 1 - output is connected

/* CTONEGAIN register */                                        // configures the gain of the audio codec tone path
#define PGA10_GAIN_MASK               0x001F
#define PGA10_GAIN_SHIFT                   0                    // select : 5 figures values 00000 to 11111
#define PGA9_GAIN_MASK                0x03E0
#define PGA9_GAIN_SHIFT                    5                    // select : 5 figures values 00000 to 11111

/* CMELODY_L register */                                        // configures the operational modes of the audio codec LTONE sound source in "melody" mode
#define LTONE_FREQ_SETTING_MASK       0x003F
#define LTONE_FREQ_SETTING_SHIFT           0                    // select : 6 figures values 000000 to 111111(mute) according to table 25
#define LTONE_WAVE_SHAPE              0x0040                    // select : 0 - Special rectangle wave , 1 - Sine wave
#define LTONE_MULTIPLYING_FACTOR_MASK 0x0380                    // select : 3 figures values : 000 - 1,001 - 0.75,010 - 0.5,011 - 0.375,100 - 0.25
//							   101 - 0.1875,110 - 0.125,111 - 0.0625
/* CMELODY_M register */                                        // configures the operational modes of the audio codec MTONE sound source in "melody" mode
#define MTONE_FREQ_SETTING_MASK       0x003F
#define MTONE_FREQ_SETTING_SHIFT           0                    // select : 6 figures values 000000 to 111111(mute) according to table 25
#define MTONE_WAVE_SHAPE              0x0040                    // select : 0 - Special rectangle wave , 1 - Sine wave
#define MTONE_MULTIPLYING_FACTOR_MASK 0x0380                    // select : 3 figures values : 000 - 1,001 - 0.75,010 - 0.5,011 - 0.375,100 - 0.25
//							   101 - 0.1875,110 - 0.125,111 - 0.0625
/* CMELODY_H register */                                        // configures the operational modes of the audio codec HTONE sound source in "melody" mode
#define HTONE_FREQ_SETTING_MASK       0x003F
#define HTONE_FREQ_SETTING_SHIFT           0                    // select : 6 figures values 000000 to 111111(mute) according to table 25
#define HTONE_WAVE_SHAPE              0x0040                    // select : 0 - Special rectangle wave , 1 - Sine wave
#define HTONE_MULTIPLYING_FACTOR_MASK 0x0380                    // select : 3 figures values : 000 - 1,001 - 0.75,010 - 0.5,011 - 0.375,100 - 0.25
//							   101 - 0.1875,110 - 0.125,111 - 0.0625
/* CDTMF    register */                                         // configures the frequency of the audio codec "DTMF" mode
#define DTMF_FREQ_SETTING_MASK            0x000F
#define DTMF_FREQ_SETTING_SHIFT            0                    // 4 figures values 0000 to 1111(mute) according to table

/* CDESIGNATION  register */                                    // configures the frequency of the audio codec "DESIGNATION" mode
#define DESIGNATION_FREQ_SETTING_MASK 0x000F
#define DESIGNATION_FREQ_SETTING_SHIFT     0                    // 4 figures values 0000 to 1111(mute) according to table

/* CWARBLE    register */                                       // configures the operation modes of the audio codec "WARBLE" mode
#define _16Hz_MODULATION_LEVEL_MASK   0x0007                    // select : 3 figures values : 000 - 40% , 001 - 50% ,010 - 60% ,011 - 70%,
//								111,100,101,110 - mute
#define TONE_FREQ_MASK                0x0018                    // select : 01 - 400Hz tone , 10 - 2KHz tone , 11,00 - mute

/* CRNGRPWM   register */                                       // configures the pulse width duty cycle of the audio codec "PWM" mode.
#define PULSE_WIDE_DUTY_CYCLE_MASK        0x003F
#define PULSE_WIDE_DUTY_CYCLE_SHIFT        0                    // select : 6 figures as mentioned at table

/* CPGD1LR_TEST_REGISTER_GAIN_CONTROL   register */
#define PGD1LR_GAIN_MASK                          0xFFFF
#define PGD1LR_CSD_COEFFICIENTS           0x02B3    //pgd1{l/r} CSD coefficients
/* CPGD3_TEST_REGISTER_GAIN_CONTROL   register */
#define PGD3_GAIN_MASK                            0xFFFF
#define PGD3_CSD_COEFFICIENTS             0x701B                //pgd3      CSD coefficients
/* CPGD_GAIN_CONTROL   register */
#define ENABLE_PGD1LR_TEST_REGISTER_GAIN_CONTROL    0x0060      // pgd1{l/r} is in signal path. Gain is controlled by test gain register address, 0xbe

#define ENABLE_PGD3_TEST_REGISTER_GAIN_CONTROL      0x0100      // pgd 3 v enable using register 0xBF


//ICAT EXPORTED STRUCT
typedef struct
{
	unsigned char order;    /* 0 = disabled, else - place in list*/
	unsigned char volume;   /* 0xFF = disabled */
	ACM_AudioMute muted;
}MICCO_CurrentPath;

//ICAT EXPORTED STRUCT
typedef struct
{
	unsigned char totalEnables;
}MICCO_Database;


//ICAT EXPORTED STRUCT
typedef struct
{
	unsigned char Data; //the way they are arranged data 1 from the right
	unsigned char RegisterAddress;
}MICCO_SingleUpdatePackage;

/* klap - not needed
   typedef enum
   {
	SpkrL_Out_Gain = 0,     //SpkrL PGA     -60 to 15 dB
	SpkrR_Out_Gain,			//SpkrR PGA     -60 to 15 dB
	Bear_Out_Gain,			//Bear PGA              -60 to 15 dB
	Mono_Out_Gain,			//Mono PGA              -60 to 15 dB
	Line_Out_Gain,			//LineOut PGA   -30 to 15 dB
	Aux1_Out_Gain,			//AUX1 PGA                0 to 18 dB
	Aux2_Out_Gain,			//AUX1 PGA                0 to 18 dB
	Aux3_Out_Gain,			//AUX1 PGA                0 to 18 dB
	Mic_In_Gain,			//Mic PGA		  0 to 30 dB
	TxPGA_In_Gain,			//TX-PGA		-18 to 18 dB
	VADC_Txdig_Gain,		//TX-dig		 -6 to 12 dB
	SideTone_Gain,			//SideTone PGA -46.5to  0 dB
	DigitalGain,
	MICCO_MAX_GAINS
   }MICCO_Gains;
 */

// DEFINEING A NUMBER TO EACH REGISITER LATER TO BE USED TO FIND IT'S
// SPECIFIC ADDRESS IN A SEQUENTIAL ARRAY HOLDING THE ADDRESSS

//ICAT EXPORTED ENUM
typedef enum
{

	RX_AUDIO_MUX1 = 0,                //Don't change the order
	RX_AUDIO_MUX2,
	RX_AUDIO_MUX3,
	RX_AUDIO_MUX4,
	RX_AUDIO_MUX5,
	AUDIO_LINE_AMP,
	STEREO_AMP_CH1,
	STEREO_AMP_CH2,
	HDAC_CTRL,
	MONO_VOL,
	BEAR_VOL,
	I2S_CTRL,
	TX_PGA,
	MIC_PGA,
	TX_PGA_MUX,
	VADC_CTRL,
	VDAC_CTRL,
	SIDE_TONE,
	PGA_AUXI1_2,
	PGA_AUXI3,
	PGA_DACS,
	MICCO_REGISTER_AMOUNT          // amount of registers involved in audio

}MICCO_ControlRegister;

//ICAT EXPORTED ENUM
typedef enum
{
	MICCO_FAIL = 0,
	MICCO_OK,
	MICCO_NO_FILE
}MICCO_FdiReadFileStatus;


//ICAT EXPORTED ENUM

typedef enum
{
	ACTIVATE_DEVICE   = 0,
	DEACTIVATE_DEVICE
}MICCO_SoftMuteMode;

//ICAT EXPORTED STRUCT
typedef struct
{
	unsigned char path;
	unsigned char volume;
	ACM_AudioMute muteEnable;
	char    *     data;
	short length;
}MICCO_APIParams;




//ICAT EXPORTED STRUCT
typedef struct
{
	unsigned long gainId;
	unsigned long path;
	signed short Slope;
	signed short Offset;
}MICCO_NvmRecord;


/*%%%%%%%%%%  START VOICE AREA : START %%%%%%%%%%*/

#ifdef   _START_VOICE_FLAG

typedef struct
{
	UINT16 freq;
	UINT16 A1;
	UINT16 B0;
}MICCO_ToneFreqCoefficients;


#endif //_START_VOICE_FLAG

/*%%%%%%%%%%  START VOICE AREA : END %%%%%%%%%%*/



#ifdef __cplusplus
}
#endif

#endif  /* _AUDIO_MICCO_H_ */


