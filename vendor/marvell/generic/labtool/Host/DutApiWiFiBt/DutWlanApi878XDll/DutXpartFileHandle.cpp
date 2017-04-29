/** @file DutXpartFileHandle.cpp
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#include "DutWlanApiClss.h"
#include "DutXpartFileHandle.h"

int GetBandSubBandTag(DWORD Band, DWORD SubBand, char *Tag)
{
	// get file name from setup.ini
	sprintf(Tag, TagFormat_BandSubBand, Band, SubBand); 

	return 0;
}

int GetFEMFileName(char *FileName)
{
	char file[_MAX_PATH]="",  tag[100]="";
 
	if(getcwd(file, _MAX_PATH) != NULL)  
		strcat(file, "\\setup.ini");
	else
		strcpy(file, "setup.ini");
	
	GetPrivateProfileString("HW_CONFIG", "FEM_FILENAME", "", 
			FileName, 256, file); 

	if(strlen(FileName) >1 && FileName[1] !=':')
	{
		if(getcwd(file, _MAX_PATH) != NULL)  
		{	
			strcat(file, "\\"); 
			strcat(file, FileName); 
			strcpy(FileName, file);
		}
	}

	return 0;
}


