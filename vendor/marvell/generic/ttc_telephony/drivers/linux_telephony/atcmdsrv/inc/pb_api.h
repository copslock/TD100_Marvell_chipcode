/******************************************************************************
*(C) Copyright 2008 Marvell International Ltd.
* All Rights Reserved
******************************************************************************/
/*--------------------------------------------------------------------------------------------------------------------
 *  -------------------------------------------------------------------------------------------------------------------
 *
 *  Filename: pb_api.h
 *
 *  Description: API interface for PBK service
 *
 *  History:
 *   Oct 13, 2008 - Qiang XU Creation of file
 *
 *  Notes:
 *
 ******************************************************************************/


#ifndef __PB_API_H__
#define __PB_API_H__

#include "ci_pb.h"

CiReturnCode PB_QueryStorage(UINT32 atHandle);
CiReturnCode PB_GetInfo(UINT32 atHandle, CiPrimitiveID priPrimId);
CiReturnCode PB_GetStorage(UINT32 atHandle);
CiReturnCode PB_SetStorage(UINT32 atHandle, char *pbStorageStr, char *pbPasswordStr, int pbPasswordStrLen);
CiReturnCode PB_QueryRead(UINT32 atHandle);
CiReturnCode PB_ReadOneEntry(UINT32 atHandle, int index);
CiReturnCode PB_ReadEntryStart(UINT32 atHandle, int indexStart, int indexEnd);
CiReturnCode PB_QueryWrite(UINT32 atHandle);
CiReturnCode PB_DeleteEntry(UINT32 atHandle, int index);
CiReturnCode PB_WriteEntry(UINT32 atHandle, int index, char *number, int numDigits, int type, char *text, int txtLen);
CiReturnCode PB_FindPBEntry(UINT32 atHandle, char *findtext, int LenFindText);

CiReturnCode PB_CRSM_ReadOneEntry(UINT32 atHandle, int index);
CiReturnCode PB_CRSM_DeleteEntry(UINT32 atHandle, int index);
CiReturnCode PB_CRSM_WriteEntry(UINT32 atHandle, int index, char *number, int numDigits, int type, char *text, int txtLen);


void pbCnf(CiSgOpaqueHandle opSgCnfHandle,      CiServiceGroupID svgId, CiPrimitiveID primId, CiRequestHandle reqHandle, void* paras);
void pbInd(CiSgOpaqueHandle opSgOpaqueHandle, CiServiceGroupID svgId, CiPrimitiveID primId, CiIndicationHandle indHandle, void* pParam);
void resetPbParas(void);
#endif

