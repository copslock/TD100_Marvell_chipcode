/** @file UartErrCode.h
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

//#define  HCICTRL_API

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


  
