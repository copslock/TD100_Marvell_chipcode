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



#ifndef __GST_H264ENC_H__
#define __GST_H264ENC_H__


#include <gst/gst.h>

#include "misc.h"			/* IPP memory management header file */
#include "codecVC.h"		/* IPP based video CODEC header file */
#include "ippGSTdefs.h"		/* IPP based MMF for Linux defines file */


G_BEGIN_DECLS

#define GST_TYPE_IPPH264ENC \
	(gst_ipph264enc_get_type())
#define GST_IPPH264ENC(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_IPPH264ENC,GstIppH264Enc))
#define GST_IPPH264ENC_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_IPPH264ENC,GstIppH264EncClass))
#define GST_IS_IPPH264ENC(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_IPPH264ENC))
#define GST_IS_IPPH264ENC_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_IPPH264ENC))


typedef struct _GstIppH264Enc      GstIppH264Enc;
typedef struct _GstIppH264EncClass GstIppH264EncClass;

struct _GstIppH264Enc
{
	GstElement					element;
	GstPad						*sinkpad;
	GstPad						*srcpad;

	//IPP h264enc
	void*						H264Enc_Handle;
	H264EncoderCBTable			H264Enc_MemCBTable;
	H264EncoderParSet			H264Enc_Par;
	IppPicture					H264Enc_SrcImage;
	IppBitstream				H264Enc_DstBitsBuf;
	unsigned char*				pBuf8Aligned;
	int							iDstBitsBufUnitLen;
	
	int							iEncW;
	int							iEncH;
	int							i1SrcFrameDataLen;
	int							iSrcUoffset;
	int							iSrcVoffset;

	int							iEncFrameRate;
	guint64						iFixedFRateDuration;

	GstClockTime				PushdownTsCandidate;

	GstBuffer*					LeftData;

	//property
	int							iTargtBitRate;
	int							iQualityLevel;
	gint64						ForceStartTimestamp;

	//for log
	guint64						iEncOutputByteCnt;
	int							iEncOutputFrameCnt;
#ifdef DEBUG_PERFORMANCE
	gint64						codec_time;
#endif
};

struct _GstIppH264EncClass 
{
	GstElementClass parent_class;
};

GType gst_ipph264enc_get_type (void);

G_END_DECLS


#endif /* __GST_H264ENC_H__ */

/* EOF */
