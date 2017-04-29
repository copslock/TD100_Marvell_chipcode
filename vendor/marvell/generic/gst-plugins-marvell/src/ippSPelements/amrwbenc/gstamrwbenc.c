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
#include "gstamrwbenc.h"

#ifdef DEBUG_PERFORMANCE
#include  <sys/time.h>

        struct timeval tstart, tend;
        struct timezone tz;
#endif

#define LEN_AMRWB_DATA 320

#define IPP_AMRWB_BITSTREAM_LEN 61

GST_DEBUG_CATEGORY_STATIC (ippamrwbenc_debug);
#define GST_CAT_DEFAULT ippamrwbenc_debug


enum {

	/* fill above with user defined signals */
	LAST_SIGNAL
};

enum {
	ARG_0,
	/* fill below with user defined arguments */
	ARG_BIT_RATE,
	ARG_DTX,

#ifdef DEBUG_PERFORMANCE	
	ARG_TOTALFRAME,	
	ARG_CODECTIME,	
#endif

};


static GstStaticPadTemplate sink_template = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,   
    GST_STATIC_CAPS ("audio/x-raw-int, "
        "width = (int) 16, "
        "depth = (int) 16, "
        "signed = (boolean) TRUE, "
        "endianness = (int) BYTE_ORDER, "
        "rate = (int) 16000," 
        "channels = (int) 1")       
    );

static GstStaticPadTemplate src_template = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("audio/AMR-WB, " "rate = (int) 16000, " "channels = (int) 1")
    );


#define GST_TYPE_IPPAMRWBENC_BITRATE (gst_ippamrwbenc_bitrate_get_type())

static GType 
gst_ippamrwbenc_bitrate_get_type (void)
{
	static GType ippamrwbenc_bitrate_type = 0;

	if (!ippamrwbenc_bitrate_type) {
		static GEnumValue bitrate_types[] = {
			{ AMR_BITRATE_6600, "6.6kbps", "6.6kbps" },
			{ AMR_BITRATE_8850, "8.85kbps", "8.85kbps" },
			{ AMR_BITRATE_12650, "12.65kbps", "12.65kbps" },
			{ AMR_BITRATE_14250, "14.25kbps", "14.25kbps" },
			{ AMR_BITRATE_15850, "15.85kbps", "15.85kbps" },
			{ AMR_BITRATE_18250, "18.25kbps", "18.25kbps" },
			{ AMR_BITRATE_19850, "19.85kbps", "19.85kbps" },
			{ AMR_BITRATE_23050, "23.05kbps", "23.05kbps" },
			{ AMR_BITRATE_23850, "23.85kbps", "23.85kbps" },
			{ 0, NULL, NULL }
		};

		ippamrwbenc_bitrate_type = g_enum_register_static ("amrwb_bit_rate", bitrate_types);
	}

	return ippamrwbenc_bitrate_type;
}


GST_BOILERPLATE (Gstippamrwbenc, gst_ippamrwbenc, GstElement, GST_TYPE_ELEMENT);

static GstCaps 
*gst_ippamrwbenc_sinkpad_getcaps (GstPad *pad)
{
	Gstippamrwbenc *amr = GST_IPPAMRWBENC (GST_OBJECT_PARENT (pad));
	GstCaps *caps =  gst_caps_copy (gst_pad_get_pad_template_caps (amr->sinkpad));
		//gst_caps_copy (GST_PAD_CAPS (amr->sinkpad));	
	
	return caps;
}




static gboolean 
gst_ippamrwbenc_sinkpad_setcaps (GstPad *pad, GstCaps *caps)
{
	const GstStructure *str = NULL;
	const gchar *name;
	int i = 0;
	const GValue *value;

	Gstippamrwbenc *amr = GST_IPPAMRWBENC (GST_OBJECT_PARENT (pad));
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
  

	amr->duration = gst_util_uint64_scale_int (GST_SECOND, LEN_AMRWB_DATA, amr->rate * amr->channels);

	return TRUE;
}

static void 
gst_ippamrwbenc_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{	
	Gstippamrwbenc *amr = GST_IPPAMRWBENC (object);
	int tmp = 0;

	switch (prop_id)
	{	
	case ARG_BIT_RATE:
		amr->input_bitrate = g_value_get_enum (value);				

		switch (amr->input_bitrate)
		{
		case AMR_BITRATE_6600:
			amr->bitrate = IPP_SPCHBR_6600;
			break;
		case AMR_BITRATE_8850:
			amr->bitrate = IPP_SPCHBR_8850;
			break;
		case AMR_BITRATE_12650:
			amr->bitrate = IPP_SPCHBR_12650;
			break;
		case AMR_BITRATE_14250:
			amr->bitrate = IPP_SPCHBR_14250;
			break;
		case AMR_BITRATE_15850:
			amr->bitrate = IPP_SPCHBR_15850;
			break;
		case AMR_BITRATE_18250:
			amr->bitrate = IPP_SPCHBR_18250;
			break;
		case AMR_BITRATE_19850:
			amr->bitrate = IPP_SPCHBR_19850;
			break;
		case AMR_BITRATE_23050:
			amr->bitrate = IPP_SPCHBR_23050;
			break;
		case AMR_BITRATE_23850:
			amr->bitrate = IPP_SPCHBR_23850;
			break;
		default:
			amr->bitrate = IPP_SPCHBR_6600;
			break;
		}
		break;

	case ARG_DTX:
		amr->dtx = g_value_get_int (value);		
		break;

	
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void 
gst_ippamrwbenc_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	Gstippamrwbenc *amr = GST_IPPAMRWBENC (object);


	switch (prop_id)
	{	

	case ARG_BIT_RATE:
		g_value_set_enum (value, amr->input_bitrate);
		break;
		
	case ARG_DTX:
		g_value_set_int (value, amr->dtx);		
		break;

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


static GstFlowReturn  _ippamrwbenc_out_data(Gstippamrwbenc *amr)
{	
						
	GstBuffer *outBuf = NULL;	
	int ret = 0;
	//unsigned char head = 0;
	
	//head = (unsigned char)(amr->config.frameType << 3);
	//head |= 4;

	//ret = gst_pad_alloc_buffer_and_set_caps (amr->srcpad, 0, amr->bitstream.bsByteLen + 1, GST_PAD_CAPS (amr->srcpad), &outBuf);
	ret = gst_pad_alloc_buffer_and_set_caps (amr->srcpad, 0, amr->bitstream.bsByteLen , GST_PAD_CAPS (amr->srcpad), &outBuf);

	if(ret != GST_FLOW_OK)
	{
		GST_ERROR_OBJECT (amr, "amr can not malloc buffer");
		
		return ret;								
	}	

	//*(GST_BUFFER_DATA (outBuf)) = head;
	//memcpy(GST_BUFFER_DATA (outBuf)+1, amr->bitstream.pBsBuffer, amr->bitstream.bsByteLen);
	memcpy(GST_BUFFER_DATA (outBuf), amr->bitstream.pBsBuffer, amr->bitstream.bsByteLen);

	GST_BUFFER_DURATION (outBuf) = amr->duration;
	GST_BUFFER_TIMESTAMP (outBuf) = amr->ts;	
	amr->ts += amr->duration;	
				
	if(GST_FLOW_OK != (ret = gst_pad_push (amr->srcpad, outBuf))){		
		
		GST_ERROR_OBJECT(amr, "ippamrwbenc push error");
		return ret; 
	}		

	return GST_FLOW_OK;
	
}

static GstFlowReturn 
gst_ippamrwbenc_chain (GstPad *pad, GstBuffer *inbuf)
{
	Gstippamrwbenc *amr = GST_IPPAMRWBENC (GST_OBJECT_PARENT (pad));
	int ret = 0;

	int left = 0;
	int offset = 0;
	guint8 *data = NULL;	

	if (GST_BUFFER_TIMESTAMP_IS_VALID (inbuf))
    		amr->ts = GST_BUFFER_TIMESTAMP (inbuf);

	 /* buffer + remaining data */
	if (amr->tempbuf) {		
		inbuf = gst_buffer_join (amr->tempbuf, inbuf);
		amr->tempbuf = NULL;
	}

	data = GST_BUFFER_DATA (inbuf);
	left = GST_BUFFER_SIZE (inbuf);	

	while(left >= amr->need_len){

		amr->speech_in.pSndFrame = (Ipp16s *)(data + offset);			

#ifdef DEBUG_PERFORMANCE
        gettimeofday (&tstart, &tz);
#endif
		if(IPP_STATUS_NOERR != (ret = Encode_AMRWB(&(amr->speech_in), &(amr->bitstream), &(amr->config), amr->pEncoderState))){
			GST_ERROR_OBJECT(amr, "ippamrwbenc encoder error");

			ret = GST_FLOW_ERROR; 
			goto err;
		}		
#ifdef DEBUG_PERFORMANCE
        gettimeofday (&tend, &tz);
        amr->codec_time += (tend.tv_sec - tstart.tv_sec)*1000000 + (tend.tv_usec - tstart.tv_usec);
#endif

		left -= amr->need_len;
		offset += amr->need_len;		

		if(GST_FLOW_OK != (ret = _ippamrwbenc_out_data(amr))){							
			goto err;
		}	

		amr->totalframe ++;
		
	}

	if (left > 0) {					
		if (left < (int)GST_BUFFER_SIZE (inbuf)) {
			amr->tempbuf = gst_buffer_create_sub (inbuf, offset, left);
		} else {
			amr->tempbuf = inbuf;
			gst_buffer_ref (inbuf);
		}
	}

	gst_buffer_unref(inbuf);
	
	return GST_FLOW_OK;

err:
	gst_buffer_unref(inbuf);
	
	return ret;

}

static gboolean
gst_ippamrwbenc_start (Gstippamrwbenc * amr)
{
	GstCaps *Tmp;
	int ret = 0;

	amr->tempbuf = NULL;
	amr->need_len = sizeof(Ipp16s) * LEN_AMRWB_DATA;

	amr->bitstream.pBsBuffer = NULL;
	amr->bitstream.pBsBuffer = (Ipp8u *)malloc(IPP_AMRWB_BITSTREAM_LEN);
	if(NULL == amr->bitstream.pBsBuffer){
		GST_ERROR_OBJECT (amr, "can not malloc memory");
		return FALSE;
	}
	
	memset(amr->bitstream.pBsBuffer, 0, IPP_AMRWB_BITSTREAM_LEN);		

	/*config para*/
	amr->config.bitstreamFormatId = AMRWB_MIME_FILE_FORMAT;	/* now we only support AMRWB_MIME_FILE_FORMAT format*/

	amr->config.bitRate = amr->bitrate;
	amr->totalframe = 0;
	amr->nFrameLen = 0;

	amr->channels = 1;
	amr->rate = 16000;
	
	amr->codec_time = 0;

	amr->ts = 0;	
	amr->duration = gst_util_uint64_scale_int (GST_SECOND, LEN_AMRWB_DATA, amr->rate * amr->channels);
	//GST_SECOND * 0.02;		/*amr is 20 ms, specified in spec*/

	
	
	
	amr->pCallbackTable = NULL;
	amr->pEncoderState = NULL;
	
	miscInitGeneralCallbackTable(&(amr->pCallbackTable));	
	EncoderInitAlloc_AMRWB(&(amr->pEncoderState), amr->dtx, amr->pCallbackTable);
	
	Tmp = gst_caps_new_simple ("audio/AMR-WB",
	      "channels", G_TYPE_INT, amr->channels,
	      "rate", G_TYPE_INT, amr->rate, NULL);
	gst_pad_set_caps(amr->srcpad, Tmp);
	gst_caps_unref(Tmp);

	return TRUE;

	

}

static gboolean
gst_ippamrwbenc_stop (Gstippamrwbenc * amr)
{
	EncoderFree_AMRWB(&(amr->pEncoderState), amr->pCallbackTable);
	miscFreeGeneralCallbackTable(&(amr->pCallbackTable));

	amr->pCallbackTable = NULL;
	amr->pEncoderState = NULL;

	if(NULL != amr->bitstream.pBsBuffer){
		free(amr->bitstream.pBsBuffer);
		amr->bitstream.pBsBuffer = NULL;
	}

	if (amr->tempbuf != NULL) {
		gst_buffer_unref (amr->tempbuf);
		amr->tempbuf = NULL;
	}

	GST_INFO_OBJECT(amr, "amr totalframe=%d\n", amr->totalframe);	
	

	return TRUE;

}

static GstStateChangeReturn 
gst_ippamrwbenc_change_state (GstElement *element, GstStateChange transition)
{
	GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;
	Gstippamrwbenc *amr = GST_IPPAMRWBENC (element);

	switch (transition)
	{
	case GST_STATE_CHANGE_NULL_TO_READY:
		if(!gst_ippamrwbenc_start(amr)){
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
		if(!gst_ippamrwbenc_stop(amr)){
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
gst_ippamrwbenc_base_init (gpointer klass)
{
	static GstElementDetails ippamrwbenc_details = {
		"amr wide band encoder", 
		"Codec/Encoder/Speech", 
		"AMR Wide Band Encoder based-on IPP & CODEC", 
	        ""
	};

	GstElementClass *element_class = GST_ELEMENT_CLASS (klass);

	gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&src_template));
	gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&sink_template));

	gst_element_class_set_details (element_class, &ippamrwbenc_details);

}

static void 
gst_ippamrwbenc_class_init (GstippamrwbencClass *klass)
{
	GObjectClass *gobject_class  = (GObjectClass*) klass;
	GstElementClass *gstelement_class = (GstElementClass*) klass;

	parent_class = g_type_class_peek_parent(klass);

	gobject_class->set_property = gst_ippamrwbenc_set_property;
	gobject_class->get_property = gst_ippamrwbenc_get_property;

	g_object_class_install_property (gobject_class, ARG_DTX, 
		g_param_spec_int ("dtx", "DTX switch", 
		"DTX switch, 0 is off, 1 is open", 
		0/* range_MIN */, 1/* range_MAX */, 1/* default_INIT */, G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, ARG_BIT_RATE, 
		g_param_spec_enum ("bit_rate", "The bit rate of ippamrwbenc", 
		"The bit rate of ippamrwbenc which will be encoded", 
		GST_TYPE_IPPAMRWBENC_BITRATE, AMR_BITRATE_6600/* default_INIT */, G_PARAM_READWRITE));

#ifdef DEBUG_PERFORMANCE	
	g_object_class_install_property (gobject_class, ARG_TOTALFRAME,
  		g_param_spec_int ("totalframes", "Totalframe of amrwb stream",
          	"Number of frame", 0, G_MAXINT, 0, G_PARAM_READABLE));
	g_object_class_install_property (gobject_class, ARG_CODECTIME,
  		g_param_spec_int64 ("codectime", "amrwb encode time",
          	"codec time (microsecond)", 0, G_MAXINT64, 0, G_PARAM_READABLE));
#endif


	
	gstelement_class->change_state = GST_DEBUG_FUNCPTR (gst_ippamrwbenc_change_state);

}


static void 
gst_ippamrwbenc_init (Gstippamrwbenc *amr, GstippamrwbencClass *klass)
{
	amr->sinkpad = gst_pad_new_from_template (
		gst_element_class_get_pad_template ((GstElementClass*)klass, "sink"), "sink");


	gst_pad_set_setcaps_function (amr->sinkpad, GST_DEBUG_FUNCPTR (gst_ippamrwbenc_sinkpad_setcaps));
	gst_pad_set_getcaps_function (amr->sinkpad, GST_DEBUG_FUNCPTR (gst_ippamrwbenc_sinkpad_getcaps));

	gst_pad_set_chain_function (amr->sinkpad, GST_DEBUG_FUNCPTR (gst_ippamrwbenc_chain));

	amr->srcpad = gst_pad_new_from_template (
		gst_element_class_get_pad_template ((GstElementClass*)klass, "src"), "src");

	gst_element_add_pad (GST_ELEMENT (amr), amr->sinkpad);
	gst_element_add_pad (GST_ELEMENT (amr), amr->srcpad);
	
	amr->bitrate = IPP_SPCHBR_6600;
	amr->dtx = 1;
	
}

static gboolean 
plugin_init (GstPlugin *plugin)
{
	return gst_element_register (plugin, "ippamrwbenc", GST_RANK_PRIMARY, GST_TYPE_IPPAMRWBENC);
}


GST_PLUGIN_DEFINE (GST_VERSION_MAJOR, 
                   GST_VERSION_MINOR, 
                   "mvl_ippamrwbenc", 
                   "AMR wide band encoder based on IPP, build date "__DATE__, 
                   plugin_init, 
                   VERSION, 
                   "Proprietary", 
                   "", 
                   "");


/* EOF */
