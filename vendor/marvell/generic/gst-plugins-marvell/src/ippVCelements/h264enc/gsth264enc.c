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

#include "gsth264enc.h"

#include <string.h>
#ifdef DEBUG_PERFORMANCE
#include <sys/time.h>
#include <time.h>
struct timeval EncAPI_clk0, EncAPI_clk1;
#endif

GST_DEBUG_CATEGORY_STATIC (ipph264enc_debug);
#define GST_CAT_DEFAULT ipph264enc_debug


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
	ARG_BIT_RATE,
	ARG_QUALITY_LEVEL,
	ARG_FORCE_STARTTIME,

#ifdef DEBUG_PERFORMANCE
	ARG_TOTALFRAME,
	ARG_CODECTIME,
#endif
};

static GstStaticPadTemplate sink_factory = \
	GST_STATIC_PAD_TEMPLATE ("sink", GST_PAD_SINK, GST_PAD_ALWAYS, \
	GST_STATIC_CAPS ("video/x-raw-yuv," \
					 "format = (fourcc) I420, " \
					 "framerate = (fraction) [ 1/1, 60/1 ], " \
					 "width = (int) [ 16, 2048 ], " \
					 "height = (int) [ 16, 2048 ]" \
					 ));

static GstStaticPadTemplate src_factory = \
	GST_STATIC_PAD_TEMPLATE ("src", GST_PAD_SRC, GST_PAD_ALWAYS, \
	GST_STATIC_CAPS ("video/x-h264, " \
					 "width = (int) [ 16, 2048 ], " \
					 "height = (int) [ 16, 2048 ], " \
					 "framerate = (fraction) [ 1/1, 60/1 ]"));

GST_BOILERPLATE (GstIppH264Enc, gst_ipph264enc, GstElement, GST_TYPE_ELEMENT);

#define DEFAULT_TARGETBITRATE	300000
#define DEFAULT_QUALITYLEVEL	2


#if 0
#define assist_myecho(...)	printf(__VA_ARGS__)
#else
#define assist_myecho(...)
#endif

#define IPPGST_H264ENC_IS_8ALIGN(x)		(((unsigned int)(x)&7) == 0)
#define IPPGST_H264ENC_IS_16ALIGN(x)		(((unsigned int)(x)&15) == 0)


static int
IPP_videoCheckPar_H264Enc(int w, int h, int FR, int bR, int iQL)
{
	if(! IPPGST_H264ENC_IS_16ALIGN(w)) {
		g_warning("src image width %d isn't 16 aligned!", w);
		return -1;
	}
	if(! IPPGST_H264ENC_IS_16ALIGN(h)) {
		g_warning("src image height %d isn't 16 aligned!", h);
		return -2;
	}
	if(FR <= 0) {
		g_warning("src framerate %d exceeds range, and don't support variable framerate!", FR);
		return -3;
	}
	if(bR <= 0) {
		g_warning("target bitrate %d exceeds range!", bR);
		return -4;
	}
	if(iQL < 1 || iQL > 3) {
		g_warning("QualityLevel %d exceeds range<1~3>!", iQL);
		return -5;
	}
	return 0;
}



static void 
IPP_SetEncPar_H264Enc(GstIppH264Enc *h264enc)
{
	assist_myecho("Set h264 encoder parameter, w %d, h %d, FR %d, bR %d, QL %d\n", h264enc->iEncW, h264enc->iEncH, h264enc->iEncFrameRate, h264enc->iTargtBitRate, h264enc->iQualityLevel);
	h264enc->H264Enc_Par.iWidth = h264enc->iEncW;
	h264enc->H264Enc_Par.iHeight = h264enc->iEncH;
	h264enc->H264Enc_Par.iFrameRate = h264enc->iEncFrameRate;
	h264enc->H264Enc_Par.iPBetweenI = h264enc->iEncFrameRate - 1;
	h264enc->H264Enc_Par.iQpFirstFrame = 30;
	h264enc->H264Enc_Par.bRCEnable = 1;
	h264enc->H264Enc_Par.iRCBitRate = h264enc->iTargtBitRate;
	h264enc->H264Enc_Par.iRCMaxBitRate = (h264enc->iTargtBitRate)<<1;
	h264enc->H264Enc_Par.iDelayLimit = 1000;
	h264enc->H264Enc_Par.nQualityLevel = h264enc->iQualityLevel;
	return;
}



static inline int
calculate_GSTstandard_I420_size(int w, int h)
{
	//gstreamer defined standard I420 layout, see <gst/video/video.h>, gstvideo and gst_video_format_get_size()
	return (GST_ROUND_UP_4(w) + GST_ROUND_UP_4(GST_ROUND_UP_2(w)>>1)) * GST_ROUND_UP_2(h);
}

static int
set_srcframe_layout(GstIppH264Enc* h264enc, int w, int h)
{
	//currently, h264 encoder only accept 16 aligned width and height, therefore, Y pitch and UV pitch are all 8 aligned. So if the first line of Y plannar is 8 aligned, each line is 8 aligned
	//And we only support GST standard I420 layout
	h264enc->i1SrcFrameDataLen = calculate_GSTstandard_I420_size(w, h);
	h264enc->H264Enc_SrcImage.picPlaneStep[0] = GST_ROUND_UP_4(w);
	h264enc->H264Enc_SrcImage.picPlaneStep[1] = GST_ROUND_UP_4(GST_ROUND_UP_2(w)>>1);
	h264enc->H264Enc_SrcImage.picPlaneStep[2] = h264enc->H264Enc_SrcImage.picPlaneStep[1];

	h264enc->iSrcUoffset = h264enc->H264Enc_SrcImage.picPlaneStep[0] * GST_ROUND_UP_2(h);
	h264enc->iSrcVoffset = h264enc->iSrcUoffset + h264enc->H264Enc_SrcImage.picPlaneStep[1] * (GST_ROUND_UP_2(h)>>1);

	h264enc->H264Enc_SrcImage.picPlaneNum = 3;
	h264enc->H264Enc_SrcImage.picWidth = h264enc->iEncW;
	h264enc->H264Enc_SrcImage.picHeight = h264enc->iEncH;

	return 0;
}

static gboolean 
gst_ipph264enc_sinkpad_setcaps(GstPad *pad, GstCaps *caps)
{
	GstIppH264Enc *h264enc = GST_IPPH264ENC (GST_OBJECT_PARENT (pad));
#if 0
	gchar* capstring = gst_caps_to_string(caps);
	assist_myecho("caps is %s\n", capstring==NULL? "none" : capstring);
	if(capstring) {
		g_free(capstring);
	}
#endif

	guint stru_num = gst_caps_get_size (caps);
	if(stru_num == 0) {
		g_warning("No content in h264enc sinkpad setcaps!");
		GST_ERROR_OBJECT(h264enc, "No content in h264enc sinkpad setcaps!");
		return FALSE;
	}
	

	GstStructure *str = gst_caps_get_structure (caps, 0);

	const char *mime = gst_structure_get_name (str);
	if (strcmp (mime, "video/x-raw-yuv") != 0) {
		g_warning("The cap mime type %s isn't video/x-raw-yuv!", mime);
		GST_ERROR_OBJECT(h264enc, "The cap mime type %s isn't video/x-raw-yuv!", mime);
		return FALSE;
	}

	guint32 fourcc = 0;
	if( FALSE == gst_structure_get_fourcc(str, "format", &fourcc) || fourcc != GST_STR_FOURCC("I420") ) {
		g_warning("Couldn't found I420 fourcc in setcaps!");
		GST_ERROR_OBJECT(h264enc, "Couldn't found I420 fourcc in setcaps!");
		return FALSE;
	}


	if (FALSE == gst_structure_get_int(str, "width", &(h264enc->iEncW))) {
		g_warning("Couldn't get src image width in setcaps!");
		GST_ERROR_OBJECT(h264enc, "Couldn't get src image width in gstcaps!");
		return FALSE;
	}

	if (FALSE == gst_structure_get_int(str, "height", &(h264enc->iEncH))) {
		g_warning("Couldn't get src image height in setcaps!");
		GST_ERROR_OBJECT(h264enc, "Couldn't get src image height in gstcaps!");
		return FALSE;
	}

	int iFRateNum, iFRateDen;
	if (FALSE == gst_structure_get_fraction(str, "framerate", &iFRateNum, &iFRateDen)) {
		g_warning("Couldn't get framerate in setcaps!");
		GST_ERROR_OBJECT(h264enc, "Couldn't get framerate in setcaps!");
		return FALSE;
	}
	h264enc->iEncFrameRate = iFRateNum/iFRateDen;
	if(iFRateDen * h264enc->iEncFrameRate != iFRateNum) {	//current h264 encoder only support interger fps
		g_warning("Couldn't support fractional framerate (%d)div(%d) fps!", iFRateNum, iFRateDen);
		GST_ERROR_OBJECT(h264enc, "Couldn't support fractional framerate (%d)div(%d) fps!", iFRateNum, iFRateDen);
		return FALSE;
	}
	if(h264enc->iEncFrameRate) {
		h264enc->iFixedFRateDuration = gst_util_uint64_scale_int(GST_SECOND, 1, h264enc->iEncFrameRate);
	}
	
	if( 0 != IPP_videoCheckPar_H264Enc(h264enc->iEncW, h264enc->iEncH, h264enc->iEncFrameRate, h264enc->iTargtBitRate, h264enc->iQualityLevel) ) {
		GST_ERROR_OBJECT(h264enc, "The parameter for encoder is wrong!!!");
		return FALSE;
	}
	
	GstCaps *TmpCap;
	TmpCap = gst_caps_new_simple ("video/x-h264", 
		"width", G_TYPE_INT, h264enc->iEncW, 
		"height", G_TYPE_INT, h264enc->iEncH, 
		"framerate", GST_TYPE_FRACTION,  h264enc->iEncFrameRate, 1,
		NULL);
	if(!gst_pad_set_caps(h264enc->srcpad, TmpCap)) {
		gst_caps_unref(TmpCap);
		g_warning("Set caps on srcpad fail!");
		GST_ERROR_OBJECT(h264enc, "Set caps on srcpad fail!");
		return FALSE;
	}
	gst_caps_unref(TmpCap);

	set_srcframe_layout(h264enc, h264enc->iEncW, h264enc->iEncH);

	IPP_SetEncPar_H264Enc(h264enc);

	return TRUE;

}


static void 
gst_ipph264enc_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	GstIppH264Enc *h264enc = GST_IPPH264ENC (object);


	switch (prop_id)
	{
	case ARG_BIT_RATE:
		{
			int bR = g_value_get_int(value);
			if(bR <=0) {
				g_warning("bitrate %d exceed range!", bR);
			}else{
				h264enc->iTargtBitRate = bR;
			}
		}
		break;

	case ARG_QUALITY_LEVEL:
		{
			int QL = g_value_get_int(value);
			if(QL < 1 || QL > 3) {
				g_warning("qualitylevel %d exceed range!", QL);
			}else{
				h264enc->iQualityLevel = QL;
			}
		}
		break;

	case ARG_FORCE_STARTTIME:
		{
			gint64 st = g_value_get_int64(value);
			if(st<-1) {
				g_warning("force-starttime %lld exceed range!", st);
			}else{
				h264enc->ForceStartTimestamp = st;
			}
		}
		break;

	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}

	return;
}

static void 
gst_ipph264enc_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	GstIppH264Enc *h264enc = GST_IPPH264ENC (object);

	switch (prop_id)
	{
	case ARG_BIT_RATE:
		g_value_set_int (value, h264enc->iTargtBitRate);
		break;
	case ARG_QUALITY_LEVEL:
		g_value_set_int (value, h264enc->iQualityLevel);
		break;
	case ARG_FORCE_STARTTIME:
		g_value_set_int64(value, h264enc->ForceStartTimestamp);
		break;

#ifdef DEBUG_PERFORMANCE
	case ARG_TOTALFRAME:
		g_value_set_int (value, h264enc->iEncOutputFrameCnt);
		break;
	case ARG_CODECTIME:
		g_value_set_int64 (value, h264enc->codec_time);
		break;
#endif
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
	return;
}



static int
init_bitsbuf_and_encoder(GstIppH264Enc* h264enc)
{
	assist_myecho("----------Init h264 raw encoder----------\n");
#define IPPGST_MAX_SPS_SIZE 25
#define IPPGST_MAX_PPS_SIZE	12

	IppCodecStatus codecret;
	h264enc->iDstBitsBufUnitLen = (h264enc->iEncW * h264enc->iEncH)/10;

	if(h264enc->iDstBitsBufUnitLen <= IPPGST_MAX_SPS_SIZE+IPPGST_MAX_PPS_SIZE) {
		h264enc->iDstBitsBufUnitLen = IPPGST_MAX_SPS_SIZE+IPPGST_MAX_PPS_SIZE + 1;
	}

	h264enc->H264Enc_DstBitsBuf.pBsBuffer = g_malloc(h264enc->iDstBitsBufUnitLen);

	if(h264enc->H264Enc_DstBitsBuf.pBsBuffer == NULL) {
		g_warning("Allocate target bit stream buffer(wanted size %d) fail", h264enc->iDstBitsBufUnitLen);
		GST_ERROR_OBJECT(h264enc, "Allocate target bit stream buffer(wanted size %d) fail", h264enc->iDstBitsBufUnitLen);
		return -1;
	}
	h264enc->H264Enc_DstBitsBuf.bsByteLen = h264enc->iDstBitsBufUnitLen;
	h264enc->H264Enc_DstBitsBuf.pBsCurByte = h264enc->H264Enc_DstBitsBuf.pBsBuffer;

	codecret = EncoderInitAlloc_H264Video(&h264enc->H264Enc_MemCBTable, &h264enc->H264Enc_Par, &h264enc->H264Enc_DstBitsBuf, &h264enc->H264Enc_Handle, NULL);
	if(codecret != IPP_STATUS_NOERR) {
		g_free(h264enc->H264Enc_DstBitsBuf.pBsBuffer);
		h264enc->H264Enc_DstBitsBuf.pBsBuffer  = NULL;
		g_warning("Init h264 encoder fail, ret %d", codecret);
		GST_ERROR_OBJECT(h264enc, "Init h264 encoder fail, ret %d", codecret);
		return -2;
	}
	return 0;
}

static int
copy_frame_to_8alignbuf(GstIppH264Enc* h264enc, unsigned char* pSrc)
{
	if(h264enc->pBuf8Aligned == NULL) {
		assist_myecho("Create 8 aligned frame buffer for transforming non 8 align to 8 align!\n");
		IPP_MemMalloc((void**)&h264enc->pBuf8Aligned, h264enc->i1SrcFrameDataLen, 8);
		if(h264enc->pBuf8Aligned == NULL) {
			g_warning("Allocate 8 aligned frame buffer fail!");
			GST_ERROR_OBJECT(h264enc, "Allocate 8 aligned frame buffer fail!");
			return -1;
		}
	}
	memcpy(h264enc->pBuf8Aligned, pSrc, h264enc->i1SrcFrameDataLen);
	return 0;
}

static GstFlowReturn 
gst_ipph264enc_chain(GstPad *pad, GstBuffer *buf)
{
	GstIppH264Enc *h264enc = GST_IPPH264ENC (GST_OBJECT_PARENT (pad));
	unsigned char* pInputImgData;
	int dataleftlen;
	GstFlowReturn pushret = GST_FLOW_OK;
	IppCodecStatus codecret;
	
	//assist_myecho("Input buffer sz %d, TS %lld, DU %lld\n", GST_BUFFER_SIZE(buf), GST_BUFFER_TIMESTAMP(buf), GST_BUFFER_DURATION(buf));

	if(h264enc->H264Enc_Par.iWidth == 0) {
		g_warning("encoder parameter is still unset in chain()");
		GST_ERROR_OBJECT(h264enc, "encoder parameter is still unset in chain()");
		goto H264ENC_CHAIN_FATALERR;
	}

	if(h264enc->H264Enc_Handle == NULL) {
		if(init_bitsbuf_and_encoder(h264enc) != 0) {
			goto H264ENC_CHAIN_FATALERR;
		}
		assist_myecho("Input data %s streaming mode: input fragment len %d, one srcframe len %d\n", (GST_BUFFER_SIZE(buf)==h264enc->i1SrcFrameDataLen? "is":"isn't"), GST_BUFFER_SIZE(buf), h264enc->i1SrcFrameDataLen);
	}

	if(h264enc->ForceStartTimestamp != -1) {
		if(h264enc->PushdownTsCandidate == GST_CLOCK_TIME_NONE) {
			//the first frame
			h264enc->PushdownTsCandidate = h264enc->ForceStartTimestamp;
		}
	}else{
		if(GST_BUFFER_TIMESTAMP(buf) != GST_CLOCK_TIME_NONE) {
			h264enc->PushdownTsCandidate = GST_BUFFER_TIMESTAMP(buf);
		}
	}


	if(h264enc->LeftData != NULL) {
		buf = gst_buffer_join(h264enc->LeftData, buf);
		h264enc->LeftData = NULL;
	}

	//consume frame data
	dataleftlen = GST_BUFFER_SIZE(buf);
	pInputImgData = GST_BUFFER_DATA(buf);
	for( ;dataleftlen >= h264enc->i1SrcFrameDataLen; ) {
		//prepare one 8aligned new frame
		if(G_UNLIKELY(! IPPGST_H264ENC_IS_8ALIGN(pInputImgData))) {
			if(copy_frame_to_8alignbuf(h264enc, pInputImgData) != 0) {
				goto H264ENC_CHAIN_FATALERR;
			}
			h264enc->H264Enc_SrcImage.ppPicPlane[0] = h264enc->pBuf8Aligned;
		}else{
			h264enc->H264Enc_SrcImage.ppPicPlane[0] = pInputImgData;
		}
		h264enc->H264Enc_SrcImage.ppPicPlane[1] = h264enc->H264Enc_SrcImage.ppPicPlane[0] + h264enc->iSrcUoffset;
		h264enc->H264Enc_SrcImage.ppPicPlane[2] = h264enc->H264Enc_SrcImage.ppPicPlane[0] + h264enc->iSrcVoffset;

		pInputImgData += h264enc->i1SrcFrameDataLen;
		dataleftlen -= h264enc->i1SrcFrameDataLen;
		

		//encode one frame
		for(;;) {
#ifdef DEBUG_PERFORMANCE
			gettimeofday(&EncAPI_clk0, NULL);
#endif
			codecret = EncodeFrame_H264Video(&h264enc->H264Enc_MemCBTable, NULL, &h264enc->H264Enc_DstBitsBuf, &h264enc->H264Enc_SrcImage, h264enc->H264Enc_Handle, NULL, NULL);
#ifdef DEBUG_PERFORMANCE
			gettimeofday(&EncAPI_clk1, NULL);
			h264enc->codec_time += (EncAPI_clk1.tv_sec - EncAPI_clk0.tv_sec)*1000000 + (EncAPI_clk1.tv_usec - EncAPI_clk0.tv_usec);
#endif
			if(G_LIKELY(codecret == IPP_STATUS_NOERR)) {
				h264enc->iEncOutputByteCnt += h264enc->H264Enc_DstBitsBuf.pBsCurByte - h264enc->H264Enc_DstBitsBuf.pBsBuffer;
				h264enc->iEncOutputFrameCnt++;
				break;
			}else if(codecret == IPP_STATUS_OUTPUT_DATA) {
				assist_myecho("Need to expand the dstbits buffer of h264 encoder, call realloc\n");
				//free space in H264Enc_DstBitsBuf isn't enough, need to expand it
				int offset = h264enc->H264Enc_DstBitsBuf.pBsCurByte - h264enc->H264Enc_DstBitsBuf.pBsBuffer;
				h264enc->H264Enc_DstBitsBuf.pBsBuffer = g_realloc(h264enc->H264Enc_DstBitsBuf.pBsBuffer, h264enc->H264Enc_DstBitsBuf.bsByteLen + h264enc->iDstBitsBufUnitLen);
				if(h264enc->H264Enc_DstBitsBuf.pBsBuffer == NULL) {
					g_warning("Expand dstbits buffer of h264 encoder fail!");
					GST_ERROR_OBJECT(h264enc, "Expand dstbits buffer of h264 encoder fail!");
					goto H264ENC_CHAIN_FATALERR;
				}
				h264enc->H264Enc_DstBitsBuf.pBsCurByte = h264enc->H264Enc_DstBitsBuf.pBsBuffer + offset;
				h264enc->H264Enc_DstBitsBuf.bsByteLen += h264enc->iDstBitsBufUnitLen;
				
			}else{
				g_warning("EncodeFrame_H264Video returned unexpected %d, stop encoding", codecret);
				GST_ERROR_OBJECT(h264enc, "EncodeFrame_H264Video returned unexpected %d, stop encoding", codecret);
				goto H264ENC_CHAIN_FATALERR;
			}
		}

		//one frame is encoded, push data to down stream
		GstBuffer* down_buf = gst_buffer_new_and_alloc(h264enc->H264Enc_DstBitsBuf.pBsCurByte - h264enc->H264Enc_DstBitsBuf.pBsBuffer);
		if(G_LIKELY(down_buf != NULL)) {
			memcpy(GST_BUFFER_DATA(down_buf), h264enc->H264Enc_DstBitsBuf.pBsBuffer, GST_BUFFER_SIZE(down_buf));
			GST_BUFFER_TIMESTAMP(down_buf) = h264enc->PushdownTsCandidate;
		}
		
		//reset h264 encoder dstbits buffer
		h264enc->H264Enc_DstBitsBuf.pBsCurByte = h264enc->H264Enc_DstBitsBuf.pBsBuffer;
		h264enc->H264Enc_DstBitsBuf.bsCurBitOffset = 0;

		//update TS for next frame
		if(h264enc->PushdownTsCandidate != GST_CLOCK_TIME_NONE) {
			h264enc->PushdownTsCandidate = h264enc->PushdownTsCandidate + h264enc->iFixedFRateDuration;
		}

		if(G_UNLIKELY(down_buf == NULL)) {
			g_warning("Allocate push down buffer fail, give up this compressed frame, continue encoding next frame");
			continue;
		}

		//push the data to downstream
		GST_BUFFER_DURATION(down_buf) = h264enc->iFixedFRateDuration;
		gst_buffer_set_caps(down_buf, GST_PAD_CAPS(h264enc->srcpad));
		
		//assist_myecho("Pushdown buffer sz %d, TS %lld, DU %lld\n", GST_BUFFER_SIZE(down_buf), GST_BUFFER_TIMESTAMP(down_buf), GST_BUFFER_DURATION(down_buf));
		pushret = gst_pad_push(h264enc->srcpad, down_buf);
		if(pushret != GST_FLOW_OK) {
			assist_myecho("gst_pad_push fail, return %d\n", pushret);
			GST_WARNING_OBJECT(h264enc, "gst_pad_push fail, return %d\n", pushret);
		}
	}

	if(dataleftlen > 0) {
		h264enc->LeftData = gst_buffer_create_sub(buf, pInputImgData-GST_BUFFER_DATA(buf), dataleftlen);
	}

	gst_buffer_unref(buf);
	return pushret;

H264ENC_CHAIN_FATALERR:
	gst_buffer_unref (buf);
	return GST_FLOW_ERROR;
}

static void
h264enc_ready2null(GstIppH264Enc* h264enc)
{
	//if there is still input data left, give up it
	if(h264enc->LeftData) {
		gst_buffer_unref(h264enc->LeftData);
		h264enc->LeftData = NULL;
	}

	//release encoder resource
	if(h264enc->H264Enc_Handle) {
		EncoderFree_H264Video(&h264enc->H264Enc_MemCBTable, &h264enc->H264Enc_Handle, NULL);
		h264enc->H264Enc_Handle = NULL;
	}
	if(h264enc->H264Enc_DstBitsBuf.pBsBuffer) {
		g_free(h264enc->H264Enc_DstBitsBuf.pBsBuffer);
		h264enc->H264Enc_DstBitsBuf.pBsBuffer = NULL;
	}
	if(h264enc->pBuf8Aligned) {
		IPP_MemFree((void**)&h264enc->pBuf8Aligned);
		h264enc->pBuf8Aligned = NULL;
	}

	//print information
	assist_myecho("h264enc resource released, h264enc totally outputted %lld bytes and %d frames.\n", h264enc->iEncOutputByteCnt, h264enc->iEncOutputFrameCnt);

#ifdef DEBUG_PERFORMANCE
	printf("h264enc outputted %d compressed frames and costed %lld microseconds!!!\n", h264enc->iEncOutputFrameCnt, h264enc->codec_time);
#endif

	return;
}

static GstStateChangeReturn
gst_ipph264enc_change_state(GstElement *element, GstStateChange transition)
{
	GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;

	assist_myecho("GST h264enc state change trans from %d to %d\n", transition>>3, transition&7);

	switch (transition)
	{
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
		h264enc_ready2null(GST_IPPH264ENC(element));
		break;

	default:
		break;
	}

	return ret;

}



static void 
gst_ipph264enc_base_init (gpointer g_klass)
{
	static GstElementDetails h264enc_details = {
		"Marvell IPP H.264 Video Encoder", 
		"Codec/Encoder/Video", 
		"H.264 Video Encoder based-on IPP & CODEC", 
		""
	};
	GstElementClass *element_class = GST_ELEMENT_CLASS (g_klass);

	gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&src_factory));
	gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&sink_factory));

	gst_element_class_set_details (element_class, &h264enc_details);

	return;
}

static void 
gst_ipph264enc_class_init (GstIppH264EncClass *klass)
{
	GObjectClass *gobject_class  = (GObjectClass*) klass;
	GstElementClass *gstelement_class = (GstElementClass*) klass;

	gobject_class->set_property = gst_ipph264enc_set_property;
	gobject_class->get_property = gst_ipph264enc_get_property;


	g_object_class_install_property (gobject_class, ARG_BIT_RATE, \
		g_param_spec_int ("bitrate", "The bit rate of encoded stream", \
		"The target bit rate of the encoded stream(bit/second)", \
		1/* range_MIN */, G_MAXINT/* range_MAX */, DEFAULT_TARGETBITRATE/* default_INIT */, G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, ARG_QUALITY_LEVEL, \
		g_param_spec_int ("qualitylevel", "quality strategy for encoder", \
		"Quality strategy for encoder, could be 1<low quality but faster>, 2<compromise>, 3<high quality but slower>", \
		1/* range_MIN */, 3/* range_MAX */, DEFAULT_QUALITYLEVEL/* default_INIT */, G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, ARG_FORCE_STARTTIME, \
		g_param_spec_int64 ("force-starttime", "The first frame's timestamp", \
		"Set the first compressed frame's timestamp(unit is nanoseconds). If this value is -1, adopt input frame's timestamp. Otherwise, ignore all timestamp attached on input frame buffer and produce following timestamps according to frame rate.", \
		-1/* range_MIN */, G_MAXINT64/* range_MAX */, -1/* default_INIT */, G_PARAM_READWRITE));



#ifdef DEBUG_PERFORMANCE
	g_object_class_install_property (gobject_class, ARG_TOTALFRAME,
		g_param_spec_int ("totalframes", "Totalframe of h264stream",
		"Number of total frames encoder outputted", 0, G_MAXINT, 0, G_PARAM_READABLE));
	g_object_class_install_property (gobject_class, ARG_CODECTIME,
		g_param_spec_int64 ("codectime", "h264 encoder time",
		"Total pure encoder spend system time(microsecond)", 0, G_MAXINT64, 0, G_PARAM_READABLE));
#endif

	gstelement_class->change_state = GST_DEBUG_FUNCPTR(gst_ipph264enc_change_state);


	GST_DEBUG_CATEGORY_INIT (ipph264enc_debug, "ipph264enc", 0, "H.264 Encode Element");

	return;
}


static inline void
gst_ipph264enc_init_members(GstIppH264Enc* h264enc)
{
	h264enc->H264Enc_Handle = NULL;

	h264enc->H264Enc_MemCBTable.h264MemCalloc = IPP_MemCalloc;
	h264enc->H264Enc_MemCBTable.h264MemFree = IPP_MemFree;

	memset(&h264enc->H264Enc_Par, 0, sizeof(h264enc->H264Enc_Par));
	memset(&h264enc->H264Enc_SrcImage, 0, sizeof(h264enc->H264Enc_SrcImage));
	memset(&h264enc->H264Enc_DstBitsBuf, 0, sizeof(h264enc->H264Enc_SrcImage));

	h264enc->pBuf8Aligned = NULL;

	h264enc->iEncFrameRate = 0;
	h264enc->iFixedFRateDuration = GST_CLOCK_TIME_NONE;

	h264enc->PushdownTsCandidate = GST_CLOCK_TIME_NONE;

	h264enc->LeftData = NULL;

	h264enc->iTargtBitRate = DEFAULT_TARGETBITRATE;
	h264enc->iQualityLevel = DEFAULT_QUALITYLEVEL;
	h264enc->ForceStartTimestamp = -1;

	h264enc->iEncOutputByteCnt = 0;
	h264enc->iEncOutputFrameCnt = 0;
#ifdef DEBUG_PERFORMANCE
	h264enc->codec_time = 0;
#endif
	return;
}

static void 
gst_ipph264enc_init(GstIppH264Enc *h264enc, GstIppH264EncClass *h264enc_klass)
{
	GstElementClass *klass = GST_ELEMENT_CLASS (h264enc_klass);

	h264enc->sinkpad = gst_pad_new_from_template (
		gst_element_class_get_pad_template (klass, "sink"), "sink");

	gst_pad_set_setcaps_function (h264enc->sinkpad, gst_ipph264enc_sinkpad_setcaps);
	gst_pad_set_chain_function (h264enc->sinkpad, gst_ipph264enc_chain);

	gst_element_add_pad (GST_ELEMENT(h264enc), h264enc->sinkpad);


	h264enc->srcpad = gst_pad_new_from_template (
		gst_element_class_get_pad_template (klass, "src"), "src");

	gst_element_add_pad (GST_ELEMENT (h264enc), h264enc->srcpad);

	gst_ipph264enc_init_members(h264enc);

	return;

}


static gboolean 
plugin_init (GstPlugin *plugin)
{
	return gst_element_register (plugin, "ipph264enc", GST_RANK_MARGINAL, GST_TYPE_IPPH264ENC);
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR, 
                   GST_VERSION_MINOR, 
                   "mvl_h264enc", 
                   "h264 encoder based on IPP, "__DATE__, 
                   plugin_init, 
                   VERSION, 
                   "LGPL", 
                   "", 
                   "");


/* EOF */
