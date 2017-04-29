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

/* gstcnmdec_mpeg2.c */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h> //to include memset(),...
#include <sys/time.h>	//to include time functions and structures

#include "gstcnmdec_mpeg2.h"
#include "cnm_bridge.h"
#include "ippGSTdefs.h"


//#define LOG_FRAMECOPYTIME
//#define LOG_TIME_NOCODEC_NOPUSH
//#define LOG_TIME_STMCOPY
typedef struct{
	struct timeval tstart;
	struct timeval tend;
	struct timezone tz;
	struct timeval tlog_0;
	struct timeval tlog_1;
	unsigned int log_copytime;
	unsigned int log_copycnt;
#ifdef LOG_TIME_NOCODEC_NOPUSH
	struct timeval t_0;
	struct timeval t_1;
	struct timeval t_2;
	struct timeval t_3;
	long long tim_nodec_nopush;
#endif
#ifdef LOG_TIME_STMCOPY
	struct timeval t_cpystm0;
	struct timeval t_cpystm1;
	long long tim_cpystm;
#endif
}CnmMPEG2ProbeData;

#define GSTCNMMPEG2_ACTIVEBUF_MAXSIZE   	(3*1024*1024)   //max size of active buffer
#define GSTCNMMPEG2_TS_MAXCOUNT 			10  			//max capacity of TS manager
#define GNM_TSABSSUB(x, y)  ( (x)>(y)? (x)-(y) : (y)-(x) )

GST_DEBUG_CATEGORY_STATIC (cnmdec_mpeg2_debug);
#define GST_CAT_DEFAULT cnmdec_mpeg2_debug

enum {

	/* fill above with user defined signals */
	LAST_SIGNAL
};

enum {
	ARG_0,
	/* fill below with user defined arguments */
	ARG_I420LAYOUTKIND,
	ARG_IGN_PBBEFOREI,
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

static GstStaticPadTemplate sink_factory =
	GST_STATIC_PAD_TEMPLATE ("sink", GST_PAD_SINK, GST_PAD_ALWAYS,
	GST_STATIC_CAPS ("video/mpeg, "
					 "mpegversion = (int) 2, "
					 "systemstream = (boolean) FALSE, "
					 "width = (int) [ 16, 1280 ], "
					 "height = (int) [ 16, 1022 ], "
					 "framerate = (fraction) [ 0, MAX ], "
					 "cnmnotsupport = (int) 0")
					 );

static GstStaticPadTemplate src_factory =
	GST_STATIC_PAD_TEMPLATE ("src", GST_PAD_SRC, GST_PAD_ALWAYS,
	GST_STATIC_CAPS ("video/x-raw-yuv,"
					 "format = (fourcc) { I420 }, "
					 "width = (int) [ 16, 1280 ], "
					 "height = (int) [ 16, 1022 ], "
					 "framerate = (fraction) [ 0, MAX ]")
					 );


GST_BOILERPLATE (Gstcnmmpeg2dec, gst_cnmmpeg2dec, GstElement, GST_TYPE_ELEMENT);


static gboolean
gst_cnmmpeg2dec_sinkpad_setcaps (GstPad *pad, GstCaps *caps)
{
	gcnm_printf("enter gst_cnmmpeg2dec_sinkpad_setcaps().\n");

	//Gstcnmmpeg2dec *mpeg2 = GST_CNMMPEG2DEC (GST_OBJECT_PARENT (pad));
	int i = 0;
	GstStructure *str;
	//const GValue *value;

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
	//str = gst_caps_get_structure(caps, 0);

	gcnm_printf("exit gst_cnmmpeg2dec_sinkpad_setcaps() succeed\n");
	return TRUE;
}

static void
gst_cnmmpeg2dec_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{

	Gstcnmmpeg2dec *mpeg2 = GST_CNMMPEG2DEC (object);

	switch (prop_id)
	{
	case ARG_I420LAYOUTKIND:
		{
			int layout = g_value_get_int(value);
			if(layout < 0 || layout > 2) {
				GST_ERROR_OBJECT(mpeg2, "I420layoutkind %d exceed range.", layout);
				layout = 0;
			}
			mpeg2->iI420LayoutKind = layout;
		}
		break;
	case ARG_IGN_PBBEFOREI:
		{
			int ignore = g_value_get_int(value);
			if(ignore < 0 || ignore > 1) {
				GST_ERROR_OBJECT(mpeg2, "ignorePBbeforeI %d exceed range.", ignore);
				ignore = 1;
			}
			mpeg2->iIgnorePBBeforeI = ignore;
		}
		break;
	case ARG_SHAREFRAMEBUF:
		{
			int sha = g_value_get_int(value);
			if(sha < 0 || sha > 1) {
				GST_ERROR_OBJECT(mpeg2, "shareFramebuf %d exceed range.", sha);
				sha = 1;
			}
			mpeg2->iShareFrameBuf = sha;
		}
		break;
	case ARG_TIMEOUTFORDISP1FRAME:
		{
			int to = g_value_get_int(value);
			if(to < -1) {
				GST_ERROR_OBJECT(mpeg2, "timeoutDisp1frame %d exceed range.", to);
				to = -1;
			}
			mpeg2->iTimeoutForDisp1Frame = to;
		}
		break;
	case ARG_SUPPORTMULTIINS:
		{
			int supm = g_value_get_int(value);
			if(supm < 0 || supm > 2) {
				GST_ERROR_OBJECT(mpeg2, "supportMultiIns %d exceed range.", supm);
				supm = 0;
			}
			mpeg2->iSupportMultiIns = supm;
		}
		break;
	case ARG_ENABLEHIBERNATEATPAUSE:
		{
			int eh = g_value_get_int(value);
			if(eh < 0 || eh > 1) {
				GST_ERROR_OBJECT(mpeg2, "cnmHibernateAtPause %d exceed range.", eh);
				eh = 0;
			}
			mpeg2->bEnableHibernateAtPause = eh;
		}
		break;
	case ARG_ENABLEDYNACLK:
		{
			int ec = g_value_get_int(value);
			if(ec < 0 || ec > 1) {
				GST_ERROR_OBJECT(mpeg2, "dynamicClock %d exceed range.", ec);
				ec = 0;
			}
			mpeg2->bEnableDynaClk = ec;
		}
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
	return;
}


static void
gst_cnmmpeg2dec_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	Gstcnmmpeg2dec *mpeg2 = GST_CNMMPEG2DEC (object);

	switch (prop_id)
	{
	case ARG_I420LAYOUTKIND:
		g_value_set_int(value, mpeg2->iI420LayoutKind);
		break;

	case ARG_IGN_PBBEFOREI:
		g_value_set_int(value, mpeg2->iIgnorePBBeforeI);
		break;
	case ARG_SHAREFRAMEBUF:
		g_value_set_int(value, mpeg2->iShareFrameBuf);
		break;
	case ARG_TIMEOUTFORDISP1FRAME:
		g_value_set_int(value, mpeg2->iTimeoutForDisp1Frame);
		break;
	case ARG_SUPPORTMULTIINS:
		g_value_set_int(value, mpeg2->iSupportMultiIns);
		break;
	case ARG_ENABLEHIBERNATEATPAUSE:
		g_value_set_int(value, mpeg2->bEnableHibernateAtPause);
		break;
	case ARG_ENABLEDYNACLK:
		g_value_set_int(value, mpeg2->bEnableDynaClk);
		break;

#ifdef DEBUG_PERFORMANCE
	case ARG_TOTALFRAME:
		g_value_set_int(value, mpeg2->totalFrames);
		break;
	case ARG_CODECTIME:
		g_value_set_int64(value, mpeg2->codec_time);
		break;
#endif

	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
	return;
}

static __inline void _cnmmpeg2_refresh_downpush_TsDu(Gstcnmmpeg2dec* mpeg2)
{
	int delay = mpeg2->SeqInfo.MaxReorderDelay+1;
	gcnmdec_remove_redundant_TsDu(&mpeg2->TsDuManager, delay+1);	//there should be 2+MaxReorderDelay TS in queue, one is for output frame, the 2nd is for data parsing(only after receiving the next frame begin data, we could confirm the current frame's ending), the others is for CNM reorder delay
	gcnmdec_refresh_output_TsDu(&mpeg2->TsDuManager);

	//some .mpg file which contain mpeg2 video are not well muxed, for those file, demuxer always provide TS as GST_CLOCK_TIME_NONE. Therefore, do workaround here, adopt segment start time as first TS.
	if(mpeg2->TsDuManager.LastOutputTs == GST_CLOCK_TIME_NONE && mpeg2->TsDuManager.SegmentStartTs != -1) {
		mpeg2->TsDuManager.LastOutputTs = mpeg2->TsDuManager.SegmentStartTs;
		gcnm_printf("Adopt segment start time as first TS %lld\n", mpeg2->TsDuManager.SegmentStartTs);
	}
	return;
}

static GstFlowReturn _cnmmpeg2_push_data(Gstcnmmpeg2dec *mpeg2, int iDecOutFrameIdx);
static __inline unsigned char* seek_MPEG2SC(unsigned char* pData, int len);
static int query_mpeg2OnePicture(unsigned char* pData, int len, unsigned char** ppPictureBeginPos, unsigned char** ppPictureEndPos, int* pPicTypeStructure, unsigned char** ppMistakeStopPos);
static int query_mpeg2Oneframe(unsigned char* pData, int len, unsigned char** pp1stPictureBeginPos, unsigned char** ppFrameEndPos, unsigned char** ppMistakeStopPos, int* bIFrame);
static int query_mpeg2SeqHdr1stFrame(unsigned char* pData, int len, unsigned char** ppSeqHdrBeginPos, unsigned char** pp1stFrameEndPos, unsigned char** ppMistakeStopPos);


static int codec_decoding_1frame(Gstcnmmpeg2dec *mpeg2, GCNMDec_StmBuf* pCnmInputBuf, int bStopWhenAbnormal, GstFlowReturn* pFlowRt)
{
	int bCallDecAgain;
	int ret = 0;
	int exitloop = 0;
	IppCodecStatus codecRt;
	int cnmDreamStatusAtlock;

	*pFlowRt = GST_FLOW_OK;
	mpeg2->FilePlay_StmBuf.phyAddrBitstreamBuf = pCnmInputBuf->BufPhyAddr;
	mpeg2->FilePlay_StmBuf.DataLen = pCnmInputBuf->iDataLen;

	gcnm_printfmore("start DecodeFrame_CMVpu for one frame, data len %d\n", mpeg2->FilePlay_StmBuf.DataLen);

	do{
		bCallDecAgain = 0;

		g_mutex_lock(mpeg2->cnmCodecMtx);
		cnmDreamStatusAtlock = mpeg2->bCnmInDream;
		if(mpeg2->bCnmInDream) {
			HibernateWakeupCNM(mpeg2->pMPEG2DecoderState, 0, &mpeg2->bCnmInDream);
		}

		if(0 != gcnmdec_informcodec_decframeconsumed(mpeg2->pMPEG2DecoderState, &mpeg2->ShareBufManager)) {
			mpeg2->iCodecErr |= ERR_CNMDEC_DISPLAYEDAPI;
			ret = -5;
			*pFlowRt = GST_FLOW_ERROR;
			g_mutex_unlock(mpeg2->cnmCodecMtx);
			gcnm_printf("gcnmdec_informcodec_decframeconsumed() fail, line %d\n", __LINE__);
			return ret;
		}

#ifdef DEBUG_PERFORMANCE
		gettimeofday(&((CnmMPEG2ProbeData*)mpeg2->pProbeData)->tstart, &((CnmMPEG2ProbeData*)mpeg2->pProbeData)->tz);
#endif
		codecRt = DecodeFrame_CMVpu(mpeg2->pMPEG2DecoderState, &mpeg2->FilePlay_StmBuf, &mpeg2->DecOutInfo, 0);
#ifdef DEBUG_PERFORMANCE
		gettimeofday(&((CnmMPEG2ProbeData*)mpeg2->pProbeData)->tend, &((CnmMPEG2ProbeData*)mpeg2->pProbeData)->tz);
		mpeg2->codec_time += (((CnmMPEG2ProbeData*)mpeg2->pProbeData)->tend.tv_sec - ((CnmMPEG2ProbeData*)mpeg2->pProbeData)->tstart.tv_sec)*1000000 + (((CnmMPEG2ProbeData*)mpeg2->pProbeData)->tend.tv_usec - ((CnmMPEG2ProbeData*)mpeg2->pProbeData)->tstart.tv_usec);
#endif
		if(! mpeg2->iShareFrameBuf && (codecRt == IPP_STATUS_OUTPUT_DATA || codecRt == IPP_STATUS_FRAME_COMPLETE)) {
			(mpeg2->ShareBufManager.DecOutFrameConsumeStatus[mpeg2->DecOutInfo.idxFrameOutput])++;//In theory, under non-iShareFrameBuf mode, should after copy the frame data, then set the status that the frame is consumed. Anyway, setting it in advance is no side effect.
		}
		if(cnmDreamStatusAtlock) {
			HibernateWakeupCNM(mpeg2->pMPEG2DecoderState, 1, &mpeg2->bCnmInDream);
		}
		g_mutex_unlock(mpeg2->cnmCodecMtx);

		gcnm_printfmore("return from DecodeFrame_CMVpu(%d), DecOutInfo.idxFrameOutput[%d], DecOutInfo.NumOfErrMbs[%d]\n", codecRt, mpeg2->DecOutInfo.idxFrameOutput, mpeg2->DecOutInfo.NumOfErrMbs);

		if(!(codecRt == IPP_STATUS_OUTPUT_DATA || (codecRt==IPP_STATUS_BUFFER_FULL && mpeg2->iShareFrameBuf))) {
			//the data has been consumed
			pCnmInputBuf->iDataLen = 0;
		}

		//record the decoded frame type
		if(mpeg2->DecOutInfo.idxFrameDecoded >= 0 && mpeg2->DecOutInfo.idxFrameDecoded < mpeg2->iDecFrameBufArrLen) {
			int tmp = (int)((mpeg2->DecFramesInfoArr[mpeg2->DecOutInfo.idxFrameDecoded]).pUserData0);
			(mpeg2->DecFramesInfoArr[mpeg2->DecOutInfo.idxFrameDecoded]).pUserData0 = (void*)((mpeg2->DecOutInfo.FrameDecodedType << 16) | (tmp & 0xffff));
		}

		switch(codecRt) {
		case IPP_STATUS_OUTPUT_DATA:
			//No frame is decoded, because the input frame data hasn't been consumed by VPU, therefore, application should feed this frame again.
			//But some frame still need output, which are reorder delayed frames.
			bCallDecAgain = 1;
		case IPP_STATUS_FRAME_COMPLETE:
			gcnm_printfmore("DecodeFrame_CMVpu ret IPP_STATUS_FRAME_COMPLETE, %d, %d, %d\n", mpeg2->totalFrames, mpeg2->DecOutInfo.idxFrameOutput, mpeg2->DecOutInfo.NumOfErrMbs);
			mpeg2->totalFrames++;
			if(mpeg2->iIgnLastSegDelayedFrames > 0) {
				gcnm_printf("Ignore the frame belong to last segment %d\n", mpeg2->iIgnLastSegDelayedFrames);
				mpeg2->iIgnLastSegDelayedFrames--;	//those frames are belong to last segment and won't consume current segment's TS and needn't to be pushed to sink
				if(mpeg2->iShareFrameBuf) {
					g_mutex_lock(mpeg2->ShareBufManager.mtx);
					(mpeg2->ShareBufManager.DecOutFrameConsumeStatus[mpeg2->DecOutInfo.idxFrameOutput])++;
					g_mutex_unlock(mpeg2->ShareBufManager.mtx);
				}
				*pFlowRt = GST_FLOW_OK;
			}else{
				_cnmmpeg2_refresh_downpush_TsDu(mpeg2);
				*pFlowRt = _cnmmpeg2_push_data(mpeg2, mpeg2->DecOutInfo.idxFrameOutput);
				if(*pFlowRt == GST_FLOW_ERROR && bStopWhenAbnormal) {
					gcnm_printf("_cnmmpeg2_push_data() fail, line %d\n", __LINE__);
					ret = -1;
					exitloop = 1;
				}
			}
			break;
		case IPP_STATUS_NOERR:
			gcnm_printf("DecodeFrame_CMVpu ret NOERR, %d %d %d\n", mpeg2->DecOutInfo.OneFrameUnbrokenDecoded, mpeg2->DecOutInfo.idxFrameOutput, mpeg2->DecOutInfo.idxFrameDecoded);
			break;
		case IPP_STATUS_NOTSUPPORTED_ERR:
			mpeg2->iCodecErr |= WARN_CNMDEC_FRAMENOTSUPPORT;
			if(mpeg2->iIgnLastSegDelayedFrames > 0) {
				gcnm_printf("Ignore the frame belong to last segment %d at returing frame-unsupport.\n", mpeg2->iIgnLastSegDelayedFrames);
				mpeg2->iIgnLastSegDelayedFrames--;	//those frames are belong to last segment and won't consume current segment's TS and needn't to be pushed to sink
			}else{
				_cnmmpeg2_refresh_downpush_TsDu(mpeg2);
			}
			gcnm_printf("DecodeFrame_CMVpu ret IPP_STATUS_NOTSUPPORTED_ERR.\n");
			if(bStopWhenAbnormal) {
				ret = -2;
				exitloop = 1;
			}
			break;
		case IPP_STATUS_FRAME_ERR:
			mpeg2->iCodecErr |= WARN_CNMDEC_FRAMECORRUPT;
			gcnm_printf("DecodeFrame_CMVpu ret WARN_CNMDEC_FRAMECORRUPT, probably the frame is corrupt.\n");
			if(bStopWhenAbnormal) {
				ret = -3;
				exitloop = 1;
			}
			break;
		case IPP_STATUS_BUFFER_FULL:
			gcnm_printf("Warning: DecodeFrame_CMVpu ret buffer full, waiting sink release buffer\n");
#ifndef NOTECHO_BUFFERFULL_WARNING
			g_warning("Warning: DecodeFrame_CMVpu ret buffer full, waiting sink release buffer\n");
#endif
			if(mpeg2->iShareFrameBuf) {
				int waitRt;
				bCallDecAgain = 1;
				waitRt = wait_sink_retDecFrameBuf(mpeg2->iTimeoutForDisp1Frame, -1, &mpeg2->ShareBufManager, &mpeg2->ElementState);
				if(waitRt > 0) {
					gcnm_printf("wait_sink_retDecFrameBuf() stopped by element state change\n");
					ret = 1;
					exitloop = 1;	//element isn't running, give up decoding
					//give up this frame, since we have limitted the length of TS queue, little need to remove the TS of this frame from TS queue
					pCnmInputBuf->iDataLen = 0;
				}else if(waitRt < 0) {
					//wait time out or some error occur
					ret = -6;
					exitloop = 1;
					mpeg2->iCodecErr |= (waitRt == -2 ? ERR_WAITSINKRETFRAME_SINKNOFRAME: ERR_WAITSINKRETFRAME_TIMEOUT);
					*pFlowRt = GST_FLOW_ERROR;
				}
				break;  //break switch
			}
		default:
			mpeg2->iCodecErr |= ERR_CNMDEC_DECFRAME_SERIOUSERR;
			gcnm_printf("Err: DecodeFrame_CMVpu fail, ret %d\n", codecRt);
			g_warning("Err: DecodeFrame_CMVpu fail, ret %d", codecRt);
			ret = -4;
			exitloop = 1;
			*pFlowRt = GST_FLOW_ERROR;
		}
	}while(bCallDecAgain == 1 && exitloop == 0);

	return ret;
}


#define WANT_FRAMEBEGIN		0
#define WANT_1PICHDR		1
#define WANT_1PICEXT		2
#define WANT_2PICHDR		3
#define WANT_2PICEXT		4
#define WANT_FRAMEEND		5

#define MPEG2SCID_SEQHDR	0xB3
#define MPEG2SCID_GOPHDR	0xB8
#define MPEG2SCID_PICHDR	0x00
#define MPEG2SCID_EXT		0xB5
#define MPEG2SCID_USER		0xB2
#define MPEG2SCID_SEQEND	0xB7
#define MPEG2SCID_SLICEMIN	0x01
#define MPEG2SCID_SLICEMAX	0xAF

#define MPEG2_PICSTUCT_RESERVED	0
#define MPEG2_PICSTUCT_TOPFIELD	1
#define MPEG2_PICSTUCT_BOTFIELD	2
#define MPEG2_PICSTUCT_FRAME	3

#define MPEG2_PICTYPE_I			1
#define MPEG2_PICTYPE_P			2
#define MPEG2_PICTYPE_B			3

static __inline void
reset_parserbuf(Gstcnmmpeg2dec* mpeg2)
{
	mpeg2->stm_parser.databuf.pWriter = mpeg2->stm_parser.databuf.pParsed = mpeg2->stm_parser.databuf.pBuf;
	mpeg2->stm_parser.wantstate = WANT_FRAMEBEGIN;
	mpeg2->stm_parser.pFrameBeg = NULL;
	return;
}

static __inline void
detach_parserbuf(Gstcnmmpeg2dec* mpeg2)
{
	mpeg2->stm_parser.databuf.pBuf = NULL;
	return;
}

static gboolean
gst_cnmmpeg2dec_sinkpad_event (GstPad *pad, GstEvent *event)
{
	gcnm_printf("enter %s, event 0x%x\n", __func__, GST_EVENT_TYPE(event));

	gboolean eventRet;
	Gstcnmmpeg2dec *mpeg2 = GST_CNMMPEG2DEC(GST_OBJECT_PARENT(pad));

	switch(GST_EVENT_TYPE (event))
	{
	case GST_EVENT_NEWSEGMENT:
		gcnm_printf("NewSeg event is received.\n");
		{
			int i;
			gboolean update;
			gdouble rate;
			GstFormat format;
			gint64 start, stop, position;

			gcnmdec_remove_redundant_TsDu(&mpeg2->TsDuManager, 0);
			mpeg2->TsDuManager.LastOutputTs = GST_CLOCK_TIME_NONE;
			mpeg2->TsDuManager.LastOutputDu = GST_CLOCK_TIME_NONE;
			mpeg2->last_inTs = GST_CLOCK_TIME_NONE;
			mpeg2->TsDuManager.SegmentStartTs = -1;

			mpeg2->iActiveDataLen = 0;
			reset_parserbuf(mpeg2);
			mpeg2->bLetDataBeginFromBorder = 1;
			for(i=0; i<mpeg2->iDecFrameBufArrLen; i++) {
				//clear frame type information
				mpeg2->DecFramesInfoArr[i].pUserData0 = (void*)(0xffff0000 | (unsigned int)(mpeg2->DecFramesInfoArr[i].pUserData0));	//bit[31:16]=0xffff, it means unknow frame type; bit[31:16]=0, I frame; bit[31:16]=1, P frame; bit[31:16]=2, B frame
			}
			if(mpeg2->iIgnorePBBeforeI) {
				mpeg2->bNotOutputPBframe = 1;
				mpeg2->bNotInputPBframe = 1;
			}else{
				mpeg2->bNotOutputPBframe = 0;
				mpeg2->bNotInputPBframe = 0;
			}
			if(mpeg2->pMPEG2DecoderState && mpeg2->SeqInfo.MaxReorderDelay > 0) {
				mpeg2->iIgnLastSegDelayedFrames = mpeg2->SeqInfo.MaxReorderDelay;
			}else{
				mpeg2->iIgnLastSegDelayedFrames = 0;
			}
			gcnm_printf("At newseg, set iIgnLastSegDelayedFrames to %d.\n", mpeg2->iIgnLastSegDelayedFrames);

			gst_event_parse_new_segment(event, &update, &rate, &format, &start, &stop, &position);
			if(format == GST_FORMAT_TIME) {
				gcnm_printf("At newseg, start %lld ns, stop %lld ns\n", start, stop);
				if(start != -1) {
					if(start >= 0) {
						mpeg2->TsDuManager.SegmentStartTs = start;
					}
				}
			}
			
			eventRet = gst_pad_push_event(mpeg2->srcpad, event);
			mpeg2->bNewSegReceivedAfterflush = 1;
		}
		break;
	case GST_EVENT_EOS:
		gcnm_printf("EOS event is received.\n");
		{
			IppCodecStatus codecRt;
			GstFlowReturn flowRt;
			GCNMDec_StmBuf* pCnmInputBuf;
			int InDreamStatusAtLock;

			if(mpeg2->bCodecSucLaunched && ! IS_CNMDEC_FATALERR(mpeg2->iCodecErr)) {
				int bNotDecoding_inEOS = 0;
#ifndef IPPGST_QUICKPARSER
				unsigned char* pPic0Begin=NULL, *pFrameEnd=NULL;
				unsigned char* pStreamMistakeEndPos = NULL;
				int bIFrame;
				int query_rt;
#endif
				int fill_len, tmp;

				//check whether one frame is received
#ifndef IPPGST_QUICKPARSER
				query_rt = query_mpeg2Oneframe(mpeg2->pActiveBuf, mpeg2->iActiveDataLen, &pPic0Begin, &pFrameEnd, &pStreamMistakeEndPos, &bIFrame);
				if(query_rt == 0 || (query_rt == 1 && pStreamMistakeEndPos == NULL)) {
					if(query_rt == 0) {
						fill_len = pFrameEnd - mpeg2->pActiveBuf;
					}else{
						fill_len = mpeg2->iActiveDataLen;
					}
#else
				if(mpeg2->stm_parser.wantstate == WANT_FRAMEEND) {
					fill_len = mpeg2->stm_parser.databuf.pWriter - mpeg2->stm_parser.pFrameBeg;
#endif

					tmp = gcnmdec_select_stream(mpeg2->StmBufPool, GSTCNMDEC_MPEG2_STMPOOLSZ, fill_len);
					if( tmp < 0 || tmp >= GSTCNMDEC_MPEG2_STMPOOLSZ ) {
						mpeg2->iMemErr = -3;
						gcnm_printf("Err: gcnmdec_select_stream fail in EOS, ret %d", tmp);
						GST_WARNING_OBJECT(mpeg2, "Err: gcnmdec_select_stream fail in EOS, ret %d", tmp);
					}else{
						pCnmInputBuf = &(mpeg2->StmBufPool[tmp]);
#ifndef IPPGST_QUICKPARSER
						gcnmdec_fill_stream(pCnmInputBuf, mpeg2->pActiveBuf, fill_len);
						mpeg2->iActiveDataLen = 0;
#else
						gcnmdec_fill_stream(pCnmInputBuf, mpeg2->stm_parser.pFrameBeg, fill_len);
#endif
						if(codec_decoding_1frame(mpeg2, pCnmInputBuf, 1, &flowRt) != 0) {
							bNotDecoding_inEOS = 1;
						}
					}

				}

				if(bNotDecoding_inEOS == 0) {
					// EOS
					gcnm_printf("start DecodeFrame_CMVpu for EOS\n");
					for(;;) {

						g_mutex_lock(mpeg2->cnmCodecMtx);
						InDreamStatusAtLock = mpeg2->bCnmInDream;
						if(InDreamStatusAtLock) {
							HibernateWakeupCNM(mpeg2->pMPEG2DecoderState, 0, &mpeg2->bCnmInDream);
						}
						if(0 != gcnmdec_informcodec_decframeconsumed(mpeg2->pMPEG2DecoderState, &mpeg2->ShareBufManager)) {
							mpeg2->iCodecErr |= ERR_CNMDEC_DISPLAYEDAPI;
							g_mutex_unlock(mpeg2->cnmCodecMtx);
							break;
						}

#ifdef DEBUG_PERFORMANCE
						gettimeofday(&((CnmMPEG2ProbeData*)mpeg2->pProbeData)->tstart, &((CnmMPEG2ProbeData*)mpeg2->pProbeData)->tz);
#endif
						codecRt = DecodeFrame_CMVpu(mpeg2->pMPEG2DecoderState, NULL, &mpeg2->DecOutInfo, 1);
#ifdef DEBUG_PERFORMANCE
						gettimeofday(&((CnmMPEG2ProbeData*)mpeg2->pProbeData)->tend, &((CnmMPEG2ProbeData*)mpeg2->pProbeData)->tz);
						mpeg2->codec_time += (((CnmMPEG2ProbeData*)mpeg2->pProbeData)->tend.tv_sec - ((CnmMPEG2ProbeData*)mpeg2->pProbeData)->tstart.tv_sec)*1000000 + (((CnmMPEG2ProbeData*)mpeg2->pProbeData)->tend.tv_usec - ((CnmMPEG2ProbeData*)mpeg2->pProbeData)->tstart.tv_usec);
#endif
						
						if(!mpeg2->iShareFrameBuf && codecRt == IPP_STATUS_FRAME_COMPLETE) {
							(mpeg2->ShareBufManager.DecOutFrameConsumeStatus[mpeg2->DecOutInfo.idxFrameOutput])++;
						}
						if(InDreamStatusAtLock) {
							HibernateWakeupCNM(mpeg2->pMPEG2DecoderState, 1, &mpeg2->bCnmInDream);
						}
						g_mutex_unlock(mpeg2->cnmCodecMtx);

						if(codecRt == IPP_STATUS_FRAME_COMPLETE) {
							gcnm_printf("DecodeFrame_CMVpu ret IPP_STATUS_FRAME_COMPLETE at EOS, %d\n", mpeg2->DecOutInfo.idxFrameOutput);
							mpeg2->totalFrames++;
							_cnmmpeg2_refresh_downpush_TsDu(mpeg2);
							flowRt = _cnmmpeg2_push_data(mpeg2, mpeg2->DecOutInfo.idxFrameOutput);
							if(flowRt == GST_FLOW_ERROR) {
								break;
							}
						}else if(codecRt == IPP_STATUS_BS_END) {
							gcnm_printf("DecodeFrame_CMVpu ret IPP_STATUS_BS_END at EOS\n");
							break;
						}else{
							mpeg2->iCodecErr |= WARN_CNMDEC_EOS;
							g_warning("DecodeFrame_CMVpu() in EOS fail, ret %d", codecRt);
							break;
						}
					}
				}
			}
			//once DecodeFrame_CMVpu return IPP_STATUS_BS_END, the codec couldn't accept new data. If application want to decoding continue(for example, seek to begin, and play again), need to close and init a new codec
			g_mutex_lock(mpeg2->cnmCodecMtx);
			if(mpeg2->pMPEG2DecoderState) {
				if(mpeg2->bCnmInDream) {
					HibernateWakeupCNM(mpeg2->pMPEG2DecoderState, 0, &mpeg2->bCnmInDream);
				}
				mpeg2->iCodecErr |= gcnmdec_close_codec(&mpeg2->pMPEG2DecoderState, &mpeg2->ShareBufManager);
			}
			g_mutex_unlock(mpeg2->cnmCodecMtx);
			mpeg2->b1stFrame_forFileplay = 1;
			mpeg2->bSeqHdrSeeked = 0;
			mpeg2->bCodecSucLaunched = 0;
			mpeg2->iActiveDataLen = 0;
			detach_parserbuf(mpeg2);
			gcnmdec_remove_redundant_TsDu(&mpeg2->TsDuManager, 0);
			mpeg2->TsDuManager.LastOutputTs = GST_CLOCK_TIME_NONE;
			mpeg2->TsDuManager.LastOutputDu = GST_CLOCK_TIME_NONE;
			mpeg2->last_inTs = GST_CLOCK_TIME_NONE;
			eventRet = gst_pad_push_event(mpeg2->srcpad, event);
		}
		break;
	case GST_EVENT_FLUSH_STOP:
		mpeg2->bNewSegReceivedAfterflush = 0;
	default:
		eventRet = gst_pad_event_default(pad, event);
		break;
	}

	return eventRet;
}

#define GSTCNMMPEG2_IS_I_FRAME(x)   		(((int)(x) >> 16) == 0)
static GstFlowReturn _cnmmpeg2_push_data(Gstcnmmpeg2dec* mpeg2, int iDecOutFrameIdx)
{
	GstFlowReturn FlowRt = GST_FLOW_OK;
	GstBuffer *outBuf = NULL;

	gcnm_printfmore("enter %s\n", __func__);

	if(mpeg2->bNotOutputPBframe) {
		if(! GSTCNMMPEG2_IS_I_FRAME(mpeg2->DecFramesInfoArr[iDecOutFrameIdx].pUserData0)) {
			if(mpeg2->iShareFrameBuf) {
				g_mutex_lock(mpeg2->ShareBufManager.mtx);
				(mpeg2->ShareBufManager.DecOutFrameConsumeStatus[iDecOutFrameIdx])++;
				g_mutex_unlock(mpeg2->ShareBufManager.mtx);
			}
			return GST_FLOW_OK;
		}else{
			mpeg2->bNotOutputPBframe = 0;
		}
	}

	if(mpeg2->iShareFrameBuf) {
		int idxInDFBShare;
		void* VA = mpeg2->DecFramesInfoArr[iDecOutFrameIdx].pUserData1;
		unsigned int PA = (unsigned int)((CnmPContMem*)mpeg2->DecFramesInfoArr[iDecOutFrameIdx].phyY)->PAddr;
		outBuf = gst_buffer_new();
		if(outBuf == NULL) {
			return GST_FLOW_ERROR;
		}
		GST_BUFFER_DATA(outBuf) = VA;
		GST_BUFFER_SIZE(outBuf) = mpeg2->iAlignedBufSz;
		gst_buffer_set_caps(outBuf, GST_PAD_CAPS(mpeg2->srcpad));
		idxInDFBShare = ((int)(mpeg2->DecFramesInfoArr[iDecOutFrameIdx].pUserData2)>>16) & 0xff;
		IPPGST_BUFFER_CUSTOMDATA(outBuf) = (gpointer) (&mpeg2->ShareBufManager.sideinfoDFBShareArr[idxInDFBShare]);
		setSideInfo(&mpeg2->ShareBufManager.sideinfoDFBShareArr[idxInDFBShare], mpeg2->SeqInfo.FrameROI.x, mpeg2->SeqInfo.FrameROI.y, mpeg2->SeqInfo.FrameAlignedWidth, mpeg2->SeqInfo.FrameAlignedHeight, PA, VA);
		mpeg2->DecFramesInfoArr[iDecOutFrameIdx].pUserData2 = (void*)((idxInDFBShare<<16)|mpeg2->ShareBufManager.CurCodecSerialNum|(iDecOutFrameIdx<<24));
		mpeg2->DecFramesInfoArr[iDecOutFrameIdx].pUserData3 = (void*)(&mpeg2->ShareBufManager);
		GST_BUFFER_MALLOCDATA(outBuf) = (guint8*)(&(mpeg2->DecFramesInfoArr[iDecOutFrameIdx]));
		GST_BUFFER_FREE_FUNC(outBuf) = (GFreeFunc)gcnmdec_downbuf_onfinalize;
		FRAMES_WATCHMAN_REF(mpeg2->ShareBufManager.watcher);

		GST_BUFFER_TIMESTAMP(outBuf) = mpeg2->TsDuManager.LastOutputTs;
		GST_BUFFER_DURATION(outBuf) = mpeg2->TsDuManager.LastOutputDu;

		g_mutex_lock(mpeg2->ShareBufManager.mtx);
		mpeg2->ShareBufManager.DFBShareArr[idxInDFBShare].pUserData3 = (void*)1;	//DFBShareArr[x].pUserData3 == 0 means display sink do not hold this frame, 1 means display sink is holding this frame.
		mpeg2->ShareBufManager.iSinkOwnedDecFrameBufCnt++;
		g_mutex_unlock(mpeg2->ShareBufManager.mtx);

		if(mpeg2->bNewSegReceivedAfterflush) {
			FlowRt = gst_pad_push(mpeg2->srcpad, outBuf);
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

	FlowRt = gcnmdec_require_framebuf_fromdownplugin(&mpeg2->DownFrameLayout, mpeg2->srcpad, &outBuf, mpeg2->iI420LayoutKind==0);
	if(GST_FLOW_OK != FlowRt) {
		if(FlowRt != GST_FLOW_WRONG_STATE) {
			//during seeking, sink element often return GST_FLOW_WRONG_STATUS. No need to echo this message
			g_warning("Require gstbuffer from downstream plugin fail, ret %d!", FlowRt);
		}
		return FlowRt;
	}
#ifdef LOG_FRAMECOPYTIME
	gettimeofday(&((CnmMPEG2ProbeData*)mpeg2->pProbeData)->tlog_0, NULL);
#endif
	gcnmdec_copy_to_downframebuf(GST_BUFFER_DATA(outBuf), &mpeg2->DownFrameLayout, &(mpeg2->DecFramesInfoArr[iDecOutFrameIdx]), &(mpeg2->SeqInfo.FrameROI));
#ifdef LOG_FRAMECOPYTIME
	gettimeofday(&((CnmMPEG2ProbeData*)mpeg2->pProbeData)->tlog_1, NULL);
	((CnmMPEG2ProbeData*)mpeg2->pProbeData)->log_copycnt++;
	((CnmMPEG2ProbeData*)mpeg2->pProbeData)->log_copytime += (((CnmMPEG2ProbeData*)mpeg2->pProbeData)->tlog_1.tv_sec - ((CnmMPEG2ProbeData*)mpeg2->pProbeData)->tlog_0.tv_sec)*1000000 + (((CnmMPEG2ProbeData*)mpeg2->pProbeData)->tlog_1.tv_usec - ((CnmMPEG2ProbeData*)mpeg2->pProbeData)->tlog_0.tv_usec);
#endif

	GST_BUFFER_TIMESTAMP(outBuf) = mpeg2->TsDuManager.LastOutputTs;
	GST_BUFFER_DURATION(outBuf) = mpeg2->TsDuManager.LastOutputDu;
	if(mpeg2->bNewSegReceivedAfterflush) {
		FlowRt = gst_pad_push(mpeg2->srcpad, outBuf);
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



static int _cnmmpeg2dec_allocateframebufs(void* pElement, IppCMVpuDecParsedSeqInfo* pSeqInfo, int* piFrameCnt, IppCMVpuDecFrameAddrInfo** pFrameArr, int* piFrameStride)
{
	int codecframeNeedCnt;
	Gstcnmmpeg2dec* ele = (Gstcnmmpeg2dec*)pElement;
	int ret;
	int bCacheable;
	int framebufsz = (pSeqInfo->FrameAlignedWidth * pSeqInfo->FrameAlignedHeight * 3)>>1;

	*piFrameStride = pSeqInfo->FrameAlignedWidth;
	*pFrameArr = ele->DecFramesInfoArr;

	if(ele->iShareFrameBuf) {
		bCacheable = 0;
		codecframeNeedCnt = gcnmdec_smartdecide_framealloc_cnt(pSeqInfo->MinFrameBufferCount, IPPCMDEC_DECFRAMEBUF_MAX_H263MPEG2MPEG4, IPPGST_CMVPUDEC_FRAMEBUF_DEFAULT_MAXTOTALSIZE, framebufsz, IPPGSTCNMDEC_SPEC_MPEG2);
	}else{
		bCacheable = 0;
		codecframeNeedCnt = pSeqInfo->MinFrameBufferCount;
	}

	ret = prepare_framebufforcodec(pSeqInfo->FrameAlignedHeight, pSeqInfo->FrameAlignedWidth, piFrameCnt, codecframeNeedCnt, &ele->ShareBufManager, ele->DecFramesInfoArr, &ele->iDecFrameBufArrLen, ele->iShareFrameBuf, bCacheable);

	return ret;
}


static __inline unsigned char* seek_MPEG2SC(unsigned char* pData, int len)
{
	return seek_SC_000001xx(pData, len);
}

//seek seq end code or seq header start code or gop start code or picture start code, they are the indicator of piture end and new "picture" begin
static unsigned char* seek_mpeg2picend(unsigned char* pData, int len)
{
	unsigned char* pSC = NULL;
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
	return pSC;
}

static int query_mpeg2OnePicture(unsigned char* pData, int len, unsigned char** ppPictureBeginPos, unsigned char** ppPictureEndPos, int* pPicTypeStructure, unsigned char** ppMistakeStopPos)
{
	unsigned char *pSC = NULL, *pPicBegin;
	int picture_structure, picture_coding_type;

	*ppPictureBeginPos = NULL;
	*ppPictureEndPos = NULL;

	//seek picture start code
	for(;;) {
		pSC = seek_MPEG2SC(pData, len);
		if(pSC == NULL) {
			return 2;
		}else if(pSC[3] == 0) {
			//found picture start code
			//gcnm_printfmore("func %s: find picture start code\n", __func__);
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
	picture_coding_type = (int)((unsigned int)(pPicBegin[5]) << 5) & (0x7<<8);

	*pPicTypeStructure = picture_structure | picture_coding_type;   //picture_coding_type at bit [10:8] of *pPicTypeStructure, picture_structure at bit [1:0]
	*ppPictureBeginPos = pPicBegin;

	//seek seq end code or seq header start code or gop start code or picture start code, they are the indicator of piture end
	pData += 7;
	len -= 7;
	pSC = seek_mpeg2picend(pData, len);

	if(pSC == NULL) {
		return 1;
	}else{
		*ppPictureEndPos = pSC;
		return 0;
	}
}

static int query_mpeg2Oneframe(unsigned char* pData, int len, unsigned char** pp1stPictureBeginPos, unsigned char** ppFrameEndPos, unsigned char** ppMistakeStopPos, int* bIFrame)
{
	unsigned char *pPic0BeginPos, *pPic0EndPos, *pPic1BeginPos, *pPic1EndPos;
	int picTypeStructure = 0, picture_structure, picture_coding_type0, picture_coding_type1;
	int queryrt;

	*pp1stPictureBeginPos = NULL;
	*ppFrameEndPos = NULL;
	*bIFrame = 0;

	//seek 1st picture
	queryrt = query_mpeg2OnePicture(pData, len, &pPic0BeginPos, &pPic0EndPos, &picTypeStructure, ppMistakeStopPos);
	*pp1stPictureBeginPos = pPic0BeginPos;
	picture_coding_type0 = picTypeStructure>>8;
	//gcnm_printfmore("func %s: picStructure %d\n", __func__, picStructure);
	if(queryrt != 0) {
	//gcnm_printfmore("func %s: return querey result %d\n", __func__, queryrt);
		return queryrt;
	}else{
		//found the whole picture
		picture_structure = picTypeStructure & 0x3;
		if(picture_structure == 0 || picture_structure == 3) {
			//picture_structure == 0 is forbidden by 13818-2, we consider it as frame
			*ppFrameEndPos = pPic0EndPos;
			//gcnm_printfmore("func %s: find the whole frame\n", __func__);
			*bIFrame = (picture_coding_type0==1);
			return 0;
		}
	}

	//last picture is a field picture, seek another picture
	len -= pPic0EndPos-pData;
	pData = pPic0EndPos;
	queryrt = query_mpeg2OnePicture(pData, len, &pPic1BeginPos, &pPic1EndPos, &picTypeStructure, ppMistakeStopPos);
	//gcnm_printfmore("func %s: picStructure %d, queryrt %d\n", __func__, picStructure, queryrt);
	if(queryrt == 0) {
		//gcnm_printfmore("func %s: find the whole frame\n", __func__);
		picture_structure = picTypeStructure & 0x3;
		picture_coding_type1 = picTypeStructure>>8;
		if(picture_structure == 0 || picture_structure == 3) {
			//it's a frame picture, not field picture, we consider it's a new frame. We consider last frame only contain 1 field.
			*ppFrameEndPos = pPic0EndPos;
			return 0;
		}else{
			if(picture_coding_type0==1 && picture_coding_type1 != 1 && picture_coding_type1 != 2) {
				//if I-field not followed by I-field or P-field, it's mistake
				*ppMistakeStopPos = pPic1BeginPos;
				return 2;
			}
			*ppFrameEndPos = pPic1EndPos;
			*bIFrame = (picture_coding_type0==1);	//if 1st field is I, then this frame is I frame
			return 0;
		}
	}else{
		//only found the frame begin, not found the frame end
		//gcnm_printfmore("func %s: only find the frame begin, not found the frame end, query return %d\n", __func__, queryrt);
		return 1;
	}
}

static unsigned char* seek_mpeg2seqhdrSC(unsigned char* pData, int len)
{
	unsigned char* pSeq;
	//seek seq header start code
	for(;;) {
		pSeq = seek_MPEG2SC(pData, len);
		if(pSeq == NULL) {
			return NULL;
		}else if(pSeq[3] == 0xb3) {
			return pSeq;
		}else{
			len -= pSeq+4-pData;
			pData = pSeq+4;
		}
	}
	return NULL;
}

static int query_mpeg2SeqHdr1stFrame(unsigned char* pData, int len, unsigned char** ppSeqHdrBeginPos, unsigned char** pp1stFrameEndPos, unsigned char** ppMistakeStopPos)
{
	unsigned char *pSeq, *pSeqExt, *p1stPicBeginPos, *pFrameEndPos;
	int rt, bIframe;

	*ppSeqHdrBeginPos = NULL;
	*pp1stFrameEndPos = NULL;

	//seek seq header start code
	for(;;) {
		pSeq = seek_MPEG2SC(pData, len);
		if(pSeq == NULL) {
			return 2;
		}else if(pSeq[3] == 0xb3) {
			//gcnm_printfmore("func %s: find sequence start code\n", __func__);
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
			//gcnm_printfmore("func  %s: find sequence extention start code\n", __func__);
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

	rt = query_mpeg2Oneframe(pData, len, &p1stPicBeginPos, &pFrameEndPos, ppMistakeStopPos, &bIframe);
	if(rt == 2) {
		*ppSeqHdrBeginPos = NULL;
		*pp1stFrameEndPos = NULL;
		return 2;
	}else{
		*ppSeqHdrBeginPos = pSeq;
		*pp1stFrameEndPos = pFrameEndPos;
		//gcnm_printfmore("func %s: find one seq header and frame end pos\n", __func__);
		return rt;
	}
}

static int gst_cnmmpeg2dec_launch_codec(unsigned char* pData, int len, Gstcnmmpeg2dec* mpeg2)
{
	GCNMDec_StartUpDec StartUp;
	int StartUpRet = 0;
	void* pSStmBuf = NULL;
	IppCMVpuDecInitPar InitPar;
	CnmPContMem* mem;

	memset(&InitPar, 0, sizeof(InitPar));
	InitPar.ReorderEnable = 1;
	InitPar.FilePlayEnable = 1;
	InitPar.VideoStreamFormat = IPP_VIDEO_STRM_FMT_MPG2;
	InitPar.MultiInstanceSync = mpeg2->iSupportMultiIns;
	InitPar.DynamicClkEnable = mpeg2->bEnableDynaClk;

	if(mpeg2->StmBufPool[0].iBufSize == 0) {
		int framelenUpperSz = len;
		//prepare static stream buffer
		InitPar.StaticBitstreamBufSize = 384*1024;  //init a value

		if(InitPar.StaticBitstreamBufSize < (unsigned int)(framelenUpperSz + IPPCMDEC_STATICSTMBUF_PROTECTSPACE)) {
			InitPar.StaticBitstreamBufSize = framelenUpperSz + IPPCMDEC_STATICSTMBUF_PROTECTSPACE;
		}
		InitPar.StaticBitstreamBufSize = (InitPar.StaticBitstreamBufSize + 1023) & (~1023); //cnm static stream buffer size must 1024 aligned

		gcnm_printf("allocate static stream buffer, want sz %d\n", InitPar.StaticBitstreamBufSize);
		mem = new_cnmStmBuf(InitPar.StaticBitstreamBufSize);
		if(mem == NULL) {
			g_warning("Err: allocate static stream buffer fail, want sz %d", InitPar.StaticBitstreamBufSize);
			mpeg2->iMemErr = -2;
			return -2;
		}
		pSStmBuf = mem->VAddr;
		InitPar.phyAddrStaticBitstreamBuf = mem->PAddr;
		mpeg2->StmBufPool[0].BufPhyAddr = InitPar.phyAddrStaticBitstreamBuf;
		mpeg2->StmBufPool[0].BufVirAddr = (unsigned char*)pSStmBuf;
		mpeg2->StmBufPool[0].iBufSize = InitPar.StaticBitstreamBufSize;
		mpeg2->StmBufPool[0].mem = mem;
	}else{
		//static stream buffer has been prepared
		gcnm_printf("static stream buffer already prepared, sz %d, phyaddr 0x%x.\n", mpeg2->StmBufPool[0].iBufSize, (unsigned int)(mpeg2->StmBufPool[0].BufPhyAddr));
		InitPar.StaticBitstreamBufSize = mpeg2->StmBufPool[0].iBufSize;
		InitPar.phyAddrStaticBitstreamBuf = mpeg2->StmBufPool[0].BufPhyAddr;
	}

	mpeg2->StmBufPool[0].iDataLen = 0;
	//fill seq header and 1st frame
	gcnmdec_fill_stream(&(mpeg2->StmBufPool[0]), pData, len);

	if(len<512) {
		//For mpeg2, if the data is too short (< 512 bytes), the CNM probably couldn't parse the sequence header correctly. For mpeg2, could append zero bytes at the frame beginning or end position to make the data long enough.
		unsigned char zeroarr[512] = {0};
		gcnmdec_fill_stream(&(mpeg2->StmBufPool[0]), zeroarr, 512-len);
	}

	//start up codec
	StartUp.StmBufPool = mpeg2->StmBufPool;
	StartUp.virAddrStaticBitstreamBuf = mpeg2->StmBufPool[0].BufVirAddr;
	StartUp.pInitPar = &InitPar;
	StartUp.pSeqInfo = &mpeg2->SeqInfo;
	StartUp.iFilledLen_forCodecStart = mpeg2->StmBufPool[0].iDataLen;
	StartUp.allocate_framebuf = _cnmmpeg2dec_allocateframebufs; //during start up codec, _cnmmpeg2dec_allocateframebufs() will allocate frame buffer

	//below is output parameter
	//StartUp.pCnmDec = NULL;
	//StartUp.iCodecApiErr = 0;

	mpeg2->iCodecErr = 0; //clear it before a new codec instance
	gcnm_printf("Call gcnmdec_startup_codec()...\n");

	g_mutex_lock(mpeg2->cnmCodecMtx);
	mpeg2->bCnmInDream = 0;
	StartUpRet = gcnmdec_startup_codec(mpeg2, &StartUp, &mpeg2->ShareBufManager);
	mpeg2->pMPEG2DecoderState = StartUp.pCnmDec;
	g_mutex_unlock(mpeg2->cnmCodecMtx);
	
	mpeg2->iCodecErr = StartUp.iCodecApiErr;
	mpeg2->iMemErr = StartUp.iMemoryErr;
	if(G_LIKELY(StartUpRet == 0)) {
		//succeed
		mpeg2->bCodecSucLaunched = 1;
		gcnm_printf("Launch cnm decoder succeed!\n");
	}else{
		GST_ERROR_OBJECT(mpeg2, "Launch cnm decoder fail, ret %d", StartUpRet);
	}
	return StartUpRet;

}

static int smartfill_activebuf(Gstcnmmpeg2dec* mpeg2, unsigned char* pSrc, int filllen)
{
	if(mpeg2->iActiveBufSz - mpeg2->iActiveDataLen < filllen) {
		//reallocate it
		mpeg2->pActiveBuf = g_realloc(mpeg2->pActiveBuf, mpeg2->iActiveBufSz + filllen);
		if(mpeg2->pActiveBuf == NULL) {
			g_warning("Err: reallocate pActiveBuf fail, want sz %d", mpeg2->iActiveBufSz + filllen);
			return -1;
		}
		mpeg2->iActiveBufSz = mpeg2->iActiveBufSz + filllen;
	}
	memcpy(mpeg2->pActiveBuf + mpeg2->iActiveDataLen, pSrc, filllen);
	mpeg2->iActiveDataLen += filllen;
	return 0;
}

static __inline int repack_activebuf(Gstcnmmpeg2dec* mpeg2, unsigned char* pShouldRemain)
{
	memmove(mpeg2->pActiveBuf, pShouldRemain, mpeg2->iActiveDataLen - (pShouldRemain-mpeg2->pActiveBuf));
	mpeg2->iActiveDataLen -= pShouldRemain-mpeg2->pActiveBuf;
	return 0;
}

static int lookup_activebufdata(Gstcnmmpeg2dec* mpeg2, unsigned char** ppPic0StartPos, unsigned char** ppFrameEndPos, unsigned char** ppStreamMistakeEndPos, int* pbIFrame)
{
	int ret = query_mpeg2Oneframe(mpeg2->pActiveBuf, mpeg2->iActiveDataLen, ppPic0StartPos, ppFrameEndPos, ppStreamMistakeEndPos, pbIFrame);
	if(ret != 0) {
		if(*ppStreamMistakeEndPos != NULL) {
			repack_activebuf(mpeg2, *ppStreamMistakeEndPos);	//flush those mistake stream data, otherwise, query_mpeg2Oneframe will always stop at those mistake stream data and enter dead loop
		}
		if(mpeg2->iActiveDataLen > GSTCNMMPEG2_ACTIVEBUF_MAXSIZE) {
			mpeg2->iActiveDataLen = 0;  //clear the buffer, give up those data to avoid active buffer too large and no memory issue. GSTCNMMPEG2_ACTIVEBUF_MAXSIZE is big enough to contain a whole frame
			gcnmdec_remove_redundant_TsDu(&mpeg2->TsDuManager, mpeg2->SeqInfo.MaxReorderDelay+1+1);
		}
		return 1;
	}
	return 0;
}


static __inline int PeekUnWrittenLen_WorkingBuf(WorkingDataBuf* buf)
{
	return buf->bufSz - (buf->pWriter-buf->pBuf);
}

static __inline int PeekWrittenLen_WorkingBuf(WorkingDataBuf* buf)
{
	return buf->pWriter - buf->pBuf;
}

static __inline int PeekUnparsedLen_WorkingBuf(WorkingDataBuf* buf)
{
	return buf->pWriter-buf->pParsed;
}

static __inline int PeekParsedLen_WorkingBuf(WorkingDataBuf* buf)
{
	return buf->pParsed-buf->pBuf;
}

static int
Expand_WorkingBuf(WorkingDataBuf* buf, int newsz)
{
	int off = buf->pParsed - buf->pBuf;
	int off1 = buf->pWriter - buf->pBuf;
	void * pTmp;
	if(newsz <= buf->bufSz) {
		return 0;
	}

	pTmp = g_realloc((void*)buf->pBuf, newsz);
	if(pTmp == NULL) {
		return -1;
	}
	buf->pBuf = pTmp;
	buf->pParsed = buf->pBuf + off;
	buf->pWriter = buf->pBuf + off1;
	buf->bufSz = newsz;
	return 0;
}

static int
AppendData_WorkingBuf(WorkingDataBuf* buf, unsigned char* pSrc, int wantlen)
{
	int fill = PeekUnWrittenLen_WorkingBuf(buf) > wantlen ? wantlen : PeekUnWrittenLen_WorkingBuf(buf);
	if(fill > 0) {
		memcpy(buf->pWriter, pSrc, fill);
		buf->pWriter += fill;
	}
	return fill;
}

static int
Repack_ParseBuf(IPP_MPEG2Parse* parse)
{
	WorkingDataBuf* buf = &parse->databuf;
	int giveuplen = 0;
	if(parse->pFrameBeg==NULL && buf->pBuf != buf->pParsed) {
		giveuplen = PeekParsedLen_WorkingBuf(buf);
		memmove(buf->pBuf, buf->pParsed, PeekUnparsedLen_WorkingBuf(buf));
		buf->pWriter -= giveuplen;
		buf->pParsed = buf->pBuf;
	}
	if(parse->pFrameBeg) {
		giveuplen = parse->pFrameBeg - buf->pBuf;
		if(giveuplen > 0) {
			memmove(buf->pBuf, parse->pFrameBeg, buf->pWriter-parse->pFrameBeg);
			buf->pWriter -= giveuplen;
			buf->pParsed -= giveuplen;
			parse->pFrameBeg = buf->pBuf;
			if(parse->pFrameEnd) {
				parse->pFrameEnd -= giveuplen;
			}
			if(parse->pPicBeg[0]) {
				parse->pPicBeg[0] -= giveuplen;
			}
			if(parse->pPicBeg[1]) {
				parse->pPicBeg[1] -= giveuplen;
			}
		}
	}
	return giveuplen;
}

static int
smartfill_pasrsebuf(Gstcnmmpeg2dec* mpeg2, unsigned char* Input, int InputLen)
{
#define MAXPARSEWBUFSZ	GSTCNMMPEG2_ACTIVEBUF_MAXSIZE
	int expand_rt;
	int couldEatlen = InputLen;
	IPP_MPEG2Parse* prs = &mpeg2->stm_parser;
	if(InputLen == 0) {
		return 0;
	}
	if(PeekUnWrittenLen_WorkingBuf(&prs->databuf) < InputLen) {
		//normal flush buffer
		int couldFlushLen = prs->pFrameBeg == NULL? PeekParsedLen_WorkingBuf(&prs->databuf) : prs->pFrameBeg - prs->databuf.pBuf;
		if(couldFlushLen > prs->databuf.bufSz>>1 || couldFlushLen + PeekUnWrittenLen_WorkingBuf(&prs->databuf) < InputLen) {
			Repack_ParseBuf(prs);
		}
	}
	
	if(PeekUnWrittenLen_WorkingBuf(&prs->databuf) < InputLen) {
		//expand buffer
		int willexpand = InputLen - PeekUnWrittenLen_WorkingBuf(&prs->databuf);
		int newsz;
		if(prs->databuf.bufSz + (willexpand>>1)+ willexpand <= MAXPARSEWBUFSZ) {
			willexpand += (willexpand>>1);
			newsz =  prs->databuf.bufSz + willexpand;
		}else if(prs->databuf.bufSz + willexpand <= MAXPARSEWBUFSZ) {
			newsz = prs->databuf.bufSz + willexpand;;
		}else{
			int overflowlen;
			newsz = MAXPARSEWBUFSZ;
			willexpand = MAXPARSEWBUFSZ - prs->databuf.bufSz;
			overflowlen = InputLen - (willexpand + PeekUnWrittenLen_WorkingBuf(&prs->databuf));
			prs->pFrameBeg = NULL;
			prs->wantstate = WANT_FRAMEBEGIN;
			if(PeekWrittenLen_WorkingBuf(&prs->databuf) > overflowlen) {
				prs->databuf.pParsed = prs->databuf.pBuf + overflowlen;
				Repack_ParseBuf(prs);
			}else{
				//flush all data
				prs->databuf.pParsed = prs->databuf.pWriter = prs->databuf.pBuf;
				couldEatlen = newsz;
			}
		}
		expand_rt = Expand_WorkingBuf(&prs->databuf, newsz);
		if(expand_rt != 0) {
			g_warning("Err: expand parse buffer error %d->%d, ret %d\n", prs->databuf.bufSz, newsz, expand_rt);
			return -1;
		}
	}

	AppendData_WorkingBuf(&prs->databuf, Input + (InputLen-couldEatlen), couldEatlen);
	return 0;
}

//seek code 0x000001XXYYY
#define REQUIRELENAFTERSC	3
static unsigned char*
seekSC_000001xxYn(unsigned char* pData, int len)
{
	unsigned int code;
	unsigned char* pStartCode = pData;
	if(len < 4 + REQUIRELENAFTERSC) {
		return NULL;
	}
	code = ((unsigned int)pStartCode[0]<<24) | ((unsigned int)pStartCode[1]<<16) | ((unsigned int)pStartCode[2]<<8) | pStartCode[3];
	while((code&0xffffff00) != 0x00000100) {
		len--;
		if(len < 4 + REQUIRELENAFTERSC) {
			return NULL;
		}
		code = (code << 8)| pStartCode[4];
		pStartCode++;
	}

	return pStartCode;
}

//0 a frame is parsed, -1 need input
static int
lookup_parserbuf(IPP_MPEG2Parse* parse, int bIgnorePB)
{
	unsigned char* pSC;
	unsigned int pictype;
	//int RequiredLenAfterSC;
	for(;;) {
		pSC = seekSC_000001xxYn(parse->databuf.pParsed, PeekUnparsedLen_WorkingBuf(&parse->databuf));
		if(pSC == NULL) {
			if(parse->databuf.pWriter - parse->databuf.pParsed >= 3 + REQUIRELENAFTERSC) {
				parse->databuf.pParsed = parse->databuf.pWriter - (3 + REQUIRELENAFTERSC);
			}
			if(parse->wantstate == WANT_FRAMEBEGIN) {
				parse->pFrameBeg = NULL;
			}
			return -1;
		}
		
		parse->databuf.pParsed = pSC + 4;
		switch(parse->wantstate) {
		case WANT_FRAMEBEGIN:
			if(pSC[3] == MPEG2SCID_SEQHDR || pSC[3] == MPEG2SCID_GOPHDR || pSC[3] == MPEG2SCID_PICHDR) {
				parse->pFrameBeg = pSC;
				parse->wantstate = WANT_1PICHDR;
				if(pSC[3] == MPEG2SCID_PICHDR) {
					parse->databuf.pParsed = pSC;
				}
			}else{
				parse->pFrameBeg = NULL;
			}
			break;
		case WANT_1PICHDR:
			if(pSC[3] == MPEG2SCID_PICHDR) {
				pictype = (pSC[5] >> 3) & 0x7;
				if(pictype >= MPEG2_PICTYPE_I && pictype <= MPEG2_PICTYPE_B && (pictype==MPEG2_PICTYPE_I||bIgnorePB==0)) {
					parse->pPicBeg[0] = pSC;
					parse->temporal_reference[0] = (pSC[4]<<2) | (pSC[5]>>6);
					parse->pic_type[0] = pictype;
					parse->sliceinPicFound[0] = 0;
					parse->sliceinPicFound[1] = 0;
					parse->wantstate = WANT_1PICEXT;
				}else{
					parse->wantstate = WANT_FRAMEBEGIN;
				}
			}else if(pSC[3] != MPEG2SCID_GOPHDR && pSC[3] != MPEG2SCID_EXT && pSC[3] != MPEG2SCID_USER) {
				parse->wantstate = WANT_FRAMEBEGIN;
				if(pSC[3] == MPEG2SCID_SEQHDR) {
					parse->databuf.pParsed = pSC;
				}
			}
			break;
		case WANT_1PICEXT:
			if(pSC[3] == MPEG2SCID_EXT && (pSC[4]>>4) == 0x8) {
				//meet pic_coding_ext
				parse->pic_struct[0] = (pSC[6] & 0x3);
				if(parse->pic_struct[0] == MPEG2_PICSTUCT_FRAME) {
					parse->wantstate = WANT_FRAMEEND;
				}else if(parse->pic_struct[0] != MPEG2_PICSTUCT_RESERVED) {
					parse->wantstate = WANT_2PICHDR;
				}else{
					parse->wantstate = WANT_FRAMEBEGIN;
				}
			}else{
				parse->databuf.pParsed = pSC;
				parse->wantstate = WANT_FRAMEBEGIN;
			}
			break;
		case WANT_2PICHDR:
			if(pSC[3] == MPEG2SCID_PICHDR) {
				pictype = (pSC[5] >> 3) & 0x7;
				if(pictype < MPEG2_PICTYPE_I || pictype > MPEG2_PICTYPE_B) {
					parse->wantstate = WANT_FRAMEBEGIN;
				}else if(parse->sliceinPicFound[0] && (pictype==parse->pic_type[0] || (parse->pic_type[0]==MPEG2_PICTYPE_I&&pictype==MPEG2_PICTYPE_P))) {
					parse->pPicBeg[1] = pSC;
					parse->temporal_reference[1] = (pSC[4]<<2)|(pSC[5]>>6);
					parse->pic_type[1] = pictype;
					parse->wantstate = WANT_2PICEXT;
				}else{
					parse->databuf.pParsed = pSC;
					parse->wantstate = WANT_FRAMEBEGIN;
				}
			}else if(pSC[3] >= MPEG2SCID_SLICEMIN && pSC[3] <= MPEG2SCID_SLICEMAX) {
				parse->sliceinPicFound[0] = 1;
			}else if(pSC[3] != MPEG2SCID_EXT && pSC[3] != MPEG2SCID_USER) {
				parse->databuf.pParsed = pSC;
				parse->wantstate = WANT_FRAMEBEGIN;
			}
			break;
		case WANT_2PICEXT:
			if(pSC[3] == MPEG2SCID_EXT && (pSC[4]>>4) == 0x8) {
				//meet pic_coding_ext
				parse->pic_struct[1] = (pSC[6] & 0x3);
				if(parse->pic_struct[1] != MPEG2_PICSTUCT_RESERVED && parse->pic_struct[1] != MPEG2_PICSTUCT_FRAME && parse->pic_struct[1] != parse->pic_struct[0]) {
					parse->wantstate = WANT_FRAMEEND;
				}else{
					//ignore last pic and this pic, to seek new frame
					parse->wantstate = WANT_FRAMEBEGIN;
				}
			}else{
				parse->databuf.pParsed = pSC;
				parse->wantstate = WANT_FRAMEBEGIN;
			}
			break;
		default:
			//parse->wantstate == WANT_FRAMEEND case
			if(pSC[3] >= MPEG2SCID_SLICEMIN && pSC[3] <= MPEG2SCID_SLICEMAX) {
				parse->sliceinPicFound[0] = 1;
				parse->sliceinPicFound[1] = 1;
			}else if(pSC[3] != MPEG2SCID_EXT && pSC[3] != MPEG2SCID_USER) {
				parse->wantstate = WANT_FRAMEBEGIN;
				parse->databuf.pParsed = pSC;
				if(parse->sliceinPicFound[0] == 1 && parse->sliceinPicFound[1] == 1) {
					//if(pSC[3] == MPEG2SCID_SEQHDR || pSC[3] == MPEG2SCID_GOPHDR || pSC[3] == MPEG2SCID_PICHDR || pSC[3] == MPEG2SCID_SEQEND) {	//only meet those SC, think a frame is ended
					if(1) {	//meet SC even it's an error SC like mpeg system SC, think a frame is ended
						parse->pFrameEnd = pSC;
						return 0;
					}
				}
			}
			break;
		}
	}
}

static __inline void
smartattach_parserbuf(Gstcnmmpeg2dec* mpeg2)
{
	if(mpeg2->stm_parser.databuf.pBuf == NULL) {
		mpeg2->stm_parser.databuf.bufSz = mpeg2->iActiveBufSz;
		mpeg2->stm_parser.databuf.pBuf = mpeg2->pActiveBuf;
		mpeg2->stm_parser.databuf.pParsed = mpeg2->pActiveBuf;
		mpeg2->stm_parser.databuf.pWriter = mpeg2->pActiveBuf + mpeg2->iActiveDataLen;
		mpeg2->stm_parser.wantstate = WANT_FRAMEBEGIN;
		mpeg2->stm_parser.pFrameBeg = NULL;
	}
	return;
}


static GstFlowReturn
gst_cnmmpeg2dec_chain(GstPad *pad, GstBuffer *buf)
{
	gcnm_printfmore("enter %s, in buf sz %d, ts %lld\n", __func__, GST_BUFFER_SIZE(buf), GST_BUFFER_TIMESTAMP(buf));
	gcnm_printfmore("in %s, thread id 0x%x, pthread id-t 0x%x, sizeof(pthread_t) %d\n", __func__, (unsigned int)syscall(224), (unsigned int)pthread_self(), sizeof(pthread_t));
	unsigned char* pDataForCnm;
	int	DataForCnmLen;
	int ret;
	unsigned char* pStreamMistakeEndPos = NULL;
#ifndef IPPGST_QUICKPARSER
	unsigned char *pPic0StartPos, *pFrameEndPos;
	int bIFrame;
#endif
	GstFlowReturn FlowRt = GST_FLOW_OK;
	Gstcnmmpeg2dec* mpeg2 = GST_CNMMPEG2DEC(GST_OBJECT_PARENT(pad));
	GCNMDec_StmBuf* pCnmInputBuf;
	unsigned char* pData = GST_BUFFER_DATA(buf);
	int DataLen = GST_BUFFER_SIZE(buf);

#ifdef LOG_TIME_NOCODEC_NOPUSH
	gettimeofday(&((CnmMPEG2ProbeData*)mpeg2->pProbeData)->t_0, NULL);
#endif

	if(mpeg2->b1stFrame_forFileplay){
		GstCaps *CapTmp;
		unsigned char* pfilldata;
		int filllen;
		unsigned char* pSeqHdr, *pFrameEnd;

		if(mpeg2->bSeqHdrSeeked == 0) {
			unsigned char* pSeq;
			pSeq = seek_mpeg2seqhdrSC(pData, DataLen);
			if(pSeq == NULL) {
				gcnm_printf("no seq hdr found in input data, ignore the data\n");
				gst_buffer_unref(buf);
				FlowRt = GST_FLOW_OK;
				goto CNMMPEG2_CHAIN_RET;
			}else{
				mpeg2->bSeqHdrSeeked = 1;
				filllen = DataLen - (pSeq - pData);
				pfilldata = pSeq;
			}
		}else{
			pfilldata = pData;
			filllen = DataLen;
		}
		ret = smartfill_activebuf(mpeg2, pfilldata, filllen);
		if(ret != 0) {
			gst_buffer_unref(buf);
			FlowRt = GST_FLOW_ERROR;
			goto CNMMPEG2_CHAIN_RET;
		}

		if(GST_BUFFER_TIMESTAMP(buf) != GST_CLOCK_TIME_NONE) {
			gcnmdec_insert_TsDu(&mpeg2->TsDuManager, buf);
			mpeg2->last_inTs = GST_BUFFER_TIMESTAMP(buf);
			gcnm_printf("TS repo length %d\n", mpeg2->TsDuManager.iTsDuListLen);
		}
		gst_buffer_unref(buf);

		ret = query_mpeg2SeqHdr1stFrame(mpeg2->pActiveBuf, mpeg2->iActiveDataLen, &pSeqHdr, &pFrameEnd, &pStreamMistakeEndPos);
		if(ret != 0) {
			if(pStreamMistakeEndPos != NULL) {
				repack_activebuf(mpeg2, pStreamMistakeEndPos);  //flush those mistake stream data, otherwise, query_mpeg2SeqHdr1stFrame will always stop at those mistake stream data and enter dead loop
			}
			if(mpeg2->iActiveDataLen > GSTCNMMPEG2_ACTIVEBUF_MAXSIZE) {
				mpeg2->iActiveDataLen = 0;  //clear the buffer, give up those data to avoid active buffer too large and no memory issue. GSTCNMMPEG2_ACTIVEBUF_MAXSIZE is big enough to contain a whole frame
				gcnmdec_remove_redundant_TsDu(&mpeg2->TsDuManager, 1);
			}
			FlowRt = GST_FLOW_OK;
			goto CNMMPEG2_CHAIN_RET;
		}

		gcnmdec_remove_redundant_TsDu(&mpeg2->TsDuManager, 1+1);	//only 1st frame and the 2nd frame TS is reasonable
		mpeg2->b1stFrame_forFileplay = 0;
		mpeg2->bLetDataBeginFromBorder = 0;
		mpeg2->bNotOutputPBframe = 0;
		mpeg2->bNotInputPBframe = 0;

		ret = gst_cnmmpeg2dec_launch_codec(pSeqHdr, pFrameEnd - pSeqHdr, mpeg2);
		gcnm_printf("gst_cnmmpeg2dec_launch_codec ret %d\n", ret);
		if(ret != 0) {
			gcnm_printf("Err: gst_cnmmpeg2dec_launch_codec fail, ret %d\n", ret);
			FlowRt = GST_FLOW_ERROR;
			goto CNMMPEG2_CHAIN_RET;
		}
		pCnmInputBuf = &(mpeg2->StmBufPool[0]);
		memmove(mpeg2->pActiveBuf, pFrameEnd, mpeg2->iActiveDataLen - (pFrameEnd - mpeg2->pActiveBuf));
		mpeg2->iActiveDataLen = mpeg2->iActiveDataLen - (pFrameEnd - mpeg2->pActiveBuf);

		mpeg2->num = mpeg2->SeqInfo.FrameRateNumerator;
		mpeg2->den = mpeg2->SeqInfo.FrameRateDenominator;

		if(mpeg2->num != 0) {
			mpeg2->TsDuManager.FrameFixPeriod = gst_util_uint64_scale_int(GST_SECOND, mpeg2->den, mpeg2->num);
			mpeg2->TS_judgement = (5*((gint64)(mpeg2->TsDuManager.FrameFixPeriod))) >> 3;
			gcnm_printf("frame fix period %lld, TS_judgement %lld\n", mpeg2->TsDuManager.FrameFixPeriod, mpeg2->TS_judgement);
		}

		mpeg2->DownFrameLayout.iDownFrameWidth = mpeg2->SeqInfo.FrameROI.width;
		mpeg2->DownFrameLayout.iDownFrameHeight = mpeg2->SeqInfo.FrameROI.height;
		if(mpeg2->iI420LayoutKind != 2) {
			gcnmdec_update_downbuf_layout(mpeg2->DownFrameLayout.iDownFrameWidth, mpeg2->DownFrameLayout.iDownFrameHeight, 0, &mpeg2->DownFrameLayout);
		}else{
			gcnmdec_update_downbuf_layout(mpeg2->DownFrameLayout.iDownFrameWidth, mpeg2->DownFrameLayout.iDownFrameHeight, 1, &mpeg2->DownFrameLayout);
		}

		mpeg2->iAlignedBufSz = (mpeg2->SeqInfo.FrameAlignedWidth * mpeg2->SeqInfo.FrameAlignedHeight * 3)>>1;

		gcnm_printf("Will set srcpad cap, fps %d/%d, w %d, h %d\n", mpeg2->num, mpeg2->den, mpeg2->DownFrameLayout.iDownFrameWidth, mpeg2->DownFrameLayout.iDownFrameHeight);
		CapTmp = gst_caps_new_simple ("video/x-raw-yuv",
			"format", GST_TYPE_FOURCC, GST_STR_FOURCC ("I420"),
			"width", G_TYPE_INT, mpeg2->DownFrameLayout.iDownFrameWidth,
			"height", G_TYPE_INT, mpeg2->DownFrameLayout.iDownFrameHeight,
			"framerate", GST_TYPE_FRACTION, mpeg2->num, mpeg2->den,
			NULL);
		gst_pad_set_caps(mpeg2->srcpad, CapTmp);
		gst_caps_unref(CapTmp);

#ifdef LOG_TIME_NOCODEC_NOPUSH
		gettimeofday(&((CnmMPEG2ProbeData*)mpeg2->pProbeData)->t_2, NULL);
#endif
		ret = codec_decoding_1frame(mpeg2, pCnmInputBuf, 0, &FlowRt);
#ifdef LOG_TIME_NOCODEC_NOPUSH
		gettimeofday(&((CnmMPEG2ProbeData*)mpeg2->pProbeData)->t_3, NULL);
		((CnmMPEG2ProbeData*)mpeg2->pProbeData)->tim_nodec_nopush -= (((CnmMPEG2ProbeData*)mpeg2->pProbeData)->t_3.tv_sec - ((CnmMPEG2ProbeData*)mpeg2->pProbeData)->t_2.tv_sec)*1000000 + (((CnmMPEG2ProbeData*)mpeg2->pProbeData)->t_3.tv_usec - ((CnmMPEG2ProbeData*)mpeg2->pProbeData)->t_2.tv_usec);
#endif
		gcnm_printfmore("codec_decoding_1frame() ret %d, line %d\n", ret, __LINE__);

		if(FlowRt != GST_FLOW_OK) {
			goto CNMMPEG2_CHAIN_RET;
		}
	}else{
		unsigned char* pBegin;
		if(mpeg2->bLetDataBeginFromBorder) {
			//found new frame begin position
			pBegin = seek_mpeg2picend(pData, DataLen);
			if(pBegin == NULL) {
				//not found, give up the data, the purpose of giving up those data is to skip those needless TS
				gst_buffer_unref(buf);
				gcnm_printf("Not found mpeg2 stream frame begin after seeking, give up those stream data.\n");
				FlowRt = GST_FLOW_OK;
				goto CNMMPEG2_CHAIN_RET;
			}
			DataLen -= pBegin-pData;
			mpeg2->bLetDataBeginFromBorder = 0;
		}else{
			pBegin = pData;
		}

#ifndef IPPGST_QUICKPARSER
		ret = smartfill_activebuf(mpeg2, pBegin, DataLen);
#else
		smartattach_parserbuf(mpeg2);
		ret = smartfill_pasrsebuf(mpeg2, pBegin, DataLen);
#endif
		if(ret != 0) {
			gst_buffer_unref(buf);
			FlowRt = GST_FLOW_ERROR;
			goto CNMMPEG2_CHAIN_RET;
		}

		if( GST_BUFFER_TIMESTAMP(buf) != GST_CLOCK_TIME_NONE ) {
			//For interlace mpeg2, must judge whether the TS is for a new frame begin or a new field begin. We only store frame TS.
			if(mpeg2->last_inTs == GST_CLOCK_TIME_NONE || (guint64)GNM_TSABSSUB(GST_BUFFER_TIMESTAMP(buf), mpeg2->last_inTs) > mpeg2->TS_judgement) {
				gcnmdec_insert_TsDu(&mpeg2->TsDuManager, buf);
				mpeg2->last_inTs = GST_BUFFER_TIMESTAMP(buf);
				gcnm_printfmore("insert TsDu list in chain func, TS in repo length %d\n", g_slist_length(mpeg2->TsDuManager.TsDuList));
			}else{
				gcnm_printfmore("++++++TS judgement find un-needed TS!\n");
			}
		}

		gst_buffer_unref(buf);

		//remove overfull TS, prevent TS list too long
		gcnmdec_remove_redundant_TsDu(&mpeg2->TsDuManager, GSTCNMMPEG2_TS_MAXCOUNT);
	}

	if(!mpeg2->bCodecSucLaunched) {
		FlowRt = GST_FLOW_ERROR;
		goto CNMMPEG2_CHAIN_RET;
	}

#ifdef IPPGST_QUICKPARSER
	smartattach_parserbuf(mpeg2);
#endif

	//decode data in active buffer, in active buffer, probably contain multiple frames, because one input fragment probably contain multiple frames
	for(;;) {
#ifndef IPPGST_QUICKPARSER
		ret = lookup_activebufdata(mpeg2, &pPic0StartPos, &pFrameEndPos, &pStreamMistakeEndPos, &bIFrame);
		if(ret != 0) {
			//not found frame
			FlowRt = GST_FLOW_OK;
			goto CNMMPEG2_CHAIN_RET;
		}

		if(mpeg2->bNotInputPBframe) {
			while(!bIFrame && ret==0) {
				//give up this whole frame, continue lookup, until found I frame or couldn't found frame
				memmove(mpeg2->pActiveBuf, pFrameEndPos, mpeg2->iActiveDataLen - (pFrameEndPos - mpeg2->pActiveBuf));
				mpeg2->iActiveDataLen = mpeg2->iActiveDataLen - (pFrameEndPos - mpeg2->pActiveBuf);
				ret = lookup_activebufdata(mpeg2, &pPic0StartPos, &pFrameEndPos, &pStreamMistakeEndPos, &bIFrame);
			}
			if(ret != 0) {
				//couldn't found frame
				FlowRt = GST_FLOW_OK;
				goto CNMMPEG2_CHAIN_RET;
			}
			mpeg2->bNotInputPBframe = 0;
			gcnmdec_remove_redundant_TsDu(&mpeg2->TsDuManager, 2);  //remove redundant TS, only left TS for this I frame and the next frame
		}
		pDataForCnm = mpeg2->pActiveBuf;
		DataForCnmLen = pFrameEndPos-mpeg2->pActiveBuf;
#else
		ret = lookup_parserbuf(&mpeg2->stm_parser, mpeg2->bNotInputPBframe);
		if(ret != 0) {
			//not found frame
			FlowRt = GST_FLOW_OK;
			goto CNMMPEG2_CHAIN_RET;
		}else{
			if(mpeg2->bNotInputPBframe) {
				mpeg2->stm_parser.pic_type[0] = MPEG2_PICTYPE_I;
				mpeg2->bNotInputPBframe = 0;
				gcnmdec_remove_redundant_TsDu(&mpeg2->TsDuManager, 2);  //remove redundant TS, only left TS for this I frame and the next frame
			}
		}
		pDataForCnm = mpeg2->stm_parser.pFrameBeg;
		DataForCnmLen = mpeg2->stm_parser.pFrameEnd - mpeg2->stm_parser.pFrameBeg;
#endif

		ret = gcnmdec_select_stream(mpeg2->StmBufPool, GSTCNMDEC_MPEG2_STMPOOLSZ, DataForCnmLen);
		if( ret < 0 || ret >= GSTCNMDEC_MPEG2_STMPOOLSZ ) {
			mpeg2->iMemErr = -3;
			gcnm_printf("Err: gcnmdec_select_stream fail, ret %d", ret);
			GST_ERROR_OBJECT(mpeg2, "Err: gcnmdec_select_stream fail, ret %d", ret);
			FlowRt = GST_FLOW_ERROR;
			goto CNMMPEG2_CHAIN_RET;
		}
		pCnmInputBuf = &(mpeg2->StmBufPool[ret]);

#ifdef LOG_TIME_STMCOPY
		gettimeofday(&((CnmMPEG2ProbeData*)mpeg2->pProbeData)->t_cpystm0, NULL);
#endif
		gcnmdec_fill_stream(pCnmInputBuf, pDataForCnm, DataForCnmLen);
#ifdef LOG_TIME_STMCOPY
		gettimeofday(&((CnmMPEG2ProbeData*)mpeg2->pProbeData)->t_cpystm1, NULL);
		((CnmMPEG2ProbeData*)mpeg2->pProbeData)->tim_cpystm += (((CnmMPEG2ProbeData*)mpeg2->pProbeData)->t_cpystm1.tv_sec - ((CnmMPEG2ProbeData*)mpeg2->pProbeData)->t_cpystm0.tv_sec)*1000000 + (((CnmMPEG2ProbeData*)mpeg2->pProbeData)->t_cpystm1.tv_usec - ((CnmMPEG2ProbeData*)mpeg2->pProbeData)->t_cpystm0.tv_usec);
#endif

#ifndef IPPGST_QUICKPARSER
		memmove(mpeg2->pActiveBuf, pFrameEndPos, mpeg2->iActiveDataLen - (pFrameEndPos - mpeg2->pActiveBuf));
		mpeg2->iActiveDataLen = mpeg2->iActiveDataLen - (pFrameEndPos - mpeg2->pActiveBuf);
#else
		mpeg2->stm_parser.pFrameBeg = NULL;
#endif

#ifdef LOG_TIME_NOCODEC_NOPUSH
		gettimeofday(&((CnmMPEG2ProbeData*)mpeg2->pProbeData)->t_2, NULL);
#endif
		ret = codec_decoding_1frame(mpeg2, pCnmInputBuf, 0, &FlowRt);
#ifdef LOG_TIME_NOCODEC_NOPUSH
		gettimeofday(&((CnmMPEG2ProbeData*)mpeg2->pProbeData)->t_3, NULL);
		((CnmMPEG2ProbeData*)mpeg2->pProbeData)->tim_nodec_nopush -= (((CnmMPEG2ProbeData*)mpeg2->pProbeData)->t_3.tv_sec - ((CnmMPEG2ProbeData*)mpeg2->pProbeData)->t_2.tv_sec)*1000000 + (((CnmMPEG2ProbeData*)mpeg2->pProbeData)->t_3.tv_usec - ((CnmMPEG2ProbeData*)mpeg2->pProbeData)->t_2.tv_usec);
#endif
		gcnm_printfmore("codec_decoding_1frame() ret %d, line %d\n", ret, __LINE__);

		if(FlowRt != GST_FLOW_OK) {
			//something wrong, stop consuming data in active buffer
			goto CNMMPEG2_CHAIN_RET;
		}
	}

CNMMPEG2_CHAIN_RET:
#ifdef LOG_TIME_NOCODEC_NOPUSH
	gettimeofday(&((CnmMPEG2ProbeData*)mpeg2->pProbeData)->t_1, NULL);
	((CnmMPEG2ProbeData*)mpeg2->pProbeData)->tim_nodec_nopush += (((CnmMPEG2ProbeData*)mpeg2->pProbeData)->t_1.tv_sec - ((CnmMPEG2ProbeData*)mpeg2->pProbeData)->t_0.tv_sec)*1000000 + (((CnmMPEG2ProbeData*)mpeg2->pProbeData)->t_1.tv_usec - ((CnmMPEG2ProbeData*)mpeg2->pProbeData)->t_0.tv_usec);
#endif
	if(FlowRt < GST_FLOW_UNEXPECTED) {
		GST_ELEMENT_ERROR(mpeg2, STREAM, DECODE, (NULL), ("%s() flow err %d, codec err 0x%x, mem err %d", __FUNCTION__, FlowRt, mpeg2->iCodecErr, mpeg2->iMemErr));
	}

	return FlowRt;
}

static void
gst_cnmmpeg2dec_clean_streambuf(Gstcnmmpeg2dec *mpeg2)
{
	int i;
	gcnm_printf("free stream buffer\n");

	for(i=0;i<GSTCNMDEC_MPEG2_STMPOOLSZ;i++) {
		if(mpeg2->StmBufPool[i].iBufSize != 0) {
			delete_cnmStmBuf(mpeg2->StmBufPool[i].mem);
			mpeg2->StmBufPool[i].iBufSize = 0;
			mpeg2->StmBufPool[i].BufPhyAddr = 0;
			mpeg2->StmBufPool[i].BufVirAddr = NULL;
			mpeg2->StmBufPool[i].iDataLen = 0;
			mpeg2->StmBufPool[i].mem = NULL;
		}
	}

	return;
}

static gboolean
gst_cnmmpeg2dec_null2ready(Gstcnmmpeg2dec *mpeg2)
{
	gcnm_printf("enter %s\n", __func__);
	gcnm_printfmore("in %s, thread id 0x%x, pthread id-t 0x%x, sizeof(pthread_t) %d\n", __func__, (unsigned int)syscall(224), (unsigned int)pthread_self(), sizeof(pthread_t));

	mpeg2->pMPEG2DecoderState = NULL;
	mpeg2->totalFrames = 0;
	mpeg2->codec_time = 0;
#define MPEG2CNM_GST_ACTIVEBUF_INITSZ   (512*1024)
	mpeg2->pActiveBuf = g_malloc(MPEG2CNM_GST_ACTIVEBUF_INITSZ);
	if(mpeg2->pActiveBuf == NULL) {
		return FALSE;
	}
	mpeg2->iActiveBufSz = MPEG2CNM_GST_ACTIVEBUF_INITSZ;

	mpeg2->ShareBufManager.watcher = gcnmdec_watcher_create(mpeg2, NULL, &mpeg2->ShareBufManager); //create watcher
	if(mpeg2->ShareBufManager.watcher == NULL) {
		g_warning("Err: gcnmdec_watcher_create() fail");
		return FALSE;
	}


	return TRUE;
}


static gboolean
gst_cnmmpeg2dec_ready2null(Gstcnmmpeg2dec *mpeg2)
{
	gcnm_printf("enter %s\n", __func__);
	gcnm_printfmore("in %s, thread id 0x%x, pthread id-t 0x%x, sizeof(pthread_t) %d\n", __func__, (unsigned int)syscall(224), (unsigned int)pthread_self(), sizeof(pthread_t));

	//close decoder
	g_mutex_lock(mpeg2->cnmCodecMtx);
	if(mpeg2->pMPEG2DecoderState) {
		if(mpeg2->bCnmInDream) {
			HibernateWakeupCNM(mpeg2->pMPEG2DecoderState, 0, &mpeg2->bCnmInDream);
		}
		mpeg2->iCodecErr |= gcnmdec_close_codec(&mpeg2->pMPEG2DecoderState, &mpeg2->ShareBufManager);
	}
	g_mutex_unlock(mpeg2->cnmCodecMtx);

	gcnmdec_remove_redundant_TsDu(&mpeg2->TsDuManager, 0);

	//free buffers
	gst_cnmmpeg2dec_clean_streambuf(mpeg2);
	if(mpeg2->pActiveBuf) {
		g_free(mpeg2->pActiveBuf);
		mpeg2->iActiveBufSz = 0;
		mpeg2->iActiveDataLen = 0;
		mpeg2->pActiveBuf = NULL;
	}

	if(! mpeg2->iShareFrameBuf) {
		gstcnmElement_clean_nonShareFramebuf(mpeg2->DecFramesInfoArr, &mpeg2->iDecFrameBufArrLen);
	}else{
		g_mutex_lock(mpeg2->ShareBufManager.mtx);
		gstcnmElement_clean_idleShareFBs(&mpeg2->ShareBufManager, NULL);	//for those share frame buffer which is still owned by sink, will free them on their gstbuffer finalizing callback function
		mpeg2->ShareBufManager.NotNeedFMemAnyMore = 1;
		g_mutex_unlock(mpeg2->ShareBufManager.mtx);
	}


	if(mpeg2->ShareBufManager.watcher) {
		FRAMES_WATCHMAN_UNREF(mpeg2->ShareBufManager.watcher);  //smart release watcher, free frame buffers
	}



#ifdef DEBUG_PERFORMANCE
	printf("codec totally outputed %d frames, spend %lld us\n", mpeg2->totalFrames, mpeg2->codec_time);
#else
	gcnm_printf("codec totally outputed %d frames, spend %lld us\n", mpeg2->totalFrames, mpeg2->codec_time);
#endif

#ifdef LOG_FRAMECOPYTIME
	printf("copy %d frame total cost %d us\n", ((CnmMPEG2ProbeData*)mpeg2->pProbeData)->log_copycnt, ((CnmMPEG2ProbeData*)mpeg2->pProbeData)->log_copytime);
#endif
#ifdef LOG_TIME_NOCODEC_NOPUSH
	printf("in _chain, except call dec API and push, system cost %lld us\n", ((CnmMPEG2ProbeData*)mpeg2->pProbeData)->tim_nodec_nopush);
#endif
#ifdef LOG_TIME_STMCOPY
	printf("in _chain, copy stream data into cnm buffer(except 1frame), system cost %lld us\n", ((CnmMPEG2ProbeData*)mpeg2->pProbeData)->tim_cpystm);
#endif
	

	gcnm_printf("-----------At ready2null, c&m codec error %d, memory error %d\n", mpeg2->iCodecErr, mpeg2->iMemErr);


	return TRUE;
}

static int gst_cnmmpeg2dec_ready2paused(Gstcnmmpeg2dec *mpeg2)
{
	gcnm_printf("gst_cnmmpeg2dec_ready2paused() is called\n");
	gcnm_printfmore("in %s, thread id 0x%x, pthread id-t 0x%x, sizeof(pthread_t) %d\n", __func__, (unsigned int)syscall(224), (unsigned int)pthread_self(), sizeof(pthread_t));
	//prepare for new stream

	return 0;
}

static int gst_cnmmpeg2dec_paused2ready(Gstcnmmpeg2dec* mpeg2)
{
	gcnm_printf("gst_cnmmpeg2dec_paused2ready() is called\n");
	gcnm_printf("-----------At paused2ready, cnm decoder error status %d\n", mpeg2->iCodecErr);
	gcnm_printfmore("in %s, thread id 0x%x, pthread id-t 0x%x, sizeof(pthread_t) %d\n", __func__, (unsigned int)syscall(224), (unsigned int)pthread_self(), sizeof(pthread_t));
	return 0;
}

static __inline int paused2play(Gstcnmmpeg2dec* mpeg2)
{
	IppCodecStatus ret = IPP_STATUS_NOERR;
	g_mutex_lock(mpeg2->cnmCodecMtx);
	if(mpeg2->pMPEG2DecoderState && mpeg2->bEnableHibernateAtPause && mpeg2->bCnmInDream) {
		ret = HibernateWakeupCNM(mpeg2->pMPEG2DecoderState, 0, &mpeg2->bCnmInDream);
	}
	g_mutex_unlock(mpeg2->cnmCodecMtx);
	if(IPP_STATUS_NOERR != ret) {
		g_warning("Wakeup cnm fail at pause2paly, ret %d", ret);
		return -1;
	}
	return 0;
}

static __inline int play2paused(Gstcnmmpeg2dec* mpeg2)
{
	IppCodecStatus ret = IPP_STATUS_NOERR;
	g_mutex_lock(mpeg2->cnmCodecMtx);
	if(mpeg2->pMPEG2DecoderState && mpeg2->bEnableHibernateAtPause && ! mpeg2->bCnmInDream) {
		ret = HibernateWakeupCNM(mpeg2->pMPEG2DecoderState, 1, &mpeg2->bCnmInDream);
	}
	g_mutex_unlock(mpeg2->cnmCodecMtx);
	if(IPP_STATUS_NOERR != ret) {
		g_warning("Hibernate cnm fail at pause2paly, ret %d", ret);
		return -1;
	}
	return 0;
}


static GstStateChangeReturn
gst_cnmmpeg2dec_change_state(GstElement *element, GstStateChange transition)
{
	gcnm_printf("enter %s(), change from %d to %d\n", __func__, transition>>3, transition&7);

	GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;
	Gstcnmmpeg2dec *mpeg2 = GST_CNMMPEG2DEC(element);

	switch (transition)
	{
	case GST_STATE_CHANGE_NULL_TO_READY:
		if(!gst_cnmmpeg2dec_null2ready(mpeg2)){
			goto statechange_failed;
		}
		break;
	case GST_STATE_CHANGE_READY_TO_PAUSED:
		gst_cnmmpeg2dec_ready2paused(mpeg2);
		break;
	case GST_STATE_CHANGE_PAUSED_TO_PLAYING:
		if(0 != paused2play(mpeg2)) {
			goto statechange_failed;
		}
		break;

	case GST_STATE_CHANGE_PAUSED_TO_READY:
		set_ElementState(&mpeg2->ElementState, CNMELESTATE_RESTING);
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
		if(0 != play2paused(mpeg2)) {
			goto statechange_failed;
		}
		break;
	case GST_STATE_CHANGE_PAUSED_TO_READY:
		gst_cnmmpeg2dec_paused2ready(mpeg2);
		break;
	case GST_STATE_CHANGE_READY_TO_NULL:
		if(!gst_cnmmpeg2dec_ready2null(mpeg2)){
			goto statechange_failed;
		}
		break;

	case GST_STATE_CHANGE_READY_TO_PAUSED:
		set_ElementState(&mpeg2->ElementState, CNMELESTATE_RUNNING);
		break;
	default:
		break;
	}

	gcnm_printf("leave %s(), change from %d to %d\n", __func__, transition>>3, transition&7);
	return ret;

	 /* ERRORS */
statechange_failed:
	{
		/* subclass must post a meaningfull error message */
		GST_ERROR_OBJECT (mpeg2, "state change failed");
		return GST_STATE_CHANGE_FAILURE;
	}
}

static void
gst_cnmmpeg2dec_base_init (gpointer klass)
{
	gcnm_printf("enter %s\n", __func__);

	static GstElementDetails mpeg2dec_details = {
		"IPPCNM decoder MPEG2",
		"Codec/Decoder/Video",
		"HARDWARE MPEG2 Decoder based-on IPP CNM CODEC",
		""
	};

	GstElementClass *element_class = GST_ELEMENT_CLASS (klass);

	gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&src_factory));
	gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&sink_factory));
	gst_element_class_set_details (element_class, &mpeg2dec_details);
}

static void
gst_cnmmpeg2dec_finalize(GObject * object)
{
	Gstcnmmpeg2dec* mpeg2 = GST_CNMMPEG2DEC(object);
	if(mpeg2->pProbeData) {
		g_free(mpeg2->pProbeData);
	}
	g_mutex_free(mpeg2->cnmCodecMtx);

	g_mutex_free(mpeg2->ElementState.elestate_mtx);

	GST_DEBUG_OBJECT(mpeg2, "Gstcnmmpeg2dec instance(%p) parent class is finalizing, codec err 0x%x, mem err %d.", mpeg2, mpeg2->iCodecErr, mpeg2->iMemErr);
	G_OBJECT_CLASS(parent_class)->finalize(object);

	gcnm_printf("Gstcnmmpeg2dec instance 0x%x is finalized!!!\n", (unsigned int)object);
	printf("Gstcnmmpeg2dec instance (0x%x) is finalized\n", (unsigned int)object);

	return;
}


static void
gst_cnmmpeg2dec_class_init (Gstcnmmpeg2decClass *klass)
{
	gcnm_printf("enter %s\n", __func__);

	GObjectClass *gobject_class  = (GObjectClass*) klass;
	GstElementClass *gstelement_class = (GstElementClass*) klass;

	gobject_class->set_property = gst_cnmmpeg2dec_set_property;
	gobject_class->get_property = gst_cnmmpeg2dec_get_property;

	g_object_class_install_property (gobject_class, ARG_I420LAYOUTKIND, \
		g_param_spec_int ("I420layoutkind", "push down I420 buffer layout", \
		"Push down I420 buffer layout. It could be 0<auto decide>, 1<compact I420> or 2<gstreamer standard I420>. Don't access this property unless you are clear what you are doing.", \
		0/* range_MIN */, 2/* range_MAX */, 0/* default_INIT */, G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, ARG_IGN_PBBEFOREI, \
		g_param_spec_int ("ignorePBbeforeI", "Ignore P and B frames before I frame", \
		"Ignore P and B frames before I frame in one new segment, could be 1<ignore> or 0<not ignore>.", \
		0/* range_MIN */, 1/* range_MAX */, 1/* default_INIT */, G_PARAM_READWRITE));

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
		g_param_spec_int ("totalframes", "Totalframe of cnm mpeg2 decoder outputted",
			"Number of frame of cnm mpeg2 decoder outputted", 0, G_MAXINT, 0, G_PARAM_READABLE));
	g_object_class_install_property (gobject_class, ARG_CODECTIME,
		g_param_spec_int64 ("codectime", "cnm mpeg2 decode time",
			"Pure codec time of calling cnm mpeg2 decoding API(microsecond)", 0, G_MAXINT64, 0, G_PARAM_READABLE));
#endif

	gobject_class->finalize = GST_DEBUG_FUNCPTR (gst_cnmmpeg2dec_finalize);
	gstelement_class->change_state = GST_DEBUG_FUNCPTR (gst_cnmmpeg2dec_change_state);
	GST_DEBUG_CATEGORY_INIT(cnmdec_mpeg2_debug, "cnmdec_mpeg2", 0, "CNM mpeg2 Decoder Element");
	return;
}

static void gst_cnmmpeg2dec_init_members(Gstcnmmpeg2dec* mpeg2)
{
	gcnm_printf("gst_cnmmpeg2dec_init_members is called\n");
	mpeg2->pProbeData = g_malloc0(sizeof(CnmMPEG2ProbeData));

	mpeg2->pMPEG2DecoderState = NULL;
	mpeg2->num = 0;
	mpeg2->den = 1;
	mpeg2->bCodecSucLaunched = 0;
	mpeg2->b1stFrame_forFileplay = 1;
	mpeg2->bSeqHdrSeeked = 0;
	mpeg2->iCodecErr = 0;
	mpeg2->iMemErr = 0;

	memset(&mpeg2->SeqInfo, 0, sizeof(mpeg2->SeqInfo));
	memset(mpeg2->DecFramesInfoArr, 0, sizeof(mpeg2->DecFramesInfoArr));
	mpeg2->iDecFrameBufArrLen = 0;
	memset(&mpeg2->FilePlay_StmBuf, 0, sizeof(mpeg2->FilePlay_StmBuf));
	memset(&mpeg2->DecOutInfo, 0, sizeof(mpeg2->DecOutInfo));
	memset(mpeg2->StmBufPool, 0, sizeof(mpeg2->StmBufPool));
	memset(&mpeg2->DownFrameLayout, 0, sizeof(mpeg2->DownFrameLayout));
	memset(&mpeg2->ShareBufManager, 0, sizeof(mpeg2->ShareBufManager));

	mpeg2->ElementState.state = CNMELESTATE_RESTING;
	mpeg2->ElementState.elestate_mtx = g_mutex_new();

	mpeg2->pActiveBuf = NULL;
	mpeg2->iActiveBufSz = 0;
	mpeg2->iActiveDataLen = 0;
	memset(&mpeg2->stm_parser, 0, sizeof(mpeg2->stm_parser));

	mpeg2->iIgnorePBBeforeI = 1;
	mpeg2->bNotOutputPBframe = 0;
	mpeg2->bNotInputPBframe = 0;

	gcnmdec_init_TsDuMgr(&mpeg2->TsDuManager);
	mpeg2->last_inTs = GST_CLOCK_TIME_NONE;
	mpeg2->TS_judgement = 0;
	mpeg2->iIgnLastSegDelayedFrames = 0;
	mpeg2->iI420LayoutKind = 0;
	mpeg2->iShareFrameBuf = 1;
	mpeg2->iTimeoutForDisp1Frame = WAIT_DOWNSTREAM_RETBUF_TIMEOUT;
	mpeg2->iSupportMultiIns = 1;
	mpeg2->bEnableHibernateAtPause = 1;
	mpeg2->bEnableDynaClk = 1;

	mpeg2->bNewSegReceivedAfterflush = 0;

	mpeg2->cnmCodecMtx = g_mutex_new();
	mpeg2->bCnmInDream = 0;

	mpeg2->totalFrames = 0;
	mpeg2->codec_time = 0;
	return;
}

static void
gst_cnmmpeg2dec_init(Gstcnmmpeg2dec *mpeg2, Gstcnmmpeg2decClass *mpeg2_klass)
{
	gcnm_printf("enter %s\n", __func__);

	GstElementClass *klass = GST_ELEMENT_CLASS(mpeg2_klass);

	mpeg2->sinkpad = gst_pad_new_from_template (
		gst_element_class_get_pad_template(klass, "sink"), "sink");

	gst_pad_set_setcaps_function (mpeg2->sinkpad, GST_DEBUG_FUNCPTR (gst_cnmmpeg2dec_sinkpad_setcaps));
	gst_pad_set_chain_function (mpeg2->sinkpad, GST_DEBUG_FUNCPTR (gst_cnmmpeg2dec_chain));
	gst_pad_set_event_function (mpeg2->sinkpad, GST_DEBUG_FUNCPTR (gst_cnmmpeg2dec_sinkpad_event));

	gst_element_add_pad(GST_ELEMENT(mpeg2), mpeg2->sinkpad);

	mpeg2->srcpad = gst_pad_new_from_template (
		gst_element_class_get_pad_template(klass, "src"), "src");

	gst_element_add_pad(GST_ELEMENT(mpeg2), mpeg2->srcpad);

	gst_cnmmpeg2dec_init_members(mpeg2);

	gcnm_printf("Gstcnmmpeg2dec instance 0x%x is inited!!!\n", (unsigned int)mpeg2);
	printf("Gstcnmmpeg2dec instance (0x%x) is inited\n", (unsigned int)mpeg2);

	return;
}

/* EOF */
