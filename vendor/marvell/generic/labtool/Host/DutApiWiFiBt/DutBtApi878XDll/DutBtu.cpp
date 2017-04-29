/** @file DutBtu.cpp
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#include "../../DutCommon/Clss_os.h"
#include "../../DutCommon/utility.h"
#include "../../DutCommon/utilities.h" 


#include "DutBtApiClss.h"
#include "../DutApi878XDll/DutDllVerNo.h"
#include "BtMfgCmd.h"
#include "DutBtu.hd"
#include "DutBrf.hd"
 

 
int DutBtApiClssHeader PokeBtRegDword(DWORD address, DWORD data)
{
	int status = 0;
 

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_MEM_REG
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_SET

	mfgBt_CmdMemReg_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

//	DebugLogRite ("PokeBtRegDword 0x%04X = 0x%04X\n", address, data);

	// set Set values
	// TxBuf...
	TxBuf.address = address;
 	TxBuf.data = data; 

	if(2 == BtIf_GetDebugLogFlag()) 
		DebugLogRite ("PokeBtRegDword 0x%04X = 0x%04X\n", address, data);

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);

	return status;
}

int DutBtApiClssHeader PeekBtRegDword(DWORD address, DWORD *data)
{
	int status = 0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_MEM_REG
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_GET

	mfgBt_CmdMemReg_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf...
	TxBuf.address = address;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);

//	DebugLogRite ("PeekBtRegDword 0x%04X = 0x%04X\n", address, TxBuf.data);


	if(data) (*data) = TxBuf.data; 

	if(2 == BtIf_GetDebugLogFlag()) 
		DebugLogRite ("PeekBtRegDword 0x%04X = 0x%04X\n", address, TxBuf.data);
	
	return status;
}

 

int DutBtApiClssHeader WriteMemDword(DWORD address, DWORD data)
{
	return PokeBtRegDword(address, data);
}
int DutBtApiClssHeader ReadMemDword(DWORD address, DWORD *data)
{
	return PeekBtRegDword(address, data);
}

int DutBtApiClssHeader WriteBtuReg(DWORD address, WORD data)
{
//	return PokeBtRegByte(BASE_BTU_BASE|(REGMASK_BTU_REGOFFSET & address), data);

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_BTU_REG
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_SET
	int status=0;
	mfgBt_CmdMemReg_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf...
	TxBuf.address = BASE_BTU_BASE | (REGMASK_BTU_REGOFFSET  & address);
   	TxBuf.data =data; 

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
 
 	return status;

}

int DutBtApiClssHeader ReadBtuReg(DWORD address, WORD *data)
{
//	return PeekBtRegWord(BASE_BTU_BASE|(REGMASK_BTU_REGOFFSET & address), data);
#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_BTU_REG
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_GET
	int status=0;
	mfgBt_CmdMemReg_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf...
	TxBuf.address = BASE_BTU_BASE | (REGMASK_BTU_REGOFFSET  & address);

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
 
	if(data) (*data) = (WORD)TxBuf.data; 

 	return status;
}


 

// TBT 
int DutBtApiClssHeader SetBtDisableBtuPwrCtl(bool DisableBtuPwrCtl)
{
	int status=0;
 

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_BTU_PWRCTL_BPS
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_SET

 	mfgBt_Enable_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values 
	TxBuf.Enable = DisableBtuPwrCtl;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
  	 
	return status;

}

// TBT 
int DutBtApiClssHeader GetBtDisableBtuPwrCtl(bool *pDisableBtuPwrCtl)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_BTU_PWRCTL_BPS
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_GET

 	mfgBt_Enable_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf...

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
 
	// pass back get values
    if(pDisableBtuPwrCtl) *pDisableBtuPwrCtl = TxBuf.Enable?true:false;
 	
	return status;


}


 
 
// not supported by this chip  
/*

int DutBtApiClssHeader BtGetRxInfo(int LinkIndex, 
								int	*pRssiQual,
				  				int	*pRssiValue,
				  				int	*pReceivedCrcCnt, 
				  				int *pHecMatchCnt,
				  				int *pCrcErrorCnt,
				  				int *pFECCorrectCnt)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_RX_INFO
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_GET

 	mfgBt_CmdRxInfo_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	TxBuf.LinkIndex = LinkIndex;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);

	
	// pass back get values
	if(pRssiQual)		*pRssiQual			= TxBuf.RssiAvg;
	if(pRssiValue)		*pRssiValue			= TxBuf.RssiLast;
	if(pReceivedCrcCnt) *pReceivedCrcCnt	= TxBuf.RxCrcOkCnt;
	if(pHecMatchCnt)	*pHecMatchCnt		= TxBuf.RxHecMatchCnt;
	if(pCrcErrorCnt)	*pCrcErrorCnt		= TxBuf.RxCrcErrorCnt;
	if(pFECCorrectCnt)	*pFECCorrectCnt		= TxBuf.RxFECCorrectCnt;
 	
	return status;


}



int DutBtApiClssHeader BtClearRxCnts(int LinkIndex)
{
	int status=0;
#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_RX_INFO
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_SET

 	mfgBt_CmdRxInfo_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	TxBuf.LinkIndex = LinkIndex;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
 	
	return status;


}

*/


#if defined(_W8780_)  

int DutBtApiClssHeader GetTrSwAntenna(int *pAntSelection)
{
	int status=0;
 
#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_ANT_TRSW
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_GET

 	mfgBt_Enable_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
 	
	if(pAntSelection) (*pAntSelection) = TxBuf.Enable;
	return status;
}

int DutBtApiClssHeader SetTrSwAntenna(int AntSelection)
{
	int status=0;
 
#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_ANT_TRSW
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_SET

 	mfgBt_Enable_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	TxBuf.Enable = AntSelection;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
 	

	return status;
}
#endif // #if defined(_W8780_)


int DutBtApiClssHeader SetPcmLoopBackMode(int enable, int useGpio12AsDin)
{
	int status=0;
 
#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_PCMLOOPBACK
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_SET

 	mfgBt_PcmLoopBack_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	TxBuf.Enable = enable;
	TxBuf.useGpio12AsDin = useGpio12AsDin;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
 	

	return status;
}

