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



#ifndef __GST_H263DEC_H__
#define __GST_H263DEC_H__


#include <gst/gst.h>

#include "misc.h"			/* IPP memory management header file */
#include "codecVC.h"		/* IPP based video CODEC header file */
#include "ippGSTdefs.h"		/* IPP based MMF for Linux defines file */


G_BEGIN_DECLS

#define GST_TYPE_H263DEC \
	(gst_h263dec_get_type())
#define GST_H263DEC(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_H263DEC,GstH263Dec))
#define GST_H263DEC_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_H263DEC,GstH263DecClass))
#define GST_IS_H263DEC(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_H263DEC))
#define GST_IS_H263DEC_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_H263DEC))


typedef struct _GstH263Dec      GstH263Dec;
typedef struct _GstH263DecClass GstH263DecClass;

struct _GstH263Dec
{
	GstElement					element;
	GstPad						*sinkpad;
	GstPad						*srcpad;

	/* IPP CODEC variables */
	MiscGeneralCallbackTable	*pCBTable;
	void						*pH263DecState;
	IppBitstream				srcBitStream;
	IppPicture					decPicture;
	IppPosition					NextSyncCode;
	IppCodecStatus				retCode;

	
	guint64						duration;
	guint64						nextTS;

	int							totalFrames;
	int							NSCCheckDisable;	

	int							picSize;
	int							picDataSize;
	int							frameWidth;
	int							frameHeight;

	gint num;
	gint den;

	gboolean					haveCaps;

	gint64							codec_time;
	
};

struct _GstH263DecClass 
{
	GstElementClass parent_class;
};

GType gst_h263dec_get_type (void);

G_END_DECLS


#endif /* __GST_H263DEC_H__ */

/* EOF */
