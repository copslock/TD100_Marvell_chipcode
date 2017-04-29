/** @file DutBtApiClss.cpp
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#include "../../DutCommon/Clss_os.h"
#include "../../DutCommon/utility.h"
#include "../../DutCommon/utilities.h"
#include "../../DutCommon/mathUtility.h"


#include DUTDLL_VER_H_PATH					// "../DutApi878XDll/DutDllVerNo.h"
#include DUTCLASS_BT_H_PATH					// "DutBtApiClss.h"
#include DUTCLASS_BT_CMDH_PATH				// "BtMfgCmd.h"

#ifdef _HCI_PROTOCAL_
#include DUTHCI_BT_LL_STRUCT_H_PATH			// "HciStruct.h"
#include DUTHCI_BT_LL_UARTERR_H_PATH		// "UartErrCode.h"
#endif //#ifdef _HCI_PROTOCAL_
 

#include "DutBrf.hd"
#include DUTCLASS_WLAN_HD_PATH		//"../DutWlanApi878XDll/DutWlanApiClss.hd" 
#include "../DutIf/DutNdis.h"

#include "DutBtCalFunc.cpp"
//#include "DutBtApiCombCmds.cpp"
#include "DutEfuse.cpp"

extern  WORD slotDur[4];
 
extern  int maxPayloadInByte[NUM_BT_DATARATE][NUM_BT_HOPSLOT];  

enum
{
  PCKTST_DATARATE_BB_NULL = 0x00,
  PCKTST_DATARATE_BB_POLL = 0x01,
  PCKTST_DATARATE_BB_FHS  = 0x02,

  PCKTST_DATARATE_BB_DM1  = 0x03,
  PCKTST_DATARATE_BB_DH1  = 0x04,
  
  PCKTST_DATARATE_BB_HV1  = 0x05,

  PCKTST_DATARATE_BB_HV2  = 0x06,
  PCKTST_DATARATE_BB_EV3  = 0x07, /* This value also applies for HV3. */
  PCKTST_DATARATE_BB_HV3  = 0x07, /* This value also applies for EV3. */
  PCKTST_DATARATE_BB_DV   = 0x08,
  PCKTST_DATARATE_BB_DM3  = 0x0A,
  PCKTST_DATARATE_BB_DH3  = 0x0B,
  PCKTST_DATARATE_BB_EV4  = 0x0C,

  PCKTST_DATARATE_BB_EV5  = 0x0D,
  PCKTST_DATARATE_BB_DM5  = 0x0E,
  PCKTST_DATARATE_BB_DH5  = 0x0F ,


  PCKTST_DATARATE_BB_DM1_EDR = 0x13,
  PCKTST_DATARATE_BB_2DH1  = 0x14,
  PCKTST_DATARATE_BB_3DH1  = 0x18,
  PCKTST_DATARATE_BB_2DH3  = 0x1A,
  PCKTST_DATARATE_BB_3DH3  = 0x1B,
  PCKTST_DATARATE_BB_2DH5  = 0x1E,
  PCKTST_DATARATE_BB_3DH5  = 0x1F,
  PCKTST_DATARATE_BB_2EV3  = 0x16,
  PCKTST_DATARATE_BB_3EV3  = 0x17,
  PCKTST_DATARATE_BB_2EV5  = 0x1C,
  PCKTST_DATARATE_BB_3EV5  = 0x1D
};

enum
{
PCKTST_PAUSE,
PCKTST_PAYLOADPATTERN_ALL0, 
PCKTST_PAYLOADPATTERN_ALL1,
PCKTST_PAYLOADPATTERN_HEX_AA,
PCKTST_PAYLOADPATTERN_PRBS, 
PCKTST_LOOPBACK_ACL,
PCKTST_CLOSED_LOOP_SYNCHRONOUS_PACKETS,
PCKTST_LOOPBACK_ACL_NO_WHITENING,
PCKTST_SYNCHRONOUS_PACKETS_NO_WHITENING,
PCKTST_PAYLOADPATTERN_HEX_F0,
PCKTST_PAYLOADPATTERN_ABORT=0xFF};

int DataRateBbValue[NUM_PROTOCAL][NUM_BT_DATARATE][NUM_BT_HOPSLOT]=
{
	//PROTOCAL_DATA_ACL
	{
	//1M FEC	1DM1							1DM3							1DM5
		{0,		PCKTST_DATARATE_BB_DM1,		0,	PCKTST_DATARATE_BB_DM3,		0,	PCKTST_DATARATE_BB_DM5},
	//1M		1DH1							1DH3							1DH5
		{0,		PCKTST_DATARATE_BB_DH1,		0,	PCKTST_DATARATE_BB_DH3,		0,	PCKTST_DATARATE_BB_DH5},
	//2M		2DH1							2DH3							2DH5
		{0,		PCKTST_DATARATE_BB_2DH1,	0,	PCKTST_DATARATE_BB_2DH3,	0,	PCKTST_DATARATE_BB_2DH5},
	//3M		3DH1							3DH3							3DH5
		{0,		PCKTST_DATARATE_BB_3DH1,	0,	PCKTST_DATARATE_BB_3DH3,	0,	PCKTST_DATARATE_BB_3DH5}
	},

	//PROTOCAL_DATA_SCO
	{//		1M FEC
	{0,		0,							0,	0,							0,	0},
//	1M:		HV1							HV2							HV3						NA	NA
	{0,		PCKTST_DATARATE_BB_HV1,		PCKTST_DATARATE_BB_HV2,		PCKTST_DATARATE_BB_HV3,	0,	0},
//	NA:
	{0,		0,	0,	0,	0,	0},
//	NA:
	{0,		0,	0,	0,	0,	0}
	},

	//PROTOCAL_DATA_ESCO
	{//	1M FEC NA
	{	0,		0,		0,	0,							0,							0},
//		NA		NA		NA	EV3							EV4							EV5
	{	0,		0,		0,	PCKTST_DATARATE_BB_EV3,		PCKTST_DATARATE_BB_EV4,		PCKTST_DATARATE_BB_EV5},
//		NA		NA		NA	2EV3						NA							2EV5
	{	0,		0,		0,	PCKTST_DATARATE_BB_2EV3,	0,							PCKTST_DATARATE_BB_2EV5},
//		NA		NA		NA	3EV3						NA							3EV5
	{	0,		0,		0,	PCKTST_DATARATE_BB_3EV3,	0,							PCKTST_DATARATE_BB_3EV5},
	
	}


};

 

#ifndef _NO_CLASS_
DutBtApiClssHeader DutBtApiClss()
{
/* 	char temp[_MAX_PATH]="";
  
 	if(getcwd(temp, _MAX_PATH) != NULL)  
 	strcat(temp, "\\setup.ini");
 	else
	strcpy(temp, "setup.ini");

 		
#ifdef	_GOLDEN_REFERENCE_UNIT_
	GetPrivateProfileString("GoldenUnit","AdapterPrefix","Marvell",adapter_prefix, sizeof(adapter_prefix), temp); 
#else //#ifdef	_GOLDEN_REFERENCE_UNIT_
	GetPrivateProfileString("Driver","AdapterPrefix","Marvell",adapter_prefix, sizeof(adapter_prefix), temp); 
#endif //#ifdef	_GOLDEN_REFERENCE_UNIT_
*/
 DutBtApiClss_Init();
}

DutBtApiClssHeader DutBtApiClss_Init()
{
 	char temp[_MAX_PATH]="";
   
 	if(getcwd(temp, _MAX_PATH) != NULL)  
 		strcat(temp, "/setup.ini");
 	else
		strcpy(temp, "setup.ini");

//	strcpy(g_NoEepromFlexFileName, "BtCalData_ext.conf");

//	g_NoEeprom = GetPrivateProfileInt("DutInitSet","NO_EEPROM",
//										g_NoEeprom, temp); 
 
//	GetPrivateProfileString("DutInitSet","NoEepromFlexFileName", 
//		g_NoEepromFlexFileName, g_NoEepromFlexFileName, _MAX_PATH, temp); 

#ifdef	_GOLDEN_REFERENCE_UNIT_
	GetPrivateProfileString("GoldenUnit","AdapterPrefix","Marvell",adapter_prefix, sizeof(adapter_prefix), temp); 
#else //#ifdef	_GOLDEN_REFERENCE_UNIT_
	GetPrivateProfileString("Driver","AdapterPrefix","Marvell",adapter_prefix, sizeof(adapter_prefix), temp); 
#endif //#ifdef	_GOLDEN_REFERENCE_UNIT_

//	PurgeAnnexPointers(0);
}

DutBtApiClssHeader ~DutBtApiClss()
{
}
#endif	//#ifndef _NO_CLASS_
 
int DutBtApiClssHeader Bt_Version(void)
{ 
 	return	MFG_VERSION(DUT_DLL_VERSION_MAJOR1, DUT_DLL_VERSION_MAJOR2,
						DUT_DLL_VERSION_MINOR1, DUT_DLL_VERSION_MINOR2); 
}

char* DutBtApiClssHeader Bt_About(void)
{
	char tempDate[50]="", tempTime[50]="";
	char AboutFormat[512]=
			"Name:\t\tDutApiClass\n"
			"Interface:\t"
#if defined (_BT_UART_)
			"HCI_UART\n" 
#else //#if defined (_BT_UART_)
			"Unknown\n" 
#endif //#if defined (_BT_UART_)

			
			"Version:\t%d.%d.%d.%02d\n" 
 			"Date:\t\t%s (%s)\n\n" 			
 			"Note:\t\t\n" 

#ifdef STATIC_OBJ
			"Static Object\n" 
#endif			
			;
 
	static char AboutString[512]="";
 
	sprintf(AboutString, AboutFormat, 
			DUT_DLL_VERSION_MAJOR1,	
			DUT_DLL_VERSION_MAJOR2,
			DUT_DLL_VERSION_MINOR1, 
			DUT_DLL_VERSION_MINOR2,
			__DATE__, __TIME__);

	return AboutString; 
}

 
 



int DutBtApiClssHeader Bt_OpenDevice( )
{
	int status=0;

#ifdef	_GOLDEN_REFERENCE_UNIT_
	status = DutBtIfClssHeader BtIf_OpenDevice(1);
#else //#ifdef	_GOLDEN_REFERENCE_UNIT_
	status = DutBtIfClssHeader BtIf_OpenDevice(0); 
#endif //#ifdef	_GOLDEN_REFERENCE_UNIT_
	
//	if(!status) status = CalCh();

	return status;
}
int DutBtApiClssHeader Bt_CloseDevice( )
{
	return DutBtIfClssHeader BtIf_CloseDevice();
}

#ifdef _HCI_PROTOCAL_
int DutBtApiClssHeader GetBDAddress(BYTE *BdAddress)
{
	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_INFORMATIONAL<<16)|HCI_CMD_INFO_READ_BD_ADDRESS)

	int status = 0;
	Bt_BdAddress TxBuf={0};
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));
	if(status) return status;

	memcpy(BdAddress,  TxBuf.BdAddress, sizeof(TxBuf.BdAddress));

	EndianSwapeByteArray(BdAddress, sizeof(TxBuf.BdAddress));

	return status;
}


int DutBtApiClssHeader GetBufferSizeInfo(WORD *pMaxAclPktLen, WORD *pTotalNumAclPkt,
										 BYTE *pMaxScoPktLen, WORD *pTotalNumScoPkt
										 )
{
	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_INFORMATIONAL<<16)|HCI_CMD_INFO_READ_BUFFER_SIZE)

	int status = 0;
	Bt_BufferSizeInfo TxBuf={0};
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));
	if(status) return status;

	if(pMaxAclPktLen)	(*pMaxAclPktLen) = TxBuf.MaxAclPktLength;
	if(pTotalNumAclPkt) (*pTotalNumAclPkt) = TxBuf.TotalNumAclPkt;

	if(pMaxScoPktLen)	(*pMaxScoPktLen) = TxBuf.MaxScoPktLength;
	if(pTotalNumScoPkt)	(*pTotalNumScoPkt) = TxBuf.TotalNumScoPkt;
 
	return status;
}


int DutBtApiClssHeader ResetBt(void)
{
#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_HC_BASEBAND<<16)|HCI_CMD_HC_RESET)
	int status = 0;
 
 	status = BtIf_query(THIS_OID, 0,0);
	return status;
}
 

int DutBtApiClssHeader EnableDeviceUnderTestMode(void)
{
#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_TESTING<<16)|HCI_CMD_TEST_ENABLE_DEVICE_UNDER_TEST_MODE)
	int status = 0;
 
 	status = BtIf_query(THIS_OID, 0,0);
	return status;
}


int DutBtApiClssHeader SetFlowControlEnable(bool enable)
{
#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_HC_BASEBAND<<16)|HCI_CMD_HC_SET_HC_TO_HOST_FLOW_CTRL)
	int status = 0;
	unsigned char Buf[1]={0};
	
	Buf[0]=enable?1:0;

 	status = BtIf_query(THIS_OID, Buf,1);
	return status;
}

#ifdef _LINUX_ 
BOOL queryHci(ULONG OID, PUCHAR Buf, UINT BufSize,
		UINT ExpectedEvent,
		UINT *BufRetSize);

#endif
 
int DutBtApiClssHeader Inquiry(BYTE ExpectedBdAddress[SIZE_BDADDRESS_INBYTE], 
								int* NumResp, int *expectedFound, 
								BYTE *pPageScanRepMode,
								WORD *pClockOffset) 
//(ULONG LAP, PINQUIRY_RESP pResp, int& NumResp);
{
#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_LINK_CONTROL<<16)|HCI_CMD_LC_INQUIRY)
#undef THIS_EVENT 
#define THIS_EVENT	 ((HCI_EVENT_INQUIRY_RESULT<<8)|(HCI_EVENT_INQUIRY_COMPLETE))

#undef BLANK_ADDRESS 
#define  BLANK_ADDRESS 0x00
	int status = 0, i=0, IqResIndex=0;
	COMMAND_INQUIRY	*pInquiryCommand=NULL;
	EVENT_INQUIRY_RESULT *pInquiryResponse=NULL;
	BYTE *pBuf=NULL;
	int expectResp=MAX_INQUIRY_RESPONSES;
	UINT EventBuf=sizeof(EVENT_INQUIRY_RESULT);
	BYTE BlankBdAddr[SIZE_BDADDRESS_INBYTE]={	BLANK_ADDRESS, BLANK_ADDRESS, BLANK_ADDRESS, 
												BLANK_ADDRESS, BLANK_ADDRESS, BLANK_ADDRESS};

	if(expectedFound) 
				(*expectedFound)=0;
	if(NumResp) *NumResp =0;

	pBuf=(BYTE*)malloc(max(sizeof(COMMAND_INQUIRY),
					sizeof(EVENT_INQUIRY_RESULT)));

	pInquiryCommand = (COMMAND_INQUIRY*)pBuf;
	pInquiryResponse = (EVENT_INQUIRY_RESULT*)pBuf;
	
	pInquiryCommand->InquiryLen =12;
	pInquiryCommand->NumResp =expectResp;
	pInquiryCommand->LAP[0]=0x33; // MARVELL?
	pInquiryCommand->LAP[1]=0x8B;
	pInquiryCommand->LAP[2]=0x9E;

	status = queryHci(THIS_OID, pBuf, sizeof(COMMAND_INQUIRY), 
							 THIS_EVENT, 
							 &EventBuf
							 );
	if(status) 
	{
	    if(pBuf) free(pBuf);
	    return status;
    }
//	if(NumResp) *NumResp += pInquiryResponse->NumResponses;

for(IqResIndex=0; IqResIndex<expectResp; IqResIndex++)
{
	if(NumResp) *NumResp += pInquiryResponse->NumResponses;

	if (!memcmp(ExpectedBdAddress, BlankBdAddr, SIZE_BDADDRESS_INBYTE)) 
	{
		for (i=0; i<pInquiryResponse->NumResponses; i++)
		{

			EndianSwapeByteArray(pInquiryResponse->RespDev[i].BdAddress, SIZE_BDADDRESS_INBYTE);

			if(expectedFound) 
				(*expectedFound)=1;
			if(ExpectedBdAddress) 
				memcpy(ExpectedBdAddress, 
					pInquiryResponse->RespDev[i].BdAddress, 
					SIZE_BDADDRESS_INBYTE);
			if(pPageScanRepMode) 
				(*pPageScanRepMode) = pInquiryResponse->RespDev[i].PageScanRepMode;	
			if(pClockOffset)
					(*pClockOffset) =pInquiryResponse->RespDev[i].ClockOffset;
			
		if(BtIf_GetDebugLogFlag())
			DebugLogRite("BD_ADDRESS: %02X-%02X-%02X-%02X-%02X-%02X\n", 
 						ExpectedBdAddress[0], ExpectedBdAddress[1], ExpectedBdAddress[2], 
						ExpectedBdAddress[3], ExpectedBdAddress[4], ExpectedBdAddress[5]);

			break;
		}
	}
	else
	{
		for (i=0; i<pInquiryResponse->NumResponses; i++)
		{
			EndianSwapeByteArray(pInquiryResponse->RespDev[i].BdAddress, SIZE_BDADDRESS_INBYTE);

			if(BtIf_GetDebugLogFlag())
				DebugLogRite("found: %02X-%02X-%02X-%02X-%02X-%02X\t", 
 							pInquiryResponse->RespDev[i].BdAddress[0], 
							pInquiryResponse->RespDev[i].BdAddress[1], 
							pInquiryResponse->RespDev[i].BdAddress[2], 
							pInquiryResponse->RespDev[i].BdAddress[3], 
							pInquiryResponse->RespDev[i].BdAddress[4], 
							pInquiryResponse->RespDev[i].BdAddress[5]);

			if(!memcmp(ExpectedBdAddress, 
						pInquiryResponse->RespDev[i].BdAddress, 
						SIZE_BDADDRESS_INBYTE))
			{ 
				if(expectedFound) 
					(*expectedFound)=1;
				if(pPageScanRepMode) 
					(*pPageScanRepMode) = pInquiryResponse->RespDev[i].PageScanRepMode;
				if(pClockOffset)
					(*pClockOffset) =pInquiryResponse->RespDev[i].ClockOffset;
				break;
			}
				
		}

		if(BtIf_GetDebugLogFlag())
			DebugLogRite("expectedFound %d\t expect: %02X-%02X-%02X-%02X-%02X-%02X\n", 
 						*expectedFound,
						ExpectedBdAddress[0], ExpectedBdAddress[1], ExpectedBdAddress[2], 
						ExpectedBdAddress[3], ExpectedBdAddress[4], ExpectedBdAddress[5]);

	} 

	if(*expectedFound || 0 == pInquiryResponse->NumResponses) 
		break;

    if(pBuf) free(pBuf);

	if(IqResIndex < (expectResp-1) && pInquiryResponse->NumResponses) 
	{
//		unsigned int BufSize =sizeof(EVENT_INQUIRY_RESULT);
		memset(pBuf, 0, EventBuf);
		status = queryHci(0, pBuf,0, THIS_EVENT, &EventBuf);
 		if(ERROR_GET_EVENT_TO == status) break;
	    if(pBuf) free(pBuf);
 		if(status) return status;
	}
}

	return status;
}

int DutBtApiClssHeader SetPageTimeOut(USHORT TimeOut)
{
#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_HC_BASEBAND<<16)|HCI_CMD_HC_WRITE_PAGE_TO)
int status = 0;
 
 	status = BtIf_query(THIS_OID, (PUCHAR)&TimeOut, sizeof(TimeOut));

	return status;
}

int DutBtApiClssHeader SetScanEnable(BYTE enableFlags)
{
#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_HC_BASEBAND<<16)|HCI_CMD_HC_WRITE_SCAN_ENABLE)
int status = 0;
 
 	status = BtIf_query(THIS_OID, (PUCHAR)&enableFlags, sizeof(enableFlags));

	return status;
}


int DutBtApiClssHeader SetEventFilter(PUCHAR pFilter, UCHAR Length)
{
#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_HC_BASEBAND<<16)|HCI_CMD_HC_SET_EVENT_FILTER)
	int status = 0;
 
 	status = BtIf_query(THIS_OID, pFilter,Length);
	return status;
}

int DutBtApiClssHeader SetEventMask(PUCHAR pMask, UCHAR Length)
{
#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_HC_BASEBAND<<16)|HCI_CMD_HC_SET_EVENT_MASK)
	int status = 0;
 
 	status = BtIf_query(THIS_OID, pMask,Length);
	return status;
}

int DutBtApiClssHeader SetAuthenticationEnable(BYTE enableFlag)
{
#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_HC_BASEBAND<<16)|HCI_CMD_HC_WRITE_AUTH_ENABLE)
	int status = 0;
 
 	status = BtIf_query(THIS_OID, (PUCHAR)&enableFlag, sizeof(enableFlag));
	return status;
}

int DutBtApiClssHeader SetVoiceSetting(WORD setting)
{
#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_HC_BASEBAND<<16)|HCI_CMD_HC_WRITE_VOICE_SETTING)
	int status = 0;
 
 	status = BtIf_query(THIS_OID, (PUCHAR)&setting, sizeof(setting));
	return status;
}


int DutBtApiClssHeader PeriodicInquery(int MaxPeriod, int MinPeriod, int InquryLength)
{
	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_LINK_CONTROL<<16)|HCI_CMD_LC_PERIODIC_INQ_MODE)
	#undef THIS_EVENT 
	#define THIS_EVENT	 ((HCI_EVENT_VENDOR_SPECIFIC<<8)|(HCI_EVENT_CMD_COMPLETE))
	int status = 0;
 
	BYTE	*pBuf=NULL;
	BYTE  TxBuf[9]={0};

	EVENT_COMMAND_COMPLETE *pRx=NULL;
	UINT RetBufSize=sizeof(EVENT_COMMAND_COMPLETE);
  
 	pRx=(EVENT_COMMAND_COMPLETE*)malloc(sizeof(EVENT_COMMAND_COMPLETE));

	TxBuf[0] = MaxPeriod&0xFF;
	TxBuf[1] = (MaxPeriod>>8)&0xFF;
	TxBuf[2] = MinPeriod&0xFF;
	TxBuf[3] = (MinPeriod>>8)&0xFF;
	TxBuf[4] = 0x33;
	TxBuf[5] = 0x8B;
	TxBuf[6] = 0x9E;
	TxBuf[7] = InquryLength&0xFF;
	TxBuf[8] = 0;

		
	status = queryHci( THIS_OID,  
			TxBuf, 
			sizeof(TxBuf),
			THIS_EVENT,
			&RetBufSize);
			
    if(pRx) free(pRx);

	return status;
}

int DutBtApiClssHeader ExitPeriodicInquery(void)
{
	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_LINK_CONTROL<<16)|HCI_CMD_LC_EXIT_PERIODIC_INQ_MODE)
	int status = 0;
 
 	status = BtIf_query(THIS_OID, 0,0);
	return status;
}

/*
int DutBtApiClssHeader CreateAclConnect(
										BYTE PartnerBdAddress[SIZE_BDADDRESS_INBYTE], 
										BYTE PageScanRepMode, 
										WORD ClockOffset,
										WORD* pHConnection)
{
#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_LINK_CONTROL<<16)|HCI_CMD_LC_CREATE_CONNECTION)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CONN_COMPLETE)

	int status = 0;
	COMMAND_CREATE_CONNECTION *pCreateConnCmd=NULL;
	BYTE* pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_CONNECTION_COMPLETE);
	EVENT_CONNECTION_COMPLETE	*pConnResult=NULL;
	int BufSize=0;

	BufSize = max(sizeof(COMMAND_CREATE_CONNECTION),
					sizeof(EVENT_CONNECTION_COMPLETE));
	pBuf=(BYTE*)malloc(BufSize);

	memset(pBuf, 0, BufSize);
	
	pCreateConnCmd = (COMMAND_CREATE_CONNECTION*)pBuf;
	pConnResult = (EVENT_CONNECTION_COMPLETE*)pBuf;

	memcpy(pCreateConnCmd->BdAddress, PartnerBdAddress,SIZE_BDADDRESS_INBYTE);
	EndianSwapeByteArray(pCreateConnCmd->BdAddress, SIZE_BDADDRESS_INBYTE);

	pCreateConnCmd->PktType = getPacketTypeSetting(PACKETTYPE_ACL_DM1); // acl
//	pCreateConnCmd->PktType = 0x330E;// getPacketTypeSetting(0x01); // acl
	pCreateConnCmd->PageScanRepMode = PageScanRepMode;
	pCreateConnCmd->ClockOffset =ClockOffset;
	pCreateConnCmd->AllowRoleSwitch =0; // no role switch;

 	status = queryHci(THIS_OID, pBuf, sizeof(COMMAND_CREATE_CONNECTION),
							THIS_EVENT, 
							&EventBuf);
	if(status) goto ERROR_HANDLE;

	status = pConnResult->Status;

	if(pHConnection) *pHConnection = pConnResult->BtConn.ConnHandle;

	EndianSwapeByteArray(pConnResult->BtConn.BdAddress, SIZE_BDADDRESS_INBYTE);
 	if(memcmp(pConnResult->BtConn.BdAddress, PartnerBdAddress, SIZE_BDADDRESS_INBYTE)) 
	{
		// disconnect
		DisConnect(pConnResult->BtConn.ConnHandle);
 		status= ERROR_MISMATCH;
		goto ERROR_HANDLE; 
	}
	
ERROR_HANDLE:
	free(pBuf);

	return status;
}
int DutBtApiClssHeader DisConnect(WORD hConnection, BYTE Reason)
{
#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_LINK_CONTROL<<16)|HCI_CMD_LC_DISCONNECT)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_DISCONNECTION_COMPLETE)

	int status = 0;
	BYTE	*pBuf=NULL;
	COMMAND_DISCONNECT *pDisConnCmd=NULL;
	EVENT_DISCONNECT_COMPLETE *pDisConnResult=NULL;
	UINT EventBuf=sizeof(EVENT_DISCONNECT_COMPLETE);

	pBuf=(BYTE*)malloc(max(sizeof(COMMAND_DISCONNECT),
					sizeof(EVENT_DISCONNECT_COMPLETE)));
	
	pDisConnCmd = (COMMAND_DISCONNECT*)pBuf;
	pDisConnResult = (EVENT_DISCONNECT_COMPLETE*)pBuf;


	pDisConnCmd->ConnHandle = hConnection;
	pDisConnCmd->Reason=Reason;

 	status = queryHci(THIS_OID, pBuf,sizeof(COMMAND_DISCONNECT), 
					THIS_EVENT, &EventBuf);

	if(status) goto ERROR_HANDLE;

	status = pDisConnResult->Status;
	
ERROR_HANDLE:
	free(pBuf);

	return status;
}

int DutBtApiClssHeader SetConnPacketType(WORD hConnection, WORD PacketType)
{
	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_LINK_CONTROL<<16)|HCI_CMD_LC_CHANGE_CONNECTION_PACKET_TYPE)
	#undef THIS_EVENT 
	#define THIS_EVENT	 (HCI_EVENT_CONN_PACKET_TYPE_CHANGED)

	int status=0;
	BYTE	*pBuf=NULL;
	COMMAND_CHANGE_PKT_TYPE *pTx=NULL;

	EVENT_PKT_TYPE_CHANGED *pRx=NULL;
 	UINT RetBufSize=sizeof(EVENT_PKT_TYPE_CHANGED);
  
	pBuf = (BYTE*) malloc(max(sizeof(COMMAND_CHANGE_PKT_TYPE), 
					sizeof(EVENT_PKT_TYPE_CHANGED)));

	pTx = (COMMAND_CHANGE_PKT_TYPE*) pBuf;
	pRx = (EVENT_PKT_TYPE_CHANGED*) pBuf;
	
	pTx->ConnHandle = hConnection;
	pTx->PacketType = PacketType;


	status = queryHci( THIS_OID,  
			pBuf, 
			sizeof(COMMAND_CHANGE_PKT_TYPE),
			THIS_EVENT,
			&RetBufSize);
			
 	return status;
}
*/

int DutBtApiClssHeader SetDeviceDiscoverable(void)
{
	int status = 0;
	BYTE Buf[1024]={0};
	WORD tempW=0;
 
	// clear all filters
	Buf[0]=0;
	status = SetEventFilter(Buf, 1);
	if(status) return status;

	// set connection filter
	Buf[0]=0x02; // connection setup filter 
	Buf[1]=0x00; // allow connection from all devices
	Buf[2]=0x02; // Auto accept connection request
	status = SetEventFilter(Buf, 3);
	if(status) return status;

	// set scan enable
	Buf[0]=0x03; // Inquiry scan and page scan enable
	status = SetScanEnable(Buf[0]);//
	if(status) return status;


	// set Page timeout
	tempW = 0x2000; // page time out in # of baseband slots
 	status = SetPageTimeOut(tempW);
	if(status) return status;

	// write authentication enable
	status = SetAuthenticationEnable(0);
	if(status) return status;

	// write voice setting
	status = SetVoiceSetting(0x0060);
	if(status) return status;

	// set event mask
	Buf[0]=0xFF;
	Buf[1]=0x9F;
	Buf[2]=0xFB;
	Buf[3]=0xBF;
	Buf[4]=0x07;
	Buf[5]=0xF8;
	Buf[6]=0xBF;
	Buf[7]=0x0D;
	status = SetEventMask(Buf, 8);
	return status;


}



/*


int DutBtApiClssHeader WriteLinkPolice(WORD hConnection, 
									 WORD RoleSwitchMode,
									 WORD HoldMode,
									 WORD SniffMode,
									 WORD ParkMode
									 )
{
	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_LINK_POLICY<<16)|HCI_CMD_LP_WRITE_LP_SETTINGS)
	#undef THIS_EVENT 
	#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status=0;
	BYTE	*pBuf=NULL;
	COMMAND_WRITE_LP Buf={0};

//	EVENT_PKT_TYPE_CHANGED *pRx=NULL;
// 	UINT RetBufSize=sizeof(EVENT_PKT_TYPE_CHANGED);
  
	pBuf = (BYTE*) &Buf;

 	Buf.ConnHandle		= hConnection;
	Buf.RoleSwitchMode	= RoleSwitchMode;
	Buf.HoldMode		= HoldMode;
	Buf.SniffMode		= SniffMode;
	Buf.ParkMode		= ParkMode;


	status = BtIf_query( THIS_OID,  
			pBuf, 
			sizeof(COMMAND_WRITE_LP));
			
 	return status;
}


int DutBtApiClssHeader ReadLinkPolice(WORD hConnection, 
									  WORD *pRoleSwitchMode,
									 WORD *pHoldMode,
									 WORD *pSniffMode,
									 WORD *pParkMode)
{
	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_LINK_POLICY<<16)|HCI_CMD_LP_READ_LP_SETTINGS)
	#undef THIS_EVENT 
	#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status=0;
	BYTE	*pBuf=NULL;
	COMMAND_READ_LP *pTx=NULL;

	EVENT_READ_LP *pRx=NULL;
 	UINT RetBufSize=sizeof(EVENT_READ_LP);
  
	pBuf = (BYTE*) malloc(max(sizeof(COMMAND_READ_LP), 
					sizeof(EVENT_READ_LP)));

	pTx = (COMMAND_READ_LP*) pBuf;
	pRx = (EVENT_READ_LP*) pBuf;
	
	pTx->ConnHandle = hConnection;
 

	status = queryHci( THIS_OID,  
			pBuf, 
			sizeof(COMMAND_READ_LP),
			THIS_EVENT,
			&RetBufSize);
			
	if(pRoleSwitchMode) *pRoleSwitchMode = pRx->RoleSwitchMode;
	if(pHoldMode)		*pHoldMode= pRx->HoldMode;
	if(pSniffMode)		*pSniffMode = pRx->SniffMode;
	if(pParkMode)		*pParkMode = pRx->ParkMode;
 	return status;
}



int DutBtApiClssHeader Hold(WORD hConnection, WORD HoldMaxInterval, WORD HoldMinInterval, bool wait4Exit)
{
#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_LINK_POLICY<<16)|HCI_CMD_LP_HOLD_MODE)
#undef THIS_EVENT 
//#define THIS_EVENT	 (HCI_EVENT_CMD_STATUS)
//	#define THIS_EVENT	 ((HCI_EVENT_CMD_STATUS<<8)|(HCI_EVENT_MODE_CHANGE))
#define THIS_EVENT	 (HCI_EVENT_MODE_CHANGE)
	int status = 0;
	WORD Buf[3]={0};
	EVENT_MODE_CHANGE *pRsp=NULL;
 	UINT BufSize=sizeof(EVENT_MODE_CHANGE);

	pRsp = (EVENT_MODE_CHANGE*)Buf;

	Buf[0] = hConnection;
	Buf[1] = HoldMaxInterval;
	Buf[2] = HoldMinInterval;

// BOOL queryHci(ULONG OID, PUCHAR Buf, UINT BufSize, 
//							 UINT ExpectedEvent=0, 
//							 UINT *BufRetSize=NULL
//							 );
	status = queryHci(THIS_OID, (unsigned char*)Buf,6, THIS_EVENT, &BufSize);
	if(status) return status;

	status = !(MODE_CURRENTMODE_HOLD== pRsp->CurrentMode);
 	
	if (BtIf_GetDebugLogFlag())
			DebugLogRite("Hold interval 0x%04X (%f ms)\n", 
				pRsp->Interval, (pRsp->Interval*0.625));
  

	if(wait4Exit)
	{
		double waitInterval=0;
		waitInterval = (pRsp->Interval*0.625)-100;
		Sleep((unsigned int)waitInterval); //ms

#undef THIS_OID 
#define THIS_OID	 (0)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_MODE_CHANGE)

	BufSize =sizeof(EVENT_MODE_CHANGE);
	status = queryHci(THIS_OID, (unsigned char*)Buf,0, THIS_EVENT, &BufSize);
 	if(status) return status;

	status = !(MODE_CURRENTMODE_ACTIVE== pRsp->CurrentMode);
	if (BtIf_GetDebugLogFlag())
			DebugLogRite("Current Mode %d\n", 
				pRsp->CurrentMode);

	}
	return status;
}
int DutBtApiClssHeader Park(WORD hConnection, 
							WORD BeaconMaxInterval, WORD BeaconMinInterval)
{
#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_LINK_POLICY<<16)|HCI_CMD_LP_PARK_MODE)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_MODE_CHANGE)
	int status = 0;
 	WORD Buf[3]={0};
	EVENT_MODE_CHANGE *pRsp=NULL;
 	UINT BufSize=sizeof(EVENT_MODE_CHANGE);

	pRsp = (EVENT_MODE_CHANGE*)Buf;

	Buf[0] = hConnection;
	Buf[1] = BeaconMaxInterval;
	Buf[2] = BeaconMinInterval;

	status = queryHci(THIS_OID, (unsigned char*)Buf,6, THIS_EVENT, &BufSize);
	if(status) return status;

	status = !( MODE_CURRENTMODE_PARK == pRsp->CurrentMode);
/ *
#undef THIS_OID 
#define THIS_OID	 (0)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_MODE_CHANGE)
	BufSize =sizeof(EVENT_MODE_CHANGE);
	status = queryHci(0, (unsigned char*)Buf,0, THIS_EVENT, &BufSize);
* /
	return status;
}

int DutBtApiClssHeader ExitPark(WORD hConnection)
{
#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_LINK_POLICY<<16)|HCI_CMD_LP_EXIT_PARK_MODE)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_MODE_CHANGE)
	int status = 0;
 	WORD Buf[3]={0};
	EVENT_MODE_CHANGE *pRsp=NULL;
 	UINT BufSize=sizeof(EVENT_MODE_CHANGE);

	pRsp = (EVENT_MODE_CHANGE*)Buf;

	Buf[0] = hConnection; 
 
	status = queryHci(THIS_OID, (unsigned char*)Buf,2, THIS_EVENT, &BufSize);
	if(status) return status;

	status = !( MODE_CURRENTMODE_ACTIVE == pRsp->CurrentMode);
/ *
#undef THIS_OID 
#define THIS_OID	 (0)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_MODE_CHANGE)
	BufSize =sizeof(EVENT_MODE_CHANGE);
	status = queryHci(0, (unsigned char*)Buf,0, THIS_EVENT, &BufSize);
* /
	return status;
}
int DutBtApiClssHeader Sniff(WORD hConnection, 
							 WORD SniffMaxInterval,	WORD SniffMinInterval,
							 WORD SniffAttempt,		WORD SniffTimeout)
{
#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_LINK_POLICY<<16)|HCI_CMD_LP_SNIFF_MODE)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_MODE_CHANGE)
	int status = 0;
	WORD Buf[5]={0};
	EVENT_MODE_CHANGE *pRsp=NULL;
 	UINT BufSize=sizeof(EVENT_MODE_CHANGE);

	pRsp = (EVENT_MODE_CHANGE*)Buf;

	Buf[0] = hConnection;
	Buf[1] = SniffMaxInterval;
	Buf[2] = SniffMinInterval;
	Buf[3] = SniffAttempt;
	Buf[4] = SniffTimeout;

	status = queryHci(THIS_OID, (unsigned char*)Buf,10, THIS_EVENT, &BufSize);
	if(status) return status;

	status = !( MODE_CURRENTMODE_SNIFF == pRsp->CurrentMode);
/ *
#undef THIS_OID 
#define THIS_OID	 (0)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_MODE_CHANGE)
	BufSize =sizeof(EVENT_MODE_CHANGE);
	status = queryHci(0, (unsigned char*)Buf,0, THIS_EVENT, &BufSize);
* /
	return status;
}

int DutBtApiClssHeader ExitSniff(WORD hConnection)
{
#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_LINK_POLICY<<16)|HCI_CMD_LP_EXIT_SNIFF_MODE)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_MODE_CHANGE)
	int status = 0;
	WORD Buf[3]={0};

	EVENT_MODE_CHANGE *pRsp=NULL;
 	UINT BufSize=sizeof(EVENT_MODE_CHANGE);

	pRsp = (EVENT_MODE_CHANGE*)Buf;
	Buf[0] = hConnection; 
 
	status = queryHci(THIS_OID, (unsigned char*)Buf,2, THIS_EVENT, &BufSize);
	if(status) return status;

	status = !( MODE_CURRENTMODE_ACTIVE == pRsp->CurrentMode);

/ *
#undef THIS_OID 
#define THIS_OID	 (0)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_MODE_CHANGE)
	BufSize =sizeof(EVENT_MODE_CHANGE);
	status = queryHci(0, (unsigned char*)Buf,0, THIS_EVENT, &BufSize);
* /
	return status;
}

  */
#endif //#ifdef _HCI_PROTOCAL_



/////////////////////////////////////////////////////
#ifdef _HCI_PROTOCAL_

int DutBtApiClssHeader TxBtDutyCycleHop(bool enable,			
										int PacketType, 
										int payloadPattern,	
										int PayloadLenghtInByte,
										bool HopMode, 
										int interval,
										int Whitening)
{
	return TxBtDutyCycleHop2( enable,			
								 PacketType, 
								 payloadPattern,	
								 PayloadLenghtInByte,
								 HopMode, 
								 interval,
								 Whitening); // legacy behavior
}
#endif // #ifdef _HCI_PROTOCAL_

int DutBtApiClssHeader MfgMrvlTxTest(bool enable,	int PacketType, 
				  int PayLoadPattern, 
				  int PayloadLength,int RandomHopping,
				  int Interval, int Whitening
				  )
{ 
		return MfgMrvlTxTest2(	enable,	PacketType, 
					PayLoadPattern, 
					PayloadLength,	RandomHopping,	Interval,	Whitening); // legacy behavior
}



#ifdef _HCI_PROTOCAL_
int DutBtApiClssHeader TxBtDutyCycleHop2(bool enable,			
										int PacketType, 
										int payloadPattern,	
										int PayloadLenghtInByte,
										bool HopMode, 
										int interval,
										int Whitening, 
										int TxCnt, char TxPwrLvl)
{
	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_TXTEST)
	
	
	
	int status = 0;
	Bt_Pfw_TxTest TxBuf={0};
	int BtChannel=0x20;
	static int currentBtCh=BT_CHANNEL_INVALID;
	Bt_PacketTypeCmd *pPacketType=NULL;
 	int dataRate=4, slotTimer=1, payloadlength=0x1B, testinterval=1;
	int Protocal=0;

	if (enable && BT_CHANNEL_INVALID == currentBtCh)
		status = GetBtChannel(&currentBtCh);
	if(status) currentBtCh = BT_CHANNEL_INVALID;

	if(currentBtCh>=0 && currentBtCh<MAX_NUM_BT_CH)
		BtChannel = currentBtCh;
// based on PacketType, to get dataRate, and payloadlength
	pPacketType = (Bt_PacketTypeCmd *)&PacketType;

	//boundary check
	if ( (pPacketType->DataRate) > MAX_BT_DATARATE) 
		pPacketType->DataRate = MAX_BT_DATARATE;

	if ( (pPacketType->DataRate) < MIN_BT_DATARATE) 
		pPacketType->DataRate = MIN_BT_DATARATE;


	if ( (pPacketType->SlotCount) > MAX_BT_HOPSLOT) 
		pPacketType->SlotCount = MAX_BT_HOPSLOT;

	if ( (pPacketType->SlotCount) < MIN_BT_HOPSLOT) 
		pPacketType->SlotCount = MIN_BT_HOPSLOT;


	if ( (payloadPattern) > MAX_PAYLOADPATTERN) 
		payloadPattern = MAX_PAYLOADPATTERN;

	if ( (payloadPattern) < MIN_PAYLOADPATTERN) 
		payloadPattern = MIN_PAYLOADPATTERN;

	Protocal = PROTOCAL_DATA_ACL;

	if(PAYLOADPATTERN_CLOSED_LOOP_SCO_PACKETS == payloadPattern) 
		Protocal = PROTOCAL_DATA_SCO;
	if(PAYLOADPATTERN_CLOSED_LOOP_ESCO_PACKETS == payloadPattern) 
		Protocal = PROTOCAL_DATA_ESCO;


	slotTimer= pPacketType->SlotCount;
	testinterval =(slotTimer+1)/2;

	payloadlength = PayloadLenghtInByte;

	ClipPayLoadLengthInByte(*((int*)pPacketType),  &payloadlength);

	
	dataRate = DataRateBbValue[Protocal][pPacketType->DataRate][pPacketType->SlotCount];

	if(0 == dataRate && enable) 	
		return ERROR_INPUT_INVALID;


	switch(payloadPattern)
	{
		case PAYLOADPATTERN_ALL0:
			payloadPattern =PCKTST_PAYLOADPATTERN_ALL0;
			break;
		case PAYLOADPATTERN_ALL1:
			payloadPattern =PCKTST_PAYLOADPATTERN_ALL1;
			break;
		case PAYLOADPATTERN_PN9:
			payloadPattern =PCKTST_PAYLOADPATTERN_PRBS;
			break;
		case PAYLOADPATTERN_HEX_AA:
			payloadPattern =PCKTST_PAYLOADPATTERN_HEX_AA;
			break;
		case PAYLOADPATTERN_HEX_F0:
			payloadPattern =PCKTST_PAYLOADPATTERN_HEX_F0;
			break;
		
		
		case PAYLOADPATTERN_LOOPBACK_ACL:
			payloadPattern =PCKTST_LOOPBACK_ACL;
			break;
		case PAYLOADPATTERN_CLOSED_LOOP_SCO_PACKETS:
		case PAYLOADPATTERN_CLOSED_LOOP_ESCO_PACKETS:
			payloadPattern =PCKTST_CLOSED_LOOP_SYNCHRONOUS_PACKETS;
			break;		
		default: 
			return ERROR_INPUT_INVALID;
			break;
	}


	TxBuf.RxOnStart		= 0xE9;
	TxBuf.SyntOnStart	= 0x81;
 	TxBuf.TxOnStart		= 0xB7;
	TxBuf.PhdOffStart	= 0xEB;
	TxBuf.PayloadPattern	= enable?payloadPattern:0xFF;
	TxBuf.RandomHopMode	= HopMode;
	TxBuf.BtChannel		= BtChannel;
	TxBuf.RxChannel		= BtChannel;
	TxBuf.TestInterval	= testinterval;
	TxBuf.PacketType	= dataRate;
	TxBuf.payloadLength = payloadlength;
	TxBuf.Whitening		= Whitening;

	TxBuf.TxCnt			= TxCnt;	 
	TxBuf.TxPwrLvl		= TxPwrLvl;	 

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));
	if(status) return status;

//	memcpy(BdAddress,  TxBuf.BdAddress, sizeof(TxBuf.BdAddress));

	if (!enable && BT_CHANNEL_INVALID != currentBtCh)
	{
  		Sleep(100);
//		DebugLogRite("currentBtCh %d\n", currentBtCh);
		status = SetBtChannel(currentBtCh);
		currentBtCh = BT_CHANNEL_INVALID;
	}
	return status;
}

#endif //#ifdef _HCI_PROTOCAL_



#ifdef _HCI_PROTOCAL_

int DutBtApiClssHeader BtMrvlRxTest(int RxChannel, 
									int PacketNumber,	
									int PacketType, 
			 int payloadPattern,	
			 int PayloadLenghtInByte, 
			 char TxBdAddress[SIZE_BDADDRESS_INBYTE]
		//	 , int TxAmAddress=DEF_AMADDRESS, 
		//	 int FramSkip=0, 
		//	 int CorrWndw=DEF_CORRELATION_WINDOW, 
		//	 int CorrTh=DEF_CORRELATION_THRESHOLD, 
		//	 bool Whitening=0,BYTE PrbsPattern[LEN_PRBS_INBYTE]=NULL
		) 
{
	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_RXTEST)
	#undef THIS_EVENT 
	#define THIS_EVENT	 ((HCI_EVENT_VENDOR_SPECIFIC<<8)|(HCI_EVENT_CMD_COMPLETE))
//	#define THIS_EVENT	 (HCI_EVENT_VENDOR_SPECIFIC)
//	#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_PfwCmd_RxTest* pTxBuf=NULL;
//	Bt_PfwRsp_RxTest* pRxBuf=NULL;
	char* pBuf=NULL;
	UINT BufSize=0;
	int BtChannel=0x20;
//	int currentBtCh=BT_CHANNEL_INVALID;
	Bt_PacketTypeCmd *pPacketType=NULL;
 	int dataRate=4, slotTimer=1, payloadlength=0x1B, testinterval=1;
	int i=0;

//	if (PacketNumber>0 && BT_CHANNEL_INVALID == currentBtCh)
//		status = GetBtChannel(&currentBtCh);
//	if(status) currentBtCh = BT_CHANNEL_INVALID;

//	if(currentBtCh>=0 && currentBtCh<MAX_NUM_BT_CH)
//		BtChannel = currentBtCh;
// based on PacketType, to get dataRate, and payloadlength
	pPacketType = (Bt_PacketTypeCmd *)&PacketType;

	//boundary check
	if ( (pPacketType->DataRate) > MAX_BT_DATARATE) 
		pPacketType->DataRate = MAX_BT_DATARATE;

	if ( (pPacketType->DataRate) < MIN_BT_DATARATE) 
		pPacketType->DataRate = MIN_BT_DATARATE;


	if ( (pPacketType->SlotCount) > MAX_BT_HOPSLOT) 
		pPacketType->SlotCount = MAX_BT_HOPSLOT;

	if ( (pPacketType->SlotCount) < MIN_BT_HOPSLOT) 
		pPacketType->SlotCount = MIN_BT_HOPSLOT;


	if ( (payloadPattern) > MAX_PAYLOADPATTERN && (payloadPattern != PAYLOADPATTERN_ABORT)) 
		payloadPattern = MAX_PAYLOADPATTERN;

	if ( (payloadPattern) < MIN_PAYLOADPATTERN) 
		payloadPattern = MIN_PAYLOADPATTERN;


	slotTimer= pPacketType->SlotCount;
	testinterval =(slotTimer+1)/2;

	payloadlength = PayloadLenghtInByte;

	ClipPayLoadLengthInByte(*((int*)pPacketType),  &payloadlength);

 
	switch(pPacketType->DataRate)
	{
		case 0:
		{
			switch(pPacketType->SlotCount)
			{
				case 1: 
					dataRate=0x03; // DM1
					break;
				case 3: 
					dataRate=0x0A; // DM3
					break;
				case 5: 
					dataRate=0x0E; // DM5
					break;
				default: 
					return ERROR_INPUT_INVALID;
					break;
			}
			break;
		}
		case 1:
		{
			switch(pPacketType->SlotCount)
			{
				case 1: 
					dataRate=0x04; // DH1
					break;
				case 3: 
					dataRate=0x0B; // DH3
					break;
				case 5: 
					dataRate=0x0F; // DH5
					break;
				default: 
					return ERROR_INPUT_INVALID;
					break;
			}
			break;
		}
		case 2:
		{
			switch(pPacketType->SlotCount)
			{
				case 1: 
					dataRate=0x14; // 2-DH1
					break;
				case 3: 
					dataRate=0x1A; // 2-DH3
					break;
				case 5: 
					dataRate=0x1E; // 2-DH5
					break;
				default: 
					return ERROR_INPUT_INVALID;
					break;
			}
			break;
		}	
 		case 3: 
		{
			switch(pPacketType->SlotCount)
			{
				case 1: 
					dataRate=0x18; // 3-DH1
					break;
				case 3: 
					dataRate=0x1B; // 3-DH3
					break;
				case 5: 
					dataRate=0x1F; // 3-DH5
					break;
				default: 
					return ERROR_INPUT_INVALID;
					break;
			}
			break;
		}
		default: 
			return ERROR_INPUT_INVALID;
			break;
	}

	switch(payloadPattern)
	{
	case PAYLOADPATTERN_ABORT:
			payloadPattern=0xFF;
			break;
		case PAYLOADPATTERN_ALL0:
			payloadPattern =1;
			break;
		case PAYLOADPATTERN_ALL1:
			payloadPattern =2;
			break;
		case PAYLOADPATTERN_PN9:
			payloadPattern =4;
			break;
		case PAYLOADPATTERN_HEX_AA:
			payloadPattern =3;
			break;
		case PAYLOADPATTERN_HEX_F0:
			payloadPattern =9;
			break;
		default: 
			return ERROR_INPUT_INVALID;
			break;
	}

	BufSize = max(sizeof(Bt_PfwCmd_RxTest), sizeof(Bt_PfwRsp_RxTest));
	pBuf = (char*)malloc(BufSize);
	memset(pBuf, 0, BufSize);
	pTxBuf = (Bt_PfwCmd_RxTest*)pBuf;

	pTxBuf->PayloadPattern	= payloadPattern;
	pTxBuf->BtChannel		= (RxChannel>40)? 20:60;
	pTxBuf->RxChannel		= RxChannel;
	pTxBuf->PacketType	= dataRate;
	pTxBuf->PacketNumber	= PacketNumber;
	pTxBuf->payloadLength = payloadlength;

	pTxBuf->TxAmAddress		= DEF_AMADDRESS; // TxAmAddress;
	
	if (TxBdAddress) 
	{
		for (i=0; i<SIZE_BDADDRESS_INBYTE; i++)
    		pTxBuf->TxBdAddress[i] = TxBdAddress[SIZE_BDADDRESS_INBYTE-i-1];
	}

	pTxBuf->ReportErrors =0; //NONE

 	status = queryHci(THIS_OID, (unsigned char*)pTxBuf, sizeof(*pTxBuf), 
 						THIS_EVENT, //(unsigned char*)pRxBuf, 
 						&BufSize);

	if (pBuf) free(pBuf);

	return status;
	
}

int DutBtApiClssHeader BtMrvlRxTestResult( 
			 // report
				BYTE*		Report_Status, // status
				DWORD*		Report_TotalPcktCount,
				DWORD*		Report_HecMatchCount,
				DWORD*		Report_HecMatchCrcPckts,
				DWORD*		Report_NoRxCount,
				DWORD*		Report_CrcErrCount,
				DWORD*		Report_SuccessfulCorrelation,
				DWORD*		Report_TotalByteCount,
				DWORD*		Report_BER,
				DWORD*		Report_PER,
				DWORD*		Report_AvgRssi,
				DWORD*		Report_TotalBitsCountExpected,
				DWORD*		Report_TotalBitsCountError_LostDrop) 

{
	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_RXTEST)
	#undef THIS_EVENT 
	#define THIS_EVENT	 ((HCI_EVENT_VENDOR_SPECIFIC<<8)|(HCI_EVENT_CMD_COMPLETE))

	int status = 0;
	Bt_PfwCmd_RxTest* pTxBuf=NULL;
	Bt_PfwRsp_RxTest* pRxBuf=NULL;
	char* pBuf=NULL;
	UINT BufSize=0;
	int BtChannel=0x20;
	int currentBtCh=BT_CHANNEL_INVALID;
	Bt_PacketTypeCmd *pPacketType=NULL;

	BufSize = max(sizeof(Bt_PfwCmd_RxTest), sizeof(Bt_PfwRsp_RxTest));
	pBuf = (char*)malloc(BufSize);
	memset(pBuf, 0, BufSize);
	pTxBuf = (Bt_PfwCmd_RxTest*)pBuf;
	pRxBuf = (Bt_PfwRsp_RxTest*)pBuf;

	pTxBuf->PayloadPattern	= 0xFF;// payloadPattern;
	pTxBuf->BtChannel		= 0;
	pTxBuf->RxChannel		= 0;
	pTxBuf->PacketType	= 0;
	pTxBuf->PacketNumber	= 0;
	pTxBuf->payloadLength = 0;
	pTxBuf->TxAmAddress		= 1;
	
	memset(	pTxBuf->TxBdAddress, 0, SIZE_BDADDRESS_INBYTE);
  
	pTxBuf->ReportErrors =0; //NONE

 	status = queryHci(THIS_OID, (unsigned char*)pTxBuf, sizeof(*pTxBuf), 
 		THIS_EVENT, //(unsigned char*)pRxBuf, 
 		&BufSize);
	if(status) return status;

	if(Report_Status) 
		*Report_Status = pRxBuf->Status_Complete0_Abort1; // test scenario
	
	if(Report_TotalPcktCount) 
		*Report_TotalPcktCount		= pRxBuf->TotalPcktCountExpected;
		
	if(Report_HecMatchCount) 
		*Report_HecMatchCount		= pRxBuf->HecMatchCount;

	if(Report_HecMatchCrcPckts) 
		*Report_HecMatchCrcPckts		= pRxBuf->HecMatchCrcPckts;

	if(Report_NoRxCount) 
		*Report_NoRxCount			= pRxBuf->NoRxCount;
		
	if(Report_CrcErrCount) 
		*Report_CrcErrCount			= pRxBuf->CrcErrCount;
		
	if(Report_SuccessfulCorrelation) 
		*Report_SuccessfulCorrelation	= pRxBuf->SuccessfulCorrelation;
		
	if(Report_TotalByteCount) 
		*Report_TotalByteCount		= pRxBuf->TotalByteCountRxed;	 
		
	if(Report_BER) 
		*Report_BER				= pRxBuf->BER;	 

	if(Report_PER) 
		*Report_PER					= pRxBuf->PER;  
		
	if(Report_AvgRssi) 
		*Report_AvgRssi				= pRxBuf->RSSI;

	if ((0 == pRxBuf->Status_Complete0_Abort1) && 
		(0 == pRxBuf->TotalPcktCountExpected ) )
		status = ERROR_MISMATCH;
	
	if (BtIf_GetDebugLogFlag())
	{
		DebugLogRite("Status_Complete0_Abort1\t0x%02X\n", 
			pRxBuf->Status_Complete0_Abort1);

		DebugLogRite("TotalPcktCountExpected\t\t0x%08X\n", 
			pRxBuf->TotalPcktCountExpected);

		DebugLogRite("NoRxCount\t\t0x%08X\n", 
			pRxBuf->NoRxCount);

		DebugLogRite("SuccessfulCorrelation\t0x%08X\n", 
			pRxBuf->SuccessfulCorrelation);

		DebugLogRite("HecMatchCount\t\t0x%08X\n", 
			pRxBuf->HecMatchCount);

		DebugLogRite("HecMatchCrcPckts\t0x%08X\n", 
			pRxBuf->HecMatchCrcPckts);

		DebugLogRite("PayLoadHdrErrorCount\t\t0x%08X\n", 
			pRxBuf->PayLoadHdrErrorCount);

		DebugLogRite("CrcErrCount\t\t0x%08X\n", 
			pRxBuf->CrcErrCount);
		
		DebugLogRite("TotalPacketRxedCount\t\t0x%08X\n", 
			pRxBuf->TotalPacketRxedCount);

		DebugLogRite("PacketOkCount\t\t0x%08X\n", 
			pRxBuf->PacketOkCount);

		DebugLogRite("DroppedPacketCount\t\t0x%08X\n", 
			pRxBuf->DroppedPacketCount);

		DebugLogRite("PER\t\t\t0x%08X\n", 
			pRxBuf->PER);

		DebugLogRite("TotalBitsCountExpected\t\t0x%08X\n", 
			pRxBuf->TotalBitsCountExpected);

		DebugLogRite("TotalBitsCountError_LostDrop\t\t0x%08X\n", 
			pRxBuf->TotalBitsCountError_LostDrop);

		DebugLogRite("BER\t\t\t0x%08X\n", 
			pRxBuf->BER);

		DebugLogRite("TotalByteCountRxed\t\t0x%08X\n", 
			pRxBuf->TotalByteCountRxed);

		DebugLogRite("TotalBitErrorRxed\t\t0x%08X\n", 
			pRxBuf->TotalBitErrorRxed);

		DebugLogRite("RSSI\t\t\t0x%08X\n", 
			pRxBuf->RSSI);
	}

	if (BT_CHANNEL_INVALID != currentBtCh)
		status = SetBtChannel(currentBtCh);

	if(pBuf) free(pBuf);

	return status;
	
}

/*

int DutBtApiClssHeader TxBtRxTest(int FrameCount,	int PacketType, 
			 int payloadPattern,	
			 int PayloadLenghtInByte, 
			 BYTE TxBdAddress[SIZE_BDADDRESS_INBYTE], int TxAmAddress, 
			 int FramSkip, int CorrWndw, int CorrTh, 
			 bool Whitening,BYTE PrbsPattern[LEN_PRBS_INBYTE],
			 // report
	BYTE*		Report_Status_Complete0_Abort1, // status
	DWORD*		Report_TotalPcktCount,
	DWORD*		Report_HecMatchCount,
	DWORD*		Report_HecMatchCrcPckts,
	DWORD*		Report_NoRxCount,
	DWORD*		Report_CrcErrCount,
	DWORD*		Report_SuccessfulCorrelation,
	DWORD*		Report_TotalByteCount,
	DWORD*		Report_ErrorBits,
	DWORD*		Report_PER,
	DWORD*		Report_BER
			 )
{
	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_RXTEST)
	#undef THIS_EVENT 
	#define THIS_EVENT	 ((HCI_EVENT_VENDOR_SPECIFIC<<8)|(HCI_EVENT_CMD_COMPLETE))
//	#define THIS_EVENT	 (HCI_EVENT_VENDOR_SPECIFIC)
//	#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_PfwCmd_RxTest* pTxBuf=NULL;
	Bt_PfwRsp_RxTest* pRxBuf=NULL;
	char* pBuf=NULL;
	UINT BufSize=0;
	int BtChannel=0x20;
	int currentBtCh=BT_CHANNEL_INVALID;
	Bt_PacketTypeCmd *pPacketType=NULL;
 	int dataRate=4, slotTimer=1, payloadlength=0x1B, testinterval=1;

	if (FrameCount>0 && BT_CHANNEL_INVALID == currentBtCh)
		status = GetBtChannel(&currentBtCh);
	if(status) currentBtCh = BT_CHANNEL_INVALID;

	if(currentBtCh>=0 && currentBtCh<MAX_NUM_BT_CH)
		BtChannel = currentBtCh;
// based on PacketType, to get dataRate, and payloadlength
	pPacketType = (Bt_PacketTypeCmd *)&PacketType;

	//boundary check
	if ( (pPacketType->DataRate) > MAX_BT_DATARATE) 
		pPacketType->DataRate = MAX_BT_DATARATE;

	if ( (pPacketType->DataRate) < MIN_BT_DATARATE) 
		pPacketType->DataRate = MIN_BT_DATARATE;


	if ( (pPacketType->SlotCount) > MAX_BT_HOPSLOT) 
		pPacketType->SlotCount = MAX_BT_HOPSLOT;

	if ( (pPacketType->SlotCount) < MIN_BT_HOPSLOT) 
		pPacketType->SlotCount = MIN_BT_HOPSLOT;


	if ( (payloadPattern) > MAX_PAYLOADPATTERN && (payloadPattern != PAYLOADPATTERN_ABORT)) 
		payloadPattern = MAX_PAYLOADPATTERN;

	if ( (payloadPattern) < MIN_PAYLOADPATTERN) 
		payloadPattern = MIN_PAYLOADPATTERN;


	slotTimer= pPacketType->SlotCount;
	testinterval =(slotTimer+1)/2;

//	payloadlength=DutyWeight*slotDur[slotTimer]/100;  // in us

	payloadlength = PayloadLenghtInByte;

	ClipPayLoadLengthInByte(*((int*)pPacketType),  &payloadlength);

/ *
	if(PAYLOADLENGTH_MAXIMUN_POSSIBLE == PayloadLenghtInByte)
	{
		payloadlength = maxPayloadInByte[pPacketType->DataRate][slotTimer];
	}
	else if( 0 > PayloadLenghtInByte)
	{
		payloadlength = 0;
	}
	else
	{
		payloadlength = PayloadLenghtInByte> maxPayloadInByte[pPacketType->DataRate][slotTimer]? 
			maxPayloadInByte[pPacketType->DataRate][slotTimer]:PayloadLenghtInByte;
	}
  * /
	switch(pPacketType->DataRate)
	{
		case 0:
		{
			switch(pPacketType->SlotCount)
			{
				case 1: 
					dataRate=0x03; // DM1
					break;
				case 3: 
					dataRate=0x0A; // DM3
					break;
				case 5: 
					dataRate=0x0E; // DM5
					break;
				default: 
					return ERROR_INPUT_INVALID;
					break;
			}
			break;
		}
		case 1:
		{
			switch(pPacketType->SlotCount)
			{
				case 1: 
					dataRate=0x04; // DH1
					break;
				case 3: 
					dataRate=0x0B; // DH3
					break;
				case 5: 
					dataRate=0x0F; // DH5
					break;
				default: 
					return ERROR_INPUT_INVALID;
					break;
			}
			break;
		}
		case 2:
		{
			switch(pPacketType->SlotCount)
			{
				case 1: 
					dataRate=0x14; // 2-DH1
					break;
				case 3: 
					dataRate=0x1A; // 2-DH3
					break;
				case 5: 
					dataRate=0x1E; // 2-DH5
					break;
				default: 
					return ERROR_INPUT_INVALID;
					break;
			}
			break;
		}	
 		case 3: 
		{
			switch(pPacketType->SlotCount)
			{
				case 1: 
					dataRate=0x18; // 3-DH1
					break;
				case 3: 
					dataRate=0x1B; // 3-DH3
					break;
				case 5: 
					dataRate=0x1F; // 3-DH5
					break;
				default: 
					return ERROR_INPUT_INVALID;
					break;
			}
			break;
		}
		default: 
			return ERROR_INPUT_INVALID;
			break;
	}

	switch(payloadPattern)
	{
	case PAYLOADPATTERN_ABORT:
			payloadPattern=0xFF;
			break;
		case PAYLOADPATTERN_ALL0:
			payloadPattern =1;
			break;
		case PAYLOADPATTERN_ALL1:
			payloadPattern =2;
			break;
		case PAYLOADPATTERN_PN9:
			payloadPattern =4;
			break;
		case PAYLOADPATTERN_HEX_AA:
			payloadPattern =3;
			break;
		case PAYLOADPATTERN_HEX_F0:
			payloadPattern =9;
			break;
		default: 
			return ERROR_INPUT_INVALID;
			break;
	}

	BufSize = max(sizeof(Bt_PfwCmd_RxTest), sizeof(Bt_PfwRsp_RxTest));
	pBuf = (char*)malloc(BufSize);
	memset(pBuf, 0, BufSize);
	pTxBuf = (Bt_PfwCmd_RxTest*)pBuf;
	pRxBuf = (Bt_PfwRsp_RxTest*)pBuf;

	pTxBuf->PayloadPattern	= payloadPattern;
	pTxBuf->BtChannel		= BtChannel;
	pTxBuf->RxChannel		= BtChannel;
	pTxBuf->PacketType	= dataRate;
	pTxBuf->PacketNumber	= FrameCount>10?10:FrameCount;
	pTxBuf->payloadLength = payloadlength;
	pTxBuf->Whitening		= Whitening;
	pTxBuf->RxDelayPacket		= (FramSkip>(slotTimer*2))? FramSkip:(slotTimer*2);
	pTxBuf->CorrelationWindow		= CorrWndw;
	if(PrbsPattern) 
		memcpy(	pTxBuf->PRBS, PrbsPattern, LEN_PRBS_INBYTE);
	else
		memset(	pTxBuf->PRBS, 0, LEN_PRBS_INBYTE);

	pTxBuf->TxAmAddress		= TxAmAddress;
	
//	GetBDAddress(pTxBuf->TxBdAddress);
	if (TxBdAddress) 
		memcpy(pTxBuf->TxBdAddress, TxBdAddress, SIZE_BDADDRESS_INBYTE);
 
	pTxBuf->ReportErrors =0; //NONE
	pTxBuf->CorrelationThreshold=CorrTh;

 	status = queryHci(THIS_OID, (unsigned char*)pTxBuf, sizeof(*pTxBuf), 
 		THIS_EVENT, //(unsigned char*)pRxBuf, 
 		&BufSize);
//// 	status = BtIf_query(THIS_OID, (unsigned char*)pTxBuf, sizeof(*pTxBuf));
	if(status) return status;

	{
 
		if(Report_Status_Complete0_Abort1) 
			*Report_Status_Complete0_Abort1 = pRxBuf->Status_Complete0_Abort1; // test scenario
		
		if(Report_TotalPcktCount) 
			*Report_TotalPcktCount		= pRxBuf->TotalPcktCount;
		
		if(Report_HecMatchCount) 
			*Report_HecMatchCount		= pRxBuf->HecMatchCount;

		if(Report_HecMatchCrcPckts) 
			*Report_HecMatchCrcPckts		= pRxBuf->HecMatchCrcPckts;

		if(Report_NoRxCount) 
			*Report_NoRxCount			= pRxBuf->NoRxCount;
		
		if(Report_CrcErrCount) 
			*Report_CrcErrCount			= pRxBuf->CrcErrCount;
		
		if(Report_SuccessfulCorrelation) 
			*Report_SuccessfulCorrelation	= pRxBuf->SuccessfulCorrelation;
		
		if(Report_TotalByteCount) 
			*Report_TotalByteCount		= pRxBuf->TotalByteCount;	 
		
		if(Report_ErrorBits) 
			*Report_ErrorBits			= pRxBuf->ErrorBits;	 

		if(Report_PER) 
			*Report_PER				= pRxBuf->PER;  
		
		if(Report_BER) 
			*Report_BER				= pRxBuf->BER;
/ *
	DebugLogRite("Status_Complete0_Abort1\t\t0x%02X\n", 
		Report_Status_Complete0_Abort1);
	DebugLogRite("TotalPcktCount\t\t0x%08X\n", Report_TotalPcktCount);
	DebugLogRite("HecMatchCount\t\t0x%08X\n", Report_HecMatchCount);
	DebugLogRite("HecMatchCrcPckts\t\t0x%08X\n", Report_HecMatchCrcPckts);
	DebugLogRite("NoRxCount\t\t0x%08X\n", Report_NoRxCount);
	DebugLogRite("CrcErrCount\t\t0x%08X\n", Report_CrcErrCount);
	DebugLogRite("SuccessfulCorrelation\t\t0x%08X\n", 
		Report_SuccessfulCorrelation);
	DebugLogRite("TotalByteCount\t\t0x%08X\n", Report_TotalByteCount);
	DebugLogRite("ErrorBits\t\t0x%08X\n", Report_ErrorBits);
	DebugLogRite("PER\t\t0x%08X\n", Report_PER);
	DebugLogRite("BER\t\t0x%08X\n", Report_BER);
	DebugLogRite("\n\n");

	DebugLogRite("Status_Complete0_Abort1\t0x%02X\n", 
		pRxBuf->Status_Complete0_Abort1);
	DebugLogRite("TotalPcktCount\t\t0x%08X\n", pRxBuf->TotalPcktCount);
	DebugLogRite("HecMatchCount\t\t0x%08X\n", pRxBuf->HecMatchCount);
	DebugLogRite("HecMatchCrcPckts\t0x%08X\n", pRxBuf->HecMatchCrcPckts);
	DebugLogRite("NoRxCount\t\t0x%08X\n", pRxBuf->NoRxCount);
	DebugLogRite("CrcErrCount\t\t0x%08X\n", pRxBuf->CrcErrCount);
	DebugLogRite("SuccessfulCorrelation\t0x%08X\n", 
		pRxBuf->SuccessfulCorrelation);
	DebugLogRite("TotalByteCount\t\t0x%08X\n", pRxBuf->TotalByteCount);
	DebugLogRite("ErrorBits\t\t0x%08X\n", pRxBuf->ErrorBits);
	DebugLogRite("PER\t\t\t0x%08X\n", pRxBuf->PER);
	DebugLogRite("BER\t\t\t0x%08X\n", pRxBuf->BER);
* / 	
	}

	if (BT_CHANNEL_INVALID != currentBtCh)
		status = SetBtChannel(currentBtCh);
	free(pBuf);

	return status;
}
*/
#endif // #ifdef _HCI_PROTOCAL_

//////////////////////////////////////////////////////////
int DutBtApiClssHeader MfgBtCmdTemplete(void)
{
	int status = 0;

 
#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_NONE
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_GET

	mfgBt_Cmd_Templete_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf...

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
 
	// pass back get values
	// if(output) *output = TxBuf...
 
	return status;
}


void DutBtApiClssHeader StuffHeader(void *TxBuf, DWORD CmdCode, DWORD Action,DWORD size)
{

#if defined(_BT_USB_) || defined(_BT_SDIO_)
	//for USB, SDIO
	static	int seqnum=0;
	seqnum++;
	((BtPkHeader*)TxBuf)->cmd = HostCmd_CMD_MFG_COMMAND;
	((BtPkHeader*)TxBuf)->len = (unsigned short) size;
	((BtPkHeader*)TxBuf)->seq = seqnum;
	((BtPkHeader*)TxBuf)->result = 0;
#endif
    //for uart data
	((BtPkHeader*)TxBuf)->MfgCmd = CmdCode;
	((BtPkHeader*)TxBuf)->Action = Action;
	((BtPkHeader*)TxBuf)->Error = 0;

}

int DutBtApiClssHeader CheckRspHeader(void *TxBuf, DWORD RspCode)
{
	int status=ERROR_NONE;

	if(((BtPkHeader*)TxBuf)->MfgCmd != RspCode) 
	{
		status = ERROR_ERROR; 
		if (BtIf_GetDebugLogFlag())
			DebugLogRite(" --------- ERR cmd %08X; expected %08X\n", 
				((BtPkHeader*)TxBuf)->MfgCmd, RspCode);
	}
	
	if(((BtPkHeader*)TxBuf)->Error  != ERROR_NONE) 
	{
		status =  ((BtPkHeader*)TxBuf)->Error;
		if (BtIf_GetDebugLogFlag())
			DebugLogRite ("Resp result  %08X\n", 
			((BtPkHeader*)TxBuf)->Error);
	}

	return status;
}

 
int DutBtApiClssHeader Bt_GetHwVersion(int *pSocChipId,int *pSocChipVersion,
					int *pBtuChipId,int *pBtuChipVersion,
					int *pBrfChipId,int *pBrfChipVersion)
{
	int status=0;
 
#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_GET_HW_VERS
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_GET
 
	mfgBt_CmdHwVers_t TxBuf={0};
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf...
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));
#if defined(_LINUX_) 
	usleep(1000);	//temp solution
#endif
	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(pSocChipId)		*pSocChipId = TxBuf.socId;
	if(pSocChipVersion) *pSocChipVersion = TxBuf.socVersion;
	if(pBtuChipId)		*pBtuChipId = TxBuf.bbId;
	if(pBtuChipVersion) *pBtuChipVersion = TxBuf.bbVersion;
	if(pBrfChipId)		*pBrfChipId = TxBuf.rfId;
	if(pBrfChipVersion) *pBrfChipVersion = TxBuf.rfVersion;
 
	return status;
}

int DutBtApiClssHeader Bt_GetFwVersion(int *pFwVersion, int *pMfgVersion, 
					char *pVersionString)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_FW_VERS
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_GET
 
	mfgBt_CmdFwVers_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf...
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));
#if defined(_LINUX_) 	
	usleep(1000);	//temp solution
#endif
	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
	if(pFwVersion) (*pFwVersion) =TxBuf.FwVersion;
	if(pMfgVersion) (*pMfgVersion)=TxBuf.MfgVersion;

	if(pVersionString) 
		sprintf(pVersionString, "FW Version:  %d.%d.%d.%02d\t"
								"Mfg Version: %d.%d.%d.%02d",
			(((TxBuf.FwVersion)&0xFF000000)>>24),
			(((TxBuf.FwVersion)&0x00FF0000)>>16),
			(((TxBuf.FwVersion)&0x0000FF00)>>8),
 			(((TxBuf.FwVersion)&0x000000FF)>>0),
			(((TxBuf.MfgVersion)&0xFF000000)>>24),
			(((TxBuf.MfgVersion)&0x00FF0000)>>16),
			(((TxBuf.MfgVersion)&0x0000FF00)>>8),
 			(((TxBuf.MfgVersion)&0x000000FF)>>0));

 
	return status;
}


int DutBtApiClssHeader Bt_GetCurrentORVersion(int *pSocVersion, int *pRfVersion)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_CURRENT_OR_REV
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_GET
 
	mfgBt_CmdFwVers_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf...
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));
#if defined(_LINUX_)
	usleep(1000);	//temp solution
#endif
	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
 
	if(pSocVersion) (*pSocVersion) =TxBuf.FwVersion;
	if(pRfVersion) (*pRfVersion)=TxBuf.MfgVersion;

	return status;
}


int DutBtApiClssHeader Bt_GetCurrentAppMode(int *pAppMode)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_CURRENT_APPMODE
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_GET
 
	mfgBt_CmdFwVers_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf...
 
 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
 

	if(pAppMode) (*pAppMode) =TxBuf.FwVersion;
 
	return status;
}


/* 
bool DutBtApiClssHeader SupportNoE2PROM()
{
	return g_NoEeprom;
}
*/ 


int DutBtApiClssHeader ClipPayLoadLengthInByte(int PacketType,  
										 int *pPayloadLenghtInByte)
{
	Bt_PacketTypeCmd *pPacketType=NULL;

	if(!pPayloadLenghtInByte) return ERROR_INPUT_INVALID; 

	pPacketType = (Bt_PacketTypeCmd *)&PacketType;

	if(PAYLOADLENGTH_MAXIMUN_POSSIBLE == (*pPayloadLenghtInByte))
	{
		(*pPayloadLenghtInByte) = maxPayloadInByte[pPacketType->DataRate][pPacketType->SlotCount];
	}
	else if( 0 > (*pPayloadLenghtInByte))
	{
		(*pPayloadLenghtInByte) = 0;
	}
	else
	{
		(*pPayloadLenghtInByte) = (*pPayloadLenghtInByte)> maxPayloadInByte[pPacketType->DataRate][pPacketType->SlotCount]? 
			maxPayloadInByte[pPacketType->DataRate][pPacketType->SlotCount]:(*pPayloadLenghtInByte);
	}
  
	return ERROR_NONE;
}

int DutBtApiClssHeader InvalidPacketType(int PacketType)
{
		
	Bt_PacketTypeCmd *pPacketType=NULL;
	
	pPacketType = (Bt_PacketTypeCmd *)&PacketType;

	switch(pPacketType->DataRate)
	{
		case 0:
		case 1:
		case 2:
 		case 3: 
		{
			switch(pPacketType->SlotCount)
			{
				case 1: 
				case 3: 
				case 5: 
					break;
				default: 
					return ERROR_INPUT_INVALID;
					break;
			}
			break;
		}
		default: 
			return ERROR_INPUT_INVALID;
			break;
	}
	
	switch(pPacketType->SlotCount)
	{
		case 1:
		case 3:
		case 5:
			break;
		default:
			return ERROR_INPUT_INVALID;
			break;
	}
	
	return ERROR_NONE;
}

#ifdef _HCI_PROTOCAL_
/*
int DutBtApiClssHeader getPacketTypeSetting(int PacketType)
{

	Bt_PacketTypeCmd *pPacketType=NULL;
	
	if(InvalidPacketType(PacketType)) return ERROR_INPUT_INVALID;

	pPacketType = (Bt_PacketTypeCmd *)&PacketType;

 	switch (pPacketType->DataRate)
	{ 
		case 0:
		{	//1DM-x
			switch(pPacketType->SlotCount)
			{
				case 1: 
					return 0x330E; // IDC_DM1
					break;
				case 3: 
					return 0x3706; // IDC_DM3
					break;
				case 5: 
					return 0x7306; // IDC_DM5:
					break;
				default: 
					return ERROR_INPUT_INVALID;
					break;
			}
			break;
		}
		case 1:
		{	//1DH-x
			switch(pPacketType->SlotCount)
			{
				case 1: 
					return 0x3316; // IDC_DH1
					break;
				case 3: 
					return 0x3B06; // IDC_DH3
					break;
				case 5: 
					return 0xB306; // IDC_DH5:
					break;
				default: 
					return ERROR_INPUT_INVALID;
					break;
			}
			break;
		}
		case 2:
		{	//2DH-x
			switch(pPacketType->SlotCount)
			{
				case 1: 
					return 0x3304; // IDC_2DH1
					break;
				case 3: 
					return 0x3206; // IDC_2DH3
					break;
				case 5: 
					return 0x2306; // IDC_2DH5:
					break;
				default: 
					return ERROR_INPUT_INVALID;
					break;
			}
			break;
		}
		case 3:
		{	//3DH-x
			switch(pPacketType->SlotCount)
			{
				case 1: 
					return 0x3302; // IDC_DH1
					break;
				case 3: 
					return 0x3106; // IDC_DH3
					break;
				case 5: 
					return 0x1306; // IDC_DH5:
					break;
				default: 
					return ERROR_INPUT_INVALID;
					break;
			}
			break;
		}
		default: 
			return ERROR_INPUT_INVALID;
			break;	
	} 

	return ERROR_ERROR;
}
*/
#endif //#ifdef _HCI_PROTOCAL_


int DutBtApiClssHeader GetFwReleaseNote(char *pReleaseNote)
{
	int status=0;

#undef THIS_OID 
#define THIS_OID	 MRVL_HCI_OID_MFG_BT
#undef THIS_CMD 
#define THIS_CMD	 MFG_BT_CMD_RELEASENOTE
#undef THIS_RSP 
#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
#undef THIS_ACTION 
#define THIS_ACTION	 HOST_ACT_GET

	mfgBt_CmdReleaseNote_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);
 
	if(pReleaseNote) 
		strcpy(pReleaseNote, (char*)TxBuf.releaseNote);

	return status;

}



 

#ifdef _HCI_PROTOCAL_
///////////////////
#define HCI_CMD_MRVL_SETMRVLTXPOWERCONTROL		(0x0052)

typedef struct  Bt_CmdSetMrvlTxPowerCtrl_HC
{
 	BYTE	HwMrvlModeOnly;
 	BYTE	SwMrvlMode;
 	WORD	RefGain1;
 	WORD	RefGain2;
 	BYTE	RefTemp;
 	BYTE	TempGain;
} PACK Bt_CmdSetMrvlTxPowerCtrl_HC;

int DutBtApiClssHeader SetMrvlTempCompModeHC(int		SwPwrCtrlMode,
											WORD	RefGain1,
 											WORD	RefGain2,
 											BYTE	RefTemp,
 											BYTE	TempGain)
{
#undef THIS_OID 
#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_SETMRVLTXPOWERCONTROL)
#undef THIS_EVENT 
#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)
	int status = 0;
	Bt_CmdSetMrvlTxPowerCtrl_HC	*pTxBuf=NULL;
	BYTE *pBuf=NULL;
	UINT EventBuf=sizeof(EVENT_COMMAND_COMPLETE);

 	pBuf=(BYTE*)malloc(max(sizeof(*pTxBuf),	sizeof(EVENT_COMMAND_COMPLETE)));
	pTxBuf = (Bt_CmdSetMrvlTxPowerCtrl_HC*) pBuf;
	pTxBuf->HwMrvlModeOnly =0;
	pTxBuf->SwMrvlMode =SwPwrCtrlMode;
	pTxBuf->RefGain1	= RefGain1;
	pTxBuf->RefGain2	= RefGain2;
	pTxBuf->RefTemp	= RefTemp;
	pTxBuf->TempGain	= TempGain;


	status = BtIf_query(THIS_OID, pBuf, sizeof(*pTxBuf)//, 
						//	 THIS_EVENT, 
						//	 &EventBuf
							 );
    if(pBuf) free(pBuf);

	return status;
}
 

#endif //#ifdef _HCI_PROTOCAL_











////////////////////////////////////////////////////////////////////
#ifdef _HCI_PROTOCAL_
int DutBtApiClssHeader SetRfDynamicClkCtrl(int		DynClkTimer,
										   int		MultiSlotProtect,
										   int		AutoDetect,
										   int		DynClkValue,
										   int		DynClkEnable,
										   int		DynClkStartTime,
										   int		DynClkFinishTime,
										   int		DynClkTimedEnable)
{
	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_RF_DYN_CLK_CONTROL)
//	#undef THIS_EVENT 
//	#define THIS_EVENT	 ((HCI_EVENT_VENDOR_SPECIFIC<<8)|(HCI_EVENT_CMD_COMPLETE))

 	
	
	int status = 0;
	Bt_PfwCmd_RfDynamicClkCtrl TxBuf={0};
  

	TxBuf.DynClkTimer		= DynClkTimer;
	TxBuf.MultiSlotProtect	= MultiSlotProtect;
	TxBuf.AutoDetect		= AutoDetect;
	TxBuf.DynClkValue		= DynClkValue;
	TxBuf.DynClkEnable		= DynClkEnable;
	TxBuf.DynClkStartTime	= DynClkStartTime;
	TxBuf.DynClkFinishTime	= DynClkFinishTime;
	TxBuf.DynClkTimedEnable	= DynClkTimedEnable;
 
	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));
	if(status) return status;

 
 	return status;
}

int DutBtApiClssHeader SetBtSleepMode(int PowerMode)
{
	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_SETBTSLEEPMODE)
	#undef THIS_EVENT 
	#define THIS_EVENT	 ((HCI_EVENT_CMD_COMPLETE<<8)|(HCI_EVENT_MRVL_LOW_POWER_MODE))
//	#define THIS_EVENT	 (HCI_EVENT_VENDOR_SPECIFIC)
//	#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_PfwCmd_SetBtSleepMode* pTxBuf=NULL;
	Bt_PfwRsp_MrvlLowPowerModeCtrolResp* pRxBuf=NULL;
	char* pBuf=NULL;
	UINT BufSize=0;
   

	BufSize = max(sizeof(Bt_PfwCmd_SetBtSleepMode), sizeof(Bt_PfwRsp_MrvlLowPowerModeCtrolResp));
	pBuf = (char*)malloc(BufSize);
	memset(pBuf, 0, BufSize);
	pTxBuf = (Bt_PfwCmd_SetBtSleepMode*)pBuf;
	pRxBuf = (Bt_PfwRsp_MrvlLowPowerModeCtrolResp*)pBuf;

	pTxBuf->PowerMode	= (PowerMode); 
 
 	status = queryHci(THIS_OID, (unsigned char*)pTxBuf, sizeof(*pTxBuf), 
 		THIS_EVENT, //(unsigned char*)pRxBuf, 
 		&BufSize);
 	if(status) 
 	{
	    if(pBuf) free(pBuf);
 	    return status;
    }
  		
	status =  pRxBuf->Status;
		
 	if (pBuf) free(pBuf);

	return status;
}


int DutBtApiClssHeader SetWakeUpMethod(	int		HostWakeUpMethod,
										int		HostWakeUpGPIO,
										int		DeviceWakeUpMethod,
										int		DeviceWakeUpGPIO)
{
	#undef THIS_OID 
//	#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_SETBTSLEEPMODE)
	#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_WAKE_METHOD_CONTROL)
//	#undef THIS_EVENT 
//	#define THIS_EVENT	 ((HCI_EVENT_VENDOR_SPECIFIC<<8)|(HCI_EVENT_CMD_COMPLETE))

 	
	
	int status = 0;
	Bt_PfwCmd_WakeUpMethod TxBuf={0};
  
	TxBuf.HostWakeUpMethod		= HostWakeUpMethod;
	TxBuf.HostWakeUpGPIO		= HostWakeUpGPIO;
	TxBuf.DeviceWakeUpMethod	= DeviceWakeUpMethod;
	TxBuf.DeviceWakeUpGPIO		= DeviceWakeUpGPIO; 
 
	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));
	if(status) return status;

 
 	return status;
}



int DutBtApiClssHeader SetMrvlHiuModuleConfig(int ModuleShutDownBringUp, 
											  int HostIfActive, int HostIfType)
{
	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_BT_MODULE_CONF)
	#undef THIS_EVENT 
	#define THIS_EVENT	 ((HCI_EVENT_MRVL_BT_MODULE_CONF<<8)|(HCI_EVENT_CMD_COMPLETE))
//	#define THIS_EVENT	 (HCI_EVENT_VENDOR_SPECIFIC)
//	#define THIS_EVENT	 (HCI_EVENT_CMD_COMPLETE)

	int status = 0;
	Bt_PfwCmd_MrvlHiuModuleConfig* pTxBuf=NULL;
	Bt_PfwRsp_MrvlHiuModuleConfig* pRxBuf=NULL;
	char* pBuf=NULL;
	UINT BufSize=0;
   
 
  

	BufSize = max(sizeof(Bt_PfwCmd_MrvlHiuModuleConfig), sizeof(Bt_PfwRsp_MrvlHiuModuleConfig));
	pBuf = (char*)malloc(BufSize);
	memset(pBuf, 0, BufSize);
	pTxBuf = (Bt_PfwCmd_MrvlHiuModuleConfig*)pBuf;
	pRxBuf = (Bt_PfwRsp_MrvlHiuModuleConfig*)pBuf;

	pTxBuf->ModuleShutDownBringUp	= (Pfw_ModuleShutDownBringUp_Map[ModuleShutDownBringUp]); 
 	pTxBuf->HostIfActive			= HostIfActive; 
	pTxBuf->HostIfType			= HostIfType; 
 
 	status = queryHci(THIS_OID, (unsigned char*)pTxBuf, sizeof(*pTxBuf), 
 		THIS_EVENT, //(unsigned char*)pRxBuf, 
 		&BufSize);
 	if(status) 
 	{
	    if(pBuf) free(pBuf);
 	    return status;
    }
  		
	status =  pRxBuf->Status;
		
 	if(pBuf) free(pBuf);

	return status;
}




int DutBtApiClssHeader SetMrvlRadioPerformanceMode(	int		MrvlRxPowerMode,	 
 									int		PerfModeBdrRssiTh,		
									int		PerfModeEdrRssiTh,		 
									int		NormModeBdrRssiTh,	 
									int		NormModeEdrRssiTh,	 

									int		PerfModeBdrCrcTh,	 
									int		PerfModeEdrCrcTh,	 
									int		NormModeBdrCrcTh,		 
									int		NormModeEdrCrcTh,		 
									int		TxModeInTestMode,	 
									int		RxModeInTestMode,	 
									int		RxModeInOperationMode,	 
									int		RssiCrcSampleInterval, 
									int		LowPowerScanMode)
{
	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_SET_RADIO_PERF_MODE)
//	#undef THIS_EVENT 
//	#define THIS_EVENT	 ((HCI_EVENT_VENDOR_SPECIFIC<<8)|(HCI_EVENT_CMD_COMPLETE))

	int status = 0;
	Bt_PfwCmd_SetMrvlRadioPerformanceMode TxBuf={0};
  
	TxBuf.MrvlRxPowerMode		= MrvlRxPowerMode;	 
 	TxBuf.PerfModeBdrRssiTh		= PerfModeBdrRssiTh;		
	TxBuf.PerfModeEdrRssiTh		= PerfModeEdrRssiTh;		 
	TxBuf.NormModeBdrRssiTh		= NormModeBdrRssiTh;	 
	TxBuf.NormModeEdrRssiTh		= NormModeEdrRssiTh;	 
 	TxBuf.PerfModeBdrCrcTh		= PerfModeBdrCrcTh;	 
	TxBuf.PerfModeEdrCrcTh		= PerfModeEdrCrcTh;	 
	TxBuf.NormModeBdrCrcTh		= NormModeBdrCrcTh;		 
	TxBuf.NormModeEdrCrcTh		= NormModeEdrCrcTh;		 
	TxBuf.TxModeInTestMode		= TxModeInTestMode;	 
	TxBuf.RxModeInTestMode		= RxModeInTestMode;	 
	TxBuf.RxModeInOperationMode	= RxModeInOperationMode;	 
	TxBuf.RssiCrcSampleInterval	= RssiCrcSampleInterval; 
	TxBuf.LowPowerScanMode		= LowPowerScanMode; 
 
	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));
	if(status) return status;

 
 	return status;
}



int DutBtApiClssHeader SetMrvlArmDynamicClkCtrl(int		SetNcoMode,	 
 									int		DynamicClockChange 	 )
{
	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_ARM_DYNCLK_CONTROL)
//	#undef THIS_EVENT 
//	#define THIS_EVENT	 ((HCI_EVENT_VENDOR_SPECIFIC<<8)|(HCI_EVENT_CMD_COMPLETE))

	int status = 0;
	Bt_PfwCmd_MrvlArmDynamicClkCtrl TxBuf={0};
  

	TxBuf.SetNcoMode			= SetNcoMode;	 
 	TxBuf.DynamicClockChange	= DynamicClockChange;	  
 
	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));
	if(status) return status;

 
 	return status;
}





int DutBtApiClssHeader SetBtXCompModeConfig(int		BTCoexistenceMode,	 
 									int		PriorityMode_Wlan,
									int PriorityMode_UpdateDefInPta,
									int PriorityDelay,
									int HighPriority_BitMap)
{
	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_BTCOEX_XCOMP_MODE)
//	#undef THIS_EVENT 
//	#define THIS_EVENT	 ((HCI_EVENT_VENDOR_SPECIFIC<<8)|(HCI_EVENT_CMD_COMPLETE))

	int status = 0;
	Bt_PfwCmd_BtXCompModeConfig TxBuf={0};
  
	TxBuf.BTCoexistenceMode				= BTCoexistenceMode;	 
 	TxBuf.PriorityMode_Wlan				= PriorityMode_Wlan;	  
	TxBuf.PriorityMode_UpdateDefInPta	= PriorityMode_UpdateDefInPta;	 
 	TxBuf.PriorityDelay_BitMap			= PriorityDelay;	  
	TxBuf.HighPriority_BitMap			= HighPriority_BitMap;	 
  
	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));
	if(status) return status;

 
 	return status;
}






int DutBtApiClssHeader SetBtMrvlModeConfig(int		BTCoexistenceMode,	 
 									int PriorityMode_Wlan,
									int PriorityMode_UpdateDefInPta,
									int PriorityDelay,
									int HighPriority_BitMap, 
									int MediumPriority_BitMap,
									int BtEndTimer,
									int BtFreqLowBand,
									int BtFreqHighBand
									)
{
	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_SET_RADIO_PERF_MODE)
//	#undef THIS_EVENT 
//	#define THIS_EVENT	 ((HCI_EVENT_VENDOR_SPECIFIC<<8)|(HCI_EVENT_CMD_COMPLETE))

	int status = 0;
	Bt_PfwCmd_BtMrvlModeConfig TxBuf={0};
  

	TxBuf.BTCoexistenceMode				= BTCoexistenceMode;	 
 	TxBuf.PriorityMode_Wlan				= PriorityMode_Wlan;	  
	TxBuf.PriorityMode_UpdateDefInPta	= PriorityMode_UpdateDefInPta;	 
 	TxBuf.PriorityDelay_BitMap			= PriorityDelay;	  
	TxBuf.HighPriority_BitMap			= HighPriority_BitMap;	 
	TxBuf.MediumPriority_BitMap			= MediumPriority_BitMap;	 
 	TxBuf.BtEndTimer					= BtEndTimer;	  
 	TxBuf.BtFreqLowBand					= BtFreqLowBand;	  
 	TxBuf.BtFreqHighBand				= BtFreqHighBand;	  
 
	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));
	if(status) return status;

 
 	return status;
}

   


int DutBtApiClssHeader SetBtMrvlMode2Config(int		BTCoexistenceMode,	 
 									int	NonPeriodicMode_Enable, 
									int NonPeriodicMode_BtTimerTlead,
									int NonPeriodicMode_BtTimerTtail,
									int PeriodicMode_BtSyncPeriod,
									int PeriodicMode_BtSyncPulseWidth,
									int PeriodicMode_BtTsyncLead,
									int PeriodicMode_BtSyncMode)
{
	#undef THIS_OID 
	#define THIS_OID	 ((HCI_CMD_GROUP_VENDOR_SPECIFIC<<16)|HCI_CMD_MRVL_BTCOEX_MARVELL_MODE2)
//	#undef THIS_EVENT 
//	#define THIS_EVENT	 ((HCI_EVENT_VENDOR_SPECIFIC<<8)|(HCI_EVENT_CMD_COMPLETE))

	int status = 0;
	Bt_PfwCmd_BtMrvlMode2Config TxBuf={0};
  

	TxBuf.BTCoexistenceMode				= BTCoexistenceMode;	 
  	TxBuf.NonPeriodicMode_Enable		= NonPeriodicMode_Enable;	  
 	TxBuf.NonPeriodicMode_BtTimerTlead	= NonPeriodicMode_BtTimerTlead;	  
 	TxBuf.NonPeriodicMode_BtTimerTtail	= NonPeriodicMode_BtTimerTtail;	  
 	TxBuf.PeriodicMode_BtSyncPeriod		= PeriodicMode_BtSyncPeriod;	  
 	TxBuf.PeriodicMode_BtSyncPulseWidth	= PeriodicMode_BtSyncPulseWidth;	  
 	TxBuf.PeriodicMode_BtTsyncLead		= PeriodicMode_BtTsyncLead;	  
 	TxBuf.PeriodicMode_BtSyncMode		= PeriodicMode_BtSyncMode;	   
 
	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));
	if(status) return status;

 
 	return status;
}

#endif //#ifdef _HCI_PROTOCAL_




int DutSpiApiClssHeader Bt_SpiReadData(DWORD StartAddress, 
									DWORD LenInDw,
 									DWORD *pDataArray)
{
	int status=0, index=0;
//	DWORD Address=0;
 

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT

	#undef		THIS_CMD
	#define		THIS_CMD		(MFG_BT_CMD_SPI_EEPROM)
	#undef		THIS_RSP
	#define		THIS_RSP		BTCMD_RSP(THIS_CMD)

#undef		THIS_ACTION
#define		THIS_ACTION		(HOST_ACT_GET)
	int lenInByte=0;
 	spi_CmdEeprom_t *pTxBuf=NULL; 

	lenInByte = LenInDw*sizeof(DWORD);
    
    pTxBuf  = (spi_CmdEeprom_t*)malloc(sizeof(spi_CmdEeprom_t)+lenInByte); 
	memset (pTxBuf, 0, sizeof(spi_CmdEeprom_t) +lenInByte);

	StuffHeader((void*)pTxBuf, THIS_CMD, THIS_ACTION, 
		sizeof(spi_CmdEeprom_t)+lenInByte); 
   	pTxBuf->signature = SIGNATURE_SPIEEPROM;
   	pTxBuf->sector_erase = 0;

   	pTxBuf->address = StartAddress;
	pTxBuf->lenInByte = lenInByte;

 	status = BtIf_query(THIS_OID, (unsigned char*)pTxBuf, 
					sizeof(spi_CmdEeprom_t)+lenInByte);
 	if (status)
	{
		if (BtIf_GetDebugLogFlag())
			DebugLogRite ("Failed BtIf_query of OID_MRVL_MFG_COMMAND; error 0x%08X  0x%08X\n", 
				status,  BtIf_GetErrorLast());
	}

	status = CheckRspHeader((void*)(pTxBuf), THIS_RSP);
 
	memcpy(pDataArray, pTxBuf->byte, lenInByte);
	EndianSwapeByte(pDataArray, (3+lenInByte)/sizeof(DWORD));
	
	if(pTxBuf) free(pTxBuf);

	return status;
}

int DutSpiApiClssHeader Bt_SpiWriteData(DWORD StartAddress, 
									DWORD LenInDw,
 									DWORD *pDataArray, 
									bool SectorErase)
{
	int status=0, index=0;
	DWORD Address=0;
	


	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT

	#undef		THIS_CMD
	#define		THIS_CMD		(MFG_BT_CMD_SPI_EEPROM)
	#undef		THIS_RSP
	#define		THIS_RSP		BTCMD_RSP(THIS_CMD)

#undef		THIS_ACTION
#define		THIS_ACTION		(HOST_ACT_SET)
	int lenInByte=0;
 	spi_CmdEeprom_t *pTxBuf=NULL; 

	lenInByte = LenInDw*sizeof(DWORD);
    
    pTxBuf  = (spi_CmdEeprom_t*)malloc(sizeof(spi_CmdEeprom_t)+lenInByte); 

	memset (pTxBuf, 0, sizeof(spi_CmdEeprom_t) +lenInByte);

	StuffHeader((void*)pTxBuf, THIS_CMD, THIS_ACTION,
					sizeof(spi_CmdEeprom_t)+lenInByte);
   	pTxBuf->signature = SIGNATURE_SPIEEPROM;
   	pTxBuf->sector_erase = SectorErase;

   	pTxBuf->address = StartAddress;
	pTxBuf->lenInByte = lenInByte;

	memcpy(pTxBuf->byte, pDataArray, lenInByte);

	EndianSwapeByte((DWORD*)(pTxBuf->byte), (3+lenInByte)/sizeof(DWORD));
 
 	status = BtIf_query(THIS_OID, (unsigned char*)pTxBuf, 
					sizeof(spi_CmdEeprom_t)+lenInByte);
 	if (status)
	{
		if (BtIf_GetDebugLogFlag())
			DebugLogRite ("Failed BtIf_query of OID_MRVL_MFG_COMMAND; error 0x%08X  0x%08X\n", 
				status,  BtIf_GetErrorLast());
	}

	status = CheckRspHeader((void*)(pTxBuf), THIS_RSP);
 
	free(pTxBuf);

	return status;
}


int DutBtApiClssHeader Bt_EraseEEPROM(int Sector)
{	
	int status=ERROR_NONE;
	DWORD data[1]={0xFFFFFFFF};
	
	return Bt_SpiWriteData(Sector, 1, data, true);    
}
  
//////////////////// 
//////////////////// 
//////////////////// 




int DutBtApiClssHeader MfgResetBt(void)
{
	int status=0;
 

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT

	#undef		THIS_CMD
	#define		THIS_CMD		(MFG_BT_CMD_HCI_RESETBT)
	#undef		THIS_RSP
	#define		THIS_RSP		BTCMD_RSP(THIS_CMD)

#undef		THIS_ACTION
#define		THIS_ACTION		(HOST_ACT_SET)
 	mfgBt_Cmd_t *pTxBuf=NULL; 

	pTxBuf  = (mfgBt_Cmd_t*)malloc(sizeof(mfgBt_Cmd_t)); 
	memset (pTxBuf, 0, sizeof(mfgBt_Cmd_t));

 
	StuffHeader((void*)pTxBuf, THIS_CMD, THIS_ACTION, 
		sizeof(mfgBt_Cmd_t)); 
 
 	status = BtIf_query(THIS_OID, (unsigned char*)pTxBuf, 
					sizeof(mfgBt_Cmd_t));
 	if (status)
	{
		if (BtIf_GetDebugLogFlag())
			DebugLogRite ("Failed BtIf_query of OID_MRVL_MFG_COMMAND; error 0x%08X  0x%08X\n", 
				status,  BtIf_GetErrorLast());
	}

	status = CheckRspHeader((void*)(pTxBuf), THIS_RSP);
 
	free(pTxBuf);

	return status;
}



int DutBtApiClssHeader MfgGetFwBdAddress(unsigned char *pBdAddress)
{
	int status=0;
 

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT

	#undef		THIS_CMD
	#define		THIS_CMD		(MFG_BT_CMD_HCI_FWBDADDRESS)
	#undef		THIS_RSP
	#define		THIS_RSP		BTCMD_RSP(THIS_CMD)

#undef		THIS_ACTION
#define		THIS_ACTION		(HOST_ACT_GET)
 	mfgBt_CmdBdAddress_t *pTxBuf=NULL; 

	pTxBuf  = (mfgBt_CmdBdAddress_t*)malloc(sizeof(mfgBt_CmdBdAddress_t)); 
	memset (pTxBuf, 0, sizeof(mfgBt_CmdBdAddress_t));

 
	StuffHeader((void*)pTxBuf, THIS_CMD, THIS_ACTION, 
		sizeof(mfgBt_CmdBdAddress_t)); 
 
 	status = BtIf_query(THIS_OID, (unsigned char*)pTxBuf, 
					sizeof(mfgBt_CmdBdAddress_t));
 	if (status)
	{
		if (BtIf_GetDebugLogFlag())
			DebugLogRite ("Failed BtIf_query of OID_MRVL_MFG_COMMAND; error 0x%08X  0x%08X\n", 
				status,  BtIf_GetErrorLast());
	}

	status = CheckRspHeader((void*)(pTxBuf), THIS_RSP);
 
	if(pBdAddress)
		memcpy(pBdAddress, pTxBuf->Address.BdAddress, SIZE_BDADDRESS_INBYTE);

	free(pTxBuf);

	return status;
}




int DutBtApiClssHeader MfgEnterTestMode(void)
{
	int status=0; 

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT

	#undef		THIS_CMD
	#define		THIS_CMD		(MFG_BT_CMD_HCI_TESTMODE)
	#undef		THIS_RSP
	#define		THIS_RSP		BTCMD_RSP(THIS_CMD)

#undef		THIS_ACTION
#define		THIS_ACTION		(HOST_ACT_SET)
 	mfgBt_Cmd_t *pTxBuf=NULL; 

	pTxBuf  = (mfgBt_Cmd_t*)malloc(sizeof(mfgBt_Cmd_t)); 
	memset (pTxBuf, 0, sizeof(mfgBt_Cmd_t));

 
	StuffHeader((void*)pTxBuf, THIS_CMD, THIS_ACTION, 
		sizeof(mfgBt_Cmd_t)); 
 
 
 	status = BtIf_query(THIS_OID, (unsigned char*)pTxBuf, 
					sizeof(mfgBt_Cmd_t));
 	if (status)
	{
		if (BtIf_GetDebugLogFlag())
			DebugLogRite ("Failed BtIf_query of OID_MRVL_MFG_COMMAND; error 0x%08X  0x%08X\n", 
				status,  BtIf_GetErrorLast());
	}

	status = CheckRspHeader((void*)(pTxBuf), THIS_RSP);
 
	free(pTxBuf);
 

	
	return status;
}


int DutBtApiClssHeader MfgPeriodicInquery(int MaxPeriod, int MinPeriod, int InquryLength)
{
	int status=0;
 

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT

	#undef		THIS_CMD
	#define		THIS_CMD		(MFG_BT_CMD_HCI_INQUIRY)
	#undef		THIS_RSP
	#define		THIS_RSP		BTCMD_RSP(THIS_CMD)

#undef		THIS_ACTION
#define		THIS_ACTION		(HOST_ACT_SET)
 	Bt_Mfg_PeriodicInquiry_t *pTxBuf=NULL; 

	pTxBuf  = (Bt_Mfg_PeriodicInquiry_t*)malloc(sizeof(Bt_Mfg_PeriodicInquiry_t)); 
	memset (pTxBuf, 0, sizeof(Bt_Mfg_PeriodicInquiry_t));

	pTxBuf->MaxPeriod = MaxPeriod;
	pTxBuf->MinPeriod = MinPeriod;
	pTxBuf->InquryLength = InquryLength;

	StuffHeader((void*)pTxBuf, THIS_CMD, THIS_ACTION, 
		sizeof(Bt_Mfg_PeriodicInquiry_t)); 
 
 	status = BtIf_query(THIS_OID, (unsigned char*)pTxBuf, 
					sizeof(Bt_Mfg_PeriodicInquiry_t));
 	if (status)
	{
		if (BtIf_GetDebugLogFlag())
			DebugLogRite ("Failed BtIf_query of OID_MRVL_MFG_COMMAND; error 0x%08X  0x%08X\n", 
				status,  BtIf_GetErrorLast());
	}

	status = CheckRspHeader((void*)(pTxBuf), THIS_RSP);
 
	free(pTxBuf);

	return status;
}


int DutBtApiClssHeader MfgExitPeriodicInquery(void)
{
	int status=0;
 

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT

	#undef		THIS_CMD
	#define		THIS_CMD		(MFG_BT_CMD_HCI_INQUIRY)
	#undef		THIS_RSP
	#define		THIS_RSP		BTCMD_RSP(THIS_CMD)

#undef		THIS_ACTION
#define		THIS_ACTION		(HOST_ACT_GET)
 	mfgBt_Cmd_t *pTxBuf=NULL; 

	pTxBuf  = (mfgBt_Cmd_t*)malloc(sizeof(mfgBt_Cmd_t)); 
	memset (pTxBuf, 0, sizeof(mfgBt_Cmd_t));

 
	StuffHeader((void*)pTxBuf, THIS_CMD, THIS_ACTION, 
		sizeof(mfgBt_Cmd_t)); 
 
 	status = BtIf_query(THIS_OID, (unsigned char*)pTxBuf, 
					sizeof(mfgBt_Cmd_t));
 	if (status)
	{
		if (BtIf_GetDebugLogFlag())
			DebugLogRite ("Failed BtIf_query of OID_MRVL_MFG_COMMAND; error 0x%08X  0x%08X\n", 
				status,  BtIf_GetErrorLast());
	}

	status = CheckRspHeader((void*)(pTxBuf), THIS_RSP);
 
	free(pTxBuf);

	return status;
}

/*

int DutSpiApiClssHeader MfgGetBdAddressFromFw(char *pBdAddress)
{
	int status=0; 

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT

	#undef		THIS_CMD
	#define		THIS_CMD		(MFG_BT_CMD_HCI_BDADDRESS)
	#undef		THIS_RSP
	#define		THIS_RSP		BTCMD_RSP(THIS_CMD)

#undef		THIS_ACTION
#define		THIS_ACTION		(HOST_ACT_GET)
 	Bt_Mfg_BdAddress_t *pTxBuf=NULL; 

	pTxBuf  = (Bt_Mfg_BdAddress_t*)malloc(sizeof(Bt_Mfg_BdAddress_t)); 
	memset (pTxBuf, 0, sizeof(Bt_Mfg_BdAddress_t));

 
	StuffHeader((void*)pTxBuf, THIS_CMD, THIS_ACTION, 
		sizeof(Bt_Mfg_BdAddress_t)); 
 
  	status = BtIf_query(THIS_OID, (unsigned char*)pTxBuf, 
					sizeof(Bt_Mfg_BdAddress_t));
 	if (status)
	{
		if (BtIf_GetDebugLogFlag())
			DebugLogRite ("Failed BtIf_query of OID_MRVL_MFG_COMMAND; error 0x%08X  0x%08X\n", 
				status,  BtIf_GetErrorLast());
	}

	status = CheckRspHeader((void*)(pTxBuf), THIS_RSP);
 
	memcpy(pBdAddress, pTxBuf->BdAddress,SIZE_BDADDRESS_INBYTE);
	free(pTxBuf);

	return status;
}



 
int DutSpiApiClssHeader MfgSetBdAddressInFw(char *pBdAddress)
{
	int status=0; 

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT

	#undef		THIS_CMD
	#define		THIS_CMD		(MFG_BT_CMD_HCI_BDADDRESS)
	#undef		THIS_RSP
	#define		THIS_RSP		BTCMD_RSP(THIS_CMD)

#undef		THIS_ACTION
#define		THIS_ACTION		(HOST_ACT_SET)
 	Bt_Mfg_BdAddress_t *pTxBuf=NULL; 

	pTxBuf  = (Bt_Mfg_BdAddress_t*)malloc(sizeof(Bt_Mfg_BdAddress_t)); 
	memset (pTxBuf, 0, sizeof(Bt_Mfg_BdAddress_t));

 
	StuffHeader((void*)pTxBuf, THIS_CMD, THIS_ACTION, 
		sizeof(Bt_Mfg_BdAddress_t)); 
 
	memcpy(pTxBuf->BdAddress,pBdAddress,SIZE_BDADDRESS_INBYTE);

 	status = BtIf_query(THIS_OID, (unsigned char*)pTxBuf, 
					sizeof(Bt_Mfg_BdAddress_t));
 	if (status)
	{
		if (BtIf_GetDebugLogFlag())
			DebugLogRite ("Failed BtIf_query of OID_MRVL_MFG_COMMAND; error 0x%08X  0x%08X\n", 
				status,  BtIf_GetErrorLast());
	}

	status = CheckRspHeader((void*)(pTxBuf), THIS_RSP);
 
	free(pTxBuf);

	return status;
}
*/





int DutSpiApiClssHeader MfgMrvlTxTest2(bool enable,	
									  int PacketType, 
									  int PayLoadPattern, 
									  int PayloadLength,
									  int RandomHopping,
                                      int Interval,
									  int Whitening, 
										int TxCnt, char TxPwrLvl)
{
	int status=0; 

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT

	#undef		THIS_CMD
	#define		THIS_CMD		(MFG_BT_CMD_MRVLTXTEST)
	#undef		THIS_RSP
	#define		THIS_RSP		BTCMD_RSP(THIS_CMD)

#undef		THIS_ACTION
#define		THIS_ACTION		(HOST_ACT_SET)
 	Bt_Mfg_TxTest_t *pTxBuf=NULL; 
	Bt_PacketTypeCmd *pPacketType=NULL;
 	int dataRate=4, slotTimer=1, payloadlength=0x1B, testinterval=1;
	int Protocal=0;
 
	pPacketType = (Bt_PacketTypeCmd *)&PacketType;

	//boundary check
	if ( (pPacketType->DataRate) > MAX_BT_DATARATE) 
		pPacketType->DataRate = MAX_BT_DATARATE;

	if ( (pPacketType->DataRate) < MIN_BT_DATARATE) 
		pPacketType->DataRate = MIN_BT_DATARATE;


	if ( (pPacketType->SlotCount) > MAX_BT_HOPSLOT) 
		pPacketType->SlotCount = MAX_BT_HOPSLOT;

	if ( (pPacketType->SlotCount) < MIN_BT_HOPSLOT) 
		pPacketType->SlotCount = MIN_BT_HOPSLOT;


	if ( (PayLoadPattern) > MAX_PAYLOADPATTERN) 
		PayLoadPattern = MAX_PAYLOADPATTERN;

	if ( (PayLoadPattern) < MIN_PAYLOADPATTERN) 
		PayLoadPattern = MIN_PAYLOADPATTERN;

	Protocal = PROTOCAL_DATA_ACL;

	if(PAYLOADPATTERN_CLOSED_LOOP_SCO_PACKETS == PayLoadPattern) 
		Protocal = PROTOCAL_DATA_SCO;
	if(PAYLOADPATTERN_CLOSED_LOOP_ESCO_PACKETS == PayLoadPattern) 
		Protocal = PROTOCAL_DATA_ESCO;


	slotTimer= pPacketType->SlotCount;
	if(!Interval)
	{
		testinterval =(slotTimer+1)/2;
	}
	else
		testinterval = Interval;

	testinterval =(slotTimer+1)/2;

	payloadlength = PayloadLength;

	ClipPayLoadLengthInByte(*((int*)pPacketType),  &payloadlength);

	
	dataRate = DataRateBbValue[Protocal][pPacketType->DataRate][pPacketType->SlotCount];

	if(0 == dataRate && enable) 	
		return ERROR_INPUT_INVALID;


	switch(PayLoadPattern)
	{
		case PAYLOADPATTERN_ALL0:
			PayLoadPattern =PCKTST_PAYLOADPATTERN_ALL0;
			break;
		case PAYLOADPATTERN_ALL1:
			PayLoadPattern =PCKTST_PAYLOADPATTERN_ALL1;
			break;
		case PAYLOADPATTERN_PN9:
			PayLoadPattern =PCKTST_PAYLOADPATTERN_PRBS;
			break;
		case PAYLOADPATTERN_HEX_AA:
			PayLoadPattern =PCKTST_PAYLOADPATTERN_HEX_AA;
			break;
		case PAYLOADPATTERN_HEX_F0:
			PayLoadPattern =PCKTST_PAYLOADPATTERN_HEX_F0;
			break;
		
		
		case PAYLOADPATTERN_LOOPBACK_ACL:
			PayLoadPattern =PCKTST_LOOPBACK_ACL;
			break;
		case PAYLOADPATTERN_CLOSED_LOOP_SCO_PACKETS:
		case PAYLOADPATTERN_CLOSED_LOOP_ESCO_PACKETS:
			PayLoadPattern =PCKTST_CLOSED_LOOP_SYNCHRONOUS_PACKETS;
			break;		
		default: 
			return ERROR_INPUT_INVALID;
			break;
	}

	pTxBuf  = (Bt_Mfg_TxTest_t*)malloc(sizeof(Bt_Mfg_TxTest_t)); 
	memset (pTxBuf, 0, sizeof(Bt_Mfg_TxTest_t));

	pTxBuf->Enable = enable;	
	pTxBuf->PayloadPattern = PayLoadPattern; //PayLoadPattern; 
	pTxBuf->RandomHopMode=RandomHopping;

	pTxBuf->TestInterval	= testinterval;
	pTxBuf->PacketType		= dataRate; // PacketType;
	pTxBuf->PayloadLength	= payloadlength; //PayloadLength;
 
	StuffHeader((void*)pTxBuf, THIS_CMD, THIS_ACTION, 
		sizeof(Bt_Mfg_TxTest_t)); 
 
	pTxBuf->Whitening = Whitening;

	pTxBuf->TxCnt		= TxCnt;
	pTxBuf->TxPwrLvl	= TxPwrLvl;

 	status = BtIf_query(THIS_OID, (unsigned char*)pTxBuf, 
					sizeof(Bt_Mfg_TxTest_t));
 	if (status)
	{
		if (BtIf_GetDebugLogFlag())
			DebugLogRite ("Failed BtIf_query of OID_MRVL_MFG_COMMAND; error 0x%08X  0x%08X\n", 
				status,  BtIf_GetErrorLast());
	}

	status = CheckRspHeader((void*)(pTxBuf), THIS_RSP);
 
	free(pTxBuf);

	return status;
}



 
int DutSpiApiClssHeader MfgMrvlRxTest(	int RxChannel,
										int PacketNumber,
										int PacketType, 
										int PayLoadPattern, 
										int PayloadLength, 
			//							char *pPRBS,	
										char *pTxBdAddress)
{
	int status=0; 

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT

	#undef		THIS_CMD
	#define		THIS_CMD		(MFG_BT_CMD_MRVLRXTEST)
	#undef		THIS_RSP
	#define		THIS_RSP		BTCMD_RSP(THIS_CMD)

#undef		THIS_ACTION
#define		THIS_ACTION		(HOST_ACT_SET)
 	Bt_MfgCmd_RxTest_t *pTxBuf=NULL; 

	Bt_PacketTypeCmd *pPacketType=NULL;
 	int dataRate=4, slotTimer=1, payloadlength=0x1B, testinterval=1;

#ifdef _BTRX_RSSIONLY_
	if (PacketNumber > 10) PacketNumber =10; 
#endif //#ifndef _BTRX_RSSIONLY_

	pPacketType = (Bt_PacketTypeCmd *)&PacketType;

	//boundary check
	if ( (pPacketType->DataRate) > MAX_BT_DATARATE) 
		pPacketType->DataRate = MAX_BT_DATARATE;

	if ( (pPacketType->DataRate) < MIN_BT_DATARATE) 
		pPacketType->DataRate = MIN_BT_DATARATE;


	if ( (pPacketType->SlotCount) > MAX_BT_HOPSLOT) 
		pPacketType->SlotCount = MAX_BT_HOPSLOT;

	if ( (pPacketType->SlotCount) < MIN_BT_HOPSLOT) 
		pPacketType->SlotCount = MIN_BT_HOPSLOT;


	if ( (PayLoadPattern) > MAX_PAYLOADPATTERN && (PayLoadPattern != PAYLOADPATTERN_ABORT)) 
		PayLoadPattern = MAX_PAYLOADPATTERN;

	if ( (PayLoadPattern) < MIN_PAYLOADPATTERN) 
		PayLoadPattern = MIN_PAYLOADPATTERN;


	slotTimer= pPacketType->SlotCount;
	testinterval =(slotTimer+1)/2;

	payloadlength = PayloadLength;

	ClipPayLoadLengthInByte(*((int*)pPacketType),  &payloadlength);

	switch(pPacketType->DataRate)
	{
		case 0:
		{
			switch(pPacketType->SlotCount)
			{
				case 1: 
					dataRate=0x03; // DM1
					break;
				case 3: 
					dataRate=0x0A; // DM3
					break;
				case 5: 
					dataRate=0x0E; // DM5
					break;
				default: 
					return ERROR_INPUT_INVALID;
					break;
			}
			break;
		}
		case 1:
		{
			switch(pPacketType->SlotCount)
			{
				case 1: 
					dataRate=0x04; // DH1
					break;
				case 3: 
					dataRate=0x0B; // DH3
					break;
				case 5: 
					dataRate=0x0F; // DH5
					break;
				default: 
					return ERROR_INPUT_INVALID;
					break;
			}
			break;
		}
		case 2:
		{
			switch(pPacketType->SlotCount)
			{
				case 1: 
					dataRate=0x14; // 2-DH1
					break;
				case 3: 
					dataRate=0x1A; // 2-DH3
					break;
				case 5: 
					dataRate=0x1E; // 2-DH5
					break;
				default: 
					return ERROR_INPUT_INVALID;
					break;
			}
			break;
		}	
 		case 3: 
		{
			switch(pPacketType->SlotCount)
			{
				case 1: 
					dataRate=0x18; // 3-DH1
					break;
				case 3: 
					dataRate=0x1B; // 3-DH3
					break;
				case 5: 
					dataRate=0x1F; // 3-DH5
					break;
				default: 
					return ERROR_INPUT_INVALID;
					break;
			}
			break;
		}
		default: 
			return ERROR_INPUT_INVALID;
			break;
	}

	switch(PayLoadPattern)
	{
	case PAYLOADPATTERN_ABORT:
			PayLoadPattern=0xFF;
			break;
		case PAYLOADPATTERN_ALL0:
			PayLoadPattern =1;
			break;
		case PAYLOADPATTERN_ALL1:
			PayLoadPattern =2;
			break;
		case PAYLOADPATTERN_PN9:
			PayLoadPattern =4;
			break;
		case PAYLOADPATTERN_HEX_AA:
			PayLoadPattern =3;
			break;
		case PAYLOADPATTERN_HEX_F0:
			PayLoadPattern =9;
			break;
		default: 
			return ERROR_INPUT_INVALID;
			break;
	}

	pTxBuf  = (Bt_MfgCmd_RxTest_t*)malloc(sizeof(Bt_MfgCmd_RxTest_t)); 
	memset (pTxBuf, 0, sizeof(Bt_MfgCmd_RxTest_t));

	pTxBuf->RxChannel = RxChannel;
	pTxBuf->PacketType		= dataRate;
	pTxBuf->PayloadPattern = PayLoadPattern; 
	pTxBuf->PayloadLength	= payloadlength;

	pTxBuf->PacketNumber	= PacketNumber;
	memcpy(pTxBuf->TxBdAddress, pTxBdAddress, SIZE_BDADDRESS_INBYTE);
 
	StuffHeader((void*)pTxBuf, THIS_CMD, THIS_ACTION, 
					sizeof(Bt_MfgCmd_RxTest_t)); 
 

 	status = BtIf_query(THIS_OID, (unsigned char*)pTxBuf, 
								sizeof(Bt_MfgCmd_RxTest_t));
 	if (status)
	{
		if (BtIf_GetDebugLogFlag())
			DebugLogRite ("Failed BtIf_query of OID_MRVL_MFG_COMMAND; error 0x%08X  0x%08X\n", 
				status,  BtIf_GetErrorLast());
	}

	status = CheckRspHeader((void*)(pTxBuf), THIS_RSP);
 
	free(pTxBuf);

	return status;
}

int DutSpiApiClssHeader MfgMrvlRxTestResult(
				BYTE*		Report_Status, 
				DWORD*		Report_TotalPcktCount,
				DWORD*		Report_HecMatchCount,
				DWORD*		Report_HecMatchCrcPckts,
				DWORD*		Report_NoRxCount,
				DWORD*		Report_CrcErrCount,
				DWORD*		Report_SuccessfulCorrelation,
				DWORD*		Report_TotalByteCount,
				DWORD*		Report_BER,
				DWORD*		Report_PER,
				DWORD*		Report_AvgRssi,
				DWORD*		Report_TotalBitsCountExpected=NULL,
				DWORD*		Report_TotalBitsCountError_LostDrop=NULL)
{
	int status=0; 

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT

	#undef		THIS_CMD
	#define		THIS_CMD		(MFG_BT_CMD_MRVLRXTEST)
	#undef		THIS_RSP
	#define		THIS_RSP		BTCMD_RSP(THIS_CMD)

#undef		THIS_ACTION
#define		THIS_ACTION		(HOST_ACT_GET)
 	Bt_MfgRsp_RxTest_t *pRxBuf=NULL; 
 
	pRxBuf  = (Bt_MfgRsp_RxTest_t*)malloc(sizeof(Bt_MfgRsp_RxTest_t)); 
	memset (pRxBuf, 0, sizeof(Bt_MfgRsp_RxTest_t));

	StuffHeader((void*)pRxBuf, THIS_CMD, THIS_ACTION, 
		sizeof(Bt_MfgRsp_RxTest_t)); 
 
 	status = BtIf_query(THIS_OID, (unsigned char*)pRxBuf, 
					sizeof(Bt_MfgRsp_RxTest_t));
 	if (status)
	{
		if (BtIf_GetDebugLogFlag())
			DebugLogRite ("Failed BtIf_query of OID_MRVL_MFG_COMMAND; error 0x%08X  0x%08X\n", 
				status,  BtIf_GetErrorLast());
	}

	status = CheckRspHeader((void*)pRxBuf, THIS_RSP);
 

	if (BtIf_GetDebugLogFlag())
	{
		DebugLogRite ("Status_Complete0_Abort1 0x%08X\n",	pRxBuf->Status_Complete0_Abort1);
		DebugLogRite ("TotalPcktCountExpected 0x%08X\n",	pRxBuf->TotalPcktCountExpected);
		DebugLogRite ("NoRxCount 0x%08X\n",	pRxBuf->NoRxCount);
		DebugLogRite ("SuccessfulCorrelation 0x%08X\n",	pRxBuf->SuccessfulCorrelation);
		DebugLogRite ("HecMatchCount 0x%08X\n", pRxBuf->HecMatchCount);
		DebugLogRite ("HecMatchCrcPckts 0x%08X\n", pRxBuf->HecMatchCrcPckts);
		DebugLogRite ("PayLoadHdrErrorCount 0x%08X\n", pRxBuf->PayLoadHdrErrorCount);
		DebugLogRite ("CrcErrCount 0x%08X\n", pRxBuf->CrcErrCount);
		DebugLogRite ("TotalPacketRxedCount 0x%08X\n",	pRxBuf->TotalPacketRxedCount);
		DebugLogRite ("PacketOkCount 0x%08X\n", pRxBuf->PacketOkCount);
		DebugLogRite ("DroppedPacketCount 0x%08X\n", pRxBuf->DroppedPacketCount);
		DebugLogRite ("PER 0x%08X\n", pRxBuf->PER);
		DebugLogRite ("TotalBitsCountExpected 0x%08X\n", pRxBuf->TotalBitsCountExpected);
		DebugLogRite ("TotalBitsCountError_LostDrop 0x%08X\n", pRxBuf->TotalBitsCountError_LostDrop);
		DebugLogRite ("BER 0x%08X\n", pRxBuf->BER);
		DebugLogRite ("TotalByteCountRxed 0x%08X\n", pRxBuf->TotalByteCountRxed);
		DebugLogRite ("TotalBitErrorRxed 0x%08X\n", pRxBuf->TotalBitErrorRxed);
		DebugLogRite ("RSSI 0x%08X\n", pRxBuf->RSSI); 
	}

	if(Report_Status) 
		(*Report_Status) = pRxBuf->Status_Complete0_Abort1;

	if(Report_TotalPcktCount)
		(*Report_TotalPcktCount) = pRxBuf->TotalPcktCountExpected;

	if(Report_HecMatchCount)
		(*Report_HecMatchCount) = pRxBuf->HecMatchCount;

	if(Report_HecMatchCrcPckts) 
		(*Report_HecMatchCrcPckts) = pRxBuf->HecMatchCrcPckts;

	if(Report_NoRxCount) 
		(*Report_NoRxCount) = pRxBuf->NoRxCount;

	if(Report_CrcErrCount) 
		*Report_CrcErrCount			= pRxBuf->CrcErrCount;

	if(Report_SuccessfulCorrelation) 
		(*Report_SuccessfulCorrelation) = pRxBuf->SuccessfulCorrelation;

	if(Report_TotalByteCount) 
		(*Report_TotalByteCount) = pRxBuf->TotalByteCountRxed;

	if(Report_PER) 
		(*Report_PER) = pRxBuf->PER;

	if(Report_BER) 
		(*Report_BER) = pRxBuf->BER;

	if(Report_AvgRssi) (*Report_AvgRssi) = pRxBuf->RSSI;

	if(Report_TotalBitsCountExpected)
		*Report_TotalBitsCountExpected = pRxBuf->TotalBitsCountExpected;

	if(Report_TotalBitsCountError_LostDrop)
		*Report_TotalBitsCountError_LostDrop = pRxBuf->TotalBitsCountError_LostDrop;

	free(pRxBuf);

	return status;
}

int DutSpiApiClssHeader MfgMrvlRssiClear(int LinkId)
{
	int status=0; 

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT

	#undef		THIS_CMD
	#define		THIS_CMD		(MFG_BT_CMD_MRVLRSSI)
	#undef		THIS_RSP
	#define		THIS_RSP		BTCMD_RSP(THIS_CMD)

#undef		THIS_ACTION
#define		THIS_ACTION		(HOST_ACT_SET)
 	Bt_Mfg_RssiRd_t *pTxBuf=NULL; 

	pTxBuf  = (Bt_Mfg_RssiRd_t*)malloc(sizeof(Bt_Mfg_RssiRd_t));  
	memset (pTxBuf, 0, sizeof(Bt_Mfg_RssiRd_t));

 
	StuffHeader((void*)pTxBuf, THIS_CMD, THIS_ACTION, 
		sizeof(Bt_Mfg_RssiRd_t)); 
 
 	pTxBuf->LinkId =LinkId; 
	status = BtIf_query(THIS_OID, (unsigned char*)pTxBuf, 
					sizeof(Bt_Mfg_RssiRd_t));
 	if (status)
	{
		if (BtIf_GetDebugLogFlag())
			DebugLogRite ("Failed BtIf_query of OID_MRVL_MFG_COMMAND; error 0x%08X  0x%08X\n", 
				status,  BtIf_GetErrorLast());
	}

	status = CheckRspHeader((void*)(pTxBuf), THIS_RSP);
 
	free(pTxBuf);

	return status;
}

										
int DutBtApiClssHeader MfgMrvlRssiRpt(int LinkId, 
									   int *pRssiAvg, int *pRssiLast)
{
	int status=0; 

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT

	#undef		THIS_CMD
	#define		THIS_CMD		(MFG_BT_CMD_MRVLRSSI)
	#undef		THIS_RSP
	#define		THIS_RSP		BTCMD_RSP(THIS_CMD)

#undef		THIS_ACTION
#define		THIS_ACTION		(HOST_ACT_GET)
 	Bt_Mfg_RssiRd_t *pTxBuf=NULL; 

     pTxBuf  = (Bt_Mfg_RssiRd_t*)malloc(sizeof(Bt_Mfg_RssiRd_t)); 

	 memset (pTxBuf, 0, sizeof(Bt_Mfg_RssiRd_t));

 
	StuffHeader((void*)pTxBuf, THIS_CMD, THIS_ACTION, 
		sizeof(Bt_Mfg_RssiRd_t)); 
 
	
	pTxBuf->LinkId =LinkId; 
 	status = BtIf_query(THIS_OID, (unsigned char*)pTxBuf, 
					sizeof(Bt_Mfg_RssiRd_t));
 	if (status)
	{
		if (BtIf_GetDebugLogFlag())
			DebugLogRite ("Failed BtIf_query of OID_MRVL_MFG_COMMAND; error 0x%08X  0x%08X\n", 
				status,  BtIf_GetErrorLast());
	}

	status = CheckRspHeader((void*)(pTxBuf), THIS_RSP);
 
	if(pRssiAvg) (*pRssiAvg) = pTxBuf->RssiAvg;
	if(pRssiLast) (*pRssiLast) = pTxBuf->RssiLast;

	free(pTxBuf);

	return status;
}

										

				
int DutBtApiClssHeader MfgPfwReloadCalfromFlash(void)
{
	int status=0; 

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT

	#undef		THIS_CMD
	#define		THIS_CMD		(MFG_BT_CMD_PFW_RELOADCAL)
	#undef		THIS_RSP
	#define		THIS_RSP		BTCMD_RSP(THIS_CMD)

#undef		THIS_ACTION
#define		THIS_ACTION		(HOST_ACT_SET)
 	mfgBt_Cmd_t *pTxBuf=NULL; 

     pTxBuf  = (mfgBt_Cmd_t*)malloc(sizeof(mfgBt_Cmd_t)); 

	 memset (pTxBuf, 0, sizeof(mfgBt_Cmd_t));

 
	StuffHeader((void*)pTxBuf, THIS_CMD, THIS_ACTION, 
		sizeof(mfgBt_Cmd_t)); 
 
 	status = BtIf_query(THIS_OID, (unsigned char*)pTxBuf, 
					sizeof(mfgBt_Cmd_t));
 	if (status)
	{
		if (BtIf_GetDebugLogFlag())
			DebugLogRite ("Failed query of OID_MRVL_MFG_COMMAND; error 0x%08X  0x%08X\n", 
				status,  BtIf_GetErrorLast());
	}

	status = CheckRspHeader((void*)(pTxBuf), THIS_RSP);
 
	free(pTxBuf);

	return status;
}

						

// IfType 0: force detection, 1: spi, 2:i2c

int DutBtApiClssHeader Bt_ForceE2PromType(DWORD		IfType, 
										  DWORD		AddrWidth,
										  DWORD		DeviceType)
{
	int status=0;

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_BT_CMD_E2PROM_TYPE
	#undef THIS_RSP 
	#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_SET

	mfgBt_CmdE2PType_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf...
	TxBuf.IfType		= IfType;
	if(-1 == AddrWidth)
	{	
		if (1 == IfType) 
			TxBuf.AddrWidth		= 2;
		else if (2 == IfType) 
			TxBuf.AddrWidth		= 1;
		else 
			TxBuf.AddrWidth		= 2;
	}
	else
		TxBuf.AddrWidth		= AddrWidth;

	if(-1 == DeviceType)
	{
		if(3 == AddrWidth) 
			TxBuf.DeviceType =2;
		else 
			TxBuf.DeviceType =1;
	}
	else
		TxBuf.DeviceType	= DeviceType;

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);

	return status;
}


int DutBtApiClssHeader Bt_GetE2PromType(DWORD		*pIfType, 
										DWORD		*pAddrWidth,
										DWORD		*pDeviceType)
{
	int status=0;

	#undef THIS_OID 
	#define THIS_OID	 MRVL_HCI_OID_MFG_BT
	#undef THIS_CMD 
	#define THIS_CMD	 MFG_BT_CMD_E2PROM_TYPE
	#undef THIS_RSP 
	#define THIS_RSP	 BTCMD_RSP(THIS_CMD)
	#undef THIS_ACTION 
	#define THIS_ACTION	 HOST_ACT_GET

	mfgBt_CmdE2PType_t TxBuf={0};
 
	StuffHeader((void*)&TxBuf, THIS_CMD, THIS_ACTION,sizeof(TxBuf));

	// set Set values
	// TxBuf...

 	status = BtIf_query(THIS_OID, (unsigned char*)&TxBuf, sizeof(TxBuf));

	if(status) return status;

	status = CheckRspHeader((void*)&TxBuf, THIS_RSP);

	if (pIfType) (*pIfType) = TxBuf.IfType;
	if (pAddrWidth) (*pAddrWidth) = TxBuf.AddrWidth;
	if (pDeviceType) (*pDeviceType) = TxBuf.DeviceType;

	return status;
}
 
