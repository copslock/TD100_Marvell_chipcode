/** @file HciStruct.h
 *
 *  Copyright (C) 2009-2010, Marvell International Ltd.
 *  All Rights Reserved
 */

//------------------------------
//	Constants
//------------------------------
#define SIZE_OF_BD_ADDRESS		6
#define MAX_INQUIRY_RESPONSES	3

#define ACL_HEADER_SIZE			4
#define SCO_HEADER_SIZE			3

//#define NUM_OF_ACL_FRAMES		12
//#define NUM_BR_ACL_FRAMES		6
//#define NUM_EDR_ACL_FRAMES		6

//#define NUM_OF_SCO_FRAMES		4

#if defined(WIN32) || defined(NDIS51_MINIPORT) || defined(NDIS50_MINIPORT) || defined(_LINUX_)
#pragma pack(1) 
#ifdef _LINUX_
#define PACK __attribute__ ((packed))
#else
#define PACK
#endif
#else
#include "win.h" fghjukl
#endif

 
 
//-----------------------------
//	Enumeration
//-----------------------------
enum
{
	HCI_NEXT_PACKET=0,		// 0
	HCI_COMMAND_PACKET,		// 1
	HCI_ACL_DATA_PACKET,	// 2
	HCI_SCO_DATA_PACKET,	// 3
	HCI_EVENT_PACKET,		// 4
 	HCI_OUT_OF_SYNC			// 5
};
/*
typedef enum apOS_EBC_eHCI
{
  // EABBC extended commands
  apOS_EBC_MISC_READ_MEM       = (1),   // Read USHORT
  apOS_EBC_MISC_WRITE_MEM      = (2) 	  // Write USHORT 
};

enum 
{
	RUN_ACL_TEST=1,
	RUN_SCO_TEST,
	RUN_ESCO_TEST
};

enum
{
	AirCoding_CVSD,
	AirCoding_uLaw,
	AirCoding_ALaw
};

enum
{
	InputCoding_Linear,
	InputCoding_uLaw,
	InputCoding_ALaw
};

enum
{
	InputDFM_1Complement,
	InputDFM_2Complemnet,
	InputDFM_Signed,
	InputDFM_Unsigned
};*/

//-----------------------------
//	HCI Command
//-----------------------------

typedef struct 
{
	UCHAR LAP[3];
	UCHAR InquiryLen;
	UCHAR NumResp;
} PACK COMMAND_INQUIRY, *PCOMMAND_INQUIRY;

typedef struct 
{
	UCHAR	BdAddress[SIZE_OF_BD_ADDRESS];
	USHORT	PktType;
	UCHAR	PageScanRepMode;
	UCHAR	Reserved;
	USHORT	ClockOffset;
	UCHAR	AllowRoleSwitch; 
} PACK COMMAND_CREATE_CONNECTION, *PCOMMAND_CREATE_CONNECTION;

typedef struct 
{
	USHORT ConnHandle;
	ULONG TxBdWdth;
	ULONG RxBdWdth;
	USHORT MaxLatency;
	USHORT VoiceSettg;
	UCHAR  Retranx;
	USHORT PktType;
} PACK COMMAND_SETUP_SCO_CONN, *PCOMMAND_SETUP_SCO_CONN;

typedef struct 
{
	USHORT ConnHandle;
	USHORT PacketType;
} PACK COMMAND_CHANGE_PKT_TYPE, *PCOMMAND_CHANGE_PKT_TYPE;

typedef struct 
{
	USHORT ConnHandle;
	USHORT FlushTimeout; // =0, no timeout
} PACK COMMAND_AUTOMATIC_FLUSH_TO, *PCOMMAND_AUTOMATIC_FLUSH_TO;

typedef struct 
{
	UCHAR FilterType;
	UCHAR FilterConditionType;
	UCHAR Condition[8]; // condition ranges from 0-7 bytes depends on FilterType & FilterConditionType
} PACK COMMAND_SET_EVENT_FILTER, *PCOMMAND_SET_EVENT_FILTER;



typedef struct 
{
	USHORT	CmdCode:10;
	USHORT	CmdGroup:6;
	UCHAR   ParamLength; // size of buffer Param
	UCHAR   Param[1];
} PACK HCI_COMMAND_STRUCT, *PHCI_COMMAND_STRUCT;


//---------------------------------
//	HCI Events 
//---------------------------------
typedef struct 
{
	UCHAR BdAddress[SIZE_OF_BD_ADDRESS];
	UCHAR PageScanRepMode;
	UCHAR Reserved[2];
	UCHAR COD[3];
	USHORT ClockOffset;
} PACK INQUIRY_RESP, *PINQUIRY_RESP;

typedef struct 
{	
	UCHAR Status;
	UCHAR NumResponses;
	INQUIRY_RESP RespDev[MAX_INQUIRY_RESPONSES];
} PACK EVENT_INQUIRY_RESULT, *PEVENT_INQUIRY_RESULT;

typedef struct 
{
	USHORT	ConnHandle;
	UCHAR	BdAddress[SIZE_OF_BD_ADDRESS];
	UCHAR	LinkType;
	UCHAR	EncrypMode;
} PACK BT_CONNECTION, *PBT_CONNECTION;

typedef struct 
{
 	UCHAR	Status;
	BT_CONNECTION BtConn;
} EVENT_CONNECTION_COMPLETE, *PEVENT_CONNECTION_COMPLETE;

typedef struct 
{
	UCHAR BdAddress[SIZE_OF_BD_ADDRESS];
	UCHAR COD[3];
	UCHAR LinkType;
} PACK EVENT_CONNECTION_REQUEST, *PEVENT_CONNECTION_REQUEST;


typedef struct 
{
	UCHAR	NumHciCmdPkt;
	USHORT	OCF:10;
	USHORT	OGF:6;	
//	UCHAR	OCF;
//	UCHAR	OGF;	
	UCHAR	ParamLength;
	UCHAR*	pParam;
} PACK EVENT_COMMAND_COMPLETE, *PEVENT_COMMAND_COMPLETE;

typedef struct 
{
 	UCHAR Status;
	UCHAR NumHciCmdPkt;
	USHORT	OCF:10;
	USHORT	OGF:6;	
//	UCHAR	OCF;
//	UCHAR	OGF;	
} PACK  EVENT_COMMAND_STATUS, *PEVENT_COMMAND_STATUS;

typedef struct 
{
	UCHAR	Status;
	USHORT	ConnHandle;
	USHORT	PacketType;
} PACK EVENT_PKT_TYPE_CHANGED, *PEVENT_PKT_TYPE_CHANGED;

typedef struct 
{
	USHORT	ConnHandle;
	UCHAR	BdAddress[SIZE_OF_BD_ADDRESS];
	UCHAR	LinkType;
	UCHAR	TxInterval;
	UCHAR	ReTxWin;
	USHORT	RxPktLength;
	USHORT	TxPktLength;
	UCHAR	AirMode;
} PACK SCO_CONNECTION, *PSCO_CONNECTION;

typedef struct 
{
	UCHAR Status;
	SCO_CONNECTION ScoConn;
} PACK EVENT_SCO_CONN_COMPLETE, *PEVENT_SCO_CONN_COMPLETE;

typedef struct 
{
	UCHAR	Status;
	UCHAR	BdAddress[SIZE_OF_BD_ADDRESS];
} PACK EVENT_READ_BD_ADDR, *PEVENT_READ_BD_ADDR;

typedef struct 
{
	UCHAR	Status;
	USHORT  MaxAclPktLength;
 	UCHAR	MaxScoPktLength;
	USHORT	TotalNumAclPkt;
	USHORT	TotalNumScoPkt;
} PACK EVENT_BUFFER_SIZE_INFO, *PEVENT_BUFFER_SIZE_INFO;

typedef struct 
{
 	USHORT  ConnHandle;
 	UCHAR	MaxSlots;
} PACK EVENT_MAX_SLOT_CHANGED, *PEVENT_MAX_SLOT_CHANGED;


typedef struct 
{
	UCHAR	Status;
	USHORT  ConnHandle;
	UCHAR	Reason;
} PACK EVENT_DISCONNECT_COMPLETE, *PEVENT_DISCONNECT_COMPLETE;

typedef struct
{
	USHORT  ConnHandle;
	UCHAR	Reason;
} COMMAND_DISCONNECT, *PCOMMAND_DISCONNECT;


// ACL Data Packet
typedef struct 
{
	USHORT	ConnHandle:12;
	USHORT	PktBdry:2;
	USHORT	Broadcast:2;
	USHORT	TotalLength;
	USHORT	DataLength;
	USHORT	CID;
	UCHAR	Data[1]; 
} PACK ACL_DATA_PACKET, *PACL_DATA_PACKET;

// SCO Data Packet
typedef struct 
{
	USHORT	ScoHandle:12;
	USHORT  Reserved:4;
	UCHAR	DataLength;
 	UCHAR	Data[1]; 
} PACK SCO_DATA_PACKET, *PSCO_DATA_PACKET;


typedef struct
{
	USHORT	ConnHandle:12;
	USHORT	Reserved:4;
	USHORT	NumOfCompletePackets; 
} NUMBER_OF_COMPLETED_PACKETS_PER_HANDLE, *PNUMBER_OF_COMPLETED_PACKETS_PER_HANDLE;


typedef struct
{
	USHORT	ConnHandle;
	USHORT	RoleSwitchMode:1;
	USHORT	HoldMode:1;
	USHORT	SniffMode:1;
	USHORT	ParkMode:1;
	USHORT	Reserved:12;
} COMMAND_WRITE_LP, *PCOMMAND_WRITE_LP;

typedef struct
{
	USHORT	ConnHandle;
} COMMAND_READ_LP, *PCOMMAND_READ_LP;


typedef struct
{
	UCHAR	Status;
	USHORT	ConnHandle;
	USHORT	RoleSwitchMode:1;
	USHORT	HoldMode:1;
	USHORT	SniffMode:1;
	USHORT	ParkMode:1;
	USHORT	Reserved:12;
} EVENT_READ_LP, *PEVENT_READ_LP;

  


typedef struct
{
	UCHAR	NumberOfHandle; 
	NUMBER_OF_COMPLETED_PACKETS_PER_HANDLE	 Data[1];
} EVENT_NUMBER_OF_COMPLETED_PACKETS, *PEVENT_NUMBER_OF_COMPLETED_PACKETS;

//HCI_EVENT_MODE_CHANGE
typedef struct
{
	UCHAR	Status;
	USHORT  ConnHandle;
	UCHAR	CurrentMode;
	USHORT  Interval;
} EVENT_MODE_CHANGE, *PEVENT_MODE_CHANGE;

// Generic Event Structure
typedef struct 
{
	BOOL  IsDataPkt;
	UCHAR EventCode;	// ignored if IsDataPkt is TRUE
	UCHAR ParamLength;	// ignored if IsDataPkt is TRUE

	union
	{
		UCHAR						EventData[1];
		EVENT_INQUIRY_RESULT		InquiryResult;
		EVENT_CONNECTION_COMPLETE	ConnComplete;
		EVENT_CONNECTION_REQUEST	ConnRequest;
		EVENT_COMMAND_STATUS		CmdStatus;
		EVENT_COMMAND_COMPLETE		CmdComplete;
		EVENT_PKT_TYPE_CHANGED		PktTypeChanged;
		EVENT_SCO_CONN_COMPLETE		ScoConnComplete;
		EVENT_MAX_SLOT_CHANGED		MaxSlotChanged;
		EVENT_DISCONNECT_COMPLETE	DisconnectComplete; 
//		EVENT_BUFFER_SIZE_INFO		BufSizeInfo;
//		EVENT_READ_BD_ADDR			GetBdAddr;
		ACL_DATA_PACKET				AclData;
		SCO_DATA_PACKET				ScoData; 
		EVENT_NUMBER_OF_COMPLETED_PACKETS	NumOfCompletePackets;
		EVENT_MODE_CHANGE			ModeChange;
		EVENT_READ_LP				LinkPolice;
	};
} PACK HCI_EVENT_STRUCT, *PHCI_EVENT_STRUCT;















