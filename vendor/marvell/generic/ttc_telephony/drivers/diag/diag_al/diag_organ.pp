//PPL Source File Name : ../diag/diag_al/diag_organ.i
//PPL Source File Name : diag_organ.c
typedef int ptrdiff_t ;typedef unsigned int size_t ;typedef unsigned int wchar_t ;typedef __signed char __int8_t ;typedef unsigned char __uint8_t ;typedef short __int16_t ;typedef unsigned short __uint16_t ;typedef int __int32_t ;typedef unsigned int __uint32_t ;typedef long long __int64_t ;typedef unsigned long long __uint64_t ;typedef __int8_t __int_least8_t ;typedef __uint8_t __uint_least8_t ;typedef __int16_t __int_least16_t ;typedef __uint16_t __uint_least16_t ;typedef __int32_t __int_least32_t ;typedef __uint32_t __uint_least32_t ;typedef __int64_t __int_least64_t ;typedef __uint64_t __uint_least64_t ;typedef __int32_t __int_fast8_t ;typedef __uint32_t __uint_fast8_t ;typedef __int32_t __int_fast16_t ;typedef __uint32_t __uint_fast16_t ;typedef __int32_t __int_fast32_t ;typedef __uint32_t __uint_fast32_t ;typedef __int64_t __int_fast64_t ;typedef __uint64_t __uint_fast64_t ;typedef int __intptr_t ;typedef unsigned int __uintptr_t ;typedef __int64_t __intmax_t ;typedef __uint64_t __uintmax_t ;typedef __int32_t __register_t ;typedef unsigned long __vaddr_t ;typedef unsigned long __paddr_t ;typedef unsigned long __vsize_t ;typedef unsigned long __psize_t ;typedef int __clock_t ;typedef int __clockid_t ;typedef long __ptrdiff_t ;typedef int __time_t ;typedef int __timer_t ;typedef __builtin_va_list __va_list ;typedef int __wchar_t ;typedef int __wint_t ;typedef int __rune_t ;typedef void * __wctrans_t ;typedef void * __wctype_t ;typedef unsigned long __cpuid_t ;typedef __int32_t __dev_t ;typedef __uint32_t __fixpt_t ;typedef __uint32_t __gid_t ;typedef __uint32_t __id_t ;typedef __uint32_t __in_addr_t ;typedef __uint16_t __in_port_t ;typedef __uint32_t __ino_t ;typedef long __key_t ;typedef __uint32_t __mode_t ;typedef __uint32_t __nlink_t ;typedef __int32_t __pid_t ;typedef __uint64_t __rlim_t ;typedef __uint16_t __sa_family_t ;typedef __int32_t __segsz_t ;typedef __uint32_t __socklen_t ;typedef __int32_t __swblk_t ;typedef __uint32_t __uid_t ;typedef __uint32_t __useconds_t ;typedef __int32_t __suseconds_t ;typedef union {
 char __mbstate8 [ 128 ] ;
 __int64_t __mbstateL ;
 } __mbstate_t ;typedef __int8_t int8_t ;typedef __uint8_t uint8_t ;typedef __int16_t int16_t ;typedef __uint16_t uint16_t ;typedef __int32_t int32_t ;typedef __uint32_t uint32_t ;typedef __int64_t int64_t ;typedef __uint64_t uint64_t ;typedef int8_t int_least8_t ;typedef int8_t int_fast8_t ;typedef uint8_t uint_least8_t ;typedef uint8_t uint_fast8_t ;typedef int16_t int_least16_t ;typedef int32_t int_fast16_t ;typedef uint16_t uint_least16_t ;typedef uint32_t uint_fast16_t ;typedef int32_t int_least32_t ;typedef int32_t int_fast32_t ;typedef uint32_t uint_least32_t ;typedef uint32_t uint_fast32_t ;typedef int64_t int_least64_t ;typedef int64_t int_fast64_t ;typedef uint64_t uint_least64_t ;typedef uint64_t uint_fast64_t ;typedef int intptr_t ;typedef unsigned int uintptr_t ;typedef uint64_t uintmax_t ;typedef int64_t intmax_t ;typedef long int ssize_t ;typedef void ( *__kernel_sighandler_t ) ( int ) ;typedef int __kernel_key_t ;typedef int __kernel_mqd_t ;typedef unsigned long __kernel_ino_t ;typedef unsigned short __kernel_mode_t ;typedef unsigned short __kernel_nlink_t ;typedef long __kernel_off_t ;typedef int __kernel_pid_t ;typedef unsigned short __kernel_ipc_pid_t ;typedef unsigned short __kernel_uid_t ;typedef unsigned short __kernel_gid_t ;typedef unsigned int __kernel_size_t ;typedef int __kernel_ssize_t ;typedef int __kernel_ptrdiff_t ;typedef long __kernel_time_t ;typedef long __kernel_suseconds_t ;typedef long __kernel_clock_t ;typedef int __kernel_timer_t ;typedef int __kernel_clockid_t ;typedef int __kernel_daddr_t ;typedef char * __kernel_caddr_t ;typedef unsigned short __kernel_uid16_t ;typedef unsigned short __kernel_gid16_t ;typedef unsigned int __kernel_uid32_t ;typedef unsigned int __kernel_gid32_t ;typedef unsigned short __kernel_old_uid_t ;typedef unsigned short __kernel_old_gid_t ;typedef unsigned short __kernel_old_dev_t ;typedef long long __kernel_loff_t ;typedef unsigned short umode_t ;typedef __signed__ char __s8 ;typedef unsigned char __u8 ;typedef __signed__ short __s16 ;typedef unsigned short __u16 ;typedef __signed__ int __s32 ;typedef unsigned int __u32 ;typedef __signed__ long long __s64 ;typedef unsigned long long __u64 ;typedef __u16 __le16 ;typedef __u16 __be16 ;typedef __u32 __le32 ;typedef __u32 __be32 ;typedef __u64 __le64 ;typedef __u64 __be64 ;typedef unsigned long __kernel_blkcnt_t ;typedef unsigned long __kernel_blksize_t ;typedef unsigned long __kernel_fsblkcnt_t ;typedef unsigned long __kernel_fsfilcnt_t ;typedef unsigned int __kernel_id_t ;typedef __u32 __kernel_dev_t ;typedef __kernel_blkcnt_t blkcnt_t ;typedef __kernel_blksize_t blksize_t ;typedef __kernel_clock_t clock_t ;typedef __kernel_clockid_t clockid_t ;typedef __kernel_dev_t dev_t ;typedef __kernel_fsblkcnt_t fsblkcnt_t ;typedef __kernel_fsfilcnt_t fsfilcnt_t ;typedef __kernel_gid32_t gid_t ;typedef __kernel_id_t id_t ;typedef __kernel_ino_t ino_t ;typedef __kernel_key_t key_t ;typedef __kernel_mode_t mode_t ;typedef __kernel_nlink_t nlink_t ;typedef __kernel_off_t off_t ;typedef __kernel_loff_t loff_t ;typedef loff_t off64_t ;typedef __kernel_pid_t pid_t ;typedef __kernel_suseconds_t suseconds_t ;typedef __kernel_time_t time_t ;typedef __kernel_uid32_t uid_t ;typedef signed long useconds_t ;typedef __kernel_daddr_t daddr_t ;typedef __kernel_timer_t timer_t ;typedef __kernel_mqd_t mqd_t ;typedef __kernel_caddr_t caddr_t ;typedef unsigned int uint_t ;typedef unsigned int uint ;typedef unsigned char u_char ;typedef unsigned short u_short ;typedef unsigned int u_int ;typedef unsigned long u_long ;typedef uint32_t u_int32_t ;typedef uint16_t u_int16_t ;typedef uint8_t u_int8_t ;typedef uint64_t u_int64_t ;typedef unsigned short sa_family_t ;typedef int socklen_t ;typedef unsigned long sigset_t ;typedef void __signalfn_t ( int ) ;typedef __signalfn_t *__sighandler_t ;typedef void __restorefn_t ( void ) ;typedef __restorefn_t *__sigrestore_t ;typedef union sigval {
 int sival_int ;
 void *sival_ptr ;
 } sigval_t ;typedef int sig_atomic_t ;typedef void ( *sig_t ) ( int ) ;typedef sig_t sighandler_t ;typedef __kernel_fd_set fd_set ;//ICAT EXPORTED STRUCT 
 typedef struct HiFi_Organ_Params {	 
 unsigned short paramType ; // 0 -playback 1 -record	 
 unsigned short deviceType ; // output or input device	 
 unsigned short streamType ; // stream type for playback , application style for record	 
 unsigned short formatType ; // file format for playback , encoding format for record	 
 unsigned short volumeIndex ; // only valid for playback for record fixed to 0	 
 unsigned short dBIndex ; // index to indicate the dB	 
 } hifiParam;

//ICAT EXPORTED ENUM 
 typedef enum {	 
 MUSIC_MP3_SPK =0 ,	 
 MUSIC_AAC_SPK =1 ,	 
 MUSIC_OGG_SPK =2 ,	 
 MUSIC_MIDI_SPK =3 ,	 
 MUSIC_AMR_SPK =4 ,	 
 MUSIC_WAV_SPK =5 ,	 
 MUSIC_TONE_SPK =6 ,	 
 MUSIC_AVI_SPK =7 ,	 
 MUSIC_MP4_SPK =8 ,	 
 MUSIC_RMVB_SPK =9 ,	 
 MUSIC_MP3_HS =10 ,	 
 MUSIC_AAC_HS =11 ,	 
 MUSIC_OGG_HS =12 ,	 
 MUSIC_MIDI_HS =13 ,	 
 MUSIC_AMR_HS =14 ,	 
 MUSIC_WAV_HS =15 ,	 
 MUSIC_TONE_HS = 16 ,	 
 MUSIC_AVI_HS =17 ,	 
 MUSIC_MP4_HS =18 ,	 
 MUSIC_RMVB_HS =19 ,	 
 MUSIC_MP3_BT =20 ,	 
 MUSIC_AAC_BT =21 ,	 
 MUSIC_OGG_BT =22 ,	 
 MUSIC_MIDI_BT =23 ,	 
 MUSIC_AMR_BT =24 ,	 
 MUSIC_WAV_BT =25 ,	 
 MUSIC_TONE_BT =26 ,	 
 MUSIC_AVI_BT =27 ,	 
 MUSIC_MP4_BT =28 ,	 
 MUSIC_RMVB_BT =29 ,	 
 SYSTEM_MP3_SPK =30 ,	 
 SYSTEM_AAC_SPK =31 ,	 
 SYSTEM_OGG_SPK = 32 ,	 
 SYSTEM_MIDI_SPK =33 ,	 
 SYSTEM_AMR_SPK = 34 ,	 
 SYSTEM_WAV_SPK = 35 ,	 
 SYSTEM_TONE_SPK =36 ,	 
 SYSTEM_MP3_HS = 37 ,	 
 SYSTEM_AAC_HS = 38 ,	 
 SYSTEM_OGG_HS = 39 ,	 
 SYSTEM_MIDI_HS = 40 ,	 
 SYSTEM_AMR_HS = 41 ,	 
 SYSTEM_WAV_HS = 42 ,	 
 SYSTEM_TONE_HS = 43 ,	 
 RING_MP3_SPK = 44 ,	 
 RING_AAC_SPK = 45 ,	 
 RING_OGG_SPK = 46 ,	 
 RING_MIDI_SPK = 47 ,	 
 RING_AMR_SPK = 48 ,	 
 RING_WAV_SPK = 49 ,	 
 RING_TONE_SPK =50 ,	 
 RING_MP3_HS = 51 ,	 
 RING_AAC_HS = 52 ,	 
 RING_OGG_HS = 53 ,	 
 RING_MIDI_HS = 54 ,	 
 RING_AMR_HS = 55 ,	 
 RING_WAV_HS = 56 ,	 
 RING_TONE_HS = 57 ,	 
 RING_MP3_SPK_FSPK = 58 ,	 
 RING_AAC_SPK_FSPK = 59 ,	 
 RING_OGG_SPK_FSPK = 60 ,	 
 RING_MIDI_SPK_FSPK = 61 ,	 
 RING_AMR_SPK_FSPK = 62 ,	 
 RING_WAV_SPK_FSPK = 63 ,	 
 RING_TONE_SPK_FSPK =64 ,	 
 RING_MP3_HS_FSPK = 65 ,	 
 RING_AAC_HS_FSPK = 66 ,	 
 RING_OGG_HS_FSPK = 67 ,	 
 RING_MIDI_HS_FSPK = 68 ,	 
 RING_AMR_HS_FSPK =69 ,	 
 RING_WAV_HS_FSPK =70 ,	 
 RING_TONE_HS_FSPK =71 ,	 
 ALARM_MP3_SPK = 72 ,	 
 ALARM_AAC_SPK = 73 ,	 
 ALARM_OGG_SPK = 74 ,	 
 ALARM_MIDI_SPK =75 ,	 
 ALARM_AMR_SPK = 76 ,	 
 ALARM_WAV_SPK = 77 ,	 
 ALARM_TONE_SPK =78 ,	 
 ALARM_MP3_HS = 79 ,	 
 ALARM_AAC_HS = 80 ,	 
 ALARM_OGG_HS = 81 ,	 
 ALARM_MIDI_HS = 82 ,	 
 ALARM_AMR_HS = 83 ,	 
 ALARM_WAV_HS = 84 ,	 
 ALARM_TONE_HS= 85 ,	 
 ALARM_MP3_SPK_FSPK =86 ,	 
 ALARM_AAC_SPK_FSPK =87 ,	 
 ALARM_OGG_SPK_FSPK =88 ,	 
 ALARM_MIDI_SPK_FSPK =89 ,	 
 ALARM_AMR_SPK_FSPK = 90 ,	 
 ALARM_WAV_SPK_FSPK =91 ,	 
 ALARM_TONE_SPK_FSPK =92 ,	 
 ALARM_MP3_HS_FSPK = 93 ,	 
 ALARM_AAC_HS_FSPK = 94 ,	 
 ALARM_OGG_HS_FSPK = 95 ,	 
 ALARM_MIDI_HS_FSPK = 96 ,	 
 ALARM_AMR_HS_FSPK = 97 ,	 
 ALARM_WAV_HS_FSPK = 98 ,	 
 ALARM_TONE_HS_FSPK= 99 ,	 
 NOTIFICATION_MP3_SPK =100 ,	 
 NOTIFICATION_AAC_SPK =101 ,	 
 NOTIFICATION_OGG_SPK =102 ,	 
 NOTIFICATION_MIDI_SPK =103 ,	 
 NOTIFICATION_AMR_SPK =104 ,	 
 NOTIFICATION_WAV_SPK =105 ,	 
 NOTIFICATION_TONE_SPK =106 ,	 
 NOTIFICATION_MP3_HS = 107 ,	 
 NOTIFICATION_AAC_HS = 108 ,	 
 NOTIFICATION_OGG_HS = 109 ,	 
 NOTIFICATION_MIDI_HS = 110 ,	 
 NOTIFICATION_AMR_HS = 111 ,	 
 NOTIFICATION_WAV_HS = 112 ,	 
 NOTIFICATION_TONE_HS= 113 ,	 
 NOTIFICATION_MP3_SPK_FSPK =114 ,	 
 NOTIFICATION_AAC_SPK_FSPK =115 ,	 
 NOTIFICATION_OGG_SPK_FSPK =116 ,	 
 NOTIFICATION_MIDI_SPK_FSPK =117 ,	 
 NOTIFICATION_AMR_SPK_FSPK =118 ,	 
 NOTIFICATION_WAV_SPK_FSPK =119 ,	 
 NOTIFICATION_TONE_SPK_FSPK =120 ,	 
 NOTIFICATION_MP3_HS_FSPK =121 ,	 
 NOTIFICATION_AAC_HS_FSPK =122 ,	 
 NOTIFICATION_OGG_HS_FSPK =123 ,	 
 NOTIFICATION_MIDI_HS_FSPK =124 ,	 
 NOTIFICATION_AMR_HS_FSPK =125 ,	 
 NOTIFICATION_WAV_HS_FSPK =126 ,	 
 NOTIFICATION_TONE_HS_FSPK =127 ,	 
 ENFORCED_MP3_SPK =128 ,	 
 ENFORCED_AAC_SPK =129 ,	 
 ENFORCED_OGG_SPK =130 ,	 
 ENFORCED_MIDI_SPK =131 ,	 
 ENFORCED_AMR_SPK =132 ,	 
 ENFORCED_WAV_SPK =133 ,	 
 ENFORCED_TONE_SPK =134 ,	 
 ENFORCED_MP3_HS =135 ,	 
 ENFORCED_AAC_HS =136 ,	 
 ENFORCED_OGG_HS =137 ,	 
 ENFORCED_MIDI_HS =138 ,	 
 ENFORCED_AMR_HS =139 ,	 
 ENFORCED_WAV_HS =140 ,	 
 ENFORCED_TONE_HS= 141 ,	 
 ENFORCED_MP3_SPK_FSPK =142 ,	 
 ENFORCED_AAC_SPK_FSPK =143 ,	 
 ENFORCED_OGG_SPK_FSPK =144 ,	 
 ENFORCED_MIDI_SPK_FSPK =145 ,	 
 ENFORCED_AMR_SPK_FSPK =146 ,	 
 ENFORCED_WAV_SPK_FSPK =147 ,	 
 ENFORCED_TONE_SPK_FSPK= 148 ,	 
 ENFORCED_MP3_HS_FSPK=149 ,	 
 ENFORCED_AAC_HS_FSPK=150 ,	 
 ENFORCED_OGG_HS_FSPK=151 ,	 
 ENFORCED_MIDI_HS_FSPK=152 ,	 
 ENFORCED_AMR_HS_FSPK=153 ,	 
 ENFORCED_WAV_HS_FSPK=154 ,	 
 ENFORCED_TONE_HS_FSPK= 155 ,	 
 VOICE_CALL_SPK =156 ,	 
 VOICE_CALL_HS =157 ,	 
 BLUETOOTH_SCO_SPK =158 ,	 
 BLUETOOTH_SCO_HS =159 ,	 
 DTMF_SPK =160 ,	 
 DTMF_HS =161 ,	 
 TTS_SPK =162 ,	 
 TTS_HS =163 ,	 
 FM_RADIO_SPK =164 ,	 
 FM_RADIO_HS =165 ,	 
 RECORDER_PCM_MIC1 =166 ,	 
 RECORDER_AMR_NB_MIC1 =167 ,	 
 RECORDER_AAC_MIC1 =168 ,	 
 RECORDER_PCM_MIC2 =169 ,	 
 RECORDER_AMR_NB_MIC2 =170 ,	 
 RECORDER_AAC_MIC2 =171 ,	 
 RECORDER_PCM_HSMIC =172 ,	 
 RECORDER_AMR_NB_HSMIC =173 ,	 
 RECORDER_AAC_HSMIC =174 ,	 
 CAMCORDER_PCM_MIC1 =175 ,	 
 CAMCORDER_AMR_NB_MIC1 =176 ,	 
 CAMCORDER_AAC_MIC1 =177 ,	 
 CAMCORDER_PCM_MIC2 =178 ,	 
 CAMCORDER_AMR_NB_MIC2 =179 ,	 
 CAMCORDER_AAC_MIC2 =180 ,	 
 CAMCORDER_PCM_HSMIC =181 ,	 
 CAMCORDER_AMR_NB_HSMIC =182 ,	 
 CAMCORDER_AAC_HSMIC =183 ,	 
 ASR_PCM_MIC1 =184 ,	 
 ASR_AMR_NB_MIC1 =185 ,	 
 ASR_AAC_MIC1 =186 ,	 
 ASR_PCM_MIC2 =187 ,	 
 ASR_AMR_NB_MIC2 =188 ,	 
 ASR_AAC_MIC2 =189 ,	 
 ASR_PCM_HSMIC =190 ,	 
 ASR_AMR_NB_HSMIC =191 ,	 
 ASR_AAC_HSMIC =192 ,	 
 VOIP_SPK =193 ,	 
 VOIP_CALL_HS =194 ,	 
 VOIP_RECORDER_PCM_MIC1 =195 ,	 
 VOIP_RECORDER_AMR_NB_MIC1 =196 ,	 
 VOIP_RECORDER_AAC_MIC1 =197 ,	 
 VOIP_RECORDER_PCM_MIC2 =198 ,	 
 VOIP_RECORDER_AMR_NB_MIC2 =199 ,	 
 VOIP_RECORDER_AAC_MIC2 =200 ,	 
 VOIP_RECORDER_PCM_HSMIC =201 ,	 
 VOIP_RECORDER_AMR_NB_HSMIC =202 ,	 
 VOIP_RECORDER_AAC_HSMIC =203 ,	 
 VOICE_CALL_EPC =204 ,	 
 VOIP_EPC =205 ,	 
 ORGANPATHNUMB	 
 } ORGAN_PATH;

typedef __builtin_va_list __gnuc_va_list ;typedef off_t fpos_t ;typedef __gnuc_va_list va_list ;//ICAT EXPORTED FUNCTION - Audio , HW , AUDIO_HW_GetPathsStatus 
 void GetPathsStatus ( void *data ) 
 {	 
 int fd ;	 
 struct sockaddr_un unix_addr ;	 
 int command = GET_VOLUME ;	 
 char * streamStatus ;	 
 char * pathStatus ;	 
 int len , pathID ;	 
 int i ;	 
 int pathNum = sizeof ( hifi_mapping ) / sizeof ( hifiMapping ) ;	 
 int transResult ;	 
 ( void ) data ; ;	 
	 
 eeh_log ( 5 , " ***** Recv AUDIO_HW_GetPathsStatus... " ) ;	 
	 
 fd = socket ( 1 , 1 , 0 ) ;	 
 if ( fd < 0 ) {		 
 eeh_log ( 5 , " ***** Fail to create socket! " ) ;		 
 return ;		 
 }	 
	 
 memset ( &unix_addr , 0 , sizeof ( unix_addr ) ) ;	 
 unix_addr.sun_family = 1 ;	 
 strcpy ( unix_addr.sun_path , " / data / misc / organ / sockets / organ_daemon " ) ;	 
 len = sizeof ( unix_addr.sun_family ) + strlen ( unix_addr.sun_path ) ;	 
	 
 if ( connect ( fd , ( struct sockaddr * ) &unix_addr , len ) < 0 ) {		 
 eeh_log ( 5 , " ***** Fail to connect to server! " ) ;		 
 close ( fd ) ;		 
 return ;		 
 }	 
	 
 write ( fd , &command , sizeof ( command ) ) ;	 
	 
 len = pathNum * 3 ;	 
 streamStatus = ( char * ) malloc ( len ) ;	 
 if ( ( ( void * ) 0 ) == streamStatus ) {		 
 eeh_log ( 5 , " ***** Unable to allocate buffer for streamStatus! " ) ;		 
 close ( fd ) ;		 
 return ;		 
 }	 
	 
 if ( read ( fd , streamStatus , len ) < 0 ) {		 
 eeh_log ( 5 , " ***** Fail to read server response! " ) ;		 
 close ( fd ) ;		 
 free ( streamStatus ) ;		 
 return ;		 
 }	 
	 
 len = ORGANPATHNUMB * 4 + 3 ;	 
 pathStatus = ( char * ) malloc ( len ) ;	 
 if ( ( ( void * ) 0 ) == pathStatus ) {		 
 eeh_log ( 5 , " ***** Unable to allocate buffer for pathStatus! " ) ;		 
 close ( fd ) ;		 
 free ( streamStatus ) ;		 
 return ;		 
 }	 
	 
 // Initialize path status	 
 memset ( pathStatus , 0 , len ) ;	 
 for ( i = 0 ; i < ORGANPATHNUMB ; i++ ) {		 
 pathStatus [ i * 4 + 3 ] = i ; // path ID		 
 }	 
	 
 // Component number	 
 pathStatus [ 0 ] = 1 ;	 
 // Organ type code	 
 pathStatus [ 1 ] = 0x0A ;	 
 // Path number	 
 pathStatus [ 2 ] = ( char ) pathNum ;	 
	 
 for ( i = 0 ; i < pathNum ; i++ ) {		 
 transResult = Organ2Path ( hifi_mapping [ i ] .paramType ,		 
 hifi_mapping [ i ] .deviceType ,		 
 hifi_mapping [ i ] .streamType ,		 
 hifi_mapping [ i ] .formatType ,		 
 &pathID ) ;		 
 if ( transResult == 0 ) {			 
 pathStatus [ ( pathID * 4 ) + 3 ] = ( char ) pathID ; // path ID			 
 pathStatus [ ( pathID * 4 + 1 ) + 3 ] = ( char ) ( streamStatus [ i * 3 ] ) ; // state			 
 if ( pathID < RECORDER_PCM_MIC1			 
 || pathID == VOIP_SPK || pathID == VOIP_CALL_HS			 
 || pathID == VOICE_CALL_EPC || pathID == VOIP_EPC ) {				 
 // ignore record path ( volume is always 0 )				 
 pathStatus [ ( pathID * 4 + 2 ) + 3 ] = ( char ) ( streamStatus [ i * 3 + 1 ] ) ; // volume				 
 }			 
 pathStatus [ ( pathID * 4 + 3 ) + 3 ] = ( char ) ( streamStatus [ i * 3 + 2 ] >0 ? 1 : 0 ) ; // mute			 
 }		 
 }	 
	 
 close ( fd ) ;	 
 free ( streamStatus ) ;	 
	 
 eeh_log ( 5 , " ***** Sending AUDIO_HW_PathsStatus... " ) ;	 
	 
DIAG_FILTER ( Audio , HW , AUDIO_HW_PathsStatus , DIAG_INFORMATION)  
 diagStructPrintf ( " " , ( void * ) pathStatus , len );

	 
	 
 free ( pathStatus ) ;	 
 }

//ICAT EXPORTED FUNCTION - Audio , Organ , GetSlopeOffset 
 void GetSlopeOffset ( void *data ) 
 {	 
 FILE * fp = ( ( void * ) 0 ) ;	 
 NVM_Header_ts header ;	 
 hifiParam param ;	 
 hifiParam paramTemp ;	 
 char * slopeOffset ;	 
 char * pTemp ;	 
 int len , pathID ;	 
 int i , j ;	 
 int flag = 1 ;	 
 int transResult ;	 
 int * defaultDBTable ;	 
 char *nvm_root_dir = getenv ( " NVM_ROOT_DIR " ) ;	 
 char fullpath [ 256 ] = { 0 } ;	 
 ( void ) data ; ;	 
	 
 eeh_log ( 5 , " ***** Recv Organ_GetSlopeOffset... " ) ;	 
	 
 if ( nvm_root_dir != ( ( void * ) 0 ) )	 
 {		 
 strcpy ( fullpath , nvm_root_dir ) ;		 
 strcat ( fullpath , " / " ) ;		 
 }	 
 strcat ( fullpath , " audio_hifi_volume.nvm " ) ;	 
	 
 fp = fopen ( fullpath , " rb " ) ;	 
 if ( ( ( void * ) 0 ) == fp ) {		 
 eeh_log ( 5 , " ***** Unable to open %s! " , fullpath ) ;		 
 flag = 0 ;		 
 }	 
	 
 if ( flag && fread ( &header , sizeof ( NVM_Header_ts ) , 1 , fp ) != 1 ) {		 
 eeh_log ( 5 , " ***** Incomplete NVM header! " ) ;		 
 fclose ( fp ) ;		 
 return ;		 
 }	 
	 
 slopeOffset = ( char * ) malloc ( 8000 ) ;	 
 if ( ( ( void * ) 0 ) == slopeOffset ) {		 
 eeh_log ( 5 , " ***** Unable to allocate buffer! " ) ;		 
 if ( fp != ( ( void * ) 0 ) ) {			 
 fclose ( fp ) ;			 
 }		 
 return ;		 
 }	 
	 
 pTemp = slopeOffset ;	 
 len = 0 ;	 
	 
 for ( i = 0 ; i < ORGANPATHNUMB ; i++ ) {		 
 transResult = Path2Organ ( i ,		 
 &param.paramType ,		 
 &param.deviceType ,		 
 &param.streamType ,		 
 &param.formatType ) ;		 
		 
 if ( transResult == 0 ) {			 
 *pTemp++ = ( unsigned char ) i ; // path ID			 
 *pTemp++ = ( unsigned char ) ( MAX_STREAM_VOLUME [ param.streamType - 1 ] + 1 ) ; // gain number			 
 len += 2 ;			 
			 
 // Organ stream type = android stream type + 1			 
 switch ( MAX_STREAM_VOLUME [ param.streamType - 1 ] ) {				 
 case 15 :				 
 defaultDBTable = DEFAULT_DB_VALUE_15 ;				 
 break ;				 
 case 7 :				 
 defaultDBTable = DEFAULT_DB_VALUE_7 ;				 
 break ;				 
 case 5 :				 
 defaultDBTable = DEFAULT_DB_VALUE_5 ;				 
 break ;				 
 default:				 
 defaultDBTable = DEFAULT_DB_VALUE_1 ;				 
 break ;				 
 }			 
			 
 // Organ stream type = android stream type + 1			 
 for ( j = 0 ; j <= MAX_STREAM_VOLUME [ param.streamType - 1 ] ; j++ ) {				 
 *pTemp++ = ( char ) j ; // Gain ID				 
 *pTemp++ = ( char ) defaultDBTable [ j ] ; // offset				 
 len += 2 ;				 
 }			 
 }		 
 }	 
	 
 if ( flag ) {		 
 for ( i = 0 ; i < ( int ) header.NumofStructs ; i++ ) {			 
 if ( fread ( &param , header.StructSize , 1 , fp ) != 1 ) {				 
 eeh_log ( 5 , " ***** Incomplete NVM record! " ) ;				 
 fclose ( fp ) ;				 
 free ( slopeOffset ) ;				 
 return ;				 
 }			 
			 
 transResult = Organ2Path ( param.paramType ,			 
 param.deviceType ,			 
 param.streamType ,			 
 param.formatType ,			 
 &pathID ) ;			 
 if ( transResult == 0 ) {				 
 pTemp = slopeOffset ;				 
 for ( j = 0 ; j < ORGANPATHNUMB ; j++ ) {					 
 if ( j != pathID ) {						 
 transResult = Path2Organ ( j ,						 
 &paramTemp.paramType ,						 
 &paramTemp.deviceType ,						 
 &paramTemp.streamType ,						 
 &paramTemp.formatType ) ;						 
 // Organ stream type = android stream type + 1						 
 pTemp += ( ( MAX_STREAM_VOLUME [ paramTemp.streamType - 1 ] + 1 ) * 2 + 2 ) ;						 
 continue ;						 
 }					 
					 
 pTemp += 2 ;					 
 * ( pTemp + param.volumeIndex * 2 + 1 ) = ( char ) param.dBIndex ;					 
 }				 
 }			 
 }		 
 }	 
	 
 eeh_log ( 5 , " ***** Sending AUDIO_ORGAN_SlopeOffset... " ) ;	 
	 
DIAG_FILTER ( Audio , Organ , AUDIO_ORGAN_SlopeOffset , DIAG_INFORMATION)  
 diagStructPrintf ( " " , ( void * ) slopeOffset , len );

	 
	 
 if ( fp != ( ( void * ) 0 ) ) {		 
 fclose ( fp ) ;		 
 }	 
 free ( slopeOffset ) ;	 
 }

//ICAT EXPORTED FUNCTION - Audio , Organ , ReadFdiCalibData 
 void ReadFdiCalibData ( void *data ) 
 {	 
 int fd ;	 
 struct sockaddr_un unix_addr ;	 
 int command = RELOAD_NVM ;	 
 int status ;	 
 int len ;	 
 ( void ) data ; ;	 
	 
 eeh_log ( 5 , " ***** Recv Organ_ReadFdiCalibData... " ) ;	 
	 
 fd = socket ( 1 , 1 , 0 ) ;	 
 if ( fd < 0 ) {		 
 eeh_log ( 5 , " ***** Fail to create socket! " ) ;		 
 return ;		 
 }	 
	 
 memset ( &unix_addr , 0 , sizeof ( unix_addr ) ) ;	 
 unix_addr.sun_family = 1 ;	 
 strcpy ( unix_addr.sun_path , " / data / misc / organ / sockets / organ_daemon " ) ;	 
 len = sizeof ( unix_addr.sun_family ) + strlen ( unix_addr.sun_path ) ;	 
	 
 if ( connect ( fd , ( struct sockaddr * ) &unix_addr , len ) < 0 ) {		 
 eeh_log ( 5 , " ***** Fail to connect to server! " ) ;		 
 close ( fd ) ;		 
 return ;		 
 }	 
	 
 write ( fd , &command , sizeof ( command ) ) ;	 
 read ( fd , &status , sizeof ( status ) ) ;	 
 close ( fd ) ;	 
	 
 eeh_log ( 5 , " ***** Sending AUDIO_ORGAN_ReadFdiDataOK... " ) ;	 
	 
 if ( STATUS_OK == status ) {		 
DIAG_FILTER ( Audio , Organ , AUDIO_ORGAN_ReadFdiDataOK , DIAG_INFORMATION)  
 diagStructPrintf ( " OK " , ( ( void * ) 0 ) , 0 );

		 
 } else {		 
DIAG_FILTER ( Audio , Organ , AUDIO_ORGAN_ReadFdiDataERROR , DIAG_INFORMATION)  
 diagStructPrintf ( " ERROR " , ( ( void * ) 0 ) , 0 );

		 
 }	 
 }

//ICAT EXPORTED FUNCTION - Audio , HW , AUDIO_HW_ORGAN_PathEnable 
 void PathEnable ( void *data ) 
 {	 
 int fd ;	 
 struct sockaddr_un unix_addr ;	 
 int command = SET_VOLUME ;	 
 int status ;	 
 char streamVolume [ 2 ] ;	 
 int len ;	 
 hifiParam param ;	 
 int transResult ;	 
	 
 eeh_log ( 5 , " ***** Recv AUDIO_HW_PathEnable... " ) ;	 
	 
 transResult = Path2Organ ( ( ( char * ) data ) [ 1 ] ,	 
 &param.paramType ,	 
 &param.deviceType ,	 
 &param.streamType ,	 
 &param.formatType ) ;	 
	 
 if ( transResult < 0 || param.streamType > FM_RADIO ) {		 
 eeh_log ( 5 , " ***** not playback type , ignore... " ) ;		 
DIAG_FILTER ( Audio , HW , AUDIO_HW_PathEnableIgnore , DIAG_INFORMATION)  
 diagStructPrintf ( " OK " , ( ( void * ) 0 ) , 0 );

		 
 return ;		 
 }	 
	 
 streamVolume [ 0 ] = ( char ) param.streamType ;	 
 streamVolume [ 1 ] = ( ( char * ) data ) [ 3 ] ;	 
	 
 fd = socket ( 1 , 1 , 0 ) ;	 
 if ( fd < 0 ) {		 
 eeh_log ( 5 , " ***** Fail to create socket! " ) ;		 
 return ;		 
 }	 
	 
 memset ( &unix_addr , 0 , sizeof ( unix_addr ) ) ;	 
 unix_addr.sun_family = 1 ;	 
 strcpy ( unix_addr.sun_path , " / data / misc / organ / sockets / organ_daemon " ) ;	 
 len = sizeof ( unix_addr.sun_family ) + strlen ( unix_addr.sun_path ) ;	 
	 
 if ( connect ( fd , ( struct sockaddr * ) &unix_addr , len ) < 0 ) {		 
 eeh_log ( 5 , " ***** Fail to connect to server! " ) ;		 
 close ( fd ) ;		 
 return ;		 
 }	 
	 
 write ( fd , &command , sizeof ( command ) ) ;	 
 write ( fd , streamVolume , 2 ) ;	 
	 
 read ( fd , &status , sizeof ( status ) ) ;	 
 close ( fd ) ;	 
	 
 eeh_log ( 5 , " ***** Sending AUDIO_HW_PathEnable... " ) ;	 
	 
 if ( STATUS_OK == status ) {		 
DIAG_FILTER ( Audio , HW , AUDIO_HW_PathEnableOK , DIAG_INFORMATION)  
 diagStructPrintf ( " OK " , ( ( void * ) 0 ) , 0 );

		 
 } else {		 
DIAG_FILTER ( Audio , HW , AUDIO_HW_PathEnableSTATUSERROR , DIAG_INFORMATION)  
 diagStructPrintf ( " ERROR " , ( ( void * ) 0 ) , 0 );

		 
 }	 
 }

//ICAT EXPORTED FUNCTION - Audio , HW , AUDIO_HW_ORGAN_PathDisable 
 void PathDisable ( void *data ) 
 {	 
 ( void ) data ; ;	 
 eeh_log ( 5 , " ***** Recv AUDIO_HW_PathDisable... " ) ;	 
	 
DIAG_FILTER ( Audio , HW , AUDIO_HW_PathDisableOK , DIAG_INFORMATION)  
 diagStructPrintf ( " OK " , ( ( void * ) 0 ) , 0 );

	 
 }

//ICAT EXPORTED FUNCTION - Audio , HW , AUDIO_HW_PathMute 
 void PathMute ( void *data ) 
 {	 
 eeh_log ( 5 , " ***** Recv AUDIO_HW_PathMute , %d... " , ( ( char * ) data ) [ 3 ] ) ;	 
	 
DIAG_FILTER ( Audio , HW , AUDIO_HW_PathMuteOK , DIAG_INFORMATION)  
 diagStructPrintf ( " OK " , ( ( void * ) 0 ) , 0 );

	 
 }

