/******************************************************************************
*(C) Copyright 2008 Marvell International Ltd.
* All Rights Reserved
******************************************************************************/
/*--------------------------------------------------------------------------------------------------------------------
 *  -------------------------------------------------------------------------------------------------------------------
 *
 *  Filename: teldat.c
 *
 *  Description: Telephony Implementation for AT Command for CI_SG_ID_DEV.
 *
 *  History:
 *   Jan 17, 2008 - Creation of file
 *
 *  Notes:
 *
 ******************************************************************************/

/******************************************************************************
*    Include files
******************************************************************************/
#include "ci_api.h"
#include "teldat.h"
#include "telps.h"
#include "teldev.h"
#include "utlMalloc.h"
#include "telconfig.h"
#include "telatparamdef.h"
#include "telatci.h"
#include "telutl.h"
#include "telcontroller.h"
#include "ci_dat.h"
#include <sys/ioctl.h>
#include <fcntl.h>

/*  Globals */
UINT32 g_CGSENDSwitch = 1;
UINT32 g_TGSINKSwitch = 1;



/*  Local variabls */
static INT32 gSinkCid;
static INT32 gSinkPacketSize;
static INT16 gSinkPduSize;
static INT32 gSinkPort;
static CHAR gSinkAddress[ATCI_PS_ADDRESS_SIZE + ATCI_NULL_TERMINATOR_LENGTH];
static INT8 gSinkIpDataBuf[ATCI_IP_MTU];
static INT32 sinkPacketId = 0;
static INT32 sinkRemainingPacketSize = 0;

INT32 gSinkCount = 0;
BOOL gNewSinkPacket = FALSE;
BOOL gFinishOnePacket = FALSE;

#define TIOPPPON _IOW('T', 208, int)
#define TIOPPPOFF _IOW('T', 209, int)
#define TIOPPPONCSD _IOW('T', 211, int)

extern CiServiceHandle gAtciSvgHandle[CI_SG_NUMIDS + 1];
extern AtciCurrentPsCntxList gCIDList;
extern AtciCurrentCallsList gCurrentCallsList;
int cidata_fd = 0;

int cinetdevfd = 0;
int cctdatafd = 0;
BOOL giUseUlDlOpt = FALSE;


void  createSinkIpFrame( UINT32 atHandle );
void  sendSinkData( UINT32 atHandle, UINT8 connType );
void  createUDPPacket( UINT8 * pDatabuf, UINT32 len );


/************************************************************************************
 * F@: ciCGSend - GLOBAL API for GCF AT+CGSEND -command
 *
 */
RETURNCODE_T  ciCGSend(            const utlAtParameterOp_T op,
				   const char                      *command_name_p,
				   const utlAtParameterValue_P2c parameter_values_p,
				   const size_t num_parameters,
				   const char                      *info_text_p,
				   unsigned int                    *xid_p,
				   void                            *arg_p)
{
	UNUSEDPARAM(command_name_p)
	UNUSEDPARAM(num_parameters)
	UNUSEDPARAM(info_text_p)

	CiReturnCode ret = CIRC_FAIL;
	RETURNCODE_T rc = INITIAL_RETURN_CODE;

	INT32 datalen = 0, cid;
	static GCFDATA gcfdata;
	/*
	 *  Put parser index into the variable
	 */
	CiRequestHandle reqHandle;
	TelAtParserID sAtpIndex = *(TelAtParserID *)arg_p;
	reqHandle = MAKE_AT_HANDLE( sAtpIndex );
	*xid_p = reqHandle;
	DBGMSG("[%s] Line(%d): reqHandle(%d),op(%d) \n", __FUNCTION__, __LINE__, reqHandle, op);
	F_ENTER();

	/*
	**  Check the format of the request.
	*/
	switch (op)
	{
	case TEL_EXT_GET_CMD:            /* AT+CGSEND?  */
	{
		ret = ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+CGSEND: (0-20000)\r\n");
		break;
	}

	case TEL_EXT_TEST_CMD:            /* AT+CGSEND=? */
	{
		ret = ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+CGSEND: (0-20000)\r\n");
		break;
	}

	case TEL_EXT_SET_CMD:              /* AT+CGSEND= */
	{
		if ( getExtValue(parameter_values_p, 0, &datalen, 0, 20000, 0) == TRUE )
		{
			DBGMSG("[%s] Line(%d): datalen(%d) \n", __FUNCTION__, __LINE__, datalen);

			for(cid = 0; cid < CI_PS_MAX_PDP_CTX_NUM; cid++)
			{
				if((gCIDList.cInfo[cid].reqHandle != INVALID_REQ_HANDLE) &&
					(((sAtpIndex == TEL_MODEM_AT_CMD_ATP) && (GET_ATP_INDEX(gCIDList.cInfo[cid].reqHandle) == TEL_MODEM_AT_CMD_ATP)) ||
					((sAtpIndex != TEL_MODEM_AT_CMD_ATP) && (GET_ATP_INDEX(gCIDList.cInfo[cid].reqHandle) != TEL_MODEM_AT_CMD_ATP))))
					break;
			}
			gcfdata.cid = cid;
			if (gcfdata.cid >= CI_PS_MAX_PDP_CTX_NUM)
			{
				ERRMSG("[%s]no appropriate active cid\n", __FUNCTION__);
				break;
			}
			while (datalen > 0)
			{
				if (datalen > 1500)               //CI_STUB_DAT_BUFFER_SIZE - sizeof(CiDatPduInfo))
				{
					gcfdata.len = 1500;               //CI_STUB_DAT_BUFFER_SIZE - sizeof(CiDatPduInfo) ;
				}
				else
				{
					gcfdata.len = datalen;
				}

				gcfdata.databuf = (UINT8 *)malloc(gcfdata.len);
				if(sAtpIndex == TEL_MODEM_AT_CMD_ATP)
					ioctl(cidata_fd, CCIDATASTUB_GCFDATA_REMOTE, &gcfdata);
				else
					ioctl(cidata_fd, CCIDATASTUB_GCFDATA, &gcfdata);
				free(gcfdata.databuf);
				datalen = datalen - gcfdata.len;
			}

			ret = ATRESP(reqHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		}
		else
		{

			DBGMSG("[%s] Line(%d): error to get size of +CGSEND \n", __FUNCTION__, __LINE__);
			/* **  There was a problem extracting the functionality. */
			ret = ATRESP( reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL );
		}
		break;
	}

	case TEL_EXT_ACTION_CMD:           /* AT+CGSEND */
	default:
	{
		ret = ATRESP( reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL );
		break;
	}
	}


	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	F_LEAVE();

	return(rc);

}





/* version 2: use IOCTL to cistub, instead of through CIREQUEST. */
/* NEW IMPLEMENTATION FOR AT*TGSINK */
/************************************************************************************
 * F@: ciGpTGSINK - GLOBAL API for GCF AT*TGSINK  -command
 *
 */
RETURNCODE_T  ciGpTGSINK(            const utlAtParameterOp_T op,
				     const char                      *command_name_p,
				     const utlAtParameterValue_P2c parameter_values_p,
				     const size_t num_parameters,
				     const char                      *info_text_p,
				     unsigned int                    *xid_p,
				     void                            *arg_p)
{
	UNUSEDPARAM(command_name_p)
	UNUSEDPARAM(num_parameters)
	UNUSEDPARAM(info_text_p)

	CiReturnCode ret = CIRC_FAIL;
	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	INT32 cid;
	INT32 packetSize;
	INT32 pduCount;
	char tgsinkBuf[200];
	static GCFDATA gcfdata;
	F_ENTER();
	/*
	 *  Put parser index into the variable
	 */
	CiRequestHandle reqHandle;
	TelAtParserID sAtpIndex = *(TelAtParserID *)arg_p;
	reqHandle = MAKE_AT_HANDLE( sAtpIndex );
	*xid_p = reqHandle;
	DBGMSG("%s: reqHandle = %d.\n", __FUNCTION__, reqHandle);

	switch (op)
	{
	case TEL_EXT_TEST_CMD:         /* AT*TGSINK=? */
	{
		sprintf(tgsinkBuf, "*TGSINK: (1-8),(0-%d),(1-20)\r\n", ATCI_MAX_PACKET_SIZE);
		ret = ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, tgsinkBuf);
		break;
	}

	case TEL_EXT_SET_CMD:         /* AT*TGSINK=XX,XX,XX */
	{
		/*	 **  Extract the arguments starting with the CID. */

		if (( getExtValue(parameter_values_p, 0, &cid, 1, CI_PS_MAX_PDP_CTX_NUM, TEL_AT_PS_CID_VAL_DEFAULT) == TRUE ) &&
			(gCIDList.cInfo[cid - 1].reqHandle != INVALID_REQ_HANDLE) &&
			(((sAtpIndex == TEL_MODEM_AT_CMD_ATP) && (GET_ATP_INDEX(gCIDList.cInfo[cid - 1].reqHandle) == TEL_MODEM_AT_CMD_ATP)) ||
			((sAtpIndex != TEL_MODEM_AT_CMD_ATP) && (GET_ATP_INDEX(gCIDList.cInfo[cid - 1].reqHandle) != TEL_MODEM_AT_CMD_ATP))))
		{
			DBGMSG("[%s] Line(%d): cid(%d) \n", __FUNCTION__, __LINE__, cid);
			if (( getExtValue(parameter_values_p, 1, &packetSize, 0, ATCI_MAX_PACKET_SIZE, ATCI_MAX_PDU_SIZE) == TRUE ) &&
			    ( packetSize <= ATCI_MAX_PACKET_SIZE ) )                             /* Check the PacketSize */
			{
				DBGMSG("[%s] Line(%d): packetSize(%d) \n", __FUNCTION__, __LINE__, packetSize);
				if (( getExtValue(parameter_values_p, 2, &pduCount, 1, ATCI_DEFAULT_PDU_COUNT, 1) == TRUE ) &&
				    ( pduCount <= ATCI_DEFAULT_PDU_COUNT ) )                             /* Check the PacketCount */
				{
					DBGMSG("[%s] Line(%d): pduCount(%d) \n", __FUNCTION__, __LINE__, pduCount);

					gSinkCid = cid - 1;
					gSinkPacketSize = packetSize;
					gSinkCount = pduCount;

					gcfdata.cid = gSinkCid;

					while(gSinkCount){
						gFinishOnePacket = FALSE;
						while(!gFinishOnePacket){
							createSinkIpFrame(reqHandle);
							gcfdata.len = gSinkPduSize;
							gcfdata.databuf = (UINT8 *)gSinkIpDataBuf;
							if(sAtpIndex == TEL_MODEM_AT_CMD_ATP)
								ioctl(cidata_fd, CCIDATASTUB_GCFDATA_REMOTE, &gcfdata);
							else
								ioctl(cidata_fd, CCIDATASTUB_GCFDATA, &gcfdata);
						}
					}

					ret = ATRESP(reqHandle, ATCI_RESULT_CODE_OK, 0, NULL);
				}
			}
		}

		break;
	}

	case TEL_EXT_GET_CMD:            /* AT*TGSINK?  */
	case TEL_EXT_ACTION_CMD:
	default:
		break;
	}


	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	F_LEAVE();
	return(rc);

}

/************************************************************************************
 * F@: ciGpCGSink - GLOBAL API for GCF AT+CGSINK  -command
 *
 */
RETURNCODE_T  ciGpCGSink(            const utlAtParameterOp_T op,
				     const char                      *command_name_p,
				     const utlAtParameterValue_P2c parameter_values_p,
				     const size_t num_parameters,
				     const char                      *info_text_p,
				     unsigned int                    *xid_p,
				     void                            *arg_p)
{
	UNUSEDPARAM(command_name_p)
	UNUSEDPARAM(num_parameters)
	UNUSEDPARAM(info_text_p)

	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	BOOL cmdValid = FALSE;
	INT32 cid;
	INT32 packetSize;
	INT32 pduCount;
	INT32 addrStrLen;
	char cgsinkBuf[200];

	/*
	 *  Put parser index into the variable
	 */
	CiRequestHandle reqHandle;
	TelAtParserID sAtpIndex = *(TelAtParserID *)arg_p;
	reqHandle = MAKE_AT_HANDLE( sAtpIndex );
	*xid_p = reqHandle;
	DBGMSG("%s: reqHandle = %d.\n", __FUNCTION__, reqHandle);


	switch (op)
	{
	case TEL_EXT_TEST_CMD:         /* AT+CGSINK=? */
	{
		sprintf(cgsinkBuf, "CGSINK = [CID 0-%d],[packetSize 0-%d],[PacketCount 1-20],[Address],[UDPPort]\r\n",
			CI_PS_MAX_PDP_CTX_NUM, ATCI_MAX_PDU_SIZE);
		ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, cgsinkBuf);
		cmdValid = TRUE;
		break;
	}

	case TEL_EXT_SET_CMD:         /* AT+CGSINK=XX,XX,XX */
	{
		/*
		**  Extract the arguments starting with the CID.
		*/
		if (( getExtValue(parameter_values_p, 0, &cid, 0, 255, ATCI_DEFAULT_CID) == TRUE ) &&
		    ( cid > 0 ) && ( cid <= CI_PS_MAX_PDP_CTX_NUM ))
		{
			if (( getExtValue(parameter_values_p, 1, &packetSize, 0, 200, ATCI_DEFAULT_PDU_SIZE) == TRUE ) &&
			    ( packetSize <= ATCI_MAX_PACKET_SIZE ) )                             /* Check the PacketSize */
			{
				if (( getExtValue(parameter_values_p, 2, &pduCount, 1, 200, ATCI_DEFAULT_PDU_COUNT) == TRUE ) &&
				    ( pduCount <= ATCI_DEFAULT_PDU_COUNT ) )                             /* Check the PacketCount */
				{
					/* Pdp destination address ("255.255.255.255") */
					if ( getExtString(parameter_values_p, 3, gSinkAddress, ATCI_PS_ADDRESS_SIZE, (INT16 *)&addrStrLen, NULL) == TRUE )
					{
						/* UDP port */
						if ( getExtValue(parameter_values_p, 4, &gSinkPort, 0, 255, ATCI_DEFAULT_UDP_PORT) == TRUE )
						{
							/*
							**  Set the connection information for CI.
							*/
							gSinkCid = cid - 1;
							gSinkPacketSize = packetSize;
							gSinkCount = pduCount;
							gNewSinkPacket = TRUE;

							/* check if ready for data transfer */
							if ( gCIDList.currCntx[sAtpIndex].currCid == gSinkCid )
							{
								/* start sending data */
								sendSinkData(reqHandle, CI_DAT_CONNTYPE_PS);

								cmdValid = TRUE;
							}
						}                       /* if ( getExtString ((CommandLine_t*)parameter_values_p, X, gSinkPort */
					}                               /* if ( getExtString ((CommandLine_t*)parameter_values_p, X, (CHAR*)address  */
				}                                       /* if (( getExtValue(parameter_values_p, X, &pduCount,  */
			}                                               /* if (( getExtValue(parameter_values_p, X, &packetSize, */
		}                                                       /* if (( getExtValue(parameter_values_p, X, &cid, */

		break;
	}

	case TEL_EXT_GET_CMD:            /* AT+CGSINK?  */
	case TEL_EXT_ACTION_CMD:
	default:
		break;
	}

	if ( cmdValid == TRUE )
	{
		rc = utlSUCCESS;
	}


	return(rc);

}

/************************************************************************************
 * F@: sendSinkData
 *
 */
void  sendSinkData( UINT32 atHandle, UINT8 connType )
{
	CiReturnCode ret = CIRC_FAIL;

	static CiDatPrimSendDataOptReq sinkDataSendReq;
	//static CiDatPdu               sinkSendPdu;
	static UINT16 sinkSeqNo = 0;
	static UINT16 sinkCurrentBufPosition = 0;
	char sendDataBuf[500];


	/* create the IP packet if new packet to send (i.e. not new packet segment) */
	if ( gNewSinkPacket == TRUE )
	{
		createSinkIpFrame(atHandle);

		gNewSinkPacket = FALSE;
	}

	/*
	**  Set the PDU information for the CI.
	*/
	if (connType == CI_DAT_CONNTYPE_PS)
	{
		sinkDataSendReq.connInfo.id      = gSinkCid;
		sinkDataSendReq.connInfo.type    = CI_DAT_CONNTYPE_PS;
	}
	else
	{
		sinkDataSendReq.connInfo.id      = gCurrentCallsList.currCall[GET_ATP_INDEX(atHandle)].currCallId;
		sinkDataSendReq.connInfo.type    = CI_DAT_CONNTYPE_CS;
	}

#if 0
	sinkDataSendReq.sendPdu = &sinkSendPdu;
	sinkSendPdu.seqNo  = sinkSeqNo;
	sinkSendPdu.type   = CI_DAT_TYPE_IP;
	sinkSendPdu.data   = &gSinkIpDataBuf[sinkCurrentBufPosition];
#endif

	sinkDataSendReq.sendPdu.seqNo = sinkSeqNo;
	sinkDataSendReq.sendPdu.type = CI_DAT_TYPE_IP;
	sinkDataSendReq.sendPdu.data = (UINT8 *)&gSinkIpDataBuf[sinkCurrentBufPosition];

	/* check size of data to send */
	if ( gSinkPduSize <= CI_MAX_CI_DATA_PDU_SIZE )
	{
		sprintf(sendDataBuf, "Send IP Data: packet #%d, segment size=%d\r\n", gSinkCount + 1, gSinkPduSize );

		/* set size of data to be sent in this request */
		//sinkSendPdu.len = gSinkPduSize;
		sinkDataSendReq.sendPdu.len = gSinkPduSize;

		/* all can be sent in one request */
		gSinkPduSize = 0;

		/* update position for data to be sent next time */
		sinkCurrentBufPosition = 0;

		/* this is last segment of the IP packet */
		//sinkSendPdu.isLast = TRUE;
		sinkDataSendReq.sendPdu.isLast = TRUE;

		/* reset the segment number in the IP data */
		sinkSeqNo = 0;

		/* if more packets to send, new IP packet will be created for next CI request */
		gNewSinkPacket = TRUE;
	}
	else
	{
		sprintf(sendDataBuf, "Send IP Data: packet #%d, segment size=%d\r\n", gSinkCount, CI_MAX_CI_DATA_PDU_SIZE );

		/* set size of data to be sent in this request */
		//sinkSendPdu.len = CI_MAX_CI_DATA_PDU_SIZE;
		sinkDataSendReq.sendPdu.len = CI_MAX_CI_DATA_PDU_SIZE;

		/* more to be sent next time */
		gSinkPduSize = gSinkPduSize - CI_MAX_CI_DATA_PDU_SIZE;

		/* update position for data to be sent next time */
		sinkCurrentBufPosition = sinkCurrentBufPosition + CI_MAX_CI_DATA_PDU_SIZE;

		/* update the segment number in the IP data */
		sinkSeqNo++;

		/* this is not the last segment of the IP packet */
		//sinkSendPdu.isLast = FALSE;
		sinkDataSendReq.sendPdu.isLast = FALSE;
	}

	/*
	**  Send the CI request.
	*/
	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_DAT], CI_PS_PRIM_SEND_DATA_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_PS_PRIM_SEND_DATA_REQ), (void *)&sinkDataSendReq );

}

/************************************************************************************
 * F@: createSinkIPMtu
 *
 * insert the IP header
 */
void  createSinkIpFrame( UINT32 atHandle )
{
	CHAR                          *pAddr;
	INT32 i;
	INT32 headerChecksum16 = 0;
	INT32 sinkHeaderSize = ATCI_IP_LEN;


	/* fill the data buffer with NULL value */
	memset( gSinkIpDataBuf, 0x00, ATCI_IP_MTU );

	/* first segment of the IP packet */
	if ( sinkRemainingPacketSize == 0 )
	{

		/* init size left to be transmited from the IP packet */
		sinkRemainingPacketSize = gSinkPacketSize;

		/* new IP , new ID */
		sinkPacketId++;

		/* header contains UDP info */
		sinkHeaderSize += ATCI_UDP_LEN;

		/* fill in UDP specific info */
		gSinkIpDataBuf[20] = 0x00;
		gSinkIpDataBuf[21] = 0x00;                                              /* Source (optional) */
		gSinkIpDataBuf[22] = gSinkPort >> 8;                                    /* UDP +  Source=0  | Dest=9  */
		gSinkIpDataBuf[23] = gSinkPort & 0x00ff;
		gSinkIpDataBuf[24] = (ATCI_UDP_LEN + gSinkPacketSize) >> 8;             /* Length */
		gSinkIpDataBuf[25] = (ATCI_UDP_LEN + gSinkPacketSize) & 0x00ff;
		gSinkIpDataBuf[26] = 0x00;
		gSinkIpDataBuf[27] = 0x00; /* Checksum (optional) */
	}

	/* get size of current IP packet, relative to the size to be still transmitted
	 * and maximum transmit unit size */
	if ( sinkRemainingPacketSize <= (ATCI_IP_MTU - sinkHeaderSize)  )
	{
		gSinkPduSize = sinkRemainingPacketSize + sinkHeaderSize;

		/* a whole packet has been transmitted */
		sinkRemainingPacketSize = 0;

		/* update number of packets still to be sent */
		gFinishOnePacket = TRUE;
		gSinkCount--;
	}
	else
	{
		gSinkPduSize = ATCI_IP_MTU;
		sinkRemainingPacketSize = sinkRemainingPacketSize - (ATCI_IP_MTU - sinkHeaderSize);
	}

	gSinkIpDataBuf[0] = 0x45;                                                                                                       /* IP +  Version=4 */
	gSinkIpDataBuf[1] = 0x00;                                                                                                       /* Routine   |D=0|T=0|R=0| 0 | 0  |  00 */
	gSinkIpDataBuf[2] = gSinkPduSize  >> 8;                                                                                         /* Total Length=XXXX */
	gSinkIpDataBuf[3] = gSinkPduSize  & 0x00ff;
	gSinkIpDataBuf[4] = sinkPacketId >> 8;                                                                                          /* Identification=XXXX */
	gSinkIpDataBuf[5] = sinkPacketId & 0x00ff;
	gSinkIpDataBuf[6] = (( ATCI_UDP_LEN + ATCI_IP_LEN + gSinkPacketSize - sinkRemainingPacketSize - gSinkPduSize ) / 8) >> 8;       /* DF=0 MF=X Fragment Offset=XX */
	if ( sinkRemainingPacketSize )
		gSinkIpDataBuf[6] |= 0x20;
	gSinkIpDataBuf[7] = (( ATCI_UDP_LEN + ATCI_IP_LEN + gSinkPacketSize - sinkRemainingPacketSize - gSinkPduSize ) / 8) & 0x00ff;
	gSinkIpDataBuf[8] = 0x0F;                       /* Time to Live=15 |  0F */
	gSinkIpDataBuf[9]  = 0x11;                      /* Protocol=UDP  |  11 */
	gSinkIpDataBuf[10] = 0x00;                      /* Header Checksum   OK |  77 EF */
	gSinkIpDataBuf[11] = 0x00;

	/* create a default address - Do I need it or can I leave NULL bytes */
	CHAR defaultAddress[] = "192.168.1.101";

	/* Copy Source address; ex: 018.052.086.120  |  12 34 56 78 */
	if ( gCIDList.currCntx[GET_ATP_INDEX(atHandle)].currCid == gSinkCid )
	{
		pAddr = (CHAR *)gCIDList.cInfo[gSinkCid].pdpAddress;
	}
	else
	{
		pAddr = &defaultAddress[0];
	}

	gSinkIpDataBuf[12] = atoi((char *)pAddr); while (*pAddr) if (*pAddr++ == '.') break;
	gSinkIpDataBuf[13] = atoi((char *)pAddr); while (*pAddr) if (*pAddr++ == '.') break;
	gSinkIpDataBuf[14] = atoi((char *)pAddr); while (*pAddr) if (*pAddr++ == '.') break;
	gSinkIpDataBuf[15] = atoi((char *)pAddr); while (*pAddr) if (*pAddr++ == '.') break;

	/* Copy Destination address */
	if (strlen( (char *)gSinkAddress ) <  strlen( "x.x.x.x" ))
	{       /* Destination = Source */
		memcpy(&gSinkIpDataBuf[16], &gSinkIpDataBuf[12], 4);
	}
	else
	{
		/* Destination address: ex: 192.168.010.010  |  C0 A8 0A 0A  */
		pAddr = &gSinkAddress[0];
		gSinkIpDataBuf[16] = atoi((char *)pAddr); while (*pAddr) if (*pAddr++ == '.') break;
		gSinkIpDataBuf[17] = atoi((char *)pAddr); while (*pAddr) if (*pAddr++ == '.') break;
		gSinkIpDataBuf[18] = atoi((char *)pAddr); while (*pAddr) if (*pAddr++ == '.') break;
		gSinkIpDataBuf[19] = atoi((char *)pAddr); while (*pAddr) if (*pAddr++ == '.') break;
	}

	/* IP Header Checksum */
	for (i = 0; i < ATCI_IP_LEN; i += 2)
	{
		headerChecksum16 += ((gSinkIpDataBuf[i] << 8) | gSinkIpDataBuf[i + 1]);
	}
	headerChecksum16 = ( headerChecksum16  & 0xFFFF) + (headerChecksum16 >> 16);
	headerChecksum16 = ( headerChecksum16  & 0xFFFF) + (headerChecksum16 >> 16);
	headerChecksum16 = (~headerChecksum16) & 0xFFFF;

	gSinkIpDataBuf[10] = (INT8)(headerChecksum16 >> 8);
	gSinkIpDataBuf[11] = (INT8)(headerChecksum16  & 0x00ff);

}




/************************************************************************************
 * F@: createUDPPacket
 *
 * Create one UDP packet.
 */
void  createUDPPacket( UINT8 * pDatabuf, UINT32 len )
{
	CHAR                          *pAddr;
	INT32 i;
	INT32 headerChecksum16 = 0;
	INT32 sinkHeaderSize = ATCI_IP_LEN;
	static INT32 sinkPacketId = 0;
	static INT32 sinkRemainingPacketSize = 0;


	if (len >= ATCI_IP_MTU)
		len = ATCI_IP_MTU;

	/* fill the data buffer with NULL value */
//	memset ( pDatabuf, 0x00, ATCI_IP_MTU );

	memset( pDatabuf, 0x00, len + ATCI_IP_LEN + ATCI_UDP_LEN );

	pDatabuf[0] = 0x45;                                                                                                     /* IP +  Version=4 */
	pDatabuf[1] = 0x00;                                                                                                     /* Routine   |D=0|T=0|R=0| 0 | 0  |  00 */
	pDatabuf[2] = gSinkPduSize  >> 8;                                                                                       /* Total Length=XXXX */
	pDatabuf[3] = gSinkPduSize  & 0x00ff;
	pDatabuf[4] = sinkPacketId >> 8;                                                                                        /* Identification=XXXX */
	pDatabuf[5] = sinkPacketId & 0x00ff;
	pDatabuf[6] = (( ATCI_UDP_LEN + ATCI_IP_LEN + gSinkPacketSize - sinkRemainingPacketSize - gSinkPduSize ) / 8) >> 8;     /* DF=0 MF=X Fragment Offset=XX */
	if ( sinkRemainingPacketSize )
		pDatabuf[6] |= 0x20;
	pDatabuf[7] = (( ATCI_UDP_LEN + ATCI_IP_LEN + gSinkPacketSize - sinkRemainingPacketSize - gSinkPduSize ) / 8) & 0x00ff;
	pDatabuf[8] = 0x0F;                             /* Time to Live=15 |  0F */
	pDatabuf[9]  = 0x11;                            /* Protocol=UDP  |  11 */
	pDatabuf[10] = 0x00;                            /* Header Checksum   OK |  77 EF */
	pDatabuf[11] = 0x00;

	/* Copy Source address; ex: 018.052.086.120  |  12 34 56 78 */
	/* create a default address - */
	CHAR defaultAddress[] = "192.168.2.2";
	pAddr = &defaultAddress[0];

	pDatabuf[12] = atoi((char *)pAddr); while (*pAddr) if (*pAddr++ == '.') break;
	pDatabuf[13] = atoi((char *)pAddr); while (*pAddr) if (*pAddr++ == '.') break;
	pDatabuf[14] = atoi((char *)pAddr); while (*pAddr) if (*pAddr++ == '.') break;
	pDatabuf[15] = atoi((char *)pAddr); while (*pAddr) if (*pAddr++ == '.') break;

	/* Destination address: ex: 192.168.010.010  |  C0 A8 0A 0A  */

	pDatabuf[16] = 0xff;
	pDatabuf[17] = 0xff;
	pDatabuf[18] = 0xff;
	pDatabuf[19] = 0xff;


	/* first segment of the IP packet */
	/* init size left to be transmited from the IP packet */
	sinkRemainingPacketSize = gSinkPacketSize;

	/* new IP , new ID */
	sinkPacketId++;

	/* header contains UDP info */
	sinkHeaderSize += ATCI_UDP_LEN;

	/* fill in UDP specific info */
	pDatabuf[20] = 0x00;
	pDatabuf[21] = 0x00;                                            /* Source (optional) */
	pDatabuf[22] = gSinkPort >> 8;                                  /* UDP +  Source=0  | Dest=9  */
	pDatabuf[23] = gSinkPort & 0x00ff;
	pDatabuf[24] = (ATCI_UDP_LEN + gSinkPacketSize) >> 8;           /* Length */
	pDatabuf[25] = (ATCI_UDP_LEN + gSinkPacketSize) & 0x00ff;
	pDatabuf[26] = 0x00;
	pDatabuf[27] = 0x00; /* Checksum (optional) */


	/* IP Header Checksum */
	for (i = 0; i < ATCI_IP_LEN; i += 2)
	{
		headerChecksum16 += ((pDatabuf[i] << 8) | pDatabuf[i + 1]);
	}
	headerChecksum16 = ( headerChecksum16  & 0xFFFF) + (headerChecksum16 >> 16);
	headerChecksum16 = ( headerChecksum16  & 0xFFFF) + (headerChecksum16 >> 16);
	headerChecksum16 = (~headerChecksum16) & 0xFFFF;

	pDatabuf[10] = (INT8)(headerChecksum16 >> 8);
	pDatabuf[11] = (INT8)(headerChecksum16  & 0x00ff);

}


/************************************************************************************
 * F@: ciCsDataSink - GLOBAL API for GCF AT*ICSSINK  -command
 *
 */
RETURNCODE_T  ciCsDataSink(            const utlAtParameterOp_T op,
				       const char                      *command_name_p,
				       const utlAtParameterValue_P2c parameter_values_p,
				       const size_t num_parameters,
				       const char                      *info_text_p,
				       unsigned int                    *xid_p,
				       void                            *arg_p)
{
	UNUSEDPARAM(command_name_p)
	UNUSEDPARAM(num_parameters)
	UNUSEDPARAM(info_text_p)
	/*
	**  Set the result code to INITIAL_RETURN_CODE.  This allows
	**  the indications to display the correct return code after the
	**  AT Command is issued.
	*/
	RETURNCODE_T rc = INITIAL_RETURN_CODE;

	INT32 packetSize;
	INT32 pduCount;
	char csDataBuf[500];
	static GCFDATA gcfdata;

	/*
	 *  Put parser index into the variable
	 */
	CiRequestHandle reqHandle;
	TelAtParserID sAtpIndex = *(TelAtParserID *)arg_p;
	reqHandle = MAKE_AT_HANDLE( sAtpIndex );
	*xid_p = reqHandle;
	DBGMSG("%s: reqHandle = %d.\n", __FUNCTION__, reqHandle);


	switch (op)
	{
	case TEL_EXT_TEST_CMD:         /* AT*ICSSINK=? */
	{
		sprintf(csDataBuf, "*ICSSINK: (0-%d),(1-20)\r\n", ATCI_MAX_PDU_SIZE);
		ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, csDataBuf);
		rc = utlSUCCESS;
		break;
	}

	case TEL_EXT_SET_CMD:         /* AT*ICSSINK=XX,XX */
	{
		if (bLocalTest == FALSE)
		{
			ERRMSG("[%s] only supported in Test Mode!\n", __FUNCTION__);
			break;
		}

		if (( getExtValue(parameter_values_p, 0, &packetSize, 0, ATCI_DEFAULT_PDU_SIZE, ATCI_DEFAULT_PDU_SIZE) == TRUE ) &&
		    ( packetSize <= ATCI_MAX_PDU_SIZE ) )                             /* Check the PacketSize */
		{
			if (( getExtValue(parameter_values_p, 1, &pduCount, 1, ATCI_DEFAULT_PDU_COUNT, ATCI_DEFAULT_PDU_COUNT) == TRUE ) &&
			    ( pduCount <= ATCI_DEFAULT_PDU_COUNT ) )                             /* Check the PacketCount */
			{
				if (gCurrentCallsList.currCall[sAtpIndex].currCallId != 0)
				{
					gcfdata.cid = gCurrentCallsList.currCall[sAtpIndex].currCallId;
					gcfdata.len = packetSize * pduCount;
					gcfdata.databuf = (UINT8 *)utlMalloc(gcfdata.len + ATCI_IP_LEN + ATCI_UDP_LEN);
					createUDPPacket( gcfdata.databuf, gcfdata.len );
					ioctl(cidata_fd, CCIDATASTUB_GCFDATA, &gcfdata);
					utlFree(gcfdata.databuf);
					ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, NULL);
					rc = utlSUCCESS;
				}
			}                       /* if (( getExtValue(parameter_values_p, X, &pduCount,  */
		}                               /* if (( getExtValue(parameter_values_p, X, &packetSize, */

		break;
	}

	default:
		break;
	}


	return(rc);
}


/*******************************************************************
*  FUNCTION: vPDUS
*
*  DESCRIPTION: Get Max PDU Size
*
*  PARAMETERS:
*
*  RETURNS:
*
*******************************************************************/
utlReturnCode_T  vPDUS( const utlAtParameterOp_T op,
			const char                      *command_name_p,
			const utlAtParameterValue_P2c parameter_values_p,
			const size_t num_parameters,
			const char                      *info_text_p,
			unsigned int                    *xid_p,
			void                            *arg_p)
{
	UNUSEDPARAM(op)
	UNUSEDPARAM(command_name_p)
	UNUSEDPARAM(parameter_values_p)
	UNUSEDPARAM(num_parameters)
	UNUSEDPARAM(info_text_p)
	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	CiReturnCode ret = CIRC_FAIL;
	CiDatPrimGetMaxPduSizeReq *getPduSizeReq = NULL;


	/*
	 *  Put parser index into the variable
	 */
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);
	*xid_p = atHandle;

	ret = ciRequest(gAtciSvgHandle[CI_SG_ID_DAT], CI_DAT_PRIM_GET_MAX_PDU_SIZE_REQ, atHandle, (void *)getPduSizeReq);

	if (ret != CIRC_SUCCESS )
	{
		DBGMSG("Error doing CI_DAT_PRIM_GET_MAX_PDU_SIZE_REQ\n");
	}

	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}




/*******************************************************************
*  FUNCTION: atdb
*
*  DESCRIPTION: Used to debug setting
*
*  PARAMETERS:
*
*  RETURNS:
*
*******************************************************************/
utlReturnCode_T  atdb( const utlAtParameterOp_T op,
		       const char                      *command_name_p,
		       const utlAtParameterValue_P2c parameter_values_p,
		       const size_t num_parameters,
		       const char                      *info_text_p,
		       unsigned int                    *xid_p,
		       void                            *arg_p)
{
	UNUSEDPARAM(op)
	UNUSEDPARAM(command_name_p)
	UNUSEDPARAM(num_parameters)
	UNUSEDPARAM(info_text_p)
	UNUSEDPARAM(xid_p)
	TelAtParserID * sAtpIndex;
	sAtpIndex =  arg_p;
	UINT32 fun = 1, value = 1;


	if ( getExtValue(parameter_values_p, 0, (int *)&fun, TEL_AT_CFUN_0_FUN_VAL_MIN, TEL_AT_CFUN_0_FUN_VAL_MAX, TEL_AT_CFUN_0_FUN_VAL_DEFAULT) == TRUE )
	{
		if ( getExtValue(parameter_values_p, 0, (int *)&value, TEL_AT_CFUN_0_FUN_VAL_MIN, TEL_AT_CFUN_0_FUN_VAL_MAX, TEL_AT_CFUN_0_FUN_VAL_DEFAULT) == TRUE )
		{
			switch (fun)
			{
			case 1:
				g_CGSENDSwitch = value;
				break;
			case 2:
				g_TGSINKSwitch = value;
				break;
			default:
				break;
			}
		}
	}
	return utlSUCCESS;
}

void initDirectIP()
{
	DBGMSG("initDirectIP\n");
	if (cinetdevfd <= 0)
		cinetdevfd = open("/dev/ccichar", O_RDWR);

	if (cinetdevfd < 0)
	{
		ERRMSG("Error open /dev/ccichar\n");
		exit(-1);
	}
}

void initDataPPP()
{
	char name[20];

	DBGMSG("initDataPPP\n");
	// cctdatadev0 is reserved for VT CSD; cctdatadev1 is for internal Data PPP
	sprintf(name, "/dev/cctdatadev1");
	if ((cctdatafd = open(name, O_RDWR)) == -1)
	{
		ERRMSG("Error open name %s\n", name);
		exit(-1);
	}
}

/************************************************************************************
 *
 * DAT CI confirmations
 *
 *************************************************************************************/
void datCnf(CiSgOpaqueHandle opSgCnfHandle,
	    CiServiceGroupID svgId,
	    CiPrimitiveID primId,
	    CiRequestHandle reqHandle,
	    void*             paras)
{
	UNUSEDPARAM(opSgCnfHandle)
	CiPsPrimSendDataCnf  * pSendDataCnf;
	CiDatPrimSetUlTpIndCnf *pSetU1TpIndCnf;
	CiDatPrimGetMaxPduSizeCnf *pGetMaxPduSizeCnf;
	char tmpBuf[15];
	F_ENTER();

	/*
	**  Determine the primitive being confirmed.
	*/
	DBGMSG("[%s] Line(%d): primId(%d) \n", __FUNCTION__, __LINE__, primId);
	switch (primId)
	{
	case CI_DAT_PRIM_SEND_CNF:
	{
		ERRMSG("%s: CI_DAT_PRIM_SEND_CNF is received!\n");
		break;
	}
	case CI_DAT_PRIM_SEND_DATA_OPT_CNF:
	{
		ERRMSG("%s: CI_DAT_PRIM_SEND_DATA_OPT_CNF is received!\n");
		break;
	}

	case CI_PS_PRIM_SEND_DATA_CNF:
		//It seems that there is memory leak for CI_PS_PRIM_SEND_DATA_CNF
		pSendDataCnf = (CiPsPrimSendDataCnf*)paras;
		switch (pSendDataCnf->rc)
		{
		case CIRC_PS_SUCCESS:
			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, NULL);
			break;
		default:
			DBGMSG("[%s] Line(%d): pSendDataCnf->rc(%d) \n", __FUNCTION__, __LINE__, pSendDataCnf->rc);
			ATRESP( reqHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
			break;
		}
		break;

	case CI_DAT_PRIM_SET_UL_TP_IND_CNF:
		pSetU1TpIndCnf = (CiDatPrimSetUlTpIndCnf *)paras;
		switch (pSetU1TpIndCnf->result)
		{
		case CIRC_PS_SUCCESS:
			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, NULL);
			break;
		default:
			DBGMSG("[%s] Line(%d): pSetU1TpIndCnf->rc(%d) \n", __FUNCTION__, __LINE__, pSetU1TpIndCnf->result);
			ATRESP( reqHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
			break;

		}
		break;

	case CI_DAT_PRIM_GET_MAX_PDU_SIZE_CNF:
		pGetMaxPduSizeCnf = (CiDatPrimGetMaxPduSizeCnf *)paras;
		switch (pGetMaxPduSizeCnf->rc){
			case CIRC_PS_SUCCESS:
				sprintf(tmpBuf, "+VPDUS: %d", pGetMaxPduSizeCnf->size);
				ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, tmpBuf);
				break;
			default:
				DBGMSG("[%s] Line(%d): pGetMaxPduSizeCnf->rc(%d) \n", __FUNCTION__, __LINE__, pGetMaxPduSizeCnf->rc);
				ATRESP( reqHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
			break;
		}

	default:
		break;
	}

	/* free up the confirmation memory */
	atciSvgFreeCnfMem( svgId, primId, paras );

	F_LEAVE();

	return;
}



/************************************************************************************
 *
 * DAT CI notifications
 *
 *************************************************************************************/

void datInd(CiSgOpaqueHandle opSgIndHandle,
	    CiServiceGroupID svgId,
	    CiPrimitiveID primId,
	    CiIndicationHandle indHandle,
	    void*               pParam)
{
	UNUSEDPARAM(opSgIndHandle)
	UNUSEDPARAM(indHandle)
	F_ENTER();

	struct datahandle_obj dataHandle;
	UINT32 i, reqHandle = INVALID_REQ_HANDLE;

	CiDatPrimOkInd            *datPrimOkInd;

	switch (primId)
	{
	case CI_DAT_PRIM_RECV_IND:
	{
		/* Now data info should not be received in user space */
		ERRMSG("%s: CI_DAT_PRIM_RECV_IND is received!\n", __FUNCTION__);
		break;
	}
	case CI_DAT_PRIM_RECV_DATA_OPT_IND:
	{
		/* Now data info should not be received in user space */
		ERRMSG("%s: CI_DAT_PRIM_RECV_DATA_OPT_IND is received!\n", __FUNCTION__);
		break;
	}


	case CI_DAT_PRIM_OK_IND:
	{
		datPrimOkInd = (CiDatPrimOkInd *)pParam;

		dataHandle.m_connType = datPrimOkInd->connInfo.type;
		dataHandle.m_maxPduSize = datPrimOkInd->maxPduSize;
		dataHandle.m_cid = datPrimOkInd->connInfo.id;
		dataHandle.m_datConnInfo.id = dataHandle.m_cid;
		dataHandle.m_datConnInfo.type = dataHandle.m_connType;
		dataHandle.chanState = DATA_CHAN_READY_STATE;

		if (dataHandle.m_connType == CI_DAT_CONNTYPE_CS)
		{
			dataHandle.pdptype = CI_PS_PDP_TYPE_PPP;

			for (i = 0; i < gCurrentCallsList.NumCalls; i++)
			{
				if (dataHandle.m_cid == gCurrentCallsList.callInfo[i].CallId)
				{
					reqHandle = gCurrentCallsList.callInfo[i].reqHandle;
					break;
				}
			}
		}
		else
		{
			reqHandle = gCIDList.cInfo[dataHandle.m_cid].reqHandle;
			dataHandle.pdptype = gCIDList.cInfo[dataHandle.m_cid].pdpType;
		}

		if (GET_ATP_INDEX(reqHandle) == TEL_MODEM_AT_CMD_ATP)
			dataHandle.connectionType = ATCI_REMOTE;
		else
			dataHandle.connectionType = ATCI_LOCAL;

		ioctl(cidata_fd, CCIDATASTUB_DATAHANDLE, &dataHandle);

		if (giUseUlDlOpt)
		{
			CiDatPrimSetUlTpIndReq  *setUlTpIndReq;

			setUlTpIndReq = utlCalloc(1, sizeof(CiDatPrimSetUlTpIndReq));
			if (setUlTpIndReq == NULL) {
				ERRMSG("%s: CI_DAT_PRIM_OK_IND handling failed in calloc!\n", __FUNCTION__);
				break;
			}

			eeh_log(LOG_INFO, "*** sending TP request\r\n");
			setUlTpIndReq->reportInterval = 75;
			/*
			**  Send the CI request.
			*/
			ciRequest( gAtciSvgHandle[CI_SG_ID_DAT], CI_DAT_PRIM_SET_UL_TP_IND_REQ,
				   MAKE_CI_REQ_HANDLE(reqHandle, CI_DAT_PRIM_SET_UL_TP_IND_REQ), (void *)setUlTpIndReq );
		}

		/* update the connection state */
		switch ( dataHandle.m_connType )
		{
		case CI_DAT_CONNTYPE_CS:
		{
			ATRESP( reqHandle, ATCI_RESULT_CODE_SUPPRESS, 0, (char *)"CONNECT\r\n");

			/* if PPP connection requested -> enable PPP frame transfer */
			if ( dataHandle.connectionType == ATCI_REMOTE)
			{
				int fd = getFd(TEL_MODEM_AT_CMD_ATP);
				DPRINTF("enable CSD PPP for Modem: fd:%d\n", fd);
				ioctl(fd, TIOPPPONCSD, datPrimOkInd->connInfo.id);
			}
			else
			{
				if (bLocalTest)
				{
					initDataPPP();
					ioctl(cctdatafd, TIOPPPONCSD, datPrimOkInd->connInfo.id);
				}
			}

			break;
		}
		case CI_DAT_CONNTYPE_PS:
		{

			ATRESP( reqHandle, ATCI_RESULT_CODE_SUPPRESS, 0, (char *)"CONNECT\r\n");

			/* if PPP connection requested -> enable PPP frame transfer */
			if ( dataHandle.pdptype == CI_PS_PDP_TYPE_PPP )
			{
				if ( dataHandle.connectionType == ATCI_LOCAL)
				{
					if (bLocalTest)
					{
						initDataPPP();
						ioctl(cctdatafd, TIOPPPON, datPrimOkInd->connInfo.id);
					}
				}
				else
				{
					int fd = getFd(TEL_MODEM_AT_CMD_ATP);
					DPRINTF("enable PSD PPP for Modem :%d\n", fd);
					ioctl(fd, TIOPPPON, datPrimOkInd->connInfo.id);
				}

			}
			else
			{
				/* connection type is ready for data transfer in PS mode */
				if (bLocalTest)
				{
					initDirectIP();
					ioctl(cinetdevfd, CCINET_IP_ENABLE, datPrimOkInd->connInfo.id);
				}

			}

			break;
		}

		default:
		{
			break;
		}
		}

		break;
	}

	case CI_DAT_PRIM_NOK_IND:
	{
		/*vcy fix */
		CiDatPrimNokInd *datPrimNOkInd = (CiDatPrimNokInd *)pParam;

		switch ( datPrimNOkInd->connInfo.type )
		{
		case CI_DAT_CONNTYPE_CS:
		{
			for (i = 0; i < gCurrentCallsList.NumCalls; i++)
			{
				if (datPrimNOkInd->connInfo.id == gCurrentCallsList.callInfo[i].CallId)
				{
					reqHandle = gCurrentCallsList.callInfo[i].reqHandle;
					break;
				}
			}
			if (GET_ATP_INDEX(reqHandle) == TEL_MODEM_AT_CMD_ATP)
			{
				int fd = getFd(TEL_MODEM_AT_CMD_ATP);
				DPRINTF("turn off remote PPP over CSD:%d, %x\n", fd, TIOPPPOFF);
				if (fd != -1)
				{
					ioctl(fd, TIOPPPOFF, datPrimNOkInd->connInfo.id);
				}
			}

			usleep(200*1000);//maybe there is still CSD data after NOK, wait for 200 ms
			
			/* update the connection state */
			ioctl(cidata_fd, CCIDATASTUB_CS_CHNOK, datPrimNOkInd->connInfo.id);

			ATRESP(reqHandle, ATCI_RESULT_CODE_SUPPRESS, 0, (char*)"NO CARRIER");
			break;
		}

		case CI_DAT_CONNTYPE_PS:
		{
			DPRINTF("I am NOK,datPrimNOkInd->connInfo.type:%d,connInfo.id:%d\n",datPrimNOkInd->connInfo.type, datPrimNOkInd->connInfo.id);

			reqHandle = gCIDList.cInfo[datPrimNOkInd->connInfo.id].reqHandle;

			if (GET_ATP_INDEX(reqHandle) == TEL_MODEM_AT_CMD_ATP)
			{
				int fd = getFd(TEL_MODEM_AT_CMD_ATP);
				DPRINTF("turn off remote PPP:%d, %x\n", fd, TIOPPPOFF);
				if (fd != -1)
				{
					ioctl(fd, TIOPPPOFF, datPrimNOkInd->connInfo.id);
				}
			}
			else
			{
				if (bLocalTest)
				{
					if (cinetdevfd > 0)
						ioctl(cinetdevfd, CCINET_IP_DISABLE, datPrimNOkInd->connInfo.id);
					if (cctdatafd > 0)
						ioctl(cctdatafd, TIOPPPOFF, datPrimNOkInd->connInfo.id);
				}
			}

			/* update the connection state */
			
			if (gCIDList.cInfo[datPrimNOkInd->connInfo.id].pdpType == CI_PS_PDP_TYPE_PPP) //only PPP need wait 
				sleep(1);//Fix me: maybe there is still PPP data over Modem
				
			ioctl(cidata_fd, CCIDATASTUB_CHNOK, datPrimNOkInd->connInfo.id);

			/*Reset CID information */
			gCIDList.cInfo[datPrimNOkInd->connInfo.id].reqHandle = INVALID_REQ_HANDLE;

			/* Delete PDP context */
			//PS_DeleteGPRSContext(IND_REQ_HANDLE, datPrimNOkInd->connInfo.id);

			ATRESP(reqHandle, ATCI_RESULT_CODE_SUPPRESS, 0, (char*)"NO CARRIER");

							#if 0 //vcy testing
			/* Delete PDP context */


			deletePdpCtxReq.cid = gCurrentPdpSetCtx.cid;
			DBGMSG("%s: at line %d.\n", __FUNCTION__, __LINE__);

			sleep( WAIT_TIME_BEFORE_PDP_CTX_DELETE );

			ciRequest( gAtciSvgHandle[CI_SG_ID_PS], CI_PS_PRIM_DELETE_PDP_CTX_REQ,
				   gAtciSvgReqHandle[CI_SG_ID_PS]++, (void *)&deletePdpCtxReq );

			//          sleep( WAIT_TIME_BEFORE_PDP_CTX_DELETE);
			sleep(1);

							#endif
			break;
		}

		default:
		{
			break;
		}
		}
		break;
	}

	case CI_DAT_PRIM_UL_TP_IND:
	{
		char respBuffer[255];
		CiDatPrimUlTpInd *pDatPrimU1TpInd = (CiDatPrimUlTpInd *)pParam;

		//how to handle this????????
		sprintf(respBuffer, "CI_DAT_PRIM_UL_TP_IND:(minLimitBytes)%d\r\n",
			pDatPrimU1TpInd->minLimitBytes);

		ATRESP(IND_REQ_HANDLE, 0, 0, respBuffer);
		break;
	}

	default:
		break;
	}

	//delete me
	//  if (primId != CI_DAT_PRIM_NOK_IND)
	/* free up the indication memory */
	atciSvgFreeIndMem( svgId, primId, pParam );

	F_LEAVE();

	return;
}

