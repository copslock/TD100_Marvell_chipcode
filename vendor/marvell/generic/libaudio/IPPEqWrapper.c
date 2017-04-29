/*------------------------------------------------------------
(C) Copyright [2006-2008] Marvell International Ltd.
All Rights Reserved
------------------------------------------------------------*/

/******************************************************************************
*               MODULE IMPLEMENTATION FILE
*******************************************************************************
* Title: audio sample
*
* Filename: IPPEqWrapper.c
*
* Target, platform: Common Platform, SW platform
*
* Authors: Isar Ariel, Israel Davidenko
*
* Description: Sample code for NVM access.
*
* Last Updated:
*
* Notes:
******************************************************************************/


#include <sys/stat.h>
#include <stdio.h>
#include <utils/Log.h>
#include <semaphore.h>

#include <ippdefs.h>
#include <ippSP.h>
#include <codecDef.h>

#include "audio_nvm_libaudio.h"

#define ERR_LOG(x...)     LOGE("Error %s: %s\n", __func__, x)
#define ASSERT

#define NVM_DIR_NAME_MAX_LENGTH (6)
#define NVM_FILE_NAME_MAX_LENGTH (NVM_DIR_NAME_MAX_LENGTH + 60)
typedef struct NVM_Header
{
	unsigned long	StructSize;	// the size of the user structure below
	unsigned long	NumofStructs;	// >1 in case of array of structs (default is 1).
	char	StructName[64];	// the name of the user structure below
	char	Date[32];	// date updated by the ICAT when the file is saved. Filled by calibration SW.
	char	time[32];	// time updated by the ICAT when the file is saved. Filled by calibration SW.
	char	Version[64];	// user version - this field is updated by the SW eng. Every time they update the UserStruct.
	char	HW_ID[32];	// signifies the board number. Filled by calibration SW.
	char	CalibVersion[32];	// signifies the calibration SW version. Filled by calibration SW.
} NVM_Header_ts;

#define FILE_NAME_SIZE               128 /* Maximum length of filename       */
#define MAGIC		0x123

Ipp32s	*g_pDelayLineIRR = NULL;
typedef struct BiQuadTaps
{
    int numBiquad;
    Ipp16s pTaps[MAX_APPLIANCE_VALUE*6];
}BiQuadTaps_ts;

BiQuadTaps_ts g_hybridEQ;
sem_t      mLock;

typedef int *FILE_ID;

typedef struct
{
   /* filename plus end of string character */
   char file_name[FILE_NAME_SIZE + 1];
   int       time;          /* updated time stamp when modified */
   int       date;          /* updated date stamp when modified */
   int       size;          /* size of file data in bytes */
   int       owner_id;
   int       group_id;
   int       permissions;
   FILE_ID   data_id;       /* FDI identifier for file data */

   /* the following fields are needed for power loss recovery */

   FILE_ID   plr_id;        /* used for power loss recovery */
   int       plr_time;      /* used for power loss recovery */
   int       plr_date;      /* used for power loss recovery */
   int       plr_size;      /* used for power loss recovery */
} FILE_INFO;

typedef enum
{
  EQ_HIFI_TABLE = 0,
  HIFI_VOLUME_TABLE,

  MAX_FDI_TABLE_TYPES
} FdiTableTypeName;

typedef struct
{
	uint32_t     tableListSize;
	void       *paramTablePtr;
} TableParamInfo;


static TableParamInfo    _paramsTable[MAX_FDI_TABLE_TYPES];


/*******************************************************************************
* Function: findfirstRecord
*******************************************************************************
* Description: Find first FDI record
*
* Parameters: none
*
* Return value: BOOL
*
* Notes:
*******************************************************************************/
static int findfirstRecord(char* fName, FILE_INFO *fInfo)
{
	int iRet = 1;
	FILE *fpTmp = NULL;
	char fullPath[NVM_FILE_NAME_MAX_LENGTH];

	strcpy(fullPath, NVM_AUDIO_DIR);
	strcat(fullPath, fName);

	fpTmp = fopen(fullPath, "r");
	if (fpTmp != NULL)
	{
		struct stat fStat;

		fclose(fpTmp);

		iRet = stat(fullPath, &fStat);
		if(iRet != 0)
			iRet = 1;

		fInfo->size = fStat.st_size;
	}

	return iRet;
}


/*******************************************************************************
* Function: fdiFopen
*******************************************************************************
* Description: Open FDI file
*
* Parameters: none
*
* Return value: BOOL
*
* Notes:
*******************************************************************************/
static FILE *fdiFopen(char *fName, char *openFlags)
{
	char fullPath[NVM_FILE_NAME_MAX_LENGTH];

	strcpy(fullPath, NVM_AUDIO_DIR);
	strcat(fullPath, fName);

	return ( fopen(fullPath, openFlags) );
}


/*******************************************************************************
* Function: findFDITable
*******************************************************************************
* Description: Search for specific table in FDI
*
* Parameters: none
*
* Return value: BOOL
*
* Notes:
*******************************************************************************/
static int findFDITable(char *tableFileIDName)
{
    int	fdiFileFindError;
	FILE_INFO fdiFileInfo;

	fdiFileFindError = findfirstRecord((char *)tableFileIDName, &fdiFileInfo);  /* find size of specific file */

	return (fdiFileFindError ? 0 : 1);
}

/*******************************************************************************
* Function: readDefaultTable
*******************************************************************************
* Description: Read Tables from default constant tables
*
* Parameters: none
*
* Return value: void
*
* Notes:
******************************************************************************/
static void readDefaultTable(FdiTableTypeName fdiTableName)
{
    void *defaultTablePtr;

	switch(fdiTableName)
	{
		case EQ_HIFI_TABLE:
			defaultTablePtr = malloc((uint32_t) sizeof(hifiEqParamsTable));
			ASSERT(defaultTablePtr != NULL);
			memcpy((unsigned char *)defaultTablePtr, (unsigned char *)hifiEqParamsTable, sizeof(hifiEqParamsTable));
			_paramsTable[EQ_HIFI_TABLE].paramTablePtr = (AUDIO_HiFi_EQ_Params *)defaultTablePtr;
			_paramsTable[EQ_HIFI_TABLE].tableListSize = sizeof(hifiEqParamsTable) / sizeof(AUDIO_HiFi_EQ_Params);
			break;

		case HIFI_VOLUME_TABLE:
			defaultTablePtr = malloc((uint32_t) sizeof(hifiVolumeParamsTable));
			ASSERT(defaultTablePtr != NULL);
			memcpy((unsigned char *)defaultTablePtr, (unsigned char *)hifiVolumeParamsTable, sizeof(hifiVolumeParamsTable));
			_paramsTable[HIFI_VOLUME_TABLE].paramTablePtr = (AUDIO_HiFi_Volume_Params *)defaultTablePtr;
			_paramsTable[HIFI_VOLUME_TABLE].tableListSize = sizeof(hifiVolumeParamsTable) / sizeof(AUDIO_HiFi_Volume_Params);
			break;

		default:
			ASSERT(0);
			break;
	}
}


/*******************************************************************************
* Function: readFDITable
*******************************************************************************
* Description: Read Tables from FDI
*
* Parameters: none
*
* Return value: void
*
* Notes:
******************************************************************************/
static void readFDITable(char *tableFileIDName, FdiTableTypeName fdiTableName)
{
    void      *fdiFilePtr = NULL;
	size_t    fdiFileBytesRead;
    int       fdiFileCloseErr, fdiFileFindError;
    int       errFdi;
	FILE_INFO fdiFileInfo;
	FILE      *tableFileID;
	NVM_Header_ts header;

	fdiFileFindError = findfirstRecord((char *)tableFileIDName, &fdiFileInfo);  /* find size of specific file */
	ASSERT(fdiFileFindError == 0);

	tableFileID = fdiFopen((char *)tableFileIDName, "rb");

    if(tableFileID)
    {
		fdiFileBytesRead = fread(&header, sizeof(uint8_t), sizeof(NVM_Header_ts), tableFileID);
		fdiFilePtr = malloc((uint32_t)(fdiFileInfo.size - sizeof(NVM_Header_ts)));
        if(fdiFilePtr)
        {
            fdiFileBytesRead = fread(fdiFilePtr, sizeof(uint8_t), (fdiFileInfo.size - sizeof(NVM_Header_ts)), tableFileID);
            errFdi = ferror(tableFileID);

			ASSERT(errFdi);

			switch(fdiTableName)
			{
				case EQ_HIFI_TABLE:
					_paramsTable[EQ_HIFI_TABLE].paramTablePtr = (AUDIO_HiFi_EQ_Params *)fdiFilePtr;
					_paramsTable[EQ_HIFI_TABLE].tableListSize = (fdiFileInfo.size - sizeof(NVM_Header_ts)) / sizeof(AUDIO_HiFi_EQ_Params);
					break;

				case HIFI_VOLUME_TABLE:
					_paramsTable[HIFI_VOLUME_TABLE].paramTablePtr = (AUDIO_HiFi_Volume_Params *)fdiFilePtr;
					_paramsTable[HIFI_VOLUME_TABLE].tableListSize = (fdiFileInfo.size - sizeof(NVM_Header_ts)) / sizeof(AUDIO_HiFi_Volume_Params);
					break;

				default:
					ASSERT(0);
					break;
			}

			/* can't "free(fdiFilePtr)" ==> it is being used during all times */
        }
        else  /*error alocation*/
            ASSERT(0);

		fclose(tableFileID);
	}
    else
        ASSERT(0);
}


/*******************************************************************************
* Function: readTableParams
*******************************************************************************
* Description: Read Tables from FDI
*
* Parameters: none
*
* Return value: void
*
* Notes:
*******************************************************************************/
static void readTableParams(void)
{
	uint32_t i = 0;
	AUDIO_HiFi_EQ_Params		*hifiEqParams = NULL;
	AUDIO_HiFi_Volume_Params	*hifiVolumeParams = NULL;

	/* only when all files exist acm nvm is read, due to nvm dependecies */
	if (findFDITable(AUDIO_EQ_HIFI_TABLE_FILENAME))
	{
		LOGD("%s exists ==> Use it!\n", AUDIO_EQ_HIFI_TABLE_FILENAME);
		readFDITable(AUDIO_EQ_HIFI_TABLE_FILENAME, EQ_HIFI_TABLE);
	}
	else
	{  /* other -  use defaults */
		LOGD("%s does NOT exist ==> Use hard-coded default!\n", AUDIO_EQ_HIFI_TABLE_FILENAME);
		readDefaultTable(EQ_HIFI_TABLE);
	}

	if (findFDITable(AUDIO_HIFI_VOLUME_TABLE_FILENAME))
	{
		LOGD("%s exists ==> Use it!\n", AUDIO_HIFI_VOLUME_TABLE_FILENAME);
		readFDITable(AUDIO_HIFI_VOLUME_TABLE_FILENAME, HIFI_VOLUME_TABLE);
	}
	else
	{  /* other -  use defaults */
		LOGD("%s does NOT exist ==> Use hard-ceded default!\n", AUDIO_HIFI_VOLUME_TABLE_FILENAME);
		readDefaultTable(HIFI_VOLUME_TABLE);
	}


   /* Use the Eq HiFi Params */
	hifiEqParams = (AUDIO_HiFi_EQ_Params *)_paramsTable[EQ_HIFI_TABLE].paramTablePtr;

	g_hybridEQ.numBiquad = 0;

	for(i=0; i < _paramsTable[EQ_HIFI_TABLE].tableListSize; i++)
	{
		LOGD("[%d] noOfTaps= %d, params[0]= %d, params[1]= %d, params[2]= %d, params[3]= %d, params[4]= %d, params[5]= %d, appliance= %d, onOff= %d, description= %s\n", i, hifiEqParams->noOfTaps, hifiEqParams->params[0], hifiEqParams->params[1], hifiEqParams->params[2], hifiEqParams->params[3], hifiEqParams->params[4], hifiEqParams->params[5], hifiEqParams->appliance, hifiEqParams->onOff, hifiEqParams->description);

                if(hifiEqParams->onOff == ON){
                    memcpy(g_hybridEQ.pTaps+g_hybridEQ.numBiquad*6, hifiEqParams->params, 6*sizeof(hifiEqParams->params[0]));
                    g_hybridEQ.numBiquad++;
                }
		hifiEqParams++;
	}


   /* Use the HiFi Volume Params */
	hifiVolumeParams = (AUDIO_HiFi_Volume_Params *)_paramsTable[HIFI_VOLUME_TABLE].paramTablePtr;

	for(i=0; i < _paramsTable[HIFI_VOLUME_TABLE].tableListSize; i++)
	{
		LOGD("[%d] containerType= %d, streamType= %d, volumeOffset= %d\n", i, hifiVolumeParams->containerType, hifiVolumeParams->streamType, hifiVolumeParams->volumeOffset);

		hifiVolumeParams++;
	}
}

void reloadFDI(void)
{
	void				  *oldParamTablePtr[MAX_FDI_TABLE_TYPES] = {NULL};
	void			      *oldDeviceTable = NULL;
	int 				  i;
	unsigned char         success = 1;

	LOGD("reloadFDI enter");
	/* save all tables original allocation (to be free after reload) */
	for(i = 0; i < MAX_FDI_TABLE_TYPES; i++)
	{
		oldParamTablePtr[i] = (void *)_paramsTable[i].paramTablePtr;
	}

	/* read FDI tables */
	readTableParams();

	/* free old tables after reload */
	for(i = 0; i < MAX_FDI_TABLE_TYPES; i++)
	{
		if (oldParamTablePtr[i] != NULL)
		{
			free((void *)oldParamTablePtr[i]);
		}
	}
	free((void *)oldDeviceTable);

	LOGD("reloadFDI exit");
}


void EqReset()
{
	if (g_pDelayLineIRR != NULL)
	{

        sem_wait(&mLock);
	memset(g_pDelayLineIRR, 0, g_hybridEQ.numBiquad * 2 *2 * sizeof(Ipp32s));
        sem_post(&mLock);
	}
}

static void FileSystemNotify(int iParam)
{
	LOGD("File changed iParam=%d", iParam);
	if (iParam == MAGIC)
	{
		reloadFDI();
		EqReset();
	}
}

int EqTransformPacket(void* bufferInOut, size_t bytesIn,size_t channel)
{
	AUDIO_HiFi_EQ_Params *pParansTable;
        IppStatus ippret;

	if (g_pDelayLineIRR == NULL)
	{
		/* delayed initialisation */
		readTableParams();
        sem_init(&mLock, 0, 1);
	}

//	pParansTable = (AUDIO_HiFi_EQ_Params*)_paramsTable[EQ_HIFI_TABLE].paramTablePtr;
	if (g_hybridEQ.numBiquad == 0)
	{
		return 1;
	}

	if (g_pDelayLineIRR == NULL)
	{      
		g_pDelayLineIRR = (Ipp32s*)malloc(sizeof(Ipp32s) * g_hybridEQ.numBiquad * 2 *2);
		if (g_pDelayLineIRR == NULL)
		{
			ERR_LOG("unable allocate g_pDelayLineIRR");
			return 0;
		}
		memset(g_pDelayLineIRR, 0, g_hybridEQ.numBiquad * 2 * 2 * sizeof(Ipp32s));
	}
        if(channel == 1){
           ippret = ippsIIR_BiQuadDirect_16s((Ipp16s*)bufferInOut,(Ipp16s*)bufferInOut, bytesIn/2, g_hybridEQ.pTaps,  g_hybridEQ.numBiquad, g_pDelayLineIRR);
        }
        else if(channel == 2){
           ippret = ippsIIR_BiQuadDirect_16s_IntStereo((Ipp16s*)bufferInOut,(Ipp16s*)bufferInOut, bytesIn/2/2, g_hybridEQ.pTaps,  g_hybridEQ.numBiquad, g_pDelayLineIRR);
        }
        else
            return 0;
        if (ippret == IPP_STATUS_NOERR)
	{
		return 1;
	}
	else
		LOGE("ippsIIR_Direct_16s error = %d", ippret);


	return 0;
}
