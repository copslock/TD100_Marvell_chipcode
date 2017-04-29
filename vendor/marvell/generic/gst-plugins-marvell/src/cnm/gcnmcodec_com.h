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
/* gcnmcodec_com.h */

#ifndef __GCNMCODEC_COM_H__
#define __GCNMCODEC_COM_H__

#include "ippGST_sideinfo.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NOTECHO_BUFFERFULL_WARNING		//not echo warnning when codec return BUFFER_FULL


#if 0
#define gcnm_printf(...)		printf(__VA_ARGS__)
#else
#define gcnm_printf(...)
#endif

#if 0
#define gcnm_printfmore(...)	printf(__VA_ARGS__)
#else
#define gcnm_printfmore(...)
#endif

#define IPP_CMVPUDEC_STREAMBUF_ALIGNMENT	4
#define IPP_CMVPUDEC_FRAMEBUF_ALIGNMENT 	8

#define IS_CNMDEC_FATALERR(codecErrStatus)  (((codecErrStatus) & 0xffff0000) != 0)
#define WARN_CNMDEC_EOS 					0x1
#define WARN_CNMDEC_FRAMENOTSUPPORT 		0x2
#define WARN_CNMDEC_FRAMECORRUPT			0x4
#define WARN_WAITSINKRETFRAME_PUSHDUMMY_AVOID_DEADLOCK	0x8
#define ERR_CNMDEC_DISPLAYEDAPI 			0x10000
#define ERR_CNMDEC_INIT 					0x20000
#define ERR_CNMDEC_FILLSSTMBUFAPI   		0x40000
#define ERR_CNMDEC_PARSESEQHDR  			0x80000
#define ERR_CNMDEC_REGFRAMEBUF  			0x100000
#define ERR_CNMDEC_DECFRAME_SERIOUSERR  	0x200000
#define ERR_CNMDEC_DEINIT   				0x400000
#define ERR_WAITSINKRETFRAME_TIMEOUT		0x10000000
#define ERR_WAITSINKRETFRAME_SINKNOFRAME	0x20000000

typedef struct{
	void* VAddr;
	unsigned int PAddr;
	void* pOtherData0;
	void* pOtherData1;
	void* pOtherData2;
	void* pOtherData3;
}CnmPContMem;

typedef struct{
	unsigned int BufPhyAddr;
	unsigned char* BufVirAddr;
	int iBufSize;
	int iDataLen;
	CnmPContMem* mem;
}GCNMDec_StmBuf;

typedef struct{
	int iDownFrameWidth;
	int iDownFrameHeight;
	int iDownbufYpitch;
	int iDownbufUVpitch;
	int iDownbufUoffset;
	int iDownbufVoffset;
	int iDownbufSz;
}GCNMDec_DownBufLayout;

typedef struct{
	GstClockTime TS;
	GstClockTime DU;
}GCNMDec_TsDuPair;

typedef struct{
	GSList* 		TsDuList;
	int 			iTsDuListLen;
	GstClockTime	LastOutputTs;
	GstClockTime	LastOutputDu;
	GstClockTime	FrameFixPeriod;
	gint64			SegmentStartTs;
}GCNMDec_TSDUManager;

typedef struct{
	IppCMVpuDecInitPar* pInitPar;
	IppCMVpuDecParsedSeqInfo* pSeqInfo;
	int iFilledLen_forCodecStart;
	int (*allocate_framebuf)(void* pElement, IppCMVpuDecParsedSeqInfo* pSeqInfo, int* piFrameCnt, IppCMVpuDecFrameAddrInfo** pFrameArr, int* piFrameStride);

	void* pCnmDec;
	int iCodecApiErr;
	unsigned char* virAddrStaticBitstreamBuf;	//virtual address of pInitPar->phyAddrStaticBitstreamBuf
	GCNMDec_StmBuf* StmBufPool;
	int iMemoryErr;
}GCNMDec_StartUpDec;

#define gcnmdec_fill_stream(pStm, pSrc, iSrcLen) {  				\
	if((iSrcLen) > 0) {													\
		memcpy((pStm)->BufVirAddr+(pStm)->iDataLen, (pSrc), (iSrcLen)); \
		(pStm)->iDataLen += (iSrcLen);  								\
	}																	\
}

int gcnmdec_select_stream(GCNMDec_StmBuf StmPool[], int iPoolSz, int iWantSz);
void gcnmdec_update_downbuf_layout(int w, int h, int category, GCNMDec_DownBufLayout* pDownBufLayout);
GstFlowReturn gcnmdec_require_framebuf_fromdownplugin(GCNMDec_DownBufLayout* pDownBufLayout, GstPad* srcPad, GstBuffer** ppGstBuf, int bAutoToGSTStandardI420);
void gcnmdec_copy_to_downframebuf(unsigned char* pDst, GCNMDec_DownBufLayout* pDownBufLayout, IppCMVpuDecFrameAddrInfo* pCNMFrame, IppiRect* pCNMFrameRoi);

void gcnmdec_init_TsDuMgr(GCNMDec_TSDUManager* pTsDuMgr);
void gcnmdec_remove_redundant_TsDu(GCNMDec_TSDUManager* pTsDuMgr, int leftlen);
void gcnmdec_insert_TsDu(GCNMDec_TSDUManager* pTsDuMgr, GstBuffer* buf);
void gcnmdec_refresh_output_TsDu(GCNMDec_TSDUManager* pTsDuMgr);

typedef void*   							FRAMES_WATCHMAN;
#define FRAMES_WATCHMAN_REF(watcher)		gst_buffer_ref(watcher)
#define FRAMES_WATCHMAN_UNREF(watcher)  	gst_buffer_unref(watcher)

#define MAX_CAPACITY_SFBREPO	13
typedef struct {
	IppCMVpuDecFrameAddrInfo SFBs[MAX_CAPACITY_SFBREPO];	//array to implement this repo
	int len;
	IPPGSTDecDownBufSideInfo sideinfoSFBs[MAX_CAPACITY_SFBREPO];
}SkipFrameBufRepo;

typedef void (*cnm_callback_echo)(void* ele, const char* fmtstr, int par0, int par1);
typedef struct{
	FRAMES_WATCHMAN watcher;
	int DecOutFrameConsumeStatus[32];   //Should use some sync mechanism like mutex to protect DecOutFrameConsumeStatus[], because probably multiple thread will access it
	unsigned int CurCodecSerialNum;	//only 20bit valid. Ubuntu totem player will repeat play a file. When begin play the file again, probably, the shared decoder frame buffer hasn't been consumed by sink but the codec has been closed and reopened. Therefore, we must know which codec is related with the shared decoder frame buffer, only call DecoderFrameDisplayed_CMVpu() for frame buffer related with current codec. Anyway, to solve this problem totally, must modify frame buffer management mechanism.
	IppCMVpuDecFrameAddrInfo DFBShareArr[64];	//DFBShareArr[x].pUserData3 == 0 means display sink do not hold this frame, 1 means display sink is holding this frame.
	int iDFBShareLen;
	int NotNeedFMemAnyMore;			//once this flag is set, it means decoder plug-in needn't the frame buffer memory any more, but the sink probably still need the buffer
	GMutex* mtx;
	int iSinkOwnedDecFrameBufCnt;
	SkipFrameBufRepo* wmvShareSFBRepo;
	int iSinkOwnedSFBCnt;		//NOTE: it's for SkipFrameBufRepo
	IPPGSTDecDownBufSideInfo sideinfoDFBShareArr[64];
	void* ele;
	cnm_callback_echo pfun_echo;
}CNMDec_ShareFrameManager;

#define CNMELESTATE_RUNNING		16	//running state include paused state and playing state
#define CNMELESTATE_RESTING		0
typedef struct{
	GMutex* elestate_mtx;
	int	state;
}CNMDecEleState;


int gcnmdec_startup_codec(void* pElement, GCNMDec_StartUpDec* pStart, CNMDec_ShareFrameManager* mgr);
int gcnmdec_close_codec(void** ppCnmDec, CNMDec_ShareFrameManager* mgr);

FRAMES_WATCHMAN gcnmdec_watcher_create(gpointer ele, void (*dothing_atwatcherdead)(gpointer), CNMDec_ShareFrameManager* mgr);
void gcnmdec_watcher_onfinalize(gpointer ctrl);

void gcnmdec_downbuf_onfinalize(gpointer info);

int gcnmdec_informcodec_decframeconsumed(void* codec, CNMDec_ShareFrameManager* mgr);

#define CHECK_DOWNSTREAM_DISPLAYFRAME_PERIOD		5000   	//in usecond
#define WAIT_DOWNSTREAM_RETBUF_TIMEOUT				2500000	//in usecond
int wait_sink_retDecFrameBuf(int iTimeOut, int flag, CNMDec_ShareFrameManager* mgr, CNMDecEleState* EleState);

#define IPPGST_CMVPUDEC_FRAMEBUF_DEFAULT_MAXTOTALSIZE   	(22*1024*1024)

#define IPPGSTCNMDEC_SPEC_H263		1
#define IPPGSTCNMDEC_SPEC_H264		2
#define IPPGSTCNMDEC_SPEC_MPEG2		3
#define IPPGSTCNMDEC_SPEC_MPEG4		4
#define IPPGSTCNMDEC_SPEC_VC1SPMP	5
int gcnmdec_smartdecide_framealloc_cnt(int codec_mincnt, int codec_maxcnt, int maxsz, int onesz, int spec);

unsigned char* seek_SC_000001xx(unsigned char* pData, int len);


CnmPContMem* new_cnmFrameBuf(int sz, int bCacheable);
int delete_cnmFrameBuf(CnmPContMem* mem);
CnmPContMem* new_cnmStmBuf(int sz);
int delete_cnmStmBuf(CnmPContMem* mem);

void gstcnmElement_clean_nonShareFramebuf(IppCMVpuDecFrameAddrInfo Arr[], int* pArrLen);
void gstcnmElement_clean_idleShareFBs(CNMDec_ShareFrameManager* mgr, SkipFrameBufRepo* SFBRepo);

int prepare_framebufforcodec(int iHeight, int yBufStride, int* piPreparedCnt, int needPrepareCnt, CNMDec_ShareFrameManager* mgr, IppCMVpuDecFrameAddrInfo CodecBufArr[], int* pCodecBufLen, int bShareBuf, int bCacheableBuf);

static __inline IppCodecStatus
HibernateWakeupCNM(void * pCNMDecObj, int bHibernate, int* pIsInDream)
{
	IppCodecStatus cdRet;
	if(bHibernate) {
		cdRet = DecoderHibernate_CMVpu(pCNMDecObj, 3);
	}else{
		cdRet = DecoderWakeup_CMVpu(pCNMDecObj);
	}
	*pIsInDream = bHibernate;
	return cdRet;
}

#define set_ElementState(ES, newstate)						\
{															\
	g_mutex_lock(((CNMDecEleState*)(ES))->elestate_mtx);	\
	((CNMDecEleState*)(ES))->state = newstate;				\
	g_mutex_unlock(((CNMDecEleState*)(ES))->elestate_mtx);	\
	gcnm_printf("set element state to %d\n", newstate);		\
}

#define get_ElementState(ES, state)							\
{															\
	g_mutex_lock(((CNMDecEleState*)(ES))->elestate_mtx);	\
	state = ((CNMDecEleState*)(ES))->state;					\
	g_mutex_unlock(((CNMDecEleState*)(ES))->elestate_mtx);	\
	gcnm_printfmore("get element state %d\n", state);		\
}

static __inline void setSideInfo(IPPGSTDecDownBufSideInfo* sinfo, int x_off, int y_off, int x_stride, int y_stride, unsigned int PA, void* VA)
{
	sinfo->x_off = x_off;
	sinfo->y_off = y_off;
	sinfo->x_stride = x_stride;
	sinfo->y_stride = y_stride;
	sinfo->phyAddr = PA;
	sinfo->virAddr = VA;
	return;
}

#ifdef __cplusplus
}
#endif

#endif

/* EOF */
