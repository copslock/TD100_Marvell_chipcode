/*
Copyright (c) 2009, Marvell International Ltd.
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


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gstmpeg4dec.h"

#include <string.h>

#ifdef DEBUG_PERFORMANCE
#include  <sys/time.h>

        struct timeval tstart, tend;
        struct timezone tz;
#endif
GST_DEBUG_CATEGORY_STATIC (mpeg4dec_debug);
#define GST_CAT_DEFAULT mpeg4dec_debug

#if 0
#define assist_myecho(...)	printf(__VA_ARGS__)
#else
#define assist_myecho(...)
#endif

#define SUCCESSIVE_DEC_ERROR_TOLERANCE	5	//if decoder output successive error reach the threashold, stop decoding

/******************************************************************************
// IPP specific function entries
******************************************************************************/
static void 
IPP_OutputPicture_MPEG4(IppPicture *pSrcPicture, unsigned char * pDst, int iDstYpitch, int iDstUVpitch, int Uoffset, int Voffset)
{
	int i;

	Ipp8u *pSrcY = pSrcPicture->ppPicPlane[0];
	Ipp8u *pSrcU = pSrcPicture->ppPicPlane[1];
	Ipp8u *pSrcV = pSrcPicture->ppPicPlane[2];

	int ystep = pSrcPicture->picPlaneStep[0];
	int ustep = pSrcPicture->picPlaneStep[1];
	int vstep = pSrcPicture->picPlaneStep[2];

	unsigned char* pTarget = pDst;
	for(i = 0; i < pSrcPicture->picHeight; i++) {
		memcpy(pTarget, pSrcY, pSrcPicture->picWidth);
		pSrcY += ystep;
		pTarget += iDstYpitch;
	}

	pTarget = pDst + Uoffset;
	for(i = 0; i < (pSrcPicture->picHeight >> 1); i++) {
		memcpy(pTarget, pSrcU, pSrcPicture->picWidth>>1);
		pSrcU += ustep;
		pTarget += iDstUVpitch;
	}

	pTarget = pDst + Voffset;
	for(i = 0; i < (pSrcPicture->picHeight >> 1); i++) {
		memcpy(pTarget, pSrcV, pSrcPicture->picWidth>>1);
		pSrcV += vstep;
		pTarget += iDstUVpitch;
	}

	return;
}


/******************************************************************************
// GStreamer plug-in element implementation
******************************************************************************/

enum {

	/* fill above with user defined signals */
	LAST_SIGNAL
};

enum {
	ARG_0,
	/* fill below with user defined arguments */

#ifdef DEBUG_PERFORMANCE	
	ARG_TOTALFRAME,	
	ARG_CODECTIME,	
#endif
};

#ifndef DEBUG_DOWN_IS_YV12	//DEBUG_DOWN_IS_YV12 is only defined when debuging
#define MPEG4DECOUT_4CC_STR	"I420"
#else
#define MPEG4DECOUT_4CC_STR	"YV12"
#endif

static GstStaticPadTemplate sink_factory = 
	GST_STATIC_PAD_TEMPLATE ("sink", GST_PAD_SINK, GST_PAD_ALWAYS, 
	//GST_STATIC_CAPS ("ANY")/*
	GST_STATIC_CAPS ("video/mpeg, " 
					 "mpegversion = (int) 4, " 
					 "systemstream = (boolean) FALSE, " 
					 "width = (int) [ 16, 2048 ], " 
					 "height = (int) [ 16, 2048 ], " 
					 "framerate = (fraction) [ 0, MAX ];"
					 "video/x-divx, " 
					 "divxversion = (int) {4, 5};"
					 "video/x-xvid")//*/
					 );

static GstStaticPadTemplate src_factory = 
	GST_STATIC_PAD_TEMPLATE ("src", GST_PAD_SRC, GST_PAD_ALWAYS, 
	GST_STATIC_CAPS ("video/x-raw-yuv," 
					 "format = (fourcc) " MPEG4DECOUT_4CC_STR ", " 
					 "width = (int) [ 16, 2048 ], " 
					 "height = (int) [ 16, 2048 ], " 
					 "framerate = (fraction) [ 0, MAX ]")
					 );


GST_BOILERPLATE (GstMPEG4Dec, gst_mpeg4dec, GstElement, GST_TYPE_ELEMENT);


static int
parse_mpeg4_TIB(unsigned char* p, int len)
{
#define __GETnBITS_InByte(B, Bitoff, N, Code)	{Code = (B<<(24+Bitoff))>>(32-N); Bitoff += N;}
	assist_myecho("parse_mpeg4_TIB is called.\n");
	unsigned int SCode;
	if(len < 9) {
		//at least, 4byte VOL startcode and 34 bits contain vop_time_increment_resolution
		return -1;
	}

	//ISO 14496-2, sec 6.2.3
	//seek video object layer startcode
	SCode = ((unsigned int)p[0]<<16) | ((unsigned int)p[1]<<8) | ((unsigned int)p[2]);
	len -= 3;
	p += 3;
	while(len > 0) {
		SCode = (SCode<<8) | *p++ ;
		len--;
		if((SCode>>4) == (0x00000120>>4)) {
			break;
		}
	}

	if(len < 5) {	//at least, should have 34 bits to contain vop_time_increment_resolution
		return -1;
	}

	if((p[0]&127) == 5 && (p[1]&128) == 0) {	//video_object_type_indication
		return -1;
	}else{
		unsigned int vtir, code, Byte = *++p;
		int time_inc_bits;;
		int bitoff;
		len--;
		if(Byte & 0x40) {	//is_object_layer_identifier
			len--;
			if(len<=0) {return -1;}
			Byte = *++p;
			bitoff = 1;
		}else{
			bitoff = 2;
		}

		__GETnBITS_InByte(Byte, bitoff, 4, code);	//aspect_ratio_info
		if(code == 0xf) {
			len -= 2;
			if(len<=0) {return -1;}
			p += 2;
			Byte = *p;
		}

		__GETnBITS_InByte(Byte, bitoff, 1, code);	//vol_control_parameters
		if(len<3) {return -1;}	//video_object_layer_shape+marker_bit+vop_time_increment_resolution have 19bits at least
		if(code) {
			len--;
			Byte = *++p;
			bitoff = bitoff + 3 - 8;
			__GETnBITS_InByte(Byte, bitoff, 1, code);//vbv_parameters
			if(code) {
				len -= 10;
				if(len<=0) {return -1;}
				p += 10;
				bitoff -= 1;
				Byte = *p;
			}
		}

		//video_object_layer_shape
		code = (((Byte<<8)|p[1])<<(16+bitoff)) >> 30;
		bitoff += 2;
		if(bitoff >= 8) {
			bitoff -= 8;
			len--;
			p++;
		}
		if(code != 0) {	//only support video_object_layer_shape==rectangular
			return -2;
		}

		//vop_time_increment_resolution
		if(len < 3) {
			return -1;
		}
		vtir = (((unsigned int)p[0]<<16)|((unsigned int)p[1]<<8)|(unsigned int)p[2])<<(8+1+bitoff);

		if((vtir>>16) == 0) {
			return -3;
		}
		assist_myecho("parse_mpeg4_TIB parsed vtir %d\n", vtir>>16);
		vtir -= 0x10000;
		for(time_inc_bits = 16; time_inc_bits>0; time_inc_bits--) {
			if(((int)vtir) < 0) {
				break;
			}
			vtir <<= 1;
		}
		if(time_inc_bits == 0) {
			time_inc_bits = 1;
		}

		assist_myecho("parse_mpeg4_TIB() parsed time_inc_bits %d\n", time_inc_bits);
		return time_inc_bits;
	}
}


static gboolean 
gst_mpeg4dec_setcaps (GstPad *pad, GstCaps *caps)
{
	GstMPEG4Dec *mpeg4dec = GST_MPEG4DEC (GST_OBJECT_PARENT (pad));

	int ret = 0;

	int i = 0;
	const gchar *name;
	GstStructure *str = NULL;
	const GValue *value;

	int stru_num = gst_caps_get_size (caps);

	if(mpeg4dec->dec_error){
		return FALSE;  
	}
	
	for (i = 0; i < stru_num; i++) {
		str = gst_caps_get_structure (caps, i);
		name = gst_structure_get_name (str);
		GST_INFO_OBJECT (mpeg4dec, "name: %s\n", name);

		gchar *sstr;

		sstr = gst_structure_to_string (str);
		GST_INFO_OBJECT (mpeg4dec, "%s\n", sstr);
		g_free (sstr);		
	}	

	if ((value = gst_structure_get_value (str, "framerate"))) {
		mpeg4dec->num = gst_value_get_fraction_numerator(value);
		mpeg4dec->den = gst_value_get_fraction_denominator(value);	

		GST_INFO_OBJECT (mpeg4dec, "mpeg4dec->num=%d,mpeg4dec->den=%d\n",mpeg4dec->num,mpeg4dec->den);
		if(mpeg4dec->num !=0 && mpeg4dec->den != 0) {
			mpeg4dec->FrameFixPeriod = gst_util_uint64_scale_int(GST_SECOND, mpeg4dec->den, mpeg4dec->num);
		}
	}
	

	if ((value = gst_structure_get_value (str, "codec_data"))) {
		guint8 *cdata;
		guint csize;
		GstBuffer *buf;

		/* We have codec data, means packetised stream */
		buf = gst_value_get_buffer (value);

		cdata = GST_BUFFER_DATA (buf);
		csize = GST_BUFFER_SIZE (buf);
		mpeg4dec->iTimeIncBits = parse_mpeg4_TIB(cdata, csize);


		mpeg4dec->srcBitStream.pBsBuffer = cdata;
		mpeg4dec->srcBitStream.pBsCurByte = mpeg4dec->srcBitStream.pBsBuffer;
		mpeg4dec->srcBitStream.bsByteLen = csize;
		mpeg4dec->srcBitStream.bsCurBitOffset = 0;

		
		if (!(mpeg4dec->pMPEG4DecState)) {
			int codedwidth, codedheight;
			ret = DecoderInitAlloc_MPEG4Video(&(mpeg4dec->srcBitStream),
				&codedwidth, &codedheight, mpeg4dec->pCBTable, &(mpeg4dec->pMPEG4DecState));
			switch(ret){
			case IPP_STATUS_NOERR:
			case IPP_STATUS_MP4_SHORTHEAD:
				break;
			case IPP_STATUS_NOTSUPPORTED_ERR:
				puts("Stream not supported.");
				goto err;
			case IPP_STATUS_SYNCNOTFOUND_ERR:
				//2009.11.4 bug fix. Sometimes, the demuxer provided codec_data has no valid sync code. Return FALSE under that case isn't suitable, we should let _chain() function has another chance to init mpeg4 decoder.
				mpeg4dec->pMPEG4DecState = NULL;
				return TRUE;
				//puts("Stream error, no sync code.");
				//goto err;
			case IPP_STATUS_ERR:
				puts("Header parse error.");
				goto err;
			default:
				puts("Unknown init error.");
				goto err;
			}

			
			mpeg4dec->totalFrames = 0;

			if (mpeg4dec->pMPEG4DecState) {
				DecodeSendCmd_MPEG4Video(IPPVC_SET_NSCCHECKDISABLE,
					&(mpeg4dec->NSCCheckDisable), NULL, mpeg4dec->pMPEG4DecState);				
			}
		}	

	}
		

	return TRUE;

err:
	mpeg4dec->dec_error = TRUE;
	return FALSE;
	
}

#ifdef DEBUG_PERFORMANCE
static void 
gst_mpeg4dec_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	GstMPEG4Dec* mpeg4dec = GST_MPEG4DEC(object);
	switch (prop_id)
	{
	case ARG_TOTALFRAME:
		g_value_set_int (value, mpeg4dec->totalFrames);
		break;
	case ARG_CODECTIME:
		g_value_set_int64 (value, mpeg4dec->codec_time);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}
#endif

typedef struct{
	GstClockTime TS;
	GstClockTime DU;
}TsDuPair;

static void
refresh_output_TsDu(GstMPEG4Dec* mpeg4dec)
{
	//mpeg4dec max re-order delay is 1 frame, therefore, only the latest 2 timestamp in list is valid
	//clear the redundant timestamp
	while(mpeg4dec->iTsDuListLen > 2) {
		g_slice_free(TsDuPair, mpeg4dec->TsDuList->data);
		mpeg4dec->TsDuList = g_slist_delete_link(mpeg4dec->TsDuList, mpeg4dec->TsDuList);
		mpeg4dec->iTsDuListLen--;
	}

	//1. use the ts in list as current output ts
	if(mpeg4dec->TsDuList != 0) {
		TsDuPair* pTsDu = (TsDuPair*)mpeg4dec->TsDuList->data;
		mpeg4dec->LastOutputTs = pTsDu->TS;
		mpeg4dec->LastOutputDu = pTsDu->DU;
		mpeg4dec->TsDuList = g_slist_delete_link(mpeg4dec->TsDuList, mpeg4dec->TsDuList);
		g_slice_free(TsDuPair, pTsDu);
		mpeg4dec->iTsDuListLen--;
		return;
	}

	//2. use auto increased ts based on the last output ts as current output ts
	if(mpeg4dec->LastOutputTs != GST_CLOCK_TIME_NONE) {
		if(mpeg4dec->FrameFixPeriod != GST_CLOCK_TIME_NONE) {
			mpeg4dec->LastOutputTs += mpeg4dec->FrameFixPeriod;
			mpeg4dec->LastOutputDu = mpeg4dec->FrameFixPeriod;
			return;
		}else if(mpeg4dec->LastOutputDu != GST_CLOCK_TIME_NONE) {
			//reuse the last duration as current duration
			mpeg4dec->LastOutputTs += mpeg4dec->LastOutputDu;
			return;
		}
	}

	//3. use GST_CLOCK_TIME_NONE as current output ts
	mpeg4dec->LastOutputTs = GST_CLOCK_TIME_NONE;
	mpeg4dec->LastOutputDu = GST_CLOCK_TIME_NONE;
	return;
}

static inline void
update_mpeg4dec_srcpad_cap(GstMPEG4Dec* mpeg4dec)
{
	GstCaps *Tmp = gst_caps_new_simple ("video/x-raw-yuv", 
		"format", GST_TYPE_FOURCC, GST_STR_FOURCC (MPEG4DECOUT_4CC_STR), 
		"width", G_TYPE_INT, mpeg4dec->iDownFrameWidth, 
		"height", G_TYPE_INT, mpeg4dec->iDownFrameHeight, 
		"framerate", GST_TYPE_FRACTION,  mpeg4dec->num, mpeg4dec->den,
		NULL);

	gst_pad_set_caps(mpeg4dec->srcpad, Tmp);
	gst_caps_unref(Tmp);

	return;
}

static inline int
calculate_GSTstandard_I420_size(int w, int h)
{
	//gstreamer defined standard I420 layout, see <gst/video/video.h>, gstvideo and gst_video_format_get_size()
	return (GST_ROUND_UP_4(w) + GST_ROUND_UP_4(GST_ROUND_UP_2(w)>>1)) * GST_ROUND_UP_2(h);
}

#define I420_standardLayout_sameas_compactLayout(w, h)	(((w)&7)==0 && ((h)&1)==0)	//if width is 8 align and height is 2 align, the GST standard I420 layout is the same as compact I420 layout

static void
update_downbuf_layout(int w, int h, int category, GstMPEG4Dec *mpeg4dec)
{
	if(category == 0) {
		//we defined compact I420 layout
		mpeg4dec->iDownbufYpitch = w;
		mpeg4dec->iDownbufUVpitch = w>>1;
		mpeg4dec->iDownbufUoffset = mpeg4dec->iDownbufYpitch * h;
		mpeg4dec->iDownbufVoffset = mpeg4dec->iDownbufUoffset + mpeg4dec->iDownbufUVpitch * (h>>1);
		mpeg4dec->iDownbufSz = mpeg4dec->iDownbufVoffset + mpeg4dec->iDownbufUVpitch * (h>>1);

	}else if(category == 1) {
		//gstreamer defined standard I420 layout, see <gst/video/video.h>, gstvideo and gst_video_format_get_size()
		mpeg4dec->iDownbufYpitch = GST_ROUND_UP_4(w);
		mpeg4dec->iDownbufUVpitch = GST_ROUND_UP_4(GST_ROUND_UP_2(w)>>1);
		mpeg4dec->iDownbufUoffset = mpeg4dec->iDownbufYpitch * GST_ROUND_UP_2(h);
		mpeg4dec->iDownbufVoffset = mpeg4dec->iDownbufUoffset + mpeg4dec->iDownbufUVpitch * (GST_ROUND_UP_2(h)>>1);
		mpeg4dec->iDownbufSz = mpeg4dec->iDownbufVoffset + mpeg4dec->iDownbufUVpitch * (GST_ROUND_UP_2(h)>>1);
	}
	return;
}

static GstFlowReturn
_mpeg4_push_data(GstMPEG4Dec *mpeg4dec, int bEos)
{
	GstFlowReturn ret = GST_FLOW_OK;
	GstBuffer *outBuf = NULL;

	if(mpeg4dec->bDropReorderDelayedFrame) {
		//IPP mpeg4 codec has no flush function. After seeking, there is a delayed frame in codec, which is belong to last segment, need to drop it manually.
		mpeg4dec->bDropReorderDelayedFrame = 0;
		return GST_FLOW_OK;
	}

	//update output Ts and Du
	refresh_output_TsDu(mpeg4dec);

	if((mpeg4dec->decPicture.picStatus & 0xff) == 0xff) {
		//it's a duplicated of last frame when B frame is skipped inside of decoder, so needn't to push it to sink
		//printf("skipped frame!!!\n");
		return GST_FLOW_OK;
	}
	ret = gst_pad_alloc_buffer_and_set_caps (mpeg4dec->srcpad, 0, mpeg4dec->iDownbufSz, GST_PAD_CAPS (mpeg4dec->srcpad), &outBuf);
	if(ret == GST_FLOW_OK) {
		//Gstreamer defined I420/YV12 format require pitch should be 4 aligned(see <gst/video/video.h>, gstvideo and gst_video_format_get_size()). xvimagesink and ffdec always obey this rule.
		//However, filesink and fakesink don't require this limit because they don't care the format/layout of buffer.
		//Therefore, we output different buffer layout for different sink.
		//printf("want buf sz %d, return buf sz %d\n", mpeg4dec->iDownbufSz, GST_BUFFER_SIZE(outBuf));
		if(!I420_standardLayout_sameas_compactLayout(mpeg4dec->iDownFrameWidth, mpeg4dec->iDownFrameHeight) && GST_BUFFER_SIZE(outBuf) != (guint)mpeg4dec->iDownbufSz) {
			if((int)GST_BUFFER_SIZE(outBuf) == calculate_GSTstandard_I420_size(mpeg4dec->iDownFrameWidth, mpeg4dec->iDownFrameHeight)) {
				update_downbuf_layout(mpeg4dec->iDownFrameWidth, mpeg4dec->iDownFrameHeight, 1, mpeg4dec);
			}
		}
	}else{
		if(ret != GST_FLOW_WRONG_STATE) {
			//during seeking, sink element often return GST_FLOW_WRONG_STATUS. No need to echo this message
			g_warning("mpeg4dec can not malloc buffer from downstream, ret %d, wanted width %d, height %d, wanted sz %d", ret, mpeg4dec->iDownFrameWidth, mpeg4dec->iDownFrameHeight, mpeg4dec->iDownbufSz);
		}
		GST_WARNING_OBJECT (mpeg4dec, "mpeg4dec can not malloc buffer from downstream, ret %d, wanted width %d, height %d, wanted sz %d", ret, mpeg4dec->iDownFrameWidth, mpeg4dec->iDownFrameHeight, mpeg4dec->iDownbufSz);
		return ret;
	}
			

	/* pack up picture's ROI to GstBuffer */
#ifndef DEBUG_DOWN_IS_YV12
	IPP_OutputPicture_MPEG4(&(mpeg4dec->decPicture), GST_BUFFER_DATA(outBuf), mpeg4dec->iDownbufYpitch, mpeg4dec->iDownbufUVpitch, mpeg4dec->iDownbufUoffset, mpeg4dec->iDownbufVoffset);
#else
	IPP_OutputPicture_MPEG4(&(mpeg4dec->decPicture), GST_BUFFER_DATA(outBuf), mpeg4dec->iDownbufYpitch, mpeg4dec->iDownbufUVpitch, mpeg4dec->iDownbufVoffset, mpeg4dec->iDownbufUoffset);
#endif
	if(bEos == 0) { 
		GST_BUFFER_TIMESTAMP(outBuf) = mpeg4dec->LastOutputTs;
	}else{
		//The last frame timestamp issue
		//For B frame stream, the last frame's timestamp provided by qtdemuxer isn't less than the stop time of current segment data provided by qtdemxer. Therefore, the basesink thinks the last frame exceed the current segment data's range, and ignore the last frame.
		//To fix it, decrease the last frame's timestamp a little. But sometimes, decreasing a little is useless for some stream, for those streams, the last frame's timestamp exceed the segment's duration a lot(those stream is edited by some edit tool, therefore, before the last frame, some frame are eliminated. however, the segment's duration is calculated through the frame count in the segment). To work round, force the last frame's timestamp to GST_CLOCK_TIME_NONE.
		GST_BUFFER_TIMESTAMP(outBuf) = GST_CLOCK_TIME_NONE;
	}
	GST_BUFFER_DURATION(outBuf) = mpeg4dec->LastOutputDu;

	//printf("Ts %lld, Du %lld\n", GST_BUFFER_TIMESTAMP(outBuf), GST_BUFFER_DURATION(outBuf));

	if(mpeg4dec->bNewSegReceivedAfterflush) {
		ret = gst_pad_push (mpeg4dec->srcpad, outBuf);
	}else{
		//If new segment event hasn't received, shouldn't push buffer to sink. Otherwise, sink will throw warnings and work abnormal.
		//Sometimes, user seeking position exceeds the duration of file, then demuxer only fire flush event without newseg event. And demuxer also fire EOS event. At EOS, decoder probably still output some frames, but those frames shouldn't be pushed to sink because no newseg event.
		gst_buffer_unref(outBuf);
		ret = GST_FLOW_OK;
	}
	if (GST_FLOW_OK != ret) {
		//during seeking, sink element often return GST_FLOW_WRONG_STATE. Therefore, log a waning is enough, no need to log a error
		g_warning("The decoded frame did not successfully push out to downstream element, ret is %d", ret);
		GST_WARNING_OBJECT(mpeg4dec, "The decoded frame did not successfully push out to downstream element, ret is %d", ret);
		return ret;
	}

	mpeg4dec->totalFrames ++;
	
	return GST_FLOW_OK;
	
}

#ifdef MPEG4DECGST_FRAMESKIP_ENABLE
static void
log_droprecord_and_calculate_droprate(GstMPEG4Dec *mpeg4dec, int bNominalDropped)
{
	gint iSTRate = mpeg4dec->iShortTermDropRate, iLTRate = mpeg4dec->iLongTermDropRate;
	
	//using MA filter to calculate average drop rate
	iSTRate -= mpeg4dec->DropRecordWnd_S[mpeg4dec->iNextRecordIdx_S];
	iLTRate -= mpeg4dec->DropRecordWnd_L[mpeg4dec->iNextRecordIdx_L];
	mpeg4dec->DropRecordWnd_S[mpeg4dec->iNextRecordIdx_S++] = bNominalDropped;
	iSTRate += bNominalDropped;
	mpeg4dec->DropRecordWnd_L[mpeg4dec->iNextRecordIdx_L++] = bNominalDropped;
	iLTRate += bNominalDropped;

	//ring buffer to implement moving window
	if(mpeg4dec->iNextRecordIdx_S == DROP_REC_SHORTWND_LEN) {
		mpeg4dec->iNextRecordIdx_S = 0;
	}
	if(mpeg4dec->iNextRecordIdx_L == DROP_REC_LONGWND_LEN) {
		mpeg4dec->iNextRecordIdx_L = 0;
	}

	//using atomic operation because this event comes from basesink data processing function, it probably in another thread when queue exists in pipeline.
	SET_INT32_NominalAtomic(mpeg4dec->iShortTermDropRate, iSTRate);
	SET_INT32_NominalAtomic(mpeg4dec->iLongTermDropRate, iLTRate);
	INC_INT32_NominalAtomic(mpeg4dec->iNewQosReportedCnt);

	return;
}
#endif

static gboolean 
gst_mpeg4dec_sinkpad_event (GstPad *pad, GstEvent *event)
{
	GstMPEG4Dec *mpeg4dec = GST_MPEG4DEC (GST_OBJECT_PARENT (pad));
	gboolean eventRet = FALSE;


	//printf("gst_mpeg4dec_sinkpad_event. downstreaming\n");
	switch (GST_EVENT_TYPE (event))
	{
	case GST_EVENT_NEWSEGMENT:
#ifdef MPEG4DECGST_FRAMESKIP_ENABLE
		/*Clean all frame drops footprint for MA filter */
		//reset qos mechanism
		memset(mpeg4dec->DropRecordWnd_S, 0, sizeof(mpeg4dec->DropRecordWnd_S));
		memset(mpeg4dec->DropRecordWnd_L, 0, sizeof(mpeg4dec->DropRecordWnd_L));
		mpeg4dec->iShortTermDropRate = 0;
		mpeg4dec->iLongTermDropRate = 0;
		mpeg4dec->iNewQosReportedCnt = 0;
		mpeg4dec->QosProtect_forNewSeq = QOSLOG_PROTECT_FORNEWSEG; /*No Enable before each seq start*/
#endif
		mpeg4dec->PackedFrameOccur = 0;
		//clear TsDu list
		while(mpeg4dec->iTsDuListLen != 0) {
			g_slice_free(TsDuPair, mpeg4dec->TsDuList->data);
			mpeg4dec->TsDuList = g_slist_delete_link(mpeg4dec->TsDuList, mpeg4dec->TsDuList);
			mpeg4dec->iTsDuListLen--;
		}
		mpeg4dec->LastOutputTs = GST_CLOCK_TIME_NONE;
		mpeg4dec->LastOutputDu = GST_CLOCK_TIME_NONE;

		eventRet = gst_pad_push_event (mpeg4dec->srcpad, event);
		mpeg4dec->bNewSegReceivedAfterflush = 1;
		mpeg4dec->bDropReorderDelayedFrame = 1;
		break;
	case GST_EVENT_EOS:

		/* get the last frame */
		if ((!mpeg4dec->dec_error)&&(mpeg4dec->pMPEG4DecState)) {
			IppCodecStatus codecRet = IPP_STATUS_NOERR;
			GstFlowReturn ret = GST_FLOW_OK;
			mpeg4dec->decPicture.picStatus = 0;	//clear the status
#ifdef DEBUG_PERFORMANCE
			gettimeofday (&tstart, &tz);
#endif
			codecRet = Decode_MPEG4Video(&(mpeg4dec->srcBitStream), &(mpeg4dec->decPicture), mpeg4dec->pMPEG4DecState, 1);
#ifdef DEBUG_PERFORMANCE
			gettimeofday (&tend, &tz);
			mpeg4dec->codec_time += (tend.tv_sec - tstart.tv_sec)*1000000 + (tend.tv_usec - tstart.tv_usec);
#endif

			if (codecRet == IPP_STATUS_NOERR) {
				ret = _mpeg4_push_data(mpeg4dec, 1);
			}			
		}
		GST_DEBUG_OBJECT (mpeg4dec, "Decoding Over, Total %5d frames", mpeg4dec->totalFrames);

		eventRet = gst_pad_push_event (mpeg4dec->srcpad, event);
		break;
	case GST_EVENT_FLUSH_STOP:
		mpeg4dec->bNewSegReceivedAfterflush = 0;
	default:
		eventRet = gst_pad_event_default (pad, event);
		break;
	}

	return eventRet;
}

#ifdef MPEG4DECGST_FRAMESKIP_ENABLE
static gboolean	
gst_mpeg4dec_srcpad_event (GstPad *pad, GstEvent *event)
{
	GstMPEG4Dec *mpeg4dec = GST_MPEG4DEC (GST_OBJECT_PARENT (pad));
	gboolean eventRet = FALSE;
	//printf("srcpad upstreaming event\n");
	switch(GST_EVENT_TYPE (event))
	{
	case GST_EVENT_QOS:
		{
			/*you can enable QosProtect_forNewSeq here*/
			if(mpeg4dec->QosProtect_forNewSeq <= 0){
				GstClockTimeDiff diff;
				int NominalDropRecord = 0;
				/*Diff = CurTime - TimeStamp, < 0 is no loss*/
				gst_event_parse_qos(event, NULL, &diff, NULL);
				if(G_UNLIKELY(diff == 0)) {
					//no frame Drop in sink
					NominalDropRecord = 0;	//when video sink's sync==0, the diff in QoS event reported by video sink is equal to 0
				}else{
#if 0
					if(mpeg4dec->iCurSmartSkipMode < SMARTSKIPMODE_PERFORMANCE_PREFER_MID) {
						NominalDropRecord = diff < -15000000 ? 0 : 1;	//before SMARTSKIPMODE_PERFORMANCE_PREFER_MID, if the frame arrive at the sink only a little precede it's timestamp, we still think it is dropped. Therefore, we could prevent the real frame drop by changing decoder mode in advance.
					// No obvious effect, the most case is a scene changing result in the decoding time increase a lot. Before that scene changing, no premonitor.
					}else{
						NominalDropRecord = diff < 0 ? 0 : 1;
					}
#endif
					NominalDropRecord = (diff < 0) ? 0 : 1;
				}
				log_droprecord_and_calculate_droprate(mpeg4dec, NominalDropRecord);
				//printf("[  Qos] is coming lost [%d] of diff [%lld]\n", NominalDropRecord, diff);
			}else{/*start framedrop after seq is stable*/
				mpeg4dec->QosProtect_forNewSeq --;
			}
		
		}

		eventRet = gst_pad_push_event(mpeg4dec->sinkpad, event);
		break;
	default:
		//printf("[NoQos] is coming: [%x]\n", event);
		eventRet = gst_pad_event_default (pad, event);
		break;
	}

	return eventRet;

}
#endif

static unsigned char* 
seek_vopstartcode(unsigned char* pData, int len)
{
	if(len >= 4) {
		unsigned int code = (pData[0]<<16) | (pData[1]<<8) | (pData[2]);
		len -= 3;
		pData += 3;
		while(len > 0) {
			code = (code<<8) | *pData++ ;
			len--;
			if(code == 0x000001b6) {
				return pData-4;
			}
		}
	}
	return NULL;
}

static gint
compareTSDUfun(gconstpointer a, gconstpointer b)
{
	if(((TsDuPair*)a)->TS >= ((TsDuPair*)b)->TS) {
		return 1;
	}else{
		return -1;
	}
}


#define PACKSTM_SKIPVOP_MAXLEN		15	//15 is just a rough estimation
static int
is_MPEG4_SkipVop(unsigned char* pData, int len, int iTimeIncBitsLen)
{
	if(len > 4 && len <= PACKSTM_SKIPVOP_MAXLEN && iTimeIncBitsLen > 0) {
		//probably, we needn't to check the frame coding type, check the data length is enough
		unsigned char* p = seek_vopstartcode(pData, len);
		if(p != NULL) {
			p += 4;
			len -= (p-pData);
			if(len > 0 && (p[0] >> 6) == 1) {
				//the skip frame for packed stream is always P frame
				//iso 14496-2: sec 6.2.5
				int bitoffset = 2;
				unsigned int modulo_time_base;
				do{
					modulo_time_base = (p[0]<<bitoffset)&0x80;
					bitoffset = (bitoffset+1) & 7;
					if(bitoffset == 0) {
						len--;
						p++;
					}
				}while(modulo_time_base!=0 && len>0);
				bitoffset = bitoffset + 2 + iTimeIncBitsLen;
				if(bitoffset >= (len<<3)) {	//if((len<<3)<bitoffset+1)
					return 0;
				}else{
					return ((p[bitoffset>>3]<<(bitoffset&7))&0x80) == 0;
				}
			}
		}
	}
	return 0;
}


static int
check_whether_PackstreamPBframe(GstMPEG4Dec* mpeg4dec)
{
	unsigned char* pData;
	int len;
	unsigned char* pStartVop;
	pData = mpeg4dec->srcBitStream.pBsCurByte;
	len = mpeg4dec->srcBitStream.bsByteLen - (mpeg4dec->srcBitStream.pBsCurByte - mpeg4dec->srcBitStream.pBsBuffer);
	pData = pData + ((mpeg4dec->srcBitStream.bsCurBitOffset+7)>>3);
	len -= (mpeg4dec->srcBitStream.bsCurBitOffset+7)>>3;
	pStartVop = seek_vopstartcode(pData, len);
	if(pStartVop != NULL) {
		if(len-(pStartVop-pData) > 4 && (pStartVop[4]>>6) == 2) {//B frame
			return 1;
		}
	}

	return 0;
}
#ifdef MPEG4DECGST_FRAMESKIP_ENABLE
const unsigned char _DropFrameTbl_start[5]={1/*0*/,2/*1*/,3/*2*/,4/*3*/,4/*4*/};
const unsigned char _DropFrameTbl_resume[5]={0/*0*/,0/*1*/,1/*2*/,2/*3*/,3/*4*/};
const unsigned char _DropFrameTbl_rate[5]={0/*0*/,25/*1*/,50/*2*/,75/*3*/,100/*4*/};
static int
switch_smartskipmode(GstMPEG4Dec *mpeg4dec)
{
	int newmode  = 0;
	int codecRet = 0;

	if(GET_INT32_NominalAtomic(mpeg4dec->iNewQosReportedCnt) > (DROP_REC_SHORTWND_LEN+SHORTWND_QOSWATCHLEN_ADJUST) && GET_INT32_NominalAtomic(mpeg4dec->iShortTermDropRate) >= PERFORMANCE_LACK) {
			/*
 			* if shortTermFrameDrop , then enable skiping mode of Codec. 			
			* newmode = 0; 0% Drop
			* newmode = 1; 25% Drop
			* newmode = 2; 50%  Drop
			* newmode = 3; 75%  Drop
			* newmode = 4; 100%  Drop
  			*/
			newmode = _DropFrameTbl_start[mpeg4dec->iCurSmartSkipMode];
			if(newmode != mpeg4dec->iCurSmartSkipMode){
				/*
  				printf("[Downgrade]: Mode.Rate [%d].[%d%] ---> [%d].[%d%] \n", 
					 mpeg4dec->iCurSmartSkipMode,_DropFrameTbl_rate[mpeg4dec->iCurSmartSkipMode],
					 newmode, _DropFrameTbl_rate[newmode]);
				*/
				codecRet = DecodeSendCmd_MPEG4Video(IPPVC_SET_SKIPMODE,	&(newmode), NULL, mpeg4dec->pMPEG4DecState);
				mpeg4dec->iCurSmartSkipMode = newmode;	//the default mode
			}
			SET_INT32_NominalAtomic(mpeg4dec->iNewQosReportedCnt, 0);
			
	}else if(GET_INT32_NominalAtomic(mpeg4dec->iNewQosReportedCnt) > (DROP_REC_LONGWND_LEN+LONGWND_QOSWATCHLEN_ADJUST) && GET_INT32_NominalAtomic(mpeg4dec->iLongTermDropRate) <= PERFORMANCE_RICH) {
			newmode = _DropFrameTbl_resume[mpeg4dec->iCurSmartSkipMode];
			if(newmode != mpeg4dec->iCurSmartSkipMode){
				/*
  				printf("[Upgrade  ]: Mode.Rate [%d].[%d%] ---> [%d].[%d%] \n", 
					 mpeg4dec->iCurSmartSkipMode,_DropFrameTbl_rate[mpeg4dec->iCurSmartSkipMode],
					 newmode, _DropFrameTbl_rate[newmode]);
				*/
				codecRet = DecodeSendCmd_MPEG4Video(IPPVC_SET_SKIPMODE, &(newmode), NULL, mpeg4dec->pMPEG4DecState);
				mpeg4dec->iCurSmartSkipMode = newmode;	//the default mode
			}
			SET_INT32_NominalAtomic(mpeg4dec->iNewQosReportedCnt, 0);
		//	codecRet = DecodeSendCmd_MPEG4Video(IPPVC_SET_NSCCHECKDISABLE, 
		//		&(mpeg4dec->NSCCheckDisable), NULL, mpeg4dec->pMPEG4DecState);
	}
		//increase the smart mode
	return 0;
}
#endif

static GstFlowReturn 
gst_mpeg4dec_chain (GstPad *pad, GstBuffer *buf)
{
	GstMPEG4Dec *mpeg4dec = GST_MPEG4DEC (GST_OBJECT_PARENT (pad));
	GstFlowReturn ret = GST_FLOW_OK;
	IppCodecStatus codecRet = IPP_STATUS_NOERR; 

	guint8 *data = GST_BUFFER_DATA (buf);
	int data_size = GST_BUFFER_SIZE (buf);
	GST_LOG_OBJECT(mpeg4dec, "input buffer, size %d,ts %lld", data_size, GST_BUFFER_TIMESTAMP(buf));
	if(data_size == 1 && *data == 0x7f) {
		//stuffing bits, spec 14496-2: sec 5.2.3, 5.2.4
		return GST_FLOW_OK;
	}
	if(mpeg4dec->iTimeIncBits < 0) {
		mpeg4dec->iTimeIncBits = parse_mpeg4_TIB(data, data_size);
	}

	//reorder timestamp and insert it into list
	if(GST_BUFFER_TIMESTAMP(buf) != GST_CLOCK_TIME_NONE) {		
		TsDuPair* pTsDu = g_slice_new(TsDuPair);
		pTsDu->TS = GST_BUFFER_TIMESTAMP(buf);
		pTsDu->DU = GST_BUFFER_DURATION(buf);
		mpeg4dec->TsDuList = g_slist_insert_sorted(mpeg4dec->TsDuList, pTsDu, compareTSDUfun);
		mpeg4dec->iTsDuListLen++;
	}

	if(mpeg4dec->PackedFrameOccur > 0) {
		if(is_MPEG4_SkipVop(data, data_size, mpeg4dec->iTimeIncBits)) {
			mpeg4dec->PackedFrameOccur--;
			return GST_FLOW_OK;
		}
	}

	mpeg4dec->srcBitStream.pBsBuffer = GST_BUFFER_DATA (buf);
	mpeg4dec->srcBitStream.pBsCurByte = mpeg4dec->srcBitStream.pBsBuffer;
	mpeg4dec->srcBitStream.bsByteLen = GST_BUFFER_SIZE (buf);
	mpeg4dec->srcBitStream.bsCurBitOffset = 0;	

	if (!(mpeg4dec->pMPEG4DecState)) {
		int codedwidth, codedheight;
		codecRet = DecoderInitAlloc_MPEG4Video(&(mpeg4dec->srcBitStream), 
			&codedwidth, &codedheight, mpeg4dec->pCBTable, &(mpeg4dec->pMPEG4DecState));
		mpeg4dec->totalFrames = 0;
		mpeg4dec->errcnt = 0;//clear error count
		

		if (codecRet != IPP_STATUS_NOERR) {				
			
			GST_ERROR_OBJECT (mpeg4dec, "mpeg4 init error");			
			goto err;
		}

		if (mpeg4dec->pMPEG4DecState) {
			codecRet = DecodeSendCmd_MPEG4Video(IPPVC_SET_NSCCHECKDISABLE, 
				&(mpeg4dec->NSCCheckDisable), NULL, mpeg4dec->pMPEG4DecState);
			
		}
	}	

#ifdef MPEG4DECGST_FRAMESKIP_ENABLE
	/*change frame droping stragety*/
	switch_smartskipmode(mpeg4dec);
#endif

	if (mpeg4dec->pMPEG4DecState) {			
		mpeg4dec->decPicture.picStatus = 0;	//clear the status
#ifdef DEBUG_PERFORMANCE
		gettimeofday (&tstart, &tz);
#endif
		codecRet = Decode_MPEG4Video(&(mpeg4dec->srcBitStream), &(mpeg4dec->decPicture), mpeg4dec->pMPEG4DecState, 0);
#ifdef DEBUG_PERFORMANCE
		gettimeofday (&tend, &tz);
		mpeg4dec->codec_time += (tend.tv_sec - tstart.tv_sec)*1000000 + (tend.tv_usec - tstart.tv_usec);
#endif

		/* codec delay 1 frame for reorder, so the first time has no data, and timestamp should reorder */
		if(mpeg4dec->firsttime){
			mpeg4dec->firsttime = FALSE;
			mpeg4dec->bDropReorderDelayedFrame = 0;	//dropped 1st frame
			if((mpeg4dec->decPicture.picWidth&1) != 0 || (mpeg4dec->decPicture.picHeight&1) != 0) {
				g_warning("The display frame width or height isn't even, w is %d, h is %d", mpeg4dec->decPicture.picWidth, mpeg4dec->decPicture.picHeight);
				GST_WARNING_OBJECT(mpeg4dec, "The display frame width or height isn't even, w is %d, h is %d", mpeg4dec->decPicture.picWidth, mpeg4dec->decPicture.picHeight);
			}
			//at first, we try our defined compact I420 layout, not standard gstreamer defined I420 layout in <gst/video/video.h>
			mpeg4dec->iDownFrameWidth = mpeg4dec->decPicture.picWidth;
			mpeg4dec->iDownFrameHeight = mpeg4dec->decPicture.picHeight;
			update_downbuf_layout(mpeg4dec->iDownFrameWidth, mpeg4dec->iDownFrameHeight, 0, mpeg4dec);

			/* set srcpad caps */
			update_mpeg4dec_srcpad_cap(mpeg4dec);
			
		}else{
		
			if (codecRet == IPP_STATUS_NOERR) {
				mpeg4dec->errcnt = 0;//clear error count
				ret = _mpeg4_push_data(mpeg4dec, 0);
				if (GST_FLOW_OK != ret) {
					GST_WARNING_OBJECT(mpeg4dec, "push err %d!", ret);
					goto push_err;
				}

				if(check_whether_PackstreamPBframe(mpeg4dec)) {
					mpeg4dec->PackedFrameOccur ++;
					mpeg4dec->decPicture.picStatus = 0;	//clear the status
#ifdef DEBUG_PERFORMANCE
					gettimeofday(&tstart, &tz);
#endif
					codecRet = Decode_MPEG4Video(&(mpeg4dec->srcBitStream), &(mpeg4dec->decPicture), mpeg4dec->pMPEG4DecState, 0);
#ifdef DEBUG_PERFORMANCE
					gettimeofday(&tend, &tz);
					mpeg4dec->codec_time += (tend.tv_sec - tstart.tv_sec)*1000000 + (tend.tv_usec - tstart.tv_usec);
#endif
					if(codecRet == IPP_STATUS_NOERR) {
						mpeg4dec->errcnt = 0;//clear error count
						ret = _mpeg4_push_data(mpeg4dec, 0);
						if(GST_FLOW_OK != ret) {
							GST_WARNING_OBJECT(mpeg4dec, "push err %d!", ret);
							goto push_err;
						}
					}else{
						GST_WARNING_OBJECT (mpeg4dec, "codec decode return error %d!", codecRet);
						mpeg4dec->errcnt += 1;
						if(mpeg4dec->errcnt >= SUCCESSIVE_DEC_ERROR_TOLERANCE){  //continuous error frames
							GST_ERROR_OBJECT(mpeg4dec, "codec decode accumulated errors reach %d!", SUCCESSIVE_DEC_ERROR_TOLERANCE);
							goto err;
						}
					}
				}


				
			} else if (codecRet != IPP_STATUS_ERR) {
				GST_WARNING_OBJECT(mpeg4dec, "codec decode return error %d!", codecRet);
				mpeg4dec->errcnt += 1;
				if(mpeg4dec->errcnt >= SUCCESSIVE_DEC_ERROR_TOLERANCE){  //continuous error frames
					GST_ERROR_OBJECT(mpeg4dec, "codec decode accumulated errors reach %d!", SUCCESSIVE_DEC_ERROR_TOLERANCE);
					goto err;
				}
			}
		}		

	}

	gst_buffer_unref (buf);	
	
	return GST_FLOW_OK;
	
push_err:
	gst_buffer_unref (buf);
	if(ret < GST_FLOW_UNEXPECTED) {
		GST_ELEMENT_ERROR(mpeg4dec, STREAM, DECODE, (NULL), ("%s() return un-accepted error %d when pushing", __FUNCTION__, ret));
	}

	return ret;


err:
	mpeg4dec->dec_error = TRUE;
	gst_buffer_unref(buf);
	GST_ELEMENT_ERROR(mpeg4dec, STREAM, DECODE, (NULL), ("%s() return un-accepted error %d", __FUNCTION__, GST_FLOW_ERROR));
	
	return GST_FLOW_ERROR;

}

static gboolean
gst_mpeg4dec_start (GstMPEG4Dec *mpeg4dec)
{
	mpeg4dec->pCBTable = NULL;
	mpeg4dec->pMPEG4DecState = NULL;
	mpeg4dec->NSCCheckDisable = 1;

	mpeg4dec->totalFrames = 0;
	mpeg4dec->codec_time = 0;
	mpeg4dec->errcnt = 0;


	mpeg4dec->num = 0;
	mpeg4dec->den = 1;
	mpeg4dec->FrameFixPeriod = GST_CLOCK_TIME_NONE;
	
	mpeg4dec->firsttime = TRUE;
	mpeg4dec->dec_error = FALSE;

	miscInitGeneralCallbackTable (&(mpeg4dec->pCBTable));	

	return TRUE;
}

static gboolean
gst_mpeg4dec_stop (GstMPEG4Dec *mpeg4dec)
{
	if (mpeg4dec->pMPEG4DecState) {
		DecoderFree_MPEG4Video(&(mpeg4dec->pMPEG4DecState));
		mpeg4dec->pMPEG4DecState = NULL;
	}
	miscFreeGeneralCallbackTable (&(mpeg4dec->pCBTable));	

	return TRUE;
	
}

static GstStateChangeReturn 
gst_mpeg4dec_change_state (GstElement *element, GstStateChange transition)
{
	GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;
	GstMPEG4Dec *mpeg4dec = GST_MPEG4DEC (element);

	switch (transition)
	{
	case GST_STATE_CHANGE_NULL_TO_READY:
		GST_INFO_OBJECT (mpeg4dec, "mpeg4dec GST_STATE_CHANGE_NULL_TO_READY");	

		if(!gst_mpeg4dec_start(mpeg4dec)){
			goto statechange_failed;
		}	
		
		break;
	case GST_STATE_CHANGE_READY_TO_PAUSED:
		mpeg4dec->iTimeIncBits = -1;	//prepare to decode a new stream
		break;
			
	default:
		break;
	}

	ret = parent_class->change_state (element, transition);
	if (ret == GST_STATE_CHANGE_FAILURE){
		return ret;
	}

	switch (transition)
	{
	case GST_STATE_CHANGE_READY_TO_NULL:
		GST_INFO_OBJECT (mpeg4dec, "mpeg4dec GST_STATE_CHANGE_READY_TO_NULL");
		
		if(!gst_mpeg4dec_stop(mpeg4dec)){
			goto statechange_failed;
		}        

		break;		
	default:
		break;
	}

	return ret;

statechange_failed:
	{
		/* subclass must post a meaningfull error message */
		GST_ERROR_OBJECT (mpeg4dec, "state change failed");
		return GST_STATE_CHANGE_FAILURE;
	}
}

static void 
gst_mpeg4dec_base_init (gpointer g_klass)
{
	static GstElementDetails mpeg4dec_details = {
		"MPEG-4 Video Decoder", 
		"Codec/Decoder/Video", 
		"MPEG-4 Video Decoder based-on IPP & CODEC", 
		""
	};
	GstElementClass *element_class = GST_ELEMENT_CLASS (g_klass);
	gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&src_factory));
	gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&sink_factory));

	gst_element_class_set_details (element_class, &mpeg4dec_details);
}

static void 
gst_mpeg4dec_class_init (GstMPEG4DecClass *klass)
{
	GstElementClass *gstelement_class = (GstElementClass*)klass;

#ifdef DEBUG_PERFORMANCE
	GObjectClass *gobject_class  = (GObjectClass*)klass;

	gobject_class->get_property = gst_mpeg4dec_get_property;

	g_object_class_install_property (gobject_class, ARG_TOTALFRAME,
  		g_param_spec_int ("totalframes", "Totalframe of mpeg4stream",
          	"Number of total pushed frame to downstream", 0, G_MAXINT, 0, G_PARAM_READABLE));
	g_object_class_install_property (gobject_class, ARG_CODECTIME,
  		g_param_spec_int64 ("codectime", "mpeg4 decode time",
          	"codec time (microsecond)", 0, G_MAXINT64, 0, G_PARAM_READABLE));
#endif
	gstelement_class->change_state = GST_DEBUG_FUNCPTR(gst_mpeg4dec_change_state);

	GST_DEBUG_CATEGORY_INIT (mpeg4dec_debug, "mpeg4dec", 0, "MPEG-4 Decode Element");
}


static void 
gst_mpeg4dec_init (GstMPEG4Dec *mpeg4dec, GstMPEG4DecClass *mpeg4dec_klass)
{
	GstElementClass *klass = GST_ELEMENT_CLASS (mpeg4dec_klass);

	mpeg4dec->sinkpad = gst_pad_new_from_template (
		gst_element_class_get_pad_template (klass, "sink"), "sink");
	
	gst_pad_set_setcaps_function (mpeg4dec->sinkpad, GST_DEBUG_FUNCPTR (gst_mpeg4dec_setcaps));
	gst_pad_set_event_function (mpeg4dec->sinkpad, GST_DEBUG_FUNCPTR (gst_mpeg4dec_sinkpad_event));

	gst_pad_set_chain_function (mpeg4dec->sinkpad, GST_DEBUG_FUNCPTR (gst_mpeg4dec_chain));

	gst_element_add_pad (GST_ELEMENT (mpeg4dec), mpeg4dec->sinkpad);

	mpeg4dec->srcpad = gst_pad_new_from_template (gst_element_class_get_pad_template (klass, "src"), "src");
	gst_element_add_pad (GST_ELEMENT (mpeg4dec), mpeg4dec->srcpad);
	/*Registe srcpad event handle*/
#ifdef MPEG4DECGST_FRAMESKIP_ENABLE
	gst_pad_set_event_function(mpeg4dec->srcpad, GST_DEBUG_FUNCPTR (gst_mpeg4dec_srcpad_event));
#endif
	mpeg4dec->iTsDuListLen = 0;
	mpeg4dec->TsDuList = NULL;

	mpeg4dec->iTimeIncBits = -1;

	mpeg4dec->bNewSegReceivedAfterflush = 0;
	mpeg4dec->bDropReorderDelayedFrame = 0;

#ifdef MPEG4DECGST_FRAMESKIP_ENABLE
	/*Init MA_FRAMEDROP*/
	mpeg4dec->QosProtect_forNewSeq = QOSLOG_PROTECT_FORNEWSEG;
	mpeg4dec->iNewQosReportedCnt   = 0;
	mpeg4dec->iShortTermDropRate   = 0;
	mpeg4dec->iLongTermDropRate    = 0;
	mpeg4dec->iNextRecordIdx_S     = 0;
	mpeg4dec->iNextRecordIdx_L     = 0;
	mpeg4dec->iCurSmartSkipMode    = 0;
	memset(mpeg4dec->DropRecordWnd_S, 0, sizeof(mpeg4dec->DropRecordWnd_S));
	memset(mpeg4dec->DropRecordWnd_L, 0, sizeof(mpeg4dec->DropRecordWnd_L));
#endif

}

static gboolean 
plugin_init (GstPlugin *plugin)
{	
	return gst_element_register (plugin, "mpeg4dec", GST_RANK_PRIMARY+2, GST_TYPE_MPEG4DEC);
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR, 
                   GST_VERSION_MINOR, 
                   "mvl_mpeg4dec", 
                   "mpeg4 decoder based on IPP, "__DATE__, 
                   plugin_init, 
                   VERSION, 
                   "LGPL",
                   "", 
                   "");




/* EOF */
