/*
 * PXA910 CP related
 *
 * This software program is licensed subject to the GNU General Public License
 * (GPL).Version 2,June 1991, available at http://www.fsf.org/copyleft/gpl.html

 * (C) Copyright 2007 Marvell International Ltd.
 * All Rights Reserved
 */

#include "linux_types.h"

#define LOAD_TABLE_OFFSET       0x1C0
#define LT_APP2COM_DATA_LEN     48

struct cp_load_table_head
{
	UINT32 b2init;
	UINT32 init;
	UINT32 addrLoadTableRWcopy;
	UINT32 ee_postmortem;
	UINT32 numOfLife;

	UINT32 diag_p_diagIntIfQPtrData;
	UINT32 diag_p_diagIntIfReportsList;
	UINT32 spare_CCCC;

	UINT32 ACIPCBegin;
	UINT32 ACIPCEnd;
	UINT32 LTEUpBegin;
	UINT32 LTEUpEnd;
	UINT32 LTEDownBegin;
	UINT32 LTEDownEnd;
	UINT32 apps2commDataLen;
	UINT8  apps2commDataBuf[LT_APP2COM_DATA_LEN];

	UINT8  filler[LOAD_TABLE_OFFSET-(4*15)-LT_APP2COM_DATA_LEN-4];
	UINT32 imageBegin;
	UINT32 imageEnd;
	char   Signature[16];
	UINT32 sharedFreeBegin;
	UINT32 sharedFreeEnd;
	UINT32 ramRWbegin;
	UINT32 ramRWend;
	UINT32 spare_EEEE;
	UINT32 ReliableDataBegin;
	UINT32 ReliableDataEnd;

	char   OBM_VerString[8];
	UINT32 OBM_Data32bits;
};

