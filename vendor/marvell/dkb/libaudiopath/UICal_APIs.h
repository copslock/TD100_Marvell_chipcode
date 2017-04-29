
//Macro for print
#define UICAL_PRINT printf


/********************
 MSA module status
 ********************/
#define	MSA_MODULE_OFF			0
#define	MSA_MODULE_ON			1


#define   Q13 0x2000
#define INVALID_MSA_PARAMETER 0xdead

//--- List of Voice Command OpCodes:
#define     START_VOICE_PATH	0x0040
#define     END_VOICE_PATH		0x0041
#define     VOICE_HANDOVER		0x0042
#define     VOICE_RX_PACKET		0x0043
#define     VOICE_MODULE_DEBUG  0x0044
#define     SET_CODEC			0x0045
#define     VOICE_TEST_CONTROL  0x0046
#define     PCM_STREAM_CONTROL	0x0047
#define     SELF_INVOCATION		0x0049
#define     VOICE_DRIFT_CONTROL 0x004A
#define     AMR_NSYNC_UPDATE	0x004B
#define     CODEC_IF_CONTROL	0x004C

#define     VOCODER_CONTROL		0x0060
#define     VOICE_CONTROL		0x0061
#define     VOLUME_CONTROL      0x0062
#define     MUTE_CONTROL        0x0063
#define     NS_CONTROL          0x0064
#define     EC_CONTROL          0x0065
#define     EQ_CONTROL          0x0066
#define     AVC_CONTROL         0x0067
#define     DTMF_CONTROL        0x0068
#define     AUX_MODE_CONTROL    0x0069
#define     EC_TEST_CMD         0x006A
#define     NS_TEST_CMD         0x006B
#define     AVC_TEST_CMD        0x006C
#define		DITHER_GEN_CONTROL	0x006D
#define		CTM_CONTROL			0x006E
//
//--- List of Voice Messages OpCodes:
#define     VOICE_TX_PACKET		0x0040
#define     VOICE_DRIFT_REPORT  0x0041
#define     END_CONV_ACK        0x0042
#define     PCM_STREAM_RECORD   0x0043
#define     GET_PCM_FRAME       0x0044
#define     VOICE_DECODER_ACK   0x0045
#define     START_CONV_ACK      0x0046
#define     VOICE_PCM_ACK       0x0047
#define     VOICE_DEBUG_RX_PACKET	0x0048
#define     VOICE_DEBUG_REPORT      0x0049
//#define		EC_VOICE_DEBUG_REPORT	0x004A
//
//--- List of Comm. Messages:
#define		RX_STATUS	0x009a
#define		TX_STATUS	0x009b


//voice path control cmd bitmap (note: bit 0 to 2 resereved)
#define RESERVED_0				(1 << 0 )
#define RESERVED_1				(1 << 1 )
#define RESERVED_2				(1 << 2 )
#define EC_ENABLE				(1 << 3 )
#define RES_ENABLE              (1 << 4 )
#define EC_RESET				(1 << 5 )
#define TX_NS_ENABLE            (1 << 6 )
#define RX_NS_ENABLE            (1 << 7 )
#define TX_NS_RESET             (1 << 8 )
#define RX_NS_RESET             (1 << 9 )
#define TX_EQU_ENABLE           (1 << 10)
#define RX_EQU_ENABLE           (1 << 11)
#define TX_EQU_RESET            (1 << 12)
#define RX_EQU_RESET            (1 << 13)
#define TX_AGC_ENABLE           (1 << 14)
#define RX_AVC_ENABLE           (1 << 15)
#define TX_AGC_RESET            (1 << 16)
#define RX_AVC_RESET            (1 << 17)
#define TX_VOLUME_ENABLE        (1 << 18)
#define RX_VOLUME_ENABLE        (1 << 19)
#define SIDE_TONE_ENABLE        (1 << 20)
#define DITHER_GENERATOR_ENABLE (1 << 21)
#define CTM_ENABLE              (1 << 22)
#define CTM_RESET               (1 << 23)


typedef enum {
	EC_ID = 0,                               // 0
	TX_NS_ID,                               // 1
	TX_EQ_ID,                               // 2
	TX_AGC_ID,                             // 3
	TX_VOLUME_AND_MUTE_ID,   // 4
	RX_NS_ID,                               // 5
	RX_EQ_ID,                               // 6
	RX_AVC_ID,                             // 7
	RX_VOLUME_AND_MUTE_ID,   // 8
	RX_TX_ENERGY,                              // 9
	VOICE_CONTROL_AND_VOCODER_CONTROL_ID,       // 10
	MODEM_INFO_ID,                            // 11
	TX_CTM_ID,                             // 12
	RX_CTM_ID,                             // 13
	EC_FIR_ID,                             // 14
	VOLUME_EC_THRESHOLD_ID,                             // 15

//Jackie: Expand ID for UICal to TurnOnOff ResidualEC
	ResidualEC_ID,                   // 16
}VoiceModuleId;

typedef enum
{
    OneShotMode = 0,
    CyclicMode
}VoiceDebugFrequentT;


//EQ
#define EQ_FILTER_15_TAPS_ORDER     15  /* For 15 taps */
#define EQ_FILTER_31_TAPS_ORDER     31  /* For 31 taps */


enum
{
   TX_PATH_15_TAPS   = 0,
   RX_PATH_15_TAPS   = 1,
   BOTH_PATH_15_TAPS = 2,
   TX_PATH_31_TAPS   = 3,
   RX_PATH_31_TAPS   = 4,
   BOTH_PATH_31_TAPS = 5
};

//Volume/Mute
enum VolumeSide{
	  TX_VOLUME
	, RX_VOLUME
	, SIDETONE_VOLUME
	, EC_THRESHOLD
};

enum MuteSide{
	TX_MUTE,
	RX_MUTE
};


//AGC

typedef struct
{
    unsigned short MinEnvelopeTx;
    unsigned short MinEnvelopeRx;
    unsigned short LowAmbientNoise;
} AVC_ts;

enum
{
   TX_PATH                      = 0,
   RX_PATH                      = 1,
   BOTH_PATH                    = 2,
   PATH_RESERVED                = 3,
   TX_PATH_UPDATE_INIT_PERIOD   = 4,
   RX_PATH_UPDATE_INIT_PERIOD   = 5,
   BOTH_PATH_UPDATE_INIT_PERIOD = 6
};


//EC

typedef struct
{
    unsigned short EchoToNoiseThresFactor;
    unsigned short AttenuationFactor1;
    unsigned short ECRefThresExp;
    unsigned short PeakInThreshExp;
    unsigned short RESRefThresExp;
} EC_ts;

//EC mode
enum EcMode{
	ES,  //	Normal = HANDSET, HEADSET, BT
	HF, // CARKIT
	SP
};

enum ResMode {
	ATTN,
	DTX
};
enum MuteMode {
	MUTE_OFF1,
	MUTE_ON_DC,
	MUTE_ON_RN
};


/****************************************************************
    UI elements
*****************************************************************/
//EcMode

//EC
typedef struct
{
	unsigned short Status;          //0: Off, 1: On
    unsigned short ResMode;
    unsigned short sampleDelay;
    unsigned short ecLmsBurst;
    unsigned short attenuationFactor1;
    unsigned short EchoToNoiseThresFactor;
} UICal_EC;



//NS
typedef struct
{
    unsigned short Status;         //0: Off, 1: On
    unsigned short nsMaxSupress;
    unsigned short nsMaxNoise;
} UICal_NS;

//EQ
typedef struct
{
    unsigned short Status;         //0: Off, 1: On
    unsigned short Eq_Length;      //15 or 31

    //This filter is Symmetrical Coefficients filter,  only half of it is needed
    unsigned short EqTap[(EQ_FILTER_31_TAPS_ORDER+1)/2];
} UICal_EQ;


//AGC
typedef struct
{
    unsigned short Status;			//0: Off, 1: On
    unsigned short MaxAgcGain;
    unsigned short Level1;
    unsigned short MinEnvelope;
    unsigned short initPeriod;
} UICal_AGC;


//VolMute_TX
typedef struct
{
    unsigned short Status;			//0: Off, 1: On
    unsigned short muteType;    	//0:off; 1:ON_DC; 2:ON_RN
    unsigned short VolumeIndex;
    unsigned short ECRefThresExp;
    unsigned short RESRefThresExp;
    unsigned short PeakInThreshExp;
} UICal_VolMute_TX;

//VolMute_RX
typedef struct
{
    unsigned short Status;			//0: Off, 1: On
    unsigned short muteType;
    unsigned short VolumeIndex;
} UICal_VolMute_RX;


//AVC
typedef struct
{
    unsigned short Status;			//0: Off, 1: On
    unsigned short MaxAgcGain;
    unsigned short Level1;
    unsigned short MinEnvelope;
    unsigned short DrcGainFactor;
    unsigned short LowAmbientNoise;
    unsigned short MaxDrcGain;
    unsigned short AdditionalGain;
    unsigned short initPeriod;
} UICal_AVC;



/**********************************************************
APIs for UI Cal
***********************************************************/
void UICal_MSA_Reports_Request(unsigned short moduleIDs);
void UICal_MSA_Report_Parse();


void UICal_MSA_AGC_TX_Set(
    unsigned short MaxAgcGain,
    unsigned short Level1,
    unsigned short  MinEnvelope,
    unsigned short initPeriod);

void UICal_MSA_AVC_RX_Set(
    unsigned short MaxAgcGain,
    unsigned short Level1,
    unsigned short  MinEnvelope,
    unsigned short DrcGainFactor,
    unsigned short LowAmbientNoise,
    unsigned short MaxDrcGain,
    unsigned short AdditionalGain,
    unsigned short initPeriod);

void UICal_MSA_EC_TX_Set(
    unsigned short ResMode,
    unsigned short sampleDelay,
    unsigned short ecLmsBurst,
    unsigned short attenuationFactor1,
    unsigned short EchoToNoiseThresFactor );


void UICal_MSA_EQ_RX_Set(UICal_EQ *Eq);
void UICal_MSA_EQ_TX_Set(UICal_EQ *Eq);

void UICal_MSA_NS_RX_Set(unsigned short nsMaxSupress, unsigned short nsMaxNoise);
void UICal_MSA_NS_TX_Set(unsigned short nsMaxSupress, unsigned short nsMaxNoise);

void UICal_MSA_VolumeMute_RX_Set(unsigned short muteType, unsigned short VolumeIndex);
void UICal_MSA_VolumeMute_TX_Set(
    unsigned short muteType,
    unsigned short VolumeIndex,
    unsigned short  ECRefThresExp,
    unsigned short RESRefThresExp,
    unsigned short PeakInThreshExp);


void UICal_MSA_AGC_TX_TurnOnOff(unsigned short On);
void UICal_MSA_AVC_RX_TurnOnOff(unsigned short On);
void UICal_MSA_EC_TX_TurnOnOff(unsigned short On);
void UICal_MSA_EQ_RX_TurnOnOff(unsigned short On);
void UICal_MSA_EQ_TX_TurnOnOff(unsigned short On);
void UICal_MSA_NS_RX_TurnOnOff(unsigned short On);
void UICal_MSA_NS_TX_TurnOnOff(unsigned short On);
void UICal_MSA_ResidualEC_TurnOnOff(unsigned short On);
void UICal_MSA_VolumeMute_RX_TurnOnOff(unsigned short On);
void UICal_MSA_VolumeMute_TX_TurnOnOff(unsigned short On);

/*for audio loopback test*/

void start_loopback();
void stop_loopback();
