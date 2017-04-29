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

/* gstippcnmassist.c */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include "gstippcnmassist.h"

#if 0
#define assist_myecho(...)	printf(__VA_ARGS__)
#else
#define assist_myecho(...)
#endif


GST_DEBUG_CATEGORY_STATIC (ippcnmassist_debug);
#define GST_CAT_DEFAULT ippcnmassist_debug

enum {

	/* fill above with user defined signals */
	LAST_SIGNAL
};

enum {
	ARG_0,
	/* fill below with user defined arguments */

};

static GstStaticPadTemplate sink_factory =
	GST_STATIC_PAD_TEMPLATE ("sink", GST_PAD_SINK, GST_PAD_ALWAYS,
	GST_STATIC_CAPS("video/x-h264, ippcnmassistparsed = (boolean) false"
					";video/mpeg, mpegversion = (int) 4, systemstream = (boolean) false, ippcnmassistparsed = (boolean) false"
					";video/x-xvid, ippcnmassistparsed = (boolean) false"
					";video/x-wmv, wmvversion = (int) 3, format=(fourcc)WMV3, fourcc=(fourcc)WMV3, ippcnmassistparsed = (boolean) false"));
static GstStaticPadTemplate src_factory =
	GST_STATIC_PAD_TEMPLATE ("src", GST_PAD_SRC, GST_PAD_ALWAYS,
	GST_STATIC_CAPS("video/x-h264, ippcnmassistparsed = (boolean) true, cnmnotsupport = (int) {0,1}"
					";video/mpeg, mpegversion = (int) 4, systemstream = (boolean) false, ippcnmassistparsed = (boolean) true, cnmnotsupport = (int) {0,1}"
					";video/x-xvid, ippcnmassistparsed = (boolean) true, cnmnotsupport = (int) {0,1}"
					";video/x-wmv, wmvversion = (int) 3, format=(fourcc)WMV3, fourcc=(fourcc)WMV3, ippcnmassistparsed = (boolean) true, cnmnotsupport = (int) {0,1}"));


GST_BOILERPLATE (GstIppCnmAssist, gst_ippcnmassist, GstElement, GST_TYPE_ELEMENT);


static unsigned char*
seek_H264ByteStreamSPS(unsigned char* pData, int len)
{
	unsigned int code;
	unsigned char* pStartCode = pData;
	if(len < 4) {
		return NULL;
	}
	code = ((unsigned int)pStartCode[0]<<24) | ((unsigned int)pStartCode[1]<<16) | ((unsigned int)pStartCode[2]<<8) | pStartCode[3];
	while((code&0xffffff1f) != 0x00000107) {
		len--;
		if(len < 4) {
			return NULL;
		}
		code = (code << 8)| pStartCode[4];
		pStartCode++;
	}
	return pStartCode;
}

static unsigned char*
seek_MPEG4VOLSC(unsigned char* pData, int len)
{
	unsigned int code;
	unsigned char* pStartCode = pData;
	if(len < 4) {
		return NULL;
	}
	code = ((unsigned int)pStartCode[0]<<24) | ((unsigned int)pStartCode[1]<<16) | ((unsigned int)pStartCode[2]<<8) | pStartCode[3];
	while((code&0xfffffff0) != 0x00000120) {
		len--;
		if(len < 4) {
			return NULL;
		}
		code = (code << 8)| pStartCode[4];
		pStartCode++;
	}
	return pStartCode;
}

static void
distinguish_h264avcC(GstIppCnmAssist* assist, unsigned char* data, int len)
{
	unsigned int AVCProfileIndication = data[1];
	GST_DEBUG_OBJECT(assist, "ippcnmassist found h264 profile %d in avcC", AVCProfileIndication);
	if(AVCProfileIndication == 66) {
		//baseline
		assist->bDistinguished = 1;
	}else{
		assist->bDistinguished = 2;
	}
	return;
}

static void
distinguish_h264ByteStream(GstIppCnmAssist* assist, unsigned char* data, int len)
{
	unsigned char* pSPS = seek_H264ByteStreamSPS(data, len);
	if(pSPS && len > 4 + (pSPS - data)) {
		unsigned int profile_idc = pSPS[4];
		if(profile_idc == 66) {
			assist->bDistinguished = 1;
		}else{
			assist->bDistinguished = 2;
		}
	}
	return;
}

static void
distinguish_MPEG4(GstIppCnmAssist* assist, unsigned char* data, int len)
{
	unsigned char* pVOL = seek_MPEG4VOLSC(data, len);
	if(pVOL && len > 5 + (pVOL - data)) {
		unsigned int video_object_type_indication = ((pVOL[4]<<1)|(pVOL[5]>>7)) & 0xff;
		if(video_object_type_indication == 1) {
			//simple object type
			assist->bDistinguished = 1;
		}else{
			assist->bDistinguished = 2;
		}
	}
	return;
}

#define VC1SPMP_PREFER_SWDEC_HANDLE_RESOLUTION		(352*288)
static void
distinguish_VC1SPMP(GstIppCnmAssist* assist, unsigned char* data, int len, int w, int h)
{
	int profile;
	assist->bDistinguished = 1;	//by default, let c&m decoder handle VC1 SPMP stream
	if(len < 4) {
		GST_DEBUG_OBJECT(assist, "codec_data len is too short %d, let decoder plug-in handle", len);
		return;
	}
	profile = data[0]>>4;
	GST_DEBUG_OBJECT(assist, "VC1 profile is %d, (0 is SP, 4 is MP)", profile);
	if(profile != 0 && profile != 4) {
		GST_DEBUG_OBJECT(assist, "profile %d isn't VC1 SP&MP, let decoder plug-in handle", profile);
		return;
	}
	if(profile == 4) {
		int maxbframes;
		maxbframes = (data[3]>>4)&0x7;
		GST_DEBUG_OBJECT(assist, "VC1 MP, MAXBFRAMES is %d, if it's 0, should no B frame, if it > 0, should no skip frame.", maxbframes);
		if(maxbframes > 0) {
			//if there is B frame, the stream probably contain directMV feature, which isn't supported well by c&m. If resolution is small, let SW decoder handle, if resolution is big, let c&m handle.
			if(w>0 && h>0 && w*h<=VC1SPMP_PREFER_SWDEC_HANDLE_RESOLUTION) {
				assist->bDistinguished = 2;
				GST_DEBUG_OBJECT(assist, "Because resolution(%d x %d) is small and has B frame(%d), let SW decoder to handle instead of c&m", w, h, maxbframes);
			}
		}
	}
	return;
}

static gboolean
gst_ippcnmassist_sink_setcaps(GstPad * pad, GstCaps * caps)
{
	assist_myecho("enter %s()\n", __FUNCTION__);
	GstIppCnmAssist* assist = GST_IPPCNMASSIST(GST_PAD_PARENT(pad));
	GstStructure* str = gst_caps_get_structure(caps, 0);
	const gchar* name = gst_structure_get_name(str);
	const GValue *value;
	
	if(0 == strcmp(name, "video/x-h264")) {
		assist->kind = 0;
	}else if(0 == strcmp(name, "video/mpeg") || 0 == strcmp(name, "video/x-xvid")){
		assist->kind = 1;
	}else{
		assist->kind = 2;
	}
	
	if((value = gst_structure_get_value(str, "codec_data"))) {
		GstBuffer* buf = gst_value_get_buffer(value);
		guint8* cdata = GST_BUFFER_DATA(buf);
		guint csize = GST_BUFFER_SIZE(buf);
		if(assist->kind == 0) {
			//h264
			if(csize<7) {	//SPS NAL >= 6 bytes
				GST_ERROR_OBJECT(assist, "h264 codec_data size %d too small", csize);
				return FALSE;
			}
			if(cdata[0] == 1 && csize >= 20) { //ISO 14496-15: sec 5.2.4.1, sec 5.3.4.1.2
				GST_DEBUG_OBJECT(assist, "the codec_data is avcC");
				distinguish_h264avcC(assist, cdata, csize);
			}else{
				GST_DEBUG_OBJECT(assist, "the codec_data isn't avcC, assume it's h264 bytestream");
				distinguish_h264ByteStream(assist, cdata, csize);
			}
		}else if(assist->kind == 1){
			//mpeg4
			distinguish_MPEG4(assist, cdata, csize);
		}else{
			//wmv vc1sp&mp
			int width = 0, height = 0;
			if( !gst_structure_get_int(str, "width", &width) ) {
				width = 0;
			}
			if( !gst_structure_get_int(str, "height", &height) ) {
				height = 0;
			}
			distinguish_VC1SPMP(assist, cdata, csize, width, height);
		}

	}
	assist->srcCap = gst_caps_copy(caps);
	gst_caps_set_simple(assist->srcCap, "ippcnmassistparsed", G_TYPE_BOOLEAN, TRUE, NULL);
	gst_caps_set_simple(assist->srcCap, "cnmnotsupport", G_TYPE_INT, 0, NULL);

	if(assist->bDistinguished == 2) {
		//add cnm not support indicator in cap
		gst_caps_set_simple(assist->srcCap, "cnmnotsupport", G_TYPE_INT, 1, NULL);
		GST_DEBUG_OBJECT(assist, "add cnmnotsupport to 1 in caps in %s()", __FUNCTION__);
	}

	return TRUE;
}

static void
fire_cached_events(GstIppCnmAssist* assist)
{
	if(assist->cached_events) {
		GList* oldest = g_list_last(assist->cached_events);
		while(oldest) {
			GST_DEBUG_OBJECT(assist, "cnm assist firing cached event: %s(0x%x)", GST_EVENT_TYPE_NAME(oldest->data), (unsigned int)(oldest->data));
			gst_pad_push_event(assist->srcpad, oldest->data);
			oldest = oldest->prev;
		}
		g_list_free(assist->cached_events);
		assist->cached_events = NULL;
	}
	return;
}


#define ELEMENT_IS_UNDECIDED			0
#define ELEMENT_IS_DEMUX_FLUTS			1
#define ELEMENT_IS_DEMUX_MPEGTS			2
#define ELEMENT_IS_COMMON		0xffffffff		//COMMON means the species of element is unknown. The element still probably is one of above, like ELEMENT_IS_DEMUX_MPEGTS.

#define FLUTSDEMUX_TYPENAME		"GstFluTSDemux"
#define MPEGTSDEMUX_TYPENAME	"GstMpegTSDemux"

#define DEMUXER_IS_FLUTS_MPEGTS(assist)	(query_nonqueue_upstreamelement_species(assist)==ELEMENT_IS_DEMUX_FLUTS || query_nonqueue_upstreamelement_species(assist)==ELEMENT_IS_DEMUX_MPEGTS)

static int
query_nonqueue_upstreamelement_species(GstIppCnmAssist* assist)
{
	if(assist->UpAdjacentNonQueueEle != ELEMENT_IS_UNDECIDED) {
		return assist->UpAdjacentNonQueueEle;
	}else{
		char* padname = "sink";
		GstElement* ele = GST_ELEMENT_CAST(assist);
		GstPad* pad;
		GstPad* peerPad;
		GstObject* peerEleObj;
		GstElement* peerEle;
		const gchar* peerEleTname = NULL;
		int eleIsQueue = 0;
		GstIterator *iter = NULL;

		do{
			if(eleIsQueue != 2) {
				pad = gst_element_get_static_pad(ele, padname);
				peerPad = gst_pad_get_peer(pad);
				gst_object_unref(pad);
			}else{
				iter = gst_element_iterate_sink_pads(ele);
				if(GST_ITERATOR_OK != gst_iterator_next(iter, (void**)(&pad))) {
					gst_iterator_free(iter);
					assist->UpAdjacentNonQueueEle = ELEMENT_IS_COMMON;
					return ELEMENT_IS_COMMON;
				}
				peerPad = gst_pad_get_peer(pad);
				gst_object_unref(pad);
				gst_iterator_free(iter);
			}
			peerEleObj = GST_OBJECT_PARENT(peerPad);
			if(! GST_IS_ELEMENT(peerEleObj)) {	//sometimes, the peerPad's parent isn't an element, it may be ghostpad or something else.
				gst_object_unref(peerPad);
				assist->UpAdjacentNonQueueEle = ELEMENT_IS_COMMON;
				return ELEMENT_IS_COMMON;
			}
			peerEle = GST_ELEMENT(peerEleObj);
			gst_object_unref(peerPad);
			peerEleTname = G_OBJECT_TYPE_NAME(peerEle);
			ele = peerEle;
			if(strcmp(peerEleTname, "GstQueue") == 0) {
				eleIsQueue = 1;
			}else if(strcmp(peerEleTname, "GstMultiQueue") == 0) {
				eleIsQueue = 2;
			}else{
				eleIsQueue = 0;
			}
			assist_myecho("meet %s\n", peerEleTname == NULL ? "NULL" : peerEleTname);
		}while(eleIsQueue != 0);

		if(strcmp(peerEleTname, FLUTSDEMUX_TYPENAME)==0) {
			assist->UpAdjacentNonQueueEle = ELEMENT_IS_DEMUX_FLUTS;
		}else if(strcmp(peerEleTname, MPEGTSDEMUX_TYPENAME)==0) {
			assist->UpAdjacentNonQueueEle = ELEMENT_IS_DEMUX_MPEGTS;
		}else{
			assist->UpAdjacentNonQueueEle = ELEMENT_IS_COMMON;
		}
		assist_myecho("Adjacent element at upstream is %s(id %d).\n", peerEleTname, assist->UpAdjacentNonQueueEle);
		return assist->UpAdjacentNonQueueEle;
	}
}

static GstBuffer*
consume_ts_stream(GstIppCnmAssist* assist, GstBuffer* buf)
{
    // TS, we may need to bufferring data
    if(GST_CLOCK_TIME_NONE == GST_BUFFER_TIMESTAMP(buf)) {
		GST_LOG_OBJECT(assist, "timestamp is GST_CLOCK_TIME_NONE for TS file, it's not frame begin, buffer it");
		if(NULL==assist->AccumulatedBuf){
			//only bufferring data after AccumulatedBuf has accumulated frame begin
			gst_buffer_unref(buf);
		}else{
			assist->AccumulatedBuf = gst_buffer_join(assist->AccumulatedBuf, buf);
		}
		return NULL;
    }else{
		GST_LOG_OBJECT(assist, "timestamp isn't NONE for TS file, New frame begin");
		// first get the bufferred last frame if have any
		// then bufferring for the new frame
		GstBuffer* lastframe = assist->AccumulatedBuf;
		assist->AccumulatedBuf = buf;
		return lastframe;
	}
}

static GstFlowReturn
gst_ippcnmassist_chain(GstPad *pad, GstBuffer *buf)
{
	GstIppCnmAssist* assist = GST_IPPCNMASSIST(GST_PAD_PARENT(pad));
	guint8* pData = GST_BUFFER_DATA(buf);
	guint len = GST_BUFFER_SIZE(buf);
	if(assist->bDistinguished == 0) {
		assist_myecho("Distinguish stream in %s()\n", __FUNCTION__);
		if(assist->kind == 0) {
			distinguish_h264ByteStream(assist, pData, len);
		}else if(assist->kind == 1){
			distinguish_MPEG4(assist, pData, len);
		}else{
			distinguish_VC1SPMP(assist, NULL, 0, 0, 0);
		}
		if(assist->bDistinguished == 2) {
			//add cnm not support indicator in cap
			gst_caps_set_simple(assist->srcCap, "cnmnotsupport", G_TYPE_INT, 1, NULL);
			GST_DEBUG_OBJECT(assist, "add cnmnotsupport to 1 in caps in %s()", __FUNCTION__);
		}
	}
	
	if(assist->bDistinguished == 0) {
		gst_buffer_unref(buf);
		GST_DEBUG_OBJECT(assist, "Give up input stream data, because not contain h264 SPS/mpeg4 VOL\n");
		return GST_FLOW_OK;
	}

	if(assist->bSrcPadAddedCap == 0) {
		GST_DEBUG_OBJECT(assist, "setting cap on srcpad in %s().\n", __FUNCTION__);
		gst_pad_set_caps(assist->srcpad, assist->srcCap);
		gst_caps_unref(assist->srcCap);
		assist->bSrcPadAddedCap = 1;
		fire_cached_events(assist);
	}

	if(DEMUXER_IS_FLUTS_MPEGTS(assist)) {
		buf = consume_ts_stream(assist, buf);
	}

	if(buf) {
		gst_buffer_set_caps(buf, GST_PAD_CAPS(assist->srcpad));
		return gst_pad_push(assist->srcpad, buf);
	}
	return GST_FLOW_OK;
}

static GstEvent*
pickup_latest_newseg(GstIppCnmAssist* assist)
{
	//found latest newseg
	GstEvent* latestNewseg = NULL;
	GList* pNext = assist->cached_events;
	while(pNext) {
		if(GST_EVENT_TYPE(pNext->data) == GST_EVENT_NEWSEGMENT) {
			break;
		}
		pNext = pNext->next;
	}
	if(pNext) {
		latestNewseg = pNext->data;
		assist->cached_events = g_list_delete_link(assist->cached_events, pNext);
	}
	return latestNewseg;
}

static __inline void
clean_accumulatedbuf(GstIppCnmAssist* assist)
{
	if(assist->AccumulatedBuf) {
		gst_buffer_unref(assist->AccumulatedBuf);
		assist->AccumulatedBuf = NULL;
	}
	return;
}

static gboolean 
gst_ippcnmassist_sinkpad_event(GstPad* pad, GstEvent* event)
{
	GstIppCnmAssist* assist = GST_IPPCNMASSIST(GST_PAD_PARENT(pad));

	GST_DEBUG_OBJECT(assist, "receive event: %s(0x%x) on cnm assist sinkpad", GST_EVENT_TYPE_NAME(event), (unsigned int)event);
	gboolean eventRet = TRUE;

	if(GST_EVENT_TYPE(event)==GST_EVENT_NEWSEGMENT || GST_EVENT_TYPE(event)==GST_EVENT_FLUSH_STOP) {
		clean_accumulatedbuf(assist);
	}

	//partially refer to the code gst-plugins-bad-0.10.21/gst/mpeg4videoparse

	if(assist->bSrcPadAddedCap) {
		if(GST_EVENT_TYPE(event) == GST_EVENT_EOS) {
			//flush the accumulated frame
			if(assist->AccumulatedBuf) {
				GstBuffer* buf = assist->AccumulatedBuf;
				assist->AccumulatedBuf = NULL;
				gst_buffer_set_caps(buf, GST_PAD_CAPS(assist->srcpad));
				gst_pad_push(assist->srcpad, buf);
			}
		}
		eventRet = gst_pad_event_default(pad, event);
	}else{
		switch(GST_EVENT_TYPE(event)) {
		case GST_EVENT_EOS:
			{
				GST_DEBUG_OBJECT(assist, "cnm assist receive EOS before srcpad caps set");
				GstEvent* newseg = pickup_latest_newseg(assist);
				if(newseg) {
					gst_pad_push_event(assist->srcpad, newseg);
				}
				eventRet = gst_pad_push_event(assist->srcpad, event);
			}
			break;
		default:
			GST_DEBUG_OBJECT(assist, "cnm assist caching event: %s(0x%x)", GST_EVENT_TYPE_NAME(event), (unsigned int)event);
			assist->cached_events = g_list_prepend(assist->cached_events, event);
			break;
		}
	}

	return eventRet;
}

static void ippcnmgst_event_unref(GstEvent* event)
{
	gst_event_unref(event);		//in some older version GST, gst_event_unref is defined as macro, therefore, define a function to contain it.
	return;
}

static void
gst_ippcnmassist_cleanup_events(GstIppCnmAssist* assist)
{
	if(assist->cached_events) {
		g_list_foreach(assist->cached_events, (GFunc)ippcnmgst_event_unref, NULL);
		g_list_free(assist->cached_events);
		assist->cached_events = NULL;
	}
	return;
}

static GstStateChangeReturn
gst_ippcnmassist_change_state(GstElement * element, GstStateChange transition)
{
	GstStateChangeReturn ret;
	GstIppCnmAssist* assist = GST_IPPCNMASSIST(element);

	GST_DEBUG_OBJECT(assist, "cnm assist state change from %d to %d", transition>>3, transition&7);

	ret = GST_ELEMENT_CLASS(parent_class)->change_state(element, transition);

	switch (transition) {
	case GST_STATE_CHANGE_PAUSED_TO_READY:
		gst_ippcnmassist_cleanup_events(assist);
		clean_accumulatedbuf(assist);
		break;
	default:
		break;
	}
	return ret;
}

static void
gst_ippcnmassist_base_init(gpointer klass)
{
	static GstElementDetails details = {
		"IPP CNM Assist",
		"Codec/Parser/Video",
		"IPP CNM Decoder Assistant Plug-in",
		""
	};

	GstElementClass *element_class = GST_ELEMENT_CLASS (klass);

	gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&src_factory));
	gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&sink_factory));

	gst_element_class_set_details(element_class, &details);

	GST_DEBUG_CATEGORY_INIT(ippcnmassist_debug, "ippcnmassist", 0, "IPP CNM Assistant Element");	//after this, GST_DEBUG(), GST_DEBUG_OBJECT() and ... could work

	assist_myecho("ippcnmassist base inited\n");
	return;
}


static void
gst_ippcnmassist_class_init(GstIppCnmAssistClass *klass)
{
	//GObjectClass *gobject_class  = (GObjectClass*) klass;
	GstElementClass *gstelement_class = (GstElementClass*) klass;

	//parent_class = g_type_class_peek_parent(klass);	//parent_class has been declared in GST_BOILERPLATE, and this opration has been done in GST_BOILERPLATE_FULL

	gstelement_class->change_state = GST_DEBUG_FUNCPTR(gst_ippcnmassist_change_state);

	assist_myecho("ippcnmassist class inited\n");
	return;
}


static void
gst_ippcnmassist_init(GstIppCnmAssist* assist, GstIppCnmAssistClass* klass)
{
	assist->sinkpad = gst_pad_new_from_template (
		gst_element_class_get_pad_template ((GstElementClass*)klass, "sink"), "sink");


	gst_pad_set_setcaps_function(assist->sinkpad, GST_DEBUG_FUNCPTR (gst_ippcnmassist_sink_setcaps));
	gst_pad_set_chain_function(assist->sinkpad, GST_DEBUG_FUNCPTR (gst_ippcnmassist_chain));
	gst_pad_set_event_function(assist->sinkpad, GST_DEBUG_FUNCPTR (gst_ippcnmassist_sinkpad_event));

	assist->srcpad = gst_pad_new_from_template (
		gst_element_class_get_pad_template ((GstElementClass*)klass, "src"), "src");

	gst_element_add_pad (GST_ELEMENT(assist), assist->sinkpad);
	gst_pad_use_fixed_caps(assist->srcpad);
	gst_element_add_pad (GST_ELEMENT(assist), assist->srcpad);
	
	assist->kind = -1;
	assist->bDistinguished = 0;
	assist->srcCap = NULL;
	assist->bSrcPadAddedCap = 0;
	assist->cached_events = NULL;

	assist->UpAdjacentNonQueueEle = ELEMENT_IS_UNDECIDED;
	assist->AccumulatedBuf = NULL;

	assist_myecho("ippcnmassist instance(0x%x) inited\n",(unsigned int)assist);
	return;
}

static gboolean
plugin_init(GstPlugin *plugin)
{
	assist_myecho("ippcnmassist plugin inited.\n");
	return gst_element_register(plugin, "ippcnmassist", GST_RANK_PRIMARY+5, GST_TYPE_IPPCNMASSIST);	//cnmdec_xxx rank are GST_RANK_PRIMARY+3, cnmassist should higher than it.
}

GST_PLUGIN_DEFINE(	GST_VERSION_MAJOR,
					GST_VERSION_MINOR,
					"mvl_ippcnmassist",
					"IPP CNM video decoder assistant, "__DATE__,
					plugin_init,
					VERSION,
					"LGPL",
					"",
					"");

/* EOF */
