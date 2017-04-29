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

#include "gstmpeg4enc.h"

#include <string.h>
#ifdef DEBUG_PERFORMANCE
#include  <sys/time.h>

        struct timeval tstart, tend;
        struct timezone tz;
#endif

GST_DEBUG_CATEGORY_STATIC (mpeg4enc_debug);
#define GST_CAT_DEFAULT mpeg4enc_debug

/******************************************************************************
// IPP specific function entries
******************************************************************************/

unsigned char appCustomerIntraQMatrix [64] = {
	8,	17,	18,	19,	21,	23,	25,	27,
	17,	18,	19,	21,	23,	25,	27,	28,
	20,	21,	22,	23,	24,	26,	28,	30,
	21,	22,	23,	24,	26,	28,	30,	32,
	22,	23,	24,	26,	28,	30,	32,	35,
	23,	24,	26,	28,	30,	32,	35,	38,
	25,	26,	28,	30,	32,	35,	38,	41,
	27,	28,	30,	32,	35,	38,	41,	45
};

unsigned char appCustomerInterQMatrix [64] = {
	16,	17,	18, 19, 20,	21,	22,	23,
	17,	18,	19,	20,	21,	22,	23,	24,
	18,	19,	20,	21,	22,	23,	24,	25,
	19,	20,	21,	22,	23,	24,	26,	27,
	20,	21,	22,	23,	25,	26,	27,	28,
	21,	22,	23,	24,	26,	27,	28,	30,
	22,	23,	24,	26,	27,	28,	30,	31,
	23,	24,	25,	27,	28,	30,	31,	33
};

IppCodecStatus
IPP_videoInitBuffer_MPEG4Enc (GstMPEG4Enc *mpeg4enc)
{
	int nBufSize = 0;
	IppPicture *pPic = &(mpeg4enc->srcPicture);
	IppBitstream *pBS = &(mpeg4enc->dstBitStream);

	/* allocate and initialize output bitstream */
	if(mpeg4enc->mpeg4EncPar.iSrcFrmRate != 0){
		nBufSize = (mpeg4enc->mpeg4EncPar.iRCBitRate / mpeg4enc->mpeg4EncPar.iSrcFrmRate) >> 3;
	}
	
	pBS->pBsBuffer = (Ipp8u*)g_malloc(nBufSize);
	memset(pBS->pBsBuffer, 0x00, nBufSize);
	pBS->pBsCurByte = pBS->pBsBuffer;
	pBS->bsCurBitOffset = 0;
	pBS->bsByteLen = nBufSize;

	/* allocate and initialize input picture */
	pPic->picWidth = mpeg4enc->mpeg4EncPar.iVolWidth;
	pPic->picHeight = mpeg4enc->mpeg4EncPar.iVolHeight;
	pPic->picPlaneNum = 3;
	pPic->picChannelNum = 3;
	pPic->picFormat = IPP_YCbCr411;

	pPic->picPlaneStep[0] = pPic->picWidth;
	pPic->picPlaneStep[1] = pPic->picPlaneStep[0] >> 1;
	pPic->picPlaneStep[2] = pPic->picPlaneStep[0] >> 1;

	/* calculate the picPixel number and picData size */
	mpeg4enc->picSize = pPic->picWidth * pPic->picHeight;
	mpeg4enc->picDataSize = mpeg4enc->picSize + (mpeg4enc->picSize >> 1);
	mpeg4enc->picquartSize = mpeg4enc->picSize >> 2;

	
	return IPP_STATUS_NOERR;
}

IppCodecStatus
IPP_videoFreeBuffer_MPEG4Enc (GstMPEG4Enc *mpeg4enc)
{
	IppBitstream *pBS = &(mpeg4enc->dstBitStream);
	
	if (pBS->pBsBuffer) {
		g_free(pBS->pBsBuffer);
		pBS->pBsBuffer = NULL;
	}

	return IPP_STATUS_NOERR;
}

void 
IPP_UpdateParMPEG4Encoder (GstMPEG4Enc *mpeg4enc)
{
	IppParSet *pParInfo = &(mpeg4enc->mpeg4EncPar);

	pParInfo->bObjectLayerIdentifier = 1;
	/**
	 * 1 -> version identification and priority is specified for the visual object layer
	 * 0 -> no version identification or priority needs to be specified
     */

	pParInfo->iVolVerID = 2;			
	/* 2 Option: 1 -> Version 1; 2 -> Version 2 */

	pParInfo->iVolPriority = 1;
	/* 7 Option: 1 ~ 7; 
	1 - Highest Priority
	7 - Lowest Priority
	*/

	pParInfo->bVolRandomAcess = 0;
	/* 2 Option: 1 -> every VOP in this VOL is individually decodable
	0 -> any of the VOPs in the VOL are non-intra coded
	*/

	pParInfo->iVideoObjectTypeID = 0x01;
	/* Video Object Type Identification 
	Reserved						0x00
	Simple Object Type				0x01	(Supported)
	Simple Scalable Object Type		0x02	(Not Supported)
	Core Object Type				0x03	(Not Supported)
	Main Object Type				0x04	(Not Supported)
	N-bit Object Type				0x05	(Not Supported)
	Basic Anim.2D Texture			0x06	(Not Supported)
	Anim.2D Mesh					0x07	(Not Supported)
	Simple Face						0x08	(Not Supported)
	Still Scalable Texture			0x09	(Not Supported)
	Advanced Real Time Simple		0x0a	(Not Supported)
	Core Scalable					0x0b	(Not Supported)
	Advanced Coding Efficiency		0x0c	(Not Supported)
	Advanced Scalable Texture		0x0d	(Not Supported)
	Simple FBA						0x0e	(Not Supported)
	Reserved						0x0f ~ 0xff
	*/


	pParInfo->bQuantType = Q_MPEG4;
	/* 2 Option:	Q_H263, Q_MPEG4 */

	pParInfo->bIntraQMatrixDefault = 1;
	/* 2 Option:	1 -> Default Matrix; 0 -> Customer Input Matrix */

	pParInfo->bInterQMatrixDefault = 1;
	/* 2 Option:	1 -> Default Matrix; 0 -> Customer Input Matrix */

	pParInfo->IntraQMatrix = appCustomerIntraQMatrix;
	/* Customer Input Intra Quantization Maxtrix, 64 sub-values */

	pParInfo->InterQMatrix = appCustomerInterQMatrix;
	/* Customer Input Inter Quantization Maxtrix, 64 sub-values */

	pParInfo->iPBetweenI = 20;
	/* Positive integer indicates number of P-frame between two neighbouring I-frames, 
	-1 indicates just one I-frame in starting point, no more I-frame */

	pParInfo->iIntraDcVlcThr = 3;
	/* 8 Option:  0, 1, 2, 3, 4, 5, 6, 7 */

	pParInfo->iIVOPQuantStep = 12;
	/* Legal Values:  1 - 31
	If RC enable, it indicates the initialized QP of I-VOP,
	else it indicates the QP of all I-VOPs
	*/

	pParInfo->iPVOPQuantStep = 14;
	/* Legal Values:  1 - 31
	If RC enable, it indicates the initialized QP of P-VOP,
	else it indicates the QP of all P-VOPs
	*/

	pParInfo->bRoundingCtrlEnable = 1;
	/* 2 Option: 1 -> Enable; 0 -> Disable; */

	pParInfo->iRoundingCtrlInit = 0;
	/* 2 Option: 0, 1 */


	pParInfo->bComplexEstDisable = 1;
	/* 2 Option: 1 -> Disable; 0 -> Enable; */

	pParInfo->bResyncDisable = 1;
	/* 2 Option: 1 -> Disable; 0 -> Enable; */	
	pParInfo->bDataPartitioned = 0;
	/* 2 Option: 1 -> Enable; 0 -> Disable; */	
	pParInfo->iVideoPacketLength = 500;
	/* Posititve Integer indicates the target number of bit in packet */	

	pParInfo->bReverseVLC = 0;
	/* 2 Option: 1 -> Enable; 0 -> Disable; */

	pParInfo->bAdapativeIntraRefreshEnable = 0;
	/* 2 Option: 1 -> Adapative Intra Macroblock Refreshment Enable 
	0 -> Adapative Intra Macroblock Refreshment Disable
	*/
	pParInfo->iAdapativeIntraRefreshRate = 0;
	/* Positive Integer indicates the number of VOPs in which all macroblocks refreshed a round */


	pParInfo->bAdapativeIntraRefreshEnable = 0;	
	/* 2 Option: 1 -> Enable; 0 -> Disable; */

	pParInfo->iAdapativeIntraRefreshRate = 0;
	/* Posititve Integer indicates the number of VOPs in which all macroblocks refreshed a round */	

	/* add for VBV&VOL control support */
	pParInfo->bVolControlPara = 0;
	pParInfo->bVbvParaEnable  = 0;
	/*
	// To enable VBV setting, make sure (bVolControlPara == 1 && bVbvParaEnable ==1)
	// iVbvBitRate:    It is maximum bit rate, recommend value = Max_BitPerFrame x FrameRate
	// iVbvBufferSize: Practical used buffer size for encoder
	// iVbvOccupancy:  Practical used occupancy
	*/
	pParInfo->iVbvBitRate     = pParInfo->iRCBitRate*3;
	/* Maximum_bits_per_frame * Frame_Rate */	
	/* Recommend: pParInfo->iRCBitRate*3 */	
	pParInfo->iVbvBufferSize  = 50;
	/* 
	// Percentage value. must be ">0"  
	// real buffer = pParInfo->iVbvBufferSize * pParInfo->iRCBitRate / 100
	// 50, 100	
	// Recommend: 50  <--->  0.5 second delay  
	*/	
	pParInfo->iVbvOccupancy   = 0;
	/*
	// Percentage value, valid [0, 100].  
	// real occupancy = pParInfo->iVbvOccupancy * real buffer / 100
	// 0, 50 or 200/3 
	// Corresponding buffer position: 0, 1/2, 2/3 
	// Recommend: 0 
	*/	

	pParInfo->iPictureComplexity = 1;
	/* Complexity of Clips, 3 Option: 
	0 -> Low Complexity Clips;
	1 -> Median Complexity Clips;
	2 -> High Complexity Clips;
	*/
}

/******************************************************************************
// GStreamer plug-in element implementation
//
******************************************************************************/
enum {

	/* fill above with user defined signals */
	LAST_SIGNAL
};

enum {
	ARG_0,
	/* fill below with user defined arguments */
	ARG_FRAME_WIDTH,
	ARG_FRAME_HEIGHT,
	ARG_FRAME_RATE,
	ARG_RATE_CONTROL,
	ARG_BIT_RATE,
	
#ifdef DEBUG_PERFORMANCE	
	ARG_TOTALFRAME,	
	ARG_CODECTIME,	
#endif

};

static GstStaticPadTemplate sink_factory = \
	GST_STATIC_PAD_TEMPLATE ("sink", GST_PAD_SINK, GST_PAD_ALWAYS, \
	GST_STATIC_CAPS ("video/x-raw-yuv," \
					 "format = (fourcc) { I420 }, " \
					 "framerate = (fraction) [ 0/1, 60/1 ], " \
					 "width = (int) [ 16, 640 ], " \
					 "height = (int) [ 16, 640 ]" \
					 ));

static GstStaticPadTemplate src_factory = \
	GST_STATIC_PAD_TEMPLATE ("src", GST_PAD_SRC, GST_PAD_ALWAYS, \
	GST_STATIC_CAPS ("video/mpeg, " \
					 "mpegversion = (int) 4, " \
					 "systemstream = (boolean) FALSE, " \
					 "framerate = (fraction) [ 0/1, 60/1 ], " \
					 "width = (int) [ 16, 640 ], " \
					 "height = (int) [ 16, 640 ] "\
					 ));


GST_BOILERPLATE (GstMPEG4Enc, gst_mpeg4enc, GstElement, GST_TYPE_ELEMENT);

static gboolean gst_mpeg4enc_start (GstMPEG4Enc *mpeg4enc );
static gboolean gst_mpeg4enc_stop (GstMPEG4Enc *mpeg4enc );

static gboolean 
gst_mpeg4enc_sinkpad_setcaps (GstPad *pad, GstCaps *caps)
{

	GstMPEG4Enc *mpeg4enc = GST_MPEG4ENC (GST_OBJECT_PARENT (pad));
	int i;
	GValue * value;
	int stru_num = gst_caps_get_size (caps);
	GstStructure *str = gst_caps_get_structure (caps, 0);

	const char *mime = gst_structure_get_name (str);
	if (strcmp (mime, "video/x-raw-yuv") != 0) {
		return FALSE;
	}
	/**
	 * we only accept I420 format YUV, it's the only choice 
	 * in our pad-template caps, so need not do any further check 
	 */

	/* print setted caps*/
	for (i = 0; i < stru_num; i++) {
                str = gst_caps_get_structure (caps, i);
                gchar *sstr;
                sstr = gst_structure_to_string (str);
                GST_INFO( "mpeg4 set caps: %s\n", sstr);
                g_free (sstr);
        }

	if (! gst_structure_get_int(str, "width", &(mpeg4enc->frameWidth))){
		goto caps_set_fail;
	}
	if (! gst_structure_get_int(str, "height", &(mpeg4enc->frameHeight))){
		goto caps_set_fail;
	}
	if ((value = gst_structure_get_value (str, "framerate"))) {
		mpeg4enc->frameRate = gst_value_get_fraction_numerator(value);
	}else{
		goto caps_set_fail;
	}

	mpeg4enc->mpeg4EncPar.iVolWidth = mpeg4enc->frameWidth;
	mpeg4enc->mpeg4EncPar.iVolHeight = mpeg4enc->frameHeight;
	mpeg4enc->mpeg4EncPar.iSrcFrmRate = mpeg4enc->frameRate;
	gst_mpeg4enc_stop (mpeg4enc);
	if(!(gst_mpeg4enc_start (mpeg4enc))){
		goto caps_set_fail;
	}
	return TRUE;

caps_set_fail:
	GST_ERROR("mpeg4enc set caps fail !");
	return FALSE;
}

static GstCaps 
*gst_mpeg4enc_sinkpad_getcaps (GstPad *pad)
{
	GstMPEG4Enc *mpeg4enc = GST_MPEG4ENC (GST_OBJECT_PARENT (pad));
	int i;
	GstStructure *str;
	GstCaps *caps =  gst_caps_copy (gst_pad_get_pad_template_caps (mpeg4enc->sinkpad));
	GstStructure *structure = gst_caps_get_structure (caps, 0);;
	int stru_num = gst_caps_get_size (caps);

       /*if ((gst_structure_get_value (structure, "framerate"))) {
		gst_structure_set (structure, "framerate", GST_TYPE_FRACTION_RANGE,
		0, 1, 60, 1, NULL);
	}*/

	for (i = 0; i < stru_num; i++) {
                str = gst_caps_get_structure (caps, i);
                gchar *sstr;
                sstr = gst_structure_to_string (str);
                GST_INFO ( "mpeg4 get caps: %s\n", sstr);
                g_free (sstr);
        }
	return caps;
}

static void 
gst_mpeg4enc_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	GstMPEG4Enc *mpeg4enc = GST_MPEG4ENC (object);

	switch (prop_id)
	{
	case ARG_FRAME_WIDTH:
		mpeg4enc->frameWidth = g_value_get_int (value);
		if((mpeg4enc->frameWidth <= 0)||(mpeg4enc->frameWidth>640))
		{			
			break;			
		}
		if(0 != (mpeg4enc->frameWidth % 16)){
			mpeg4enc->frameWidth = (mpeg4enc->frameWidth+15) & ~0xf;
			GST_WARNING_OBJECT (mpeg4enc, "frame width should 16 pixels align\n");
		}
		mpeg4enc->mpeg4EncPar.iVolWidth = mpeg4enc->frameWidth;
		break;
	case ARG_FRAME_HEIGHT:
		mpeg4enc->frameHeight = g_value_get_int (value);
		if((mpeg4enc->frameHeight <= 0)||(mpeg4enc->frameHeight>640))
		{			
			break;			
		}
		if(0 != (mpeg4enc->frameHeight % 16)){
			mpeg4enc->frameHeight = (mpeg4enc->frameHeight+15) & ~0xf;
			GST_WARNING_OBJECT (mpeg4enc, "frameHeight should 16 pixels align\n");
		}
		mpeg4enc->mpeg4EncPar.iVolHeight = mpeg4enc->frameHeight;
		break;
	case ARG_FRAME_RATE:
		mpeg4enc->frameRate = g_value_get_int (value);
		if((mpeg4enc->frameRate < 1)||(mpeg4enc->frameRate > 60))
		{
			break;
		}
		mpeg4enc->mpeg4EncPar.iSrcFrmRate = mpeg4enc->frameRate;
		
		break;
	case ARG_RATE_CONTROL:
		mpeg4enc->rateControl = g_value_get_boolean (value);		
		mpeg4enc->mpeg4EncPar.bRCEnable = mpeg4enc->rateControl;
		
		break;
	case ARG_BIT_RATE:
		mpeg4enc->bitRate = g_value_get_int (value);
		if(mpeg4enc->bitRate <= 0)
			break;
		mpeg4enc->mpeg4EncPar.iRCBitRate = mpeg4enc->bitRate;
		
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
		
}

static void 
gst_mpeg4enc_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	GstMPEG4Enc *mpeg4enc = GST_MPEG4ENC (object);

	switch (prop_id)
	{
	case ARG_FRAME_WIDTH:
		g_value_set_int (value, mpeg4enc->frameWidth);
		break;
	case ARG_FRAME_HEIGHT:
		g_value_set_int (value, mpeg4enc->frameHeight);
		break;
	case ARG_FRAME_RATE:
		g_value_set_int (value, mpeg4enc->frameRate);
		break;
	case ARG_RATE_CONTROL:
		g_value_set_boolean (value, mpeg4enc->rateControl);
		break;
	case ARG_BIT_RATE:
		g_value_set_int (value, mpeg4enc->bitRate);
		break;
#ifdef DEBUG_PERFORMANCE
	case ARG_TOTALFRAME:
		g_value_set_int (value, mpeg4enc->totalFrames);		
		break;
	case ARG_CODECTIME:
		g_value_set_int64 (value, mpeg4enc->codec_time);		
		break;
#endif
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}


static gboolean 
gst_mpeg4enc_sink_event (GstPad *pad, GstEvent *event)
{
	GstMPEG4Enc *mpeg4enc = GST_MPEG4ENC (GST_OBJECT_PARENT (pad));
	gboolean eventRet;

	switch (GST_EVENT_TYPE (event))
	{
	case GST_EVENT_NEWSEGMENT:
		mpeg4enc->totalFrames = 0;
		mpeg4enc->streamBegin = TRUE;

		eventRet = gst_pad_push_event (mpeg4enc->srcpad, event);
		break;
	case GST_EVENT_EOS:
		eventRet = gst_pad_push_event (mpeg4enc->srcpad, event);
		break;
	default:
		eventRet = gst_pad_event_default (pad, event);
		break;
	}

	return eventRet;
}

static GstFlowReturn _mpeg4_push_data(GstMPEG4Enc *mpeg4enc)
{
	int ret = 0;
	int len = 0;

	len = (int)(mpeg4enc->dstBitStream.pBsCurByte - mpeg4enc->dstBitStream.pBsBuffer); 
	
	if(len > 0)
	{
		GstBuffer *outBuf = NULL;		
		outBuf = gst_buffer_new_and_alloc(len);
		GST_BUFFER_CAPS (outBuf) = gst_caps_copy (GST_PAD_CAPS (mpeg4enc->srcpad));
		memcpy(GST_BUFFER_DATA (outBuf), mpeg4enc->dstBitStream.pBsBuffer, GST_BUFFER_SIZE (outBuf));
		GST_BUFFER_TIMESTAMP(outBuf) = mpeg4enc->ts;
		GST_BUFFER_DURATION(outBuf) = mpeg4enc->dur;

		ret = gst_pad_push (mpeg4enc->srcpad, outBuf);
		if (GST_FLOW_OK != ret) {			
			GST_INFO_OBJECT (mpeg4enc, "mpeg4enc push error");					
			return ret;
		}	
	}
	mpeg4enc->dstBitStream.pBsCurByte = mpeg4enc->dstBitStream.pBsBuffer;	
	mpeg4enc->dstBitStream.bsCurBitOffset = 0;

	return GST_FLOW_OK;
	
}


static GstFlowReturn 
gst_mpeg4enc_chain (GstPad *pad, GstBuffer *buf)
{
	GstMPEG4Enc *mpeg4enc = GST_MPEG4ENC (GST_OBJECT_PARENT (pad));

	int ret = 0;
	int left = 0;
	int offset = 0;
	guint8 *data = NULL;	

	if((int)(GST_BUFFER_DATA (buf)) & 0x7){
		GST_ERROR_OBJECT (mpeg4enc, "address is not 8 bytes aligned");
		ret = GST_FLOW_ERROR;
		goto err;		
	}	

	/* buffer + remaining data */

	mpeg4enc->ts = GST_BUFFER_TIMESTAMP(buf);
	mpeg4enc->dur = GST_BUFFER_DURATION(buf);
	if (mpeg4enc->tempbuf) {		
		buf = gst_buffer_join (mpeg4enc->tempbuf, buf);
		mpeg4enc->tempbuf = NULL;
	}

	left = GST_BUFFER_SIZE (buf);
	data = GST_BUFFER_DATA (buf);	

	while(left >= mpeg4enc->picDataSize)	
	{

		mpeg4enc->srcPicture.ppPicPlane[0] = data;
		data += mpeg4enc->picSize;
		
		mpeg4enc->srcPicture.ppPicPlane[1] = data;
		data += mpeg4enc->picquartSize;
		
		mpeg4enc->srcPicture.ppPicPlane[2] = data;
		data += mpeg4enc->picquartSize;
	
		//data += h264enc->picDataSize;		
		offset += mpeg4enc->picDataSize;		
		left -= mpeg4enc->picDataSize;		
#ifdef DEBUG_PERFORMANCE
                        gettimeofday (&tstart, &tz);
#endif

		ret = Encode_MPEG4Video(&(mpeg4enc->dstBitStream), &(mpeg4enc->srcPicture), mpeg4enc->pMPEG4EncState);
#ifdef DEBUG_PERFORMANCE

                        gettimeofday (&tend, &tz);
                        mpeg4enc->codec_time += (tend.tv_sec - tstart.tv_sec)*1000000 + (tend.tv_usec - tstart.tv_usec);
#endif

		if (IPP_STATUS_NOERR != ret){
			GST_ERROR_OBJECT (mpeg4enc, "codec error= %d\n", ret);
			ret = GST_FLOW_ERROR;
			goto err;
		}
		
		if (mpeg4enc->streamBegin) {
			/* set srcpad caps */
			GstCaps *Tmp;
			Tmp = gst_caps_new_simple ("video/mpeg", 
				"mpegversion", G_TYPE_INT, 4, 
				"systemstream", G_TYPE_BOOLEAN, FALSE, 
				"width", G_TYPE_INT, mpeg4enc->frameWidth, 
				"height", G_TYPE_INT, mpeg4enc->frameHeight, 
				"framerate", GST_TYPE_FRACTION, mpeg4enc->frameRate, 1,
				NULL);
			gst_pad_set_caps(mpeg4enc->srcpad, Tmp);
			gst_caps_unref(Tmp);

			mpeg4enc->streamBegin = FALSE;
		}

		ret = _mpeg4_push_data(mpeg4enc);
		if (GST_FLOW_OK != ret) {		
			GST_ERROR_OBJECT (mpeg4enc, "mpeg4enc push error");					
			goto err;
		}
		mpeg4enc->totalFrames ++;			

	}

	if (left > 0) {					
		if (left < GST_BUFFER_SIZE (buf)) {
			mpeg4enc->tempbuf = gst_buffer_create_sub (buf, offset, left);			
		} else {
			mpeg4enc->tempbuf = buf;
			gst_buffer_ref (buf);			
		}
	}

	gst_buffer_unref (buf);	
	return GST_FLOW_OK;

err:
	gst_buffer_unref (buf);	
	return ret;
	
}

static gboolean
gst_mpeg4enc_start (GstMPEG4Enc *mpeg4enc )
{	
	int ret = 0;

	miscInitGeneralCallbackTable(&(mpeg4enc->pCBTable));
	IPP_videoInitBuffer_MPEG4Enc (mpeg4enc);	
	IPP_UpdateParMPEG4Encoder (mpeg4enc);

	ret = EncoderInitAlloc_MPEG4Video(&(mpeg4enc->dstBitStream), 
		&(mpeg4enc->mpeg4EncPar), mpeg4enc->pCBTable, NULL, &(mpeg4enc->pMPEG4EncState));
			
	if (IPP_STATUS_NOERR != ret){
		goto err;
	}

	mpeg4enc->tempbuf = NULL;
	mpeg4enc->streamBegin = TRUE;


	return TRUE;	

err:
	IPP_videoFreeBuffer_MPEG4Enc(mpeg4enc);
	
	return FALSE;
	
}

static gboolean
gst_mpeg4enc_stop (GstMPEG4Enc *mpeg4enc )
{	

	EncoderFree_MPEG4Video(&(mpeg4enc->pMPEG4EncState));
	IPP_videoFreeBuffer_MPEG4Enc(mpeg4enc);

	if(mpeg4enc->pCBTable){		
		miscFreeGeneralCallbackTable(&(mpeg4enc->pCBTable));
		mpeg4enc->pCBTable = NULL;
	}		
	
	if (mpeg4enc->tempbuf != NULL) {
		gst_buffer_unref (mpeg4enc->tempbuf);
		mpeg4enc->tempbuf = NULL;
	}

	return TRUE;
	
}

static GstStateChangeReturn 
gst_mpeg4enc_change_state (GstElement *element, GstStateChange transition)
{
	GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;
	GstMPEG4Enc *mpeg4enc = GST_MPEG4ENC (element);

	switch (transition)
	{
	case GST_STATE_CHANGE_NULL_TO_READY:		

		if(!gst_mpeg4enc_start(mpeg4enc)){
			goto statechange_failed;
		}
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
		
		if(!gst_mpeg4enc_stop(mpeg4enc)){
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
		GST_ERROR_OBJECT (mpeg4enc, "state change failed");
		return GST_STATE_CHANGE_FAILURE;
	}

}

static void 
gst_mpeg4enc_base_init (gpointer g_klass)
{
	static GstElementDetails mpeg4enc_details = {
		"MPEG-4 Video Encoder", 
		"Codec/Encoder/Video", 
		"MPEG-4 Video Encoder based-on IPP & CODEC", 
		""
	};
	GstElementClass *element_class = GST_ELEMENT_CLASS (g_klass);

	gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&src_factory));
	gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&sink_factory));

	gst_element_class_set_details (element_class, &mpeg4enc_details);
}

static void 
gst_mpeg4enc_class_init (GstMPEG4EncClass *klass)
{
	GObjectClass *gobject_class  = (GObjectClass*) klass;
	GstElementClass *gstelement_class = (GstElementClass*) klass;

	parent_class = g_type_class_ref (GST_TYPE_ELEMENT);

	gobject_class->set_property = gst_mpeg4enc_set_property;
	gobject_class->get_property = gst_mpeg4enc_get_property;

	/* add arguments to the element */
	g_object_class_install_property (gobject_class, ARG_FRAME_WIDTH, \
		g_param_spec_int ("frame_width", "The width of the YUV sequences", \
		"The width of the YUV sequences which will be encoded", \
		0/* range_MIN */, 640/* range_MAX */, 176/* default_INIT */, G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, ARG_FRAME_HEIGHT, \
		g_param_spec_int ("frame_height", "The height of the YUV sequences", \
		"The height of the YUV sequences which will be encoded", \
		0/* range_MIN */, 640/* range_MAX */, 144/* default_INIT */, G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, ARG_FRAME_RATE, \
		g_param_spec_int ("frame_rate", "The frame rate of the YUV sequences", \
		"The frame rate of the YUV sequences which will be encoded", \
		0/* range_MIN */, 60/* range_MAX */, 30/* default_INIT */, G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, ARG_RATE_CONTROL, \
		g_param_spec_boolean ("rate_control", "Whether doing rate control", \
		"Whether doing rate control when encoding", \
		TRUE/* default_INIT */, G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, ARG_BIT_RATE, \
		g_param_spec_int ("bit_rate", "The bit rate of encoded stream", \
		"The target bit rate of the output encoded stream", \
		0/* range_MIN */, G_MAXINT/* range_MAX */, 128000/* default_INIT */, G_PARAM_READWRITE));

#ifdef DEBUG_PERFORMANCE	

	g_object_class_install_property (gobject_class, ARG_TOTALFRAME,
  		g_param_spec_int ("totalframes", "Totalframe of mpeg4stream",
          	"Number of frame", 0, G_MAXINT, 0, G_PARAM_READABLE));
	g_object_class_install_property (gobject_class, ARG_CODECTIME,
  		g_param_spec_int64 ("codectime", "mpeg4 encode time",
          	"codec time (microsecond)", 0, G_MAXINT64, 0, G_PARAM_READABLE));
#endif


	gstelement_class->change_state = gst_mpeg4enc_change_state;

	GST_DEBUG_CATEGORY_INIT (mpeg4enc_debug, "mpeg4enc", 0, "MPEG-4 Encode Element");
}


static void 
gst_mpeg4enc_init (GstMPEG4Enc *mpeg4enc, GstMPEG4EncClass *mpeg4enc_klass)
{
	GstElementClass *klass = GST_ELEMENT_CLASS (mpeg4enc_klass);

	mpeg4enc->sinkpad = gst_pad_new_from_template (\
		gst_element_class_get_pad_template (klass, "sink"), "sink");

	gst_pad_set_setcaps_function (mpeg4enc->sinkpad, gst_mpeg4enc_sinkpad_setcaps);
	gst_pad_set_getcaps_function (mpeg4enc->sinkpad, gst_mpeg4enc_sinkpad_getcaps);
	gst_pad_set_event_function (mpeg4enc->sinkpad, gst_mpeg4enc_sink_event);
	gst_pad_set_chain_function (mpeg4enc->sinkpad, gst_mpeg4enc_chain);

	gst_element_add_pad (GST_ELEMENT (mpeg4enc), mpeg4enc->sinkpad);


	mpeg4enc->srcpad = gst_pad_new_from_template (\
		gst_element_class_get_pad_template (klass, "src"), "src");

	
	gst_element_add_pad (GST_ELEMENT (mpeg4enc), mpeg4enc->srcpad);

	mpeg4enc->pCBTable = NULL;
	mpeg4enc->pMPEG4EncState = NULL;
	memset(&(mpeg4enc->mpeg4EncPar), 0x00, sizeof(IppParSet));
	memset(&(mpeg4enc->dstBitStream), 0x00, sizeof(IppBitstream));
	memset(&(mpeg4enc->srcPicture), 0x00, sizeof(IppPicture));

	mpeg4enc->frameWidth = 176;
	mpeg4enc->mpeg4EncPar.iVolWidth = 176;
	mpeg4enc->frameHeight = 144;
	mpeg4enc->mpeg4EncPar.iVolHeight = 144;
	mpeg4enc->frameRate = 30;
	mpeg4enc->mpeg4EncPar.iSrcFrmRate = 30;
	mpeg4enc->mpeg4EncPar.iSrcSamRate = 1;
	mpeg4enc->rateControl = 1;
	mpeg4enc->mpeg4EncPar.bRCEnable = TRUE;
	mpeg4enc->bitRate = 128000;
	mpeg4enc->mpeg4EncPar.iRCBitRate = 128000;

	mpeg4enc->mpeg4EncPar.iColorFormat = IPP_YCbCr411;
	mpeg4enc->codec_time = 0;
	
}


static gboolean 
plugin_init (GstPlugin *plugin)
{	
	return gst_element_register (plugin, "mpeg4enc", GST_RANK_PRIMARY, GST_TYPE_MPEG4ENC);
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR, 
                   GST_VERSION_MINOR, 
                   "mvl_mpeg4enc", 
                   "mpeg4 encoder based on IPP", 
                   plugin_init, 
                   VERSION, 
                   "Proprietary", 
                   "", 
                   "");




/* EOF */
