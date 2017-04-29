//PPL Source File Name : ../diag/diag_al/diag_comm_EXTif_OSA_LINUX.i
//PPL Source File Name : diag_comm_EXTif_OSA_LINUX.c
typedef int ptrdiff_t ;typedef unsigned int size_t ;typedef unsigned int wchar_t ;typedef __signed char __int8_t ;typedef unsigned char __uint8_t ;typedef short __int16_t ;typedef unsigned short __uint16_t ;typedef int __int32_t ;typedef unsigned int __uint32_t ;typedef long long __int64_t ;typedef unsigned long long __uint64_t ;typedef __int8_t __int_least8_t ;typedef __uint8_t __uint_least8_t ;typedef __int16_t __int_least16_t ;typedef __uint16_t __uint_least16_t ;typedef __int32_t __int_least32_t ;typedef __uint32_t __uint_least32_t ;typedef __int64_t __int_least64_t ;typedef __uint64_t __uint_least64_t ;typedef __int32_t __int_fast8_t ;typedef __uint32_t __uint_fast8_t ;typedef __int32_t __int_fast16_t ;typedef __uint32_t __uint_fast16_t ;typedef __int32_t __int_fast32_t ;typedef __uint32_t __uint_fast32_t ;typedef __int64_t __int_fast64_t ;typedef __uint64_t __uint_fast64_t ;typedef int __intptr_t ;typedef unsigned int __uintptr_t ;typedef __int64_t __intmax_t ;typedef __uint64_t __uintmax_t ;typedef __int32_t __register_t ;typedef unsigned long __vaddr_t ;typedef unsigned long __paddr_t ;typedef unsigned long __vsize_t ;typedef unsigned long __psize_t ;typedef int __clock_t ;typedef int __clockid_t ;typedef long __ptrdiff_t ;typedef int __time_t ;typedef int __timer_t ;typedef __builtin_va_list __va_list ;typedef int __wchar_t ;typedef int __wint_t ;typedef int __rune_t ;typedef void * __wctrans_t ;typedef void * __wctype_t ;typedef unsigned long __cpuid_t ;typedef __int32_t __dev_t ;typedef __uint32_t __fixpt_t ;typedef __uint32_t __gid_t ;typedef __uint32_t __id_t ;typedef __uint32_t __in_addr_t ;typedef __uint16_t __in_port_t ;typedef __uint32_t __ino_t ;typedef long __key_t ;typedef __uint32_t __mode_t ;typedef __uint32_t __nlink_t ;typedef __int32_t __pid_t ;typedef __uint64_t __rlim_t ;typedef __uint16_t __sa_family_t ;typedef __int32_t __segsz_t ;typedef __uint32_t __socklen_t ;typedef __int32_t __swblk_t ;typedef __uint32_t __uid_t ;typedef __uint32_t __useconds_t ;typedef __int32_t __suseconds_t ;typedef union {
 char __mbstate8 [ 128 ] ;
 __int64_t __mbstateL ;
 } __mbstate_t ;typedef __int8_t int8_t ;typedef __uint8_t uint8_t ;typedef __int16_t int16_t ;typedef __uint16_t uint16_t ;typedef __int32_t int32_t ;typedef __uint32_t uint32_t ;typedef __int64_t int64_t ;typedef __uint64_t uint64_t ;typedef int8_t int_least8_t ;typedef int8_t int_fast8_t ;typedef uint8_t uint_least8_t ;typedef uint8_t uint_fast8_t ;typedef int16_t int_least16_t ;typedef int32_t int_fast16_t ;typedef uint16_t uint_least16_t ;typedef uint32_t uint_fast16_t ;typedef int32_t int_least32_t ;typedef int32_t int_fast32_t ;typedef uint32_t uint_least32_t ;typedef uint32_t uint_fast32_t ;typedef int64_t int_least64_t ;typedef int64_t int_fast64_t ;typedef uint64_t uint_least64_t ;typedef uint64_t uint_fast64_t ;typedef int intptr_t ;typedef unsigned int uintptr_t ;typedef uint64_t uintmax_t ;typedef int64_t intmax_t ;typedef long int ssize_t ;typedef void ( *__kernel_sighandler_t ) ( int ) ;typedef int __kernel_key_t ;typedef int __kernel_mqd_t ;typedef unsigned long __kernel_ino_t ;typedef unsigned short __kernel_mode_t ;typedef unsigned short __kernel_nlink_t ;typedef long __kernel_off_t ;typedef int __kernel_pid_t ;typedef unsigned short __kernel_ipc_pid_t ;typedef unsigned short __kernel_uid_t ;typedef unsigned short __kernel_gid_t ;typedef unsigned int __kernel_size_t ;typedef int __kernel_ssize_t ;typedef int __kernel_ptrdiff_t ;typedef long __kernel_time_t ;typedef long __kernel_suseconds_t ;typedef long __kernel_clock_t ;typedef int __kernel_timer_t ;typedef int __kernel_clockid_t ;typedef int __kernel_daddr_t ;typedef char * __kernel_caddr_t ;typedef unsigned short __kernel_uid16_t ;typedef unsigned short __kernel_gid16_t ;typedef unsigned int __kernel_uid32_t ;typedef unsigned int __kernel_gid32_t ;typedef unsigned short __kernel_old_uid_t ;typedef unsigned short __kernel_old_gid_t ;typedef unsigned short __kernel_old_dev_t ;typedef long long __kernel_loff_t ;typedef unsigned short umode_t ;typedef __signed__ char __s8 ;typedef unsigned char __u8 ;typedef __signed__ short __s16 ;typedef unsigned short __u16 ;typedef __signed__ int __s32 ;typedef unsigned int __u32 ;typedef __signed__ long long __s64 ;typedef unsigned long long __u64 ;typedef __u16 __le16 ;typedef __u16 __be16 ;typedef __u32 __le32 ;typedef __u32 __be32 ;typedef __u64 __le64 ;typedef __u64 __be64 ;typedef unsigned long __kernel_blkcnt_t ;typedef unsigned long __kernel_blksize_t ;typedef unsigned long __kernel_fsblkcnt_t ;typedef unsigned long __kernel_fsfilcnt_t ;typedef unsigned int __kernel_id_t ;typedef __u32 __kernel_dev_t ;typedef __kernel_blkcnt_t blkcnt_t ;typedef __kernel_blksize_t blksize_t ;typedef __kernel_clock_t clock_t ;typedef __kernel_clockid_t clockid_t ;typedef __kernel_dev_t dev_t ;typedef __kernel_fsblkcnt_t fsblkcnt_t ;typedef __kernel_fsfilcnt_t fsfilcnt_t ;typedef __kernel_gid32_t gid_t ;typedef __kernel_id_t id_t ;typedef __kernel_ino_t ino_t ;typedef __kernel_key_t key_t ;typedef __kernel_mode_t mode_t ;typedef __kernel_nlink_t nlink_t ;typedef __kernel_off_t off_t ;typedef __kernel_loff_t loff_t ;typedef loff_t off64_t ;typedef __kernel_pid_t pid_t ;typedef __kernel_suseconds_t suseconds_t ;typedef __kernel_time_t time_t ;typedef __kernel_uid32_t uid_t ;typedef signed long useconds_t ;typedef __kernel_daddr_t daddr_t ;typedef __kernel_timer_t timer_t ;typedef __kernel_mqd_t mqd_t ;typedef __kernel_caddr_t caddr_t ;typedef unsigned int uint_t ;typedef unsigned int uint ;typedef unsigned char u_char ;typedef unsigned short u_short ;typedef unsigned int u_int ;typedef unsigned long u_long ;typedef uint32_t u_int32_t ;typedef uint16_t u_int16_t ;typedef uint8_t u_int8_t ;typedef uint64_t u_int64_t ;typedef unsigned long sigset_t ;typedef void __signalfn_t ( int ) ;typedef __signalfn_t *__sighandler_t ;typedef void __restorefn_t ( void ) ;typedef __restorefn_t *__sigrestore_t ;typedef union sigval {
 int sival_int ;
 void *sival_ptr ;
 } sigval_t ;typedef int sig_atomic_t ;typedef void ( *sig_t ) ( int ) ;typedef sig_t sighandler_t ;typedef __kernel_fd_set fd_set ;typedef __builtin_va_list __gnuc_va_list ;typedef off_t fpos_t ;typedef long pthread_mutexattr_t ;typedef long pthread_condattr_t ;typedef int pthread_key_t ;typedef long pthread_t ;typedef volatile int pthread_once_t ;typedef int pthread_rwlockattr_t ;typedef void ( *__pthread_cleanup_func_t ) ( void* ) ;typedef unsigned char cc_t ;typedef unsigned int speed_t ;typedef unsigned int tcflag_t ;typedef unsigned short sa_family_t ;typedef int socklen_t ;typedef unsigned char BOOL ;typedef unsigned char UINT8 ;typedef unsigned short UINT16 ;typedef unsigned int UINT32 ;typedef signed char CHAR ;typedef signed char INT8 ;typedef signed short INT16 ;typedef signed int INT32 ;typedef unsigned short WORD ;typedef unsigned int DWORD ;typedef int HANDLE ;typedef HANDLE* LPHANDLE ;typedef unsigned char* PUINT8 ;typedef long LONG ;typedef char* LPCTSTR ;typedef char* LPTSTR ;typedef void* LPVOID ;typedef unsigned int* LPDWORD ;typedef unsigned int* PDWORD ;typedef unsigned int* PUINT32 ;typedef unsigned short TCHAR ;typedef unsigned int UINT ;typedef INT32 *PINT32 ;typedef INT16 *PINT16 ;typedef UINT16 *PUINT16 ;typedef INT8 *PINT8 ;typedef const char * SwVersion ;typedef void* OSATaskRef ;typedef void* OSASemaRef ;typedef void* OSAMutexRef ;typedef void* OSAMsgQRef ;typedef void* OSAMailboxQRef ;typedef void* OSAPoolRef ;typedef void* OSATimerRef ;typedef void* OSAFlagRef ;typedef UINT8 OSA_STATUS ;typedef void* OSTaskRef ;typedef void* OSSemaRef ;typedef void* OSMutexRef ;typedef void* OSMsgQRef ;typedef void* OSMailboxQRef ;typedef void* OSPoolRef ;typedef void* OSTimerRef ;typedef void* OSFlagRef ;typedef UINT8 OS_STATUS ;typedef void ( *CommandAddress ) ( void ) ;typedef char* CommandProto ;typedef const char * DiagDBVersion ;//ICAT EXPORTED ENUM 
 typedef enum 
 {	 
 PROTOCOL_TYPE_0 = 0 ,	 
 MAX_PROTOCOL_TYPES	 
 } ProtocolType;

//ICAT EXPORTED STRUCT 
 typedef struct 
 {	 
 BOOL bEnabled ; // enable / disable the trace logging feature	 
 ProtocolType eProtocolType ; // protocol type for communication with ICAT , currently only protocol type 0 is supported	 
 UINT16 nMaxDataPerTrace ; // for each trace , what is the maximum data length to accompany the trace , in protocol type 0 , this is relevant only to DSP messages	 
 } DiagLoggerDefs;

typedef void VOID ;typedef int ( *DiagRxCallbackFunc ) ( char* packet , int len ) ;//ICAT EXPORTED ENUM 
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

typedef __gnuc_va_list va_list ;//ICAT EXPORTED ENUM 
 typedef enum {	 
 DIAG_COMMDEV_EXT , // EXT is also for clients that communicate with the master CMI	 
 DIAG_COMMDEV_INT ,	 
 DIAG_COMMDEV_CMI ,	 
 // for client-master - up to 5 clients	 
 DIAG_COMMDEV_CMI1 = DIAG_COMMDEV_CMI ,	 
 DIAG_COMMDEV_CMI2 ,	 
 DIAG_COMMDEV_CMI3 ,	 
 DIAG_COMMDEV_CMI4 ,	 
 DIAG_COMMDEV_CMI5 ,	 
 DIAG_COMMDEV_RX_MAX = DIAG_COMMDEV_CMI5 ,	 
	 
 // This enum is used also in UINT8 DiagReportItem_S.clientID and we want to limit the size to 8 bits.	 
 DIAG_COMMDEV_NODEVICE = 0xEF , // large enough value not to be a valid rx interface...	 
 DIAG_COMMDEV_DUMMY = 0x1FFFFFFF // to keep 4 bytes alignment in structs	 
 } COMDEV_NAME;

typedef void ( *FunctionPtr ) ( void * , UINT32 ) ;typedef void ( *RSVPFunctionPtr ) ( void * , UINT32 , UINT32 ) ;typedef union {
 UINT8 packedMsgData [ 13 ] ;
 UINT8 msgData [ 13 ] ;
 } PackedUnPackedData ;typedef UINT32 ( *DiagHSprotocolSendCB ) ( UINT8 * , UINT32 ) ;typedef void ( *DiagICATReadyNotifyEventFunc ) ( void ) ;typedef void ( *DiagEnterBootLoaderCBFunc ) ( void ) ;typedef void ( *Diag_Open_Func ) ( void ) ;typedef int ( *EventSocket_Open_Func ) ( void ) ;typedef void ( *Diag_Read_Task ) ( void * ) ;typedef int ( *Diag_Send_Target ) ( UINT8 * , UINT32 ) ;//ICAT EXPORTED ENUM 
 typedef enum 
 {	 
 // for external interfaces	 
 tNoConnection ,	 
 tUSBConnection ,	 
 tTCPConnection ,	 
 tUDPConnection ,	 
 tUARTConnection ,	 
 tSSPConnection ,	 
 tVIRTIOConnection ,	 
 // for internal interfaces	 
 tMSLConnection ,	 
 tSHMConnection , // shared memory	 
 tLocalConnection	 
 } EActiveConnectionType;

typedef void ( *MMI_STOP_CB_Func ) ( void ) ;typedef void ( *MMI_START_CB_Func ) ( const char* rel_path ) ;typedef UINT32 OS_Hisr_t ;typedef UINT32 OS_MemPool_t ;typedef pid_t OS_Proc_t ;typedef void ( *funcPtrType ) ( void* ) ;typedef void ( *timerCBPtrType ) ( UINT32 ) ;//ICAT EXPORTED FUNCTION - Diag , Debug , udpInternalICATreadyon 
 void udpInternalICATreadyon ( void ) 
 {	 
 bUDPsendICATready = 1 ;	 
DIAGM_TRACE( Diag , Debug , UDPicatReadyOn_1 , " Send ICATready ( internal ) when binded " );  

	 
	 
 }

//ICAT EXPORTED FUNCTION - Diag , Debug , udpInternalICATreadyoff 
 void udpInternalICATreadyoff ( void ) 
 {	 
 bUDPsendICATready = 0 ;	 
DIAGM_TRACE( Diag , Debug , UDPicatReadyOff_2 , " DO NOT SEND ICATready ( internal on bind ) " );  

	 
 }

