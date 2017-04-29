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
/* gstwmvdec.h */

#ifndef __GST_WMVDEC_H__
#define __GST_WMVDEC_H__


/*****************************************************************************/
/* IPP codec interface */
#include "codecWMV.h"
#include <misc.h>
/*****************************************************************************/

#include <gst/gst.h>


G_BEGIN_DECLS

#define GST_TYPE_WMVDEC	 (gst_wmvdec_get_type())
#define GST_WMVDEC(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_WMVDEC,Gstwmvdec))
#define GST_WMVDEC_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_WMVDEC,GstwmvdecClass))
#define GST_IS_WMVDEC(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_WMVDEC))
#define GST_IS_WMVDEC_CLASS(obj) (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_WMVDEC))


GType gst_wmvdec_get_type (void);

typedef struct _Gstwmvdec Gstwmvdec;
typedef struct _GstwmvdecClass GstwmvdecClass;

struct _Gstwmvdec
{
	GstElement element;
	GstPad					*sinkpad;
	GstPad					*srcpad;

	IppBitstream				srcBitStream;
	IppPicture				decPicture;

	MiscGeneralCallbackTable	*pCBTable;
	void						*pWMVDecoderState;

	int 						width;
	int 						height;
	int 						pic_size;

	int 						wmvversion;

	gint 						num;
	gint 						den;
	int						totalFrames;
	gint64						codec_time;

	guint64					duration;
	guint64					nextTS;
	guint64						saveTS;

	gboolean					dec_error;
	gboolean 				firsttime;

	int							bNewSegReceivedAfterflush;
	
	
};

struct _GstwmvdecClass 
{
	GstElementClass parent_class;
};

G_END_DECLS


#endif 

/* EOF */
