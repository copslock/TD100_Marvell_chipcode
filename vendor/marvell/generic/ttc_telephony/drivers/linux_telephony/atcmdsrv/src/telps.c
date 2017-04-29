/******************************************************************************

*(C) Copyright 2007 Marvell International Ltd.

* All Rights Reserved

******************************************************************************/
/*--------------------------------------------------------------------------------------------------------------------
 *    INTEL CONFIDENTIAL
 *    Copyright 2006 Intel Corporation All Rights Reserved.
 *    The source code contained or described herein and all documents related to the source code (“Material? are owned
 *    by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
 *    its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
 *    Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
 *    treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
 *    transmitted, distributed, or disclosed in any way without Intel’s prior express written permission.
 *
 *    No license under any patent, copyright, trade secret or other intellectual property right is granted to or
 *    conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
 *    estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
 *    Intel in writing.
 *    -------------------------------------------------------------------------------------------------------------------
 *
 *  Filename: telps.c
 *
 *  Authors:  Rovin Yu
 *
 *  Description: Process packet switched service related AT commands
 *
 *  History:
 *   Feb 18, 2008 - Creation of file
 *
 *  Notes:
 *
 ******************************************************************************/

/******************************************************************************
*   include files
******************************************************************************/
#include "ci_api_types.h"
#include "ci_api.h"
#include "teldef.h"
#include "telutl.h"
#include "telatparamdef.h"
#include "ci_ps.h"
#include "telatci.h"
#include "ps_api.h"
#include <arpa/inet.h>

#define ATCI_PDP_ACTIVE    1
#define ATCI_PDP_INACTIVE  0

/* QoS related structure */
static ErrorRatioLookup errorRatioSDU[] =
{
	{ 0,				    0,					TEL_GPRS_SDU_ERROR_RATIO_SUBSCRIBED				       },
	{ 1,				    2,					TEL_GPRS_SDU_ERROR_RATIO_1102					       },
	{ 7,				    3,					TEL_GPRS_SDU_ERROR_RATIO_7103					       },
	{ 1,				    3,					TEL_GPRS_SDU_ERROR_RATIO_1103					       },
	{ 1,				    4,					TEL_GPRS_SDU_ERROR_RATIO_1104					       },
	{ 1,				    5,					TEL_GPRS_SDU_ERROR_RATIO_1105					       },
	{ 1,				    6,					TEL_GPRS_SDU_ERROR_RATIO_1106					       },
	{ 1,				    1,					TEL_GPRS_SDU_ERROR_RATIO_1101					       },
	{ CI_PS_3G_ENUM_LOOKUP_END_ELEMENT, CI_PS_3G_ENUM_LOOKUP_END_ELEMENT,	CI_PS_3G_ENUM_LOOKUP_END_ELEMENT				       }
};

static ErrorRatioLookup errorRatioBER[] =
{
	{ 0,				    0,					TEL_GPRS_RESIDUAL_BER_SUBSCRIBED				       },
	{ 5,				    2,					TEL_GPRS_RESIDUAL_BER_5102					       },
	{ 1,				    2,					TEL_GPRS_RESIDUAL_BER_1102					       },
	{ 5,				    3,					TEL_GPRS_RESIDUAL_BER_5103					       },
	{ 4,				    3,					TEL_GPRS_RESIDUAL_BER_4103					       },
	{ 1,				    3,					TEL_GPRS_RESIDUAL_BER_1103					       },
	{ 1,				    4,					TEL_GPRS_RESIDUAL_BER_1104					       },
	{ 1,				    5,					TEL_GPRS_RESIDUAL_BER_1105					       },
	{ 1,				    6,					TEL_GPRS_RESIDUAL_BER_1106					       },
	{ 6,				    8,					TEL_GPRS_RESIDUAL_BER_6108					       },
	{ CI_PS_3G_ENUM_LOOKUP_END_ELEMENT, CI_PS_3G_ENUM_LOOKUP_END_ELEMENT,	CI_PS_3G_ENUM_LOOKUP_END_ELEMENT				       }
};
#if 0
// Mapping value is not correct
static EnumParamLookup enumDeliveryOrderLookup[] =
{
	{ CI_PS_3G_DLV_ORDER_NO,	    TEL_GPRS_DLV_ORDER_WITHOUT			       },
	{ CI_PS_3G_DLV_ORDER_YES,	    TEL_GPRS_DLV_ORDER_WITH			       },
	{ CI_PS_3G_DLV_ORDER_SUBSCRIBED,    TEL_GPRS_DLV_ORDER_SUBSCRIBED		       },
	{ CI_PS_3G_ENUM_LOOKUP_END_ELEMENT, CI_PS_3G_ENUM_LOOKUP_END_ELEMENT		       }
};
#endif

static EnumParamLookup enumTrafficPriorityLookup[] =
{
	{ CI_PS_3G_SDU_TRAFFIC_PRIORITY_SUBSCRIBED, TEL_GPRS_TRAFFIC_PRIORITY_LEVEL_SUBSCRIBED	      },
	{ CI_PS_3G_SDU_TRAFFIC_PRIORITY_LEVEL_1,    TEL_GPRS_TRAFFIC_PRIORITY_LEVEL_1		      },
	{ CI_PS_3G_SDU_TRAFFIC_PRIORITY_LEVEL_2,    TEL_GPRS_TRAFFIC_PRIORITY_LEVEL_2		      },
	{ CI_PS_3G_SDU_TRAFFIC_PRIORITY_LEVEL_3,    TEL_GPRS_TRAFFIC_PRIORITY_LEVEL_3		      },
	{ CI_PS_3G_ENUM_LOOKUP_END_ELEMENT,	    CI_PS_3G_ENUM_LOOKUP_END_ELEMENT		      }
};

extern BOOL  searchListByCid(unsigned char cid, DIRECTIPCONFIGLIST ** ppBuf);

extern char gPdpErrCauseBuf[];

/************************************************************************************
 *
 * PS related AT commands
 *
 *************************************************************************************/

/*Added by Michal Bukai*/
/************************************************************************************
 * F@: ciCGTFT - GLOBAL API for GCF AT+CGTFT -command
 *
 */
RETURNCODE_T  ciCGTFT(            const utlAtParameterOp_T op,
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
	**  Set the result code to RESULT_CODE_INTERMEDIATE.  This allows
	**  the indications to display the correct return code after the
	**  AT Command is issued.
	*/
	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	CiReturnCode ret = CIRC_FAIL;
	BOOL cmdValid = FALSE;

	CiPsTftFilter tftFilter, *pFilter;

	CHAR strAddress[TEL_AT_CGTFT_ADDRESS_STR_MAX_LEN + ATCI_NULL_TERMINATOR_LENGTH], strDestPort[TEL_AT_CGTFT_PORT_STR_MAX_LEN + ATCI_NULL_TERMINATOR_LENGTH], strSrcPort[TEL_AT_CGTFT_PORT_STR_MAX_LEN + ATCI_NULL_TERMINATOR_LENGTH], strIpsec[TEL_AT_CGTFT_IPSEC_STR_MAX_LEN + ATCI_NULL_TERMINATOR_LENGTH], strTos[TEL_AT_CGTFT_TOS_STR_MAX_LEN + ATCI_NULL_TERMINATOR_LENGTH], strFlowLable[TEL_AT_CGTFT_FLOWLABLE_STR_MAX_LEN + ATCI_NULL_TERMINATOR_LENGTH];
	int cid = 0, pfid = 0, epindex = 0, protocolNum = 0,   iNumOfDots = 0, index = 0, minVal = 0, maxVal = 0;
	INT16 addressLen = 0, destPortLen = 0, srcPortLen = 0, ipsecLen = 0, tosLen = 0, flowLableLen = 0;
	BOOL bFoundDot = FALSE, bContinue = TRUE;

	/*
	 *  Put parser index into the variable
	 */
	UINT32 atHandle;
	TelAtParserID sAtpIndex = *(TelAtParserID *)arg_p;
	atHandle = MAKE_AT_HANDLE( sAtpIndex );
	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	/*
	**  Check the format of the request.
	*/
	switch (op)
	{
	case TEL_EXT_GET_CMD:    /* AT+CGTFT?  */
	{
		ret = PS_GetTftList(atHandle);
		break;
	}

	case TEL_EXT_TEST_CMD:    /* AT+CGTFT=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+CGTFT: <cid>, <packet filter identifier>(1-8), <evaluation precedence index>(0-255), <source address and subnet mask>, <protocol number (ipv4) / next header (ipv6)>, <destination port range>, <source port range>, <ipsec security parameter index (spi)>(00000000-FFFFFFFF), <type of service (tos) (ipv4) and mask / traffic class (ipv6) and mask>, <flow label (ipv6)>(00000-FFFFF)\r\n");
		break;
	}

	case TEL_EXT_SET_CMD:      /* AT+CGDCONT= */
	{
		if ( getExtValue( parameter_values_p, 0, &cid, TEL_AT_CGTFT_CID_VAL_MIN, TEL_AT_CGTFT_CID_VAL_MAX, TEL_AT_CGTFT_CID_VAL_DEFAULT ) == TRUE )
		{
			if ( getExtValue( parameter_values_p, 1, &pfid, TEL_AT_CGTFT_PFID_VAL_MIN, TEL_AT_CGTFT_PFID_VAL_MAX, TEL_AT_CGTFT_PFID_VAL_DEFAULT ) == TRUE )
			{
				if ( getExtValue( parameter_values_p, 2, &epindex, TEL_AT_CGTFT_EPINDEX_VAL_MIN, TEL_AT_CGTFT_EPINDEX_VAL_MAX, TEL_AT_CGTFT_EPINDEX_VAL_DEFAULT ) == TRUE )
				{
					if ( getExtString( parameter_values_p, 3, strAddress, TEL_AT_CGTFT_ADDRESS_STR_MAX_LEN, &addressLen, (CHAR *)TEL_AT_CGTFT_ADDRESS_STR_DEFAULT) == TRUE )
					{
						if ( getExtValue( parameter_values_p, 4, &protocolNum, TEL_AT_CGTFT_PROTOCOLNUM_VAL_MIN, TEL_AT_CGTFT_PROTOCOLNUM_VAL_MAX, TEL_AT_CGTFT_PROTOCOLNUM_VAL_DEFAULT ) == TRUE )
						{
							if ( getExtString( parameter_values_p, 5, strDestPort, TEL_AT_CGTFT_PORT_STR_MAX_LEN, &destPortLen, (CHAR *)TEL_AT_CGTFT_PORT_STR_DEFAULT) == TRUE )
							{
								if ( getExtString( parameter_values_p, 6, strSrcPort, TEL_AT_CGTFT_PORT_STR_MAX_LEN, &srcPortLen, (CHAR *)TEL_AT_CGTFT_PORT_STR_DEFAULT) == TRUE )
								{
									if ( getExtString( parameter_values_p, 7, strIpsec, TEL_AT_CGTFT_IPSEC_STR_MAX_LEN, &ipsecLen, (CHAR *)TEL_AT_CGTFT_IPSEC_STR_DEFAULT) == TRUE )
									{
										if ( getExtString( parameter_values_p, 8, strTos, TEL_AT_CGTFT_TOS_STR_MAX_LEN, &tosLen, (CHAR *)TEL_AT_CGTFT_TOS_STR_DEFAULT) == TRUE )
										{
											if ( getExtString( parameter_values_p, 9, strFlowLable, TEL_AT_CGTFT_FLOWLABLE_STR_MAX_LEN, &flowLableLen, (CHAR *)TEL_AT_CGTFT_FLOWLABLE_STR_DEFAULT) == TRUE )
											{
												if (parameter_values_p[1].is_default && parameter_values_p[2].is_default && parameter_values_p[3].is_default && parameter_values_p[4].is_default    && parameter_values_p[5].is_default && parameter_values_p[6].is_default && parameter_values_p[7].is_default && parameter_values_p[8].is_default && parameter_values_p[9].is_default)
												{ //CGTFT = <cid> -> delete reqest
													if (( cid > 0 ) && ( cid <= CI_PS_MAX_PDP_CTX_NUM ))
													{
														/* CI - SAC use cid range starting from 0 */
														cid--;
														ret = PS_DeleteTft(atHandle, cid);
														cmdValid = TRUE;
													}
												}
												else    //set reqest process user parameter to ci reqest
												{
													if (( cid > 0 ) && ( cid <= CI_PS_MAX_PDP_CTX_NUM ) && (parameter_values_p[1].is_default == FALSE))
													{
														pFilter = &tftFilter;

														/* CI - SAC use cid range starting from 0 */
														cid--;

														pFilter->pfId = (UINT8)pfid;            /*Packet Filter ID*/
														pFilter->epIndex = (UINT8)epindex;      /*Evaluation precedence index*/

														/*source address and sub netmask*/
														/********************************/
														if (parameter_values_p[3].is_default == FALSE)
														{
															CHAR * strAddressMask = &strAddress[index];
															for (index = 0; index < addressLen; index++)
															{
																if ( strAddress[index] == '.')
																{
																	iNumOfDots++;
																	if (iNumOfDots == 4)
																		strAddressMask = &strAddress[index];
																	else if (iNumOfDots == 16)
																		strAddressMask = &strAddress[index];
																}
															}

															switch (iNumOfDots)
															{
															case 3: //IPv4 Src Addres only
															case 15: //IPv6 Src Addres only
															{
																pFilter->srcAddr.len = ((UINT8)(addressLen) < CI_PS_PDP_IP_V6_SIZE ? (UINT8)(addressLen) : CI_PS_PDP_IP_V6_SIZE);
																memcpy(pFilter->srcAddr.valData, strAddress, pFilter->srcAddr.len);
																break;
															}
															case 7: //IPv4 Src Addres and Sub net mask
															case 31: //IPv4 Src Addres and Sub net mask
															{
																pFilter->srcAddr.len = ((UINT8)(strAddressMask - strAddress) < CI_PS_PDP_IP_V6_SIZE ? (UINT8)(strAddressMask - strAddress) : CI_PS_PDP_IP_V6_SIZE);
																memcpy(pFilter->srcAddr.valData, strAddress, pFilter->srcAddr.len);
																pFilter->srcAddrMask.len = ((UINT8)(addressLen - pFilter->srcAddr.len - 1) < CI_PS_PDP_IP_V6_SIZE ? (UINT8)(addressLen - pFilter->srcAddr.len - 1) : CI_PS_PDP_IP_V6_SIZE);
																memcpy(pFilter->srcAddrMask.valData, strAddressMask + 1, pFilter->srcAddrMask.len);
																break;
															}
															default:
																bContinue = FALSE;
																break;
															}
														}

														/*protocol number (ipv4) / next header (ipv6)*/
														if (parameter_values_p[4].is_default == FALSE)
														{
															pFilter->pIdNextHdr = (UINT8)protocolNum;
															pFilter->pIdNextHdrPresent = TRUE;
														}
														else
															pFilter->pIdNextHdrPresent = FALSE;

														/*destination port range*/
														/************************/
														if (bContinue)
														{
															if ( parameter_values_p[5].is_default == FALSE)
															{
																bFoundDot = FALSE;
																for (index = 0; index < destPortLen && bFoundDot == FALSE; index++)
																{
																	if ( strDestPort[index] == '.')
																	{
																		if (index + 1 < destPortLen)
																		{
																			minVal = atoi((char*)strDestPort);
																			maxVal = atoi((char*)&strDestPort[index + 1]);
																			bFoundDot = TRUE;
																		}
																	}
																}
																if (bFoundDot)
																{
#define MAX_PORT_RANGE 65535
																	if (minVal >= 0 && minVal <= MAX_PORT_RANGE && maxVal >= 0 && maxVal <= MAX_PORT_RANGE )
																	{
																		pFilter->dstPortRange.min = (UINT32)minVal;
																		pFilter->dstPortRange.max = (UINT32)maxVal;
																		pFilter->dstPortRangePresent = TRUE;
																	}
																	else
																		bContinue = FALSE;
																}
																else
																	bContinue = FALSE;
															}
															else
																pFilter->dstPortRangePresent = FALSE;
														}

														/*source port range*/
														/*******************/
														if (bContinue)
														{
															if ( parameter_values_p[6].is_default == FALSE)
															{
																bFoundDot = FALSE;
																for (index = 0; index < srcPortLen && bFoundDot == FALSE; index++)
																{
																	if ( strSrcPort[index] == '.')
																	{
																		if (index + 1 < srcPortLen)
																		{
																			minVal = atoi((char*)strSrcPort);
																			maxVal = atoi((char*)&strSrcPort[index + 1]);
																			bFoundDot = TRUE;
																		}
																	}
																}
																if (bFoundDot)
																{
																	if (minVal >= 0 && minVal <= MAX_PORT_RANGE && maxVal >= 0 && maxVal <= MAX_PORT_RANGE )
																	{
																		pFilter->srcPortRange.min = (UINT32)minVal;
																		pFilter->srcPortRange.max = (UINT32)maxVal;
																		pFilter->srcPortRangePresent = TRUE;
																	}
																	else
																		bContinue = FALSE;
																}
																else
																	bContinue = FALSE;
															}
															else
																pFilter->srcPortRangePresent = FALSE;
														}

														/*ipsec security parameter index (spi)*/
														/**************************************/
														if (bContinue)
														{
															if ( parameter_values_p[7].is_default == FALSE)
															{
																if (HexToBin((char *)strIpsec, (int*)&pFilter->ipSecSPI, ipsecLen))
																{
																	pFilter->ipSecSPIPresent = TRUE;
																}
																else
																	bContinue = FALSE;
															}
															else
																pFilter->ipSecSPIPresent = FALSE;
														}

														/*type of service (tos) and mask (ipv4) / traffic class (ipv4) and mask*/
														/***********************************************************************/
														if (bContinue)
														{
															if ( parameter_values_p[8].is_default == FALSE)
															{
																bFoundDot = FALSE;
																for (index = 0; index < tosLen && bFoundDot == FALSE; index++)
																{
																	if ( strTos[index] == '.')
																	{
																		if (index + 1 < tosLen)
																		{
																			minVal = atoi((char*)strTos);
																			maxVal = atoi((char*)&strTos[index + 1]);
																			bFoundDot = TRUE;
																		}
																	}
																}
																if (bFoundDot)
																{
																	if (minVal >= 0 && minVal < 255 && maxVal >= 0 && maxVal < 255 )
																	{
																		pFilter->tosTc = (UINT8)minVal;
																		pFilter->tosTcMask = (UINT8)maxVal;
																		pFilter->tosPresent = TRUE;
																	}
																	else
																		bContinue = FALSE;
																}
																else
																	bContinue = FALSE;
															}
															else
																pFilter->tosPresent = FALSE;
														}

														/*flow label (ipv6)*/
														/*******************/
														if (bContinue)
														{
															if ( parameter_values_p[9].is_default == FALSE)
															{
																if (HexToBin((char *)strFlowLable, (int*)&pFilter->flowLabel, flowLableLen))
																{
																	pFilter->flowLabelPresent = TRUE;
																}
																else
																	bContinue = FALSE;
															}
															else
																pFilter->flowLabelPresent = FALSE;
														}

														/* Send CI reqest*/

														if (bContinue)
														{
															ret = PS_SetTftFilter(atHandle, cid, pFilter);

															/* mark command as valid */
															cmdValid = TRUE;
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
		if ( cmdValid == FALSE )
		{
			ret = ATRESP(atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, 0);
		}
		break;
	}

	case TEL_EXT_ACTION_CMD:   /* AT+CGTFT */
	default:
		ret = ATRESP(atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, 0);
		break;
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}

/*********************** Added By Michal Bukai **************************************
 ************************************************************************************
 * F@: ciEnterData - GLOBAL API for GCF AT+CGCMOD
 *
 */
RETURNCODE_T  ciModifyContext(            const utlAtParameterOp_T op,
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
	CiReturnCode ret = CIRC_FAIL;
	INT32 cid;
	BOOL doAll;

	/*
	 *  Put parser index into the variable
	 */
	UINT32 atHandle;
	TelAtParserID sAtpIndex = *(TelAtParserID *)arg_p;
	atHandle = MAKE_AT_HANDLE( sAtpIndex );
	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	/*
	**  Check the format of the request.
	*/
	switch (op)
	{
	case TEL_EXT_TEST_CMD:    /* AT+CGCMOD=? */
	{
		/* Send the CI Request */
		ret = PS_GetGPRSActiveCidList(atHandle);

		break;
	}

	case TEL_EXT_SET_CMD:      /* AT+CGCMOD= */
	{
		//Get context identifier - multiple cids is not supported
		if ( getExtValue(parameter_values_p, 0, &cid, TEL_AT_CGCMOD_CID_VAL_MIN, TEL_AT_CGCMOD_CID_VAL_MAX, TEL_AT_CGCMOD_CID_VAL_DEFAULT) == TRUE )
		{
			if (( cid > 0 ) && ( cid <= CI_PS_MAX_PDP_CTX_NUM ))
			{
				if (parameter_values_p[0].is_default) /* Modify all active contexts */
					doAll = TRUE;
				else
				{
					doAll = FALSE;
					/* CI - SAC use cid range starting from 0 */ //Michl Bukai - NEED TOCHEK!!!!! al gprs cmand use this logic
					cid--;
				}

				//PDP ctc status is not being tested (define, active, data mode)  - let SAC check it
				/* Send the CI Request */
				ret = PS_ModifyGPRSContext(atHandle, cid, doAll);

				/* mark command is valid */

			}
		}
		break;
	}

	case TEL_EXT_GET_CMD:           /* AT+CGCMOD?  */
	case TEL_EXT_ACTION_CMD:        /* AT+CGCMOD */
	default:
		ret = ATRESP(atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, 0);
		break;
	}


	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);
}
/************************************************************************************
 * F@: ciReturnIp - GLOBAL API for GCF AT+GETIP -command
 *
 */
utlReturnCode_T  ciReturnIp( const utlAtParameterOp_T op,
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
	INT32 index;
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);
	*xid_p = atHandle;

	switch (op)
	{
	case TEL_EXT_SET_CMD: /*AT+GETIP=cid*/
		if ( getExtValue( parameter_values_p, 0, &index, TEL_AT_IP_INDEX_VAL_MIN, TEL_AT_IP_INDEX_VAL_MAX, TEL_AT_IP_INDEX_VAL_DEFAULT ) == TRUE )
		{
			index--;
			ret = PS_GetGPRSContextIP(atHandle, index);
#if 0
			if (searchListByCid(index, &pNode) == TRUE)
			{
				struct in_addr in;
				sprintf(rspBuf, "+GETIP: ");
				in.s_addr = htonl(pNode->directIpAddress.ipv4.inIPAddress);
				strcat(rspBuf, inet_ntoa(in));
				strcat(rspBuf, ", ");
				in.s_addr = htonl(pNode->directIpAddress.ipv4.inDefaultGateway);
				strcat(rspBuf, inet_ntoa(in));
				strcat(rspBuf, ", ");
				in.s_addr = htonl(pNode->directIpAddress.ipv4.inSubnetMask);
				strcat(rspBuf, inet_ntoa(in));
				strcat(rspBuf, "\r\n");
				ATRESP(atHandle, ATCI_RESULT_CODE_OK, 0, rspBuf);
			}
			else
				ATRESP(atHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
#endif
		}
		break;
	default:
		//ATRESP(atHandle, ATCI_RESULT_CODE_ERROR, 0, NULL);
		break;
	}
	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);
}

/************************************************************************************
 * F@: ciPSRegStatus - GLOBAL API for GCF AT+CGREG -command
 *
 */
RETURNCODE_T  ciPSRegStatus(            const utlAtParameterOp_T op,
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
	INT32 cgreg;
	UINT32 atHandle = MAKE_AT_HANDLE( *(TelAtParserID *)arg_p );

	*xid_p = atHandle;

	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	switch (op)
	{
	case TEL_EXT_TEST_CMD:    /* AT+CGREG=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+CGREG: (0-2)\r\n");
		break;
	}
	case TEL_EXT_GET_CMD:    /* AT+CGREG?  */
	{
		ret = PS_GetGPRSRegStatus(atHandle);
		break;
	}
	case TEL_EXT_SET_CMD:   /* AT+CGREG=  */
	{
		/*
		**  Parse the state variable.
		*/
		if ( getExtValue(parameter_values_p, 0, &cgreg, TEL_AT_CGREG_MODE_0_N_VAL_MIN, TEL_AT_CGREG_MODE_0_N_VAL_MAX, TEL_AT_CGREG_MODE_0_N_VAL_DEFAULT) == TRUE )
		{
			ret = PS_SetGPRSRegStatus(atHandle, (UINT32)cgreg);
		}

		break;
	}
	case TEL_EXT_ACTION_CMD:   /* AT+CGREG   */
	default:
	{
		break;
	}
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}



/************************************************************************************
 * F@: ciPSAttach - GLOBAL API for GCF AT+CGATT -command
 *
 */
RETURNCODE_T  ciPSAttach(            const utlAtParameterOp_T op,
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
	INT32 cgatt;
	UINT32 atHandle = MAKE_AT_HANDLE( *(TelAtParserID *)arg_p );

	*xid_p = atHandle;

	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	switch (op)
	{
	case TEL_EXT_TEST_CMD:    /* AT+CGATT=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+CGATT: (0-1)\r\n");
		break;
	}
	case TEL_EXT_GET_CMD:    /* AT+CGATT?  */
	{
		ret = PS_GetGPRSAttached(atHandle);
		break;
	}
	case TEL_EXT_SET_CMD:   /* AT+CGATT=  */
	{
		/*
		**  Parse the state variable.
		*/
		if ( getExtValue(parameter_values_p, 0, &cgatt, TEL_AT_CGATT_0_STATE_VAL_MIN, TEL_AT_CGATT_0_STATE_VAL_MAX, TEL_AT_CGATT_0_STATE_VAL_DEFAULT) == TRUE )
		{
			/*
			**  Check if this is an attach or detach request.
			*/
			switch (cgatt)
			{
			case ATCI_GPRS_ATTACH_REQ:       /* perform GPRS attach */
			{
				ret = PS_SetGPRSAttached(atHandle, TRUE);
				break;
			}

			case ATCI_GPRS_DETACH_REQ:       /* perform GPRS detach and flag all the CIDs as inactive */
			{
				ret = PS_SetGPRSAttached(atHandle, FALSE);
				break;
			}

			default:
				break;
			}       /* switch */
		}

		break;
	}
	case TEL_EXT_ACTION_CMD:   /* AT+CGATT   */
	default:
	{
		break;
	}
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}


/************************************************************************************
 * F@: ciPDPActivate - GLOBAL API for GCF AT+CGACT -command
 *
 */

RETURNCODE_T  ciPDPActivate(            const utlAtParameterOp_T op,
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
	CiReturnCode ret = CIRC_FAIL;

	INT32 pdpState;
	INT32 cid;

	UINT32 atHandle = MAKE_AT_HANDLE( *(TelAtParserID *)arg_p );

	*xid_p = atHandle;

	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	switch (op)
	{
	case TEL_EXT_GET_CMD:    /* AT+CGACT?  */
	{
		ret = PS_GetGPRSContextActivatedList(atHandle);
		break;
	}

	case TEL_EXT_TEST_CMD:    /* AT+CGACT=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+CGACT: (0,1)");
		break;
	}

	case TEL_EXT_SET_CMD:      /* AT+CGACT= */
	{
		/*
		**  Parse the state.
		*/
		if ( getExtValue( parameter_values_p, 0, &pdpState, TEL_AT_CGACT_0_STATE_VAL_MIN, TEL_AT_CGACT_0_STATE_VAL_MAX, TEL_AT_CGACT_0_STATE_VAL_DEFAULT ) == TRUE )
		{

			/*
			**  modify the timeout for the AT command processing
			*/
			{
				utlRelativeTime_T period;
				if (pdpState == 1)
					period.seconds     = 155;
				else if (pdpState == 0)
					period.seconds     = 55;
				else
					break;
				period.nanoseconds = 0;
				if ( utlModifyPendingAsyncResponse(&period, *xid_p) != utlSUCCESS)
					break;
			}
			if (parameter_values_p[1].is_default)
			{
				/* All the pdpState is included: it should deactive all the activated PDP context */
				ret = PS_SetGPRSContextActivated(atHandle, 0, ATCI_PDP_INACTIVE, TRUE);
			}
			else
			{
				/* get the <cid> parameter */
				if ( getExtValue(parameter_values_p, 1, &cid, TEL_AT_CGACT_1_CID_VAL_MIN, TEL_AT_CGACT_1_CID_VAL_MAX, TEL_AT_CGACT_1_CID_VAL_DEFAULT) == TRUE )
				{
					if (( cid > 0 ) && ( cid <= CI_PS_MAX_PDP_CTX_NUM ))
					{
						/* CI - SAC use cid range starting from 0 */
						cid--;
						ret = PS_SetGPRSContextActivated(atHandle, cid, (pdpState == ATCI_PDP_ACTIVE), FALSE);
					}       /* if (( cid > 0 ) && ( cid <= CI_PS_MAX_PDP_CTX_NUM )) */
				}               /* if( getExtValue (parameter_values_p, X, &cid, */
			}                       /* if  pdpState */
		}                               /* if ( getExtValue( parameter_values_p, X, &state, */

		break;
	}

	case TEL_EXT_ACTION_CMD:   /* AT+CGACT */
	default:
		break;
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);

	return(rc);
}

/************************************************************************************
 * F@: ciEnterData - GLOBAL API for GCF AT+CGDATA
 *
 */
RETURNCODE_T  ciEnterData(            const utlAtParameterOp_T op,
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
	CiReturnCode ret = CIRC_FAIL;
	CHAR l2pType[TEL_AT_CGDATA_0_L2P_STR_MAX_LEN + ATCI_NULL_TERMINATOR_LENGTH] = { 0 };
	INT16 l2pTypeLength = 0;
	INT32 cid;
	CiPsL2P l2p = CI_PS_NUM_L2PS;

	UINT32 atHandle = MAKE_AT_HANDLE( *(TelAtParserID *)arg_p );
	*xid_p = atHandle;

	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	switch (op)
	{
	case TEL_EXT_TEST_CMD:    /* AT+CGDATA=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"+CGDATA: \"PPP\"");
		break;
	}

	case TEL_EXT_SET_CMD:      /* AT+CGDATA= */
	{

		if ( getExtString( parameter_values_p, 0, l2pType, TEL_AT_CGDATA_0_L2P_STR_MAX_LEN, &l2pTypeLength, (CHAR *)TEL_AT_CGDATA_0_L2P_STR_DEFAULT ) == TRUE )
		{
			if ( l2pTypeLength == 0 )
				l2p = CI_PS_L2P_NONE;
			else if (( strcmp("NULL", (char *)l2pType ) == 0) || ( strcmp("null", (char *)l2pType) == 0 ))
				l2p = CI_PS_L2P_NONE;
			else if (( strcmp("PPP", (char *)l2pType ) == 0) || ( strcmp("ppp", (char *)l2pType) == 0 ))
				l2p = CI_PS_L2P_PPP;

			if ( l2p < CI_PS_NUM_L2PS )
			{
				/*
				**  Get context identifier - multiple cids is not supported
				*/
				if ( getExtValue(parameter_values_p, 1, &cid, TEL_AT_CGDATA_1_CID_VAL_MIN, TEL_AT_CGDATA_1_CID_VAL_MAX, TEL_AT_CGDATA_1_CID_VAL_DEFAULT) == TRUE )
				{
					if (( cid > 0 ) && ( cid <= CI_PS_MAX_PDP_CTX_NUM ))
					{
						/* CI - SAC use cid range starting from 0 */
						cid--;

						ret = PS_EnterGPRSDataMode(atHandle, cid, l2p);

					}
				}
			}
		}

		break;
	}

	case TEL_EXT_GET_CMD:           /* AT+CGDATA?  */
	case TEL_EXT_ACTION_CMD:        /* AT+CGDATA */
	default:
		break;
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}


/************************************************************************************
 * F@: ciSetContext - GLOBAL API for GCF AT+CGDCONT -command
 *
 */
RETURNCODE_T  ciSetContext(            const utlAtParameterOp_T op,
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
	CiReturnCode ret = CIRC_FAIL;
	INT32 primCid;
	CHAR tmpStr[TEL_AT_CGDCONT_1_PDPTYPE_STR_MAX_LEN] = "";
	INT16 tmpStrLen = 0;
	INT32 tmpParam = 0;
	CiPsPdpCtx pdpCtxInfo;
	UINT32 atHandle = MAKE_AT_HANDLE( *(TelAtParserID *)arg_p );

	*xid_p = atHandle;

	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	switch (op)
	{
	case TEL_EXT_GET_CMD:    /* AT+CGDCONT?  */
	{
		ret = PS_GetGPRSContextList(atHandle);
		break;
	}

	case TEL_EXT_TEST_CMD:    /* AT+CGDCONT=? */
	{
		char tmpAtRspBuf[100];
		/* for every PDP type */
		sprintf( tmpAtRspBuf, "+CGDCONT: (1-%d),\"IP\",,,(0-1),(0-1)", CI_PS_MAX_PDP_CTX_NUM );
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)tmpAtRspBuf);
		break;
	}

	case TEL_EXT_SET_CMD:      /* AT+CGDCONT= */
	{
		memset(&pdpCtxInfo, 0, sizeof(pdpCtxInfo));

		/* 1  Extract the arguments starting with the CID. */
		if ( getExtValue( parameter_values_p, 0, &primCid, TEL_AT_CGDCONT_0_CID_VAL_MIN, TEL_AT_CGDCONT_0_CID_VAL_MAX, TEL_AT_CGDCONT_0_CID_VAL_DEFAULT ) != TRUE )
			break;

		if (( primCid > 0 ) && ( primCid <= CI_PS_MAX_PDP_CTX_NUM ))
		{
			/* CI - SAC use cid range starting from 0 */
			primCid--;
			pdpCtxInfo.cid = primCid;
		}
		else
			break;

		/* If only the CID is included, we should undefine this PDP Context */
		if ( parameter_values_p[1].is_default && parameter_values_p[2].is_default && parameter_values_p[3].is_default
		     && parameter_values_p[4].is_default && parameter_values_p[5].is_default )
		{
			ret = PS_DeleteGPRSContext(atHandle, primCid);
			break;
		}

		/* 2.  Determine the PDP type.     */
		if ( getExtString( parameter_values_p, 1, tmpStr, TEL_AT_CGDCONT_1_PDPTYPE_STR_MAX_LEN - ATCI_NULL_TERMINATOR_LENGTH, &tmpStrLen, (CHAR *)TEL_AT_CGDCONT_1_PDPTYPE_STR_DEFAULT) == TRUE )
		{
			if ( tmpStrLen == 0 )
			{
				/* default value */
				pdpCtxInfo.type = CI_PS_PDP_TYPE_IP;
			}
			else
			{
				if ( (strcmp("IP", (char *)tmpStr) == 0) || (strcmp("ip", (char *)tmpStr) == 0) )
					pdpCtxInfo.type = CI_PS_PDP_TYPE_IP;
				else if ( (strcmp("IPV6", (char *)tmpStr) == 0) || (strcmp("ipv6", (char *)tmpStr) == 0) )
					pdpCtxInfo.type = CI_PS_PDP_TYPE_IPV6;
				else if ( (strcmp("PPP", (char *)tmpStr) == 0) || (strcmp("ppp", (char *)tmpStr) == 0) )
					pdpCtxInfo.type = CI_PS_PDP_TYPE_PPP;
				else {            /* default to IP */
					//pdpCtxInfo.type = CI_PS_PDP_TYPE_IP;
					ret = ATRESP(atHandle, ATCI_RESULT_CODE_ERROR, 0, 0);
					break;
				}
			}
		}

		/*     ** 3 Assign the APN string.                 */
		if ( getExtString(parameter_values_p, 2, tmpStr, TEL_AT_CGDCONT_2_APN_STR_MAX_LEN, &tmpStrLen, TEL_AT_CGDCONT_2_APN_STR_DEFAULT) == TRUE )
		{
			if ( tmpStrLen != 0 )
			{
				pdpCtxInfo.apnPresent = TRUE;
				pdpCtxInfo.apn.len = tmpStrLen;
				memcpy( pdpCtxInfo.apn.valStr, tmpStr, tmpStrLen );
			}
		}

		/* 4.  Extract the PDP Address.                 */
		if ( getExtString( parameter_values_p, 3, tmpStr, TEL_AT_CGDCONT_3_PDPADDRESS_STR_MAX_LEN, &tmpStrLen, TEL_AT_CGDCONT_3_PDPADDRESS_STR_DEFAULT) == TRUE )
		{
			DBGMSG("[%s] Line(%d): got getExtString returned TRUE for addrPtr. tmpStrLen(%d) \n", __FUNCTION__, __LINE__, tmpStrLen);

			if ( tmpStrLen != 0 )
			{
				pdpCtxInfo.addrPresent = TRUE;
				pdpCtxInfo.addr.len = tmpStrLen;
				memcpy(pdpCtxInfo.addr.valData, tmpStr, tmpStrLen);

				DBGMSG("[%s] Line(%d): pdpCtxInfo.addr.valData(%.*s) \n", __FUNCTION__, __LINE__, tmpStrLen, pdpCtxInfo.addr.valData);
			}
		}
		/* 5.   Now get the D_COMP.             */
		if ( getExtValue(parameter_values_p, 4, &tmpParam, TEL_AT_CGDCONT_4_DCOMP_VAL_MIN, TEL_AT_CGDCONT_4_DCOMP_VAL_MAX, TEL_AT_CGDCONT_4_DCOMP_VAL_DEFAULT) == TRUE )
		{
			pdpCtxInfo.dcompPresent = TRUE;
			switch ( tmpParam )
			{
				case ATCI_PS_DCOMP_ON:
				{
					pdpCtxInfo.dcomp = CI_PS_DCOMP_ON;
					break;
				}
				case ATCI_PS_DCOMP_V42bis: // ONLY SUPPORT V42bis.
				{
					pdpCtxInfo.dcomp = CI_PS_DCOMP_V42bis;
					break;
				}
				case ATCI_PS_DCOMP_OFF:
				{
					pdpCtxInfo.dcomp = CI_PS_DCOMP_OFF;
					break;
				}
				default:
				{
					ret = ATRESP(atHandle, ATCI_RESULT_CODE_ERROR, 0, 0);
					goto invalid;
				}
			}
		}

		/*     6. Now get the H_COMP.     */
		if ( getExtValue(parameter_values_p, 5, &tmpParam, TEL_AT_CGDCONT_5_HCOMP_VAL_MIN, TEL_AT_CGDCONT_5_HCOMP_VAL_MAX, TEL_AT_CGDCONT_5_HCOMP_VAL_DEFAULT) == TRUE )
		{
			pdpCtxInfo.hcompPresent = TRUE;
			switch ( tmpParam )
			{
				case ATCI_PS_HCOMP_TCPIP:
				{
					pdpCtxInfo.hcomp = CI_PS_HCOMP_TCPIP;
					break;
				}
				case ATCI_PS_HCOMP_IP:
				{
					pdpCtxInfo.hcomp = CI_PS_HCOMP_IP;
					break;
				}
				case ATCI_PS_HCOMP_OFF:
				{
					pdpCtxInfo.hcomp = CI_PS_HCOMP_OFF;
					break;
				}
				default:
				{
					ret = ATRESP(atHandle, ATCI_RESULT_CODE_ERROR, 0, 0);
					goto invalid;
				}
			}
		}

		ret = PS_SetGPRSContext(atHandle, &pdpCtxInfo);

		break;
	}

	case TEL_EXT_ACTION_CMD:   /* AT+CGDCONT */
	default:
		break;
	}

invalid:
	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}

/************************************************************************************
 * F@: getAtQosParams - extracts the Qos attributes from AT command line string
 *
 */
BOOL getAtQosParams ( const utlAtParameterValue_P2c parameter_values_p, int index, CiPsQosProfile *qosProfile )
{
	BOOL result = TRUE;
	INT32 precedence;               /* Precedence class */
	INT32 delay;                    /* Delay class */
	INT32 reliability;              /* Reliability class */
	INT32 peak;                     /* Peak throughput */
	INT32 mean;                     /* Mean throughput */

	/* get the PRECEDENCE */
	if (getExtValue(parameter_values_p, index++, &precedence, TEL_AT_CGQMIN_1_PRECEDENCE_VAL_MIN, TEL_AT_CGQMIN_1_PRECEDENCE_VAL_MAX, TEL_AT_CGQMIN_1_PRECEDENCE_VAL_DEFAULT) == TRUE)
	{
		if ((precedence == ATCI_DEFAULT_PARAM_VALUE) || (precedence == 0))
		{
			/* no parameter, or zero, supplied, set up the default qos value */
			qosProfile->precedence = TEL_GPRS_PRECED_CLASS_SUBSCRIBED;
		}
		else
		{
			if (precedence <= TEL_GPRS_PRECED_CLASS_3)
			{
				qosProfile->precedence = (UINT8)precedence;
			}
			else
			{
				result = FALSE;
			}
		}
	}
	else
	{
		result = FALSE;
	}

	/* get the DELAY */
	if ( result == TRUE )
	{
		if (getExtValue(parameter_values_p, index++, &delay, TEL_AT_CGQMIN_2_DELAY_VAL_MIN, TEL_AT_CGQMIN_2_DELAY_VAL_MAX, TEL_AT_CGQMIN_2_DELAY_VAL_DEFAULT) == TRUE)
		{
			if ((delay == ATCI_DEFAULT_PARAM_VALUE) || (delay == 0))
			{
				/* no parameter, or zero, supplied, set up the default qos value */
				qosProfile->delay = TEL_GPRS_DELAY_CLASS_SUBSCRIBED;
			}
			else
			{
				if ( delay <= TEL_GPRS_DELAY_CLASS_4 )
				{
					qosProfile->delay = (UINT8)delay;
				}
				else
				{
					result = FALSE;
				}
			}
		}
		else
		{
			result = FALSE;
		}
	}

	/* get the RELIABILITY */
	if ( result == TRUE )
	{
		if (getExtValue(parameter_values_p, index++, &reliability, TEL_AT_CGQMIN_3_RELIABILITY_VAL_MIN, TEL_AT_CGQMIN_3_RELIABILITY_VAL_MAX, TEL_AT_CGQMIN_3_RELIABILITY_VAL_DEFAULT) == TRUE)
		{
			if ((reliability == ATCI_DEFAULT_PARAM_VALUE) || (reliability == 0))
			{
				/* no parameter, or zero, supplied, set up the default qos value */
				qosProfile->reliability = TEL_GPRS_RELIAB_CLASS_SUBSCRIBED;
			}
			else
			{
				if (reliability <= TEL_GPRS_RELIAB_CLASS_5)
				{
					qosProfile->reliability = (UINT8)reliability;
				}
				else
				{
					result = FALSE;
				}
			}
		}
		else /* error reading supplied parameter */
		{
			result = FALSE;
		}
	}

	/* get the PEAK */
	if ( result == TRUE )
	{
		if ( getExtValue(parameter_values_p, index++, &peak, TEL_AT_CGQMIN_4_PEAK_VAL_MIN, TEL_AT_CGQMIN_4_PEAK_VAL_MAX, TEL_AT_CGQMIN_4_PEAK_VAL_DEFAULT) == TRUE )
		{
			if ((peak == ATCI_DEFAULT_PARAM_VALUE) || (peak == 0))
			{
				/* no parameter, or zero, supplied */
				/* set up the default qos value */
				qosProfile->peak = TEL_GPRS_PEAK_THRPT_SUBSCRIBED;
			}
			else
			{
				if (peak <= TEL_GPRS_PEAK_THRPT_UPTO_256KOCT)
				{
					qosProfile->peak = (UINT8)peak;
				}
				else
				{
					result = FALSE;
				}
			}
		}
		else /* error reading supplied parameter */
		{
			result = FALSE;
		}
	}

	/* get the MEAN */
	if ( result == TRUE )
	{
		if (getExtValue(parameter_values_p, index++, &mean, TEL_AT_CGQMIN_5_MEAN_VAL_MIN, TEL_AT_CGQMIN_5_MEAN_VAL_MAX, TEL_AT_CGQMIN_5_MEAN_VAL_DEFAULT) == TRUE)
		{
			if ((mean == ATCI_DEFAULT_PARAM_VALUE) || (mean == 0))
			{
				/* no parameter, or zero, supplied, set up the default qos value */
				qosProfile->mean = TEL_GPRS_MEAN_THRPT_SUBSCRIBED;
			}
			else
			{
				if ((mean <= TEL_GPRS_MEAN_THRPT_50M_OPH) || (mean == TEL_GPRS_MEAN_THRPT_BEST_EFFORT))
				{
					qosProfile->mean = (UINT8)mean;
				}
				else
				{
					result = FALSE;
				}
			}
		}
		else /* error reading supplied parameter */
		{
			result = FALSE;
		}
	}

	return (result);
}

/************************************************************************************
 * F@: ciSetMinQOS - GLOBAL API for GCF AT+CGQMIN -command
 *
 */
RETURNCODE_T  ciSetMinQOS(            const utlAtParameterOp_T op,
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
	CiReturnCode ret = CIRC_FAIL;
	INT32 cid;
	CiPsQosProfile minQosProfile;
	UINT32 atHandle = MAKE_AT_HANDLE( *(TelAtParserID *)arg_p );

	*xid_p = atHandle;

	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	switch (op)
	{
	case TEL_EXT_TEST_CMD:    /* AT+CGQMIN=? */
	{
		ret = PS_GetGprsCapsQos(atHandle, TRUE);
		break;
	}

	case TEL_EXT_GET_CMD:    /* AT+CGQMIN?  */
	{
		ret = PS_GetQualityOfServiceList(atHandle, TRUE);
		break;
	}

	case TEL_EXT_SET_CMD:      /* AT+CGQMIN= */
	{
		/*
		**  Extract the arguments starting with the CID.
		*/
		if (( getExtValue(parameter_values_p, 0, &cid, TEL_AT_CGQMIN_0_CID_VAL_MIN, TEL_AT_CGQMIN_0_CID_VAL_MAX, TEL_AT_CGQMIN_0_CID_VAL_DEFAULT) == TRUE ) &&
		    ( cid > 0 ) && ( cid <= CI_PS_MAX_PDP_CTX_NUM ))
		{
			cid--;
			if ( getAtQosParams( parameter_values_p, 1, &minQosProfile ) == TRUE )
				ret = PS_SetQualityOfService(atHandle, cid, &minQosProfile, TRUE);
		}

		break;
	}

	case TEL_EXT_ACTION_CMD:   /* AT+CGQMIN */
	default:
		break;
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}

/************************************************************************************
 * F@: ciSetReqQOS - GLOBAL API for GCF AT+CGQREQ  -command
 *
 */
RETURNCODE_T  ciSetReqQOS(            const utlAtParameterOp_T op,
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
	CiReturnCode ret = CIRC_FAIL;
	INT32 cid;
	CiPsQosProfile reqQosProfile;
	UINT32 atHandle = MAKE_AT_HANDLE( *(TelAtParserID *)arg_p );

	*xid_p = atHandle;

	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	switch (op)
	{
	case TEL_EXT_TEST_CMD:    /* AT+CGQREQ=? */
	{
		ret = PS_GetGprsCapsQos(atHandle, FALSE);
		break;
	}

	case TEL_EXT_GET_CMD:    /* AT+CGQREQ?  */
	{
		ret = PS_GetQualityOfServiceList(atHandle, FALSE);
		break;
	}

	case TEL_EXT_SET_CMD:      /* AT+CGQREQ= */
	{
		/*
		**  Extract the arguments starting with the CID.
		*/
		if (( getExtValue(parameter_values_p, 0, &cid, TEL_AT_CGQREQ_0_CID_VAL_MIN, TEL_AT_CGQREQ_0_CID_VAL_MAX, TEL_AT_CGQREQ_0_CID_VAL_DEFAULT) == TRUE ) &&
		    ( cid > 0 ) && ( cid <= CI_PS_MAX_PDP_CTX_NUM ))
		{
			cid--;
			if ( getAtQosParams( parameter_values_p, 1, &reqQosProfile ) == TRUE )
			{
				DBGMSG("%s: reqQosProfile.precedence: %d"
				       "reqQosProfile.delay: %d reqQosProfile.reliability: %d"
				       "reqQosProfile.peak: %d reqQosProfile.mean: %d\n",
				       __FUNCTION__, reqQosProfile.precedence, reqQosProfile.delay,
				       reqQosProfile.reliability, reqQosProfile.peak, reqQosProfile.mean);
				ret = PS_SetQualityOfService(atHandle, cid, &reqQosProfile, FALSE);
			}

		}

		break;
	}

	case TEL_EXT_ACTION_CMD:   /* AT+CGQREQ */
	default:
	{
		break;
	}
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}

RETURNCODE_T ciAUTHReq(            const utlAtParameterOp_T op,
					const char                      *command_name_p,
					const utlAtParameterValue_P2c parameter_values_p,
					const size_t num_parameters,
					const char                      *info_text_p,
					unsigned int                    *xid_p,
					void                            *arg_p)
{
	UNUSEDPARAM(command_name_p);
	UNUSEDPARAM(num_parameters);
	UNUSEDPARAM(info_text_p);
	UNUSEDPARAM(arg_p);
	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	CiReturnCode ret = CIRC_FAIL;
	CiPsPrimAuthenticateReq authReq;

	CHAR                     usernameVal[TEL_AT_AUTH_2_USERNAME_STR_MAX_LEN + ATCI_NULL_TERMINATOR_LENGTH];//CI Limit for User name and password is 50
	CHAR                     passwordVal[TEL_AT_AUTH_3_PASSWORD_STR_MAX_LEN + ATCI_NULL_TERMINATOR_LENGTH];//CI Limit for User name and password is 50
	INT32                    cid;
	INT32                    authType;
	INT16                    strLenUserName = 0, strLenPassword = 0;

	UINT32 atHandle = MAKE_AT_HANDLE( *(TelAtParserID *)arg_p );
	*xid_p = atHandle;

	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);
	switch (op)
	{
	case TEL_EXT_GET_CMD:    /* AT*AUTHReq?  */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_OPERATION_NOT_SUPPORTED, NULL );
		break;
	}
	case TEL_EXT_TEST_CMD:	 /* AT*AUTHReq=?  */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"*AUTHReq: <cid>,<type>(0-None;1-PAP;2-CHAP),<UserName>,<Password>\r\n");
		break;
	}
	case TEL_EXT_SET_CMD:      /* AT*AUTHReq= */
	{
		if( getExtValue( parameter_values_p, 0, &cid, TEL_AT_AUTH_0_CID_VAL_MIN, TEL_AT_AUTH_0_CID_VAL_MAX, TEL_AT_AUTH_0_CID_VAL_DEFAULT ) == TRUE )
		{
			cid--;
			authReq.cid = cid;
			if( getExtValue( parameter_values_p, 1, &authType, TEL_AT_AUTH_1_TYPE_VAL_MIN, TEL_AT_AUTH_1_TYPE_VAL_MAX, TEL_AT_AUTH_1_TYPE_VAL_DEFAULT ) == TRUE )
			{
				authReq.AuthenticationType = authType;
				if ( getExtString(parameter_values_p, 2, &usernameVal[0], TEL_AT_AUTH_2_USERNAME_STR_MAX_LEN, &strLenUserName, TEL_AT_AUTH_2_USERNAME_STR_DEFAULT) == TRUE )
				{
					if ( getExtString(parameter_values_p, 3, &passwordVal[0], TEL_AT_AUTH_3_PASSWORD_STR_MAX_LEN, &strLenPassword, TEL_AT_AUTH_3_PASSWORD_STR_DEFAULT) == TRUE )
					{

						if(strLenUserName == 0 || strLenPassword == 0)
						{
							ret = CIRC_SH_NULL_REQPARAS;
						}
						else
						{
							authReq.UserName.len = strLenUserName;
							strcpy((char *)authReq.UserName.valStr, (char *)usernameVal);
							authReq.Password.len = strLenPassword;
							strcpy((char *)authReq.Password.valStr, (char *)passwordVal);
							ret = PS_SendAUTHReq(atHandle,&authReq);
						}
					}
				}
			}
		}
		break;
	}
	case TEL_EXT_ACTION_CMD:   /* AT*AUTHReq */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_OPERATION_NOT_SUPPORTED, NULL );
		break;
	}
	default:
		break;
	}

	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);
}

/************************************************************************************
 * F@: ciSetSContext - GLOBAL API for GCF AT+CGDSCONT -command
 *                     (define econdary PDP context)
 */
RETURNCODE_T  ciSetSContext(            const utlAtParameterOp_T op,
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
	CiReturnCode ret = CIRC_FAIL;
	INT32 dCompPar;
	INT32 hCompPar;
	INT32 secCidPar;
	INT32 primCidPar;
	CiPsSecPdpCtx secPdpContext;
	CiPsDcomp dcomp = 0;
	CiPsHcomp hcomp = 0;
	UINT32 atHandle = MAKE_AT_HANDLE( *(TelAtParserID *)arg_p );

	*xid_p = atHandle;

	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	switch (op)
	{
	case TEL_EXT_GET_CMD:    /* AT+CGDSCONT?  */
	{
		ret = PS_GetSecondaryContextList(atHandle);
		break;
	}

	case TEL_EXT_TEST_CMD:    /* AT+CGDSCONT=? */
	{
		ret = PS_GetSecondaryContextRange(atHandle);
		break;
	}

	case TEL_EXT_SET_CMD:      /* AT+CGDSCONT= */
	{
		/*
		**  Extract the arguments starting with the CID.
		*/
		if ( getExtValue(parameter_values_p, 0, &secCidPar, TEL_AT_CGDSCONT_0_CID_VAL_MIN, TEL_AT_CGDSCONT_0_CID_VAL_MAX, TEL_AT_CGDSCONT_0_CID_VAL_DEFAULT) == TRUE )
		{
			/* If only the second CID is included, it should undefine it */
			if (parameter_values_p[1].is_default && parameter_values_p[2].is_default && parameter_values_p[3].is_default)
			{
				secCidPar--;
				ret = PS_DeleteSecondaryPDPContext(atHandle, secCidPar);
				break;
			}
			if ( getExtValue(parameter_values_p, 1, &primCidPar, TEL_AT_CGDSCONT_1_PCID_VAL_MIN, TEL_AT_CGDSCONT_1_PCID_VAL_MAX, TEL_AT_CGDSCONT_1_PCID_VAL_DEFAULT) == TRUE )
			{
				if ( getExtValue(parameter_values_p, 2, &dCompPar, TEL_AT_CGDSCONT_2_DCOMP_VAL_MIN, TEL_AT_CGDSCONT_2_DCOMP_VAL_MAX, TEL_AT_CGDSCONT_2_DCOMP_VAL_DEFAULT) == TRUE )
				{
					if ( getExtValue(parameter_values_p, 3, &hCompPar, TEL_AT_CGDSCONT_3_HCOMP_VAL_MIN, TEL_AT_CGDSCONT_3_HCOMP_VAL_MAX, TEL_AT_CGDSCONT_3_HCOMP_VAL_DEFAULT) == TRUE )
					{
						/*
						**  Check the CIDs range; the primary CID must exist;
						**  the secondary CID must not exist
						*/
						if ( ( secCidPar <= CI_PS_MAX_PDP_CTX_NUM) && (secCidPar > 0)
						     && ( primCidPar <= CI_PS_MAX_PDP_CTX_NUM ) && (primCidPar > 0)
						     && ( primCidPar != secCidPar ))
						{
							/* CI - SAC use cid range starting from 0 */
							secCidPar--;
							primCidPar--;

							switch ( dCompPar )
							{
							case ATCI_PS_DCOMP_OFF:
							case ATCI_DEFAULT_PARAM_VALUE:
								dcomp = CI_PS_DCOMP_OFF;
								break;

							case ATCI_PS_DCOMP_ON:
								dcomp = CI_PS_DCOMP_ON;
								break;

							case ATCI_PS_DCOMP_V42bis:
								dcomp = CI_PS_DCOMP_V42bis;
								break;

							default:
								goto invalid;
							}

							switch ( hCompPar )
							{
							case ATCI_PS_HCOMP_OFF:
							case ATCI_DEFAULT_PARAM_VALUE:
								hcomp = CI_PS_HCOMP_OFF;
								break;

							case ATCI_PS_HCOMP_TCPIP:
								hcomp = CI_PS_HCOMP_TCPIP;
								break;

							case ATCI_PS_HCOMP_IP:
								hcomp = CI_PS_HCOMP_IP;
								break;

							default:
								goto invalid;
							}

							secPdpContext.cid = secCidPar;
							secPdpContext.p_cid = primCidPar;
							secPdpContext.dcompPresent = TRUE;
							secPdpContext.dcomp = dcomp;
							secPdpContext.hcompPresent = TRUE;
							secPdpContext.hcomp = hcomp;

							ret = PS_SetSecondaryPDPContext(atHandle, &secPdpContext);
						}       /* if ( cid <= MAX_NUMBER_OF_CIDS ) */
					}               /* if( getExtValue(parameter_values_p, X, &hCompPar,  */
				}                       /* if( getExtValue(parameter_values_p, X, &dCompPar,  */
			}                               /* if( getExtValue(parameter_values_p, X, &primCidPar,  */
		}   /* if( getExtValue(parameter_values_p, X, &secCidPar,  */

		break;
	}

	case TEL_EXT_ACTION_CMD:   /* AT+CGDSCONT */
	default:
		break;
	}

invalid:
	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}

/************************************************************************************
 * F@: Encode3GBitrate - Encode bitrate value for 3G QoS as per 3GPP TS 24.008
 * Notes       : See 3GPP TS 24.008/V3.11.0, Table 10.5.156 for
 *               the coding scheme.
 *               This function is used for the Maximum and Guaranteed
 *               Bit Rate (both Uplink and Downlink) parameters.
 */
UINT8 Encode3GBitrate( INT16 rawValue )
{
	UINT8 codedValue = 0x00; /* Default = Subscribed Value */

	if ( rawValue >= 1 && rawValue <= 63 )
	{
		/* 1kbps..63kbps (1kbps steps) -> 0x01..0x3f */
		codedValue = (UINT8)rawValue;
	}
	else if ( rawValue >= 64 && rawValue <= 568 )
	{
		/* 64kbps..568kbps (8kbps steps) -> 0x40..0x7f */
		codedValue = (UINT8)( ( rawValue - 64 ) / 8 ) + 0x40;
	}
	else if ( rawValue >= 576 && rawValue <= 8640 )
	{
		/* 576kbps..8640kbps (64kbps steps) -> 0x80..0xfe */
		codedValue = (UINT8)( ( rawValue - 576 ) / 64 ) + 0x80;
	}

	/* Out-of-range values are coded as default (set above) */

	return( codedValue );
}


/************************************************************************************
 * F@: Encode3GMaxSduSize - Encodes 3G QoS Maximum SDU Size parameter value
 * Notes       : See 3GPP TS 24.008/V3.11.0, Table 10.5.156 for
 *               the coding scheme.
 *
 */
UINT8 Encode3GMaxSduSize( INT16 rawValue )
{
	UINT8 codedValue = 0x00; /* Default = Subscribed Value */

	if ( rawValue >= 10 && rawValue <= 1500 )
	{
		/* 10..1500 octets (10-octet steps) -> 0x01..0x96 */
		codedValue = (UINT8)( rawValue / 10 );
	}
	else if ( rawValue == 1502 )
	{
		/* 1502 octets -> 0x97 */
		codedValue = (UINT8)0x97;
	}
	else if ( rawValue == 1510 )
	{
		/* 1510 octets -> 0x98 */
		codedValue = (UINT8)0x98;
	}
	else if ( rawValue == 1520 )
	{
		/* 1520 octets -> 0x99 */
		codedValue = (UINT8)0x99;
	}

	/* Out-of-range values are coded as default (set above) */

	return( codedValue );
}


/************************************************************************************
 * F@:  Encode3GTransDelay - Encodes 3G QoS Transfer Delay parameter value
 * Notes       : See 3GPP TS 24.008/V3.11.0, Table 10.5.156 for
 *               the coding scheme.
 *
 */
UINT8 Encode3GTransDelay( INT16 rawValue )
{
	UINT8 codedValue = 0x00; /* Default = Subscribed Value */

	if ( rawValue >= 10 && rawValue <= 150 )
	{
		/* 10ms..150ms (10ms steps) -> 0x01..0x0f */
		codedValue = (UINT8)( rawValue / 10 );
	}
	else if ( rawValue >= 200 && rawValue <= 950 )
	{
		/* 200ms..950ms (50ms steps) -> 0x10..0x1f */
		codedValue = (UINT8)( ( rawValue - 200 ) / 50 ) + 0x10;
	}
	else if ( rawValue >= 1000 && rawValue <= 4000 )
	{
		/* 1000ms..4000ms (100ms steps) -> 0x20..0x3e */
		codedValue = (UINT8)( ( rawValue - 1000 ) / 100 ) + 0x20;
	}

	/* Out-of-range values are coded as default (set above) */

	return( codedValue );
}

/************************************************************************************
 * F@: getAt3GQosParams - extracts the 3G Qos attributes from AT command line string
 *
 */
BOOL getAt3GQosParams ( const utlAtParameterValue_P2c parameter_values_p, CiPs3GQosProfile *qosProfile )
{

	INT32 trafficPriority;
	INT32 transferDelay;
	INT32 trafficClass;
	INT32 maxBitRateUplink;
	INT32 maxBitRateDownlink;
	INT32 guaranteedBitRateUplink;
	INT32 guaranteedBitRateDownlink;
	INT32 deliveryOrder;
	INT32 sduErrorRatio;
	INT32 ratioBER;
	INT32 maxSduSize;
	INT32 errSdu;
	BOOL cmdValid = FALSE;


	/*
	**  Get Traffic class....
	*/
	if ( getExtValue(parameter_values_p, 1, &trafficClass, TEL_AT_CGEQREQ_1_TRAFFIC_VAL_MIN, TEL_AT_CGEQREQ_1_TRAFFIC_VAL_MAX, TEL_AT_CGEQREQ_1_TRAFFIC_VAL_DEFAULT) == TRUE )
	{
		/*
		**  Get Bitrate U/L....
		*/
		cmdValid = getExtValue( parameter_values_p, 2, &maxBitRateUplink,
					TEL_AT_CGEQREQ_2_MAX_UL_VAL_MIN, TEL_AT_CGEQREQ_2_MAX_UL_VAL_MAX, TEL_AT_CGEQREQ_2_MAX_UL_VAL_DEFAULT);

		if (( cmdValid == TRUE ) &&
		    ((( maxBitRateUplink > 64 && maxBitRateUplink < 568 ) && ( maxBitRateUplink % 8 != 0 )) ||
		     (( maxBitRateUplink > 568 && maxBitRateUplink < 8640 ) && ( maxBitRateUplink % 64 != 0 ))))
		{
			/*
			**  This is an invalid value.
			*/
			cmdValid = FALSE;
		}
		else
		{
			maxBitRateUplink = (INT32)Encode3GBitrate( (INT16)maxBitRateUplink );
		}

		/*
		**  Get Bitrate D/L....
		*/
		if ( cmdValid == TRUE )
		{
			cmdValid = getExtValue( parameter_values_p, 3, &maxBitRateDownlink,
						TEL_AT_CGEQREQ_3_MAX_DL_VAL_MIN, TEL_AT_CGEQREQ_3_MAX_DL_VAL_MAX, TEL_AT_CGEQREQ_3_MAX_DL_VAL_DEFAULT);

			if (( cmdValid == TRUE ) &&
			    ((( maxBitRateDownlink > 64 && maxBitRateDownlink < 568 ) && ( maxBitRateDownlink % 8 != 0 )) ||
			     (( maxBitRateDownlink > 568 && maxBitRateDownlink < 8640 ) && ( maxBitRateDownlink % 64 != 0 ))))
			{
				/*
				**  This is an invalid value.
				*/
				cmdValid = FALSE;
			}
			else
			{
				maxBitRateDownlink = (INT32)Encode3GBitrate( (INT16)maxBitRateDownlink );
			}
		}

		/*
		**  Get Guaranteed Bitrate U/L....
		*/
		if ( cmdValid == TRUE )
		{
			cmdValid = getExtValue( parameter_values_p, 4, &guaranteedBitRateUplink,
						TEL_AT_CGEQREQ_4_GUA_UL_VAL_MIN, TEL_AT_CGEQREQ_4_GUA_UL_VAL_MAX, TEL_AT_CGEQREQ_4_GUA_UL_VAL_DEFAULT );

			if (( cmdValid == TRUE ) &&
			    (((guaranteedBitRateUplink > 64 && guaranteedBitRateUplink < 568) && (guaranteedBitRateUplink % 8 != 0)) ||
			     ((guaranteedBitRateUplink > 568 && guaranteedBitRateUplink < 8640) && (guaranteedBitRateUplink % 64 != 0))))
			{
				/*
				**  This is an invalid value.
				*/
				cmdValid = FALSE;
			}
			else
			{
				guaranteedBitRateUplink = (INT32)Encode3GBitrate( (INT16)guaranteedBitRateUplink );
			}
		}

		/*
		**  Get Guaranteed Bitrate D/L....
		*/
		if ( cmdValid == TRUE )
		{
			cmdValid = getExtValue( parameter_values_p, 5, &guaranteedBitRateDownlink,
						TEL_AT_CGEQREQ_5_GUA_DL_VAL_MIN, TEL_AT_CGEQREQ_5_GUA_DL_VAL_MAX, TEL_AT_CGEQREQ_5_GUA_DL_VAL_DEFAULT );

			if (( cmdValid == TRUE ) &&
			    (((guaranteedBitRateDownlink > 64 && guaranteedBitRateDownlink < 568) &&
			      (guaranteedBitRateDownlink % 8 != 0)) ||
			     ((guaranteedBitRateDownlink > 568 && guaranteedBitRateDownlink < 8640) &&
		      (guaranteedBitRateDownlink % 64 != 0))))
			{
				/*
				**  This is an invalid value.
				*/
				cmdValid = FALSE;
			}
			else
			{
				guaranteedBitRateDownlink = (INT32)Encode3GBitrate( (INT16)guaranteedBitRateDownlink );
			}
		}

		/*
		**  Get Delivery Order....
		*/
		if ( cmdValid == TRUE )
		{
			cmdValid = getExtValue( parameter_values_p, 6, &deliveryOrder,
						TEL_AT_CGEQREQ_6_DELORDER_VAL_MIN, TEL_AT_CGEQREQ_6_DELORDER_VAL_MAX, TEL_AT_CGEQREQ_6_DELORDER_VAL_DEFAULT );
		}

		/*
		**  Get max SDU size....
		*/
		if ( cmdValid == TRUE )
		{
			cmdValid = getExtValue( parameter_values_p, 7, &maxSduSize,
						TEL_AT_CGEQREQ_7_MAXSDU_VAL_MIN, TEL_AT_CGEQREQ_7_MAXSDU_VAL_MAX, TEL_AT_CGEQREQ_7_MAXSDU_VAL_DEFAULT );

			if (( cmdValid == TRUE ) &&
			    (( maxSduSize > 0 && maxSduSize < 10 ) ||
			     (( maxSduSize > 10 && maxSduSize < 1500) && ( maxSduSize % 10 != 0 )) ||
			     ( maxSduSize == 1501 ) ||
			     ( maxSduSize > 1502 && maxSduSize < 1510 ) ||
			     ( maxSduSize > 1510 && maxSduSize < 1520 )))
			{
				/*
				**  This is an invalid value.
				*/
				cmdValid = FALSE;
			}
			else
			{
				maxSduSize = (INT32)Encode3GMaxSduSize( (INT16)maxSduSize );
			}
		}

		/*
		**  Get SDU error ratio....
		*/
		if ( cmdValid == TRUE )
		{
			cmdValid = getErrorRatioParam( parameter_values_p, 8, errorRatioSDU,
						       TEL_AT_CGEQREQ_8_SDUERR_VAL_DEFAULT, &sduErrorRatio );
		}

		if ( cmdValid == TRUE )
		{
			cmdValid = getErrorRatioParam( parameter_values_p, 9, errorRatioBER,
						       TEL_AT_CGEQREQ_9_RBERR_VAL_DEFAULT, &ratioBER );
		}

		/*
		**  Get delivery of erroneous SDUs....
		*/
		if ( cmdValid == TRUE )
		{
			cmdValid = getExtValue( parameter_values_p, 10, &errSdu,
						TEL_AT_CGEQREQ_10_DERSDU_VAL_MIN, TEL_AT_CGEQREQ_10_DERSDU_VAL_MAX, TEL_AT_CGEQREQ_10_DERSDU_VAL_DEFAULT );
		}

		/*
		**  Get transfer delay....
		*/
		if ( cmdValid == TRUE )
		{
			cmdValid = getExtValue( parameter_values_p, 11, &transferDelay, TEL_AT_CGEQREQ_11_TRDELAY_VAL_MIN, TEL_AT_CGEQREQ_11_TRDELAY_VAL_MAX, TEL_AT_CGEQREQ_11_TRDELAY_VAL_DEFAULT );

			if ( cmdValid == TRUE )
			{
				if ((( transferDelay > 0 && transferDelay < 150 ) && ( transferDelay % 10 != 0 )) ||
				    (( transferDelay > 150 && transferDelay < 950 ) && ( transferDelay % 50 != 0 )) ||
				    ((transferDelay > 950 && transferDelay < 4000 ) && ( transferDelay % 100 != 0 )))
				{
					cmdValid = FALSE;
				}
				else
				{
					transferDelay = (INT32)Encode3GTransDelay( (INT16)transferDelay );
				}
			}
		}

		/*
		**  Get traffic handling priority....
		*/
		if ( cmdValid == TRUE )
		{
			cmdValid = atParamToCiEnum( parameter_values_p, 12, &trafficPriority,
						    TEL_AT_CGEQREQ_12_PRIORITY_VAL_DEFAULT,
						    enumTrafficPriorityLookup );
		}
	} /* if ( _atParamToCiEnum( parameter_values_p, X, &trafficClass, */


	if ( cmdValid == TRUE )
	{
		/*
		**  Set the CI request parameters.
		*/
		qosProfile->trafficClass     = (CiPs3GTrafficClass)trafficClass;
		qosProfile->deliveryOrder    = (CiPs3GDlvOrder)deliveryOrder;
		qosProfile->sduErrRatio        = (CiPs3GSduErrorRatio)sduErrorRatio;
		qosProfile->resBER            = (CiPs3GResidualBer)ratioBER;
		qosProfile->deliveryOfErrSdu = (CiPs3GDlvErrorSdu)errSdu;
		qosProfile->thPriority        = (CiPs3GTrafficPriority)trafficPriority;
		qosProfile->maxSduSize        = (UINT8)maxSduSize;
		qosProfile->transDelay        = (UINT8)transferDelay;
		qosProfile->maxULRate        = (UINT8)maxBitRateUplink;
		qosProfile->maxDLRate        = (UINT8)maxBitRateDownlink;
		qosProfile->guaranteedULRate = (UINT8)guaranteedBitRateUplink;
		qosProfile->guaranteedDLRate = (UINT8)guaranteedBitRateDownlink;
	}

	return cmdValid;
}


/************************************************************************************
 * F@: ciSet3GQOS - GLOBAL API for GCF AT+CGEQREQ  -command
 *
 * NOTE: This function does not approximate error ratio value: if a value not defined in 24.008, it is rejected
 */
RETURNCODE_T  ciSet3GQOS(            const utlAtParameterOp_T op,
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
	CiReturnCode ret = CIRC_FAIL;
	BOOL cmdValid = FALSE;
	INT32 cid;

	CiPs3GQosProfile qosProfile;
	UINT32 atHandle = MAKE_AT_HANDLE( *(TelAtParserID *)arg_p );

	*xid_p = atHandle;

	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	switch (op)
	{
	case TEL_EXT_GET_CMD:    /* AT+CGEQREQ?  */
	{
		ret = PS_Get3GQualityOfServiceList(atHandle, CI_PS_3G_QOSTYPE_REQ);
		break;
	}

	case TEL_EXT_TEST_CMD:    /* AT+CGEQREQ=? */
	{
		ret = PS_Get3GCapsQos(atHandle, CI_PS_3G_QOSTYPE_REQ);
		break;
	}

	case TEL_EXT_SET_CMD:      /* AT+CGEQREQ= */
	{
		/*
		**  Extract the arguments starting with the CID.
		*/
		if (( getExtValue(parameter_values_p, 0, &cid, TEL_AT_CGEQREQ_0_CID_VAL_MIN, TEL_AT_CGEQREQ_0_CID_VAL_MAX, TEL_AT_CGEQREQ_0_CID_VAL_DEFAULT) == TRUE )  &&
		    ( cid > 0 ) && ( cid <= CI_PS_MAX_PDP_CTX_NUM ))
		{

			cmdValid = getAt3GQosParams(parameter_values_p, &qosProfile);

			if ( cmdValid == TRUE )
				ret = PS_Set3GQualityOfService(atHandle, cid - 1, &qosProfile, CI_PS_3G_QOSTYPE_REQ);

		}       /*  if (( getExtValue(parameter_values_p, X, &cid */
		break;
	}

	case TEL_EXT_ACTION_CMD:   /* AT+CGEQREQ */
	default:
	{
		break;
	}
	}


	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}

/************************************************************************************
 * F@: ciSet3GminQOS - GLOBAL API for GCF AT+CGEQMIN  -command
 *
 */
RETURNCODE_T  ciSet3GminQOS(            const utlAtParameterOp_T op,
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
	CiReturnCode ret = CIRC_FAIL;
	BOOL cmdValid = FALSE;
	INT32 cid;

	CiPs3GQosProfile qosProfile;
	UINT32 atHandle = MAKE_AT_HANDLE( *(TelAtParserID *)arg_p );

	*xid_p = atHandle;

	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	switch (op)
	{
	case TEL_EXT_GET_CMD:    /* AT+CGEQMIN?  */
	{
		ret = PS_Get3GQualityOfServiceList(atHandle, CI_PS_3G_QOSTYPE_MIN);
		break;
	}

	case TEL_EXT_TEST_CMD:    /* AT+CGEQMIN=? */
	{
		ret = PS_Get3GCapsQos(atHandle, CI_PS_3G_QOSTYPE_MIN);
		break;
	}

	case TEL_EXT_SET_CMD:      /* AT+CGEQMIN= */
	{
		/*
		**  Extract the arguments starting with the CID.
		*/
		if (( getExtValue(parameter_values_p, 0, &cid, TEL_AT_CGEQREQ_0_CID_VAL_MIN, TEL_AT_CGEQREQ_0_CID_VAL_MAX, TEL_AT_CGEQREQ_0_CID_VAL_DEFAULT) == TRUE )  &&
		    ( cid > 0 ) && ( cid <= CI_PS_MAX_PDP_CTX_NUM ))
		{
			cmdValid = getAt3GQosParams(parameter_values_p, &qosProfile);

			if ( cmdValid == TRUE )
				ret = PS_Set3GQualityOfService(atHandle, cid - 1, &qosProfile, CI_PS_3G_QOSTYPE_MIN);
		}       /*  if (( getExtValue(parameter_values_p, X, &cid */

		break;
	}

	case TEL_EXT_ACTION_CMD:   /* AT+CGEQREQ */
	default:
	{
		break;
	}
	}


	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);
}

/************************************************************************************
 * F@: ciPdpErrorReport - GLOBAL API for AT+PEER  -command
 *
 */
RETURNCODE_T  ciPdpErrorReport(            const utlAtParameterOp_T op,
					   const char                      *command_name_p,
					   const utlAtParameterValue_P2c parameter_values_p,
					   const size_t num_parameters,
					   const char                      *info_text_p,
					   unsigned int                    *xid_p,
					   void                            *arg_p)
{
	UNUSEDPARAM(command_name_p)
	UNUSEDPARAM(parameter_values_p)
	UNUSEDPARAM(num_parameters)
	UNUSEDPARAM(info_text_p)

	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	CiReturnCode ret = CIRC_FAIL;
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);

	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	switch (op)
	{
	case TEL_EXT_ACTION_CMD:   /* AT+PEER */
	{
		char tmpBuf[300];
		sprintf(tmpBuf, "+PEER: \"%s\"\r\n", gPdpErrCauseBuf);
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, tmpBuf);
		break;
	}

	case TEL_EXT_TEST_CMD:    /* AT+PEER=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		break;
	}

	case TEL_EXT_SET_CMD:           /* AT+PEER= */
	case TEL_EXT_GET_CMD:           /* AT+PEER?  */
	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL );
		break;
	}
	}

	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}

/************************************************************************************
 * F@: ciPdpErrorReport - GLOBAL API for AT*FastDorm  -command
 *
 */
RETURNCODE_T ciFastDormancy(            const utlAtParameterOp_T op,
					    const char                      *command_name_p,
					   const utlAtParameterValue_P2c parameter_values_p,
					   const size_t num_parameters,
					   const char                      *info_text_p,
					   unsigned int                    *xid_p,
					   void                            *arg_p)
{
	UNUSEDPARAM(command_name_p);
	UNUSEDPARAM(parameter_values_p);
	UNUSEDPARAM(num_parameters);
	UNUSEDPARAM(info_text_p);
	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	CiReturnCode ret = CIRC_FAIL;

	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);

	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	switch(op)
	{
	case TEL_EXT_ACTION_CMD:	/* AT*FASTDORM */
	{
		ret = PS_FastDormancy(atHandle);
		break;
	}
	case TEL_EXT_TEST_CMD:		/* AT*FASTDORM=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"*FASTDORM:\r\nOK");
		break;
	}
	case TEL_EXT_GET_CMD:                   /* AT*FASTDORM?  */
	case TEL_EXT_SET_CMD:                   /* AT*FASTDORM=  */
	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CMS_ERROR, CMS_OPERATION_NOT_SUPPORTED, NULL );
		break;
	}
	}
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}
#ifdef AT_CUSTOMER_HTC
/************************************************************************************
 * F@: ciPdpErrorReport - GLOBAL API for AT*FDY  -command
 *
 */
RETURNCODE_T ciStarFDY(            const utlAtParameterOp_T op,
					    const char                      *command_name_p,
					   const utlAtParameterValue_P2c parameter_values_p,
					   const size_t num_parameters,
					   const char                      *info_text_p,
					   unsigned int                    *xid_p,
					   void                            *arg_p)
{
	UNUSEDPARAM(command_name_p);
	UNUSEDPARAM(parameter_values_p);
	UNUSEDPARAM(num_parameters);
	UNUSEDPARAM(info_text_p);
	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	CiReturnCode ret = CIRC_FAIL;

	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);

	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	switch(op)
	{
	case TEL_EXT_SET_CMD:	/* AT*FDY= */
	{
		BOOL cmdValid = FALSE;
		int type, interval;
		if (getExtValue(parameter_values_p, 0, &type, TEL_AT_FDY_0_TYPE_VAL_MIN, TEL_AT_FDY_0_TYPE_VAL_MAX, TEL_AT_FDY_0_TYPE_VAL_DEFAULT) == TRUE )
		{
			if (getExtValue(parameter_values_p, 1, &interval, TEL_AT_FDY_1_INTERVAL_VAL_MIN, TEL_AT_FDY_1_INTERVAL_VAL_MAX, TEL_AT_FDY_1_INTERVAL_VAL_DEFAULT) == TRUE )
			{
				cmdValid = TRUE;
			}
		}
		if(cmdValid)
		{
			ret = PS_SetFDY(atHandle, (UINT8)type, (UINT16)interval);
		}
		else
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL );
		}
		break;
	}
	case TEL_EXT_TEST_CMD:		/* AT*FDY=? */
	case TEL_EXT_GET_CMD:       /* AT*FDY?  */
	case TEL_EXT_ACTION_CMD:    /* AT*FDY  */
	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL );
		break;
	}
	}
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}
#endif
