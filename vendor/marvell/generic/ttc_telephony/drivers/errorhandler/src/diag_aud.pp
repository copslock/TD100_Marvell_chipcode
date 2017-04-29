//PPL Source File Name : ../errorhandler/src/diag_aud.i
//PPL Source File Name : diag_aud.c
typedef int ptrdiff_t ;typedef unsigned int size_t ;typedef unsigned int wchar_t ;typedef __signed char __int8_t ;typedef unsigned char __uint8_t ;typedef short __int16_t ;typedef unsigned short __uint16_t ;typedef int __int32_t ;typedef unsigned int __uint32_t ;typedef long long __int64_t ;typedef unsigned long long __uint64_t ;typedef __int8_t __int_least8_t ;typedef __uint8_t __uint_least8_t ;typedef __int16_t __int_least16_t ;typedef __uint16_t __uint_least16_t ;typedef __int32_t __int_least32_t ;typedef __uint32_t __uint_least32_t ;typedef __int64_t __int_least64_t ;typedef __uint64_t __uint_least64_t ;typedef __int32_t __int_fast8_t ;typedef __uint32_t __uint_fast8_t ;typedef __int32_t __int_fast16_t ;typedef __uint32_t __uint_fast16_t ;typedef __int32_t __int_fast32_t ;typedef __uint32_t __uint_fast32_t ;typedef __int64_t __int_fast64_t ;typedef __uint64_t __uint_fast64_t ;typedef int __intptr_t ;typedef unsigned int __uintptr_t ;typedef __int64_t __intmax_t ;typedef __uint64_t __uintmax_t ;typedef __int32_t __register_t ;typedef unsigned long __vaddr_t ;typedef unsigned long __paddr_t ;typedef unsigned long __vsize_t ;typedef unsigned long __psize_t ;typedef int __clock_t ;typedef int __clockid_t ;typedef long __ptrdiff_t ;typedef int __time_t ;typedef int __timer_t ;typedef __builtin_va_list __va_list ;typedef int __wchar_t ;typedef int __wint_t ;typedef int __rune_t ;typedef void * __wctrans_t ;typedef void * __wctype_t ;typedef unsigned long __cpuid_t ;typedef __int32_t __dev_t ;typedef __uint32_t __fixpt_t ;typedef __uint32_t __gid_t ;typedef __uint32_t __id_t ;typedef __uint32_t __in_addr_t ;typedef __uint16_t __in_port_t ;typedef __uint32_t __ino_t ;typedef long __key_t ;typedef __uint32_t __mode_t ;typedef __uint32_t __nlink_t ;typedef __int32_t __pid_t ;typedef __uint64_t __rlim_t ;typedef __uint16_t __sa_family_t ;typedef __int32_t __segsz_t ;typedef __uint32_t __socklen_t ;typedef __int32_t __swblk_t ;typedef __uint32_t __uid_t ;typedef __uint32_t __useconds_t ;typedef __int32_t __suseconds_t ;typedef union {
 char __mbstate8 [ 128 ] ;
 __int64_t __mbstateL ;
 } __mbstate_t ;typedef __int8_t int8_t ;typedef __uint8_t uint8_t ;typedef __int16_t int16_t ;typedef __uint16_t uint16_t ;typedef __int32_t int32_t ;typedef __uint32_t uint32_t ;typedef __int64_t int64_t ;typedef __uint64_t uint64_t ;typedef int8_t int_least8_t ;typedef int8_t int_fast8_t ;typedef uint8_t uint_least8_t ;typedef uint8_t uint_fast8_t ;typedef int16_t int_least16_t ;typedef int32_t int_fast16_t ;typedef uint16_t uint_least16_t ;typedef uint32_t uint_fast16_t ;typedef int32_t int_least32_t ;typedef int32_t int_fast32_t ;typedef uint32_t uint_least32_t ;typedef uint32_t uint_fast32_t ;typedef int64_t int_least64_t ;typedef int64_t int_fast64_t ;typedef uint64_t uint_least64_t ;typedef uint64_t uint_fast64_t ;typedef int intptr_t ;typedef unsigned int uintptr_t ;typedef uint64_t uintmax_t ;typedef int64_t intmax_t ;typedef long int ssize_t ;typedef unsigned char ACM_AudioVolume ;typedef signed char ACM_DigitalGain ;typedef signed char ACM_AnalogGain ;typedef void ( *ACMComponentInit_t ) ( unsigned char reinit , ACM_SspAudioConfiguration **componentSspConfig , signed short **ditherGenConfig ) ;typedef ACM_DigitalGain ( *ACMEnablePath_t ) ( unsigned char path , ACM_AudioVolume volume ) ;typedef ACM_DigitalGain ( *ACMDisablePath_t ) ( unsigned char path ) ;typedef ACM_DigitalGain ( *ACMVolumeSetPath_t ) ( unsigned char path , ACM_AudioVolume volume ) ;typedef ACM_DigitalGain ( *ACMMutePath_t ) ( unsigned char path , ACM_AudioMute mute ) ;typedef ACM_DigitalGain ( *ACMSetPathAnalogGain_t ) ( unsigned char regIndex , unsigned char regValue , unsigned char regMask , unsigned char regShift ) ;typedef short ( *ACMGetPathsStatus_t ) ( char* data , short length ) ;typedef ACM_AnalogGain ( *ACMGetPathAnalogGain_t ) ( unsigned char path ) ;typedef unsigned int ( *ACMGetActivePathNum_t ) ( void ) ;typedef unsigned int ( *ACMSetLoopbackTestStatus_t ) ( unsigned char isloopbacktest ) ;typedef unsigned int ( *ACMGetLoopbackTestStatus_t ) ( void ) ;typedef void ( *ACM_PacketTransferFunc ) ( unsigned int *buff ) ;typedef __builtin_va_list __gnuc_va_list ;typedef void ( *__kernel_sighandler_t ) ( int ) ;typedef int __kernel_key_t ;typedef int __kernel_mqd_t ;typedef unsigned long __kernel_ino_t ;typedef unsigned short __kernel_mode_t ;typedef unsigned short __kernel_nlink_t ;typedef long __kernel_off_t ;typedef int __kernel_pid_t ;typedef unsigned short __kernel_ipc_pid_t ;typedef unsigned short __kernel_uid_t ;typedef unsigned short __kernel_gid_t ;typedef unsigned int __kernel_size_t ;typedef int __kernel_ssize_t ;typedef int __kernel_ptrdiff_t ;typedef long __kernel_time_t ;typedef long __kernel_suseconds_t ;typedef long __kernel_clock_t ;typedef int __kernel_timer_t ;typedef int __kernel_clockid_t ;typedef int __kernel_daddr_t ;typedef char * __kernel_caddr_t ;typedef unsigned short __kernel_uid16_t ;typedef unsigned short __kernel_gid16_t ;typedef unsigned int __kernel_uid32_t ;typedef unsigned int __kernel_gid32_t ;typedef unsigned short __kernel_old_uid_t ;typedef unsigned short __kernel_old_gid_t ;typedef unsigned short __kernel_old_dev_t ;typedef long long __kernel_loff_t ;typedef unsigned short umode_t ;typedef __signed__ char __s8 ;typedef unsigned char __u8 ;typedef __signed__ short __s16 ;typedef unsigned short __u16 ;typedef __signed__ int __s32 ;typedef unsigned int __u32 ;typedef __signed__ long long __s64 ;typedef unsigned long long __u64 ;typedef __u16 __le16 ;typedef __u16 __be16 ;typedef __u32 __le32 ;typedef __u32 __be32 ;typedef __u64 __le64 ;typedef __u64 __be64 ;typedef unsigned long __kernel_blkcnt_t ;typedef unsigned long __kernel_blksize_t ;typedef unsigned long __kernel_fsblkcnt_t ;typedef unsigned long __kernel_fsfilcnt_t ;typedef unsigned int __kernel_id_t ;typedef __u32 __kernel_dev_t ;typedef __kernel_blkcnt_t blkcnt_t ;typedef __kernel_blksize_t blksize_t ;typedef __kernel_clock_t clock_t ;typedef __kernel_clockid_t clockid_t ;typedef __kernel_dev_t dev_t ;typedef __kernel_fsblkcnt_t fsblkcnt_t ;typedef __kernel_fsfilcnt_t fsfilcnt_t ;typedef __kernel_gid32_t gid_t ;typedef __kernel_id_t id_t ;typedef __kernel_ino_t ino_t ;typedef __kernel_key_t key_t ;typedef __kernel_mode_t mode_t ;typedef __kernel_nlink_t nlink_t ;typedef __kernel_off_t off_t ;typedef __kernel_loff_t loff_t ;typedef loff_t off64_t ;typedef __kernel_pid_t pid_t ;typedef __kernel_suseconds_t suseconds_t ;typedef __kernel_time_t time_t ;typedef __kernel_uid32_t uid_t ;typedef signed long useconds_t ;typedef __kernel_daddr_t daddr_t ;typedef __kernel_timer_t timer_t ;typedef __kernel_mqd_t mqd_t ;typedef __kernel_caddr_t caddr_t ;typedef unsigned int uint_t ;typedef unsigned int uint ;typedef unsigned char u_char ;typedef unsigned short u_short ;typedef unsigned int u_int ;typedef unsigned long u_long ;typedef uint32_t u_int32_t ;typedef uint16_t u_int16_t ;typedef uint8_t u_int8_t ;typedef uint64_t u_int64_t ;typedef off_t fpos_t ;typedef unsigned char BOOL ;typedef unsigned char UINT8 ;typedef unsigned short UINT16 ;typedef unsigned int UINT32 ;typedef signed char CHAR ;typedef signed char INT8 ;typedef signed short INT16 ;typedef signed int INT32 ;typedef unsigned short WORD ;typedef unsigned int DWORD ;typedef int HANDLE ;typedef HANDLE* LPHANDLE ;typedef unsigned char* PUINT8 ;typedef long LONG ;typedef char* LPCTSTR ;typedef char* LPTSTR ;typedef void* LPVOID ;typedef unsigned int* LPDWORD ;typedef unsigned int* PDWORD ;typedef unsigned int* PUINT32 ;typedef unsigned short TCHAR ;typedef unsigned int UINT ;typedef INT32 *PINT32 ;typedef INT16 *PINT16 ;typedef UINT16 *PUINT16 ;typedef INT8 *PINT8 ;typedef const char * SwVersion ;typedef void VOID ;typedef int ( *DiagRxCallbackFunc ) ( char* packet , int len ) ;//ICAT EXPORTED ENUM 
 typedef enum 
 {	 
 ON = 0 ,	 
 OFF ,	 
	 
 ON_OFF_ENUM_32_BIT = 0x7FFFFFFF /* 32 bit enum compiling enforcement */	 
 } On_Off;

//ICAT EXPORTED ENUM 
 typedef enum 
 {	 
 SPEAKER_PHONE_IIR1 ,	 
 HANDSET ,	 
 HEADSET ,	 
 CAR_KIT ,	 
 BT_HEADSET ,	 
 SPEAKER_PHONE_IIR2 , // Non voice call filter	 
	 
 BUILTIN_MIC_IIR_1 ,	 
 BUILTIN_MIC_IIR_2 ,	 
 MAX_APPLIANCE_VALUE ,	 
	 
 APPLIANCE_TYPE_ENUM_32_BIT = 0x7FFFFFFF /* 32 bit enum compiling enforcement */	 
 } Appliance;

//ICAT EXPORTED ENUM 
 typedef enum 
 {	 
 MP3 ,	 
 AAC ,	 
 OGG ,	 
 MP4 ,	 
 AVI ,	 
	 
 CONTAINER_TYPE_ENUM_32_BIT = 0x7FFFFFFF /* 32 bit enum compiling enforcement */	 
 } Container_Type;

//ICAT EXPORTED ENUM 
 typedef enum 
 {	 
 SYSTEM ,	 
 RINGTONE ,	 
 MUSIC ,	 
 ALARM ,	 
	 
 STREAM_TYPE_ENUM_32_BIT = 0x7FFFFFFF /* 32 bit enum compiling enforcement */	 
 } Stream_Type;

//ICAT EXPORTED STRUCT 
 typedef struct 
 {	 
 unsigned short nSampleRate ; // change from NOofTaps. ;	 
 signed short params [ 6 ] ;	 
 Appliance appliance ;	 
 On_Off onOff ;	 
 unsigned char description [ 32 ] ;	 
 } AUDIO_HiFi_EQ_Params;

//ICAT EXPORTED STRUCT 
 typedef struct 
 {	 
 Container_Type containerType ;	 
 Stream_Type streamType ;	 
 unsigned short volumeOffset ;	 
 } AUDIO_HiFi_Volume_Params;

//ICAT EXPORTED STRUCT 
 typedef struct 
 {	 
 unsigned short noOfTaps ;	 
 short params [ 6 ] ;	 
 unsigned long nSampleRate ;	 
 Appliance appliance ;	 
 unsigned long nBand ;	 
 On_Off OnOff ;	 
	 
 unsigned char description [ 32 ] ;	 
 } AUDIO_HiFi_MultiEQ_Params;

//ICAT EXPORTED STRUCT 
 typedef struct {	 
 int nOffset ; // Q16	 
 int nThreshold ; // Q16	 
 int nCompressFlag ; // 0 : expand 1 :compress	 
 int nRatio ; // Q16	 
 } ae_drc_gain_curve_t;

//ICAT EXPORTED STRUCT 
 typedef struct 
 {	 
 // Totally ( nNumOfDrcSections-1 ) threshold , compose nNumOfDrcSections sections ,	 
	 
 // eg: 3 threshold , 4 sections	 
	 
 // [ -100 , nThreshold0 ] [ nThreshold0 , nThreshold1 ] [ nThreshold1 , nThreshold2 ] [ nThreshold2 , 0 ]	 
	 
 ae_drc_gain_curve_t drcGainMap [ 5 ] ;	 
 unsigned int nNumOfDrcSections ;	 
 unsigned int nAttackTime ; // ms	 
 unsigned int nReleaseTime ; // ms	 
 } AUDIO_HIFI_DRC_Params;

//ICAT EXPORTED FUNCTION - Audio , HW , AUDIO_HW_PathEnable 
 void ACMAudioPathEnable_Env ( void *data ) 
 {	 
 int client , value = 0 ;	 
 unsigned char component = 0 , path = 0 ;	 
	 
DIAG_FILTER ( AUDIO , ACM , ACMAudioPathEnable , DIAG_ALL)  
 diagPrintf ( " ACMAudioPathEnable\n " );

	 
 /*	 
 *data [ 0 ] indicate componentId , data [ 1 ] indicate pathID ,	 
 *data [ 2 ] indicate path direction , data [ 3 ] indicate volume	 
 */	 
 value |= ( ACM_AudioVolume ) ( * ( ( unsigned char* ) data + 3 ) ) ; /*volume*/	 
 path = ( unsigned char ) ( * ( ( unsigned char * ) data + 1 ) ) ; /*pathnum*/	 
 component = ( ACM_Component ) ( * ( ( unsigned char * ) data ) ) ; /*component number*/	 
	 
 client = 0x00000003 ;	 
 value |= ( path << 16 ) | ( component << 24 ) ;	 
	 
 audio_route_control ( client , 0x00000001 , 0x00000005 , 0 , value ) ;	 
 }

//ICAT EXPORTED FUNCTION - Audio , HW , AUDIO_HW_PathDisable 
 void ACMAudioPathDisable_Env ( void *data ) 
 {	 
 int client , value = 0 ;	 
 unsigned char component = 0 , path = 0 ;	 
	 
DIAG_FILTER ( AUDIO , ACM , ACMAudioPathDisable , DIAG_ALL)  
 diagPrintf ( " ACMAudioPathDisable\n " );

	 
 /*	 
 *data [ 0 ] indicate componentId , data [ 1 ] indicate pathID ,	 
 *data [ 2 ] indicate path direction , data [ 3 ] indicate volume	 
 */	 
 path = ( unsigned char ) ( * ( ( unsigned short * ) data + 1 ) ) ; // pathnum	 
 component = ( ACM_Component ) ( * ( ( unsigned short * ) data ) ) ; // component number	 
	 
 client = 0x00000003 ;	 
 value |= ( path << 16 ) | ( component << 24 ) ;	 
	 
 audio_route_control ( client , 0x00000001 , 0x00000006 , 0 , value ) ;	 
 }

//ICAT EXPORTED FUNCTION - Audio , ACM , ACMAudioDeviceEnable 
 void ACMAudioDeviceEnable_Env ( void *data ) 
 {	 
 int client , device , value = 0 ;	 
	 
DIAG_FILTER ( AUDIO , ACM , ACMAudioDeviceEnable , DIAG_ALL)  
 diagPrintf ( " ACMAudioDeviceEnable\n " );

	 
	 
 value |= ( ACM_AudioVolume ) ( * ( ( unsigned short* ) data + 2 ) ) ; // volume	 
	 
 switch ( ( ACM_AudioFormat ) ( * ( ( unsigned short* ) data + 1 ) ) ) { // client		 
 case ACM_MSA_PCM:		 
 client = 0x00000003 ;		 
 break ;		 
 case ACM_I2S:		 
 client = 0x00000001 ;		 
 break ;		 
 case ACM_AUX_FM:		 
 client = 0x00000004 ;		 
 break ;		 
 default:		 
 return ;		 
 }	 
	 
 switch ( ( ACM_AudioDevice ) ( * ( ( unsigned short* ) data ) ) ) {		 
 case ACM_MAIN_SPEAKER:		 
 device = 0x00000001 ; // device		 
 value |= ( 0x00000002 << 16 ) ; // direction		 
 break ;		 
 case ACM_AUX_SPEAKER:		 
 device = 0x00000002 ;		 
 value |= ( 0x00000002 << 16 ) ;		 
 break ;		 
 case ACM_HEADSET_SPEAKER:		 
 device = 0x00000004 ;		 
 value |= ( 0x00000002 << 16 ) ;		 
 break ;		 
 case ACM_BLUETOOTH_SPEAKER:		 
 device = 0x00000003 ;		 
 value |= ( 0x00000002 << 16 ) ;		 
 break ;		 
 case ACM_MIC:		 
 device = 0x00000002 ;		 
 value |= ( 0x00000001 << 16 ) ;		 
 break ;		 
 case ACM_HEADSET_MIC:		 
 device = 0x00000004 ;		 
 value |= ( 0x00000001 << 16 ) ;		 
 break ;		 
 case ACM_BLUETOOTH_MIC:		 
 device = 0x00000003 ;		 
 value |= ( 0x00000001 << 16 ) ;		 
 break ;		 
 case ACM_MIC_EC:		 
 device = 0x00000002 ;		 
 value |= ( 0x00000001 << 16 ) ;		 
 value |= ( 0x00000080 << 16 ) ;		 
 break ;		 
 case ACM_HEADSET_MIC_EC:		 
 device = 0x00000004 ;		 
 value |= ( 0x00000001 << 16 ) ;		 
 value |= ( 0x00000080 << 16 ) ;		 
 break ;		 
 case ACM_AUX_MIC:		 
 if ( client != 0x00000004 )		 
 return ;		 
 device = ACM_AUX_SPEAKER ;		 
 value |= ( 0x00000001 << 16 ) ;		 
 break ;		 
 case ACM_MAIN_SPEAKER__LOOP:		 
 device = 0x00000001 ; // device		 
 value |= ( 0x00000002 << 16 ) ; // direction		 
 value |= ( 0x00000100 << 16 ) ; // loopback test		 
 break ;		 
 case ACM_AUX_SPEAKER__LOOP:		 
 device = 0x00000002 ;		 
 value |= ( 0x00000002 << 16 ) ;		 
 value |= ( 0x00000100 << 16 ) ; // loopback test		 
 break ;		 
 case ACM_HEADSET_SPEAKER__LOOP:		 
 device = 0x00000004 ;		 
 value |= ( 0x00000002 << 16 ) ;		 
 value |= ( 0x00000100 << 16 ) ; // loopback test		 
 break ;		 
 case ACM_MIC__LOOP:		 
 device = 0x00000002 ;		 
 value |= ( 0x00000001 << 16 ) ;		 
 value |= ( 0x00000100 << 16 ) ; // loopback test		 
 case ACM_AUX_MIC__LOOP:		 
 device = ACM_AUX_SPEAKER ;		 
 value |= ( 0x00000001 << 16 ) ;		 
 value |= ( 0x00000100 << 16 ) ; // loopback test		 
 break ;		 
 case ACM_HEADSET_MIC__LOOP:		 
 device = 0x00000004 ;		 
 value |= ( 0x00000001 << 16 ) ;		 
 value |= ( 0x00000100 << 16 ) ; // loopback test		 
 break ;		 
		 
 default:		 
 return ;		 
 }	 
	 
 audio_route_control ( client , 0x00000001 , 0x00000001 , device , value ) ;	 
 }

//ICAT EXPORTED FUNCTION - Audio , ACM , ACMAudioDeviceDisable 
 void ACMAudioDeviceDisable_Env ( void *data ) 
 {	 
 int client , device , value = 0 ;	 
	 
DIAG_FILTER ( AUDIO , ACM , ACMAudioDeviceDisable , DIAG_ALL)  
 diagPrintf ( " ACMAudioDeviceDisable\n " );

	 
	 
 switch ( ( ACM_AudioFormat ) ( * ( ( unsigned short* ) data + 1 ) ) ) { // client		 
 case ACM_MSA_PCM:		 
 client = 0x00000003 ;		 
 break ;		 
 case ACM_I2S:		 
 client = 0x00000001 ;		 
 break ;		 
 case ACM_AUX_FM:		 
 client = 0x00000004 ;		 
 break ;		 
 default:		 
 return ;		 
 }	 
	 
 switch ( ( ACM_AudioDevice ) ( * ( ( unsigned short* ) data ) ) ) {		 
 case ACM_MAIN_SPEAKER:		 
 device = 0x00000001 ; // device		 
 value |= ( 0x00000002 << 16 ) ; // direction		 
 break ;		 
 case ACM_AUX_SPEAKER:		 
 device = 0x00000002 ;		 
 value |= ( 0x00000002 << 16 ) ;		 
 break ;		 
 case ACM_HEADSET_SPEAKER:		 
 device = 0x00000004 ;		 
 value |= ( 0x00000002 << 16 ) ;		 
 break ;		 
 case ACM_BLUETOOTH_SPEAKER:		 
 device = 0x00000003 ;		 
 value |= ( 0x00000002 << 16 ) ;		 
 break ;		 
 case ACM_MIC:		 
 device = 0x00000002 ;		 
 value |= ( 0x00000001 << 16 ) ;		 
 break ;		 
 case ACM_HEADSET_MIC:		 
 device = 0x00000004 ;		 
 value |= ( 0x00000001 << 16 ) ;		 
 break ;		 
 case ACM_BLUETOOTH_MIC:		 
 device = 0x00000003 ;		 
 value |= ( 0x00000001 << 16 ) ;		 
 break ;		 
 case ACM_MIC_EC:		 
 device = 0x00000002 ;		 
 value |= ( 0x00000001 << 16 ) ;		 
 value |= ( 0x00000080 << 16 ) ;		 
 break ;		 
 case ACM_HEADSET_MIC_EC:		 
 device = 0x00000004 ;		 
 value |= ( 0x00000001 << 16 ) ;		 
 value |= ( 0x00000080 << 16 ) ;		 
 break ;		 
 case ACM_AUX_MIC:		 
 if ( client != 0x00000004 )		 
 return ;		 
 device = ACM_AUX_SPEAKER ;		 
 value |= ( 0x00000001 << 16 ) ;		 
 break ;		 
 case ACM_MAIN_SPEAKER__LOOP:		 
 device = 0x00000001 ; // device		 
 value |= ( 0x00000002 << 16 ) ; // direction		 
 value |= ( 0x00000100 << 16 ) ; // loopback test		 
 break ;		 
 case ACM_AUX_SPEAKER__LOOP:		 
 device = 0x00000002 ;		 
 value |= ( 0x00000002 << 16 ) ;		 
 value |= ( 0x00000100 << 16 ) ; // loopback test		 
 break ;		 
 case ACM_HEADSET_SPEAKER__LOOP:		 
 device = 0x00000004 ;		 
 value |= ( 0x00000002 << 16 ) ;		 
 value |= ( 0x00000100 << 16 ) ; // loopback test		 
 break ;		 
 case ACM_MIC__LOOP:		 
 device = 0x00000002 ;		 
 value |= ( 0x00000001 << 16 ) ;		 
 value |= ( 0x00000100 << 16 ) ; // loopback test		 
 case ACM_AUX_MIC__LOOP:		 
 device = ACM_AUX_SPEAKER ;		 
 value |= ( 0x00000001 << 16 ) ;		 
 value |= ( 0x00000100 << 16 ) ; // loopback test		 
 break ;		 
 case ACM_HEADSET_MIC__LOOP:		 
 device = 0x00000004 ;		 
 value |= ( 0x00000001 << 16 ) ;		 
 value |= ( 0x00000100 << 16 ) ; // loopback test		 
 break ;		 
 default:		 
 return ;		 
 }	 
	 
 audio_route_control ( client , 0x00000001 , 0x00000002 , device , value ) ;	 
 }

//ICAT EXPORTED FUNCTION - Audio , ACM , ACMAudioDeviceMute 
 void ACMAudioDeviceMute_Env ( void *data ) 
 {	 
 int client , device , value = 0 ;	 
	 
DIAG_FILTER ( AUDIO , ACM , ACMAudioDeviceMute , DIAG_ALL)  
 diagPrintf ( " ACMAudioDeviceMute\n " );

	 
	 
 value |= ( ACM_AudioVolume ) ( * ( ( unsigned short* ) data + 2 ) ) ; // mute	 
	 
 switch ( ( ACM_AudioFormat ) ( * ( ( unsigned short* ) data + 1 ) ) ) { // client		 
 case ACM_MSA_PCM:		 
 client = 0x00000003 ;		 
 break ;		 
 case ACM_I2S:		 
 client = 0x00000001 ;		 
 break ;		 
 case ACM_AUX_FM:		 
 client = 0x00000004 ;		 
 break ;		 
 default:		 
 return ;		 
 }	 
	 
 switch ( ( ACM_AudioDevice ) ( * ( ( unsigned short* ) data ) ) ) {		 
 case ACM_MAIN_SPEAKER:		 
 device = 0x00000001 ; // device		 
 value |= ( 0x00000002 << 16 ) ; // direction		 
 break ;		 
 case ACM_AUX_SPEAKER:		 
 device = 0x00000002 ;		 
 value |= ( 0x00000002 << 16 ) ;		 
 break ;		 
 case ACM_HEADSET_SPEAKER:		 
 device = 0x00000004 ;		 
 value |= ( 0x00000002 << 16 ) ;		 
 break ;		 
 case ACM_BLUETOOTH_SPEAKER:		 
 device = 0x00000003 ;		 
 value |= ( 0x00000002 << 16 ) ;		 
 break ;		 
 case ACM_MIC:		 
 device = 0x00000002 ;		 
 value |= ( 0x00000001 << 16 ) ;		 
 break ;		 
 case ACM_HEADSET_MIC:		 
 device = 0x00000004 ;		 
 value |= ( 0x00000001 << 16 ) ;		 
 break ;		 
 case ACM_BLUETOOTH_MIC:		 
 device = 0x00000003 ;		 
 value |= ( 0x00000001 << 16 ) ;		 
 break ;		 
 case ACM_MIC_EC:		 
 device = 0x00000002 ;		 
 value |= ( 0x00000001 << 16 ) ;		 
 value |= ( 0x00000080 << 16 ) ;		 
 break ;		 
 case ACM_HEADSET_MIC_EC:		 
 device = 0x00000004 ;		 
 value |= ( 0x00000001 << 16 ) ;		 
 value |= ( 0x00000080 << 16 ) ;		 
 break ;		 
 case ACM_AUX_MIC:		 
 if ( client != 0x00000004 )		 
 return ;		 
 device = ACM_AUX_SPEAKER ;		 
 value |= ( 0x00000001 << 16 ) ;		 
 break ;		 
 case ACM_MAIN_SPEAKER__LOOP:		 
 device = 0x00000001 ; // device		 
 value |= ( 0x00000002 << 16 ) ; // direction		 
 value |= ( 0x00000100 << 16 ) ; // loopback test		 
 break ;		 
 case ACM_AUX_SPEAKER__LOOP:		 
 device = 0x00000002 ;		 
 value |= ( 0x00000002 << 16 ) ;		 
 value |= ( 0x00000100 << 16 ) ; // loopback test		 
 break ;		 
 case ACM_HEADSET_SPEAKER__LOOP:		 
 device = 0x00000004 ;		 
 value |= ( 0x00000002 << 16 ) ;		 
 value |= ( 0x00000100 << 16 ) ; // loopback test		 
 break ;		 
 case ACM_MIC__LOOP:		 
 device = 0x00000002 ;		 
 value |= ( 0x00000001 << 16 ) ;		 
 value |= ( 0x00000100 << 16 ) ; // loopback test		 
 case ACM_AUX_MIC__LOOP:		 
 device = ACM_AUX_SPEAKER ;		 
 value |= ( 0x00000001 << 16 ) ;		 
 value |= ( 0x00000100 << 16 ) ; // loopback test		 
 break ;		 
 case ACM_HEADSET_MIC__LOOP:		 
 device = 0x00000004 ;		 
 value |= ( 0x00000001 << 16 ) ;		 
 value |= ( 0x00000100 << 16 ) ; // loopback test		 
 break ;		 
 default:		 
 return ;		 
 }	 
	 
 audio_route_control ( client , 0x00000001 , 0x00000003 , device , value ) ;	 
 }

//ICAT EXPORTED FUNCTION - Audio , ACM , ACMAudioDeviceVolumeSet 
 void ACMAudioDeviceVolumeSet_Env ( void *data ) 
 {	 
 int client , device , value = 0 ;	 
	 
DIAG_FILTER ( AUDIO , ACM , AACMAudioDeviceVolumeSet , DIAG_ALL)  
 diagPrintf ( " ACMAudioDeviceVolumeSet\n " );

	 
	 
 value |= ( ACM_AudioVolume ) ( * ( ( unsigned short* ) data + 2 ) ) ; // volume	 
	 
 switch ( ( ACM_AudioFormat ) ( * ( ( unsigned short* ) data + 1 ) ) ) { // client		 
 case ACM_MSA_PCM:		 
 client = 0x00000003 ;		 
 break ;		 
 case ACM_I2S:		 
 client = 0x00000001 ;		 
 break ;		 
 case ACM_AUX_FM:		 
 client = 0x00000004 ;		 
 break ;		 
 default:		 
 return ;		 
 }	 
	 
 switch ( ( ACM_AudioDevice ) ( * ( ( unsigned short* ) data ) ) ) {		 
 case ACM_MAIN_SPEAKER:		 
 device = 0x00000001 ; // device		 
 value |= ( 0x00000002 << 16 ) ; // direction		 
 break ;		 
 case ACM_AUX_SPEAKER:		 
 device = 0x00000002 ;		 
 value |= ( 0x00000002 << 16 ) ;		 
 break ;		 
 case ACM_HEADSET_SPEAKER:		 
 device = 0x00000004 ;		 
 value |= ( 0x00000002 << 16 ) ;		 
 break ;		 
 case ACM_BLUETOOTH_SPEAKER:		 
 device = 0x00000003 ;		 
 value |= ( 0x00000002 << 16 ) ;		 
 break ;		 
 case ACM_MIC:		 
 device = 0x00000002 ;		 
 value |= ( 0x00000001 << 16 ) ;		 
 break ;		 
 case ACM_HEADSET_MIC:		 
 device = 0x00000004 ;		 
 value |= ( 0x00000001 << 16 ) ;		 
 break ;		 
 case ACM_BLUETOOTH_MIC:		 
 device = 0x00000003 ;		 
 value |= ( 0x00000001 << 16 ) ;		 
 break ;		 
 case ACM_MIC_EC:		 
 device = 0x00000002 ;		 
 value |= ( 0x00000001 << 16 ) ;		 
 value |= ( 0x00000080 << 16 ) ;		 
 break ;		 
 case ACM_HEADSET_MIC_EC:		 
 device = 0x00000004 ;		 
 value |= ( 0x00000001 << 16 ) ;		 
 value |= ( 0x00000080 << 16 ) ;		 
 break ;		 
 case ACM_AUX_MIC:		 
 if ( client != 0x00000004 )		 
 return ;		 
 device = ACM_AUX_SPEAKER ;		 
 value |= ( 0x00000001 << 16 ) ;		 
 break ;		 
 case ACM_MAIN_SPEAKER__LOOP:		 
 device = 0x00000001 ; // device		 
 value |= ( 0x00000002 << 16 ) ; // direction		 
 value |= ( 0x00000100 << 16 ) ; // loopback test		 
 break ;		 
 case ACM_AUX_SPEAKER__LOOP:		 
 device = 0x00000002 ;		 
 value |= ( 0x00000002 << 16 ) ;		 
 value |= ( 0x00000100 << 16 ) ; // loopback test		 
 break ;		 
 case ACM_HEADSET_SPEAKER__LOOP:		 
 device = 0x00000004 ;		 
 value |= ( 0x00000002 << 16 ) ;		 
 value |= ( 0x00000100 << 16 ) ; // loopback test		 
 break ;		 
 case ACM_MIC__LOOP:		 
 device = 0x00000002 ;		 
 value |= ( 0x00000001 << 16 ) ;		 
 value |= ( 0x00000100 << 16 ) ; // loopback test		 
 case ACM_AUX_MIC__LOOP:		 
 device = ACM_AUX_SPEAKER ;		 
 value |= ( 0x00000001 << 16 ) ;		 
 value |= ( 0x00000100 << 16 ) ; // loopback test		 
 break ;		 
 case ACM_HEADSET_MIC__LOOP:		 
 device = 0x00000004 ;		 
 value |= ( 0x00000001 << 16 ) ;		 
 value |= ( 0x00000100 << 16 ) ; // loopback test		 
 break ;		 
 default:		 
 return ;		 
 }	 
	 
 audio_route_control ( client , 0x00000001 , 0x00000004 , device , value ) ;	 
 }

//ICAT EXPORTED FUNCTION - Audio , ACM , ACMAudioCalibrationStateSet 
 void ACMAudioCalibrationStateSet_Env ( void *data ) 
 {	 
 int client , value = 0 ;	 
	 
 value |= ( ACM_AudioCalibrationStatus ) ( * ( ( unsigned short* ) data ) ) ; /*calibtation state*/	 
	 
 /*This diag command isn ' t relate with client type , and we select PHONE_CLIENT to switch the calibration state*/	 
 client = 0x00000003 ;	 
	 
DIAG_FILTER ( AUDIO , ACM , ACMAudioSetCalibrationStateSet , DIAG_ALL)  
 diagPrintf ( " calibration state: %s\n " , value? " ACM_CALIBRATION_ON " : " ACM_CALIBRATION_OFF " );

	 
	 
 value |= ( 0x00000400 << 16 ) ;	 
 audio_route_control ( client , 0x00000001 , 0 , 0 , value ) ;	 
 }

