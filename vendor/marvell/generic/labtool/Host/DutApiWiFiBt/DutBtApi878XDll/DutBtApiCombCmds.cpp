/** @file DutBtApiCombCmds.cpp
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#ifdef _HCI_PROTOCAL_
#include GRUCLASS_BT_H_PATH		//	"../GruBtApi878xDll/GruBtApiClss.h"

int DutBtApiClssHeader StartAclConnection(	WORD	*hMasterConnection, 
											 void *pSlave,  
											 WORD	*hSlaveConnection )
{

#if defined (_IF_UDP_) | defined(_IF_UART_)
	return 0;
#else
#define MAXCNT_INQUIRY		(50)
#define MAXCNT_LINK			(50)
	int status=0;
	int NumResp=0, Found=0;
	int MaxInquiryCnt=0, MaxLinkCnt=0;

 
	BYTE	partnerBdAddress[SIZE_BDADDRESS_INBYTE]={0};
	BYTE	PartnerPageScanRep=0;
	WORD	ClockOffset=0;
	

	if(!pSlave)
	{
		char line[20]="";

		DebugLogRite ("Please perform on partner: \n\t 1> BT_RESET \n\t2> Put device discoverable\n");
		DebugLogRite ("press [Enter] when Done \n");
		fgets(line, sizeof(line), stdin);

	}


	MaxLinkCnt=0;

//	do{
//	MaxInquiryCnt=0;

	do{
	
	status = ResetBt();
	if(status) goto ERROR2STOP;

	status = SetDeviceDiscoverable();
	if(status) goto ERROR2STOP;

//	status = SetBtXtal(0x3F);
//	if(status) goto ERROR2STOP;

//	status = pMaster->SetFlowControlEnable(1);
//	if(status) goto ERROR2STOP;

	if(pSlave)
	{
	status = ((DutBtApiClss *)pSlave)->ResetBt();
	if(status) goto ERROR2STOP;

	status = ((DutBtApiClss *)pSlave)->SetDeviceDiscoverable();
	if(status) goto ERROR2STOP;

	status = ((DutBtApiClss *)pSlave)->EnableDeviceUnderTestMode();
	if(status) goto ERROR2STOP;

//	status = ((DutBtApiClss *)pSlave)->SetFlowControlEnable(1);
//	if(status) goto ERROR2STOP;
 
	status = ((DutBtApiClss *)pSlave)->GetBDAddress(partnerBdAddress);
	if(status) goto ERROR2STOP;
	}
 

 
	SetTimeOut(50000);
 
//	NumResp = 10;

// Found =1;
	status = Inquiry(partnerBdAddress, &NumResp, &Found, 
 									&PartnerPageScanRep, &ClockOffset);
	if(status) goto ERROR2STOP;		//return status;

 	if(GetDebugLogFlag())
		DebugLogRite ("NumResp %d-%d (%d 0x%04X)\n", NumResp, Found, PartnerPageScanRep, ClockOffset);
 	else
		DebugLogRite ("I%d ",Found);

	

	}while (!Found && ++MaxInquiryCnt <MAXCNT_INQUIRY);

	if(!Found)
	{
		status = ERROR_LINK_NOTFOUND;
		goto ERROR2STOP;
	}

//PartnerPageScanRep=0;
// ClockOffset=0;
 
	do{
	MaxInquiryCnt=0;
	status = CreateAclConnect(partnerBdAddress,
					PartnerPageScanRep, ClockOffset,
					hMasterConnection);
 
	
	if(GetDebugLogFlag())
		DebugLogRite ("hMasterConnection %d (status %d)\t", *hMasterConnection, status);
	else
		DebugLogRite ("h%d ",status);

	}while (!(*hMasterConnection) && ++MaxLinkCnt <MAXCNT_LINK);

	DebugLogRite ("\n");

	if(!(*hMasterConnection) ) 
	{
		status = ERROR_LINK_NOLINK;
		goto ERROR2STOP;
	}

	if(status) goto ERROR2STOP;		// return status;

//	wait for Slave's connection report event
	if(pSlave && hSlaveConnection)
	{
//		WORD	hSlaveConnection=0;
	  	HCI_EVENT_STRUCT *pHciEvent=NULL;
		unsigned char Buffer[2048]={0};
	
		pHciEvent = (PHCI_EVENT_STRUCT)Buffer;

		while (!(*hSlaveConnection))
		{
 			if (((DutBtApiClss *)pSlave)->RxData(Buffer, 2048)!=ERROR_SUCCESS)
			{
				continue;
			}
 			
			if (pHciEvent->EventCode == HCI_EVENT_CONN_COMPLETE)
				(*hSlaveConnection) = pHciEvent->ConnComplete.BtConn.ConnHandle;

			if(GetDebugLogFlag())
				DebugLogRite ("hSlaveConnection %d\n", (*hSlaveConnection));

		}
 	}



	ResetTimeOut();

 	return status;

ERROR2STOP:
	ResetTimeOut();
 
	return status;

#endif //#ifndef _IF_UDP_

}


int DutBtApiClssHeader StopAclConnection(WORD	*hMasterConnection)
{
	int status=0;

	if(hMasterConnection && *hMasterConnection) 
	{
		status = DisConnect(*hMasterConnection);
		*hMasterConnection=0;
	}

	 return status;
}



int DutBtApiClssHeader Test_Hold(int modeOnOff, 
							  WORD HoldMaxInterval, 
							  WORD HoldMinInterval,
							  bool partnerAutoCtrl)
{
	int status=0;
	static	WORD hConnection=0, hConnSlave=0;
	GruBtApiClss	*pGru=NULL, GruObj;

	if(partnerAutoCtrl)
	{
		status = GruObj.OpenDevice();
		if(status==ERROR_GET_EVENT_TO) 
		{
#if defined (_IF_SDIO_) ||   defined (_IF_USB_)
			GruObj.CloseDevice();
#endif
		}
		if(status) return status;
		pGru = &GruObj;
	}

	if(modeOnOff)
	{
		WORD mode0=0, mode1=0, mode2=0, mode3=0;

		DebugLogRite ("Start ACL connection.\n");
		status = StartAclConnection( &hConnection, pGru, &hConnSlave);
		if(status) goto ERROR_HANDLE;

 		DebugLogRite ("Write LP.\n");

		status = WriteLinkPolice(hConnection,  0,1,0,0);
		if(status) goto ERROR_HANDLE;

 
		if(partnerAutoCtrl)
		{
			status = GruObj.WriteLinkPolice(hConnection,  0,1,0,0);
			if(status) goto ERROR_HANDLE;
		}
		else
		{
			char line[20]="";
			
			DebugLogRite ("Please enable HOLD mode on partner.\n");
			DebugLogRite ("press [Enter] when Done \n");
			fgets(line, sizeof(line), stdin);

		}

 		DebugLogRite ("Enter Hold mode.\n");
		status = Hold(hConnection,  HoldMaxInterval, HoldMinInterval,1);
		if(status) goto ERROR_HANDLE;
 

	}
	else
	{

  		DebugLogRite ("Stop ACL connection.\n");

// 		status = WriteLinkPolice(hConnection,  0,0,0,0);
//		if(status) goto ERROR_HANDLE;

		status = StopAclConnection(&hConnection);
		if(status) goto ERROR_HANDLE;
	
		ResetBt();
	
		if(partnerAutoCtrl && pGru)
		{
	//		Sleep(10000);
			pGru->ResetBt();
		}

	}

	if(partnerAutoCtrl && pGru)
	{
#if defined (_IF_SDIO_) ||   defined (_IF_USB_)
 		pGru->CloseDevice();
#endif
	}

	return status;
ERROR_HANDLE:
	if(hConnection)
		StopAclConnection(&hConnection);
 
	ResetBt();
	if(partnerAutoCtrl && pGru)
	{
		pGru->ResetBt();
#ifdef _IF_SDIO_
		pGru->CloseDevice();
#endif
	}
	return status;

}


int DutBtApiClssHeader Test_Park(int modeOnOff, 
							  WORD BeaconMaxInterval, 
							  WORD BeaconMinInterval,
							  bool partnerAutoCtrl)
{
	int status=0;
	static	WORD hConnection=0, hConnSlave=0;
	GruBtApiClss	*pGru=NULL, GruObj;

	if(partnerAutoCtrl)
	{
		status = GruObj.OpenDevice();
		if(status==ERROR_GET_EVENT_TO) 
		{
#ifdef	 _IF_SDIO_
			GruObj.CloseDevice();
#endif
		}
		if(status) return status;
		pGru = &GruObj;
	}
	if(modeOnOff)
	{
		DebugLogRite ("Start ACL connection.\n");

		status = StartAclConnection( &hConnection, pGru, &hConnSlave);
		if(status) goto ERROR_HANDLE;

  		DebugLogRite ("Write LP.\n");

		status = WriteLinkPolice(hConnection,  0,0,0,1);
		if(status) goto ERROR_HANDLE;

 
		if(partnerAutoCtrl)
		{
			status = GruObj.WriteLinkPolice(hConnection,  0,0,0,1);
			if(status) goto ERROR_HANDLE;
		}
		else
		{
			char line[20]="";
			
			DebugLogRite ("Please enable PARK mode on partner.\n");
			DebugLogRite ("press [Enter] when Done \n");
			fgets(line, sizeof(line), stdin);

		}

		
 		DebugLogRite ("Enter Park mode.\n");
		status = Park(hConnection,  BeaconMaxInterval, BeaconMinInterval);
		if(status) goto ERROR_HANDLE;
	}
	else
	{
 		DebugLogRite ("Exit Park mode.\n");

		status = ExitPark(hConnection);
		if(status) goto ERROR_HANDLE;

// 		status = WriteLinkPolice(hConnection,  0,0,0,0);
//		if(status) goto ERROR_HANDLE;

 		DebugLogRite ("Stop ACL connection.\n");

		status = StopAclConnection(&hConnection);
		if(status) goto ERROR_HANDLE;
	}

	if(partnerAutoCtrl && pGru)
	{
#ifdef	 _IF_SDIO_
 		pGru->CloseDevice();
#endif
	}

	return status;
ERROR_HANDLE:
	if(hConnection)
		StopAclConnection(&hConnection);

	ResetBt();
	if(partnerAutoCtrl && pGru)
	{
		pGru->ResetBt();
#ifdef	 _IF_SDIO_
		pGru->CloseDevice();
#endif
	}
	return status;

}



int DutBtApiClssHeader Test_Sniff(int modeOnOff, 
								WORD SniffMaxInterval,	
								WORD SniffMinInterval,
								WORD SniffAttempt,		
								WORD SniffTimeout,
								bool partnerAutoCtrl)
{
	int status=0;
	static WORD hConnection=0, hConnSlave=0;
	GruBtApiClss	*pGru=NULL, GruObj;

	if(partnerAutoCtrl)
	{
		status = GruObj.OpenDevice();
		if(status==ERROR_GET_EVENT_TO) 
		{
#ifdef	 _IF_SDIO_
			GruObj.CloseDevice();
#endif
		}
		if(status) return status;
		pGru = &GruObj;
	}
	if(modeOnOff)
	{
		DebugLogRite ("Start ACL connection.\n");

		status = StartAclConnection( &hConnection, pGru, &hConnSlave);
		if(status) goto ERROR_HANDLE;

  		DebugLogRite ("Write LP.\n");
		status = WriteLinkPolice(hConnection,  0,0,1,0);
		if(status) goto ERROR_HANDLE;

 
		if(partnerAutoCtrl)
		{
			status = GruObj.WriteLinkPolice(hConnection,  0,0,1,0);
			if(status) goto ERROR_HANDLE;
		}
		else
		{
			char line[20]="";
			
			DebugLogRite ("Please enable SNIFF mode on partner.\n");
			DebugLogRite ("press [Enter] when Done \n");
			fgets(line, sizeof(line), stdin);

		}
		
 		DebugLogRite ("Enter Sniff mode.\n");

		status = Sniff(hConnection,  SniffMaxInterval,	
								SniffMinInterval,
								SniffAttempt,		
								SniffTimeout);
		if(status) goto ERROR_HANDLE;
	}
	else
	{
 		DebugLogRite ("Exit Sniff mode.\n");

		status = ExitSniff(hConnection);
		if(status) goto ERROR_HANDLE;

 //		status = WriteLinkPolice(hConnection,  0,0,0,0);
//		if(status) goto ERROR_HANDLE;

 		DebugLogRite ("Stop ACL connection.\n");

		status = StopAclConnection(&hConnection);
		if(status) goto ERROR_HANDLE;
	}

	if(partnerAutoCtrl && pGru)
	{
#ifdef	 _IF_SDIO_
 		pGru->CloseDevice();
#endif
	}

	return status;
ERROR_HANDLE:
	if(hConnection)
		StopAclConnection(&hConnection);

	ResetBt();
	if(partnerAutoCtrl && pGru)
	{
		pGru->ResetBt();
#ifdef	 _IF_SDIO_
		pGru->CloseDevice();
#endif
	}
	return status;

}

#endif //#ifdef _HCI_PROTOCAL_
