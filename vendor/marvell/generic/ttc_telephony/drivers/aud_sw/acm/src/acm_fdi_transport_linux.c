/***************************************************************************
* (C) Copyright 2006, 2007 Marvell Ltd. All Rights Reserved.
*
*
* File Description
* ----------------
*    Function as FDI virtual driver to Linux file system for ICAT's users
*    and modules usage on Linux .
***************************************************************************/

#define INTEL_FDI
#if defined(INTEL_FDI)
#include "acm_fdi_ext.h"
#include "acm_fdi_file.h"
#include "acm_fdi_type.h"
#include "acm_linux.h"

// Defines
#define MAX_READ_BUFFER_SIZE        1024
#define MAX_FILES_NAME_BUFFER       1024
#define MAX_FILE_NAME_LENGTH        256

#define DUMMY_FDI_SIZE                          ((2 << 3) << 20)        //8MB
#define DUMMY_FREE_FDI_SIZE                     ((2 << 2) << 20)        //4MB


/***********************************************************************************
 * Function     : _SetRTC
 ***********************************************************************************
 *
 * Description  : Initialize the RTC with input parameter RTC_CalendarTime structure.
 *                                                                                  *
 * Input:         RTC_CalendarTime structure.                                       *
 *                                                                                  *
 * Output:        Success: Success message.                                         *
 *                Error: Error message.                                             *
 * Notes:         Not functional in wince, just a dummy one.
 ************************************************************************************/
//ICAT EXPORTED FUNCTION - Diag,Utils,SetRTC
void _SetRTC(void *pDateAmdTime)
{
}
/***********************************************************************************
 * Function     : _ReadRTC
 ***********************************************************************************
 *
 * Description  : Read the value of the RTC return its value.
 *                                                                                  *
 * Input:         None                                                              *
 *                                                                                  *
 * Output:        Success: Target current time and data information.                *
 *                Error: Failure message.                                           *
 ************************************************************************************/
//ICAT EXPORTED FUNCTION - Diag,Utils,ReadRTC
void _ReadRTC(void)
{
}

/***********************************************************************************
 * Function     : _GetVersionDiag
 ***********************************************************************************
 *
 * Description  : send the diag version
 *                                                                                  *
 * Input:         None                                                              *
 *                                                                                  *
 * Output:
 *
 ************************************************************************************/
//ICAT EXPORTED FUNCTION - Diag,Utils,GetVersionDiag
void _GetVersionDiag(void)
{
}

/******************************************************************************
* Function     : _Fopen
*******************************************************************************
*                                                                             *
* Description  : Open file for read/write                                     *
*                                                                             *
* Input:         FOpenInputStruct.fileName - file name to be opened.          *
*                FOpenInputStruct.mode - instruct weather to open the file to.*
*                read / write / truncate (rb, wb, wb+ etc.) for more see FDI  *
*                Documentation.                                               *                                                                             *
* Output:        Success: FOpenReturnStruct.fileID - opened file ID.          *
*                            FOpenReturnStruct.fileName - opened file name.   *
*                            (to avoid several file open output mistakes).    *
*                Error:  Error message with the error code.                   *
******************************************************************************/
//ICAT EXPORTED FUNCTION - FDI,Transport,Fopen
void _Fopen(void* fInOpenStruct)
{
}


/******************************************************************************
* Function     : _Fclose
*******************************************************************************
*                                                                             *
* Description  : close file                                                   *
*                                                                             *
* Input:         fileIDStruct - ID of an open file.                           *
*                                                                             *
* Output:        Success: success message with file ID.                       *
*                Error:   Error message with error number.                    *
******************************************************************************/
//ICAT EXPORTED FUNCTION - FDI,Transport,Fclose
void _Fclose(void *fileID)
{
}

/************************************************************************************
* Function     : _Fread
************************************************************************************
*                                                                                  *
* Description  : read constant size buffer from an open flash file.                *
*                                                                                  *
* Input:         ReadInputStruct.fileID - ID of open file to be read               *
*                ReadInputStruct.bufferSize - read buffer size (currenly not in use)
*                                                                                  *
* Output:        Success:
*                  InReadReturnStruct.PacketOK -  no error occurred while reading  *
*                  ReadReturnStruct.LastPacket - last packet read form file        *
*                  eadReturnStruct.PacketSize - size of read packet/buffer         *
*                  ReadReturnStruct.readBuf - buffer of bytes read form file       *
*                Error: Error message
************************************************************************************/
//ICAT EXPORTED FUNCTION - FDI,Transport,Fread
void _Fread(void *pReadStruct)
{
}


/************************************************************************************
* Function     : _Fwrite
************************************************************************************
*                                                                                  *
* Description  : write file to flash.                                              *
*                                                                                  *
* Input:         WriteInputStruct.fileID - open flash file ID.                     *
*                WriteInputStruct.PacketSize - size of the buffer to be write      *
*                WriteInputStruct.writeBuf - bytes buffer to be write/append to open file *
*                                                                                  *
* Output:        Success: Success message with the number of written bytes.        *
*                Error: Error message with error code.                             *
************************************************************************************/
//ICAT EXPORTED FUNCTION - FDI,Transport,Fwrite
void _Fwrite(void * pWriteStruct)
{
}

/************************************************************************************
* Function     : _Format
************************************************************************************
*                                                                                  *
* Description  : Format flash                                                      *
*                                                                                  *
* Input:         None                                                              *
*                                                                                  *
* Output:        Success: Success message.                                         *
*                Error: Error message.                                             *
************************************************************************************/
//ICAT EXPORTED FUNCTION - FDI,Transport,Format_Flash
void _Format(void)
{
}

/************************************************************************************
* Function     : _Remove
************************************************************************************
*                                                                                  *
* Description  : Delete file from flash                                            *
*                                                                                  *
* Input:         file name                                                         *
*                                                                                  *
* Output:        Success: Success message.                                         *
*                Error: Error message including the error code.                    *
************************************************************************************/
//ICAT EXPORTED FUNCTION - FDI,Transport,Remove_File
void _Remove(void* fileName)
{
}


/************************************************************************************
* Function     : _GetFileNameList
************************************************************************************
*                                                                                  *
* Description  : Retrieve a list of flash file names and information               *
*                                                                                  *
* Input:         wildcard characters,when placed within filename search criteria,  *
*                perform specific search functions                                 *
*                                                                                  *
* Output:        FNamesReturnStruct.fInfoList - list of file names and info separated by '@' character  *
*                FNamesReturnStruct.fListLen - the above list lenght.                                   *
*                FNamesReturnStruct.LastPacket - bolean flag to indecate on last file info list         *
************************************************************************************/
//ICAT EXPORTED FUNCTION - FDI,Transport,GetFileNameList
void _GetFileNameList(void* wildcard)
{
}


/************************************************************************************
* Function     : _RenameFile
************************************************************************************
*                                                                                  *
* Description  : Delete file from flash                                            *
*                                                                                  *
* Input:         RenameInputStruct.fOldName - file name to be renamed              *
*                RenameInputStruct.fNewName - new file name                        *
* Output:        Success: Success message.                                         *
*                Error: Error message including the error code.                    *
************************************************************************************/
//ICAT EXPORTED FUNCTION - FDI,Transport,RenameFile
void _RenameFile(void* pRenameStruct)
{

}
/************************************************************************************
* Function     : _FStatus
************************************************************************************
*                                                                                  *
* Description  : change file statuse                                               *
*                                                                                  *
* Input:         fileName - file name to query it status                           *
*                                                                                  *
* Output:        Success: file status.                                             *
*                Error: Error message including the error code.                    *
************************************************************************************/
//ICAT EXPORTED FUNCTION - FDI,Transport,FStatus
void _FStatus(void* fileName)
{

}

/************************************************************************************
* Function     : _FChangeMode
************************************************************************************
*                                                                                  *
* Description  : change file status                                               *
*                                                                                  *
* Input:         ChangModeStruct.fileName - name of the file its mode should be changed.  *
*                ChangModeStruct.newMode - new mode to be set for the file mention above. *                                                                 *
*                                                                                  *
* Output:        Success: success message                                          *
*                Error: Error message including file name.                         *
************************************************************************************/
//ICAT EXPORTED FUNCTION - FDI,Transport,FchangeMode
void _FChangeMode(void* stInChMode)
{
}

/********************************************************************************************
 * Function     : _Fseek
 ********************************************************************************************
 *                                                                                              *
 * Description  : Seek within a file. Sets a new position to the file indicator .               *
 *                                The new position, measured in bytes from the beginning of the file,       *
 *                                is obtained by adding offset to the position specified by wherefrom       *
 *                                                                                              *
 * Input:         FSeekStruct.fileID - Handle to open file							                *
 *                FSeekStruct.offset - Byte offset from position specified by wherefrom.        *
 *                FSeekStruct.whereFrom - indicating from what point in the file the offset should be measured.  *
 *                                                                                              *
 * Output:        Success: success message                                                      *
 *                Error: Error message including file name.                                     *
 *******************************************************************************************/
//ICAT EXPORTED FUNCTION - FDI,Transport,Fseek
void _Fseek(void* stFSeek)
{
}

/********************************************************************************************
 * Function     : _GetMaxFileNameLength
 ********************************************************************************************
 *                                                                                              *
 * Description  : Returns the number of maximum characters permited for file name in FDI FDV. *
 *                                                                                              *
 * Input:         None																		*
 * Output:        a number indicates on the maximum valid file name                             *
 *																							*
 *******************************************************************************************/
//ICAT EXPORTED FUNCTION - FDI,Transport,GetMaxFileNameLen
void _GetMaxFileNameLength()
{
}

/********************************************************************************************
 * Function     : _GetFdiFdvSize
 ********************************************************************************************
 *                                                                                              *
 * Description  :  Returns the total space in byte of the Fdi Data Volume.		            *
 *                                                                                              *
 * Input:                  None																		*
 * Output:         Buffer of 64 bytes contains the total space in byte of the Fdi Data Volume *
 *																							*
 *******************************************************************************************/
//ICAT EXPORTED FUNCTION - FDI,Transport,GetFdiFdvSize
void _GetFdiFdvSize()
{
}


/*********************************************************************************************
 * Function     : _GetFdiFdvAvailableSpace
 *********************************************************************************************
 *                                                                                               *
 * Description  :  Returns the total available space (in bytes) of the Fdi Data Volume (FDV).*
 *                                                                                               *
 * Input:                  None																		 *
 * Output:         Get the space abailable in the flash data volume for new file/data objects*
 *																							 *
 *******************************************************************************************/
//ICAT EXPORTED FUNCTION - FDI,Transport,GetFdiFdvAvailableSpace
void _GetFdiFdvAvailableSpace()
{
}


/********************************************************************************************
 * Function     : _GetFdiVersion
 ********************************************************************************************
 *                                                                                              *
 * Description  :  Returns the software version of the FDI.						            *
 *                                                                                              *
 * Input:                  None																		*
 * Output:         Fdi Version                                                                                                                          *
 *******************************************************************************************/
//ICAT EXPORTED FUNCTION - FDI,Transport,GetFdiVersion
void _GetFdiVersion()
{
}

#endif //defined(INTEL_FDI)

