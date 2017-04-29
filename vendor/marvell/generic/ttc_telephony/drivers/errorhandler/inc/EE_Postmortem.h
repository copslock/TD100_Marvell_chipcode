/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code (“Material”) are owned
   by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
   its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
   Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
   treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
   transmitted, distributed, or disclosed in any way without Intel’s prior express written permission.

   No license under any patent, copyright, trade secret or other intellectual property right is granted to or
   conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
   estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
   Intel in writing.
   -------------------------------------------------------------------------------------------------------------------*/

/*******************************************************************************
*               MODULE HEADER FILE
********************************************************************************
*
* Filename: EE_Postmortem.h
*
* Description:
*  Descriptor-Table describing all buffers used by case of Exception or Assert
*  for postmortem debugging.
*
*******************************************************************************/

#ifndef _ERROR_POSTMORTEM_DESC_H_
#define _ERROR_POSTMORTEM_DESC_H_


#include "loadTable.h"

#define INTEL_FDI

#if !defined (ADDR_CONVERT)    /* May be defined in EE_Postmortem.h or loadTable.h */

#define ADDR_CONVERT(aDDR)    ((UINT8*)((UINT32)(aDDR)))

#endif //ADDR_CONVERT

#define POSTMORTEM_BUF_NAME_SIZE       (7 + 1) /* FIXED size including \0 */
#define POSTMORTEM_BUF_NAME_MIPSRAM    "MipsRam"        /* 7chars + \0 */
#define POSTMORTEM_BUF_NAME_EEBUF      "EE_Hbuf"        /* 7chars + \0 */
#define POSTMORTEM_BUF_NAME_APLPSPY    "aplpSpy"        /* 7chars + \0 */
#define POSTMORTEM_BUF_NAME_RAMLOG     "RamLog_"        /* 7chars + \0 */
#define POSTMORTEM_BUF_NAME_GENDUMP    "dumpBuf"        /* 7chars + \0 */
//   The final FILE-name is "com_"#POSTMORTEM_BUF_NAME_xx#".bin"

#define POSTMORTEM_NUMOF_ENTRIES        (15 + 1) /* The last one is all zeros as end! */

//ICAT EXPORTED STRUCT
typedef struct
{
	char name[POSTMORTEM_BUF_NAME_SIZE];    //8 bytes    Name  for the buffer to be accessed or saved into file
	UINT8*    bufAddr;                      //4 bytes   pointer to the buffer to be accessed or saved into file
	UINT32 bufLen;                          //4 bytes   length  of the buffer to be accessed or saved into file
} EE_PostmortemDesc_Entry;                      //=16 bytes

extern EE_PostmortemDesc_Entry EE_PostmortemDesc[POSTMORTEM_NUMOF_ENTRIES];

//on HARBELL: AVOR_SHARED_MEM_ADDR_0 == (UINT32)&boerne_shared_data
//on BOERNE:  there is no label
#define TAVOR_SHARED_MEM_ADDR_0      (ARBEL_BASE_ADDRESS + 1 * sizeof(void*))   /* Used for ARBEL_ADDR_FOR_ID in OBM*/
#define TAVOR_SHARED_MEM_ADDR_1      (ARBEL_BASE_ADDRESS + 2 * sizeof(void*))
#define TAVOR_SHARED_MEM_ADDR_2      (ARBEL_BASE_ADDRESS + 3 * sizeof(void*))   /* Used for postmortem */
#define TAVOR_SHARED_MEM_ADDR_3      (ARBEL_BASE_ADDRESS + 4 * sizeof(void*))


//#define PUB_ADDR_OF_PTR2POSTMORTEM_DESC        ADDR_CONVERT (TAVOR_SHARED_MEM_ADDR_2)
#define PUB_ADDR_OF_PTR2POSTMORTEM_OFFS        /*ADDR_2*/ (3 * sizeof(void*))
#define PUB_ADDR_OF_PTR2POSTMORTEM_DESC        ADDR_CONVERT(getCommImageBaseAddr() + PUB_ADDR_OF_PTR2POSTMORTEM_OFFS)


/* export ICAT functions */
#ifdef __cplusplus
extern "C" {
#endif
void SaveComPostmortem(void);                   //Save all buffers from the Dec-Table into files
void SaveCom_Addr_Len(void* p);                 //Generic procedure to save into file "comm_"POSTMORTEM_BUF_NAME_GENDUMP".bin"
UINT32  EELOG_shareReadFWrite(void);            // Log file-stream
#if defined (OSA_WINCE)
void    WinCE_EELOG_shareReadFWrite(void);      // Log file-stream
#endif

#if defined (OSA_LINUX)
void    Linux_EELOG_shareReadFWrite(void); // Log file-stream
#endif

#ifdef __cplusplus
}
#endif


//#define EE_POSTMORTEM_EXTEND_ICAT_EXPORT
#if defined (EE_POSTMORTEM_EXTEND_ICAT_EXPORT)
void SaveComMIPSbuf(void);
void SaveComEEbuf(void);
#endif

extern BOOL EE_PostmortemRegistration(char* name, UINT8* bufAddr, UINT32 bufLen);
extern void EE_SaveComDDR_RW(UINT32 sizeLimitBytes);
#if defined (TD_SUPPORT)
extern void EE_SaveComDDR_DiagOffline(UINT32 sizeLimitBytes);
#endif
extern BOOL GetComEEDescbuf(char *pDesc, int len);

#endif //end of _ERROR_POSTMORTEM_DESC_H_




