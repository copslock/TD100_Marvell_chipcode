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

#ifndef	__VGUSER_H__
#define	__VGUSER_H__


#include "ippVC.h"
#include "codecVC.h"
#include "stdlib.h"
#include "misc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define     MAX_STREAM_LEN_QCIF  (64*1024/8)     /* 64K bit */
#define     MAX_STREAM_LEN_CIF   (1024*1024/8)   /* 256K bits */
#define     DATA_BUFFER_SIZE     (MAX_STREAM_LEN_CIF * 2)

int videoInitBuffer (IppBitstream *pBufInfo);

int videoReloadBuffer (IppBitstream *pBufInfo, IPP_FILE *srcFile);

void videoFreeBuffer (IppBitstream *pBufInfo);

int outfile_frame ( IPP_FILE *outfile,IppPicture *pPicture);

int CheckStreamSpace(IppBitstream *pVideoStream);

#ifdef __cplusplus
}
#endif

#endif
