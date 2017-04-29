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
#include <string.h>		//include memcpy
#include "codecCNM.h"
#include "cnm_bridge.h"
#include "misc.h"
#include "readdata.h"
#include "ippLV.h"

#define IPPCMDEC_FILEBUF_SZ0				(400000)
#define IPPCMDEC_STATIC_STMBUF_SZ0  		(512*1024)
#define IPP_CMVPUDEC_STREAMBUF_ALIGNMENT	4
#define IPP_CMVPUDEC_FRAMEBUF_ALIGNMENT 	8

#define CMDLINE_PAR_EXPLANATION 	"<-fmt:n> <-i:infilename> [-o:outfilename] [-l:logfilename] [-reorderchoice:0|1|2] [-pollingmode:0|1|2] [-dynaclk:0|1] [-fileplay:1|0] [-multiinstancesync:0|1] [-rotmir_enable:0|1] [-rotdir:0|1|2|3] [-mirdir:0|1|2|3] [-lenfile:xxx] [-multifullexit:0|1] [-dispvc1repeat:1|0] [-postdbk:0|1] [-codecyuvfmt:0|1] [-reformvc1dmv:0|1]"

//NOTE: currently, IPP_Printf only support the string whose length < 1024
static char scHelpCodaDx8Dec[] = {
	"Marvell CNM decoder CodaDx8Dec Usage1: \n"
	"./App "CMDLINE_PAR_EXPLANATION"\n\n"
	"video format:"
	  "\t(__MPEG1_NoSupport   = 0)\n"
	"\t\t(__MPEG2_Support     = 1)\n"
	"\t\t(__MPEG4_Support     = 2)\n"
	"\t\t(__H261_NoSupport    = 3)\n"
	"\t\t(__H263_Support      = 4)\n"
	"\t\t(__H264_Support      = 5)\n"
	"\t\t(__VC1(MP&SP)_Support= 6)\n"
	"Marvell  CNM decoder CodaDx8Dec Usage2: \n"
	"./App \"<-fmt:n> <-i:infilename> [-o:outfilename] [-l:logfilename] [-rotmir_enable:0|1] [-reorderchoice:0|1|2] ...\"\n"
	"Sample1: ./App -fmt:5 -i:t.264 -o:my.yuv\n"
	"Sample2: ./App \"-i:t.mpeg2 -l:mylog.log -fmt:1 -o:out.yuv -rotmir_enable:1\"\n"
};

static char scformat[][8] = {
	"MPG1","MPG2","MPG4","H261","H263","H264","VC1","JPEG","MJPG","VP6"
};


typedef struct{
	int format;
	int rotmir_enable;
	int reorder_choice;
	int polling_mode;
	int dynaclk_enable;
	int fileplay_enable;
	int rot_dir;
	int mir_dir;
	char lenfilepath[256];
	int multiple_buffull_exit;
	int multi_instance_sync;
	int dispVC1repeatframe;
	int postdbk_enable;
	int codecyuvfmt;
	int reformVC1DMV;
}CMDecCmdLinePar;

/******************************************************************************
// Name:			 DisplayLibVersion
//
// Description: 	 Display library build information on the text console
//
// Input Arguments:  None.
//
// Output Arguments: None
//
// Returns: 		 None
*******************************************************************************/
void DisplayLibVersion()
{
	unsigned char libversion[128]={'\0'};
	IppCodecStatus ret;
	ret = GetLibVersion_CNMDEC(libversion,sizeof(libversion));
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


//Allocate physical continuous memory, return physical address and virtual address
//Application should notice the cacheable attribute of memory, if it's cacheable, probably application should call some cache flush function when host CPU read/write data in those memory, because CNM VPU only directly access memory.
PhyContMemInfo* os_construct_phycontbuf(int type, int sz, int align)
{
	int ret;
	PhyContMemInfo* MemHandle;
	if(IPP_OK != IPP_MemMalloc((void**)(&MemHandle), sizeof(PhyContMemInfo), 4)) {
		return NULL;
	}
	MemHandle->iSize = sz;
	MemHandle->iAlign = align;
	
	if(type == 0) {
		//allocate a stream buffer
		ret = cnm_os_api_pcontmem_alloc(MemHandle);
		if(ret != 0) {
			IPP_MemFree((void**)(&MemHandle));
			return NULL;
		}
	}else if(type == 1) {
		//allocate a frame buffer
		ret = cnm_os_api_pcontmem_alloc(MemHandle);
		if(ret != 0) {
			IPP_MemFree((void**)(&MemHandle));
			return NULL;
		}
	}else{
		IPP_Printf("the type %d is unknown in os_construct_phycontbuf()!\n");
		return NULL;
	}
	return MemHandle;
}

static __inline Ipp32u os_phyaddr2viraddr(Ipp32u phyaddr_base, Ipp32u viraddr_base, Ipp32u phyaddr_cur)
{
	return viraddr_base + (phyaddr_cur - phyaddr_base); //phyaddr_cur should >= phyaddr_base
}


int os_deconstruct_phycontbuf(PhyContMemInfo* MemHandle)
{
	if(MemHandle == NULL) {
		return -1;
	}
	cnm_os_api_pcontmem_free(MemHandle);
	IPP_MemFree((void**)(&MemHandle));
	return 0;
}

#define CMDEC_VPU_MAXFRAMEBUFCNT			32  	//cm decoder support at most 32 frame buffer

static short g_randArr[4];
short produce_random()
{
	static int firsttime = 0;
	short rand16s;
	if(firsttime == 0) {
		firsttime = 1;
		g_randArr[0] = 1;   //seed
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

#define ADDITIONAL_DECODEBUFF_CNT   		0   //to avoid IPP_STATUS_BUFFER_FULL, for some stream of h264, it need 3 frame buffer to decode, therefore, add 2 buffer could cover 3+2 display latency
#define MAX_DISPLAY_LATENCY 				5   //to simulate display delay in real system
int produce_random_latency()
{
	unsigned short rand16u = (unsigned short)produce_random();
	//return 3 + rand16u % (MAX_DISPLAY_LATENCY-3);
	return 0;
}


static void draw_1frame(unsigned char* pY, unsigned char* pU, unsigned char* pV, int w, int h, int strideY, int strideUV, IPP_FILE * fpout)
{
	int i;
#if 0
	{
		static int cnt = 0;
		cnt++;
		if(cnt > 30) {
			return;
		}
	}
#endif

	if(fpout == NULL){
		return;
	}

	/////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////
	// NOTE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// CM decoder only access the content in memory, it won't effect the cache's status.
	// Therefore, if the frame buffer is cacheable, before application or other software component read the data in frame buffer,
	// it should call some flush functions to ensure the content in memory could be updated to cache. Because generally speaking, those software
	// components retrive data from cache first.
	// Calling some flush function if necessary......

	/*Display Output Frame. YV12 Plane*/
	/* Y plane */
	for (i= 0; i<h; i++) {
		IPP_Fwrite(pY, sizeof(char), w, fpout);
		pY += strideY;
	}

	/* U plane */
	h >>= 1;
	w >>= 1;
	for (i= 0; i<h; i++) {
		IPP_Fwrite(pU, sizeof(char), w, fpout);
		pU += strideUV;
	}

	/* V plane */
	for (i= 0; i<h; i++) {
		IPP_Fwrite(pV, sizeof(char), w, fpout);
		pV += strideUV;
	}
	return;
}

//draw_1frame_uvinterleave() fwrite the UV interleave frame to plannar frame
static void draw_1frame_uvinterleave(unsigned char* pY, unsigned char* pUV, int w, int h, int stride, IPP_FILE * fpout)
{
	int i,j;
	unsigned char* UVPlannarBuf;
	unsigned char* pU, *pV;
	IPP_MemMalloc((void**)(&UVPlannarBuf), w*h>>1, 4);
	if(fpout == NULL){
		return;
	}
	/* Y plane */
	for (i= 0; i<h; i++) {
		IPP_Fwrite(pY, sizeof(unsigned char), w, fpout);
		pY += stride;
	}
	//UV deinterleave
	pU = UVPlannarBuf;
	pV = UVPlannarBuf + (w*h>>2);
	for (i=0; i<h>>1; i++) {
		for(j=0;j<w>>1;j++) {
			*pU++ = *pUV++;
			*pV++ = *pUV++;
		}
		pUV += stride - w;
	}
	IPP_Fwrite(UVPlannarBuf, sizeof(unsigned char), w*h>>1, fpout);
	IPP_MemFree((void**)(&UVPlannarBuf));
	return;
}

int copy_frame(IppCMVpuDecFrameAddrInfo* pDst, IppCMVpuDecFrameAddrInfo* pSrc, int iFrameStride, int iFrameHeight)
{
	memcpy(pDst->pUserData1, pSrc->pUserData1, (iFrameStride*iFrameHeight*3)>>1);
	return 0;
}


int free_framebuf_fordispskipframe(IppCMVpuDecFrameAddrInfo FrameArr[], int arrsz)
{
	int i;
	for(i=0;i<arrsz;i++) {
		if(FrameArr[i].pUserData0 != NULL) {
			os_deconstruct_phycontbuf(FrameArr[i].pUserData0);
			FrameArr[i].pUserData0 = (void*)0;
		}
	}
	return 0;
}

int select_framebuf_fordispskipframe(IppCMVpuDecFrameAddrInfo FrameArr[], int arrsz, int iFrameStride, int iFrameHeight)
{
	PhyContMemInfo* Mem;
	int i;
	for(i=0;i<arrsz;i++) {
		if(FrameArr[i].pUserData0 != NULL && ((PhyContMemInfo*)FrameArr[i].pUserData0)->pExtData0 == (void*)0) {
			((PhyContMemInfo*)FrameArr[i].pUserData0)->pExtData0 = (void*)1;	//1 means it was using
			return i;
		}
	}
	for(i=0;i<arrsz;i++) {
		if(FrameArr[i].pUserData0 == NULL) {
			Mem = os_construct_phycontbuf(1, (iFrameStride*iFrameHeight*3)>>1, IPP_CMVPUDEC_FRAMEBUF_ALIGNMENT);
			if(Mem == NULL) {
				IPP_Printf("Err: Allocate frame buffer fail for skip frame!\n");
				return -1;
			}
			//FrameArr[i].phyY/phyU/phyV is useless for DispSkipFrame, because this frame won't be registered to codec, only used by application.
			//Therefore, we do not assignment value to them.
			FrameArr[i].pUserData0 = (void*)Mem;
			FrameArr[i].pUserData1 = Mem->virAddr;
			FrameArr[i].pUserData2 = (void*)((unsigned int)Mem->virAddr + iFrameStride*iFrameHeight);
			FrameArr[i].pUserData3 = (void*)((unsigned int)Mem->virAddr + iFrameStride*iFrameHeight + (iFrameStride*iFrameHeight>>2));
			Mem->pExtData0 = (void*)1;	//1 means it was using
			return i;
		}
	}
	return -2;
}
int vc1skipframe_displayed(IppCMVpuDecFrameAddrInfo FrameArr[], int idx)
{
	((PhyContMemInfo*)FrameArr[idx].pUserData0)->pExtData0 = (void*)0;	//0 means it was't be used any more
	return 0;
}

int flush_display_queue(IPP_FILE* fpout, int display_latency, int* pItemInDisQCnt, IppPicture* DisplayQueue, int bUVinterleave)
{
	int i;
	IppCodecStatus  codecRet;
	//flush picture in display queue
	IppPicture* pPic;
	while(*pItemInDisQCnt > display_latency) {
		pPic = &DisplayQueue[0];

		if(bUVinterleave == 0) {
			draw_1frame(pPic->ppPicPlane[0], pPic->ppPicPlane[1], pPic->ppPicPlane[2], \
				pPic->picWidth, pPic->picHeight, pPic->picPlaneStep[0], pPic->picPlaneStep[0]>>1, fpout);
		}else{
			draw_1frame_uvinterleave(pPic->ppPicPlane[0], pPic->ppPicPlane[1], pPic->picWidth, pPic->picHeight, pPic->picPlaneStep[0], fpout);
		}
		if(pPic->ppPicPlane[3] == NULL) {
			//Inform CM decoder, this frame buffer is displayed, and the content in this frame buffer isn't needed by application any more, therefore, decoder could reuse it.
			codecRet = DecoderFrameDisplayed_CMVpu(pPic->ppPicPlane[4], pPic->picStatus);
			if(codecRet != IPP_STATUS_NOERR) {
				IPP_Printf("DecoderFrameDisplayed_CMVpu() fail, ret %d\n", codecRet);
				return -1;
			}
		}else if(pPic->ppPicPlane[3] == (void*)2) {
			vc1skipframe_displayed(pPic->ppPicPlane[4], pPic->picStatus);
		}
		(*pItemInDisQCnt)--;

		//update queue
		for(i=0;i<*pItemInDisQCnt;i++) {
			DisplayQueue[i] = DisplayQueue[i+1];
		}
	}
	return 0;
}


int consume1frame_and_flush_displayQ(int* pItemInDisQCnt, IppPicture* DisplayQueue, void* pDec, IPP_FILE* fpout, IppCMVpuDecFrameAddrInfo FrameArr[], int fidx, int iYStride, IppiRect* pROI, int display_latency, int bUVinterleave)
{
	int ret;
	int ItemInDisQCnt = *pItemInDisQCnt;
	//add a frame into display queue
	if(bUVinterleave == 0) {
		DisplayQueue[ItemInDisQCnt].ppPicPlane[0] = (unsigned char*)FrameArr[fidx].pUserData1 + pROI->y*iYStride + pROI->x;
		DisplayQueue[ItemInDisQCnt].ppPicPlane[1] = (unsigned char*)FrameArr[fidx].pUserData2 + (pROI->y>>1)*(iYStride>>1) + (pROI->x>>1);
		DisplayQueue[ItemInDisQCnt].ppPicPlane[2] = (unsigned char*)FrameArr[fidx].pUserData3 + (pROI->y>>1)*(iYStride>>1) + (pROI->x>>1);
	}else{
		DisplayQueue[ItemInDisQCnt].ppPicPlane[0] = (unsigned char*)FrameArr[fidx].pUserData1 + pROI->y*iYStride + pROI->x;
		DisplayQueue[ItemInDisQCnt].ppPicPlane[1] = (unsigned char*)FrameArr[fidx].pUserData2 + (pROI->y>>1)*(iYStride) + (pROI->x);
		DisplayQueue[ItemInDisQCnt].ppPicPlane[2] = (unsigned char*)0;//for UV interleave, ppPicPlane[2] useless
	}
	DisplayQueue[ItemInDisQCnt].picWidth = pROI->width;
	DisplayQueue[ItemInDisQCnt].picHeight = pROI->height;
	DisplayQueue[ItemInDisQCnt].picPlaneStep[0] = iYStride;
	if(pDec && pDec != (void*)1) {	//buffer from decoder registered frame buffer array
		DisplayQueue[ItemInDisQCnt].ppPicPlane[3] = NULL;   //use ppPicPlane[3] to indicate the frame type
		DisplayQueue[ItemInDisQCnt].ppPicPlane[4] = pDec;
		DisplayQueue[ItemInDisQCnt].picStatus = fidx;   	//need fidx information after drawn this frame
	}else if(pDec == (void*)1){		//buffer from addition frame buffer array for VC1 skip frame
		DisplayQueue[ItemInDisQCnt].ppPicPlane[3] = (void*)2;   //use ppPicPlane[3] to indicate the frame type
		DisplayQueue[ItemInDisQCnt].ppPicPlane[4] = (void*)FrameArr;
		DisplayQueue[ItemInDisQCnt].picStatus = fidx;
	}else{	//buffer from rotation mirror frame buffer arry
		DisplayQueue[ItemInDisQCnt].ppPicPlane[3] = (void*)1;   //use ppPicPlane[3] to indicate the frame type
		DisplayQueue[ItemInDisQCnt].ppPicPlane[4] = (void*)FrameArr;
	}

	ItemInDisQCnt++;
	*pItemInDisQCnt = ItemInDisQCnt;

	ret = flush_display_queue(fpout, display_latency, pItemInDisQCnt, DisplayQueue, bUVinterleave);

	return ret;
}


int force_flush_alldisplayframes(int* pItemInDisQCnt, IppPicture* DisplayQueue, IPP_FILE* fpout, int bUVinterleave)
{
	return flush_display_queue(fpout, 0, pItemInDisQCnt, DisplayQueue, bUVinterleave);
}


static int FillDataToRingStmBuf(void* pDecObj, Ipp32u phyStartAddrStaticBuf, Ipp32u WholeStaticBufSz, Ipp32u virStartAddrStaticBuf, unsigned char* pSrc, int iSrcDataLen)
{
	int EmptySpaceSz;
	Ipp32u phyAddrEmptySapce;
	IppCodecStatus codecRet;
	int len_willfill;
	int tmp;
	void* WrPtr;
	if(iSrcDataLen == 0) {
		return 0;
	}

	//NOTE: Because there is some pretect space left, even application hasn't fill any data into the static stream buffer, the size of waitforfillspace is still < buffer size of static stream buffer
	codecRet = DecoderGetStaticStmBufInfo_CMVpu(pDecObj, &phyAddrEmptySapce, &EmptySpaceSz, NULL);
	if(codecRet != IPP_STATUS_NOERR) {
		IPP_Printf("DecoderGetStaticStmBufInfo_CMVpu() fail, ret %d\n", codecRet);
		return -1;
	}
	len_willfill = iSrcDataLen < EmptySpaceSz ? iSrcDataLen : EmptySpaceSz;
	//////////////////////////////////////////////
	// NOTE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// for nonfileplay mode, the static stream buffer is a ring buffer. Therefore, when written data reach the end of buffer, application should write left data from the head of buffer again.
	WrPtr = (void*)os_phyaddr2viraddr(phyStartAddrStaticBuf, virStartAddrStaticBuf, phyAddrEmptySapce);
	tmp = WholeStaticBufSz-(phyAddrEmptySapce-phyStartAddrStaticBuf);
	if(len_willfill > tmp) {
		IPP_Memcpy(WrPtr, (void*)pSrc, tmp);
		IPP_Memcpy((void*)virStartAddrStaticBuf, (void*)(pSrc+tmp), len_willfill-tmp);
	}else{
		IPP_Memcpy(WrPtr, (void*)pSrc, len_willfill);
	}
	//////////////////////////////////////////////
	// NOTE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// CM decoder only access the content in memory, it won't effect the cache's status.
	// Therefore, if the stream buffer is cacheable, after application or other software component write the data in stream buffer,
	// it should call some cache flush functions to ensure the content in cache could be updated to memory. Because generally speaking, those software
	// components write data to cache first.
	codecRet = DecoderStaticStmBufDataFilled_CMVpu(pDecObj, len_willfill);
	if(codecRet != IPP_STATUS_NOERR) {
		IPP_Printf("DecoderStaticStmBufDataFilled_CMVpu() fail, ret %d\n", codecRet);
		return -2;
	}
	return len_willfill;
}

int FillDataToStmBuf_forNonFilePlay(void* pDecObj, Ipp32u phyStartAddrStaticBuf, Ipp32u WholeStaticBufSz, Ipp32u virStartAddrStaticBuf, unsigned char* pSrc, int iSrcDataLen)
{
	return FillDataToRingStmBuf(pDecObj, phyStartAddrStaticBuf, WholeStaticBufSz, virStartAddrStaticBuf, pSrc, iSrcDataLen);
}


static int FillDataToCommonStmBuf(Ipp32u virAddrDstbuf, unsigned char* pSrc, int iSrcDataLen)
{
	IPP_Memcpy((void*)virAddrDstbuf, pSrc, iSrcDataLen);
	//////////////////////////////////////////////
	// NOTE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// CM decoder only access the content in memory, it won't effect the cache's status.
	// Therefore, if the stream buffer is cacheable, after application or other software component write the data in stream buffer,
	// it should call some cache flush functions to ensure the content in cache could be updated to memory. Because generally speaking, those software
	// components write data to cache first.
	return iSrcDataLen;
}

int FillDataToStmBuf_forFilePlay(void* pDecObj, Ipp32u phyAddrBufStart, Ipp32u iStaticBufSz, Ipp32u virAddrBufStart, unsigned char* pSrc, int iSrcDataLen, int bForSeqHdrParing)
{
	if(bForSeqHdrParing) {
		return FillDataToRingStmBuf(pDecObj, phyAddrBufStart, iStaticBufSz, virAddrBufStart, pSrc, iSrcDataLen);
	}else{
		return FillDataToCommonStmBuf(virAddrBufStart, pSrc, iSrcDataLen);
	}
}





typedef struct
{
	int counter;
	int lastEmptySpaceLen;
	void* pDec;
}WaitFunUserData;

static WaitFunUserData g_waitFunData = {0};

//In DecodeFrame_CMVpu(), it check CM VPU state and call IppCMVpuDecInitPar.PollingModeWaitFun() repeatedly
//The logic is described as following:
// if DecodeFrame_CMVpu found VPU is idle, it return FRAME_COMPLETE.
// if WaitFun_forNonfileplay(&IppCMVpuDecInitPar.PollingModeUserData, bNoMoreData, ...) return 0,
// it continue check VPU state and call WaitFun_forNonfileplay().
// if WaitFun_forNonfileplay() return 1,
// DecodeFrame_CMVpu() return NEED_INPUT if bNoMoreData==0.
// if WaitFun_forNonfileplay() return -1,
// DecodeFrame_CMVpu() return fatal error and the decoding process should be terminated.
// Generally speaking, waitfun() return 0 means continue decoding loop, return 1 means VPU is hungry and cause DecodeFrame_CMVpu() return NEED_INPUT
int WaitFun_forNonfileplay(void* pUserData, int bNoMoreData, int reserved)
{
#define WAITFUN_CHECK_STMBUFINFO_INTERVAL   5
#define WAITFUN_CHECK_STMBUFINFO_THRESHOLD  512

	WaitFunUserData* pUsrD = (WaitFunUserData*)pUserData;
	int lastEmptyLen;
	IppCodecStatus  codecRet;
	int curEmptyLen;

	if(--pUsrD->counter > 0) {
		cnm_os_api_sleep(15);   //sleep some usecond
		return 0;   	//In DecodeFrame_CMVpu(), if WaitFun_forNonfileplay() return 0 and CM VPU still hasn't finish one frame decoding, it will continue decoding and call WaitFun_forNonfileplay() again.
	}else{
		pUsrD->counter = WAITFUN_CHECK_STMBUFINFO_INTERVAL; //reset counter
		if(bNoMoreData) {
			//NOTE: when no more data, once VPU decoded the last frame in static stream buffer, the *pPhyAddrVPUWillReadSpace will stay at the next 512 aligned address. Therefore, the *pPhyAddrWaitForFillSpace probably exceeds the *pPhyAddrWaitForFillSpace	and	DecoderGetStaticStmBufInfo_CMVpu()	returned	*pWaitForFillSpaceSz	are	mistake.
			//Consequently, when no more data, shouldn't call DecoderGetStaticStmBufInfo_CMVpu()
			return 0;
		}

		//check stream buffer consumption infomation
		//!!!!!!! DecoderGetStaticStmBufInfo_CMVpu is the unique decoder API could be called in WaitFun_forNonfileplay()
		//!!!!!!! Calling any other function will cause un-defined behavior
		codecRet = DecoderGetStaticStmBufInfo_CMVpu(pUsrD->pDec, NULL, &curEmptyLen, NULL);
		if(codecRet != IPP_STATUS_NOERR) {
			IPP_Printf("DecoderGetStaticStmBufInfo_CMVpu() fail, ret %d in WaitFun_forNonfileplay()", codecRet);
			return -1;  //In DecodeFrame_CMVpu(), if WaitFun_forNonfileplay() return -1, DecodeFrame_CMVpu() will return IPP_STATUS_ERR immediately.
		}

		lastEmptyLen = pUsrD->lastEmptySpaceLen;
		pUsrD->lastEmptySpaceLen = curEmptyLen;
		if( lastEmptyLen == -1 ) {
			return 0;   //continue waiting decoding finish
		}else{
			if(curEmptyLen - lastEmptyLen >= WAITFUN_CHECK_STMBUFINFO_THRESHOLD) {
				//means in last interval, CM decoder consumed some bytes, therefore, it probably still enough data in stream buffer.
				return 0;
			}else{
				//it's probably that there is not enough data in stream buffer, and the CM VPU probably is in hungry state
				//return 1 means wait function think VPU is hungry
				return 1;   //In DecodeFrame_CMVpu(), if WaitFun_forNonfileplay() return 1, DecodeFrame_CMVpu() probably return IPP_STATUS_NEED_INPUT
			}
		}
	}
}

int calculate_rotmir_layout(IppCMVpuDecParsedSeqInfo* pSeqInfo, int iRotDir, int iMirDir, int* pRotMired_stride, int* pRotMired_height, int* pRotMired_bufsz, int* pUoffset, int* pVoffset, IppiRect* pRotMired_roi)
{
	if(pSeqInfo->FrameAlignedWidth != pSeqInfo->FrameROI.width || pSeqInfo->FrameAlignedHeight != pSeqInfo->FrameROI.height || pSeqInfo->FrameROI.x != 0 || pSeqInfo->FrameROI.y != 0) {
		return -1;  //currently, only support ROI is the same as the encoded area
	}

	*pRotMired_bufsz = (pSeqInfo->FrameAlignedWidth*pSeqInfo->FrameAlignedHeight*3)>>1;
	*pUoffset = pSeqInfo->FrameAlignedWidth*pSeqInfo->FrameAlignedHeight;
	*pVoffset = *pUoffset + (*pUoffset>>2);

	if(iRotDir == 0 || iRotDir == 2) {
		*pRotMired_stride = pSeqInfo->FrameAlignedWidth;
		*pRotMired_roi = pSeqInfo->FrameROI;
		*pRotMired_height = pSeqInfo->FrameAlignedHeight;
	}else if(iRotDir == 1 || iRotDir == 3) {
		*pRotMired_stride = pSeqInfo->FrameAlignedHeight;
		*pRotMired_height = pSeqInfo->FrameAlignedWidth;
		pRotMired_roi->width = pSeqInfo->FrameAlignedHeight;
		pRotMired_roi->height = pSeqInfo->FrameAlignedWidth;
		pRotMired_roi->x = 0;
		pRotMired_roi->y = 0;
	}else{
		return -2;
	}

	return 0;
}

PhyContMemInfo* select_fileplay_stream_bufer(unsigned int wanted_len, PhyContMemInfo* poolArr[], unsigned int poolsz)
{
	unsigned int i;

//application could define NOT_USE_STATIC_BUF_AS_STMBUF_FILEPLAY or not as your like
//#define NOT_USE_STATIC_BUF_AS_STMBUF_FILEPLAY
#ifdef NOT_USE_STATIC_BUF_AS_STMBUF_FILEPLAY
	for(i=1;i<poolsz;i++) {
#else
	for(i=0;i<poolsz;i++) {
#endif
		if(poolArr[i] != NULL && (int)wanted_len <= (poolArr[i])->iSize) {
			return poolArr[i];
		}
	}
	IPP_Printf("No suitable stream buffer for file play, wanted len %d\n", wanted_len);
	//no suitable buffer found

	//find an idle node
	for(i=1;i<poolsz;i++) {
		if(poolArr[i] == NULL) {
			break;
		}
	}

	if(i == poolsz) {
		//no idle node found
		//free all buffer except the static buffer
		IPP_Printf("No suitable stream buffer for file play, wanted len %d, free all buffer\n", wanted_len);
		for(i=1;i<poolsz;i++) {	//begin from 1, the item 0 is for static stream buffer, shouldn't be freed
			if(poolArr[i] != NULL) {
				os_deconstruct_phycontbuf(poolArr[i]);
				poolArr[i] = NULL;
			}
		}
		i = 1;
	}


	poolArr[i] = os_construct_phycontbuf(0, wanted_len, IPP_CMVPUDEC_STREAMBUF_ALIGNMENT);
	return poolArr[i];
}


int vpuDec_setting_initConf(IppCMVpuDecInitPar *pPar, CMDecCmdLinePar* pCmdPar);
/***************************************************************************
// Name:			 CodaDx8Dec
// Description: 	 Decoding entry for CodaDx8 decoder, support: H264/BP,
//  				 MPEG4/SP, VC1/MP, H263/P3, MPEG2/MP.
//
// Input Arguments:
//  	inputCmpFile  Pointer to input file stream buffer
//  	  outYUVFile	  Pointer to output YUV file stream buffer.
//
// Output Arguments:
//  	  pFrameNumber  Overall decoded frame numbers
//
// Returns:
//  	  IPP Codec Status Code
*****************************************************************************/
#define SEQHEADER_ESTIMATE_MAXLEN   1000
#define IS_FRAME_IDX_VALID(idx) 			((idx)>=0 && (idx)<iFrameBufferCnt)
int CodaDx8Dec(IPP_FILE *inputCmpFile, IPP_FILE * outYUVFile, unsigned long *pFrameNumber, char *logfile, CMDecCmdLinePar* pLinePar)
{
	void*   					pTmp;

	//performance measure
	int 						iLog_FrameDecOutputted = 0;
	int 						perf_index;
	int							thread_perf_index;
	Ipp64s		   				i64Log_CodecTime; //in usecond
	Ipp64s		   				i64Log_CodecCpuTime; //in usecond
	int 						iLog_NeedInputInDecoding = 0, iLog_NeedInputOutDecoding = 0;

	int							iLog_VC1MP_DMVFrameCnt = 0;		//to log the VC1 frame which adopt DirectModeMV
	int							iLog_VC1_SkipFrameCnt = 0;		//to log the VC1 skip frame

	IppCodecStatus  			codecRet;
	int 						Ret;
	IppCMVpuDecInitPar  		InitPar = {0};
	void*   					pDecObj = NULL;
	IppCMVpuDecParsedSeqInfo	SeqInfo;
	StoreInputFileDataBuf   	InputFileData = {0};
	int 						iCMVpuFilledLen;
	int 						bSeqParsed = 0;
	IppCMVpuDecFrameAddrInfo*   DecFramesInfoArr = NULL;
	int 						iFrameBufferCnt = 0;
	int							DecFramesHealthInfo[CMDEC_VPU_MAXFRAMEBUFCNT] = {0};	//used to record the decoded frame healthy degree. cnm decoder could be registered CMDEC_VPU_MAXFRAMEBUFCNT decoder buffers at most.
	int 						i;
	int 						iFrameBufSz, iFrameStride;
	int 						bAllDataLoadedintoDec = 0, bFileEOF = 0, newRead, bDecodingNormalEnd = 0, DecoderFailFlag = 0;
	IppCMVpuDecOutputInfo   	DecOutInfo;
	int 						bCallDecAgain, bLastBufferFull;
	Ipp32u  					StaticStmBufVaddrBase;
	PhyContMemInfo*				StaticStmMemHandle = NULL;
	int 						iStaticStmBufSz;
	int 						lastFragmentAllFilledIntoVPU = 0;
	PhyContMemInfo*				Mem;

	IppPicture DisplayQueue[CMDEC_VPU_MAXFRAMEBUFCNT];
	int ItemInDisQCnt = 0;
	IppCMVpuDecFrameAddrInfo FrameBufsForDispSkip[CMDEC_VPU_MAXFRAMEBUFCNT] = {{0}};


	//for fileplay mode
	int 						b1stFrame_forFileplay = 1;
	unsigned char*  			pSeqHdr1stFrame, *pStmFrame;
	int 						iSeqHdrAnd1stFrameLen, iStmFrameLen;
	IppCMDecFilePlayStmBufInfo  FilePlay_StmBuf;
	int (*load_seqhdr1stframe)(unsigned char**, int*, StoreInputFileDataBuf*, IPP_FILE*, int*, void*);
	int (*load_nextframe)(unsigned char**, int*, StoreInputFileDataBuf*, IPP_FILE*, int*, void*);
	PhyContMemInfo*				fileplay_stmbuf_pool[5] = {0};
	IPP_FILE*   				len_fp = NULL;


	//rotation mirror
	int 						bRotMir = pLinePar->rotmir_enable;  //1: enable rotation mirror
	int 						iRotDir = pLinePar->rot_dir, iMirDir = pLinePar->mir_dir;   //iRotDir: only could be 0,1,2,3, correspond to 0, 90, 180, 270 degree, anti-clockwise direction.//iMirDir: only could be 0,1,2,3, correspond to no mirror, vertical	mirror(top	to	down),	horizontal	mirror,	horizontal-vertical	mirror.
	IppCMVpuDecFrameAddrInfo	RotMirFrames[2] = {{0}};
	int 						iRotMirFrameIdx = 0;
	IppiRect					RotMired_roi;
	int 						iRotMired_stride, iRotMired_height;

	int							bPostDBK = pLinePar->postdbk_enable;
	IppCMVpuDecFrameAddrInfo	PostDBKFrames[3] = {{0}};
	int 						iPostDBKFrameIdx = 0;

	int							bCodec420SPMode = pLinePar->codecyuvfmt;

	if(bPostDBK && bRotMir) {
		IPP_Printf("Err: Sample code not support both enable post-dbk and rotate-mirror currently.\n");
		IPP_Log(logfile, "a", "Err: Sample code not support both enable post-dbk and rotate-mirror currently.\n");
		return IPP_FAIL;
	}

	load_seqhdr1stframe = NULL;
	load_nextframe = NULL;

	IPP_Printf("command line parameters are:\n"
		"-fmt: %d\n"
		"-rotmir_enable: %d\n"
		"-reorderchoice: %d\n"
		"-pollingmode: %d\n"
		"-dynaclk: %d\n"
		"-fileplay: %d\n"
		"-rotdir: %d\n"
		"-mirdir: %d\n"
		"-lenfile: %s\n"
		"-multifullexit: %d\n"
		"-multiinstancesync: %d\n"
		"-dispvc1repeat: %d\n"
		"-postdbk: %d\n"
		"-codecyuvfmt: %d\n"
		"-reformvc1dmv: %d\n"
		"\n", pLinePar->format, pLinePar->rotmir_enable, pLinePar->reorder_choice, pLinePar->polling_mode, pLinePar->dynaclk_enable, pLinePar->fileplay_enable, pLinePar->rot_dir, pLinePar->mir_dir, pLinePar->lenfilepath[0]=='\0'?"<none>":pLinePar->lenfilepath, pLinePar->multiple_buffull_exit, pLinePar->multi_instance_sync, pLinePar->dispVC1repeatframe, pLinePar->postdbk_enable, pLinePar->codecyuvfmt, pLinePar->reformVC1DMV);

	if(pFrameNumber) {
		*pFrameNumber = 0;
	}

	if(pLinePar->lenfilepath[0]!='\0') {
		len_fp = IPP_Fopen(pLinePar->lenfilepath, "rt");
		if(len_fp == NULL) {
			IPP_Printf("Err: open fragment length file: %s fail!\n", pLinePar->lenfilepath);
			IPP_Log(logfile, "a", "Err: open fragment length file: %s fail!\n", pLinePar->lenfilepath);
			return IPP_FAIL;
		}
	}


	//set init configuration
	IPP_Printf("Setting init configuration...\n");
	if(vpuDec_setting_initConf(&InitPar, pLinePar)) {
		IPP_Log(logfile, "a", "Err: Set init configuration fail!\n");
                if (len_fp) {
                    IPP_Fclose(len_fp);
                }
		return IPP_FAIL;
	}

	if(InitPar.FilePlayEnable) {
		if(len_fp != NULL) {
			//fragment length file has the highest priority
			load_seqhdr1stframe = load_fragment_bylengthfile;
			load_nextframe = load_fragment_bylengthfile;
			IPP_Printf("Load data according to fragment length file<%s> is available!!!\n", pLinePar->lenfilepath);
		}else{
			switch(InitPar.VideoStreamFormat) {
			case IPP_VIDEO_STRM_FMT_H264:
				load_seqhdr1stframe = load_rawh264_seqhdr1stframe;
				load_nextframe = load_rawh264_nextframe;
				break;
			case IPP_VIDEO_STRM_FMT_VC1M:
				load_seqhdr1stframe = load_rcv2VC1SPMP_seqhdr1stframe;
				load_nextframe = load_rcv2VC1SPMP_nextframe;
				break;
			case IPP_VIDEO_STRM_FMT_H263:
				load_seqhdr1stframe = load_rawh263_1stframe;
				load_nextframe = load_rawh263_nextframe;
				break;
			case IPP_VIDEO_STRM_FMT_MPG2:
				load_seqhdr1stframe = load_rawMpeg2_seqhdr1stframe;
				load_nextframe = load_rawMpeg2_nextframe;
				break;
			case IPP_VIDEO_STRM_FMT_MPG4:
				load_seqhdr1stframe = load_rawMpeg4_seqhdr1stframe;
				load_nextframe = load_rawMpeg4_nextframe;
				break;
			default:
				IPP_Printf("Err: Unsupported stream format %d under fileplay mode!\n", InitPar.VideoStreamFormat);
				IPP_Log(logfile, "a", "Err: Unsupported stream format %d under fileplay mode!\n", InitPar.VideoStreamFormat);
				return IPP_FAIL;
			}
		}
	}

	//init InputFileData
	if(Init_FileBuf(&InputFileData, IPPCMDEC_FILEBUF_SZ0)) {
		IPP_Printf("Err: init file buffer fail!!!!\n");
		IPP_Log(logfile, "a", "Err: init file buffer fail!!!!\n");
		return IPP_FAIL;
	}

	//allocate static stream buffer
	iStaticStmBufSz = IPPCMDEC_STATIC_STMBUF_SZ0;
	if(InitPar.FilePlayEnable) {
		//read the sequence header and 1st frame
		IPP_Printf("Read the sequence header and 1st frame from file.\n");
		Ret = load_seqhdr1stframe(&pSeqHdr1stFrame, &iSeqHdrAnd1stFrameLen, &InputFileData, inputCmpFile, &bFileEOF, len_fp);
		IPP_Printf("Read the sequence header and 1st frame from file, len is %d\n", iSeqHdrAnd1stFrameLen);
		if(Ret<0) {
			IPP_Printf("Err: load sequence and 1st frame fail!!!\n");
			IPP_Log(logfile, "a", "Err: load sequence and 1st frame fail!!!\n");
			IPP_Printf("Free input file buffer.\n");
			Deinit_FileBuf(&InputFileData);
			return IPP_FAIL;
		}
		if(iStaticStmBufSz < iSeqHdrAnd1stFrameLen + IPPCMDEC_STATICSTMBUF_PROTECTSPACE) {
			//+IPPCMDEC_STATICSTMBUF_PROTECTSPACE is to avoid static stream buffer overflow, because for static stream buffer, even it's totally empty, its usable spare space is less than its buffer size
			iStaticStmBufSz = iSeqHdrAnd1stFrameLen + IPPCMDEC_STATICSTMBUF_PROTECTSPACE;
		}
	}
	iStaticStmBufSz = (iStaticStmBufSz + 1023) & (~1023);   //must 1024 align

	IPP_Printf("Allocating static stream buffer...\n");
	StaticStmMemHandle = os_construct_phycontbuf(0, iStaticStmBufSz, IPP_CMVPUDEC_STREAMBUF_ALIGNMENT);

	if(StaticStmMemHandle == NULL) {
		IPP_Printf("Err: Allocate static stream buffer fail!\n");
		IPP_Log(logfile, "a", "Err: Allocate static stream buffer fail!\n");
		IPP_Printf("Free input file buffer.\n");
		Deinit_FileBuf(&InputFileData);
		return IPP_FAIL;
	}

	InitPar.phyAddrStaticBitstreamBuf = StaticStmMemHandle->phyAddr;
	StaticStmBufVaddrBase = (Ipp32u)StaticStmMemHandle->virAddr;

	IPP_Printf("static stream buffer phyaddr %x, viraddr %x, size %d is allocated.\n", InitPar.phyAddrStaticBitstreamBuf, StaticStmBufVaddrBase, iStaticStmBufSz);
	InitPar.StaticBitstreamBufSize = iStaticStmBufSz;

	//static stream buffer act as the first item of fileplay_stmbuf_pool
	fileplay_stmbuf_pool[0] = StaticStmMemHandle;


	//init decoder
	IPP_Printf("Calling DecoderInitAlloc_CMVpu...\n");
	codecRet = DecoderInitAlloc_CMVpu(&InitPar, IPP_MemMalloc, IPP_MemFree, &pDecObj);
	IPP_Printf("DecoderInitAlloc_CMVpu() ret %d\n", codecRet);
	if(codecRet != IPP_STATUS_NOERR) {
		IPP_Printf("Err: DecoderInitAlloc_CMVpu() fail, ret %d!\n", codecRet);
		IPP_Log(logfile, "a", "Err: DecoderInitAlloc_CMVpu() fail, ret %d!\n", codecRet);

		IPP_Printf("Free input file buffer.\n");
		Deinit_FileBuf(&InputFileData);

		if(StaticStmMemHandle) {
			IPP_Printf("Free static stream buffers...\n");
			os_deconstruct_phycontbuf(StaticStmMemHandle);
		}
		return IPP_FAIL;
	}

	IPP_GetPerfCounter(&perf_index, DEFAULT_TIMINGFUNC_START, DEFAULT_TIMINGFUNC_STOP);
	IPP_ResetPerfCounter(perf_index);
	IPP_GetPerfCounter(&thread_perf_index, IPP_TimeGetThreadTime, IPP_TimeGetThreadTime);
	IPP_ResetPerfCounter(thread_perf_index);

	//parse sequence header
	if(InitPar.FilePlayEnable == 0) {
		int WanttoNewFillLen;
		//repeated loading the sequence header stream into static buffer, and parsing seq header
		while(bSeqParsed == 0){
			//load data from file
			IPP_Printf("read more data from file to parse seq header...\n");
			newRead = Prepare_FileBufData(&InputFileData, inputCmpFile);
			if(newRead == 0 && lastFragmentAllFilledIntoVPU == 1){
				break;
			}
			WanttoNewFillLen = InputFileData.datalen-(InputFileData.pBsCur-InputFileData.pBuf);
			IPP_Printf("load more data %d bytes to VPU to parse seq header...\n", WanttoNewFillLen);
			iCMVpuFilledLen = FillDataToStmBuf_forNonFilePlay(pDecObj, InitPar.phyAddrStaticBitstreamBuf, InitPar.StaticBitstreamBufSize, StaticStmBufVaddrBase, InputFileData.pBsCur, WanttoNewFillLen);
			if(iCMVpuFilledLen < 0) {
				IPP_Printf("Err: FillDataToStmBuf_forNonFilePlay() fail, ret %d\n", iCMVpuFilledLen);
				IPP_Log(logfile, "a", "Err: FillDataToStmBuf_forNonFilePlay() fail, ret %d\n", iCMVpuFilledLen);
				DecoderFailFlag = -11;
				goto CMDEC_DECODING_TERMINATED;
			}
			if(InputFileData.datalen-(InputFileData.pBsCur-InputFileData.pBuf) == iCMVpuFilledLen) {
				lastFragmentAllFilledIntoVPU = 1;
			}
			InputFileData.pBsCur += iCMVpuFilledLen;

			IPP_Printf("Calling DecoderParseSeqHeader_CMVpu...\n");
			codecRet = DecoderParseSeqHeader_CMVpu(pDecObj, &SeqInfo);
			if(codecRet == IPP_STATUS_NEED_INPUT) {
				//CM need at least 512 byte to parse sequence header, otherwise, it return IPP_STATUS_NEED_INPUT.
				//If the total file is less 512 bytes, could append zeros at the end of stream to meet 512 byte request.
				IPP_Printf("CM need at least 512 byte to parse sequence header, otherwise, it return IPP_STATUS_NEED_INPUT.\n");
			}else if(codecRet == IPP_STATUS_NOERR) {
				//seq header parse succeed
				bSeqParsed = 1;
				Repack_FileBuf(&InputFileData);
				IPP_Printf("!!!!!! DecoderParseSeqHeader_CMVpu() succeed !!!!!!\n");
				IPP_Printf("DecoderParseSeqHeader_CMVpu() ret ROI x_off %d, y_off %d, w %d, h %d, alinged w %d, h %d\n", SeqInfo.FrameROI.x, SeqInfo.FrameROI.y, SeqInfo.FrameROI.width, SeqInfo.FrameROI.height, SeqInfo.FrameAlignedWidth, SeqInfo.FrameAlignedHeight);
				//goto CMDEC_DECODING_TERMINATED;

			}else if(codecRet == IPP_STATUS_BUFOVERFLOW_ERR) {
				//there isn't enough data in bitstream
				IPP_Printf("parse sequence fail, probably not enough data in bitstream, reload data and try again\n");

				//If seq header data cross the last filled data fragment, it should rollback some data

#if IPPCMDEC_FILEBUF_SZ0 < SEQHEADER_ESTIMATE_MAXLEN || IPPCMDEC_STATIC_STMBUF_SZ0 < SEQHEADER_ESTIMATE_MAXLEN
#error "file read buffer or static stream buffer is too small!!!"
#endif
				InputFileData.pBsCur -= iCMVpuFilledLen > SEQHEADER_ESTIMATE_MAXLEN ? SEQHEADER_ESTIMATE_MAXLEN : iCMVpuFilledLen - 1;

			}else if(codecRet == IPP_STATUS_NOTSUPPORTED_ERR) {
				//not supported video stream, for example, H264 high profile stream
				IPP_Printf("Err: DecoderParseSeqHeader_CMVpu() fail, ret %d, not supportted stream\n", codecRet);
				IPP_Log(logfile, "a", "Err: DecoderParseSeqHeader_CMVpu() fail, ret %d, not supportted stream\n", codecRet);
				DecoderFailFlag = 1;
				goto CMDEC_DECODING_TERMINATED;
			}else{
				IPP_Printf("Err: DecoderParseSeqHeader_CMVpu() fatal fail, ret %d\n", codecRet);
				IPP_Log(logfile, "a", "Err: DecoderParseSeqHeader_CMVpu() fatal fail, ret %d\n", codecRet);
				DecoderFailFlag = -12;
				goto CMDEC_DECODING_TERMINATED;
			}
		}

	}else{//for file play mode

		//for fileplay mode, the sequence header and 1st frame data still need to be filled into static stream buffer.
		iCMVpuFilledLen = FillDataToStmBuf_forFilePlay(pDecObj, InitPar.phyAddrStaticBitstreamBuf, InitPar.StaticBitstreamBufSize, StaticStmBufVaddrBase, pSeqHdr1stFrame, iSeqHdrAnd1stFrameLen, 1);
		if(iCMVpuFilledLen != iSeqHdrAnd1stFrameLen) {
			DecoderFailFlag = 5;
			IPP_Printf("Err: the 1st frame is too big %d!!!\n", iSeqHdrAnd1stFrameLen);
			goto CMDEC_DECODING_TERMINATED;
		}
		if(iSeqHdrAnd1stFrameLen < 512 && InitPar.VideoStreamFormat != IPP_VIDEO_STRM_FMT_VC1M) {
			//CM need at least 512 byte to parse sequence header, otherwise, it return IPP_STATUS_NEED_INPUT.
			//Could append zero bytes at the end of 1st frame to make the data long enough. Not insert zero between sequence header and 1st frame, it's not safe. Should append zero behind of 1st frame.
			//Another equivalent solution is to fill 512 zeros before the real sequence data except VC1 SP&MP.
			//Because VC1 SP&MP spec. does not define stuff bytes, it's better not to fill zero. Application could fill 2nd frame, 3rd frame ....
			unsigned char zerofill[512] = {0};
			IPP_Printf("Append %d bytes zero to meet %d bytes seq hdr parsing limitation.\n", sizeof(zerofill) - iSeqHdrAnd1stFrameLen, sizeof(zerofill));
			FillDataToStmBuf_forFilePlay(pDecObj, InitPar.phyAddrStaticBitstreamBuf, InitPar.StaticBitstreamBufSize, StaticStmBufVaddrBase, zerofill, sizeof(zerofill) - iSeqHdrAnd1stFrameLen, 1);
		}

		IPP_Printf("Calling DecoderParseSeqHeader_CMVpu(fileplay mode)...\n");
		codecRet = DecoderParseSeqHeader_CMVpu(pDecObj, &SeqInfo);
		IPP_Printf("DecoderParseSeqHeader_CMVpu() ret %d\n", codecRet);
		if(codecRet == IPP_STATUS_NOERR) {
			//seq header parse succeed
			bSeqParsed = 1;
			IPP_Printf("!!!!!! DecoderParseSeqHeader_CMVpu() succeed !!!!!!\n");
			IPP_Printf("DecoderParseSeqHeader_CMVpu() ret ROI x_off %d, y_off %d, w %d, h %d, alinged w %d, h %d\n", SeqInfo.FrameROI.x, SeqInfo.FrameROI.y, SeqInfo.FrameROI.width, SeqInfo.FrameROI.height, SeqInfo.FrameAlignedWidth, SeqInfo.FrameAlignedHeight);

		}else if(codecRet == IPP_STATUS_NOTSUPPORTED_ERR) {
			//not supported video stream, for example, H264 high profile stream
			IPP_Printf("Err: DecoderParseSeqHeader_CMVpu() fail, ret %d, not supportted stream\n", codecRet);
			IPP_Log(logfile, "a", "Err: DecoderParseSeqHeader_CMVpu() fail, ret %d, not supportted stream\n", codecRet);
			DecoderFailFlag = 1;
			goto CMDEC_DECODING_TERMINATED;
		}else{
			IPP_Printf("Err: DecoderParseSeqHeader_CMVpu() fatal fail, ret %d\n", codecRet);
			IPP_Log(logfile, "a", "Err: DecoderParseSeqHeader_CMVpu() fatal fail, ret %d\n", codecRet);
			DecoderFailFlag = -13;
			goto CMDEC_DECODING_TERMINATED;
		}
	}

	if(bSeqParsed == 0) {
		IPP_Printf("Err: !!!!! Couldn't parse seq header !!!!!\n");
		IPP_Log(logfile, "a", "Err: !!!!! Couldn't parse seq header !!!!!\n");
		DecoderFailFlag = 2;
		goto CMDEC_DECODING_TERMINATED;
	}

	IPP_Printf("Seq header info parsed fps %d/%d, buf min cnt %d, reorder delay %d, buf num for next decoding %d\n", SeqInfo.FrameRateNumerator, SeqInfo.FrameRateDenominator, SeqInfo.MinFrameBufferCount, SeqInfo.MaxReorderDelay, SeqInfo.MaxNumOfFrameBufUsedForNextDecoded);
	//allocate frame buffer
	//at least, should allocate SeqInfo.MinFrameBufferCount frame buffers
	//at most, CMDEC_VPU_MAXFRAMEBUFCNT, for different stream format, it's different, but not exceed 32
	iFrameBufferCnt = SeqInfo.MinFrameBufferCount + ADDITIONAL_DECODEBUFF_CNT;
	if(iFrameBufferCnt > CMDEC_VPU_MAXFRAMEBUFCNT) {
		IPP_Printf("Err: Too many frame buffer(%d), CM decoder only support at most %d frame buffer!\n", iFrameBufferCnt, CMDEC_VPU_MAXFRAMEBUFCNT);
		DecoderFailFlag = -14;
		goto CMDEC_DECODING_TERMINATED;
	}

	IPP_MemMalloc(&pTmp, sizeof(IppCMVpuDecFrameAddrInfo)*iFrameBufferCnt, 4);
	DecFramesInfoArr = (IppCMVpuDecFrameAddrInfo*)pTmp;
	IPP_Memset(DecFramesInfoArr, 0, sizeof(IppCMVpuDecFrameAddrInfo)*iFrameBufferCnt);

	iFrameStride	= SeqInfo.FrameAlignedWidth;
	iFrameBufSz 	= (iFrameStride * SeqInfo.FrameAlignedHeight * 3)>>1;
	IPP_Printf("Application would allocate %d frame buffers, each size %d bytes!\n", iFrameBufferCnt, iFrameBufSz);
	IPP_Printf("Allocating frame buffer...\n");
	for(i=0; i<iFrameBufferCnt; i++) {
		Mem = os_construct_phycontbuf(1, iFrameBufSz, IPP_CMVPUDEC_FRAMEBUF_ALIGNMENT);
		if(Mem == NULL) {
			IPP_Printf("Err: Allocate frame buffer fail!\n");
			IPP_Log(logfile, "a", "Err: Allocate frame buffer fail!\n");
			DecoderFailFlag = -15;
			goto CMDEC_DECODING_TERMINATED;
		}

		DecFramesInfoArr[i].phyY = Mem->phyAddr;
		DecFramesInfoArr[i].phyU = Mem->phyAddr + iFrameStride*SeqInfo.FrameAlignedHeight;
		DecFramesInfoArr[i].phyV = Mem->phyAddr + iFrameStride*SeqInfo.FrameAlignedHeight + (iFrameStride*SeqInfo.FrameAlignedHeight>>2);
		DecFramesInfoArr[i].pUserData0 = (void*)Mem;
		DecFramesInfoArr[i].pUserData1 = Mem->virAddr;
		DecFramesInfoArr[i].pUserData2 = (void*)((unsigned int)Mem->virAddr + iFrameStride*SeqInfo.FrameAlignedHeight);
		DecFramesInfoArr[i].pUserData3 = (void*)((unsigned int)Mem->virAddr + iFrameStride*SeqInfo.FrameAlignedHeight + (iFrameStride*SeqInfo.FrameAlignedHeight>>2));
	}
	IPP_Printf("Calling DecoderRegisterFrameBufs_CMVpu...\n");
	codecRet = DecoderRegisterFrameBufs_CMVpu(pDecObj, DecFramesInfoArr, iFrameBufferCnt, iFrameStride);
	IPP_Printf("Call DecoderRegisterFrameBufs_CMVpu() ret %d\n", codecRet);
	if(codecRet != IPP_STATUS_NOERR) {
		IPP_Printf("Err: DecoderRegisterFrameBufs_CMVpu fail, ret %d\n", codecRet);
		IPP_Log(logfile, "a", "Err: DecoderRegisterFrameBufs_CMVpu fail, ret %d\n", codecRet);
		DecoderFailFlag = -16;
		goto CMDEC_DECODING_TERMINATED;
	}
	IPP_Printf("Calling DecoderRegisterFrameBufs_CMVpu suc finished.\n");

	if(bPostDBK) {
		if(SeqInfo.H263_annexJEnableReport == 0) {
			for(i=0;i<sizeof(PostDBKFrames)/sizeof(PostDBKFrames[0]);i++) {
				//for post-processing de-blocking buffer, its layout should be same as the buffer which are registered to decoder through DecoderRegisterFrameBufs_CMVpu()
				Mem = os_construct_phycontbuf(1, iFrameBufSz, IPP_CMVPUDEC_FRAMEBUF_ALIGNMENT);
				if(Mem == NULL) {
					IPP_Printf("Err: Allocate frame buffer for post-dbk fail!\n");
					IPP_Log(logfile, "a", "Err: Allocate frame buffer for post-dbk fail!\n");
					DecoderFailFlag = -15;
					goto CMDEC_DECODING_TERMINATED;
				}

				PostDBKFrames[i].phyY = Mem->phyAddr;
				PostDBKFrames[i].phyU = Mem->phyAddr + iFrameStride*SeqInfo.FrameAlignedHeight;
				PostDBKFrames[i].phyV = Mem->phyAddr + iFrameStride*SeqInfo.FrameAlignedHeight + (iFrameStride*SeqInfo.FrameAlignedHeight>>2);
				PostDBKFrames[i].pUserData0 = (void*)Mem;
				PostDBKFrames[i].pUserData1 = Mem->virAddr;
				PostDBKFrames[i].pUserData2 = (void*)((unsigned int)Mem->virAddr + iFrameStride*SeqInfo.FrameAlignedHeight);
				PostDBKFrames[i].pUserData3 = (void*)((unsigned int)Mem->virAddr + iFrameStride*SeqInfo.FrameAlignedHeight + (iFrameStride*SeqInfo.FrameAlignedHeight>>2));
			}
			IPP_Printf("Calling DecoderSetPostDBKBuf_CMVpu...\n");
			//only set 1 post-dbk buffer to codec
			codecRet = DecoderSetPostDBKBuf_CMVpu(pDecObj, &PostDBKFrames[0]);
			if(codecRet != IPP_STATUS_NOERR) {
				IPP_Printf("Err: DecoderSetPostDBKBuf_CMVpu fail, ret %d\n", codecRet);
				IPP_Log(logfile, "a", "Err: DecoderSetPostDBKBuf_CMVpu fail, ret %d\n", codecRet);
				DecoderFailFlag = -501;
				goto CMDEC_DECODING_TERMINATED;
			}
			IPP_Printf("Calling DecoderSetPostDBKBuf_CMVpu suc finished.\n");
		}else{
			//if annexJ in h263 is enabled, the DecFramesInfoArr stored content is de-blocked, decoder won't fill de-blocked content into post-processing de-blocking buffer
			bPostDBK = 0;
			IPP_Printf("annexJ in h263 is enabled, the DecFramesInfoArr stored content is de-blocked, decoder won't fill de-blocked content into post-processing de-blocking buffer.\n");
		}
	}

	if(bRotMir) {
		int RotMired_bufsz, Uoffset, Voffset;
		int ret = calculate_rotmir_layout(&SeqInfo, iRotDir, iMirDir, &iRotMired_stride, &iRotMired_height, &RotMired_bufsz, &Uoffset, &Voffset, &RotMired_roi);
		if(ret != 0) {
			IPP_Printf("Err: Not supported rotation mirror layout, application end\n");
			IPP_Log(logfile, "a", "Err: Not supported rotation mirror layout, application end\n");
			DecoderFailFlag = 3;
			goto CMDEC_DECODING_TERMINATED;
		}
		IPP_Printf("allocate rot mirror buffer, cnt %d, each size %d...\n", sizeof(RotMirFrames)/sizeof(RotMirFrames[0]), RotMired_bufsz);

		for(i=0;i<sizeof(RotMirFrames)/sizeof(RotMirFrames[0]);i++) {
			Mem = os_construct_phycontbuf(1, RotMired_bufsz, IPP_CMVPUDEC_FRAMEBUF_ALIGNMENT);
			if(Mem == NULL) {
				IPP_Printf("Err: Allocate rot mirror frame buffer fail!\n");
				IPP_Log(logfile, "a", "Err: Allocate rot mirror frame buffer fail!\n");
				DecoderFailFlag = -17;
				goto CMDEC_DECODING_TERMINATED;
			}

			RotMirFrames[i].phyY = Mem->phyAddr;
			RotMirFrames[i].phyU = RotMirFrames[i].phyY + Uoffset;
			RotMirFrames[i].phyV = RotMirFrames[i].phyY + Voffset;
			RotMirFrames[i].pUserData0 = (void*)Mem;
			RotMirFrames[i].pUserData1 = Mem->virAddr;
			RotMirFrames[i].pUserData2 = (void*)((unsigned int)Mem->virAddr + Uoffset);
			RotMirFrames[i].pUserData3 = (void*)((unsigned int)Mem->virAddr + Voffset);
		}

		//only set 1 rot-mir buffer to codec
		codecRet = DecoderSetRotMir_CMVpu(pDecObj, 1, 1, iRotDir, iMirDir, &RotMirFrames[iRotMirFrameIdx], iRotMired_stride);
		if(codecRet != IPP_STATUS_NOERR) {
			IPP_Printf("Err: DecoderSetRotMir_CMVpu() fail, ret %d\n", codecRet);
			IPP_Log(logfile, "a", "Err: DecoderSetRotMir_CMVpu() fail, ret %d\n", codecRet);
			DecoderFailFlag = -18;
			goto CMDEC_DECODING_TERMINATED;
		}
	}

	if(pLinePar->reformVC1DMV) {
		codecRet = DecoderReformVC1MPDirectMVFrame_CMVpu(pDecObj);
		if(codecRet != IPP_STATUS_NOERR) {
			IPP_Printf("Err: DecoderReformVC1MPDirectMVFrame_CMVpu() fail, ret %d\n", codecRet);
			IPP_Log(logfile, "a", "Err: DecoderReformVC1MPDirectMVFrame_CMVpu() fail, ret %d\n", codecRet);
			DecoderFailFlag = -28;
			goto CMDEC_DECODING_TERMINATED;
		}
	}


	//decoding loop
	do{
		//load data into VPU stream buffer
		if(InitPar.FilePlayEnable == 0) {//non file play mode
			int WanttoNewFillinLen;
			newRead = Prepare_FileBufData(&InputFileData, inputCmpFile);
			if(newRead == 0) {
				bFileEOF = 1;
			}
			WanttoNewFillinLen = InputFileData.datalen-(InputFileData.pBsCur-InputFileData.pBuf);
			iCMVpuFilledLen = FillDataToStmBuf_forNonFilePlay(pDecObj, InitPar.phyAddrStaticBitstreamBuf, InitPar.StaticBitstreamBufSize, StaticStmBufVaddrBase, InputFileData.pBsCur, WanttoNewFillinLen);
			if(iCMVpuFilledLen < 0) {
				IPP_Printf("Err: FillDataToStmBuf_forNonFilePlay() fail, ret %d\n", iCMVpuFilledLen);
				IPP_Log(logfile, "a", "Err: FillDataToStmBuf_forNonFilePlay() fail, ret %d\n", iCMVpuFilledLen);
				DecoderFailFlag = -19;
				goto CMDEC_DECODING_TERMINATED;
			}
			InputFileData.pBsCur += iCMVpuFilledLen;
			if(bFileEOF == 1 && InputFileData.pBsCur == InputFileData.pBuf + InputFileData.datalen) {
				bAllDataLoadedintoDec = 1;
			}
		}else{//file play mode
			if(b1stFrame_forFileplay) {
				//since 1st frame has been loaded into static buffer, no need to reload it, and don't change the frame data in static buffer
				FilePlay_StmBuf.DataLen = iSeqHdrAnd1stFrameLen;
				FilePlay_StmBuf.DataOffset = 0;
				FilePlay_StmBuf.phyAddrBitstreamBuf = InitPar.phyAddrStaticBitstreamBuf;
				b1stFrame_forFileplay = 0;
			}else{
				Ret = load_nextframe(&pStmFrame, &iStmFrameLen, &InputFileData, inputCmpFile, &bFileEOF, len_fp);
				//IPP_Printf("fileplay, loaded frame len %d\n", iStmFrameLen);
				if(Ret<0) {
					DecoderFailFlag = 7;
					IPP_Printf("Err: load_nextframe() fail, ret %d!!!!\n", Ret);
					goto CMDEC_DECODING_TERMINATED;
				}
				if(Ret == 0 || Ret == 1) {
					//read a frame from file success
					//choose a stream buffer
					Mem = select_fileplay_stream_bufer(iStmFrameLen, fileplay_stmbuf_pool, sizeof(fileplay_stmbuf_pool)/sizeof(fileplay_stmbuf_pool[0]));
					if(Mem == NULL) {
						DecoderFailFlag = 8;
						IPP_Printf("Err: The frame is too big, length %d, no candidate stream buffer\n", iStmFrameLen);
						goto CMDEC_DECODING_TERMINATED;
					}
					FillDataToStmBuf_forFilePlay(pDecObj, 0, 0, (Ipp32u)Mem->virAddr, pStmFrame, iStmFrameLen, 0);
					FilePlay_StmBuf.DataLen = iStmFrameLen;
					FilePlay_StmBuf.DataOffset = (Mem->phyAddr & 3);
					FilePlay_StmBuf.phyAddrBitstreamBuf = Mem->phyAddr & (~3);
				}else{
					//no frame data left
					bAllDataLoadedintoDec = 1;
					IPP_Printf("No frame read under file play mode\n");
				}
			}
		}


		bLastBufferFull = 0;
		//decode data
		do{
			bCallDecAgain = 0;
			if(InitPar.PollingMode == 2) {
				g_waitFunData.counter = WAITFUN_CHECK_STMBUFINFO_INTERVAL;
				g_waitFunData.pDec = pDecObj;
				g_waitFunData.lastEmptySpaceLen = -1;
			}

			IPP_StartPerfCounter(perf_index);
			IPP_StartPerfCounter(thread_perf_index);
			codecRet = DecodeFrame_CMVpu(pDecObj, &FilePlay_StmBuf, &DecOutInfo, bAllDataLoadedintoDec);
			IPP_StopPerfCounter(perf_index);
			IPP_StopPerfCounter(thread_perf_index);
			switch(codecRet) {
			case IPP_STATUS_NEED_INPUT:
				if(DecOutInfo.bVPUonDecodingStage == 0) {
					//decoder need more data to start decoding new frame
					iLog_NeedInputOutDecoding++;
					if(bAllDataLoadedintoDec == 1) {
						bDecodingNormalEnd = 1;
					}
				}else{
					//decoder need more data to finish decoding current frame
					iLog_NeedInputInDecoding++;
				}
				break;
			case IPP_STATUS_OUTPUT_DATA:
				//No frame is decoded, because the input frame data hasn't been consumed by VPU, therefore, application should feed this frame again.
				//But some frame still need output, which are reorder delayed frames.
				//Usually, under h264 re-order case, it probably happen.
				bCallDecAgain = 1;
				IPP_Printf("DecodeFrame_CMVpu() return IPP_STATUS_OUTPUT_DATA, output idx %d.\n", DecOutInfo.idxFrameOutput);
			case IPP_STATUS_FRAME_COMPLETE:
				//store the decoded frame's health degree
				if(IS_FRAME_IDX_VALID(DecOutInfo.idxFrameDecoded)) {
					DecFramesHealthInfo[DecOutInfo.idxFrameDecoded] = DecOutInfo.NumOfErrMbs;
				}

				IPP_Printf("DecodeFrame_CMVpu() output idx %3d (%2d,%2d,%d,%d) frame.\n", iLog_FrameDecOutputted, DecOutInfo.idxFrameOutput, DecOutInfo.idxFrameDecoded, DecOutInfo.FrameDecodedType, DecOutInfo.VC1MPDirectModeMVUsed);
				if(DecFramesHealthInfo[DecOutInfo.idxFrameOutput] != 0) {
					IPP_Printf("Warn: some MB(cnt %d) is corrupted at frame buffer %d!\n", DecFramesHealthInfo[DecOutInfo.idxFrameOutput], DecOutInfo.idxFrameOutput);
				}
				iLog_FrameDecOutputted++;
				if(DecOutInfo.VC1MPDirectModeMVUsed) {
					iLog_VC1MP_DMVFrameCnt++;
				}

				if(bRotMir == 0 && bPostDBK == 0) {
					//no post-processing
					consume1frame_and_flush_displayQ(&ItemInDisQCnt, DisplayQueue, pDecObj, outYUVFile, DecFramesInfoArr, DecOutInfo.idxFrameOutput, iFrameStride, &SeqInfo.FrameROI, produce_random_latency(), bCodec420SPMode);
				}else if(bRotMir) {
					//rotate-mirror enabled
					//release the decode frame buffer
					if(IPP_STATUS_NOERR != DecoderFrameDisplayed_CMVpu(pDecObj, DecOutInfo.idxFrameOutput)) {
						IPP_Printf("Err: Fail when DecoderFrameDisplayed_CMVpu() for rotation mirror case.\n");
						IPP_Log(logfile, "a", "Err: Fail when DecoderFrameDisplayed_CMVpu() for rotation mirror case.\n");
						DecoderFailFlag = -3;
						goto CMDEC_DECODING_TERMINATED;
					}
					consume1frame_and_flush_displayQ(&ItemInDisQCnt, DisplayQueue, NULL, outYUVFile, RotMirFrames, iRotMirFrameIdx, iRotMired_stride, &RotMired_roi, 1, bCodec420SPMode);

					//because last rotation-mirror buffer still hold by display, select a new rotation mirror buffer
					//if the last rotation-mirror buffer could be reused before next decoding, no-need to select another ratation-mirror buffer
					iRotMirFrameIdx++;
					if(iRotMirFrameIdx == sizeof(RotMirFrames)/sizeof(RotMirFrames[0])) {
						iRotMirFrameIdx = 0;
					}
					if(IPP_STATUS_NOERR != DecoderChangeRotMirBuf_CMVpu(pDecObj, &(RotMirFrames[iRotMirFrameIdx]))) {
						IPP_Printf("Err: DecoderChangeRotMirBuf_CMVpu() fail!!!\n");
						IPP_Log(logfile, "a", "Err: DecoderChangeRotMirBuf_CMVpu() fail!!!\n");
						DecoderFailFlag = -20;
						goto CMDEC_DECODING_TERMINATED;
					}
				}else if(bPostDBK) {
					//post-processing de-blocking enabled
					//release the decode frame buffer
					if(IPP_STATUS_NOERR != DecoderFrameDisplayed_CMVpu(pDecObj, DecOutInfo.idxFrameOutput)) {
						IPP_Printf("Err: Fail when DecoderFrameDisplayed_CMVpu() for post-dbk case.\n");
						IPP_Log(logfile, "a", "Err: Fail when DecoderFrameDisplayed_CMVpu() for post-dbk case.\n");
						DecoderFailFlag = -3;
						goto CMDEC_DECODING_TERMINATED;
					}
					consume1frame_and_flush_displayQ(&ItemInDisQCnt, DisplayQueue, NULL, outYUVFile, PostDBKFrames, iPostDBKFrameIdx, iFrameStride, &SeqInfo.FrameROI, 0, bCodec420SPMode);
#if 0
					//If before next frame decoding, application couldn't finishing consume the de-blocking frame data, it should set another de-blocking buffer to decoder because the content in buffer will be changed during next frame decoding. Otherwise, no-need to set another de-blocking buffer.
					iPostDBKFrameIdx++;
					if(iPostDBKFrameIdx == sizeof(PostDBKFrames)/sizeof(PostDBKFrames[0])) {
						iPostDBKFrameIdx = 0;
					}
					if(IPP_STATUS_NOERR != DecoderSetPostDBKBuf_CMVpu(pDecObj, &PostDBKFrames[iPostDBKFrameIdx])) {
						IPP_Printf("Err: DecoderSetPostDBKBuf_CMVpu() fail!!!\n");
						IPP_Log(logfile, "a", "Err: DecoderSetPostDBKBuf_CMVpu() fail!!!\n");
						DecoderFailFlag = -501;
						goto CMDEC_DECODING_TERMINATED;
					}
#endif
				}

#if 0
				//just to test Hibernate function
				{
					IppCodecStatus Rt;
					if(iLog_FrameDecOutputted == 15 || iLog_FrameDecOutputted == 28) {
						Rt = DecoderHibernate_CMVpu(pDecObj, 3);
						IPP_Printf("Call DecoderHibernate_CMVpu() ret %d\n", Rt);
						IPP_Printf("Sleeping...\n");
						usleep(1000000);
						IPP_Printf("Sleeping finish\n");
						Rt = DecoderWakeup_CMVpu(pDecObj);
						IPP_Printf("Call DecoderWakeup_CMVpu() ret %d\n", Rt);
					}
				}
#endif
				break;
			case IPP_STATUS_FRAME_UNDERRUN:
				//Only VC1 probably return IPP_STATUS_FRAME_UNDERRUN, it means current output frame is a skip frame(FrameDecodedType=4), and codec output the previous buffer directly instead of decoding skip frame into a new buffer. For other video format, won't return IPP_STATUS_FRAME_UNDERRUN and always decode the skip frame into a new buffer.
				//Another possibility is current output frame is a directmodemv frame(VC1MPDirectModeMVUsed=1) when DecoderReformVC1MPDirectMVFrame_CMVpu is called for VC1 stream, and codec output the previous buffer directly instead of output the real frame.
				
				//For this repeat output buffer, application also should call DecodeFrame_CMVpu() for it after application consumed it. Therefore, if some frame buffer is repeated outputted because of skip frame or directmodemv frame, application should call multiple times of DecodeFrame_CMVpu() for the buffer.

				IPP_Printf("Codec return IPP_STATUS_FRAME_UNDERRUN, frame out %d dec %d, type %d, directmv %d\n", DecOutInfo.idxFrameOutput, DecOutInfo.idxFrameDecoded, DecOutInfo.FrameDecodedType, DecOutInfo.VC1MPDirectModeMVUsed);
				iLog_FrameDecOutputted++;
				if(DecOutInfo.VC1MPDirectModeMVUsed) {
					iLog_VC1MP_DMVFrameCnt++;
				}else if(DecOutInfo.FrameDecodedType == 4) {
					iLog_VC1_SkipFrameCnt++;
				}

				if(pLinePar->dispVC1repeatframe == 0) {
					IPP_Printf("application don't want to display/dump VC1 repeat frame, ignore the frame\n");
					if(IPP_STATUS_NOERR != DecoderFrameDisplayed_CMVpu(pDecObj, DecOutInfo.idxFrameOutput)) {
						IPP_Printf("Err: Fail call DecoderFrameDisplayed_CMVpu().\n");
						IPP_Log(logfile, "a", "Err: Fail call DecoderFrameDisplayed_CMVpu().\n");
						DecoderFailFlag = -301;
						goto CMDEC_DECODING_TERMINATED;
					}
					break;
				}

#if 0
				//When application hasn't enable rotation&mirror function of CNM, if application display system could accept continuous repeated frame buffer, application could handle IPP_STATUS_FRAME_UNDERRUN just like handling IPP_STATUS_FRAME_COMPLETE.
				consume1frame_and_flush_displayQ(&ItemInDisQCnt, DisplayQueue, pDecObj, outYUVFile, DecFramesInfoArr, DecOutInfo.idxFrameOutput, iFrameStride, &SeqInfo.FrameROI, produce_random_latency(), bCodec420SPMode);
				break;
#else
				//if application couldn't accept continuous repeated frame buffer, copy the frame into an additional buffer
				//if enable rotation or mirror function, also need copy.
				if(bRotMir == 0) {
					int idx = select_framebuf_fordispskipframe(FrameBufsForDispSkip, CMDEC_VPU_MAXFRAMEBUFCNT, iFrameStride, SeqInfo.FrameAlignedHeight);
					if(idx < 0) {
						IPP_Printf("Err: Fail to select a frame buffer to store VC1 skip frame, ret %d\n", idx);
						IPP_Log(logfile, "a", "Err: Fail to select a frame buffer to store VC1 skip frame, ret %d\n", idx);
						DecoderFailFlag = -9;
						goto CMDEC_DECODING_TERMINATED;
					}
					copy_frame(&FrameBufsForDispSkip[idx], &DecFramesInfoArr[DecOutInfo.idxFrameOutput], iFrameStride, SeqInfo.FrameAlignedHeight);
					if(IPP_STATUS_NOERR != DecoderFrameDisplayed_CMVpu(pDecObj, DecOutInfo.idxFrameOutput)) {
						IPP_Printf("Err: Fail call DecoderFrameDisplayed_CMVpu().\n");
						IPP_Log(logfile, "a", "Err: Fail call DecoderFrameDisplayed_CMVpu().\n");
						DecoderFailFlag = -302;
						goto CMDEC_DECODING_TERMINATED;
					}

					consume1frame_and_flush_displayQ(&ItemInDisQCnt, DisplayQueue, (void*)1, outYUVFile, FrameBufsForDispSkip, idx, iFrameStride, &SeqInfo.FrameROI, produce_random_latency(), bCodec420SPMode);

				}else{
					int lastRotMirFrameIdx;
					if(IPP_STATUS_NOERR != DecoderFrameDisplayed_CMVpu(pDecObj, DecOutInfo.idxFrameOutput)) {
						IPP_Printf("Err: Fail call DecoderFrameDisplayed_CMVpu().\n");
						IPP_Log(logfile, "a", "Err: Fail call DecoderFrameDisplayed_CMVpu().\n");
						DecoderFailFlag = -303;
						goto CMDEC_DECODING_TERMINATED;
					}

					lastRotMirFrameIdx = iRotMirFrameIdx == 0 ? sizeof(RotMirFrames)/sizeof(RotMirFrames[0]) -1 : iRotMirFrameIdx -1;
					//For VC1 skip frame, decoder also won't fill rotated&mirrored data into rotate&mirror buffer.
					//Therefore, application should copy last reference rotated&mirrored data into the buffer
					copy_frame(&RotMirFrames[iRotMirFrameIdx], &RotMirFrames[lastRotMirFrameIdx], iRotMired_stride, iRotMired_height);

					consume1frame_and_flush_displayQ(&ItemInDisQCnt, DisplayQueue, NULL, outYUVFile, RotMirFrames, iRotMirFrameIdx, iRotMired_stride, &RotMired_roi, 1, bCodec420SPMode);
					//because last rotation-mirror buffer still hold by display, select a new rotation mirror buffer
					//if the last rotation-mirror buffer could be reused before next decoding, no-need to select another ratation-mirror buffer
					iRotMirFrameIdx++;
					if(iRotMirFrameIdx == sizeof(RotMirFrames)/sizeof(RotMirFrames[0])) {
						iRotMirFrameIdx = 0;
					}
					if(IPP_STATUS_NOERR != DecoderChangeRotMirBuf_CMVpu(pDecObj, &(RotMirFrames[iRotMirFrameIdx]))) {
						IPP_Printf("Err: DecoderChangeRotMirBuf_CMVpu() fail!!!\n");
						IPP_Log(logfile, "a", "Err: DecoderChangeRotMirBuf_CMVpu() fail!!!\n");
						DecoderFailFlag = -20;
						goto CMDEC_DECODING_TERMINATED;
					}
				}
				break;
#endif
			case IPP_STATUS_NOERR:
				//one frame decoding is finished, but this frame shouldn't be displayed yet. It probably caused by re-order.
				//store the decoded frame's health degree
				if(IS_FRAME_IDX_VALID(DecOutInfo.idxFrameDecoded)) {
					DecFramesHealthInfo[DecOutInfo.idxFrameDecoded] = DecOutInfo.NumOfErrMbs;
				}
				IPP_Printf("DecodeFrame_CMVpu() ret IPP_STATUS_NOERR %d, (%2d, %2d, %d, %d), err mb cnt %d.\n", DecOutInfo.OneFrameUnbrokenDecoded, DecOutInfo.idxFrameOutput, DecOutInfo.idxFrameDecoded, DecOutInfo.FrameDecodedType, DecOutInfo.VC1MPDirectModeMVUsed, DecOutInfo.NumOfErrMbs);
				break;
			case IPP_STATUS_BS_END:
				//IPP_STATUS_BS_END only return when DecodeFrame_CMVpu(, , , 1). When this return, the static stream buffer is cleared, and the re-order delayed frames already are all outputted.
				IPP_Printf("DecodeFrame_CMVpu() return IPP_STATUS_BS_END, decoding will finish.\n");
				bDecodingNormalEnd = 1;
				//currently, once DecodeFrame_CMVpu() return IPP_STATUS_BS_END, the decoding process should be finished. If application still feed data to codec and call DecodeFrame_CMVpu(), the behaviour is undefined.
#if 0
				//reseek to file begin and redecoding again
				bDecodingNormalEnd = 0;
				bAllDataLoadedintoDec = 0;
				bFileEOF = 0;
				IPP_Fseek(inputCmpFile, 0, 0);
				Clear_FileBuf(&InputFileData);
#endif
				break;
			case IPP_STATUS_FRAME_ERR:
				//one frame decoding couldn't successfully finished, probably caused by corrupt input stream.
				IPP_Printf("DecodeFrame_CMVpu() ret IPP_STATUS_FRAME_ERR (%d, %d, %d).\n", DecOutInfo.OneFrameUnbrokenDecoded, DecOutInfo.idxFrameOutput, DecOutInfo.idxFrameDecoded);
				break;
			case IPP_STATUS_BUFFER_FULL:
				if(bLastBufferFull) {
					IPP_Printf("Err: DecodeFrame_CMVpu() ret continuous IPP_STATUS_BUFFER_FULL, stop decoding.\n");
					IPP_Log(logfile, "a", "Err: DecodeFrame_CMVpu() ret continuous IPP_STATUS_BUFFER_FULL, stop decoding.\n");
					DecoderFailFlag = -4;
					//if pLinePar->multiple_buffull_exit isn't set to 0, application will enter dead loop. For debug purpose, sometimes, we need this dead loop.
					if(pLinePar->multiple_buffull_exit == 1) {
						goto CMDEC_DECODING_TERMINATED;
					}
				}
				bLastBufferFull = 1;
				IPP_Printf("DecodeFrame_CMVpu() ret IPP_STATUS_BUFFER_FULL, force flush display frames\n");
				force_flush_alldisplayframes(&ItemInDisQCnt, DisplayQueue, outYUVFile, bCodec420SPMode);
				bCallDecAgain = 1;
				break;

			case IPP_STATUS_NOTSUPPORTED_ERR:
				IPP_Printf("Err: DecodeFrame_CMVpu() return IPP_STATUS_NOTSUPPORTED_ERR, this frame isn't supported by VPU.\n");
				IPP_Log(logfile, "a", "Err: DecodeFrame_CMVpu() return IPP_STATUS_NOTSUPPORTED_ERR, this frame isn't supported by VPU.\n");
#if 0			//if this NOTSUPPORTED_ERR is caused by corrupted stream, we ignore it and continue decoding.
				break;
#else			//if the stream indeed contains the feature which isn't supported by cnm, we stop decoding.
				DecoderFailFlag = -1;
				goto CMDEC_DECODING_TERMINATED;
#endif
			case IPP_STATUS_TIMEOUT_ERR:
				IPP_Printf("Err: Decoder return time out(%d), VPU is hanging, stop decoding\n", codecRet);
				IPP_Log(logfile, "a", "Err: Decoder return time out(%d), VPU is hanging, stop decoding\n", codecRet);
				DecoderFailFlag = -5;
				goto CMDEC_DECODING_TERMINATED;
			case IPP_STATUS_WAIT_FOR_EVENT:
				IPP_Printf("Err: Decoder return wait_event, it means multiinstance sync fail, stop decoding\n", codecRet);
				IPP_Log(logfile, "a", "Err: Decoder return wait_event, it means multiinstance sync fail, stop decoding\n", codecRet);
				DecoderFailFlag = -31;
				goto CMDEC_DECODING_TERMINATED;
			default:
				IPP_Printf("Err: Decoder return un-expected return code %d, stop decoding\n", codecRet);
				IPP_Log(logfile, "a", "Err: Decoder return un-expected return code %d, stop decoding\n", codecRet);
				DecoderFailFlag = -32;
				goto CMDEC_DECODING_TERMINATED;
			}
		}while(bCallDecAgain == 1);

	}while(bDecodingNormalEnd == 0 && DecoderFailFlag == 0);



	//before close decoder, if VPU is still working, should stop it first
	if(DecOutInfo.bVPUonDecodingStage == 1 && InitPar.FilePlayEnable == 0) {
		IPP_Printf("Calling DecoderStopDecoding_CMVpu()...\n");
		codecRet = DecoderStopAndClearStaticStmBuf_CMVpu(pDecObj);
		if(codecRet != IPP_STATUS_NOERR) {
			IPP_Printf("Err: DecoderStopDecoding_CMVpu fail, return %d\n", codecRet);
			IPP_Log(logfile, "a", "Err: DecoderStopDecoding_CMVpu fail, return %d\n", codecRet);
			DecoderFailFlag = -41;
			goto CMDEC_DECODING_TERMINATED;
		}
	}

	//flush all display frame
	if(DecoderFailFlag == 0) {
		force_flush_alldisplayframes(&ItemInDisQCnt, DisplayQueue, outYUVFile, bCodec420SPMode);
	}


CMDEC_DECODING_TERMINATED:
	IPP_Printf("decoding procedure status %d\n", DecoderFailFlag);

	if(len_fp) {
		IPP_Fclose(len_fp);
		len_fp = NULL;
	}

	//close decoder
	if(pDecObj != NULL) {
		IPP_Printf("Calling DecoderFree_CMVpu()...\n");
		codecRet = DecoderFree_CMVpu(&pDecObj);
		IPP_Printf("DecoderFree_CMVpu() ret %d\n", codecRet);
		if(codecRet != IPP_STATUS_NOERR) {
			IPP_Printf("Err: DecoderFree_CMVpu fail, return %d\n", codecRet);
			IPP_Log(logfile, "a", "Err: DecoderFree_CMVpu fail, return %d\n", codecRet);
			DecoderFailFlag = -51;
		}
	}

	if(StaticStmMemHandle) {
		IPP_Printf("Free static stream buffers...\n");
		os_deconstruct_phycontbuf(StaticStmMemHandle);
	}

	for(i=1;i<sizeof(fileplay_stmbuf_pool)/sizeof(fileplay_stmbuf_pool[0]);i++) {
		if(fileplay_stmbuf_pool[i]) {
			IPP_Printf("Free fileplay stream buffer...\n");
			os_deconstruct_phycontbuf(fileplay_stmbuf_pool[i]);
		}
	}


	if(DecFramesInfoArr) {
		IPP_Printf("Free frame buffers...\n");
		for(i=0; i<iFrameBufferCnt; i++) {
			if(DecFramesInfoArr[i].pUserData0) {
				os_deconstruct_phycontbuf(DecFramesInfoArr[i].pUserData0);
			}
		}
		pTmp = (void*)DecFramesInfoArr;
		IPP_MemFree(&pTmp);
		DecFramesInfoArr = NULL;
	}

	if(bPostDBK) {
		IPP_Printf("Free post-dbk buffer...\n");
		for(i=0;i<sizeof(PostDBKFrames)/sizeof(PostDBKFrames[0]);i++) {
			if(PostDBKFrames[i].pUserData0) {
				os_deconstruct_phycontbuf(PostDBKFrames[i].pUserData0);
			}
		}
	}


	if(bRotMir) {
		IPP_Printf("Free rot mirror buffer...\n");
		for(i=0;i<sizeof(RotMirFrames)/sizeof(RotMirFrames[0]);i++) {
			if(RotMirFrames[i].pUserData0) {
				os_deconstruct_phycontbuf(RotMirFrames[i].pUserData0);
			}
		}
	}

	free_framebuf_fordispskipframe(FrameBufsForDispSkip, CMDEC_VPU_MAXFRAMEBUFCNT);

	if(InputFileData.pBuf) {
		IPP_Printf("Free input file buffer...\n");
		Deinit_FileBuf(&InputFileData);
	}

	if(pFrameNumber) {
		*pFrameNumber = iLog_FrameDecOutputted;
	}
	IPP_Printf("CodaDx8Dec() done, decoder outputted total %d frames, %d vc1 skip frame, %d vc1 DMV frame!!!\n", iLog_FrameDecOutputted, iLog_VC1_SkipFrameCnt, iLog_VC1MP_DMVFrameCnt);
	i64Log_CodecTime = IPP_GetPerfData(perf_index);
	if(i64Log_CodecTime == 0) {
		i64Log_CodecTime = 1; //to avoid divide zero
	}
	i64Log_CodecCpuTime = IPP_GetPerfData(thread_perf_index);
	if(i64Log_CodecCpuTime == 0) {
		i64Log_CodecCpuTime = 1;
	}
	//IPP_Printf("CodaDx8Dec() is done, pure codec spend %.3f sec, fps %.2f!!!\n", (double)i64Log_CodecTime/1000000., (double)iLog_FrameDecOutputted*1000000./(double)i64Log_CodecTime);
	{
		int sec = i64Log_CodecTime/1000000;
		int msec = (int)(i64Log_CodecTime - sec*(Ipp64s)1000000)/1000;
		int fps = iLog_FrameDecOutputted*(Ipp64s)1000000/i64Log_CodecTime;
		int fpsdot = iLog_FrameDecOutputted*(Ipp64s)(1000000*100)/i64Log_CodecTime - fps*100;
		int cpu_sec = i64Log_CodecCpuTime/1000000;
		int cpu_msec = (int)(i64Log_CodecCpuTime - cpu_sec*(Ipp64s)1000000)/1000;
		IPP_Printf("PERFORMANCE: CodaDx8Dec() is done, pure codec spend %d.%d sec(%lld usec), out %d frames, fps %d.%02d !!!!\n", sec, msec, i64Log_CodecTime, iLog_FrameDecOutputted, fps, fpsdot);
		IPP_Log(logfile, "a", "PERFORMANCE: CodaDx8Dec() is done, pure codec spend %d.%d sec(%lld usec), out %d frames, fps %d.%02d !!!!\n", sec, msec, i64Log_CodecTime, iLog_FrameDecOutputted, fps, fpsdot);
		IPP_Printf("CPU_PERF: cpu time %d.%d sec(%lld usec)!!!!\n", cpu_sec, cpu_msec, i64Log_CodecCpuTime);
	}
	IPP_Printf("CodaDx8Dec() done, DecodeFrame_CMVpu() ret need input cnt %d(during decoding), %d(out decoding).\n", iLog_NeedInputInDecoding, iLog_NeedInputOutDecoding);

	//add for physical memory test, just for memory leak test
	IPP_PysicalMemTest();

	if(DecoderFailFlag != 0) {
		return IPP_FAIL;
	}else{
		return IPP_OK;
	}

}

int vpuDec_setting_initConf(IppCMVpuDecInitPar *pPar, CMDecCmdLinePar* pCmdPar)
{
	switch(pCmdPar->format) {
	case 1:
		pPar->VideoStreamFormat = IPP_VIDEO_STRM_FMT_MPG2;
		break;
	case 2:
		pPar->VideoStreamFormat = IPP_VIDEO_STRM_FMT_MPG4;
		break;
	case 4:
		pPar->VideoStreamFormat = IPP_VIDEO_STRM_FMT_H263;
		break;
	case 5:
		pPar->VideoStreamFormat = IPP_VIDEO_STRM_FMT_H264;
		break;
	case 6:
		pPar->VideoStreamFormat = IPP_VIDEO_STRM_FMT_VC1M;
		break;
	default:
		IPP_Printf("Wrong! Unsupported video format!!!\n");
		return -1;  //not support
	}

	if(pCmdPar->reorder_choice == 0)  { //reorder_choice == 0 means select a suitable re-order mode based on video format
		if(pPar->VideoStreamFormat==IPP_VIDEO_STRM_FMT_MPG4 || pPar->VideoStreamFormat==IPP_VIDEO_STRM_FMT_H263) {
			//for those format, only support ReorderEnable=0
			pPar->ReorderEnable = 0;
		}else if(pPar->VideoStreamFormat==IPP_VIDEO_STRM_FMT_H264) {
			//for h264, highly suggest to adopt ReorderEnable=0, it's more stable.
			pPar->ReorderEnable = 0;
		}else{
			pPar->ReorderEnable = 1;
		}
	}else if(pCmdPar->reorder_choice == 1) {	//reorder_choice == 1 means enable re-order constrainedly
		pPar->ReorderEnable = 1;
	}else{  //reorder_choice == 2 means disable re-order constrainedly
		pPar->ReorderEnable = 0;
	}

	pPar->PollingMode = pCmdPar->polling_mode;
	if(pPar->PollingMode == 2) {
		pPar->PollingModeUserData = &g_waitFunData;
		pPar->PollingModeWaitFun = WaitFun_forNonfileplay;
	}


	pPar->FilePlayEnable			= pCmdPar->fileplay_enable;

	if(pPar->FilePlayEnable) {
		if(pPar->PollingMode != 0 && pPar->PollingMode != 1) {
			//file play mode not support custom polling mode
			IPP_Printf("Wrong! Under fileplay mode, only support polling mode 0 and 1.!!!\n");
			return -3;
		}
	}

	pPar->DynamicClkEnable  		= pCmdPar->dynaclk_enable;
	pPar->MultiInstanceSync 		= pCmdPar->multi_instance_sync;

	if(pPar->MultiInstanceSync) {
		if(!pPar->FilePlayEnable) {
			//MultiInstanceSync only support fileplay mode
			IPP_Printf("Wrong! Under multi instance sync mode, only support fileplay mode.!!!\n");
			return -4;
		}
	}

	pPar->PostDBKEnable				= pCmdPar->postdbk_enable;
	pPar->YUVFormat					= pCmdPar->codecyuvfmt;

	return 0;
}


int ParseCMDecAppCmd(char *pCmdLine, char *pSrcFileName, char *pDstFileName, char *pLogFileName, CMDecCmdLinePar* pLineParSet)
{
#define MAX_PAR_NAME_LEN	512
#define MAX_PAR_VALUE_LEN   512
#define CMVPU_STRNCPY(dst, src, len) \
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

	while((p1 = IPP_Strstr(pCur, "-"))){
		p2 = IPP_Strstr(p1, ":");
		if (NULL == p2) {
			return IPP_FAIL;
		}
		p3 = IPP_Strstr(p2, " "); /*one space*/
		if (NULL == p3) {
			p3 = pEnd;
		}

		par_name_len	= p2 - p1 - 1;
		par_value_len   = p3 - p2 - 1;

		if ((0 >= par_name_len)  || (MAX_PAR_NAME_LEN <= par_name_len) ||
			(0 >= par_value_len) || (MAX_PAR_VALUE_LEN <= par_value_len)) {
			return IPP_FAIL;
		}

		CMVPU_STRNCPY(par_name, p1 + 1, par_name_len);
		if ((0 == IPP_Strcmp(par_name, "i")) || (0 == IPP_Strcmp(par_name, "I"))) {
			/*input file*/
			CMVPU_STRNCPY(pSrcFileName, p2 + 1, par_value_len);
		} else if ((0 == IPP_Strcmp(par_name, "o")) || (0 == IPP_Strcmp(par_name, "O"))) {
			/*output file*/
			CMVPU_STRNCPY(pDstFileName, p2 + 1, par_value_len);
		} else if (0 == IPP_Strcmp(par_name, "lenfile")) {
			/*fragment length file, it's only for debug*/
			CMVPU_STRNCPY(pLineParSet->lenfilepath, p2 + 1, par_value_len);
			IPP_Printf("In cmd line: lenfilepath set to be %s\n", pLineParSet->lenfilepath);
		} else if ((0 == IPP_Strcmp(par_name, "l")) || (0 == IPP_Strcmp(par_name, "L"))) {
			/*log file*/
			CMVPU_STRNCPY(pLogFileName, p2 + 1, par_value_len);
		} else if ((0 == IPP_Strcmp(par_name, "fmt")) || (0 == IPP_Strcmp(par_name, "FMT"))) {
			CMVPU_STRNCPY(par_value, p2 + 1, par_value_len);
			pLineParSet->format = IPP_Atoi(par_value);
			IPP_Printf("In cmd line: fmt set to be %d\n", pLineParSet->format);
		} else if (0 == IPP_Strcmp(par_name, "rotmir_enable")) {
			CMVPU_STRNCPY(par_value, p2 + 1, par_value_len);
			if (0 == IPP_Strcmp(par_value, "0")) {
				pLineParSet->rotmir_enable = 0;
			} else if (0 == IPP_Strcmp(par_value, "1")) {
				pLineParSet->rotmir_enable = 1;
			} else {
				IPP_Printf("cmd input error, should be: -rotmir_enable:0|1 !\n");
				return IPP_FAIL;
			}
			IPP_Printf("In cmd line: rotmir_enable set to be %d\n", pLineParSet->rotmir_enable);
		} else if (0 == IPP_Strcmp(par_name, "reorderchoice")) {
			CMVPU_STRNCPY(par_value, p2 + 1, par_value_len);
			if (0 == IPP_Strcmp(par_value, "0")) {
				pLineParSet->reorder_choice = 0;
			} else if (0 == IPP_Strcmp(par_value, "1")) {
				pLineParSet->reorder_choice = 1;
			} else if (0 == IPP_Strcmp(par_value, "2")) {
				pLineParSet->reorder_choice = 2;
			} else {
				IPP_Printf("cmd input error, should be: -reorderchoice:0|1|2 !\n");
				return IPP_FAIL;
			}
			IPP_Printf("In cmd line: reorderchoice set to be %d\n", pLineParSet->reorder_choice);
		} else if (0 == IPP_Strcmp(par_name, "pollingmode")) {
			CMVPU_STRNCPY(par_value, p2 + 1, par_value_len);
			if (0 == IPP_Strcmp(par_value, "0")) {
				pLineParSet->polling_mode = 0;
			} else if (0 == IPP_Strcmp(par_value, "1")) {
				pLineParSet->polling_mode = 1;
			} else if (0 == IPP_Strcmp(par_value, "2")) {
				pLineParSet->polling_mode = 2;
			} else {
				IPP_Printf("cmd input error, should be: -pollingmode:0|1|2 !\n");
				return IPP_FAIL;
			}
			IPP_Printf("In cmd line: reorderchoice set to be %d\n", pLineParSet->polling_mode);
		} else if (0 == IPP_Strcmp(par_name, "dynaclk")) {
			CMVPU_STRNCPY(par_value, p2 + 1, par_value_len);
			if (0 == IPP_Strcmp(par_value, "0")) {
				pLineParSet->dynaclk_enable = 0;
			} else if (0 == IPP_Strcmp(par_value, "1")) {
				pLineParSet->dynaclk_enable = 1;
			} else {
				IPP_Printf("cmd input error, should be: -dynaclk:0|1 !\n");
				return IPP_FAIL;
			}
			IPP_Printf("In cmd line: dynaclk set to be %d\n", pLineParSet->dynaclk_enable);
		} else if (0 == IPP_Strcmp(par_name, "fileplay")) {
			CMVPU_STRNCPY(par_value, p2 + 1, par_value_len);
			if (0 == IPP_Strcmp(par_value, "0")) {
				pLineParSet->fileplay_enable = 0;
			} else if (0 == IPP_Strcmp(par_value, "1")) {
				pLineParSet->fileplay_enable = 1;
			} else {
				IPP_Printf("cmd input error, should be: -fileplay:0|1 !\n");
				return IPP_FAIL;
			}
			IPP_Printf("In cmd line: fileplay set to be %d\n", pLineParSet->fileplay_enable);
		} else if (0 == IPP_Strcmp(par_name, "rotdir")) {
			CMVPU_STRNCPY(par_value, p2 + 1, par_value_len);
			if (0 == IPP_Strcmp(par_value, "0")) {
				pLineParSet->rot_dir = 0;
			} else if (0 == IPP_Strcmp(par_value, "1")) {
				pLineParSet->rot_dir = 1;
			} else if (0 == IPP_Strcmp(par_value, "2")) {
				pLineParSet->rot_dir = 2;
			} else if (0 == IPP_Strcmp(par_value, "3")) {
				pLineParSet->rot_dir = 3;
			} else {
				IPP_Printf("cmd input error, should be: -rotdir:0|1|2|3 !\n");
				return IPP_FAIL;
			}
			IPP_Printf("In cmd line: rotdir set to be %d\n", pLineParSet->rot_dir);
		} else if (0 == IPP_Strcmp(par_name, "mirdir")) {
			CMVPU_STRNCPY(par_value, p2 + 1, par_value_len);
			if (0 == IPP_Strcmp(par_value, "0")) {
				pLineParSet->mir_dir = 0;
			} else if (0 == IPP_Strcmp(par_value, "1")) {
				pLineParSet->mir_dir = 1;
			} else if (0 == IPP_Strcmp(par_value, "2")) {
				pLineParSet->mir_dir = 2;
			} else if (0 == IPP_Strcmp(par_value, "3")) {
				pLineParSet->mir_dir = 3;
			} else {
				IPP_Printf("cmd input error, should be: -mirdir:0|1|2|3 !\n");
				return IPP_FAIL;
			}
			IPP_Printf("In cmd line: rotdir set to be %d\n", pLineParSet->mir_dir);
		} else if (0 == IPP_Strcmp(par_name, "multifullexit")) {
			CMVPU_STRNCPY(par_value, p2 + 1, par_value_len);
			if (0 == IPP_Strcmp(par_value, "0")) {
				pLineParSet->multiple_buffull_exit = 0;
			} else if (0 == IPP_Strcmp(par_value, "1")) {
				pLineParSet->multiple_buffull_exit = 1;
			} else {
				IPP_Printf("cmd input error, should be: -multifullexit:1|0 !\n");
				return IPP_FAIL;
			}
			IPP_Printf("In cmd line: multifullexit set to be %d\n", pLineParSet->multiple_buffull_exit);
		} else if (0 == IPP_Strcmp(par_name, "multiinstancesync")) {
			CMVPU_STRNCPY(par_value, p2 + 1, par_value_len);
			if (0 == IPP_Strcmp(par_value, "0")) {
				pLineParSet->multi_instance_sync = 0;
			} else if (0 == IPP_Strcmp(par_value, "1")) {
				pLineParSet->multi_instance_sync = 1;
			} else {
				IPP_Printf("cmd input error, should be: -multiinstancesync:0|1 !\n");
				return IPP_FAIL;
			}
			IPP_Printf("In cmd line: multiinstancesync set to be %d\n", pLineParSet->multi_instance_sync);
		} else if (0 == IPP_Strcmp(par_name, "dispvc1repeat")) {
			CMVPU_STRNCPY(par_value, p2 + 1, par_value_len);
			if (0 == IPP_Strcmp(par_value, "0")) {
				pLineParSet->dispVC1repeatframe = 0;
			} else if (0 == IPP_Strcmp(par_value, "1")) {
				pLineParSet->dispVC1repeatframe = 1;
			} else {
				IPP_Printf("cmd input error, should be: -dispvc1repeat:0|1 !\n");
				return IPP_FAIL;
			}
			IPP_Printf("In cmd line: dispvc1repeat set to be %d\n", pLineParSet->dispVC1repeatframe);
		} else if (0 == IPP_Strcmp(par_name, "postdbk")) {
			CMVPU_STRNCPY(par_value, p2 + 1, par_value_len);
			if (0 == IPP_Strcmp(par_value, "0")) {
				pLineParSet->postdbk_enable = 0;
			} else if (0 == IPP_Strcmp(par_value, "1")) {
				pLineParSet->postdbk_enable = 1;
			} else {
				IPP_Printf("cmd input error, should be: -postdbk:0|1 !\n");
				return IPP_FAIL;
			}
			IPP_Printf("In cmd line: postdbk set to be %d\n", pLineParSet->postdbk_enable);
		} else if (0 == IPP_Strcmp(par_name, "codecyuvfmt")) {
			CMVPU_STRNCPY(par_value, p2 + 1, par_value_len);
			if (0 == IPP_Strcmp(par_value, "0")) {
				pLineParSet->codecyuvfmt = 0;
			} else if (0 == IPP_Strcmp(par_value, "1")) {
				pLineParSet->codecyuvfmt = 1;
			} else {
				IPP_Printf("cmd input error, should be: -codecyuvfmt:0|1 !\n");
				return IPP_FAIL;
			}
			IPP_Printf("In cmd line: codecyuvfmt set to be %d\n", pLineParSet->codecyuvfmt);
		} else if (0 == IPP_Strcmp(par_name, "reformvc1dmv")) {
			CMVPU_STRNCPY(par_value, p2 + 1, par_value_len);
			if (0 == IPP_Strcmp(par_value, "0")) {
				pLineParSet->reformVC1DMV = 0;
			} else if (0 == IPP_Strcmp(par_value, "1")) {
				pLineParSet->reformVC1DMV = 1;
			} else {
				IPP_Printf("cmd input error, should be: -reformvc1dmv:0|1 !\n");
				return IPP_FAIL;
			}
			IPP_Printf("In cmd line: reformvc1dmv set to be %d\n", pLineParSet->reformVC1DMV);
		} else {
			IPP_Printf("cmd input error: undefined par: -%s\n", par_name);
			return IPP_FAIL;
		}

		pCur = p3;
	}

	return IPP_OK;

}


/***************************************************************************
// Name:			 main
// Description: 	 Entry function for CodaDx8 decoder
//
// Returns: 		 0: 		 decoding okay
//  				 others:  decoding fail
*****************************************************************************/

int CodecTest(int argc, char *argv[])
{
	int   ret = IPP_OK;
	unsigned long ulFrameNumber;

	char	input_file_name[512]	 = {'\0'};
	char	output_file_name[512]    = {'\0'};
	char	log_file_name[512]  	 = {'\0'};
	IPP_FILE* fp_cmp = NULL;
	IPP_FILE* fp_yuv = NULL;

	//set default value
	CMDecCmdLinePar LinePar = {0};
	LinePar.format = -1;
	LinePar.fileplay_enable = 1;	//by default, enable fileplay mode
	LinePar.multiple_buffull_exit = 0;
	LinePar.dispVC1repeatframe = 1;

	if (2 > argc) {
		IPP_Printf("%s\n", scHelpCodaDx8Dec);
		return IPP_FAIL;
	} else if (2 == argc){
		/*for validation*/
		IPP_Printf("Input command line: %s\n", argv[1]);
		if (IPP_OK != ParseCMDecAppCmd(argv[1], input_file_name, output_file_name, log_file_name, &LinePar)){
			IPP_Printf("command line is wrong! %s\n\n", argv[1]);
			IPP_Log(log_file_name, "a", "command line is wrong! %s\n\n", argv[1]);
			return IPP_FAIL;
		}
	} else {
		int i;
		for(i=1;i<argc;i++) {
			if(IPP_OK != ParseCMDecAppCmd(argv[i], input_file_name, output_file_name, log_file_name, &LinePar)) {
				IPP_Printf("some command line is mistake! %s\n\n", argv[i]);
				IPP_Log(log_file_name, "a", "some command line is mistake! %s\n\n", argv[i]);
				return IPP_FAIL;
			}

		}
	}


	switch(LinePar.format) {
	case 1:
	case 2:
	case 4:
	case 5:
	case 6:
		IPP_Printf("Input video format is supported: [%s]\n", scformat[LinePar.format]);
		break;
	default:
		IPP_Printf("Input video format(%d) is un-supported.\n", LinePar.format);
		IPP_Log(log_file_name, "a", "Input video format(%d) is un-supported.\n", LinePar.format);
		return IPP_FAIL;
	}

	if (0 == IPP_Strcmp(input_file_name, "\0")) {
		IPP_Printf("input file name is null!\n");
		IPP_Log(log_file_name, "a", "input file name is null!\n");
		return IPP_FAIL;
	} else {
		fp_cmp = IPP_Fopen(input_file_name, "rb");
		if (!fp_cmp) {
			IPP_Printf("Fails to open file %s!\n", input_file_name);
			IPP_Log(log_file_name, "a", "Fails to open file %s!\n", input_file_name);
			return IPP_FAIL;
		}
	}

	if (0 == IPP_Strcmp(output_file_name, "\0")) {
		IPP_Log(log_file_name, "a", "output file name is null!\n");
		//sometimes, we don't want to dump decoded data, such as performance test. therefore, not return IPP_FAIL here.
	} else {
		fp_yuv = IPP_Fopen(output_file_name, "wb");
		if (!fp_yuv) {
			IPP_Log(log_file_name, "a", "Fails to open file %s\n", output_file_name);
			return IPP_FAIL;
		}
	}

	IPP_Printf("input file  : %s\n", input_file_name);

	IPP_Log(log_file_name, "a", "input file  : %s\n", input_file_name);
	IPP_Log(log_file_name, "a", "output file : %s\n", output_file_name);
	IPP_Log(log_file_name, "a", "begin to decode\n");

	DisplayLibVersion();

	ret = CodaDx8Dec(fp_cmp, fp_yuv, &ulFrameNumber, log_file_name, &LinePar);

	switch (ret)
	{
	case IPP_OK:
		IPP_Log(log_file_name, "a", "Everything is Okay.\n");
		break;
	default:
		IPP_Log(log_file_name, "a", "Something wrong, CodaDx8Dec() ret %d\n", ret);
		break;
	}

	if ( fp_cmp ){
		IPP_Fclose (fp_cmp);
	}

	if ( fp_yuv ){
		IPP_Fclose (fp_yuv);
	}


	return ret;
}

