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
#include "config.h"
#endif

#include "gstcnmenc_mpeg4.h"

GST_DEBUG_CATEGORY_STATIC (cnmenc_mpeg4_debug);
#define GST_CAT_DEFAULT cnmenc_mpeg4_debug

#define IPPGSTCNMENC_INITQP_MPEG4			15

enum {
	ARG_CNMENC_COMPROP_NUM = CNMENC_COMPROP_NUM,
	/* fill below with user defined arguments */
	ARG_MPEG4_DATA_PARTITION,
	ARG_MPEG4_RVLC,
	ARG_MPEG4_IDCVLCTHR,
	ARG_MPEG4_HEC,
	ARG_MPEG4_VERID,
	ARG_MPEG4_INSERTSEQHDR,
};

static GstStaticPadTemplate sink_factory =
	GST_STATIC_PAD_TEMPLATE ("sink", GST_PAD_SINK, GST_PAD_ALWAYS,
	GST_STATIC_CAPS ("video/x-raw-yuv, format = (fourcc) I420, width = (int) [ 48, 720 ], height = (int) [ 32, 720 ], framerate = (fraction) [ 1, 100 ]")
					 );
static GstStaticPadTemplate src_factory =
	GST_STATIC_PAD_TEMPLATE ("src", GST_PAD_SRC, GST_PAD_ALWAYS,
	GST_STATIC_CAPS ("video/mpeg, mpegversion = (int) 4, systemstream = (boolean) FALSE, width = (int) [ 48, 720 ], height = (int) [ 32, 720 ], framerate = (fraction) [ 1, 100 ]")
					 );


GST_BOILERPLATE (Gstcnmmpeg4enc, gst_cnmmpeg4enc, GstElement, GST_TYPE_ELEMENT);

void gst_cnmmpeg4enc_echofun(void* mpeg4, GstDebugLevel level, const char* format, ...)
{
	if(G_LIKELY(level >= GST_LEVEL_ERROR && level <= GST_LEVEL_LOG)) {
		char msg[256];
		va_list args;
		msg[sizeof(msg)-1] = '\0';
		va_start(args, format);
		vsnprintf(msg, sizeof(msg)-1, format, args);
		va_end(args);

		switch(level) {
		case GST_LEVEL_ERROR:
			GST_ERROR_OBJECT(mpeg4, msg);
			break;
		case GST_LEVEL_WARNING:
			GST_WARNING_OBJECT(mpeg4, msg);
			break;
		case GST_LEVEL_INFO:
			GST_INFO_OBJECT(mpeg4, msg);
			break;
		case GST_LEVEL_DEBUG:
			GST_DEBUG_OBJECT(mpeg4, msg);
			break;
		case GST_LEVEL_LOG:
			GST_LOG_OBJECT(mpeg4, msg);
			break;
		/*
		default:
			break;
		*/
		}
	}
	return;
}

static gboolean
gst_cnmmpeg4enc_sinkpad_setcaps(GstPad *pad, GstCaps *caps)
{
	Gstcnmmpeg4enc* mpeg4 = GST_CNMMPEG4ENC(GST_PAD_PARENT(pad));
	return _cnmenccom_sinkpad_setcaps(pad, caps, &mpeg4->comobj);
}

static GstFlowReturn
gst_cnmmpeg4enc_chain(GstPad *pad, GstBuffer *buf)
{
	Gstcnmmpeg4enc* mpeg4 = GST_CNMMPEG4ENC(GST_PAD_PARENT(pad));
	int bTerminate = 0, iErrReason = 0;
	GstFlowReturn flowRt = _cnmenccom_chain(buf, &mpeg4->comobj, &bTerminate, &iErrReason);
	if(bTerminate) {
		GST_ELEMENT_ERROR(mpeg4, STREAM, ENCODE, (NULL), ("%s() will return un-accepted flow return code %d, error reason %d", __FUNCTION__, flowRt, iErrReason));
	}
	return flowRt;
}

static GstStateChangeReturn
gst_cnmmpeg4enc_change_state(GstElement *element, GstStateChange transition)
{
	Gstcnmmpeg4enc* mpeg4 = GST_CNMMPEG4ENC(element);
	return _cnmenccom_change_state(element, transition, &mpeg4->comobj, parent_class);
}

static void
gst_cnmmpeg4enc_set_property(GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	Gstcnmmpeg4enc* mpeg4 = GST_CNMMPEG4ENC(object);
	IppGSTCnMEncCOM* cnmcom = &mpeg4->comobj;
	int rt = _cnmenccom_set_com_prop(object, prop_id, value, pspec, cnmcom);
	if(rt < 0) {
		switch(prop_id) {
		case ARG_MPEG4_DATA_PARTITION:
			{
				int iVal = g_value_get_int(value);
				if( (iVal!=0) && (iVal!=1) ) {
					GST_ERROR_OBJECT(mpeg4, "mpeg4-datapartition %d exceed range.", iVal);
					iVal= 0;
				}
				cnmcom->Prop_mpeg4Par.mp4_dataPartitionEnable = iVal;
			}
			break;
		case ARG_MPEG4_RVLC:
			{
				int iVal = g_value_get_int(value);
				if( (iVal!=0) && (iVal!=1) ) {
					GST_ERROR_OBJECT(mpeg4, "mpeg4-reversevlc %d exceed range.", iVal);
					iVal= 0;
				}
				cnmcom->Prop_mpeg4Par.mp4_reversibleVlcEnable = iVal;
			}
			break;
		case ARG_MPEG4_IDCVLCTHR:
			{
				int iVal = g_value_get_int(value);
				if( (iVal<0) || (iVal>7) ) {
					GST_ERROR_OBJECT(mpeg4, "mpeg4-intradcvlcthr %d exceed range.", iVal);
					iVal= 0;
				}
				cnmcom->Prop_mpeg4Par.mp4_intraDcVlcThr = iVal;
			}
			break;
		case ARG_MPEG4_HEC:
			{
				int iVal = g_value_get_int(value);
				if( (iVal!=0) && (iVal!=1) ) {
					GST_ERROR_OBJECT(mpeg4, "mpeg4-hec %d exceed range.", iVal);
					iVal= 0;
				}
				cnmcom->Prop_mpeg4Par.mp4_hecEnable = iVal;
			}
			break;
		case ARG_MPEG4_VERID:
			{
				int iVal = g_value_get_int(value);
				if( (iVal!=1) && (iVal!=2) ) {
					GST_ERROR_OBJECT(mpeg4, "mpeg4-vol_verid %d exceed range.", iVal);
					iVal= 1;
				}
				cnmcom->Prop_mpeg4Par.mp4_verid = iVal;
			}
			break;
		case ARG_MPEG4_INSERTSEQHDR:
			{
				int iVal = g_value_get_int(value);
				if( (iVal!=0) && (iVal!=1) ) {
					GST_ERROR_OBJECT(mpeg4, "insertSeqHdr %d exceed range.", iVal);
					iVal= 1;
				}
				cnmcom->Prop_bInsertSeqHdr = iVal;
			}
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
		}
	}
	return;
}

static void
gst_cnmmpeg4enc_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	Gstcnmmpeg4enc* mpeg4 = GST_CNMMPEG4ENC(object);
	IppGSTCnMEncCOM* cnmcom = &mpeg4->comobj;
	int rt = _cnmenccom_get_prop(object, prop_id, value, pspec, cnmcom);
	if(rt < 0) {
		switch(prop_id) {
		case ARG_MPEG4_DATA_PARTITION:
			g_value_set_int(value, cnmcom->Prop_mpeg4Par.mp4_dataPartitionEnable);
			break;
		case ARG_MPEG4_RVLC:
			g_value_set_int(value, cnmcom->Prop_mpeg4Par.mp4_reversibleVlcEnable);
			break;
		case ARG_MPEG4_IDCVLCTHR:
			g_value_set_int(value, cnmcom->Prop_mpeg4Par.mp4_intraDcVlcThr);
			break;
		case ARG_MPEG4_HEC:
			g_value_set_int(value, cnmcom->Prop_mpeg4Par.mp4_hecEnable);
			break;
		case ARG_MPEG4_VERID:
			g_value_set_int(value, cnmcom->Prop_mpeg4Par.mp4_verid);
			break;
		case ARG_MPEG4_INSERTSEQHDR:
			g_value_set_int(value, cnmcom->Prop_bInsertSeqHdr);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
		}
	}
	return;
}


static void
gst_cnmmpeg4enc_finalize(GObject * object)
{
	GST_DEBUG_OBJECT(object, "Gstcnmmpeg4enc instance %p will be finalized!!!", object);
	G_OBJECT_CLASS(parent_class)->finalize(object);

	printf("Gstcnmmpeg4enc instance %p is finalized!!!\n", object);

	return;
}


static void
gst_cnmmpeg4enc_base_init(gpointer klass)
{
	static GstElementDetails enc_details = {
		"IPPCNM encoder mpeg4",
		"Codec/Encoder/Video",
		"HARDWARE Encoder based-on IPP C&M CODEC for mpeg4",
		""
	};

	GstElementClass *element_class = GST_ELEMENT_CLASS(klass);

	gst_element_class_add_pad_template(element_class, gst_static_pad_template_get(&src_factory));
	gst_element_class_add_pad_template(element_class, gst_static_pad_template_get(&sink_factory));
	gst_element_class_set_details(element_class, &enc_details);

	return;
}

static void
gst_cnmmpeg4enc_class_init(Gstcnmmpeg4encClass* klass)
{
	GObjectClass* gobject_class  = (GObjectClass*)klass;
	GstElementClass* gstelement_class = (GstElementClass*)klass;

	gobject_class->set_property = gst_cnmmpeg4enc_set_property;
	gobject_class->get_property = gst_cnmmpeg4enc_get_property;

	_cnmenccom_class_install_com_prop(gobject_class);
	g_object_class_install_property (gobject_class, ARG_MPEG4_DATA_PARTITION, \
		g_param_spec_int ("mpeg4-datapartition", "enable data partition when format is mpeg4", \
		"Enable data partition when format is mpeg4, 0<disable>, 1<enable>.", \
		0/* range_MIN */, 1/* range_MAX */, 0/* default_INIT */, G_PARAM_READWRITE));
	g_object_class_install_property (gobject_class, ARG_MPEG4_RVLC, \
		g_param_spec_int ("mpeg4-reversevlc", "enable reverse VLC encoding when format is mpeg4", \
		"Enable reverse VLC encoding when format is mpeg4, 0<disable>, 1<enable>.", \
		0/* range_MIN */, 1/* range_MAX */, 0/* default_INIT */, G_PARAM_READWRITE));
	g_object_class_install_property (gobject_class, ARG_MPEG4_IDCVLCTHR, \
		g_param_spec_int ("mpeg4-intradcvlcthr", "set intra dc vlc threshold when format is mpeg4", \
		"Set intra dc vlc threshold when format is mpeg4.", \
		0/* range_MIN */, 7/* range_MAX */, 0/* default_INIT */, G_PARAM_READWRITE));
	g_object_class_install_property (gobject_class, ARG_MPEG4_HEC, \
		g_param_spec_int ("mpeg4-hec", "enable hec when format is mpeg4", \
		"Enable Header Extension Code(HEC) option when format is mpeg4, 0<disable>, 1<enable>.", \
		0/* range_MIN */, 1/* range_MAX */, 0/* default_INIT */, G_PARAM_READWRITE));
	g_object_class_install_property (gobject_class, ARG_MPEG4_VERID, \
		g_param_spec_int ("mpeg4-vol_verid", "set vol_verid when format is mpeg4", \
		"Set video_object_layer_verid when format is mpeg4.", \
		1/* range_MIN */, 2/* range_MAX */, 1/* default_INIT */, G_PARAM_READWRITE));
	g_object_class_install_property (gobject_class, ARG_MPEG4_INSERTSEQHDR, \
		g_param_spec_int ("insertSeqHdr", "insert VOS/VO/VO/VOL before each I frame", \
		"Enable inserting VisualOS/VisualO/VideoO/VideoOL before each I frame, 0<disable>, 1<enable>.", \
		0/* range_MIN */, 1/* range_MAX */, 1/* default_INIT */, G_PARAM_READWRITE));

	gobject_class->finalize = gst_cnmmpeg4enc_finalize;
	gstelement_class->change_state = GST_DEBUG_FUNCPTR(gst_cnmmpeg4enc_change_state);

	GST_DEBUG_CATEGORY_INIT(cnmenc_mpeg4_debug, "cnmenc_mpeg4", 0, "c&m Encoder mpeg4 Element");

	return;
}


static GstFlowReturn
gst_cnmmpeg4enc_sinkpad_allocbuf(GstPad *pad, guint64 offset, guint size, GstCaps *caps, GstBuffer **buf)
{
	gcnm_printfmore("%s() is called, pad(%p), offset(%lld), size(%d)\n", __FUNCTION__, pad, offset, size);
	Gstcnmmpeg4enc* mpeg4 = GST_CNMMPEG4ENC(GST_PAD_PARENT(pad));
	return _cnmenccom_sinkpad_allocbuf(pad, offset, size, caps, buf, &mpeg4->comobj);
}


static gboolean
gst_cnmmpeg4enc_sinkpad_event(GstPad* pad, GstEvent* event)
{
	Gstcnmmpeg4enc* mpeg4 = GST_CNMMPEG4ENC(GST_PAD_PARENT(pad));
	return _cnmenccom_sinkpad_event(pad, event, &mpeg4->comobj);
}

static void
gst_cnmmpeg4enc_init(Gstcnmmpeg4enc* mpeg4, Gstcnmmpeg4encClass* enc_klass)
{
	GstElementClass *klass = GST_ELEMENT_CLASS(enc_klass);

	mpeg4->sinkpad = gst_pad_new_from_template(gst_element_class_get_pad_template(klass, "sink"), "sink");

	gst_pad_set_setcaps_function(mpeg4->sinkpad, GST_DEBUG_FUNCPTR (gst_cnmmpeg4enc_sinkpad_setcaps));
	gst_pad_set_chain_function(mpeg4->sinkpad, GST_DEBUG_FUNCPTR (gst_cnmmpeg4enc_chain));
	gst_pad_set_event_function(mpeg4->sinkpad, GST_DEBUG_FUNCPTR (gst_cnmmpeg4enc_sinkpad_event));
	gst_pad_set_bufferalloc_function(mpeg4->sinkpad, GST_DEBUG_FUNCPTR (gst_cnmmpeg4enc_sinkpad_allocbuf));

	gst_element_add_pad(GST_ELEMENT(mpeg4), mpeg4->sinkpad);

	mpeg4->srcpad = gst_pad_new_from_template(gst_element_class_get_pad_template(klass, "src"), "src");

	gst_element_add_pad(GST_ELEMENT(mpeg4), mpeg4->srcpad);

	_cnmenccom_init_members(&mpeg4->comobj, IPPGSTCNMENC_STMFORMAT_MPEG4, IPPGSTCNMENC_INITQP_MPEG4, mpeg4, gst_cnmmpeg4enc_echofun, mpeg4->sinkpad, mpeg4->srcpad);
	mpeg4->comobj.Prop_bInsertSeqHdr = 1;

	GST_DEBUG_OBJECT(mpeg4, "Gstcnmmpeg4enc instance %p is inited!!!\n", mpeg4);
	printf("Gstcnmmpeg4enc instance %p is inited!!!\n", mpeg4);

	return;
}
