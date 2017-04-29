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


#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>

#include "ippdefs.h"
#include "codecVC.h"
#include "vguser.h"

#ifdef __cplusplus
	extern "C" {
#endif

#define    EOS_LEN    4
#define    EOS1       0       /* EOS data, byte 1,2,3,4 */
#define    EOS2       0
#define    EOS3       0x01
#define    EOS4       0xb6

#define		MPEG4_MAX_STREAM_LEN_CIF	(1024*1024/8)		/*	256K bits	*/

/* General Buffer Control Function Set */
extern  int init_cmp_input_buf (IppBitstream *pBufInfo);
extern  int free_cmp_input_buf (IppBitstream *pBufInfo);
extern void insert_eos (IppBitstream *pBufInfo);
extern int reload_bitstream (IppBitstream *pBufInfo, FILE *stream);

extern int init_cmp_output_buf (IppBitstream *pBufInfo, int buffersize);
extern int free_cmp_output_buf (IppBitstream *pBufInfo);
extern int output_bitstream (IppBitstream *pBufInfo, FILE *stream);

IPPCODECAPI(IppCodecStatus, LoadExtendPlane, (FILE* infile, IppPicture * pPicture));


extern int MPEG4encoder(FILE * inParFile, FILE * outYUVFile, AppGUIPara *pGUIPara,
			int * p_terminate, unsigned long * pFrameNumber);
extern int MPEG4decoder(FILE * inputCmpFile,FILE * outYUVFile, AppGUIPara *pGUIPara, 
			int * p_terminate,int iWidth,int iHeight, unsigned long * pFrameNumber);

extern int MPEG4Coredecoder(FILE * inputCmpFile, FILE * outYUVFile, AppGUIPara *pGUIPara, 
			int * p_terminate,int iWidth,int iHeight, unsigned long * pFrameNumber);


extern void outfile_frame (FILE *outfile,IppPicture *pPicture);



#ifdef __cplusplus
	}
#endif

