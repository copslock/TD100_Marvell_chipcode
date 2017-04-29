/** @file DutSharedCalData.cpp
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#include "../../DutCommon/Dut_error.hc" 
#include "../../DutCommon/Clss_os.h"
#include "../../DutCommon/utilities_os.h"
#include "../../DutCommon/utilities.h"

#include "../DutWlanApi.hc"
/*
#ifndef _LINUX_
#include "stdafx.h"
#include <stdlib.h>
#include <time.h>
#include <direct.h>
#include "../DutWlanApi.hc"
#else	//_LINUX_
#include "DutWlanApi.hc"
#include <malloc.h>
#include <string.h>
#define TRUE 1
#define FALSE 0

typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef unsigned int BOOL;
typedef unsigned char BYTE;
extern int DebugLogRite(const char*,...);
extern int WritePrivateProfileString(char*, char*, char*, char*);
extern int GetPrivateProfileSection(char*, char*, int, char*);
extern int GetPrivateProfileSectionNames(char*, int, char*);
extern int GetPrivateProfileInt(char*, char*, int, char*);
extern int debugLog;
#endif	//#ifndef _LINUX_
*/

#ifdef _LINUX_
int g_NoEeprom=0;
char g_NoEepromWlanFlexFileName[_MAX_PATH]="";
char g_NoEepromBtFlexFileName[_MAX_PATH]="";

int DeBugLog=0;
int g_writeTemplate=0; 

void *pAnnex14[MAXNUM_MIMODEVICE][MAXNUM_TXPATH]={NULL}; 
void *pAnnex15[MAXNUM_MIMODEVICE][MAXNUM_TXPATH]={NULL};
void *pAnnex16_G[MAXNUM_MIMODEVICE][MAXNUM_TXPATH][NUM_SUBBAND_G]={NULL};
void *pAnnex16_A[MAXNUM_MIMODEVICE][MAXNUM_TXPATH][NUM_SUBBAND_A]={NULL};
void *pAnnex17[MAXNUM_MIMODEVICE][MAXNUM_RXPATH]={NULL};
void *pAnnex18=NULL;
void *pAnnex19[MAXNUM_MIMODEVICE]={NULL};
void *pAnnex20=NULL;
void *pAnnex21[MAXNUM_MIMODEVICE]={NULL};
void *pAnnex22=NULL;//[MAXNUM_MIMODEVICE];
void *pAnnex23=NULL;//[MAXNUM_MIMODEVICE];
void *pAnnex24=NULL;//[MAXNUM_MIMODEVICE];
void *pAnnex25=NULL;//[MAXNUM_MIMODEVICE];
void *pAnnex26[MAXNUM_MIMODEVICE][MAXNUM_TXPATH]={NULL};
void *pAnnex27=NULL;
void *pAnnex28=NULL;
void *pAnnex29=NULL;
void *pAnnex30[MAXNUM_MIMODEVICE][MAXNUM_TXPATH]={NULL};
void *pAnnex31[MAXNUM_MIMODEVICE]={NULL};//[MAXNUM_RXPATH];
void *pAnnex32[MAXNUM_MIMODEVICE]={NULL};//[MAXNUM_RXPATH];
void *pAnnex33[MAXNUM_MIMODEVICE]={NULL};//[MAXNUM_RXPATH];
void *pAnnex34[MAXNUM_MIMODEVICE]={NULL};//[MAXNUM_RXPATH];
void *pAnnex35[MAXNUM_MIMODEVICE]={NULL};//[MAXNUM_RXPATH];
void *pAnnex36[MAXNUM_MIMODEVICE]={NULL};//[MAXNUM_RXPATH];
void *pAnnex37[MAXNUM_MIMODEVICE][MAXNUM_TXPATH]={NULL};

void *pAnnex40=NULL;
void *pAnnex41=NULL;
void *pAnnex42=NULL;
void *pAnnex43=NULL;
void *pAnnex44=NULL;
void *pAnnex45=NULL;
void *pAnnex46=NULL;//[MAXNUM_MIMODEVICE];
void *pAnnex47[MAXNUM_MIMODEVICE][MAXNUM_RXPATH]={NULL};  
void *pAnnex48[MAXNUM_MIMODEVICE][MAXNUM_RXPATH]={NULL};
void *pAnnex49[MAXNUM_MIMODEVICE][MAXNUM_TXPATH]={NULL};
void *pAnnex50[MAXNUM_MIMODEVICE][MAXNUM_TXPATH][NUM_SUBBAND_A];
void *pAnnex51[MAXNUM_MIMODEVICE][MAXNUM_TXPATH]={NULL};
void *pAnnex52[MAXNUM_MIMODEVICE][MAXNUM_RXPATH]={NULL};  
void *pAnnex53[MAXNUM_MIMODEVICE][MAXNUM_TXPATH]={NULL};
void *pAnnex54[MAXNUM_MIMODEVICE][MAXNUM_TXPATH]={NULL};
void *pAnnex55=NULL; //[MAXNUM_MIMODEVICE][MAXNUM_TXPATH];
void *pAnnex56=NULL; //[MAXNUM_MIMODEVICE][MAXNUM_TXPATH];
void *pAnnex57[MAXNUM_MIMODEVICE][MAXNUM_TXPATH][NUM_SUBBAND_A_REVD+NUM_SUBBAND_G_REVD]={NULL};
void *pMain=NULL;

#endif // #ifdef _LINUX_

#include DUTCLASS_SHARED_H_PATH // "DutSharedClss.h"
// *** SPI signature and so on shall be shared 
#include DUTCLASS_WLAN_HD_PATH	//	"../DutWlanApi878XDll/DutWlanApiClss.hd"
 
/////////////////
/////////////////
#ifndef _NO_CLASS_
DutSharedClssHeader DutSharedClss()
{
 	char temp[_MAX_PATH]="";
   
 	if(getcwd(temp, _MAX_PATH) != NULL)  
 	strcat(temp, "/setup.ini");
 	else
	strcpy(temp, "setup.ini");

	DeBugLog	= GetPrivateProfileInt("DEBUG","debugLog", 
		0, temp); 

	g_NoEeprom =0;
	g_NoEeprom	= GetPrivateProfileInt("DutInitSet","NO_EEPROM",
										g_NoEeprom, temp); 
 
	strcpy(g_NoEepromBtFlexFileName, "BtCalData_ext.conf");
	GetPrivateProfileString("DutInitSet","NoEepromBtFlexFileName", 
		g_NoEepromBtFlexFileName, g_NoEepromBtFlexFileName, _MAX_PATH, temp); 

	strcpy(g_NoEepromWlanFlexFileName, "WlanCalData_ext.conf");
	GetPrivateProfileString("DutInitSet","NoEepromWlanFlexFileName", 
		g_NoEepromWlanFlexFileName, g_NoEepromWlanFlexFileName, _MAX_PATH, temp); 

	g_writeTemplate = 0; 

	PurgeAnnexPointers(0);
}

DutSharedClssHeader ~DutSharedClss()
{
}
#endif	//#ifndef _NO_CLASS_

/////////////////
/////////////////
BYTE DutSharedClssHeader DoCs1(BYTE *TblData, DWORD length)
{
	BYTE *ptr=NULL; 
	BYTE  sum = 0;
	DWORD index=0; 

	ptr = TblData;; 

	for (index =0; index < length; index ++)
	{	
		sum +=(*ptr); 
		ptr ++; 
	}
	if (DeBugLog)
	{
		DebugLogRite("Check Sum 0x%X\n", sum);
	}
	sum = 1-sum;
	
	return (sum);
 
}

void DutSharedClssHeader DoCs1Check(BYTE *TblData, DWORD length, int *CsC)
{
	// DoCsCheck
	BYTE sum = 0;
	BYTE  *ptr=NULL; 
 
	DWORD index=0; 

  
  	ptr = TblData; 
	for (index =0; index < length; index ++)
	{	
		sum +=(*ptr); 
		ptr ++; 
	}
	
	if(1 == (BYTE)sum) 
		*CsC = TRUE;
	else
		*CsC =FALSE;
 
}
 
 
void DutSharedClssHeader DoCs0(DWORD *TblData, DWORD length)
{
	BYTE *dataTmp=NULL, *ptr=NULL; 
	WORD sum=0;
	DWORD index=0; 

 	DWORD *ptrDword=NULL, Version=0;
  
// Do CS
	ptrDword = (DWORD*)TblData;

 	dataTmp = (BYTE*) malloc((length)); 
	memset(dataTmp, 0, length); 
	ptr = dataTmp; 

	for (index =0; index < length/sizeof(DWORD); index ++)
	{
		*ptr = (BYTE)(((*ptrDword) &0xFF000000) >>24); 
		ptr ++; 
		*ptr = (BYTE)(((*ptrDword) &0x00FF0000) >>16); 
		ptr ++; 
		*ptr = (BYTE)(((*ptrDword) &0x0000FF00) >>8);  
		ptr ++; 
		*ptr = (BYTE)(((*ptrDword) &0x000000FF) >>0);  
		ptr ++; 
		ptrDword++; 
	}

	ptr = dataTmp; 

	for (index =0; index < length-2; index ++)
	{	
		sum +=(*ptr); ptr ++; 
	}
	
	free(dataTmp); 

	ptrDword --;
	*ptrDword = (*ptrDword &0xFFFF0000)+sum;
}

void DutSharedClssHeader DoCs0Check(DWORD *TblData, DWORD length, int *CsC)
{
	// DoCsCheck
	WORD Csc=0, sum=0;
	BYTE *dataTmp=NULL, *ptr=NULL; 
	DWORD *tempD=NULL;
	DWORD index=0; 


	
	tempD = (DWORD*)malloc(length);
	memcpy(tempD, TblData, length); 

	Csc = (WORD)	tempD[((length)/sizeof(DWORD))-1];

	dataTmp = (BYTE*) malloc((length)); 
	memset(dataTmp, 0, length); 
	ptr = dataTmp; 

	for (index =0; index < (length)/sizeof(DWORD); index ++)
	{
		*ptr = (BYTE)((tempD[index   ]&0xFF000000) >>24); ptr ++; 
		*ptr = (BYTE)((tempD[index   ]&0x00FF0000) >>16); ptr ++; 
		*ptr = (BYTE)((tempD[index   ]&0x0000FF00) >>8);  ptr ++; 
		*ptr = (BYTE)((tempD[index   ]&0x000000FF) >>0);  ptr ++; 
	}

	ptr = dataTmp; 

	for (index =0; index < (length)-2; index ++)
	{	
		sum +=(*ptr); 
		ptr ++; 
	}
	
	free(dataTmp); 

	if(Csc == sum) 
		*CsC = TRUE;
	else
		*CsC =FALSE;

	free(tempD);
}
   
int DutSharedClssHeader GetAnnexType27Data(BYTE *pLED)
{
 	int status=0, i=0, j=0, ChCnt=0;
 	ANNEX_TYPE_27_LED_VC *ptr=NULL;
	// parse the data and pass it back to app. 
	if(NULL == pAnnex27) return ERROR_DATANOTEXIST; 

	ptr = (ANNEX_TYPE_27_LED_VC *)pAnnex27;

	if(pLED)
	{
			for (i=0; i <MAX_LED; i++)
			{
				pLED[i]	= ptr->LED[i]; 
			}
	}
 	return status;
}

int DutSharedClssHeader SetAnnexType27Data(BYTE *pLED)
{
  	int status=0, i=0;
 	ANNEX_TYPE_27_LED_VC *ptr=NULL;
 
	// alloc space to pointer, fill structure, other than next pointer and csc
	
	if(NULL == pAnnex27)
		pAnnex27 = malloc(sizeof(ANNEX_TYPE_27_LED_VC));

	ptr = (ANNEX_TYPE_27_LED_VC *)pAnnex27;
	memset(ptr, 0, sizeof(ANNEX_TYPE_27_LED_VC));
	setAnnexInfo(ptr, sizeof(ANNEX_TYPE_27_LED_VC),ANNEX_TYPE_NUM27,
					0, 0, 0,0); 

	if(pLED)
	{
		for (i=0; i <MAX_LED; i++)
		{
			ptr->LED[i] = pLED[i]; 
		}
	}
 	return status;
}
 

int DutSharedClssHeader GetAnnexType28Data(
								   BYTE *pUsbVendorString, 
								   BYTE *pUsbProductString)
{
 	int status=0, i=0, j=0, ChCnt=0;
 	ANNEX_TYPE_28_USBSTRINGS_VC *ptr=NULL;
	// parse the data and pass it back to app. 
	if(NULL == pAnnex28) return ERROR_DATANOTEXIST; 

	ptr = (ANNEX_TYPE_28_USBSTRINGS_VC *)pAnnex28;

	if(pUsbVendorString)
	{
		strcpy((char*)pUsbVendorString, (char*)ptr->VendorProductString);
	}

	if(pUsbProductString)
	{
		i=strlen((char*)ptr->VendorProductString);
		strcpy((char*)pUsbProductString, (char*)&ptr->VendorProductString[i+1]);

	}
 	return status;
}
int DutSharedClssHeader SetAnnexType28Data(
								   BYTE *pUsbVendorString, 
								   BYTE *pUsbProductString)
{
  	int status=0, i=0;
 	ANNEX_TYPE_28_USBSTRINGS_VC *ptr=NULL;
 
	// alloc space to pointer, fill structure, other than next pointer and csc
	
	if(NULL == pAnnex28)
		pAnnex28 = malloc(sizeof(ANNEX_TYPE_28_USBSTRINGS_VC));

	ptr = (ANNEX_TYPE_28_USBSTRINGS_VC *)pAnnex28;
	memset(ptr, 0, sizeof(ANNEX_TYPE_28_USBSTRINGS_VC));
	setAnnexInfo(ptr, sizeof(ANNEX_TYPE_28_USBSTRINGS_VC),ANNEX_TYPE_NUM28,
					0, 0, 0,0); 

	if(pUsbVendorString)
	{
		strcpy((char*)ptr->VendorProductString, (char*)pUsbVendorString);
	}
	i=strlen((char*)pUsbVendorString)+1;

	if(pUsbProductString)
	{
		strcpy((char*)&ptr->VendorProductString[i], (char*)pUsbVendorString);
	}

 	return status;
}

int DutSharedClssHeader GetAnnexType31Data(
#if defined (_MIMO_)
						   int DeviceId, 
#endif	// #if defined (_MIMO_)
						   //int PathId, int BandId,
						char pRssi_Nf[NUM_SUBBAND_G], 
						char pRssi_cck[NUM_SUBBAND_G], 
						char pRssi_ofdm[NUM_SUBBAND_G], 
						char pRssi_mcs[NUM_SUBBAND_G])
{
	int status=0, i=0, ChCnt=0;
 	ANNEX_TYPE_31_47_RSSI_G_VC *ptr=NULL;
#if !defined (_MIMO_)
		int DeviceId=0;
#endif	// #if defined (_MIMO_)

	// parse the data and pass it back to app. 

	if(NULL == pAnnex31[DeviceId]) return ERROR_DATANOTEXIST; 
	ptr = (ANNEX_TYPE_31_47_RSSI_G_VC *)pAnnex31[DeviceId];

	for (i=0; i<NUM_SUBBAND_G ; i++)
	{
 			if(pRssi_Nf)	pRssi_Nf[i]		= ptr->Rssi[i].NfOffset;	
			if(pRssi_cck)	pRssi_cck[i]	= ptr->Rssi[i].RssiOffset_CCK_11B;
			if(pRssi_ofdm)	pRssi_ofdm[i]	= ptr->Rssi[i].RssiOffset_OFDM_11G;	
			if(pRssi_mcs)	pRssi_mcs[i]	= ptr->Rssi[i].RssiOffset_MCS_11N;
	}
 	return status;
}						

int DutSharedClssHeader SetAnnexType31Data(	
#if defined (_MIMO_)
						   int DeviceId, 
#endif	// #if defined (_MIMO_)
						char pRssi_Nf[NUM_SUBBAND_G], 
						char pRssi_cck[NUM_SUBBAND_G], 
						char pRssi_ofdm[NUM_SUBBAND_G], 
						char pRssi_mcs[NUM_SUBBAND_G]) 
{
	int status=0, i=0;
 	ANNEX_TYPE_31_47_RSSI_G_VC *ptr=NULL;
#if !defined (_MIMO_)
		int DeviceId=0;
#endif	// #if defined (_MIMO_)

	// alloc space to pointer, fill structure, other than next pointer and csc
	if(NULL == pAnnex31[DeviceId])
		pAnnex31[DeviceId] = malloc(sizeof(ANNEX_TYPE_31_47_RSSI_G_VC));

	ptr = (ANNEX_TYPE_31_47_RSSI_G_VC *)pAnnex31[DeviceId];
	
	memset(ptr, 0, sizeof(ANNEX_TYPE_31_47_RSSI_G_VC));
	setAnnexInfo(ptr, sizeof(ANNEX_TYPE_31_47_RSSI_G_VC),ANNEX_TYPE_NUM31,
#if defined (_MIMO_)
					DeviceId, 
#else	// #if defined (_MIMO_)
					0,
#endif	// #if defined (_MIMO_)
					0, BAND_802_11G,0); 

	ptr->Flag=3;

	for (i=0; i<NUM_SUBBAND_G ; i++)
	{
		if(pRssi_Nf)
			ptr->Rssi[i].NfOffset				= pRssi_Nf[i];
		if(pRssi_cck)
			ptr->Rssi[i].RssiOffset_CCK_11B	= pRssi_cck[i];
		if(pRssi_ofdm)
			ptr->Rssi[i].RssiOffset_OFDM_11G	= pRssi_ofdm[i];
		if(pRssi_mcs)
			ptr->Rssi[i].RssiOffset_MCS_11N	= pRssi_mcs[i];
	}
 
  	return status;
}

int DutSharedClssHeader GetAnnexType32Data(
#if defined (_MIMO_)
						   int DeviceId, 
#endif	// #if defined (_MIMO_)
						char pRssi_Nf[NUM_SUBBAND_A], 
						char pRssi_cck[NUM_SUBBAND_A], 
						char pRssi_ofdm[NUM_SUBBAND_A], 
						char pRssi_mcs[NUM_SUBBAND_A]) 
{
	int status=0, i=0, ChCnt=0;
 	ANNEX_TYPE_32_48_RSSI_A_VC *ptr=NULL;
#if !defined (_MIMO_)
		int DeviceId=0;
#endif	// #if defined (_MIMO_)

	// parse the data and pass it back to app. 
	if(NULL == pAnnex32[DeviceId]) return ERROR_DATANOTEXIST; 

	ptr = (ANNEX_TYPE_32_48_RSSI_A_VC *)pAnnex32[DeviceId];

	for (i=0; i<NUM_SUBBAND_A ; i++)
	{
 		if(pRssi_Nf)	pRssi_Nf[i]		= ptr->Rssi[i].NfOffset;	
		if(pRssi_cck)	pRssi_cck[i]	= ptr->Rssi[i].RssiOffset_CCK_11B;
		if(pRssi_ofdm)	pRssi_ofdm[i]	= ptr->Rssi[i].RssiOffset_OFDM_11G;	
		if(pRssi_mcs)	pRssi_mcs[i]	= ptr->Rssi[i].RssiOffset_MCS_11N;
	}
 	return status;
}

int DutSharedClssHeader SetAnnexType32Data(	
#if defined (_MIMO_)
						   int DeviceId, 
#endif	// #if defined (_MIMO_)
						char pRssi_Nf[NUM_SUBBAND_A], 
						char pRssi_cck[NUM_SUBBAND_A], 
						char pRssi_ofdm[NUM_SUBBAND_A], 
						char pRssi_mcs[NUM_SUBBAND_A]) 
{
  	int status=0, i=0;
 	ANNEX_TYPE_32_48_RSSI_A_VC *ptr=NULL;
#if !defined (_MIMO_)
		int DeviceId=0;
#endif	// #if defined (_MIMO_)

	// alloc space to pointer, fill structure, other than next pointer and csc
	
	if(NULL == pAnnex32[DeviceId])
		pAnnex32[DeviceId] = malloc(sizeof(ANNEX_TYPE_32_48_RSSI_A_VC));

	ptr = (ANNEX_TYPE_32_48_RSSI_A_VC *)pAnnex32[DeviceId];
	memset(ptr, 0, sizeof(ANNEX_TYPE_32_48_RSSI_A_VC));
	setAnnexInfo(ptr, sizeof(ANNEX_TYPE_32_48_RSSI_A_VC),ANNEX_TYPE_NUM32,
					DeviceId, 0, BAND_802_11A,0); 

	ptr->Flag=3;

	for (i=0; i<NUM_SUBBAND_A ; i++)
	{
		if(pRssi_Nf) 
			ptr->Rssi[i].NfOffset				= pRssi_Nf[i];
		if(pRssi_cck)
			ptr->Rssi[i].RssiOffset_CCK_11B	= pRssi_cck[i];
		if(pRssi_ofdm)
			ptr->Rssi[i].RssiOffset_OFDM_11G	= pRssi_ofdm[i];
		if(pRssi_mcs)
			ptr->Rssi[i].RssiOffset_MCS_11N	= pRssi_mcs[i];
	}
 
  	return status;
}

/////////////////////
#if defined (_MIMO_)

int DutSharedClssHeader GetAnnexType47Data( 
						   int DeviceId, 
						   int PathId, 
						char pRssi_Nf[NUM_SUBBAND_G], 
						char pRssi_cck[NUM_SUBBAND_G], 
						char pRssi_ofdm[NUM_SUBBAND_G], 
						char pRssi_mcs[NUM_SUBBAND_G])
{
	int status=0, i=0, ChCnt=0;
 	ANNEX_TYPE_31_47_RSSI_G_VC *ptr=NULL;
 
	// parse the data and pass it back to app. 
	if(NULL == pAnnex47[DeviceId][PathId]) return ERROR_DATANOTEXIST; 
	ptr = (ANNEX_TYPE_31_47_RSSI_G_VC *)pAnnex47[DeviceId][PathId];
 

	for (i=0; i<NUM_SUBBAND_G ; i++)
	{
 		if(pRssi_Nf)	pRssi_Nf[i]		= ptr->Rssi[i].NfOffset;	
		if(pRssi_cck)	pRssi_cck[i]	= ptr->Rssi[i].RssiOffset_CCK_11B;
		if(pRssi_ofdm)	pRssi_ofdm[i]	= ptr->Rssi[i].RssiOffset_OFDM_11G;	
		if(pRssi_mcs)	pRssi_mcs[i]	= ptr->Rssi[i].RssiOffset_MCS_11N;
	}
 	return status;
}	
					
int DutSharedClssHeader SetAnnexType47Data(	
						   int DeviceId, 
						   int PathId,
						char pRssi_Nf[NUM_SUBBAND_G], 
						char pRssi_cck[NUM_SUBBAND_G], 
						char pRssi_ofdm[NUM_SUBBAND_G], 
						char pRssi_mcs[NUM_SUBBAND_G]) 
{
	int status=0, i=0;
 	ANNEX_TYPE_31_47_RSSI_G_VC *ptr=NULL;
 
	// alloc space to pointer, fill structure, other than next pointer and csc
//	if(BAND_802_11A == BandId) return ERROR_INPUT_INVALID; 
	
	if(NULL == pAnnex47[DeviceId][PathId])
		pAnnex47[DeviceId][PathId] = malloc(sizeof(ANNEX_TYPE_31_47_RSSI_G_VC));

	ptr = (ANNEX_TYPE_31_47_RSSI_G_VC *)pAnnex47[DeviceId][PathId];
	
	memset(ptr, 0, sizeof(ANNEX_TYPE_31_47_RSSI_G_VC));
	setAnnexInfo(ptr, sizeof(ANNEX_TYPE_31_47_RSSI_G_VC),ANNEX_TYPE_NUM47,
					DeviceId, 
					PathId, BAND_802_11G,0); 

	for (i=0; i<NUM_SUBBAND_G ; i++)
	{
		if(pRssi_Nf) 
			ptr->Rssi[i].NfOffset				= pRssi_Nf[i];
		if(pRssi_cck)
			ptr->Rssi[i].RssiOffset_CCK_11B	= pRssi_cck[i];
		if(pRssi_ofdm)
			ptr->Rssi[i].RssiOffset_OFDM_11G	= pRssi_ofdm[i];
		if(pRssi_mcs)
			ptr->Rssi[i].RssiOffset_MCS_11N	= pRssi_mcs[i];
	}
 
  	return status;
}

int DutSharedClssHeader GetAnnexType48Data(
						   int DeviceId, 
						   int PathId, 
						char pRssi_Nf[NUM_SUBBAND_A], 
						char pRssi_cck[NUM_SUBBAND_A], 
						char pRssi_ofdm[NUM_SUBBAND_A], 
						char pRssi_mcs[NUM_SUBBAND_A]) 
{
	int status=0, i=0, ChCnt=0;
 	ANNEX_TYPE_32_48_RSSI_A_VC *ptr=NULL;

	// parse the data and pass it back to app. 
	if(NULL == pAnnex48[DeviceId][PathId]) return ERROR_DATANOTEXIST; 

	ptr = (ANNEX_TYPE_32_48_RSSI_A_VC *)pAnnex48[DeviceId][PathId];

	for (i=0; i<NUM_SUBBAND_A ; i++)
	{
 		if(pRssi_Nf)	pRssi_Nf[i]		= ptr->Rssi[i].NfOffset;	
		if(pRssi_cck)	pRssi_cck[i]	= ptr->Rssi[i].RssiOffset_CCK_11B;
		if(pRssi_ofdm)	pRssi_ofdm[i]	= ptr->Rssi[i].RssiOffset_OFDM_11G;	
		if(pRssi_mcs)	pRssi_mcs[i]	= ptr->Rssi[i].RssiOffset_MCS_11N;
	}
 	return status;
}

int DutSharedClssHeader SetAnnexType48Data(	
						   int DeviceId, 
						   int PathId, 
						char pRssi_Nf[NUM_SUBBAND_A], 
						char pRssi_cck[NUM_SUBBAND_A], 
						char pRssi_ofdm[NUM_SUBBAND_A], 
						char pRssi_mcs[NUM_SUBBAND_A]) 
{
  	int status=0, i=0;
 	ANNEX_TYPE_32_48_RSSI_A_VC *ptr=NULL;


	// alloc space to pointer, fill structure, other than next pointer and csc
	if(NULL == pAnnex48[DeviceId][PathId])
		pAnnex48[DeviceId][PathId] = malloc(sizeof(ANNEX_TYPE_32_48_RSSI_A_VC));

	ptr = (ANNEX_TYPE_32_48_RSSI_A_VC *)pAnnex48[DeviceId];
	memset(ptr, 0, sizeof(ANNEX_TYPE_32_48_RSSI_A_VC));
	setAnnexInfo(ptr, sizeof(ANNEX_TYPE_32_48_RSSI_A_VC),ANNEX_TYPE_NUM48,
					DeviceId, PathId, BAND_802_11A,0); 

	for (i=0; i<NUM_SUBBAND_A ; i++)
	{
		if(pRssi_Nf)
			ptr->Rssi[i].NfOffset				= pRssi_Nf[i];
		if(pRssi_cck)
			ptr->Rssi[i].RssiOffset_CCK_11B	= pRssi_cck[i];
		if(pRssi_ofdm)
			ptr->Rssi[i].RssiOffset_OFDM_11G	= pRssi_ofdm[i];
		if(pRssi_mcs)
			ptr->Rssi[i].RssiOffset_MCS_11N	= pRssi_mcs[i];
	}
 
  	return status;
}
#endif	// #if defined (_MIMO_)
///////////////////

//////////////////////////////////////////////
//////////////////////////////////////////////
int DutSharedClssHeader PurgeAnnexPointers(int FlagFree)  
{
	int i=0, j=0, k=0;

	if(pMain)
	{
		if(FlagFree) free(pMain);
		pMain = NULL;
	}
 
	if(pAnnex24)
	{
		if(FlagFree) free(pAnnex24); 
		pAnnex24 = NULL; 
	}

	if(pAnnex25)
	{
		if(FlagFree) free(pAnnex25); 
		pAnnex25 = NULL; 
	}

	if(pAnnex27)
	{
		if(FlagFree) free(pAnnex27);
		pAnnex27 = NULL; 
	}

	if(pAnnex28)
	{
		if(FlagFree) free(pAnnex28);
		pAnnex28 = NULL; 
	}

	for (i=0; i<MAXNUM_MIMODEVICE ; i++)
	{
		if(pAnnex31[i])
		{
			if(FlagFree) free(pAnnex31[i]);
			pAnnex31[i] =NULL; 
		}

		if(pAnnex32[i])
		{
			if(FlagFree) free(pAnnex32[i]);
			pAnnex32[i] =NULL; 
		}

	}

	if(pAnnex40)
	{
		if(FlagFree) free(pAnnex40);
		pAnnex40 =NULL; 
	}		

	if(pAnnex42)
	{
		if(FlagFree) free(pAnnex42);
		pAnnex42 =NULL; 
	}

	if(pAnnex45)
	{
		if(FlagFree) free(pAnnex45);
		pAnnex45 =NULL; 
	}

	if(pAnnex46)
	{
		if(FlagFree) free(pAnnex46);
		pAnnex46 =NULL; 
	}

	for (i=0; i<MAXNUM_MIMODEVICE ; i++)
	{
		for (j=0; j<MAXNUM_RXPATH ; j++)
		{
			if(pAnnex47[i][j])
			{
				if(FlagFree) free(pAnnex47[i][j]);
				pAnnex47[i][j] =NULL; 
			}

			if(pAnnex48[i][j])
			{
				if(FlagFree) free(pAnnex48[i][j]);
				pAnnex48[i][j] =NULL; 
			}

			if(pAnnex49[i][j])
			{
				if(FlagFree) free(pAnnex49[i][j]);
				pAnnex49[i][j] =NULL; 
			}

			for (k=0; k<NUM_SUBBAND_A ; k++)
			{
				if(pAnnex50[i][j][k])
				{
					if(FlagFree) free(pAnnex50[i][j][k]);
					pAnnex50[i][j][k] =NULL; 
				}
			}

			if(pAnnex52[i][j])
			{
				if(FlagFree) free(pAnnex52[i][j]);
				pAnnex52[i][j] =NULL; 
			}
			
			if(pAnnex53[i][j])
			{
				if(FlagFree) free(pAnnex53[i][j]);
				pAnnex53[i][j] =NULL; 
			}

			if(pAnnex54[i][j])
			{
				if(FlagFree) free(pAnnex54[i][j]);
				pAnnex54[i][j] =NULL; 
			}

			for(k=0;k<NUM_SUBBAND_A_REVD+NUM_SUBBAND_G_REVD;k++)
			{
				if(pAnnex57[i][j])
				{
					if(FlagFree) free(pAnnex57[i][j][k]);
					pAnnex57[i][j][k] =NULL; 
				}
			}	//for (k=0;
		}	// for (j=0; 
	}	// for (i=0; 

	if(pAnnex55)
	{
		if(FlagFree) free(pAnnex55);
		pAnnex55 =NULL; 
	}

	if(pAnnex56)
	{
		if(FlagFree) free(pAnnex56);
		pAnnex56 =NULL; 
	}

	return 0; 
} 
 
int DutSharedClssHeader GetAnnexType40Data( 
					   WORD *pVid,	WORD *pPid,	BYTE *pChipId,	BYTE *pChipVersion,					   
					   BYTE *pFlag_SettingPrev, BYTE *pNumOfRailAvaiableInChip,
						BYTE *pPowerDownMode,

						BYTE pNormalMode[MAX_PM_RAIL_PW886], 
						BYTE pSleepModeFlag[MAX_PM_RAIL_PW886],
						BYTE pSleepModeSetting[MAX_PM_RAIL_PW886],
						BYTE *pDpSlp
						)
{
	int status=0, i=0;
 	ANNEX_TYPE_40_PM_VC *ptr=NULL;
	// parse the data and pass it back to app. 
	if(NULL == pAnnex40) return ERROR_DATANOTEXIST; 

	ptr = (ANNEX_TYPE_40_PM_VC *)pAnnex40;

	if(pVid)			
		(*pVid) = ptr->VID;
	if(pPid)			
		(*pPid) = ptr->PID;
	if(pChipId)			
		(*pChipId) = ptr->ChipId;
	if(pChipVersion)	
		(*pChipVersion)  = ptr->ChipVersion;
	if(pFlag_SettingPrev) 
		(*pFlag_SettingPrev)  = ptr->GlobleInfo.PrevRevSettingFlag;
	if(pNumOfRailAvaiableInChip) 
		(*pNumOfRailAvaiableInChip) = ptr->GlobleInfo.NumOfRailAvalable;
	if(pDpSlp) 
		(*pDpSlp) = ptr->DPSLP;
	 
	if(pPowerDownMode)
	{
		(*pPowerDownMode) = ptr->PowerDownMode;
	}
	
	for(i=0; i<MAX_PM_RAIL_PW886; i++)
	{
		if(pNormalMode) pNormalMode[i] = ptr->NormalModeSetting[i]; 
 
		if(pSleepModeSetting) pSleepModeSetting[i]= ptr->SleepMode[i].Setting;
		if(pSleepModeFlag) pSleepModeFlag[i]= ptr->SleepMode[i].Flag;
	}
 	return status;
}
 

int DutSharedClssHeader SetAnnexType40Data(WORD *pVid,	WORD *pPid,	
										   BYTE *pChipId,	BYTE *pChipVersion,					   
						BYTE *pFlag_SettingPrev, BYTE *pNumOfRailAvaiableInChip,
						BYTE *pPowerDownMode,

						BYTE pNormalMode[MAX_PM_RAIL_PW886], 
						BYTE pSleepModeFlag[MAX_PM_RAIL_PW886],
						BYTE pSleepModeSetting[MAX_PM_RAIL_PW886],
						BYTE *pDpSlp
)
{ 
  	int status=0, i=0;
 	ANNEX_TYPE_40_PM_VC *ptr=NULL;
	int SocId=0; 

	// alloc space to pointer, fill structure, other than next pointer and csc	
	if(NULL == pAnnex40)
		pAnnex40 = malloc(sizeof(ANNEX_TYPE_40_PM_VC));

	ptr = (ANNEX_TYPE_40_PM_VC *)pAnnex40;
	memset(ptr, 0, sizeof(ANNEX_TYPE_40_PM_VC));
	setAnnexInfo(ptr, sizeof(ANNEX_TYPE_40_PM_VC),ANNEX_TYPE_NUM40,
					0, 0, 0,0); 

	if(pVid)
		ptr->VID = *pVid;
	if(pPid)
		ptr->PID = *pPid;
	if(pChipId)
		ptr->ChipId = *pChipId;
	if(pChipVersion)
		ptr->ChipVersion = *pChipVersion;
	if(pFlag_SettingPrev)
		ptr->GlobleInfo.PrevRevSettingFlag = *pFlag_SettingPrev;
	if(pNumOfRailAvaiableInChip)
		ptr->GlobleInfo.NumOfRailAvalable = *pNumOfRailAvaiableInChip;
	if(pDpSlp)
		ptr->DPSLP = *pDpSlp;

	ptr->PowerDownMode=(*pPowerDownMode);

	
	for(i=0; i<MAX_PM_RAIL_PW886; i++)
	{
		ptr->NormalModeSetting[i]= pNormalMode[i]; 
 
		ptr->SleepMode[i].Setting= pSleepModeSetting[i];
		ptr->SleepMode[i].Flag= pSleepModeFlag[i];
	}
	
	return status;
}	
int DutSharedClssHeader GetAnnexType45Data(
						bool *pFlag_GpioCtrlXosc,
					   BYTE *pXoscSettling,
					   DWORD *pPinOut_DrvLow,
					   DWORD *pPinIn_PullDiable
						)
{
	int status=0, i=0;
 	ANNEX_TYPE_45_SLEEPMODE_VC *ptr=NULL;
	// parse the data and pass it back to app. 
	if(NULL == pAnnex45) return ERROR_DATANOTEXIST; 

	ptr = (ANNEX_TYPE_45_SLEEPMODE_VC *)pAnnex45;

 	
 	if(pFlag_GpioCtrlXosc)		
		(*pFlag_GpioCtrlXosc) = ptr->SleepFlag.XoscDrvnByGpio;
	if(pXoscSettling)		(*pXoscSettling) = ptr->XoscSettling;
	if(pPinOut_DrvLow)
	{
		pPinOut_DrvLow[0] = ptr->PinOut_DrvLow_Low32;
		pPinOut_DrvLow[1] = ptr->PinOut_DrvLow_High32;
	}
	if(pPinIn_PullDiable)
	{
		pPinIn_PullDiable[0] = ptr->PinIn_PullDiable_Low32;
		pPinIn_PullDiable[1] = ptr->PinIn_PullDiable_High32;
	}

 	return status;
}
 
int DutSharedClssHeader SetAnnexType45Data(
						bool Flag_GpioCtrlXosc,
					   BYTE XoscSettling,
					   DWORD *pPinOut_DrvLow,
					   DWORD *pPinIn_PullDiable
						)
{ 
  	int status=0, i=0;
 	ANNEX_TYPE_45_SLEEPMODE_VC *ptr=NULL;
 
	// alloc space to pointer, fill structure, other than next pointer and csc
	
	if(NULL == pAnnex45) 
		pAnnex45 = malloc(sizeof(ANNEX_TYPE_45_SLEEPMODE_VC));

	ptr = (ANNEX_TYPE_45_SLEEPMODE_VC *)pAnnex45;
	memset(ptr, 0, sizeof(ANNEX_TYPE_45_SLEEPMODE_VC));
	setAnnexInfo(ptr, sizeof(ANNEX_TYPE_45_SLEEPMODE_VC),ANNEX_TYPE_NUM45,
					0, 0, 0,0); 

	ptr->SleepFlag.XoscDrvnByGpio = Flag_GpioCtrlXosc;
	ptr->XoscSettling = XoscSettling;
	ptr->PinOut_DrvLow_Low32 = pPinOut_DrvLow[0];
	ptr->PinOut_DrvLow_High32 = pPinOut_DrvLow[1];
	ptr->PinIn_PullDiable_Low32 = pPinIn_PullDiable[0];
	ptr->PinIn_PullDiable_High32 = pPinIn_PullDiable[1];
 
	return status;
}					   

int DutSharedClssHeader GetAnnexType50Data(
#if defined (_MIMO_)
					int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)
	int		SubBandId, 
	BYTE	*pNumOfSavedChInSubBand,
	BYTE	*pNumOfTxPowerLevelStore,
	
	bool	*pCal_Method,
	bool 	*pClose_TX_PW_Detection,
	bool	*pExtPAPolarize,
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
	
	BYTE* pPwrTbl)
{
	int status=0, i=0, j=0, ChCnt=0;
 	ANNEX_TYPE_50_5G_POWERTABLE_VD *ptr=NULL;
#if !defined (_MIMO_)
	int DeviceId=0, PathId=0;	
#endif // #if !defined (_MIMO_)

	// parse the data and pass it back to app. 
	if(NULL == pAnnex50[DeviceId][PathId][SubBandId]) return ERROR_DATANOTEXIST; 

	ptr = (ANNEX_TYPE_50_5G_POWERTABLE_VD *)pAnnex50[DeviceId][PathId][SubBandId];

	if(pCalXtal)
	 (*pCalXtal) = ptr->CalXtal;



	if(pNumOfTxPowerLevelStore)
		(*pNumOfTxPowerLevelStore) = ptr->NumOfTxPowerLevelStore;

	if(pCal_Method)
		(*pCal_Method) = ptr->CalOption.Cal_Method;

    if(pClose_TX_PW_Detection)
		(*pClose_TX_PW_Detection) = ptr->CalOption.Close_TX_PW_Detection;

	if(pExtPAPolarize)
		(*pExtPAPolarize) = ptr->CalOption.ExtPAPolarize;

	if(pPdetHiLoGap)
		(*pPdetHiLoGap) = ptr->PdetHiLoGap;

    if(pMCS20MPdetOffset)
		(*pMCS20MPdetOffset) = ptr->MCS20MPdetOffset;
	if(pMCS20MInitPOffset)
		(*pMCS20MInitPOffset) = ptr->MCS20MInitPOffset;
	if(pMCS40MPdetOffset)
		(*pMCS40MPdetOffset) = ptr->MCS40MPdetOffset;
	if(pMCS40MInitPOffset)
		(*pMCS40MInitPOffset) = ptr->MCS40MInitPOffset;
 	ChCnt=0;
	if(pNumOfSavedChInSubBand)
	{
		(*pNumOfSavedChInSubBand) = ptr->NumOfSavedChInSubBand;

		if(pSavedChannelIndex)
			for (i=0; i<(*pNumOfSavedChInSubBand); i++)
			{	
				pSavedChannelIndex[ChCnt]= ptr->SavedChannelIndex[ChCnt];
				ChCnt++; 
			}
	}


	for (i=0; i<(ChCnt); i++)
	for (j=0; j<RFPWRRANGE_NUM_REVD; j++)
	{
		if(pCorrPDetTh)
			pCorrPDetTh[i][j] = ptr->PaCal[j][i].CorrPdetTh; 
		if(pCorrInitTrgt)
			pCorrInitTrgt[i][j] = ptr->PaCal[j][i].CorrInitPwrTrgt; 
	}
    

	if(pFreqBound_Hi) 
	{
		(*pFreqBound_Hi) = ptr->FreqBound.HighFreqBound;
	}

	
	if(pFreqBound_Lo) 
	{
			(*pFreqBound_Lo) = ptr->FreqBound.LowFreqBound;
	}

 
 	if(pPwrTbl) memcpy( pPwrTbl, ptr->pwrTblLevel,
							(sizeof(ptr->pwrTblLevel)));

	return status;
}

int DutSharedClssHeader SetAnnexType50Data(
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

	BYTE* pPwrTbl)
{
	int status=0, i=0, j=0, ChCnt=0;
 	ANNEX_TYPE_50_5G_POWERTABLE_VD *ptr=NULL;
#if !defined (_MIMO_)
	int DeviceId=0, PathId=0; 
#endif //#if !defined (_MIMO_)

	// alloc space to pointer, fill structure, other than next pointer and csc

	if(NULL == pAnnex50[DeviceId][PathId][SubBandId])
		pAnnex50[DeviceId][PathId][SubBandId]= malloc(sizeof(ANNEX_TYPE_50_5G_POWERTABLE_VD));

	ptr = (ANNEX_TYPE_50_5G_POWERTABLE_VD *)pAnnex50[DeviceId][PathId][SubBandId];
	memset(ptr, 0, sizeof(ANNEX_TYPE_50_5G_POWERTABLE_VD));
	setAnnexInfoRevD(ptr, sizeof(ANNEX_TYPE_50_5G_POWERTABLE_VD),ANNEX_TYPE_NUM50,
					DeviceId, PathId); 

	ptr->SubBandId= SubBandId;

	ptr->NumOfTxPowerLevelStore = (*pNumOfTxPowerLevelStore);


	ptr->CalOption.Cal_Method = (*pCalMethod);
	ptr->CalOption.Close_TX_PW_Detection = (*pCloseTXPWDetection);
	ptr->CalOption.ExtPAPolarize = (*pExtPAPolarize);
	ptr->CalXtal = (*pCalXtal);
	ptr->PdetHiLoGap = (*pPdetHiLoGap);
	ptr->MCS20MPdetOffset = (*pMCS20MPdetOffset);
	ptr->MCS20MInitPOffset = (*pMCS20MInitPOffset);
	ptr->MCS40MPdetOffset = (*pMCS40MPdetOffset);
	ptr->MCS40MInitPOffset = (*pMCS40MInitPOffset);

	
 	ChCnt=0;
	if(pNumOfSavedChInSubBand)
	{
			ptr->NumOfSavedChInSubBand = (*pNumOfSavedChInSubBand);

			for (i=0; i<(*pNumOfSavedChInSubBand); i++)
			{	
				ptr->SavedChannelIndex[ChCnt]	= pSavedChannelIndex[ChCnt];
				ChCnt++; 
			}
	}

	for (i=0; i<(ChCnt); i++)
	for (j=0; j<RFPWRRANGE_NUM_REVD; j++)
	{
		if(pCorrPDetTh)
			ptr->PaCal[j][i].CorrPdetTh = pCorrPDetTh[i][j]; 
		if(pCorrInitTrgt)
			ptr->PaCal[j][i].CorrInitPwrTrgt = pCorrInitTrgt[i][j]; 
	}
    

	if(pFreqBound_Hi) 
	{
		ptr->FreqBound.HighFreqBound = (*pFreqBound_Hi);
	}

	
	if(pFreqBound_Lo) 
	{
		ptr->FreqBound.LowFreqBound = (*pFreqBound_Lo);
	}

 
 	if(pPwrTbl) memcpy(ptr->pwrTblLevel, pPwrTbl,  
							(sizeof(ptr->pwrTblLevel)));

  	return status;
}

int DutSharedClssHeader GetAnnexType51Data(
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
					BYTE	*pMCS_InitP_Delta_20M)
{
	int status=0;
 	ANNEX_TYPE_51_SINGAL_CALDATA_ADDITION_VD *ptr=NULL;
#if !defined (_MIMO_)
	int DeviceId=0, PathId=0;	
#endif // #if !defined (_MIMO_)

	if(NULL == pAnnex51[DeviceId][PathId]) return ERROR_DATANOTEXIST; 

	ptr = (ANNEX_TYPE_51_SINGAL_CALDATA_ADDITION_VD *)pAnnex51[DeviceId][PathId];

	if(*pOFDM_TH_LO_Delta_40M)
		(*pOFDM_TH_LO_Delta_40M) = ptr->OFDM_TH_LO_Delta_40M;
	if(*pOFDM_InitP_Delta_40M)
		(*pOFDM_InitP_Delta_40M) = ptr->OFDM_InitP_Delta_40M;


	if(*pOFDM_TH_LO_Delta_20M)
		(*pOFDM_TH_LO_Delta_20M) = ptr->OFDM_TH_LO_Delta_20M;
	if(*pOFDM_InitP_Delta_20M)
		(*pOFDM_InitP_Delta_20M) = ptr->OFDM_InitP_Delta_20M;


	if(*pMCS_TH_LO_Delta_40M)
		(*pMCS_TH_LO_Delta_40M) = ptr->MCS_TH_LO_Delta_40M;
	if(*pMCS_InitP_Delta_40M)
		(*pMCS_InitP_Delta_40M) = ptr->MCS_InitP_Delta_40M;


	if(*pMCS_TH_LO_Delta_20M)
		(*pOFDM_TH_LO_Delta_20M) = ptr->MCS_TH_LO_Delta_20M;
	if(*pMCS_InitP_Delta_20M)
		(*pOFDM_InitP_Delta_20M) = ptr->MCS_InitP_Delta_20M;
	return status;
}

int DutSharedClssHeader SetAnnexType51Data(
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
					BYTE	*pMCS_InitP_Delta_20M)
{

	int status=0;
 	ANNEX_TYPE_51_SINGAL_CALDATA_ADDITION_VD *ptr=NULL;
 #if !defined (_MIMO_)
	int DeviceId=0, PathId=0; 
#endif //#if defined (_MIMO_)

	// alloc space to pointer, fill structure, other than next pointer and csc
	
	if(NULL == pAnnex51[DeviceId][PathId])
		pAnnex51[DeviceId][PathId] = malloc(sizeof(ANNEX_TYPE_51_SINGAL_CALDATA_ADDITION_VD));
	ptr = (ANNEX_TYPE_51_SINGAL_CALDATA_ADDITION_VD *)pAnnex51[DeviceId][PathId];
	memset(ptr, 0, sizeof(ANNEX_TYPE_51_SINGAL_CALDATA_ADDITION_VD));
	setAnnexInfo(ptr, sizeof(ANNEX_TYPE_51_SINGAL_CALDATA_ADDITION_VD),ANNEX_TYPE_NUM51,
					DeviceId, PathId, 0,0); 

	if(*pOFDM_TH_LO_Delta_40M)
		ptr->OFDM_TH_LO_Delta_40M = (*pOFDM_TH_LO_Delta_40M);
	if(*pOFDM_InitP_Delta_40M)
		ptr->OFDM_InitP_Delta_40M = (*pOFDM_InitP_Delta_40M);


	if(*pOFDM_TH_LO_Delta_20M)
		ptr->OFDM_TH_LO_Delta_20M = (*pOFDM_TH_LO_Delta_20M);
	if(*pOFDM_InitP_Delta_20M)
		ptr->OFDM_InitP_Delta_20M = (*pOFDM_InitP_Delta_20M);


	if(*pMCS_TH_LO_Delta_40M)
		ptr->MCS_TH_LO_Delta_40M = (*pMCS_TH_LO_Delta_40M);
	if(*pMCS_InitP_Delta_40M)
		ptr->MCS_InitP_Delta_40M = (*pMCS_InitP_Delta_40M);


	if(*pMCS_TH_LO_Delta_20M)
		ptr->MCS_TH_LO_Delta_20M = (*pOFDM_TH_LO_Delta_20M);
	if(*pMCS_InitP_Delta_20M)
		ptr->MCS_InitP_Delta_20M = (*pOFDM_InitP_Delta_20M);
	return status;
}


int DutSharedClssHeader GetAnnexType42Data_VD( BYTE *pNoOfEntry, THERMALCALPARMS_VD *ThermalCalParam)
{
		
	int status=0, i=0;
 	ANNEX_TYPE_42_THERMALCAL_VD *ptr=NULL;
	THERMALCALPARMS_IN_DLL_VD *PtrThermalCalParam= (THERMALCALPARMS_IN_DLL_VD *) ThermalCalParam;
	THERMALCALPARMS_IN_DLL_VD *AnnexPtr=NULL;
	int ThermalCalParamSize = sizeof(THERMALCALPARMS_IN_DLL_VD);

	if(NULL == pAnnex42) return ERROR_DATANOTEXIST; 

	ptr = (ANNEX_TYPE_42_THERMALCAL_VD *)pAnnex42;

    (*pNoOfEntry) = ((ptr->typeHdr.LengthInByte) - sizeof(ANNEX_TYPE_HEADER_VC)) / ThermalCalParamSize;
    
	AnnexPtr = (THERMALCALPARMS_IN_DLL_VD*)ptr->ThermalCalParam;
	for (i=0; i<(*pNoOfEntry); i++)
	{
		PtrThermalCalParam->BandID = AnnexPtr->BandID;
		PtrThermalCalParam->SubBand = AnnexPtr->SubBand;
		PtrThermalCalParam->TempRefMinus40 = AnnexPtr->TempRefMinus40;
		PtrThermalCalParam->TempSlope = AnnexPtr->TempSlope;
		PtrThermalCalParam->TempCalTime = AnnexPtr->TempCalTime;
		PtrThermalCalParam->TempRef85 = AnnexPtr->TempRef85;
		PtrThermalCalParam->Numerator = AnnexPtr->Numerator;
		PtrThermalCalParam->Denom = AnnexPtr->Denom;
		PtrThermalCalParam->PPAConefficient = AnnexPtr->PPAConefficient;
		PtrThermalCalParam->PAConefficient = AnnexPtr->PAConefficient;
		PtrThermalCalParam->XTAL = AnnexPtr->XTAL;
		PtrThermalCalParam->PTargetDelta = AnnexPtr->PTargetDelta;
		PtrThermalCalParam->Numerator1 = AnnexPtr->Numerator1;
		PtrThermalCalParam++;
		AnnexPtr++;
	}
	return status;
}

int DutSharedClssHeader SetAnnexType42Data_VD( BYTE *pNoOfEntry, THERMALCALPARMS_VD *ThermalCalParam)
{
	int status=0;
 	ANNEX_TYPE_42_THERMALCAL_VD *ptr=NULL;
	BYTE *PtrThermalCalParam=NULL;
	int ThermalCalParamSize = sizeof(THERMALCALPARMS_IN_DLL_VD)*(*pNoOfEntry);
 #if !defined (_MIMO_)
	int DeviceId=0, PathId=0; 
#endif //#if defined (_MIMO_)

	// alloc space to pointer, fill structure, other than next pointer and csc
	
	if(NULL == pAnnex42)
		pAnnex42 = malloc(sizeof(ANNEX_TYPE_HEADER_VC)+ 
		ThermalCalParamSize
		);
	ptr = (ANNEX_TYPE_42_THERMALCAL_VD *)pAnnex42;
	memset(ptr, 0, sizeof(ANNEX_TYPE_HEADER_VC)+ 
		ThermalCalParamSize);
	//header
	ptr->typeHdr.AnnexType= ANNEX_TYPE_NUM42;
	ptr->typeHdr.LengthInByte = sizeof(ANNEX_TYPE_HEADER_VC)+ ThermalCalParamSize;
	PtrThermalCalParam = (BYTE *)(ptr->ThermalCalParam);
	memcpy(PtrThermalCalParam, ThermalCalParam, ThermalCalParamSize);
	return 0;
}

int DutSharedClssHeader SetAnnexType49Data(
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

					BYTE* pPwrTbl)
{
	 int status=0 , i=0, j=0;;

	 ANNEX_TYPE_49_2G_POWERTABLE_VD *ptr=NULL;
#if !defined (_MIMO_)
	int DeviceId=0, PathId=0; 
#endif //#if defined (_MIMO_)
	// parse the data and pass it back to app. 		
	if(NULL == pAnnex49[DeviceId][PathId]) 
		pAnnex49[DeviceId][PathId] = malloc(sizeof(ANNEX_TYPE_49_2G_POWERTABLE_VD));

	ptr = (ANNEX_TYPE_49_2G_POWERTABLE_VD *)pAnnex49[DeviceId][PathId];

	memset(ptr, 0, sizeof(ANNEX_TYPE_49_2G_POWERTABLE_VD));
	setAnnexInfoRevD(ptr, sizeof(ANNEX_TYPE_49_2G_POWERTABLE_VD),ANNEX_TYPE_NUM49,
					DeviceId, PathId);

	ptr->NumOfTxPowerLevelStore = (*pNumOfTxPowerLevelStore);

	if(pNumOfSavedCh)
	{
		 ptr->NumOfSavedCh= (*pNumOfSavedCh);
		for (i=0; i<(*pNumOfSavedCh); i++)
			ptr->SavedChannelIndex[i]= pChNumList[i] ;
		for (i=(*pNumOfSavedCh); i<MAXSAVEDCH_G; i++)
			ptr->SavedChannelIndex[i] = 0;
	}

	ptr->CalOption.Cal_Method = (*pCalMethod);
	ptr->CalOption.Close_TX_PW_Detection = (*pCloseTXPWDetection);
	ptr->CalOption.ExtPAPolarize = (*pExtPAPolarize);
	ptr->CalXtal = (*pCalXtal);
	ptr->PdetHiLoGap = (*pPdetHiLoGap);
	ptr->MCS20MPdetOffset = (*pMCS20MPdetOffset);
	ptr->MCS20MInitPOffset = (*pMCS20MInitPOffset);
	ptr->MCS40MPdetOffset = (*pMCS40MPdetOffset);
	ptr->MCS40MInitPOffset = (*pMCS40MInitPOffset);
	
	if(pNumOfSavedCh)
	{
		for (i=0; i<(*pNumOfSavedCh); i++)
		{
			for (j=0; j<RFPWRRANGE_NUM_REVD; j++)
			{
				if(pCorrPDetTh_B)	
					ptr->PaCalCCK[j][i].CorrPdetTh = pCorrPDetTh_B[i][j] ; 
				if(pCorrInitTrgt_B) 
					 ptr->PaCalCCK[j][i].CorrInitPwrTrgt = pCorrInitTrgt_B[i][j] ;

				if(pCorrPDetTh_G)	
					ptr->PaCalOFDM[j][i].CorrPdetTh = pCorrPDetTh_G[i][j] ; 
				if(pCorrInitTrgt_G) 
					 ptr->PaCalOFDM[j][i].CorrInitPwrTrgt = pCorrInitTrgt_G[i][j]; 
			}
		}

		for (i=(*pNumOfSavedCh); i<MAXSAVEDCH_G_REVD; i++)
		{
			for (j=0; j<RFPWRRANGE_NUM_REVD; j++)
			{
				ptr->PaCalCCK[j][i].CorrPdetTh = 0; 
				ptr->PaCalCCK[j][i].CorrInitPwrTrgt= 0;
				ptr->PaCalOFDM[j][i].CorrPdetTh = 0; 
				ptr->PaCalOFDM[j][i].CorrInitPwrTrgt= 0;
			}
		}
	}
	(*pNumOfTxPowerLevelStore) = ptr->NumOfTxPowerLevelStore;
	
	if(pPwrTbl)
	{
	//;	if((*pSizeOfPwrTblInByte) <= sizeof(ptr->fwPwrTbl.pwrTbl.pwrTblLevel))
			memcpy(ptr->pwrTblLevel, pPwrTbl,  sizeof(ptr->pwrTblLevel));
	//;	else
	//;		status = ERROR_MISMATCH;
	}
	
	return status;
}

int DutSharedClssHeader GetAnnexType49Data(
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
	
					BYTE* pPwrTbl)
{
	int status=0, i=0, j=0;
 	ANNEX_TYPE_49_2G_POWERTABLE_VD *ptr=NULL;
#if !defined (_MIMO_)
	int DeviceId=0, PathId=0;	
#endif // #if !defined (_MIMO_)

	if(NULL == pAnnex49[DeviceId][PathId]) return ERROR_DATANOTEXIST; 

	ptr = (ANNEX_TYPE_49_2G_POWERTABLE_VD *)pAnnex49[DeviceId][PathId];

	if(pNumOfTxPowerLevelStore)
		(*pNumOfTxPowerLevelStore) = ptr->NumOfTxPowerLevelStore;
	if(pNumOfSavedCh)
	{
		 (*pNumOfSavedCh) = ptr->NumOfSavedCh;
		for (i=0; i<(*pNumOfSavedCh); i++)
			 pChNumList[i] = ptr->SavedChannelIndex[i];
		for (i=(*pNumOfSavedCh); i<MAXSAVEDCH_G_REVD; i++)
			pChNumList[i] = 0;
	}

    if(pPdetHiLoGap)
		(*pPdetHiLoGap) = ptr->PdetHiLoGap;
	if(pCalMethod)
		(*pCalMethod) = ptr->CalOption.Cal_Method;
	if(pCloseTXPWDetection)
		(*pCloseTXPWDetection) = ptr->CalOption.Close_TX_PW_Detection;
	if(pExtPAPolarize)
		 (*pExtPAPolarize) = ptr->CalOption.ExtPAPolarize;
	if(pCalXtal)
	 (*pCalXtal) = ptr->CalXtal;

    
	if(pMCS20MPdetOffset)
		(*pMCS20MPdetOffset) = ptr->MCS20MPdetOffset;
	if(pMCS20MInitPOffset)
		(*pMCS20MInitPOffset) = ptr->MCS20MInitPOffset;
	if(pMCS40MPdetOffset)
		(*pMCS40MPdetOffset) = ptr->MCS40MPdetOffset;
	if(pMCS40MInitPOffset)
		(*pMCS40MInitPOffset) = ptr->MCS40MInitPOffset;

	if(pNumOfSavedCh)
	{
		for (i=0; i<(*pNumOfSavedCh); i++)
		for (j=0; j<RFPWRRANGE_NUM_REVD; j++)
		{
			if(pCorrPDetTh_B)	
				pCorrPDetTh_B[i][j] = ptr->PaCalCCK[j][i].CorrPdetTh; 
			if(pCorrInitTrgt_B) 
				pCorrInitTrgt_B[i][j] = ptr->PaCalCCK[j][i].CorrInitPwrTrgt;
			if(pCorrPDetTh_G)	
				pCorrPDetTh_G[i][j] = ptr->PaCalOFDM[j][i].CorrPdetTh; 
			if(pCorrInitTrgt_G) 
				pCorrInitTrgt_G[i][j] = ptr->PaCalOFDM[j][i].CorrInitPwrTrgt;
		}


		for (i=(*pNumOfSavedCh); i<MAXSAVEDCH_G_REVD; i++)
		for (j=0; j<RFPWRRANGE_NUM_REVD; j++)
			{
				pCorrPDetTh_B[i][j] = 0; 
				pCorrInitTrgt_B[i][j]= 0; 
				pCorrPDetTh_G[i][j] = 0; 
				pCorrInitTrgt_G[i][j]= 0;
		}
	}

	if(*pNumOfTxPowerLevelStore)
	(*pNumOfTxPowerLevelStore) = ptr->NumOfTxPowerLevelStore;
	
	if(pPwrTbl)
	{
	//;	if((*pSizeOfPwrTblInByte) <= sizeof(ptr->fwPwrTbl.pwrTbl.pwrTblLevel))
			memcpy(pPwrTbl, ptr->pwrTblLevel, sizeof(ptr->pwrTblLevel));
	//;	else
	//;		status = ERROR_MISMATCH;
	}
	return status;
}

int DutSharedClssHeader GetAnnexType52Data( 
#if defined (_MIMO_)
					   int DeviceId,		int PathId,  
#endif //#if defined (_MIMO_)
					    bool *Cal2G,
						bool *Cal5G,
						BYTE *LoLeakageCal2G_I,
						BYTE *LoLeakageCal2G_Q,
						BYTE LoLeakageCal5G_I[NUM_SUBBAND_A_REVD],
						BYTE LoLeakageCal5G_Q[NUM_SUBBAND_A_REVD]
						) 
{
	int status=0, i=0;
 	ANNEX_TYPE_52_LOLEAKAGE_VD *ptr=NULL;
#if !defined (_MIMO_)
	int DeviceId=0, PathId=0;	
#endif // #if !defined (_MIMO_)

	// parse the data and pass it back to app. 
	if(NULL == pAnnex52) return ERROR_DATANOTEXIST; 

	// parse the data and pass it back to app. 
	if(NULL == pAnnex52[DeviceId][PathId]) return ERROR_DATANOTEXIST; 

	ptr = (ANNEX_TYPE_52_LOLEAKAGE_VD *)pAnnex52[DeviceId][PathId];
	
	if(Cal2G) (*Cal2G) = ptr->CalFlag.Cal_2G;
	if(Cal5G) (*Cal5G) = ptr->CalFlag.Cal_5G;
	if(LoLeakageCal2G_I) (*LoLeakageCal2G_I) = ptr->Cal2G.CAL_I;
	if(LoLeakageCal2G_Q) (*LoLeakageCal2G_Q) = ptr->Cal2G.CAL_Q;
	if(LoLeakageCal5G_I && LoLeakageCal5G_Q)
	{
		for (int i=0; i<NUM_SUBBAND_A_REVD;i ++)
		{
			LoLeakageCal5G_I[i] = ptr->Cal5G[i].CAL_I;
			LoLeakageCal5G_Q[i] = ptr->Cal5G[i].CAL_Q;
		}
	}


		return status;
}

int DutSharedClssHeader SetAnnexType52Data(
#if defined (_MIMO_)
					   int DeviceId,		int PathId,  
#endif //#if defined (_MIMO_)
					    bool Cal_2G,
						bool Cal_5G,
						BYTE LoLeakageCal2G_I,
						BYTE LoLeakageCal2G_Q,
						BYTE LoLeakageCal5G_I[NUM_SUBBAND_A_REVD],
						BYTE LoLeakageCal5G_Q[NUM_SUBBAND_A_REVD])
{	
	int status=0, i=0, j=0;
 	ANNEX_TYPE_52_LOLEAKAGE_VD *ptr=NULL;
#if !defined (_MIMO_)
	int DeviceId=0, PathId=0;	
#endif // #if !defined (_MIMO_)

	// alloc space to pointer, fill structure, other than next pointer and csc
	
	if(NULL == pAnnex52[DeviceId][PathId])
		pAnnex52[DeviceId][PathId] = malloc(sizeof(ANNEX_TYPE_52_LOLEAKAGE_VD));
	ptr = (ANNEX_TYPE_52_LOLEAKAGE_VD *)pAnnex52[DeviceId][PathId];
	memset(ptr, 0, sizeof(ANNEX_TYPE_52_LOLEAKAGE_VD));
	setAnnexInfo(ptr, sizeof(ANNEX_TYPE_52_LOLEAKAGE_VD),ANNEX_TYPE_NUM52,
					DeviceId, PathId, 0,0); 

	if(Cal_2G)			  ptr->CalFlag.Cal_2G = (Cal_2G);
	if(Cal_5G)			  ptr->CalFlag.Cal_5G = (Cal_5G);	
	if(LoLeakageCal2G_I)  ptr->Cal2G.CAL_I = LoLeakageCal2G_I;
	if(LoLeakageCal2G_Q)  ptr->Cal2G.CAL_Q = LoLeakageCal2G_Q;
	if(LoLeakageCal5G_I && LoLeakageCal5G_Q)
	{
		for (int i=0; i<NUM_SUBBAND_A_REVD;i ++)
		{
			ptr->Cal5G[i].CAL_I = LoLeakageCal5G_I[i];
			ptr->Cal5G[i].CAL_Q = LoLeakageCal5G_Q[i];
		}
	}
	
	return status;
}

int DutSharedClssHeader GetAnnexType53Data( 
#if defined (_MIMO_)
					   int DeviceId,		int PathId,  
#endif //#if defined (_MIMO_)
						bool  *Cal2G,
						bool  *Cal5G,
						char *TxIQmismatch_Amp_2G,	
						char *TxIQmismatch_Phase_2G,
						char TxIQmismatch_Amp_5G[NUM_SUBBAND_A_REVD],	
						char TxIQmismatch_Phase_5G[NUM_SUBBAND_A_REVD])
{
	int status=0, i=0;
 	ANNEX_TYPE_53_TX_IQ_LOLEAKAGE_VD *ptr=NULL;
#if !defined (_MIMO_)
	int DeviceId=0, PathId=0;	
#endif // #if !defined (_MIMO_)

	// parse the data and pass it back to app. 
	if(NULL == pAnnex53) return ERROR_DATANOTEXIST; 

	// parse the data and pass it back to app. 
	if(NULL == pAnnex53[DeviceId][PathId]) return ERROR_DATANOTEXIST; 

	ptr = (ANNEX_TYPE_53_TX_IQ_LOLEAKAGE_VD *)pAnnex53[DeviceId][PathId];
	
	if(Cal2G) (*Cal2G) = ptr->CalFlag.Cal_2G;
	if(Cal5G) (*Cal5G) = ptr->CalFlag.Cal_5G;
	if(TxIQmismatch_Amp_2G) (*TxIQmismatch_Amp_2G) = ptr->CalTxIQ_2G.Amp;
	if(TxIQmismatch_Phase_2G) (*TxIQmismatch_Phase_2G) = ptr->CalTxIQ_2G.Phase;
	if(TxIQmismatch_Amp_5G && TxIQmismatch_Phase_5G)
	{
		for (int i=0; i<NUM_SUBBAND_A_REVD;i ++)
		{
			TxIQmismatch_Amp_5G[i] = ptr->CalTxIQ_5G[i].Amp;
			TxIQmismatch_Phase_5G[i] = ptr->CalTxIQ_5G[i].Phase;
		}
	}
	 return status;
}

int DutSharedClssHeader SetAnnexType53Data(
#if defined (_MIMO_)
					   int DeviceId,		int PathId,  
#endif //#if defined (_MIMO_)
						bool  Cal2G,
						bool  Cal5G,
						char TxIQmismatch_Amp_2G,	
						char TxIQmismatch_Phase_2G,
						char TxIQmismatch_Amp_5G[NUM_SUBBAND_A_REVD],	
						char TxIQmismatch_Phase_5G[NUM_SUBBAND_A_REVD])
{
    int status=0;
 	ANNEX_TYPE_53_TX_IQ_LOLEAKAGE_VD *ptr=NULL;
#if !defined (_MIMO_)
	int DeviceId=0, PathId=0;	
#endif // #if !defined (_MIMO_)

	// alloc space to pointer, fill structure, other than next pointer and csc
	if(NULL == pAnnex53[DeviceId][PathId])
		pAnnex53[DeviceId][PathId] = malloc(sizeof(ANNEX_TYPE_53_TX_IQ_LOLEAKAGE_VD));
	ptr = (ANNEX_TYPE_53_TX_IQ_LOLEAKAGE_VD *)pAnnex53[DeviceId][PathId];
	memset(ptr, 0, sizeof(ANNEX_TYPE_53_TX_IQ_LOLEAKAGE_VD));
	setAnnexInfoRevD(ptr, sizeof(ANNEX_TYPE_53_TX_IQ_LOLEAKAGE_VD),ANNEX_TYPE_NUM53,
					DeviceId, PathId); 

	ptr->CalFlag.Cal_2G = Cal2G;
	ptr->CalFlag.Cal_5G = Cal5G;
	ptr->CalTxIQ_2G.Amp = TxIQmismatch_Amp_2G;
    ptr->CalTxIQ_2G.Phase = TxIQmismatch_Phase_2G;

		for (int i=0; i<NUM_SUBBAND_A_REVD;i ++)
		{
			ptr->CalTxIQ_5G[i].Amp = TxIQmismatch_Amp_5G[i];
			ptr->CalTxIQ_5G[i].Phase = TxIQmismatch_Phase_5G[i];
		}


	 return status;
}

int DutSharedClssHeader GetAnnexType54Data(
#if defined (_MIMO_)
					   int DeviceId,		int PathId,  
#endif //#if defined (_MIMO_)
						bool  *Cal2G,
						bool  *Cal5G,
						char *RxIQmismatch_Amp_2G,	
						char *RxIQmismatch_Phase_2G,
						char RxIQmismatch_Amp_5G[NUM_SUBBAND_A_REVD],	
						char RxIQmismatch_Phase_5G[NUM_SUBBAND_A_REVD])
{

	int status=0, i=0;
 	ANNEX_TYPE_54_RX_IQ_LOLEAKAGE_VD *ptr=NULL;
#if !defined (_MIMO_)
	int DeviceId=0, PathId=0;	
#endif // #if !defined (_MIMO_)

	// parse the data and pass it back to app. 
	if(NULL == pAnnex54) return ERROR_DATANOTEXIST; 

	// parse the data and pass it back to app. 
	if(NULL == pAnnex54[DeviceId][PathId]) return ERROR_DATANOTEXIST; 

	ptr = (ANNEX_TYPE_54_RX_IQ_LOLEAKAGE_VD *)pAnnex54[DeviceId][PathId];
	
	if(Cal2G) (*Cal2G) = ptr->CalFlag.Cal_2G;
	if(Cal5G) (*Cal5G) = ptr->CalFlag.Cal_5G;
	if(RxIQmismatch_Amp_2G) (*RxIQmismatch_Amp_2G) = ptr->CalRxIQ_2G.Amp;
	if(RxIQmismatch_Phase_2G) (*RxIQmismatch_Phase_2G) = ptr->CalRxIQ_2G.Phase;
	if(RxIQmismatch_Amp_5G && RxIQmismatch_Phase_5G)
	{
		for (int i=0; i<NUM_SUBBAND_A_REVD;i ++)
		{
			RxIQmismatch_Amp_5G[i] = ptr->CalRxIQ_5G[i].Amp;
			RxIQmismatch_Phase_5G[i] = ptr->CalRxIQ_5G[i].Phase;
		}
	}

	 return status;
}

int DutSharedClssHeader SetAnnexType54Data( 
#if defined (_MIMO_)
					   int DeviceId,		int PathId,  
#endif //#if defined (_MIMO_)
						bool  Cal2G,
						bool  Cal5G,
						char RxIQmismatch_Amp_2G,	
						char RxIQmismatch_Phase_2G,
						char RxIQmismatch_Amp_5G[NUM_SUBBAND_A_REVD],	
						char RxIQmismatch_Phase_5G[NUM_SUBBAND_A_REVD])
{
	int status=0;
 	ANNEX_TYPE_54_RX_IQ_LOLEAKAGE_VD *ptr=NULL;
#if !defined (_MIMO_)
	int DeviceId=0, PathId=0; 
#endif //#if defined (_MIMO_)

	// alloc space to pointer, fill structure, other than next pointer and csc
	if(NULL == pAnnex54[DeviceId][PathId])
		pAnnex54[DeviceId][PathId] = malloc(sizeof(ANNEX_TYPE_54_RX_IQ_LOLEAKAGE_VD));
	ptr = (ANNEX_TYPE_54_RX_IQ_LOLEAKAGE_VD *)pAnnex54[DeviceId][PathId];
	memset(ptr, 0, sizeof(ANNEX_TYPE_54_RX_IQ_LOLEAKAGE_VD));
	setAnnexInfoRevD(ptr, sizeof(ANNEX_TYPE_54_RX_IQ_LOLEAKAGE_VD),ANNEX_TYPE_NUM54,
						DeviceId, PathId); 

	ptr->CalFlag.Cal_2G = Cal2G;
	ptr->CalFlag.Cal_5G = Cal5G;
	ptr->CalRxIQ_2G.Amp = RxIQmismatch_Amp_2G;
	ptr->CalRxIQ_2G.Phase = RxIQmismatch_Phase_2G;
	
	for (int i=0; i<NUM_SUBBAND_A_REVD;i ++)
	{
		ptr->CalRxIQ_5G[i].Amp = RxIQmismatch_Amp_5G[i];
		ptr->CalRxIQ_5G[i].Phase = RxIQmismatch_Phase_5G[i];
	}

	 return 0;
}

int DutSharedClssHeader GetAnnexType55Data(
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
					BYTE BDAddr[SIZE_BDADDRESS_INBYTE],
					BYTE *Encr_Key_Len_Max,
					BYTE *Encr_Key_Len_Min
					)
{
	int status=0, i=0;
 	ANNEX_TYPE_55_BT_CONFIGURE *ptr=NULL;
	// parse the data and pass it back to app. 

	// parse the data and pass it back to app. 
	if(NULL == pAnnex55) return ERROR_DATANOTEXIST; 

	ptr = (ANNEX_TYPE_55_BT_CONFIGURE *)pAnnex55;

	if (Version)
		 (*Version) = ptr->Version;

	if (RFXtal)
		(*RFXtal) = ptr->RFXtal;

	if (InitPwr)
		(*InitPwr) = ptr->InitPwr;
	if(FELoss)
		(*FELoss) = ptr->FELoss;
    if(ForceClass2Op)
		(*ForceClass2Op) = ptr->BTOption.ForceClass2Op;
	if(DisablePwrControl)
		(*DisablePwrControl) = ptr->BTOption.DisablePwrControl;
	if(MiscFlag)
		(*MiscFlag) = ptr->BTOption.MiscFlag;
    if(UsedInternalSleepClock)
		(*UsedInternalSleepClock) = ptr->BTOption.UsedInternalSleepClock;
	if(Rssi_Golden_Lo)
		(*Rssi_Golden_Lo) = ptr->Rssi_Golden_Lo;
	if(Rssi_Golden_Hi)
		(*Rssi_Golden_Hi) = ptr->Rssi_Golden_Hi;
	if(BTBAUDRate)
		 (*BTBAUDRate) = ptr->BTBAUDRate;
    if(BDAddr)
		for (i=0; i<SIZE_BDADDRESS_INBYTE; i++)
			BDAddr[i] = ptr->BDAddr[SIZE_BDADDRESS_INBYTE-i-1];

	if(Encr_Key_Len_Max)
		(*Encr_Key_Len_Max)= ptr->Encr_Key_Len_Max;	
	if(Encr_Key_Len_Min)
		(*Encr_Key_Len_Min)= ptr->Encr_Key_Len_Min;	

	return status;
}

int DutSharedClssHeader SetAnnexType55Data( 
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
					BYTE BDAddr[SIZE_BDADDRESS_INBYTE],
					BYTE Encr_Key_Len_Max,
					BYTE Encr_Key_Len_Min
					)
{
	int status=0, i=0;
 	ANNEX_TYPE_55_BT_CONFIGURE *ptr=NULL;
 
	// alloc space to pointer, fill structure, other than next pointer and csc
	
	if(NULL == pAnnex55)
		pAnnex55 = malloc(sizeof(ANNEX_TYPE_55_BT_CONFIGURE));
	ptr = (ANNEX_TYPE_55_BT_CONFIGURE *)pAnnex55;
	memset(ptr, 0, sizeof(ANNEX_TYPE_55_BT_CONFIGURE));
	setAnnexInfoRevD(ptr, sizeof(ANNEX_TYPE_55_BT_CONFIGURE),ANNEX_TYPE_NUM55,
						0, 0);
	
	ptr->Version = Version;
	ptr->RFXtal	= RFXtal;
	ptr->InitPwr	= InitPwr;
	ptr->FELoss	= FELoss;

	ptr->BTOption.ForceClass2Op	= ForceClass2Op;
	ptr->BTOption.DisablePwrControl	= DisablePwrControl;
	ptr->BTOption.MiscFlag	= MiscFlag;
	ptr->BTOption.UsedInternalSleepClock	= UsedInternalSleepClock;

	ptr->Rssi_Golden_Lo	= Rssi_Golden_Lo;
	ptr->Rssi_Golden_Hi	= Rssi_Golden_Hi;

	ptr->BTBAUDRate = BTBAUDRate;

	for (i=0; i<SIZE_BDADDRESS_INBYTE; i++)
		ptr->BDAddr[i] = BDAddr[SIZE_BDADDRESS_INBYTE-i-1];

	ptr->Encr_Key_Len_Max	= Encr_Key_Len_Max;	
	ptr->Encr_Key_Len_Min	= Encr_Key_Len_Min;	
	return status;
}

//Annex 56
int DutSharedClssHeader GetAnnexType56Data( 
					BYTE Lincense_Key[16],
	                WORD *Link_Key_Mask,
	                BYTE Key_Value[3][22])
{
	int status=0, i=0;
 	ANNEX_TYPE_56_BT_SECURITY_DATA *ptr=NULL;
	// parse the data and pass it back to app. 
	if(NULL == pAnnex56) return ERROR_DATANOTEXIST; 

	// parse the data and pass it back to app. 
	if(NULL == pAnnex56) return ERROR_DATANOTEXIST; 

	ptr = (ANNEX_TYPE_56_BT_SECURITY_DATA *)pAnnex56;

	if(Lincense_Key)
		memcpy(Lincense_Key, ptr->Lincense_Key,  sizeof(Lincense_Key));
	
	if(Link_Key_Mask)
	//;	memcpy(ptr->Link_Key_Mask, Link_Key_Mask, sizeof(Link_Key_Mask));
	(*Link_Key_Mask) = ptr->Link_Key_Mask;

	if(Key_Value)
		memcpy(Key_Value, ptr->Key_Value, sizeof(ptr->Key_Value));

	return status;
}

int DutSharedClssHeader SetAnnexType56Data( 
					BYTE Lincense_Key[16],
	                WORD Link_Key_Mask,
	                BYTE Key_Value[3][22])
{
	int status=0;
 	ANNEX_TYPE_56_BT_SECURITY_DATA *ptr=NULL;
#if !defined (_MIMO_)
	int DeviceId=0, PathId=0; 
#endif //#if defined (_MIMO_)

	// alloc space to pointer, fill structure, other than next pointer and csc
	
	if(NULL == pAnnex56)
		pAnnex56 = malloc(sizeof(ANNEX_TYPE_56_BT_SECURITY_DATA));
	ptr = (ANNEX_TYPE_56_BT_SECURITY_DATA *)pAnnex56;
	memset(ptr, 0, sizeof(ANNEX_TYPE_56_BT_SECURITY_DATA));
	setAnnexInfoRevD(ptr, sizeof(ANNEX_TYPE_56_BT_SECURITY_DATA),ANNEX_TYPE_NUM56,
						0, 0); 

	memcpy(ptr->Lincense_Key, Lincense_Key,  sizeof(ptr->Lincense_Key));

	ptr->Link_Key_Mask =Link_Key_Mask  ;

	memcpy( ptr->Key_Value,  Key_Value, sizeof(ptr->Key_Value));
	return status;
}

int DutSharedClssHeader GetAnnexType57Data(
#if defined (_MIMO_)
					   int DeviceId,		int PathId,  
#endif //#if defined (_MIMO_)
					BYTE Band,
					BYTE SubBand,
					BYTE *MEM_ROW,
					BYTE *NumOfEnterirs,
					DPD_ConeffcicentData *Coneffcicent_Data)

{	
	int status=0, i=0, j=0;

 	ANNEX_TYPE_57_DPD_DATA *ptr=NULL;
	DPD_ConeffcicentData *PtrConeffcicentData= (DPD_ConeffcicentData *) Coneffcicent_Data;
	DPD_ConeffcicentData *AnnexPtr=NULL;
	int ConeffcicentDataSize = sizeof(DPD_ConeffcicentData);
#if !defined (_MIMO_)
	int DeviceId=0, PathId=0;
#endif // #if defined (_MIMO_)

	if(NULL == pAnnex57[DeviceId][PathId][Band+SubBand]) return ERROR_DATANOTEXIST; 

	ptr = (ANNEX_TYPE_57_DPD_DATA *)pAnnex57[DeviceId][PathId][Band+SubBand];
      
    (*NumOfEnterirs) = ((ptr->typeHdr.LengthInByte) - sizeof(ANNEX_TYPE_HEADER_VC)-4) / ConeffcicentDataSize;
    
	AnnexPtr = (DPD_ConeffcicentData*)ptr->AnnexConeffcicentData;
	SubBand = (BYTE)ptr->SubBandId;
	Band = (BYTE)ptr->BandId;
	for (i=0; i<(*NumOfEnterirs); i++)
	{
		Coneffcicent_Data[i].DPDIdx=AnnexPtr->DPDIdx;
		Coneffcicent_Data[i].PowerCode=AnnexPtr->PowerCode;
		Coneffcicent_Data[i].StartChan=AnnexPtr->StartChan;
		Coneffcicent_Data[i].EndChan=AnnexPtr->EndChan;	
		for(j=0; j<(DPD_COEFF_ENTRY_NUM); j++)
		{ 
			PtrConeffcicentData->ConeffcicentData[j].RealHi = AnnexPtr->ConeffcicentData[j].RealHi;
			PtrConeffcicentData->ConeffcicentData[j].RealLo= AnnexPtr->ConeffcicentData[j].RealLo;
			PtrConeffcicentData->ConeffcicentData[j].ImageHi= AnnexPtr->ConeffcicentData[j].ImageHi;
			PtrConeffcicentData->ConeffcicentData[j].ImageLo= AnnexPtr->ConeffcicentData[j].ImageLo;			
		}
		PtrConeffcicentData++;
        AnnexPtr++;
	}

	return status;
}

int DutSharedClssHeader SetAnnexType57Data(
#if defined (_MIMO_)
					int DeviceId,		int PathId,  
#endif //#if defined (_MIMO_)
					BYTE Band,
					BYTE SubBand,
					BYTE MEM_ROW,
					BYTE NumOfEnterirs,
					DPD_ConeffcicentData *Coneffcicent_Data)
{	
	int status=0, i=0, j=0;

 	ANNEX_TYPE_57_DPD_DATA *ptr=NULL;
	BYTE *ConeffcicentData=NULL;
	int ConeffcicentDataSize = (sizeof(DPD_ConeffcicentData))*(NumOfEnterirs);
 #if !defined (_MIMO_)
	int DeviceId=0, PathId=0; 
#endif //#if defined (_MIMO_)

	// alloc space to pointer, fill structure, other than next pointer and csc
	if(NULL == pAnnex57[DeviceId][PathId][Band+SubBand])
		pAnnex57[DeviceId][PathId][Band+SubBand] = malloc(sizeof(ANNEX_TYPE_HEADER_VC)+ ConeffcicentDataSize+4);
	ptr = (ANNEX_TYPE_57_DPD_DATA *)pAnnex57[DeviceId][PathId][Band+SubBand];
	memset(ptr, 0, sizeof(ANNEX_TYPE_HEADER_VC)+ 
		ConeffcicentDataSize);
	//header
	ptr->typeHdr.AnnexType= ANNEX_TYPE_NUM57;
	ptr->typeHdr.LengthInByte = sizeof(ANNEX_TYPE_HEADER_VC)+ ConeffcicentDataSize+4;
#if defined (_MIMO_)
	ptr->DeviceId = DeviceId;
	ptr->PathId = PathId;
#endif //#if defined (_MIMO_)
	ptr->BandId = Band;
	ptr->SubBandId = SubBand;
	ptr->MEMRow = MEM_ROW;
	ptr->NUMOfEntries = NumOfEnterirs;
	memcpy(ptr->AnnexConeffcicentData, Coneffcicent_Data, ConeffcicentDataSize);

	return status;
}

int DutSharedClssHeader CheckHeaderTbl(DWORD HeaderTbl[], DWORD *Version, int *CsC)
{
	int status=ERROR_NONE;

	*Version =  ((HeaderTbl[VMR_SPIVERSION_OFFSET/sizeof(DWORD)] >>16 ) &0xFFFF);
		
 	if(0x0 == (*Version) || 0xFFFF == (*Version))
 		*Version = 0xFFFFFFFF;

 // DoCsCheck 

	DoCs0Check(HeaderTbl, VMR_HEADER_LENGTH, CsC); 
 	return status;
}

bool DutSharedClssHeader SupportNoE2PROM()
{ 
	return  (g_NoEeprom!=0);
}
  
////////////////////////
// WLAN
int DutSharedClssHeader LoadHwData( 
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
				WORD *pTestToolVer,		WORD *pDllVer) 
{ 
	return 0;	  
}
