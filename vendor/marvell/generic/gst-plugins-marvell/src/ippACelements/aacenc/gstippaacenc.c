
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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gstippaacenc.h"
#include <string.h>

#define IPPGST_AACENC_FRAME_SIZE	AAC_FRAME_SIZE
#define IPPGST_AACENC_MIN_BPS		8000	//8kbps
#define IPPGST_AACENC_DEFAULT_BPS	96000	//96kbps
#define IPPGST_AACENC_MAX_BPS		320000	//320kbps

#ifdef DEBUG_PERFORMANCE
#include  <sys/time.h>
        struct timeval tstart, tend;
        struct timezone tz;
#endif

#if 0
#define gippaacenc_printf(...)		printf(__VA_ARGS__)
#else
#define gippaacenc_printf(...)
#endif

GST_DEBUG_CATEGORY_STATIC(ippaaclcenc_debug);
#define GST_CAT_DEFAULT ippaaclcenc_debug


enum {
	/* fill above with user defined signals */
	LAST_SIGNAL
};

enum {
	ARG_0,
	/* fill below with user defined arguments */
	ARG_BITRATE,
	ARG_BSFORMAT,
	ARG_TNSENABLE,
	ARG_OUTPUTENDBS,
#ifdef DEBUG_PERFORMANCE
	ARG_TOTALFRAME,
	ARG_CODECTIME,
#endif
};


/* the capabilities of the inputs and outputs.
 *
 * describe the real formats here.
 */
static GstStaticPadTemplate sink_factory = \
	GST_STATIC_PAD_TEMPLATE ("sink", GST_PAD_SINK, GST_PAD_ALWAYS,
	GST_STATIC_CAPS ("audio/x-raw-int, "
 					 "endianness = (int) BYTE_ORDER, "
					 "signed = (boolean) true, "
					 "width = (int) 16, "
					 "depth = (int) 16, "
					 "rate = (int) {16000, 44100, 48000},"
					 "channels = (int) [ 1, 2 ]")
					 );

static GstStaticPadTemplate src_factory = \
	GST_STATIC_PAD_TEMPLATE ("src", GST_PAD_SRC, GST_PAD_ALWAYS,
	GST_STATIC_CAPS ("audio/mpeg"
					 ",mpegversion = (int) 4 "
					 ",rate = (int) {16000, 44100, 48000} "
					 ",channels = (int) [ 1, 2 ] "
					 ",stream-format = (string) { adts, raw } "
 					 ",framed = (boolean) true "
					 ",bitrate = (int) [" G_STRINGIFY(IPPGST_AACENC_MIN_BPS) "," G_STRINGIFY(IPPGST_AACENC_MAX_BPS) "]")
					 );

GST_BOILERPLATE (GstippaacLCenc, gst_ippaaclcenc, GstElement, GST_TYPE_ELEMENT);


static IppCodecStatus
IPP_FreeBuffer_AACEnc (GstippaacLCenc *aacenc)
{
	IppBitstream *pBufInfo = &(aacenc->dstbitStream);
	if (pBufInfo->pBsBuffer) {
		g_free (pBufInfo->pBsBuffer);
		pBufInfo->pBsBuffer = NULL;
	}
	if(aacenc->leftPcmBuf) {
		IPP_MemFree((void**)(&aacenc->leftPcmBuf));
		aacenc->leftPcmBuf = NULL;
	}

	return IPP_STATUS_NOERR;
}


static IppCodecStatus
IPP_InitBuffer_AACEnc (GstippaacLCenc *aacenc)
{
/* ippsound src will be init during the chain funcs*/
	int nBsBufSize = 0;
	IppBitstream *pBufInfo = &(aacenc->dstbitStream);
	/* allocate and initialize output bitstream */
	nBsBufSize = IPPGST_AACENC_FRAME_SIZE * sizeof(Ipp16s) * (aacenc->channels);
	if(nBsBufSize < 3840) {
		nBsBufSize = 3840;	//3840 is calculated from IPPGST_AACENC_MAX_BPS/(16000/IPPGST_AACENC_FRAME_SIZE)/8*1.5
	}

	pBufInfo->pBsBuffer = (Ipp8u*)g_malloc(nBsBufSize);
	//memset(pBufInfo->pBsBuffer, 0x00, nBsBufSize);
	pBufInfo->pBsCurByte = pBufInfo->pBsBuffer;
	pBufInfo->bsCurBitOffset = 0;
	pBufInfo->bsByteLen = nBsBufSize;

	IPP_MemMalloc((void**)(&aacenc->leftPcmBuf), aacenc->PCMOneFrameSize, 4);

	return IPP_STATUS_NOERR;
}

static gboolean
aac_deinit (GstippaacLCenc *aacenc)
{
	if(aacenc->pEncoderState) {
		EncoderFree_AAC(&(aacenc->pEncoderState));
		aacenc->pEncoderState = NULL;
	}
	if(aacenc->pCallBackTable) {
		miscFreeGeneralCallbackTable(&(aacenc->pCallBackTable));
		aacenc->pCallBackTable = NULL;
	}
	IPP_FreeBuffer_AACEnc(aacenc);
	return TRUE;
}

static gboolean
aac_init(GstippaacLCenc *aacenc)
{
	IppCodecStatus ret;
	IppAACEncoderConfig	encoderConfig;
	encoderConfig.samplingRate = aacenc->Samplerate;
	encoderConfig.channelConfiguration = aacenc->channels;
	encoderConfig.bitRate = aacenc->bitrate;
	encoderConfig.bsFormat = aacenc->bitstmFormat == 1 ? 1 : 6;	//encoderConfig.bsFormat=1 ADTS, encoderConfig.bsFormat=6 Raw

	gippaacenc_printf("aac encoder config sample rate %d, channel %d, bps %d, bsFormat %d\n", encoderConfig.samplingRate, encoderConfig.channelConfiguration, encoderConfig.bitRate, encoderConfig.bsFormat);

	IPP_InitBuffer_AACEnc(aacenc);
	miscInitGeneralCallbackTable(&(aacenc->pCallBackTable));
	ret = EncoderInitAlloc_AAC(&encoderConfig,(aacenc->pCallBackTable),&(aacenc->pEncoderState));
	if(IPP_STATUS_INIT_OK != ret){
		EncoderFree_AAC(&(aacenc->pEncoderState));
		aacenc->pEncoderState = NULL;
		miscFreeGeneralCallbackTable(&(aacenc->pCallBackTable));
		aacenc->pCallBackTable = NULL;
		IPP_FreeBuffer_AACEnc(aacenc);
		GST_ERROR_OBJECT (aacenc, "call EncoderInitAlloc_AAC() at line %d fail, ret %d", __LINE__, ret);
		return FALSE;
	}

	if(aacenc->tnsEn == 1) {
		ret = EncodeSendCmd_AAC(IPPAC_AAC_SET_TNS_MODE, &(aacenc->tnsEn), NULL, (aacenc->pEncoderState));
		if(ret != IPP_STATUS_NOERR) {
			EncoderFree_AAC(&(aacenc->pEncoderState));
			aacenc->pEncoderState = NULL;
			miscFreeGeneralCallbackTable(&(aacenc->pCallBackTable));
			aacenc->pCallBackTable = NULL;
			IPP_FreeBuffer_AACEnc(aacenc);
			GST_ERROR_OBJECT (aacenc, "call EncodeSendCmd_AAC() at line %d fail, ret %d", __LINE__, ret);
			return FALSE;
		}
	}

	aacenc->totalFrames = 0;
	aacenc->iEncodedBytesLen = 0;
	aacenc->codec_time = 0;

	return TRUE;
}


static gboolean gst_ippaaclcenc_sinkpad_setcaps (GstPad * pad, GstCaps * caps)
{
	GstippaacLCenc* aacenc = GST_IPPAACLCENC(GST_OBJECT_PARENT (pad));

	GstStructure * str = gst_caps_get_structure(caps, 0);
	const char * mime = gst_structure_get_name (str);
	if (strcmp (mime, "audio/x-raw-int") != 0) {
		return FALSE;
	}

	if (! gst_structure_get_int(str, "rate", &(aacenc->Samplerate))){
		goto caps_set_fail;
	}
	if (! gst_structure_get_int(str, "channels", &(aacenc->channels))){
		goto caps_set_fail;
	}

	GstCaps *TmpCap = gst_caps_new_simple ("audio/mpeg",
		"mpegversion", G_TYPE_INT, 4,
		"rate", G_TYPE_INT, aacenc->Samplerate,
		"channels", G_TYPE_INT, aacenc->channels,
		"stream-format", G_TYPE_STRING, (aacenc->bitstmFormat == 1 ? "adts" : "raw"),
		"framed", G_TYPE_BOOLEAN, TRUE,
		"bitrate", G_TYPE_INT, aacenc->bitrate,
		NULL);


	if (aacenc->bitstmFormat == 2) {	//only for raw
		GstBuffer *codec_data;
		guint8 config[2];
		gulong config_len = sizeof(config)/sizeof(config[0]);

		// codec_data is part information, http://wiki.multimedia.cx/index.php?title=Understanding_AAC
		// object type (5 bits)
		// rate_index 4 bits
		// channel 4 bits
		unsigned int object_type = 2; //MAIN 1, LC 2, SSR 3, LTP 4
		unsigned int rate_index;	//AAC sampling frequency table,	const int fs[12]={96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050, 16000, 12000, 11025, 8000 };
		unsigned int channel = aacenc->channels;
		if(aacenc->Samplerate == 16000) {
			rate_index = 8;
		}else if(aacenc->Samplerate == 44100) {
			rate_index = 4;
		}else{
			rate_index = 3;
		}
		config[0] = (object_type<<3) | (rate_index>>1);
		config[1] = (rate_index<<7) | (channel<<3);

		/* copy it into a gst buffer */
		codec_data = gst_buffer_new_and_alloc(config_len);
		memcpy(GST_BUFFER_DATA(codec_data), config, config_len);

		/* add to caps */
		gst_caps_set_simple(TmpCap,	"codec_data", GST_TYPE_BUFFER, codec_data, NULL);

		gst_buffer_unref(codec_data);
	}


	if(!gst_pad_set_caps(aacenc->srcpad, TmpCap)) {
		gst_caps_unref(TmpCap);
		GST_ERROR_OBJECT(aacenc, "Set caps on srcpad fail!");
		return FALSE;
	}
	gst_caps_unref(TmpCap);
	return TRUE;

caps_set_fail:
	return FALSE;
}

static void gst_ippaaclcenc_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
	GstippaacLCenc *aacenc = GST_IPPAACLCENC (object);
	switch (prop_id)
	{
		case ARG_BITRATE:
			aacenc->bitrate = g_value_get_int (value);
			break;
		case ARG_BSFORMAT:
			aacenc->bitstmFormat = g_value_get_int (value);
			break;
		case ARG_TNSENABLE:
			aacenc->tnsEn = g_value_get_int (value);
			break;
		case ARG_OUTPUTENDBS:
			aacenc->ForceOutputEndBS = g_value_get_int (value);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
	return;
}

static void gst_ippaaclcenc_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
	GstippaacLCenc* aacenc = GST_IPPAACLCENC (object);
	switch (prop_id)
	{
	case ARG_BITRATE:
		g_value_set_int (value, aacenc->bitrate);
		break;
	case ARG_BSFORMAT:
		g_value_set_int (value, aacenc->bitstmFormat);
		break;
	case ARG_TNSENABLE:
		g_value_set_int (value, aacenc->tnsEn);
		break;
	case ARG_OUTPUTENDBS:
		g_value_set_int (value, aacenc->ForceOutputEndBS);
		break;

#ifdef DEBUG_PERFORMANCE
	case ARG_TOTALFRAME:
		g_value_set_int (value, aacenc->totalFrames);
		break;
	case ARG_CODECTIME:
		g_value_set_int64 (value, aacenc->codec_time);
		break;
#endif

	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
	return;
}

static GstFlowReturn _aac_push_data(GstippaacLCenc *aacenc)
{
	GstFlowReturn ret = GST_FLOW_OK;
	GstBuffer *outBuf = NULL;
	int len = aacenc->dstbitStream.pBsCurByte - aacenc->dstbitStream.pBsBuffer;
	if(len > 0){
		aacenc->totalFrames ++;
		aacenc->iEncodedBytesLen += len;

		outBuf = gst_buffer_new_and_alloc (len);
  		gst_buffer_set_caps (outBuf, GST_PAD_CAPS (aacenc->srcpad));
		memcpy(GST_BUFFER_DATA (outBuf), aacenc->dstbitStream.pBsBuffer, len);
		aacenc->dstbitStream.pBsCurByte = aacenc->dstbitStream.pBsBuffer;
		aacenc->dstbitStream.bsCurBitOffset = 0;

		GST_BUFFER_TIMESTAMP(outBuf) = aacenc->ts;
		GST_BUFFER_DURATION(outBuf) = aacenc->dur;

		if (aacenc->ts != GST_CLOCK_TIME_NONE && aacenc->dur != GST_CLOCK_TIME_NONE) {
			aacenc->ts += aacenc->dur;
		}

		gippaacenc_printf("aacenc push down buf sz %d, ts %lld, du %lld\n", GST_BUFFER_SIZE(outBuf), GST_BUFFER_TIMESTAMP(outBuf), GST_BUFFER_DURATION(outBuf));
		ret = gst_pad_push (aacenc->srcpad, outBuf);
		if (GST_FLOW_OK != ret) {
			GST_WARNING_OBJECT (aacenc, "aacenc push error ret=%d\n", ret);
		}
	}

	return ret;
}

static void _aac_consume_data_nopush(GstippaacLCenc *aacenc)
{
	int len = aacenc->dstbitStream.pBsCurByte - aacenc->dstbitStream.pBsBuffer;
	if(len > 0){
		aacenc->totalFrames ++;
		aacenc->iEncodedBytesLen += len;

		aacenc->dstbitStream.pBsCurByte = aacenc->dstbitStream.pBsBuffer;
		aacenc->dstbitStream.bsCurBitOffset = 0;

	    if (aacenc->ts != GST_CLOCK_TIME_NONE && aacenc->dur != GST_CLOCK_TIME_NONE)
			aacenc->ts += aacenc->dur;
	}

	return;
}


static GstFlowReturn
gst_ippaaclcenc_chain (GstPad * pad, GstBuffer * buf)
{
	GstippaacLCenc *aacenc = GST_IPPAACLCENC(GST_OBJECT_PARENT (pad));
	IppCodecStatus ret;
	GstFlowReturn flowRt = GST_FLOW_OK;
	IppSound srcsound;
	int datalen = GST_BUFFER_SIZE (buf);
	guint8 *data = GST_BUFFER_DATA (buf);

	//gippaacenc_printf("enter %s(), ts %lld, du %lld, sz %d\n", __FUNCTION__, GST_BUFFER_TIMESTAMP(buf), GST_BUFFER_DURATION(buf), GST_BUFFER_SIZE(buf));

	if(GST_BUFFER_TIMESTAMP_IS_VALID (buf) && aacenc->ts == GST_CLOCK_TIME_NONE) {
		aacenc->ts = GST_BUFFER_TIMESTAMP (buf);
	}

	if(aacenc->pEncoderState == NULL) {
		aacenc->PCMOneFrameSize = sizeof(Ipp16s)*IPPGST_AACENC_FRAME_SIZE*aacenc->channels;
		aacenc->leftPcmSize = 0;
		if(aac_init(aacenc) != TRUE) {
			gst_buffer_unref(buf);
			return GST_FLOW_ERROR;
		}
		aacenc->dur = gst_util_uint64_scale_int(GST_SECOND, IPPGST_AACENC_FRAME_SIZE, aacenc->Samplerate);
		if(GST_BUFFER_TIMESTAMP_IS_VALID (buf)) {
			aacenc->ts = GST_BUFFER_TIMESTAMP (buf);
		}
	}

	if(aacenc->leftPcmSize > 0) {
		int fill = aacenc->PCMOneFrameSize - aacenc->leftPcmSize;
		if(datalen < fill) {
			fill = datalen;
		}
		memcpy(aacenc->leftPcmBuf + aacenc->leftPcmSize, data, fill);
		datalen -= fill;
		data += fill;
		aacenc->leftPcmSize += fill;
	}

	srcsound.sndChannelNum = aacenc->channels;
	srcsound.sndLen = aacenc->PCMOneFrameSize;

	while(aacenc->leftPcmSize == aacenc->PCMOneFrameSize || datalen >= aacenc->PCMOneFrameSize) {
		if(aacenc->leftPcmSize == aacenc->PCMOneFrameSize) {
			srcsound.pSndFrame = (Ipp16s*)aacenc->leftPcmBuf;
			aacenc->leftPcmSize = 0;
		}else{
			srcsound.pSndFrame = (Ipp16s*)data;
			data += aacenc->PCMOneFrameSize;
			datalen -= aacenc->PCMOneFrameSize;
		}
#ifdef DEBUG_PERFORMANCE
        gettimeofday (&tstart, &tz);
#endif
		ret = Encode_AAC(&srcsound, &(aacenc->dstbitStream), (aacenc->pEncoderState));
#ifdef DEBUG_PERFORMANCE
        gettimeofday (&tend, &tz);
        aacenc->codec_time += (tend.tv_sec - tstart.tv_sec)*1000000 + (tend.tv_usec - tstart.tv_usec);
#endif

		switch(ret) {
		case IPP_STATUS_FRAME_COMPLETE:
		case IPP_STATUS_NOERR:
			if(flowRt == GST_FLOW_OK) {
				flowRt = _aac_push_data(aacenc);
			}else{
				_aac_consume_data_nopush(aacenc);
			}
			break;
		default:
			gst_buffer_unref(buf);
			GST_ERROR_OBJECT (aacenc, "call Encode_AAC() fail, ret %d", ret);
			return GST_FLOW_ERROR;
		}
	}
	if(datalen > 0) {
		memcpy(aacenc->leftPcmBuf + aacenc->leftPcmSize, data, datalen);
		aacenc->leftPcmSize += datalen;
	}
	gst_buffer_unref(buf);

	//gippaacenc_printf("leave %s()\n", __FUNCTION__);
	return flowRt;
}

static GstStateChangeReturn
gst_ippaaclcenc_change_state (GstElement *element, GstStateChange transition)
{
	GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;
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
			{
				gippaacenc_printf("aacenc ready 2 null\n");
				GstippaacLCenc *aacenc = GST_IPPAACLCENC(element);
				GST_INFO_OBJECT (aacenc, "aacenc GST_STATE_CHANGE_READY_TO_NULL");
				aac_deinit(aacenc);
#ifdef DEBUG_PERFORMANCE
				printf("ipp aac encoder output %d frames(%lld bytes), totally cost %lld us\n", aacenc->totalFrames, aacenc->iEncodedBytesLen, aacenc->codec_time);
#endif
			}
			break;
		default:
			break;
	}

	return ret;
}

static gboolean
gst_ippaaclcenc_sinkpad_event (GstPad *pad, GstEvent *event)
{
	GstippaacLCenc *aacenc = GST_IPPAACLCENC (GST_OBJECT_PARENT (pad));
	gboolean eventRet;

	switch (GST_EVENT_TYPE (event))
	{
	case GST_EVENT_NEWSEGMENT:
		aacenc->ts = GST_CLOCK_TIME_NONE;
		eventRet = gst_pad_push_event (aacenc->srcpad, event);
		break;
	case GST_EVENT_EOS:
		{
			gippaacenc_printf("GST_EVENT_EOS is arrived\n");
			if(aacenc->pEncoderState != NULL && aacenc->ForceOutputEndBS == 1) {
				IppSound sound;
				int appendedZeroSize = 0;
				IppCodecStatus ret;
				GstFlowReturn flowRt = GST_FLOW_OK;
				//because AAC encoder has 1600 sample delay, must appending 1600 sample zero.
				aacenc->ts = GST_CLOCK_TIME_NONE;
				aacenc->dur = GST_CLOCK_TIME_NONE;

				sound.sndChannelNum = aacenc->channels*sizeof(Ipp16s);
				sound.sndLen = aacenc->PCMOneFrameSize;
				sound.pSndFrame = (Ipp16s*)aacenc->leftPcmBuf;
				while(appendedZeroSize < (int)(1600*aacenc->channels*sizeof(Ipp16s))) {
					memset(aacenc->leftPcmBuf+aacenc->leftPcmSize, 0, aacenc->PCMOneFrameSize-aacenc->leftPcmSize);
					appendedZeroSize += aacenc->PCMOneFrameSize-aacenc->leftPcmSize;
					aacenc->leftPcmSize = 0;
#ifdef DEBUG_PERFORMANCE
					gettimeofday (&tstart, &tz);
#endif
					ret = Encode_AAC(&sound, &(aacenc->dstbitStream), (aacenc->pEncoderState));
#ifdef DEBUG_PERFORMANCE
					gettimeofday (&tend, &tz);
					aacenc->codec_time += (tend.tv_sec - tstart.tv_sec)*1000000 + (tend.tv_usec - tstart.tv_usec);
#endif
					gippaacenc_printf("Encode_AAC ret %d in EOS\n", ret);
					if(ret == IPP_STATUS_FRAME_COMPLETE || ret == IPP_STATUS_NOERR) {
						if(flowRt == GST_FLOW_OK) {
							flowRt = _aac_push_data(aacenc);
						}else{
							_aac_consume_data_nopush(aacenc);
						}
					}else{
						//error
						break;
					}
				}
			}
			gippaacenc_printf("will call aacdeinit in EOS\n");
			aac_deinit(aacenc);
		}
		eventRet = gst_pad_push_event (aacenc->srcpad, event);
		break;
	default:
		eventRet = gst_pad_event_default (pad, event);
		break;
	}
	return eventRet;
}

/* GObject vmethod implementations */

static void
gst_ippaaclcenc_base_init (gpointer gclass)
{
  	static GstElementDetails aaclcenc_details = {
  		"AACLC Audio Encoder (IPP)",
		"Codec/Encoder/Audio",
		"AACLC Audio Encoder based-on IPP & CODEC",
		""
	};


	GstElementClass *element_class = GST_ELEMENT_CLASS (gclass);

	gst_element_class_add_pad_template (element_class,gst_static_pad_template_get (&src_factory));
	gst_element_class_add_pad_template (element_class,gst_static_pad_template_get (&sink_factory));
	gst_element_class_set_details (element_class, &aaclcenc_details);
	return;
}

/* initialize the aaclcenc's class */
static void
gst_ippaaclcenc_class_init (GstippaacLCencClass * klass)
{
	GObjectClass *gobject_class = (GObjectClass *) klass;
	GstElementClass *gstelement_class = (GstElementClass *) klass;

	gobject_class->set_property = gst_ippaaclcenc_set_property;
	gobject_class->get_property = gst_ippaaclcenc_get_property;

	g_object_class_install_property (gobject_class, ARG_BITRATE, \
		g_param_spec_int ("bitrate", "The bitrate of the AACLC bitstream", \
		"The target bitrate(bits per second) of compressed aac stream",\
		IPPGST_AACENC_MIN_BPS/* range_MIN */, IPPGST_AACENC_MAX_BPS/* range_MAX */, IPPGST_AACENC_DEFAULT_BPS/* default_INIT */, G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, ARG_BSFORMAT, \
		g_param_spec_int ("bsformat", "The format of the encoding AACLC bitstream", \
		"The compressed AAC stream format, support 1:MP4ADTS or 2:RAW", \
		1/* range_MIN*/, 2/* range_MAX */, 2/* default_INIT */, G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, ARG_TNSENABLE, \
		g_param_spec_int ("tnsenable", "The tns mode of compressed AAC stream", \
		"Enable TNS in compressed AAC stream, 0: disable, 1: enable", \
		0/* range_MIN */, 1/* range_MAX */, 0/* default_INIT */, G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, ARG_OUTPUTENDBS, \
		g_param_spec_int ("force-output-endBs", "Force output the end bitstream", \
		"Force output end bitstream, aac encoder outputs additional stream at the end of encoding because of encoding delay. 0: not output, 1: output", \
		0/* range_MIN */, 1/* range_MAX */, 0/* default_INIT */, G_PARAM_READWRITE));

#ifdef DEBUG_PERFORMANCE
	g_object_class_install_property (gobject_class, ARG_TOTALFRAME,
  		g_param_spec_int ("totalframes", "Totalframe of aaclc encoded stream",
          	"Number of frame AAC encoder output", 0, G_MAXINT, 0, G_PARAM_READABLE));
	g_object_class_install_property (gobject_class, ARG_CODECTIME,
  		g_param_spec_int64 ("codectime", "ipp aaclcenc codec spent time",
          	"Total pure encoder spend system time(microsecond)", 0, G_MAXINT64, 0, G_PARAM_READABLE));
#endif
	gstelement_class->change_state = GST_DEBUG_FUNCPTR(gst_ippaaclcenc_change_state);
	return;
}

static __inline void
init_members(GstippaacLCenc *aac)
{
	memset(&aac->dstbitStream, 0, sizeof(aac->dstbitStream));
	aac->leftPcmBuf = NULL;
	aac->leftPcmSize = 0;

	aac->Samplerate = 16000;
	aac->channels = 1;
	aac->bitrate = IPPGST_AACENC_DEFAULT_BPS;
	aac->tnsEn = 0;
	aac->bitstmFormat = 2;	//1:ADTS, 2:RAW

	aac->PCMOneFrameSize = sizeof(Ipp16s)*IPPGST_AACENC_FRAME_SIZE*aac->channels;

	aac->ts = GST_CLOCK_TIME_NONE;
	aac->dur = GST_CLOCK_TIME_NONE;
	aac->pCallBackTable = NULL;
	aac->pEncoderState = NULL;

	aac->ForceOutputEndBS = 0;

	aac->iEncodedBytesLen = 0;
	aac->totalFrames = 0;
	aac->codec_time = 0;

	return;
}


/* initialize the new element
 * instantiate pads and add them to element
 * set pad calback functions
 * initialize instance structure
 */

static void
gst_ippaaclcenc_init (GstippaacLCenc * aacenc,
    GstippaacLCencClass * aacenc_gclass)
{
	GstElementClass *klass = GST_ELEMENT_CLASS (aacenc_gclass);

	aacenc->sinkpad = gst_pad_new_from_template (
	  gst_element_class_get_pad_template (klass, "sink"), "sink");

	gst_pad_set_setcaps_function(aacenc->sinkpad,
	                            GST_DEBUG_FUNCPTR(gst_ippaaclcenc_sinkpad_setcaps));
	gst_pad_set_event_function(aacenc->sinkpad,
								GST_DEBUG_FUNCPTR(gst_ippaaclcenc_sinkpad_event));
	gst_pad_set_chain_function(aacenc->sinkpad,
	                          GST_DEBUG_FUNCPTR(gst_ippaaclcenc_chain));

	gst_element_add_pad(GST_ELEMENT (aacenc), aacenc->sinkpad);

	aacenc->srcpad = gst_pad_new_from_template (
		gst_element_class_get_pad_template (klass, "src"), "src");

	gst_element_add_pad (GST_ELEMENT (aacenc), aacenc->srcpad);

	init_members(aacenc);
	return;
}
/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
static gboolean
plugin_init (GstPlugin * plugin)
{
  /* debug category for fltering log messages
   *
   * exchange the string 'Template aaclcenc' with your description
   */
  GST_DEBUG_CATEGORY_INIT (ippaaclcenc_debug, "ippaaclcenc", 0, "ipp aaclc encoder element");
  return gst_element_register (plugin, "ippaaclcenc", GST_RANK_MARGINAL, GST_TYPE_IPPAACLCENC);

}

/* gstreamer looks for this structure to register aaclcencs
 *
 * exchange the string 'Template aaclcenc' with your aaclcenc description
 */
GST_PLUGIN_DEFINE (
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    "mvl_aaclcenc",
    "aacenc_LC_encoder based on IPP, build date "__DATE__,
    plugin_init,
    VERSION,
    "LGPL",
    "",
    ""
)
