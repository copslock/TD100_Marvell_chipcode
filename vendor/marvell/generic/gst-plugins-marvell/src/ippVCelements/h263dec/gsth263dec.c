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

#include "gsth263dec.h"

#include <string.h>

#ifdef DEBUG_PERFORMANCE
#include  <sys/time.h>

        struct timeval tstart, tend;
        struct timezone tz;

#endif

GST_DEBUG_CATEGORY_STATIC (h263dec_debug);
#define GST_CAT_DEFAULT h263dec_debug

void 
IPP_OutputPicture_H263 (IppPicture *pSrcPicture, GstBuffer *PicROI_buf)
{
	int i;

	Ipp8u *pResultY = pSrcPicture->ppPicPlane[0];
	Ipp8u *pResultU = pSrcPicture->ppPicPlane[1];
	Ipp8u *pResultV = pSrcPicture->ppPicPlane[2];

	int step = pSrcPicture->picPlaneStep[0];
	GST_BUFFER_OFFSET (PicROI_buf) = 0;

	for(i = 0; i < pSrcPicture->picHeight; i ++) {
		memcpy (GST_BUFFER_DATA (PicROI_buf) + GST_BUFFER_OFFSET (PicROI_buf), pResultY, pSrcPicture->picWidth);
		GST_BUFFER_OFFSET (PicROI_buf) += pSrcPicture->picWidth;
		pResultY += step;
	}

	for(i = 0; i < (pSrcPicture->picHeight >> 1); i ++) {
		memcpy (GST_BUFFER_DATA (PicROI_buf) + GST_BUFFER_OFFSET (PicROI_buf), pResultU, (pSrcPicture->picWidth) >> 1);
		GST_BUFFER_OFFSET (PicROI_buf) += (pSrcPicture->picWidth >> 1);
		pResultU += (step >> 1);
	}

	for(i = 0; i < (pSrcPicture->picHeight >> 1); i ++) {
		memcpy (GST_BUFFER_DATA (PicROI_buf) + GST_BUFFER_OFFSET (PicROI_buf), pResultV, (pSrcPicture->picWidth) >> 1);
		GST_BUFFER_OFFSET (PicROI_buf) += (pSrcPicture->picWidth >> 1);
		pResultV += (step >> 1);
	}
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
	ARG_WORK_MODE,
	ARG_FRAME_WIDTH,
	ARG_FRAME_HEIGHT,

#ifdef DEBUG_PERFORMANCE	
	ARG_TOTALFRAME,	
	ARG_CODECTIME,	
#endif


};

static GstStaticPadTemplate sink_factory = \
	GST_STATIC_PAD_TEMPLATE ("sink", GST_PAD_SINK, GST_PAD_ALWAYS, \
    GST_STATIC_CAPS ("video/x-h263, " \
					 "width = (int) [ 16, 1408 ], " \
					 "height = (int) [ 16, 1152 ], " \
					 "framerate = (fraction) [ 0, 60 ]"));

static GstStaticPadTemplate src_factory = \
	GST_STATIC_PAD_TEMPLATE ("src", GST_PAD_SRC, GST_PAD_ALWAYS, \
	GST_STATIC_CAPS ("video/x-raw-yuv," \
					 "format = (fourcc) { I420 }, " \
					 "width = (int) [ 16, 1408 ], " \
					 "height = (int) [ 16, 1152 ], " \
					 "framerate = (fraction) [ 0, 60 ]"));


GST_BOILERPLATE (GstH263Dec, gst_h263dec, GstElement, GST_TYPE_ELEMENT);


static gboolean 
gst_h263dec_setcaps (GstPad *pad, GstCaps *caps)
{
	GstH263Dec *h263dec = GST_H263DEC (GST_OBJECT_PARENT (pad));

	int i = 0;
	gchar *name;
	GstStructure *str;
	const GValue *value;
	int stru_num = gst_caps_get_size (caps);

	GST_INFO_OBJECT (h263dec, "gst_h263dec_setcaps stru_num =%d\n",stru_num);	
	GST_INFO_OBJECT (h263dec, "****************************");
	for (i = 0; i < stru_num; i++) {
		str = gst_caps_get_structure (caps, i);
		name = gst_structure_get_name (str);
		GST_INFO_OBJECT (h263dec, "name: %s\n", name);

		gchar *sstr;

		sstr = gst_structure_to_string (str);
		GST_INFO_OBJECT (h263dec, "%s\n", sstr);
		g_free (sstr);		
	}	
	GST_INFO_OBJECT (h263dec, "****************************");

	if ((value = gst_structure_get_value (str, "framerate"))) {
		h263dec->num = gst_value_get_fraction_numerator(value);
		h263dec->den = gst_value_get_fraction_denominator(value);	

		GST_INFO_OBJECT (h263dec, "h263dec->num=%d,h263dec->den=%d\n",h263dec->num,h263dec->den);		
	}
	

	return TRUE;
}

static void 
gst_h263dec_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	//GstH263Dec *h263dec = GST_H263DEC (object);

	switch (prop_id)
	{	
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void 
gst_h263dec_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
#ifdef DEBUG_PERFORMANCE
	GstH263Dec *h263dec = GST_H263DEC (object);
#endif
	switch (prop_id)
	{	

#ifdef DEBUG_PERFORMANCE
	case ARG_TOTALFRAME:
		g_value_set_int (value, h263dec->totalFrames);		
		break;
	case ARG_CODECTIME:
		g_value_set_int64 (value, h263dec->codec_time);		
		break;
#endif

	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}


static gboolean 
gst_h263dec_sinkpad_event (GstPad *pad, GstEvent *event)
{
	GstH263Dec *h263dec = GST_H263DEC (GST_OBJECT_PARENT (pad));
	gboolean eventRet = FALSE;

	switch (GST_EVENT_TYPE (event))
	{

	/*
	case GST_EVENT_NEWSEGMENT:
		
		break;
		*/
	case GST_EVENT_EOS:

		eventRet = gst_pad_push_event (h263dec->srcpad, event);
		break;
	default:
		eventRet = gst_pad_event_default (pad, event);
		break;
	}

	return eventRet;
}

static GstFlowReturn _h263_push_data(GstH263Dec *h263dec)
{
	int ret = 0;

	GstBuffer *outBuf = NULL;

	ret = gst_pad_alloc_buffer_and_set_caps (h263dec->srcpad, 0, h263dec->picDataSize, GST_PAD_CAPS (h263dec->srcpad), &outBuf);
	if(ret != GST_FLOW_OK)
	{
		GST_ERROR_OBJECT (h263dec, "h263dec can not malloc buffer");								
		return ret;								
	}
				
#ifdef DEBUG_FULL_SPEED	

	GST_BUFFER_TIMESTAMP (outBuf) = GST_CLOCK_TIME_NONE;
	GST_BUFFER_DURATION (outBuf) = GST_CLOCK_TIME_NONE;
#else
	GST_BUFFER_TIMESTAMP (outBuf) = h263dec->nextTS;
	GST_BUFFER_DURATION (outBuf) = h263dec->duration;
#endif						
				
				
	/* pack up picture's ROI to GstBuffer */
	IPP_OutputPicture_H263 (&(h263dec->decPicture), outBuf);
	ret = gst_pad_push (h263dec->srcpad, outBuf);
	if (GST_FLOW_OK != ret) {
		GST_ERROR_OBJECT (h263dec, "h263dec push error ret=%d\n",ret);
		return ret;
	}

	h263dec->totalFrames ++;
	
	return GST_FLOW_OK;
	
}


static GstFlowReturn 
gst_h263dec_chain (GstPad *pad, GstBuffer *buf)
{
	GstH263Dec *h263dec = GST_H263DEC (GST_OBJECT_PARENT (pad));
	GstFlowReturn ret = GST_FLOW_OK;

	if (GST_BUFFER_TIMESTAMP (buf) != GST_CLOCK_TIME_NONE) {
		h263dec->nextTS = GST_BUFFER_TIMESTAMP (buf);;
		h263dec->duration = GST_BUFFER_DURATION (buf);
 	}

	h263dec->srcBitStream.pBsBuffer = GST_BUFFER_DATA (buf);
	h263dec->srcBitStream.pBsCurByte = h263dec->srcBitStream.pBsBuffer;
	h263dec->srcBitStream.bsByteLen = GST_BUFFER_SIZE (buf);
	h263dec->srcBitStream.bsCurBitOffset = 0;	
	
	if (NULL == h263dec->pH263DecState) {
		h263dec->retCode = DecoderInitAlloc_H263Video(&(h263dec->srcBitStream), h263dec->pCBTable, &(h263dec->pH263DecState));
		
		if (h263dec->retCode != IPP_STATUS_NOERR) {
			h263dec->srcBitStream.pBsCurByte = h263dec->srcBitStream.pBsBuffer;
			h263dec->srcBitStream.bsCurBitOffset = 0;
		}

		if (NULL != h263dec->pH263DecState) {
			h263dec->retCode = DecodeSendCmd_H263Video(IPPVC_SET_NSCCHECKDISABLE,
				&(h263dec->NSCCheckDisable), NULL, h263dec->pH263DecState);
		}
	}


	if (NULL != h263dec->pH263DecState) {			
					
#ifdef DEBUG_PERFORMANCE
        gettimeofday (&tstart, &tz);
#endif
		
		h263dec->retCode = Decode_H263Video(&(h263dec->srcBitStream), &(h263dec->decPicture), h263dec->pH263DecState);		
#ifdef DEBUG_PERFORMANCE

        gettimeofday (&tend, &tz);
        h263dec->codec_time += (tend.tv_sec - tstart.tv_sec)*1000000 + (tend.tv_usec - tstart.tv_usec);
#endif

		if (h263dec->retCode == IPP_STATUS_NOERR) {
			/* set srcpad caps and set some stream related information */
			if (!h263dec->haveCaps) {
				GstCaps *Tmp;
				h263dec->frameWidth = h263dec->decPicture.picWidth;
				h263dec->frameHeight = h263dec->decPicture.picHeight;

				/* only compute 1 time, use every output */
				h263dec->picSize = h263dec->frameWidth * h263dec->frameHeight;
				h263dec->picDataSize = h263dec->picSize + (h263dec->picSize >> 1);

				Tmp = gst_caps_new_simple ("video/x-raw-yuv", \
					"format", GST_TYPE_FOURCC, GST_MAKE_FOURCC ('I', '4', '2', '0'), \
					"width", G_TYPE_INT, h263dec->frameWidth, 
					"height", G_TYPE_INT, h263dec->frameHeight, 
					"framerate", GST_TYPE_FRACTION, h263dec->num, h263dec->den,
					//"framerate", GST_TYPE_FRACTION, 30, 1,
					NULL);
				gst_pad_set_caps(h263dec->srcpad, Tmp);
				gst_caps_unref(Tmp);

				h263dec->haveCaps = TRUE;
			}

			ret = _h263_push_data(h263dec);
			if (GST_FLOW_OK != ret) {		
				GST_ERROR_OBJECT (h263dec, "h263dec push error");					
				goto chain_ret;
			}
		} else if (h263dec->retCode != IPP_STATUS_ERR) {
			ret = GST_FLOW_ERROR;	//!!!! What does the code mean? why h263dec->retCode != IPP_STATUS_ERR result in GST_FLOW_ERROR
			goto chain_ret;
		}
		
	}

chain_ret:
	gst_buffer_unref (buf);
	if(ret < GST_FLOW_UNEXPECTED) {
		GST_ELEMENT_ERROR(h263dec, STREAM, DECODE, (NULL), ("%s() return un-accepted error %d", __FUNCTION__, ret));
	}

	return ret;
}

static gboolean
gst_h263dec_start (GstH263Dec *h263dec)
{
	h263dec->pCBTable = NULL;
	h263dec->pH263DecState = NULL;
	h263dec->NSCCheckDisable = 1;
	h263dec->haveCaps = FALSE;

	h263dec->nextTS = GST_CLOCK_TIME_NONE;
	h263dec->duration = 0;

	h263dec->num = 0;
	h263dec->den = 1;
	h263dec->srcBitStream.bsCurBitOffset = 0;

	h263dec->totalFrames = 0;
	h263dec->codec_time = 0;

	miscInitGeneralCallbackTable (&(h263dec->pCBTable));	

	return TRUE;	
}

static gboolean
gst_h263dec_stop (GstH263Dec *h263dec)
{
	if (h263dec->pH263DecState) {
		DecoderFree_H263Video(&(h263dec->pH263DecState));
	}
	
	miscFreeGeneralCallbackTable (&(h263dec->pCBTable));


	return TRUE;
	
}

static GstStateChangeReturn 
gst_h263dec_change_state (GstElement *element, GstStateChange transition)
{
	GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;
	GstH263Dec *h263dec = GST_H263DEC (element);

	switch (transition)
	{
	case GST_STATE_CHANGE_NULL_TO_READY:
		GST_INFO_OBJECT (h263dec, "h263dec GST_STATE_CHANGE_NULL_TO_READY");	

		if(!gst_h263dec_start(h263dec)){
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
		GST_INFO_OBJECT (h263dec, "h263dec GST_STATE_CHANGE_READY_TO_NULL");
		
		if(!gst_h263dec_stop(h263dec)){
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
		GST_ERROR_OBJECT (h263dec, "state change failed");
		return GST_STATE_CHANGE_FAILURE;
	}
	
}

static void 
gst_h263dec_base_init (gpointer g_klass)
{
	static GstElementDetails h263dec_details = {
		"H.263 Video Decoder", 
		"Codec/Decoder/Video", 
		"H.263 Video Decoder based-on IPP & CODEC", 
		""
	};
	GstElementClass *element_class = GST_ELEMENT_CLASS (g_klass);

	gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&src_factory));
	gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&sink_factory));

	gst_element_class_set_details (element_class, &h263dec_details);
}

static void 
gst_h263dec_class_init (GstH263DecClass *klass)
{
	GObjectClass *gobject_class  = (GObjectClass*) klass;
	GstElementClass *gstelement_class = (GstElementClass*) klass;

	parent_class = g_type_class_ref (GST_TYPE_ELEMENT);

	gobject_class->set_property = gst_h263dec_set_property;
	gobject_class->get_property = gst_h263dec_get_property;

#ifdef DEBUG_PERFORMANCE	
	g_object_class_install_property (gobject_class, ARG_TOTALFRAME,
  		g_param_spec_int ("totalframes", "Totalframe of h263stream",
          	"Number of frame", 0, G_MAXINT, 0, G_PARAM_READABLE));
	g_object_class_install_property (gobject_class, ARG_CODECTIME,
  		g_param_spec_int64 ("codectime", "h263 decode time",
          	"codec time (microsecond)", 0, G_MAXINT64, 0, G_PARAM_READABLE));
#endif


	gstelement_class->change_state = gst_h263dec_change_state;

}


static void 
gst_h263dec_init (GstH263Dec *h263dec, GstH263DecClass *h263dec_klass)
{
	GstElementClass *klass = GST_ELEMENT_CLASS (h263dec_klass);

	h263dec->sinkpad = gst_pad_new_from_template (
		gst_element_class_get_pad_template (klass, "sink"), "sink");

	gst_pad_set_setcaps_function (h263dec->sinkpad, gst_h263dec_setcaps);
	gst_pad_set_event_function (h263dec->sinkpad, gst_h263dec_sinkpad_event);
	gst_pad_set_chain_function (h263dec->sinkpad, gst_h263dec_chain);

	gst_element_add_pad (GST_ELEMENT (h263dec), h263dec->sinkpad);

	h263dec->srcpad = gst_pad_new_from_template (
		gst_element_class_get_pad_template (klass, "src"), "src");	

	gst_element_add_pad (GST_ELEMENT (h263dec), h263dec->srcpad);
	
}

static gboolean 
plugin_init (GstPlugin *plugin)
{	
	return gst_element_register (plugin, "h263dec", GST_RANK_PRIMARY+2, GST_TYPE_H263DEC);
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR, 
                   GST_VERSION_MINOR, 
                   "mvl_h263dec", 
                   "h263 decoder based on IPP, build date "__DATE__,
                   plugin_init, 
                   VERSION, 
                   "LGPL",
                   "", 
                   "");


/* EOF */
