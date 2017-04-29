/** @file DutFmu.cpp
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#if defined (_FM_) 

#include "../../DutCommon/Clss_os.h"
#include "../../DutCommon/mathUtility.h"
#include "../../DutCommon/utility.h"
#include "../../DutCommon/utilities.h" 

#include "../DutBtApi878XDll/DutBtApiClss.h"
#include "../DutApi878XDll/DutDllVerNo.h"
#include "../DutBtApi878XDll/BtMfgCmd.h"
#include  "../DutBtApi878XDll/HciStruct.h"

#include "FmMfgCmd.h"

#ifdef _LINUX_
BOOL queryHci(ULONG OID, PUCHAR Buf, UINT BufSize,
		UINT ExpectedEvent, UINT *BufRetSize);
#endif

int DutBtApiClssHeader WriteFmuReg(DWORD address, DWORD data)
{
	int status=0;

#if defined (_HCI_PROTOCAL_)
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_BT_CMD_FMU_REG
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmReg_t TxBuf={0};
 
	if(2 == BtIf_GetDebugLogFlag()) 
		DebugLogRite ("WriteFmuReg\t0x%04X = 0x%08X\n", address, data);

	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf...
	TxBuf.Address = address;
	TxBuf.Data = data;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);

 
#else // #if defined (_HCI_PROTOCAL_)

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_FMU_REG
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmReg_t TxBuf={0};
 
	if(2 == BtIf_GetDebugLogFlag()) 
		DebugLogRite ("WriteFmuReg\t0x%04X = 0x%08X\n", address, data);

	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf...
	TxBuf.Address = address;
 	TxBuf.Data = data; 

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);

#endif // #if defined (_HCI_PROTOCAL_)
	return status;
}


int DutBtApiClssHeader ReadFmuReg(DWORD address, DWORD *data)
{
	int status=0;
#if defined (_HCI_PROTOCAL_)
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_BT_CMD_FMU_REG
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_GET

	mfgFm_CmdFmReg_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf...
	TxBuf.Address = address;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(status) return status;

	if(data) *data = TxBuf.Data; 

	if(2 == BtIf_GetDebugLogFlag()) 
		DebugLogRite ("ReadFmReg\t0x%04X = 0x%08X\n", address, *data);

 
	return status;

#else // #if defined (_HCI_PROTOCAL_)

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_FMU_REG
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_GET

	mfgFm_CmdFmReg_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf...
	TxBuf.Address = address;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(status) return status;

	if(data) *data = TxBuf.Data; 

	if(2 == BtIf_GetDebugLogFlag()) 
		DebugLogRite ("ReadFmReg\t0x%04X = 0x%08X\n", address, *data);

#endif // #if defined (_HCI_PROTOCAL_)

	return status;
}
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

/* // TBRM 
int DutBtApiClssHeader FmReset(void)
{

#if defined (_HCI_PROTOCAL_)

	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
	#undef THIS_EVENT 
	#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmd	*pTxBuf=NULL;
 	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmd*) pBuf;
 	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_RXRESET);



	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) return status;
 
	return status;
#else // #if defined (_HCI_PROTOCAL_)
	int status=0;

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_RESET
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_Cmd_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}
*/
  
int DutBtApiClssHeader FmInitialization(int XtalFreqInKHz) // , int FreqErrPpm)
{
#if defined (_HCI_PROTOCAL_)

	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
	#undef THIS_EVENT 
	#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)
	int status = 0;
	Bt_FmBCmdD	*pTxBuf=NULL;
 	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdD*) pBuf;
 	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_RXINIT);
	pTxBuf->Data = XtalFreqInKHz*1000;

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
 
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_INIT
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
	TxBuf.Data = XtalFreqInKHz*1000;
//	TxBuf.Data2 = FreqErrPpm;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	return status;

#endif //#if defined (_HCI_PROTOCAL_)
}

int DutBtApiClssHeader FmPowerUpMode(int PowerMode)
{
#if defined (_HCI_PROTOCAL_)
	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
	#undef THIS_EVENT 
	#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)
	int status = 0;
	Bt_FmBCmdB	*pTxBuf=NULL;
 	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdB*) pBuf;
 	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_POWERUPMODE);

	pTxBuf->Data = PowerMode;

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
 
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_FMPOWERUPMODE
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
	TxBuf.Data = PowerMode;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	return status;

#endif //#if defined (_HCI_PROTOCAL_)
}




int DutBtApiClssHeader SetFmChannel(DWORD *pFmChInKHz)
{
#if defined (_HCI_PROTOCAL_)
	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
	#undef THIS_EVENT 
	#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmdD	*pTxBuf=NULL;
	Bt_HciFmEventD	*pRxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(pRxBuf)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(pRxBuf)));
	pTxBuf = (Bt_FmBCmdD*) pBuf;
	pRxBuf= (Bt_HciFmEventD*)pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SETFMCH);
	pTxBuf->Data = (*pFmChInKHz);

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) 
	{
	    if(pBuf) free(pBuf);
	    return status;
    } 
	
	(*pFmChInKHz) = pRxBuf->Data;

    if(pBuf) free(pBuf);
	
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_FMCHAN
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
	TxBuf.Data = *pFmChInKHz;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(status) return status;

	(*pFmChInKHz) =	TxBuf.Data;

	return status;

#endif //#if defined (_HCI_PROTOCAL_)
}

int DutBtApiClssHeader GetFmChannel(DWORD *pFmChInKHz)
{
#if defined (_HCI_PROTOCAL_)
	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
	#undef THIS_EVENT 
	#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmd	*pTxBuf=NULL;
	Bt_HciFmEventD	*pRxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmd*) pBuf;
	pRxBuf= (Bt_HciFmEventD*)pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_GETFMCH);

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) 
	{
	    if(pBuf) free(pBuf);
	    return status;
    }
	
	if(pFmChInKHz) 
		(*pFmChInKHz) = pRxBuf->Data;
 
    if(pBuf) free(pBuf);

	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_FMCHAN
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_GET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(status) return status;
	// get Get values
	// TxBuf... 
	if(pFmChInKHz) 
		(*pFmChInKHz) = (TxBuf).Data;
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)
}



int DutBtApiClssHeader SetFmAfChannel(DWORD *pFmAfChInKHz, BYTE AfMode)
{
#if defined (_HCI_PROTOCAL_)
	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
	#undef THIS_EVENT 
	#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmdDB	*pTxBuf=NULL;
	Bt_HciFmEventD	*pRxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmdDB*) pBuf;
	pRxBuf= (Bt_HciFmEventD*)pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SETFMAFCH);
	pTxBuf->DataD = (*pFmAfChInKHz);
	pTxBuf->DataB = AfMode;

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf);
	if(status) 
	{
	    if(pBuf) free(pBuf);
	    return status;
    }
	
	*pFmAfChInKHz = pRxBuf->Data;
 
    if(pBuf) free(pBuf);

	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_AFCHAN
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD3_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 	TxBuf.Data1 = *pFmAfChInKHz;
 	TxBuf.Data3 = AfMode;


 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(status) return status;
	// get Get values
	// TxBuf... 
	*pFmAfChInKHz = TxBuf.Data2;
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)
}

/*
int DutBtApiClssHeader GetFmAfChannel(DWORD *pFmAfChInKHz)
{
#if defined (_HCI_PROTOCAL_)
	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
	#undef THIS_EVENT 
	#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmd	*pTxBuf=NULL;
	Bt_HciFmEventD	*pRxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmd*) pBuf;
	pRxBuf= (Bt_HciFmEventD*)pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_GETFMAFCH);

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) return status;

	if(pFmAfChInKHz) 
		(*pFmAfChInKHz) = pRxBuf->Data;
 
	return status;
#else // #if defined (_HCI_PROTOCAL_)
	int status=0;

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_AFCHAN
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_GET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(status) return status;
	// get Get values
	// TxBuf... 
	if(pFmAfChInKHz) 
		(*pFmAfChInKHz) = (TxBuf).data;
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)
}

*/


int DutBtApiClssHeader SetFmSearchDir(BYTE FmSearchDir_F0B1)
{
#if defined (_HCI_PROTOCAL_)
	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
	#undef THIS_EVENT 
	#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)
	int status = 0;
	Bt_FmBCmdB	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdB*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SETFMSEARCHDIR);
	pTxBuf->Data = FmSearchDir_F0B1;


	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf);
    if(pBuf) free(pBuf);
 
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_SEARCH_DIR
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
	TxBuf.Data = FmSearchDir_F0B1;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)
}

int DutBtApiClssHeader GetFmSearchDir(DWORD *pFmSearchDir_F0B1)
{
#if defined (_HCI_PROTOCAL_)
	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
	#undef THIS_EVENT 
	#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)
	int status = 0;
	Bt_FmBCmd	*pTxBuf=NULL;
	Bt_HciFmEventD	*pRxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmd*) pBuf;
	pRxBuf= (Bt_HciFmEventD*)pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_GETFMSEARCHDIR);

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) 
	{
	    if(pBuf) free(pBuf);
	    return status;
    }
	if(pFmSearchDir_F0B1) 
		(*pFmSearchDir_F0B1) = pRxBuf->Data;

    if(pBuf) free(pBuf);
 
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_SEARCH_DIR
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_GET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(status) return status;
	// get Get values
	// TxBuf... 
	if(pFmSearchDir_F0B1) 
		(*pFmSearchDir_F0B1) = (TxBuf).Data;
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)
}



int DutBtApiClssHeader SetFmAutoSearchMode(BYTE FmAutoSearchMode)
{
#if defined (_HCI_PROTOCAL_)
	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
	#undef THIS_EVENT 
	#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)
	int status = 0;
	Bt_FmBCmdB	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdB*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SETFMAUTOSEARCHMODE);
	pTxBuf->Data = FmAutoSearchMode;


	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
 
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_AUTOSEARCHMODE
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
	TxBuf.Data = FmAutoSearchMode;
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)
}

int DutBtApiClssHeader GetFmAutoSearchMode(DWORD *pFmAutoSearchMode)
{
#if defined (_HCI_PROTOCAL_)
	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
	#undef THIS_EVENT 
	#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)
	int status = 0;
	Bt_FmBCmd	*pTxBuf=NULL;
	Bt_HciFmEventD	*pRxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmd*) pBuf;
	pRxBuf= (Bt_HciFmEventD*)pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_GETFMAUTOSEARCHMODE);

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
 
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_AUTOSEARCHMODE
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_GET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(status) return status;
	// get Get values
	// TxBuf... 
	if(pFmAutoSearchMode) 
		(*pFmAutoSearchMode) = (TxBuf).Data;
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)
}


int DutBtApiClssHeader SetFmRxForceStereoMode(BYTE FmForceStereoMode)
{
	// this is host implemeted MFG only command, for now, there is HCI tool box match to it for force=true.

	int status=0;

	if(2 == BtIf_GetDebugLogFlag()) 
	{
		int StereoStatus=0;

		GetFmStereoStatus(&StereoStatus);
		DebugLogRite ("StereoStatus %d\n", StereoStatus);

	}

	if(FmForceStereoMode)
	{
		DWORD tempInt=0;


		status = ReadFmuReg(0x850, &tempInt);
		if(status) return status;
		tempInt &= (~0x00000FFF);
		tempInt |= (0x00000030);
		status = WriteFmuReg(0x850, tempInt);
		if(status) return status;

		status = ReadFmuReg(0x854, &tempInt);
		if(status) return status;
		tempInt &= (~0x00000FFF);
		tempInt |= (0x00000030);
		status = WriteFmuReg(0x854, tempInt);
		if(status) return status;

		status = ReadFmuReg(0xac8, &tempInt);
		if(status) return status;
		tempInt &=(~0x000000FF);
		tempInt |=(0x00000000);
		status = WriteFmuReg(0xac8, tempInt);
		if(status) return status;

		status = SetFmStereoBlendingMode(1); // soft blending mode
		if(status) return status;


	}
	else
	{
		status = SetFmRxForceMonoMode(true);
		if(status) return status;
		status = SetFmRxForceMonoMode(false);
		if(status) return status;
	}

	if(2 == BtIf_GetDebugLogFlag()) 
	{
		int StereoStatus=0;

		GetFmStereoStatus(&StereoStatus);
		DebugLogRite ("StereoStatus %d\n", StereoStatus);

	}

	return status;

}


int DutBtApiClssHeader SetFmRxForceMonoMode(BYTE FmForceMonoMode)
{
#if defined (_HCI_PROTOCAL_)
	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
	#undef THIS_EVENT 
	#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)
	int status = 0;
	Bt_FmBCmdB	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdB*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SETFMFORCEMONOMODE);
	pTxBuf->Data = FmForceMonoMode;


	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
 
	return status;
#else // #if defined (_HCI_PROTOCAL_)
	int status=0;

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_RX_FORCE_MONOMODE
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
	(TxBuf).Data = FmForceMonoMode;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 
  
	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}

int DutBtApiClssHeader GetFmRxForceMonoMode(DWORD *pFmForceMonoMode)
{
#if defined (_HCI_PROTOCAL_)
	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
	#undef THIS_EVENT 
	#define THIS_EVENT	 ((HCI_EVENT_MRVL_FM_MONOSTEROSTATUS<<8)|HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmd	*pTxBuf=NULL;
	Bt_HciFmEventB	*pRxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmd*) pBuf;
	pRxBuf= (Bt_HciFmEventB*)pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_GETFMFORCEMONOMODE);

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) 
	{
	    if(pBuf) free(pBuf);
	    return status;
    }
	
	if(pFmForceMonoMode) 
		(*pFmForceMonoMode) = pRxBuf->Data;

    if(pBuf) free(pBuf);
 
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_RX_FORCE_MONOMODE
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_GET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(status) return status;
	// get Get values
	// TxBuf... 
 	if(pFmForceMonoMode) 
		(*pFmForceMonoMode) = TxBuf.Data;
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}


int DutBtApiClssHeader SetFmRssiThreshold(DWORD FmRssiThreshold)
{
#if defined (_HCI_PROTOCAL_)

	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
	#undef THIS_EVENT 
	#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)
	int status = 0;
	Bt_FmBCmdD	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdD*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SETFMRSSITHRESHOLD);
	pTxBuf->Data = FmRssiThreshold;


	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
 
	return status;
#else // #if defined (_HCI_PROTOCAL_)
	int status=0;

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_RSSI_THRESHOLD
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
	TxBuf.Data = FmRssiThreshold;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 
  
	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}

int DutBtApiClssHeader GetFmRssiThreshold(DWORD *pFmRssiThreshold)
{
#if defined (_HCI_PROTOCAL_)
	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
	#undef THIS_EVENT 
	#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)
	int status = 0;
	Bt_FmBCmd	*pTxBuf=NULL;
	Bt_HciFmEventD	*pRxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmd*) pBuf;
	pRxBuf= (Bt_HciFmEventD*)pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_GETFMRSSITHRESHOLD);

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) 
	{
	    if(pBuf) free(pBuf);
	    return status;
    }
    
	if(pFmRssiThreshold) 
		(*pFmRssiThreshold) = pRxBuf->Data;
 
    if(pBuf) free(pBuf);

	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_RSSI_THRESHOLD
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_GET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(status) return status;
	// get Get values
	// TxBuf... 
	if(pFmRssiThreshold) 
		(*pFmRssiThreshold) = TxBuf.Data;

	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}

int DutBtApiClssHeader SetFmCmiThreshold(DWORD FmCmiThreshold)
{
#if defined (_HCI_PROTOCAL_)

	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
	#undef THIS_EVENT 
	#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)
	int status = 0;
	Bt_FmBCmdD	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdD*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SETFMCMITHRESHOLD);
	pTxBuf->Data = FmCmiThreshold;

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
 
	return status;
#else // #if defined (_HCI_PROTOCAL_)
	int status=0;

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_CMI_THRESHOLD
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
	TxBuf.Data = FmCmiThreshold;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 
  
	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}

int DutBtApiClssHeader GetFmCmiThreshold(DWORD *pFmCmiThreshold)
{
#if defined (_HCI_PROTOCAL_)
	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
	#undef THIS_EVENT 
	#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)
	int status = 0;
	Bt_FmBCmd	*pTxBuf=NULL;
	Bt_HciFmEventD	*pRxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmd*) pBuf;
	pRxBuf= (Bt_HciFmEventD*)pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_GETFMCMITHRESHOLD);

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) 
	{
	    if(pBuf) free(pBuf);
	    return status;
    }
    
	if(pFmCmiThreshold) 
		(*pFmCmiThreshold) = pRxBuf->Data;
 
    if(pBuf) free(pBuf);

	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_CMI_THRESHOLD
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_GET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(status) return status;
	// get Get values
	// TxBuf... 
	if(pFmCmiThreshold) 
		(*pFmCmiThreshold) = TxBuf.Data;

	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}

int DutBtApiClssHeader SetFmBand(BYTE FmBand)
{
#if defined (_HCI_PROTOCAL_)
	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
	#undef THIS_EVENT 
	#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)
	int status = 0;
	Bt_FmBCmdB	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdB*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SETFMBAND);
	pTxBuf->Data = FmBand;

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
 
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_FMBAND
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
	TxBuf.Data = FmBand;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 

	return status;

#endif //#if defined (_HCI_PROTOCAL_)
}

/* // TBRM 
int DutBtApiClssHeader GetFmBand(DWORD *pFmBand)
{
#if defined (_HCI_PROTOCAL_)
	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
	#undef THIS_EVENT 
	#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)
	int status = 0;
	Bt_FmBCmd	*pTxBuf=NULL;
	Bt_HciFmEventD	*pRxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmd*) pBuf;
	pRxBuf= (Bt_HciFmEventD*)pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_GETFMBAND);

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) return status;

	if(pFmBand) (*pFmBand) = pRxBuf->Data;
 
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_FMBAND
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_GET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(status) return status;
	// get Get values
	// TxBuf... 
	if(pFmBand) (*pFmBand) = TxBuf.Data;

	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}
// TBRM  */




int DutBtApiClssHeader SetFmChStepSizeKHz(DWORD StepSizeKHz)
{
#if defined (_HCI_PROTOCAL_)
	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
	#undef THIS_EVENT 
	#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)
	int status = 0;
	Bt_FmBCmdD	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdD*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SETSTEPSIZE);
	pTxBuf->Data = StepSizeKHz;

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
 
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_CHAN_STEPSIZE
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
	TxBuf.Data = StepSizeKHz;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 

	return status;

#endif //#if defined (_HCI_PROTOCAL_)
}

/* // TBRM 
int DutBtApiClssHeader GetFmChStepSizeKHz(DWORD *pStepSizeKHz)
{
#if defined (_HCI_PROTOCAL_)
	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
	#undef THIS_EVENT 
	#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)
	int status = 0;
	Bt_FmBCmd	*pTxBuf=NULL;
	Bt_HciFmEventD	*pRxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmd*) pBuf;
	pRxBuf= (Bt_HciFmEventD*)pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_GETSTEPSIZE);



	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) return status;

	if(pStepSizeKHz) (*pStepSizeKHz) = pRxBuf->Data;
 
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_CHAN_STEPSIZE
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_GET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(status) return status;
	// get Get values
	// TxBuf... 
	if(pStepSizeKHz) (*pStepSizeKHz) = TxBuf.Data;

	return status;

#endif //#if defined (_HCI_PROTOCAL_)
}
// TBRM */



int DutBtApiClssHeader FmMoveChUp(DWORD *pFmChInKHz)
{
#if defined (_HCI_PROTOCAL_)
	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
	#undef THIS_EVENT 
	#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmd	*pTxBuf=NULL;
	Bt_HciFmEventD	*pRxBuf=NULL;
 	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmd*) pBuf;
 	pRxBuf = (Bt_HciFmEventD*) pBuf;

	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_CHUP);

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) 
	{
	    if(pBuf) free(pBuf);
        return status;
    }
    
	if (pFmChInKHz) (*pFmChInKHz) = pRxBuf->Data;

    if(pBuf) free(pBuf);
	
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_CHAN_STEPUPDN
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
	TxBuf.Data = 0;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(status) return status;
	// get Get values
	// TxBuf... 
	if(pFmChInKHz) (*pFmChInKHz) = TxBuf.Data;

	return status;

#endif //#if defined (_HCI_PROTOCAL_)
}

int DutBtApiClssHeader FmMoveChDown(DWORD *pFmChInKHz)
{
#if defined (_HCI_PROTOCAL_)
	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
	#undef THIS_EVENT 
	#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)
	int status = 0;
	Bt_FmBCmd	*pTxBuf=NULL;
	Bt_HciFmEventD	*pRxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmd*) pBuf;
	pRxBuf = (Bt_HciFmEventD*) pBuf;

	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_CHDOWN);

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) 
	{
	    if(pBuf) free(pBuf);
        return status;
    }
    
 	if (pFmChInKHz) (*pFmChInKHz) = pRxBuf->Data;
 
    if(pBuf) free(pBuf);

	return status;
#else // #if defined (_HCI_PROTOCAL_)
	int status=0;

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_CHAN_STEPUPDN
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
	TxBuf.Data = 1;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(status) return status;
	// get Get values
	// TxBuf... 
	if(pFmChInKHz) (*pFmChInKHz) = TxBuf.Data;

	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}

int DutBtApiClssHeader GetFmFwVersion(DWORD *pFwVersion)
{
#if defined (_HCI_PROTOCAL_)
	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
	#undef THIS_EVENT 
	#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)
	int status = 0;
	Bt_FmBCmd	*pTxBuf=NULL;
	Bt_HciFmEventD	*pRxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmd*) pBuf;
	pRxBuf= (Bt_HciFmEventD*)pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_GETFWVERSION);

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) 
	{
	    if(pBuf) free(pBuf);
	    return status;
    }
    
	if(pFwVersion) (*pFwVersion) = pRxBuf->Data;
 
    if(pBuf) free(pBuf);

	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_FW_VERS
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_GET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(status) return status;
	// get Get values
	// TxBuf... 
	if(pFwVersion) (*pFwVersion) = TxBuf.Data;

	return status;

#endif //#if defined (_HCI_PROTOCAL_)
}

int DutBtApiClssHeader GetFmHwVersion(DWORD *pHwVersion)
{
#if defined (_HCI_PROTOCAL_)
	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
	#undef THIS_EVENT 
	#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)
	int status = 0;
	Bt_FmBCmd	*pTxBuf=NULL;
	Bt_HciFmEventD	*pRxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmd*) pBuf;
	pRxBuf= (Bt_HciFmEventD*)pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_GETHWVERSION);

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) 
	{
	    if(pBuf) free(pBuf);
	    return status;
    }
    
	if(pHwVersion) (*pHwVersion) = pRxBuf->Data;
    if(pBuf) free(pBuf);

	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_HW_VERS
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_GET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(status) return status;
	// get Get values
	// TxBuf... 
	if(pHwVersion) (*pHwVersion) = TxBuf.Data;

	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}


int DutBtApiClssHeader GetFmHwId(DWORD *pHwId)
{
#if defined (_HCI_PROTOCAL_)
	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
	#undef THIS_EVENT 
	#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)
	int status = 0;
	Bt_FmBCmd	*pTxBuf=NULL;
	Bt_HciFmEventD	*pRxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmd*) pBuf;
	pRxBuf= (Bt_HciFmEventD*)pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_GETHWID);

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) 
	{
	    if(pBuf) free(pBuf);
	    return status;
    }
	if(pHwId) (*pHwId) = pRxBuf->Data;
    if(pBuf) free(pBuf);

	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_HW_ID
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_GET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(status) return status;
	// get Get values
	// TxBuf... 
	if(pHwId) (*pHwId) = TxBuf.Data;

	return status;

#endif //#if defined (_HCI_PROTOCAL_)
}


int DutBtApiClssHeader GetFmManId(DWORD *pVId)
{
#if defined (_HCI_PROTOCAL_)
	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
	#undef THIS_EVENT 
	#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)
	int status = 0;
	Bt_FmBCmd	*pTxBuf=NULL;
	Bt_HciFmEventD	*pRxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmd*) pBuf;
	pRxBuf= (Bt_HciFmEventD*)pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_GETMANUFACTUREID);

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) 
	{
	    if(pBuf) free(pBuf);
	    return status;
    }
	if(pVId) (*pVId) = pRxBuf->Data;
    if(pBuf) free(pBuf);

	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_VENDOR_ID
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_GET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(status) return status;
	// get Get values
	// TxBuf... 
	if(pVId) (*pVId) = TxBuf.Data;

	return status;

#endif //#if defined (_HCI_PROTOCAL_)
}




int DutBtApiClssHeader GetFmCurrentRssi(DWORD *pCurrentRssi)
{
#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 ((HCI_EVENT_VENDOR_SPECIFIC<<8)| HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmd	*pTxBuf=NULL;
	Bt_HciFmEventW	*pRxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmd*) pBuf;
	pRxBuf= (Bt_HciFmEventW*)pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_GETCURRENTRSSI);

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) 
	{
	    if(pBuf) free(pBuf);
	    return status;
    }
	if(pCurrentRssi) 
		(*pCurrentRssi) = pRxBuf->Data;
    if(pBuf) free(pBuf);

	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_CURRENT_RSSI
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_GET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(status) return status;
	// get Get values
	// TxBuf... 
	if(pCurrentRssi) 
		(*pCurrentRssi) = TxBuf.Data;

	return status;

#endif //#if defined (_HCI_PROTOCAL_)
}


int DutBtApiClssHeader GetFmCurrentCmi(DWORD *pCurrentCmi)
{
#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 ((HCI_EVENT_VENDOR_SPECIFIC<<8)| HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmd	*pTxBuf=NULL;
	Bt_HciFmEventW	*pRxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmd*) pBuf;
	pRxBuf= (Bt_HciFmEventW*)pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_GETCURRENTCMI);

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) 
	{
	    if(pBuf) free(pBuf);
        return status;
    }
    
	if(pCurrentCmi) 
		(*pCurrentCmi) = pRxBuf->Data;
    if(pBuf) free(pBuf);

	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_CURRENT_CMI
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_GET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(status) return status;
	// get Get values
	// TxBuf... 
	if(pCurrentCmi) 
		(*pCurrentCmi) = TxBuf.Data;

	return status;

#endif //#if defined (_HCI_PROTOCAL_)
}




/* TBD_KY

int DutBtApiClssHeader SetFmReferenceClk(int CalFreqInKHz) 

{
#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmdD	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmdD*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_REFCLK);
	pTxBuf->Data = CalFreqInKHz*1000;

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) return status;

  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_REFCLK
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
	TxBuf.Data = CalFreqInKHz;
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)
}


*/

int DutBtApiClssHeader SetFmReferenceClkError(int FreqErrPpm)

{
#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmdD2	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdD2*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_REFCLKERR);
	pTxBuf->Data1 = abs(FreqErrPpm);
	if(0 != FreqErrPpm)
		pTxBuf->Data2 = FreqErrPpm/abs(FreqErrPpm);
	else 
		pTxBuf->Data2=1;

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_REFERNCECLKERR
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD2_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
	TxBuf.Data1 = abs(FreqErrPpm);
 	if(0 != FreqErrPpm)
		TxBuf.Data2 = FreqErrPpm/abs(FreqErrPpm);
	else 
		TxBuf.Data2=1;


 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)
}
 




int DutBtApiClssHeader ResetFmRdsBLER(int SecPeriod)
{
#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
 
	Bt_FmBCmdD	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdD*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_RDSBLER_RESET);
	pTxBuf->Data = SecPeriod;

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
 
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_RESETRDSBLER
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
	TxBuf.Data = SecPeriod;
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)
}




int DutBtApiClssHeader GetFmRdsBLER(int *pSeqGood, int *pSeqNoSync, 
									int *pExpected, int *pSuccRate)

{
#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
 
	Bt_FmBCmd	*pTxBuf=NULL;
	Bt_HciFmEventD4 *pRxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);
	
	if(pSeqGood)	*pSeqGood = 0;
	if(pSeqNoSync)	*pSeqNoSync = 0;
	if(pExpected)	*pExpected = 0;
	if(pSuccRate)	*pSuccRate = 0;

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmd*) pBuf;
	pRxBuf = (Bt_HciFmEventD4*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_RDSBLER_STAT);
 
	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) 
	{
	    if(pBuf) free(pBuf);
    	return status;
    }
    
	if(pSeqGood)	*pSeqGood = pRxBuf->Data1;
	if(pSeqNoSync)	*pSeqNoSync = pRxBuf->Data2;
	if(pExpected)	*pExpected = pRxBuf->Data3;
	if(pSuccRate)	*pSuccRate = pRxBuf->Data4;

    if(pBuf) free(pBuf);
	
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_GETRDSBLER
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_GET

	mfgFm_CmdFmDataD4_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
  
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(status) return status;
	// get Get values
	// TxBuf... 
	if(pSeqGood)	*pSeqGood = TxBuf.Data1;
	if(pSeqNoSync)	*pSeqNoSync = TxBuf.Data2;
	if(pExpected)	*pExpected = TxBuf.Data3;
	if(pSuccRate)	*pSuccRate = TxBuf.Data4;
									

	return status;

#endif //#if defined (_HCI_PROTOCAL_)
}




// 0: 75Ohm, 1: TxLoop 
int DutBtApiClssHeader SelectFmRxAntennaType(int AntType)

{
#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmdB	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdB*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SELECTRXANTTYPE);
	pTxBuf->Data = AntType;

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_SELECTRXANTTYPE
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
	TxBuf.Data = AntType;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)
}


int DutBtApiClssHeader SetFmIrsMask(
				int RssiLow,	int NewRdsData,	int RssiIndicator,	int BandLimit, 
				int SyncLost,	int SearchStop, int BMatch,			int PiMatched,
				int MonoStereoChanged,	int AudioPause, int CmiLow)
{
#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)
	int status = 0;
	Bt_FmBCmdD	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdD*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SETIRSMASK);
	pTxBuf->Data = ((RssiLow&0x01)<<0) | ((NewRdsData&0x01)<<1) | 
					 ((RssiIndicator&0x01)<<2) | ((BandLimit&0x01)<<3) | 
					 ((SyncLost&0x01)<<4) | ((SearchStop&0x01)<<5) | 
					 ((BMatch&0x01)<<6) | ((PiMatched&0x01)<<7) |
					 ((MonoStereoChanged&0x01)<<8) | ((AudioPause&0x01)<<9) | 
					 ((CmiLow&0x01)<<10) ;

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_IRSMASK
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
	TxBuf.Data =	((RssiLow&0x01)<<0) | ((NewRdsData&0x01)<<1) | 
					 ((RssiIndicator&0x01)<<2) | ((BandLimit&0x01)<<3) | 
					 ((SyncLost&0x01)<<4) | ((SearchStop&0x01)<<5) | 
					 ((BMatch&0x01)<<6) | ((PiMatched&0x01)<<7) |
					 ((MonoStereoChanged&0x01)<<8) | ((AudioPause&0x01)<<9) | 
					 ((CmiLow&0x01)<<10) ;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)
}


int DutBtApiClssHeader GetFmIrsMask(int *pRssiLow,		int *pNewRdsData,	int *pRssiIndicator,	int *pBandLimit, 
				 int *pSyncLost,	int *pSearchStop,	int *pBMatch,			int *pPiMatched,
				 int *pMonoStereoChanged,	int *pAudioPause, int *pCmiLow)
{
#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmd	*pTxBuf=NULL;
	Bt_HciFmEventD	*pRxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmd*) pBuf;
	pRxBuf = (Bt_HciFmEventD*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_GETIRSMASK);

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) 
	{
	    if(pBuf) free(pBuf);
	    return status;
    }

	if(pRssiLow)			(*pRssiLow)			= ((pRxBuf->Data>>0) &0x1);
	if(pNewRdsData)			(*pNewRdsData)		= ((pRxBuf->Data>>1) &0x1);
	if(pRssiIndicator)		(*pRssiIndicator)	= ((pRxBuf->Data>>2) &0x1);
	if(pBandLimit)			(*pBandLimit)		= ((pRxBuf->Data>>3) &0x1);
	if(pSyncLost)			(*pSyncLost)		= ((pRxBuf->Data>>4) &0x1);
	if(pSearchStop)			(*pSearchStop)		= ((pRxBuf->Data>>5) &0x1);
	if(pBMatch)				(*pBMatch)			= ((pRxBuf->Data>>6) &0x1);
	if(pPiMatched)			(*pPiMatched)		= ((pRxBuf->Data>>7) &0x1);
	if(pMonoStereoChanged)	(*pMonoStereoChanged)=((pRxBuf->Data>>8) &0x1);
	if(pAudioPause)			(*pAudioPause)		= ((pRxBuf->Data>>9) &0x1);
	if(pCmiLow)				(*pCmiLow)			= ((pRxBuf->Data>>10)&0x1); 

    if(pBuf) free(pBuf);

	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_IRSMASK
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_GET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(status) return status;
	// get Get values
	// TxBuf... 
	if(pRssiLow)			(*pRssiLow)			= ((TxBuf.Data>>0) &0x1);
	if(pNewRdsData)			(*pNewRdsData)		= ((TxBuf.Data>>1) &0x1);
	if(pRssiIndicator)		(*pRssiIndicator)	= ((TxBuf.Data>>2) &0x1);
	if(pBandLimit)			(*pBandLimit)		= ((TxBuf.Data>>3) &0x1);
	if(pSyncLost)			(*pSyncLost)		= ((TxBuf.Data>>4) &0x1);
	if(pSearchStop)			(*pSearchStop)		= ((TxBuf.Data>>5) &0x1);
	if(pBMatch)				(*pBMatch)			= ((TxBuf.Data>>6) &0x1);
	if(pPiMatched)			(*pPiMatched)		= ((TxBuf.Data>>7) &0x1);
	if(pMonoStereoChanged)	(*pMonoStereoChanged)=((TxBuf.Data>>8) &0x1);
	if(pAudioPause)			(*pAudioPause)		= ((TxBuf.Data>>9) &0x1);
	if(pCmiLow)				(*pCmiLow)			= ((TxBuf.Data>>10)&0x1); 

	return status;

#endif //#if defined (_HCI_PROTOCAL_)
}





int DutBtApiClssHeader GetFmCurrentFlags(DWORD *pFlags)

{
#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 ((HCI_EVENT_VENDOR_SPECIFIC<<8)| HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmd	*pTxBuf=NULL;
	Bt_HciFmEventD	*pRxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmd*) pBuf;
	pRxBuf = (Bt_HciFmEventD*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_GETCURRENTFLAGS);

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) 
	{
	    if(pBuf) free(pBuf);
	    return status;
    }

	if(pFlags)	(*pFlags) = pRxBuf->Data; 
  
    if(pBuf) free(pBuf);

	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_RXCURRENTFLAGS
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_GET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(status) return status;
	// get Get values
	// TxBuf... 
	if(pFlags) 
		(*pFlags) = TxBuf.Data;

	return status;

#endif //#if defined (_HCI_PROTOCAL_)
}

int DutBtApiClssHeader FmPowerDown(void)
{
	
#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmd	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmd*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_FMPOWERDOWN);

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_FMPOWERDOWN
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}


int DutBtApiClssHeader StopFmSearch(void)
{
	
#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmd	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmd*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_STOPSEARCH);

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_STOPSEARCH
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}


/* // TBRM 
//	LoInjectionSide
//	{
//	LOINJECTION_HIGHSIDE,	//Flo=Frx+Fif, 
//	LOINJECTION_LOWSIDE,	//Flo=Frx-Fif, 
//	LOINJECTION_AUTO 
//	}
int DutBtApiClssHeader SetFmLoInjectionSide(int mode)
{
	
#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmdB	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmdB*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SETLOINJECTSIDE);
	pTxBuf->Data = mode;

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) return status;

  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_SETLOINJECTSIDE
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
	TxBuf.Data = mode;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 

	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}
*/


//	emphasis
//	{
//	EMPHASIS_NONE, EMPHASIS_50US, EMPHASIS_75US
//	}
int DutBtApiClssHeader SetFmAudioEmphasis(int Mode)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE) 

	int status = 0;
	Bt_FmBCmdB	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdB*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SETAUDIODE_EMPHASIS);
	pTxBuf->Data = Mode;

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_AUDIODE_EMPHASIS
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
	TxBuf.Data = Mode;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}

/* // TBRM 
int DutBtApiClssHeader GetFmRxAudioDeemphasis(int *pMode)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 ((HCI_EVENT_VENDOR_SPECIFIC<<8)| HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmd	*pTxBuf=NULL;
	Bt_HciFmEventB	*pRxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmd*) pBuf;
	pRxBuf = (Bt_HciFmEventB*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_GETAUDIODE_EMPHASIS);
 
	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) return status;

	if(pMode) (*pMode) = pRxBuf->Data;
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_AUDIODE_RXDEEMPHASIS
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_GET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(status) return status;
	// get Get values
	// TxBuf... 
	if(pMode) (*pMode) = TxBuf.Data;

	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}
*/

//	MuteMode
// {
//	MUTEMODE_ALL_MUTE_OFF,		
//	MUTEMODE_AC_MUTE_ON_L_R,	 
//	MUTEMODE_HARD_MUTE_LEFT_ON,
//  MUTEMODE_HARD_MUTE_LEFT_AC_MUTE_RIGHT,	 
//	MUTEMODE_HARD_MUTE_RIGHT_ON,
//	MUTEMODE_HARD_MUTE_RIGHT_AC_MUTE_LEFT,			
//	MUTEMODE_HARD_MUTE_ON_L_R
// }
int DutBtApiClssHeader SetFmRxMuteMode(int MuteMode) 
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 ((HCI_EVENT_VENDOR_SPECIFIC<<8)| HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmdB	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdB*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SETRXMUTE);
	pTxBuf->Data = MuteMode;

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_RXMUTEMODE
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
	TxBuf.Data = MuteMode;
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}


int DutBtApiClssHeader GetFmRxMuteMode(int *pMuteMode)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 ((HCI_EVENT_VENDOR_SPECIFIC<<8)| HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmd	*pTxBuf=NULL;
	Bt_HciFmEventB *pRxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmd*) pBuf;
	pRxBuf = (Bt_HciFmEventB*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_GETRXMUTE);
 
	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) 
	{
	    if(pBuf) free(pBuf);
	    return status;
    }
    
	if(pMuteMode) (*pMuteMode)=pRxBuf->Data;

    if(pBuf) free(pBuf);

	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_RXMUTEMODE
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_GET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(status) return status;
	// get Get values
	// TxBuf... 
	if(pMuteMode) (*pMuteMode) = TxBuf.Data;

	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}


//	AudioPath
//	{
//	AUDIO_FM,  I2S_FM, I2S_AIU, SQU, SPDIF
//	}
//	I2sOperation
//	{
//  SLAVE, MASTER
//	}
//	I2sMode
//	{
//	I2S, MSB, LSB, PCM
//	}
int DutBtApiClssHeader SetRxAudioPath(BYTE AudioPath,		BYTE I2sOperation,	BYTE I2sMode)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmdB3	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdB3*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SETRXAUDIOPATH);
	pTxBuf->Data1 = AudioPath;
	pTxBuf->Data2 = I2sOperation;
	pTxBuf->Data3 = I2sMode;

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_RXAUDIOPATH
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD3_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 	TxBuf.Data1 = AudioPath;
	TxBuf.Data2 = I2sOperation;
	TxBuf.Data3 = I2sMode;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}


int DutBtApiClssHeader GetRxAudioPath(BYTE *pAudioPath)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 ((HCI_EVENT_VENDOR_SPECIFIC<<8)| HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmd	*pTxBuf=NULL;
	Bt_HciFmEventB *pRxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmd*) pBuf;
	pRxBuf = (Bt_HciFmEventB*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_GETRXAUDIOPATH);
 
	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) 
	{
	    if(pBuf) free(pBuf);
	    return status;
    }
	if(pAudioPath) (*pAudioPath)=pRxBuf->Data;
    if(pBuf) free(pBuf);

	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_RXAUDIOPATH
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_GET

	mfgFm_CmdFmDataD3_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(status) return status;
	// get Get values
	// TxBuf... 
	if(pAudioPath) 
		(*pAudioPath) = (BYTE)TxBuf.Data1;

	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}


  
//	Sampling
//	{
//	SAMP_8K,	SAMP_11D025K,	SAMP_12K,	SAMP_16K,	SAMP_22D05K, 
//	SAMP_24K,	SAMP_32K,		SAMP_44D1K, SAMP_48K
//	}

//	BClk_LrClk
//	{
//	CLK_32X,	CLK_50X,	CLK_64X
//	}
int DutBtApiClssHeader SetFmRxAudioSamplingRate(int Sampling, int BClk_LrClk)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmdB2	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdB2*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SETRXAUDIOSAMPLINGRATE);
	pTxBuf->Data1 = Sampling;
	pTxBuf->Data2 = BClk_LrClk;

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_RXAUDIOSAMPLINGRATE
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD2_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 	TxBuf.Data1 = Sampling;
	TxBuf.Data2 = BClk_LrClk;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}


//	PauseMode
//	{
//	PAUSEMODE_OFF, PAUSEMODE_PAUSESTARTONLY, PAUSEMODE_PAUSESTOPONLY, PAUSEMODE_STARTSTOP
//	}
int DutBtApiClssHeader EnableFmAudioPauseMode(int PauseMode)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmdB	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdB*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SETRXAUDIOPAUSEMODE);
	pTxBuf->Data = PauseMode;

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_RXAUDIOPAUSEMODE
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 	TxBuf.Data = PauseMode;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}

int DutBtApiClssHeader SetFmAudioPauseDuration(WORD PauseDurInMs)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmdW	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdW*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SETRXAUDIOPAUSEDURATION);
	pTxBuf->Data = PauseDurInMs;

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_RXAUDIOPAUSEDURATION
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
	TxBuf.Data = PauseDurInMs;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}


/*
int DutBtApiClssHeader GetFmAudioPauseDuration(WORD *pPauseDurInMs)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmd	*pTxBuf=NULL;
	Bt_HciFmEventW	*pRxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmd*) pBuf;
	pRxBuf = (Bt_HciFmEventW*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_GETRXAUDIOPAUSEDURATION);
 
	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) return status;

	if(pPauseDurInMs) (*pPauseDurInMs) = pRxBuf->Data;

	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_RXAUDIOPAUSEDURATION
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_GET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(status) return status;
	// get Get values
	// TxBuf... 
	if(pPauseDurInMs) (*pPauseDurInMs) = (WORD)TxBuf.Data;

	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}
// TBRM */


int DutBtApiClssHeader SetFmAudioPauseLevel(WORD PauseLevel)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmdW	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdW*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SETRXAUDIOPAUSELEVEL);
	pTxBuf->Data = PauseLevel;

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
 
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_RXAUDIOPAUSELEVEL
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 	TxBuf.Data = PauseLevel;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}


/* // TBRM
int DutBtApiClssHeader GetFmAudioPauseLevel(WORD *pPauseLevel)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmd	*pTxBuf=NULL;
	Bt_HciFmEventW	*pRxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmd*) pBuf;
	pRxBuf = (Bt_HciFmEventW*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_GETRXAUDIOPAUSELEVEL); 

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) return status;

	if(pPauseLevel) (*pPauseLevel) = pRxBuf->Data;

	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_RXAUDIOPAUSELEVEL
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_GET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(status) return status;
	// get Get values
	// TxBuf... 
	if(pPauseLevel) (*pPauseLevel) = (WORD)TxBuf.Data;

	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}
// TBRM */

/*
//	RdsDataPath
//	{
//	RDSDATAPATH_2NOWHERE, RDSDATAPATH_2HCI, RDSDATAPATH_2I2C, RDSDATAPATH_2HFP, RDSDATAPATH_2A2DP	
//	}
int DutBtApiClssHeader SetFmRxRdsDataPath(int Path)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmdB	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmdB*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SETRXRDSDATAPATH);
	pTxBuf->Data = Path;

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) return status;

  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_RXRDSDATAPATH
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
	TxBuf.Data = Path;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}
 



int DutBtApiClssHeader GetFmRxRdsDataPath(int *pPath)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmd	*pTxBuf=NULL;
	Bt_HciFmEventB	*pRxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmd*) pBuf;
	pRxBuf = (Bt_HciFmEventB*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_GETRXRDSDATAPATH); 

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) return status;

	if(pPath) (*pPath) = pRxBuf->Data;
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_RXRDSDATAPATH
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_GET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(status) return status;
	// get Get values
	// TxBuf... 
	if(pPath) (*pPath) = TxBuf.Data;

	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}
*/
//



int DutBtApiClssHeader GetFmRxRdsSyncStatus(BYTE *pSyncStatus)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmd	*pTxBuf=NULL;
	Bt_HciFmEventB	*pRxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmd*) pBuf;
	pRxBuf = (Bt_HciFmEventB*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_GETRXRDSSYNCSTATUS); 

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) 
	{
	    if(pBuf) free(pBuf);
	    return status;
    }
	if(pSyncStatus) (*pSyncStatus) = pRxBuf->Data;
    if(pBuf) free(pBuf);
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_RXRDSSYNCSTATUS
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_GET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(status) return status;
	// get Get values
	// TxBuf... 
	if(pSyncStatus) (*pSyncStatus) = (BYTE)TxBuf.Data;

	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}



int DutBtApiClssHeader FmRxRdsFlush(void) 
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 ((HCI_EVENT_VENDOR_SPECIFIC<<8)| HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmd	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmd*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_FLUSHRDS);

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_FLUSHRDS
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}


int DutBtApiClssHeader SetFmRxRdsMemDepth(BYTE Depth)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmdB	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdB*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SETRDSMEMDEPTH);
	pTxBuf->Data = Depth;

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_RDSMEMDEPTH
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
	TxBuf.Data = Depth;
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}



int DutBtApiClssHeader GetFmRxRdsMemDepth(BYTE *pDepth)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmd	*pTxBuf=NULL;
	Bt_HciFmEventB	*pRxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmd*) pBuf;
	pRxBuf = (Bt_HciFmEventB*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_GETRDSMEMDEPTH); 

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) 
	{
	    if(pBuf) free(pBuf);
	    return status;
    }
	if(pDepth) (*pDepth) = pRxBuf->Data;
    if(pBuf) free(pBuf);

	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_RDSMEMDEPTH
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_GET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(status) return status;
	// get Get values
	// TxBuf... 
	if(pDepth) (*pDepth) = (BYTE)TxBuf.Data;

	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}

 


int DutBtApiClssHeader SetFmRxRdsBlockB(WORD BlockB)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmdW	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdW*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SETRDSBLOCKB);
	pTxBuf->Data = BlockB;

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_RDSBLOCKB
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 	TxBuf.Data = BlockB;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}



int DutBtApiClssHeader GetFmRxRdsBlockB(WORD *pBlockB)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmd	*pTxBuf=NULL;
	Bt_HciFmEventW	*pRxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmd*) pBuf;
	pRxBuf = (Bt_HciFmEventW*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_GETRDSBLOCKB); 

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) 
	{
	    if(pBuf) free(pBuf);
	    return status;
    }
    
	if(pBlockB) (*pBlockB) = pRxBuf->Data;
    if(pBuf) free(pBuf);
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_RDSBLOCKB
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_GET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(status) return status;
	// get Get values
	// TxBuf... 
	if(pBlockB) (*pBlockB) = (WORD)TxBuf.Data;

	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}
 

int DutBtApiClssHeader SetFmRxRdsBlockBMask(WORD BlockBMask)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmdW	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdW*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SETRDSBLOCKBMASK);
	pTxBuf->Data = BlockBMask;

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_RDSBLOCKBMASK
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
	TxBuf.Data = BlockBMask;
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}



int DutBtApiClssHeader GetFmRxRdsBlockBMask(WORD *pBlockBMask)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmd	*pTxBuf=NULL;
	Bt_HciFmEventW	*pRxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmd*) pBuf;
	pRxBuf = (Bt_HciFmEventW*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_GETRDSBLOCKBMASK); 

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) 
	{
	    if(pBuf) free(pBuf);
	    return status;
    }
    
	if(pBlockBMask) (*pBlockBMask) = pRxBuf->Data;
    if(pBuf) free(pBuf);
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_RDSBLOCKBMASK
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_GET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(status) return status;
	// get Get values
	// TxBuf... 
	if(pBlockBMask) (*pBlockBMask) = (WORD)TxBuf.Data;

	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}
 

int DutBtApiClssHeader SetFmRxRdsPiCode(WORD PiCode)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmdW	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdW*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SETRDSPICODE);
	pTxBuf->Data = PiCode;

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_RDSPICODE
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 	TxBuf.Data = PiCode;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 

	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}



int DutBtApiClssHeader GetFmRxRdsPiCode(WORD *pPiCode)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmd	*pTxBuf=NULL;
	Bt_HciFmEventW	*pRxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmd*) pBuf;
	pRxBuf = (Bt_HciFmEventW*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_GETRDSPICODE); 

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) 
	{
	    if(pBuf) free(pBuf);
	    return status;
    }
    
	if(pPiCode) (*pPiCode) = pRxBuf->Data;
    if(pBuf) free(pBuf);

	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_RDSPICODE
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_GET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}

//////////


int DutBtApiClssHeader SetFmRxRdsPiCodeMask(WORD PiCodeMask)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmdW	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdW*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SETRDSPICODEMASK);
	pTxBuf->Data = PiCodeMask;

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_RDSPICODEMASK
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 	TxBuf.Data = PiCodeMask;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}



int DutBtApiClssHeader GetFmRxRdsPiCodeMask(WORD *pPiCodeMask)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmd	*pTxBuf=NULL;
	Bt_HciFmEventW	*pRxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmd*) pBuf;
	pRxBuf = (Bt_HciFmEventW*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_GETRDSPICODEMASK); 

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) 
	{
	    if(pBuf) free(pBuf);
	    return status;
    }
    
	if(pPiCodeMask) (*pPiCodeMask) = pRxBuf->Data;
    if(pBuf) free(pBuf);
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_RDSPICODEMASK
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_GET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(status) return status;
	// get Get values
	// TxBuf... 
	if(pPiCodeMask) (*pPiCodeMask) = (WORD)TxBuf.Data;

	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}



  
//	RdsMode
//	{
//	RDSMODE_RDS_STD, RDSMODE_RBDSWOE
//	}
int DutBtApiClssHeader SetFmRxRdsMode(int RdsMode)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 ((HCI_EVENT_VENDOR_SPECIFIC<<8)| HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmdB	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdB*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SETRDSMODE);
	pTxBuf->Data = RdsMode;

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);

	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_RDSMODE
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 	TxBuf.Data = RdsMode;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}

int DutBtApiClssHeader GetFmRxRdsMode(int *pRdsMode)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmd	*pTxBuf=NULL;
	Bt_HciFmEventB	*pRxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmd*) pBuf;
	pRxBuf = (Bt_HciFmEventB*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_GETRDSMODE); 

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) 
	{
	    if(pBuf) free(pBuf);
	    return status;
    }
	if(pRdsMode) (*pRdsMode) = pRxBuf->Data;
    if(pBuf) free(pBuf);

	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_RDSMODE
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_GET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
	
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(status) return status;
	// get Get values
	// TxBuf... 
	if(pRdsMode) (*pRdsMode) = TxBuf.Data;

	return status;

#endif //#if defined (_HCI_PROTOCAL_)



}

int DutBtApiClssHeader GetFmTxOutputPwr(BYTE *pTxPwr)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmd	*pTxBuf=NULL;
	Bt_HciFmEventB	*pRxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmd*) pBuf;
	pRxBuf = (Bt_HciFmEventB*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_GETTXOUTPUTPOWER);

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) 
	{
	    if(pBuf) free(pBuf);
	    return status;
    }
    
    if(pTxPwr) (*pTxPwr) = pRxBuf->Data;
    if(pBuf) free(pBuf);
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_TXOUTPUTPOWER
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_GET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(status) return status;
	// get Get values
	// TxBuf... 
  	if(pTxPwr) (*pTxPwr) = TxBuf.Data;

	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}


int DutBtApiClssHeader SetFmTxOutputPwr(BYTE TxPwr)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmdB	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdB*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SETTXOUTPUTPOWER);
	pTxBuf->Data = TxPwr;

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_TXOUTPUTPOWER
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 	TxBuf.Data= TxPwr;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}

/* // TBRM 
//	emphasis
//	{
//	EMPHASIS_NONE, EMPHASIS_50US, EMPHASIS_75US
//	}
int DutBtApiClssHeader SetFmAudioPreemphasis(int Mode) 
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmdB	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmdB*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SETTXPREEMPHASIS);
	pTxBuf->Data = Mode;

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) return status;

  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_TXPREEMPHASIS
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 	TxBuf.Data = Mode;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}
// TBRM */

int DutBtApiClssHeader SetFmFreqDeviation(DWORD FreqDeviaInHz)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmdD	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdD*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SETTXFREQDEVIATION);
	pTxBuf->Data = FreqDeviaInHz;

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_TXFREQDEVIATION
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 	TxBuf.Data = FreqDeviaInHz;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}


/*
int DutBtApiClssHeader SearchFmTxFreeCh(DWORD StartChFreqInKhz)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmdD	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmdD*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SERCHTXFREECHAN);
	pTxBuf->Data = StartChFreqInKhz;

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) return status;

  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_SERCHTXFREECHAN
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 	TxBuf.Data = StartChFreqInKhz;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}
*/

int DutBtApiClssHeader SetFmTxMonoStereoMode(BYTE FmMonoStereoMode)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmdB	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdB*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SETTXMONOSTEREO);
	pTxBuf->Data = FmMonoStereoMode;

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_TXMONOSTEREO
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
	TxBuf.Data = FmMonoStereoMode;
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 

	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}


  
//	MuteMode
// {
// OFF, ON
// }
int DutBtApiClssHeader SetFmTxMuteMode(int MuteMode)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmdB	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdB*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SETTXMUTE); 
	pTxBuf->Data = MuteMode;
 
	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_TXMUTEMODE
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 	TxBuf.Data = MuteMode;
//	TxBuf.Data2 = SoftMuteRssiThr;
//	TxBuf.Data3 = SoftMuteMaxAtten;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}



//	InputGain
//	{
//	INPUTGAIN_2D5,	INPUTGAIN_1D25, INPUTGAIN_0D833
//	}
//	ActiveAudioCtrl
//	{
//	ACTAUDIOCTRL_RONLY,	ACTAUDIOCTRL_LONLY,	ACTAUDIOCTRL_STEREO  
//	}
int DutBtApiClssHeader ConfigFmAudioInput(int InputGain, int ActiveAudioCtrl)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmdB2	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdB2*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_CONFIGAUDIOINPUT);
	pTxBuf->Data1 = InputGain;
	pTxBuf->Data2 = ActiveAudioCtrl;

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_CONFIGAUDIOINPUT
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD2_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 	TxBuf.Data1 = InputGain;
	TxBuf.Data2 = ActiveAudioCtrl;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}


int DutBtApiClssHeader EnableFmTxAudioAGC(int Mode_OffOn)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmdB	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdB*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_ENABLETXAUDIOAGC);
	pTxBuf->Data = Mode_OffOn;

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_ENABLETXAUDIOAGC
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 	TxBuf.Data = Mode_OffOn;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}




int DutBtApiClssHeader SetFmFreqDeviationExtended(	DWORD LprDev, 
										DWORD LmrDev,	
										DWORD PilotDev,
										DWORD RdsDev)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmdD4	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdD4*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SETTXFREQDEVIATION_EXTENDED);
	pTxBuf->Data1 = LprDev;
	pTxBuf->Data2 = LmrDev;
	pTxBuf->Data3 = PilotDev;
	pTxBuf->Data4 = RdsDev; 

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_TXFREQDEVIATION_EXTENDED
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD4_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values 
	// TxBuf... 
 	TxBuf.Data1 = LprDev;
	TxBuf.Data2 = LmrDev;
 	TxBuf.Data3 = PilotDev;
	TxBuf.Data4 = RdsDev; 

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 

	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}
 

 
int DutBtApiClssHeader SetFmTxPowerMode(int Mode_HighLow)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmdB	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdB*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SET_TX_OUT_POWER_MODE);
	pTxBuf->Data = Mode_HighLow; 

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_TXOUTPUTPOWERMODE
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 	TxBuf.Data = Mode_HighLow;
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 

	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}

/* // TBRM 
int DutBtApiClssHeader GetFmTxPowerMode(int *pMode_HighLow)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmdB	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmdB*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_GET_TX_OUT_POWER_MODE);
 
	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) return status;

	if(pMode_HighLow) (*pMode_HighLow) = pTxBuf->Data;
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_TXOUTPUTPOWERMODE
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_GET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 
	if(pMode_HighLow) (*pMode_HighLow) = TxBuf.Data;

	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}
*/



int DutBtApiClssHeader SetFmTxRdsPiCode(WORD PiCode)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmdW	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdW*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SETTXRDSPICODE);
	pTxBuf->Data = PiCode;
	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_TXRDSPICODE
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 	TxBuf.Data = PiCode;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 

	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}

   
//	RdsGroup
//	{
//	RDSGROUP_PS0A, RDSGROUP_PS0B, RDSGROUP_RT2A, RDSGROUP_RT2B
//	}
int DutBtApiClssHeader SetFmTxRdsGroup(int RdsGroup)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmdB	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdB*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SETRDSGROUP);
	pTxBuf->Data = RdsGroup;

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_RDSGROUP
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 	TxBuf.Data = RdsGroup;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 

	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}



int DutBtApiClssHeader SetFmTxRdsPty(BYTE RdsPty)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmdB	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdB*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SETRDSPTY);
	pTxBuf->Data = RdsPty;

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_RDSPTY
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 	TxBuf.Data = RdsPty;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 

	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}



int DutBtApiClssHeader SetFmTxRdsAfCode(BYTE AfCode)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmdB	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdB*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SETRDSAFCODE);
	pTxBuf->Data = AfCode;

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_RDSAFCODE
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 	TxBuf.Data = AfCode;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}
   
//	RdsTxMode
//	{
//	RDSTXMODE_PS_RT, RDSTXMODE_RAW
//	}
int DutBtApiClssHeader SetFmTxRdsMode(int RdsTxMode)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmdB	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdB*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SETTXRDSMODE);
	pTxBuf->Data = RdsTxMode;

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_TXRDSMODE
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 	TxBuf.Data = RdsTxMode;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}


int DutBtApiClssHeader SetFmTxRdsScrolling(int Mode_OffOn)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmdB	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdB*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SETRDSSCROLLING);
	pTxBuf->Data = Mode_OffOn;

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_RDSSCROLLING
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 	TxBuf.Data = Mode_OffOn;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 

	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}


int DutBtApiClssHeader SetFmTxRdsScrollingRate(BYTE Rate_CharPerScroll)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmdB	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdB*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SETRDSSCROLLRATE);
	pTxBuf->Data = Rate_CharPerScroll;

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_RDSSCROLLRATE
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 	TxBuf.Data = Rate_CharPerScroll;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 

	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}



int DutBtApiClssHeader SetFmTxRdsDisplayLength(BYTE Len_Chars) 
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmdB	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdB*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SETRDSDISPLAYLEN);
	pTxBuf->Data = Len_Chars;

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_RDSDISPLAYLEN
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
	TxBuf.Data = Len_Chars;
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}




int DutBtApiClssHeader SetFmTxRdsToggleAB(void)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmd	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmd*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SETRDSTOGGLEAB);
 
	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_RDSTOGGLEAB
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}



 
 

   
//	Repository
//	{
//	REPOSITORY_G0, REPOSITORY_G1,REPOSITORY_G2
//	}
int DutBtApiClssHeader SetFmTxRdsRepository(int Repository)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmdB	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdB*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SETRDSREPOSITORY);
	pTxBuf->Data = Repository;

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBT_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_RDSREPOSITORY
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 	TxBuf.Data = Repository;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}

 

int DutBtApiClssHeader GetFmSoftMute(int *pEnable )
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmd			*pTxBuf=NULL;
	Bt_HciFmEventB	*pRxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmd*) pBuf;
	pRxBuf = (Bt_HciFmEventB*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_GET_TX_SOFTMUTE_MODE);
 
	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) 
	{
        if(pBuf) free(pBuf);
	    return status;
    }
    
	if(pEnable) (*pEnable) =  pRxBuf->Data;
    if(pBuf) free(pBuf);
 	
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBD_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_SOFTMUTEMODE
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_GET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(status) return status;
	// get Get values
	// RxBuf... 
 	if(pEnable)			(*pEnable) =  (TxBuf).Data;
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}

int DutBtApiClssHeader SetFmSoftMute(int Enable, WORD MuteThreshold, BYTE AudioAttenuation)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmdBWB	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdBWB*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SET_TX_SOFTMUTE_MODE);
	pTxBuf->DataB1 = Enable;
	pTxBuf->DataW = MuteThreshold;
	pTxBuf->DataB2 = AudioAttenuation;

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBD_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_SOFTMUTEMODE
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD3_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf...
 	TxBuf.Data1 = Enable;
 	TxBuf.Data2 = MuteThreshold;
 	TxBuf.Data3 = AudioAttenuation;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}
 
int DutBtApiClssHeader GetPllLockStatus(int *pLocked )
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmd			*pTxBuf=NULL;
	Bt_HciFmEventB	*pRxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmd*) pBuf;
	pRxBuf = (Bt_HciFmEventB*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_GETPLLLOCLSTATUS);
 
	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) 
	{
        if(pBuf) free(pBuf);
	    return status;
    }
	if(pLocked) (*pLocked) =  pRxBuf->Data;
    if(pBuf) free(pBuf);
 	
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBD_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_PLLLOCKSTATUS
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_GET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(status) return status;
	// get Get values
	// RxBuf... 
 	if(pLocked)			(*pLocked) =  (TxBuf).Data;
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}

  
int DutBtApiClssHeader GetFmStereoBlendingMode(int *pBlendingMode )
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmd			*pTxBuf=NULL;
	Bt_HciFmEventB	*pRxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmd*) pBuf;
	pRxBuf = (Bt_HciFmEventB*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_GETSTEREOBLENDINGMODE);
 
	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) 
	{
	    if(pBuf) free(pBuf);
        return status;
    }
    
	if(pBlendingMode) (*pBlendingMode) =  pRxBuf->Data;
    if(pBuf) free(pBuf);
 	
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBD_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_STEREOBLENDINGMODE
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_GET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(status) return status;
	// get Get values
	// RxBuf... 
 	if(pBlendingMode)			(*pBlendingMode) =  (TxBuf).Data;
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}

int DutBtApiClssHeader SetFmStereoBlendingMode(int BlendingMode)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmdB	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdB*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SETSTEREOBLENDINGMODE);
	pTxBuf->Data = BlendingMode; 

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBD_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_STEREOBLENDINGMODE
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf...
 	TxBuf.Data = BlendingMode; 

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}
 
int DutBtApiClssHeader SetFmStereoBlendingConfigration(int TimeConstant)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmdB	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdB*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_STEREOBLENDINGCONFIG);
	pTxBuf->Data = TimeConstant; 

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);

	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBD_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_STEREOBLENDINGCONFIG
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf...
 	TxBuf.Data = TimeConstant; 

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}
///////////////
 


int DutBtApiClssHeader GetFmStereoStatus(int *pStatus )
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmd			*pTxBuf=NULL;
	Bt_HciFmEventB	*pRxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmd*) pBuf;
	pRxBuf = (Bt_HciFmEventB*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_GET_STEREOSTATUS);
 
	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) 
	{
	    if(pBuf) free(pBuf);
        return status;
    }
    
	if(pStatus) (*pStatus) =  pRxBuf->Data;
    if(pBuf) free(pBuf);
 	
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBD_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_STEREOSTATUS
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_GET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(status) return status;
	// get Get values
	// RxBuf... 
 	if(pStatus)			(*pStatus) =  (TxBuf).Data;
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}


int DutBtApiClssHeader GetFmI2SParameter(int ChFreqInKHz, int *pSamplingRate, int *pBClk_LrClk)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmdD			*pTxBuf=NULL;
	Bt_HciFmEventB2	*pRxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmdD*) pBuf;
	pRxBuf = (Bt_HciFmEventB2*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_GET_I2S_PARAMETER);
	pTxBuf->Data = ChFreqInKHz;

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) 
	{
	    if(pBuf) free(pBuf);
        return status;
    }
	if(pSamplingRate)	(*pSamplingRate) =  pRxBuf->Data1;
	if(pBClk_LrClk)		(*pBClk_LrClk) =  pRxBuf->Data2;
    if(pBuf) free(pBuf);
 	
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBD_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_I2S_PARAMETER
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_GET

	mfgFm_CmdFmDataD3_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 	(TxBuf).Data1 = ChFreqInKHz;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(status) return status;
	// get Get values
	// RxBuf... 
  	if(pSamplingRate)	(*pSamplingRate) =  (TxBuf).Data2;
	if(pBClk_LrClk)		(*pBClk_LrClk) =  (TxBuf).Data3;

	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}



//////////////
  
int DutBtApiClssHeader GetFmAudioVolume(int *pVolume)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmd		*pTxBuf=NULL;
	Bt_HciFmEventW	*pRxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(*pRxBuf)));
	pTxBuf = (Bt_FmBCmd*) pBuf;
	pRxBuf = (Bt_HciFmEventW*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_GETFMAUDIOVOLUME);
 
	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) 
	{
	    if(pBuf) free(pBuf);
        return status;
    }
	if(pVolume) (*pVolume) =  pRxBuf->Data;
    if(pBuf) free(pBuf);
 	
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBD_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_FMAUDIOVOLUME
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_GET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf... 
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(status) return status;
	// get Get values
	// RxBuf... 
 	if(pVolume)			(*pVolume) =  (TxBuf).Data;
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}

int DutBtApiClssHeader SetFmAudioVolume(int Volume)
{

#if defined (_HCI_PROTOCAL_)

#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_FM_CMD)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_FmBCmdW	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	memset(pBuf, 0, max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_FmBCmdW*) pBuf;
	pTxBuf->Cmd = (HCI_CMD_MRVL_FM_SUBCMD_SETFMAUDIOVOLUME);
	pTxBuf->Data = Volume; 

	status = queryHci(THIS_OID, pBuf, sizeof(*pTxBuf), 
							 THIS_EVENT, 
							 &EventBuf
							 );
    if(pBuf) free(pBuf);
  
	return status;

#else // #if defined (_HCI_PROTOCAL_)
	int status=0;
// TBD_KY
	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_FM_CMD_FMAUDIOVOLUME
	#undef THIS_RSP 
	#define THIS_RSP	 FMCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgFm_CmdFmDataD_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf...
 	TxBuf.Data = Volume; 

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	// get Get values
	// TxBuf... 
 
	return status;

#endif //#if defined (_HCI_PROTOCAL_)

}
 
#endif // #if defined (_FM_) 
