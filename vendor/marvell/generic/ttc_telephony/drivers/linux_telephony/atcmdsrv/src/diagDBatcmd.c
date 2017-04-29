#define DIAG_DB_C

#include "diagDBatcmd.h"

/* __trace_desc_t_Generic_No_Params prototype */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
}__trace_desc_t_Generic_No_Params;

/* GLOBAL VARIABLES */
UINT8 g_OverridePassFilter;
UINT8 filterArray[(1034+8)/8];
const DiagTraceDescriptor* diagTraceDesc = NULL;
const DiagDBVersion DBversionID = "0x319c6c3c";
const UINT32 _FirstCommandID = 1008;
const UINT32 _FirstReportID = 1008;
const UINT32 _LastCommandID = 1023;
const UINT32 _LastReportID = 1034;
__trace_desc_t_Generic_No_Params global_trace_desc_t_no_params;

/* Exported function prototypes */
extern void udpInternalICATreadyon(void);	/*exported in file diag_comm_EXTif_OSA_LINUX.c*/
extern void udpInternalICATreadyoff(void);	/*exported in file diag_comm_EXTif_OSA_LINUX.c*/
extern void GetPathsStatus(void);	/*exported in file diag_organ.c*/
extern void GetSlopeOffset(void);	/*exported in file diag_organ.c*/
extern void ReadFdiCalibData(void);	/*exported in file diag_organ.c*/
extern void PathEnable(void);	/*exported in file diag_organ.c*/
extern void PathDisable(void);	/*exported in file diag_organ.c*/
extern void PathMute(void);	/*exported in file diag_organ.c*/
extern void StartATCmdIF(void);	/*exported in file diag_al.c*/
extern void StopATCmdIF(void);	/*exported in file diag_al.c*/
extern void SendATCmd(void);	/*exported in file diag_al.c*/
extern void SendATCmdChars(void);	/*exported in file diag_al.c*/
extern void SendSystem(void);	/*exported in file diag_al.c*/
extern void diagRcvDataFromCPTask(void);	/*exported in file diag_al.c*/
extern void AT_SER(void);	/*exported in file diag_al.c*/
extern void Atcmd_HW_test(void);	/*exported in file linux_telephony*/


/* Exported function table */
const CommandAddress cat3CommandGlobalAddress[] =
{	udpInternalICATreadyon/* Diag, Debug, udpInternalICATreadyon */,
	udpInternalICATreadyoff/* Diag, Debug, udpInternalICATreadyoff */,
	GetPathsStatus/* Audio, HW, AUDIO_HW_GetPathsStatus */,
	GetSlopeOffset/* Audio, Organ, GetSlopeOffset */,
	ReadFdiCalibData/* Audio, Organ, ReadFdiCalibData */,
	PathEnable/* Audio, HW, AUDIO_HW_ORGAN_PathEnable */,
	PathDisable/* Audio, HW, AUDIO_HW_ORGAN_PathDisable */,
	PathMute/* Audio, HW, AUDIO_HW_PathMute */,
	StartATCmdIF/* VALI_IF, ATCMD_IF, StartATCmdIF */,
	StopATCmdIF/* VALI_IF, ATCMD_IF, StopATCmdIF */,
	SendATCmd/* VALI_IF, ATCMD_IF, SendATCmd */,
	SendATCmdChars/* VALI_IF, ATCMD_IF, SendATCmdChars */,
	SendSystem/* VALI_IF, MISC_IF, SendSystem */,
	diagRcvDataFromCPTask/* Diag, timeprint, diagRcvDataFromCPTask */,
	AT_SER/* SYSTEM, PROD, AT_SER */,
	Atcmd_HW_test/* VALI_IF, ATCMD_IF, TestATCmdIF */
};

/* Total function number (allows range checking on the (moduleID<<16)+commandID) */
const int cat3CommandGlobalNumber = sizeof(cat3CommandGlobalAddress)/sizeof(CommandAddress);

const UINT16 diagNumOfReports = 1034;

int	nNumOfParameters=0;

/* generic function for a non params report */
int __trace_generic_no_params_func(INT16 moduleID, UINT16 reportID)
{
	if(!PASS_FILTER(reportID)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	global_trace_desc_t_no_params. moduleID = moduleID;
	global_trace_desc_t_no_params. reportID = reportID;
	/*global_trace_desc_t_no_params. numOfParams is always zero!*/
	diagTraceDesc = (DiagTraceDescriptor*)& global_trace_desc_t_no_params;
	return 1;
}

/* Report - Diag__Debug__UDPicatReadyOn_1 */
int __trace_Diag__Debug__UDPicatReadyOn_1(void)
{
	return __trace_generic_no_params_func(2,1008);
}

/* Report - Diag__Debug__UDPicatReadyOff_2 */
int __trace_Diag__Debug__UDPicatReadyOff_2(void)
{
	return __trace_generic_no_params_func(2,1009);
}

/* Report - SW_PLAT__DIAG__ERROR_PRINTFTYPE */
int __trace_SW_PLAT__DIAG__ERROR_PRINTFTYPE(void)
{
	return __trace_generic_no_params_func(3,1010);
}

/* Report - SW_PLAT__DIAG__PRINTF_ERROR_IN_PARAMS_NUMBER */
int __trace_SW_PLAT__DIAG__PRINTF_ERROR_IN_PARAMS_NUMBER(void)
{
	return __trace_generic_no_params_func(3,1011);
}

/* Report - Audio__HW__AUDIO_HW_PathsStatus */
int __trace_Audio__HW__AUDIO_HW_PathsStatus(void)
{
	return __trace_generic_no_params_func(6,1012);
}

/* Report - Audio__Organ__AUDIO_ORGAN_SlopeOffset */
int __trace_Audio__Organ__AUDIO_ORGAN_SlopeOffset(void)
{
	return __trace_generic_no_params_func(6,1013);
}

/* Report - Audio__Organ__AUDIO_ORGAN_ReadFdiDataOK */
int __trace_Audio__Organ__AUDIO_ORGAN_ReadFdiDataOK(void)
{
	return __trace_generic_no_params_func(6,1014);
}

/* Report - Audio__Organ__AUDIO_ORGAN_ReadFdiDataERROR */
int __trace_Audio__Organ__AUDIO_ORGAN_ReadFdiDataERROR(void)
{
	return __trace_generic_no_params_func(6,1015);
}

/* Report - Audio__HW__AUDIO_HW_PathEnableIgnore */
int __trace_Audio__HW__AUDIO_HW_PathEnableIgnore(void)
{
	return __trace_generic_no_params_func(6,1016);
}

/* Report - Audio__HW__AUDIO_HW_PathEnableOK */
int __trace_Audio__HW__AUDIO_HW_PathEnableOK(void)
{
	return __trace_generic_no_params_func(6,1017);
}

/* Report - Audio__HW__AUDIO_HW_PathEnableSTATUSERROR */
int __trace_Audio__HW__AUDIO_HW_PathEnableSTATUSERROR(void)
{
	return __trace_generic_no_params_func(6,1018);
}

/* Report - Audio__HW__AUDIO_HW_PathDisableOK */
int __trace_Audio__HW__AUDIO_HW_PathDisableOK(void)
{
	return __trace_generic_no_params_func(6,1019);
}

/* Report - Audio__HW__AUDIO_HW_PathMuteOK */
int __trace_Audio__HW__AUDIO_HW_PathMuteOK(void)
{
	return __trace_generic_no_params_func(6,1020);
}

/* Report - VALI_IF__ATCMD_IF__ATOUT_CHAR */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[1];  // specified fixed length
}__trace_desc_t_VALI_IF__ATCMD_IF__ATOUT_CHAR;

int __trace_VALI_IF__ATCMD_IF__ATOUT_CHAR(void)
{
	static const __trace_desc_t_VALI_IF__ATCMD_IF__ATOUT_CHAR localTraceDesc=
	{
		8,1021,1,{{DIAG_STRING,1}}
	};
	if(!PASS_FILTER(1021)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - VALI_IF__ATCMD_IF__ATOUT */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[1];  // specified fixed length
}__trace_desc_t_VALI_IF__ATCMD_IF__ATOUT;

int __trace_VALI_IF__ATCMD_IF__ATOUT(void)
{
	static const __trace_desc_t_VALI_IF__ATCMD_IF__ATOUT localTraceDesc=
	{
		8,1022,1,{{DIAG_STRING,1}}
	};
	if(!PASS_FILTER(1022)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - Diag__Utils__Print_RTC */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[1];  // specified fixed length
}__trace_desc_t_Diag__Utils__Print_RTC;

int __trace_Diag__Utils__Print_RTC(void)
{
	static const __trace_desc_t_Diag__Utils__Print_RTC localTraceDesc=
	{
		8,1023,1,{{DIAG_STRING,1}}
	};
	if(!PASS_FILTER(1023)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - Diag__Utils__ReadRTC_Failure */
int __trace_Diag__Utils__ReadRTC_Failure(void)
{
	return __trace_generic_no_params_func(8,1024);
}

/* Report - VALI_IF__ATCMD_IF__ALREADYRUNNING */
int __trace_VALI_IF__ATCMD_IF__ALREADYRUNNING(void)
{
	return __trace_generic_no_params_func(8,1025);
}

/* Report - VALI_IF__ATCMD_IF__STOPIFNOTSTARTED */
int __trace_VALI_IF__ATCMD_IF__STOPIFNOTSTARTED(void)
{
	return __trace_generic_no_params_func(8,1026);
}

/* Report - VALI_IF__ATCMD_IF__ATCMDIFNOTSTARTEDINSENDAT */
int __trace_VALI_IF__ATCMD_IF__ATCMDIFNOTSTARTEDINSENDAT(void)
{
	return __trace_generic_no_params_func(8,1027);
}

/* Report - VALI_IF__ATCMD_IF__ATCMDIFNOTSTARTEDINSENDATCHAR */
int __trace_VALI_IF__ATCMD_IF__ATCMDIFNOTSTARTEDINSENDATCHAR(void)
{
	return __trace_generic_no_params_func(8,1028);
}

/* Report - VALI_IF__MISC_IF__ATCMDSYSTEM */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[1];  // specified fixed length
}__trace_desc_t_VALI_IF__MISC_IF__ATCMDSYSTEM;

int __trace_VALI_IF__MISC_IF__ATCMDSYSTEM(void)
{
	static const __trace_desc_t_VALI_IF__MISC_IF__ATCMDSYSTEM localTraceDesc=
	{
		8,1029,1,{{DIAG_STRING,1}}
	};
	if(!PASS_FILTER(1029)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - DIAG__Dispatcher__ExportedPtr */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[2];  // specified fixed length
}__trace_desc_t_DIAG__Dispatcher__ExportedPtr;

int __trace_DIAG__Dispatcher__ExportedPtr(void)
{
	static const __trace_desc_t_DIAG__Dispatcher__ExportedPtr localTraceDesc=
	{
		8,1030,2,{{DIAG_SHORT,1},{DIAG_SHORT,1}}
	};
	if(!PASS_FILTER(1030)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - SW_PLAT__DIAG__COMMANDID_WRONG */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[1];  // specified fixed length
}__trace_desc_t_SW_PLAT__DIAG__COMMANDID_WRONG;

int __trace_SW_PLAT__DIAG__COMMANDID_WRONG(void)
{
	static const __trace_desc_t_SW_PLAT__DIAG__COMMANDID_WRONG localTraceDesc=
	{
		8,1031,1,{{DIAG_SHORT,1}}
	};
	if(!PASS_FILTER(1031)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - SW_PLAT__DIAG__RX_SERVICEID_WRONG */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[1];  // specified fixed length
}__trace_desc_t_SW_PLAT__DIAG__RX_SERVICEID_WRONG;

int __trace_SW_PLAT__DIAG__RX_SERVICEID_WRONG(void)
{
	static const __trace_desc_t_SW_PLAT__DIAG__RX_SERVICEID_WRONG localTraceDesc=
	{
		8,1032,1,{{DIAG_SHORT,1}}
	};
	if(!PASS_FILTER(1032)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - SYSTEM__PROD__AT_SER_OK */
int __trace_SYSTEM__PROD__AT_SER_OK(void)
{
	return __trace_generic_no_params_func(8,1033);
}

/* Report - VALI_IF__ATCMD__ATCMDTEST */
int __trace_VALI_IF__ATCMD__ATCMDTEST(void)
{
	return __trace_generic_no_params_func(9,1034);
}


#undef DIAG_DB_C

