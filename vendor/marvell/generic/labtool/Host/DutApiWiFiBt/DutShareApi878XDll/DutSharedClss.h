/** @file DutSharedClss.h
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#ifndef _DUT_SHARE_API_H_
#define _DUT_SHARE_API_H_
//#include "DutIf_IfClss.h"
#include "../DutBtApi.hc" 
#include "DutShared_Caldata.hd" 
//#if !defined(AFX_NDISIF_H__90DC2111_AEB5_435E_B719_6A54BEB076A6__INCLUDED_)
//#define AFX_NDISIF_H__90DC2111_AEB5_435E_B719_6A54BEB076A6__INCLUDED_

#ifndef _LINUX_
#ifndef _NO_CLASS_
class DutSharedClss
#endif //#ifndef _NO_CLASS_
{
int g_NoEeprom;
char g_NoEepromWlanFlexFileName[_MAX_PATH];
char g_NoEepromBtFlexFileName[_MAX_PATH];

int DeBugLog;
int g_writeTemplate; 

void *pAnnex24;//[MAXNUM_MIMODEVICE];
void *pAnnex25;//[MAXNUM_MIMODEVICE];
void *pAnnex27;
void *pAnnex28;
void *pAnnex31[MAXNUM_MIMODEVICE];//[MAXNUM_RXPATH];
void *pAnnex32[MAXNUM_MIMODEVICE];//[MAXNUM_RXPATH];
void *pAnnex40;
void *pAnnex42;
void *pAnnex45;
void *pAnnex46;//[MAXNUM_MIMODEVICE];
void *pAnnex47[MAXNUM_MIMODEVICE][MAXNUM_RXPATH];  
void *pAnnex48[MAXNUM_MIMODEVICE][MAXNUM_RXPATH];
//REV D
void *pAnnex49[MAXNUM_MIMODEVICE][MAXNUM_TXPATH];
void *pAnnex50[MAXNUM_MIMODEVICE][MAXNUM_TXPATH][NUM_SUBBAND_A];
void *pAnnex51[MAXNUM_MIMODEVICE][MAXNUM_TXPATH];
void *pAnnex52[MAXNUM_MIMODEVICE][MAXNUM_RXPATH];  
void *pAnnex53[MAXNUM_MIMODEVICE][MAXNUM_TXPATH];
void *pAnnex54[MAXNUM_MIMODEVICE][MAXNUM_TXPATH];
void *pAnnex55; //[MAXNUM_MIMODEVICE][MAXNUM_TXPATH];
void *pAnnex56; //[MAXNUM_MIMODEVICE][MAXNUM_TXPATH];
void *pAnnex57[MAXNUM_MIMODEVICE][MAXNUM_RXPATH][NUM_SUBBAND_A_REVD+NUM_SUBBAND_G_REVD];
void *pMain;
#else	//_LINUX_ 

extern int g_NoEeprom;
extern char g_NoEepromWlanFlexFileName[_MAX_PATH];
extern char g_NoEepromBtFlexFileName[_MAX_PATH];

extern int DeBugLog;
extern int g_writeTemplate; 

extern 	void *pAnnex24;//[MAXNUM_MIMODEVICE];
extern 	void *pAnnex25;//[MAXNUM_MIMODEVICE];
extern 	void *pAnnex27;
extern 	void *pAnnex28;
extern 	void *pAnnex31[MAXNUM_MIMODEVICE];//[MAXNUM_RXPATH];
extern 	void *pAnnex32[MAXNUM_MIMODEVICE];//[MAXNUM_RXPATH];
extern 	void *pAnnex40;
extern 	void *pAnnex42;
extern 	void *pAnnex45;
extern 	void *pAnnex46;//[MAXNUM_MIMODEVICE];
extern 	void *pAnnex47[MAXNUM_MIMODEVICE][MAXNUM_RXPATH];  
extern 	void *pAnnex48[MAXNUM_MIMODEVICE][MAXNUM_RXPATH];

//Rev D
extern void *pAnnex49[MAXNUM_MIMODEVICE][MAXNUM_TXPATH];
extern 	void *pAnnex50[MAXNUM_MIMODEVICE][MAXNUM_TXPATH][NUM_SUBBAND_A];
extern 	void *pAnnex51[MAXNUM_MIMODEVICE][MAXNUM_TXPATH];
extern 	void *pAnnex52[MAXNUM_MIMODEVICE][MAXNUM_RXPATH];  
extern 	void *pAnnex53[MAXNUM_MIMODEVICE][MAXNUM_TXPATH];
extern 	void *pAnnex54[MAXNUM_MIMODEVICE][MAXNUM_RXPATH]; 
extern 	void *pAnnex55;//[MAXNUM_MIMODEVICE][MAXNUM_TXPATH];
extern 	void *pAnnex56;//[MAXNUM_MIMODEVICE][MAXNUM_TXPATH];

extern  void *pAnnex57[MAXNUM_MIMODEVICE][MAXNUM_TXPATH][NUM_SUBBAND_A_REVD+NUM_SUBBAND_G_REVD];
extern 	void *pMain;

#endif	//#ifndef _LINUX_
#ifndef _NO_CLASS_
private:
#if defined(_BT_)
friend class DutBtApiClss;
#endif	//#if defined(_BT_)
friend class DutWlanApiClss;
#endif //#ifndef _NO_CLASS_
 
int SetAnnexTblInfo(int deviceType, void*pObj, 
										ANNEX_TYPE_HEADER_VC	*pAnnex ,
									   int LenofAnnex, int *pNextOffsetInMem, 
									   int *pUsedSize, int *pTotalSize, 
#ifdef _FLASH_
									   bool lastAnnex,
#endif //#ifdef _FLASH_
									   char *pFlexFileNameNoE2prom);
int SpiGetHeaderTbl( int deviceType, void *pObj, 
										DWORD *HeaderTbl);
int SpiSetHeaderTbl(int deviceType, void *pObj, 
										DWORD *HeaderTbl);

#ifndef _NO_CLASS_
public:
DutSharedClss();
~DutSharedClss();
#endif //#ifndef _NO_CLASS_

int GetWriteTemplateFlag(void);
void SetWriteTemplateFlag(int Flag);

BYTE DoCs1(BYTE *TblData, DWORD length);
void DoCs1Check(BYTE *TblData, DWORD length, int *CsC);
void DoCs0(DWORD *TblData, DWORD length);
void DoCs0Check(DWORD *TblData, DWORD length, int *CsC);

int CheckHeaderTbl(DWORD HeaderTbl[], DWORD *Version, int *CsC);
bool SupportNoE2PROM(void); 
//ULONG  GetDebugLogFlag(void);
int SpiGetBoot2Version(int deviceType, void *pObj,  DWORD *pBoot2Version);
int SpiGetProdFwVersion(int deviceType, void *pObj, DWORD *pProdFwVersion);

int GetAnnexType27Data(BYTE *pLED);
int SetAnnexType27Data(BYTE *pLED);
int GetAnnexType28Data(	BYTE *pUsbVendorString, 
						BYTE *pUsbProductString);
int SetAnnexType28Data(BYTE *pUsbVendorString, 
						BYTE *pUsbProductString);
int GetAnnexType31Data(
#if defined (_MIMO_)
 						   int DeviceId, 
#endif //#if defined (_MIMO_)
						//int PathId, int BandId,
						char pRssi_Nf[NUM_SUBBAND_G], 
						char pRssi_cck[NUM_SUBBAND_G], 
						char pRssi_ofdm[NUM_SUBBAND_G], 
						char pRssi_mcs[NUM_SUBBAND_G]);
int SetAnnexType31Data(	
#if defined (_MIMO_)
 						   int DeviceId, 
#endif //#if defined (_MIMO_)
					   //int PathId, int BandId,
						char pRssi_Nf[NUM_SUBBAND_G], 
						char pRssi_cck[NUM_SUBBAND_G], 
						char pRssi_ofdm[NUM_SUBBAND_G], 
						char pRssi_mcs[NUM_SUBBAND_G]); 
int GetAnnexType32Data(//int *CsC, 
#if defined (_MIMO_)
 						   int DeviceId, 
#endif //#if defined (_MIMO_)
					   //int PathId, int BandId,
						char pRssi_Nf[NUM_SUBBAND_A], 
						char pRssi_cck[NUM_SUBBAND_A], 
						char pRssi_ofdm[NUM_SUBBAND_A], 
						char pRssi_mcs[NUM_SUBBAND_A]); 

int SetAnnexType32Data(	
#if defined (_MIMO_)
 						   int DeviceId, 
#endif //#if defined (_MIMO_)
					   //int PathId, int BandId,
						char pRssi_Nf[NUM_SUBBAND_A], 
						char pRssi_cck[NUM_SUBBAND_A], 
						char pRssi_ofdm[NUM_SUBBAND_A], 
						char pRssi_mcs[NUM_SUBBAND_A]); 

int GetAnnexType40Data( WORD *pVid,		WORD *pPid,	
						BYTE *pChipId,	BYTE *pChipVersion,					   
						BYTE *pFlag_SettingPrev, 
						BYTE *pNumOfRailAvaiableInChip,
						BYTE *pPowerDownMode,
						BYTE pNormalMode[MAX_PM_RAIL_PW886], 
						BYTE pSleepModeFlag[MAX_PM_RAIL_PW886],
						BYTE pSleepModeSetting[MAX_PM_RAIL_PW886],
						BYTE *pDpSlp);
int SetAnnexType40Data(	WORD *pVid,		WORD *pPid,	
						BYTE *pChipId,	BYTE *pChipVersion,					   
						BYTE *pFlag_SettingPrev, 
						BYTE *pNumOfRailAvaiableInChip,
						BYTE *pPowerDownMode,
						BYTE pNormalMode[MAX_PM_RAIL_PW886], 
						BYTE pSleepModeFlag[MAX_PM_RAIL_PW886],
						BYTE pSleepModeSetting[MAX_PM_RAIL_PW886],
						BYTE *pDpSlp);

int GetAnnexType45Data(	bool *pFlag_GpioCtrlXosc,
						BYTE *pXoscSettling,
						DWORD *pPinOut_DrvLow,
						DWORD *pPinIn_PullDiable);
int SetAnnexType45Data(
						bool Flag_GpioCtrlXosc,
					   BYTE XoscSettling,
					   DWORD *pPinOut_DrvLow,
					   DWORD *pPinIn_PullDiable
						);
#if defined (_MIMO_)

int GetAnnexType47Data( int DeviceId, 
						int PathId, //int BandId,
						char Rssi_Nf[NUM_SUBBAND_G], 
						char Rssi_cck[NUM_SUBBAND_G], 
						char Rssi_ofdm[NUM_SUBBAND_G], 
						char Rssi_mcs[NUM_SUBBAND_G]);
int SetAnnexType47Data(	int DeviceId, 
						int PathId, //int BandId,
						char Rssi_Nf[NUM_SUBBAND_G], 
						char Rssi_cck[NUM_SUBBAND_G], 
						char Rssi_ofdm[NUM_SUBBAND_G], 
						char Rssi_mcs[NUM_SUBBAND_G]); 
int GetAnnexType48Data( int DeviceId, 
						int PathId, //int BandId,
						char Rssi_Nf[NUM_SUBBAND_A], 
						char Rssi_cck[NUM_SUBBAND_A], 
						char Rssi_ofdm[NUM_SUBBAND_A], 
						char Rssi_mcs[NUM_SUBBAND_A]); 

int SetAnnexType48Data(	
 						int DeviceId, 
						int PathId, //int BandId,
						char Rssi_Nf[NUM_SUBBAND_A], 
						char Rssi_cck[NUM_SUBBAND_A], 
						char Rssi_ofdm[NUM_SUBBAND_A], 
						char Rssi_mcs[NUM_SUBBAND_A]); 
#endif //#if defined (_MIMO_)

int GetCalDataRevC(int deviceType, void *pObj, int *CsC, int *CsCValue, char *FlexFileNameNoE2prom);
int SetCalDataRevC(int deviceType, void *pObj, int PurgeAfter, char *FlexFileNameNoE2prom);
int UpdateCalDataRevC_Bt(int deviceType, void *pObj, 
											 int PurgeAfter, char *FlexFileNameNoE2prom); 

int PurgeAnnexPointers(int FlagFree=1); 

int Spi_GetHeaderTbl(int deviceType, void *pObj,  DWORD *HeaderTbl);
int Spi_SetHeaderTbl(int deviceType, void *pObj,  DWORD *HeaderTbl);
int getCalMainTbl(int deviceType, void *pObj, DWORD CalData[], DWORD *sizeOfTblInByte);
int setCalMainTbl(int deviceType, void *pObj, DWORD *DataArray, DWORD *sizeOfTblInByte);
int GetHeaderTbl(int deviceType, void *pObj, DWORD HeaderTbl[], DWORD *Version, int *CsC);
int GetCalTbl(int deviceType, void *pObj, 
								  DWORD *CalData,  DWORD *length, int *CsC);
int GetHeaderPresent(int deviceType, void *pObj, int *pPresent, DWORD *pVersion, int *CsC);
int CheckCalDataSpace(int deviceType, void *pObj, int* pAvalable);
int GetEepromSize(int deviceType, void *pObj, int *pSizeInKb, int *pMaxAddress);
int SetHeaderTbl(int deviceType, void *pObj, DWORD HeaderTbl[]);
int GetNextAnnexPtr(int type, void *pObj, int myPtr, int *nextPointer);
int GetFirstAnnexInfo(int deviceType, void *pObj, 
					  int *pointer, int *sizeOfTblInByte, int* type,
							 int *DeviceId, int *PathId, 
							 int *BandId,	int *SubBandId);
 int GetAnnexInfo(int deviceType, void *pObj, 
										int pointer,	int *sizeOfTblInByte, int* type,
							 int *DeviceId, int *PathId, 
							 int *BandId,	int *SubBandId); 
int GetMainAnnexTbl(int deviceType, void *pObj, 
										int *sizeOfTblInByte, int offsetInMem, 
								DWORD* tbl, int *CsC);
int SetMainAnnexTbl(int deviceType, void*pObj, 
										int *sizeOfTblInByte, int offsetInMem, 
										DWORD* tbl);
#define setMainAnnexTbl SetMainAnnexTbl
#define getMainAnnexTbl GetMainAnnexTbl
int GetNextOpenSpace(int deviceType, void*pObj, 
										 int PartMaxAddress, bool includeCal, 
										 int sizeOfTblInByte, int* pointer);


int FindLastAnnex(int deviceType, void*pObj, 
					DWORD* pLastAnnexPointer, 
					int *pAnnexType, DWORD* pEndOfAnnexPointer);
int FindBtAnnex(int deviceType, void*pObj, 
				DWORD* pointer, int *pAnnexType, int *pAnnexSize);

int AddBtAnnex(int deviceType, void *pObj, 
				DWORD offsetInMemAnnex, 
				int PurgeAfter);

int UpdateBtAnnex(int deviceType, void*pObj, 
					DWORD offsetInMemAnnex, 
					DWORD NextOffsetInMemAnnex, 
					int PurgeAfter);

int UpdateAnnexHeader(int deviceType, void*pObj, 
										  DWORD offsetInMemLastAnnex, 
										  DWORD NewOffsetInMemAnnex);
int GetBdAddressFromE2P(int deviceType, void*pObj, 
						BYTE BdAddress[SIZE_BDADDRESS_INBYTE]);

int LoadCustomizedSettings(int deviceType, void*pObj);

int FlexSpiDL(int deviceType, void*pObj, bool erase, char *fileName);
int FlexSpiCompare(int deviceType, void*pObj, char *fileName);  
int CompareE2PData(int deviceType, void*pObj, 
					DWORD  *RefArray,int SizeInByte);



int DumpE2PData(int deviceType, void *pObj, 
									DWORD StartAddress, 
									DWORD LenInDw,
									DWORD *pAddressArray, 
									DWORD *pDataArray);
int WriteLumpedData(int deviceType, void *pObj, 
										DWORD StartAddress, 
								   DWORD LenInDw, 
									DWORD *pDataArray);




int SetUsbAttributes(int deviceType, void *pObj, 
										 BYTE Attribute, BYTE MaxPower);
int GetUsbAttributes(int deviceType, void *pObj, 
										 BYTE *pAttribute, BYTE *pMaxPower);

///////////////////////////////////////////////////////////////
int GetPwrTbleFileName(
#if defined (_MIMO_)
						int DeviceId, int PathId, 
#endif // #if defined (_MIMO_)
					   int BandId, int SubBandId,   
						int Upload, char* pGivenFileName);
 
int ReadPwrTableFile(
#if defined (_MIMO_)
						int DeviceId, int PathId, 
#endif // #if defined (_MIMO_)
					 int BandId, int SubBandId,  
					   int *pSizeOfTblInByte, BYTE* pPwrTbl, 
					   char* pGivenFileName=NULL); 
int WritePwrTableFile(
#if defined (_MIMO_)
						int DeviceId, int PathId, 
#endif // #if defined (_MIMO_)
					  int BandId, int SubBandId,  
					   int *pSizeOfTblInByte, BYTE* pPwrTbl, 
					   char* pGivenFileName=NULL); 

int ReadPwrTableFile(
#if defined (_MIMO_)
						int DeviceId, int PathId, 
#endif // #if defined (_MIMO_)
					 int BandId, int SubBandId,  
					 int *pHighFreqBondary, int *pLowFreqBondary,
					 int pPpaGain1_Rng[RFPWRRANGE_NUM], 
					   int pPpaGain2_Rng[RFPWRRANGE_NUM],  
					   int pMaxPwr_Rng[RFPWRRANGE_NUM], 
					   int pMinPwr_Rng[RFPWRRANGE_NUM], 
					   int *pSizeOfPwrTblInByte, BYTE* pPwrTbl, 
					   char* GivenFileName=NULL); 
int WritePwrTableFile(
#if defined (_MIMO_)
						int DeviceId, int PathId, 
#endif // #if defined (_MIMO_)
					  int BandId, int SubBandId,   
					  int HighFreqBondary, int LowFreqBondary,
					   int pPpaGain1_Rng[RFPWRRANGE_NUM], 
					   int pPpaGain2_Rng[RFPWRRANGE_NUM],  
					   int pMaxPwr_Rng[RFPWRRANGE_NUM], 
					   int pMinPwr_Rng[RFPWRRANGE_NUM], 
					   int *pSizeOfPwrTblInByte, BYTE* pPwrTbl, 
					   char* pGivenFileName=NULL); 

//////////////////////
int InitCalDataFile(char *FileName);
int UpDateLenCalDataFile(char *FileName, int AddLength);
int ReadCalDataFile(char *FileName, DWORD StartAddress, 
								   DWORD LenInDw, 
									DWORD *pDataArray);
int WriteCalDataFile(char *FileName, DWORD StartAddress, 
								   DWORD LenInDw, 
									DWORD *pDataArray);

///////////////////////
int ReadPwrTableFile_D(
#if defined (_MIMO_)
						int DeviceId, int PathId, 
#endif // #if defined (_MIMO_)
						int BandId, int SubBandId,  
						int *pSizeOfTblInByte, BYTE* pPwrTbl, 
						char* pGivenFileName); 
int ReadPwrTableFile_RevD(
#if defined (_MIMO_)
						int DeviceId, int PathId, 
#endif // #if defined (_MIMO_)
						int BandId, int SubBandId,  
						int *pHighFreqBondary, int *pLowFreqBondary,
					   int *pSizeOfPwrTblInByte, BYTE* pPwrTbl, 
					   char* pGivenFileName);
int WritePwrTableFile_RevD(
#if defined (_MIMO_)
						int DeviceId, int PathId, 
#endif // #if defined (_MIMO_)
					  int BandId, int SubBandId,   
					  int HighFreqBondary, int LowFreqBondary,
					   int *pSizeOfPwrTblInByte, BYTE* pPwrTbl, 
					   char* pGivenFileName=NULL); 

int ReadMainDataFromFile_RevD( char *FileName,
						BYTE *pStructRev,
						BYTE *pDesignType,
						BYTE *pLNAGain,
						BYTE *pAntDiversityInfo,
						BYTE *FEMVersion,

						//PA option
						bool *pPaOption_ExtPaPolar_PathABC_G,
						bool *pPaOption_ExtPaPolar_PathABC_A,

						BYTE *pSpiSizeInfo,
					    BYTE *pCC,
						
						BYTE *pExtXtalSource,		
						BYTE *pExtSleepClk,	
						BYTE *pWlanWakeUp,
						

						BYTE pAntTx[MAXNUM_MIMODEVICE],		//
						BYTE pAntRx[MAXNUM_MIMODEVICE],		//

						//Capabilities
						bool *pCapable_2G, //
						bool *pCapable_5G, //
						bool *pInternalBluetooth,	//
						bool *pAntennaDiversity,
						bool *pLNA_2G,	//
						bool *pLNA_5G,
						bool *pSameAnt_2G_BT,
						bool *pSameAnt_5G_BT,

						
						//Concurrency
						BYTE *pCapabilities_2G_BT,
						BYTE *pCapabilities_5G_BT,

						BYTE  pBTFrontEnd2G[9],
						BYTE  pBTFrontEnd5G[9],	
					
						DWORD *pTestToolVer,	DWORD *pMfgTaskVersion,	
						DWORD *pDllVersion, 
						BYTE *pSocOrRev, BYTE *pRfuOrRev,
						BYTE *pMimoTxCnt, 	BYTE *pMimoRxCnt, 
						BYTE pMimoMap[MAXNUM_MIMODEVICE][MAXNUM_RXPATH]);

int WriteMainDataToFile_RevD(char *FileName,
						BYTE pStructRev,
						BYTE pDesignType,
						BYTE pLNAGain,
						BYTE AntDiversityInfo,
						BYTE pFEMVersion,

						bool pPaOption_ExtLnaFlag_PathABC_G,
						bool pPaOption_ExtLnaFlag_PathABC_A,
						//PA option
						bool pPaOption_ExtPaPolar_PathABC_G,
						bool pPaOption_ExtPaPolar_PathABC_A,

						BYTE pSpiSizeInfo,
					    BYTE pCC,
						
						BYTE pExtXtalSource,		
						BYTE pExtSleepClk,	
						BYTE pWlanWakeUp,
						
						BYTE pAntTx[MAXNUM_MIMODEVICE],		//
						BYTE pAntRx[MAXNUM_MIMODEVICE],		//

						//Capabilities
						bool *pCapable_2G, //
						bool *pCapable_5G, //
						bool *pInternalBluetooth,	//
						bool *pAntennaDiversity,
						bool *pLNA_2G,	//
						bool *pLNA_5G,
						bool *pSameAnt_2G_BT,
						bool *pSameAnt_5G_BT,
						
						//Concurrency
						BYTE Capabilities_2G,
						BYTE Capabilities_5G,

						BYTE  BTFrontEnd2G[9],
						BYTE  BTFrontEnd5G[9],	
					
						DWORD pTestToolVer,	DWORD pMfgTaskVersion,	
						DWORD pDllVersion, 
						BYTE pSocOrRev, BYTE pRfuOrRev,
						BYTE pMimoTxCnt, 	BYTE pMimoRxCnt, 
						BYTE pMimoMap[MAXNUM_MIMODEVICE][MAXNUM_RXPATH]);

int ReadMainDataFromFile(char *FileName,
						BYTE *pStructRev, 	
						BYTE *pPcbRev_DesignType,
						BYTE *pPcbRev_MajorRev,
						BYTE *pPcbRev_MinorRev,
						BYTE *pPcbRev_Variation,

						bool pExtPaPolar_Neg_G[MAXNUM_MIMODEVICE], //
						bool pExtPaPolar_Neg_A[MAXNUM_MIMODEVICE], //
						bool pExtLna_G[MAXNUM_MIMODEVICE],	//
						bool pExtLna_A[MAXNUM_MIMODEVICE],	//
						bool pExtLnaPolar_G[MAXNUM_MIMODEVICE],	//
						bool pExtLnaPolar_A[MAXNUM_MIMODEVICE],	//
						BYTE pAntTx[MAXNUM_MIMODEVICE],		//
						BYTE pAntRx[MAXNUM_MIMODEVICE],		//
						BYTE *pBT_Coexist,
						BYTE pFemConfId[MAXNUM_MIMODEVICE],	//
						BYTE pAnt_PartId[MAXNUM_MIMODEVICE][MAXNUM_RXPATH],		//
						BYTE *pLnaGain,
						BYTE *pSpiSizeInfo,		BYTE *pSpiUnitInfo,		
						BYTE *pAssemblyHouse,	DWORD *pSN, 
 						BYTE *pCC,				 
						BYTE *pExtXtalSource,		
						BYTE *pExtSleepClk,	
						BYTE *pWlanWakeUp,	
						WORD *pSocId,			BYTE *pSocVer,			
						BYTE *pBbuId, 			BYTE *pBbuVers, 
						BYTE *pRfId, 			BYTE *pRfVer, 
						DWORD *pTestToolVer,	DWORD *pMfgTaskVersion,	
						DWORD *pDllVersion,
						BYTE *pAppModeFlag,	 
						BYTE *pSocOrRev, BYTE *pRfuOrRev,
						BYTE *pMimoTxCnt, 	BYTE *pMimoRxCnt, 
						BYTE pMimoMap[MAXNUM_MIMODEVICE][MAXNUM_RXPATH]
						);

int WriteMainDataToFile(char *FileName,
						BYTE *pStructRev, 	
						BYTE *pPcbRev_DesignType,
						BYTE *pPcbRev_MajorRev,
						BYTE *pPcbRev_MinorRev,
						BYTE *pPcbRev_Variation,

						bool pExtPaPolar_Neg_G[MAXNUM_MIMODEVICE], //
						bool pExtPaPolar_Neg_A[MAXNUM_MIMODEVICE], //
						bool pExtLna_G[MAXNUM_MIMODEVICE],	//
						bool pExtLna_A[MAXNUM_MIMODEVICE],	//
						bool pExtLnaPolar_G[MAXNUM_MIMODEVICE],	//
						bool pExtLnaPolar_A[MAXNUM_MIMODEVICE],	//
						BYTE pAntTx[MAXNUM_MIMODEVICE],		//
						BYTE pAntRx[MAXNUM_MIMODEVICE],		//
						BYTE *pBT_Coexist,
						BYTE pFemConfId[MAXNUM_MIMODEVICE],	//
						BYTE pAnt_PartId[MAXNUM_MIMODEVICE][MAXNUM_RXPATH],		//
						BYTE *pLnaGain,
						BYTE *pSpiSizeInfo,		BYTE *pSpiUnitInfo,		
						BYTE *pAssemblyHouse,	DWORD *pSN, 
 						BYTE *pCC,				 
						BYTE *pExtXtalSource,		
						BYTE *pExtSleepClk,	
						BYTE *pWlanWakeUp,	
						WORD *pSocId,			BYTE *pSocVer,			
						BYTE *pBbuId, 			BYTE *pBbuVers, 
						BYTE *pRfId, 			BYTE *pRfVer, 
						DWORD *pTestToolVer,	DWORD *pMfgTaskVersion,	
						DWORD *pDllVersion,
						BYTE *pAppModeFlag,	 
						BYTE *pSocOrRev, BYTE *pRfuOrRev,
						BYTE *pMimoTxCnt, 	BYTE *pMimoRxCnt, 
						BYTE pMimoMap[MAXNUM_MIMODEVICE][MAXNUM_RXPATH]);

int ReadAnnex27DataFromFile(char *FileName,BYTE *pLED);
int WriteAnnex27DataToFile(char *FileName,BYTE *pLED);
int ReadAnnex28DataFromFile(char *FileName, 
								   BYTE *pUsbVendorString, 
								   BYTE *pUsbProductString);
int WriteAnnex28DataToFile(char *FileName, 
								   BYTE *pUsbVendorString, 
								   BYTE *pUsbProductString);

int ReadAnnex29DataFromFile(char *FileName, 
							int *pBandSupport2_4,  
							int *pBandSupport4_9, 
							int *pBandSupport5_0,  
							int *pBandSupport5_2);
int WriteAnnex29DataToFile(	char *FileName, 
						    int *pBandSupport2_4,  
							int *pBandSupport4_9, 
							int *pBandSupport5_0,  
							int *pBandSupport5_2);


int ReadAnnex31DataFromFile(char *FileName,
#if defined (_MIMO_)
						   int DeviceId, //int PathId, int BandId,
#endif	// #if defined (_MIMO_)
						char pRssi_Nf[NUM_SUBBAND_G], 
						char pRssi_cck[NUM_SUBBAND_G], 
						char pRssi_ofdm[NUM_SUBBAND_G], 
						char pRssi_mcs[NUM_SUBBAND_G]);
int WriteAnnex31DataToFile(char *FileName,
#if defined (_MIMO_)
						   int DeviceId, //int PathId, int BandId,
#endif	// #if defined (_MIMO_)
						char pRssi_Nf[NUM_SUBBAND_G], 
						char pRssi_cck[NUM_SUBBAND_G], 
						char pRssi_ofdm[NUM_SUBBAND_G], 
						char pRssi_mcs[NUM_SUBBAND_G]);

int ReadAnnex32DataFromFile(char *FileName,
#if defined (_MIMO_)
						   int DeviceId, //int PathId, int BandId,
#endif	// #if defined (_MIMO_)
						char pRssi_Nf[NUM_SUBBAND_A], 
						char pRssi_cck[NUM_SUBBAND_A], 
						char pRssi_ofdm[NUM_SUBBAND_A], 
						char pRssi_mcs[NUM_SUBBAND_A]);
int WriteAnnex32DataToFile(char *FileName,
#if defined (_MIMO_)
						   int DeviceId, //int PathId, int BandId,
#endif	// #if defined (_MIMO_)
						char pRssi_Nf[NUM_SUBBAND_A], 
						char pRssi_cck[NUM_SUBBAND_A], 
						char pRssi_ofdm[NUM_SUBBAND_A], 
						char pRssi_mcs[NUM_SUBBAND_A]);

int ReadAnnex40DataFromFile(char *FileName,
						WORD *pVid,		WORD *pPid,	
						BYTE *pChipId,	BYTE *pChipVersion,					   
						BYTE *pFlag_SettingPrev, 
						BYTE *pNumOfRailAvaiableInChip,
						BYTE *pPowerDownMode,
						BYTE pNormalMode[MAX_PM_RAIL_PW886], 
						BYTE pSleepModeFlag[MAX_PM_RAIL_PW886],
						BYTE pSleepModeSetting[MAX_PM_RAIL_PW886],
						BYTE *pDpSlp);

int WriteAnnex40DataToFile(char *FileName,
						WORD *pVid,		WORD *pPid,	
						BYTE *pChipId,	BYTE *pChipVersion,					   
						BYTE *pFlag_SettingPrev, 
						BYTE *pNumOfRailAvaiableInChip,
						BYTE *pPowerDownMode,
						BYTE pNormalMode[MAX_PM_RAIL_PW886], 
						BYTE pSleepModeFlag[MAX_PM_RAIL_PW886],
						BYTE pSleepModeSetting[MAX_PM_RAIL_PW886],
						BYTE *pDpSlp);

int ReadAnnex42DataFromFile(char *FileName,
					BYTE *pNoOfSubBand,
					BYTE *pBandList,
					BYTE *pSubBandList,
					BYTE *pTempRefMinus40CList,
					BYTE *pTempRef0CList,
					BYTE *pTempRef65CList,
					BYTE *pTempRef85CList,
					BYTE *pPPACoeffList,
					BYTE *pPACoeffList,
					BYTE *pNumeratorList,
					BYTE *pNumerator1List,
					BYTE *pDenomList,
					BYTE *pPTargetDeltaList);

int WriteAnnex42DataToFile(char *FileName,
					BYTE *pNoOfSubBand,
					BYTE *pBandList,
					BYTE *pSubBandList,
					BYTE *pTempRefMinus40CList,
					BYTE *pTempRef0CList,
					BYTE *pTempRef65CList,
					BYTE *pTempRef85CList,
					BYTE *pPPACoeffList,
					BYTE *pPACoeffList,
					BYTE *pNumeratorList,
					BYTE *pNumerator1List,
					BYTE *pDenomList,
					BYTE *pPTargetDeltaList);

int ReadAnnexBtDataFromFile(char *FileName, 
						   BOOL *pLimitPwrAsC2, 
					   BYTE *pGainPpa6dBStepMode,
						char *pPwrIndBm_C2,
						BYTE *pGainPpa_C2,
					   BYTE *pGainPga_C2,
					   BYTE *pGainBb_C2,
						char *pPwrIndBm_C1_5,
					   BYTE *pGainPpa_C1_5,
					   BYTE *pGainPga_C1_5,
					   BYTE *pGainBb_C1_5, 
					   int  *pPwrUpPwrLvl,
					   BYTE *pTempSlope_Gain,
					   BYTE *pTempRd_Ref,
					   BYTE *pFELoss,
					   BYTE *pXtal,

					   BYTE *pMaxPowerdBm_NoRssi,
					   BYTE *pRssiCorrection,
					   DWORD *pUartBaudRate,
					    BYTE *pLocalVersion,
						BYTE *pRemoteVersion,
					   BYTE *pDeepSleepState,
					   bool *pUseEncModeBrcst,
					   bool *pUseUnitKey,
					   bool *pOnChipProfileEnable, 
					   WORD *pDeviceJitter,
					   WORD *pDeviceDrift,
					   WORD *pEncKeyLmax,
					   WORD *pEncKeyLmin,
					   WORD *pMaxAcls,
					   WORD *pMaxScos,
					   WORD *pMaxAclPktLen,
					   WORD *pMaxAclPkts,
					   BYTE *pMaxScoPktLen,
					   WORD *pMaxScoPkts,
					   WORD *pConnRxWnd,
					   BYTE *pRssiGoldenRangeLow,
					   BYTE *pRssiGoldenRangeHigh,
					   BYTE *pCompatibleMode,
						WORD *pLMP_PwrReqMsgInterval,
						BYTE *pLMP_DecrPwrReqMsgIntervalRatio,
						BYTE *pNumOfPwrStep4Clss1p5,
						BYTE *pPwrStepSize4Clss1p5,
						BYTE *pPwrCtrlInClss2Device,
						WORD *pDynamicRssiLmpTimeInterval,
						WORD *pDynamicRssiLmpTimeIntervalThrhld,
					   WORD pLocalSupportedCmds[NUM_WORD_SUPPORTCOMMAND],
					   WORD pLocalSupportedFeatures[NUM_WORD_SUPPORTFEATURE],
					   BYTE	pBdAddress[SIZE_BDADDRESS_INBYTE], 
					   BYTE	pLocalDeviceName[MAXSTRLEN_LOCALDEVICENAME],
					   WORD *pMask,					   
					   WORD pBtAddrList[MAXPARTNER][SIZE_BDADDRESS_INBYTE],
					   WORD pBtKeyList[MAXPARTNER][SIZE_LINKKEY_INBYTE],
					   BYTE pSecurityKey[MAX_BYTE_SECURITYKEY]
						);

int WriteAnnexBtDataToFile(char *FileName, 
						   BOOL *pLimitPwrAsC2, 
					   BYTE *pGainPpa6dBStepMode,
						char *pPwrIndBm_C2,
						BYTE *pGainPpa_C2,
					   BYTE *pGainPga_C2,
					   BYTE *pGainBb_C2,
						char *pPwrIndBm_C1_5,
					   BYTE *pGainPpa_C1_5,
					   BYTE *pGainPga_C1_5,
					   BYTE *pGainBb_C1_5, 
					   int  *pPwrUpPwrLvl,
					   BYTE *pTempSlope_Gain,
					   BYTE *pTempRd_Ref,
					   BYTE *pFELoss,
					   BYTE *pXtal,

					   BYTE *pMaxPowerdBm_NoRssi,
					   BYTE *pRssiCorrection,
					   DWORD *pUartBaudRate,
					    BYTE *pLocalVersion,
						BYTE *pRemoteVersion,
					   BYTE *pDeepSleepState,
					   bool *pUseEncModeBrcst,
					   bool *pUseUnitKey,
					   bool *pOnChipProfileEnable, 
					   WORD *pDeviceJitter,
					   WORD *pDeviceDrift,
					   WORD *pEncKeyLmax,
					   WORD *pEncKeyLmin,
					   WORD *pMaxAcls,
					   WORD *pMaxScos,
					   WORD *pMaxAclPktLen,
					   WORD *pMaxAclPkts,
					   BYTE *pMaxScoPktLen,
					   WORD *pMaxScoPkts,
					   WORD *pConnRxWnd,
					   BYTE *pRssiGoldenRangeLow,
					   BYTE *pRssiGoldenRangeHigh,
					   BYTE *pCompatibleMode,
						WORD *pLMP_PwrReqMsgInterval,
						BYTE *pLMP_DecrPwrReqMsgIntervalRatio,
						BYTE *pNumOfPwrStep4Clss1p5,
						BYTE *pPwrStepSize4Clss1p5,
						BYTE *pPwrCtrlInClss2Device,
						WORD *pDynamicRssiLmpTimeInterval,
						WORD *pDynamicRssiLmpTimeIntervalThrhld,
					   WORD pLocalSupportedCmds[NUM_WORD_SUPPORTCOMMAND],
					   WORD pLocalSupportedFeatures[NUM_WORD_SUPPORTFEATURE],
					   BYTE	pBdAddress[SIZE_BDADDRESS_INBYTE], 
					   BYTE	pLocalDeviceName[MAXSTRLEN_LOCALDEVICENAME],
					   WORD *pMask,					   
					   WORD pBtAddrList[MAXPARTNER][SIZE_BDADDRESS_INBYTE],
					   WORD pBtKeyList[MAXPARTNER][SIZE_LINKKEY_INBYTE],
					   BYTE pSecurityKey[MAX_BYTE_SECURITYKEY]
						);

int ReadAnnex45DataFromFile(char *FileName,  
						   bool *pFlag_GpioCtrlXosc,
						BYTE *pXoscSettling,
						DWORD *pPinOut_DrvLow,
						DWORD *pPinIn_PullDiable);

int WriteAnnex45DataToFile(char *FileName,  
						   bool *pFlag_GpioCtrlXosc,
						BYTE *pXoscSettling,
						DWORD *pPinOut_DrvLow,
						DWORD *pPinIn_PullDiable);


#if defined (_MIMO_)
int ReadAnnex47DataFromFile(char *FileName,
 						   int DeviceId, 
						   int PathId, //int BandId,
 						char pRssi_Nf[NUM_SUBBAND_G], 
						char pRssi_cck[NUM_SUBBAND_G], 
						char pRssi_ofdm[NUM_SUBBAND_G], 
						char pRssi_mcs[NUM_SUBBAND_G]);

int WriteAnnex47DataToFile(char *FileName,
 						   int DeviceId, 
						   int PathId, //int BandId,
 						char pRssi_Nf[NUM_SUBBAND_G], 
						char pRssi_cck[NUM_SUBBAND_G], 
						char pRssi_ofdm[NUM_SUBBAND_G], 
						char pRssi_mcs[NUM_SUBBAND_G]);

int ReadAnnex48DataFromFile(char *FileName,
						   int DeviceId, 
						   int PathId,  //int BandId,
						char pRssi_Nf[NUM_SUBBAND_A], 
						char pRssi_cck[NUM_SUBBAND_A], 
						char pRssi_ofdm[NUM_SUBBAND_A], 
						char pRssi_mcs[NUM_SUBBAND_A]);

int WriteAnnex48DataToFile(char *FileName,
 						   int DeviceId, 
						   int PathId, //int BandId,
 						char pRssi_Nf[NUM_SUBBAND_A], 
						char pRssi_cck[NUM_SUBBAND_A], 
						char pRssi_ofdm[NUM_SUBBAND_A], 
						char pRssi_mcs[NUM_SUBBAND_A]);
#endif //#if defined (_MIMO_)

//Rev D
int WriteAnnex42DataToFile_VD(char *FileName,
					BYTE *pNoOfEntry,
					THERMALCALPARMS_VD *ThermalCalParam);

int ReadAnnex42DataFromFile_VD(char *FileName,
					BYTE *pNoOfEntry,
					THERMALCALPARMS_VD *ThermalCalParam);

int WriteAnnex49DataToFile(char *FileName,
#if defined (_MIMO_)
					int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)
					BYTE *pNumOfTxPowerLevelStore,
					BYTE *pNumOfSavedCh,
					bool *pCalMethod,
					bool *pCloseTXPWDetection,
					bool *pExtPAPolarize,
					BYTE *pCalXtal,
					BYTE *pPdetHiLoGap,
					BYTE *pMCS20MPdetOffset,
					BYTE *pMCS20MInitPOffset,
					BYTE *pMCS40MPdetOffset,
					BYTE *pMCS40MInitPOffset,
					int  pChNumList[MAXSAVEDCH_G_REVD],
					char pCorrInitTrgt_B[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD], 
					char pCorrPDetTh_B[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD],

					char pCorrInitTrgt_G[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD], 
					char pCorrPDetTh_G[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD],
					BYTE* pwrTbl);

int ReadAnnex49DataFromFile(char *FileName,
#if defined (_MIMO_)
					int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)
					BYTE *pNumOfTxPowerLevelStore,
					BYTE *pNumOfSavedCh,
					bool *pCalMethod,
					bool *pCloseTXPWDetection,
					bool *pExtPAPolarize,
					BYTE *pCalXtal,
					BYTE *pPdetHiLoGap,
					BYTE *pMCS20MPdetOffset,
					BYTE *pMCS20MInitPOffset,
					BYTE *pMCS40MPdetOffset,
					BYTE *pMCS40MInitPOffset,
					int  pChNumList[MAXSAVEDCH_G_REVD],
					char pCorrInitTrgt_B[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD], 
					char pCorrPDetTh_B[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD],
					char pCorrInitTrgt_G[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD], 
					char pCorrPDetTh_G[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD]);
int WriteAnnex50DataToFile(char * FileName,
#if defined (_MIMO_)
					int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)
	int		SubBandId, 
	BYTE	*pNumOfSavedChInSubBand,
	BYTE	*pNumOfTxPowerLevelStore,
	BYTE	*pNumOfSavedCh,
	bool	*pCal_Method,
	bool 	*pClose_TX_PW_Detection,
	bool    *pExtPAPolarize,
	BYTE	*pCalXtal,	
	BYTE	*pPdetHiLoGap,
	BYTE	*pMCS20MPdetOffset,
	BYTE	*pMCS20MInitPOffset,
	BYTE	*pMCS40MPdetOffset,
	BYTE	*pMCS40MInitPOffset,
	BYTE	pSavedChannelIndex[MAXSAVEDCH_G_REVD],
	WORD	*pFreqBound_Lo,
	WORD	*pFreqBound_Hi,
	char	pCorrInitTrgt[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD], 
	char	pCorrPDetTh[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD]);
	
int ReadAnnex50DataFromFile(char *FileName,
#if defined (_MIMO_)
					int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)
	int		SubBandId, 
	BYTE	*pNumOfSavedChInSubBand,
	BYTE	*pNumOfTxPowerLevelStore,
	BYTE	*pNumOfSavedCh,
	bool	*pCal_Method,
	bool 	*pClose_TX_PW_Detection,
	bool    *ExtPAPolarize,
	BYTE	*pCalXtal,	
	BYTE	*pPdetHiLoGap,
	BYTE	*pMCS20MPdetOffset,
	BYTE	*pMCS20MInitPOffset,
	BYTE	*pMCS40MPdetOffset,
	BYTE	*pMCS40MInitPOffset,
	BYTE	pSavedChannelIndex[MAXSAVEDCH_G_REVD],
	WORD	*pFreqBound_Lo,
	WORD	*pFreqBound_Hi,
	char	pCorrInitTrgt[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD], 
	char	pCorrPDetTh[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD]);	

int ReadAnnex52DataFromFile(char *FileName,
#if defined (_MIMO_)
					int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)
					bool *Cal2G,
					bool *Cal5G,
					BYTE *LoLeakageCal2G_I,
					BYTE *LoLeakageCal2G_Q,
					char LoLeakageCal5G_I[NUM_SUBBAND_A_REVD],
					char LoLeakageCal5G_Q[NUM_SUBBAND_A_REVD]);

int WriteAnnex52DataToFile(char *FileName,
#if defined (_MIMO_)
					int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)
					bool Cal2G,
					bool Cal5G,
 					BYTE  LoLeakageCal2G_I, 
					BYTE  LoLeakageCal2G_Q,
					char LoLeakageCal5G_I[NUM_SUBBAND_A_REVD],
					char LoLeakageCal5G_Q[NUM_SUBBAND_A_REVD]);
int WriteAnnex53DataToFile(char *FileName,
#if defined (_MIMO_)
				   int DeviceId,		int PathId,  //int *CsC, 
#endif //#if defined (_MIMO_)
					bool Cal2G,
					bool Cal5G,
					char TxIQmismatch_Amp_2G,	char TxIQmismatch_Phase_2G,
					char TxIQmismatch_Amp_5G[NUM_SUBBAND_A_REVD],	char TxIQmismatch_Phase_5G[NUM_SUBBAND_A_REVD]);

int ReadAnnex53DataFromFile(char *FileName,
#if defined (_MIMO_)
				   int DeviceId,		int PathId,  //int *CsC, 
#endif //#if defined (_MIMO_)
					bool *Cal2G,
					bool *Cal5G,
					char *TxIQmismatch_Amp_2G,	char *TxIQmismatch_Phase_2G,
					char TxIQmismatch_Amp_5G[NUM_SUBBAND_A_REVD],	char TxIQmismatch_Phase_5G[NUM_SUBBAND_A_REVD]);
//Annex 54
int WriteAnnex54DataToFile(char *FileName,
#if defined (_MIMO_)
				   int DeviceId,		int PathId,  //int *CsC, 
#endif //#if defined (_MIMO_)
					bool Cal2G,
					bool Cal5G,
					char RxIQmismatch_Amp_2G,	char RxIQmismatch_Phase_2G,
					char RxIQmismatch_Amp_5G[NUM_SUBBAND_A_REVD],	char RxIQmismatch_Phase_5G[NUM_SUBBAND_A_REVD]);

int ReadAnnex54DataFromFile(char *FileName,
#if defined (_MIMO_)
				   int DeviceId,		int PathId,  //int *CsC, 
#endif //#if defined (_MIMO_)
					bool *Cal2G,
					bool *Cal5G,
					char *RxIQmismatch_Amp_2G,	char *RxIQmismatch_Phase_2G,
					char RxIQmismatch_Amp_5G[NUM_SUBBAND_A_REVD],	char RxIQmismatch_Phase_5G[NUM_SUBBAND_A_REVD]);
int WriteAnnex55DataToFile(char *FileName,
 					BYTE pVersion,
					BYTE pRFXtal,
					BYTE pInitPwr,
					BYTE pFELoss,

					bool pForceClass2Op,
					bool pDisablePwrControl,
					bool pMiscFlag,
					bool pUsedInternalSleepClock,
					BYTE pRssi_Golden_Lo,
					BYTE pRssi_Golden_Hi,
					DWORD pBTBAUDRate,
					BYTE pBDAddr[6],
					BYTE pEncr_Key_Len_Max,
					BYTE pEncr_Key_Len_Min);

int ReadAnnex55DataFromFile(char *FileName,
 					BYTE *pVersion,
					BYTE *pRFXtal,
					BYTE *pInitPwr,
					BYTE *pFELoss,

					bool *pForceClass2Op,
					bool *pDisablePwrControl,
					bool *pMiscFlag,
					bool *pUsedInternalSleepClock,
					BYTE *pRssi_Golden_Lo,
					BYTE *pRssi_Golden_Hi,
					DWORD *pBTBAUDRate,
					BYTE pBDAddr[6],
					BYTE *pEncr_Key_Len_Max,
					BYTE *pEncr_Key_Len_Min);


int WriteAnnex56DataToFile(char *FileName,
 					BYTE Lincense_Key[16],
	                WORD Link_Key_Mask,
	                BYTE Key_Value[3][22]);

int ReadAnnex56DataFromFile(char *FileName,
 					BYTE Lincense_Key[16],
	                WORD *Link_Key_Mask,
	                BYTE Key_Value[3][22]);



int ReadAnnex57DataFromFile(char *FileName,
#if defined (_MIMO_)
					int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)
					BYTE BandId,
					BYTE SubBandId,
					BYTE *MEM_ROW,
					BYTE *NumOfEnterirs,
					DPD_ConeffcicentData *Coneffcicent_Data);

int WriteAnnex57DataToFile(char *FileName,
#if defined (_MIMO_)
					int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)
					BYTE BandId,
					BYTE SubBandId,
					BYTE MEM_ROW,
					BYTE NumOfEnterirs,
					DPD_ConeffcicentData *Coneffcicent_Data);

////////////////////
// WLAN 
int LoadHwData( 
			    BOOL *CsC,				BYTE *pStructRev, 
				BYTE *PcbRev, 
				BOOL *pExtPaPolar_neg,	BOOL *pPa_External, 
				BYTE *pExtPA_PartId,	 	 
				BYTE *pAnt,				BYTE *pAntBT,	 	 
				BOOL *pCalTableOption,   
				BYTE *pCalRateB,		BYTE *pCalRateG,
				BYTE *pCalRng1,			BYTE *pCalRng2,
				BYTE *pCalRng3,			BYTE *pCalRng4,
				BYTE *pCC,				 
 				BYTE *pAssemblyHouse,	DWORD *pSN, 
				BYTE *pSocVer,			BYTE *pRfVer, 
				BYTE *pSpiSizeInfo,		BYTE *pSpiUnitInfo,		
				BYTE *pAntPartInfo, 
				WORD *pTestToolVer,		WORD *pDllVer) ;

int setCalDataRevC(int deviceType, void *pObj, int PurgeAfter);  

int   getCalMainDataRevD(
						BYTE *pStructRev=NULL,
						BYTE *pDesignType=NULL,
						BYTE *pLNAGain=NULL,
						BYTE *pAntDiversityInfo=NULL,
						BYTE *pFEVersion=NULL,
						
						bool *pPaOption_ExtLnaFlag_PathABC_G=NULL,
						bool *pPaOption_ExtLnaFlag_PathABC_A=NULL,
	
						bool *pPaOption_ExtPaPolar_PathABC_G=NULL,
						bool *pPaOption_ExtPaPolar_PathABC_A=NULL,

						BYTE *pSpiSizeInfo=NULL,
 						BYTE *pCC=NULL,
						
						BYTE *pExtXtalSource=NULL,		
						BYTE *pExtSleepClk=NULL,	
						BYTE *pWlanWakeUp=NULL,
						
						BYTE pAntTx[MAXNUM_MIMODEVICE]=NULL,		//
						BYTE pAntRx[MAXNUM_MIMODEVICE]=NULL,		//

						//Capabilities
						bool *pCapable_2G=NULL, //
						bool *pCapable_5G=NULL, //
						bool *pInternalBluetooth=NULL,	//
						bool *pAntennaDiversity=NULL,
						bool *pLNA_2G=NULL,	//
						bool *pLNA_5G=NULL,
						bool *pSameAnt_2G_BT=NULL,
						bool *pSameAnt_5G_BT=NULL,
						
						//Concurrency
						BYTE *Capabilities_2G=NULL,
						BYTE *Capabilities_5G=NULL,

						BYTE  BTFrontEnd2G[9]=NULL,
						BYTE  BTFrontEnd5G[9]=NULL,	
					
						DWORD *pTestToolVer=NULL,	DWORD *pMfgTaskVersion=NULL,	
						DWORD *pDllVersion=NULL, 
						BYTE *pSocOrRev=NULL, BYTE *pRfuOrRev=NULL,
						BYTE *pMimoTxCnt=NULL, 	BYTE *pMimoRxCnt=NULL, 
						BYTE pMimoMap[MAXNUM_MIMODEVICE][MAXNUM_RXPATH]=NULL);

int   setCalMainDataRevD( 
						BYTE *pStructRev=NULL,
						BYTE *pDesignType=NULL,
						BYTE *pSpiSizeInfo=NULL,
						BYTE *pLNAGain=NULL,
						BYTE *pAntDiversityInfo=NULL,
						BYTE *FEVersion=NULL,
					    BYTE *pCC=NULL,
						
						BYTE *pExtXtalSource=NULL,		
						BYTE *pExtSleepClk=NULL,	
						BYTE *pWlanWakeUp=NULL,
					
						BYTE pAntTx[MAXNUM_MIMODEVICE]=NULL,		//
						BYTE pAntRx[MAXNUM_MIMODEVICE]=NULL,		//

						//Capabilities
						bool *pCapable_2G=NULL, //
						bool *pCapable_5G=NULL, //
						bool *pInternalBluetooth=NULL,	//
						bool *pAntennaDiversity=NULL,
						bool *pLNA_2G=NULL,	//
						bool *pLNA_5G=NULL,
						bool *pSameAnt_2G_BT=NULL,
						bool *pSameAnt_5G_BT=NULL,
						
						//Concurrency
						BYTE *pCapabilities_2G=NULL,
						BYTE *pCapabilities_5G=NULL,

						BYTE  pBTFrontEnd2G[9]=NULL,
						BYTE  pBTFrontEnd5G[9]=NULL,	
					
						DWORD *pTestToolVer=NULL,	DWORD *pMfgTaskVersion=NULL,	
						DWORD *pDllVersion=NULL, 
						BYTE *pSocOrRev=NULL, BYTE *pRfuOrRev=NULL,
						BYTE *pMimoTxCnt=NULL, 	BYTE *pMimoRxCnt=NULL, 
						BYTE pMimoMap[MAXNUM_MIMODEVICE][MAXNUM_RXPATH]=NULL);

#if defined(_W8787__) || defined(_W8790_) || defined(_W8782_)
int getAnnexType24Data(WORD *BbuAddr, BYTE *BbuVal);
int setAnnexType24Data(WORD *BbuAddr, BYTE *BbuVal);

int getAnnexType25Data(DWORD *SocAddr, DWORD *SocVal);
int setAnnexType25Data(DWORD *SocAddr, DWORD *SocVal);

int getAnnexType27Data(BYTE *LED);
int setAnnexType27Data(BYTE *LED);

int getAnnexType28Data(BYTE *UsbString);
int setAnnexType28Data(BYTE *UsbString);

int getAnnexType31Data(	int DeviceId,
						char Rssi_Nf[NUM_SUBBAND_G], 
						char Rssi_cck[NUM_SUBBAND_G], 
						char Rssi_ofdm[NUM_SUBBAND_G], 
						char Rssi_mcs[NUM_SUBBAND_G]);
int setAnnexType31Data(	int DeviceId,
						char Rssi_Nf[NUM_SUBBAND_G], 
						char Rssi_cck[NUM_SUBBAND_G], 
						char Rssi_ofdm[NUM_SUBBAND_G], 
						char Rssi_mcs[NUM_SUBBAND_G]);

int getAnnexType32Data(	int DeviceId, 
						char Rssi_Nf[NUM_SUBBAND_A], 
						char Rssi_cck[NUM_SUBBAND_A], 
						char Rssi_ofdm[NUM_SUBBAND_A], 
						char Rssi_mcs[NUM_SUBBAND_A]);

int setAnnexType32Data(	int DeviceId, 
						char Rssi_Nf[NUM_SUBBAND_A], 
						char Rssi_cck[NUM_SUBBAND_A], 
						char Rssi_ofdm[NUM_SUBBAND_A], 
						char Rssi_mcs[NUM_SUBBAND_A]);
#endif	//#if defined(_W8787__) || defined(_W8790_)

//RevD
int GetAnnexType42Data_VD(BYTE *pNoOfEntry,THERMALCALPARMS_VD *ThermalCalParam);
int SetAnnexType42Data_VD(BYTE *pNoOfEntry,THERMALCALPARMS_VD *ThermalCalParam);

int SetAnnexType49Data(
#if defined (_MIMO_)
					int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)
					BYTE *NumOfTxPowerLevelStore,
					BYTE *NumOfSavedCh,
					bool *CalMethod,
					bool *CloseTXPWDetection,
					bool *ExtPAPolarize,
					BYTE *CalXtal,
                 
                    BYTE *PdetHiLoGap,
					BYTE *pMCS20MPdetOffset,
					BYTE *pMCS20MInitPOffset,
					BYTE *pMCS40MPdetOffset,
					BYTE *pMCS40MInitPOffset,

					int  pChNumList[MAXSAVEDCH_G_REVD],

					char pCorrInitTrgt_B[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD], 
					char pCorrPDetTh_B[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD],

					char pCorrInitTrgt_G[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD], 
					char pCorrPDetTh_G[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD],
					BYTE* pwrTbl);

int GetAnnexType49Data(
#if defined (_MIMO_)
					int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)
					BYTE *NumOfTxPowerLevelStore,
					BYTE *NumOfSavedCh,
					bool *CalMethod,
					bool *CloseTXPWDetection,
					bool *ExtPAPolarize,
					BYTE *CalXtal,
	                
					BYTE *PdetHiLoGap,
					BYTE *pMCS20MPdetOffset,
					BYTE *pMCS20MInitPOffset,
					BYTE *pMCS40MPdetOffset,
					BYTE *pMCS40MInitPOffset,

					int  pChNumList[MAXSAVEDCH_G_REVD],

					char pCorrInitTrgt_B[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD], 
					char pCorrPDetTh_B[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD],
					char pCorrInitTrgt_G[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD], 
					char pCorrPDetTh_G[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD], 
	
					BYTE* pwrTbl);

int GetAnnexType50Data(
#if defined (_MIMO_)
					int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)
	int		SubBandId, 
	BYTE	*pNumOfSavedChInSubBand,
	BYTE	*pNumOfTxPowerLevelStore,
	
	bool	*pCal_Method,
	bool 	*pClose_TX_PW_Detection,
	bool    *pExtPAPolarize,
	BYTE	*pCalXtal,

	BYTE	*pPdetHiLoGap,
	BYTE	*pMCS20MPdetOffset,
	BYTE	*pMCS20MInitPOffset,
	BYTE	*pMCS40MPdetOffset,
	BYTE	*pMCS40MInitPOffset,
	
	BYTE	pSavedChannelIndex[MAXSAVEDCH_G_REVD],	
	
	WORD	*pFreqBound_Lo,
	WORD	*pFreqBound_Hi,

	char	pCorrInitTrgt[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD], 
	char	pCorrPDetTh[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD], 
	
	BYTE	*pPwrTbl);

int SetAnnexType50Data(
#if defined (_MIMO_)
					int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)
	int		SubBandId, 
	BYTE	*pNumOfSavedChInSubBand,
	BYTE	*pNumOfTxPowerLevelStore,
	BYTE	*pNumOfSavedCh,
	bool	*pCalMethod,
	bool 	*pCloseTXPWDetection,
	bool    *pExtPAPolarize,
	BYTE	*pCalXtal,
	
	BYTE	*pPdetHiLoGap,
	BYTE	*pMCS20MPdetOffset,
	BYTE	*pMCS20MInitPOffset,
	BYTE	*pMCS40MPdetOffset,
	BYTE	*pMCS40MInitPOffset,

	BYTE	pSavedChannelIndex[MAXSAVEDCH_G_REVD],

	WORD	*pFreqBound_Lo,
	WORD	*pFreqBound_Hi,

	char	pCorrInitTrgt[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD], 
	char	pCorrPDetTh[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD],

	BYTE	*pPwrTbl);

int  GetAnnexType51Data(
#if defined (_MIMO_)
					int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)	
					BYTE	*pOFDM_TH_LO_Delta_40M,
					BYTE	*pOFDM_InitP_Delta_40M,
					BYTE	*pOFDM_TH_LO_Delta_20M,
					BYTE	*pOFDM_InitP_Delta_20M,
	
					BYTE	*pMCS_TH_LO_Delta_40M,
					BYTE	*pMCS_InitP_Delta_40M,
					BYTE	*pMCS_TH_LO_Delta_20M,
					BYTE	*pMCS_InitP_Delta_20M);


int SetAnnexType51Data(
#if defined (_MIMO_)
					int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)	
					BYTE	*pOFDM_TH_LO_Delta_40M,
					BYTE	*pOFDM_InitP_Delta_40M,
					BYTE	*pOFDM_TH_LO_Delta_20M,
					BYTE	*pOFDM_InitP_Delta_20M,
	
					BYTE	*pMCS_TH_LO_Delta_40M,
					BYTE	*pMCS_InitP_Delta_40M,
					BYTE	*pMCS_TH_LO_Delta_20M,
					BYTE	*pMCS_InitP_Delta_20M);

int GetAnnexType52Data(
#if defined (_MIMO_)
					   int DeviceId,		int PathId, 
#endif //#if defined (_MIMO_)
						bool *Cal2G,
						bool *Cal5G,
						BYTE *LoLeakageCal2G_I,
						BYTE *LoLeakageCal2G_Q,
						BYTE LoLeakageCal5G_I[NUM_SUBBAND_A_REVD],
						BYTE LoLeakageCal5G_Q[NUM_SUBBAND_A_REVD]);
						
int SetAnnexType52Data(
#if defined (_MIMO_)
					   int DeviceId,		int PathId,  
#endif //#if defined (_MIMO_)
						bool Cal2G,
						bool Cal5G,	
						BYTE LoLeakageCal2G_I,
						BYTE LoLeakageCal2G_Q,
						BYTE LoLeakageCal5G_I[NUM_SUBBAND_A_REVD],
						BYTE LoLeakageCal5G_Q[NUM_SUBBAND_A_REVD]);

int SetAnnexType53Data(
#if defined (_MIMO_)
					   int DeviceId,		int PathId,  
#endif //#if defined (_MIMO_)
						bool  Cal_2G,
						bool  Cal_5G,
						char TxIQmismatch_Amp_2G,	
						char TxIQmismatch_Phase_2G,
						char TxIQmismatch_Amp_5G[NUM_SUBBAND_A_REVD],	
						char TxIQmismatch_Phase_5G[NUM_SUBBAND_A_REVD]);

int GetAnnexType53Data(
#if defined (_MIMO_)
					   int DeviceId,		int PathId,  
#endif //#if defined (_MIMO_)
						bool  *Cal_2G,
						bool  *Cal_5G,
						char *TxIQmismatch_Amp_2G,	
						char *TxIQmismatch_Phase_2G,
						char TxIQmismatch_Amp_5G[NUM_SUBBAND_A_REVD],	
						char TxIQmismatch_Phase_5G[NUM_SUBBAND_A_REVD]);

int SetAnnexType54Data( 
#if defined (_MIMO_)
					   int DeviceId,		int PathId,   
#endif //#if defined (_MIMO_)
						bool  Cal_2G,
						bool  Cal_5G,
						char RxIQmismatch_Amp_2G,	
						char RxIQmismatch_Phase_2G,
						char RxIQmismatch_Amp_5G[NUM_SUBBAND_A_REVD],	
						char RxIQmismatch_Phase_5G[NUM_SUBBAND_A_REVD]);

int GetAnnexType54Data(
#if defined (_MIMO_)
					   int DeviceId,		int PathId,  
#endif //#if defined (_MIMO_)
						bool  *Cal_2G,
						bool  *Cal_5G,
						char *RxIQmismatch_Amp_2G,	
						char *RxIQmismatch_Phase_2G,
						char RxIQmismatch_Amp_5G[NUM_SUBBAND_A_REVD],	
						char RxIQmismatch_Phase_5G[NUM_SUBBAND_A_REVD]);
int SetAnnexType55Data(//int *CsC, 
					BYTE Version,
					BYTE RFXtal,
					BYTE InitPwr,
					BYTE FELoss,

					bool ForceClass2Op,
					bool DisablePwrControl,
					bool MiscFlag,
					bool UsedInternalSleepClock,

					BYTE Rssi_Golden_Lo,
					BYTE Rssi_Golden_Hi,
					DWORD BTBAUDRate,
					BYTE BDAddr[6],
					BYTE Encr_Key_Len_Max,
					BYTE Encr_Key_Len_Min
					);

int GetAnnexType55Data(//int *CsC, 
 					BYTE *Version,
					BYTE *RFXtal,
					BYTE *InitPwr,
					BYTE *FELoss,

					bool *ForceClass2Op,
					bool *DisablePwrControl,
					bool *MiscFlag,
					bool *UsedInternalSleepClock,

					BYTE *Rssi_Golden_Lo,
					BYTE *Rssi_Golden_Hi,
					DWORD *BTBAUDRate,
					BYTE BDAddr[6],
					BYTE *Encr_Key_Len_Max,
					BYTE *Encr_Key_Len_Min
					);

int GetAnnexType56Data(
 					BYTE Lincense_Key[16],
	                WORD *Link_Key_Mask,
	                BYTE Key_Value[3][22]);

int SetAnnexType56Data( 
 					BYTE Lincense_Key[16],
	                WORD Link_Key_Mask,
	                BYTE Key_Value[3][22]);

int GetAnnexType57Data(
#if defined (_MIMO_)
					   int DeviceId,		int PathId,  
#endif //#if defined (_MIMO_)
					BYTE Band,
					BYTE SubBand,
					BYTE *MEM_ROW,
					BYTE *NumOfEnterirs,
					DPD_ConeffcicentData *Coneffcicent_Data);


int SetAnnexType57Data(
#if defined (_MIMO_)
					   int DeviceId,		int PathId,  
#endif //#if defined (_MIMO_)
					BYTE Band,
					BYTE SubBand,
					BYTE MEM_ROW,
					BYTE NumOfEnterirs,
					DPD_ConeffcicentData *Coneffcicent_Data);



int getEndOfAnnex(int deviceType, void *pObj, DWORD* pointer);
int getNextOpenSpace(int deviceType, void *pObj, int PartMaxAddress,  BOOL includeCal, 
					 int sizeOfTblInByte, int* pointer);
int FindAnnexLocation(const int type2Find,  DWORD *pLocation, int *pTypeFound);

int getAnnexInfo(int deviceType, void *pObj, int pointer, int *sizeOfTblInByte, int* type,
							 int *DeviceId, int *PathId, 
							 int *BandId,	int *SubBandId) ;
int setAnnexInfo(void *pHeader, 
							 int sizeOfTblInByte, 
							 int type,
							 int DeviceId,	int PathId, 
							 int BandId,	int SubBandId);

int setAnnexInfoRevD(void *pHeader, 
							 int sizeOfTblInByte, 
							 int type,
							 int DeviceId,	int PathId);

int SetCustomizedSettings(char *FileName);

int GetCustomizedSettings(int* Setting);

#ifndef _LINUX_
}; // end of class declare
#endif	//#ifndef _LINUX_

#endif //SHARE
