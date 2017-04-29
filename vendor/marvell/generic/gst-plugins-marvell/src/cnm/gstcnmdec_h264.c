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

/* gstcnmdec_h264.c */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h> //to include memset(),...
#include <sys/time.h>	//to include time functions and structures

#include "gstcnmdec_h264.h"
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
}CnmH264ProbeData;


static const unsigned char H264NALSyncCode[4] = {0,0,0,1};

GST_DEBUG_CATEGORY_STATIC (cnmdec_h264_debug);
#define GST_CAT_DEFAULT cnmdec_h264_debug

enum {

	/* fill above with user defined signals */
	LAST_SIGNAL
};

enum {
	ARG_0,
	/* fill below with user defined arguments */
	ARG_NOTREORDERBASELINE,
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

//CNM support height < 1024 and should be even, therefore 1022
static GstStaticPadTemplate sink_factory =
	GST_STATIC_PAD_TEMPLATE ("sink", GST_PAD_SINK, GST_PAD_ALWAYS,
	GST_STATIC_CAPS ("video/x-h264, "
					 "width = (int) [ 16, 1280 ], "
					 "height = (int) [ 16, 1022 ], "
					 "cnmnotsupport = (int) 0, "
					 "framerate = (fraction) [ 0, MAX ]")
					 );

static GstStaticPadTemplate src_factory =
	GST_STATIC_PAD_TEMPLATE ("src", GST_PAD_SRC, GST_PAD_ALWAYS,
	GST_STATIC_CAPS ("video/x-raw-yuv,"
					 "format = (fourcc) { I420 }, "
					 "width = (int) [ 16, 1280 ], "
					 "height = (int) [ 16, 1022 ], "
					 "framerate = (fraction) [ 0, MAX ]")
					 );

GST_BOILERPLATE (Gstcnmh264dec, gst_cnmh264dec, GstElement, GST_TYPE_ELEMENT);


static gboolean
gst_cnmh264dec_sinkpad_setcaps (GstPad *pad, GstCaps *caps)
{
	gcnm_printf("enter gst_cnmh264dec_sinkpad_setcaps().\n");

	Gstcnmh264dec *h264 = GST_CNMH264DEC (GST_OBJECT_PARENT (pad));
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

	if(G_UNLIKELY(FALSE == gst_structure_get_fraction(str, "framerate", &h264->num, &h264->den))) {
		h264->num = 0;  	//if this information isn't in caps, we assume it is 0
		h264->den = 1;
	}

	gcnm_printf("fps: %d/%d\n", h264->num, h264->den);

	if(h264->num != 0) {
		h264->TsDuManager.FrameFixPeriod = gst_util_uint64_scale_int(GST_SECOND, h264->den, h264->num);
	}

	gcnm_printf("frame duration %lld ns\n",  h264->TsDuManager.FrameFixPeriod);


	h264->is_avcC = FALSE;  //set the default value of is_avcC
	if ((value = gst_structure_get_value (str, "codec_data"))) {
		guint8 *cdata;
		guint csize;
		GstBuffer *buf;

		gcnm_printf("have codec_data\n");

		/* h264 codec data is avcC data, contains sps and pps, start from offset =5 */
		buf = gst_value_get_buffer(value);
		cdata = GST_BUFFER_DATA (buf);
		csize = GST_BUFFER_SIZE (buf);

		gcnm_printf("codec data size %d\n", csize);

		if(h264->seqMeta== NULL) {
			h264->seqMeta = g_malloc(csize);
			if(h264->seqMeta == NULL) {
				gcnm_printf("Malloc seqMeta(,%d,) fail.\n", csize);
				return FALSE;
			}else{
				h264->seqMetaBufSz = csize;
				gcnm_printf("Create sequence internal stream buffer to hold sps and pps, 0x%x, size %d\n", (unsigned int)h264->seqMeta, csize);
			}
		}else if((unsigned int)h264->seqMetaBufSz < csize) {
			h264->seqMeta = g_realloc(h264->seqMeta, csize);
			if(h264->seqMeta == NULL) {
				gcnm_printf("Malloc seqMeta(,%d,) fail.\n", csize);
				return FALSE;
			}else{
				h264->seqMetaBufSz = csize;
				gcnm_printf("Re-Create sequence internal stream buffer 0x%x, size %d\n", (unsigned int)h264->seqMeta, csize);
			}
		}

		 //if have codec_data, we assume it contains one SPS and one PPS at least
		if(csize < 15){ //each sync code 3 bytes, SPS NAL>= 6 bytes, PPS NAL>= 3 bytes
			g_warning("codec_data length is %d, not have enough data in code_data(at least SPS+PPS) for h264.", csize);
			gcnm_printf("codec_data length is %d, not have enough data in code_data(at least SPS+PPS) for h264.\n", csize);
			//return FALSE;
			if(csize > 0) {
				memcpy(h264->seqMeta, cdata, csize);
				h264->seqMetaLen = csize;
			}
			return TRUE;	//still store the data, and will fill to codec, let codec to decide whether this data could be accept.
		}

		//ISO 14496-15: sec 5.2.4.1, sec 5.3.4.1.2
		if(csize >= 20 && cdata && 1 == cdata[0]){
			unsigned int AVCProfileIndication = cdata[1];
			int offset = 4;
			int cnt_sps;
			int cnt_pps;
			unsigned int len;
			int lengthSizeMinusOne;
			int i;

			gcnm_printf("is avcC stream, parsing avcC\n");

			h264->is_avcC= TRUE;
			h264->seqMetaLen = 0;

			if(AVCProfileIndication != 66) {
				gcnm_printf("AVCProfileIndication in avcC is %d, not 66(Baseline Profile)\n", AVCProfileIndication);
				g_warning("AVCProfileIndication in avcC is %d, not 66(Baseline Profile)\n", AVCProfileIndication);
				GST_ERROR_OBJECT(h264, "AVCProfileIndication in avcC is %d, not 66(Baseline Profile)\n", AVCProfileIndication);
				return FALSE;
			}

			/*parse sps from avcC*/
			cdata += offset;					/* start from 4 */
			lengthSizeMinusOne = (*(cdata++) & 0x03);
			if(lengthSizeMinusOne == 2) {
				gcnm_printf("lengthSizeMinusOne in avcC shouldn't equal to 2 according to ISO 14496-15: sec 5.2.4.1.2");
				//return FALSE;
				lengthSizeMinusOne = 3;
			}
			h264->nalLenFieldSize = lengthSizeMinusOne+1;

			gcnm_printf("nalLenFieldSize %d\n", h264->nalLenFieldSize);

			cnt_sps = *(cdata++) & 0x1f;		/* sps cnt */
			/* some stream may be parsed with cnt_sps = 0, we force it as 1 to init codec */
			cnt_sps = cnt_sps > 0 ? cnt_sps : 1;
			for (i = 0; i < cnt_sps; i++) {
				len = *(cdata++)<<8;
				len |= *(cdata++);  	/* one sps len 2 bytes*/

				/* fill sps to seqMeta stream buffer */
				if(4 + len > (unsigned int)(h264->seqMetaBufSz - h264->seqMetaLen)) {
					h264->seqMeta = g_realloc(h264->seqMeta, h264->seqMetaBufSz + 4 + len);
					if(h264->seqMeta == NULL) {
						h264->seqMetaBufSz = 0;
						gcnm_printf("allocate a big seqMeta buffer fail.\n");
						return FALSE;
					}
					h264->seqMetaBufSz = h264->seqMetaBufSz + 4 + len;
				}
				memcpy(h264->seqMeta + h264->seqMetaLen, H264NALSyncCode, 4);
				h264->seqMetaLen += 4;
				memcpy(h264->seqMeta + h264->seqMetaLen, cdata, len);
				cdata += len;
				h264->seqMetaLen += len;
				gcnm_printf("sps[%d], size %d\n", i, len);
			}

			/*parse pps from avcC*/
			cnt_pps= *(cdata++);				/* pps cnt*/
			/* some stream may be parsed with cnt_pps = 0, we force it as 1 to init codec */
			cnt_pps = cnt_pps > 0 ? cnt_pps : 1;
			for (i = 0; i < cnt_pps; i++) {
				len = *(cdata++)<<8;
				len |= *(cdata++);  		/*one pps len */
				/* fill pps to seqMeta stream buffer */
				if(4 + len > (unsigned int)(h264->seqMetaBufSz - h264->seqMetaLen)) {
					h264->seqMeta = g_realloc(h264->seqMeta, h264->seqMetaBufSz + 4 + len);
					if(h264->seqMeta == NULL) {
						h264->seqMetaBufSz = 0;
						gcnm_printf("allocate a big seqMeta buffer fail.\n");
						return FALSE;
					}
					h264->seqMetaBufSz = h264->seqMetaBufSz + 4 + len;
				}
				memcpy(h264->seqMeta + h264->seqMetaLen, H264NALSyncCode, 4);
				h264->seqMetaLen += 4;
				memcpy(h264->seqMeta + h264->seqMetaLen, cdata, len);
				cdata += len;
				h264->seqMetaLen += len;
				gcnm_printf("pps[%d], size %d\n", i, len);
			}

		}else{
			//usually, for h264 in avi file, it's layout isn't avcC but h.264 byte stream format(h.264 spec. annex B) picture by picture
			gcnm_printf("not avcC stream\n");
			if(cdata && csize > 0) {
				memcpy(h264->seqMeta, cdata, csize);
			}
			h264->seqMetaLen = csize;
		}
	}else{
		GST_INFO_OBJECT(h264, "No codec_data provided by demuxer, suppose the stream is h.264 byte stream format, and sps+pps are combined into element stream");
	}

	gcnm_printf("exit gst_cnmh264dec_sinkpad_setcaps() succeed\n");
	return TRUE;
}

static void
gst_cnmh264dec_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{

	Gstcnmh264dec *h264 = GST_CNMH264DEC (object);

	switch (prop_id)
	{
	case ARG_NOTREORDERBASELINE:
		{
			int notr = g_value_get_int(value);
			if(notr != 0 && notr != 1) {
				GST_ERROR_OBJECT(h264, "notreorder-baseline %d exceed range", notr);
			}else{
				h264->bNotReorderForBaseline = notr;
			}
		}
		break;
	case ARG_I420LAYOUTKIND:
		{
			int layout = g_value_get_int(value);
			if(layout < 0 || layout > 2) {
				GST_ERROR_OBJECT(h264, "I420layoutkind %d exceed range.", layout);
				layout = 0;
			}
			h264->iI420LayoutKind = layout;
		}
		break;
	case ARG_SHAREFRAMEBUF:
		{
			int sha = g_value_get_int(value);
			if(sha < 0 || sha > 1) {
				GST_ERROR_OBJECT(h264, "shareFramebuf %d exceed range.", sha);
				sha = 1;
			}
			h264->iShareFrameBuf = sha;
		}
		break;
	case ARG_TIMEOUTFORDISP1FRAME:
		{
			int to = g_value_get_int(value);
			if(to < -1) {
				GST_ERROR_OBJECT(h264, "timeoutDisp1frame %d exceed range.", to);
				to = -1;
			}
			h264->iTimeoutForDisp1Frame = to;
		}
		break;
	case ARG_SUPPORTMULTIINS:
		{
			int supm = g_value_get_int(value);
			if(supm < 0 || supm > 2) {
				GST_ERROR_OBJECT(h264, "supportMultiIns %d exceed range.", supm);
				supm = 0;
			}
			h264->iSupportMultiIns = supm;
		}
		break;
	case ARG_ENABLEHIBERNATEATPAUSE:
		{
			int eh = g_value_get_int(value);
			if(eh < 0 || eh > 1) {
				GST_ERROR_OBJECT(h264, "cnmHibernateAtPause %d exceed range.", eh);
				eh = 0;
			}
			h264->bEnableHibernateAtPause = eh;
		}
		break;
	case ARG_ENABLEDYNACLK:
		{
			int ec = g_value_get_int(value);
			if(ec < 0 || ec > 1) {
				GST_ERROR_OBJECT(h264, "dynamicClock %d exceed range.", ec);
				ec = 0;
			}
			h264->bEnableDynaClk = ec;
		}
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
	return;
}

static void
gst_cnmh264dec_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	Gstcnmh264dec *h264 = GST_CNMH264DEC(object);

	switch (prop_id)
	{
	case ARG_NOTREORDERBASELINE:
		g_value_set_int(value, h264->bNotReorderForBaseline);
		break;

	case ARG_I420LAYOUTKIND:
		g_value_set_int(value, h264->iI420LayoutKind);
		break;
	case ARG_SHAREFRAMEBUF:
		g_value_set_int(value, h264->iShareFrameBuf);
		break;
	case ARG_TIMEOUTFORDISP1FRAME:
		g_value_set_int(value, h264->iTimeoutForDisp1Frame);
		break;
	case ARG_SUPPORTMULTIINS:
		g_value_set_int(value, h264->iSupportMultiIns);
		break;
	case ARG_ENABLEHIBERNATEATPAUSE:
		g_value_set_int(value, h264->bEnableHibernateAtPause);
		break;
	case ARG_ENABLEDYNACLK:
		g_value_set_int(value, h264->bEnableDynaClk);
		break;

#ifdef DEBUG_PERFORMANCE
	case ARG_TOTALFRAME:
		g_value_set_int(value, h264->totalFrames);
		break;
	case ARG_CODECTIME:
		g_value_set_int64(value, h264->codec_time);
		break;
#endif

	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
	return;
}

static __inline void _cnmh264_refresh_downpush_TsDu(Gstcnmh264dec* h264)
{
	int delay = h264->bNotReorderForBaseline ? 0 : h264->SeqInfo.MaxReorderDelay;
	gcnmdec_remove_redundant_TsDu(&h264->TsDuManager, delay+1);
	gcnmdec_refresh_output_TsDu(&h264->TsDuManager);
	return;
}

static GstFlowReturn _cnmh264_push_data(Gstcnmh264dec *h264, int iDecOutFrameIdx);

static gboolean
gst_cnmh264dec_sinkpad_event (GstPad *pad, GstEvent *event)
{
	gcnm_printf("enter %s, event 0x%x\n", __func__, GST_EVENT_TYPE(event));

	gboolean eventRet;
	Gstcnmh264dec *h264 = GST_CNMH264DEC(GST_OBJECT_PARENT(pad));

	switch(GST_EVENT_TYPE (event))
	{
	case GST_EVENT_NEWSEGMENT:
		gcnm_printf("NewSeg event is received.\n");
		{
			gcnmdec_remove_redundant_TsDu(&h264->TsDuManager, 0);
			h264->TsDuManager.LastOutputTs = GST_CLOCK_TIME_NONE;
			h264->TsDuManager.LastOutputDu = GST_CLOCK_TIME_NONE;
			eventRet = gst_pad_push_event(h264->srcpad, event);
			h264->bNewSegReceivedAfterflush = 1;
		}
		break;
	case GST_EVENT_EOS:
		gcnm_printf("EOS event is received.\n");
		{
			IppCodecStatus codecRt;
			GstFlowReturn pushRt;
			int InDreamStatusAtLock;
			if(h264->bCodecSucLaunched && ! IS_CNMDEC_FATALERR(h264->iCodecErr) && h264->SeqInfo.MaxReorderDelay > 0) {
				//if there is no reorder delay, all frames have been outputted, no need to call DecodeFrame_CMVpu()
				for(;;) {

					g_mutex_lock(h264->cnmCodecMtx);
					InDreamStatusAtLock = h264->bCnmInDream;
					if(InDreamStatusAtLock) {
						HibernateWakeupCNM(h264->pH264DecoderState, 0, &h264->bCnmInDream);
					}

					if(0 != gcnmdec_informcodec_decframeconsumed(h264->pH264DecoderState, &h264->ShareBufManager)) {
						h264->iCodecErr |= ERR_CNMDEC_DISPLAYEDAPI;
						g_mutex_unlock(h264->cnmCodecMtx);
						break;
					}

#ifdef DEBUG_PERFORMANCE
					gettimeofday(&((CnmH264ProbeData*)h264->pProbeData)->tstart, &((CnmH264ProbeData*)h264->pProbeData)->tz);
#endif
					codecRt = DecodeFrame_CMVpu(h264->pH264DecoderState, NULL, &h264->DecOutInfo, 1);
#ifdef DEBUG_PERFORMANCE
					gettimeofday(&((CnmH264ProbeData*)h264->pProbeData)->tend, &((CnmH264ProbeData*)h264->pProbeData)->tz);
					h264->codec_time += (((CnmH264ProbeData*)h264->pProbeData)->tend.tv_sec - ((CnmH264ProbeData*)h264->pProbeData)->tstart.tv_sec)*1000000 + (((CnmH264ProbeData*)h264->pProbeData)->tend.tv_usec - ((CnmH264ProbeData*)h264->pProbeData)->tstart.tv_usec);
#endif

					if(!h264->iShareFrameBuf && codecRt == IPP_STATUS_FRAME_COMPLETE) {
						(h264->ShareBufManager.DecOutFrameConsumeStatus[h264->DecOutInfo.idxFrameOutput])++;
					}
					if(InDreamStatusAtLock) {
						HibernateWakeupCNM(h264->pH264DecoderState, 1, &h264->bCnmInDream);
					}
					g_mutex_unlock(h264->cnmCodecMtx);

					if(codecRt == IPP_STATUS_FRAME_COMPLETE) {
						gcnm_printf("DecodeFrame_CMVpu ret IPP_STATUS_FRAME_COMPLETE at EOS, %d\n", h264->DecOutInfo.idxFrameOutput);
						h264->totalFrames++;
						_cnmh264_refresh_downpush_TsDu(h264);
						pushRt = _cnmh264_push_data(h264, h264->DecOutInfo.idxFrameOutput);
						if(pushRt == GST_FLOW_ERROR) {
							break;
						}
					}else if(codecRt == IPP_STATUS_BS_END) {
						gcnm_printf("DecodeFrame_CMVpu ret IPP_STATUS_BS_END at EOS\n");
						break;
					}else{
						h264->iCodecErr |= WARN_CNMDEC_EOS;
						g_warning("DecodeFrame_CMVpu() in EOS fail, ret %d", codecRt);
						break;
					}
				}
			}

			//once DecodeFrame_CMVpu return IPP_STATUS_BS_END, the codec couldn't accept new data. If application want to decoding continue(for example, seek to begin, and play again), need to close and init a new codec
			g_mutex_lock(h264->cnmCodecMtx);
			if(h264->pH264DecoderState) {
				if(h264->bCnmInDream) {
					HibernateWakeupCNM(h264->pH264DecoderState, 0, &h264->bCnmInDream);
				}
				h264->iCodecErr |= gcnmdec_close_codec(&h264->pH264DecoderState, &h264->ShareBufManager);
			}
			g_mutex_unlock(h264->cnmCodecMtx);

			h264->b1stFrame_forFileplay = 1;
			h264->bCodecSucLaunched = 0;
			gcnmdec_remove_redundant_TsDu(&h264->TsDuManager, 0);
			h264->TsDuManager.LastOutputTs = GST_CLOCK_TIME_NONE;
			h264->TsDuManager.LastOutputDu = GST_CLOCK_TIME_NONE;
			eventRet = gst_pad_push_event(h264->srcpad, event);
		}
		break;
	case GST_EVENT_FLUSH_STOP:
		h264->bNewSegReceivedAfterflush = 0;
	default:
		eventRet = gst_pad_event_default(pad, event);
		break;
	}

	return eventRet;
}

static GstFlowReturn _cnmh264_push_data(Gstcnmh264dec* h264, int iDecOutFrameIdx)
{
	GstFlowReturn FlowRt = GST_FLOW_OK;
	GstBuffer *outBuf = NULL;

	gcnm_printfmore("enter %s\n", __func__);

	if(h264->iShareFrameBuf) {
		int idxInDFBShare;
		void* VA = h264->DecFramesInfoArr[iDecOutFrameIdx].pUserData1;
		unsigned int PA = (unsigned int)((CnmPContMem*)h264->DecFramesInfoArr[iDecOutFrameIdx].phyY)->PAddr;
		outBuf = gst_buffer_new();
		if(outBuf == NULL) {
			return GST_FLOW_ERROR;
		}
		GST_BUFFER_DATA(outBuf) = VA;
		GST_BUFFER_SIZE(outBuf) = h264->iAlignedBufSz;
		gst_buffer_set_caps(outBuf, GST_PAD_CAPS(h264->srcpad));
		idxInDFBShare = ((int)(h264->DecFramesInfoArr[iDecOutFrameIdx].pUserData2)>>16) & 0xff;
		IPPGST_BUFFER_CUSTOMDATA(outBuf) = (gpointer) (&h264->ShareBufManager.sideinfoDFBShareArr[idxInDFBShare]);
		setSideInfo(&h264->ShareBufManager.sideinfoDFBShareArr[idxInDFBShare], h264->SeqInfo.FrameROI.x, h264->SeqInfo.FrameROI.y, h264->SeqInfo.FrameAlignedWidth, h264->SeqInfo.FrameAlignedHeight, PA, VA);
		h264->DecFramesInfoArr[iDecOutFrameIdx].pUserData2 = (void*)((idxInDFBShare<<16)|h264->ShareBufManager.CurCodecSerialNum|(iDecOutFrameIdx<<24));
		h264->DecFramesInfoArr[iDecOutFrameIdx].pUserData3 = (void*)(&h264->ShareBufManager);
		GST_BUFFER_MALLOCDATA(outBuf) = (guint8*)(&(h264->DecFramesInfoArr[iDecOutFrameIdx]));
		GST_BUFFER_FREE_FUNC(outBuf) = (GFreeFunc)gcnmdec_downbuf_onfinalize;
		FRAMES_WATCHMAN_REF(h264->ShareBufManager.watcher);

		GST_BUFFER_TIMESTAMP(outBuf) = h264->TsDuManager.LastOutputTs;
		GST_BUFFER_DURATION(outBuf) = h264->TsDuManager.LastOutputDu;

		g_mutex_lock(h264->ShareBufManager.mtx);
		h264->ShareBufManager.DFBShareArr[idxInDFBShare].pUserData3 = (void*)1;	//DFBShareArr[x].pUserData3 == 0 means display sink do not hold this frame, 1 means display sink is holding this frame.
		h264->ShareBufManager.iSinkOwnedDecFrameBufCnt++;
		g_mutex_unlock(h264->ShareBufManager.mtx);

		if(h264->bNewSegReceivedAfterflush) {
			FlowRt = gst_pad_push(h264->srcpad, outBuf);
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


	FlowRt = gcnmdec_require_framebuf_fromdownplugin(&h264->DownFrameLayout, h264->srcpad, &outBuf, h264->iI420LayoutKind==0);
	if(GST_FLOW_OK != FlowRt) {
		if(FlowRt != GST_FLOW_WRONG_STATE) {
			//during seeking, sink element often return GST_FLOW_WRONG_STATUS. No need to echo this message
			g_warning("Require gstbuffer from downstream plugin fail, ret %d!", FlowRt);
		}
		return FlowRt;
	}
#ifdef LOG_FRAMECOPYTIME
	gettimeofday(&((CnmH264ProbeData*)h264->pProbeData)->tlog_0, NULL);
#endif
	gcnmdec_copy_to_downframebuf(GST_BUFFER_DATA(outBuf), &h264->DownFrameLayout, &(h264->DecFramesInfoArr[iDecOutFrameIdx]), &(h264->SeqInfo.FrameROI));
#ifdef LOG_FRAMECOPYTIME
	gettimeofday(&((CnmH264ProbeData*)h264->pProbeData)->tlog_1, NULL);
	((CnmH264ProbeData*)h264->pProbeData)->log_copycnt++;
	((CnmH264ProbeData*)h264->pProbeData)->log_copytime += (((CnmH264ProbeData*)h264->pProbeData)->tlog_1.tv_sec - ((CnmH264ProbeData*)h264->pProbeData)->tlog_0.tv_sec)*1000000 + (((CnmH264ProbeData*)h264->pProbeData)->tlog_1.tv_usec - ((CnmH264ProbeData*)h264->pProbeData)->tlog_0.tv_usec);
#endif


	GST_BUFFER_TIMESTAMP(outBuf) = h264->TsDuManager.LastOutputTs;
	GST_BUFFER_DURATION(outBuf) = h264->TsDuManager.LastOutputDu;
	if(h264->bNewSegReceivedAfterflush) {
		FlowRt = gst_pad_push(h264->srcpad, outBuf);
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

static int _cnmh264dec_allocateframebufs(void* pElement, IppCMVpuDecParsedSeqInfo* pSeqInfo, int* piFrameCnt, IppCMVpuDecFrameAddrInfo** pFrameArr, int* piFrameStride)
{
	int codecframeNeedCnt;
	Gstcnmh264dec* ele = (Gstcnmh264dec*)pElement;
	int ret;
	int bCacheable;
	int framebufsz = (pSeqInfo->FrameAlignedWidth * pSeqInfo->FrameAlignedHeight * 3)>>1;

	*piFrameStride = pSeqInfo->FrameAlignedWidth;
	*pFrameArr = ele->DecFramesInfoArr;

	if(ele->iShareFrameBuf) {
		bCacheable = 0;
		codecframeNeedCnt = gcnmdec_smartdecide_framealloc_cnt(pSeqInfo->MinFrameBufferCount, IPPCMDEC_DECFRAMEBUF_MAX_H264, IPPGST_CMVPUDEC_FRAMEBUF_DEFAULT_MAXTOTALSIZE, framebufsz, IPPGSTCNMDEC_SPEC_H264);
	}else{
		bCacheable = 0;
		codecframeNeedCnt = pSeqInfo->MinFrameBufferCount;
	}

	ret = prepare_framebufforcodec(pSeqInfo->FrameAlignedHeight, pSeqInfo->FrameAlignedWidth, piFrameCnt, codecframeNeedCnt, &ele->ShareBufManager, ele->DecFramesInfoArr, &ele->iDecFrameBufArrLen, ele->iShareFrameBuf, bCacheable);

	return ret;
}


static __inline unsigned int
_peek_unsigned_from_Leftstream(Ipp8u* pData, int len)
{
	int i;
	unsigned int val = 0;
	for(i=0;i<len;i++) {
		val = val<<8 | pData[i];
	}
	return val;  /* the length of the NAL unit */
}

static int _cnmh264_fill_avcCstream(Gstcnmh264dec* h264, GCNMDec_StmBuf* pCnmInputBuf, unsigned char* pInData, int nInDataLen)
{
	int nalLen;
	int len = nInDataLen;
	unsigned char* pNal = pInData;
	while(len>h264->nalLenFieldSize) {
		nalLen = _peek_unsigned_from_Leftstream(pNal, h264->nalLenFieldSize);
		if((unsigned int)nalLen > (unsigned int)(len-h264->nalLenFieldSize) || nalLen < 0) {
			//error bitstream, left stream is less than the NAL unit size
			nalLen = len-h264->nalLenFieldSize;
		}
		pNal += h264->nalLenFieldSize;
		if(nalLen > 0) {	//in some .mov, there are zero padding bytes at the end of stream
			gcnmdec_fill_stream(pCnmInputBuf, H264NALSyncCode, 4);
			gcnmdec_fill_stream(pCnmInputBuf, pNal, nalLen);
		}
		pNal += nalLen;
		len -= (h264->nalLenFieldSize + nalLen);

	}
	return 0;
}

//not calculate the frame length, but calculate the upper limit of frame length
static int _cal_framelenUpperSz_from_avcC(unsigned char* pData, int srclen, int lenfieldsz)
{
	int dst_len, nalLen;
	if(lenfieldsz >= 4) {
		//therefore, avcC format frame is longer than byte stream format
		return srclen;
	}
	dst_len = 0;
	while(srclen > lenfieldsz) {
		nalLen = _peek_unsigned_from_Leftstream(pData, lenfieldsz);
		if((unsigned int)nalLen > (unsigned int)(srclen - lenfieldsz) || nalLen < 0) {
			nalLen = srclen - lenfieldsz;
		}
		srclen -= lenfieldsz + nalLen;
		pData += lenfieldsz + nalLen;
		dst_len += 4 + nalLen;
	}
	dst_len += srclen;
	return dst_len;
}

static int gst_cnmh264dec_launch_codec(GstBuffer* buf, Gstcnmh264dec* h264)
{
	GCNMDec_StartUpDec StartUp;
	int StartUpRet = 0;
	void* pSStmBuf = NULL;
	IppCMVpuDecInitPar InitPar;
	CnmPContMem* mem;

	memset(&InitPar, 0, sizeof(InitPar));
	if(h264->bNotReorderForBaseline) {
		InitPar.ReorderEnable = 0;
	}else{
		InitPar.ReorderEnable = 1;
	}
	InitPar.FilePlayEnable = 1;
	InitPar.VideoStreamFormat = IPP_VIDEO_STRM_FMT_H264;
	InitPar.MultiInstanceSync = h264->iSupportMultiIns;
	InitPar.DynamicClkEnable = h264->bEnableDynaClk;

	if(h264->StmBufPool[0].iBufSize == 0) {
		int framelenUpperSz;
		//prepare static stream buffer
		InitPar.StaticBitstreamBufSize = 384*1024;  //init a value

		if(h264->is_avcC) {
			framelenUpperSz = _cal_framelenUpperSz_from_avcC(GST_BUFFER_DATA(buf), GST_BUFFER_SIZE(buf), h264->nalLenFieldSize);
		}else{
			framelenUpperSz = GST_BUFFER_SIZE(buf);
		}

		if(InitPar.StaticBitstreamBufSize < (unsigned int)(framelenUpperSz + h264->seqMetaLen + IPPCMDEC_STATICSTMBUF_PROTECTSPACE)) {
			InitPar.StaticBitstreamBufSize = framelenUpperSz + h264->seqMetaLen + IPPCMDEC_STATICSTMBUF_PROTECTSPACE;
		}
		InitPar.StaticBitstreamBufSize = (InitPar.StaticBitstreamBufSize + 1023) & (~1023); //cnm static stream buffer size must 1024 aligned

		gcnm_printf("allocate static stream buffer, want sz %d\n", InitPar.StaticBitstreamBufSize);
		mem = new_cnmStmBuf(InitPar.StaticBitstreamBufSize);
		if(mem == NULL) {
			g_warning("Err: allocate static stream buffer fail, want sz %d", InitPar.StaticBitstreamBufSize);
			h264->iMemErr = -2;
			return -2;
		}
		pSStmBuf = mem->VAddr;
		InitPar.phyAddrStaticBitstreamBuf = mem->PAddr;
		h264->StmBufPool[0].BufPhyAddr = InitPar.phyAddrStaticBitstreamBuf;
		h264->StmBufPool[0].BufVirAddr = (unsigned char*)pSStmBuf;
		h264->StmBufPool[0].iBufSize = InitPar.StaticBitstreamBufSize;
		h264->StmBufPool[0].mem = mem;
	}else{
		//static stream buffer has been prepared
		gcnm_printf("static stream buffer already prepared, sz %d, phyaddr 0x%x.\n", h264->StmBufPool[0].iBufSize, (unsigned int)(h264->StmBufPool[0].BufPhyAddr));
		InitPar.StaticBitstreamBufSize = h264->StmBufPool[0].iBufSize;
		InitPar.phyAddrStaticBitstreamBuf = h264->StmBufPool[0].BufPhyAddr;
	}

	h264->StmBufPool[0].iDataLen = 0;
	//fill seq header and 1st frame
	gcnmdec_fill_stream(&(h264->StmBufPool[0]), h264->seqMeta, h264->seqMetaLen);
	if(h264->is_avcC) {
		_cnmh264_fill_avcCstream(h264, &(h264->StmBufPool[0]), GST_BUFFER_DATA(buf), GST_BUFFER_SIZE(buf));
	}else{
		gcnmdec_fill_stream(&(h264->StmBufPool[0]), GST_BUFFER_DATA(buf), GST_BUFFER_SIZE(buf));
	}

	//start up codec
	StartUp.StmBufPool = h264->StmBufPool;
	StartUp.virAddrStaticBitstreamBuf = h264->StmBufPool[0].BufVirAddr;
	StartUp.pInitPar = &InitPar;
	StartUp.pSeqInfo = &h264->SeqInfo;
	StartUp.iFilledLen_forCodecStart = h264->StmBufPool[0].iDataLen;
	StartUp.allocate_framebuf = _cnmh264dec_allocateframebufs;  //during start up codec, _cnmh264dec_allocateframebufs() will allocate frame buffer

	//below is output parameter
	//StartUp.pCnmDec = NULL;
	//StartUp.iCodecApiErr = 0;

	h264->iCodecErr = 0; //clear it before a new codec instance
	gcnm_printf("Call gcnmdec_startup_codec()...\n");

	g_mutex_lock(h264->cnmCodecMtx);
	h264->bCnmInDream = 0;
	StartUpRet = gcnmdec_startup_codec(h264, &StartUp, &h264->ShareBufManager);
	h264->pH264DecoderState = StartUp.pCnmDec;
	g_mutex_unlock(h264->cnmCodecMtx);

	h264->iCodecErr = StartUp.iCodecApiErr;
	h264->iMemErr = StartUp.iMemoryErr;
	if(G_LIKELY(StartUpRet == 0)) {
		//succeed
		h264->bCodecSucLaunched = 1;
		gcnm_printf("Launch cnm decoder succeed!\n");
	}else{
		GST_ERROR_OBJECT(h264, "Launch cnm decoder fail, ret %d", StartUpRet);
	}
	return StartUpRet;

}

static __inline unsigned char*
seek_bytestream_h264nal(unsigned char* pData, int len, int* pStartCodeLen)
{
	unsigned char* pStartCode = seek_SC_000001xx(pData, len);
	if(pStartCode == NULL) {
		return NULL;
	}
	*pStartCodeLen = 3;
	if(pStartCode>pData) {
		//though 0x000001 is the official startcode for h264, but for most of h264, use 0x00000001 as the startcode.
		//therefore, if 0x00 exist before 0x000001, we think 0x00000001 is the startcode
		if(*(pStartCode-1) == 0) {
			pStartCode--;
			*pStartCodeLen = 4;
		}
	}
	return pStartCode;
}

#define H264_NALU_TYPE_SPS  	7
#define H264_NALU_TYPE_PPS  	8
static unsigned char*
found_h264_bytestream_sps(unsigned char* pData, int len)
{
	int SClen;
	unsigned char* pNAL;
	unsigned char* pSPS = NULL;
	//seek SPS
	for(;;) {
		pNAL = seek_bytestream_h264nal(pData, len, &SClen);
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
	return pSPS;
}

static int
judge_AllZeroAVCCNALs(unsigned char* pData, int len, int nal_length_size)
{
	int nalLen;
	int zeroNalcnt = 0;
	if(nal_length_size <= 0) {
		return 0;
	}
	while(len > nal_length_size) {
		nalLen = _peek_unsigned_from_Leftstream(pData, nal_length_size);
		if(nalLen != 0) {
			return 0;	//not all zero nal
		}
		zeroNalcnt++;
		if(zeroNalcnt > 10) {
			return 1;	//occur continuous zero nals, we consider the data is all zero
		}
		len -= nal_length_size;
		pData += nal_length_size;
	}
	return 1;
}

static GstFlowReturn
gst_cnmh264dec_chain(GstPad *pad, GstBuffer *buf)
{
	gcnm_printfmore("enter %s, in buf sz %d, ts %lld\n", __func__, GST_BUFFER_SIZE(buf), GST_BUFFER_TIMESTAMP(buf));
	int ret, tmp, bCallDecAgain, bStopDecodingLoop, framelenUpperSz;
	GstFlowReturn FlowRt = GST_FLOW_OK;
	Gstcnmh264dec* h264 = GST_CNMH264DEC(GST_OBJECT_PARENT(pad));
	IppCodecStatus codecRt;
	GCNMDec_StmBuf* pCnmInputBuf;
	int cnmDreamStatusAtlock;
	unsigned char* pData = GST_BUFFER_DATA(buf);
	unsigned int DataLen = GST_BUFFER_SIZE(buf);

	if(h264->is_avcC) {
		if(judge_AllZeroAVCCNALs(pData, DataLen, h264->nalLenFieldSize)) {
			//in some .mov, there are zero padding bytes at the end of stream
			gst_buffer_unref(buf);
			FlowRt = GST_FLOW_OK;
			goto chain_exit;
		}
	}

	if(h264->is_avcC == FALSE) {
		if(!h264->bSPSReceivedForNonAVCC) {
			unsigned char* pSPS = found_h264_bytestream_sps(pData, DataLen);
			if(pSPS == NULL) {
				gst_buffer_unref(buf);
				gcnm_printf("give up the input stream data, because not contain sps\n");
				FlowRt = GST_FLOW_OK;
				goto chain_exit;//give up the data
			}else{
				if(pSPS != pData) {
					//recreate buf, let SPS at the beginning of buf
					GstBuffer* newbuf = gst_buffer_new_and_alloc(DataLen - (pSPS-pData));
					if(newbuf == NULL) {
						gcnm_printf("Err: call gst_buffer_new_and_alloc fail, line %d\n", __LINE__);
						g_warning("Err: call gst_buffer_new_and_alloc fail, line %d", __LINE__);
						GST_ERROR_OBJECT(h264, "Err: call gst_buffer_new_and_alloc fail, line %d", __LINE__);
						FlowRt = GST_FLOW_ERROR;
						goto chain_exit;
					}
					memcpy(GST_BUFFER_DATA(newbuf), pSPS, DataLen - (pSPS-pData));
					GST_BUFFER_DURATION(newbuf) = GST_BUFFER_DURATION(buf);
					GST_BUFFER_TIMESTAMP(newbuf) = GST_BUFFER_TIMESTAMP(buf);
					gst_buffer_unref(buf);
					buf = newbuf;
					pData = GST_BUFFER_DATA(buf);
					DataLen = GST_BUFFER_SIZE(buf);
				}
				h264->bSPSReceivedForNonAVCC = 1;
			}
		}
	}
	gcnmdec_insert_TsDu(&h264->TsDuManager, buf);


	if(h264->b1stFrame_forFileplay){
		GstCaps *CapTmp;

		h264->b1stFrame_forFileplay = 0;
		ret = gst_cnmh264dec_launch_codec(buf, h264);
		if(ret != 0) {
			gst_buffer_unref(buf);
			gcnm_printf("Err: gst_cnmh264dec_launch_codec fail, ret %d\n", ret);
			FlowRt = GST_FLOW_ERROR;
			goto chain_exit;
		}
		pCnmInputBuf = &(h264->StmBufPool[0]);

		h264->DownFrameLayout.iDownFrameWidth = h264->SeqInfo.FrameROI.width;
		h264->DownFrameLayout.iDownFrameHeight = h264->SeqInfo.FrameROI.height;
		if(h264->iI420LayoutKind != 2) {
			gcnmdec_update_downbuf_layout(h264->DownFrameLayout.iDownFrameWidth, h264->DownFrameLayout.iDownFrameHeight, 0, &h264->DownFrameLayout);
		}else{
			gcnmdec_update_downbuf_layout(h264->DownFrameLayout.iDownFrameWidth, h264->DownFrameLayout.iDownFrameHeight, 1, &h264->DownFrameLayout);
		}

		h264->iAlignedBufSz = (h264->SeqInfo.FrameAlignedWidth * h264->SeqInfo.FrameAlignedHeight * 3)>>1;

		gcnm_printf("Will set srcpad cap, fps %d/%d, w %d, h %d\n", h264->num, h264->den, h264->DownFrameLayout.iDownFrameWidth, h264->DownFrameLayout.iDownFrameHeight);
		CapTmp = gst_caps_new_simple ("video/x-raw-yuv",
			"format", GST_TYPE_FOURCC, GST_STR_FOURCC ("I420"),
			"width", G_TYPE_INT, h264->DownFrameLayout.iDownFrameWidth,
			"height", G_TYPE_INT, h264->DownFrameLayout.iDownFrameHeight,
			"framerate", GST_TYPE_FRACTION, h264->num, h264->den,
			NULL);
		gst_pad_set_caps(h264->srcpad, CapTmp);
		gst_caps_unref(CapTmp);

	}else{
		//load frame data
		if(h264->is_avcC) {
			framelenUpperSz = _cal_framelenUpperSz_from_avcC(pData, DataLen, h264->nalLenFieldSize);
		}else{
			framelenUpperSz = DataLen;
		}
		tmp = gcnmdec_select_stream(h264->StmBufPool, GSTCNMDEC_H264_STMPOOLSZ, framelenUpperSz);
		if( tmp < 0 || tmp >= GSTCNMDEC_H264_STMPOOLSZ ) {
			h264->iMemErr = -3;
			gcnm_printf("Err: gcnmdec_select_stream fail, ret %d", tmp);
			GST_ERROR_OBJECT(h264, "Err: gcnmdec_select_stream fail, ret %d", tmp);
			gst_buffer_unref(buf);
			FlowRt = GST_FLOW_ERROR;
			goto chain_exit;
		}
		pCnmInputBuf = &(h264->StmBufPool[tmp]);
		if(TRUE==h264->is_avcC){
			_cnmh264_fill_avcCstream(h264, pCnmInputBuf, pData, DataLen);
		}else{
			gcnmdec_fill_stream(pCnmInputBuf, pData, DataLen);
		}
	}
	gst_buffer_unref(buf);

	if(!h264->bCodecSucLaunched) {
		FlowRt = GST_FLOW_ERROR;
		goto chain_exit;
	}

	h264->FilePlay_StmBuf.phyAddrBitstreamBuf = pCnmInputBuf->BufPhyAddr;
	h264->FilePlay_StmBuf.DataLen = pCnmInputBuf->iDataLen;


	bStopDecodingLoop = 0;
	do{
		bCallDecAgain = 0;

		g_mutex_lock(h264->cnmCodecMtx);
		cnmDreamStatusAtlock = h264->bCnmInDream;
		if(cnmDreamStatusAtlock) {
			HibernateWakeupCNM(h264->pH264DecoderState, 0, &h264->bCnmInDream);
		}

		if(0 != gcnmdec_informcodec_decframeconsumed(h264->pH264DecoderState, &h264->ShareBufManager)) {
			h264->iCodecErr |= ERR_CNMDEC_DISPLAYEDAPI;
			g_mutex_unlock(h264->cnmCodecMtx);
			FlowRt = GST_FLOW_ERROR;
			goto chain_exit;
		}

#ifdef DEBUG_PERFORMANCE
		gettimeofday(&((CnmH264ProbeData*)h264->pProbeData)->tstart, &((CnmH264ProbeData*)h264->pProbeData)->tz);
#endif
		codecRt = DecodeFrame_CMVpu(h264->pH264DecoderState, &h264->FilePlay_StmBuf, &h264->DecOutInfo, 0);
#ifdef DEBUG_PERFORMANCE
		gettimeofday(&((CnmH264ProbeData*)h264->pProbeData)->tend, &((CnmH264ProbeData*)h264->pProbeData)->tz);
		h264->codec_time += (((CnmH264ProbeData*)h264->pProbeData)->tend.tv_sec - ((CnmH264ProbeData*)h264->pProbeData)->tstart.tv_sec)*1000000 + (((CnmH264ProbeData*)h264->pProbeData)->tend.tv_usec - ((CnmH264ProbeData*)h264->pProbeData)->tstart.tv_usec);
#endif

		if(! h264->iShareFrameBuf && (codecRt == IPP_STATUS_OUTPUT_DATA || codecRt == IPP_STATUS_FRAME_COMPLETE)) {
			(h264->ShareBufManager.DecOutFrameConsumeStatus[h264->DecOutInfo.idxFrameOutput])++;//In theory, under non-iShareFrameBuf mode, should after copy the frame data, then set the status that the frame is consumed. Anyway, setting it in advance is no side effect.
		}
		if(cnmDreamStatusAtlock) {
			HibernateWakeupCNM(h264->pH264DecoderState, 1, &h264->bCnmInDream);
		}
		g_mutex_unlock(h264->cnmCodecMtx);

		if(!(codecRt == IPP_STATUS_OUTPUT_DATA || (codecRt==IPP_STATUS_BUFFER_FULL && h264->iShareFrameBuf))) {
			//the data has been consumed
			pCnmInputBuf->iDataLen = 0;
		}

		switch(codecRt) {
		case IPP_STATUS_OUTPUT_DATA:
			//No frame is decoded, because the input frame data hasn't been consumed by VPU, therefore, application should feed this frame again.
			//But some frame still need output, which are reorder delayed frames.
			bCallDecAgain = 1;
		case IPP_STATUS_FRAME_COMPLETE:
			gcnm_printfmore("DecodeFrame_CMVpu ret IPP_STATUS_OUTPUT_DATA or IPP_STATUS_FRAME_COMPLETE, %d, %d, %d\n", h264->totalFrames, h264->DecOutInfo.idxFrameOutput, h264->DecOutInfo.NumOfErrMbs);
			h264->totalFrames++;
			_cnmh264_refresh_downpush_TsDu(h264);
			FlowRt = _cnmh264_push_data(h264, h264->DecOutInfo.idxFrameOutput);
			break;
		case IPP_STATUS_NOERR:
			gcnm_printf("DecodeFrame_CMVpu ret NOERR, %d %d %d\n", h264->DecOutInfo.OneFrameUnbrokenDecoded, h264->DecOutInfo.idxFrameOutput, h264->DecOutInfo.idxFrameDecoded);
			break;
		case IPP_STATUS_NOTSUPPORTED_ERR:
			h264->iCodecErr |= WARN_CNMDEC_FRAMENOTSUPPORT;
			_cnmh264_refresh_downpush_TsDu(h264);
			gcnm_printf("DecodeFrame_CMVpu ret IPP_STATUS_NOTSUPPORTED_ERR.\n");
			break;
		case IPP_STATUS_FRAME_ERR:
			h264->iCodecErr |= WARN_CNMDEC_FRAMECORRUPT;
			gcnm_printf("DecodeFrame_CMVpu ret WARN_CNMDEC_FRAMECORRUPT, probably the frame is corrupt.\n");
			break;
		case IPP_STATUS_BUFFER_FULL:
#ifndef NOTECHO_BUFFERFULL_WARNING
			g_warning("Warning: DecodeFrame_CMVpu ret buffer full, waiting sink release buffer\n");
#endif
			if(h264->iShareFrameBuf) {
				int waitRt;
				bCallDecAgain = 1;
				waitRt = wait_sink_retDecFrameBuf(h264->iTimeoutForDisp1Frame, -1, &h264->ShareBufManager, &h264->ElementState);
				if(waitRt > 0) {
					gcnm_printf("wait_sink_retDecFrameBuf() stopped by element state change\n");
					bStopDecodingLoop = 1;	//element isn't running, give up decoding
					//give up this frame, since we have limitted the length of TS queue, little need to remove the TS of this frame from TS queue
					pCnmInputBuf->iDataLen = 0;
				}else if(waitRt < 0) {
					//wait time out or error occur
					FlowRt = GST_FLOW_ERROR;
					h264->iCodecErr |= (waitRt == -2 ? ERR_WAITSINKRETFRAME_SINKNOFRAME: ERR_WAITSINKRETFRAME_TIMEOUT);
					goto chain_exit;
				}
				break;  //break switch
			}
		default:
			h264->iCodecErr |= ERR_CNMDEC_DECFRAME_SERIOUSERR;
			gcnm_printf("Err: DecodeFrame_CMVpu fail, ret %d\n", codecRt);
			g_warning("Err: DecodeFrame_CMVpu fail, ret %d", codecRt);
			GST_ERROR_OBJECT(h264, "Err: DecodeFrame_CMVpu fail, ret %d", codecRt);
			FlowRt = GST_FLOW_ERROR;
			goto chain_exit;
		}
	}while(bCallDecAgain == 1 && bStopDecodingLoop == 0);

chain_exit:
	if(FlowRt < GST_FLOW_UNEXPECTED) {
		GST_ELEMENT_ERROR(h264, STREAM, DECODE, (NULL), ("%s() flow err %d, codec err 0x%x, mem err %d", __FUNCTION__, FlowRt, h264->iCodecErr, h264->iMemErr));
	}

	return FlowRt;
}

static void
gst_cnmh264dec_clean_streambuf(Gstcnmh264dec *h264)
{
	int i;
	gcnm_printf("free stream buffer\n");

	for(i=0;i<GSTCNMDEC_H264_STMPOOLSZ;i++) {
		if(h264->StmBufPool[i].iBufSize != 0) {
			delete_cnmStmBuf(h264->StmBufPool[i].mem);
			h264->StmBufPool[i].iBufSize = 0;
			h264->StmBufPool[i].BufPhyAddr = 0;
			h264->StmBufPool[i].BufVirAddr = NULL;
			h264->StmBufPool[i].iDataLen = 0;
			h264->StmBufPool[i].mem = NULL;
		}
	}
	if(NULL!=h264->seqMeta){
		g_free(h264->seqMeta);
		h264->seqMeta = NULL;
		h264->seqMetaBufSz = 0;
		h264->seqMetaLen = 0;
	}
	return;
}

static gboolean
gst_cnmh264dec_null2ready(Gstcnmh264dec *h264)
{
	gcnm_printf("enter %s\n", __func__);

	h264->pH264DecoderState = NULL;
	h264->totalFrames = 0;
	h264->codec_time = 0;

	h264->ShareBufManager.watcher = gcnmdec_watcher_create(h264, NULL, &h264->ShareBufManager);   //create watcher
	if(h264->ShareBufManager.watcher == NULL) {
		g_warning("Err: gcnmdec_watcher_create() fail");
		return FALSE;
	}

	return TRUE;
}


static gboolean
gst_cnmh264dec_ready2null(Gstcnmh264dec *h264)
{
	gcnm_printf("enter %s\n", __func__);

	//close decoder
	g_mutex_lock(h264->cnmCodecMtx);
	if(h264->pH264DecoderState) {
		if(h264->bCnmInDream) {
			HibernateWakeupCNM(h264->pH264DecoderState, 0, &h264->bCnmInDream);
		}
		h264->iCodecErr |= gcnmdec_close_codec(&h264->pH264DecoderState, &h264->ShareBufManager);
	}
	g_mutex_unlock(h264->cnmCodecMtx);

	gcnmdec_remove_redundant_TsDu(&h264->TsDuManager, 0);

	//free buffers
	gst_cnmh264dec_clean_streambuf(h264);

	if(! h264->iShareFrameBuf) {
		gstcnmElement_clean_nonShareFramebuf(h264->DecFramesInfoArr, &h264->iDecFrameBufArrLen);
	}else{
		g_mutex_lock(h264->ShareBufManager.mtx);
		gstcnmElement_clean_idleShareFBs(&h264->ShareBufManager, NULL);	//for those share frame buffer which is still owned by sink, will free them on their gstbuffer finalizing callback function
		h264->ShareBufManager.NotNeedFMemAnyMore = 1;
		g_mutex_unlock(h264->ShareBufManager.mtx);
	}

	if(h264->ShareBufManager.watcher) {
		FRAMES_WATCHMAN_UNREF(h264->ShareBufManager.watcher);   //smart release watcher, free frame buffers
	}

#ifdef DEBUG_PERFORMANCE
	printf("codec totally outputed %d frames, spend %lld us\n", h264->totalFrames, h264->codec_time);
#else
	gcnm_printf("codec totally outputed %d frames, spend %lld us\n", h264->totalFrames, h264->codec_time);
#endif

#ifdef LOG_FRAMECOPYTIME
	printf("copy %d frame total cost %d us\n", ((CnmH264ProbeData*)h264->pProbeData)->log_copycnt, ((CnmH264ProbeData*)h264->pProbeData)->log_copytime);
#endif

	gcnm_printf("-----------At ready2null, c&m codec error %d, memory error %d\n", h264->iCodecErr, h264->iMemErr);


	return TRUE;
}

static int gst_cnmh264dec_ready2paused(Gstcnmh264dec *h264)
{
	gcnm_printf("gst_cnmh264dec_ready2paused() is called\n");
	//prepare for new stream

	return 0;
}

static int gst_cnmh264dec_paused2ready(Gstcnmh264dec* h264)
{
	gcnm_printf("gst_cnmh264dec_paused2ready() is called\n");
	gcnm_printf("-----------At paused2ready, cnm decoder error status %d\n", h264->iCodecErr);
	return 0;
}

static __inline int paused2play(Gstcnmh264dec* h264)
{
	IppCodecStatus ret = IPP_STATUS_NOERR;
	g_mutex_lock(h264->cnmCodecMtx);
	if(h264->pH264DecoderState && h264->bEnableHibernateAtPause && h264->bCnmInDream) {
		ret = HibernateWakeupCNM(h264->pH264DecoderState, 0, &h264->bCnmInDream);
	}
	g_mutex_unlock(h264->cnmCodecMtx);
	if(IPP_STATUS_NOERR != ret) {
		g_warning("Wakeup cnm fail at pause2paly, ret %d", ret);
		return -1;
	}
	return 0;
}

static __inline int play2paused(Gstcnmh264dec* h264)
{
	IppCodecStatus ret = IPP_STATUS_NOERR;
	g_mutex_lock(h264->cnmCodecMtx);
	if(h264->pH264DecoderState && h264->bEnableHibernateAtPause && ! h264->bCnmInDream) {
		ret = HibernateWakeupCNM(h264->pH264DecoderState, 1, &h264->bCnmInDream);
	}
	g_mutex_unlock(h264->cnmCodecMtx);
	if(IPP_STATUS_NOERR != ret) {
		g_warning("Hibernate cnm fail at pause2paly, ret %d", ret);
		return -1;
	}
	return 0;
}


static GstStateChangeReturn
gst_cnmh264dec_change_state(GstElement *element, GstStateChange transition)
{
	gcnm_printf("enter %s, change from %d to %d\n", __func__, transition>>3, transition&7);

	GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;
	Gstcnmh264dec *h264 = GST_CNMH264DEC(element);

	switch (transition)
	{
	case GST_STATE_CHANGE_NULL_TO_READY:
		if(!gst_cnmh264dec_null2ready(h264)){
			goto statechange_failed;
		}
		break;
	case GST_STATE_CHANGE_READY_TO_PAUSED:
		gst_cnmh264dec_ready2paused(h264);
		break;
	case GST_STATE_CHANGE_PAUSED_TO_PLAYING:
		if(0 != paused2play(h264)) {
			goto statechange_failed;
		}
		break;

	case GST_STATE_CHANGE_PAUSED_TO_READY:
		set_ElementState(&h264->ElementState, CNMELESTATE_RESTING);
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
		if(0 != play2paused(h264)) {
			goto statechange_failed;
		}
		break;
	case GST_STATE_CHANGE_PAUSED_TO_READY:
		gst_cnmh264dec_paused2ready(h264);
		break;
	case GST_STATE_CHANGE_READY_TO_NULL:
		if(!gst_cnmh264dec_ready2null(h264)){
			goto statechange_failed;
		}
		break;

	case GST_STATE_CHANGE_READY_TO_PAUSED:
		set_ElementState(&h264->ElementState, CNMELESTATE_RUNNING);
		break;
	default:
		break;
	}

	return ret;

	 /* ERRORS */
statechange_failed:
	{
		/* subclass must post a meaningfull error message */
		GST_ERROR_OBJECT (h264, "state change failed");
		return GST_STATE_CHANGE_FAILURE;
	}
}

static void
gst_cnmh264dec_base_init (gpointer klass)
{
	gcnm_printf("enter %s\n", __func__);

	static GstElementDetails h264dec_details = {
		"IPPCNM decoder H264",
		"Codec/Decoder/Video",
		"HARDWARE H264 Decoder based-on IPP CNM CODEC",
		""
	};

	GstElementClass *element_class = GST_ELEMENT_CLASS (klass);

	gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&src_factory));
	gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&sink_factory));
	gst_element_class_set_details (element_class, &h264dec_details);
}

static void
gst_cnmh264dec_finalize(GObject * object)
{
	Gstcnmh264dec* h264 = GST_CNMH264DEC(object);
	if(h264->pProbeData) {
		g_free(h264->pProbeData);
	}

	g_mutex_free(h264->cnmCodecMtx);

	g_mutex_free(h264->ElementState.elestate_mtx);

	GST_DEBUG_OBJECT(h264, "Gstcnmh264dec instance(%p) parent class is finalizing, codec err 0x%x, mem err %d.", h264, h264->iCodecErr, h264->iMemErr);
	G_OBJECT_CLASS(parent_class)->finalize(object);

	gcnm_printf("Gstcnmh264dec instance 0x%x is finalized!!!\n", (unsigned int)object);
	printf("Gstcnmh264dec instance (0x%x) is finalized\n", (unsigned int)object);

	return;
}


static void
gst_cnmh264dec_class_init (Gstcnmh264decClass *klass)
{
	gcnm_printf("enter %s\n", __func__);

	GObjectClass *gobject_class  = (GObjectClass*) klass;
	GstElementClass *gstelement_class = (GstElementClass*) klass;

	gobject_class->set_property = gst_cnmh264dec_set_property;
	gobject_class->get_property = gst_cnmh264dec_get_property;

	g_object_class_install_property (gobject_class, ARG_NOTREORDERBASELINE, \
		g_param_spec_int ("notreorder-baseline", "do not reorder for baseline h264", \
		"Not do auto reorder for baseline h264. It could be 1<not do reorder for baseline stream> or 0<do reorder>.", \
		0/* range_MIN */, 1/* range_MAX */, 1/* default_INIT */, G_PARAM_READWRITE));

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
		g_param_spec_int ("totalframes", "Totalframe of cnm h264 decoder outputted",
			"Number of frame of cnm h264 decoder outputted", 0, G_MAXINT, 0, G_PARAM_READABLE));
	g_object_class_install_property (gobject_class, ARG_CODECTIME,
		g_param_spec_int64 ("codectime", "cnm h264 decode time",
			"Pure codec time of calling cnm h264 decoding API(microsecond)", 0, G_MAXINT64, 0, G_PARAM_READABLE));
#endif

	gobject_class->finalize = GST_DEBUG_FUNCPTR (gst_cnmh264dec_finalize);
	gstelement_class->change_state = GST_DEBUG_FUNCPTR (gst_cnmh264dec_change_state);
	GST_DEBUG_CATEGORY_INIT(cnmdec_h264_debug, "cnmdec_h264", 0, "CNM h264 Decoder Element");
	return;
}

static void gst_cnmh264dec_init_members(Gstcnmh264dec* h264)
{
	gcnm_printf("gst_cnmh264dec_init_members is called\n");

	h264->pProbeData = g_malloc0(sizeof(CnmH264ProbeData));

	h264->pH264DecoderState = NULL;
	h264->num = 0;
	h264->den = 1;
	h264->bCodecSucLaunched = 0;
	h264->b1stFrame_forFileplay = 1;
	h264->iCodecErr = 0;
	h264->iMemErr = 0;
	h264->is_avcC = FALSE;
	h264->nalLenFieldSize = 0;

	memset(&h264->SeqInfo, 0, sizeof(h264->SeqInfo));
	memset(h264->DecFramesInfoArr, 0, sizeof(h264->DecFramesInfoArr));
	h264->iDecFrameBufArrLen = 0;
	memset(&h264->FilePlay_StmBuf, 0, sizeof(h264->FilePlay_StmBuf));
	memset(&h264->DecOutInfo, 0, sizeof(h264->DecOutInfo));
	memset(&h264->ShareBufManager, 0, sizeof(h264->ShareBufManager));

	h264->ElementState.state = CNMELESTATE_RESTING;
	h264->ElementState.elestate_mtx = g_mutex_new();

	h264->seqMeta = NULL;
	h264->seqMetaLen = 0;
	h264->seqMetaBufSz = 0;
	memset(h264->StmBufPool, 0, sizeof(h264->StmBufPool));
	memset(&h264->DownFrameLayout, 0, sizeof(h264->DownFrameLayout));
	gcnmdec_init_TsDuMgr(&h264->TsDuManager);

	h264->bSPSReceivedForNonAVCC = 0;

	h264->bNotReorderForBaseline = 1;
	h264->iI420LayoutKind = 0;
	h264->iShareFrameBuf = 1;
	h264->iTimeoutForDisp1Frame = WAIT_DOWNSTREAM_RETBUF_TIMEOUT;
	h264->iSupportMultiIns = 1;
	h264->bEnableHibernateAtPause = 1;
	h264->bEnableDynaClk = 1;
	h264->bNewSegReceivedAfterflush = 0;

	h264->cnmCodecMtx = g_mutex_new();
	h264->bCnmInDream = 0;

	h264->totalFrames = 0;
	h264->codec_time = 0;
	return;
}

static void
gst_cnmh264dec_init(Gstcnmh264dec *h264, Gstcnmh264decClass *h264_klass)
{
	gcnm_printf("enter %s\n", __func__);

	GstElementClass *klass = GST_ELEMENT_CLASS(h264_klass);

	h264->sinkpad = gst_pad_new_from_template (
		gst_element_class_get_pad_template(klass, "sink"), "sink");

	gst_pad_set_setcaps_function (h264->sinkpad, GST_DEBUG_FUNCPTR (gst_cnmh264dec_sinkpad_setcaps));
	gst_pad_set_chain_function (h264->sinkpad, GST_DEBUG_FUNCPTR (gst_cnmh264dec_chain));
	gst_pad_set_event_function (h264->sinkpad, GST_DEBUG_FUNCPTR (gst_cnmh264dec_sinkpad_event));

	gst_element_add_pad(GST_ELEMENT(h264), h264->sinkpad);

	h264->srcpad = gst_pad_new_from_template (
		gst_element_class_get_pad_template(klass, "src"), "src");

	gst_element_add_pad(GST_ELEMENT(h264), h264->srcpad);

	gst_cnmh264dec_init_members(h264);

	gcnm_printf("Gstcnmh264dec instance 0x%x is inited!!!\n", (unsigned int)h264);
	printf("Gstcnmh264dec instance (0x%x) is inited\n", (unsigned int)h264);

	return;
}

/* EOF */
