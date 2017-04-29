/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/

/******************************************************************************
*               MODULE IMPLEMENTATION FILE
*******************************************************************************
* Title: audioLevante
*
* Filename: audioLevante.c
*
* Target, platform: SW platform
*
* Authors: Isar Ariel
*
* Description: Levante component
*
* Last Updated:
*
* Notes:
******************************************************************************/

/*----------- Local include files --------------------------------------------*/
#include "audiolevanteapi.h"
#include "audiolevante.h"
#include "fdi_file.h"
#include "nvm_header.h"
#include "FSNotifyManager.h"

/*----------- External include files -----------------------------------------*/
#include "acm.h"
#include "acmtypes.h"
#include <stdlib.h>
#include <assert.h>
/* Linux */
#include <pxa_dbg.h>
#define printf DPRINTF
extern void LevanteI2CWrite(unsigned char I2CRegAddr, unsigned char I2CRegData);



/*----------- Local function prototypes --------------------------------------*/
/*static*/ void        LevanteInit(unsigned char reinit, ACM_SspAudioConfiguration **sspAudioConfiguration, signed short **ditherGenConfig);
static ACM_DigitalGain LevantePathEnable(unsigned char path, ACM_AudioVolume volume);
static ACM_DigitalGain LevantePathDisable(unsigned char path);
static ACM_DigitalGain LevantePathVolumeSet(unsigned char path, ACM_AudioVolume volume);
static ACM_DigitalGain LevantePathMute(unsigned char path, ACM_AudioMute mute);
static ACM_DigitalGain LevanteSetPathsAnalogGain(unsigned char regIndex, unsigned char regValue, unsigned char regMask, unsigned char regShift);
static short           LevanteGetPathsStatus(char* data, short length);
static ACM_AnalogGain  LevanteGetPathAnalogGain(unsigned char path);
static void            LevanteReadFdiData(void);
static ACM_AnalogGain  LevanteGetGainIndB(unsigned char path, LevanteGainsId gainId, unsigned char pathLoc);
static unsigned int    LevanteGetActivePathNum(void);
static unsigned int LevanteSetLoopbackTestStatus(unsigned char isloopbacktest);
static unsigned int LevanteGetLoopbackTestStatus(void);


/*----------- Global extrenal variable definition ----------------------------*/
ACM_ComponentHandle HWLevanteHandle  =
{
	LevanteInit,
	LevantePathEnable,
	LevantePathDisable,
	LevantePathVolumeSet,
	LevantePathMute,
	LevanteGetPathsStatus,
	LevanteGetPathAnalogGain,
	LevanteSetPathsAnalogGain,
	LevanteGetActivePathNum,
	LevanteSetLoopbackTestStatus,
	LevanteGetLoopbackTestStatus
};


/*----------- Local defines  -------------------------------------------------*/
#undef TRANSPARENT
#define TRANSPARENT     0xFF

#define LEVANTE_I2C_WRITE_SLAVE_ADDRESS            0x68
#define LEVANTE_I2C_READ_SLAVE_ADDRESS             0x69

/* Levante Reg: Clock generator control, Address 0x42 */
#define LEVANTE_MISC2_ADDRESS                          0x42
#define LEVANTE_MISC2_IDX                              0
#define LEVANTE_MISC2_DEFAULT_VALUE		   \
	( (1 << AUDIO_PU_SHIFT)   | \
	  (1 << AUDIO_RSTB_SHIFT) | \
	  (1 << PU_PLL_SHIFT) )

#define AUDIO_PU_MASK                                          0x1   /* audio_pu;    bit 3 */
#define AUDIO_PU_SHIFT                                         3

#define AUDIO_RSTB_MASK                                        0x1   /* audio_rstb;  bit 4 */
#define AUDIO_RSTB_SHIFT                               4

#define PU_PLL_MASK                                                    0x1   /* audio_rstb;  bit 5 */
#define PU_PLL_SHIFT                                           5

/* Levante Reg: PLL_CTRL1, Address 0x43 */
#define LEVANTE_PLL_CTRL1_ADDRESS                          0x43
#define LEVANTE_PLL_CTRL1_IDX                      1
#define LEVANTE_PLL_CTRL1_DEFAULT_VALUE            0x18

#define PLL_ICP_MASK                                           0x3   /* pll_ipc;     bits [1:0] */
#define PLL_ICP_SHIFT                                          0

#define MCLK_DIV_MASK                                          0x3   /* mclk_div;    bits [3:2] */
#define MCLK_DIV_SHIFT                                         2

/* Levante Reg: PLL_FRAC1, Address 0x44 */
#define LEVANTE_PLL_FRAC1_ADDRESS                          0x44
#define LEVANTE_PLL_FRAC1_IDX                      2
#define LEVANTE_PLL_FRAC1_DEFAULT_VALUE            0x08

#define PLL_FRAC1_MASK                                         0xFF   /* pll_frac1;  bits [7:0] */
#define PLL_FRAC1_SHIFT                                    0

/* Levante Reg: PLL_FRAC2, Address 0x45 */
#define LEVANTE_PLL_FRAC2_ADDRESS                          0x45
#define LEVANTE_PLL_FRAC2_IDX                      3
#define LEVANTE_PLL_FRAC2_DEFAULT_VALUE            0x82

#define PLL_FRAC2_MASK                                         0xFF   /* pll_frac2;  bits [7:0] */
#define PLL_FRAC2_SHIFT                                    0

/* Levante Reg: PLL_FRAC3, Address 0x46 */
#define LEVANTE_PLL_FRAC3_ADDRESS                          0x46
#define LEVANTE_PLL_FRAC3_IDX                      4
#define LEVANTE_PLL_FRAC3_DEFAULT_VALUE            0

#define PLL_FRAC3_MASK                                         0xF   /* pll_frac3;   bits [3:0] */
#define PLL_FRAC3_SHIFT                                    0

/* Levante Reg: PCM_INTERFACE1, Address 0xB0 */
#define LEVANTE_PCM_INTERFACE1_ADDRESS                 0xB0
#define LEVANTE_PCM_INTERFACE1_IDX                     5
#define LEVANTE_PCM_INTERFACE1_DEFAULT_VALUE       0

#define PCM_OFFS_L_MASK				               0x3F  /* pcm_offs_l;   bits [5:0] */
#define PCM_OFFS_L_SHIFT     		               4

/* For Levante A0 & A1 only */
	#define PCM_OFFSR_Ax_MASK				       0xF   /* pcm_offsr;   bits [3:0] */
	#define PCM_OFFSR_Ax_SHIFT     		           0

	#define PCM_OFFSL_Ax_MASK				       0xF   /* pcm_offsl;   bits [7:4] */
	#define PCM_OFFSL_Ax_SHIFT     		           4
/* End of part - for Levante A0 & A1 only */

/* Levante Reg: PCM_INTERFACE2, Address 0xB1 */
#define LEVANTE_PCM_INTERFACE2_ADDRESS                 0xB1
#define LEVANTE_PCM_INTERFACE2_IDX                 6
#define LEVANTE_PCM_INTERFACE2_DEFAULT_VALUE       0

#define PCM_MODE_MASK                                          0x7   /* pcm_mode;          bits [2:0] */
#define PCM_MODE_SHIFT                                         0

#define PCM_WL_MASK                                                    0x1   /* pcm_wl;            bit 3      */
#define PCM_WL_SHIFT                                           3

#define PCM_MASTER_MASK                                        0x1   /* pcm_master;        bit 4      */
#define PCM_MASTER_SHIFT                                       4

#define PCM_FS_POL_MASK                                        0x1   /* pcm_fs_pol;        bit 5      */
#define PCM_FS_POL_SHIFT                                       5

#define PCM_BCLK_POL_MASK                                      0x1   /* pcm_bclk_pol;      bit 6      */
#define PCM_BCLK_POL_SHIFT                                     6

#define PCM_EXT_LOOPBACK_MASK                          0x1   /* pcm_ext_loopback;  bit 7      */
#define PCM_EXT_LOOPBACK_SHIFT                         7

/* Levante Reg: PCM_INTERFACE3, Address 0xB2 */
#define LEVANTE_PCM_INTERFACE3_ADDRESS                 0xB2
#define LEVANTE_PCM_INTERFACE3_IDX                 7
#define LEVANTE_PCM_INTERFACE3_DEFAULT_VALUE       0

#define HIZ_DOUT_MASK                                          0x1   /* hiz_dout;          bit 1      */
#define HIZ_DOUT_SHIFT                                         1

#define RIGHT_DATSEL_MASK                                      0x3   /* right_datsel;      bits [3:2] */
#define RIGHT_DATSEL_SHIFT                             2

#define LEFT_DATSEL_MASK                                       0x3   /* left_datsel;       bits [5:4] */
#define LEFT_DATSEL_SHIFT                              4

#define PCM_SEL_DATIN_MASK                                     0x1   /* pcm_sel_datin;     bit 6      */
#define PCM_SEL_DATIN_SHIFT                            6

#define PCM_INT_LOOPBACK_MASK                          0x1   /* pcm_int_loopback;  bit 7      */
#define PCM_INT_LOOPBACK_SHIFT                     7

/* Levante Reg: PCM_RATE, Address 0xB3 */
#define LEVANTE_PCM_RATE_ADDRESS                               0xB3
#define LEVANTE_PCM_RATE_IDX                       8
#define LEVANTE_PCM_RATE_DEFAULT_VALUE             0

#define RATE_ADC_PCM_MASK                                                  0xF   /* rate_adc_pcm;  bits [3:0] */
#define RATE_ADC_PCM_SHIFT                                         0

/* Levante Reg: ECHO_CANCEL_PATH, Address 0xB4 */
#define LEVANTE_ECHO_CANCEL_PATH_ADDRESS               0xB4
#define LEVANTE_ECHO_CANCEL_PATH_IDX               9
#define LEVANTE_ECHO_CANCEL_PATH_DEFAULT_VALUE     0

#define EC_DATSEL_MASK                                                     0x3   /* ec_datsel;  bits [2:1] */
#define EC_DATSEL_SHIFT                                            1

/* Levante Reg: SIDETONE_GAIN1, Address 0xB5 */
#define LEVANTE_SIDETONE_GAIN1_ADDRESS                 0xB5
#define LEVANTE_SIDETONE_GAIN1_IDX                 10
#define LEVANTE_SIDETONE_GAIN1_DEFAULT_VALUE       0

#define ST_GAINL_MASK                                                      0x3F  /* st_gainl;  bits [5:0] */
#define ST_GAINL_SHIFT                                                     0

/* Levante Reg: SIDETONE_GAIN2, Address 0xB6 */
#define LEVANTE_SIDETONE_GAIN2_ADDRESS                 0xB6
#define LEVANTE_SIDETONE_GAIN2_IDX                 11
#define LEVANTE_SIDETONE_GAIN2_DEFAULT_VALUE       0

#define ST_GAINR_MASK                                                      0x3F  /* st_gainr;  bits [5:0] */
#define ST_GAINR_SHIFT                                                     0

/* Levante Reg: SIDETONE_GAIN3, Address 0xB7 */
#define LEVANTE_SIDETONE_GAIN3_ADDRESS                 0xB7
#define LEVANTE_SIDETONE_GAIN3_IDX                 12
#define LEVANTE_SIDETONE_GAIN3_DEFAULT_VALUE       0

#define ST_SHIFTR_MASK                                                     0xF   /* st_shiftr;  bits [3:0] */
#define ST_SHIFTR_SHIFT                                            0

#define ST_SHIFTL_MASK                                                     0xF   /* st_shiftl;  bits [7:4] */
#define ST_SHIFTL_SHIFT                                            4

/* Levante Reg: ADC_OFFSET1, Address 0xB8 */
#define LEVANTE_ADC_OFFSET1_ADDRESS                        0xB8
#define LEVANTE_ADC_OFFSET1_IDX                    13
#define LEVANTE_ADC_OFFSET1_DEFAULT_VALUE              0

#define OFFSET_L_MASK                                          0xFF  /* offset_l;  bits [7:0] */
#define OFFSET_L_SHIFT                                     0

/* Levante Reg: ADC_OFFSET2, Address 0xB9 */
#define LEVANTE_ADC_OFFSET2_ADDRESS                        0xB9
#define LEVANTE_ADC_OFFSET2_IDX                    14
#define LEVANTE_ADC_OFFSET2_DEFAULT_VALUE              0

#define OFFSET_R_MASK                                          0xFF  /* offset_r;  bits [7:0] */
#define OFFSET_R_SHIFT                                     0

/* Levante Reg: DMIC_DELAY, Address 0xBA */
#define LEVANTE_DMIC_DELAY_ADDRESS                         0xBA
#define LEVANTE_DMIC_DELAY_IDX                     15
#define LEVANTE_DMIC_DELAY_DEFAULT_VALUE               0

#define DMIC_CLOCK_SEL_MASK                                        0xF   /* dmic_clock_sel;  bits [3:0] */
#define DMIC_CLOCK_SEL_SHIFT                               0

#define DMIC_DATA_SEL_MASK                                         0xF   /* dmic_data_sel;   bits [7:4] */
#define DMIC_DATA_SEL_SHIFT                                4

/* Levante Reg: I2S_INTERFACE1, Address 0xBB */
#define LEVANTE_I2S_INTERFACE1_ADDRESS                 0xBB
#define LEVANTE_I2S_INTERFACE1_IDX                 16
#define LEVANTE_I2S_INTERFACE1_DEFAULT_VALUE       0

#define I2S_OFFS_L_MASK	   			               0x3F  /* i2s_offs_l;  bits [5:0] */
#define I2S_OFFS_L_SHIFT     		               0

/* For Levante A0 & A1 only */
	#define I2S_OFFSR_Ax_MASK	   			       0xF   /* i2s_offsr;  bits [3:0] */
	#define I2S_OFFSR_Ax_SHIFT     		           0

	#define I2S_OFFSL_Ax_MASK	   			       0xF   /* i2s_offsl;  bits [7:4] */
	#define I2S_OFFSL_Ax_SHIFT     		           4
/* End of part - for Levante A0 & A1 only */

/* Levante Reg: I2S_INTERFACE2, Address 0xBC */
#define LEVANTE_I2S_INTERFACE2_ADDRESS                 0xBC
#define LEVANTE_I2S_INTERFACE2_IDX                 17
#define LEVANTE_I2S_INTERFACE2_DEFAULT_VALUE	   \
	((1 << I2S_MASTER_SHIFT) | \
	 (1 << I2S_MODE_SHIFT))                                           /* I2S Master, Exact I2S mode */

#define GENERAL_I2S 0
#define EXACT_I2S 1

#define I2S_MODE_MASK                                          0x7   /* i2s_mode;          bits [2:0]*/
#define I2S_MODE_SHIFT                                         0

#define I2S_WL_MASK                                                0x1   /* i2s_wl;            bit 3     */
#define I2S_WL_SHIFT                                           3

#define I2S_MASTER_MASK                                        0x1   /* i2s_master;        bit 4     */
#define I2S_MASTER_SHIFT                               4

#define I2S_FS_POL_MASK                                        0x1   /* i2s_fs_pol;        bit 5     */
#define I2S_FS_POL_SHIFT                               5

#define I2S_BCLK_POL_MASK                                      0x1   /* i2s_bclk_pol;      bit 6     */
#define I2S_BCLK_POL_SHIFT                             6

#define I2S_EXT_LOOPBACK_MASK                          0x1   /* i2s_ext_loopback;  bit 7     */
#define I2S_EXT_LOOPBACK_SHIFT                         7

/* Levante Reg: I2S_INTERFACE3, Address 0xBD */
#define LEVANTE_I2S_INTERFACE3_ADDRESS                 0xBD
#define LEVANTE_I2S_INTERFACE3_IDX                 18
#define LEVANTE_I2S_INTERFACE3_DEFAULT_VALUE       0

#define I2S_IN2SEL_MASK                                            0x1   /* i2s_in2sel;        bit 1 */
#define I2S_IN2SEL_SHIFT                                   1

#define I2S_MICDATS_MASK                                           0x1   /* i2s_micdats;       bit 4 */
#define I2S_MICDATS_SHIFT                                  4

#define HIZ_I2S_DOUT_MASK                                  0x1   /* hiz_i2s_dout;      bit 5 */
#define HIZ_I2S_DOUT_SHIFT                                 5

#define I2S_INT_LOOPBACK_MASK                              0x1   /* i2s_int_loopback;  bit 7 */
#define I2S_INT_LOOPBACK_SHIFT                             7

/* Levante Reg: I2S_INTERFACE4, Address 0xBE */
#define LEVANTE_I2S_INTERFACE4_ADDRESS                 0xBE
#define LEVANTE_I2S_INTERFACE4_IDX                 19
#define LEVANTE_I2S_INTERFACE4_DEFAULT_VALUE        0 //Raul: bypass DWA will cause noise when volume is low in voice call (1 << DWA_BYP_SHIFT)

#define RATE_DAC_I2S_MASK                                          0xF   /* rate_dac_i2s;  bits [3:0] */
#define RATE_DAC_I2S_SHIFT                                 0

#define EQU_BYP_MASK                                               0x1   /* equ_byp;       bit 6      */
#define EQU_BYP_SHIFT                                              6

#define DWA_BYP_MASK                                               0x1   /* dwa_byp;	   bit 7      */
#define DWA_BYP_SHIFT                                              7

/* Levante Reg: Equalizer_N0_1, Address 0xBF */
#define LEVANTE_EQUALIZER_N0_1_ADDRESS                 0xBF
#define LEVANTE_EQUALIZER_N0_1_IDX                 20
#define LEVANTE_EQUALIZER_N0_1_DEFAULT_VALUE       0

#define EQ_N0_1_MASK                                           0xFF  /* eq_n0_1;  bits [7:0] */
#define EQ_N0_1_SHIFT                                          0

/* Levante Reg: Equalizer_N0_2, Address 0xC0 */
#define LEVANTE_EQUALIZER_N0_2_ADDRESS                 0xC0
#define LEVANTE_EQUALIZER_N0_2_IDX                 21
#define LEVANTE_EQUALIZER_N0_2_DEFAULT_VALUE       0

#define EQ_N0_2_MASK                                           0xFF  /* eq_n0_2;  bits [7:0] */
#define EQ_N0_2_SHIFT                                          0

/* Levante Reg: Equalizer_N1_1, Address 0xC1 */
#define LEVANTE_EQUALIZER_N1_1_ADDRESS                 0xC1
#define LEVANTE_EQUALIZER_N1_1_IDX                 22
#define LEVANTE_EQUALIZER_N1_1_DEFAULT_VALUE       0

#define EQ_N1_1_MASK                                           0xFF  /* eq_n1_1;  bits [7:0] */
#define EQ_N1_1_SHIFT                                          0

/* Levante Reg: Equalizer_N1_2, Address 0xC2 */
#define LEVANTE_EQUALIZER_N1_2_ADDRESS                 0xC2
#define LEVANTE_EQUALIZER_N1_2_IDX                 23
#define LEVANTE_EQUALIZER_N1_2_DEFAULT_VALUE       0

#define EQ_N1_2_MASK                                           0xFF  /* eq_n1_2;  bits [7:0] */
#define EQ_N1_2_SHIFT                                          0

/* Levante Reg: Equalizer_D1_1, Address 0xC3 */
#define LEVANTE_EQUALIZER_D1_1_ADDRESS                 0xC3
#define LEVANTE_EQUALIZER_D1_1_IDX                 24
#define LEVANTE_EQUALIZER_D1_1_DEFAULT_VALUE       0

#define EQ_D1_1_MASK                                           0xFF  /* eq_d1_1;  bits [7:0] */
#define EQ_D1_1_SHIFT                                          0

/* Levante Reg: Equalizer_D1_2, Address 0xC4 */
#define LEVANTE_EQUALIZER_D1_2_ADDRESS                 0xC4
#define LEVANTE_EQUALIZER_D1_2_IDX                 25
#define LEVANTE_EQUALIZER_D1_2_DEFAULT_VALUE       0

#define EQ_D1_2_MASK                                           0xFF  /* eq_d1_2;  bits [7:0] */
#define EQ_D1_2_SHIFT                                          0

/* Levante Reg: SIDE_TONE1, Address 0xC5 */
#define LEVANTE_SIDE_TONE1_ADDRESS                                 0xC5
#define LEVANTE_SIDE_TONE1_IDX                     26
#define LEVANTE_SIDE_TONE1_DEFAULT_VALUE                   0x3F

#define LOFI_GAIN_LEFT_MASK                                                0x3F  /* lofi_gain_left;  bits [5:0] */
#define LOFI_GAIN_LEFT_SHIFT                                       0

/* Levante Reg: SIDE_TONE2, Address 0xC6 */
#define LEVANTE_SIDE_TONE2_ADDRESS                                 0xC6
#define LEVANTE_SIDE_TONE2_IDX                     27
#define LEVANTE_SIDE_TONE2_DEFAULT_VALUE                   0x3F

#define LOFI_GAIN_RIGHT_MASK                                       0x3F  /* lofi_gain_right;  bits [5:0] */
#define LOFI_GAIN_RIGHT_SHIFT                                      0

/* Levante Reg: LEFT_GAIN1, Address 0xC7 */
#define LEVANTE_LEFT_GAIN1_ADDRESS                         0xC7
#define LEVANTE_LEFT_GAIN1_IDX                     28
#define LEVANTE_LEFT_GAIN1_DEFAULT_VALUE               0x3F

#define HIFIL_GAIN_LEFT_MASK                                       0x3F  /* hifil_gain_left;  bits [5:0] */
#define HIFIL_GAIN_LEFT_SHIFT                                      0

/* Levante Reg: LEFT_GAIN2, Address 0xC8 */
#define LEVANTE_LEFT_GAIN2_ADDRESS                         0xC8
#define LEVANTE_LEFT_GAIN2_IDX                     29
#define LEVANTE_LEFT_GAIN2_DEFAULT_VALUE               0x3F

#define HIFIL_GAIN_RIGHT_MASK                                      0x3F  /* hifil_gain_right;  bits [5:0] */
#define HIFIL_GAIN_RIGHT_SHIFT                                     0

/* Levante Reg: RIGHT_GAIN1, Address 0xC9 */
#define LEVANTE_RIGHT_GAIN1_ADDRESS                        0xC9
#define LEVANTE_RIGHT_GAIN1_IDX                    30
#define LEVANTE_RIGHT_GAIN1_DEFAULT_VALUE              0x3F

#define HIFIR_GAIN_LEFT_MASK                                       0x3F  /* hifir_gain_left;  bits [5:0] */
#define HIFIR_GAIN_LEFT_SHIFT                                      0

/* Levante Reg: RIGHT_GAIN2, Address 0xCA */
#define LEVANTE_RIGHT_GAIN2_ADDRESS                        0xCA
#define LEVANTE_RIGHT_GAIN2_IDX                    31
#define LEVANTE_RIGHT_GAIN2_DEFAULT_VALUE              0x3F

#define HIFIR_GAIN_RIGHT_MASK                                      0x3F  /* hifir_gain_right;  bits [5:0] */
#define HIFIR_GAIN_RIGHT_SHIFT                                     0

/* Levante Reg: DAC_OFFSET, Address 0xCB */
#define LEVANTE_DAC_OFFSET_ADDRESS                         0xCB
#define LEVANTE_DAC_OFFSET_IDX                     32
#define LEVANTE_DAC_OFFSET_DEFAULT_VALUE               0

#define MUTE_RIGHT_MASK                                                    0x1   /* mute_right;  bit 2 */
#define MUTE_RIGHT_SHIFT                                           2

#define MUTE_LEFT_MASK                                                     0x1   /* mute_left;   bit 6 */
#define MUTE_LEFT_SHIFT                                            6

/* Levante Reg: OFFSET_LEFT1, Address 0xCC */
#define LEVANTE_OFFSET_LEFT1_ADDRESS                   0xCC
#define LEVANTE_OFFSET_LEFT1_IDX                   33
#define LEVANTE_OFFSET_LEFT1_DEFAULT_VALUE             0

#define OFFSET_L_LSB_MASK                                                  0xFF  /* offset_l_lsb;  bits [7:0] */
#define OFFSET_L_LSB_SHIFT                                         0

/* Levante Reg: OFFSET_LEFT2, Address 0xCD */
#define LEVANTE_OFFSET_LEFT2_ADDRESS                   0xCD
#define LEVANTE_OFFSET_LEFT2_IDX                   34
#define LEVANTE_OFFSET_LEFT2_DEFAULT_VALUE         0

#define OFFSET_L_MSB_MASK                                                  0xFF  /* offset_l_msb;  bits [15:8] */
#define OFFSET_L_MSB_SHIFT                                         8

/* Levante Reg: OFFSET_RIGHT1, Address 0xCE */
#define LEVANTE_OFFSET_RIGHT1_ADDRESS                  0xCE
#define LEVANTE_OFFSET_RIGHT1_IDX                  35
#define LEVANTE_OFFSET_RIGHT1_DEFAULT_VALUE            0

#define OFFSET_R_LSB_MASK                                                  0xFF  /* offset_r_lsb;  bits [7:0] */
#define OFFSET_R_LSB_SHIFT                                         0

/* Levante Reg: OFFSET_RIGHT2, Address 0xCF */
#define LEVANTE_OFFSET_RIGHT2_ADDRESS                  0xCF
#define LEVANTE_OFFSET_RIGHT2_IDX                  36
#define LEVANTE_OFFSET_RIGHT2_DEFAULT_VALUE            0

#define OFFSET_R_MSB_MASK                                                  0xFF  /* offset_r_msb;  bits [15:8] */
#define OFFSET_R_MSB_SHIFT                                         8

/* Levante Reg: ADC_ANALOG_PROGRAM1, Address 0xD0 */
#define LEVANTE_ADC_ANALOG_PROGRAM1_ADDRESS                0xD0
#define LEVANTE_ADC_ANALOG_PROGRAM1_IDX            37
/*FIXME: Raul: workaround for Levante A1, need to set sense switch on for Mic2 to enable headset (Mic2 workaround) detection.
   To workaround this, in kenerl we set 0xd0 to 0x3c, here we set 0xd0 t0 0x64. Donot know why*/
#define LEVANTE_ADC_ANALOG_PROGRAM1_DEFAULT_VALUE  0x64

#define BYPASS_PGA_MIC1_MASK                                       0x1   /* bypass_pga_mic1;  bit 0 */
#define BYPASS_PGA_MIC1_SHIFT                                      0

#define BYPASS_PGA_MIC3_MASK                                       0x1   /* bypass_pga_mic3;  bit 1 */
#define BYPASS_PGA_MIC3_SHIFT                                      1

#define EN_MICBIAS1_2_MASK                                                 0x1   /* en_micbias1_2;    bit 2 */
#define EN_MICBIAS1_2_SHIFT                                        2

#define EN_MICBIAS1_PAD_MASK                                       0x3   /* en_micbias1_pad;  bits [4:3] */
#define EN_MICBIAS1_PAD_SHIFT                                      3

#define EN_MICBIAS2_PAD_MASK                                       0x3   /* en_micbias2_pad;  bits [6:5] */
#define EN_MICBIAS2_PAD_SHIFT                                      5

#define EN_MICBIAS3_MASK                                                   0x1   /* en_micbias3;      bit 7 */
#define EN_MICBIAS3_SHIFT                                          7

/* Levante Reg: ADC_ANALOG_PROGRAM2, Address 0xD1 */
#define LEVANTE_ADC_ANALOG_PROGRAM2_ADDRESS                0xD1
#define LEVANTE_ADC_ANALOG_PROGRAM2_IDX            38
#define LEVANTE_ADC_ANALOG_PROGRAM2_DEFAULT_VALUE  0

#define GAIN_PGA_MIC1_MASK                                                 0x7   /* gain_pga_mic1;  bits [2:0] */
#define GAIN_PGA_MIC1_SHIFT                                        0

#define GAIN_PGA_MIC2_MASK                                                 0x7   /* gain_pga_mic2;  bits [5:3] */
#define GAIN_PGA_MIC2_SHIFT                                        3

#define GAIN_MOD1_MASK                                                 0x3   /* gain_mod1;      bits [7:6] */
#define GAIN_MOD1_SHIFT                                        6

/* Levante Reg: ADC_ANALOG_PROGRAM3, Address 0xD2 */
#define LEVANTE_ADC_ANALOG_PROGRAM3_ADDRESS                0xD2
#define LEVANTE_ADC_ANALOG_PROGRAM3_IDX            39
#define LEVANTE_ADC_ANALOG_PROGRAM3_DEFAULT_VALUE  0

#define GAIN_PGA_AUX1_MASK                                                 0x7   /* gain_pga_aux1;  bits [2:0] */
#define GAIN_PGA_AUX1_SHIFT                                        0

#define GAIN_PGA_AUX2_MASK                                                 0x7   /* gain_pga_aux1;  bits [5:3] */
#define GAIN_PGA_AUX2_SHIFT                                        3

#define GAIN_MOD2_MASK                                                     0x3   /* gain_mod2;      bits [7:6] */
#define GAIN_MOD2_SHIFT                                            6

/* Levante Reg: ADC_ANALOG_PROGRAM4, Address 0xD3 */
#define LEVANTE_ADC_ANALOG_PROGRAM4_ADDRESS                0xD3
#define LEVANTE_ADC_ANALOG_PROGRAM4_IDX            40
#define LEVANTE_ADC_ANALOG_PROGRAM4_DEFAULT_VALUE  (1 << AUDIO_ZLOAD_4OHM_SHIFT) |  \
					(3 << MBC1_2_SHIFT)         /*575 uA | 8 ohm*/
#define MBC1_2_MASK                                                                0x3          /* mbc1_2;            bits [1:0] */
#define MBC1_2_SHIFT                                                       0

#define MBC3_MASK                                                                  0x3   /* mbc3;              bits [3:2] */
#define MBC3_SHIFT                                                         2

#define MIC1_MIC2_SEL_MASK                                                 0x1   /* mic1_mic2_sel;     bit 4      */
#define MIC1_MIC2_SEL_SHIFT                                        4

#define AUDIO_ZLOAD_4OHM_MASK                                      0x1   /* audio_zload_4ohm;  bit 5      */
#define AUDIO_ZLOAD_4OHM_SHIFT                                     5

/* Levante Reg: ANALOG_TO_ANALOG, Address 0xD4 */
#define LEVANTE_ANALOG_TO_ANALOG_ADDRESS                   0xD4
#define LEVANTE_ANALOG_TO_ANALOG_IDX               41
#define LEVANTE_ANALOG_TO_ANALOG_DEFAULT_VALUE     \
							( (1 << PGA_TO_PA_SEL1_SHIFT) | \
							(1 << PGA_TO_PA_SEL2_SHIFT) )


#define HS1_SEL_ANADIG_IN_MASK                                     0x1   /* hs1_sel_anadig_in;  bit 0 */
#define HS1_SEL_ANADIG_IN_SHIFT                            0

#define HS2_SEL_ANADIG_IN_MASK                                     0x1   /* hs2_sel_anadig_in;  bit 1 */
#define HS2_SEL_ANADIG_IN_SHIFT                            1

#define LO1_SEL_ANADIG_IN_MASK                                     0x1   /* lo1_sel_anadig_in;  bit 2 */
#define LO1_SEL_ANADIG_IN_SHIFT                            2

#define LO2_SEL_ANADIG_IN_MASK                                     0x1   /* lo2_sel_anadig_in;  bit 3 */
#define LO2_SEL_ANADIG_IN_SHIFT                            3

#define PGA_TO_PA_SEL1_MASK                                                0x1   /* pga_to_pa_sel1;     bit 4 */
#define PGA_TO_PA_SEL1_SHIFT                                       4

#define PGA_TO_PA_SEL2_MASK                                                0x1   /* pga_to_pa_sel2;     bit 5 */
#define PGA_TO_PA_SEL2_SHIFT                                       5

#define SEL_SP_EA_MASK                                                     0x1   /* sel_sp_ea;          bit 6 */
#define SEL_SP_EA_SHIFT                                            6

/* Levante Reg: HS1_CTRL, Address 0xD5 */
#define LEVANTE_HS1_CTRL_ADDRESS                           0xD5
#define LEVANTE_HS1_CTRL_IDX                       42
#define LEVANTE_HS1_CTRL_DEFAULT_VALUE             0

#define HS1_GAIN_MASK                                                      0x7   /* hs1_gain;   bits [2:0] */
#define HS1_GAIN_SHIFT                                                     0

#define HS1_IBIAS3_MASK                            0x3   /* hs1_ibias3; bits [4:3] */
#define HS1_IBIAS3_SHIFT                           3

#define HS1_IOP1_MASK                              0x3   /* hs1_iop1;   bits [6:5] */
#define HS1_IOP1_SHIFT                             5

/* Levante Reg: HS2_CTRL, Address 0xD6 */
#define LEVANTE_HS2_CTRL_ADDRESS                           0xD6
#define LEVANTE_HS2_CTRL_IDX                       43
#define LEVANTE_HS2_CTRL_DEFAULT_VALUE             0

#define HS2_GAIN_MASK                                                      0x7   /* hs2_gain;   bits [2:0] */
#define HS2_GAIN_SHIFT                                                     0

#define HS2_IBIAS3_MASK                            0x3   /* hs2_ibias3; bits [4:3] */
#define HS2_IBIAS3_SHIFT                           3

#define HS2_IOP1_MASK                              0x3   /* hs2_iop1l;  bits [6:5] */
#define HS2_IOP1_SHIFT                             5

/* Levante Reg: LO1_CTRL, Address 0xD7 */
#define LEVANTE_LO1_CTRL_ADDRESS                           0xD7
#define LEVANTE_LO1_CTRL_IDX                       44
#define LEVANTE_LO1_CTRL_DEFAULT_VALUE             0

#define LO1_GAIN_MASK                                                      0x7   /* lo1_gain;  bits [2:0] */
#define LO1_GAIN_SHIFT                                                     0

/* Levante Reg: LO2_CTRL, Address 0xD8 */
#define LEVANTE_LO2_CTRL_ADDRESS                           0xD8
#define LEVANTE_LO2_CTRL_IDX                       45
#define LEVANTE_LO2_CTRL_DEFAULT_VALUE             0

#define LO2_GAIN_MASK                                                      0x7   /* lo2_gain;  bits [2:0] */
#define LO2_GAIN_SHIFT                                                     0

/* Levante Reg: EAR_SPKR_CTRL1, Address 0xD9 */
#define LEVANTE_EAR_SPKR_CTRL1_ADDRESS                 0xD9
#define LEVANTE_EAR_SPKR_CTRL1_IDX                 46
#define LEVANTE_EAR_SPKR_CTRL1_DEFAULT_VALUE       0

#define SPEA_GAIN_MASK                                                     0x7   /* spea_gain;  bits [2:0] */
#define SPEA_GAIN_SHIFT                                            0

#define SPEA_IHOP1_MASK                            0x3   /* spea_ihop1; bits [4:3] */
#define SPEA_IHOP1_SHIFT                           3

#define SPKR_IBIAS3_MASK                           0x3   /* spkr_ibias3; bits [6:5] */
#define SPKR_IBIAS3_SHIFT                          5

/* Levante Reg: EAR_SPKR_CTRL2, Address 0xDA */
#define LEVANTE_EAR_SPKR_CTRL2_ADDRESS                 0xDA
#define LEVANTE_EAR_SPKR_CTRL2_IDX                 47
#define LEVANTE_EAR_SPKR_CTRL2_DEFAULT_VALUE       (1 << EAR_IBIAS3_SHIFT)

#define EAR_IBIAS3_MASK                            0x3   /* ear_ibias3;     bits [1:0] */
#define EAR_IBIAS3_SHIFT                           0

#define APPLY_CHANGES_MASK                                                 0x1   /* apply_changes;  bit 2 */
#define APPLY_CHANGES_SHIFT                                        2

#define I_OPA_HALF_MASK                                                    0x1   /* i_opa_half;     bit 7 */
#define I_OPA_HALF_SHIFT                                           7

/* Levante Reg: AUDIO_SUPPLIES1, Address 0xDB */
#define LEVANTE_AUDIO_SUPPLIES1_ADDRESS                0xDB
#define LEVANTE_AUDIO_SUPPLIES1_IDX                48
#define LEVANTE_AUDIO_SUPPLIES1_DEFAULT_VALUE      0x0 //((1 << CPUMP_SEL_SHIFT) | (1 << CPUMP_PWR_SEL_SHIFT))//Raul

#define CPUMP_SEL_MASK                                                     0x1   /* cpump_sel;      bit 0 */
#define CPUMP_SEL_SHIFT                                            0

#define CPUMP_PWR_SEL_MASK                                                 0x1   /* cpump_pwr_sel;  bit 1 */
#define CPUMP_PWR_SEL_SHIFT                                        1

/* Levante Reg: AUDIO_SUPPLIES2, Address 0xDC */
#define LEVANTE_AUDIO_SUPPLIES2_ADDRESS                0xDC
#define LEVANTE_AUDIO_SUPPLIES2_IDX                49
#define LEVANTE_AUDIO_SUPPLIES2_DEFAULT_VALUE	   \
	( (1 << REG27_MPU_SHIFT) | \
	  (1 << CPUMP_EN_SHIFT)  | \
	  (1 << AUDIO_EN_SHIFT) )                                                               /*0x0b*/

#define AUDIO_EN_MASK                                                      0x1   /* audio_en;   bit 0 */
#define AUDIO_EN_SHIFT                                                     0

#define CPUMP_EN_MASK                                                      0x1   /* cpump_en;   bit 1 */
#define CPUMP_EN_SHIFT                                                     1

#define REG27_MPU_MASK                                                     0x1   /* reg27_mpu;  bit 3 */
#define REG27_MPU_SHIFT                                            3

/* Levante Reg: ANALOG_AUDIO_SECTION_ENABLES, Address 0xDD */
#define LEVANTE_ADC_ENABLES1_ADDRESS                   0xDD
#define LEVANTE_ADC_ENABLES1_IDX                   50
#define LEVANTE_ADC_ENABLES1_DEFAULT_VALUE             0

#define EN_MOD1_MASK                                                       0x1   /* en_mod1;      bit 0 */
#define EN_MOD1_SHIFT                                                      0

#define EN_MOD2_MASK                                                       0x1   /* en_mod2;      bit 1 */
#define EN_MOD2_SHIFT                                                      1

#define EN_PGA_MIC1_MASK                                                   0x1   /* en_pga_mic1;  bit 2 */
#define EN_PGA_MIC1_SHIFT                                          2

#define EN_PGA_MIC3_MASK                                                   0x1   /* en_pga_mic3;  bit 3 */
#define EN_PGA_MIC3_SHIFT                                          3

#define EN_PGA_AUX1_MASK                                                   0x1   /* en_pga_aux1;  bit 4 */
#define EN_PGA_AUX1_SHIFT                                          4

#define EN_PGA_AUX2_MASK                                                   0x1   /* en_pga_aux2;  bit 5 */
#define EN_PGA_AUX2_SHIFT                                          5

/* Levante Reg: DIGITAL_AUDIO_SECTION_ENABLES, Address 0xDE */
#define LEVANTE_ADC_ENABLES2_ADDRESS                   0xDE
#define LEVANTE_ADC_ENABLES2_IDX                   51
#define LEVANTE_ADC_ENABLES2_DEFAULT_VALUE             0

#define PCM_EN_MASK                                                                0x1   /* pcm_en;        bit 0 */
#define PCM_EN_SHIFT                                                       0

#define ST_EN_MASK                                                                 0x1   /* st_en;         bit 1 */
#define ST_EN_SHIFT                                                        1

#define LOFI_RX_EN_MASK                                                    0x1   /* lofi_rx_en;    bit 2 */
#define LOFI_RX_EN_SHIFT                                           2

#define EC_SELECT_MASK                                                     0x1   /* ec_select;     bit 3 */
#define EC_SELECT_SHIFT                                            3

#define EN_ADC_RIGHT_MASK                                                  0x1   /* en_adc_right;  bit 4 */
#define EN_ADC_RIGHT_SHIFT                                         4

#define EN_ADC_LEFT_MASK                                                   0x1   /* en_adc_left;   bit 5 */
#define EN_ADC_LEFT_SHIFT                                          5

/* Levante Reg: ANALOG_AUDIO_SECTION_ENABLES_1, Address 0xDF */
#define LEVANTE_DAC_ENABLES1_ADDRESS                       0xDF
#define LEVANTE_DAC_ENABLES1_IDX                   52
#define LEVANTE_DAC_ENABLES1_DEFAULT_VALUE             0

#define HS1_PA_EN_MASK                                                     0x1   /* hs1_pa_en;   bit 0 */
#define HS1_PA_EN_SHIFT                                            0

#define HS2_PA_EN_MASK                                                     0x1   /* hs2_pa_en;   bit 1 */
#define HS2_PA_EN_SHIFT                                            1

#define LO1_PA_EN_MASK                                                     0x1   /* lo1_pa_en;   bit 2 */
#define LO1_PA_EN_SHIFT                                            2

#define LO2_PA_EN_MASK                                                     0x1   /* lo2_pa_en;   bit 3 */
#define LO2_PA_EN_SHIFT                                            3

#define EAR_PA_EN_MASK                                                     0x1   /* ear_pa_en;   bit 4 */
#define EAR_PA_EN_SHIFT                                            4

#define SPKR_PA_EN_MASK                                                    0x1   /* spkr_pa_en;  bit 5 */
#define SPKR_PA_EN_SHIFT                                           5

/* Levante Reg: DIGITAL_AUDIO_SECTION_ENABLES_1, Address 0xE1 */
#define LEVANTE_DAC_ENABLES2_ADDRESS                       0xE1
#define LEVANTE_DAC_ENABLES2_IDX                   53
#define LEVANTE_DAC_ENABLES2_DEFAULT_VALUE	       0

#define I2S_EN_MASK                                                                0x1   /* i2s_en;        bit 0 */
#define I2S_EN_SHIFT                                                       0

#define EPA_EN_R_MASK                                                      0x1   /* epa_en_r;      bit 1 */
#define EPA_EN_R_SHIFT                                                     1

#define EPA_EN_L_MASK                                                      0x1   /* epa_en_l;      bit 2 */
#define EPA_EN_L_SHIFT                                                     2

#define EN_MODULATOR_MASK                                                  0x1   /* en_modulator;  bit 3 */
#define EN_MODULATOR_SHIFT                                         3

#define EN_DAC_RIGHT_MASK                                                  0x1   /* en_dac_right;  bit 4 */
#define EN_DAC_RIGHT_SHIFT                                         4

#define EN_DAC_LEFT_MASK                                                   0x1   /* en_dac_left;   bit 5 */
#define EN_DAC_LEFT_SHIFT                                          5

/* Levante Reg: AUDIO_CAL1, Address 0xE2 */
#define LEVANTE_AUDIO_CAL1_ADDRESS                             0xE2
#define LEVANTE_AUDIO_CAL1_IDX                     54
#define LEVANTE_AUDIO_CAL1_DEFAULT_VALUE               0

#define START_CAL_L_MASK                                                   0x1   /* start_cal_l;   bit 0 */
#define START_CAL_L_SHIFT                                          0

/* Levante Reg: AUDIO_CAL2, Address 0xE3 */
#define LEVANTE_AUDIO_CAL2_ADDRESS                             0xE3
#define LEVANTE_AUDIO_CAL2_IDX                     55
#define LEVANTE_AUDIO_CAL2_DEFAULT_VALUE               0

#define MIN_OFF_L_MASK                                                 0xF   /* min_off_l;     bits [3:0] */
#define MIN_OFF_L_SHIFT                                            0

#define MAX_OFF_L_MASK                                                 0xF   /* max_off_l;     bits [7:4] */
#define MAX_OFF_L_SHIFT                                            4

/* Levante Reg: AUDIO_CAL3, Address 0xE4 */
#define LEVANTE_AUDIO_CAL3_ADDRESS                             0xE4
#define LEVANTE_AUDIO_CAL3_IDX                     56
#define LEVANTE_AUDIO_CAL3_DEFAULT_VALUE               0

#define NZ_EVAL_L_MASK                                                 0x7   /* nz_eval_l;     bits [2:0] */
#define NZ_EVAL_L_SHIFT                                            0

/* Levante Reg: AUDIO_CAL4, Address 0xE5 */
#define LEVANTE_AUDIO_CAL4_ADDRESS                             0xE5
#define LEVANTE_AUDIO_CAL4_IDX                     57
#define LEVANTE_AUDIO_CAL4_DEFAULT_VALUE               0

#define MIN_OFF_R_MASK                                                 0xF   /* min_off_r;     bits [3:0] */
#define MIN_OFF_R_SHIFT                                            0

#define MAX_OFF_R_MASK                                                 0xF   /* max_off_r;     bits [7:4] */
#define MAX_OFF_R_SHIFT                                            4

/* Levante Reg: AUDIO_CAL5, Address 0xE6 */
#define LEVANTE_AUDIO_CAL5_ADDRESS                             0xE6
#define LEVANTE_AUDIO_CAL5_IDX                     58
#define LEVANTE_AUDIO_CAL5_DEFAULT_VALUE               0

#define NZ_EVAL_R_MASK                                                 0x7   /* nz_eval_r;     bits [2:0] */
#define NZ_EVAL_R_SHIFT                                            0

/* Levante Reg: ANALOG_INPUT_SELECTION_1, Address 0xE7 */
#define LEVANTE_ANALOG_INPUT_SEL1_ADDRESS                  0xE7
#define LEVANTE_ANALOG_INPUT_SEL1_IDX              59
#define LEVANTE_ANALOG_INPUT_SEL1_DEFAULT_VALUE    0

#define SEL_HS1_MASK                                                       0x3   /* sel_hs1;  bits [1:0] */
#define SEL_HS1_SHIFT                                                      0

#define SEL_HS2_MASK                                                       0x3   /* sel_hs2;  bits [3:2] */
#define SEL_HS2_SHIFT                                                      2

#define SEL_LO1_MASK                                                       0x3   /* sel_lo1;  bits [5:4] */
#define SEL_LO1_SHIFT                                                      4

#define SEL_LO2_MASK                                                       0x3   /* sel_lo2;  bits [7:6] */
#define SEL_LO2_SHIFT                                                      6

/* Levante Reg: ANALOG_INPUT_SELECTION_2, Address 0xE8 */
#define LEVANTE_ANALOG_INPUT_SEL2_ADDRESS                  0xE8
#define LEVANTE_ANALOG_INPUT_SEL2_IDX              60
#define LEVANTE_ANALOG_INPUT_SEL2_DEFAULT_VALUE    0

#define SEL_SPEA_MASK                                                      0x3   /* sel_spea;  bits [1:0] */
#define SEL_SPEA_SHIFT                                                     0

/* Levante Reg: PCM_INTERFACE4, Address 0xE9 */
#define LEVANTE_PCM_INTERFACE4_ADDRESS       0xE9
#define LEVANTE_PCM_INTERFACE4_IDX           61
#define LEVANTE_PCM_INTERFACE4_DEFAULT_VALUE 0x27                                 /*Raul, now use mic detect instead of headset, sanremo chip id < 0x41*/

#define PCM_OFFS_R_MASK				               0x3F  /* pcm_offs_r;   bits [5:0] */
#define PCM_OFFS_R_SHIFT     		               0

/* Levante Reg: I2S_INTERFACE5, Address 0xEA */
#define LEVANTE_I2S_INTERFACE5_ADDRESS              0xEA
#define LEVANTE_I2S_INTERFACE5_IDX              62
#define LEVANTE_I2S_INTERFACE5_DEFAULT_VALUE    0

#define I2S_OFFS_R_MASK	   			               0x3F  /* i2s_offs_r;  bits [5:0] */
#define I2S_OFFS_R_SHIFT     		               0

/* Levante Reg: PA_SHORT_MANAGMENT, Address 0xEB */
#define LEVANTE_SHORTS_ADDRESS                         0xEB
#define LEVANTE_SHORTS_IDX                         63
#define LEVANTE_SHORTS_DEFAULT_VALUE               0


#define HS1_SHORT_P_MASK                                                   0x1   /* hs1_short_p;  bit 1 */
#define HS1_SHORT_P_SHIFT                                          1

#define HS2_SHORT_P_MASK                                                   0x1   /* hs2_short_p;  bit 3 */
#define HS2_SHORT_P_SHIFT                                          3

#define LO1_SHORT_P_MASK                                                   0x1   /* lo1_short_p;  bit 5 */
#define LO1_SHORT_P_SHIFT                                          5

#define LO2_SHORT_P_MASK                                                   0x1   /* lo2_short_p;  bit 7 */
#define LO2_SHORT_P_SHIFT                                          7

/*CP maintain a register list, CP must align with AP. if we don't want to touch CP code, put the additional register define here*/
/* Levante Reg: Mclk, Address 0x41 */
#define LEVANTE_MCLK_ADDRESS		               0x41
#define LEVANTE_MCLK_IDX	                       64
#define LEVANTE_MCLK_DEFAULT_VALUE                 0

#define BK_CKSEL_MASK                              0x3   /* bk_cksel;    bits [2:0] */
#define BK_CKSEL_SHIFT                             0

#define CP_CKSEL_MASK                              0x3   /* cp_cksel;    bits [4:3] */
#define CP_CKSEL_SHIFT                             3

#define BC_CKSEL_MASK                              0x3   /* bc_cksel;    bits [6:5] */
#define BC_CKSEL_SHIFT                             5

#define BK_CKSLP_DIS_MASK                          0x1   /* bk_cksel;    bit 7 */
#define BK_CKSLP_DIS_SHIFT                         7

#define MCLK_ALL_MASK                              0xFF  /* mclk_all;    bits [7:0] */
#define MCLK_ALL_SHIFT                             0

#define NUM_OF_REGS                                (LEVANTE_MCLK_IDX + 1)

#define LEVANTE_NO_IDX                             0xFF
#define LEVANTE_NO_ADDRESS                         0xFF
#define LEVANTE_NO_VALUE                           0xFF

#define VOLUME_RAMP_STEPS                          3

#define DPGA1_HIFIL_HIFIR_GAIN_LEFT_DEFAULT_VALUE  0x0A
#define DPGA2_HIFIL_HIFIR_GAIN_RIGHT_DEFAULT_VALUE 0x0A

/*----------- Local type definitions -----------------------------------------*/
typedef struct
{
	LevantePaths path;
	LEVANTE_SlopeOffsetS pga;
	unsigned char regIndex;
	unsigned char gainMask;
	unsigned char gainShift;
	unsigned char gainTableId;
} LevanteGains;

typedef struct
{
	LevanteGains            *gainPath;
	UINT8 totalPaths;
} LEVANTE_FileGain2StructGainConverter;

typedef struct
{
	unsigned char index;
	unsigned char address;
	unsigned char defaultValue;
} RegisterInfo;

typedef enum
{
	/* FREQUENCY_SETTINGS */
	I_OPA_HALF = 0,
	RATE_ADC_PCM,
	RATE_DAC_I2S,

	/* EQUALIZER_SETTINGS */
	EQU_BYP,
	EQ_N0_1,
	EQ_N0_2,
	EQ_N1_1,
	EQ_N1_2,
	EQ_D1_1,
	EQ_D1_2,

	/* TRI_STATE_SETTINGS */
	HIZ_DOUT,
	HIZ_I2S_DOUT,

	/* DATA_LENGTH_SETTINGS */
	PCM_WL,
	I2S_WL,

	/* DIG_MIC_INIT_SETTINGS */
	DMIC_CLOCK_SEL,
	DMIC_DATA_SEL,

	/* ANALOG_MIC_INIT_SETTINGS */
	MBC1_2,
	MBC3,

	/* LOOPBACK_TEST_SETTINGS */
	PCM_INT_LOOPBACK,
	PCM_EXT_LOOPBACK,
	I2S_INT_LOOPBACK,
	I2S_EXT_LOOPBACK,

	/* MASTER_SLAVE_MODE_SETTINGS */
	PCM_MASTER,
	I2S_MASTER,

	/* LOUD_SPEAKER_SETTINGS */
	AUDIO_ZLOAD_4OHM,

	/* FORCE_SPEAKER_GAIN_SETTINGS */
	DPGA1_HIFIL_HIFIR_GAIN_LEFT,   /* Gain IDs 9+10 */
	DPGA2_HIFIL_HIFIR_GAIN_RIGHT,  /* Gain IDs 12+13 */

	/* Must be at the end of the list */
	NUM_OF_EXTRA_SETTINGS_FIELDS
} LevanteRegField;


typedef struct
{
	RegisterInfo const *registerInfo;
	BOOL               isRsyncNeeded;
} RegisterInfoGroupData;

typedef struct
{
	UINT16 fmFrequency;
	UINT8  cpClkDivider_0_9;
	UINT8  cpClkDivider_1_8;
} FmFreqToClkDivTable;

/*----------- Local macro definitions ----------------------------------------*/
#define LEVANTE_FIND_PATH(iDX, pATH, lIST, nUMiTEMS)  {    int i;			\
							   for (i = 0; i < nUMiTEMS; i++)  \
							   {				\
								   if (lIST[i].path == pATH)											     \
									   break;											  \
							   }				\
							   iDX = (unsigned char)i;	\
}

#define LEVANTE_DIGITAL_GAIN_CLIP(gAIN)               (    ((gAIN) < (-36)) ? -36 : \
							   (((gAIN) > ( 12)) ? 12 : \
							    ((signed char)(gAIN)))    )

#define LEVANTE_GET_PATH_INPUT(fULLpATH)        (gLevantePathDescription[fULLpATH].pathInput)
#define LEVANTE_GET_PATH_OUTPUT(fULLpATH)       (gLevantePathDescription[fULLpATH].pathOutput)

#define READ_SPECIFIC_BITS(rEG, bITSnAME)       (((rEG) & (bITSnAME##_MASK << (bITSnAME##_SHIFT))) >> (bITSnAME##_SHIFT))
#define CLEAR_SPECIFIC_BITS(rEG, bITSnAME)      (rEG) &= ~(bITSnAME ## _MASK << (bITSnAME ## _SHIFT))
#define WRITE_SPECIFIC_BITS(rEG, vAL, bITSnAME) (rEG) &= ((unsigned char)(~(bITSnAME ## _MASK << (bITSnAME ## _SHIFT)))); \
	(rEG) |= ((vAL) << (bITSnAME ## _SHIFT))
#define WRITE_GAIN_BITS(rEG, gAIN, gAINmASK, gAINsHIFT) (rEG) &= ~(gAINmASK << gAINsHIFT); \
	(rEG) |= ((gAIN) << (gAINsHIFT))
#define READ_PARAM_FROM_FDI_AND_TEST(pARAM, tEMPvAR, fDIiD)			 \
	{									   \
		if (FDI_fread(&(tEMPvAR), sizeof(tEMPvAR), 1, (FILE*)(fDIiD)) != 1)										   \
			break;										      \
		pARAM = tEMPvAR;						   \
	}


/*----------- Local variable definitions -------------------------------------*/
static unsigned char _pmicId = 0x48; /*Raul: Currently we only support Levante B0 or newer than B0*/
static LevanteCurrentPathS gLevanteCurrentPaths[LEVANTE_TOTAL_VALID_PATHS];
static LevantePaths gLevantePathsSortedList[LEVANTE_TOTAL_VALID_PATHS];
static LevanteDatabaseS gLevanteDatabase;
static unsigned char shadowReg[NUM_OF_REGS];
static unsigned char regField[NUM_OF_EXTRA_SETTINGS_FIELDS];
static unsigned char osrRateOffset = 0;
extern int gCPIsPCMMaster;
static unsigned short            _fmRadioFrequency = 0;
static const FmFreqToClkDivTable fmFreqToClkDiv[] = { FM_FREQ_TO_CLK_DIV };

/* According to the total gain of LEVANTE_I2S_TO_HS and LEVANTE_PCM_L_TO_HS, the charge-pump voltage will be set.
 * If the gain is below -7.25 dB, use 0.9 V, otherwise use 1.8 V.
 * The above path is using 2 gain Table IDs: 0 and 1.
 * Below is the threshold calculation (~ -7.25 dB):
 * Table 0: for index (0-6) the gain (in dB):                      0,      2.4,    5.6,    7,      8.9,    9.8,   12
 * Table 1: for index (5, 7, 9, 10, 11, 12, 13) the gain (in dB): -7.50, -10.55, -13.52, -14.91, -16.57, -18.06, -19.54
 *                                                               ====================================================
 * The sum of the 2 gains <= -7.25 dB:                            -7.50, -8.15,  -7.92,  -7.91,  -7.67,  -8.26,  -7.54  */
static unsigned char        gainOfTableIdOne[] =                 { 5,     7,      9,     10,     11,     12,     13 };

/*here is the sanremo audio default value*/
static const unsigned char sanremo_audio_regs[] = {
	0x00, 0x00, 0x08, 0x82, 0x00,		  /*0x42 ~ 0x46*/
	0x00, 0x00, 0x00, 0x00,	/*0xb0 ~ 0xb3*/
	0x00, 0x00, 0x00, 0x00, /*0xb4 ~ 0xb7*/
	0x00, 0x00, 0x00, 0x00, /*0xb8 ~ 0xbb*/
	0x08, 0x00, 0x40, 0x00, /*0xbc ~ 0xbf*/
	0x00, 0x00, 0x00, 0x00,	/*0xc0 ~ 0xc3*/
	0x00, 0x3f, 0x3f, 0x3f,	/*0xc4 ~ 0xc7*/
	0x3f, 0x3f, 0x3f, 0x00,	/*0xc8 ~ 0xcb*/
	0x00, 0x00, 0x00, 0x00,	/*0xcc ~ 0xcf*/
	0x60, 0x00, 0x00, 0x00,	/*0xd0 ~ 0xd3*/
	0x00, 0x00, 0x00, 0x00,	/*0xd4 ~ 0xd7*/
	0x00, 0x00, 0x00, 0x00,	/*0xd8 ~ 0xdb*/
	0x00, 0x00, 0x00, 0x00,	/*0xdc ~ 0xdf*/
	      0x00, 0x00, 0x00,	/*0xe1 ~ 0xe3*/
	0x00, 0x00, 0x00, 0x00,	/*0xe4 ~ 0xe7*/
	0x00, 0x00, 0x00, 0x00,	/*0xe8 ~ 0xeb*/
	0x00,                   /*0x41*/
};


/* Gain ID 0 */
#define LEVANTE_MSA_DIGITAL_GAIN_MAX    LEVANTE_TOTAL_VALID_PATHS
static LevanteGains gLevanteMsaDigitalGain[LEVANTE_MSA_DIGITAL_GAIN_MAX] =
{
	// PCM 1 output
	{ LEVANTE_AnaMIC1_TO_PCM_L,				     { 0, 0xFF00 }, 0, 0, 0, 0 },
	{ LEVANTE_AnaMIC1_LOUD_TO_PCM_L,			     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_AnaMIC2_TO_PCM_L,				     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_DigMIC1_TO_PCM_L,				     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_DigMIC1_LOUD_TO_PCM_L,			     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_DigMIC2_TO_PCM_R,				     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_AnaMIC3_TO_PCM_R,				     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_AUX1_TO_PCM_L,				     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_AUX2_TO_PCM_R,				     { 0, 0	 }, 0, 0, 0, 0 },

	// PCM 2 output
	{ LEVANTE_AnaMIC1_TO_PCM_R,				     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_AnaMIC1_LOUD_TO_PCM_R,			     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_AnaMIC2_TO_PCM_R,				     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_DigMIC1_TO_PCM_R,				     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_DigMIC1_LOUD_TO_PCM_R,			     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_DigMIC2_TO_PCM_L,				     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_AnaMIC3_TO_PCM_L,				     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_AUX1_TO_PCM_R,				     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_AUX2_TO_PCM_L,				     { 0, 0	 }, 0, 0, 0, 0 },

	// I2S 1 output
	{ LEVANTE_AnaMIC1_TO_I2S_L,				     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_AnaMIC1_LOUD_TO_I2S_L,			     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_AnaMIC2_TO_I2S_L,				     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_DigMIC1_TO_I2S_L,				     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_DigMIC1_LOUD_TO_I2S_L,			     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_DigMIC2_TO_I2S_R,				     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_AnaMIC3_TO_I2S_R,				     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_AUX1_TO_I2S_L,				     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_AUX2_TO_I2S_R,				     { 0, 0	 }, 0, 0, 0, 0 },

	// I2S 2 output
	{ LEVANTE_AnaMIC1_TO_I2S_R,				     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_AnaMIC1_LOUD_TO_I2S_R,			     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_AnaMIC2_TO_I2S_R,				     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_DigMIC1_TO_I2S_R,				     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_DigMIC1_LOUD_TO_I2S_R,			     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_DigMIC2_TO_I2S_L,				     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_AnaMIC3_TO_I2S_L,				     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_AUX1_TO_I2S_R,				     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_AUX2_TO_I2S_L,				     { 0, 0	 }, 0, 0, 0, 0 },

	// From PCM Left
	{ LEVANTE_PCM_L_TO_EAR,					     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_PCM_L_TO_SPKR,				     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_PCM_L_TO_HS1,					     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_PCM_L_TO_HS2,					     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_PCM_L_TO_HS,					     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_PCM_L_TO_LINEOUT1,				     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_PCM_L_TO_LINEOUT2,				     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_PCM_L_TO_LINEOUT,				     { 0, 0	 }, 0, 0, 0, 0 },

	// From PCM Right
	{ LEVANTE_PCM_R_TO_EAR,					     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_PCM_R_TO_SPKR,				     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_PCM_R_TO_HS1,					     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_PCM_R_TO_HS2,					     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_PCM_R_TO_HS,					     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_PCM_R_TO_LINEOUT1,				     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_PCM_R_TO_LINEOUT2,				     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_PCM_R_TO_LINEOUT,				     { 0, 0	 }, 0, 0, 0, 0 },

	// From I2S
	{ LEVANTE_I2S_TO_EAR,					     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_I2S_TO_SPKR,					     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_I2S_TO_HS1,					     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_I2S_TO_HS2,					     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_I2S_TO_HS,					     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_I2S_TO_LINEOUT1,				     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_I2S_TO_LINEOUT2,				     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_I2S_TO_LINEOUT,				     { 0, 0	 }, 0, 0, 0, 0 },

	// From I2S1
	{ LEVANTE_I2S1_TO_EAR,					     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_I2S1_TO_SPKR,					     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_I2S1_TO_HS1,					     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_I2S1_TO_HS2,					     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_I2S1_TO_HS,					     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_I2S1_TO_LINEOUT1,				     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_I2S1_TO_LINEOUT2,				     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_I2S1_TO_LINEOUT,				     { 0, 0	 }, 0, 0, 0, 0 },

	// Side Tone
	{ LEVANTE_IN_L_TO_OUT,					     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_IN_R_TO_OUT,					     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_IN_BOTH_TO_OUT,				     { 0, 0	 }, 0, 0, 0, 0 },

	// Digital Loops
	{ LEVANTE_PCM_INT_LOOP,					     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_PCM_EXT_LOOP,					     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_I2S_INT_LOOP,					     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_I2S_EXT_LOOP,					     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_DAC1_IN_EC_LOOP,				     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_DAC2_IN_EC_LOOP,				     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_DAC1_DAC2_IN_EC_LOOP,				     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_PCM_L_IN_TO_I2S_L_OUT_LOOP,			     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_PCM_L_IN_TO_I2S_R_OUT_LOOP,			     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_PCM_L_IN_TO_I2S_Both_OUT_LOOP,		     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_PCM_R_IN_TO_I2S_L_OUT_LOOP,			     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_PCM_R_IN_TO_I2S_R_OUT_LOOP,			     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_PCM_R_IN_TO_I2S_Both_OUT_LOOP,		     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_I2S_IN_TO_I2S_L_OUT_LOOP,			     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_I2S_IN_TO_I2S_R_OUT_LOOP,			     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_I2S_IN_TO_I2S_Both_OUT_LOOP,			     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_I2S1_IN_TO_I2S_L_OUT_LOOP,			     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_I2S1_IN_TO_I2S_R_OUT_LOOP,			     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_I2S1_IN_TO_I2S_Both_OUT_LOOP,			     { 0, 0	 }, 0, 0, 0, 0 },

	// Analog Loops
	{ LEVANTE_AUX1_TO_HS2,					     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_AUX1_TO_LOUT2,				     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_AUX2_TO_HS1,					     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_AUX2_TO_LOUT1,				     { 0, 0	 }, 0, 0, 0, 0 },

	//Jackie,2011-0222
	//Loop include codec and MSA
	{ LEVANTE_PCM_L_TO_EAR_LOOP,             {0, 0     } , 0, 0, 0, 0 },
	{ LEVANTE_PCM_L_TO_HS_LOOP,              {0, 0     } , 0, 0, 0, 0 },
	{ LEVANTE_PCM_L_TO_SPKR_LOOP,            {0, 0     } , 0, 0, 0, 0 },
	{ LEVANTE_AnaMIC1_TO_PCM_L_LOOP,         {0, 0xFF00} , 0, 0, 0, 0 },
	{ LEVANTE_AnaMIC2_TO_PCM_L_LOOP,         {0, 0     } , 0, 0, 0, 0 },
	{ LEVANTE_AnaMIC1_LOUD_TO_PCM_L_LOOP,    {0, 0     } , 0, 0, 0, 0 },

	//Aux to speaker use sidetone path
	{ LEVANTE_AUX1_TO_SIDETONE_L,				     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_AUX2_TO_SIDETONE_R,				     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_SIDETONE_IN_TO_OUT,				     {0, 0    }, 0, 0, 0, 0 },
	{ LEVANTE_SIDETONE_TO_SPKR,				     {0, 0    }, 0, 0, 0, 0 },

	// added for recording by GSSP
	{ LEVANTE_AnaMIC1_TO_PCM_I2S_L,				     { 0, 0xFF00 }, 0, 0, 0, 0 },
	{ LEVANTE_AnaMIC1_TO_PCM_I2S_R,				     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_AnaMIC1_LOUD_TO_PCM_I2S_L,			     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_AnaMIC1_LOUD_TO_PCM_I2S_R,			     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_AnaMIC2_TO_PCM_I2S_L,				     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_AnaMIC2_TO_PCM_I2S_R,				     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_AUX1_TO_PCM_I2S_L,				     { 0, 0	 }, 0, 0, 0, 0 },
	{ LEVANTE_AUX2_TO_PCM_I2S_R,				     { 0, 0	 }, 0, 0, 0, 0 }
};

/* Gain ID 1 */
#define LEVANTE_PGA_EARP_GAIN_MAX       5
static LevanteGains gLevantePgaEarpGain[] =
{
	{ LEVANTE_PCM_L_TO_EAR,	  { 0x003D, 0x0 }, LEVANTE_EAR_SPKR_CTRL1_IDX, SPEA_GAIN_MASK, SPEA_GAIN_SHIFT, 0 },
	{ LEVANTE_PCM_R_TO_EAR,	  { 0x003D, 0x0 }, LEVANTE_EAR_SPKR_CTRL1_IDX, SPEA_GAIN_MASK, SPEA_GAIN_SHIFT, 0 },
	{ LEVANTE_I2S_TO_EAR,	  { 0x003D, 0x0 }, LEVANTE_EAR_SPKR_CTRL1_IDX, SPEA_GAIN_MASK, SPEA_GAIN_SHIFT, 0 },
	{ LEVANTE_I2S1_TO_EAR,	  { 0x003D, 0x0 }, LEVANTE_EAR_SPKR_CTRL1_IDX, SPEA_GAIN_MASK, SPEA_GAIN_SHIFT, 0 },

	{ LEVANTE_PCM_L_TO_EAR_LOOP,	  { 0x003D, 0x0 }, LEVANTE_EAR_SPKR_CTRL1_IDX, SPEA_GAIN_MASK, SPEA_GAIN_SHIFT, 0 }
};

/* Gain ID 2 */
#define LEVANTE_PGA_SPKR_GAIN_MAX       7
static LevanteGains gLevantePgaSpkrGain[LEVANTE_PGA_SPKR_GAIN_MAX] =
{
	{ LEVANTE_PCM_L_TO_SPKR,   { 0x003D, 0x0 }, LEVANTE_EAR_SPKR_CTRL1_IDX, SPEA_GAIN_MASK, SPEA_GAIN_SHIFT, 0 },
	{ LEVANTE_PCM_R_TO_SPKR,   { 0x003D, 0x0 }, LEVANTE_EAR_SPKR_CTRL1_IDX, SPEA_GAIN_MASK, SPEA_GAIN_SHIFT, 0 },
	{ LEVANTE_I2S_TO_SPKR,	   { 0x003D, 0x0 }, LEVANTE_EAR_SPKR_CTRL1_IDX, SPEA_GAIN_MASK, SPEA_GAIN_SHIFT, 0 },
	{ LEVANTE_I2S1_TO_SPKR,	   { 0x003D, 0x0 }, LEVANTE_EAR_SPKR_CTRL1_IDX, SPEA_GAIN_MASK, SPEA_GAIN_SHIFT, 0 },

	{ LEVANTE_PCM_L_TO_SPKR_LOOP, {0x003D, 0x0}, LEVANTE_EAR_SPKR_CTRL1_IDX, SPEA_GAIN_MASK, SPEA_GAIN_SHIFT, 0 },
	{ LEVANTE_SIDETONE_TO_SPKR,   { 0x003D, 0x0 }, LEVANTE_EAR_SPKR_CTRL1_IDX, SPEA_GAIN_MASK, SPEA_GAIN_SHIFT, 0 },
	{ LEVANTE_I2S_TO_SPKR_HS,  { 0x0000, 0x0 }, LEVANTE_EAR_SPKR_CTRL1_IDX, SPEA_GAIN_MASK, SPEA_GAIN_SHIFT, 0 }
};

/* Gain ID 3 */
#define LEVANTE_PGA_HS1_GAIN_MAX       11
static LevanteGains gLevantePgaHs1Gain[LEVANTE_PGA_HS1_GAIN_MAX] =
{
	{ LEVANTE_PCM_L_TO_HS1,	   { 0x003D, 0x0 }, LEVANTE_HS1_CTRL_IDX, HS1_GAIN_MASK, HS1_GAIN_SHIFT, 0 },
	{ LEVANTE_PCM_L_TO_HS,	   { 0x003D, 0x0 }, LEVANTE_HS1_CTRL_IDX, HS1_GAIN_MASK, HS1_GAIN_SHIFT, 0 },
	{ LEVANTE_PCM_R_TO_HS1,	   { 0x003D, 0x0 }, LEVANTE_HS1_CTRL_IDX, HS1_GAIN_MASK, HS1_GAIN_SHIFT, 0 },
	{ LEVANTE_PCM_R_TO_HS,	   { 0x003D, 0x0 }, LEVANTE_HS1_CTRL_IDX, HS1_GAIN_MASK, HS1_GAIN_SHIFT, 0 },
	{ LEVANTE_I2S_TO_HS1,	   { 0x003D, 0x0 }, LEVANTE_HS1_CTRL_IDX, HS1_GAIN_MASK, HS1_GAIN_SHIFT, 0 },
	{ LEVANTE_I2S_TO_HS,	   { 0x003D, 0x0 }, LEVANTE_HS1_CTRL_IDX, HS1_GAIN_MASK, HS1_GAIN_SHIFT, 0 },
	{ LEVANTE_I2S1_TO_HS1,	   { 0x003D, 0x0 }, LEVANTE_HS1_CTRL_IDX, HS1_GAIN_MASK, HS1_GAIN_SHIFT, 0 },
	{ LEVANTE_I2S1_TO_HS,	   { 0x003D, 0x0 }, LEVANTE_HS1_CTRL_IDX, HS1_GAIN_MASK, HS1_GAIN_SHIFT, 0 },
	{ LEVANTE_AUX2_TO_HS1,	   { 0x003D, 0x0 }, LEVANTE_HS1_CTRL_IDX, HS1_GAIN_MASK, HS1_GAIN_SHIFT, 0 },

	{ LEVANTE_PCM_L_TO_HS_LOOP,  {0x003D, 0x0}, LEVANTE_HS1_CTRL_IDX, HS1_GAIN_MASK, HS1_GAIN_SHIFT, 0 },
	{ LEVANTE_I2S_TO_SPKR_HS,  { 0x0000, 0x0 }, LEVANTE_HS1_CTRL_IDX, HS1_GAIN_MASK, HS1_GAIN_SHIFT, 0 }
};

/* Gain ID 4 */
#define LEVANTE_PGA_HS2_GAIN_MAX       11
static LevanteGains gLevantePgaHs2Gain[LEVANTE_PGA_HS2_GAIN_MAX] =
{
	{ LEVANTE_PCM_L_TO_HS2,	   { 0x003D, 0x0 }, LEVANTE_HS2_CTRL_IDX, HS2_GAIN_MASK, HS2_GAIN_SHIFT, 0 },
	{ LEVANTE_PCM_L_TO_HS,	   { 0x003D, 0x0 }, LEVANTE_HS2_CTRL_IDX, HS2_GAIN_MASK, HS2_GAIN_SHIFT, 0 },
	{ LEVANTE_PCM_R_TO_HS2,	   { 0x003D, 0x0 }, LEVANTE_HS2_CTRL_IDX, HS2_GAIN_MASK, HS2_GAIN_SHIFT, 0 },
	{ LEVANTE_PCM_R_TO_HS,	   { 0x003D, 0x0 }, LEVANTE_HS2_CTRL_IDX, HS2_GAIN_MASK, HS2_GAIN_SHIFT, 0 },
	{ LEVANTE_I2S_TO_HS2,	   { 0x003D, 0x0 }, LEVANTE_HS2_CTRL_IDX, HS2_GAIN_MASK, HS2_GAIN_SHIFT, 0 },
	{ LEVANTE_I2S_TO_HS,	   { 0x003D, 0x0 }, LEVANTE_HS2_CTRL_IDX, HS2_GAIN_MASK, HS2_GAIN_SHIFT, 0 },
	{ LEVANTE_I2S1_TO_HS2,	   { 0x003D, 0x0 }, LEVANTE_HS2_CTRL_IDX, HS2_GAIN_MASK, HS2_GAIN_SHIFT, 0 },
	{ LEVANTE_I2S1_TO_HS,	   { 0x003D, 0x0 }, LEVANTE_HS2_CTRL_IDX, HS2_GAIN_MASK, HS2_GAIN_SHIFT, 0 },
	{ LEVANTE_AUX1_TO_HS2,	   { 0x003D, 0x0 }, LEVANTE_HS2_CTRL_IDX, HS2_GAIN_MASK, HS2_GAIN_SHIFT, 0 },

	{ LEVANTE_PCM_L_TO_HS_LOOP,  {0x003D, 0x0}, LEVANTE_HS2_CTRL_IDX, HS2_GAIN_MASK, HS2_GAIN_SHIFT, 0 },
	{ LEVANTE_I2S_TO_SPKR_HS,  { 0x0000, 0x0 }, LEVANTE_HS2_CTRL_IDX, HS2_GAIN_MASK, HS2_GAIN_SHIFT, 0 }
};

/* Gain ID 5 */
#define LEVANTE_PGA_LOUT1_GAIN_MAX       9
static LevanteGains gLevantePgaLout1Gain[LEVANTE_PGA_LOUT1_GAIN_MAX] =
{
	{ LEVANTE_PCM_L_TO_LINEOUT1,	 { 0x003D, 0x0 }, LEVANTE_LO1_CTRL_IDX, LO1_GAIN_MASK, LO1_GAIN_SHIFT, 0 },
	{ LEVANTE_PCM_L_TO_LINEOUT,	 { 0x003D, 0x0 }, LEVANTE_LO1_CTRL_IDX, LO1_GAIN_MASK, LO1_GAIN_SHIFT, 0 },
	{ LEVANTE_PCM_R_TO_LINEOUT1,	 { 0x003D, 0x0 }, LEVANTE_LO1_CTRL_IDX, LO1_GAIN_MASK, LO1_GAIN_SHIFT, 0 },
	{ LEVANTE_PCM_R_TO_LINEOUT,	 { 0x003D, 0x0 }, LEVANTE_LO1_CTRL_IDX, LO1_GAIN_MASK, LO1_GAIN_SHIFT, 0 },
	{ LEVANTE_I2S_TO_LINEOUT1,	 { 0x003D, 0x0 }, LEVANTE_LO1_CTRL_IDX, LO1_GAIN_MASK, LO1_GAIN_SHIFT, 0 },
	{ LEVANTE_I2S_TO_LINEOUT,	 { 0x003D, 0x0 }, LEVANTE_LO1_CTRL_IDX, LO1_GAIN_MASK, LO1_GAIN_SHIFT, 0 },
	{ LEVANTE_I2S1_TO_LINEOUT1,	 { 0x003D, 0x0 }, LEVANTE_LO1_CTRL_IDX, LO1_GAIN_MASK, LO1_GAIN_SHIFT, 0 },
	{ LEVANTE_I2S1_TO_LINEOUT,	 { 0x003D, 0x0 }, LEVANTE_LO1_CTRL_IDX, LO1_GAIN_MASK, LO1_GAIN_SHIFT, 0 },
	{ LEVANTE_AUX2_TO_LOUT1,	 { 0x003D, 0x0 }, LEVANTE_LO1_CTRL_IDX, LO1_GAIN_MASK, LO1_GAIN_SHIFT, 0 }
};

/* Gain ID 6 */
#define LEVANTE_PGA_LOUT2_GAIN_MAX       9
static LevanteGains gLevantePgaLout2Gain[LEVANTE_PGA_LOUT2_GAIN_MAX] =
{
	{ LEVANTE_PCM_L_TO_LINEOUT2,	 { 0x003D, 0x0 }, LEVANTE_LO2_CTRL_IDX, LO2_GAIN_MASK, LO2_GAIN_SHIFT, 0 },
	{ LEVANTE_PCM_L_TO_LINEOUT,	 { 0x003D, 0x0 }, LEVANTE_LO2_CTRL_IDX, LO2_GAIN_MASK, LO2_GAIN_SHIFT, 0 },
	{ LEVANTE_PCM_R_TO_LINEOUT2,	 { 0x003D, 0x0 }, LEVANTE_LO2_CTRL_IDX, LO2_GAIN_MASK, LO2_GAIN_SHIFT, 0 },
	{ LEVANTE_PCM_R_TO_LINEOUT,	 { 0x003D, 0x0 }, LEVANTE_LO2_CTRL_IDX, LO2_GAIN_MASK, LO2_GAIN_SHIFT, 0 },
	{ LEVANTE_I2S_TO_LINEOUT2,	 { 0x003D, 0x0 }, LEVANTE_LO2_CTRL_IDX, LO2_GAIN_MASK, LO2_GAIN_SHIFT, 0 },
	{ LEVANTE_I2S_TO_LINEOUT,	 { 0x003D, 0x0 }, LEVANTE_LO2_CTRL_IDX, LO2_GAIN_MASK, LO2_GAIN_SHIFT, 0 },
	{ LEVANTE_I2S1_TO_LINEOUT2,	 { 0x003D, 0x0 }, LEVANTE_LO2_CTRL_IDX, LO2_GAIN_MASK, LO2_GAIN_SHIFT, 0 },
	{ LEVANTE_I2S1_TO_LINEOUT,	 { 0x003D, 0x0 }, LEVANTE_LO2_CTRL_IDX, LO2_GAIN_MASK, LO2_GAIN_SHIFT, 0 },
	{ LEVANTE_AUX1_TO_LOUT2,	 { 0x003D, 0x0 }, LEVANTE_LO2_CTRL_IDX, LO2_GAIN_MASK, LO2_GAIN_SHIFT, 0 }
};

/* Gain ID 7 */
#define LEVANTE_DPGA1_LOFI_GAIN_LEFT_GAIN_MAX 26
static LevanteGains gLevanteDpga1LofiGainLeftGain[LEVANTE_DPGA1_LOFI_GAIN_LEFT_GAIN_MAX] =
{
	{ LEVANTE_PCM_L_TO_EAR,					   { 0xFF48, 0x0B00 }, LEVANTE_SIDE_TONE1_IDX, LOFI_GAIN_LEFT_MASK, LOFI_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_PCM_L_TO_SPKR,				   { 0xFF66, 0x0980 }, LEVANTE_SIDE_TONE1_IDX, LOFI_GAIN_LEFT_MASK, LOFI_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_PCM_L_TO_HS1,					   { 0xFDAF, 0x1F00 }, LEVANTE_SIDE_TONE1_IDX, LOFI_GAIN_LEFT_MASK, LOFI_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_PCM_L_TO_HS2,					   { 0xFDAF, 0x1F00 }, LEVANTE_SIDE_TONE1_IDX, LOFI_GAIN_LEFT_MASK, LOFI_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_PCM_L_TO_HS,					   { 0xFF48, 0x0D00 }, LEVANTE_SIDE_TONE1_IDX, LOFI_GAIN_LEFT_MASK, LOFI_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_PCM_L_TO_LINEOUT1,				   { 0xFDAF, 0x1F00 }, LEVANTE_SIDE_TONE1_IDX, LOFI_GAIN_LEFT_MASK, LOFI_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_PCM_L_TO_LINEOUT2,				   { 0xFDAF, 0x1F00 }, LEVANTE_SIDE_TONE1_IDX, LOFI_GAIN_LEFT_MASK, LOFI_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_PCM_L_TO_LINEOUT,				   { 0xFDAF, 0x1F00 }, LEVANTE_SIDE_TONE1_IDX, LOFI_GAIN_LEFT_MASK, LOFI_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_PCM_R_TO_EAR,					   { 0xFDAF, 0x1F00 }, LEVANTE_SIDE_TONE1_IDX, LOFI_GAIN_LEFT_MASK, LOFI_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_PCM_R_TO_SPKR,				   { 0xFDAF, 0x1F00 }, LEVANTE_SIDE_TONE1_IDX, LOFI_GAIN_LEFT_MASK, LOFI_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_PCM_R_TO_HS1,					   { 0xFDAF, 0x1F00 }, LEVANTE_SIDE_TONE1_IDX, LOFI_GAIN_LEFT_MASK, LOFI_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_PCM_R_TO_HS2,					   { 0xFDAF, 0x1F00 }, LEVANTE_SIDE_TONE1_IDX, LOFI_GAIN_LEFT_MASK, LOFI_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_PCM_R_TO_HS,					   { 0xFDAF, 0x1F00 }, LEVANTE_SIDE_TONE1_IDX, LOFI_GAIN_LEFT_MASK, LOFI_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_PCM_R_TO_LINEOUT1,				   { 0xFDAF, 0x1F00 }, LEVANTE_SIDE_TONE1_IDX, LOFI_GAIN_LEFT_MASK, LOFI_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_PCM_R_TO_LINEOUT2,				   { 0xFDAF, 0x1F00 }, LEVANTE_SIDE_TONE1_IDX, LOFI_GAIN_LEFT_MASK, LOFI_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_PCM_R_TO_LINEOUT,				   { 0xFDAF, 0x1F00 }, LEVANTE_SIDE_TONE1_IDX, LOFI_GAIN_LEFT_MASK, LOFI_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_PCM_L_IN_TO_I2S_L_OUT_LOOP,			   { 0xFDAF, 0x1F00 }, LEVANTE_SIDE_TONE1_IDX, LOFI_GAIN_LEFT_MASK, LOFI_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_PCM_L_IN_TO_I2S_R_OUT_LOOP,			   { 0xFDAF, 0x1F00 }, LEVANTE_SIDE_TONE1_IDX, LOFI_GAIN_LEFT_MASK, LOFI_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_PCM_L_IN_TO_I2S_Both_OUT_LOOP,		   { 0xFDAF, 0x1F00 }, LEVANTE_SIDE_TONE1_IDX, LOFI_GAIN_LEFT_MASK, LOFI_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_PCM_R_IN_TO_I2S_L_OUT_LOOP,			   { 0xFDAF, 0x1F00 }, LEVANTE_SIDE_TONE1_IDX, LOFI_GAIN_LEFT_MASK, LOFI_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_PCM_R_IN_TO_I2S_R_OUT_LOOP,			   { 0xFDAF, 0x1F00 }, LEVANTE_SIDE_TONE1_IDX, LOFI_GAIN_LEFT_MASK, LOFI_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_PCM_R_IN_TO_I2S_Both_OUT_LOOP,		   { 0xFDAF, 0x1F00 }, LEVANTE_SIDE_TONE1_IDX, LOFI_GAIN_LEFT_MASK, LOFI_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_PCM_L_TO_EAR_LOOP,				   {0xFF48, 0x0D00}, LEVANTE_SIDE_TONE1_IDX, LOFI_GAIN_LEFT_MASK, LOFI_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_PCM_L_TO_HS_LOOP,				   {0xFF48, 0x0D00}, LEVANTE_SIDE_TONE1_IDX, LOFI_GAIN_LEFT_MASK, LOFI_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_PCM_L_TO_SPKR_LOOP,				   {0xFF66, 0x0980}, LEVANTE_SIDE_TONE1_IDX, LOFI_GAIN_LEFT_MASK, LOFI_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_SIDETONE_TO_SPKR,				   {0xFF66, 0x0980}, LEVANTE_SIDE_TONE1_IDX, LOFI_GAIN_LEFT_MASK, LOFI_GAIN_LEFT_SHIFT, 1 }
};

/* Gain ID 8 */
#define LEVANTE_DPGA1_HIFIL_GAIN_LEFT_GAIN_MAX 22
static LevanteGains gLevanteDpga1HifilGainLeftGain[LEVANTE_DPGA1_HIFIL_GAIN_LEFT_GAIN_MAX] =
{
	{ LEVANTE_I2S_TO_EAR,					   { 0xFDAF, 0x1F00 }, LEVANTE_LEFT_GAIN1_IDX, HIFIL_GAIN_LEFT_MASK, HIFIL_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_I2S_TO_SPKR,					   { 0xFDAF, 0x1F00 }, LEVANTE_LEFT_GAIN1_IDX, HIFIL_GAIN_LEFT_MASK, HIFIL_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_I2S_TO_HS1,					   { 0xFDAF, 0x1F00 }, LEVANTE_LEFT_GAIN1_IDX, HIFIL_GAIN_LEFT_MASK, HIFIL_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_I2S_TO_HS2,					   { 0xFDAF, 0x1F00 }, LEVANTE_LEFT_GAIN1_IDX, HIFIL_GAIN_LEFT_MASK, HIFIL_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_I2S_TO_HS,					   { 0xFDAF, 0x1F00 }, LEVANTE_LEFT_GAIN1_IDX, HIFIL_GAIN_LEFT_MASK, HIFIL_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_I2S_TO_LINEOUT1,				   { 0xFDAF, 0x1F00 }, LEVANTE_LEFT_GAIN1_IDX, HIFIL_GAIN_LEFT_MASK, HIFIL_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_I2S_TO_LINEOUT2,				   { 0xFDAF, 0x1F00 }, LEVANTE_LEFT_GAIN1_IDX, HIFIL_GAIN_LEFT_MASK, HIFIL_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_I2S_TO_LINEOUT,				   { 0xFDAF, 0x1F00 }, LEVANTE_LEFT_GAIN1_IDX, HIFIL_GAIN_LEFT_MASK, HIFIL_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_I2S1_TO_EAR,					   { 0xFDAF, 0x1F00 }, LEVANTE_LEFT_GAIN1_IDX, HIFIL_GAIN_LEFT_MASK, HIFIL_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_I2S1_TO_SPKR,					   { 0xFDAF, 0x1F00 }, LEVANTE_LEFT_GAIN1_IDX, HIFIL_GAIN_LEFT_MASK, HIFIL_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_I2S1_TO_HS1,					   { 0xFDAF, 0x1F00 }, LEVANTE_LEFT_GAIN1_IDX, HIFIL_GAIN_LEFT_MASK, HIFIL_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_I2S1_TO_HS2,					   { 0xFDAF, 0x1F00 }, LEVANTE_LEFT_GAIN1_IDX, HIFIL_GAIN_LEFT_MASK, HIFIL_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_I2S1_TO_HS,					   { 0xFDAF, 0x1F00 }, LEVANTE_LEFT_GAIN1_IDX, HIFIL_GAIN_LEFT_MASK, HIFIL_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_I2S1_TO_LINEOUT1,				   { 0xFDAF, 0x1F00 }, LEVANTE_LEFT_GAIN1_IDX, HIFIL_GAIN_LEFT_MASK, HIFIL_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_I2S1_TO_LINEOUT2,				   { 0xFDAF, 0x1F00 }, LEVANTE_LEFT_GAIN1_IDX, HIFIL_GAIN_LEFT_MASK, HIFIL_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_I2S1_TO_LINEOUT,				   { 0xFDAF, 0x1F00 }, LEVANTE_LEFT_GAIN1_IDX, HIFIL_GAIN_LEFT_MASK, HIFIL_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_I2S_IN_TO_I2S_L_OUT_LOOP,			   { 0xFDAF, 0x1F00 }, LEVANTE_LEFT_GAIN1_IDX, HIFIL_GAIN_LEFT_MASK, HIFIL_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_I2S_IN_TO_I2S_R_OUT_LOOP,			   { 0xFDAF, 0x1F00 }, LEVANTE_LEFT_GAIN1_IDX, HIFIL_GAIN_LEFT_MASK, HIFIL_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_I2S_IN_TO_I2S_Both_OUT_LOOP,			   { 0xFDAF, 0x1F00 }, LEVANTE_LEFT_GAIN1_IDX, HIFIL_GAIN_LEFT_MASK, HIFIL_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_I2S1_IN_TO_I2S_L_OUT_LOOP,			   { 0xFDAF, 0x1F00 }, LEVANTE_LEFT_GAIN1_IDX, HIFIL_GAIN_LEFT_MASK, HIFIL_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_I2S1_IN_TO_I2S_R_OUT_LOOP,			   { 0xFDAF, 0x1F00 }, LEVANTE_LEFT_GAIN1_IDX, HIFIL_GAIN_LEFT_MASK, HIFIL_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_I2S1_IN_TO_I2S_Both_OUT_LOOP,			   { 0xFDAF, 0x1F00 }, LEVANTE_LEFT_GAIN1_IDX, HIFIL_GAIN_LEFT_MASK, HIFIL_GAIN_LEFT_SHIFT, 1 }
};

/* Gain ID 9 */
#define LEVANTE_DPGA1_HIFIR_GAIN_LEFT_GAIN_MAX 21
static LevanteGains gLevanteDpga1HifirGainLeftGain[LEVANTE_DPGA1_HIFIR_GAIN_LEFT_GAIN_MAX] =
{
	{ LEVANTE_I2S_TO_EAR,					   { 0xFDAF, 0x1F00 }, LEVANTE_RIGHT_GAIN1_IDX, HIFIR_GAIN_LEFT_MASK, HIFIR_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_I2S_TO_SPKR,					   { 0xFDAF, 0x1F00 }, LEVANTE_RIGHT_GAIN1_IDX, HIFIR_GAIN_LEFT_MASK, HIFIR_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_I2S_TO_HS1,					   { 0xFDAF, 0x1F00 }, LEVANTE_RIGHT_GAIN1_IDX, HIFIR_GAIN_LEFT_MASK, HIFIR_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_I2S_TO_HS2,					   { 0xFDAF, 0x1F00 }, LEVANTE_RIGHT_GAIN1_IDX, HIFIR_GAIN_LEFT_MASK, HIFIR_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_I2S_TO_LINEOUT1,				   { 0xFDAF, 0x1F00 }, LEVANTE_RIGHT_GAIN1_IDX, HIFIR_GAIN_LEFT_MASK, HIFIR_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_I2S_TO_LINEOUT2,				   { 0xFDAF, 0x1F00 }, LEVANTE_RIGHT_GAIN1_IDX, HIFIR_GAIN_LEFT_MASK, HIFIR_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_I2S_TO_LINEOUT,				   { 0xFDAF, 0x1F00 }, LEVANTE_RIGHT_GAIN1_IDX, HIFIR_GAIN_LEFT_MASK, HIFIR_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_I2S1_TO_EAR,					   { 0xFDAF, 0x1F00 }, LEVANTE_RIGHT_GAIN1_IDX, HIFIR_GAIN_LEFT_MASK, HIFIR_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_I2S1_TO_SPKR,					   { 0xFDAF, 0x1F00 }, LEVANTE_RIGHT_GAIN1_IDX, HIFIR_GAIN_LEFT_MASK, HIFIR_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_I2S1_TO_HS1,					   { 0xFDAF, 0x1F00 }, LEVANTE_RIGHT_GAIN1_IDX, HIFIR_GAIN_LEFT_MASK, HIFIR_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_I2S1_TO_HS2,					   { 0xFDAF, 0x1F00 }, LEVANTE_RIGHT_GAIN1_IDX, HIFIR_GAIN_LEFT_MASK, HIFIR_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_I2S1_TO_HS,					   { 0xFDAF, 0x1F00 }, LEVANTE_RIGHT_GAIN1_IDX, HIFIR_GAIN_LEFT_MASK, HIFIR_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_I2S1_TO_LINEOUT1,				   { 0xFDAF, 0x1F00 }, LEVANTE_RIGHT_GAIN1_IDX, HIFIR_GAIN_LEFT_MASK, HIFIR_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_I2S1_TO_LINEOUT2,				   { 0xFDAF, 0x1F00 }, LEVANTE_RIGHT_GAIN1_IDX, HIFIR_GAIN_LEFT_MASK, HIFIR_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_I2S1_TO_LINEOUT,				   { 0xFDAF, 0x1F00 }, LEVANTE_RIGHT_GAIN1_IDX, HIFIR_GAIN_LEFT_MASK, HIFIR_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_I2S_IN_TO_I2S_L_OUT_LOOP,			   { 0xFDAF, 0x1F00 }, LEVANTE_RIGHT_GAIN1_IDX, HIFIR_GAIN_LEFT_MASK, HIFIR_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_I2S_IN_TO_I2S_R_OUT_LOOP,			   { 0xFDAF, 0x1F00 }, LEVANTE_RIGHT_GAIN1_IDX, HIFIR_GAIN_LEFT_MASK, HIFIR_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_I2S_IN_TO_I2S_Both_OUT_LOOP,			   { 0xFDAF, 0x1F00 }, LEVANTE_RIGHT_GAIN1_IDX, HIFIR_GAIN_LEFT_MASK, HIFIR_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_I2S1_IN_TO_I2S_L_OUT_LOOP,			   { 0xFDAF, 0x1F00 }, LEVANTE_RIGHT_GAIN1_IDX, HIFIR_GAIN_LEFT_MASK, HIFIR_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_I2S1_IN_TO_I2S_R_OUT_LOOP,			   { 0xFDAF, 0x1F00 }, LEVANTE_RIGHT_GAIN1_IDX, HIFIR_GAIN_LEFT_MASK, HIFIR_GAIN_LEFT_SHIFT, 1 },
	{ LEVANTE_I2S1_IN_TO_I2S_Both_OUT_LOOP,			   { 0xFDAF, 0x1F00 }, LEVANTE_RIGHT_GAIN1_IDX, HIFIR_GAIN_LEFT_MASK, HIFIR_GAIN_LEFT_SHIFT, 1 }
};

/* Gain ID 10 */
#define LEVANTE_DPGA2_LOFI_GAIN_RIGHT_GAIN_MAX 26
static LevanteGains gLevanteDpga2LofiGainRightGain[LEVANTE_DPGA2_LOFI_GAIN_RIGHT_GAIN_MAX] =
{
	{ LEVANTE_PCM_L_TO_EAR,					   { 0xFF48, 0x0B00 }, LEVANTE_SIDE_TONE2_IDX, LOFI_GAIN_RIGHT_MASK, LOFI_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_PCM_L_TO_SPKR,				   { 0xFF66, 0x0980 }, LEVANTE_SIDE_TONE2_IDX, LOFI_GAIN_RIGHT_MASK, LOFI_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_PCM_L_TO_HS1,					   { 0xFDAF, 0x1F00 }, LEVANTE_SIDE_TONE2_IDX, LOFI_GAIN_RIGHT_MASK, LOFI_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_PCM_L_TO_HS2,					   { 0xFDAF, 0x1F00 }, LEVANTE_SIDE_TONE2_IDX, LOFI_GAIN_RIGHT_MASK, LOFI_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_PCM_L_TO_HS,					   { 0xFF48, 0x0D00 }, LEVANTE_SIDE_TONE2_IDX, LOFI_GAIN_RIGHT_MASK, LOFI_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_PCM_L_TO_LINEOUT1,				   { 0xFDAF, 0x1F00 }, LEVANTE_SIDE_TONE2_IDX, LOFI_GAIN_RIGHT_MASK, LOFI_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_PCM_L_TO_LINEOUT2,				   { 0xFDAF, 0x1F00 }, LEVANTE_SIDE_TONE2_IDX, LOFI_GAIN_RIGHT_MASK, LOFI_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_PCM_L_TO_LINEOUT,				   { 0xFDAF, 0x1F00 }, LEVANTE_SIDE_TONE2_IDX, LOFI_GAIN_RIGHT_MASK, LOFI_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_PCM_R_TO_EAR,					   { 0xFDAF, 0x1F00 }, LEVANTE_SIDE_TONE2_IDX, LOFI_GAIN_RIGHT_MASK, LOFI_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_PCM_R_TO_SPKR,				   { 0xFDAF, 0x1F00 }, LEVANTE_SIDE_TONE2_IDX, LOFI_GAIN_RIGHT_MASK, LOFI_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_PCM_R_TO_HS1,					   { 0xFDAF, 0x1F00 }, LEVANTE_SIDE_TONE2_IDX, LOFI_GAIN_RIGHT_MASK, LOFI_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_PCM_R_TO_HS2,					   { 0xFDAF, 0x1F00 }, LEVANTE_SIDE_TONE2_IDX, LOFI_GAIN_RIGHT_MASK, LOFI_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_PCM_R_TO_HS,					   { 0xFDAF, 0x1F00 }, LEVANTE_SIDE_TONE2_IDX, LOFI_GAIN_RIGHT_MASK, LOFI_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_PCM_R_TO_LINEOUT1,				   { 0xFDAF, 0x1F00 }, LEVANTE_SIDE_TONE2_IDX, LOFI_GAIN_RIGHT_MASK, LOFI_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_PCM_R_TO_LINEOUT2,				   { 0xFDAF, 0x1F00 }, LEVANTE_SIDE_TONE2_IDX, LOFI_GAIN_RIGHT_MASK, LOFI_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_PCM_R_TO_LINEOUT,				   { 0xFDAF, 0x1F00 }, LEVANTE_SIDE_TONE2_IDX, LOFI_GAIN_RIGHT_MASK, LOFI_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_PCM_L_IN_TO_I2S_L_OUT_LOOP,			   { 0xFDAF, 0x1F00 }, LEVANTE_SIDE_TONE2_IDX, LOFI_GAIN_RIGHT_MASK, LOFI_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_PCM_L_IN_TO_I2S_R_OUT_LOOP,			   { 0xFDAF, 0x1F00 }, LEVANTE_SIDE_TONE2_IDX, LOFI_GAIN_RIGHT_MASK, LOFI_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_PCM_L_IN_TO_I2S_Both_OUT_LOOP,		   { 0xFDAF, 0x1F00 }, LEVANTE_SIDE_TONE2_IDX, LOFI_GAIN_RIGHT_MASK, LOFI_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_PCM_R_IN_TO_I2S_L_OUT_LOOP,			   { 0xFDAF, 0x1F00 }, LEVANTE_SIDE_TONE2_IDX, LOFI_GAIN_RIGHT_MASK, LOFI_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_PCM_R_IN_TO_I2S_R_OUT_LOOP,			   { 0xFDAF, 0x1F00 }, LEVANTE_SIDE_TONE2_IDX, LOFI_GAIN_RIGHT_MASK, LOFI_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_PCM_R_IN_TO_I2S_Both_OUT_LOOP,		   { 0xFDAF, 0x1F00 }, LEVANTE_SIDE_TONE2_IDX, LOFI_GAIN_RIGHT_MASK, LOFI_GAIN_RIGHT_SHIFT, 1 },

	{ LEVANTE_PCM_L_TO_EAR_LOOP,		 {0xFF48, 0x0D00}, LEVANTE_SIDE_TONE2_IDX, LOFI_GAIN_RIGHT_MASK, LOFI_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_PCM_L_TO_HS_LOOP,				 {0xFF48, 0x0D00}, LEVANTE_SIDE_TONE2_IDX, LOFI_GAIN_RIGHT_MASK, LOFI_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_PCM_L_TO_SPKR_LOOP,		 {0xFF66, 0x0980}, LEVANTE_SIDE_TONE2_IDX, LOFI_GAIN_RIGHT_MASK, LOFI_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_SIDETONE_TO_SPKR,		 {0xFF66, 0x0980}, LEVANTE_SIDE_TONE2_IDX, LOFI_GAIN_RIGHT_MASK, LOFI_GAIN_RIGHT_SHIFT, 1 }
};

/* Gain ID 11 */
#define LEVANTE_DPGA2_HIFIL_GAIN_RIGHT_GAIN_MAX 21
static LevanteGains gLevanteDpga2HifilGainRightGain[LEVANTE_DPGA2_HIFIL_GAIN_RIGHT_GAIN_MAX] =
{
	{ LEVANTE_I2S_TO_EAR,					   { 0xFDAF, 0x1F00 }, LEVANTE_LEFT_GAIN2_IDX, HIFIL_GAIN_RIGHT_MASK, HIFIL_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_I2S_TO_SPKR,					   { 0xFDAF, 0x1F00 }, LEVANTE_LEFT_GAIN2_IDX, HIFIL_GAIN_RIGHT_MASK, HIFIL_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_I2S_TO_HS1,					   { 0xFDAF, 0x1F00 }, LEVANTE_LEFT_GAIN2_IDX, HIFIL_GAIN_RIGHT_MASK, HIFIL_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_I2S_TO_HS2,					   { 0xFDAF, 0x1F00 }, LEVANTE_LEFT_GAIN2_IDX, HIFIL_GAIN_RIGHT_MASK, HIFIL_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_I2S_TO_LINEOUT1,				   { 0xFDAF, 0x1F00 }, LEVANTE_LEFT_GAIN2_IDX, HIFIL_GAIN_RIGHT_MASK, HIFIL_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_I2S_TO_LINEOUT2,				   { 0xFDAF, 0x1F00 }, LEVANTE_LEFT_GAIN2_IDX, HIFIL_GAIN_RIGHT_MASK, HIFIL_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_I2S_TO_LINEOUT,				   { 0xFDAF, 0x1F00 }, LEVANTE_LEFT_GAIN2_IDX, HIFIL_GAIN_RIGHT_MASK, HIFIL_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_I2S1_TO_EAR,					   { 0xFDAF, 0x1F00 }, LEVANTE_LEFT_GAIN2_IDX, HIFIL_GAIN_RIGHT_MASK, HIFIL_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_I2S1_TO_SPKR,					   { 0xFDAF, 0x1F00 }, LEVANTE_LEFT_GAIN2_IDX, HIFIL_GAIN_RIGHT_MASK, HIFIL_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_I2S1_TO_HS1,					   { 0xFDAF, 0x1F00 }, LEVANTE_LEFT_GAIN2_IDX, HIFIL_GAIN_RIGHT_MASK, HIFIL_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_I2S1_TO_HS2,					   { 0xFDAF, 0x1F00 }, LEVANTE_LEFT_GAIN2_IDX, HIFIL_GAIN_RIGHT_MASK, HIFIL_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_I2S1_TO_HS,					   { 0xFDAF, 0x1F00 }, LEVANTE_LEFT_GAIN2_IDX, HIFIL_GAIN_RIGHT_MASK, HIFIL_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_I2S1_TO_LINEOUT1,				   { 0xFDAF, 0x1F00 }, LEVANTE_LEFT_GAIN2_IDX, HIFIL_GAIN_RIGHT_MASK, HIFIL_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_I2S1_TO_LINEOUT2,				   { 0xFDAF, 0x1F00 }, LEVANTE_LEFT_GAIN2_IDX, HIFIL_GAIN_RIGHT_MASK, HIFIL_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_I2S1_TO_LINEOUT,				   { 0xFDAF, 0x1F00 }, LEVANTE_LEFT_GAIN2_IDX, HIFIL_GAIN_RIGHT_MASK, HIFIL_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_I2S_IN_TO_I2S_L_OUT_LOOP,			   { 0xFDAF, 0x1F00 }, LEVANTE_LEFT_GAIN2_IDX, HIFIL_GAIN_RIGHT_MASK, HIFIL_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_I2S_IN_TO_I2S_R_OUT_LOOP,			   { 0xFDAF, 0x1F00 }, LEVANTE_LEFT_GAIN2_IDX, HIFIL_GAIN_RIGHT_MASK, HIFIL_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_I2S_IN_TO_I2S_Both_OUT_LOOP,			   { 0xFDAF, 0x1F00 }, LEVANTE_LEFT_GAIN2_IDX, HIFIL_GAIN_RIGHT_MASK, HIFIL_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_I2S1_IN_TO_I2S_L_OUT_LOOP,			   { 0xFDAF, 0x1F00 }, LEVANTE_LEFT_GAIN2_IDX, HIFIL_GAIN_RIGHT_MASK, HIFIL_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_I2S1_IN_TO_I2S_R_OUT_LOOP,			   { 0xFDAF, 0x1F00 }, LEVANTE_LEFT_GAIN2_IDX, HIFIL_GAIN_RIGHT_MASK, HIFIL_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_I2S1_IN_TO_I2S_Both_OUT_LOOP,			   { 0xFDAF, 0x1F00 }, LEVANTE_LEFT_GAIN2_IDX, HIFIL_GAIN_RIGHT_MASK, HIFIL_GAIN_RIGHT_SHIFT, 1 }
};

/* Gain ID 12 */
#define LEVANTE_DPGA2_HIFIR_GAIN_RIGHT_GAIN_MAX 22
static LevanteGains gLevanteDpga2HifirGainRightGain[LEVANTE_DPGA2_HIFIR_GAIN_RIGHT_GAIN_MAX] =
{
	{ LEVANTE_I2S_TO_EAR,					   { 0xFDAF, 0x1F00 }, LEVANTE_RIGHT_GAIN2_IDX, HIFIR_GAIN_RIGHT_MASK, HIFIR_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_I2S_TO_SPKR,					   { 0xFDAF, 0x1F00 }, LEVANTE_RIGHT_GAIN2_IDX, HIFIR_GAIN_RIGHT_MASK, HIFIR_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_I2S_TO_HS1,					   { 0xFDAF, 0x1F00 }, LEVANTE_RIGHT_GAIN2_IDX, HIFIR_GAIN_RIGHT_MASK, HIFIR_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_I2S_TO_HS2,					   { 0xFDAF, 0x1F00 }, LEVANTE_RIGHT_GAIN2_IDX, HIFIR_GAIN_RIGHT_MASK, HIFIR_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_I2S_TO_HS,					   { 0xFDAF, 0x1F00 }, LEVANTE_RIGHT_GAIN2_IDX, HIFIR_GAIN_RIGHT_MASK, HIFIR_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_I2S_TO_LINEOUT1,				   { 0xFDAF, 0x1F00 }, LEVANTE_RIGHT_GAIN2_IDX, HIFIR_GAIN_RIGHT_MASK, HIFIR_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_I2S_TO_LINEOUT2,				   { 0xFDAF, 0x1F00 }, LEVANTE_RIGHT_GAIN2_IDX, HIFIR_GAIN_RIGHT_MASK, HIFIR_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_I2S_TO_LINEOUT,				   { 0xFDAF, 0x1F00 }, LEVANTE_RIGHT_GAIN2_IDX, HIFIR_GAIN_RIGHT_MASK, HIFIR_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_I2S1_TO_EAR,					   { 0xFDAF, 0x1F00 }, LEVANTE_RIGHT_GAIN2_IDX, HIFIR_GAIN_RIGHT_MASK, HIFIR_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_I2S1_TO_SPKR,					   { 0xFDAF, 0x1F00 }, LEVANTE_RIGHT_GAIN2_IDX, HIFIR_GAIN_RIGHT_MASK, HIFIR_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_I2S1_TO_HS1,					   { 0xFDAF, 0x1F00 }, LEVANTE_RIGHT_GAIN2_IDX, HIFIR_GAIN_RIGHT_MASK, HIFIR_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_I2S1_TO_HS2,					   { 0xFDAF, 0x1F00 }, LEVANTE_RIGHT_GAIN2_IDX, HIFIR_GAIN_RIGHT_MASK, HIFIR_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_I2S1_TO_HS,					   { 0xFDAF, 0x1F00 }, LEVANTE_RIGHT_GAIN2_IDX, HIFIR_GAIN_RIGHT_MASK, HIFIR_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_I2S1_TO_LINEOUT1,				   { 0xFDAF, 0x1F00 }, LEVANTE_RIGHT_GAIN2_IDX, HIFIR_GAIN_RIGHT_MASK, HIFIR_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_I2S1_TO_LINEOUT2,				   { 0xFDAF, 0x1F00 }, LEVANTE_RIGHT_GAIN2_IDX, HIFIR_GAIN_RIGHT_MASK, HIFIR_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_I2S1_TO_LINEOUT,				   { 0xFDAF, 0x1F00 }, LEVANTE_RIGHT_GAIN2_IDX, HIFIR_GAIN_RIGHT_MASK, HIFIR_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_I2S_IN_TO_I2S_L_OUT_LOOP,			   { 0xFDAF, 0x1F00 }, LEVANTE_RIGHT_GAIN2_IDX, HIFIR_GAIN_RIGHT_MASK, HIFIR_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_I2S_IN_TO_I2S_R_OUT_LOOP,			   { 0xFDAF, 0x1F00 }, LEVANTE_RIGHT_GAIN2_IDX, HIFIR_GAIN_RIGHT_MASK, HIFIR_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_I2S_IN_TO_I2S_Both_OUT_LOOP,			   { 0xFDAF, 0x1F00 }, LEVANTE_RIGHT_GAIN2_IDX, HIFIR_GAIN_RIGHT_MASK, HIFIR_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_I2S1_IN_TO_I2S_L_OUT_LOOP,			   { 0xFDAF, 0x1F00 }, LEVANTE_RIGHT_GAIN2_IDX, HIFIR_GAIN_RIGHT_MASK, HIFIR_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_I2S1_IN_TO_I2S_R_OUT_LOOP,			   { 0xFDAF, 0x1F00 }, LEVANTE_RIGHT_GAIN2_IDX, HIFIR_GAIN_RIGHT_MASK, HIFIR_GAIN_RIGHT_SHIFT, 1 },
	{ LEVANTE_I2S1_IN_TO_I2S_Both_OUT_LOOP,			   { 0xFDAF, 0x1F00 }, LEVANTE_RIGHT_GAIN2_IDX, HIFIR_GAIN_RIGHT_MASK, HIFIR_GAIN_RIGHT_SHIFT, 1 }
};

/* Gain ID 13 */
#define LEVANTE_PGA_MIC1_GAIN_MAX 21
static LevanteGains gLevantePgaMic1Gain[LEVANTE_PGA_MIC1_GAIN_MAX] =
{
	{ LEVANTE_AnaMIC1_TO_PCM_L,	      { 0,    0x0080 }, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_PGA_MIC1_MASK, GAIN_PGA_MIC1_SHIFT, 2 },
	{ LEVANTE_AnaMIC1_LOUD_TO_PCM_L,      { 0,    0x0080 }, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_PGA_MIC1_MASK, GAIN_PGA_MIC1_SHIFT, 2 },
	{ LEVANTE_AnaMIC2_TO_PCM_L,	      { 0,    0x0080 }, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_PGA_MIC1_MASK, GAIN_PGA_MIC1_SHIFT, 2 },
	{ LEVANTE_AnaMIC1_TO_PCM_R,	      { 0,    0	     }, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_PGA_MIC1_MASK, GAIN_PGA_MIC1_SHIFT, 2 },
	{ LEVANTE_AnaMIC1_LOUD_TO_PCM_R,      { 0,    0	     }, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_PGA_MIC1_MASK, GAIN_PGA_MIC1_SHIFT, 2 },
	{ LEVANTE_AnaMIC2_TO_PCM_R,	      { 0,    0	     }, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_PGA_MIC1_MASK, GAIN_PGA_MIC1_SHIFT, 2 },
	{ LEVANTE_AnaMIC1_TO_I2S_L,	      { 0x43, 0	     }, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_PGA_MIC1_MASK, GAIN_PGA_MIC1_SHIFT, 2 },
	{ LEVANTE_AnaMIC1_LOUD_TO_I2S_L,      { 0x43, 0	     }, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_PGA_MIC1_MASK, GAIN_PGA_MIC1_SHIFT, 2 },
	{ LEVANTE_AnaMIC2_TO_I2S_L,	      { 0,    0	     }, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_PGA_MIC1_MASK, GAIN_PGA_MIC1_SHIFT, 2 },
	{ LEVANTE_AnaMIC1_TO_I2S_R,	      { 0x43, 0	     }, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_PGA_MIC1_MASK, GAIN_PGA_MIC1_SHIFT, 2 },
	{ LEVANTE_AnaMIC1_LOUD_TO_I2S_R,      { 0x43, 0	     }, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_PGA_MIC1_MASK, GAIN_PGA_MIC1_SHIFT, 2 },
	{ LEVANTE_AnaMIC2_TO_I2S_R,	      { 0,    0	     }, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_PGA_MIC1_MASK, GAIN_PGA_MIC1_SHIFT, 2 },

	{ LEVANTE_AnaMIC1_TO_PCM_L_LOOP, {0, 0x0080}, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_PGA_MIC1_MASK, GAIN_PGA_MIC1_SHIFT, 2 },
	{ LEVANTE_AnaMIC2_TO_PCM_L_LOOP, {0, 0	   }, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_PGA_MIC1_MASK, GAIN_PGA_MIC1_SHIFT, 2 },
	{ LEVANTE_AnaMIC1_LOUD_TO_PCM_L_LOOP, {0, 0	}, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_PGA_MIC1_MASK, GAIN_PGA_MIC1_SHIFT, 2 },

	{ LEVANTE_AnaMIC1_TO_PCM_I2S_L,	      { 0,    0x0380 }, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_PGA_MIC1_MASK, GAIN_PGA_MIC1_SHIFT, 2 },
	{ LEVANTE_AnaMIC1_TO_PCM_I2S_R,	      { 0,    0x0380 }, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_PGA_MIC1_MASK, GAIN_PGA_MIC1_SHIFT, 2 },
	{ LEVANTE_AnaMIC1_LOUD_TO_PCM_I2S_L,      { 0,    0x0380 }, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_PGA_MIC1_MASK, GAIN_PGA_MIC1_SHIFT, 2 },
	{ LEVANTE_AnaMIC1_LOUD_TO_PCM_I2S_R,      { 0,    0x0380 }, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_PGA_MIC1_MASK, GAIN_PGA_MIC1_SHIFT, 2 },
	{ LEVANTE_AnaMIC2_TO_PCM_I2S_L,	      { 0,    0x0380   }, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_PGA_MIC1_MASK, GAIN_PGA_MIC1_SHIFT, 2 },
	{ LEVANTE_AnaMIC2_TO_PCM_I2S_R,	      { 0,    0x0380   }, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_PGA_MIC1_MASK, GAIN_PGA_MIC1_SHIFT, 2 }
};

/* Gain ID 14 */
#define LEVANTE_PGA_MIC3_GAIN_MAX 4
static LevanteGains gLevantePgaMic3Gain[LEVANTE_PGA_MIC3_GAIN_MAX] =
{
	{ LEVANTE_AnaMIC3_TO_PCM_R, { 0x0, 0x0 }, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_PGA_MIC2_MASK, GAIN_PGA_MIC2_SHIFT, 2 },
	{ LEVANTE_AnaMIC3_TO_PCM_L, { 0x0, 0x0 }, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_PGA_MIC2_MASK, GAIN_PGA_MIC2_SHIFT, 2 },
	{ LEVANTE_AnaMIC3_TO_I2S_R, { 0x0, 0x0 }, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_PGA_MIC2_MASK, GAIN_PGA_MIC2_SHIFT, 2 },
	{ LEVANTE_AnaMIC3_TO_I2S_L, { 0x0, 0x0 }, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_PGA_MIC2_MASK, GAIN_PGA_MIC2_SHIFT, 2 }
};

/* Gain ID 15 */
#define LEVANTE_PGA_AUX1_GAIN_MAX 10
static LevanteGains gLevantePgaAux1Gain[LEVANTE_PGA_AUX1_GAIN_MAX] =
{
	{ LEVANTE_AUX1_TO_PCM_L,   { 0x0,  0x0 }, LEVANTE_ADC_ANALOG_PROGRAM3_IDX, GAIN_PGA_AUX1_MASK, GAIN_PGA_AUX1_SHIFT, 3 },
	{ LEVANTE_AUX1_TO_PCM_R,   { 0x0,  0x0 }, LEVANTE_ADC_ANALOG_PROGRAM3_IDX, GAIN_PGA_AUX1_MASK, GAIN_PGA_AUX1_SHIFT, 3 },
	{ LEVANTE_AUX1_TO_I2S_L,   { 0x1A, 0x0 }, LEVANTE_ADC_ANALOG_PROGRAM3_IDX, GAIN_PGA_AUX1_MASK, GAIN_PGA_AUX1_SHIFT, 3 },
	{ LEVANTE_AUX1_TO_I2S_R,   { 0x1A, 0x0 }, LEVANTE_ADC_ANALOG_PROGRAM3_IDX, GAIN_PGA_AUX1_MASK, GAIN_PGA_AUX1_SHIFT, 3 },
	{ LEVANTE_AUX1_TO_HS2,	   { 0x1A,  0x0 }, LEVANTE_ADC_ANALOG_PROGRAM3_IDX, GAIN_PGA_AUX1_MASK, GAIN_PGA_AUX1_SHIFT, 3 },
	{ LEVANTE_AUX1_TO_LOUT2,   { 0x0,  0x0 }, LEVANTE_ADC_ANALOG_PROGRAM3_IDX, GAIN_PGA_AUX1_MASK, GAIN_PGA_AUX1_SHIFT, 3 },
	{ LEVANTE_AUX2_TO_HS1,	   { 0x1A,  0x0 }, LEVANTE_ADC_ANALOG_PROGRAM3_IDX, GAIN_PGA_AUX2_MASK, GAIN_PGA_AUX2_SHIFT, 3 },
	{ LEVANTE_AUX2_TO_LOUT1,   { 0x0,  0x0 }, LEVANTE_ADC_ANALOG_PROGRAM3_IDX, GAIN_PGA_AUX2_MASK, GAIN_PGA_AUX2_SHIFT, 3 },
	{ LEVANTE_AUX1_TO_SIDETONE_L,{ 0x0,  0x180}, LEVANTE_ADC_ANALOG_PROGRAM3_IDX, GAIN_PGA_AUX1_MASK, GAIN_PGA_AUX1_SHIFT, 3 },
	{ LEVANTE_AUX1_TO_PCM_I2S_L,   { 0x1A,  0x0 }, LEVANTE_ADC_ANALOG_PROGRAM3_IDX, GAIN_PGA_AUX1_MASK, GAIN_PGA_AUX1_SHIFT, 3 }
};

/* Gain ID 16 */
#define LEVANTE_PGA_AUX2_GAIN_MAX 6
static LevanteGains gLevantePgaAux2Gain[LEVANTE_PGA_AUX2_GAIN_MAX] =
{
	{ LEVANTE_AUX2_TO_PCM_R, { 0x0,	 0x0 }, LEVANTE_ADC_ANALOG_PROGRAM3_IDX, GAIN_PGA_AUX2_MASK, GAIN_PGA_AUX2_SHIFT, 3 },
	{ LEVANTE_AUX2_TO_PCM_L, { 0x0,	 0x0 }, LEVANTE_ADC_ANALOG_PROGRAM3_IDX, GAIN_PGA_AUX2_MASK, GAIN_PGA_AUX2_SHIFT, 3 },
	{ LEVANTE_AUX2_TO_I2S_R, { 0x1A, 0x0 }, LEVANTE_ADC_ANALOG_PROGRAM3_IDX, GAIN_PGA_AUX2_MASK, GAIN_PGA_AUX2_SHIFT, 3 },
	{ LEVANTE_AUX2_TO_I2S_L, { 0x1A, 0x0 }, LEVANTE_ADC_ANALOG_PROGRAM3_IDX, GAIN_PGA_AUX2_MASK, GAIN_PGA_AUX2_SHIFT, 3 },
	{ LEVANTE_AUX2_TO_SIDETONE_R, { 0x0,0x180 }, LEVANTE_ADC_ANALOG_PROGRAM3_IDX, GAIN_PGA_AUX2_MASK, GAIN_PGA_AUX2_SHIFT, 3 },
	{ LEVANTE_AUX2_TO_PCM_I2S_R, { 0x1A,	 0x0 }, LEVANTE_ADC_ANALOG_PROGRAM3_IDX, GAIN_PGA_AUX2_MASK, GAIN_PGA_AUX2_SHIFT, 3 }
};

/* Gain ID 17 */
#define LEVANTE_SIDETONE_GAIN_R_MAX 6
static LevanteGains gLevanteSidetoneRGain[LEVANTE_SIDETONE_GAIN_R_MAX] =
{
	{ LEVANTE_IN_R_TO_OUT,	     { 0x0, 0x0 }, LEVANTE_SIDETONE_GAIN2_IDX, ST_GAINR_MASK,  ST_GAINR_SHIFT,	4  },
	{ LEVANTE_IN_R_TO_OUT,	     { 0x0, 0x0 }, LEVANTE_SIDETONE_GAIN3_IDX, ST_SHIFTR_MASK, ST_SHIFTR_SHIFT, 5  },
	{ LEVANTE_IN_BOTH_TO_OUT,    { 0x0, 0x0 }, LEVANTE_SIDETONE_GAIN2_IDX, ST_GAINR_MASK,  ST_GAINR_SHIFT,	4  },
	{ LEVANTE_IN_BOTH_TO_OUT,    { 0x0, 0x0 }, LEVANTE_SIDETONE_GAIN3_IDX, ST_SHIFTR_MASK, ST_SHIFTR_SHIFT, 5  },
	{ LEVANTE_SIDETONE_IN_TO_OUT, { 0x0, 0x200 }, LEVANTE_SIDETONE_GAIN2_IDX, ST_GAINR_MASK,  ST_GAINR_SHIFT,	4  },
	{ LEVANTE_SIDETONE_IN_TO_OUT, { 0x0, 0x1E00 }, LEVANTE_SIDETONE_GAIN3_IDX, ST_SHIFTR_MASK, ST_SHIFTR_SHIFT, 5  }
};

/* Gain ID 18 */
#define LEVANTE_SIDETONE_GAIN_L_MAX 6
static LevanteGains gLevanteSidetoneLGain[LEVANTE_SIDETONE_GAIN_L_MAX] =
{
	{ LEVANTE_IN_L_TO_OUT,	     { 0x0, 0x0 }, LEVANTE_SIDETONE_GAIN1_IDX, ST_GAINL_MASK,  ST_GAINL_SHIFT,	4  },
	{ LEVANTE_IN_L_TO_OUT,	     { 0x0, 0x0 }, LEVANTE_SIDETONE_GAIN3_IDX, ST_SHIFTL_MASK, ST_SHIFTL_SHIFT, 5  },
	{ LEVANTE_IN_BOTH_TO_OUT,    { 0x0, 0x0 }, LEVANTE_SIDETONE_GAIN1_IDX, ST_GAINL_MASK,  ST_GAINL_SHIFT,	4  },
	{ LEVANTE_IN_BOTH_TO_OUT,    { 0x0, 0x0 }, LEVANTE_SIDETONE_GAIN3_IDX, ST_SHIFTL_MASK, ST_SHIFTL_SHIFT, 5  },
	{ LEVANTE_SIDETONE_IN_TO_OUT, { 0x0, 0x200 }, LEVANTE_SIDETONE_GAIN1_IDX, ST_GAINL_MASK,  ST_GAINL_SHIFT,	4  },
	{ LEVANTE_SIDETONE_IN_TO_OUT, { 0x0, 0x1E00 }, LEVANTE_SIDETONE_GAIN3_IDX, ST_SHIFTL_MASK, ST_SHIFTL_SHIFT, 5  }
};

/* Gain ID 19 */
#define LEVANTE_MOD1_GAIN_MAX 27
static LevanteGains gLevanteMod1Gain[LEVANTE_MOD1_GAIN_MAX] =
{
	{ LEVANTE_AnaMIC1_TO_PCM_L,		 { 0x0,	 0x0100 }, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_MOD1_MASK, GAIN_MOD1_SHIFT, 6 },
	{ LEVANTE_AnaMIC1_LOUD_TO_PCM_L,	 { 0x0,	 0x0100	}, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_MOD1_MASK, GAIN_MOD1_SHIFT, 6 },
	{ LEVANTE_AnaMIC2_TO_PCM_L,		 { 0x0,	 0x0100	}, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_MOD1_MASK, GAIN_MOD1_SHIFT, 6 },
	{ LEVANTE_AUX1_TO_PCM_L,		 { 0x0,	 0x0	}, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_MOD1_MASK, GAIN_MOD1_SHIFT, 6 },
	{ LEVANTE_AnaMIC1_TO_PCM_R,		 { 0x0,	 0x0	}, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_MOD1_MASK, GAIN_MOD1_SHIFT, 6 },
	{ LEVANTE_AnaMIC1_LOUD_TO_PCM_R,	 { 0x0,	 0x0	}, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_MOD1_MASK, GAIN_MOD1_SHIFT, 6 },
	{ LEVANTE_AnaMIC2_TO_PCM_R,		 { 0x0,	 0x0	}, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_MOD1_MASK, GAIN_MOD1_SHIFT, 6 },
	{ LEVANTE_AUX1_TO_PCM_R,		 { 0x0,	 0x0	}, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_MOD1_MASK, GAIN_MOD1_SHIFT, 6 },
	{ LEVANTE_AnaMIC1_TO_I2S_L,		 { 0x1A, 0x0	}, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_MOD1_MASK, GAIN_MOD1_SHIFT, 6 },
	{ LEVANTE_AnaMIC1_LOUD_TO_I2S_L,	 { 0x1A, 0x0	}, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_MOD1_MASK, GAIN_MOD1_SHIFT, 6 },
	{ LEVANTE_AnaMIC2_TO_I2S_L,		 { 0x0,	 0x0	}, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_MOD1_MASK, GAIN_MOD1_SHIFT, 6 },
	{ LEVANTE_AUX1_TO_I2S_L,		 { 0x0,	 0x0	}, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_MOD1_MASK, GAIN_MOD1_SHIFT, 6 },
	{ LEVANTE_AnaMIC1_TO_I2S_R,		 { 0x1A, 0x0	}, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_MOD1_MASK, GAIN_MOD1_SHIFT, 6 },
	{ LEVANTE_AnaMIC1_LOUD_TO_I2S_R,	 { 0x1A, 0x0	}, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_MOD1_MASK, GAIN_MOD1_SHIFT, 6 },
	{ LEVANTE_AnaMIC2_TO_I2S_R,		 { 0x0,	 0x0	}, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_MOD1_MASK, GAIN_MOD1_SHIFT, 6 },
	{ LEVANTE_AUX1_TO_I2S_R,		 { 0x0,	 0x0	}, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_MOD1_MASK, GAIN_MOD1_SHIFT, 6 },

	{ LEVANTE_AnaMIC1_TO_PCM_L_LOOP, {0x0, 0x0100}, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_MOD1_MASK, GAIN_MOD1_SHIFT, 6 },
	{ LEVANTE_AnaMIC2_TO_PCM_L_LOOP, {0x0, 0x0   }, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_MOD1_MASK, GAIN_MOD1_SHIFT, 6 },
	{ LEVANTE_AnaMIC1_LOUD_TO_PCM_L_LOOP, {0x0, 0x0   }, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_MOD1_MASK, GAIN_MOD1_SHIFT, 6 },
	{ LEVANTE_AUX1_TO_SIDETONE_L,		 { 0x0,	 0x0	}, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_MOD1_MASK, GAIN_MOD1_SHIFT, 6 },

	{ LEVANTE_AnaMIC1_TO_PCM_I2S_L,		 { 0x0,	 0x0180 }, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_MOD1_MASK, GAIN_MOD1_SHIFT, 6 },
	{ LEVANTE_AnaMIC1_TO_PCM_I2S_R,		 { 0x0,	 0x0180	}, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_MOD1_MASK, GAIN_MOD1_SHIFT, 6 },
	{ LEVANTE_AnaMIC1_LOUD_TO_PCM_I2S_L,	 { 0x0,	 0x0180	}, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_MOD1_MASK, GAIN_MOD1_SHIFT, 6 },
	{ LEVANTE_AnaMIC1_LOUD_TO_PCM_I2S_R,	 { 0x0,	 0x0180	}, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_MOD1_MASK, GAIN_MOD1_SHIFT, 6 },
	{ LEVANTE_AnaMIC2_TO_PCM_I2S_L,		 { 0x0,	 0x0180	}, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_MOD1_MASK, GAIN_MOD1_SHIFT, 6 },
	{ LEVANTE_AnaMIC2_TO_PCM_I2S_R,		 { 0x0,	 0x0180	}, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_MOD1_MASK, GAIN_MOD1_SHIFT, 6 },
	{ LEVANTE_AUX1_TO_PCM_I2S_L,		 { 0x0,	 0x0180	}, LEVANTE_ADC_ANALOG_PROGRAM2_IDX, GAIN_MOD1_MASK, GAIN_MOD1_SHIFT, 6 }

};

/* Gain ID 20 */
#define LEVANTE_MOD2_GAIN_MAX 10
static LevanteGains gLevanteMod2Gain[LEVANTE_MOD2_GAIN_MAX] =
{
	{ LEVANTE_AnaMIC3_TO_PCM_R,    { 0x0, 0x0 }, LEVANTE_ADC_ANALOG_PROGRAM3_IDX, GAIN_MOD2_MASK, GAIN_MOD2_SHIFT, 6 },
	{ LEVANTE_AUX2_TO_PCM_R,       { 0x0, 0x0 }, LEVANTE_ADC_ANALOG_PROGRAM3_IDX, GAIN_MOD2_MASK, GAIN_MOD2_SHIFT, 6 },
	{ LEVANTE_AnaMIC3_TO_PCM_L,    { 0x0, 0x0 }, LEVANTE_ADC_ANALOG_PROGRAM3_IDX, GAIN_MOD2_MASK, GAIN_MOD2_SHIFT, 6 },
	{ LEVANTE_AUX2_TO_PCM_L,       { 0x0, 0x0 }, LEVANTE_ADC_ANALOG_PROGRAM3_IDX, GAIN_MOD2_MASK, GAIN_MOD2_SHIFT, 6 },
	{ LEVANTE_AnaMIC3_TO_I2S_R,    { 0x0, 0x0 }, LEVANTE_ADC_ANALOG_PROGRAM3_IDX, GAIN_MOD2_MASK, GAIN_MOD2_SHIFT, 6 },
	{ LEVANTE_AUX2_TO_I2S_R,       { 0x0, 0x0 }, LEVANTE_ADC_ANALOG_PROGRAM3_IDX, GAIN_MOD2_MASK, GAIN_MOD2_SHIFT, 6 },
	{ LEVANTE_AnaMIC3_TO_I2S_L,    { 0x0, 0x0 }, LEVANTE_ADC_ANALOG_PROGRAM3_IDX, GAIN_MOD2_MASK, GAIN_MOD2_SHIFT, 6 },
	{ LEVANTE_AUX2_TO_I2S_L,       { 0x0, 0x0 }, LEVANTE_ADC_ANALOG_PROGRAM3_IDX, GAIN_MOD2_MASK, GAIN_MOD2_SHIFT, 6 },
	{ LEVANTE_AUX2_TO_SIDETONE_R,  { 0x0, 0x0 }, LEVANTE_ADC_ANALOG_PROGRAM3_IDX, GAIN_MOD2_MASK, GAIN_MOD2_SHIFT, 6 },
	{ LEVANTE_AUX2_TO_PCM_I2S_R,   { 0x0, 0x0180 }, LEVANTE_ADC_ANALOG_PROGRAM3_IDX, GAIN_MOD2_MASK, GAIN_MOD2_SHIFT, 6 }
};


/*----------- Local constant definitions -------------------------------------*/
// SSP configuration for 8 KHz
static const ACM_SspAudioConfiguration _sspAudioLevante8KHzConfiguration =
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
	/* devicePort  */ 0,            /* (GSSP0_PORT=0, GSSP1_PORT=1) */
	/* SSP_Rate;   */ 0,            /* (0=8 kHz, 1=16 kHz)    */
	/* SSP_Mode;   */ 0             /* (0=unpacked, 1=packed) */
};

// SSP configuration for 16 KHz
static const ACM_SspAudioConfiguration _sspAudioLevante16KHzConfiguration =
{
	/* shift       */ 0,
	/* SSCR0_HIGH  */ 0xE0C0,
	/* SSCR0_LOW   */ 0x003F,
	/* SSCR1_HIGH  */ 0x6000,
	/* SSCR1_LOW   */ 0x1C01,
	/* SSTSA_LOW   */ 0x0001,
	/* SSRSA_LOW   */ 0x0001,
	/* SSPSP_HIGH  */ 0x3181,
	/* SSPSP_LOW   */ 0x0015,
	/* SSACD_LOW   */ 0x00EA,
	/* SSACDD_HIGH */ 0x130B,
	/* SSACDD_LOW  */ 0x0010,
	/* devicePort  */ 0,            /* (GSSP0_PORT=0, GSSP1_PORT=1) */
	/* SSP_Rate;   */ 1,            /* (0=8 kHz, 1=16 kHz)    */
	/* SSP_Mode;   */ 1             /* (0=unpacked, 1=packed) */
};

static const unsigned char gLevanteGainTableTranslate[][65] =
{
/* Table ID*/
	/* 0 */ { 4,	       2,	     1,	 6,  5,	 3,  7,	 0  },
	/* 1 */ { TRANSPARENT, 0,	     0,	 0,  0,	 0,  0,	 0  },
	/* 2 */ { TRANSPARENT, 0,	     0,	 0,  0,	 0,  0,	 0  },
	/* 3 */ { 3,	       0,	     5,	 6,  7,	 0,  0,	 0  },
	/* 4 */ { 17,	       20,	     23, 28, 32, 19, 23, 27, 32, 19, 23, 27, 32, 19, 23, 27, 32, 19, 23, 27, 32, 19, 23, 27, 32, 19, 23, 27, 32, 19, 23, 27, 32,
		  19, 23, 27, 17, 19, 23, 27, 32, 19, 23, 27, 32, 19, 23, 27, 32, 19, 23, 27, 32, 19, 23, 27, 32, 19, 23, 27, 32, 36, 40, 53, 63 },
	/* 5 */ { 14,	       14,	     14, 14, 14, 13, 13, 13, 13, 12, 12, 12, 12, 11, 11, 11, 11, 10, 10, 10, 10,  9,  9,  9,  9,  8,  8,  8,  8,  7,  7,  7,  7,
		  6,  6,  6,  5,  5,  5,  5,  5,  4,  4,  4,  4,  3,  3,  3,  3,  2,  2,  2,  2,  1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0 },
	/* 6 */ { TRANSPARENT, 0,	     0,	 0,  0,	 0,  0,	 0  }
};

static RegisterInfo registerInfo[] =
{
	/* index                             address                               default value */
	{ LEVANTE_MISC2_IDX,		    LEVANTE_MISC2_ADDRESS,		  LEVANTE_MISC2_DEFAULT_VALUE				    },
	{ LEVANTE_PLL_CTRL1_IDX,	    LEVANTE_PLL_CTRL1_ADDRESS,		  LEVANTE_PLL_CTRL1_DEFAULT_VALUE			    },
	{ LEVANTE_PLL_FRAC1_IDX,	    LEVANTE_PLL_FRAC1_ADDRESS,		  LEVANTE_PLL_FRAC1_DEFAULT_VALUE			    },
	{ LEVANTE_PLL_FRAC2_IDX,	    LEVANTE_PLL_FRAC2_ADDRESS,		  LEVANTE_PLL_FRAC2_DEFAULT_VALUE			    },
	{ LEVANTE_PLL_FRAC3_IDX,	    LEVANTE_PLL_FRAC3_ADDRESS,		  LEVANTE_PLL_FRAC3_DEFAULT_VALUE			    },

	{ LEVANTE_PCM_INTERFACE1_IDX,	    LEVANTE_PCM_INTERFACE1_ADDRESS,	  LEVANTE_PCM_INTERFACE1_DEFAULT_VALUE			    },
	{ LEVANTE_PCM_INTERFACE2_IDX,	    LEVANTE_PCM_INTERFACE2_ADDRESS,	  LEVANTE_PCM_INTERFACE2_DEFAULT_VALUE			    },
	{ LEVANTE_PCM_INTERFACE3_IDX,	    LEVANTE_PCM_INTERFACE3_ADDRESS,	  LEVANTE_PCM_INTERFACE3_DEFAULT_VALUE			    },
	{ LEVANTE_PCM_RATE_IDX,		    LEVANTE_PCM_RATE_ADDRESS,		  LEVANTE_PCM_RATE_DEFAULT_VALUE			    },
	{ LEVANTE_ECHO_CANCEL_PATH_IDX,	    LEVANTE_ECHO_CANCEL_PATH_ADDRESS,	  LEVANTE_ECHO_CANCEL_PATH_DEFAULT_VALUE		    },
	{ LEVANTE_SIDETONE_GAIN1_IDX,	    LEVANTE_SIDETONE_GAIN1_ADDRESS,	  LEVANTE_SIDETONE_GAIN1_DEFAULT_VALUE			    },
	{ LEVANTE_SIDETONE_GAIN2_IDX,	    LEVANTE_SIDETONE_GAIN2_ADDRESS,	  LEVANTE_SIDETONE_GAIN2_DEFAULT_VALUE			    },
	{ LEVANTE_SIDETONE_GAIN3_IDX,	    LEVANTE_SIDETONE_GAIN3_ADDRESS,	  LEVANTE_SIDETONE_GAIN3_DEFAULT_VALUE			    },

	{ LEVANTE_ADC_OFFSET1_IDX,	    LEVANTE_ADC_OFFSET1_ADDRESS,	  LEVANTE_ADC_OFFSET1_DEFAULT_VALUE			    },
	{ LEVANTE_ADC_OFFSET2_IDX,	    LEVANTE_ADC_OFFSET2_ADDRESS,	  LEVANTE_ADC_OFFSET2_DEFAULT_VALUE			    },
	{ LEVANTE_DMIC_DELAY_IDX,	    LEVANTE_DMIC_DELAY_ADDRESS,		  LEVANTE_DMIC_DELAY_DEFAULT_VALUE			    },

	{ LEVANTE_I2S_INTERFACE1_IDX,	    LEVANTE_I2S_INTERFACE1_ADDRESS,	  LEVANTE_I2S_INTERFACE1_DEFAULT_VALUE			    },
	{ LEVANTE_I2S_INTERFACE2_IDX,	    LEVANTE_I2S_INTERFACE2_ADDRESS,	  LEVANTE_I2S_INTERFACE2_DEFAULT_VALUE			    },
	{ LEVANTE_I2S_INTERFACE3_IDX,	    LEVANTE_I2S_INTERFACE3_ADDRESS,	  LEVANTE_I2S_INTERFACE3_DEFAULT_VALUE			    },
	{ LEVANTE_I2S_INTERFACE4_IDX,	    LEVANTE_I2S_INTERFACE4_ADDRESS,	  LEVANTE_I2S_INTERFACE4_DEFAULT_VALUE			    },

	{ LEVANTE_EQUALIZER_N0_1_IDX,	    LEVANTE_EQUALIZER_N0_1_ADDRESS,	  LEVANTE_EQUALIZER_N0_1_DEFAULT_VALUE			    },
	{ LEVANTE_EQUALIZER_N0_2_IDX,	    LEVANTE_EQUALIZER_N0_2_ADDRESS,	  LEVANTE_EQUALIZER_N0_2_DEFAULT_VALUE			    },
	{ LEVANTE_EQUALIZER_N1_1_IDX,	    LEVANTE_EQUALIZER_N1_1_ADDRESS,	  LEVANTE_EQUALIZER_N1_1_DEFAULT_VALUE			    },
	{ LEVANTE_EQUALIZER_N1_2_IDX,	    LEVANTE_EQUALIZER_N1_2_ADDRESS,	  LEVANTE_EQUALIZER_N1_2_DEFAULT_VALUE			    },
	{ LEVANTE_EQUALIZER_D1_1_IDX,	    LEVANTE_EQUALIZER_D1_1_ADDRESS,	  LEVANTE_EQUALIZER_D1_1_DEFAULT_VALUE			    },
	{ LEVANTE_EQUALIZER_D1_2_IDX,	    LEVANTE_EQUALIZER_D1_2_ADDRESS,	  LEVANTE_EQUALIZER_D1_2_DEFAULT_VALUE			    },

	{ LEVANTE_SIDE_TONE1_IDX,	    LEVANTE_SIDE_TONE1_ADDRESS,		  LEVANTE_SIDE_TONE1_DEFAULT_VALUE			    },
	{ LEVANTE_SIDE_TONE2_IDX,	    LEVANTE_SIDE_TONE2_ADDRESS,		  LEVANTE_SIDE_TONE2_DEFAULT_VALUE			    },
	{ LEVANTE_LEFT_GAIN1_IDX,	    LEVANTE_LEFT_GAIN1_ADDRESS,		  LEVANTE_LEFT_GAIN1_DEFAULT_VALUE			    },
	{ LEVANTE_LEFT_GAIN2_IDX,	    LEVANTE_LEFT_GAIN2_ADDRESS,		  LEVANTE_LEFT_GAIN2_DEFAULT_VALUE			    },
	{ LEVANTE_RIGHT_GAIN1_IDX,	    LEVANTE_RIGHT_GAIN1_ADDRESS,	  LEVANTE_RIGHT_GAIN1_DEFAULT_VALUE			    },
	{ LEVANTE_RIGHT_GAIN2_IDX,	    LEVANTE_RIGHT_GAIN2_ADDRESS,	  LEVANTE_RIGHT_GAIN2_DEFAULT_VALUE			    },
	{ LEVANTE_DAC_OFFSET_IDX,	    LEVANTE_DAC_OFFSET_ADDRESS,		  LEVANTE_DAC_OFFSET_DEFAULT_VALUE			    },
	{ LEVANTE_OFFSET_LEFT1_IDX,	    LEVANTE_OFFSET_LEFT1_ADDRESS,	  LEVANTE_OFFSET_LEFT1_DEFAULT_VALUE			    },
	{ LEVANTE_OFFSET_LEFT2_IDX,	    LEVANTE_OFFSET_LEFT2_ADDRESS,	  LEVANTE_OFFSET_LEFT2_DEFAULT_VALUE			    },
	{ LEVANTE_OFFSET_RIGHT1_IDX,	    LEVANTE_OFFSET_RIGHT1_ADDRESS,	  LEVANTE_OFFSET_RIGHT1_DEFAULT_VALUE			    },
	{ LEVANTE_OFFSET_RIGHT2_IDX,	    LEVANTE_OFFSET_RIGHT2_ADDRESS,	  LEVANTE_OFFSET_RIGHT2_DEFAULT_VALUE			    },
	{ LEVANTE_ADC_ANALOG_PROGRAM1_IDX,  LEVANTE_ADC_ANALOG_PROGRAM1_ADDRESS,  LEVANTE_ADC_ANALOG_PROGRAM1_DEFAULT_VALUE		    },
	{ LEVANTE_ADC_ANALOG_PROGRAM2_IDX,  LEVANTE_ADC_ANALOG_PROGRAM2_ADDRESS,  LEVANTE_ADC_ANALOG_PROGRAM2_DEFAULT_VALUE		    },
	{ LEVANTE_ADC_ANALOG_PROGRAM3_IDX,  LEVANTE_ADC_ANALOG_PROGRAM3_ADDRESS,  LEVANTE_ADC_ANALOG_PROGRAM3_DEFAULT_VALUE		    },
	{ LEVANTE_ADC_ANALOG_PROGRAM4_IDX,  LEVANTE_ADC_ANALOG_PROGRAM4_ADDRESS,  LEVANTE_ADC_ANALOG_PROGRAM4_DEFAULT_VALUE		    },
	{ LEVANTE_ANALOG_TO_ANALOG_IDX,	    LEVANTE_ANALOG_TO_ANALOG_ADDRESS,	  LEVANTE_ANALOG_TO_ANALOG_DEFAULT_VALUE		    },

	{ LEVANTE_HS1_CTRL_IDX,		    LEVANTE_HS1_CTRL_ADDRESS,		  LEVANTE_HS1_CTRL_DEFAULT_VALUE			    },
	{ LEVANTE_HS2_CTRL_IDX,		    LEVANTE_HS2_CTRL_ADDRESS,		  LEVANTE_HS2_CTRL_DEFAULT_VALUE			    },
	{ LEVANTE_LO1_CTRL_IDX,		    LEVANTE_LO1_CTRL_ADDRESS,		  LEVANTE_LO1_CTRL_DEFAULT_VALUE			    },
	{ LEVANTE_LO2_CTRL_IDX,		    LEVANTE_LO2_CTRL_ADDRESS,		  LEVANTE_LO2_CTRL_DEFAULT_VALUE			    },
	{ LEVANTE_EAR_SPKR_CTRL1_IDX,	    LEVANTE_EAR_SPKR_CTRL1_ADDRESS,	  LEVANTE_EAR_SPKR_CTRL1_DEFAULT_VALUE			    },

	{ LEVANTE_EAR_SPKR_CTRL2_IDX,	    LEVANTE_EAR_SPKR_CTRL2_ADDRESS,	  LEVANTE_EAR_SPKR_CTRL2_DEFAULT_VALUE			    },
	{ LEVANTE_AUDIO_SUPPLIES1_IDX,	    LEVANTE_AUDIO_SUPPLIES1_ADDRESS,	  LEVANTE_AUDIO_SUPPLIES1_DEFAULT_VALUE			    },
	{ LEVANTE_AUDIO_SUPPLIES2_IDX,	    LEVANTE_AUDIO_SUPPLIES2_ADDRESS,	  LEVANTE_AUDIO_SUPPLIES2_DEFAULT_VALUE			    },
	{ LEVANTE_ADC_ENABLES1_IDX,	    LEVANTE_ADC_ENABLES1_ADDRESS,	  LEVANTE_ADC_ENABLES1_DEFAULT_VALUE			    },
	{ LEVANTE_ADC_ENABLES2_IDX,	    LEVANTE_ADC_ENABLES2_ADDRESS,	  LEVANTE_ADC_ENABLES2_DEFAULT_VALUE			    },
	{ LEVANTE_DAC_ENABLES1_IDX,	    LEVANTE_DAC_ENABLES1_ADDRESS,	  LEVANTE_DAC_ENABLES1_DEFAULT_VALUE			    },
	{ LEVANTE_DAC_ENABLES2_IDX,	    LEVANTE_DAC_ENABLES2_ADDRESS,	  LEVANTE_DAC_ENABLES2_DEFAULT_VALUE			    },

	{ LEVANTE_AUDIO_CAL1_IDX,	    LEVANTE_AUDIO_CAL1_ADDRESS,		  LEVANTE_AUDIO_CAL1_DEFAULT_VALUE			    },
	{ LEVANTE_AUDIO_CAL2_IDX,	    LEVANTE_AUDIO_CAL2_ADDRESS,		  LEVANTE_AUDIO_CAL2_DEFAULT_VALUE			    },
	{ LEVANTE_AUDIO_CAL3_IDX,	    LEVANTE_AUDIO_CAL3_ADDRESS,		  LEVANTE_AUDIO_CAL3_DEFAULT_VALUE			    },
	{ LEVANTE_AUDIO_CAL4_IDX,	    LEVANTE_AUDIO_CAL4_ADDRESS,		  LEVANTE_AUDIO_CAL4_DEFAULT_VALUE			    },
	{ LEVANTE_AUDIO_CAL5_IDX,	    LEVANTE_AUDIO_CAL5_ADDRESS,		  LEVANTE_AUDIO_CAL5_DEFAULT_VALUE			    },

	{ LEVANTE_ANALOG_INPUT_SEL1_IDX,    LEVANTE_ANALOG_INPUT_SEL1_ADDRESS,	  LEVANTE_ANALOG_INPUT_SEL1_DEFAULT_VALUE		    },
	{ LEVANTE_ANALOG_INPUT_SEL2_IDX,    LEVANTE_ANALOG_INPUT_SEL2_ADDRESS,	  LEVANTE_ANALOG_INPUT_SEL2_DEFAULT_VALUE		    },

	{ LEVANTE_PCM_INTERFACE4_IDX,    LEVANTE_PCM_INTERFACE4_ADDRESS,          LEVANTE_PCM_INTERFACE4_DEFAULT_VALUE		    },
	{ LEVANTE_I2S_INTERFACE5_IDX,    LEVANTE_I2S_INTERFACE5_ADDRESS,	  LEVANTE_I2S_INTERFACE5_DEFAULT_VALUE		    },

	{ LEVANTE_SHORTS_IDX,		    LEVANTE_SHORTS_ADDRESS,		  LEVANTE_SHORTS_DEFAULT_VALUE				    },
	{ LEVANTE_MCLK_IDX,                 LEVANTE_MCLK_ADDRESS,                 LEVANTE_MCLK_DEFAULT_VALUE                		 }

};

//I2S/PCM interface settings
static const RegisterInfo registerInfoGroup_0[] =
{
	/* Group 0 */
	/* index                             address                               default value */
	{ LEVANTE_MCLK_IDX,                 LEVANTE_MCLK_ADDRESS,                 LEVANTE_MCLK_DEFAULT_VALUE                 },
	{ LEVANTE_I2S_INTERFACE5_IDX,       LEVANTE_I2S_INTERFACE5_ADDRESS,       LEVANTE_I2S_INTERFACE5_DEFAULT_VALUE       },
	{ LEVANTE_I2S_INTERFACE1_IDX,       LEVANTE_I2S_INTERFACE1_ADDRESS,       LEVANTE_I2S_INTERFACE1_DEFAULT_VALUE       },
	{ LEVANTE_I2S_INTERFACE2_IDX,       LEVANTE_I2S_INTERFACE2_ADDRESS,       LEVANTE_I2S_INTERFACE2_DEFAULT_VALUE       },
	{ LEVANTE_I2S_INTERFACE3_IDX,       LEVANTE_I2S_INTERFACE3_ADDRESS,       LEVANTE_I2S_INTERFACE3_DEFAULT_VALUE       },
	{ LEVANTE_I2S_INTERFACE4_IDX,       LEVANTE_I2S_INTERFACE4_ADDRESS,       LEVANTE_I2S_INTERFACE4_DEFAULT_VALUE       },
	{ LEVANTE_PCM_INTERFACE4_IDX,       LEVANTE_PCM_INTERFACE4_ADDRESS,       LEVANTE_PCM_INTERFACE4_DEFAULT_VALUE       },
	{ LEVANTE_PCM_INTERFACE1_IDX,       LEVANTE_PCM_INTERFACE1_ADDRESS,       LEVANTE_PCM_INTERFACE1_DEFAULT_VALUE       },
	{ LEVANTE_PCM_INTERFACE2_IDX,       LEVANTE_PCM_INTERFACE2_ADDRESS,       LEVANTE_PCM_INTERFACE2_DEFAULT_VALUE       },
	{ LEVANTE_PCM_INTERFACE3_IDX,       LEVANTE_PCM_INTERFACE3_ADDRESS,       LEVANTE_PCM_INTERFACE3_DEFAULT_VALUE       },
	{ LEVANTE_PCM_RATE_IDX,             LEVANTE_PCM_RATE_ADDRESS,             LEVANTE_PCM_RATE_DEFAULT_VALUE             },

	/* Must be at the end! */
	{ LEVANTE_NO_IDX,                   LEVANTE_NO_ADDRESS,                   LEVANTE_NO_VALUE                           }
};

//DAC/ADC settings
static const RegisterInfo registerInfoGroup_1[] =
{
	/* Group 1 */
	/* index                             address                               default value */
	{ LEVANTE_DAC_OFFSET_IDX,           LEVANTE_DAC_OFFSET_ADDRESS,           LEVANTE_DAC_OFFSET_DEFAULT_VALUE           },
	{ LEVANTE_ADC_ENABLES2_IDX,         LEVANTE_ADC_ENABLES2_ADDRESS,         LEVANTE_ADC_ENABLES2_DEFAULT_VALUE         },
	{ LEVANTE_DAC_ENABLES2_IDX,         LEVANTE_DAC_ENABLES2_ADDRESS,         LEVANTE_DAC_ENABLES2_DEFAULT_VALUE         },

	/* Must be at the end! */
	{ LEVANTE_NO_IDX,                   LEVANTE_NO_ADDRESS,                   LEVANTE_NO_VALUE                           }
};

//MUX selections
static const RegisterInfo registerInfoGroup_2[] =
{
	/* Group 2 */
	/* index                             address                               default value */
	{ LEVANTE_ANALOG_INPUT_SEL2_IDX,    LEVANTE_ANALOG_INPUT_SEL2_ADDRESS,    LEVANTE_ANALOG_INPUT_SEL2_DEFAULT_VALUE    },
	{ LEVANTE_ANALOG_TO_ANALOG_IDX,     LEVANTE_ANALOG_TO_ANALOG_ADDRESS,     LEVANTE_ANALOG_TO_ANALOG_DEFAULT_VALUE     },
	{ LEVANTE_ANALOG_INPUT_SEL1_IDX,    LEVANTE_ANALOG_INPUT_SEL1_ADDRESS,    LEVANTE_ANALOG_INPUT_SEL1_DEFAULT_VALUE    },

	/* Must be at the end! */
	{ LEVANTE_NO_IDX,                   LEVANTE_NO_ADDRESS,                   LEVANTE_NO_VALUE                           }
};

//PA gain settings
static const RegisterInfo registerInfoGroup_3[] =
{
	/* Group 3 */
	/* index                             address                               default value */
	{ LEVANTE_DAC_ENABLES1_IDX,         LEVANTE_DAC_ENABLES1_ADDRESS,         LEVANTE_DAC_ENABLES1_DEFAULT_VALUE         },
	{ LEVANTE_HS1_CTRL_IDX,             LEVANTE_HS1_CTRL_ADDRESS,             LEVANTE_HS1_CTRL_DEFAULT_VALUE             },
	{ LEVANTE_HS2_CTRL_IDX,             LEVANTE_HS2_CTRL_ADDRESS,             LEVANTE_HS2_CTRL_DEFAULT_VALUE             },
	{ LEVANTE_LO1_CTRL_IDX,             LEVANTE_LO1_CTRL_ADDRESS,             LEVANTE_LO1_CTRL_DEFAULT_VALUE             },
	{ LEVANTE_LO2_CTRL_IDX,             LEVANTE_LO2_CTRL_ADDRESS,             LEVANTE_LO2_CTRL_DEFAULT_VALUE             },
	{ LEVANTE_EAR_SPKR_CTRL1_IDX,       LEVANTE_EAR_SPKR_CTRL1_ADDRESS,       LEVANTE_EAR_SPKR_CTRL1_DEFAULT_VALUE       },

	/* Must be at the end! */
	{ LEVANTE_NO_IDX,                   LEVANTE_NO_ADDRESS,                   LEVANTE_NO_VALUE                           }
};

//Digital gain settings
static const RegisterInfo registerInfoGroup_4[] =
{
	/* Group 4 */
	/* index                             address                               default value */
	{ LEVANTE_SIDE_TONE1_IDX,           LEVANTE_SIDE_TONE1_ADDRESS,           LEVANTE_SIDE_TONE1_DEFAULT_VALUE           },
	{ LEVANTE_SIDE_TONE2_IDX,           LEVANTE_SIDE_TONE2_ADDRESS,           LEVANTE_SIDE_TONE2_DEFAULT_VALUE           },
	{ LEVANTE_LEFT_GAIN1_IDX,           LEVANTE_LEFT_GAIN1_ADDRESS,           LEVANTE_LEFT_GAIN1_DEFAULT_VALUE           },
	{ LEVANTE_LEFT_GAIN2_IDX,           LEVANTE_LEFT_GAIN2_ADDRESS,           LEVANTE_LEFT_GAIN2_DEFAULT_VALUE           },
	{ LEVANTE_RIGHT_GAIN1_IDX,          LEVANTE_RIGHT_GAIN1_ADDRESS,          LEVANTE_RIGHT_GAIN1_DEFAULT_VALUE          },
	{ LEVANTE_RIGHT_GAIN2_IDX,          LEVANTE_RIGHT_GAIN2_ADDRESS,          LEVANTE_RIGHT_GAIN2_DEFAULT_VALUE          },

	/* Must be at the end! */
	{ LEVANTE_NO_IDX,                   LEVANTE_NO_ADDRESS,                   LEVANTE_NO_VALUE                           }
};

//Other settings
static const RegisterInfo registerInfoGroup_5[] =
{
	/* Group 5 */
	/* index                             address                               default value */
	{ LEVANTE_MISC2_IDX,                LEVANTE_MISC2_ADDRESS,                LEVANTE_MISC2_DEFAULT_VALUE                },
	{ LEVANTE_PLL_CTRL1_IDX,            LEVANTE_PLL_CTRL1_ADDRESS,            LEVANTE_PLL_CTRL1_DEFAULT_VALUE            },
	{ LEVANTE_PLL_FRAC1_IDX,            LEVANTE_PLL_FRAC1_ADDRESS,            LEVANTE_PLL_FRAC1_DEFAULT_VALUE            },
	{ LEVANTE_PLL_FRAC2_IDX,            LEVANTE_PLL_FRAC2_ADDRESS,            LEVANTE_PLL_FRAC2_DEFAULT_VALUE            },
	{ LEVANTE_PLL_FRAC3_IDX,            LEVANTE_PLL_FRAC3_ADDRESS,            LEVANTE_PLL_FRAC3_DEFAULT_VALUE            },

	{ LEVANTE_ECHO_CANCEL_PATH_IDX,     LEVANTE_ECHO_CANCEL_PATH_ADDRESS,     LEVANTE_ECHO_CANCEL_PATH_DEFAULT_VALUE     },

	{ LEVANTE_ADC_OFFSET1_IDX,          LEVANTE_ADC_OFFSET1_ADDRESS,          LEVANTE_ADC_OFFSET1_DEFAULT_VALUE          },
	{ LEVANTE_ADC_OFFSET2_IDX,          LEVANTE_ADC_OFFSET2_ADDRESS,          LEVANTE_ADC_OFFSET2_DEFAULT_VALUE          },
	{ LEVANTE_DMIC_DELAY_IDX,           LEVANTE_DMIC_DELAY_ADDRESS,           LEVANTE_DMIC_DELAY_DEFAULT_VALUE           },

	{ LEVANTE_OFFSET_LEFT1_IDX,         LEVANTE_OFFSET_LEFT1_ADDRESS,         LEVANTE_OFFSET_LEFT1_DEFAULT_VALUE         },
	{ LEVANTE_OFFSET_LEFT2_IDX,         LEVANTE_OFFSET_LEFT2_ADDRESS,         LEVANTE_OFFSET_LEFT2_DEFAULT_VALUE         },
	{ LEVANTE_OFFSET_RIGHT1_IDX,        LEVANTE_OFFSET_RIGHT1_ADDRESS,        LEVANTE_OFFSET_RIGHT1_DEFAULT_VALUE        },
	{ LEVANTE_OFFSET_RIGHT2_IDX,        LEVANTE_OFFSET_RIGHT2_ADDRESS,        LEVANTE_OFFSET_RIGHT2_DEFAULT_VALUE        },
	{ LEVANTE_ADC_ANALOG_PROGRAM1_IDX,  LEVANTE_ADC_ANALOG_PROGRAM1_ADDRESS,  LEVANTE_ADC_ANALOG_PROGRAM1_DEFAULT_VALUE  },
	{ LEVANTE_ADC_ANALOG_PROGRAM2_IDX,  LEVANTE_ADC_ANALOG_PROGRAM2_ADDRESS,  LEVANTE_ADC_ANALOG_PROGRAM2_DEFAULT_VALUE  },
	{ LEVANTE_ADC_ANALOG_PROGRAM3_IDX,  LEVANTE_ADC_ANALOG_PROGRAM3_ADDRESS,  LEVANTE_ADC_ANALOG_PROGRAM3_DEFAULT_VALUE  },
	{ LEVANTE_ADC_ANALOG_PROGRAM4_IDX,  LEVANTE_ADC_ANALOG_PROGRAM4_ADDRESS,  LEVANTE_ADC_ANALOG_PROGRAM4_DEFAULT_VALUE  },

	{ LEVANTE_AUDIO_SUPPLIES1_IDX,      LEVANTE_AUDIO_SUPPLIES1_ADDRESS,      LEVANTE_AUDIO_SUPPLIES1_DEFAULT_VALUE      },
	{ LEVANTE_AUDIO_SUPPLIES2_IDX,      LEVANTE_AUDIO_SUPPLIES2_ADDRESS,      LEVANTE_AUDIO_SUPPLIES2_DEFAULT_VALUE      },
	{ LEVANTE_ADC_ENABLES1_IDX,         LEVANTE_ADC_ENABLES1_ADDRESS,         LEVANTE_ADC_ENABLES1_DEFAULT_VALUE         },

	{ LEVANTE_AUDIO_CAL1_IDX,           LEVANTE_AUDIO_CAL1_ADDRESS,           LEVANTE_AUDIO_CAL1_DEFAULT_VALUE           },
	{ LEVANTE_AUDIO_CAL2_IDX,           LEVANTE_AUDIO_CAL2_ADDRESS,           LEVANTE_AUDIO_CAL2_DEFAULT_VALUE           },
	{ LEVANTE_AUDIO_CAL3_IDX,           LEVANTE_AUDIO_CAL3_ADDRESS,           LEVANTE_AUDIO_CAL3_DEFAULT_VALUE           },
	{ LEVANTE_AUDIO_CAL4_IDX,           LEVANTE_AUDIO_CAL4_ADDRESS,           LEVANTE_AUDIO_CAL4_DEFAULT_VALUE           },
	{ LEVANTE_AUDIO_CAL5_IDX,           LEVANTE_AUDIO_CAL5_ADDRESS,           LEVANTE_AUDIO_CAL5_DEFAULT_VALUE           },

	{ LEVANTE_SHORTS_IDX,               LEVANTE_SHORTS_ADDRESS,               LEVANTE_SHORTS_DEFAULT_VALUE               },

	/* Must be at the end! */
	{ LEVANTE_NO_IDX,                   LEVANTE_NO_ADDRESS,                   LEVANTE_NO_VALUE                           }
};

//Other settings
static const RegisterInfo registerInfoGroup_6[] =
{
	/* Group 6 */
	/* index                             address                               default value */
    { LEVANTE_EAR_SPKR_CTRL2_IDX,       LEVANTE_EAR_SPKR_CTRL2_ADDRESS,       LEVANTE_EAR_SPKR_CTRL2_DEFAULT_VALUE       },

	{ LEVANTE_SIDETONE_GAIN1_IDX,       LEVANTE_SIDETONE_GAIN1_ADDRESS,       LEVANTE_SIDETONE_GAIN1_DEFAULT_VALUE       },
	{ LEVANTE_SIDETONE_GAIN2_IDX,       LEVANTE_SIDETONE_GAIN2_ADDRESS,       LEVANTE_SIDETONE_GAIN2_DEFAULT_VALUE       },
	{ LEVANTE_SIDETONE_GAIN3_IDX,       LEVANTE_SIDETONE_GAIN3_ADDRESS,       LEVANTE_SIDETONE_GAIN3_DEFAULT_VALUE       },

	{ LEVANTE_EQUALIZER_N0_1_IDX,       LEVANTE_EQUALIZER_N0_1_ADDRESS,       LEVANTE_EQUALIZER_N0_1_DEFAULT_VALUE       },
	{ LEVANTE_EQUALIZER_N0_2_IDX,       LEVANTE_EQUALIZER_N0_2_ADDRESS,       LEVANTE_EQUALIZER_N0_2_DEFAULT_VALUE       },
	{ LEVANTE_EQUALIZER_N1_1_IDX,       LEVANTE_EQUALIZER_N1_1_ADDRESS,       LEVANTE_EQUALIZER_N1_1_DEFAULT_VALUE       },
	{ LEVANTE_EQUALIZER_N1_2_IDX,       LEVANTE_EQUALIZER_N1_2_ADDRESS,       LEVANTE_EQUALIZER_N1_2_DEFAULT_VALUE       },
	{ LEVANTE_EQUALIZER_D1_1_IDX,       LEVANTE_EQUALIZER_D1_1_ADDRESS,       LEVANTE_EQUALIZER_D1_1_DEFAULT_VALUE       },
	{ LEVANTE_EQUALIZER_D1_2_IDX,       LEVANTE_EQUALIZER_D1_2_ADDRESS,       LEVANTE_EQUALIZER_D1_2_DEFAULT_VALUE       },

	/* Must be at the end! */
	{ LEVANTE_NO_IDX,                   LEVANTE_NO_ADDRESS,                   LEVANTE_NO_VALUE                           }
};

static RegisterInfoGroupData registerInfoGroupData[] =
{
	{ registerInfoGroup_0, FALSE },
	{ registerInfoGroup_1, TRUE  },
	{ registerInfoGroup_2, FALSE },
	{ registerInfoGroup_3, FALSE },
	{ registerInfoGroup_4, TRUE  },
	{ registerInfoGroup_5, FALSE },
	{ registerInfoGroup_6, TRUE  },

	/* Must be at the end! */
	{ NULL               , FALSE }
};

static int groupOrder[][7] =
{
	{ 0, 1, 2, 3, 4, 5, 6 },  /* for DAC Enable  */
	{ 4, 3, 2, 1, 0, 5, 6 },  /* for DAC Disable */
	/*For modify, move 1 after 5 since when mute during voice call, we must write reg 0xde after writing reg 0xdd, otherwise far end has a big noise*/
	{ 0, 2, 3, 4, 5, 1, 6 }   /* for Modify (DAC Stays the same) */
};

static const LEVANTE_FileGain2StructGainConverter gainConverter[LEVANTE_TOTAL_GAINS] =
{
	{ gLevanteMsaDigitalGain,	   LEVANTE_MSA_DIGITAL_GAIN_MAX				  },    /* Gain ID 0 - digital gain */
	{ gLevantePgaEarpGain,		   LEVANTE_PGA_EARP_GAIN_MAX				  },    /* Gain ID 1  */
	{ gLevantePgaSpkrGain,		   LEVANTE_PGA_SPKR_GAIN_MAX				  },    /* Gain ID 2  */
	{ gLevantePgaHs1Gain,		   LEVANTE_PGA_HS1_GAIN_MAX				  },    /* Gain ID 3  */
	{ gLevantePgaHs2Gain,		   LEVANTE_PGA_HS2_GAIN_MAX				  },    /* Gain ID 4  */
	{ gLevantePgaLout1Gain,		   LEVANTE_PGA_LOUT1_GAIN_MAX				  },    /* Gain ID 5  */
	{ gLevantePgaLout2Gain,		   LEVANTE_PGA_LOUT2_GAIN_MAX				  },    /* Gain ID 6  */
	{ gLevanteDpga1LofiGainLeftGain,   LEVANTE_DPGA1_LOFI_GAIN_LEFT_GAIN_MAX		  },    /* Gain ID 7  */
	{ gLevanteDpga1HifilGainLeftGain,  LEVANTE_DPGA1_HIFIL_GAIN_LEFT_GAIN_MAX		  },    /* Gain ID 8  */
	{ gLevanteDpga1HifirGainLeftGain,  LEVANTE_DPGA1_HIFIR_GAIN_LEFT_GAIN_MAX		  },    /* Gain ID 9  */
	{ gLevanteDpga2LofiGainRightGain,  LEVANTE_DPGA2_LOFI_GAIN_RIGHT_GAIN_MAX		  },    /* Gain ID 10 */
	{ gLevanteDpga2HifilGainRightGain, LEVANTE_DPGA2_HIFIL_GAIN_RIGHT_GAIN_MAX		  },    /* Gain ID 11 */
	{ gLevanteDpga2HifirGainRightGain, LEVANTE_DPGA2_HIFIR_GAIN_RIGHT_GAIN_MAX		  },    /* Gain ID 12 */
	{ gLevantePgaMic1Gain,		   LEVANTE_PGA_MIC1_GAIN_MAX				  },    /* Gain ID 13 */
	{ gLevantePgaMic3Gain,		   LEVANTE_PGA_MIC3_GAIN_MAX				  },    /* Gain ID 14 */
	{ gLevantePgaAux1Gain,		   LEVANTE_PGA_AUX1_GAIN_MAX				  },    /* Gain ID 15 */
	{ gLevantePgaAux2Gain,		   LEVANTE_PGA_AUX2_GAIN_MAX				  },    /* Gain ID 16 */
	{ gLevanteSidetoneRGain,	   LEVANTE_SIDETONE_GAIN_R_MAX				  },    /* Gain ID 17 */
	{ gLevanteSidetoneLGain,	   LEVANTE_SIDETONE_GAIN_L_MAX				  },    /* Gain ID 18 */
	{ gLevanteMod1Gain,		   LEVANTE_MOD1_GAIN_MAX				  },    /* Gain ID 19 */
	{ gLevanteMod2Gain,		   LEVANTE_MOD2_GAIN_MAX				  } /* Gain ID 20 */
};

/* Path to Input/Output */
static const LevantePathDescriptionS gLevantePathDescription[LEVANTE_TOTAL_VALID_PATHS] =
{
	/* Input             Output                              PCM 1 output                         */
	{ AnaMIC1_IN,	    PCM_L_OUT					   },   /* LEVANTE_AnaMIC1_TO_PCM_L              */
	{ AnaMIC1_IN,	    PCM_L_OUT					   },   /* LEVANTE_AnaMIC1_LOUD_TO_PCM_L         */
	{ AnaMIC2_IN,	    PCM_L_OUT					   },   /* LEVANTE_AnaMIC2_TO_PCM_L              */
	{ DigMIC1_IN,	    PCM_L_OUT					   },   /* LEVANTE_DigMIC1_TO_PCM_L              */
	{ DigMIC1_IN,	    PCM_L_OUT					   },   /* LEVANTE_DigMIC1_LOUD_TO_PCM_L         */
	{ DigMIC2_IN,	    PCM_R_OUT					   },   /* LEVANTE_DigMIC2_TO_PCM_R              */
	{ AnaMIC3_IN,	    PCM_R_OUT					   },   /* LEVANTE_AnaMIC3_TO_PCM_R              */
	{ AUX1_IN,	    PCM_L_OUT					   },   /* LEVANTE_AUX1_TO_PCM_L                 */
	{ AUX2_IN,	    PCM_R_OUT					   },   /* LEVANTE_AUX2_TO_PCM_R                 */

	/* Input             Output                              PCM 2 output                         */
	{ AnaMIC1_IN,	    PCM_R_OUT					   },   /* LEVANTE_AnaMIC1_TO_PCM_R              */
	{ AnaMIC1_IN,	    PCM_R_OUT					   },   /* LEVANTE_AnaMIC1_LOUD_TO_PCM_R         */
	{ AnaMIC2_IN,	    PCM_R_OUT					   },   /* LEVANTE_AnaMIC2_TO_PCM_R              */
	{ DigMIC1_IN,	    PCM_R_OUT					   },   /* LEVANTE_DigMIC1_TO_PCM_R              */
	{ DigMIC1_IN,	    PCM_R_OUT					   },   /* LEVANTE_DigMIC1_LOUD_TO_PCM_R         */
	{ DigMIC2_IN,	    PCM_L_OUT					   },   /* LEVANTE_DigMIC2_TO_PCM_L              */
	{ AnaMIC3_IN,	    PCM_L_OUT					   },   /* LEVANTE_AnaMIC3_TO_PCM_L              */
	{ AUX1_IN,	    PCM_R_OUT					   },   /* LEVANTE_AUX1_TO_PCM_R                 */
	{ AUX2_IN,	    PCM_L_OUT					   },   /* LEVANTE_AUX2_TO_PCM_L                 */

	/* Input             Output                              I2S 1 output                         */
	{ AnaMIC1_IN,	    I2S_VIA_PCM_L_OUT				   },   /* LEVANTE_AnaMIC1_TO_I2S_L              */
	{ AnaMIC1_IN,	    I2S_VIA_PCM_L_OUT				   },   /* LEVANTE_AnaMIC1_LOUD_TO_I2S_L         */
	{ AnaMIC2_IN,	    I2S_VIA_PCM_L_OUT				   },   /* LEVANTE_AnaMIC2_TO_I2S_L              */
	{ DigMIC1_IN,	    I2S_VIA_PCM_L_OUT				   },   /* LEVANTE_DigMIC1_TO_I2S_L              */
	{ DigMIC1_IN,	    I2S_VIA_PCM_L_OUT				   },   /* LEVANTE_DigMIC1_LOUD_TO_I2S_L         */
	{ DigMIC2_IN,	    I2S_VIA_PCM_R_OUT				   },   /* LEVANTE_DigMIC2_TO_I2S_R              */
	{ AnaMIC3_IN,	    I2S_VIA_PCM_R_OUT				   },   /* LEVANTE_AnaMIC3_TO_I2S_R              */
	{ AUX1_IN,	    I2S_VIA_PCM_L_OUT				   },   /* LEVANTE_AUX1_TO_I2S_L                 */
	{ AUX2_IN,	    I2S_VIA_PCM_R_OUT				   },   /* LEVANTE_AUX2_TO_I2S_R                 */

	/* Input             Output                              I2S 2 output                         */
	{ AnaMIC1_IN,	    I2S_VIA_PCM_R_OUT				   },   /* LEVANTE_AnaMIC1_TO_I2S_R              */
	{ AnaMIC1_IN,	    I2S_VIA_PCM_R_OUT				   },   /* LEVANTE_AnaMIC1_LOUD_TO_I2S_R         */
	{ AnaMIC2_IN,	    I2S_VIA_PCM_R_OUT				   },   /* LEVANTE_AnaMIC2_TO_I2S_R              */
	{ DigMIC1_IN,	    I2S_VIA_PCM_R_OUT				   },   /* LEVANTE_DigMIC1_TO_I2S_R              */
	{ DigMIC1_IN,	    I2S_VIA_PCM_R_OUT				   },   /* LEVANTE_DigMIC1_LOUD_TO_I2S_R         */
	{ DigMIC2_IN,	    I2S_VIA_PCM_L_OUT				   },   /* LEVANTE_DigMIC2_TO_I2S_L              */
	{ AnaMIC3_IN,	    I2S_VIA_PCM_L_OUT				   },   /* LEVANTE_AnaMIC3_TO_I2S_L              */
	{ AUX1_IN,	    I2S_VIA_PCM_R_OUT				   },   /* LEVANTE_AUX1_TO_I2S_R                 */
	{ AUX2_IN,	    I2S_VIA_PCM_L_OUT				   },   /* LEVANTE_AUX2_TO_I2S_L                 */

	/* Input             Output                              From PCM Left                        */
	{ PCM_L_IN,	    EAR_VIA_DAC1_OUT				   },   /* LEVANTE_PCM_L_TO_EAR                  */
	{ PCM_L_IN,	    SPKR_VIA_DAC1_OUT				   },   /* LEVANTE_PCM_L_TO_SPKR                 */
	{ PCM_L_IN,	    HS1_VIA_DAC1_OUT				   },   /* LEVANTE_PCM_L_TO_HS1                  */
	{ PCM_L_IN,	    HS2_VIA_DAC1_OUT				   },   /* LEVANTE_PCM_L_TO_HS2                  */
	{ PCM_L_IN,	    HS_STEREO_OUT				   },   /* LEVANTE_PCM_L_TO_HS                   */
	{ PCM_L_IN,	    LINEOUT1_VIA_DAC1_OUT			   },   /* LEVANTE_PCM_L_TO_LINEOUT1             */
	{ PCM_L_IN,	    LINEOUT2_VIA_DAC1_OUT			   },   /* LEVANTE_PCM_L_TO_LINEOUT2             */
	{ PCM_L_IN,	    LINEOUT_STEREO_OUT				   },   /* LEVANTE_PCM_L_TO_LINEOUT              */

	/* Input             Output                              From PCM Right                       */
	{ PCM_R_IN,	    EAR_VIA_DAC1_OUT				   },   /* LEVANTE_PCM_R_TO_EAR                  */
	{ PCM_R_IN,	    SPKR_VIA_DAC1_OUT				   },   /* LEVANTE_PCM_R_TO_SPKR                 */
	{ PCM_R_IN,	    HS1_VIA_DAC1_OUT				   },   /* LEVANTE_PCM_R_TO_HS1                  */
	{ PCM_R_IN,	    HS2_VIA_DAC1_OUT				   },   /* LEVANTE_PCM_R_TO_HS2                  */
	{ PCM_R_IN,	    HS_STEREO_OUT				   },   /* LEVANTE_PCM_R_TO_HS                   */
	{ PCM_R_IN,	    LINEOUT1_VIA_DAC1_OUT			   },   /* LEVANTE_PCM_R_TO_LINEOUT1             */
	{ PCM_R_IN,	    LINEOUT2_VIA_DAC1_OUT			   },   /* LEVANTE_PCM_R_TO_LINEOUT2             */
	{ PCM_R_IN,	    LINEOUT_STEREO_OUT				   },   /* LEVANTE_PCM_R_TO_LINEOUT              */

	/* Input             Output                              From I2S                             */
	{ I2S_IN,	    EAR_VIA_DAC1_OUT				   },   /* LEVANTE_I2S_TO_EAR                    */
	{ I2S_IN,	    SPKR_VIA_DAC1_OUT				   },   /* LEVANTE_I2S_TO_SPKR                   */
	{ I2S_IN,	    HS1_VIA_DAC1_OUT				   },   /* LEVANTE_I2S_TO_HS1                    */
	{ I2S_IN,	    HS2_VIA_DAC1_OUT				   },   /* LEVANTE_I2S_TO_HS2                    */
	{ I2S_IN,	    HS_STEREO_FLIP_OUT				   },   /* LEVANTE_I2S_TO_HS                     */
	{ I2S_IN,	    LINEOUT1_VIA_DAC1_OUT			   },   /* LEVANTE_I2S_TO_LINEOUT1               */
	{ I2S_IN,	    LINEOUT2_VIA_DAC1_OUT			   },   /* LEVANTE_I2S_TO_LINEOUT2               */
	{ I2S_IN,	    LINEOUT_STEREO_OUT				   },   /* LEVANTE_I2S_TO_LINEOUT                */

	/* Input             Output                              From I2S1                            */
	{ I2S1_IN,	    EAR_VIA_DAC1_OUT				   },   /* LEVANTE_I2S1_TO_EAR                   */
	{ I2S1_IN,	    SPKR_VIA_DAC1_OUT				   },   /* LEVANTE_I2S1_TO_SPKR                  */
	{ I2S1_IN,	    HS1_VIA_DAC1_OUT				   },   /* LEVANTE_I2S1_TO_HS1                   */
	{ I2S1_IN,	    HS2_VIA_DAC1_OUT				   },   /* LEVANTE_I2S1_TO_HS2                   */
	{ I2S1_IN,	    HS_STEREO_FLIP_OUT				   },   /* LEVANTE_I2S1_TO_HS                    */
	{ I2S1_IN,	    LINEOUT1_VIA_DAC1_OUT			   },   /* LEVANTE_I2S1_TO_LINEOUT1              */
	{ I2S1_IN,	    LINEOUT2_VIA_DAC1_OUT			   },   /* LEVANTE_I2S1_TO_LINEOUT2              */
	{ I2S1_IN,	    LINEOUT_STEREO_OUT				   },   /* LEVANTE_I2S1_TO_LINEOUT               */

	/* Input             Output                              Side Tone                            */
	{ SideTone_L_IN,    SideTone_OUT				   },   /* LEVANTE_IN_L_TO_OUT                   */
	{ SideTone_R_IN,    SideTone_OUT				   },   /* LEVANTE_IN_R_TO_OUT                   */
	{ SideTone_BOTH_IN, SideTone_OUT				   },   /* LEVANTE_IN_BOTH_TO_OUT                */

	/* Input             Output                              Digital Loops                        */
	{ PCM_LOOP_INT_IN,  PCM_LOOP_INT_OUT				   },   /* LEVANTE_PCM_INT_LOOP                  */
	{ PCM_LOOP_EXT_IN,  PCM_LOOP_EXT_OUT				   },   /* LEVANTE_PCM_EXT_LOOP                  */
	{ I2S_LOOP_INT_IN,  I2S_LOOP_INT_OUT				   },   /* LEVANTE_I2S_INT_LOOP                  */
	{ I2S_LOOP_EXT_IN,  I2S_LOOP_EXT_OUT				   },   /* LEVANTE_I2S_EXT_LOOP                  */
	{ EC_DAC1_IN,	    EC_OUT					   },   /* LEVANTE_DAC1_IN_EC_LOOP               */
	{ EC_DAC2_IN,	    EC_OUT					   },   /* LEVANTE_DAC2_IN_EC_LOOP               */
	{ EC_DAC1_DAC2_IN,  EC_OUT					   },   /* LEVANTE_DAC1_DAC2_IN_EC_LOOP          */
	{ PCM_L_IN,	    I2S_L_Via_Ext_PA_LOOP_OUT			   },   /* LEVANTE_PCM_L_IN_TO_I2S_L_OUT_LOOP    */
	{ PCM_L_IN,	    I2S_R_Via_Ext_PA_LOOP_OUT			   },   /* LEVANTE_PCM_L_IN_TO_I2S_R_OUT_LOOP    */
	{ PCM_L_IN,	    I2S_BOTH_Via_Ext_PA_LOOP_OUT		   },   /* LEVANTE_PCM_L_IN_TO_I2S_Both_OUT_LOOP */
	{ PCM_R_IN,	    I2S_L_Via_Ext_PA_LOOP_OUT			   },   /* LEVANTE_PCM_R_IN_TO_I2S_L_OUT_LOOP    */
	{ PCM_R_IN,	    I2S_R_Via_Ext_PA_LOOP_OUT			   },   /* LEVANTE_PCM_R_IN_TO_I2S_R_OUT_LOOP    */
	{ PCM_R_IN,	    I2S_BOTH_Via_Ext_PA_LOOP_OUT		   },   /* LEVANTE_PCM_R_IN_TO_I2S_Both_OUT_LOOP */
	{ I2S_IN,	    I2S_L_Via_Ext_PA_LOOP_OUT			   },   /* LEVANTE_I2S_IN_TO_I2S_L_OUT_LOOP      */
	{ I2S_IN,	    I2S_R_Via_Ext_PA_LOOP_OUT			   },   /* LEVANTE_I2S_IN_TO_I2S_R_OUT_LOOP      */
	{ I2S_IN,	    I2S_BOTH_Via_Ext_PA_LOOP_OUT		   },   /* LEVANTE_I2S_IN_TO_I2S_Both_OUT_LOOP   */
	{ I2S1_IN,	    I2S_L_Via_Ext_PA_LOOP_OUT			   },   /* LEVANTE_I2S1_IN_TO_I2S_L_OUT_LOOP     */
	{ I2S1_IN,	    I2S_R_Via_Ext_PA_LOOP_OUT			   },   /* LEVANTE_I2S1_IN_TO_I2S_R_OUT_LOOP     */
	{ I2S1_IN,	    I2S_BOTH_Via_Ext_PA_LOOP_OUT		   },   /* LEVANTE_I2S1_IN_TO_I2S_Both_OUT_LOOP  */

	/* Input             Output                              Analog Loops                         */
	{ AUX1_ANA_LOOP_IN, HS2_ANA_LOOP_OUT				   },   /* LEVANTE_AUX1_TO_HS2                   */
	{ AUX1_ANA_LOOP_IN, LINEOUT2_ANA_LOOP_OUT			   },   /* LEVANTE_AUX1_TO_LOUT2                 */
	{ AUX2_ANA_LOOP_IN, HS1_ANA_LOOP_OUT				   },   /* LEVANTE_AUX2_TO_HS1                   */
	{ AUX2_ANA_LOOP_IN, LINEOUT1_ANA_LOOP_OUT			   },   /* LEVANTE_AUX2_TO_LOUT1                 */

	//Jackie,2011-0222
	//Loop include codec and MSA
	/* Input	     Output				 From PCM Left			      */
	{ PCM_L_IN,		EAR_VIA_DAC1_OUT	     },  /* LEVANTE_PCM_L_TO_EAR_LOOP		       */
	{ PCM_L_IN,		HS_STEREO_OUT		     },  /* LEVANTE_PCM_L_TO_HS_LOOP		       */
	{ PCM_L_IN,		SPKR_VIA_DAC1_OUT	     },  /* LEVANTE_PCM_L_TO_SPKR_LOOP		       */
	{ AnaMIC1_IN,	    PCM_L_OUT			 },  /* LEVANTE_AnaMIC1_TO_PCM_L_LOOP		   */
	{ AnaMIC2_IN,	    PCM_L_OUT			 },  /* LEVANTE_AnaMIC2_TO_PCM_L_LOOP		   */
	{ AnaMIC1_IN,	    PCM_L_OUT			 }, /* LEVANTE_AnaMIC1_LOUD_TO_PCM_L_LOOP	 */

	/*aux to speaker by using sidetone path*/
	{ AUX1_IN,	    SIDETONE_L_OUT					   },   /* LEVANTE_AUX1_TO_SIDETONE_L                 */
	{ AUX2_IN,	    SIDETONE_R_OUT					   },   /* LEVANTE_AUX2_TO_SIDETONE_R                 */
	{ SideTone_BOTH_IN, SideTone_OUT					   },   /* LEVANTE_SIDETONE_IN_TO_OUT                */
	{ FM_VIA_SIDETONE_IN,  SPKR_VIA_DAC1_OUT				   },   /* LEVANTE_SIDETONE_TO_SPKR                 */

	{ I2S_IN,           HS_VIA_DAC1_OUT_SPKR_VIA_DAC2_OUT },  /* LEVANTE_I2S_TO_SPKR_HS            */
	/*add path for recording by GSSP*/
	{ AnaMIC1_IN,	    PCM_I2S_L_OUT					   },   /* LEVANTE_AnaMIC1_TO_PCM_I2S_L              */
	{ AnaMIC1_IN,	    PCM_I2S_R_OUT					   },   /* LEVANTE_AnaMIC1_TO_PCM_I2S_R              */
	{ AnaMIC1_IN,	    PCM_I2S_L_OUT					   },   /* LEVANTE_AnaMIC1_LOUD_TO_PCM_I2S_L         */
	{ AnaMIC1_IN,	    PCM_I2S_R_OUT					   },   /* LEVANTE_AnaMIC1_LOUD_TO_PCM_I2S_R         */
	{ AnaMIC2_IN,	    PCM_I2S_L_OUT					   },   /* LEVANTE_AnaMIC2_TO_PCM_I2S_L              */
	{ AnaMIC2_IN,	    PCM_I2S_R_OUT					   },   /* LEVANTE_AnaMIC2_TO_PCM_I2S_R              */
	{ AUX1_IN,	    PCM_I2S_L_OUT					   },   /* LEVANTE_AUX1_TO_PCM_I2S_L                 */
	{ AUX2_IN,	    PCM_I2S_R_OUT					   }   /* LEVANTE_AUX2_TO_PCM_R                 */
};


/*----------- Local functions ------------------------------------------------*/


/************************************************************************
 * Function: LevanteRegSet
 *************************************************************************
 * Description:  Set the values of the registers.
 *
 * Parameters:   unsigned char regAddress
 *               unsigned char regValue
 *
 * Return value: ACM_DigitalGain
 *
 * Notes:
 *************************************************************************/
static void LevanteRegSet(unsigned char regAddress, unsigned char regValue)
{
	//DIAG_FILTER(AUDIO_LEVANTE, LEVANTE, AUDIO_LEVANTE_RegSet, DIAG_INFORMATION)
	//diagPrintf("AUDIO_LEVANTE_RegSet; address= 0x%x, val= 0x%x", regAddress, regValue);

	LevanteI2CWrite(regAddress, regValue);
} /* End of LevanteRegSet */



/************************************************************************
 * Function: LevanteHardResetRegister
 *************************************************************************
 * Description: Hard reset the values of the registers to the default value. If we write 0x42 to reset
 * the register, we should call this function to make asoc to be sync
 * Parameters:
 *
 * Return value:
 *
 * Notes:
 *************************************************************************/
static void LevanteHardResetRegister(void)
{
	int i;
	for (i = 0; i < NUM_OF_REGS; i++)
	{
		LevanteRegSet(registerInfo[i].address, sanremo_audio_regs[i]);
	}
}



/************************************************************************
 * Function: LevanteStartUpConfigSet
 *************************************************************************
 * Description: Set the values of the registers as for the start up
 *              configuration.
 * Parameters:
 *
 * Return value:
 *
 * Notes:
 *************************************************************************/
static void LevanteStartUpConfigSet(void)
{
	int i;
	//DIAG_FILTER(AUDIO_LEVANTE, LEVANTE, AUDIO_LEVANTE_StartUpConfigSet, DIAG_INFORMATION)
	//diagTextPrintf("Levante StartUp Config Set");

	/*added by Raul, after LevanteShutDownConfigSet, reg become defaul value*/
	for (i = 0; i < NUM_OF_REGS; i++)
	{
		shadowReg[i] = registerInfo[i].defaultValue;
	}

	/* Shadow should NOT be updated! */
	/* Turn On regulator for audio, Release Audio Clocks, Turn On audio PLL */

	/* Make ASOC to be sync with the register values */
	LevanteHardResetRegister();
	LevanteRegSet(LEVANTE_MISC2_ADDRESS, LEVANTE_MISC2_DEFAULT_VALUE & ~(1 << AUDIO_RSTB_SHIFT));
	/* The audio section only can be reset 160 usec after AUDIO_PU set to '1'. */
	usleep(200);
	LevanteRegSet(LEVANTE_MISC2_ADDRESS, LEVANTE_MISC2_DEFAULT_VALUE);
	/* The audio I2C registers (register addresses 0x0 to 0xEE) are accessible for read or write activity 160 usec after AUDIO_PU set to '1'. */
	usleep(200);

	/* Turn On regulator 2.7 V, Turn On charge pump, Audio is Active */
	LevanteRegSet(LEVANTE_AUDIO_SUPPLIES2_ADDRESS, LEVANTE_AUDIO_SUPPLIES2_DEFAULT_VALUE & (~(1 << CPUMP_EN_SHIFT)));
	LevanteRegSet(LEVANTE_AUDIO_SUPPLIES2_ADDRESS, LEVANTE_AUDIO_SUPPLIES2_DEFAULT_VALUE);

	LevanteRegSet(LEVANTE_PLL_CTRL1_ADDRESS, LEVANTE_PLL_CTRL1_DEFAULT_VALUE);


	if(_pmicId <= 0x41)
	{  /* LEVANTE_ID_A0 or LEVANTE_ID_A1 */
		LevanteRegSet(LEVANTE_SIDE_TONE1_ADDRESS, LEVANTE_SIDE_TONE1_DEFAULT_VALUE);
		LevanteRegSet(LEVANTE_SIDE_TONE2_ADDRESS, LEVANTE_SIDE_TONE2_DEFAULT_VALUE);
		LevanteRegSet(LEVANTE_LEFT_GAIN1_ADDRESS, LEVANTE_LEFT_GAIN1_DEFAULT_VALUE);
		LevanteRegSet(LEVANTE_LEFT_GAIN2_ADDRESS, LEVANTE_LEFT_GAIN2_DEFAULT_VALUE);
		LevanteRegSet(LEVANTE_RIGHT_GAIN1_ADDRESS, LEVANTE_RIGHT_GAIN1_DEFAULT_VALUE);
		LevanteRegSet(LEVANTE_RIGHT_GAIN2_ADDRESS, LEVANTE_RIGHT_GAIN2_DEFAULT_VALUE);

		LevanteRegSet(LEVANTE_PLL_FRAC1_ADDRESS, LEVANTE_PLL_FRAC1_DEFAULT_VALUE);
		LevanteRegSet(LEVANTE_PLL_FRAC2_ADDRESS, LEVANTE_PLL_FRAC2_DEFAULT_VALUE);
		LevanteRegSet(LEVANTE_PLL_FRAC3_ADDRESS, LEVANTE_PLL_FRAC3_DEFAULT_VALUE);
	}

	LevanteRegSet(LEVANTE_I2S_INTERFACE4_ADDRESS, LEVANTE_I2S_INTERFACE4_DEFAULT_VALUE); //

	//LevanteRegSet(LEVANTE_AUDIO_SUPPLIES1_ADDRESS, LEVANTE_AUDIO_SUPPLIES1_DEFAULT_VALUE);

	/*FIXME: Raul: workaround for Levante A1, need to set sense switch on for Mic2 to enable headset (Mic2 workaround) detection*/
	if (_pmicId <= 0x41)
	{
		static int setOneTime = 0;
		LevanteRegSet(registerInfo[LEVANTE_ADC_ANALOG_PROGRAM1_IDX].address, registerInfo[LEVANTE_ADC_ANALOG_PROGRAM1_IDX].defaultValue);
		if (setOneTime == 0)
			LevanteRegSet(registerInfo[LEVANTE_PCM_INTERFACE4_IDX].address, registerInfo[LEVANTE_PCM_INTERFACE4_IDX].defaultValue);
		setOneTime = 1;
	}
	else
	{
		/*Fixme: make sure [0xd0]=0x60 at the beginning, or the hook detection fails after booting the board with headset plugin*/
		LevanteRegSet(registerInfo[LEVANTE_ADC_ANALOG_PROGRAM1_IDX].address, registerInfo[LEVANTE_ADC_ANALOG_PROGRAM1_IDX].defaultValue);
		LevanteRegSet(registerInfo[LEVANTE_PCM_INTERFACE4_IDX].address, 0);
		registerInfo[LEVANTE_PCM_INTERFACE4_IDX].defaultValue = 0;
		shadowReg[LEVANTE_PCM_INTERFACE4_IDX] = 0;
	}

	LevanteRegSet(LEVANTE_EAR_SPKR_CTRL2_ADDRESS, LEVANTE_EAR_SPKR_CTRL2_DEFAULT_VALUE);

	LevanteRegSet(LEVANTE_I2S_INTERFACE2_ADDRESS, LEVANTE_I2S_INTERFACE2_DEFAULT_VALUE);

	/*FIXME Raul: since kernel has set this register to 0xc7 to get the maximum recording gain, and we found that sometimes it is still 0xc7 after
	   LevanteShutDownConfigSet was called, so we set it here again, otherwise during voice call the far end will have large noise*/
	LevanteRegSet(LEVANTE_ADC_ANALOG_PROGRAM2_ADDRESS, LEVANTE_ADC_ANALOG_PROGRAM2_DEFAULT_VALUE);

	LevanteRegSet(LEVANTE_ADC_ANALOG_PROGRAM4_ADDRESS, LEVANTE_ADC_ANALOG_PROGRAM4_DEFAULT_VALUE);

	LevanteRegSet(LEVANTE_ANALOG_TO_ANALOG_ADDRESS, LEVANTE_ANALOG_TO_ANALOG_DEFAULT_VALUE);

	/*Raul: ASOC will remember the value, so first clear and then set to ensure write the register successfully*/
	LevanteRegSet(LEVANTE_EAR_SPKR_CTRL2_ADDRESS, (shadowReg[LEVANTE_EAR_SPKR_CTRL2_IDX] & (~(1 << APPLY_CHANGES_SHIFT))));
	LevanteRegSet(LEVANTE_EAR_SPKR_CTRL2_ADDRESS, (shadowReg[LEVANTE_EAR_SPKR_CTRL2_IDX] | (1 << APPLY_CHANGES_SHIFT)));

} /* End of LevanteStartUpConfigSet */


/************************************************************************
 * Function: LevanteShutDownConfigSet
 *************************************************************************
 * Description: Set the values of the registers as for the shut down
 *              configuration.
 * Parameters:
 *
 * Return value:
 *
 * Notes:
 *************************************************************************/
static void LevanteShutDownConfigSet(void)
{
	//DIAG_FILTER(AUDIO_LEVANTE, LEVANTE, AUDIO_LEVANTE_ShutDownConfigSet, DIAG_INFORMATION)
	//diagTextPrintf("Levante ShutDown Config Set");

	/* Handle gain for Force Speaker (headset + loud-speaker); set back to default values */
	regField[DPGA1_HIFIL_HIFIR_GAIN_LEFT]  = DPGA1_HIFIL_HIFIR_GAIN_LEFT_DEFAULT_VALUE;
	regField[DPGA2_HIFIL_HIFIR_GAIN_RIGHT] = DPGA2_HIFIL_HIFIR_GAIN_RIGHT_DEFAULT_VALUE;

	if (_pmicId > 0x41 /* LEVANTE_ID_A1 */)
	{       /* LEVANTE_ID_B0 or newer */
		/* This part is deleted b/c it prevents from the accesory detection from working! should be fixed in Levante B0 */
		/* Turn Off digital and analog sections, Audio clocks and registers are Off, Turn Off audio PLL; Shadow should NOT be updated!  */
		LevanteRegSet(LEVANTE_MISC2_ADDRESS,
			      ( shadowReg[LEVANTE_MISC2_IDX] &
				(~((1 << AUDIO_PU_SHIFT)     |
				   (1 << AUDIO_RSTB_SHIFT)   |
				   (1 << PU_PLL_SHIFT))) ));

		/* Turn Off regulator 2.7 V, Turn Off charge pump, Audio is Reset; Shadow should NOT be updated! */
		LevanteRegSet(LEVANTE_AUDIO_SUPPLIES2_ADDRESS,
			      ( shadowReg[LEVANTE_AUDIO_SUPPLIES2_IDX] &
				(~((1 << REG27_MPU_SHIFT)              |
				   (1 << CPUMP_EN_SHIFT)               |
				   (1 << AUDIO_EN_SHIFT))) ));
	}
	else
	{       /* LEVANTE_ID_A0 or LEVANTE_ID_A1 */
		LevanteRegSet(LEVANTE_AUDIO_SUPPLIES2_ADDRESS,
			      ( shadowReg[LEVANTE_AUDIO_SUPPLIES2_IDX] &
				(~((1 << CPUMP_EN_SHIFT)               |
				   (1 << AUDIO_EN_SHIFT))) ));
	}

	/* Apply the changes! */
	//DIAG_FILTER(AUDIO_LEVANTE, LEVANTE, AUDIO_LEVANTE_ShutDown_Apply, DIAG_INFORMATION)
	//diagTextPrintf("Levante Apply Changes");

	/*Raul: ASOC will remember the value, so first clear and then set to ensure write the register successfully*/
	LevanteRegSet(LEVANTE_EAR_SPKR_CTRL2_ADDRESS, (shadowReg[LEVANTE_EAR_SPKR_CTRL2_IDX] & (~(1 << APPLY_CHANGES_SHIFT))));
	LevanteRegSet(LEVANTE_EAR_SPKR_CTRL2_ADDRESS, (shadowReg[LEVANTE_EAR_SPKR_CTRL2_IDX] | (1 << APPLY_CHANGES_SHIFT)));
} /* End of LevanteShutDownConfigSet */


/************************************************************************
 * Function: LevanteAddPathToList
 *************************************************************************
 * Description:  Get the digital gain (MSA).
 *
 * Parameters:   unsigned char   path
 *               ACM_AudioVolume volume
 *
 * Return value: ACM_DigitalGain
 *
 * Notes:
 *************************************************************************/
static ACM_DigitalGain LevanteGetMsaDigitalGain(unsigned char path, ACM_AudioVolume volume)
{
	unsigned char pathIdx;
	int gain;

	LEVANTE_FIND_PATH(pathIdx, path, gLevanteMsaDigitalGain, LEVANTE_MSA_DIGITAL_GAIN_MAX);

	if (pathIdx == LEVANTE_MSA_DIGITAL_GAIN_MAX)
	{       /* No digital gain was found! */
		return INVALID_DIGITAL_GAIN;
	}

	gain = LEVANTE_GAIN_CALC(volume, gLevanteMsaDigitalGain[pathIdx].pga);

	//DIAG_FILTER(AUDIO_LEVANTE, LEVANTE, AUDIO_LEVANTE_DigitalGain, DIAG_INFORMATION)
	//diagPrintf("AUDIO_LEVANTE_DigitalGain (%d)", (ACM_DigitalGain)LEVANTE_DIGITAL_GAIN_CLIP(gain) );

	return (ACM_DigitalGain)LEVANTE_DIGITAL_GAIN_CLIP(gain);
} /* End of LevanteGetMsaDigitalGain */


/************************************************************************
 * Function: LevanteAddPathToList
 *************************************************************************
 * Description:  Update the codec data-base. Sort the path list.
 *
 * Parameters:   unsigned char path
 *
 * Return value:
 *
 * Notes:
 *************************************************************************/
static void LevanteSortPathList(void)
{
	int path;

	for (path = 0; path < LEVANTE_TOTAL_VALID_PATHS; path++)
	{
		if (gLevanteCurrentPaths[path].order != INVALID_INDEX)
		{
			gLevantePathsSortedList[gLevanteCurrentPaths[path].order - 1] = path;
		}
	}
} /* End of LevanteSortPathList */


/************************************************************************
 * Function: LevanteAddPathToList
 *************************************************************************
 * Description:  Update the codec data-base. Remove path to the list.
 *
 * Parameters:   unsigned char path
 *
 * Return value:
 *
 * Notes:
 *************************************************************************/
static void LevanteRemovePathFromList(unsigned char path)
{
	int i;
	unsigned char order = gLevanteCurrentPaths[path].order;

	/* if already disabled */
	if (gLevanteCurrentPaths[path].order == INVALID_INDEX)
		return;

	/* remove path from list */
	gLevanteCurrentPaths[path].order   = INVALID_INDEX;
	gLevanteCurrentPaths[path].volume  = INVALID_INDEX;
	gLevanteCurrentPaths[path].muted   = ACM_MUTE_OFF;

	/* update database */
	gLevanteDatabase.totalEnables--;

	if (gLevanteDatabase.totalEnables != 0)
	{
		for (i = 0; i < LEVANTE_TOTAL_VALID_PATHS; i++)
		{
			if ((gLevanteCurrentPaths[i].order != INVALID_INDEX) && (gLevanteCurrentPaths[i].order > order))
			{
				gLevanteCurrentPaths[i].order--;
			}
		}
		LevanteSortPathList();
	}
} /* End of LevanteRemovePathFromList */


/************************************************************************
 * Function: LevanteAddPathToList
 *************************************************************************
 * Description:  Update the codec data-base. Add path to the list.
 *
 * Parameters:   unsigned char   path
 *               ACM_AudioVolume volume
 *               ACM_DigitalGain digGain
 *
 * Return value:
 *
 * Notes:
 *************************************************************************/
static void LevanteAddPathToList(unsigned char path, ACM_AudioVolume volume, ACM_DigitalGain digGain)
{
	BOOL IsPathReEnabled = FALSE;
	/* re-enabled ->first remove it's current location */
	if (gLevanteCurrentPaths[path].order != INVALID_INDEX)
	{
		IsPathReEnabled = TRUE;
		LevanteRemovePathFromList(path);
	}
	/*we shouldn't StartUpConfigSet again after re-enable the same path*/
	/*or it sets 0x42 with 0 before disabling DAC, and this results in black screen*/
       /* After setting 0x42.2, make sure power down audio after disabling DAC*/
	if ((gLevanteDatabase.totalEnables == 0) &&  (!IsPathReEnabled))
	{       /* This is the case of turning the first path on */
		printf("----------- LevanteStartUpConfigSet: we only support Levante B0 now! ---------\n");
		LevanteStartUpConfigSet();
	}

	/* update database */
	gLevanteDatabase.totalEnables++;

	/* update path data */
	gLevanteCurrentPaths[path].order       = gLevanteDatabase.totalEnables;
	gLevanteCurrentPaths[path].volume      = volume;
	gLevanteCurrentPaths[path].muted       = ACM_MUTE_OFF;
	gLevanteCurrentPaths[path].digitalGain = digGain;

	LevanteSortPathList();
} /* End of LevanteAddPathToList */


/************************************************************************
 * Function: LevanteIsVoicePath
 *************************************************************************
 * Description:  To see whether the path is a voice call audio path
 *
 * Parameters:
 *
 * Return value:
 *
 * Notes:
 *************************************************************************/
static BOOL LevanteIsVoicePath(int path)
{
	if (path >= LEVANTE_IN_L_TO_OUT && path <= LEVANTE_IN_BOTH_TO_OUT) /*sidetone path*/
		return TRUE;
	if (path >= LEVANTE_PCM_L_TO_EAR && path <= LEVANTE_PCM_R_TO_LINEOUT) /*PCM input path*/
		return TRUE;
	if (path >= LEVANTE_AnaMIC1_TO_PCM_L && path <= LEVANTE_AUX2_TO_PCM_L) /*PCM output path*/
		return TRUE;

	return FALSE;
}


/************************************************************************
 * Function: LevanteUpdateHardware
 *************************************************************************
 * Description:  Update the codec. Find the right value of all codec's
 *               registers.
 *
 * Parameters:
 *
 * Return value:
 *
 * Notes:
 *************************************************************************/
static void LevanteUpdateHardware(void)
{
	int sortedPath, i, j, k, groupIdx, groupOrderIdx;
	LevantePathInput pathInput;
	LevantePathOutput pathOutput;
	unsigned char gain, totalPathInGain;
	unsigned char       gainHS1 = INVALID_INDEX, gainHS2 = INVALID_INDEX, gainLofiLeft = INVALID_INDEX;
	unsigned char       gainHifilLeft = INVALID_INDEX, gainLofiRight = INVALID_INDEX, gainHifirRight = INVALID_INDEX;
	unsigned char reg[NUM_OF_REGS];
	BOOL isChanged = FALSE, isHighVoltageUse = FALSE, isForceSpeakerOn = FALSE, isFmHeadsetEnabled = FALSE, isVoicePathUse = FALSE;

	for (i = 0; i < NUM_OF_REGS; i++)
	{
		reg[i] = registerInfo[i].defaultValue;
	}

	for (sortedPath = 0; sortedPath < gLevanteDatabase.totalEnables; sortedPath++)
	{
		LevantePaths path   = gLevantePathsSortedList[sortedPath];
		unsigned char volume = gLevanteCurrentPaths[path].volume;
		ACM_AudioMute mute   = gLevanteCurrentPaths[path].muted;

		pathInput  = LEVANTE_GET_PATH_INPUT(gLevantePathsSortedList[sortedPath]);
		pathOutput = LEVANTE_GET_PATH_OUTPUT(gLevantePathsSortedList[sortedPath]);

		//DIAG_FILTER(AUDIO_LEVANTE, LEVANTE, AUDIO_LEVANTE_LevanteUpdateHardware_0, DIAG_INFORMATION)
		//diagPrintf("AUDIO_LEVANTE_LevanteUpdateHardware (p= %d, i= %d, o= %d)", (UINT16)path, (UINT16)pathInput, (UINT16)pathOutput);

		switch (pathOutput)
		{
		case SideTone_OUT:
			break;

		case EAR_VIA_DAC1_OUT:
			WRITE_SPECIFIC_BITS(reg[LEVANTE_ANALOG_INPUT_SEL2_IDX], 2, SEL_SPEA);          /* 0b10 */
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_ANALOG_TO_ANALOG_IDX], SEL_SP_EA);
//				printf("-- 1 reg 0xd4 is 0x%x, shadowReg 0xd4 is 0x%x\n", reg[LEVANTE_ANALOG_TO_ANALOG_IDX], shadowReg[LEVANTE_ANALOG_TO_ANALOG_IDX]);

			WRITE_SPECIFIC_BITS(reg[LEVANTE_EAR_SPKR_CTRL1_IDX], 2, SPEA_IHOP1);          /* 0b10 */
			WRITE_SPECIFIC_BITS(reg[LEVANTE_EAR_SPKR_CTRL1_IDX], 1, SPKR_IBIAS3);

			if (mute == ACM_MUTE_ON)
			{
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_MODULATOR);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_DAC_LEFT);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_DAC_RIGHT);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], EAR_PA_EN);

				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], 1, MUTE_LEFT);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_MODULATOR);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_DAC_LEFT);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_DAC_RIGHT);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], 1, EAR_PA_EN);

				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], MUTE_LEFT);
			}
			break;

		case SPKR_VIA_DAC1_OUT:
			WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ANALOG_PROGRAM4_IDX], regField[AUDIO_ZLOAD_4OHM], AUDIO_ZLOAD_4OHM);

			WRITE_SPECIFIC_BITS(reg[LEVANTE_ANALOG_INPUT_SEL2_IDX], 2, SEL_SPEA);          /* 0b10 */
			WRITE_SPECIFIC_BITS(reg[LEVANTE_ANALOG_TO_ANALOG_IDX], 1, SEL_SP_EA);
//				printf("-- 2 reg 0xd4 is 0x%x, shadowReg 0xd4 is 0x%x\n", reg[LEVANTE_ANALOG_TO_ANALOG_IDX], shadowReg[LEVANTE_ANALOG_TO_ANALOG_IDX]);
			if (mute == ACM_MUTE_ON)
			{
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_MODULATOR);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_DAC_LEFT);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_DAC_RIGHT);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], SPKR_PA_EN);

				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], 1, MUTE_LEFT);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_MODULATOR);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_DAC_LEFT);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_DAC_RIGHT);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], 1, SPKR_PA_EN);

				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], MUTE_LEFT);
			}
			break;

		case HS1_VIA_DAC1_OUT:
			WRITE_SPECIFIC_BITS(reg[LEVANTE_ANALOG_INPUT_SEL1_IDX], 2, SEL_HS1);          /* 0b10 */
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_ANALOG_TO_ANALOG_IDX], HS1_SEL_ANADIG_IN);

			shadowReg[LEVANTE_SHORTS_IDX] = 0;          /* Makes sure it'll be sent even if there's no change! */
			WRITE_SPECIFIC_BITS(reg[LEVANTE_SHORTS_IDX], 1, HS1_SHORT_P);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_SHORTS_IDX], 1, HS2_SHORT_P);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_SHORTS_IDX], 1, LO1_SHORT_P);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_SHORTS_IDX], 1, LO2_SHORT_P);

			if (mute == ACM_MUTE_ON)
			{
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_MODULATOR);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_DAC_LEFT);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_DAC_RIGHT);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], HS1_PA_EN);

				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], 1, MUTE_LEFT);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_MODULATOR);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_DAC_LEFT);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_DAC_RIGHT);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], 1, HS1_PA_EN);

				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], MUTE_LEFT);
			}
			break;

		case HS2_VIA_DAC1_OUT:
			WRITE_SPECIFIC_BITS(reg[LEVANTE_ANALOG_INPUT_SEL1_IDX], 2, SEL_HS2);          /* 0b10 */
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_ANALOG_TO_ANALOG_IDX], HS2_SEL_ANADIG_IN);

			shadowReg[LEVANTE_SHORTS_IDX] = 0;          /* Makes sure it'll be sent even if there's no change! */
			WRITE_SPECIFIC_BITS(reg[LEVANTE_SHORTS_IDX], 1, HS1_SHORT_P);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_SHORTS_IDX], 1, HS2_SHORT_P);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_SHORTS_IDX], 1, LO1_SHORT_P);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_SHORTS_IDX], 1, LO2_SHORT_P);

			if (mute == ACM_MUTE_ON)
			{
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_MODULATOR);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_DAC_LEFT);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_DAC_RIGHT);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], HS2_PA_EN);

				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], 1, MUTE_LEFT);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_MODULATOR);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_DAC_LEFT);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_DAC_RIGHT);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], 1, HS2_PA_EN);

				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], MUTE_LEFT);
			}
			break;

		case LINEOUT1_VIA_DAC1_OUT:
			WRITE_SPECIFIC_BITS(reg[LEVANTE_ANALOG_INPUT_SEL1_IDX], 2, SEL_LO1);          /* 0b10 */
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_ANALOG_TO_ANALOG_IDX], LO1_SEL_ANADIG_IN);

			shadowReg[LEVANTE_SHORTS_IDX] = 0;          /* Makes sure it'll be sent even if there's no change! */
			WRITE_SPECIFIC_BITS(reg[LEVANTE_SHORTS_IDX], 1, HS1_SHORT_P);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_SHORTS_IDX], 1, HS2_SHORT_P);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_SHORTS_IDX], 1, LO1_SHORT_P);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_SHORTS_IDX], 1, LO2_SHORT_P);

			if (mute == ACM_MUTE_ON)
			{
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_MODULATOR);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_DAC_LEFT);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_DAC_RIGHT);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], LO1_PA_EN);

				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], 1, MUTE_LEFT);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_MODULATOR);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_DAC_LEFT);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_DAC_RIGHT);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], 1, LO1_PA_EN);

				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], MUTE_LEFT);
			}
			break;

		case LINEOUT2_VIA_DAC1_OUT:
			WRITE_SPECIFIC_BITS(reg[LEVANTE_ANALOG_INPUT_SEL1_IDX], 2, SEL_LO2);          /* 0b10 */
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_ANALOG_TO_ANALOG_IDX], LO2_SEL_ANADIG_IN);

			shadowReg[LEVANTE_SHORTS_IDX] = 0;          /* Makes sure it'll be sent even if there's no change! */
			WRITE_SPECIFIC_BITS(reg[LEVANTE_SHORTS_IDX], 1, HS1_SHORT_P);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_SHORTS_IDX], 1, HS2_SHORT_P);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_SHORTS_IDX], 1, LO1_SHORT_P);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_SHORTS_IDX], 1, LO2_SHORT_P);

			if (mute == ACM_MUTE_ON)
			{
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_MODULATOR);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_DAC_LEFT);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_DAC_RIGHT);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], LO2_PA_EN);

				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], 1, MUTE_LEFT);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_MODULATOR);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_DAC_LEFT);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_DAC_RIGHT);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], 1, LO2_PA_EN);

				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], MUTE_LEFT);
			}
			break;

		case EAR_VIA_DAC2_OUT:
			WRITE_SPECIFIC_BITS(reg[LEVANTE_ANALOG_INPUT_SEL2_IDX], 1, SEL_SPEA);
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_ANALOG_TO_ANALOG_IDX], SEL_SP_EA);

			if (mute == ACM_MUTE_ON)
			{
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_MODULATOR);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_DAC_RIGHT);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_DAC_LEFT);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], EAR_PA_EN);

				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], 1, MUTE_RIGHT);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_MODULATOR);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_DAC_RIGHT);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_DAC_LEFT);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], 1, EAR_PA_EN);

				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], MUTE_RIGHT);
			}
			break;

		case SPKR_VIA_DAC2_OUT:
			WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ANALOG_PROGRAM4_IDX], regField[AUDIO_ZLOAD_4OHM], AUDIO_ZLOAD_4OHM);

			WRITE_SPECIFIC_BITS(reg[LEVANTE_ANALOG_INPUT_SEL2_IDX], 1, SEL_SPEA);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_ANALOG_TO_ANALOG_IDX], 1, SEL_SP_EA);

			if (mute == ACM_MUTE_ON)
			{
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_MODULATOR);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_DAC_RIGHT);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_DAC_LEFT);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], SPKR_PA_EN);

				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], 1, MUTE_RIGHT);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_MODULATOR);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_DAC_RIGHT);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_DAC_LEFT);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], 1, SPKR_PA_EN);

				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], MUTE_RIGHT);
			}
			break;

		case HS1_VIA_DAC2_OUT:
			WRITE_SPECIFIC_BITS(reg[LEVANTE_ANALOG_INPUT_SEL1_IDX], 1, SEL_HS1);
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_ANALOG_TO_ANALOG_IDX], HS1_SEL_ANADIG_IN);

			if (mute == ACM_MUTE_ON)
			{
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_MODULATOR);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_DAC_RIGHT);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_DAC_LEFT);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], HS1_PA_EN);

				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], 1, MUTE_RIGHT);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_MODULATOR);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_DAC_RIGHT);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_DAC_LEFT);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], 1, HS1_PA_EN);

				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], MUTE_RIGHT);
			}
			break;

		case HS2_VIA_DAC2_OUT:
			WRITE_SPECIFIC_BITS(reg[LEVANTE_ANALOG_INPUT_SEL1_IDX], 1, SEL_HS2);
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_ANALOG_TO_ANALOG_IDX], HS2_SEL_ANADIG_IN);

			if (mute == ACM_MUTE_ON)
			{
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_MODULATOR);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_DAC_RIGHT);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_DAC_LEFT);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], HS2_PA_EN);

				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], 1, MUTE_RIGHT);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_MODULATOR);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_DAC_RIGHT);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_DAC_LEFT);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], 1, HS2_PA_EN);

				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], MUTE_RIGHT);
			}
			break;

		case LINEOUT1_VIA_DAC2_OUT:
			WRITE_SPECIFIC_BITS(reg[LEVANTE_ANALOG_INPUT_SEL1_IDX], 1, SEL_LO1);
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_ANALOG_TO_ANALOG_IDX], LO1_SEL_ANADIG_IN);

			if (mute == ACM_MUTE_ON)
			{
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_MODULATOR);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_DAC_RIGHT);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_DAC_LEFT);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], LO1_PA_EN);

				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], 1, MUTE_RIGHT);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_MODULATOR);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_DAC_RIGHT);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_DAC_LEFT);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], 1, LO1_PA_EN);

				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], MUTE_RIGHT);
			}
			break;

		case LINEOUT2_VIA_DAC2_OUT:
			WRITE_SPECIFIC_BITS(reg[LEVANTE_ANALOG_INPUT_SEL1_IDX], 1, SEL_LO2);
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_ANALOG_TO_ANALOG_IDX], LO2_SEL_ANADIG_IN);

			if (mute == ACM_MUTE_ON)
			{
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_MODULATOR);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_DAC_RIGHT);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_DAC_LEFT);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], LO2_PA_EN);

				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], 1, MUTE_RIGHT);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_MODULATOR);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_DAC_RIGHT);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_DAC_LEFT);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], 1, LO2_PA_EN);

				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], MUTE_RIGHT);
			}
			break;

		case HS_STEREO_OUT:
			WRITE_SPECIFIC_BITS(reg[LEVANTE_ANALOG_INPUT_SEL1_IDX], 2, SEL_HS1);          /* 0b10 */
			WRITE_SPECIFIC_BITS(reg[LEVANTE_ANALOG_INPUT_SEL1_IDX], 1, SEL_HS2);

			CLEAR_SPECIFIC_BITS(reg[LEVANTE_ANALOG_TO_ANALOG_IDX], LO2_SEL_ANADIG_IN);
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_ANALOG_TO_ANALOG_IDX], HS2_SEL_ANADIG_IN);

			shadowReg[LEVANTE_SHORTS_IDX] = 0;          /* Makes sure it'll be sent even if there's no change! */
			WRITE_SPECIFIC_BITS(reg[LEVANTE_SHORTS_IDX], 1, HS1_SHORT_P);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_SHORTS_IDX], 1, HS2_SHORT_P);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_SHORTS_IDX], 1, LO1_SHORT_P);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_SHORTS_IDX], 1, LO2_SHORT_P);

			if (mute == ACM_MUTE_ON)
			{
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_MODULATOR);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_DAC_RIGHT);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_DAC_LEFT);

				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], HS1_PA_EN);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], HS2_PA_EN);

				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], 1, MUTE_LEFT);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], 1, MUTE_RIGHT);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_MODULATOR);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_DAC_RIGHT);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_DAC_LEFT);

				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], 1, HS1_PA_EN);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], 1, HS2_PA_EN);

				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], MUTE_LEFT);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], MUTE_RIGHT);
			}
			break;

		case HS_STEREO_FLIP_OUT:
			WRITE_SPECIFIC_BITS(reg[LEVANTE_ANALOG_INPUT_SEL1_IDX], 1, SEL_HS1);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_ANALOG_INPUT_SEL1_IDX], 2, SEL_HS2);          /* 0b10 */

			CLEAR_SPECIFIC_BITS(reg[LEVANTE_ANALOG_TO_ANALOG_IDX], HS1_SEL_ANADIG_IN);
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_ANALOG_TO_ANALOG_IDX], HS2_SEL_ANADIG_IN);

			shadowReg[LEVANTE_SHORTS_IDX] = 0;          /* Makes sure it'll be sent even if there's no change! */
			WRITE_SPECIFIC_BITS(reg[LEVANTE_SHORTS_IDX], 1, HS1_SHORT_P);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_SHORTS_IDX], 1, HS2_SHORT_P);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_SHORTS_IDX], 1, LO1_SHORT_P);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_SHORTS_IDX], 1, LO2_SHORT_P);

			if (mute == ACM_MUTE_ON)
			{
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_MODULATOR);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_DAC_RIGHT);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_DAC_LEFT);

				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], HS1_PA_EN);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], HS2_PA_EN);

				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], 1, MUTE_LEFT);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], 1, MUTE_RIGHT);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_MODULATOR);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_DAC_RIGHT);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_DAC_LEFT);

				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], 1, HS1_PA_EN);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], 1, HS2_PA_EN);

				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], MUTE_LEFT);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], MUTE_RIGHT);
			}
			break;

		case LINEOUT_STEREO_OUT:
			WRITE_SPECIFIC_BITS(reg[LEVANTE_ANALOG_INPUT_SEL1_IDX], 2, SEL_LO1);          /* 0b10 */
			WRITE_SPECIFIC_BITS(reg[LEVANTE_ANALOG_INPUT_SEL1_IDX], 1, SEL_LO2);

			CLEAR_SPECIFIC_BITS(reg[LEVANTE_ANALOG_TO_ANALOG_IDX], LO1_SEL_ANADIG_IN);
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_ANALOG_TO_ANALOG_IDX], LO2_SEL_ANADIG_IN);

			shadowReg[LEVANTE_SHORTS_IDX] = 0;          /* Makes sure it'll be sent even if there's no change! */
			WRITE_SPECIFIC_BITS(reg[LEVANTE_SHORTS_IDX], 1, HS1_SHORT_P);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_SHORTS_IDX], 1, HS2_SHORT_P);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_SHORTS_IDX], 1, LO1_SHORT_P);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_SHORTS_IDX], 1, LO2_SHORT_P);

			if (mute == ACM_MUTE_ON)
			{
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_MODULATOR);

				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], LO1_PA_EN);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], LO2_PA_EN);

				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], 1, MUTE_LEFT);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], 1, MUTE_RIGHT);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_MODULATOR);

				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], 1, LO1_PA_EN);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], 1, LO2_PA_EN);

				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], MUTE_LEFT);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], MUTE_RIGHT);
			}
			break;

		case LINEOUT_STEREO_FLIP_OUT:
			WRITE_SPECIFIC_BITS(reg[LEVANTE_ANALOG_INPUT_SEL1_IDX], 1, SEL_LO1);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_ANALOG_INPUT_SEL1_IDX], 2, SEL_LO2);          /* 0b10 */

			CLEAR_SPECIFIC_BITS(reg[LEVANTE_ANALOG_TO_ANALOG_IDX], LO1_SEL_ANADIG_IN);
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_ANALOG_TO_ANALOG_IDX], LO2_SEL_ANADIG_IN);

			shadowReg[LEVANTE_SHORTS_IDX] = 0;          /* Makes sure it'll be sent even if there's no change! */
			WRITE_SPECIFIC_BITS(reg[LEVANTE_SHORTS_IDX], 1, HS1_SHORT_P);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_SHORTS_IDX], 1, HS2_SHORT_P);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_SHORTS_IDX], 1, LO1_SHORT_P);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_SHORTS_IDX], 1, LO2_SHORT_P);

			if (mute == ACM_MUTE_ON)
			{
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_MODULATOR);

				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], LO1_PA_EN);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], LO2_PA_EN);

				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], 1, MUTE_LEFT);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], 1, MUTE_RIGHT);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_MODULATOR);

				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], 1, LO1_PA_EN);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], 1, LO2_PA_EN);

				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], MUTE_LEFT);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], MUTE_RIGHT);
			}
			break;

		case HS2_ANA_LOOP_OUT:
			WRITE_SPECIFIC_BITS(reg[LEVANTE_ANALOG_TO_ANALOG_IDX], 1, HS2_SEL_ANADIG_IN);

			if (mute == ACM_MUTE_ON)
			{
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], HS2_PA_EN);
				//FIXME: we need to enable EN_DAC_LEFT or EN_DAC_RIGHT to make an active output
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_DAC_LEFT);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], 1, HS2_PA_EN);
				//FIXME: we need to enable EN_DAC_LEFT or EN_DAC_RIGHT to make an active output
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_DAC_LEFT);
			}
			break;

		case LINEOUT2_ANA_LOOP_OUT:
			WRITE_SPECIFIC_BITS(reg[LEVANTE_ANALOG_TO_ANALOG_IDX], 1, LO2_SEL_ANADIG_IN);


			if (mute == ACM_MUTE_ON)
			{
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], LO2_PA_EN);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], 1, LO2_PA_EN);
			}
			break;

		case HS1_ANA_LOOP_OUT:
			WRITE_SPECIFIC_BITS(reg[LEVANTE_ANALOG_TO_ANALOG_IDX], 1, HS1_SEL_ANADIG_IN);

			if (mute == ACM_MUTE_ON)
			{
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], HS1_PA_EN);
				//FIXME: we need to enable EN_DAC_LEFT or EN_DAC_RIGHT to make an active output
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_DAC_LEFT);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], 1, HS1_PA_EN);
				//FIXME: we need to enable EN_DAC_LEFT or EN_DAC_RIGHT to make an active output
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_DAC_LEFT);
			}
			break;

		case LINEOUT1_ANA_LOOP_OUT:
			WRITE_SPECIFIC_BITS(reg[LEVANTE_ANALOG_TO_ANALOG_IDX], 1, LO1_SEL_ANADIG_IN);

			if (mute == ACM_MUTE_ON)
			{
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], LO1_PA_EN);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], 1, LO1_PA_EN);
			}
			break;

		case PCM_L_OUT:
			switch (pathInput)
			{
			case AnaMIC3_IN:
			case AUX2_IN:
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE3_IDX], LEFT_DATSEL);
				break;

			case AnaMIC1_IN:
			case AnaMIC2_IN:
			case AUX1_IN:
				WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE3_IDX], 1, LEFT_DATSEL);
				//FIXME: Raul add this per Jackie's request, without this may cause sidetone no effect?
				WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE3_IDX], 1, RIGHT_DATSEL);
				break;

			case DigMIC1_IN:
				WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE3_IDX], 2, LEFT_DATSEL);                  /* 0b10 */
				break;

			case DigMIC2_IN:
				WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE3_IDX], 3, LEFT_DATSEL);                  /* 0b11 */
				break;

			default:
				DPRINTF("\nassert in PCM_L_OUT default\n");
				assert(FALSE);
				break;
			}

			WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], 1, PCM_EN);

			WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE2_IDX], 4, PCM_MODE);          /* 0b100 */
			WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE2_IDX], regField[PCM_WL], PCM_WL);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE2_IDX], regField[PCM_MASTER], PCM_MASTER);
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE2_IDX], PCM_FS_POL);
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE2_IDX], PCM_BCLK_POL);

			WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE3_IDX], regField[HIZ_DOUT], HIZ_DOUT);

			if (mute == ACM_MUTE_ON)
			{
				/*comment this by Raul, if clear this bit, when mute mic in voice call, pcm out will also be muted, don't know why*/
				//CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], LOFI_RX_EN);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], EN_ADC_LEFT);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], 1, LOFI_RX_EN);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], 1, EN_ADC_LEFT);
			}
			break;

		case PCM_I2S_L_OUT:
			switch (pathInput)
			{
			case AnaMIC3_IN:
			case AUX2_IN:
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE3_IDX], LEFT_DATSEL);
				break;

			case AnaMIC1_IN:
			case AnaMIC2_IN:
			case AUX1_IN:
				WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE3_IDX], 1, LEFT_DATSEL);
				//FIXME: Raul add this per Jackie's request, without this may cause sidetone no effect?
				WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE3_IDX], 1, RIGHT_DATSEL);
				break;

			case DigMIC1_IN:
				WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE3_IDX], 2, LEFT_DATSEL);                  /* 0b10 */
				break;

			case DigMIC2_IN:
				WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE3_IDX], 3, LEFT_DATSEL);                  /* 0b11 */
				break;

			default:
				DPRINTF("\nassert in PCM_L_OUT default\n");
				assert(FALSE);
				break;
			}

			WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], 1, PCM_EN);

			WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE2_IDX], 1, PCM_MODE);          /* 0b100 */
			WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE2_IDX], regField[PCM_WL], PCM_WL);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE2_IDX], regField[PCM_MASTER], PCM_MASTER);
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE2_IDX], PCM_FS_POL);
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE2_IDX], PCM_BCLK_POL);

			WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE3_IDX], regField[HIZ_DOUT], HIZ_DOUT);

			if (mute == ACM_MUTE_ON)
			{
				/*comment this by Raul, if clear this bit, when mute mic in voice call, pcm out will also be muted, don't know why*/
				//CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], LOFI_RX_EN);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], EN_ADC_LEFT);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], 1, LOFI_RX_EN);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], 1, EN_ADC_LEFT);
			}
			break;

		case SIDETONE_L_OUT:
			switch (pathInput)
			{
			case AnaMIC3_IN:
			case AUX2_IN:
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE3_IDX], LEFT_DATSEL);
				break;

			case AnaMIC1_IN:
			case AnaMIC2_IN:
			case AUX1_IN:
				WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE3_IDX], 1, LEFT_DATSEL);
				//FIXME: Raul add this per Jackie's request, without this may cause sidetone no effect?
				WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE3_IDX], 1, RIGHT_DATSEL);
				break;

			case DigMIC1_IN:
				WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE3_IDX], 2, LEFT_DATSEL);                  /* 0b10 */
				break;

			case DigMIC2_IN:
				WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE3_IDX], 3, LEFT_DATSEL);                  /* 0b11 */
				break;

			default:
				DPRINTF("\nassert in PCM_L_OUT default\n");
				assert(FALSE);
				break;
			}

			if (mute == ACM_MUTE_ON)
			{
				/*comment this by Raul, if clear this bit, when mute mic in voice call, pcm out will also be muted, don't know why*/
				//CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], LOFI_RX_EN);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], EN_ADC_LEFT);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], 1, LOFI_RX_EN);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], 1, EN_ADC_LEFT);
			}
			break;

		case PCM_R_OUT:
			switch (pathInput)
			{
			case AnaMIC1_IN:
			case AnaMIC2_IN:
			case AUX1_IN:
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE3_IDX], RIGHT_DATSEL);
				break;

			case AnaMIC3_IN:
			case AUX2_IN:
				WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE3_IDX], 1, RIGHT_DATSEL);
				break;

			case DigMIC2_IN:
				WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE3_IDX], 2, RIGHT_DATSEL);                  /* 0b10 */
				break;

			case DigMIC1_IN:
				WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE3_IDX], 3, RIGHT_DATSEL);                  /* 0b11 */
				break;

			default:
				DPRINTF("\nassert in PCM_R_OUT default\n");
				assert(FALSE);
				break;
			}

			WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], 1, PCM_EN);

			WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE2_IDX], 4, PCM_MODE);          /* 0b100 */
			WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE2_IDX], regField[PCM_WL], PCM_WL);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE2_IDX], regField[PCM_MASTER], PCM_MASTER);
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE2_IDX], PCM_FS_POL);
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE2_IDX], PCM_BCLK_POL);

			WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE3_IDX], regField[HIZ_DOUT], HIZ_DOUT);

			if (mute == ACM_MUTE_ON)
			{
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], LOFI_RX_EN);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], EN_ADC_RIGHT);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], 1, LOFI_RX_EN);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], 1, EN_ADC_RIGHT);
			}
			break;

		case PCM_I2S_R_OUT:
			switch (pathInput)
			{
			case AnaMIC1_IN:
			case AnaMIC2_IN:
			case AUX1_IN:
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE3_IDX], RIGHT_DATSEL);
				break;

			case AnaMIC3_IN:
			case AUX2_IN:
				WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE3_IDX], 1, RIGHT_DATSEL);
				break;

			case DigMIC2_IN:
				WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE3_IDX], 2, RIGHT_DATSEL);                  /* 0b10 */
				break;

			case DigMIC1_IN:
				WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE3_IDX], 3, RIGHT_DATSEL);                  /* 0b11 */
				break;

			default:
				DPRINTF("\nassert in PCM_R_OUT default\n");
				assert(FALSE);
				break;
			}

			WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], 1, PCM_EN);

			WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE2_IDX], 1, PCM_MODE);          /* 0b100 */
			WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE2_IDX], regField[PCM_WL], PCM_WL);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE2_IDX], regField[PCM_MASTER], PCM_MASTER);
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE2_IDX], PCM_FS_POL);
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE2_IDX], PCM_BCLK_POL);

			WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE3_IDX], regField[HIZ_DOUT], HIZ_DOUT);

			if (mute == ACM_MUTE_ON)
			{
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], LOFI_RX_EN);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], EN_ADC_RIGHT);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], 1, LOFI_RX_EN);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], 1, EN_ADC_RIGHT);
			}
			break;

		case SIDETONE_R_OUT:
			switch (pathInput)
			{
			case AnaMIC1_IN:
			case AnaMIC2_IN:
			case AUX1_IN:
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE3_IDX], RIGHT_DATSEL);
				break;

			case AnaMIC3_IN:
			case AUX2_IN:
				WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE3_IDX], 1, RIGHT_DATSEL);
				break;

			case DigMIC2_IN:
				WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE3_IDX], 2, RIGHT_DATSEL);                  /* 0b10 */
				break;

			case DigMIC1_IN:
				WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE3_IDX], 3, RIGHT_DATSEL);                  /* 0b11 */
				break;

			default:
				DPRINTF("\nassert in PCM_R_OUT default\n");
				assert(FALSE);
				break;
			}

			if (mute == ACM_MUTE_ON)
			{
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], LOFI_RX_EN);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], EN_ADC_RIGHT);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], 1, LOFI_RX_EN);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], 1, EN_ADC_RIGHT);
			}
			break;

		case I2S_VIA_PCM_L_OUT:
			switch (pathInput)
			{
			case AnaMIC3_IN:
			case AUX2_IN:
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE3_IDX], LEFT_DATSEL);
				break;

			case AnaMIC1_IN:
			case AnaMIC2_IN:
			case AUX1_IN:
				WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE3_IDX], 1, LEFT_DATSEL);
				break;

			case DigMIC1_IN:
				WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE3_IDX], 2, LEFT_DATSEL);                  /* 0b10 */
				break;

			case DigMIC2_IN:
				WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE3_IDX], 3, LEFT_DATSEL);                  /* 0b11 */
				break;

			default:
				DPRINTF("\nassert in I2S_VIA_PCM_L_OUT default\n");
				assert(FALSE);
				break;
			}

			WRITE_SPECIFIC_BITS(reg[LEVANTE_EAR_SPKR_CTRL2_IDX], regField[I_OPA_HALF], I_OPA_HALF);

			if(_pmicId <= 0x41)
			{  /* LEVANTE_ID_A0 or LEVANTE_ID_A1 */
				WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE1_IDX], 0, I2S_OFFSR_Ax);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE1_IDX], 0, I2S_OFFSL_Ax);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE5_IDX], 0, I2S_OFFS_R);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE1_IDX], 0, I2S_OFFS_L);
			}

			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE2_IDX], EXACT_I2S, I2S_MODE);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE2_IDX], regField[I2S_WL], I2S_WL);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE2_IDX], regField[I2S_MASTER], I2S_MASTER);
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE2_IDX], I2S_FS_POL);
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE2_IDX], I2S_BCLK_POL);

			WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, I2S_EN);

			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE3_IDX], 1, I2S_MICDATS);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE3_IDX], regField[HIZ_I2S_DOUT], HIZ_I2S_DOUT);

			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE4_IDX], regField[RATE_DAC_I2S], RATE_DAC_I2S);          /* I2S rate set */

			if (mute == ACM_MUTE_ON)
			{
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], LOFI_RX_EN);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], EN_ADC_LEFT);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], 1, LOFI_RX_EN);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], 1, EN_ADC_LEFT);
			}
			break;

		case I2S_VIA_PCM_R_OUT:
			switch (pathInput)
			{
			case AnaMIC1_IN:
			case AnaMIC2_IN:
			case AUX1_IN:
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE3_IDX], RIGHT_DATSEL);
				break;

			case AnaMIC3_IN:
			case AUX2_IN:
				WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE3_IDX], 1, RIGHT_DATSEL);
				break;

			case DigMIC2_IN:
				WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE3_IDX], 2, RIGHT_DATSEL);                  /* 0b10 */
				break;

			case DigMIC1_IN:
				WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE3_IDX], 3, RIGHT_DATSEL);                  /* 0b11 */
				break;

			default:
				DPRINTF("\nassert in I2S_VIA_PCM_R_OUT default\n");
				assert(FALSE);
				break;
			}

			WRITE_SPECIFIC_BITS(reg[LEVANTE_EAR_SPKR_CTRL2_IDX], regField[I_OPA_HALF], I_OPA_HALF);

			if(_pmicId <= 0x41)
			{  /* LEVANTE_ID_A0 or LEVANTE_ID_A1 */
				WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE1_IDX], 0, I2S_OFFSR_Ax);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE1_IDX], 0, I2S_OFFSL_Ax);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE5_IDX], 0, I2S_OFFS_R);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE1_IDX], 0, I2S_OFFS_L);
			}

			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE2_IDX], EXACT_I2S, I2S_MODE);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE2_IDX], regField[I2S_WL], I2S_WL);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE2_IDX], regField[I2S_MASTER], I2S_MASTER);
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE2_IDX], I2S_FS_POL);
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE2_IDX], I2S_BCLK_POL);

			WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, I2S_EN);

			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE3_IDX], 1, I2S_MICDATS);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE3_IDX], regField[HIZ_I2S_DOUT], HIZ_I2S_DOUT);

			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE4_IDX], regField[RATE_DAC_I2S], RATE_DAC_I2S);          /* I2S rate set */

			if (mute == ACM_MUTE_ON)
			{
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], LOFI_RX_EN);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], EN_ADC_RIGHT);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], 1, LOFI_RX_EN);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], 1, EN_ADC_RIGHT);
			}
			break;

		case I2S_LOOP_EXT_OUT:
			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE2_IDX], regField[I2S_MASTER], I2S_MASTER);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE2_IDX], regField[I2S_EXT_LOOPBACK], I2S_EXT_LOOPBACK);

			CLEAR_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE3_IDX], I2S_MICDATS);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE3_IDX], regField[HIZ_I2S_DOUT], HIZ_I2S_DOUT);

			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE4_IDX], regField[RATE_DAC_I2S], RATE_DAC_I2S);          /* I2S rate set */
			break;

		case I2S_LOOP_INT_OUT:
			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE2_IDX], regField[I2S_MASTER], I2S_MASTER);

			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE3_IDX], regField[I2S_INT_LOOPBACK], I2S_INT_LOOPBACK);

			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE4_IDX], regField[RATE_DAC_I2S], RATE_DAC_I2S);          /* I2S rate set */

			/* Equalizer Settings */
			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE4_IDX], regField[EQU_BYP], EQU_BYP);

			if (regField[EQU_BYP] == 0)
			{          /* Equalizer is enabled */
				WRITE_SPECIFIC_BITS(reg[LEVANTE_EQUALIZER_N0_1_IDX], regField[EQ_N0_1], EQ_N0_1);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_EQUALIZER_N0_2_IDX], regField[EQ_N0_2], EQ_N0_2);

				WRITE_SPECIFIC_BITS(reg[LEVANTE_EQUALIZER_N1_1_IDX], regField[EQ_N1_1], EQ_N1_1);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_EQUALIZER_N1_2_IDX], regField[EQ_N1_2], EQ_N1_2);

				WRITE_SPECIFIC_BITS(reg[LEVANTE_EQUALIZER_D1_1_IDX], regField[EQ_D1_1], EQ_D1_1);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_EQUALIZER_D1_2_IDX], regField[EQ_D1_2], EQ_D1_2);
			}

			break;

		case PCM_LOOP_EXT_OUT:
			WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE3_IDX], regField[HIZ_DOUT], HIZ_DOUT);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE2_IDX], regField[PCM_MASTER], PCM_MASTER);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE2_IDX], regField[PCM_EXT_LOOPBACK], PCM_EXT_LOOPBACK);
			break;

		case PCM_LOOP_INT_OUT:
			WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE3_IDX], regField[PCM_INT_LOOPBACK], PCM_INT_LOOPBACK);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE2_IDX], regField[PCM_MASTER], PCM_MASTER);
			break;

		case EC_OUT:
			break;

		case I2S_L_Via_Ext_PA_LOOP_OUT:
			WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EPA_EN_R);

			if(_pmicId <= 0x41)
			{  /* LEVANTE_ID_A0 or LEVANTE_ID_A1 */
				WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE1_IDX], 0, I2S_OFFSR_Ax);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE1_IDX], 0, I2S_OFFSL_Ax);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE5_IDX], 0, I2S_OFFS_R);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE1_IDX], 0, I2S_OFFS_L);
			}

			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE2_IDX], EXACT_I2S, I2S_MODE);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE2_IDX], regField[I2S_WL], I2S_WL);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE2_IDX], regField[I2S_MASTER], I2S_MASTER);
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE2_IDX], I2S_FS_POL);
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE2_IDX], I2S_BCLK_POL);

			CLEAR_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE3_IDX], I2S_MICDATS);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE3_IDX], regField[HIZ_I2S_DOUT], HIZ_I2S_DOUT);

			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE4_IDX], regField[RATE_DAC_I2S], RATE_DAC_I2S);          /* I2S rate set */

			if (mute == ACM_MUTE_ON)
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], 1, MUTE_LEFT);
			}
			else
			{
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], MUTE_LEFT);
			}
			break;

		case I2S_R_Via_Ext_PA_LOOP_OUT:
			WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EPA_EN_L);

			if(_pmicId <= 0x41)
			{  /* LEVANTE_ID_A0 or LEVANTE_ID_A1 */
				WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE1_IDX], 0, I2S_OFFSR_Ax);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE1_IDX], 0, I2S_OFFSL_Ax);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE5_IDX], 0, I2S_OFFS_R);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE1_IDX], 0, I2S_OFFS_L);
			}

			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE2_IDX], EXACT_I2S, I2S_MODE);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE2_IDX], regField[I2S_WL], I2S_WL);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE2_IDX], regField[I2S_MASTER], I2S_MASTER);
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE2_IDX], I2S_FS_POL);
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE2_IDX], I2S_BCLK_POL);

			CLEAR_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE3_IDX], I2S_MICDATS);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE3_IDX], regField[HIZ_I2S_DOUT], HIZ_I2S_DOUT);

			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE4_IDX], regField[RATE_DAC_I2S], RATE_DAC_I2S);          /* I2S rate set */

			if (mute == ACM_MUTE_ON)
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], 1, MUTE_RIGHT);
			}
			else
			{
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], MUTE_RIGHT);
			}
			break;

		case I2S_BOTH_Via_Ext_PA_LOOP_OUT:
			WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EPA_EN_R);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EPA_EN_L);

			if(_pmicId <= 0x41)
			{  /* LEVANTE_ID_A0 or LEVANTE_ID_A1 */
				WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE1_IDX], 0, I2S_OFFSR_Ax);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE1_IDX], 0, I2S_OFFSL_Ax);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE5_IDX], 0, I2S_OFFS_R);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE1_IDX], 0, I2S_OFFS_L);
			}

			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE2_IDX], EXACT_I2S, I2S_MODE);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE2_IDX], regField[I2S_WL], I2S_WL);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE2_IDX], regField[I2S_MASTER], I2S_MASTER);
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE2_IDX], I2S_FS_POL);
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE2_IDX], I2S_BCLK_POL);

			CLEAR_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE3_IDX], I2S_MICDATS);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE3_IDX], regField[HIZ_I2S_DOUT], HIZ_I2S_DOUT);

			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE4_IDX], regField[RATE_DAC_I2S], RATE_DAC_I2S);          /* I2S rate set */

			if (mute == ACM_MUTE_ON)
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], 1, MUTE_LEFT);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], 1, MUTE_RIGHT);
			}
			else
			{
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], MUTE_LEFT);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], MUTE_RIGHT);
			}
			break;

		case HS_VIA_DAC1_OUT_SPKR_VIA_DAC2_OUT:
			/* SPKR_VIA_DAC2_OUT */
			WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ANALOG_PROGRAM4_IDX], regField[AUDIO_ZLOAD_4OHM], AUDIO_ZLOAD_4OHM);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_ANALOG_INPUT_SEL2_IDX], 1, SEL_SPEA);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_ANALOG_TO_ANALOG_IDX], 1, SEL_SP_EA);

			if(mute == ACM_MUTE_ON)
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 0, EN_MODULATOR);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 0, EN_DAC_RIGHT);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], 0, SPKR_PA_EN);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], 1, MUTE_RIGHT);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_MODULATOR);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_DAC_RIGHT);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], 1, SPKR_PA_EN);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_EAR_SPKR_CTRL1_IDX], 2, SPEA_IHOP1);  /* 0b10 */
				WRITE_SPECIFIC_BITS(reg[LEVANTE_EAR_SPKR_CTRL1_IDX], 1, SPKR_IBIAS3);  /* 0b01 */
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], 0, MUTE_RIGHT);
			}
			/* HS_VIA_DAC1_OUT */
			WRITE_SPECIFIC_BITS(reg[LEVANTE_ANALOG_INPUT_SEL1_IDX], 2, SEL_HS1);  /* 0b10 ==> Left  */
			WRITE_SPECIFIC_BITS(reg[LEVANTE_ANALOG_INPUT_SEL1_IDX], 2, SEL_HS2);  /* 0b01 ==> Left */
			WRITE_SPECIFIC_BITS(reg[LEVANTE_ANALOG_TO_ANALOG_IDX], 0, LO2_SEL_ANADIG_IN);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_ANALOG_TO_ANALOG_IDX], 0, HS2_SEL_ANADIG_IN);
			if(mute == ACM_MUTE_ON)
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 0, EN_MODULATOR);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 0, EN_DAC_RIGHT);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 0, EN_DAC_LEFT);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], 0, HS1_PA_EN);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], 0, HS2_PA_EN);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], 1, MUTE_LEFT);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], 1, MUTE_RIGHT);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_MODULATOR);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_DAC_RIGHT);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, EN_DAC_LEFT);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], 1, HS1_PA_EN);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_HS1_CTRL_IDX], 2, HS1_IOP1);  /* 0b10 */
				WRITE_SPECIFIC_BITS(reg[LEVANTE_HS1_CTRL_IDX], 3, HS1_IBIAS3);  /* 0b11 */
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES1_IDX], 1, HS2_PA_EN);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_HS2_CTRL_IDX], 2, HS2_IOP1);  /* 0b10 */
				WRITE_SPECIFIC_BITS(reg[LEVANTE_HS2_CTRL_IDX], 3, HS2_IBIAS3);  /* 0b11 */
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], 0, MUTE_LEFT);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_OFFSET_IDX], 0, MUTE_RIGHT);
			}
			break;

		default:
			break;
		}  /* switch(pathOutput) */

		switch (pathInput)
		{
		case AnaMIC1_IN:
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ANALOG_PROGRAM4_IDX], MIC1_MIC2_SEL);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ANALOG_PROGRAM4_IDX], regField[MBC1_2], MBC1_2);
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ANALOG_PROGRAM1_IDX], BYPASS_PGA_MIC1);

			WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_RATE_IDX], regField[RATE_ADC_PCM], RATE_ADC_PCM);          /* PCM rate set */

			WRITE_SPECIFIC_BITS(reg[LEVANTE_EAR_SPKR_CTRL2_IDX], regField[I_OPA_HALF], I_OPA_HALF);

			if (mute == ACM_MUTE_ON)
			{
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES1_IDX], EN_PGA_MIC1);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES1_IDX], EN_MOD1);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ANALOG_PROGRAM1_IDX], EN_MICBIAS1_2);
				/*make sure EN_MICBISAS2_PAD is set always for hook detection*/
				WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ANALOG_PROGRAM1_IDX], 3, EN_MICBIAS2_PAD);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ANALOG_PROGRAM1_IDX], EN_MICBIAS1_PAD);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES1_IDX], 1, EN_PGA_MIC1);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES1_IDX], 1, EN_MOD1);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ANALOG_PROGRAM1_IDX], 1, EN_MICBIAS1_2);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ANALOG_PROGRAM1_IDX], 3, EN_MICBIAS1_PAD);
				/*make sure EN_MICBISAS2_PAD is set always for hook detection*/
				WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ANALOG_PROGRAM1_IDX], 2, EN_MICBIAS2_PAD);
			}
			break;

		case AnaMIC2_IN:
			WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ANALOG_PROGRAM4_IDX], regField[MBC1_2], MBC1_2);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ANALOG_PROGRAM4_IDX], 1, MIC1_MIC2_SEL);
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ANALOG_PROGRAM1_IDX], BYPASS_PGA_MIC1);

			WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_RATE_IDX], regField[RATE_ADC_PCM], RATE_ADC_PCM);          /* PCM rate set */

			WRITE_SPECIFIC_BITS(reg[LEVANTE_EAR_SPKR_CTRL2_IDX], regField[I_OPA_HALF], I_OPA_HALF);

			if (mute == ACM_MUTE_ON)
			{
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES1_IDX], EN_PGA_MIC1);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ANALOG_PROGRAM1_IDX], EN_MICBIAS1_2);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ANALOG_PROGRAM1_IDX], EN_MICBIAS2_PAD);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES1_IDX], EN_MOD1);
				/*make sure EN_MICBISAS2_PAD is set always for hook detection*/
				WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ANALOG_PROGRAM1_IDX], 3, EN_MICBIAS2_PAD);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES1_IDX], 1, EN_PGA_MIC1);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ANALOG_PROGRAM1_IDX], 1, EN_MICBIAS1_2);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ANALOG_PROGRAM1_IDX], 3, EN_MICBIAS2_PAD);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES1_IDX], 1, EN_MOD1);
			}
			break;

		case AnaMIC3_IN:
			WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ANALOG_PROGRAM4_IDX], regField[MBC3], MBC3);
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ANALOG_PROGRAM1_IDX], BYPASS_PGA_MIC3);

			WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_RATE_IDX], regField[RATE_ADC_PCM], RATE_ADC_PCM);          /* PCM rate set */

			WRITE_SPECIFIC_BITS(reg[LEVANTE_EAR_SPKR_CTRL2_IDX], regField[I_OPA_HALF], I_OPA_HALF);

			if (mute == ACM_MUTE_ON)
			{
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES1_IDX], EN_PGA_MIC3);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES1_IDX], EN_MOD2);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ANALOG_PROGRAM1_IDX], EN_MICBIAS3);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES1_IDX], 1, EN_PGA_MIC3);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES1_IDX], 1, EN_MOD2);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ANALOG_PROGRAM1_IDX], 1, EN_MICBIAS3);
			}
			break;

		case DigMIC1_IN:
			if ((regField[RATE_ADC_PCM] + osrRateOffset) > 8 /* 0b1000 */)
				DPRINTF("\nassert in ((regField[RATE_ADC_PCM] + osrRateOffset) <= 8 /* 0b1000 */)\n");
			assert((regField[RATE_ADC_PCM] + osrRateOffset) <= 8 /* 0b1000 */);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_RATE_IDX], (regField[RATE_ADC_PCM] + osrRateOffset), RATE_ADC_PCM);          /* PCM rate set */

			WRITE_SPECIFIC_BITS(reg[LEVANTE_EAR_SPKR_CTRL2_IDX], regField[I_OPA_HALF], I_OPA_HALF);

			WRITE_SPECIFIC_BITS(reg[LEVANTE_DMIC_DELAY_IDX], regField[DMIC_CLOCK_SEL], DMIC_CLOCK_SEL);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_DMIC_DELAY_IDX], regField[DMIC_DATA_SEL], DMIC_DATA_SEL);
			break;

		case DigMIC2_IN:
			if ((regField[RATE_ADC_PCM] + osrRateOffset) > 8 /* 0b1000 */)
				DPRINTF("\nassert in ((regField[RATE_ADC_PCM] + osrRateOffset) <= 8 /* 0b1000 */)\n");
			assert((regField[RATE_ADC_PCM] + osrRateOffset) <= 8 /* 0b1000 */);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_RATE_IDX], (regField[RATE_ADC_PCM] + osrRateOffset), RATE_ADC_PCM);          /* PCM rate set */

			WRITE_SPECIFIC_BITS(reg[LEVANTE_EAR_SPKR_CTRL2_IDX], regField[I_OPA_HALF], I_OPA_HALF);

			WRITE_SPECIFIC_BITS(reg[LEVANTE_DMIC_DELAY_IDX], regField[DMIC_CLOCK_SEL], DMIC_CLOCK_SEL);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_DMIC_DELAY_IDX], regField[DMIC_DATA_SEL], DMIC_DATA_SEL);
			break;

		case AUX1_IN:
			WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_RATE_IDX], 8, RATE_ADC_PCM);          /* PCM rate set */

			WRITE_SPECIFIC_BITS(reg[LEVANTE_EAR_SPKR_CTRL2_IDX], regField[I_OPA_HALF], I_OPA_HALF);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_ANALOG_TO_ANALOG_IDX], 0, PGA_TO_PA_SEL1);

			if (mute == ACM_MUTE_ON)
			{
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES1_IDX], EN_PGA_AUX1);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES1_IDX], EN_MOD1);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES1_IDX], 1, EN_PGA_AUX1);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES1_IDX], 1, EN_MOD1);
			}

			/*added by Raul, mux2 select AUX1 */
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_ANALOG_TO_ANALOG_IDX], PGA_TO_PA_SEL1);
			break;

		case AUX2_IN:
			WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_RATE_IDX], 8, RATE_ADC_PCM);          /* PCM rate set */

			WRITE_SPECIFIC_BITS(reg[LEVANTE_EAR_SPKR_CTRL2_IDX], regField[I_OPA_HALF], I_OPA_HALF);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_ANALOG_TO_ANALOG_IDX], 0, PGA_TO_PA_SEL2);

			if (mute == ACM_MUTE_ON)
			{
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES1_IDX], EN_PGA_AUX2);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES1_IDX], EN_MOD2);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES1_IDX], 1, EN_PGA_AUX2);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES1_IDX], 1, EN_MOD2);
			}

			/*added by Raul, mux3 select AUX2 */
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_ANALOG_TO_ANALOG_IDX], PGA_TO_PA_SEL2);
			break;

		case AUX1_ANA_LOOP_IN:
			WRITE_SPECIFIC_BITS(reg[LEVANTE_ANALOG_TO_ANALOG_IDX], 1, PGA_TO_PA_SEL1);

			if (mute == ACM_MUTE_ON)
			{
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES1_IDX], EN_PGA_AUX1);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES1_IDX], 1, EN_PGA_AUX1);
			}
			break;

		case AUX2_ANA_LOOP_IN:
			WRITE_SPECIFIC_BITS(reg[LEVANTE_ANALOG_TO_ANALOG_IDX], 1, PGA_TO_PA_SEL2);

			if (mute == ACM_MUTE_ON)
			{
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES1_IDX], EN_PGA_AUX2);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES1_IDX], 1, EN_PGA_AUX2);
			}
			break;

		case SideTone_L_IN:
		case SideTone_R_IN:
		case SideTone_BOTH_IN:
			WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], 1, ST_EN);
			break;

		case PCM_L_IN:
			if(_pmicId <= 0x41)
			{  /* LEVANTE_ID_A0 or LEVANTE_ID_A1 */
				WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE1_IDX], 0, PCM_OFFSR_Ax);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE1_IDX], 0, PCM_OFFSL_Ax);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE4_IDX], 0, PCM_OFFS_R);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE1_IDX], 0, PCM_OFFS_L);
			}

			WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE2_IDX], 4, PCM_MODE);          /* 0b100 */
			WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE2_IDX], regField[PCM_WL], PCM_WL);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE2_IDX], regField[PCM_MASTER], PCM_MASTER);
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE2_IDX], PCM_FS_POL);
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE2_IDX], PCM_BCLK_POL);

			CLEAR_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE3_IDX], PCM_SEL_DATIN);

			WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], 1, PCM_EN);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], 1, LOFI_RX_EN);

			WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_RATE_IDX], regField[RATE_ADC_PCM], RATE_ADC_PCM);          /* PCM rate set */

			WRITE_SPECIFIC_BITS(reg[LEVANTE_EAR_SPKR_CTRL2_IDX], regField[I_OPA_HALF], I_OPA_HALF);
			break;

		case FM_VIA_SIDETONE_IN:

			WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], 1, LOFI_RX_EN);

			WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_RATE_IDX], 6, RATE_ADC_PCM);          /* PCM rate set, 32khz*/

			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE4_IDX], 6, RATE_DAC_I2S);          /* DAC rate set, 32khz*/

			WRITE_SPECIFIC_BITS(reg[LEVANTE_EAR_SPKR_CTRL2_IDX], regField[I_OPA_HALF], I_OPA_HALF);
			break;

		case PCM_R_IN:
			WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE2_IDX], 4, PCM_MODE);          /* 0b100 */
			WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE2_IDX], regField[PCM_WL], PCM_WL);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE2_IDX], regField[PCM_MASTER], PCM_MASTER);
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE2_IDX], PCM_FS_POL);
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE2_IDX], PCM_BCLK_POL);

			WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE3_IDX], 1, PCM_SEL_DATIN);

			WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], 1, PCM_EN);

			WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_RATE_IDX], regField[RATE_ADC_PCM], RATE_ADC_PCM);          /* PCM rate set */

			WRITE_SPECIFIC_BITS(reg[LEVANTE_EAR_SPKR_CTRL2_IDX], regField[I_OPA_HALF], I_OPA_HALF);
			break;

		case I2S_IN:
			if(_pmicId <= 0x41)
			{  /* LEVANTE_ID_A0 or LEVANTE_ID_A1 */
				WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE1_IDX], 0, I2S_OFFSR_Ax);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE1_IDX], 0, I2S_OFFSL_Ax);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE5_IDX], 0, I2S_OFFS_R);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE1_IDX], 0, I2S_OFFS_L);
			}
			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE2_IDX], EXACT_I2S, I2S_MODE);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE2_IDX], regField[I2S_WL], I2S_WL);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE2_IDX], regField[I2S_MASTER], I2S_MASTER);

			CLEAR_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE2_IDX], I2S_FS_POL);
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE2_IDX], I2S_BCLK_POL);

			WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, I2S_EN);

			CLEAR_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE3_IDX], I2S_IN2SEL);

			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE4_IDX], regField[RATE_DAC_I2S], RATE_DAC_I2S);          /* I2S rate set */

			/* Equalizer Settings */
			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE4_IDX], regField[EQU_BYP], EQU_BYP);

			if (regField[EQU_BYP] == 0)
			{          /* Equalizer is enabled */
				WRITE_SPECIFIC_BITS(reg[LEVANTE_EQUALIZER_N0_1_IDX], regField[EQ_N0_1], EQ_N0_1);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_EQUALIZER_N0_2_IDX], regField[EQ_N0_2], EQ_N0_2);

				WRITE_SPECIFIC_BITS(reg[LEVANTE_EQUALIZER_N1_1_IDX], regField[EQ_N1_1], EQ_N1_1);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_EQUALIZER_N1_2_IDX], regField[EQ_N1_2], EQ_N1_2);

				WRITE_SPECIFIC_BITS(reg[LEVANTE_EQUALIZER_D1_1_IDX], regField[EQ_D1_1], EQ_D1_1);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_EQUALIZER_D1_2_IDX], regField[EQ_D1_2], EQ_D1_2);
			}
			break;

		case I2S1_IN:
			if(_pmicId <= 0x41)
			{  /* LEVANTE_ID_A0 or LEVANTE_ID_A1 */
				WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE1_IDX], 0, I2S_OFFSR_Ax);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE1_IDX], 0, I2S_OFFSL_Ax);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE5_IDX], 0, I2S_OFFS_R);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE1_IDX], 0, I2S_OFFS_L);
			}

			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE2_IDX], EXACT_I2S, I2S_MODE);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE2_IDX], regField[I2S_WL], I2S_WL);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE2_IDX], regField[I2S_MASTER], I2S_MASTER);
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE2_IDX], I2S_FS_POL);
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE2_IDX], I2S_BCLK_POL);

			WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, I2S_EN);

			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE3_IDX], 1, I2S_IN2SEL);

			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE4_IDX], regField[RATE_DAC_I2S], RATE_DAC_I2S);          /* I2S rate set */

			/* Equalizer Settings */
			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE4_IDX], regField[EQU_BYP], EQU_BYP);

			if (regField[EQU_BYP] == 0)
			{          /* Equalizer is enabled */
				WRITE_SPECIFIC_BITS(reg[LEVANTE_EQUALIZER_N0_1_IDX], regField[EQ_N0_1], EQ_N0_1);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_EQUALIZER_N0_2_IDX], regField[EQ_N0_2], EQ_N0_2);

				WRITE_SPECIFIC_BITS(reg[LEVANTE_EQUALIZER_N1_1_IDX], regField[EQ_N1_1], EQ_N1_1);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_EQUALIZER_N1_2_IDX], regField[EQ_N1_2], EQ_N1_2);

				WRITE_SPECIFIC_BITS(reg[LEVANTE_EQUALIZER_D1_1_IDX], regField[EQ_D1_1], EQ_D1_1);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_EQUALIZER_D1_2_IDX], regField[EQ_D1_2], EQ_D1_2);
			}
			break;

		case I2S_LOOP_EXT_IN:
			if(_pmicId <= 0x41)
			{  /* LEVANTE_ID_A0 or LEVANTE_ID_A1 */
				WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE1_IDX], 0, I2S_OFFSR_Ax);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE1_IDX], 0, I2S_OFFSL_Ax);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE5_IDX], 0, I2S_OFFS_R);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE1_IDX], 0, I2S_OFFS_L);
			}

			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE2_IDX], EXACT_I2S, I2S_MODE);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE2_IDX], regField[I2S_WL], I2S_WL);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE2_IDX], regField[I2S_MASTER], I2S_MASTER);
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE2_IDX], I2S_FS_POL);
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE2_IDX], I2S_BCLK_POL);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE2_IDX], 1, I2S_EXT_LOOPBACK);

			WRITE_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], 1, I2S_EN);

			CLEAR_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE3_IDX], I2S_IN2SEL);
			break;

		case I2S_LOOP_INT_IN:
			WRITE_SPECIFIC_BITS(reg[LEVANTE_I2S_INTERFACE2_IDX], regField[I2S_MASTER], I2S_MASTER);
			break;

		case PCM_LOOP_EXT_IN:
			WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE2_IDX], 4, PCM_MODE);          /* 0b100 */
			WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE2_IDX], regField[PCM_WL], PCM_WL);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE2_IDX], regField[PCM_MASTER], PCM_MASTER);
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE2_IDX], PCM_FS_POL);
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE2_IDX], PCM_BCLK_POL);
			WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE2_IDX], 1, PCM_EXT_LOOPBACK);          /* 0b100 */

			WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], 1, PCM_EN);

			WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_RATE_IDX], regField[RATE_ADC_PCM], RATE_ADC_PCM);          /* PCM rate set */

			WRITE_SPECIFIC_BITS(reg[LEVANTE_EAR_SPKR_CTRL2_IDX], regField[I_OPA_HALF], I_OPA_HALF);
			break;

		case PCM_LOOP_INT_IN:
			WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_RATE_IDX], regField[RATE_ADC_PCM], RATE_ADC_PCM);          /* PCM rate set */

			WRITE_SPECIFIC_BITS(reg[LEVANTE_EAR_SPKR_CTRL2_IDX], regField[I_OPA_HALF], I_OPA_HALF);

			WRITE_SPECIFIC_BITS(reg[LEVANTE_PCM_INTERFACE2_IDX], regField[PCM_MASTER], PCM_MASTER);
			break;

		case EC_DAC1_IN:
			WRITE_SPECIFIC_BITS(reg[LEVANTE_ECHO_CANCEL_PATH_IDX], 1, EC_DATSEL);

			if (mute == ACM_MUTE_ON)
			{
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], EC_SELECT);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], LOFI_RX_EN);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], EN_ADC_LEFT);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], 1, EC_SELECT);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], 1, LOFI_RX_EN);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], 1, EN_ADC_LEFT);
			}
			break;

		case EC_DAC2_IN:
			CLEAR_SPECIFIC_BITS(reg[LEVANTE_ECHO_CANCEL_PATH_IDX], EC_DATSEL);

			if (mute == ACM_MUTE_ON)
			{
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], EC_SELECT);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], LOFI_RX_EN);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], EN_ADC_LEFT);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], 1, EC_SELECT);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], 1, LOFI_RX_EN);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], 1, EN_ADC_LEFT);
			}
			break;

		case EC_DAC1_DAC2_IN:
			WRITE_SPECIFIC_BITS(reg[LEVANTE_ECHO_CANCEL_PATH_IDX], 2, EC_DATSEL);          /* 0b10 */

			if (mute == ACM_MUTE_ON)
			{
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], EC_SELECT);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], LOFI_RX_EN);
				CLEAR_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], EN_ADC_LEFT);
			}
			else
			{
				WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], 1, EC_SELECT);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], 1, LOFI_RX_EN);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_ADC_ENABLES2_IDX], 1, EN_ADC_LEFT);
			}
			break;

		default:
			break;
		}  /* switch(pathInput) */

		/* Handle HIFI Gains / Mute first, use local slope/offset to compute reg value*/
		for (i = 1; i < LEVANTE_TOTAL_GAINS; i++)  /* start from index "1" - skip digital gain */
		{
			totalPathInGain = gainConverter[i].totalPaths;
			for (j = 0; j < totalPathInGain; j++)
			{
				if (path == gainConverter[i].gainPath[j].path && !LevanteIsVoicePath(path))
				{
					if (mute == ACM_MUTE_OFF)
					{
						gain = LEVANTE_GAIN_CALC(volume, gainConverter[i].gainPath[j].pga);

						/* This path will decide regarding the charge-pump voltage (0.9 V or 1.8 V) */
						//if( (path == LEVANTE_PCM_L_TO_HS) || (path == LEVANTE_I2S_TO_HS) )
						if(path == LEVANTE_I2S_TO_HS) /*different from PTK, TD separate voice path and hifi path*/
						{
							if(gainConverter[i].gainPath == gLevantePgaHs1Gain)
								gainHS1 = gain;         /* Gain ID 3 */
							else if(gainConverter[i].gainPath ==gLevantePgaHs2Gain)
								gainHS2 = gain;         /* Gain ID 4 */
							else if(gainConverter[i].gainPath == gLevanteDpga1LofiGainLeftGain)
								gainLofiLeft = gain;   /* Gain ID 7 */
							else if(gainConverter[i].gainPath == gLevanteDpga1HifilGainLeftGain)
								gainHifilLeft = gain;   /* Gain ID 8 */
							else if(gainConverter[i].gainPath == gLevanteDpga2LofiGainRightGain)
								gainLofiRight = gain;  /* Gain ID 10 */
							else if(gainConverter[i].gainPath == gLevanteDpga2HifirGainRightGain)
								gainHifirRight = gain;  /* Gain ID 12 */
						}  /* End of charge-pump voltage part */

						if (gLevanteGainTableTranslate[gainConverter[i].gainPath[j].gainTableId][0] != TRANSPARENT)
							gain = gLevanteGainTableTranslate[gainConverter[i].gainPath[j].gainTableId][gain];
					}
					else
					{	/* handle mute */
						if (gainConverter[i].gainPath[j].gainTableId == 0)
							gain = 0;
						else if (gainConverter[i].gainPath[j].gainTableId == 1)
							gain = 63;
						else
							gain = 0;
					}

					WRITE_GAIN_BITS(reg[gainConverter[i].gainPath[j].regIndex],
							gain,
							gainConverter[i].gainPath[j].gainMask,
							gainConverter[i].gainPath[j].gainShift);
				}	/* if */
			}		/* for */
		}			/* for */

		/* Then handle Voice call path Gains / Mute, use shadowReg as reg value*/
		for (i = 1; i < LEVANTE_TOTAL_GAINS; i++)  /* start from index "1" - skip digital gain */
		{
			totalPathInGain = gainConverter[i].totalPaths;
			for (j = 0; j < totalPathInGain; j++)
			{
				if (path == gainConverter[i].gainPath[j].path && LevanteIsVoicePath(path))
				{
					if (!LevanteGetLoopbackTestStatus())
					{
						/*If it is a voice call path then we should use the shadow value for it since we possibly have update the gains by
						calling LevanteSetPathsAnalogGain*/
						printf("path %d, reg 0x%x use shadow gain 0x%x\n", path,
							registerInfo[gainConverter[i].gainPath[j].regIndex].address,
							shadowReg[gainConverter[i].gainPath[j].regIndex]);
						reg[gainConverter[i].gainPath[j].regIndex] = shadowReg[gainConverter[i].gainPath[j].regIndex];

						isVoicePathUse = TRUE;
						CLEAR_SPECIFIC_BITS(reg[LEVANTE_AUDIO_SUPPLIES1_IDX], CPUMP_SEL);
						CLEAR_SPECIFIC_BITS(reg[LEVANTE_AUDIO_SUPPLIES1_IDX], CPUMP_PWR_SEL);

					}
					else /*loopback test*/
					{
						gain = LEVANTE_GAIN_CALC(volume, gainConverter[i].gainPath[j].pga);

						if (gLevanteGainTableTranslate[gainConverter[i].gainPath[j].gainTableId][0] != TRANSPARENT)
							gain = gLevanteGainTableTranslate[gainConverter[i].gainPath[j].gainTableId][gain];
						WRITE_GAIN_BITS(reg[gainConverter[i].gainPath[j].regIndex],
								gain,
								gainConverter[i].gainPath[j].gainMask,
								gainConverter[i].gainPath[j].gainShift);
					}
				}	/* if */
			}		/* for */
		}			/* for */
		/* Handle gain for Force Speaker (headset + loud-speaker) */
		if(path == LEVANTE_I2S_TO_SPKR_HS)
			isForceSpeakerOn = TRUE;
	}                               /* for */
	/* Handle gain for Force Speaker (headset + loud-speaker); in case that this path is active, use it's gains */
	if(isForceSpeakerOn)
	{
		/* Gain IDs 9+10 (headset) */
		WRITE_SPECIFIC_BITS(reg[LEVANTE_LEFT_GAIN1_IDX],  regField[DPGA1_HIFIL_HIFIR_GAIN_LEFT], HIFIL_GAIN_LEFT);
		WRITE_SPECIFIC_BITS(reg[LEVANTE_RIGHT_GAIN1_IDX], regField[DPGA1_HIFIL_HIFIR_GAIN_LEFT], HIFIR_GAIN_LEFT);

		/* Gain IDs 12+13 (loud-speaker) */
		WRITE_SPECIFIC_BITS(reg[LEVANTE_LEFT_GAIN2_IDX],   regField[DPGA2_HIFIL_HIFIR_GAIN_RIGHT], HIFIL_GAIN_RIGHT);
		WRITE_SPECIFIC_BITS(reg[LEVANTE_RIGHT_GAIN2_IDX],  regField[DPGA2_HIFIL_HIFIR_GAIN_RIGHT], HIFIR_GAIN_RIGHT);
	}

	if(!isVoicePathUse)
	{
		/* Set the charge-pump voltage (0.9 V or 1.8 V) */
		isHighVoltageUse = FALSE;
		if( (gainHS1 != INVALID_INDEX) && (gainHifirRight != INVALID_INDEX) )
		{
			ASSERT(gainHS1 <= 7);  /* Index must be between 0 and 7 */

			isFmHeadsetEnabled = TRUE;
			if(gainHifirRight < gainOfTableIdOne[gainHS1])
			{  /* Left gain sum is higher then -7.25 dB ==> use 1.8 V */
				isHighVoltageUse = TRUE;
				WRITE_SPECIFIC_BITS(reg[LEVANTE_AUDIO_SUPPLIES1_IDX], 1, CPUMP_SEL);
				WRITE_SPECIFIC_BITS(reg[LEVANTE_AUDIO_SUPPLIES1_IDX], 1, CPUMP_PWR_SEL);
			}
		}

		if( (READ_SPECIFIC_BITS(reg[LEVANTE_AUDIO_SUPPLIES1_IDX], CPUMP_SEL) == 0) &&
			(READ_SPECIFIC_BITS(reg[LEVANTE_AUDIO_SUPPLIES1_IDX], CPUMP_PWR_SEL) == 0) )
		{  /* Previous gain is lower then -7.25 dB ==> check the next gain */
			if( (gainHS2 != INVALID_INDEX) && (gainHifilLeft != INVALID_INDEX) )
			{
				ASSERT(gainHS2 <= 7);  /* Index must be between 0 and 7 */

				isFmHeadsetEnabled = TRUE;
				if(gainHifilLeft < gainOfTableIdOne[gainHS2])
				{  /* At least Right or Left gain sum is higher then -7.25 dB ==> use 1.8 V */
					isHighVoltageUse = TRUE;
					WRITE_SPECIFIC_BITS(reg[LEVANTE_AUDIO_SUPPLIES1_IDX], 1, CPUMP_SEL);
					WRITE_SPECIFIC_BITS(reg[LEVANTE_AUDIO_SUPPLIES1_IDX], 1, CPUMP_PWR_SEL);
				}
			}
		}

		if( (READ_SPECIFIC_BITS(reg[LEVANTE_AUDIO_SUPPLIES1_IDX], CPUMP_SEL) == 0) &&
			(READ_SPECIFIC_BITS(reg[LEVANTE_AUDIO_SUPPLIES1_IDX], CPUMP_PWR_SEL) == 0) )
		{  /* Previous gain is lower then -7.25 dB ==> check the next gain */
			if( (gainHS1 != INVALID_INDEX) && (gainLofiRight != INVALID_INDEX) )
			{
				ASSERT(gainHS1 <= 7);  /* Index must be between 0 and 7 */

				if(gainLofiRight < gainOfTableIdOne[gainHS1])
				{  /* At least Right or Left gain sum is higher then -7.25 dB ==> use 1.8 V */
					isHighVoltageUse = TRUE;
					WRITE_SPECIFIC_BITS(reg[LEVANTE_AUDIO_SUPPLIES1_IDX], 1, CPUMP_SEL);
					WRITE_SPECIFIC_BITS(reg[LEVANTE_AUDIO_SUPPLIES1_IDX], 1, CPUMP_PWR_SEL);
				}
			}
		}

		if( (READ_SPECIFIC_BITS(reg[LEVANTE_AUDIO_SUPPLIES1_IDX], CPUMP_SEL) == 0) &&
			(READ_SPECIFIC_BITS(reg[LEVANTE_AUDIO_SUPPLIES1_IDX], CPUMP_PWR_SEL) == 0) )
		{  /* Previous gain is lower then -7.25 dB ==> check the next gain */
			if( (gainHS2 != INVALID_INDEX) && (gainLofiLeft != INVALID_INDEX) )
			{
				ASSERT(gainHS2 <= 7);  /* Index must be between 0 and 7 */

				if(gainLofiLeft < gainOfTableIdOne[gainHS2])
				{  /* At least Right or Left gain sum is higher then -7.25 dB ==> use 1.8 V */
					isHighVoltageUse = TRUE;
					WRITE_SPECIFIC_BITS(reg[LEVANTE_AUDIO_SUPPLIES1_IDX], 1, CPUMP_SEL);
					WRITE_SPECIFIC_BITS(reg[LEVANTE_AUDIO_SUPPLIES1_IDX], 1, CPUMP_PWR_SEL);
				}
			}
		}
	}

	/* Set the churge-pump clock divider */
	if( (isFmHeadsetEnabled) && (_fmRadioFrequency != 0))
	{
		i = 0;

		while(fmFreqToClkDiv[i].fmFrequency != 0)
		{
			if(_fmRadioFrequency == fmFreqToClkDiv[i].fmFrequency)
			{
				if(isHighVoltageUse)
				{
					printf("FM_Radio: freq= %d, idx= %d; set 0x41 to 0x%x\n", _fmRadioFrequency, i, fmFreqToClkDiv[i].cpClkDivider_1_8);
					WRITE_SPECIFIC_BITS(reg[LEVANTE_MCLK_IDX], fmFreqToClkDiv[i].cpClkDivider_1_8, MCLK_ALL);
				}
				else
				{
					printf("FM_Radio: freq= %d, idx= %d; set 0x41 to 0x%x\n", _fmRadioFrequency, i, fmFreqToClkDiv[i].cpClkDivider_0_9);
					WRITE_SPECIFIC_BITS(reg[LEVANTE_MCLK_IDX], fmFreqToClkDiv[i].cpClkDivider_0_9, MCLK_ALL);
				}

				break;
			}

			i++;
		}
	}
	/* End of charge-pump voltage part */

	static int temp = 0;
	temp++;

	/*FIXME: Raul: workaround for Levante A1, need to set sense switch on for Mic2 to enable headset (Mic2 workaround) detection*/
	if (_pmicId <= 0x41)
		reg[LEVANTE_ADC_ANALOG_PROGRAM1_IDX] |= LEVANTE_ADC_ANALOG_PROGRAM1_DEFAULT_VALUE;

	/* Detect the needed sequence: Enable, Disable or Modify */
	if( ( (READ_SPECIFIC_BITS(shadowReg[LEVANTE_DAC_ENABLES2_IDX], EN_DAC_RIGHT) == 0) &&
	      (READ_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_DAC_RIGHT) == 1) )     ||
	    ( (READ_SPECIFIC_BITS(shadowReg[LEVANTE_DAC_ENABLES2_IDX], EN_DAC_LEFT) == 0)  &&
	      (READ_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_DAC_LEFT) == 1) ) )
	{  /* DAC Enable */
		printf("Levante Update Sequence: DAC Enable \n");
		groupOrderIdx = 0;
	}
	else if( ( (READ_SPECIFIC_BITS(shadowReg[LEVANTE_DAC_ENABLES2_IDX], EN_DAC_RIGHT) == 1) &&
	           (READ_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_DAC_RIGHT) == 0) )     ||
	         ( (READ_SPECIFIC_BITS(shadowReg[LEVANTE_DAC_ENABLES2_IDX], EN_DAC_LEFT) == 1) &&
	           (READ_SPECIFIC_BITS(reg[LEVANTE_DAC_ENABLES2_IDX], EN_DAC_LEFT) == 0) ) )
	{  /* DAC Disable */
		printf("Levante Update Sequence: DAC Disable \n");
		groupOrderIdx = 1;
	}
	else
	{  /* Modify */
		printf("Levante Update Sequence: Modify \n");
		groupOrderIdx = 2;
	}

	/* Update changed registers */
	i = 0;
	while(registerInfoGroupData[i].registerInfo != NULL)
	{
		isChanged = FALSE;
		groupIdx = groupOrder[groupOrderIdx][i];  /* Use the Group Order according to the activity - Enable, Disable or Modify */

		j = 0;
		//ramp up/down the digital gain to anti-pop, do this only when enabling path
		if (groupOrderIdx == 0 && groupIdx == 4)//registerInfoGroup_4
		{
			BOOL rampChangeFailed = FALSE;
			BOOL registerNeedChange[sizeof(registerInfoGroup_4)/sizeof(registerInfoGroup_4[0])];
			unsigned char orinalReg = 0;
			unsigned char currentReg = 0;
			unsigned char targetReg = 0;

			while( (registerInfoGroupData[groupIdx].registerInfo[j].index        != LEVANTE_NO_IDX)     &&
			       (registerInfoGroupData[groupIdx].registerInfo[j].address      != LEVANTE_NO_ADDRESS) &&
			       (registerInfoGroupData[groupIdx].registerInfo[j].defaultValue != LEVANTE_NO_VALUE) )
			{
				if (rampChangeFailed)
				{
					printf("## ramp up/down digital gain failed due to the unmached gain setting! ##\n");
					break;
				}

				registerNeedChange[j] = FALSE;

				if(shadowReg[registerInfoGroupData[groupIdx].registerInfo[j].index] != reg[registerInfoGroupData[groupIdx].registerInfo[j].index])
				{
					registerNeedChange[j] = TRUE;

					printf("try to ramp up/down: reg 0x%x is 0x%x, shadowReg 0x%x is 0x%x\n",
						registerInfo[registerInfoGroupData[groupIdx].registerInfo[j].index].address,
						reg[registerInfoGroupData[groupIdx].registerInfo[j].index],
						registerInfo[registerInfoGroupData[groupIdx].registerInfo[j].index].address,
						shadowReg[registerInfoGroupData[groupIdx].registerInfo[j].index]);

					if (orinalReg == 0)
					{
						orinalReg = shadowReg[registerInfoGroupData[groupIdx].registerInfo[j].index];
						targetReg = reg[registerInfoGroupData[groupIdx].registerInfo[j].index];
						shadowReg[registerInfoGroupData[groupIdx].registerInfo[j].index] = reg[registerInfoGroupData[groupIdx].registerInfo[j].index];
					}
					else if ((orinalReg == shadowReg[registerInfoGroupData[groupIdx].registerInfo[j].index])
						&& (targetReg == reg[registerInfoGroupData[groupIdx].registerInfo[j].index]))
					{
						shadowReg[registerInfoGroupData[groupIdx].registerInfo[j].index] = reg[registerInfoGroupData[groupIdx].registerInfo[j].index];
					}
					else
						rampChangeFailed = TRUE;

				}

				j++;
			}

			int actualSteps = (orinalReg < targetReg ? targetReg - orinalReg : orinalReg - targetReg);
			float adjustment;
			if (actualSteps <= VOLUME_RAMP_STEPS)
				adjustment = 1;
			else
				adjustment = (float)actualSteps/(float)VOLUME_RAMP_STEPS;
			if (orinalReg != targetReg)
				printf("ramp step is %f\n", adjustment);

			currentReg = orinalReg;

			for (k = 0; currentReg < targetReg; k++)//ramp down
			{
				currentReg = orinalReg + (int)(adjustment * (k + 1));
				if (currentReg > targetReg)
					currentReg = targetReg;

				for (j = 0; (registerInfoGroupData[groupIdx].registerInfo[j].index	!= LEVANTE_NO_IDX)     &&
				       (registerInfoGroupData[groupIdx].registerInfo[j].address		!= LEVANTE_NO_ADDRESS) &&
				       (registerInfoGroupData[groupIdx].registerInfo[j].defaultValue	!= LEVANTE_NO_VALUE); j++)
				{
					if (registerNeedChange[j])
					{
						LevanteRegSet(registerInfoGroupData[groupIdx].registerInfo[j].address, currentReg);
					}
				}
				//apply changes
				LevanteRegSet(LEVANTE_EAR_SPKR_CTRL2_ADDRESS, (shadowReg[LEVANTE_EAR_SPKR_CTRL2_IDX] & (~(1 << APPLY_CHANGES_SHIFT))));
				LevanteRegSet(LEVANTE_EAR_SPKR_CTRL2_ADDRESS, (shadowReg[LEVANTE_EAR_SPKR_CTRL2_IDX] | (1 << APPLY_CHANGES_SHIFT)));
			}

			for (k = 0; currentReg > targetReg; k++)//ramp up
			{
				currentReg = orinalReg - (int)(adjustment * (k + 1));
				if (currentReg < targetReg)
					currentReg = targetReg;

				for (j = 0; (registerInfoGroupData[groupIdx].registerInfo[j].index	!= LEVANTE_NO_IDX)     &&
				       (registerInfoGroupData[groupIdx].registerInfo[j].address		!= LEVANTE_NO_ADDRESS) &&
				       (registerInfoGroupData[groupIdx].registerInfo[j].defaultValue	!= LEVANTE_NO_VALUE); j++)
				{
					if (registerNeedChange[j])
					{
						LevanteRegSet(registerInfoGroupData[groupIdx].registerInfo[j].address, currentReg);
					}
				}
				//apply changes
				LevanteRegSet(LEVANTE_EAR_SPKR_CTRL2_ADDRESS, (shadowReg[LEVANTE_EAR_SPKR_CTRL2_IDX] & (~(1 << APPLY_CHANGES_SHIFT))));
				LevanteRegSet(LEVANTE_EAR_SPKR_CTRL2_ADDRESS, (shadowReg[LEVANTE_EAR_SPKR_CTRL2_IDX] | (1 << APPLY_CHANGES_SHIFT)));
			}

			if (rampChangeFailed)
			{
				j = 0;
				while( (registerInfoGroupData[groupIdx].registerInfo[j].index        != LEVANTE_NO_IDX)     &&
				       (registerInfoGroupData[groupIdx].registerInfo[j].address      != LEVANTE_NO_ADDRESS) &&
				       (registerInfoGroupData[groupIdx].registerInfo[j].defaultValue != LEVANTE_NO_VALUE) )
				{
					if(shadowReg[registerInfoGroupData[groupIdx].registerInfo[j].index] != reg[registerInfoGroupData[groupIdx].registerInfo[j].index])
					{  /* There was a change ==> update the register */
						isChanged = TRUE;
						printf("ramp up/down failed: reg 0x%x is 0x%x, shadowReg 0x%x is 0x%x\n",
							registerInfo[registerInfoGroupData[groupIdx].registerInfo[j].index].address,
							reg[registerInfoGroupData[groupIdx].registerInfo[j].index],
							registerInfo[registerInfoGroupData[groupIdx].registerInfo[j].index].address,
							shadowReg[registerInfoGroupData[groupIdx].registerInfo[j].index]);
						LevanteRegSet(registerInfoGroupData[groupIdx].registerInfo[j].address, reg[registerInfoGroupData[groupIdx].registerInfo[j].index]);
						shadowReg[registerInfoGroupData[groupIdx].registerInfo[j].index] = reg[registerInfoGroupData[groupIdx].registerInfo[j].index];
					}

					j++;
				}
			}
		}
		else
		{
			while( (registerInfoGroupData[groupIdx].registerInfo[j].index        != LEVANTE_NO_IDX)     &&
			       (registerInfoGroupData[groupIdx].registerInfo[j].address      != LEVANTE_NO_ADDRESS) &&
			       (registerInfoGroupData[groupIdx].registerInfo[j].defaultValue != LEVANTE_NO_VALUE) )
			{
				if(shadowReg[registerInfoGroupData[groupIdx].registerInfo[j].index] != reg[registerInfoGroupData[groupIdx].registerInfo[j].index])
				{  /* There was a change ==> update the register */
					isChanged = TRUE;
					printf("-- %d: reg 0x%x is 0x%x, shadowReg 0x%x is 0x%x\n", temp,
						registerInfo[registerInfoGroupData[groupIdx].registerInfo[j].index].address,
						reg[registerInfoGroupData[groupIdx].registerInfo[j].index],
						registerInfo[registerInfoGroupData[groupIdx].registerInfo[j].index].address,
						shadowReg[registerInfoGroupData[groupIdx].registerInfo[j].index]);
					LevanteRegSet(registerInfoGroupData[groupIdx].registerInfo[j].address, reg[registerInfoGroupData[groupIdx].registerInfo[j].index]);
					shadowReg[registerInfoGroupData[groupIdx].registerInfo[j].index] = reg[registerInfoGroupData[groupIdx].registerInfo[j].index];
				}

				j++;
			}
		}

		if( isChanged && registerInfoGroupData[groupIdx].isRsyncNeeded )
		{
			/*Raul: ASOC will remember the value, so first clear and then set to ensure write the register successfully*/
			LevanteRegSet(LEVANTE_EAR_SPKR_CTRL2_ADDRESS, (shadowReg[LEVANTE_EAR_SPKR_CTRL2_IDX] & (~(1 << APPLY_CHANGES_SHIFT))));
			LevanteRegSet(LEVANTE_EAR_SPKR_CTRL2_ADDRESS, (shadowReg[LEVANTE_EAR_SPKR_CTRL2_IDX] | (1 << APPLY_CHANGES_SHIFT)));
		}

		i++;
	}
} /* End of LevanteUpdateHardware */


/*******************************************************************************
* Function:	LevanteReadGainPathFromFdi
********************************************************************************
* Description:	If openning of FDI file is successful, reading the calibration
*		gains information from it.
*
* Parameters:	FILE_ID *fdiID_p - the GainPath file ID.
*
* Return value: LevanteFdiReadFileStatus - the status of the file read.
*
* Notes:
********************************************************************************/
static LevanteFdiReadFileStatus LevanteReadGainPathFromFdi(FILE_ID *fdiID_p)
{
	unsigned short				temp16bit;
	unsigned long				temp32bit;
	unsigned long				path;
	unsigned long				gainId;
	signed short				slope;
	signed short				offset;
	LevanteFdiReadFileStatus	AllOK;
	NVM_Header_ts				audioNvmHeader;
	int							i;
	LevanteGains 	    *gainU;

	/* reading the header */
	if(FDI_fread(&audioNvmHeader, sizeof(audioNvmHeader), 1, (FILE *)(*fdiID_p)) != 1)
	{
		return LEVANTE_FAIL;
	}
	printf("\nread HIFI calibration nvm file\n");
	printf("gainId          path          slope          offset\n");
	/* Read till the End-Of-File */
	while(1)
	{
		AllOK = LEVANTE_OK;
		READ_PARAM_FROM_FDI_AND_TEST(gainId, temp32bit, *fdiID_p);
		READ_PARAM_FROM_FDI_AND_TEST(path,   temp32bit, *fdiID_p);
		READ_PARAM_FROM_FDI_AND_TEST(slope,  temp16bit, *fdiID_p);
		READ_PARAM_FROM_FDI_AND_TEST(offset, temp16bit, *fdiID_p);
		printf("0x%08x      0x%08x    0x%04hx         0x%04hx\n", gainId, path, slope, offset);
		/* check for errors */
		if( (path < LEVANTE_TOTAL_VALID_PATHS) && (gainId < LEVANTE_TOTAL_GAINS) )  /* start from index "2" - skip two digital gains; also, skip the last gain (dummy gain) */
		{
			gainU = gainConverter[gainId].gainPath;
			for(i=0; i < gainConverter[gainId].totalPaths; i++)
			{
				if(path == (*gainU).path)
				{
					(*gainU).pga.slope  = slope;
					(*gainU).pga.offset = offset;
					break;
				}

				/* move to next path in current gain list */
				gainU = (LevanteGains *)(((char *)gainU) + sizeof(LevanteGains));
			}
		}
		else
		{
			return LEVANTE_FAIL;
		}
	}  /* end of while(1) */

	return AllOK;
} /* End of LevanteReadGainPathFromFdi */


/*******************************************************************************
* Function:	LevanteReadFdiData
********************************************************************************
* Description: First loading default values for the gains {slope,offset}
*			   of each known path .
*			   Then if openning of FDI file is successful, reading the calibration
*	       gains information from it.
* Parameters:
*
* Return value:
*
* Notes:
********************************************************************************/
static void LevanteReadFdiData(void)
{
	BOOL			    fdiRetCode;
	FILE_ID						fdiID;
	LevanteFdiReadFileStatus	readOK = LEVANTE_NO_FILE;

	fdiID = FDI_fopen(LEVANTE_AP_AUDIO_FDI_FILENAME, "rb");
	if(fdiID)
	{
		readOK = LevanteReadGainPathFromFdi((FILE_ID *)&fdiID);
		fdiRetCode = FDI_fclose(fdiID);
		if (fdiRetCode != FALSE)
			DPRINTF("\nassert in (fdiRetCode != FALSE)\n");
		assert(fdiRetCode == FALSE);

		//DIAG_FILTER(AUDIO, LEVANTE, AUDIO_LEVANTE_ReadFdiDataOK, DIAG_ALL)
	}
} /* End of LevanteReadFdiData */


/*----------- Local Codec functions ---------------------------------------*/

/***************** C O M P O N E N T   F U N C T I O N S ******************/


/************************************************************************
 * Function: FileSystemNotify
 *************************************************************************
 * Description:  monitor the nvm file and reload the nvm file runtime.
 *
 * Parameters:
 *
 * Return value:
 *
 * Notes:
 *************************************************************************/
void FileSystemNotify(int iParam)
{
	printf("AUDIO, FileSystemNotify\n");
	if(iParam == MAGIC)
		LevanteReadFdiData();
}
/************************************************************************
 * Function: LevanteInit
 *************************************************************************
 * Description:  Initialize the Levante codec.
 *
 * Parameters:   unsigned char             reinit - first init or re-init of the codec.
 *               ACM_SspAudioConfiguration **sspAudioConfiguration
 *               signed short              **ditherGenConfig
 *
 * Return value:
 *
 * Notes:
 *************************************************************************/
/*static*/ void LevanteInit(unsigned char reinit, ACM_SspAudioConfiguration **sspAudioConfiguration, signed short **ditherGenConfig)
{
	int i;
	char *nvm_root_dir = getenv("NVM_ROOT_DIR");
	UNUSEDPARAM(ditherGenConfig)
#if 0
	static OSA_STATUS creatingSemaphoreStatus = OS_INVALID_REF;
	unsigned char pmicId = ACMPmicIdGet();

	DIAG_FILTER(AUDIO_LEVANTE, LEVANTE, AUDIO_LEVANTE_audioLevanteInit, DIAG_INFORMATION)
	diagPrintf("AUDIO_LEVANTE_audioLevanteInit; pmicId= 0x%x, reinit= %d", pmicId, reinit);

	ditherGenConfig = ditherGenConfig;
	_pmicId         = pmicId;

	ACM_ASSERT(sspAudioConfiguration != NULL);
	if ( (ACMPlatformInfoGet() & (1 << PLATFORM_TAVOR_PV2)) == (1 << PLATFORM_TAVOR_PV2) )
		*sspAudioConfiguration = (ACM_SspAudioConfiguration *)&_sspAudioLevante16KHzConfiguration;      /* Configure the SSP parameters for 16 KHz */
	else
		*sspAudioConfiguration = (ACM_SspAudioConfiguration *)&_sspAudioLevante8KHzConfiguration;       /* Configure the SSP parameters for 8 KHz */
#endif

	*sspAudioConfiguration = (ACM_SspAudioConfiguration *)&_sspAudioLevante8KHzConfiguration;       /* Configure the SSP parameters for 8 KHz */
	if (reinit)
	{                                                                                               /* In case of reinit, update the SSP Configuration only and return! */
		return;
	}

	for (i = 0; i < NUM_OF_REGS; i++)
	{
		shadowReg[i] = registerInfo[i].defaultValue;
	}

	gLevanteDatabase.totalEnables = 0;
	gLevanteDatabase.loopbacktest = 0;

	for (i = 0; i < LEVANTE_TOTAL_VALID_PATHS; i++)
	{
		gLevantePathsSortedList[i]     = INVALID_INDEX;

		gLevanteCurrentPaths[i].order  = INVALID_INDEX;
		gLevanteCurrentPaths[i].volume = INVALID_INDEX;
		gLevanteCurrentPaths[i].muted  = ACM_MUTE_OFF;
	}

	/* Default field values for the additional settings */
	for (i = 0; i < NUM_OF_EXTRA_SETTINGS_FIELDS; i++)
		regField[i] = 0;

	regField[EQU_BYP]          = 1;         /* Bypass equalizer */
	regField[MBC1_2]           = 3;         /* Maximum current for Analog MIC 1 & 2 */
	regField[MBC3]             = 3;         /* Maximum current for Analog MIC 3 */
	regField[AUDIO_ZLOAD_4OHM] = 1;         /* 8 ohm */
	regField[I2S_MASTER] = 1;               /* Codec as master --Raul */
	if (gCPIsPCMMaster == 1)
		regField[PCM_MASTER] = 1;           /* Codec as master, otherwise we will get click noise */
	regField[I2S_INT_LOOPBACK] = 1;         /*Enable I2S internal loopback for FM radio output to speaker and earpiece --Raul*/

	/* Handle gain for Force Speaker (headset + loud-speaker); set to default values */
	regField[DPGA1_HIFIL_HIFIR_GAIN_LEFT]  = DPGA1_HIFIL_HIFIR_GAIN_LEFT_DEFAULT_VALUE;
	regField[DPGA2_HIFIL_HIFIR_GAIN_RIGHT] = DPGA2_HIFIL_HIFIR_GAIN_RIGHT_DEFAULT_VALUE;

	LevanteReadFdiData();
	addfileNotifyCb(nvm_root_dir, LEVANTE_AP_AUDIO_FDI_FILENAME, FileSystemNotify, MAGIC);
} /* End of LevanteInit */


/************************************************************************
 * Function: LevantePathEnable
 *************************************************************************
 * Description:  Enable Levante Path.
 *
 * Parameters:   unsigned char   path
 *               ACM_AudioVolume volume
 *
 * Return value: ACM_DigitalGain
 *
 * Notes:
 *************************************************************************/
static ACM_DigitalGain LevantePathEnable(unsigned char path, ACM_AudioVolume volume)
{
	ACM_DigitalGain digGain;

	//DIAG_FILTER(AUDIO_LEVANTE, LEVANTE, AUDIO_LEVANTE_audioLevantePathEnable, DIAG_INFORMATION)
	//diagPrintf("AUDIO_LEVANTE_audioLevantePathEnable (%d, %d)", path, volume);
	printf("AUDIO_LEVANTE_audioLevantePathEnable (%d, %d)\n", path, volume);

	digGain = LevanteGetMsaDigitalGain(path, volume);

	LevanteAddPathToList(path, volume, digGain);

	LevanteUpdateHardware();

	return digGain;
} /* End of LevantePathEnable */


/************************************************************************
 * Function: LevantePathDisable
 *************************************************************************
 * Description:  Disable Levante Path.
 *
 * Parameters:   unsigned char path
 *
 * Return value: ACM_DigitalGain
 *
 * Notes:
 *************************************************************************/
static ACM_DigitalGain LevantePathDisable(unsigned char path)
{
	//DIAG_FILTER(AUDIO_LEVANTE, LEVANTE, AUDIO_LEVANTE_audioLevantePathDisable, DIAG_INFORMATION)
	//diagPrintf("AUDIO_LEVANTE_audioLevantePathDisable (%d)", path);
	printf("AUDIO_LEVANTE_audioLevantePathDisable (%d)\n", path);

	LevanteRemovePathFromList(path);
	LevanteUpdateHardware();

	if (gLevanteDatabase.totalEnables == 0)
	{       /* This is the case of turning the last path off */
		printf("---------------- LevanteShutDownConfigSet --------------\n");
		LevanteShutDownConfigSet();
	}

	return gLevanteCurrentPaths[path].digitalGain;
} /* End of LevantePathDisable */


/************************************************************************
 * Function: LevantePathVolumeSet
 *************************************************************************
 * Description:  Set Levante Path's volume.
 *
 * Parameters:   unsigned char   path
 *               ACM_AudioVolume volume
 *
 * Return value: ACM_DigitalGain
 *
 * Notes:
 *************************************************************************/
static ACM_DigitalGain LevantePathVolumeSet(unsigned char path, ACM_AudioVolume volume)
{
	ACM_DigitalGain digGain;

	//DIAG_FILTER(AUDIO_LEVANTE, LEVANTE, AUDIO_LEVANTE_audioLevantePathVolumeSet, DIAG_INFORMATION)
	//diagPrintf("AUDIO_LEVANTE_audioLevantePathVolumeSet (%d, %d)", path, volume);
	//printf("AUDIO_LEVANTE_audioLevantePathVolumeSet (%d, %d)\n", path, volume);

	digGain = LevanteGetMsaDigitalGain(path, volume);

	gLevanteCurrentPaths[path].volume      = volume;
	gLevanteCurrentPaths[path].digitalGain = digGain;

	LevanteUpdateHardware();

	return digGain;
} /* End of LevantePathVolumeSet */


/************************************************************************
 * Function: LevantePathMute
 *************************************************************************
 * Description:  Set Levante Path's to mute / un-mute.
 *
 * Parameters:   unsigned char path
 *               ACM_AudioMute mute
 *
 * Return value: ACM_DigitalGain
 *
 * Notes:
 *************************************************************************/
static ACM_DigitalGain LevantePathMute(unsigned char path, ACM_AudioMute mute)
{
	//DIAG_FILTER(AUDIO_LEVANTE, LEVANTE, AUDIO_LEVANTE_audioLevantePathMute, DIAG_INFORMATION)
	//diagPrintf("AUDIO_LEVANTE_audioLevantePathMute (%d, %d)", path, mute);
	printf("AUDIO_LEVANTE_audioLevantePathMute (%d, %d)\n", path, mute);

	gLevanteCurrentPaths[path].muted = mute;
	LevanteUpdateHardware();

	return gLevanteCurrentPaths[path].digitalGain;
} /* End of LevantePathMute */


/************************************************************************
 * Function: LevanteSetPathsAnalogGain
 *************************************************************************
 * Description:  Set Levante Path's Analog Gain according to CP ACM's return value.
 *
 * Parameters:   unsigned char regIndex - the index the register to set
 *               unsigned char regValue - the value to set
 *               unsigned char regMask
 *               unsigned char regShift
 *
 * Return value: ACM_DigitalGain
 *
 * Notes:
 ***************************************************************************/
static ACM_DigitalGain LevanteSetPathsAnalogGain(unsigned char regIndex, unsigned char regValue, unsigned char regMask, unsigned char regShift)
{
	BOOL isChanged = FALSE;
	unsigned char reg = shadowReg[regIndex];

	WRITE_GAIN_BITS(reg, regValue, regMask, regShift);

	if (shadowReg[regIndex] != reg)
	{
		printf("-- SetGain: reg 0x%x is 0x%x, shadowReg 0x%x is 0x%x\n", registerInfo[regIndex].address, reg, registerInfo[regIndex].address, shadowReg[regIndex]);
		isChanged = TRUE;
		LevanteRegSet(registerInfo[regIndex].address, reg);
		shadowReg[regIndex] = reg;
	}

	if (isChanged)
	{		/* Apply the changes! */
		/*Raul: ASOC will remember the value, so first clear and then set to ensure write the register successfully*/
		LevanteRegSet(LEVANTE_EAR_SPKR_CTRL2_ADDRESS, (shadowReg[LEVANTE_EAR_SPKR_CTRL2_IDX] & (~(1 << APPLY_CHANGES_SHIFT))));
		LevanteRegSet(LEVANTE_EAR_SPKR_CTRL2_ADDRESS, (shadowReg[LEVANTE_EAR_SPKR_CTRL2_IDX] | (1 << APPLY_CHANGES_SHIFT)));
	}

	return regValue;
}


/************************************************************************
 * Function: LevanteGetPathsStatus
 *************************************************************************
 * Description:  Get Levante Path's status.
 *
 * Parameters:   char* data - pointer to the status info (output param)
 *               short length
 *
 * Return value: short - number of written bytes.
 *
 * Notes:
 ***************************************************************************/
static short LevanteGetPathsStatus(char* data, short length)
{
	unsigned char path;
	char          *ptr = data;

	//DIAG_FILTER(AUDIO_LEVANTE, LEVANTE, AUDIO_LEVANTE_audioLevanteGetPathsStatus, DIAG_INFORMATION)
	//diagTextPrintf("AUDIO_LEVANTE_audioLevanteGetPathsStatus");

	if (length < (1 + LEVANTE_TOTAL_VALID_PATHS * 4))
	{
		return ((short)(-1));  /* buffer too small */
	}

	*(ptr++) = LEVANTE_TOTAL_VALID_PATHS;

	for (path = 0; path < LEVANTE_TOTAL_VALID_PATHS; path++)
	{
		*(ptr++) = path;
		*(ptr++) = (gLevanteCurrentPaths[path].order != INVALID_INDEX);  /* 1 = enabled, 0 = disabled */
		*(ptr++) = gLevanteCurrentPaths[path].volume;
		*(ptr++) = gLevanteCurrentPaths[path].muted;
	}

	return ((short)(ptr - data));  /* bytes written */
} /* End of LevanteGetPathsStatus */


/************************************************************************
 * Function: LevanteGetPathAnalogGain
 *************************************************************************
 * Description:  Get Levante Path's analog gain.
 *
 * Parameters:   unsigned char path
 *
 * Return value: ACM_DigitalGain - the analog gain that would have been
 *               configured if this path is at high priority, AKA was just
 *               enabled.
 *
 * Notes:
 *************************************************************************/
static ACM_AnalogGain LevanteGetPathAnalogGain(unsigned char path)
{
	unsigned char i, j;
	unsigned char totalPathInGain;
	ACM_AnalogGain analogGain = 0;

	/* loop over all the gain, and if the path is included in this gain, add to analogGain */
	for (i = 1; i < LEVANTE_TOTAL_GAINS; i++)  /* start from index "1" - skip digital gain */
	{
		/* IMPORTANT
		 * this calculation is based on the fact that this function "LevanteGetPathAnalogGain"
		 * is called AFTER "Enable" of that specific path - only then the values returned
		 * by LevanteGetGainIndB is the value which the register is configured to.
		 */

		totalPathInGain = gainConverter[i].totalPaths;
		for (j = 0; j < totalPathInGain; j++)
		{
			/* calculating the gain if the path is included in the gain */
			if (gainConverter[i].gainPath[j].path == path)
				analogGain += LevanteGetGainIndB(path, (LevanteGainsId)i, j);
		}
	}

	//DIAG_FILTER(AUDIO_LEVANTE, LEVANTE, AUDIO_LEVANTE_GetPathAnalogGain, DIAG_INFORMATION)
	//diagPrintf("(AnalogGain = %d)", analogGain);

	return analogGain;
} /* End of LevanteGetPathAnalogGain */


/************************************************************************
 * Function: LevanteGetActivePathNum
 *************************************************************************
 * Description:  Get the number of Levante enabled path
 *
 * Parameters:   none
 *
 * Return value: unsigned int
 *
 * Notes:
 *************************************************************************/
static unsigned int LevanteGetActivePathNum(void)
{
	return gLevanteDatabase.totalEnables;
}

/************************************************************************
 * Function: LevanteSetLoopbackTestStatus
 *************************************************************************
 * Description:  set if it enters loopback test for using ACAT
 *
 * Parameters:   none
 *
 * Return value: unsigned int
 *
 * Notes:
 *************************************************************************/
static unsigned int LevanteSetLoopbackTestStatus(unsigned char isloopbacktest)
{
	return gLevanteDatabase.loopbacktest = isloopbacktest;
}

/************************************************************************
 * Function: LevanteGetLoopbackTestStatus
 *************************************************************************
 * Description:  indicate if it enters loopback test
 *
 * Parameters:   none
 *
 * Return value: unsigned int
 *
 * Notes:
 *************************************************************************/
static unsigned int LevanteGetLoopbackTestStatus(void)
{
	return gLevanteDatabase.loopbacktest;
}

/************************************************************************
 * Function: LevanteAdditionalSettingsSet
 *************************************************************************
 * Description:  Set the Additional Settings for the Levante Codec.
 *
 * Parameters:   LevanteSettingsType settingsType - the type of settings
 *               void *settingsData - the data of the settings
 *
 * Return value:
 *
 * Notes:
 *************************************************************************/
void LevanteAdditionalSettingsSet(LevanteSettingsType settingsType, void *settingsData)
{
	switch (settingsType)
	{
	case FREQUENCY_SETTINGS:
	{
		LevanteFrequencySettingsData frequencySettingsData = *((LevanteFrequencySettingsData *)settingsData);

		regField[I_OPA_HALF] = 0;          /* for all rates excluding 8 kHz */

		switch (frequencySettingsData.digitalPath)
		{
		case LOFI:
			switch (frequencySettingsData.bitRate)
			{
			case LEVANTE_BIT_RATE_8_KHZ:
				regField[I_OPA_HALF]   = 1;
				regField[RATE_ADC_PCM] = 0;                          /* 0b0000 */
				break;

			case LEVANTE_BIT_RATE_16_KHZ:
				regField[RATE_ADC_PCM] = 3;                          /* 0b0011 */
				break;

			case LEVANTE_BIT_RATE_32_KHZ:
				regField[RATE_ADC_PCM] = 6;                          /* 0b0110 */
				break;

			case LEVANTE_BIT_RATE_48_KHZ:
				//assert(frequencySettingsData.freqDirection == RECORD);  /* Only Record is supported in this rate */
				regField[RATE_ADC_PCM] = 8;                          /* 0b1000 */
				break;

			default:
				DPRINTF("\nassert in LOFI default\n");
				assert(FALSE);
				break;
			}
			break;

		case HIFI:
			switch (frequencySettingsData.bitRate)
			{
			case LEVANTE_BIT_RATE_8_KHZ:
				regField[RATE_DAC_I2S] = 0;                                     /* 0b0000 */
				if (frequencySettingsData.freqDirection != PLAY)
				{                                                               /* Record or Both Play+Record */
					regField[I_OPA_HALF]   = 1;
					regField[RATE_ADC_PCM] = 0;                             /* 0b0000 */
				}
				break;

			case LEVANTE_BIT_RATE_11_025_KHZ:
				if (frequencySettingsData.freqDirection != PLAY)
					DPRINTF("\nassert in (frequencySettingsData.freqDirection == PLAY)\n");
				assert(frequencySettingsData.freqDirection == PLAY);                            /* Record or Both Play+Record are not supported in this rate */
				regField[RATE_DAC_I2S] = 1;                                                     /* 0b0001 */
				break;

			case LEVANTE_BIT_RATE_16_KHZ:
				regField[RATE_DAC_I2S] = 3;                                     /* 0b0011 */
				if (frequencySettingsData.freqDirection != PLAY)
				{                                                               /* Record or Both Play+Record */
					regField[RATE_ADC_PCM] = 3;                             /* 0b0011 */
				}
				break;

			case LEVANTE_BIT_RATE_22_05_KHZ:
				if (frequencySettingsData.freqDirection != PLAY)
					DPRINTF("\nassert in (frequencySettingsData.freqDirection == PLAY)\n");
				assert(frequencySettingsData.freqDirection == PLAY);                            /* Record or Both Play+Record are not supported in this rate */
				regField[RATE_DAC_I2S] = 4;                                                     /* 0b0100 */
				break;

			case LEVANTE_BIT_RATE_32_KHZ:
				regField[RATE_DAC_I2S] = 6;                                     /* 0b0110 */
				if (frequencySettingsData.freqDirection != PLAY)
				{                                                               /* Record or Both Play+Record */
					regField[RATE_ADC_PCM] = 6;                             /* 0b0110 */
				}
				break;

			case LEVANTE_BIT_RATE_44_1_KHZ:
				if (frequencySettingsData.freqDirection != PLAY)
					DPRINTF("\nassert in (frequencySettingsData.freqDirection == PLAY)\n");
				assert(frequencySettingsData.freqDirection == PLAY);                            /* Record or Both Play+Record are not supported in this rate */
				regField[RATE_DAC_I2S] = 7;                                                     /* 0b0111 */
				break;

			case LEVANTE_BIT_RATE_48_KHZ:
				regField[RATE_DAC_I2S] = 8;                                     /* 0b1000 */
				if (frequencySettingsData.freqDirection != PLAY)
				{                                                               /* Record or Both Play+Record */
					regField[RATE_ADC_PCM] = 8;                             /* 0b1000 */
				}
				break;

			default:
				DPRINTF("\nassert in HIFI default\n");
				assert(FALSE);
				break;
			}
			break;

		default:
			DPRINTF("\nassert in default default\n");
			assert(FALSE);
			break;
		}
	}
	break;

	case EQUALIZER_SETTINGS:
	{
		LevanteEqualizerSettingsData equalizerSettingsData = *((LevanteEqualizerSettingsData *)settingsData);

		regField[EQU_BYP] = (equalizerSettingsData.isEqualizerEnabled == FALSE) ? 1 : 0;

		regField[EQ_N0_1] = (unsigned char)(equalizerSettingsData.N0 & 0x00FF);
		regField[EQ_N0_2] = (unsigned char)((equalizerSettingsData.N0 & 0xFF00) >> 8);

		regField[EQ_N1_1] = (unsigned char)(equalizerSettingsData.N1 & 0x00FF);
		regField[EQ_N1_2] = (unsigned char)((equalizerSettingsData.N1 & 0xFF00) >> 8);

		regField[EQ_D1_1] = (unsigned char)(equalizerSettingsData.D1 & 0x00FF);
		regField[EQ_D1_2] = (unsigned char)((equalizerSettingsData.D1 & 0xFF00) >> 8);
	}
	break;

	case TRI_STATE_SETTINGS:
	{
		LevanteTriStateSettingsData triStateSettingsData = *((LevanteTriStateSettingsData *)settingsData);

		regField[HIZ_DOUT]     = (unsigned char)(triStateSettingsData.isPcmSdoTriStateEnabled ? 1 : 0);
		regField[HIZ_I2S_DOUT] = (unsigned char)(triStateSettingsData.isI2sSdoTriStateEnabled ? 1 : 0);
	}
	break;

	case DATA_LENGTH_SETTINGS:
	{
		LevanteFormatDataLengthSettingsData formatDataLengthData = *((LevanteFormatDataLengthSettingsData *)settingsData);

		assert( (formatDataLengthData.dataLength == LENGTH_16_BIT) || (formatDataLengthData.dataLength == LENGTH_18_BIT) );

		switch (formatDataLengthData.inputType)
		{
		case PCM_TYPE:
			regField[PCM_WL] = (unsigned char)((formatDataLengthData.dataLength == LENGTH_16_BIT) ? 0 : 1);
			break;

		case I2S_TYPE:
			regField[I2S_WL] = (unsigned char)((formatDataLengthData.dataLength == LENGTH_16_BIT) ? 0 : 1);
			break;

		default:
			assert(FALSE);
			break;
		}
	}
	break;

	case DIG_MIC_INIT_SETTINGS:
	{
		LevanteDigMicInitSettingsData digMicInitData = *((LevanteDigMicInitSettingsData *)settingsData);

		assert(digMicInitData.OSR <= OSR_72);
		assert(digMicInitData.clockDelay <= 15);
		assert(digMicInitData.dataDelay <= 15);

		osrRateOffset            = (unsigned char)digMicInitData.OSR;          /* 0=OSR_126, 1=OSR_84, 2=OSR_72 */
		regField[DMIC_CLOCK_SEL] = (unsigned char)digMicInitData.clockDelay;
		regField[DMIC_DATA_SEL]  = (unsigned char)digMicInitData.dataDelay;
	}
	break;

	case ANALOG_MIC_INIT_SETTINGS:
	{
		LevanteAnalogMicInitSettingsData analogMicInitData = *((LevanteAnalogMicInitSettingsData *)settingsData);

		assert(analogMicInitData.micBiasCurrent_1_2 <= 3);
		assert(analogMicInitData.micBiasCurrent_3 <= 3);

		regField[MBC1_2] = (unsigned char)analogMicInitData.micBiasCurrent_1_2;
		regField[MBC3]   = (unsigned char)analogMicInitData.micBiasCurrent_3;
	}
	break;

	case LOOPBACK_TEST_SETTINGS:
	{               /* ASK ASSAF R. - for which in/out paths does it apply? */
		LevanteLoopbackTestSettingsData loopbackTestData = *((LevanteLoopbackTestSettingsData *)settingsData);

		switch (loopbackTestData.interfaceType)
		{
		case PCM_INTERFACE:
			switch (loopbackTestData.loopbackType)
			{
			case INTERNAL_LOOPBACK:
				regField[PCM_INT_LOOPBACK] = (unsigned char)loopbackTestData.isLoopbackEnabled ? 1 : 0;
				break;

			case EXTERNAL_LOOPBACK:
				regField[PCM_EXT_LOOPBACK] = (unsigned char)loopbackTestData.isLoopbackEnabled ? 1 : 0;
				break;

			default:
				assert(FALSE);
				break;
			}
			break;

		case I2S_INTERFACE:
			switch (loopbackTestData.loopbackType)
			{
			case INTERNAL_LOOPBACK:
				regField[I2S_INT_LOOPBACK] = (unsigned char)loopbackTestData.isLoopbackEnabled ? 1 : 0;
				break;

			case EXTERNAL_LOOPBACK:
				regField[I2S_EXT_LOOPBACK] = (unsigned char)loopbackTestData.isLoopbackEnabled ? 1 : 0;
				break;

			default:
				assert(FALSE);
				break;
			}
			break;

		default:
			assert(FALSE);
			break;
		}
	}
	break;

	case MASTER_SLAVE_MODE_SETTINGS:
	{
		LevanteMasterSlaveModeSettingsData masterSlaveModeSettingsData = *((LevanteMasterSlaveModeSettingsData *)settingsData);

		switch (masterSlaveModeSettingsData.inputType)
		{
		case PCM_TYPE:
			regField[PCM_MASTER] = (unsigned char)(masterSlaveModeSettingsData.isMasterOnClockAndFrame ? 1 : 0);
			break;

		case I2S_TYPE:
			regField[I2S_MASTER] = (unsigned char)(masterSlaveModeSettingsData.isMasterOnClockAndFrame ? 1 : 0);
			break;

		default:
			assert(FALSE);
			break;
		}
	}
	break;

	case LOUD_SPEAKER_SETTINGS:
	{
		LevanteLoudSpeakerSettingsData loudSpeakerSettingsData = *((LevanteLoudSpeakerSettingsData *)settingsData);

		assert( (loudSpeakerSettingsData.speakerLoad == LOAD_4_OHM) || (loudSpeakerSettingsData.speakerLoad == LOAD_8_OHM) );

		regField[AUDIO_ZLOAD_4OHM] = (unsigned char)((loudSpeakerSettingsData.speakerLoad == LOAD_4_OHM) ? 0 : 1);
	}
	break;

	case CHARGE_PUMP_CLOCK_SETTINGS:
	{
		LevanteChargePumpClockSettingsData chargePumpClockSettingsData = *((LevanteChargePumpClockSettingsData *)settingsData);

		//DIAG_FILTER(AUDIO, LEVANTE, LevanteAdditionalSettingsSet_charge_pump, DIAG_ALL)
		//diagPrintf("LevanteAdditionalSettingsSet; fmFrequency= %d", chargePumpClockSettingsData.fmFrequency);
		printf("LevanteAdditionalSettingsSet; fmFrequency= %d\n", chargePumpClockSettingsData.fmFrequency);
		_fmRadioFrequency = chargePumpClockSettingsData.fmFrequency;
	}
	break;

	case FORCE_SPEAKER_GAIN_SETTINGS:  /* Headset + Loud-Speaker */
	{
		LevanteForceSpeakerGainSettingsData forceSpeakerGainSettingsData = *((LevanteForceSpeakerGainSettingsData *)settingsData);

		regField[DPGA1_HIFIL_HIFIR_GAIN_LEFT]  = forceSpeakerGainSettingsData.headsetVolumeIdx;  /* Gain IDs 9+10 */
		regField[DPGA2_HIFIL_HIFIR_GAIN_RIGHT] = forceSpeakerGainSettingsData.speakerVolumeIdx;  /* Gain IDs 12+13 */
	}
	break;

	default:
		break;
	}

	LevanteUpdateHardware();
} /* End of LevanteAdditionalSettingsSet */


/************************************************************************
 * Function: LevanteGetGainData
 *************************************************************************
 * Description:  Get LevanteGains settings .
 *
 * Parameters:  ACM_SYNC_Gain **ppACMGains
 * Parameters:  int *pLen, length of ppACMGains, in sizeof(ACM_SYNC_Gain)
 *
 * Return value:
 *
 * Notes:
 *************************************************************************/
void LevanteGetGainData(void **ppACMGains, int *pLen)
{
	unsigned char i, j;
	unsigned char totalPathInGain;
	ACM_SYNC_Gain *pACMGains;
	int len = 0;

	for (i = 1; i < LEVANTE_TOTAL_GAINS; i++)  /* start from index "1" - skip digital gain */
	{
		len += gainConverter[i].totalPaths;
	}
	*pLen = len;
	*ppACMGains = (void *)malloc(len * sizeof(ACM_SYNC_Gain));
	pACMGains = (ACM_SYNC_Gain *)*ppACMGains;

	/* Read the Gains*/
	for (i = 1; i < LEVANTE_TOTAL_GAINS; i++)  /* start from index "1" - skip digital gain */
	{
		totalPathInGain = gainConverter[i].totalPaths;
		for (j = 0; j < totalPathInGain; j++)
		{
			pACMGains->gainID = (unsigned char)i;
			pACMGains->path = (unsigned char)gainConverter[i].gainPath[j].path;
			pACMGains->slope = (unsigned short)gainConverter[i].gainPath[j].pga.slope;
			pACMGains->offset = (unsigned short)gainConverter[i].gainPath[j].pga.offset;

			pACMGains += 1;
		}		/* for */
	}			/* for */
} /* End of LevanteRegSet */


/****************** C A L I B R A T I O N   F U N C T I O N S ******************/

/*******************************************************************************
 * Function:    LevanteGetGainIndB
 ********************************************************************************
 * Description:  If openning of FDI file is successful, reading the calibration
 *               gains information from it.
 *
 * Parameters:   unsigned char  path
 *               LevanteGainsId gainId
 *               unsigned char  pathLoc
 *
 * Return value: ACM_AnalogGain - gain in dB depanding on the amps range in use.
 *
 * Notes:
 ********************************************************************************/
static ACM_AnalogGain LevanteGetGainIndB(unsigned char path, LevanteGainsId gainId, unsigned char pathLoc)
{
	short gain = 0;

	if ( (gainId > DIGITAL_GAIN) && (gainId < LEVANTE_TOTAL_GAINS) )
		gain = LEVANTE_GAIN_CALC(gLevanteCurrentPaths[path].volume, gainConverter[gainId].gainPath[pathLoc].pga);

	return ((ACM_AnalogGain)(gain & 0xFF));
} /* End of LevanteGetGainIndB */

