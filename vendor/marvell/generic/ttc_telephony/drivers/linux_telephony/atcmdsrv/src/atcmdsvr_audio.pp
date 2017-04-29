//PPL Source File Name : ../linux_telephony/atcmdsrv/src/atcmdsvr_audio.i
//PPL Source File Name : linux_telephony/atcmdsrv/src/atcmdsvr_audio.c
typedef unsigned char BOOL ;typedef unsigned char UINT8 ;typedef unsigned short UINT16 ;typedef unsigned int UINT32 ;typedef signed char CHAR ;typedef signed char INT8 ;typedef signed short INT16 ;typedef signed int INT32 ;typedef unsigned short WORD ;typedef unsigned int DWORD ;typedef int HANDLE ;typedef HANDLE* LPHANDLE ;typedef unsigned char* PUINT8 ;typedef long LONG ;typedef char* LPCTSTR ;typedef char* LPTSTR ;typedef void* LPVOID ;typedef unsigned int* LPDWORD ;typedef unsigned int* PDWORD ;typedef unsigned int* PUINT32 ;typedef unsigned short TCHAR ;typedef unsigned int UINT ;typedef INT32 *PINT32 ;typedef INT16 *PINT16 ;typedef UINT16 *PUINT16 ;typedef INT8 *PINT8 ;typedef signed char __int8_t ;typedef unsigned char __uint8_t ;typedef short __int16_t ;typedef unsigned short __uint16_t ;typedef int __int32_t ;typedef unsigned int __uint32_t ;typedef long long __int64_t ;typedef unsigned long long __uint64_t ;typedef __int8_t __int_least8_t ;typedef __uint8_t __uint_least8_t ;typedef __int16_t __int_least16_t ;typedef __uint16_t __uint_least16_t ;typedef __int32_t __int_least32_t ;typedef __uint32_t __uint_least32_t ;typedef __int64_t __int_least64_t ;typedef __uint64_t __uint_least64_t ;typedef __int32_t __int_fast8_t ;typedef __uint32_t __uint_fast8_t ;typedef __int32_t __int_fast16_t ;typedef __uint32_t __uint_fast16_t ;typedef __int32_t __int_fast32_t ;typedef __uint32_t __uint_fast32_t ;typedef __int64_t __int_fast64_t ;typedef __uint64_t __uint_fast64_t ;typedef int __intptr_t ;typedef unsigned int __uintptr_t ;typedef __int64_t __intmax_t ;typedef __uint64_t __uintmax_t ;typedef __int32_t __register_t ;typedef unsigned long __vaddr_t ;typedef unsigned long __paddr_t ;typedef unsigned long __vsize_t ;typedef unsigned long __psize_t ;typedef int __clock_t ;typedef int __clockid_t ;typedef long __ptrdiff_t ;typedef int __time_t ;typedef int __timer_t ;typedef __builtin_va_list __va_list ;typedef int __wchar_t ;typedef int __wint_t ;typedef int __rune_t ;typedef void * __wctrans_t ;typedef void * __wctype_t ;typedef unsigned long __cpuid_t ;typedef __int32_t __dev_t ;typedef __uint32_t __fixpt_t ;typedef __uint32_t __gid_t ;typedef __uint32_t __id_t ;typedef __uint32_t __in_addr_t ;typedef __uint16_t __in_port_t ;typedef __uint32_t __ino_t ;typedef long __key_t ;typedef __uint32_t __mode_t ;typedef __uint32_t __nlink_t ;typedef __int32_t __pid_t ;typedef __uint64_t __rlim_t ;typedef __uint16_t __sa_family_t ;typedef __int32_t __segsz_t ;typedef __uint32_t __socklen_t ;typedef __int32_t __swblk_t ;typedef __uint32_t __uid_t ;typedef __uint32_t __useconds_t ;typedef __int32_t __suseconds_t ;typedef union {
 char __mbstate8 [ 128 ] ;
 __int64_t __mbstateL ;
 } __mbstate_t ;typedef __builtin_va_list __gnuc_va_list ;typedef unsigned int size_t ;typedef int ptrdiff_t ;typedef unsigned int wchar_t ;typedef __int8_t int8_t ;typedef __uint8_t uint8_t ;typedef __int16_t int16_t ;typedef __uint16_t uint16_t ;typedef __int32_t int32_t ;typedef __uint32_t uint32_t ;typedef __int64_t int64_t ;typedef __uint64_t uint64_t ;typedef int8_t int_least8_t ;typedef int8_t int_fast8_t ;typedef uint8_t uint_least8_t ;typedef uint8_t uint_fast8_t ;typedef int16_t int_least16_t ;typedef int32_t int_fast16_t ;typedef uint16_t uint_least16_t ;typedef uint32_t uint_fast16_t ;typedef int32_t int_least32_t ;typedef int32_t int_fast32_t ;typedef uint32_t uint_least32_t ;typedef uint32_t uint_fast32_t ;typedef int64_t int_least64_t ;typedef int64_t int_fast64_t ;typedef uint64_t uint_least64_t ;typedef uint64_t uint_fast64_t ;typedef int intptr_t ;typedef unsigned int uintptr_t ;typedef uint64_t uintmax_t ;typedef int64_t intmax_t ;typedef long int ssize_t ;typedef void ( *__kernel_sighandler_t ) ( int ) ;typedef int __kernel_key_t ;typedef int __kernel_mqd_t ;typedef unsigned long __kernel_ino_t ;typedef unsigned short __kernel_mode_t ;typedef unsigned short __kernel_nlink_t ;typedef long __kernel_off_t ;typedef int __kernel_pid_t ;typedef unsigned short __kernel_ipc_pid_t ;typedef unsigned short __kernel_uid_t ;typedef unsigned short __kernel_gid_t ;typedef unsigned int __kernel_size_t ;typedef int __kernel_ssize_t ;typedef int __kernel_ptrdiff_t ;typedef long __kernel_time_t ;typedef long __kernel_suseconds_t ;typedef long __kernel_clock_t ;typedef int __kernel_timer_t ;typedef int __kernel_clockid_t ;typedef int __kernel_daddr_t ;typedef char * __kernel_caddr_t ;typedef unsigned short __kernel_uid16_t ;typedef unsigned short __kernel_gid16_t ;typedef unsigned int __kernel_uid32_t ;typedef unsigned int __kernel_gid32_t ;typedef unsigned short __kernel_old_uid_t ;typedef unsigned short __kernel_old_gid_t ;typedef unsigned short __kernel_old_dev_t ;typedef long long __kernel_loff_t ;typedef unsigned short umode_t ;typedef __signed__ char __s8 ;typedef unsigned char __u8 ;typedef __signed__ short __s16 ;typedef unsigned short __u16 ;typedef __signed__ int __s32 ;typedef unsigned int __u32 ;typedef __signed__ long long __s64 ;typedef unsigned long long __u64 ;typedef __u16 __le16 ;typedef __u16 __be16 ;typedef __u32 __le32 ;typedef __u32 __be32 ;typedef __u64 __le64 ;typedef __u64 __be64 ;typedef unsigned long __kernel_blkcnt_t ;typedef unsigned long __kernel_blksize_t ;typedef unsigned long __kernel_fsblkcnt_t ;typedef unsigned long __kernel_fsfilcnt_t ;typedef unsigned int __kernel_id_t ;typedef __u32 __kernel_dev_t ;typedef __kernel_blkcnt_t blkcnt_t ;typedef __kernel_blksize_t blksize_t ;typedef __kernel_clock_t clock_t ;typedef __kernel_clockid_t clockid_t ;typedef __kernel_dev_t dev_t ;typedef __kernel_fsblkcnt_t fsblkcnt_t ;typedef __kernel_fsfilcnt_t fsfilcnt_t ;typedef __kernel_gid32_t gid_t ;typedef __kernel_id_t id_t ;typedef __kernel_ino_t ino_t ;typedef __kernel_key_t key_t ;typedef __kernel_mode_t mode_t ;typedef __kernel_nlink_t nlink_t ;typedef __kernel_off_t off_t ;typedef __kernel_loff_t loff_t ;typedef loff_t off64_t ;typedef __kernel_pid_t pid_t ;typedef __kernel_suseconds_t suseconds_t ;typedef __kernel_time_t time_t ;typedef __kernel_uid32_t uid_t ;typedef signed long useconds_t ;typedef __kernel_daddr_t daddr_t ;typedef __kernel_timer_t timer_t ;typedef __kernel_mqd_t mqd_t ;typedef __kernel_caddr_t caddr_t ;typedef unsigned int uint_t ;typedef unsigned int uint ;typedef unsigned char u_char ;typedef unsigned short u_short ;typedef unsigned int u_int ;typedef unsigned long u_long ;typedef uint32_t u_int32_t ;typedef uint16_t u_int16_t ;typedef uint8_t u_int8_t ;typedef uint64_t u_int64_t ;typedef off_t fpos_t ;typedef __gnuc_va_list va_list ;typedef int utlReturnCode_T , *utlReturnCode_P ;typedef const utlReturnCode_T *utlReturnCode_P2c ;typedef unsigned int utlDataId_T , *utlDataId_P ;typedef const utlDataId_T *utlDataId_P2c ;typedef const utlLinkedListNode_T *utlLinkedListNode_P2c ;typedef unsigned int utlNodeCount_T ;typedef const utlLinkedList_T *utlLinkedList_P2c ;typedef int utlSecond_T ;typedef int utlNanosecond_T ;typedef const utlRelativeTime_T *utlRelativeTime_P2c ;typedef const utlAbsoluteTime_T *utlAbsoluteTime_P2c ;typedef const utlVString_T *utlVString_P2c ;typedef unsigned long utlTimerId_T ;typedef unsigned long utlTimeOutCount_T , *utlTimeOutCount_P ;typedef utlReturnCode_T ( *utlTimerFunction_P ) ( const utlTimerId_T id ,
 const utlTimeOutCount_T time_out_count ,
 void *arg_p ,
 const utlAbsoluteTime_P2c curr_time_p ) ;typedef unsigned long sigset_t ;typedef void __signalfn_t ( int ) ;typedef __signalfn_t *__sighandler_t ;typedef void __restorefn_t ( void ) ;typedef __restorefn_t *__sigrestore_t ;typedef union sigval {
 int sival_int ;
 void *sival_ptr ;
 } sigval_t ;typedef int sig_atomic_t ;typedef void ( *sig_t ) ( int ) ;typedef sig_t sighandler_t ;typedef __kernel_fd_set fd_set ;typedef long pthread_mutexattr_t ;typedef long pthread_condattr_t ;typedef int pthread_key_t ;typedef long pthread_t ;typedef volatile int pthread_once_t ;typedef int pthread_rwlockattr_t ;typedef void ( *__pthread_cleanup_func_t ) ( void* ) ;typedef unsigned int utlMutexAttributes_T , *utlMutexAttributes_P ;typedef unsigned int utlSemaphoreAttributes_T , *utlSemaphoreAttributes_P ;typedef int utlStateMachineStateId_T , *utlStateMachineStateId_P ;typedef int utlStateMachineEventId_T , *utlStateMachineEventId_P ;typedef utlReturnCode_T ( *utlStateMachineStateFunction_P ) ( const utlStateMachine_P state_machine_p ,
 const utlStateMachineStateId_T state ,
 const utlAbsoluteTime_P2c curr_time_p ,
 void *arg_p ) ;typedef utlReturnCode_T ( *utlStateMachineEventFunction_P ) ( const utlStateMachine_P state_machine_p ,
 const utlStateMachineStateId_T state ,
 const utlStateMachineEventId_T event ,
 const utlAbsoluteTime_P2c curr_time_p ,
 void *arg_p ,
 va_list va_arg_p ) ;typedef const utlStateMachineEvent_T *utlStateMachineEvent_P2c ;typedef const utlStateMachineState_T *utlStateMachineState_P2c ;typedef unsigned int utlStateMachineFlags_T ;typedef const utlStateMachine_T *utlStateMachine_P2c ;typedef union utlAtDataValue_U {
 unsigned int decimal ;
 unsigned int hexadecimal ;
 unsigned int binary ;
 char *string_p ;
 char *qstring_p ;
 char *dial_string_p ;
 } utlAtDataValue_T , *utlAtDataValue_P ;typedef const utlAtParameterValue_T *utlAtParameterValue_P2c ;typedef const utlAtParameter_T *utlAtParameter_P2c ;typedef const utlAtDceIoConfig_T *utlAtDceIoConfig_P2c ;typedef const utlAtSoundConfig_T *utlAtSoundConfig_P2c ;typedef utlReturnCode_T ( *utlAtGetParameterFunction_P ) ( const utlAtParameterOp_T op , const char *command_name_p , const utlAtParameterValue_P2c parameter_values_p , const size_t num_parameters , const char *info_text_p , unsigned int *xid_p , void *arg_p ) ;typedef utlReturnCode_T ( *utlAtSetParameterFunction_P ) ( const utlAtParameterOp_T op , const char *command_name_p , const utlAtParameterValue_P2c parameter_values_p , const size_t num_parameters , const char *info_text_p , unsigned int *xid_p , void *arg_p ) ;typedef utlReturnCode_T ( *utlAtDceIoConfigFunction_P ) ( const utlAtDceIoConfig_P2c dce_io_config_p , void *arg_p ) ;typedef utlReturnCode_T ( *utlAtSoundConfigFunction_P ) ( const utlAtSoundConfig_P2c sound_config_p , void *arg_p ) ;typedef utlReturnCode_T ( *utlAtSParameterFunction_P ) ( const unsigned int parameter_num , const unsigned int parameter_value , void *arg_p ) ;typedef utlReturnCode_T ( *utlAtSaveDialStringFunction_P ) ( const char *location_name_p , const char *dial_string_p , void *arg_p ) ;typedef utlReturnCode_T ( *utlAtRetrieveDialStringFunction_P ) ( const char **location_name_pp , const char **dial_string_pp , void *arg_p ) ;typedef utlReturnCode_T ( *utlAtReplyFunction_P ) ( const char *string_p , void *arg_p ) ;typedef utlReturnCode_T ( *utlAtTxLineDataFunction_P ) ( const unsigned char *octets_p , const size_t n , void *arg_p ) ;typedef utlReturnCode_T ( *utlAtDriverRequestFunction_P ) ( const utlAtParser_P parser_p , const utlAtDriverRequest_T request , void *arg_p , ... ) ;typedef utlReturnCode_T ( *utlAtCommandSyntaxFunction_P ) ( const utlAtParameterOp_T op , const char *command_name_p , const utlAtParameterValue_P2c parameter_values_p , const size_t num_parameters , const char *info_text_p , unsigned int *xid_p , void *arg_p ) ;typedef const utlAtCommand_T *utlAtCommand_P2c ;typedef const utlAtDialStringOptions_T *utlAtDialStringOptions_P2c ;typedef const utlAtAsyncResponse_T *utlAtAsyncResponse_P2c ;typedef const utlAtAsyncResponses_T *utlAtAsyncResponses_P2c ;typedef const utlAtParser_T *utlAtParser_P2c ;typedef const char * SwVersion ;typedef void VOID ;typedef int ( *DiagRxCallbackFunc ) ( char* packet , int len ) ;//ICAT EXPORTED ENUM 
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

//ICAT EXPORTED FUNCTION - VALI_IF , ATCMD_IF , TestATCmdIF 
 void Atcmd_HW_test ( void *data ) 
 {	 
 ( void ) data ; ;	 
 eeh_log ( 5 , " ATCMD DIAG Test:DO Nothing! ha ha ha\n " ) ;	 
DIAG_FILTER ( VALI_IF , ATCMD , ATCMDTEST , DIAG_TEST)  
 diagPrintf ( " Atcmd_HW_test " );

	 
 }

