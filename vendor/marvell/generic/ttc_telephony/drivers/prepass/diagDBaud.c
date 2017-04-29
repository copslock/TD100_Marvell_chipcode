#define DIAG_DB_C

#include "diagDBaud.h"

/* __trace_desc_t_Generic_No_Params prototype */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
}__trace_desc_t_Generic_No_Params;

/* GLOBAL VARIABLES */
UINT8 g_OverridePassFilter;
UINT8 filterArray[(531+8)/8];
const DiagTraceDescriptor* diagTraceDesc = NULL;
const DiagDBVersion DBversionID = "0x3a30f33d";
const UINT32 _FirstCommandID = 504;
const UINT32 _FirstReportID = 504;
const UINT32 _LastCommandID = 519;
const UINT32 _LastReportID = 531;
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
extern void Audio_HW_test(void);	/*exported in file audio_file.c*/


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
	Audio_HW_test/* Audio, HW, AUDIO_HW_TEST */
};

/* Total function number (allows range checking on the (moduleID<<16)+commandID) */
const int cat3CommandGlobalNumber = sizeof(cat3CommandGlobalAddress)/sizeof(CommandAddress);

const UINT16 diagNumOfReports = 531;

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
	return __trace_generic_no_params_func(2,504);
}

/* Report - Diag__Debug__UDPicatReadyOff_2 */
int __trace_Diag__Debug__UDPicatReadyOff_2(void)
{
	return __trace_generic_no_params_func(2,505);
}

/* Report - SW_PLAT__DIAG__ERROR_PRINTFTYPE */
int __trace_SW_PLAT__DIAG__ERROR_PRINTFTYPE(void)
{
	return __trace_generic_no_params_func(3,506);
}

/* Report - SW_PLAT__DIAG__PRINTF_ERROR_IN_PARAMS_NUMBER */
int __trace_SW_PLAT__DIAG__PRINTF_ERROR_IN_PARAMS_NUMBER(void)
{
	return __trace_generic_no_params_func(3,507);
}

/* Report - Audio__HW__AUDIO_HW_PathsStatus */
int __trace_Audio__HW__AUDIO_HW_PathsStatus(void)
{
	return __trace_generic_no_params_func(6,508);
}

/* Report - Audio__Organ__AUDIO_ORGAN_SlopeOffset */
int __trace_Audio__Organ__AUDIO_ORGAN_SlopeOffset(void)
{
	return __trace_generic_no_params_func(6,509);
}

/* Report - Audio__Organ__AUDIO_ORGAN_ReadFdiDataOK */
int __trace_Audio__Organ__AUDIO_ORGAN_ReadFdiDataOK(void)
{
	return __trace_generic_no_params_func(6,510);
}

/* Report - Audio__Organ__AUDIO_ORGAN_ReadFdiDataERROR */
int __trace_Audio__Organ__AUDIO_ORGAN_ReadFdiDataERROR(void)
{
	return __trace_generic_no_params_func(6,511);
}

/* Report - Audio__HW__AUDIO_HW_PathEnableIgnore */
int __trace_Audio__HW__AUDIO_HW_PathEnableIgnore(void)
{
	return __trace_generic_no_params_func(6,512);
}

/* Report - Audio__HW__AUDIO_HW_PathEnableOK */
int __trace_Audio__HW__AUDIO_HW_PathEnableOK(void)
{
	return __trace_generic_no_params_func(6,513);
}

/* Report - Audio__HW__AUDIO_HW_PathEnableSTATUSERROR */
int __trace_Audio__HW__AUDIO_HW_PathEnableSTATUSERROR(void)
{
	return __trace_generic_no_params_func(6,514);
}

/* Report - Audio__HW__AUDIO_HW_PathDisableOK */
int __trace_Audio__HW__AUDIO_HW_PathDisableOK(void)
{
	return __trace_generic_no_params_func(6,515);
}

/* Report - Audio__HW__AUDIO_HW_PathMuteOK */
int __trace_Audio__HW__AUDIO_HW_PathMuteOK(void)
{
	return __trace_generic_no_params_func(6,516);
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
		8,517,1,{{DIAG_STRING,1}}
	};
	if(!PASS_FILTER(517)) return DIAG_INVALID_MODULE_ID;
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
		8,518,1,{{DIAG_STRING,1}}
	};
	if(!PASS_FILTER(518)) return DIAG_INVALID_MODULE_ID;
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
		8,519,1,{{DIAG_STRING,1}}
	};
	if(!PASS_FILTER(519)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - Diag__Utils__ReadRTC_Failure */
int __trace_Diag__Utils__ReadRTC_Failure(void)
{
	return __trace_generic_no_params_func(8,520);
}

/* Report - VALI_IF__ATCMD_IF__ALREADYRUNNING */
int __trace_VALI_IF__ATCMD_IF__ALREADYRUNNING(void)
{
	return __trace_generic_no_params_func(8,521);
}

/* Report - VALI_IF__ATCMD_IF__STOPIFNOTSTARTED */
int __trace_VALI_IF__ATCMD_IF__STOPIFNOTSTARTED(void)
{
	return __trace_generic_no_params_func(8,522);
}

/* Report - VALI_IF__ATCMD_IF__ATCMDIFNOTSTARTEDINSENDAT */
int __trace_VALI_IF__ATCMD_IF__ATCMDIFNOTSTARTEDINSENDAT(void)
{
	return __trace_generic_no_params_func(8,523);
}

/* Report - VALI_IF__ATCMD_IF__ATCMDIFNOTSTARTEDINSENDATCHAR */
int __trace_VALI_IF__ATCMD_IF__ATCMDIFNOTSTARTEDINSENDATCHAR(void)
{
	return __trace_generic_no_params_func(8,524);
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
		8,525,1,{{DIAG_STRING,1}}
	};
	if(!PASS_FILTER(525)) return DIAG_INVALID_MODULE_ID;
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
		8,526,2,{{DIAG_SHORT,1},{DIAG_SHORT,1}}
	};
	if(!PASS_FILTER(526)) return DIAG_INVALID_MODULE_ID;
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
		8,527,1,{{DIAG_SHORT,1}}
	};
	if(!PASS_FILTER(527)) return DIAG_INVALID_MODULE_ID;
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
		8,528,1,{{DIAG_SHORT,1}}
	};
	if(!PASS_FILTER(528)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - SYSTEM__PROD__AT_SER_OK */
int __trace_SYSTEM__PROD__AT_SER_OK(void)
{
	return __trace_generic_no_params_func(8,529);
}

/* Report - AUDIO__ACM__ACMAudioTEST */
int __trace_AUDIO__ACM__ACMAudioTEST(void)
{
	return __trace_generic_no_params_func(9,530);
}

/* Report - AUDIO__ACM__StructAudioTEST */
int __trace_AUDIO__ACM__StructAudioTEST(void)
{
	return __trace_generic_no_params_func(9,531);
}


#undef DIAG_DB_C

