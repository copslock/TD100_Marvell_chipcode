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

#ifndef __GST_CNMENC_H264_H__
#define __GST_CNMENC_H264_H__

#include <gst/gst.h>
#include "gstcnmenc_com.h"

G_BEGIN_DECLS
/* Standard macros for defining types for this element. */
#define GST_TYPE_CNMH264ENC		(gst_cnmh264enc_get_type())
#define GST_CNMH264ENC(obj)		(G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_CNMH264ENC,Gstcnmh264enc))
#define GST_CNMH264ENC_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_CNMH264ENC,Gstcnmh264encClass))
#define GST_IS_CNMH264ENC(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_CNMH264ENC))
#define GST_IS_CNMH264ENC_CLASS(obj)	(G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_CNMH264ENC))

/* Standard function returning type information. */
GType gst_cnmh264enc_get_type(void);

typedef struct _Gstcnmh264enc Gstcnmh264enc;
typedef struct _Gstcnmh264encClass Gstcnmh264encClass;
struct _Gstcnmh264enc
{
	GstElement	element;
	GstPad		*sinkpad;
	GstPad		*srcpad;

	IppGSTCnMEncCOM		comobj;
};

struct _Gstcnmh264encClass
{
	GstElementClass parent_class;
};

G_END_DECLS

#endif