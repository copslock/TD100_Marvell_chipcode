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


#ifndef __GST_MJPEGDEC_H__
#define __GST_MJPEGDEC_H__


#include <gst/gst.h>

#include "misc.h"			/* IPP memory management header file */
#include "codecJP.h"		/* IPP-based JPEG codec header */
#include "codecDef.h"

//#define DEBUG_PERFORMANCE

G_BEGIN_DECLS

#define GST_TYPE_MJPEGDEC \
	(gst_mjpegdec_get_type())
#define GST_MJPEGDEC(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_MJPEGDEC,GstMJPEGDec))
#define GST_MJPEGDEC_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_MJPEGDEC,GstMJPEGDecClass))
#define GST_IS_MJPEGDEC(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_MJPEGDEC))
#define GST_IS_MJPEGDEC_CLASS(obj) \
	(G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_MJPEGDEC))


/* definition the structure and class of GST_MJPEGDEC */
typedef struct _GstMJPEGDec GstMJPEGDec;
typedef struct _GstMJPEGDecClass GstMJPEGDecClass;



//#define LOG_MJPEGDEC_INFO
struct _GstMJPEGDec
{
	GstElement					element;
	GstPad						*sinkpad;
	GstPad						*srcpad;

	/* GST IPP JPEG decoder variables */
	MiscGeneralCallbackTable	*pCBTable;
	void						*pJPEGDecState;
	IppJPEGDecoderParam			jpegDecPar;
	IppBitstream				srcBitStream;
	IppPicture					dstPicture;


	/* argument: JPEG decoder properties */
	IppJPEGColorFormat			outGstBufDataFormat;
	int							outputWidth; //output picture dimension.
	int							outputHeight;


	int							bCapSet;

	guint64						nextTS;
	guint64 					duration;
	int							iFRateNum;
	int							iFRateDen;

	GstBuffer*					outGstBuf;
	int							bJpegDstBufAllocatedBySelf;

	//property
	int							bForce_422to420;

	//for frame skip
	int							skip_strategy;
	int							is_playing;
	int							is_newSeg;
	int							frameSerialNum_in1stream;
	int							skip_patternidx;
	gint64						TS_exitSink;
	int							est_demuxdecCost_usec;
	int							t_decCost_usec;
	int							t_decCost_usec_avg;

#ifdef LOG_MJPEGDEC_INFO
	int							skipframecnt_in1stream;	//this var is only for log
#endif

#ifdef DEBUG_PERFORMANCE
	int							totalFrames;	//only count the decoded frame for all streams
	gint64						codec_time;
#endif
};

struct _GstMJPEGDecClass
{
	GstElementClass parent_class;
};


GType gst_mjpegdec_get_type (void);

G_END_DECLS


#endif	/* __GST_MJPEGDEC_H__ */

/* EOF */

