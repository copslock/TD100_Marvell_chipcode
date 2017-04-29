/*
 
the unified interface for labtool to external modules like AT command server 
 
*/


#include "W878XApp.h"
#include "DutWlanApi878XDll.h"
#include "DutBtApi878XDll.h"
#include "DutFmApi878XDll.h"

#if !defined(_NO_EEPROM_)
#include "../DutCommon/Dut_error.hc"
#include "../DutCommon/utilities_os.h"
#include "../DutCommon/utilities.h"
#include "../DutCommon/mathUtility.h"
#include "../DutCommon/utilities_os.h"
#endif

#define WIFI_LOAD_DRV_PROD_FW  "/marvell/tel/load_prod_8787.sh"
#define WIFI_LOAD_DRV_MFG_FW   "/marvell/tel/load_mfg_8787.sh"
#define WIFI_UNLOAD_DRV_FW     "/marvell/tel/unload_8787.sh"

#define OPEN_AUD_PATH_FM_HEADSET    "/marvell/tel/open_aud_path_fm_headset.sh"
#define OPEN_AUD_PATH_FM_SPEAKER    "/marvell/tel/open_aud_path_fm_speaker.sh"
#define CLOSE_AUD_PATH_FM           "/marvell/tel/close_aud_path_fm.sh"
#define OPEN_AUD_PATH_HIFI_HEADSET  "/marvell/tel/open_aud_path_hifi_headset.sh"
#define CLOSE_AUD_PATH_HIFI_HEADSET "/marvell/tel/close_aud_path_hifi_headset.sh"

extern "C"
{
#if !defined(_NO_EEPROM_)
static int ChangeBdAddress(BYTE *pNewBdAddress);
static int GetBdAddressFromE2p(BYTE *pBdAddress);
#endif
static int is_no_gui_mode();

int wifi_test_api(int attr, int p1, int p2, char *data)
{
	int dataValue;
	int status;
	int rate, channel;
	double freq;

	void *oneObj=NULL;

	if(attr!=0)
		return 1;

	switch(p1)
	{
	case 0:
		switch (p2)
		{
		case 0:
			//set channel command	
			dataValue = atoi(data);
			status = DutIf_SetRfChannel(dataValue);
			if ( status )
			{
				return status;
			}
			data[0]=0;
			return 0;
		case 1:
			//set tx rate	
			dataValue = atoi(data);
			if( dataValue < 0 || dataValue > 29 )
			{
				return 1;
			}
			status = DutIf_SetTxDataRate(dataValue);
			if ( status )
			{
				return status;
			}
			data[0]=0;
			return 0;
		case 6:
			// set band 2.4G or 5G	
			dataValue = atoi(data);
			if( dataValue <0 || dataValue > 1 )
			{
				return 1;
			}
			status = DutIf_SetBandAG(dataValue);
			if ( status )
			{
				return status;
			}
			data[0]=0;
			return 0;
		case 7:
			// set bandwidth	
			dataValue = atoi(data);
			if( dataValue < 0 || dataValue > 5 )
			{
				return 1;
			}
			status = DutIf_SetChannelBw(dataValue);
			if ( status )
			{
				return status;
			}
			data[0]=0;
			return 0;
		case 8:
			//set power level
			dataValue = atoi(data);
			data[0]=0;
			status = DutIf_GetTxDataRate(&rate);
			if ( status )
			{
				return status;
			}
			status = DutIf_GetRfChannel(&channel, &freq);
			if ( status )
			{
				return status;
			}
			status = DutIf_SetRfChannel(channel);
			status = DutIf_SetRfPowerUseCal(&dataValue, rate);
			if ( status )
			{
				return status;
			}
			return 0;
		case 9:
			//get channel 
			status = DutIf_GetRfChannel(&channel, &freq);
			if ( status )
			{
				return status;
			}
			sprintf(data, "+MVWFTST: %d,%d,%d,%d %lf\r\n", attr, p1, p2, channel, freq);
			return 0;
		case 10:
			int channelBW;
			status = DutIf_GetChannelBw(&channelBW);
			if(status)
			{
				return status;
			}
			sprintf(data, "+MVWFTST: %d,%d,%d,%d\r\n", attr, p1, p2, channelBW);
			return 0;
		default:
			return 1;
		}
		break;
	case 1:
		data[0]=0;
		switch(p2)
		{
		case 0:
			//close DUT
			status = DutIf_Disconnection(NULL);
			return 0;
		case 1:
			//open DUT
			status = DutIf_InitConnection(&oneObj);
			if (status) 
			{
				return status;
			}
			return 0;
		case 2:
			//load product FW
			status = system(WIFI_LOAD_DRV_PROD_FW);
			return 0;
		case 3:
			//load manufacture FW
			status = system(WIFI_LOAD_DRV_MFG_FW);
			return 0;
		case 4:
			//unload FW
			status = system(WIFI_UNLOAD_DRV_FW);
			return 0;
		default:
			return 1;
		}
		break;
	case 5:
		data[0]=0;
		switch(p2)
		{
		case 0:
			//stop continues Tx
			status = DutIf_SetTxContMode(0, 0);
			if (status) 
			{
				return status;
			}
			return 0;
		case 1:
			//start continues tx
			status = DutIf_SetTxContMode(1, 0);
			if (status) 
			{
				return status;
			}
			return 0;
		case 2:
			//start continues CW tx
			status = DutIf_SetTxContMode(1, 1);
			if (status) 
			{
				return status;
			}
			return 0;
		case 3:
			//send burst tx
			status = DutIf_AdjustPcktSifs(0, 4, 0xAAAAAAAA, 0x400, 1, "",3,0,0,0,0,0,0,0,-1,false,0,0);
			if(status)
			{
				return status;
			}
			return 0;
		default:
			return 1;
		}
		break;
	case 8:
		switch(p2)
		{
		case 0:
			{
				// Set Carrier Suppresion
				int enable = 0;
				enable = atoi(data);
				data[0] = 0;
				status = DutIf_SetTxCarrierSuppression(enable);
				return status;
			}
			break;
		case 1:
			{
				// Set Power at Antenna using Cal data
 				int Pwr4Pa=0;
				BOOL rateG=0;
				int totalLen=0;
				int channel = 0;
				int cnt = 0;

				FILE* hFile;
				char temp[MAX_LENGTH]="WlanCalData_ext.conf";
				int i =0xf0;
				DWORD j=0;
				char temp1[4]="";

				cnt = sscanf (data, "%d %d %d", &channel, &Pwr4Pa, &rateG);
				data[0] = 0;
 				if(2 > cnt) Pwr4Pa= 15;
 		        status = DutIf_SetRfChannel(channel);
				if(status)
					return status;

				if (Dut_Shared_SupportNoE2PROM())
				{
					//read the cal data content from file
					int CalDataLoaded=0;
					status = DutIf_GetCustomizedSettings(&CalDataLoaded);
					if(CalDataLoaded)
					{
						status = DutIf_SetRfPowerUseCal(&Pwr4Pa, rateG);
					}
					else
					{
						hFile = fopen(temp,"rb"); 
						if (!hFile)
						{
							return 1;
						}

						fclose (hFile);
						status = DutIf_SetCustomizedSettings(temp);
						status = DutIf_SetRfPowerUseCal(&Pwr4Pa, rateG);
					}
				}
				else
				{
					status = DutIf_SetRfPowerUseCal(&Pwr4Pa, rateG);
				}
				return status;
			}
			break;
		case 2:
			{
				// Set Dutycycle Tx Mode
				int loss=0, tuning=0, payloadweight=50;
				int pattern=0, shortPreamble=0;
				int enable = 0, datarate = 0;
#if defined (_MIMO_)
				int ActSubCh=3, ShortGI=0, AdvCoding=0;
				int	TxBfOn = 0, GFMode=0, STBC=0;
				DWORD DPDEnable =0, PowerID=0;
				DWORD		SignalBw=-1;
#endif // #if defined (_MIMO_)

				sscanf (data, "%d %d %d %x %d"
#if defined (_MIMO_)
					" %d %d %d %d %d %d %d %d %d"
#endif // #if defined (_MIMO_)
					,
					&enable, &datarate, &payloadweight, &pattern, &shortPreamble
#if defined (_MIMO_)
					, 
					&ActSubCh, &ShortGI, &AdvCoding, &TxBfOn, &GFMode, &STBC, 
					&DPDEnable, &PowerID, &SignalBw 
#endif // #if defined (_MIMO_)
					); 
				data[0] = 0;
				if(0 == datarate) datarate =4; 

 				status = DutIf_SetTxDutyCycle(enable, datarate-1, payloadweight, 
									pattern, shortPreamble
#if defined (_MIMO_)	
									,
									ActSubCh, ShortGI, AdvCoding, TxBfOn, 
									GFMode, STBC, DPDEnable, PowerID, SignalBw
#endif // #if defined (_MIMO_)									
									);
				return status;
			}
			break;
		case 3:
			// Clear received packet count
			status = DutIf_ClearRxPckts();
			data[0] = 0;
			return status;
		case 4:
			{
				// Get received packet count
				DWORD cnt=0, CntOk=0, CntKo=0;
				data[0] = 0;
				status = DutIf_GetRxPckts(&cnt, &CntOk, &CntKo);
				if(status)
					return status;
				sprintf(data, "+MVWFTST: %d,%d,%d,%d %d %d\r\n", attr, p1, p2, cnt, CntOk, CntKo);
				return 0;
			}
            break;
		case 5:
			{
				// Get FW version
				BYTE FwVersion[80]="";

				data[0] = 0;

				status = DutIf_GetFWVersion(FwVersion);

				if(status)
					return status;
				
				sprintf(data, "+MVWFTST: %d,%d,%d,%s\r\n", attr, p1, p2, FwVersion);
				return 0;
			}
            break;
		case 6:
			{
				// Get HW version
				WORD socId=0;
				BYTE rfId=0, bbpId=0;
				BYTE socVersion=0, rfVersion=0, bbpVersion=0;

				data[0] = 0;
				
				status = DutIf_GetHWVersion(&socId, &socVersion, 
										&rfId, &rfVersion, 
										&bbpId, &bbpVersion);
				if (status) 
					return status;

				sprintf(data, "+MVWFTST: %d,%d,%d,%04X %02X %02X %02X %02X %02X\r\n",
					attr, p1, p2, socId, socVersion, bbpId, bbpVersion, rfId, rfVersion);
				return 0;
			}
            break;

		default:
			return 1;
		}
		break;

	default:
		return 1;
	}

	return 1;
}

int wifi_id_api(int attr, int p1, int p2, char *data)
{
	int status;

	unsigned char tempMacAddr[6]={0,0,0,0,0,0};
	int tempInt[6]={0}, cnt=0; 

	if (attr!=0)
	{
		return 1;
	}
	switch(p1)
	{
	case 0:
		switch(p2)
		{
		case 0:
			//get MAC address
			status = DutIf_GetMACAddress(tempMacAddr);
			data[0] = 0;
			if( status )
			{
				return status;
			}
			sprintf(data, "+MVWFIDRW: %d,%d,%d,%02x.%02x.%02x.%02x.%02x.%02x\r\n",
					attr, p1, p2,
					tempMacAddr[0], tempMacAddr[1], tempMacAddr[2],
					tempMacAddr[3], tempMacAddr[4], tempMacAddr[5]);
			return 0;
		case 1:
			// set MAC address
			cnt = sscanf (data, "%x %x %x %x %x %x", 
				&tempInt[0], &tempInt[1], &tempInt[2],
				&tempInt[3], &tempInt[4], &tempInt[5]);
			data[0] = 0;
			if(cnt < 6) 
			{
				return 1;
			}
			else{
				tempMacAddr[0]= tempInt[0];
				tempMacAddr[1]= tempInt[1];
				tempMacAddr[2]= tempInt[2];
				tempMacAddr[3]= tempInt[3];
				tempMacAddr[4]= tempInt[4];
				tempMacAddr[5]= tempInt[5];

				status = DutIf_SetMACAddress(tempMacAddr);
				if( status )
					return status;
			}
			return 0;
		default:
			return 1;
		}
	default:
		return 1;

	}
}

int bt_test_api(int attr, int p1, int p2, char *data)
{
	int dataValue;
	int status;
	int rate, channel;
	double freq;

	void *oneObj=NULL;

	if(attr!=0)
		return 1;

	switch(p1)
	{
	case 0:
		data[0]=0;
		switch (p2)
		{
		case 0:
			// activation
			return 1;
		default:
			return 1;
		}
		break;
	case 1:
		data[0]=0;
		switch(p2)
		{
		case 0:
			// search start
			return 1;
		case 1:
			// search w/ ack
			return 1;
		case 2:
			// search stop
			return 1;
		default:
			return 1;
		}
		break;
	case 2:
		data[0]=0;
		switch(p2)
		{
		case 0:
			// deactivation (normal)
			return 1;
		case 1:
			// deactivate RF test
			return 1;
		case 2:
			// deactivate audio test
			return 0;
		default:
			return 1;
		}
		break;
	case 3:
		data[0]=0;
		switch(p2)
		{
		case 0:
			// turn test mode on
			return 1;
		default:
			return 1;
		}
		break;
	case 4:
		data[0]=0;
		switch(p2)
		{
		case 0:
			// turn audio test mode on
			return 1;
		default:
			return 1;
		}
		break;
	case 5:
		data[0]=0;
		switch(p2)
		{
		case 0:
			// Open device
			status = Dut_Bt_OpenDevice(&oneObj);
			return status;
		case 1:
			// Close device
			status = Dut_Bt_CloseDevice(&oneObj);
			return status;
		default:
			return 1;
		}
		break;
	case 6:
		switch(p2)
		{
		case 0:
			{
				// Get BT channel (BT2 = 0)
				int channelNum =0, FreqInMHz=0, BT2; 
				BT2 = atoi(data);
				data[0] = 0;
				status = Dut_Bt_GetBtChannel(&channelNum, &FreqInMHz, (BT2)?true:false);
				if(status)
					return status;
				sprintf(data, "+MVBTTST: %d,%d,%d,%d %d\r\n", attr, p1, p2, channelNum, FreqInMHz);
				return 0;
			}
			break;
		case 1:
			{
				// Set BT channel (BT2 = 0)
				DWORD channelNum =0, FreqInMHz=0;
				DWORD BT2=0;
				sscanf (data, "%d %d", &channelNum, &BT2);
				data[0] = 0;
				if(channelNum > 78)
					return 1;
				status = Dut_Bt_SetBtChannel(channelNum, (BT2)?true:false);
				return status;
			}
			break;
		default:
			return 1;
		}
		break;
	case 7:
		switch(p2)
		{
		case 0:
			{
				// Dutycycle Tx
				BOOL enable =0;
				int packetType=DEF_PACKETTYPE, payloadPattern=DEF_PAYLOADPATTERN;
				int PayloadLenghtInByte=PAYLOADLENGTH_MAXIMUN_POSSIBLE, HopMode=0, Interval=1, Whitening=0;
				sscanf (data, "%d %x %x %d %d %d %d",
					&enable, 
					&packetType, &payloadPattern, 
					&PayloadLenghtInByte, &HopMode, &Interval, &Whitening);
				data[0]=0;
    	
#ifdef _HCI_PROTOCAL_
				status = Dut_Bt_TxBtDutyCycleHop(
					enable?true:false, 
					packetType,
					payloadPattern, 
					PayloadLenghtInByte, 
					HopMode?true:false, 
					Interval, 
					Whitening?true:false);
#else
				status = Dut_Bt_MfgMrvlTxTest(
					enable?true:false, 
					packetType,
					payloadPattern, 
					PayloadLenghtInByte, 
					HopMode?true:false, 
					Interval, 
					Whitening?true:false);
#endif // #ifdef _HCI_PROTOCAL_
				return status;
			}
			break;
		default:
			return 1;
		}
		break;
	case 8:
		switch(p2)
		{
		case 0:
			{
				// Enable Device Under TestMode
#ifdef _HCI_PROTOCAL_
				int resetBT = 0;
				resetBT = atoi(data);
				data[0] = 0;
				if(resetBT)
				{ 
					status = Dut_Bt_ResetBt();
					status = Dut_Bt_SetDeviceDiscoverable(); 
				}
				status = Dut_Bt_EnableDeviceUnderTestMode(); 
#else // #ifdef _HCI_PROTOCAL_
				status = Dut_Bt_MfgEnterTestMode(); 
				data[0]=0;
#endif
				return status;
			}
		default:
			return 1;
		}
		break;
	case 9:
		switch(p2)
		{
		case 0:
			{
				// FW version
 				char VersionString[_MAX_PATH]="";

				status = Dut_Bt_GetFwVersion(NULL, NULL, VersionString);
				data[0] = 0;
				if (status)
					return status;

				sprintf(data, "+MVBTTST: %d,%d,%d,%s\r\n", attr, p1, p2, VersionString);
				return 0;
			}
			return 1;
		case 1:
			{
				// HW version
				int SocId=0, SocVer=0, BtuId=0, BtuVer=0, BrfId=0, BrfVer=0;

				status = Dut_Bt_GetHwVersion(&SocId, &SocVer, 
											&BtuId, &BtuVer, 
											&BrfId, &BrfVer);
				data[0] = 0;
				if (status)
					return status;

				sprintf(data, "+MVBTTST: %d,%d,%d,%04X %04X %04X %04X %04X %04X\r\n",
					attr, p1, p2, SocId, SocVer, BtuId, BtuVer, BrfId, BrfVer);
				return 0;
			}
			return 1;
		default:
			return 1;
		}
		break;

	default:
		return 1;
	}
	return 1;
}

int bt_id_api(int attr, int p1, int p2, char *data)
{
	int status;

	unsigned char tempBdAddress[6]={0}; 
	int tempInt[6]={0}, cnt=0; 

	if (attr!=0)
	{
		return 1;
	}
	switch(p1)
	{
	case 0:
		switch(p2)
		{
		case 0:
			// get MAC address
#if !defined(_NO_EEPROM_)
			status = GetBdAddressFromE2p(tempBdAddress);
#endif //#if !defined(_NO_EEPROM_)				

#if 0
#ifdef _HCI_PROTOCAL_
			status = Dut_Bt_GetBDAddress(tempBdAddress);
#else	//#ifdef _HCI_PROTOCAL_
			status = Dut_Bt_MfgGetFwBdAddress(tempBdAddress);
#endif //#ifdef _HCI_PROTOCAL_
#endif

			data[0] = 0;
			if( status )
			{
				return status;
			}

			sprintf(data, "+MVBTIDRW: %d,%d,%d,%02X-%02X-%02X-%02X-%02X-%02X\r\n",
				attr, p1, p2,
				tempBdAddress[0], tempBdAddress[1], tempBdAddress[2],
				tempBdAddress[3], tempBdAddress[4], tempBdAddress[5]);

			return 0;
		default:
			return 1;
		}
	case 1:
		switch(p2)
		{
		case 0:
			// set MAC address
			cnt = sscanf (data, "%x %x %x %x %x %x", 
				&tempInt[0], &tempInt[1], &tempInt[2],
				&tempInt[3], &tempInt[4], &tempInt[5]);
			data[0] = 0;
			if(cnt < 6) 
			{
				return 1;
			}

			tempBdAddress[0]= tempInt[0];
			tempBdAddress[1]= tempInt[1];
			tempBdAddress[2]= tempInt[2];
			tempBdAddress[3]= tempInt[3];
			tempBdAddress[4]= tempInt[4];
			tempBdAddress[5]= tempInt[5];

#if !defined(_NO_EEPROM_)
			status = ChangeBdAddress(tempBdAddress);
#else
			return 1;
#endif

			if( status )
				return status;
			return 0;
		default:
			return 1;
		}
	default:
		return 1;

	}
	return 1;
}

int fm_test_api(int attr, int p1, int p2, char *data)
{
	int status = 0;

	void *oneObj=NULL;

	switch(attr)
	{
	case 0:
		switch (p1)
		{
		case 0:
			switch(p2)
			{
			case 0:
				// FM set radio on, output to earphone,
				// set channel
			case 1:
				// FM set radio on, output to speaker,
				// set channel
				{
					DWORD chInKHz = 87500;
					chInKHz = atoi(data);
					data[0] = 0;

					// initialize
					status = Dut_Fm_FmInitialization(38400);
					if(status)
						return status;

					// set radio on
					// here 3 means: start audio and RDS receiver
					status = Dut_Fm_FmPowerUpMode(3);
					if(status)
						return status;

					// set channel
					status = Dut_Fm_SetFmChannel(&chInKHz);
					if(status)
						return status;

					// set audio path

					// if we are in no gui mode
					// we have to close the HIFI headset audio path first
					if(is_no_gui_mode())
						system(CLOSE_AUD_PATH_HIFI_HEADSET);

					if(p2 == 0)
						status = system(OPEN_AUD_PATH_FM_HEADSET);
					else
						status = system(OPEN_AUD_PATH_FM_SPEAKER);

					return 0;
				}
				return 1;
			case 2:
				{
					// FM set frequency
					DWORD chInKHz = 87500;
					chInKHz = atoi(data);
					data[0] = 0;

					status = Dut_Fm_SetFmChannel(&chInKHz);
					return status;
				}
				return 1;
			case 3:
				{
					// FM set volume
					int volume=0;
					volume = atoi(data);
					data[0] = 0;

					status = Dut_Fm_SetFmAudioVolume(volume);
					return status;
				}
				return 1;
			default:
				return 1;
			}
			break;
		case 1:
			switch(p2)
			{
			case 0:
				// FM set radio off
				data[0] = 0;
				status = Dut_Fm_FmPowerDown();
				if(status)
					return status;

				// close fm audio path
				status = system(CLOSE_AUD_PATH_FM);
				// if we are in no gui mode
				// we have to open the HIFI headset audio path, after close fm
				// audio path
				if(is_no_gui_mode())
					system(OPEN_AUD_PATH_HIFI_HEADSET);

				return 0;
			default:
				return 1;
			}
			break;
		case 2:
			switch(p2)
			{
			case 0:
				// FM open device
				data[0] = 0;
				status = Dut_Fm_OpenDevice(&oneObj);
				return status;
			case 1:
				// FM close device
				data[0] = 0;
				status = Dut_Fm_CloseDevice(&oneObj);
				return status;
			default:
				return 1;
			}
		default:
			return 1;
		}
		break;
	case 1:
		switch(p1)
		{
		case 0:
			switch(p2)
			{
			case 0:
				{
					// FM get RSSI
					DWORD rssi = 0;
					status = Dut_Fm_GetFmCurrentRssi(&rssi);
					data[0] = 0;
					if(status)
						return status;

					sprintf(data, "+MVFMTST: %d,%d,%d,%d\r\n", attr, p1, p2, rssi);
					return 0;
				}
				return 1;
			case 1:
				{
					// FM get audio sensitivity
					int stereo_status = 0;

					data[0] = 0;

					status = Dut_Fm_GetFmStereoStatus(&stereo_status);
					if(status)
						return status;

					sprintf(data, "+MVFMTST: %d,%d,%d,%d\r\n", attr, p1, p2, stereo_status);
					return 0;
				}
				return 1;
			case 2:
				{
					// FM get RDS sensitivity
					BYTE SyncStatus =0;
					data[0] = 0;

					status = Dut_Fm_GetFmRxRdsSyncStatus(&SyncStatus );
					if(status)
						return status;

					sprintf(data, "+MVFMTST: %d,%d,%d,%d\r\n", attr, p1, p2, SyncStatus);

					return 0;
				}
				return 1;
			default:
				return 1;
			}
			break;
		default:
			return 1;
		}
		break;
	default:
		return 1;
	}
	return 1;
}

#if !defined(_NO_EEPROM_)
static int ChangeBdAddress(BYTE *pNewBdAddress)
{
	int status=0;
	DWORD UartBaudRate=DEFVALUE_BT_UARTBAUDRATE;
	int CsC=0, CsValue=0; 
	BYTE pBdAddress[SIZE_BDADDRESS_INBYTE]={0}; 
  
 	BYTE Version=0, RFXtal=0, InitPwr=DEFVALUE_BT_INITPWR, FELoss=0;
				 
	bool ForceClass2Op=DEFVALUE_BT_FORCECLASS20, DisablePwrControl=DEFVALUE_BT_DISABLE_PWR_CONTROL, MiscFlag=DEFVALUE_BT_MISC_FLAG;
	bool UsedInternalSleepClock=DEFVALUE_BT_USED_INTERNAL_SLEEP_CLOCK;
	BYTE Rssi_Golden_Lo=DEFVALUE_BT_RSSIGOLDENLOW, Rssi_Golden_Hi=DEFVALUE_BT_RSSIGOLDENHIGH;
 	BYTE Encr_Key_Len_Max=DEFVALUE_BT_ENCRYP_KEY_LEN_MAX, Encr_Key_Len_Min=DEFVALUE_BT_ENCRYP_KEY_LEN_MIN;
 	
 	status =Dut_Bt_GetCalDataRevC(&CsC, &CsValue);
 	if(status)	return status ;
 	if(!(CsC))	return status ;

 	status = Dut_Shared_GetAnnexType55Data( 
 				&Version, &RFXtal,
				&InitPwr, &FELoss,

				&ForceClass2Op,
				&DisablePwrControl,
				&MiscFlag,
				&UsedInternalSleepClock,
				&Rssi_Golden_Lo,
				&Rssi_Golden_Hi,
				&UartBaudRate,
				pBdAddress,
				&Encr_Key_Len_Max,
				&Encr_Key_Len_Min);
	if(ERROR_DATANOTEXIST == status)  
	{
		return status;
	}

	memcpy(pBdAddress, pNewBdAddress,  sizeof(BYTE)*SIZE_BDADDRESS_INBYTE);

 	status = Dut_Shared_SetAnnexType55Data( 
				Version, RFXtal,
				InitPwr, FELoss,

				ForceClass2Op,
				DisablePwrControl,
				MiscFlag,
				UsedInternalSleepClock,
				Rssi_Golden_Lo,
				Rssi_Golden_Hi,
				UartBaudRate,
				pBdAddress,
				Encr_Key_Len_Max,
				Encr_Key_Len_Min);
	if(status)	return status ;

	status = Dut_Bt_UpdateCalDataRevC_Bt(1);

	return status;
}

static int GetBdAddressFromE2p(BYTE *pBdAddress)
{
	int status=0;
	DWORD UartBaudRate=DEFVALUE_BT_UARTBAUDRATE;
	int CsC=0, CsValue=0; 

 	BYTE Version=0, RFXtal=0, InitPwr=DEFVALUE_BT_INITPWR, FELoss=0;
				 
	bool ForceClass2Op=DEFVALUE_BT_FORCECLASS20, DisablePwrControl=DEFVALUE_BT_DISABLE_PWR_CONTROL, MiscFlag=DEFVALUE_BT_MISC_FLAG;
	bool UsedInternalSleepClock=DEFVALUE_BT_USED_INTERNAL_SLEEP_CLOCK;
	BYTE Rssi_Golden_Lo=DEFVALUE_BT_RSSIGOLDENLOW, Rssi_Golden_Hi=DEFVALUE_BT_RSSIGOLDENHIGH;
 	BYTE Encr_Key_Len_Max=DEFVALUE_BT_ENCRYP_KEY_LEN_MAX, Encr_Key_Len_Min=DEFVALUE_BT_ENCRYP_KEY_LEN_MIN;

 	status =Dut_Bt_GetCalDataRevC(&CsC, &CsValue);
 	if(status)	return status ;
 	if(!(CsC))	return status ;

 	status = Dut_Shared_GetAnnexType55Data( 
 				&Version, &RFXtal,
				&InitPwr, &FELoss,

				(bool*)&ForceClass2Op,
				(bool*)&DisablePwrControl,
				(bool*)&MiscFlag,
				(bool*)&UsedInternalSleepClock,
				&Rssi_Golden_Lo,
				&Rssi_Golden_Hi,
				&UartBaudRate,
				pBdAddress,
				&Encr_Key_Len_Max,
				&Encr_Key_Len_Min);
	if(ERROR_DATANOTEXIST == status)  
	{
		return status;
	}

	return status;
}
#endif

static int is_no_gui_mode()
{
	static int inited = 0;
	static int is_no_gui = 0;
	char cmdline[1024] = {0};
	char *ptr = NULL;
	int fd = -1;

	if(inited)
		return is_no_gui;

	fd = open("/proc/cmdline", O_RDONLY);
	if(fd >= 0)
	{
		int n = read(fd, cmdline, sizeof(cmdline)/sizeof(cmdline[0])-1);
		if(n < 0) n = 0;

		cmdline[n] = 0;

		close(fd);
	}

	ptr = strstr(cmdline, "androidboot.bsp");
	if(ptr)
	{
		ptr = strchr(ptr, '=');
		if(ptr)
		{
			++ptr;
			int bsp_flag = atoi(ptr);
			if(bsp_flag > 0)
				is_no_gui = 1;
		}
	}

	inited = 1;
	return is_no_gui;
}

}

