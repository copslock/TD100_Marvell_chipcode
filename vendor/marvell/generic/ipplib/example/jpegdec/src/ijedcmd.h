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



#ifndef _IJEDCMD_H_
#define _IJEDCMD_H_

#include "codecDef.h"	/* General Codec external header file*/


#ifdef __cplusplus
extern "C" {
#endif

/* High-level JPEG decoder API */
IPPCODECEXAPI(IppCodecStatus, JPEGDec_Init, (void *pInStreamHandler, IppBitstream *pSrcBitStream, 
			IppPicture *pDstPicture, MiscGeneralCallbackTable  *pSrcCallbackTable, void **ppDecoderState))

IPPCODECEXAPI(IppCodecStatus, JPEG_ROIDec, (void *pInStreamHandler, IppBitstream *pSrcBitStream,
			IppPicture *pDstPicture, IppJPEGDecoderParam *pDecoderPar, void *pDecoderState))

IPPCODECEXAPI(IppCodecStatus, JPEG_Rotate, (void *pInStreamHandler, void *pOutStreamHandler,
			IppBitstream *pSrcBitStream, IppBitstream *pDstBitStream, 
			IppJPEGDecoderParam *pDecoderPar, void *pDecoderState))

extern int ReadStreamFromFile(void **pDstBuf, int iSize, int iCount, void *pHandler);

#ifdef __cplusplus
}
#endif

#endif    /* #ifndef _CODECJP_H_ */
/* EOF */