/** @file DutIf_SdioAdapter.cpp
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

// HW interface adapter layer
#include <basetyps.h>
#include <stdlib.h>
#include <wtypes.h>
#include <setupapi.h>
#include <initguid.h>
#include <stdio.h>
#include <string.h>
#include <winioctl.h>
#include <conio.h>
#include "public.h"


#include "DutIf_SdioAdapter.h"
#include "../DutApi878XDll.h" 
//FileHanel
//   IOCTL_DISK_HISTOGRAM_DATA

// OpenDevice
// CloseDevice
// Query
// GetLastError
BOOL ToasterDeviceInfo();

 
DutIf_SdioAdpt::DutIf_SdioAdpt()
{
	char temp[_MAX_PATH]="";
	
	instanceExist = false;

	h=0;
	DebugLog =0L;

	if(GetCurrentDirectory(_MAX_PATH, temp) != 0)  
	{
		strcat(temp, "\\setup.ini");
	}
	else
	{
		strcpy(temp, "setup.ini");
	}

   	vg_maxWait	= GetPrivateProfileInt("DutInitSet","maxWait",100, temp); 

   
// 	GetPrivateProfileString("Driver","AdapterPrefix","Marvell",adapter_prefix, sizeof(adapter_prefix), temp); 
 
  	DebugLog = GetPrivateProfileInt("DEBUG","debugLog", 0, temp); 

	vg_IfSpy = GetPrivateProfileInt("DEBUG","IfSpy", 0, temp); 
	
	timeOut = GetPrivateProfileInt("DutInitSet","TimeOut",1000, temp); 


	if(DebugLog)
	{
		DebugLogRite( "debugLog %d\n", DebugLog);
 		DebugLogRite("%s\n", temp);
//		DebugLogRite( "adapter_prefix %s\n", adapter_prefix);
 		DebugLogRite( "maxWait %d\n", vg_maxWait);
 		DebugLogRite( "ifSpy %d\n", vg_IfSpy);
	}
}
DutIf_SdioAdpt::~DutIf_SdioAdpt()
{

}



BOOL DutIf_SdioAdpt::query(ULONG OID, PUCHAR pInfo, UINT len)
{
	unsigned long rdCnt=0;
	BOOL err=0;
	UINT i=0;

	rdCnt = len;

	if(vg_IfSpy)
	{
		for (i=0; i<len; i++)
			DebugLogRite("%02X ", *(pInfo+i));
	
		DebugLogRite("\n");
	}

	err = !DeviceIoControl (h, 
                  0x34, //IOCTL_DISK_HISTOGRAM_DATA,
	  			  pInfo, len,
	  			  pInfo, len,
				  //NULL, 0,	
	  			  &rdCnt, NULL) ;

	if(vg_IfSpy)
	{
		for (i=0; i<len; i++)
			DebugLogRite("%02X ", *(pInfo+i));
		DebugLogRite("\n");
	}

	return err;
}


BOOL DutIf_SdioAdpt::set( ULONG OID, PUCHAR pInfo, UINT len)
{
	return query(OID, pInfo, len);
}
DWORD DutIf_SdioAdpt::GetErrorLast(void)
{
	return -1;//W32N_GetErrorLast();
}

int DutIf_SdioAdpt::OpenDevice(void)
{

    HDEVINFO                            hardwareDeviceInfo;
    SP_DEVICE_INTERFACE_DATA            deviceInterfaceData;
    PSP_DEVICE_INTERFACE_DETAIL_DATA    deviceInterfaceDetailData = NULL;
    ULONG                               predictedLength = 0;
    ULONG                               requiredLength = 0, bytes=0;
    int                                 i=0;
    BOOL                                bHide = FALSE;

	if (instanceExist)
		return 0;
	
	instanceExist=true;

	if(!ToasterDeviceInfo())
    {
        DebugLogRite("No toaster devices present\n");
        return ERROR_ACCESS_DENIED;
    }
   
    //
    // Open a handle to the device interface information set of all 
    // present toaster class interfaces.
    //

    hardwareDeviceInfo = SetupDiGetClassDevs (
                       (LPGUID)&GUID_DEVINTERFACE_TOASTER,
                       NULL, // Define no enumerator (global)
                       NULL, // Define no
                       (DIGCF_PRESENT | // Only Devices present
                       DIGCF_DEVICEINTERFACE)); // Function class devices.
    if(INVALID_HANDLE_VALUE == hardwareDeviceInfo)
    {
        DebugLogRite("SetupDiGetClassDevs failed: %x\n", GetLastError());
        return ERROR_INVALID_HANDLE;
    }
    
    deviceInterfaceData.cbSize = sizeof (SP_DEVICE_INTERFACE_DATA);

    printf("\nList of Toaster Device Interfaces\n");
    printf("---------------------------------\n");

    i = 0;

    //
    // Enumerate devices of toaster class
    //
    
    do {
        if (SetupDiEnumDeviceInterfaces (hardwareDeviceInfo,
                                 0, // No care about specific PDOs
                                 (LPGUID)&GUID_DEVINTERFACE_TOASTER,
                                 i, //
                                 &deviceInterfaceData)) {
                                 
            if(deviceInterfaceDetailData) {
                free (deviceInterfaceDetailData);
                deviceInterfaceDetailData = NULL;
            }

            //
            // Allocate a function class device data structure to 
            // receive the information about this particular device.
            //

            //
            // First find out required length of the buffer
            //
            
            if(!SetupDiGetDeviceInterfaceDetail (
                    hardwareDeviceInfo,
                    &deviceInterfaceData,
                    NULL, // probing so no output buffer yet
                    0, // probing so output buffer length of zero
                    &requiredLength,
                    NULL)) { // not interested in the specific dev-node
                if(ERROR_INSUFFICIENT_BUFFER != GetLastError()) {
                    DebugLogRite("SetupDiGetDeviceInterfaceDetail failed %d\n", GetLastError());
                    SetupDiDestroyDeviceInfoList (hardwareDeviceInfo);
                    return ERROR_INSUFFICIENT_BUFFER;                
                }
     
            }

            predictedLength = requiredLength;

            deviceInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc (predictedLength);

            if(deviceInterfaceDetailData) {
                deviceInterfaceDetailData->cbSize = 
                                sizeof (SP_DEVICE_INTERFACE_DETAIL_DATA);
            } else {
                DebugLogRite("Couldn't allocate %d bytes for device interface details.\n", predictedLength);
                SetupDiDestroyDeviceInfoList (hardwareDeviceInfo);
                return ERROR_INSUFFICIENT_BUFFER;
            }


            if (! SetupDiGetDeviceInterfaceDetail (
                       hardwareDeviceInfo,
                       &deviceInterfaceData,
                       deviceInterfaceDetailData,
                       predictedLength,
                       &requiredLength,
                       NULL)) {
                DebugLogRite("Error in SetupDiGetDeviceInterfaceDetail\n");
                SetupDiDestroyDeviceInfoList (hardwareDeviceInfo);
                free (deviceInterfaceDetailData);
                return ERROR_INSUFFICIENT_BUFFER;
            }
            printf("%d) %s\n", ++i,
                    deviceInterfaceDetailData->DevicePath);
        }
        else if (ERROR_NO_MORE_ITEMS != GetLastError()) {
            free (deviceInterfaceDetailData);
            deviceInterfaceDetailData = NULL;
            continue;
        }
        else 
            break;

    } while (TRUE); 


    SetupDiDestroyDeviceInfoList (hardwareDeviceInfo);

    if(!deviceInterfaceDetailData)
    {
        DebugLogRite("No device interfaces present\n");
        return ERROR_INVALID_HANDLE;
    }

    //
    // Open the last toaster device interface
    //

    printf("\nOpening the last interface:\n %s\n", 
                    deviceInterfaceDetailData->DevicePath);
    
    h = CreateFile ( deviceInterfaceDetailData->DevicePath,
                        GENERIC_READ | GENERIC_WRITE,
                        0,
                        NULL, // no SECURITY_ATTRIBUTES structure
                        OPEN_EXISTING, // No special create flags
                        0, // No special attributes
                        NULL);

    if (INVALID_HANDLE_VALUE == (h)) {
        DebugLogRite("Error in CreateFile: 0x%x\n", GetLastError());
        free (deviceInterfaceDetailData);
        return ERROR_INVALID_HANDLE;
    }

    //
    // Invalidate the Device State
    //
        
    if(bHide) 
    {
        if (!DeviceIoControl (h,
                              IOCTL_TOASTER_DONT_DISPLAY_IN_UI_DEVICE,
                              NULL, 0,
                              NULL, 0,
                              &bytes, NULL)) {
            DebugLogRite("Invalidate device request failed:0x%x\n", GetLastError());
            free (deviceInterfaceDetailData);            
            CloseHandle(h);
            return FALSE;
        }
        printf("\nRequest to hide the device completed successfully\n");
       
    }

	free (deviceInterfaceDetailData);
	
	return ERROR_SUCCESS;
}


int DutIf_SdioAdpt::CloseDevice( )
{
//	if (!instanceExist)
//		return ERROR_NONE;
	if(h)	
	{	
		instanceExist = false;
		return CloseHandle(h);
	}
	else 
		return ERROR_NONE;
}




// ======================


BOOL DutIf_SdioAdpt::ToasterDeviceInfo()
{
    HDEVINFO        hdi;
    DWORD           dwIndex=0;
    SP_DEVINFO_DATA deid;
    BOOL            fSuccess=FALSE;
    CHAR           szCompInstanceId[MAX_PATH];
    CHAR           szCompDescription[MAX_PATH];
    CHAR           szFriendlyName[MAX_PATH];
    DWORD           dwRegType;
    BOOL            fFound=FALSE;

    // get a list of all devices of class 'GUID_DEVCLASS_TOASTER'
    hdi = SetupDiGetClassDevs((LPGUID)&GUID_DEVCLASS_TOASTER, NULL, NULL, 
                                            DIGCF_PRESENT);

    if (INVALID_HANDLE_VALUE != hdi)
    {

        // enumerate over each device
        while (deid.cbSize = sizeof(SP_DEVINFO_DATA),
               SetupDiEnumDeviceInfo(hdi, dwIndex, &deid))
        {
            dwIndex++;

            // the right thing to do here would be to call this function
            // to get the size required to hold the instance ID and then
            // to call it second time with a buffer large enough for that size.
            // However, that would tend to obscure the control flow in
            // the sample code. Lets keep things simple by keeping the
            // buffer large enough.

            // get the device instance ID
            fSuccess = SetupDiGetDeviceInstanceId(hdi, &deid,
                                                  (char*)szCompInstanceId,
                                                  MAX_PATH, NULL);
            if (fSuccess)
            {
                // get the description for this instance
                fSuccess =
                    SetupDiGetDeviceRegistryProperty(hdi, &deid,
                                                     SPDRP_DEVICEDESC,
                                                     &dwRegType,
                                                     (BYTE*) szCompDescription,
                                                     MAX_PATH,
                                                     NULL);
                if (fSuccess)
                {
                    memset(szFriendlyName, 0, MAX_PATH);
                    SetupDiGetDeviceRegistryProperty(hdi, &deid,
                                                     SPDRP_FRIENDLYNAME,
                                                     &dwRegType,
                                                     (BYTE*) szFriendlyName,
                                                     MAX_PATH,
                                                     NULL);
                    fFound = TRUE;
                    printf("Instance ID : %s\n", szCompInstanceId);
                    printf("Description : %s\n", szCompDescription);
                    printf("FriendlyName: %s\n\n", szFriendlyName);
                }
            }
        }

        // release the device info list
        SetupDiDestroyDeviceInfoList(hdi);
    }

    if(fFound)
        return TRUE;
    else 
        return FALSE;
}
 



ULONG DutIf_SdioAdpt::GetDebugLogFlag()
{
	return DebugLog;
}
void DutIf_SdioAdpt::SetDebugLogFlag(BOOL Flag_Log)
{
	DebugLog = Flag_Log;
}

void DutIf_SdioAdpt:: GetTimeOut(int *pTimeOut)
{
	//*pTimeOut=GetTimeoutTimer(hPort);
}
void DutIf_SdioAdpt:: SetTimeOut(int TimeOut)
{
//	vg_maxWait = TimeOut;
//	SetTimeoutTimer(hPort, TimeOut);
}

void DutIf_SdioAdpt:: ResetTimeOut(void)
{
//	vg_maxWait = TimeOut;
//	SetTimeoutTimer(hPort, vg_maxWait);
}

void DutIf_SdioAdpt:: ResetDataTimeOut(void)
{
//	vg_maxWait = TimeOut;
//	SetPortTimeoutTimer(hPort, vg_DataMaxWait);
}
BOOL  DutIf_SdioAdpt:: SendAclData(PUCHAR pData, ULONG DataLength)
{
	return 1;//SendHciData(hPort, true, pData, DataLength);
}

BOOL  DutIf_SdioAdpt:: RxData(PUCHAR pData, 
							   ULONG BufLength)
{
	return 1;//RxHciDataEvent(hPort, pData, BufLength);
}


BOOL DutIf_SdioAdpt::queryHci(ULONG OID, PUCHAR Buf, UINT BufSize, 
						UINT ExpectedEvent,	  
						UINT *BufRetSize)
{
#if 0 //def _HCI_PROTOCAL_

#define BUFSIZE	1024
#define EVENT_BUF_SIZE 400
	UCHAR EventData[EVENT_BUF_SIZE]={0}, Buffer[BUFSIZE]={0};
 	USHORT RetBufSize;
	USHORT	CmdGrp=0;
	USHORT	CmdCode=0;
	PUCHAR pInBuf=NULL;
	UINT InBufSize=0;
	int i=0, status=0;
	UINT	expectEvent=HCI_EVENT_CMD_COMPLETE;
	UINT	expectedReturnBuf=EVENT_BUF_SIZE;
	
     if(vg_IfSpy)
	 {
		 DebugLogRite("\nOID %08X\n", OID);
		 DebugLogRite("Command\n");

		 for (i=0; i<BufSize; i++)
			DebugLogRite("%02X ", *(Buf+i));

		 DebugLogRite("\n");
	 }
	if(ExpectedEvent) 
		expectEvent = (0xFFFF & ExpectedEvent);
	else
		expectEvent = HCI_EVENT_CMD_COMPLETE;

	if(BufRetSize) 
		expectedReturnBuf = *BufRetSize;	
	else 
		expectedReturnBuf = BufSize;	


	CmdGrp = (OID>>16);
	CmdCode = (OID&0x3FF);
	 switch(CmdGrp)
	 {
		case HCI_CMD_GROUP_INFORMATIONAL:
			pInBuf=NULL; // for GetBDAddress
			InBufSize=0; 
			break;
		case HCI_CMD_GROUP_HC_BASEBAND:
		case HCI_CMD_GROUP_VENDOR_SPECIFIC:
		case HCI_CMD_GROUP_LINK_CONTROL:
		case HCI_CMD_GROUP_LINK_POLICY:
			pInBuf=Buf;
			InBufSize=BufSize;
			break;
		case 0:
			break;
		default:
			return (-4);
			break;

	 }
	memset(EventData, 0, EVENT_BUF_SIZE);

	Sleep(vg_dwSleep);

 

	status = SendHciCommand(hPort,CmdGrp,  //
					CmdCode,
					expectEvent,
					pInBuf,
					InBufSize,
					EventData,
					(RetBufSize = EVENT_BUF_SIZE));
 	if(status)
	{
		return (status); 
	}

	if (EventData[0]!=ERROR_SUCCESS)
	{
		 
		return EventData[0];
	}
	else
	{
		memset(Buf, 0, (BufSize>expectedReturnBuf)?BufSize:expectedReturnBuf);
 
		if(RetBufSize <= expectedReturnBuf) 
		{
			memcpy(Buf, &EventData[0], RetBufSize);

			if(vg_IfSpy)
			{
				 DebugLogRite("\nResponse\n");
				 for (i=0; i<RetBufSize; i++)
					DebugLogRite("%02X ", *(Buf+i));
			
				 DebugLogRite("\n");
			}
			return EventData[0];
		}
		else
		{

			memcpy(Buf, &EventData[1], expectedReturnBuf-1);

			if(vg_IfSpy)
			{
				 DebugLogRite("\nResponse\n");
				 for (i=0; i<expectedReturnBuf-1; i++)
					DebugLogRite("%02X ", *(Buf+i));
			
				 DebugLogRite("\n");
			}
			return EventData[0];
		}

	}
#endif //#ifdef _HCI_PROTOCAL_
	return 0;
}

