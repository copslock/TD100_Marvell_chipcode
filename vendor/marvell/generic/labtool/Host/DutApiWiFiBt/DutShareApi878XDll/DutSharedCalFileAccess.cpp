/** @file DutSharedCalFileAccess.cpp
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#if defined(_CAL_REV_D_)
int DutSharedClssHeader ReadMainDataFromFile_RevD( char *FileName,
						BYTE *pStructRev,
						BYTE *pDesignType,
						BYTE *pLNAGain,
						BYTE *pAntDiversityInfo,
						BYTE *pFEVersion,

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
						BYTE pMimoMap[MAXNUM_MIMODEVICE][MAXNUM_RXPATH])
{
	char	value[MAX_LENGTH]="";
	char	key[MAX_LENGTH]="";
	int		status=0, i=0, j=0;

	//header
	DebugLogRite("File Name :%s\n", FileName);

	//header
 	if(pStructRev)
		(*pStructRev) = GetPrivateProfileInt(TAG_STRUCTINFO, KEY_STRUCTURE_REV, 
                                (*pStructRev), FileName); 
 
	//Rf Design
 	if(pDesignType)
		(*pDesignType) = GetPrivateProfileInt(TAG_MAIN, KEY_DESIGN_TYPE, 
										(*pDesignType), FileName); 

     //ant option
 	for (i=0; i<MAXNUM_MIMODEVICE; i++)
	{
		sprintf(key, KEY_ANT_TX_FORMAT, i); 
   		if(pAntTx)
			pAntTx[i] = GetPrivateProfileInt(TAG_MAIN, key, 
											pAntTx[i], FileName); 

  		sprintf(key, KEY_ANT_RX_FORMAT, i); 
   		if(pAntRx)
			pAntRx[i] = GetPrivateProfileInt(TAG_MAIN, key, 
											pAntRx[i], FileName); 
	}
	
    if(pLNAGain)
		(*pLNAGain) = GetPrivateProfileInt(TAG_MAIN, KEY_LNA_GAIN, 
											(*pLNAGain), FileName); 
	
	if(pAntDiversityInfo)
		(*pAntDiversityInfo) = GetPrivateProfileInt(TAG_MAIN, KEY_ANT_DIV_INFO, 
											(*pLNAGain), FileName);

	//PA option
	if(pPaOption_ExtPaPolar_PathABC_G)
		(*pPaOption_ExtPaPolar_PathABC_G) = (GetPrivateProfileInt(TAG_MAIN, KEY_EXTPA_POLAR_G_FORMAT, 
											(*pPaOption_ExtPaPolar_PathABC_G), FileName))?true:false;
	if(pPaOption_ExtPaPolar_PathABC_A)
		(*pPaOption_ExtPaPolar_PathABC_A) = (GetPrivateProfileInt(TAG_MAIN, KEY_EXTPA_POLAR_A_FORMAT, 
											(*pPaOption_ExtPaPolar_PathABC_A), FileName))?true:false;

	GetPrivateProfileString(TAG_MAIN,KEY_SPI_SIZE, VAL_SPI_SIZE_DEF, value, 20, FileName); 
	{
		int tempInt=0; 
		sscanf(value, "%d%s", &tempInt, key);
		if(pSpiSizeInfo) (*pSpiSizeInfo) = tempInt;
	}

	if(pFEVersion)
		(*pFEVersion) =  GetPrivateProfileInt(TAG_MAIN, KEY_FE_VERSION,
										(*pFEVersion), FileName); 

	if(pCC)
		(*pCC) = GetPrivateProfileInt(TAG_MAIN, KEY_REGION_CODE, 
										(*pCC), FileName); 
	//FWM capability
    if(pCapable_2G)
		(*pCapable_2G) = (GetPrivateProfileInt(TAG_MAIN,KEY_FEM_CAP_2G,(*pCapable_2G),FileName))?true:false; 

	if(pCapable_5G)

		(*pCapable_5G)= (GetPrivateProfileInt(TAG_MAIN,KEY_FEM_CAP_5G,(*pCapable_5G),FileName))?true:false;
    
	if(pInternalBluetooth)
		(*pInternalBluetooth) =(GetPrivateProfileInt(TAG_MAIN,KEY_FEM_INTERNAL_BT,(*pInternalBluetooth),FileName))?true:false;
	
	if(pAntennaDiversity)
		(*pAntennaDiversity) = (GetPrivateProfileInt(TAG_MAIN,KEY_FEM_ANT_DIV,(*pInternalBluetooth),FileName))?true:false; 
	
	if(pLNA_2G)
		(*pLNA_2G) = (GetPrivateProfileInt(TAG_MAIN,KEY_FEM_EXTERNAL_2G_LNA,(*pLNA_2G),FileName))?true:false;

	if(pLNA_5G)
		(*pLNA_5G) = (GetPrivateProfileInt(TAG_MAIN,KEY_FEM_EXTERNAL_5G_LNA,(*pLNA_5G),FileName))?true:false;

	if(pSameAnt_2G_BT)
		(*pSameAnt_2G_BT) = (GetPrivateProfileInt(TAG_MAIN,KEY_FEM_SAMEANT_2G_BT,(*pSameAnt_2G_BT),FileName))?true:false;

	if(pSameAnt_5G_BT)
		(*pSameAnt_5G_BT) = (GetPrivateProfileInt(TAG_MAIN,KEY_FEM_SAMEANT_5G_BT,(*pSameAnt_5G_BT),FileName))?true:false;

	//Concurrency
	if(pCapabilities_2G_BT)
	  (*pCapabilities_2G_BT)= GetPrivateProfileInt(TAG_MAIN,KEY_CONCURRENT_CAP_2G_BT, (*pCapabilities_2G_BT), FileName);

	if(pCapabilities_5G_BT)
	 (*pCapabilities_5G_BT) = GetPrivateProfileInt(TAG_MAIN,KEY_CONCURRENT_CAP_5G_BT, (*pCapabilities_5G_BT),FileName);
	
	//Frone-end
	if(pBTFrontEnd2G)
	{
		for (i=0; i<9; i++)
		{		
			 sprintf(key, KEY_FRONT_END_CONFIG_2G,i);
			pBTFrontEnd2G[i]=GetPrivateProfileInt(TAG_MAIN, key, pBTFrontEnd2G[i],FileName);		  
		}
		for (i=0; i<9; i++)
		{		
			  sprintf(key, KEY_FRONT_END_CONFIG_5G,i);
			pBTFrontEnd5G[i] = GetPrivateProfileInt(TAG_MAIN, key, pBTFrontEnd5G[i],FileName);		  
		}
	}
	if(pExtXtalSource)
		(*pExtXtalSource) = GetPrivateProfileInt(TAG_MAIN, KEY_MISC_FLAG_CRYSTAL, 
										(*pExtXtalSource), FileName); 

	if(pExtSleepClk)
		(*pExtSleepClk) = GetPrivateProfileInt(TAG_MAIN, KEY_MISC_FLAG_SLEEPCLOCK, 
										(*pExtSleepClk), FileName); 

	if(pWlanWakeUp)
		(*pWlanWakeUp) = GetPrivateProfileInt(TAG_MAIN, KEY_MISC_FLAG_WLANWAKEUP, 
										(*pWlanWakeUp), FileName); 

  	if(pTestToolVer)
		(*pTestToolVer) = GetPrivateProfileInt(TAG_MAIN, KEY_TEST_VERSION, 
										(*pTestToolVer), FileName); 

  	if(pMfgTaskVersion)
		(*pMfgTaskVersion) = GetPrivateProfileInt(TAG_MAIN, KEY_MFG_VERSION, 
										(*pMfgTaskVersion), FileName); 

  	if(pDllVersion)
		(*pDllVersion) = GetPrivateProfileInt(TAG_MAIN, KEY_DLL_VERSION, 
										(*pDllVersion), FileName); 

   	if(pSocOrRev)
		(*pSocOrRev) = GetPrivateProfileInt(TAG_MAIN, KEY_SOC_OR_REV, 
										(*pSocOrRev), FileName); 
 
   	if(pRfuOrRev)
		(*pRfuOrRev) = GetPrivateProfileInt(TAG_MAIN, KEY_RFU_OR_REV, 
										(*pRfuOrRev), FileName); 

   	if(pMimoTxCnt)
		(*pMimoTxCnt) = GetPrivateProfileInt(TAG_MAIN, KEY_MIMO_CONFIG_CNTTX, 
										(*pMimoTxCnt), FileName); 
	
   	if(pMimoRxCnt)
		(*pMimoRxCnt) = GetPrivateProfileInt(TAG_MAIN, KEY_MIMO_CONFIG_CNTRX, 
										(*pMimoRxCnt), FileName); 

  	if(pMimoMap) 
	{
		for (i=0; i<MAXNUM_MIMODEVICE; i++)
		{
 			for (j=0; j<MAXNUM_RXPATH; j++)
			{
  				sprintf(key, KEY_MIMO_CONFIG_MAP_FORMAT, i, j); 
				pMimoMap[i][j] = GetPrivateProfileInt(TAG_MAIN, key, 
										pMimoMap[i][j], FileName); 
			}
		}
	}

	return status;
}

int DutSharedClssHeader  WriteMainDataToFile_RevD(char *FileName,
						BYTE pStructRev,
						BYTE pDesignType,
						BYTE pLNAGain,
						BYTE AntDiversityInfo,
						BYTE pFEVersion,
						
							//LNA option
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
						BYTE Capabilities_2G_BT,
						BYTE Capabilities_5G_BT,

						BYTE  BTFrontEnd2G[9],
						BYTE  BTFrontEnd5G[9],	
					
						DWORD pTestToolVer,	DWORD pMfgTaskVersion,	
						DWORD pDllVersion, 
						BYTE pSocOrRev,		BYTE pRfuOrRev,
						BYTE pMimoTxCnt, 	BYTE pMimoRxCnt, 
						BYTE pMimoMap[MAXNUM_MIMODEVICE][MAXNUM_RXPATH]
						)
{
char	value[MAX_LENGTH]="";
	char	key[MAX_LENGTH]="";
	int		status=0, i=0, j=0;

	//header

	DebugLogRite("File Name :%s\n", FileName);

	//header

#ifdef _LINUX_
	//clear the file first
	{
	    FILE *hFileTemp=0;
		hFileTemp = fopen(FileName, "w");
		fclose (hFileTemp);
    }
#endif //#ifdef _LINUX_

	//header
	if(pStructRev) 
		sprintf(value, "0x%X", (pStructRev));
	else 
		strcpy(value, "0x0");
	WritePrivateProfileString(TAG_STRUCTINFO, KEY_STRUCTURE_REV, value, FileName);

	//Rf Design

	if(pDesignType) 
		sprintf(value, "0x%X", (pDesignType));
	else 
		strcpy(value, "0x0");
	WritePrivateProfileString(TAG_MAIN, KEY_DESIGN_TYPE, value, FileName);

     //ant option
 	for (i=0; i<MAXNUM_MIMODEVICE; i++)
	{
		if(pAntTx) 
			sprintf(value, "0x%X", pAntTx[i]); 
  		else 
			strcpy(value, "0x0");
		sprintf(key, KEY_ANT_TX_FORMAT, i); 
		WritePrivateProfileString(TAG_MAIN, key,value,FileName); 

		if(pAntRx) 
			sprintf(value, "0x%X", pAntRx[i]); 
  		else 
			strcpy(value, "0x0");
  		sprintf(key, KEY_ANT_RX_FORMAT, i); 
		WritePrivateProfileString(TAG_MAIN, key,value,FileName); 
	}
    
	if(pLNAGain) 
		sprintf(value, "%d", (pLNAGain)); 
	else
		strcpy(value,"0x00");
	WritePrivateProfileString(TAG_MAIN, KEY_LNA_GAIN, value, FileName);
	
	if(AntDiversityInfo)
		sprintf(value, "0x%X", AntDiversityInfo);
	else
		strcpy(value,"0x00");
	WritePrivateProfileString(TAG_MAIN, KEY_ANT_DIV_INFO, value, FileName);

	//LNA option
//;	if(pPaOption_ExtLnaFlag_PathABC_G)
//;		sprintf(value, "%d", (pPaOption_ExtLnaFlag_PathABC_G)); 
//;	else
//;		strcpy(value,"0x00");
//;	WritePrivateProfileString(TAG_MAIN, KEY_EXTLNA_G_FORMAT, value, FileName);


//;	if(pPaOption_ExtLnaFlag_PathABC_A)
//;		sprintf(value, "%d", (pPaOption_ExtLnaFlag_PathABC_A)); 
//;	else
//;		strcpy(value,"0x00");
//;	WritePrivateProfileString(TAG_MAIN, KEY_EXTLNA_A_FORMAT, value, FileName);

	for (i=0; i<MAXNUM_MIMODEVICE; i++)
	{
		sprintf(key, KEY_EXTPA_POLAR_G_FORMAT, i);
		//PA option
		if(pPaOption_ExtPaPolar_PathABC_G)
			sprintf(value, "%d", (pPaOption_ExtPaPolar_PathABC_G)); 
		else
			strcpy(value,"0x00");
			WritePrivateProfileString(TAG_MAIN, key, value, FileName);

        sprintf(key, KEY_EXTPA_POLAR_A_FORMAT, i);
		if(pPaOption_ExtPaPolar_PathABC_A)
			sprintf(value, "%d", (pPaOption_ExtPaPolar_PathABC_A)); 
		else
			strcpy(value,"0x00");
			WritePrivateProfileString(TAG_MAIN, key, value, FileName);
    } 

	if (pSpiSizeInfo)
		sprintf(value, "%dK", (pSpiSizeInfo));
 	else
		strcpy(value,"0x00");
	WritePrivateProfileString(TAG_MAIN,KEY_SPI_SIZE,value,FileName);
	
	if (pFEVersion)
		sprintf(value, "0x%X", (pFEVersion)); 
 	else
		strcpy(value,"0x00");
	WritePrivateProfileString(TAG_MAIN,KEY_FE_VERSION,value,FileName);

	if(pCC)
		sprintf(value, "0x%X", (pCC)); 
 	else
		strcpy(value,"0x00");
	WritePrivateProfileString(TAG_MAIN,KEY_REGION_CODE,value,FileName); 

	//FWM capability
    if(pCapable_2G)
		sprintf(value, "0x%X", (*pCapable_2G)); 
 	else
		strcpy(value,"0x00");
			WritePrivateProfileString(TAG_MAIN,KEY_FEM_CAP_2G,value,FileName); 

	if(pCapable_5G)
		sprintf(value, "0x%X", (*pCapable_5G)); 
 	else
		strcpy(value,"0x00");
			WritePrivateProfileString(TAG_MAIN,KEY_FEM_CAP_5G,value,FileName);
	
	if(pInternalBluetooth)
		sprintf(value, "0x%X", (*pInternalBluetooth)); 
 	else
		strcpy(value,"0x00");
			WritePrivateProfileString(TAG_MAIN,KEY_FEM_INTERNAL_BT,value,FileName);

    if(pAntennaDiversity)
		sprintf(value, "0x%X", (*pAntennaDiversity)); 
 	else
		strcpy(value,"0x00");
			WritePrivateProfileString(TAG_MAIN,KEY_FEM_ANT_DIV,value,FileName);

	if(pLNA_2G)
		sprintf(value, "0x%X", (*pLNA_2G)); 
 	else
		strcpy(value,"0x00");
			WritePrivateProfileString(TAG_MAIN,KEY_FEM_EXTERNAL_2G_LNA,value,FileName);

	if(pLNA_5G)
		sprintf(value, "0x%X", (*pLNA_5G)); 
 	else
		strcpy(value,"0x00");
			WritePrivateProfileString(TAG_MAIN,KEY_FEM_EXTERNAL_5G_LNA,value,FileName);

	
	if(pSameAnt_2G_BT)
		sprintf(value, "0x%X", (*pSameAnt_2G_BT)); 
 	else
		strcpy(value,"0x00");
			WritePrivateProfileString(TAG_MAIN,KEY_FEM_SAMEANT_2G_BT,value,FileName);

	if(pSameAnt_5G_BT)
		sprintf(value, "0x%X", (*pSameAnt_5G_BT)); 
 	else
		strcpy(value,"0x00");
			WritePrivateProfileString(TAG_MAIN,KEY_FEM_SAMEANT_5G_BT,value,FileName);
						
	//Concurrency
	if(Capabilities_2G_BT)
		sprintf(value, "0x%X", (Capabilities_2G_BT)); 
 	else
		strcpy(value,"0x00");
			WritePrivateProfileString(TAG_MAIN,KEY_CONCURRENT_CAP_2G_BT,value,FileName);
    

	if(Capabilities_5G_BT)
		sprintf(value, "0x%X", (Capabilities_5G_BT)); 
 	else
		strcpy(value,"0x00");
			WritePrivateProfileString(TAG_MAIN,KEY_CONCURRENT_CAP_5G_BT,value,FileName);


    //Mise
	if(pExtXtalSource) 
		sprintf(value, "0x%X", (pExtXtalSource)); 
 	else
		strcpy(value,"0x00");
	WritePrivateProfileString(TAG_MAIN,KEY_MISC_FLAG_CRYSTAL,value,FileName); 

  	if(pExtSleepClk) 
		sprintf(value, "0x%X", (pExtSleepClk)); 
 	else
		strcpy(value, "0x00");
	WritePrivateProfileString(TAG_MAIN,KEY_MISC_FLAG_SLEEPCLOCK,value,FileName); 
  
	if(pWlanWakeUp) 
		sprintf(value, "0x%X", (pWlanWakeUp)); 
 	else
		strcpy(value, "0x00");
	WritePrivateProfileString(TAG_MAIN,KEY_MISC_FLAG_WLANWAKEUP,value,FileName); 


	if(pTestToolVer)
		sprintf(value, "0x%04X", (pTestToolVer)); 
 	else
		strcpy(value,"0x00");
	WritePrivateProfileString(TAG_MAIN,KEY_TEST_VERSION, value, FileName); 

 	if(pMfgTaskVersion)
		sprintf(value, "0x%04X", (pMfgTaskVersion)); 
 	else
		strcpy(value,"0x00");
	WritePrivateProfileString(TAG_MAIN,KEY_MFG_VERSION, value, FileName); 

  	if(pDllVersion)
		sprintf(value, "0x%04X", (pDllVersion)); 
 	else
		strcpy(value,"0x00");
	WritePrivateProfileString(TAG_MAIN,KEY_DLL_VERSION,	value, FileName); 
 

	if(pSocOrRev)
		sprintf(value, "0x%X", (pSocOrRev)); 
 	else
		strcpy(value,"0x00");
	WritePrivateProfileString(TAG_MAIN, KEY_SOC_OR_REV, value,FileName); 
	
	if(pRfuOrRev)
		sprintf(value, "0x%X", (pRfuOrRev)); 
 	else
		strcpy(value,"0x00");
	WritePrivateProfileString(TAG_MAIN, KEY_RFU_OR_REV, value,FileName);  

	if(pMimoTxCnt)
   	sprintf(value, "0x%X", (pMimoTxCnt)); 
 	else
		strcpy(value,"0x00");
	WritePrivateProfileString(TAG_MAIN, KEY_MIMO_CONFIG_CNTTX, value,FileName);  
 
	if(pMimoRxCnt)
		sprintf(value, "0x%X", (pMimoRxCnt)); 
 	else
		strcpy(value,"0x00");
	WritePrivateProfileString(TAG_MAIN, KEY_MIMO_CONFIG_CNTRX, value,FileName);  

  	if(pMimoMap) 
	{
		for (i=0; i<MAXNUM_MIMODEVICE; i++)
		{
 			for (j=0; j<MAXNUM_RXPATH; j++)
			{
		
				sprintf(value, "0x%X", pMimoMap[i][j]); 
  				sprintf(key, KEY_MIMO_CONFIG_MAP_FORMAT, i, j); 
				WritePrivateProfileString(TAG_MAIN, key, value,FileName);  
			}
		}
	}
	else
	{
		for (i=0; i<MAXNUM_MIMODEVICE; i++)
		{
 			for (j=0; j<MAXNUM_RXPATH; j++)
			{
		
				sprintf(value, "0x%02X", 0x00); 
  				sprintf(key, KEY_MIMO_CONFIG_MAP_FORMAT, i, j); 
				WritePrivateProfileString(TAG_MAIN, key, value,FileName);  
			}
		}

	}
	//Frone-end
	for (i=0; i<9; i++)
	{	  sprintf(value, "0x%X", BTFrontEnd2G[i]);	
          sprintf(key, KEY_FRONT_END_CONFIG_2G,i);
		  WritePrivateProfileString(TAG_MAIN, key, value,FileName);		  

	}
	for (i=0; i<9; i++)
	{	  sprintf(value, "0x%X", BTFrontEnd5G[i]);	
          sprintf(key, KEY_FRONT_END_CONFIG_5G,i);
		  WritePrivateProfileString(TAG_MAIN, key, value,FileName);		  

	}
	return status;
}
#endif	//#if defined(_CAL_REV_D_)

int DutSharedClssHeader ReadMainDataFromFile(char *FileName,
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
						)
{
 	char	value[MAX_LENGTH]="";
	char	key[MAX_LENGTH]="";
	int		status=0, i=0, j=0;

	//header

	DebugLogRite("File Name :%s\n", FileName);


	//header
 	if(pStructRev)
		(*pStructRev) = GetPrivateProfileInt(TAG_STRUCTINFO, KEY_STRUCTURE_REV, 
                                (*pStructRev), FileName); 
 
	//Rf Design
 	if(pPcbRev_DesignType)
		(*pPcbRev_DesignType) = GetPrivateProfileInt(TAG_MAIN, KEY_DESIGN_TYPE, 
										(*pPcbRev_DesignType), FileName); 
 
 	if(pPcbRev_MajorRev)
		(*pPcbRev_MajorRev) = GetPrivateProfileInt(TAG_MAIN, KEY_MAJOR_VERSION, 
										(*pPcbRev_MajorRev), FileName); 
 
 	if(pPcbRev_MinorRev)
		(*pPcbRev_MinorRev) = GetPrivateProfileInt(TAG_MAIN, KEY_MINOR_VERSION, 
										(*pPcbRev_MinorRev), FileName); 

 	if(pPcbRev_Variation)
		(*pPcbRev_Variation) = GetPrivateProfileInt(TAG_MAIN, KEY_DESIGN_VARIATION, 
										(*pPcbRev_Variation), FileName); 

	
  	for (i=0; i<MAXNUM_MIMODEVICE; i++)
	{

 		sprintf(key, KEY_EXTLNA_POLORITY_G_FORMAT, i); 
		if(pExtLnaPolar_G)
			pExtLnaPolar_G[i] = GetPrivateProfileInt(TAG_MAIN, key, 
            pExtLnaPolar_G[i], FileName)?true:false; 

 		sprintf(key, KEY_EXTLNA_POLORITY_A_FORMAT, i); 
		if(pExtLnaPolar_A)
			pExtLnaPolar_A[i] = GetPrivateProfileInt(TAG_MAIN, key, 
            pExtLnaPolar_A[i], FileName)?true:false; 


 		sprintf(key, KEY_EXTLNA_G_FORMAT, i); 
		if(pExtLna_G)
			pExtLna_G[i] = GetPrivateProfileInt(TAG_MAIN, key, 
            pExtLna_G[i], FileName)?true:false; 

 		sprintf(key, KEY_EXTLNA_A_FORMAT, i); 
 		if(pExtLna_A)
			pExtLna_A[i] = GetPrivateProfileInt(TAG_MAIN, key, 
            pExtLna_A[i], FileName)?true:false; 

		sprintf(key, KEY_EXTPA_POLAR_G_FORMAT, i); 
  		if(pExtPaPolar_Neg_G)
			pExtPaPolar_Neg_G[i] = GetPrivateProfileInt(TAG_MAIN, key, 
            pExtPaPolar_Neg_G[i], FileName)?true:false; 
 
		sprintf(key, KEY_EXTPA_POLAR_A_FORMAT, i); 
   		if(pExtPaPolar_Neg_A)
			pExtPaPolar_Neg_A[i] = GetPrivateProfileInt(TAG_MAIN, key, 
            pExtPaPolar_Neg_A[i], FileName)?true:false; 
	} 

     //ant option
 	for (i=0; i<MAXNUM_MIMODEVICE; i++)
	{
		sprintf(key, KEY_ANT_TX_FORMAT, i); 
   		if(pAntTx)
			pAntTx[i] = GetPrivateProfileInt(TAG_MAIN, key, 
											pAntTx[i], FileName); 

  		sprintf(key, KEY_ANT_RX_FORMAT, i); 
   		if(pAntRx)
			pAntRx[i] = GetPrivateProfileInt(TAG_MAIN, key, 
											pAntRx[i], FileName); 

		
		for (j=0; j<MAXNUM_RXPATH; j++)
		{
 			sprintf(key, KEY_ANT_ID_FORMAT, i, j); 
    		if(pAnt_PartId)
				pAnt_PartId[i][j] = GetPrivateProfileInt(TAG_MAIN, key, 
											pAnt_PartId[i][j], FileName); 
		}
	}
	
	
	for (i=0; i<MAXNUM_MIMODEVICE; i++)
	{
 		sprintf(key, KEY_FEMID_FORMAT, i); 
   		if(pFemConfId)
			pFemConfId[i] = GetPrivateProfileInt(TAG_MAIN, key, 
											pFemConfId[i], FileName); 
	}
 
    if(pLnaGain)
		(*pLnaGain) = GetPrivateProfileInt(TAG_MAIN, KEY_LNA_GAIN, 
											(*pLnaGain), FileName); 
 

	GetPrivateProfileString(TAG_MAIN,KEY_SPI_SIZE, VAL_SPI_SIZE_DEF, value, 20, FileName); 
	{
		int tempInt=0; 
		sscanf(value, "%d%s", &tempInt, key);
		if(pSpiSizeInfo) (*pSpiSizeInfo) = tempInt;
		if(pSpiUnitInfo)
			if('M' == key[0] || 'm' == key[0]) 
				(*pSpiUnitInfo) = 1;
			else
				(*pSpiUnitInfo) = 0;
	}

   	if(pAssemblyHouse)
		(*pAssemblyHouse) = GetPrivateProfileInt(TAG_MAIN, KEY_ASSEMBLY_HOUSE, 
										(*pAssemblyHouse), FileName); 

   	if(pSN)
		(*pSN) = GetPrivateProfileInt(TAG_MAIN, KEY_SN, 
										(*pSN), FileName); 

	if(pCC)
		(*pCC) = GetPrivateProfileInt(TAG_MAIN, KEY_REGION_CODE, 
										(*pCC), FileName); 

	if(pExtXtalSource)
		(*pExtXtalSource) = GetPrivateProfileInt(TAG_MAIN, KEY_MISC_FLAG_CRYSTAL, 
										(*pExtXtalSource), FileName); 

	if(pExtSleepClk)
		(*pExtSleepClk) = GetPrivateProfileInt(TAG_MAIN, KEY_MISC_FLAG_SLEEPCLOCK, 
										(*pExtSleepClk), FileName); 

	if(pWlanWakeUp)
		(*pWlanWakeUp) = GetPrivateProfileInt(TAG_MAIN, KEY_MISC_FLAG_WLANWAKEUP, 
										(*pWlanWakeUp), FileName); 
 
  	 
 	if(pSocId)
		(*pSocId) = GetPrivateProfileInt(TAG_MAIN, KEY_SOC_ID, 
										(*pSocId), FileName); 

  	if(pSocVer)
		(*pSocVer) = GetPrivateProfileInt(TAG_MAIN, KEY_SOC_VERSION, 
										(*pSocVer), FileName); 

  	if(pBbuId)
		(*pBbuId) = GetPrivateProfileInt(TAG_MAIN, KEY_BBU_ID, 
										(*pBbuId), FileName); 

  	if(pBbuVers)
		(*pBbuVers) = GetPrivateProfileInt(TAG_MAIN, KEY_BBU_VERSION, 
										(*pBbuVers), FileName); 
	
  	if(pRfId)
		(*pRfId) = GetPrivateProfileInt(TAG_MAIN, KEY_RF_ID, 
										(*pRfId), FileName); 

  	if(pRfVer)
		(*pRfVer) = GetPrivateProfileInt(TAG_MAIN, KEY_RF_VERSION, 
										(*pRfVer), FileName); 

  	if(pTestToolVer)
		(*pTestToolVer) = GetPrivateProfileInt(TAG_MAIN, KEY_TEST_VERSION, 
										(*pTestToolVer), FileName); 

  	if(pMfgTaskVersion)
		(*pMfgTaskVersion) = GetPrivateProfileInt(TAG_MAIN, KEY_MFG_VERSION, 
										(*pMfgTaskVersion), FileName); 

  	if(pDllVersion)
		(*pDllVersion) = GetPrivateProfileInt(TAG_MAIN, KEY_DLL_VERSION, 
										(*pDllVersion), FileName); 

  	if(pBT_Coexist)
		(*pBT_Coexist) = GetPrivateProfileInt(TAG_MAIN, KEY_BT_COEXISTENCE, 
										(*pBT_Coexist), FileName); 

   	if(pAppModeFlag)
		(*pAppModeFlag) = GetPrivateProfileInt(TAG_MAIN, KEY_APPMODEFALG, 
										(*pAppModeFlag), FileName); 

   	if(pSocOrRev)
		(*pSocOrRev) = GetPrivateProfileInt(TAG_MAIN, KEY_SOC_OR_REV, 
										(*pSocOrRev), FileName); 
 
   	if(pRfuOrRev)
		(*pRfuOrRev) = GetPrivateProfileInt(TAG_MAIN, KEY_RFU_OR_REV, 
										(*pRfuOrRev), FileName); 

   	if(pMimoTxCnt)
		(*pMimoTxCnt) = GetPrivateProfileInt(TAG_MAIN, KEY_MIMO_CONFIG_CNTTX, 
										(*pMimoTxCnt), FileName); 
	
   	if(pMimoRxCnt)
		(*pMimoRxCnt) = GetPrivateProfileInt(TAG_MAIN, KEY_MIMO_CONFIG_CNTRX, 
										(*pMimoRxCnt), FileName); 

 

  	if(pMimoMap) 
	{
		for (i=0; i<MAXNUM_MIMODEVICE; i++)
		{
 			for (j=0; j<MAXNUM_RXPATH; j++)
			{
  				sprintf(key, KEY_MIMO_CONFIG_MAP_FORMAT, i, j); 
				pMimoMap[i][j] = GetPrivateProfileInt(TAG_MAIN, key, 
										pMimoMap[i][j], FileName); 
			}
		}
	}

	return status;
}


int DutSharedClssHeader WriteMainDataToFile(char *FileName,
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
						)
{
 	char	value[MAX_LENGTH]="";
	char	key[MAX_LENGTH]="";
	int		status=0, i=0, j=0;

	//header

	DebugLogRite("File Name :%s\n", FileName);

	//header

#ifdef _LINUX_
	//clear the file first
	{
	    FILE *hFileTemp=0;
		hFileTemp = fopen(FileName, "w");
		fclose (hFileTemp);
    }
#endif //#ifdef _LINUX_

	//header
	if(pStructRev) 
		sprintf(value, "0x%X", (*pStructRev));
	else 
		strcpy(value, "0x00");
	WritePrivateProfileString(TAG_STRUCTINFO, KEY_STRUCTURE_REV, value, FileName);

	//Rf Design

	if(pPcbRev_DesignType) 
		sprintf(value, "0x%X", (*pPcbRev_DesignType));
	else 
		strcpy(value, "0x00");
	WritePrivateProfileString(TAG_MAIN, KEY_DESIGN_TYPE, value, FileName);

	if(pPcbRev_MajorRev) 
		sprintf(value, "0x%X", (*pPcbRev_MajorRev));
	else 
		strcpy(value, "0x00");
	WritePrivateProfileString(TAG_MAIN, KEY_MAJOR_VERSION, value, FileName);

	if(pPcbRev_MinorRev) 
		sprintf(value, "0x%X", (*pPcbRev_MinorRev));
	else 
		strcpy(value, "0x00");
	WritePrivateProfileString(TAG_MAIN, KEY_MINOR_VERSION, value, FileName);
	
	if(pPcbRev_Variation) 
		sprintf(value, "0x%X", (*pPcbRev_Variation));
	else 
		strcpy(value, "0x00");
	WritePrivateProfileString(TAG_MAIN, KEY_DESIGN_VARIATION, value, FileName);
 	for (i=0; i<MAXNUM_MIMODEVICE; i++)
	{
		if(pExtLna_G) 
			sprintf(value, "0x%X", pExtLna_G[i]); 
 		else 
			strcpy(value, "0x00");
		sprintf(key, KEY_EXTLNA_G_FORMAT, i); 
		WritePrivateProfileString(TAG_MAIN, key, value, FileName); 

		if(pExtLna_A) 
			sprintf(value, "0x%X", pExtLna_A[i]); 
 		else 
			strcpy(value, "0x00");
 		sprintf(key, KEY_EXTLNA_A_FORMAT, i); 
		WritePrivateProfileString(TAG_MAIN, key, value, FileName); 
 
		if(pExtLnaPolar_G) 
			sprintf(value, "0x%X", pExtLnaPolar_G[i]); 
 		else 
			strcpy(value, "0x00");
		sprintf(key, KEY_EXTLNA_POLORITY_G_FORMAT, i); 
		WritePrivateProfileString(TAG_MAIN, key, value, FileName); 

		if(pExtLnaPolar_A) 
			sprintf(value, "0x%X", pExtLnaPolar_A[i]); 
 		else 
			strcpy(value, "0x00");
 		sprintf(key, KEY_EXTLNA_POLORITY_A_FORMAT, i); 
		WritePrivateProfileString(TAG_MAIN, key, value, FileName); 
 

		if(pExtPaPolar_Neg_G) 
			sprintf(value, "0x%X", pExtPaPolar_Neg_G[i]);
		else 
			strcpy(value, "0x00");
		sprintf(key, KEY_EXTPA_POLAR_G_FORMAT, i); 
		WritePrivateProfileString(TAG_MAIN, key, value, FileName);

		if(pExtPaPolar_Neg_A) 
			sprintf(value, "0x%X", pExtPaPolar_Neg_A[i]);
		else 
			strcpy(value, "0x00");
		sprintf(key, KEY_EXTPA_POLAR_A_FORMAT, i); 
		WritePrivateProfileString(TAG_MAIN, key, value, FileName);
	} 

     //ant option
 	for (i=0; i<MAXNUM_MIMODEVICE; i++)
	{
		if(pAntTx) 
			sprintf(value, "0x%X", pAntTx[i]); 
  		else 
			strcpy(value, "0x00");
		sprintf(key, KEY_ANT_TX_FORMAT, i); 
		WritePrivateProfileString(TAG_MAIN, key,value,FileName); 

		if(pAntRx) 
			sprintf(value, "0x%X", pAntRx[i]); 
  		else 
			strcpy(value, "0x00");
  		sprintf(key, KEY_ANT_RX_FORMAT, i); 
		WritePrivateProfileString(TAG_MAIN, key,value,FileName); 
		
		for (j=0; j<MAXNUM_RXPATH; j++)
		{
  			if(pAnt_PartId)
				sprintf(value, "0x%X", pAnt_PartId[i][j]); 
   			else 
				strcpy(value, "0x00");
 			sprintf(key, KEY_ANT_ID_FORMAT, i, j); 
			WritePrivateProfileString(TAG_MAIN, key,value,FileName); 

		}
	}
	
	
	for (i=0; i<MAXNUM_MIMODEVICE; i++)
	{
 			if(pFemConfId) 
				sprintf(value, "0x%X",pFemConfId[i]); 
			else
				strcpy(value, "0x00");

 			sprintf(key, KEY_FEMID_FORMAT, i); 
			WritePrivateProfileString(TAG_MAIN, key, value, FileName); 
	}
 
	if(pLnaGain) 
		sprintf(value, "%d", (*pLnaGain)); 
	else
		strcpy(value,"0x00");
	WritePrivateProfileString(TAG_MAIN, KEY_LNA_GAIN, value, FileName); 


	if(pSpiSizeInfo && pSpiUnitInfo) 
		sprintf(value, "%d%c", (*pSpiSizeInfo),  (*pSpiUnitInfo)?'M':'K' ); 
	else
		strcpy(value,"0x00");
	WritePrivateProfileString(TAG_MAIN, KEY_SPI_SIZE, value, FileName); 

	if(pAssemblyHouse)
		sprintf(value, "%d", (*pAssemblyHouse)); 
 	else
		strcpy(value,"0x00");
	WritePrivateProfileString(TAG_MAIN,KEY_ASSEMBLY_HOUSE, 
			value,FileName); 

	if(pSN) 
		sprintf(value, "%d", (*pSN)); 
 	else
		strcpy(value,"0x00");
	WritePrivateProfileString(TAG_MAIN,KEY_SN, value, FileName); 

	if(pCC)
		sprintf(value, "0x%X", (*pCC)); 
 	else
		strcpy(value,"0x00");
	WritePrivateProfileString(TAG_MAIN,KEY_REGION_CODE,value,FileName); 


	if(pExtXtalSource) 
		sprintf(value, "0x%X", (*pExtXtalSource)); 
 	else
		strcpy(value,"0x00");
	WritePrivateProfileString(TAG_MAIN,KEY_MISC_FLAG_CRYSTAL,value,FileName); 

  	if(pExtSleepClk) 
		sprintf(value, "0x%X", (*pExtSleepClk)); 
 	else
		strcpy(value, "0x00");
	WritePrivateProfileString(TAG_MAIN,KEY_MISC_FLAG_SLEEPCLOCK,value,FileName); 
  
	if(pWlanWakeUp) 
		sprintf(value, "0x%X", (*pWlanWakeUp)); 
 	else
		strcpy(value, "0x00");
	WritePrivateProfileString(TAG_MAIN,KEY_MISC_FLAG_WLANWAKEUP,value,FileName); 

	if(pSocId)
		sprintf(value, "0x%X", (*pSocId)); 
 	else
		strcpy(value,"0x00");
	WritePrivateProfileString(TAG_MAIN,KEY_SOC_ID, value, FileName); 

	if(pSocVer) 
		sprintf(value, "0x%X", (*pSocVer)); 
 	else
		strcpy(value, "0x00");
	WritePrivateProfileString(TAG_MAIN,KEY_SOC_VERSION, value, FileName); 

	if(pBbuId) 
		sprintf(value, "0x%X", (*pBbuId)); 
 	else
		strcpy(value, "0x00");
	WritePrivateProfileString(TAG_MAIN,KEY_BBU_ID, value, FileName);
	
	if(pBbuVers) 
		sprintf(value, "0x%X", (*pBbuVers)); 
 	else
		strcpy(value,"0x00");
	WritePrivateProfileString(TAG_MAIN,KEY_BBU_VERSION, value, FileName); 

	if(pRfId)
		sprintf(value, "0x%X", (*pRfId)); 
 	else
		strcpy(value,"0x00");
	WritePrivateProfileString(TAG_MAIN,	KEY_RF_ID,value, FileName); 

	if(pRfVer)
		sprintf(value, "0x%X", (*pRfVer)); 
 	else
		strcpy(value,"0x00");
	WritePrivateProfileString(TAG_MAIN,	KEY_RF_VERSION,value, FileName); 

	if(pTestToolVer)
		sprintf(value, "0x%04X", (*pTestToolVer)); 
 	else
		strcpy(value,"0x00");
	WritePrivateProfileString(TAG_MAIN,KEY_TEST_VERSION, value, FileName); 

 	if(pMfgTaskVersion)
		sprintf(value, "0x%04X", (*pMfgTaskVersion)); 
 	else
		strcpy(value,"0x00");
	WritePrivateProfileString(TAG_MAIN,KEY_MFG_VERSION, value, FileName); 

  	if(pDllVersion)
		sprintf(value, "0x%04X", (*pDllVersion)); 
 	else
		strcpy(value,"0x00");
	WritePrivateProfileString(TAG_MAIN,KEY_DLL_VERSION,	value, FileName); 

 
   	if(pBT_Coexist)
		sprintf(value, "0x%X", (*pBT_Coexist)); 
  	else 
		strcpy(value, "0x00");
	WritePrivateProfileString(TAG_MAIN, KEY_BT_COEXISTENCE, value,FileName);  

	if(pAppModeFlag)
		sprintf(value, "0x%X", (*pAppModeFlag)); 
 	else
		strcpy(value,"0x00");
	WritePrivateProfileString(TAG_MAIN, KEY_APPMODEFALG, value,FileName);  

	if(pSocOrRev)
		sprintf(value, "0x%X", (*pSocOrRev)); 
 	else
		strcpy(value,"0x00");
	WritePrivateProfileString(TAG_MAIN, KEY_SOC_OR_REV, value,FileName); 
	
	if(pRfuOrRev)
		sprintf(value, "0x%X", (*pRfuOrRev)); 
 	else
		strcpy(value,"0x00");
	WritePrivateProfileString(TAG_MAIN, KEY_RFU_OR_REV, value,FileName);  

	if(pMimoTxCnt)
   	sprintf(value, "0x%X", (*pMimoTxCnt)); 
 	else
		strcpy(value,"0x00");
	WritePrivateProfileString(TAG_MAIN, KEY_MIMO_CONFIG_CNTTX, value,FileName);  
 
	if(pMimoRxCnt)
		sprintf(value, "0x%X", (*pMimoRxCnt)); 
 	else
		strcpy(value,"0x00");
	WritePrivateProfileString(TAG_MAIN, KEY_MIMO_CONFIG_CNTRX, value,FileName);  

  	if(pMimoMap) 
	{
		for (i=0; i<MAXNUM_MIMODEVICE; i++)
		{
 			for (j=0; j<MAXNUM_RXPATH; j++)
			{
		
				sprintf(value, "0x%X", pMimoMap[i][j]); 
  				sprintf(key, KEY_MIMO_CONFIG_MAP_FORMAT, i, j); 
				WritePrivateProfileString(TAG_MAIN, key, value,FileName);  
			}
		}
	}
	else
	{
		for (i=0; i<MAXNUM_MIMODEVICE; i++)
		{
 			for (j=0; j<MAXNUM_RXPATH; j++)
			{
		
				sprintf(value, "0x%02X", 0x00); 
  				sprintf(key, KEY_MIMO_CONFIG_MAP_FORMAT, i, j); 
				WritePrivateProfileString(TAG_MAIN, key, value,FileName);  
			}
		}

	}
	return status;
}

int DutSharedClssHeader ReadAnnex27DataFromFile(char *FileName,BYTE *pLED)
{
	char value[MAX_LENGTH]="";
	char key[MAX_LENGTH]="";
	char tag[MAX_LENGTH]="";
	int status=0, i=0;

	//header

	DebugLogRite("File Name :%s\n", FileName);
  	sprintf(tag, TAG_ANNEX27); 
 	if (!GetPrivateProfileSection(tag, value, MAX_LENGTH, FileName))
		return ERROR_DATANOTEXIST;

	for (i=0; i<MAX_LED; i++)
	{
		sprintf(key, KEY_SETPAIRS_ADDRESS_FORMAT, i); 
		if(pLED)
			pLED[i] = GetPrivateProfileInt(tag, key, 
										pLED[i], FileName); 
	}

	return status;

}

int DutSharedClssHeader WriteAnnex27DataToFile(char *FileName,BYTE *pLED)
{
	char value[MAX_LENGTH]="";
	char key[MAX_LENGTH]="";
	char tag[MAX_LENGTH]="";
	int status=0, i=0;

	//header

	DebugLogRite("File Name :%s\n", FileName);
  	sprintf(tag, TAG_ANNEX27); 
  	for (i=0; i<MAX_LED; i++)
	{
  		if(pLED)
			sprintf(value, "0x%X", pLED[i]);
		else
			sprintf(value, "0xFF");

		sprintf(key, KEY_SETPAIRS_ADDRESS_FORMAT, i); 
 		WritePrivateProfileString(tag,	key,	value, FileName);
	}

	return status;

}


int DutSharedClssHeader ReadAnnex28DataFromFile(char *FileName, 
								   BYTE *pUsbVendorString, 
								   BYTE *pUsbProductString)
{
	char value[MAX_LENGTH]="";
	char key[MAX_LENGTH]="";
	char tag[MAX_LENGTH]="";
	int status=0, i=0;

	//header

	DebugLogRite("File Name :%s\n", FileName);
  	sprintf(tag, TAG_ANNEX28); 
  	if (!GetPrivateProfileSection(tag, value, MAX_LENGTH, FileName))
		return ERROR_DATANOTEXIST;

	for (i=0; i<MAX_LED; i++)
	{
		sprintf(key, KEY_VENDORSTRING); 
   		if(pUsbVendorString)
			GetPrivateProfileString( tag,	key, 
										"", (char*)pUsbVendorString, 
										MAX_LENGTH,  FileName); 
	
		sprintf(key, KEY_PRODUCTSTRING); 
   		if(pUsbProductString)
			GetPrivateProfileString( tag,	key, 
										"", (char*)pUsbProductString, 
										MAX_LENGTH,  FileName); 
	}

	return status;

}



int DutSharedClssHeader WriteAnnex28DataToFile(char *FileName, 
								   BYTE *pUsbVendorString, 
								   BYTE *pUsbProductString)
{
	char value[MAX_LENGTH]="";
	char key[MAX_LENGTH]="";
	char tag[MAX_LENGTH]="";
	int status=0, i=0;

	//header

	DebugLogRite("File Name :%s\n", FileName);
  	sprintf(tag, TAG_ANNEX28); 
  	for (i=0; i<MAX_LED; i++)
	{
  		if(pUsbVendorString)
			sprintf(value, "%s ", pUsbVendorString);
		else
			sprintf(value, "");
		sprintf(key, KEY_VENDORSTRING); 
 		WritePrivateProfileString(tag,	key,	value, FileName);

  		if(pUsbProductString)
			sprintf(value, "%s ", pUsbProductString);
		else
			sprintf(value, "");
		sprintf(key, KEY_PRODUCTSTRING); 
 		WritePrivateProfileString(tag,	key,	value, FileName);
	}

	return status;

}

int DutSharedClssHeader ReadAnnex29DataFromFile(char *FileName, 
							int *pBandSupport2_4,  
							int *pBandSupport4_9, 
							int *pBandSupport5_0,  
							int *pBandSupport5_2)
{
	int  tempInt=0, i=0, j=0; 
	char value[256]="", tag[256]="", key[256]="";
	int status=0;

	DebugLogRite("File Name :%s\n", FileName);
  	sprintf(tag, TAG_ANNEX29); 
  	if (!GetPrivateProfileSection(tag, value, MAX_LENGTH, FileName))
		return ERROR_DATANOTEXIST;

	if(pBandSupport2_4)
		(*pBandSupport2_4) = GetPrivateProfileInt(tag,KEY_BAND_2_4G, (*pBandSupport2_4), FileName);  
  	if(pBandSupport4_9)
		(*pBandSupport4_9) = GetPrivateProfileInt(tag,KEY_BAND_4_9G, (*pBandSupport4_9), FileName);  
  	if(pBandSupport5_0) 
		(*pBandSupport5_0) = GetPrivateProfileInt(tag,KEY_BAND_5_0G, (*pBandSupport5_0), FileName);  
  	if(pBandSupport5_2)
		(*pBandSupport5_2) = GetPrivateProfileInt(tag,KEY_BAND_5_2G, (*pBandSupport5_2), FileName);  
	
	return status;
}
int DutSharedClssHeader WriteAnnex29DataToFile(char *FileName, 
							int *pBandSupport2_4,  
							int *pBandSupport4_9, 
							int *pBandSupport5_0,  
							int *pBandSupport5_2)
{
	int  tempInt=0, i=0, j=0; 
	char value[256]="", tag[256]="", key[256]="";
	int status=0;


 	sprintf(tag, TAG_ANNEX29); 

   	if(pBandSupport2_4)
	{
			sprintf(value, "%d", (*pBandSupport2_4));
  			WritePrivateProfileString(tag,	KEY_BAND_2_4G,	value, FileName);
	}
   	if(pBandSupport4_9)
	{
			sprintf(value, "%d", *pBandSupport4_9);
  			WritePrivateProfileString(tag,	KEY_BAND_4_9G,	value, FileName);
	}
    if(pBandSupport5_0)
	{
  		sprintf(value, "%d", *pBandSupport5_0);
  		WritePrivateProfileString(tag,	KEY_BAND_5_0G,	value, FileName);
	}
	if(pBandSupport5_2)
	{
   		sprintf(value, "%d", *pBandSupport5_2);
  		WritePrivateProfileString(tag,	KEY_BAND_5_2G,	value, FileName);
	}
 
	return status;
}



int DutSharedClssHeader ReadAnnex31DataFromFile(char *FileName,
#if defined (_MIMO_)
						   int DeviceId, //int PathId, int BandId,
#endif	// #if defined (_MIMO_)
						char pRssi_Nf[NUM_SUBBAND_G], 
						char pRssi_cck[NUM_SUBBAND_G], 
						char pRssi_ofdm[NUM_SUBBAND_G], 
						char pRssi_mcs[NUM_SUBBAND_G])
{
 
 	char value[MAX_LENGTH]="";
	char key[MAX_LENGTH]="";
	char tag[MAX_LENGTH]="";
	int status=0, i=0;

	//header

	DebugLogRite("File Name :%s\n", FileName);
#if defined (_MIMO_)
 	sprintf(tag, TAG_ANNEX31_FORMAT, DeviceId); 
#else  //#if defined (_MIMO_)
 	sprintf(tag, TAG_ANNEX31); 
#endif //#if defined (_MIMO_)

  	if (!GetPrivateProfileSection(tag, value, MAX_LENGTH, FileName))
		return ERROR_DATANOTEXIST;
 
 
		for (i=0; i<NUM_SUBBAND_G; i++)
		{
			//sprintf(key, KEY_RSSI_NF_FORMAT, i); 
			sprintf(key, KEY_RSSI_CCK_BYPASS_FORMAT, i); 
 			if(pRssi_Nf)
				pRssi_Nf[i] = GetPrivateProfileInt(tag, key, 
												pRssi_Nf[i], FileName); 
				
			//sprintf(key, KEY_RSSI_CCK_FORMAT, i); 
			sprintf(key, KEY_RSSI_CCK_NORMAL_FORMAT, i);
  			if(pRssi_cck)
				pRssi_cck[i] = GetPrivateProfileInt(tag, key, 
												pRssi_cck[i], FileName); 

			//sprintf(key, KEY_RSSI_OFMD_FORMAT, i); 
			sprintf(key, KEY_RSSI_OFDM_BYPASS_FORMAT, i);
  			if(pRssi_ofdm)
				pRssi_ofdm[i] = GetPrivateProfileInt(tag, key, 
												pRssi_ofdm[i], FileName); 

			//sprintf(key, KEY_RSSI_MCS_FORMAT, i); 
			sprintf(key, KEY_RSSI_OFDM_NORMAL_FORMAT, i); 
  			if(pRssi_mcs)
				pRssi_mcs[i] = GetPrivateProfileInt(tag, key, 
												pRssi_mcs[i], FileName); 
		}
	
	
	return status;

}


int DutSharedClssHeader WriteAnnex31DataToFile(char *FileName,
#if defined (_MIMO_)
						   int DeviceId, //int PathId, int BandId,
#endif	// #if defined (_MIMO_)
						char pRssi_Nf[NUM_SUBBAND_G], 
						char pRssi_cck[NUM_SUBBAND_G], 
						char pRssi_ofdm[NUM_SUBBAND_G], 
						char pRssi_mcs[NUM_SUBBAND_G])
{
 
 	char value[MAX_LENGTH]="";
	char key[MAX_LENGTH]="";
	char tag[MAX_LENGTH]="";
	int status=0, i=0;

	//header

	DebugLogRite("File Name :%s\n", FileName);
#if defined (_MIMO_)
 	sprintf(tag, TAG_ANNEX31_FORMAT, DeviceId); 
#else  //#if defined (_MIMO_)
 	sprintf(tag, TAG_ANNEX31); 
#endif //#if defined (_MIMO_)

 
 
		for (i=0; i<NUM_SUBBAND_G; i++)
		{
			if(pRssi_Nf)
				sprintf(value, "0x%X", pRssi_Nf[i]); 
			else
				sprintf(value, "0x00");
			//sprintf(key, KEY_RSSI_NF_FORMAT, i); 
			sprintf(key, KEY_RSSI_CCK_BYPASS_FORMAT, i);
 			WritePrivateProfileString(tag,key, value, FileName);  

			if(pRssi_cck)
				sprintf(value, "0x%X", pRssi_cck[i]); 
			else
				sprintf(value, "0x00");
			//sprintf(key, KEY_RSSI_CCK_FORMAT, i); 
			sprintf(key, KEY_RSSI_CCK_NORMAL_FORMAT, i);
 			WritePrivateProfileString(tag,key, value, FileName); 

			if(pRssi_ofdm)
				sprintf(value, "0x%X", pRssi_ofdm[i]); 
			else
				sprintf(value, "0x00");
			//sprintf(key, KEY_RSSI_OFMD_FORMAT, i); 
			sprintf(key, KEY_RSSI_OFDM_BYPASS_FORMAT, i);
 			WritePrivateProfileString(tag,key, value, FileName);  

			if(pRssi_mcs)
				sprintf(value, "0x%X", pRssi_mcs[i]); 
			else
				sprintf(value, "0x00");
			//sprintf(key, KEY_RSSI_MCS_FORMAT, i); 
			sprintf(key, KEY_RSSI_OFDM_NORMAL_FORMAT, i);
 			WritePrivateProfileString(tag,key, value, FileName);  
		}
	
	
	return status;

}




int DutSharedClssHeader ReadAnnex32DataFromFile(char *FileName,
#if defined (_MIMO_)
					    int DeviceId, 
#endif	// #if defined (_MIMO_)
						char pRssi_Nf[NUM_SUBBAND_A], 
						char pRssi_cck[NUM_SUBBAND_A], 
						char pRssi_ofdm[NUM_SUBBAND_A], 
						char pRssi_mcs[NUM_SUBBAND_A])
{
 
 	char value[MAX_LENGTH]="";
	char key[MAX_LENGTH]="";
	char tag[MAX_LENGTH]="";
	int status=0, i=0;

	//header

	DebugLogRite("File Name :%s\n", FileName);
#if defined (_MIMO_)
 	sprintf(tag, TAG_ANNEX32_FORMAT, DeviceId); 
#else  //#if defined (_MIMO_)
 	sprintf(tag, TAG_ANNEX32); 
#endif //#if defined (_MIMO_)

 
   	if (!GetPrivateProfileSection(tag, value, MAX_LENGTH, FileName))
		return ERROR_DATANOTEXIST;

		for (i=0; i<NUM_SUBBAND_A; i++)
		{
			//sprintf(key, KEY_RSSI_NF_FORMAT, i); 
			sprintf(key, KEY_RSSI_CCK_BYPASS_FORMAT, i);
 			if(pRssi_Nf)
				pRssi_Nf[i] = GetPrivateProfileInt(tag, key, 
												pRssi_Nf[i], FileName); 
				
			//sprintf(key, KEY_RSSI_CCK_FORMAT, i); 
			sprintf(key, KEY_RSSI_CCK_NORMAL_FORMAT, i);
  			if(pRssi_cck)
				pRssi_cck[i] = GetPrivateProfileInt(tag, key, 
												pRssi_cck[i], FileName); 

			//sprintf(key, KEY_RSSI_OFMD_FORMAT, i); 
			sprintf(key, KEY_RSSI_OFDM_BYPASS_FORMAT, i);
  			if(pRssi_ofdm)
				pRssi_ofdm[i] = GetPrivateProfileInt(tag, key, 
												pRssi_ofdm[i], FileName); 

			//sprintf(key, KEY_RSSI_MCS_FORMAT, i); 
			sprintf(key, KEY_RSSI_OFDM_NORMAL_FORMAT, i);
  			if(pRssi_mcs)
				pRssi_mcs[i] = GetPrivateProfileInt(tag, key, 
												pRssi_mcs[i], FileName); 		    
		}
	
	
	return status;

}


int DutSharedClssHeader WriteAnnex32DataToFile(char *FileName,
#if defined (_MIMO_)
						int DeviceId, 
#endif	// #if defined (_MIMO_)
						char pRssi_Nf[NUM_SUBBAND_A], 
						char pRssi_cck[NUM_SUBBAND_A], 
						char pRssi_ofdm[NUM_SUBBAND_A], 
						char pRssi_mcs[NUM_SUBBAND_A])
{
 
 	char value[MAX_LENGTH]="";
	char key[MAX_LENGTH]="";
	char tag[MAX_LENGTH]="";
	int status=0, i=0;

	//header

	DebugLogRite("File Name :%s\n", FileName);
#if defined (_MIMO_)
 	sprintf(tag, TAG_ANNEX32_FORMAT, DeviceId); 
#else  //#if defined (_MIMO_)
 	sprintf(tag, TAG_ANNEX32); 
#endif //#if defined (_MIMO_)
 
	for (i=0; i<NUM_SUBBAND_A; i++)
	{
		if(pRssi_Nf)
			sprintf(value, "0x%X", pRssi_Nf[i]); 
		else
			sprintf(value, "0x00");
		//sprintf(key, KEY_RSSI_NF_FORMAT, i); 
		sprintf(key, KEY_RSSI_CCK_BYPASS_FORMAT, i);
		WritePrivateProfileString(tag,key, value, FileName);  

		if(pRssi_cck)
			sprintf(value, "0x%X", pRssi_cck[i]); 
		else
			sprintf(value, "0x00");
		//sprintf(key, KEY_RSSI_CCK_FORMAT, i); 
		sprintf(key, KEY_RSSI_CCK_NORMAL_FORMAT, i);
		WritePrivateProfileString(tag,key, value, FileName); 

		if(pRssi_ofdm)
			sprintf(value, "0x%X", pRssi_ofdm[i]); 
		else
			sprintf(value, "0x00");
		//sprintf(key, KEY_RSSI_OFMD_FORMAT, i); 
		sprintf(key, KEY_RSSI_OFDM_BYPASS_FORMAT, i);
		WritePrivateProfileString(tag,key, value, FileName);  

		if(pRssi_mcs)
			sprintf(value, "0x%X", pRssi_mcs[i]); 
		else
			sprintf(value, "0x00");
		//sprintf(key, KEY_RSSI_MCS_FORMAT, i); 
		sprintf(key, KEY_RSSI_OFDM_NORMAL_FORMAT, i);
		WritePrivateProfileString(tag,key, value, FileName);  
	}
	
	return status;
}

#if defined (_MIMO_)
int DutSharedClssHeader ReadAnnex47DataFromFile(char *FileName,
 						   int DeviceId, 
						   int PathId,
 						char pRssi_Nf[NUM_SUBBAND_G], 
						char pRssi_cck[NUM_SUBBAND_G], 
						char pRssi_ofdm[NUM_SUBBAND_G], 
						char pRssi_mcs[NUM_SUBBAND_G])
{
 	char value[MAX_LENGTH]="";
	char key[MAX_LENGTH]="";
	char tag[MAX_LENGTH]="";
	int status=0, i=0;

	//header
	DebugLogRite("File Name :%s\n", FileName);
 
 	sprintf(tag, TAG_ANNEX47_FORMAT, DeviceId, PathId); 

   	if (!GetPrivateProfileSection(tag, value, MAX_LENGTH, FileName))
		return ERROR_DATANOTEXIST;

	for (i=0; i<NUM_SUBBAND_G; i++)
		{
 			//sprintf(key, KEY_RSSI_NF_FORMAT, i); 
			sprintf(key, KEY_RSSI_CCK_BYPASS_FORMAT, i);
 			if(pRssi_Nf)
				pRssi_Nf[i] = GetPrivateProfileInt(tag, key, 
												pRssi_Nf[i], FileName); 
				
			//sprintf(key, KEY_RSSI_CCK_FORMAT, i); 
			sprintf(key, KEY_RSSI_CCK_NORMAL_FORMAT, i);
  			if(pRssi_cck)
				pRssi_cck[i] = GetPrivateProfileInt(tag, key, 
												pRssi_cck[i], FileName); 

			//sprintf(key, KEY_RSSI_OFMD_FORMAT, i); 
			sprintf(key, KEY_RSSI_OFDM_BYPASS_FORMAT, i);
  			if(pRssi_ofdm)
				pRssi_ofdm[i] = GetPrivateProfileInt(tag, key, 
												pRssi_ofdm[i], FileName); 

			//sprintf(key, KEY_RSSI_MCS_FORMAT, i); 
			sprintf(key, KEY_RSSI_OFDM_NORMAL_FORMAT, i);
  			if(pRssi_mcs)
				pRssi_mcs[i] = GetPrivateProfileInt(tag, key, 
												pRssi_mcs[i], FileName); 		    
		}
	
	
	return status;

}


int DutSharedClssHeader WriteAnnex47DataToFile(char *FileName,
 						   int DeviceId, 
						   int PathId, //int BandId,
 						char pRssi_Nf[NUM_SUBBAND_G], 
						char pRssi_cck[NUM_SUBBAND_G], 
						char pRssi_ofdm[NUM_SUBBAND_G], 
						char pRssi_mcs[NUM_SUBBAND_G])
{
 
 	char value[MAX_LENGTH]="";
	char key[MAX_LENGTH]="";
	char tag[MAX_LENGTH]="";
	int status=0, i=0;

	//header

	DebugLogRite("File Name :%s\n", FileName);
 
 	sprintf(tag, TAG_ANNEX47_FORMAT, DeviceId, PathId); 

 		for (i=0; i<NUM_SUBBAND_G; i++)
		{
			if(pRssi_Nf)
				sprintf(value, "0x%X", pRssi_Nf[i]); 
			else
				sprintf(value, "0x00");
			//sprintf(key, KEY_RSSI_NF_FORMAT, i); 
			sprintf(key, KEY_RSSI_CCK_BYPASS_FORMAT, i);
 			WritePrivateProfileString(tag,key, value, FileName);  

			if(pRssi_cck)
				sprintf(value, "0x%X", pRssi_cck[i]); 
			else
				sprintf(value, "0x00");
			//sprintf(key, KEY_RSSI_CCK_FORMAT, i); 
			sprintf(key, KEY_RSSI_CCK_NORMAL_FORMAT, i);
 			WritePrivateProfileString(tag,key, value, FileName); 

			if(pRssi_ofdm)
				sprintf(value, "0x%X", pRssi_ofdm[i]); 
			else
				sprintf(value, "0x00");
			//sprintf(key, KEY_RSSI_OFMD_FORMAT, i); 
			sprintf(key, KEY_RSSI_OFDM_BYPASS_FORMAT, i);
 			WritePrivateProfileString(tag,key, value, FileName);  

			if(pRssi_mcs)
				sprintf(value, "0x%X", pRssi_mcs[i]); 
			else
				sprintf(value, "0x00");
			//sprintf(key, KEY_RSSI_MCS_FORMAT, i); 
			sprintf(key, KEY_RSSI_OFDM_NORMAL_FORMAT, i);
 			WritePrivateProfileString(tag,key, value, FileName);  
		}
	
	
	return status;

}



int DutSharedClssHeader ReadAnnex48DataFromFile(char *FileName,
						   int DeviceId, 
						   int PathId,  //int BandId,
						char pRssi_Nf[NUM_SUBBAND_A], 
						char pRssi_cck[NUM_SUBBAND_A], 
						char pRssi_ofdm[NUM_SUBBAND_A], 
						char pRssi_mcs[NUM_SUBBAND_A])
{
 
 	char value[MAX_LENGTH]="";
	char key[MAX_LENGTH]="";
	char tag[MAX_LENGTH]="";
	int status=0, i=0;

	//header

	DebugLogRite("File Name :%s\n", FileName);
 	sprintf(tag, TAG_ANNEX48_FORMAT, DeviceId, PathId); 
 
  	if (!GetPrivateProfileSection(tag, value, MAX_LENGTH, FileName))
		return ERROR_DATANOTEXIST;

	for (i=0; i<NUM_SUBBAND_A; i++)
	{
 		//sprintf(key, KEY_RSSI_NF_FORMAT, i); 
		sprintf(key, KEY_RSSI_CCK_BYPASS_FORMAT, i);
 		if(pRssi_Nf)
			pRssi_Nf[i] = GetPrivateProfileInt(tag, key, 
											pRssi_Nf[i], FileName); 
			
		//sprintf(key, KEY_RSSI_CCK_FORMAT, i); 
		sprintf(key, KEY_RSSI_CCK_NORMAL_FORMAT, i);
  		if(pRssi_cck)
			pRssi_cck[i] = GetPrivateProfileInt(tag, key, 
											pRssi_cck[i], FileName); 

		//sprintf(key, KEY_RSSI_OFMD_FORMAT, i); 
		sprintf(key, KEY_RSSI_OFDM_BYPASS_FORMAT, i);
 		if(pRssi_ofdm)
			pRssi_ofdm[i] = GetPrivateProfileInt(tag, key, 
												pRssi_ofdm[i], FileName); 

		//sprintf(key, KEY_RSSI_MCS_FORMAT, i); 
		sprintf(key, KEY_RSSI_OFDM_NORMAL_FORMAT, i);
  		if(pRssi_mcs)
			pRssi_mcs[i] = GetPrivateProfileInt(tag, key, 
												pRssi_mcs[i], FileName); 		    
	}
	
	
	return status;

}


int DutSharedClssHeader WriteAnnex48DataToFile(char *FileName,
						   int DeviceId, 
						   int PathId,  //int BandId,
						char pRssi_Nf[NUM_SUBBAND_A], 
						char pRssi_cck[NUM_SUBBAND_A], 
						char pRssi_ofdm[NUM_SUBBAND_A], 
						char pRssi_mcs[NUM_SUBBAND_A])
{
 
 	char value[MAX_LENGTH]="";
	char key[MAX_LENGTH]="";
	char tag[MAX_LENGTH]="";
	int status=0, i=0;

	//header

	DebugLogRite("File Name :%s\n", FileName);
 	sprintf(tag, TAG_ANNEX48_FORMAT, DeviceId, PathId); 
 
 
		for (i=0; i<NUM_SUBBAND_A; i++)
		{
			if(pRssi_Nf)
				sprintf(value, "0x%X", pRssi_Nf[i]); 
			else
				sprintf(value, "0x00");
			//sprintf(key, KEY_RSSI_NF_FORMAT, i); 
			sprintf(key, KEY_RSSI_CCK_BYPASS_FORMAT, i);
 			WritePrivateProfileString(tag,key, value, FileName);  

			if(pRssi_cck)
				sprintf(value, "0x%X", pRssi_cck[i]); 
			else
				sprintf(value, "0x00");
			//sprintf(key, KEY_RSSI_CCK_FORMAT, i); 
			sprintf(key, KEY_RSSI_CCK_NORMAL_FORMAT, i);
 			WritePrivateProfileString(tag,key, value, FileName); 

			if(pRssi_ofdm)
				sprintf(value, "0x%X", pRssi_ofdm[i]); 
			else
				sprintf(value, "0x00");
			//sprintf(key, KEY_RSSI_OFMD_FORMAT, i); 
			sprintf(key, KEY_RSSI_OFDM_BYPASS_FORMAT, i);
 			WritePrivateProfileString(tag,key, value, FileName);  

			if(pRssi_mcs)
				sprintf(value, "0x%X", pRssi_mcs[i]); 
			else
				sprintf(value, "0x00");
			//sprintf(key, KEY_RSSI_MCS_FORMAT, i); 
			sprintf(key, KEY_RSSI_OFDM_NORMAL_FORMAT, i);
 			WritePrivateProfileString(tag,key, value, FileName);  
		}
	
	
	return status;

}
#endif //#if defined (_MIMO_)

int DutSharedClssHeader ReadAnnex40DataFromFile(char *FileName,
						WORD *pVid,		WORD *pPid,	
						BYTE *pChipId,	BYTE *pChipVersion,					   
						BYTE *pFlag_SettingPrev, 
						BYTE *pNumOfRailAvaiableInChip,
						BYTE *pPowerDownMode,
						BYTE pNormalMode[MAX_PM_RAIL_PW886], 
						BYTE pSleepModeFlag[MAX_PM_RAIL_PW886],
						BYTE pSleepModeSetting[MAX_PM_RAIL_PW886],
					   BYTE *pDpSlp)
{
 	char value[MAX_LENGTH]="";
	char key[MAX_LENGTH]="";
	char tag[MAX_LENGTH]="";
	int status=0, i=0;
 
	//header

	DebugLogRite("File Name :%s\n", FileName);
  	sprintf(tag, TAG_ANNEX40); 
 
  	if (!GetPrivateProfileSection(tag, value, MAX_LENGTH, FileName))
		return ERROR_DATANOTEXIST;
	//header
 		
 	if(pVid)
		(*pVid) = GetPrivateProfileInt(tag, KEY_VID,
										(*pVid), FileName); 

 	if(pPid)
		(*pPid) = GetPrivateProfileInt(tag, KEY_PID,
										(*pPid), FileName); 

 	if(pChipId)
		(*pChipId) = GetPrivateProfileInt(tag, KEY_CHIPID,
										(*pChipId), FileName); 

 	if(pChipVersion)
		(*pChipVersion) = GetPrivateProfileInt(tag, KEY_CHIPVERSION,
										(*pChipVersion), FileName); 

 	if(pFlag_SettingPrev)
		(*pFlag_SettingPrev) = GetPrivateProfileInt(tag, KEY_SETTINGROLLBACK,
										(*pFlag_SettingPrev), FileName); 
 
 	if(pNumOfRailAvaiableInChip)
		(*pNumOfRailAvaiableInChip) = GetPrivateProfileInt(tag, KEY_RAIL_AVAIL_IN_CHIP,
										(*pNumOfRailAvaiableInChip), FileName); 
 
 	if(pDpSlp)
		(*pDpSlp) = GetPrivateProfileInt(tag, KEY_DPDLP_BYTE,
										(*pDpSlp), FileName); 
   	
	if(pPowerDownMode)
			(*pPowerDownMode) = GetPrivateProfileInt(tag, KEY_PWRDN_BYTE,
											(*pPowerDownMode), FileName); 
	for(i=0; i<MAX_PM_RAIL_PW886; i++)
	{

  		
		sprintf(key, KEY_NORMALMODE_VOLT_FORMAT, i);
  		if(pNormalMode)
			pNormalMode[i] = GetPrivateProfileInt(tag, key,
											pNormalMode[i], FileName); 

		sprintf(key, KEY_SLEEPMODE_FLAG_FORMAT, i);
  		if(pSleepModeFlag)
			pSleepModeFlag[i] = GetPrivateProfileInt(tag, key,
											pSleepModeFlag[i], FileName); 

		sprintf(key, KEY_SLEEPMODE_VOLT_FORMAT, i);
  		if(pSleepModeSetting)
			pSleepModeSetting[i] = GetPrivateProfileInt(tag, key,
											pSleepModeSetting[i], FileName); 
	}
	
	return status;
}


int DutSharedClssHeader WriteAnnex40DataToFile(char *FileName,
						WORD *pVid,		WORD *pPid,	
						BYTE *pChipId,	BYTE *pChipVersion,					   
						BYTE *pFlag_SettingPrev, 
						BYTE *pNumOfRailAvaiableInChip,
						BYTE *pPowerDownMode,
						BYTE pNormalMode[MAX_PM_RAIL_PW886], 
						BYTE pSleepModeFlag[MAX_PM_RAIL_PW886],
						BYTE pSleepModeSetting[MAX_PM_RAIL_PW886],
					   BYTE *pDpSlp)
{
 	char value[MAX_LENGTH]="";
	char key[MAX_LENGTH]="";
	char tag[MAX_LENGTH]="";
	int status=0, i=0;
 
	//header

	DebugLogRite("File Name :%s\n", FileName);
  	sprintf(tag, TAG_ANNEX40); 
 
	//header
 		
	if(pVid)
		sprintf(value, "0x%X", (*pVid)); 
	else
		sprintf(value, "0x00");
	WritePrivateProfileString(tag, KEY_VID, value,  FileName); 

	if(pPid)
		sprintf(value, "0x%X", (*pPid)); 
	else
		sprintf(value, "0x00");
	WritePrivateProfileString(tag, KEY_PID, value,  FileName); 

	if(pChipId)
		sprintf(value, "0x%X", (*pChipId)); 
 	else
		sprintf(value, "0x00");
	WritePrivateProfileString(tag, KEY_CHIPID, value,  FileName); 

	if(pChipVersion)
		sprintf(value, "0x%X",  (*pChipVersion)); 
	else
		sprintf(value, "0x00");
	WritePrivateProfileString(tag, KEY_CHIPVERSION, value,  FileName); 

	if(pFlag_SettingPrev)
		sprintf(value, "0x%X",  (*pFlag_SettingPrev)); 
	else
		sprintf(value, "0x00");
	WritePrivateProfileString(tag, KEY_SETTINGROLLBACK, value,  FileName);
	
	if(pNumOfRailAvaiableInChip)
		sprintf(value, "0x%X",  (*pNumOfRailAvaiableInChip)); 
	else
		sprintf(value, "0x00");
	WritePrivateProfileString(tag, KEY_RAIL_AVAIL_IN_CHIP, value,  FileName); 

 	if(pDpSlp)
		sprintf(value, "0x%X",  (*pDpSlp)); 
	else
		sprintf(value, "0x00");
	WritePrivateProfileString(tag, KEY_DPDLP_BYTE, value,  FileName); 
 

	if(pPowerDownMode)
		sprintf(value, "0x%X", (*pPowerDownMode)); 
 	else
		sprintf(value, "0x00");
	WritePrivateProfileString(tag, KEY_PWRDN_BYTE, value,  FileName); 
  		
	for(i=0; i<MAX_PM_RAIL_PW886; i++)
	{
		sprintf(key, KEY_NORMALMODE_VOLT_FORMAT, i);
 		if(pNormalMode)
			sprintf(value, "0x%X", pNormalMode[i]); 
 		else
			sprintf(value, "0x00");
		WritePrivateProfileString(tag, key, value,  FileName); 

		sprintf(key, KEY_SLEEPMODE_FLAG_FORMAT, i);
 		if(pSleepModeFlag)
			sprintf(value, "0x%X", pSleepModeFlag[i]); 
 		else
			sprintf(value, "0x00");
		WritePrivateProfileString(tag, key, value,  FileName); 

		sprintf(key, KEY_SLEEPMODE_VOLT_FORMAT, i);
 		if(pSleepModeSetting)
			sprintf(value, "0x%X", pSleepModeSetting[i]); 
 		else
			sprintf(value, "0x00");
		WritePrivateProfileString(tag, key, value,  FileName); 
	}
	
	return status;

}


int DutSharedClssHeader ReadAnnex42DataFromFile(char *FileName,
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
					BYTE *pPTargetDeltaList)
{
 	char value[MAX_LENGTH]="";
	char key[MAX_LENGTH]="";
	char tag[MAX_LENGTH]="";
	int status=0, i=0;
 
	//header

	DebugLogRite("File Name :%s\n", FileName);
  	sprintf(tag, TAG_ANNEX42); 
 
  	if (!GetPrivateProfileSection(tag, value, MAX_LENGTH, FileName))
		return ERROR_DATANOTEXIST;
	//header
 	
 	if(pNoOfSubBand)
		(*pNoOfSubBand) = GetPrivateProfileInt(tag, KEY_THERMAL_CAL_NO_SUBBAND, 
										(*pNoOfSubBand), FileName); 
	else 
		return ERROR_INPUT_INVALID;

	for (i=0; i<(*pNoOfSubBand); i++)
	{  
		sprintf(key, KEY_THERMAL_CAL_BAND_FORMAT, i); 
 		if(pBandList)
			pBandList[i] = GetPrivateProfileInt(tag, key, 
											pBandList[i], FileName); 

		sprintf(key, KEY_THERMAL_CAL_SUBBAND_FORMAT, i); 
 		if(pSubBandList)
			pSubBandList[i] = GetPrivateProfileInt(tag, key, 
											pSubBandList[i], FileName); 

		sprintf(key, KEY_THERMAL_CAL_TEMPREF_MINUS40C_FORMAT, i); 
 		if(pTempRefMinus40CList)
			pTempRefMinus40CList[i] = GetPrivateProfileInt(tag, key, 
											pTempRefMinus40CList[i], FileName); 
 
		sprintf(key, KEY_THERMAL_CAL_TEMPREF_0C_FORMAT, i); 
 		if(pTempRef0CList)
			pTempRef0CList[i] = GetPrivateProfileInt(tag, key, 
											pTempRef0CList[i], FileName); 
 
		sprintf(key, KEY_THERMAL_CAL_TEMPREF_65C_FORMAT, i); 
 		if(pTempRef65CList)
			pTempRef65CList[i] = GetPrivateProfileInt(tag, key, 
											pTempRef65CList[i], FileName); 

		sprintf(key, KEY_THERMAL_CAL_TEMPREF_85C_FORMAT, i); 
 		if(pTempRef85CList)
			pTempRef85CList[i] = GetPrivateProfileInt(tag, key, 
											pTempRef85CList[i], FileName); 
 
		sprintf(key, KEY_THERMAL_CAL_PPA_COEFF_FORMAT, i); 
 		if(pPPACoeffList)
			pPPACoeffList[i] = GetPrivateProfileInt(tag, key, 
											pPPACoeffList[i], FileName); 
 
		sprintf(key, KEY_THERMAL_CAL_PA_COEFF_FORMAT, i); 
 		if(pPACoeffList)
			pPACoeffList[i] = GetPrivateProfileInt(tag, key, 
											pPACoeffList[i], FileName); 

		sprintf(key, KEY_THERMAL_CAL_NUMERATOR_FORMAT, i); 
 		if(pNumeratorList)
			pNumeratorList[i] = GetPrivateProfileInt(tag, key, 
											pNumeratorList[i], FileName); 

		sprintf(key, KEY_THERMAL_CAL_NUMERATOR1_FORMAT, i); 
 		if(pNumerator1List)
			pNumerator1List[i] = GetPrivateProfileInt(tag, key, 
											pNumerator1List[i], FileName); 

		sprintf(key, KEY_THERMAL_CAL_DENOM_FORMAT, i); 
 		if(pDenomList)
			pDenomList[i] = GetPrivateProfileInt(tag, key, 
											pDenomList[i], FileName); 

		sprintf(key, KEY_THERMAL_CAL_PTARGETDELTA_FORMAT, i); 
 		if(pPTargetDeltaList)
			pPTargetDeltaList[i] = GetPrivateProfileInt(tag, key, 
											pPTargetDeltaList[i], FileName); 
	} 
	
	return status;
}

int DutSharedClssHeader WriteAnnex42DataToFile(char *FileName,
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
					BYTE *pPTargetDeltaList)
{
 	char value[MAX_LENGTH]="";
	char key[MAX_LENGTH]="";
	char tag[MAX_LENGTH]="";
	int status=0, i=0;
 
	//header

	DebugLogRite("File Name :%s\n", FileName);
  	sprintf(tag, TAG_ANNEX42); 
 
	//header
 	
	if(pNoOfSubBand)
		sprintf(value, "0x%X",  (*pNoOfSubBand)); 
	else
		sprintf(value, "0x00");
	WritePrivateProfileString(tag, KEY_THERMAL_CAL_NO_SUBBAND, value,  FileName); 

	if(pNoOfSubBand)
	for (i=0; i<(*pNoOfSubBand); i++)
	{ 
		if(pBandList)
			sprintf(value, "%d", pBandList[i]); 
		else
			sprintf(value, "0");
		sprintf(key, KEY_THERMAL_CAL_BAND_FORMAT, i); 
		WritePrivateProfileString(tag, key, value,  FileName); 

		if(pSubBandList)
			sprintf(value, "%d", pSubBandList[i]); 
		else
			sprintf(value, "0");
		sprintf(key, KEY_THERMAL_CAL_SUBBAND_FORMAT, i); 
		WritePrivateProfileString(tag, key, value,  FileName); 

		if(pTempRefMinus40CList)
			sprintf(value, "0x%X", pTempRefMinus40CList[i]); 
		else
			sprintf(value, "0x00");
		sprintf(key, KEY_THERMAL_CAL_TEMPREF_MINUS40C_FORMAT, i); 
		WritePrivateProfileString(tag, key, value,  FileName); 

		if(pTempRef0CList)
			sprintf(value, "0x%X", pTempRef0CList[i]); 
		else
			sprintf(value, "0x00");
		sprintf(key, KEY_THERMAL_CAL_TEMPREF_0C_FORMAT, i); 
		WritePrivateProfileString(tag, key, value,  FileName); 

		if(pTempRef65CList)
			sprintf(value, "0x%X", pTempRef65CList[i]); 
		else
			sprintf(value, "0x00");
		sprintf(key, KEY_THERMAL_CAL_TEMPREF_65C_FORMAT, i); 
		WritePrivateProfileString(tag, key, value,  FileName); 
 
		if(pTempRef85CList)
			sprintf(value, "%d", pTempRef85CList[i]); 
		else
			sprintf(value, "0");
		sprintf(key, KEY_THERMAL_CAL_TEMPREF_85C_FORMAT, i); 
		WritePrivateProfileString(tag, key, value,  FileName); 

		if(pPPACoeffList)
			sprintf(value, "%d", pPPACoeffList[i]); 
		else
			sprintf(value, "0");
		sprintf(key, KEY_THERMAL_CAL_PPA_COEFF_FORMAT, i); 
		WritePrivateProfileString(tag, key, value,  FileName); 

		if(pPACoeffList)
			sprintf(value, "%d", pPACoeffList[i]); 
		else
			sprintf(value, "0");
		sprintf(key, KEY_THERMAL_CAL_PA_COEFF_FORMAT, i); 
		WritePrivateProfileString(tag, key, value,  FileName); 

		if(pNumeratorList)
			sprintf(value, "%d", pNumeratorList[i]); 
		else
			sprintf(value, "0");
		sprintf(key, KEY_THERMAL_CAL_NUMERATOR_FORMAT, i); 
		WritePrivateProfileString(tag, key, value,  FileName); 

		if(pNumerator1List)
			sprintf(value, "%d", pNumerator1List[i]); 
		else
			sprintf(value, "0");
		sprintf(key, KEY_THERMAL_CAL_NUMERATOR1_FORMAT, i); 
		WritePrivateProfileString(tag, key, value,  FileName); 

		if(pDenomList)
			sprintf(value, "%d", pDenomList[i]); 
		else
			sprintf(value, "0");
		sprintf(key, KEY_THERMAL_CAL_DENOM_FORMAT, i); 
		WritePrivateProfileString(tag, key, value,  FileName); 

		if(pPTargetDeltaList)
			sprintf(value, "0x%x", pPTargetDeltaList[i]); 
		else
			sprintf(value, "0");
		sprintf(key, KEY_THERMAL_CAL_PTARGETDELTA_FORMAT, i); 
		WritePrivateProfileString(tag, key, value,  FileName); 
	} 
	
	return status;

}


 
int DutSharedClssHeader ReadAnnexBtDataFromFile(char *FileName, 
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
						)
{
 	char value[MAX_LENGTH]="";
	char key[MAX_LENGTH]="";
//	char tag[MAX_LENGTH]="";
	int status=0, i=0, j=0, cnt=0;
 
	//header

	DebugLogRite("File Name :%s\n", FileName);
	if(!IsFileExist(FileName)) return ERROR_FILENOTEXIST;
 
	//header
  	if(pLimitPwrAsC2)
		(*pLimitPwrAsC2) = GetPrivateProfileInt(TAG_ANNEX43_CAL_FORMAT, 
										KEY_LIMITPWR2C2, 
										(*pLimitPwrAsC2), FileName); 

 
	if(pGainPpa6dBStepMode)
		(*pGainPpa6dBStepMode) = GetPrivateProfileInt(TAG_ANNEX43_CAL_FORMAT, 
										KEY_PPAGAININ6DBMODE, 
										(*pGainPpa6dBStepMode), FileName); 

	

	if(pPwrUpPwrLvl)
		(*pPwrUpPwrLvl) = GetPrivateProfileInt(TAG_ANNEX43_CAL_FORMAT, 
										KEY_PWRUP_PWRLVL_DBM, 
										(*pPwrUpPwrLvl), FileName); 

  
	if(pTempSlope_Gain)
		(*pTempSlope_Gain) = GetPrivateProfileInt(TAG_ANNEX43_CAL_FORMAT, 
										KEY_GAINTEMPSLOPE, 
										(*pTempSlope_Gain), FileName); 

	
 	
	if(pTempRd_Ref)
		(*pTempRd_Ref) = GetPrivateProfileInt(TAG_ANNEX43_CAL_FORMAT, 
										KEY_REFERENCE_TEMP_RDBK, 
										(*pTempRd_Ref), FileName); 
 	
	if(pFELoss)
		(*pFELoss) = GetPrivateProfileInt(TAG_ANNEX43_CAL_FORMAT, 
										KEY_FE_LOSS, 
										(*pFELoss), FileName); 

	 	
	if(pXtal)
		(*pXtal) = GetPrivateProfileInt(TAG_ANNEX43_CAL_FORMAT, 
										KEY_XTAL, 
										(*pXtal), FileName); 
 	

	for (i=0; i<MAXSAVED_PWRLVL_C2; i++)
	{
		sprintf(key, KEY_PWRINDBM_C2_FORMAT, i);
		if(pPwrIndBm_C2)
			pPwrIndBm_C2[i] = GetPrivateProfileInt(TAG_ANNEX43_CAL_FORMAT, 
										key, 
										pPwrIndBm_C2[i], FileName); 

		sprintf(key, KEY_GAIN_PPA_C2_FORMAT, i);
		if(pGainPpa_C2)
			pGainPpa_C2[i] = GetPrivateProfileInt(TAG_ANNEX43_CAL_FORMAT, 
										key, 
										pGainPpa_C2[i], FileName); 
 
		sprintf(key, KEY_GAIN_PGA_C2_FORMAT, i);
		if(pGainPga_C2)
			pGainPga_C2[i] = GetPrivateProfileInt(TAG_ANNEX43_CAL_FORMAT, 
										key, 
										pGainPga_C2[i], FileName); 

		sprintf(key, KEY_GAIN_BB_C2_FORMAT, i);
		if(pGainBb_C2)
			pGainBb_C2[i] = GetPrivateProfileInt(TAG_ANNEX43_CAL_FORMAT, 
										key, 
										pGainBb_C2[i], FileName); 
	}

	for (i=0; i<MAXSAVED_PWRLVL_C1_5; i++)
	{
		sprintf(key, KEY_PWRINDBM_C1_5_FORMAT, i);
		if(pPwrIndBm_C1_5)
			pPwrIndBm_C1_5[i] = GetPrivateProfileInt(TAG_ANNEX43_CAL_FORMAT, 
										key, 
										pPwrIndBm_C1_5[i], FileName); 
	
		sprintf(key, KEY_GAIN_PPA_C1_5_FORMAT, i);
		if(pGainPpa_C1_5)
			pGainPpa_C1_5[i] = GetPrivateProfileInt(TAG_ANNEX43_CAL_FORMAT, 
										key, 
										pGainPpa_C1_5[i], FileName); 

		sprintf(key, KEY_GAIN_PGA_C1_5_FORMAT, i);
		if(pGainPga_C1_5)
			pGainPga_C1_5[i] = GetPrivateProfileInt(TAG_ANNEX43_CAL_FORMAT, 
										key, 
										pGainPga_C1_5[i], FileName); 
		 
		sprintf(key, KEY_GAIN_BB_C1_5_FORMAT, i);
		if(pGainBb_C1_5)
			pGainBb_C1_5[i] = GetPrivateProfileInt(TAG_ANNEX43_CAL_FORMAT, 
										key, 
										pGainBb_C1_5[i], FileName); 
	}
	
// 44
	if(pMaxPowerdBm_NoRssi)
		(*pMaxPowerdBm_NoRssi) = GetPrivateProfileInt(TAG_ANNEX44_CONFIG_FORMAT, 
										KEY_CONFIG_MINTXPWR_NORSSI_DBM, 
										(*pMaxPowerdBm_NoRssi), FileName); 
  	
	if(pRssiCorrection)
		(*pRssiCorrection) = GetPrivateProfileInt(TAG_ANNEX44_CONFIG_FORMAT, 
										KEY_CAL_RSSI_CORR, 
										(*pRssiCorrection), FileName); 

	
 
 	if(pUartBaudRate)
		(*pUartBaudRate) = GetPrivateProfileInt(TAG_ANNEX44_CONFIG_FORMAT, 
										KEY_UART_BAUDRATE, 
										(*pUartBaudRate), FileName); 
 
 	if(pLocalVersion)
		(*pLocalVersion) = GetPrivateProfileInt(TAG_ANNEX44_CONFIG_FORMAT, 
										KEY_CONFIG_LOCALVERSION, 
										(*pLocalVersion), FileName); 
 	
	if(pRemoteVersion)
		(*pRemoteVersion) = GetPrivateProfileInt(TAG_ANNEX44_CONFIG_FORMAT, 
										KEY_CONFIG_REMOTEVERSION, 
										(*pRemoteVersion), FileName); 

 	if(pDeepSleepState)
		(*pDeepSleepState) = GetPrivateProfileInt(TAG_ANNEX44_CONFIG_FORMAT, 
										KEY_CONFIG_DEEPSLEEPSTATE, 
										(*pDeepSleepState), FileName); 

 	if(pUseEncModeBrcst)
		(*pUseEncModeBrcst) = GetPrivateProfileInt(TAG_ANNEX44_CONFIG_FORMAT, 
										KEY_CONFIG_USEENCMODEBRCST, 
                                        (*pUseEncModeBrcst), FileName)?true:false; 

	if(pUseUnitKey)
		(*pUseUnitKey) = GetPrivateProfileInt(TAG_ANNEX44_CONFIG_FORMAT, 
										KEY_CONFIG_USEUNITKEY, 
                                        (*pUseUnitKey), FileName)?true:false; 
 	
	if(pOnChipProfileEnable)
		(*pOnChipProfileEnable) = GetPrivateProfileInt(TAG_ANNEX44_CONFIG_FORMAT, 
										KEY_CONFIG_ONCHIPPROFILE_ENABLE, 
                                        (*pOnChipProfileEnable), FileName)?true:false; 
 	
	if(pDeviceJitter)
		(*pDeviceJitter) = GetPrivateProfileInt(TAG_ANNEX44_CONFIG_FORMAT, 
										KEY_CONFIG_JITTER, 
										(*pDeviceJitter), FileName); 
	
	if(pDeviceDrift)
		(*pDeviceDrift) = GetPrivateProfileInt(TAG_ANNEX44_CONFIG_FORMAT, 
										KEY_CONFIG_DRIFT, 
										(*pDeviceDrift), FileName); 

 	if(pEncKeyLmax)
		(*pEncKeyLmax) = GetPrivateProfileInt(TAG_ANNEX44_CONFIG_FORMAT, 
										KEY_CONFIG_ENCKEYMAX, 
										(*pEncKeyLmax), FileName); 
 	
	if(pEncKeyLmin)
		(*pEncKeyLmin) = GetPrivateProfileInt(TAG_ANNEX44_CONFIG_FORMAT, 
										KEY_CONFIG_ENCKEYMIN, 
										(*pEncKeyLmin), FileName); 
 	 	
	if(pMaxAcls)
		(*pMaxAcls) = GetPrivateProfileInt(TAG_ANNEX44_CONFIG_FORMAT, 
										KEY_CONFIG_MAXACLS, 
										(*pMaxAcls), FileName); 
 	 	
	if(pMaxScos)
		(*pMaxScos) = GetPrivateProfileInt(TAG_ANNEX44_CONFIG_FORMAT, 
										KEY_CONFIG_MAXSCOS, 
										(*pMaxScos), FileName); 
	
	if(pMaxAclPktLen)
		(*pMaxAclPktLen) = GetPrivateProfileInt(TAG_ANNEX44_CONFIG_FORMAT, 
										KEY_CONFIG_MAXACLPKTLEN, 
										(*pMaxAclPktLen), FileName); 
 	
	if(pMaxAclPkts)
		(*pMaxAclPkts) = GetPrivateProfileInt(TAG_ANNEX44_CONFIG_FORMAT, 
										KEY_CONFIG_MAXACLPKTS, 
										(*pMaxAclPkts), FileName); 

	if(pMaxScoPktLen)
		(*pMaxScoPktLen) = GetPrivateProfileInt(TAG_ANNEX44_CONFIG_FORMAT, 
										KEY_CONFIG_MAXSCOPKTLEN, 
										(*pMaxScoPktLen), FileName); 

	if(pMaxScoPkts)
		(*pMaxScoPkts) = GetPrivateProfileInt(TAG_ANNEX44_CONFIG_FORMAT, 
										KEY_CONFIG_MAXSCOPKTS, 
										(*pMaxScoPkts), FileName); 

	if(pConnRxWnd)
		(*pConnRxWnd) = GetPrivateProfileInt(TAG_ANNEX44_CONFIG_FORMAT, 
										KEY_CONFIG_CONNRXWINDOW, 
										(*pConnRxWnd), FileName); 

	if(pRssiGoldenRangeLow)
		(*pRssiGoldenRangeLow) = GetPrivateProfileInt(TAG_ANNEX44_CONFIG_FORMAT, 
										KEY_CONFIG_RSSIGOLDENRANGE_LOW, 
										(*pRssiGoldenRangeLow), FileName); 

	if(pRssiGoldenRangeHigh)
		(*pRssiGoldenRangeHigh) = GetPrivateProfileInt(TAG_ANNEX44_CONFIG_FORMAT, 
										KEY_CONFIG_RSSIGOLDENRANGE_HIGH, 
										(*pRssiGoldenRangeHigh), FileName); 

	if(pCompatibleMode)
		(*pCompatibleMode) = GetPrivateProfileInt(TAG_ANNEX44_CONFIG_FORMAT, 
										KEY_CONFIG_COMPATIBLEMODE, 
										(*pCompatibleMode), FileName); 

	if(pLMP_PwrReqMsgInterval)
		(*pLMP_PwrReqMsgInterval) = GetPrivateProfileInt(TAG_ANNEX44_CONFIG_FORMAT, 
										KEY_CONFIG_LMP_PWRREQMDGINTERVAL, 
										(*pLMP_PwrReqMsgInterval), FileName); 
	
	if(pLMP_DecrPwrReqMsgIntervalRatio)
		(*pLMP_DecrPwrReqMsgIntervalRatio) = GetPrivateProfileInt(TAG_ANNEX44_CONFIG_FORMAT, 
										KEY_CONFIG_LMP_DECRPWRREQMDGINTERVALRATIO, 
										(*pLMP_DecrPwrReqMsgIntervalRatio), FileName); 
		
	if(pNumOfPwrStep4Clss1p5)
		(*pNumOfPwrStep4Clss1p5) = GetPrivateProfileInt(TAG_ANNEX44_CONFIG_FORMAT, 
										KEY_CONFIG_NUMOFPWRSTEP4CLASS1P5, 
										(*pNumOfPwrStep4Clss1p5), FileName); 
	
	if(pPwrStepSize4Clss1p5)
		(*pPwrStepSize4Clss1p5) = GetPrivateProfileInt(TAG_ANNEX44_CONFIG_FORMAT, 
										KEY_CONFIG_PWRSTEPSIZE4CLASS1P5, 
										(*pPwrStepSize4Clss1p5), FileName); 

	if(pPwrCtrlInClss2Device)
		(*pPwrCtrlInClss2Device) = GetPrivateProfileInt(TAG_ANNEX44_CONFIG_FORMAT, 
										KEY_CONFIG_PWRCTRLINCLASS2DEVICE, 
										(*pPwrCtrlInClss2Device), FileName); 
	
	if(pDynamicRssiLmpTimeInterval)
		(*pDynamicRssiLmpTimeInterval) = GetPrivateProfileInt(TAG_ANNEX44_CONFIG_FORMAT, 
										KEY_CONFIG_DYMANICRSSILMPTIMEINTERVAL, 
										(*pDynamicRssiLmpTimeInterval), FileName); 
		
	if(pDynamicRssiLmpTimeIntervalThrhld)
		(*pDynamicRssiLmpTimeIntervalThrhld) = GetPrivateProfileInt(TAG_ANNEX44_CONFIG_FORMAT, 
										KEY_CONFIG_DYMANICRSSILMPTIMEINTERVALTHRESHOLD, 
										(*pDynamicRssiLmpTimeIntervalThrhld), FileName); 

	for(i=0; i<NUM_WORD_SUPPORTCOMMAND; i++)
	{
		sprintf(key, KEY_CONFIG_LOCAL_SUPP_CMDS_FORMAT, i);
		if(pLocalSupportedCmds)
			pLocalSupportedCmds[i] = GetPrivateProfileInt(TAG_ANNEX44_CONFIG_FORMAT, 
										key, 
										pLocalSupportedCmds[i], FileName); 
	}
	
	
	for(i=0; i<NUM_WORD_SUPPORTFEATURE; i++)
	{
		sprintf(key, KEY_CONFIG_LOCAL_SUPP_FEATS_FORMAT, i);
		if(pLocalSupportedFeatures)
			pLocalSupportedFeatures[i] = GetPrivateProfileInt(TAG_ANNEX44_CONFIG_FORMAT, 
										key, 
										pLocalSupportedFeatures[i], FileName); 
	}
	
	if(pBdAddress)
	{
		int tempI[6]={0};
		sprintf(key, KEY_CONFIG_BDADDRESS);
			cnt = GetPrivateProfileString(TAG_ANNEX44_CONFIG_FORMAT, 
										key, 
										"", value, MAX_LENGTH, FileName); 

	
	 
		sscanf(value, "%02X.%02X.%02X.%02X.%02X.%02X", 
			&tempI[0], &tempI[1], &tempI[2], 
			&tempI[3], &tempI[4], &tempI[5]); 
	
		pBdAddress[0] = tempI[0];
		pBdAddress[1] = tempI[1];
		pBdAddress[2] = tempI[2];
		pBdAddress[3] = tempI[3];
		pBdAddress[4] = tempI[4];
		pBdAddress[5] = tempI[5];

	}
 	
	if(pLocalDeviceName)
		GetPrivateProfileString(TAG_ANNEX44_CONFIG_FORMAT, KEY_LOCALDEVICENAME, 
					"", (char*)pLocalDeviceName, MAXSTRLEN_LOCALDEVICENAME, FileName); 
 

 	if(pMask)
		(*pMask) = GetPrivateProfileInt(TAG_ANNEX44_CONFIG_FORMAT, 
										KEY_CONFIG_MASK, 
										(*pMask), FileName); 

#ifndef _W8780_
	for(i=0; i<MAXPARTNER; i++)
	{
		sprintf(key, KEY_CONFIG_MASK_BDADDR_FORMAT, i);

 		if(pBtAddrList)
		{	
			GetPrivateProfileString(TAG_ANNEX44_CONFIG_FORMAT,key,"", value, 20, FileName); 
			sscanf(value, "%02X.%02X.%02X.%02X.%02X.%02X", 
				&pBtAddrList[i][0], &pBtAddrList[i][1], &pBtAddrList[i][2], 
				&pBtAddrList[i][3], &pBtAddrList[i][4], &pBtAddrList[i][5]); 
		}
 
 
		sprintf(key, KEY_CONFIG_MASK_LINKKEY_FORMAT, i);
 		if(pBtKeyList)
		{	
			char *ptr=NULL;
			cnt = GetPrivateProfileString(TAG_ANNEX44_CONFIG_FORMAT, key, "", value, 20, FileName); 
			ptr = value;

			for (j=0; j<cnt/2; j++)
			{
					sscanf(ptr, "%02X", &pBtKeyList[i][j]);  
					ptr +=2;
			}
		}
 
	}

	for (i=0; i<MAX_BYTE_SECURITYKEY; i++)
	{
		sprintf(key, KEY_CONFIG_MASK_SECURITYKEY_FORMAT, i);
		if(pSecurityKey)
			pSecurityKey[i] = GetPrivateProfileInt(TAG_ANNEX44_CONFIG_FORMAT, 
										key, 
										pSecurityKey[i], FileName); 
	}

#endif //#ifndef _W8780_
	
	return status;

}


 
int DutSharedClssHeader WriteAnnexBtDataToFile(char *FileName, 
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
						)
{
 	char value[MAX_LENGTH]="";
	char key[MAX_LENGTH]="";
//	char tag[MAX_LENGTH]="";
	int status=0, i=0, cnt=0;
 
	//header

	DebugLogRite("File Name :%s\n", FileName);
 
	//header
  	if(pLimitPwrAsC2)
		sprintf(value, "0x%X", (*pLimitPwrAsC2)); 
	else
		sprintf(value, "0x00");
	WritePrivateProfileString(TAG_ANNEX43_CAL_FORMAT,KEY_LIMITPWR2C2, value, FileName); 

	if(pGainPpa6dBStepMode)
		sprintf(value, "0x%X", (*pGainPpa6dBStepMode)); 
	else
		sprintf(value, "0x00");
	WritePrivateProfileString(TAG_ANNEX43_CAL_FORMAT,KEY_PPAGAININ6DBMODE, value, FileName); 

	if(pPwrUpPwrLvl)
		sprintf(value, "0x%X", (*pPwrUpPwrLvl)); 
	else
		sprintf(value, "0x00");
	WritePrivateProfileString(TAG_ANNEX43_CAL_FORMAT,KEY_PWRUP_PWRLVL_DBM, value, FileName); 
 
	if(pTempSlope_Gain)
		sprintf(value, "0x%X", (*pTempSlope_Gain)); 
	else
		sprintf(value, "0x00");
	WritePrivateProfileString(TAG_ANNEX43_CAL_FORMAT,KEY_GAINTEMPSLOPE, value, FileName); 
 	
	if(pTempRd_Ref)
		sprintf(value, "0x%X", (*pTempRd_Ref)); 
	else
		sprintf(value, "0x00");
	WritePrivateProfileString(TAG_ANNEX43_CAL_FORMAT,KEY_REFERENCE_TEMP_RDBK, value, FileName); 
 	
	if(pFELoss)
		sprintf(value, "0x%X", (*pFELoss)); 
	else
		sprintf(value, "0x00");
	WritePrivateProfileString(TAG_ANNEX43_CAL_FORMAT,KEY_FE_LOSS, value, FileName); 
 	
	if(pXtal)
		sprintf(value, "0x%X", (*pXtal)); 
	else
		sprintf(value, "0x00");
	WritePrivateProfileString(TAG_ANNEX43_CAL_FORMAT,KEY_XTAL, value, FileName); 
 	

	for (i=0; i<MAXSAVED_PWRLVL_C2; i++)
	{
		sprintf(key, KEY_PWRINDBM_C2_FORMAT, i);
		if(pPwrIndBm_C2)
			sprintf(value, "%d", pPwrIndBm_C2[i]); 
		else
			sprintf(value, "0");
		WritePrivateProfileString(TAG_ANNEX43_CAL_FORMAT,key, value, FileName); 

		sprintf(key, KEY_GAIN_PPA_C2_FORMAT, i);
		if(pGainPpa_C2)
			sprintf(value, "0x%X", pGainPpa_C2[i]); 
		else
			sprintf(value, "0x00");
		WritePrivateProfileString(TAG_ANNEX43_CAL_FORMAT,key, value, FileName); 
 
		sprintf(key, KEY_GAIN_PGA_C2_FORMAT, i);
		if(pGainPga_C2)
			sprintf(value, "0x%X", pGainPga_C2[i]); 
		else
			sprintf(value, "0x00");
		WritePrivateProfileString(TAG_ANNEX43_CAL_FORMAT,key, value, FileName); 

		sprintf(key, KEY_GAIN_BB_C2_FORMAT, i);
		if(pGainBb_C2)
			sprintf(value, "0x%X", pGainBb_C2[i]); 
		else
			sprintf(value, "0x00");
		WritePrivateProfileString(TAG_ANNEX43_CAL_FORMAT,key, value, FileName); 

	}
	for (i=0; i<MAXSAVED_PWRLVL_C1_5; i++)
	{
		sprintf(key, KEY_PWRINDBM_C1_5_FORMAT, i);
		if(pPwrIndBm_C1_5)
			sprintf(value, "%d", pPwrIndBm_C1_5[i]); 
		else
			sprintf(value, "0");
		WritePrivateProfileString(TAG_ANNEX43_CAL_FORMAT,key, value, FileName); 
	
		sprintf(key, KEY_GAIN_PPA_C1_5_FORMAT, i);
		if(pGainPpa_C1_5)
			sprintf(value, "0x%X", pGainPpa_C1_5[i]); 
		else
			sprintf(value, "0x00");
		WritePrivateProfileString(TAG_ANNEX43_CAL_FORMAT,key, value, FileName); 
 
		sprintf(key, KEY_GAIN_PGA_C1_5_FORMAT, i);
		if(pGainPga_C1_5)
			sprintf(value, "0x%X", pGainPga_C1_5[i]); 
		else
			sprintf(value, "0x00");
		WritePrivateProfileString(TAG_ANNEX43_CAL_FORMAT,key, value, FileName); 

		sprintf(key, KEY_GAIN_BB_C1_5_FORMAT, i);
		if(pGainBb_C1_5)
			sprintf(value, "0x%X", pGainBb_C1_5[i]); 
		else
			sprintf(value, "0x00");
		WritePrivateProfileString(TAG_ANNEX43_CAL_FORMAT,key, value, FileName); 

	}
	
// 44
  	if(pMaxPowerdBm_NoRssi)
		sprintf(value, "%d", (*pMaxPowerdBm_NoRssi)); 
	else
		sprintf(value, "0");
	WritePrivateProfileString(TAG_ANNEX44_CONFIG_FORMAT, KEY_CONFIG_MINTXPWR_NORSSI_DBM, value, FileName);  
 	
	if(pRssiCorrection)
		sprintf(value, "0x%X", (*pRssiCorrection)); 
	else
		sprintf(value, "0x00");
	WritePrivateProfileString(TAG_ANNEX44_CONFIG_FORMAT, KEY_CAL_RSSI_CORR, value, FileName);  

 	if(pUartBaudRate)
		sprintf(value, "%d", (*pUartBaudRate)); 
	else
		sprintf(value, "0");
	WritePrivateProfileString(TAG_ANNEX44_CONFIG_FORMAT, KEY_UART_BAUDRATE, value, FileName);  

 
 	if(pLocalVersion)
		sprintf(value, "0x%X", (*pLocalVersion)); 
	else
		sprintf(value, "0x00");
	WritePrivateProfileString(TAG_ANNEX44_CONFIG_FORMAT, KEY_CONFIG_LOCALVERSION, value, FileName);  
 	
	if(pRemoteVersion)
		sprintf(value, "0x%X", (*pRemoteVersion)); 
	else
		sprintf(value, "0x00");
	WritePrivateProfileString(TAG_ANNEX44_CONFIG_FORMAT, KEY_CONFIG_REMOTEVERSION, value, FileName);  

 	if(pDeepSleepState)
		sprintf(value, "%d", (*pDeepSleepState)); 
	else
		sprintf(value, "0");
	WritePrivateProfileString(TAG_ANNEX44_CONFIG_FORMAT, KEY_CONFIG_DEEPSLEEPSTATE, value, FileName);  
 	
	if(pUseEncModeBrcst)
		sprintf(value, "%d", (*pUseEncModeBrcst)); 
	else
		sprintf(value, "0");
	WritePrivateProfileString(TAG_ANNEX44_CONFIG_FORMAT, KEY_CONFIG_USEENCMODEBRCST, value, FileName);  
 	
	if(pUseUnitKey)
		sprintf(value, "%d", (*pUseUnitKey)); 
	else
		sprintf(value, "0");
	WritePrivateProfileString(TAG_ANNEX44_CONFIG_FORMAT, KEY_CONFIG_USEUNITKEY, value, FileName);  
 	
	if(pOnChipProfileEnable)
		sprintf(value, "%d", (*pOnChipProfileEnable)); 
	else
		sprintf(value, "0");
	WritePrivateProfileString(TAG_ANNEX44_CONFIG_FORMAT, KEY_CONFIG_ONCHIPPROFILE_ENABLE, value, FileName);  

	if(pDeviceJitter)
		sprintf(value, "%d", (*pDeviceJitter)); 
	else
		sprintf(value, "0");
	WritePrivateProfileString(TAG_ANNEX44_CONFIG_FORMAT, KEY_CONFIG_JITTER, value, FileName);  
 	
	if(pDeviceDrift)
		sprintf(value, "%d", (*pDeviceDrift)); 
	else
		sprintf(value, "0");
	WritePrivateProfileString(TAG_ANNEX44_CONFIG_FORMAT, KEY_CONFIG_DRIFT, value, FileName);  


 	if(pEncKeyLmax)
		sprintf(value, "%d", (*pEncKeyLmax)); 
	else
		sprintf(value, "0");
	WritePrivateProfileString(TAG_ANNEX44_CONFIG_FORMAT, KEY_CONFIG_ENCKEYMAX, value, FileName);  
 	
	if(pEncKeyLmin)
		sprintf(value, "%d", (*pEncKeyLmin)); 
	else
		sprintf(value, "0");
	WritePrivateProfileString(TAG_ANNEX44_CONFIG_FORMAT, KEY_CONFIG_ENCKEYMIN, value, FileName);  
 	
	if(pMaxAcls)
		sprintf(value, "%d", (*pMaxAcls)); 
	else
		sprintf(value, "0");
	WritePrivateProfileString(TAG_ANNEX44_CONFIG_FORMAT, KEY_CONFIG_MAXACLS, value, FileName);  

	if(pMaxScos)
		sprintf(value, "%d", (*pMaxScos)); 
	else
		sprintf(value, "0");
	WritePrivateProfileString(TAG_ANNEX44_CONFIG_FORMAT, KEY_CONFIG_MAXSCOS, value, FileName);  
 	
	if(pMaxAclPktLen)
		sprintf(value, "%d", (*pMaxAclPktLen)); 
	else
		sprintf(value, "0");
	WritePrivateProfileString(TAG_ANNEX44_CONFIG_FORMAT, KEY_CONFIG_MAXACLPKTLEN, value, FileName);  
 	
	if(pMaxAclPkts)
		sprintf(value, "%d", (*pMaxAclPkts)); 
	else
		sprintf(value, "0");
	WritePrivateProfileString(TAG_ANNEX44_CONFIG_FORMAT, KEY_CONFIG_MAXACLPKTS, value, FileName);  
  	
	if(pMaxScoPktLen)
		sprintf(value, "%d", (*pMaxScoPktLen)); 
	else
		sprintf(value, "0");
	WritePrivateProfileString(TAG_ANNEX44_CONFIG_FORMAT, KEY_CONFIG_MAXSCOPKTLEN, value, FileName);  
	
	if(pMaxScoPkts)
		sprintf(value, "%d", (*pMaxScoPkts)); 
	else
		sprintf(value, "0");
	WritePrivateProfileString(TAG_ANNEX44_CONFIG_FORMAT, KEY_CONFIG_MAXSCOPKTS, value, FileName);  

	if(pConnRxWnd)
		sprintf(value, "%d", (*pConnRxWnd)); 
	else
		sprintf(value, "0");
	WritePrivateProfileString(TAG_ANNEX44_CONFIG_FORMAT, KEY_CONFIG_CONNRXWINDOW, value, FileName);  
  	
	if(pRssiGoldenRangeLow)
		sprintf(value, "0x%X", (*pRssiGoldenRangeLow)); 
	else
		sprintf(value, "0x00");
	WritePrivateProfileString(TAG_ANNEX44_CONFIG_FORMAT, KEY_CONFIG_RSSIGOLDENRANGE_LOW, value, FileName);  
  	
	if(pRssiGoldenRangeHigh)
		sprintf(value, "0x%X", (*pRssiGoldenRangeHigh)); 
	else
		sprintf(value, "0x00");
	WritePrivateProfileString(TAG_ANNEX44_CONFIG_FORMAT, KEY_CONFIG_RSSIGOLDENRANGE_HIGH, value, FileName);  

	if(pCompatibleMode)
		sprintf(value, "%d", (*pCompatibleMode)); 
	else
		sprintf(value, "0");
	WritePrivateProfileString(TAG_ANNEX44_CONFIG_FORMAT, KEY_CONFIG_COMPATIBLEMODE, value, FileName);  

	if(pLMP_PwrReqMsgInterval)
		sprintf(value, "0x%X", (*pLMP_PwrReqMsgInterval)); 
	else
		sprintf(value, "0x00");
	WritePrivateProfileString(TAG_ANNEX44_CONFIG_FORMAT, KEY_CONFIG_LMP_PWRREQMDGINTERVAL, value, FileName);  
	
	if(pLMP_DecrPwrReqMsgIntervalRatio)
		sprintf(value, "%d", (*pLMP_DecrPwrReqMsgIntervalRatio)); 
	else
		sprintf(value, "0");
	WritePrivateProfileString(TAG_ANNEX44_CONFIG_FORMAT, KEY_CONFIG_LMP_DECRPWRREQMDGINTERVALRATIO, value, FileName);  
	
	if(pNumOfPwrStep4Clss1p5)
		sprintf(value, "%d", (*pNumOfPwrStep4Clss1p5)); 
	else
		sprintf(value, "0");
	WritePrivateProfileString(TAG_ANNEX44_CONFIG_FORMAT, KEY_CONFIG_NUMOFPWRSTEP4CLASS1P5, value, FileName);  
	
	if(pPwrStepSize4Clss1p5)
		sprintf(value, "0x%X", (*pPwrStepSize4Clss1p5)); 
	else
		sprintf(value, "0x00");
	WritePrivateProfileString(TAG_ANNEX44_CONFIG_FORMAT, KEY_CONFIG_PWRSTEPSIZE4CLASS1P5, value, FileName);  
	
	if(pPwrCtrlInClss2Device)
		sprintf(value, "%d", (*pPwrCtrlInClss2Device)); 
	else
		sprintf(value, "0");
	WritePrivateProfileString(TAG_ANNEX44_CONFIG_FORMAT, KEY_CONFIG_PWRCTRLINCLASS2DEVICE, value, FileName);  
	
	if(pDynamicRssiLmpTimeInterval)
		sprintf(value, "0x%X", (*pDynamicRssiLmpTimeInterval)); 
	else
		sprintf(value, "0x00");
	WritePrivateProfileString(TAG_ANNEX44_CONFIG_FORMAT, KEY_CONFIG_DYMANICRSSILMPTIMEINTERVAL, value, FileName);  
	
	if(pDynamicRssiLmpTimeIntervalThrhld)
		sprintf(value, "%d", (*pDynamicRssiLmpTimeIntervalThrhld)); 
	else
		sprintf(value, "0");
	WritePrivateProfileString(TAG_ANNEX44_CONFIG_FORMAT, KEY_CONFIG_DYMANICRSSILMPTIMEINTERVALTHRESHOLD, value, FileName);  


	for(i=0; i<NUM_WORD_SUPPORTCOMMAND; i++)
	{
		sprintf(key, KEY_CONFIG_LOCAL_SUPP_CMDS_FORMAT, i);
		if(pLocalSupportedCmds)
			sprintf(value, "0x%04X", pLocalSupportedCmds[i]); 
		else
			sprintf(value, "0x0000");
		WritePrivateProfileString(TAG_ANNEX44_CONFIG_FORMAT, key, value, FileName);  
	}
	
	
	for(i=0; i<NUM_WORD_SUPPORTFEATURE; i++)
	{
		sprintf(key, KEY_CONFIG_LOCAL_SUPP_FEATS_FORMAT, i);
		if(pLocalSupportedFeatures)
			sprintf(value, "0x%04X", pLocalSupportedFeatures[i]); 
		else
			sprintf(value, "0x0000");
		WritePrivateProfileString(TAG_ANNEX44_CONFIG_FORMAT, key, value, FileName);  
	}
	
	if(pBdAddress)
	{
		sprintf(value, "%02X.%02X.%02X.%02X.%02X.%02X", 
			pBdAddress[0], pBdAddress[1], pBdAddress[2], 
			pBdAddress[3], pBdAddress[4], pBdAddress[5]); 
	}
	else
		sprintf(value, "00.00.00.00.00.00"); 
	WritePrivateProfileString(TAG_ANNEX44_CONFIG_FORMAT, KEY_CONFIG_BDADDRESS, value, FileName); 
	
	if(pLocalDeviceName)
		WritePrivateProfileString(TAG_ANNEX44_CONFIG_FORMAT, KEY_LOCALDEVICENAME, 
					(char*)pLocalDeviceName, FileName); 
	else
		WritePrivateProfileString(TAG_ANNEX44_CONFIG_FORMAT, KEY_LOCALDEVICENAME, 
					"", FileName); 


 	if(pMask)
		sprintf(value, "0x%X", (*pMask)); 
	else
		sprintf(value, "0x00");
	WritePrivateProfileString(TAG_ANNEX44_CONFIG_FORMAT, KEY_CONFIG_MASK, value, FileName);  


#ifndef _W8780_
	for(i=0; i<MAXPARTNER; i++)
	{
		sprintf(key, KEY_CONFIG_MASK_BDADDR_FORMAT, i);

		GetPrivateProfileString(TAG_ANNEX44_CONFIG_FORMAT,key,"", value, 20, FileName); 
 		if(pBtAddrList)
		{
			sprintf(value, "%02X.%02X.%02X.%02X.%02X.%02X", 
				pBtAddrList[i][0], pBtAddrList[i][1], pBtAddrList[i][2], 
				pBtAddrList[i][3], pBtAddrList[i][4], pBtAddrList[i][5]); 
		}
		else
		{
			sprintf(value, "00.00.00.00.00.00");
		}
		WritePrivateProfileString(TAG_ANNEX44_CONFIG_FORMAT, key, value, FileName); 

 
		sprintf(key, KEY_CONFIG_MASK_LINKKEY_FORMAT, i);
		strcpy(value, "");
		for (cnt=0; cnt<SIZE_LINKKEY_INBYTE; cnt++)
		{
 			if(pBtKeyList)
				sprintf(value, "%s%02X", value, pBtKeyList[i][cnt]);  
		}
		WritePrivateProfileString(TAG_ANNEX44_CONFIG_FORMAT, key, value, FileName); 

 
	}
	for (i=0; i<MAX_BYTE_SECURITYKEY; i++)
	{
		sprintf(key, KEY_CONFIG_MASK_SECURITYKEY_FORMAT, i);
		if(pSecurityKey)
			sprintf(value, "0x%02X",pSecurityKey[i]);
		else
			sprintf(value, "0x00");
		WritePrivateProfileString(TAG_ANNEX44_CONFIG_FORMAT, key, value, FileName); 
	}
#endif //#ifndef _W8780_

	
	return status;

}




int DutSharedClssHeader ReadAnnex45DataFromFile(char *FileName,  
						   bool *pFlag_GpioCtrlXosc,
						BYTE *pXoscSettling,
						DWORD *pPinOut_DrvLow,
						DWORD *pPinIn_PullDiable)
{
 	char value[MAX_LENGTH]="";
//	char key[MAX_LENGTH]="";
	char tag[MAX_LENGTH]="";
	int status=0;
 
	//header

	DebugLogRite("File Name :%s\n", FileName);
  	sprintf(tag, TAG_ANNEX45); 
 
  	if (!GetPrivateProfileSection(tag, value, MAX_LENGTH, FileName))
		return ERROR_DATANOTEXIST;
	//header
	
		if(pFlag_GpioCtrlXosc)
			(*pFlag_GpioCtrlXosc) = GetPrivateProfileInt(tag, 
										KEY_FLAG_GPIOCTRLXOSC, 
                                        (*pFlag_GpioCtrlXosc), FileName)?true:false; 

		if(pXoscSettling)
			(*pXoscSettling) = GetPrivateProfileInt(tag, 
										KEY_FLAG_XOSCSETTLING, 
										(*pXoscSettling), FileName); 

		if(pPinOut_DrvLow)
		{
			pPinOut_DrvLow[0] = GetPrivateProfileInt(tag, 
										KEY_CONFIG_PINOUT_DRVLOW_LOW, 
										pPinOut_DrvLow[0], FileName); 

			pPinOut_DrvLow[1] = GetPrivateProfileInt(tag, 
										KEY_CONFIG_PINOUT_DRVLOW_HIGH, 
										pPinOut_DrvLow[1], FileName); 
		}

		if(pPinIn_PullDiable)
		{
			pPinIn_PullDiable[0] = GetPrivateProfileInt(tag, 
										KEY_CONFIG_PININ_PULLDISABLE_LOW, 
										pPinIn_PullDiable[0], FileName); 

			pPinIn_PullDiable[1] = GetPrivateProfileInt(tag, 
										KEY_CONFIG_PININ_PULLDISABLE_HIGH, 
										pPinIn_PullDiable[1], FileName); 
		}
	return status;

}

int DutSharedClssHeader WriteAnnex45DataToFile(char *FileName,  
												bool *pFlag_GpioCtrlXosc,
												BYTE *pXoscSettling,
												DWORD *pPinOut_DrvLow,
												DWORD *pPinIn_PullDiable)
{
 	char value[MAX_LENGTH]="";
	char tag[MAX_LENGTH]="";
	int status=0;
 
	//header
	DebugLogRite("File Name :%s\n", FileName);
  	sprintf(tag, TAG_ANNEX45); 
 
	//header
		if(pFlag_GpioCtrlXosc)
			sprintf(value, "0x%X", (*pFlag_GpioCtrlXosc)); 
		else
			sprintf(value, "0x00");

		WritePrivateProfileString(tag, KEY_FLAG_GPIOCTRLXOSC, value,  FileName); 

		if(pXoscSettling)
			sprintf(value, "0x%X", (*pXoscSettling)); 
		else
			sprintf(value, "0x00");
		WritePrivateProfileString(tag, KEY_FLAG_XOSCSETTLING, value,  FileName); 

		if(pPinOut_DrvLow)
		{
			sprintf(value, "0x%X", pPinOut_DrvLow[0]); 
			WritePrivateProfileString(tag, KEY_CONFIG_PINOUT_DRVLOW_LOW, value,  FileName); 

			sprintf(value, "0x%X", pPinOut_DrvLow[1]); 
			WritePrivateProfileString(tag, KEY_CONFIG_PINOUT_DRVLOW_HIGH, value,  FileName); 
		}
		else
		{
			sprintf(value, "0x00"); 
			WritePrivateProfileString(tag, KEY_CONFIG_PINOUT_DRVLOW_LOW, value,  FileName); 

			sprintf(value, "0x00"); 
			WritePrivateProfileString(tag, KEY_CONFIG_PINOUT_DRVLOW_HIGH, value,  FileName); 
		}

		if(pPinIn_PullDiable)
		{
			sprintf(value, "0x%X", pPinIn_PullDiable[0]); 
			WritePrivateProfileString(tag, KEY_CONFIG_PININ_PULLDISABLE_LOW, value,  FileName);
			
			sprintf(value, "0x%X", pPinIn_PullDiable[1]); 
			WritePrivateProfileString(tag, KEY_CONFIG_PININ_PULLDISABLE_HIGH, value,  FileName); 
		}
		else
		{
			sprintf(value, "0x00"); 
			WritePrivateProfileString(tag, KEY_CONFIG_PININ_PULLDISABLE_LOW, value,  FileName);
			
			sprintf(value, "0x00"); 
			WritePrivateProfileString(tag, KEY_CONFIG_PININ_PULLDISABLE_HIGH, value,  FileName); 
		}
	return status;

}

//REV D
#if defined(_CAL_REV_D_)
//Annex 42
int DutSharedClssHeader WriteAnnex42DataToFile_VD(char *FileName,
					BYTE *pNoOfEntry,
					THERMALCALPARMS_VD *ThermalCalParam)
{
	char value[MAX_LENGTH]="";
	char key[MAX_LENGTH]="";
	char tag[MAX_LENGTH]="";
	int status=0, i=0;
	int NumOfSubG=0,  NumOfSubA=0, BadnID=BAND_802_11G;
	int  buf=0;
	THERMALCALPARMS_VD *ptr=(THERMALCALPARMS_VD *)ThermalCalParam;


	DebugLogRite("File Name :%s\n", FileName);
  
	for (i=0; i <(*pNoOfEntry); i++)
	{
		if(ptr->BandID==BAND_802_11G)
			NumOfSubG++;
		
		if(ptr->BandID==BAND_802_11A)
			NumOfSubA++;

		ptr++;
	}
	//JKLCHEN
	NumOfSubG=1;
	NumOfSubA=4;
	//JKLCHEN
	ptr=(THERMALCALPARMS_VD *)ThermalCalParam;
	//
	if(*pNoOfEntry)
	{
		sprintf(value, "%d", (NumOfSubG));
		WritePrivateProfileString(TAG_ANNEX42,KEY_THERMAL_CAL_NO_SUBBAND_G, 
									value, FileName);
		sprintf(value, "%d", (NumOfSubA));
		WritePrivateProfileString(TAG_ANNEX42,KEY_THERMAL_CAL_NO_SUBBAND_A, 
									value, FileName);
	}
    //G Band
	for (i=0; i <(NumOfSubG); i++)
	{
		sprintf(key, KEY_THERMAL_CAL_RSSICALNUM_NORMAL, BadnID, i);
		sprintf(value, "0x%x", (ptr->RSSICalNum_normal&0xFF));
		//sprintf(key, KEY_THERMAL_CAL_TEMPREF_FORMAT_40, BadnID, i);
		//sprintf(value, "0x%x", (ptr->TempRefMinus40&0xFF));
		WritePrivateProfileString(TAG_ANNEX42, key, value, FileName);

		sprintf(key, KEY_THERMAL_CAL_TEMPREF_SLOPE_FORMAT, BadnID, i);
        sprintf(value, "0x%x", ptr->TempSlope);
		WritePrivateProfileString(TAG_ANNEX42, key, value, FileName);

		//PTargetDelta; //PTARGET_TEMP=0x20
		sprintf(key, KEY_THERMAL_CAL_PTARGET_TEMP, BadnID, i);
		sprintf(value, "0x%x", ptr->PTargetDelta);
		WritePrivateProfileString(TAG_ANNEX42,key, value, FileName);

		sprintf(key, KEY_THERMAL_CAL_RSSICALNUM_BYPASS, BadnID, i); 
		sprintf(value, "0x%x", ptr->RSSICalNum_bypass&0xFF);
		//TempRef85
		//sprintf(key, KEY_THERMAL_CAL_TEMPREF_FORMAT_85, BadnID, i); 
		//sprintf(value, "0x%x", ptr->TempRef85&0xFF);
		WritePrivateProfileString(TAG_ANNEX42, key, value, FileName);


		//Numerator; //SLOPENUM=190 

		sprintf(key, KEY_THERMAL_CAL_SLOPENUM, BadnID, i);
		//;sprintf(value, "%d", ptr->Numerator);
		sprintf(value, "%d",(ptr->Numerator_hot&0xFF));	
		WritePrivateProfileString(TAG_ANNEX42, key, value, FileName);

		//Numerator1; //SLOPENUM2=200
		sprintf(key, KEY_THERMAL_CAL_SLOPENUM2, BadnID, i);
		sprintf(value, "%d", ptr->Numerator_cold&0xFF);
		WritePrivateProfileString(TAG_ANNEX42,key, value, FileName);

		//Denom;			//SLOPEDENOM=50
		sprintf(key, KEY_THERMAL_CAL_SLOPEDENOM, BadnID, i);
		sprintf(value, "%d", ptr->Denom&0xFF);
		WritePrivateProfileString(TAG_ANNEX42, key, value, FileName);
	
		sprintf(key, KEY_THERMAL_CAL_CALINTERVALINMS, BadnID, i);
		sprintf(value, "0x%02x", ptr->TempCalTime&0xFF);
		WritePrivateProfileString(TAG_ANNEX42, key, value, FileName);

		//PPAConefficient; //PPACOEFF=1
		sprintf(key, KEY_THERMAL_CAL_PPACOEFF, BadnID, i);
		sprintf(value, "%d", ptr->PPAConefficient&0xFF);
		WritePrivateProfileString(TAG_ANNEX42,key, value, FileName);

	
		//PAConefficient; //PACOEFF=1
		sprintf(key, KEY_THERMAL_CAL_PACOEFF, BadnID, i);
		sprintf(value, "%d", ptr->PAConefficient&0xFF);
		WritePrivateProfileString(TAG_ANNEX42,key, value, FileName);

		//XTAL; //XTAL =0
		sprintf(key, KEY_THERMAL_CAL_XTAL, BadnID, i);
		sprintf(value, "%d", ptr->XTAL&0xFF);
		WritePrivateProfileString(TAG_ANNEX42,key, value, FileName);

	
		
	}
	ptr++;
	//A band
	BadnID = BAND_802_11A;
    for (i=0; i <(NumOfSubA); i++)
	{
		sprintf(key, KEY_THERMAL_CAL_RSSICALNUM_NORMAL, BadnID, i);
		sprintf(value, "0x%x", (ptr->RSSICalNum_normal & 0xFF));
		//sprintf(key, KEY_THERMAL_CAL_TEMPREF_FORMAT_40, BadnID, i);
		//sprintf(value, "0x%x", (ptr->TempRefMinus40 & 0xFF));
		WritePrivateProfileString(TAG_ANNEX42, key, value, FileName);

		sprintf(key, KEY_THERMAL_CAL_TEMPREF_SLOPE_FORMAT, BadnID, i);
        sprintf(value, "0x%x", ptr->TempSlope);
		WritePrivateProfileString(TAG_ANNEX42, key, value, FileName);

		//PTargetDelta; //PTARGET_TEMP=0x20
		sprintf(key, KEY_THERMAL_CAL_PTARGET_TEMP, BadnID, i);
		sprintf(value, "0x%x", ptr->PTargetDelta);
		WritePrivateProfileString(TAG_ANNEX42,key, value, FileName);

		
		sprintf(key, KEY_THERMAL_CAL_RSSICALNUM_BYPASS, BadnID, i); 
		sprintf(value, "0x%x", ptr->RSSICalNum_bypass &0xFF);
		//TempRef85
		//sprintf(key, KEY_THERMAL_CAL_TEMPREF_FORMAT_85, BadnID, i); 
		//sprintf(value, "0x%x", ptr->TempRef85 &0xFF);
		WritePrivateProfileString(TAG_ANNEX42, key, value, FileName);


		//Numerator; //SLOPENUM=190 

		sprintf(key, KEY_THERMAL_CAL_SLOPENUM, BadnID, i);
		//;sprintf(value, "%d", ptr->Numerator);
		sprintf(value, "%d",(ptr->Numerator_hot&0xFF));	
		WritePrivateProfileString(TAG_ANNEX42, key, value, FileName);

		//Numerator1; //SLOPENUM2=200
		sprintf(key, KEY_THERMAL_CAL_SLOPENUM2, BadnID, i);
		sprintf(value, "%d", ptr->Numerator_cold&0xFF);
		WritePrivateProfileString(TAG_ANNEX42,key, value, FileName);

		//Denom;			//SLOPEDENOM=50
		sprintf(key, KEY_THERMAL_CAL_SLOPEDENOM, BadnID, i);
		sprintf(value, "%d", ptr->Denom&0xFF);
		WritePrivateProfileString(TAG_ANNEX42, key, value, FileName);
	
		sprintf(key, KEY_THERMAL_CAL_CALINTERVALINMS, BadnID, i);
		sprintf(value, "0x%02x", ptr->TempCalTime&0xFF);
		WritePrivateProfileString(TAG_ANNEX42, key, value, FileName);

		//PPAConefficient; //PPACOEFF=1
		sprintf(key, KEY_THERMAL_CAL_PPACOEFF, BadnID, i);
		sprintf(value, "%d", ptr->PPAConefficient&0xFF);
		WritePrivateProfileString(TAG_ANNEX42,key, value, FileName);

	
		//PAConefficient; //PACOEFF=1
		sprintf(key, KEY_THERMAL_CAL_PACOEFF, BadnID, i);
		sprintf(value, "%d", ptr->PAConefficient&0xFF);
		WritePrivateProfileString(TAG_ANNEX42,key, value, FileName);

		//XTAL; //XTAL =0
		sprintf(key, KEY_THERMAL_CAL_XTAL, BadnID, i);
		sprintf(value, "%d", ptr->XTAL&0xFF);
		WritePrivateProfileString(TAG_ANNEX42,key, value, FileName);
		ptr++;
	}

	return 0;
}

int DutSharedClssHeader ReadAnnex42DataFromFile_VD(char *FileName,
					BYTE *pNoOfEntry,
					THERMALCALPARMS_VD *ThermalCalParam)
{

	char value[MAX_LENGTH]="";
	char key[MAX_LENGTH]="";
	char tag[MAX_LENGTH]="";
	int status=0, i=0;
 
	
	int num=0, NumOfSubG=0, NumOfSubA=0;
	int BadnID=BAND_802_11G;
	int THERMALCALPARMS_VD_SIZE= sizeof(THERMALCALPARMS_VD);
    THERMALCALPARMS_VD *ptr=NULL;  
	//header
	DebugLogRite("File Name :%s\n", FileName);
  	sprintf(tag, TAG_ANNEX42); 
 
  	if (!GetPrivateProfileSection(tag, value, MAX_LENGTH, FileName))
		return ERROR_DATANOTEXIST;
	//header
 	
 	if(pNoOfEntry)
	{
		NumOfSubG = GetPrivateProfileInt(tag, KEY_THERMAL_CAL_NO_SUBBAND_G, 
										NumOfSubG, FileName);
		NumOfSubA = GetPrivateProfileInt(tag, KEY_THERMAL_CAL_NO_SUBBAND_A, 
										NumOfSubA, FileName);
		(*pNoOfEntry) = NumOfSubG + NumOfSubA;
	}
	else 
		return ERROR_INPUT_INVALID;

	//
//;	ThermalCalParam = (THERMALCALPARMS_VD *)malloc( sizeof(THERMALCALPARMS_VD) *(*pNoOfEntry));
	ptr = (THERMALCALPARMS_VD*)ThermalCalParam;
	memset(ptr, 0xFF, (sizeof(THERMALCALPARMS_VD) *(*pNoOfEntry)));//JKLCHEN 0xFF for use


	for (i=0; i<NumOfSubG; i++)//JKLCHEN NumOfSubG
	{
		ptr->BandID = BadnID;
		ptr->SubBand = i;
		sprintf(key, KEY_THERMAL_CAL_RSSICALNUM_NORMAL, BadnID, i); 
			ptr->RSSICalNum_normal = GetPrivateProfileInt(tag, key, 
										ptr->RSSICalNum_normal, FileName);
//		sprintf(key, KEY_THERMAL_CAL_TEMPREF_FORMAT_40, BadnID, i); 
//			ptr->TempRefMinus40 = GetPrivateProfileInt(tag, key, 
//										ptr->TempRefMinus40, FileName);

		sprintf(key, KEY_THERMAL_CAL_TEMPREF_SLOPE_FORMAT, BadnID, i);
			ptr->TempSlope = GetPrivateProfileInt(tag, key,
										ptr->TempSlope, FileName);

		if(ptr->TempSlope == (char)0xff)//jklchen fill 0xff to memory if no data
		{
			ptr->BandID=(char)0xff;
			ptr->SubBand=(char)0xff;			
		};		

		sprintf(key, KEY_THERMAL_CAL_CALINTERVALINMS, BadnID, i);
			ptr->TempCalTime = GetPrivateProfileInt(tag, key,
										ptr->TempCalTime, FileName);	

		sprintf(key, KEY_THERMAL_CAL_RSSICALNUM_BYPASS, BadnID, i);
			ptr->RSSICalNum_bypass = GetPrivateProfileInt(tag, key,
										ptr->RSSICalNum_bypass, FileName);
		//TempRef85
//		sprintf(key, KEY_THERMAL_CAL_TEMPREF_FORMAT_85, BadnID, i);
//			ptr->TempRef85 = GetPrivateProfileInt(tag, key,
//										ptr->TempRef85, FileName);

		//Numerator; //SLOPENUM=190  
		sprintf(key, KEY_THERMAL_CAL_SLOPENUM, BadnID, i);
			ptr->Numerator_hot = GetPrivateProfileInt(tag, key,
										ptr->Numerator_hot, FileName);
		//Denom;			//SLOPEDENOM=50
		sprintf(key, KEY_THERMAL_CAL_SLOPEDENOM, BadnID, i);
			ptr->Denom = GetPrivateProfileInt(tag, key,
										ptr->Denom, FileName);
		//PPAConefficient; //PPACOEFF=1
		sprintf(key, KEY_THERMAL_CAL_PPACOEFF, BadnID, i);
			ptr->PPAConefficient = GetPrivateProfileInt(tag, key,
										ptr->PPAConefficient, FileName);
		//PAConefficient; //PACOEFF=1
		sprintf(key, KEY_THERMAL_CAL_PACOEFF, BadnID, i);
			ptr->PAConefficient = GetPrivateProfileInt(tag, key,
										ptr->PAConefficient, FileName);

		//XTAL; //XTAL =0
		sprintf(key, KEY_THERMAL_CAL_XTAL, BadnID, i);
			ptr->XTAL = GetPrivateProfileInt(tag, key,
										ptr->XTAL, FileName);
		//PTargetDelta; //PTARGET_TEMP=0x20
		sprintf(key, KEY_THERMAL_CAL_PTARGET_TEMP, BadnID, i);
			ptr->PTargetDelta = GetPrivateProfileInt(tag, key,
										ptr->PTargetDelta, FileName);
		//Numerator1; //SLOPENUM2=200
		sprintf(key, KEY_THERMAL_CAL_SLOPENUM2, BadnID, i);
			ptr->Numerator_cold = GetPrivateProfileInt(tag, key,
										ptr->Numerator_cold, FileName);
	}
	ptr ++;
	//A band
    BadnID = BAND_802_11A;// JKLCHEN ???
	for (i=0; i<(NumOfSubA); i++)//JKLCHEN from 1 to 4 //NumOfSubA 4 is temp solution
	{
		ptr->BandID = BadnID;
		ptr->SubBand = i;
		sprintf(key, KEY_THERMAL_CAL_RSSICALNUM_NORMAL, BadnID, i); 
			ptr->RSSICalNum_normal = GetPrivateProfileInt(tag, key, 
										ptr->RSSICalNum_normal, FileName);
//		sprintf(key, KEY_THERMAL_CAL_TEMPREF_FORMAT_40, BadnID, i); 
//			ptr->TempRefMinus40 = GetPrivateProfileInt(tag, key, 
//										ptr->TempRefMinus40, FileName);

		sprintf(key, KEY_THERMAL_CAL_TEMPREF_SLOPE_FORMAT, BadnID, i);
			ptr->TempSlope = GetPrivateProfileInt(tag, key,
										ptr->TempSlope, FileName);
		if(ptr->TempSlope == (char)0xff)//jklchen fill 0xff to memory if no data
		{
			ptr->BandID=(char)0xff;
			ptr->SubBand=(char)0xff;			
		};	
		
		sprintf(key, KEY_THERMAL_CAL_CALINTERVALINMS, BadnID, i);
			ptr->TempCalTime = GetPrivateProfileInt(tag, key,
										ptr->TempCalTime, FileName);	
		sprintf(key, KEY_THERMAL_CAL_RSSICALNUM_BYPASS, BadnID, i);
			ptr->RSSICalNum_bypass = GetPrivateProfileInt(tag, key,
										ptr->RSSICalNum_bypass, FileName);
		//TempRef85
//		sprintf(key, KEY_THERMAL_CAL_TEMPREF_FORMAT_85, BadnID, i);
//			ptr->TempRef85 = GetPrivateProfileInt(tag, key,
//										ptr->TempRef85, FileName);

		//Numerator; //SLOPENUM=190  
		sprintf(key, KEY_THERMAL_CAL_SLOPENUM, BadnID, i);
			ptr->Numerator_hot = GetPrivateProfileInt(tag, key,
										ptr->Numerator_hot, FileName);
		//Denom;			//SLOPEDENOM=50
		sprintf(key, KEY_THERMAL_CAL_SLOPEDENOM, BadnID, i);
			ptr->Denom = GetPrivateProfileInt(tag, key,
										ptr->Denom, FileName);
		//PPAConefficient; //PPACOEFF=1
		sprintf(key, KEY_THERMAL_CAL_PPACOEFF, BadnID, i);
			ptr->PPAConefficient = GetPrivateProfileInt(tag, key,
										ptr->PPAConefficient, FileName);
		//PAConefficient; //PACOEFF=1
		sprintf(key, KEY_THERMAL_CAL_PACOEFF, BadnID, i);
			ptr->PAConefficient = GetPrivateProfileInt(tag, key,
										ptr->PAConefficient, FileName);

		//XTAL; //XTAL =0
		sprintf(key, KEY_THERMAL_CAL_XTAL, BadnID, i);
			ptr->XTAL = GetPrivateProfileInt(tag, key,
										ptr->XTAL, FileName);
		//PTargetDelta; //PTARGET_TEMP=0x20
		sprintf(key, KEY_THERMAL_CAL_PTARGET_TEMP, BadnID, i);
			ptr->PTargetDelta = GetPrivateProfileInt(tag, key,
										ptr->PTargetDelta, FileName);
		//Numerator1; //SLOPENUM2=200
		sprintf(key, KEY_THERMAL_CAL_SLOPENUM2, BadnID, i);
			ptr->Numerator_cold = GetPrivateProfileInt(tag, key,
										ptr->Numerator_cold, FileName);
		ptr ++;;
	}
	/*
	for (i=0; i<(*pNoOfSubBand); i++)
	{  
		sprintf(key, KEY_THERMAL_CAL_BAND_FORMAT, i); 
 		if(pBandList)
			pBandList[i] = GetPrivateProfileInt(tag, key, 
											pBandList[i], FileName); 

		sprintf(key, KEY_THERMAL_CAL_SUBBAND_FORMAT, i); 
 		if(pSubBandList)
			pSubBandList[i] = GetPrivateProfileInt(tag, key, 
											pSubBandList[i], FileName); 

		sprintf(key, KEY_THERMAL_CAL_TEMPREF_MINUS40C_FORMAT, i); 
 		if(pTempRefMinus40CList)
			pTempRefMinus40CList[i] = GetPrivateProfileInt(tag, key, 
											pTempRefMinus40CList[i], FileName); 
 
		sprintf(key, KEY_THERMAL_CAL_TEMPREF_0C_FORMAT, i); 
 		if(pTempRef0CList)
			pTempRef0CList[i] = GetPrivateProfileInt(tag, key, 
											pTempRef0CList[i], FileName); 
 
		sprintf(key, KEY_THERMAL_CAL_TEMPREF_65C_FORMAT, i); 
 		if(pTempRef65CList)
			pTempRef65CList[i] = GetPrivateProfileInt(tag, key, 
											pTempRef65CList[i], FileName); 

		sprintf(key, KEY_THERMAL_CAL_TEMPREF_85C_FORMAT, i); 
 		if(pTempRef85CList)
			pTempRef85CList[i] = GetPrivateProfileInt(tag, key, 
											pTempRef85CList[i], FileName); 
 
		sprintf(key, KEY_THERMAL_CAL_PPA_COEFF_FORMAT, i); 
 		if(pPPACoeffList)
			pPPACoeffList[i] = GetPrivateProfileInt(tag, key, 
											pPPACoeffList[i], FileName); 
 
		sprintf(key, KEY_THERMAL_CAL_PA_COEFF_FORMAT, i); 
 		if(pPACoeffList)
			pPACoeffList[i] = GetPrivateProfileInt(tag, key, 
											pPACoeffList[i], FileName); 

		sprintf(key, KEY_THERMAL_CAL_NUMERATOR_FORMAT, i); 
 		if(pNumeratorList)
			pNumeratorList[i] = GetPrivateProfileInt(tag, key, 
											pNumeratorList[i], FileName); 

		sprintf(key, KEY_THERMAL_CAL_NUMERATOR1_FORMAT, i); 
 		if(pNumerator1List)
			pNumerator1List[i] = GetPrivateProfileInt(tag, key, 
											pNumerator1List[i], FileName); 
		sprintf(key, KEY_THERMAL_CAL_DENOM_FORMAT, i); 
 		if(pDenomList)
			pDenomList[i] = GetPrivateProfileInt(tag, key, 
											pDenomList[i], FileName); 
		sprintf(key, KEY_THERMAL_CAL_PTARGETDELTA_FORMAT, i); 
 		if(pPTargetDeltaList)
			pPTargetDeltaList[i] = GetPrivateProfileInt(tag, key, 
											pPTargetDeltaList[i], FileName); 
	} 
	*/
	return status;

}
int DutSharedClssHeader WriteAnnex49DataToFile(char *FileName,
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
					BYTE* pwrTbl)
{
	//header
	char tag[MAX_LENGTH]="";
	char value[MAX_LENGTH]="";
	char key[MAX_LENGTH]="";

	DebugLogRite("File Name :%s\n", FileName);
#if defined (_MIMO_)
 	sprintf(tag, TAG_ANNEX14_FORMAT, DeviceId, PathId); 
#else  //#if defined (_MIMO_)
 	sprintf(tag, TAG_ANNEX14); 
#endif //#if defined (_MIMO_)
 
	//header
	if(pNumOfTxPowerLevelStore)
		sprintf(value, "0x%X", (*pNumOfTxPowerLevelStore));
	else
		sprintf(value, "0x00");

	WritePrivateProfileString(tag, KEY_NO_POWER_LEVEL_SAVED, value, FileName);	

	if(pCalXtal)
		sprintf(value, "0x%X", (*pCalXtal));
	else
		sprintf(value, "0x00");

	WritePrivateProfileString(tag, KEY_RFXTAL, value, FileName);

	if(pCalMethod)
		sprintf(value, "0x%X", (*pCalMethod));
	else
		sprintf(value, "0x00");
 	WritePrivateProfileString(tag, KEY_CAL_OPTION_METHOD, value, FileName);
	
	if(pCloseTXPWDetection)
			sprintf(value, "0x%X", (*pCloseTXPWDetection));
	else
		sprintf(value, "0x00");
 	WritePrivateProfileString(tag, KEY_CLOSE_TX_PW_DET, value, FileName);
	
	if(pExtPAPolarize)
				sprintf(value, "%d", (*pExtPAPolarize));
	else
		sprintf(value, "0x00");
 	WritePrivateProfileString(tag, KEY_EXT_PA_POLARIZE, value, FileName);

	if(pNumOfSavedCh)
		sprintf(value, "0x%X", (*pNumOfSavedCh));
	else
		sprintf(value, "0x00");
	WritePrivateProfileString(tag, KEY_NO_CH_SAVED, value, FileName);

    if(pPdetHiLoGap)
		sprintf(value, "0x%X", (*pPdetHiLoGap));
	else
		sprintf(value, "0x00");
	WritePrivateProfileString(tag, KEY_PDET_HI_LO_GAP, value, FileName);

	{
	 	int noChSaved= 0;
		if(pNumOfSavedCh) noChSaved = (*pNumOfSavedCh);
	
		for (int i=0; i<*pNumOfSavedCh;i++)
		{
			int j=0;
			if(pChNumList)
				sprintf(value, "0x%X", pChNumList[i]);
			else
				sprintf(value, "0x00");
			sprintf(key, KEY_CHANNEL_CALED_FORMAT, i);
			WritePrivateProfileString(tag, key, value, FileName);


			for (j=0; j<RFPWRRANGE_NUM_REVD;j++)
			{
				if(pCorrInitTrgt_B)
					sprintf(value, "0x%X", pCorrInitTrgt_B[i][j]);
				else
					sprintf(value, "0x00");
				sprintf(key, KEY_RANGE_INITP_FORMAT, i, j);
				WritePrivateProfileString(tag, key, value, FileName);

				if(pCorrPDetTh_B)
					sprintf(value, "0x%X", pCorrPDetTh_B[i][j]);
				else
					sprintf(value, "0x00");
				sprintf(key, KEY_RANGE_CORRECTION_FORMAT, i, j);
				WritePrivateProfileString(tag, key, value, FileName);
			}
			for (j=0; j<RFPWRRANGE_NUM_REVD;j++)
			{
				if(pCorrInitTrgt_G)
					sprintf(value, "0x%X", pCorrInitTrgt_G[i][j]);
				else
					sprintf(value, "0x00");
				sprintf(key, KEY_RANGE_INITP_G_FORMAT, i, j);
				WritePrivateProfileString(tag, key, value, FileName);

				if(pCorrPDetTh_G)
					sprintf(value, "0x%X", pCorrPDetTh_G[i][j]);
				else
					sprintf(value, "0x00");
				sprintf(key, KEY_RANGE_CORRECTION_G_FORMAT, i, j);
				WritePrivateProfileString(tag, key, value, FileName);
			}
		}
	}

	return 0;
}

int DutSharedClssHeader ReadAnnex49DataFromFile(char *FileName,
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
					char pCorrPDetTh_G[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD]
					)		
{
	
 	char value[MAX_LENGTH]="";
	char key[MAX_LENGTH]="";
	char tag[MAX_LENGTH]="";
	int noChSaved=0, i=0;
	int status=0;

	DebugLogRite("File Name :%s\n", FileName);
#if defined (_MIMO_)
 	sprintf(tag, TAG_ANNEX14_FORMAT, DeviceId, PathId); 
#else  //#if defined (_MIMO_)
 	sprintf(tag, TAG_ANNEX14); 
#endif //#if defined (_MIMO_)

	if (!GetPrivateProfileSection(tag, value, MAX_LENGTH, FileName))
		return ERROR_DATANOTEXIST;

	//header
	if(pNumOfTxPowerLevelStore)
		(*pNumOfTxPowerLevelStore) = GetPrivateProfileInt(tag,KEY_NO_POWER_LEVEL_SAVED,
									(*pNumOfTxPowerLevelStore), FileName);
	if(pCalMethod)
		(*pCalMethod) = GetPrivateProfileInt(tag, KEY_CAL_OPTION_METHOD, 
										(*pCalMethod), FileName)?true:false; 
	if(pCloseTXPWDetection)
		(*pCloseTXPWDetection) = GetPrivateProfileInt(tag, KEY_CLOSE_TX_PW_DET, 
										(*pCloseTXPWDetection), FileName)?true:false; 
	
	if(pExtPAPolarize)
        (*pExtPAPolarize) = GetPrivateProfileInt(tag, KEY_EXT_PA_POLARIZE,
										(*pExtPAPolarize),FileName)?true:false;
 	if(pCalXtal)
		(*pCalXtal) = GetPrivateProfileInt(tag, KEY_RFXTAL, 
										(*pCalXtal), FileName); 

	if(pPdetHiLoGap)
		(*pPdetHiLoGap) = GetPrivateProfileInt(tag, KEY_PDET_HI_LO_GAP, 
										(*pPdetHiLoGap), FileName); 

	if(pMCS20MPdetOffset)
		(*pMCS20MPdetOffset) = GetPrivateProfileInt(tag, KEY_MCS20M_PDET_OFFSET, 
										(*pMCS20MPdetOffset), FileName);
	
     if(pMCS20MInitPOffset)
		(*pMCS20MInitPOffset) = GetPrivateProfileInt(tag, KEY_MCS20M_INIT_OFFSET, 
										(*pMCS20MInitPOffset), FileName);
	 if(pMCS40MPdetOffset)
		(*pMCS40MPdetOffset) = GetPrivateProfileInt(tag, KEY_MCS40M_PDET_OFFSET, 
										(*pMCS40MPdetOffset), FileName);
	
     if(pMCS40MInitPOffset)
		(*pMCS40MInitPOffset) = GetPrivateProfileInt(tag, KEY_MCS40M_INIT_OFFSET, 
										(*pMCS40MInitPOffset), FileName);

	if(pNumOfSavedCh)
	{
		(*pNumOfSavedCh) = GetPrivateProfileInt(tag, KEY_NO_CH_SAVED, 
										(*pNumOfSavedCh), FileName); 

		noChSaved = (*pNumOfSavedCh);
	}
	else
		noChSaved = GetPrivateProfileInt(tag, KEY_NO_CH_SAVED, 
										noChSaved, FileName); 

	
		for (i=0; i<noChSaved;i++)
		{
			sprintf(key, KEY_CHANNEL_CALED_FORMAT, i);
			if(pChNumList)
				pChNumList[i] = GetPrivateProfileInt(tag, key, 
										pChNumList[i], FileName); 

			for (int j=0; j<RFPWRRANGE_NUM_REVD;j++)
			{
				sprintf(key, KEY_RANGE_INITP_FORMAT, i, j);
				if(pCorrInitTrgt_B)
					pCorrInitTrgt_B[i][j] = GetPrivateProfileInt(tag, key, 
										pCorrInitTrgt_B[i][j], FileName); 
				sprintf(key, KEY_RANGE_CORRECTION_FORMAT, i, j);
				if(pCorrPDetTh_B)
					pCorrPDetTh_B[i][j] = GetPrivateProfileInt(tag, key, 
										pCorrPDetTh_B[i][j], FileName); 			
                //OFDM 
				sprintf(key, KEY_RANGE_INITP_G_FORMAT, i, j);
				if(pCorrInitTrgt_G)
					pCorrInitTrgt_G[i][j] = GetPrivateProfileInt(tag, key, 
										pCorrInitTrgt_G[i][j], FileName); 
				sprintf(key, KEY_RANGE_CORRECTION_G_FORMAT, i, j);
				if(pCorrPDetTh_G)
					pCorrPDetTh_G[i][j] = GetPrivateProfileInt(tag, key, 
										pCorrPDetTh_G[i][j], FileName);
			}
		}

	return status;
}

int DutSharedClssHeader ReadAnnex52DataFromFile(char *FileName,
#if defined (_MIMO_)
					int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)
					bool *Cal2G,
					bool *Cal5G,
					BYTE *LoLeakageCal2G_I,
					BYTE *LoLeakageCal2G_Q,
					char LoLeakageCal5G_I[NUM_SUBBAND_A_REVD],
					char LoLeakageCal5G_Q[NUM_SUBBAND_A_REVD])
{
	char value[MAX_LENGTH]="";
//	char key[MAX_LENGTH]="";
	char tag[MAX_LENGTH]="";
	int status=0;
#if !defined (_MIMO_)
	int DeviceId=0, PathId=0;		
#endif // #if defined (_MIMO_)

	//header

	DebugLogRite("File Name :%s\n", FileName);
	if (!GetPrivateProfileSection(TAG_ANNEX52, value, MAX_LENGTH, FileName))
		return ERROR_DATANOTEXIST;
    
	 
	sprintf(tag, KEY_CAL_2G_FLAG ,PathId);
	(*Cal2G) = GetPrivateProfileInt(TAG_ANNEX52, 
										tag, 
										(*Cal2G), FileName)?true:false;

	sprintf(tag, KEY_CAL_5G_FLAG ,PathId);
	(*Cal5G) = GetPrivateProfileInt(TAG_ANNEX52, 
										tag, 
										(*Cal5G), FileName)?true:false;

	sprintf(tag, KEY_LO_LEAKAGE_2G_I_FORMAT ,PathId);
	(*LoLeakageCal2G_I) = GetPrivateProfileInt(TAG_ANNEX52, 
										tag, 
										(*LoLeakageCal2G_I), FileName);

	sprintf(tag, KEY_LO_LEAKAGE_2G_Q_FORMAT ,PathId);
	(*LoLeakageCal2G_Q) = GetPrivateProfileInt(TAG_ANNEX52, 
										tag, 
										(*LoLeakageCal2G_Q) , FileName);

	for(int i=0;i<NUM_SUBBAND_A_REVD; i++)
	{
		sprintf(tag, KEY_LO_LEAKAGE_5G_I_FORMAT, i+1 ,PathId);
		(LoLeakageCal5G_I[i]) = GetPrivateProfileInt(TAG_ANNEX52, 
										tag, 
										LoLeakageCal5G_I[i], FileName);

		sprintf(tag, KEY_LO_LEAKAGE_5G_Q_FORMAT, i+1, PathId);

		(LoLeakageCal5G_Q[i]) = GetPrivateProfileInt(TAG_ANNEX52, 
										tag, 
										LoLeakageCal5G_Q[i], FileName);
	}

	return status;

}

int DutSharedClssHeader WriteAnnex50DataToFile(char * FileName,
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
	char	pCorrPDetTh[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD]
	)
{
	char value[MAX_LENGTH];
	char key[MAX_LENGTH];
	char tag[MAX_LENGTH]="";
	int i=0;
#if !defined (_MIMO_)
	int DeviceId=0, PathId=0; 
#endif //#if defined (_MIMO_)

	DebugLogRite("File Name :%s\n", FileName);

	sprintf(tag, TAG_ANNEX50_FORMAT, DeviceId, PathId, SubBandId); 
 
	//header
    if(pNumOfTxPowerLevelStore)
		sprintf(value, "%d", (*pNumOfTxPowerLevelStore));
	else
		sprintf(value, "0");
	WritePrivateProfileString(tag, KEY_NO_POWER_LEVEL_SAVED_A, value, FileName);

	if(pCalXtal)
		sprintf(value, "0x%X", (*pCalXtal));
	else
		sprintf(value, "0x00");
	WritePrivateProfileString(tag, KEY_RFXTAL, value, FileName);


	if(pCloseTXPWDetection)
		sprintf(value, "0x%X", (*pCloseTXPWDetection));
	else
		sprintf(value, "0x00");
	WritePrivateProfileString(tag, KEY_CLOSE_TX_PW_DET, value, FileName);

	if(pExtPAPolarize)
		sprintf(value, "0x%X", (*pExtPAPolarize));
	else
		sprintf(value, "0x00");
	WritePrivateProfileString(tag, KEY_EXT_PA_POLARIZE, value, FileName);
   

	if(pCalMethod)
		sprintf(value, "0x%X", (*pCalMethod));
	else
		sprintf(value, "0x00");
	WritePrivateProfileString(tag, KEY_CAL_OPTION_METHOD, value, FileName);

	if(pNumOfSavedChInSubBand)
		sprintf(value, "%d", (*pNumOfSavedChInSubBand));
	else
		sprintf(value, "0x00");
	WritePrivateProfileString(tag, KEY_NO_CH_SAVED	, value, FileName);

  
    if(pPdetHiLoGap)
		sprintf(value, "0x%X", (*pPdetHiLoGap));
	else
		sprintf(value, "0x00");
	WritePrivateProfileString(tag, KEY_PDET_HI_LO_GAP, value, FileName);

	if(pMCS20MPdetOffset)
		sprintf(value, "0x%X", (*pMCS20MPdetOffset));
	else
		sprintf(value, "0x00");
	WritePrivateProfileString(tag, KEY_MCS20M_PDET_OFFSET, value, FileName);

	if(pMCS20MInitPOffset)
		sprintf(value, "0x%X", (*pMCS20MInitPOffset));
	else
		sprintf(value, "0x00");
	WritePrivateProfileString(tag, KEY_MCS20M_INIT_OFFSET, value, FileName);

	if(pMCS40MPdetOffset)
		sprintf(value, "0x%X", (*pMCS40MPdetOffset));
	else
		sprintf(value, "0x00");
	WritePrivateProfileString(tag, KEY_MCS40M_PDET_OFFSET, value, FileName);

	if(pMCS40MInitPOffset)
		sprintf(value, "0x%X", (*pMCS40MInitPOffset));
	else
		sprintf(value, "0x00");
	WritePrivateProfileString(tag, KEY_MCS40M_INIT_OFFSET, value, FileName);


//;	if(pFreqBound_Lo)
//;		sprintf(value, "%d", (*pFreqBound_Lo));
//;	else
//;		sprintf(value, "0");
//;	WritePrivateProfileString(tag, KEY_HI_FREQ, value, FileName);



/*
	if(pCalRange)
	{
		for (i=0; i<RFPWRRANGE_NUM; i++)
		{
		sprintf(key, KEY_CAL_OPTION_RANGE_FORMAT, i);
		sprintf(value, "0x%X", pCalRange[i]);
		WritePrivateProfileString(tag, key, value, FileName);
		}
	}
	else
	{
		for (i=0; i<RFPWRRANGE_NUM; i++)
		{
		sprintf(key, KEY_CAL_OPTION_RANGE_FORMAT, i);
		sprintf(value, "0x00");
		WritePrivateProfileString(tag, key, value, FileName);
		}
	}
*/ 

/*	
	if(pThermalScaler)
		sprintf(value, "0x%X", (*pThermalScaler));
	else
		sprintf(value, "0x00");
	WritePrivateProfileString(tag, KEY_THERMALSCALER, value, FileName);

#if defined (_MIMO_)
	if(pInitPCommon)
		sprintf(value, "0x%X", (*pInitPCommon));
	else
		sprintf(value, "0x00");
	WritePrivateProfileString(tag, KEY_INITPCOMMON, value, FileName);
#endif // #if defined (_MIMO_)
*/

/*	
	if(pNumOfSavedChInSubBand)
	{
		for (i=0; i<SUBBAND_NUM_A; i++)
		{
			sprintf(key, KEY_NO_CH_SUBBAND_FORMAT, i);
			sprintf(value, "0x%X", pSavedChannelIndex[i]);
  			WritePrivateProfileString(tag, key, value, FileName);
		}
	}
	else
	{
		for (i=0; i<SUBBAND_NUM_A; i++)
		{
			sprintf(key, KEY_NO_CH_SUBBAND_FORMAT, i);
			sprintf(value, "0x00");
  			WritePrivateProfileString(tag, key, value, FileName);
		}
	}
 */
	{
//;		int chSaved = 0;

//;		if(pNumOfSavedCh)
//;			for (i=0; i<SUBBAND_NUM_A; i++)
//;				chSaved +=	pSavedChannelIndex[i];
		 
		for (int i=0; i<(*pNumOfSavedChInSubBand);i++)
		{
			if(pSavedChannelIndex)
				sprintf(value, "%d", pSavedChannelIndex[i]);
			else
				sprintf(value, "0x00");
			sprintf(key, KEY_CHANNEL_CALED_FORMAT, i);
			WritePrivateProfileString(tag, key, value, FileName);

		
			for (int j=0; j<RFPWRRANGE_NUM_REVD;j++)
			{
				if(pCorrPDetTh)
					sprintf(value, "0x%X", pCorrPDetTh[i][j]);
				else
					sprintf(value, "0x00");
				sprintf(key, KEY_RANGE_CORRECTION_A_FORMAT, i, j);
				WritePrivateProfileString(tag, key, value, FileName);

				if(pCorrInitTrgt)
					sprintf(value, "0x%X",pCorrInitTrgt[i][j]);
				else
					sprintf(value, "0x00");
				sprintf(key, KEY_RANGE_INITP_A_FORMAT, i, j);
				WritePrivateProfileString(tag, key, value, FileName);

			
			}
		}
		


/*
		for (i=0; i<NUM_SUBBAND_A; i++)
		{
			if(pIQMis20_Amp)
				sprintf(value, "0x%X", pIQMis20_Amp[i]);
			else
				sprintf(value, "0x00");
			sprintf(key, KEY_TXIQMISMATCH_AMP_FORMAT,  i, 20);
			WritePrivateProfileString(tag, key, value, FileName);

			if(pIQMis20_Phase)
				sprintf(value, "0x%X", pIQMis20_Phase[i]);
			else
				sprintf(value, "0x00");
			sprintf(key, KEY_TXIQMISMATCH_PHASE_FORMAT, i, 20);
			WritePrivateProfileString(tag, key, value, FileName);

			if(pIQMis10_Amp)
				sprintf(value, "0x%X", pIQMis10_Amp[i]);
			else
				sprintf(value, "0x00");
			sprintf(key, KEY_TXIQMISMATCH_AMP_FORMAT,  i, 10);
			WritePrivateProfileString(tag, key, value, FileName);

			if(pIQMis10_Phase)
				sprintf(value, "0x%X", pIQMis10_Phase[i]);
			else
				sprintf(value, "0x00");
			sprintf(key, KEY_TXIQMISMATCH_PHASE_FORMAT, i, 10);
			WritePrivateProfileString(tag, key, value, FileName);

			if(pIQMis5_Amp)
				sprintf(value, "0x%X", pIQMis5_Amp[i]);
			else
				sprintf(value, "0x00");
			sprintf(key, KEY_TXIQMISMATCH_AMP_FORMAT,  i, 5);
			WritePrivateProfileString(tag, key, value, FileName);

			if(pIQMis5_Phase)
				sprintf(value, "0x%X", pIQMis5_Phase[i]);
			else
				sprintf(value, "0x00");
			sprintf(key, KEY_TXIQMISMATCH_PHASE_FORMAT, i, 5);
			WritePrivateProfileString(tag, key, value, FileName);

			if(pIQMis40_Amp)
				sprintf(value, "0x%X", pIQMis40_Amp[i]);
			else
				sprintf(value, "0x00");
			sprintf(key, KEY_TXIQMISMATCH_AMP_FORMAT,  i, 40);
			WritePrivateProfileString(tag, key, value, FileName);

			if(pIQMis40_Phase)
				sprintf(value, "0x%X", pIQMis40_Phase[i]);
			else
				sprintf(value, "0x00");
			sprintf(key, KEY_TXIQMISMATCH_PHASE_FORMAT, i, 40);
			WritePrivateProfileString(tag, key, value, FileName);
		}
*/
	}
       

	return 0;
}
int DutSharedClssHeader ReadAnnex50DataFromFile(char *FileName,
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
	char	pCorrPDetTh[MAXSAVEDCH_G_REVD][RFPWRRANGE_NUM_REVD]
	)
{

		char value[MAX_LENGTH];
	char key[MAX_LENGTH];
	char tag[MAX_LENGTH]="";
	int i=0;

	DebugLogRite("File Name :%s\n", FileName);
#if defined (_MIMO_)
 	sprintf(tag, TAG_ANNEX50_FORMAT, DeviceId, PathId, SubBandId); 
#else  //#if defined (_MIMO_)
 	sprintf(tag, TAG_ANNEX15); 
#endif //#if defined (_MIMO_)

 	if (!GetPrivateProfileSection(tag, value, MAX_LENGTH, FileName))
		return ERROR_DATANOTEXIST;

	//header
 	if(pCalXtal)
		(*pCalXtal) = GetPrivateProfileInt(tag, KEY_RFXTAL, 
										(*pCalXtal), FileName); 

 
 	if(pCalMethod)
		(*pCalMethod) = GetPrivateProfileInt(tag, KEY_CAL_OPTION_METHOD, 
										(*pCalMethod), FileName)?true:false; 

	if(pCloseTXPWDetection)
		(*pCloseTXPWDetection) = GetPrivateProfileInt(tag, KEY_CLOSE_TX_PW_DET, 
										(*pCloseTXPWDetection), FileName)?true:false; 
	
	if(pExtPAPolarize)
        (*pExtPAPolarize) = GetPrivateProfileInt(tag, KEY_EXT_PA_POLARIZE,
										(*pExtPAPolarize),FileName)?true:false;

	if(pPdetHiLoGap)
		(*pPdetHiLoGap) = GetPrivateProfileInt(tag, KEY_PDET_HI_LO_GAP, 
										(*pPdetHiLoGap), FileName); 

	if(pMCS20MPdetOffset)
		(*pMCS20MPdetOffset) = GetPrivateProfileInt(tag, KEY_MCS20M_PDET_OFFSET, 
										(*pMCS20MPdetOffset), FileName);
	
     if(pMCS20MInitPOffset)
		(*pMCS20MInitPOffset) = GetPrivateProfileInt(tag, KEY_MCS20M_INIT_OFFSET, 
										(*pMCS20MInitPOffset), FileName);
	 if(pMCS40MPdetOffset)
		(*pMCS40MPdetOffset) = GetPrivateProfileInt(tag, KEY_MCS40M_PDET_OFFSET, 
										(*pMCS40MPdetOffset), FileName);
	
     if(pMCS40MInitPOffset)
		(*pMCS40MInitPOffset) = GetPrivateProfileInt(tag, KEY_MCS40M_INIT_OFFSET, 
										(*pMCS40MInitPOffset), FileName);


 	if(pNumOfSavedCh)
		(*pNumOfSavedCh) = GetPrivateProfileInt(tag, KEY_NO_CH_SAVED, 
										(*pNumOfSavedCh), FileName); 
	(*pNumOfSavedChInSubBand)=(*pNumOfSavedCh);

	if(pNumOfTxPowerLevelStore)
		(*pNumOfTxPowerLevelStore) = GetPrivateProfileInt(tag,KEY_NO_POWER_LEVEL_SAVED,
									(*pNumOfTxPowerLevelStore), FileName);
 
 
	{
		int chSaved = 0;
		
		for (int i=0; i<(*pNumOfSavedCh);i++)
		{
			sprintf(key, KEY_CHANNEL_CALED_FORMAT, i);
 			if(pSavedChannelIndex)
				pSavedChannelIndex[i] = GetPrivateProfileInt(tag, key, 
												pSavedChannelIndex[i], FileName);
		
			for (int j=0; j<RFPWRRANGE_NUM_REVD;j++)
			{
				sprintf(key, KEY_RANGE_INITP_A_FORMAT, i, j);
 				if(pCorrInitTrgt)
					pCorrInitTrgt[i][j] = GetPrivateProfileInt(tag, key, 
												pCorrInitTrgt[i][j], FileName);

				sprintf(key, KEY_RANGE_CORRECTION_A_FORMAT, i, j);
  				if(pCorrPDetTh)
					pCorrPDetTh[i][j] = GetPrivateProfileInt(tag, key, 
												pCorrPDetTh[i][j], FileName);
			}
		}
		


/*
		for (i=0; i<NUM_SUBBAND_A; i++)
		{
			sprintf(key, KEY_TXIQMISMATCH_AMP_FORMAT,  i, 20);
  			if(pIQMis20_Amp)
				pIQMis20_Amp[i] = GetPrivateProfileInt(tag, key, 
											pIQMis20_Amp[i], FileName);

			sprintf(key, KEY_TXIQMISMATCH_PHASE_FORMAT, i, 20);
  			if(pIQMis20_Phase)
				pIQMis20_Phase[i] = GetPrivateProfileInt(tag, key, 
											pIQMis20_Phase[i], FileName);

			sprintf(key, KEY_TXIQMISMATCH_AMP_FORMAT,  i, 10);
    		if(pIQMis10_Amp)
				pIQMis10_Amp[i] = GetPrivateProfileInt(tag, key, 
											pIQMis10_Amp[i], FileName);

			sprintf(key, KEY_TXIQMISMATCH_PHASE_FORMAT, i, 10);
   			if(pIQMis10_Phase)
				pIQMis10_Phase[i] = GetPrivateProfileInt(tag, key, 
											pIQMis10_Phase[i], FileName);

			sprintf(key, KEY_TXIQMISMATCH_AMP_FORMAT,  i, 5);
     		if(pIQMis5_Amp)
				pIQMis5_Amp[i] = GetPrivateProfileInt(tag, key, 
											pIQMis5_Amp[i], FileName);

			sprintf(key, KEY_TXIQMISMATCH_PHASE_FORMAT, i, 5);
    		if(pIQMis5_Phase)
				pIQMis5_Phase[i] = GetPrivateProfileInt(tag, key, 
											pIQMis5_Phase[i], FileName);

			sprintf(key, KEY_TXIQMISMATCH_AMP_FORMAT,  i, 40);
      		if(pIQMis40_Amp)
				pIQMis40_Amp[i] = GetPrivateProfileInt(tag, key, 
											pIQMis40_Amp[i], FileName);

			sprintf(key, KEY_TXIQMISMATCH_PHASE_FORMAT, i, 40);
     		if(pIQMis40_Phase)
				pIQMis40_Phase[i] = GetPrivateProfileInt(tag, key, 
											pIQMis40_Phase[i], FileName);
		}
		*/
	}

	return 0;
}

int DutSharedClssHeader WriteAnnex52DataToFile(char *FileName,
#if defined (_MIMO_)
					int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)
					bool Cal2G,
					bool Cal5G,
 					BYTE  LoLeakageCal2G_I, 
					BYTE  LoLeakageCal2G_Q,
					char LoLeakageCal5G_I[NUM_SUBBAND_A],
					char LoLeakageCal5G_Q[NUM_SUBBAND_A])
{
	char value[MAX_LENGTH]="";
//	char key[MAX_LENGTH]="";
	char tag[MAX_LENGTH]="";
	int status=0;
#if !defined (_MIMO_)
	int DeviceId=0, PathId=0;	
#endif // #if !defined (_MIMO_)
 
	//header

	DebugLogRite("File Name :%s\n", FileName);
	

	if(Cal2G)
			sprintf(value, "%d", (Cal2G)); 
		else
			sprintf(value, "0x00");
		sprintf(tag, KEY_CAL_2G_FLAG ,PathId);
	WritePrivateProfileString(TAG_ANNEX52, tag, value,  FileName);

	if(Cal5G)
			sprintf(value, "%d", (Cal5G)); 
		else
			sprintf(value, "0x00");
		sprintf(tag, KEY_CAL_5G_FLAG ,PathId);
	WritePrivateProfileString(TAG_ANNEX52, tag, value,  FileName);
	if(LoLeakageCal2G_I)
			sprintf(value, "0x%X", (LoLeakageCal2G_I)); 
		else
			sprintf(value, "0x00");
		sprintf(tag, KEY_LO_LEAKAGE_2G_I_FORMAT ,PathId);
		WritePrivateProfileString(TAG_ANNEX52, tag, value,  FileName);


	if(LoLeakageCal2G_Q)
			sprintf(value, "0x%X", (LoLeakageCal2G_Q)); 
		else
			sprintf(value, "0x00");
		sprintf(tag, KEY_LO_LEAKAGE_2G_Q_FORMAT ,PathId);
		WritePrivateProfileString(TAG_ANNEX52, tag, value,  FileName);
	if(LoLeakageCal5G_I)
	{
		for(int i=0;i<NUM_SUBBAND_A_REVD; i++)
		{
			sprintf(value, "0x%X", (LoLeakageCal5G_I[i])); 
			sprintf(tag, KEY_LO_LEAKAGE_5G_I_FORMAT, i+1 ,PathId);
			WritePrivateProfileString(TAG_ANNEX52, tag, value,  FileName);

			sprintf(value, "0x%X", (LoLeakageCal5G_Q[i])); 
			sprintf(tag, KEY_LO_LEAKAGE_5G_Q_FORMAT, i+1 ,PathId);
			WritePrivateProfileString(TAG_ANNEX52, tag, value,  FileName);
		}
	}
	return status;	
}
int DutSharedClssHeader WriteAnnex53DataToFile(char *FileName,
#if defined (_MIMO_)
				   int DeviceId,		int PathId,  
#endif //#if defined (_MIMO_)
					bool Cal2G,
					bool Cal5G,
					char TxIQmismatch_Amp_2G,	char TxIQmismatch_Phase_2G,
					char TxIQmismatch_Amp_5G[NUM_SUBBAND_A_REVD],	char TxIQmismatch_Phase_5G[NUM_SUBBAND_A_REVD])
{ 
	char value[MAX_LENGTH]="";
//	char key[MAX_LENGTH]="";
	char tag[MAX_LENGTH]="";
	int status=0;
#if !defined (_MIMO_)
	int DeviceId=0, PathId=0;	
#endif // #if !defined (_MIMO_)

	//header

	DebugLogRite("File Name :%s\n", FileName);
	

	if(Cal2G)
			sprintf(value, "%d", (Cal2G)); 
		else
			sprintf(value, "0x00");
		sprintf(tag, KEY_CAL_2G_FLAG ,PathId);
	WritePrivateProfileString(TAG_ANNEX53, tag, value,  FileName);

	if(Cal5G)
			sprintf(value, "%d", (Cal5G)); 
		else
			sprintf(value, "0x00");
		sprintf(tag, KEY_CAL_5G_FLAG ,PathId);
	WritePrivateProfileString(TAG_ANNEX53, tag, value,  FileName);

	if(TxIQmismatch_Amp_2G)
			sprintf(value, "0x%X", (TxIQmismatch_Amp_2G)); 
		else
			sprintf(value, "0x00");
		sprintf(tag, KEY_TXIQ_LEAKAGE_2G_ALPHA_FORMAT ,PathId);
		WritePrivateProfileString(TAG_ANNEX53, tag, value,  FileName);


	if(TxIQmismatch_Phase_2G)
			sprintf(value, "0x%X", (TxIQmismatch_Phase_2G)); 
		else
			sprintf(value, "0x00");
		sprintf(tag, KEY_TXIQ_LEAKAGE_2G_BETA_FORMAT ,PathId);
		WritePrivateProfileString(TAG_ANNEX53, tag, value,  FileName);
	if(TxIQmismatch_Amp_5G && TxIQmismatch_Phase_5G)
	{
		for(int i=0;i<NUM_SUBBAND_A_REVD; i++)
		{
			sprintf(value, "0x%X", (TxIQmismatch_Amp_5G[i])); 
			sprintf(tag, KEY_TXIQ_LEAKAGE_5G_ALPHA_FORMAT, i+1 ,PathId);
			WritePrivateProfileString(TAG_ANNEX53, tag, value,  FileName);

			sprintf(value, "0x%X", (TxIQmismatch_Phase_5G[i])); 
			sprintf(tag, KEY_TXIQ_LEAKAGE_5G_BETA_FORMAT, i+1 ,PathId);
			WritePrivateProfileString(TAG_ANNEX53, tag, value,  FileName);
		}
	}
	return status;
}

int DutSharedClssHeader ReadAnnex53DataFromFile(char *FileName,
#if defined (_MIMO_)
				   int DeviceId,		int PathId,  //int *CsC, 
#endif //#if defined (_MIMO_)
					bool *Cal2G,
					bool *Cal5G,
					char *TxIQmismatch_Amp_2G,	char *TxIQmismatch_Phase_2G,
					char TxIQmismatch_Amp_5G[NUM_SUBBAND_A_REVD],	char TxIQmismatch_Phase_5G[NUM_SUBBAND_A_REVD])
{
	char value[MAX_LENGTH]="";
//	char key[MAX_LENGTH]="";
	char tag[MAX_LENGTH]="";
	int status=0;
 #if !defined (_MIMO_)
	int DeviceId=0, PathId=0; 
#endif //#if defined (_MIMO_)

	//header

	DebugLogRite("File Name :%s\n", FileName);
	if (!GetPrivateProfileSection(TAG_ANNEX53, value, MAX_LENGTH, FileName))
		return ERROR_DATANOTEXIST;
    
	 
	sprintf(tag, KEY_CAL_2G_FLAG ,PathId);
	(*Cal2G) = GetPrivateProfileInt(TAG_ANNEX53, 
										tag, 
										(*Cal2G), FileName)?true:false;

	sprintf(tag, KEY_CAL_5G_FLAG ,PathId);
	(*Cal5G) = GetPrivateProfileInt(TAG_ANNEX53, 
										tag, 
										(*Cal5G), FileName)?true:false;

	sprintf(tag, KEY_TXIQ_LEAKAGE_2G_ALPHA_FORMAT ,PathId);
	(*TxIQmismatch_Amp_2G) = GetPrivateProfileInt(TAG_ANNEX53, 
										tag, 
										(*TxIQmismatch_Amp_2G), FileName);

	sprintf(tag, KEY_TXIQ_LEAKAGE_2G_BETA_FORMAT ,PathId);
	(*TxIQmismatch_Phase_2G) = GetPrivateProfileInt(TAG_ANNEX53, 
										tag, 
										(*TxIQmismatch_Phase_2G) , FileName);

	for(int i=0;i<NUM_SUBBAND_A_REVD; i++)
	{
		sprintf(tag, KEY_TXIQ_LEAKAGE_5G_ALPHA_FORMAT, i+1 ,PathId);
		(TxIQmismatch_Amp_5G[i]) = GetPrivateProfileInt(TAG_ANNEX53, 
										tag, 
										TxIQmismatch_Amp_5G[i], FileName);

		sprintf(tag, KEY_TXIQ_LEAKAGE_5G_BETA_FORMAT, i+1, PathId);

		(TxIQmismatch_Phase_5G[i]) = GetPrivateProfileInt(TAG_ANNEX53, 
										tag, 
										TxIQmismatch_Phase_5G[i], FileName);
	}

	return status;
}
//Annex 54
int DutSharedClssHeader WriteAnnex54DataToFile(char *FileName,
#if defined (_MIMO_)
				   int DeviceId,		int PathId,  //int *CsC, 
#endif //#if defined (_MIMO_)
					bool Cal2G,
					bool Cal5G,
					char RxIQmismatch_Amp_2G,	char RxIQmismatch_Phase_2G,
					char RxIQmismatch_Amp_5G[NUM_SUBBAND_A_REVD],	char RxIQmismatch_Phase_5G[NUM_SUBBAND_A_REVD])
{ 
	char value[MAX_LENGTH]="";
//	char key[MAX_LENGTH]="";
	char tag[MAX_LENGTH]="";
	int status=0;
#if !defined (_MIMO_)
	int DeviceId=0, PathId=0; 
#endif //#if defined (_MIMO_)

	//header

	DebugLogRite("File Name :%s\n", FileName);
	

	if(Cal2G)
			sprintf(value, "%d", (Cal2G)); 
		else
			sprintf(value, "0x00");
		sprintf(tag, KEY_CAL_2G_FLAG ,PathId);
	WritePrivateProfileString(TAG_ANNEX54, tag, value,  FileName);

	if(Cal5G)
			sprintf(value, "%d", (Cal5G)); 
		else
			sprintf(value, "0x00");
		sprintf(tag, KEY_CAL_5G_FLAG ,PathId);
	WritePrivateProfileString(TAG_ANNEX54, tag, value,  FileName);

	if(RxIQmismatch_Amp_2G)
			sprintf(value, "0x%X", (RxIQmismatch_Amp_2G)); 
		else
			sprintf(value, "0x00");
		sprintf(tag, KEY_RXIQ_LEAKAGE_2G_ALPHA_FORMAT ,PathId);
		WritePrivateProfileString(TAG_ANNEX54, tag, value,  FileName);


	if(RxIQmismatch_Phase_2G)
			sprintf(value, "0x%X", (RxIQmismatch_Phase_2G)); 
		else
			sprintf(value, "0x00");
		sprintf(tag, KEY_RXIQ_LEAKAGE_2G_BETA_FORMAT ,PathId);
		WritePrivateProfileString(TAG_ANNEX54, tag, value,  FileName);
	if(RxIQmismatch_Amp_5G && RxIQmismatch_Phase_5G)
	{
		for(int i=0;i<NUM_SUBBAND_A_REVD; i++)
		{
			sprintf(value, "0x%X", (RxIQmismatch_Amp_5G[i])); 
			sprintf(tag, KEY_RXIQ_LEAKAGE_5G_ALPHA_FORMAT, i+1 ,PathId);
			WritePrivateProfileString(TAG_ANNEX54, tag, value,  FileName);

			sprintf(value, "0x%X", (RxIQmismatch_Phase_5G[i])); 
			sprintf(tag, KEY_RXIQ_LEAKAGE_5G_BETA_FORMAT, i+1 ,PathId);
			WritePrivateProfileString(TAG_ANNEX54, tag, value,  FileName);
		}
	}
	return status;
}

int DutSharedClssHeader ReadAnnex54DataFromFile(char *FileName,
#if defined (_MIMO_)
				   int DeviceId,		int PathId,  //int *CsC, 
#endif //#if defined (_MIMO_)
					bool *Cal2G,
					bool *Cal5G,
					char *RxIQmismatch_Amp_2G,	char *RxIQmismatch_Phase_2G,
					char RxIQmismatch_Amp_5G[NUM_SUBBAND_A_REVD],	char RxIQmismatch_Phase_5G[NUM_SUBBAND_A_REVD])
{
	char value[MAX_LENGTH]="";
//	char key[MAX_LENGTH]="";
	char tag[MAX_LENGTH]="";
	int status=0;
#if !defined (_MIMO_)
	int DeviceId=0, PathId=0; 
#endif //#if defined (_MIMO_)
 
	//header

	DebugLogRite("File Name :%s\n", FileName);
	if (!GetPrivateProfileSection(TAG_ANNEX54, value, MAX_LENGTH, FileName))
		return ERROR_DATANOTEXIST;
    
	 
	sprintf(tag, KEY_CAL_2G_FLAG ,PathId);
	(*Cal2G) = GetPrivateProfileInt(TAG_ANNEX54, 
										tag, 
										(*Cal2G), FileName)?true:false;

	sprintf(tag, KEY_CAL_5G_FLAG ,PathId);
	(*Cal5G) = GetPrivateProfileInt(TAG_ANNEX54, 
										tag, 
										(*Cal5G), FileName)?true:false;

	sprintf(tag, KEY_RXIQ_LEAKAGE_2G_ALPHA_FORMAT ,PathId);
	(*RxIQmismatch_Amp_2G) = GetPrivateProfileInt(TAG_ANNEX54, 
										tag, 
										(*RxIQmismatch_Amp_2G), FileName);

	sprintf(tag, KEY_RXIQ_LEAKAGE_2G_BETA_FORMAT ,PathId);
	(*RxIQmismatch_Phase_2G) = GetPrivateProfileInt(TAG_ANNEX54, 
										tag, 
										(*RxIQmismatch_Phase_2G) , FileName);

	for(int i=0;i<NUM_SUBBAND_A_REVD; i++)
	{
		sprintf(tag, KEY_RXIQ_LEAKAGE_5G_ALPHA_FORMAT, i+1 ,PathId);
		(RxIQmismatch_Amp_5G[i]) = GetPrivateProfileInt(TAG_ANNEX54, 
										tag, 
										RxIQmismatch_Amp_5G[i], FileName);

		sprintf(tag, KEY_RXIQ_LEAKAGE_5G_BETA_FORMAT, i+1, PathId);

		(RxIQmismatch_Phase_5G[i]) = GetPrivateProfileInt(TAG_ANNEX54, 
										tag, 
										RxIQmismatch_Phase_5G[i], FileName);
	}

	return status;
}
int DutSharedClssHeader WriteAnnex55DataToFile(char *FileName,
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
					BYTE pEncr_Key_Len_Min
					)
{
	char value[MAX_LENGTH]="";
//	char key[MAX_LENGTH]="";
	char tag[MAX_LENGTH]="";
	int TempInt[SIZE_BDADDRESS_INBYTE]={0};
	char line[256]="00000000000000000000000000000000";
	int status=0;
		DebugLogRite("File Name :%s\n", FileName);
//;	if (!GetPrivateProfileSection(TAG_ANNEX55, value, MAX_LENGTH, FileName))
//;		return ERROR_DATANOTEXIST;


	if(pVersion)
			sprintf(value, "0x%x", (pVersion)); 
		else
			sprintf(value, "0x00");
	WritePrivateProfileString(TAG_ANNEX55, KEY_BT_CONFIG_VERSION, value,  FileName);

	 

	if(pRFXtal)
			sprintf(value, "0x%x", (pRFXtal)); 
		else
			sprintf(value, "0x00");
	WritePrivateProfileString(TAG_ANNEX55, KEY_BT_CONFIG_XTAL, value,  FileName);



	if(pInitPwr)
			sprintf(value, "%d", (pInitPwr)); 
		else
			sprintf(value, "%d", DEFVALUE_BT_INITPWR);
	WritePrivateProfileString(TAG_ANNEX55, KEY_BT_INITPWRINDBM, value,  FileName);


	if(pFELoss)
			sprintf(value, "0x%x", (pFELoss)); 
	else
			sprintf(value, "0x00");
	WritePrivateProfileString(TAG_ANNEX55, KEY_BT_FE_LOSS, value,  FileName);

	if(pForceClass2Op)
			sprintf(value, "%d", (pForceClass2Op)); 
	else
			sprintf(value, "%d", DEFVALUE_BT_FORCECLASS20);
	WritePrivateProfileString(TAG_ANNEX55, KEY_BT_FORCECLASS20, value,  FileName);


	if(pDisablePwrControl)
			sprintf(value, "%d", (pDisablePwrControl)); 
	else
			sprintf(value, "%d", DEFVALUE_BT_DISABLE_PWR_CONTROL);
	WritePrivateProfileString(TAG_ANNEX55, KEY_BT_DISABLE_PWR_CONTROL, value,  FileName);

	if(pMiscFlag)
			sprintf(value, "%d", (pMiscFlag)); 
	else
			sprintf(value, "%d", DEFVALUE_BT_MISC_FLAG);
	WritePrivateProfileString(TAG_ANNEX55, KEY_BT_MISC_FLAG, value,  FileName);


	if(pUsedInternalSleepClock)
			sprintf(value, "%d", (pUsedInternalSleepClock)); 
	else
			sprintf(value, "%d", DEFVALUE_BT_USED_INTERNAL_SLEEP_CLOCK);
	WritePrivateProfileString(TAG_ANNEX55, KEY_BT_USED_INTERNAL_SLEEP_CLOCK, value,  FileName);
	
	if(pRssi_Golden_Lo)
		sprintf(value, "0x%x", (pRssi_Golden_Lo)); 
	else
		sprintf(value, "0x%x", DEFVALUE_BT_RSSIGOLDENLOW);
	WritePrivateProfileString(TAG_ANNEX55, KEY_BT_CONFIG_RSSIGOLDENRANGE_LOW, value,  FileName);
	
	if(pRssi_Golden_Hi)
		sprintf(value, "0x%x", (pRssi_Golden_Hi)); 
	else
		sprintf(value, "0x%x", DEFVALUE_BT_RSSIGOLDENHIGH);
	WritePrivateProfileString(TAG_ANNEX55, KEY_BT_CONFIG_RSSIGOLDENRANGE_HIGH, value,  FileName);
	
	
	if(pBTBAUDRate)
		sprintf(value, "%d", (pBTBAUDRate)); 
	else
		sprintf(value, "%d", DEFVALUE_BT_UARTBAUDRATE);
	WritePrivateProfileString(TAG_ANNEX55, KEY_UART_BAUDRATE, value,  FileName);


	sprintf(line, "%02X.%02X.%02X.%02X.%02X.%02X", 
		pBDAddr[0], pBDAddr[1], pBDAddr[2], 
		pBDAddr[3], pBDAddr[4], pBDAddr[5]); 
	WritePrivateProfileString(TAG_ANNEX55, KEY_BT_CONFIG_BDADDRESS, line, FileName);

	if(pEncr_Key_Len_Min)
		sprintf(value, "0x%x", (pEncr_Key_Len_Min)); 
	else
		sprintf(value, "0x%x", DEFVALUE_BT_ENCRYP_KEY_LEN_MIN);
	WritePrivateProfileString(TAG_ANNEX55, KEY_BT_CONFIG_MIN_ENCRYP_KEY_LEN, value,  FileName);

	if(pEncr_Key_Len_Max)
	   	sprintf(value, "0x%x", (pEncr_Key_Len_Max)); 
	else
		sprintf(value, "0x%x", DEFVALUE_BT_ENCRYP_KEY_LEN_MAX);
	WritePrivateProfileString(TAG_ANNEX55, KEY_BT_CONFIG_MAX_ENCRYP_KEY_LEN, value,  FileName);


	


	return status;
}

int DutSharedClssHeader ReadAnnex55DataFromFile(char *FileName,
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
					BYTE *pEncr_Key_Len_Min)
{
	char value[MAX_LENGTH]="";
	char tag[MAX_LENGTH]="";
	int TempInt[SIZE_BDADDRESS_INBYTE]={0};
	char line[256]="00000000000000000000000000000000";
	int status=0, cnt=0;
 
	//header

	DebugLogRite("File Name :%s\n", FileName);
	if (!GetPrivateProfileSection(TAG_ANNEX55, value, MAX_LENGTH, FileName))
		return ERROR_DATANOTEXIST;

	if(pVersion)
		(*pVersion) = GetPrivateProfileInt(TAG_ANNEX55, 
										KEY_BT_CONFIG_VERSION, 
											(*pVersion), FileName);
	 
	if(pRFXtal)
		(*pRFXtal) = GetPrivateProfileInt(TAG_ANNEX55, 
										KEY_BT_CONFIG_XTAL, 
											(*pRFXtal), FileName);

	if(pInitPwr)
	{	
		(*pInitPwr) = DEFVALUE_BT_INITPWR;
		(*pInitPwr) = GetPrivateProfileInt(TAG_ANNEX55, 
										KEY_BT_INITPWRINDBM, 
											(*pInitPwr), FileName);
	}


	if(pFELoss)
		(*pFELoss) = GetPrivateProfileInt(TAG_ANNEX55, 
										KEY_BT_FE_LOSS, 
											(*pFELoss), FileName);
	if(pForceClass2Op)
	{
		(*pForceClass2Op) = DEFVALUE_BT_FORCECLASS20;
		(*pForceClass2Op) = GetPrivateProfileInt(TAG_ANNEX55, 
										KEY_BT_FORCECLASS20, 
										(*pForceClass2Op), FileName)?true:false;
	}

	if(pDisablePwrControl)
	{
		(*pDisablePwrControl) = DEFVALUE_BT_DISABLE_PWR_CONTROL;
		(*pDisablePwrControl) = GetPrivateProfileInt(TAG_ANNEX55, 
										KEY_BT_DISABLE_PWR_CONTROL, 
											(*pDisablePwrControl), FileName)?true:false;
	}

	if(pMiscFlag)
	{
		(*pMiscFlag) = DEFVALUE_BT_MISC_FLAG;
		(*pMiscFlag) = GetPrivateProfileInt(TAG_ANNEX55, 
										KEY_BT_MISC_FLAG, 
											(*pMiscFlag), FileName)?true:false;
	}
	if(pUsedInternalSleepClock)
	{
		(*pUsedInternalSleepClock) = DEFVALUE_BT_USED_INTERNAL_SLEEP_CLOCK;
		(*pUsedInternalSleepClock) = GetPrivateProfileInt(TAG_ANNEX55, 
										KEY_BT_USED_INTERNAL_SLEEP_CLOCK, 
											(*pUsedInternalSleepClock), FileName)?true:false;
	}
	if(pRssi_Golden_Lo)
	{	
		(*pRssi_Golden_Lo) = DEFVALUE_BT_RSSIGOLDENLOW;
		(*pRssi_Golden_Lo) = GetPrivateProfileInt(TAG_ANNEX55, 
										KEY_BT_CONFIG_RSSIGOLDENRANGE_LOW, 
											(*pRssi_Golden_Lo), FileName);
	}

	if(pRssi_Golden_Hi)
	{
		(*pRssi_Golden_Hi) = DEFVALUE_BT_RSSIGOLDENHIGH;
		(*pRssi_Golden_Hi) = GetPrivateProfileInt(TAG_ANNEX55, 
										KEY_BT_CONFIG_RSSIGOLDENRANGE_HIGH, 
											(*pRssi_Golden_Hi), FileName);
	}

	if(pBTBAUDRate)
	{
		(*pBTBAUDRate) = DEFVALUE_BT_UARTBAUDRATE;
		(*pBTBAUDRate) = GetPrivateProfileInt(TAG_ANNEX55, 
										KEY_UART_BAUDRATE, 
											(*pBTBAUDRate), FileName);
	}

	GetPrivateProfileString(TAG_ANNEX55, KEY_BT_CONFIG_BDADDRESS,"", line, 20, FileName); 
	cnt= sscanf(line, "%02x.%02x.%02x.%02x.%02x.%02x", 
			&TempInt[0],&TempInt[1],&TempInt[2],
			&TempInt[3],&TempInt[4],&TempInt[5]); 

	if(cnt!=SIZE_BDADDRESS_INBYTE) 
	{
		memset(pBDAddr, 0, SIZE_BDADDRESS_INBYTE);
	}
	else
	{
		pBDAddr[0]= TempInt[0];
		pBDAddr[1]= TempInt[1];
		pBDAddr[2]= TempInt[2];
		pBDAddr[3]= TempInt[3];
		pBDAddr[4]= TempInt[4];
		pBDAddr[5]= TempInt[5];

	}
	
	if(pEncr_Key_Len_Max)
	{
		(*pEncr_Key_Len_Max) = DEFVALUE_BT_ENCRYP_KEY_LEN_MAX;
		(*pEncr_Key_Len_Max) = GetPrivateProfileInt(TAG_ANNEX55, 
										KEY_BT_CONFIG_MAX_ENCRYP_KEY_LEN,
											(*pEncr_Key_Len_Max), FileName);
	}
	
	if(pEncr_Key_Len_Min)
	{
		(*pEncr_Key_Len_Min) = DEFVALUE_BT_ENCRYP_KEY_LEN_MIN;
		(*pEncr_Key_Len_Min) = GetPrivateProfileInt(TAG_ANNEX55, 
										KEY_BT_CONFIG_MIN_ENCRYP_KEY_LEN, 
											(*pEncr_Key_Len_Min), FileName);
	}

	return status;
}

int DutSharedClssHeader WriteAnnex56DataToFile(char *FileName,
 					BYTE Lincense_Key[16],
	                WORD Link_Key_Mask,
	                BYTE Key_Value[3][22])
{
		int status=0, i=0, cnt=0;
	char key[256]="";
    int TempInt[SIZE_BDADDRESS_INBYTE]={0};
	char line[256]="00000000000000000000000000000000";
	char value[MAX_LENGTH]="";

	if(Lincense_Key)
	{
		for (i=0; i<MAX_BYTE_SECURITYKEY; i++)
		{
			sprintf(key, KEY_BT_CONFIG_MASK_SECURITYKEY_FORMAT, i);
				if(Lincense_Key[i]!=0)
					sprintf(value, "0x%x", (Lincense_Key[i])); 
				else
					sprintf(value, "0x00");

			WritePrivateProfileString(TAG_ANNEX55, key, value,  FileName);
			
		}
	}

	if(Link_Key_Mask)
			sprintf(value, "0x%x", (Link_Key_Mask));
	else
			sprintf(value, "0x00");
	WritePrivateProfileString(TAG_ANNEX55, KEY_BT_CONFIG_LINK_KEY_MASK, value,  FileName);

	for(i=0; i<MAXPARTNER_REVD; i++)
	{
		sprintf(key, KEY_CONFIG_MASK_BDADDR_FORMAT, i);

		GetPrivateProfileString(TAG_ANNEX55,key,"", line, 20, FileName); 
 		sprintf(line, "%02X.%02X.%02X.%02X.%02X.%02X", 
			Key_Value[i][0], Key_Value[i][1], Key_Value[i][2], 
			Key_Value[i][3], Key_Value[i][4], Key_Value[i][5]); 
		WritePrivateProfileString(TAG_ANNEX55, key, line, FileName); 

 
		sprintf(key, KEY_CONFIG_MASK_LINKKEY_FORMAT, i);
		strcpy(line, "");
		for (cnt=6; cnt<22; cnt++)
		{
 			sprintf(line, "%s%02X", line, Key_Value[i][cnt]); 
		}
		WritePrivateProfileString(TAG_ANNEX55, key, line, FileName); 

 
	}
	return status;

}

int DutSharedClssHeader ReadAnnex56DataFromFile(char *FileName,
 					BYTE Lincense_Key[16],
	                WORD *Link_Key_Mask,
	                BYTE Key_Value[3][22])
{
	int status=0, i=0, cnt=0;

	char key[MAX_LENGTH]="";
    int  TempInt[SIZE_BDADDRESS_INBYTE]={0};
	char line[MAX_LENGTH]="";
	char KeyBuf[16]={0};
	char value[MAX_LENGTH]="";


	DebugLogRite("File Name :%s\n", FileName);
	if (!GetPrivateProfileSection(TAG_ANNEX55, value, MAX_LENGTH, FileName))
		return ERROR_DATANOTEXIST;

	if(Lincense_Key)
	{
		for (i=0; i<MAX_BYTE_SECURITYKEY; i++)
		{
			sprintf(key, KEY_BT_CONFIG_MASK_SECURITYKEY_FORMAT, i);
			Lincense_Key[i]= GetPrivateProfileInt (TAG_ANNEX55, key, 
							Lincense_Key[i], FileName); 
		}
	}

	if(Link_Key_Mask)
		(*Link_Key_Mask) = GetPrivateProfileInt (TAG_ANNEX55,
								KEY_BT_CONFIG_LINK_KEY_MASK,
								(*Link_Key_Mask), FileName); 

	if(Key_Value)
	{
		for(i=0; i<MAXPARTNER_REVD; i++)
		{
			sprintf(key, KEY_CONFIG_MASK_BDADDR_FORMAT, i);

			GetPrivateProfileString(TAG_ANNEX55 ,key,"", line, 20, FileName); 
			cnt= sscanf(line, "%02x.%02x.%02x.%02x.%02x.%02x", 
				&TempInt[0],&TempInt[1],&TempInt[2],
				&TempInt[3],&TempInt[4],&TempInt[5]);

			if(cnt!=SIZE_BDADDRESS_INBYTE) 
			{
				memset(Key_Value[i], 0, SIZE_BDADDRESS_INBYTE);
			}
			else
			{
				Key_Value[i][0] = TempInt[0];
				Key_Value[i][1] = TempInt[1];
				Key_Value[i][2] = TempInt[2];
				Key_Value[i][3] = TempInt[3];
				Key_Value[i][4] = TempInt[4];
				Key_Value[i][5] = TempInt[5];

			}
   
			sprintf(key, KEY_CONFIG_MASK_LINKKEY_FORMAT, i);

			GetPrivateProfileString(TAG_ANNEX55, key,"", line, 40, FileName); 

	
			cnt= sscanf(line, 
				"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
	 			&KeyBuf[0], &KeyBuf[1],
				&KeyBuf[2], &KeyBuf[3],
				&KeyBuf[4], &KeyBuf[5],
				&KeyBuf[6], &KeyBuf[7],
				&KeyBuf[8],  &KeyBuf[9],
				&KeyBuf[10], &KeyBuf[11],
				&KeyBuf[12], &KeyBuf[13],
				&KeyBuf[14], &KeyBuf[15]);
			
		
			if(cnt==16) 
			{
				memcpy(&Key_Value[i][6], KeyBuf, sizeof(KeyBuf));
			}
 
			
		}
	
	  
	} //	if(Key_Value)

	return status;
}

int DutSharedClssHeader ReadAnnex57DataFromFile(char *FileName,
#if defined (_MIMO_)
					int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)
					BYTE BandId,
					BYTE SubBandId,
					BYTE *MEM_ROW,
					BYTE *NumOfEnterirs,
					DPD_ConeffcicentData *Coneffcicent_Data)
{

	char value[MAX_LENGTH]="";
	char section[MAX_LENGTH]="";
	char tag[MAX_LENGTH]="";
	int status=0;
	DPD_ConeffcicentData *ptr;

#if !defined (_MIMO_)
	int DeviceId=0, PathId=0;		
#endif // #if defined (_MIMO_)

	ptr = (DPD_ConeffcicentData*)Coneffcicent_Data;
	//header

	DebugLogRite("File Name :%s\n", FileName);
	sprintf(section, TAG_ANNEX57, BandId, SubBandId);
	if (!GetPrivateProfileSection(section, value, MAX_LENGTH, FileName))
		return ERROR_DATANOTEXIST;
	
	(*MEM_ROW) = GetPrivateProfileInt(section, 
										KEY_DPD_MEMORY_ROW, 
										(*MEM_ROW), FileName);  

	(*NumOfEnterirs) = GetPrivateProfileInt(section, 
										KEY_DPD_NUMBER_ENTERIRS, 
										(*NumOfEnterirs), FileName);

	memset(ptr, 0xFF, (sizeof(DPD_ConeffcicentData) *(*NumOfEnterirs)));//JKLCHEN 0xFF for use
   
	for ( int j=0; j<(*NumOfEnterirs); j++)
	{
		sprintf(tag, KEY_DPD_POWER_ID, j);
			(ptr->DPDIdx) = GetPrivateProfileInt(section, 
										tag, 
										ptr->DPDIdx, FileName);

		sprintf(tag, KEY_DPD_POWER_CODE, j);
			(ptr->PowerCode) = GetPrivateProfileInt(section, 
										tag, 
										ptr->PowerCode, FileName);

		sprintf(tag, KEY_DPD_START_CHAN, j);
		(ptr->StartChan) = GetPrivateProfileInt(	section, 
													tag, 
													ptr->StartChan, FileName);

		sprintf(tag, KEY_DPD_END_CHAN, j);
		(ptr->EndChan) = GetPrivateProfileInt(	section, 
												tag, 
												ptr->EndChan, FileName);
		for(int i=0;i<DPD_COEFF_ENTRY_NUM; i++)
		{
		
			sprintf(tag, KEY_DPD_REAL_HI, j, i);
			(ptr->ConeffcicentData[i].RealHi) = GetPrivateProfileInt(section, 
										tag, 
										ptr->ConeffcicentData[i].RealHi, FileName);

			sprintf(tag, KEY_DPD_REAL_LO, j, i);
			(ptr->ConeffcicentData[i].RealLo) = GetPrivateProfileInt(section, 
										tag, 
									ptr->ConeffcicentData[i].RealLo, FileName);
		
			sprintf(tag, KEY_DPD_IMAGE_HI, j, i);
			(ptr->ConeffcicentData[i].ImageHi) = GetPrivateProfileInt(section, 
											tag, 
											ptr->ConeffcicentData[i].ImageHi, FileName);

			sprintf(tag, KEY_DPD_IMAGE_LO, j, i);
			(ptr->ConeffcicentData[i].ImageLo) = GetPrivateProfileInt(section, 
											tag, 
											ptr->ConeffcicentData[i].ImageLo, FileName);

		}
		ptr++;
	}
	return status;

}

int DutSharedClssHeader WriteAnnex57DataToFile(char *FileName,
#if defined (_MIMO_)
					int DeviceId,		int PathId,		
#endif // #if defined (_MIMO_)
					BYTE BandId,
					BYTE SubBandId,
					BYTE MEM_ROW,
					BYTE NumOfEnterirs,
					DPD_ConeffcicentData *Coneffcicent_Data)
{
	char value[MAX_LENGTH]="";
	char section[MAX_LENGTH]="";
	char tag[MAX_LENGTH]="";
	int status=0;
	DPD_ConeffcicentData *ptr=(DPD_ConeffcicentData *)Coneffcicent_Data;

#if !defined (_MIMO_)
	int DeviceId=0, PathId=0;		
#endif // #if defined (_MIMO_)

	ptr = (DPD_ConeffcicentData*)Coneffcicent_Data;
	//header

	DebugLogRite("File Name :%s\n", FileName);

	sprintf(section, TAG_ANNEX57, BandId, SubBandId);
	if(MEM_ROW)
		sprintf(value, "0x%x", (MEM_ROW)); 
	else
		sprintf(value, "0x0");
	WritePrivateProfileString(section, KEY_DPD_MEMORY_ROW, value,  FileName);

    if(NumOfEnterirs)
		sprintf(value, "0x%x", (NumOfEnterirs)); 
	else
		sprintf(value, "0x0");
	WritePrivateProfileString(section, KEY_DPD_NUMBER_ENTERIRS, value,  FileName);

	for ( int j=0; j<(NumOfEnterirs); j++)
	{
		
		sprintf(tag, KEY_DPD_START_CHAN, j);
		sprintf(value, "%d", (ptr->StartChan)); 
		WritePrivateProfileString(section,tag, value, FileName);

		sprintf(tag, KEY_DPD_END_CHAN, j);
		sprintf(value, "%d", (ptr->EndChan)); 
		WritePrivateProfileString(section,tag, value, FileName);

		sprintf(tag, KEY_DPD_POWER_ID, j);
		sprintf(value, "0x%x", (ptr->DPDIdx)); 
		   	WritePrivateProfileString(section,tag, value, FileName);

		sprintf(tag, KEY_DPD_POWER_CODE, j);
		sprintf(value, "0x%x", (ptr->PowerCode)); 
		WritePrivateProfileString(section,tag, value, FileName);

		for(int i=0;i<DPD_COEFF_ENTRY_NUM; i++)
		{
			sprintf(tag, KEY_DPD_REAL_HI, j, i);
			sprintf(value, "0x%x", (ptr->ConeffcicentData[i].RealHi &0xff)); 
				WritePrivateProfileString(section,tag, value, FileName);

			sprintf(tag, KEY_DPD_REAL_LO, j, i);
				sprintf(value, "0x%x", (ptr->ConeffcicentData[i].RealLo &0xf));
			WritePrivateProfileString(section,tag, value, FileName);
		
			sprintf(tag, KEY_DPD_IMAGE_HI, j, i);
			sprintf(value, "0x%x", (ptr->ConeffcicentData[i].ImageHi &0xf));
				WritePrivateProfileString(section,tag, value, FileName);

			sprintf(tag, KEY_DPD_IMAGE_LO, j, i);
			sprintf(value, "0x%x", (ptr->ConeffcicentData[i].ImageLo &0xff));
				WritePrivateProfileString(section,tag, value, FileName);
		}
		ptr++;
	}

	return status;
}
#endif	//#if defined(_CAL_REV_D_)
