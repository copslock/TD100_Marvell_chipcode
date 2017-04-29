/*
Copyright (c) 2010, Marvell International Ltd.
All Rights Reserved.

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
/* gcnmcodec_com.c */


#include <gst/gst.h>

#include <string.h>
#include <unistd.h> //to include usleep()
#include "cnm_bridge.h"
#include "codecCNM.h"
#include "misc.h"
#include "gcnmcodec_com.h"

#define GSTCNMDEC_STMPOOLMEM_RECYCLE_THRESHOLD		(896*1024)
int gcnmdec_select_stream(GCNMDec_StmBuf StmPool[], int iPoolSz, int iWantSz)
{
	int i;
	int totalbufsz = 0;
	CnmPContMem* mem;
	if(iWantSz <= 0) {
		gcnm_printf("in gcnmdec_select_stream, iWantSz err %d\n", iWantSz);
		return -2;
	}
	//iDataLen != 0 means the stream is using, iBufSize == 0 means the node is idle
	for(i=0;i<iPoolSz;i++) {
		totalbufsz += StmPool[i].iBufSize;
		if(StmPool[i].iDataLen == 0) {
			if(StmPool[i].iBufSize >= iWantSz) {
				return i;
			}
		}
	}
	gcnm_printf("No suitable stream buffer is found, want sz %d\n", iWantSz);

	if(totalbufsz >= GSTCNMDEC_STMPOOLMEM_RECYCLE_THRESHOLD) {
		//begin memory recycle, clean the pool except the 1st node, which is for static stream buffer
		for(i=1;i<iPoolSz;i++) {
			if(StmPool[i].iDataLen == 0) {
				delete_cnmStmBuf(StmPool[i].mem);
				StmPool[i].mem = NULL;
				StmPool[i].BufVirAddr = NULL;
				StmPool[i].BufPhyAddr = 0;
				StmPool[i].iBufSize = 0;
			}
		}
	}

	//find idle node except 1st node
	for(i=1;i<iPoolSz;i++) {
		if(StmPool[i].iBufSize == 0) {
			break;
		}
	}

	//no found idle node, clean the pool except the 1st node, which is for static stream buffer
	if(i == iPoolSz) {
		for(i=1;i<iPoolSz;i++) {
			if(StmPool[i].iDataLen == 0) {
				delete_cnmStmBuf(StmPool[i].mem);
				StmPool[i].mem = NULL;
				StmPool[i].BufVirAddr = NULL;
				StmPool[i].BufPhyAddr = 0;
				StmPool[i].iBufSize = 0;
			}
		}
	}

	//find idle node
	for(i=1;i<iPoolSz;i++) {
		if(StmPool[i].iBufSize == 0) {
			break;
		}
	}

	if(i == iPoolSz) {
		return i;   //all Pool Node are using
	}

	mem = new_cnmStmBuf(iWantSz);
	if(mem == NULL) {
		gcnm_printf("Call pfun_malloc(,%d,) fail.\n", iWantSz);
		return -1;  //-1 means no memory error
	}else{
		gcnm_printf("Call pfun_malloc(%d) succeed.\n", iWantSz);
		StmPool[i].mem = mem;
		StmPool[i].BufVirAddr = mem->VAddr;
		StmPool[i].BufPhyAddr = mem->PAddr;
		StmPool[i].iBufSize = iWantSz;
		StmPool[i].iDataLen = 0;
		return i;
	}
}

#define I420_standardLayout_sameas_compactLayout(w, h)  (((w)&7)==0 && ((h)&1)==0)  //if width is 8 align and height is 2 align, the GST standard I420 layout is the same as compact I420 layout
static __inline int calculate_GSTstandard_I420_size(int w, int h)
{
	//gstreamer defined standard I420 layout, see <gst/video/video.h>, gstvideo and gst_video_format_get_size()
	return (GST_ROUND_UP_4(w) + GST_ROUND_UP_4(GST_ROUND_UP_2(w)>>1)) * GST_ROUND_UP_2(h);
}

void gcnmdec_update_downbuf_layout(int w, int h, int category, GCNMDec_DownBufLayout* pDownBufLayout)
{
	if(category == 0) {
		//we defined compact I420 layout
		pDownBufLayout->iDownbufYpitch = w;
		pDownBufLayout->iDownbufUVpitch = w>>1;
		pDownBufLayout->iDownbufUoffset = pDownBufLayout->iDownbufYpitch * h;
		pDownBufLayout->iDownbufVoffset = pDownBufLayout->iDownbufUoffset + pDownBufLayout->iDownbufUVpitch * (h>>1);
		pDownBufLayout->iDownbufSz = pDownBufLayout->iDownbufVoffset + pDownBufLayout->iDownbufUVpitch * (h>>1);

	}else if(category == 1) {
		//gstreamer defined standard I420 layout, see <gst/video/video.h>, gstvideo and gst_video_format_get_size()
		pDownBufLayout->iDownbufYpitch = GST_ROUND_UP_4(w);
		pDownBufLayout->iDownbufUVpitch = GST_ROUND_UP_4(GST_ROUND_UP_2(w)>>1);
		pDownBufLayout->iDownbufUoffset = pDownBufLayout->iDownbufYpitch * GST_ROUND_UP_2(h);
		pDownBufLayout->iDownbufVoffset = pDownBufLayout->iDownbufUoffset + pDownBufLayout->iDownbufUVpitch * (GST_ROUND_UP_2(h)>>1);
		pDownBufLayout->iDownbufSz = pDownBufLayout->iDownbufVoffset + pDownBufLayout->iDownbufUVpitch * (GST_ROUND_UP_2(h)>>1);
	}
	return;
}

GstFlowReturn gcnmdec_require_framebuf_fromdownplugin(GCNMDec_DownBufLayout* pDownBufLayout, GstPad* srcPad, GstBuffer** ppGstBuf, int bAutoToGSTStandardI420)
{
	GstFlowReturn ret;
	ret = gst_pad_alloc_buffer_and_set_caps(srcPad, 0, pDownBufLayout->iDownbufSz, GST_PAD_CAPS(srcPad), ppGstBuf);
	if(ret == GST_FLOW_OK) {
		if(bAutoToGSTStandardI420) {
			//Gstreamer defined I420/YV12 format require pitch should be 4 aligned(see <gst/video/video.h>, gstvideo and gst_video_format_get_size()). xvimagesink and ffdec always obey this rule.
			//However, filesink and fakesink don't require this limit because they don't care the format/layout of buffer.
			//Therefore, we output different buffer layout for different sink.
			if(!I420_standardLayout_sameas_compactLayout(pDownBufLayout->iDownFrameWidth, pDownBufLayout->iDownFrameHeight) && GST_BUFFER_SIZE(*ppGstBuf) != (guint)pDownBufLayout->iDownbufSz) {
				if((int)GST_BUFFER_SIZE(*ppGstBuf) == calculate_GSTstandard_I420_size(pDownBufLayout->iDownFrameWidth, pDownBufLayout->iDownFrameHeight)) {
					gcnmdec_update_downbuf_layout(pDownBufLayout->iDownFrameWidth, pDownBufLayout->iDownFrameHeight, 1, pDownBufLayout);
				}
			}
		}
	}
	return ret;
}

void gcnmdec_copy_to_downframebuf(unsigned char* pDst, GCNMDec_DownBufLayout* pDownBufLayout, IppCMVpuDecFrameAddrInfo* pCNMFrame, IppiRect* pCNMFrameRoi)
{
	int i;

	int SrcStep = (int)(pCNMFrame->pUserData0) & 0xffff;	//high 16 bit probably record other information, for example, in mpeg2 plug-in, high 16 bit record the frame type
	Ipp8u *pSrc = (Ipp8u*)(pCNMFrame->pUserData1) + pCNMFrameRoi->y * SrcStep + pCNMFrameRoi->x;
	Ipp8u* pTgt = pDst;

	if(SrcStep == pDownBufLayout->iDownbufYpitch) {
		memcpy(pTgt, pSrc, SrcStep * pCNMFrameRoi->height);
	}else{
		for(i = 0; i < pCNMFrameRoi->height; i++) {
			memcpy(pTgt, pSrc, pCNMFrameRoi->width);
			pSrc += SrcStep;
			pTgt += pDownBufLayout->iDownbufYpitch;
		}
	}

	SrcStep >>= 1;
	pSrc = (Ipp8u*)(pCNMFrame->pUserData2) + (pCNMFrameRoi->y>>1) * SrcStep + (pCNMFrameRoi->x>>1);
	pTgt = pDst + pDownBufLayout->iDownbufUoffset;

	if(SrcStep == pDownBufLayout->iDownbufUVpitch) {
		memcpy(pTgt, pSrc, SrcStep * (pCNMFrameRoi->height>>1) );
	}else{
		for(i = 0; i < (pCNMFrameRoi->height>>1); i++) {
			memcpy(pTgt, pSrc, pCNMFrameRoi->width>>1);
			pSrc += SrcStep;
			pTgt += pDownBufLayout->iDownbufUVpitch;
		}
	}


	pSrc = (Ipp8u*)(pCNMFrame->pUserData3) + (pCNMFrameRoi->y>>1) * SrcStep + (pCNMFrameRoi->x>>1);
	pTgt = pDst + pDownBufLayout->iDownbufVoffset;

	if(SrcStep == pDownBufLayout->iDownbufUVpitch) {
		memcpy(pTgt, pSrc, SrcStep * (pCNMFrameRoi->height>>1) );
	}else{
		for(i = 0; i < (pCNMFrameRoi->height>>1); i++) {
			memcpy(pTgt, pSrc, pCNMFrameRoi->width>>1);
			pSrc += SrcStep;
			pTgt += pDownBufLayout->iDownbufUVpitch;
		}
	}

	return;
}

void gcnmdec_init_TsDuMgr(GCNMDec_TSDUManager* pTsDuMgr)
{
	pTsDuMgr->TsDuList = NULL;
	pTsDuMgr->iTsDuListLen = 0;
	pTsDuMgr->LastOutputTs = GST_CLOCK_TIME_NONE;
	pTsDuMgr->LastOutputDu = GST_CLOCK_TIME_NONE;
	pTsDuMgr->FrameFixPeriod = GST_CLOCK_TIME_NONE;
	pTsDuMgr->SegmentStartTs = -1;
	return;
}

void gcnmdec_remove_redundant_TsDu(GCNMDec_TSDUManager* pTsDuMgr, int leftlen)
{
	while(pTsDuMgr->iTsDuListLen > leftlen) {
		g_slice_free(GCNMDec_TsDuPair, pTsDuMgr->TsDuList->data);
		pTsDuMgr->TsDuList = g_slist_delete_link(pTsDuMgr->TsDuList, pTsDuMgr->TsDuList);
		pTsDuMgr->iTsDuListLen--;
		gcnm_printf("Removed 1 TSDU, left cnt %d\n", pTsDuMgr->iTsDuListLen);
	}
	return;
}

static gint compareTSDUfun(gconstpointer a, gconstpointer b)
{
	if(((GCNMDec_TsDuPair*)a)->TS >= ((GCNMDec_TsDuPair*)b)->TS) {
		return 1;
	}else{
		return -1;
	}
}

void gcnmdec_insert_TsDu(GCNMDec_TSDUManager* pTsDuMgr, GstBuffer* buf)
{
	//reorder timestamp and insert it into list
	if(GST_BUFFER_TIMESTAMP(buf) != GST_CLOCK_TIME_NONE) {
		GCNMDec_TsDuPair* pTsDu = g_slice_new(GCNMDec_TsDuPair);
		pTsDu->TS = GST_BUFFER_TIMESTAMP(buf);
		pTsDu->DU = GST_BUFFER_DURATION(buf);
		pTsDuMgr->TsDuList = g_slist_insert_sorted(pTsDuMgr->TsDuList, pTsDu, compareTSDUfun);
		pTsDuMgr->iTsDuListLen++;
	}
	return;
}

void gcnmdec_refresh_output_TsDu(GCNMDec_TSDUManager* pTsDuMgr)
{
	//1. use the ts in list as current output ts
	if(pTsDuMgr->TsDuList != NULL) {
		GCNMDec_TsDuPair* pTsDu = (GCNMDec_TsDuPair*)pTsDuMgr->TsDuList->data;
		pTsDuMgr->LastOutputTs = pTsDu->TS;
		pTsDuMgr->LastOutputDu = pTsDu->DU;
		pTsDuMgr->TsDuList = g_slist_delete_link(pTsDuMgr->TsDuList, pTsDuMgr->TsDuList);
		g_slice_free(GCNMDec_TsDuPair, pTsDu);
		pTsDuMgr->iTsDuListLen--;
		return;
	}

	//2. use auto increased ts based on the last output ts as current output ts
	if(pTsDuMgr->LastOutputTs != GST_CLOCK_TIME_NONE) {
		if(pTsDuMgr->FrameFixPeriod != GST_CLOCK_TIME_NONE) {
			pTsDuMgr->LastOutputTs += pTsDuMgr->FrameFixPeriod;
			pTsDuMgr->LastOutputDu = pTsDuMgr->FrameFixPeriod;
			return;
		}else if(pTsDuMgr->LastOutputDu != GST_CLOCK_TIME_NONE) {
			//reuse the last duration as current duration
			pTsDuMgr->LastOutputTs += pTsDuMgr->LastOutputDu;
			return;
		}
	}

	//3. use GST_CLOCK_TIME_NONE as current output ts
	pTsDuMgr->LastOutputTs = GST_CLOCK_TIME_NONE;
	pTsDuMgr->LastOutputDu = GST_CLOCK_TIME_NONE;
	return;
}


int gcnmdec_startup_codec(void* pElement, GCNMDec_StartUpDec* pStart, CNMDec_ShareFrameManager* mgr)
{
	int totalframecnt,iFrameBufYStride;
	int i;
	IppCMVpuDecFrameAddrInfo codecBuf[32];
	IppCMVpuDecFrameAddrInfo* FrameArr = NULL;

	IppCodecStatus codecRt;
	int rt;
	IppCMVpuDecInitPar* pPar = pStart->pInitPar;

	//init codec instance
	pStart->iCodecApiErr = 0;   //clean it for a new codec instance
	pStart->pCnmDec = NULL;
	gcnm_printf("Call DecoderInitAlloc_CMVpu()...\n");
	codecRt = DecoderInitAlloc_CMVpu(pPar, IPP_MemMalloc, IPP_MemFree, &pStart->pCnmDec);
	gcnm_printf("DecoderInitAlloc_CMVpu() ret %d, codec addr 0x%x\n", codecRt, (unsigned int)pStart->pCnmDec);
	mgr->CurCodecSerialNum = (mgr->CurCodecSerialNum + 1) & 0xff;	//only 8bits valid. whether successful, we always change codec SN
	memset(mgr->DecOutFrameConsumeStatus, 0, sizeof(mgr->DecOutFrameConsumeStatus));
	if(codecRt != IPP_STATUS_NOERR) {
		pStart->iCodecApiErr |= ERR_CNMDEC_INIT;
		g_warning("Err: DecoderInitAlloc_CMVpu fail, ret %d", codecRt);
		rt = -20;
		goto RET_gcnmdec_startup_codec;
	}

	if(pStart->iFilledLen_forCodecStart < 512) {
		memset(pStart->virAddrStaticBitstreamBuf+pStart->iFilledLen_forCodecStart, 0, 512-pStart->iFilledLen_forCodecStart);	//c&m has limitation for some stream, must > 512 bytes to parse sequence header
		pStart->iFilledLen_forCodecStart = 512;
	}

	codecRt = DecoderStaticStmBufDataFilled_CMVpu(pStart->pCnmDec, pStart->iFilledLen_forCodecStart);
	if(codecRt != IPP_STATUS_NOERR) {
		pStart->iCodecApiErr |= ERR_CNMDEC_FILLSSTMBUFAPI;
		g_warning("Err: DecoderStaticStmBufDataFilled_CMVpu fail, ret %d", codecRt);
		rt = -21;
		goto RET_gcnmdec_startup_codec;
	}

	gcnm_printf("call DecoderParseSeqHeader_CMVpu(0x%x)...\n", (unsigned int)pStart->pCnmDec);
	codecRt = DecoderParseSeqHeader_CMVpu(pStart->pCnmDec, pStart->pSeqInfo);
	if(codecRt != IPP_STATUS_NOERR) {
		pStart->iCodecApiErr |= ERR_CNMDEC_PARSESEQHDR;
		g_warning("Err: DecoderParseSeqHeader_CMVpu fail, ret %d", codecRt);
		rt = -22;
		goto RET_gcnmdec_startup_codec;
	}
	gcnm_printf("cnm sequence parse result: w %d, h %d; roi w %d, h %d, %d, %d; fps %d/%d; mincnt %d, delay %d, nextcnt %d\n", pStart->pSeqInfo->FrameAlignedWidth, pStart->pSeqInfo->FrameAlignedHeight, pStart->pSeqInfo->FrameROI.width, pStart->pSeqInfo->FrameROI.height, pStart->pSeqInfo->FrameROI.x, pStart->pSeqInfo->FrameROI.y, pStart->pSeqInfo->FrameRateNumerator, pStart->pSeqInfo->FrameRateDenominator, pStart->pSeqInfo->MinFrameBufferCount, pStart->pSeqInfo->MaxReorderDelay, pStart->pSeqInfo->MaxNumOfFrameBufUsedForNextDecoded);

	rt = pStart->allocate_framebuf(pElement, pStart->pSeqInfo, &totalframecnt, &FrameArr, &iFrameBufYStride);
	if(0 != rt) {
		g_warning("Err: call allocate_framebuf() fail, ret %d", rt);
		rt = -30;
		//allocate frame buffer fail, free static stream buffer
		if(pStart->StmBufPool[0].iBufSize && pStart->StmBufPool[0].mem) {
			delete_cnmStmBuf(pStart->StmBufPool[0].mem);
			pStart->StmBufPool[0].iBufSize = 0;
			pStart->StmBufPool[0].BufPhyAddr = 0;
			pStart->StmBufPool[0].BufVirAddr = NULL;
			pStart->StmBufPool[0].iDataLen = 0;
			pStart->StmBufPool[0].mem = NULL;
		}
		pStart->iMemoryErr = -1;
		goto RET_gcnmdec_startup_codec;
	}
	pStart->iMemoryErr = 0;

	for(i=0;i<totalframecnt;i++) {
		codecBuf[i].phyY = ((CnmPContMem*)(FrameArr[i].phyY))->PAddr;
		codecBuf[i].phyU = FrameArr[i].phyU;
		codecBuf[i].phyV = FrameArr[i].phyV;
	}

	gcnm_printf("Call DecoderRegisterFrameBufs_CMVpu(0x%x,,%d,%d)...\n", (unsigned int)pStart->pCnmDec, totalframecnt, iFrameBufYStride);
	codecRt = DecoderRegisterFrameBufs_CMVpu(pStart->pCnmDec, codecBuf, totalframecnt, iFrameBufYStride);
	if(codecRt != IPP_STATUS_NOERR) {
		pStart->iCodecApiErr |= ERR_CNMDEC_REGFRAMEBUF;
		g_warning("Err: DecoderRegisterFrameBufs_CMVpu fail, ret %d", codecRt);
		rt = -23;
		goto RET_gcnmdec_startup_codec;
	}

	gcnm_printf("gcnmdec_startup_codec() is succeed!\n");
	rt = 0;

RET_gcnmdec_startup_codec:
	return rt;
}

int gcnmdec_close_codec(void** ppCnmDec, CNMDec_ShareFrameManager* mgr)
{
	IppCodecStatus codecRt;
	int codecErr = 0;
	gcnm_printf("Call DecoderFree_CMVpu...\n");
	codecRt = DecoderFree_CMVpu(ppCnmDec);
	if(codecRt != IPP_STATUS_NOERR) {
		codecErr = ERR_CNMDEC_DEINIT;
		gcnm_printf("Err: DecoderFree_CMVpu fail, ret %d\n", codecRt);
	}
	*ppCnmDec = NULL;
	mgr->CurCodecSerialNum = (mgr->CurCodecSerialNum + 1) & 0xff;	//only 8bit valid. when codec is free, we also change current codec SN
	gcnm_printf("Call DecoderFree_CMVpu end.\n");
	return codecErr;
}

typedef struct{
	gpointer ele;
	void (*watcher_onfinalize_hook)(gpointer);
	CNMDec_ShareFrameManager* mgr;
}WatcherControl;

FRAMES_WATCHMAN gcnmdec_watcher_create(gpointer ele, void (*dothing_atwatcherdead)(gpointer), CNMDec_ShareFrameManager* mgr)
{
	GstBuffer* watcher;
	WatcherControl* ctrl;
	gcnm_printf("frame watcher is creating on ele 0x%x\n", (unsigned int)ele);
	watcher = gst_buffer_new();
	if(watcher == NULL) {
		gcnm_printf("frame watcher create fail!\n");
		return NULL;
	}
	mgr->mtx = g_mutex_new();
	if(mgr->mtx == NULL) {
		gcnm_printf("mutex in ShareFrameManager create fail!\n");
		gst_buffer_unref(watcher);
		return NULL;
	}
	ctrl = g_slice_new(WatcherControl);
	ctrl->ele = gst_object_ref(ele);
	ctrl->watcher_onfinalize_hook = dothing_atwatcherdead;
	ctrl->mgr = mgr;
	
	GST_BUFFER_MALLOCDATA(watcher) = (guint8*)ctrl;
	GST_BUFFER_FREE_FUNC(watcher) = (GFreeFunc)gcnmdec_watcher_onfinalize;
	gcnm_printf("frame watcher is created\n");
	return watcher;
}

void gcnmdec_watcher_onfinalize(gpointer ctrl)
{
	void (*dothing_atwatcherdead)(gpointer);
	CNMDec_ShareFrameManager* mgr = ((WatcherControl*)ctrl)->mgr;
	gpointer ele = ((WatcherControl*)ctrl)->ele;
	dothing_atwatcherdead = ((WatcherControl*)ctrl)->watcher_onfinalize_hook;
	g_slice_free(WatcherControl, ctrl);

#if 0	//just for debug
	{
		int jj;
		for(jj=0;jj<sizeof(mgr->DFBShareArr)/sizeof(mgr->DFBShareArr[0]);jj++) {
			printf(">>>>>>> mgr->DFBShareArr[%d] status 0x%x 0x%x\n", jj, (unsigned int)mgr->DFBShareArr[jj].pUserData1, (unsigned int)mgr->DFBShareArr[jj].pUserData3);
		}
	}
#endif

	//gstcnmElement_clean_idleShareFBs(mgr, mgr->wmvShareSFBRepo);	//in fact, no need to call gstcnmElement_clean_idleShareFBs here, because at buffer finalizing, will call gstcnmElement_clean_idleShareFBs. Useless code, will remove.
	mgr->iDFBShareLen = 0;

	g_mutex_free(mgr->mtx);
	mgr->mtx = NULL;

	gcnm_printf("frame watcher is finalizing, ele 0x%x\n", (unsigned int)ele);
	if(dothing_atwatcherdead) {
		dothing_atwatcherdead(ele);
	}

	gst_object_unref(ele);   //since the frame buffer watcher is cleaned, object could be finalized. NOTE: This calling probably cause element finalize, all action on element should be called before this calling.
	gcnm_printf("frame watcher is finalized\n");
	return;
}

void gcnmdec_downbuf_onfinalize(gpointer info)
{
	gcnm_printfmore("downbuf is finalizing, info 0x%x\n", (unsigned int)info);
	gcnm_printfmore("in %s, thread id 0x%x, pthread id-t 0x%x, sizeof(pthread_t) %d\n", __func__, (unsigned int)syscall(224), (unsigned int)pthread_self(), sizeof(pthread_t));
	CNMDec_ShareFrameManager* mgr = (CNMDec_ShareFrameManager*)(((IppCMVpuDecFrameAddrInfo*)info)->pUserData3);
	unsigned int sNum_frameidx = (unsigned int)(((IppCMVpuDecFrameAddrInfo*)info)->pUserData2);

	g_mutex_lock(mgr->mtx);

	if(sNum_frameidx != (unsigned int)-1) {
		int codecSerialNum = sNum_frameidx & 0xff;
		int frameidx =  sNum_frameidx >> 24;
		int idxInDFBShare = (sNum_frameidx>>16)&0xff;
		mgr->DFBShareArr[idxInDFBShare].pUserData3 = (void*)0;
		mgr->iSinkOwnedDecFrameBufCnt--;
		if(codecSerialNum == (int)mgr->CurCodecSerialNum) {
			mgr->DecOutFrameConsumeStatus[frameidx] = mgr->DecOutFrameConsumeStatus[frameidx] + 1;
			gcnm_printfmore("downbuf is finalized idx %d, DFB idx %d\n", frameidx, idxInDFBShare);
		}else{
			gcnm_printf("downbuf is finalized(sn %d, idx %d, DFB idx %d), but codec(sn %d) has changed\n", codecSerialNum, frameidx, idxInDFBShare, mgr->CurCodecSerialNum);
		}
		if(mgr->pfun_echo) {
			mgr->pfun_echo(mgr->ele, "gstbuf finalize allocd=%p, sink bcnt %d", info, mgr->iSinkOwnedDecFrameBufCnt);
		}
	}else{
		//it's VC1 skip frame
		((IppCMVpuDecFrameAddrInfo*)info)->pUserData0 = (void*)0;	//flag this skip frame buffer is idle, could be reused again
		mgr->iSinkOwnedSFBCnt--;
		gcnm_printfmore("VC1 skip disp frame 0x%x finalized\n", (unsigned int)info);
	}
	if(mgr->NotNeedFMemAnyMore) {
		gstcnmElement_clean_idleShareFBs(mgr, mgr->wmvShareSFBRepo);
	}
	g_mutex_unlock(mgr->mtx);

	//finally, below operation cause watcher's finalizing and cnm element's finalizing
	FRAMES_WATCHMAN_UNREF(mgr->watcher);
	
	return;
}

int gcnmdec_informcodec_decframeconsumed(void* codec, CNMDec_ShareFrameManager* mgr)
{
	unsigned int i;
	IppCodecStatus codecRt;
	int* Status = mgr->DecOutFrameConsumeStatus;
	int ret = 0;

	g_mutex_lock(mgr->mtx);	
	for(i=0;i<sizeof(mgr->DecOutFrameConsumeStatus)/sizeof(mgr->DecOutFrameConsumeStatus[0]);i++) {
		while(Status[i] > 0) {
			Status[i] = Status[i] - 1;
			codecRt = DecoderFrameDisplayed_CMVpu(codec, i);
			gcnm_printfmore("DecoderFrameDisplayed_CMVpu called for idx %d, ret %d\n", i, codecRt);
			if(codecRt != IPP_STATUS_NOERR) {
				gcnm_printf("Err: Call DecoderFrameDisplayed_CMVpu() fail in %s, ret %d\n", __FUNCTION__, codecRt);
				g_warning("Err: Call DecoderFrameDisplayed_CMVpu fail in %s, ret %d", __FUNCTION__, codecRt);
				ret = -1;
				break;
			}
		}
		if(ret != 0) {
			break;
		}
	}
	g_mutex_unlock(mgr->mtx);
	return ret;
}

static int gcnmdec_summary_downbufdisplayed(CNMDec_ShareFrameManager* mgr, int* pSinkOwnedDecFBCnt)
{
	int i;
	int sum = 0;
	g_mutex_lock(mgr->mtx);
	for(i=0;i<32;i++) {
		sum += mgr->DecOutFrameConsumeStatus[i];
	}
	*pSinkOwnedDecFBCnt = mgr->iSinkOwnedDecFrameBufCnt;
	g_mutex_unlock(mgr->mtx);
	return sum;
}

int gcnmdec_smartdecide_framealloc_cnt(int codec_mincnt, int codec_maxcnt, int maxsz, int onesz, int spec)
{
	int cnt;
	int addcnt;
	int divcnt = maxsz/onesz;
	gcnm_printf("%s is called, parameter %d, %d, %d, %d\n", __FUNCTION__, codec_mincnt, codec_maxcnt, maxsz, onesz);
	if(divcnt < codec_mincnt) {
		return -1;  //fail
	}
	if(onesz <= (640*480*3>>1)) {
		addcnt = 10;
	}else if(onesz <=(720*576*3>>1)) {
		addcnt = 8;
	}else{
		addcnt = 6;	//In GST, the max buffer number of queue plug-in after decoder plug-in is 5, and render plug-in probably will hold 1~2 frame. Therefore, allocate additional 6 buffer.
	}
	if(spec == IPPGSTCNMDEC_SPEC_VC1SPMP) {
		addcnt = addcnt + 1;	//For VC1SPMP, the frame buffer in c&m codec doesn't obey "ring list" rule, in other word, the usage order for VC1SPMP frame buffer is out of order to some degree. Therefore, allocate more buffer is more safe.
	}
	cnt = codec_mincnt + addcnt;
	if(cnt > codec_maxcnt) {
		cnt = codec_maxcnt;
	}
	if(cnt > divcnt) {
		cnt = divcnt;
	}
	gcnm_printf("%s decide frame buffer count %d\n", __FUNCTION__, cnt);
	return cnt;
}

//seek code 0x000001XX
unsigned char* seek_SC_000001xx(unsigned char* pData, int len)
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

CnmPContMem* new_cnmFrameBuf(int sz, int bCacheable)
{
	void* vA;
	unsigned int pA;
	CnmPContMem* mem;
	
	if(bCacheable) {
		return NULL;	//currently, not support it	
	}

	PhyContMemInfo* memInfo = g_slice_new(PhyContMemInfo);

	memInfo->iAlign = IPP_CMVPUDEC_FRAMEBUF_ALIGNMENT;
	memInfo->iSize = sz;

	if(cnm_os_api_pcontmem_alloc(memInfo) != 0) {
		g_slice_free(PhyContMemInfo, memInfo);
		return NULL;
	}

	vA = memInfo->virAddr;
	pA = memInfo->phyAddr;

	mem = g_slice_new(CnmPContMem);
	mem->VAddr = vA;
	mem->PAddr = pA;
	mem->pOtherData1 = (void*)memInfo;
	gcnm_printf("%s() is called for (%d, %d), ret %p(vA:%p, pA:%p)\n", __FUNCTION__, sz, bCacheable, (void*)mem, (void*)mem->VAddr, (void*)mem->PAddr);

	return mem;
}


int delete_cnmFrameBuf(CnmPContMem* mem)
{
	gcnm_printf("%s() is called for %p\n", __FUNCTION__, (void*)mem);
	if(mem == NULL || mem->VAddr == NULL) {
		return -1;
	}
	cnm_os_api_pcontmem_free(mem->pOtherData1);
	g_slice_free(PhyContMemInfo, mem->pOtherData1);
	g_slice_free(CnmPContMem, mem);

	return 0;
}

CnmPContMem* new_cnmStmBuf(int sz)
{
	void* vA;
	unsigned int pA;
	CnmPContMem* mem;

	PhyContMemInfo* memInfo = g_slice_new(PhyContMemInfo);
	
	memInfo->iAlign = IPP_CMVPUDEC_STREAMBUF_ALIGNMENT;
	memInfo->iSize = sz;

	if(cnm_os_api_pcontmem_alloc(memInfo) != 0) {
		g_slice_free(PhyContMemInfo, memInfo);
		return NULL;
	}
	
	vA = memInfo->virAddr;
	pA = memInfo->phyAddr;

	
	mem = g_slice_new(CnmPContMem);
	mem->VAddr = vA;
	mem->PAddr = pA;
	mem->pOtherData1 = (void*)memInfo;
	gcnm_printf("%s() is called for (%d), ret %p(vA:%p, pA:%p)\n", __FUNCTION__, sz, (void*)mem, (void*)mem->VAddr, (void*)mem->PAddr);

	return mem;

}

int delete_cnmStmBuf(CnmPContMem* mem)
{
	gcnm_printf("%s() is called for %p\n", __FUNCTION__, (void*)mem);
	if(mem == NULL || mem->VAddr == NULL) {
		return -1;
	}
	cnm_os_api_pcontmem_free(mem->pOtherData1);
	g_slice_free(PhyContMemInfo, mem->pOtherData1);
	g_slice_free(CnmPContMem, mem);
	return 0;
}


int wait_sink_retDecFrameBuf(int iTimeOut, int flag, CNMDec_ShareFrameManager* mgr, CNMDecEleState* EleState)
{
	int totalwaittime;
	int SinkDisplayedCnt, SinkNotDisplayedCnt;
	int state;

	gcnm_printfmore("enter %s().\n", __FUNCTION__);

	if(iTimeOut == -1) {
		totalwaittime = 1;
	}else{
		totalwaittime = iTimeOut;
	}

	for(;;) {
		SinkDisplayedCnt = gcnmdec_summary_downbufdisplayed(mgr, &SinkNotDisplayedCnt);
		//at first, check whether some frame are returned from sink
		if(SinkDisplayedCnt > 0) {
			gcnm_printfmore("leave %s(), sink released %d frame.\n", __FUNCTION__, SinkDisplayedCnt);
			return 0;
		}
		//if no frame are returned and no share frame are owned by sink, return fatal error
		if(SinkNotDisplayedCnt == 0) {
			gcnm_printf("Err: Sink do not own any dec frame buffer.\n");
			g_warning("Err: Sink do not own any dec frame buffer.");
			return -2;
		}
		if(flag > 0 && SinkNotDisplayedCnt <= flag) {
			//some frame are owned by sink or other downstream elements, and those frames probably couldn't be return to decoder automatically, need to flush them
			return -3;
		}

		get_ElementState(EleState, state);

		if(state < CNMELESTATE_RUNNING) {
			gcnm_printf("give up waiting sink return frame buffer because element state is %d\n", state);
			return 1;	//give up waiting
		}

		usleep(CHECK_DOWNSTREAM_DISPLAYFRAME_PERIOD);
		if(iTimeOut != -1) {
			totalwaittime -= CHECK_DOWNSTREAM_DISPLAYFRAME_PERIOD;
		}

		if(totalwaittime < 0) {
			gcnm_printf("Err: wait down stream plug-in displayed frame timeout\n");
			g_warning("Err: wait down stream plug-in displayed frame timeout");
			return -1;
		}
	}
	return 0;
}

//pick up a decoder frame buffer which isn't hold by sink for buffer share enabled case
int pickup_DFBs_fromeRepo(CNMDec_ShareFrameManager* mgr, int wantCnt, int pickedIdx[])
{
	unsigned int i;
	int j=0;
	gcnm_printf("+++ %s(,%d,) is called at line %d +++\n",__FUNCTION__, wantCnt, __LINE__);
	g_mutex_lock(mgr->mtx);
	for(i=0;i<sizeof(mgr->DFBShareArr)/sizeof(mgr->DFBShareArr[0]);i++) {
		if(mgr->DFBShareArr[i].pUserData1) {
			if(mgr->DFBShareArr[i].pUserData3 == (void*)0) {
				gcnm_printf("found an idle allcoated node in DFBShareArr(%d)---.\n", i);
				pickedIdx[j++] = i;
				if(j == wantCnt) {
					gcnm_printf("found all needed nodes(cnt %d) in DFBShareArr---.\n", wantCnt);
					break;
				}
			}
		}
	}
	g_mutex_unlock(mgr->mtx);
	return j;
}

int prepare_framebufforcodec(int iHeight, int yBufStride, int* piPreparedCnt, int needPrepareCnt, CNMDec_ShareFrameManager* mgr, IppCMVpuDecFrameAddrInfo CodecBufArr[], int* pCodecBufLen, int bShareBuf, int bCacheableBuf)
{
	int i;
	int ybufsz = iHeight*yBufStride;
	int framebufsz = ybufsz + (ybufsz>>1);
	unsigned char* pFBuf;
	CnmPContMem* mem;
	unsigned int PA;
	if(!bShareBuf) {
		if(*pCodecBufLen == 0) {
			//allocate frame buffers
			gcnm_printf("Will allocate %d frame buf, each size %d\n", needPrepareCnt, framebufsz);

			*piPreparedCnt = 0;
			for(i=0; i<needPrepareCnt; i++) {
				mem = new_cnmFrameBuf(framebufsz, bCacheableBuf);
				if(mem == NULL) {
					g_warning("Err: allocate frame buf(%d) fail.", i);

					//rollback to free memory
					int j;
					for(j=i-1; j>=0; j--) {
						delete_cnmFrameBuf((CnmPContMem*)CodecBufArr[j].phyY);
					}
					memset(&(CodecBufArr[0]), 0, i*sizeof(IppCMVpuDecFrameAddrInfo));
					*piPreparedCnt = 0;
					*pCodecBufLen = 0;

					return -1;
				}

				pFBuf = mem->VAddr;
				PA = mem->PAddr;
				CodecBufArr[i].phyY = (unsigned int)mem;	//NOTE: it's a trick, phyY only is useful when register buffer to codec
				CodecBufArr[i].phyU = PA + ybufsz;
				CodecBufArr[i].phyV = PA + ybufsz + (ybufsz>>2);
				CodecBufArr[i].pUserData0 = (void*)yBufStride;
				CodecBufArr[i].pUserData1 = (void*)pFBuf;
				CodecBufArr[i].pUserData2 = (void*)(pFBuf + ybufsz);
				CodecBufArr[i].pUserData3 = (void*)(pFBuf + ybufsz + (ybufsz>>2));

				(*piPreparedCnt)++;
				(*pCodecBufLen)++;
				gcnm_printf("allocate frame buffer idx %d for non-sharebuf mode\n", i);
			}
		}else{
			gcnm_printf("frame buffer already prepared %d\n", *pCodecBufLen);
			*piPreparedCnt = *pCodecBufLen;
		}
	}else{
		int pickedcnt;
		int pickedArr[32];
		pickedcnt = pickup_DFBs_fromeRepo(mgr, needPrepareCnt, pickedArr);
		gcnm_printf("pick up %d buffers from repo, need new %d buffers\n", pickedcnt, needPrepareCnt-pickedcnt);
		for(i=0;i<pickedcnt;i++) {
			memcpy(&(CodecBufArr[i]), &(mgr->DFBShareArr[pickedArr[i]]), sizeof(IppCMVpuDecFrameAddrInfo));
			CodecBufArr[i].pUserData2 = (void*)(pickedArr[i]<<16);
		}
		*piPreparedCnt = pickedcnt;
		for(i=pickedcnt; i<needPrepareCnt; i++) {

			mem = new_cnmFrameBuf(framebufsz, bCacheableBuf);
			if(mem == NULL) {
				g_warning("Err: allocate frame buf(%d) fail.", i);

				//rollback to free memory
				int j;
				for(j=i-1; j>=pickedcnt; j--) {
					delete_cnmFrameBuf((CnmPContMem*)CodecBufArr[j].phyY);
				}
				memset(&(CodecBufArr[0]), 0, i*sizeof(IppCMVpuDecFrameAddrInfo));
				*piPreparedCnt = 0;

				return -1;
			}
			pFBuf = mem->VAddr;
			PA = mem->PAddr;

			(*piPreparedCnt)++;
			CodecBufArr[i].phyY = (unsigned int)mem;
			CodecBufArr[i].phyU = PA + ybufsz;
			CodecBufArr[i].phyV = PA + ybufsz + (ybufsz>>2);
			CodecBufArr[i].pUserData0 = (void*)yBufStride;
			CodecBufArr[i].pUserData1 = (void*)pFBuf;

			gcnm_printf("allocate frame buffer idx %d for sharebuf mode suc\n", i);
		}
		
		for(i=pickedcnt; i<needPrepareCnt; i++) {
			gcnm_printf("install frame buffer(idx %d for sharebuf mode) into DFBShareArr(%d)\n", i, mgr->iDFBShareLen);
			if(mgr->iDFBShareLen >= (int)(sizeof(mgr->DFBShareArr)/sizeof(mgr->DFBShareArr[0]))) {
				g_warning("Err: the frame buffer count exceeds the share frame repo capacity!");
				return -2;
			}
			memcpy(&(mgr->DFBShareArr[mgr->iDFBShareLen]), &(CodecBufArr[i]), sizeof(IppCMVpuDecFrameAddrInfo));
			CodecBufArr[i].pUserData2 = (void*)(mgr->iDFBShareLen<<16);
			mgr->DFBShareArr[mgr->iDFBShareLen].pUserData3 = (void*)0;
			mgr->iDFBShareLen++;
		}

	}
	return 0;

}


void gstcnmElement_clean_nonShareFramebuf(IppCMVpuDecFrameAddrInfo Arr[], int* pArrLen)
{
	int i;
	for(i=0;i<*pArrLen;i++) {
		if(Arr[i].phyY) {
			delete_cnmFrameBuf((CnmPContMem*)(Arr[i].phyY));
			memset(&(Arr[i]), 0, sizeof(IppCMVpuDecFrameAddrInfo));
		}
		*pArrLen = 0;
	}
	return;
}


void gstcnmElement_clean_idleShareFBs(CNMDec_ShareFrameManager* mgr, SkipFrameBufRepo* SFBRepo)
{
	unsigned int i;
	for(i=0;i<sizeof(mgr->DFBShareArr)/sizeof(mgr->DFBShareArr[0]);i++) {
		if(mgr->DFBShareArr[i].pUserData3 == (void*)0 && mgr->DFBShareArr[i].phyY) {
			delete_cnmFrameBuf((CnmPContMem*)(mgr->DFBShareArr[i].phyY));
			memset(&(mgr->DFBShareArr[i]), 0, sizeof(mgr->DFBShareArr[0]));
		}
	}
	if(SFBRepo) {
		for(i=0;i<sizeof(SFBRepo->SFBs)/sizeof(SFBRepo->SFBs[0]);i++) {
			if(SFBRepo->SFBs[i].pUserData0 == (void*)0 && SFBRepo->SFBs[i].phyY) {
				delete_cnmFrameBuf((CnmPContMem*)(SFBRepo->SFBs[i].phyY));
				memset(&(SFBRepo->SFBs[i]), 0, sizeof(SFBRepo->SFBs[0]));
			}
		}
	}
	return;
}
