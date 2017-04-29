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

#include "codecDef.h"
#include "codecVC.h"
#include "misc.h"
#include "ippLV.h"

#define		DATA_BUFFER_SIZE		(1024 * 1024) /*1M*/

typedef struct _H264DecoderParSet {
    int NSCCheckDisable;
    int bOutputDelayDisable;
	int nFdbMode;
	IppSkipMode SkipMode;
	int bCustomFrameMalloc;
}H264DecoderParSet;


static char NALNameTbl[32][100] = {
    "Unspecified",
    "Coded slice of a non-IDR picture",
    "Coded slice data partition A",
    "Coded slice data partition B",
    "Coded slice data partition C",
    "Coded slice of an IDR picture",
    "Supplemental enhancement information (SEI)",
    "Sequence parameter set",
    "Picture parameter set",
    "Access unit delimiter",
    "End of sequence",
    "End of stream",
    "Filler data",
    "Sequence parameter set extension",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Coded slice of an auxiliary coded picture without partitioning",
    "Researved",
    "Researved",
    "Researved",
    "Researved",
    "Unspecified",
    "Unspecified",
    "Unspecified",
    "Unspecified",
    "Unspecified",
    "Unspecified",
    "Unspecified",
    "Unspecified"
};

/******************************************************************************
// Name:			 DisplayLibVersion
//
// Description:		 Display library build information on the text console
//
// Input Arguments:  None.
//
// Output Arguments: None				
//
// Returns:			 None				
*******************************************************************************/
void DisplayLibVersion()
{
	char libversion[128]={'\0'};
	IppCodecStatus ret;
	ret = GetLibVersion_H264DEC(libversion,sizeof(libversion));
	if(0 == ret){
		IPP_Printf("\n*****************************************************************\n");
	    IPP_Printf("This library is built from %s\n",libversion);
		IPP_Printf("*****************************************************************\n");
	}else{
		IPP_Printf("\n*****************************************************************\n");
		IPP_Printf("Can't find this library version information\n");
		IPP_Printf("*****************************************************************\n");
	}
}


/***************************************************************************
// Name:            OutputPicture_H264
// Description:     Output one picture to destination file
// Input Arguments:
//   pSrcPicture  - Pointer to the picture to be written into file
//   fpout        - Pointer to the output file struct
// Output Arguments:
//  None
// Returns:
//  None
*****************************************************************************/
void OutputPicture_H264(IppPicture *pSrcPicture, IPP_FILE *fpout) {
    
    int     i, step;
    Ipp8u   *pDstY, *pDstU, *pDstV;

    //return;
    if (!fpout){
        return;
    }

    pDstY = pSrcPicture->ppPicPlane[0];
    pDstU = pSrcPicture->ppPicPlane[1];
    pDstV = pSrcPicture->ppPicPlane[2];
    
    /* Y plane */
    step = pSrcPicture->picPlaneStep[0];
	pDstY = pDstY + step * pSrcPicture->picROI.y + pSrcPicture->picROI.x;
	for (i = 0; i<pSrcPicture->picROI.height; i++) {
		IPP_Fwrite(pDstY, sizeof(char), pSrcPicture->picROI.width, fpout);
        pDstY += step;
    }
    /* U plane */
    step = pSrcPicture->picPlaneStep[1];
	pDstU = pDstU + step * (pSrcPicture->picROI.y >> 1) + (pSrcPicture->picROI.x >> 1);
	for (i = 0; i<(pSrcPicture->picROI.height>>1); i++) {
		IPP_Fwrite(pDstU, sizeof(char), pSrcPicture->picROI.width>>1, fpout);
		pDstU += step;
	}
    /* V plane */
    step = pSrcPicture->picPlaneStep[2];
	pDstV = pDstV + step * (pSrcPicture->picROI.y >> 1) + (pSrcPicture->picROI.x >> 1);
	for (i = 0; i<(pSrcPicture->picROI.height>>1); i++) {
		IPP_Fwrite(pDstV, sizeof(char), pSrcPicture->picROI.width>>1, fpout);
		pDstV += step;
	}
}

/***************************************************************************
// Name:            InsertSyncCode_H264
// Description:     Insert to sync code (001) into source stream to ensure
//                  one complete NAL Unit in stream buffer.
// Input Arguments:
//  ppSrcBitStream- Pointer to input IppBitstream struct
// Output Arguments:
//  ppSrcBitStream- Pointer to updated IppBitstream struct
// Returns:
//  None
*****************************************************************************/
void InsertSyncCode_H264(IppBitstream *pSrcDstStream)
{
    /* video buffer was allocated extra 3 bytes to ensure it will not
     * overflow when inserting start_code_prefix_one_3bytes */
    pSrcDstStream->pBsBuffer[pSrcDstStream->bsByteLen + 0] = 0x00;
    pSrcDstStream->pBsBuffer[pSrcDstStream->bsByteLen + 1] = 0x00;
    pSrcDstStream->pBsBuffer[pSrcDstStream->bsByteLen + 2] = 0x01;

    pSrcDstStream->bsByteLen += 3;
}

/***********************************************************
// Name:			ReadOneNAL_H264
// Description:		read one nal unit from input stream file
//
// Input Parameters:
//		f   	    input stream file
// Output Parameters:
//      fStream     pointer to destination stream structure
//
// Note:
//
*************************************************************/
int ReadOneNAL_H264(IppBitstream *pStream, IPP_FILE *f)
{
    Ipp8u byte;
    Ipp32u code;
    int nFreeBytes;
    static int nBufferSize = DATA_BUFFER_SIZE;

    pStream->bsByteLen      = 0;
    pStream->bsCurBitOffset = 0;
    pStream->pBsCurByte     = pStream->pBsBuffer;

    /*search first start code*/
    code = 0xffffff;
    byte = 0;
    while (!IPP_Feof(f) && (0x000001 != code)){
        byte = IPP_Fgetc(f);
        code = (code << 8) | byte;
        code &= 0x00ffffff;
    }

    if (0x000001 != code) {
        return -1;
    }

    nFreeBytes = nBufferSize - (pStream->pBsCurByte - pStream->pBsBuffer);

    if (3 > nFreeBytes) {
        int nRemainData = pStream->pBsCurByte - pStream->pBsBuffer;
        pStream->pBsBuffer = (Ipp8u*)IPP_MemRealloc((void**)(&pStream->pBsBuffer), nBufferSize, nBufferSize + DATA_BUFFER_SIZE);
        if (NULL == pStream->pBsBuffer) {
            return -1;
        }
        pStream->pBsCurByte = pStream->pBsBuffer + nRemainData;
        nFreeBytes  += DATA_BUFFER_SIZE;
        nBufferSize += DATA_BUFFER_SIZE;
    }
#if 0
    /*optional: insert sync code*/
    *pStream->pBsCurByte++ = 0;
    *pStream->pBsCurByte++ = 0;
    *pStream->pBsCurByte++ = 1;
    nFreeBytes -= 3;
#endif

    /*search second start code*/
    code = 0xffffff;
    byte = 0;
    while (!IPP_Feof(f) && (0x000001 != code)){
        byte = IPP_Fgetc(f);
        code = (code << 8) | byte;
        code &= 0x00ffffff;
        if (0 >= nFreeBytes) {
            int nRemainData = pStream->pBsCurByte - pStream->pBsBuffer;
			int rtCode;
            rtCode = IPP_MemRealloc((void**)(&pStream->pBsBuffer), nBufferSize, nBufferSize + DATA_BUFFER_SIZE);
            if (NULL == pStream->pBsBuffer || rtCode != IPP_OK) {
                return -1;
            }
            pStream->pBsCurByte = pStream->pBsBuffer + nRemainData;
            nFreeBytes  += DATA_BUFFER_SIZE;
            nBufferSize += DATA_BUFFER_SIZE;
        }
        *pStream->pBsCurByte++ = byte;
        nFreeBytes--;
    }
    
    if (0x000001 != code) {
        pStream->bsByteLen     = pStream->pBsCurByte - pStream->pBsBuffer;
        pStream->pBsCurByte    = pStream->pBsBuffer;
        return 0;
    } else {
        pStream->bsByteLen     = pStream->pBsCurByte - pStream->pBsBuffer - 3;
        pStream->pBsCurByte    = pStream->pBsBuffer;
        IPP_Fseek(f, -3, IPP_SEEK_CUR);
        return 1;
    }
}


/***************************************************************************
// Name:             videoInitBuffer
// Description:      Initialize the input bitstream buffer for MPEG4 decoder
//
// Input Arguments:
//      pBufInfo  Pointer to decoder input bitstream buffer
//
// Output Arguments:
//      pBufInfo  Pointer to updated decoder bitstream buffer
//
// Returns:
//     IPP_STATUS_NOERR		--		No Error
*****************************************************************************/
IppCodecStatus videoInitBuffer (IppBitstream *pBufInfo)
{
    /*
    // Initialize IppBitstream
    // at least big enough to store 2 frame data for less reload 
    */
    IPP_MemMalloc((void**)(&pBufInfo->pBsBuffer), DATA_BUFFER_SIZE, 4);

    if (NULL == pBufInfo->pBsBuffer) {
        return IPP_STATUS_NOMEM_ERR;
    }

     if ( pBufInfo->pBsBuffer ) {
        IPP_Memset(pBufInfo->pBsBuffer, 0, DATA_BUFFER_SIZE);
    }

    /*
    //no read data at beginning
    //set current pointer to the end of buffer
    */

    pBufInfo->pBsCurByte = pBufInfo->pBsBuffer + DATA_BUFFER_SIZE;
    pBufInfo->bsCurBitOffset = 0;
    pBufInfo->bsByteLen = DATA_BUFFER_SIZE;

    return IPP_STATUS_NOERR;
}

/***************************************************************************
// Name:             videoReloadBuffer
// Description:      Reload the input bitstream buffer for MPEG4 decoder
//
// Input Arguments:
//      pBufInfo  Pointer to decoder input bitstream buffer
//		stream    Pointer to the input file stream buffer
//
// Output Arguments:
//      pBufInfo  Pointer to updated decoder bitstream buffer
//
// Returns:
//		IPP_STATUS_NOERR		--		No Error
//		IPP_STATUS_ERR			--      fread return 0  		
*****************************************************************************/
IppCodecStatus videoReloadBuffer (IppBitstream *pBufInfo, IPP_FILE *stream)
{
    int    offset;	
    int	remain_data;

    offset = pBufInfo->pBsCurByte - pBufInfo->pBsBuffer;
    remain_data = pBufInfo->bsByteLen - offset;

    if ( remain_data != 0 ) {
        IPP_Memmove( pBufInfo->pBsBuffer, pBufInfo->pBsCurByte, remain_data );
    }

    pBufInfo->bsByteLen = remain_data + IPP_Fread( pBufInfo->pBsBuffer + remain_data, 1,
        DATA_BUFFER_SIZE - remain_data, stream ); 

    if ( pBufInfo->bsByteLen == remain_data ) {
        return IPP_STATUS_ERR;
    }

    pBufInfo->pBsCurByte = pBufInfo->pBsBuffer;	

    return IPP_STATUS_NOERR;
}

/***************************************************************************
// Name:             videoFreeBuffer
// Description:      Free input bitstream buffer for MPEG4 decoder
//
// Input Arguments:
//      pBufInfo  Pointer to decoder input bitstream buffer
//
// Output Arguments:
//      pBufInfo  Pointer to updated decoder input bitstream buffer
//
// Returns:
//     IPP_STATUS_NOERR		--		No Error
*****************************************************************************/
IppCodecStatus videoFreeBuffer (IppBitstream *pBufInfo)
{
    if ( pBufInfo->pBsBuffer ) {
        IPP_MemFree((void**)(&pBufInfo->pBsBuffer));
        pBufInfo->pBsBuffer = NULL;
    }

    return IPP_STATUS_NOERR;
}

//bofang, following code is used to do external frame reorder
typedef struct {
	void * pData;
	int	nominalPoc;
} DecodedImage;

#define REORDERPOOL_SZ	17	//17 is enough
DecodedImage g_ReOrderPool[REORDERPOOL_SZ];
int	g_ItemInReorderPoolCnt = 0;
int	g_ReorderDelay = 0;

#define MAX_DISPLAY_LATENCY	8	//MAX_DISPLAY_LATENCY should less than FIRE_INCREASEBUF_CMD_MAX
#define DISPLAYQUEUE_SZ	50	//display_latency should less than DISPLAYQUEUE_SZ
IppPicture* g_DisplayQueue[DISPLAYQUEUE_SZ] = {0};
int g_ItemInDisQCnt = 0;

#define FIRE_INCREASEBUF_CMD_MAX	20	//the additional buffer should <= FIRE_INCREASEBUF_CMD_MAX
#if MAX_DISPLAY_LATENCY >= FIRE_INCREASEBUF_CMD_MAX
#error "MAX_DISPLAY_LATENCY should less than FIRE_INCREASEBUF_CMD_MAX"
#endif

int g_suc_fire_increasebuf_cmd_cnt = 0;

static short g_randArr[4];

short produce_random()
{
	static int firsttime = 0;
	short rand16s;
	if(firsttime == 0) {
		firsttime = 1;
		g_randArr[0] = 1;	//seed
		g_randArr[1] = 1;
		g_randArr[2] = 1;
		g_randArr[3] = 1;
	}

	rand16s = (short)((int)g_randArr[0] + (int)g_randArr[3]);
	if((rand16s & 32768L) != 0) {
		rand16s += 1;
	}
	g_randArr[3] = g_randArr[2];
	g_randArr[2] = g_randArr[1];
	g_randArr[1] = g_randArr[0];
	g_randArr[0] = rand16s;

	return rand16s;
}

int produce_random_latency()
{
	unsigned short rand16u = (unsigned short)produce_random();
	return 4 + rand16u % (MAX_DISPLAY_LATENCY-4);
	//return 0;
}

void* h264_rawdecoder_frameMalloc(int size, int alignment, void* pUsrData)
{
	void* ptr;
	if(IPP_MemMalloc(&ptr, size, alignment) != IPP_OK) {
		IPP_Printf("frameMalloc is called, ret addr %x, size %d, align %d, userdata %x\n", NULL, size, alignment, pUsrData);
		return NULL;
	}else{
		IPP_Printf("frameMalloc is called, ret addr %x, size %d, align %d, userdata %x\n", ptr, size, alignment, pUsrData);
		return ptr;
	}
}

void h264_rawdecoder_frameFree(void* pointer, void* pUsrData)
{
	IPP_Printf("frameFree is called, pointer %x, userdata %x\n", pointer, pUsrData);
	IPP_MemFree(&pointer);
	return;
}

#define POC_PROTECT_GAP	1000
void InsertFrame2Pool_inPocIncreaseOrder(void* newpData, int newpoc, int bRefreshNominalPoc)
{
	int i,j;

	if(bRefreshNominalPoc == 1) {
		if(g_ItemInReorderPoolCnt > 0) {
			//force all previous poc < pImage->nominalPoc - POC_PROTECT_GAP
			int delta = newpoc - (POC_PROTECT_GAP+1) - g_ReOrderPool[g_ItemInReorderPoolCnt-1].nominalPoc;
			for(i=0;i<g_ItemInReorderPoolCnt;i++) {
				g_ReOrderPool[i].nominalPoc += delta;
			}
		}
	}
	//search a right position
	for(i=0;i<g_ItemInReorderPoolCnt;i++) {
		if(newpoc < g_ReOrderPool[i].nominalPoc) {
			break;
		}
	}
	for(j=g_ItemInReorderPoolCnt;j>i;j--) {
		g_ReOrderPool[j].nominalPoc = g_ReOrderPool[j-1].nominalPoc;
		g_ReOrderPool[j].pData = g_ReOrderPool[j-1].pData;
	}
	g_ReOrderPool[i].pData = newpData;
	g_ReOrderPool[i].nominalPoc = newpoc;
	g_ItemInReorderPoolCnt++;

	return;
}

void render1frame(IppPicture* pPic, IPP_FILE* fpout, DISPLAY_CB* hDispCB, int* pframecnt)
{
	IPP_Printf("Display picture ... ... %d POC=%d\r", *pframecnt, pPic->picOrderCnt);
	(*pframecnt)++;
	OutputPicture_H264(pPic, fpout);
	display_frame(hDispCB, pPic);
	if(pPic->picStatus == -1 && pPic->picPlaneStep[0] == pPic->picROI.width) {
		//this IppPicture is a duplicated IppPicture
		IPP_MemFree(&pPic->ppPicPlane[0]);
		IPP_MemFree(&pPic);
	}else{
		//this IppPicture is raw decoder allocated IppPicture
		pPic->picStatus |= 1;
	}
	return;
}

void flush_display_queue(IPP_FILE* fpout, DISPLAY_CB* hDispCB, int* pframecnt, int display_latency)
{
	int i;
	//flush picture in display queue
	while(g_ItemInDisQCnt > display_latency) {
		render1frame(g_DisplayQueue[0], fpout, hDispCB, pframecnt);
		g_ItemInDisQCnt--;
		//update queue
		for(i=0;i<g_ItemInDisQCnt;i++) {
			g_DisplayQueue[i] = g_DisplayQueue[i+1];
		}
	}
	return;
}

void consume1frame_from_reorderpool_and_flush_displayQ(IPP_FILE* fpout, DISPLAY_CB* hDispCB, int* pframecnt, int display_latency)
{
	int i;
	if(g_ItemInReorderPoolCnt > 0) {
		//push the picture from reorder pool to display queue
		g_DisplayQueue[g_ItemInDisQCnt++] = (IppPicture*)g_ReOrderPool[0].pData;
		g_ItemInReorderPoolCnt--;
		for(i=0;i<g_ItemInReorderPoolCnt;i++) {
			g_ReOrderPool[i].nominalPoc = g_ReOrderPool[i+1].nominalPoc;
			g_ReOrderPool[i].pData = g_ReOrderPool[i+1].pData;
		}
		

		//refresh display queue, if some picture in the display queue has reached the display_latency, display it
		flush_display_queue(fpout, hDispCB, pframecnt, display_latency);
	}

	return;
}

int	whether_thisframe_inReorderPool(IppPicture* pPic)
{
	int i;
	for(i=0;i<g_ItemInReorderPoolCnt;i++) {
		if(pPic == g_ReOrderPool[i].pData) {
			break;
		}
	}
	if(i==g_ItemInReorderPoolCnt) {
		return 0;
	}else{
		return 1;
	}
}

int	whether_thisframe_inDispQ(IppPicture* pPic)
{
	int i;
	for(i=0;i<g_ItemInDisQCnt;i++) {
		if(pPic == g_DisplayQueue[i]) {
			break;
		}
	}
	if(i==g_ItemInDisQCnt) {
		return 0;
	}else{
		return 1;
	}
}

IppPicture* duplicate_IppPic(IppPicture* pPicSrc)
{
	int i;
	unsigned char* pSrcData;
	unsigned char* pData;
	IppPicture* pPicDst;
	int sz;
	int memopRet;
	sz = (pPicSrc->picROI.width * pPicSrc->picROI.height * 3) >> 1;
	memopRet = IPP_MemMalloc(&pData, sz, 4);
	if(IPP_FAIL == memopRet || NULL == pData) {
		return NULL;
	}
	memopRet = IPP_MemMalloc(&pPicDst, sizeof(IppPicture), 4);
	if(IPP_FAIL == memopRet || NULL == pPicDst) {
		IPP_MemFree(&pData);
		return NULL;
	}
	
	IPP_Memcpy(pPicDst, pPicSrc, sizeof(IppPicture));
	pPicDst->ppPicPlane[0] = pData;
	pPicDst->ppPicPlane[1] = pData + (pPicSrc->picROI.width * pPicSrc->picROI.height);
	pPicDst->ppPicPlane[2] = pData + ((pPicSrc->picROI.width * pPicSrc->picROI.height * 5)>>2);
	pPicDst->picWidth = pPicSrc->picROI.width;
	pPicDst->picHeight = pPicSrc->picROI.height;
	pPicDst->picPlaneStep[0] = pPicSrc->picROI.width;
	pPicDst->picPlaneStep[1] = pPicSrc->picROI.width>>1;
	pPicDst->picPlaneStep[2] = pPicSrc->picROI.width>>1;
	pPicDst->picROI.x = 0;
	pPicDst->picROI.y = 0;
	pPicDst->picROI.width = pPicSrc->picROI.width;
	pPicDst->picROI.height = pPicSrc->picROI.height;
	pPicDst->picStatus = -1;

	pSrcData = (unsigned char*)pPicSrc->ppPicPlane[0] + pPicSrc->picPlaneStep[0]*pPicSrc->picROI.y + pPicSrc->picROI.x;
	for(i=0;i<pPicSrc->picROI.height;i++) {
		IPP_Memcpy(pData, pSrcData, pPicSrc->picROI.width);
		pData += pPicSrc->picROI.width;
		pSrcData += pPicSrc->picPlaneStep[0];
		
	}
	pSrcData = (unsigned char*)pPicSrc->ppPicPlane[1] + pPicSrc->picPlaneStep[1]*(pPicSrc->picROI.y>>1) + (pPicSrc->picROI.x>>1);
	for(i=0;i<pPicSrc->picROI.height>>1;i++) {
		IPP_Memcpy(pData, pSrcData, pPicSrc->picROI.width>>1);
		pData += pPicSrc->picROI.width>>1;
		pSrcData += pPicSrc->picPlaneStep[1];
	}
	pSrcData = (unsigned char*)pPicSrc->ppPicPlane[2] + pPicSrc->picPlaneStep[2]*(pPicSrc->picROI.y>>1) + (pPicSrc->picROI.x>>1);
	for(i=0;i<pPicSrc->picROI.height>>1;i++) {
		IPP_Memcpy(pData, pSrcData, pPicSrc->picROI.width>>1);
		pData += pPicSrc->picROI.width>>1;
		pSrcData += pPicSrc->picPlaneStep[2];
	}

	return pPicDst;
}

int getReorderDelay(void* pH264DecoderState)
{
	int poctype, dpbsize;
	IppCodecStatus ret;
	ret = DecodeSendCmd_H264Video(IPPVC_GET_POCTYPE, NULL, &poctype, pH264DecoderState);
	if(ret != IPP_STATUS_NOERR) {
		poctype = -1;	
	}
	IPP_Printf("===========Poc type is %d\n", poctype);
	if(poctype == 2) {
		return 0;
	}
	ret = DecodeSendCmd_H264Video(IPPVC_GET_DPBSIZE, NULL, &dpbsize, pH264DecoderState);
	if(ret != IPP_STATUS_NOERR) {
		dpbsize = 16;	
	}
	IPP_Printf("===========dpbsize is %d\n", dpbsize);
	return dpbsize;
}

int forceflush_all_left_pictures_reorderPool_dispQ(int bOutputDelayDisable, IppH264PicList* pDstPicList, int* pnTotalFrames, IPP_FILE *fpout, DISPLAY_CB* hDispCB)
{
	if(0 == bOutputDelayDisable) {
		/* process the last several pictures of the video stream */    
		while (pDstPicList) {
			IPP_Printf("Display picture ... ... %d POC=%d\r", *pnTotalFrames, pDstPicList->pPic->picOrderCnt);
			(*pnTotalFrames)++;
			OutputPicture_H264(pDstPicList->pPic, fpout);
			display_frame(hDispCB, pDstPicList->pPic);
			pDstPicList->pPic->picStatus |= 1;
			pDstPicList = pDstPicList->pNextPic;
		}
	}else{
		while(g_ItemInReorderPoolCnt > 0) {
			consume1frame_from_reorderpool_and_flush_displayQ(fpout, hDispCB, pnTotalFrames, 0);
		}
		while(g_ItemInDisQCnt > 0) {
			flush_display_queue(fpout, hDispCB, pnTotalFrames, 0);
		}
	}
	return 0;
}


/***********************************************************
// Name:			H264Decoder
// Description:		real h.264 decode function.
//
// Input Parameters:
//		fpin	    input stream file
//      pH264DecParSet decoder parameter set
// Output Parameters:
//      fpout       output yuv file
//      fplog       log file
//
// Note:
//
*************************************************************/
int H264Decoder(IPP_FILE *fpin, IPP_FILE *fpout, char *log_file_name, H264DecoderParSet *pH264DecParSet)
{
    void                        *pH264DecoderState = NULL;
    IppBitstream                srcBitStream;
    MiscGeneralCallbackTable    SrcCBTable;
    MiscGeneralCallbackTable    *pSrcCBTable = NULL;
    IppH264PicList              *pDstPicList = NULL;
    IppCodecStatus              rtCode = IPP_STATUS_NOERR;    
    int                         bLastNALUnit = 0, bEndOfStream = 0;
    int                         nAvailFrames;
    int							NSCCheckDisable = 1;
	int                         bOutputDelayDisable = pH264DecParSet->bOutputDelayDisable;
	int							bCustomFrameMalloc = pH264DecParSet->bCustomFrameMalloc;
	int							bRawDecDelayPar;
    IppiSize                    picsize;     

    int                         nTotalFrames = 0;
    int                         nTotalTime = 0;
    int                         FrameType = 2;
    int                         rt;
    int                         bUsed;
    Ipp8u                       *pBsCurByteBackup;
    H264NALUnitType             nalUnitType;
    int                         perf_index;
    int                         rtFlag = IPP_OK;

    DISPLAY_CB                  DispCB;
    DISPLAY_CB                  *hDispCB = &DispCB;


    /*initialize*/
    nTotalFrames        = 0;
    nTotalTime          = 0;
    bLastNALUnit        = 0; 
    bEndOfStream        = 0;
    FrameType           = 2;
    rtCode              = IPP_STATUS_NOERR;
    pH264DecoderState   = NULL;
    pSrcCBTable         = NULL;
    pDstPicList         = NULL;

	

    IPP_GetPerfCounter(&perf_index, DEFAULT_TIMINGFUNC_START, DEFAULT_TIMINGFUNC_STOP);
    IPP_ResetPerfCounter(perf_index);
     
    rtCode = videoInitBuffer(&srcBitStream);
    if (IPP_STATUS_NOERR != rtCode) {
        rtFlag = IPP_FAIL;
        IPP_Log(log_file_name, "a", "error: no memory!\n");
        goto H264DecEnd;
    }
    
    pSrcCBTable             = &SrcCBTable;
    pSrcCBTable->fMemCalloc = IPP_MemCalloc;
    pSrcCBTable->fMemMalloc = IPP_MemMalloc;
    pSrcCBTable->fMemFree   = IPP_MemFree;

    /*decoder initialize*/
    rtCode = DecoderInitAlloc_H264Video(pSrcCBTable, &pH264DecoderState);
    if (IPP_STATUS_NOERR != rtCode) {
        rtFlag = IPP_FAIL;
        IPP_Log(log_file_name, "a", "error: decoder init fail, error code %d!\n", rtCode);
        goto H264DecEnd;
    } 

    /*
    NSCCheckDisable = 1: Input data in the unit of NAL
    NSCCheckDisable = 0: Input data in arbitrary size, but the format must be Byte stream format(ITU-T H.264 Annex B)
    */
    NSCCheckDisable = 1;
    rtCode = DecodeSendCmd_H264Video(IPPVC_SET_NSCCHECKDISABLE, (&NSCCheckDisable), NULL, pH264DecoderState);
    if (IPP_STATUS_NOERR != rtCode) {
        IPP_Log(log_file_name, "a", "error: DecodeSendCmd_H264Video(IPPVC_SET_NSCCHECKDISABLE) %d!\n", rtCode);
    }

    /*
    bRawDecDelayPar = 0: decoder handle display list and hold enough frames in internal frame buffer (need large memory)
    bRawDecDelayPar = 1: user handle display list, and decoder hold enough frames in internal frame buffer (need large memory)
    bRawDecDelayPar = 2: user handle display list, and decoder only hold necessary frames in internal frame buffer (need small memory)
    */
	bRawDecDelayPar = bOutputDelayDisable;

	rtCode = DecodeSendCmd_H264Video(IPPVC_SET_OUTPUTDELAYDISABLE, (&bRawDecDelayPar), NULL, pH264DecoderState);
    if (IPP_STATUS_NOERR != rtCode){
        IPP_Log(log_file_name, "a", "error: DecodeSendCmd_H264Video(IPPVC_SET_OUTPUTDELAYDISABLE) %d!\n", rtCode);
    }
	IPP_Printf("\nThe sample code buffer management method == %d, the raw decoder delay parameter == %d\n", bOutputDelayDisable, bRawDecDelayPar);

	if( 1 == bCustomFrameMalloc ) {
		FrameMemOpSet MemOpsObj;
		MemOpsObj.fMallocFrame = h264_rawdecoder_frameMalloc;
		MemOpsObj.fFreeFrame = h264_rawdecoder_frameFree;
		MemOpsObj.pUsrObj = NULL;
		IPP_Printf("Set customer defined frame malloc, usrdata %x\n", MemOpsObj.pUsrObj);
		rtCode = DecodeSendCmd_H264Video(IPPVC_SET_FRAMEMEMOP, &MemOpsObj, NULL, pH264DecoderState);
		if (IPP_STATUS_NOERR != rtCode){
			IPP_Log(log_file_name, "a", "error: DecodeSendCmd_H264Video(IPPVC_SET_FRAMEMEMOP) %d!\n", rtCode);
		}
	}

    if (!NSCCheckDisable) {
        videoReloadBuffer(&srcBitStream, fpin);
    } else {
        bUsed = 1;
    }

#if 1
	rtCode = DecodeSendCmd_H264Video(IPPVC_SET_SKIPMODE, (&pH264DecParSet->SkipMode), NULL, pH264DecoderState);
	if (IPP_STATUS_NOERR != rtCode){ 
		return IPP_STATUS_ERR;
	}	
#endif

    while (!bEndOfStream) {
        /*read a NAL*/
        if (NSCCheckDisable) {
            if (bUsed) {
                rt = ReadOneNAL_H264(&srcBitStream, fpin);
                if (-1 == rt) {
                    rtFlag = IPP_FAIL;
                    IPP_Log(log_file_name, "a", "error: can't read a NAL!\n");
                    goto H264DecEnd;
                } else if (0 == rt) {
                    bLastNALUnit = 1;
                }
            }
        }

        rtCode = DecodeSendCmd_H264Video(IPPVC_GET_NEXTPICTURETYPE, &srcBitStream, &nalUnitType, pH264DecoderState);
        IPP_Printf("next nal is %s\r", NALNameTbl[(int)nalUnitType]);

		/*decode a NAL*/
        pBsCurByteBackup = srcBitStream.pBsCurByte;
        
        IPP_StartPerfCounter(perf_index);
        rtCode = DecodeFrame_H264Video(&srcBitStream, &pDstPicList, pH264DecoderState, &nAvailFrames);
        IPP_StopPerfCounter(perf_index);

        if ((pBsCurByteBackup == srcBitStream.pBsCurByte) && (0 < srcBitStream.bsByteLen)) {
            bUsed = 0;
        } else {
            bUsed = 1;
        }

        if (bUsed) {
            if (1 == bLastNALUnit) {
                bEndOfStream = 1;
            }
        }

        if (IPP_STATUS_NEW_VIDEO_SEQ == rtCode){

			g_suc_fire_increasebuf_cmd_cnt = 0;

			//flush all left pictures for last video sequence
			forceflush_all_left_pictures_reorderPool_dispQ(bOutputDelayDisable, pDstPicList, &nTotalFrames, fpout, hDispCB);

			
			IPP_Printf("\nskip mode is %d\n", pH264DecParSet->SkipMode);
			DecodeSendCmd_H264Video(IPPVC_SET_SKIPMODE, (&pH264DecParSet->SkipMode), NULL, pH264DecoderState);;
			
			g_ReorderDelay = getReorderDelay(pH264DecoderState);
			IPP_Printf("\nThe reorder delay is %d\n", g_ReorderDelay);

            rtCode = DecodeSendCmd_H264Video(IPPVC_GET_PICSIZE, NULL, &picsize, pH264DecoderState);
            if (IPP_STATUS_NOERR == rtCode){
                IPP_Printf("\nthe pic size of the new sequence is: %d*%d\n", picsize.width, picsize.height);    
                display_open(hDispCB, picsize.width, picsize.height);
            } else {
                IPP_Printf("new sequence is not coming!");
            }
        } else if (IPP_STATUS_FRAME_COMPLETE == rtCode) {
			if(0 == bOutputDelayDisable) {
				for (nAvailFrames; nAvailFrames>0; nAvailFrames--){
					IPP_Printf("Display picture ... ... %d POC=%d\r", nTotalFrames, pDstPicList->pPic->picOrderCnt);
					nTotalFrames++;
					OutputPicture_H264(pDstPicList->pPic, fpout);
					display_frame(hDispCB, pDstPicList->pPic);
					pDstPicList->pPic->picStatus |= 1;
					pDstPicList = pDstPicList->pNextPic;
				}
			}else if(1 == bOutputDelayDisable) {	//external do frame re-order
				if(nAvailFrames > 0) {
					if((pDstPicList->pPic->picStatus & 0x100) != 0/*check whether it's the end of video sequence*/) {
						//it's the end of video sequence, and the new video sequence will be started.
						//There are some repeated frames in pDstPicList, because those frame isn't freed in reorder pool or display queue 
						//flush all pictures
						forceflush_all_left_pictures_reorderPool_dispQ(bOutputDelayDisable, pDstPicList, &nTotalFrames, fpout, hDispCB);
					}else{
						if(whether_thisframe_inReorderPool(pDstPicList->pPic) == 1 || whether_thisframe_inDispQ(pDstPicList->pPic) == 1 ) {
							IPP_Printf("\nError: h264dec return repeated IPP_STATUS_FRAME_COMPLETE when it's not the end of video sequence!\n");
							rtFlag = IPP_FAIL;
							goto H264DecEnd;
						}

						//a new decoded frame, put it into reorder pool
						for (nAvailFrames; nAvailFrames>0; nAvailFrames--){
							int bUpdataPOC = pDstPicList->pPic->picOrderCnt==0 ? 1 : 0;
							if(pDstPicList->pPic->picOrderCnt==0) {
								//IPP_Printf("poc is 0, it should be an IDR frame.\n");
							}
							InsertFrame2Pool_inPocIncreaseOrder(pDstPicList->pPic, pDstPicList->pPic->picOrderCnt, bUpdataPOC);
							pDstPicList = pDstPicList->pNextPic;

							if(g_ItemInReorderPoolCnt == g_ReorderDelay + 1) {
								//reach the reorder delay
								consume1frame_from_reorderpool_and_flush_displayQ(fpout, hDispCB, &nTotalFrames, produce_random_latency());
							}
						}

					}

				}
			}else if(2 == bOutputDelayDisable) {//external do frame re-order
				if(nAvailFrames != 1) {
					IPP_Printf("\nError: h264dec return IPP_STATUS_FRAME_COMPLETE when bOutputDelayDisable==2, and available frame cnt is %d\n", nAvailFrames);
					rtFlag = IPP_FAIL;
					goto H264DecEnd;
				}else{
					int bUpdataPOC = pDstPicList->pPic->picOrderCnt==0 ? 1 : 0;
					IppPicture* pNewPic = duplicate_IppPic(pDstPicList->pPic);
					pDstPicList->pPic->picStatus |= 1;
					if(pNewPic == NULL) {
						rtFlag = IPP_FAIL;
						IPP_Log(log_file_name, "a", "error: no memory!\n");
						goto H264DecEnd;
					}
					InsertFrame2Pool_inPocIncreaseOrder(pNewPic, pNewPic->picOrderCnt, bUpdataPOC);
					if(g_ItemInReorderPoolCnt == g_ReorderDelay + 1) {
						//reach the reorder delay, output 1 frame
						consume1frame_from_reorderpool_and_flush_displayQ(fpout, hDispCB, &nTotalFrames, 0);
					}
				}
			}
		} else if (IPP_STATUS_BUFFER_FULL == rtCode) {
			/*
            This return code indicates the internal frame buffer is full.  
            In this case, user needs to take below actions depending on their desires:
            1. Output pictures from display list to display buffer or other location (reference implemented below)
            2. stop decoding. 
            Note: taking no action will result in endless loop!
            */
            if (0 == bOutputDelayDisable){
                for (nAvailFrames; nAvailFrames>0; nAvailFrames--){
                    IPP_Printf("Display picture ... ... %d POC=%d\r", nTotalFrames, pDstPicList->pPic->picOrderCnt);
					nTotalFrames++;
                    OutputPicture_H264(pDstPicList->pPic, fpout);
                    display_frame(hDispCB, pDstPicList->pPic);
                    pDstPicList->pPic->picStatus |= 1;
                    pDstPicList = pDstPicList->pNextPic;
                }
            } else if(1 == bOutputDelayDisable) {
				IPP_Printf("\nDecoder return buffer full!\n");
				if( g_suc_fire_increasebuf_cmd_cnt < FIRE_INCREASEBUF_CMD_MAX ) {
					//increase additional buffer
					IppCodecStatus cmdrt;
					cmdrt = DecodeSendCmd_H264Video(IPPVC_MORE_FRAMEBUFFUER, NULL, NULL, pH264DecoderState);
					IPP_Printf("\nFire IPPVC_MORE_FRAMEBUFFUER cmd(has been successfully fired %d times), ret %d\n", g_suc_fire_increasebuf_cmd_cnt, cmdrt);
					if(IPP_STATUS_NOERR == cmdrt) {
						g_suc_fire_increasebuf_cmd_cnt++;
					}else{
						IPP_Log(log_file_name, "a", "error: DecodeSendCmd_H264Video(IPPVC_MORE_FRAMEBUFFUER) %d!\n", rtCode);
					}
				}else{
					IPP_Printf("\nError: h264dec return IPP_STATUS_BUFFER_FULL but we have fired too much commands(%d) to increase buffer\n", g_suc_fire_increasebuf_cmd_cnt);
					rtFlag = IPP_FAIL;
					goto H264DecEnd;
				}
			} else if(2 == bOutputDelayDisable) {
				IPP_Printf("\nError: h264dec return IPP_STATUS_BUFFER_FULL when bOutputDelayDisable==2\n");
				rtFlag = IPP_FAIL;
				goto H264DecEnd;
			} 
        } else if (IPP_STATUS_NOERR == rtCode) {
            /*need more data*/
        } else if (IPP_STATUS_SYNCNOTFOUND_ERR == rtCode) {
            if (IPP_Feof(fpin)) {
                bLastNALUnit = 1;
                InsertSyncCode_H264(&srcBitStream);
            } else {
                if (videoReloadBuffer(&srcBitStream, fpin)) {
                    IPP_Log(log_file_name, "a", "error: fail to fill one NAL unit in source buffer!\n");
                    bEndOfStream = 1;
                }
            }
        } else {
            if ((IPP_STATUS_NOTSUPPORTED_ERR == rtCode) || 
                (IPP_STATUS_BITSTREAM_ERR == rtCode) ||
                (IPP_STATUS_INPUT_ERR == rtCode)) {
                bUsed = 1;
                if (bUsed) {
                    if (1 == bLastNALUnit) {
                        bEndOfStream = 1;
                    }
                }
                IPP_Log(log_file_name, "a", "error: decoding error, error code %d!\n", rtCode);
            } else {
                /* error occurs */
                rtFlag = IPP_FAIL;
                IPP_Log(log_file_name, "a", "error: fails to display frame %d, error code %d!\n", nTotalFrames, rtCode);
                bEndOfStream = 1;
            }
        }
    } /*while (!bEndOfStream)*/
	
	forceflush_all_left_pictures_reorderPool_dispQ(bOutputDelayDisable, pDstPicList, &nTotalFrames, fpout, hDispCB);

    nTotalTime = (int)IPP_GetPerfData(perf_index);
    IPP_Printf("\nTotal Frame: %d, Total Time: %d(ms), FPS: %f\n", 
        nTotalFrames, nTotalTime/1000, (float)(1000.0 * 1000.0 * nTotalFrames / nTotalTime));

    g_Tot_Time[IPP_VIDEO_INDEX]     = nTotalTime;
    g_Frame_Num[IPP_VIDEO_INDEX]    = nTotalFrames;
H264DecEnd:
    rtCode = DecoderFree_H264Video(&pH264DecoderState);
    if (IPP_STATUS_NOERR != rtCode) {
        IPP_Log(log_file_name, "a", "error: decoder free fail, error code %d!\n", rtCode);
        rtFlag = IPP_FAIL;
    }
    videoFreeBuffer(&srcBitStream);

    IPP_FreePerfCounter(perf_index);
    display_close();

    return rtFlag;
}



/******************************************************************************
// Name:                ParseH264DecCmd
// Description:         Parse the user command 
//
// Input Arguments:
//      pCmdLine    :   Pointer to the input command line
//
// Output Arguments:
//      pSrcFileName:   Pointer to src file name
//      pDstFileName:   Pointer to dst file name
//      pLogFileName:   Pointer to log file name
//      pParSet     :   Pointer to codec parameter set
// Returns:
//        [Success]     IPP_OK
//        [Failure]     IPP_FAIL
******************************************************************************/
int ParseH264DecCmd(char *pCmdLine, 
                    char *pSrcFileName, 
                    char *pDstFileName, 
                    char *pLogFileName, 
                    void *pParSet)
{
#define MAX_PAR_NAME_LEN    1024
#define MAX_PAR_VALUE_LEN   2048
#define STRNCPY(dst, src, len) \
{\
    IPP_Strncpy((dst), (src), (len));\
    (dst)[(len)] = '\0';\
}
    char *pCur, *pEnd;
    char par_name[MAX_PAR_NAME_LEN];
    char par_value[MAX_PAR_VALUE_LEN];
    int  par_name_len;
    int  par_value_len;
    char *p1, *p2, *p3;

    pCur = pCmdLine;
    pEnd = pCmdLine + IPP_Strlen(pCmdLine);

	((H264DecoderParSet*)pParSet)->SkipMode = IPPVC_SKIPMODE_0;
    while((p1 = IPP_Strstr(pCur, "-"))){
        p2 = IPP_Strstr(p1, ":");
        if (NULL == p2) {
            return IPP_FAIL;
        }
        p3 = IPP_Strstr(p2, " "); /*one space*/
        if (NULL == p3) {
            p3 = pEnd;
        }

        par_name_len    = p2 - p1 - 1;
        par_value_len   = p3 - p2 - 1;

        if ((0 >= par_name_len)  || (MAX_PAR_NAME_LEN <= par_name_len) ||
            (0 >= par_value_len) || (MAX_PAR_VALUE_LEN <= par_value_len)) {
            return IPP_FAIL;
        }

        STRNCPY(par_name, p1 + 1, par_name_len);
        if ((0 == IPP_Strcmp(par_name, "i")) || (0 == IPP_Strcmp(par_name, "I"))) {
            /*input file*/
            STRNCPY(pSrcFileName, p2 + 1, par_value_len);
        } else if ((0 == IPP_Strcmp(par_name, "o")) || (0 == IPP_Strcmp(par_name, "O"))) {
            /*output file*/
            STRNCPY(pDstFileName, p2 + 1, par_value_len);
        } else if ((0 == IPP_Strcmp(par_name, "l")) || (0 == IPP_Strcmp(par_name, "L"))) {
            /*log file*/
            STRNCPY(pLogFileName, p2 + 1, par_value_len);
        } else if ((0 == IPP_Strcmp(par_name, "p")) || (0 == IPP_Strcmp(par_name, "P"))) {
            /*par file*/
            /*parse par file to fill pParSet*/
        } else if ((0 == IPP_Strcmp(par_name, "outdelaymode")) || (0 == IPP_Strcmp(par_name, "OUTDELAYMODE"))) {
			STRNCPY(par_value, p2 + 1, par_value_len);
			if (0 == IPP_Strcmp(par_value, "0")) {
				((H264DecoderParSet*)pParSet)->bOutputDelayDisable = 0;
				IPP_Printf("Set output delay mode 0, decoder internal re-order!\n");			
			} else if (0 == IPP_Strcmp(par_value, "1")) {
				((H264DecoderParSet*)pParSet)->bOutputDelayDisable = 1;
				IPP_Printf("Set output delay mode 1, decoder allocate many buffer, external re-order, simulate display latency!\n");			
			} else if (0 == IPP_Strcmp(par_value, "2")) {
				((H264DecoderParSet*)pParSet)->bOutputDelayDisable = 2;
				IPP_Printf("Set output delay mode 2, decoder allocate few buffer, external re-order!\n");			
			} else {
				IPP_Printf("input error: -outdelaymode:0/1/2 !\n");
				return IPP_FAIL;
			}
        } else if ((0 == IPP_Strcmp(par_name, "custfmalloc")) || (0 == IPP_Strcmp(par_name, "CUSTFMALLOC"))) {
			STRNCPY(par_value, p2 + 1, par_value_len);
			if (0 == IPP_Strcmp(par_value, "0")) {
				((H264DecoderParSet*)pParSet)->bCustomFrameMalloc = 0;
				IPP_Printf("Set custom frame malloc mode 0, using default frame mallocate function!\n");
			} else if (0 == IPP_Strcmp(par_value, "1")) {
				((H264DecoderParSet*)pParSet)->bCustomFrameMalloc = 1;
				IPP_Printf("Set custom frame malloc mode 1, using customer defined frame mallocate function!\n");
			} else {
				IPP_Printf("input error: -custfmalloc:0/1 !\n");
				return IPP_FAIL;
			}
        } else if ((0 == IPP_Strcmp(par_name, "skipmode")) || (0 == IPP_Strcmp(par_name, "SKIPMODE"))) {
			STRNCPY(par_value, p2 + 1, par_value_len);
			if (0 == IPP_Strcmp(par_value, "0")) {
				((H264DecoderParSet*)pParSet)->SkipMode = IPPVC_SKIPMODE_0;
				IPP_Printf("Set skip mode 0, Normal decoding!\n");			
			} else if (0 == IPP_Strcmp(par_value, "1")) {
				((H264DecoderParSet*)pParSet)->SkipMode = IPPVC_SKIPMODE_1;
				IPP_Printf("Set skip mode 1, Fast deblocking for all frames!\n");			
			} else if (0 == IPP_Strcmp(par_value, "2")) {
				((H264DecoderParSet*)pParSet)->SkipMode = IPPVC_SKIPMODE_2;
				IPP_Printf("Set skip mode 2, No deblocking for all frames!\n");			
			} else if (0 == IPP_Strcmp(par_value, "2.5")) {
				((H264DecoderParSet*)pParSet)->SkipMode = IPPVC_SKIPMODE_2_5;
				IPP_Printf("Set skip mode 2.5, skip 1/2 non-reference frames!\n");			
			} else if (0 == IPP_Strcmp(par_value, "3")) {
				((H264DecoderParSet*)pParSet)->SkipMode = IPPVC_SKIPMODE_3;
				IPP_Printf("Set skip mode 3, skip all non-reference frames!\n");			
			} else {
				IPP_Printf("input error: -skipmode:0/1/2.5/3 !\n");
				return IPP_FAIL;
			}
		} else {
            /*parse other parameters for encoder*/
        }

        pCur = p3;
    }

    return IPP_OK;
}

/*Interface for IPP sample code template*/
int CodecTest(int argc, char **argv)
{
    IPP_FILE *fpin = NULL, *fpout = NULL;
    char input_file_name[2048]  = {'\0'};
    char output_file_name[2048] = {'\0'};
    char log_file_name[2048]    = {'\0'};
    H264DecoderParSet    h264_dec_par_set;
    int rtFlag;

	h264_dec_par_set.SkipMode = IPPVC_SKIPMODE_0;
	h264_dec_par_set.bOutputDelayDisable = 0;	//default value
	h264_dec_par_set.bCustomFrameMalloc = 1;	//default value

    DisplayLibVersion();

    if (2 > argc) {
        IPP_Printf("Usage: h264dec.exe -i:input.cmp -o:output.yuv -l:dec.log!\n");
        return IPP_FAIL;
    } else if (2 == argc){
        /*for validation*/
        IPP_Printf("Input command line: %s\n", argv[1]);
        if (IPP_OK != ParseH264DecCmd(argv[1], input_file_name, output_file_name, log_file_name, &h264_dec_par_set)){
            IPP_Log(log_file_name, "w", 
                "command line is wrong! %s\nUsage: h264dec.exe -i:input.cmp -o:output.yuv -l:dec.log!\n", argv[1]);
            return IPP_FAIL;
        }
    } else {
        /*for internal debug*/
        IPP_Strcpy(input_file_name, argv[1]);
        IPP_Strcpy(output_file_name, argv[2]);
    }

    if (0 == IPP_Strcmp(input_file_name, "\0")) {
        IPP_Log(log_file_name, "a", "input file name is null!\n");
        return IPP_FAIL;
    } else {
        fpin = IPP_Fopen(input_file_name, "rb");
        if (!fpin) {
            IPP_Log(log_file_name, "a", "Fails to open file %s!\n", input_file_name);
            return IPP_FAIL;
        }
    }
    
    if (0 == IPP_Strcmp(output_file_name, "\0")) {
        IPP_Log(log_file_name, "a", "output file name is null!\n");
    } else {
        fpout = IPP_Fopen(output_file_name, "wb");
        if (!fpout) {
            IPP_Log(log_file_name, "a", "Fails to open file %s\n", output_file_name);
            return IPP_FAIL;
        }
    }

    IPP_Log(log_file_name, "a", "command line: %s\n", argv[1]);
    IPP_Log(log_file_name, "a", "input file  : %s\n", input_file_name);
    IPP_Log(log_file_name, "a", "output file : %s\n", output_file_name);
    IPP_Log(log_file_name, "a", "begin to decode\n");
    rtFlag = H264Decoder(fpin, fpout, log_file_name, &h264_dec_par_set);
    if (IPP_OK == rtFlag) {
        IPP_Log(log_file_name, "a", "everything is OK!\n");
    } else {
        IPP_Log(log_file_name, "a", "decoding fail!\n");
    }

    if (fpin) {
        IPP_Fclose(fpin);
    }

    if (fpout) {
        IPP_Fclose(fpout);
    }

    return rtFlag;
}

/* EOF */
