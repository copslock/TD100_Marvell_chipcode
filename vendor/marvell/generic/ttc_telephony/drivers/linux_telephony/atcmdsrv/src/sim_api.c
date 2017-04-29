/******************************************************************************
*(C) Copyright 2009 Marvell International Ltd.
* All Rights Reserved
******************************************************************************/
/*--------------------------------------------------------------------------------------------------------------------
 *  -------------------------------------------------------------------------------------------------------------------
 *
 *  Filename: sim_api.c
 *
 *  Description: API interface implementation for Sim related service
 *
 *  History:
 *   Oct 16, 2008 - Qiang Xu, Creation of file
 *
 *  Notes:
 *
 ******************************************************************************/
#include "ci_api_types.h"
#include "ci_api.h"
#include "telatci.h"
#include "teldef.h"
#include "telatparamdef.h"
#include "telutl.h"
#include "ci_sim.h"
#include "utlMalloc.h"
#include "telsim.h"
#include "ci_ss.h"
#include "cc_api.h"
#include "msg_api.h"
#include "pb_api.h"
#include "sim_api.h"

/************************************************************************************
 *
 * SIM related const and global variables
 *
 *************************************************************************************/

#define SIM_SELECT         164
#define READ_BINARY        176
#define READ_RECORD        178
#define GET_RESPONSE       192
#define UPDATE_BINARY      214
#define UPDATE_RECORD      220
#define STATUS             242
#define RETRIEVE_DATA      203
#define SET_DATA           219
#define LAST_RECORD        25
#define FIRST_RECORD       25
#define INVALID_INDEX      -1
#define EMPTY              0xFF

/* Define some private primitives for SIM service */
#define CI_SIM_PRIM_GENERIC_CMD_ERGA_REQ (CI_SIM_PRIM_LAST_COMMON_PRIM + 1)
#define CI_SIM_PRIM_GENERIC_CMD_ERTCA_REQ (CI_SIM_PRIM_LAST_COMMON_PRIM + 2)
#define CI_SIM_PRIM_MEP_READ_STATUS_REQ (CI_SIM_PRIM_LAST_COMMON_PRIM + 3) // for AT+CLCK="xx",2
#define CI_SIM_PRIM_MEP_READ_STATUS_EXT_REQ (CI_SIM_PRIM_LAST_COMMON_PRIM + 4) // for AT*CLCK="xx",2
#define CI_SIM_PRIM_MEP_SET_REQ (CI_SIM_PRIM_LAST_COMMON_PRIM + 5) // for AT+CLCK="xx",0/1 and AT*CLCK="xx",0/1
#define CI_SIM_PRIM_CHV_OPER_CPIN_VERIFY_REQ (CI_SIM_PRIM_LAST_COMMON_PRIM + 6)
#define CI_SIM_PRIM_CHV_OPER_CPIN_UNBLOCK_REQ (CI_SIM_PRIM_LAST_COMMON_PRIM + 7)
#define CI_SIM_PRIM_CHV_OPER_CPIN_CHANGE_REQ (CI_SIM_PRIM_LAST_COMMON_PRIM + 8)
#define CI_SIM_PRIM_RESET_ACM_REQ (CI_SIM_PRIM_LAST_COMMON_PRIM + 9)
#define CI_SIM_PRIM_SET_ACM_MAX_REQ (CI_SIM_PRIM_LAST_COMMON_PRIM + 10)
#define CI_SIM_PRIM_SET_CPUC_REQ (CI_SIM_PRIM_LAST_COMMON_PRIM + 11)
#define CI_SIM_PRIM_GET_EPIN_STATE_REQ (CI_SIM_PRIM_LAST_COMMON_PRIM + 12)
#define CI_SIM_PRIM_GET_SIM_DETECT_REQ (CI_SIM_PRIM_LAST_COMMON_PRIM + 13)

#ifndef AT_PARSE_REFRESH
#define AT_PARSE_REFRESH
#endif

extern CiServiceHandle gAtciSvgHandle[CI_SG_NUMIDS + 1];

extern AtciSSCntx gSSCntx;
INT32 gWaitToSetAcmMax = -1;

extern CiSimStatus gSimStatus;
extern CiSimPinState gSimPinState;

AtSimCardType gSimCardType  = AT_SIM_CARD_TYPE_SIM;

/*add for AT+CPUC support*/
extern CiCcPrimSetPuctInfoReq gSetPuctInfoReq;

/*add for AT*EUICC*/
extern BOOL getSimTypeInd;



typedef struct RespHandle_tag
{
	UINT32 respHandle;
	UINT16 bWaiting;
} RespHandle;

/* Record ind msg handle, the response msg handle should be same with ind msg handle */
static RespHandle gStkProactiveInd = { 0, 0 }, gStkSetupCallInd = { 0, 0 };
static void processRestrictedSimAccessConf(UINT32 atHandle, const void *paras);
/************************************************************************************
 *
 * Internal library function
 *
 *************************************************************************************/
static SimFileDir libGetDirFromEFFileID(const SimEfFileId FileIndex)
{
	SimFileDir SIMFileDir;

	switch (FileIndex)
	{
	case SIM_EF_FILEIDSAI:
		SIMFileDir = SIM_FILE_DIR_DF_PHONEBOOK;
		break;
		
	case SIM_EF_FILEIDICCID:
	case SIM_EF_FILEIDPL:
	{
		SIMFileDir = SIM_FILE_DIR_MF;
		break;
	}

	case SIM_EF_FILEIDSMS:
	case SIM_EF_FILEIDFDN:
	//case SIM_EF_FILEIDADN:
	case SIM_EF_FILEIDCCP:
	case SIM_EF_FILEIDMSISDN:
	case SIM_EF_FILEIDSMSS:
	case SIM_EF_FILEIDLND:
	case SIM_EF_FILEIDSMSP:
	case SIM_EF_FILEIDEXT1:
	case SIM_EF_FILEIDEXT2:
	case SIM_EF_FILEIDCPHS_INFO_NUM_OLD:
	case SIM_EF_FILEIDEXT3:
	case SIM_EF_FILEIDEXT4:
	case SIM_EF_FILEIDSMSR:
	case SIM_EF_FILEIDBDN:
	case SIM_EF_FILEIDSDN:
	{
		//if is USIM
		if ((gSimCardType == AT_SIM_CARD_TYPE_USIM) || (gSimCardType == AT_SIM_CARD_TYPE_TEST_USIM))
		{
			SIMFileDir = SIM_FILE_DIR_ADF_USIM;
		}
		else
		{
			SIMFileDir = SIM_FILE_DIR_DF_TELECOM;
		}
		break;
	}

	//linda add for test
	case SIM_EF_FILEIDADN:
		SIMFileDir = SIM_FILE_DIR_DF_TELECOM;
		break;
	
	case SIM_EF_FILEIDIMG:
	{
		SIMFileDir = SIM_FILE_DIR_DF_GRAPHICS;
		break;
	}

	case SIM_EF_FILEIDLOCI:
	{
		SIMFileDir = SIM_FILE_DIR_DF_GSM;
		break;
	}

	case SIM_EF_FILEIDINVALID:
	{
		SIMFileDir = SIM_FILE_DIR_INVALID;
		break;
	}

	default:
	{
		if ((FileIndex & 0xFF00) == 0x4F00)
		{
			SIMFileDir =  SIM_FILE_DIR_DF_GRAPHICS;
		}
		else
		{
			//if is USIM
			if ((gSimCardType == AT_SIM_CARD_TYPE_USIM) || (gSimCardType == AT_SIM_CARD_TYPE_TEST_USIM))
			{
				SIMFileDir = SIM_FILE_DIR_ADF_USIM;
			}
			else
			{
				SIMFileDir = SIM_FILE_DIR_DF_GSM;
			}
		}
		break;
	}
	}
	return (SIMFileDir);
}

static BOOL libCheckIsMasterFile(SimEfFileId *EFFileID, SimFileDir *SIMFileDir)
{
	BOOL bIsMasterFile = FALSE;

	switch (*EFFileID)
	{
	case SIM_FILE_DIR_MF:
	case SIM_FILE_DIR_DF_MEXE:
	case SIM_FILE_DIR_DF_GSM:
	case SIM_FILE_DIR_DF_DCS1800:
	case SIM_FILE_DIR_DF_GRAPHICS:
	case SIM_FILE_DIR_DF_TELECOM:
	case SIM_FILE_DIR_DF_GSM_ACCESS:
	case SIM_FILE_DIR_DF_PHONEBOOK:
	case SIM_FILE_DIR_ADF_USIM:
		*SIMFileDir  = (SimFileDir) * EFFileID;
		*EFFileID = SIM_EF_FILEIDINVALID;
		bIsMasterFile =  TRUE;
		break;

	default:
		bIsMasterFile =  FALSE;
		break;
	}

	return bIsMasterFile;
}

static void libGetSIMEfFilePath( UINT16 root, UINT16 FileDir, UINT16 EFFileID, CiSimFilePath* pEfFilePath)
{
	UINT8 index = 0;
	CiSimFilePath efFilePath;

	memset(&efFilePath, 0, sizeof(CiSimFilePath));

	if (root != SIM_FILE_DIR_INVALID)
	{
		efFilePath.valStr[index++] =  (CHAR)((root & 0xff00) >> 8);
		efFilePath.valStr[index++] =  (CHAR)(root & 0x00ff);
	}

	if (FileDir != SIM_FILE_DIR_INVALID)
	{
		efFilePath.valStr[index++] =  (CHAR)((FileDir & 0xff00) >> 8);
		efFilePath.valStr[index++] =  (CHAR)(FileDir & 0x00ff);
	}

	if (EFFileID != SIM_EF_FILEIDINVALID)
	{
		efFilePath.valStr[index++] =  (CHAR)((EFFileID & 0xff00) >> 8);
		efFilePath.valStr[index++] =  (CHAR)(EFFileID & 0x00ff);
	}

	efFilePath.len = index;
	*pEfFilePath = efFilePath;
}

#ifdef ATCMDSRV_CONVERT_CRSM
/****************************************************************************************
*
*  DESCRIPTION: Following libs are used for +CRSM request transfer
*
****************************************************************************************/
/* For PBK: If either first half of data (PB Name) or second half of data (PB Number) are all EMPTY,
    the option is deleting instead of replacing entry*/
static CiBoolean libIsCrsmPbkOptionDelete(char* pData, UINT8 len)
{
	UINT8 index = 0, flag = FALSE, len_half = len / 2;

	while (index < len_half)
	{
		if (pData[index] == EMPTY)
			index++;
		else
			break;
	}

	/* If all first half data are EMPTY, it is to delete msg */
	if (index == len_half)
	{
		flag = TRUE;
		return flag;
	}

	index = len_half;
	while (index < len)
	{
		if (pData[index] == EMPTY)
			index++;
		else
			break;
	}

	/* If all second half data are EMPTY, it is to delete msg, otherwise it is to replace entry */
	if (index == len)
		flag = TRUE;
	else
		flag = FALSE;

	return flag;
}


/* For SMS: If any of data is EMPTY, the option is replacing instead of deleting */
static CiBoolean libIsCrsmSmsOptionDelete(char* pData, UINT8 len)
{
	UINT8 index = 1, flag;

	while (index < len)
	{
		if (pData[index] == EMPTY)
			index++;
		else
			break;
	}
	/* If all data are EMPTY, it is to delete msg */
	if (index == len)
		flag = TRUE;
	else
		flag = FALSE;

	return flag;
}

static CiReturnCode libGetCrsmRecordIndex(UINT8 p1, UINT8 p2)
{
	switch (p2)
	{
	/* p2 = 3: previous record */
	case 3:
		if (p1 == 0)
		{
			p1 = LAST_RECORD;
		}
		else if (p1 == FIRST_RECORD)
		{
			p1 = INVALID_INDEX;
		}
		else
		{
			p1 = p1 - 1;
		}
		break;

	/* p2 = 2: next record */
	case 2:
		if (p1 == 0)
		{
			p1 = 1;
		}
		else if (p1 == LAST_RECORD)
		{
			p1 = INVALID_INDEX;
		}
		else
		{
			p1 = p1 + 1;
		}
		break;

	default:
		break;
	}
	return p1;
}

static void  libGetDataFromCrsm(char *src, int srcLen, char *dest, UINT16* destLen)
{
	UINT16 i = 0;

	while ((*(src + i) != EMPTY) && (i < srcLen))
	{
		*(dest + i) = *(src + i);
		i++;
	}
	*destLen = i;

}

/****************************************************************
 *  F@: libConvertCrsmAtRequest - Convert those +CRSM cmd which not supported by CP to other AT cmd
 */
static CiReturnCode libConvertCrsmAtRequest(UINT32 atHandle, int cmd, SimEfFileId fileId, int p1, int p2, int p3, char *data, int dataLen)
{
	UNUSEDPARAM(p3)
	CiReturnCode ret = CIRC_FAIL;
	int index=0;

	if (fileId == SIM_EF_FILEIDSMS)
	{
		index = libGetCrsmRecordIndex(p1, p2);
		if (index == INVALID_INDEX)
		{
			DBGMSG("[%s] libGetCrsmRecordIndex returns invalid index", __FUNCTION__);
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
			return ret;
		}

		if (cmd == READ_RECORD)
		{
			ret = MSG_CRSM_ReadMsg(atHandle, index);
		}

		else if (cmd == UPDATE_RECORD)
		{

			/* If all data are empty, it is to delete the msg specified by index */
			if (libIsCrsmSmsOptionDelete(data, dataLen) == TRUE)
			{
				ret = MSG_CRSM_DeleteMsg(atHandle, index, CI_MSG_MSG_DELFLAG_INDEX);
			}

			/* If the data is not empty, it is to write msg to specified index */
			else
			{
				CiMsgPdu smsPdu;
				int tpduAddr;
				tpduAddr = 1 + data[1] + 1;  //data[0]:EFsms status, data[1]:sca_len
				libGetDataFromCrsm(data + tpduAddr, dataLen - tpduAddr, (char *)(&(smsPdu.data)), &(smsPdu.len));
				ret = MSG_CRSM_WriteMsgWithIndex(atHandle, index, &smsPdu);
			}
		}
	}

	else if (fileId == SIM_EF_FILEIDFDN)
	{
		char pbStorageStr[3] = "FD";
		ret = PB_SetStorage(atHandle, pbStorageStr, NULL, 0);
		if (cmd == READ_RECORD)
		{
			ret = PB_CRSM_ReadOneEntry(atHandle, index);
		}

		else if (cmd == UPDATE_RECORD)
		{
			if (libIsCrsmPbkOptionDelete(data, dataLen) == TRUE)
			{
				ret = PB_CRSM_DeleteEntry(atHandle, index);
			}
			else
			{
				char number[CI_MAX_ADDRESS_LENGTH + ATCI_NULL_TERMINATOR_LENGTH];
				char text[CI_MAX_NAME_LENGTH + ATCI_NULL_TERMINATOR_LENGTH];
				UINT16 numDigits, type, txtLen;

				libGetDataFromCrsm(data, dataLen, text, &txtLen);
				libGetDataFromCrsm(&(data[16]), dataLen - 16, number, &numDigits);
				if (data[15] == 0x91)
					type = ATCI_DIAL_NUMBER_INTERNATIONAL;
				else
					type = ATCI_DIAL_NUMBER_UNKNOWN;
				ret = PB_CRSM_WriteEntry(atHandle, index, number, numDigits, type, text, txtLen);
			}
		}
	}

	return ret;
}
#endif
/****************************************************************
 *  F@: libConvertHexToNum - Convert HEX data in AT cmd  to data of number used in CCI msg
 */
int libConvertHexToNum(char *inData, int inLen, char *outData)
{
	int i, j, dataLen = 0;

	if (inLen > 0)
	{
		for ( i = 0, j = 0; j < inLen - 1; i++, j = j + 2 )
		{
			outData[i] = (hexToNum(inData[j]) << 4) + hexToNum(inData[j + 1]);
		}
		dataLen = i;
	}
	else
	{
		outData[0] = '\0';
		dataLen = 0;
	}
	return dataLen;
}


/****************************************************************
 *  F@: libConvert2UsimResp - Convert to USIM response format
 */
static void libConvert2UsimResp(UINT32 len, UINT8 *pData, UINT8 *pUSIMResponse, UINT32 USIMSize)
{
	UINT8 temp[CI_SIM_MAX_CMD_DATA_SIZE];
	UINT8 index = 0, i;

	memset(temp, 0x00, sizeof(temp));
	memset(pUSIMResponse, 0x00, USIMSize);
	memcpy(temp, pData, len);

	while (index < len)
	{
		switch (temp[index])
		{
		case 0x62:
			index += 2; //skip 'length' field
			break;
		case 0x82:
			if ((temp[index + 2] & 0x0F) == 0x01)
				pUSIMResponse[EF_STRUCTURE_OF_FILE] = 0x00;     //Transparent
			else if ((temp[index + 2] & 0x0F) == 0x02)
				pUSIMResponse[EF_STRUCTURE_OF_FILE] = 0x01;     //Linear fixed
			else
				DPRINTF("[SIM API] Fatal Error: wrong EF structure\r\n");

			if (temp[index + 1] == 0x02)
				index += 4;
			else if (temp[index + 1] == 0x05)
			{
				pUSIMResponse[EF_RECORD_LENGTH] = temp[index + 5];
				index += 7;
			}
			else
				DPRINTF("[SIM API] Fatal Error!!\n");

			pUSIMResponse[EF_TYPE_OF_FILE] = 0x04;
			break;
		case 0x83:
			pUSIMResponse[EF_FILE_ID1] = temp[index + 2];
			pUSIMResponse[EF_FILE_ID2] = temp[index + 3];
			index += 4;
			break;
		case 0xA5:
			index += (2 + temp[index + 1]);
			break;
		case 0x8A:
			index += 3;
			break;
		case 0x8B:
		case 0x8C:
		case 0xAB:
			index += (2 + temp[index + 1]);
			break;
		case 0x80:
			pUSIMResponse[EF_FILE_SIZE_BYTE1] = temp[index + 2];
			pUSIMResponse[EF_FILE_SIZE_BYTE2] = temp[index + 3];
			index += (2 + temp[index + 1]);
			break;
		case 0x81:
			index += (2 + temp[index + 1]);
			break;
		case 0x88:
			index += 3;
			break;
		default:
			DPRINTF("[SIM API] Fatal Error: Unknown tag\n");
			for (i = 0; i < 10; i++)
				DPRINTF("0x%02x  ", temp[i]);
			DPRINTF("\n");
			index += 100; //Escape from WHILE loop
			break;
		}
	} //end of WHILE loop
	  //pre-assigned
	pUSIMResponse[EF_FILE_STATUS]        = 0x01;
	pUSIMResponse[EF_FOLLOWING_DATA_LEN] = 0x02;
	pUSIMResponse[9]  = 0xFF;
	pUSIMResponse[10] = 0xBB;

	return;
}

#ifdef AT_PARSE_REFRESH
/****************************************************************
 *  F@: libConvertProactiveCmdInd - REFRESH Convert to other Indication format
 * For android, add +REFRESH: <result>[,<efid>] to map RIL_UNSOL_SIM_REFRESH
 * RIL_UNSOL_SIM_REFRESH
 *
 * Indicates that file(s) on the SIM have been updated, or the SIM
 * has been reinitialized.
 *
 * "data" is an int *
 * ((int *)data)[0] is a RIL_SimRefreshResult.
 * ((int *)data)[1] is the EFID of the updated file if the result is
 * SIM_FILE_UPDATE or NULL for any other result.
 *
 * Note: If the radio state changes as a result of the SIM refresh (eg,
 * SIM_READY -> SIM_LOCKED_OR_ABSENT), RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED
 * should be sent.
 *
 * For customer, HTC, add +COTA: ; +SIM_INIT
 * REFRESH;
 *'00' =SIM Initialization and Full File Change Notification;
 *'02' = SIM Initialization and File Change Notification;
 *'03' = SIM Initialization;
 *	The radio should send 
 *	+COTA: FFFF // issue refresh
 *	+SIM_INIT // issue AP have to reload the relate sim data
 *'01' = File Change Notification;
 *	The radio should send 
 *	+COTA: CHANGE FILE
 *'04' = SIM Reset;
 *	The radio should send
 *	+SIM_RESET
 *'05' to 'FF' = reserved values.
 *
 * Send CI_SIM_PRIM_PROACTIVE_CMD_RSP at here, due to up layer will not do it, but our CP need this response
 */
static BOOL libProcessRefresh(CiSimPrimProactiveCmdInd* originInd, char* customerRefreshIndBuf, char* androidRefreshIndBuf)
{
	int currentIndex = 0, offset = 3;
	int tag, cmdType, cmdNumber, cmdQual, temp, length, filePathTag, fileCount;
	int startIndex, endIndex, fileNumber;
	BOOL cr;
	char terminalResp[100];
	CiSimPrimProactiveCmdRsp refreshCmdRsp;
	char tmpBuf[30];

#ifdef AT_CUSTOMER_HTC
	UNUSEDPARAM(androidRefreshIndBuf)
#else
	UNUSEDPARAM(customerRefreshIndBuf)
#endif
	
	if((originInd->data[currentIndex++]&0xff) == 0xD0){
		temp = (originInd->data[currentIndex++])&0xff;
		if(temp<0x80) {
			length = temp;
		} else if (temp == 0x81) {
			temp = (originInd->data[currentIndex++])&0xff;
			length = temp;
		} else {
			ERRMSG("%s: the length of Indication is not correct\n", __FUNCTION__);
			return FALSE; 
		}
		
		temp = originInd->data[currentIndex++] & 0xff;
		if(temp == 0x7f) {
			tag = ((originInd->data[currentIndex] & 0xff) << 8) | (originInd->data[currentIndex + 1] & 0xff);
			cr = (tag & 0x8000) != 0;
			tag &= ~0x8000;
			currentIndex += 2;
		} else {
			tag = temp;
			cr = (tag & 0x80) != 0;
			tag &= ~0x80;
		}
		
		//detail command tag
		if(tag == 0x01) {
			if((originInd->data[currentIndex++]&0xff) == 0x03) {
				cmdNumber = (originInd->data[currentIndex++])&0xff;
				cmdType = (originInd->data[currentIndex++])&0xff;
				if(cmdType == 0x01) {
				//this is Refresh, continute to parse it.
				cmdQual = (originInd->data[currentIndex++])&0xff;
				DBGMSG("[%s] BerTlv length=%d,DetailCmdTag=%d,cmdNumber=%d,cmdType=%d,cmdQual=%d", 
					__FUNCTION__, length, tag, cmdNumber, cmdType, cmdQual);
				switch(cmdQual) {
				case 0x00:
				case 0x02:
				case 0x03:
#ifdef AT_CUSTOMER_HTC
					strcpy( customerRefreshIndBuf, "+COTA:FFFF\r\n+SIM_INIT\r\n");
#else
					sprintf( androidRefreshIndBuf, "+REFRESH: %d\r\n", 1); 
#endif
					break;
				case 0x01:
					startIndex = currentIndex+4; //skip device identities
					endIndex = length;
					tag = (originInd->data[startIndex++])&0xff;
					tag &= ~0x80;
					if(tag == 0x12) {
						//this is File list tag
						temp = (originInd->data[startIndex++])&0xff;
						if(temp < 0x80) {
							length = temp;
						} else if(temp == 0x81) {
							length = originInd->data[startIndex++] & 0xff;
						} else if(temp == 0x82) {
							length = ((originInd->data[startIndex] & 0xff) << 8) | (originInd->data[startIndex+1] & 0xff);
							startIndex +=2;
						} else if(temp == 0x83) {
							length = ((originInd->data[startIndex] & 0xff) << 16) 
								| ((originInd->data[startIndex + 1] & 0xff) << 8) | (originInd->data[startIndex + 2] & 0xff);
							startIndex += 3;
						}
						fileNumber = (originInd->data[startIndex])&0xff;
						DBGMSG("[%s] RefreshTlv filelistlength=%d,RefreshCmdTag=%d,start=%d,end=%d,fileNumber=%d", 
							__FUNCTION__, length, tag, startIndex, endIndex, fileNumber);
						//Full paths are given to files. Each of these must be at least 4 octets in length (e.g. "3F002FE2" or "3F007F206F07")
						for(fileCount = 0; fileCount < (fileNumber - 1); fileCount++) {
							temp = originInd->data[startIndex+offset] << 8;
							temp |= originInd->data[startIndex+offset+1];
							filePathTag = originInd->data[startIndex+offset+2] << 8;
							filePathTag |= originInd->data[startIndex+offset+3];

							// An entry in the file description shall therefore always begin with 3F00
							if(filePathTag == 0x3f00) {
#ifdef AT_CUSTOMER_HTC
								sprintf(tmpBuf, "+COTA:%02X%02X\r\n",originInd->data[startIndex+offset],originInd->data[startIndex+offset+1]);
								strcat(customerRefreshIndBuf, tmpBuf);
#else
								sprintf(tmpBuf, "+REFRESH: %d, %d\r\n", 0, temp);
								strcat (androidRefreshIndBuf, tmpBuf);
#endif
								offset = offset + 4;								
							} else { 
#ifdef AT_CUSTOMER_HTC
								sprintf(tmpBuf, "+COTA:%02X%02X\r\n",originInd->data[startIndex+offset+2],originInd->data[startIndex+offset+3]);
								strcat(customerRefreshIndBuf, tmpBuf);
#else
								sprintf(tmpBuf, "+REFRESH: %d, %d\r\n", 0, filePathTag);
								strcat(androidRefreshIndBuf, tmpBuf);
#endif
								offset = offset + 6;
							}
						}
						//control the last two or four bytes
						if((length-offset) == 2) {
							temp = originInd->data[startIndex+offset] << 8;
							temp |= originInd->data[startIndex+offset+1]; 
#ifdef AT_CUSTOMER_HTC
							sprintf(tmpBuf, "+COTA:%02X%02X\r\n",originInd->data[startIndex+offset],originInd->data[startIndex+offset+1]);
							strcat(customerRefreshIndBuf, tmpBuf);
#else
							sprintf(tmpBuf, "+REFRESH: %d, %d\r\n", 0, temp);
							strcat(androidRefreshIndBuf, tmpBuf);
#endif
						} else if ((length-offset) == 4) {
						    temp = originInd->data[startIndex+offset+2] << 8;
							temp |= originInd->data[startIndex+offset+3];
#ifdef AT_CUSTOMER_HTC
							sprintf(tmpBuf, "+COTA:%02X%02X\r\n",originInd->data[startIndex+offset+2],originInd->data[startIndex+offset+3]);
							strcat(customerRefreshIndBuf, tmpBuf);
#else
							sprintf(tmpBuf, "+REFRESH: %d, %d\r\n", 0, temp);
							strcat(androidRefreshIndBuf, tmpBuf);
#endif
						}
					} else {
						ERRMSG("%s: Refresh File Change Notification, no file list tag\n", __FUNCTION__);
#ifdef AT_CUSTOMER_HTC
						strcpy( customerRefreshIndBuf, "+COTA:FFFF\r\n+SIM_INIT\r\n");
#else
						//if missing Filelist, consider it's init case
						sprintf( androidRefreshIndBuf, "+REFRESH: %d\r\n", 1);
#endif
					}
					break;
				case 0x04:
#ifdef AT_CUSTOMER_HTC
					strcpy( customerRefreshIndBuf, "+SIM_RESET\r\n");
#else
					sprintf( androidRefreshIndBuf, "+REFRESH: %d\r\n", 2);
#endif
					break;
				case 0x05:
					break;
				}
				sprintf( terminalResp, "0103%02X01%02X82028281830100", (unsigned char)cmdNumber, (unsigned char)cmdQual);
				ERRMSG("%s: terminal Response sent, %s\n", __FUNCTION__, terminalResp);
				/* Convert hex style of data in AT cmd to Num stype */
				refreshCmdRsp.len = libConvertHexToNum(terminalResp, strlen(terminalResp), (char *)(refreshCmdRsp.data));
				ciRespond( gAtciSvgHandle[CI_SG_ID_SIM], CI_SIM_PRIM_PROACTIVE_CMD_RSP,
					gStkProactiveInd.respHandle, (void *)&refreshCmdRsp );
				gStkProactiveInd.bWaiting = 0;
				return true;
			} else {
				ERRMSG("%s: This Indication is not Refresh\n", __FUNCTION__);
				return FALSE;
			} 
		 } else {
			ERRMSG("%s: the length of commandDetail is not correct\n", __FUNCTION__);
			return FALSE;
		 }
		} else {
			ERRMSG("%s: comamnd Detail not found \n", __FUNCTION__);
			return FALSE;
		}
	} else {
		ERRMSG("%s: This is not proactvice command \n", __FUNCTION__);
		return FALSE;
	}
}

#endif

/****************************************************************
 *  F@: SIM_ConvertCrsmAtReply - Convert those +CRSM cmd which not supported by CP to other AT cmd
 */
CiBoolean SIM_ConvertCrsmAtReply(UINT32 atHandle, CiPrimitiveID primId, const void *paras)
{
	CiSimPrimGenericCmdCnf    *simCmdCnf;

	/* If it is to change PBK storage to FDN, skip this cnf msg and continue to wait next CI cnf msg */
	if (primId == CI_PB_PRIM_SELECT_PHONEBOOK_CNF)
		return TRUE;

	simCmdCnf = utlCalloc(1, sizeof(CiSimPrimGenericCmdCnf));
	if (simCmdCnf == NULL)
		return FALSE;

	switch (primId)
	{

	case CI_ERR_PRIM_SIMNOTREADY_CNF:
	{
		simCmdCnf->rc = CIRC_SIM_NOT_INSERTED;
		break;
	}

	case CI_MSG_PRIM_READ_MESSAGE_CNF:
	{
		CiMsgPrimReadMessageCnf * readPduCnf;
		readPduCnf = (CiMsgPrimReadMessageCnf *)paras;
		if (readPduCnf->rc == CIRC_MSG_SUCCESS)
		{
			simCmdCnf->rc = CIRC_SIM_OK;
			simCmdCnf->cnf.len = readPduCnf->pdu.len + 4; //SW1+SW2+default SC(0x00)+SMS status;
			simCmdCnf->cnf.data[simCmdCnf->cnf.len - 2] = 144;
			simCmdCnf->cnf.data[simCmdCnf->cnf.len - 1] = 0;

			/* Convert read msg cnf status to CRSM SMS status, refer to TS 51.011 10.5.3 */
			if (readPduCnf->status == 0)                    //REC UNREAD
				simCmdCnf->cnf.data[0] = 0x03;
			else if (readPduCnf->status == 1)               //REC READ
				simCmdCnf->cnf.data[0] = 0x01;
			else if (readPduCnf->status == 2)               //STO UNSENT
				simCmdCnf->cnf.data[0] = 0x07;
			else if (readPduCnf->status == 3)               //STO SENT
				simCmdCnf->cnf.data[0] = 0x05;

			simCmdCnf->cnf.data[1] = 0x00; //default SC(0x00)
			memcpy(&(simCmdCnf->cnf.data[2]), readPduCnf->pdu.data, readPduCnf->pdu.len);
		}
		else if (readPduCnf->rc == CIRC_MSG_INVALID_MEM_INDEX)
		{
			simCmdCnf->rc = CIRC_SIM_OK;
			simCmdCnf->cnf.len = 176 + 2;      //SW1+SW2
			simCmdCnf->cnf.data[simCmdCnf->cnf.len - 2] = 144;
			simCmdCnf->cnf.data[simCmdCnf->cnf.len - 1] = 0;
			simCmdCnf->cnf.data[0] = 0x00;
			memset(&(simCmdCnf->cnf.data[1]), EMPTY, simCmdCnf->cnf.len - 3);
		}
		else
		{
			simCmdCnf->rc = CIRC_SIM_FAILURE;
		}
		break;
	}

	case CI_MSG_PRIM_DELETE_MESSAGE_CNF:
	{
		CiMsgPrimDeleteMessageCnf * deleteMsgCnf;
		deleteMsgCnf = (CiMsgPrimDeleteMessageCnf *)paras;
		if ( deleteMsgCnf->rc == CIRC_MSG_SUCCESS )
		{
			simCmdCnf->rc = CIRC_SIM_OK;
			simCmdCnf->cnf.len = 2; //SW1+SW2
			simCmdCnf->cnf.data[simCmdCnf->cnf.len - 2] = 144;
			simCmdCnf->cnf.data[simCmdCnf->cnf.len - 1] = 0;
		}
		else
		{
			simCmdCnf->rc = CIRC_SIM_FAILURE;
		}
		break;
	}

	case CI_MSG_PRIM_WRITE_MSG_WITH_INDEX_CNF:
	{
		CiMsgPrimWriteMsgWithIndexCnf* writeMsgWithIndexCnf;
		writeMsgWithIndexCnf = (CiMsgPrimWriteMsgWithIndexCnf*)paras;
		if ( writeMsgWithIndexCnf->rc == CIRC_MSG_SUCCESS )
		{
			simCmdCnf->rc = CIRC_SIM_OK;
			simCmdCnf->cnf.len = 2; //SW1+SW2
			simCmdCnf->cnf.data[simCmdCnf->cnf.len - 2] = 144;
			simCmdCnf->cnf.data[simCmdCnf->cnf.len - 1] = 0;
		}
		else
		{
			simCmdCnf->rc = CIRC_SIM_FAILURE;
		}
		break;
	}

	case CI_PB_PRIM_READ_PHONEBOOK_ENTRY_CNF:
	{
		CiPbPrimReadPhonebookEntryCnf* readEntryCnf;
		readEntryCnf = (CiPbPrimReadPhonebookEntryCnf*)paras;
		if ( readEntryCnf->Result == CIRC_PB_SUCCESS )
		{
			char tempBuf[28];
			UINT8 numTemp;
			UINT8 fdnNumLen;
			memcpy( tempBuf, readEntryCnf->entry.Number.Digits, readEntryCnf->entry.Number.Length );
			memset(simCmdCnf->cnf.data, EMPTY, 28);
			string2BCD(&(simCmdCnf->cnf.data[16]), tempBuf, readEntryCnf->entry.Number.Length);
			if ((readEntryCnf->entry.Number.Length) % 2 == 0)
				numTemp = (readEntryCnf->entry.Number.Length) / 2 + 1;
			else
				numTemp = (readEntryCnf->entry.Number.Length) / 2 + 2;
			fdnNumLen = numTemp;

			simCmdCnf->rc = CIRC_SIM_OK;
			simCmdCnf->cnf.len = 28 + 2; //SW1+SW2
			simCmdCnf->cnf.data[14] = fdnNumLen - 0x30;
			if (readEntryCnf->entry.Number.AddrType.NumType == CI_NUMTYPE_INTERNATIONAL)
				simCmdCnf->cnf.data[15] = 0x91;
			else
				simCmdCnf->cnf.data[15] = 0x81;
			memcpy(simCmdCnf->cnf.data, readEntryCnf->entry.Name.Name, readEntryCnf->entry.Name.Length );
			simCmdCnf->cnf.data[simCmdCnf->cnf.len - 2] = 144;
			simCmdCnf->cnf.data[simCmdCnf->cnf.len - 1] = 0;
		}

		else if ( readEntryCnf->Result == CIRC_PB_DATA_UNAVAILABLE )
		{
			simCmdCnf->rc = CIRC_SIM_OK;
			simCmdCnf->cnf.len = 28 + 2;
			simCmdCnf->cnf.data[simCmdCnf->cnf.len - 2] = 144;
			simCmdCnf->cnf.data[simCmdCnf->cnf.len - 1] = 0;
			memset(simCmdCnf->cnf.data, EMPTY, simCmdCnf->cnf.len - 2);
		}
		else
		{
			simCmdCnf->rc = CIRC_SIM_FAILURE;
		}
		break;
	}

	case CI_PB_PRIM_DELETE_PHONEBOOK_ENTRY_CNF:
	{
		CiPbPrimDeletePhonebookEntryCnf* deleteEntryCnf;
		deleteEntryCnf = (CiPbPrimDeletePhonebookEntryCnf*)paras;
		if ( deleteEntryCnf->Result == CIRC_PB_SUCCESS )
		{
			simCmdCnf->rc = CIRC_SIM_OK;
			simCmdCnf->cnf.len = 2; //SW1+SW2
			simCmdCnf->cnf.data[0] = 144;
			simCmdCnf->cnf.data[1] = 0;
		}
		else
		{
			simCmdCnf->rc = CIRC_SIM_FAILURE;
		}
		break;
	}

	case CI_PB_PRIM_REPLACE_PHONEBOOK_ENTRY_CNF:
	{
		CiPbPrimReplacePhonebookEntryCnf* replaceEntryCnf;
		replaceEntryCnf = (CiPbPrimReplacePhonebookEntryCnf*)paras;
		if (replaceEntryCnf->Result == CIRC_PB_SUCCESS)
		{
			simCmdCnf->rc = CIRC_SIM_OK;
			simCmdCnf->cnf.len = 2; //SW1+SW2
			simCmdCnf->cnf.data[0] = 144;
			simCmdCnf->cnf.data[1] = 0;
		}
		else
		{
			simCmdCnf->rc = CIRC_SIM_FAILURE;
		}
		break;
	}
	case CI_PB_PRIM_READ_ENH_PBK_ENTRY_CNF:
	case CI_PB_PRIM_EDIT_ENH_PBK_ENTRY_CNF:
	case CI_PB_PRIM_READ_CATEGORY_NAME_CNF:
	case CI_PB_PRIM_EDIT_CATEGORY_NAME_CNF:
	case CI_PB_PRIM_READ_GROUP_NAME_CNF:
	case CI_PB_PRIM_EDIT_GROUP_NAME_CNF:

		//TBD: Just for test.
		simCmdCnf->rc = CIRC_SIM_FAILURE;
		break;
	default:
	{
		ERRMSG("CRSM converted Reply: PrimId(%d) not matched and CI msg not processed\n", primId);
		utlFree(simCmdCnf);
		return FALSE;
	}

	}

	processRestrictedSimAccessConf(atHandle, simCmdCnf);
	utlFree(simCmdCnf);

	return TRUE;

}

/************************************************************************************
 *
 * Implementation of SIM related AT commands by sending Request to CCI. Called by Telsim.c
 *
 *************************************************************************************/

/****************************************************************************************
*  FUNCTION:  SIM_RunGSMAlgorithm
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to run GSM algorithm, implementation of AT+ERGA=
*	   AT+ERGA=<rand> (string type values)
*	   +ERGA: <sres>,<kc>
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode SIM_RunGSMAlgorithm(UINT32 atHandle, char *data, int dataLen)
{
	CiReturnCode ret = CIRC_FAIL;
	CiSimPrimGenericCmdReq	*restrictSimCmd;
	INT16 outLen = 0;
	CHAR outdata[RGA_RAND_LEN * 2];

	outLen = libConvertHexToNum(data, dataLen, (char *)outdata);

	if (outLen != RGA_RAND_LEN)
		return ret;

	restrictSimCmd = utlCalloc(1, sizeof(CiSimPrimGenericCmdReq));
	if (restrictSimCmd == NULL)
		return CIRC_FAIL;

	restrictSimCmd->length = outLen;

	memset(&restrictSimCmd->path, 0, sizeof(CiSimFilePath));

	restrictSimCmd->path.len = 0x02;
	restrictSimCmd->path.valStr[0] = 0x7F;
	restrictSimCmd->path.valStr[1] = 0x20;

	restrictSimCmd->instruction = CI_SIM_RUN_GSM_ALGORITHM;
	restrictSimCmd->param1 = 0;
	restrictSimCmd->param2 = 0x00;

	memcpy(restrictSimCmd->data, outdata, outLen);

	restrictSimCmd->responseExpected = FALSE;
	restrictSimCmd->responseLength = 255;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SIM], CI_SIM_PRIM_GENERIC_CMD_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_SIM_PRIM_GENERIC_CMD_ERGA_REQ), (void *)restrictSimCmd );

	return ret;

}

/****************************************************************************************
*  FUNCTION:  SIM_RunTContextAuth
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to run GSM algorithm, implementation of AT+ERTCA=
*	   AT+ERTCA=<rand>,<autn> (string type values)
*	   +ERTCA: <status>,[<res/auts>[,<ck>,<ik>[,<kc>]]]
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode SIM_RunTContextAuth(UINT32 atHandle, char *rand, int randLen, char *autn, int autnLen)
{
	CiReturnCode ret = CIRC_FAIL;
	CiSimPrimGenericCmdReq	*restrictSimCmd;
	INT16 outrandLen = 0, outautnLen = 0;
	CHAR outrand[RGA_RAND_LEN * 2];
	CHAR outautn[RGA_AUTN_LEN * 2];

	outrandLen = libConvertHexToNum(rand, randLen, (char *)outrand);
	outautnLen = libConvertHexToNum(autn, autnLen, (char *)outautn);

	if (outrandLen != RGA_RAND_LEN &&
		outautnLen != RGA_AUTN_LEN)
	{
		return ret;
	}

	restrictSimCmd = utlCalloc(1, sizeof(CiSimPrimGenericCmdReq));
	if (restrictSimCmd == NULL)
		return CIRC_FAIL;

	restrictSimCmd->length = outrandLen + outautnLen + 2;

	memset(&restrictSimCmd->path, 0, sizeof(CiSimFilePath));

	restrictSimCmd->path.len = 0x02;
	restrictSimCmd->path.valStr[0] = 0x7F;
	restrictSimCmd->path.valStr[1] = 0xFF;

	restrictSimCmd->instruction = CI_SIM_AUTHENTICATE;
	restrictSimCmd->param1 = 0;
	restrictSimCmd->param2 = 0x81;

	restrictSimCmd->data[0] = outrandLen;
	memcpy(&restrictSimCmd->data[1], outrand, outrandLen);

	restrictSimCmd->data[outrandLen + 1] = outautnLen;
	memcpy(&restrictSimCmd->data[outrandLen + 2], outautn, outautnLen);

	restrictSimCmd->responseExpected = TRUE;
	restrictSimCmd->responseLength = 255;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SIM], CI_SIM_PRIM_GENERIC_CMD_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_SIM_PRIM_GENERIC_CMD_ERTCA_REQ), (void *)restrictSimCmd );

	return ret;

}

/****************************************************************************************
*  FUNCTION:  SIM_RestrictedAccess
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to restricted access SIM card by sending command directly, implementation of AT+CRSM=
*      AT+CRSM=<command>[,<fileid>[,<P1>,<P2>,<P3>[,<data>[,<pathid>]]]]
*      +CRSM: <sw1>,<sw2>[,<response>]
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
//change by linda add path parameter
//CiReturnCode SIM_RestrictedAccess(UINT32 atHandle, int cmd, int iFileId, int p1, int p2, int p3, char *data, int dataLen)
CiReturnCode SIM_RestrictedAccess(UINT32 atHandle, int cmd, int iFileId, char *valStr, int p1, int p2, int p3, char *data, int dataLen)
{
	CiReturnCode ret = CIRC_FAIL;
	SimEfFileId fileId = (SimEfFileId)iFileId;
	SimFileDir fileDir;
	CiSimFilePath efFilePath;
	CiSimPrimGenericCmdReq  *restrictSimCmd;
	CiSimFilePath myFilePath;
	UINT8 index;

	/* === Step 1: Construct CI msg, which is also used in convert case */
	restrictSimCmd = utlCalloc(1, sizeof(CiSimPrimGenericCmdReq));
	if (restrictSimCmd == NULL)
		return CIRC_FAIL;

	/* Set sim cmd data and datalen*/
	restrictSimCmd->length = libConvertHexToNum(data, dataLen, (char *)(restrictSimCmd->data));

	/* Set path of the elementary file*/
	memset(&efFilePath, 0, sizeof(CiSimFilePath));
	memset(&myFilePath, 0, sizeof(CiSimFilePath));
	
        myFilePath.len = libConvertHexToNum(valStr, strlen(valStr), (char *)myFilePath.valStr);
	
	if(myFilePath.len == 0)
	{
		if (libCheckIsMasterFile(&fileId, &fileDir) != TRUE )
		{
			fileDir = libGetDirFromEFFileID(fileId);
		}
		if (fileDir == SIM_FILE_DIR_DF_PHONEBOOK || fileDir == SIM_FILE_DIR_DF_GRAPHICS) 
		{
			libGetSIMEfFilePath(SIM_FILE_DIR_DF_TELECOM, fileDir, fileId, &efFilePath);
		}
		else
		{
			libGetSIMEfFilePath(SIM_FILE_DIR_MF, fileDir, fileId, &efFilePath);
		}
	}
	else
	{
	 	index = myFilePath.len;
	 	myFilePath.valStr[index++] = ((fileId & 0xff00)>>8);
	 	myFilePath.valStr[index++] = (fileId & 0x00ff);
    		myFilePath.len = index;
		memcpy(&efFilePath, &myFilePath, sizeof(CiSimFilePath));		
	     
	}

	memcpy(&restrictSimCmd->path, &efFilePath, sizeof(CiSimFilePath));

	/* Set other params */
	restrictSimCmd->instruction = cmd;
	restrictSimCmd->param1 = p1;
	restrictSimCmd->param2 = p2;

	if (p3 > 0 )
	{
		restrictSimCmd->responseExpected = 1;
		restrictSimCmd->responseLength = p3;
	}
	else
	{
		restrictSimCmd->responseExpected = 0;
	}
#ifdef ATCMDSRV_CONVERT_CRSM
	/*  === Step 2a: Convert those +CRSM cmd which not supported by CP to other AT cmd */
	/* currently, it need to do special handle for SIM_EF_FILEIDSMS and SIM_EF_FILEIDSFDN (for read/update command) */
	switch (fileId)
	{
	case SIM_EF_FILEIDSMS:
	case SIM_EF_FILEIDFDN:
	{
		if ((cmd == READ_RECORD) || (cmd == UPDATE_RECORD))
		{
			ret = libConvertCrsmAtRequest(atHandle, cmd, fileId, p1, p2, p3, (char *)(restrictSimCmd->data), restrictSimCmd->length);
			return ret;
		}

		break;
	}
	default:
	{
		break;
	}
	}
	/* For response command, it always needs to be replaced by SELECT command. */
	if (cmd == GET_RESPONSE)
	{
		restrictSimCmd->instruction = CI_SIM_SELECT;
		restrictSimCmd->param1 = 0;
		restrictSimCmd->param2 = 0;
		restrictSimCmd->length = 2;

		restrictSimCmd->data[0] = (iFileId >> 8) & 0xFF;
		restrictSimCmd->data[1] = (iFileId) & 0xFF;
		restrictSimCmd->responseExpected = TRUE;
		restrictSimCmd->responseLength = 255;

		if (restrictSimCmd->path.len > 1) /* remove the file id the last 2 bytes*/
		{
			restrictSimCmd->path.valStr[restrictSimCmd->path.len - 1] = '\0';
			restrictSimCmd->path.valStr[restrictSimCmd->path.len - 2] = '\0';
			restrictSimCmd->path.len -= 2;
		}
	}
#endif
	/* === Step 2b: If CP support, send CCI request */
	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SIM], CI_SIM_PRIM_GENERIC_CMD_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_SIM_PRIM_GENERIC_CMD_REQ), (void *)restrictSimCmd );

	return ret;
}

/****************************************************************************************
*  FUNCTION:  SIM_SetCommand
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to set SIM command directly, implementation of AT+CSIM=
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode SIM_GenericAccess(UINT32 atHandle, int cmdLen, char *cmdStr)
{
	CiReturnCode ret = CIRC_FAIL;
	int i, j;
	CiSimPrimExecCmdReq  *genSimCmd;

	/* According to GSM 11.11 (sect.9.1), command is at least 5 octets */
	if ( cmdLen < 10 )
		return CIRC_FAIL;

	genSimCmd = utlCalloc(1, sizeof(CiSimPrimExecCmdReq));
	if (genSimCmd == NULL)
		return CIRC_FAIL;

	for ( i = 0, j = 0; j < cmdLen - 1; i++, j = j + 2 )
	{
		genSimCmd->cmd.data[i] = (hexToNum(cmdStr[j]) << 4) + hexToNum(cmdStr[j + 1]);
	}
	genSimCmd->cmd.len = i;
	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SIM], CI_SIM_PRIM_EXECCMD_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_SIM_PRIM_EXECCMD_REQ), (void *)genSimCmd );

	return ret;
}

/****************************************************************************************
*  FUNCTION:  SIM_GetImsi
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to get IMSI, implementation of AT+CIMI
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode SIM_GetImsi(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiSimPrimGetSubscriberIdReq  *getSubIDReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SIM], CI_SIM_PRIM_GET_SUBSCRIBER_ID_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_SIM_PRIM_GET_SUBSCRIBER_ID_REQ), (void *)getSubIDReq );

	return ret;
}

/****************************************************************************************
*  FUNCTION:  SIM_GetPinState
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to get Pin state, implementation of AT+CPIN? or AT+EPIN?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode SIM_GetPinState(UINT32 atHandle, int cpinCmdType)
{
	CiReturnCode ret = CIRC_FAIL;
	CiSimPrimGetPinStateReq  *getPinStateReq = NULL;

	/* use different primitive ID to Record it is AT+CPIN? or AT+EPIN?, used when receive conf msg */
	if(cpinCmdType == CMD_TYPE_CPIN)
		ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SIM], CI_SIM_PRIM_GET_PIN_STATE_REQ,
				MAKE_CI_REQ_HANDLE(atHandle, CI_SIM_PRIM_GET_PIN_STATE_REQ), (void *)getPinStateReq );
	else if(cpinCmdType == CMD_TYPE_EPIN)
		ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SIM], CI_SIM_PRIM_GET_PIN_STATE_REQ,
				MAKE_CI_REQ_HANDLE(atHandle, CI_SIM_PRIM_GET_EPIN_STATE_REQ), (void *)getPinStateReq );
	else
		return CIRC_FAIL;

	return ret;
}


/****************************************************************************************
*  FUNCTION:  SIM_GetSIMInserted
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to get Pin state, implementation of AT*SIMDETEC=
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode SIM_GetSIMInserted(UINT32 atHandle, UINT32 simSlot)
{
	UNUSEDPARAM(simSlot);
	CiReturnCode ret = CIRC_FAIL;
	CiSimPrimGetPinStateReq  *getPinStateReq = NULL;

	//TODO: support simslot which indicate dual-SIM

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SIM], CI_SIM_PRIM_GET_PIN_STATE_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_SIM_PRIM_GET_SIM_DETECT_REQ), (void *)getPinStateReq );


	return ret;
}


/****************************************************************************************
*  FUNCTION:  SIM_EnterPin
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to enter pin code to unblock or change pin code, implementation of AT+CPIN=
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode SIM_EnterPin(UINT32 atHandle, UINT8 no, UINT8 oper, CHAR* oldPasswd, CHAR* newPasswd)
{
	CiReturnCode ret = CIRC_FAIL;
	CiSimPrimOperChvReq   *chvOperReq;
	UINT8 operNeedSimChv;
	CiRequestHandle reqHandle;

	if (oldPasswd == NULL)
		return CIRC_FAIL;

	chvOperReq = utlCalloc(1, sizeof(CiSimPrimOperChvReq));
	if (chvOperReq == NULL)
		return CIRC_FAIL;

	chvOperReq->chvNo = no;
	/*the low four bit represent CiSimChvOper, the high four bit represent different operation need CiSimChvOper*/
	operNeedSimChv = ((oper & 0xf0) >> 4);
	chvOperReq->oper = (oper & 0x0f);

	chvOperReq->chvVal.len = strlen((char *)oldPasswd);
	if(CI_MAX_PASSWORD_LENGTH - 1 < chvOperReq->chvVal.len)
	{
		ERRMSG("%s: the length of %s is too long, exceed %d\n", __FUNCTION__, (char *)oldPasswd, CI_MAX_PASSWORD_LENGTH - 1);
		utlFree(chvOperReq);
		return ret;
	}
	strcpy((char *)chvOperReq->chvVal.data,  (char *)oldPasswd);
	if (newPasswd != NULL)
	{
		chvOperReq->newChvVal.len = strlen((char *)newPasswd);
		if(CI_MAX_PASSWORD_LENGTH - 1 < chvOperReq->newChvVal.len)
		{
			ERRMSG("%s: the length of %s is too long, exceed %d\n", __FUNCTION__, (char *)newPasswd, CI_MAX_PASSWORD_LENGTH - 1);
			utlFree(chvOperReq);
			return ret;
		}
		strcpy((char *)chvOperReq->newChvVal.data,  (char *)newPasswd);
	}
	else
	{
		chvOperReq->newChvVal.len = 0;
	}
	if(operNeedSimChv == RESET_ACM)
		reqHandle = MAKE_CI_REQ_HANDLE(atHandle, CI_SIM_PRIM_RESET_ACM_REQ);
	else if(operNeedSimChv == SET_ACM_MAX)
		reqHandle = MAKE_CI_REQ_HANDLE(atHandle, CI_SIM_PRIM_SET_ACM_MAX_REQ);
	else if(operNeedSimChv == SET_CPUC)
		reqHandle = MAKE_CI_REQ_HANDLE(atHandle, CI_SIM_PRIM_SET_CPUC_REQ);
	else if(chvOperReq->oper == CI_SIM_CHV_VERIFY)
		reqHandle = MAKE_CI_REQ_HANDLE(atHandle, CI_SIM_PRIM_CHV_OPER_CPIN_VERIFY_REQ);
	else if(chvOperReq->oper == CI_SIM_CHV_UNBLOCK)
		reqHandle = MAKE_CI_REQ_HANDLE(atHandle, CI_SIM_PRIM_CHV_OPER_CPIN_UNBLOCK_REQ);
	else if(chvOperReq->oper == CI_SIM_CHV_CHANGE)
		reqHandle = MAKE_CI_REQ_HANDLE(atHandle, CI_SIM_PRIM_CHV_OPER_CPIN_CHANGE_REQ);
	else
		reqHandle = MAKE_CI_REQ_HANDLE(atHandle, CI_SIM_PRIM_OPERCHV_REQ);

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SIM], CI_SIM_PRIM_OPERCHV_REQ,
			 reqHandle, (void *)chvOperReq );

	return ret;

}

/****************************************************************************************
*  FUNCTION:   SIM_SetLockFacility
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to enter pin code to unblock or change pin code, implementation of AT+CPIN=
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode SIM_SetLockFacility(UINT32 atHandle, int mode, int facType, CHAR* passwd, int passwdStrLen)
{
	CiReturnCode ret = CIRC_FAIL;
	CiSimPrimLockFacilityReq   *lockFacReq;

	if(CI_MAX_PASSWORD_LENGTH - 1 < passwdStrLen)
	{
		ERRMSG("%s: the length of %s is too long, exceed %d\n", __FUNCTION__, (char *)passwd, CI_MAX_PASSWORD_LENGTH - 1);
		ATRESP( atHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
		return ret;
	}
	lockFacReq = utlCalloc(1, sizeof(CiSimPrimLockFacilityReq));
	if (lockFacReq == NULL)
		return CIRC_FAIL;

	lockFacReq->pass.len = passwdStrLen;
	strcpy( (char *)lockFacReq->pass.data, (char *)passwd);
	lockFacReq->fac = facType;
	lockFacReq->mode = (CiSimFacLckMode)mode;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SIM], CI_SIM_PRIM_LOCK_FACILITY_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_SIM_PRIM_LOCK_FACILITY_REQ), (void *)lockFacReq );

	return ret;
}

/****************************************************************************************
*  FUNCTION:   STK_EnableProactiveCmdInd
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to Enable/Disable Proactive command Indication, implementation of AT+MSTK=0,<flag>
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode STK_EnableProactiveCmdIndConfig(UINT32 atHandle, UINT8 enable)
{
	CiReturnCode ret = CIRC_FAIL;
	CiSimPrimEnableSimatIndsReq   *enableSimatIndReq;

	enableSimatIndReq = utlCalloc(1, sizeof(CiSimPrimEnableSimatIndsReq));
	if (enableSimatIndReq == NULL)
		return CIRC_FAIL;

	enableSimatIndReq->enable = enable;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SIM], CI_SIM_PRIM_ENABLE_SIMAT_INDS_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_SIM_PRIM_ENABLE_SIMAT_INDS_REQ), (void *)enableSimatIndReq );

	return ret;
}

/****************************************************************************************
*  FUNCTION:   STK_GetSimProfile
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to get SIM card profile, implementation of AT+MSTK=3
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode STK_GetSimProfile(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiSimPrimGetTerminalProfileReq   *getProfileReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SIM], CI_SIM_PRIM_GET_TERMINALPROFILE_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_SIM_PRIM_GET_TERMINALPROFILE_REQ), (void *)getProfileReq );

	return ret;
}

/****************************************************************************************
*  FUNCTION:   STK_DownloadMeProfile
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to download ME profile, implementation of AT+MSTK=1,<ME profile data>
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode STK_DownloadMeProfile(UINT32 atHandle, char *data, int dataLen)
{
	CiReturnCode ret = CIRC_FAIL;
	CiSimPrimDownloadProfileReq   *downloadProfileReq;

	downloadProfileReq = utlCalloc(1, sizeof(CiSimPrimDownloadProfileReq));
	if (downloadProfileReq == NULL)
		return CIRC_FAIL;

	/* Convert hex style of data in AT cmd to Num stype */
	downloadProfileReq->profile.len = libConvertHexToNum(data, (dataLen < 2*CI_SIM_MAX_ME_PROFILE_SIZE ? dataLen : 2*CI_SIM_MAX_ME_PROFILE_SIZE), (char *)(downloadProfileReq->profile.data));

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SIM], CI_SIM_PRIM_DOWNLOADPROFILE_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_SIM_PRIM_DOWNLOADPROFILE_REQ), (void *)downloadProfileReq );

	return ret;
}

/****************************************************************************************
*  FUNCTION:   STK_SendEnvelopeCmd
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to send envelope command, implementation of AT+MSTK=4, < Envelope command data >
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode STK_SendEnvelopeCmd(UINT32 atHandle, char *data, int dataLen)
{
	CiReturnCode ret = CIRC_FAIL;
	CiSimPrimEnvelopeCmdReq   *envelopeCmdReq;

	envelopeCmdReq = utlCalloc(1, sizeof(CiSimPrimEnvelopeCmdReq));
	if (envelopeCmdReq == NULL)
		return CIRC_FAIL;

	/* Convert hex style of data in AT cmd to Num stype */
	envelopeCmdReq->len = libConvertHexToNum(data, dataLen, (char *)(envelopeCmdReq->data));

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SIM], CI_SIM_PRIM_ENVELOPE_CMD_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_SIM_PRIM_ENVELOPE_CMD_REQ), (void *)envelopeCmdReq );

	return ret;
}

/****************************************************************************************
*  FUNCTION:   STK_RespProactiveCmd
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to send proactive command response to +MSTK: 11, < proactive command >
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode STK_RespProactiveCmd(UINT32 atHandle, char *data, int dataLen)
{
	CiReturnCode ret = CIRC_FAIL;
	CiSimPrimProactiveCmdRsp   proactiveCmdRsp;

	/* Convert hex style of data in AT cmd to Num stype */
	proactiveCmdRsp.len = libConvertHexToNum(data, dataLen, (char *)(proactiveCmdRsp.data));

	//if(gStkProactiveInd.bWaiting)
	if (1)
	{
		ret = ciRespond( gAtciSvgHandle[CI_SG_ID_SIM], CI_SIM_PRIM_PROACTIVE_CMD_RSP,
				 gStkProactiveInd.respHandle, (void *)&proactiveCmdRsp );
		gStkProactiveInd.bWaiting = 0;

		/* Reply to AT parser to stop timer of this AT command session */
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
	}

	else
	{
		ERRMSG("There is no pending CI indication message to wait for response");

		/* Reply to AT parser to stop timer of this AT command session */
		ATRESP( atHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
	}

	return ret;
}

/****************************************************************************************
*  FUNCTION:   STK_RespSetupCall
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to response setup call request originated from STK app, implementation of AT+MSTK=12
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode STK_RespSetupCall(UINT32 atHandle, UINT8 accept)
{
	CiReturnCode ret = CIRC_FAIL;
	CiSimPrimGetCallSetupAckRsp   getCallSetupAckRsp;

	getCallSetupAckRsp.accept = accept;

	if (gStkSetupCallInd.bWaiting)
	{
		ret = ciRespond( gAtciSvgHandle[CI_SG_ID_SIM], CI_SIM_PRIM_GET_CALL_SETUP_ACK_RSP,
				 gStkSetupCallInd.respHandle, (void *)&getCallSetupAckRsp );
		gStkSetupCallInd.bWaiting = 0;

		/* Reply to AT parser to stop timer of this AT command session */
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
	}

	else
	{
		ERRMSG("There is no pending CI indication message to wait for response");

		/* Reply to AT parser to stop timer of this AT command session */
		ATRESP( atHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
	}

	return ret;
}

/****************************************************************************************
*  FUNCTION:   STK_GetSimCapInfo
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to requests SIM Application Toolkit (SIMAT) Notification Capability information, implementation of AT+MSTK=2
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode STK_GetSimCapInfo(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiSimPrimGetSimatNotifyCapReq   *getCapReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SIM], CI_SIM_PRIM_GET_SIMAT_NOTIFY_CAP_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_SIM_PRIM_GET_SIMAT_NOTIFY_CAP_REQ), (void *)getCapReq );

	return ret;
}


/****************************************************************************************
*  FUNCTION:   VSIM_GetVSim
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to get Virtual SIM, implementation of AT*EnVsim?
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/

CiReturnCode VSIM_GetVSim(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiSimPrimGetVSimReq  *getVSimReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SIM], CI_SIM_PRIM_GET_VSIM_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_SIM_PRIM_GET_VSIM_REQ), (void *)getVSimReq );
	return ret;
}

/****************************************************************************************
*  FUNCTION:   VSIM_SetVSim
*
*  PARAMETERS:
*
*  DESCRIPTION: interface to set Virtual SIM, implementation of AT*EnVsim
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode VSIM_SetVSim(UINT32 atHandle)
{
	CiReturnCode ret = CIRC_FAIL;
	CiSimPrimSetVSimReq    *setVSimReq = NULL;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SIM], CI_SIM_PRIM_SET_VSIM_REQ,
			 MAKE_CI_REQ_HANDLE(atHandle, CI_SIM_PRIM_SET_VSIM_REQ), (void *)setVSimReq );
	return ret;
}

/****************************************************************************************
*  FUNCTION:   MEPReadCodes
*
*  PARAMETERS:
*
*  DESCRIPTION: read MEP codes from CP
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode MEPReadCodes(UINT32 atHandle, AtciFacType ciFacType)
{
	CiReturnCode ret = CIRC_FAIL;
	CiSimPrimReadMEPCodesReq		 *readMEPCodesReq;

	readMEPCodesReq = utlCalloc(1, sizeof(CiSimPrimReadMEPCodesReq));
	if (readMEPCodesReq == NULL)
		return CIRC_FAIL;

	readMEPCodesReq->MEPCategory = ciFacType.facId;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SIM], CI_SIM_PRIM_READ_MEP_CODES_REQ,
			MAKE_CI_REQ_HANDLE(atHandle,CI_SIM_PRIM_READ_MEP_CODES_REQ), (void *)readMEPCodesReq );

	return ret;
}

/****************************************************************************************
*  FUNCTION:   MEPCallBarringStatus
*
*  PARAMETERS:
*
*  RETURNS: CiReturnCode
*
****************************************************************************************/
CiReturnCode PersonalizeME(UINT32 atHandle, AtciFacType ciFacType, INT32 mode, CHAR * facPwdStr, INT16 facPwdStrLen, BOOL is_ext)
{
	CiReturnCode ret = CIRC_FAIL;
	CiSimPrimPersonalizeMEReq	*personalizeMEReq;
	unsigned int req_id = 0;

	personalizeMEReq = utlCalloc(1, sizeof(CiSimPrimPersonalizeMEReq));
	if (personalizeMEReq == NULL)
		return CIRC_FAIL;

	/* Check entered string.... */
	if ( facPwdStrLen >= CI_MIN_PASSWORD_LENGTH )
	{
		/* set common fields */
		personalizeMEReq->pass.len = facPwdStrLen;
		memcpy(&personalizeMEReq->pass.data[0],facPwdStr,facPwdStrLen);
	}
	else
	{
		personalizeMEReq->pass.len = 0;
	}

	switch (mode)
	{
		case 0:
			personalizeMEReq->oper = ATCI_MEP_MODE_DISABLE;
			break;
		case 1:
			personalizeMEReq->oper = ATCI_MEP_MODE_UNBLOCK;
			break;
		case 2:
			personalizeMEReq->oper = ATCI_MEP_MODE_READ;
			break;
		case 3:
			personalizeMEReq->oper = ATCI_MEP_MODE_DEACTIVATE;
			break;
		case 4:
			personalizeMEReq->oper = ATCI_MEP_MODE_ACTIVATE;
			break;
	}

	if(mode == 2 && is_ext)
		req_id = CI_SIM_PRIM_MEP_READ_STATUS_EXT_REQ;
	else if(mode == 2)
		req_id = CI_SIM_PRIM_MEP_READ_STATUS_REQ;
	else
		req_id = CI_SIM_PRIM_MEP_SET_REQ;

	personalizeMEReq->cat = ciFacType.facId;

	ret = ciRequest( gAtciSvgHandle[CI_SG_ID_SIM], CI_SIM_PRIM_PERSONALIZEME_REQ,
			MAKE_CI_REQ_HANDLE(atHandle,req_id), (void *)personalizeMEReq );

	return ret;
}

/****************************************************************
 *  F@: libConvertNumToHex - Convert number in CCI msg to HEX data displayed in AT cmd
 *  Note:  the data is appended to *outData
 */
void libConvertNumToHex(char *inData, int inLen, char *outData)
{
	int i;
	char tmpBuf[10];

	if (inLen > 0)
	{
		for ( i = 0; i < inLen; i++ )
		{
			sprintf( tmpBuf, "%02X", inData[i] );
			strcat( outData, tmpBuf );
		}
	}
}

/************************************************************************************
 *
 * Implementation of processing reply or indication of SIM related AT commands. Called by msgCnf() or msgInd
 * Return : bool: whether needs to display info of left PIN times
 *
 *************************************************************************************/
static void libDisplayPinState (CiRequestHandle atHandle, CiSimPinState pinState )
{
	DBGMSG("[%s]:line(%d), pinState(%d).\n", __FUNCTION__, __LINE__, pinState );
	switch ( pinState )
	{
	case CI_SIM_PIN_ST_READY:
		if (gSimStatus == CI_SIM_ST_READY)
			ATRESP( atHandle, 0, 0, (char *)"+CPIN: READY");
		break;

	case CI_SIM_PIN_ST_UNIVERSALPIN_REQUIRED:
	case CI_SIM_PIN_ST_CHV1_REQUIRED:
		ATRESP( atHandle, 0, 0, (char *)"+CPIN: SIM PIN");
		break;

	case CI_SIM_PIN_ST_CHV2_REQUIRED:
		ATRESP( atHandle, 0, 0, (char *)"+CPIN: SIM PIN2");
		break;

	case CI_SIM_PIN_ST_UNBLOCK_UNIVERSALPIN_REQUIRED:
	case CI_SIM_PIN_ST_UNBLOCK_CHV1_REQUIRED:
		ATRESP( atHandle, 0, 0, (char *)"+CPIN: SIM PUK");
		break;

	case CI_SIM_PIN_ST_UNBLOCK_CHV2_REQUIRED:
		ATRESP( atHandle, 0, 0, (char *)"+CPIN: SIM PUK2");
		break;

	case CI_SIM_PIN_ST_HIDDENKEY_REQUIRED:
		ATRESP( atHandle, 0, 0, (char *)"+CPIN: PH-FSIM PIN");
		break;

	case CI_SIM_PIN_ST_UNBLOCK_HIDDENKEY_REQUIRED:
		ATRESP( atHandle, 0, 0, (char *)"+CPIN: PH-FSIM PUK");
		break;

	case CI_SIM_PIN_ST_PCK_REQUIRED:
		ATRESP( atHandle, 0, 0, (char *)"+CPIN: PH-SIMLOCK PIN");
		break;

	case CI_SIM_PIN_ST_NCK_REQUIRED:
		ATRESP( atHandle, 0, 0, (char *)"+CPIN: PH-NET PIN");
		break;

	case CI_SIM_PIN_ST_NSCK_REQUIRED:
		ATRESP( atHandle, 0, 0, (char *)"+CPIN: PH-NETSUB PIN");
		break;

	case CI_SIM_PIN_ST_SPCK_REQUIRED:
		ATRESP( atHandle, 0, 0, (char *)"+CPIN: PH-SP PIN");
		break;

	case CI_SIM_PIN_ST_CCK_REQUIRED:
		ATRESP( atHandle, 0, 0, (char *)"+CPIN: PH-CORP PIN");
		break;

	case CI_SIM_PIN_ST_UNBLOCK_PCK_REQUIRED:
		ATRESP( atHandle, 0, 0, (char *)"+CPIN: PH-SIMLOCK PUK");
		break;

	case CI_SIM_PIN_ST_UNBLOCK_NCK_REQUIRED:
		ATRESP( atHandle, 0, 0, (char *)"+CPIN: PH-NET PUK");
		break;

	case CI_SIM_PIN_ST_UNBLOCK_NSCK_REQUIRED:
		ATRESP( atHandle, 0, 0, (char *)"+CPIN: PH-NETSUB PUK");
		break;

	case CI_SIM_PIN_ST_UNBLOCK_SPCK_REQUIRED:
		ATRESP( atHandle, 0, 0, (char *)"+CPIN: PH-SP PUK");
		break;

	case CI_SIM_PIN_ST_UNBLOCK_CCK_REQUIRED:
		ATRESP( atHandle, 0, 0, (char *)"+CPIN: PH-CORP PUK");
		break;

	/* Following AT responses are private string which are consistent with RIL code */
	case CI_SIM_PIN_ST_CHV1_BLOCKED:
	case CI_SIM_PIN_ST_CHV2_BLOCKED:
	case CI_SIM_PIN_ST_UNIVERSALPIN_BLOCKED:
	case CI_SIM_PIN_ST_UNBLOCK_CHV1_BLOCKED:
	case CI_SIM_PIN_ST_UNBLOCK_CHV2_BLOCKED:
	case CI_SIM_PIN_ST_UNBLOCK_UNIVERSALPIN_BLOCKED:
		ATRESP( atHandle, 0, 0, (char *)"+CPIN: SIM BLOCKED");
		break;

	case CI_SIM_PIN_ST_REMOVED:

		if (IND_REQ_HANDLE == atHandle)
		{
			ATRESP(atHandle, 0, 0, (char *)"+CPIN: SIM REMOVED");
		}
		else
		{
			ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_NO_SIM, NULL);
		}

		break;

	case CI_SIM_PIN_ST_WAIT_INITIALISATION:
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_SIM_BUSY, NULL);
		break;

	case CI_SIM_PIN_ST_NETWORK_REJECTED:
	case CI_SIM_PIN_ST_INIT_FAILED:
	case CI_SIM_PIN_ST_WRONG_SIM:
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_SIM_FAILURE, NULL);
		break;

	case CI_SIM_PIN_ST_GENERAL_ERROR:
	case CI_SIM_PIN_ST_MEP_ERROR:
	case CI_SIM_PIN_ST_UDP_ERROR:
	case CI_SIM_PIN_ST_CPHS_ERROR:
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_SIM_WRONG, NULL);
		break;

	/* Unexpected case */
	default:
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
		ERRMSG( "+CPIN: unprocessed state code: %d", pinState );
		break;
	}
}

/* Display SIM return code
 */
static void libDisplaySimRc (CiRequestHandle atHandle, CiSimRc rc)
{
	const AtciCmeError error_map[CIRC_SIM_NUM_RESCODES] = 
	{
		0xFFFF,                    // CIRC_SIM_OK
		CME_SIM_FAILURE,           // CIRC_SIM_FAILURE
		CME_MEMORY_FAILURE,        // CIRC_SIM_MEM_PROBLEM
		CME_SIM_BUSY,              // CIRC_SIM_SIMAT_BUSY
		CME_SIM_FAILURE,           // CIRC_SIM_INFO_UNAVAILABLE
		CME_NO_SIM,                // CIRC_SIM_NOT_INSERTED
		CME_SIM_PIN_REQUIRED,      // CIRC_SIM_PIN_REQUIRED
		CME_SIM_PUK_REQUIRED,      // CIRC_SIM_PUK_REQUIRED
		CME_SIM_BUSY,              // CIRC_SIM_BUSY
		CME_SIM_WRONG,             // CIRC_SIM_WRONG
		CME_INCORRECT_PASSWD,      // CIRC_SIM_INCORRECT_PASSWORD
		CME_SIM_PIN2_REQUIRED,     // CIRC_SIM_PIN2_REQUIRED
		CME_SIM_PUK2_REQUIRED,     // CIRC_SIM_PUK2_REQUIRED
		CME_OPERATION_NOT_ALLOWED, // CIRC_SIM_OPERATION_NOT_ALLOWED
		CME_MEMORY_FULL,           // CIRC_SIM_MEMORY_FULL
		CME_UNKNOWN,               // CIRC_SIM_UNKNOWN
		CME_UNKNOWN,               // CIRC_SIM_PERSONALISATION_DISABLED
		CME_UNKNOWN,               // CIRC_SIM_PERSONALISATION_BLOCKED
		CME_UNKNOWN,               // CIRC_SIM_PERSONALISATION_UNKNOWN
		CME_UNKNOWN                // CIRC_SIM_PERSONALISATION_NOT_SUPPORTED
	};

	if(rc == CIRC_SIM_OK)
	{
		ATRESP(atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
	}
	else if(rc >= CIRC_SIM_NUM_RESCODES)
	{
		ATRESP(atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
		ERRMSG("%s: unknown rc: %d", rc);
	}
	else
	{
		ATRESP(atHandle, ATCI_RESULT_CODE_CME_ERROR, error_map[rc], NULL);
	}
}



/****************************************************************
 *  F@: processEnterPinConf - Process CI cnf msg of AT+CPIN=
 */
static void processEnterPinConf(UINT32 reqHandle, const void *paras)
{
	CiSimPrimOperChvCnf   *chvOperCnf = (CiSimPrimOperChvCnf *)paras;
	UINT32 atHandle = GET_AT_HANDLE(reqHandle);

	if ( chvOperCnf->rc == CIRC_SIM_OK )
	{
		/* check if waiting for CHV reply before sending a 'reset ACM' */
		if ( GET_REQ_ID(reqHandle) ==  CI_SIM_PRIM_RESET_ACM_REQ )
		{
			if (CC_SetAcm(atHandle, NULL) != CIRC_SUCCESS)
			{
				ATRESP( atHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
			}
		}
		/* check if waiting for CHV reply before sending a 'set ACM Max value' */
		else if ( GET_REQ_ID(reqHandle) ==  CI_SIM_PRIM_SET_ACM_MAX_REQ )
		{
			if (CC_SetAmm(atHandle, gWaitToSetAcmMax, NULL) != CIRC_SUCCESS)
			{
				ATRESP( atHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
			}
		}
		else if ( GET_REQ_ID(reqHandle) ==  CI_SIM_PRIM_CHV_OPER_CPIN_VERIFY_REQ)
		{
			ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		}
		else if ( GET_REQ_ID(reqHandle) == CI_SIM_PRIM_CHV_OPER_CPIN_UNBLOCK_REQ )
		{
			ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		}
		else if ( GET_REQ_ID(reqHandle) == CI_SIM_PRIM_CHV_OPER_CPIN_CHANGE_REQ )
		{
			ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		}
		else if ( GET_REQ_ID(reqHandle) == CI_SIM_PRIM_SET_CPUC_REQ )
		{
			if (CC_SetCPUC(atHandle, TRUE, 0, NULL) != CIRC_SUCCESS)
			{
				ATRESP( atHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
			}

		}
		else
		{
			/* PIN verification required for other operations - for future implementation */
		}
	}
	else
	{
		libDisplaySimRc(atHandle, chvOperCnf->rc);
	}

	return;
}

/****************************************************************
 *  F@: processGenericSimAccessConf - Process CI cnf msg of AT+CSIM=
 */
static void processGenericSimAccessConf(UINT32 atHandle, const void *paras)
{
	char simRspBuf[2 * CI_SIM_MAX_CMD_DATA_SIZE + 50];
	CiSimPrimExecCmdCnf    *execCmdCnf;

	execCmdCnf = (CiSimPrimExecCmdCnf *)paras;

	if ( execCmdCnf->rc == CIRC_SIM_OK )
	{
		/* From Voyager AT command description, it looks like we ignore status words (SW1 & SW2) */
		sprintf(simRspBuf, "+CSIM:%d,", (2 * execCmdCnf->cnf.len) );
		libConvertNumToHex((char *)(execCmdCnf->cnf.data), execCmdCnf->cnf.len, simRspBuf);
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, simRspBuf);
	}
	else
	{
		libDisplaySimRc(atHandle, execCmdCnf->rc);
	}

	return;
}

/****************************************************************
 *  F@: processRestrictedAccessConf - Process CI cnf msg of AT+CRSM=
 */
static void processRestrictedSimAccessConf(UINT32 atHandle, const void *paras)
{
	char simRspBuf[2 * CI_SIM_MAX_CMD_DATA_SIZE + 50];
	UINT8 aucUSimCmdResponse[15];
	UINT32 i = 0;
	CiSimPrimGenericCmdCnf    *simCmdCnf;

	simCmdCnf = (CiSimPrimGenericCmdCnf *)paras;

	if ( simCmdCnf->rc == CIRC_SIM_OK )
	{

		UINT32 cbResponse = simCmdCnf->cnf.len - 2; //SW1+SW2=2 bytes;
		UINT32 sw1Offset = simCmdCnf->cnf.len - 2;
		UINT32 sw2Offset = simCmdCnf->cnf.len - 1;

		if ( cbResponse > 0 )
		{
			sprintf( simRspBuf, "+CRSM: %d,%d,", simCmdCnf->cnf.data[sw1Offset], simCmdCnf->cnf.data[sw2Offset] );

			/* Check if it is resonse of USIM or normal SIM */
			if ((simCmdCnf->cnf.data[0] == 0x62) && (simCmdCnf->cnf.data[2] == 0x82))
			{
				/* For USIM, it need to do convert  */
				libConvert2UsimResp(cbResponse, simCmdCnf->cnf.data, aucUSimCmdResponse, sizeof(aucUSimCmdResponse));

				for ( i = 0; i < sizeof(aucUSimCmdResponse); i++ )
				{
					sprintf( (char*)simRspBuf, "%s%02X", simRspBuf, aucUSimCmdResponse[i]);
				}
			}
			else
			{
				/* For normal SIM, it can return response direclty  */
				libConvertNumToHex((char *)(simCmdCnf->cnf.data), cbResponse, simRspBuf);
			}
		}
		else
		{
			sprintf( simRspBuf, "+CRSM: %d,%d", simCmdCnf->cnf.data[sw1Offset], simCmdCnf->cnf.data[sw2Offset] );
		}

		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, simRspBuf);

	}
	else
	{
		libDisplaySimRc(atHandle, simCmdCnf->rc);
	}

	return;
}

/****************************************************************
 *  F@: processRunGSMAlgorithmConf - Process CI cnf msg of AT+ERGA=
 */
static void processRunGSMAlgorithmConf(UINT32 atHandle, const void *paras)
{
	char simRspBuf[100];
	char tempBuf[20];
	UINT8 *pData;
	UINT32 i = 0;
	UINT32 len;
	CiSimPrimGenericCmdCnf    *simCmdCnf;

	simCmdCnf = (CiSimPrimGenericCmdCnf *)paras;

	if ( simCmdCnf->rc == CIRC_SIM_OK )
	{

		UINT32 cbResponse = simCmdCnf->cnf.len - 2; //SW1+SW2=2 bytes;

		if ( cbResponse == (RGA_SRES_LEN + RGA_CK_LEN) )
		{
			sprintf( simRspBuf, "+ERGA:");

			/* For normal SIM, it can return response direclty  */
			len = cbResponse;
			pData = simCmdCnf->cnf.data;

			/* format response data to string values*/
			for ( i = 0; i < len; i++ )
			{
				sprintf( tempBuf, "%02X", pData[i] );
				strcat( simRspBuf, tempBuf );

				if (i == (RGA_SRES_LEN - 1))
					strcat(simRspBuf, ",");
			}
			ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, simRspBuf);
		}
		else
		{
			ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
		}
	}
	else
	{
		libDisplaySimRc(atHandle, simCmdCnf->rc);
	}

	return;
}

/****************************************************************
 *  F@: processRunTContextAuthConf - Process CI cnf msg of AT+ERTCA=
 */
static void processRunTContextAuthConf(UINT32 atHandle, const void *paras)
{
	char simRspBuf[2 * CI_SIM_MAX_CMD_DATA_SIZE + 50];
	char tempBuf[80];
	UINT8 *pData;
	UINT32 i = 0;
	CiSimPrimGenericCmdCnf    *simCmdCnf;

	simCmdCnf = (CiSimPrimGenericCmdCnf *)paras;

	if ( simCmdCnf->rc == CIRC_SIM_OK )
	{
		UINT8 lenStatus, lenRES, lenCK, lenIK, lenKc, lenAUTS;
		UINT32 cbResponse = simCmdCnf->cnf.len - 2; //SW1+SW2=2 bytes;
		UINT32 sw1Offset = simCmdCnf->cnf.len - 2;
		UINT32 sw2Offset = simCmdCnf->cnf.len - 1;

		if ( cbResponse > 0 )
		{
			sprintf(simRspBuf, "+ERTCA:");

			/* 3GPP TS 31.102 v7.3.0 */
			if (simCmdCnf->cnf.data[0] == 0xDB) /*successful 3G authentication*/
			{
				/* Status */
				lenStatus = 1;
				strcat(simRspBuf, "0, ");

				/* RES */
				lenRES = simCmdCnf->cnf.data[lenStatus];
				pData = &simCmdCnf->cnf.data[lenStatus + 1];
				for (i = 0; i < lenRES; i++)
				{
					sprintf( tempBuf, "%02X", pData[i] );
					strcat(simRspBuf, tempBuf);
				}
				strcat(simRspBuf, ", ");

				/* CK */
				lenCK = simCmdCnf->cnf.data[lenStatus + 1 + lenRES];
				pData = &simCmdCnf->cnf.data[lenStatus + 1 + lenRES + 1];
				for (i = 0; i < lenCK; i++)
				{
					sprintf( tempBuf, "%02X", pData[i] );
					strcat(simRspBuf, tempBuf);
				}
				strcat(simRspBuf, ", ");

				/* IK */
				lenIK = simCmdCnf->cnf.data[lenStatus + 1 + lenRES + 1 + lenCK];
				pData = &simCmdCnf->cnf.data[lenStatus + 1 + lenRES + 1 + lenCK + 1];
				for (i = 0; i < lenIK; i++)
				{
					sprintf( tempBuf, "%02X", pData[i] );
					strcat(simRspBuf, tempBuf);
				}

				if (cbResponse >
					(UINT32)(lenStatus + 1 + lenRES + 1 + lenCK + 1 + lenIK))
				{
					strcat(simRspBuf, ", ");

					/* Kc */
					lenKc = simCmdCnf->cnf.data[lenStatus + 1 + lenRES + 1 + lenCK + 1 + lenIK];
					pData = &simCmdCnf->cnf.data[lenStatus + 1 + lenRES + 1 + lenCK + 1 + lenIK + 1];
					for (i = 0; i < lenKc; i++)
					{
						sprintf( tempBuf, "%02X", pData[i] );
						strcat(simRspBuf, tempBuf);
					}
				}
				strcat(simRspBuf, "\r\n");

			}
			else if (simCmdCnf->cnf.data[0] == 0xDC) /*synchronisation failure*/
			{
				/* Status */
				lenStatus = 1;
				strcat(simRspBuf, "1, ");

				/* AUTS */
				lenAUTS = simCmdCnf->cnf.data[lenStatus];
				pData = &simCmdCnf->cnf.data[lenStatus + 1];
				for (i = 0; i < lenAUTS; i++)
				{
					sprintf( tempBuf, "%02X", pData[i] );
					strcat(simRspBuf, tempBuf);
				}
				strcat(simRspBuf, "\r\n");
			}
			ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, simRspBuf);
		}
		else
		{
			if (simCmdCnf->cnf.data[sw1Offset] == 0x98 &&
				simCmdCnf->cnf.data[sw2Offset] == 0x62)
			{
				sprintf( simRspBuf, "+ERTCA: 2");
				ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, simRspBuf);
			}
			else if (simCmdCnf->cnf.data[sw1Offset] == 0x98 &&
					 simCmdCnf->cnf.data[sw2Offset] == 0x64)
			{
				sprintf( simRspBuf, "+ERTCA: 3");
				ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, simRspBuf);
			}
			else
			{
				ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
			}
		}
	}
	else
	{
		libDisplaySimRc(atHandle, simCmdCnf->rc);
	}

	return;
}

/****************************************************************
 *  F@: processGetPinStateConf - Process CI cnf msg of AT+CPIN? or AT+EPIN?
 */
static void processGetPinStateConf(UINT32 reqHandle, const void *paras)
{
	char simRspBuf[300];
	CiSimPrimGetPinStateCnf *simStateCnf = (CiSimPrimGetPinStateCnf *)paras;
	UINT32 atHandle = GET_AT_HANDLE(reqHandle);

	if ( simStateCnf->rc == CIRC_SIM_OK )
	{
		/* save info for validity checking of further operation */
		gSimPinState = simStateCnf->state;

		/* If AT+CPIN?,  display the pin state */
		if (GET_REQ_ID(reqHandle) == CI_SIM_PRIM_GET_PIN_STATE_REQ)
		{
			libDisplayPinState( atHandle, simStateCnf->state );
			ATRESP(atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		}

		/* If AT+EPIN?, display PIN/PUK left times */
		else if (GET_REQ_ID(reqHandle) == CI_SIM_PRIM_GET_EPIN_STATE_REQ)
		{
			sprintf(simRspBuf, "+EPIN:%d,%d,%d,%d\r\n", simStateCnf->chv1NumRetrys, simStateCnf->chv2NumRetrys,
				simStateCnf->puk1NumRetrys, simStateCnf->puk2NumRetrys);
			ATRESP(atHandle, ATCI_RESULT_CODE_OK, 0, simRspBuf);
		}
		/* if AT*SIMDETEC, display if SIM inserted*/
		else if(GET_REQ_ID(reqHandle) == CI_SIM_PRIM_GET_SIM_DETECT_REQ)
		{
			if(simStateCnf->state == CI_SIM_PIN_ST_REMOVED)
				ATRESP( atHandle, 0, 0, (char *)"*SIMDETEC: NOS");
			else
				ATRESP( atHandle, 0, 0, (char *)"*SIMDETEC: SIM");
			
			ATRESP(atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		}
		else
		{
			ERRMSG("invalid gCpinCmdType=%d", GET_REQ_ID(reqHandle) -  CI_SIM_PRIM_GET_PIN_STATE_REQ);
			ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
		}

	} /* if( simStateCnf->rc */
	else
	{
		if(GET_REQ_ID(reqHandle) == CI_SIM_PRIM_GET_SIM_DETECT_REQ && simStateCnf->rc == CIRC_SIM_NOT_INSERTED)
		{
			ATRESP( atHandle, 0, 0, (char *)"*SIMDETEC: NOS");
			ATRESP(atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		}
		else
		{
			libDisplaySimRc(atHandle, simStateCnf->rc);
		}
	}
	return;
}

/****************************************************************
 *  F@: processSetFacilityLockConf - Process CI cnf msg of AT+CLCK=<fac>,<mode = 0 or 1>
 */
static void processSetFacilityLockConf(UINT32 atHandle, const void *paras)
{
	CiSimPrimLockFacilityCnf  *lockFacCnf = (CiSimPrimLockFacilityCnf *)paras;

	if ( lockFacCnf->rc == CIRC_SIM_OK )
	{
		if (lockFacCnf->status == CI_SIM_FACLCK_ST_ACTIVE)
			ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+CLCK: 1");
		else
			ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+CLCK: 0");
	}
	else
	{
		libDisplaySimRc(atHandle, lockFacCnf->rc);
	}

	return;
}

/****************************************************************
 *  F@: processGetFacilityCapConf - Process CI cnf msg of AT+CLCK=<fac>,<mode = 2>
 */
static void processGetFacilityCapConf(UINT32 atHandle, const void *paras)
{
	char simRspBuf[300];
	CiSimPrimGetFacilityCapCnf  *getFacCapCnf = (CiSimPrimGetFacilityCapCnf *)paras;

	if ( getFacCapCnf->rc == CIRC_SIM_OK )
	{
		if ( getFacCapCnf->bitsFac == 0 )
		{
			ATRESP( atHandle, 0, 0, (char *)"+CLCK: 0\r\n");
		}
		else
		{
			if ( gSSCntx.queryCallClassStatus & CISS_BSMAP_VOICE )
			{
				sprintf(simRspBuf, "+CLCK: %d, 1\r\n", ( getFacCapCnf->bitsFac & CISS_BSMAP_VOICE ) );
				ATRESP( atHandle, 0, 0, simRspBuf);
			}
			if ( gSSCntx.queryCallClassStatus & CISS_BSMAP_DATA )
			{
				sprintf(simRspBuf, "+CLCK: %d, 2\r\n", ( getFacCapCnf->bitsFac & CISS_BSMAP_DATA ) );
				ATRESP( atHandle, 0, 0, simRspBuf);
			}
			if ( gSSCntx.queryCallClassStatus & CISS_BSMAP_FAX )
			{
				sprintf(simRspBuf, "+CLCK: %d, 4\r\n", ( getFacCapCnf->bitsFac & CISS_BSMAP_FAX ) );
				ATRESP( atHandle, 0, 0, simRspBuf);
			}
			if ( gSSCntx.queryCallClassStatus & CISS_BSMAP_SMS )
			{
				sprintf(simRspBuf, "+CLCK: %d, 8\r\n", ( getFacCapCnf->bitsFac & CISS_BSMAP_SMS ) );
				ATRESP( atHandle, 0, 0, simRspBuf);
			}
			if ( gSSCntx.queryCallClassStatus & CISS_BSMAP_DATA_SYNC )
			{
				sprintf(simRspBuf, "+CLCK: %d, 16\r\n", ( getFacCapCnf->bitsFac & CISS_BSMAP_DATA_SYNC ) );
				ATRESP( atHandle, 0, 0, simRspBuf);
			}
			if ( gSSCntx.queryCallClassStatus & CISS_BSMAP_DATA_ASYNC )
			{
				sprintf(simRspBuf, "+CLCK: %d, 32\r\n", ( getFacCapCnf->bitsFac & CISS_BSMAP_DATA_ASYNC ) );
				ATRESP( atHandle, 0, 0, simRspBuf);
			}
			if ( gSSCntx.queryCallClassStatus & CISS_BSMAP_PACKET )
			{
				sprintf(simRspBuf, "+CLCK: %d, 64\r\n", ( getFacCapCnf->bitsFac & CISS_BSMAP_PACKET ) );
				ATRESP( atHandle, 0, 0, simRspBuf);
			}
			if ( gSSCntx.queryCallClassStatus & CISS_BSMAP_PAD )
			{
				sprintf(simRspBuf, "+CLCK: %d, 128\r\n", ( getFacCapCnf->bitsFac & CISS_BSMAP_PAD ) );
				ATRESP( atHandle, 0, 0, simRspBuf);
			}
		}
	}
	else
	{
		libDisplaySimRc(atHandle, getFacCapCnf->rc);
	}

	ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
	gSSCntx.queryCallClassStatus = 0;
	return;
}

/****************************************************************
 *  F@: processGetImsiConf - Process CI cnf msg of AT+CIMI
 */
static void processGetImsiConf(UINT32 atHandle, const void *paras)
{
	char simRspBuf[300];
	UINT32 i, len;
	UINT8 errorDigitFlag = FALSE; 
	CiSimPrimGetSubscriberIdCnf  *getSubIDCnf = (CiSimPrimGetSubscriberIdCnf*)paras;
	if ( getSubIDCnf->rc == CIRC_SIM_OK )
	{
		len = getSubIDCnf->subscriberStr.len;
		memcpy( simRspBuf, getSubIDCnf->subscriberStr.valStr, len);
		simRspBuf[len] = '\0';
		//strcpy( simRspBuf, "1234??789?");
		//here add str numeric check     
		for(i = 0; i < len; i++) 
		{
			if(!isdigit(simRspBuf[i])) 
			{
		    		errorDigitFlag = TRUE;
		    }
		}
		if(errorDigitFlag == TRUE)
		{
			ERRMSG("Imsi is %s\n", simRspBuf);
			ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
			errorDigitFlag = FALSE;
		}
		else
		{	
			ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, simRspBuf );
		}

	}
	else
	{
		libDisplaySimRc(atHandle, getSubIDCnf->rc);
	}
	return;
}

/****************************************************************
 *  F@: processStkEnableProactiveCmdIndConfig - Process CI cnf msg of AT+MSTK=0,<flag>
 */
static void processStkEnableProactiveCmdIndConfig(UINT32 atHandle, const void *paras)
{
	CiSimPrimEnableSimatIndsCnf  *enableSimatIndsCnf = (CiSimPrimEnableSimatIndsCnf*)paras;

	if ( enableSimatIndsCnf->rc == CIRC_SIM_OK )
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL );
	}
	else
	{
		libDisplaySimRc(atHandle, enableSimatIndsCnf->rc);
	}
	return;
}

/****************************************************************
 *  F@: processStkGetSimProfile - Process CI cnf msg of AT+MSTK=3, return +MSTK: 3,<SIM profile data>
 */
static void processStkGetSimProfile(UINT32 atHandle, const void *paras)
{
	char simRspBuf[300];
	CiSimPrimGetTerminalProfileCnf  *getTerminalProfileCnf = (CiSimPrimGetTerminalProfileCnf*)paras;

	if ( getTerminalProfileCnf->rc == CIRC_SIM_OK )
	{
		sprintf( simRspBuf, "+MSTK:%d, ", STK_CMD_GET_PROFILE);
		libConvertNumToHex((char *)(getTerminalProfileCnf->profile.data), getTerminalProfileCnf->profile.len, simRspBuf);
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, simRspBuf );
	}
	else
	{
		libDisplaySimRc(atHandle, getTerminalProfileCnf->rc);
	}
	return;
}

/****************************************************************
 *  F@: processStkDownloadMeProfile - Process CI cnf msg of AT+MSTK=1,<ME profile data>
 */
static void processStkDownloadMeProfile(UINT32 atHandle, const void *paras)
{
	CiSimPrimDownloadProfileCnf  *downloadProfileCnf = (CiSimPrimDownloadProfileCnf*)paras;

	if ( downloadProfileCnf->rc == CIRC_SIM_OK )
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL );
	}
	else
	{
		libDisplaySimRc(atHandle, downloadProfileCnf->rc);
	}
	return;
}

/****************************************************************
 *  F@: processStkSendEnvelopeCmd - Process CI cnf msg of AT+MSTK=4, < Envelope command data >
 *  return +MSTK: 4,<length>,<cmd response> (such as 9000)
 */
static void processStkSendEnvelopeCmd(UINT32 atHandle, const void *paras)
{
	char simRspBuf[2 * CI_SIM_MAX_CMD_DATA_SIZE + 50];
	CiSimPrimEnvelopeCmdCnf  *envelopeCmdCnf = (CiSimPrimEnvelopeCmdCnf*)paras;

	if ( envelopeCmdCnf->rc == CIRC_SIM_OK )
	{
		sprintf( simRspBuf, "+MSTK: %d, ", STK_CMD_SEND_ENVELOPE);
		libConvertNumToHex((char *)(envelopeCmdCnf->cnf.data), envelopeCmdCnf->cnf.len, simRspBuf);
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL );
	}
	else
	{
		libDisplaySimRc(atHandle, envelopeCmdCnf->rc);
	}
	return;
}

/****************************************************************
 *  F@: processStkGetSimCapInfo - Process CI cnf msg of AT+MSTK=2
 *  return +MSTK: 2,<cap info length>,<SIM toolkit capability info>
 */
static void processStkGetSimCapInfo(UINT32 atHandle, const void *paras)
{
	char simRspBuf[300], tmpBuf[200];
	CiSimPrimGetSimatNotifyCapCnf  *getCapCnf = (CiSimPrimGetSimatNotifyCapCnf*)paras;

	if ( getCapCnf->rc == CIRC_SIM_OK )
	{
		sprintf( simRspBuf, "+MSTK: %d, ", STK_CMD_GET_CAP_INFO);
		sprintf(tmpBuf, "%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
			getCapCnf->caps.capDisplayText, getCapCnf->caps.capGetInkey, getCapCnf->caps.capGetInput,
			getCapCnf->caps.capMoreTime, getCapCnf->caps.capPlayTone, getCapCnf->caps.capPollInterval,
			getCapCnf->caps.capRefresh, getCapCnf->caps.capSetupMenu, getCapCnf->caps.capSelectItem,
			getCapCnf->caps.capSendSMS, getCapCnf->caps.capSendSS, getCapCnf->caps.capSendUSSD,
			getCapCnf->caps.capSetupCall, getCapCnf->caps.capPollingOff, getCapCnf->caps.capSetupIdleModeText,
			getCapCnf->caps.capProvideLocalInfo, getCapCnf->caps.capSetupEventList, getCapCnf->caps.capLaunchBrowser);
		strcat(simRspBuf, tmpBuf);
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, simRspBuf );
	}
	else
	{
		libDisplaySimRc(atHandle, getCapCnf->rc);
	}
	return;
}

/****************************************************************
 *  F@: processSetVSim - Process CI cnf msg of AT*EnVsim
 *  return EnVsim: OK/Fail
 */
static void processSetVSim(UINT32 atHandle, const void *paras)
{
	CiSimPrimSetVSimCnf *setVSimCnf = (CiSimPrimSetVSimCnf *)paras;

	if ( setVSimCnf->result == CIRC_SIM_OK )
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
	}
	else
	{
		libDisplaySimRc(atHandle, setVSimCnf->result);
	}
}

/****************************************************************
 *  F@: processGetVSim - Process CI cnf msg of AT*ENVSIM?
 *  return *ENVSIM: (0,1)status
 */
static void processGetVSim(UINT32 atHandle, const void *paras)
{
	CiSimPrimGetVSimCnf *getVSimCnf = (CiSimPrimGetVSimCnf *)paras;

	if ( getVSimCnf->result == CIRC_SIM_OK )
	{
		if (getVSimCnf->status == TRUE)
			ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"*ENVSIM: 1");
		else
			ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"*ENVSIM: 0");
	}
	else
	{
		libDisplaySimRc(atHandle, getVSimCnf->result);
	}
}

/****************************************************************
 *  F@: processGetVSim - Process CI cnf msg of AT*MEPCG=
 */
static void processMEPReadCodes(UINT32 atHandle, const void *paras)
{
	CiSimPrimReadMEPCodesCnf *readMEPCodesCnf = (CiSimPrimReadMEPCodesCnf *)paras;
	char simRspBuf[100],simRspTmpBuf[100];
	UINT32 i = 0;

	memset(simRspBuf,0,100);
	memset(simRspTmpBuf,0,100);

	if( readMEPCodesCnf->rc == CIRC_SIM_OK )
	{
		sprintf(simRspBuf, "*MEPCG:");
		switch (readMEPCodesCnf->MEPCategory)
		{
			case CI_SIM_PERS_CAT_SIM:
				{
					int len = 0;
					char ImsiStr[CI_SIM_MEP_MAX_IMSI_LENGTH + 1];
					char tmpIMSI[16];
					for(i = 0; i < readMEPCodesCnf->NumberOfcodes && i < CI_SIM_MEP_MAX_NUMBER_OF_CODES; i++)
					{
						memset(ImsiStr,0,CI_SIM_MEP_MAX_IMSI_LENGTH + 1);
						memset(tmpIMSI,'0',15);
						for(len = 0; len < readMEPCodesCnf->Codes[i].SimUsim.simId.length && len < CI_SIM_MEP_MAX_IMSI_LENGTH; len++)
						{
							ImsiStr[len] = readMEPCodesCnf->Codes[i].SimUsim.simId.contents[len];
						}

						if(readMEPCodesCnf->Codes[i].SimUsim.simId.length > 0)
						{
							tmpIMSI[0]+=ImsiStr[0]>>4;
							int j = 1;
							for(;j<readMEPCodesCnf->Codes[i].SimUsim.simId.length;j++)
							{
								tmpIMSI[2*j-1]+=ImsiStr[j]&0x0F;
								tmpIMSI[2*j]+=ImsiStr[j]>>4;
							}
							tmpIMSI[15]=0;
							if(i > 0)
								sprintf(simRspTmpBuf, ", %s", tmpIMSI);
							else
								sprintf(simRspTmpBuf, "%s", tmpIMSI);
							strcat((char *)simRspBuf, (char *)simRspTmpBuf);
						}
					}
					break;
				}
			case CI_SIM_PERS_CAT_NETWORK:
				{
					for(i = 0; i < readMEPCodesCnf->NumberOfcodes && i < CI_SIM_MEP_MAX_NUMBER_OF_CODES; i++)
					{
						if(readMEPCodesCnf->Codes[i].SimUsim.simId.length > 0)
						{
							if(i > 0)
							{
								if(readMEPCodesCnf->Codes[i].Network.mncThreeDigitsDecoding)
									sprintf(simRspTmpBuf, ", %.3x%.3x", readMEPCodesCnf->Codes[i].Network.plmn.mcc, readMEPCodesCnf->Codes[i].Network.plmn.mnc);
								else
									sprintf(simRspTmpBuf, ", %.3x%.2x", readMEPCodesCnf->Codes[i].Network.plmn.mcc, readMEPCodesCnf->Codes[i].Network.plmn.mnc);
							}
							else
							{
								if(readMEPCodesCnf->Codes[i].Network.mncThreeDigitsDecoding)
									sprintf(simRspTmpBuf, "%.3x%.3x", readMEPCodesCnf->Codes[i].Network.plmn.mcc, readMEPCodesCnf->Codes[i].Network.plmn.mnc);
								else
									sprintf(simRspTmpBuf, "%.3x%.2x", readMEPCodesCnf->Codes[i].Network.plmn.mcc, readMEPCodesCnf->Codes[i].Network.plmn.mnc);
							}
							strcat((char *)simRspBuf, (char *)simRspTmpBuf);
						}
					}
					break;
				}
			case CI_SIM_PERS_CAT_NETWORKSUBSET:
				{
					for(i = 0; i < readMEPCodesCnf->NumberOfcodes && i < CI_SIM_MEP_MAX_NUMBER_OF_CODES; i++)
					{
						if(i > 0)
							sprintf(simRspTmpBuf, ", %d", readMEPCodesCnf->Codes[i].NetworkSubset.networkSubsetId);
						else
							sprintf(simRspTmpBuf, "%d", readMEPCodesCnf->Codes[i].NetworkSubset.networkSubsetId);
						strcat((char *)simRspBuf, (char *)simRspTmpBuf);
					}
					break;
				}
			case CI_SIM_PERS_CAT_SERVICEPROVIDER:
				{
					for(i = 0; i < readMEPCodesCnf->NumberOfcodes && i < CI_SIM_MEP_MAX_NUMBER_OF_CODES; i++)
					{
						if(i > 0)
							sprintf(simRspTmpBuf, ", %d", readMEPCodesCnf->Codes[i].SP.serviceproviderId);
						else
							sprintf(simRspTmpBuf, "%d", readMEPCodesCnf->Codes[i].SP.serviceproviderId);
						strcat((char *)simRspBuf, (char *)simRspTmpBuf);
					}
					break;
				}
			case CI_SIM_PERS_CAT_CORPORATE:
				{
					for(i = 0; i < readMEPCodesCnf->NumberOfcodes && i < CI_SIM_MEP_MAX_NUMBER_OF_CODES; i++)
					{
						if(i > 0)
							sprintf(simRspTmpBuf, ", %d", readMEPCodesCnf->Codes[i].Corporate.corporateId);
						else
							sprintf(simRspTmpBuf, "%d", readMEPCodesCnf->Codes[i].Corporate.corporateId);
						strcat((char *)simRspBuf, (char *)simRspTmpBuf);
					}
					break;
				}
		}
		ATRESP( atHandle,ATCI_RESULT_CODE_OK,0,simRspBuf);
	}
	else
	{
		libDisplaySimRc(atHandle, readMEPCodesCnf->rc);
	}
}

/****************************************************************
 *  F@: processGetVSim - Process CI cnf msg of AT*CLCK=
 */
static void processPersonalizeME(UINT32 reqHandle, const void *paras)
{
	CiSimPrimPersonalizeMECnf *personalizeMECnf = (CiSimPrimPersonalizeMECnf *)paras;
	char simRspBuf[100];
	memset(simRspBuf,0,100);
	int status;

	if(personalizeMECnf->rc == CIRC_SIM_OK)
	{
		switch(GET_REQ_ID(reqHandle))
		{
		case CI_SIM_PRIM_MEP_READ_STATUS_EXT_REQ:
			sprintf(simRspBuf, "*CLCK: %d", personalizeMECnf->status);
			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, (char *)simRspBuf);
			break;

		case CI_SIM_PRIM_MEP_READ_STATUS_REQ:
			if(personalizeMECnf->status == CI_SIM_MEP_ACTIVATED || personalizeMECnf->status == CI_SIM_MEP_BLOCKED)
				status = 1; // activate
			else
				status = 0; // not activate
			sprintf(simRspBuf, "+CLCK: %d", status);
			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, (char *)simRspBuf);
			break;

		case CI_SIM_PRIM_MEP_SET_REQ:
			ATRESP( reqHandle, ATCI_RESULT_CODE_OK, 0, NULL );
			break;

		default:
			ATRESP( reqHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
			break;
		}
	}
	else
	{
		libDisplaySimRc(reqHandle, personalizeMECnf->rc);
	}
}


/****************************************************************************************
*
*  DESCRIPTION: Callback function when receiving confirmation CI message related with SIM
*
****************************************************************************************/
void simCnf(CiSgOpaqueHandle opSgCnfHandle,
	    CiServiceGroupID svgId,
	    CiPrimitiveID primId,
	    CiRequestHandle reqHandle,
	    void*             paras)
{
	UNUSEDPARAM(opSgCnfHandle)

	UINT32 atHandle = GET_AT_HANDLE(reqHandle);

	/* Validate the para pointer when primId is valid */
	if ( (paras == NULL) && (primId < CI_SIM_PRIM_LAST_COMMON_PRIM) )
	{
		ERRMSG("para pointer of CI cnf msg is NULL!");
		return;
	}

	/*
	**  Determine the primitive being confirmed.
	*/
	switch (primId)
	{
	case CI_SIM_PRIM_GENERIC_CMD_CNF:
	{
		switch (GET_REQ_ID(reqHandle))
		{
		case CI_SIM_PRIM_GENERIC_CMD_ERGA_REQ:
			processRunGSMAlgorithmConf(atHandle, paras);
		break;
		case CI_SIM_PRIM_GENERIC_CMD_ERTCA_REQ:
			processRunTContextAuthConf(atHandle, paras);
		break;
		default:
			processRestrictedSimAccessConf(atHandle, paras);
		break;
		}

		break;
	}

	case CI_SIM_PRIM_EXECCMD_CNF:
	{
		processGenericSimAccessConf(atHandle, paras);
		break;
	}

	case CI_SIM_PRIM_OPERCHV_CNF:
	{
		processEnterPinConf(reqHandle, paras);
		break;
	}

	case CI_SIM_PRIM_GET_PIN_STATE_CNF:
	{
		processGetPinStateConf(reqHandle, paras);
		break;
	}

	case CI_SIM_PRIM_LOCK_FACILITY_CNF:
	{
		processSetFacilityLockConf(atHandle, paras);
		break;
	}

	case CI_SIM_PRIM_GET_FACILITY_CAP_CNF:
	{
		processGetFacilityCapConf(atHandle, paras);
		break;
	}

	case CI_SIM_PRIM_GET_SUBSCRIBER_ID_CNF:
	{
		processGetImsiConf(atHandle, paras);
		break;
	}

	/* Following are processing to STK cnf msg */
	case CI_SIM_PRIM_ENABLE_SIMAT_INDS_CNF:
	{
		processStkEnableProactiveCmdIndConfig(atHandle, paras);
		break;
	}

	case CI_SIM_PRIM_GET_TERMINALPROFILE_CNF:
	{
		processStkGetSimProfile(atHandle, paras);
		break;
	}

	case CI_SIM_PRIM_GET_SIMAT_NOTIFY_CAP_CNF:
	{
		processStkGetSimCapInfo(atHandle, paras);
		break;
	}

	case CI_SIM_PRIM_DOWNLOADPROFILE_CNF:
	{
		processStkDownloadMeProfile(atHandle, paras);
		break;
	}

	case CI_SIM_PRIM_ENVELOPE_CMD_CNF:
	{
		processStkSendEnvelopeCmd(atHandle, paras);
		break;
	}

	case CI_SIM_PRIM_SET_VSIM_CNF:
	{
		processSetVSim(atHandle, paras);
		break;

	}
	case CI_SIM_PRIM_GET_VSIM_CNF:
	{
		processGetVSim(atHandle, paras);
		break;

	}

	case CI_ERR_PRIM_SIMNOTREADY_CNF:
	case CI_ERR_PRIM_ISINVALIDREQUEST_CNF:
	{
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_SIM_FAILURE, NULL);
		break;
	}

	/* Error cnf msg */
	case CI_ERR_PRIM_HASNOSUPPORT_CNF:
	case CI_ERR_PRIM_HASINVALIDPARAS_CNF:
	{
		/* Reply with an "Operation not supported" error (CME ERROR 4) */
		ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
		break;
	}

	case CI_SIM_PRIM_READ_MEP_CODES_CNF:
	{
		processMEPReadCodes(atHandle, paras);
		break;
	}

	case CI_SIM_PRIM_PERSONALIZEME_CNF:
	{
		processPersonalizeME(reqHandle, paras);
		break;
	}

	default:
		break;
	}

	/* free up the confirmation memory */
	atciSvgFreeCnfMem( svgId, primId, paras );

	return;
}

/****************************************************************
 *  F@: processRegStatusInd - Process CI ind msg of +CPIN:
 */
static void processDevStatusInd(const void *pParam)
{
	CiSimPrimDeviceInd  *simDevInd = (CiSimPrimDeviceInd *)pParam;

	/* save info for validity checking of further operation */
	gSimStatus = simDevInd->status;
	gSimPinState = simDevInd->pinState;

	/* display the pin status */
	libDisplayPinState(IND_REQ_HANDLE, gSimPinState );

	return;
}

/***************************************************************
 * F@: processMmiStateInd - Process CI ind msg of *CkMMI
 */
static void processMmiStatusInd(const void *pParami, CiIndicationHandle respHandle)
{
	UNUSEDPARAM(pParami);
	CiSimPrimCheckMMIStateRsp MmiStatusRsp;
	MmiStatusRsp.MMIState = CI_SIM_MMI_IDLE;
	// send the response
	ciRespond( gAtciSvgHandle[CI_SG_ID_SIM],CI_SIM_PRIM_CHECK_MMI_STATE_RSP,
	 		respHandle, (void *)&MmiStatusRsp);
	return;
}


/****************************************************************
 *  F@: processStkProactiveCmdInd - Process CI ind msg of proactive cmd and report as +MSTK: 11, < proactive command >
 */
static void processStkProactiveCmdInd(const void *pParam)
{
	CiSimPrimProactiveCmdInd  *proactiveCmdInd = (CiSimPrimProactiveCmdInd *)pParam;
	char* simIndBuf;	
#ifdef AT_PARSE_REFRESH
	char customerRefreshIndBuf[100]= "\0";
	char androidRefreshIndBuf[100]= "\0";
	BOOL result;
#endif

	simIndBuf = malloc(2*proactiveCmdInd->len+20);

	sprintf( simIndBuf, "+MSTK: %d, ", STK_CMD_PROACTIVE);
	libConvertNumToHex((char *)(proactiveCmdInd->data), proactiveCmdInd->len, simIndBuf);

	ATRESP( IND_REQ_HANDLE, 0, 0, simIndBuf );
	free(simIndBuf);	

#ifdef AT_PARSE_REFRESH
	//if application send TR for it, please remove send TR in libProcessRefresh function.
	result = libProcessRefresh(proactiveCmdInd, customerRefreshIndBuf, androidRefreshIndBuf);
	if(result == TRUE) {
		//Now we support one indication at the same time
#ifdef AT_CUSTOMER_HTC
		ATRESP( IND_REQ_HANDLE, 0, 0, customerRefreshIndBuf);
#else
		ATRESP( IND_REQ_HANDLE, 0, 0, androidRefreshIndBuf);
#endif
	} else {
		//do nothing
	}
#endif

	return;
}

/*1. new add for CI_SIM_PRIM_SIMAT_CC_STATUS_IND*/
static void processStkSIMATCcStatusInd(const void *pParam)
{
	CiSimPrimSIMATCcStatusInd  *statusInd = (CiSimPrimSIMATCcStatusInd *)pParam;

#ifdef AT_CUSTOMER_HTC
	char simIndBuf[ CI_MAX_ADDRESS_LENGTH * 2 + ATCI_NULL_TERMINATOR_LENGTH + 2 * CI_SIMAT_MAX_ALPHATAG_LENGTH + 60];
	char tmpBuf[10];


	//report alphaIdPresent and addressPresent to RIL
	//"D8 len 01 02 status operationType 
	//alphaIdPresent alphaIdLen 
	//addressPresent Length (alphaTag, AddrType.numType, AddrType.numPlan, Digits)
	sprintf( simIndBuf, "+MSTK: %d, D8%02X0102%02X%02X%02X%02X%02X%02X", STK_CMD_SETUP_CALL_REQ_STATUS, 
			8 + statusInd->alphaId.len + statusInd->AddressInfo.Length + (statusInd->addressPresent ? 2 : 0),
			(unsigned char)statusInd->status, (unsigned char)statusInd->OperationType,
			(unsigned char)statusInd->alphaIdPresent, (unsigned char)statusInd->alphaId.len,
			(unsigned char)statusInd->addressPresent, (unsigned char)statusInd->AddressInfo.Length);

	if(statusInd->alphaIdPresent) {
		libConvertNumToHex((char *)(statusInd->alphaId.tag), statusInd->alphaId.len, simIndBuf);
	}

	if(statusInd->addressPresent) {
		sprintf( tmpBuf, "%02X%02X", (unsigned char)statusInd->AddressInfo.AddrType.NumType, (unsigned char)statusInd->AddressInfo.AddrType.NumPlan);
		strcat( simIndBuf, tmpBuf);
		libConvertNumToHex((char *)(statusInd->AddressInfo.Digits), statusInd->AddressInfo.Length, simIndBuf);
	}
#else
    char simIndBuf[2 * CI_SIMAT_MAX_ALPHATAG_LENGTH + 60];
    sprintf( simIndBuf, "+MSTK: %d, D8%02X0102%02X%02X%02X", STK_CMD_SETUP_CALL_REQ_STATUS, 4 + 1 + statusInd->alphaId.len,
    	(unsigned char)statusInd->status, (unsigned char)statusInd->OperationType, (unsigned char)statusInd->alphaId.len);
    libConvertNumToHex((char *)(statusInd->alphaId.tag), statusInd->alphaId.len, simIndBuf);
#endif

	ATRESP( IND_REQ_HANDLE, 0, 0, simIndBuf );

	return;
}

/****************************************************************
 *  F@: processStkCallSetupInd - Process CI ind msg of set up call request from STK
 */
static void processStkCallSetupInd(const void *pParam)
{
	CiSimPrimGetCallSetupAckInd  *getCallSetupAckInd = (CiSimPrimGetCallSetupAckInd *)pParam;
	char simIndBuf[2 * CI_SIMAT_MAX_ALPHATAG_LENGTH + 60];
	if(getCallSetupAckInd->alphaId.len > 127) 
	{
        sprintf( simIndBuf, "+MSTK: %d, D0%02X%02X01030110000202818205%02X%02X", STK_CMD_SETUP_CALL, 129, 11 + getCallSetupAckInd->alphaId.len, 129, getCallSetupAckInd->alphaId.len);
	}
	else if((getCallSetupAckInd->alphaId.len + 11) > 127)
	{
        sprintf( simIndBuf, "+MSTK: %d, D0%02X%02X01030110000202818205%02X", STK_CMD_SETUP_CALL, 129, 11 + getCallSetupAckInd->alphaId.len, getCallSetupAckInd->alphaId.len);
	}
	else
	{
	    sprintf( simIndBuf, "+MSTK: %d, D0%02X01030110000202818205%02X", STK_CMD_SETUP_CALL, 11 + getCallSetupAckInd->alphaId.len, getCallSetupAckInd->alphaId.len);
	}
	libConvertNumToHex((char *)(getCallSetupAckInd->alphaId.tag), getCallSetupAckInd->alphaId.len, simIndBuf);
	ATRESP( IND_REQ_HANDLE, 0, 0, simIndBuf );

	return;
}


/*2. new add for CI_SIM_PRIM_SIMAT_SEND_CALL_SETUP_RSP_IND*/
static void processStkSIMATSendCallSetupRspInd(const void *pParam)
{
	CiSimPrimSIMATSendCallSetupRspInd  *rspInd = (CiSimPrimSIMATSendCallSetupRspInd *)pParam;
	char simIndBuf[2 * CI_SIMAT_MAX_ALPHATAG_LENGTH + 60];

	sprintf( simIndBuf, "+MSTK: %d, D9030101%02X", STK_CMD_SETUP_CALL_RESULT, (unsigned char)rspInd->status);
	ATRESP( IND_REQ_HANDLE, 0, 0, simIndBuf );

	return;
}

/*3. new add for CI_SIM_PRIM_SIMAT_SEND_SS_USSD_RSP_IND*/
static void processStkSIMATSendSsUssdRspInd(const void *pParam)
{
	CiSimPrimSIMATSendSsUssdRspInd  *rspInd = (CiSimPrimSIMATSendSsUssdRspInd *)pParam;
	char simIndBuf[2 * CI_SIMAT_MAX_ALPHATAG_LENGTH + 60];

	sprintf( simIndBuf, "+MSTK: %d, DC040101%02X%02X", STK_CMD_PROACTIVE_SEND_USSD_RESULT, (unsigned char)rspInd->status, (unsigned char)rspInd->OperationType);
	ATRESP( IND_REQ_HANDLE, 0, 0, simIndBuf );

	return;
}

/*4. new add for CI_SIM_PRIM_SIMAT_SM_CONTROL_STATUS_IND*/
static void processStkSIMATSmControlStatusInd(const void *pParam)
{
	CiSimPrimSIMATSmControlStatusInd  *statusInd = (CiSimPrimSIMATSmControlStatusInd *)pParam;
	char simIndBuf[2 * CI_SIMAT_MAX_ALPHATAG_LENGTH + 60];

	sprintf( simIndBuf, "+MSTK: %d, DA%02X0101%02X%02X", STK_CMD_PROACTIVE_SEND_SM_REQ_STATUS, 4 + statusInd->alphaId.len,
			(unsigned char)statusInd->status,(unsigned char)statusInd->alphaId.len);
	libConvertNumToHex((char *)(statusInd->alphaId.tag), statusInd->alphaId.len, simIndBuf);
	ATRESP( IND_REQ_HANDLE, 0, 0, simIndBuf );

	return;
}

/*5. new add for CI_SIM_PRIM_SIMAT_SEND_SM_RSP_IND*/
static void processStkSIMATSendSmRspInd(const void *pParam)
{
	CiSimPrimSIMATSendSmRspInd  *rspInd = (CiSimPrimSIMATSendSmRspInd *)pParam;
	char simIndBuf[2 * CI_SIMAT_MAX_ALPHATAG_LENGTH + 60];

	sprintf( simIndBuf, "+MSTK: %d, DB030101%02X", STK_CMD_PROACTIVE_SEND_SM_RESULT, (unsigned char)rspInd->status);
	ATRESP( IND_REQ_HANDLE, 0, 0, simIndBuf );

	return;
}
/****************************************************************
 *  F@: processStkDisplayInfoInd - Process CI ind msg of display info request from STK
 */
static void processStkDisplayInfoInd(const void *pParam)
{
	CiSimPrimSimatDisplayInfoInd  *simatDisplayInfoInd = (CiSimPrimSimatDisplayInfoInd *)pParam;
	char simIndBuf[2 * CI_SIMAT_MAX_ALPHATAG_LENGTH + 60];
	char tmpBuf[10] = "\0";

	//ignore invalid Display information.
	if(simatDisplayInfoInd->type > CI_SIMAT_DISPLAYCMD_SD )
		return;

	//11.14 
	//section 6.6.9 SEND SHORT MESSAGE(0x13)
	//section 6.6.10 SEND SS (0x11)
	//section 6.6.11 SEND USSD(0x12)
	//Proactive SIM Command Tag, Length, Command details, Device identities, Alpha identifier, string, Icon identifier

	if(simatDisplayInfoInd->alphaId.len > 127)   
	{
	  sprintf( simIndBuf, "+MSTK: %d, D0%02X%02X810301%02X008202818305%02X%02X", STK_CMD_DISPLAY_INFO, 129, (11 + simatDisplayInfoInd->alphaId.len + (simatDisplayInfoInd->iconIdPresent?4:0)), 
			  (19 - simatDisplayInfoInd->type), 129, simatDisplayInfoInd->alphaId.len);
	} 
	else if ((simatDisplayInfoInd->alphaId.len + 11 + (simatDisplayInfoInd->iconIdPresent?4:0)) > 127)
	{
	  sprintf( simIndBuf, "+MSTK: %d, D0%02X%02X810301%02X008202818305%02X", STK_CMD_DISPLAY_INFO, 129, (11 + simatDisplayInfoInd->alphaId.len + (simatDisplayInfoInd->iconIdPresent?4:0)), 
			  (19 - simatDisplayInfoInd->type), simatDisplayInfoInd->alphaId.len);	
	}
	else
	{
	  sprintf( simIndBuf, "+MSTK: %d, D0%02X810301%02X008202818305%02X", STK_CMD_DISPLAY_INFO, (11 + simatDisplayInfoInd->alphaId.len + (simatDisplayInfoInd->iconIdPresent?4:0)), 
			  (19 - simatDisplayInfoInd->type), simatDisplayInfoInd->alphaId.len);
	}
	libConvertNumToHex((char *)(simatDisplayInfoInd->alphaId.tag), simatDisplayInfoInd->alphaId.len, simIndBuf);

	if(simatDisplayInfoInd->iconIdPresent) {
		sprintf( tmpBuf, "1E02%02X%02X", simatDisplayInfoInd->iconId.display, simatDisplayInfoInd->iconId.tag );
		strcat( simIndBuf, tmpBuf);
	}        

	ATRESP( IND_REQ_HANDLE, 0, 0, simIndBuf );

	return;
}

/****************************************************************
 *  F@: processStkEndSessionInd - Process CI ind msg of end session from STK (no para)
 */
static void processStkEndSessionInd(const void *pParam)
{
	UNUSEDPARAM(pParam)
	char simIndBuf[300];

	sprintf( simIndBuf, "+MSTK: %d", STK_CMD_END_SESSION);
	ATRESP( IND_REQ_HANDLE, 0, 0, simIndBuf );

	return;
}

/****************************************************************
 *  F@: processStkCardInd - Process CI ind msg of card type from STK (no para)
 */
static void processStkCardInd(const void *pParam)
{
	UNUSEDPARAM(pParam)
	CiSimPrimCardInd *simPrimCardInd = (CiSimPrimCardInd *)pParam;
	char simIndBuf[50];
#ifdef AT_CUSTOMER_HTC
	if(simPrimCardInd->status == CI_SIM_ST_NOT_READY)
		gSimCardType = AT_SIM_CARD_TYPE_UNKNOWN;
	else
		gSimCardType = (simPrimCardInd->cardIsUicc << SIM_CARD_TYPE_USIM_BIT) + (simPrimCardInd->isTestCard << SIM_CARD_TYPE_TEST_BIT);
#else
	gSimCardType = simPrimCardInd->cardIsUicc;
#endif
	getSimTypeInd = TRUE;
	sprintf( simIndBuf, "*EUICC: %d", gSimCardType);
	ATRESP( IND_REQ_HANDLE, 0, 0, simIndBuf );

	return;
}

/****************************************************************************************
*
*  DESCRIPTION: Callback function when receiving indication CI message related with SIM
*
****************************************************************************************/
void simInd(CiSgOpaqueHandle opSgIndHandle,
	    CiServiceGroupID svgId,
	    CiPrimitiveID primId,
	    CiIndicationHandle indHandle,
	    void*               pParam)
{
	UNUSEDPARAM(opSgIndHandle)

	switch (primId)
	{
	case CI_SIM_PRIM_DEVICE_IND:
	{
		processDevStatusInd(pParam);
		break;
	}

	/* Linda add support MMI States check */
	case CI_SIM_PRIM_CHECK_MMI_STATE_IND:
	{
	    	processMmiStatusInd(pParam, indHandle);
		break;
	}

	/* Following are processing of STK ind msg */
	case CI_SIM_PRIM_PROACTIVE_CMD_IND:
	{
		gStkProactiveInd.respHandle = indHandle;
		gStkProactiveInd.bWaiting = 1;
		processStkProactiveCmdInd(pParam);
		break;
	}

	/*1.  new add CI_SIM_PRIM_SIMAT_CC_STATUS_IND*/
	case CI_SIM_PRIM_SIMAT_CC_STATUS_IND:
	{
		processStkSIMATCcStatusInd(pParam);
		break;
	}

	/*2. new add CI_SIM_PRIM_SIMAT_SEND_CALL_SETUP_RSP_IND*/
	case CI_SIM_PRIM_SIMAT_SEND_CALL_SETUP_RSP_IND:
	{
		processStkSIMATSendCallSetupRspInd(pParam);
		break;
	}

	/*3. new add CI_SIM_PRIM_SIMAT_SEND_SS_USSD_RSP_IND*/
	case CI_SIM_PRIM_SIMAT_SEND_SS_USSD_RSP_IND:
	{
		processStkSIMATSendSsUssdRspInd(pParam);
		break;
	}

	/*4. new add CI_SIM_PRIM_SIMAT_SM_CONTROL_STATUS_IND*/
	case CI_SIM_PRIM_SIMAT_SM_CONTROL_STATUS_IND:
	{
		processStkSIMATSmControlStatusInd(pParam);
		break;
	}

	/*5. new add CI_SIM_PRIM_SIMAT_SEND_SM_RSP_IND*/
	case CI_SIM_PRIM_SIMAT_SEND_SM_RSP_IND:
	{
		processStkSIMATSendSmRspInd(pParam);
		break;
	}


	case CI_SIM_PRIM_GET_CALL_SETUP_ACK_IND:
	{
#ifndef AT_CUSTOMER_HTC
		processStkCallSetupInd(pParam);
#endif
		/* For HTC don't deal with this indication.
		CI_SIM_PRIM_PROACTIVE_CMD_IND will include
		all raw data about CALL SETUP. */  
		gStkSetupCallInd.respHandle = indHandle;
		gStkSetupCallInd.bWaiting = 1;
		break;
	}

	case CI_SIM_PRIM_SIMAT_DISPLAY_INFO_IND:
	{
		processStkDisplayInfoInd(pParam);
		break;
	}

	case CI_SIM_PRIM_ENDATSESSION_IND:
	{
		processStkEndSessionInd(pParam);
		break;
	}

	case CI_SIM_PRIM_CARD_IND:
	{
		processStkCardInd(pParam);
		break;
	}

	default:
		break;
	}

	/* free up the indication memory */
	atciSvgFreeIndMem( svgId, primId, pParam );

	return;
}

void resetSimParas(void)
{
	gWaitToSetAcmMax = -1;
	memset(&gSetPuctInfoReq, 0x0, sizeof(CiCcPrimSetPuctInfoReq));
	getSimTypeInd = FALSE;
	gSimStatus = CI_SIM_ST_NOT_READY;
	gSimPinState = CI_SIM_PIN_NUM_STATES;
}
