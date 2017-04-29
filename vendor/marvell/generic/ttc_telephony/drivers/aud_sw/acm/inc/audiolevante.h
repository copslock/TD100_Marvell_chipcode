/*------------------------------------------------------------
   (C) Copyright [2006-2008] Marvell International Ltd.
   All Rights Reserved
   ------------------------------------------------------------*/

/*******************************************************************************
*               MODULE HEADER FILE
********************************************************************************
* Title: audioLevante
*
* Filename: audioLevante.h
*
* Target, platform: Common Platform, SW platform
*
* Authors: Isar Ariel
*
* Description:
*
* Last Updated:
*
* Notes:
*******************************************************************************/
#ifndef _AUDIO_LEVANTE_H_
#define _AUDIO_LEVANTE_H_

#ifdef __cplusplus
extern "C" {
#endif

/*----------- Global constant definitions ------------------------------------*/
#define INVALID_INDEX                   0xFF
#define LEVANTE_AP_AUDIO_FDI_FILENAME              "audio_levante.nvm"

#define MAGIC 0x127

/*----------- Global macro definitions ---------------------------------------*/
#define LEVANTE_GAIN_CALC(vOL, sLOPEoFFSET)   ((signed char)(((((signed long)(sLOPEoFFSET.slope) * (vOL) + ((sLOPEoFFSET.offset) << 3)) >> 9) + 1) >> 1))


/*----------- Global type definitions ----------------------------------------*/
typedef struct
{
	unsigned char order;    /* 0 = disabled, else - place in list*/
	unsigned char volume;   /* 0xFF = disabled */
	ACM_AudioMute muted;
	ACM_DigitalGain digitalGain;
} LevanteCurrentPathS;

typedef struct
{
	unsigned char totalEnables;
	LEVANTE_I2SbitRate playBitRate;
	LEVANTE_I2SbitRate recBitRate;
	unsigned char loopbacktest;
} LevanteDatabaseS;

typedef signed short LEVANTE_SlopeType;
typedef signed short LEVANTE_OffsetType;

typedef struct
{
	LEVANTE_SlopeType slope;
	LEVANTE_OffsetType offset;
} LEVANTE_SlopeOffsetS;

/* input */
typedef enum
{
	AnaMIC1_IN = 0,
	AnaMIC2_IN,
	AnaMIC3_IN,
	DigMIC1_IN,
	DigMIC2_IN,
	AUX1_IN,
	AUX2_IN,
	AUX1_ANA_LOOP_IN,
	AUX2_ANA_LOOP_IN,
	SideTone_L_IN,
	SideTone_R_IN,
	SideTone_BOTH_IN,
	PCM_L_IN,
	PCM_R_IN,
	I2S_IN,
	I2S1_IN,
	I2S_LOOP_EXT_IN,
	I2S_LOOP_INT_IN,
	PCM_LOOP_EXT_IN,
	PCM_LOOP_INT_IN,
	EC_DAC1_IN,
	EC_DAC2_IN,
	EC_DAC1_DAC2_IN,
	/*add for aux to speaker by using sidetone*/
	FM_VIA_SIDETONE_IN,

	LEVANTE_MAX_PATH_INPUT,
	LEVANTE_PATHS_INPUT_ENUM_32_BIT = 0x7FFFFFFF //32bit enum compiling enforcement
} LevantePathInput;

/* output*/
typedef enum
{
	SideTone_OUT = 0,
	EAR_VIA_DAC1_OUT,
	SPKR_VIA_DAC1_OUT,
	HS1_VIA_DAC1_OUT,
	HS2_VIA_DAC1_OUT,
	LINEOUT1_VIA_DAC1_OUT,
	LINEOUT2_VIA_DAC1_OUT,
	EAR_VIA_DAC2_OUT,
	SPKR_VIA_DAC2_OUT,
	HS1_VIA_DAC2_OUT,
	HS2_VIA_DAC2_OUT,
	LINEOUT1_VIA_DAC2_OUT,
	LINEOUT2_VIA_DAC2_OUT,
	HS_STEREO_OUT,
	HS_STEREO_FLIP_OUT,
	LINEOUT_STEREO_OUT,
	LINEOUT_STEREO_FLIP_OUT,
	HS2_ANA_LOOP_OUT,
	LINEOUT2_ANA_LOOP_OUT,
	HS1_ANA_LOOP_OUT,
	LINEOUT1_ANA_LOOP_OUT,
	PCM_L_OUT,
	PCM_R_OUT,
	I2S_VIA_PCM_L_OUT,
	I2S_VIA_PCM_R_OUT,
	I2S_LOOP_EXT_OUT,
	I2S_LOOP_INT_OUT,
	PCM_LOOP_EXT_OUT,
	PCM_LOOP_INT_OUT,
	EC_OUT,
	I2S_L_Via_Ext_PA_LOOP_OUT,
	I2S_R_Via_Ext_PA_LOOP_OUT,
	I2S_BOTH_Via_Ext_PA_LOOP_OUT,
	/*add for aux to speaker by using sidetone*/
	SIDETONE_L_OUT,
	SIDETONE_R_OUT,

	/*forced speaker*/
	HS_VIA_DAC1_OUT_SPKR_VIA_DAC2_OUT,

	/*add for recording by GSSP*/
	PCM_I2S_L_OUT,
	PCM_I2S_R_OUT,

	LEVANTE_MAX_PATH_OUTPUT,
	LEVANTE_PATHS_OUTPUT_ENUM_32_BIT = 0x7FFFFFFF //32bit enum compiling enforcement
} LevantePathOutput;

typedef enum
{
	DIGITAL_GAIN = 0,

	PGA_EARP_GAIN,                  /* Earphone Analog Driver            0 dB to 12.89 dB */
	PGA_SPKR_GAIN,                  /* Speakerphone Analog Driver        0 dB to 12.89 dB */
	PGA_HS1_GAIN,                   /* HeadSet1 Analog Driver            0 dB to 12.89 dB */
	PGA_HS2_GAIN,                   /* HeadSet2 Analog Driver            0 dB to 12.89 dB */
	PGA_LOUT1_GAIN,                 /* Line Out 1 Analog Driver          0 dB to 12.89 dB */
	PGA_LOUT2_GAIN,                 /* Line Out 2 Analog Driver          0 dB to 12.89 dB */

	DPGA1_LOFI_GAIN_LEFT_GAIN,      /* LOFI Left Digital Gain            0 dB to (-93.18) dB */
	DPGA1_HIFIL_GAIN_LEFT_GAIN,     /* Left HIFI Left Digital Gain       0 dB to (-93.18) dB */
	DPGA1_HIFIR_GAIN_LEFT_GAIN,     /* Right HIFI Left Digital Gain      0 dB to (-93.18) dB */
	DPGA2_LOFI_GAIN_RIGHT_GAIN,     /* LOFI Right Digital Gain           0 dB to (-93.18) dB */
	DPGA2_HIFIL_GAIN_RIGHT_GAIN,    /* Left HIFI Right Digital Gain      0 dB to (-93.18) dB */
	DPGA2_HIFIR_GAIN_RIGHT_GAIN,    /* Right HIFI Right Digital Gain     0 dB to (-93.18) dB */

	PGA_MIC1_GAIN,                  /* MIC1or MIC2 Analog Driver         6 dB to 29 dB */
	PGA_MIC3_GAIN,                  /* MIC3 Analog Driver                6 dB to 29 dB */

	PGA_AUX1_GAIN,                  /* AUX1 Analog Driver                (-6) dB to 18 dB */
	PGA_AUX2_GAIN,                  /* AUX2 Analog Driver                (-6) dB to 18 dB */

	ST_R_GAIN,                      /* Side Tone Right Gain              ??? to ??? */
	ST_L_GAIN,                      /* Side Tone Left Gain               ??? to ??? */

	MOD1_GAIN,                      /* ADC Modulator 1 Gain              (-6) dB to 3 dB */
	MOD2_GAIN,                      /* ADC Modulator 2 Gain              (-6) dB to 3 dB */

	LEVANTE_TOTAL_GAINS
} LevanteGainsId;

typedef struct
{
	LevantePathInput pathInput;
	LevantePathOutput pathOutput;
} LevantePathDescriptionS;

//ICAT EXPORTED ENUM
typedef enum
{
	LEVANTE_FAIL = 0,
	LEVANTE_OK,
	LEVANTE_NO_FILE
} LevanteFdiReadFileStatus;

//ICAT EXPORTED STRUCT
typedef struct
{
	unsigned long gainId;
	unsigned long path;
	signed short slope;
	signed short offset;
} LevanteNvmRecord;

#define FM_FREQ_TO_CLK_DIV      \
    /* Freq    0.9v    1.8v */  \
	{	760	,	18	,	8	},	\
	{	761	,	18	,	10	},	\
	{	762	,	18	,	0	},	\
	{	763	,	10	,	0	},	\
	{	764	,	0	,	0	},	\
	{	765	,	8	,	18	},	\
	{	766	,	8	,	18	},	\
	{	767	,	18	,	18	},	\
	{	768	,	18	,	8	},	\
	{	769	,	10	,	8	},	\
	{	770	,	10	,	8	},	\
	{	771	,	0	,	8	},	\
	{	772	,	8	,	0	},	\
	{	773	,	8	,	0	},	\
	{	774	,	18	,	0	},	\
	{	775	,	18	,	0	},	\
	{	776	,	10	,	18	},	\
	{	777	,	10	,	18	},	\
	{	778	,	0	,	18	},	\
	{	779	,	0	,	8	},	\
	{	780	,	8	,	8	},	\
	{	781	,	8	,	8	},	\
	{	782	,	10	,	8	},	\
	{	783	,	10	,	8	},	\
	{	784	,	10	,	0	},	\
	{	785	,	0	,	0	},	\
	{	786	,	0	,	0	},	\
	{	787	,	0	,	0	},	\
	{	788	,	8	,	0	},	\
	{	789	,	8	,	10	},	\
	{	790	,	10	,	10	},	\
	{	791	,	10	,	10	},	\
	{	792	,	0	,	10	},	\
	{	793	,	0	,	8	},	\
	{	794	,	0	,	8	},	\
	{	795	,	8	,	0	},	\
	{	796	,	8	,	0	},	\
	{	797	,	8	,	0	},	\
	{	798	,	8	,	0	},	\
	{	799	,	18	,	10	},	\
	{	800	,	18	,	10	},	\
	{	801	,	0	,	10	},	\
	{	802	,	0	,	10	},	\
	{	803	,	8	,	10	},	\
	{	804	,	8	,	8	},	\
	{	805	,	8	,	8	},	\
	{	806	,	18	,	8	},	\
	{	807	,	18	,	0	},	\
	{	808	,	0	,	0	},	\
	{	809	,	0	,	0	},	\
	{	810	,	10	,	10	},	\
	{	811	,	8	,	10	},	\
	{	812	,	8	,	10	},	\
	{	813	,	8	,	18	},	\
	{	814	,	8	,	18	},	\
	{	815	,	0	,	8	},	\
	{	816	,	0	,	8	},	\
	{	817	,	0	,	8	},	\
	{	818	,	8	,	8	},	\
	{	819	,	8	,	0	},	\
	{	820	,	8	,	0	},	\
	{	821	,	8	,	10	},	\
	{	822	,	10	,	10	},	\
	{	823	,	10	,	18	},	\
	{	824	,	0	,	18	},	\
	{	825	,	0	,	18	},	\
	{	826	,	8	,	8	},	\
	{	827	,	8	,	8	},	\
	{	828	,	8	,	8	},	\
	{	829	,	10	,	8	},	\
	{	830	,	10	,	0	},	\
	{	831	,	0	,	10	},	\
	{	832	,	0	,	10	},	\
	{	833	,	0	,	18	},	\
	{	834	,	8	,	18	},	\
	{	835	,	8	,	18	},	\
	{	836	,	8	,	18	},	\
	{	837	,	10	,	8	},	\
	{	838	,	0	,	8	},	\
	{	839	,	0	,	8	},	\
	{	840	,	0	,	8	},	\
	{	841	,	8	,	8	},	\
	{	842	,	8	,	0	},	\
	{	843	,	8	,	0	},	\
	{	844	,	8	,	0	},	\
	{	845	,	0	,	0	},	\
	{	846	,	0	,	0	},	\
	{	847	,	0	,	0	},	\
	{	848	,	0	,	0	},	\
	{	849	,	8	,	0	},	\
	{	850	,	8	,	0	},	\
	{	851	,	8	,	0	},	\
	{	852	,	8	,	0	},	\
	{	853	,	0	,	8	},	\
	{	854	,	0	,	8	},	\
	{	855	,	0	,	8	},	\
	{	856	,	18	,	8	},	\
	{	857	,	10	,	8	},	\
	{	858	,	8	,	18	},	\
	{	859	,	8	,	18	},	\
	{	860	,	8	,	18	},	\
	{	861	,	0	,	18	},	\
	{	862	,	0	,	10	},	\
	{	863	,	10	,	0	},	\
	{	864	,	10	,	8	},	\
	{	865	,	8	,	8	},	\
	{	866	,	8	,	8	},	\
	{	867	,	8	,	8	},	\
	{	868	,	0	,	18	},	\
	{	869	,	0	,	18	},	\
	{	870	,	10	,	18	},	\
	{	871	,	10	,	10	},	\
	{	872	,	8	,	10	},	\
	{	873	,	8	,	10	},	\
	{	874	,	8	,	0	},	\
	{	875	,	8	,	0	},	\
	{	876	,	0	,	8	},	\
	{	877	,	0	,	8	},	\
	{	878	,	0	,	8	},	\
	{	879	,	0	,	18	},	\
	{	880	,	8	,	18	},	\
	{	881	,	8	,	10	},	\
	{	882	,	8	,	10	},	\
	{	883	,	8	,	10	},	\
	{	884	,	0	,	0	},	\
	{	885	,	0	,	0	},	\
	{	886	,	0	,	0	},	\
	{	887	,	18	,	8	},	\
	{	888	,	18	,	8	},	\
	{	889	,	8	,	8	},	\
	{	890	,	8	,	8	},	\
	{	891	,	0	,	10	},	\
	{	892	,	0	,	10	},	\
	{	893	,	0	,	10	},	\
	{	894	,	18	,	10	},	\
	{	895	,	18	,	0	},	\
	{	896	,	8	,	0	},	\
	{	897	,	8	,	0	},	\
	{	898	,	8	,	0	},	\
	{	899	,	0	,	8	},	\
	{	900	,	0	,	8	},	\
	{	901	,	0	,	8	},	\
	{	902	,	10	,	10	},	\
	{	903	,	10	,	10	},	\
	{	904	,	10	,	10	},	\
	{	905	,	8	,	10	},	\
	{	906	,	8	,	0	},	\
	{	907	,	0	,	0	},	\
	{	908	,	0	,	0	},	\
	{	909	,	10	,	0	},	\
	{	910	,	10	,	8	},	\
	{	911	,	10	,	8	},	\
	{	912	,	8	,	8	},	\
	{	913	,	8	,	8	},	\
	{	914	,	0	,	8	},	\
	{	915	,	0	,	18	},	\
	{	916	,	0	,	18	},	\
	{	917	,	10	,	18	},	\
	{	918	,	10	,	18	},	\
	{	919	,	18	,	0	},	\
	{	920	,	8	,	0	},	\
	{	921	,	8	,	0	},	\
	{	922	,	0	,	8	},	\
	{	923	,	0	,	8	},	\
	{	924	,	10	,	8	},	\
	{	925	,	18	,	8	},	\
	{	926	,	18	,	18	},	\
	{	927	,	8	,	18	},	\
	{	928	,	8	,	18	},	\
	{	929	,	8	,	0	},	\
	{	930	,	0	,	0	},	\
	{	931	,	10	,	0	},	\
	{	932	,	18	,	0	},	\
	{	933	,	18	,	10	},	\
	{	934	,	8	,	8	},	\
	{	935	,	8	,	8	},	\
	{	936	,	8	,	8	},	\
	{	937	,	8	,	8	},	\
	{	938	,	0	,	8	},	\
	{	939	,	0	,	0	},	\
	{	940	,	0	,	0	},	\
	{	941	,	0	,	0	},	\
	{	942	,	0	,	0	},	\
	{	943	,	0	,	0	},	\
	{	944	,	0	,	0	},	\
	{	945	,	8	,	8	},	\
	{	946	,	8	,	8	},	\
	{	947	,	8	,	8	},	\
	{	948	,	18	,	8	},	\
	{	949	,	18	,	8	},	\
	{	950	,	18	,	8	},	\
	{	951	,	10	,	10	},	\
	{	952	,	0	,	10	},	\
	{	953	,	8	,	0	},	\
	{	954	,	8	,	0	},	\
	{	955	,	18	,	0	},	\
	{	956	,	18	,	0	},	\
	{	957	,	10	,	8	},	\
	{	958	,	10	,	8	},	\
	{	959	,	0	,	8	},	\
	{	960	,	8	,	8	},	\
	{	961	,	8	,	10	},	\
	{	962	,	18	,	10	},	\
	{	963	,	18	,	10	},	\
	{	964	,	10	,	0	},	\
	{	965	,	10	,	0	},	\
	{	966	,	0	,	0	},	\
	{	967	,	0	,	0	},	\
	{	968	,	8	,	8	},	\
	{	969	,	8	,	8	},	\
	{	970	,	10	,	8	},	\
	{	971	,	10	,	8	},	\
	{	972	,	10	,	10	},	\
	{	973	,	0	,	10	},	\
	{	974	,	0	,	18	},	\
	{	975	,	0	,	0	},	\
	{	976	,	8	,	0	},	\
	{	977	,	8	,	0	},	\
	{	978	,	10	,	0	},	\
	{	979	,	10	,	0	},	\
	{	980	,	0	,	8	},	\
	{	981	,	0	,	8	},	\
	{	982	,	0	,	8	},	\
	{	983	,	8	,	8	},	\
	{	984	,	8	,	8	},	\
	{	985	,	8	,	8	},	\
	{	986	,	8	,	0	},	\
	{	987	,	18	,	0	},	\
	{	988	,	18	,	0	},	\
	{	989	,	0	,	0	},	\
	{	990	,	0	,	0	},	\
	{	991	,	8	,	8	},	\
	{	992	,	8	,	8	},	\
	{	993	,	8	,	8	},	\
	{	994	,	18	,	8	},	\
	{	995	,	18	,	8	},	\
	{	996	,	0	,	8	},	\
	{	997	,	0	,	0	},	\
	{	998	,	10	,	0	},	\
	{	999	,	8	,	0	},	\
	{	1000,	8	,	0	},	\
	{	1001,	8	,	0	},	\
	{	1002,	8	,	18	},	\
	{	1003,	0	,	10	},	\
	{	1004,	0	,	8	},	\
	{	1005,	0	,	8	},	\
	{	1006,	0	,	8	},	\
	{	1007,	8	,	8	},	\
	{	1008,	8	,	8	},	\
	{	1009,	8	,	0	},	\
	{	1010,	10	,	0	},	\
	{	1011,	10	,	0	},	\
	{	1012,	0	,	0	},	\
	{	1013,	0	,	10	},	\
	{	1014,	8	,	10	},	\
	{	1015,	8	,	8	},	\
	{	1016,	8	,	8	},	\
	{	1017,	10	,	8	},	\
	{	1018,	10	,	8	},	\
	{	1019,	0	,	8	},	\
	{	1020,	0	,	0	},	\
	{	1021,	0	,	0	},	\
	{	1022,	8	,	0	},	\
	{	1023,	8	,	10	},	\
	{	1024,	8	,	10	},	\
	{	1025,	10	,	10	},	\
	{	1026,	18	,	8	},	\
	{	1027,	0	,	8	},	\
	{	1028,	0	,	8	},	\
	{	1029,	0	,	8	},	\
	{	1030,	8	,	8	},	\
	{	1031,	8	,	8	},	\
	{	1032,	8	,	0	},	\
	{	1033,	0	,	0	},	\
	{	1034,	0	,	0	},	\
	{	1035,	0	,	0	},	\
	{	1036,	0	,	0	},	\
	{	1037,	8	,	8	},	\
	{	1038,	8	,	8	},	\
	{	1039,	8	,	8	},	\
	{	1040,	8	,	8	},	\
	{	1041,	0	,	8	},	\
	{	1042,	0	,	8	},	\
	{	1043,	0	,	10	},	\
	{	1044,	18	,	0	},	\
	{	1045,	10	,	0	},	\
	{	1046,	8	,	0	},	\
	{	1047,	8	,	0	},	\
	{	1048,	8	,	18	},	\
	{	1049,	0	,	18	},	\
	{	1050,	0	,	18	},	\
	{	1051,	0	,	8	},	\
	{	1052,	10	,	8	},	\
	{	1053,	8	,	8	},	\
	{	1054,	8	,	8	},	\
	{	1055,	8	,	0	},	\
	{	1056,	8	,	0	},	\
	{	1057,	0	,	0	},	\
	{	1058,	0	,	18	},	\
	{	1059,	10	,	18	},	\
	{	1060,	10	,	18	},	\
	{	1061,	8	,	8	},	\
	{	1062,	8	,	8	},	\
	{	1063,	8	,	8	},	\
	{	1064,	0	,	8	},	\
	{	1065,	0	,	8	},	\
	{	1066,	0	,	0	},	\
	{	1067,	0	,	0	},	\
	{	1068,	8	,	0	},	\
	{	1069,	8	,	0	},	\
	{	1070,	8	,	0	},	\
	{	1071,	8	,	10	},	\
	{	1072,	0	,	10	},	\
	{	1073,	0	,	10	},	\
	{	1074,	0	,	10	},	\
	{	1075,	18	,	8	},	\
	{	1076,	18	,	8	},	\
	{	1077,	8	,	8	},	\
	{	1078,	8	,	0	},	\
	{	1079,	8	,	0	},	\
	{	1080,	0	,	0	},	\
	{	0,	    0	,	0	}

#ifdef __cplusplus
}
#endif

#endif  /* _AUDIO_LEVANTE_H_ */


