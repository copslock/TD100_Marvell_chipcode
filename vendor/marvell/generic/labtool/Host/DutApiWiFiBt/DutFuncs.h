/** @file DutFuncs.h
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

int LoadInitConfigFile ( char *FileName); 
int LoadInitBtConfigFile (char *FileName);
#ifdef _FM_
int LoadInitFmConfigFile (char *FileName);
#endif // #ifdef _FM_

int LoadThermalTestFile(char *FileName, DWORD *pPTarget, DWORD *pTempRef,
						DWORD *pSlopeNumerator, DWORD *pSlopeDenominator,
						DWORD *pCalibrationIntervalInMS,
						DWORD *pTemp,
						DWORD *pSlopeNumerator1, DWORD *pSlopeDenominator1);

void FlexSpiDLSaveCalMacAddr(void);

#ifndef _FLASH_
 
void SetPidVid(void);

void GetPidVid(void); 

#endif //#ifndef _FLASH_
