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

#include "gstcnmenc_h263.h"

GST_DEBUG_CATEGORY_STATIC (cnmenc_h263_debug);
#define GST_CAT_DEFAULT cnmenc_h263_debug

#define IPPGSTCNMENC_INITQP_H263			15
enum {
	ARG_CNMENC_COMPROP_NUM = CNMENC_COMPROP_NUM,
	/* fill below with user defined arguments */
	ARG_H263_ANNEXJ,
	ARG_H263_ANNEXK,
	ARG_H263_ANNEXT,
};

static GstStaticPadTemplate sink_factory =
	GST_STATIC_PAD_TEMPLATE ("sink", GST_PAD_SINK, GST_PAD_ALWAYS,
	GST_STATIC_CAPS ("video/x-raw-yuv, format = (fourcc) I420, width = (int) [ 48, 704 ], height = (int) [ 32, 576 ], framerate = (fraction) [ 1, 100 ]")
					 );
static GstStaticPadTemplate src_factory =
	GST_STATIC_PAD_TEMPLATE ("src", GST_PAD_SRC, GST_PAD_ALWAYS,
	GST_STATIC_CAPS ("video/x-h263, width = (int) [ 48, 704 ], height = (int) [ 32, 576 ], framerate = (fraction) [ 1, 100 ]")
					 );


GST_BOILERPLATE (Gstcnmh263enc, gst_cnmh263enc, GstElement, GST_TYPE_ELEMENT);

void gst_cnmh263enc_echofun(void* h263, GstDebugLevel level, const char* format, ...)
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
			GST_ERROR_OBJECT(h263, msg);
			break;
		case GST_LEVEL_WARNING:
			GST_WARNING_OBJECT(h263, msg);
			break;
		case GST_LEVEL_INFO:
			GST_INFO_OBJECT(h263, msg);
			break;
		case GST_LEVEL_DEBUG:
			GST_DEBUG_OBJECT(h263, msg);
			break;
		case GST_LEVEL_LOG:
			GST_LOG_OBJECT(h263, msg);
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
gst_cnmh263enc_sinkpad_setcaps(GstPad *pad, GstCaps *caps)
{
	Gstcnmh263enc* h263 = GST_CNMH263ENC(GST_PAD_PARENT(pad));
	return _cnmenccom_sinkpad_setcaps(pad, caps, &h263->comobj);
}

static GstFlowReturn
gst_cnmh263enc_chain(GstPad *pad, GstBuffer *buf)
{
	Gstcnmh263enc* h263 = GST_CNMH263ENC(GST_PAD_PARENT(pad));
	int bTerminate = 0, iErrReason = 0;
	GstFlowReturn flowRt = _cnmenccom_chain(buf, &h263->comobj, &bTerminate, &iErrReason);
	if(bTerminate) {
		GST_ELEMENT_ERROR(h263, STREAM, ENCODE, (NULL), ("%s() will return un-accepted flow return code %d, error reason %d", __FUNCTION__, flowRt, iErrReason));
	}
	return flowRt;
}

static GstStateChangeReturn
gst_cnmh263enc_change_state(GstElement *element, GstStateChange transition)
{
	Gstcnmh263enc* h263 = GST_CNMH263ENC(element);
	return _cnmenccom_change_state(element, transition, &h263->comobj, parent_class);
}

static void
gst_cnmh263enc_set_property(GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	Gstcnmh263enc* h263 = GST_CNMH263ENC(object);
	IppGSTCnMEncCOM* cnmcom = &h263->comobj;
	int rt = _cnmenccom_set_com_prop(object, prop_id, value, pspec, cnmcom);
	if(rt < 0) {
		switch(prop_id) {
		case ARG_H263_ANNEXJ:
			{
				int iVal = g_value_get_int(value);
				if( (iVal!=0) && (iVal!=1) ) {
					GST_ERROR_OBJECT(263, "h263-annex-j %d exceed range.", iVal);
					iVal= 0;
				}
				cnmcom->Prop_h263Par.h263_annexJEnable = iVal;
			}
			break;
		case ARG_H263_ANNEXK:
			{
				int iVal = g_value_get_int(value);
				if( (iVal!=0) && (iVal!=1) ) {
					GST_ERROR_OBJECT(263, "h263-annex-k %d exceed range.", iVal);
					iVal= 0;
				}
				cnmcom->Prop_h263Par.h263_annexKEnable = iVal;
			}
			break;
		case ARG_H263_ANNEXT:
			{
				int iVal = g_value_get_int(value);
				if( (iVal!=0) && (iVal!=1) ) {
					GST_ERROR_OBJECT(263, "h263-annex-t %d exceed range.", iVal);
					iVal= 0;
				}
				cnmcom->Prop_h263Par.h263_annexTEnable = iVal;
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
gst_cnmh263enc_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	Gstcnmh263enc* h263 = GST_CNMH263ENC(object);
	IppGSTCnMEncCOM* cnmcom = &h263->comobj;
	int rt = _cnmenccom_get_prop(object, prop_id, value, pspec, cnmcom);
	if(rt < 0) {
		switch(prop_id) {
		case ARG_H263_ANNEXJ:
			g_value_set_int(value, cnmcom->Prop_h263Par.h263_annexJEnable);
			break;
		case ARG_H263_ANNEXK:
			g_value_set_int(value, cnmcom->Prop_h263Par.h263_annexKEnable);
			break;
		case ARG_H263_ANNEXT:
			g_value_set_int(value, cnmcom->Prop_h263Par.h263_annexTEnable);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
			break;
		}
	}
	return;
}


static void
gst_cnmh263enc_finalize(GObject * object)
{
	GST_DEBUG_OBJECT(object, "Gstcnmh263enc instance %p will be finalized!!!", object);
	G_OBJECT_CLASS(parent_class)->finalize(object);

	printf("Gstcnmh263enc instance %p is finalized!!!\n", object);

	return;
}


static void
gst_cnmh263enc_base_init(gpointer klass)
{
	static GstElementDetails enc_details = {
		"IPPCNM encoder h263",
		"Codec/Encoder/Video",
		"HARDWARE Encoder based-on IPP C&M CODEC for h263",
		""
	};

	GstElementClass *element_class = GST_ELEMENT_CLASS(klass);

	gst_element_class_add_pad_template(element_class, gst_static_pad_template_get(&src_factory));
	gst_element_class_add_pad_template(element_class, gst_static_pad_template_get(&sink_factory));
	gst_element_class_set_details(element_class, &enc_details);

	return;
}

static void
gst_cnmh263enc_class_init(Gstcnmh263encClass* klass)
{
	GObjectClass* gobject_class  = (GObjectClass*)klass;
	GstElementClass* gstelement_class = (GstElementClass*)klass;

	gobject_class->set_property = gst_cnmh263enc_set_property;
	gobject_class->get_property = gst_cnmh263enc_get_property;

	_cnmenccom_class_install_com_prop(gobject_class);
	g_object_class_install_property (gobject_class, ARG_H263_ANNEXJ, \
		g_param_spec_int ("h263-annex-j", "enable Annex J when format is h263", \
		"Enable Annex J when format is h263, 0<disable>, 1<enable>.", \
		0/* range_MIN */, 1/* range_MAX */, 0/* default_INIT */, G_PARAM_READWRITE));
	g_object_class_install_property (gobject_class, ARG_H263_ANNEXK, \
		g_param_spec_int ("h263-annex-k", "enable Annex K when format is h263", \
		"Enable Annex K when format is h263, 0<disable>, 1<enable>.", \
		0/* range_MIN */, 1/* range_MAX */, 0/* default_INIT */, G_PARAM_READWRITE));
	g_object_class_install_property (gobject_class, ARG_H263_ANNEXT, \
		g_param_spec_int ("h263-annex-t", "enable Annex I when format is h263", \
		"Enable Annex T when format is h263, 0<disable>, 1<enable>.", \
		0/* range_MIN */, 1/* range_MAX */, 0/* default_INIT */, G_PARAM_READWRITE));

	gobject_class->finalize = gst_cnmh263enc_finalize;
	gstelement_class->change_state = GST_DEBUG_FUNCPTR(gst_cnmh263enc_change_state);

	GST_DEBUG_CATEGORY_INIT(cnmenc_h263_debug, "cnmenc_h263", 0, "c&m Encoder h263 Element");

	return;
}


static GstFlowReturn
gst_cnmh263enc_sinkpad_allocbuf(GstPad *pad, guint64 offset, guint size, GstCaps *caps, GstBuffer **buf)
{
	gcnm_printfmore("%s() is called, pad(%p), offset(%lld), size(%d)\n", __FUNCTION__, pad, offset, size);
	Gstcnmh263enc* h263 = GST_CNMH263ENC(GST_PAD_PARENT(pad));
	return _cnmenccom_sinkpad_allocbuf(pad, offset, size, caps, buf, &h263->comobj);
}


static gboolean
gst_cnmh263enc_sinkpad_event(GstPad* pad, GstEvent* event)
{
	Gstcnmh263enc* h263 = GST_CNMH263ENC(GST_PAD_PARENT(pad));
	return _cnmenccom_sinkpad_event(pad, event, &h263->comobj);
}

static void
gst_cnmh263enc_init(Gstcnmh263enc* h263, Gstcnmh263encClass* enc_klass)
{
	GstElementClass *klass = GST_ELEMENT_CLASS(enc_klass);

	h263->sinkpad = gst_pad_new_from_template(gst_element_class_get_pad_template(klass, "sink"), "sink");

	gst_pad_set_setcaps_function(h263->sinkpad, GST_DEBUG_FUNCPTR (gst_cnmh263enc_sinkpad_setcaps));
	gst_pad_set_chain_function(h263->sinkpad, GST_DEBUG_FUNCPTR (gst_cnmh263enc_chain));
	gst_pad_set_event_function(h263->sinkpad, GST_DEBUG_FUNCPTR (gst_cnmh263enc_sinkpad_event));
	gst_pad_set_bufferalloc_function(h263->sinkpad, GST_DEBUG_FUNCPTR (gst_cnmh263enc_sinkpad_allocbuf));

	gst_element_add_pad(GST_ELEMENT(h263), h263->sinkpad);

	h263->srcpad = gst_pad_new_from_template(gst_element_class_get_pad_template(klass, "src"), "src");

	gst_element_add_pad(GST_ELEMENT(h263), h263->srcpad);

	_cnmenccom_init_members(&h263->comobj, IPPGSTCNMENC_STMFORMAT_H263, IPPGSTCNMENC_INITQP_H263, h263, gst_cnmh263enc_echofun, h263->sinkpad, h263->srcpad);

	GST_DEBUG_OBJECT(h263, "Gstcnmh263enc instance %p is inited!!!\n", h263);
	printf("Gstcnmh263enc instance %p is inited!!!\n", h263);

	return;
}
