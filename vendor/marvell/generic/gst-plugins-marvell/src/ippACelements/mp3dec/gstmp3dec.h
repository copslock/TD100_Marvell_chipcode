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

#ifndef __GST_MP3DEC_H__
#define __GST_MP3DEC_H__


#include <gst/gst.h>

#include "ippdefs.h"

#include "misc.h"			/* IPP memory management header file */
#include "codecAC.h"		/* IPP based audio CODEC header file */
#include "ippGSTdefs.h"		/* IPP based MMF for Linux defines file */


G_BEGIN_DECLS

#define GST_TYPE_MP3DEC \
	(gst_mp3dec_get_type())
#define GST_MP3DEC(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_MP3DEC,GstMP3Dec))
#define GST_MP3DEC_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_MP3DEC,GstMP3DecClass))
#define GST_IS_MP3DEC(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_MP3DEC))
#define GST_IS_MP3DEC_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_MP3DEC))


typedef struct _GstMP3Dec      GstMP3Dec;
typedef struct _GstMP3DecClass GstMP3DecClass;


struct _GstMP3Dec
{
	GstElement					element;
	GstPad						*sinkpad;
	GstPad						*srcpad;

	/* IPP MP3 decoder variables */
	MiscGeneralCallbackTable	*pCBTable;
	void						*pMP3DecState;
	IppBitstream				srcBitStream;
	IppSound					sound;
	int							totalFrames;
	IppCodecStatus				retCode;
	

	/* GstBuffer for input repack */
	GstBuffer 					*tempbuf;

	//buffer
	codecbuff curr_buf;
	
	/* buffer and counter for output multi-frame */
	GstBuffer	*outBuf;
	GstBuffer	*lastBuf;
	void		*pcmData;
	int		framecnt;
	gint64		codec_time;

	guint64						lastTS;
	guint64						nextTS;

	/* stream information */
	int							mpegVersion;
	int							mpegLayer;
	int							channelNumber;
	int							framesize;
	int							samplingRate;
	int							bitRateIndex;
	int							bitRate;
	int							init_error;

	gboolean					streamBegin;
	/* seek indicate seek operation is required, codec should find a new snyc code to begin decoding. */
	gboolean					seek;
	/* seeked indicate seek operation has just happened, we should ignore any codec error that proceeding seek operation. */
	gboolean					seeked;

	int							iID3TagLen;
	int							iAPETagLen;
	int							bFirstPushDownDataAfterNewseg;
	
	int							iCodecContinueOkCnt;
};

struct _GstMP3DecClass 
{
	GstElementClass parent_class;
};

GType gst_mp3dec_get_type (void);

G_END_DECLS


#endif /* __GST_MP3DEC_H__ */

/* EOF */
