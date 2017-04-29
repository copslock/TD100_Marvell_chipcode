// GoldenAPTool.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <afx.h>

#include <stdio.h>
#include <conio.h>

#include "../Golden87XXDll.h"
#include "../Golden87XXDll_Bt.hc"
#include "../Golden87XXDll_Wlan.hc"

char line[255]="";


int WifiMenu(void)
{
int cmd=0;
//	int loss=0, tuning=0; 
	int status=0;
 int parm1=0, parm2=0;
	status= Gru_Wlan_GoldenInit();

	if(status ) return status;
  
	{
	 	 

	 
    do
    {
        DebugLogRite ("\n");
        DebugLogRite ("GOLDEN WLAN MAIN MENU\n");
        DebugLogRite ("-------------------\n");
 		DebugLogRite ("10. Select Antenna (0=main, 1=aux)\n");
		DebugLogRite ("12. Set Tx Channel (ch)\n");
 
		DebugLogRite ("17. Start Continuous Tx (enable DataRate)\n");
		DebugLogRite ("18. Start CW Transmit (enable)\n");
		DebugLogRite ("19. Start Carrier Supression (enable)Tx\n");
 		DebugLogRite ("30. Change Band (0=2.4GHz Band, 1 = 5GHz Band).\n");
		DebugLogRite ("31. Soft Reset\n");
		DebugLogRite ("32. Stop Tx PER Test (Rxed Frame Counting)\n");
		DebugLogRite ("33. Tx MultiCast Packet "
			"(0x)(len=400) (0x)(Count=64) (rate=4) \n"
			"\t(pattern=0xAA) (shortPreamble=1) (bssid-xx.xx.xx.xx.xx.xx)\n");
		DebugLogRite ("34. Enable BSSID filter (enable MacAddress (xx.xx.xx.xx.xx.xx) )\n");
		DebugLogRite ("99. Exit\n"); 
        DebugLogRite ("Enter option: ");
        cmd = 0;
        fgets(line, sizeof(line), stdin);
		parm1=0; 
		parm2=0;
        sscanf (line, "%d %x %x",&cmd, &parm1, &parm2);
       switch (cmd)
        {
	  
		case 10:
			Gru_Wlan_SelectTxAntenna(parm1);
            break;
		case 11: // Set Tx Channel
			Gru_Wlan_GetChannel(&parm1, &parm2);
			DebugLogRite("Channel: %d (%d MHz)\n", parm1, parm2);
            break;
		case 12: // Set Tx Channel
			sscanf (line, "%d %d",&cmd, &parm1 );
			Gru_Wlan_SwitchChannel(parm1);
            break;
		case 17: // Set continuous Tx
			sscanf (line, "%d %d %d",&cmd, &parm1, &parm2);
			if(parm2 == 0) parm2=4;
			Gru_Wlan_StartContinuousTx(parm1, parm2-1);
            break;
        case 18: // Set continuous Tx
			Gru_Wlan_StartCWTx(parm1);
            break;
        case 19: // Set continuous Tx
			Gru_Wlan_CarrierSuppression(parm1);
            break;
		case 30:
			Gru_Wlan_SwitchBand(parm1);
 			break;
		case 31:
			Gru_Wlan_SoftReset();
 			break;
		case 32:
			{ 
  				DWORD count=0x0, McCnt=0, errCnt=0; 
				Gru_Wlan_ReadRxPcktCount(&count, &McCnt, &errCnt); 
			
				DebugLogRite("Rx Counts: %d\n", count);
				DebugLogRite("Mc Counts: %d\n", McCnt);
				DebugLogRite("CRC Error Counts: %d\n", errCnt);

			}
			break;
       case 33: // Set continuous Tx
		   { DWORD rate=4, pattern=0xAAAAAAAA, len=0x400, count=100, shortPreamble=1;
				int cnt=0; 
				char Bssid[6]="";
				int tempIntArray[6]={0}; 
 				cnt = sscanf (line, "%d %x %x  %d %d %d %x.%x.%x.%x.%x.%x",
					&cmd, &parm1, &parm2, &rate, &pattern, &shortPreamble,
				&tempIntArray[0], &tempIntArray[1], &tempIntArray[2], 
				&tempIntArray[3], &tempIntArray[4], &tempIntArray[5]);
				if(cnt == 12) 
				{
					Bssid[0] = tempIntArray[0];
					Bssid[1] = tempIntArray[1];
					Bssid[2] = tempIntArray[2]; 
					Bssid[3] = tempIntArray[3];
					Bssid[4] = tempIntArray[4];
					Bssid[5] = tempIntArray[5];
				}
				else
				{
					Bssid[0] = (char)0xFF;
					Bssid[1] = (char)0xFF;
					Bssid[2] = (char)0xFF; 
					Bssid[3] = (char)0xFF;
					Bssid[4] = (char)0xFF;
					Bssid[5] = (char)0xFF;
				}
				len = (0 == parm1) ? 0x400:parm1; 
				count = (0 == parm2) ? 100:parm2; 

			status = Gru_Wlan_TxMulticastFrame(rate-1, pattern, len, count, shortPreamble, Bssid);
			DebugLogRite(" DutCtrl.TxBrdCstPkts: 0x%08X\n", status);
			}
			break;
		case 34: // enable BSSID filter
		   { 
				int cnt=0;
//				char Ssid[50]="";
				char Bssid[6]="";
				int tempIntArray[6]={0}; 
 				cnt = sscanf (line, "%d %x %x.%x.%x.%x.%x.%x",&cmd, &parm1,
				&tempIntArray[0], &tempIntArray[1], &tempIntArray[2], 
				&tempIntArray[3], &tempIntArray[4], &tempIntArray[5]);

				if(cnt == 8) 
				{
					Bssid[0] = tempIntArray[0];
					Bssid[1] = tempIntArray[1];
					Bssid[2] = tempIntArray[2]; 
					Bssid[3] = tempIntArray[3];
					Bssid[4] = tempIntArray[4];
					Bssid[5] = tempIntArray[5];
				}
				else
				{
					Bssid[0] = 0xFF;
					Bssid[1] = 0xFF;
					Bssid[2] = 0xFF; 
					Bssid[3] = 0xFF;
					Bssid[4] = 0xFF;
					Bssid[5] = 0xFF;
				}
			status = Gru_Wlan_BssidFilterMode(parm1, Bssid);
			DebugLogRite("Gru_Wlan_BssidFilterMode: 0x%08X\n", status);
			}
            break;
 		case 99:
			DebugLogRite("Exiting\n");
			break;	 
		default:
			DebugLogRite("Unknown Option\n");
			break;
        }
       
		if(99 != cmd )
		{
			DebugLogRite ("Press [Enter] key to continue ... ");
 			fgets(line, sizeof(line), stdin);
		}
   } while (cmd!=99);

	}

 	Gru_Wlan_GoldenClose();
   return 0;
}


//BTMenu

int BTMenu(void)
{
	int cmd=0;
	int parm1=0, parm2=0; 
	int status=0;
 
	status= Gru_Bt_GoldenInit();

	if(status ) return status;
  

 
	{
	 	 

	 
    do
    {
        DebugLogRite ("\n");
        DebugLogRite ("GOLDEN BT MAIN MENU\n");
        DebugLogRite ("-------------------\n");
 		DebugLogRite ("10. Select Antenna  (0=main, 1=aux)\n");
		DebugLogRite ("12. Set Tx Channel (Ch 0-78)\n"); 

		DebugLogRite ("PayloadPattern: 0: all 0, 1: all 1, 2: PN9, 3: 0xAA, 4: 0xF0\n");
		
		DebugLogRite ("Packet Types \n"
			"\t\tTesting Pattern and ACL:\n"
			"\t\t\t  DM1 = 0x01;   DM3 = 0x03;   DM5 = 0x05; (GFSK, 1M FEC)\n"
			"\t\t\t  DH1 = 0x11;   DH3 = 0x13;   DH5 = 0x15; (GFSK, 1M)\n"
			"\t\t\t2-DH1 = 0x21; 2-DH3 = 0x23; 2-DH5 = 0x25; (DQPSK, 2M)\n"
			"\t\t\t3-DH1 = 0x31; 3-DH3 = 0x33; 3-DH5 = 0x35; (8PSK, 3M)\n"

			"\t\tSco:\n"
			"\t\t\t  HV1 = 0x11;   HV2 = 0x12;   HV3 = 0x13; (GFSK, 1M)\n"
			"\t\teSco:\n"
			"\t\t\t  EV3 = 0x13;   EV4 = 0x14;   EV5 = 0x15; (GFSK, 1M)\n"
			"\t\t\t2-EV3 = 0x23; 2-EV5 = 0x25;               (DQPSK, 2M)\n"
			"\t\t\t3-EV3 = 0x33; 3-EV5 = 0x35;               (8PSK, 3M)\n");


		DebugLogRite ("17. Start Continuous Tx (enable DataRate)\n"); 
  		DebugLogRite ("25. Dutycycle Tx (Enable=0) (PacketType(0x)=11)\n"
		//	"\t(PayloadPattern=2) (PayloadLenInByte=-1) (HopMode=0)\n"
			"\tEnable: 0: off; 1: on\n");

		DebugLogRite ("31. Marvell Rx\n"
			"\t(RxChannel=0) (FrameCount=100) (PayloadLength=-1)\n"
			"\t(PacketType(0x)=11)\n"
			"\t(PayloadPattern=2)\n"
			"\t(TxBdAddress=xx-xx-xx-xx-xx-xx)\n"
			"\n"
			"\tRxChannel: Rx Channel\n"
			"\tFrameCount: number of frames to Tx\n"
			"\tPayloadLength: PayLoad Length in Byte. -1=max for the PacketType\n"
			"\tTxBdAddress: Transmitter's BD address\n"
			);
		DebugLogRite ("32. Stop/Get Rx Test Result\n"); 
		DebugLogRite ("99. Exit\n"); 
        DebugLogRite ("Enter option: ");
        cmd = 0;
        fgets(line, sizeof(line), stdin);
  		parm1=0; 
		parm2=0;
		sscanf (line, "%d %x %x",&cmd, &parm1, &parm2);
		switch (cmd)
        {
	  
 		case 10:
			Gru_Bt_SwitchAntenna(parm1);
            break;
       
		case 12: // Set Tx Channel
			Gru_Bt_SwitchChannel(parm1);
            break;
     
		case 17: // Set continuous Tx
			Gru_Bt_StartContinuousTx(parm1, parm2, DEF_PAYLOADPATTERN,0);
            break;
        case 25: // Set continuous Tx
			Gru_Bt_SetTxDutyCycleHop(parm1, parm2, DEF_PAYLOADPATTERN, 
				PAYLOADLENGTH_MAXIMUN_POSSIBLE, 0, 1);
            break;
		case 31:
 			{	
				int status=0;
				int RxChannel=0;
				DWORD FrameCount =100, packetType=DEF_PACKETTYPE, payloadPattern=DEF_PAYLOADPATTERN;
				DWORD PayloadLength=1024, payloadOnly=0; 
				char TxBdAddress[SIZE_BDADDRESS_INBYTE]={0x5A, 0x4A, 0x4A, 0x3A, 0x3A, 0x01};
  
				sscanf (line, "%d %d %d %d %x %x %x-%x-%x-%x-%x-%x", //:%x %d %d %x %d",
					&cmd, &RxChannel, &FrameCount, &PayloadLength,
					&packetType, &payloadPattern,  
					&TxBdAddress[0], &TxBdAddress[1], &TxBdAddress[2],
					&TxBdAddress[3], &TxBdAddress[4], &TxBdAddress[5]); 
			

				
				status = Gru_Bt_StartRxTest //Dut_Bt_TxBtRxTest
					(RxChannel, FrameCount, packetType, 
										 payloadPattern,	
										 PayloadLength,  
										 TxBdAddress );
				DebugLogRite(" Dut_Bt_MfgMrvlRxTest: %08X\n", status);
			}
  			break;
		case 32:
			{ 
			 			 // report
				int		Report_Status_Complete0_Abort1=0;  
/*
				int		Report_TotalPcktCount=0;
				int		Report_SuccessfulCorrelation=0;
				int		Report_TotalRxedPkt=0; 
				int		Report_HecMatchCount=0;
				int		Report_HecMatchCrcPckts=0;
				int		Report_HdrErrCount=0;
				int		Report_CrcErrCount=0;
				int		Report_NoErrPktCount=0; 
				int		Report_TotalByteCount=0;
				int		Report_ErrorBits=0;
				double		Report_PER=0;
				double		Report_BER=0;
*/
				int		Report_Rssi=0;

				status = Gru_Bt_StopRxTest  
									( 		 // report
										&Report_Status_Complete0_Abort1, // status
/*
										&Report_TotalPcktCount,
										&Report_SuccessfulCorrelation,
										&Report_TotalRxedPkt,
										&Report_HecMatchCount,
										&Report_HecMatchCrcPckts,
										&Report_HdrErrCount,
										&Report_CrcErrCount,
										&Report_NoErrPktCount,
										&Report_TotalByteCount,
										&Report_ErrorBits,
										&Report_PER,
										&Report_BER,
*/
										&Report_Rssi); 
				DebugLogRite(" Gru_Bt_StopRxTest: %08X\n", status);

				if(!status)
				{
					DebugLogRite("\nReport\n"); 

					DebugLogRite("Status_Complete0_Abort1\t%8d\n", 
						Report_Status_Complete0_Abort1);
/*
					DebugLogRite("TotalPcktCount\t\t%8d\n", Report_TotalPcktCount);
					DebugLogRite("SuccessfulCorrelation\t%8d\n", 
						Report_SuccessfulCorrelation);
					DebugLogRite("HecMatchCount\t\t%8d\n", Report_HecMatchCount);
					DebugLogRite("TotalRxedPkt\t\t%8d\n", Report_TotalRxedPkt);
					DebugLogRite("HecMatchCrcPckts\t%8d\n", Report_HecMatchCrcPckts);
					DebugLogRite("HeaderErrCount\t\t%8d\n", Report_HdrErrCount);
					DebugLogRite("CrcErrCount\t\t%8d\n", Report_CrcErrCount);
					DebugLogRite("TotalByteCount\t\t%8d\n", Report_TotalByteCount);
					DebugLogRite("NoErrPktCount\t\t%8d\n", Report_NoErrPktCount);
					DebugLogRite("ErrorBits\t\t%8d\n", Report_ErrorBits);
					DebugLogRite("PER\t\t\t%8.4lf %%\n", Report_PER);
					DebugLogRite("BER\t\t\t%8.4lf %%\n", Report_BER);
*/
					DebugLogRite("RSSI\t\t\t%6d %%\n", Report_Rssi);
					DebugLogRite("\n\n");
				}
			}
			break;
		case 99:
			DebugLogRite("Exiting\n");
			break;	 
		default:
			DebugLogRite("Unknown Option\n");
			break;
        }

        if(99 != cmd )
		{
			DebugLogRite ("Press [Enter] key to continue ... ");
 			fgets(line, sizeof(line), stdin);
		}
    } while (cmd!=99);

	}

 	Gru_Bt_GoldenClose();

	return 0;
}

int main(int argc, char* argv[])
{
	int cmd=0;
	int parm1=0; 
	int status=0;
  
	do{
	DebugLogRite ("1. =========WiFi tool=============\n");
	DebugLogRite ("2. =========BT   tool============ \n");
	DebugLogRite ("99.Exit \n");
		cmd = 0;
		parm1 = 0;

#ifdef _LINUX_MS_
		if (line)

			free(line);

		line = readline("Enter option: ");

		if (line && *line)

		{

			add_history(line);

		}

#else
        DebugLogRite ("Enter option: ");

		fgets(line, sizeof(line), stdin);

#endif

		DebugLogRite("%s\n", line);
        sscanf (line, "%d %x %x",&cmd, &parm1);
        switch (cmd)
		{
		case 1:
			WifiMenu();
			break;
		case 2:
			BTMenu();
			break;
		case 99:
			DebugLogRite("Exiting\n");
			break;		
		default:
			DebugLogRite("Unknown Option\n");
			break;
        }

    } while (99 != cmd);

	return 0;

}


