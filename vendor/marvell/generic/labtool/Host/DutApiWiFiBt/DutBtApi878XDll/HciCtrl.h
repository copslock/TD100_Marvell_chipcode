/** @file HciCtrl.h
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

#ifdef _HCICTRL_EXPORTS
#define HCICTRL_API __declspec(dllexport)
#else
#define HCICTRL_API __declspec(dllimport)
#endif

#ifdef _LINUX_ 
#undef HCICTRL_API 
#define  HCICTRL_API
#endif

#ifndef RTS_CONTROL_DISABLE
#define RTS_CONTROL_DISABLE		(0)
#endif

#ifndef RTS_CONTROL_HANDSHAKE
#define RTS_CONTROL_HANDSHAKE	(2)
#endif

#ifndef CBR_38400
#define CBR_38400			 (38400)
#endif

#ifndef CBR_115200
#define CBR_115200			(115200)
#endif

#ifndef CBR_3000000
#define CBR_3000000			(3000000)
#endif 
 

/*
//-----------------------------------------------
//	Error Codes
//------------------------------------------------
#define ERROR_OUT_OF_SYNC				0x00F0
#define ERROR_COMMAND_PACKET			0x00F1
#define ERROR_UNKNOWN_PACKET			0x00F2
#define ERROR_HARDWARE_FAILURE			0x00F3

#define ERROR_ACCESS_COM_PORT			0x00F4
#define ERROR_BUFFER_TOO_SMALL			0x00F5
#define ERROR_INVALID_PACKET_TYPE		0x00F6
#define ERROR_INVALID_TEST_OPTION		0x00F7
#define ERROR_GET_EVENT_TO				0x00F8

 
#define ERROR_TX_PKT_TYPE_MISMATCH		0x8001
#define ERROR_TX_PKT_SENT_NOT_SET		0x8002
#define ERROR_TX_PKT_BYTE_SENT			0x8003
#define ERROR_TX_PAYLOAD_HEADER			0x8004
#define ERROR_TX_CHUNK_REMAINING		0x8005

#define ERROR_GET_RD_STATUS				0x8006
#define ERROR_RD_RX_PKT_TYPE			0x8007
#define ERROR_RD_PAYLOAD_HEADER			0x8008
#define ERROR_RD_WRITE_LEVEL			0x8009
#define ERROR_RD_PAYLOAD_LENGTH			0x800A
#define ERROR_RX_MATCH_STATUS			0x800B
#define ERROR_RX_DATA_MISMATCH			0x800C
#define ERROR_8DC_RX_PKT_TYPE			0x800D
#define ERROR_RX_FLOW_ERROR				0x800E
#define ERROR_RX_ARQN_ERROR				0x800F
#define ERROR_RX_SEQN_ERROR				0x8010

#define ERROR_LINK_DISCONNECT			0x8011

*/
 
//-----------------------------------------------
//	Functions
//------------------------------------------------
HCICTRL_API HANDLE InitUARTPort(char* ComPort, DWORD BaudRate=CBR_115200, 
								DWORD flowControl=RTS_CONTROL_HANDSHAKE, 
								bool OverLapped=false);
HCICTRL_API BOOL ClosePort(HANDLE hPort);

HCICTRL_API int SendHciCommand(HANDLE hPort, USHORT CmdGroup, 
							   USHORT CmdCode, USHORT ExpEvent, 
							   PUCHAR pParam, USHORT ParamSize, 
								PUSHORT pFoundEvent,
							   PUCHAR pReturn, USHORT& RetBufSize);
HCICTRL_API int GetEvent(HANDLE hPort, 
						 USHORT CmdGroup, USHORT CmdCode, 
						 USHORT ExpEvent, 
						 PUCHAR pEvent, USHORT RetBufSize);

//HCICTRL_API int SendHciCommand2(HANDLE hPort, USHORT CmdGroup, 
//							   USHORT CmdCode, USHORT ExpEvent, 
//							   PUCHAR pParam, USHORT ParamSize, 
//							   PUCHAR pReturn, USHORT& RetBufSize);

HCICTRL_API int SendHciData(HANDLE hPort, BOOL IsAcl, PUCHAR pData, 
							ULONG DataLength);
HCICTRL_API int RxHciDataEvent(HANDLE hPort, PUCHAR pData, 
							   ULONG BufLength);
HCICTRL_API ULONG GetTimeoutTimer(HANDLE hPort);
HCICTRL_API void SetTimeoutTimer(HANDLE hPort, ULONG Timer);
 
HCICTRL_API ULONG GetPortTimeoutTimer(HANDLE hPort);
HCICTRL_API void SetPortTimeoutTimer(HANDLE hPort, ULONG Timer);
HCICTRL_API int SetBaudRateFlowControlMode(HANDLE hPort, DWORD BaudRate, DWORD flowControl); 
HCICTRL_API int HciCtrlVersion(void);
HCICTRL_API char* HciCtrlAbout(void);

