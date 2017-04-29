// #include "DutApi.hc"
#ifndef  _DUTAPI_HC_
#define _DUTAPI_HC_

//#define   DUTCTRL_VERSION 0x01040000  
//#define   DUTCTRL_VERSION 0x01030800          //"1.3.7.2" Menu add/remove 
//#define DUTCTRL_VERSION 0x01030710        //"1.3.7.1" Menu add/remove 
//#define DUTCTRL_VERSION 0x01030700		//"1.3.7.0"	Add USB interface support
//#define DUTCTRL_VERSION 0x01030600		//"1.3.6.0"	In cmd 54, annex 15 do not need to be swapped
//#define DUTCTRL_VERSION 0x01030500		//"1.3.5.0"	NO EEPROM Support
//#define DUTCTRL_VERSION 0x01030400		//"1.3.4.0"	Save OR Version to cal table.  Display OR version in cmd 88
											//			in cmd 53/54 save a band table as well.
											//			added annex 42 to save openloop calibration parameters
											//			enable openloop calibration for BlueJay			
//#define DUTCTRL_VERSION 0x01030300		//"1.3.3.0"	Add direct access of MAC Address in MAC Reg
//#define DUTCTRL_VERSION 0x01030201		//"1.3.2.1"	Update Fast cal command test content
//#define DUTCTRL_VERSION 0x01030200	//"1.3.2.0"	Support 9bit BBU address for 8688
//#define DUTCTRL_VERSION 0x01030100	//"1.3.1.0"	Added support for Thermal calibration
//#define DUTCTRL_VERSION 0x01030000	//"1.3.0.0"	Added three functions for fast calibration and verification test
//#define DUTCTRL_VERSION 0x01020B00	//"1.2.11.0"	Support for 8688 Annex 32 for A band

//#define DUTCTRL_VERSION 0x01020a00	//"1.2.10.0"	Support for 8686 C2
												//	Calibration for A band (8688)	.
//#define DUTCTRL_VERSION 0x01020900	//"1.2.9.0"	RSSI Calibration.
//#define DUTCTRL_VERSION 0x01020801	//"1.2.8.1"	Fixed cmd 54 to save data for annex 37 correctly.
//#define DUTCTRL_VERSION 0x01020800	//"1.2.8.0"	Added access to PM register and set PM Rail.
//#define DUTCTRL_VERSION 0x01020701	//"1.2.7.1"	Change the wording fro LDO setting to External LDO setting
//#define DUTCTRL_VERSION 0x01020700	//"1.2.7.0"	Add LDO enable/disable functionality
//#define DUTCTRL_VERSION 0x01020600	//"1.2.6.0"	Modified the cal_data_ext.conf file format according to Production team request
//#define DUTCTRL_VERSION 0x01020501	//"1.2.5.1"	Added annex 37 and 31 for cmd 53 and 54 for Linux version.
											//		Add new function GetPrivateProfileSection Linux Version
//#define DUTCTRL_VERSION 0x01020500	//"1.2.5"	Added annex 37 and 31 for cmd 53 and 54.
//#define DUTCTRL_VERSION 0x01020400	//"1.2.4"	Added access to annex table 37 for Tx Calibration for G rate
//#define DUTCTRL_VERSION 0x01020300	//"1.2.3"	Next pointer for NO_EEPROM is wrong in annex 26,21,and 16
//#define DUTCTRL_VERSION 0x01020200	//"1.2.2"	Bring out the interface for XTAL set/get
										//			Added menu item for set/reset TxIQCal Mode
//#define DUTCTRL_VERSION 0x01020100	//"1.2.1"	Swap the IQ mismatch cal data for ch1-13 and ch14
//#define DUTCTRL_VERSION 0x01020000	//"1.2.0"	Support Annex table 31.
//#define DUTCTRL_VERSION 0x01010300	//"1.1.3"	Move Close loop cal from mfgtool to dll.
//#define DUTCTRL_VERSION 0x01010201	//"1.1.2"	Added function to enable user to specify the 							out put file name 
					//		Swap the mismatch cal data.
//#define DUTCTRL_VERSION 0x01010100	//"1.1.1"


#define _SUPPORT_11J_
#include "../../../DutCommon/Dut_error.hc"

#define LEN_MACADDRESS (6)

#define LEN_MACADDRESS_VENDOR (3)

#define MAXNUM_MACADDRESS (32)

#define MAXNUM_SWTCONFIG (0x40)

#define VMR_OFFSET_SECONDARYMACADDRESS	(0x100)

#define VMR_MAX_MULTIMACADDRESS		MAXNUM_MACADDRESS

#define VMR_MAX_SECONDARYMACADDRESS	(VMR_MAX_MULTIMACADDRESS-1)

/* 
#define ERROR_INPUT_INVALID -1
#define ERROR_NOT_IMPLEMENTED   0
#define ERROR_NONE			0
#define ERROR_ERROR			1
#define ERROR_MISMATCH		-2
#define ERROR_READY_TIMEOUT		2
#define ERROR_FRAMEEXPIRE	3
#define ERROR_DATANOTEXIST 5
*/ 

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

#define POW_MAX_A 20
#define POW_MIN_A 0 
#define POW_LEVEL_A (POW_MAX_A - POW_MIN_A + 1)

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

#define SELE_CURRENT			(-1)

//#define MAX_SETTING_PAIRS 		(10)
//#define MAX_VPSTR_SIZE			(64)

#define MAXSAVEDCH_G 			(6)
#define MAXSAVEDCH_A 			(16)

//#define ANNEX_ADDR_END 			(0xFFFFFFFF)
//#define ANNEX_ADDR_BLANK 		(0x0)
#define MAX_LED 				(8)
#define ANNEX_LENGTH_INVALID	(0xFFFF)

//#define CHIPID_8010 0x01
//#define CHIPID_8015 0x05
//#define CHIPID_8020 0x02
//#define CHIPID_8030 0x03
//#define CHIPID_8035 0x06
#define CHIPID_8060 0x07
#define CHIPID_8688		(0x11)

#ifndef _W8689_
#define XTALMAX			(0xFF)
#define XTALCENTER		(0x7F)
#define XTALMIN			(0x0)
#endif //#ifndef _W8689_

#define CHIPID_PW886		(0x8)
#define MAX_PM_RAIL_PW886		(8)


#define POW_MAX_DEF_G 		(20)
#define POW_MIN_DEF_G 		(0)
//#define POW_LEVEL_G		(POW_MAX_DEF_G - POW_MIN_DEF_G + 1)

//#define POW_MAX_DEF_A 		(20)
//#define POW_MIN_DEF_A 		(0) 
//#define POW_LEVEL_A		(POW_MAX_DEF_A - POW_MIN_DEF_A + 1)

#define POWTBL_LEVEL 	POW_LEVEL_G

#define DEFAULT_PWR_BANDG	14.0
#define DEFAULT_PWR_BANDA	14.0

#define RFPWRRANGE_MIN	0
#define RFPWRRANGE_MAX	3
#define RFPWRRANGE_NUM	(RFPWRRANGE_MAX - RFPWRRANGE_MIN +1)

#define MAXNUM_MIMODEVICE	1
enum  {MIMODEVICE0, MIMODEVICE1};
#define MAXNUM_TXPATH 		1
enum  {TXPATH_PATHA, TXPATH_PATHB};
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

//#define NUM_BAND 		(2)
#define NUM_SUBBAND_G 	(2)
#define NUM_SUBBAND_A 	(5)

//enum  {BAND_802_11G, BAND_802_11A };
//const char	BandName[NUM_BAND] = {'G', 'A'}; 
  
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
 

#define MAX_TXIQCTRL		(0x1F)
#define MIN_TXIQCTRL		(-0x1F)
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

 
//enum ANTMODE {ANTMODE_ANT1=1, ANTMODE_ANT2, ANTMODE_DYNAMIC}; 
//enum RSSISRC {RSSISRC_ANT1=1, RSSISRC_ANT2, RSSISRC_DYNAMIC}; 


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


#endif //_DUTAPI_HC_
