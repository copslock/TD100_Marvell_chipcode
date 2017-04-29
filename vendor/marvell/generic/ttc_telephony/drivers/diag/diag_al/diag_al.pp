//PPL Source File Name : ../diag/diag_al/diag_al.i
//PPL Source File Name : diag_al.c
typedef int ptrdiff_t ;typedef unsigned int size_t ;typedef unsigned int wchar_t ;typedef signed char __int8_t ;typedef unsigned char __uint8_t ;typedef short __int16_t ;typedef unsigned short __uint16_t ;typedef int __int32_t ;typedef unsigned int __uint32_t ;typedef long long __int64_t ;typedef unsigned long long __uint64_t ;typedef __int8_t __int_least8_t ;typedef __uint8_t __uint_least8_t ;typedef __int16_t __int_least16_t ;typedef __uint16_t __uint_least16_t ;typedef __int32_t __int_least32_t ;typedef __uint32_t __uint_least32_t ;typedef __int64_t __int_least64_t ;typedef __uint64_t __uint_least64_t ;typedef __int32_t __int_fast8_t ;typedef __uint32_t __uint_fast8_t ;typedef __int32_t __int_fast16_t ;typedef __uint32_t __uint_fast16_t ;typedef __int32_t __int_fast32_t ;typedef __uint32_t __uint_fast32_t ;typedef __int64_t __int_fast64_t ;typedef __uint64_t __uint_fast64_t ;typedef int __intptr_t ;typedef unsigned int __uintptr_t ;typedef __int64_t __intmax_t ;typedef __uint64_t __uintmax_t ;typedef __int32_t __register_t ;typedef unsigned long __vaddr_t ;typedef unsigned long __paddr_t ;typedef unsigned long __vsize_t ;typedef unsigned long __psize_t ;typedef int __clock_t ;typedef int __clockid_t ;typedef long __ptrdiff_t ;typedef int __time_t ;typedef int __timer_t ;typedef __builtin_va_list __va_list ;typedef int __wchar_t ;typedef int __wint_t ;typedef int __rune_t ;typedef void * __wctrans_t ;typedef void * __wctype_t ;typedef unsigned long __cpuid_t ;typedef __int32_t __dev_t ;typedef __uint32_t __fixpt_t ;typedef __uint32_t __gid_t ;typedef __uint32_t __id_t ;typedef __uint32_t __in_addr_t ;typedef __uint16_t __in_port_t ;typedef __uint32_t __ino_t ;typedef long __key_t ;typedef __uint32_t __mode_t ;typedef __uint32_t __nlink_t ;typedef __int32_t __pid_t ;typedef __uint64_t __rlim_t ;typedef __uint16_t __sa_family_t ;typedef __int32_t __segsz_t ;typedef __uint32_t __socklen_t ;typedef __int32_t __swblk_t ;typedef __uint32_t __uid_t ;typedef __uint32_t __useconds_t ;typedef __int32_t __suseconds_t ;typedef union {
 char __mbstate8 [ 128 ] ;
 __int64_t __mbstateL ;
 } __mbstate_t ;typedef __int8_t int8_t ;typedef __uint8_t uint8_t ;typedef __int16_t int16_t ;typedef __uint16_t uint16_t ;typedef __int32_t int32_t ;typedef __uint32_t uint32_t ;typedef __int64_t int64_t ;typedef __uint64_t uint64_t ;typedef int8_t int_least8_t ;typedef int8_t int_fast8_t ;typedef uint8_t uint_least8_t ;typedef uint8_t uint_fast8_t ;typedef int16_t int_least16_t ;typedef int32_t int_fast16_t ;typedef uint16_t uint_least16_t ;typedef uint32_t uint_fast16_t ;typedef int32_t int_least32_t ;typedef int32_t int_fast32_t ;typedef uint32_t uint_least32_t ;typedef uint32_t uint_fast32_t ;typedef int64_t int_least64_t ;typedef int64_t int_fast64_t ;typedef uint64_t uint_least64_t ;typedef uint64_t uint_fast64_t ;typedef int intptr_t ;typedef unsigned int uintptr_t ;typedef uint64_t uintmax_t ;typedef int64_t intmax_t ;typedef long int ssize_t ;typedef void ( *__kernel_sighandler_t ) ( int ) ;typedef int __kernel_key_t ;typedef int __kernel_mqd_t ;typedef unsigned long __kernel_ino_t ;typedef unsigned short __kernel_mode_t ;typedef unsigned short __kernel_nlink_t ;typedef long __kernel_off_t ;typedef int __kernel_pid_t ;typedef unsigned short __kernel_ipc_pid_t ;typedef unsigned short __kernel_uid_t ;typedef unsigned short __kernel_gid_t ;typedef unsigned int __kernel_size_t ;typedef int __kernel_ssize_t ;typedef int __kernel_ptrdiff_t ;typedef long __kernel_time_t ;typedef long __kernel_suseconds_t ;typedef long __kernel_clock_t ;typedef int __kernel_timer_t ;typedef int __kernel_clockid_t ;typedef int __kernel_daddr_t ;typedef char * __kernel_caddr_t ;typedef unsigned short __kernel_uid16_t ;typedef unsigned short __kernel_gid16_t ;typedef unsigned int __kernel_uid32_t ;typedef unsigned int __kernel_gid32_t ;typedef unsigned short __kernel_old_uid_t ;typedef unsigned short __kernel_old_gid_t ;typedef unsigned short __kernel_old_dev_t ;typedef long long __kernel_loff_t ;typedef unsigned short umode_t ;typedef __signed__ char __s8 ;typedef unsigned char __u8 ;typedef __signed__ short __s16 ;typedef unsigned short __u16 ;typedef __signed__ int __s32 ;typedef unsigned int __u32 ;typedef __signed__ long long __s64 ;typedef unsigned long long __u64 ;typedef __u16 __le16 ;typedef __u16 __be16 ;typedef __u32 __le32 ;typedef __u32 __be32 ;typedef __u64 __le64 ;typedef __u64 __be64 ;typedef unsigned long __kernel_blkcnt_t ;typedef unsigned long __kernel_blksize_t ;typedef unsigned long __kernel_fsblkcnt_t ;typedef unsigned long __kernel_fsfilcnt_t ;typedef unsigned int __kernel_id_t ;typedef __u32 __kernel_dev_t ;typedef __kernel_blkcnt_t blkcnt_t ;typedef __kernel_blksize_t blksize_t ;typedef __kernel_clock_t clock_t ;typedef __kernel_clockid_t clockid_t ;typedef __kernel_dev_t dev_t ;typedef __kernel_fsblkcnt_t fsblkcnt_t ;typedef __kernel_fsfilcnt_t fsfilcnt_t ;typedef __kernel_gid32_t gid_t ;typedef __kernel_id_t id_t ;typedef __kernel_ino_t ino_t ;typedef __kernel_key_t key_t ;typedef __kernel_mode_t mode_t ;typedef __kernel_nlink_t nlink_t ;typedef __kernel_off_t off_t ;typedef __kernel_loff_t loff_t ;typedef loff_t off64_t ;typedef __kernel_pid_t pid_t ;typedef __kernel_suseconds_t suseconds_t ;typedef __kernel_time_t time_t ;typedef __kernel_uid32_t uid_t ;typedef signed long useconds_t ;typedef __kernel_daddr_t daddr_t ;typedef __kernel_timer_t timer_t ;typedef __kernel_mqd_t mqd_t ;typedef __kernel_caddr_t caddr_t ;typedef unsigned int uint_t ;typedef unsigned int uint ;typedef unsigned char u_char ;typedef unsigned short u_short ;typedef unsigned int u_int ;typedef unsigned long u_long ;typedef uint32_t u_int32_t ;typedef uint16_t u_int16_t ;typedef uint8_t u_int8_t ;typedef uint64_t u_int64_t ;typedef unsigned long sigset_t ;typedef void __signalfn_t ( int ) ;typedef __signalfn_t *__sighandler_t ;typedef void __restorefn_t ( void ) ;typedef __restorefn_t *__sigrestore_t ;typedef union sigval {
 int sival_int ;
 void *sival_ptr ;
 } sigval_t ;typedef int sig_atomic_t ;typedef void ( *sig_t ) ( int ) ;typedef sig_t sighandler_t ;typedef __builtin_va_list __gnuc_va_list ;typedef off_t fpos_t ;typedef __gnuc_va_list va_list ;typedef long pthread_mutexattr_t ;typedef long pthread_condattr_t ;typedef int pthread_key_t ;typedef long pthread_t ;typedef volatile int pthread_once_t ;typedef int pthread_rwlockattr_t ;typedef void ( *__pthread_cleanup_func_t ) ( void* ) ;typedef unsigned char BOOL ;typedef unsigned char UINT8 ;typedef unsigned short UINT16 ;typedef unsigned int UINT32 ;typedef signed char CHAR ;typedef signed char INT8 ;typedef signed short INT16 ;typedef signed int INT32 ;typedef unsigned short WORD ;typedef unsigned int DWORD ;typedef int HANDLE ;typedef HANDLE* LPHANDLE ;typedef unsigned char* PUINT8 ;typedef long LONG ;typedef char* LPCTSTR ;typedef char* LPTSTR ;typedef void* LPVOID ;typedef unsigned int* LPDWORD ;typedef unsigned int* PDWORD ;typedef unsigned int* PUINT32 ;typedef unsigned short TCHAR ;typedef unsigned int UINT ;typedef INT32 *PINT32 ;typedef INT16 *PINT16 ;typedef UINT16 *PUINT16 ;typedef INT8 *PINT8 ;typedef unsigned short sa_family_t ;typedef int socklen_t ;typedef __kernel_fd_set fd_set ;typedef unsigned char cc_t ;typedef unsigned int speed_t ;typedef unsigned int tcflag_t ;typedef const char * SwVersion ;typedef void VOID ;typedef int ( *DiagRxCallbackFunc ) ( char* packet , int len ) ;//ICAT EXPORTED ENUM 
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

typedef void ( *FunctionPtr ) ( void * , UINT32 ) ;typedef void ( *RSVPFunctionPtr ) ( void * , UINT32 , UINT32 ) ;typedef union {
 UINT8 packedMsgData [ 13 ] ;
 UINT8 msgData [ 13 ] ;
 } PackedUnPackedData ;typedef void* OSATaskRef ;typedef void* OSASemaRef ;typedef void* OSAMutexRef ;typedef void* OSAMsgQRef ;typedef void* OSAMailboxQRef ;typedef void* OSAPoolRef ;typedef void* OSATimerRef ;typedef void* OSAFlagRef ;typedef UINT8 OSA_STATUS ;typedef void* OSTaskRef ;typedef void* OSSemaRef ;typedef void* OSMutexRef ;typedef void* OSMsgQRef ;typedef void* OSMailboxQRef ;typedef void* OSPoolRef ;typedef void* OSTimerRef ;typedef void* OSFlagRef ;typedef UINT8 OS_STATUS ;typedef UINT32 OS_Hisr_t ;typedef UINT32 OS_MemPool_t ;typedef pid_t OS_Proc_t ;typedef void ( *funcPtrType ) ( void* ) ;typedef void ( *timerCBPtrType ) ( UINT32 ) ;typedef void ( *CommandAddress ) ( void ) ;typedef char* CommandProto ;typedef const char * DiagDBVersion ;//ICAT EXPORTED ENUM 
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

typedef void ( *DiagICATReadyNotifyEventFunc ) ( void ) ;typedef void ( *DiagEnterBootLoaderCBFunc ) ( void ) ;typedef void ( *Diag_Open_Func ) ( void ) ;typedef int ( *EventSocket_Open_Func ) ( void ) ;typedef void ( *Diag_Read_Task ) ( void * ) ;typedef int ( *Diag_Send_Target ) ( UINT8 * , UINT32 ) ;//ICAT EXPORTED ENUM 
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

//ICAT EXPORTED ENUM 
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

typedef void ( *PROD_PROCESS_FP ) ( const unsigned char * , int len ) ;typedef uint32_t in_addr_t ;DIAG_FILTER ( VALI_IF , ATCMD_IF , ATOUT_CHAR , 0)  
 diagPrintf ( " %s " , buffer );

DIAG_FILTER ( VALI_IF , ATCMD_IF , ATOUT , 0)  
 diagPrintf ( " %s " , buffer_out );

DIAG_FILTER ( Diag , Utils , Print_RTC , DIAG_INFORMATION)  
 diagPrintf ( " %s " , sCalanderTime );

DIAG_FILTER ( Diag , Utils , ReadRTC_Failure , DIAG_INFORMATION)  
 diagPrintf ( " Failed to read RTC " );

//ICAT EXPORTED FUNCTION - VALI_IF , ATCMD_IF , StartATCmdIF 
 void StartATCmdIF ( ) 
 {	 
 pthread_attr_t tattr ;	 
	 
 void initCittyConnection ( ) ;	 
	 
 if ( SerialClientIsInit )	 
 {		 
 // Already init - get out!		 
DIAG_FILTER ( VALI_IF , ATCMD_IF , ALREADYRUNNING , 0)  
 diagPrintf ( " Error -> Already Running! " );

		 
 return ;		 
 }	 
	 
 /*	 
 * Open the Device	 
 */	 
	 
 initCittyConnection ( ) ;	 
	 
 /*	 
 * Create a thread to recieve	 
 */	 
 pthread_attr_init ( &tattr ) ;	 
	 
 pthread_attr_setdetachstate ( &tattr , 0x00000001 ) ;	 
	 
 SerialClientIsInit = 1 ;	 
	 
 if ( pthread_create ( &InputThread , &tattr , ( void * ) ReceiveDataATCmdIF , ( ( void * ) 0 ) ) != 0 )	 
 {		 
		 
 perror ( " pthread_create " ) ;		 
 exit ( 1 ) ;		 
 }	 
	 
 }

//ICAT EXPORTED FUNCTION - VALI_IF , ATCMD_IF , StopATCmdIF 
 void StopATCmdIF ( ) 
 {	 
 if ( !SerialClientIsInit )	 
 {		 
 // It was not init , nothing to close - get out!		 
DIAG_FILTER ( VALI_IF , ATCMD_IF , STOPIFNOTSTARTED , 0)  
 diagPrintf ( " Interface was not started jet " );

		 
 return ;		 
 }	 
	 
	 
	 
 SerialClientIsInit = 0 ;	 
 }

//ICAT EXPORTED FUNCTION - VALI_IF , ATCMD_IF , SendATCmd 
 void SendATCmd ( char* Cmd ) 
 {	 
 unsigned char buf [ 514 ] ;	 
 int rc , i ;	 
 int recieved ;	 
	 
 eeh_log ( 5 , " >>>>>>>>>Enter SendAT CMD!!!\r\n " ) ;	 
 eeh_log ( 5 , " >>>SerialClientIsInit: %d\r\n " , SerialClientIsInit ) ;	 
 // Check for init	 
 if ( !SerialClientIsInit )	 
 {		 
DIAG_FILTER ( VALI_IF , ATCMD_IF , ATCMDIFNOTSTARTEDINSENDAT , 0)  
 diagPrintf ( " Received AT Command but interface not started " );

		 
 return ;		 
 }	 
 recieved = 0 ;	 
 for ( i = 0 ; i < ( int ) sizeof ( buf ) - 1 ; i++ )	 
 {		 
 int symb ;		 
 symb = ( int ) ( * ( unsigned char* ) Cmd ) ;		 
 // Check for ASCII		 
 if ( isprint ( symb ) || ( symb == 0x1a ) ) // Check for printable and cntrl-Z		 
 {			 
 buf [ i ] = ( * ( unsigned char* ) Cmd ) ;			 
 Cmd++ ;			 
 recieved++ ;			 
 }		 
 else		 
 {			 
 break ;			 
 }		 
 }	 
 if ( recieved > 0 )	 
 {		 
 if ( buf [ recieved - 1 ] == ' \a ' || buf [ recieved - 1 ] == 0xa )		 
 {			 
 buf [ recieved - 1 ] = 0xd ;			 
 }		 
 else		 
 {			 
 buf [ recieved++ ] = 0xd ;			 
 }		 
 buf [ recieved ] = 0 ; // Add NULL termination to the string		 
 eeh_log ( 5 , " >>>SendATCmd: %s\r\n " , buf ) ;		 
 rc = write ( serialfd , buf , recieved ) ;		 
		 
 if ( rc < 0 )		 
 {			 
 eeh_log ( 5 , " cannot send data " ) ;			 
 // close ( sd ) ;			 
 // exit ( 1 ) ;			 
			 
 }		 
 }	 
 }

//ICAT EXPORTED FUNCTION - VALI_IF , ATCMD_IF , SendATCmdChars 
 void SendATCmdChars ( char* Cmd ) 
 {	 
 int rc ;	 
 int recieved ;	 
	 
 // Check for init	 
 if ( !SerialClientIsInit )	 
 {		 
DIAG_FILTER ( VALI_IF , ATCMD_IF , ATCMDIFNOTSTARTEDINSENDATCHAR , 0)  
 diagPrintf ( " Received AT Command but interface not started " );

		 
 return ;		 
 }	 
	 
 recieved = strlen ( Cmd ) ;	 
 rc = write ( serialfd , Cmd , recieved ) ;	 
	 
 if ( rc < 0 )	 
 {		 
 perror ( " cannot send data " ) ;		 
 // close ( sd ) ;		 
 // exit ( 1 ) ;		 
 }	 
	 
 }

//ICAT EXPORTED FUNCTION - VALI_IF , MISC_IF , SendSystem 
 void SendSystem ( char* Cmd ) 
 {	 
 char buf [ 514 ] ;	 
 int i ;	 
 int recieved ;	 
	 
 recieved = 0 ;	 
 for ( i = 0 ; i < ( int ) sizeof ( buf ) - 1 ; i++ )	 
 {		 
 int symb ;		 
 symb = ( int ) ( * ( unsigned char* ) Cmd ) ;		 
 // Check for ASCII		 
 if ( isprint ( symb ) ) // Check for printable		 
 {			 
 buf [ i ] = ( * ( unsigned char* ) Cmd ) ;			 
 Cmd++ ;			 
 recieved++ ;			 
 }		 
 else		 
 {			 
 break ;			 
 }		 
 }	 
	 
 buf [ recieved ] = 0 ; // Add NULL termination to the string	 
DIAG_FILTER ( VALI_IF , MISC_IF , ATCMDSYSTEM , 0)  
 diagPrintf ( " Received command: %s " , buf );

	 
	 
 eeh_log ( 5 , " >>>SendSystem:%s\r\n " , buf ) ;	 
	 
	 
 if ( strncmp ( buf , " reboot " , strlen ( " reboot " ) ) == 0 )	 
 reboot ( 0x01234567 ) ;	 
 else	 
	 
 system ( buf ) ;	 
	 
 }

DIAG_FILTER ( DIAG , Dispatcher , ExportedPtr , DIAG_INFO)  
 diagPrintf ( " Dispatching exported function pointer [ 0x%x%x ] " , ( ( ( unsigned int ) cat3CommandGlobalAddress [ fID - _FirstCommandID ] ) >> 16 ) & 0xffff , ( ( unsigned int ) cat3CommandGlobalAddress [ fID - _FirstCommandID ] ) & 0xffff );

DIAG_FILTER ( SW_PLAT , DIAG , COMMANDID_WRONG , DIAG_ERROR)  
 diagPrintf ( " DIAG - command received with ID out of range: %d " , fID );

DIAG_FILTER ( SW_PLAT , DIAG , RX_SERVICEID_WRONG , DIAG_FATAL_ERROR)  
 diagPrintf ( " DIAG - msg received with wrong RX service ID , the serviceID is %d " , serviceID );

//ICAT EXPORTED FUNCTION - Diag , timeprint , diagRcvDataFromCPTask 
 void diagRcvDataFromCPTask ( void *data ) 
 {	 
 ( void ) data ;	 
	 
 UINT8 * diagDataRecvMsg , * rawPacket ;	 
 int dwBytesTransferred = -1 ;	 
 UINT32 *pData ;	 
 DIAGHDR *pHeader ;	 
 UINT16 pduLen , received ;	 
 void diagSCInitTask ( void *data ) ;	 
 BOOL Wraped = 1 ;	 
 struct diag_target_buffer *diag_buffer ;	 
	 
 diag_buffer = malloc ( sizeof ( struct diag_target_buffer ) ) ;	 
 if ( !diag_buffer )	 
 {		 
 eeh_log ( 5 , " ***** DIAG CP Chunk Buff is NULL *****\r\n " ) ;		 
 do { if ( ! ( 0 ) ) { char buffer [ 512 ] ; sprintf ( buffer , " ASSERT FAIL AT FILE %s FUNC %s LINE %d " , " diag_al.c " , __FUNCTION__ , 1075 ) ; eeh_draw_panic_text ( buffer , strlen ( buffer ) , 0 ) ; } } while ( 0 ) ;		 
 }	 
 diag_disc_flush ( diag_buffer ) ;	 
 diagDataRecvMsg = malloc ( 2048 + 6 ) ; // align?	 
 if ( !diagDataRecvMsg )	 
 {		 
 eeh_log ( 5 , " ***** DIAG CP RX TransferBuffer is NULL *****\r\n " ) ;		 
 free ( diag_buffer ) ;		 
 do { if ( ! ( 0 ) ) { char buffer [ 512 ] ; sprintf ( buffer , " ASSERT FAIL AT FILE %s FUNC %s LINE %d " , " diag_al.c " , __FUNCTION__ , 1083 ) ; eeh_draw_panic_text ( buffer , strlen ( buffer ) , 0 ) ; } } while ( 0 ) ;		 
 }	 
 eeh_log ( 5 , " *****DIAG Enter diagRcvDataFromCPTask!!!\n " ) ;	 
 received = 0 ;	 
 while ( 1 )	 
 {		 
 dwBytesTransferred = mrecv ( mydiagsockfd , diagDataRecvMsg , 2048 , 0 ) ;		 
 if ( dwBytesTransferred <= 0 )		 
 continue ;		 
		 
 pHeader = ( DIAGHDR* ) diagDataRecvMsg ;		 
		 
 if ( pHeader->packetlen == 0 )		 
 {			 
 eeh_log ( 5 , " Meet Zero packet len !!!\n " ) ;			 
 continue ;			 
 }		 
		 
 rawPacket = diagDataRecvMsg + sizeof ( DIAGHDR ) ;		 
 pData = ( UINT32* ) ( diagDataRecvMsg + sizeof ( DIAGHDR ) ) ;		 
		 
 switch ( *pData )		 
 {			 
 case 0x6d727632 :			 
 bDiagChannelInited = 1 ;			 
 eeh_log ( 5 , " >>DiagDataConfirmStartMsg\n " ) ;			 
 printf ( " >>>DiagDataConfirmStartMsg\n " ) ;			 
 break ;			 
 case 0x6d727634 :			 
 bDiagConnAcked = 1 ;			 
 eeh_log ( 5 , " >>DiagExtIfConnectedAckMsg\n " ) ;			 
 printf ( " >>>DiagExtIfConnectedAckMsg\n " ) ;			 
 break ;			 
 case 0x6d727636 :			 
 bDiagDiscAcked = 1 ;			 
 break ;			 
 case 0xFFFE :			 
 eeh_log ( 5 , " %s: received MsocketLinkdownProcId!\n " , __FUNCTION__ ) ;			 
 bDiagChannelInited = 0 ;			 
 bDiagConnAcked = 0 ;			 
 bDiagDiscAcked = 0 ;			 
 break ;			 
 case 0xFFFD :			 
 eeh_log ( 5 , " %s: received MsocketLinkupProcId!\n " , __FUNCTION__ ) ;			 
			 
 // ' pthread_cancel ' is not implemented in Android libpthread.			 
 // Fortunately , we don ' t need pthread_cancel here.			 
 //			 
 // if ( pthread_cancel ( diagSCInit ) == -1 )			 
 // {			 
 // DIAGPRINT ( " pthread_cancel return -1\r\n " ) ;			 
 // }			 
			 
 if ( pthread_create ( &diagSCInit , ( ( void * ) 0 ) , ( void * ) diagInitTask , ( ( void * ) 0 ) ) != 0 )			 
 {				 
 eeh_log ( 5 , " pthread_create diagRcvDataFromCPTask error " ) ;				 
 free ( diag_buffer ) ;				 
 free ( diagDataRecvMsg ) ;				 
 return ;				 
 }			 
			 
 break ;			 
			 
 default:			 
 pduLen = dwBytesTransferred - sizeof ( DIAGHDR ) ;			 
 if ( hPort >= 0 || diag_target.hPort >=0 )			 
 {				 
 if ( ( diag_count % 1000 == 0 ) && Wraped )				 
 {					 
 eeh_log ( 5 , " diag_count:%d , read RTC\n " , diag_count ) ;					 
 printRTC ( ) ;					 
 }				 
 {					 
 char * pbuff = ( char * ) pData ;					 
 int ret ;					 
 do					 
 {						 
 ret = diag_disc_rx ( diag_buffer , pbuff , pduLen ) ;						 
 if ( ret >= 0 )						 
 {							 
 SendCommMsg ( ( UINT8 * ) diag_buffer->chunk , diag_buffer->chunk_size ) ;							 
 diag_disc_flush ( diag_buffer ) ;							 
 pbuff += ( pduLen-ret ) ;							 
 pduLen = ret ;							 
 diag_count++ ;							 
 }						 
						 
 } while ( ret > 0 ) ;					 
 }				 
 }			 
 break ;			 
 }		 
 }	 
 free ( diagDataRecvMsg ) ;	 
 free ( diag_buffer ) ;	 
 eeh_log ( 5 , " Exit diagRcvDataFromCPTask!!!\n " ) ;	 
 }

//ICAT EXPORTED FUNCTION - SYSTEM , PROD , AT_SER 
 void AT_SER ( void ) 
 {	 
 extern void disconnectCOMM ( ) ;	 
 unsigned long code = 0x00000001 UL ;	 
	 
DIAG_FILTER ( SYSTEM , PROD , AT_SER_OK , DIAG_INFO)  
 diagPrintf ( " OK " );

	 
	 
 eeh_log ( 5 , " %s: close diag " , __FUNCTION__ ) ;	 
	 
 if ( get_bspflag_from_kernel_cmdline ( ) != 2 )	 
 return ;	 
	 
 // stop diag	 
 is_diag_running = 0 ;	 
 disconnectCOMM ( ) ;	 
	 
 // send message to start at command server	 
 if ( diag_prod_ctrl.fd != -1 )	 
 write ( diag_prod_ctrl.fd , &code , sizeof ( code ) ) ;	 
 }

