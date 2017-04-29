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

#include "gsth263enc.h"

#include <string.h>
#ifdef DEBUG_PERFORMANCE
#include <sys/time.h>
#include <time.h>
struct timeval EncAPI_clk0, EncAPI_clk1;
#endif

GST_DEBUG_CATEGORY_STATIC (ipph263enc_debug);
#define GST_CAT_DEFAULT ipph263enc_debug


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
	GST_STATIC_CAPS ("video/x-h263, " \
					 "width = (int) [ 16, 2048 ], " \
					 "height = (int) [ 16, 2048 ], " \
					 "framerate = (fraction) [ 1/1, 60/1 ]"));

GST_BOILERPLATE (GstIppH263Enc, gst_ipph263enc, GstElement, GST_TYPE_ELEMENT);

#if 0
#define assist_myecho(...)	printf(__VA_ARGS__)
#else
#define assist_myecho(...)
#endif

#define IPPGST_H263ENC_IS_8ALIGN(x)			(((unsigned int)(x)&7) == 0)
#define IPPGST_H263ENC_IS_16ALIGN(x)		(((unsigned int)(x)&15) == 0)

static void 
IPP_SetEncOtherPar_H263Enc(GstIppH263Enc *h263enc)
{
	IppH263ParSet * pH263EncoderPar = &(h263enc->H263Enc_Par);

	assist_myecho("update standard par\n");

	/* Update standard defined codec configuration */

	pH263EncoderPar->iRCType = RC_TMN8;

	pH263EncoderPar->iPBetweenI = 20;
		/* Positive integer indicates number of P-frame between two neighbouring I-frames, 
		   -1 indicates just one I-frame in starting point, no more I-frame */

	pH263EncoderPar->iIntraQuant = 12;
		/* Legal Values:  1 - 31
		   If RC enable, it indicates the initialized QP of Intra-picture,
		   else it indicates the QP of all Intra-pictures
		*/

	pH263EncoderPar->iInterQuant = 14;
		/* Legal Values:  1 - 31
		   If RC enable, it indicates the initialized QP of Inter-picture,
		   else it indicates the QP of all Inter-pictures
		*/

	pH263EncoderPar->bRoundingCtrlEnable = 0;
		/* 2 Option: 1 -> Enable; 0 -> Disable; */

	pH263EncoderPar->GOBHeaderPresent = 0;
		/* 2 Option: 1 -> Enable; 0 -> Disable; */
		/* If enabled in GOB mode, every encoded GOB will have a GOBheader
		   This flag has no influence in Slice Structured mode
		*/

	/* Annex Options */
	pH263EncoderPar->optUMV = 0;
		/* 2 Option: 1 -> Enable; 0 -> Disable; */

	pH263EncoderPar->optSAC = 0;
		/* 2 Option: 1 -> Enable; 0 -> Disable; */

	pH263EncoderPar->optAP = 0;
		/* 2 Option: 1 -> Enable; 0 -> Disable; */

	pH263EncoderPar->optPBframes = 0;
		/* 2 Option: 1 -> Enable; 0 -> Disable; */

	pH263EncoderPar->optAIC = 0;
		/* 2 Option: 1 -> Enable; 0 -> Disable; */

	/* RC_VT for Borqs case, sync. between OpenMAX and lib */
	//pH263EncoderPar->optDF = 1;
	pH263EncoderPar->optDF = 0;
		/* 2 Option: 1 -> Enable; 0 -> Disable; */

	pH263EncoderPar->optSS = 0;
		/* 2 Option: 1 -> Enable; 0 -> Disable; */
	pH263EncoderPar->BitsPerSlice = 2000;
		/* Threshold of Bits per slice when in SS mode */

	pH263EncoderPar->optRPS = 0;
		/* 2 Option: 1 -> Enable; 0 -> Disable; */

	pH263EncoderPar->optISD = 0;
		/* 2 Option: 1 -> Enable; 0 -> Disable; */

	pH263EncoderPar->optAIV = 0;
		/* 2 Option: 1 -> Enable; 0 -> Disable; */

	/* RC_VT for Borqs case, sync. between OpenMAX and lib */
	//pH263EncoderPar->optMQ = 1;
	pH263EncoderPar->optMQ = 0;
		/* 2 Option: 1 -> Enable; 0 -> Disable; */

	pH263EncoderPar->optRPR = 0;
		/* 2 Option: 1 -> Enable; 0 -> Disable; */

	pH263EncoderPar->optRRU = 0;
		/* 2 Option: 1 -> Enable; 0 -> Disable; */

	pH263EncoderPar->iSearchRange = 15;
		/* Positive Integer, Search Range of Motion Estimation */

	pH263EncoderPar->iMEAlgorithm = 0;
		/* Motion Estimation Algorithm Selection
		   2 Option:	0 -> MVFAST;
						1 -> SEA;
		*/

	pH263EncoderPar->iMEStrategy = ME_QUALITY_BIAS;

	pH263EncoderPar->iMaxQP = 31;
	pH263EncoderPar->iMinQP = 1; // sync. between codec and OMXIL
	return;
}

static int
IPP_videoCheckMainPar_H263Enc(int w, int h, int FR, int bR)
{
	if(! IPPGST_H263ENC_IS_16ALIGN(w)) {
		g_warning("src image width %d isn't 16 aligned!", w);
		return -1;
	}
	if(! IPPGST_H263ENC_IS_16ALIGN(h)) {
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
	return 0;
}



static void 
IPP_SetEncMainPar_H263Enc(GstIppH263Enc *h263enc)
{
	h263enc->H263Enc_Par.bAlgorithmFromCustomer = 0;

	assist_myecho("Set h263 encoder parameter, w %d, h %d, FR %d, bR %d\n", h263enc->iEncW, h263enc->iEncH, h263enc->iEncFrameRate, h263enc->iTargtBitRate);

	if( (128==h263enc->iEncW) && (96==h263enc->iEncH) ){
		h263enc->H263Enc_Par.iSourceFormat = IPP_VIDEO_SUBQCIF;
		assist_myecho("Frame Resulotion: SUBQCIF\n");
	}else if( (176==h263enc->iEncW) && (144==h263enc->iEncH) ){
		h263enc->H263Enc_Par.iSourceFormat = IPP_VIDEO_QCIF;
		assist_myecho("Frame Resulotion: QCIF\n");
	}else if( (352==h263enc->iEncW) && (288==h263enc->iEncH) ){
		h263enc->H263Enc_Par.iSourceFormat = IPP_VIDEO_CIF;
		assist_myecho("Frame Resulotion: CIF\n");
	}else if( (704==h263enc->iEncW) && (576==h263enc->iEncH) ){
		h263enc->H263Enc_Par.iSourceFormat = IPP_VIDEO_4CIF;
		assist_myecho("Frame Resulotion: 4CIF\n");
	}else if( (1408==h263enc->iEncW) && (1152==h263enc->iEncH) ){
		h263enc->H263Enc_Par.iSourceFormat = IPP_VIDEO_16CIF;
		assist_myecho("Frame Resulotion: 16CIF\n");
	}else{
		h263enc->H263Enc_Par.iSourceFormat = IPP_VIDEO_CUSTOM;
		assist_myecho("Frame Resulotion: custom w %d, h %d\n", h263enc->iEncW, h263enc->iEncH);
	}

	h263enc->H263Enc_Par.iPicWidth = h263enc->iEncW;
	h263enc->H263Enc_Par.iPicHeight = h263enc->iEncH;
	h263enc->H263Enc_Par.iSrcFrmRate = h263enc->iEncFrameRate;
	h263enc->H263Enc_Par.iSrcSamRate = 1;
	h263enc->H263Enc_Par.iColorFormat = IPP_YCbCr411;
	h263enc->H263Enc_Par.bRCEnable = 1;
	h263enc->H263Enc_Par.iRCBitRate = h263enc->iTargtBitRate;

	h263enc->H263Enc_Par.bHWMEEnable = 0;	//it's useless for software h263 encoder

	assist_myecho("frame rate: %d\n", h263enc->iEncFrameRate);


	return;
}



static inline int
calculate_GSTstandard_I420_size(int w, int h)
{
	//gstreamer defined standard I420 layout, see <gst/video/video.h>, gstvideo and gst_video_format_get_size()
	return (GST_ROUND_UP_4(w) + GST_ROUND_UP_4(GST_ROUND_UP_2(w)>>1)) * GST_ROUND_UP_2(h);
}

static int
set_srcframe_layout(GstIppH263Enc* h263enc, int w, int h)
{
	//currently, h263 encoder only accept 16 aligned width and height, therefore, Y pitch and UV pitch are all 8 aligned. So if the first line of Y plannar is 8 aligned, each line is 8 aligned
	//And we only support GST standard I420 layout
	h263enc->i1SrcFrameDataLen = calculate_GSTstandard_I420_size(w, h);
	h263enc->H263Enc_SrcImage.picPlaneStep[0] = GST_ROUND_UP_4(w);
	h263enc->H263Enc_SrcImage.picPlaneStep[1] = GST_ROUND_UP_4(GST_ROUND_UP_2(w)>>1);
	h263enc->H263Enc_SrcImage.picPlaneStep[2] = h263enc->H263Enc_SrcImage.picPlaneStep[1];

	h263enc->iSrcUoffset = h263enc->H263Enc_SrcImage.picPlaneStep[0] * GST_ROUND_UP_2(h);
	h263enc->iSrcVoffset = h263enc->iSrcUoffset + h263enc->H263Enc_SrcImage.picPlaneStep[1] * (GST_ROUND_UP_2(h)>>1);

	h263enc->H263Enc_SrcImage.picWidth = h263enc->iEncW;
	h263enc->H263Enc_SrcImage.picHeight = h263enc->iEncH;
	h263enc->H263Enc_SrcImage.picPlaneNum = 3;
	h263enc->H263Enc_SrcImage.picChannelNum = 3;
	h263enc->H263Enc_SrcImage.picFormat = IPP_YCbCr411;

	return 0;
}

static gboolean 
gst_ipph263enc_sinkpad_setcaps(GstPad *pad, GstCaps *caps)
{
	GstIppH263Enc *h263enc = GST_IPPH263ENC (GST_OBJECT_PARENT (pad));

	guint stru_num = gst_caps_get_size (caps);
	if(stru_num == 0) {
		g_warning("No content in h263enc sinkpad setcaps!");
		GST_ERROR_OBJECT(h263enc, "No content in h263enc sinkpad setcaps!");
		return FALSE;
	}
	

	GstStructure *str = gst_caps_get_structure (caps, 0);

	const char *mime = gst_structure_get_name (str);
	if (strcmp (mime, "video/x-raw-yuv") != 0) {
		g_warning("The cap mime type %s isn't video/x-raw-yuv!", mime);
		GST_ERROR_OBJECT(h263enc, "The cap mime type %s isn't video/x-raw-yuv!", mime);
		return FALSE;
	}

	guint32 fourcc = 0;
	if( FALSE == gst_structure_get_fourcc(str, "format", &fourcc) || fourcc != GST_STR_FOURCC("I420") ) {
		g_warning("Couldn't found I420 fourcc in setcaps!");
		GST_ERROR_OBJECT(h263enc, "Couldn't found I420 fourcc in setcaps!");
		return FALSE;
	}


	if (FALSE == gst_structure_get_int(str, "width", &(h263enc->iEncW))) {
		g_warning("Couldn't get src image width in setcaps!");
		GST_ERROR_OBJECT(h263enc, "Couldn't get src image width in gstcaps!");
		return FALSE;
	}

	if (FALSE == gst_structure_get_int(str, "height", &(h263enc->iEncH))) {
		g_warning("Couldn't get src image height in setcaps!");
		GST_ERROR_OBJECT(h263enc, "Couldn't get src image height in gstcaps!");
		return FALSE;
	}

	int iFRateNum, iFRateDen;
	if (FALSE == gst_structure_get_fraction(str, "framerate", &iFRateNum, &iFRateDen)) {
		g_warning("Couldn't get framerate in setcaps!");
		GST_ERROR_OBJECT(h263enc, "Couldn't get framerate in setcaps!");
		return FALSE;
	}
	h263enc->iEncFrameRate = iFRateNum/iFRateDen;
	if(iFRateDen * h263enc->iEncFrameRate != iFRateNum) {	//current h263 encoder only support interger fps
		g_warning("Couldn't support fractional framerate (%d)div(%d) fps!", iFRateNum, iFRateDen);
		GST_ERROR_OBJECT(h263enc, "Couldn't support fractional framerate (%d)div(%d) fps!", iFRateNum, iFRateDen);
		return FALSE;
	}
	if(h263enc->iEncFrameRate) {
		h263enc->iFixedFRateDuration = gst_util_uint64_scale_int(GST_SECOND, 1, h263enc->iEncFrameRate);
	}

	if( 0 != IPP_videoCheckMainPar_H263Enc(h263enc->iEncW, h263enc->iEncH, h263enc->iEncFrameRate, h263enc->iTargtBitRate) ) {
		GST_ERROR_OBJECT(h263enc, "The parameter for encoder is wrong!!!");
		return FALSE;
	}
	
	GstCaps *TmpCap;
	TmpCap = gst_caps_new_simple ("video/x-h263", 
		"width", G_TYPE_INT, h263enc->iEncW, 
		"height", G_TYPE_INT, h263enc->iEncH, 
		"framerate", GST_TYPE_FRACTION,  h263enc->iEncFrameRate, 1,
		NULL);
	if(!gst_pad_set_caps(h263enc->srcpad, TmpCap)) {
		gst_caps_unref(TmpCap);
		g_warning("Set caps on srcpad fail!");
		GST_ERROR_OBJECT(h263enc, "Set caps on srcpad fail!");
		return FALSE;
	}
	gst_caps_unref(TmpCap);

	set_srcframe_layout(h263enc, h263enc->iEncW, h263enc->iEncH);

	IPP_SetEncMainPar_H263Enc(h263enc);
	IPP_SetEncOtherPar_H263Enc(h263enc);

	return TRUE;

}


static void 
gst_ipph263enc_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	GstIppH263Enc *h263enc = GST_IPPH263ENC (object);


	switch (prop_id)
	{
	case ARG_BIT_RATE:
		{
			int BR = g_value_get_int(value);
			if(BR <=0) {
				g_warning("bitrate %d exceed range!", BR);
			}else{
				h263enc->iTargtBitRate = BR;
			}
		}		
		break;
	case ARG_FORCE_STARTTIME:
		{
			gint64 st = g_value_get_int64(value);
			if(st<-1) {
				g_warning("force-starttime %lld exceed range!", st);
			}else{
				h263enc->ForceStartTimestamp = st;
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
gst_ipph263enc_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	GstIppH263Enc *h263enc = GST_IPPH263ENC (object);

	switch (prop_id)
	{
	case ARG_BIT_RATE:
		g_value_set_int (value, h263enc->iTargtBitRate);
		break;
	case ARG_FORCE_STARTTIME:
		g_value_set_int64(value, h263enc->ForceStartTimestamp);
		break;

#ifdef DEBUG_PERFORMANCE
	case ARG_TOTALFRAME:
		g_value_set_int (value, h263enc->iEncOutputFrameCnt);
		break;
	case ARG_CODECTIME:
		g_value_set_int64 (value, h263enc->codec_time);
		break;
#endif
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
	return;
}



static int
init_bitsbuf_and_h263enc(GstIppH263Enc* h263enc)
{
	assist_myecho("----------Init h263 raw encoder----------\n");

	if(NULL==h263enc->pH263Enc_CBTable){
		miscInitGeneralCallbackTable(&(h263enc->pH263Enc_CBTable));
		if(h263enc->pH263Enc_CBTable == NULL) {
			g_warning("Init memory callback table fail!");
			GST_ERROR_OBJECT(h263enc, "Init memory callback table fail!");
			return -3;
		}
	}

	int iDstBufSz;
	//iDstBufSz = (h263enc->iEncW * h263enc->iEncH)/10;
	iDstBufSz = h263enc->iTargtBitRate / h263enc->iEncFrameRate;
#define H263ENCODER_DSTBUF_SIZE_MIN			256
	if(iDstBufSz < H263ENCODER_DSTBUF_SIZE_MIN) {
		iDstBufSz = H263ENCODER_DSTBUF_SIZE_MIN;
	}

	//in Encode_H263Video(), it will use callback function in pH263Enc_CBTable to reallocate this dst bits buffer
	h263enc->pH263Enc_CBTable->fMemMalloc((void**)&(h263enc->H263Enc_DstBitsBuf.pBsBuffer), iDstBufSz, 8);

	if(h263enc->H263Enc_DstBitsBuf.pBsBuffer == NULL) {
		g_warning("Allocate target bit stream buffer(wanted size %d) fail", iDstBufSz);
		GST_ERROR_OBJECT(h263enc, "Allocate target bit stream buffer(wanted size %d) fail", iDstBufSz);
		return -1;
	}
	h263enc->H263Enc_DstBitsBuf.bsByteLen = iDstBufSz;
	h263enc->H263Enc_DstBitsBuf.pBsCurByte = h263enc->H263Enc_DstBitsBuf.pBsBuffer;

	assist_myecho("allocated bits buffer\n");

	assist_myecho("init API: EncoderInitAlloc_H263Video\n");
	IppCodecStatus codecret = EncoderInitAlloc_H263Video(&h263enc->H263Enc_Par, h263enc->pH263Enc_CBTable, NULL, (void**)&h263enc->H263Enc_Handle);
	assist_myecho("finish init API: EncoderInitAlloc_H263Video\n");

	if(codecret != IPP_STATUS_NOERR) {
		h263enc->pH263Enc_CBTable->fMemFree((void**)&(h263enc->H263Enc_DstBitsBuf.pBsBuffer));
		h263enc->H263Enc_DstBitsBuf.pBsBuffer  = NULL;
		miscFreeGeneralCallbackTable(&(h263enc->pH263Enc_CBTable));
		h263enc->pH263Enc_CBTable = NULL;
		g_warning("Init h263 encoder fail, ret %d", codecret);
		GST_ERROR_OBJECT(h263enc, "Init h263 encoder fail, ret %d", codecret);
		return -2;
	}
	return 0;
}

static int
copy_frame_to_8alignbuf(GstIppH263Enc* h263enc, unsigned char* pSrc)
{
	if(h263enc->pBuf8Aligned == NULL) {
		assist_myecho("Create 8 aligned frame buffer for transforming non 8 align to 8 align!\n");
		IPP_MemMalloc((void**)&h263enc->pBuf8Aligned, h263enc->i1SrcFrameDataLen, 8);
		if(h263enc->pBuf8Aligned == NULL) {
			g_warning("Allocate 8 aligned frame buffer fail!");
			GST_ERROR_OBJECT(h263enc, "Allocate 8 aligned frame buffer fail!");
			return -1;
		}
	}
	memcpy(h263enc->pBuf8Aligned, pSrc, h263enc->i1SrcFrameDataLen);
	return 0;
}

static GstFlowReturn 
gst_ipph263enc_chain(GstPad *pad, GstBuffer *buf)
{
	GstIppH263Enc *h263enc = GST_IPPH263ENC (GST_OBJECT_PARENT (pad));
	unsigned char* pInputImgData;
	int dataleftlen;
	GstFlowReturn pushret = GST_FLOW_OK;
	IppCodecStatus codecret;
	
	//assist_myecho("Input buffer sz %d, TS %lld, DU %lld\n", GST_BUFFER_SIZE(buf), GST_BUFFER_TIMESTAMP(buf), GST_BUFFER_DURATION(buf));

	if(h263enc->H263Enc_Par.iPicWidth == 0) {
		g_warning("encoder parameter is still unset in chain()");
		GST_ERROR_OBJECT(h263enc, "encoder parameter is still unset in chain()");
		goto H263ENC_CHAIN_FATALERR;
	}

	if(h263enc->H263Enc_Handle == NULL) {
		assist_myecho("h263enc handle is NULL\n");
		if(init_bitsbuf_and_h263enc(h263enc) != 0) {
			goto H263ENC_CHAIN_FATALERR;
		}
		assist_myecho("Input data %s streaming mode: input fragment len %d, one srcframe len %d\n", (GST_BUFFER_SIZE(buf)==h263enc->i1SrcFrameDataLen? "is":"isn't"), GST_BUFFER_SIZE(buf), h263enc->i1SrcFrameDataLen);
	}


	if(h263enc->ForceStartTimestamp != -1) {
		if(h263enc->PushdownTsCandidate == GST_CLOCK_TIME_NONE) {
			//the first frame
			h263enc->PushdownTsCandidate = h263enc->ForceStartTimestamp;
		}
	}else{
		if(GST_BUFFER_TIMESTAMP(buf) != GST_CLOCK_TIME_NONE) {
			h263enc->PushdownTsCandidate = GST_BUFFER_TIMESTAMP(buf);
		}
	}

	if(h263enc->LeftData != NULL) {
		buf = gst_buffer_join(h263enc->LeftData, buf);
		h263enc->LeftData = NULL;
	}

	//consume frame data
	dataleftlen = GST_BUFFER_SIZE(buf);
	pInputImgData = GST_BUFFER_DATA(buf);
	for( ;dataleftlen >= h263enc->i1SrcFrameDataLen; ) {
		//prepare one 8aligned new frame
		if(G_UNLIKELY(! IPPGST_H263ENC_IS_8ALIGN(pInputImgData))) {
			if(copy_frame_to_8alignbuf(h263enc, pInputImgData) != 0) {
				goto H263ENC_CHAIN_FATALERR;
			}
			h263enc->H263Enc_SrcImage.ppPicPlane[0] = h263enc->pBuf8Aligned;
		}else{
			h263enc->H263Enc_SrcImage.ppPicPlane[0] = pInputImgData;
		}
		h263enc->H263Enc_SrcImage.ppPicPlane[1] = h263enc->H263Enc_SrcImage.ppPicPlane[0] + h263enc->iSrcUoffset;
		h263enc->H263Enc_SrcImage.ppPicPlane[2] = h263enc->H263Enc_SrcImage.ppPicPlane[0] + h263enc->iSrcVoffset;

		pInputImgData += h263enc->i1SrcFrameDataLen;
		dataleftlen -= h263enc->i1SrcFrameDataLen;
		

		//encode one frame
		for(;;) {
#ifdef DEBUG_PERFORMANCE
			gettimeofday(&EncAPI_clk0, NULL);
#endif
			codecret = Encode_H263Video(&h263enc->H263Enc_DstBitsBuf, &h263enc->H263Enc_SrcImage, h263enc->H263Enc_Handle);
#ifdef DEBUG_PERFORMANCE
			gettimeofday(&EncAPI_clk1, NULL);
			h263enc->codec_time += (EncAPI_clk1.tv_sec - EncAPI_clk0.tv_sec)*1000000 + (EncAPI_clk1.tv_usec - EncAPI_clk0.tv_usec);
#endif
			if(G_LIKELY(codecret == IPP_STATUS_NOERR)) {
				//one frame is encoded
				break;
			}else if(codecret == IPP_STATUS_ERR) {
				g_warning("Encode_H263Video return IPP_STATUS_ERR, try call Encode_H263Video again");
				GST_WARNING_OBJECT(h263enc, "Encode_H263Video return IPP_STATUS_ERR, try call Encode_H263Video again");
				//It's strange that recalling Encode_H263Video when it return IPP_STATUS_ERR, but in IPP h263enc sample code, it does such.
				//Probably, we should stop encoding when Encode_H263Video return IPP_STATUS_ERR
				continue;
			}else{
				g_warning("Encode_H263Video returned unexpected %d, stop encoding", codecret);
				GST_ERROR_OBJECT(h263enc, "Encode_H263Video returned unexpected %d, stop encoding", codecret);
				goto H263ENC_CHAIN_FATALERR;
			}
		}

		//if encoder decide to skip current frame, encoder should output a skipped frame instead of no bits output
		codecret = Encode_FillFreezeH263Video(&h263enc->H263Enc_DstBitsBuf, h263enc->H263Enc_Handle);
		if(codecret != IPP_STATUS_NOERR) {
			g_warning("Encode_FillFreezeH263Video returned unexpected %d, stop encoding", codecret);
			GST_ERROR_OBJECT(h263enc, "Encode_FillFreezeH263Video returned unexpected %d, stop encoding", codecret);
			goto H263ENC_CHAIN_FATALERR;
		}
		int compressedlen = h263enc->H263Enc_DstBitsBuf.pBsCurByte - h263enc->H263Enc_DstBitsBuf.pBsBuffer;
		if(compressedlen <= 0) {
			g_warning("Encoder compressed frame len is %d <=0 !\n", compressedlen);
			GST_ERROR_OBJECT(h263enc, "Encoder compressed frame len is %d <=0 !\n", compressedlen);
			goto H263ENC_CHAIN_FATALERR;
		}
		h263enc->iEncOutputByteCnt += compressedlen;
		h263enc->iEncOutputFrameCnt++;

		//one frame is encoded, push data to down stream
		GstBuffer* down_buf = gst_buffer_new_and_alloc(compressedlen);
		if(G_LIKELY(down_buf != NULL)) {
			memcpy(GST_BUFFER_DATA(down_buf), h263enc->H263Enc_DstBitsBuf.pBsBuffer, GST_BUFFER_SIZE(down_buf));
			GST_BUFFER_TIMESTAMP(down_buf) = h263enc->PushdownTsCandidate;
		}

		//reset h263 encoder dstbits buffer
		h263enc->H263Enc_DstBitsBuf.pBsCurByte = h263enc->H263Enc_DstBitsBuf.pBsBuffer;
		h263enc->H263Enc_DstBitsBuf.bsCurBitOffset = 0;
		
		//update TS for next frame
		if(h263enc->PushdownTsCandidate != GST_CLOCK_TIME_NONE) {
			h263enc->PushdownTsCandidate = h263enc->PushdownTsCandidate + h263enc->iFixedFRateDuration;
		}

		if(G_UNLIKELY(down_buf == NULL)) {
			g_warning("Allocate push down buffer fail, give up this compressed frame, continue encoding next frame");
			continue;
		}

		GST_BUFFER_DURATION(down_buf) = h263enc->iFixedFRateDuration;
		gst_buffer_set_caps(down_buf, GST_PAD_CAPS(h263enc->srcpad));
		
		//assist_myecho("Pushdown buffer sz %d, TS %lld, DU %lld\n", GST_BUFFER_SIZE(down_buf), GST_BUFFER_TIMESTAMP(down_buf), GST_BUFFER_DURATION(down_buf));
		pushret = gst_pad_push(h263enc->srcpad, down_buf);
		if(pushret != GST_FLOW_OK) {
			assist_myecho("gst_pad_push fail, return %d\n", pushret);
			GST_WARNING_OBJECT(h263enc, "gst_pad_push fail, return %d\n", pushret);
		}
	}

	if(dataleftlen > 0) {
		h263enc->LeftData = gst_buffer_create_sub(buf, pInputImgData-GST_BUFFER_DATA(buf), dataleftlen);
	}

	gst_buffer_unref(buf);
	return pushret;

H263ENC_CHAIN_FATALERR:
	gst_buffer_unref (buf);
	return GST_FLOW_ERROR;
}

static void
h263enc_ready2null(GstIppH263Enc* h263enc)
{
	//if there is still input data left, give up it
	if(h263enc->LeftData) {
		gst_buffer_unref(h263enc->LeftData);
		h263enc->LeftData = NULL;
	}

	//release encoder resource
	if(h263enc->H263Enc_Handle) {
		EncoderFree_H263Video(&h263enc->H263Enc_Handle);
		h263enc->H263Enc_Handle = NULL;
	}

	if(h263enc->H263Enc_DstBitsBuf.pBsBuffer && h263enc->pH263Enc_CBTable) {
		h263enc->pH263Enc_CBTable->fMemFree((void**)&(h263enc->H263Enc_DstBitsBuf.pBsBuffer));
		h263enc->H263Enc_DstBitsBuf.pBsBuffer = NULL;
	}

	if(h263enc->pH263Enc_CBTable){
		miscFreeGeneralCallbackTable(&(h263enc->pH263Enc_CBTable));
		h263enc->pH263Enc_CBTable = NULL;
	}
	
	if(h263enc->pBuf8Aligned) {
		IPP_MemFree((void**)&h263enc->pBuf8Aligned);
		h263enc->pBuf8Aligned = NULL;
	}

	//print information
	assist_myecho("h263enc resource released, h263enc totally outputted %lld bytes and %d frames.\n", h263enc->iEncOutputByteCnt, h263enc->iEncOutputFrameCnt);

#ifdef DEBUG_PERFORMANCE
	printf("h263enc outputted %d compressed frames and costed %lld microseconds!!!\n", h263enc->iEncOutputFrameCnt, h263enc->codec_time);
#endif

	return;
}

static GstStateChangeReturn
gst_ipph263enc_change_state(GstElement *element, GstStateChange transition)
{
	GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;

	assist_myecho("GST h263enc state change trans from %d to %d\n", transition>>3, transition&7);

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
		h263enc_ready2null(GST_IPPH263ENC(element));
		break;

	default:
		break;
	}

	return ret;

}



static void 
gst_ipph263enc_base_init (gpointer g_klass)
{
	static GstElementDetails h263enc_details = {
		"Marvell IPP H.263 Video Encoder", 
		"Codec/Encoder/Video", 
		"H.263 Video Encoder based-on IPP & CODEC", 
		""
	};
	GstElementClass *element_class = GST_ELEMENT_CLASS (g_klass);

	gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&src_factory));
	gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&sink_factory));

	gst_element_class_set_details (element_class, &h263enc_details);

	return;
}

#define DEFAULT_TARGETBITRATE	300000
static void 
gst_ipph263enc_class_init (GstIppH263EncClass *klass)
{
	GObjectClass *gobject_class  = (GObjectClass*) klass;
	GstElementClass *gstelement_class = (GstElementClass*) klass;

	gobject_class->set_property = gst_ipph263enc_set_property;
	gobject_class->get_property = gst_ipph263enc_get_property;

	g_object_class_install_property (gobject_class, ARG_BIT_RATE, \
		g_param_spec_int ("bitrate", "The bit rate of encoded stream", \
		"The target bit rate of the encoded stream(bit/second)", \
		1/* range_MIN */, G_MAXINT/* range_MAX */, DEFAULT_TARGETBITRATE/* default_INIT */, G_PARAM_READWRITE));
	g_object_class_install_property (gobject_class, ARG_FORCE_STARTTIME, \
		g_param_spec_int64 ("force-starttime", "The first frame's timestamp", \
		"Set the first compressed frame's timestamp(unit is nanoseconds). If this value is -1, adopt input frame's timestamp. Otherwise, ignore all timestamp attached on input frame buffer and produce following timestamps according to frame rate.", \
		-1/* range_MIN */, G_MAXINT64/* range_MAX */, -1/* default_INIT */, G_PARAM_READWRITE));


#ifdef DEBUG_PERFORMANCE
	g_object_class_install_property (gobject_class, ARG_TOTALFRAME,
		g_param_spec_int ("totalframes", "Totalframe of h263stream",
		"Number of total frames encoder outputted", 0, G_MAXINT, 0, G_PARAM_READABLE));
	g_object_class_install_property (gobject_class, ARG_CODECTIME,
		g_param_spec_int64 ("codectime", "h263 encoder time",
		"Total pure encoder spend system time(microsecond)", 0, G_MAXINT64, 0, G_PARAM_READABLE));
#endif

	gstelement_class->change_state = GST_DEBUG_FUNCPTR(gst_ipph263enc_change_state);


	GST_DEBUG_CATEGORY_INIT (ipph263enc_debug, "ipph263enc", 0, "H.263 Encode Element");

	return;
}


static inline void
gst_ipph263enc_init_members(GstIppH263Enc* h263enc)
{
	h263enc->H263Enc_Handle = NULL;

	memset(&h263enc->H263Enc_Par, 0, sizeof(IppH263ParSet));
	memset(&h263enc->H263Enc_SrcImage, 0, sizeof(IppPicture));
	memset(&h263enc->H263Enc_DstBitsBuf, 0, sizeof(IppBitstream));

	h263enc->pBuf8Aligned = NULL;

	h263enc->iEncFrameRate = 0;
	h263enc->iFixedFRateDuration = GST_CLOCK_TIME_NONE;
	h263enc->PushdownTsCandidate = GST_CLOCK_TIME_NONE;

	h263enc->LeftData = NULL;

	h263enc->iTargtBitRate = DEFAULT_TARGETBITRATE;
	h263enc->ForceStartTimestamp = -1;

	h263enc->iEncOutputByteCnt = 0;
	h263enc->iEncOutputFrameCnt = 0;
#ifdef DEBUG_PERFORMANCE
	h263enc->codec_time = 0;
#endif
	return;
}

static void 
gst_ipph263enc_init(GstIppH263Enc *h263enc, GstIppH263EncClass *h263enc_klass)
{
	GstElementClass *klass = GST_ELEMENT_CLASS (h263enc_klass);

	h263enc->sinkpad = gst_pad_new_from_template (
		gst_element_class_get_pad_template (klass, "sink"), "sink");

	gst_pad_set_setcaps_function (h263enc->sinkpad, gst_ipph263enc_sinkpad_setcaps);
	gst_pad_set_chain_function (h263enc->sinkpad, gst_ipph263enc_chain);

	gst_element_add_pad (GST_ELEMENT(h263enc), h263enc->sinkpad);


	h263enc->srcpad = gst_pad_new_from_template (
		gst_element_class_get_pad_template (klass, "src"), "src");

	gst_element_add_pad (GST_ELEMENT (h263enc), h263enc->srcpad);

	gst_ipph263enc_init_members(h263enc);

	return;

}


static gboolean 
plugin_init (GstPlugin *plugin)
{
	return gst_element_register (plugin, "ipph263enc", GST_RANK_MARGINAL, GST_TYPE_IPPH263ENC);
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR, 
                   GST_VERSION_MINOR, 
                   "mvl_h263enc", 
                   "h263 encoder based on IPP, "__DATE__, 
                   plugin_init, 
                   VERSION, 
                   "LGPL", 
                   "", 
                   "");


/* EOF */
