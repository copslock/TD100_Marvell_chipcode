/** @file DutCalFileHandle.cpp
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#if !defined(_NO_EEPROM_)
#include "../DutCommon/Dut_error.hc"
#include "../DutCommon/utilities_os.h"
#include "DutBtApi878XDll.h"
#include "DutWlanApi878XDll.h"
#include "../DutCommon/utilities.h"
#include "../DutCommon/mathUtility.h"
#include "../DutCommon/utilities_os.h"

char BtCalFileName[255]="";
char BtCalDownLoadFileName[255]="CalBtDataFile.txt";
char BtCalUploadFileName[255]="CalBtDataFile_Upload.txt";
const char Def_BtCalDownLoadFileName[255]="CalBtDataFile.txt";
const char Def_BtCalUploadFileName[255]="CalBtDataFile_Upload.txt";
 
char WlanCalFileName[255]="";
char WlanCalPwrTFileName[255]="";
char WlanCalDownLoadFileName[255]="CalWlanDataFile.txt";
char WlanCalUploadFileName[255]="CalWlanDataFile_Upload.txt";
const char Def_WlanCalDownLoadFileName[255]="CalWlanDataFile.txt";
const char Def_WlanCalUploadFileName[255]="CalWlanDataFile_Upload.txt";

#if defined (_MIMO_)
char WlanCalMapFileNameFormat[255]="";
char WlanCalMapFileName[255]="";
char WlanCalDownLoadMapFileNameFormat[255]="WlanPwrTargetMap_CC%X_Dev%d.txt";
char WlanCalUploadMapFileNameFormat[255]="WlanPwrTargetMap_CC%X_Dev%d_Upload.txt";
const char Def_WlanCalDownLoadMapFileNameFormat[255]="WlanPwrTargetMap_CC%X_Dev%d.txt";
const char Def_WlanCalUploadMapFileNameFormat[255]="WlanPwrTargetMap_CC%X_Dev%d_Upload.txt";
#endif // #if defined (_MIMO_)

int LoadSaveCalMain(char *FileName);
int UpLoadSaveCalMain(char *FileName);

//REV D
int UpLoadSaveCalAnnex42_VD(char *FileName);
int LoadSaveCalAnnex42_VD(char *FileName);
#if defined (_WLAN_)
int UpLoadSaveCalAnnex49(char *FileName 
#if defined (_MIMO_)
				,	int	DeviceId,	int PathId 		
#endif // #if defined (_MIMO_) 
					   );
int LoadSaveCalAnnex49(char *FileName
#if defined (_MIMO_)
				,	int	DeviceId,	int PathId 		
#endif // #if defined (_MIMO_) 
);

int UpLoadSaveCalAnnex50(char *FileName, 
#if defined (_MIMO_)
				DWORD DeviceId, DWORD PathId,
#endif //#if defined (_MIMO_)
					DWORD BandId, DWORD SubBandId);

int LoadSaveCalAnnex50(char *FileName,
#if defined (_MIMO_)
				DWORD	DeviceId,	DWORD PathId, 		
#endif // #if defined (_MIMO_) 
					 DWORD BandId, DWORD SubBandId);
#endif //#if defined (_WLAN_)

int LoadSaveCalAnnex52(char *FileName
#if defined (_MIMO_)
				,	int	DeviceId,	int PathId 		
#endif // #if defined (_MIMO_) 
);

int LoadSaveCalAnnex27(char *FileName);
int UpLoadSaveCalAnnex27(char *FileName);

int LoadSaveCalAnnex31(char *FileName
#if defined (_MIMO_)
				,	DWORD	DeviceId 	
#endif // #if defined (_MIMO_)
				);
int UpLoadSaveCalAnnex31(char *FileName
#if defined (_MIMO_)
				,	DWORD	DeviceId 	
#endif // #if defined (_MIMO_)
				);
int LoadSaveCalAnnex32(char *FileName
#if defined (_MIMO_)
				,	DWORD	DeviceId 	
#endif // #if defined (_MIMO_)
				);
int UpLoadSaveCalAnnex32(char *FileName
#if defined (_MIMO_)
				,	DWORD	DeviceId 	
#endif // #if defined (_MIMO_)
				);

int UpLoadSaveCalAnnex52( char *FileName,	DWORD	DeviceId, int PathId );
int UpLoadSaveCalAnnex53( char *FileName,	DWORD	DeviceId, int PathId );
int UpLoadSaveCalAnnex54( char *FileName,	DWORD	DeviceId, int PathId );
int UpLoadSaveCalAnnex55( char *FileName);
int UpLoadSaveCalAnnex56( char *FileName);
 
int UpLoadSaveCalAnnex57( char *FileName ,	
#if defined (_MIMO_)
						 DWORD	DeviceId, int PathId,	
#endif // #if defined (_MIMO_)
						  BYTE BandId, BYTE SubbandId);

int LoadSaveCalAnnex57(char *FileName,	
#if defined (_MIMO_)
				int	DeviceId,	int PathId,	
#endif // #if defined (_MIMO_) 
				BYTE BandId, BYTE SubbandID);
 
int LoadSaveCalAnnex42(char *FileName);
int UpLoadSaveCalAnnex42( char *FileName);

int LoadSaveCalAnnex40(char *FileName);
int UpLoadSaveCalAnnex40( char *FileName);
int LoadSaveCalAnnex45(char *FileName);
int UpLoadSaveCalAnnex45( char *FileName);

int LoadSaveCalData(int DeviceType=0,
 					char *BtCalFlexFileName=NULL,
					char *WlanCalFlexFileName=NULL
#if defined (_MIMO_)
					,char *WlanCalFlexPwrTrgtFileFormat=NULL	
#endif // #if defined (_MIMO_)
					);
int UpdateSaveBtCalData(int DeviceType=0, 
					char *BtCalFlexFileName=NULL);

int UpLoadSaveCalData(int *CsC,
					  int DeviceType=0,
					  char *BtCalFlexFileName=NULL,  
					  char *WlanCalFlexFileName=NULL
#if defined (_MIMO_)
					  , 
 					  char *WlanCalFlexPwrTrgtFileFormat=NULL
#endif // #if defined (_MIMO_)
					  );
int UpLoadHwInfoData(void); 

void resetDownLoadFileNames(void) 
{
	strcpy(WlanCalDownLoadFileName, Def_WlanCalDownLoadFileName);
#if defined (_MIMO_)
	strcpy(WlanCalDownLoadMapFileNameFormat, Def_WlanCalDownLoadMapFileNameFormat);
#endif // #if defined (_MIMO_)
	strcpy(BtCalDownLoadFileName, Def_BtCalDownLoadFileName);
}
void resetUpLoadFileNames(void) 
{
	strcpy(WlanCalUploadFileName, Def_WlanCalUploadFileName);
#if defined (_MIMO_)
	strcpy(WlanCalUploadMapFileNameFormat, Def_WlanCalUploadMapFileNameFormat);
#endif // #if defined (_MIMO_)
	strcpy(BtCalUploadFileName, Def_BtCalUploadFileName);
}

int LoadSaveCalMain(char *FileName)
{
	int  tempInt=0, i=0, j=0; 
	char line[256]="", key[256]="";

	int status=0;
	BYTE	PcbRev_DesignType=0,	PcbRev_MajorRev=0;
	BYTE	PcbRev_MinorRev=0,		PcbRev_Variation=0;

	bool ExtPaPolar_neg_G[MAXNUM_MIMODEVICE] ={1
#if MAXNUM_MIMODEVICE == 2
		,1  
#endif //#if MAXNUM_MIMODEVICE == 2
	}; 
	bool ExtPaPolar_neg_A[MAXNUM_MIMODEVICE] ={1
#if MAXNUM_MIMODEVICE == 2
		,1
#endif //#if MAXNUM_MIMODEVICE == 2
	};

	BYTE FemConfId[MAXNUM_MIMODEVICE]={0}; 

  	BYTE	StructRev=0x0C; 
	BYTE	PAThermaScaler=0x2; 
	BYTE	AssemblyHouse=0, SpiSize=0, SpiUnit=0;
	DWORD	Sn=0; 
	WORD	SocId=0; 
	BYTE	SocVer=0, BbuId=0, BbuVer=0, RfId=0, RfVer=0; 
	DWORD	TestToolVersion=0, MfgTaskVersion =0, DllVersion=0;  
	BYTE	BT_Coexist =13;
	BYTE	AntTx[MAXNUM_MIMODEVICE]={0x03
#if MAXNUM_MIMODEVICE == 2
	, 0x03
#endif //#if MAXNUM_MIMODEVICE == 2
	};
	BYTE AntRx[MAXNUM_MIMODEVICE]={0x07
#if MAXNUM_MIMODEVICE == 2
		, 0x07
#endif //#if MAXNUM_MIMODEVICE == 2
	};
	BYTE	AntId[MAXNUM_MIMODEVICE][MAXNUM_RXPATH]={0};
 	BYTE	CC=0x10;
	BYTE	ExtXtalSource=0,	ExtSleepClk=0, WlanWakeUp=0;
	bool	ExtLna_G[MAXNUM_MIMODEVICE]={0};
	bool	ExtLna_A[MAXNUM_MIMODEVICE]={0}; 
	bool	ExtLnaPolar_G[MAXNUM_MIMODEVICE]={0}; 
	bool	ExtLnaPolar_A[MAXNUM_MIMODEVICE]={0}; 
	BYTE	LnaGainDelta=0;
//	BYTE	LnaId[MAXNUM_MIMODEVICE][MAXNUM_RXPATH]={0};
//	BYTE	LnaId_A[MAXNUM_MIMODEVICE][MAXNUM_RXPATH]={0};  
	BYTE	AppModeFlag=3;
	BYTE	SocOrRev=0, RfuOrRev=0;

	BYTE	MimoTxCnt=2, MimoRxCnt=3;
	BYTE	MimoMap[MAXNUM_MIMODEVICE][MAXNUM_RXPATH]=
	{
		{0x00
#if MAXNUM_RXPATH >1
			,   0x01, 0x02
#endif //#if MAXNUM_RXPATH >1
		}
#if MAXNUM_MIMODEVICE == 2
	, {0x10, 0x11, 0x12}
#endif //#if MAXNUM_MIMODEVICE == 2
	}; 

	{
			BYTE	Capabilities_2G=0x02;
			BYTE	Capabilities_5G=0x05;
			BYTE	BTFrontEnd2G[9];
			BYTE	BTFrontEnd5G[9];
			BYTE	LNAGain=0x10;
			BYTE	AntDiversityInfo = 0;

			bool Capable_2G=0;//
			bool Capable_5G=0;//
			bool InternalBluetooth=0;	//
			bool LNA_2G=0;//
			bool AntennaDiversity=0;
			bool LNA_5G=0;
			bool SameAnt_2G_BT=0;
			bool SameAnt_5G_BT=0;

			bool PaOption_ExtLnaFlag_PathABC_G=0;
			bool PaOption_ExtLnaFlag_PathABC_A=0;
	
			bool PaOption_ExtPaPolar_PathABC_G=0;
			bool PaOption_ExtPaPolar_PathABC_A=0;

			BYTE EXT_BT_CODE=0;
			BYTE FEVersion=0x00;

		Dut_Shared_ReadMainDataFromFile_RevD(FileName,
			&StructRev,		&PcbRev_DesignType, &LnaGainDelta,
			&AntDiversityInfo, &FEVersion,

			&PaOption_ExtPaPolar_PathABC_G,
			&PaOption_ExtPaPolar_PathABC_A,

			&SpiSize,		 &CC,
			&ExtXtalSource,	&ExtSleepClk,		&WlanWakeUp, 	
			AntTx, AntRx,
			//Capabilities FEM
			//Capabilities FEM
			&Capable_2G, //
			&Capable_5G, //
			&InternalBluetooth,	//
			&AntennaDiversity,
			&LNA_2G,	//
			&LNA_5G,  //15
			&SameAnt_2G_BT,
			&SameAnt_5G_BT,

			&Capabilities_2G, &Capabilities_5G, BTFrontEnd2G, BTFrontEnd5G,
			&TestToolVersion, &MfgTaskVersion, 	&DllVersion,
			&SocOrRev,			&RfuOrRev,
			&MimoTxCnt, 		&MimoRxCnt,			MimoMap);
		
			StructRev =0x0D;

			return  Dut_Shared_SetCalMainDataRevD(
			&StructRev,		 &PcbRev_DesignType, &SpiSize, &LnaGainDelta, &AntDiversityInfo, &FEVersion, &CC,
			&ExtXtalSource,	&ExtSleepClk,		&WlanWakeUp, 	
			AntTx, AntRx,
	
			//Capabilities FEM
			&Capable_2G, //
			&Capable_5G, //
			&InternalBluetooth,	//
			&AntennaDiversity,
			&LNA_2G,	//
			&LNA_5G,
			&SameAnt_2G_BT,
			&SameAnt_5G_BT,

			&Capabilities_2G, &Capabilities_5G, BTFrontEnd2G, BTFrontEnd5G,
			&TestToolVersion, &MfgTaskVersion, 	&DllVersion,
			&SocOrRev,			&RfuOrRev,
			&MimoTxCnt, 		&MimoRxCnt,			MimoMap);
		} 
}

int UpLoadSaveCalMain(char *FileName)
{
	int  tempInt=0, i=0, j=0; 
 	char line[256]="", key[256]="";
	
	int status=0;
	BYTE	PcbRev_DesignType=0,	PcbRev_MajorRev=0;
	BYTE	PcbRev_MinorRev=0,		PcbRev_Variation=0;

	bool ExtPaPolar_neg_G[MAXNUM_MIMODEVICE] ={1
#if MAXNUM_MIMODEVICE == 2
		,1
#endif //#if MAXNUM_MIMODEVICE == 2
	};   
	bool ExtPaPolar_neg_A[MAXNUM_MIMODEVICE] ={1
#if MAXNUM_MIMODEVICE == 2
		,1
#endif //#if MAXNUM_MIMODEVICE == 2
	};
	bool	ExtLnaPolar_G[MAXNUM_MIMODEVICE]={0}; 
	bool	ExtLnaPolar_A[MAXNUM_MIMODEVICE]={0}; 
	BYTE FemConfId[MAXNUM_MIMODEVICE]={0}; 

  	BYTE StructRev=0x0C; 
	BYTE PAThermaScaler=0x2; 
	BYTE AssemblyHouse=0, SpiSize=0, SpiUnit=0;
	DWORD	Sn=0; 
	WORD	SocId=0; 
	BYTE	SocVer=0, BbuId=0, BbuVer=0, RfId=0, RfVer=0; 
	DWORD	TestToolVersion=0, MfgTaskVersion =0, DllVersion=0;  
	BYTE	BT_Coexist =13;
	BYTE	AntTx[MAXNUM_MIMODEVICE]={0x03
#if MAXNUM_MIMODEVICE == 2
		, 0x03
#endif //#if MAXNUM_MIMODEVICE == 2
	};
	BYTE AntRx[MAXNUM_MIMODEVICE]={0x07
#if MAXNUM_MIMODEVICE == 2
		, 0x07
#endif //#if MAXNUM_MIMODEVICE == 2
	};
	BYTE	AntId[MAXNUM_MIMODEVICE][MAXNUM_RXPATH]={0};
 	BYTE	CC=0x10;
	BYTE	ExtXtalSource=0,	ExtSleepClk=0, WlanWakeUp=0;
	bool	ExtLna_G[MAXNUM_MIMODEVICE]={0};
	bool	ExtLna_A[MAXNUM_MIMODEVICE]={0}; 
	BYTE	LnaGainDelta=0;
	BYTE	AppModeFlag=3;
	BYTE	SocOrRev=0, RfuOrRev=0;
	BYTE	MimoTxCnt=2, MimoRxCnt=3;
	BYTE	MimoMap[MAXNUM_MIMODEVICE][MAXNUM_RXPATH]=
	{
		{0x00
#if MAXNUM_RXPATH >1
			,   0x01, 0x02
#endif //#if MAXNUM_RXPATH >1

		}
#if MAXNUM_MIMODEVICE == 2
		, {0x10, 0x11, 0x12}
#endif //#if MAXNUM_MIMODEVICE == 2
	}; 
	bool Capable_2G=0, Capable_5G=0, InternalBluetooth=0,LNA_2G=0;
	BYTE Capabilities_2G_BT=0, Capabilities_5G_BT=0;
	BYTE	BTFrontEnd2G[9]={0};
	BYTE	BTFrontEnd5G[9]={0};
	bool	AntennaDiversity=0;
	bool    LNA_5G=0, SameAnt_2G_BT=0, SameAnt_5G_BT=0;
	BYTE	AntDiversityInfo=0;
	BYTE    FEVersion=0;
	bool	PaOption_ExtLnaFlag_PathABC_G=0;
	bool	PaOption_ExtLnaFlag_PathABC_A=0;
	
	bool	PaOption_ExtPaPolar_PathABC_G=0;
	bool	PaOption_ExtPaPolar_PathABC_A=0;

	status = Dut_Shared_GetCalMainDataRevD(
						&StructRev,
						&PcbRev_DesignType,
						&LnaGainDelta,
						&AntDiversityInfo,
						&FEVersion,

						&PaOption_ExtLnaFlag_PathABC_G,
						&PaOption_ExtLnaFlag_PathABC_A,
	
						&PaOption_ExtPaPolar_PathABC_G,
						&PaOption_ExtPaPolar_PathABC_A,

						&SpiSize,			
 						&CC,
						
						&ExtXtalSource,		&ExtSleepClk,		&WlanWakeUp,
						
						AntTx,		//
						AntRx,		//

						//Capabilities
						&Capable_2G, //
						&Capable_5G, //
						&InternalBluetooth,	//
						&AntennaDiversity,
						&LNA_2G,	//
						&LNA_5G,
						&SameAnt_2G_BT,
						&SameAnt_5G_BT,
						
						//Concurrency
						&Capabilities_2G_BT,
						&Capabilities_5G_BT,

						BTFrontEnd2G,
						BTFrontEnd5G,	
					
						&TestToolVersion,	&MfgTaskVersion,	
						&DllVersion, 
						&SocOrRev, &RfuOrRev,
						&MimoTxCnt, 	&MimoRxCnt, 
						MimoMap);
	if(status)	return status ;

	status = Dut_Shared_WriteMainDataToFile_RevD(FileName,
						StructRev,
						PcbRev_DesignType,
						LnaGainDelta,
						AntDiversityInfo,
						FEVersion,
						
						PaOption_ExtLnaFlag_PathABC_G,
						PaOption_ExtLnaFlag_PathABC_A,
	
						PaOption_ExtPaPolar_PathABC_G,
						PaOption_ExtPaPolar_PathABC_A,


						SpiSize,
 						CC,
						
						ExtXtalSource,		ExtSleepClk,		WlanWakeUp,
						
						AntTx,		//
						AntRx,		//

						//Capabilities
						&Capable_2G, //
						&Capable_5G, //
						&InternalBluetooth,	//
						&AntennaDiversity,
						&LNA_2G,	//
						&LNA_5G,
						&SameAnt_2G_BT,
						&SameAnt_5G_BT,
						
						//Concurrency
						Capabilities_2G_BT,
						Capabilities_5G_BT,

						BTFrontEnd2G,
						BTFrontEnd5G,	
					
						TestToolVersion,	MfgTaskVersion,	
						DllVersion, 
						SocOrRev, RfuOrRev,
						MimoTxCnt, 	MimoRxCnt, 
						MimoMap);

	return status ;
}

int LoadSaveCalAnnex27(char *FileName)
{
	int  tempInt=0, i=0, j=0; 
	char line[256]="", tag[256]="", key[256]="";
	int status=0;

	BYTE Led[8]={0}; 



	status = Dut_Shared_ReadAnnex27DataFromFile(FileName, Led);
	if(status) return status;
	
	status = Dut_Shared_SetAnnexType27Data(Led);

	return status;
}

int UpLoadSaveCalAnnex27( char *FileName)
{
	int  tempInt=0, i=0, j=0; 
	char line[256]="", tag[256]="", key[256]="";
	int status=0;

	BYTE Led[8]={0}; 

	status = Dut_Shared_GetAnnexType27Data(Led);
	if(ERROR_DATANOTEXIST == status && !Dut_Shared_GetWriteTemplateFlag())  
	{
		status =ERROR_NONE;
 		return status ;
	}
	else
	{	

		if(!Dut_Shared_GetWriteTemplateFlag()) 
		{
			if(status)	return status ;
		}
		else 
			status =ERROR_NONE;

 		Dut_Shared_WriteAnnex27DataToFile(FileName, Led);
	}

	return status;
}


int LoadSaveCalAnnex28(char *FileName)
{
	int  tempInt=0, i=0, j=0; 
	char line[256]="", tag[256]="", key[256]="";
	int status=0;

	BYTE UsbVendorString[MAX_LENGTH]={0}, UsbProductString[MAX_LENGTH]={0}; 



	status = Dut_Shared_ReadAnnex28DataFromFile(FileName,
										UsbVendorString, 
										UsbProductString);
	if(status) return status;
	
	status = Dut_Shared_SetAnnexType28Data(UsbVendorString, 
											UsbProductString);

	return status;
}

int UpLoadSaveCalAnnex28( char *FileName)
{
	int  tempInt=0, i=0, j=0; 
	char line[256]="", tag[256]="", key[256]="";
	int status=0;

	BYTE UsbVendorString[MAX_LENGTH]={0}, UsbProductString[MAX_LENGTH]={0}; 

	status = Dut_Shared_GetAnnexType28Data(UsbVendorString, 
											UsbProductString);

	if(ERROR_DATANOTEXIST == status && !Dut_Shared_GetWriteTemplateFlag())  
	{
		status =ERROR_NONE;
 		return status ;
	}
	else
	{	

		if(!Dut_Shared_GetWriteTemplateFlag()) 
		{
			if(status)	return status ;
		}
		else 
			status =ERROR_NONE;

 		Dut_Shared_WriteAnnex28DataToFile(FileName, 
										UsbVendorString, 
										UsbProductString);


	}

	return status;
}

int LoadSaveCalAnnex31(char *FileName, DWORD DeviceId)
{
	int  tempInt=0, i=0, j=0; 
	char line[256]="", tag[256]="", key[256]="";
	int status=0;
	char Offset_Nf[NUM_SUBBAND_G]={0},		Offset_CCK[NUM_SUBBAND_G]={0}; 
	char Offset_OFDM[NUM_SUBBAND_G]={0},	Offset_MCS[NUM_SUBBAND_G]={0}; 



	status = Dut_Shared_ReadAnnex31DataFromFile(FileName,
#if defined (_MIMO_)
						DeviceId,  
#endif	// #if defined (_MIMO_)
						Offset_Nf, Offset_CCK, 
						Offset_OFDM, Offset_MCS);  
	if(status) return status;

	status = Dut_Shared_SetAnnexType31Data(
#if defined (_MIMO_)
									DeviceId, 
#endif // #if defined (_MIMO_)
									Offset_Nf, Offset_CCK, 
									Offset_OFDM, Offset_MCS);

	return status;
}

int UpLoadSaveCalAnnex31( char *FileName, DWORD DeviceId)
{
	int  tempInt=0, i=0, j=0; 
	char line[256]="", tag[256]="", key[256]="";
	int status=0;
	char Offset_Nf[NUM_SUBBAND_G]={0},		Offset_CCK[NUM_SUBBAND_G]={0}; 
	char Offset_OFDM[NUM_SUBBAND_G]={0},	Offset_MCS[NUM_SUBBAND_G]={0}; 

 
	status = Dut_Shared_GetAnnexType31Data(
#if defined (_MIMO_)
									DeviceId,  
#endif // #if defined (_MIMO_)
									Offset_Nf, Offset_CCK, 
									Offset_OFDM, Offset_MCS);

	if(ERROR_DATANOTEXIST == status && !Dut_Shared_GetWriteTemplateFlag())  
	{
		status =ERROR_NONE;
  		return status ;
	}
	else
	{	
 
		if(!Dut_Shared_GetWriteTemplateFlag()) 
		{
			if(status)	return status ;
		}
 		else 
			status =ERROR_NONE;

			Dut_Shared_WriteAnnex31DataToFile(FileName,
#if defined (_MIMO_)
						DeviceId, //int PathId, int BandId,
#endif	// #if defined (_MIMO_)
						Offset_Nf, Offset_CCK, 
						Offset_OFDM, Offset_MCS); 
	}

	return status;
}

int LoadSaveCalAnnex32(char *FileName, DWORD DeviceId)
{
	int  tempInt=0, i=0, j=0; 
	char line[256]="", tag[256]="", key[256]="";
	int status=0;
	char Offset_Nf[NUM_SUBBAND_A]={0},		Offset_CCK[NUM_SUBBAND_A]={0}; 
	char Offset_OFDM[NUM_SUBBAND_A]={0},	Offset_MCS[NUM_SUBBAND_A]={0}; 

	status = Dut_Shared_ReadAnnex32DataFromFile(FileName,
#if defined (_MIMO_)
						 DeviceId, // PathId,  BandId,
#endif	// #if defined (_MIMO_)
						Offset_Nf, Offset_CCK, 
						Offset_OFDM, Offset_MCS);

	if(status) return status;


	status = Dut_Shared_SetAnnexType32Data(
#if defined (_MIMO_)
									DeviceId,  
#endif // #if defined (_MIMO_)
									Offset_Nf, Offset_CCK, 
									Offset_OFDM, Offset_MCS);

	return status;
}

int UpLoadSaveCalAnnex32( char *FileName, DWORD DeviceId)
{
	int  tempInt=0, i=0, j=0; 
	char line[256]="", tag[256]="", key[256]="";
	int status=0;
	char Offset_Nf[NUM_SUBBAND_A]={0},		Offset_CCK[NUM_SUBBAND_A]={0}; 
	char Offset_OFDM[NUM_SUBBAND_A]={0},	Offset_MCS[NUM_SUBBAND_A]={0}; 

 
	status = Dut_Shared_GetAnnexType32Data(
#if defined (_MIMO_)
									DeviceId,  
#endif // #if defined (_MIMO_)
									Offset_Nf, Offset_CCK, 
									Offset_OFDM, Offset_MCS);

	if(ERROR_DATANOTEXIST == status && !Dut_Shared_GetWriteTemplateFlag())  
	{
		status =ERROR_NONE;
  		return status ;
	}
	else
	{	
 
		if(!Dut_Shared_GetWriteTemplateFlag()) 
		{
			if(status)	return status ;
		}
 		else 
			status =ERROR_NONE;
			Dut_Shared_WriteAnnex32DataToFile(FileName,
#if defined (_MIMO_)
						 DeviceId, // PathId,  BandId,
#endif	// #if defined (_MIMO_)
						Offset_Nf, Offset_CCK, 
						Offset_OFDM, Offset_MCS);
	}

	return status;
}

int UpLoadSaveCalAnnex52( char *FileName, DWORD DeviceId, int PathId )
{
	int  tempInt=0, i=0, j=0; 
	char line[1024]="", tag[256]="", key[256]="";
	int status=0;
	BYTE  LoLeakageCal2G_I=0, LoLeakageCal2G_Q=0;
	BYTE  LoLeakageCal5G_I[5]={0};
	BYTE  LoLeakageCal5G_Q[5]={0};
	bool  Cal2G=0, Cal5G=0;
	

	status = Dut_Shared_GetAnnexType52Data(
#if defined (_MIMO_)
		DeviceId, PathId,
#endif //#if defined (_MIMO_)
						&Cal2G,
						&Cal5G,
						&LoLeakageCal2G_I,
						&LoLeakageCal2G_Q,
						LoLeakageCal5G_I,
						LoLeakageCal5G_Q);

	if(ERROR_DATANOTEXIST == status && !Dut_Shared_GetWriteTemplateFlag())  
	{
		status =ERROR_NONE;
  		return status ;
	}
	else
	{	
		if(!Dut_Shared_GetWriteTemplateFlag()) 
		{
			if(status)	return status ;
		}
 		else 
			status =ERROR_NONE;

			Dut_Shared_WriteAnnex52DataToFile(FileName, 
#if defined (_MIMO_)
					DeviceId, PathId,		
#endif // #if defined (_MIMO_)
									Cal2G,
									Cal5G,
									LoLeakageCal2G_I,
									LoLeakageCal2G_Q,
									(char *)LoLeakageCal5G_I,
									(char *)LoLeakageCal5G_Q); 
	}

	return status;
}

int LoadSaveCalAnnex53(char *FileName
#if defined (_MIMO_)
				,	int	DeviceId,	int PathId 		
#endif // #if defined (_MIMO_) 
)
{
	int  tempInt=0, i=0, j=0; 
	char line[1024]="", tag[256]="", key[256]="";
	int status=0;
	bool Cal2G=0, Cal5G=0;
	char TxIQmismatch_Amp_2G=0, TxIQmismatch_Phase_2G=0;
	char TxIQmismatch_Amp_5G[NUM_SUBBAND_A_REVD]={0},	TxIQmismatch_Phase_5G[NUM_SUBBAND_A_REVD]={0};

		
	 
	status = Dut_Shared_ReadAnnex53DataFromFile(FileName,
#if defined (_MIMO_)
				    DeviceId,		PathId,  //int *CsC, 
#endif //#if defined (_MIMO_)
					&Cal2G,
					&Cal5G,
					&TxIQmismatch_Amp_2G,	&TxIQmismatch_Phase_2G,
					TxIQmismatch_Amp_5G,TxIQmismatch_Phase_5G);
    
	if(status) return status;
 
status = Dut_Shared_SetAnnexType53Data(//int *CsC, 
#if defined (_MIMO_)
					    DeviceId,		PathId,  //int *CsC, 
#endif //#if defined (_MIMO_)
						Cal2G,
						Cal5G,
						TxIQmismatch_Amp_2G, TxIQmismatch_Phase_2G,
						TxIQmismatch_Amp_5G,	TxIQmismatch_Phase_5G);
	return status;
}

int UpLoadSaveCalAnnex53( char *FileName, DWORD DeviceId, int PathId )
{
	int  tempInt=0, i=0, j=0; 
	char line[1024]="", tag[256]="", key[256]="";
	int status=0;
	bool Cal2G=0, Cal5G=0;
	char TxIQmismatch_Amp_2G=0, TxIQmismatch_Phase_2G=0;
	char TxIQmismatch_Amp_5G[NUM_SUBBAND_A_REVD]={0}, TxIQmismatch_Phase_5G[NUM_SUBBAND_A_REVD]={0};
	
    Dut_Shared_GetAnnexType53Data(//int *CsC, 
#if defined (_MIMO_)
					   DeviceId,		PathId,  
#endif //#if defined (_MIMO_)
						&Cal2G,
						&Cal5G,
						&TxIQmismatch_Amp_2G,	&TxIQmismatch_Phase_2G,
						TxIQmismatch_Amp_5G,	TxIQmismatch_Phase_5G);

	if(ERROR_DATANOTEXIST == status && !Dut_Shared_GetWriteTemplateFlag())  
	{
		status =ERROR_NONE;
  		return status ;
	}
	else
	{	
 
		if(!Dut_Shared_GetWriteTemplateFlag()) 
		{
			if(status)	return status ;
		}
 		else 
			status =ERROR_NONE;

			Dut_Shared_WriteAnnex53DataToFile(FileName, 
#if defined (_MIMO_)
				DeviceId, PathId,
#endif //#if defined (_MIMO_)
									Cal2G,
									Cal5G,
									TxIQmismatch_Amp_2G,
									TxIQmismatch_Phase_2G,
									TxIQmismatch_Amp_5G,
									TxIQmismatch_Phase_5G); 
	}

	return status;
}

int LoadSaveCalAnnex54(char *FileName
#if defined (_MIMO_)
				,	int	DeviceId,	int PathId 		
#endif // #if defined (_MIMO_) 
)
{
	int  tempInt=0, i=0, j=0; 
	char line[1024]="", tag[256]="", key[256]="";
	int status=0;
	bool Cal2G=0, Cal5G=0;
	char RxIQmismatch_Amp_2G=0, RxIQmismatch_Phase_2G=0;
	char RxIQmismatch_Amp_5G[NUM_SUBBAND_A_REVD]={0}, RxIQmismatch_Phase_5G[NUM_SUBBAND_A_REVD]={0};

	status = Dut_Shared_ReadAnnex54DataFromFile(FileName,
#if defined (_MIMO_)
				    DeviceId,		PathId,  
#endif //#if defined (_MIMO_)
					&Cal2G,
					&Cal5G,
					&RxIQmismatch_Amp_2G,	&RxIQmismatch_Phase_2G,
					RxIQmismatch_Amp_5G,RxIQmismatch_Phase_5G);
    
	if(status) return status;
 
status = Dut_Shared_SetAnnexType54Data(
#if defined (_MIMO_)
					    DeviceId,		PathId, 
#endif //#if defined (_MIMO_)
						Cal2G,
						Cal5G,
						RxIQmismatch_Amp_2G, RxIQmismatch_Phase_2G,
						RxIQmismatch_Amp_5G,	RxIQmismatch_Phase_5G);

	return status;
}

int UpLoadSaveCalAnnex54( char *FileName, DWORD DeviceId, int PathId )
{
	int  tempInt=0, i=0, j=0; 
	char line[1024]="", tag[256]="", key[256]="";
	int status=0;
	bool Cal2G=0, Cal5G=0;
	char RxIQmismatch_Amp_2G=0, RxIQmismatch_Phase_2G=0;
	char RxIQmismatch_Amp_5G[NUM_SUBBAND_A_REVD]={0},	RxIQmismatch_Phase_5G[NUM_SUBBAND_A_REVD]={0};
	
    status = Dut_Shared_GetAnnexType54Data(
#if defined (_MIMO_)
					   DeviceId,		PathId,  
#endif //#if defined (_MIMO_)
						&Cal2G,
						&Cal5G,
						&RxIQmismatch_Amp_2G,	&RxIQmismatch_Phase_2G,
						RxIQmismatch_Amp_5G,	RxIQmismatch_Phase_5G);

	if(ERROR_DATANOTEXIST == status && !Dut_Shared_GetWriteTemplateFlag())  
	{
		status =ERROR_NONE;
  		return status ;
	}
	else
	{	
		if(!Dut_Shared_GetWriteTemplateFlag()) 
		{
			if(status)	return status ;
		}
 		else 
			status =ERROR_NONE;

			Dut_Shared_WriteAnnex54DataToFile(FileName, 
#if defined (_MIMO_)
				DeviceId, PathId,
#endif //#if defined (_MIMO_)
									Cal2G,
									Cal5G,
									RxIQmismatch_Amp_2G,
									RxIQmismatch_Phase_2G,
									RxIQmismatch_Amp_5G,
									RxIQmismatch_Phase_5G); 
	}

	return status;
}

int LoadSaveCalAnnex55(char *FileName)
{
	int status=0;
    BYTE Version =0, RFXtal =0, InitPwr=0, FELoss=0;
	bool ForceClass2Op=0, DisablePwrControl=0, MiscFlag=0, UsedInternalSleepClock=0;
	BYTE Rssi_Golden_Lo=0, Rssi_Golden_Hi=0, BDAddr[6] ={0}, Encr_Key_Len_Max=0, Encr_Key_Len_Min=0;
	DWORD		BTBAUDRate=0;

	status = Dut_Shared_ReadAnnex55DataFromFile(FileName,
 					&Version,
					&RFXtal,
					&InitPwr,
					&FELoss,

					&ForceClass2Op,
					&DisablePwrControl,
					&MiscFlag,
					&UsedInternalSleepClock,

					&Rssi_Golden_Lo,
					&Rssi_Golden_Hi,
					&BTBAUDRate,
					BDAddr,
					&Encr_Key_Len_Max,
					&Encr_Key_Len_Min);

	if(ERROR_DATANOTEXIST == status && !Dut_Shared_GetWriteTemplateFlag())  
	{
		status =ERROR_NONE;
  		return status ;
	}
	else
	{	
		if(!Dut_Shared_GetWriteTemplateFlag()) 
		{
			if(status)	return status ;
		}
 		else 
			status =ERROR_NONE;
		status=Dut_Shared_SetAnnexType55Data(
					Version,
					RFXtal,
					InitPwr,
					FELoss,

					ForceClass2Op,
					DisablePwrControl,
					MiscFlag,
					UsedInternalSleepClock,

					Rssi_Golden_Lo,
					Rssi_Golden_Hi,
					BTBAUDRate,
					BDAddr,
				    Encr_Key_Len_Max,
					Encr_Key_Len_Min
					);
	}
		return status;
}

int UpLoadSaveCalAnnex55( char *FileName )
{
	int status=0;
    BYTE Version =0, RFXtal =0, InitPwr=0, FELoss=0;
	bool ForceClass2Op=0, DisablePwrControl=0, MiscFlag=0, UsedInternalSleepClock=0;
	BYTE Rssi_Golden_Lo=0, Rssi_Golden_Hi=0, BDAddr[6] ={0}, Encr_Key_Len_Max=0, Encr_Key_Len_Min=0;
	DWORD BTBAUDRate=0;

	status = Dut_Shared_GetAnnexType55Data(
					&Version,
					&RFXtal,
					&InitPwr,
					&FELoss,

					&ForceClass2Op,
					&DisablePwrControl,
					&MiscFlag,
					&UsedInternalSleepClock,

					&Rssi_Golden_Lo,
					&Rssi_Golden_Hi,
					&BTBAUDRate,
					BDAddr,
					&Encr_Key_Len_Max,
					&Encr_Key_Len_Min);
	if(ERROR_DATANOTEXIST == status && !Dut_Shared_GetWriteTemplateFlag())  
	{
		status =ERROR_NONE;
  		return status ;
	}
	else
	{	
		if(!Dut_Shared_GetWriteTemplateFlag()) 
		{
			if(status)	return status ;
		}
 		else 
			status =ERROR_NONE;

		status = Dut_Shared_WriteAnnex55DataToFile(FileName,  
					Version,
					RFXtal,
					InitPwr,
					FELoss,

					ForceClass2Op,
					DisablePwrControl,
					MiscFlag,
					UsedInternalSleepClock,

					Rssi_Golden_Lo,
					Rssi_Golden_Hi,
					BTBAUDRate,
					BDAddr,
					Encr_Key_Len_Max,
					Encr_Key_Len_Min);
	}

	return status;
}

int LoadSaveCalAnnex56(char *FileName)
{
	int status=0;
	BYTE Lincense_Key[16]={0},  Key_Value[3][22]={0};
	WORD Link_Key_Mask=0;

	status = Dut_Shared_ReadAnnex56DataFromFile(FileName, 
					Lincense_Key,
	                &Link_Key_Mask,
	                Key_Value);

    if(ERROR_DATANOTEXIST == status && !Dut_Shared_GetWriteTemplateFlag())  
	{
		status =ERROR_NONE;
		return status ;
	} 
	else
	{
		status=Dut_Shared_SetAnnexType56Data( 
					Lincense_Key,
	                Link_Key_Mask,
	                Key_Value);

	}

	return status;
}

int UpLoadSaveCalAnnex56( char *FileName )
{
	int status=0;
	BYTE Lincense_Key[16]={0},  Key_Value[3][22]={0};
	WORD Link_Key_Mask=0;

	status = Dut_Shared_GetAnnexType56Data( 
					Lincense_Key,
	                &Link_Key_Mask,
	                Key_Value);
	if(ERROR_DATANOTEXIST == status && !Dut_Shared_GetWriteTemplateFlag())  
	{
		status =ERROR_NONE;
		return status ;
	} 
	else
	{
		Dut_Shared_WriteAnnex56DataToFile(FileName, 
					Lincense_Key,
	                Link_Key_Mask,
	                Key_Value);
	}

	return status;
}

int UpLoadSaveCalAnnex57(	char *FileName, 
							DWORD DeviceId, int PathId ,					 
							BYTE BandId, BYTE SubBandId)
{
	int  tempInt=0, i=0, j=0; 
	char line[1024]="", tag[256]="", key[256]="";
	int status=0;
	BYTE MEM_ROW=0;
	BYTE NumOfEnterirs=0;
 	
	DPD_ConeffcicentData Coneffcicent_Data[20];
	
    memset(Coneffcicent_Data, 0xff, sizeof(Coneffcicent_Data));
	status = Dut_Shared_GetAnnexType57Data(
#if defined (_MIMO_)
		DeviceId, PathId,
#endif //#if defined (_MIMO_)
						BandId, SubBandId,
						&MEM_ROW,
						&NumOfEnterirs,
						Coneffcicent_Data);

	if(ERROR_DATANOTEXIST == status && !Dut_Shared_GetWriteTemplateFlag())  
	{
		status =ERROR_NONE;
  		return status ;
	}
	else
	{	
		if(!Dut_Shared_GetWriteTemplateFlag()) 
		{
			if(status)	return status ;
		}
 		else 
			status =ERROR_NONE;

			Dut_Shared_WriteAnnex57DataToFile(FileName, 
#if defined (_MIMO_)
					DeviceId, PathId,		
#endif // #if defined (_MIMO_)
						BandId,
						SubBandId,
						MEM_ROW,
						NumOfEnterirs,
						Coneffcicent_Data);

	}

	return status;
}

/////////////////
int LoadSaveCalData(int DeviceType, 
					char *BtCalFlexFileName, 
					char *WlanCalFlexFileName
#if defined (_MIMO_)
					, 
					char *WlanCalFlexPwrTrgtFileFormat
#endif // #if defined (_MIMO_)
					)
{  
  	int  tempInt=0; 
 	char line[256]="";
 
	int status=0; 
	int NumOfDevice=1, NumOfPath=2, NumOfTxPath=2, NumOfRxPath=3, NumOfBand=1, NumOfSubBand=2;
	int DeviceIndex=0, PathIndex=0, BandIndex=0, SubBandIndex=0;
	int Flag2_4G=1, Flag5G=0, Flag4_9G=0, Flag5_2G=0;
	BYTE CountryCode=0;

	bool FEMCap5G=0;
	resetDownLoadFileNames();
 	if(WlanCalFlexFileName) 
		strcpy(WlanCalDownLoadFileName, WlanCalFlexFileName);
#if defined (_MIMO_)
	if(WlanCalFlexPwrTrgtFileFormat) 
		strcpy(WlanCalDownLoadMapFileNameFormat, WlanCalFlexPwrTrgtFileFormat);
#endif // #if defined (_MIMO_)
 	if(BtCalFlexFileName) 
		strcpy(BtCalDownLoadFileName, BtCalFlexFileName);

#ifdef _LINUX_
 		strcpy(BtCalFileName, BtCalDownLoadFileName);
		strcpy(WlanCalFileName, WlanCalDownLoadFileName);
#if defined (_MIMO_)
		strcpy(WlanCalMapFileNameFormat, WlanCalDownLoadMapFileNameFormat);
#endif //#if defined (_MIMO_)
#else //#ifdef _LINUX_
	if(GetCurrentDirectory( _MAX_PATH, line) != NULL)  
	{
		char tempFileName[255]="";

		strcat(line, "\\"); 
		strcpy(tempFileName, line);
		strcat(tempFileName, BtCalDownLoadFileName); 
		strcpy(BtCalFileName, tempFileName);

		strcat(line, "\\"); 
		strcpy(tempFileName, line);
		strcat(tempFileName, WlanCalDownLoadFileName); 
		strcpy(WlanCalFileName, tempFileName);

#if defined (_MIMO_)
		strcpy(tempFileName, line);
		strcat(tempFileName, WlanCalDownLoadMapFileNameFormat); 
		strcpy(WlanCalMapFileNameFormat, tempFileName);
#endif //#if defined (_MIMO_)
	}
	
#endif //#ifdef _LINUX_	

#if defined (_BT_)
 	if (FALSE == IsFileExist(BtCalFileName))
	{
		DebugLogRite("Cannot Find File '%s'. Use Default Values.\n", BtCalFileName);
		if(DEVICETYPE_BT == DeviceType) return ERROR_ERROR;
	}
#endif // #if defined (_BT_)

 #if defined (_WLAN_)
	if (FALSE == IsFileExist(WlanCalFileName))
	{
		DebugLogRite("Cannot Find File '%s'\n", WlanCalFileName);
		if(DEVICETYPE_WLAN == DeviceType) return ERROR_ERROR;
	}
 #endif //#if defined (_WLAN_)

	status = LoadSaveCalMain(WlanCalFileName); 
	if(status) return status;
 
	Dut_Shared_GetCalMainDataRevD(
		NULL,NULL,NULL,NULL,		NULL, NULL,
		NULL,NULL,NULL,NULL,	&CountryCode,
		NULL,NULL,NULL,NULL,		
		NULL,NULL,&FEMCap5G, NULL, 		NULL,
		NULL,NULL,NULL,NULL,		NULL,
		NULL,NULL,NULL,NULL,
		NULL,NULL,NULL,NULL,
		(BYTE*) &NumOfTxPath, (BYTE*)&NumOfRxPath, NULL); 

#if defined (_WLAN_)
	NumOfRxPath = 1;
	NumOfTxPath = 1;
#if defined (_MIMO_)
	NumOfDevice = (int)ceil(1.0*NumOfTxPath/MAXNUM_TXPATH);
#else //#if defined (_MIMO_)
	NumOfDevice = 1;
#endif //#if defined (_MIMO_)

	if(FEMCap5G)
		Flag5G  =1;

 	for(DeviceIndex=0; DeviceIndex<NumOfDevice; DeviceIndex++)
	{
		NumOfPath = min(NumOfTxPath-(MAXNUM_TXPATH*DeviceIndex), MAXNUM_TXPATH);

		for(PathIndex=0; PathIndex<NumOfPath; PathIndex++)
		{
			if(Flag2_4G) 
			{
				status = LoadSaveCalAnnex49(WlanCalFileName
#if defined (_MIMO_)
				, DeviceIndex, PathIndex
#endif // #if defined (_MIMO_)
									); 	
			} // if(Flag2_4G) 

			if(Flag5G)
			{
				BandIndex = BAND_802_11A;
	 			for(SubBandIndex=0; SubBandIndex<NUM_SUBBAND_A; SubBandIndex++)
					{
	 					status = LoadSaveCalAnnex50( WlanCalFileName, 
#if defined (_MIMO_)
							DeviceIndex, PathIndex, 
#endif // #if defined (_MIMO_)
							BandIndex, SubBandIndex);
						if((status && ERROR_FILENOTEXIST != status) && (status &&ERROR_DATANOTEXIST !=status))
							return status; 
					}

			} // if(Flag5G)
		} //  for(PathIndex=0; PathIndex<NumOfPath; PathIndex++)

		NumOfPath = min(NumOfRxPath-(MAXNUM_RXPATH*DeviceIndex), MAXNUM_RXPATH);
        for(PathIndex=0; PathIndex<NumOfPath; PathIndex++)
		{
		//annex 52
			if (DeviceIndex==0)
			{
				status = LoadSaveCalAnnex52(WlanCalFileName
		#if defined (_MIMO_)
					,	DeviceIndex, PathIndex		
		#endif // #if defined (_MIMO_)
					);
				if(status && ERROR_DATANOTEXIST != status)   
					return status;

				status = LoadSaveCalAnnex53(WlanCalFileName
		#if defined (_MIMO_)
					,	DeviceIndex, PathIndex		
		#endif // #if defined (_MIMO_)
					);
				if(status && ERROR_DATANOTEXIST != status)   
					return status;

				DebugLogRite("LoadSaveCalAnnex54\n");
				status = LoadSaveCalAnnex54(WlanCalFileName
		#if defined (_MIMO_)
					,	DeviceIndex, PathIndex		
		#endif // #if defined (_MIMO_)
					);
				if(status && ERROR_DATANOTEXIST != status)   
					return status;

#if defined (_W8782_)
				BandIndex = BAND_802_11G;
				SubBandIndex = 0;
				status = LoadSaveCalAnnex57(WlanCalFileName
		#if defined (_MIMO_)
					,	DeviceIndex, PathIndex,		
		#endif // #if defined (_MIMO_)
					BandIndex,	SubBandIndex);
				if(status && ERROR_DATANOTEXIST != status)   
					return status; 

				BandIndex = BAND_802_11A;
	 			for(SubBandIndex=0; SubBandIndex<NUM_SUBBAND_A; SubBandIndex++)
				{
					status = LoadSaveCalAnnex57(WlanCalFileName
		#if defined (_MIMO_)
					,	DeviceIndex, PathIndex,		
		#endif // #if defined (_MIMO_)
					BandIndex,	SubBandIndex);
					if(status && ERROR_DATANOTEXIST != status)   
						return status; 
				}
#endif //#if defined (_W8782_)

			}  // if (DeviceIndex==0)
		} // for(PathIndex

		if(Flag2_4G) 
		{
			status = LoadSaveCalAnnex31(WlanCalFileName, DeviceIndex); 
			if(status && ERROR_DATANOTEXIST != status) 
				return status; 
		}

		if(Flag5G) 
		{
			status = LoadSaveCalAnnex32(WlanCalFileName, DeviceIndex); 
			if(status && ERROR_DATANOTEXIST != status) 
				return status; 
		}

	} //DeviceIndex

	status = LoadSaveCalAnnex27(WlanCalFileName); 
	if(status && ERROR_DATANOTEXIST != status) 
		return status; 
 
	status = LoadSaveCalAnnex40(WlanCalFileName); 
	if(status && ERROR_DATANOTEXIST != status) 
		return status; 
	status = LoadSaveCalAnnex42_VD(WlanCalFileName); 
	if(status && ERROR_DATANOTEXIST != status) 
		return status; 

	status = LoadSaveCalAnnex45(WlanCalFileName); 
	if(status && ERROR_DATANOTEXIST != status) 
		return status; 
#endif //#if defined (_WLAN_)
		
#if defined(_BT_)
#if !defined (_W8782_)
	status = LoadSaveCalAnnex55(BtCalFileName  );
	if(status && ERROR_DATANOTEXIST != status)   
		return status;

	status = LoadSaveCalAnnex56(BtCalFileName  );
	if(status && ERROR_DATANOTEXIST != status)   
		return status;

  	if(DeviceType)
	{
 		status = Dut_Bt_SetCalDataRevC(1);
		if(status) return status; 
	}
#endif //#if !defined (_W8782_)
#endif	//#if defined(_BT_)


#if defined(_WLAN_)
	else
	{
 		status = DutIf_SetCalDataRevC(1); 
		if(status) return status; 
	}
#endif	//#if defined(_WLAN_)

	resetDownLoadFileNames();

	return status; 
}
 
int UpdateSaveBtCalData(int DeviceType, 
					char *BtCalFlexFileName)
{  
  
  	int  tempInt=0; 
 	char line[256]=""; 
 
	int status=0; 

	resetDownLoadFileNames();

	if(BtCalFlexFileName) 
		strcpy(BtCalDownLoadFileName, BtCalFlexFileName);

#ifdef _LINUX_
 		strcpy(BtCalFileName, BtCalDownLoadFileName);
	
#else //#ifdef _LINUX_
	if(GetCurrentDirectory( _MAX_PATH, line) != NULL)  
	{
		char tempFileName[255]="";

		strcat(line, "\\"); 
		strcpy(tempFileName, line);
		strcat(tempFileName, BtCalDownLoadFileName); 
		strcpy(BtCalFileName, tempFileName);

	}
	
#endif //#ifdef _LINUX_	
 	if (FALSE == IsFileExist(BtCalFileName))
	{
		DebugLogRite("Cannot Find File %s. Use Default Values.\n", BtCalFileName);
		if(DEVICETYPE_BT == DeviceType) return ERROR_ERROR;
	}

	status = LoadSaveCalMain(WlanCalFileName); 
	if(status) return status;

#if defined(_BT_)
	status = LoadSaveCalAnnex40(BtCalFileName); 
	if(status && ERROR_FILENOTEXIST != status) return status; 
	status = LoadSaveCalAnnex45(BtCalFileName); 
	if(status && ERROR_FILENOTEXIST != status) return status; 

  	if(DeviceType)
	{
 		status = Dut_Bt_UpdateCalDataRevC_Bt(1);
		if(status) return status; 
	}
 #endif	//#if defined(_BT_)
	resetDownLoadFileNames();

	return status; 
}
 
int UpLoadSaveCalData(int *CsC,	int DeviceType, 
					  char *BtCalFlexFileName,  
					  char *WlanCalFlexFileName
#if defined (_MIMO_)
					  , 
 					  char *WlanCalFlexPwrTrgtFileFormat
#endif // #if defined (_MIMO_)
					  )
{  
   	char line[256]="" , key[256]="";
	BYTE CountryCode=0; 
	int  CsCValue=0; 
	int DeviceIndex=0, PathIndex=0, BandIndex=0, SubBandIndex=0;
	int status=0;
  
	resetUpLoadFileNames();

 	if(WlanCalFlexFileName) 
		strcpy(WlanCalUploadFileName, WlanCalFlexFileName);
#if defined (_MIMO_)
	if(WlanCalFlexPwrTrgtFileFormat) 
		strcpy(WlanCalUploadMapFileNameFormat, WlanCalFlexPwrTrgtFileFormat);
#endif // #if defined (_MIMO_)
 	if(BtCalFlexFileName) 
		strcpy(BtCalUploadFileName, BtCalFlexFileName);
 
	{
		char temp[_MAX_PATH]="";
		int tempInt=0;
 
		if(getcwd(temp, _MAX_PATH) != NULL)  
		strcat(temp, "/setup.ini");
		else
		strcpy(temp, "setup.ini");

		tempInt	= GetPrivateProfileInt("DEBUG","WriteTemplate", 
			Dut_Shared_GetWriteTemplateFlag(), temp); 
		Dut_Shared_SetWriteTemplateFlag(tempInt);
	}

#ifdef _LINUX_
 		strcpy(BtCalFileName, BtCalUploadFileName);
 		strcpy(WlanCalFileName, WlanCalUploadFileName);
#if defined (_MIMO_)
		strcpy(WlanCalMapFileNameFormat, WlanCalUploadMapFileNameFormat);
#endif //#if defined (_MIMO_)
 
#else // #ifdef _LINUX_
	if(GetCurrentDirectory( _MAX_PATH, line) != NULL)  
	{	
		char tempFileName[_MAX_PATH]="";

		strcat(line, "\\"); 
		strcpy(tempFileName, line);
		strcat(tempFileName, BtCalUploadFileName); 
		strcpy(BtCalFileName, tempFileName);
 		strcat(line, "\\"); 
		strcpy(tempFileName, line);
		strcat(tempFileName, WlanCalUploadFileName); 
		strcpy(WlanCalFileName, tempFileName);

#if defined (_MIMO_)
		strcpy(tempFileName, line);
		strcat(tempFileName, WlanCalUploadMapFileNameFormat); 
		strcpy(WlanCalMapFileNameFormat, tempFileName);
#endif //#if defined (_MIMO_)
	}
#endif // #ifdef _LINUX_
	if(DeviceType)
	{
#if defined(_BT_)
		status = Dut_Bt_GetCalDataRevC(CsC, &CsCValue); 
#endif	//#if defined(_BT_)
	}
#if defined(_WLAN_)
	else 
		status = DutIf_GetCalDataRevC(CsC, &CsCValue); 
#endif	//#if defined(_WLAN_)
	if(status)	return status ;
 	if(!(*CsC))	return status ;

	// clean out the file
	remove(BtCalFileName);
	remove(WlanCalFileName); 
	
	status = UpLoadSaveCalMain(WlanCalFileName); 
 	if(status) return status; 
 
#if defined (_WLAN_)
	for(DeviceIndex=0; DeviceIndex<MAXNUM_MIMODEVICE; DeviceIndex++)
	{
		for(PathIndex=0; PathIndex<MAXNUM_TXPATH; PathIndex++)
		{
			status = UpLoadSaveCalAnnex49(WlanCalFileName
	#if defined (_MIMO_)
				, DeviceIndex, PathIndex
	#endif //#if defined (_MIMO_)
										);
			BandIndex = BAND_802_11A;
 			for(SubBandIndex=0; SubBandIndex<NUM_SUBBAND_A; SubBandIndex++)
			{
				status = UpLoadSaveCalAnnex50(WlanCalFileName, 
#if defined (_MIMO_)
					DeviceIndex, PathIndex, 
#endif // #if defined (_MIMO_)
					BandIndex, SubBandIndex);
				if(status) return status;
			}
		}
	
		status = UpLoadSaveCalAnnex31(WlanCalFileName, DeviceIndex);
		if(status) return status; 

		status = UpLoadSaveCalAnnex32(WlanCalFileName, DeviceIndex);
		if(status) return status; 
	}

	status = UpLoadSaveCalAnnex27(WlanCalFileName); 
	if(status) return status; 
 
	status = UpLoadSaveCalAnnex40(WlanCalFileName); 
	if(status) return status; 

	status = UpLoadSaveCalAnnex42_VD(WlanCalFileName); 
	if(status) return status;

	status = UpLoadSaveCalAnnex45(WlanCalFileName); 
	if(status) return status; 
	
	for(DeviceIndex=0; DeviceIndex<MAXNUM_MIMODEVICE; DeviceIndex++)
	{
		for(PathIndex=0; PathIndex<MAXNUM_TXPATH; PathIndex++)
		{
			status = UpLoadSaveCalAnnex52(WlanCalFileName, DeviceIndex, PathIndex);
			if(status) return status;

			status = UpLoadSaveCalAnnex53(WlanCalFileName, DeviceIndex, PathIndex);
			if(status) return status;

			status = UpLoadSaveCalAnnex54(WlanCalFileName, DeviceIndex, PathIndex);
			if(status) return status;

#if defined (_W8782_)
			BandIndex = BAND_802_11G;
			SubBandIndex = 0;
			status = UpLoadSaveCalAnnex57(WlanCalFileName, DeviceIndex, PathIndex, (BYTE)BandIndex, (BYTE)SubBandIndex);
			if(status) return status;

			BandIndex = BAND_802_11A;
			for(SubBandIndex=0; SubBandIndex<NUM_SUBBAND_A_REVD; SubBandIndex++)
			{
				status = UpLoadSaveCalAnnex57(WlanCalFileName, DeviceIndex, PathIndex, (BYTE)BandIndex, (BYTE)SubBandIndex);
				if(status) return status;
			}
#endif //#if defined (_W8782_)
		}	//for(PathIndex
	}	//for(DeviceIndex
	
#endif //#if defined (_WLAN_)

#if !defined (_W8782_)
	status = UpLoadSaveCalAnnex55(BtCalFileName);
	if(status) return status;

	status = UpLoadSaveCalAnnex56(BtCalFileName);
	if(status) return status;
#endif //#if !defined (_W8782_)
	resetUpLoadFileNames();
 
	return status ;
}
 
int UpLoadHwInfoData(void)
{  
 	char line[256]="" , key[256]="";
	int  CsC=0, CsCValue=0; 
	int DeviceIndex=0, PathIndex=0, BandIndex=0, SubBandIndex=0;
	int status=0, i=0, j=0;
  
 	BYTE	PcbRev_DesignType=0,	PcbRev_MajorRev=0;
	BYTE	PcbRev_MinorRev=0,		PcbRev_Variation=0;

	bool ExtPaPolar_neg_G[MAXNUM_MIMODEVICE] ={1
#if MAXNUM_MIMODEVICE == 2
		,1
#endif //#if MAXNUM_MIMODEVICE == 2
	};   
	bool ExtPaPolar_neg_A[MAXNUM_MIMODEVICE] ={1
#if MAXNUM_MIMODEVICE == 2
		,1
#endif //#if MAXNUM_MIMODEVICE == 2
	};
	BYTE FemConfId[MAXNUM_MIMODEVICE]={0}; 

  	BYTE StructRev=0x0C; 
	BYTE PAThermaScaler=0x2; 
	BYTE AssemblyHouse=0, SpiSize=0, SpiUnit=0;
	DWORD	Sn=0; 
	WORD	SocId=0; 
	BYTE	SocVer=0, BbuId=0, BbuVer=0, RfId=0, RfVer=0; 
	DWORD	TestToolVersion=0, MfgTaskVersion =0, DllVersion=0;  
	BYTE	pBtCoexist =0;
	BYTE	AntTx[MAXNUM_MIMODEVICE]={0x03
#if MAXNUM_MIMODEVICE == 2
		,0x03
#endif //#if MAXNUM_MIMODEVICE == 2
}; 
BYTE AntRx[MAXNUM_MIMODEVICE]={0x07
#if MAXNUM_MIMODEVICE == 2
, 0x07
#endif //#if MAXNUM_MIMODEVICE == 2
};
	BYTE	AntId[MAXNUM_MIMODEVICE][MAXNUM_RXPATH]={0};
 	BYTE	CC=0x10;
	BYTE	ExtXtalSource=0,	ExtSleepClk=0, WlanWakeUp=0;
	bool	ExtLna_G[MAXNUM_MIMODEVICE]={0};
	bool	ExtLna_A[MAXNUM_MIMODEVICE]={0}; 
	bool	ExtLnaPolority_G[MAXNUM_MIMODEVICE]={0};
	bool	ExtLnaPolority_A[MAXNUM_MIMODEVICE]={0}; 
	BYTE	LnaId[MAXNUM_MIMODEVICE][MAXNUM_RXPATH]={0};
	BYTE	LnaGainDelta=0;
//	BYTE	LnaId_A[MAXNUM_MIMODEVICE][MAXNUM_RXPATH]={0};  
	BYTE	MimoTxCnt=2, MimoRxCnt=3;
	BYTE	MimoMap[MAXNUM_MIMODEVICE][MAXNUM_RXPATH]=
	{{0x00
#if MAXNUM_RXPATH >1
	,   0x01, 0x02
#endif //#if MAXNUM_RXPATH >1
	}
#if MAXNUM_MIMODEVICE == 2
	, 
	{0x10, 0x11, 0x12}
#endif //#if MAXNUM_MIMODEVICE == 2
	}; 
	int BandSupport2_4=0, BandSupport4_9=0, BandSupport5_0=0, BandSupport5_2=0; 
	int DeviceCnt=0; 
	BYTE AppModeFlag=0, SocOrRev=0, RfuOrRev=0;



#if defined(_WLAN_)
	status = DutIf_GetCalDataRevC(&CsC, &CsCValue); 
#else 
	status = Dut_Bt_GetCalDataRevC(&CsC, &CsCValue); 
#endif	//#if defined(_WLAN_)
	if(status)	return status ;
 	if(!(CsC))	return status ;

#if defined(_CAL_REV_D_)
//jsz revisit Dut_Shared_GetCalMainDataRevD();
#else
	status = Dut_Shared_GetCalMainDataRevC( 
		&StructRev,			&PcbRev_DesignType,	&PcbRev_MajorRev,	&PcbRev_MinorRev,	&PcbRev_Variation,
 		ExtPaPolar_neg_G,	ExtPaPolar_neg_A,	ExtLna_G,			ExtLna_A,			ExtLnaPolority_G,	
		ExtLnaPolority_A,	AntTx,				AntRx,				&pBtCoexist,		FemConfId,			
		AntId,				&LnaGainDelta,		&SpiSize,			&SpiUnit,			&AssemblyHouse,		
		&Sn,				&CC,				&ExtXtalSource,		&ExtSleepClk,		&WlanWakeUp,		
		&SocId,				&SocVer,			&BbuId,				&BbuVer,			&RfId,				
		&RfVer, 			&TestToolVersion,	&MfgTaskVersion, 	&DllVersion, 		&AppModeFlag,		
		&SocOrRev,			&RfuOrRev,			&MimoTxCnt, 		&MimoRxCnt,			MimoMap
 		);  
#endif	//#if defined(_CAL_REV_D_)

	DeviceCnt = (int)ceil(1.0*MimoTxCnt/MAXNUM_TXPATH); 
	if(!status || ERROR_DATANOTEXIST != status)   
	{
 
		DebugLogRite("Structure Integraty CsC: %d (%X)\n", CsC, CsCValue);
		DebugLogRite("=========================================================\n");
		DebugLogRite("Structure Rev:\t\t%X\n", StructRev); 
		DebugLogRite("PcbRefDesign:\t\t%0X\nPcbMajorRevision:\t%0X\n"
							 "PcbMinorRevision:\t%0X\nPcbVariation:\t\t%0X\n\n", 
								PcbRev_DesignType, PcbRev_MajorRev, 
								PcbRev_MinorRev, PcbRev_Variation);
		DebugLogRite("BtCoexist:\t\t%d\n", pBtCoexist);
 		DebugLogRite("PA Thermal Scaler:\t%d\n", PAThermaScaler);
 		DebugLogRite("Ext Xtal Source:\t%d\n", ExtXtalSource);
 		DebugLogRite("Ext Sleep Clock:\t%d\n", ExtSleepClk);
 		DebugLogRite("Wlan Wake Up:\t\t%d\n", WlanWakeUp);

		DebugLogRite("CC:\t\t\t0x%X\n", CC  );
 
 		DebugLogRite("AssemblyHouse:\t\t%d\nSN:\t\t\t%d\n", AssemblyHouse, Sn);
		DebugLogRite("SpiSizeInfo:\t\t%d%C\n", SpiSize, SpiUnit?'M':'K');
 		DebugLogRite("TestToolVer:\t\t%04X\nTaskVer:\t\t%04X\n", 
									TestToolVersion, MfgTaskVersion);
 		DebugLogRite("DllVersion:\t\t%04X\n", DllVersion);
		DebugLogRite("SOC:\t\t\t%04X\t%02X\n"
								"BBP:\t\t\t%02X\t%02X\nRF:\t\t\t%02X\t%02X\n", 
								SocId,  SocVer, BbuId, BbuVer, RfId, RfVer); 

		DebugLogRite("AppModeFlag:\t\t\t%d\n", AppModeFlag);
		DebugLogRite("SocOrRev:\t\t\t%02X\n", SocOrRev);
		DebugLogRite("RfuOrRev:\t\t\t%02X\n", RfuOrRev);
		DebugLogRite("\n"); 
		DebugLogRite("MimoTxCnt:\t\t\t%d\n", MimoTxCnt);
		DebugLogRite("MimoRxCnt:\t\t\t%d\n", MimoRxCnt);
		DebugLogRite("MimoMap:\n"); 
		for (i=0; i<DeviceCnt; i++)
		{
			for (j=0; j<MAXNUM_RXPATH; j++) 
			{
				DebugLogRite("%d\t", MimoMap[i][j]); 
			}
		
			DebugLogRite("\n"); 
		}

		DebugLogRite("\n"); 
		for (i=0; i<DeviceCnt; i++)
		{
 			DebugLogRite("MIMO DEVICE %d\n=====================\n", i);
 			DebugLogRite("FemConfId:\t%d\n", FemConfId[i]);

			DebugLogRite("ExtPA's control Polarizatin for A:\t%d\n", ExtPaPolar_neg_A[i]);
 			DebugLogRite("ExtPA's control Polarizatin for G:\t%d\n", ExtPaPolar_neg_G[i]);

			DebugLogRite("Ext LNA for A:\t%d\n", ExtLna_A[i]);
			DebugLogRite("Ext LNA for G:\t%d\n", ExtLna_G[i]);
			for (j=0; j<MAXNUM_RXPATH; j++) 
			{
				DebugLogRite("Ext LNA PartId for A:\t%d\n", LnaId[i][j]); 
			}
 
			DebugLogRite("TxAnt:\t\t\t%d\n", AntTx[i]);
			DebugLogRite("RxAnt:\t\t\t%d\n", AntRx[i]);
			for (j=0; j<MAXNUM_RXPATH; j++) 
			{
				DebugLogRite("Ant Part Info:\t\t%d\n", AntId[i][j]);
			}
				DebugLogRite("\n");

		}
		DebugLogRite("Lna Gain Delta:\t%d\n", LnaGainDelta);

	}
 
	return status;
}

#if !defined(_CAL_REV_D_)
int ChangeCountryCodeInEEPROM(int NewCountryCode, int addRegionalPowerTargetTable)
{
	int  tempInt=0; 
	char line[256]=""; 
 

	int  CsC=0, CsCValue=0; 
	int status=0, DeviceIndex=0;
	BYTE	PcbRev_DesignType=0,	PcbRev_MajorRev=0;
	BYTE	PcbRev_MinorRev=0,		PcbRev_Variation=0;

	BOOL ExtPaPolar_neg_G[MAXNUM_MIMODEVICE] ={1
#if MAXNUM_MIMODEVICE == 2
		,1   
#endif //#if MAXNUM_MIMODEVICE == 2
	};	
	BOOL ExtPaPolar_neg_A[MAXNUM_MIMODEVICE] ={1
#if MAXNUM_MIMODEVICE == 2
		,1
#endif //#if MAXNUM_MIMODEVICE == 2
	};	
	BYTE ExtPA_PartId[MAXNUM_MIMODEVICE][MAXNUM_TXPATH]={0}; 

  	BYTE StructRev=0x0C; 
	BYTE PAThermaScaler=0x2; 
	BYTE AssemblyHouse=0, SpiSize=0, SpiUnit=0;
	DWORD	Sn=0; 
	WORD	SocId=0; 
	BYTE	SocVer=0, BbuId=0, BbuVer=0, RfId=0, RfVer=0; 
	DWORD	TestToolVersion=0, MfgTaskVersion =0, DllVersion=0;  
	BYTE	AntBT =0;
	BYTE	AntTx[MAXNUM_MIMODEVICE]={0x03
#if MAXNUM_MIMODEVICE == 2
		, 0x03
#endif //#if MAXNUM_MIMODEVICE == 2
	};
	BYTE AntRx[MAXNUM_MIMODEVICE]={0x07
#if MAXNUM_MIMODEVICE == 2
		, 0x07
#endif //#if MAXNUM_MIMODEVICE == 2
	};
	BYTE	AntId[MAXNUM_MIMODEVICE][MAXNUM_RXPATH]={0};
 	BYTE	CC=0x10;
	BYTE	ExtXtalSource=0,	ExtSleepClk=0, WlanWakeUp=0;
	int	ExtLna_G[MAXNUM_MIMODEVICE]={0};
	int	ExtLna_A[MAXNUM_MIMODEVICE]={0}; 
	BYTE	LnaId[MAXNUM_MIMODEVICE][MAXNUM_RXPATH]={0};
//	BYTE	LnaId_A[MAXNUM_MIMODEVICE][MAXNUM_RXPATH]={0};  
	BYTE	MimoTxCnt=2, MimoRxCnt=3;
	BYTE	MimoMap[MAXNUM_MIMODEVICE][MAXNUM_RXPATH]={
		{0x00
#if MAXNUM_RXPATH >1
			,   0x01, 0x02
#endif //#if MAXNUM_RXPATH >1
		}
#if MAXNUM_MIMODEVICE == 2
		, {0x10, 0x11, 0x12}
#endif //#if MAXNUM_MIMODEVICE == 2
	}; 

	status = DutIf_GetCalDataRevC(&CsC, &CsCValue); 
	if(status)	return status ;
 	if(!(CsC))	return !(CsC) ;


 	status = DutIf_LoadCustomizedSettings(); 
	if(status)	return status ;

#if defined(_CAL_REV_D_)
#else
	status = Dut_Shared_GetCalMainDataRevC( 
					&StructRev, NULL,	NULL,	NULL,	NULL,
 					NULL,		NULL,	NULL,	NULL,	NULL, 
					NULL,		NULL,	NULL,	NULL,	NULL, 
					NULL,		NULL,	NULL,	NULL,	NULL, 
					NULL,  		&CC,	NULL,	NULL,	NULL,  
					NULL,		NULL,	NULL,	NULL,	NULL, 
					NULL,		NULL,	NULL,	NULL,	NULL, 
					NULL,		NULL,	NULL,	NULL,	NULL
 					); 
	if(status)	return status ;
#endif	//#if defined(_CAL_REV_D_)

	DebugLogRite(" Current Country code is 0x%02X, Change to 0x%02X\n", CC, NewCountryCode);

	CC = NewCountryCode;

	 status = Dut_Shared_SetCalMainDataRevC( 
					&StructRev, NULL, NULL, NULL, NULL, 		
 					NULL,		NULL,	NULL,	NULL,	NULL, 
					NULL,		NULL,	NULL,	NULL,	NULL, 
					NULL,		NULL,	NULL,	NULL,	NULL, 
					NULL,  		&CC,	NULL,	NULL,	NULL,  
					NULL,		NULL,	NULL,	NULL,	NULL, 
					NULL,		NULL,	NULL,	NULL,	NULL, 
					NULL,		NULL,	NULL,	NULL,	NULL
		 
 		); 

	if(status)	return status ;

 
#ifdef _LINUX_
#if defined (_MIMO_)
		strcpy(WlanCalMapFileNameFormat, WlanCalDownLoadMapFileNameFormat);
#endif //#if defined (_MIMO_)
#else // #ifdef _LINUX_
	if(GetCurrentDirectory( _MAX_PATH, line) != NULL)  
	{
		char tempFileName[255]="";

#if defined (_MIMO_)
		strcat(line, "\\"); 
 		strcpy(tempFileName, line);
		strcat(tempFileName, WlanCalDownLoadMapFileNameFormat); 
		strcpy(WlanCalMapFileNameFormat, tempFileName);
#endif //#if defined (_MIMO_)
	}
#endif // #ifdef _LINUX_


	for(DeviceIndex=0; DeviceIndex<MAXNUM_MIMODEVICE; DeviceIndex++)
	{

#if defined (_MIMO_)
 		sprintf(WlanCalMapFileName, WlanCalMapFileNameFormat, CC,DeviceIndex); 

		if(	addRegionalPowerTargetTable || 
			ERROR_DATANOTEXIST != Dut_Shared_GetAnnexType33Data(DeviceIndex, 
										NULL, NULL, NULL, NULL) )		
		{

 			if (FALSE == IsFileExist(WlanCalMapFileName))
			{
				DebugLogRite("Cannot Find File %s\n", WlanCalMapFileName);
				DebugLogRite("Please make sure this file exist before continous with this action.\n");

				return ERROR_ERROR;
			}
 
			status = LoadSaveCalAnnex3335(WlanCalMapFileName, DeviceIndex); 
				if(status) return status; 
		}	

		if((addRegionalPowerTargetTable	&& 
			ERROR_DATANOTEXIST != Dut_Shared_GetAnnexType16Data(DeviceIndex, TXPATH_PATHA, 
										BAND_802_11A, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL))
			|| 
			ERROR_DATANOTEXIST != Dut_Shared_GetAnnexType35Data(DeviceIndex, NULL, 
										NULL, NULL, NULL, NULL) )	
		{
 			if (FALSE == IsFileExist(WlanCalMapFileName))
			{
				DebugLogRite("Cannot Find File %s\n", WlanCalMapFileName);
				DebugLogRite("Please make sure this file exist before continoue with this action.\n");
				return ERROR_ERROR;
			}
 
			status = LoadSaveCalAnnex3436(WlanCalMapFileName, DeviceIndex); 
				if(status) return status; 
		}
#endif //#if defined (_MIMO_)
	}
	return DutIf_SetCalDataRevC(1); 

}
#endif	//#if !defined(_CAL_REV_D_)


int LoadSaveCalAnnex40(char *FileName)
{
 	int status =0;
	WORD	Vid=0, Pid=0;
	BYTE	ChipId=0, ChipVersion=0;
	BYTE Flag_SettingPrev=0, NumOfRailInChip=0;
	
	BYTE PowerDownMode = {0};
	BYTE NormalMode[MAX_PM_RAIL_PW886] = {0};
	BYTE SleepModeSetting[MAX_PM_RAIL_PW886] = {0};
	BYTE SleepModeFlag[MAX_PM_RAIL_PW886] = {0};
	BYTE DpSlp=0;

  	char tag[256]="", key[256]="";
	int i=0, cnt=0;
  
	status = Dut_Shared_ReadAnnex40DataFromFile(FileName,
						&Vid,		&Pid,	
						&ChipId,	&ChipVersion,					   
						&Flag_SettingPrev, 
						&NumOfRailInChip,
						&PowerDownMode,
						NormalMode, 
						SleepModeFlag, SleepModeSetting,
						&DpSlp);
	if(status && ERROR_DATANOTEXIST != status) return status;

	if(ERROR_DATANOTEXIST != status)  
	{
		status = Dut_Shared_SetAnnexType40Data(
						&Vid,		&Pid,	
						&ChipId,	&ChipVersion,					   
						&Flag_SettingPrev, 
						&NumOfRailInChip,
						&PowerDownMode,
						NormalMode, 
						SleepModeFlag, SleepModeSetting,
						&DpSlp);
	}
	else
		status = ERROR_NONE;

 
	return status; 
}



int UpLoadSaveCalAnnex40(char *FileName)
{
 	int status =0;
	WORD	Vid=0, Pid=0;
	BYTE	ChipId=0, ChipVersion=0;
	BYTE Flag_SettingPrev=0, NumOfRailInChip=0;
	
	BYTE PowerDownMode = {0};
	BYTE NormalMode[MAX_PM_RAIL_PW886] = {0};
	BYTE SleepModeSetting[MAX_PM_RAIL_PW886] = {0};
	BYTE SleepModeFlag[MAX_PM_RAIL_PW886] = {0};
	BYTE DpSlp=0;

  	char line[256]="", key[256]="";
	int i=0;
 
	status = Dut_Shared_GetAnnexType40Data(
						 &Vid,	 &Pid,	
						 &ChipId,	 &ChipVersion,					   
						 &Flag_SettingPrev, 
						&NumOfRailInChip,
						&PowerDownMode,
						NormalMode, 
						SleepModeFlag, 
						SleepModeSetting,
						&DpSlp);

	if(ERROR_DATANOTEXIST == status && !Dut_Shared_GetWriteTemplateFlag())  
	{
		status =ERROR_NONE;
 		return status ;
	}
	else
	{	
		if(!Dut_Shared_GetWriteTemplateFlag()) 
		{
			if(status)	return status ;
		}
		else 
			status =ERROR_NONE;

	Dut_Shared_WriteAnnex40DataToFile(FileName,
						&Vid,		&Pid,	
						&ChipId,	&ChipVersion,					   
						&Flag_SettingPrev, 
						&NumOfRailInChip,
						&PowerDownMode,
						NormalMode, 
						SleepModeFlag, SleepModeSetting,
						&DpSlp);
	}

 	return status;
}



/////////////////////////////////////////////////

 
/////////////////////////////////////////////////
int LoadSaveCalAnnex45(char *FileName)
{
 	int status =0;
  	bool	Flag_GpioCtrlXosc=0; 
	BYTE XoscSettling=0;
 	
	DWORD GpioMask[2] = {0}, GpioOutLvl[2] = {0};
 
  	char tag[256]="", key[256]="";
	int i=0, cnt=0;
 
	status = Dut_Shared_ReadAnnex45DataFromFile(FileName,  
											&Flag_GpioCtrlXosc,
											&XoscSettling,
											GpioMask,
											GpioOutLvl);

	if(status && ERROR_DATANOTEXIST != status) return status;

	if(ERROR_DATANOTEXIST != status)  
	{
		status = Dut_Shared_SetAnnexType45Data(
						 Flag_GpioCtrlXosc,	 XoscSettling,	
						 GpioMask,	 GpioOutLvl);
	}
	else
		status = ERROR_NONE;

	return status;
}



int UpLoadSaveCalAnnex45(char *FileName)
{
 	int status =0;
 	bool	Flag_GpioCtrlXosc=0; 
	BYTE XoscSettling=0;
 	
	DWORD GpioMask[2] = {0}, GpioOutLvl[2] = {0};

  	char line[256]="", key[256]="";
	int i=0;
 
	status = Dut_Shared_GetAnnexType45Data(
						 &Flag_GpioCtrlXosc,	 &XoscSettling, 
						GpioMask, 
						GpioOutLvl);

	if(ERROR_DATANOTEXIST == status && !Dut_Shared_GetWriteTemplateFlag())  
	{
		status =ERROR_NONE;
 		return status ;
	}
	else
	{	
		if(!Dut_Shared_GetWriteTemplateFlag()) 
		{
			if(status)	return status ;
		}
		else 
			status =ERROR_NONE;
 
	Dut_Shared_WriteAnnex45DataToFile(FileName,  
											&Flag_GpioCtrlXosc,
											&XoscSettling,
											GpioMask,
											GpioOutLvl);
	}

 	return status;
}


#if defined (_MIMO_)
int LoadSaveCalAnnex47(char *FileName,	int	DeviceId, int PathId)   
{
	int  tempInt=0, i=0, j=0; 
	char line[256]="", tag[256]="", key[256]="";
	int status=0;
	char Offset_Nf[NUM_SUBBAND_G]={0},		Offset_CCK[NUM_SUBBAND_G]={0}; 
	char Offset_OFDM[NUM_SUBBAND_G]={0},	Offset_MCS[NUM_SUBBAND_G]={0}; 

	status = Dut_Shared_ReadAnnex47DataFromFile(FileName,
						DeviceId,  PathId,
						Offset_Nf, Offset_CCK, 
						Offset_OFDM, Offset_MCS); 
	if(status) return status;
 

	status = Dut_Shared_SetAnnexType47Data(
									DeviceId, PathId,
									Offset_Nf, Offset_CCK, 
									Offset_OFDM, Offset_MCS);

	return status;
}

int UpLoadSaveCalAnnex47( char *FileName,	int	DeviceId, int PathId)  
{
	int  tempInt=0, i=0, j=0; 
	char line[256]="", tag[256]="", key[256]="";
	int status=0;
	char Offset_Nf[NUM_SUBBAND_G]={0},		Offset_CCK[NUM_SUBBAND_G]={0}; 
	char Offset_OFDM[NUM_SUBBAND_G]={0},	Offset_MCS[NUM_SUBBAND_G]={0}; 

	status = Dut_Shared_GetAnnexType47Data(
									DeviceId, PathId,
									Offset_Nf, Offset_CCK, 
									Offset_OFDM, Offset_MCS);

	if(ERROR_DATANOTEXIST == status && !Dut_Shared_GetWriteTemplateFlag())  
	{
		status =ERROR_NONE;
		return status ;
	} 
	else
	{
	Dut_Shared_WriteAnnex47DataToFile(FileName,
						DeviceId,  PathId,
						Offset_Nf, Offset_CCK, 
						Offset_OFDM, Offset_MCS); 
	}
	
	return status;
}

int LoadSaveCalAnnex48(char *FileName,	int	DeviceId, int PathId) 
{
	int  tempInt=0, i=0, j=0; 
	char line[256]="", tag[256]="", key[256]="";
	int status=0;
	char Offset_Nf[NUM_SUBBAND_A]={0},		Offset_CCK[NUM_SUBBAND_A]={0}; 
	char Offset_OFDM[NUM_SUBBAND_A]={0},	Offset_MCS[NUM_SUBBAND_A]={0}; 

	status = Dut_Shared_ReadAnnex48DataFromFile(FileName,
						DeviceId,  PathId,
						Offset_Nf, Offset_CCK, 
						Offset_OFDM, Offset_MCS); 

	if(status) return status;

	status = Dut_Shared_SetAnnexType48Data(
									DeviceId, PathId,
									Offset_Nf, Offset_CCK, 
									Offset_OFDM, Offset_MCS);

	return status;
}


int UpLoadSaveCalAnnex48( char *FileName,	int	DeviceId, int PathId) 
{
	int  tempInt=0, i=0, j=0; 
	char line[256]="", tag[256]="", key[256]="";
	int status=0;
	char Offset_Nf[NUM_SUBBAND_A]={0},		Offset_CCK[NUM_SUBBAND_A]={0}; 
	char Offset_OFDM[NUM_SUBBAND_A]={0},	Offset_MCS[NUM_SUBBAND_A]={0}; 

	status = Dut_Shared_GetAnnexType48Data(
									DeviceId, PathId,
									Offset_Nf, Offset_CCK, 
									Offset_OFDM, Offset_MCS);

	if(ERROR_DATANOTEXIST == status && !Dut_Shared_GetWriteTemplateFlag())  
	{
		status =ERROR_NONE;
		return status ;
	} 
	else
	{
	Dut_Shared_WriteAnnex48DataToFile(FileName,
						DeviceId,  PathId,
						Offset_Nf, Offset_CCK, 
						Offset_OFDM, Offset_MCS); 
	}
	
	return status;
}
#endif // #if defined (_MIMO_)

int LoadSaveCalAnnex52(char *FileName
#if defined (_MIMO_)
				,	int	DeviceId,	int PathId 		
#endif // #if defined (_MIMO_) 
)
{

#if !defined (_MIMO_) 
	int	DeviceId =0, PathId=0;
#endif // #if !defined (_MIMO_) 
	int  tempInt=0, i=0, j=0; 
	char line[256]="", tag[256]="", key[256]="";
	
    int status=0;
    BYTE LoLeakageCal2G_I=0x00, LoLeakageCal2G_Q=0x00;
    BYTE LoLeakageCal5G_I[NUM_SUBBAND_A_REVD];
	BYTE LoLeakageCal5G_Q[NUM_SUBBAND_A_REVD];
	bool Cal2G =0, Cal5G=0;
		
	status = Dut_Shared_ReadAnnex52DataFromFile(FileName,
#if defined (_MIMO_)
					DeviceId, PathId,		
#endif // #if defined (_MIMO_)
					&Cal2G,
					&Cal5G,
					&LoLeakageCal2G_I,
					&LoLeakageCal2G_Q,	
					(char *)LoLeakageCal5G_I,
					(char *)LoLeakageCal5G_Q);
    
	if(status) return status;
 
status = Dut_Shared_SetAnnexType52Data(
#if defined (_MIMO_)
					   DeviceId,		PathId,  
#endif //#if defined (_MIMO_)
						Cal2G,
						Cal5G,
						LoLeakageCal2G_I,
						LoLeakageCal2G_Q,
						LoLeakageCal5G_I,
						LoLeakageCal5G_Q);
	return status;
}

int UpLoadSaveCalAnnex42_VD(char *FileName)
{
	int status=0;
	BYTE pNoOfEntry=0;
	THERMALCALPARMS_VD	ThermalCalParam[5];

    status = Dut_Shared_GetAnnexType42Data_VD( 
					&pNoOfEntry,
					ThermalCalParam);

	if(ERROR_DATANOTEXIST == status && !Dut_Shared_GetWriteTemplateFlag())  
	{
		status =ERROR_NONE;
		return status ;
	} 
	else
	{
	status = Dut_Shared_WriteAnnex42DataToFile_VD(FileName,
					&pNoOfEntry,
					ThermalCalParam);
	}
	
	return status;
}

int LoadSaveCalAnnex42_VD(char *FileName)
{
	int status=0;
	BYTE pNoOfEntry=0;
	THERMALCALPARMS_VD	ThermalCalParam[5];

	//JKLCHEN
	memset(ThermalCalParam,0xff,sizeof(ThermalCalParam));	
	//JKLCHEN

	status = Dut_Shared_ReadAnnex42DataFromFile_VD(FileName,
					&pNoOfEntry, ThermalCalParam);

	//jklchen
	//if(pNoOfEntry < 4)
		pNoOfEntry=5;//
	//jklchen

    status = Dut_Shared_SetAnnexType42Data_VD( 
					&pNoOfEntry,
					ThermalCalParam);

	return status;
}

#if defined (_WLAN_)
int UpLoadSaveCalAnnex49(char *FileName
#if defined (_MIMO_)
				,	int	DeviceId,	int PathId 		
#endif // #if defined (_MIMO_) 
)
{
	int status=0;
    BYTE NumOfSavedCh=0, CalXtal=0, NumOfTxPowerLevelStore=0;
	BYTE PdetHiLoGap=0, MCS20MPdetOffset=0, MCS20MInitPOffset =0;
	BYTE MCS40MPdetOffset =0, MCS40MInitPOffset=0;
	bool CalMethod=0, CloseTXPWDetection=0 ,ExtPAPolarize=0;
	char CorrInitPwr_B[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD]={0}, CorrDetThr_B[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD]={0};
	char CorrInitPwr_G[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD]={0}, CorrDetThr_G[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD]={0};
	BYTE pwrTbl[200]={0};
	int ChNumList[MAXSAVEDCH_G_REVD]={0};

	status = Dut_Shared_GetAnnexType49Data(
#if defined (_MIMO_)
					DeviceId,		PathId,		
#endif // #if defined (_MIMO_)
					&NumOfTxPowerLevelStore,
					&NumOfSavedCh,
					&CalMethod,
					&CloseTXPWDetection,
					&ExtPAPolarize,
					&CalXtal,
					&PdetHiLoGap,
					&MCS20MPdetOffset,
					&MCS20MInitPOffset,
					&MCS40MPdetOffset,
					&MCS40MInitPOffset,
					ChNumList,
					CorrInitPwr_B, 
					CorrDetThr_B,
					CorrInitPwr_G, 
					CorrDetThr_G,
					pwrTbl);

	//power table

	// write to file use write Power Table file func
	if(ERROR_DATANOTEXIST == status && !Dut_Shared_GetWriteTemplateFlag())  
	{
		status =ERROR_NONE;
  		return status ;
	}
	else
	{	
		int BandId=0, SubBandId=0;
		int HighFreqBondary=2480,LowFreqBondary=2410;
		int sizeOfPwrTblInByte=sizeof(pwrTbl);

 
		sprintf(WlanCalPwrTFileName, "PwrTble_Band0_Subband0_Upload.txt");
		if(!Dut_Shared_GetWriteTemplateFlag()) 
		{
			if(status)	return status ;
		}
 		else 
			status =ERROR_NONE;

		if(0 != HighFreqBondary && 0 != LowFreqBondary)
		if (NULL!= strstr(WlanCalUploadFileName, "swap"))
		{
			Dut_Shared_GetPwrTbleFileName(
#if defined (_MIMO_)
					DeviceId,	PathId, 
#endif // #if defined (_MIMO_)
					BandId,		SubBandId,  
					2, 
					WlanCalPwrTFileName);

			status = Dut_Shared_WritePwrTableFile_RevD(
#if defined (_MIMO_)
						DeviceId,	PathId, 
#endif // #if defined (_MIMO_)
					   BandId,	SubBandId,   
					   HighFreqBondary,  LowFreqBondary,
					   &sizeOfPwrTblInByte, pwrTbl, WlanCalPwrTFileName); 
		}
		else
			status = Dut_Shared_WritePwrTableFile_RevD(
#if defined (_MIMO_)
						DeviceId,	PathId, 
#endif // #if defined (_MIMO_)			
					   BandId, SubBandId,   
					   HighFreqBondary,  LowFreqBondary,  
					   &sizeOfPwrTblInByte, pwrTbl, WlanCalPwrTFileName ); 

	}

	status = Dut_Shared_WriteAnnex49DataToFile(FileName,
#if defined (_MIMO_)
					DeviceId,		PathId,		
#endif // #if defined (_MIMO_)
					&NumOfTxPowerLevelStore,
					&NumOfSavedCh,
					&CalMethod,
					&CloseTXPWDetection,
					&ExtPAPolarize,
					&CalXtal,
					&PdetHiLoGap,
					&MCS20MPdetOffset,
					&MCS20MInitPOffset,
					&MCS40MPdetOffset,
					&MCS40MInitPOffset,
					ChNumList,
					CorrInitPwr_B, 
					CorrDetThr_B,
					CorrInitPwr_G, 
					CorrDetThr_G,
					pwrTbl); 
	return status;
}

int LoadSaveCalAnnex49(char *FileName
#if defined (_MIMO_)
				,	int	DeviceId,	int PathId 		
#endif // #if defined (_MIMO_) 
)
{
	int  tempInt=0, i=0, j=0; 
	char line[256]="", tag[256]="", key[256]="";
	
    int status=0;
    BYTE NumOfSavedCh=0, CalXtal=0, NumOfTxPowerLevelStore=0;
	BYTE PdetHiLoGap=0, MCS20MPdetOffset=0, MCS20MInitPOffset=0;
	BYTE MCS40MPdetOffset=0, MCS40MInitPOffset=0;
	bool CalMethod=0, CloseTXPWDetection=0, ExtPAPolarize=0;
	char CorrInitPwr_B[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD]={0}, CorrDetThr_B[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD]={0};
	char CorrInitPwr_G[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD]={0}, CorrDetThr_G[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD]={0};
	BYTE pwrTbl[200]={0};
	int ChNumList[MAXSAVEDCH_G]={0};	

	status = Dut_Shared_ReadAnnex49DataFromFile(FileName,
#if defined (_MIMO_)
					DeviceId,		PathId,		
#endif // #if defined (_MIMO_)
					&NumOfTxPowerLevelStore,
					&NumOfSavedCh,
					&CalMethod,
					&CloseTXPWDetection,
					&ExtPAPolarize,
					&CalXtal,

					&PdetHiLoGap,
					&MCS20MPdetOffset,
					&MCS20MInitPOffset,
					&MCS40MPdetOffset,
					&MCS40MInitPOffset,
					ChNumList,
					CorrInitPwr_B, 
					CorrDetThr_B,
					CorrInitPwr_G, 
					CorrDetThr_G
					);

	//Power table
    {
	int HighFreqBondary=0, LowFreqBondary=0, sizeOfPwrTblInByte; 
	int PpaGain1_Rng[RFPWRRANGE_NUM]={0}, PpaGain2_Rng[RFPWRRANGE_NUM]={0};
	int MaxPwr_Rng[RFPWRRANGE_NUM]={0}, MinPwr_Rng[RFPWRRANGE_NUM]={0}; 
	char tempFile[_MAX_PATH];
	int BandId=0;
	int SubBandId =0;
		
	sprintf(WlanCalPwrTFileName, "PwrTble_Band%d_Subband%d.txt", BandId, SubBandId);
//#ifndef _LINUX_
	getcwd(tempFile, _MAX_PATH);
//	GetCurrentDirectory(_MAX_PATH, tempFile);
	strcat(tempFile, "//");
//#endif //#ifndef _LINUX_
	strcat (tempFile, WlanCalPwrTFileName);
	strcpy(WlanCalPwrTFileName, tempFile);
	if (NULL!= strstr(WlanCalDownLoadFileName, "swap"))
	{
 		Dut_Shared_GetPwrTbleFileName(
#if defined (_MIMO_)
					DeviceId,	PathId, 
#endif // #if defined (_MIMO_)
					BandId, SubBandId,  
					2, 
					WlanCalPwrTFileName);

// load from file use loadPowerTablefile func
	status = Dut_Shared_ReadPwrTableFile_RevD(
#if defined (_MIMO_)
						DeviceId,	PathId, 
#endif // #if defined (_MIMO_)
					   BandId, SubBandId,  
					   &HighFreqBondary,  &LowFreqBondary,
					   &sizeOfPwrTblInByte, pwrTbl, WlanCalPwrTFileName );
	}
	else
		status = Dut_Shared_ReadPwrTableFile_RevD(
#if defined (_MIMO_)
						DeviceId,	PathId, 
#endif // #if defined (_MIMO_)
					   BandId, SubBandId,  
					   &HighFreqBondary,  &LowFreqBondary,
					   &sizeOfPwrTblInByte, pwrTbl, WlanCalPwrTFileName );
	if(status) return status;
	}

	status = Dut_Shared_SetAnnexType49Data(
#if defined (_MIMO_)
					 DeviceId,		 PathId,		
#endif // #if defined (_MIMO_)
					&NumOfTxPowerLevelStore,
					&NumOfSavedCh,
					&CalMethod,
					&CloseTXPWDetection,
					&ExtPAPolarize,
					&CalXtal,		
					&PdetHiLoGap,
					&MCS20MPdetOffset,
					&MCS20MInitPOffset,
					&MCS40MPdetOffset,
					&MCS40MInitPOffset,
					ChNumList,
					CorrInitPwr_B, 
					CorrDetThr_B,
					CorrInitPwr_G, 
					CorrDetThr_G, 
					pwrTbl);

	return status;
}

int LoadSaveCalAnnex50(char *FileName, 
#if defined (_MIMO_)
					   DWORD DeviceId, DWORD PathId, 
#endif // #if defined (_MIMO_)
					   DWORD BandId, DWORD SubBandId)
{
	int HighFreqBondary=0, LowFreqBondary=0, sizeOfPwrTblInByte; 
	int PpaGain1_Rng[RFPWRRANGE_NUM]={0}, PpaGain2_Rng[RFPWRRANGE_NUM]={0};
	int MaxPwr_Rng[RFPWRRANGE_NUM]={0}, MinPwr_Rng[RFPWRRANGE_NUM]={0}; 
	BYTE pwrTbl[200]={0};
 	int status=0;
	char tempFile[_MAX_PATH];

	//50
	BYTE	pNumOfSavedChInSubBand=0, pNumOfTxPowerLevelStore=0, pNumOfSavedCh=0;
	bool	pCalMethod =0, pCloseTXPWDetection=0, pExtPAPolarize=0;
	BYTE	pCalXtal,	pPdetHiLoGap=0, pMCS20MPdetOffset=0, pMCS20MInitPOffset=0, pMCS40MPdetOffset=0, pMCS40MInitPOffset=0;
	BYTE	pSavedChannelIndex[MAXSAVEDCH_G_REVD];
	WORD	pFreqBound_Lo=0, pFreqBound_Hi=0;
	char CorrDetThr_A[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD]={0};
	char CorrInitPwr_A[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD]={0};

	status = Dut_Shared_ReadAnnex50DataFromFile(FileName,
#if defined (_MIMO_)
					DeviceId,		PathId,		
#endif // #if defined (_MIMO_)
	SubBandId, 
	&pNumOfSavedChInSubBand,
	&pNumOfTxPowerLevelStore,
	&pNumOfSavedCh,
	&pCalMethod,
	&pCloseTXPWDetection,
	&pExtPAPolarize,
	&pCalXtal,	
	&pPdetHiLoGap,
	&pMCS20MPdetOffset,
	&pMCS20MInitPOffset,
	&pMCS40MPdetOffset,
	&pMCS40MInitPOffset,
	pSavedChannelIndex,
	&pFreqBound_Lo,
	&pFreqBound_Hi,
	CorrDetThr_A, 
	CorrInitPwr_A
	);	

	if (status == ERROR_DATANOTEXIST)
		return status;

	sprintf(WlanCalPwrTFileName, "PwrTble_Band%d_Subband%d.txt", BandId, SubBandId);
//#ifndef _LINUX_
	getcwd(tempFile, _MAX_PATH);
//	GetCurrentDirectory(_MAX_PATH, tempFile);
	strcat(tempFile, "//");
//#endif //#ifndef _LINUX_
	strcat (tempFile, WlanCalPwrTFileName);
	strcpy(WlanCalPwrTFileName, tempFile);
	if (NULL!= strstr(WlanCalDownLoadFileName, "swap"))
	{
 		Dut_Shared_GetPwrTbleFileName(
#if defined (_MIMO_)
					DeviceId,	PathId, 
#endif // #if defined (_MIMO_)
					BandId, SubBandId,  
					2, 
					WlanCalPwrTFileName);

// load from file use loadPowerTablefile func
		status = Dut_Shared_ReadPwrTableFile_RevD(
#if defined (_MIMO_)
						DeviceId,	PathId, 
#endif // #if defined (_MIMO_)
					   BandId, SubBandId,  
					   &HighFreqBondary,  &LowFreqBondary,
					   &sizeOfPwrTblInByte, pwrTbl, WlanCalPwrTFileName );
	}
	else
		status = Dut_Shared_ReadPwrTableFile_RevD(
#if defined (_MIMO_)
						DeviceId,	PathId, 
#endif // #if defined (_MIMO_)
					   BandId, SubBandId,  
					   &HighFreqBondary,  &LowFreqBondary,
					   &sizeOfPwrTblInByte, pwrTbl, WlanCalPwrTFileName );
	if(status) return status; 

// set annex 16 data
	pFreqBound_Lo =LowFreqBondary;
	pFreqBound_Hi= HighFreqBondary;

	if(HighFreqBondary != 0 && LowFreqBondary != 0)
		status = Dut_Shared_SetAnnexType50Data(
#if defined (_MIMO_)
			DeviceId, PathId,  
#endif // #if defined (_MIMO_)
	SubBandId, 
	&pNumOfSavedChInSubBand,
	&pNumOfTxPowerLevelStore,
	&pNumOfSavedCh,
	&pCalMethod,
	&pCloseTXPWDetection,
	&pExtPAPolarize,
	&pCalXtal,	
	&pPdetHiLoGap,
	&pMCS20MPdetOffset,
	&pMCS20MInitPOffset,
	&pMCS40MPdetOffset,
	&pMCS40MInitPOffset,
	pSavedChannelIndex,
	&pFreqBound_Lo,
	&pFreqBound_Hi,
	CorrDetThr_A, 
	CorrInitPwr_A,
	pwrTbl);
	
	return status;
}

int UpLoadSaveCalAnnex50(char *FileName, 
#if defined (_MIMO_)
						 DWORD DeviceId, DWORD PathId, 
#endif // #if defined (_MIMO_)
						 DWORD BandId, DWORD SubBandId)
{
	BYTE pwrTbl[200]={0};
 	int status=0, sizeOfPwrTblInByte;
	//Annex 50
	BYTE	pNumOfSavedChInSubBand=0, pNumOfTxPowerLevelStore=0, pNumOfSavedCh=0;
	bool	pCal_Method =0, pClose_TX_PW_Detection=0, pExtPAPolarize=0;
	BYTE	pCalXtal,	pPdetHiLoGap=0, pMCS20MPdetOffset=0, pMCS20MInitPOffset=0, pMCS40MPdetOffset=0, pMCS40MInitPOffset=0;
	BYTE	pSavedChannelIndex[MAXSAVEDCH_G_REVD];
	WORD	pFreqBound_Lo=0, pFreqBound_Hi=0;
	char CorrDetThr_A[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD]={0};
	char CorrInitPwr_A[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD]={0};
// get annex 50 data
	status = Dut_Shared_GetAnnexType50Data(
#if defined (_MIMO_)
		DeviceId, PathId,  
#endif // #if defined (_MIMO_)
	SubBandId, 
	&pNumOfSavedChInSubBand,
	&pNumOfTxPowerLevelStore,
	&pCal_Method,
	&pClose_TX_PW_Detection,
	&pExtPAPolarize,
	&pCalXtal,
	&pPdetHiLoGap,
	&pMCS20MPdetOffset,
	&pMCS20MInitPOffset,
	&pMCS40MPdetOffset,
	&pMCS40MInitPOffset,
	pSavedChannelIndex,	
	&pFreqBound_Lo,
	&pFreqBound_Hi,
	CorrDetThr_A, 
	CorrInitPwr_A, 	
	pwrTbl); 
 // write to file use write Power Table file func
	if(ERROR_DATANOTEXIST == status && !Dut_Shared_GetWriteTemplateFlag())  
	{
		status =ERROR_NONE;
  		return status ;
	}
	else
	{	
 
		sprintf(WlanCalPwrTFileName, "PwrTble_Band%d_Subband%d_Upload.txt", BandId, SubBandId);
		if(!Dut_Shared_GetWriteTemplateFlag()) 
		{
			if(status)	return status ;
		}
 		else 
			status =ERROR_NONE;

		if(0 != pFreqBound_Lo && 0 != pFreqBound_Hi)
		if (NULL!= strstr(WlanCalUploadFileName, "swap"))
		{
			Dut_Shared_GetPwrTbleFileName(
#if defined (_MIMO_)
					DeviceId,	PathId, 
#endif // #if defined (_MIMO_)
					BandId,		SubBandId,  
					2, 
					WlanCalPwrTFileName);

			status = Dut_Shared_WritePwrTableFile_RevD(
#if defined (_MIMO_)
						DeviceId,	PathId, 
#endif // #if defined (_MIMO_)
					   BandId,	SubBandId,   
					   pFreqBound_Hi,  pFreqBound_Lo,
					   &sizeOfPwrTblInByte, pwrTbl, WlanCalPwrTFileName); 

		}

		else

			status = Dut_Shared_WritePwrTableFile_RevD(
#if defined (_MIMO_)
						DeviceId,	PathId, 
#endif // #if defined (_MIMO_)			
					   BandId, SubBandId,   
					   pFreqBound_Hi,  pFreqBound_Lo,  
					   &sizeOfPwrTblInByte, pwrTbl, WlanCalPwrTFileName ); 

	}

			Dut_Shared_WriteAnnex50DataToFile(FileName,
#if defined (_MIMO_)
				DeviceId,	PathId,		
#endif // #if defined (_MIMO_)
				SubBandId, 
				&pNumOfSavedChInSubBand,
				&pNumOfTxPowerLevelStore,
				&pNumOfSavedCh,
				&pCal_Method,
				&pClose_TX_PW_Detection,
				&pExtPAPolarize,
				&pCalXtal,	
				&pPdetHiLoGap,
				&pMCS20MPdetOffset,
				&pMCS20MInitPOffset,
				&pMCS40MPdetOffset,
				&pMCS40MInitPOffset,
				pSavedChannelIndex,
				&pFreqBound_Lo,
				&pFreqBound_Hi,
				CorrDetThr_A, 
				CorrInitPwr_A);
			
	return status;
}

#endif //#if defined (_WLAN_)

int LoadSaveCalAnnex57(char *FileName,	
#if defined (_MIMO_)
				int	DeviceId,	int PathId,		
#endif // #if defined (_MIMO_) 
					 BYTE BandId, BYTE SubBandId)
{

#if !defined (_MIMO_) 
	int	DeviceId =0, PathId=0;
#endif // #if !defined (_MIMO_) 
	int  tempInt=0, i=0, j=0; 
	char line[256]="", tag[256]="", key[256]="";
	
    int status=0;
	BYTE MEM_ROW=0;
	BYTE NumOfEnterirs=0;
 	
	DPD_ConeffcicentData Coneffcicent_Data[20];

		
	status = Dut_Shared_ReadAnnex57DataFromFile(FileName,
#if defined (_MIMO_)
					DeviceId, PathId,		
#endif // #if defined (_MIMO_)
					BandId,
					SubBandId,
					&MEM_ROW,
					&NumOfEnterirs,
					Coneffcicent_Data);
    
	if(status) return status;
 
status = Dut_Shared_SetAnnexType57Data(
#if defined (_MIMO_)
					   DeviceId,		PathId,  
#endif //#if defined (_MIMO_)
					BandId, 
					SubBandId,
					MEM_ROW,
					NumOfEnterirs,
					Coneffcicent_Data);
	return status;
}

#if defined (_BT_)
int ChangeUartRate(int NewBaudRate)
{
	int status=0;
	DWORD UartBaudRate=DEFVALUE_BT_UARTBAUDRATE;
	int CsC=0, CsValue=0; 
 	BYTE BdAddress[SIZE_BDADDRESS_INBYTE] = {0x00, 0x50, 0x43, 0x20, 0xFF, 0xFF};
 	BYTE Version=0, RFXtal=0, InitPwr=DEFVALUE_BT_INITPWR, FELoss=0;
				 
	bool ForceClass2Op=DEFVALUE_BT_FORCECLASS20, DisablePwrControl=DEFVALUE_BT_DISABLE_PWR_CONTROL, MiscFlag=DEFVALUE_BT_MISC_FLAG;
	bool UsedInternalSleepClock=DEFVALUE_BT_USED_INTERNAL_SLEEP_CLOCK;
	BYTE Rssi_Golden_Lo=DEFVALUE_BT_RSSIGOLDENLOW, Rssi_Golden_Hi=DEFVALUE_BT_RSSIGOLDENHIGH;
 	BYTE Encr_Key_Len_Max=DEFVALUE_BT_ENCRYP_KEY_LEN_MAX, Encr_Key_Len_Min=DEFVALUE_BT_ENCRYP_KEY_LEN_MIN;

	status =Dut_Bt_GetCalDataRevC(&CsC, &CsValue);
	if(status)	return status ;
 	if(!(CsC))	return status ;

 	status = Dut_Shared_GetAnnexType55Data( 
 				&Version, &RFXtal,
				&InitPwr, &FELoss,

				&ForceClass2Op,
				&DisablePwrControl,
				&MiscFlag,
				&UsedInternalSleepClock,
				&Rssi_Golden_Lo,
				&Rssi_Golden_Hi,
				&UartBaudRate,
				BdAddress,
				&Encr_Key_Len_Max,
				&Encr_Key_Len_Min);

	if(ERROR_DATANOTEXIST == status)  
	{
		DebugLogRite("BT Cal data is not present in EEPROM\n");
		return status;
	}

	UartBaudRate = NewBaudRate;

 	status = Dut_Shared_SetAnnexType55Data( 
 				Version, RFXtal,
				InitPwr, FELoss,

				ForceClass2Op,
				DisablePwrControl,
				MiscFlag,
				UsedInternalSleepClock,
				Rssi_Golden_Lo,
				Rssi_Golden_Hi,
				UartBaudRate,
				BdAddress,
				Encr_Key_Len_Max,
				Encr_Key_Len_Min);
	if(status)	return status ;

	status = Dut_Bt_UpdateCalDataRevC_Bt(1);

	return status;
}
 
int ChangeBdAddress(BYTE *pNewBdAddress)
{
	int status=0;
	DWORD UartBaudRate=DEFVALUE_BT_UARTBAUDRATE;
	int CsC=0, CsValue=0; 
	BYTE pBdAddress[SIZE_BDADDRESS_INBYTE]={0}; 
  
 	BYTE Version=0, RFXtal=0, InitPwr=DEFVALUE_BT_INITPWR, FELoss=0;
				 
	bool ForceClass2Op=DEFVALUE_BT_FORCECLASS20, DisablePwrControl=DEFVALUE_BT_DISABLE_PWR_CONTROL, MiscFlag=DEFVALUE_BT_MISC_FLAG;
	bool UsedInternalSleepClock=DEFVALUE_BT_USED_INTERNAL_SLEEP_CLOCK;
	BYTE Rssi_Golden_Lo=DEFVALUE_BT_RSSIGOLDENLOW, Rssi_Golden_Hi=DEFVALUE_BT_RSSIGOLDENHIGH;
 	BYTE Encr_Key_Len_Max=DEFVALUE_BT_ENCRYP_KEY_LEN_MAX, Encr_Key_Len_Min=DEFVALUE_BT_ENCRYP_KEY_LEN_MIN;
 	
 	status =Dut_Bt_GetCalDataRevC(&CsC, &CsValue);
 	if(status)	return status ;
 	if(!(CsC))	return status ;

 	status = Dut_Shared_GetAnnexType55Data( 
 				&Version, &RFXtal,
				&InitPwr, &FELoss,

				&ForceClass2Op,
				&DisablePwrControl,
				&MiscFlag,
				&UsedInternalSleepClock,
				&Rssi_Golden_Lo,
				&Rssi_Golden_Hi,
				&UartBaudRate,
				pBdAddress,
				&Encr_Key_Len_Max,
				&Encr_Key_Len_Min);
	if(ERROR_DATANOTEXIST == status)  
	{
		DebugLogRite("BT Cal data is not present in EEPROM\n");
		return status;
	}

	memcpy(pBdAddress, pNewBdAddress,  sizeof(BYTE)*SIZE_BDADDRESS_INBYTE);

 	status = Dut_Shared_SetAnnexType55Data( 
				Version, RFXtal,
				InitPwr, FELoss,

				ForceClass2Op,
				DisablePwrControl,
				MiscFlag,
				UsedInternalSleepClock,
				Rssi_Golden_Lo,
				Rssi_Golden_Hi,
				UartBaudRate,
				pBdAddress,
				Encr_Key_Len_Max,
				Encr_Key_Len_Min);
	if(status)	return status ;

	status = Dut_Bt_UpdateCalDataRevC_Bt(1);

	return status;
}

int GetBdAddressFromE2p(BYTE *pBdAddress)
{
	int status=0;
	DWORD UartBaudRate=DEFVALUE_BT_UARTBAUDRATE;
	int CsC=0, CsValue=0; 

 	BYTE Version=0, RFXtal=0, InitPwr=DEFVALUE_BT_INITPWR, FELoss=0;
				 
	bool ForceClass2Op=DEFVALUE_BT_FORCECLASS20, DisablePwrControl=DEFVALUE_BT_DISABLE_PWR_CONTROL, MiscFlag=DEFVALUE_BT_MISC_FLAG;
	bool UsedInternalSleepClock=DEFVALUE_BT_USED_INTERNAL_SLEEP_CLOCK;
	BYTE Rssi_Golden_Lo=DEFVALUE_BT_RSSIGOLDENLOW, Rssi_Golden_Hi=DEFVALUE_BT_RSSIGOLDENHIGH;
 	BYTE Encr_Key_Len_Max=DEFVALUE_BT_ENCRYP_KEY_LEN_MAX, Encr_Key_Len_Min=DEFVALUE_BT_ENCRYP_KEY_LEN_MIN;

 	status =Dut_Bt_GetCalDataRevC(&CsC, &CsValue);
 	if(status)	return status ;
 	if(!(CsC))	return status ;

 	status = Dut_Shared_GetAnnexType55Data( 
 				&Version, &RFXtal,
				&InitPwr, &FELoss,

				(bool*)&ForceClass2Op,
				(bool*)&DisablePwrControl,
				(bool*)&MiscFlag,
				(bool*)&UsedInternalSleepClock,
				&Rssi_Golden_Lo,
				&Rssi_Golden_Hi,
				&UartBaudRate,
				pBdAddress,
				&Encr_Key_Len_Max,
				&Encr_Key_Len_Min);
	if(ERROR_DATANOTEXIST == status)  
	{
		DebugLogRite("BT Cal data is not present in EEPROM\n");
		return status;
	}

	return status;
}
#endif // #if defined (_BT_)
 
#endif // #if !defined(_NO_EEPROM_)

/////////////////////////////////

 
