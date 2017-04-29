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

#include <string.h>	//to include memcpy, memset
#include "gstamrnbdec.h"

#ifdef DEBUG_PERFORMANCE
#include  <sys/time.h>

		struct timeval tstart, tend;
		struct timezone tz;
#endif

#define LEN_AMRNB_DATA 160


GST_DEBUG_CATEGORY_STATIC (ippamrnbdec_debug);
#define GST_CAT_DEFAULT ippamrnbdec_debug


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


static GstStaticPadTemplate sink_factory =
		GST_STATIC_PAD_TEMPLATE ("sink",
		GST_PAD_SINK,
		GST_PAD_ALWAYS,
		GST_STATIC_CAPS ("audio/AMR, " "rate = (int) 8000, " "channels = (int) 1" )
		);

static GstStaticPadTemplate src_factory =
	GST_STATIC_PAD_TEMPLATE ("src", GST_PAD_SRC, GST_PAD_ALWAYS,
	GST_STATIC_CAPS ("audio/x-raw-int, "
					 "endianness = (int) "G_STRINGIFY (G_BYTE_ORDER)", "
					 "signed = (boolean) TRUE, " \
					 "width = (int) 16, "
					 "depth = (int) 16, "
					 "rate = (int) 8000, "
					 "channels = (int) 1"));




GST_BOILERPLATE (Gstippamrnbdec, gst_ippamrnbdec, GstElement, GST_TYPE_ELEMENT);


#if 0
/*********************************************************************************************
// Name:			 GetFrameLength
//
// Description: 	 Get the length of a frame in type defined, counted in byte
//
**********************************************************************************************/
static int GetFrameLength(Gstippamrnbdec *amr, int nFrameType)
{
	int nFrameLen;
	switch(nFrameType)
	{
		case IPP_SPCHBR_4750:
			amr->rate = 4750;
			nFrameLen = IPP_GSMAMR_BITSTREAM_LEN_475;
			break;
		case IPP_SPCHBR_5150:
			amr->rate = 5150;
			nFrameLen = IPP_GSMAMR_BITSTREAM_LEN_515;
			break;
		case IPP_SPCHBR_5900:
			amr->rate = 5900;
			nFrameLen = IPP_GSMAMR_BITSTREAM_LEN_59;
			break;
		case IPP_SPCHBR_6700:
			amr->rate = 6700;
			nFrameLen = IPP_GSMAMR_BITSTREAM_LEN_67;
			break;
		case IPP_SPCHBR_7400:
			amr->rate = 7400;
			nFrameLen = IPP_GSMAMR_BITSTREAM_LEN_74;
			break;
		case IPP_SPCHBR_7950:
			amr->rate = 7950;
			nFrameLen = IPP_GSMAMR_BITSTREAM_LEN_795;
			break;
		case IPP_SPCHBR_10200:
			amr->rate = 10200;
			nFrameLen = IPP_GSMAMR_BITSTREAM_LEN_102;
			break;
		case IPP_SPCHBR_12200:
			amr->rate = 12200;
			nFrameLen = IPP_GSMAMR_BITSTREAM_LEN_122;
			break;
		case IPP_SPCHBR_DTX:
			nFrameLen = IPP_GSMAMR_BITSTREAM_LEN_DTX;
			break;
		case 15:		/* 15 is the no data type */
			nFrameLen = 1;
			break;
		default:
			fprintf(stderr,"Invalid bitrate in GetFrameLength, abort\n");
			exit(13);
			break;
	}
	return nFrameLen;
}
#endif

static const gint block_size[16] = { 12, 13, 15, 17, 19, 20, 26, 31, 5,
  -1, -1, -1, -1, -1, -1, 0
};

static gboolean
gst_ippamrnbdec_sinkpad_setcaps (GstPad *pad, GstCaps *caps)
{
	GstStructure *str = NULL;
	const gchar *name;
	int i = 0;
	GstCaps *Tmp;
	//const GValue *value;

	Gstippamrnbdec *amr = GST_IPPAMRNBDEC (GST_OBJECT_PARENT (pad));
	int stru_num = gst_caps_get_size (caps);

	for (i = 0; i < stru_num; i++) {
		str = gst_caps_get_structure (caps, i);
		name = gst_structure_get_name (str);
		GST_INFO_OBJECT (amr, "name: %s\n", name);

		gchar *sstr;

		sstr = gst_structure_to_string (str);
		GST_INFO_OBJECT (amr, "%s\n", sstr);
		g_free (sstr);
	}

	/* get channel count */
	gst_structure_get_int (str, "channels", &amr->channels);
	gst_structure_get_int (str, "rate", &amr->rate);

	amr->duration = gst_util_uint64_scale_int (GST_SECOND, LEN_AMRNB_DATA, amr->rate * amr->channels);

	Tmp = gst_caps_new_simple ("audio/x-raw-int",
		"endianness", G_TYPE_INT, G_BYTE_ORDER,
		"signed", G_TYPE_BOOLEAN, TRUE,
		"width", G_TYPE_INT, 16,
		"depth", G_TYPE_INT, 16,
		"rate", G_TYPE_INT, 8000,   				/*amrnb is 8000, specified in spec*/
		"channels", G_TYPE_INT, amr->channels,
		NULL);
	gst_pad_set_caps(amr->srcpad, Tmp);
	gst_caps_unref(Tmp);

	return TRUE;
}

static void
gst_ippamrnbdec_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	//Gstippamrnbdec *amr = GST_IPPAMRNBDEC (object);

	switch (prop_id)
	{
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
gst_ippamrnbdec_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
#ifdef DEBUG_PERFORMANCE
	Gstippamrnbdec *amr = GST_IPPAMRNBDEC (object);
#endif
	switch (prop_id)
	{

#ifdef DEBUG_PERFORMANCE
	case ARG_TOTALFRAME:
		g_value_set_int (value, amr->totalframe);
		break;
	case ARG_CODECTIME:
		g_value_set_int64 (value, amr->codec_time);
		break;
#endif

	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}



static GstFlowReturn  _ippamrnbdec_out_data(Gstippamrnbdec *amr)
{

	GstBuffer *outBuf = NULL;
	int ret = 0;

	ret = gst_pad_alloc_buffer_and_set_caps (amr->srcpad, 0,
		LEN_AMRNB_DATA*sizeof(gint16), GST_PAD_CAPS (amr->srcpad), &outBuf);

	if(ret != GST_FLOW_OK)
	{
		GST_ERROR_OBJECT (amr, "amr can not malloc buffer");

		return ret;
	}
	memcpy(GST_BUFFER_DATA (outBuf), amr->speech_out.pSndFrame, sizeof(gint16) * LEN_AMRNB_DATA);


	GST_BUFFER_DURATION (outBuf) = amr->duration;
	GST_BUFFER_TIMESTAMP (outBuf) = amr->ts;
	amr->ts += amr->duration;

	ret = gst_pad_push (amr->srcpad, outBuf);
	if(GST_FLOW_OK != ret){

		//gst_buffer_unref (outBuf); /* need check whether shuld unref here */
		return ret;
	}

	return GST_FLOW_OK;

}

static GstFlowReturn
gst_ippamrnbdec_chain (GstPad *pad, GstBuffer *inbuf)
{
	Gstippamrnbdec *amr = GST_IPPAMRNBDEC (GST_OBJECT_PARENT (pad));
	guint8 uFirstByte = 0;
	int ret = 0;

	int left = 0;
	int offset = 0;
	guint8 *data = NULL;

	IppCodecStatus rtCode = IPP_STATUS_NOERR;


	if (GST_BUFFER_TIMESTAMP_IS_VALID (inbuf)){
			amr->ts = GST_BUFFER_TIMESTAMP (inbuf);
		}

	left = GST_BUFFER_SIZE (inbuf);
	data = GST_BUFFER_DATA (inbuf);

	while(left > 0){

		uFirstByte = *(data + offset);

		amr->config.frameType = (uFirstByte >> 3) & 0xf;

		amr->nFrameLen = block_size[amr->config.frameType];

		if(amr->nFrameLen < 0){
			ret = GST_FLOW_ERROR;
			goto err;
		}

		/*remove head*/
		offset += 1;
		left -= 1;

		amr->bitstream.pBsBuffer = data + offset;
		amr->bitstream.pBsCurByte = amr->bitstream.pBsBuffer;
		amr->bitstream.bsCurBitOffset = 0;
		amr->bitstream.bsByteLen = left;

#ifdef DEBUG_PERFORMANCE
		gettimeofday (&tstart, &tz);
#endif
				rtCode = Decode_GSMAMR(&(amr->bitstream), &(amr->speech_out), amr->pDecoderState, &(amr->config));
		if(IPP_STATUS_NOERR != rtCode){
			GST_ERROR_OBJECT(amr, "amrndec decoder error");
			switch(rtCode){
				case IPP_STATUS_BADARG_ERR:
					GST_ERROR_OBJECT(amr, "bad argument error");
					break;
				case IPP_STATUS_BITSTREAM_ERR:
				default:
					GST_ERROR_OBJECT(amr, "bitstream error");
					break;
				}
			ret = GST_FLOW_ERROR;
			goto err;
		}
#ifdef DEBUG_PERFORMANCE
		gettimeofday (&tend, &tz);
		amr->codec_time += (tend.tv_sec - tstart.tv_sec)*1000000 + (tend.tv_usec - tstart.tv_usec);
#endif

		left -= amr->nFrameLen;
		offset += amr->nFrameLen;

		if(GST_FLOW_OK != (ret = _ippamrnbdec_out_data(amr))){
			goto err;
		}

		amr->totalframe ++;
	}

	gst_buffer_unref(inbuf);

	return GST_FLOW_OK;

err:
	gst_buffer_unref(inbuf);
	if(ret < GST_FLOW_UNEXPECTED) {
		GST_ELEMENT_ERROR(amr, STREAM, DECODE, (NULL), ("%s() return un-accepted error %d", __FUNCTION__, ret));
	}

	return ret;



}

static gboolean
gst_ippamrnbdec_start (Gstippamrnbdec * amr)
{

	amr->speech_out.pSndFrame = NULL;
	amr->speech_out.pSndFrame = (Ipp16s *)malloc(LEN_AMRNB_DATA*sizeof(gint16));
	if(NULL == amr->speech_out.pSndFrame){
		GST_ERROR_OBJECT (amr, "can not malloc memory");
		return FALSE;
	}

	memset(amr->speech_out.pSndFrame, 0, LEN_AMRNB_DATA*sizeof(gint16));


	/*config para*/
	amr->config.formatInd = HEADERLESS_IF1_FORMAT;  /* now we only support HEADERLESS_IF1_FORMAT format*/


	amr->totalframe = 0;
	amr->nFrameLen = 0;

	amr->channels = 1;
	amr->rate = 8000;

	amr->ts = 0;
	//amr->duration = gst_util_uint64_scale_int (GST_SECOND, LEN_AMRNB_DATA, amr->rate * amr->channels);
	//GST_SECOND * 0.02;		/*amr is 20 ms, specified in spec*/



	amr->pCallbackTable = NULL;
	amr->pDecoderState = NULL;

	miscInitGeneralCallbackTable(&(amr->pCallbackTable));
	DecoderInitAlloc_GSMAMR(&(amr->pDecoderState), amr->pCallbackTable);

	amr->codec_time = 0;

	return TRUE;



}

static gboolean
gst_ippamrnbdec_stop (Gstippamrnbdec * amr)
{

	DecoderFree_GSMAMR(&(amr->pDecoderState), amr->pCallbackTable);
	miscFreeGeneralCallbackTable(&(amr->pCallbackTable));
	amr->pCallbackTable = NULL;
	amr->pDecoderState = NULL;

	if(NULL != amr->speech_out.pSndFrame){
		free(amr->speech_out.pSndFrame);
		amr->speech_out.pSndFrame = NULL;
	}

	GST_INFO_OBJECT(amr, "amr totalframe=%d\n", amr->totalframe);

	//amr->totalframe = 0;
	amr->ts = 0;


	return TRUE;

}

static GstStateChangeReturn
gst_ippamrnbdec_change_state (GstElement *element, GstStateChange transition)
{
	GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;
	Gstippamrnbdec *amr = GST_IPPAMRNBDEC (element);

	switch (transition)
	{
	case GST_STATE_CHANGE_NULL_TO_READY:
		if(!gst_ippamrnbdec_start(amr)){
			goto statechange_failed;
		}
		break;
	default:
		break;
	}

	ret = GST_ELEMENT_CLASS (parent_class)->change_state (element, transition);
	if (ret == GST_STATE_CHANGE_FAILURE)
		return ret;

	switch (transition)
	{
	case GST_STATE_CHANGE_READY_TO_NULL:
		if(!gst_ippamrnbdec_stop(amr)){
			goto statechange_failed;
		}
		break;
	default:
		break;
	}

	return ret;

	 /* ERRORS */
statechange_failed:
	{
		/* subclass must post a meaningfull error message */
		GST_ERROR_OBJECT (amr, "state change failed");
		return GST_STATE_CHANGE_FAILURE;
	}
}

static void
gst_ippamrnbdec_base_init (gpointer klass)
{
	static GstElementDetails ippamrnbdec_details = {
		"amr narrow band decoder",
		"Codec/Decoder/Speech",
		"AMR Narrow Band Decoder based-on IPP & CODEC",
			""
	};

	GstElementClass *element_class = GST_ELEMENT_CLASS (klass);

	gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&src_factory));
	gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&sink_factory));

	gst_element_class_set_details (element_class, &ippamrnbdec_details);
	return;
}

static void
gst_ippamrnbdec_class_init(GstippamrnbdecClass *klass)
{
	GObjectClass *gobject_class  = (GObjectClass*) klass;
	GstElementClass *gstelement_class = (GstElementClass*) klass;

	parent_class = g_type_class_peek_parent(klass);

	gobject_class->set_property = gst_ippamrnbdec_set_property;
	gobject_class->get_property = gst_ippamrnbdec_get_property;

#ifdef DEBUG_PERFORMANCE
	g_object_class_install_property (gobject_class, ARG_TOTALFRAME,
		g_param_spec_int ("totalframes", "Totalframe of amrnb stream",
			"Number of frame", 0, G_MAXINT, 0, G_PARAM_READABLE));
	g_object_class_install_property (gobject_class, ARG_CODECTIME,
		g_param_spec_int64 ("codectime", "amrnb decode time",
			"codec time (microsecond)", 0, G_MAXINT64, 0, G_PARAM_READABLE));
#endif

	gstelement_class->change_state = GST_DEBUG_FUNCPTR (gst_ippamrnbdec_change_state);

	GST_DEBUG_CATEGORY_INIT (ippamrnbdec_debug, "ippamrnbdec", 0, "IPP AMR NB Decode Element");
	return;
}


static void
gst_ippamrnbdec_init (Gstippamrnbdec *amr, GstippamrnbdecClass *klass)
{
	amr->sinkpad = gst_pad_new_from_template (
		gst_element_class_get_pad_template ((GstElementClass*)klass, "sink"), "sink");


	gst_pad_set_setcaps_function (amr->sinkpad, GST_DEBUG_FUNCPTR (gst_ippamrnbdec_sinkpad_setcaps));
	//gst_pad_set_event_function (amr->sinkpad, GST_DEBUG_FUNCPTR (gst_ippamrnbdec_sinkpad_event));
	gst_pad_set_chain_function (amr->sinkpad, GST_DEBUG_FUNCPTR (gst_ippamrnbdec_chain));

	amr->srcpad = gst_pad_new_from_template (
		gst_element_class_get_pad_template ((GstElementClass*)klass, "src"), "src");

	gst_element_add_pad (GST_ELEMENT (amr), amr->sinkpad);
	gst_element_add_pad (GST_ELEMENT (amr), amr->srcpad);

	return;
}

static gboolean
plugin_init (GstPlugin *plugin)
{
	return gst_element_register (plugin, "ippamrnbdec", GST_RANK_PRIMARY+2, GST_TYPE_IPPAMRNBDEC);
}


GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
				   GST_VERSION_MINOR,
				   "mvl_amrnbdec",
				   "AMR narrow band decoder based on IPP, build date "__DATE__,
				   plugin_init,
				   VERSION,
				   "LGPL",
				   "",
				   "");


/* EOF */
