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

/******************************************************************************
 *
 *now only support HEADERLESS_IF1_FORMAT format, since we got data from .3gp file
 *
******************************************************************************/

#ifndef __GST_AMRNBENC_H__
#define __GST_AMRNBENC_H__


/*****************************************************************************/
#include <gst/gst.h>

//#include "ippdefs.h"

#include "misc.h"   		/* IPP memory management header file */
#include "codecSC.h"
#include "ippGSTdefs.h" 	/* IPP based MMF for Linux defines file *///declare amrnb_bit_rate


G_BEGIN_DECLS

#define GST_TYPE_IPPAMRNBENC    (gst_ippamrnbenc_get_type())
#define GST_IPPAMRNBENC(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_IPPAMRNBENC,Gstippamrnbenc))
#define GST_IPPAMRNBENC_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_IPPAMRNBENC,GstippamrnbencClass))
#define GST_IS_IPPAMRNBENC(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_IPPAMRNBENC))
#define GST_IS_IPPAMRNBENC_CLASS(obj) (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_IPPAMRNBENC))



typedef struct _Gstippamrnbenc Gstippamrnbenc;
typedef struct _GstippamrnbencClass GstippamrnbencClass;

struct _Gstippamrnbenc
{
	GstElement element;
	GstPad  					*sinkpad;
	GstPad  					*srcpad;

	/* GSMAMR related structures and buffers */

	IppSound speech_in;
	IppBitstream bitstream;
	IppGSMAMRFormatConfig config;

	/* GstBuffer for input repack */
	GstBuffer   				*tempbuf;



	MiscGeneralCallbackTable *pCallbackTable;
	void *pEncoderState;

	int 	dtx;
	int 	vad;
	int 	bitrate;

	amrnb_bit_rate input_bitrate;	//declared in ippGSTdefs.h



	int nFrameLen;
	int totalframe;
	int need_len;


	/* output settings */
	gint channels, rate;
	gint duration;

	guint64 ts;
	gint64 codec_time;

};

struct _GstippamrnbencClass
{
	GstElementClass parent_class;
};

GType gst_ippamrnbenc_get_type (void);

G_END_DECLS


#endif

/* EOF */
