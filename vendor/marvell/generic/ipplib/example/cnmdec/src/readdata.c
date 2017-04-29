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
#include "readdata.h"
#include "misc.h"

int Init_FileBuf(StoreInputFileDataBuf* buf, unsigned int sz)
{
	void* pTmp;
	if(IPP_OK != IPP_MemMalloc(&pTmp, sz, 4)) {
		return -1;
	}
	buf->bufSz = sz;
	buf->datalen = 0;
	buf->pBuf = buf->pBsCur = (unsigned char*)pTmp;
	return 0;
}

int Deinit_FileBuf(StoreInputFileDataBuf* buf)
{
	void* pTmp;
	int rt = 0;
	pTmp = (void*)buf->pBuf;
	if(pTmp) {
		rt = IPP_MemFree(&pTmp);
	}
	buf->pBuf = NULL;
	return rt;
}

int Clear_FileBuf(StoreInputFileDataBuf* buf)
{
	buf->datalen = 0;
	buf->pBsCur = buf->pBuf;
	return 0;
}

static int Expand_FileBuf(StoreInputFileDataBuf* buf, int newsz)
{
	int off = buf->pBsCur - buf->pBuf;
	void * pTmp = (void*)buf->pBuf;
	int rt = IPP_MemRealloc(&pTmp, buf->bufSz, newsz);
	if(NULL == (buf->pBuf=(unsigned char*)pTmp) || IPP_OK != rt) {
		return -1;
	}
	IPP_Printf("Expand file buffer sz from %d to %d\n", buf->bufSz, newsz);
	buf->pBsCur = buf->pBuf + off;
	buf->bufSz = newsz;
	return 0;
}

void Repack_FileBuf(StoreInputFileDataBuf* buf)
{
	if(buf->pBuf != buf->pBsCur) {
		IPP_Memmove(buf->pBuf, buf->pBsCur, buf->datalen-(buf->pBsCur-buf->pBuf));
		buf->datalen -= buf->pBsCur-buf->pBuf;
		buf->pBsCur = buf->pBuf;
	}
	return;
}

static __inline int Fill_FileBufFromFile(StoreInputFileDataBuf* buf, IPP_FILE* fp)
{
	int read = (int)IPP_Fread(buf->pBuf + buf->datalen, 1, buf->bufSz - buf->datalen, fp);
	buf->datalen += read;
	return read;
}


int Prepare_FileBufData(StoreInputFileDataBuf* buf, IPP_FILE* fp)
{
	Repack_FileBuf(buf);
	if(buf->bufSz == buf->datalen) {
		return -1;
	}
	return Fill_FileBufFromFile(buf, fp);
}

static int ForceRead_FileBufData(StoreInputFileDataBuf* buf, IPP_FILE* fp, int wantlen)
{
	int read = (int)fread(buf->pBuf, 1, wantlen, fp);
	buf->datalen = read;
	buf->pBsCur = buf->pBuf;
	return read;
}

#define H264_NALU_TYPE_SLICE	1
#define H264_NALU_TYPE_DPA  	2
#define H264_NALU_TYPE_DPB  	3
#define H264_NALU_TYPE_DPC  	4
#define H264_NALU_TYPE_IDR  	5
#define H264_NALU_TYPE_SEI  	6
#define H264_NALU_TYPE_SPS  	7
#define H264_NALU_TYPE_PPS  	8
#define H264_NALU_TYPE_AUD  	9
#define H264_NALU_TYPE_EOSEQ	10
#define H264_NALU_TYPE_EOSTREAM 11
#define H264_NALU_TYPE_FILL 	12

static unsigned char* seek_h264nal(unsigned char* pData, int len, int* pStartCodeLen)
{
	unsigned int code;
	unsigned char* pStartCode = pData;
	if(len < 4) {
		return NULL;
	}
	code = ((unsigned int)pStartCode[0]<<24) | ((unsigned int)pStartCode[1]<<16) | ((unsigned int)pStartCode[2]<<8) | pStartCode[3];
	while((code&0xffffff00) != 0x00000100) {
		len--;
		if(len < 4) {
			return NULL;
		}
		code = (code << 8)| pStartCode[4];
		pStartCode++;
	}

	*pStartCodeLen = 3;
	if(pStartCode>pData) {
		//though 0x000001 is the official startcode for h264, but for most of h264, use 0x00000001 as the startcode.
		//therefore, if 0x00 exist before 0x000001, we think 0x00000001 is the startcode
		if(pStartCode[-1] == 0) {
			pStartCode--;
			*pStartCodeLen = 4;
		}
	}
	return pStartCode;
}

static int query_h264Oneframe(unsigned char* pData, int len, unsigned char** ppBeginSliceBeginPos, unsigned char** ppEndSliceEndPos, unsigned char** ppMistakeStopPos)
{
	unsigned char* pNAL = NULL, *p1stSlice = NULL, *pFrameEnd = NULL;
	unsigned int naltype;
	int SClen;
	//seek first slice in frame
	for(;;) {
		pNAL = seek_h264nal(pData, len, &SClen);
		if(pNAL == NULL) {
			break;
		}
		len -= pNAL+SClen+1-pData;
		pData = pNAL+SClen+1;
		if((pNAL[SClen]&0x1f) == H264_NALU_TYPE_SLICE || (pNAL[SClen]&0x1f) == H264_NALU_TYPE_IDR) {
			p1stSlice = pNAL;
			break;
		}
	}

	if(p1stSlice == NULL) {
		*ppBeginSliceBeginPos = NULL;
		*ppEndSliceEndPos = NULL;
		return 2;
	}

	//seek the frame end
	for(;;) {
		pNAL = seek_h264nal(pData, len, &SClen);
		if(pNAL == NULL) {
			break;
		}
		len -= pNAL+SClen+1-pData;
		pData = pNAL+SClen+1;
		naltype = pNAL[SClen]&0x1f;
		if(naltype == H264_NALU_TYPE_SLICE || naltype == H264_NALU_TYPE_IDR) {
			if(len<1) {
				//no more data
				break;
			}
			if((*pData&0x80) == 0x80) {
				//If first_mb_in_slice is 0, we think this slice should belong to new frame.
				//!!!!NOTE: This judgement method is very simplified, and only fit about 90% h264 baseline stream. For some stream, this method is wrong.!!!!
				//Because parsing stream isn't the key purpose of this sample code, we still adopt this method though it's not totally correct. For real application, it should use more strict method to parse stream.
				//For stream which isn't fit for this simplified method, could parsing the stream by some 3rd tool offline, and use load_fragment_bylengthfile()
				pFrameEnd = pNAL;
				break;
			}
		}else if(naltype>=H264_NALU_TYPE_SEI && naltype<=H264_NALU_TYPE_FILL){
			//!!!!!!!!!!!!!!!!!!!!!!
			//NOTE: if some special nal occur between slice nals, consider those slice nals are belong to different frame. It's not accurate even for baseline h264, but could work for most of stream.
			pFrameEnd = pNAL;
			break;
		}
	}

	*ppBeginSliceBeginPos = p1stSlice;
	*ppEndSliceEndPos = pFrameEnd;
	if(pFrameEnd==NULL) {
		return 1;
	}else{
		return 0;
	}
}

static int query_h264SeqHdr1stFrame(unsigned char* pData, int len, unsigned char** ppSeqHdrBeginPos, unsigned char** pp1stFrameEndPos, unsigned char** ppMistakeStopPos)
{
	unsigned char* pNAL = NULL, *pSPS = NULL, *pPPS = NULL;
	unsigned char* p1stFrameBeg = NULL, *p1stFrameEnd = NULL;
	int SClen;
	int rt;

	//seek SPS
	for(;;) {
		pNAL = seek_h264nal(pData, len, &SClen);
		if(pNAL == NULL) {
			break;
		}
		len -= pNAL+SClen+1-pData;
		pData = pNAL+SClen+1;
		if((pNAL[SClen]&0x1f) == H264_NALU_TYPE_SPS) {
			pSPS = pNAL;
			break;
		}
	}

	if(pSPS == NULL) {
		*ppSeqHdrBeginPos = NULL;
		*pp1stFrameEndPos = NULL;
		return 2;
	}

	//seek PPS
	for(;;) {
		pNAL = seek_h264nal(pData, len, &SClen);
		if(pNAL == NULL) {
			break;
		}
		len -= pNAL+SClen+1-pData;
		pData = pNAL+SClen+1;
		if((pNAL[SClen]&0x1f) == H264_NALU_TYPE_PPS) {
			pPPS = pNAL;
			break;
		}
	}

	if(pPPS == NULL) {
		*ppSeqHdrBeginPos = NULL;
		*pp1stFrameEndPos = NULL;
		return 2;
	}

	rt = query_h264Oneframe(pData, len, &p1stFrameBeg, &p1stFrameEnd, ppMistakeStopPos);
	if(rt == 2) {
		*ppSeqHdrBeginPos = NULL;
		*pp1stFrameEndPos = NULL;
		return 2;
	}else{
		*ppSeqHdrBeginPos = pSPS;
		*pp1stFrameEndPos = p1stFrameEnd;
		return rt;
	}

}


static unsigned char* seek_vc1SPMP_seqMeta(unsigned char* p, int len, int* pSeqMetaLen)
{
	for(; len>=36; ) {
		if(p[3] == 0xC5) {
			if((p[4]==0x04 || p[4]==0x05) && (p[5]==0 && p[6]==0 && p[7]==0)) {
				if(len >= 36 + (p[4]==0x05?1:0)) {
					*pSeqMetaLen = 36 + (p[4]==0x05?1:0);
					return p;
				}
			}
		}
		len--;
		p++;
	}
	return NULL;
}

static int query_vc1SPMPOneframe(unsigned char* pData, int len, unsigned char** ppFrameBeginPos, unsigned char** ppFrameEndPos, unsigned char** ppMistakeStopPos)
{
	unsigned int pureframelen;
	if(len<2) {
		*ppFrameBeginPos = NULL;
		*ppFrameEndPos = NULL;
		return 2;
	}
	*ppFrameBeginPos = pData;
	pureframelen = pData[0] | ((unsigned int)pData[1]<<8) | ((unsigned int)pData[2]<<16);
	if(len < 8 + (int)pureframelen) {
		*ppFrameEndPos = NULL;
		return 1;
	}
	*ppFrameEndPos = pData + 8 + pureframelen;
	return 0;
}


static int query_vc1SPMPSeqHdr1stFrame(unsigned char* pData, int len, unsigned char** ppSeqHdrBeginPos, unsigned char** pp1stFrameEndPos, unsigned char** ppMistakeStopPos)
{
	unsigned char *pSeqMeta = NULL;
	unsigned char* p1stFrameBeg = NULL, *p1stFrameEnd = NULL;
	int SeqMetalen;
	int rt;

	//seek seqMeta
	pSeqMeta = seek_vc1SPMP_seqMeta(pData, len, &SeqMetalen);
	if(pSeqMeta == NULL) {
		*ppSeqHdrBeginPos = NULL;
		*pp1stFrameEndPos = NULL;
		return 2;
	}
	len -= (pSeqMeta-pData)+SeqMetalen;
	pData = pSeqMeta + SeqMetalen;

	IPP_Printf("vc1 struct_c data: 0x%02x%02x%02x%02x\n", pSeqMeta[8], pSeqMeta[9], pSeqMeta[10], pSeqMeta[11]);
	IPP_Printf("vc1 character %d, profile %d, %s simple profile\n", pSeqMeta[4], pSeqMeta[8]>>4, (pSeqMeta[8]>>4) == 0 ? "is" : "isn't");
	IPP_Printf("vc1 MAXBFRAMES %d, if it's 0, should no B frame, if it > 0, should no skip frame.\n", (pSeqMeta[11]>>4)&0x7);
	
	rt = query_vc1SPMPOneframe(pData, len, &p1stFrameBeg, &p1stFrameEnd, ppMistakeStopPos);
	if(rt == 2) {
		*ppSeqHdrBeginPos = NULL;
		*pp1stFrameEndPos = NULL;
		return 2;
	}else{
		*ppSeqHdrBeginPos = pSeqMeta;
		*pp1stFrameEndPos = p1stFrameEnd;
		return rt;
	}

}

static unsigned char* seek_h263PSC(unsigned char* pData, int len)
{
	unsigned int code;
	unsigned char* pStartCode = pData;
	if(len < 5) {
		return NULL;
	}
	code = ((unsigned int)pStartCode[0]<<24) | ((unsigned int)pStartCode[1]<<16) | ((unsigned int)pStartCode[2]<<8) | pStartCode[3];
	while((code&0xfffffc00) != 0x00008000) {
		len--;
		if(len < 5) {
			return NULL;
		}
		code = (code << 8)| pStartCode[4];
		pStartCode++;
	}

	return pStartCode;
}


static int query_h263Oneframe(unsigned char* pData, int len, unsigned char** ppPictureBeginPos, unsigned char** ppPictureEndPos, unsigned char** ppMistakeStopPos)
{
	unsigned char *p1stPSC = NULL, *p2ndPSC = NULL;
	//seek first PSC
	p1stPSC = seek_h263PSC(pData, len);

	if(p1stPSC == NULL) {
		*ppPictureBeginPos = NULL;
		*ppPictureEndPos = NULL;
		return 2;
	}

	//seek the 2nd PSC
	len -= (p1stPSC-pData) + 5;
	pData = p1stPSC + 5;
	p2ndPSC = seek_h263PSC(pData, len);
	*ppPictureBeginPos = p1stPSC;
	*ppPictureEndPos = p2ndPSC;
	if(p2ndPSC == NULL) {
		return 1;
	}
	return 0;
}

//seek code 0x000001XX
static unsigned char* seek_SC_000001xx(unsigned char* pData, int len)
{
	unsigned int code;
	unsigned char* pStartCode = pData;
	if(len < 4) {
		return NULL;
	}
	code = ((unsigned int)pStartCode[0]<<24) | ((unsigned int)pStartCode[1]<<16) | ((unsigned int)pStartCode[2]<<8) | pStartCode[3];
	while((code&0xffffff00) != 0x00000100) {
		len--;
		if(len < 4) {
			return NULL;
		}
		code = (code << 8)| pStartCode[4];
		pStartCode++;
	}

	return pStartCode;
}

static __inline unsigned char* seek_MPEG2SC(unsigned char* pData, int len)
{
	return seek_SC_000001xx(pData, len);
}

static int query_mpeg2OnePicture(unsigned char* pData, int len, unsigned char** ppPictureBeginPos, unsigned char** ppPictureEndPos, int* pPicStructure, unsigned char** ppMistakeStopPos)
{
	unsigned char *pSC = NULL, *pPicBegin;
	int picture_structure;

	*ppPictureBeginPos = NULL;
	*ppPictureEndPos = NULL;

	//seek picture start code
	for(;;) {
		pSC = seek_MPEG2SC(pData, len);
		if(pSC == NULL) {
			return 2;
		}else if(pSC[3] == 0) {
			//found picture start code
			break;
		}else{
			//found other start code
			len -= pSC+4-pData;
			pData = pSC+4;
		}

	}
	pPicBegin = pSC;

	//distinguish field or frame picture
	//seek picture_coding_extension
	len -= pSC+4-pData;
	pData = pSC+4;
	pSC = seek_MPEG2SC(pData, len);
	if(pSC == NULL) {
		return 2;
	}
	len -= pSC-pData;
	pData = pSC;
	if(pSC[3] != 0xB5) {  // 0xB5: extension_start_code
		*ppMistakeStopPos = pSC + 4;
		return 2;
	}
	if(len < 9) {
		//no enough length to hold picture_coding_extension
		return 2;
	}
	if((pSC[4]&0xf0) != 0x80) {
		//not Picture Coding Extension ID
		*ppMistakeStopPos = pSC + 4;
		return 2;
	}
	picture_structure = pSC[6]&0x3;

	*pPicStructure = picture_structure;
	*ppPictureBeginPos = pPicBegin;

	//seek seq end code or seq header start code or gop start code or picture start code, they are the indicator of piture end
	pData += 7;
	len -= 7;
	for(;;) {
		pSC = seek_MPEG2SC(pData, len);
		if(pSC == NULL) {
			break;
		}else if(pSC[3] == 0xb7 || pSC[3] == 0xb3 || pSC[3] == 0xb8 || pSC[3] == 0x00) {
			//found
			break;
		}else{
			//found other start code
			len -= pSC+4-pData;
			pData = pSC+4;
		}
	}

	if(pSC == NULL) {
		return 1;
	}else{
		*ppPictureEndPos = pSC;
		return 0;
	}
}

static int query_mpeg2Oneframe(unsigned char* pData, int len, unsigned char** pp1stPictureBeginPos, unsigned char** ppFrameEndPos, unsigned char** ppMistakeStopPos)
{
	unsigned char *pPic0BeginPos, *pPic0EndPos, *pPic1BeginPos, *pPic1EndPos;
	int picStructure;
	int queryrt;

	*pp1stPictureBeginPos = NULL;
	*ppFrameEndPos = NULL;

	//seek 1st picture
	queryrt = query_mpeg2OnePicture(pData, len, &pPic0BeginPos, &pPic0EndPos, &picStructure, ppMistakeStopPos);
	*pp1stPictureBeginPos = pPic0BeginPos;
	if(queryrt != 0) {
		return queryrt;
	}else{
		//found the whole picture
		if(picStructure == 0 || picStructure == 3) {
			//picStructure == 0 is forbidden by 13818-2, we consider it as frame
			*ppFrameEndPos = pPic0EndPos;
			return 0;
		}
	}

	//last picture is a field picture, seek another picture
	len -= pPic0EndPos-pData;
	pData = pPic0EndPos;
	queryrt = query_mpeg2OnePicture(pData, len, &pPic1BeginPos, &pPic1EndPos, &picStructure, ppMistakeStopPos);
	if(queryrt == 0) {
		if(picStructure == 0 || picStructure == 3) {
			//it's a frame picture, not field picture, we consider it's a new frame. We consider last frame only contain 1 field.
			*ppFrameEndPos = pPic0EndPos;
			return 0;
		}else{
			*ppFrameEndPos = pPic1EndPos;
			return 0;
		}
	}else{
		//only found the frame begin, not found the frame end
		return 1;
	}
}


static int query_mpeg2SeqHdr1stFrame(unsigned char* pData, int len, unsigned char** ppSeqHdrBeginPos, unsigned char** pp1stFrameEndPos, unsigned char** ppMistakeStopPos)
{
	unsigned char *pSeq, *pSeqExt, *p1stPicBeginPos, *pFrameEndPos;
	int rt;

	*ppSeqHdrBeginPos = NULL;
	*pp1stFrameEndPos = NULL;

	//seek seq header start code
	for(;;) {
		pSeq = seek_MPEG2SC(pData, len);
		if(pSeq == NULL) {
			return 2;
		}else if(pSeq[3] == 0xb3) {
			break;
		}else{
			len -= pSeq+4-pData;
			pData = pSeq+4;
		}
	}

	//seek seq ext start code
	len -= pSeq+4-pData;
	pData = pSeq+4;
	for(;;) {
		pSeqExt = seek_MPEG2SC(pData, len);
		if(pSeqExt == NULL) {
			return 2;
		}else if(pSeqExt[3] == 0xb5) {
			break;
		}else{
			len -= pSeqExt+4-pData;
			pData = pSeqExt+4;
		}
	}
	len -= pSeqExt+4-pData;
	pData = pSeqExt+4;
	if(len < 6) {
		//no enough to hold sequence ext header
		return 2;
	}
	if((pSeqExt[4]&0xf0) != 0x10) {
		//not seq ext header
		*ppMistakeStopPos = pSeqExt + 4;
		return 2;
	}

	rt = query_mpeg2Oneframe(pData, len, &p1stPicBeginPos, &pFrameEndPos, ppMistakeStopPos);
	if(rt == 2) {
		*ppSeqHdrBeginPos = NULL;
		*pp1stFrameEndPos = NULL;
		return 2;
	}else{
		*ppSeqHdrBeginPos = pSeq;
		*pp1stFrameEndPos = pFrameEndPos;
		return rt;
	}
}


static __inline unsigned char* seek_MPEG4SC(unsigned char* pData, int len)
{
	return seek_SC_000001xx(pData, len);
}


static int query_mpeg4OneVOP(unsigned char* pData, int len, unsigned char** ppVOPBeginPos, unsigned char** ppVOPEndPos, unsigned char** ppMistakeStopPos)
{
	unsigned char* pVOP, *pSC;

	*ppVOPBeginPos = NULL;
	*ppVOPEndPos = NULL;

	//seek VOP start code
	for(;;) {
		pVOP = seek_MPEG4SC(pData, len);
		if(pVOP == NULL) {
			return 2;
		}
		if(pVOP[3] == 0xB6) {
			break;
		}else{
			len -= pVOP+4-pData;
			pData = pVOP+4;
		}
	}

	*ppVOPBeginPos = pVOP;

	len -= pVOP+4-pData;
	pData = pVOP+4;

	//seek next start code
	pSC = seek_MPEG4SC(pData, len);
	//For SP mpeg4, StudioVideoObjectPlane should not occur. Therefore slice_start_code, extension_start_code and user_data_start_code shouldn't occur in a VOP.
	//Consequently, founding any start code means finishing of one VideoObjectPlane.
	if(pSC == NULL) {
		return 1;   //only found VOP begin, not found VOP end
	}

	*ppVOPEndPos = pSC;
	return 0;   	//found VOP begin and end
}


static int query_mpeg4SeqHdr1stVOP(unsigned char* pData, int len, unsigned char** ppSeqHdrBeginPos, unsigned char** pp1stVOPEndPos, unsigned char** ppMistakeStopPos)
{
	unsigned int tmp, rt;

	unsigned char *pVOL, *pSC=pData, *p1stVOPBegin, *p1stVOPEnd;

	*ppSeqHdrBeginPos = NULL;
	*pp1stVOPEndPos = NULL;

	//seek video_object_layer_start_code
	for(;;) {
		pVOL = seek_MPEG4SC(pData, len);
		if(pVOL == NULL) {
			return 2;
		}
		if((pVOL[3]>>4) == 0x2) {
			break;
		}else{
			len -= pVOL+4-pData;
			pData = pVOL+4;
		}
	}
	//found video_object_layer_start_code
	*ppSeqHdrBeginPos = pVOL;
	//if video_object_start_code is adjacent to video_object_layer_start_code, we consider video_object_start_code is the sequence begin
	if((pVOL-pSC)>=4) {
		pSC = pVOL-4;
		tmp = ((unsigned int)(pSC[0])<<24) | ((unsigned int)(pSC[1])<<16) | ((unsigned int)(pSC[2])<<8) | pSC[3];
		if((tmp&0xffffffe0) == 0x00000100) {
			//found video_object_start_code
			*ppSeqHdrBeginPos = pSC;
		}
	}
	len -= pVOL+4-pData;
	pData = pVOL+4;

	rt = query_mpeg4OneVOP(pData, len, &p1stVOPBegin, &p1stVOPEnd, ppMistakeStopPos);
	if(rt == 2) {
		*ppSeqHdrBeginPos = NULL;
		*pp1stVOPEndPos = NULL;
		return 2;
	}else{
		*pp1stVOPEndPos = p1stVOPEnd;
		return rt;
	}

}


static int Read_SeqHdrOrFrame(unsigned char** ppFragdata, int* pFragdataLen, StoreInputFileDataBuf* buf, IPP_FILE* fp, int kind, int* pbAllFileDataLoaded)
{
	unsigned char* pBegin, *pEnd;
	int query_rt;
	int readfile_rt;
	int (*pfun_query)(unsigned char*, int, unsigned char**, unsigned char**, unsigned char**);
	unsigned char* pMistakeStopPos;

	switch(kind) {
	case 0://query h264 frame
		pfun_query = query_h264Oneframe;
		break;
	case 1://query h264 seq header and 1st frame
		pfun_query = query_h264SeqHdr1stFrame;
		break;
	case 2://query vc1spmp frame
		pfun_query = query_vc1SPMPOneframe;
		break;
	case 3://query vc1spmp seq header and 1st frame
		pfun_query = query_vc1SPMPSeqHdr1stFrame;
		break;
	case 4://query h263 frame
	case 5:
		pfun_query = query_h263Oneframe;
		break;
	case 6://query mpeg2 frame
		pfun_query = query_mpeg2Oneframe;
		break;
	case 7://query mpeg2 seq header and 1st frame
		pfun_query = query_mpeg2SeqHdr1stFrame;
		break;
	case 8://query mpeg4 VOP
		pfun_query = query_mpeg4OneVOP;
		break;
	case 9://query mpeg4 seq header and 1st VOP
		pfun_query = query_mpeg4SeqHdr1stVOP;
		break;
	default:
		return -20;
	}

	for(;;) {
		pMistakeStopPos = NULL;
		query_rt = pfun_query(buf->pBsCur, buf->datalen-(buf->pBsCur-buf->pBuf), &pBegin, &pEnd, &pMistakeStopPos);
		if(query_rt == 0) {
			if((kind&1) == 0) {
				//for frame
				//consider all nals like SEI before this frame are belong to this frame
				//consider all nals like SEI after this frame are belong to next frame
				*ppFragdata = buf->pBsCur;
			}else{
				//for seq header
				//ignore all data before seq header
				*ppFragdata = pBegin;
			}
			*pFragdataLen = pEnd - *ppFragdata;
			buf->pBsCur = pEnd;
			return 0;
		}

		if(pMistakeStopPos) {
			//flush those mistake stream data, otherwise, pfun_query() will always stop at those mistake stream data
			IPP_Printf("Met mistake stream data, flush those mistake data fragment !!!\n");
			buf->pBsCur = pMistakeStopPos;
			Repack_FileBuf(buf);
			continue;
		}

		if(*pbAllFileDataLoaded) {
			*ppFragdata = buf->pBsCur;
			*pFragdataLen = buf->datalen - (*ppFragdata-buf->pBuf);
			buf->pBsCur = buf->pBuf + buf->datalen;
			if(query_rt == 1) {
				//only found the frame slice begin, because of at the end of file, no next frame begin
				return 1;
			}else if(query_rt == 2) {
				//not found the frame slice begin, therefore, the left data hasn't contain a frame
				//but still fill ppFragdata and pFragdataLen
				return 2;
			}
		}

		//continue load data
		readfile_rt = Prepare_FileBufData(buf, fp);
		if(readfile_rt == -1) {
			//expand the buffer
			//unlimited expanding the buffer is dangerous, therefore add this limit
			//it's only for error stream case, for correct stream, one frame size won't exceeds FILEBUF_SZMAX
#define FILEBUF_SZMAX   	(8*1024*1024)   //8M
			if(buf->bufSz < FILEBUF_SZMAX) {
				//expand buffer
				if( Expand_FileBuf(buf, buf->bufSz+(buf->bufSz>>1)) != 0 ) {
					//no memory error
					return -1;
				}
			}else{
				//give up current data
				buf->pBsCur = buf->pBuf + buf->datalen;
			}
			//load data again
			Prepare_FileBufData(buf, fp);
		}

		if(buf->datalen < buf->bufSz) {
			*pbAllFileDataLoaded = 1;
		}
	}

	return -2;
}

int load_rawh264_seqhdr1stframe(unsigned char** ppSeqHdr, int* pFragLen, StoreInputFileDataBuf* buf, IPP_FILE* fp, int* pbAllFileDataLoaded, void* pOther)
{
	int rt = Read_SeqHdrOrFrame(ppSeqHdr, pFragLen, buf, fp, 1, pbAllFileDataLoaded);
	if(rt<0 || rt == 2) {
		*ppSeqHdr = NULL;
		*pFragLen = 0;
		if(rt == 2) {
			//if no found sequence header and frame start header, consider loading fatal fail
			return -10;
		}
	}
	return rt;

}

int load_rawh264_nextframe(unsigned char** ppFrame, int* pFrameLen, StoreInputFileDataBuf* buf, IPP_FILE* fp, int* pbAllFileDataLoaded, void* pOther)
{
	int rt = Read_SeqHdrOrFrame(ppFrame, pFrameLen, buf, fp, 0, pbAllFileDataLoaded);
	if(rt<0) {
		*ppFrame = NULL;
		*pFrameLen = 0;
	}
	return rt;
}

int load_rcv2VC1SPMP_seqhdr1stframe(unsigned char** ppSeqHdr, int* pFragLen, StoreInputFileDataBuf* buf, IPP_FILE* fp, int* pbAllFileDataLoaded, void* pOther)
{
	int rt = Read_SeqHdrOrFrame(ppSeqHdr, pFragLen, buf, fp, 3, pbAllFileDataLoaded);
	if(rt<0 || rt == 2) {
		*ppSeqHdr = NULL;
		*pFragLen = 0;
		if(rt == 2) {
			//if no found sequence header and frame start header, consider loading fatal fail
			return -10;
		}
	}
	return rt;
}

int load_rcv2VC1SPMP_nextframe(unsigned char** ppFrame, int* pFrameLen, StoreInputFileDataBuf* buf, IPP_FILE* fp, int* pbAllFileDataLoaded, void* pOther)
{
	int rt = Read_SeqHdrOrFrame(ppFrame, pFrameLen, buf, fp, 2, pbAllFileDataLoaded);
	if(rt<0) {
		*ppFrame = NULL;
		*pFrameLen = 0;
	}
	return rt;
}


int load_rawh263_1stframe(unsigned char** ppSeqHdr, int* pFragLen, StoreInputFileDataBuf* buf, IPP_FILE* fp, int* pbAllFileDataLoaded, void* pOther)
{
	int rt = Read_SeqHdrOrFrame(ppSeqHdr, pFragLen, buf, fp, 5, pbAllFileDataLoaded);
	if(rt<0 || rt == 2) {
		*ppSeqHdr = NULL;
		*pFragLen = 0;
		if(rt == 2) {
			//if no found sequence header and frame start header, consider loading fatal fail
			return -10;
		}
	}
	return rt;
}

int load_rawh263_nextframe(unsigned char** ppFrame, int* pFrameLen, StoreInputFileDataBuf* buf, IPP_FILE* fp, int* pbAllFileDataLoaded, void* pOther)
{
	int rt = Read_SeqHdrOrFrame(ppFrame, pFrameLen, buf, fp, 4, pbAllFileDataLoaded);
	if(rt<0) {
		*ppFrame = NULL;
		*pFrameLen = 0;
	}
	return rt;
}


int load_rawMpeg2_seqhdr1stframe(unsigned char** ppSeqHdr, int* pFragLen, StoreInputFileDataBuf* buf, IPP_FILE* fp, int* pbAllFileDataLoaded, void* pOther)
{
	int rt = Read_SeqHdrOrFrame(ppSeqHdr, pFragLen, buf, fp, 7, pbAllFileDataLoaded);
	if(rt<0 || rt == 2) {
		*ppSeqHdr = NULL;
		*pFragLen = 0;
		if(rt == 2) {
			//if no found sequence header and frame start header, consider loading fatal fail
			return -10;
		}
	}
	return rt;
}

int load_rawMpeg2_nextframe(unsigned char** ppFrame, int* pFrameLen, StoreInputFileDataBuf* buf, IPP_FILE* fp, int* pbAllFileDataLoaded, void* pOther)
{
	int rt = Read_SeqHdrOrFrame(ppFrame, pFrameLen, buf, fp, 6, pbAllFileDataLoaded);
	if(rt<0) {
		*ppFrame = NULL;
		*pFrameLen = 0;
	}
	return rt;
}

int load_rawMpeg4_seqhdr1stframe(unsigned char** ppSeqHdr, int* pFragLen, StoreInputFileDataBuf* buf, IPP_FILE* fp, int* pbAllFileDataLoaded, void* pOther)
{
	int rt = Read_SeqHdrOrFrame(ppSeqHdr, pFragLen, buf, fp, 9, pbAllFileDataLoaded);
	if(rt<0 || rt == 2) {
		*ppSeqHdr = NULL;
		*pFragLen = 0;
		if(rt == 2) {
			//if no found sequence header and frame start header, consider loading fatal fail
			return -10;
		}
	}
	return rt;
}

int load_rawMpeg4_nextframe(unsigned char** ppFrame, int* pFrameLen, StoreInputFileDataBuf* buf, IPP_FILE* fp, int* pbAllFileDataLoaded, void* pOther)
{
	int rt = Read_SeqHdrOrFrame(ppFrame, pFrameLen, buf, fp, 8, pbAllFileDataLoaded);
	if(rt<0) {
		*ppFrame = NULL;
		*pFrameLen = 0;
	}
	return rt;
}


int load_fragment_bylengthfile(unsigned char** ppFrag, int* pFragLen, StoreInputFileDataBuf* buf, IPP_FILE* fp, int* pbAllFileDataLoaded, void* pOther)
{
	IPP_FILE* lenfp = (IPP_FILE*)pOther;
	int fraglen;
	int read;
	int expand_rt;
	if(IPP_Feof(lenfp)) {
		//if no more fragment length, consider already load all file data
		*pbAllFileDataLoaded = 1;
		*ppFrag = NULL;
		*pFragLen = 0;
		return 2;
	}
	fraglen = -1;
	IPP_Fscanf(lenfp, "%d", &fraglen);
	if(fraglen == -1) {
		//if no more fragment length, consider already load all file data
		*pbAllFileDataLoaded = 1;
		*ppFrag = NULL;
		*pFragLen = 0;
		return 2;
	}

	if(buf->bufSz < fraglen) {
#define FILEBUF_SZMAX_FORLENLOAD	FILEBUF_SZMAX
//#define FILEBUF_SZMAX_FORLENLOAD  (10*1024*1024)
		if(fraglen < FILEBUF_SZMAX_FORLENLOAD) {
			expand_rt = Expand_FileBuf(buf, fraglen);
		}else{
			expand_rt = -2;
		}
		if(expand_rt == -1 || expand_rt == -2) {
			//no memory error
			//skip this fragment
			IPP_Fseek(fp, fraglen, IPP_SEEK_CUR);
			if(IPP_Feof(fp)) {
				*pbAllFileDataLoaded = 1;
			}
			*ppFrag = NULL;
			*pFragLen = 0;
			return expand_rt;
		}
	}

	read = ForceRead_FileBufData(buf, fp, fraglen);
	*ppFrag = buf->pBuf;
	*pFragLen = read;
	if(read < fraglen) {
		*pbAllFileDataLoaded = 1;
		return 1;
	}
	return 0;
}
