//PPL Source File Name : ../errorhandler/src/EE_Postmortem.i
//PPL Source File Name : src/EE_Postmortem.c
typedef int ptrdiff_t ;typedef unsigned int size_t ;typedef unsigned int wchar_t ;typedef signed char __int8_t ;typedef unsigned char __uint8_t ;typedef short __int16_t ;typedef unsigned short __uint16_t ;typedef int __int32_t ;typedef unsigned int __uint32_t ;typedef long long __int64_t ;typedef unsigned long long __uint64_t ;typedef __int8_t __int_least8_t ;typedef __uint8_t __uint_least8_t ;typedef __int16_t __int_least16_t ;typedef __uint16_t __uint_least16_t ;typedef __int32_t __int_least32_t ;typedef __uint32_t __uint_least32_t ;typedef __int64_t __int_least64_t ;typedef __uint64_t __uint_least64_t ;typedef __int32_t __int_fast8_t ;typedef __uint32_t __uint_fast8_t ;typedef __int32_t __int_fast16_t ;typedef __uint32_t __uint_fast16_t ;typedef __int32_t __int_fast32_t ;typedef __uint32_t __uint_fast32_t ;typedef __int64_t __int_fast64_t ;typedef __uint64_t __uint_fast64_t ;typedef int __intptr_t ;typedef unsigned int __uintptr_t ;typedef __int64_t __intmax_t ;typedef __uint64_t __uintmax_t ;typedef __int32_t __register_t ;typedef unsigned long __vaddr_t ;typedef unsigned long __paddr_t ;typedef unsigned long __vsize_t ;typedef unsigned long __psize_t ;typedef int __clock_t ;typedef int __clockid_t ;typedef long __ptrdiff_t ;typedef int __time_t ;typedef int __timer_t ;typedef __builtin_va_list __va_list ;typedef int __wchar_t ;typedef int __wint_t ;typedef int __rune_t ;typedef void * __wctrans_t ;typedef void * __wctype_t ;typedef unsigned long __cpuid_t ;typedef __int32_t __dev_t ;typedef __uint32_t __fixpt_t ;typedef __uint32_t __gid_t ;typedef __uint32_t __id_t ;typedef __uint32_t __in_addr_t ;typedef __uint16_t __in_port_t ;typedef __uint32_t __ino_t ;typedef long __key_t ;typedef __uint32_t __mode_t ;typedef __uint32_t __nlink_t ;typedef __int32_t __pid_t ;typedef __uint64_t __rlim_t ;typedef __uint16_t __sa_family_t ;typedef __int32_t __segsz_t ;typedef __uint32_t __socklen_t ;typedef __int32_t __swblk_t ;typedef __uint32_t __uid_t ;typedef __uint32_t __useconds_t ;typedef __int32_t __suseconds_t ;typedef union {
 char __mbstate8 [ 128 ] ;
 __int64_t __mbstateL ;
 } __mbstate_t ;typedef __builtin_va_list __gnuc_va_list ;typedef __int8_t int8_t ;typedef __uint8_t uint8_t ;typedef __int16_t int16_t ;typedef __uint16_t uint16_t ;typedef __int32_t int32_t ;typedef __uint32_t uint32_t ;typedef __int64_t int64_t ;typedef __uint64_t uint64_t ;typedef int8_t int_least8_t ;typedef int8_t int_fast8_t ;typedef uint8_t uint_least8_t ;typedef uint8_t uint_fast8_t ;typedef int16_t int_least16_t ;typedef int32_t int_fast16_t ;typedef uint16_t uint_least16_t ;typedef uint32_t uint_fast16_t ;typedef int32_t int_least32_t ;typedef int32_t int_fast32_t ;typedef uint32_t uint_least32_t ;typedef uint32_t uint_fast32_t ;typedef int64_t int_least64_t ;typedef int64_t int_fast64_t ;typedef uint64_t uint_least64_t ;typedef uint64_t uint_fast64_t ;typedef int intptr_t ;typedef unsigned int uintptr_t ;typedef uint64_t uintmax_t ;typedef int64_t intmax_t ;typedef long int ssize_t ;typedef void ( *__kernel_sighandler_t ) ( int ) ;typedef int __kernel_key_t ;typedef int __kernel_mqd_t ;typedef unsigned long __kernel_ino_t ;typedef unsigned short __kernel_mode_t ;typedef unsigned short __kernel_nlink_t ;typedef long __kernel_off_t ;typedef int __kernel_pid_t ;typedef unsigned short __kernel_ipc_pid_t ;typedef unsigned short __kernel_uid_t ;typedef unsigned short __kernel_gid_t ;typedef unsigned int __kernel_size_t ;typedef int __kernel_ssize_t ;typedef int __kernel_ptrdiff_t ;typedef long __kernel_time_t ;typedef long __kernel_suseconds_t ;typedef long __kernel_clock_t ;typedef int __kernel_timer_t ;typedef int __kernel_clockid_t ;typedef int __kernel_daddr_t ;typedef char * __kernel_caddr_t ;typedef unsigned short __kernel_uid16_t ;typedef unsigned short __kernel_gid16_t ;typedef unsigned int __kernel_uid32_t ;typedef unsigned int __kernel_gid32_t ;typedef unsigned short __kernel_old_uid_t ;typedef unsigned short __kernel_old_gid_t ;typedef unsigned short __kernel_old_dev_t ;typedef long long __kernel_loff_t ;typedef unsigned short umode_t ;typedef __signed__ char __s8 ;typedef unsigned char __u8 ;typedef __signed__ short __s16 ;typedef unsigned short __u16 ;typedef __signed__ int __s32 ;typedef unsigned int __u32 ;typedef __signed__ long long __s64 ;typedef unsigned long long __u64 ;typedef __u16 __le16 ;typedef __u16 __be16 ;typedef __u32 __le32 ;typedef __u32 __be32 ;typedef __u64 __le64 ;typedef __u64 __be64 ;typedef unsigned long __kernel_blkcnt_t ;typedef unsigned long __kernel_blksize_t ;typedef unsigned long __kernel_fsblkcnt_t ;typedef unsigned long __kernel_fsfilcnt_t ;typedef unsigned int __kernel_id_t ;typedef __u32 __kernel_dev_t ;typedef __kernel_blkcnt_t blkcnt_t ;typedef __kernel_blksize_t blksize_t ;typedef __kernel_clock_t clock_t ;typedef __kernel_clockid_t clockid_t ;typedef __kernel_dev_t dev_t ;typedef __kernel_fsblkcnt_t fsblkcnt_t ;typedef __kernel_fsfilcnt_t fsfilcnt_t ;typedef __kernel_gid32_t gid_t ;typedef __kernel_id_t id_t ;typedef __kernel_ino_t ino_t ;typedef __kernel_key_t key_t ;typedef __kernel_mode_t mode_t ;typedef __kernel_nlink_t nlink_t ;typedef __kernel_off_t off_t ;typedef __kernel_loff_t loff_t ;typedef loff_t off64_t ;typedef __kernel_pid_t pid_t ;typedef __kernel_suseconds_t suseconds_t ;typedef __kernel_time_t time_t ;typedef __kernel_uid32_t uid_t ;typedef signed long useconds_t ;typedef __kernel_daddr_t daddr_t ;typedef __kernel_timer_t timer_t ;typedef __kernel_mqd_t mqd_t ;typedef __kernel_caddr_t caddr_t ;typedef unsigned int uint_t ;typedef unsigned int uint ;typedef unsigned char u_char ;typedef unsigned short u_short ;typedef unsigned int u_int ;typedef unsigned long u_long ;typedef uint32_t u_int32_t ;typedef uint16_t u_int16_t ;typedef uint8_t u_int8_t ;typedef uint64_t u_int64_t ;typedef off_t fpos_t ;typedef __gnuc_va_list va_list ;typedef unsigned char BOOL ;typedef unsigned char UINT8 ;typedef unsigned short UINT16 ;typedef unsigned int UINT32 ;typedef signed char CHAR ;typedef signed char INT8 ;typedef signed short INT16 ;typedef signed int INT32 ;typedef unsigned short WORD ;typedef unsigned int DWORD ;typedef int HANDLE ;typedef HANDLE* LPHANDLE ;typedef unsigned char* PUINT8 ;typedef long LONG ;typedef char* LPCTSTR ;typedef char* LPTSTR ;typedef void* LPVOID ;typedef unsigned int* LPDWORD ;typedef unsigned int* PDWORD ;typedef unsigned int* PUINT32 ;typedef unsigned short TCHAR ;typedef unsigned int UINT ;typedef INT32 *PINT32 ;typedef INT16 *PINT16 ;typedef UINT16 *PUINT16 ;typedef INT8 *PINT8 ;typedef const char * SwVersion ;//ICAT EXPORTED STRUCT 
 typedef struct 
 {	 
 char name [ ( 7 + 1 ) ] ; // 8 bytes Name for the buffer to be accessed or saved into file	 
 UINT8* bufAddr ; // 4 bytes pointer to the buffer to be accessed or saved into file	 
 UINT32 bufLen ; // 4 bytes length of the buffer to be accessed or saved into file	 
 } EE_PostmortemDesc_Entry;

typedef union sigval {
 int sival_int ;
 void *sival_ptr ;
 } sigval_t ;//ICAT EXPORTED STRUCT 
 typedef struct 
 {	 
 UINT8 second ; // Seconds after minute: 0 - 59	 
 UINT8 minute ; // Minutes after hour: 0 - 59	 
 UINT8 hour ; // Hours after midnight: 0 - 23	 
 UINT8 day ; // Day of month: 1 - 31	 
 UINT8 month ; // Month of year: 1 - 12	 
 UINT16 year ; // Calendar year: e.g 2001	 
 } RTC_CalendarTime;

typedef UINT8 RTC_Handle ;typedef void ( *RTC_ISR ) ( void ) ;typedef void ( *RTCOnTimeSetNotifyCallback ) ( RTC_CalendarTime* oldTime , RTC_CalendarTime* newTime ) ;//ICAT EXPORTED ENUM 
 typedef enum 
 {	 
 // enumerated	 
 EE_SYS_RESET_EN ,	 
 EE_ASSERT_EN ,	 
 EE_EXCEPTION_EN ,	 
 EE_WARNING_EN ,	 
 EE_NUM_ENTRY_TYPES ,	 
 // Codes	 
 EE_SYS_RESET = 300 ,	 
 EE_ASSERT = 350 ,	 
 EE_EXCEPTION = 450 ,	 
 EE_WARNING = 550	 
	 
	 
	 
 } EE_entry_t_;

typedef UINT16 EE_entry_t ;//ICAT EXPORTED ENUM 
 typedef enum 
 {	 
 EEE_DataAbort ,	 
 EEE_PrefetchAbort ,	 
 EEE_FatalError ,	 
 EEE_SWInterrupt ,	 
 EEE_UndefInst ,	 
 EEE_ReservedInt	 
	 
	 
	 
 } EE_ExceptionType_t_;

typedef UINT8 EE_ExceptionType_t ;//ICAT EXPORTED ENUM 
 typedef enum 
 {	 
 EE_NO_RESET_SOURCE ,	 
	 
	 
	 
 EE_POWER_ON_RESET ,	 
	 
 EE_EXT_MASTER_RESET ,	 
	 
	 
	 
 EE_WDT_RESET	 
	 
	 
	 
	 
 } EE_PMU_t_;

typedef UINT8 EE_PMU_t ;//ICAT EXPORTED STRUCT 
 typedef struct 
 {	 
 UINT32 r0 ; /* register r0 contents */	 
 UINT32 r1 ; /* register r1 contents */	 
 UINT32 r2 ; /* register r2 contents */	 
 UINT32 r3 ; /* register r3 contents */	 
 UINT32 r4 ; /* register r4 contents */	 
 UINT32 r5 ; /* register r5 contents */	 
 UINT32 r6 ; /* register r6 contents */	 
 UINT32 r7 ; /* register r7 contents */	 
 UINT32 r8 ; /* register r8 contents */	 
 UINT32 r9 ; /* register r9 contents */	 
 UINT32 r10 ; /* register r10 contents */	 
 UINT32 r11 ; /* register r11 contents */	 
 UINT32 r12 ; /* register r12 contents */	 
 UINT32 SP ; /* register r13 contents */	 
 UINT32 LR ; /* register r14 contents ( excepted mode ) */	 
 UINT32 PC ; /* PC - excepted instruction */	 
 UINT32 cpsr ; /* saved program status register contents */	 
 UINT32 FSR ; /* Fault status register */	 
 UINT32 FAR_R ; /* Fault address register */	 
 EE_PMU_t PMU_reg ; /* saved reset cause - should be last */	 
	 
 // UINT32 PESR ; / * Extension * /	 
 // UINT32 XESR ;	 
 // UINT32 PEAR ;	 
 // UINT32 FEAR ;	 
 // UINT32 SEAR ;	 
 // UINT32 GEAR ;	 
 } EE_RegInfo_Data_t;

//ICAT EXPORTED ENUM 
 typedef enum 
 {	 
 EE_CT_None ,	 
 EE_CT_ExecTrace ,	 
 EE_CT_StackDump	 
	 
	 
	 
 } EE_ContextType_t_;

typedef UINT8 EE_ContextType_t ;//ICAT EXPORTED ENUM 
 typedef enum 
 {	 
 EE_CDT_None ,	 
 EE_CDT_ExecTrace ,	 
 EE_CDT_StackDump ,	 
 EE_CDT_UserDefined = 0x10	 
	 
	 
	 
 } EE_ContextDataType_t_;

typedef UINT8 EE_ContextDataType_t ;//ICAT EXPORTED STRUCT 
 typedef struct 
 {	 
 UINT32 _PESR ;	 
 UINT32 _XESR ;	 
 UINT32 _PEAR ;	 
 UINT32 _FEAR ;	 
 UINT32 _SEAR ;	 
 UINT32 _GEAR ;	 
 } EE_XscGasketRegs;

//ICAT EXPORTED STRUCT 
 typedef struct 
 {	 
 UINT16 fileWriteOffset ; // DO NOT REMOVE OR CHANGE TYPE!!! ( for cyclic file )	 
 EE_entry_t type ;	 
 RTC_CalendarTime dateAndTime ;	 
 char desc [ 100 ] ; /* Description string size =ERROR_HANDLER_MAX_DESC_SIZE*/	 
 EE_RegInfo_Data_t RegInfo ;	 
 EE_ContextDataType_t contextBufferType ;	 
 UINT8 contextBuffer [ 256 * 2 ] ;	 
 UINT32 CHKPT0 ;	 
 UINT32 CHKPT1 ;	 
 char taskName [ 10 ] ;	 
 UINT32 taskStackStart ;	 
 UINT32 taskStackEnd ;	 
 // UP TO HERE 0x1e4 bytes ( out of 0x200 allocated by linker control file INT_RAM_EE segment )	 
 EE_XscGasketRegs xscaleGasketRegs ;	 
 UINT32 warningCntr ; // reserved [ 1 ] ;	 
 } EE_Entry_t;

typedef void voidPFuncVoid ( void ) ;typedef void ( *ExceptionHendler ) ( EE_RegInfo_Data_t* ) ;typedef EEHandlerAction ( *ExceptionHendlerExt ) ( EE_ExceptionType_t type , EE_RegInfo_Data_t* ) ;typedef unsigned long sigset_t ;typedef void __signalfn_t ( int ) ;typedef __signalfn_t *__sighandler_t ;typedef void __restorefn_t ( void ) ;typedef __restorefn_t *__sigrestore_t ;typedef int sig_atomic_t ;typedef void ( *sig_t ) ( int ) ;typedef sig_t sighandler_t ;typedef __kernel_fd_set fd_set ;//ICAT EXPORTED STRUCT 
 typedef struct NVM_Header 
 {	 
 unsigned long StructSize ; // the size of the user structure below	 
 unsigned long NumofStructs ; // >1 in case of array of structs ( default is 1 ) .	 
 char StructName [ 64 ] ; // the name of the user structure below	 
 char Date [ 32 ] ; // date updated by the ICAT when the file is saved. Filled by calibration SW.	 
 char time [ 32 ] ; // time updated by the ICAT when the file is saved. Filled by calibration SW.	 
 char Version [ 64 ] ; // user version - this field is updated by the SW eng. Every time they update the UserStruct.	 
 char HW_ID [ 32 ] ; // signifies the board number. Filled by calibration SW.	 
 char CalibVersion [ 32 ] ; // signifies the calibration SW version. Filled by calibration SW.	 
 } NVM_Header_ts;

//ICAT EXPORTED ENUM 
 typedef enum 
 {	 
 EE_RESET ,	 
 EE_STALL ,	 
 EE_RESERVED ,	 
 EE_EXTERNAL ,	 
 EE_RESET_START_BASIC ,	 
 EE_RESET_COMM_SILENT	 
	 
	 
	 
 } FinalAct_t_;

typedef UINT8 FinalAct_t ;//ICAT EXPORTED ENUM 
 typedef enum 
 {	 
 EE_OFF ,	 
 EE_ON	 
	 
	 
	 
 } EE_OnOff_t_;

typedef UINT8 EE_OnOff_t ;//ICAT EXPORTED ENUM 
 typedef enum 
 {	 
 EE_WARN_OFF ,	 
 EE_WARN_ASSERTCONTINUE ,	 
 EE_WARN_ASSERT	 
	 
	 
	 
 } EE_WarningOn_t_;

typedef UINT8 EE_WarningOn_t ;//ICAT EXPORTED ENUM 
 typedef enum 
 {	 
 EE_VER_3 = 3 ,	 
 EE_VER	 
	 
	 
	 
 } EE_Version_t_;

typedef UINT8 EE_Version_t ;//ICAT EXPORTED ENUM 
 typedef enum 
 {	 
 EE_HSL_OFF = 0 ,	 
 EE_HSL_1_8V = 1 ,	 
 EE_HSL_3V = 3	 
	 
	 
	 
 } EE_HSL_V_t_;

typedef UINT8 EE_HSL_V_t ;//ICAT EXPORTED ENUM 
 typedef enum 
 {	 
 EE_WDT_OFF = 0 ,	 
 EE_WDT_TIME_6SEC = 6000 , // in miliseconds ; for max WCDMA / GSM DRX cycle	 
 EE_WDT_TIME_7SEC = 7000 , // in miliseconds	 
 EE_WDT_TIME_8SEC = 8000 , // in miliseconds	 
 EE_WDT_TIME_10SEC = 10000 , // in miliseconds	 
 EE_WDT_TIME_20SEC = 20000 , // in miliseconds	 
 EE_WDT_TIME_30SEC = 30000 , // in miliseconds	 
 EE_WDT_TIME_MAX = 0xFFFF // UINT16	 
	 
	 
	 
 } EE_WdtTimeCfg_t_;

typedef UINT16 EE_WdtTimeCfg_t ;//ICAT EXPORTED ENUM 
 typedef enum 
 {	 
 EE_DEFER_NONE ,	 
 EE_DEFER_ASSERTS ,	 
 EE_DEFER_EXCEPTIONS ,	 
 EE_DEFER_ALL	 
	 
	 
	 
 } EE_DeferredMode_t_;

typedef UINT8 EE_DeferredMode_t ;//ICAT EXPORTED STRUCT 
 typedef struct 
 {	 
 EE_DeferredMode_t enable ;	 
 EE_OnOff_t deferIntermediateActions ;	 
 UINT16 limitMs ;	 
 UINT16 limitHits ;	 
 UINT16 reserved2 ;	 
 } EE_DeferredCfg_t;

//ICAT EXPORTED STRUCT 
 typedef struct 
 {	 
 EE_OnOff_t AssertHandlerOn ;	 
 EE_OnOff_t ExcepHandlerOn ;	 
 EE_WarningOn_t WarningHandlerOn ;	 
 EE_OnOff_t powerUpLogOn ;	 
 EE_OnOff_t extHandlersOn ;	 
 EE_OnOff_t printRecentLogOnStartup ;	 
 FinalAct_t finalAction ;	 
 UINT32 EELogFileSize ;	 
 UINT16 delayOnStartup ; // Delay for ICAT log coverage in 5 ms units	 
 EE_ContextType_t assertContextBufType ;	 
 EE_ContextType_t exceptionContextBufType ;	 
 EE_ContextType_t warningContextBufType ;	 
	 
 // -------- version 1 +D ----------	 
 EE_DeferredCfg_t deferredCfg ;	 
	 
	 
	 
 EE_WdtTimeCfg_t wdtConfigTime ; // UINT16	 
 UINT16 sysEeHandlerLimit ; // relevant for EE_ASSISTING_MASTER only ; ZERO is no limits	 
 UINT32 dumpDdrSizeBytes ; // relevant for EE_ASSISTING_MASTER only	 
 UINT8 reserved [ 24 ] ;	 
 } EE_Configuration_t;

DIAG_FILTER ( SW_PLAT , PERFORMANCE , SAVE2FILE0 , DIAG_INFORMATION)  
 diagPrintf ( " COMM MEM-DUMP File %s: nothing to save " , fNameBuf );

DIAG_FILTER ( SW_PLAT , PERFORMANCE , SAVE2FILE , DIAG_INFORMATION)  
 diagPrintf ( " COMM MEM-DUMP File %s: %ld bytes saved " , fNameBuf , count );

DIAG_FILTER ( Diag , comMem , EE_LOG_TXT , DIAG_INFORMATION)  
 diagPrintf ( " COMM EE LOG: %s " , pEElog->desc );

DIAG_FILTER ( Diag , comMem , EE_LOG , DIAG_INFORMATION)  
 diagStructPrintf ( " COMM EE LOG: %S { EE_Entry_t } " , ( UINT16 * ) pEElog , sizeof ( EE_Entry_t ) );

DIAG_FILTER ( Diag , comMem , EE_LOG_NOERR , DIAG_INFORMATION)  
 diagPrintf ( " COMM WDT expired ( NOT caused by ASSERT ) " );

//ICAT EXPORTED FUNCTION - Diag , comMem , SaveComPostmortem 
 void SaveComPostmortem ( void ) 
 {	 
 EE_PostmortemDesc_Entry* p ; // = getEntryByName ( POSTMORTEM_BUF_NAME_EEBUF ) ;	 
	 
 p = getEntryNext ( 1 /*getFirst*/ ) ;	 
 while ( p != ( ( EE_PostmortemDesc_Entry* ) 0xDEADDEAD ) )	 
 {		 
		 
		 
		 
		 
 p = ( EE_PostmortemDesc_Entry* ) ConvertPhysicalAddrToVirtualAddr ( ( UINT32 ) ( p ) ) ;		 
 eeh_log ( 7 , " %s: p: 0x%x\n " , __FUNCTION__ , p ) ;		 
 if ( strcmp ( p->name , " EE_Hbuf " ) == 0 )		 
 {			 
 printEEinfo ( ( UINT8* ) ConvertPhysicalAddrToVirtualAddr ( ( UINT32 ) ( p->bufAddr ) ) ) ;			 
 }		 
		 
 SaveToFile ( p->name , /*ADDR_CONVERT*/ ( p->bufAddr ) , p->bufLen ) ;		 
 p = getEntryNext ( 0 /*getFirst*/ ) ;		 
 }	 
 }

//ICAT EXPORTED FUNCTION - Diag , comMem , SaveCom_Addr_Len 
 void SaveCom_Addr_Len ( void* p ) 
 {	 
 UINT32* pVal = ( UINT32* ) ConvertPhysicalAddrToVirtualAddr ( ( UINT32 ) ( p ) ) ;	 
 UINT8* pBuf = ( UINT8* ) pVal [ 0 ] ;	 
 UINT32 len = pVal [ 1 ] ;	 
	 
	 
 SaveToFile ( " dumpBuf " , pBuf , len ) ;	 
 }

DIAG_FILTER ( Diag , comMem , DDR_RW , DIAG_INFORMATION)  
 diagPrintf ( " WAIT 1 MIN! Be patient! DDR_RW Dumping 0x%lx bytes of DDR_RW " , len );

DIAG_FILTER ( Diag , comMem , DDR_DiagOffline , DIAG_INFORMATION)  
 diagPrintf ( " WAIT 30 seconds! Be patient! DDR_DiagOffline Dumping 0x%lx bytes of DIAGLOG " , len );

