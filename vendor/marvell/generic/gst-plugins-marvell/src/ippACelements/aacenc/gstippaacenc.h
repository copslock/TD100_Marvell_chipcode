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


#ifndef __GST_IPPAACENC_H__
#define __GST_IPPAACENC_H__

#include <gst/gst.h>

#include "misc.h"			/* IPP memory management header file */
#include "codecAC.h"		/* IPP based audio CODEC header file */

G_BEGIN_DECLS

/* #defines don't like whitespacey bits */
#define GST_TYPE_IPPAACLCENC \
  (gst_ippaaclcenc_get_type())
#define GST_IPPAACLCENC(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_IPPAACLCENC,GstippaacLCenc))
#define GST_IPPAACLCENC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_IPPAACLCENC,GstippaacLCencClass))
#define GST_IS_IPPAACLCENC(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_IPPAACLCENC))
#define GST_IS_IPPAACLCENC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_IPPAACLCENC))

typedef struct _GstippaacLCenc      GstippaacLCenc;
typedef struct _GstippaacLCencClass GstippaacLCencClass;


struct _GstippaacLCenc
{
	GstElement element;

	GstPad *sinkpad, *srcpad;

	/* AAC decoder variables */
	IppBitstream	dstbitStream;

	Ipp8u*			leftPcmBuf;
	int				leftPcmSize;

	/*param from sink & property pad*/
	int Samplerate;
	int channels;
	int bitrate;
	int tnsEn;
	int	bitstmFormat;

	int				PCMOneFrameSize;	//in bytes

	/*Time stamp & Clock*/
	GstClockTime					ts;
	GstClockTime					dur;

	MiscGeneralCallbackTable	*pCallBackTable;
	void 			*pEncoderState;

	int ForceOutputEndBS;

	guint64		iEncodedBytesLen;
	int			totalFrames;
	gint64 		codec_time;			/* codec time used for performance testing */
};

struct _GstippaacLCencClass
{
  GstElementClass parent_class;
};

GType gst_ippaaclcenc_get_type (void);

G_END_DECLS

#endif /* __GST_IPPAACENC_H__ */

/*EOF*/
