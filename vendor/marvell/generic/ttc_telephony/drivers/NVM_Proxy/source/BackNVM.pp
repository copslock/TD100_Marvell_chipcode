//PPL Source File Name : ../NVM_Proxy/source/BackNVM.i
//PPL Source File Name : source/BackNVM.c
typedef signed char __int8_t ;typedef unsigned char __uint8_t ;typedef short __int16_t ;typedef unsigned short __uint16_t ;typedef int __int32_t ;typedef unsigned int __uint32_t ;typedef long long __int64_t ;typedef unsigned long long __uint64_t ;typedef __int8_t __int_least8_t ;typedef __uint8_t __uint_least8_t ;typedef __int16_t __int_least16_t ;typedef __uint16_t __uint_least16_t ;typedef __int32_t __int_least32_t ;typedef __uint32_t __uint_least32_t ;typedef __int64_t __int_least64_t ;typedef __uint64_t __uint_least64_t ;typedef __int32_t __int_fast8_t ;typedef __uint32_t __uint_fast8_t ;typedef __int32_t __int_fast16_t ;typedef __uint32_t __uint_fast16_t ;typedef __int32_t __int_fast32_t ;typedef __uint32_t __uint_fast32_t ;typedef __int64_t __int_fast64_t ;typedef __uint64_t __uint_fast64_t ;typedef int __intptr_t ;typedef unsigned int __uintptr_t ;typedef __int64_t __intmax_t ;typedef __uint64_t __uintmax_t ;typedef __int32_t __register_t ;typedef unsigned long __vaddr_t ;typedef unsigned long __paddr_t ;typedef unsigned long __vsize_t ;typedef unsigned long __psize_t ;typedef int __clock_t ;typedef int __clockid_t ;typedef long __ptrdiff_t ;typedef int __time_t ;typedef int __timer_t ;typedef __builtin_va_list __va_list ;typedef int __wchar_t ;typedef int __wint_t ;typedef int __rune_t ;typedef void * __wctrans_t ;typedef void * __wctype_t ;typedef unsigned long __cpuid_t ;typedef __int32_t __dev_t ;typedef __uint32_t __fixpt_t ;typedef __uint32_t __gid_t ;typedef __uint32_t __id_t ;typedef __uint32_t __in_addr_t ;typedef __uint16_t __in_port_t ;typedef __uint32_t __ino_t ;typedef long __key_t ;typedef __uint32_t __mode_t ;typedef __uint32_t __nlink_t ;typedef __int32_t __pid_t ;typedef __uint64_t __rlim_t ;typedef __uint16_t __sa_family_t ;typedef __int32_t __segsz_t ;typedef __uint32_t __socklen_t ;typedef __int32_t __swblk_t ;typedef __uint32_t __uid_t ;typedef __uint32_t __useconds_t ;typedef __int32_t __suseconds_t ;typedef union {
 char __mbstate8 [ 128 ] ;
 __int64_t __mbstateL ;
 } __mbstate_t ;typedef __builtin_va_list __gnuc_va_list ;typedef unsigned int size_t ;typedef int ptrdiff_t ;typedef unsigned int wchar_t ;typedef __int8_t int8_t ;typedef __uint8_t uint8_t ;typedef __int16_t int16_t ;typedef __uint16_t uint16_t ;typedef __int32_t int32_t ;typedef __uint32_t uint32_t ;typedef __int64_t int64_t ;typedef __uint64_t uint64_t ;typedef int8_t int_least8_t ;typedef int8_t int_fast8_t ;typedef uint8_t uint_least8_t ;typedef uint8_t uint_fast8_t ;typedef int16_t int_least16_t ;typedef int32_t int_fast16_t ;typedef uint16_t uint_least16_t ;typedef uint32_t uint_fast16_t ;typedef int32_t int_least32_t ;typedef int32_t int_fast32_t ;typedef uint32_t uint_least32_t ;typedef uint32_t uint_fast32_t ;typedef int64_t int_least64_t ;typedef int64_t int_fast64_t ;typedef uint64_t uint_least64_t ;typedef uint64_t uint_fast64_t ;typedef int intptr_t ;typedef unsigned int uintptr_t ;typedef uint64_t uintmax_t ;typedef int64_t intmax_t ;typedef long int ssize_t ;typedef void ( *__kernel_sighandler_t ) ( int ) ;typedef int __kernel_key_t ;typedef int __kernel_mqd_t ;typedef unsigned long __kernel_ino_t ;typedef unsigned short __kernel_mode_t ;typedef unsigned short __kernel_nlink_t ;typedef long __kernel_off_t ;typedef int __kernel_pid_t ;typedef unsigned short __kernel_ipc_pid_t ;typedef unsigned short __kernel_uid_t ;typedef unsigned short __kernel_gid_t ;typedef unsigned int __kernel_size_t ;typedef int __kernel_ssize_t ;typedef int __kernel_ptrdiff_t ;typedef long __kernel_time_t ;typedef long __kernel_suseconds_t ;typedef long __kernel_clock_t ;typedef int __kernel_timer_t ;typedef int __kernel_clockid_t ;typedef int __kernel_daddr_t ;typedef char * __kernel_caddr_t ;typedef unsigned short __kernel_uid16_t ;typedef unsigned short __kernel_gid16_t ;typedef unsigned int __kernel_uid32_t ;typedef unsigned int __kernel_gid32_t ;typedef unsigned short __kernel_old_uid_t ;typedef unsigned short __kernel_old_gid_t ;typedef unsigned short __kernel_old_dev_t ;typedef long long __kernel_loff_t ;typedef unsigned short umode_t ;typedef __signed__ char __s8 ;typedef unsigned char __u8 ;typedef __signed__ short __s16 ;typedef unsigned short __u16 ;typedef __signed__ int __s32 ;typedef unsigned int __u32 ;typedef __signed__ long long __s64 ;typedef unsigned long long __u64 ;typedef __u16 __le16 ;typedef __u16 __be16 ;typedef __u32 __le32 ;typedef __u32 __be32 ;typedef __u64 __le64 ;typedef __u64 __be64 ;typedef unsigned long __kernel_blkcnt_t ;typedef unsigned long __kernel_blksize_t ;typedef unsigned long __kernel_fsblkcnt_t ;typedef unsigned long __kernel_fsfilcnt_t ;typedef unsigned int __kernel_id_t ;typedef __u32 __kernel_dev_t ;typedef __kernel_blkcnt_t blkcnt_t ;typedef __kernel_blksize_t blksize_t ;typedef __kernel_clock_t clock_t ;typedef __kernel_clockid_t clockid_t ;typedef __kernel_dev_t dev_t ;typedef __kernel_fsblkcnt_t fsblkcnt_t ;typedef __kernel_fsfilcnt_t fsfilcnt_t ;typedef __kernel_gid32_t gid_t ;typedef __kernel_id_t id_t ;typedef __kernel_ino_t ino_t ;typedef __kernel_key_t key_t ;typedef __kernel_mode_t mode_t ;typedef __kernel_nlink_t nlink_t ;typedef __kernel_off_t off_t ;typedef __kernel_loff_t loff_t ;typedef loff_t off64_t ;typedef __kernel_pid_t pid_t ;typedef __kernel_suseconds_t suseconds_t ;typedef __kernel_time_t time_t ;typedef __kernel_uid32_t uid_t ;typedef signed long useconds_t ;typedef __kernel_daddr_t daddr_t ;typedef __kernel_timer_t timer_t ;typedef __kernel_mqd_t mqd_t ;typedef __kernel_caddr_t caddr_t ;typedef unsigned int uint_t ;typedef unsigned int uint ;typedef unsigned char u_char ;typedef unsigned short u_short ;typedef unsigned int u_int ;typedef unsigned long u_long ;typedef uint32_t u_int32_t ;typedef uint16_t u_int16_t ;typedef uint8_t u_int8_t ;typedef uint64_t u_int64_t ;typedef off_t fpos_t ;typedef unsigned long sigset_t ;typedef void __signalfn_t ( int ) ;typedef __signalfn_t *__sighandler_t ;typedef void __restorefn_t ( void ) ;typedef __restorefn_t *__sigrestore_t ;typedef union sigval {
 int sival_int ;
 void *sival_ptr ;
 } sigval_t ;typedef int sig_atomic_t ;typedef void ( *sig_t ) ( int ) ;typedef sig_t sighandler_t ;typedef __kernel_fd_set fd_set ;typedef unsigned char BOOL ;typedef unsigned char UINT8 ;typedef unsigned short UINT16 ;typedef unsigned int UINT32 ;typedef signed char CHAR ;typedef signed char INT8 ;typedef signed short INT16 ;typedef signed int INT32 ;typedef unsigned short WORD ;typedef unsigned int DWORD ;typedef int HANDLE ;typedef HANDLE* LPHANDLE ;typedef unsigned char* PUINT8 ;typedef long LONG ;typedef char* LPCTSTR ;typedef char* LPTSTR ;typedef void* LPVOID ;typedef unsigned int* LPDWORD ;typedef unsigned int* PDWORD ;typedef unsigned int* PUINT32 ;typedef unsigned short TCHAR ;typedef unsigned int UINT ;typedef INT32 *PINT32 ;typedef INT16 *PINT16 ;typedef UINT16 *PUINT16 ;typedef INT8 *PINT8 ;typedef const char * SwVersion ;typedef __gnuc_va_list va_list ;typedef void VOID ;typedef int ( *DiagRxCallbackFunc ) ( char* packet , int len ) ;DIAG_FILTER ( FDI , Transport , BackNVMOpenError , DIAG_ERROR)  
 diagPrintf ( " Cannot open BackNVM: %s " , " / dev / mtdblock1 " );

DIAG_FILTER ( FDI , Transport , StoreBackNVMLimitError , DIAG_ERROR)  
 diagPrintf ( " BackNVM limit err: %d > %d " , size + len + 4 + 4 , ( 0x20000 - 0x200 ) );

DIAG_FILTER ( FDI , Transport , StoreBackNVMError , DIAG_ERROR)  
 diagPrintf ( " Error on storing %s to BackNVM [ %d ] " , p , num );

DIAG_FILTER ( FDI , Transport , StoreBackNVMFile , DIAG_INFORMATION)  
 diagPrintf ( " Succeeded in storing %s to BackNVM [ %d ] " , p , num );

DIAG_FILTER ( FDI , Transport , LoadBackNVMError , DIAG_ERROR)  
 diagPrintf ( " Error on loading %s from BackNVM [ %d ] " , p , num );

DIAG_FILTER ( FDI , Transport , LoadBackNVMWarning , DIAG_INFORMATION)  
 diagPrintf ( " Length mismatch: loading %s from BackNVM [ %d ] " , p , num );

DIAG_FILTER ( FDI , Transport , LoadBackNVMFile , DIAG_INFORMATION)  
 diagPrintf ( " Succeeded in loading %s from BackNVM [ %d ] " , p , num );

//ICAT EXPORTED FUNCTION - FDI , Transport , IsBackNVMValid 
 BOOL IsBackNVMValid ( void ) 
 {	 
 int year , mon , day , hour , min , sec ;	 
	 
 year = mon = day = hour = min = sec = 0 ;	 
	 
 do {		 
 /* BackNVM should include at least " time\nBackNVM.cfg\nCRC32 " */		 
 if ( lenBackNVM < 12 + 12 + 4 )		 
 break ;		 
		 
 /* Check time and the first filename */		 
 if ( sscanf ( bufBackNVM , " %d / %d / %d , %d:%d:%d\nBackNVM.cfg\n " ,		 
 &day , &mon , &year , &hour , &min , &sec ) != 6 )		 
 break ;		 
		 
 /* Check the checksum */		 
 if ( CRC32 ( ( unsigned char * ) bufBackNVM , lenBackNVM ) != crcBackNVM )		 
 break ;		 
		 
DIAG_FILTER ( FDI , Transport , BackNVMValid , DIAG_INFORMATION)  
 diagPrintf ( " Valid BackNVM ( CRC=%d ) at %d / %d / %d , %d:%d:%d " , 
 crcBackNVM , day , mon , year , hour , min , sec );

		 
		 
 return 1 ;		 
 } while ( 0 ) ;	 
	 
 /* error exit */	 
DIAG_FILTER ( FDI , Transport , BackNVMInvalid , DIAG_ERROR)  
 diagPrintf ( " Invalid BackNVM ( CRC=%d ) at %d / %d / %d , %d:%d:%d " , 
 crcBackNVM , day , mon , year , hour , min , sec );

	 
	 
 return 0 ;	 
 }

//ICAT EXPORTED FUNCTION - FDI , Transport , IsBackNVMCfgValid 
 BOOL IsBackNVMCfgValid ( void ) 
 {	 
 UINT32 len ;	 
 char *buf = ( ( void * ) 0 ) ;	 
 BOOL ret ;	 
	 
 /* Load BackNVM.cfg */	 
 len = loadNVMFile ( " BackNVM.cfg " , &buf ) ;	 
 if ( len == 0 )	 
 {		 
 ret = 0 ;		 
 }	 
 else if ( IsBackNVMValid ( ) == 0 )	 
 {		 
 ret = 1 ;		 
 free ( buf ) ;		 
 }	 
 else	 
 {		 
 /* Check BackNVM.cfg */		 
 ret = isBackNVMCfgValid ( buf , len ) ;		 
 free ( buf ) ;		 
 }	 
	 
 if ( ret == 0 )	 
 {		 
DIAG_FILTER ( FDI , Transport , BackNVMCfgInvalid , DIAG_ERROR)  
 diagPrintf ( " Invalid BackNVM.cfg " );

		 
 }	 
 else	 
 {		 
DIAG_FILTER ( FDI , Transport , BackNVMCfgValid , DIAG_INFORMATION)  
 diagPrintf ( " Valid BackNVM.cfg " );

		 
 }	 
	 
 return ret ;	 
 }

//ICAT EXPORTED FUNCTION - FDI , Transport , StoreBackNVM 
 BOOL StoreBackNVM ( void ) 
 {	 
 int num ;	 
 UINT32 len ;	 
 char *buf ;	 
	 
 do {		 
 if ( updateBackNVMCfg ( ) == 0 )		 
 break ;		 
		 
 len = loadNVMFile ( " BackNVM.cfg " , &buf ) ;		 
 if ( len == 0 )		 
 break ;		 
		 
 if ( ( num = opBackNVM ( buf , len , 0 ) ) > 0 )		 
 {			 
 /* reload BackNVM to RAM cache */			 
 loadBackNVM ( ) ;			 
			 
 /* Check BackNVM again */			 
 if ( IsBackNVMValid ( ) == 0 )			 
 break ;			 
			 
DIAG_FILTER ( FDI , Transport , StoreBackNVMSuccess , DIAG_INFORMATION)  
 diagPrintf ( " Succeeded in storing %d files to BackNVM " , num );

			 
			 
 return 1 ;			 
 }		 
 } while ( 0 ) ;	 
	 
 /* error exit */	 
DIAG_FILTER ( FDI , Transport , StoreBackNVMFailure , DIAG_ERROR)  
 diagPrintf ( " Failed to store BackNVM " );

	 
	 
 return 0 ;	 
 }

//ICAT EXPORTED FUNCTION - FDI , Transport , LoadBackNVM 
 BOOL LoadBackNVM ( void ) 
 {	 
 int num ;	 
 UINT32 len ;	 
 char *buf ;	 
	 
 if ( IsBackNVMValid ( ) == 1 )	 
 {		 
 len = lenBackNVM ;		 
 buf = malloc ( len + 1 ) ;		 
 if ( buf != ( ( void * ) 0 ) )		 
 {			 
 memcpy ( buf , bufBackNVM , len ) ;			 
 buf [ len ] = 0 ;			 
 if ( ( num = opBackNVM ( buf , len , 1 ) ) > 0 )			 
 {				 
 /* Check BackNVM.cfg again */				 
 if ( IsBackNVMCfgValid ( ) == 1 )				 
 {					 
DIAG_FILTER ( FDI , Transport , LoadBackNVMSuccess , DIAG_INFORMATION)  
 diagPrintf ( " Succeeded in loading %d files from BackNVM " , num );

					 
 free ( buf ) ;					 
 return 1 ;					 
 }				 
 }			 
 free ( buf ) ;			 
 }		 
 }	 
	 
DIAG_FILTER ( FDI , Transport , LoadBackNVMFailure , DIAG_ERROR)  
 diagPrintf ( " Failed to load BackNVM " );

	 
	 
 return 0 ;	 
 }

//ICAT EXPORTED FUNCTION - FDI , Transport , CreateDefaultBackNVMCfg 
 void CreateDefaultBackNVMCfg ( ) 
 {	 
 FILE *fp = fopen ( " BackNVM.cfg " , " w " ) ;	 
 if ( fp != ( ( void * ) 0 ) )	 
 {		 
 fprintf ( fp , " 0 / 0 / 0 , 0 :0:0\n%s\n " , " BackNVM.cfg " ) ;		 
 fclose ( fp ) ;		 
		 
DIAG_FILTER ( FDI , Transport , CreateDefaultBackNVMCfgSuccess , DIAG_INFORMATION)  
 diagPrintf ( " Default %s created " , " BackNVM.cfg " );

		 
 } else	 
 {		 
DIAG_FILTER ( FDI , Transport , CreateDefaultBackNVMCfgFail , DIAG_INFORMATION)  
 diagPrintf ( " Error: Create default %s BACK NVM dir fail " , " BackNVM.cfg " );

		 
 }	 
 }

