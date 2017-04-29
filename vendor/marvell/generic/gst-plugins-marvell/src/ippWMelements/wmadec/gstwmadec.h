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
/* gstwmadec.h */

#ifndef __GST_WMADEC_H__
#define __GST_WMADEC_H__


/*****************************************************************************/
/* IPP codec interface */
#include "codecWMA.h"
#include <misc.h>
/*****************************************************************************/

#include <gst/gst.h>


G_BEGIN_DECLS

#define GST_TYPE_WMADEC	 (gst_wmadec_get_type())
#define GST_WMADEC(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_WMADEC,Gstwmadec))
#define GST_WMADEC_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_WMADEC,GstwmadecClass))
#define GST_IS_WMADEC(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_WMADEC))
#define GST_IS_WMADEC_CLASS(obj) (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_WMADEC))


GType gst_wmadec_get_type (void);

typedef struct _Gstwmadec Gstwmadec;
typedef struct _GstwmadecClass GstwmadecClass;

struct _Gstwmadec
{
	GstElement element;
	GstPad						*sinkpad;
	GstPad						*srcpad;
	
	MiscGeneralCallbackTable	*pCBTable;
	void 					*pDecoderState;

	char 					*poutbuf;	/* use for output buffer, since wma need align request */

	int 						max_framesize;

	int						totalFrames;
	gint64						codec_time;

	guint64					duration;
	guint64					nextTS;

	IppWMADecoderConfig 		decoderConfig;
	IppBitstream				srcBitStream;
	IppSound 				sound;

	gboolean					init_error;

	
};

struct _GstwmadecClass 
{
	GstElementClass parent_class;
};

G_END_DECLS


#endif 

/* EOF */
