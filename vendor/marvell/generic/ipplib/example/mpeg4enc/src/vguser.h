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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "misc.h"
#include "ippVC.h"
#include "codecDef.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TOKEN_SIZE 4096
#define ERES_OK 0
#define ERES_PARAMS 1
#define ERES_NOOBJ 2
#define ERES_MEMORY 3
#define ERES_EOF 4
#define ERES_FORMAT 5

#define    EOS_LEN    4
#define    EOS1       0       /* EOS data, byte 1,2,3,4 */
#define    EOS2       0
#define    EOS3       0x01
#define    EOS4       0xb6

#define     MAX_STREAM_LEN_QCIF     (64*1024/8)     /* 64K bit */
#define		MAX_STREAM_LEN_CIF		(1024*1024/8)		/*	256K bits	*/
#define		DATA_BUFFER_SIZE			(MAX_STREAM_LEN_CIF * 2 * 4)

/* info from GUI. */
typedef struct _AppGUIPara {

	int		bDecDisplayEnable;
	int		bTransposeEnable;
	int		bIsQVGA;
	int		bEncPreviewEnable;

} AppGUIPara;

/* Memory Allocation related functions */
int malloc_align_64u (
	Ipp8u ** ppAddress, 
	int bufSize
);

int malloc_align_32u (
	Ipp8u ** ppAddress, 
	int bufSize
);

void free_align (
	Ipp8u * pAddress
);

IppCodecStatus videoReloadBuffer (
	IppBitstream *pBufInfo,
	FILE *stream
);

IppCodecStatus videoInitBuffer (
	IppBitstream *pBufInfo
);

IppCodecStatus videoFreeBuffer (
	IppBitstream *pBufInfo
);

/* General Buffer Control Function Set */
extern  int init_cmp_input_buf (IppBitstream *pBufInfo);
extern  int free_cmp_input_buf (IppBitstream *pBufInfo);
extern int reload_bitstream (IppBitstream *pBufInfo, FILE *stream);
extern void outfile_frame (FILE *outfile,IppPicture *pPicture);

/* Subsampling for VOP preview in encoder part */
int QCIFYTo64x48(const Ipp8u * pSrc, int srcStep, Ipp8u * pDst, int dstStep);
int QCIFCTo64x48(const Ipp8u * pSrc, int srcStep, Ipp8u * pDst, int dstStep);
int QCIFYTTo48x64(const Ipp8u * pSrc, int srcStep, Ipp8u * pDst, int dstStep);
int QCIFCTTo48x64(const Ipp8u * pSrc, int srcStep, Ipp8u * pDst, int dstStep);

int	 parGetToken(FILE *m_fp, char *pchBuf);
int	 parSkipComment(FILE *m_fp);
int	 parSkipSpace(FILE *m_fp);
extern int free_cmp_output_buf (IppBitstream *pBufInfo);
extern int init_cmp_output_buf (IppBitstream *pBufInfo, int buffersize);
extern int output_bitstream (IppBitstream *pBufInfo, FILE *stream);
IPPCODECAPI(IppCodecStatus, LoadExtendPlane, (FILE* infile, IppPicture * pPicture));




#ifdef __cplusplus
}
#endif


#endif
