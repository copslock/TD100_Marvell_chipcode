#ifndef ACM_STUB_H
#define ACM_STUB_H

#include "linux_types.h"
#include "acm.h"

/* ACM Control definition */
/*
 * increase the max gain number from 8 to 16 after forwarding
 * hifi audio path control msg to CP side
 */
#define ACM_MAXPGACNT_PERPATH  (16)

/* ACM component definition */

typedef struct
{
	ACM_Component component;
} AudioCpntRegReq;

typedef AudioCpntRegReq AudioCpntRegCnf;

typedef enum
{
	STUB_SUCCESS,
	STUB_ERROR,
	STUB_TIMEOUT,
} STUB_ReturnCode;

typedef struct
{
	int path;
	unsigned int gain_id;
	int gain_val;
	unsigned char mutePath;
	unsigned char volume;
	/*Below are for Levante. In micco they are dummy */
	unsigned char regIndex;
	unsigned char gainMask;
	unsigned char gainShift;
} GAIN_RECORD;

typedef struct
{
	int gain_nums;
	GAIN_RECORD gain_record[ACM_MAXPGACNT_PERPATH];
} ACM_GAINS;


typedef struct AudioControlReq_struct
{
	ACM_AudioDevice device;
	ACM_AudioFormat format;
	ACM_AudioVolume volume;
	ACM_AudioMute mute;
}AudioControlReq;

typedef struct AudioControlCnf_struct
{
	ACM_AudioDevice device;
	ACM_AudioFormat format;
	ACM_GAINS gains;
}AudioControlCnf; /*no use*/


/* ACM stream definition */
typedef struct
{
	ACM_PacketTransferFunc streamHandle;
	ACM_StreamType streamType;
	ACM_SrcDst srcDst;
	ACM_CombWithCall combWithCall;
} AudioStreamInOutReq;

typedef struct
{
	int status;
	ACM_PacketTransferFunc streamHandle;
} AudioStreamInOutCnf;


typedef  struct
{
	ACM_PacketTransferFunc streamHandle;
	unsigned short   *data;
	int dataSize;
} STREAM_INDICATE;

#define NUM_OF_DB_ENTRIES       (8)

typedef  struct
{
	int handle_num;           //max is NUM_OF_DB_ENTRIES
	STREAM_INDICATE streamInd[NUM_OF_DB_ENTRIES];
} UPLINKSTREAM_REQUEST;

typedef struct
{
	int handle_num;
	STREAM_INDICATE streamInd[1];
} DOWNLINKSTREAM_REQUEST;

typedef struct
{
	ACM_Component component;
} ACMAudioComponentRegisterReq;

typedef ACMAudioComponentRegisterReq ACMAudioComponentRegisterRsp;

typedef struct
{
	ACM_AudioDevice device;
	ACM_AudioFormat format;
	ACM_AudioVolume volume;
} AudioDeviceEnableReq;


typedef struct
{
	int status;
	ACM_AudioDevice device;
	ACM_AudioFormat format;
	ACM_GAINS gains;
} AudioDeviceEnableRsp;


typedef struct
{
	ACM_AudioDevice device;
	ACM_AudioFormat format;
} AudioDeviceDisableReq;

typedef AudioDeviceEnableRsp AudioDeviceDisableRsp;

typedef AudioDeviceEnableReq AudioDeviceVolumeSetReq;

typedef AudioDeviceEnableRsp AudioDeviceVolumeSetRsp;

typedef struct
{
	ACM_AudioDevice device;
	ACM_AudioFormat format;
	ACM_AudioMute mute;
} AudioDeviceMuteReq;

typedef AudioDeviceEnableRsp AudioDeviceMuteRsp;


typedef ACM_PacketTransferFunc APPSStreamHandle_t;

typedef struct
{
	APPSStreamHandle_t streamHandle;
	ACM_StreamType streamType;
	ACM_SrcDst srcDst;
	ACM_CombWithCall combWithCall;
} AudioStreamOutStartReq;

typedef struct
{
	int status;
	APPSStreamHandle_t streamHandle;
} AudioStreamOutStartRsp;

typedef struct
{
	APPSStreamHandle_t streamHandle;
	ACM_StreamType streamType;
	ACM_SrcDst srcDst;
} AudioStreamInStartReq;

typedef AudioStreamOutStartRsp AudioStreamInStartRsp;

typedef struct
{
	APPSStreamHandle_t streamHandle;
} AudioStreamOutStopReq;

typedef AudioStreamOutStartRsp AudioStreamOutStopRsp;


typedef struct
{
	APPSStreamHandle_t streamHandle;
} AudioStreamInStopReq;

typedef AudioStreamOutStartRsp AudioStreamInStopRsp;

typedef struct
{
	UINT8 data[MSASETTINGDATALEN];
} ACMAudioDSPSettings;

int AudioGetUpLinkStream(ACM_PacketTransferFunc packetTransferFunc, unsigned int *data, unsigned int dataSize);
int AudioPutDownLinkStream(ACM_PacketTransferFunc packetTransferFunc, unsigned int *data, unsigned int dataSize);

#endif


