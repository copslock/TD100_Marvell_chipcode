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

/* gstwmadec.c */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gst/riff/riff-ids.h"
#include "gstwmadec.h"
#ifdef DEBUG_PERFORMANCE
#include  <sys/time.h>

        struct timeval tstart, tend;
        struct timezone tz;
#endif

GST_DEBUG_CATEGORY_STATIC (wmadec_debug);
#define GST_CAT_DEFAULT wmadec_debug


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
	GST_STATIC_PAD_TEMPLATE ("sink", GST_PAD_SINK, GST_PAD_ALWAYS, 
		GST_STATIC_CAPS("audio/x-wma,"
			"wmaversion = (int) [1, 3] "					
			)
		);

static GstStaticPadTemplate src_factory = 
	GST_STATIC_PAD_TEMPLATE ("src", GST_PAD_SRC, GST_PAD_ALWAYS, 
	GST_STATIC_CAPS ("audio/x-raw-int, " 
			"endianness = (int) "G_STRINGIFY (G_BYTE_ORDER)", " 
			"signed = (boolean) TRUE, "
			"width = (int) 16, " 
			"depth = (int) 16, " 
			"rate = (int) [ 8000, 96000 ], " 
			"channels = (int) [ 1, 2 ]"));

GST_BOILERPLATE (Gstwmadec, gst_wmadec, GstElement, GST_TYPE_ELEMENT);


static gboolean 
gst_wmadec_sinkpad_setcaps (GstPad *pad, GstCaps *caps)
{
	Gstwmadec *wma = GST_WMADEC (GST_OBJECT_PARENT (pad));
	
	int i = 0;
	gchar *name;
	GstStructure *str;
	const GValue *value;
	int stru_num = gst_caps_get_size (caps);

	if(wma->init_error){
		return FALSE;  
	}
	
	for (i = 0; i < stru_num; i++) {
		str = gst_caps_get_structure (caps, i);
		name = gst_structure_get_name (str);
		GST_INFO_OBJECT (wma, "name: %s\n", name);

		gchar *sstr;

		sstr = gst_structure_to_string (str);
		GST_INFO_OBJECT (wma, "%s\n", sstr);
		g_free (sstr);		
	}	

	wma->decoderConfig.dwPacketSize = 0;
	wma->decoderConfig.iDwnmixLevel = IPP_WMA_MS_DOWNMIX;
	wma->decoderConfig.iDecoderFlags = 0;

	if ((value = gst_structure_get_value (str, "codec_data"))) {
		guint8 *cdata;
		guint csize;
		GstBuffer *buf;	

		buf = gst_value_get_buffer (value);

		cdata = GST_BUFFER_DATA (buf);
		csize = GST_BUFFER_SIZE (buf);		

		wma->decoderConfig.cbSize = csize;	
		memcpy(wma->decoderConfig.pData, cdata, csize);
	}

 	int codec_id = 0;
	int wmaversion = 0;
	if (gst_structure_get_int(str, "wmaversion", &(wmaversion) )){
		codec_id = GST_RIFF_WAVE_FORMAT_WMAV1 + wmaversion - 1;
		wma->decoderConfig.wFormatTag = codec_id;
	}

	int bitrate = 0;
	int byterate = 0;
	if (gst_structure_get_int(str, "bitrate", &(bitrate) )){
		byterate = bitrate/8;
		wma->decoderConfig.nAvgBytesPerSec = byterate;
	}

	int block_align = 0;
	if (gst_structure_get_int(str, "block_align", &(block_align) )){
		wma->decoderConfig.nBlockAlign = block_align;
	}

	int channels = 0;
	if (gst_structure_get_int(str, "channels", &(channels) )){
		wma->decoderConfig.nChannels = channels;		
		if(channels > 2){
			/* fold-down to 2 channels */
			wma->decoderConfig.iDecoderFlags  |= IPP_DECOPT_CHANNEL_DOWNMIXING;			
			channels = 2;	
		}
	}

	int depth = 0;
	if (gst_structure_get_int(str, "depth", &(depth) )){
		wma->decoderConfig.wBitsPerSample = depth;		
		if(depth > 16){
			/* resample to 16 bits, -pcm 16 */
			wma->decoderConfig.iDecoderFlags |= IPP_DECOPT_REQUANTTO16;			
			depth = 16;
		}
	}

	int rate = 0;
	if (gst_structure_get_int(str, "rate", &(rate) )){		
		wma->decoderConfig.nSamplesPerSec = rate;

		if(rate > 48000){
			/* downsample */
			wma->decoderConfig.iDecoderFlags |= IPP_DECOPT_DOWNSAMPLETO44OR48;		
			if( 96000 == rate){
				rate = 48000;
			}else if(88200 == rate){
				rate = 44100;
			}else{
				GST_ERROR_OBJECT (wma, "sample do not support");
				return FALSE;
			}			
		}
	}

	if(!wma->pDecoderState){
		GstCaps *Tmp;
		int ret = 0;
		
		ret = DecoderInitAlloc_WMA(&(wma->srcBitStream), &(wma->decoderConfig),wma->pCBTable, 
			&(wma->max_framesize), &(wma->pDecoderState));

		if (IPP_STATUS_INIT_OK != ret ){
			wma->init_error = TRUE;			
			GST_ERROR_OBJECT (wma, "init wmadecoder error");				
	        	return FALSE;
		}		

		/* set srcpad caps */
		Tmp = gst_caps_new_simple ("audio/x-raw-int", 
			"endianness", G_TYPE_INT, G_BYTE_ORDER, 
			"signed", G_TYPE_BOOLEAN, TRUE, 
			"width", G_TYPE_INT, depth, 
			"depth", G_TYPE_INT, depth, 
			"rate", G_TYPE_INT, rate, 
			"channels", G_TYPE_INT, channels, 					
			NULL);	
		gst_pad_set_caps(wma->srcpad, Tmp);
		gst_caps_unref(Tmp);

		if(!wma->poutbuf){
			wma->poutbuf = malloc(wma->max_framesize + 1);
			if((int)(wma->poutbuf) & 0x1){
				wma->sound.pSndFrame = (short *)((Ipp8u *)(wma->poutbuf)+1);
			}else{
				wma->sound.pSndFrame = (short *)((Ipp8u *)(wma->poutbuf));
			}
			wma->sound.sndLen =0;
		}		
		
	}
	
	return TRUE;
}
static void 
gst_wmadec_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{		
	switch (prop_id)
	{	
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void 
gst_wmadec_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
#ifdef DEBUG_PERFORMANCE
	Gstwmadec *wma = GST_WMADEC (object);
#endif
	switch (prop_id)
	{	

#ifdef DEBUG_PERFORMANCE
	case ARG_TOTALFRAME:
		g_value_set_int (value, wma->totalFrames);		
		break;
	case ARG_CODECTIME:
		g_value_set_int64 (value, wma->codec_time);		
		break;
#endif

	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}
static GstFlowReturn _wma_push_data(Gstwmadec *wma)
{
	int ret = 0;

	GstBuffer *outBuf = NULL;

	ret = gst_pad_alloc_buffer_and_set_caps (wma->srcpad, 0, 
		wma->sound.sndLen, GST_PAD_CAPS (wma->srcpad), &outBuf);
	
	if(ret != GST_FLOW_OK){
		GST_ERROR_OBJECT (wma, "wma can not malloc buffer");		
		return ret;								
	}
	
	memcpy (GST_BUFFER_DATA (outBuf), wma->sound.pSndFrame, wma->sound.sndLen);

	if(0 != wma->sound.sndSampleRate){
		
		GST_BUFFER_TIMESTAMP (outBuf) = wma->nextTS;
		GST_BUFFER_DURATION (outBuf) = gst_util_uint64_scale((wma->sound.sndLen >> 1), 
				GST_SECOND, wma->sound.sndChannelNum * wma->sound.sndSampleRate);
		wma->nextTS += GST_BUFFER_DURATION (outBuf);
	
	}
	if(outBuf->size != 0){
		ret = gst_pad_push (wma->srcpad, outBuf);
		if (GST_FLOW_OK != ret) {
			GST_ERROR_OBJECT (wma, "wma: The decoded frame did not successfully push out to downstream element");
			return ret;
		}
		
	} else {
		GST_ERROR_OBJECT (wma, "wma: The output buffer is empty!");
	}
	wma->totalFrames ++;
	
	return GST_FLOW_OK;
	
}

static GstFlowReturn 
gst_wmadec_chain (GstPad *pad, GstBuffer *buf)
{
	Gstwmadec *wma = GST_WMADEC (GST_OBJECT_PARENT (pad));
	int ret = 0;
	int status = 0;

	int left = 0;
	int offset = 0;

	if (GST_BUFFER_TIMESTAMP (buf) != GST_CLOCK_TIME_NONE) {	
		wma->nextTS = GST_BUFFER_TIMESTAMP (buf);
 	}

	 /* buffer + remaining data */
	wma->srcBitStream.pBsBuffer = GST_BUFFER_DATA (buf);
	wma->srcBitStream.pBsCurByte = wma->srcBitStream.pBsBuffer;
	wma->srcBitStream.bsByteLen = GST_BUFFER_SIZE (buf); //wma->decoderConfig.nBlockAlign;//
	wma->srcBitStream.bsCurBitOffset = 0;

	do{

#ifdef DEBUG_PERFORMANCE
                        gettimeofday (&tstart, &tz);
#endif
		status = Decode_WMA(&(wma->srcBitStream), &(wma->sound), wma->pDecoderState);		
#ifdef DEBUG_PERFORMANCE
                        gettimeofday (&tend, &tz);
                        wma->codec_time += (tend.tv_sec - tstart.tv_sec)*1000000 + (tend.tv_usec - tstart.tv_usec);
#endif

		switch (status)
		{
			case IPP_STATUS_FRAME_COMPLETE:		
				ret = _wma_push_data(wma);
				if (GST_FLOW_OK != ret) {					
					gst_buffer_unref (buf);	
					return ret;
				}

				offset = wma->srcBitStream.pBsCurByte - wma->srcBitStream.pBsBuffer;
				left = GST_BUFFER_SIZE (buf) - offset;
				
				break;

			case IPP_STATUS_BUFFER_UNDERRUN:	

				/* Keep the leftovers in raw stream */
				break;

			case IPP_STATUS_FRAME_ERR:
				
				GST_ERROR_OBJECT (wma, "IPP_STATUS_FRAME_ERR");
			
				break;

			case IPP_STATUS_NOTSUPPORTED:
				
				GST_ERROR_OBJECT (wma, "IPP_STATUS_NOTSUPPORTED");
			
				break;

			case IPP_STATUS_BS_END:
				
				GST_ERROR_OBJECT (wma, "IPP_STATUS_BS_END");
			
				break;
				

			default:				
				goto err;	
		}	
	}while(IPP_STATUS_FRAME_COMPLETE == status);

	
	gst_buffer_unref (buf);	
	return GST_FLOW_OK;

err:
	gst_buffer_unref (buf);	
	GST_ELEMENT_ERROR(wma, STREAM, DECODE, (NULL), ("%s() return un-accepted error %d", __FUNCTION__, GST_FLOW_ERROR));
	return GST_FLOW_ERROR;	
	
}

static gboolean
gst_wmadec_start (Gstwmadec *wma)
{
	int ret = 0;	

	wma->pDecoderState = NULL;
	wma->pCBTable = NULL;
	wma->poutbuf= NULL;
	wma->srcBitStream.bsCurBitOffset = 0;

	wma->sound.sndLen =0;

	wma->max_framesize = 0;

	wma->nextTS = GST_CLOCK_TIME_NONE;
	wma->duration = 0;

	wma->totalFrames = 0;

	wma->codec_time = 0;

	wma->init_error = FALSE;


	memset(&(wma->decoderConfig), 0, sizeof(wma->decoderConfig));
	
	ret = miscInitGeneralCallbackTable(&(wma->pCBTable));
	if (IPP_STATUS_NOERR != ret ){
		GST_ERROR_OBJECT (wma, "init table error");			
        	return FALSE;
	}		
	return TRUE;
}

static gboolean
gst_wmadec_stop (Gstwmadec *wma)
{

	if(wma->pDecoderState){
		DecoderFree_WMA(&(wma->pDecoderState));
		wma->pDecoderState = NULL;
	}
	
	if(wma->pCBTable){
		miscFreeGeneralCallbackTable(&(wma->pCBTable));
		wma->pCBTable = NULL;
	}

	if(wma->poutbuf){
		free(wma->poutbuf);
	}	


	return TRUE;
}

static gboolean 
gst_wmadec_sinkpad_event (GstPad *pad, GstEvent *event)
{
	Gstwmadec *wma = GST_WMADEC (GST_OBJECT_PARENT (pad));
	gboolean eventRet;


	switch (GST_EVENT_TYPE (event))
	{
	case GST_EVENT_NEWSEGMENT:
	{		
		eventRet = gst_pad_push_event (wma->srcpad, event);
		
		break;
	}
	case GST_EVENT_EOS:
			
		eventRet = gst_pad_push_event (wma->srcpad, event);
		break;
	default:
		eventRet = gst_pad_event_default (pad, event);
		break;
	}

	return eventRet;
}

static GstStateChangeReturn 
gst_wmadec_change_state (GstElement *element, GstStateChange transition)
{
	GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;
	Gstwmadec *wma = GST_WMADEC (element);
	
	switch (transition)
	{
	case GST_STATE_CHANGE_NULL_TO_READY:
		if(!gst_wmadec_start(wma)){
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
		if(!gst_wmadec_stop(wma)){
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
		GST_ERROR_OBJECT (wma, "state change failed");
		return GST_STATE_CHANGE_FAILURE;
	}
}

static void 
gst_wmadec_base_init (gpointer klass)
{
	static GstElementDetails wmadec_details = {
		"wma decoder", 
		"Codec/Decoder/Audio", 
		"WMA Decoder based-on IPP & CODEC", 
	        ""
	};

	GstElementClass *element_class = GST_ELEMENT_CLASS (klass);

	gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&src_factory));
	gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&sink_factory));

	gst_element_class_set_details (element_class, &wmadec_details);
}

static void 
gst_wmadec_class_init (GstwmadecClass *klass)
{
	GObjectClass *gobject_class  = (GObjectClass*) klass;
	GstElementClass *gstelement_class = (GstElementClass*) klass;

	parent_class = g_type_class_peek_parent(klass);
	
	gobject_class->set_property = gst_wmadec_set_property;
	gobject_class->get_property = gst_wmadec_get_property;
	
#ifdef DEBUG_PERFORMANCE	
	g_object_class_install_property (gobject_class, ARG_TOTALFRAME,
  		g_param_spec_int ("totalframes", "Totalframe of wmadec stream",
          	"Number of frame", 0, G_MAXINT, 0, G_PARAM_READABLE));
	g_object_class_install_property (gobject_class, ARG_CODECTIME,
  		g_param_spec_int64 ("codectime", "wma decode time",
          	"codec time (microsecond)", 0, G_MAXINT64, 0, G_PARAM_READABLE));
#endif


	gstelement_class->change_state = GST_DEBUG_FUNCPTR (gst_wmadec_change_state);
	GST_DEBUG_CATEGORY_INIT(wmadec_debug, "wmadec", 0, "IPP SW WMA Decoder Element");
}


static void 
gst_wmadec_init (Gstwmadec *wma, GstwmadecClass *klass)
{
	wma->sinkpad = gst_pad_new_from_template (
		gst_element_class_get_pad_template ((GstElementClass*)klass, "sink"), "sink");


	gst_pad_set_chain_function (wma->sinkpad, GST_DEBUG_FUNCPTR (gst_wmadec_chain));
	gst_pad_set_setcaps_function (wma->sinkpad, GST_DEBUG_FUNCPTR (gst_wmadec_sinkpad_setcaps));
	gst_pad_set_event_function (wma->sinkpad, GST_DEBUG_FUNCPTR (gst_wmadec_sinkpad_event));


	wma->srcpad = gst_pad_new_from_template (
		gst_element_class_get_pad_template ((GstElementClass*)klass, "src"), "src");

	gst_element_add_pad (GST_WMADEC (wma), wma->sinkpad);
	gst_element_add_pad (GST_WMADEC (wma), wma->srcpad);

	/* Init config, since we do not have such info */
	
	wma->decoderConfig.iRmsAmplitudeRef = 0;
	wma->decoderConfig.iRmsAmplitudeTarget = 0;
	wma->decoderConfig.iPeakAmplitudeRef = 0;
	wma->decoderConfig.iPeakAmplitudeTarget = 0;
	
}

static gboolean 
plugin_init (GstPlugin *plugin)
{	
	return gst_element_register (plugin, "wmadec", GST_RANK_PRIMARY+2, GST_TYPE_WMADEC);
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR, 
                   GST_VERSION_MINOR, 
                   "mvl_wmadec", 
                   "wmadec decoder based on IPP, "__DATE__,
                   plugin_init, 
                   VERSION, 
                   "LGPL",
                   "", 
                   "");



/* EOF */
