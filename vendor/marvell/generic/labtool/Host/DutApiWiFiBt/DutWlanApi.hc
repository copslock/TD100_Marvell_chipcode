// #include "DutApi.hc"
#ifndef  _DUTAPI_HC_
#define _DUTAPI_HC_


#define DEVICETYPE_WLAN		(0)
#define DEVICETYPE_BT		(1)
 
#define OR_ID_RF	0
#define OR_ID_SOC	1


#define _SUPPORT_11J_
#include "../DutCommon/Dut_error.hc"

#define LEN_MACADDRESS (6)

#define LEN_MACADDRESS_VENDOR (3)

#define MAXNUM_MACADDRESS (32)

#define MAXNUM_SWTCONFIG (0x40)

#define VMR_OFFSET_SECONDARYMACADDRESS	(0x100)

#define VMR_MAX_MULTIMACADDRESS		MAXNUM_MACADDRESS

#define VMR_MAX_SECONDARYMACADDRESS	(VMR_MAX_MULTIMACADDRESS-1)


#define MAX_SETTING_PAIRS 10
#define MAX_VPSTR_SIZE	64

#define ANNEX_ADDR_END (0xFFFFFFFF)
#define ANNEX_ADDR_BLANK (0x0)


#define CHIPID_8010 0x01
#define CHIPID_8015 0x05

#define CHIPID_8020 0x02
#define CHIPID_8030 0x03
#define CHIPID_8035 0x06

#define ANT_DEF 1
#define ANT_MIN 1
#define ANT_MAX 4

#define POW_MAX 20
#define POW_MIN 0
#define POW_BOUNDARY 11
#define POW_BOUNDMARGINE 1


#define CH_MIN 1
#define CH_MAX 14
#define POW_MAX_G 20
#define POW_MIN_G 0
#define POW_LEVEL_G (POW_MAX_G - POW_MIN_G + 1)
#define POW_LEVEL_G_REVD	4

#define POW_MAX_A 20
#define POW_MIN_A 0 
#define POW_LEVEL_A (POW_MAX_A - POW_MIN_A + 1)

#define POW_LEVEL_A_REVD	4

#define DEFAULT_PWR_BANDG	14.0
#define DEFAULT_PWR_BANDA	14.0

enum {SIGNAL_801_11B, SIGNAL_801_11G}; 

enum	{BAND_802_11G, BAND_802_11A };
#define NUM_BAND 2

//#define CHCNT_MAX_G  14 
#define CHCNT_MAX_A_ONLY (35)
#ifdef _SUPPORT_11J_
#define CHCNT_MAX_J_NBW	(9)
#define CHCNT_MAX_J_DBLCT	(3)
#else
#define CHCNT_MAX_J_NBW	(0)
#define CHCNT_MAX_J_DBLCT	(0)
#endif
//#define CHCNT_MAX_A (CHCNT_MAX_A_ONLY + CHCNT_MAX_J_NBW-CHCNT_MAX_J_DBLCT)

#define CH_MIN_G 1
#define CH_MAX_G 14
#define CH_NUM_G (CH_MAX_G - CH_MIN_G + 1)
#define CH_MIN_A 1
#define CH_MAX_A 200
#define CH_NUM_A (52)
#define SUBBAND_NUM_A (5)
#define SUBBAND_NUM_G (2)

const char BandName[NUM_BAND]= {'G', 'A'};

#define LOMODE_MIN	0
#define LOMODE_MAX	2
#define LOMODE_NUM	3

#define RFPWRRANGE_MIN	0
#define RFPWRRANGE_MAX	3
#define RFPWRRANGE_NUM	(RFPWRRANGE_MAX - RFPWRRANGE_MIN +1)

#define RFPWRRANGE_NUM_REVD 3

#define SELE_CURRENT			(-1)


#define MAXSAVEDCH_G 			(6)
#define MAXSAVEDCH_A 			(16)

#define MAXSAVEDCH_G_REVD		(4)

#define MAX_LED 				(8)
#define ANNEX_LENGTH_INVALID	(0xFFFF)

#define CHIPID_8060 0x07
#define CHIPID_8688		(0x11)

#if defined(_W8787_) || defined(_W8782_)
#define XTALMAX			(0x7F)
#define XTALCENTER		(0x3F)
#define XTALMIN			(0x0)
#else	//#if defined(_W8787_)
#ifndef _W8689_
#define XTALMAX			(0xFF)
#define XTALCENTER		(0x7F)
#define XTALMIN			(0x0)
#endif //#ifndef _W8689_
#endif	//#if defined(_W8787_)

#define CHIPID_PW886		(0x8)
#define MAX_PM_RAIL_PW886		(7)


#define POW_MAX_DEF_G 		(20)
#define POW_MIN_DEF_G 		(0)

#define POWTBL_LEVEL 	POW_LEVEL_G
#define POWTBL_LEVEL_REV_D  (4)

#define DEFAULT_PWR_BANDG	14.0
#define DEFAULT_PWR_BANDA	14.0

#define RFPWRRANGE_MIN	0
#define RFPWRRANGE_MAX	3
#define RFPWRRANGE_NUM	(RFPWRRANGE_MAX - RFPWRRANGE_MIN +1)

#define RFPWRRANGE_NUM_REVD 3

#define MAXNUM_MIMODEVICE	1
enum  {MIMODEVICE0, MIMODEVICE1};
#define MAXNUM_TXPATH 		1
enum  {TXPATH_PATHA, TXPATH_PATHB, TXPATH_PATHC};
#define MAXNUM_RXPATH 		1
enum  {RXPATH_PATHA, RXPATH_PATHB, RXPATH_PATHC };
const char	PathName[MAXNUM_RXPATH] = 
{'A'
#if MAXNUM_RXPATH >1 
, 'B'
#endif

#if MAXNUM_RXPATH >2 
, 'C'
#endif
}; 

#if defined(_CAL_REV_D_)
#define NUM_SUBBAND_G_REVD 	(1)
#define NUM_SUBBAND_A_REVD 	(4)
#define NUM_SUBBAND_AG_REVD  (NUM_SUBBAND_A_REVD + NUM_SUBBAND_G_REVD)
#endif	//#if defined(_CAL_REV_D_)

#define NUM_SUBBAND_G 	(2)
#define NUM_SUBBAND_A 	(5)
  
#define CHCNT_MAX_G_N  		(9) 
#define CHCNT_MAX_G_LEGACY  (14)
#define CHCNT_MAX_G  (CHCNT_MAX_G_LEGACY +CHCNT_MAX_G_N)

#define CHCNT_MAX_A_ONLY 	(35)
#ifdef _SUPPORT_11J_
#define CHCNT_MAX_J_NBW		(9)
#define CHCNT_MAX_J_DBLCT	(3)
#else //#ifdef _SUPPORT_11J_
#define CHCNT_MAX_J_NBW		(0)
#define CHCNT_MAX_J_DBLCT	(0)
#endif //#ifdef _SUPPORT_11J_

#define CHCNT_MAX_A_N  		(11) 
#define CHCNT_MAX_A 		(CHCNT_MAX_A_ONLY + CHCNT_MAX_J_NBW-CHCNT_MAX_J_DBLCT+CHCNT_MAX_A_N)

#define MAXNUM_BW 			(4)
enum  
{
	CHANNEL_BW_11N_20MHZ, 
	CHANNEL_BW_11N_40MHZ, 
	CHANNEL_BW_11N_10MHZ, 
	CHANNEL_BW_11N_05MHZ
};
 

enum 
{
	ACTSUBCH_40MHZ_LOWER, 
	ACTSUBCH_40MHZ_UPPER, 
	ACTSUBCH_40MHZ_BOTH0, 
	ACTSUBCH_40MHZ_BOTH
};
 
  
#define LCCAPMIN 			0
#define LCCAPMAX 			7
 

#define MAX_TXIQCTRL		(0x7F)
#define MIN_TXIQCTRL		(-0x7F)
#define MASK_TXIQCTRL		(MAX_TXIQCTRL - MIN_TXIQCTRL +1)

#define MAX_TXLOSPURSCTRL	(0x1F)
#define MIN_TXLOSPURSCTRL	(0x00)
#define DEF_TXLOSPURSCTRL	(0x0F)

#define MAX_RXGAINPATHDELTA	(0x0F)
#define MIN_RXGAINPATHDELTA	(0x00)
#define DEF_RXGAINPATHDELTA_0DB	(0x07)

enum 
{
	DATARATE_1M, 		//	0	// data rate 1	Mbps
	DATARATE_2M, 		//	1	// data rate 2	Mbps
	DATARATE_5M, 		//	2	// data rate 5.5Mbps
	DATARATE_11M, 		//	3	// data rate 11 Mbps
	DATARATE_22M, 		//	4	// data rate 22 Mbps
	DATARATE_6M, 		//	5	// data rate 6	Mbps
	DATARATE_9M, 		//	6	// data rate 9	Mbps
	DATARATE_12M, 		//	7	// data rate 12 Mbps
	DATARATE_18M, 		//	8	// data rate 18 Mbps
	DATARATE_24M, 		//	9	// data rate 24 Mbps
	DATARATE_36M, 		//	10	// data rate 36 Mbps
	DATARATE_48M, 		//	11	// data rate 48 Mbps
	DATARATE_54M, 		//	12	// data rate 54 Mbps
	DATARATE_72M, 		//	13	// data rate 72 Mbps 
	DATARATE_MCS0, 		//	14	// data rate MCS0
	DATARATE_MCS1, 		//	15	// data rate MCS1
	DATARATE_MCS2, 		//	16	// data rate MCS2
	DATARATE_MCS3, 		//	17	// data rate MCS3
	DATARATE_MCS4, 		//	18	// data rate MCS4
	DATARATE_MCS5, 		//	19	// data rate MCS5
	DATARATE_MCS6, 		//	20	// data rate MCS6
	DATARATE_MCS7, 		//	21	// data rate MCS7
	DATARATE_MCS8, 		//	22	// data rate MCS8
	DATARATE_MCS9, 		//	23	// data rate MCS9
	DATARATE_MCS10, 	//	24	// data rate MCS10
	DATARATE_MCS11, 	//	25	// data rate MCS11
	DATARATE_MCS12, 	//	26	// data rate MCS12
	DATARATE_MCS13, 	//	27	// data rate MCS13
	DATARATE_MCS14, 	//	28	// data rate MCS14
	DATARATE_MCS15 		//	29	// data rate MCS15
};

enum PWRMODE {PWRMODE_ACTIDOL, PWRMODE_PWRSAVE_LONG,  PWRMODE_PWRDWN, PWRMODE_STDBY, PWRMODE_PWRSAVE_SHORT};  

//#define MODE_G_RFCH_DEFAULT	 0
#ifdef _SUPPORT_11J_
#define MODE_J_10MHZ_INDICATOR	(0x1000)
#endif //#ifdef _SUPPORT_11J_


#define MODE_G_RFCH_1		(1)	
#define MODE_G_RFCH_2		(2) 
#define MODE_G_RFCH_3		(3)
#define MODE_G_RFCH_4		(4)
#define MODE_G_RFCH_5		(5)
#define MODE_G_RFCH_6		(6)
#define MODE_G_RFCH_7		(7)
#define MODE_G_RFCH_8		(8)
#define MODE_G_RFCH_9		(9)
#define MODE_G_RFCH_10		(10)
#define MODE_G_RFCH_11		(11)
#define MODE_G_RFCH_12		(12)
#define MODE_G_RFCH_13		(13)
#define MODE_G_JP_RFCH_14	(14)
#define MODE_N_RFCH_15		(15)
#define MODE_N_RFCH_26		(26)
#define MODE_N_RFCH_37		(37)
#define MODE_N_RFCH_48		(48)
#define MODE_N_RFCH_59		(59)
#define MODE_N_RFCH_610		(610)
#define MODE_N_RFCH_711		(711)
#define MODE_N_RFCH_812		(812)
#define MODE_N_RFCH_913		(913)


#define MODE_A_RFCH_36		(36)	
#define MODE_A_RFCH_40		(40)
#define MODE_A_RFCH_44		(44)
#define MODE_A_RFCH_48		(48)
#define MODE_A_RFCH_52		(52)
#define MODE_A_RFCH_56		(56)
#define MODE_A_RFCH_60		(60)
#define MODE_A_RFCH_64		(64)
	
#define MODE_H_RFCH_100		(100)
#define MODE_H_RFCH_104		(104)
#define MODE_H_RFCH_108 	(108)
#define MODE_H_RFCH_112 	(112)
#define MODE_H_RFCH_116 	(116)
#define MODE_H_RFCH_120 	(120)
#define MODE_H_RFCH_124 	(124)
#define MODE_H_RFCH_128 	(128)
#define MODE_H_RFCH_132 	(132)
#define MODE_H_RFCH_136 	(136)
#define MODE_H_RFCH_140 	(140)
#define MODE_H_RFCH_149 	(149)
#define MODE_H_RFCH_153 	(153)
#define MODE_H_RFCH_157 	(157)	
#define MODE_H_RFCH_161 	(161)	
#define MODE_H_RFCH_165 	(165)	

#define MODE_A_JP_RFCH_8 	(8)	
#define MODE_A_JP_RFCH_12 	(12)
#define MODE_A_JP_RFCH_16 	(16)
#define MODE_A_JP_RFCH_34 	(34)
#define MODE_A_JP_RFCH_38 	(38)
#define MODE_A_JP_RFCH_42 	(42)
#define MODE_A_JP_RFCH_46 	(46) 

#define MODE_A_JP_RFCH_184 	(184)
#define MODE_A_JP_RFCH_188 	(188)
#define MODE_A_JP_RFCH_192 	(192)
#define MODE_A_JP_RFCH_196 	(196)

#ifdef _SUPPORT_11J_
#define MODE_J_JP_RFCH_7	(7|MODE_J_10MHZ_INDICATOR)
#define MODE_J_JP_RFCH_8	(8|MODE_J_10MHZ_INDICATOR)
#define MODE_J_JP_RFCH_11	(11|MODE_J_10MHZ_INDICATOR)
#define MODE_J_JP_RFCH_183	(183|MODE_J_10MHZ_INDICATOR)
#define MODE_J_JP_RFCH_184	(184|MODE_J_10MHZ_INDICATOR)
#define MODE_J_JP_RFCH_185	(185|MODE_J_10MHZ_INDICATOR)
#define MODE_J_JP_RFCH_187	(187|MODE_J_10MHZ_INDICATOR)
#define MODE_J_JP_RFCH_188	(188|MODE_J_10MHZ_INDICATOR)
#define MODE_J_JP_RFCH_189	(189|MODE_J_10MHZ_INDICATOR)
#endif //#ifdef _SUPPORT_11J_

#define MODE_N_RFCH_3640	(3640)
#define MODE_N_RFCH_4448	(4448)
#define MODE_N_RFCH_5256	(5256)
#define MODE_N_RFCH_6064	(6064)
#define MODE_N_RFCH_149153	(149153)
#define MODE_N_RFCH_157161	(157161)
#define MODE_N_RFCH_100104	(100104)
#define MODE_N_RFCH_108112	(108112)
#define MODE_N_RFCH_116120	(116120)
#define MODE_N_RFCH_124128	(124128)
#define MODE_N_RFCH_132136	(132136)

//DPD
#define DPD_COEFF_ENTRY_NUM (3)
typedef struct THERMALCALPARMS_VD
{
    char BandID:4;
	char SubBand:4;
	char RSSICalNum_normal;
	char TempSlope;
	char TempCalTime;
	char RSSICalNum_bypass;
	char Numerator_hot;
	char Denom;
	char PPAConefficient;
	char PAConefficient;
	char XTAL;
	char PTargetDelta;
	char Numerator_cold;
}THERMALCALPARMS_VD;

#define RSSICAL_DENUM	1000

typedef struct DPD_Sub_ConeffcicentData
{
	char			RealHi:8;
	char			RealLo:4;
	char			ImageHi:4;
	char			ImageLo:8;
	char			Reserved:8;
}DPD_Sub_ConeffcicentData; 

typedef struct DPD_ConeffcicentData
{
	//char			PowerTarget:4;
	//char			PowerCode:4;
	char			DPDIdx;
	char			PowerCode;
	char			StartChan;
	char			EndChan;
	DPD_Sub_ConeffcicentData ConeffcicentData[3];
}DPD_ConeffcicentData;

#ifdef _DUTAPIDLLMAIN_
extern const int BandG_ChIndex[CHCNT_MAX_G];
extern const int BandA_ChIndex[CHCNT_MAX_A];
#else
const int BandG_ChIndex[CHCNT_MAX_G]=
{
MODE_G_RFCH_1 ,
MODE_G_RFCH_2 , 
MODE_G_RFCH_3 ,
MODE_G_RFCH_4 ,
MODE_G_RFCH_5 ,
MODE_G_RFCH_6 ,
MODE_G_RFCH_7 ,
MODE_G_RFCH_8 ,
MODE_G_RFCH_9 ,
MODE_G_RFCH_10 ,
MODE_G_RFCH_11 ,
MODE_G_RFCH_12 ,
MODE_G_RFCH_13 ,
MODE_G_JP_RFCH_14,  
MODE_N_RFCH_15 ,
MODE_N_RFCH_26 ,
MODE_N_RFCH_37 ,
MODE_N_RFCH_48 ,
MODE_N_RFCH_59 ,
MODE_N_RFCH_610 ,
MODE_N_RFCH_711,
MODE_N_RFCH_812,
MODE_N_RFCH_913
};

const int BandA_ChIndex[CHCNT_MAX_A+CHCNT_MAX_J_NBW]=
{
MODE_A_RFCH_36 ,	
MODE_A_RFCH_40 ,	
MODE_A_RFCH_44 ,	
MODE_A_RFCH_48 ,	
MODE_A_RFCH_52 ,	
MODE_A_RFCH_56 ,	
MODE_A_RFCH_60 ,	
MODE_A_RFCH_64 ,
	
MODE_H_RFCH_100 ,	
MODE_H_RFCH_104 ,	
MODE_H_RFCH_108 ,	
MODE_H_RFCH_112 ,	
MODE_H_RFCH_116 ,	
MODE_H_RFCH_120 ,	
MODE_H_RFCH_124 ,	
MODE_H_RFCH_128 ,	
MODE_H_RFCH_132 ,	
MODE_H_RFCH_136 ,	
MODE_H_RFCH_140 ,	
MODE_H_RFCH_149 ,	
MODE_H_RFCH_153 ,
MODE_H_RFCH_157 ,	
MODE_H_RFCH_161 ,	
MODE_H_RFCH_165 ,	

MODE_A_JP_RFCH_8 ,
MODE_A_JP_RFCH_12 ,
MODE_A_JP_RFCH_16 ,
MODE_A_JP_RFCH_34 ,
MODE_A_JP_RFCH_38 ,
MODE_A_JP_RFCH_42 ,
MODE_A_JP_RFCH_46 , 

MODE_A_JP_RFCH_184 ,
MODE_A_JP_RFCH_188 ,
MODE_A_JP_RFCH_192 ,
MODE_A_JP_RFCH_196 ,

#ifdef _SUPPORT_11J_
MODE_J_JP_RFCH_7,		
MODE_J_JP_RFCH_8,		
MODE_J_JP_RFCH_11,		
MODE_J_JP_RFCH_183,		
MODE_J_JP_RFCH_184,		
MODE_J_JP_RFCH_185,		
MODE_J_JP_RFCH_187,		
MODE_J_JP_RFCH_188,		
MODE_J_JP_RFCH_189,		
#endif //#ifdef _SUPPORT_11J_
 
MODE_N_RFCH_3640, 
MODE_N_RFCH_4448,
MODE_N_RFCH_5256,
MODE_N_RFCH_6064,
MODE_N_RFCH_149153,
MODE_N_RFCH_157161,
MODE_N_RFCH_100104,
MODE_N_RFCH_108112,
MODE_N_RFCH_116120,
MODE_N_RFCH_124128,
MODE_N_RFCH_132136,

};
#endif
#define CHANNEL_NUM_A 41
const int aChannelList[CHANNEL_NUM_A] =
	{	36,  40,  44,  48,  52,  56,  60,  64, 
		149, 153, 157, 161, 165, 
		183, 184, 185, 187, 188, 189, 192, 196,
		7, 8, 11, 12, 16, 34, 38, 42, 46,
		100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140};

// DPD table info
#define HAL_DPD_MAX_NUM_COEFF    18

typedef struct hal_dpd_coeff {
    // This structure could be memory mapped but 
    // since this will be cached, we want to save
    // memory. Hence we need to do a bit of data
    // massaging and not directly copy from BB memory
    unsigned int real_hi:8;
    unsigned int real_lo:4;
    unsigned int imag_hi:4;
    unsigned int imag_lo:8;
    unsigned int resv:8; // reserved for storing some status
} HAL_DPD_COEFF;

// LNA mode
#define LNA_MODE_AUTO_CHECK			0x0
#define LNA_MODE_SHARED_LNA			0x0001
#define LNA_MODE_BYPASS				0x0002
#define LNA_MODE_FORCED_SHARED_LNA	0x0101
#define LNA_MODE_FORCED_BYPASS		0x0102

#endif //_DUTAPI_HC_
