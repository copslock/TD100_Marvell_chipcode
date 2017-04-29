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

#ifndef __READDATA__H__
#define __READDATA__H__

#include "misc.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
	unsigned char* pBuf;
	unsigned char* pBsCur;
	int datalen;
	int bufSz;
}StoreInputFileDataBuf;

int Init_FileBuf(StoreInputFileDataBuf* buf, unsigned int sz);
int Deinit_FileBuf(StoreInputFileDataBuf* buf);
int Clear_FileBuf(StoreInputFileDataBuf* buf);
void Repack_FileBuf(StoreInputFileDataBuf* buf);
int Prepare_FileBufData(StoreInputFileDataBuf* buf, IPP_FILE* fp);

//load the sequence header and 1st frame
int load_rawh264_seqhdr1stframe(unsigned char** ppSeqHdr, int* pFragLen, StoreInputFileDataBuf* buf, IPP_FILE* fp, int* pbAllFileDataLoaded, void* pOther);
int load_rcv2VC1SPMP_seqhdr1stframe(unsigned char** ppSeqHdr, int* pFragLen, StoreInputFileDataBuf* buf, IPP_FILE* fp, int* pbAllFileDataLoaded, void* pOther);
int load_rawh263_1stframe(unsigned char** ppSeqHdr, int* pFragLen, StoreInputFileDataBuf* buf, IPP_FILE* fp, int* pbAllFileDataLoaded, void* pOther);
int load_rawMpeg2_seqhdr1stframe(unsigned char** ppSeqHdr, int* pFragLen, StoreInputFileDataBuf* buf, IPP_FILE* fp, int* pbAllFileDataLoaded, void* pOther);
int load_rawMpeg4_seqhdr1stframe(unsigned char** ppSeqHdr, int* pFragLen, StoreInputFileDataBuf* buf, IPP_FILE* fp, int* pbAllFileDataLoaded, void* pOther);

//load the next frame
int load_rawh264_nextframe(unsigned char** ppFrame, int* pFrameLen, StoreInputFileDataBuf* buf, IPP_FILE* fp, int* pbAllFileDataLoaded, void* pOther);
int load_rcv2VC1SPMP_nextframe(unsigned char** ppFrame, int* pFrameLen, StoreInputFileDataBuf* buf, IPP_FILE* fp, int* pbAllFileDataLoaded, void* pOther);
int load_rawh263_nextframe(unsigned char** ppFrame, int* pFrameLen, StoreInputFileDataBuf* buf, IPP_FILE* fp, int* pbAllFileDataLoaded, void* pOther);
int load_rawMpeg2_nextframe(unsigned char** ppFrame, int* pFrameLen, StoreInputFileDataBuf* buf, IPP_FILE* fp, int* pbAllFileDataLoaded, void* pOther);
int load_rawMpeg4_nextframe(unsigned char** ppFrame, int* pFrameLen, StoreInputFileDataBuf* buf, IPP_FILE* fp, int* pbAllFileDataLoaded, void* pOther);

int load_fragment_bylengthfile(unsigned char** ppFrag, int* pFragLen, StoreInputFileDataBuf* buf, IPP_FILE* fp, int* pbAllFileDataLoaded, void* pOther);

#ifdef __cplusplus
}
#endif

#endif
