/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/

#ifndef AMIXER_H
#define AMIXER_H


#define MUX_MONO                  1
#define MUX_BEAR                  2
#define MUX_LINE_OUT              3
#define MUX_STEREO_CH1            4
#define MUX_STEREO_CH2            5
#define AUDIO_LINE_AMP            6
#define STEREO_AMPLITUDE_CH1      7
#define STEREO_AMPLITUDE_CH2      8
#define HIFI_DAC_CONTROL          9
#define MONO_VOL_NEW              10
#define BEAR_VOL_NEW              11
#define I2S_CONTROL               12
#define TX_PGA                    13
#define MIC_PGA                   14
#define TX_PGA_MUX                15
#define VOICE_CODEC_ADC_CONTROL   16
#define VOICE_CODEC_VDAC_CONTROL  17
#define SIDETONE                  18
#define PGA_AUXI1_2               19
#define PGA_AUXI3                 20
#define PGA_DACS                  21
#define SOFT_START_RAMP           22


void MICCOI2CWrite(unsigned char I2CRegAddr, unsigned char I2CRegData);
int MICCOI2CRead(unsigned char I2CRegAddr, unsigned char *I2CRegData);
void LevanteI2CWrite(unsigned char I2CRegAddr, unsigned char I2CRegData);
unsigned int LEVANTEI2CRead(unsigned char I2CRegAddr, unsigned char *I2CRegData);

#endif

