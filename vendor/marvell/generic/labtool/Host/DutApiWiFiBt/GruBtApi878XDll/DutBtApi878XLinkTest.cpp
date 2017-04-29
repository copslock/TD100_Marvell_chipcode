#ifdef _HCI_PROTOCAL_
 
#include "../../DutCommon/Clss_os.h"
#include "../../DutCommon/utility.h"
#include "../../DutCommon/utilities.h"

#include GRUCLASS_BT_H_PATH			//	"../GruBtApi878XDll/GruBtApiClss.h"
#include DUTDLL_VER_H_PATH		//	"../DutApi878XDll/DutDllVerNo.h"	 


#include DUTHCI_BT_LL_STRUCT_H_PATH		//	"../DutBtApi878XDll/HciStruct.h"
#include DUTHCI_BT_LL_OPCODE_H_PATH		//	"../DutBtApi878XDll/hci_opcodes.h"
#include DUTHCI_BT_LL_UARTERR_H_PATH	//	"../DutBtApi878XDll/UartErrCode.h"

//#include "../DutBtApi878XDll/DutApiCombalCmds.cpp"


extern  int maxPayloadInByte[NUM_BT_DATARATE][NUM_BT_HOPSLOT];


/////////////////
#define TX_THREAD_END		(0x1)
#define RX_THREAD_END		(0x2)
#define STOP_TEST_CALLED	(0x80)
#define CID_NUM_ACLLINKTEST	(0xCA53)

int setAbort(int *fAbort)
{
#ifndef _LINUX_	

	char tempC=0;

	if(!fAbort) return 0;

 	if(_kbhit())
	{	
		tempC = _getch();
		
		if((tempC==0x0)||(tempC==0xe0)) //control code
		{	
			tempC = _getch();	
		}
			
			if(tempC==0x1b)
			{
				*fAbort= 1;
			}
	}
#endif		 
		return 0;
}


 

#define FIRST_PAYLOAD_HEADER	2
#define NEXT_PAYLOAD_HEADER		1


int  Test_AclLinkTest(DutBtApiClss *DutObj, 
					  int PacketType, int PacketCount, 
					  int PayloadLengthInByte, int PayloadPattern, 
					  bool MasterAsTx, bool DutAsMaster, 
					  bool stress, 
						int *pRxPacket,
						int *pMissingPacket,
						double *pPer, double*pBer )
{
//#define GRU GruObj->
	int status=0;
//	DutBtApiClss Master, Slave;
	GruBtApiClss Master, Slave;
//	int PacketType=0x11;
//	int PacketCount=100, PayloadLengthInByte=10;
//	int PayloadPattern =PAYLOADPATTERN_PN9;
	BtLinkTestBtApiClss test;
 
//	bool MasterAsTx=1, DutAsMaster=1,  stress=0;
//	int loopBack=0;
	bool TestDone=0;
	double Per=0, Ber=0;

 	DebugLogRite("PacketType 0x%02X\nPacketCount %d\n"
				"PayloadLengthInByte %d\n" 
				"PayloadPattern %d\n", 
				PacketType, PacketCount, 
				PayloadLengthInByte, PayloadPattern);


	status=  test.StartTest(DutObj, &Slave, 
			  PacketType, PacketCount, 
			  PayloadLengthInByte, PayloadPattern, 
			  MasterAsTx, DutAsMaster, 
			  stress);
	
	if(test.flag_UserAbort)  
		DebugLogRite ("User Abort\n");
	else if(test.flag_Abort)  
		DebugLogRite ("Thread Auto Abort\n");



	test.GetTestResult(&TestDone,  pRxPacket, pMissingPacket, pPer, pBer);

	//DebugLogRite ("TestDone %d\t Rx %dPacket(s)\tMissing %dPacket(s)\n"
	//		"PER %lf\tBER %lf\n", 
	//		TestDone, *pRxPacket, *pMissingPacket, 	*pPer, *pBer);

	if(status) 
	{
 		test.StopTest(0);
		return status;
	}



	return status;

}


#ifdef _LINUX_
void*  AclTxThread(void* lpvParam)
#else
DWORD WINAPI AclTxThread(LPVOID lpvParam)
#endif
{
#define MAXWAIT4BUF (5)
	int status = ERROR_NONE;

	BtLinkTestBtApiClss *pTest = (BtLinkTestBtApiClss*) lpvParam;
	
	ACL_DATA_PACKET *pAclData=NULL;
	HCI_EVENT_STRUCT HciEvent={0};
	UCHAR Buffer[2048]={0};
//	ULONG PktType=0;
	USHORT PktSetting=0;
	USHORT MaxLength=0;
	ULONG WaitBugCnt=0, TxCnt = 0, BufIndex=0, ConnIndex=0;
	int tempInt=0;
 
#ifdef _LINUX_
 	setpriority(PRIO_PROCESS, 0, PRIO_MAX);
#endif //#ifdef _LINUX_
	pTest->AvailableBufCnt = pTest->TxMaxBuf;
 	
	PktSetting = pTest->pTxer->getPacketTypeSetting(pTest->TestPacketType);	 
	if (pTest->pTxer->SetConnPacketType(pTest->hConnection, PktSetting)!=ERROR_SUCCESS)
	{
		pTest->flag_Abort = TRUE;
	}

//	pTest->pTxer->SetDataTimeOut(0);

	while (!pTest->flag_TestDone && ((++TxCnt)<=(ULONG)pTest->TestPktCount))
	{ 
			if(pTest->pTxer->GetDebugLogFlag())
				DebugLogRite ("AclTxThread\n");

		if(pTest->flag_Abort) break;

		pAclData = (ACL_DATA_PACKET*)Buffer;
		pAclData->ConnHandle = pTest->hConnection;
		pAclData->Broadcast = 0;  // point to point
		pAclData->PktBdry =  FIRST_PAYLOAD_HEADER; 

//		PktType = pTest->TestPacketType;
 				
// 		PktSetting = pTest->pTxer->getPacketTypeSetting(pTest->TestPacketType);	 

 		tempInt = PAYLOADLENGTH_MAXIMUN_POSSIBLE;

		pTest->pTxer->ClipPayLoadLengthInByte(pTest->TestPacketType, &tempInt);
		MaxLength =tempInt-ACL_HEADER_SIZE; // max data length = max pkt length-4 

//		if(MaxLength>200) MaxLength = 200;

		pAclData->DataLength =  ((pTest->TestPacketLengthInByte>MaxLength)?
									MaxLength:pTest->TestPacketLengthInByte);

 		pAclData->TotalLength = pAclData->DataLength+ACL_HEADER_SIZE;
		pAclData->CID = 0xCA53;


	switch(pTest->TestPayloadPattern)
	{
		case PAYLOADPATTERN_ALL0:
			memset(pTest->DataBuffer[BufIndex], 0x00, 
				sizeof(pTest->DataBuffer[BufIndex]));
			break;
		case PAYLOADPATTERN_ALL1:
			memset(pTest->DataBuffer[BufIndex], 0xFF, 
				sizeof(pTest->DataBuffer[BufIndex]));
			break;
		case PAYLOADPATTERN_PN9:
			for (tempInt=0; tempInt<pAclData->DataLength; tempInt++)
			{
				pTest->DataBuffer[BufIndex][tempInt] = rand();
			}
 			break;
		case PAYLOADPATTERN_HEX_AA:
			memset(pTest->DataBuffer[BufIndex], 0xAA, 
				sizeof(pTest->DataBuffer[BufIndex]));
			break;
		case PAYLOADPATTERN_HEX_F0:
			memset(pTest->DataBuffer[BufIndex], 0xF0, 
				sizeof(pTest->DataBuffer[BufIndex]));
			break;
		default: 
#ifdef _LINUX_
			status = ERROR_INPUT_INVALID;
			return lpvParam;
#else
			return ERROR_INPUT_INVALID;
#endif
 
			break;
	}
 
		memcpy(pAclData->Data, pTest->DataBuffer[BufIndex], 
						pAclData->DataLength);

 		if ((++BufIndex)==pTest->TxMaxBuf)
		{
			BufIndex=0;
		}
	 
		
//		if (pTest->pTxer->SetConnPacketType(pTest->hConnection, PktSetting)!=ERROR_SUCCESS)
//		{
//			pTest->flag_Abort = TRUE;
//			break;
//		}
  
#if defined(_IF_SDIO_) ||  defined(_IF_USB_)
		status = pTest->pTxer->SendAclData((PUCHAR)pAclData, pAclData->TotalLength+4);
 		if(status && pTest->pTxer->GetDebugLogFlag())
			DebugLogRite ("SendAclData ERROR = %d\n", status);

		if(status) break;
		pTest->AvailableBufCnt --;
#endif //#ifndef  _IF_UDP_
 		if(pTest->pTxer->GetDebugLogFlag())
			DebugLogRite ("SendAclData %d (AvailableBufCnt %d)\n", 
							TxCnt, pTest->AvailableBufCnt);
 		else
			DebugLogRite ("*");

		if(pTest->TestStress && (pTest->AvailableBufCnt>0))
		{
				if(pTest->pTxer->GetDebugLogFlag())
					DebugLogRite ("Stress Testing,  AvailableBufCnt %d\n", 
									pTest->AvailableBufCnt);

		}
		else
		{
#ifdef _LINUX_
{
	while ( (pTest->TestStress && pTest->AvailableBufCnt<=0) || 
			(!pTest->TestStress && pTest->AvailableBufCnt<pTest->TxMaxBuf) )
	{	
		Sleep(100*(pTest->TestPacketType&0x0F));
	}
 
} 
#else
			WaitForSingleObject(pTest->hTxBufThread,100);
#endif
		}
 
 
 
	}


// wait for all packet out before anounce done.  
 		while(pTest->AvailableBufCnt != pTest->TxMaxBuf)
		{
			Sleep(0);
		}


	pTest->flag_ReadyToEnd |= TX_THREAD_END;

	if ((pTest->flag_ReadyToEnd & STOP_TEST_CALLED)==0 && 
		(pTest->flag_ReadyToEnd & RX_THREAD_END)!=0)
	{
#ifdef _LINUX_
//		pthread_exit(0);
#else
		ExitThread(0);
#endif
 

		pTest->flag_ReadyToEnd |= STOP_TEST_CALLED;
		pTest->StopTest(TRUE);
	}
#ifdef _LINUX_
return lpvParam;
#else
return 0;
#endif
}

//DWORD WINAPI AclRxThread(LPVOID lpvParam)
#ifdef _LINUX_
void*  AclRxThread(void* lpvParam)
#else
DWORD WINAPI AclRxThread(LPVOID lpvParam)
#endif
{
	BtLinkTestBtApiClss *pTest = (BtLinkTestBtApiClss*) lpvParam;
  	HCI_EVENT_STRUCT *pHciEvent=NULL;
	unsigned char Buffer[2048]={0};
	ULONG Cnt=0, ErrCnt=0, OkCnt=0, ErrPcktCnt=0, OkPcktCnt=0, Count=0, BufIndex=0;
	ULONG ErrBitCnt=0,TotalBitCnt=0, ErrByteCnt=0, TotalByteCnt=0;
//	CString szFile;

// 	pTest->pRxer->SetDataTimeOut(0);

#ifdef _LINUX_
	setpriority(PRIO_PROCESS, 0, PRIO_MIN);
#endif // #ifdef _LINUX_
	
	pHciEvent = (PHCI_EVENT_STRUCT)Buffer;

	while (!pTest->flag_TestDone && (Cnt)<(ULONG)pTest->TestPktCount)
	{
		if(pTest->pRxer->GetDebugLogFlag())
			DebugLogRite ("AclRxThread\n");

		if(pTest->flag_Abort) break;


 	 	memset(Buffer, 0x00, 2048);

#if defined(_IF_SDIO_) ||  defined(_IF_USB_)
		if (pTest->pRxer->RxData(Buffer, 2048)!=ERROR_SUCCESS)
		{
			Sleep(1);
			continue;
		}
#endif //#ifndef  _IF_UDP_
		
		if (pHciEvent->IsDataPkt)
		{

			if ((ULONG)pHciEvent->EventCode==HCI_ACL_DATA_PACKET 
				&& pHciEvent->AclData.ConnHandle == pTest->hConnection &&
				pHciEvent->AclData.CID == CID_NUM_ACLLINKTEST)
			{
			Cnt++;

			if(pTest->pMaster->GetDebugLogFlag())
				DebugLogRite ("found Rx data packet %d\n", Cnt);
			else 
				DebugLogRite (".");

//			DebugLogRite ("Rx 0x%02X -- 0x%02X\t", 
//				pTest->DataBuffer[BufIndex][0], 
//				pHciEvent->AclData.Data[0] );

////////////////
				{ int ErrBit=0, TotalBit=0, ErrByte=0;

	//			if(BufIndex ==0)
	//			{
	//			pHciEvent->AclData.Data[0] |=0x3;
	//			pHciEvent->AclData.Data[2] |=0x3;
	//			}
					pTest->CountError(pTest->DataBuffer[BufIndex], 
						pHciEvent->AclData.Data, pHciEvent->AclData.DataLength, 
						&ErrBit, &TotalBit, &ErrByte);
//			DebugLogRite ("Errors 0x%02X -- %02X -- %02X\n",  
//		ErrBit, TotalBit, ErrByte);

  
				if(ErrByte) 
					ErrPcktCnt ++;
				else
					OkPcktCnt ++;
				
				ErrBitCnt +=ErrBit;
				TotalBitCnt +=TotalBit;
				ErrByteCnt +=ErrByte;
				TotalByteCnt +=pHciEvent->AclData.DataLength;

				if ((++BufIndex)==pTest->TxMaxBuf)
				{
					BufIndex=0;
				}
				
				}
 
 
////////////////////
 

			}		 
		}
 		else if (pHciEvent->EventCode==HCI_EVENT_DISCONNECTION_COMPLETE)
		{
			DebugLogRite (" rx thread HCI_EVENT_DISCONNECTION_COMPLETE\n");

 			pTest->flag_Abort = TRUE;
		}		 

	}	

	DebugLogRite ("\n");
 
	pTest->RxErrorByteCount = ErrByteCnt;
	pTest->RxByteCount=TotalByteCnt;
	pTest->RxErrorBitCount = ErrBitCnt;
	pTest->RxBitCount=TotalBitCnt;
	pTest->RxErrorPacketCount =ErrPcktCnt;
	pTest->RxPacketCount =Cnt;
	
 	pTest->flag_ReadyToEnd |= RX_THREAD_END;
  
	if ((pTest->flag_ReadyToEnd & STOP_TEST_CALLED)==0 
		&& 
		(pTest->flag_ReadyToEnd & TX_THREAD_END)!=0
		)
	{
#ifdef _LINUX_
//		pthread_exit(0);
#else
		ExitThread(0);
#endif
		pTest->flag_ReadyToEnd |= (RX_THREAD_END|STOP_TEST_CALLED);
		pTest->StopTest(TRUE);
	}

  
#ifdef _LINUX_
return lpvParam;
#else
return 0;
#endif
}


//DWORD WINAPI AclTxBufferThread(LPVOID lpvParam)
#ifdef _LINUX_
void*  AclTxBufferThread(void* lpvParam)
#else
DWORD WINAPI AclTxBufferThread(LPVOID lpvParam)
#endif
{
	BtLinkTestBtApiClss *pTest = (BtLinkTestBtApiClss*) lpvParam;

	HCI_EVENT_STRUCT HciEvent={0};
	int status =0, ConnIndex=0;


	while (!pTest->flag_TestDone && !pTest->flag_Abort)
	{ 
		if(pTest->TxMaxBuf != pTest->AvailableBufCnt) 
		{
			if(pTest->pTxer->GetDebugLogFlag())
				DebugLogRite ("AclTxBufferThread\n");
			
			memset(&HciEvent, 0, sizeof(HCI_EVENT_STRUCT));
#if defined(_IF_SDIO_) ||  defined(_IF_USB_)
			status= pTest->pTxer->RxData((PUCHAR)&HciEvent, sizeof(HCI_EVENT_STRUCT));
	//			if(pTest->pTxer->GetDebugLogFlag())
	//				DebugLogRite ("Wait for buffer %d\n", WaitBugCnt);
 
//			if(status)  ;
#endif //#ifndef  _IF_UDP_ 
		if (!status && 
			HciEvent.EventCode == HCI_EVENT_NUMBER_OF_COMPLETED_PACKETS && 
			HciEvent.NumOfCompletePackets.NumberOfHandle >0)
		{
			for(ConnIndex=0; ConnIndex<HciEvent.NumOfCompletePackets.NumberOfHandle; ConnIndex++)
			{
				//	ConnIndex=HciEvent.NumOfCompletePackets.NumberOfHandle-1;

				if(HciEvent.NumOfCompletePackets.Data[ConnIndex].ConnHandle == pTest->hConnection)
				{
					pTest->AvailableBufCnt +=HciEvent.NumOfCompletePackets.Data[ConnIndex].NumOfCompletePackets;
							
//		DebugLogRite ("###########################Event %d\n", HciEvent.NumOfCompletePackets.Data[ConnIndex].NumOfCompletePackets);
							
					if(pTest->pTxer->GetDebugLogFlag())
						DebugLogRite ("Got tx buffer. AvalableBufCnt %d\n", 
						pTest->AvailableBufCnt);
 				
					if(pTest->AvailableBufCnt>0)
						break;
				}
			}
		}
	//	else
	//		Sleep(0);
		}
	}			

#ifdef _LINUX_
return lpvParam;
#else
return 0;
#endif
 
}
 



//#ifndef _LINUX_
BtLinkTestClssHeader BtLinkTestBtApiClss()
{
	TxMaxBuf=0;
	AvailableBufCnt=0; 

	hConnection=(unsigned short)NULL;
	LinkType=0;
	EncryptMode=0;

#ifdef _LINUX_
	hTxThread=(pthread_t)NULL;
	hRxThread=(pthread_t)NULL;
	hTxBufThread =(pthread_t)NULL;
#else //#ifndef _LINUX_
	hTxThread=NULL;
	hRxThread=NULL;
	hTxBufThread =NULL;

#endif //#ifndef _LINUX_ 
	TestPktCount=0;
	TestPacketType=0;
	TestPacketLengthInByte=0;
	TestPayloadPattern=0;
	TestStress=false;

	TestResult_PER=0;
	TestResult_BER=0;

	flag_Started=0;
	flag_Abort=0;
	flag_UserAbort =0;
	flag_ReadyToEnd=0;
	flag_TestDone=0;
	memset(DataBuffer, 0, (HOST_BUFFER_COUNT*2048));


	RxPacketCount=0;
	RxByteCount=0;
	RxBitCount=0;
	RxErrorPacketCount=0;
	RxErrorByteCount=0;
	RxErrorBitCount=0;
}

BtLinkTestClssHeader ~BtLinkTestBtApiClss()
{
}
//#endif	//#ifndef _LINUX_


int BtLinkTestClssHeader Version(void)
{ 
 	return	MFG_VERSION(DUT_DLL_VERSION_MAJOR1, DUT_DLL_VERSION_MAJOR2,
						DUT_DLL_VERSION_MINOR1, DUT_DLL_VERSION_MINOR2);
}

char* BtLinkTestClssHeader About(void)
{
	char tempDate[50]="", tempTime[50]="";
	char AboutFormat[512]=
			"Name:\t\tDutApiClass\n"
#ifdef _UDP_
			"Interface:\t""HCI_UART\n" 
#elif _BT_USB_
			"Interface:\t""USB\n"
#else 
			"Interface:\t""unknown\n"
#endif	
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

 


int BtLinkTestClssHeader StartTest(void* pDutObj, void* pGruObj, 
			  int PacketType, int PacketCount, 
			  int PayloadLengthInByte, int PayloadPattern, 
			  bool MasterAsTx, bool DutAsMaster, 
			  bool stress)
{
	if(flag_Started) return ERROR_ERROR;


//	pMaster = (DutBtApiClss*)pMasterObj;
//	pSlave	= (DutBtApiClss*)pSlaveObj;

 		
	pGru = (GruBtApiClss*) pGruObj;

	if(DutAsMaster)
	{
		pMaster = (DutBtApiClss*)pDutObj;
		pSlave	= (DutBtApiClss*)pGruObj;
	}
	else
	{
		pMaster = (DutBtApiClss*)pGruObj;
		pSlave	= (DutBtApiClss*)pDutObj;
	}

	if(MasterAsTx)
	{
		pTxer = pMaster;
		pRxer = pSlave;
	}
	else
	{
		pTxer = pSlave;
		pRxer = pMaster;
	}


 
	pTxer->ClipPayLoadLengthInByte(PacketType, &PayloadLengthInByte);

	TestPacketType	= PacketType;
	TestPktCount	= PacketCount;
	TestPacketLengthInByte	= PayloadLengthInByte;
	TestPayloadPattern		= PayloadPattern;
	TestStress=stress;

	flag_Started=1;
	flag_Abort=0;
	flag_UserAbort=0;
	flag_ReadyToEnd=0;
	flag_TestDone=0;
	memset(DataBuffer, 0, sizeof(DataBuffer));

	return DoTest();
}



int BtLinkTestClssHeader GetTestResult(bool *pTestDone, 
									   int *pRxPacket,
									   int *pMissingPacket,
									   double * pPer, 
									   double *pBer)
{
//	if(!flag_Started)	return ERROR_ERROR;

	//if(pTestDone) *pTestDone = (bool)flag_TestDone;
    if(pTestDone) *pTestDone = flag_TestDone?true:false;
	
//	if(GetDebugLogFlag())
	{
		DebugLogRite( "\n");
		DebugLogRite( "RxPacketCount:\t\t%12d\n",		RxPacketCount);
  		DebugLogRite( "RxErrorPacketCount:\t%12d\n",	RxErrorPacketCount);
  		DebugLogRite( "RxByteCount:\t\t%12d\n",			RxByteCount);
  		DebugLogRite( "RxErrorByteCount:\t%12d\n",		RxErrorByteCount);
  		DebugLogRite( "RxBitCount:\t\t%12d\n",			RxBitCount);
  		DebugLogRite( "RxErrorBitCount:\t%12d\n",		RxErrorBitCount);
		DebugLogRite( "\n");
	}
	if(pRxPacket)  
		(*pRxPacket) = RxPacketCount;

	if(pMissingPacket)  
		(*pMissingPacket) = flag_TestDone?(TestPktCount - RxPacketCount):0;

	if(pPer && RxPacketCount)		
		*pPer = 1.0*RxErrorPacketCount/RxPacketCount;

	if(pBer && RxBitCount)		
		*pBer = 1.0*RxErrorBitCount/RxBitCount;

	return ERROR_NONE;
}

int BtLinkTestClssHeader DoTest(void)
{
//#define MAXCNT_INQUIRY		(50)
//#define MAXCNT_LINK			(50)
	int status=0;
//	int NumResp=0, Found=0;
//	int MaxInquiryCnt=0, MaxLinkCnt=0;
	WORD	hSlaveConnection=0;

	status = pGru->OpenDevice();
	if(status==ERROR_GET_EVENT_TO) 
	{
#if defined(_IF_SDIO_) ||  defined(_IF_USB_)
		pGru->CloseDevice();
#endif //#ifndef  _IF_UDP_
	}
	if(status) return status;
 
	status = pMaster->StartAclConnection(//pMaster, 
							 &hConnection, 
							pSlave,
							&hSlaveConnection );
	if(status) goto ERROR2STOP;

	if(pSlave == pTxer) 
		hConnection = hSlaveConnection;

	status = pTxer->GetBufferSizeInfo(NULL, &TxMaxBuf, NULL, NULL);
	if(status) return status;
	
	if(HOST_BUFFER_COUNT <TxMaxBuf) 
		TxMaxBuf=HOST_BUFFER_COUNT;
 
 

 /*
	status = pTxer->GetBufferSizeInfo(NULL, &TxMaxBuf, NULL, NULL);
	if(status) return status;
	
	if(HOST_BUFFER_COUNT <TxMaxBuf) 
		TxMaxBuf=HOST_BUFFER_COUNT;


 
	status = pSlave->GetBDAddress(partnerBdAddress);
	if(status) goto ERROR2STOP;
	


	MaxLinkCnt=0;

//	do{
//	MaxInquiryCnt=0;

	do{
	
	status = pMaster->ResetBt();
	if(status) goto ERROR2STOP;

	status = pMaster->SetDeviceDiscoverable();
	if(status) goto ERROR2STOP;

//	status = pMaster->SetFlowControlEnable(1);
//	if(status) goto ERROR2STOP;

	status = pSlave->ResetBt();
	if(status) goto ERROR2STOP;

	status = pSlave->SetDeviceDiscoverable();
	if(status) goto ERROR2STOP;

//	status = pSlave->SetFlowControlEnable(1);
//	if(status) goto ERROR2STOP;

 
///////////////////////

//	pSlave->WriteBtuReg(0x90A, 0x0707);
//	pSlave->WriteBtuReg(0x8BE, 1<<8);

//	pMaster->WriteBtuReg(0x90A, 0x0707);
//	pMaster->WriteBtuReg(0x8BE, 1<<8);
	
	
////////////////////////

	setAbort(&flag_UserAbort);
	if(flag_UserAbort) goto ERROR2STOP;	

	pMaster->SetTimeOut(50000);
 
//	NumResp = 10;

// Found =1;
	status = pMaster->Inquiry(partnerBdAddress, &NumResp, &Found, 
 									&PartnerPageScanRep, &ClockOffset);
	if(status) goto ERROR2STOP;		//return status;

 	if(GetDebugLogFlag())
		DebugLogRite ("NumResp %d-%d (%x %x)\n", NumResp, Found, PartnerPageScanRep, ClockOffset);
 	else
		DebugLogRite ("I%d ",Found);

	

	}while (!Found && ++MaxInquiryCnt <MAXCNT_INQUIRY);

	if(!Found)
	{
		status = ERROR_LINKTEST_NOTFOUND;
		goto ERROR2STOP;
	}

//	{
//		WORD tempW=0;

//		pSlave->ReadBtuReg(0x8BE, &tempW);
//		DebugLogRite ("Slave %04X\t", tempW);

// 		pMaster->ReadBtuReg(0x8BE, &tempW);
//		DebugLogRite ("master %04X\n", tempW);
	
//	}
	do{
	MaxInquiryCnt=0;

 	setAbort(&flag_UserAbort);
	if(flag_UserAbort) goto ERROR2STOP;	

	status = pMaster->CreateAclConnect(partnerBdAddress,
					PartnerPageScanRep, ClockOffset,
					&hConnection);

//	{
//		WORD tempW=0;
//
//		pSlave->ReadBtuReg(0x8BE, &tempW);
//		DebugLogRite ("Slave %04X\t", tempW);
//
//		pMaster->ReadBtuReg(0x8BE, &tempW);
//		DebugLogRite ("master %04X\n", tempW);
//	
//	}
	
	if(GetDebugLogFlag())
		DebugLogRite ("hConnection %d (status %d)\t", hConnection, status);
	else
		DebugLogRite ("h%d ",status);

	}while (!hConnection && ++MaxLinkCnt <MAXCNT_LINK);

	DebugLogRite ("\n");

	if(!hConnection)  
	{
		status = ERROR_LINKTEST_NOLINK;
		goto ERROR2STOP;
	}

	if(status) goto ERROR2STOP;		// return status;

//	wait for Slave's connection report event
	{
		WORD	hSlaveConnection=0;
	  	HCI_EVENT_STRUCT *pHciEvent=NULL;
		unsigned char Buffer[2048]={0};
	
		pHciEvent = (PHCI_EVENT_STRUCT)Buffer;

		while (!hSlaveConnection)
		{
 			if (pSlave->RxData(Buffer, 2048)!=ERROR_SUCCESS)
			{
				continue;
			}
 			
			if (pHciEvent->EventCode == HCI_EVENT_CONN_COMPLETE)
				hSlaveConnection = pHciEvent->ConnComplete.BtConn.ConnHandle;
		}

		if(pSlave == pTxer) 
			hConnection = hSlaveConnection;
	}



	pMaster->ResetTimeOut();

 */

 

// create three threads and each do a job
#ifdef _LINUX_
//	pthread_create(&hTxThread, NULL, AclTxThread, (void*)this);
 	pthread_create(&hRxThread, NULL, AclRxThread, (void*)this);
	pthread_create(&hTxBufThread, NULL, AclTxBufferThread, (void*)this);
	pthread_create(&hTxThread, NULL, AclTxThread, (void*)this);
	
//	pthread_join(hTxThread, NULL);
//	pthread_join(hRxThread, NULL);
//	pthread_join(hTxBufThread, NULL);

#else
	hTxThread = CreateThread(NULL, 0, AclTxThread, this, 0, 0);
	hRxThread = CreateThread(NULL, 0, AclRxThread, this, 0, 0);
	hTxBufThread = CreateThread(NULL, 0, AclTxBufferThread, this, 0, 0);
 	SetThreadPriority(hTxThread, THREAD_PRIORITY_NORMAL);
//	SetThreadPriority(hRxThread, THREAD_PRIORITY_ABOVE_NORMAL);
	SetThreadPriority(hRxThread, THREAD_PRIORITY_NORMAL);
	SetThreadPriority(hTxBufThread, THREAD_PRIORITY_NORMAL);
#endif
//THREAD_PRIORITY_HIGHEST THREAD_PRIORITY_ABOVE_NORMAL THREAD_PRIORITY_NORMAL

	StopTest(1);

status = pMaster->StopAclConnection(&hConnection);
//	status = pMaster->DisConnect(hConnection);

#if defined(_IF_SDIO_) ||  defined(_IF_USB_)
	status = pGru->CloseDevice();
#endif //#ifndef  _IF_UDP_

	flag_TestDone =1;
	flag_Started =0;
 	return status;

ERROR2STOP:

#if defined(_IF_SDIO_) ||  defined(_IF_USB_)
	pMaster->ResetTimeOut();

	pGru->CloseDevice();
#endif //#ifndef  _IF_UDP_
	flag_TestDone =0;
	flag_Started =0;
	return status;
}



void BtLinkTestClssHeader StopTest(BOOL bWaitForThread)
{
	flag_ReadyToEnd |= STOP_TEST_CALLED;
 
	while (bWaitForThread &&
		(!(flag_ReadyToEnd & TX_THREAD_END) || 
		 !(flag_ReadyToEnd & RX_THREAD_END)))
	{
//	printf("Wait for STOP %d\n", flag_ReadyToEnd); 
	
		Sleep(1);
		// check if abort
		setAbort(&flag_UserAbort);
		flag_Abort |= flag_UserAbort;
	}

#ifdef _LINUX_
//	pthread_exit(NULL);
	hRxThread=0;
	hTxThread=0;
	hTxBufThread=0;

#else //#ifdef _LINUX_
	CloseHandle(hRxThread);
	hRxThread=0;

	CloseHandle(hTxThread);
	hTxThread=0;

	CloseHandle(hTxBufThread);
	hTxBufThread=0;
#endif //#ifdef _LINUX_

#if defined(_IF_SDIO_) ||  defined(_IF_USB_)
	pTxer->ResetDataTimeOut();
	pRxer->ResetDataTimeOut();
#endif //#ifndef  _IF_UDP_
}

void BtLinkTestClssHeader CountError(BYTE *pTxData, BYTE *pRxData, int DataLenInByte, 
				int* pErrBit, int* pTotalBit, int* pErrByte)
{
	int		i=0, bitCountErr=0, bitCountTotal=0, ByteCountErr=0;
	BYTE	tempByte=0;

				for (i=0; i<DataLenInByte; i++)
				{
 				tempByte = pTxData[i]^pRxData[i];
 				bitCountErr+=CountBit1(tempByte);
				bitCountTotal +=8; 

					if (tempByte != 0)
					{
 						ByteCountErr++;
				
			//			if(this->pMaster->GetDebugLogFlag())
//							DebugLogRite ("ByteCountErr %d\n", ByteCountErr);
//					break;
					}
				}

		if(pErrBit)		(*pErrBit)		= bitCountErr;
		if(pTotalBit)	(*pTotalBit)	= bitCountTotal;
		if(pErrByte)	(*pErrByte)		= ByteCountErr;

//				if (i==DataLenInByte)
//				{
//					OkCnt++;
//					if(pTest->pMaster->GetDebugLogFlag())
//						DebugLogRite ("Packet OkCnt %d\n", OkCnt);
//				}
////////////////////
}


#endif //#ifdef _HCI_PROTOCAL_

