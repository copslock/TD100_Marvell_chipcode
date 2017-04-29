/******************************************************************************

*(C) Copyright 2007 Marvell International Ltd.

* All Rights Reserved

******************************************************************************
*  Filename: telmisc.c
*
*  Description: Process WIFI/BT/FM related AT commands and other misc functions
*
*  History:
*   Aug 17, 2010 - Creation of file
*   
*
******************************************************************************/

/******************************************************************************
*    Include files
******************************************************************************/
#ifdef AT_LABTOOL_CMNDS

#include <stdio.h>
#include <stdlib.h>
#include "teldev.h"
#include "teldef.h"
#include "telutl.h"
#include "telmisc.h"
#include "utilities.h"

extern int wifi_test_api(int attr, int p1, int p2, char *data);
extern int wifi_id_api(int attr, int p1, int p2, char *data);
extern int bt_test_api(int attr, int p1, int p2, char *data);
extern int bt_id_api(int attr, int p1, int p2, char *data);
extern int fm_test_api(int attr, int p1, int p2, char *data);

#define INPUT_DATA_LENGTH 150

/************************************************************************************
 * F@: ciMVWFTST - GLOBAL API for AT+MVWFTST command
 *
 */
RETURNCODE_T  ciMVWFTST(            const utlAtParameterOp_T        op,					
		const char                      *command_name_p,		
		const utlAtParameterValue_P2c   parameter_values_p,  
		const size_t                    num_parameters,		
		const char                      *info_text_p,       	
		unsigned int                    *xid_p,               		
		void                            *arg_p)
{
    
	UNUSEDPARAM(command_name_p);
	UNUSEDPARAM(num_parameters);
	UNUSEDPARAM(info_text_p);
	RETURNCODE_T      rc = INITIAL_RETURN_CODE;
	CiReturnCode      ret = CIRC_FAIL;
	INT16             dataStrLength;
	UINT32            atHandle = MAKE_AT_HANDLE(* (TelAtParserID *) arg_p);
	char              InputData[INPUT_DATA_LENGTH+1]={0};    
	*xid_p = atHandle;

	/*
	 * process operation
	 */
	switch(op)
	{
		case TEL_EXT_SET_CMD:      /* AT+MVWFTST= */
			if(InProduction_Mode())
			{
				int    attrVal,P1,P2;

				if( getExtValue( parameter_values_p, 0, &attrVal, 0, 0, 0 ) == TRUE )
				{
					if( getExtValue( parameter_values_p, 1, &P1, 0, 8, 0 ) == TRUE )
					{
						if( getExtValue( parameter_values_p, 2, &P2, 0, 10, 0 ) == TRUE )
						{
							if(getExtString(parameter_values_p, 3, (CHAR *)InputData, INPUT_DATA_LENGTH, &dataStrLength, NULL) == TRUE)
							{
								if(wifi_test_api(attrVal, P1, P2, InputData))
									ret = ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_WIFI_RETURN_ERROR,NULL);
								else
									ret = ATRESP( atHandle,ATCI_RESULT_CODE_OK,0,InputData);
							}
							else
								ret = ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_INVALID_PARAM,NULL);
						} 
						else
							ret = ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_INVALID_PARAM,NULL); 
					} 
					else
						ret = ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_INVALID_PARAM,NULL); 
				} 
				else
					ret = ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_INVALID_PARAM,NULL);
				
				break;
			}
			else
			{
				ret = ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_OPERATION_NOT_SUPPORTED,NULL);
				break;
			}

		case TEL_EXT_TEST_CMD:    /* AT+MVWFTST=? */
			{
				ret = ATRESP( atHandle,ATCI_RESULT_CODE_OK,0,(char *)"+MVWFTST: 0,(0-8),(0-10),data\r\n");
				break;
			}

		case TEL_EXT_GET_CMD:      /* AT+MVWFTST?  */
			{  
				ret = ATRESP( atHandle,ATCI_RESULT_CODE_OK,0,0);
				break;
			}

		default:
		      ret = ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_OPERATION_NOT_SUPPORTED,NULL);
            
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);
}



/************************************************************************************
 * F@: MVWFIDRW - GLOBAL API for AT+MVWFIDRW command
 *
 */
RETURNCODE_T  ciMVWFIDRW(            const utlAtParameterOp_T        op,					
		const char                      *command_name_p,		
		const utlAtParameterValue_P2c   parameter_values_p,  
		const size_t                    num_parameters,		
		const char                      *info_text_p,       	
		unsigned int                    *xid_p,               		
		void                            *arg_p)
{
    
	UNUSEDPARAM(command_name_p);
	UNUSEDPARAM(num_parameters);
	UNUSEDPARAM(info_text_p);
	RETURNCODE_T      rc = INITIAL_RETURN_CODE;
	CiReturnCode      ret = CIRC_FAIL;
	INT16             dataStrLength;
	UINT32            atHandle = MAKE_AT_HANDLE(* (TelAtParserID *) arg_p);
	char              MACAddress[100]={0};    
	*xid_p = atHandle;

	/*
	 * process operation
	 */
	switch(op)
	{
		case TEL_EXT_SET_CMD:      /* AT+MVWFIDRW= */
			if(InProduction_Mode())
			{
				int    attrVal,P1,P2;

				if( getExtValue( parameter_values_p, 0, &attrVal, 0, 0, 0 ) == TRUE )
				{
					if( getExtValue( parameter_values_p, 1, &P1, 0, 0, 0 ) == TRUE )
					{
						if( getExtValue( parameter_values_p, 2, &P2, 0, 1, 0 ) == TRUE )
						{
							if(getExtString(parameter_values_p, 3, (CHAR *)MACAddress, 17, &dataStrLength, NULL) == TRUE)
							{
								if(wifi_id_api(attrVal, P1, P2, MACAddress) )
									ret = ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_WIFI_RETURN_ERROR,NULL);
								else
									ret = ATRESP( atHandle,ATCI_RESULT_CODE_OK,0,MACAddress);
							}
							else
								ret = ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_INVALID_PARAM,NULL);
						} 
						else
							ret = ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_INVALID_PARAM,NULL); 
					} 
					else
						ret = ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_INVALID_PARAM,NULL); 
				} 
				else
					ret = ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_INVALID_PARAM,NULL);
				
				break;
			}
			else
			{
				ret = ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_OPERATION_NOT_SUPPORTED,NULL);
				break;
			}

		case TEL_EXT_TEST_CMD:    /* AT+MVWFIDRW=? */
			{
				ret = ATRESP( atHandle,ATCI_RESULT_CODE_OK,0,(char *)"+MVWFIDRW: 0,0,(0-1),data\r\n");
				break;
			}

		case TEL_EXT_GET_CMD:      /* AT+MVWFIDRW?  */
			{  
				ret = ATRESP( atHandle,ATCI_RESULT_CODE_OK,0,0);
				break;
			}

		default:
		      ret = ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_OPERATION_NOT_SUPPORTED,NULL);
            
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);
}



/************************************************************************************
 * F@: MVBTTST - GLOBAL API for AT+MVBTTST command
 *
 */
RETURNCODE_T  ciMVBTTST(            const utlAtParameterOp_T        op,					
		const char                      *command_name_p,		
		const utlAtParameterValue_P2c   parameter_values_p,  
		const size_t                    num_parameters,		
		const char                      *info_text_p,       	
		unsigned int                    *xid_p,               		
		void                            *arg_p)
{
    
	UNUSEDPARAM(command_name_p);
	UNUSEDPARAM(num_parameters);
	UNUSEDPARAM(info_text_p);
	RETURNCODE_T      rc = INITIAL_RETURN_CODE;
	CiReturnCode      ret = CIRC_FAIL;
	INT16             dataStrLength;
	UINT32            atHandle = MAKE_AT_HANDLE(* (TelAtParserID *) arg_p);
	char              InputData[INPUT_DATA_LENGTH + 1]={0}; 
	*xid_p = atHandle;

	/*
	 * process operation
	 */
	switch(op)
	{
		case TEL_EXT_SET_CMD:      /* AT+MVBTTST= */
			if(InProduction_Mode())
			{
				int    attrVal,P1,P2;

				if( getExtValue( parameter_values_p, 0, &attrVal, 0, 0, 0 ) == TRUE )
				{
					if( getExtValue( parameter_values_p, 1, &P1, 0, 9, 0 ) == TRUE )
					{
						if( getExtValue( parameter_values_p, 2, &P2, 0, 2, 0 ) == TRUE )
						{
							if(getExtString(parameter_values_p, 3, (CHAR *)InputData, INPUT_DATA_LENGTH, &dataStrLength, NULL) == TRUE)
							{
								if( bt_test_api(attrVal, P1, P2, InputData) )
									ret = ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_BT_RETURN_ERROR,NULL);
								else
									ret = ATRESP( atHandle,ATCI_RESULT_CODE_OK,0,InputData);
							}
							else
								ret = ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_INVALID_PARAM,NULL);
						} 
						else
							ret = ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_INVALID_PARAM,NULL); 
					} 
					else
						ret = ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_INVALID_PARAM,NULL); 
				} 
				else
					ret = ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_INVALID_PARAM,NULL);
				
				break;
			}
			else
			{
				ret = ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_OPERATION_NOT_SUPPORTED,NULL);
				break;
			}

		case TEL_EXT_TEST_CMD:    /* AT+MVBTTST=? */
			{
				ret = ATRESP( atHandle,ATCI_RESULT_CODE_OK,0,(char *)"+MVBTTST: 0,(0-9),(0-2),data\r\n");
				break;
			}

		case TEL_EXT_GET_CMD:      /* AT+MVBTTST?  */
			{  
				ret = ATRESP( atHandle,ATCI_RESULT_CODE_OK,0,0);
				break;
			}

		default:
		      ret = ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_OPERATION_NOT_SUPPORTED,NULL);
            
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);
}



/************************************************************************************
 * F@: MVBTIDRW - GLOBAL API for AT+MVBTIDRW command
 *
 */
RETURNCODE_T  ciMVBTIDRW(            const utlAtParameterOp_T        op,					
		const char                      *command_name_p,		
		const utlAtParameterValue_P2c   parameter_values_p,  
		const size_t                    num_parameters,		
		const char                      *info_text_p,       	
		unsigned int                    *xid_p,               		
		void                            *arg_p)
{
    
	UNUSEDPARAM(command_name_p);
	UNUSEDPARAM(num_parameters);
	UNUSEDPARAM(info_text_p);
	RETURNCODE_T      rc = INITIAL_RETURN_CODE;
	CiReturnCode      ret = CIRC_FAIL;
	INT16             dataStrLength;
	UINT32            atHandle = MAKE_AT_HANDLE(* (TelAtParserID *) arg_p);
	char              MACAddress[100]={0};
	*xid_p = atHandle;

	/*
	 * process operation
	 */
	switch(op)
	{
		case TEL_EXT_SET_CMD:      /* AT+MVBTIDRW= */
			if(InProduction_Mode())
			{
				int    attrVal,P1,P2;

				if( getExtValue( parameter_values_p, 0, &attrVal, 0, 0, 0 ) == TRUE )
				{
					if( getExtValue( parameter_values_p, 1, &P1, 0, 1, 0 ) == TRUE )
					{
						if( getExtValue( parameter_values_p, 2, &P2, 0, 0, 0 ) == TRUE )
						{
							if(getExtString(parameter_values_p, 3, (CHAR *)MACAddress, 17, &dataStrLength, NULL) == TRUE)
							{
								if(bt_id_api(attrVal, P1, P2, MACAddress) )
									ret = ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_BT_RETURN_ERROR,NULL);
								else
									ret = ATRESP( atHandle,ATCI_RESULT_CODE_OK,0,MACAddress);
							}
							else
								ret = ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_INVALID_PARAM,NULL);
						} 
						else
							ret = ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_INVALID_PARAM,NULL); 

					} 
					else
						ret = ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_INVALID_PARAM,NULL); 

				} 
				else
					ret = ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_INVALID_PARAM,NULL);

				
				break;
			}
			else
			{
				ret = ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_OPERATION_NOT_SUPPORTED,NULL);
				break;
			}

		case TEL_EXT_TEST_CMD:    /* AT+MVBTIDRW=? */
			{
				ret = ATRESP( atHandle,ATCI_RESULT_CODE_OK,0,(char *)"+MVBTIDRW: 0,(0-1),0,data\r\n");
				break;
			}

		case TEL_EXT_GET_CMD:      /* AT+MVBTIDRW?  */
			{  
				ret = ATRESP( atHandle,ATCI_RESULT_CODE_OK,0,0);
				break;
			}

		default:
		      ret = ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_OPERATION_NOT_SUPPORTED,NULL);
            
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);
}


/************************************************************************************
 * F@: MVFMTST - GLOBAL API for AT+MVFMTST command
 *
 */
RETURNCODE_T  ciMVFMTST(            const utlAtParameterOp_T        op,					
		const char                      *command_name_p,		
		const utlAtParameterValue_P2c   parameter_values_p,  
		const size_t                    num_parameters,		
		const char                      *info_text_p,       	
		unsigned int                    *xid_p,               		
		void                            *arg_p)
{
	UNUSEDPARAM(command_name_p);
	UNUSEDPARAM(num_parameters);
	UNUSEDPARAM(info_text_p);
	RETURNCODE_T      rc = INITIAL_RETURN_CODE;
	CiReturnCode      ret = CIRC_FAIL;
	INT16             dataStrLength;
	UINT32            atHandle = MAKE_AT_HANDLE(* (TelAtParserID *) arg_p);
	char              InputData[INPUT_DATA_LENGTH + 1]={0};
	*xid_p = atHandle;

	/*
	 * process operation
	 */
	switch(op)
	{
		case TEL_EXT_SET_CMD:      /* AT+MVFMTST= */
			if(InProduction_Mode())
			{
				int    attrVal,P1,P2;

				if( getExtValue( parameter_values_p, 0, &attrVal, 0, 1, 0 ) == TRUE )
				{
					if( getExtValue( parameter_values_p, 1, &P1, 0, 2, 0 ) == TRUE )
					{
						if( getExtValue( parameter_values_p, 2, &P2, 0, 3, 0 ) == TRUE )
						{
							if(getExtString(parameter_values_p, 3, (CHAR *)InputData, 8, &dataStrLength, NULL) == TRUE)
							{
								if( fm_test_api(attrVal,P1,P2, InputData) )
									ret = ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_FM_RETURN_ERROR,NULL);
								else
									ret = ATRESP( atHandle,ATCI_RESULT_CODE_OK,0,InputData);
							}
							else
								ret = ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_INVALID_PARAM,NULL);
						}
						else
							ret = ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_INVALID_PARAM,NULL);

					}
					else
						ret = ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_INVALID_PARAM,NULL);

				}
				else
					ret = ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_INVALID_PARAM,NULL);


				break;
			}
			else
			{
				ret = ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_OPERATION_NOT_SUPPORTED,NULL);
				break;
			}

		case TEL_EXT_TEST_CMD:    /* AT+MVFMTST=? */
			{
				ret = ATRESP( atHandle,ATCI_RESULT_CODE_OK,0,(char *)"+MVFMTST: (0-1),(0-2),(0-3),data\r\n");
				break;
			}

		case TEL_EXT_GET_CMD:      /* AT+MVFMTST?  */
			{
				ret = ATRESP( atHandle,ATCI_RESULT_CODE_OK,0,0);
				break;
			}

		default:
		      ret = ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_OPERATION_NOT_SUPPORTED,NULL);

	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);
}

/************************************************************************************
 * F@: ciSWUPGRADE - GLOBAL API for AT^SWUPGRADE command
 *
 */
RETURNCODE_T  ciSWUPGRADE(            const utlAtParameterOp_T        op,					
		const char                      *command_name_p,		
		const utlAtParameterValue_P2c   parameter_values_p,  
		const size_t                    num_parameters,		
		const char                      *info_text_p,       	
		unsigned int                    *xid_p,               		
		void                            *arg_p)
{
    
	UNUSEDPARAM(command_name_p);
	UNUSEDPARAM(num_parameters);
	UNUSEDPARAM(info_text_p);
	RETURNCODE_T      rc = INITIAL_RETURN_CODE;
	CiReturnCode      ret = CIRC_FAIL;
	UINT32            atHandle = MAKE_AT_HANDLE(* (TelAtParserID *) arg_p);
	char              AtBuf[100]={0};
    
	*xid_p = atHandle;

	/*
	 * process operation
	 */
	switch(op)
	{
		case TEL_EXT_SET_CMD:      /* AT^SWUPGRADE= */
			{
				INT32    typeVal;

				if( getExtValue( parameter_values_p, 0, &typeVal, 0, 1, 1 ) == TRUE )
				{
					if(!typeVal)
						system("echo 0 > /sys/class/power_supply/battery/type");     //0x3E in sanremo is set to 0
					else if(typeVal == 1)
						system("echo 1 > /sys/class/power_supply/battery/type");      //0x3E in sanremo is set to 1
					else
					{                                
						sprintf( (char *)AtBuf, (char *)"input value is not 0 or 1\r\n");                                   	  
						ret = ATRESP( atHandle,ATCI_RESULT_CODE_OK,0,AtBuf);  
						rc = HANDLE_RETURN_VALUE(ret);
						return(rc);                               
					}


					ret = ATRESP( atHandle,ATCI_RESULT_CODE_OK,0,AtBuf);
					system("/sbin/reboot");
				} 
				else
				{
					ret = ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_INVALID_PARAM,NULL);
				}
				break;
			}

		case TEL_EXT_TEST_CMD:    /* AT^SWUPGRADE=? */
			{
				ret = ATRESP( atHandle,ATCI_RESULT_CODE_OK,0,(char *)"^SWUPGRADE: (0-1)\r\n");
				break;
			}

		case TEL_EXT_GET_CMD:      /* AT^SWUPGRADE?  */
			{  
				system("cat /sys/class/power_supply/battery/read_sanremo");          //read 0x3E in sanremo 

				ret = ATRESP( atHandle,ATCI_RESULT_CODE_OK,0,0);
				break;
			}

		default:
		      ret = ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_OPERATION_NOT_SUPPORTED,NULL);
            
	}

	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);
}

#endif // AT_LABTOOL_CMNDS

