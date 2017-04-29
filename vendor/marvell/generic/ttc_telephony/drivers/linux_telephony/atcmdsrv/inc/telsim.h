/******************************************************************************

*(C) Copyright 2007~2009 Marvell International Ltd.

* All Rights Reserved

******************************************************************************
*  Filename: telSIM.h

*  Description: Process MM related AT commands
*
*  History:
*   May 31, 2008 - Johnny He, Creation of file
*   Oct 16, 2008 - Qiang XU, Modification for optimization
*
******************************************************************************/
#ifndef TELSIM_H
#define TELSIM_H

#include "ci_api_types.h"

typedef enum SimFileDirTag
{
	SIM_FILE_DIR_MF                  =   0x3f00,
	SIM_FILE_DIR_DF_MEXE             =   0x5f3c,
	SIM_FILE_DIR_DF_GRAPHICS         =   0x5f50,
	SIM_FILE_DIR_DF_SOLSA            =   0x5f70, /* Added by Michal Bukai*/
	SIM_FILE_DIR_DF_GSM              =   0x7f20,
	SIM_FILE_DIR_DF_DCS1800          =   0x7f21,
	SIM_FILE_DIR_DF_TELECOM          =   0x7f10,

	SIM_FILE_DIR_DF_ENS              =   0x7f66,
	SIM_FILE_DIR_DF_IRIDIUM          =   0x5f30,


	SIM_FILE_DIR_DF_PHONEBOOK        =   0x5f3a,
	SIM_FILE_DIR_DF_GSM_ACCESS       =   0x5f3b,
	SIM_FILE_DIR_ADF_USIM            =   0x7fff,

	SIM_FILE_DIR_INVALID             =   0x0000
}SimFileDir;

typedef enum SimEfFileIdTag
{
	SIM_EF_FILEIDARR_MF               =   0x2f06,
	SIM_EF_FILEIDDIR                  =   0x2f00,

	SIM_EF_FILEIDICCID                =   0x2fe2,
	SIM_EF_FILEIDPL                   =   0x2f05,
	SIM_EF_FILEIDARR_TELECOM          =   0x6f06,

	SIM_EF_FILEIDIMG                  =   0x4f20,
/* Start - Added by Michal Bukai*/
	SIM_EF_FILEIDSAI                  =   0x4f30,
	SIM_EF_FILEKCGPRS                 =   0x4f52,
	SIM_EF_FILECPBCCH                 =   0x4f63,
	SIM_EF_FILEINVSCAN                =   0x4f64,
	SIM_EF_FILEIDLP                   =   0x6f05,
	SIM_EF_FILEIDIMSI                 =   0x6f07,
	SIM_EF_FILEIDKC                   =   0x6f20,
	SIM_EF_FILEIDPLMN_SEL             =   0x6f30,
	SIM_EF_FILEIDHPLMN                =   0x6f31,
	SIM_EF_FILEIDACM_MAX              =   0x6f37,
	SIM_EF_FILEIDSST                  =   0x6f38,
	SIM_EF_FILEIDACM                  =   0x6f39,
	SIM_EF_FILEIDGID1                 =   0x6f3e,
	SIM_EF_FILEIDGID2                 =   0x6f3f,
	SIM_EF_FILEIDSPN                  =   0x6f46,
	SIM_EF_FILEIDPUCT                 =   0x6f41,
	SIM_EF_FILEIDCBMI                 =   0x6f45,
	SIM_EF_FILEIDBCCH                 =   0x6f74,
	SIM_EF_FILEIDACC                  =   0x6f78,
	SIM_EF_FILEIDFPLMN                =   0x6f7b,
	SIM_EF_FILEIDLOCI                 =   0x6f7e,
	SIM_EF_FILEIDAD                   =   0x6fad,
	SIM_EF_FILEIDPHASE                =   0x6fae,
	SIM_EF_FILEIDADN                  =   0x6f3a,
	SIM_EF_FILEIDFDN                  =   0x6f3b,
	SIM_EF_FILEIDSMS                  =   0x6f3c,
	SIM_EF_FILEIDCCP                  =   0x6f3d,
	SIM_EF_FILEIDMSISDN               =   0x6f40,
	SIM_EF_FILEIDSMSP                 =   0x6f42,
	SIM_EF_FILEIDSMSS                 =   0x6f43,
	SIM_EF_FILEIDLND                  =   0x6f44,
	SIM_EF_FILEIDEXT1                 =   0x6f4a,
	SIM_EF_FILEIDEXT2                 =   0x6f4b,
	SIM_EF_FILEIDSUME                 =   0x6f54,

	SIM_EF_FILEIDCPS                  =   0x6f3e,
	SIM_EF_FILEIDVGCS                 =   0x6fb1,
	SIM_EF_FILEIDVGCSS                =   0x6fb2,
	SIM_EF_FILEIDVBS                  =   0x6fb3,
	SIM_EF_FILEIDVBSS                 =   0x6fb4,
	SIM_EF_FILEIDEMLPP                =   0x6fb5,
	SIM_EF_FILEIDAAEM                 =   0x6fb6,
	SIM_EF_FILEIDECC                  =   0x6fb7,
	SIM_EF_FILEIDCBMID                =   0x6f48,
	SIM_EF_FILEIDCBMIR                =   0x6f50,
	SIM_EF_FILEIDDCK                  =   0x6f2c,
	SIM_EF_FILEIDCNL                  =   0x6f32,
	SIM_EF_FILEIDNIA                  =   0x6f51,
	SIM_EF_FILEIDSDN                  =   0x6f49,
	SIM_EF_FILEIDEXT3                 =   0x6f4c,
	SIM_EF_FILEIDBDN                  =   0x6f4d,
	SIM_EF_FILEIDEXT4                 =   0x6f4e,
	SIM_EF_FILEIDSMSR                 =   0x6f47,
	SIM_EF_FILEIDCPHS_VMWF            =   0x6f11,
	SIM_EF_FILEIDCPHS_CFF             =   0x6f13,
	SIM_EF_FILEIDCPHS_ON              =   0x6f14,
	SIM_EF_FILEIDCPHS_CSP             =   0x6f15,
	SIM_EF_FILEIDCPHS_INFO            =   0x6f16,
	SIM_EF_FILEIDCPHS_MN              =   0x6f17,
	SIM_EF_FILEIDCPHS_ONS             =   0x6f18,
	SIM_EF_FILEIDCPHS_INFO_NUM        =   0x6f19,
	SIM_EF_FILEIDCPHS_INFO_NUM_OLD    =   0xEA01,


	SIM_EF_FILEIDPLMNW_ACT            =   0x6F60,
	SIM_EF_FILEIDOPLMNW_ACT           =   0x6F61,
	SIM_EF_FILEIDHPLMNW_ACT           =   0x6F62,
	SIM_EF_FILEIDCPBCCH               =   0x6F63,
	SIM_EF_FILEIDINVSCAN              =   0x6f64,


	SIM_EF_FILEIDKC_GPRS              =   0x6f52,
	SIM_EF_FILEIDLOCI_GPRS            =   0x6f53,


	SIM_EF_FILEIDPNN                  =   0x6fc5,
	SIM_EF_FILEIDOPL                  =   0x6fc6,

	SIM_EF_FILEIDTST                  =   0x6fd2,
	SIM_EF_FILEIDACT_HPLMN            =   0x4F34,

	SIM_EF_FILEIDINVALID              =   0x0000
}SimEfFileId;


#define EF_FILE_SIZE_BYTE1      2
#define EF_FILE_SIZE_BYTE2      3
#define EF_FILE_ID1             4
#define EF_FILE_ID2             5
#define EF_TYPE_OF_FILE         6
#define EF_FILE_STATUS         11
#define EF_FOLLOWING_DATA_LEN  12
#define EF_STRUCTURE_OF_FILE   13
#define EF_RECORD_LENGTH       14


RETURNCODE_T  ciEnterPin(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciEnterPin2(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciDetectSIM(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciGetPinRetryTimes(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciGenericAccessSim(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciRestrictedAccessSIM(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciGetImsi(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciSTK(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
/* Michal Bukai*/
RETURNCODE_T  ciMEPCLCK(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciMEPCG(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciULCK(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciUCPWD(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciUASLM(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciEnVsim(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);

BOOL isCHVOper(char* dialDigits);
CiReturnCode ciUnblockPin(UINT32 atHandle, const char *command_name_p);
RETURNCODE_T  ciEUICC(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciRunGSMAlgorithm(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);
RETURNCODE_T  ciRunTContextAuth(const utlAtParameterOp_T op, const char *command_name_p, const utlAtParameterValue_P2c parameter_values_p, const size_t num_parameters, const char *info_text_p, unsigned int *xid_p, void *arg_p);

#endif

