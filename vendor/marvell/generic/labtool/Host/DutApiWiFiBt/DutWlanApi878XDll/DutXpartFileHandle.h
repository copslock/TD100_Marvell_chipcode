/** @file DutXpartFileHandle.h
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#if !defined (__DUTXPARTFILEHANDLE_H__)
#define __DUTXPARTFILEHANDLE_H__

int GetFEMFileName(char *FileName);

int GetBandSubBandTag(DWORD Band, DWORD SubBand, char *Tag);

const char  TagFormat_BandSubBand[] ="BAND%c_SUB%1d"; 
//jsz 
/*int LoadXPASettingPerPath(int AllowAbsent=false, 
											   int DeviceId=MIMODEVICE0,
											   int PathId=TXPATH_PATHA, 
											   int OfdmOffset_G[NUM_SUBBAND_G][RFPWRRANGE_NUM]=NULL,
											   char *FileName=NULL); 
int LoadXLNASettingPerPath(int AllowAbsent=false, 
												int DeviceId=MIMODEVICE0,
												int PathId=RXPATH_PATHA, 
												char *FileName=NULL); 

//jsz int LoadXPASetting(int AllowAbsent=false, 
						//					   int DeviceId=MIMODEVICE0,
						//					   int PathId=TXPATH_PATHA, 
//jsz											   int OfdmOffset_G[MAXNUM_MIMODEVICE][MAXNUM_TXPATH][NUM_SUBBAND_G][RFPWRRANGE_NUM]=NULL,
//jsz											   char *FileName=NULL); 

//jsz int LoadXLNASetting(int AllowAbsent=false, 
						//						int DeviceId=MIMODEVICE0,
						//						int PathId=RXPATH_PATHA, 
//jsz												char *FileName=NULL); 

int GetExtPaFileName(DWORD DeviceId, DWORD PathId, char *FileName);

int GetExtLnaFileName(DWORD DeviceId, DWORD PathId, char *FileName);

const char XPAFileTagFormat[255]="ExtPA_PAFile_Device%d_Path%d";
const char XLNAFileTagFormat[255]="ExtLNA_LNAFile_Device%d_Path%d";
*/ //jsz

#endif

