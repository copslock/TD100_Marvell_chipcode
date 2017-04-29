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

/* gstcnmdec_mpeg4.c */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h> //to include memset(),...
#include <sys/time.h>	//to include time functions and structures

#include "gstcnmdec_mpeg4.h"
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
}CnmMPEG4ProbeData;

GST_DEBUG_CATEGORY_STATIC (cnmdec_mpeg4_debug);
#define GST_CAT_DEFAULT cnmdec_mpeg4_debug

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

static GstStaticPadTemplate sink_factory =
	GST_STATIC_PAD_TEMPLATE ("sink", GST_PAD_SINK, GST_PAD_ALWAYS,
	GST_STATIC_CAPS ("video/mpeg, mpegversion = (int) 4, systemstream = (boolean) FALSE, width = (int) [ 16, 1280 ], height = (int) [ 16, 1022 ], cnmnotsupport = (int) 0, framerate = (fraction) [ 0, MAX ]"
					 ";video/x-xvid, width = (int) [ 16, 1280 ], height = (int) [ 16, 1022 ], cnmnotsupport = (int) 0, framerate = (fraction) [ 0, MAX ]"
					 //";video/x-divx, divxversion = (int) {4, 5}"		//cnm only support mpeg4 SP, current divx encoder usually adopt B frame feature and other mpeg4 ASP features
					 ));

static GstStaticPadTemplate src_factory =
	GST_STATIC_PAD_TEMPLATE ("src", GST_PAD_SRC, GST_PAD_ALWAYS,
	GST_STATIC_CAPS ("video/x-raw-yuv,"
					 "format = (fourcc) { I420 }, "
					 "width = (int) [ 16, 1280 ], "
					 "height = (int) [ 16, 1022 ], "
					 "framerate = (fraction) [ 0, MAX ]")
					 );


GST_BOILERPLATE (Gstcnmmpeg4dec, gst_cnmmpeg4dec, GstElement, GST_TYPE_ELEMENT);


static gboolean
gst_cnmmpeg4dec_sinkpad_setcaps (GstPad *pad, GstCaps *caps)
{
	gcnm_printf("enter gst_cnmmpeg4dec_sinkpad_setcaps().\n");

	Gstcnmmpeg4dec *mpeg4 = GST_CNMMPEG4DEC (GST_OBJECT_PARENT (pad));
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

	if(G_UNLIKELY(FALSE == gst_structure_get_fraction(str, "framerate", &mpeg4->num, &mpeg4->den))) {
		mpeg4->num = 0; 	//if this information isn't in caps, we assume it is 0
		mpeg4->den = 1;
	}

	gcnm_printf("fps: %d/%d\n", mpeg4->num, mpeg4->den);

	if(mpeg4->num != 0) {
		mpeg4->TsDuManager.FrameFixPeriod = gst_util_uint64_scale_int(GST_SECOND, mpeg4->den, mpeg4->num);
	}

	gcnm_printf("frame duration %lld ns\n",  mpeg4->TsDuManager.FrameFixPeriod);



	if ((value = gst_structure_get_value (str, "codec_data"))){
		guint8 *cdata;
		guint csize;
		GstBuffer *buf;

		/* We have codec data, means packetised stream */
		gcnm_printf("have codec_data\n");
		buf = gst_value_get_buffer (value);
		cdata = GST_BUFFER_DATA (buf);
		csize = GST_BUFFER_SIZE (buf);

		gcnm_printf("have codec_data, size %d\n", csize);

		if(mpeg4->codecData== NULL) {
			mpeg4->codecData = g_malloc(csize);
			if(mpeg4->codecData == NULL) {
				gcnm_printf("Malloc seqMeta(,%d,) fail.\n", csize);
				return FALSE;
			}else{
				mpeg4->codecDataBufSz = csize;
				gcnm_printf("Create sequence internal stream buffer 0x%x, size %d\n", (unsigned int)mpeg4->codecData, csize);
			}
		}else if(mpeg4->codecDataBufSz < (int)csize) {
			mpeg4->codecData = g_realloc(mpeg4->codecData, csize);
			if(mpeg4->codecData == NULL) {
				gcnm_printf("Malloc seqMeta(,%d,) fail.\n", csize);
				return FALSE;
			}else{
				mpeg4->codecDataBufSz = csize;
				gcnm_printf("Re-Create sequence internal stream buffer 0x%x, size %d\n", (unsigned int)mpeg4->codecData, csize);
			}
		}

		memcpy(mpeg4->codecData, cdata, csize);
		mpeg4->codecDataLen = csize;
	}

	gcnm_printf("exit gst_cnmmpeg4dec_sinkpad_setcaps() succeed\n");
	return TRUE;
}

static void
gst_cnmmpeg4dec_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{

	Gstcnmmpeg4dec *mpeg4 = GST_CNMMPEG4DEC (object);

	switch (prop_id)
	{
	case ARG_I420LAYOUTKIND:
		{
			int layout = g_value_get_int(value);
			if(layout < 0 || layout > 2) {
				GST_ERROR_OBJECT(mpeg4, "I420layoutkind %d exceed range.", layout);
				layout = 0;
			}
			mpeg4->iI420LayoutKind = layout;
		}
		break;
	case ARG_SHAREFRAMEBUF:
		{
			int sha = g_value_get_int(value);
			if(sha < 0 || sha > 1) {
				GST_ERROR_OBJECT(mpeg4, "shareFramebuf %d exceed range.", sha);
				sha = 1;
			}
			mpeg4->iShareFrameBuf = sha;
		}
		break;
	case ARG_TIMEOUTFORDISP1FRAME:
		{
			int to = g_value_get_int(value);
			if(to < -1) {
				GST_ERROR_OBJECT(mpeg4, "timeoutDisp1frame %d exceed range.", to);
				to = -1;
			}
			mpeg4->iTimeoutForDisp1Frame = to;
		}
		break;
	case ARG_SUPPORTMULTIINS:
		{
			int supm = g_value_get_int(value);
			if(supm < 0 || supm > 2) {
				GST_ERROR_OBJECT(mpeg4, "supportMultiIns %d exceed range.", supm);
				supm = 0;
			}
			mpeg4->iSupportMultiIns = supm;
		}
		break;
	case ARG_IGNOREPBPICNEWSEG:
		{
			int ig = g_value_get_int(value);
			if(ig < 0 || ig > 1) {
				GST_ERROR_OBJECT(mpeg4, "ignorePBInNewseg %d exceed range.", ig);
				ig = 0;
			}
			mpeg4->iIgnPBInNewseg = ig;
		}
		break;
	case ARG_ENABLEHIBERNATEATPAUSE:
		{
			int eh = g_value_get_int(value);
			if(eh < 0 || eh > 1) {
				GST_ERROR_OBJECT(mpeg4, "cnmHibernateAtPause %d exceed range.", eh);
				eh = 0;
			}
			mpeg4->bEnableHibernateAtPause = eh;
			
		}
		break;
	case ARG_ENABLEDYNACLK:
		{
			int ec = g_value_get_int(value);
			if(ec < 0 || ec > 1) {
				GST_ERROR_OBJECT(mpeg4, "dynamicClock %d exceed range.", ec);
				ec = 0;
			}
			mpeg4->bEnableDynaClk = ec;
		}
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
	return;
}


static void
gst_cnmmpeg4dec_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	Gstcnmmpeg4dec *mpeg4 = GST_CNMMPEG4DEC (object);

	switch (prop_id)
	{
	case ARG_I420LAYOUTKIND:
		g_value_set_int(value, mpeg4->iI420LayoutKind);
		break;
	case ARG_SHAREFRAMEBUF:
		g_value_set_int(value, mpeg4->iShareFrameBuf);
		break;
	case ARG_TIMEOUTFORDISP1FRAME:
		g_value_set_int(value, mpeg4->iTimeoutForDisp1Frame);
		break;
	case ARG_SUPPORTMULTIINS:
		g_value_set_int(value, mpeg4->iSupportMultiIns);
		break;
	case ARG_IGNOREPBPICNEWSEG:
		g_value_set_int(value, mpeg4->iIgnPBInNewseg);
		break;
	case ARG_ENABLEHIBERNATEATPAUSE:
		g_value_set_int(value, mpeg4->bEnableHibernateAtPause);
		break;
	case ARG_ENABLEDYNACLK:
		g_value_set_int(value, mpeg4->bEnableDynaClk);
		break;

#ifdef DEBUG_PERFORMANCE
	case ARG_TOTALFRAME:
		g_value_set_int(value, mpeg4->totalFrames);
		break;
	case ARG_CODECTIME:
		g_value_set_int64(value, mpeg4->codec_time);
		break;
#endif

	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
	return;
}

static __inline void _cnmmpeg4_refresh_downpush_TsDu(Gstcnmmpeg4dec* mpeg4)
{
	//gcnmdec_remove_redundant_TsDu(&mpeg4->TsDuManager, mpeg4->SeqInfo.MaxReorderDelay+1);
	//since no reorder, reorder delay should always be 0
	gcnmdec_remove_redundant_TsDu(&mpeg4->TsDuManager, 1);
	gcnmdec_refresh_output_TsDu(&mpeg4->TsDuManager);
	return;
}

static GstFlowReturn _cnmmpeg4_push_data(Gstcnmmpeg4dec *mpeg4, int iDecOutFrameIdx);

static gboolean
gst_cnmmpeg4dec_sinkpad_event (GstPad *pad, GstEvent *event)
{
	gcnm_printf("enter %s, event 0x%x\n", __func__, GST_EVENT_TYPE(event));

	gboolean eventRet;
	Gstcnmmpeg4dec *mpeg4 = GST_CNMMPEG4DEC(GST_OBJECT_PARENT(pad));

	switch(GST_EVENT_TYPE (event))
	{
	case GST_EVENT_NEWSEGMENT:
		gcnm_printf("NewSeg event is received.\n");
		{
			gcnmdec_remove_redundant_TsDu(&mpeg4->TsDuManager, 0);
			mpeg4->TsDuManager.LastOutputTs = GST_CLOCK_TIME_NONE;
			mpeg4->TsDuManager.LastOutputDu = GST_CLOCK_TIME_NONE;
			if(mpeg4->iIgnPBInNewseg) {
				mpeg4->bWaitIVop = 1;
			}

			eventRet = gst_pad_push_event(mpeg4->srcpad, event);
			mpeg4->bNewSegReceivedAfterflush = 1;
		}
		break;
	case GST_EVENT_EOS:
		gcnm_printf("EOS event is received.\n");
		{
			//for h263&mpeg4, always no-reorder mode and fileplay mode, when EOS coming, all compressed data are decoded by decoder, and all frame are outputted. Therefore, could close codec.
			
			g_mutex_lock(mpeg4->cnmCodecMtx);
			if(mpeg4->pMPEG4DecoderState) {
				if(mpeg4->bCnmInDream) {
					HibernateWakeupCNM(mpeg4->pMPEG4DecoderState, 0, &mpeg4->bCnmInDream);
				}
				mpeg4->iCodecErr |= gcnmdec_close_codec(&mpeg4->pMPEG4DecoderState, &mpeg4->ShareBufManager);
			}
			g_mutex_unlock(mpeg4->cnmCodecMtx);

			mpeg4->b1stFrame_forFileplay = 1;
			mpeg4->bCodecSucLaunched = 0;
			mpeg4->bVOLReceived = 0;
			mpeg4->bWaitIVop = 1;
			gcnmdec_remove_redundant_TsDu(&mpeg4->TsDuManager, 0);
			mpeg4->TsDuManager.LastOutputTs = GST_CLOCK_TIME_NONE;
			mpeg4->TsDuManager.LastOutputDu = GST_CLOCK_TIME_NONE;
			eventRet = gst_pad_push_event(mpeg4->srcpad, event);
		}
		break;
	case GST_EVENT_FLUSH_STOP:
		mpeg4->bNewSegReceivedAfterflush = 0;
	default:
		eventRet = gst_pad_event_default(pad, event);
		break;
	}

	return eventRet;
}

static GstFlowReturn _cnmmpeg4_push_data(Gstcnmmpeg4dec* mpeg4, int iDecOutFrameIdx)
{
	GstFlowReturn FlowRt = GST_FLOW_OK;
	GstBuffer *outBuf = NULL;

	gcnm_printfmore("enter %s\n", __func__);

	if(mpeg4->iShareFrameBuf) {
		int idxInDFBShare;
		void* VA = mpeg4->DecFramesInfoArr[iDecOutFrameIdx].pUserData1;
		unsigned int PA = (unsigned int)((CnmPContMem*)mpeg4->DecFramesInfoArr[iDecOutFrameIdx].phyY)->PAddr;
		outBuf = gst_buffer_new();
		if(outBuf == NULL) {
			return GST_FLOW_ERROR;
		}
		GST_BUFFER_DATA(outBuf) = VA;
		GST_BUFFER_SIZE(outBuf) = mpeg4->iAlignedBufSz;
		gst_buffer_set_caps(outBuf, GST_PAD_CAPS(mpeg4->srcpad));
		idxInDFBShare = ((int)(mpeg4->DecFramesInfoArr[iDecOutFrameIdx].pUserData2)>>16) & 0xff;
		IPPGST_BUFFER_CUSTOMDATA(outBuf) = (gpointer) (&mpeg4->ShareBufManager.sideinfoDFBShareArr[idxInDFBShare]);
		setSideInfo(&mpeg4->ShareBufManager.sideinfoDFBShareArr[idxInDFBShare], mpeg4->SeqInfo.FrameROI.x, mpeg4->SeqInfo.FrameROI.y, mpeg4->SeqInfo.FrameAlignedWidth, mpeg4->SeqInfo.FrameAlignedHeight, PA, VA);
		mpeg4->DecFramesInfoArr[iDecOutFrameIdx].pUserData2 = (void*)((idxInDFBShare<<16)|mpeg4->ShareBufManager.CurCodecSerialNum|(iDecOutFrameIdx<<24));
		mpeg4->DecFramesInfoArr[iDecOutFrameIdx].pUserData3 = (void*)(&mpeg4->ShareBufManager);
		GST_BUFFER_MALLOCDATA(outBuf) = (guint8*)(&(mpeg4->DecFramesInfoArr[iDecOutFrameIdx]));
		GST_BUFFER_FREE_FUNC(outBuf) = (GFreeFunc)gcnmdec_downbuf_onfinalize;
		FRAMES_WATCHMAN_REF(mpeg4->ShareBufManager.watcher);

		GST_BUFFER_TIMESTAMP(outBuf) = mpeg4->TsDuManager.LastOutputTs;
		GST_BUFFER_DURATION(outBuf) = mpeg4->TsDuManager.LastOutputDu;

		g_mutex_lock(mpeg4->ShareBufManager.mtx);
		mpeg4->ShareBufManager.DFBShareArr[idxInDFBShare].pUserData3 = (void*)1;	//DFBShareArr[x].pUserData3 == 0 means display sink do not hold this frame, 1 means display sink is holding this frame.
		mpeg4->ShareBufManager.iSinkOwnedDecFrameBufCnt++;
		g_mutex_unlock(mpeg4->ShareBufManager.mtx);

		if(mpeg4->bNewSegReceivedAfterflush) {
			FlowRt = gst_pad_push(mpeg4->srcpad, outBuf);
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

	FlowRt = gcnmdec_require_framebuf_fromdownplugin(&mpeg4->DownFrameLayout, mpeg4->srcpad, &outBuf, mpeg4->iI420LayoutKind==0);
	if(GST_FLOW_OK != FlowRt) {
		if(FlowRt != GST_FLOW_WRONG_STATE) {
			//during seeking, sink element often return GST_FLOW_WRONG_STATUS. No need to echo this message
			g_warning("Require gstbuffer from downstream plugin fail, ret %d!", FlowRt);
		}
		return FlowRt;
	}
#ifdef LOG_FRAMECOPYTIME
	gettimeofday(&((CnmMPEG4ProbeData*)mpeg4->pProbeData)->tlog_0, NULL);
#endif
	gcnmdec_copy_to_downframebuf(GST_BUFFER_DATA(outBuf), &mpeg4->DownFrameLayout, &(mpeg4->DecFramesInfoArr[iDecOutFrameIdx]), &(mpeg4->SeqInfo.FrameROI));
#ifdef LOG_FRAMECOPYTIME
	gettimeofday(&((CnmMPEG4ProbeData*)mpeg4->pProbeData)->tlog_1, NULL);
	((CnmMPEG4ProbeData*)mpeg4->pProbeData)->log_copycnt++;
	((CnmMPEG4ProbeData*)mpeg4->pProbeData)->log_copytime += (((CnmMPEG4ProbeData*)mpeg4->pProbeData)->tlog_1.tv_sec - ((CnmMPEG4ProbeData*)mpeg4->pProbeData)->tlog_0.tv_sec)*1000000 + (((CnmMPEG4ProbeData*)mpeg4->pProbeData)->tlog_1.tv_usec - ((CnmMPEG4ProbeData*)mpeg4->pProbeData)->tlog_0.tv_usec);
#endif


	GST_BUFFER_TIMESTAMP(outBuf) = mpeg4->TsDuManager.LastOutputTs;
	GST_BUFFER_DURATION(outBuf) = mpeg4->TsDuManager.LastOutputDu;
	if(mpeg4->bNewSegReceivedAfterflush) {
		FlowRt = gst_pad_push(mpeg4->srcpad, outBuf);
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


static int _cnmmpeg4dec_allocateframebufs(void* pElement, IppCMVpuDecParsedSeqInfo* pSeqInfo, int* piFrameCnt, IppCMVpuDecFrameAddrInfo** pFrameArr, int* piFrameStride)
{
	int codecframeNeedCnt;
	Gstcnmmpeg4dec* ele = (Gstcnmmpeg4dec*)pElement;
	int ret;
	int bCacheable;
	int framebufsz = (pSeqInfo->FrameAlignedWidth * pSeqInfo->FrameAlignedHeight * 3)>>1;

	*piFrameStride = pSeqInfo->FrameAlignedWidth;
	*pFrameArr = ele->DecFramesInfoArr;
	

	if(ele->iShareFrameBuf) {
		bCacheable = 0;
		codecframeNeedCnt = gcnmdec_smartdecide_framealloc_cnt(pSeqInfo->MinFrameBufferCount, IPPCMDEC_DECFRAMEBUF_MAX_H263MPEG2MPEG4, IPPGST_CMVPUDEC_FRAMEBUF_DEFAULT_MAXTOTALSIZE, framebufsz, IPPGSTCNMDEC_SPEC_MPEG4);
	}else{
		bCacheable = 0;
		codecframeNeedCnt = pSeqInfo->MinFrameBufferCount;
	}

	ret = prepare_framebufforcodec(pSeqInfo->FrameAlignedHeight, pSeqInfo->FrameAlignedWidth, piFrameCnt, codecframeNeedCnt, &ele->ShareBufManager, ele->DecFramesInfoArr, &ele->iDecFrameBufArrLen, ele->iShareFrameBuf, bCacheable);

	return ret;
}


static int gst_cnmmpeg4dec_launch_codec(GstBuffer* buf, Gstcnmmpeg4dec* mpeg4)
{
	GCNMDec_StartUpDec StartUp;
	int StartUpRet = 0;
	void* pSStmBuf = NULL;
	IppCMVpuDecInitPar InitPar;
	CnmPContMem* mem;

	gcnm_printf("enter %s\n", __func__);

	memset(&InitPar, 0, sizeof(InitPar));
	InitPar.ReorderEnable = 0;
	InitPar.FilePlayEnable = 1;
	InitPar.VideoStreamFormat = IPP_VIDEO_STRM_FMT_MPG4;
	InitPar.MultiInstanceSync = mpeg4->iSupportMultiIns;
	InitPar.DynamicClkEnable = mpeg4->bEnableDynaClk;

	if(mpeg4->StmBufPool[0].iBufSize == 0) {
		int framelenUpperSz;
		InitPar.StaticBitstreamBufSize = 384*1024;  //init a value
		framelenUpperSz = GST_BUFFER_SIZE(buf);
		if(InitPar.StaticBitstreamBufSize < (unsigned int)(framelenUpperSz + mpeg4->codecDataLen + IPPCMDEC_STATICSTMBUF_PROTECTSPACE) ) {
			InitPar.StaticBitstreamBufSize = framelenUpperSz + mpeg4->codecDataLen + IPPCMDEC_STATICSTMBUF_PROTECTSPACE;
		}
		InitPar.StaticBitstreamBufSize = (InitPar.StaticBitstreamBufSize + 1023) & (~1023); //cnm static stream buffer size must 1024 aligned

		gcnm_printf("allocate static stream buffer, want sz %d\n", InitPar.StaticBitstreamBufSize);
		mem = new_cnmStmBuf(InitPar.StaticBitstreamBufSize);
		if(mem == NULL) {
			g_warning("Err: allocate static stream buffer fail, want sz %d", InitPar.StaticBitstreamBufSize);
			mpeg4->iMemErr = -2;
			return -2;
		}
		pSStmBuf = mem->VAddr;
		InitPar.phyAddrStaticBitstreamBuf = mem->PAddr;
		mpeg4->StmBufPool[0].BufPhyAddr = InitPar.phyAddrStaticBitstreamBuf;
		mpeg4->StmBufPool[0].BufVirAddr = (unsigned char*)pSStmBuf;
		mpeg4->StmBufPool[0].iBufSize = InitPar.StaticBitstreamBufSize;
		mpeg4->StmBufPool[0].mem = mem;
	}else{
		//static stream buffer has been prepared
		gcnm_printf("static stream buffer already prepared, sz %d, phyaddr 0x%x.\n", mpeg4->StmBufPool[0].iBufSize, (unsigned int)(mpeg4->StmBufPool[0].BufPhyAddr));
		InitPar.StaticBitstreamBufSize = mpeg4->StmBufPool[0].iBufSize;
		InitPar.phyAddrStaticBitstreamBuf = mpeg4->StmBufPool[0].BufPhyAddr;
	}

	mpeg4->StmBufPool[0].iDataLen = 0;
	//fill seq header and 1st frame
	gcnmdec_fill_stream(&(mpeg4->StmBufPool[0]), mpeg4->codecData, mpeg4->codecDataLen);
	gcnmdec_fill_stream(&(mpeg4->StmBufPool[0]), GST_BUFFER_DATA(buf), GST_BUFFER_SIZE(buf));

	StartUp.StmBufPool = mpeg4->StmBufPool;
	StartUp.virAddrStaticBitstreamBuf = mpeg4->StmBufPool[0].BufVirAddr;
	StartUp.pInitPar = &InitPar;
	StartUp.pSeqInfo = &mpeg4->SeqInfo;
	StartUp.iFilledLen_forCodecStart = mpeg4->StmBufPool[0].iDataLen;
	StartUp.allocate_framebuf = _cnmmpeg4dec_allocateframebufs; //during start up codec, _cnmmpeg4dec_allocateframebufs() will allocate frame buffer

	mpeg4->iCodecErr = 0; //clear it before a new codec instance
	gcnm_printf("Call gcnmdec_startup_codec()...\n");

	g_mutex_lock(mpeg4->cnmCodecMtx);
	mpeg4->bCnmInDream = 0;
	StartUpRet = gcnmdec_startup_codec(mpeg4, &StartUp, &mpeg4->ShareBufManager);
	mpeg4->pMPEG4DecoderState = StartUp.pCnmDec;
	g_mutex_unlock(mpeg4->cnmCodecMtx);
	
	mpeg4->iCodecErr = StartUp.iCodecApiErr;
	mpeg4->iMemErr = StartUp.iMemoryErr;
	if(G_LIKELY(StartUpRet == 0)) {
		//succeed
		mpeg4->bCodecSucLaunched = 1;
		gcnm_printf("Launch cnm decoder succeed!\n");
	}else{
		GST_ERROR_OBJECT(mpeg4, "Launch cnm decoder fail, ret %d", StartUpRet);
	}
	return StartUpRet;

}

static __inline unsigned char*
seek_MPEG4SC(unsigned char* pData, int len)
{
	return seek_SC_000001xx(pData, len);
}

static unsigned char*
found_mpeg4_seqhdr(unsigned char* pData, int len)
{
	unsigned char* pSeqHdr;
	unsigned char* pData0 = pData;
	//seek video_object_layer_start_code
	for(;;) {
		pSeqHdr = seek_MPEG4SC(pData, len);
		if(pSeqHdr == NULL) {
			return NULL;
		}
		if((pSeqHdr[3]>>4) == 0x2) {
			break;	//found video_object_layer_start_code
		}else{
			len -= pSeqHdr+4-pData;
			pData = pSeqHdr+4;
		}
	}
	//if video_object_start_code is adjacent to video_object_layer_start_code, we consider video_object_start_code is the sequence begin, we ignore the visual_object_sequence() and VisualObject(), only care video object().
	if(pSeqHdr >= pData0+4) {
		pData0 = pSeqHdr-4;
		unsigned int tmp = ((unsigned int)(pData0[0])<<24) | ((unsigned int)(pData0[1])<<16) | ((unsigned int)(pData0[2])<<8) | pData0[3];
		if((tmp&0xffffffe0) == 0x00000100) {
			//found video_object_start_code
			pSeqHdr = pData0;
		}
	}
	return pSeqHdr;
}

static unsigned char*
seek_mpeg4vophdr(unsigned char* pData, int len)
{
	unsigned int code;
	unsigned char* pStartCode = pData;
	if(len < 5) {
		return NULL;
	}
	code = ((unsigned int)pStartCode[0]<<24) | ((unsigned int)pStartCode[1]<<16) | ((unsigned int)pStartCode[2]<<8) | pStartCode[3];
	while(code != 0x000001b6) {
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
gst_cnmmpeg4dec_chain(GstPad *pad, GstBuffer *buf)
{
	gcnm_printfmore("enter %s, in buf sz %d, ts %lld\n", __func__, GST_BUFFER_SIZE(buf), GST_BUFFER_TIMESTAMP(buf));
	int ret, tmp, bCallDecAgain, bStopDecodingLoop, framelenUpperSz;
	GstFlowReturn FlowRt = GST_FLOW_OK;
	Gstcnmmpeg4dec* mpeg4 = GST_CNMMPEG4DEC(GST_OBJECT_PARENT(pad));
	IppCodecStatus codecRt;
	GCNMDec_StmBuf* pCnmInputBuf;
	int cnmDreamStatusAtlock;
	unsigned char* pData = GST_BUFFER_DATA(buf);
	unsigned int DataLen = GST_BUFFER_SIZE(buf);

	if(!mpeg4->bVOLReceived) {
		unsigned char* pSeqHdr;
		if(mpeg4->codecDataLen != 0) {
			//at first, query sequence header in codecData
			pSeqHdr = found_mpeg4_seqhdr(mpeg4->codecData, mpeg4->codecDataLen);
			if(pSeqHdr) {
				gcnm_printf("found seq header in codec_data, 0x%x, 0x%x\n", (unsigned int)pSeqHdr, (unsigned int)mpeg4->codecData);
				mpeg4->bVOLReceived = 1;
			}else{
				//since no sequence header in codecData, it's useless, clean it
				mpeg4->codecDataLen = 0;
			}
		}
		if(!mpeg4->bVOLReceived) {
			//then query seq header in common data
			pSeqHdr = found_mpeg4_seqhdr(pData, DataLen);
			if(pSeqHdr == NULL) {
				gst_buffer_unref(buf);
				gcnm_printf("give up the input stream data, because not contain sequence header\n");
				FlowRt = GST_FLOW_OK;
				goto chain_exit;//give up the data
			}else{
				if(pSeqHdr != pData) {
					//recreate buf, let seqheader at the beginning of buf
					GstBuffer* newbuf = gst_buffer_new_and_alloc(DataLen - (pSeqHdr-pData));
					if(newbuf == NULL) {
						gcnm_printf("Err: call gst_buffer_new_and_alloc fail, line %d\n", __LINE__);
						g_warning("Err: call gst_buffer_new_and_alloc fail, line %d", __LINE__);
						GST_ERROR_OBJECT(mpeg4, "Err: call gst_buffer_new_and_alloc fail, line %d", __LINE__);
						FlowRt = GST_FLOW_ERROR;
						goto chain_exit;
					}
					memcpy(GST_BUFFER_DATA(newbuf), pSeqHdr, DataLen - (pSeqHdr-pData));
					GST_BUFFER_DURATION(newbuf) = GST_BUFFER_DURATION(buf);
					GST_BUFFER_TIMESTAMP(newbuf) = GST_BUFFER_TIMESTAMP(buf);
					gst_buffer_unref(buf);
					buf = newbuf;
					pData = GST_BUFFER_DATA(buf);
					DataLen = GST_BUFFER_SIZE(buf);
				}
				mpeg4->bVOLReceived = 1;
			}
		}
	}

	if(mpeg4->bWaitIVop) {
		unsigned char* pVopHdr = seek_mpeg4vophdr(pData, DataLen);
		if(pVopHdr) {
			if((pVopHdr[4]>>6) == 0) {
				//found I VOP
				mpeg4->bWaitIVop = 0;
			}
		}
		if(mpeg4->bWaitIVop) {
			gst_buffer_unref(buf);
			gcnm_printf("give up the input stream data, because not I vop\n");
			FlowRt = GST_FLOW_OK;
			goto chain_exit;//give up the data
		}
	}


	gcnmdec_insert_TsDu(&mpeg4->TsDuManager, buf);

	if(DataLen == 1 && *pData == 0x7f) {
		//stuffing bits, spec 14496-2: sec 5.2.3, 5.2.4
		FlowRt = GST_FLOW_OK;
		goto chain_exit;
	}


	if(mpeg4->b1stFrame_forFileplay){
		GstCaps *CapTmp;
		mpeg4->b1stFrame_forFileplay = 0;
		ret = gst_cnmmpeg4dec_launch_codec(buf, mpeg4);
		if(ret != 0) {
			gst_buffer_unref(buf);
			gcnm_printf("Err: gst_cnmmpeg4dec_launch_codec fail, ret %d\n", ret);
			FlowRt = GST_FLOW_ERROR;
			goto chain_exit;
		}
		pCnmInputBuf = &(mpeg4->StmBufPool[0]);
		mpeg4->DownFrameLayout.iDownFrameWidth = mpeg4->SeqInfo.FrameROI.width;
		mpeg4->DownFrameLayout.iDownFrameHeight = mpeg4->SeqInfo.FrameROI.height;
		if(mpeg4->iI420LayoutKind != 2) {
			gcnmdec_update_downbuf_layout(mpeg4->DownFrameLayout.iDownFrameWidth, mpeg4->DownFrameLayout.iDownFrameHeight, 0, &mpeg4->DownFrameLayout);
		}else{
			gcnmdec_update_downbuf_layout(mpeg4->DownFrameLayout.iDownFrameWidth, mpeg4->DownFrameLayout.iDownFrameHeight, 1, &mpeg4->DownFrameLayout);
		}

		mpeg4->iAlignedBufSz = (mpeg4->SeqInfo.FrameAlignedWidth * mpeg4->SeqInfo.FrameAlignedHeight * 3)>>1;


		gcnm_printf("Will set srcpad cap, fps %d/%d, w %d, h %d\n", mpeg4->num, mpeg4->den, mpeg4->DownFrameLayout.iDownFrameWidth, mpeg4->DownFrameLayout.iDownFrameHeight);
		CapTmp = gst_caps_new_simple ("video/x-raw-yuv",
			"format", GST_TYPE_FOURCC, GST_STR_FOURCC ("I420"),
			"width", G_TYPE_INT, mpeg4->DownFrameLayout.iDownFrameWidth,
			"height", G_TYPE_INT, mpeg4->DownFrameLayout.iDownFrameHeight,
			"framerate", GST_TYPE_FRACTION, mpeg4->num, mpeg4->den,
			NULL);
		gst_pad_set_caps(mpeg4->srcpad, CapTmp);
		gst_caps_unref(CapTmp);

	}else{
		//load frame data
		framelenUpperSz = DataLen;
		tmp = gcnmdec_select_stream(mpeg4->StmBufPool, GSTCNMDEC_MPEG4_STMPOOLSZ, framelenUpperSz);
		if( tmp < 0 || tmp >= GSTCNMDEC_MPEG4_STMPOOLSZ ) {
			mpeg4->iMemErr = -3;
			gcnm_printf("Err: gcnmdec_select_stream fail, ret %d", tmp);
			GST_ERROR_OBJECT(mpeg4, "Err: gcnmdec_select_stream fail, ret %d", tmp);
			gst_buffer_unref(buf);
			FlowRt = GST_FLOW_ERROR;
			goto chain_exit;
		}
		pCnmInputBuf = &(mpeg4->StmBufPool[tmp]);
		gcnmdec_fill_stream(pCnmInputBuf, pData, DataLen);
	}

	gst_buffer_unref(buf);

	if(!mpeg4->bCodecSucLaunched) {
		FlowRt = GST_FLOW_ERROR;
		goto chain_exit;
	}

	mpeg4->FilePlay_StmBuf.phyAddrBitstreamBuf = pCnmInputBuf->BufPhyAddr;
	mpeg4->FilePlay_StmBuf.DataLen = pCnmInputBuf->iDataLen;

	bStopDecodingLoop = 0;

	do{
		bCallDecAgain = 0;

		g_mutex_lock(mpeg4->cnmCodecMtx);
		cnmDreamStatusAtlock = mpeg4->bCnmInDream;
		if(cnmDreamStatusAtlock) {
			HibernateWakeupCNM(mpeg4->pMPEG4DecoderState, 0, &mpeg4->bCnmInDream);
		}

		if(0 != gcnmdec_informcodec_decframeconsumed(mpeg4->pMPEG4DecoderState, &mpeg4->ShareBufManager)) {
			mpeg4->iCodecErr |= ERR_CNMDEC_DISPLAYEDAPI;
			g_mutex_unlock(mpeg4->cnmCodecMtx);
			FlowRt = GST_FLOW_ERROR;
			goto chain_exit;
		}

#ifdef DEBUG_PERFORMANCE
		gettimeofday(&((CnmMPEG4ProbeData*)mpeg4->pProbeData)->tstart, &((CnmMPEG4ProbeData*)mpeg4->pProbeData)->tz);
#endif
		codecRt = DecodeFrame_CMVpu(mpeg4->pMPEG4DecoderState, &mpeg4->FilePlay_StmBuf, &mpeg4->DecOutInfo, 0);
#ifdef DEBUG_PERFORMANCE
		gettimeofday(&((CnmMPEG4ProbeData*)mpeg4->pProbeData)->tend, &((CnmMPEG4ProbeData*)mpeg4->pProbeData)->tz);
		mpeg4->codec_time += (((CnmMPEG4ProbeData*)mpeg4->pProbeData)->tend.tv_sec - ((CnmMPEG4ProbeData*)mpeg4->pProbeData)->tstart.tv_sec)*1000000 + (((CnmMPEG4ProbeData*)mpeg4->pProbeData)->tend.tv_usec - ((CnmMPEG4ProbeData*)mpeg4->pProbeData)->tstart.tv_usec);
#endif
		if(! mpeg4->iShareFrameBuf && (codecRt == IPP_STATUS_OUTPUT_DATA || codecRt == IPP_STATUS_FRAME_COMPLETE)) {
			(mpeg4->ShareBufManager.DecOutFrameConsumeStatus[mpeg4->DecOutInfo.idxFrameOutput])++;//In theory, under non-iShareFrameBuf mode, should after copy the frame data, then set the status that the frame is consumed. Anyway, setting it in advance is no side effect.
		}
		if(cnmDreamStatusAtlock) {
			HibernateWakeupCNM(mpeg4->pMPEG4DecoderState, 1, &mpeg4->bCnmInDream);
		}
		g_mutex_unlock(mpeg4->cnmCodecMtx);

		if(!(codecRt == IPP_STATUS_OUTPUT_DATA || (codecRt==IPP_STATUS_BUFFER_FULL && mpeg4->iShareFrameBuf))) {
			//the data has been consumed
			pCnmInputBuf->iDataLen = 0;
		}

		switch(codecRt) {
		case IPP_STATUS_OUTPUT_DATA:
			//No frame is decoded, because the input frame data hasn't been consumed by VPU, therefore, application should feed this frame again.
			//But some frame still need output, which are reorder delayed frames.
			bCallDecAgain = 1;
		case IPP_STATUS_FRAME_COMPLETE:
			gcnm_printfmore("DecodeFrame_CMVpu ret IPP_STATUS_FRAME_COMPLETE, %d, %d, %d\n", mpeg4->totalFrames, mpeg4->DecOutInfo.idxFrameOutput, mpeg4->DecOutInfo.NumOfErrMbs);
			mpeg4->totalFrames++;
			_cnmmpeg4_refresh_downpush_TsDu(mpeg4);
			FlowRt = _cnmmpeg4_push_data(mpeg4, mpeg4->DecOutInfo.idxFrameOutput);
			break;
		case IPP_STATUS_NOERR:
			gcnm_printf("DecodeFrame_CMVpu ret NOERR, %d %d %d\n", mpeg4->DecOutInfo.OneFrameUnbrokenDecoded, mpeg4->DecOutInfo.idxFrameOutput, mpeg4->DecOutInfo.idxFrameDecoded);
			break;
		case IPP_STATUS_NOTSUPPORTED_ERR:
			mpeg4->iCodecErr |= WARN_CNMDEC_FRAMENOTSUPPORT;
			_cnmmpeg4_refresh_downpush_TsDu(mpeg4);
			gcnm_printf("DecodeFrame_CMVpu ret IPP_STATUS_NOTSUPPORTED_ERR.\n");
			break;
		case IPP_STATUS_FRAME_ERR:
			mpeg4->iCodecErr |= WARN_CNMDEC_FRAMECORRUPT;
			gcnm_printf("DecodeFrame_CMVpu ret WARN_CNMDEC_FRAMECORRUPT, probably the frame is corrupt.\n");
			break;
		case IPP_STATUS_BUFFER_FULL:
#ifndef NOTECHO_BUFFERFULL_WARNING
			g_warning("Warning: DecodeFrame_CMVpu ret buffer full, waiting sink release buffer\n");
#endif
			if(mpeg4->iShareFrameBuf) {
				int waitRt;
				bCallDecAgain = 1;
				waitRt = wait_sink_retDecFrameBuf(mpeg4->iTimeoutForDisp1Frame, -1, &mpeg4->ShareBufManager, &mpeg4->ElementState);
				if(waitRt > 0) {
					gcnm_printf("wait_sink_retDecFrameBuf() stopped by element state change\n");
					bStopDecodingLoop = 1;	//element isn't running, give up decoding
					//give up this frame, since we have limitted the length of TS queue, little need to remove the TS of this frame from TS queue
					pCnmInputBuf->iDataLen = 0;
				}else if(waitRt < 0) {
					//wait time out or error occur
					FlowRt = GST_FLOW_ERROR;
					mpeg4->iCodecErr |= (waitRt == -2 ? ERR_WAITSINKRETFRAME_SINKNOFRAME: ERR_WAITSINKRETFRAME_TIMEOUT);
					goto chain_exit;
				}
				break;  //break switch
			}
		default:
			mpeg4->iCodecErr |= ERR_CNMDEC_DECFRAME_SERIOUSERR;
			gcnm_printf("Err: DecodeFrame_CMVpu fail, ret %d\n", codecRt);
			g_warning("Err: DecodeFrame_CMVpu fail, ret %d", codecRt);
			FlowRt = GST_FLOW_ERROR;
			goto chain_exit;
		}
	}while(bCallDecAgain == 1 && bStopDecodingLoop == 0);

chain_exit:
	if(FlowRt < GST_FLOW_UNEXPECTED) {
		GST_ELEMENT_ERROR(mpeg4, STREAM, DECODE, (NULL), ("%s() flow err %d, codec err 0x%x, mem err %d", __FUNCTION__, FlowRt, mpeg4->iCodecErr, mpeg4->iMemErr));
	}

	return FlowRt;
}

static void
gst_cnmmpeg4dec_clean_streambuf(Gstcnmmpeg4dec *mpeg4)
{
	int i;
	gcnm_printf("free stream buffer\n");

	for(i=0;i<GSTCNMDEC_MPEG4_STMPOOLSZ;i++) {
		if(mpeg4->StmBufPool[i].iBufSize != 0) {
			delete_cnmStmBuf(mpeg4->StmBufPool[i].mem);
			mpeg4->StmBufPool[i].iBufSize = 0;
			mpeg4->StmBufPool[i].BufPhyAddr = 0;
			mpeg4->StmBufPool[i].BufVirAddr = NULL;
			mpeg4->StmBufPool[i].iDataLen = 0;
			mpeg4->StmBufPool[i].mem = NULL;
		}
	}

	if(NULL!=mpeg4->codecData) {
		g_free(mpeg4->codecData);
		mpeg4->codecData = NULL;
		mpeg4->codecDataBufSz = 0;
		mpeg4->codecDataLen = 0;
	}
	return;
}

static gboolean
gst_cnmmpeg4dec_null2ready(Gstcnmmpeg4dec *mpeg4)
{
	gcnm_printf("enter %s\n", __func__);

	mpeg4->pMPEG4DecoderState = NULL;
	mpeg4->totalFrames = 0;
	mpeg4->codec_time = 0;

	mpeg4->ShareBufManager.watcher = gcnmdec_watcher_create(mpeg4, NULL, &mpeg4->ShareBufManager); //create watcher
	if(mpeg4->ShareBufManager.watcher == NULL) {
		g_warning("Err: gcnmdec_watcher_create() fail");
		return FALSE;
	}

	return TRUE;
}


static gboolean
gst_cnmmpeg4dec_ready2null(Gstcnmmpeg4dec *mpeg4)
{
	gcnm_printf("enter %s\n", __func__);

	//close decoder
	g_mutex_lock(mpeg4->cnmCodecMtx);
	if(mpeg4->pMPEG4DecoderState) {
		if(mpeg4->bCnmInDream) {
			HibernateWakeupCNM(mpeg4->pMPEG4DecoderState, 0, &mpeg4->bCnmInDream);
		}
		mpeg4->iCodecErr |= gcnmdec_close_codec(&mpeg4->pMPEG4DecoderState, &mpeg4->ShareBufManager);
	}
	g_mutex_unlock(mpeg4->cnmCodecMtx);

	gcnmdec_remove_redundant_TsDu(&mpeg4->TsDuManager, 0);

	//free buffers
	gst_cnmmpeg4dec_clean_streambuf(mpeg4);

	if(! mpeg4->iShareFrameBuf) {
		gstcnmElement_clean_nonShareFramebuf(mpeg4->DecFramesInfoArr, &mpeg4->iDecFrameBufArrLen);
	}else{
		g_mutex_lock(mpeg4->ShareBufManager.mtx);
		gstcnmElement_clean_idleShareFBs(&mpeg4->ShareBufManager, NULL);	//for those share frame buffer which is still owned by sink, will free them on their gstbuffer finalizing callback function
		mpeg4->ShareBufManager.NotNeedFMemAnyMore = 1;
		g_mutex_unlock(mpeg4->ShareBufManager.mtx);
	}


	if(mpeg4->ShareBufManager.watcher) {
		FRAMES_WATCHMAN_UNREF(mpeg4->ShareBufManager.watcher);  //smart release watcher, free frame buffers
	}

#ifdef DEBUG_PERFORMANCE
	printf("codec totally outputed %d frames, spend %lld us\n", mpeg4->totalFrames, mpeg4->codec_time);
#else
	gcnm_printf("codec totally outputed %d frames, spend %lld us\n", mpeg4->totalFrames, mpeg4->codec_time);
#endif

#ifdef LOG_FRAMECOPYTIME
	printf("copy %d frame total cost %d us\n", ((CnmMPEG4ProbeData*)mpeg4->pProbeData)->log_copycnt, ((CnmMPEG4ProbeData*)mpeg4->pProbeData)->log_copytime);
#endif
	gcnm_printf("-----------At ready2null, c&m codec error %d, memory error %d\n", mpeg4->iCodecErr, mpeg4->iMemErr);

	return TRUE;
}

static int gst_cnmmpeg4dec_ready2paused(Gstcnmmpeg4dec *mpeg4)
{
	gcnm_printf("gst_cnmmpeg4dec_ready2paused() is called\n");
	//prepare for new stream

	return 0;
}

static int gst_cnmmpeg4dec_paused2ready(Gstcnmmpeg4dec* mpeg4)
{
	gcnm_printf("gst_cnmmpeg4dec_paused2ready() is called\n");
	gcnm_printf("-----------At paused2ready, cnm decoder error status %d\n", mpeg4->iCodecErr);
	return 0;
}

static __inline int paused2play(Gstcnmmpeg4dec* mpeg4)
{
	IppCodecStatus ret = IPP_STATUS_NOERR;
	g_mutex_lock(mpeg4->cnmCodecMtx);
	if(mpeg4->pMPEG4DecoderState && mpeg4->bEnableHibernateAtPause && mpeg4->bCnmInDream) {
		ret = HibernateWakeupCNM(mpeg4->pMPEG4DecoderState, 0, &mpeg4->bCnmInDream);
	}
	g_mutex_unlock(mpeg4->cnmCodecMtx);
	if(IPP_STATUS_NOERR != ret) {
		g_warning("Wakeup cnm fail at pause2paly, ret %d", ret);
		return -1;
	}
	return 0;
}

static __inline int play2paused(Gstcnmmpeg4dec* mpeg4)
{
	IppCodecStatus ret = IPP_STATUS_NOERR;
	g_mutex_lock(mpeg4->cnmCodecMtx);
	if(mpeg4->pMPEG4DecoderState && mpeg4->bEnableHibernateAtPause && ! mpeg4->bCnmInDream) {
		ret = HibernateWakeupCNM(mpeg4->pMPEG4DecoderState, 1, &mpeg4->bCnmInDream);
	}
	g_mutex_unlock(mpeg4->cnmCodecMtx);
	if(IPP_STATUS_NOERR != ret) {
		g_warning("Hibernate cnm fail at pause2paly, ret %d", ret);
		return -1;
	}
	return 0;
}


static GstStateChangeReturn
gst_cnmmpeg4dec_change_state(GstElement *element, GstStateChange transition)
{
	gcnm_printf("enter %s, change from %d to %d\n", __func__, transition>>3, transition&7);

	GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;
	Gstcnmmpeg4dec *mpeg4 = GST_CNMMPEG4DEC(element);

	switch (transition)
	{
	case GST_STATE_CHANGE_NULL_TO_READY:
		if(!gst_cnmmpeg4dec_null2ready(mpeg4)){
			goto statechange_failed;
		}
		break;
	case GST_STATE_CHANGE_READY_TO_PAUSED:
		gst_cnmmpeg4dec_ready2paused(mpeg4);
		break;
	case GST_STATE_CHANGE_PAUSED_TO_PLAYING:
		if(0 != paused2play(mpeg4)) {
			goto statechange_failed;
		}
		break;

	case GST_STATE_CHANGE_PAUSED_TO_READY:
		set_ElementState(&mpeg4->ElementState, CNMELESTATE_RESTING);
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
		if(0 != play2paused(mpeg4)) {
			goto statechange_failed;
		}
		break;
	case GST_STATE_CHANGE_PAUSED_TO_READY:
		gst_cnmmpeg4dec_paused2ready(mpeg4);
		break;
	case GST_STATE_CHANGE_READY_TO_NULL:
		if(!gst_cnmmpeg4dec_ready2null(mpeg4)){
			goto statechange_failed;
		}
		break;

	case GST_STATE_CHANGE_READY_TO_PAUSED:
		set_ElementState(&mpeg4->ElementState, CNMELESTATE_RUNNING);
		break;
	default:
		break;
	}

	return ret;

	 /* ERRORS */
statechange_failed:
	{
		/* subclass must post a meaningfull error message */
		GST_ERROR_OBJECT (mpeg4, "state change failed");
		return GST_STATE_CHANGE_FAILURE;
	}
}

static void
gst_cnmmpeg4dec_base_init (gpointer klass)
{
	gcnm_printf("enter %s\n", __func__);

	static GstElementDetails mpeg4dec_details = {
		"IPPCNM decoder MPEG4",
		"Codec/Decoder/Video",
		"HARDWARE MPEG4 Decoder based-on IPP CNM CODEC",
		""
	};

	GstElementClass *element_class = GST_ELEMENT_CLASS (klass);

	gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&src_factory));
	gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&sink_factory));
	gst_element_class_set_details (element_class, &mpeg4dec_details);
}

static void
gst_cnmmpeg4dec_finalize(GObject * object)
{
	Gstcnmmpeg4dec* mpeg4 = GST_CNMMPEG4DEC(object);
	if(mpeg4->pProbeData) {
		g_free(mpeg4->pProbeData);
	}

	g_mutex_free(mpeg4->cnmCodecMtx);

	g_mutex_free(mpeg4->ElementState.elestate_mtx);

	GST_DEBUG_OBJECT(mpeg4, "Gstcnmmpeg4dec instance(%p) parent class is finalizing, codec err 0x%x, mem err %d.", mpeg4, mpeg4->iCodecErr, mpeg4->iMemErr);
	G_OBJECT_CLASS(parent_class)->finalize(object);

	gcnm_printf("Gstcnmmpeg4dec instance 0x%x is finalized!!!\n", (unsigned int)object);
	printf("Gstcnmmpeg4dec instance (0x%x) is finalized\n", (unsigned int)object);

	return;
}


static void
gst_cnmmpeg4dec_class_init (Gstcnmmpeg4decClass *klass)
{
	gcnm_printf("enter %s\n", __func__);

	GObjectClass *gobject_class  = (GObjectClass*) klass;
	GstElementClass *gstelement_class = (GstElementClass*) klass;

	gobject_class->set_property = gst_cnmmpeg4dec_set_property;
	gobject_class->get_property = gst_cnmmpeg4dec_get_property;

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
		g_param_spec_int ("totalframes", "Totalframe of cnm mpeg4 decoder outputted",
			"Number of frame of cnm mpeg4 decoder outputted", 0, G_MAXINT, 0, G_PARAM_READABLE));
	g_object_class_install_property (gobject_class, ARG_CODECTIME,
		g_param_spec_int64 ("codectime", "cnm mpeg4 decode time",
			"Pure codec time of calling cnm mpeg4 decoding API(microsecond)", 0, G_MAXINT64, 0, G_PARAM_READABLE));
#endif

	gobject_class->finalize = GST_DEBUG_FUNCPTR (gst_cnmmpeg4dec_finalize);
	gstelement_class->change_state = GST_DEBUG_FUNCPTR (gst_cnmmpeg4dec_change_state);
	GST_DEBUG_CATEGORY_INIT(cnmdec_mpeg4_debug, "cnmdec_mpeg4", 0, "CNM mpeg4 Decoder Element");
	return;
}

static void gst_cnmmpeg4dec_init_members(Gstcnmmpeg4dec* mpeg4)
{
	gcnm_printf("gst_cnmmpeg4dec_init_members is called\n");

	mpeg4->pProbeData = g_malloc0(sizeof(CnmMPEG4ProbeData));

	mpeg4->pMPEG4DecoderState = NULL;
	mpeg4->num = 0;
	mpeg4->den = 1;
	mpeg4->bCodecSucLaunched = 0;
	mpeg4->b1stFrame_forFileplay = 1;
	mpeg4->iCodecErr = 0;
	mpeg4->iMemErr = 0;
	mpeg4->codecData = NULL;
	mpeg4->codecDataBufSz= 0;
	mpeg4->codecDataLen = 0;

	memset(&mpeg4->SeqInfo, 0, sizeof(mpeg4->SeqInfo));
	memset(mpeg4->DecFramesInfoArr, 0, sizeof(mpeg4->DecFramesInfoArr));
	mpeg4->iDecFrameBufArrLen = 0;
	memset(&mpeg4->FilePlay_StmBuf, 0, sizeof(mpeg4->FilePlay_StmBuf));
	memset(&mpeg4->DecOutInfo, 0, sizeof(mpeg4->DecOutInfo));
	memset(&mpeg4->ShareBufManager, 0, sizeof(mpeg4->ShareBufManager));

	mpeg4->ElementState.state = CNMELESTATE_RESTING;
	mpeg4->ElementState.elestate_mtx = g_mutex_new();

	memset(mpeg4->StmBufPool, 0, sizeof(mpeg4->StmBufPool));
	memset(&mpeg4->DownFrameLayout, 0, sizeof(mpeg4->DownFrameLayout));
	gcnmdec_init_TsDuMgr(&mpeg4->TsDuManager);

	mpeg4->bVOLReceived = 0;
	mpeg4->bWaitIVop = 1;

	mpeg4->iI420LayoutKind = 0;
	mpeg4->iShareFrameBuf = 1;
	mpeg4->iTimeoutForDisp1Frame = WAIT_DOWNSTREAM_RETBUF_TIMEOUT;
	mpeg4->iSupportMultiIns = 1;
	mpeg4->iIgnPBInNewseg = 1;
	mpeg4->bEnableHibernateAtPause = 1;
	mpeg4->bEnableDynaClk = 1;

	mpeg4->bNewSegReceivedAfterflush = 0;

	mpeg4->cnmCodecMtx = g_mutex_new();
	mpeg4->bCnmInDream = 0;

	mpeg4->totalFrames = 0;
	mpeg4->codec_time = 0;
	return;
}

static void
gst_cnmmpeg4dec_init(Gstcnmmpeg4dec *mpeg4, Gstcnmmpeg4decClass *mpeg4_klass)
{
	gcnm_printf("enter %s\n", __func__);

	GstElementClass *klass = GST_ELEMENT_CLASS(mpeg4_klass);

	mpeg4->sinkpad = gst_pad_new_from_template (
		gst_element_class_get_pad_template(klass, "sink"), "sink");

	gst_pad_set_setcaps_function (mpeg4->sinkpad, GST_DEBUG_FUNCPTR (gst_cnmmpeg4dec_sinkpad_setcaps));
	gst_pad_set_chain_function (mpeg4->sinkpad, GST_DEBUG_FUNCPTR (gst_cnmmpeg4dec_chain));
	gst_pad_set_event_function (mpeg4->sinkpad, GST_DEBUG_FUNCPTR (gst_cnmmpeg4dec_sinkpad_event));

	gst_element_add_pad(GST_ELEMENT(mpeg4), mpeg4->sinkpad);

	mpeg4->srcpad = gst_pad_new_from_template (
		gst_element_class_get_pad_template(klass, "src"), "src");

	gst_element_add_pad(GST_ELEMENT(mpeg4), mpeg4->srcpad);

	gst_cnmmpeg4dec_init_members(mpeg4);

	gcnm_printf("Gstcnmmpeg4dec instance 0x%x is inited!!!\n", (unsigned int)mpeg4);
	printf("Gstcnmmpeg4dec instance (0x%x) is inited\n", (unsigned int)mpeg4);

	return;
}

/* EOF */
