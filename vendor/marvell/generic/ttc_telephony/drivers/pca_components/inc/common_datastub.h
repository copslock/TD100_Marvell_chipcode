/*--------------------------------------------------------------------------------------------------------------------
   (C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
   -------------------------------------------------------------------------------------------------------------------*/

/******************************************************************************
 *
 *  COPYRIGHT (C) 2005 Intel Corporation.
 *
 *  This software as well as the software described in it is furnished under
 *  license and may only be used or copied in accordance with the terms of the
 *  license. The information in this file is furnished for informational use
 *  only, is subject to change without notice, and should not be construed as
 *  a commitment by Intel Corporation. Intel Corporation assumes no
 *  responsibility or liability for any errors or inaccuracies that may appear
 *  in this document or any software that may be provided in association with
 *  this document.
 *  Except as permitted by such license, no part of this document may be
 *  reproduced, stored in a retrieval system, or transmitted in any form or by
 *  any means without the express written consent of Intel Corporation.
 *
 *  FILENAME:   data_channel.h
 *  PURPOSE:    data_channel API used by stub
 *
 *****************************************************************************/

#ifndef __COMMON_DATASTUB_H__
#define __COMMON_DATASTUB_H__

//#include <ci_dat.h>
#include <linux_types.h>

#define CCIDATASTUB_MAJOR 246
#define CCIDATASTUB_IOC_MAGIC CCIDATASTUB_MAJOR
#define CCIDATASTUB_CONN_CONF _IOW(CCIDATASTUB_IOC_MAGIC, 2, int)
#define CCIDATASTUB_ACT_CONF _IOW(CCIDATASTUB_IOC_MAGIC, 3, int)
#define CCIDATASTUB_START  _IOW(CCIDATASTUB_IOC_MAGIC, 1, int)
#define CCIDATASTUB_DATAHANDLE  _IOW(CCIDATASTUB_IOC_MAGIC, 4, int)
#define CCIDATASTUB_DATASVGHANDLE  _IOW(CCIDATASTUB_IOC_MAGIC, 5, int)
#define CCIDATASTUB_LINKSTATUS  _IOW(CCIDATASTUB_IOC_MAGIC, 6, int)
#define CCIDATASTUB_CHNOK  _IOW(CCIDATASTUB_IOC_MAGIC, 7, int)
#define CCIDATASTUB_CHOK  _IOW(CCIDATASTUB_IOC_MAGIC, 8, int)
#define CCIDATASTUB_GCFDATA _IOW(CCIDATASTUB_IOC_MAGIC, 9, int)
#define CCIDATASTUB_REMOVE_ALLCH  _IOW(CCIDATASTUB_IOC_MAGIC, 10, int)
#define CCIDATASTUB_CS_CHNOK  _IOW(CCIDATASTUB_IOC_MAGIC, 11, int)
#define CCIDATASTUB_CS_CHOK  _IOW(CCIDATASTUB_IOC_MAGIC, 12, int)
#define CCIDATASTUB_GCFDATA_REMOTE _IOW(CCIDATASTUB_IOC_MAGIC, 13, int)

#define RETURN_ETH_HDR { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00 }

#define CCINET_MAJOR 241
#define CCINET_IOC_MAGIC CCINET_MAJOR
#define CCINET_IP_ENABLE  _IOW(CCINET_IOC_MAGIC, 1, int)
#define CCINET_IP_DISABLE  _IOW(CCINET_IOC_MAGIC, 2, int)
#define CCINET_ALL_IP_DISABLE  _IOW(CCINET_IOC_MAGIC, 3, int)
#define CIDATATTY_TTY_MINORS            3
#if 0
typedef struct _ttyfd
{
	int fd;
	BOOL occupied;
	unsigned char cid;
}TTYFD;
#endif
typedef struct directipconfig_tag {
	INT32 dwContextId;
	INT32 dwProtocol;
	struct
	{
		INT32 inIPAddress;
		INT32 inPrimaryDNS;
		INT32 inSecondaryDNS;
		INT32 inDefaultGateway;
		INT32 inSubnetMask;
	} ipv4;
} DIRECTIPCONFIG;
typedef struct _ipconfiglist
{
	DIRECTIPCONFIG directIpAddress;
	struct _ipconfiglist *next;
}DIRECTIPCONFIGLIST;
struct ccitx_packet {
	char  *pktdata;
	int length;
	unsigned char cid;
};
typedef struct _cci_tx_buf_list
{
	struct ccitx_packet tx_packet;
	struct _cci_tx_buf_list *next;
}TXBUFNODE;
typedef enum
{       // com events
	DATA_CHAN_INIT_STATE = 0,
	DATA_CHAN_NOTREADY_STATE,
	DATA_CHAN_READY_STATE,
	DATA_CHAN_SNUMSTATES
}DATACHANSTATES;

typedef UINT8 CiDatConnType;

/* Connection Information */
// ICAT EXPORTED STRUCTURE
typedef struct CiDatConnInfo_struct {
	CiDatConnType type;     /* connection type */
	UINT32 id;              /* link id: call id for CS connection; context id for PS connection */
} CiDatConnInfo;
typedef enum
{
	PDP_PPP = 0,
	PDP_DIRECTIP = 1,
	PDP_PPP_MODEM = 2,
	CSD_RAW = 3
}SVCTYPE;

typedef enum ATCI_CONNECTION_TYPE {
	ATCI_LOCAL,
	ATCI_REMOTE = 2
} _AtciConnectionType;

struct datahandle_obj
{
	CiDatConnInfo m_datConnInfo;
	BOOL m_fOptimizedData;
	INT16 m_maxPduSize;
	CiDatConnType m_connType;
	UINT32 m_cid;
	DATACHANSTATES chanState;
	UINT16 m_dwCurSendPduSeqNo;
	UINT16 m_dwCurSendReqHandle;
	SVCTYPE pdptype;
	UINT32 connectionType;
//	 OSAFlagRef writeSync;
};

typedef struct _datahandle
{
	struct datahandle_obj handle;;
	struct _datahandle *next;
}DATAHANDLELIST;

typedef struct
{
	UINT32 cid;
	UINT32 len;        //length of databuf
	UINT8 *databuf;
}GCFDATA;

#endif /* __COMMON_DATASTUB_H__ */

