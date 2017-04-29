/******************************************************************************
// INTEL CONFIDENTIAL
// Copyright 2000 ~ 2006 Intel Corporation All Rights Reserved. 
// The source code contained or described herein and all documents related to 
// the source code (¡°Material¡±) are owned by Intel Corporation or its 
// suppliers or licensors. Title to the Material remains with Intel Corporation
// or its suppliers and licensors. The Material contains trade secrets and 
// proprietary and confidential information of Intel or its suppliers and 
// licensors. The Material is protected by worldwide copyright and trade secret 
// laws and treaty provisions. No part of the Material may be used, copied, 
// reproduced, modified, published, uploaded, posted, transmitted, distributed, 
// or disclosed in any way without Intel¡¯s prior express written permission.
//
// No license under any patent, copyright, trade secret or other intellectual 
// property right is granted to or conferred upon you by disclosure or delivery 
// of the Materials, either expressly, by implication, inducement, estoppel or 
// otherwise. Any license under such intellectual property rights must be 
// express and approved by Intel in writing.
******************************************************************************/

#ifndef _ASFPARSER_H_
#define _ASFPARSER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "misc.h"
#define _ASF_HEADER_OBJECT_TAG_SIZE 30
#define _ASF_DATA_OBJECT_TAG_SIZE	50
#define _ASF_HEADER_OBJECT_NUMBER   1000

//#define _IPP_GET_WORD( pb, w )   (w) = *(Ipp16u*)(pb); 
//#define _IPP_GET_DWORD( pb, dw ) (dw) = *(Ipp32u*)(pb);
//#define _IPP_GET_QWORD( pb, qw ) (qw) = *(Ipp64u*)(pb);

#define _IPP_GET_WORD( pb, w ) \
            (w) = ((Ipp16u) *(pb + 1) << 8) + *pb;

#define _IPP_GET_DWORD( pb, dw ) \
            (dw) = ((Ipp32u) *(pb + 3) << 24) + \
                   ((Ipp32u) *(pb + 2) << 16) + \
                   ((Ipp32u) *(pb + 1) << 8) + *pb;

#define _IPP_GET_QWORD( pb, qw ) \
            (qw) = ((Ipp64u) *(pb + 6) << 48) + \
                   ((Ipp64u) *(pb + 5) << 40) + \
                   ((Ipp64u) *(pb + 4) << 32) + \
                   ((Ipp64u) *(pb + 3) << 24) + \
                   ((Ipp64u) *(pb + 2) << 16) + \
                   ((Ipp64u) *(pb + 1) << 8) + *pb;
	
#define _IPP_GET_WORD_EX( pb, w )     _IPP_GET_WORD( pb, w ); (pb) += sizeof(Ipp16u);
#define _IPP_GET_DWORD_EX( pb, dw )   _IPP_GET_DWORD( pb, dw ); (pb) += sizeof(Ipp32u);
#define _IPP_GET_QWORD_EX( pb, qw )   _IPP_GET_QWORD( pb, qw ); (pb) += sizeof(Ipp64u);

#define _IPP_LOAD_BYTE( b, p )	b = *(Ipp8u *)p;  p += sizeof( Ipp8u )
#define _IPP_LOAD_WORD( w, p )	_IPP_GET_WORD_EX( p, w )
#define _IPP_LOAD_DWORD( dw, p ) _IPP_GET_DWORD_EX( p, dw )
#define _IPP_LOAD_QWORD( qw, p )  _IPP_GET_QWORD_EX( p, qw )

#define _IPP_LOAD_GUID( g, p ) \
{ \
	_IPP_LOAD_DWORD( g.Data1, p ); \
	_IPP_LOAD_WORD( g.Data2, p ); \
	_IPP_LOAD_WORD( g.Data3, p ); \
	_IPP_LOAD_BYTE( g.Data4[0], p ); \
	_IPP_LOAD_BYTE( g.Data4[1], p ); \
	_IPP_LOAD_BYTE( g.Data4[2], p ); \
	_IPP_LOAD_BYTE( g.Data4[3], p ); \
	_IPP_LOAD_BYTE( g.Data4[4], p ); \
	_IPP_LOAD_BYTE( g.Data4[5], p ); \
	_IPP_LOAD_BYTE( g.Data4[6], p ); \
	_IPP_LOAD_BYTE( g.Data4[7], p ); \
}

typedef struct _IPP_GUID_WMA {          // size is 16
	Ipp32u	Data1;
	Ipp16u	Data2;
	Ipp16u	Data3;
	Ipp8u	Data4[8];
} IPP_GUID_WMA;

typedef struct
{
    Ipp16u		wFormatTag;         /* format type */
    Ipp16s		nChannels;          /* number of channels (i.e. mono, stereo...) */
    Ipp32s		nSamplesPerSec;     /* sample rate */
    Ipp32s		nAvgBytesPerSec;    /* for buffer estimation */
    Ipp16s		nBlockAlign;        /* block size of data */
    Ipp16s		wBitsPerSample;     /* number of bits per sample of mono data */
    Ipp16s		cbSize;             /* the count in bytes of the size of */
									/* extra information (after cbSize) */
} appWAVEFORMATEX;


#define WAVE_FORMAT_EXTENSIBLE 65534
#define WAVE_FORMAT_PCM	1
#define SPEAKER_FRONT_LEFT              0x1
#define SPEAKER_FRONT_RIGHT             0x2
#define SPEAKER_FRONT_CENTER            0x4
#define SPEAKER_LOW_FREQUENCY           0x8
#define SPEAKER_BACK_LEFT               0x10
#define SPEAKER_BACK_RIGHT              0x20
#define SPEAKER_FRONT_LEFT_OF_CENTER    0x40
#define SPEAKER_FRONT_RIGHT_OF_CENTER   0x80
#define SPEAKER_BACK_CENTER             0x100
#define SPEAKER_SIDE_LEFT               0x200
#define SPEAKER_SIDE_RIGHT              0x400
#define SPEAKER_TOP_CENTER              0x800
#define SPEAKER_TOP_FRONT_LEFT          0x1000
#define SPEAKER_TOP_FRONT_CENTER        0x2000
#define SPEAKER_TOP_FRONT_RIGHT         0x4000
#define SPEAKER_TOP_BACK_LEFT           0x8000
#define SPEAKER_TOP_BACK_CENTER         0x10000
#define SPEAKER_TOP_BACK_RIGHT          0x20000


typedef struct
{
    appWAVEFORMATEX Format;       /* The traditional wave file header */
    union {
        unsigned short wValidBitsPerSample; /* bits of precision */
        unsigned short wSamplesPerBlock;    /* valid if wBitsPerSample==0 */
        unsigned short wReserved;           /* If neither applies, set to zero */
    } Samples;
    unsigned int    dwChannelMask;        /* which channels are present in stream */
    IPP_GUID_WMA SubFormat;           /* specialization */
} IPP_WAVEFORMATEXTENSIBLE;

IPPCODECFUN(IppCodecStatus, ParseMetadataObject)(const Ipp8u *pBuffer, IppWMADecoderConfig *pConfig);
IPPCODECFUN(IppCodecStatus, ParseASFHeader)(IPP_FILE *fpi, IppWMADecoderConfig *pConfig);
IPPCODECFUN(IppCodecStatus, ParseDataObjectHead)(IPP_FILE *fpi, int *pPackets);
IPPCODECFUN(IppCodecStatus, ParseNextPacketAndGetPayload)(IPP_FILE *fpi, IppWMADecoderConfig *pConfig, IppBitstream *pBitStream);
IPPCODECFUN(IppCodecStatus, IPP_WriteWaveHeaders)(IPP_FILE *pwfio, IppWMADecoderConfig *pConfig);
IPPCODECFUN(IppCodecStatus, IPP_UpdateWaveHeaders)(IPP_FILE *pwfio, unsigned int cbSize, int dataSize);

#ifdef __cplusplus
}
#endif

#endif    /* #ifndef _CODECWMA_H_ */

/* EOF */
