/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
   INTEL CONFIDENTIAL
   Copyright 2006 Intel Corporation All Rights Reserved.
   The source code contained or described herein and all documents related to the source code (“Material<94>) are owned
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
 * Filename: loadTable.c
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
#define _LOAD_TABLE_C_


#include <string.h>
#include "loadTable.h"

#if !defined (OSA_WINCE) && !defined (OSA_LINUX)
#include "bsp_util.h"
#include "global_types.h"
#include "asserts.h"
#endif

#if defined (OSA_LINUX)
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include "eeh_ioctl.h"
#include "pxa_dbg.h"
#include "diag_API.h"

extern int sehdrv_fd;
#define PRE_LOADTABLE_PHYADDR LOAD_SEAGULL_ADDR
void printComLoadTable(void);
#endif

#if defined (_TAVOR_HARBELL_)
extern UINT32 Load$$IMAGE_END$$Base;
extern UINT32 Load$$DDR_RO_INIT_CODE$$Base;
extern UINT32 Image$$DDR_NONCACHE_REGION$$Base;
extern UINT32 Image$$DDR_NONCACHE_REGION_AC_SHARE$$Base;
extern UINT32 Image$$DDR_NONCACHE_REGION_AC_SHARE_ENDMARKER$$Base;
extern UINT32 Image$$DDR_HEAP_ENDMARK$$Base;

const LoadTableType loadTable =
{
	/* UINT32 imageBegin;      */ (UINT32)(&Load$$DDR_RO_INIT_CODE$$Base),
	/* UINT32 imageEnd;        */ (UINT32)(&Load$$IMAGE_END$$Base),
	/* char   Signature[16];   */ LOAD_TABLE_SIGNATURE_STR,
	/* UINT32 sharedFreeBegin; */ (UINT32)(&Image$$DDR_NONCACHE_REGION_AC_SHARE$$Base),
	/* UINT32 sharedFreeEnd;   */ (UINT32)(&Image$$DDR_NONCACHE_REGION_AC_SHARE_ENDMARKER$$Base),
	/* UINT32 ramRWbegin;      */ (UINT32)(&Image$$DDR_NONCACHE_REGION$$Base),
	/* UINT32 ramRWbend;       */ (UINT32)(&Image$$DDR_HEAP_ENDMARK$$Base)
	/* char   spare[24];       */
};
#else //_TAVOR_HARBELL_
#if defined (_TAVOR_BOERNE_) && !defined (OSA_WINCE) && !defined (OSA_LINUX)
//APPS loadTable               // Currently WINCE and LINUX does not supports built-in loadTable.
extern UINT32 Load$$DDR_RO_INIT_CODE$$Base;
extern UINT32 Load$$END_LOAD_MARKER$$Base;

const LoadTableType loadTable =
{
	/* UINT32 imageBegin;      */ (UINT32)(&Load$$DDR_RO_INIT_CODE$$Base),
	/* UINT32 imageEnd;        */ (UINT32)(&Load$$END_LOAD_MARKER$$Base),
	/* char   Signature[16];   */ LOAD_TABLE_SIGNATURE_STR,
	/* UINT32 sharedFreeBegin; */ (UINT32)0,
	/* UINT32 sharedFreeEnd;   */ (UINT32)0,
	/* UINT32 ramRWbegin;      */ (UINT32)0,
	/* UINT32 ramRWend;      */ (UINT32)0,
	/* char   spare[28];       */
};
//go to search for the COMM's loadTable
#else //_TAVOR_BOERNE_
const LoadTableType loadTable =
{
	/* UINT32 imageBegin;   */ INVALID_ADDRESS,
	/* UINT32 imageEnd;     */ INVALID_ADDRESS,
	/* char   Signature[16];   */ {0},
	/* UINT32 sharedFreeBegin; */ (UINT32)0,
	/* UINT32 sharedFreeEnd;   */ (UINT32)0,
	/* UINT32 ramRWbegin;      */ (UINT32)0,
	/* UINT32 ramRWend;      */ (UINT32)0,
	/* char   spare[28];       */{0}
};
#endif
#endif //defined (_TAVOR_HARBELL_)


LoadTableType  *pLoadTable = (LoadTableType*)INVALID_ADDRESS;

#if defined (OSA_WINCE) || defined (OSA_LINUX)
UINT32 CommRWaddr = (UINT32)NULL;
UINT32 CommRObaseAddr = (UINT32)NULL;
UINT32 CommResetAddrRoutine = (UINT32)NULL;
UINT32 CommShareMemoryAddr = (UINT32)NULL;
UINT32 g_CommRWArea, g_CommRWAreaSizeInBytes; //Read Write area to post mortem
#if defined (TD_SUPPORT)
UINT32 CommDiagOfflineAddr = (UINT32)NULL;
UINT32 g_CommDiagOfflineArea, g_CommDiagOfflineAreaSizeInBytes;
#endif
#endif


UINT32 getCommImageBaseAddr(void)
{
	//   ASSERT((UINT32)pLoadTable != INVALID_ADDRESS);
	if ((UINT32)pLoadTable == INVALID_ADDRESS)
		return(INVALID_ADDRESS);
	else
		return(pLoadTable->imageBegin);
}

void getAppComShareMemAddr(UINT32* begin, UINT32* end)
{
	if ((UINT32)pLoadTable == INVALID_ADDRESS)
	{       //set the SAME address for begin and end, e.g. the size is zero
		*begin = INVALID_ADDRESS;
		*end   = INVALID_ADDRESS;
	}
	else
	{
		*begin = pLoadTable->sharedFreeBegin;
		*end   = pLoadTable->sharedFreeEnd;
	}
}

#if defined (OSA_LINUX)
#define PAGE_OFFS_BITS(pgsz) ((unsigned long)(pgsz) - 1)
#define PAGE_MASK_BITS(pgsz) (~PAGE_OFFS_BITS(pgsz))

void * eeh_mmap(void* addr, size_t len, int prot, int flags, int fd, off_t offset)
{
	int pageSize = sysconf(_SC_PAGESIZE);
	unsigned long addrAligned;
	volatile unsigned long* pa;
	void* vpa;

	// Align the length so the mapped area is page-aligned and contains the requested area
	addrAligned = offset & PAGE_MASK_BITS(pageSize);

	if ((vpa = mmap(addr, len, prot, flags, fd, addrAligned)) == MAP_FAILED)
	{
		ERRMSG("mmap failed (%d)\n", errno);
		return NULL;
	}

	pa = (volatile unsigned long*)vpa;
	pa += (offset & PAGE_OFFS_BITS(pageSize)) >> 2; /* long ptr type*/
	return (void *)pa;
}

int eeh_munmap (void *addr, size_t length)
{
	int pageSize = sysconf(_SC_PAGESIZE);
	unsigned long addrAligned = (unsigned long)addr & PAGE_MASK_BITS(pageSize);

	return munmap((void*)addrAligned, length);
}


#endif

void commImageTableInit(void)
{
#if defined(_QT_)
// nothing to do for QT
#else
#if !defined (_TAVOR_BOERNE_)
	pLoadTable = (LoadTableType*)&loadTable;
#else
	char signature[] = LOAD_TABLE_SIGNATURE_STR;
	BOOL signatureFound = FALSE;
	LoadTableType*   p;

#if defined (OSA_WINCE)
    #define RESET_ROUTINE_ARBEL_ADDR   (0xD0FFFFE0)
	PHYSICAL_ADDRESS PA;

	PA.QuadPart = (UINT32)ADDR_CONVERT(RESET_ROUTINE_ARBEL_ADDR);
	if (CommResetAddrRoutine == (UINT32)NULL)
		CommResetAddrRoutine = (UINT32)MmMapIoSpace(PA, 0x10, FALSE);

	//Obtain win-mapped COM Base Address of the RO-Code segment
	PA.QuadPart = (UINT32)ADDR_CONVERT(*(UINT32*)(CommResetAddrRoutine + sizeof(UINT32)));
	if (CommRObaseAddr == (UINT32)NULL)
		CommRObaseAddr = (UINT32)MmMapIoSpace(PA, LOAD_TABLE_OFFSET + sizeof(LoadTableType), FALSE);

	// Find loadTable
	p = (LoadTableType*)(CommRObaseAddr + LOAD_TABLE_OFFSET);
#elif defined (OSA_LINUX)

	CommRObaseAddr = (UINT32)eeh_mmap(0, sizeof(LoadTableType) + LOAD_TABLE_OFFSET, PROT_READ, MAP_PRIVATE, sehdrv_fd, PRE_LOADTABLE_PHYADDR);
	if (CommRObaseAddr == (UINT32)NULL)
	{
		ERRMSG("%s: CommRObaseAddr is NULL\n", __FUNCTION__);
		return;
	}

	DBGMSG("%s: CommRObaseAddr Logic Addr: 0x%x\n", __FUNCTION__, CommRObaseAddr);

	// Find loadTable
	p = (LoadTableType*)(CommRObaseAddr + LOAD_TABLE_OFFSET);

#else

	p = (LoadTableType*)(
		( *(UINT32*)(ADDR_CONVERT(RESET_ROUTINE_ARBEL_ADDR) + sizeof(UINT32)) )
		+ LOAD_TABLE_OFFSET);
#endif

	signatureFound = ( memcmp(p->Signature, signature, sizeof(signature))  == 0 );
	if (signatureFound)
	{
		pLoadTable = p;
#if defined (OSA_WINCE)
		// Take COM RW area begin/end descriptions from the loadTable
		// and make WIN-MAP for them
		g_CommRWArea = p->ramRWbegin;
		g_CommRWAreaSizeInBytes = (p->ramRWend - p->ramRWbegin + 4);
		PA.QuadPart = (UINT32)ADDR_CONVERT(p->ramRWbegin);
		if (CommRWaddr == (UINT32)NULL)
			CommRWaddr = (UINT32)MmMapIoSpace(PA, (p->ramRWend - p->ramRWbegin + 4), FALSE);
#endif

#if defined (OSA_LINUX)
		DBGMSG("commImageTableInit: signatureFound\n");
		DBGMSG("pLoadTable->imageBegin: 0x%x, pLoadTable->imageEnd: 0x%x\n", pLoadTable->imageBegin, pLoadTable->imageEnd);
		DBGMSG("pLoadTable->ramRWbegin: 0x%x, pLoadTable->ramRWend: 0x%x\n", pLoadTable->ramRWbegin, pLoadTable->ramRWend);
		DBGMSG("pLoadTable->sharedFreeBegin: 0x%x, pLoadTable->sharedFreeEnd: 0x%x\n", pLoadTable->sharedFreeBegin, pLoadTable->sharedFreeEnd);
		printComLoadTable();
		g_CommRWArea = p->ramRWbegin;
		DBGMSG("%s: g_CommRWArea: 0x%x\n", __FUNCTION__, g_CommRWArea);
		g_CommRWAreaSizeInBytes = (p->ramRWend - p->ramRWbegin + 4);
		CommRWaddr = (UINT32)eeh_mmap(0, g_CommRWAreaSizeInBytes, PROT_READ | PROT_WRITE, MAP_SHARED, sehdrv_fd, (UINT32)ADDR_CONVERT(p->ramRWbegin));
		if (CommRWaddr == (UINT32)NULL)
		{
			ERRMSG("%s: CommRWaddr is NULL\n", __FUNCTION__);
			return;
		}
		DBGMSG("%s: CommRWaddr: 0x%x\n", __FUNCTION__, CommRWaddr);
#endif

#if defined (TD_SUPPORT)
#if defined (OSA_WINCE)
		g_CommDiagOfflineArea = COM_DIAG_OFFLINE_BEGIN;
		g_CommDiagOfflineAreaSizeInBytes = (COM_DIAG_OFFLINE_END - COM_DIAG_OFFLINE_BEGIN + 4);
		PA.QuadPart = (UINT32)ADDR_CONVERT(g_CommDiagOfflineArea);
		if (CommDiagOfflineAddr == (UINT32)NULL)
			CommDiagOfflineAddr = (UINT32)MmMapIoSpace(PA, g_CommDiagOfflineAreaSizeInBytes, FALSE);
#endif
#if defined (OSA_LINUX)
		DBGMSG("diagOffline begin: 0x%x, diagOffline end: 0x%x\n", COM_DIAG_OFFLINE_BEGIN, COM_DIAG_OFFLINE_END);
		g_CommDiagOfflineArea = COM_DIAG_OFFLINE_BEGIN;
		DBGMSG("%s: g_CommDiagOfflineArea: 0x%x\n", __FUNCTION__, g_CommDiagOfflineArea);
		g_CommDiagOfflineAreaSizeInBytes = (COM_DIAG_OFFLINE_END - COM_DIAG_OFFLINE_BEGIN + 4);
		CommDiagOfflineAddr = (UINT32)eeh_mmap(0, g_CommDiagOfflineAreaSizeInBytes, PROT_READ | PROT_WRITE, MAP_SHARED, sehdrv_fd, (UINT32)ADDR_CONVERT(g_CommDiagOfflineArea));
		if (CommDiagOfflineAddr == (UINT32)NULL)
		{
			ERRMSG("%s: CommDiagOfflineAddr is NULL\n", __FUNCTION__);
			return;
		}
		DBGMSG("%s: CommDiagOfflineAddr: 0x%x\n", __FUNCTION__, CommDiagOfflineAddr);
#endif
#endif
		return;
	}
	else
	{
		ERRMSG("commImageTableInit: signature NOT Found\n");
		return;
	}

	//else ASSERT()
#endif //_TAVOR_BOERNE_
#endif //_QT_
}

void commImageTableFree(void)
{

	#if defined(OSA_LINUX)
	if (CommRObaseAddr != (UINT32)NULL)
	{
		eeh_munmap((void*)CommRObaseAddr, LOAD_TABLE_OFFSET + sizeof(LoadTableType));
		CommRObaseAddr = (UINT32)NULL;

	}
	if (CommRWaddr != (UINT32)NULL)
	{
		eeh_munmap((void*)CommRWaddr, g_CommRWAreaSizeInBytes);
		CommRWaddr = (UINT32)NULL;
	}
	#if defined(TD_SUPPORT)
	if (CommDiagOfflineAddr != (UINT32)NULL)
	{
		eeh_munmap((void*)CommDiagOfflineAddr, g_CommDiagOfflineAreaSizeInBytes);
		CommDiagOfflineAddr = (UINT32)NULL;
	}
	#endif
	#else
	if (CommResetAddrRoutine != (UINT32)NULL)
	{
		MmUnmapIoSpace((PVOID)CommResetAddrRoutine, 0x10);
		CommResetAddrRoutine = (UINT32)NULL;

	}
	if (CommRObaseAddr != (UINT32)NULL)
	{
		MmUnmapIoSpace((PVOID)CommRObaseAddr, LOAD_TABLE_OFFSET + sizeof(LoadTableType));
		CommRObaseAddr = (UINT32)NULL;

	}
	if (CommRWaddr != (UINT32)NULL)
	{
		MmUnmapIoSpace((PVOID)CommRWaddr, g_CommRWAreaSizeInBytes);
		CommRWaddr = (UINT32)NULL;
	}
	#if defined(TD_SUPPORT)
	if (CommDiagOfflineAddr != (UINT32)NULL)
	{
		MmUnmapIoSpace((PVOID)CommDiagOfflineAddr, g_CommDiagOfflineAreaSizeInBytes);
		CommDiagOfflineAddr = (UINT32)NULL;
	}
	#endif
	#endif
	return;
}


//This routine converst a Physical Addr at Comm Or BOERNE space To Virtual Addr at BOERNE space
UINT32 ConvertPhysicalAddrToVirtualAddr(UINT32 PhysicalAddr)
{
	UINT32 ret_Addr = PhysicalAddr;
	UINT32 StartAddr, OffsetInBytes;
	UINT32 temp;

	StartAddr = PRE_LOADTABLE_PHYADDR;
	OffsetInBytes = LOAD_TABLE_OFFSET + sizeof(LoadTableType);
	temp = (UINT32)ADDR_CONVERT(StartAddr);

	//if((PhysicalAddr >= StartAddr)&&(PhysicalAddr <= StartAddr+OffsetInBytes)){ //Comm Space
	//	ret_Addr = (CommRObaseAddr+(PhysicalAddr-StartAddr));
	//}
	if ((PhysicalAddr >= (UINT32)ADDR_CONVERT(StartAddr)) && (PhysicalAddr <= (UINT32)ADDR_CONVERT(StartAddr + OffsetInBytes)))  //BOERNE Space
	{
		ret_Addr = (CommRObaseAddr + (PhysicalAddr - (UINT32)ADDR_CONVERT(StartAddr)));
		return ret_Addr;
	}

	StartAddr = g_CommRWArea;
	OffsetInBytes = g_CommRWAreaSizeInBytes;
	temp = (UINT32)ADDR_CONVERT(StartAddr);

	//if((PhysicalAddr >= StartAddr)&&(PhysicalAddr <= StartAddr+OffsetInBytes)){
	//		ret_Addr = (CommRWaddr+(PhysicalAddr-StartAddr));
	//}
	if ((PhysicalAddr >= (UINT32)ADDR_CONVERT(StartAddr)) && (PhysicalAddr <= (UINT32)ADDR_CONVERT(StartAddr + OffsetInBytes)))  //BOERNE Space
	{
		ret_Addr = (CommRWaddr + (PhysicalAddr - (UINT32)ADDR_CONVERT(StartAddr)));
		return ret_Addr;
	}
#if defined (TD_SUPPORT)
	StartAddr = g_CommDiagOfflineArea;
	OffsetInBytes = g_CommDiagOfflineAreaSizeInBytes;
	temp = (UINT32)ADDR_CONVERT(StartAddr);

	if ((PhysicalAddr >= (UINT32)ADDR_CONVERT(StartAddr)) && (PhysicalAddr <= (UINT32)ADDR_CONVERT(StartAddr + OffsetInBytes)))  //BOERNE Space
	{
		ret_Addr = (CommDiagOfflineAddr + (PhysicalAddr - (UINT32)ADDR_CONVERT(StartAddr)));
	}
#endif
	//RETAILMSG(1, (TEXT("[SHACHAR_DEBUG] ** VA Conv PhysicalAddr=%x	VA=%x	ADDR_CONVERT(VA)=%x ** .\r\n"),PhysicalAddr,ret_Addr,(UINT32)ADDR_CONVERT(ret_Addr)));


	return(ret_Addr);
}

//#ifdef _DIAG_ENABLED_
//#include "diag.h"

//ICAT EXPORTED FUNCTION - Diag,comMem,printComLoadTable
void printComLoadTable(void)
{
	if ((UINT32)pLoadTable != INVALID_ADDRESS)
	{
		DIAG_FILTER(Diag, comMem, image, DIAG_INFORMATION)
		diagPrintf("COMM image BEGIN=0x%lx,  END=0x%lx, rw-ram-Begin=0x%lx",
			   ADDR_CONVERT(pLoadTable->imageBegin), ADDR_CONVERT(pLoadTable->imageEnd),
			   ADDR_CONVERT(pLoadTable->ramRWbegin) );
	}
	else
	{
		DIAG_FILTER(Diag, comMem, imageNO, DIAG_INFORMATION)
		diagPrintf("COMM   loadTable not found");
	}
}
//#endif

