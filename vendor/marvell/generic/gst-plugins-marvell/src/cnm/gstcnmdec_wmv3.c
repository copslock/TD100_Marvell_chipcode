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

/* gstcnmdec_wmv3.c */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h> //to include memset(),...
#include <unistd.h> //to include usleep()
#include <sys/time.h>	//to include time functions and structures

#include "gstcnmdec_wmv3.h"
#include "cnm_bridge.h"
#include "ippGSTdefs.h"

//Define IPPGST_CNMWMV_ENABLE_DIRECTMVREFORM will enable VC1 MP DirectModeMv frame reform function.
//This feature is to workaround for C&M known RTL issue, but it has some side effect.
//It use repeated frame to replace DirectModeMv frame, therefore, it decrease the real fps. Sometimes, real fps downgrades 50% ~ 75%.
//Sometimes, it will reform the frame which isn't need reform.
#define IPPGST_CNMWMV_ENABLE_DIRECTMVREFORM

//#define LOG_FRAMECOPYTIME

typedef struct{
	struct timeval tstart;
	struct timeval tend;
	struct timezone tz;
	struct timeval tlog_0;
	struct timeval tlog_1;
	unsigned int log_copytime;
	unsigned int log_copycnt;
}CnmWMV3ProbeData;

GST_DEBUG_CATEGORY_STATIC (cnmdec_wmv3_debug);
#define GST_CAT_DEFAULT cnmdec_wmv3_debug

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
	ARG_ENABLEHIBERNATEATPAUSE,
	ARG_ENABLEDYNACLK,

#ifdef DEBUG_PERFORMANCE
	ARG_TOTALFRAME,
	ARG_CODECTIME,
#endif

};

//#define IPPGST_FORCEBLOCK_WMV12
#ifndef IPPGST_FORCEBLOCK_WMV12
//CNM support height < 1024 and should be even, therefore 1022
#define IPPGST_CNMWMV_SINKCAPTEMPLATE	"video/x-wmv, wmvversion = (int) 3, format=(fourcc)WMV3, fourcc=(fourcc)WMV3, width = (int) [ 16, 1280 ], height = (int) [ 16, 1022 ], framerate = (fraction) [ 0, MAX ], cnmnotsupport = (int) 0"
#else
//block wmv1 & wmv2
#define IPPGST_CNMWMV_SINKCAPTEMPLATE	"video/x-wmv, wmvversion = (int) 3, format=(fourcc)WMV3, fourcc=(fourcc)WMV3, width = (int) [ 16, 1280 ], height = (int) [ 16, 1022 ], framerate = (fraction) [ 0, MAX ], cnmnotsupport = (int) 0; video/x-wmv, wmvversion = (int) {1, 2}, format=(fourcc){ WMV1, WMV2 }, fourcc=(fourcc){ WMV1, WMV2 }"
#endif


#define IPPGST_CNMWMV_VSINK_HOLDBUFCNT		3	//The surfaceflingersink probably hold some buffers. Sometimes, we need to push some dummy buffers to let video sink release those buffers. If videosink won't hold any buffers, it could be 0.

static GstStaticPadTemplate sink_factory =
	GST_STATIC_PAD_TEMPLATE ("sink", GST_PAD_SINK, GST_PAD_ALWAYS,
	GST_STATIC_CAPS (IPPGST_CNMWMV_SINKCAPTEMPLATE)
					 );

static GstStaticPadTemplate src_factory =
	GST_STATIC_PAD_TEMPLATE ("src", GST_PAD_SRC, GST_PAD_ALWAYS,
	GST_STATIC_CAPS ("video/x-raw-yuv,"
					 "format = (fourcc) { I420 }, "
					 "width = (int) [ 16, 1280 ], "
					 "height = (int) [ 16, 1022 ], "
					 "framerate = (fraction) [ 0, MAX ]")
					 );



GST_BOILERPLATE (Gstcnmwmvdec, gst_cnmwmvdec, GstElement, GST_TYPE_ELEMENT);


static gboolean
gst_cnmwmvdec_sinkpad_setcaps (GstPad *pad, GstCaps *caps)
{
	gcnm_printf("enter gst_cnmwmvdec_sinkpad_setcaps().\n");

	Gstcnmwmvdec *wmv = GST_CNMWMVDEC (GST_PAD_PARENT (pad));
	int i = 0;
	GstStructure *str;
	const GValue *value;
	int wid, hei;

	int stru_num = gst_caps_get_size(caps);

	gcnm_printf("gst_cnmwmvdec_sinkpad_setcaps stru_num =%d\n",stru_num);

	if(stru_num != 1) {
		gchar* sstr;
		GST_WARNING_OBJECT(wmv, "Multiple MIME stream type in sinkpad caps, only select the first item.");
		for(i=0; i<stru_num; i++) {
			str = gst_caps_get_structure(caps, i);
			sstr = gst_structure_to_string(str);
			GST_WARNING_OBJECT(wmv, "struture %d is %s.", i, sstr);
			g_free(sstr);
		}
	}
	str = gst_caps_get_structure(caps, 0);

	wmv->wmvversion = 0;
	gst_structure_get_int(str, "wmvversion", &wmv->wmvversion);
	GST_DEBUG_OBJECT(wmv, "wmvversion is %d",  wmv->wmvversion);
	if(wmv->wmvversion != 3) {
		//c&m not support wmv1/wmv2, will return error at _chain
		return TRUE;
	}

	if(G_UNLIKELY(FALSE == gst_structure_get_fraction(str, "framerate", &wmv->num, &wmv->den))) {
		wmv->num = 0;   	//if this information isn't in caps, we assume it is 0
		wmv->den = 1;
	}

	gcnm_printf("In gst_cnmwmvdec_sinkpad_setcaps, fps: %d/%d\n", wmv->num, wmv->den);

	if(wmv->num != 0) {
		wmv->TsDuManager.FrameFixPeriod = gst_util_uint64_scale_int(GST_SECOND, wmv->den, wmv->num);
	}

	gcnm_printf("In gst_cnmwmvdec_sinkpad_setcaps, frame duration %lld ns\n",  wmv->TsDuManager.FrameFixPeriod);


	if(gst_structure_get_int(str, "width", &wid)){
		gcnm_printf("width=%d\n", wid);
	}else{
		GST_ERROR_OBJECT(wmv, "Err: couldn't get width in setcaps");
		return FALSE;
	}

	if(gst_structure_get_int(str, "height", &hei)){
		gcnm_printf("height=%d\n", hei);
	}else{
		GST_ERROR_OBJECT(wmv, "Err: couldn't get height in setcaps");
		return FALSE;
	}

	if((value = gst_structure_get_value (str, "codec_data"))) {
		GstBuffer* buf = gst_value_get_buffer(value);
		unsigned char* cdata = GST_BUFFER_DATA(buf);
		unsigned int csize = GST_BUFFER_SIZE(buf);
		int rounded_fps;

		GST_DEBUG_OBJECT(wmv, "codec_data len is %d in %s(), 4 is standard, 5 or 6 still is acceptable for c&m codec", csize, __FUNCTION__);
		if(csize < 4){
			GST_ERROR_OBJECT(wmv, "Err: codec_data len is %d in %s(), too small", csize, __FUNCTION__);
			return FALSE;
		}
		if((cdata[0]>>4) != 0 && (cdata[0]>>4) != 4) {
			GST_ERROR_OBJECT(wmv, "Err: codec_data[0] is 0x%x, neither simple profile nor main profile, not supported", cdata[0]);
			return FALSE;
		}
#define IPPCODEC_SUPPORT_STRUCTC_MAXLEN		5
		//SMPTE 421M, Annex L. The last bit in STRUCT_C[3] stand for different encoder version(latest spec doesn't claim that). 0 means beta version, 1 means RTM version.
		//In lastest spec, STRUCT_C len should be 4, but for some early version, STRUCT_C probably is 5 even 6. But the data after STRUCT_C[3] doesn't affect decoding process.
		//Though c&m could accept 6 bytes for STRUCT_C, we still trunc it to IPPCODEC_SUPPORT_STRUCTC_MAXLEN bytes. Not trunc also should be ok.
#if 1
		if((cdata[3]&1) == 1 && csize > IPPCODEC_SUPPORT_STRUCTC_MAXLEN) {
			GST_DEBUG_OBJECT(wmv, "trunc STRUCT_C data from %d to "G_STRINGIFY(IPPCODEC_SUPPORT_STRUCTC_MAXLEN)" in %s()", csize, __FUNCTION__);
			csize = IPPCODEC_SUPPORT_STRUCTC_MAXLEN;
		}
#endif
		if(csize > sizeof(wmv->seqMeta) - 32) {
			GST_ERROR_OBJECT(wmv, "Err: STRUCT_C len is %d in %s(), too large", csize, __FUNCTION__);
			return FALSE;
		}

		if(wmv->den && wmv->num) {
			rounded_fps = wmv->num/wmv->den;
		}else{
			rounded_fps = 0xffffffff;
		}
		//SMPTE 421M, Annex L
		memset(wmv->seqMeta, 0, sizeof(wmv->seqMeta));
		wmv->seqMetaLen = 0;
		(wmv->seqMeta)[0] = 0xff;
		(wmv->seqMeta)[1] = 0xff;
		(wmv->seqMeta)[2] = 0xff;   //NUMFRAMES = 0xffffff
		(wmv->seqMeta)[3] = 0xC5;
		(wmv->seqMeta)[4] = csize;
		memcpy(&(wmv->seqMeta[8]), cdata, csize);
		wmv->seqMetaLen = 8+csize;
		(wmv->seqMeta)[wmv->seqMetaLen++] = hei & 0xff;
		(wmv->seqMeta)[wmv->seqMetaLen++] = (hei>>8) & 0xff;
		(wmv->seqMeta)[wmv->seqMetaLen++] = (hei>>16) & 0xff;
		(wmv->seqMeta)[wmv->seqMetaLen++] = (hei>>24) & 0xff;
		(wmv->seqMeta)[wmv->seqMetaLen++] = wid & 0xff;
		(wmv->seqMeta)[wmv->seqMetaLen++] = (wid>>8) & 0xff;
		(wmv->seqMeta)[wmv->seqMetaLen++] = (wid>>16) & 0xff;
		(wmv->seqMeta)[wmv->seqMetaLen++] = (wid>>24) & 0xff;
		(wmv->seqMeta)[wmv->seqMetaLen] = 0x0C;
		wmv->seqMetaLen += 4;

		(wmv->seqMeta)[wmv->seqMetaLen++] = 0xff;   //HRD_BUFFER = 0x007fff
		(wmv->seqMeta)[wmv->seqMetaLen++] = 0x7f;
		(wmv->seqMeta)[wmv->seqMetaLen++] = 0;

		if((cdata[0]>>4) == 4) {	//main profile
			(wmv->seqMeta)[wmv->seqMetaLen++] = ((4<<1)|1)<<4;  //high level, CBR=1
		}else{  //simple profile
			(wmv->seqMeta)[wmv->seqMetaLen++] = ((2<<1)|1)<<4;  //mail level, CBR=1
		}

		(wmv->seqMeta)[wmv->seqMetaLen++] = 0xff;   //HRD_RATE = 0x00007fff
		(wmv->seqMeta)[wmv->seqMetaLen++] = 0x7f;
		wmv->seqMetaLen+=2;

		(wmv->seqMeta)[wmv->seqMetaLen++] = rounded_fps & 0xff; //FRAMERATE = rounded_fps
		(wmv->seqMeta)[wmv->seqMetaLen++] = (rounded_fps>>8) & 0xff;
		(wmv->seqMeta)[wmv->seqMetaLen++] = (rounded_fps>>16) & 0xff;
		(wmv->seqMeta)[wmv->seqMetaLen++] = (rounded_fps>>24) & 0xff;
		gcnm_printf("seq Meta data len %d\n", wmv->seqMetaLen);
	}else{
		GST_ERROR_OBJECT(wmv, "Err: couldn't get codec_data");
		return FALSE;
	}

	gcnm_printf("exit gst_cnmwmvdec_sinkpad_setcaps() succeed\n");
	return TRUE;
}

static void
gst_cnmwmvdec_set_property(GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	Gstcnmwmvdec* wmv = GST_CNMWMVDEC(object);

	switch (prop_id)
	{
	case ARG_I420LAYOUTKIND:
		{
			int layout = g_value_get_int(value);
			if(layout < 0 || layout > 2) {
				GST_ERROR_OBJECT(wmv, "I420layoutkind %d exceed range.", layout);
				layout = 0;
			}
			wmv->iI420LayoutKind = layout;
		}
		break;
	case ARG_SHAREFRAMEBUF:
		{
			int sha = g_value_get_int(value);
			if(sha < 0 || sha > 1) {
				GST_ERROR_OBJECT(wmv, "shareFramebuf %d exceed range.", sha);
				sha = 1;
			}
			wmv->iShareFrameBuf = sha;
		}
		break;
	case ARG_TIMEOUTFORDISP1FRAME:
		{
			int to = g_value_get_int(value);
			if(to < -1) {
				GST_ERROR_OBJECT(wmv, "timeoutDisp1frame %d exceed range.", to);
				to = -1;
			}
			wmv->iTimeoutForDisp1Frame = to;
		}
		break;
	case ARG_SUPPORTMULTIINS:
		{
			int supm = g_value_get_int(value);
			if(supm < 0 || supm > 2) {
				GST_ERROR_OBJECT(wmv, "supportMultiIns %d exceed range.", supm);
				supm = 0;
			}
			wmv->iSupportMultiIns = supm;
		}
		break;
	case ARG_ENABLEHIBERNATEATPAUSE:
		{
			int eh = g_value_get_int(value);
			if(eh < 0 || eh > 1) {
				GST_ERROR_OBJECT(wmv, "cnmHibernateAtPause %d exceed range.", eh);
				eh = 0;
			}
			wmv->bEnableHibernateAtPause = eh;
		}
		break;
	case ARG_ENABLEDYNACLK:
		{
			int ec = g_value_get_int(value);
			if(ec < 0 || ec > 1) {
				GST_ERROR_OBJECT(wmv, "dynamicClock %d exceed range.", ec);
				ec = 0;
			}
			wmv->bEnableDynaClk = ec;
		}
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
	return;
}

static void
gst_cnmwmvdec_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	Gstcnmwmvdec* wmv = GST_CNMWMVDEC(object);

	switch (prop_id)
	{
	case ARG_I420LAYOUTKIND:
		g_value_set_int(value, wmv->iI420LayoutKind);
		break;
	case ARG_SHAREFRAMEBUF:
		g_value_set_int(value, wmv->iShareFrameBuf);
		break;
	case ARG_TIMEOUTFORDISP1FRAME:
		g_value_set_int(value, wmv->iTimeoutForDisp1Frame);
		break;
	case ARG_SUPPORTMULTIINS:
		g_value_set_int(value, wmv->iSupportMultiIns);
		break;
	case ARG_ENABLEHIBERNATEATPAUSE:
		g_value_set_int(value, wmv->bEnableHibernateAtPause);
		break;
	case ARG_ENABLEDYNACLK:
		g_value_set_int(value, wmv->bEnableDynaClk);
		break;

#ifdef DEBUG_PERFORMANCE
	case ARG_TOTALFRAME:
		g_value_set_int(value, wmv->totalFrames);
		break;
	case ARG_CODECTIME:
		g_value_set_int64(value, wmv->codec_time);
		break;
#endif

	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
	return;
}

static __inline void _cnmwmv_refresh_downpush_TsDu(Gstcnmwmvdec* wmv)
{
	gcnmdec_remove_redundant_TsDu(&wmv->TsDuManager, wmv->SeqInfo.MaxReorderDelay+1);
	gcnmdec_refresh_output_TsDu(&wmv->TsDuManager);
	return;
}

static GstFlowReturn _cnmwmv_push_data(Gstcnmwmvdec *wmv, int iDecOutFrameIdx);

static void dummy_1frame_onfinalize(gpointer m)
{
	CnmPContMem* p = (CnmPContMem*)m;
	if(p != NULL) {
		g_slice_free(IPPGSTDecDownBufSideInfo, (gpointer)(p->pOtherData0));
		delete_cnmFrameBuf(p);
	}
	return;
}

static int push_dummy_1frame_withTS(Gstcnmwmvdec* wmv, gint64 ts)
{
	GstFlowReturn FlowRt;
	CnmPContMem* p = new_cnmFrameBuf(wmv->iAlignedBufSz, 0);
	if(p == NULL) {
		return -1;
	}
	GstBuffer* buf = gst_buffer_new();
	void* VA = p->VAddr;
	unsigned int PA = p->PAddr;
	GST_BUFFER_DATA(buf) = VA;
	GST_BUFFER_SIZE(buf) = wmv->iAlignedBufSz;
	gst_buffer_set_caps(buf, GST_PAD_CAPS(wmv->srcpad));
	IPPGSTDecDownBufSideInfo* psideinfo = g_slice_new(IPPGSTDecDownBufSideInfo);
	memset(psideinfo, 0, sizeof(IPPGSTDecDownBufSideInfo));
	setSideInfo(psideinfo, wmv->SeqInfo.FrameROI.x, wmv->SeqInfo.FrameROI.y, wmv->SeqInfo.FrameAlignedWidth, wmv->SeqInfo.FrameAlignedHeight, PA, VA);

	if(wmv->lastPushedShareBufVaddr) {
		//always no cacheable buffer
		//copy last frame content to dummy frame
		memcpy(p->VAddr, wmv->lastPushedShareBufVaddr, wmv->iAlignedBufSz);
	}

	IPPGST_BUFFER_CUSTOMDATA(buf) = (gpointer) psideinfo;

	GST_BUFFER_MALLOCDATA(buf) = (guint8*)p;
	p->pOtherData0 = (void*)psideinfo;
	GST_BUFFER_FREE_FUNC(buf) = (GFreeFunc)dummy_1frame_onfinalize;

	GST_BUFFER_TIMESTAMP(buf) = ts;
	GST_BUFFER_DURATION(buf) = GST_CLOCK_TIME_NONE;

	GST_DEBUG_OBJECT(wmv, "pad push dummy ts %lld, gstbuf %p, bufd %p", ts, buf, GST_BUFFER_DATA(buf));
	FlowRt = gst_pad_push(wmv->srcpad, buf);
	GST_DEBUG_OBJECT(wmv, "pad push dummy gstbuf rt %d", FlowRt);
	return 0;
}

static __inline int push_dummy_frames(Gstcnmwmvdec* wmv)
{
	int i;
	int ret = 0;
	for(i=0;i<IPPGST_CNMWMV_VSINK_HOLDBUFCNT;i++) {
		ret = push_dummy_1frame_withTS(wmv, -1);
		if(ret == -1) {
			break;
		}
	}
	return ret;
}

static gboolean
gst_cnmwmvdec_sinkpad_event (GstPad *pad, GstEvent *event)
{
	gcnm_printf("enter %s, event 0x%x\n", __func__, GST_EVENT_TYPE(event));

	Gstcnmwmvdec* wmv = GST_CNMWMVDEC(GST_PAD_PARENT(pad));

	GST_DEBUG_OBJECT(wmv, "cnmwmv receive event %s", GST_EVENT_TYPE_NAME(event));

	gboolean eventRet;

	switch(GST_EVENT_TYPE (event))
	{
	case GST_EVENT_NEWSEGMENT:
		gcnm_printf("NewSeg event is received.\n");
		{
			gboolean update;
			gdouble rate;
			GstFormat format = GST_FORMAT_UNDEFINED;
			gint64 start = 0, stop, position;
			gcnmdec_remove_redundant_TsDu(&wmv->TsDuManager, 0);
			wmv->TsDuManager.LastOutputTs = GST_CLOCK_TIME_NONE;
			wmv->TsDuManager.LastOutputDu = GST_CLOCK_TIME_NONE;
			wmv->b1stChainAfterNewSeg = 1;

			wmv->TsDuManager.SegmentStartTs = -1;

			if(wmv->iShareFrameBuf && wmv->pWMVDecoderState && wmv->iAlignedBufSz > 0) {
				push_dummy_frames(wmv);
			}

			gst_event_parse_new_segment(event, &update, &rate, &format, &start, &stop, &position);
			if(format == GST_FORMAT_TIME) {
				GST_DEBUG_OBJECT(wmv, "receive time newseg start %lld, stop %lld", start, stop);
				if(start != -1) {
					if(start >= 0) {
						wmv->TsDuManager.SegmentStartTs = start;
					}
				}
			}

			eventRet = gst_pad_push_event(wmv->srcpad, event);
			wmv->bNewSegReceivedAfterflush = 1;
		}
		break;
	case GST_EVENT_EOS:
		gcnm_printf("EOS event is received.\n");
		{
			IppCodecStatus codecRt;
			GstFlowReturn pushRt;
			int InDreamStatusAtLock;
			if(wmv->bCodecSucLaunched && ! IS_CNMDEC_FATALERR(wmv->iCodecErr) && wmv->SeqInfo.MaxReorderDelay > 0) {
				//if there is no reorder delay, all frames have been outputted, no need to call DecodeFrame_CMVpu()
				//VC1 skip frame only occur when no B frame, therefore, VC1 skip frame only occur when reorder delay == 0.
				//If B frame exist, generally speaking, the reorder delay should be 1, therefore, B frame should has been outputted before EOS. Therefore, the DirectModeMv frame should has been outputted before EOS.
				for(;;) {
					
					g_mutex_lock(wmv->cnmCodecMtx);
					InDreamStatusAtLock = wmv->bCnmInDream;
					if(InDreamStatusAtLock) {
						HibernateWakeupCNM(wmv->pWMVDecoderState, 0, &wmv->bCnmInDream);
					}
					if(0 != gcnmdec_informcodec_decframeconsumed(wmv->pWMVDecoderState, &wmv->ShareBufManager)) {
						wmv->iCodecErr |= ERR_CNMDEC_DISPLAYEDAPI;
						g_mutex_unlock(wmv->cnmCodecMtx);
						break;
					}

#ifdef DEBUG_PERFORMANCE
					gettimeofday(&((CnmWMV3ProbeData*)wmv->pProbeData)->tstart, &((CnmWMV3ProbeData*)wmv->pProbeData)->tz);
#endif
					codecRt = DecodeFrame_CMVpu(wmv->pWMVDecoderState, NULL, &wmv->DecOutInfo, 1);
#ifdef DEBUG_PERFORMANCE
					gettimeofday(&((CnmWMV3ProbeData*)wmv->pProbeData)->tend, &((CnmWMV3ProbeData*)wmv->pProbeData)->tz);
					wmv->codec_time += (((CnmWMV3ProbeData*)wmv->pProbeData)->tend.tv_sec - ((CnmWMV3ProbeData*)wmv->pProbeData)->tstart.tv_sec)*1000000 + (((CnmWMV3ProbeData*)wmv->pProbeData)->tend.tv_usec - ((CnmWMV3ProbeData*)wmv->pProbeData)->tstart.tv_usec);
#endif

					if(! wmv->iShareFrameBuf && codecRt == IPP_STATUS_FRAME_COMPLETE) {
						(wmv->ShareBufManager.DecOutFrameConsumeStatus[wmv->DecOutInfo.idxFrameOutput])++;
					}

					if(InDreamStatusAtLock) {
						HibernateWakeupCNM(wmv->pWMVDecoderState, 1, &wmv->bCnmInDream);
					}
					g_mutex_unlock(wmv->cnmCodecMtx);

					if(codecRt == IPP_STATUS_FRAME_COMPLETE) {
						gcnm_printf("DecodeFrame_CMVpu ret IPP_STATUS_FRAME_COMPLETE at EOS, %d\n", wmv->DecOutInfo.idxFrameOutput);
						wmv->totalFrames++;
						_cnmwmv_refresh_downpush_TsDu(wmv);
						//For B frame VC1 MP stream, the last frame's timestamp provided by asfdemuxer isn't less than the stop time of current segment data provided by asfdemxer. Therefore, the basesink thinks the last frame exceed the current segment data's	range,	and	ignore	the	last	frame.
						//to fix it, let last frame's TS be GST_CLOCK_TIME_NONE
						if(wmv->SeqInfo.MaxReorderDelay > 0) {  // >0 means there is B frame
							wmv->TsDuManager.LastOutputTs = GST_CLOCK_TIME_NONE;
						}
						pushRt = _cnmwmv_push_data(wmv, wmv->DecOutInfo.idxFrameOutput);
						if(pushRt == GST_FLOW_ERROR) {
							break;
						}
					}else if(codecRt == IPP_STATUS_BUFFER_FULL) {
						if(wmv->iShareFrameBuf) {
							gcnm_printf("DecodeFrame_CMVpu ret buffer full at EOS, waiting sink release buffer\n");
							GST_WARNING_OBJECT(wmv, "DecodeFrame_CMVpu ret buffer full at EOS, waiting sink release buffer");
							if(0 != wait_sink_retDecFrameBuf(wmv->iTimeoutForDisp1Frame>>1, -1, &wmv->ShareBufManager, &wmv->ElementState)) {
								gcnm_printf("Couldn't wait sink to release a frame buffer at EOS, give up decoding.");
								GST_WARNING_OBJECT(wmv, "Couldn't wait sink to release a frame buffer at EOS, give up decoding.");
								break;
							}
						}else{
							wmv->iCodecErr |= WARN_CNMDEC_EOS;
							gcnm_printf("codec return buffer_full under non-shareframe mode at EOS, it's mistake, give up decoding");
							GST_WARNING_OBJECT(wmv, "codec return buffer_full under non-shareframe mode at EOS, it's mistake, give up decoding");
							break;
						}
					}else if(codecRt == IPP_STATUS_BS_END) {
						gcnm_printf("DecodeFrame_CMVpu ret IPP_STATUS_BS_END at EOS\n");
						break;
					}else{
						wmv->iCodecErr |= WARN_CNMDEC_EOS;
						GST_WARNING_OBJECT(wmv, "DecodeFrame_CMVpu() in EOS ret %d, isn't expected", codecRt);
						break;
					}
				}
			}

			//once DecodeFrame_CMVpu return IPP_STATUS_BS_END, the codec couldn't accept new data. If application want to decoding continue(for example, seek to begin, and play again), need to close and init a new codec
			g_mutex_lock(wmv->cnmCodecMtx);
			if(wmv->pWMVDecoderState) {
				if(wmv->bCnmInDream) {
					HibernateWakeupCNM(wmv->pWMVDecoderState, 0, &wmv->bCnmInDream);
				}
				wmv->iCodecErr |= gcnmdec_close_codec(&wmv->pWMVDecoderState, &wmv->ShareBufManager);
			}
			g_mutex_unlock(wmv->cnmCodecMtx);

			wmv->b1stFrame_forFileplay = 1;
			wmv->bCodecSucLaunched = 0;
			gcnmdec_remove_redundant_TsDu(&wmv->TsDuManager, 0);
			wmv->TsDuManager.LastOutputTs = GST_CLOCK_TIME_NONE;
			wmv->TsDuManager.LastOutputDu = GST_CLOCK_TIME_NONE;
			eventRet = gst_pad_push_event(wmv->srcpad, event);
		}
		break;
	case GST_EVENT_FLUSH_STOP:
		wmv->bNewSegReceivedAfterflush = 0;
	default:
		eventRet = gst_pad_event_default(pad, event);
		break;
	}

	return eventRet;
}

static GstFlowReturn _cnmwmv_push_data(Gstcnmwmvdec* wmv, int iDecOutFrameIdx)
{
	GstFlowReturn FlowRt = GST_FLOW_OK;
	GstBuffer *outBuf = NULL;

	gcnm_printfmore("enter %s\n", __func__);

	if(wmv->iShareFrameBuf) {
		int idxInDFBShare;
		void* VA = wmv->DecFramesInfoArr[iDecOutFrameIdx].pUserData1;
		unsigned int PA = (unsigned int)((CnmPContMem*)wmv->DecFramesInfoArr[iDecOutFrameIdx].phyY)->PAddr;
		outBuf = gst_buffer_new();
		if(outBuf == NULL) {
			return GST_FLOW_ERROR;
		}
		GST_BUFFER_DATA(outBuf) = VA;
		GST_BUFFER_SIZE(outBuf) = wmv->iAlignedBufSz;
		gst_buffer_set_caps(outBuf, GST_PAD_CAPS(wmv->srcpad));
		idxInDFBShare = ((int)(wmv->DecFramesInfoArr[iDecOutFrameIdx].pUserData2)>>16) & 0xff;
		IPPGST_BUFFER_CUSTOMDATA(outBuf) = (gpointer) (&wmv->ShareBufManager.sideinfoDFBShareArr[idxInDFBShare]);
		setSideInfo(&wmv->ShareBufManager.sideinfoDFBShareArr[idxInDFBShare], wmv->SeqInfo.FrameROI.x, wmv->SeqInfo.FrameROI.y, wmv->SeqInfo.FrameAlignedWidth, wmv->SeqInfo.FrameAlignedHeight, PA, VA);
		wmv->DecFramesInfoArr[iDecOutFrameIdx].pUserData2 = (void*)((idxInDFBShare<<16)|wmv->ShareBufManager.CurCodecSerialNum|(iDecOutFrameIdx<<24));
		wmv->DecFramesInfoArr[iDecOutFrameIdx].pUserData3 = (void*)(&wmv->ShareBufManager);
		GST_BUFFER_MALLOCDATA(outBuf) = (guint8*)(&(wmv->DecFramesInfoArr[iDecOutFrameIdx]));
		GST_BUFFER_FREE_FUNC(outBuf) = (GFreeFunc)gcnmdec_downbuf_onfinalize;
		FRAMES_WATCHMAN_REF(wmv->ShareBufManager.watcher);

		GST_BUFFER_TIMESTAMP(outBuf) = wmv->TsDuManager.LastOutputTs;
		GST_BUFFER_DURATION(outBuf) = wmv->TsDuManager.LastOutputDu;

		g_mutex_lock(wmv->ShareBufManager.mtx);
		wmv->ShareBufManager.DFBShareArr[idxInDFBShare].pUserData3 = (void*)1;	//DFBShareArr[x].pUserData3 == 0 means display sink do not hold this frame, 1 means display sink is holding this frame.
		wmv->ShareBufManager.iSinkOwnedDecFrameBufCnt++;
		g_mutex_unlock(wmv->ShareBufManager.mtx);

#if 0
		if(wmv->TsDuManager.SegmentStartTs >= 0 && GST_BUFFER_TIMESTAMP(outBuf) != GST_CLOCK_TIME_NONE) {
			if(GST_BUFFER_TIMESTAMP(outBuf) < (guint64)wmv->TsDuManager.SegmentStartTs) {
				GST_DEBUG_OBJECT(wmv, "TS %lld exceeds seg start TS %lld", GST_BUFFER_TIMESTAMP(outBuf), wmv->TsDuManager.SegmentStartTs);
			}
		}
		GST_DEBUG_OBJECT(wmv, "_push gstbuf %p buf data %p TS %lld idx %d", outBuf, GST_BUFFER_DATA(outBuf), GST_BUFFER_TIMESTAMP(outBuf), iDecOutFrameIdx);
#endif

		wmv->lastPushedShareBufVaddr = GST_BUFFER_DATA(outBuf);

		if(wmv->bNewSegReceivedAfterflush) {
			int bNotPush = 0;
			//below adjustment is only for improve subjective feeling, could comment this adjustment by making bNotPush = 1
			if(wmv->TsDuManager.SegmentStartTs != -1 && GST_BUFFER_TIMESTAMP(outBuf) != GST_CLOCK_TIME_NONE && wmv->adjustTsForNewSegGap != 0) {
				if(GST_BUFFER_TIMESTAMP(outBuf) < (GstClockTime)wmv->TsDuManager.SegmentStartTs + wmv->adjustTsForNewSegGap) {
					bNotPush = 1;
				}
			}

			if(G_LIKELY(bNotPush==0)) {
				//GST_DEBUG_OBJECT(wmv, "=== wmv3 push gbuf=%p, dat=%p, allocd=%p, ts=%lld", outBuf, GST_BUFFER_DATA(outBuf), GST_BUFFER_MALLOCDATA(outBuf), GST_BUFFER_TIMESTAMP(outBuf));
				FlowRt = gst_pad_push(wmv->srcpad, outBuf);
			}else{
				gst_buffer_unref(outBuf);
				FlowRt = GST_FLOW_OK;
			}
		}else{
			//If new segment event hasn't received, shouldn't push buffer to sink. Otherwise, sink will throw warnings and work abnormal.
			//Sometimes, user seeking position exceeds the duration of file, then demuxer only fire flush event without newseg event. And demuxer also fire EOS event. At EOS, decoder probably still output some frames, but those frames shouldn't be pushed to sink because no newseg event.
			gst_buffer_unref(outBuf);
			FlowRt = GST_FLOW_OK;
		}
		if(GST_FLOW_OK != FlowRt) {
			if(FlowRt != GST_FLOW_WRONG_STATE) {
				//during seeking, sink element often return GST_FLOW_WRONG_STATUS. No need to echo this message
				GST_WARNING_OBJECT(wmv, "gst_pad_push gstbuffer to downstream plugin fail, ret %d!", FlowRt);
			}
		}
		return FlowRt;
	}

	FlowRt = gcnmdec_require_framebuf_fromdownplugin(&wmv->DownFrameLayout, wmv->srcpad, &outBuf, wmv->iI420LayoutKind==0);
	if(GST_FLOW_OK != FlowRt) {
		if(FlowRt != GST_FLOW_WRONG_STATE) {
			//during seeking, sink element often return GST_FLOW_WRONG_STATUS. No need to echo this message
			GST_WARNING_OBJECT(wmv, "Require gstbuffer from downstream plugin fail, ret %d!", FlowRt);
		}
		return FlowRt;
	}
#ifdef LOG_FRAMECOPYTIME
	gettimeofday(&((CnmWMV3ProbeData*)wmv->pProbeData)->tlog_0, NULL);
#endif
	gcnmdec_copy_to_downframebuf(GST_BUFFER_DATA(outBuf), &wmv->DownFrameLayout, &(wmv->DecFramesInfoArr[iDecOutFrameIdx]), &(wmv->SeqInfo.FrameROI));
#ifdef LOG_FRAMECOPYTIME
	gettimeofday(&((CnmWMV3ProbeData*)wmv->pProbeData)->tlog_1, NULL);
	((CnmWMV3ProbeData*)wmv->pProbeData)->log_copycnt++;
	((CnmWMV3ProbeData*)wmv->pProbeData)->log_copytime += (((CnmWMV3ProbeData*)wmv->pProbeData)->tlog_1.tv_sec - ((CnmWMV3ProbeData*)wmv->pProbeData)->tlog_0.tv_sec)*1000000 + (((CnmWMV3ProbeData*)wmv->pProbeData)->tlog_1.tv_usec - ((CnmWMV3ProbeData*)wmv->pProbeData)->tlog_0.tv_usec);
#endif


	GST_BUFFER_TIMESTAMP(outBuf) = wmv->TsDuManager.LastOutputTs;
	GST_BUFFER_DURATION(outBuf) = wmv->TsDuManager.LastOutputDu;
	gcnm_printfmore("gst_pad_push Ts %lld, Du %lld\n", GST_BUFFER_TIMESTAMP(outBuf), GST_BUFFER_DURATION(outBuf));
	if(wmv->bNewSegReceivedAfterflush) {
		//GST_DEBUG_OBJECT(wmv, "gst_pad_push Ts %lld, Du %lld\n", GST_BUFFER_TIMESTAMP(outBuf), GST_BUFFER_DURATION(outBuf));
		FlowRt = gst_pad_push(wmv->srcpad, outBuf);
	}else{
		//If new segment event hasn't received, shouldn't push buffer to sink. Otherwise, sink will throw warnings and work abnormal.
		//Sometimes, user seeking position exceeds the duration of file, then demuxer only fire flush event without newseg event. And demuxer also fire EOS event. At EOS, decoder probably still output some frames, but those frames shouldn't be pushed to sink because no newseg event.
		gst_buffer_unref(outBuf);
		FlowRt = GST_FLOW_OK;
	}
	if(GST_FLOW_OK != FlowRt) {
		if(FlowRt != GST_FLOW_WRONG_STATE) {
			//during seeking, sink element often return GST_FLOW_WRONG_STATUS. No need to echo this message
			GST_WARNING_OBJECT(wmv, "gst_pad_push gstbuffer to downstream plugin fail, ret %d!", FlowRt);
		}
	}
	return FlowRt;

}

static __inline IppCMVpuDecFrameAddrInfo*
new1buf_inSFBRepo(SkipFrameBufRepo* repo, int wantsz)
{
	CnmPContMem* mem;
	IppCMVpuDecFrameAddrInfo* pNode = &(repo->SFBs[repo->len]);
	//always non-cached
	mem = new_cnmFrameBuf(wantsz, 0);
	if(mem == NULL) {
		return NULL;
	}

	pNode->phyY = (unsigned int)mem;
	pNode->pUserData0 = (void*)(0|(repo->len));	//high 16 bits indicate the node is idle(0 idle, 1 busy), low 16 bit indicate the index of node in SFBs array.
	pNode->pUserData1 = mem->VAddr;
	pNode->pUserData2 = (void*)-1;			//indicate it's skip frame buffer
	//not care physical address, because codec won't use the buffer in SkipFrameBufRepo

	repo->len++;
	
	return pNode;
}

static IppCMVpuDecFrameAddrInfo*
seek_1idlebuf_inSFBRepo(SkipFrameBufRepo* repo, GMutex* mtx)
{
	int i;
	g_mutex_lock(mtx);
	for(i=0;i<repo->len;i++) {
		if(((unsigned int)(repo->SFBs[i].pUserData0)>>16) == 0) {
			break;
		}
	}
	g_mutex_unlock(mtx);
	if(i<repo->len) {
		return &(repo->SFBs[i]);
	}
	return NULL;
}


#define MIN_SKIPFRAMEBUF_NUM_SFBREPO	3	//at least, there should be 3 buffer in skip frame buffer repo, because skip often occur sequentially and display sink couldn't display the frame immediately.
#define IPPGST_CMVPUDEC_FRAMEBUF_DEFAULT_MAXTOTALSIZE_VC1		(20*1024*1024)
static __inline int
decide_whether_new1SFBuf(Gstcnmwmvdec* wmv)
{
	if(wmv->SFBufRepo.len < MIN_SKIPFRAMEBUF_NUM_SFBREPO) {
		return 1;
	}
	if(wmv->SFBufRepo.len < MAX_CAPACITY_SFBREPO && (wmv->SFBufRepo.len+wmv->iDecFrameBufArrLen)*wmv->iAlignedBufSz < IPPGST_CMVPUDEC_FRAMEBUF_DEFAULT_MAXTOTALSIZE_VC1) {
		return 1;
	}
	return 0;
}

static IppCMVpuDecFrameAddrInfo*
wait_sink_return1SFB(Gstcnmwmvdec* wmv)
{
	int totalwaittime;
	IppCMVpuDecFrameAddrInfo* pNode = NULL;
	if(wmv->iTimeoutForDisp1Frame == -1) {
		totalwaittime = 1;
	}else{
		totalwaittime = wmv->iTimeoutForDisp1Frame;
	}

	while(totalwaittime > 0 && NULL == (pNode = seek_1idlebuf_inSFBRepo(&wmv->SFBufRepo, wmv->ShareBufManager.mtx))) {
		usleep(CHECK_DOWNSTREAM_DISPLAYFRAME_PERIOD);
		if(wmv->iTimeoutForDisp1Frame != -1) {
			totalwaittime -= CHECK_DOWNSTREAM_DISPLAYFRAME_PERIOD;
		}
	}
	if(totalwaittime <= 0) {
		gcnm_printf("Err: wait down stream plug-in displayed frame timeout in %s\n", __FUNCTION__);
		return NULL;
	}
	return pNode;
}

static IppCMVpuDecFrameAddrInfo* 
rent_buffer_forrepeatframe(Gstcnmwmvdec* wmv)
{
	IppCMVpuDecFrameAddrInfo * pNode;
	//look for an idle buffer
	pNode = seek_1idlebuf_inSFBRepo(&wmv->SFBufRepo, wmv->ShareBufManager.mtx);
	if(pNode) {
		gcnm_printfmore("found one idle skip frame buf 0x%x in repo\n", (unsigned int)pNode);
	}else{
		gcnm_printf("Not found idle skip frame buf in repo\n");
		if(decide_whether_new1SFBuf(wmv) == 1) {
			gcnm_printf("skip frame buf repo len is small %d, will new a skip frame buf\n", wmv->SFBufRepo.len);
			pNode = new1buf_inSFBRepo(&wmv->SFBufRepo, wmv->iAlignedBufSz);
			gcnm_printf("new a skip frame buf %s, then len %d\n", pNode == NULL ? "fail!!!":"suc", wmv->SFBufRepo.len);
		}else{
			//wait video sink displayed a skip frame
			pNode = wait_sink_return1SFB(wmv);
		}
	}
	if(pNode) {
		unsigned int idx = (unsigned int)(pNode->pUserData0) & 0xffff;
		pNode->pUserData0 = (void*) ((1<<16)|idx);	//it is busy
	}
	return pNode;
}

static GstFlowReturn _cnmwmv_consume_repeatframe(Gstcnmwmvdec* wmv, int iDecOutFrameIdx)
{
	GstFlowReturn FlowRt = GST_FLOW_OK;
	GstBuffer *outBuf = NULL;
	int idx;

	gcnm_printfmore("enter %s\n", __func__);

	if(wmv->iShareFrameBuf) {
#ifdef IPPGST_CNMWMV_ENABLE_DIRECTMVREFORM
		if(wmv->DecOutInfo.VC1MPDirectModeMVUsed) {
			//When enable reform feature, the VC1MPDirectModeMVUsed reformed frame is a repeat frame. Because VPU has spend some time to decode the frame, do additional memory copy will cost too much time. For those repeat frame, not displaying it isn't a big problem.
			wmv->iDirectMVReformedFrameCnt++;
			return GST_FLOW_OK;
		}
#endif

		IppCMVpuDecFrameAddrInfo* pFrame = rent_buffer_forrepeatframe(wmv);
		if(pFrame == NULL) {
			return GST_FLOW_OK;	//for repeat frame, it repeat previous frame. Therefore, not displaying it isn't a big problem.
		}
		void* VA = pFrame->pUserData1;
		unsigned int PA = ((CnmPContMem*)(pFrame->phyY))->PAddr;

		idx = (unsigned int)(pFrame->pUserData0) & 0xffff;
		outBuf = gst_buffer_new();
		if(outBuf == NULL) {
			pFrame->pUserData0 = (void*)(0 | idx);
			return GST_FLOW_OK;	//for repeat frame, it repeat previous frame. Therefore, not displaying it isn't a big problem.
		}

		//copy the previous frame into skip frame buffer
		//if share frame buffer, buffer in DecFramesInfoArr and skip frame buffers always are non-cached .
		memcpy(pFrame->pUserData1, wmv->DecFramesInfoArr[iDecOutFrameIdx].pUserData1, wmv->iAlignedBufSz);
		

		GST_BUFFER_DATA(outBuf) = VA;
		GST_BUFFER_SIZE(outBuf) = wmv->iAlignedBufSz;
		gst_buffer_set_caps(outBuf, GST_PAD_CAPS(wmv->srcpad));
		IPPGST_BUFFER_CUSTOMDATA(outBuf) = (gpointer) (&wmv->SFBufRepo.sideinfoSFBs[idx]);
		setSideInfo(&wmv->SFBufRepo.sideinfoSFBs[idx], wmv->SeqInfo.FrameROI.x, wmv->SeqInfo.FrameROI.y, wmv->SeqInfo.FrameAlignedWidth, wmv->SeqInfo.FrameAlignedHeight, PA, VA);
		GST_BUFFER_MALLOCDATA(outBuf) = (guint8*)pFrame;
		GST_BUFFER_FREE_FUNC(outBuf) = (GFreeFunc)gcnmdec_downbuf_onfinalize;
		pFrame->pUserData3 = (void*)(&wmv->ShareBufManager);
		FRAMES_WATCHMAN_REF(wmv->ShareBufManager.watcher);

		GST_BUFFER_TIMESTAMP(outBuf) = wmv->TsDuManager.LastOutputTs;
		GST_BUFFER_DURATION(outBuf) = wmv->TsDuManager.LastOutputDu;
		
		g_mutex_lock(wmv->ShareBufManager.mtx);
		wmv->ShareBufManager.iSinkOwnedSFBCnt++;
		g_mutex_unlock(wmv->ShareBufManager.mtx);


		wmv->lastPushedShareBufVaddr = GST_BUFFER_DATA(outBuf);
		
		if(wmv->bNewSegReceivedAfterflush) {
			//GST_DEBUG_OBJECT(wmv, "----gst_pad_push Ts %lld, Du %lld\n", GST_BUFFER_TIMESTAMP(outBuf), GST_BUFFER_DURATION(outBuf));
			FlowRt = gst_pad_push(wmv->srcpad, outBuf);
		}else{
			//If new segment event hasn't received, shouldn't push buffer to sink. Otherwise, sink will throw warnings and work abnormal.
			//Sometimes, user seeking position exceeds the duration of file, then demuxer only fire flush event without newseg event. And demuxer also fire EOS event. At EOS, decoder probably still output some frames, but those frames shouldn't be pushed to sink because no newseg event.
			gst_buffer_unref(outBuf);
			FlowRt = GST_FLOW_OK;
		}
		if(GST_FLOW_OK != FlowRt) {
			if(FlowRt != GST_FLOW_WRONG_STATE) {
				//during seeking, sink element often return GST_FLOW_WRONG_STATUS. No need to echo this message
				GST_WARNING_OBJECT(wmv, "gst_pad_push gstbuffer to downstream plugin fail, ret %d!", FlowRt);
			}
		}
		return FlowRt;
	}

	FlowRt = gcnmdec_require_framebuf_fromdownplugin(&wmv->DownFrameLayout, wmv->srcpad, &outBuf, wmv->iI420LayoutKind==0);
	if(GST_FLOW_OK != FlowRt) {
		if(FlowRt != GST_FLOW_WRONG_STATE) {
			//during seeking, sink element often return GST_FLOW_WRONG_STATUS. No need to echo this message
			GST_WARNING_OBJECT(wmv, "Require gstbuffer from downstream plugin fail, ret %d!", FlowRt);
		}
		return FlowRt;
	}


#ifdef LOG_FRAMECOPYTIME
	gettimeofday(&((CnmWMV3ProbeData*)wmv->pProbeData)->tlog_0, NULL);
#endif
	gcnmdec_copy_to_downframebuf(GST_BUFFER_DATA(outBuf), &wmv->DownFrameLayout, &(wmv->DecFramesInfoArr[iDecOutFrameIdx]), &(wmv->SeqInfo.FrameROI));
#ifdef LOG_FRAMECOPYTIME
	gettimeofday(&((CnmWMV3ProbeData*)wmv->pProbeData)->tlog_1, NULL);
	((CnmWMV3ProbeData*)wmv->pProbeData)->log_copycnt++;
	((CnmWMV3ProbeData*)wmv->pProbeData)->log_copytime += (((CnmWMV3ProbeData*)wmv->pProbeData)->tlog_1.tv_sec - ((CnmWMV3ProbeData*)wmv->pProbeData)->tlog_0.tv_sec)*1000000 + (((CnmWMV3ProbeData*)wmv->pProbeData)->tlog_1.tv_usec - ((CnmWMV3ProbeData*)wmv->pProbeData)->tlog_0.tv_usec);
#endif

	GST_BUFFER_TIMESTAMP(outBuf) = wmv->TsDuManager.LastOutputTs;
	GST_BUFFER_DURATION(outBuf) = wmv->TsDuManager.LastOutputDu;
	gcnm_printfmore("gst_pad_push Ts %lld, Du %lld\n", GST_BUFFER_TIMESTAMP(outBuf), GST_BUFFER_DURATION(outBuf));
	if(wmv->bNewSegReceivedAfterflush) {
		//GST_DEBUG_OBJECT(wmv, "++++ gst_pad_push Ts %lld, Du %lld\n", GST_BUFFER_TIMESTAMP(outBuf), GST_BUFFER_DURATION(outBuf));
		FlowRt = gst_pad_push(wmv->srcpad, outBuf);
	}else{
		//If new segment event hasn't received, shouldn't push buffer to sink. Otherwise, sink will throw warnings and work abnormal.
		//Sometimes, user seeking position exceeds the duration of file, then demuxer only fire flush event without newseg event. And demuxer also fire EOS event. At EOS, decoder probably still output some frames, but those frames shouldn't be pushed to sink because no newseg event.
		gst_buffer_unref(outBuf);
		FlowRt = GST_FLOW_OK;
	}
	if(GST_FLOW_OK != FlowRt) {
		if(FlowRt != GST_FLOW_WRONG_STATE) {
			//during seeking, sink element often return GST_FLOW_WRONG_STATUS. No need to echo this message
			GST_WARNING_OBJECT(wmv, "gst_pad_push gstbuffer to downstream plugin fail, ret %d!", FlowRt);
		}
	}
	return FlowRt;

}

static int _cnmwmvdec_allocateframebufs(void* pElement, IppCMVpuDecParsedSeqInfo* pSeqInfo, int* piFrameCnt, IppCMVpuDecFrameAddrInfo** pFrameArr, int* piFrameStride)
{
	int codecframeNeedCnt;
	Gstcnmwmvdec* ele = (Gstcnmwmvdec*)pElement;
	int ret;
	int bCacheable;
	int framebufsz = (pSeqInfo->FrameAlignedWidth * pSeqInfo->FrameAlignedHeight * 3)>>1;

	*piFrameStride = pSeqInfo->FrameAlignedWidth;
	*pFrameArr = ele->DecFramesInfoArr;

	if(ele->iShareFrameBuf) {
		bCacheable = 0;
		codecframeNeedCnt = gcnmdec_smartdecide_framealloc_cnt(pSeqInfo->MinFrameBufferCount, IPPCMDEC_DECFRAMEBUF_MAX_VC1, IPPGST_CMVPUDEC_FRAMEBUF_DEFAULT_MAXTOTALSIZE_VC1, framebufsz, IPPGSTCNMDEC_SPEC_VC1SPMP);
	}else{
		bCacheable = 0;
		codecframeNeedCnt = pSeqInfo->MinFrameBufferCount;
	}

	ret = prepare_framebufforcodec(pSeqInfo->FrameAlignedHeight, pSeqInfo->FrameAlignedWidth, piFrameCnt, codecframeNeedCnt, &ele->ShareBufManager, ele->DecFramesInfoArr, &ele->iDecFrameBufArrLen, ele->iShareFrameBuf, bCacheable);

	return ret;
}


static int gst_cnmwmvdec_launch_codec(GstBuffer* buf, Gstcnmwmvdec* wmv)
{
	GCNMDec_StartUpDec StartUp;
	int StartUpRet = 0;
	void* pSStmBuf = NULL;
	int frameMeta[2];
	IppCMVpuDecInitPar InitPar;
	CnmPContMem* mem;

	if(wmv->seqMetaLen == 0) {
		GST_ERROR_OBJECT(wmv, "Err: seq meta data isn't ready yet");
		return -1;
	}

	memset(&InitPar, 0, sizeof(InitPar));
	InitPar.ReorderEnable = 1;
	InitPar.FilePlayEnable = 1;
	InitPar.VideoStreamFormat = IPP_VIDEO_STRM_FMT_VC1M;
	InitPar.MultiInstanceSync = wmv->iSupportMultiIns;
	InitPar.DynamicClkEnable = wmv->bEnableDynaClk;

	if(wmv->StmBufPool[0].iBufSize == 0) {
		//prepare static stream buffer
		InitPar.StaticBitstreamBufSize = 384*1024;  //init a value
		if(InitPar.StaticBitstreamBufSize < GST_BUFFER_SIZE(buf) + 8 + wmv->seqMetaLen + IPPCMDEC_STATICSTMBUF_PROTECTSPACE) {
			InitPar.StaticBitstreamBufSize = GST_BUFFER_SIZE(buf) + 8 + wmv->seqMetaLen + IPPCMDEC_STATICSTMBUF_PROTECTSPACE;
		}
		InitPar.StaticBitstreamBufSize = (InitPar.StaticBitstreamBufSize + 1023) & (~1023); //cnm static stream buffer size must 1024 aligned

		gcnm_printf("allocate static stream buffer, want sz %d\n", InitPar.StaticBitstreamBufSize);
		mem = new_cnmStmBuf(InitPar.StaticBitstreamBufSize);
		if(mem == NULL) {
			GST_ERROR_OBJECT(wmv, "Err: allocate static stream buffer fail, want sz %d", InitPar.StaticBitstreamBufSize);
			wmv->iMemErr = -2;
			return -2;
		}
		pSStmBuf = mem->VAddr;
		InitPar.phyAddrStaticBitstreamBuf = mem->PAddr;
		wmv->StmBufPool[0].BufPhyAddr = InitPar.phyAddrStaticBitstreamBuf;
		wmv->StmBufPool[0].BufVirAddr = (unsigned char*)pSStmBuf;
		wmv->StmBufPool[0].iBufSize = InitPar.StaticBitstreamBufSize;
		wmv->StmBufPool[0].mem = mem;
	}else{
		//static stream buffer has been prepared
		gcnm_printf("static stream buffer already prepared, sz %d, phyaddr 0x%x.\n", wmv->StmBufPool[0].iBufSize, (unsigned int)(wmv->StmBufPool[0].BufPhyAddr));
		InitPar.StaticBitstreamBufSize = wmv->StmBufPool[0].iBufSize;
		InitPar.phyAddrStaticBitstreamBuf = wmv->StmBufPool[0].BufPhyAddr;
	}

	wmv->StmBufPool[0].iDataLen = 0;
	//fill seq header and 1st frame
	frameMeta[0] = (GST_BUFFER_SIZE(buf)) | (1<<31);	//consider 1st frame is a key frame, NOTE:!!! target machine must be little endian!!!!!
	frameMeta[1] = 0;
	gcnmdec_fill_stream(&(wmv->StmBufPool[0]), wmv->seqMeta, wmv->seqMetaLen);
	gcnmdec_fill_stream(&(wmv->StmBufPool[0]), frameMeta, 8);
	gcnmdec_fill_stream(&(wmv->StmBufPool[0]), GST_BUFFER_DATA(buf), GST_BUFFER_SIZE(buf));


	//start up codec
	StartUp.StmBufPool = wmv->StmBufPool;
	StartUp.virAddrStaticBitstreamBuf = wmv->StmBufPool[0].BufVirAddr;
	StartUp.pInitPar = &InitPar;
	StartUp.pSeqInfo = &wmv->SeqInfo;
	StartUp.iFilledLen_forCodecStart = wmv->StmBufPool[0].iDataLen;
	StartUp.allocate_framebuf = _cnmwmvdec_allocateframebufs;   //during start up codec, _cnmwmvdec_allocateframebufs() will allocate frame buffer

	//below is output parameter
	//StartUp.pCnmDec = NULL;
	//StartUp.iCodecApiErr = 0;

	wmv->iCodecErr = 0; //clear it before a new codec instance
	gcnm_printf("Call gcnmdec_startup_codec()...\n");
	
	g_mutex_lock(wmv->cnmCodecMtx);
	wmv->bCnmInDream = 0;
	StartUpRet = gcnmdec_startup_codec(wmv, &StartUp, &wmv->ShareBufManager);
	wmv->pWMVDecoderState = StartUp.pCnmDec;
	g_mutex_unlock(wmv->cnmCodecMtx);
	
	wmv->iCodecErr = StartUp.iCodecApiErr;
	wmv->iMemErr = StartUp.iMemoryErr;
	if(G_LIKELY(StartUpRet == 0)) {
		//succeed
		wmv->bCodecSucLaunched = 1;
		gcnm_printf("Launch cnm decoder succeed!\n");
	}else{
		GST_ERROR_OBJECT(wmv, "Launch cnm decoder fail, ret %d", StartUpRet);
	}
	return StartUpRet;

}

static void calc_adjustTsForNewSegGap(Gstcnmwmvdec* wmv, GstBuffer* buf)
{
#define GAP_BETWEEN_1STDATA_AND_NEWSEGSTART		500000000	//0.5 sec
#define ADJUST_FOR_GAP							350000000
	if(wmv->TsDuManager.SegmentStartTs != -1 && GST_BUFFER_TIMESTAMP(buf) != GST_CLOCK_TIME_NONE) {
		if((guint64)GST_BUFFER_TIMESTAMP(buf) + GAP_BETWEEN_1STDATA_AND_NEWSEGSTART < (guint64)wmv->TsDuManager.SegmentStartTs) {
			wmv->adjustTsForNewSegGap = ADJUST_FOR_GAP;
		}
	}
	return;
}

static GstFlowReturn
gst_cnmwmvdec_chain(GstPad *pad, GstBuffer *buf)
{
	gcnm_printfmore("enter %s, in buf sz %d, ts %lld\n", __func__, GST_BUFFER_SIZE(buf), GST_BUFFER_TIMESTAMP(buf));
	int ret, tmp, bCallDecAgain, bStopDecodingLoop;
	GstFlowReturn FlowRt = GST_FLOW_OK;
	Gstcnmwmvdec* wmv = GST_CNMWMVDEC(GST_PAD_PARENT(pad));
	IppCodecStatus codecRt;
	GCNMDec_StmBuf* pCnmInputBuf;
	int cnmDreamStatusAtlock;
	unsigned char* pData = GST_BUFFER_DATA(buf);
	unsigned int DataLen = GST_BUFFER_SIZE(buf);
	unsigned int frameMeta[2];
	int wait_sinkretframe_cnt = 0;
	//GST_DEBUG_OBJECT(wmv, "enter %s, in buf sz %d, ts %lld", __func__, GST_BUFFER_SIZE(buf), GST_BUFFER_TIMESTAMP(buf));

	if(wmv->wmvversion != 3) {
		gst_buffer_unref(buf);
		GST_WARNING_OBJECT(wmv, "Its wmvversion is %d, we do not support it, block it", wmv->wmvversion);
		FlowRt = GST_FLOW_ERROR;
		goto chain_exit;
	}

	if(G_UNLIKELY(wmv->b1stChainAfterNewSeg == 1)) {
		wmv->b1stChainAfterNewSeg = 0;
		wmv->adjustTsForNewSegGap = 0;
		calc_adjustTsForNewSegGap(wmv, buf);
	}

	gcnmdec_insert_TsDu(&wmv->TsDuManager, buf);


	if(wmv->b1stFrame_forFileplay){
		GstCaps *CapTmp;

		wmv->b1stFrame_forFileplay = 0;
		ret = gst_cnmwmvdec_launch_codec(buf, wmv);
		if(ret != 0) {
			gst_buffer_unref(buf);
			gcnm_printf("Err: gst_cnmwmvdec_launch_codec fail, ret %d\n", ret);
			FlowRt = GST_FLOW_ERROR;
			goto chain_exit;
		}
		pCnmInputBuf = &(wmv->StmBufPool[0]);
		GST_DEBUG_OBJECT(wmv, "cnm wmv dec seq info minbcnt %d, delay %d, nextcnt %d", wmv->SeqInfo.MinFrameBufferCount, wmv->SeqInfo.MaxReorderDelay, wmv->SeqInfo.MaxNumOfFrameBufUsedForNextDecoded);

#ifdef IPPGST_CNMWMV_ENABLE_DIRECTMVREFORM
		if(wmv->pWMVDecoderState) {
			IppCodecStatus reformRt = DecoderReformVC1MPDirectMVFrame_CMVpu(wmv->pWMVDecoderState);
			GST_DEBUG_OBJECT(wmv, "DecoderReformVC1MPDirectMVFrame_CMVpu rt %d", reformRt);
			if(reformRt != IPP_STATUS_NOERR) {
				gst_buffer_unref(buf);
				FlowRt = GST_FLOW_ERROR;
				goto chain_exit;
			}
		}
#endif

		wmv->DownFrameLayout.iDownFrameWidth = wmv->SeqInfo.FrameROI.width;
		wmv->DownFrameLayout.iDownFrameHeight = wmv->SeqInfo.FrameROI.height;
		if(wmv->iI420LayoutKind != 2) {
			gcnmdec_update_downbuf_layout(wmv->DownFrameLayout.iDownFrameWidth, wmv->DownFrameLayout.iDownFrameHeight, 0, &wmv->DownFrameLayout);
		}else{
			gcnmdec_update_downbuf_layout(wmv->DownFrameLayout.iDownFrameWidth, wmv->DownFrameLayout.iDownFrameHeight, 1, &wmv->DownFrameLayout);
		}

		wmv->iAlignedBufSz = (wmv->SeqInfo.FrameAlignedWidth * wmv->SeqInfo.FrameAlignedHeight * 3)>>1;


		gcnm_printf("Will set srcpad cap, fps %d/%d, w %d, h %d\n", wmv->num, wmv->den, wmv->DownFrameLayout.iDownFrameWidth, wmv->DownFrameLayout.iDownFrameHeight);
		CapTmp = gst_caps_new_simple ("video/x-raw-yuv",
				"format", GST_TYPE_FOURCC, GST_STR_FOURCC ("I420"),
				"width", G_TYPE_INT, wmv->DownFrameLayout.iDownFrameWidth,
				"height", G_TYPE_INT, wmv->DownFrameLayout.iDownFrameHeight,
				"framerate", GST_TYPE_FRACTION, wmv->num, wmv->den,
				NULL);
		gst_pad_set_caps(wmv->srcpad, CapTmp);
		gst_caps_unref(CapTmp);
	}else{
		//load frame data
		tmp = gcnmdec_select_stream(wmv->StmBufPool, GSTCNMDEC_WMV3_STMPOOLSZ, DataLen + 8);
		if( tmp < 0 || tmp >= GSTCNMDEC_WMV3_STMPOOLSZ ) {
			wmv->iMemErr = -3;
			gcnm_printf("Err: gcnmdec_select_stream fail, ret %d", tmp);
			GST_ERROR_OBJECT(wmv, "Err: gcnmdec_select_stream fail, ret %d", tmp);
			gst_buffer_unref(buf);
			FlowRt = GST_FLOW_ERROR;
			goto chain_exit;
		}
		pCnmInputBuf = &(wmv->StmBufPool[tmp]);
		frameMeta[0] = DataLen; //NOTE:!!! target machine must be little-endian!!!!!
		frameMeta[1] = 0;
		gcnmdec_fill_stream(pCnmInputBuf, frameMeta, 8);
		gcnmdec_fill_stream(pCnmInputBuf, pData, DataLen);
	}

	gst_buffer_unref(buf);

	if(!wmv->bCodecSucLaunched) {
		FlowRt = GST_FLOW_ERROR;
		goto chain_exit;
	}

	wmv->FilePlay_StmBuf.phyAddrBitstreamBuf = pCnmInputBuf->BufPhyAddr;
	wmv->FilePlay_StmBuf.DataLen = pCnmInputBuf->iDataLen;

	bStopDecodingLoop = 0;

	do{
		bCallDecAgain = 0;

		g_mutex_lock(wmv->cnmCodecMtx);
		cnmDreamStatusAtlock = wmv->bCnmInDream;
		if(cnmDreamStatusAtlock) {
			HibernateWakeupCNM(wmv->pWMVDecoderState, 0, &wmv->bCnmInDream);
		}

		if(0 != gcnmdec_informcodec_decframeconsumed(wmv->pWMVDecoderState, &wmv->ShareBufManager)) {
			wmv->iCodecErr |= ERR_CNMDEC_DISPLAYEDAPI;
			g_mutex_unlock(wmv->cnmCodecMtx);
			FlowRt = GST_FLOW_ERROR;
			goto chain_exit;
		}

#ifdef DEBUG_PERFORMANCE
		gettimeofday(&((CnmWMV3ProbeData*)wmv->pProbeData)->tstart, &((CnmWMV3ProbeData*)wmv->pProbeData)->tz);
#endif
		codecRt = DecodeFrame_CMVpu(wmv->pWMVDecoderState, &wmv->FilePlay_StmBuf, &wmv->DecOutInfo, 0);
#ifdef DEBUG_PERFORMANCE
		gettimeofday(&((CnmWMV3ProbeData*)wmv->pProbeData)->tend, &((CnmWMV3ProbeData*)wmv->pProbeData)->tz);
		wmv->codec_time += (((CnmWMV3ProbeData*)wmv->pProbeData)->tend.tv_sec - ((CnmWMV3ProbeData*)wmv->pProbeData)->tstart.tv_sec)*1000000 + (((CnmWMV3ProbeData*)wmv->pProbeData)->tend.tv_usec - ((CnmWMV3ProbeData*)wmv->pProbeData)->tstart.tv_usec);
#endif
		//GST_DEBUG_OBJECT(wmv, "DecodeFrame_CMVpu rt %d, nextbuf[0] %d", codecRt, wmv->DecOutInfo.idxFrameBufUsedForNextDecoded[0]);
		if((! wmv->iShareFrameBuf && (codecRt == IPP_STATUS_OUTPUT_DATA || codecRt == IPP_STATUS_FRAME_COMPLETE)) || codecRt == IPP_STATUS_FRAME_UNDERRUN) {
			(wmv->ShareBufManager.DecOutFrameConsumeStatus[wmv->DecOutInfo.idxFrameOutput])++;//In theory, under non-iShareFrameBuf mode, should after copy the frame data, then set the status that the frame is consumed. Anyway, setting it in advance is no side effect.
		}
		if(cnmDreamStatusAtlock) {
			HibernateWakeupCNM(wmv->pWMVDecoderState, 1, &wmv->bCnmInDream);
		}
		g_mutex_unlock(wmv->cnmCodecMtx);

		if(!(codecRt == IPP_STATUS_OUTPUT_DATA || (codecRt==IPP_STATUS_BUFFER_FULL && wmv->iShareFrameBuf))) {
			//the data has been consumed
			pCnmInputBuf->iDataLen = 0;
		}

		switch(codecRt) {
		case IPP_STATUS_OUTPUT_DATA:
			//No frame is decoded, because the input frame data hasn't been consumed by VPU, therefore, application should feed this frame again.
			//But some frame still need output, which are reorder delayed frames.
			bCallDecAgain = 1;
		case IPP_STATUS_FRAME_COMPLETE:
			gcnm_printfmore("DecodeFrame_CMVpu ret IPP_STATUS_OUTPUT_DATA or IPP_STATUS_FRAME_COMPLETE, %d, %d, %d\n", wmv->totalFrames, wmv->DecOutInfo.idxFrameOutput, wmv->DecOutInfo.NumOfErrMbs);
			wmv->totalFrames++;
			_cnmwmv_refresh_downpush_TsDu(wmv);
			FlowRt = _cnmwmv_push_data(wmv, wmv->DecOutInfo.idxFrameOutput);
			break;
		case IPP_STATUS_FRAME_UNDERRUN:
			//meet vc1 spmp skip or DirectModeMV reformed frame, for those repeat frame, cnm decoder directly output the reference frame idx instead of decode the frame into a new buffer, therefore, we should copy the frame content from the reference frame buffer
			gcnm_printfmore("DecodeFrame_CMVpu ret IPP_STATUS_FRAME_UNDERRUN, %d, %d, %d\n", wmv->totalFrames, wmv->DecOutInfo.idxFrameOutput, wmv->DecOutInfo.NumOfErrMbs);
			wmv->totalFrames++;
			_cnmwmv_refresh_downpush_TsDu(wmv);
			FlowRt = _cnmwmv_consume_repeatframe(wmv, wmv->DecOutInfo.idxFrameOutput);
			break;
		case IPP_STATUS_NOERR:
			gcnm_printf("DecodeFrame_CMVpu ret NOERR, %d %d %d\n", wmv->DecOutInfo.OneFrameUnbrokenDecoded, wmv->DecOutInfo.idxFrameOutput, wmv->DecOutInfo.idxFrameDecoded);
			break;
		case IPP_STATUS_NOTSUPPORTED_ERR:
			wmv->iCodecErr |= WARN_CNMDEC_FRAMENOTSUPPORT;
			_cnmwmv_refresh_downpush_TsDu(wmv);
			gcnm_printf("DecodeFrame_CMVpu ret IPP_STATUS_NOTSUPPORTED_ERR.\n");
			break;
		case IPP_STATUS_FRAME_ERR:
			wmv->iCodecErr |= WARN_CNMDEC_FRAMECORRUPT;
			gcnm_printf("DecodeFrame_CMVpu ret WARN_CNMDEC_FRAMECORRUPT, probably the frame is corrupt.\n");
			break;
		case IPP_STATUS_BUFFER_FULL:
#ifndef NOTECHO_BUFFERFULL_WARNING
			GST_WARNING_OBJECT(wmv, "Warning: DecodeFrame_CMVpu ret buffer full, waiting sink release buffer\n");
#endif
			if(wmv->iShareFrameBuf) {
				int waitRt;
				bCallDecAgain = 1;
				//GST_DEBUG_OBJECT(wmv, "will call wait_sink_retDecFrameBuf, sink own buf %d", wmv->ShareBufManager.iSinkOwnedDecFrameBufCnt);
				wait_sinkretframe_cnt++;
#define WAIT_SINK_RETBUF_RETRY_CNT		4
				if(wait_sinkretframe_cnt < WAIT_SINK_RETBUF_RETRY_CNT) {
					waitRt = wait_sink_retDecFrameBuf(wmv->iTimeoutForDisp1Frame, 1, &wmv->ShareBufManager, &wmv->ElementState);
				}else{
					//if we have tried waiting many times, reduce timeout threshold and do waiting last time
					waitRt = wait_sink_retDecFrameBuf(wmv->iTimeoutForDisp1Frame>>1, -1, &wmv->ShareBufManager, &wmv->ElementState);
				}
				if(waitRt > 0) {
					bStopDecodingLoop = 1;	//element isn't running, give up decoding
					//give up this frame, since we have limitted the length of TS queue, little need to remove the TS of this frame from TS queue
					pCnmInputBuf->iDataLen = 0;
				}else if(waitRt < 0) {
					//wait time out or something abnormal occur
					if(waitRt == -2) {
						FlowRt = GST_FLOW_ERROR;
						wmv->iCodecErr |= ERR_WAITSINKRETFRAME_SINKNOFRAME;
						goto chain_exit;
					}else if(waitRt == -3) {
						wmv->iCodecErr |= WARN_WAITSINKRETFRAME_PUSHDUMMY_AVOID_DEADLOCK;
						GST_DEBUG_OBJECT(wmv, "cnmdec_wmv3 push 1 dummy frame to prevent dead lock");
						push_dummy_1frame_withTS(wmv, -1);
					}else{
						//return is -1, means time out, terminate decoding
						FlowRt = GST_FLOW_ERROR;
						wmv->iCodecErr |= ERR_WAITSINKRETFRAME_TIMEOUT;
						GST_DEBUG_OBJECT(wmv, "wait sink ret buf timeout, sink owned buffer cnt %d", wmv->ShareBufManager.iSinkOwnedDecFrameBufCnt);
						goto chain_exit;
					}
				}
				break;  //break switch
			}
		default:
			wmv->iCodecErr |= ERR_CNMDEC_DECFRAME_SERIOUSERR;
			gcnm_printf("Err: DecodeFrame_CMVpu fail, ret %d\n", codecRt);
			GST_ERROR_OBJECT(wmv, "Err: DecodeFrame_CMVpu fail, ret %d", codecRt);
			FlowRt = GST_FLOW_ERROR;
			goto chain_exit;
		}
	}while(bCallDecAgain == 1 && bStopDecodingLoop == 0);

chain_exit:
	if(FlowRt < GST_FLOW_UNEXPECTED) {
		GST_ELEMENT_ERROR(wmv, STREAM, DECODE, (NULL), ("%s() flow err %d, codec err 0x%x, mem err %d", __FUNCTION__, FlowRt, wmv->iCodecErr, wmv->iMemErr));
	}

	return FlowRt;
}

static void
gst_cnmwmvdec_clean_streambuf(Gstcnmwmvdec *wmv)
{
	gcnm_printf("free stream buffer\n");
	int i;
	for(i=0;i<GSTCNMDEC_WMV3_STMPOOLSZ;i++) {
		if(wmv->StmBufPool[i].iBufSize != 0) {
			delete_cnmStmBuf(wmv->StmBufPool[i].mem);
			wmv->StmBufPool[i].iBufSize = 0;
			wmv->StmBufPool[i].BufPhyAddr = 0;
			wmv->StmBufPool[i].BufVirAddr = NULL;
			wmv->StmBufPool[i].iDataLen = 0;
			wmv->StmBufPool[i].mem = NULL;
		}
	}
	return;
}


static gboolean
gst_cnmwmvdec_null2ready(Gstcnmwmvdec *wmv)
{
	gcnm_printf("enter %s\n", __func__);

	wmv->pWMVDecoderState = NULL;
	wmv->totalFrames = 0;
	wmv->codec_time = 0;

	wmv->ShareBufManager.watcher = gcnmdec_watcher_create(wmv, NULL, &wmv->ShareBufManager); //create watcher
	if(wmv->ShareBufManager.watcher == NULL) {
		GST_WARNING_OBJECT(wmv, "Err: gcnmdec_watcher_create() fail");
		return FALSE;
	}

	return TRUE;
}


static gboolean
gst_cnmwmvdec_ready2null(Gstcnmwmvdec *wmv)
{
	gcnm_printf("enter %s\n", __func__);

	//close decoder
	g_mutex_lock(wmv->cnmCodecMtx);
	if(wmv->pWMVDecoderState) {
		if(wmv->bCnmInDream) {
			HibernateWakeupCNM(wmv->pWMVDecoderState, 0, &wmv->bCnmInDream);
		}
		wmv->iCodecErr |= gcnmdec_close_codec(&wmv->pWMVDecoderState, &wmv->ShareBufManager);
	}
	g_mutex_unlock(wmv->cnmCodecMtx);

	gcnmdec_remove_redundant_TsDu(&wmv->TsDuManager, 0);

	//free buffers
	gst_cnmwmvdec_clean_streambuf(wmv);

	if(! wmv->iShareFrameBuf) {
		gstcnmElement_clean_nonShareFramebuf(wmv->DecFramesInfoArr, &wmv->iDecFrameBufArrLen);
	}else{
		g_mutex_lock(wmv->ShareBufManager.mtx);
		gstcnmElement_clean_idleShareFBs(&wmv->ShareBufManager, &wmv->SFBufRepo);	//for those share frame buffer which is still owned by sink, will free them on their gstbuffer finalizing callback function
		wmv->ShareBufManager.NotNeedFMemAnyMore = 1;
		g_mutex_unlock(wmv->ShareBufManager.mtx);
	}


	if(wmv->ShareBufManager.watcher) {
		FRAMES_WATCHMAN_UNREF(wmv->ShareBufManager.watcher);	//smart release watcher, free frame buffers
	}

#ifdef DEBUG_PERFORMANCE
	printf("codec totally outputed %d frames, spend %lld us\n", wmv->totalFrames, wmv->codec_time);
#else
	gcnm_printf("codec totally outputed %d frames\n", wmv->totalFrames);
#endif

#ifdef LOG_FRAMECOPYTIME
	printf("copy %d frame total cost %d us\n", ((CnmWMV3ProbeData*)wmv->pProbeData)->log_copycnt, ((CnmWMV3ProbeData*)wmv->pProbeData)->log_copytime);
#endif

	gcnm_printf("-----------At ready2null, c&m codec error %d, memory error %d\n", wmv->iCodecErr, wmv->iMemErr);
	GST_DEBUG_OBJECT(wmv, "cnmdec_wmv3 instance(%p) ready2null: codec err 0x%x, mem err %d, DMV cnt %d, total frame %d.", wmv, wmv->iCodecErr, wmv->iMemErr, wmv->iDirectMVReformedFrameCnt, wmv->totalFrames);
	return TRUE;
}

static int gst_cnmwmvdec_ready2paused(Gstcnmwmvdec *wmv)
{
	gcnm_printf("gst_cnmwmvdec_ready2paused() is called\n");
	//prepare for new stream

	return 0;
}

static int gst_cnmwmvdec_paused2ready(Gstcnmwmvdec* wmv)
{
	gcnm_printf("gst_cnmwmvdec_paused2ready() is called\n");
	gcnm_printf("-----------At paused2ready, cnm decoder error status %d\n", wmv->iCodecErr);
	return 0;
}

static __inline int paused2play(Gstcnmwmvdec* wmv)
{
	IppCodecStatus ret = IPP_STATUS_NOERR;
	g_mutex_lock(wmv->cnmCodecMtx);
	if(wmv->pWMVDecoderState && wmv->bEnableHibernateAtPause && wmv->bCnmInDream) {
		ret = HibernateWakeupCNM(wmv->pWMVDecoderState, 0, &wmv->bCnmInDream);
	}
	g_mutex_unlock(wmv->cnmCodecMtx);
	if(IPP_STATUS_NOERR != ret) {
		GST_WARNING_OBJECT(wmv, "Wakeup cnm fail at pause2paly, ret %d", ret);
		return -1;
	}
	return 0;
}

static __inline int play2paused(Gstcnmwmvdec* wmv)
{
	IppCodecStatus ret = IPP_STATUS_NOERR;
	g_mutex_lock(wmv->cnmCodecMtx);
	if(wmv->pWMVDecoderState && wmv->bEnableHibernateAtPause && ! wmv->bCnmInDream) {
		ret = HibernateWakeupCNM(wmv->pWMVDecoderState, 1, &wmv->bCnmInDream);
	}
	g_mutex_unlock(wmv->cnmCodecMtx);
	if(IPP_STATUS_NOERR != ret) {
		GST_WARNING_OBJECT(wmv, "Hibernate cnm fail at pause2paly, ret %d", ret);
		return -1;
	}
	return 0;
}

static GstStateChangeReturn
gst_cnmwmvdec_change_state(GstElement *element, GstStateChange transition)
{
	gcnm_printf("enter %s, change from %d to %d\n", __func__, transition>>3, transition&7);

	GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;
	Gstcnmwmvdec *wmv = GST_CNMWMVDEC(element);

	switch (transition)
	{
	case GST_STATE_CHANGE_NULL_TO_READY:
		if(!gst_cnmwmvdec_null2ready(wmv)){
			goto statechange_failed;
		}
		break;
	case GST_STATE_CHANGE_READY_TO_PAUSED:
		gst_cnmwmvdec_ready2paused(wmv);
		break;
	case GST_STATE_CHANGE_PAUSED_TO_PLAYING:
		if(0 != paused2play(wmv)) {
			goto statechange_failed;
		}
		break;

	case GST_STATE_CHANGE_PAUSED_TO_READY:
		set_ElementState(&wmv->ElementState, CNMELESTATE_RESTING);
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
		if(0 != play2paused(wmv)) {
			goto statechange_failed;
		}
		break;
	case GST_STATE_CHANGE_PAUSED_TO_READY:
		gst_cnmwmvdec_paused2ready(wmv);
		break;
	case GST_STATE_CHANGE_READY_TO_NULL:
		if(!gst_cnmwmvdec_ready2null(wmv)){
			goto statechange_failed;
		}
		break;

	case GST_STATE_CHANGE_READY_TO_PAUSED:
		set_ElementState(&wmv->ElementState, CNMELESTATE_RUNNING);
		break;
	default:
		break;
	}

	return ret;

	 /* ERRORS */
statechange_failed:
	{
		/* subclass must post a meaningfull error message */
		GST_ERROR_OBJECT (wmv, "state change failed");
		return GST_STATE_CHANGE_FAILURE;
	}
}

static void
gst_cnmwmvdec_base_init (gpointer klass)
{
	gcnm_printf("enter %s\n", __func__);

	static GstElementDetails wmvdec_details = {
		"IPPCNM decoder WMV3",
		"Codec/Decoder/Video",
		"HARDWARE WMV3 Decoder based-on IPP CNM CODEC",
		""
	};

	GstElementClass *element_class = GST_ELEMENT_CLASS (klass);

	gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&src_factory));
	gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&sink_factory));
	gst_element_class_set_details (element_class, &wmvdec_details);
}

static void
gst_cnmwmvdec_finalize(GObject * object)
{
	Gstcnmwmvdec* wmv = GST_CNMWMVDEC(object);
	if(wmv->pProbeData) {
		g_free(wmv->pProbeData);
	}

	g_mutex_free(wmv->cnmCodecMtx);

	g_mutex_free(wmv->ElementState.elestate_mtx);

	GST_DEBUG_OBJECT(wmv, "Gstcnmwmvdec instance(%p) parent class is finalizing, codec err 0x%x, mem err %d, DMV cnt %d, total frame %d.", wmv, wmv->iCodecErr, wmv->iMemErr, wmv->iDirectMVReformedFrameCnt, wmv->totalFrames);
	G_OBJECT_CLASS(parent_class)->finalize(object);

	gcnm_printf("Gstcnmwmvdec instance 0x%x is finalized!!!\n", (unsigned int)object);
	printf("Gstcnmwmvdec instance (0x%x) is finalized\n", (unsigned int)object);

	return;
}


static void
gst_cnmwmvdec_class_init (GstcnmwmvdecClass *klass)
{
	gcnm_printf("enter %s\n", __func__);

	GObjectClass *gobject_class  = (GObjectClass*) klass;
	GstElementClass *gstelement_class = (GstElementClass*) klass;

	gobject_class->set_property = gst_cnmwmvdec_set_property;
	gobject_class->get_property = gst_cnmwmvdec_get_property;

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
		g_param_spec_int ("totalframes", "Totalframe of cnm wmv3 decoder outputted",
			"Number of frame of cnm wmv3 decoder outputted", 0, G_MAXINT, 0, G_PARAM_READABLE));
	g_object_class_install_property (gobject_class, ARG_CODECTIME,
		g_param_spec_int64 ("codectime", "cnm wmv3 decode time",
			"Pure codec time of calling cnm wmv3 decoding API(microsecond)", 0, G_MAXINT64, 0, G_PARAM_READABLE));
#endif

	gobject_class->finalize = GST_DEBUG_FUNCPTR (gst_cnmwmvdec_finalize);
	gstelement_class->change_state = GST_DEBUG_FUNCPTR (gst_cnmwmvdec_change_state);
	GST_DEBUG_CATEGORY_INIT(cnmdec_wmv3_debug, "cnmdec_wmv3", 0, "CNM WMV3 Decoder Element");
	return;
}

static void cnmwmv3_echo(void* ele, const char* fmtstr, int par0, int par1)
{
	GST_DEBUG_OBJECT(ele, fmtstr, par0, par1);
	return;
}


static void gst_cnmwmvdec_init_members(Gstcnmwmvdec* wmv)
{
	gcnm_printf("gst_cnmwmvdec_init_members is called\n");
	wmv->pProbeData = g_malloc0(sizeof(CnmWMV3ProbeData));

	wmv->pWMVDecoderState = NULL;
	wmv->num = 0;
	wmv->den = 1;
	wmv->bCodecSucLaunched = 0;
	wmv->b1stFrame_forFileplay = 1;
	wmv->iCodecErr = 0;
	wmv->iMemErr = 0;
	memset(&wmv->SeqInfo, 0, sizeof(wmv->SeqInfo));
	memset(wmv->DecFramesInfoArr, 0, sizeof(wmv->DecFramesInfoArr));
	wmv->iDecFrameBufArrLen = 0;
	memset(&wmv->FilePlay_StmBuf, 0, sizeof(wmv->FilePlay_StmBuf));
	memset(&wmv->DecOutInfo, 0, sizeof(wmv->DecOutInfo));
	memset(&wmv->ShareBufManager, 0, sizeof(wmv->ShareBufManager));
	wmv->ShareBufManager.wmvShareSFBRepo = &wmv->SFBufRepo;
	//just for debug, in release, following 2 members should be 0
	//wmv->ShareBufManager.ele = (void*)wmv;
	//wmv->ShareBufManager.pfun_echo = cnmwmv3_echo;

	wmv->ElementState.state = CNMELESTATE_RESTING;
	wmv->ElementState.elestate_mtx = g_mutex_new();
	
	memset(&wmv->SFBufRepo, 0, sizeof(wmv->SFBufRepo));

	memset(wmv->seqMeta, 0, sizeof(wmv->seqMeta));
	wmv->seqMetaLen = 0;
	memset(wmv->StmBufPool, 0, sizeof(wmv->StmBufPool));
	memset(&wmv->DownFrameLayout, 0, sizeof(wmv->DownFrameLayout));
	gcnmdec_init_TsDuMgr(&wmv->TsDuManager);

	wmv->iI420LayoutKind = 0;
	wmv->iShareFrameBuf = 1;
	wmv->iTimeoutForDisp1Frame = WAIT_DOWNSTREAM_RETBUF_TIMEOUT;
	wmv->iSupportMultiIns = 1;
	wmv->bEnableHibernateAtPause = 1;
	wmv->bEnableDynaClk = 1;

	wmv->bNewSegReceivedAfterflush = 0;

	wmv->cnmCodecMtx = g_mutex_new();
	wmv->bCnmInDream = 0;

	wmv->totalFrames = 0;
	wmv->codec_time = 0;
	wmv->iDirectMVReformedFrameCnt = 0;

	wmv->wmvversion = 0;
	wmv->lastPushedShareBufVaddr = NULL;
	wmv->adjustTsForNewSegGap = 0;
	wmv->b1stChainAfterNewSeg = 0;
	return;
}

static void
gst_cnmwmvdec_init(Gstcnmwmvdec *wmv, GstcnmwmvdecClass *wmv_klass)
{
	gcnm_printf("enter %s\n", __func__);

	GstElementClass *klass = GST_ELEMENT_CLASS(wmv_klass);

	wmv->sinkpad = gst_pad_new_from_template (
		gst_element_class_get_pad_template(klass, "sink"), "sink");

	gst_pad_set_setcaps_function (wmv->sinkpad, GST_DEBUG_FUNCPTR (gst_cnmwmvdec_sinkpad_setcaps));
	gst_pad_set_chain_function (wmv->sinkpad, GST_DEBUG_FUNCPTR (gst_cnmwmvdec_chain));
	gst_pad_set_event_function (wmv->sinkpad, GST_DEBUG_FUNCPTR (gst_cnmwmvdec_sinkpad_event));

	gst_element_add_pad(GST_ELEMENT(wmv), wmv->sinkpad);

	wmv->srcpad = gst_pad_new_from_template (
		gst_element_class_get_pad_template(klass, "src"), "src");

	gst_element_add_pad(GST_ELEMENT(wmv), wmv->srcpad);

	gst_cnmwmvdec_init_members(wmv);

	gcnm_printf("Gstcnmwmvdec instance 0x%x is inited!!!\n", (unsigned int)wmv);
	printf("Gstcnmwmvdec instance (0x%x) is inited\n", (unsigned int)wmv);

	return;
}

/* EOF */
