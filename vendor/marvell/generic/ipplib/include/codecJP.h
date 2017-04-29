/***************************************************************************************** 
Copyright (c) 2009, Marvell International Ltd. 
All Rights Reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Marvell nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY MARVELL ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL MARVELL BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************************/


#ifndef _CODECJP_H_
#define _CODECJP_H_

#include "codecDef.h"	/* General Codec external header file*/

#ifdef __cplusplus
extern "C" {
#endif

/*********** Data Types, Data Structures ************************/
typedef enum {
    JPEG_BASELINE       = 1,
    JPEG_PROGRESSIVE    = 4
} IppJPEGMode;

typedef enum {
    JPEG_444            = 0,
    JPEG_422            = 1,
    JPEG_422I           = 2,
    JPEG_411            = 3
} IppJPEGYCbCrSampling;

typedef enum {
    JPEG_BGR888,
    JPEG_BGR555,
    JPEG_BGR565,
	JPEG_ABGR,
	/* RGB */
	JPEG_RGB888,
	JPEG_RGB555,
	JPEG_RGB565,
	JPEG_ARGB,
    /* added to support YUV input & output */
    JPEG_YUV444,
    JPEG_YUV422,
	JPEG_YUV422I,
    JPEG_YUV411,
	JPEG_GRAY8
} IppJPEGColorFormat;

typedef enum {
	JPEG_SLICEBUF = 0,
	JPEG_INTEGRATEBUF = 1
} IppJPEGBufferMode;


typedef struct _IppJPEGEncoderParam {
    int     nQuality;				/* 0..100 */
    int     nRestartInterval;		/* number of restart intervals */
    int     nJPEGMode;				/* JPEG_BASELINE or JPEG_PROGRESSIVE */
    int     nSubsampling;			/* JPEG_444, JPEG_422 or JPEG_411 */
	int		nBufType;	
    void    *pStreamHandler;
	void    *pSrcPicHandler;
} IppJPEGEncoderParam;

typedef struct _IppJPEGDecoderParam {

	IppiRect			srcROI;					/* Src ROI region for ROI decoding */
	int					nScale;
	int             	nDesiredColor;			/* Output image format: YUV or RGB space */
	int   				nAlphaValue;			/* Alpha value, only valid for ARGB format */
}IppJPEGDecoderParam;

/* JPEG encoder API */
IPPCODECEXAPI(IppCodecStatus, EncoderInitAlloc_JPEG, (IppJPEGEncoderParam *pEncoderPar, 
			IppPicture *pSrcPicture, IppBitstream *pDstBitstream, 
			MiscGeneralCallbackTable *pSrcCallbackTable, void **ppDstEncoderState))

IPPCODECEXAPI(IppCodecStatus, Encode_JPEG, (IppPicture *pSrcPicture,
			IppBitstream *pDstBitstream, void *pEncoderState))

IPPCODECEXAPI(IppCodecStatus, EncoderFree_JPEG, (void **ppEncoderState))


IPPCODECEXAPI(IppCodecStatus, DecoderFree_JPEG, (void **ppDecoderState))

/* Low-level JPEG decoder API */
IPPCODECEXAPI(IppCodecStatus, DecoderInitAlloc_JPEG, (IppBitstream *pSrcStream, IppPicture *pDstPicture,
			MiscGeneralCallbackTable  *pSrcCallbackTable, void **ppDecoderState, void* fp))

IPPCODECEXAPI(IppCodecStatus, Decode_JPEG, (IppPicture *pDstPicture, IppJPEGDecoderParam *pDecoderPar, void *pDecoderState))

#ifdef __cplusplus
}
#endif

#endif    /* #ifndef _CODECJP_H_ */
/* EOF */




