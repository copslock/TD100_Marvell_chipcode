/** @file DutFuncs.cpp
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#include "../DutCommon/Clss_os.h"
#include "../DutCommon/Dut_error.hc"
#include "../DutCommon/utilities_os.h"
#include "../DutCommon/utility.h"
#include "../DutCommon/utilities.h"
  

#include "DutApi878XDll.h"
#include "DutBtApi878XDll.h"
#include "DutFmApi878XDll.h"
#include "DutWlanApi878XDll.h"
#include "DutFuncs.h" 
 
static char line[255];
  
int LoadThermalTestFile(char *FileName, DWORD *pPTarget, DWORD *pTempRef,
						DWORD *pSlopeNumerator, DWORD *pSlopeDenominator,
						DWORD *pCalibrationIntervalInMS,
						DWORD *pTemp,
						DWORD *pSlopeNumerator1, DWORD *pSlopeDenominator1)
{
	int status =0;

	char tempStr[256]="";

	if (!getcwd(tempStr, _MAX_PATH)) return -1;
	
	strcat(tempStr, "\\");
	strcat(tempStr,FileName);
	strcpy(FileName, tempStr);
	
	if(!strlen(FileName)) 	return -1;
	
	*pSlopeNumerator = GetPrivateProfileInt("THERMAL", "NUMERATOR", *pSlopeNumerator, FileName);
	*pSlopeDenominator = GetPrivateProfileInt("THERMAL", "DENOMINATOR", *pSlopeDenominator, FileName);
	*pCalibrationIntervalInMS = GetPrivateProfileInt("THERMAL", "INTERVAL", *pCalibrationIntervalInMS, FileName);
	*pSlopeNumerator1 = GetPrivateProfileInt("THERMAL", "NUMERATOR1", *pSlopeNumerator1, FileName);
	*pSlopeDenominator1 = GetPrivateProfileInt("THERMAL", "DENOMINATOR1", *pSlopeDenominator1, FileName);
	GetPrivateProfileString( "THERMAL", "PTARGET", "0x50", tempStr, 256, FileName);
	sscanf(tempStr, "0x%x", pPTarget);
	GetPrivateProfileString( "THERMAL", "TEMPREF", "0x70", tempStr, 256, FileName);
	sscanf(tempStr, "0x%x", pTempRef);
	GetPrivateProfileString( "THERMAL", "TEMPERATURE", "0x70", tempStr, 256, FileName);
	sscanf(tempStr, "0x%x", pTemp);

	return status;
}

#if defined (_WLAN_)

int   LoadInitConfigFile ( char *FileName)
{
 	enum {SECT_OPEN=-1, SECT_RF,	SECT_BB,	SECT_MAC,	SECT_SOC, SECT_CAU, SECT_END};
	FILE		* hFile = NULL;
	const char	*sectionName[] = {"//RF",	"//BB",  "//MAC",	"//SOC", "//CAU"};
	const char	*sectionTag[] =  {"[RF]",	"[BB]",  "[MAC]",	"[SOC]", "[CAU]"};
	char  tempString[255]=""; 
	char deliminator[20]="";
	int status =ERROR_NONE;
	int			flagSection=SECT_OPEN; 

	unsigned int		regAddress=0, regValue=0;

	if(!strlen(FileName)) 	return -1;

	hFile = fopen(FileName, "r");
	if (NULL == hFile) {
		DebugLogRite("Cannot Find File %s\n", FileName);
		return -1;
	}

	flagSection=SECT_OPEN; // file openned
	
	while (!feof(hFile)) 
	{
		// get one line
  		fgets(tempString, 255, hFile);
 	
		// is this a blank line
		if(1 >= strlen(tempString) )
			;	// 			next 
		else if('/' == tempString[0] || '#' == tempString[0] || '[' == tempString[0] ) // header or comment
		{
		// is this a section header line, header line can not have comment in it
		// is this a comment line , comment line start with // also
 
			if(		!strnicmp(tempString, sectionName[SECT_RF], sizeof(sectionName[SECT_RF])	) 
				||	!strnicmp(tempString, sectionTag[SECT_RF] , sizeof(sectionTag[SECT_RF]) ))
				flagSection = SECT_RF;

			if(		!strnicmp(tempString, sectionName[SECT_BB], sizeof(sectionName[SECT_BB])	) 
				||	!strnicmp(tempString, sectionTag[SECT_BB], sizeof(sectionTag[SECT_BB])	) )
				flagSection = SECT_BB;

			if(		!strnicmp(tempString, sectionName[SECT_MAC], sizeof(sectionName[SECT_MAC])	) 
				||	!strnicmp(tempString, sectionTag[SECT_MAC], sizeof(sectionTag[SECT_MAC])	) )
				flagSection = SECT_MAC;
	
			if(		!strnicmp(tempString, sectionName[SECT_SOC], sizeof(sectionName[SECT_SOC])	) 
				||	!strnicmp(tempString, sectionTag[SECT_SOC],	sizeof(sectionTag[SECT_SOC])	) )
				flagSection = SECT_SOC;

			if(		!strnicmp(tempString, sectionName[SECT_CAU], sizeof(sectionName[SECT_CAU])	) 
				||	!strnicmp(tempString, sectionTag[SECT_CAU],	sizeof(sectionTag[SECT_CAU])	) )
				flagSection = SECT_CAU;
			//if it is a comment line no need to change flag


		}
		else
		{
            int readFlag=0;
            DWORD regTemp=0,target=0, mask=0;
		// then it is a setting line 
		// setting line can have format of "reg value", "reg, value" or "reg = value" format
			sscanf(tempString, "%x %[^a-f0-9] %x %[^a-f0-9] %x %[^a-f0-9] %x", 
                &regAddress, 
                deliminator, &regValue, 
                deliminator, &readFlag, 
                deliminator, &target); 
			switch(flagSection)
			{
			case SECT_RF:
				//DebugLogRite("RF register %02X with %02X\n", regAddress,  regValue);
                if (readFlag==1)
                {
                    status = DutIf_ReadRfReg(regAddress, (BYTE*)&regTemp);
                    DebugLogRite("RF\t0x%08x,\t0x%08x\n", regAddress, regTemp);
                }
                else
                {
                    status = DutIf_WriteRfReg(regAddress,  regValue);
                }
				break;

			case SECT_BB:
				//DebugLogRite("BB register %02X with %02X\n", regAddress,  regValue);
                if (readFlag==1)
                {
                    status = DutIf_ReadBBReg(regAddress, (BYTE*)&regTemp);
                    DebugLogRite("BB\t0x%08x,\t0x%08x\n", regAddress, regTemp);
                }
                else
				status = DutIf_WriteBBReg(regAddress,  regValue);
				break;

			case SECT_MAC:
				if(regAddress > 0xFFF && (0x0000A000 != (regAddress &0x0000F000)) )
				{
				    //	DebugLogRite("SOC/MAC register %08X with %08X\n", regAddress,  regValue);
                    if (readFlag==1)
                    {
                        status = DutIf_PeekRegDword(regAddress, &regTemp);
                        DebugLogRite("SOC\t0x%08x,\t0x%08x\n", regAddress, regTemp);
                    }
                    else
					    status = DutIf_PokeRegDword(regAddress,  regValue);
				}
				else
				{
				    //	DebugLogRite("MAC register %04X with %08X\n", regAddress,  regValue);
                    if (readFlag>=1 && readFlag <=3)
                    {
                        switch (readFlag)
                        {
                        case 2:
                            status = DutIf_ReadMACReg(regAddress, &regTemp);
                            DebugLogRite("MAC\t0x8000a%03x,\t0x%08x,\t0x%08x,\t%s\n", 
                                regAddress, regTemp, target,(regTemp==target)?"true":"false");
                            break;
                        case 3:
                            status = DutIf_ReadMACReg(regAddress, &regTemp);
                            mask = regValue;
                            DebugLogRite("MAC\t0x8000a%03x, 0x%08x(0x%08x), 0x%08x, 0x%08x, %s\n", 
                                regAddress, regTemp, regTemp&mask, mask, target,((regTemp&mask)==target)?"true":"false");
                            break;
                        case 1:
                        default:
                            status = DutIf_ReadMACReg(regAddress, &regTemp);
                            DebugLogRite("MAC\t0x8000a%03x,\t0x%08x\n", 
                                regAddress, regTemp);
                            break;
                        }
                    }
                    else
					    status = DutIf_WriteMACReg(regAddress,  regValue);
				}
				break;

			case SECT_SOC:
				//DebugLogRite("SOC register %08X with %08X\n", regAddress,  regValue);
                if (readFlag==1)
                {
                    status = DutIf_PeekRegDword(regAddress, &regTemp);
                    DebugLogRite("SOC\t0x%08x,\t0x%08x\n", regAddress, regTemp);
                }
                else
				status = DutIf_PokeRegDword(regAddress,  regValue);
				break;

			case SECT_CAU:
				//DebugLogRite("CAU register %08X with %08X\n", regAddress,  regValue);
				//status = DutIf_PokeRegDword(regAddress,  regValue);
                if (readFlag==1)
                {
                    status = DutIf_ReadCAUReg(regAddress, (BYTE*)&regTemp);
                    DebugLogRite("CAU\t0x%08x,\t0x%08x\n", regAddress, regTemp);
                }
                else
                status = DutIf_WriteCAUReg((int)regAddress,(BYTE)(regValue&0xFF));
				break;
			default:
				// do nothing about this line
				DebugLogRite("Un-sectioned setting %08X with %08X\n", regAddress,  regValue);
				break;
			}

		}

	}

	flagSection=SECT_END; // file ends
	
	fclose(hFile);

		return status; 

}
#endif //#if defined (_WLAN_)

#if defined (_BT_) 
int LoadInitBtConfigFile (char *FileName)
{
  
	enum {SECT_OPEN=-1, SECT_RF,	SECT_BB,	SECT_MAC,	SECT_SOC, SECT_END};
	FILE		* hFile = NULL;
	const char	*sectionName[] = {"//BRF",	"//BTU",  "//MAC",	"//SOC"};
	const char	*sectionTag[] =  {"[BRF]",	"[BTU]",  "[MAC]",	"[SOC]"};
	char  tempString[_MAX_PATH]=""; 
	char deliminator[20]="";
	int status =ERROR_NONE;
	int			flagSection=SECT_OPEN; 

	unsigned int		regAddress=0, regValue=0, readFlag=0, mask=0;

	if(!strlen(FileName)) 	return -1;

	hFile = fopen(FileName, "r");
	if (NULL == hFile) {
		DebugLogRite("Cannot Find File %s\n", FileName);
		return -1;
	}

	flagSection=SECT_OPEN; // file openned
	
	while (!feof(hFile)) 
	{
		// empty the line
		tempString[0]=0;

		// get one line
  		fgets(tempString, _MAX_PATH, hFile);
 	
		// is this a blank line
		if(1 >= strlen(tempString) )
			;	// 			next 
		else if('/' == tempString[0] || '[' == tempString[0] ||
				'#' == tempString[0] || ';' == tempString[0] ) // header or comment
		{
		// is this a section header line, header line can not have comment in it
		// is this a comment line , comment line start with // also
 
 #ifdef _LINUX_
			if(		!strncmp(tempString, sectionName[SECT_RF], strlen(sectionName[SECT_RF])	) 
				||	!strncmp(tempString, sectionTag[SECT_RF] , strlen(sectionTag[SECT_RF]) ))
				flagSection = SECT_RF;

			if(		!strncmp(tempString, sectionName[SECT_BB], strlen(sectionName[SECT_BB])	) 
				||	!strncmp(tempString, sectionTag[SECT_BB], strlen(sectionTag[SECT_BB])	) )
				flagSection = SECT_BB;

			if(		!strncmp(tempString, sectionName[SECT_MAC], strlen(sectionName[SECT_MAC])	) 
				||	!strncmp(tempString, sectionTag[SECT_MAC], strlen(sectionTag[SECT_MAC])	) )
				flagSection = SECT_MAC;
	
			if(		!strncmp(tempString, sectionName[SECT_SOC], strlen(sectionName[SECT_SOC])	) 
				||	!strncmp(tempString, sectionTag[SECT_SOC],	strlen(sectionTag[SECT_SOC])	) )
				flagSection = SECT_SOC;
 
 #else
			if(		!strncmp(tempString, sectionName[SECT_RF], sizeof(sectionName[SECT_RF])	) 
				||	!strncmp(tempString, sectionTag[SECT_RF] , sizeof(sectionTag[SECT_RF]) ))
				flagSection = SECT_RF;

			if(		!strncmp(tempString, sectionName[SECT_BB], sizeof(sectionName[SECT_BB])	) 
				||	!strncmp(tempString, sectionTag[SECT_BB], sizeof(sectionTag[SECT_BB])	) )
				flagSection = SECT_BB;

			if(		!strncmp(tempString, sectionName[SECT_MAC], sizeof(sectionName[SECT_MAC])	) 
				||	!strncmp(tempString, sectionTag[SECT_MAC], sizeof(sectionTag[SECT_MAC])	) )
				flagSection = SECT_MAC;
	
			if(		!strncmp(tempString, sectionName[SECT_SOC], sizeof(sectionName[SECT_SOC])	) 
				||	!strncmp(tempString, sectionTag[SECT_SOC],	sizeof(sectionTag[SECT_SOC])	) )
				flagSection = SECT_SOC;
#endif
			//if it is a comment line no need to change flag


		}
		else
		{
		// then it is a setting line 
		// setting line can have format of "reg value", "reg, value" or "reg = value" format
			char *ptr=0;
			// truncate at ; or # for comments
			ptr = strpbrk (tempString, "#;");
			if(ptr) *ptr=0;
 
			sscanf(tempString, "%x %[^a-f0-9] %x %[^a-f0-9] %x %[^a-f0-9] %x", 
				&regAddress, deliminator, &regValue, 
				deliminator, &readFlag, deliminator, &mask); 
			switch(flagSection)
			{
			case SECT_RF:
				if(readFlag)
				{
					unsigned short regTemp=0;
					status = Dut_Bt_ReadBrfRegister(regAddress,  &regTemp);
					switch (readFlag)
					{
					case 3: // compare with masked value
					case 2: // compare with masked value
						if(0 == mask) 
						{ // compare whole value
							DebugLogRite("BRF register %04X with %02X, Compare with %02X (%s)\n", 
								regAddress,  regTemp, regValue, (regValue==regTemp)?"match":"mismatch");							
						}
						else
						{// compare only masked bits
							DebugLogRite("BRF register %04X with %02X, Compare with %02X masked with %02X (%s)\n", 
								regAddress,  regTemp, regValue, 
								((mask&regValue)==(mask&regTemp))?"match":"mismatch");
						}

						break;
					case 1: // just read and print out
					default:
						DebugLogRite("BRF register %04X with %02X\n", regAddress,  regTemp);
						break;
					}
				}
				else
				{
					//DebugLogRite("BRF register %02X with %02X\n", regAddress,  regValue);
					status = Dut_Bt_WriteBrfRegister(regAddress,  regValue);
				}
				break;
			case SECT_BB:
				if(readFlag)
				{
					unsigned short regTemp=0;
					status = Dut_Bt_ReadBtuRegister(regAddress,  &regTemp);
					switch (readFlag)
					{
					case 3: // compare with masked value
					case 2: // compare with masked value
						if(0 == mask) 
						{ // compare whole value
							DebugLogRite("BTU register %04X with %04X, Compare with %04X (%s)\n", 
								regAddress,  regTemp, regValue, (regValue==regTemp)?"match":"mismatch");							
						}
						else
						{// compare only masked bits
							DebugLogRite("BTU register %04X with %04X, Compare with %04X masked with %04X (%s)\n", 
								regAddress,  regTemp, regValue, 
								((mask&regValue)==(mask&regTemp))?"match":"mismatch");
						}

						break;
					case 1: // just read and print out
					default:
						DebugLogRite("BTU register %04X with %04X\n", regAddress,  regTemp);
						break;
					}
				}
				else
				{
					//DebugLogRite("BB register %02X with %02X\n", regAddress,  regValue);
					status = Dut_Bt_WriteBtuRegister(regAddress,  regValue);
				}
				break;

			case SECT_SOC:
				if(readFlag)
				{
					unsigned long regTemp=0;
					status = Dut_Bt_ReadMem(regAddress,  &regTemp);
					switch (readFlag)
					{
					case 3: // compare with masked value
					case 2: // compare with masked value
						if(0 == mask) 
						{ // compare whole value
							DebugLogRite("SOC register %08X with %08X, Compare with %08X (%s)\n", 
								regAddress,  regTemp, regValue, (regValue==regTemp)?"match":"mismatch");							
						}
						else
						{// compare only masked bits
							DebugLogRite("SOC register %08X with %08X, Compare with %08X masked with %08X (%s)\n", 
								regAddress,  regTemp, regValue, 
								((mask&regValue)==(mask&regTemp))?"match":"mismatch");
						}

						break;
					case 1: // just read and print out
					default:
						DebugLogRite("SOC register %08X with %08X\n", regAddress,  regTemp);
						break;
					}
				}
				else
				{
					//DebugLogRite("SOC register %08X with %08X\n", regAddress,  regValue);
 					status = Dut_Bt_WriteMem(regAddress,  regValue);
				}
				break;
			default:
				// do nothing about this line
				DebugLogRite("Un-sectioned setting %08X with %08X\n", regAddress,  regValue);
				break;
			}

		} 

	}

	flagSection=SECT_END; // file ends
	
	fclose(hFile);

	return status; 

}
#endif //#if defined (_BT_)

#ifdef _FM_
int LoadInitFmConfigFile (char *FileName)
{
  
	enum {SECT_OPEN=-1, SECT_FMRF, SECT_SOC, SECT_END};
	FILE		* hFile = NULL;
	const char	*sectionName[] = {"//FMRF",	 	"//SOC"};
	const char	*sectionTag[] =  {"[FMRF]",	 	"[SOC]"};
	char  tempString[_MAX_PATH]=""; 
	char deliminator[20]="";
	int status =ERROR_NONE;
	int			flagSection=SECT_OPEN; 

	unsigned int		regAddress=0, regValue=0, readFlag=0, mask=0;

	if(!strlen(FileName)) 	return -1;

	hFile = fopen(FileName, "r");
	if (NULL == hFile) {
		DebugLogRite("Cannot Find File %s\n", FileName);
		return -1;
	}

	flagSection=SECT_OPEN; // file openned
	
	while (!feof(hFile)) 
	{
		// empty the line
		tempString[0]=0;

		// get one line
  		fgets(tempString, _MAX_PATH, hFile);
 	
		// is this a blank line
		if(1 >= strlen(tempString) )
			;	// 			next 
		else if('/' == tempString[0] || '[' == tempString[0] ||
				'#' == tempString[0] || ';' == tempString[0] ) // header or comment
		{
		// is this a section header line, header line can not have comment in it
		// is this a comment line , comment line start with // also
 
 #ifdef _LINUX_
			if(		!strncmp(tempString, sectionName[SECT_FMRF], strlen(sectionName[SECT_FMRF])	) 
				||	!strncmp(tempString, sectionTag[SECT_FMRF] , strlen(sectionTag[SECT_FMRF]) ))
				flagSection = SECT_FMRF;

			if(		!strncmp(tempString, sectionName[SECT_SOC], strlen(sectionName[SECT_SOC])	) 
				||	!strncmp(tempString, sectionTag[SECT_SOC],	strlen(sectionTag[SECT_SOC])	) )
				flagSection = SECT_SOC;
 
 #else
			if(		!strncmp(tempString, sectionName[SECT_FMRF], sizeof(sectionName[SECT_FMRF])	) 
				||	!strncmp(tempString, sectionTag[SECT_FMRF] , sizeof(sectionTag[SECT_FMRF]) ))
				flagSection = SECT_FMRF;
 
	
			if(		!strncmp(tempString, sectionName[SECT_SOC], sizeof(sectionName[SECT_SOC])	) 
				||	!strncmp(tempString, sectionTag[SECT_SOC],	sizeof(sectionTag[SECT_SOC])	) )
				flagSection = SECT_SOC;
#endif
			//if it is a comment line no need to change flag


		}
		else
		{
		// then it is a setting line 
		// setting line can have format of "reg value", "reg, value" or "reg = value" format
			char *ptr=0;
			// truncate at ; or # for comments
			ptr = strpbrk (tempString, "#;");
			if(ptr) *ptr=0;
 
			sscanf(tempString, "%x %[^a-f0-9] %x %[^a-f0-9] %x %[^a-f0-9] %x", 
				&regAddress, deliminator, &regValue, 
				deliminator, &readFlag, deliminator, &mask); 
			switch(flagSection)
			{
			case SECT_FMRF:
				if(readFlag)
				{
					unsigned long regTemp=0;
					status = Dut_Fm_ReadFmuRegister(regAddress,  &regTemp);
					switch (readFlag)
					{
					case 3: // compare with masked value
					case 2: // compare with masked value
						if(0 == mask) 
						{ // compare whole value
							DebugLogRite("FMRF register %04X with %04X, Compare with %04X (%s)\n", 
								regAddress,  regTemp, regValue, (regValue==regTemp)?"match":"mismatch");							
						}
						else
						{// compare only masked bits
							DebugLogRite("FMRF register %04X with %04X, Compare with %04X masked with %04X (%s)\n", 
								regAddress,  regTemp, regValue, 
								((mask&regValue)==(mask&regTemp))?"match":"mismatch");
						}

						break;
					case 1: // just read and print out
					default:
						DebugLogRite("FMRF register %04X with %04X\n", regAddress,  regTemp);
						break;
					}
				}
				else
				{
					//DebugLogRite("FMRF register %02X with %04X\n", regAddress,  regValue);
					status = Dut_Fm_WriteFmuRegister(regAddress,  regValue);
				}
				break;
			 

			case SECT_SOC:
				if(readFlag)
				{
					unsigned long regTemp=0;
					status = Dut_Bt_ReadMem(regAddress,  &regTemp);
					switch (readFlag)
					{
					case 3: // compare with masked value
					case 2: // compare with masked value
						if(0 == mask) 
						{ // compare whole value
							DebugLogRite("SOC register %08X with %08X, Compare with %08X (%s)\n", 
								regAddress,  regTemp, regValue, (regValue==regTemp)?"match":"mismatch");							
						}
						else
						{// compare only masked bits
							DebugLogRite("SOC register %08X with %08X, Compare with %08X masked with %08X (%s)\n", 
								regAddress,  regTemp, regValue, 
								((mask&regValue)==(mask&regTemp))?"match":"mismatch");
						}

						break;
					case 1: // just read and print out
					default:
						DebugLogRite("SOC register %08X with %08X\n", regAddress,  regTemp);
						break;
					}
				}
				else
				{
					//DebugLogRite("SOC register %08X with %08X\n", regAddress,  regValue);
 					status = Dut_Bt_WriteMem(regAddress,  regValue);
				}
				break;
			default:
				// do nothing about this line
				DebugLogRite("Un-sectioned setting %08X with %08X\n", regAddress,  regValue);
				break;
			}

		}

	}

	flagSection=SECT_END; // file ends
	
	fclose(hFile);

	return status; 

}

#endif //#ifdef _FM_


 
 
#if 0 //ndef _FLASH_

 
void FlexSpiDLSaveCalMacAddr(void)  
{	int Crc=0;
	BYTE MacAddress[6]={0};
	char CalFileName[255]="CalDataFile_swape.txt";
//	char PwrTblFileName[255]="PwrTbl_swape.txt";
	char PwrTrgtFileName[255]="PwrTarget_swape.txt";

	DutIf_GetMACAddress(MacAddress);
	UpLoadSaveCalData(&Crc, DEVICETYPE_WLAN, CalFileName);

	DutIf_FlexSpiDL(0, NULL);  
	
	if(Crc) LoadSaveCalData(DEVICETYPE_WLAN, CalFileName);
	DutIf_SetMACAddress(MacAddress);
}

void SetPidVid(void)
{
	WORD Pid=0x4500, Vid=0x1385; 
	WORD SubPid=0x4500, SubVid=0x1385; 
	DWORD ClassID=0x020000; 
	WORD ReadbackSubPid=0, ReadbackSubVid=0; 
	WORD ReadbackPid=0, ReadbackVid=0; 
	DWORD ReadbackClassID= 0; 

	DutIf_GetPIDVID(0x30, &Pid, &Vid);
	DutIf_GetPIDVID(0x40, &SubPid, &SubVid);
	DutIf_GetClassId(0x34, &ClassID);

	DebugLogRite("^^ Please enter  new Class ID(0x%06X) ", ClassID);
	fgets(line, sizeof(line), stdin);
	sscanf (line, "%x",   &ClassID);


	DebugLogRite("^^ Please enter  new  PID(0x%04X) ", Pid);
	fgets(line, sizeof(line), stdin);
	sscanf (line, "%x",   &Pid);

	DebugLogRite("^^ Please enter  new  VID(0x%04X) ", Vid);
	fgets(line, sizeof(line), stdin);
	sscanf (line, "%x",   &Vid);


	DebugLogRite("^^ Please enter  new sub PID(0x%04X) ", SubPid);
	fgets(line, sizeof(line), stdin);
	sscanf (line, "%x",   &SubPid);

	DebugLogRite("^^ Please enter  new sub VID(0x%04X) ", SubVid);
	fgets(line, sizeof(line), stdin);
	sscanf (line, "%x",   &SubVid);

	DutIf_SetPIDVID(0x30,Pid, Vid);
	DutIf_SetPIDVID(0x40,SubPid, SubVid);
	DutIf_SetClassId(0x34,ClassID);

	DutIf_GetPIDVID(0x30, &ReadbackPid, &ReadbackVid);
	DutIf_GetPIDVID(0x40, &ReadbackSubPid, &ReadbackSubVid);
	DutIf_GetClassId(0x34, &ReadbackClassID);

	if(ClassID == ReadbackClassID )
		DebugLogRite(" ClassID programming verified\n"); 
	else
		DebugLogRite(" ClassID programming failed to verify\n"); 

	if(Pid == ReadbackPid && Vid == ReadbackVid )
		DebugLogRite(" PID and VID programming verified\n"); 
	else
		DebugLogRite(" PID and VID programming failed to verify\n"); 

	if(SubPid == ReadbackSubPid && SubVid == ReadbackSubVid )
		DebugLogRite(" SubPID and subVID programming verified\n"); 
	else
		DebugLogRite(" SubPID and subVID programming failed to verify\n"); 
	return;
}

void GetPidVid(void) 
{
 	WORD ReadbackSubPid=0, ReadbackSubVid=0; 
	DWORD ClassId=0; 

 	DutIf_GetPIDVID(0x30, &ReadbackSubPid, &ReadbackSubVid);

 	DebugLogRite(" PID    0x%04X and VID    0x%04X \n", ReadbackSubPid, ReadbackSubVid); 
 
	DutIf_GetPIDVID(0x40, &ReadbackSubPid, &ReadbackSubVid);
 	DebugLogRite(" SubPID 0x%04X and SubVID 0x%04X \n", ReadbackSubPid, ReadbackSubVid); 

	DutIf_GetClassId(0x34, &ClassId);
 	DebugLogRite(" ClassId 0x%06X \n", ClassId, ReadbackSubVid); 
}
#endif //#ifndef _FLASH_
 
 
