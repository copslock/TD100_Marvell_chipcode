//PPL Source File Name : ../NVM_Proxy/source/NVMDiag.i
//PPL Source File Name : source/NVMDiag.c
typedef unsigned char BOOL ;typedef unsigned char UINT8 ;typedef unsigned short UINT16 ;typedef unsigned int UINT32 ;typedef signed char CHAR ;typedef signed char INT8 ;typedef signed short INT16 ;typedef signed int INT32 ;typedef unsigned short WORD ;typedef unsigned int DWORD ;typedef int HANDLE ;typedef HANDLE* LPHANDLE ;typedef unsigned char* PUINT8 ;typedef long LONG ;typedef char* LPCTSTR ;typedef char* LPTSTR ;typedef void* LPVOID ;typedef unsigned int* LPDWORD ;typedef unsigned int* PDWORD ;typedef unsigned int* PUINT32 ;typedef unsigned short TCHAR ;typedef unsigned int UINT ;typedef INT32 *PINT32 ;typedef INT16 *PINT16 ;typedef UINT16 *PUINT16 ;typedef INT8 *PINT8 ;typedef signed char __int8_t ;typedef unsigned char __uint8_t ;typedef short __int16_t ;typedef unsigned short __uint16_t ;typedef int __int32_t ;typedef unsigned int __uint32_t ;typedef long long __int64_t ;typedef unsigned long long __uint64_t ;typedef __int8_t __int_least8_t ;typedef __uint8_t __uint_least8_t ;typedef __int16_t __int_least16_t ;typedef __uint16_t __uint_least16_t ;typedef __int32_t __int_least32_t ;typedef __uint32_t __uint_least32_t ;typedef __int64_t __int_least64_t ;typedef __uint64_t __uint_least64_t ;typedef __int32_t __int_fast8_t ;typedef __uint32_t __uint_fast8_t ;typedef __int32_t __int_fast16_t ;typedef __uint32_t __uint_fast16_t ;typedef __int32_t __int_fast32_t ;typedef __uint32_t __uint_fast32_t ;typedef __int64_t __int_fast64_t ;typedef __uint64_t __uint_fast64_t ;typedef int __intptr_t ;typedef unsigned int __uintptr_t ;typedef __int64_t __intmax_t ;typedef __uint64_t __uintmax_t ;typedef __int32_t __register_t ;typedef unsigned long __vaddr_t ;typedef unsigned long __paddr_t ;typedef unsigned long __vsize_t ;typedef unsigned long __psize_t ;typedef int __clock_t ;typedef int __clockid_t ;typedef long __ptrdiff_t ;typedef int __time_t ;typedef int __timer_t ;typedef __builtin_va_list __va_list ;typedef int __wchar_t ;typedef int __wint_t ;typedef int __rune_t ;typedef void * __wctrans_t ;typedef void * __wctype_t ;typedef unsigned long __cpuid_t ;typedef __int32_t __dev_t ;typedef __uint32_t __fixpt_t ;typedef __uint32_t __gid_t ;typedef __uint32_t __id_t ;typedef __uint32_t __in_addr_t ;typedef __uint16_t __in_port_t ;typedef __uint32_t __ino_t ;typedef long __key_t ;typedef __uint32_t __mode_t ;typedef __uint32_t __nlink_t ;typedef __int32_t __pid_t ;typedef __uint64_t __rlim_t ;typedef __uint16_t __sa_family_t ;typedef __int32_t __segsz_t ;typedef __uint32_t __socklen_t ;typedef __int32_t __swblk_t ;typedef __uint32_t __uid_t ;typedef __uint32_t __useconds_t ;typedef __int32_t __suseconds_t ;typedef union {
 char __mbstate8 [ 128 ] ;
 __int64_t __mbstateL ;
 } __mbstate_t ;typedef __builtin_va_list __gnuc_va_list ;typedef unsigned int size_t ;typedef int ptrdiff_t ;typedef unsigned int wchar_t ;typedef __int8_t int8_t ;typedef __uint8_t uint8_t ;typedef __int16_t int16_t ;typedef __uint16_t uint16_t ;typedef __int32_t int32_t ;typedef __uint32_t uint32_t ;typedef __int64_t int64_t ;typedef __uint64_t uint64_t ;typedef int8_t int_least8_t ;typedef int8_t int_fast8_t ;typedef uint8_t uint_least8_t ;typedef uint8_t uint_fast8_t ;typedef int16_t int_least16_t ;typedef int32_t int_fast16_t ;typedef uint16_t uint_least16_t ;typedef uint32_t uint_fast16_t ;typedef int32_t int_least32_t ;typedef int32_t int_fast32_t ;typedef uint32_t uint_least32_t ;typedef uint32_t uint_fast32_t ;typedef int64_t int_least64_t ;typedef int64_t int_fast64_t ;typedef uint64_t uint_least64_t ;typedef uint64_t uint_fast64_t ;typedef int intptr_t ;typedef unsigned int uintptr_t ;typedef uint64_t uintmax_t ;typedef int64_t intmax_t ;typedef long int ssize_t ;typedef void ( *__kernel_sighandler_t ) ( int ) ;typedef int __kernel_key_t ;typedef int __kernel_mqd_t ;typedef unsigned long __kernel_ino_t ;typedef unsigned short __kernel_mode_t ;typedef unsigned short __kernel_nlink_t ;typedef long __kernel_off_t ;typedef int __kernel_pid_t ;typedef unsigned short __kernel_ipc_pid_t ;typedef unsigned short __kernel_uid_t ;typedef unsigned short __kernel_gid_t ;typedef unsigned int __kernel_size_t ;typedef int __kernel_ssize_t ;typedef int __kernel_ptrdiff_t ;typedef long __kernel_time_t ;typedef long __kernel_suseconds_t ;typedef long __kernel_clock_t ;typedef int __kernel_timer_t ;typedef int __kernel_clockid_t ;typedef int __kernel_daddr_t ;typedef char * __kernel_caddr_t ;typedef unsigned short __kernel_uid16_t ;typedef unsigned short __kernel_gid16_t ;typedef unsigned int __kernel_uid32_t ;typedef unsigned int __kernel_gid32_t ;typedef unsigned short __kernel_old_uid_t ;typedef unsigned short __kernel_old_gid_t ;typedef unsigned short __kernel_old_dev_t ;typedef long long __kernel_loff_t ;typedef unsigned short umode_t ;typedef __signed__ char __s8 ;typedef unsigned char __u8 ;typedef __signed__ short __s16 ;typedef unsigned short __u16 ;typedef __signed__ int __s32 ;typedef unsigned int __u32 ;typedef __signed__ long long __s64 ;typedef unsigned long long __u64 ;typedef __u16 __le16 ;typedef __u16 __be16 ;typedef __u32 __le32 ;typedef __u32 __be32 ;typedef __u64 __le64 ;typedef __u64 __be64 ;typedef unsigned long __kernel_blkcnt_t ;typedef unsigned long __kernel_blksize_t ;typedef unsigned long __kernel_fsblkcnt_t ;typedef unsigned long __kernel_fsfilcnt_t ;typedef unsigned int __kernel_id_t ;typedef __u32 __kernel_dev_t ;typedef __kernel_blkcnt_t blkcnt_t ;typedef __kernel_blksize_t blksize_t ;typedef __kernel_clock_t clock_t ;typedef __kernel_clockid_t clockid_t ;typedef __kernel_dev_t dev_t ;typedef __kernel_fsblkcnt_t fsblkcnt_t ;typedef __kernel_fsfilcnt_t fsfilcnt_t ;typedef __kernel_gid32_t gid_t ;typedef __kernel_id_t id_t ;typedef __kernel_ino_t ino_t ;typedef __kernel_key_t key_t ;typedef __kernel_mode_t mode_t ;typedef __kernel_nlink_t nlink_t ;typedef __kernel_off_t off_t ;typedef __kernel_loff_t loff_t ;typedef loff_t off64_t ;typedef __kernel_pid_t pid_t ;typedef __kernel_suseconds_t suseconds_t ;typedef __kernel_time_t time_t ;typedef __kernel_uid32_t uid_t ;typedef signed long useconds_t ;typedef __kernel_daddr_t daddr_t ;typedef __kernel_timer_t timer_t ;typedef __kernel_mqd_t mqd_t ;typedef __kernel_caddr_t caddr_t ;typedef unsigned int uint_t ;typedef unsigned int uint ;typedef unsigned char u_char ;typedef unsigned short u_short ;typedef unsigned int u_int ;typedef unsigned long u_long ;typedef uint32_t u_int32_t ;typedef uint16_t u_int16_t ;typedef uint8_t u_int8_t ;typedef uint64_t u_int64_t ;typedef off_t fpos_t ;typedef const char * SwVersion ;typedef UINT32 NVM_OP_CODE ;//ICAT EXPORTED STRUCTURE 
 typedef struct 
 {	 
 CHAR file_name [ ( ( 6 ) + 60 ) + 1 ] ;	 
 INT32 time ;	 
 INT32 date ;	 
 UINT32 size ;	 
 UINT32 data_id ; /* FDI identifier for file data */	 
 INT32 dir_id ; /* use for winCE handler and FDI directory handler*/	 
 INT32 plr_date ; /* use by FDI */	 
 } NVM_FILE_INFO;

//ICAT EXPORTED STRUCTURE 
 typedef struct 
 {	 
 NVM_OP_CODE op_code ;	 
 NVM_FILE_INFO fileParms ;	 
 NVM_STATUS_T error_code ; // error code of the requested operation	 
 } NVM_FILE_FIND_FIRST_RESPONSE_STRUCT;

//ICAT EXPORTED STRUCTURE 
 typedef struct 
 {	 
 NVM_OP_CODE op_code ;	 
 NVM_FILE_INFO fileParms ;	 
 NVM_STATUS_T error_code ; // error code of the requested operation	 
 } NVM_FILE_FIND_NEXT_RESPONSE_STRUCT;

//ICAT EXPORTED STRUCT 
 typedef struct 
 {	 
 NVM_OP_CODE op_code ;	 
 NVM_STATUS_T error_code ; // error code of the requested operation	 
 UINT8 second ; // Seconds after minute: 0 - 59	 
 UINT8 minute ; // Minutes after hour: 0 - 59	 
 UINT8 hour ; // Hours after midnight: 0 - 23	 
 UINT8 day ; // Day of month: 1 - 31	 
 UINT8 month ; // Month of year: 1 - 12	 
 UINT16 year ; // Calendar year: e.g 2001	 
 } RTC_GET_RESPONSE_STRUCT;

typedef __gnuc_va_list va_list ;typedef union sigval {
 int sival_int ;
 void *sival_ptr ;
 } sigval_t ;typedef unsigned char cc_t ;typedef unsigned int speed_t ;typedef unsigned int tcflag_t ;typedef unsigned long sigset_t ;typedef void __signalfn_t ( int ) ;typedef __signalfn_t *__sighandler_t ;typedef void __restorefn_t ( void ) ;typedef __restorefn_t *__sigrestore_t ;typedef int sig_atomic_t ;typedef void ( *sig_t ) ( int ) ;typedef sig_t sighandler_t ;typedef __kernel_fd_set fd_set ;typedef void VOID ;typedef int ( *DiagRxCallbackFunc ) ( char* packet , int len ) ;//ICAT EXPORTED ENUM 
 typedef enum {	 
 diagErrNoMemory ,	 
 diagErrMemoryOK ,	 
 diagErrExtQueueCongest ,	 
 diagErrINTQueueCongest ,	 
 diagErrCMIQueueCongest ,	 
 diagErrExtQueueOK ,	 
 diagErrINTQueueOK ,	 
 diagErrCMIQueueOK ,	 
 diagErrMSGmaxLen ,	 
 diagErrMsgwarningLen	 
 } diagErrStatesE;

//ICAT EXPORTED STRUCT 
 typedef struct {	 
 UINT32 CP_TimeStamp ; // time stamp in COMM when X bytes ( or more ) were produced	 
 UINT32 CP_bytes_produced ; // number of bytes produced in traces in the last period	 
 UINT32 CP_bytes_dropped ; // number of bytes produced but discarded ( due to congestion ) in the last period	 
 UINT32 CP_bytes_sent ; // number of bytes sent out on the internal channel in the last period	 
 UINT32 CP_bytes_sent_tot_time ; // total time to send all bytes in the period ( each send has start / end TS , delta is added to this counter )	 
 UINT32 CP_max_time_byteOint ; // max time to send a byte over internal interface	 
 UINT32 AP_TimeStamp ; // time stamp in APPS when the message arrived over internal interface and processed	 
 UINT32 AP_bytes_produced ; // number of bytes produced in traces ( on APPS or coming from COMM ) in the last period	 
 UINT32 AP_bytes_dropped ; // number of bytes produced but discarded ( due to congestion ) in the last period	 
 UINT32 AP_bytes_sent ; // number of bytes sent out on the external channel in the last period	 
 UINT32 AP_bytes_sent_tot_time ; // total time to send all bytes in the period ( each send has start / end TS , delta is added to this counter )	 
 UINT32 AP_max_time_byteOext ; // max time to send a byte over external interface	 
 UINT32 AP_bytes_recieved_from_CP ; // total time to send all bytes in the period ( each send has start / end TS , delta is added to this counter )	 
 // Fields added after first diag release of stats ( rel 4.280000 ) - not exist in all versions!! must be checked for.	 
 UINT16 CP_struct_size ; // size of the statistics structure on CP side ( data allows for backward / forward compatibility )	 
 UINT16 AP_struct_size ; // size of the statistics structure on AP side ( data allows for backward / forward compatibility )	 
 UINT32 CP_bytes_added_INTif ; // bytes added for sending over INT if	 
 UINT32 AP_bytes_removed_INTif ; // bytes recieved from CP and removed ( used for IntIF protocol only )	 
 UINT32 AP_bytes_added_ExtIf ; // bytes added on external interface	 
 } DiagStats_CP_AP_S;

//ICAT EXPORTED STRUCT 
 typedef struct {	 
 UINT32 diagMaxMsgOutLimit ;	 
 UINT32 diagMaxMsgOutWarning ;	 
 UINT32 diagMaxMsgInLimit ;	 
 UINT32 diagMaxMsgInWarning ;	 
 } diagMsgLimitSet_S;

//ICAT EXPORTED STRUCT 
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
	 
 } EE_entry_t;

//ICAT EXPORTED ENUM 
 typedef enum 
 {	 
 EEE_DataAbort ,	 
 EEE_PrefetchAbort ,	 
 EEE_FatalError ,	 
 EEE_SWInterrupt ,	 
 EEE_UndefInst ,	 
 EEE_ReservedInt	 
	 
 } EE_ExceptionType_t;

//ICAT EXPORTED ENUM 
 typedef enum 
 {	 
 EE_NO_RESET_SOURCE ,	 
	 
	 
	 
 EE_POWER_ON_RESET ,	 
	 
 EE_EXT_MASTER_RESET ,	 
	 
	 
	 
 EE_WDT_RESET	 
	 
	 
 } EE_PMU_t;

//ICAT EXPORTED STRUCT 
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
	 
 } EE_ContextType_t;

//ICAT EXPORTED ENUM 
 typedef enum 
 {	 
 EE_CDT_None ,	 
 EE_CDT_ExecTrace ,	 
 EE_CDT_StackDump ,	 
 EE_CDT_UserDefined = 0x10	 
	 
 } EE_ContextDataType_t;

//ICAT EXPORTED STRUCT 
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

typedef void voidPFuncVoid ( void ) ;typedef void ( *ExceptionHendler ) ( EE_RegInfo_Data_t* ) ;typedef EEHandlerAction ( *ExceptionHendlerExt ) ( EE_ExceptionType_t type , EE_RegInfo_Data_t* ) ;//ICAT EXPORTED STRUCT 
 typedef struct 
 {	 
 unsigned char cmd ;	 
 short data ;	 
 } InputStruct;

//ICAT EXPORTED STRUCT 
 typedef struct 
 {	 
 unsigned char d4 ;	 
 unsigned char d5 ;	 
 unsigned char d7 ;	 
 signed short offset_ibat ;	 
 signed short offset_vbat ;	 
 unsigned short slope_low ;	 
 unsigned short slope_high ;	 
 } CaliReturnStruct;

//ICAT EXPORTED STRUCT 
 typedef struct 
 {	 
 char fileName [ 256 ] ; // name of file located in Flash ( or to be create )	 
 char mode [ 256 ] ; // Opening mode ( e.g. open for readonly )	 
 } FOpenInputStruct;

//ICAT EXPORTED STRUCT 
 typedef struct 
 {	 
 UINT32 fileID ;	 
 char fileName [ 256 ] ; // name of file located in Flash memory.	 
 } FOpenReturnStruct;

//ICAT EXPORTED STRUCT 
 typedef struct 
 {	 
 UINT32 fileID ; // File ID of the file being handled.	 
 UINT16 PacketSize ; // size of packet being transfered.	 
 char writeBuf [ 1024 ] ; // buffer of bytes to be written	 
 } WriteInputStruct;

//ICAT EXPORTED STRUCT 
 typedef struct 
 {	 
 UINT32 fileID ;	 
 UINT16 bufferSize ;	 
 } ReadInputStruct;

//ICAT EXPORTED STRUCT 
 typedef struct 
 {	 
 UINT16 PacketOK ; // Contian the error code see FDI Doc ( e.g. ERR_OK )	 
 UINT16 LastPacket ; // equal to 1 if it ' s the last packet , 0 - otherwise.	 
 UINT16 PacketSize ; // size in bytes of the buffer being read from file.	 
 char readBuf [ 1024 ] ; // buffer of bytes contains the Flash file data.	 
 } ReadReturnStruct;

//ICAT EXPORTED STRUCT 
 typedef struct 
 {	 
 char fInfoList [ 1024 ] ; // buffer contain file properties information ( name , size , date etc. )	 
 UINT16 fListLen ; // Number of files describe in the buffer ( fInfoList )	 
 UINT8 LastPacket ; // Flag which intecates whether this is the last structure contain file -	 
 } FNamesReturnStruct;

//ICAT EXPORTED STRUCT 
 typedef struct 
 {	 
 char fOldName [ 256 ] ;	 
 char fNewName [ 256 ] ;	 
 } RenameInputStruct;

//ICAT EXPORTED STRUCT 
 typedef struct 
 {	 
 char fileName [ 256 ] ;	 
 UINT32 newMode ;	 
 } ChangModeStruct;

//ICAT EXPORTED STRUCT 
 typedef struct 
 {	 
 UINT32 fileID ;	 
 long offset ;	 
 int whereFrom ;	 
 } FSeekStruct;

//ICAT EXPORTED STRUCT 
 typedef struct 
 {	 
 unsigned long long fdvSize ;	 
 } FdvSizeStruct;

//ICAT EXPORTED STRUCT 
 typedef struct 
 {	 
 UINT32 fdvAvailableSpace ;	 
 } FdvAvailableSpaceStruct;

//ICAT EXPORTED STRUCT 
 typedef struct 
 {	 
 char dirName [ 256 ] ;	 
 } RmDirStruct;

//ICAT EXPORTED STRUCT 
 typedef struct 
 {	 
 char dirName [ 256 ] ;	 
 UINT16 mode ;	 
 } MkDirStruct;

//ICAT EXPORTED STRUCT 
 typedef struct 
 {	 
 UINT8 ffs_num_volumes ;	 
 char volumes [ 6 ] [ 10 ] ;	 
 } FdvVolumesStruct;

//ICAT EXPORTED FUNCTION - Diag , Utils , OutMsgBodyLimit 
 UINT32 diagGetOutMsgBodyLimit ( void ) 
 {	 
	 
 // YK change -100 to value of FOOTER+HEADER ( sizeof ( TxPDU ) +TX_PDU_FOOTER_SIZE )	 
 UINT32 msgBodySize = ( _diagInternalData.MsgLimits.diagMaxMsgOutLimit - 100 ) ;	 
 /* deduct 100 bytes ~= message header overhead */	 
	 
 // MB - This should be set to the corrcet value depending on te header used.	 
 // At this point this is a work around for the worst case. ( ethernet-l2 / l4 )	 
 msgBodySize = 4096 - 100 ;	 
	 
 // Due to a problem found in Linux this number was reduced need further research - workaround	 
 msgBodySize = 0x400 ;	 
	 
	 
DIAG_FILTER ( Diag , Utils , MaxOutMsgBodySize , DIAG_INFORMATION)  
 diagPrintf ( " diag ( target ) to ACAT: %ld " , msgBodySize );

	 
	 
 return msgBodySize ;	 
 }

//ICAT EXPORTED FUNCTION - FDI , Transport , Fopen 
 void _Fopen ( void* fInOpenStruct ) 
 {	 
 UINT32 hFileID ;	 
 FOpenInputStruct* fOpenInSt = ( FOpenInputStruct* ) fInOpenStruct ;	 
	 
 if ( NVMS_FileOpen ( nvm_acat_client_id , fOpenInSt->fileName , fOpenInSt->mode ,	 
 &hFileID ) == NVM_STATUS_SUCCESS )	 
 {		 
 FOpenReturnStruct retOpenStruct ;		 
 retOpenStruct.fileID = hFileID ;		 
 strncpy ( retOpenStruct.fileName , fOpenInSt->fileName , 256 ) ;		 
 eeh_log ( 7 , " %s reports %d , %s\n " , __PRETTY_FUNCTION__ , retOpenStruct.fileID , retOpenStruct.fileName ) ;		 
		 
DIAG_FILTER ( FDI , Transport , F_Open_Return_FileID , DIAG_INFORMATION)  
 diagStructPrintf ( " %S { FOpenReturnStruct } " , ( void * ) &retOpenStruct , sizeof ( FOpenReturnStruct ) );

		 
 }	 
 else	 
 {		 
DIAG_FILTER ( FDI , Transport , F_Open_Error , DIAG_INFORMATION)  
 diagPrintf ( " fopen %s error " , fOpenInSt->fileName );

		 
 }	 
 }

//ICAT EXPORTED FUNCTION - FDI , Transport , Fclose 
 void _Fclose ( void *fileID ) 
 {	 
 UINT32 hFileID = * ( ( UINT32 * ) fileID ) ;	 
	 
 if ( NVMS_FileClose ( nvm_acat_client_id , hFileID ) == NVM_STATUS_SUCCESS )	 
 {		 
DIAG_FILTER ( FDI , Transport , F_Close_Success , DIAG_INFORMATION)  
 diagPrintf ( " File ( ID: %lu ) was succesfully closed " , hFileID );

		 
 }	 
 else	 
 {		 
DIAG_FILTER ( FDI , Transport , F_Close_Error , DIAG_INFORMATION)  
 diagPrintf ( " File ( ID: %lu ) close error " , hFileID );

		 
 }	 
 }

//ICAT EXPORTED FUNCTION - FDI , Transport , Fread 
 void _Fread ( void *pReadStruct ) 
 {	 
 static ReadReturnStruct* pRetReadBuf = ( ( void * ) 0 ) ;	 
 static UINT32 nReadBufAllocSize = 0 ;	 
 UINT32 numBytesToRead , numActualReadBytes , nReadBufReqSize ;	 
	 
 UINT32 hFileID = ( ( ReadInputStruct* ) pReadStruct ) ->fileID ;	 
	 
 numBytesToRead = ( UINT32 ) ( ( ( ReadInputStruct* ) pReadStruct ) ->bufferSize ) ;	 
	 
 if ( numBytesToRead > diagGetOutMsgBodyLimit ( ) )	 
 {		 
DIAG_FILTER ( FDI , Transport , F_Read_Bad_Input , DIAG_INFORMATION)  
 diagPrintf ( " Number of packets to read exceeded the maximal number ( %d > %d ) " , numBytesToRead , diagGetOutMsgBodyLimit ( ) );

		 
 return ;		 
 }	 
	 
 /*	 
 for backward compatiblity the smallest size of buffer is sizeof ( ReadReturnStruct )	 
 which already contain 1024 bytes buffer for file content. the following assignment calculate the number of bytes	 
 required in addition to this structure , with consideration of DIAG max OUT message body limitation	 
 */	 
 nReadBufReqSize = ( numBytesToRead > 1024 ) ?	 
 ( sizeof ( ReadReturnStruct ) + numBytesToRead - 1024 ) :	 
 sizeof ( ReadReturnStruct ) ;	 
 /* if buffer was not allocated yet or its allocated size is smaller than requested now */	 
 if ( !pRetReadBuf || ( nReadBufAllocSize < nReadBufReqSize ) )	 
 {		 
 if ( pRetReadBuf ) /* means that allocated buffer size is smaller than read-size requested now */		 
 free ( pRetReadBuf ) ;		 
		 
 pRetReadBuf = ( ReadReturnStruct* ) ( malloc ( nReadBufReqSize ) ) ;		 
 }	 
	 
 if ( pRetReadBuf )	 
 {		 
 nReadBufAllocSize = nReadBufReqSize ;		 
 if ( NVMS_FileRead ( nvm_acat_client_id , hFileID , sizeof ( char ) , numBytesToRead , &numActualReadBytes ,		 
 pRetReadBuf->readBuf ) == NVM_STATUS_SUCCESS )		 
 {			 
 pRetReadBuf->PacketSize = ( UINT16 ) numActualReadBytes ;			 
 if ( numActualReadBytes == numBytesToRead )			 
 {				 
 pRetReadBuf->PacketOK = ERR_NONE ; // Update the return struct to contain any error information.				 
 pRetReadBuf->LastPacket = 0 ;				 
 }			 
 else /*if ( numActualReadBytes < numBytesToRead ) */			 
 {				 
 pRetReadBuf->PacketOK = ERR_EOF ;				 
 pRetReadBuf->LastPacket = 1 ;				 
 }			 
			 
DIAG_FILTER ( FDI , Transport , F_Read_Packet , DIAG_INFORMATION)  
 diagStructPrintf ( " %S { ReadReturnStruct } " , ( void * ) pRetReadBuf , nReadBufAllocSize );

			 
 }		 
 else		 
 {			 
DIAG_FILTER ( FDI , Transport , F_Read_Error , DIAG_INFORMATION)  
 diagPrintf ( " File read error ( ID: %lu ) " , hFileID );

			 
 }		 
 }	 
 else // Unsuccessful allocation	 
 {		 
 nReadBufAllocSize = 0 ;		 
DIAG_FILTER ( FDI , Transport , F_Read_Malloc_Error , DIAG_INFORMATION)  
 diagPrintf ( " Malloc in Fread failed " );

		 
 }	 
 }

//ICAT EXPORTED FUNCTION - FDI , Transport , Fwrite 
 void _Fwrite ( void * pWriteStruct ) 
 {	 
 WriteInputStruct *pws = ( WriteInputStruct * ) pWriteStruct ;	 
 UINT32 numberOfBytesWritten = 0 ;	 
 UINT32 numBytesToWrite = ( UINT32 ) ( pws->PacketSize ) ;	 
	 
 if ( NVMS_FileWrite ( nvm_acat_client_id , pws->fileID , pws->writeBuf , numBytesToWrite ,	 
 sizeof ( char ) , numBytesToWrite , &numberOfBytesWritten ) == NVM_STATUS_SUCCESS	 
 && numberOfBytesWritten == numBytesToWrite )	 
 {		 
DIAG_FILTER ( FDI , Transport , F_Write_Success , DIAG_INFORMATION)  
 diagPrintf ( " The Packet was written successfully ( packet size %d ) " , numBytesToWrite );

		 
 }	 
 else	 
 {		 
DIAG_FILTER ( FDI , Transport , F_Write_Error , DIAG_INFORMATION)  
 diagPrintf ( " File write error ( ID: %lu ) " , pws->fileID );

		 
 }	 
 }

//ICAT EXPORTED FUNCTION - FDI , Transport , Format_Flash 
 void _Format ( void ) 
 {	 
DIAG_FILTER ( FDI , Transport , Format_Error , DIAG_INFORMATION)  
 diagPrintf ( " Flash format is NOT supported " );

	 
 }

//ICAT EXPORTED FUNCTION - FDI , Transport , Remove_File 
 void _Remove ( void* fileName ) 
 {	 
 if ( NVMS_FileDelete ( nvm_acat_client_id , ( char * ) fileName ) == NVM_STATUS_SUCCESS )	 
 {		 
DIAG_FILTER ( FDI , Transport , Remove_Success , DIAG_INFORMATION)  
 diagPrintf ( " The file was successfully deleted " );

		 
 }	 
 else	 
 {		 
DIAG_FILTER ( FDI , Transport , Remove_Error , DIAG_INFORMATION)  
 diagPrintf ( " An error occurred while deleting file " );

		 
 }	 
 }

//ICAT EXPORTED FUNCTION - FDI , Transport , GetFileNameList 
 void _GetFileNameList ( void* wildcard ) 
 {	 
 NVM_FILE_INFO file_info ; /* Holds the file information */	 
 char sFDI [ ( ( 6 ) + 60 ) + 41 ] ; // max num of digits recieved from DWORD / int ( type of file attributes ) = 10 + file name length + 1 ( NULL terminator )	 
 char sDir [ ( ( 6 ) + 60 ) ] ;	 
 char sPath [ ( ( 6 ) + 60 ) ] ;	 
 char *psDir ;	 
 NVM_STATUS_T ret_val ;	 
 INT16 nFInfoListLen ;	 
 FNamesReturnStruct retFNamesStruct ;	 
 HANDLE hsearch = 0 ;	 
 UINT32 fStatus ;	 
	 
 strncpy ( sDir , wildcard , ( ( 6 ) + 60 ) ) ;	 
	 
 psDir = dirname ( sDir ) ;	 
 memmove ( sDir , psDir , strlen ( psDir ) + 1 ) ;	 
 strncat ( sDir , " / " , ( ( 6 ) + 60 ) ) ;	 
	 
 ret_val = NVMS_FileFindFirst ( nvm_acat_client_id , ( char* ) wildcard , &file_info , &hsearch ) ;	 
 if ( ret_val != NVM_STATUS_SUCCESS )	 
 {		 
DIAG_FILTER ( FDI , Transport , FName_LIST_NOTEXISTS , DIAG_INFORMATION)  
 diagPrintf ( " Currently there are no files on flash matching wildcard - %s " , ( char * ) wildcard );

		 
 return ;		 
 }	 
	 
 do {		 
 retFNamesStruct.LastPacket = 1 ;		 
 retFNamesStruct.fListLen = 0 ; /* Length meauered in number of files in the list */		 
 retFNamesStruct.fInfoList [ 0 ] = 0 ;		 
 nFInfoListLen = 0 ;		 
 do {			 
 fStatus = 0 ;			 
 strncpy ( sPath , sDir , ( ( 6 ) + 60 ) ) ;			 
 strncat ( sPath , ( char * ) file_info.file_name , ( ( 6 ) + 60 ) ) ;			 
 NVMS_GetFileStat ( nvm_acat_client_id , sPath , &fStatus ) ;			 
 nFInfoListLen += sprintf ( sFDI , " %s@%d@%d@%u@%u@ " , ( char* ) file_info.file_name , file_info.time , file_info.date , file_info.size , fStatus ) ;			 
			 
 /* Check if file properties list will not exceed maximum list buffer length ( +1 counts the ' \n ' at the end ) */			 
 if ( ( nFInfoListLen + 1 ) > 1024 )			 
 {				 
 retFNamesStruct.LastPacket = 0 ; /* Indicates on packet which DOES NOT contain all flash files information */				 
 break ;				 
 }			 
 retFNamesStruct.fListLen++ ; /* counts number of files */			 
 strncat ( retFNamesStruct.fInfoList , sFDI , 1024 ) ;			 
			 
 ret_val = NVMS_FileFindNext ( nvm_acat_client_id , &file_info , &hsearch ) ;			 
 } while ( ret_val == NVM_STATUS_SUCCESS ) ; /* Loop until we don ' t find anymore files. */		 
		 
DIAG_FILTER ( FDI , Transport , F_NameList_Packet , DIAG_INFORMATION)  
 diagStructPrintf ( " %S { FNamesReturnStruct } " , ( void * ) &retFNamesStruct , sizeof ( FNamesReturnStruct ) );

		 
 } while ( retFNamesStruct.LastPacket != ( UINT8 ) 1 ) ; /* Loop until we don ' t find anymore files. */	 
	 
 NVMS_FileFindClose ( nvm_acat_client_id , &hsearch ) ;	 
 }

//ICAT EXPORTED FUNCTION - FDI , Transport , RenameFile 
 void _RenameFile ( void* pRenameStruct ) 
 {	 
 RenameInputStruct* pRenameSt = ( RenameInputStruct* ) pRenameStruct ;	 
	 
 if ( NVMS_FileRename ( nvm_acat_client_id , pRenameSt->fOldName , pRenameSt->fNewName ) == NVM_STATUS_SUCCESS )	 
 {		 
DIAG_FILTER ( FDI , Transport , F_Rename_Succes , DIAG_INFORMATION)  
 diagPrintf ( " The file was successfully renamed to %s " , pRenameSt->fNewName );

		 
 }	 
 else	 
 {		 
DIAG_FILTER ( FDI , Transport , F_Rename_Error , DIAG_INFORMATION)  
 diagPrintf ( " Error occured while trying to change file name " );

		 
 }	 
 }

//ICAT EXPORTED FUNCTION - FDI , Transport , FStatus 
 void _FStatus ( void* fileName ) 
 {	 
 INT32 fStatus ;	 
	 
 if ( NVMS_GetFileStat ( nvm_acat_client_id , ( char* ) fileName , ( PDWORD ) &fStatus ) == NVM_STATUS_SUCCESS )	 
 {		 
DIAG_FILTER ( FDI , Transport , F_Status_Success , DIAG_INFORMATION)  
 diagPrintf ( " ( %d ) " , fStatus );

		 
 }	 
 else	 
 {		 
DIAG_FILTER ( FDI , Transport , F_Status_Error , DIAG_INFORMATION)  
 diagPrintf ( " An error occurred while checking the file status " );

		 
 }	 
 }

//ICAT EXPORTED FUNCTION - FDI , Transport , FchangeMode 
 void _FChangeMode ( void* stInChMode ) 
 {	 
 char *fileName = ( char* ) ( ( ( ChangModeStruct* ) stInChMode ) ->fileName ) ;	 
 INT32 newMode = ( INT32 ) ( ( ( ChangModeStruct* ) stInChMode ) ->newMode ) ;	 
	 
 if ( NVMS_FileChangeMode ( nvm_acat_client_id , fileName , newMode ) == NVM_STATUS_SUCCESS )	 
 {		 
DIAG_FILTER ( FDI , Transport , F_ChangeMode_Success , DIAG_INFORMATION)  
 diagPrintf ( " mode of %s was successfully changed " , fileName );

		 
 }	 
 else	 
 {		 
DIAG_FILTER ( FDI , Transport , F_ChangeMode_Error , DIAG_INFORMATION)  
 diagPrintf ( " An error occurred while changing file [ %s ] to mode %lu " , fileName , newMode );

		 
 }	 
 }

//ICAT EXPORTED FUNCTION - FDI , Transport , Fseek 
 void _Fseek ( void* stFSeek ) 
 {	 
 FSeekStruct* fdvSeek = ( FSeekStruct* ) ( stFSeek ) ;	 
	 
 if ( NVMS_FileSeek ( nvm_acat_client_id , fdvSeek->fileID , fdvSeek->offset , fdvSeek->whereFrom ) == NVM_STATUS_SUCCESS )	 
 {		 
DIAG_FILTER ( FDI , Transport , Fseek_Success , DIAG_INFORMATION)  
 diagPrintf ( " File ( ID: %lu ) indicator was offset by %lu from %lu " , fdvSeek->fileID , fdvSeek->offset , fdvSeek->whereFrom );

		 
 }	 
 else	 
 {		 
DIAG_FILTER ( FDI , Transport , Fseek_Error , DIAG_INFORMATION)  
 diagPrintf ( " Error occured while trying to seek within file ( ID: %lu ) " , fdvSeek->fileID );

		 
 }	 
 }

//ICAT EXPORTED FUNCTION - FDI , Transport , GetMaxFileNameLen 
 void _GetMaxFileNameLength ( ) 
 {	 
DIAG_FILTER ( FDI , Transport , MaxFileNameLen , DIAG_INFORMATION)  
 diagPrintf ( " %d " , 256 );

	 
 }

//ICAT EXPORTED FUNCTION - FDI , Transport , GetFdiFdvSize 
 void _GetFdiFdvSize ( ) 
 {	 
 FdvSizeStruct fdvSize ;	 
	 
 if ( NVMS_GetTotalSpace ( nvm_acat_client_id , ( UINT32 * ) ( & ( fdvSize.fdvSize ) ) ) == NVM_STATUS_SUCCESS )	 
 {		 
DIAG_FILTER ( FDI , Transport , FdvSize , DIAG_INFORMATION)  
 diagStructPrintf ( " %S { FdvSizeStruct } " , ( void * ) &fdvSize , sizeof ( FdvSizeStruct ) );

		 
 }	 
 else	 
 {		 
DIAG_FILTER ( FDI , Transport , FdvSize_Error , DIAG_ERROR)  
 diagPrintf ( " Error: cannot get NVM size " );

		 
 }	 
 }

//ICAT EXPORTED FUNCTION - FDI , Transport , GetFdiFdvAvailableSpace 
 void _GetFdiFdvAvailableSpace ( void *volName ) 
 {	 
 FdvAvailableSpaceStruct fdvSize ;	 
	 
 if ( NVMS_GetAvailSpace ( nvm_acat_client_id , volName , & ( fdvSize.fdvAvailableSpace ) ) == NVM_STATUS_SUCCESS )	 
 {		 
DIAG_FILTER ( FDI , Transport , FdvAvailableSize , DIAG_INFORMATION)  
 diagStructPrintf ( " %S { FdvAvailableSpaceStruct } " , ( void * ) &fdvSize , sizeof ( FdvAvailableSpaceStruct ) );

		 
 }	 
 else	 
 {		 
DIAG_FILTER ( FDI , Transport , FdvAvailableSize_Error , DIAG_ERROR)  
 diagPrintf ( " Error: cannot get available NVM space " );

		 
 }	 
 }

//ICAT EXPORTED FUNCTION - FDI , Transport , GetFdiVersion 
 void _GetFdiVersion ( ) 
 {	 
DIAG_FILTER ( FDI , Transport , FDI71 , DIAG_INFORMATION)  
 diagPrintf ( " FDI 71 " );

	 
DIAG_FILTER ( FDI , Transport , FDI721 , DIAG_INFORMATION)  
 diagPrintf ( " Aug06:upgrade to FDI721 " );

	 
 }

//ICAT EXPORTED FUNCTION - FDI , Transport , MkDir 
 void _MkDir ( void* MakeDirStruct ) 
 {	 
 MkDirStruct* MakeDirSt = ( MkDirStruct* ) MakeDirStruct ;	 
	 
 if ( NVMS_MkDir ( nvm_acat_client_id , MakeDirSt->dirName , MakeDirSt->mode ) == NVM_STATUS_SUCCESS )	 
 {		 
DIAG_FILTER ( FDI , Transport , mkdir_Success , DIAG_INFORMATION)  
 diagPrintf ( " Make Directory: %s " , MakeDirSt->dirName );

		 
 }	 
 else	 
 {		 
DIAG_FILTER ( FDI , Transport , mkdir_Error , DIAG_ERROR)  
 diagPrintf ( " mkdir %s error " , MakeDirSt->dirName );

		 
 }	 
 }

//ICAT EXPORTED FUNCTION - FDI , Transport , RmDir 
 void _RmDir ( void* RemoveDirStruct ) 
 {	 
 RmDirStruct* RemoveDirSt = ( RmDirStruct* ) RemoveDirStruct ;	 
	 
 if ( NVMS_RmDir ( nvm_acat_client_id , RemoveDirSt->dirName ) == NVM_STATUS_SUCCESS )	 
 {		 
DIAG_FILTER ( FDI , Transport , Rmdir_Success , DIAG_INFORMATION)  
 diagPrintf ( " Remove Directory: %s " , RemoveDirSt->dirName );

		 
 }	 
 else	 
 {		 
DIAG_FILTER ( FDI , Transport , Rmdir_Error , DIAG_ERROR)  
 diagPrintf ( " rmdir: %s error " , RemoveDirSt->dirName );

		 
 }	 
 }

//ICAT EXPORTED FUNCTION - FDI , Transport , GetFdiVolumes 
 void _GetFdiVolumes ( ) 
 {	 
 int i = 0 ;	 
 FdvVolumesStruct retFdvVolumesStruct ;	 
	 
 memset ( &retFdvVolumesStruct , 0 , sizeof ( FdvVolumesStruct ) ) ;	 
	 
 /* RootFS with absolute path */	 
 strncpy ( retFdvVolumesStruct.volumes [ i++ ] , " / " , 10 ) ;	 
	 
 retFdvVolumesStruct.ffs_num_volumes = i ;	 
DIAG_FILTER ( FDI , Transport , GetFdiVolumes_List , DIAG_INFORMATION)  
 diagStructPrintf ( " %S { FdvVolumesStruct } " , ( void* ) &retFdvVolumesStruct , sizeof ( FdvVolumesStruct ) );

	 
 }

//ICAT EXPORTED FUNCTION - FDI , Transport , ExtendedMode 
 void _ExtendetMode ( ) 
 {	 
DIAG_FILTER ( FDI , Transport , ExtendetMode , DIAG_INFORMATION)  
 diagPrintf ( " FDI7 Extendet Mode is ON " );

	 
 }

//ICAT EXPORTED FUNCTION - Diag , Utils , SetRTC 
 void _SetRTC ( void *pDateAmdTime ) 
 {	 
 ( void ) pDateAmdTime ;	 
 RTC_CalendarTime * Ctime = pDateAmdTime ;	 
 struct tm curgmtime ;	 
 time_t curtime ;	 
 int utc = 0 ;	 
	 
 curgmtime.tm_mday = Ctime->day ;	 
 curgmtime.tm_mon = Ctime->month-1 ;	 
 curgmtime.tm_year = Ctime->year-1900 ;	 
 curgmtime.tm_hour = Ctime->hour ;	 
 curgmtime.tm_min = Ctime->minute ;	 
 curgmtime.tm_sec = Ctime->second ;	 
 curgmtime.tm_isdst = 0 ;	 
 utc = 1 ;	 
 eeh_log ( 7 , " utc is %d\n " , utc ) ;	 
 curtime =mktime ( &curgmtime ) ;	 
 if ( write_rtc ( &curgmtime ) <0 )	 
 {		 
DIAG_FILTER ( Diag , Utils , SetRTC_Failure , DIAG_INFORMATION)  
 diagPrintf ( " RTC set failed , check the input parameter validity. " );

		 
		 
 }	 
 else	 
 {		 
 to_sys_clock ( utc ) ;		 
DIAG_FILTER ( Diag , Utils , SetRTC_Success , DIAG_INFORMATION)  
 diagPrintf ( " RTC was successfully set. " );

		 
 }	 
	 
	 
 }

//ICAT EXPORTED FUNCTION - Diag , Utils , ReadRTC 
 void _ReadRTC ( void ) 
 {	 
 time_t curtime ;	 
 struct tm *curgmtime ;	 
 RTC_CalendarTime Ctime ;	 
 if ( 1 )	 
 {		 
 char sCalanderTime [ 256 ] ;		 
 time ( &curtime ) ;		 
 curgmtime = gmtime ( &curtime ) ;		 
 sprintf ( sCalanderTime , " The RTC Value is: %d / %d / %d , %d:%d:%d " ,		 
 curgmtime->tm_mday ,		 
 curgmtime->tm_mon + 1 ,		 
 curgmtime->tm_year + 1900 ,		 
 curgmtime->tm_hour ,		 
 curgmtime->tm_min ,		 
 curgmtime->tm_sec ) ;		 
 Ctime.day = curgmtime->tm_mday ;		 
 Ctime.hour = curgmtime->tm_hour ;		 
 Ctime.minute = curgmtime->tm_min ;		 
 Ctime.month = curgmtime->tm_mon + 1 ;		 
 Ctime.second = curgmtime->tm_sec ;		 
 Ctime.year = curgmtime->tm_year + 1900 ;		 
DIAG_FILTER ( Diag , Utils , ReadRTC_Calendar , DIAG_INFORMATION)  
 diagStructPrintf ( " The CaledarTime is %S { RTC_CalendarTime } " , ( void * ) &Ctime , sizeof ( RTC_CalendarTime ) );

		 
		 
DIAG_FILTER ( Diag , Utils , ReadRTC_Success , DIAG_INFORMATION)  
 diagPrintf ( " %s " , sCalanderTime );

		 
 return ;		 
 }	 
 else	 
 {		 
DIAG_FILTER ( Diag , Utils , ReadRTC_Failure , DIAG_INFORMATION)  
 diagPrintf ( " Failed to read RTC " );

		 
 }	 
 }

//ICAT EXPORTED FUNCTION - EE_HANDLER , EE , EE_Test_data_abort 
 void EeHandlerTestDataAbortExcep ( void ) 
 {	 
 int i = 2 ;	 
	 
 * ( volatile int* ) ( 0x1807bbff ) = i ;	 
 }

//ICAT EXPORTED FUNCTION - EE_HANDLER , EE , EE_Test_Assert 
 void EeHandlerTestAssert ( void ) 
 {	 
 do { if ( ! ( 0 ) ) { char buffer [ 512 ] ; sprintf ( buffer , " ASSERT FAIL AT FILE %s FUNC %s LINE %d " , " source / NVMDiag.c " , __FUNCTION__ , 1020 ) ; eeh_draw_panic_text ( buffer , strlen ( buffer ) , 0 ) ; } } while ( 0 ) ;	 
 }

//ICAT EXPORTED FUNCTION - Diag , Utils , GetVersionDiag 
 void _GetVersionDiag ( void ) 
 {	 
DIAG_FILTER ( Diag , Utils , SWVersion , DIAG_INFORMATION)  
 diagPrintf ( " Tavor Linux V6.6 " );

	 
 }

DIAG_FILTER ( VALI_IF , MISC_IF , RESET_IF_Info , DIAG_INFORMATION)  
 diagPrintf ( " Reset... " );

DIAG_FILTER ( VALI_IF , MISC_IF , RESET_IF_Fail , DIAG_INFORMATION)  
 diagPrintf ( " Reset failed. " );

DIAG_FILTER ( VALI_IF , MISC_IF , RESET_IF_Timeout , DIAG_INFORMATION)  
 diagPrintf ( " Reset timeout. " );

//ICAT EXPORTED FUNCTION - VALI_IF , MISC_IF , temp_RESET_IF 
 void ResetTarget ( ) 
 {	 
 ap_reboot ( ) ;	 
 }

//ICAT EXPORTED STRUCT 
 typedef struct ouputIPM_DrvDetails_Tag 
 {	 
 UINT16 id ;	 
 UINT16 name [ 32 ] ;	 
 } ouputIPM_DrvDetails;

//ICAT EXPORTED STRUCT 
 typedef struct IPM_Drv_Export_struct_TYPE_Tag 
 {	 
 ouputIPM_DrvDetails IPMDrv_Params [ 32 ] ;	 
 } IPM_Drv_Export_struct_TYPE;

//ICAT EXPORTED FUNCTION - VALI_IF , CT_P_API , INIT_CT_Panalyzer 
 void INIT_CT_Panalyzer ( ) 
 {	 
 int fd = open ( " / dev / ispt " , 2 ) ;	 
	 
 if ( fd <= 0 )	 
 return ;	 
	 
 if ( ioctl ( fd , ( ( ( 1 U ) << ( ( ( 0 +8 ) +8 ) +14 ) ) | ( ( ( ' T ' ) ) << ( 0 +8 ) ) | ( ( ( 0 ) ) << 0 ) | ( ( ( ( ( sizeof ( int ) == sizeof ( int [ 1 ] ) && sizeof ( int ) < ( 1 << 14 ) ) ? sizeof ( int ) : __invalid_size_argument_for_IOC ) ) ) << ( ( 0 +8 ) +8 ) ) ) ) == 0 )	 
 {		 
DIAG_FILTER ( VALI_IF , CT_P_API , INIT_SUCCESS , DIAG_INFORMATION)  
 diagPrintf ( " [ CT_P ] CT_Panalyzer INIT success " );

		 
 }	 
	 
 close ( fd ) ;	 
 }

//ICAT EXPORTED FUNCTION - VALI_IF , CT_P_API , TERM_CT_Panalyzer 
 void TERM_CT_Panalyzer ( ) 
 {	 
 int fd = open ( " / dev / ispt " , 2 ) ;	 
	 
 if ( fd <= 0 )	 
 return ;	 
	 
 if ( ioctl ( fd , ( ( ( 1 U ) << ( ( ( 0 +8 ) +8 ) +14 ) ) | ( ( ( ' T ' ) ) << ( 0 +8 ) ) | ( ( ( 1 ) ) << 0 ) | ( ( ( ( ( sizeof ( int ) == sizeof ( int [ 1 ] ) && sizeof ( int ) < ( 1 << 14 ) ) ? sizeof ( int ) : __invalid_size_argument_for_IOC ) ) ) << ( ( 0 +8 ) +8 ) ) ) ) == 0 )	 
 {		 
DIAG_FILTER ( VALI_IF , CT_P_API , TERM_SUCCESS , DIAG_INFORMATION)  
 diagPrintf ( " [ CT_P ] CT_Panalyzer TERM success " );

		 
 }	 
	 
 close ( fd ) ;	 
 }

//ICAT EXPORTED FUNCTION - Validation , Utils , Get_IPM_Drivers_List 
 void Get_IPM_Drivers_List ( ) 
 {	 
	 
 /* Coverity CID=238 */	 
 int fd = -1 ;	 
 int num ;	 
 int i ;	 
 IPM_Drv_Export_struct_TYPE retData ;	 
 struct name_list *p = ( ( void * ) 0 ) ;	 
 struct name_list *cur = ( ( void * ) 0 ) ;	 
	 
 do {		 
 p = ( struct name_list * ) malloc ( /*PAGE_SIZE*/ 4096 ) ;		 
 if ( p == ( ( void * ) 0 ) )		 
 break ;		 
		 
 fd = open ( " / dev / ispt " , 2 ) ;		 
 if ( fd <= 0 )		 
 break ;		 
		 
 if ( ioctl ( fd , ( ( ( 2 U ) << ( ( ( 0 +8 ) +8 ) +14 ) ) | ( ( ( ' T ' ) ) << ( 0 +8 ) ) | ( ( ( 2 ) ) << 0 ) | ( ( ( ( ( sizeof ( int * ) == sizeof ( int * [ 1 ] ) && sizeof ( int * ) < ( 1 << 14 ) ) ? sizeof ( int * ) : __invalid_size_argument_for_IOC ) ) ) << ( ( 0 +8 ) +8 ) ) ) , &num ) < 0 )		 
 break ;		 
		 
 if ( num <= 0 || num > 32 )		 
 break ;		 
		 
 if ( ioctl ( fd , ( ( ( 2 U ) << ( ( ( 0 +8 ) +8 ) +14 ) ) | ( ( ( ' T ' ) ) << ( 0 +8 ) ) | ( ( ( 3 ) ) << 0 ) | ( ( ( ( ( sizeof ( void * ) == sizeof ( void * [ 1 ] ) && sizeof ( void * ) < ( 1 << 14 ) ) ? sizeof ( void * ) : __invalid_size_argument_for_IOC ) ) ) << ( ( 0 +8 ) +8 ) ) ) , p ) < 0 )		 
 break ;		 
		 
 memset ( &retData , 0 , sizeof ( IPM_Drv_Export_struct_TYPE ) ) ;		 
 for ( i = 0 , cur = p ; i < num ; i++ , cur++ )		 
 {			 
 retData.IPMDrv_Params [ i ] .id = cur->id ;			 
 strncpy ( ( char * ) retData.IPMDrv_Params [ i ] .name , cur->name , 32 ) ;			 
 }		 
		 
DIAG_FILTER ( Validation , Utils , Export_IPMDrv_list , DIAG_INFORMATION)  
 diagStructPrintf ( " VIF: Export_IPMDrv_list%S { IPM_Drv_Export_struct_TYPE } " , 
 ( ( void* ) &retData ) , sizeof ( IPM_Drv_Export_struct_TYPE ) );

		 
 } while ( 0 ) ;	 
	 
 if ( fd > 0 )	 
 close ( fd ) ;	 
 if ( p != ( ( void * ) 0 ) )	 
 free ( p ) ;	 
 }

DIAG_FILTER ( Diag , Utils , battery_cali_ok , DIAG_INFORMATION)  
 diagPrintf ( " OK " );

DIAG_FILTER ( Diag , Utils , battery_cali_error , DIAG_INFORMATION)  
 diagPrintf ( " ERROR " );

DIAG_FILTER ( Diag , Utils , battery_cali_debug , DIAG_INFORMATION)  
 diagPrintf ( " %s " , buf );

//ICAT EXPORTED FUNCTION - Diag , Utils , battery_calibration 
 void battery_calibration ( void* CaliInputStruct ) 
 {	 
 FILE *fd ;	 
 char value [ 20 ] ;	 
 unsigned char cmd ;	 
 short data ;	 
	 
 signed short vbat_offset=0 ;	 
 unsigned short slope_low=1000 , slope_high=1000 ;	 
 unsigned char d4=0 , d5=0 , d7=0 ;	 
 CaliReturnStruct TrimValue ;	 
	 
 int calibration_ibat = 0 ;	 
 int calibration_vbat = 0 ;	 
 char cmd_buf [ 128 ] ;	 
	 
 int ret = 0 ;	 
	 
 InputStruct* Input = ( InputStruct* ) CaliInputStruct ;	 
 cmd = Input->cmd ;	 
 data = Input->data ;	 
	 
 dbg_print ( " battery calibration: " " acat call battery_calibration function , cmd = %d , data = %d \n " , cmd , data ) ;	 
 switch ( cmd )	 
 {		 
 case 0 :		 
 dbg_print ( " battery calibration: " " calibration 0 0 \n " ) ;		 
 system ( " cat " " / sys / class / power_supply / battery / device / trim_d4 " ) ;		 
 print_ok ( ) ;		 
 break ;		 
 case 1 :		 
 dbg_print ( " battery calibration: " " calibration 1 xxx \n " ) ;		 
 fd=fopen ( " / sys / class / power_supply / battery / device / trim_d4 " , " w " ) ;		 
 if ( !fd ) {			 
 dbg_print ( " battery calibration: " " can ' t open d4 \n " ) ;			 
 exit ( 1 ) ;			 
 }		 
 dbg_print ( " battery calibration: " " 1 xxx , input value = %d \n " , data ) ;		 
 sprintf ( value , " %d\n " , data ) ;		 
 fwrite ( &value , strlen ( value ) , 1 , fd ) ;		 
 print_ok ( ) ;		 
 rewind ( fd ) ;		 
 fclose ( fd ) ;		 
 break ;		 
 case 2 :		 
 dbg_print ( " battery calibration: " " calibration 2 0 \n " ) ;		 
 fd=fopen ( " / sys / class / power_supply / battery / device / trim_d5d7 " , " r " ) ;		 
 if ( !fd ) {			 
 dbg_print ( " battery calibration: " " can ' t open d5d7 \n " ) ;			 
 exit ( 1 ) ;			 
 }		 
 fgets ( value , sizeof ( int ) , fd ) ;		 
 // ibat_offset = atoi ( value ) ;		 
 dbg_print ( " battery calibration: " " 2 0 , read trim value = %s\n " , value ) ;		 
 print_ok ( ) ;		 
 rewind ( fd ) ;		 
 fclose ( fd ) ;		 
 break ;		 
 case 3 :		 
 dbg_print ( " battery calibration: " " calibration 3 xxx \n " ) ;		 
 fd=fopen ( " / sys / class / power_supply / battery / device / trim_d5d7 " , " w " ) ;		 
 if ( !fd ) {			 
 dbg_print ( " battery calibration: " " can ' t open d5d7 \n " ) ;			 
 exit ( 1 ) ;			 
 }		 
 dbg_print ( " battery calibration: " " 3 xxx , input value = %d \n " , data ) ;		 
 sprintf ( value , " %d\n " , data ) ;		 
 fwrite ( &value , strlen ( value ) , 1 , fd ) ;		 
 print_ok ( ) ;		 
 rewind ( fd ) ;		 
 fclose ( fd ) ;		 
 break ;		 
		 
		 
 case 4 :		 
 dbg_print ( " battery calibration: " " calibration 4 0 \n " ) ;		 
 fd=fopen ( " / sys / class / power_supply / battery / device / trim_return " , " r " ) ;		 
 if ( !fd ) {			 
 dbg_print ( " battery calibration: " " can ' t open trim return \n " ) ;			 
 exit ( 1 ) ;			 
 }		 
 fgets ( value , sizeof ( value ) , fd ) ;		 
 sanremo_trim_value = atoi ( value ) ;		 
 print_ok ( ) ;		 
 dbg_print ( " battery calibration: " " 4 0 , read trim value = %d \n " , sanremo_trim_value ) ;		 
 rewind ( fd ) ;		 
 fclose ( fd ) ;		 
 break ;		 
		 
		 
 case 5 :		 
 dbg_print ( " battery calibration: " " calibration 5 xxx \n " ) ;		 
 fd=fopen ( " / sys / class / power_supply / battery / device / trim_vol " , " w " ) ;		 
 if ( !fd ) {			 
 dbg_print ( " battery calibration: " " can ' t open trim vol \n " ) ;			 
 exit ( 1 ) ;			 
 }		 
 input_vol [ 0 ] = data ;		 
 dbg_print ( " battery calibration: " " 5 xxx , input value = %d \n " , data ) ;		 
 sprintf ( value , " %d\n " , data ) ;		 
 fwrite ( &value , strlen ( value ) , 1 , fd ) ;		 
 if ( input_vol [ 0 ] >= 3195 && input_vol [ 0 ] <= 3205 )		 
 print_ok ( ) ;		 
 else		 
 print_error ( ) ;		 
 rewind ( fd ) ;		 
 fclose ( fd ) ;		 
 break ;		 
 case 6 :		 
 dbg_print ( " battery calibration: " " calibration 6 0 \n " ) ;		 
 fd=fopen ( " / sys / class / power_supply / battery / device / trim_vol " , " r " ) ;		 
 if ( !fd ) {			 
 dbg_print ( " battery calibration: " " can ' t open trim vol \n " ) ;			 
 exit ( 1 ) ;			 
 }		 
 fgets ( value , sizeof ( value ) , fd ) ;		 
 voltage [ 0 ] = atoi ( value ) ;		 
 dbg_print ( " battery calibration: " " input 3200 , voltage = %d \n " , voltage [ 0 ] ) ;		 
 print_ok ( ) ;		 
 rewind ( fd ) ;		 
 fclose ( fd ) ;		 
 break ;		 
		 
 case 7 :		 
 dbg_print ( " battery calibration: " " calibration 7 xxx \n " ) ;		 
 fd=fopen ( " / sys / class / power_supply / battery / device / trim_vol " , " w " ) ;		 
 if ( !fd ) {			 
 dbg_print ( " battery calibration: " " can ' t open trim vol \n " ) ;			 
 exit ( 1 ) ;			 
 }		 
 input_vol [ 1 ] = data ;		 
 dbg_print ( " battery calibration: " " 7 xxx , input value = %d \n " , data ) ;		 
 sprintf ( value , " %d\n " , data ) ;		 
 fwrite ( &value , strlen ( value ) , 1 , fd ) ;		 
 if ( input_vol [ 1 ] >= 3695 && input_vol [ 1 ] <= 3705 )		 
 print_ok ( ) ;		 
 else		 
 print_error ( ) ;		 
 rewind ( fd ) ;		 
 fclose ( fd ) ;		 
 break ;		 
 case 8 :		 
 dbg_print ( " battery calibration: " " calibration 8 0 \n " ) ;		 
 fd=fopen ( " / sys / class / power_supply / battery / device / trim_vol " , " r " ) ;		 
 if ( !fd ) {			 
 dbg_print ( " battery calibration: " " can ' t open trim vol \n " ) ;			 
 exit ( 1 ) ;			 
 }		 
 fgets ( value , sizeof ( value ) , fd ) ;		 
 voltage [ 1 ] = atoi ( value ) ;		 
 dbg_print ( " battery calibration: " " input 3700 , voltage = %d \n " , voltage [ 1 ] ) ;		 
 print_ok ( ) ;		 
 rewind ( fd ) ;		 
 fclose ( fd ) ;		 
 break ;		 
		 
 case 9 :		 
 dbg_print ( " battery calibration: " " calibration 9 xxx \n " ) ;		 
 fd=fopen ( " / sys / class / power_supply / battery / device / trim_vol " , " w " ) ;		 
 if ( !fd ) {			 
 dbg_print ( " battery calibration: " " can ' t open trim vol \n " ) ;			 
 exit ( 1 ) ;			 
 }		 
 input_vol [ 2 ] = data ;		 
 dbg_print ( " battery calibration: " " 9 xxx , input value = %d \n " , data ) ;		 
 sprintf ( value , " %d\n " , data ) ;		 
 fwrite ( &value , strlen ( value ) , 1 , fd ) ;		 
 if ( input_vol [ 2 ] >= 4195 && input_vol [ 2 ] <= 4205 )		 
 print_ok ( ) ;		 
 else		 
 print_error ( ) ;		 
 rewind ( fd ) ;		 
 fclose ( fd ) ;		 
 break ;		 
 case 10 :		 
 dbg_print ( " battery calibration: " " calibration 10 0 \n " ) ;		 
 fd=fopen ( " / sys / class / power_supply / battery / device / trim_vol " , " r " ) ;		 
 if ( !fd ) {			 
 dbg_print ( " battery calibration: " " can ' t open trim vol \n " ) ;			 
 exit ( 1 ) ;			 
 }		 
 fgets ( value , sizeof ( value ) , fd ) ;		 
 voltage [ 2 ] = atoi ( value ) ;		 
 dbg_print ( " battery calibration: " " input 4200 , voltage = %d \n " , voltage [ 2 ] ) ;		 
 print_ok ( ) ;		 
 rewind ( fd ) ;		 
 fclose ( fd ) ;		 
 break ;		 
 case 11 :		 
 dbg_print ( " battery calibration: " " calibration 11 0 \n " ) ;		 
 fd=fopen ( " / sys / class / power_supply / battery / device / trim_ibat_remain " , " r " ) ;		 
 if ( !fd ) {			 
 dbg_print ( " battery calibration: " " can ' t open trim return \n " ) ;			 
 exit ( 1 ) ;			 
 }		 
 fgets ( value , sizeof ( value ) , fd ) ;		 
 ibat_offset = atoi ( value ) ;		 
 dbg_print ( " battery calibration: " " ibat_offset = %d \n " , ibat_offset ) ;		 
 print_ok ( ) ;		 
 rewind ( fd ) ;		 
 fclose ( fd ) ;		 
 break ;		 
 case 12 :		 
 dbg_print ( " battery calibration: " " calibration 12 0 \n " ) ;		 
 vbat_offset = input_vol [ 1 ] - voltage [ 1 ] ;		 
 slope_low = 1000 * ( input_vol [ 1 ] - input_vol [ 0 ] ) / ( voltage [ 1 ] - voltage [ 0 ] ) ;		 
 slope_high = 1000 * ( input_vol [ 2 ] - input_vol [ 1 ] ) / ( voltage [ 2 ] -voltage [ 1 ] ) ;		 
		 
 d4 = sanremo_trim_value&0xff ; // sanremo register 0xD4 trim value		 
 d5 = ( sanremo_trim_value>>8 ) &0xff ; // sanremo register 0xD5 trim value		 
 d7 = ( sanremo_trim_value>>16 ) &0xff ; // sanremo register 0xD7 trim value		 
 dbg_print ( " battery calibration: " " input_vol0 = %d , input_vol1 = %d , input_vol2 = %d \n " , input_vol [ 0 ] , input_vol [ 1 ] , input_vol [ 2 ] ) ;		 
 dbg_print ( " battery calibration: " " voltage0 = %d , voltage1 = %d , voltage2 = %d \n " , voltage [ 0 ] , voltage [ 1 ] , voltage [ 2 ] ) ;		 
 dbg_print ( " battery calibration: " " d4=%x , d5=%x , d7=%x \n " , d4 , d5 , d7 ) ;		 
 dbg_print ( " battery calibration: " " vbat_offset=%d , ibat_offset=%d \n " , vbat_offset , ibat_offset ) ;		 
 dbg_print ( " battery calibration: " " slope_low=%d , slope_high=%d \n " , slope_low , slope_high ) ;		 
		 
 TrimValue.d4 = d4 ;		 
 TrimValue.d5 = d5 ;		 
 TrimValue.d7 = d7 ;		 
 TrimValue.offset_ibat = ibat_offset ;		 
 TrimValue.offset_vbat = vbat_offset ;		 
 TrimValue.slope_low = slope_low ;		 
 TrimValue.slope_high = slope_high ;		 
DIAG_FILTER ( Diag , Utils , battery_calibration , DIAG_INFORMATION)  
 diagStructPrintf ( " DIAG: calibration_return%S { CaliReturnStruct } " , 
 ( ( void* ) &TrimValue ) , sizeof ( CaliReturnStruct ) );

		 
		 
 break ;		 
 case 13 :		 
 dbg_print ( " battery calibration: " " calibration 13 0 \n " ) ;		 
 vbat_offset = input_vol [ 1 ] - voltage [ 1 ] ;		 
 slope_low = 1000 * ( input_vol [ 1 ] - input_vol [ 0 ] ) / ( voltage [ 1 ] - voltage [ 0 ] ) ;		 
 slope_high = 1000 * ( input_vol [ 2 ] - input_vol [ 1 ] ) / ( voltage [ 2 ] -voltage [ 1 ] ) ;		 
		 
 d4 = sanremo_trim_value&0xff ; // sanremo register 0xD4 trim value		 
 d5 = ( sanremo_trim_value>>8 ) &0xff ; // sanremo register 0xD5 trim value		 
 d7 = ( sanremo_trim_value>>16 ) &0xff ; // sanremo register 0xD7 trim value		 
		 
 cmd_buf [ 0 ] = 0 ;		 
 sprintf ( cmd_buf , " echo d4value %d > %s " , d4 , " / sys / class / power_supply / battery / device / calibration " ) ;		 
 ret = system ( cmd_buf ) ;		 
 dbg_print ( " battery calibration: " " cmd %s ret %d\n " , cmd_buf , ret ) ;		 
 cmd_buf [ 0 ] = 0 ;		 
 sprintf ( cmd_buf , " echo d5value %d > %s " , d5 , " / sys / class / power_supply / battery / device / calibration " ) ;		 
 ret = system ( cmd_buf ) ;		 
 dbg_print ( " battery calibration: " " cmd %s ret %d\n " , cmd_buf , ret ) ;		 
 cmd_buf [ 0 ] = 0 ;		 
 sprintf ( cmd_buf , " echo d7value %d > %s " , d7 , " / sys / class / power_supply / battery / device / calibration " ) ;		 
 ret = system ( cmd_buf ) ;		 
 dbg_print ( " battery calibration: " " cmd %s ret %d\n " , cmd_buf , ret ) ;		 
 cmd_buf [ 0 ] = 0 ;		 
 sprintf ( cmd_buf , " echo ibat_offset %d > %s " , ibat_offset , " / sys / class / power_supply / battery / device / calibration " ) ;		 
 ret = system ( cmd_buf ) ;		 
 dbg_print ( " battery calibration: " " cmd %s ret %d\n " , cmd_buf , ret ) ;		 
 cmd_buf [ 0 ] = 0 ;		 
 sprintf ( cmd_buf , " echo vbat_offset %d > %s " , vbat_offset , " / sys / class / power_supply / battery / device / calibration " ) ;		 
 ret = system ( cmd_buf ) ;		 
 dbg_print ( " battery calibration: " " cmd %s ret %d\n " , cmd_buf , ret ) ;		 
 cmd_buf [ 0 ] = 0 ;		 
 sprintf ( cmd_buf , " echo vbat_slope_low %d > %s " , slope_low , " / sys / class / power_supply / battery / device / calibration " ) ;		 
 ret = system ( cmd_buf ) ;		 
 dbg_print ( " battery calibration: " " cmd %s ret %d\n " , cmd_buf , ret ) ;		 
 cmd_buf [ 0 ] = 0 ;		 
 sprintf ( cmd_buf , " echo vbat_slope_high %d > %s " , slope_high , " / sys / class / power_supply / battery / device / calibration " ) ;		 
 ret = system ( cmd_buf ) ;		 
 dbg_print ( " battery calibration: " " cmd %s ret %d\n " , cmd_buf , ret ) ;		 
 print_ok ( ) ;		 
 break ;		 
 case 14 :		 
 dbg_print ( " battery calibration: " " calibration 14 0 \n " ) ;		 
 fd=fopen ( " / sys / class / power_supply / battery / device / calibration_vbat " , " r " ) ;		 
 if ( !fd ) {			 
 dbg_print ( " battery calibration: " " can ' t open calibration vbat \n " ) ;			 
 exit ( 1 ) ;			 
 }		 
 fgets ( value , sizeof ( value ) , fd ) ;		 
 calibration_vbat = atoi ( value ) ;		 
 rewind ( fd ) ;		 
 fclose ( fd ) ;		 
DIAG_FILTER ( Diag , Utils , bettery_calibration_vbat , DIAG_INFORMATION)  
 diagPrintf ( " DIAG: battery calibration_vbat: %d " , calibration_vbat );

		 
 break ;		 
 case 15 :		 
 dbg_print ( " battery calibration: " " calibration 15 0 \n " ) ;		 
 fd=fopen ( " / sys / class / power_supply / battery / device / calibration_ibat " , " r " ) ;		 
 if ( !fd ) {			 
 dbg_print ( " battery calibration: " " can ' t open calibration ibat \n " ) ;			 
 exit ( 1 ) ;			 
 }		 
 fgets ( value , sizeof ( value ) , fd ) ;		 
 calibration_ibat = atoi ( value ) ;		 
 rewind ( fd ) ;		 
 fclose ( fd ) ;		 
DIAG_FILTER ( Diag , Utils , battery_calibration_ibat , DIAG_INFORMATION)  
 diagPrintf ( " DIAG: battery calibration_ibat: %d " , calibration_ibat );

		 
 break ;		 
 }	 
 }

