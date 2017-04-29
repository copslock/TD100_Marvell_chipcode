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

#ifndef MP2DEC_H
#define MP2DEC_H

#ifdef _DEBUG
#include <stdlib.h>
#endif

#ifdef _LINUX
	#include <sys/time.h>
	#define _T(x) (x)
	#define _tprintf printf
	#define _tfopen fopen
	#define TCHAR char
	#define _TCHAR	char
	#define _tmain	main
#else
	#include <time.h>
	#include <tchar.h>
#endif

#include <stdio.h>
#include "codecVC.h"
#include "misc.h"

//define the input buffer size
#define     MAX_STREAM_LEN_QCIF     (64*1024/8)     /* 64K bit */
#define		MAX_STREAM_LEN_CIF		(1024*1024/8)		/*	256K bits	*/
#define		DATA_BUFFER_SIZE			(MAX_STREAM_LEN_CIF * 2)	//DATA_BUFFER_SIZE is the input buffer size
//#define		DATA_BUFFER_SIZE			8000

//following macros are used by a simple MPEG2 PS parser in sample code
#define MPEG2_SEQ_HEADER_SYNC	0x000001B3
#define MPEG2_PACK_START_CODE	0x000001BA
#define MPEG2_VIDEO_ESTREAM		0x000001E0
#define MPEG2_ISO_END_CODE		0x000001B9

#define GETNBYTES(pBitstream,code,n)				\
{												\
	int _i = 0;									\
	code = 0;									\
	(pBitstream)->pBsCurByte += ((pBitstream)->bsCurBitOffset)>>3;\
	(pBitstream)->bsCurBitOffset &= 0x7;		\
	while ( _i < n )							\
	{											\
		code <<= 8;								\
		code |= (pBitstream)->pBsCurByte[_i++];	\
	}											\
}

#define INITCODE(pBitstream,code)				\
code = (pBitstream)->pBsCurByte[0];				\
code = (code<<8)|(pBitstream)->pBsCurByte[1];	\
code = (code<<8)|(pBitstream)->pBsCurByte[2];	\
code = (code<<8)|(pBitstream)->pBsCurByte[3];

//function declaration
int CmdOptionsParser(int argCnt, _TCHAR *argVal[], FILE **ppFile);
int MP2DecApp(int argc, _TCHAR *argv[]);

int MPEG2decoder_interNSCdisabled(FILE* inputCmpFile, FILE* outYUVFile,unsigned long * pFrameNumber);
int MPEG2decoder_interNSCenabled(FILE* inputCmpFile, FILE* outYUVFile,unsigned long * pFrameNumber);
IppCodecStatus MPEG2decoder(FILE* inputCmpFile, FILE* outYUVFile,unsigned long * pFrameNumber);

IppCodecStatus SeekSyncCode(IppBitstream *pSrcBitstream, Ipp32u *pOffset);
IppCodecStatus videoReloadBufferFromPES_MPEG2(IppBitstream *pSystemBitstream, IppBitstream *pVideostream, FILE *stream, Ipp32u* puCurPacket_length);
IppCodecStatus Seek_Next_Packet(IppBitstream *pBitstream,FILE * fpStream,Ipp32u *puPacketLength);

IppCodecStatus videoInitBuffer (IppBitstream *pBufInfo);
IppCodecStatus videoReloadBuffer (IppBitstream *pBufInfo, FILE *stream);
IppCodecStatus videoFreeBuffer (IppBitstream *pBufInfo);

IppCodecStatus InitCheckVideoInput(FILE* inputCmpFile, IppBitstream* pVideoStream, IppBitstream* pSysStream, Ipp32u* pCurPacketLength, int* pbSystemInfo);
IppCodecStatus RepackFillVideoInput(FILE* inputCmpFile, IppBitstream* pVideoStream, IppBitstream* pSysStream, Ipp32u* pCurPacketLength, int bSysStm);
IppCodecStatus FreeVideoInput(IppBitstream* pVideoStream, IppBitstream* pSysStream);

void outfile_frame (FILE *outfile,IppPicture *pPicture);



#endif

