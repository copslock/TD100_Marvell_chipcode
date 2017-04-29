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

/* gstwmvdec.c */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gstwmvdec.h"
#ifdef DEBUG_PERFORMANCE
#include  <sys/time.h>

        struct timeval tstart, tend;
        struct timezone tz;
#endif

GST_DEBUG_CATEGORY_STATIC (wmvdec_debug);
#define GST_CAT_DEFAULT wmvdec_debug

/******************************************************************************
// IPP specific function entries
******************************************************************************/

void 
__IPP_OutputPicture_wmv (IppPicture *pSrcPicture, GstBuffer *PicROI_buf)
{
	int i;

	Ipp8u *pResultY = pSrcPicture->ppPicPlane[0];
	Ipp8u *pResultU = pSrcPicture->ppPicPlane[1];
	Ipp8u *pResultV = pSrcPicture->ppPicPlane[2];

	int step = pSrcPicture->picPlaneStep[0];

	GST_BUFFER_OFFSET (PicROI_buf) = 0;

	for(i = 0; i < pSrcPicture->picHeight; i ++) {
		memcpy (GST_BUFFER_DATA (PicROI_buf) + GST_BUFFER_OFFSET (PicROI_buf), \
			pResultY, pSrcPicture->picWidth);

		GST_BUFFER_OFFSET (PicROI_buf) += pSrcPicture->picWidth;
		pResultY += step;
	}

	step = pSrcPicture->picPlaneStep[1];

	

	for(i = 0; i < (pSrcPicture->picHeight >> 1); i ++) {
		memcpy (GST_BUFFER_DATA (PicROI_buf) + GST_BUFFER_OFFSET (PicROI_buf), \
			pResultU, (pSrcPicture->picWidth) >> 1);

		GST_BUFFER_OFFSET (PicROI_buf) += (pSrcPicture->picWidth >> 1);
		pResultU += step;
	}

	step = pSrcPicture->picPlaneStep[2];


	for(i = 0; i < (pSrcPicture->picHeight >> 1); i ++) {
		memcpy (GST_BUFFER_DATA (PicROI_buf) + GST_BUFFER_OFFSET (PicROI_buf), \
			pResultV, (pSrcPicture->picWidth) >> 1);

		GST_BUFFER_OFFSET (PicROI_buf) += (pSrcPicture->picWidth >> 1);
		pResultV += step;
	}
}


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
	//GST_STATIC_CAPS ("ANY")/*	
	GST_STATIC_CAPS ("video/x-wmv," 
					 "wmvversion = (int) 3, "
					 "format=(fourcc)WMV3, fourcc=(fourcc)WMV3, "
					 "width = (int) [ 16, 2048 ], " 
					 "height = (int) [ 16, 2048 ], " 					 
					 "framerate = (fraction) [ 0, MAX ]")
					 );

static GstStaticPadTemplate src_factory = 
	GST_STATIC_PAD_TEMPLATE ("src", GST_PAD_SRC, GST_PAD_ALWAYS, 
	GST_STATIC_CAPS ("video/x-raw-yuv," 
					 "format = (fourcc) { I420 }, " 
					 "width = (int) [ 16, 4096 ], " 
					 "height = (int) [ 16, 4096 ], " 
					 "framerate = (fraction) [ 0, 60 ]"));



GST_BOILERPLATE (Gstwmvdec, gst_wmvdec, GstElement, GST_TYPE_ELEMENT);


static gboolean 
gst_wmvdec_sinkpad_setcaps (GstPad *pad, GstCaps *caps)
{
	Gstwmvdec *wmv = GST_WMVDEC (GST_OBJECT_PARENT (pad));
	
	guint8 *pseq_head = NULL;
	guint seq_head_size = 0;
	int codec_version = 0;
	GstBuffer *buf = NULL;
	int ret = 0;
	
	int i = 0;
	gchar *name;
	GstStructure *str;
	const GValue *value;
	int stru_num = gst_caps_get_size (caps);	

	printf("gst_wmvdec_sinkpad_setcaps stru_num =%d\n",stru_num);		
	
	puts("****************************");
	for (i = 0; i < stru_num; i++) {
		str = gst_caps_get_structure (caps, i);
		name = gst_structure_get_name (str);
		g_print ("name: %s\n", name);

		gchar *sstr;

		sstr = gst_structure_to_string (str);
		g_print ("%s\n", sstr);
		g_free (sstr);		
	}	
	puts("****************************");

	if ((value = gst_structure_get_value (str, "framerate"))) {

		wmv->num = gst_value_get_fraction_numerator(value);
		wmv->den = gst_value_get_fraction_denominator(value);

		//printf("num=%d, den=%d\n", wmv->num, wmv->den);		
	}
	
	if (gst_structure_get_int(str, "wmvversion", &(wmv->wmvversion) )){
		printf("wmvversion=%d\n", wmv->wmvversion);
	}

	if(3 == wmv->wmvversion){
		/* current IPP only support WMV3 */
		codec_version = 5;
	}else{
		puts("do not support such version");
		return FALSE;  
	}

	if (gst_structure_get_int(str, "width", &(wmv->width) )){
		//printf("width=%d\n", wmv->width);
	}

	if (gst_structure_get_int(str, "height", &(wmv->height) )){
		//printf("height=%d\n", wmv->height);
	}

	wmv->pic_size = wmv->width*wmv->height;
	wmv->pic_size = wmv->pic_size + (wmv->pic_size >> 1);
	
	//printf("pic_size=%d\n", wmv->pic_size);

	if ((value = gst_structure_get_value (str, "codec_data"))) {		
		
		buf = gst_value_get_buffer (value);
		pseq_head = GST_BUFFER_DATA (buf);
		seq_head_size = GST_BUFFER_SIZE (buf);		

		/*for(i=0; i<seq_head_size; i++){
			printf("pseq_head[%d]=%x\n", i, pseq_head[i]);
		}*/
	}

	if((pseq_head)&&(!wmv->pWMVDecoderState)){			
		GstCaps *Tmp;
		ret = DecoderInitAlloc_WMVVideo(wmv->width, wmv->height, codec_version, 1, 
			pseq_head, seq_head_size, wmv->pCBTable, &(wmv->pWMVDecoderState));

		
		if (IPP_STATUS_INIT_OK != ret ){
			puts("init wmvdecoder error");				
                        GST_DEBUG_OBJECT(wmv,"DecoderInitAlloc_WMVVideo return error %d", ret);
	        	return FALSE;
		}
		
		Tmp = gst_caps_new_simple ("video/x-raw-yuv",
			"format", GST_TYPE_FOURCC, GST_MAKE_FOURCC ('I', '4', '2', '0'),
			"width", G_TYPE_INT, wmv->width,
			"height", G_TYPE_INT, wmv->height,
			"framerate", GST_TYPE_FRACTION, wmv->num, wmv->den,
			NULL);
		gst_pad_set_caps(wmv->srcpad, Tmp);
		gst_caps_unref(Tmp);

	}



	return TRUE;	
}

static void 
gst_wmvdec_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{		
	//Gstwmvdec *wmv = GST_WMVDEC (object);

	switch (prop_id)
	{	
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void 
gst_wmvdec_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
#ifdef DEBUG_PERFORMANCE
	Gstwmvdec *wmv = GST_WMVDEC (object);
#endif	
	switch (prop_id)
	{	

#ifdef DEBUG_PERFORMANCE
	case ARG_TOTALFRAME:
		g_value_set_int (value, wmv->totalFrames);		
		break;
	case ARG_CODECTIME:
		g_value_set_int64 (value, wmv->codec_time);		
		break;
#endif

	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}
//FILE file;


static GstFlowReturn _wmv_push_data(Gstwmvdec *wmv)
{
	int ret = 0;

	GstBuffer *outBuf = NULL;

	
	ret = gst_pad_alloc_buffer_and_set_caps (wmv->srcpad, 0, wmv->pic_size, GST_PAD_CAPS (wmv->srcpad), &outBuf);
	if(ret != GST_FLOW_OK)
	{
		GST_ERROR_OBJECT (wmv, "wmv can not malloc buffer");								
		return ret;								
	}

#ifdef DEBUG_FULL_SPEED	

	GST_BUFFER_TIMESTAMP (outBuf) = GST_CLOCK_TIME_NONE;
	GST_BUFFER_DURATION (outBuf) = GST_CLOCK_TIME_NONE;
#else

	/* time stamp reorder, choose the smaller one */
	if(wmv->nextTS <= wmv->saveTS){
		GST_BUFFER_TIMESTAMP (outBuf) = wmv->nextTS;
		
	}else{
		GST_BUFFER_TIMESTAMP (outBuf) = wmv->saveTS;
		wmv->saveTS = wmv->nextTS;
	}
	GST_BUFFER_DURATION (outBuf) = wmv->duration;
	
#endif	


	/* pack up picture's ROI to GstBuffer */
	__IPP_OutputPicture_wmv (&(wmv->decPicture), outBuf);

	//fwrite(GST_BUFFER_DATA (outBuf), 1, GST_BUFFER_SIZE (outBuf), file);

	if(outBuf->size != 0)
	{
		if(wmv->bNewSegReceivedAfterflush) {
			ret = gst_pad_push (wmv->srcpad, outBuf);
		}else{
			//If new segment event hasn't received, shouldn't push buffer to sink. Otherwise, sink will throw warnings and work abnormal.
			//Sometimes, user seeking position exceeds the duration of file, then demuxer only fire flush event without newseg event. And demuxer also fire EOS event. At EOS, decoder probably still output some frames, but those frames shouldn't be pushed to sink because no newseg event.
			gst_buffer_unref(outBuf);
			ret = GST_FLOW_OK;
		}

		if (GST_FLOW_OK != ret) {
			GST_ERROR_OBJECT (wmv, "The decoded frame did not successfully push out to downstream element");
			return ret;
		}
	} else {
		GST_ERROR_OBJECT (wmv, "The output buffer is empty!");
	}
	wmv->totalFrames ++;
	
	return GST_FLOW_OK;
	
}

static gboolean 
gst_wmvdec_sinkpad_event (GstPad *pad, GstEvent *event)
{
	gboolean eventRet = 0;
	Gstwmvdec *wmv = GST_WMVDEC (GST_OBJECT_PARENT (pad));

	switch(GST_EVENT_TYPE (event))
	{
	
	case GST_EVENT_EOS:

		/* get the last frame */
		if ((!wmv->dec_error)&&(wmv->pWMVDecoderState)) {
			int ret = 0;
#ifdef DEBUG_PERFORMANCE
                        gettimeofday (&tstart, &tz);
#endif
			ret = Decode_WMVVideo(&(wmv->srcBitStream), &(wmv->decPicture), wmv->pWMVDecoderState, 1);
#ifdef DEBUG_PERFORMANCE
                        gettimeofday (&tend, &tz);
                        wmv->codec_time += (tend.tv_sec - tstart.tv_sec)*1000000 + (tend.tv_usec - tstart.tv_usec);
#endif
			if((IPP_STATUS_NOERR == ret)||(IPP_STATUS_FRAME_COMPLETE == ret)){
				ret = _wmv_push_data(wmv);
				if (GST_FLOW_OK != ret) {	
					puts("push problem");					
				}		
			}			
		}
		
		GST_DEBUG_OBJECT (wmv, "Decoding Over, Total %5d frames", wmv->totalFrames);

		eventRet = gst_pad_push_event (wmv->srcpad, event);
		break;
		
	case GST_EVENT_NEWSEGMENT:
		eventRet = gst_pad_push_event (wmv->srcpad, event);
		wmv->bNewSegReceivedAfterflush = 1;
		break;
	case GST_EVENT_FLUSH_STOP:
		wmv->bNewSegReceivedAfterflush = 0;
	default:
		eventRet = gst_pad_event_default (pad, event);
		break;
	}

	return eventRet;
}

static GstFlowReturn 
gst_wmvdec_chain (GstPad *pad, GstBuffer *buf)
{
	Gstwmvdec *wmv = GST_WMVDEC (GST_OBJECT_PARENT (pad));
	int ret = 0;

	if (GST_BUFFER_TIMESTAMP (buf) != GST_CLOCK_TIME_NONE) {	
		wmv->nextTS = GST_BUFFER_TIMESTAMP (buf);
		wmv->duration = GST_BUFFER_DURATION (buf);
 	}

	wmv->srcBitStream.pBsBuffer = GST_BUFFER_DATA (buf);
	wmv->srcBitStream.pBsCurByte = wmv->srcBitStream.pBsBuffer;
	wmv->srcBitStream.bsByteLen = GST_BUFFER_SIZE (buf);
	wmv->srcBitStream.bsCurBitOffset = 0;

#ifdef DEBUG_PERFORMANCE
                        gettimeofday (&tstart, &tz);
#endif
	ret = Decode_WMVVideo(&(wmv->srcBitStream), &(wmv->decPicture), wmv->pWMVDecoderState, 0);
#ifdef DEBUG_PERFORMANCE
                        gettimeofday (&tend, &tz);
                        wmv->codec_time += (tend.tv_sec - tstart.tv_sec)*1000000 + (tend.tv_usec - tstart.tv_usec);
#endif

	if(wmv->firsttime){
		/* no output */
		wmv->saveTS = wmv->nextTS;			
		wmv->firsttime = FALSE;
	}else{
		if(GST_CLOCK_TIME_NONE == wmv->saveTS){
			wmv->saveTS += (wmv->nextTS - wmv->saveTS)/2;
		}

	}
	if((IPP_STATUS_NOERR == ret)||(IPP_STATUS_FRAME_COMPLETE == ret)){
		ret = _wmv_push_data(wmv);
		if (GST_FLOW_OK != ret) {	
			puts("push problem");
			gst_buffer_unref (buf);	
			return ret;
		}		
	}else{
		puts("wmv decoder error");
		goto err;
	}			
		
		

	
	gst_buffer_unref (buf);	
	return GST_FLOW_OK;

err:
	
	wmv->dec_error = TRUE;
	gst_buffer_unref (buf);	
	GST_ELEMENT_ERROR(wmv, STREAM, DECODE, (NULL), ("%s() return un-accepted error %d", __FUNCTION__, GST_FLOW_ERROR));
	return GST_FLOW_ERROR;
}

static gboolean
gst_wmvdec_start (Gstwmvdec *wmv)
{
	int ret = 0;
	

	wmv->pWMVDecoderState = NULL;
	wmv->pCBTable = NULL;
	
	wmv->nextTS = GST_CLOCK_TIME_NONE;
	wmv->saveTS = GST_CLOCK_TIME_NONE;
	wmv->duration = 0;

	wmv->num = 0;
	wmv->den = 1;
	wmv->totalFrames = 0;
	wmv->codec_time = 0;
	wmv->srcBitStream.bsCurBitOffset = 0;


	wmv->wmvversion = 0;
	wmv->width= 0;
	wmv->height= 0;
	wmv->pic_size= 0;

	wmv->dec_error = FALSE;
	wmv->firsttime = TRUE;

	//file=fopen("wmv.yuv", "wb");


	
	ret = miscInitGeneralCallbackTable(&(wmv->pCBTable));
	if (IPP_STATUS_NOERR != ret ){
		puts("init table error");			
        	return FALSE;
	}
	
	return TRUE;
}

static gboolean
gst_wmvdec_stop (Gstwmvdec *wmv)
{
	if(wmv->pWMVDecoderState){
		DecoderFree_WMVVideo(wmv->pWMVDecoderState);
		wmv->pWMVDecoderState = NULL;
	}
	
	if(wmv->pCBTable){
		miscFreeGeneralCallbackTable(&(wmv->pCBTable));
		wmv->pCBTable = NULL;
	}


	//fcose(file);
	return TRUE;
}

static GstStateChangeReturn 
gst_wmvdec_change_state (GstElement *element, GstStateChange transition)
{
	GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;
	Gstwmvdec *wmv = GST_WMVDEC (element);
	
	switch (transition)
	{
	case GST_STATE_CHANGE_NULL_TO_READY:
		if(!gst_wmvdec_start(wmv)){
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
		if(!gst_wmvdec_stop(wmv)){
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
		GST_ERROR_OBJECT (wmv, "state change failed");
		return GST_STATE_CHANGE_FAILURE;
	}
}

static void 
gst_wmvdec_base_init (gpointer klass)
{
	static GstElementDetails wmvdec_details = {
		"wmv decoder", 
		"Codec/Decoder/Video", 
		"SOFTWARE WMV Decoder based-on IPP & CODEC", 
	        ""
	};

	GstElementClass *element_class = GST_ELEMENT_CLASS (klass);

	gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&src_factory));
	gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&sink_factory));

	gst_element_class_set_details (element_class, &wmvdec_details);
}

static void 
gst_wmvdec_class_init (GstwmvdecClass *klass)
{
	GObjectClass *gobject_class  = (GObjectClass*) klass;
	GstElementClass *gstelement_class = (GstElementClass*) klass;

	parent_class = g_type_class_peek_parent(klass);

	gobject_class->set_property = gst_wmvdec_set_property;
	gobject_class->get_property = gst_wmvdec_get_property;
	
#ifdef DEBUG_PERFORMANCE	
	g_object_class_install_property (gobject_class, ARG_TOTALFRAME,
  		g_param_spec_int ("totalframes", "Totalframe of wmvdec stream",
          	"Number of frame", 0, G_MAXINT, 0, G_PARAM_READABLE));
	g_object_class_install_property (gobject_class, ARG_CODECTIME,
  		g_param_spec_int64 ("codectime", "wmv decode time",
          	"codec time (microsecond)", 0, G_MAXINT64, 0, G_PARAM_READABLE));
#endif

	
	gstelement_class->change_state = GST_DEBUG_FUNCPTR (gst_wmvdec_change_state);
	GST_DEBUG_CATEGORY_INIT(wmvdec_debug, "wmvdec", 0, "IPP Software WMV3 SP&MP Decoder Element");
}


static void 
gst_wmvdec_init (Gstwmvdec *wmv, GstwmvdecClass *klass)
{


	wmv->sinkpad = gst_pad_new_from_template (
		gst_element_class_get_pad_template ((GstElementClass*)klass, "sink"), "sink");


	gst_pad_set_chain_function (wmv->sinkpad, GST_DEBUG_FUNCPTR (gst_wmvdec_chain));
	gst_pad_set_setcaps_function (wmv->sinkpad, GST_DEBUG_FUNCPTR (gst_wmvdec_sinkpad_setcaps));
	gst_pad_set_event_function (wmv->sinkpad, GST_DEBUG_FUNCPTR (gst_wmvdec_sinkpad_event));



	wmv->srcpad = gst_pad_new_from_template (
		gst_element_class_get_pad_template ((GstElementClass*)klass, "src"), "src");

	gst_element_add_pad (GST_WMVDEC (wmv), wmv->sinkpad);
	gst_element_add_pad (GST_WMVDEC (wmv), wmv->srcpad);

	wmv->bNewSegReceivedAfterflush = 0;
}

static gboolean 
plugin_init (GstPlugin *plugin)
{	
	return gst_element_register (plugin, "wmvdec", GST_RANK_PRIMARY+2, GST_TYPE_WMVDEC);
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR, 
                   GST_VERSION_MINOR, 
                   "mvl_wmvdec", 
                   "wmvdec decoder based on IPP, "__DATE__,
                   plugin_init, 
                   VERSION, 
                   "LGPL",
                   "", 
                   "");



/* EOF */
