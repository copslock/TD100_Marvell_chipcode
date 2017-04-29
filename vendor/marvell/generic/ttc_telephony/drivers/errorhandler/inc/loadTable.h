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

/***************************************************************************
 *               MODULE IMPLEMENTATION FILE
 ****************************************************************************
 *
 * Filename: loadTable.h
 *
 * The OBM is responsible to copy image from flash to the DDR.
 * It doesn't know about real image size and always copy the maximum 7MB.
 * The problems:
 *  - long time for copying (about 2 sec),
 *  - all ERR/spy/debug buffers are overwriten.
 *
 * SOLUTION:
 * Put the Image BEGIN and END address onto predefined area - offset_0x1C0 size 0x40
 * Add the the text-signature to recognize are these addresses present in image or not.
 * The signature is following the BEGIN/END and is next ":BEGIN:END:LOAD_TABLE_SIGNATURE"
 * OBM should check signature in flash and if it is present MAY use the size=(END-BEGIN).
 * If signature is invalid the default 7MB is used.
 * The IMAGE_END region added into scatter file
 *
 ******************************************************************************/

/*=======================================================================*/
/*        NOTE: This file may be used by OBM or WIN-CE                   */
/*=======================================================================*/

#ifndef _LOAD_TABLE_H_
#define _LOAD_TABLE_H_



#if !defined (OSA_WINCE)
#include "global_types.h"
#else
#include <windows.h>
#include <ceddk.h>
#include <Regext.h>
//#include <diag_api.h>
#include <Oal_memory.h>


#endif


//#if defined (_TAVOR_HARBELL_) || defined (_TAVOR_BOERNE_)

#if !defined (ADDR_CONVERT)    /* May be defined in EE_Postmortem.h or loadTable.h */


#if defined (OSA_WINCE)
// Tavor: DDR COMM mapping
#define TAVOR_BASE_PH_COMM                                 0xBF400000           /* TBD: loadTable.ramRWbegin */
#define TAVOR_SIZE_COMM                    0x00C00000                           /*     ramRWend - ramRWbegin = 6MB */
#endif

#define ADDR_CONVERT(aDDR)    ((UINT8*)((UINT32)(aDDR)))

#endif //ADDR_CONVERT

//Offfset of the LOAD_TABLE = 1C0 (in the First Flash-block)
#define LOAD_TABLE_OFFSET    0x01C0

typedef struct
{
	UINT32 imageBegin;      // image addresses are in HARBELL address space 0xD0??.????
	UINT32 imageEnd;        //                 for BOERNE use conversion to 0xBF??.????
	char Signature[16];
	UINT32 sharedFreeBegin;
	UINT32 sharedFreeEnd;
	UINT32 ramRWbegin;
	UINT32 ramRWend;
	char spare[24];                                 /* This area may be used for data transfer from loader to COMM */
}LoadTableType;                                         /*total size 0x40=64bytes */

#define LOAD_TABLE_SIGNATURE_STR  "LOAD_TABLE_SIGN"     /*15 + zeroEnd */
#define INVALID_ADDRESS           0xBAD0DEAD

extern UINT32 getCommImageBaseAddr(void);
extern void   getAppComShareMemAddr(UINT32* begin, UINT32* end);

#if defined (OSA_WINCE) || defined (OSA_LINUX)
UINT32 ConvertPhysicalAddrToVirtualAddr(UINT32 PhysicalAddr);
void   commImageTableInit();
void commImageTableFree(void);

#define MAP_PHYSICAL_TO_VIRTUAL_ADDRESS(pHYaddr)    /*Returns (UINT32) virtual*/ \
	ConvertPhysicalAddrToVirtualAddr((UINT32)(pHYaddr))
#else

#define MAP_PHYSICAL_TO_VIRTUAL_ADDRESS((pHYaddr))    (pHYaddr)
#endif

#ifndef _LOAD_TABLE_C_
extern LoadTableType loadTable;    /* do NOT use "const" in EXTERN prototype */
#endif

extern LoadTableType  *pLoadTable;

#if defined (TD_SUPPORT)
#define COM_DIAG_OFFLINE_BEGIN 0x7000000
#define COM_DIAG_OFFLINE_END 0x73ffffc
#endif
//#endif// defined (_TAVOR_HARBELL_) || defined (_TAVOR_BOERNE_)
#endif //_LOAD_TABLE_H_

