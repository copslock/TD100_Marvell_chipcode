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
/* gstmpeg2dec.h */

#ifndef __GST_MPEG2DEC_H__
#define __GST_MPEG2DEC_H__


/*****************************************************************************/
/* IPP codec interface */
 #include "codecVC.h"
/*****************************************************************************/

#include <gst/gst.h>


G_BEGIN_DECLS

#define GST_TYPE_IPPMPEG2DEC		(gst_ippmpeg2dec_get_type())
#define GST_IPPMPEG2DEC(obj)		(G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_IPPMPEG2DEC,Gstippmpeg2dec))
#define GST_IPPMPEG2DEC_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_IPPMPEG2DEC,Gstippmpeg2decClass))
#define GST_IS_IPPMPEG2DEC(obj)		(G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_IPPMPEG2DEC))
#define GST_IS_IPPMPEG2DEC_CLASS(obj)	(G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_IPPMPEG2DEC))


typedef struct _Gstippmpeg2dec Gstippmpeg2dec;
typedef struct _Gstippmpeg2decClass Gstippmpeg2decClass;


struct _Gstippmpeg2dec
{
	GstElement					element;
	GstPad						*sinkpad;
	GstPad						*srcpad;

	MiscGeneralCallbackTable	*pCbTable;
	void						*pMPEG2DecObj;
	int							bDecoderDisNSC;

	int							iDownFrameWidth;
	int							iDownFrameHeight;
	int							iDownbufYpitch;
	int							iDownbufUVpitch;
	int							iDownbufUoffset;
	int							iDownbufVoffset;
	int							iDownbufSz;
	int							bGetFRinfoFromDemuxer;
	int							iFRateNum;
	int							iFRateDen;
	GstClockTime				InputTs;
	GstClockTime				LattermostTs;
	GstClockTime				FrameFixDuration;

	int							bFirstDummyFrame;
	int							iLastFrameTemporalReference;

	int							UpAdjacentNonQueueEle;

	GstBuffer*					LeftData;
	int							bsCurBitOffset;

	int							iError_in1stream;

	int							bMetIFrameIn1Seg;
	int							bBBeforeI_CouldBeHealth;

	//property
	int							bNotDisplayFramesBeforeI;
	int							bEnableDynamicDeinterlace;

	int							bNewSegReceivedAfterflush;
	int							MpegVersion;

#ifdef DEBUG_PERFORMANCE
	int							totalFrames;	//only count the decoded frame for all streams
	gint64						codec_time;
#endif
};

struct _Gstippmpeg2decClass
{
	GstElementClass parent_class;
};

G_END_DECLS


#endif

/* EOF */
