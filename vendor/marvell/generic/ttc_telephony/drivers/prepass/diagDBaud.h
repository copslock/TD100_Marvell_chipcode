#if !defined DIAG_DB_H
#define DIAG_DB_H

#include "diag.h"

#ifdef	DIAG_DB_C
DIAG_EXPORT const DiagDBVersion DBversionID;
DIAG_EXPORT UINT8 g_OverridePassFilter;
DIAG_EXPORT UINT8 filterArray[(531+8)/8];
DIAG_EXPORT const DiagTraceDescriptor* diagTraceDesc;
DIAG_EXPORT const UINT32 _FirstCommandID;
DIAG_EXPORT const UINT32 _FirstReportID;
DIAG_EXPORT const UINT32 _LastCommandID;
DIAG_EXPORT const UINT32 _LastReportID;
#else
DIAG_IMPORT const DiagDBVersion DBversionID;
DIAG_IMPORT UINT8 g_OverridePassFilter;
DIAG_IMPORT UINT8 filterArray[(531+8)/8];
DIAG_IMPORT const DiagTraceDescriptor* diagTraceDesc;
DIAG_IMPORT const UINT32 _FirstCommandID;
DIAG_IMPORT const UINT32 _FirstReportID;
DIAG_IMPORT const UINT32 _LastCommandID;
DIAG_IMPORT const UINT32 _LastReportID;
#endif //DIAG_DB_C

DIAG_EXPORT	int             __trace_Diag__Debug__UDPicatReadyOn_1 (void);
DIAG_EXPORT	int             __trace_Diag__Debug__UDPicatReadyOff_2 (void);
DIAG_EXPORT	int             __trace_SW_PLAT__DIAG__ERROR_PRINTFTYPE (void);
DIAG_EXPORT	int             __trace_SW_PLAT__DIAG__PRINTF_ERROR_IN_PARAMS_NUMBER (void);
DIAG_EXPORT	int             __trace_Audio__HW__AUDIO_HW_PathsStatus (void);
DIAG_EXPORT	int             __trace_Audio__Organ__AUDIO_ORGAN_SlopeOffset (void);
DIAG_EXPORT	int             __trace_Audio__Organ__AUDIO_ORGAN_ReadFdiDataOK (void);
DIAG_EXPORT	int             __trace_Audio__Organ__AUDIO_ORGAN_ReadFdiDataERROR (void);
DIAG_EXPORT	int             __trace_Audio__HW__AUDIO_HW_PathEnableIgnore (void);
DIAG_EXPORT	int             __trace_Audio__HW__AUDIO_HW_PathEnableOK (void);
DIAG_EXPORT	int             __trace_Audio__HW__AUDIO_HW_PathEnableSTATUSERROR (void);
DIAG_EXPORT	int             __trace_Audio__HW__AUDIO_HW_PathDisableOK (void);
DIAG_EXPORT	int             __trace_Audio__HW__AUDIO_HW_PathMuteOK (void);
DIAG_EXPORT	int             __trace_VALI_IF__ATCMD_IF__ATOUT_CHAR (void);
DIAG_EXPORT	int             __trace_VALI_IF__ATCMD_IF__ATOUT (void);
DIAG_EXPORT	int             __trace_Diag__Utils__Print_RTC (void);
DIAG_EXPORT	int             __trace_Diag__Utils__ReadRTC_Failure (void);
DIAG_EXPORT	int             __trace_VALI_IF__ATCMD_IF__ALREADYRUNNING (void);
DIAG_EXPORT	int             __trace_VALI_IF__ATCMD_IF__STOPIFNOTSTARTED (void);
DIAG_EXPORT	int             __trace_VALI_IF__ATCMD_IF__ATCMDIFNOTSTARTEDINSENDAT (void);
DIAG_EXPORT	int             __trace_VALI_IF__ATCMD_IF__ATCMDIFNOTSTARTEDINSENDATCHAR (void);
DIAG_EXPORT	int             __trace_VALI_IF__MISC_IF__ATCMDSYSTEM (void);
DIAG_EXPORT	int             __trace_DIAG__Dispatcher__ExportedPtr (void);
DIAG_EXPORT	int             __trace_SW_PLAT__DIAG__COMMANDID_WRONG (void);
DIAG_EXPORT	int             __trace_SW_PLAT__DIAG__RX_SERVICEID_WRONG (void);
DIAG_EXPORT	int             __trace_SYSTEM__PROD__AT_SER_OK (void);
DIAG_EXPORT	int             __trace_AUDIO__ACM__ACMAudioTEST (void);
DIAG_EXPORT	int             __trace_AUDIO__ACM__StructAudioTEST (void);

#endif //DIAG_DB_H
