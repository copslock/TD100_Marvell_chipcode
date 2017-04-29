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

/* gstcnmdec_h263.c */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h> //to include memset(),...
#include <sys/time.h>	//to include time functions and structures

#include "gstcnmdec_h263.h"
#include "cnm_bridge.h"
#include "ippGSTdefs.h"

//#define LOG_FRAMECOPYTIME

typedef struct{
	struct timeval tstart;
	struct timeval tend;
	struct timezone tz;
	struct timeval tlog_0;
	struct timeval tlog_1;
	unsigned int log_copytime;
	unsigned int log_copycnt;
}CnmH263ProbeData;

static const unsigned char H263NALSyncCode[4] = {0,0,0,1};

GST_DEBUG_CATEGORY_STATIC (cnmdec_h263_debug);
#define GST_CAT_DEFAULT cnmdec_h263_debug

enum {

	/* fill above with user defined signals */
	LAST_SIGNAL
};

enum {
	ARG_0,
	/* fill below with user defined arguments */
	ARG_I420LAYOUTKIND,
	ARG_SHAREFRAMEBUF,
	ARG_TIMEOUTFORDISP1FRAME,
	ARG_SUPPORTMULTIINS,
	ARG_IGNOREPBPICNEWSEG,
	ARG_ENABLEHIBERNATEATPAUSE,
	ARG_ENABLEDYNACLK,

#ifdef DEBUG_PERFORMANCE
	ARG_TOTALFRAME,
	ARG_CODECTIME,
#endif

};

//CNM support height < 1024 and should be even, therefore 1022
static GstStaticPadTemplate sink_factory =
	GST_STATIC_PAD_TEMPLATE ("sink", GST_PAD_SINK, GST_PAD_ALWAYS,
	GST_STATIC_CAPS ("video/x-h263, "
					 "width = (int) [ 16, 1280 ], "
					 "height = (int) [ 16, 1022 ], "
					 "framerate = (fraction) [ 0, MAX ], "
					 "cnmnotsupport = (int) 0")
					 );

static GstStaticPadTemplate src_factory =
	GST_STATIC_PAD_TEMPLATE ("src", GST_PAD_SRC, GST_PAD_ALWAYS,
	GST_STATIC_CAPS ("video/x-raw-yuv,"
					 "format = (fourcc) { I420}, "
					 "width = (int) [ 16, 1280 ], "
					 "height = (int) [ 16, 1022 ], "
					 "framerate = (fraction) [ 0, MAX ]")
					 );

GST_BOILERPLATE (Gstcnmh263dec, gst_cnmh263dec, GstElement, GST_TYPE_ELEMENT);


static gboolean
gst_cnmh263dec_sinkpad_setcaps (GstPad *pad, GstCaps *caps)
{
	gcnm_printf("enter gst_cnmh263dec_sinkpad_setcaps().\n");

	Gstcnmh263dec *h263 = GST_CNMH263DEC (GST_OBJECT_PARENT (pad));
	int i = 0;
	GstStructure *str;
	const GValue *value;

	int stru_num = gst_caps_get_size(caps);

	gcnm_printf("stru_num =%d\n",stru_num);

	if(stru_num != 1) {
		gchar* sstr;
		g_warning("Multiple MIME stream type in sinkpad caps, only select the first item.");
		for(i=0; i<stru_num; i++) {
			str = gst_caps_get_structure(caps, i);
			sstr = gst_structure_to_string(str);
			g_warning("struture %d is %s.", i, sstr);
			g_free(sstr);
		}
	}
	str = gst_caps_get_structure(caps, 0);

	if(G_UNLIKELY(FALSE == gst_structure_get_fraction(str, "framerate", &h263->num, &h263->den))) {
		h263->num = 0;  	//if this information isn't in caps, we assume it is 0
		h263->den = 1;
	}

	gcnm_printf("fps: %d/%d\n", h263->num, h263->den);

	if(h263->num != 0) {
		h263->TsDuManager.FrameFixPeriod = gst_util_uint64_scale_int(GST_SECOND, h263->den, h263->num);
	}

	gcnm_printf("frame duration %lld ns\n",  h263->TsDuManager.FrameFixPeriod);



	if ((value = gst_structure_get_value (str, "codec_data"))){
		guint8 *cdata;
		guint csize;
		GstBuffer *buf;


		gcnm_printf("have codec_data\n");

		buf = gst_value_get_buffer(value);
		cdata = GST_BUFFER_DATA (buf);
		csize = GST_BUFFER_SIZE (buf);

		gcnm_printf("codec data size %d\n", csize);

		if(h263->seqMeta== NULL) {
			h263->seqMeta = g_malloc(csize);
			if(h263->seqMeta == NULL) {
				gcnm_printf("Malloc seqMeta(,%d,) fail.\n", csize);
				return FALSE;
			}else{
				h263->seqMetaBufSz = csize;
				gcnm_printf("Create sequence internal stream buffer 0x%x, size %d\n", (unsigned int)h263->seqMeta, csize);
			}
		}else if((unsigned int)h263->seqMetaBufSz < csize) {
			h263->seqMeta = g_realloc(h263->seqMeta, csize);
			if(h263->seqMeta == NULL) {
				gcnm_printf("Malloc seqMeta(,%d,) fail.\n", csize);
				return FALSE;
			}else{
				h263->seqMetaBufSz = csize;
				gcnm_printf("Re-Create sequence internal stream buffer 0x%x, size %d\n", (unsigned int)h263->seqMeta, csize);
			}
		}
		memcpy(h263->seqMeta, cdata, csize);
		h263->seqMetaLen = csize;

	}

	gcnm_printf("exit gst_cnmh263dec_sinkpad_setcaps() succeed\n");
	return TRUE;
}

static void
gst_cnmh263dec_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{

	Gstcnmh263dec *h263 = GST_CNMH263DEC (object);

	switch (prop_id)
	{
	case ARG_I420LAYOUTKIND:
		{
			int layout = g_value_get_int(value);
			if(layout < 0 || layout > 2) {
				GST_ERROR_OBJECT(h263, "I420layoutkind %d exceed range.", layout);
				layout = 0;
			}
			h263->iI420LayoutKind = layout;
		}
		break;
	case ARG_SHAREFRAMEBUF:
		{
			int sha = g_value_get_int(value);
			if(sha < 0 || sha > 1) {
				GST_ERROR_OBJECT(h263, "shareFramebuf %d exceed range.", sha);
				sha = 1;
			}
			h263->iShareFrameBuf = sha;
		}
		break;
	case ARG_TIMEOUTFORDISP1FRAME:
		{
			int to = g_value_get_int(value);
			if(to < -1) {
				GST_ERROR_OBJECT(h263, "timeoutDisp1frame %d exceed range.", to);
				to = -1;
			}
			h263->iTimeoutForDisp1Frame = to;
		}
		break;
	case ARG_SUPPORTMULTIINS:
		{
			int supm = g_value_get_int(value);
			if(supm < 0 || supm > 2) {
				GST_ERROR_OBJECT(h263, "supportMultiIns %d exceed range.", supm);
				supm = 0;
			}
			h263->iSupportMultiIns = supm;
		}
		break;
	case ARG_IGNOREPBPICNEWSEG:
		{
			int ig = g_value_get_int(value);
			if(ig < 0 || ig > 1) {
				GST_ERROR_OBJECT(h263, "ignorePBInNewseg %d exceed range.", ig);
				ig = 0;
			}
			h263->iIgnPBInNewseg = ig;
		}
		break;
	case ARG_ENABLEHIBERNATEATPAUSE:
		{
			int eh = g_value_get_int(value);
			if(eh < 0 || eh > 1) {
				GST_ERROR_OBJECT(h263, "cnmHibernateAtPause %d exceed range.", eh);
				eh = 0;
			}
			h263->bEnableHibernateAtPause = eh;
		}
		break;
	case ARG_ENABLEDYNACLK:
		{
			int ec = g_value_get_int(value);
			if(ec < 0 || ec > 1) {
				GST_ERROR_OBJECT(h263, "dynamicClock %d exceed range.", ec);
				ec = 0;
			}
			h263->bEnableDynaClk = ec;
		}
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
	return;
}

static void
gst_cnmh263dec_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	Gstcnmh263dec *h263 = GST_CNMH263DEC(object);

	switch (prop_id)
	{
	case ARG_I420LAYOUTKIND:
		g_value_set_int(value, h263->iI420LayoutKind);
		break;
	case ARG_SHAREFRAMEBUF:
		g_value_set_int(value, h263->iShareFrameBuf);
		break;
	case ARG_TIMEOUTFORDISP1FRAME:
		g_value_set_int(value, h263->iTimeoutForDisp1Frame);
		break;
	case ARG_SUPPORTMULTIINS:
		g_value_set_int(value, h263->iSupportMultiIns);
		break;
	case ARG_IGNOREPBPICNEWSEG:
		g_value_set_int(value, h263->iIgnPBInNewseg);
		break;
	case ARG_ENABLEHIBERNATEATPAUSE:
		g_value_set_int(value, h263->bEnableHibernateAtPause);
		break;
	case ARG_ENABLEDYNACLK:
		g_value_set_int(value, h263->bEnableDynaClk);
		break;

#ifdef DEBUG_PERFORMANCE
	case ARG_TOTALFRAME:
		g_value_set_int(value, h263->totalFrames);
		break;
	case ARG_CODECTIME:
		g_value_set_int64(value, h263->codec_time);
		break;
#endif

	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
	return;
}

static __inline void _cnmh263_refresh_downpush_TsDu(Gstcnmh263dec* h263)
{
	//gcnmdec_remove_redundant_TsDu(&h263->TsDuManager, h263->SeqInfo.MaxReorderDelay+1);
	//since no reorder, reorder delay should always be 0
	gcnmdec_remove_redundant_TsDu(&h263->TsDuManager, 1);
	gcnmdec_refresh_output_TsDu(&h263->TsDuManager);
	return;
}

static GstFlowReturn _cnmh263_push_data(Gstcnmh263dec *h263, int iDecOutFrameIdx);

static gboolean
gst_cnmh263dec_sinkpad_event (GstPad *pad, GstEvent *event)
{
	gcnm_printf("enter %s, event 0x%x\n", __func__, GST_EVENT_TYPE(event));

	gboolean eventRet;
	Gstcnmh263dec *h263 = GST_CNMH263DEC(GST_OBJECT_PARENT(pad));

	switch(GST_EVENT_TYPE (event))
	{
	case GST_EVENT_NEWSEGMENT:
		gcnm_printf("NewSeg event is received.\n");
		{
			gcnmdec_remove_redundant_TsDu(&h263->TsDuManager, 0);
			h263->TsDuManager.LastOutputTs = GST_CLOCK_TIME_NONE;
			h263->TsDuManager.LastOutputDu = GST_CLOCK_TIME_NONE;

			if(h263->iIgnPBInNewseg) {
				h263->bWaitIPicture = 1;
			}

			eventRet = gst_pad_push_event(h263->srcpad, event);
			h263->bNewSegReceivedAfterflush = 1;
		}
		break;
	case GST_EVENT_EOS:
		gcnm_printf("EOS event is received.\n");
		{
			//for h263&mpeg4, always no-reorder mode and fileplay mode, when EOS coming, all compressed data are decoded by decoder, and all frame are outputted. Therefore, could close codec.

			g_mutex_lock(h263->cnmCodecMtx);
			if(h263->pH263DecoderState) {
				if(h263->bCnmInDream) {
					HibernateWakeupCNM(h263->pH263DecoderState, 0, &h263->bCnmInDream);
				}
				h263->iCodecErr |= gcnmdec_close_codec(&h263->pH263DecoderState, &h263->ShareBufManager);
			}
			g_mutex_unlock(h263->cnmCodecMtx);

			h263->b1stFrame_forFileplay = 1;
			h263->bCodecSucLaunched = 0;
			h263->bWaitIPicture = 1;
			gcnmdec_remove_redundant_TsDu(&h263->TsDuManager, 0);
			h263->TsDuManager.LastOutputTs = GST_CLOCK_TIME_NONE;
			h263->TsDuManager.LastOutputDu = GST_CLOCK_TIME_NONE;
			eventRet = gst_pad_push_event(h263->srcpad, event);
		}
		break;
	case GST_EVENT_FLUSH_STOP:
		h263->bNewSegReceivedAfterflush = 0;
	default:
		eventRet = gst_pad_event_default(pad, event);
		break;
	}

	return eventRet;
}

static GstFlowReturn _cnmh263_push_data(Gstcnmh263dec* h263, int iDecOutFrameIdx)
{
	GstFlowReturn FlowRt = GST_FLOW_OK;
	GstBuffer *outBuf = NULL;

	gcnm_printfmore("enter %s\n", __func__);

	if(h263->iShareFrameBuf) {
		int idxInDFBShare;
		void* VA = h263->DecFramesInfoArr[iDecOutFrameIdx].pUserData1;
		unsigned int PA = (unsigned int)((CnmPContMem*)h263->DecFramesInfoArr[iDecOutFrameIdx].phyY)->PAddr;
		outBuf = gst_buffer_new();
		if(outBuf == NULL) {
			return GST_FLOW_ERROR;
		}
		GST_BUFFER_DATA(outBuf) = VA;
		GST_BUFFER_SIZE(outBuf) = h263->iAlignedBufSz;
		gst_buffer_set_caps(outBuf, GST_PAD_CAPS(h263->srcpad));
		idxInDFBShare = ((int)(h263->DecFramesInfoArr[iDecOutFrameIdx].pUserData2)>>16) & 0xff;
		IPPGST_BUFFER_CUSTOMDATA(outBuf) = (gpointer) (&h263->ShareBufManager.sideinfoDFBShareArr[idxInDFBShare]);
		setSideInfo(&h263->ShareBufManager.sideinfoDFBShareArr[idxInDFBShare], h263->SeqInfo.FrameROI.x, h263->SeqInfo.FrameROI.y, h263->SeqInfo.FrameAlignedWidth, h263->SeqInfo.FrameAlignedHeight, PA, VA);
		h263->DecFramesInfoArr[iDecOutFrameIdx].pUserData2 = (void*)((idxInDFBShare<<16)|h263->ShareBufManager.CurCodecSerialNum|(iDecOutFrameIdx<<24));
		h263->DecFramesInfoArr[iDecOutFrameIdx].pUserData3 = (void*)(&h263->ShareBufManager);
		GST_BUFFER_MALLOCDATA(outBuf) = (guint8*)(&(h263->DecFramesInfoArr[iDecOutFrameIdx]));
		GST_BUFFER_FREE_FUNC(outBuf) = (GFreeFunc)gcnmdec_downbuf_onfinalize;
		FRAMES_WATCHMAN_REF(h263->ShareBufManager.watcher);

		GST_BUFFER_TIMESTAMP(outBuf) = h263->TsDuManager.LastOutputTs;
		GST_BUFFER_DURATION(outBuf) = h263->TsDuManager.LastOutputDu;

		g_mutex_lock(h263->ShareBufManager.mtx);
		h263->ShareBufManager.DFBShareArr[idxInDFBShare].pUserData3 = (void*)1;	//DFBShareArr[x].pUserData3 == 0 means display sink do not hold this frame, 1 means display sink is holding this frame.
		h263->ShareBufManager.iSinkOwnedDecFrameBufCnt++;
		g_mutex_unlock(h263->ShareBufManager.mtx);

		if(h263->bNewSegReceivedAfterflush) {
			FlowRt = gst_pad_push(h263->srcpad, outBuf);
		}else{
			//If new segment event hasn't received, shouldn't push buffer to sink. Otherwise, sink will throw warnings and work abnormal.
			//Sometimes, user seeking position exceeds the duration of file, then demuxer only fire flush event without newseg event. And demuxer also fire EOS event. At EOS, decoder probably still output some frames, but those frames shouldn't be pushed to sink because no newseg event.
			gst_buffer_unref(outBuf);
			FlowRt = GST_FLOW_OK;
		}
		if(GST_FLOW_OK != FlowRt) {
			if(FlowRt != GST_FLOW_WRONG_STATE) {
				//during seeking, sink element often return GST_FLOW_WRONG_STATUS. No need to echo this message
				g_warning("gst_pad_push gstbuffer to downstream plugin fail, ret %d!", FlowRt);
			}
		}
		return FlowRt;
	}


	FlowRt = gcnmdec_require_framebuf_fromdownplugin(&h263->DownFrameLayout, h263->srcpad, &outBuf, h263->iI420LayoutKind==0);
	if(GST_FLOW_OK != FlowRt) {
		if(FlowRt != GST_FLOW_WRONG_STATE) {
			//during seeking, sink element often return GST_FLOW_WRONG_STATUS. No need to echo this message
			g_warning("Require gstbuffer from downstream plugin fail, ret %d!", FlowRt);
		}
		return FlowRt;
	}
#ifdef LOG_FRAMECOPYTIME
	gettimeofday(&((CnmH263ProbeData*)h263->pProbeData)->tlog_0, NULL);
#endif
	gcnmdec_copy_to_downframebuf(GST_BUFFER_DATA(outBuf), &h263->DownFrameLayout, &(h263->DecFramesInfoArr[iDecOutFrameIdx]), &(h263->SeqInfo.FrameROI));
#ifdef LOG_FRAMECOPYTIME
	gettimeofday(&((CnmH263ProbeData*)h263->pProbeData)->tlog_1, NULL);
	((CnmH263ProbeData*)h263->pProbeData)->log_copycnt++;
	((CnmH263ProbeData*)h263->pProbeData)->log_copytime += (((CnmH263ProbeData*)h263->pProbeData)->tlog_1.tv_sec - ((CnmH263ProbeData*)h263->pProbeData)->tlog_0.tv_sec)*1000000 + (((CnmH263ProbeData*)h263->pProbeData)->tlog_1.tv_usec - ((CnmH263ProbeData*)h263->pProbeData)->tlog_0.tv_usec);
#endif

	GST_BUFFER_TIMESTAMP(outBuf) = h263->TsDuManager.LastOutputTs;
	GST_BUFFER_DURATION(outBuf) = h263->TsDuManager.LastOutputDu;
	if(h263->bNewSegReceivedAfterflush) {
		FlowRt = gst_pad_push(h263->srcpad, outBuf);
	}else{
		//If new segment event hasn't received, shouldn't push buffer to sink. Otherwise, sink will throw warnings and work abnormal.
		//Sometimes, user seeking position exceeds the duration of file, then demuxer only fire flush event without newseg event. And demuxer also fire EOS event. At EOS, decoder probably still output some frames, but those frames shouldn't be pushed to sink because no newseg event.
		gst_buffer_unref(outBuf);
		FlowRt = GST_FLOW_OK;
	}
	if(GST_FLOW_OK != FlowRt) {
		if(FlowRt != GST_FLOW_WRONG_STATE) {
			//during seeking, sink element often return GST_FLOW_WRONG_STATUS. No need to echo this message
			g_warning("gst_pad_push gstbuffer to downstream plugin fail, ret %d!", FlowRt);
		}
	}
	return FlowRt;

}


static int _cnmh263dec_allocateframebufs(void* pElement, IppCMVpuDecParsedSeqInfo* pSeqInfo, int* piFrameCnt, IppCMVpuDecFrameAddrInfo** pFrameArr, int* piFrameStride)
{
	int codecframeNeedCnt;
	Gstcnmh263dec* ele = (Gstcnmh263dec*)pElement;
	int ret;
	int bCacheable;
	int framebufsz = (pSeqInfo->FrameAlignedWidth * pSeqInfo->FrameAlignedHeight * 3)>>1;

	*piFrameStride = pSeqInfo->FrameAlignedWidth;
	*pFrameArr = ele->DecFramesInfoArr;

	if(ele->iShareFrameBuf) {
		bCacheable = 0;
		codecframeNeedCnt = gcnmdec_smartdecide_framealloc_cnt(pSeqInfo->MinFrameBufferCount, IPPCMDEC_DECFRAMEBUF_MAX_H263MPEG2MPEG4, IPPGST_CMVPUDEC_FRAMEBUF_DEFAULT_MAXTOTALSIZE, framebufsz, IPPGSTCNMDEC_SPEC_H263);
	}else{
		bCacheable = 0;
		codecframeNeedCnt = pSeqInfo->MinFrameBufferCount;
	}

	ret = prepare_framebufforcodec(pSeqInfo->FrameAlignedHeight, pSeqInfo->FrameAlignedWidth, piFrameCnt, codecframeNeedCnt, &ele->ShareBufManager, ele->DecFramesInfoArr, &ele->iDecFrameBufArrLen, ele->iShareFrameBuf, bCacheable);

	return ret;
}

static int gst_cnmh263dec_launch_codec(GstBuffer* buf, Gstcnmh263dec* h263)
{
	GCNMDec_StartUpDec StartUp;
	int StartUpRet = 0;
	void* pSStmBuf = NULL;
	IppCMVpuDecInitPar InitPar;
	CnmPContMem* mem;

	memset(&InitPar, 0, sizeof(InitPar));
	InitPar.ReorderEnable = 0;
	InitPar.FilePlayEnable = 1;
	InitPar.VideoStreamFormat = IPP_VIDEO_STRM_FMT_H263;
	InitPar.MultiInstanceSync = h263->iSupportMultiIns;
	InitPar.DynamicClkEnable = h263->bEnableDynaClk;

	if(h263->StmBufPool[0].iBufSize == 0) {
		int framelenUpperSz;

		//prepare static stream buffer
		InitPar.StaticBitstreamBufSize = 384*1024;  //init a value
		framelenUpperSz = GST_BUFFER_SIZE(buf);


		if(InitPar.StaticBitstreamBufSize < (unsigned int)(framelenUpperSz + h263->seqMetaLen + IPPCMDEC_STATICSTMBUF_PROTECTSPACE)) {
			InitPar.StaticBitstreamBufSize = framelenUpperSz + h263->seqMetaLen + IPPCMDEC_STATICSTMBUF_PROTECTSPACE;
		}
		InitPar.StaticBitstreamBufSize = (InitPar.StaticBitstreamBufSize + 1023) & (~1023); //cnm static stream buffer size must 1024 aligned

		gcnm_printf("allocate static stream buffer, want sz %d\n", InitPar.StaticBitstreamBufSize);
		mem = new_cnmStmBuf(InitPar.StaticBitstreamBufSize);
		if(mem == NULL) {
			g_warning("Err: allocate static stream buffer fail, want sz %d", InitPar.StaticBitstreamBufSize);
			h263->iMemErr = -2;
			return -2;
		}
		pSStmBuf = mem->VAddr;
		InitPar.phyAddrStaticBitstreamBuf = mem->PAddr;
		h263->StmBufPool[0].BufPhyAddr = InitPar.phyAddrStaticBitstreamBuf;
		h263->StmBufPool[0].BufVirAddr = (unsigned char*)pSStmBuf;
		h263->StmBufPool[0].iBufSize = InitPar.StaticBitstreamBufSize;
		h263->StmBufPool[0].mem = mem;
	}else{
		//static stream buffer has been prepared
		gcnm_printf("static stream buffer already prepared, sz %d, phyaddr 0x%x.\n", h263->StmBufPool[0].iBufSize, (unsigned int)(h263->StmBufPool[0].BufPhyAddr));
		InitPar.StaticBitstreamBufSize = h263->StmBufPool[0].iBufSize;
		InitPar.phyAddrStaticBitstreamBuf = h263->StmBufPool[0].BufPhyAddr;
	}

	h263->StmBufPool[0].iDataLen = 0;
	//fill seq header and 1st frame
	gcnmdec_fill_stream(&(h263->StmBufPool[0]), h263->seqMeta, h263->seqMetaLen);
	gcnmdec_fill_stream(&(h263->StmBufPool[0]), GST_BUFFER_DATA(buf), GST_BUFFER_SIZE(buf));

	StartUp.StmBufPool = h263->StmBufPool;
	StartUp.virAddrStaticBitstreamBuf = h263->StmBufPool[0].BufVirAddr;
	StartUp.pInitPar = &InitPar;
	StartUp.pSeqInfo = &h263->SeqInfo;
	StartUp.iFilledLen_forCodecStart = h263->StmBufPool[0].iDataLen;
	StartUp.allocate_framebuf = _cnmh263dec_allocateframebufs;  //during start up codec, _cnmh263dec_allocateframebufs() will allocate frame buffer

	h263->iCodecErr = 0; //clear it before a new codec instance
	gcnm_printf("Call gcnmdec_startup_codec()...\n");
	
	g_mutex_lock(h263->cnmCodecMtx);
	h263->bCnmInDream = 0;
	StartUpRet = gcnmdec_startup_codec(h263, &StartUp, &h263->ShareBufManager);
	h263->pH263DecoderState = StartUp.pCnmDec;
	g_mutex_unlock(h263->cnmCodecMtx);
	
	h263->iCodecErr = StartUp.iCodecApiErr;
	h263->iMemErr = StartUp.iMemoryErr;
	if(G_LIKELY(StartUpRet == 0)) {
		//succeed
		h263->bCodecSucLaunched = 1;
		gcnm_printf("Launch cnm decoder succeed!\n");
	}else{
		GST_ERROR_OBJECT(h263, "Launch cnm decoder fail, ret %d", StartUpRet);
	}
	return StartUpRet;

}

static unsigned char*
seek_h263PSC(unsigned char* pData, int len)
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


static GstFlowReturn
gst_cnmh263dec_chain(GstPad *pad, GstBuffer *buf)
{
	gcnm_printfmore("enter %s, in buf sz %d, ts %lld\n", __func__, GST_BUFFER_SIZE(buf), GST_BUFFER_TIMESTAMP(buf));
	int ret, tmp, bCallDecAgain, bStopDecodingLoop, framelenUpperSz;
	GstFlowReturn FlowRt = GST_FLOW_OK;
	Gstcnmh263dec* h263 = GST_CNMH263DEC(GST_OBJECT_PARENT(pad));
	IppCodecStatus codecRt;
	GCNMDec_StmBuf* pCnmInputBuf;
	int cnmDreamStatusAtlock;
	unsigned char* pData = GST_BUFFER_DATA(buf);
	unsigned int DataLen = GST_BUFFER_SIZE(buf);

	if(h263->bWaitIPicture) {
		unsigned char* pPicHdr = seek_h263PSC(pData, DataLen);
		if(pPicHdr) {
			if((pPicHdr[4]&0x2) == 0) {
				//found I picture
				h263->bWaitIPicture = 0;
			}
		}
		if(h263->bWaitIPicture) {
			gst_buffer_unref(buf);
			gcnm_printf("give up the input stream data, because not I picture\n");
			FlowRt = GST_FLOW_OK;
			goto chain_exit;	//give up the data
		}
	}
		
	gcnmdec_insert_TsDu(&h263->TsDuManager, buf);

	if(h263->b1stFrame_forFileplay){
		GstCaps *CapTmp;
		h263->b1stFrame_forFileplay = 0;
		ret = gst_cnmh263dec_launch_codec(buf, h263);
		if(ret != 0) {
			gst_buffer_unref(buf);
			gcnm_printf("Err: gst_cnmh263dec_launch_codec fail, ret %d\n", ret);
			FlowRt = GST_FLOW_ERROR;
			goto chain_exit;
		}
		pCnmInputBuf = &(h263->StmBufPool[0]);
		h263->DownFrameLayout.iDownFrameWidth = h263->SeqInfo.FrameROI.width;
		h263->DownFrameLayout.iDownFrameHeight = h263->SeqInfo.FrameROI.height;
		if(h263->iI420LayoutKind != 2) {
			gcnmdec_update_downbuf_layout(h263->DownFrameLayout.iDownFrameWidth, h263->DownFrameLayout.iDownFrameHeight, 0, &h263->DownFrameLayout);
		}else{
			gcnmdec_update_downbuf_layout(h263->DownFrameLayout.iDownFrameWidth, h263->DownFrameLayout.iDownFrameHeight, 1, &h263->DownFrameLayout);
		}

		h263->iAlignedBufSz = (h263->SeqInfo.FrameAlignedWidth * h263->SeqInfo.FrameAlignedHeight * 3)>>1;


		gcnm_printf("Will set srcpad cap, fps %d/%d, w %d, h %d\n", h263->num, h263->den, h263->DownFrameLayout.iDownFrameWidth, h263->DownFrameLayout.iDownFrameHeight);
		CapTmp = gst_caps_new_simple ("video/x-raw-yuv",
			"format", GST_TYPE_FOURCC, GST_STR_FOURCC ("I420"),
			"width", G_TYPE_INT, h263->DownFrameLayout.iDownFrameWidth,
			"height", G_TYPE_INT, h263->DownFrameLayout.iDownFrameHeight,
			"framerate", GST_TYPE_FRACTION, h263->num, h263->den,
			NULL);
		gst_pad_set_caps(h263->srcpad, CapTmp);
		gst_caps_unref(CapTmp);

	}else{
		//load frame data
		framelenUpperSz = DataLen;
		tmp = gcnmdec_select_stream(h263->StmBufPool, GSTCNMDEC_H263_STMPOOLSZ, framelenUpperSz);
		if( tmp < 0 || tmp >= GSTCNMDEC_H263_STMPOOLSZ ) {
			h263->iMemErr = -3;
			gcnm_printf("Err: gcnmdec_select_stream fail, ret %d", tmp);
			GST_ERROR_OBJECT(h263, "Err: gcnmdec_select_stream fail, ret %d", tmp);
			gst_buffer_unref(buf);
			FlowRt = GST_FLOW_ERROR;
			goto chain_exit;
		}
		pCnmInputBuf = &(h263->StmBufPool[tmp]);
		gcnmdec_fill_stream(pCnmInputBuf, pData, DataLen);
	}

	gst_buffer_unref(buf);

	if(!h263->bCodecSucLaunched) {
		FlowRt = GST_FLOW_ERROR;
		goto chain_exit;
	}

	h263->FilePlay_StmBuf.phyAddrBitstreamBuf = pCnmInputBuf->BufPhyAddr;
	h263->FilePlay_StmBuf.DataLen = pCnmInputBuf->iDataLen;


	bStopDecodingLoop = 0;
	do{
		bCallDecAgain = 0;

		g_mutex_lock(h263->cnmCodecMtx);
		cnmDreamStatusAtlock = h263->bCnmInDream;
		if(cnmDreamStatusAtlock) {
			HibernateWakeupCNM(h263->pH263DecoderState, 0, &h263->bCnmInDream);
		}

		if(0 != gcnmdec_informcodec_decframeconsumed(h263->pH263DecoderState, &h263->ShareBufManager)) {
			h263->iCodecErr |= ERR_CNMDEC_DISPLAYEDAPI;
			g_mutex_unlock(h263->cnmCodecMtx);
			FlowRt = GST_FLOW_ERROR;
			goto chain_exit;
		}

#ifdef DEBUG_PERFORMANCE
		gettimeofday(&((CnmH263ProbeData*)h263->pProbeData)->tstart, &((CnmH263ProbeData*)h263->pProbeData)->tz);
#endif
		codecRt = DecodeFrame_CMVpu(h263->pH263DecoderState, &h263->FilePlay_StmBuf, &h263->DecOutInfo, 0);
#ifdef DEBUG_PERFORMANCE
		gettimeofday(&((CnmH263ProbeData*)h263->pProbeData)->tend, &((CnmH263ProbeData*)h263->pProbeData)->tz);
		h263->codec_time += (((CnmH263ProbeData*)h263->pProbeData)->tend.tv_sec - ((CnmH263ProbeData*)h263->pProbeData)->tstart.tv_sec)*1000000 + (((CnmH263ProbeData*)h263->pProbeData)->tend.tv_usec - ((CnmH263ProbeData*)h263->pProbeData)->tstart.tv_usec);
#endif
		if(! h263->iShareFrameBuf && (codecRt == IPP_STATUS_OUTPUT_DATA || codecRt == IPP_STATUS_FRAME_COMPLETE)) {
			(h263->ShareBufManager.DecOutFrameConsumeStatus[h263->DecOutInfo.idxFrameOutput])++;//In theory, under non-iShareFrameBuf mode, should after copy the frame data, then set the status that the frame is consumed. Anyway, setting it in advance is no side effect.
		}
		if(cnmDreamStatusAtlock) {
			HibernateWakeupCNM(h263->pH263DecoderState, 1, &h263->bCnmInDream);
		}
		g_mutex_unlock(h263->cnmCodecMtx);

		if(!(codecRt == IPP_STATUS_OUTPUT_DATA || (codecRt==IPP_STATUS_BUFFER_FULL && h263->iShareFrameBuf))) {
			//the data has been consumed
			pCnmInputBuf->iDataLen = 0;
		}

		switch(codecRt) {
		case IPP_STATUS_OUTPUT_DATA:
			//No frame is decoded, because the input frame data hasn't been consumed by VPU, therefore, application should feed this frame again.
			//But some frame still need output, which are reorder delayed frames.
			bCallDecAgain = 1;
		case IPP_STATUS_FRAME_COMPLETE:
			gcnm_printfmore("DecodeFrame_CMVpu ret IPP_STATUS_FRAME_COMPLETE, %d, %d, %d\n", h263->totalFrames, h263->DecOutInfo.idxFrameOutput, h263->DecOutInfo.NumOfErrMbs);
			h263->totalFrames++;
			_cnmh263_refresh_downpush_TsDu(h263);
			FlowRt = _cnmh263_push_data(h263, h263->DecOutInfo.idxFrameOutput);
			break;
		case IPP_STATUS_NOERR:
			gcnm_printf("DecodeFrame_CMVpu ret NOERR, %d %d %d\n", h263->DecOutInfo.OneFrameUnbrokenDecoded, h263->DecOutInfo.idxFrameOutput, h263->DecOutInfo.idxFrameDecoded);
			break;
		case IPP_STATUS_NOTSUPPORTED_ERR:
			h263->iCodecErr |= WARN_CNMDEC_FRAMENOTSUPPORT;
			_cnmh263_refresh_downpush_TsDu(h263);
			gcnm_printf("DecodeFrame_CMVpu ret IPP_STATUS_NOTSUPPORTED_ERR.\n");
			break;
		case IPP_STATUS_FRAME_ERR:
			h263->iCodecErr |= WARN_CNMDEC_FRAMECORRUPT;
			gcnm_printf("DecodeFrame_CMVpu ret WARN_CNMDEC_FRAMECORRUPT, probably the frame is corrupt.\n");
			break;
		case IPP_STATUS_BUFFER_FULL:
#ifndef NOTECHO_BUFFERFULL_WARNING
			g_warning("Warning: DecodeFrame_CMVpu ret buffer full, waiting sink release buffer\n");
#endif
			if(h263->iShareFrameBuf) {
				int waitRt;
				bCallDecAgain = 1;
				waitRt = wait_sink_retDecFrameBuf(h263->iTimeoutForDisp1Frame, -1, &h263->ShareBufManager, &h263->ElementState);
				if(waitRt > 0) {
					gcnm_printf("wait_sink_retDecFrameBuf() stopped by element state change\n");
					bStopDecodingLoop = 1;	//element isn't running, give up decoding
					//give up this frame, since we have limited the length of TS queue, little need to remove the TS of this frame from TS queue
					pCnmInputBuf->iDataLen = 0;
				}else if(waitRt < 0) {
					//wait time out or some error occur
					FlowRt = GST_FLOW_ERROR;
					h263->iCodecErr |= (waitRt == -2 ? ERR_WAITSINKRETFRAME_SINKNOFRAME: ERR_WAITSINKRETFRAME_TIMEOUT);
					goto chain_exit;
				}
				break;  //break switch
			}
		default:
			h263->iCodecErr |= ERR_CNMDEC_DECFRAME_SERIOUSERR;
			gcnm_printf("Err: DecodeFrame_CMVpu fail, ret %d\n", codecRt);
			GST_ERROR_OBJECT(h263, "Err: DecodeFrame_CMVpu fail, ret %d", codecRt);
			FlowRt = GST_FLOW_ERROR;
			goto chain_exit;
		}
	}while(bCallDecAgain == 1 && bStopDecodingLoop == 0);


chain_exit:
	if(FlowRt < GST_FLOW_UNEXPECTED) {
		GST_ELEMENT_ERROR(h263, STREAM, DECODE, (NULL), ("%s() flow err %d, codec err 0x%x, mem err %d", __FUNCTION__, FlowRt, h263->iCodecErr, h263->iMemErr));
	}
	return FlowRt;
}

static void
gst_cnmh263dec_clean_streambuf(Gstcnmh263dec *h263)
{
	int i;
	gcnm_printf("free stream buffer\n");

	for(i=0;i<GSTCNMDEC_H263_STMPOOLSZ;i++) {
		if(h263->StmBufPool[i].iBufSize != 0) {
			delete_cnmStmBuf(h263->StmBufPool[i].mem);
			h263->StmBufPool[i].iBufSize = 0;
			h263->StmBufPool[i].BufPhyAddr = 0;
			h263->StmBufPool[i].BufVirAddr = NULL;
			h263->StmBufPool[i].iDataLen = 0;
			h263->StmBufPool[i].mem = NULL;
		}
	}
	if(NULL!=h263->seqMeta){
		g_free(h263->seqMeta);
		h263->seqMeta = NULL;
		h263->seqMetaBufSz = 0;
		h263->seqMetaLen = 0;
	}
	return;
}

static gboolean
gst_cnmh263dec_null2ready(Gstcnmh263dec *h263)
{
	gcnm_printf("enter %s\n", __func__);

	h263->pH263DecoderState = NULL;
	h263->totalFrames = 0;
	h263->codec_time = 0;

	h263->ShareBufManager.watcher = gcnmdec_watcher_create(h263, NULL, &h263->ShareBufManager);   //create watcher
	if(h263->ShareBufManager.watcher == NULL) {
		g_warning("Err: gcnmdec_watcher_create() fail");
		return FALSE;
	}

	return TRUE;
}


static gboolean
gst_cnmh263dec_ready2null(Gstcnmh263dec *h263)
{
	gcnm_printf("enter %s\n", __func__);

	//close decoder
	g_mutex_lock(h263->cnmCodecMtx);
	if(h263->pH263DecoderState) {
		if(h263->bCnmInDream) {
			HibernateWakeupCNM(h263->pH263DecoderState, 0, &h263->bCnmInDream);
		}
		h263->iCodecErr |= gcnmdec_close_codec(&h263->pH263DecoderState, &h263->ShareBufManager);
	}
	g_mutex_unlock(h263->cnmCodecMtx);

	gcnmdec_remove_redundant_TsDu(&h263->TsDuManager, 0);

	//free buffers
	gst_cnmh263dec_clean_streambuf(h263);

	if(! h263->iShareFrameBuf) {
		gstcnmElement_clean_nonShareFramebuf(h263->DecFramesInfoArr, &h263->iDecFrameBufArrLen);
	}else{
		g_mutex_lock(h263->ShareBufManager.mtx);
		gstcnmElement_clean_idleShareFBs(&h263->ShareBufManager, NULL);	//for those share frame buffer which is still owned by sink, will free them on their gstbuffer finalizing callback function
		h263->ShareBufManager.NotNeedFMemAnyMore = 1;
		g_mutex_unlock(h263->ShareBufManager.mtx);
	}


	if(h263->ShareBufManager.watcher) {
		FRAMES_WATCHMAN_UNREF(h263->ShareBufManager.watcher);   //smart release watcher and free frame buffers
	}

#ifdef DEBUG_PERFORMANCE
	printf("codec totally outputed %d frames, spend %lld us\n", h263->totalFrames, h263->codec_time);
#else
	gcnm_printf("codec totally outputed %d frames, spend %lld us\n", h263->totalFrames, h263->codec_time);
#endif

#ifdef LOG_FRAMECOPYTIME
	printf("copy %d frame total cost %d us\n", ((CnmH263ProbeData*)h263->pProbeData)->log_copycnt, ((CnmH263ProbeData*)h263->pProbeData)->log_copytime);
#endif
	gcnm_printf("-----------At ready2null, c&m codec error %d, memory error %d\n", h263->iCodecErr, h263->iMemErr);

	return TRUE;
}

static int gst_cnmh263dec_ready2paused(Gstcnmh263dec *h263)
{
	gcnm_printf("gst_cnmh263dec_ready2paused() is called\n");
	//prepare for new stream

	return 0;
}

static int gst_cnmh263dec_paused2ready(Gstcnmh263dec* h263)
{
	gcnm_printf("gst_cnmh263dec_paused2ready() is called\n");
	gcnm_printf("-----------At paused2ready, cnm decoder error status %d\n", h263->iCodecErr);
	return 0;
}

static __inline int paused2play(Gstcnmh263dec* h263)
{
	IppCodecStatus ret = IPP_STATUS_NOERR;
	g_mutex_lock(h263->cnmCodecMtx);
	if(h263->pH263DecoderState && h263->bEnableHibernateAtPause && h263->bCnmInDream) {
		ret = HibernateWakeupCNM(h263->pH263DecoderState, 0, &h263->bCnmInDream);
	}
	g_mutex_unlock(h263->cnmCodecMtx);
	if(IPP_STATUS_NOERR != ret) {
		g_warning("Wakeup cnm fail at pause2paly, ret %d", ret);
		return -1;
	}
	return 0;
}

static __inline int play2paused(Gstcnmh263dec* h263)
{
	IppCodecStatus ret = IPP_STATUS_NOERR;
	g_mutex_lock(h263->cnmCodecMtx);
	if(h263->pH263DecoderState && h263->bEnableHibernateAtPause && ! h263->bCnmInDream) {
		ret = HibernateWakeupCNM(h263->pH263DecoderState, 1, &h263->bCnmInDream);
	}
	g_mutex_unlock(h263->cnmCodecMtx);
	if(IPP_STATUS_NOERR != ret) {
		g_warning("Hibernate cnm fail at pause2paly, ret %d", ret);
		return -1;
	}
	return 0;
}

static GstStateChangeReturn
gst_cnmh263dec_change_state(GstElement *element, GstStateChange transition)
{
	gcnm_printf("enter %s, change from %d to %d\n", __func__, transition>>3, transition&7);

	GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;
	Gstcnmh263dec *h263 = GST_CNMH263DEC(element);

	switch (transition)
	{
	case GST_STATE_CHANGE_NULL_TO_READY:
		if(!gst_cnmh263dec_null2ready(h263)){
			goto statechange_failed;
		}
		break;
	case GST_STATE_CHANGE_READY_TO_PAUSED:
		gst_cnmh263dec_ready2paused(h263);
		break;
	case GST_STATE_CHANGE_PAUSED_TO_PLAYING:
		if(0 != paused2play(h263)) {
			goto statechange_failed;
		}
		break;

	case GST_STATE_CHANGE_PAUSED_TO_READY:
		set_ElementState(&h263->ElementState, CNMELESTATE_RESTING);
		break;
	default:
		break;
	}

	ret = parent_class->change_state (element, transition);
	if (ret == GST_STATE_CHANGE_FAILURE) {
		return ret;
	}

	switch (transition)
	{
	case GST_STATE_CHANGE_PLAYING_TO_PAUSED:
		if(0 != play2paused(h263)) {
			goto statechange_failed;
		}
		break;
	case GST_STATE_CHANGE_PAUSED_TO_READY:
		gst_cnmh263dec_paused2ready(h263);
		break;
	case GST_STATE_CHANGE_READY_TO_NULL:
		if(!gst_cnmh263dec_ready2null(h263)){
			goto statechange_failed;
		}
		break;

	case GST_STATE_CHANGE_READY_TO_PAUSED:
		set_ElementState(&h263->ElementState, CNMELESTATE_RUNNING);
		break;
	default:
		break;
	}

	return ret;

	 /* ERRORS */
statechange_failed:
	{
		/* subclass must post a meaningfull error message */
		GST_ERROR_OBJECT (h263, "state change failed");
		return GST_STATE_CHANGE_FAILURE;
	}
}

static void
gst_cnmh263dec_base_init (gpointer klass)
{
	gcnm_printf("enter %s\n", __func__);

	static GstElementDetails h263dec_details = {
		"IPPCNM decoder H263",
		"Codec/Decoder/Video",
		"HARDWARE H263 Decoder based-on IPP CNM CODEC",
		""
	};

	GstElementClass *element_class = GST_ELEMENT_CLASS (klass);

	gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&src_factory));
	gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&sink_factory));
	gst_element_class_set_details (element_class, &h263dec_details);
}

static void
gst_cnmh263dec_finalize(GObject * object)
{
	Gstcnmh263dec* h263 = GST_CNMH263DEC(object);
	if(h263->pProbeData) {
		g_free(h263->pProbeData);
	}

	g_mutex_free(h263->cnmCodecMtx);

	g_mutex_free(h263->ElementState.elestate_mtx);

	GST_DEBUG_OBJECT(h263, "Gstcnmh263dec instance(%p) parent class is finalizing, codec err 0x%x, mem err %d.", h263, h263->iCodecErr, h263->iMemErr);
	G_OBJECT_CLASS(parent_class)->finalize(object);

	gcnm_printf("Gstcnmh263dec instance 0x%x is finalized!!!\n", (unsigned int)object);
	printf("Gstcnmh263dec instance (0x%x) is finalized\n", (unsigned int)object);

	return;
}


static void
gst_cnmh263dec_class_init (Gstcnmh263decClass *klass)
{
	gcnm_printf("enter %s\n", __func__);

	GObjectClass *gobject_class  = (GObjectClass*) klass;
	GstElementClass *gstelement_class = (GstElementClass*) klass;

	gobject_class->set_property = gst_cnmh263dec_set_property;
	gobject_class->get_property = gst_cnmh263dec_get_property;

	g_object_class_install_property (gobject_class, ARG_I420LAYOUTKIND, \
		g_param_spec_int ("I420layoutkind", "push down I420 buffer layout", \
		"Push down I420 buffer layout. It could be 0<auto decide>, 1<compact I420> or 2<gstreamer standard I420>. Don't access this property unless you are clear what you are doing.", \
		0/* range_MIN */, 2/* range_MAX */, 0/* default_INIT */, G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, ARG_SHAREFRAMEBUF, \
		g_param_spec_int ("shareFramebuf", "share frame buffer", \
		"Share frame buffer between decoder and down-stream plug-in, could be 1<share> or 0<not share>. If this property is enabled, I420layoutkind would be invalid. Don't access this property unless you are clear what you are doing.", \
		0/* range_MIN */, 1/* range_MAX */, 1/* default_INIT */, G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, ARG_TIMEOUTFORDISP1FRAME, \
		g_param_spec_int ("timeoutDisp1frame", "timeout for sink consumed 1 frame", \
		"Timeout for sink to finish consuming 1 frame(in useconds). Only valid when shareFramebuf==1. Could be -1(unlimited) to 2147483647. Don't access this property unless you are clear what you are doing.", \
		-1/* range_MIN */, G_MAXINT/* range_MAX */, -1/* default_INIT */, G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, ARG_SUPPORTMULTIINS, \
		g_param_spec_int ("supportMultiIns", "support multiple instance", \
		"Enable codec internal sync mechanism to support multiple instance. Could be 0<not enable>, or 1<support multi-instance>, or 2<reserved>. Don't access this property unless you are clear what you are doing.", \
		0/* range_MIN */, 1/* range_MAX */, 0/* default_INIT */, G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, ARG_IGNOREPBPICNEWSEG, \
		g_param_spec_int ("ignorePBInNewseg", "ignore non-I picture when new segment begin", \
		"Ignore non-I picture input data when new segment begin. Could be 0<not ignore>, or 1<ignore>.", \
		0/* range_MIN */, 1/* range_MAX */, 1/* default_INIT */, G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, ARG_ENABLEHIBERNATEATPAUSE, \
		g_param_spec_int ("cnmHibernateAtPause", "let cnm hardware hibernate at pause", \
		"Let cnm hardware hibernate at pause. Could be 0<not hibernate>, or 1<hibernate>.", \
		0/* range_MIN */, 1/* range_MAX */, 1/* default_INIT */, G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, ARG_ENABLEDYNACLK, \
		g_param_spec_int ("dynamicClock", "let cnm hardware dynamic close clock", \
		"Let cnm hardware dynamically close clock. Could be 0<not dynamic close clock>, or 1<dynamic close clock>.", \
		0/* range_MIN */, 1/* range_MAX */, 1/* default_INIT */, G_PARAM_READWRITE));

#ifdef DEBUG_PERFORMANCE
	g_object_class_install_property (gobject_class, ARG_TOTALFRAME,
		g_param_spec_int ("totalframes", "Totalframe of cnm h263 decoder outputted",
			"Number of frame of cnm h263 decoder outputted", 0, G_MAXINT, 0, G_PARAM_READABLE));
	g_object_class_install_property (gobject_class, ARG_CODECTIME,
		g_param_spec_int64 ("codectime", "cnm h263 decode time",
			"Pure codec time of calling cnm h263 decoding API(microsecond)", 0, G_MAXINT64, 0, G_PARAM_READABLE));
#endif

	gobject_class->finalize = GST_DEBUG_FUNCPTR (gst_cnmh263dec_finalize);
	gstelement_class->change_state = GST_DEBUG_FUNCPTR (gst_cnmh263dec_change_state);
	GST_DEBUG_CATEGORY_INIT(cnmdec_h263_debug, "cnmdec_h263", 0, "CNM h263 Decoder Element");
	return;
}

static void gst_cnmh263dec_init_members(Gstcnmh263dec* h263)
{
	gcnm_printf("gst_cnmh263dec_init_members is called\n");
	h263->pProbeData = g_malloc0(sizeof(CnmH263ProbeData));

	h263->pH263DecoderState = NULL;
	h263->num = 0;
	h263->den = 1;
	h263->bCodecSucLaunched = 0;
	h263->b1stFrame_forFileplay = 1;
	h263->iCodecErr = 0;
	h263->iMemErr = 0;

	memset(&h263->SeqInfo, 0, sizeof(h263->SeqInfo));
	memset(h263->DecFramesInfoArr, 0, sizeof(h263->DecFramesInfoArr));
	h263->iDecFrameBufArrLen = 0;
	memset(&h263->FilePlay_StmBuf, 0, sizeof(h263->FilePlay_StmBuf));
	memset(&h263->DecOutInfo, 0, sizeof(h263->DecOutInfo));
	memset(&h263->ShareBufManager, 0, sizeof(h263->ShareBufManager));

	h263->ElementState.state = CNMELESTATE_RESTING;
	h263->ElementState.elestate_mtx = g_mutex_new();

	h263->seqMeta = NULL;
	h263->seqMetaBufSz= 0;
	h263->seqMetaLen = 0;
	memset(h263->StmBufPool, 0, sizeof(h263->StmBufPool));
	memset(&h263->DownFrameLayout, 0, sizeof(h263->DownFrameLayout));
	gcnmdec_init_TsDuMgr(&h263->TsDuManager);

	h263->bWaitIPicture = 1;

	h263->iI420LayoutKind = 0;
	h263->iShareFrameBuf = 1;
	h263->iTimeoutForDisp1Frame = WAIT_DOWNSTREAM_RETBUF_TIMEOUT;
	h263->iSupportMultiIns = 1;
	h263->iIgnPBInNewseg = 1;
	h263->bEnableHibernateAtPause = 1;
	h263->bEnableDynaClk = 1;

	h263->bNewSegReceivedAfterflush = 0;

	h263->cnmCodecMtx = g_mutex_new();
	h263->bCnmInDream = 0;

	h263->totalFrames = 0;
	h263->codec_time = 0;
	return;
}

static void
gst_cnmh263dec_init(Gstcnmh263dec *h263, Gstcnmh263decClass *h263_klass)
{
	gcnm_printf("enter %s\n", __func__);

	GstElementClass *klass = GST_ELEMENT_CLASS(h263_klass);

	h263->sinkpad = gst_pad_new_from_template (
		gst_element_class_get_pad_template(klass, "sink"), "sink");

	gst_pad_set_setcaps_function (h263->sinkpad, GST_DEBUG_FUNCPTR (gst_cnmh263dec_sinkpad_setcaps));
	gst_pad_set_chain_function (h263->sinkpad, GST_DEBUG_FUNCPTR (gst_cnmh263dec_chain));
	gst_pad_set_event_function (h263->sinkpad, GST_DEBUG_FUNCPTR (gst_cnmh263dec_sinkpad_event));

	gst_element_add_pad(GST_ELEMENT(h263), h263->sinkpad);

	h263->srcpad = gst_pad_new_from_template (
		gst_element_class_get_pad_template(klass, "src"), "src");

	gst_element_add_pad(GST_ELEMENT(h263), h263->srcpad);

	gst_cnmh263dec_init_members(h263);

	gcnm_printf("Gstcnmh263dec instance 0x%x is inited!!!\n", (unsigned int)h263);
	printf("Gstcnmh263dec instance (0x%x) is inited\n", (unsigned int)h263);

	return;
}

/* EOF */
