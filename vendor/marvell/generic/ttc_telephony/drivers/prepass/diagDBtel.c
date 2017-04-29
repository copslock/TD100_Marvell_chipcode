#define DIAG_DB_C

#include "diagDBtel.h"

/* __trace_desc_t_Generic_No_Params prototype */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
}__trace_desc_t_Generic_No_Params;

/* GLOBAL VARIABLES */
UINT8 g_OverridePassFilter;
UINT8 filterArray[(126+8)/8];
const DiagTraceDescriptor* diagTraceDesc = NULL;
const DiagDBVersion DBversionID = "0x308aa80c";
const UINT32 _FirstCommandID = 0;
const UINT32 _FirstReportID = 0;
const UINT32 _LastCommandID = 64;
const UINT32 _LastReportID = 126;
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
extern void IsBackNVMValid(void);	/*exported in file source*/
extern void IsBackNVMCfgValid(void);	/*exported in file source*/
extern void StoreBackNVM(void);	/*exported in file source*/
extern void LoadBackNVM(void);	/*exported in file source*/
extern void CreateDefaultBackNVMCfg(void);	/*exported in file source*/
extern void diagGetOutMsgBodyLimit(void);	/*exported in file source*/
extern void _Fopen(void);	/*exported in file source*/
extern void _Fclose(void);	/*exported in file source*/
extern void _Fread(void);	/*exported in file source*/
extern void _Fwrite(void);	/*exported in file source*/
extern void _Format(void);	/*exported in file source*/
extern void _Remove(void);	/*exported in file source*/
extern void _GetFileNameList(void);	/*exported in file source*/
extern void _RenameFile(void);	/*exported in file source*/
extern void _FStatus(void);	/*exported in file source*/
extern void _FChangeMode(void);	/*exported in file source*/
extern void _Fseek(void);	/*exported in file source*/
extern void _GetMaxFileNameLength(void);	/*exported in file source*/
extern void _GetFdiFdvSize(void);	/*exported in file source*/
extern void _GetFdiFdvAvailableSpace(void);	/*exported in file source*/
extern void _GetFdiVersion(void);	/*exported in file source*/
extern void _MkDir(void);	/*exported in file source*/
extern void _RmDir(void);	/*exported in file source*/
extern void _GetFdiVolumes(void);	/*exported in file source*/
extern void _ExtendetMode(void);	/*exported in file source*/
extern void _SetRTC(void);	/*exported in file source*/
extern void _ReadRTC(void);	/*exported in file source*/
extern void EeHandlerTestDataAbortExcep(void);	/*exported in file source*/
extern void EeHandlerTestAssert(void);	/*exported in file source*/
extern void _GetVersionDiag(void);	/*exported in file source*/
extern void ResetTarget(void);	/*exported in file source*/
extern void INIT_CT_Panalyzer(void);	/*exported in file source*/
extern void TERM_CT_Panalyzer(void);	/*exported in file source*/
extern void Get_IPM_Drivers_List(void);	/*exported in file source*/
extern void battery_calibration(void);	/*exported in file source*/
extern void printComLoadTable(void);	/*exported in file src*/
extern void SaveComPostmortem(void);	/*exported in file src*/
extern void SaveCom_Addr_Len(void);	/*exported in file src*/
extern void ACMAudioPathEnable_Env(void);	/*exported in file diag_aud.c*/
extern void ACMAudioPathDisable_Env(void);	/*exported in file diag_aud.c*/
extern void ACMAudioDeviceEnable_Env(void);	/*exported in file diag_aud.c*/
extern void ACMAudioDeviceDisable_Env(void);	/*exported in file diag_aud.c*/
extern void ACMAudioDeviceMute_Env(void);	/*exported in file diag_aud.c*/
extern void ACMAudioDeviceVolumeSet_Env(void);	/*exported in file diag_aud.c*/
extern void ACMAudioCalibrationStateSet_Env(void);	/*exported in file diag_aud.c*/
extern void eePrintConfiguration(void);	/*exported in file src*/
extern void eeInitConfiguration(void);	/*exported in file src*/
extern void eeSetConfiguration(void);	/*exported in file src*/
extern void eeReadConfiguration_ICAT(void);	/*exported in file src*/
extern void eeSaveConfiguration(void);	/*exported in file src*/


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
	IsBackNVMValid/* FDI, Transport, IsBackNVMValid */,
	IsBackNVMCfgValid/* FDI, Transport, IsBackNVMCfgValid */,
	StoreBackNVM/* FDI, Transport, StoreBackNVM */,
	LoadBackNVM/* FDI, Transport, LoadBackNVM */,
	CreateDefaultBackNVMCfg/* FDI, Transport, CreateDefaultBackNVMCfg */,
	diagGetOutMsgBodyLimit/* Diag, Utils, OutMsgBodyLimit */,
	_Fopen/* FDI, Transport, Fopen */,
	_Fclose/* FDI, Transport, Fclose */,
	_Fread/* FDI, Transport, Fread */,
	_Fwrite/* FDI, Transport, Fwrite */,
	_Format/* FDI, Transport, Format_Flash */,
	_Remove/* FDI, Transport, Remove_File */,
	_GetFileNameList/* FDI, Transport, GetFileNameList */,
	_RenameFile/* FDI, Transport, RenameFile */,
	_FStatus/* FDI, Transport, FStatus */,
	_FChangeMode/* FDI, Transport, FchangeMode */,
	_Fseek/* FDI, Transport, Fseek */,
	_GetMaxFileNameLength/* FDI, Transport, GetMaxFileNameLen */,
	_GetFdiFdvSize/* FDI, Transport, GetFdiFdvSize */,
	_GetFdiFdvAvailableSpace/* FDI, Transport, GetFdiFdvAvailableSpace */,
	_GetFdiVersion/* FDI, Transport, GetFdiVersion */,
	_MkDir/* FDI, Transport, MkDir */,
	_RmDir/* FDI, Transport, RmDir */,
	_GetFdiVolumes/* FDI, Transport, GetFdiVolumes */,
	_ExtendetMode/* FDI, Transport, ExtendedMode */,
	_SetRTC/* Diag, Utils, SetRTC */,
	_ReadRTC/* Diag, Utils, ReadRTC */,
	EeHandlerTestDataAbortExcep/* EE_HANDLER, EE, EE_Test_data_abort */,
	EeHandlerTestAssert/* EE_HANDLER, EE, EE_Test_Assert */,
	_GetVersionDiag/* Diag, Utils, GetVersionDiag */,
	ResetTarget/* VALI_IF, MISC_IF, temp_RESET_IF */,
	INIT_CT_Panalyzer/* VALI_IF, CT_P_API, INIT_CT_Panalyzer */,
	TERM_CT_Panalyzer/* VALI_IF, CT_P_API, TERM_CT_Panalyzer */,
	Get_IPM_Drivers_List/* Validation, Utils, Get_IPM_Drivers_List */,
	battery_calibration/* Diag, Utils, battery_calibration */,
	printComLoadTable/* Diag, comMem, printComLoadTable */,
	SaveComPostmortem/* Diag, comMem, SaveComPostmortem */,
	SaveCom_Addr_Len/* Diag, comMem, SaveCom_Addr_Len */,
	ACMAudioPathEnable_Env/* Audio, HW, AUDIO_HW_PathEnable */,
	ACMAudioPathDisable_Env/* Audio, HW, AUDIO_HW_PathDisable */,
	ACMAudioDeviceEnable_Env/* Audio, ACM, ACMAudioDeviceEnable */,
	ACMAudioDeviceDisable_Env/* Audio, ACM, ACMAudioDeviceDisable */,
	ACMAudioDeviceMute_Env/* Audio, ACM, ACMAudioDeviceMute */,
	ACMAudioDeviceVolumeSet_Env/* Audio, ACM, ACMAudioDeviceVolumeSet */,
	ACMAudioCalibrationStateSet_Env/* Audio, ACM, ACMAudioCalibrationStateSet */,
	eePrintConfiguration/* EE_HANDLER, EE, eePrintConfiguration */,
	eeInitConfiguration/* EE_HANDLER, EE, eeInitConfiguration */,
	eeSetConfiguration/* EE_HANDLER, EE, eeSetConfiguration */,
	eeReadConfiguration_ICAT/* EE_HANDLER, EE, eeReadConfiguration */,
	eeSaveConfiguration/* EE_HANDLER, EE, eeSaveConfiguration */
};

/* Total function number (allows range checking on the (moduleID<<16)+commandID) */
const int cat3CommandGlobalNumber = sizeof(cat3CommandGlobalAddress)/sizeof(CommandAddress);

const UINT16 diagNumOfReports = 126;

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
	return __trace_generic_no_params_func(2,1);
}

/* Report - Diag__Debug__UDPicatReadyOff_2 */
int __trace_Diag__Debug__UDPicatReadyOff_2(void)
{
	return __trace_generic_no_params_func(2,2);
}

/* Report - SW_PLAT__DIAG__ERROR_PRINTFTYPE */
int __trace_SW_PLAT__DIAG__ERROR_PRINTFTYPE(void)
{
	return __trace_generic_no_params_func(3,3);
}

/* Report - SW_PLAT__DIAG__PRINTF_ERROR_IN_PARAMS_NUMBER */
int __trace_SW_PLAT__DIAG__PRINTF_ERROR_IN_PARAMS_NUMBER(void)
{
	return __trace_generic_no_params_func(3,4);
}

/* Report - Audio__HW__AUDIO_HW_PathsStatus */
int __trace_Audio__HW__AUDIO_HW_PathsStatus(void)
{
	return __trace_generic_no_params_func(6,5);
}

/* Report - Audio__Organ__AUDIO_ORGAN_SlopeOffset */
int __trace_Audio__Organ__AUDIO_ORGAN_SlopeOffset(void)
{
	return __trace_generic_no_params_func(6,6);
}

/* Report - Audio__Organ__AUDIO_ORGAN_ReadFdiDataOK */
int __trace_Audio__Organ__AUDIO_ORGAN_ReadFdiDataOK(void)
{
	return __trace_generic_no_params_func(6,7);
}

/* Report - Audio__Organ__AUDIO_ORGAN_ReadFdiDataERROR */
int __trace_Audio__Organ__AUDIO_ORGAN_ReadFdiDataERROR(void)
{
	return __trace_generic_no_params_func(6,8);
}

/* Report - Audio__HW__AUDIO_HW_PathEnableIgnore */
int __trace_Audio__HW__AUDIO_HW_PathEnableIgnore(void)
{
	return __trace_generic_no_params_func(6,9);
}

/* Report - Audio__HW__AUDIO_HW_PathEnableOK */
int __trace_Audio__HW__AUDIO_HW_PathEnableOK(void)
{
	return __trace_generic_no_params_func(6,10);
}

/* Report - Audio__HW__AUDIO_HW_PathEnableSTATUSERROR */
int __trace_Audio__HW__AUDIO_HW_PathEnableSTATUSERROR(void)
{
	return __trace_generic_no_params_func(6,11);
}

/* Report - Audio__HW__AUDIO_HW_PathDisableOK */
int __trace_Audio__HW__AUDIO_HW_PathDisableOK(void)
{
	return __trace_generic_no_params_func(6,12);
}

/* Report - Audio__HW__AUDIO_HW_PathMuteOK */
int __trace_Audio__HW__AUDIO_HW_PathMuteOK(void)
{
	return __trace_generic_no_params_func(6,13);
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
		8,14,1,{{DIAG_STRING,1}}
	};
	if(!PASS_FILTER(14)) return DIAG_INVALID_MODULE_ID;
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
		8,15,1,{{DIAG_STRING,1}}
	};
	if(!PASS_FILTER(15)) return DIAG_INVALID_MODULE_ID;
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
		8,16,1,{{DIAG_STRING,1}}
	};
	if(!PASS_FILTER(16)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - Diag__Utils__ReadRTC_Failure */
int __trace_Diag__Utils__ReadRTC_Failure(void)
{
	return __trace_generic_no_params_func(8,17);
}

/* Report - VALI_IF__ATCMD_IF__ALREADYRUNNING */
int __trace_VALI_IF__ATCMD_IF__ALREADYRUNNING(void)
{
	return __trace_generic_no_params_func(8,18);
}

/* Report - VALI_IF__ATCMD_IF__STOPIFNOTSTARTED */
int __trace_VALI_IF__ATCMD_IF__STOPIFNOTSTARTED(void)
{
	return __trace_generic_no_params_func(8,19);
}

/* Report - VALI_IF__ATCMD_IF__ATCMDIFNOTSTARTEDINSENDAT */
int __trace_VALI_IF__ATCMD_IF__ATCMDIFNOTSTARTEDINSENDAT(void)
{
	return __trace_generic_no_params_func(8,20);
}

/* Report - VALI_IF__ATCMD_IF__ATCMDIFNOTSTARTEDINSENDATCHAR */
int __trace_VALI_IF__ATCMD_IF__ATCMDIFNOTSTARTEDINSENDATCHAR(void)
{
	return __trace_generic_no_params_func(8,21);
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
		8,22,1,{{DIAG_STRING,1}}
	};
	if(!PASS_FILTER(22)) return DIAG_INVALID_MODULE_ID;
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
		8,23,2,{{DIAG_SHORT,1},{DIAG_SHORT,1}}
	};
	if(!PASS_FILTER(23)) return DIAG_INVALID_MODULE_ID;
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
		8,24,1,{{DIAG_SHORT,1}}
	};
	if(!PASS_FILTER(24)) return DIAG_INVALID_MODULE_ID;
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
		8,25,1,{{DIAG_SHORT,1}}
	};
	if(!PASS_FILTER(25)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - SYSTEM__PROD__AT_SER_OK */
int __trace_SYSTEM__PROD__AT_SER_OK(void)
{
	return __trace_generic_no_params_func(8,26);
}

/* Report - FDI__Transport__BackNVMOpenError */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[1];  // specified fixed length
}__trace_desc_t_FDI__Transport__BackNVMOpenError;

int __trace_FDI__Transport__BackNVMOpenError(void)
{
	static const __trace_desc_t_FDI__Transport__BackNVMOpenError localTraceDesc=
	{
		9,27,1,{{DIAG_STRING,1}}
	};
	if(!PASS_FILTER(27)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - FDI__Transport__StoreBackNVMLimitError */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[2];  // specified fixed length
}__trace_desc_t_FDI__Transport__StoreBackNVMLimitError;

int __trace_FDI__Transport__StoreBackNVMLimitError(void)
{
	static const __trace_desc_t_FDI__Transport__StoreBackNVMLimitError localTraceDesc=
	{
		9,28,2,{{DIAG_SHORT,1},{DIAG_SHORT,1}}
	};
	if(!PASS_FILTER(28)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - FDI__Transport__StoreBackNVMError */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[2];  // specified fixed length
}__trace_desc_t_FDI__Transport__StoreBackNVMError;

int __trace_FDI__Transport__StoreBackNVMError(void)
{
	static const __trace_desc_t_FDI__Transport__StoreBackNVMError localTraceDesc=
	{
		9,29,2,{{DIAG_STRING,1},{DIAG_SHORT,1}}
	};
	if(!PASS_FILTER(29)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - FDI__Transport__StoreBackNVMFile */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[2];  // specified fixed length
}__trace_desc_t_FDI__Transport__StoreBackNVMFile;

int __trace_FDI__Transport__StoreBackNVMFile(void)
{
	static const __trace_desc_t_FDI__Transport__StoreBackNVMFile localTraceDesc=
	{
		9,30,2,{{DIAG_STRING,1},{DIAG_SHORT,1}}
	};
	if(!PASS_FILTER(30)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - FDI__Transport__LoadBackNVMError */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[2];  // specified fixed length
}__trace_desc_t_FDI__Transport__LoadBackNVMError;

int __trace_FDI__Transport__LoadBackNVMError(void)
{
	static const __trace_desc_t_FDI__Transport__LoadBackNVMError localTraceDesc=
	{
		9,31,2,{{DIAG_STRING,1},{DIAG_SHORT,1}}
	};
	if(!PASS_FILTER(31)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - FDI__Transport__LoadBackNVMWarning */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[2];  // specified fixed length
}__trace_desc_t_FDI__Transport__LoadBackNVMWarning;

int __trace_FDI__Transport__LoadBackNVMWarning(void)
{
	static const __trace_desc_t_FDI__Transport__LoadBackNVMWarning localTraceDesc=
	{
		9,32,2,{{DIAG_STRING,1},{DIAG_SHORT,1}}
	};
	if(!PASS_FILTER(32)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - FDI__Transport__LoadBackNVMFile */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[2];  // specified fixed length
}__trace_desc_t_FDI__Transport__LoadBackNVMFile;

int __trace_FDI__Transport__LoadBackNVMFile(void)
{
	static const __trace_desc_t_FDI__Transport__LoadBackNVMFile localTraceDesc=
	{
		9,33,2,{{DIAG_STRING,1},{DIAG_SHORT,1}}
	};
	if(!PASS_FILTER(33)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - FDI__Transport__BackNVMValid */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[7];  // specified fixed length
}__trace_desc_t_FDI__Transport__BackNVMValid;

int __trace_FDI__Transport__BackNVMValid(void)
{
	static const __trace_desc_t_FDI__Transport__BackNVMValid localTraceDesc=
	{
		9,34,7,{{DIAG_SHORT,1},{DIAG_SHORT,1},{DIAG_SHORT,1},{DIAG_SHORT,1},{DIAG_SHORT,1},{DIAG_SHORT,1},{DIAG_SHORT,1}}
	};
	if(!PASS_FILTER(34)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - FDI__Transport__BackNVMInvalid */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[7];  // specified fixed length
}__trace_desc_t_FDI__Transport__BackNVMInvalid;

int __trace_FDI__Transport__BackNVMInvalid(void)
{
	static const __trace_desc_t_FDI__Transport__BackNVMInvalid localTraceDesc=
	{
		9,35,7,{{DIAG_SHORT,1},{DIAG_SHORT,1},{DIAG_SHORT,1},{DIAG_SHORT,1},{DIAG_SHORT,1},{DIAG_SHORT,1},{DIAG_SHORT,1}}
	};
	if(!PASS_FILTER(35)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - FDI__Transport__BackNVMCfgInvalid */
int __trace_FDI__Transport__BackNVMCfgInvalid(void)
{
	return __trace_generic_no_params_func(9,36);
}

/* Report - FDI__Transport__BackNVMCfgValid */
int __trace_FDI__Transport__BackNVMCfgValid(void)
{
	return __trace_generic_no_params_func(9,37);
}

/* Report - FDI__Transport__StoreBackNVMSuccess */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[1];  // specified fixed length
}__trace_desc_t_FDI__Transport__StoreBackNVMSuccess;

int __trace_FDI__Transport__StoreBackNVMSuccess(void)
{
	static const __trace_desc_t_FDI__Transport__StoreBackNVMSuccess localTraceDesc=
	{
		9,38,1,{{DIAG_SHORT,1}}
	};
	if(!PASS_FILTER(38)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - FDI__Transport__StoreBackNVMFailure */
int __trace_FDI__Transport__StoreBackNVMFailure(void)
{
	return __trace_generic_no_params_func(9,39);
}

/* Report - FDI__Transport__LoadBackNVMSuccess */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[1];  // specified fixed length
}__trace_desc_t_FDI__Transport__LoadBackNVMSuccess;

int __trace_FDI__Transport__LoadBackNVMSuccess(void)
{
	static const __trace_desc_t_FDI__Transport__LoadBackNVMSuccess localTraceDesc=
	{
		9,40,1,{{DIAG_SHORT,1}}
	};
	if(!PASS_FILTER(40)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - FDI__Transport__LoadBackNVMFailure */
int __trace_FDI__Transport__LoadBackNVMFailure(void)
{
	return __trace_generic_no_params_func(9,41);
}

/* Report - FDI__Transport__CreateDefaultBackNVMCfgSuccess */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[1];  // specified fixed length
}__trace_desc_t_FDI__Transport__CreateDefaultBackNVMCfgSuccess;

int __trace_FDI__Transport__CreateDefaultBackNVMCfgSuccess(void)
{
	static const __trace_desc_t_FDI__Transport__CreateDefaultBackNVMCfgSuccess localTraceDesc=
	{
		9,42,1,{{DIAG_STRING,1}}
	};
	if(!PASS_FILTER(42)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - FDI__Transport__CreateDefaultBackNVMCfgFail */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[1];  // specified fixed length
}__trace_desc_t_FDI__Transport__CreateDefaultBackNVMCfgFail;

int __trace_FDI__Transport__CreateDefaultBackNVMCfgFail(void)
{
	static const __trace_desc_t_FDI__Transport__CreateDefaultBackNVMCfgFail localTraceDesc=
	{
		9,43,1,{{DIAG_STRING,1}}
	};
	if(!PASS_FILTER(43)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - Diag__Utils__MaxOutMsgBodySize */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[1];  // specified fixed length
}__trace_desc_t_Diag__Utils__MaxOutMsgBodySize;

int __trace_Diag__Utils__MaxOutMsgBodySize(void)
{
	static const __trace_desc_t_Diag__Utils__MaxOutMsgBodySize localTraceDesc=
	{
		10,44,1,{{DIAG_LONG,1}}
	};
	if(!PASS_FILTER(44)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - FDI__Transport__F_Open_Return_FileID */
int __trace_FDI__Transport__F_Open_Return_FileID(void)
{
	return __trace_generic_no_params_func(10,45);
}

/* Report - FDI__Transport__F_Open_Error */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[1];  // specified fixed length
}__trace_desc_t_FDI__Transport__F_Open_Error;

int __trace_FDI__Transport__F_Open_Error(void)
{
	static const __trace_desc_t_FDI__Transport__F_Open_Error localTraceDesc=
	{
		10,46,1,{{DIAG_STRING,1}}
	};
	if(!PASS_FILTER(46)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - FDI__Transport__F_Close_Success */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[1];  // specified fixed length
}__trace_desc_t_FDI__Transport__F_Close_Success;

int __trace_FDI__Transport__F_Close_Success(void)
{
	static const __trace_desc_t_FDI__Transport__F_Close_Success localTraceDesc=
	{
		10,47,1,{{DIAG_LONG,1}}
	};
	if(!PASS_FILTER(47)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - FDI__Transport__F_Close_Error */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[1];  // specified fixed length
}__trace_desc_t_FDI__Transport__F_Close_Error;

int __trace_FDI__Transport__F_Close_Error(void)
{
	static const __trace_desc_t_FDI__Transport__F_Close_Error localTraceDesc=
	{
		10,48,1,{{DIAG_LONG,1}}
	};
	if(!PASS_FILTER(48)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - FDI__Transport__F_Read_Bad_Input */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[2];  // specified fixed length
}__trace_desc_t_FDI__Transport__F_Read_Bad_Input;

int __trace_FDI__Transport__F_Read_Bad_Input(void)
{
	static const __trace_desc_t_FDI__Transport__F_Read_Bad_Input localTraceDesc=
	{
		10,49,2,{{DIAG_SHORT,1},{DIAG_SHORT,1}}
	};
	if(!PASS_FILTER(49)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - FDI__Transport__F_Read_Packet */
int __trace_FDI__Transport__F_Read_Packet(void)
{
	return __trace_generic_no_params_func(10,50);
}

/* Report - FDI__Transport__F_Read_Error */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[1];  // specified fixed length
}__trace_desc_t_FDI__Transport__F_Read_Error;

int __trace_FDI__Transport__F_Read_Error(void)
{
	static const __trace_desc_t_FDI__Transport__F_Read_Error localTraceDesc=
	{
		10,51,1,{{DIAG_LONG,1}}
	};
	if(!PASS_FILTER(51)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - FDI__Transport__F_Read_Malloc_Error */
int __trace_FDI__Transport__F_Read_Malloc_Error(void)
{
	return __trace_generic_no_params_func(10,52);
}

/* Report - FDI__Transport__F_Write_Success */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[1];  // specified fixed length
}__trace_desc_t_FDI__Transport__F_Write_Success;

int __trace_FDI__Transport__F_Write_Success(void)
{
	static const __trace_desc_t_FDI__Transport__F_Write_Success localTraceDesc=
	{
		10,53,1,{{DIAG_SHORT,1}}
	};
	if(!PASS_FILTER(53)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - FDI__Transport__F_Write_Error */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[1];  // specified fixed length
}__trace_desc_t_FDI__Transport__F_Write_Error;

int __trace_FDI__Transport__F_Write_Error(void)
{
	static const __trace_desc_t_FDI__Transport__F_Write_Error localTraceDesc=
	{
		10,54,1,{{DIAG_LONG,1}}
	};
	if(!PASS_FILTER(54)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - FDI__Transport__Format_Error */
int __trace_FDI__Transport__Format_Error(void)
{
	return __trace_generic_no_params_func(10,55);
}

/* Report - FDI__Transport__Remove_Success */
int __trace_FDI__Transport__Remove_Success(void)
{
	return __trace_generic_no_params_func(10,56);
}

/* Report - FDI__Transport__Remove_Error */
int __trace_FDI__Transport__Remove_Error(void)
{
	return __trace_generic_no_params_func(10,57);
}

/* Report - FDI__Transport__FName_LIST_NOTEXISTS */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[1];  // specified fixed length
}__trace_desc_t_FDI__Transport__FName_LIST_NOTEXISTS;

int __trace_FDI__Transport__FName_LIST_NOTEXISTS(void)
{
	static const __trace_desc_t_FDI__Transport__FName_LIST_NOTEXISTS localTraceDesc=
	{
		10,58,1,{{DIAG_STRING,1}}
	};
	if(!PASS_FILTER(58)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - FDI__Transport__F_NameList_Packet */
int __trace_FDI__Transport__F_NameList_Packet(void)
{
	return __trace_generic_no_params_func(10,59);
}

/* Report - FDI__Transport__F_Rename_Succes */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[1];  // specified fixed length
}__trace_desc_t_FDI__Transport__F_Rename_Succes;

int __trace_FDI__Transport__F_Rename_Succes(void)
{
	static const __trace_desc_t_FDI__Transport__F_Rename_Succes localTraceDesc=
	{
		10,60,1,{{DIAG_STRING,1}}
	};
	if(!PASS_FILTER(60)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - FDI__Transport__F_Rename_Error */
int __trace_FDI__Transport__F_Rename_Error(void)
{
	return __trace_generic_no_params_func(10,61);
}

/* Report - FDI__Transport__F_Status_Success */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[1];  // specified fixed length
}__trace_desc_t_FDI__Transport__F_Status_Success;

int __trace_FDI__Transport__F_Status_Success(void)
{
	static const __trace_desc_t_FDI__Transport__F_Status_Success localTraceDesc=
	{
		10,62,1,{{DIAG_SHORT,1}}
	};
	if(!PASS_FILTER(62)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - FDI__Transport__F_Status_Error */
int __trace_FDI__Transport__F_Status_Error(void)
{
	return __trace_generic_no_params_func(10,63);
}

/* Report - FDI__Transport__F_ChangeMode_Success */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[1];  // specified fixed length
}__trace_desc_t_FDI__Transport__F_ChangeMode_Success;

int __trace_FDI__Transport__F_ChangeMode_Success(void)
{
	static const __trace_desc_t_FDI__Transport__F_ChangeMode_Success localTraceDesc=
	{
		10,64,1,{{DIAG_STRING,1}}
	};
	if(!PASS_FILTER(64)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - FDI__Transport__F_ChangeMode_Error */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[2];  // specified fixed length
}__trace_desc_t_FDI__Transport__F_ChangeMode_Error;

int __trace_FDI__Transport__F_ChangeMode_Error(void)
{
	static const __trace_desc_t_FDI__Transport__F_ChangeMode_Error localTraceDesc=
	{
		10,65,2,{{DIAG_STRING,1},{DIAG_LONG,1}}
	};
	if(!PASS_FILTER(65)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - FDI__Transport__Fseek_Success */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[3];  // specified fixed length
}__trace_desc_t_FDI__Transport__Fseek_Success;

int __trace_FDI__Transport__Fseek_Success(void)
{
	static const __trace_desc_t_FDI__Transport__Fseek_Success localTraceDesc=
	{
		10,66,3,{{DIAG_LONG,1},{DIAG_LONG,1},{DIAG_LONG,1}}
	};
	if(!PASS_FILTER(66)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - FDI__Transport__Fseek_Error */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[1];  // specified fixed length
}__trace_desc_t_FDI__Transport__Fseek_Error;

int __trace_FDI__Transport__Fseek_Error(void)
{
	static const __trace_desc_t_FDI__Transport__Fseek_Error localTraceDesc=
	{
		10,67,1,{{DIAG_LONG,1}}
	};
	if(!PASS_FILTER(67)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - FDI__Transport__MaxFileNameLen */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[1];  // specified fixed length
}__trace_desc_t_FDI__Transport__MaxFileNameLen;

int __trace_FDI__Transport__MaxFileNameLen(void)
{
	static const __trace_desc_t_FDI__Transport__MaxFileNameLen localTraceDesc=
	{
		10,68,1,{{DIAG_SHORT,1}}
	};
	if(!PASS_FILTER(68)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - FDI__Transport__FdvSize */
int __trace_FDI__Transport__FdvSize(void)
{
	return __trace_generic_no_params_func(10,69);
}

/* Report - FDI__Transport__FdvSize_Error */
int __trace_FDI__Transport__FdvSize_Error(void)
{
	return __trace_generic_no_params_func(10,70);
}

/* Report - FDI__Transport__FdvAvailableSize */
int __trace_FDI__Transport__FdvAvailableSize(void)
{
	return __trace_generic_no_params_func(10,71);
}

/* Report - FDI__Transport__FdvAvailableSize_Error */
int __trace_FDI__Transport__FdvAvailableSize_Error(void)
{
	return __trace_generic_no_params_func(10,72);
}

/* Report - FDI__Transport__FDI71 */
int __trace_FDI__Transport__FDI71(void)
{
	return __trace_generic_no_params_func(10,73);
}

/* Report - FDI__Transport__FDI721 */
int __trace_FDI__Transport__FDI721(void)
{
	return __trace_generic_no_params_func(10,74);
}

/* Report - FDI__Transport__mkdir_Success */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[1];  // specified fixed length
}__trace_desc_t_FDI__Transport__mkdir_Success;

int __trace_FDI__Transport__mkdir_Success(void)
{
	static const __trace_desc_t_FDI__Transport__mkdir_Success localTraceDesc=
	{
		10,75,1,{{DIAG_STRING,1}}
	};
	if(!PASS_FILTER(75)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - FDI__Transport__mkdir_Error */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[1];  // specified fixed length
}__trace_desc_t_FDI__Transport__mkdir_Error;

int __trace_FDI__Transport__mkdir_Error(void)
{
	static const __trace_desc_t_FDI__Transport__mkdir_Error localTraceDesc=
	{
		10,76,1,{{DIAG_STRING,1}}
	};
	if(!PASS_FILTER(76)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - FDI__Transport__Rmdir_Success */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[1];  // specified fixed length
}__trace_desc_t_FDI__Transport__Rmdir_Success;

int __trace_FDI__Transport__Rmdir_Success(void)
{
	static const __trace_desc_t_FDI__Transport__Rmdir_Success localTraceDesc=
	{
		10,77,1,{{DIAG_STRING,1}}
	};
	if(!PASS_FILTER(77)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - FDI__Transport__Rmdir_Error */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[1];  // specified fixed length
}__trace_desc_t_FDI__Transport__Rmdir_Error;

int __trace_FDI__Transport__Rmdir_Error(void)
{
	static const __trace_desc_t_FDI__Transport__Rmdir_Error localTraceDesc=
	{
		10,78,1,{{DIAG_STRING,1}}
	};
	if(!PASS_FILTER(78)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - FDI__Transport__GetFdiVolumes_List */
int __trace_FDI__Transport__GetFdiVolumes_List(void)
{
	return __trace_generic_no_params_func(10,79);
}

/* Report - FDI__Transport__ExtendetMode */
int __trace_FDI__Transport__ExtendetMode(void)
{
	return __trace_generic_no_params_func(10,80);
}

/* Report - Diag__Utils__SetRTC_Failure */
int __trace_Diag__Utils__SetRTC_Failure(void)
{
	return __trace_generic_no_params_func(10,81);
}

/* Report - Diag__Utils__SetRTC_Success */
int __trace_Diag__Utils__SetRTC_Success(void)
{
	return __trace_generic_no_params_func(10,82);
}

/* Report - Diag__Utils__ReadRTC_Calendar */
int __trace_Diag__Utils__ReadRTC_Calendar(void)
{
	return __trace_generic_no_params_func(10,83);
}

/* Report - Diag__Utils__ReadRTC_Success */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[1];  // specified fixed length
}__trace_desc_t_Diag__Utils__ReadRTC_Success;

int __trace_Diag__Utils__ReadRTC_Success(void)
{
	static const __trace_desc_t_Diag__Utils__ReadRTC_Success localTraceDesc=
	{
		10,84,1,{{DIAG_STRING,1}}
	};
	if(!PASS_FILTER(84)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - Diag__Utils__SWVersion */
int __trace_Diag__Utils__SWVersion(void)
{
	return __trace_generic_no_params_func(10,85);
}

/* Report - VALI_IF__MISC_IF__RESET_IF_Info */
int __trace_VALI_IF__MISC_IF__RESET_IF_Info(void)
{
	return __trace_generic_no_params_func(10,86);
}

/* Report - VALI_IF__MISC_IF__RESET_IF_Fail */
int __trace_VALI_IF__MISC_IF__RESET_IF_Fail(void)
{
	return __trace_generic_no_params_func(10,87);
}

/* Report - VALI_IF__MISC_IF__RESET_IF_Timeout */
int __trace_VALI_IF__MISC_IF__RESET_IF_Timeout(void)
{
	return __trace_generic_no_params_func(10,88);
}

/* Report - VALI_IF__CT_P_API__INIT_SUCCESS */
int __trace_VALI_IF__CT_P_API__INIT_SUCCESS(void)
{
	return __trace_generic_no_params_func(10,89);
}

/* Report - VALI_IF__CT_P_API__TERM_SUCCESS */
int __trace_VALI_IF__CT_P_API__TERM_SUCCESS(void)
{
	return __trace_generic_no_params_func(10,90);
}

/* Report - Validation__Utils__Export_IPMDrv_list */
int __trace_Validation__Utils__Export_IPMDrv_list(void)
{
	return __trace_generic_no_params_func(10,91);
}

/* Report - Diag__Utils__battery_cali_ok */
int __trace_Diag__Utils__battery_cali_ok(void)
{
	return __trace_generic_no_params_func(10,92);
}

/* Report - Diag__Utils__battery_cali_error */
int __trace_Diag__Utils__battery_cali_error(void)
{
	return __trace_generic_no_params_func(10,93);
}

/* Report - Diag__Utils__battery_cali_debug */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[1];  // specified fixed length
}__trace_desc_t_Diag__Utils__battery_cali_debug;

int __trace_Diag__Utils__battery_cali_debug(void)
{
	static const __trace_desc_t_Diag__Utils__battery_cali_debug localTraceDesc=
	{
		10,94,1,{{DIAG_STRING,1}}
	};
	if(!PASS_FILTER(94)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - Diag__Utils__battery_calibration */
int __trace_Diag__Utils__battery_calibration(void)
{
	return __trace_generic_no_params_func(10,95);
}

/* Report - Diag__Utils__bettery_calibration_vbat */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[1];  // specified fixed length
}__trace_desc_t_Diag__Utils__bettery_calibration_vbat;

int __trace_Diag__Utils__bettery_calibration_vbat(void)
{
	static const __trace_desc_t_Diag__Utils__bettery_calibration_vbat localTraceDesc=
	{
		10,96,1,{{DIAG_SHORT,1}}
	};
	if(!PASS_FILTER(96)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - Diag__Utils__battery_calibration_ibat */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[1];  // specified fixed length
}__trace_desc_t_Diag__Utils__battery_calibration_ibat;

int __trace_Diag__Utils__battery_calibration_ibat(void)
{
	static const __trace_desc_t_Diag__Utils__battery_calibration_ibat localTraceDesc=
	{
		10,97,1,{{DIAG_SHORT,1}}
	};
	if(!PASS_FILTER(97)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - Diag__comMem__image */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[3];  // specified fixed length
}__trace_desc_t_Diag__comMem__image;

int __trace_Diag__comMem__image(void)
{
	static const __trace_desc_t_Diag__comMem__image localTraceDesc=
	{
		12,98,3,{{DIAG_LONG,1},{DIAG_LONG,1},{DIAG_LONG,1}}
	};
	if(!PASS_FILTER(98)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - Diag__comMem__imageNO */
int __trace_Diag__comMem__imageNO(void)
{
	return __trace_generic_no_params_func(12,99);
}

/* Report - SW_PLAT__PERFORMANCE__SAVE2FILE0 */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[1];  // specified fixed length
}__trace_desc_t_SW_PLAT__PERFORMANCE__SAVE2FILE0;

int __trace_SW_PLAT__PERFORMANCE__SAVE2FILE0(void)
{
	static const __trace_desc_t_SW_PLAT__PERFORMANCE__SAVE2FILE0 localTraceDesc=
	{
		13,100,1,{{DIAG_STRING,1}}
	};
	if(!PASS_FILTER(100)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - SW_PLAT__PERFORMANCE__SAVE2FILE */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[2];  // specified fixed length
}__trace_desc_t_SW_PLAT__PERFORMANCE__SAVE2FILE;

int __trace_SW_PLAT__PERFORMANCE__SAVE2FILE(void)
{
	static const __trace_desc_t_SW_PLAT__PERFORMANCE__SAVE2FILE localTraceDesc=
	{
		13,101,2,{{DIAG_STRING,1},{DIAG_LONG,1}}
	};
	if(!PASS_FILTER(101)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - Diag__comMem__EE_LOG_TXT */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[1];  // specified fixed length
}__trace_desc_t_Diag__comMem__EE_LOG_TXT;

int __trace_Diag__comMem__EE_LOG_TXT(void)
{
	static const __trace_desc_t_Diag__comMem__EE_LOG_TXT localTraceDesc=
	{
		13,102,1,{{DIAG_STRING,1}}
	};
	if(!PASS_FILTER(102)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - Diag__comMem__EE_LOG */
int __trace_Diag__comMem__EE_LOG(void)
{
	return __trace_generic_no_params_func(13,103);
}

/* Report - Diag__comMem__EE_LOG_NOERR */
int __trace_Diag__comMem__EE_LOG_NOERR(void)
{
	return __trace_generic_no_params_func(13,104);
}

/* Report - Diag__comMem__DDR_RW */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[1];  // specified fixed length
}__trace_desc_t_Diag__comMem__DDR_RW;

int __trace_Diag__comMem__DDR_RW(void)
{
	static const __trace_desc_t_Diag__comMem__DDR_RW localTraceDesc=
	{
		13,105,1,{{DIAG_LONG,1}}
	};
	if(!PASS_FILTER(105)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - Diag__comMem__DDR_DiagOffline */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[1];  // specified fixed length
}__trace_desc_t_Diag__comMem__DDR_DiagOffline;

int __trace_Diag__comMem__DDR_DiagOffline(void)
{
	static const __trace_desc_t_Diag__comMem__DDR_DiagOffline localTraceDesc=
	{
		13,106,1,{{DIAG_LONG,1}}
	};
	if(!PASS_FILTER(106)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - AUDIO__ACM__ACMAudioPathEnable */
int __trace_AUDIO__ACM__ACMAudioPathEnable(void)
{
	return __trace_generic_no_params_func(14,107);
}

/* Report - AUDIO__ACM__ACMAudioPathDisable */
int __trace_AUDIO__ACM__ACMAudioPathDisable(void)
{
	return __trace_generic_no_params_func(14,108);
}

/* Report - AUDIO__ACM__ACMAudioDeviceEnable */
int __trace_AUDIO__ACM__ACMAudioDeviceEnable(void)
{
	return __trace_generic_no_params_func(14,109);
}

/* Report - AUDIO__ACM__ACMAudioDeviceDisable */
int __trace_AUDIO__ACM__ACMAudioDeviceDisable(void)
{
	return __trace_generic_no_params_func(14,110);
}

/* Report - AUDIO__ACM__ACMAudioDeviceMute */
int __trace_AUDIO__ACM__ACMAudioDeviceMute(void)
{
	return __trace_generic_no_params_func(14,111);
}

/* Report - AUDIO__ACM__AACMAudioDeviceVolumeSet */
int __trace_AUDIO__ACM__AACMAudioDeviceVolumeSet(void)
{
	return __trace_generic_no_params_func(14,112);
}

/* Report - AUDIO__ACM__ACMAudioSetCalibrationStateSet */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[1];  // specified fixed length
}__trace_desc_t_AUDIO__ACM__ACMAudioSetCalibrationStateSet;

int __trace_AUDIO__ACM__ACMAudioSetCalibrationStateSet(void)
{
	static const __trace_desc_t_AUDIO__ACM__ACMAudioSetCalibrationStateSet localTraceDesc=
	{
		14,113,1,{{DIAG_STRING,1}}
	};
	if(!PASS_FILTER(113)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - EE_HANDLER__EE_LOG__logOpen */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[1];  // specified fixed length
}__trace_desc_t_EE_HANDLER__EE_LOG__logOpen;

int __trace_EE_HANDLER__EE_LOG__logOpen(void)
{
	static const __trace_desc_t_EE_HANDLER__EE_LOG__logOpen localTraceDesc=
	{
		15,114,1,{{DIAG_STRING,1}}
	};
	if(!PASS_FILTER(114)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - EE_HANDLER__EE_LOG__logBlock */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[2];  // specified fixed length
}__trace_desc_t_EE_HANDLER__EE_LOG__logBlock;

int __trace_EE_HANDLER__EE_LOG__logBlock(void)
{
	static const __trace_desc_t_EE_HANDLER__EE_LOG__logBlock localTraceDesc=
	{
		15,115,2,{{DIAG_STRING,1},{DIAG_SHORT,1}}
	};
	if(!PASS_FILTER(115)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - EE_HANDLER__EE_LOG__logClose */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[1];  // specified fixed length
}__trace_desc_t_EE_HANDLER__EE_LOG__logClose;

int __trace_EE_HANDLER__EE_LOG__logClose(void)
{
	static const __trace_desc_t_EE_HANDLER__EE_LOG__logClose localTraceDesc=
	{
		15,116,1,{{DIAG_STRING,1}}
	};
	if(!PASS_FILTER(116)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - EE_HANDLER__EE_LOG__logDbg0 */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[2];  // specified fixed length
}__trace_desc_t_EE_HANDLER__EE_LOG__logDbg0;

int __trace_EE_HANDLER__EE_LOG__logDbg0(void)
{
	static const __trace_desc_t_EE_HANDLER__EE_LOG__logDbg0 localTraceDesc=
	{
		15,117,2,{{DIAG_SHORT,1},{DIAG_SHORT,1}}
	};
	if(!PASS_FILTER(117)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - EE_HANDLER__EE_LOG__logDbg1 */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[2];  // specified fixed length
}__trace_desc_t_EE_HANDLER__EE_LOG__logDbg1;

int __trace_EE_HANDLER__EE_LOG__logDbg1(void)
{
	static const __trace_desc_t_EE_HANDLER__EE_LOG__logDbg1 localTraceDesc=
	{
		15,118,2,{{DIAG_SHORT,1},{DIAG_SHORT,1}}
	};
	if(!PASS_FILTER(118)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - EE_HANDLER__EE_LOG__logDbg2 */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[2];  // specified fixed length
}__trace_desc_t_EE_HANDLER__EE_LOG__logDbg2;

int __trace_EE_HANDLER__EE_LOG__logDbg2(void)
{
	static const __trace_desc_t_EE_HANDLER__EE_LOG__logDbg2 localTraceDesc=
	{
		15,119,2,{{DIAG_SHORT,1},{DIAG_SHORT,1}}
	};
	if(!PASS_FILTER(119)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - EE_HANDLER__EE_LOG__logDbg3 */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[2];  // specified fixed length
}__trace_desc_t_EE_HANDLER__EE_LOG__logDbg3;

int __trace_EE_HANDLER__EE_LOG__logDbg3(void)
{
	static const __trace_desc_t_EE_HANDLER__EE_LOG__logDbg3 localTraceDesc=
	{
		15,120,2,{{DIAG_SHORT,1},{DIAG_SHORT,1}}
	};
	if(!PASS_FILTER(120)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - EE_HANDLER__EE_LOG__logDbg4 */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[2];  // specified fixed length
}__trace_desc_t_EE_HANDLER__EE_LOG__logDbg4;

int __trace_EE_HANDLER__EE_LOG__logDbg4(void)
{
	static const __trace_desc_t_EE_HANDLER__EE_LOG__logDbg4 localTraceDesc=
	{
		15,121,2,{{DIAG_SHORT,1},{DIAG_SHORT,1}}
	};
	if(!PASS_FILTER(121)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - EE_HANDLER__EE_LOG__NoLog */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[1];  // specified fixed length
}__trace_desc_t_EE_HANDLER__EE_LOG__NoLog;

int __trace_EE_HANDLER__EE_LOG__NoLog(void)
{
	static const __trace_desc_t_EE_HANDLER__EE_LOG__NoLog localTraceDesc=
	{
		15,122,1,{{DIAG_STRING,1}}
	};
	if(!PASS_FILTER(122)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - EE_HANDLER__EE_LOG__ABNORMAL */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[1];  // specified fixed length
}__trace_desc_t_EE_HANDLER__EE_LOG__ABNORMAL;

int __trace_EE_HANDLER__EE_LOG__ABNORMAL(void)
{
	static const __trace_desc_t_EE_HANDLER__EE_LOG__ABNORMAL localTraceDesc=
	{
		15,123,1,{{DIAG_STRING,1}}
	};
	if(!PASS_FILTER(123)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - EE_HANDLER__EE_LOG__STALL */
typedef struct
{
	INT16 moduleID;
	UINT16 reportID;
	UINT16 numOfParams;
	ParameterInfo parameters[1];  // specified fixed length
}__trace_desc_t_EE_HANDLER__EE_LOG__STALL;

int __trace_EE_HANDLER__EE_LOG__STALL(void)
{
	static const __trace_desc_t_EE_HANDLER__EE_LOG__STALL localTraceDesc=
	{
		15,124,1,{{DIAG_STRING,1}}
	};
	if(!PASS_FILTER(124)) return DIAG_INVALID_MODULE_ID;
	diag_lock_G();
	diagTraceDesc = (DiagTraceDescriptor*)&localTraceDesc;
	return 1;
}

/* Report - EE_HANDLER__EE__EE_CONFIG_PRINT */
int __trace_EE_HANDLER__EE__EE_CONFIG_PRINT(void)
{
	return __trace_generic_no_params_func(16,125);
}

/* Report - EE_HANDLER__EE__EE_CONFIG_OK */
int __trace_EE_HANDLER__EE__EE_CONFIG_OK(void)
{
	return __trace_generic_no_params_func(16,126);
}


#undef DIAG_DB_C

