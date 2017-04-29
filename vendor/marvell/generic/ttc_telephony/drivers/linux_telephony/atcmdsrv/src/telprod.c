/******************************************************************************
*(C) Copyright 2008 Marvell International Ltd.
* All Rights Reserved
******************************************************************************/
/*--------------------------------------------------------------------------------------------------------------------
 *  -------------------------------------------------------------------------------------------------------------------
 *
 *  Filename: telprod.c
 *
 *  Description: Implementation of propriatry AT Commands for production mode.
 *
 *  History:
 *   May 20, 2010 - Creation of file
 *
 *  Notes:
 *
 ******************************************************************************/
#ifdef AT_PRODUCTION_CMNDS
/******************************************************************************
 *    Include files
 ******************************************************************************/
#include "telprod.h"
#include "teldef.h"
#include "telutl.h"
#include <fcntl.h>
#include <dlfcn.h>
//#include <arch/arm/mach-pxa/include/mach/pxa3xx_dvfm.h>
#include "MRD.h"
#include "imei.h"
#include "mep.h"
#include <stdlib.h>
#include "pxa_dbg.h"
#include <linux/input.h>
#include "utilities.h"
#include "prod_api.h"

/******************************************************************************
 *    Local Definitions
 ******************************************************************************/
typedef struct
{
            unsigned long data_arg1;
            unsigned long data_arg2;
}data_struct;

#define MAX_FILE_NAME_LEN	116
#define MAX_VERSION_LEN		4
#define MAX_DATE_LEN		9
#define MAX_SERIAL_LEN      32
#define MAX_IMEI_LEN      	16
#define USER_DATA "Marvell"
#define SERIAL_NUM_FILE_FULL_NAME "/tmp/serialNum"
#define SERIAL_NUM_FILE_NAME "serialNum"
#define IMEI_NUM_FILE_FULL_NAME "/tmp/imei"
#define IMEI_NUM_FILE_NAME "imeiNum"
#define MEP1_FILE_FULL_NAME "/tmp/mep1"
#define MEP1_FILE_NAME "mep1"
#define MAX_PASSWORD_LEN   16

#define UNUSEDPARAM(param) (void)param
#define NVM_ROOT_DIR "NVM_ROOT_DIR"

#define RUN_SLEEP_SCRIPT "/marvell/tel/run_sleep.sh"
#define SEND_CFUN0       "/marvell/tel/serial_cfun0"
#define KEYUP            0
#define KEYDOWN          1
#define KEYREPEATE       2
#define KEYPAD_FILE      "/dev/input/event0"
#define KEY_POWER        116
#define KEY_END          107
#define KEY_MENU         139
#define SYN_REPORT       0
#define KEY_BACK         158

/******************************************************************************
 *    Functions
 ******************************************************************************/

/************************************************************************************
 * F@: ValidImei - Check if validity if the IMEI number.
 * The IMEI number of digits should be 14, 15, or 16 digits. otherwise return error.
 *
 * Returns TRUE if the IMEI is valid, otherwise resturns FALSE *
 ************************************************************************************/
bool ValidImei(CHAR *imei, INT16 imeiLen)
{
	bool valid = FALSE;
 	UINT8 uiIMEIarr[MAX_IMEI_LEN];
    UINT8 uiIMEIsum = 0;
    UINT8 uiCD = 0;
    UINT8 i = 0;
	char c;

	if (imeiLen == 14)
	{
		// Zero the last 2 digits
		imei[14] = imei[15] = 0;
		valid = TRUE;
	}
	else if (imeiLen == 15)
	{
		// the last digits is check digit. compute the check digit an compare it the the given check digit. if there is a mtach IMEI is valid.
		// The last number of the IMEI is a check digit calculated using the Luhn algorithm.(ISO/IEC 7812).
		// See GSM 02.16 / 3GPP 22.016

		/************************************************************
		CD (check digit = D) calclulation example
		TAC (Type Allocation Code) D14-D7, SNR (Serial Number) D6-D1

		D14	D13	D12	D11	D10	D9	D8	D7	D6	D5	D4	D3	D2	D1
		2	6	0	5	3	1	7	9	3	1	1	3	8	3

		Step 1 : multiply each odd digit by two
		------
		2	6	0	5	3	1	7	9	3	1	1	3	8	3
			x2		x2		x2		x2		x2		x2		x2
			12		10		2		18		2		6		6

		Step 2 : sum all multiplications results
		------
		2+	1+2	0+	1+0	3+	2+	7+	1+8	3+	2+	1+	6+	8+	6  = 53

		Step 3 : substruct result of last step from the closest 10's multiple
		------
		CD=60-53=7
		*************************************************************/
		// Step 1 - multiply each odd digit by two
		for(i = 0; i<14; i++)
		{
			if(i%2)//double odd digit
			{
				c = imei[i];
				uiIMEIarr[i] = 2*atoi(&c);
			}
			else
			{
				c = imei[i];
				uiIMEIarr[i] =  atoi(&c);
			}
		}

		// Step 2 - sum IMEI array
		for(i = 0; i<14; i++)
		{
			if(uiIMEIarr[i] >= 10)
			{
				uiIMEIarr[i] = uiIMEIarr[i]/10 + uiIMEIarr[i]%10;
			}
			uiIMEIsum += uiIMEIarr[i];
		}
		// Step 3
		if(uiIMEIsum%10) //assign value to the CD
			uiCD = 10 - uiIMEIsum%10;

		c = imei[14];
		if (uiCD == atoi(&c))
		{
			// IMEI is valid. Zero the last 2 digits.
			imei[14] = imei[15] = 0;
			valid = TRUE;
		}
	}
	else if (imeiLen == 16)
	{
		// this is ok.
		valid = TRUE;
	}
	// note that I ptobably have a problem here with the null character at the end. what to do? TBD
	return valid;
}
/************************************************************************************
 * F@: my_basename - Extract file name from full file path
 *
 ************************************************************************************/
char * my_basename(char * full_path_file_name)
{
	char *file_name = NULL;

	file_name = strrchr(full_path_file_name,'/');
	if (file_name == NULL)
		file_name  = full_path_file_name;
	else
		file_name++;

	return file_name;
}

/************************************************************************************
 * F@: GetDate - Convert input date format from 9 ASCII string format (ex. 05JUN2010)
 * 				to C time format
 *  The function expect that the input date format is 9 ascii characters
 * where first 2 characters are day in month - range 0-31
 * 		next 3 characteres are month initials in upper case only
 * ("JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC")
 * last 4 characters are the year.
 ************************************************************************************/
UINT32	GetDate(CHAR *StrDate)
{
	struct tm *time_m;
	time_t ctime;
	int date_y,date_m,date_d;
	int i;
	char tmpstr[5];
	int rc;
	char *month[12] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};

	// convert first 2 char to day in month
	memcpy(tmpstr,(char *)StrDate,2);
	tmpstr[2]=0;
	date_d = atoi(tmpstr);

	// convert next 3 char to month
	memcpy(tmpstr,(char *)&StrDate[2],3);
	tmpstr[3] = 0;
	for (i=0; i<12; i++)
	{
		rc = strcmp(month[i], tmpstr);
		if (rc == 0)
			break;
	}
	date_m = i;
	// convert last 4 char to year
	date_y = atoi((char *)&StrDate[5]);
    
	time(&ctime);
	time_m = localtime ( &ctime );
	// Fill in time_t sturture with input date.
	time_m->tm_year = date_y - 1900;
	time_m->tm_mon = date_m;
	time_m->tm_mday = date_d;

	time_m->tm_hour = 0;
	time_m->tm_min = 0;
	time_m->tm_sec = 0;
	time_m->tm_isdst = 0;
	//time_m->tm_wday
	//time_m->tm_yday
	ctime = mktime(time_m);
	return (ctime);
}

int simulate_key_event(int code)
{
	int ret = 0;
	int fd = 0;
	struct input_event event;

	fd = open(KEYPAD_FILE, O_RDWR);
	if(fd <= 0)
	{
		DBGMSG("%s: error open keyboard\n", __FUNCTION__);
		return -1;
	}

	event.type = EV_KEY;
	event.code = code;
	event.value = KEYDOWN;
	gettimeofday(&event.time, 0);
	if(write(fd, &event, sizeof(event)) < 0)
	{
		DBGMSG("%s: simulate key event error\n", __FUNCTION__);
		ret = 1;
	}


	event.type = EV_SYN;
	event.code = SYN_REPORT;
	event.value = 0;
	gettimeofday(&event.time, 0);
	if(write(fd, &event, sizeof(event)) < 0)
	{
		DBGMSG("%s: simulate key event error\n", __FUNCTION__);
		ret = 1;
	}


	event.type = EV_KEY;
	event.code = code;
	event.value = KEYUP;
	gettimeofday(&event.time, 0);
	if(write(fd, &event, sizeof(event)) < 0)
	{
		DBGMSG("%s: simulate key event error\n", __FUNCTION__);
		ret = 1;
	}


	event.type = EV_SYN;
	event.code = SYN_REPORT;
	event.value = 0;
	gettimeofday(&event.time, 0);
	if(write(fd, &event, sizeof(event)) < 0)
	{
		DBGMSG("%s: simulate key event error\n", __FUNCTION__);
	}

	close(fd);
	return ret;
}

/* return value:
 *    > 0 v1 is newer than v2
 *    = 0 v1 is same to v2
 *    < 0 v1 is older than v2
 */
int version_compare(const CHAR *v1, const CHAR *v2)
{
	return strcmp((const char*)v1, (const char*)v2);
}

/************************************************************************************
 * F@: SysSleep - GLOBAL API for GCF AT*SYSSLEEP command
 *
 ************************************************************************************/
RETURNCODE_T  SysSleep(            const utlAtParameterOp_T        op,
		const char                      *command_name_p,
		const utlAtParameterValue_P2c   parameter_values_p,
		const size_t                    num_parameters,
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

	/*
	**  Check the format of the request.
	*/
	switch (op)
	{
	case TEL_EXT_ACTION_CMD:                /* AT*SYSSLEEP */
	{
		if(InProduction_Mode())
		{
			system(RUN_SLEEP_SCRIPT);
		}
		else
		{
			/* send AT+CFUN=0 to cp */
			system(SEND_CFUN0);
			/* send end key to driver */
			simulate_key_event(KEY_END);
		}
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
		break;
	}

	case TEL_EXT_TEST_CMD:            /* AT*SYSSLEEP=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"*SYSSLEEP\r\n");
		break;
	}

	case TEL_EXT_GET_CMD:              /* AT*SYSSLEEP?  */
	case TEL_EXT_SET_CMD:              /* AT*SYSSLEEP= */
	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
		break;
	}
	}


	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);
}


/************************************************************************************
 * F@: AtMrdImei - GLOBAL API for GCF AT*MRD_IMEI command
 *
 ************************************************************************************/
RETURNCODE_T  AtMrdImei(            const utlAtParameterOp_T        op,
		const char                      *command_name_p,
		const utlAtParameterValue_P2c   parameter_values_p,
		const size_t                    num_parameters,
		const char                      *info_text_p,
		unsigned int                    *xid_p,
		void                            *arg_p)
{
	UINT32         atHandle = MAKE_AT_HANDLE(* (TelAtParserID *) arg_p);

	CHAR aString[2];
	INT16 aStringLen;
	CHAR Version[MAX_VERSION_LEN+1] = {0};
	INT16 VersionLen;
	CHAR Date[MAX_DATE_LEN+1] = {0};
	INT16 DateLen;
	CHAR imei[MAX_IMEI_LEN+1] = {0};
	INT16 imeiLen;
	MRDErrorCodes mrdrc;
	char	Buf[500];
	char  Datebuf[25];
	FILE *ImeiNumFile = NULL;
	UINT32	ver = 0;
	UINT32	date = 0;
	IMEIExtendedData imei_struct;
	memset(&imei_struct, 0x0, sizeof(imei_struct));

	UNUSEDPARAM(arg_p);
	UNUSEDPARAM(info_text_p);
	UNUSEDPARAM(num_parameters);
	UNUSEDPARAM(command_name_p);

	MRDInit();

    *xid_p = atHandle;
    DBGMSG("%s: reqHandle = %d.\n", __FUNCTION__, atHandle);
    
	switch( op )
	{
		case TEL_EXT_SET_CMD:
		{
			// First check if we are in production mode. this command is permitted only during production mode.
			if (InProduction_Mode())
			{
				//Read manadatory parameters.
				if (getExtString(parameter_values_p, 0, aString, 1, &aStringLen, NULL) == TRUE )
				{
					if ( !strcmp((char *)aString,"w") || !strcmp((char *)aString,"W"))
					{
						// This is write request
						// get write parameters
						if (getExtString(parameter_values_p, 1, Version, MAX_VERSION_LEN, &VersionLen, NULL) == TRUE )
						{
						if (getExtString(parameter_values_p, 2, Date, MAX_DATE_LEN, &DateLen, NULL) == TRUE )
						{
						if (getExtString(parameter_values_p, 3, imei, MAX_IMEI_LEN, &imeiLen, NULL) == TRUE )
						{   
							if (imeiLen)
							{
								//validate IMEI number
								if (ValidImei(imei, imeiLen))
								{
									// Fill the imei stucture
									memcpy(&imei_struct.DeycrptNumber, imei, MAX_IMEI_LEN);
									strcpy((char *)imei_struct.UserData,USER_DATA);
									memset(&imei_struct.EncryptNumber, 0, MAX_IMEI_LEN);
									memset(&imei_struct.UserKey, 0, IMEI_USER_KEY_SIZE*4);
									memset(&imei_struct.SubKey, 0, IMEI_SUB_KEY_SIZE*4);
									imei_struct.ImeiEncrypt = IMEI_IS_NOT_ENC;
									// save IMEI in a temporary file.
									ImeiNumFile = fopen(IMEI_NUM_FILE_FULL_NAME,"w");
									if (ImeiNumFile!=NULL)
									{
										fwrite (&imei_struct , 1 , sizeof(imei_struct) , ImeiNumFile );
										fclose (ImeiNumFile);
									}
									//Convert Date to C time format.
									if(DateLen)
										date = GetDate(Date);
									else
										date = 0;
									// now call mrd interface to write file.
									memcpy(&ver, Version, sizeof(ver));
									mrdrc = MRDFileAdd(IMEI_NUM_FILE_NAME, 0, MRD_IMEI_TYPE, ver, date, IMEI_NUM_FILE_FULL_NAME);
									DBGMSG("%s: MRDFileAdd return code: %d\n", __FUNCTION__, mrdrc);
									switch (mrdrc)
									{
										case MRD_NO_ERROR :
										{
											system("sync");
											ATRESP( atHandle,ATCI_RESULT_CODE_OK,0,0);
											break;
										}

										case MRD_FILE_ALREADY_EXISTS_ERROR :
										{
											ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_FILE_EXIST_ERROR,NULL);
											break;
										}

										default:
										{
											ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_INVALID_PARAM,NULL);
											break;
										}
									}
								}//if (ImeiIsValid(imei))
								else
								{
									ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_INVALID_PARAM,NULL);
								}
							} //(VersionLen && DateLen && imeiLen)
							else
							{
						 		ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_INVALID_PARAM,NULL);
							}
						}
						}
						}
						else
						{
						 	ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_INVALID_PARAM,NULL);
						}
					} // write

					if ( !strcmp((char *)aString,"d") || !strcmp((char *)aString,"D"))
					{
						// This is a delete request
						// call mrd interface to delete file.
						mrdrc = MRDFileRemove(IMEI_NUM_FILE_NAME, MRD_IMEI_TYPE);
						DBGMSG("%s: MRDFileRemove return code: %d\n", __FUNCTION__, mrdrc);
						switch (mrdrc)
						{
							case MRD_FILE_NOT_FOUND_ERROR :
							{
								ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_NOT_FOUND,NULL);
								break;
							}
							case MRD_NO_ERROR :
							{
								ATRESP( atHandle,ATCI_RESULT_CODE_OK,0,0);
								break;
							}
							default:
							{
								ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_INVALID_PARAM,NULL);
								break;
							}
						}
					} // delete

					if ( !strcmp((char *)aString,"r")|| !strcmp((char *)aString,"R"))
					{
						// This is a read request with version and date
						// call mrd interface to read the file.
						if (MRDFileRead(IMEI_NUM_FILE_NAME, MRD_IMEI_TYPE, 0, &ver, &date, IMEI_NUM_FILE_FULL_NAME) == MRD_NO_ERROR)
						{
							memcpy(Version, &ver, sizeof(ver));
							ImeiNumFile = fopen(IMEI_NUM_FILE_FULL_NAME,"r");
							if (ImeiNumFile!=NULL)
							{
								fread (&imei_struct, sizeof(imei_struct), 1,ImeiNumFile);
								fclose (ImeiNumFile);
							}
							memcpy(imei, &imei_struct.DeycrptNumber, MAX_IMEI_LEN);
							strncpy(Datebuf,ctime((time_t *)&date),24);//ignore '\n'
							Datebuf[24] = '\0';
							sprintf(Buf,"*MRD_IMEI:%s,%s,%s\n\r", Version, Datebuf, imei);
							ATRESP( atHandle,ATCI_RESULT_CODE_OK,0,Buf);
						}
						else
							ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_INVALID_PARAM,NULL);
					} // read

					else
				 		// unsupported command
						   ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_INVALID_PARAM,NULL);
				}
			}
			else //InProduction_Mode
				  ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_INVALID_PARAM,NULL);

			break;
		}

		case TEL_EXT_GET_CMD:      /* AT*MRD_IMEI? */
		{
			// This is a read request
			// call mrd interface to read the file.
			if (MRDFileRead(IMEI_NUM_FILE_NAME, MRD_IMEI_TYPE, 0, &ver, &date, IMEI_NUM_FILE_FULL_NAME) == MRD_NO_ERROR)
			{
				memcpy(Version, &ver, sizeof(ver));
				ImeiNumFile = fopen(IMEI_NUM_FILE_FULL_NAME,"r");
			   	if (ImeiNumFile!=NULL)
			   	{
			   		fread (&imei_struct, sizeof(imei_struct), 1,ImeiNumFile);
			   		fclose (ImeiNumFile);
			   	}

				memcpy(imei, &imei_struct.DeycrptNumber, MAX_IMEI_LEN);
			   	sprintf(Buf,"*MRD_IMEI:%s\n\r", imei);
			   	ATRESP( atHandle,ATCI_RESULT_CODE_OK,0,Buf);
			}
			else
				ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_INVALID_PARAM,NULL);

			break;
		}

		default:
		{
			//ret = OPER_NOT_SUPPORTED;
			ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_INVALID_PARAM,NULL);
			break;
		}
	}

	MRDClose();
	/* handle the return value */
	//rc = HANDLE_RETURN_VALUE(ret);
	return(utlSUCCESS);
}


/************************************************************************************
 * F@: AtMrdCdf - GLOBAL API for GCF AT*MRD_CDF command
 *
 ************************************************************************************/
RETURNCODE_T  AtMrdCdf(            const utlAtParameterOp_T        op,
		const char                      *command_name_p,
		const utlAtParameterValue_P2c   parameter_values_p,
		const size_t                    num_parameters,
		const char                      *info_text_p,
		unsigned int                    *xid_p,
		void                            *arg_p)
{  
    UINT32  atHandle = MAKE_AT_HANDLE(* (TelAtParserID *) arg_p);
    
	CHAR aString[2];
	INT16 aStringLen;
	CHAR FileName[MAX_FILE_NAME_LEN+1];
	INT16	FileNameLen;
	CHAR Version[MAX_VERSION_LEN+1] = {0};
	INT16 VersionLen;
	CHAR Date[MAX_DATE_LEN+1] = {0};
	INT16 DateLen;
	char fn[128 + MAX_FILE_NAME_LEN] = {0};
	MRDErrorCodes mrdrc;
	char	Buf[500];
	char    Datebuf[25];
	UINT32	date = 0;
	UINT32	ver = 0;
	const char *nvm_root_dir = NULL;

	UNUSEDPARAM(arg_p);
	UNUSEDPARAM(info_text_p);
	UNUSEDPARAM(num_parameters);
	UNUSEDPARAM(command_name_p);
    
	MRDInit();

	nvm_root_dir = getenv(NVM_ROOT_DIR);
	if(nvm_root_dir)
	{
		int len;
		strcpy(fn, nvm_root_dir);
		len = strlen(fn);
		if(len && fn[len - 1] != '/')
		{
			fn[len++] = '/';
			fn[len] = '\0';
		}
	}

	*xid_p = atHandle;
    DBGMSG("%s: reqHandle = %d.\n", __FUNCTION__, atHandle);
    
	switch( op )
	{
		case TEL_EXT_SET_CMD:
		{
			// First check if we are in production mode. this command is permitted only during production mode.
			if (InProduction_Mode())
			{   
				//Read manadatory parameters.
				if (getExtString(parameter_values_p, 0, aString, 1, &aStringLen, NULL) == TRUE )
				{
					if (getExtString(parameter_values_p, 1, FileName, MAX_FILE_NAME_LEN, &FileNameLen, NULL) == TRUE )
					{
						    if ( !strcmp((char *)aString,"w")|| !strcmp((char *)aString,"W"))
						    {
							    // This is write request
							    // get write parameters
							    if (getExtString(parameter_values_p, 2, Version, MAX_VERSION_LEN, &VersionLen, NULL) == TRUE )
							    {
							        if (getExtString(parameter_values_p, 3, Date, MAX_DATE_LEN, &DateLen, NULL) == TRUE )
							        {
								        CHAR existVersion[MAX_VERSION_LEN+1] = {0};
								        UINT32 existVer = 0;
								        UINT32 existDate = 0;

								        //Convert Date to C time format.
										if(DateLen)
											date = GetDate(Date);
										else
											date = 0;
								        // now call mrd interface to write file.
								        // concat /userfs/Linux/Marvell/NVM/ with file name to have full path name
								        strcat(fn,(char *)FileName);
								        memcpy(&ver, Version, sizeof(ver));

								        // before writing to MRD, compare the
								        // version number first
								        if (MRDFileRead((char *)FileName, MRD_CDF_TYPE, 0, &existVer, &existDate, NULL) == MRD_NO_ERROR)
								        {
								            int ver_ret;
								            memcpy(existVersion, &existVer, sizeof(existVer));
								            ver_ret = version_compare(Version, existVersion);
								            if(ver_ret > 0)
								            {
								                // newer than exist file
								                // erase exist file
								                MRDFileRemove((char *)FileName, MRD_CDF_TYPE);
								            }
								            else if(ver_ret == 0)
								            {
								                // same to exist file
								                // return file already exist error
								                ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR, CME_FILE_WITH_SAME_VERSION_EXIST_ERROR, NULL);
								                break;
								            }
								            else
								            {
								                // older than exist file
								                // return file already exist error
								                ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR, CME_FILE_WITH_NEWER_VERSION_EXIST_ERROR, NULL);
								                break;
								            }
								        }

								        mrdrc = MRDFileAdd((char *)FileName, 0, MRD_CDF_TYPE, ver, date, fn);
								        DBGMSG("%s: MRDFileAdd return code: %d\n", __FUNCTION__, mrdrc);
								        switch (mrdrc)
								        {
									        case MRD_NO_ERROR :
									        {
										        system("sync");
									            ATRESP(atHandle, ATCI_RESULT_CODE_OK, 0, 0);
										        break;
									        }

									        case MRD_FILE_ALREADY_EXISTS_ERROR :
									        {
										        ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR, CME_FILE_EXIST_ERROR, NULL);
										        break;
									        }

									        case MRD_FILE_NOT_FOUND_ERROR :
									        {
										        ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_NOT_FOUND, NULL);
										        break;
									        }

									        default:
									        {
										        ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_INVALID_PARAM, NULL);
										        break;
									        }
								        }
							    }
							    else
							    {
							         ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_INVALID_PARAM,NULL);
								}
							}
							else
							{
							    ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_INVALID_PARAM,NULL);
							}
						}// write

						else if  ( !strcmp((char *)aString,"d")|| !strcmp((char *)aString,"D"))
						{
							// This is a delete request
							// call mrd interface to delete file.
							mrdrc = MRDFileRemove((char *)FileName, MRD_CDF_TYPE);
							DBGMSG("%s: MRDFileRemove return code: %d\n", __FUNCTION__, mrdrc);
							switch (mrdrc)
							{
								case MRD_FILE_NOT_FOUND_ERROR :
								{
									ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR, CME_NOT_FOUND, NULL);
									break;
								}
								case MRD_NO_ERROR :
								{
									ATRESP( atHandle,ATCI_RESULT_CODE_OK,0,0);
									break;
								}
								default:
								{
									ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
									break;
								}
							}
						} // delete

						else if ( !strcmp((char *)aString,"q")|| !strcmp((char *)aString,"Q"))
						{
							// This is a query request
							// call mrd interface to check if file exist in MRD.
							if (MRDFileRead((char *)FileName, MRD_CDF_TYPE, 0, &ver, &date, NULL) == MRD_NO_ERROR)
							{
								memcpy(Version, &ver, sizeof(ver));
								strncpy(Datebuf,ctime((time_t *)&date),24);//ignore '\n'
								Datebuf[24] = '\0';
								sprintf(Buf,"*MRD_CDF:%s,%s,%s\n\r", Version, Datebuf, FileName);
								ATRESP( atHandle,ATCI_RESULT_CODE_OK,0, Buf);
							}
							else//ERROR: no such file
							{
								ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
							}
						} // query

						else//unsupported command
						{
							ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
						}
					}
					else
					{
					    ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
					}
				}
				else
				{
				    ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
				}
			}
			else //InProduction_Mode
			{
				ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_INVALID_PARAM,NULL);
			}
			break;
		}

		case TEL_EXT_GET_CMD:      /* AT*MRD_CDF? */
		{
			 ATRESP( atHandle,ATCI_RESULT_CODE_OK,0,0);
			 break;
		}

		default:
		{
			//OPER_NOT_SUPPORTED;
		    ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
			break;
		}
	}

	MRDClose();
   	return(utlSUCCESS);
}

/************************************************************************************
 * F@: AtMrdSn - GLOBAL API for GCF AT*MRD_SN command
 *
 ************************************************************************************/
RETURNCODE_T  AtMrdSn(            const utlAtParameterOp_T        op,
		const char                      *command_name_p,
		const utlAtParameterValue_P2c   parameter_values_p,
		const size_t                    num_parameters,
		const char                      *info_text_p,
		unsigned int                    *xid_p,
		void                            *arg_p)
{
	 UINT32    atHandle = MAKE_AT_HANDLE(* (TelAtParserID *) arg_p);
	  
	CHAR aString[2];
	INT16 aStringLen;
	CHAR Version[MAX_VERSION_LEN+1] = {0};
	INT16 VersionLen;
	CHAR Date[MAX_DATE_LEN+1] = {0};
	INT16 DateLen;
	CHAR serial[MAX_SERIAL_LEN+1];
	INT16 serialLen;
	MRDErrorCodes mrdrc;
	char	Buf[500];
	char Datebuf[25];
	FILE *SerialNumFile = NULL;
	UINT32	date = 0;
	UINT32  ver = 0;

	UNUSEDPARAM(arg_p);
	UNUSEDPARAM(info_text_p);
	UNUSEDPARAM(num_parameters);
	UNUSEDPARAM(command_name_p);
    
	MRDInit();

    *xid_p = atHandle;
    DBGMSG("%s: reqHandle = %d.\n", __FUNCTION__, atHandle);
    
	switch( op )
	{
		case TEL_EXT_SET_CMD:
		{
			// First check if we are in production mode. this command is permitted only during production mode.
			if (InProduction_Mode())
			{
				//Read manadatory parameters.
				if (getExtString(parameter_values_p, 0, aString, 1, &aStringLen, NULL) == TRUE )
				{
					if ( !strcmp((char *)aString,"w") || !strcmp((char *)aString,"W"))
					{
						// This is write request
						// get write parameters
						if (getExtString(parameter_values_p, 1, Version, MAX_VERSION_LEN, &VersionLen, NULL) == TRUE )
						{
						if (getExtString(parameter_values_p, 2, Date, MAX_DATE_LEN, &DateLen, NULL) == TRUE )
						{
						if (getExtString(parameter_values_p, 3, serial, MAX_SERIAL_LEN, &serialLen, NULL) == TRUE )
						{
							if (serialLen)
							{
								// save serial name in a temporary file.
								SerialNumFile = fopen(SERIAL_NUM_FILE_FULL_NAME,"w");
								if (SerialNumFile!=NULL)
								{
									fputs ((char *)serial,SerialNumFile);
									fclose (SerialNumFile);
								}

								//Convert Date to C time format.
								if(DateLen)
									date = GetDate(Date);
								else
									date = 0;
								memcpy(&ver, Version, sizeof(ver));
								// now call mrd interface to write file.
								mrdrc = MRDFileAdd(SERIAL_NUM_FILE_NAME, 0, MRD_SN_TYPE, ver, date, SERIAL_NUM_FILE_FULL_NAME);
								DBGMSG("%s: MRDFileAdd return code: %d\n", __FUNCTION__, mrdrc);
								switch (mrdrc)
								{
									case MRD_NO_ERROR :
									{
										system("sync");
										ATRESP( atHandle,ATCI_RESULT_CODE_OK,0,0);
										break;
									}

									case MRD_FILE_ALREADY_EXISTS_ERROR :
									{
										ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_FILE_EXIST_ERROR, NULL);
										break;
									}

									default:
									{
										ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_INVALID_PARAM,NULL);
										break;
									}
								}
							} //(VersionLen && DateLen)
							else
							{
						 		ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_INVALID_PARAM,NULL);
							}
						}
						else
						{
						 	ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_INVALID_PARAM,NULL);
						}
						}
						else
						{
						 	ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_INVALID_PARAM,NULL);
						}
						}
						else
						{
						 	ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_INVALID_PARAM,NULL);
						}
					} // write

					if ( !strcmp((char *)aString,"d") || !strcmp((char *)aString,"D"))
					{
						// This is a delete request
						// call mrd interface to delete file.
						mrdrc = MRDFileRemove(SERIAL_NUM_FILE_NAME, MRD_SN_TYPE);
						DBGMSG("%s: MRDFileRemove return code: %d\n", __FUNCTION__, mrdrc);
						switch (mrdrc)
						{
							case MRD_FILE_NOT_FOUND_ERROR :
							{
								ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_NOT_FOUND, NULL);
								break;
							}
							case MRD_NO_ERROR :
							{
								ATRESP( atHandle,ATCI_RESULT_CODE_OK,0, 0);
								break;
							}
							default:
							{
								ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_INVALID_PARAM,NULL);
								break;
							}
						}
					} // delete

					if ( !strcmp((char *)aString,"r") || !strcmp((char *)aString,"R"))
					{
						// This is a read request with version and date
						// call mrd interface to read the file.
						if (MRDFileRead(SERIAL_NUM_FILE_NAME, MRD_SN_TYPE, 0, &ver, &date, SERIAL_NUM_FILE_FULL_NAME) == MRD_NO_ERROR)
						{
							memcpy(Version, &ver, sizeof(ver));
							SerialNumFile = fopen(SERIAL_NUM_FILE_FULL_NAME,"r");
								if (SerialNumFile!=NULL)
								{
									fgets ((char *)serial,(MAX_SERIAL_LEN+1),SerialNumFile);
									fclose (SerialNumFile);
								}
							strncpy(Datebuf,ctime((time_t *)&date),24);//ignore '\n'
							Datebuf[24] = '\0';
							sprintf(Buf,"*MRD_SN:%s,%s,%s\n\r", Version, Datebuf, serial);
							ATRESP( atHandle,ATCI_RESULT_CODE_OK,0, Buf);
						}
						else// Missing SN
							ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_INVALID_PARAM,NULL);
					} // read

					else
				 		// unsupported command
						ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
				}
				else
					ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
			}
			else //InProduction_Mode
				ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);

			break;
		}

		case TEL_EXT_GET_CMD:      /* AT*MRD_SN? */
		{
			// This is a read request
			// call mrd interface to read the file.
			if (MRDFileRead(SERIAL_NUM_FILE_NAME, MRD_SN_TYPE, 0, &ver, &date, SERIAL_NUM_FILE_FULL_NAME) == MRD_NO_ERROR)
			{
				SerialNumFile = fopen(SERIAL_NUM_FILE_FULL_NAME,"r");
			   	if (SerialNumFile!=NULL)
			   	{
			   		fgets ((char *)serial,(MAX_SERIAL_LEN+1),SerialNumFile);
			   		fclose (SerialNumFile);
			   	}
			   	 sprintf(Buf,"*MRD_SN:%s\n\r", serial);
			   	 ATRESP( atHandle,ATCI_RESULT_CODE_OK, 0, Buf);
			}
			else
				 ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);

			break;
		}

		default:
		{	//OPER_NOT_SUPPORTED;
			ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
			break;
		}
	}

	MRDClose();
   	return(utlSUCCESS);
}


/**************************************************************************************/
// Because the Confidentiality function must be multiple of 16 bytes, this function
// calculate the number of bytes needed to be align to a number which is multiple of 16.
// Input -- the data structure real size to be transfer to the Confidentiality function
/**************************************************************************************/
UINT8 MEP_CalcPadding ( UINT32 size )
{
	UINT32 package , CalSize;


	package = (size >> 4);    // (size / 16);
	CalSize = (package * 16); // The Confidentiality nust be multiple of 16 bytes

	if ( CalSize  == size )
		return 0;             // No padding is needed
	else
	{
		package++;
		CalSize = (package * 16);
		return (CalSize - size);
	}
}

/************************************************************************************
 * F@: AtMrdMep - GLOBAL API for GCF AT*MRD_MEP command
 *
 ************************************************************************************/
RETURNCODE_T  AtMrdMep(            const utlAtParameterOp_T        op,
		const char                      *command_name_p,
		const utlAtParameterValue_P2c   parameter_values_p,
		const size_t                    num_parameters,
		const char                      *info_text_p,
		unsigned int                    *xid_p,
		void                            *arg_p)
{
	UINT32  atHandle = MAKE_AT_HANDLE(* (TelAtParserID *) arg_p);
	
	CHAR aString[2];
	INT16 aStringLen;
	CHAR Version[MAX_VERSION_LEN+1] = {0};
	INT16 VersionLen;
	CHAR Date[MAX_DATE_LEN+1] = {0};
	INT16 DateLen;
	CHAR pw1[MAX_PASSWORD_LEN+1];
	INT16 pw1Len;
	CHAR pw2[MAX_PASSWORD_LEN+1];
	INT16 pw2Len;
	CHAR pw3[MAX_PASSWORD_LEN+1];
	INT16 pw3Len;
	CHAR pw4[MAX_PASSWORD_LEN+1];
	INT16 pw4Len;
	CHAR pw5[MAX_PASSWORD_LEN+1];
	INT16 pw5Len;
	CHAR pw6[MAX_PASSWORD_LEN+1];
	INT16 pw6Len;
	CHAR TrialLimit[MAX_PASSWORD_LEN+1];
	INT16 TrialLimitLen;
	MRDErrorCodes mrdrc;
	char	Buf[500];
	FILE *Mep1File = NULL;
	MEP_BLOCK_DATA mep1_struct;
	int mep_length;
	int padding;
	unsigned char *mep_data = NULL;

	UINT32	date = 0;
	UINT32	ver = 0;
	UINT32 existVer = 0;
	UINT32 existDate = 0;

	UNUSEDPARAM(arg_p);
	UNUSEDPARAM(info_text_p);
	UNUSEDPARAM(num_parameters);
	UNUSEDPARAM(command_name_p);

	MRDInit();

	*xid_p = atHandle;
    DBGMSG("%s: reqHandle = %d.\n", __FUNCTION__, atHandle);
    
	switch( op )
	{
		case TEL_EXT_SET_CMD:
		{
			// First check if we are in production mode. this command is permitted only during production mode.
			if (InProduction_Mode())
			{
				//Read manadatory parameters.
				if (getExtString(parameter_values_p, 0, aString, 1, &aStringLen, NULL) == TRUE )
				{
					if ( !strcmp((char *)aString,"w") || !strcmp((char *)aString,"W"))
					{
						// This is write request
						// get write parameters
						if (getExtString(parameter_values_p, 1, Version, MAX_VERSION_LEN, &VersionLen, NULL) == TRUE ) {
						if (getExtString(parameter_values_p, 2, Date, MAX_DATE_LEN, &DateLen, NULL) == TRUE )          {
						if (getExtString(parameter_values_p, 3, pw1, MAX_PASSWORD_LEN, &pw1Len, NULL) == TRUE )        {
						if (getExtString(parameter_values_p, 4, pw2, MAX_PASSWORD_LEN, &pw2Len, NULL) == TRUE )        {
						if (getExtString(parameter_values_p, 5, pw3, MAX_PASSWORD_LEN, &pw3Len, NULL) == TRUE )        {
						if (getExtString(parameter_values_p, 6, pw4, MAX_PASSWORD_LEN, &pw4Len, NULL) == TRUE )        {
						if (getExtString(parameter_values_p, 7, pw5, MAX_PASSWORD_LEN, &pw5Len, NULL) == TRUE )        {
						if (getExtString(parameter_values_p, 8, pw6, MAX_PASSWORD_LEN, &pw6Len, NULL) == TRUE )        {
						if (getExtString(parameter_values_p, 9, TrialLimit, MAX_PASSWORD_LEN, &TrialLimitLen, NULL) == TRUE )
						{
							if (pw1Len && pw2Len && pw3Len && pw4Len && pw5Len && pw6Len && TrialLimitLen)
							{
								memset(&mep1_struct, 0, sizeof(mep1_struct));
								// Fill the MEP1 stucture
								memcpy(mep1_struct.CatPsw[0].number, pw1, pw1Len);
								mep1_struct.CatPsw[0].length = pw1Len;
								memcpy(mep1_struct.CatPsw[1].number, pw2, pw2Len);
								mep1_struct.CatPsw[1].length = pw2Len;
								memcpy(mep1_struct.CatPsw[2].number, pw3, pw3Len);
								mep1_struct.CatPsw[2].length = pw3Len;
								memcpy(mep1_struct.CatPsw[3].number, pw4, pw4Len);
								mep1_struct.CatPsw[3].length = pw4Len;
								memcpy(mep1_struct.CatPsw[4].number, pw5, pw5Len);
								mep1_struct.CatPsw[4].length = pw5Len;
								memcpy(mep1_struct.UnblockPsw.number, pw6, pw6Len);
								mep1_struct.UnblockPsw.length = pw6Len;
								mep1_struct.TrialLimit = atoi((char *)TrialLimit);

								// NOTE: from the CP side code, the signature need some paddings
								// so the whole length of mep file is
								// mep fix size + signature size + padding size + 1 byte (DataIsEncrypted)
								padding = MEP_CalcPadding(MEP_FIX_SIZE);
								mep_length = MEP_FIX_SIZE + MEP_SIGNATURE_SIZE + padding + 1;

								mep_data = malloc(mep_length);
								if(!mep_data)
								{
									DBGMSG("%s: alloc memory error\n", __FUNCTION__);
									ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
									break;
								}
								memset(mep_data, 0, mep_length);

								// read out exist MEP file
								if (MRDFileRead(MEP1_FILE_NAME, MRD_MEP_TYPE, 0, &existVer, &existDate, MEP1_FILE_FULL_NAME) == MRD_NO_ERROR)
								{
									Mep1File = fopen(MEP1_FILE_FULL_NAME,"r");
									if (Mep1File!=NULL)
									{
										if(fseek(Mep1File, 0, SEEK_END) != 0)
										{
											fclose (Mep1File);
											free(mep_data);
											ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
											break;
										}
										if(ftell(Mep1File) != mep_length)
										{
											DBGMSG("%s: mep file length is not valid\n", __FUNCTION__);
											fclose (Mep1File);
											free(mep_data);
											ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
											break;
										}
										rewind(Mep1File);
										fread(mep_data, 1, mep_length, Mep1File);
										fclose(Mep1File);
									}
									else
									{
										free(mep_data);
										ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
										break;
									}

									// erase exist file
									MRDFileRemove(MEP1_FILE_NAME, MRD_MEP_TYPE);

									// now we only support unencrypted mode
									// TODO: add encrypted mode support

									// NOTE: according alignment requirement of signature the real offset of DataIsEncryped is
									// MEP_FIX size + signature size + padding size
									// the meanings of ambiguous macros MEP_ENCRYPT and MEP_DECRYPT
									//   MEP_ENCRYPT: 0, unencrypted mode
									//   MEP_DECRYPT: 1, encrypted mode
									// to simplify things, we use TRUE or FALSE here
									if(*(mep_data + MEP_FIX_SIZE + MEP_SIGNATURE_SIZE + padding) == FALSE)
									{
										((MEP_FIX*)mep_data)->blocking = mep1_struct;
									}
								}
								else
								{
									free(mep_data);
									ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
									break;
								}

								// save MEP in a temporary file.
								Mep1File = fopen(MEP1_FILE_FULL_NAME,"w");
								if (Mep1File!=NULL)
								{
									fwrite (mep_data, 1, mep_length, Mep1File);
							 		fclose (Mep1File);
								}
								free(mep_data);
								//Convert Date to C time format.
								if(DateLen)
									date = GetDate(Date);
								else
									date = 0;
								memcpy(&ver, Version, sizeof(ver));
								// now call mrd interface to write file.
								mrdrc = MRDFileAdd(MEP1_FILE_NAME, 0, MRD_MEP_TYPE, ver, date, MEP1_FILE_FULL_NAME);
								DBGMSG("%s: MRDFileAdd return code: %d\n", __FUNCTION__, mrdrc);
								switch (mrdrc)
								{
									case MRD_NO_ERROR :
									{
										system("sync");
										ATRESP( atHandle,ATCI_RESULT_CODE_OK, 0, 0);
										break;
									}

									case MRD_FILE_ALREADY_EXISTS_ERROR :
									{
										ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_FILE_EXIST_ERROR, NULL);
										break;
									}

									case MRD_FILE_NOT_FOUND_ERROR :
									{//Missing MEP file
										ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_NOT_FOUND, NULL);
										break;
									}

									default:
									{
										ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
										break;
									}
								}
							} //(VersionLen && DateLen)
							else
							{//ERROR - missing parameters
						 		ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
							}
						}
						else
						{//ERROR - unexpected error in parameters
						 	ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
						}
					}}}}}}}}}// write

					if ( !strcmp((char *)aString,"d") || !strcmp((char *)aString,"D"))
					{
						// This is a delete request
						// call mrd interface to delete file.
						mrdrc = MRDFileRemove(MEP1_FILE_NAME, MRD_MEP_TYPE);
						switch (mrdrc)
						{
							case MRD_FILE_NOT_FOUND_ERROR :
							{//file not found
								ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR,CME_NOT_FOUND, NULL);
								break;
							}
							case MRD_NO_ERROR :
							{
								 ATRESP( atHandle,ATCI_RESULT_CODE_OK, 0, 0);
								break;
							}
							default:
							{
                                ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
								break;
							}
						}
					} // delete

					if ( !strcmp((char *)aString,"q") || !strcmp((char *)aString,"Q"))
					{
						// This is a query request with version and date
						// call mrd interface to read the file.
						if (MRDFileRead(MEP1_FILE_NAME, MRD_MEP_TYPE, 0, &ver, &date, MEP1_FILE_FULL_NAME) == MRD_NO_ERROR)
						{
							memcpy(Version, &ver, sizeof(ver));
							sprintf(Buf,"*MRD_MEP: %s,%s\n\r", Version, ctime((time_t *)&date));
						    ATRESP( atHandle,ATCI_RESULT_CODE_OK, 0, Buf);
						}
						else//Missing MEP
						    ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
					} // read

					else// unsupported command
						    ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
				}
			}
			else //InProduction_Mode
                  ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
			break;
		}

		case TEL_EXT_GET_CMD:      /* AT*MRD_MEP? */
		{
			ATRESP( atHandle,ATCI_RESULT_CODE_OK, 0, 0);
			break;
		}

		default:
		{
			ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
			break;
		}
	}

	/* handle the return value */
	//rc = HANDLE_RETURN_VALUE(ret, xid_p);
   //	return(rc);
	MRDClose();
   	return(utlSUCCESS);
}
/************************************************************************************
 * F@: AtGpsTest - GLOBAL API for AT*GPSTEST command
 *
 ************************************************************************************/
RETURNCODE_T  AtGpsTest(const utlAtParameterOp_T		op,
						const char						*command_name_p,
						const utlAtParameterValue_P2c	parameter_values_p,
						const size_t					num_parameters,
						const char						*info_text_p,
						unsigned int					*xid_p,
						void							*arg_p)
{
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);

	static void* dlHandle = NULL;
	static tSIRF_INT32 (*p_sirf_gps_test_mode)(LSM_SINT32, LSM_SINT32, LSM_CHAR*) = NULL;
	int gps_test_result;
	tSIRF_MSG_SSB_TEST_MODE_DATA test_mode_4_data;
	tSIRF_MSG_SSB_TEST_MODE_DATA_7 test_mode_7_data;

	char buf[128];

	UNUSEDPARAM(arg_p);
	UNUSEDPARAM(info_text_p);
	UNUSEDPARAM(num_parameters);
	UNUSEDPARAM(command_name_p);
	UNUSEDPARAM(test_mode_4_data);

	*xid_p = atHandle;

	switch( op )
	{
		case TEL_EXT_SET_CMD:			/* AT*GPSTEST= */
		{
			int attr, param1, param2;
			int ErrorFlag = 1;	// 0=OK, 1=Error

			// check if we are in production mode. this command is permitted only during production mode.
			// Temp disable the limitation for production mode.
			if (0)//(InProduction_Mode() == FALSE)
			{
				DBGMSG("target not in production mode\n");
				ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_ALLOWED, NULL);
			}
			else
			{
				// get the parameter values
				if ( getExtValue( parameter_values_p, 0, &attr, GPSTEST_ATTR_OPER, GPSTEST_ATTR_READ, GPSTEST_ATTR_OPER ) == TRUE )
				{
					if ( getExtValue( parameter_values_p, 1, &param1, GPSTEST_PAR1_DEACT, GPSTEST_PAR1_ACT, GPSTEST_PAR1_DEACT ) == TRUE )
					{
						if ( getExtValue( parameter_values_p, 2, &param2, GPSTEST_PAR2_MULTI_CHAN, GPSTEST_PAR2_SINGLE_CHAN, GPSTEST_PAR2_SINGLE_CHAN ) == TRUE )
						{
							ErrorFlag = 0;	// No error
						}
						else
						{
							DBGMSG("param2 is out of range\n");
							ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
						}
					}
					else
					{
						DBGMSG("param1 is out of range\r\n");
						ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
					}
				}
				else
				{
					DBGMSG("attr is out of range\r\n");
					ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
				}

				if (ErrorFlag == 1)
				{
					break;
				}

				// load the GPS library, if exists
				if (dlHandle == NULL)
				{
					dlHandle = dlopen(GPS_SHARED_OBJ_PATH, RTLD_NOW);
				}

				if (dlHandle == NULL)
				{
					DBGMSG("%s: dlopen failed: %s\n", __FUNCTION__, dlerror());
					DBGMSG("gps test not supported on current platform\n");
					ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
					ErrorFlag = 1;
					break;
				}
				DBGMSG("%s: dlHandle=0x%x\n", __FUNCTION__, (unsigned int) dlHandle);

				// get the GPS test symbol.
				p_sirf_gps_test_mode = (tSIRF_INT32 (*)(LSM_SINT32, LSM_SINT32, LSM_CHAR*)) dlsym(dlHandle, "sirf_gps_test_mode");
				if (p_sirf_gps_test_mode == NULL)
				{
					DBGMSG("%s: symbol 'sirf_gps_test_mode' was not found in %s\n", __FUNCTION__, GPS_SHARED_OBJ_PATH);
					DBGMSG("gps test not supported on current platform\n");
					ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
					ErrorFlag = 1;
					break;
				}

				// parse the parameters and dispatch the appropriate test
				if (attr == GPSTEST_ATTR_OPER)
				{
					if (param1 == GPSTEST_PAR1_DEACT)
					{
						DBGMSG("%s: deactivate\n", __FUNCTION__);
						p_sirf_gps_test_mode(GPS_SIRF_CMD_TEST_MODE_OFF, 1, NULL);
						sprintf(buf, "*GPSTEST:0\r\n");
						ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, buf);
					}
					else if (param1 == GPSTEST_PAR1_ACT)
					{
						if (param2 == GPSTEST_PAR2_MULTI_CHAN)
						{
							DBGMSG("multi channel not supported\n");
							ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
							ErrorFlag = 1;
						}
						else if (param2 == GPSTEST_PAR2_SINGLE_CHAN)
						{
							DBGMSG("%s: activate single channel\n", __FUNCTION__);
							p_sirf_gps_test_mode(GPS_SIRF_CMD_TEST_MODE_7, 1, NULL); // 1.57562 GHz
							sprintf(buf, "*GPSTEST:0\r\n");
							ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, buf);

						}
					}
				}
				else if (attr == GPSTEST_ATTR_READ)
				{
					if (param1 == GPSTEST_PAR1_READ_DEFAULT)
					{
						if (param2 == GPSTEST_PAR2_MULTI_CHAN)
						{
							DBGMSG("read multi channel not supported\n");
							ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
							ErrorFlag = 1;
						}
						else if (param2 == GPSTEST_PAR2_SINGLE_CHAN)
						{
							DBGMSG("%s: read single channel\n", __FUNCTION__);
							gps_test_result = p_sirf_gps_test_mode(GPS_SIRF_CMD_TEST_MODE_READ, 1, (LSM_CHAR *) &test_mode_7_data); // 1.57562 GHz
							if (gps_test_result == SIRF_SUCCESS)
							{
								sprintf(buf, "*GPSTEST:1,%ld,%02d\n\r", test_mode_7_data.spur1_frequency, test_mode_7_data.spur1_sig_to_noise);
								ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, buf);

							}
							else
							{
								DBGMSG("illegal test result\r\n");
								ATRESP(atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_UNKNOWN, NULL);
								ErrorFlag = 1;
							}
						}
					}
					else
					{
						DBGMSG("param1 is out of range\r\n");
						ATRESP( atHandle,ATCI_RESULT_CODE_CME_ERROR, CME_INVALID_PARAM, NULL);
						ErrorFlag = 1;
					}
				}
			}
			break;
		}
		case TEL_EXT_GET_CMD:		/* AT*GPSTEST? */
		{
			ATRESP( atHandle,ATCI_RESULT_CODE_OK,0,0);
			break;
		}
		default:
		{
			DBGMSG("ERROR - unsupported operation\n");
			ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
			break;
		}
	}

	return(utlSUCCESS);
}

extern TelAtpCtrl gAtpCtrl[];

static PROD_CTRL at_prod_ctrl;
static void process_atsrv_buf(const unsigned char *buf, int len);

void init_at_prod_conn(void)
{
	at_prod_ctrl.fd = -1;
	at_prod_ctrl.fp = &process_atsrv_buf;
	create_prod_conn_task(&at_prod_ctrl);
}

void start_diag(void)
{
	if(at_prod_ctrl.fd != -1)
	{
		unsigned long code = CODE_SWITCH_TO_DIAG;
		write(at_prod_ctrl.fd, &code, sizeof(code));
	}
}

extern int start_prod_at_cmd_srv(void);
extern int stop_prod_at_cmd_srv(int b_start_diag);

void process_atsrv_buf(const unsigned char *buf, int len)
{
	unsigned long code;
	if(len != 4)
	{
		return;
	}

	memcpy(&code, buf, sizeof(code));
	if(code == CODE_SWITCH_TO_AT_CMD_SRV)
	{
		if(gAtpCtrl[TEL_PROD_AT_CMD_ATP].bEnable == FALSE)
			start_prod_at_cmd_srv();
	}
}

/************************************************************************************
 * F@: DiagCommand - GLOBAL API for AT*RFC -command
 *
 */
RETURNCODE_T  DiagCommand(const utlAtParameterOp_T		op,
						const char						*command_name_p,
						const utlAtParameterValue_P2c	parameter_values_p,
						const size_t					num_parameters,
						const char						*info_text_p,
						unsigned int					*xid_p,
						void							*arg_p)
{
	UINT32 atHandle = MAKE_AT_HANDLE(*(TelAtParserID *)arg_p);
	RETURNCODE_T rc = INITIAL_RETURN_CODE;
	CiReturnCode ret = CIRC_FAIL;

	UNUSEDPARAM(arg_p);
	UNUSEDPARAM(info_text_p);
	UNUSEDPARAM(num_parameters);
	UNUSEDPARAM(command_name_p);
	UNUSEDPARAM(parameter_values_p);

	*xid_p = atHandle;
	DBGMSG("%s: atHandle = %d.\n", __FUNCTION__, atHandle);

	/*
	**  Check the format of the request.
	*/
	switch (op)
	{
	case TEL_EXT_ACTION_CMD:                /* AT*RFC */
	{
		if((get_bspflag_from_kernel_cmdline() == 2) && (GET_ATP_INDEX(GET_AT_HANDLE(atHandle ))== TEL_PROD_AT_CMD_ATP))
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, NULL);
			stop_prod_at_cmd_srv(1);
		}
		else
		{
			ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
		}
		break;
	}

	case TEL_EXT_TEST_CMD:            /* AT*RFC=? */
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_OK, 0, (char *)"*RFC\r\n");
		break;
	}

	case TEL_EXT_GET_CMD:              /* AT*RFC?  */
	case TEL_EXT_SET_CMD:              /* AT*RFC= */
	default:
	{
		ret = ATRESP( atHandle, ATCI_RESULT_CODE_CME_ERROR, CME_OPERATION_NOT_SUPPORTED, NULL);
		break;
	}
	}


	/* handle the return value */
	rc = HANDLE_RETURN_VALUE(ret);
	return(rc);

}

#endif //AT_PRODUCTION_CMNDS

