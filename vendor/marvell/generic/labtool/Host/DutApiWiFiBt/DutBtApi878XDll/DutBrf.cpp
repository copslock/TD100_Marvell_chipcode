/** @file DutBrf.cpp
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#include "../../DutCommon/Clss_os.h"
#include "../../DutCommon/mathUtility.h"
#include "../../DutCommon/utility.h"
#include "../../DutCommon/utilities.h" 
 
#include "DutBtApiClss.h"
#include "../DutApi878XDll/DutDllVerNo.h"
#include "BtMfgCmd.h"

#include "DutBrf.hd"

WORD slotDur[4]={312, 625, 1876, 3126}; //us
int maxPayloadInByte[NUM_BT_DATARATE][NUM_BT_HOPSLOT]=
{//		1DM1	1DM3	1DM5
	{0,	17, 0,	121, 0,	224},
//		1DH1	1DH3	1DH5
	{0,	27, 0,	183, 0,	339},
//		2DH1	2DH3	2DH5
	{0,	54, 0,	367, 0,	679},
//		1DH1	1DH3	1DH5
	{0,	83, 0,	552, 0,	1021}
};

 



//////////////////////////////////////////
int DutBtApiClssHeader WriteBrfReg(DWORD address, WORD data)
{
	int status=0;
	WORD RadioType=0;
	
 

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_RF_REG
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_SET

	mfgBt_CmdMemReg_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf...
	TxBuf.address = address;
 	TxBuf.data = data; 

	if(2 == BtIf_GetDebugLogFlag()) 
		DebugLogRite ("WriteBrfReg 0x%04X = 0x%04X\n", address, data);

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	
	return status;
}


int DutBtApiClssHeader ReadBrfReg(DWORD address, WORD *data)
{
	int status=0;
 
#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_RF_REG
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

	if(data) *data = (WORD)TxBuf.data; 

	if(2 == BtIf_GetDebugLogFlag()) 
		DebugLogRite ("ReadBrfReg 0x%04X = 0x%04X\n", address, *data);

	return status;
}




int DutBtApiClssHeader SetBtChannel(int channelNum, bool BT2)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_RF_CHAN
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_SET
 
	mfgBt_CmdRfChan_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf...
	TxBuf.channel = channelNum;
   	TxBuf.Bt2 =BT2; 

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
 
	return status;
}

int DutBtApiClssHeader SetBtChannelByFreq(int ChannelFreqInMHz, bool BT2)
{
	int status=0;
 
	return SetBtChannel(ChannelFreqInMHz - CHANNEL_INIT_FREQ_MHZ,  BT2);
}

int DutBtApiClssHeader GetBtChannel(int *pChannelNum, 
									int *pChannelFreqInMHz, 
									bool BT2)
{
	int		status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_RF_CHAN
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_GET
 
	mfgBt_CmdRfChan_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf...
   	TxBuf.Bt2 =BT2; 

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);

	if(pChannelNum) (*pChannelNum) = TxBuf.channel;
	if(pChannelFreqInMHz) (*pChannelFreqInMHz) = TxBuf.ChanFreqInMHz;
	return status;

}



  
int DutBtApiClssHeader SetBt2ChHopping(bool enable)
{
	int status=0;
 
#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_HOP2CH
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_SET

	mfgBt_Enable_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	TxBuf.Enable =enable;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);

	return status;
}

 
int DutBtApiClssHeader GetBt2ChHopping(bool *pEnable)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_HOP2CH
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_GET

	mfgBt_Enable_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);

	// pass back get values
    if(pEnable) *pEnable = TxBuf.Enable?true:false;

 	return status;
}


int DutBtApiClssHeader SetBtBrfReset(void)			
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_RF_BRFRESET
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_SET

	mfgBt_Cmd_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);

	return status;

}

/*
// TBT
int DutBtApiClssHeader GetBtCloseLoop(bool *pCloseLoop)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_RF_POW_CTRL_LOOPMODE
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_GET

	mfgBt_Enable_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);

	// pass back get values
    if(pCloseLoop) *pCloseLoop = TxBuf.Enable?true:false;
	
	return status;

}
*/

//0:MRVL Class2, 1:MRVL Class1.5  
 
int DutBtApiClssHeader SetBtPwrControlClass(DWORD option)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_PW_CLASS_MODE
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_SET

	mfgBt_Enable_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	TxBuf.Enable = option;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);

	return status;
}
		

 
int DutBtApiClssHeader GetBtPwrControlClass(DWORD *pOption) 
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_PW_CLASS_MODE
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_GET

	mfgBt_Enable_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
 
	if(pOption) (*pOption) = TxBuf.Enable;

	return status;
}

// TBT 
int DutBtApiClssHeader SetBtPwrLvlValue(double PwrLvlValueDB, DWORD IsEDR)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_PWRLVL
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_SET

	//mfgBt_Enable_t TxBuf={0};
	mfgBt_BtTxPower_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	//TxBuf.Enable = (DWORD)(PwrLvlValueDB *2);
	TxBuf.BtTxPower = (DWORD)(PwrLvlValueDB *2);
	TxBuf.IsEDR = IsEDR;
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
 
	return status;
}
		

// TBT 
int DutBtApiClssHeader GetBtPwrLvlValue(double *pPwrLvlValueDB)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_PWRLVL
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_GET

	mfgBt_Enable_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
 
	if(pPwrLvlValueDB) (*pPwrLvlValueDB) = (int)TxBuf.Enable / 2;
 
	return status;
}

/*
// TBT 
int DutBtApiClssHeader SetBtPwrMaxMin(double PwrMaxDB, double PwrMinDB)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_PWRLVL_MAXMIN
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_SET

	mfgBt_CmdPwrMaxMinV_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	TxBuf.MaxValue = PwrMaxDB * 2;
	TxBuf.MinValue = PwrMinDB * 2;
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
 
	return status;
}
*/		

// TBT 
int DutBtApiClssHeader GetBtPwrMaxMin(double *pPwrMaxDB, double *pPwrMinDB)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_PWRLVL_MAXMIN
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_GET

	mfgBt_CmdPwrMaxMinV_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
 
	if(pPwrMaxDB) (*pPwrMaxDB) = (int)TxBuf.MaxValue / 2;
 	if(pPwrMinDB) (*pPwrMinDB) = (int)TxBuf.MinValue / 2;

	return status;
}

  
int DutBtApiClssHeader StepPower(double StepDB)
{
 
#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_STEP_RF_PWR
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_SET

	int status=0;
	mfgBt_CmdStepRfPwr_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	TxBuf.StepRfPwr = (int)(StepDB * 2);

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);

//	if(step != TxBuf.StepRfPwr) 
//		  error = TOPPED;
 	
	return status;
}


// 0 internal mode 1: external mode
int DutBtApiClssHeader SetBtPwrGain(DWORD Value)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_FIXVGACTRL
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_SET

	mfgBt_Enable_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
 	TxBuf.Enable = Value; 

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
 
	return status;
}
											 

int DutBtApiClssHeader GetBtPwrGain(DWORD *pValue)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_FIXVGACTRL
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_GET

	mfgBt_Enable_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
 
	if(pValue)	(*pValue)		= TxBuf.Enable;
  	
	return status;
}


int DutBtApiClssHeader BrfTxBtCont(bool enable,			int dataRate, 
			 int payloadPattern)
{
	return BrfTxBtDutyCycle0(enable, dataRate, payloadPattern, 0, 1);
}

 
// dataRate 0; 1M, 1:2M, 2: 3M
// SlotCount: 1, 3, 5
// PacketType	reserved(26bits) FEC (4bits) SlotCount (4bits) dataRate(4bits)
// payloadPattern 0: all 0, 1: all 1, 2: PN9, 3: 0xAA, 4: 0xF0
// payloadOnly: tx Payload only, no header (true, false)
// DutyWeight: On-Time percentage

int DutBtApiClssHeader BrfTxBtDutyCycle(bool enable,			
									 int DataRate, 
			 int payloadPattern,	int DutyWeight, bool payloadOnly)
{
	if((DutyWeight <2) || (DutyWeight >98))
		return ERROR_INPUT_INVALID;
	else 
	return BrfTxBtDutyCycle0( enable,DataRate, 
							payloadPattern, DutyWeight, payloadOnly);
}

int DutBtApiClssHeader BrfTxBtDutyCycle0(bool enable,			
									 int DataRate, 
			 int payloadPattern,	int DutyWeight, bool payloadOnly)
{
	int status=0;
 
#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_DUTY_CYCLE
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_SET

	mfgBt_CmdBrfDutyCycle_t TxBuf={0};

  
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
 	TxBuf.Enable = enable;
	TxBuf.DataRate = DataRate;
	TxBuf.PayloadPattern = payloadPattern;
	TxBuf.Percent = DutyWeight;
	TxBuf.payloadOnly = payloadOnly;


 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);

	return status;
}


/*
int DutBtApiClssHeader TxBtMultiCastFrames(int FrameCount,	
										   int PayloadLength, 
										   int PacketType, 
			 int payloadPattern, bool payloadOnly)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_TX_FRAME
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_SET

	mfgBt_CmdDutyCycle_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
 	TxBuf.Enable = FrameCount;
	TxBuf.PacketType = PacketType;
	TxBuf.PayloadPattern = payloadPattern;
	TxBuf.Percent = PayloadLength;
	TxBuf.payloadOnly = payloadOnly;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	return status;
}

*/


/*


int DutBtApiClssHeader TxBtCst(bool enable, int FreqOffsetinKHz)
{
	int status=0;

  #undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_EN_CST
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_SET

	mfgBt_CmdCst_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	TxBuf.Enable = enable;
	TxBuf.FreqOffsetInKHz = FreqOffsetinKHz;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);

	return status;
}
*/


int DutBtApiClssHeader TxBtCw(bool enable)
{
	int status=0;
#if 0

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_TX_CW
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_SET

	mfgBt_CmdCst_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	TxBuf.Enable = enable; 

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
 
#else
	if(enable)
	{
		//BRF
		//0x009, 0x0D
		//
		//0x14b, 0x02
		//0x158, 0x3e	; no change 
		//
		//0x14e, 0x01
		//0x14f, 0xFF	; a1   
		//0x150, 0x00	; a2
		//
		//0x14c, 0x00	; f1 0
		//0x14d, 0x00 	; f2 0
		//
		//0x151, 0x00	; Fcerr.5:0 MSB
		//0x152, 0x00	; Fcerr.7:0 LSB
		//
		//0x153, 0x00	; I
		//0x154, 0x00	; Q
		//
		//0x009, 0x0E

		status = WriteBrfReg(0x009, 0x0D);	
		if(status) return status;

		status = WriteBrfReg(0x14b, 0x02);	
		if(status) return status;
		status = WriteBrfReg(0x158, 0x3e);		// no change 
		if(status) return status;

		status = WriteBrfReg(0x14e, 0x01);	
		if(status) return status;
		status = WriteBrfReg(0x14f, 0xFF);		// a1   
		if(status) return status;
		status = WriteBrfReg(0x150, 0x00);		// a2
		if(status) return status;

		status = WriteBrfReg(0x14c, 0x00);		// f1 0
		if(status) return status;
		status = WriteBrfReg(0x14d, 0x00);	 	// f2 0
		if(status) return status;

		status = WriteBrfReg(0x151, 0x00);		// Fcerr.5:0 MSB
		if(status) return status;
		status = WriteBrfReg(0x152, 0x00);		// Fcerr.7:0 LSB
		if(status) return status;

		status = WriteBrfReg(0x153, 0x00);		// I
		if(status) return status;
		status = WriteBrfReg(0x154, 0x00);		// Q
		if(status) return status;

		status = WriteBrfReg(0x009, 0x0E);	

	}
	else
	{
		//BRF
		//0x009, 0x0D
		//
		//0x14b, 0x00
		//0x158, 0x3e

		//BRF
		status = WriteBrfReg(0x009, 0x0D);	
		if(status) return status;

		status = WriteBrfReg(0x14b, 0x00);	
		if(status) return status;
		status = WriteBrfReg(0x158, 0x3e);	
	}



#endif 
	return status;
}

 


// not supported by this chip  
/*
int DutBtApiClssHeader SetBtXPaPdetPolority(bool Polority_Neg)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_RF_XPA_POLORITY
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_SET

	mfgBt_Enable_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	TxBuf.Enable = Polority_Neg;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);

	return status;
}

 
int DutBtApiClssHeader GetBtXPaPdetPolority(bool *pPolority_Neg)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_RF_XPA_POLORITY
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_GET

	mfgBt_Enable_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);

	if(pPolority_Neg) (*pPolority_Neg) = TxBuf.Enable?true:false;
 	
	return status;
}

  
int DutBtApiClssHeader SetBtPpaPgaPriority(bool PgaFirst)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_RF_PPAPGA_PRIORITY
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_SET

	mfgBt_Enable_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	TxBuf.Enable = PgaFirst;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);

	return status;
}

 
int DutBtApiClssHeader GetBtPpaPgaPriority(bool *pPgaFirst)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_RF_PPAPGA_PRIORITY
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_GET

	mfgBt_Enable_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	
	if(pPgaFirst)	(*pPgaFirst) = TxBuf.Enable?true:false;

	return status;
}


 
int DutBtApiClssHeader SetBtPaThermScaler(int ThermScaler)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_RF_PA_THERMAL_SCALER
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_SET

	mfgBt_CmdRfPaThermalScaler_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	TxBuf.ThermalScaler = ThermScaler;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);

	return status;
}

 
int DutBtApiClssHeader GetBtPaThermScaler(int *pThermScaler)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_RF_PA_THERMAL_SCALER
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_GET

	mfgBt_CmdRfPaThermalScaler_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

  
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);

	if(pThermScaler) 
		(*pThermScaler) = TxBuf.ThermalScaler;
	
	return status;
}


 
int DutBtApiClssHeader SetBtPaBias(WORD PaBias1, WORD PaBias2,	
								   bool PerformanceMode)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_RF_PABIAS
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_SET

	host_CmdRfPaBias_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	TxBuf.PaBias1 = PaBias1;
	TxBuf.PaBias2 = PaBias2;
	TxBuf.PerformanceMode = PerformanceMode;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);

	return status;
}


 
int DutBtApiClssHeader GetBtPaBias(WORD *pPaBias1, WORD *pPaBias2,	
								   bool PerformanceMode)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_RF_PABIAS
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_GET

	host_CmdRfPaBias_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
 	TxBuf.PerformanceMode = PerformanceMode;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	
	if(pPaBias1)	(*pPaBias1) = TxBuf.PaBias1;
	if(pPaBias2)	(*pPaBias2) = TxBuf.PaBias2;

	return status;
}

// not supported by this chip  
*/
/*
// TBT 
int DutBtApiClssHeader ResetAccum(void)
{
	int status=0; 

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_RESETACCUM
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_SET

	mfgBt_Enable_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	
	return status;
}


int DutBtApiClssHeader GetAccum(int *pAccum)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_RESETACCUM
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_GET

	mfgBt_Enable_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);

	if(pAccum) (*pAccum) = TxBuf.Enable;

	return status;
}

*/
 


 
int DutBtApiClssHeader Bt_GetBtModePdetFlag(int *pFlagThHiU,
											int *pFlagThHiD,
											int *pFlagThLoU,
											int *pFlagThLoD)
{
	int status=0;
	WORD	BrfRb=0;
	BrfReg_ChipStatus *ptr=NULL;
	ptr= (BrfReg_ChipStatus*)&BrfRb;

	status = ReadBrfReg(REG_BRF_CHIP_STATUS, &BrfRb);

 	if(pFlagThHiU) *pFlagThHiU = (*ptr).ChipStatus_VtPwDetHiUFlag;
 	if(pFlagThHiD) *pFlagThHiD = (*ptr).ChipStatus_VtPwDetHiDFlag;
 	if(pFlagThLoU) *pFlagThLoU = (*ptr).ChipStatus_VtPwDetLoUFlag;
 	if(pFlagThLoD) *pFlagThLoD = (*ptr).ChipStatus_VtPwDetLoDFlag;
	
	return status;

}
 
 

 // TBT 
// 0 internal mode 1: external mode
int DutBtApiClssHeader SetBtXtal(BYTE ExtMode, BYTE XtalValue)	
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_RFXTAL_CTRL
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_SET

	mfgBt_CmdRfXTalCtrl_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));
	
//	TxBuf.enable = extMode;
	TxBuf.value = XtalValue;
	TxBuf.extMode = ExtMode;
  
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);

	return status;

}

// TBT 
int DutBtApiClssHeader GetBtXtal(BYTE *pExtMode, BYTE *pXtalValue)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_RFXTAL_CTRL
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_GET

	mfgBt_CmdRfXTalCtrl_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

  
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
 
	if(pExtMode)	(*pExtMode) = (BYTE)TxBuf.extMode;
	if(pXtalValue) (*pXtalValue) = (BYTE)TxBuf.value;

	return status;

}

#if defined (_W8688_) || defined (_W8787_) || defined(_W8782_)
// not supported by this chip  
/*
int DutBtApiClssHeader SetBcaMode(int Mode)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_BCA
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_SET

	mfgBt_Enable_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	TxBuf.Enable = Mode;
  
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
 
	return status;

}



int DutBtApiClssHeader GetBcaMode(int *pMode)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_BCA
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_GET

	mfgBt_Enable_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

   
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
 
	if(pMode) (*pMode) = TxBuf.Enable;

	return status;

}
*/
int DutBtApiClssHeader SetFE_BTOnly(bool bOn)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_FE_BTONLY
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_SET

	mfgBt_Enable_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));
	TxBuf.Enable = bOn;
   
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
 
 
	return status;

}

#endif //#if defined (_W8688_) || defined (_W8787_) || defined(_W8782_)

// not supported by this chip  
/*
int DutBtApiClssHeader SetExtendedPwrCtrlMode(int Mode)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_EXTENDEDPWRCTRLRANGEMODE
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_SET

	mfgBt_Enable_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	TxBuf.Enable = Mode;
  
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);

	return status;

}



int DutBtApiClssHeader GetExtendedPwrCtrlMode(int *pMode)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_EXTENDEDPWRCTRLRANGEMODE
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_GET

	mfgBt_Enable_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

   
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
 
	if(pMode) (*pMode) = TxBuf.Enable;

	return status;

}






int DutBtApiClssHeader SetBbGainInitValue(WORD Gain)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_BBGAININITVALUE
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_SET

	mfgBt_Enable_t TxBuf={0};

//	if(Gain >REGVALUE_BRF_BBGAIN_MAX) return ERROR_INPUT_INVALID;

//	DebugLogRite("SetBbGainInitValue %04X %d\n", Gain, Gain);
  
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	TxBuf.Enable = Gain;
  
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);

	return status;

}



int DutBtApiClssHeader GetBbGainInitValue(WORD *pGain)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_BBGAININITVALUE
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_GET

	mfgBt_Enable_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

   
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
 
	if(pGain) (*pGain) = (WORD)TxBuf.Enable;

	return status;

}

// ============================
// ============================

int DutBtApiClssHeader SetBtPwrBbGainMaxMin(bool enable,	
										 WORD MaxValue,		
										 WORD MinValue )		// 0 internal mode 1: external mode
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_BBGAINMAXMIN
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_SET

	mfgBt_CmdPpaPgaMaxMinV_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
 	TxBuf.Enable = enable;
	TxBuf.MaxValue = MaxValue;
	TxBuf.MinValue = MinValue;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
 
	return status;

}



int DutBtApiClssHeader GetBtPwrBbGainMaxMin(bool *pEnable, 
										 WORD *pMaxValue,		
										 WORD *pMinValue) 		// 0 internal mode 1: external mode
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_BBGAINMAXMIN
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_GET

	mfgBt_CmdPpaPgaMaxMinV_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);

	if(pEnable)		(*pEnable)		= TxBuf.Enable?true:false;
	if(pMaxValue)	(*pMaxValue)	= TxBuf.MaxValue;
	if(pMinValue)	(*pMinValue)	= TxBuf.MinValue;

	return status;

}



int DutBtApiClssHeader SetPpaGainStepMode(int Mode)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_PPAGAINSTEPMODE
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_SET

	mfgBt_Enable_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	TxBuf.Enable = Mode;
  
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
 
	return status;

}



int DutBtApiClssHeader GetPpaGainStepMode(int *pMode)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_PPAGAINSTEPMODE
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_GET

	mfgBt_Enable_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

   
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
 
	if(pMode) (*pMode) = TxBuf.Enable;

	return status;

}




int DutBtApiClssHeader SetAutoZeroMode(int Mode, WORD Value)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_AUTOZEROMODE
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_SET

	mfgBt_CmdCst_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	TxBuf.Enable = Mode;
 	TxBuf.FreqOffsetInKHz = Value;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
 
	return status;

}



int DutBtApiClssHeader GetAutoZeroMode(int *pMode, WORD *pValue)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_AUTOZEROMODE
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_GET

	mfgBt_CmdCst_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

   
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
 
	if(pMode) (*pMode) = TxBuf.Enable;
	if(pValue) (*pValue) = (WORD)TxBuf.FreqOffsetInKHz;

	return status;

}






 
int DutBtApiClssHeader SetTxPwrModeHighLow(int Mode)
{
	int status=0;
 
#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_TXHILOWPWRMODE
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_SET

	mfgBt_Enable_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	TxBuf.Enable = Mode;
  
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
 
	return status;

}



int DutBtApiClssHeader GetTxPwrModeHighLow(int *pMode)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_TXHILOWPWRMODE
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_GET

	mfgBt_Enable_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

   
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
 
	if(pMode) (*pMode) = TxBuf.Enable;

	return status;

}
*/

int SetBtPwrBbGainExtMode(bool extMode, WORD BbGainValue);		// 0 internal mode 1: external mode
int GetBtPwrBbGainExtMode(bool *pExtMode, WORD *pBbGainValue);

/*
// 0 internal mode 1: external mode

int DutBtApiClssHeader SetBtPwrBbGainExtMode(bool extMode, 
											 WORD BbGainValue)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_FIXEDBBGAIN
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_SET

	mfgBt_PpaPgaInitV_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
 	TxBuf.ext_enable = extMode;
	TxBuf.PpaValue = BbGainValue;
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
 
	return status;
}
											 
 
int DutBtApiClssHeader GetBtPwrBbGainExtMode(bool *pExtMode, 
											 WORD *pBbGainValue)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_FIXEDBBGAIN
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_GET

	mfgBt_PpaPgaInitV_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
 
	if(pExtMode)		(*pExtMode)		= TxBuf.ext_enable?true:false;
	if(pBbGainValue)	(*pBbGainValue)	= (WORD)TxBuf.PpaValue;
  	
	return status;
}
 


// not supported by this chip
int DutBtApiClssHeader SetTxIQ(DWORD extMode, BYTE Alpha, BYTE Beta)
{
	int status=0;
 	WORD tempW=0;
	BrfReg_IqCal0 *pIqCal=NULL;
	 
	pIqCal = (BrfReg_IqCal0*)&tempW;

	status = ReadBrfReg(REG_BRF_IQ_CAL0, &tempW);
	if (status) return status;

	(*pIqCal).iqcal0_TxPreCompByPass= extMode;
	(*pIqCal).iqcal0_iqcalAlphaInit	= Alpha;
	(*pIqCal).iqcal0_iqcalBetaInit	= Beta;

	status = WriteBrfReg(REG_BRF_IQ_CAL0, tempW);
 
 	return status;
}


int DutBtApiClssHeader GetTxIQ(DWORD *pExtMode, BYTE *pAlpha, BYTE *pBeta)
{
	int status=0;
 	WORD tempW=0;
	BrfReg_IqCal0 *pIqCal=NULL;
	 
	pIqCal = (BrfReg_IqCal0*)&tempW;

	status = ReadBrfReg(REG_BRF_IQ_CAL0, &tempW);
	if (status) return status;

	if(pExtMode)	(*pExtMode) = (*pIqCal).iqcal0_TxPreCompByPass;
	if(pAlpha)		(*pAlpha)	= (*pIqCal).iqcal0_iqcalAlphaInit;
	if(pBeta)		(*pBeta)	= (*pIqCal).iqcal0_iqcalBetaInit;

 	return status;
}
// not supported by this chip
*/

int DutBtApiClssHeader GetBtTxDetectedInChip(int *pTx)
{
 	int status=0;

	int FlagThHiU=0, FlagThHiD=0, FlagThLoU=0, FlagThLoD=0;
	status = Bt_GetBtModePdetFlag(&FlagThHiU,
								&FlagThHiD,
								&FlagThLoU,
								&FlagThLoD);

	if(pTx) (*pTx) = (0 != (FlagThHiU|FlagThHiD|FlagThLoU|FlagThLoD));

 	return status;
}
 
/*
int DutBtApiClssHeader GetBtThermalSensorReading(DWORD *pValue)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_THERMREADING
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_GET

	mfgBt_Enable_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
 
	if(pValue)		(*pValue)		= TxBuf.Enable;
   	
	return status;
}
*/


int DutBtApiClssHeader SetBtThermalCtrlMode(DWORD Enable)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_BTU_TEMPCTRL
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_SET

	mfgBt_Enable_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	if(Enable && Enable<200)
		TxBuf.Enable = 200;		// ms
	else
		TxBuf.Enable = Enable;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP); 

	return status;
}


#ifdef _ENGINEERING_
int DutBtApiClssHeader GetBtDbgThermalCtrl(	DWORD *pCtrlActive,
											DWORD *pTempAvg,
											DWORD *pTempCur,
											DWORD *pCorrDelta,
											DWORD *pTemp10Avg)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_DBG_TEMPREADING
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_GET

	Bt_Mfg_Debug_TempCtrl_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(pCtrlActive)		(*pCtrlActive)	= TxBuf.CtrlActive;
	if(pTempAvg)		(*pTempAvg)		= TxBuf.TempAvg;
	if(pTempCur)		(*pTempCur)		= TxBuf.TempCur;
	if(pCorrDelta)		(*pCorrDelta)	= TxBuf.CorrDelta;
	if(pTemp10Avg)		(*pTemp10Avg)	= TxBuf.Temp10Avg;
   	
	return status;
}
#endif // #ifdef _ENGINEERING_


int DutBtApiClssHeader GetChipClassModeCapacity(int *pMode)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_CHIPCLASSCAPACITY
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_GET

	mfgBt_Enable_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
 
	if(pMode)		(*pMode)		= TxBuf.Enable;
   	
	return status;
}

/*
int DutBtApiClssHeader GetBtThermalCompensationMode(DWORD *pEnable,
   													DWORD *pTempRef,
   													DWORD *pSlopeNumerator,
   													DWORD *pSlopeDenominator,
   													DWORD *pCalInterval,
   													DWORD *pPPACoeff,
   													DWORD *pPACoeff)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_THERMCOMP
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_GET

	mfgBt_CmdThermCompMode_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
 
	if(pEnable)				(*pEnable)			= TxBuf.Enable;
	if(pTempRef)			(*pTempRef)			= TxBuf.TempRef;
	if(pSlopeNumerator)		(*pSlopeNumerator)	= TxBuf.SlopeNumerator;
	if(pSlopeDenominator)	(*pSlopeDenominator)= TxBuf.SlopeDenominator;
	if(pCalInterval)		(*pCalInterval)		= TxBuf.CalInterval;
	if(pPPACoeff)			(*pPPACoeff)		= TxBuf.PPACoeff;
	if(pPACoeff)			(*pPACoeff)			= TxBuf.PACoeff;
   	
	return status;
}




int DutBtApiClssHeader SetBtThermalCompensationMode(DWORD Enable,
   													DWORD TempRef,
   													DWORD SlopeNumerator,
   													DWORD SlopeDenominator,
   													DWORD CalInterval,
   													DWORD PPACoeff,
   													DWORD PACoeff)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_THERMCOMP
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_SET

	mfgBt_CmdThermCompMode_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	TxBuf.Enable			= Enable;
	TxBuf.TempRef			= TempRef;
	TxBuf.SlopeNumerator	= SlopeNumerator;
	TxBuf.SlopeDenominator	= SlopeDenominator;
	TxBuf.CalInterval		= CalInterval;
	TxBuf.PPACoeff			= PPACoeff;
	TxBuf.PACoeff			= PACoeff;


 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
    	
	return status;
}

 */
 
 

 
int DutBtApiClssHeader findStepdB(int Points, int GainPpa6dBStepMode,
	char *PwrIndBm,
	BYTE *GainPpa,
	BYTE *GainPga,
	BYTE *GainBb, 
	double targetPwr, 
	int *lowEndIndex, 
	double *stepdB) 
{
	int dBrng=0, BbCodeRange=0;
 // linear interpolation with 3 stage nonlinear steps
		// how many dBs there are and how many BbGainCode there are. 
		// how many BbGain Code need to change covert to idea case dBs,
		// then set the high bound and step down

	if(Points>2)
	{
		int lowIndex=0; 
		for (lowIndex=0; lowIndex<Points-1; lowIndex++)
			if(targetPwr>=PwrIndBm[lowIndex] && targetPwr<=PwrIndBm[lowIndex])
				break;

		if (lowIndex <Points)
		{	
			if(lowEndIndex) (*lowEndIndex) =lowIndex;
			findStepdB(2, GainPpa6dBStepMode,
					&PwrIndBm[lowIndex],
					&GainPpa[lowIndex],
					&GainPga[lowIndex],
					&GainBb[lowIndex], targetPwr, NULL, stepdB) ;

		}
	}
	else if(Points>1)
	{
		if(lowEndIndex) (*lowEndIndex) =0;

		dBrng = PwrIndBm[Points-1]-PwrIndBm[0];
		BbCodeRange = GainBb[Points-1]-GainBb[0];
		//BbCodeRange += 6*BBGAIN_CODEPERDB * (GainPga[Points-1]-GainPga[0]);
		//BbCodeRange += (GainPpa6dBStepMode?6:3)*(GainPpa[Points-1]-GainPpa[0]);
        BbCodeRange += 6*BBGAIN_CODEPERDB*(GainPga[Points-1]-GainPga[0]);
        BbCodeRange += (GainPpa6dBStepMode?6:3)*BBGAIN_CODEPERDB*(GainPpa[Points-1]-GainPpa[0]);



		(*stepdB)= (1.0*BbCodeRange*(targetPwr-PwrIndBm[0])/dBrng)/BBGAIN_CODEPERDB; 
	}
	else if(Points>0)
	{
		if(lowEndIndex) (*lowEndIndex) =0;
		(*stepdB)= (targetPwr - PwrIndBm[0]);
	}
	else
	{
		return ERROR_ERROR;
	}
	return ERROR_NONE;
}

