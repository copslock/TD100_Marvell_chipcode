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

#ifndef __GST_AACDEC_H__
#define __GST_AACDEC_H__


#include <gst/gst.h>

#include "misc.h"			/* IPP memory management header file */
#include "codecAC.h"		/* IPP based audio CODEC header file */
#include "ippGSTdefs.h"		/* IPP based MMF for Linux defines file */


G_BEGIN_DECLS

#define GST_TYPE_AACDEC \
	(gst_aacdec_get_type())
#define GST_AACDEC(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_AACDEC,GstAACDec))
#define GST_AACDEC_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_AACDEC,GstAACDecClass))
#define GST_IS_AACDEC(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_AACDEC))
#define GST_IS_AACDEC_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_AACDEC))


typedef struct _GstAACDec      GstAACDec;
typedef struct _GstAACDecClass GstAACDecClass;

struct _GstAACDec
{
	GstElement					element;
	GstPad						*sinkpad;
	GstPad						*srcpad;

	/* AAC decoder variables */

	/* arguments of Stream Profile & Stream Format */
	GstAACDecStreamFormat		streamFormat;
	GstAACDecChannelConfig		channelConfig;
	GstAACDecFrameBytes			frameLenFlag;
	


	IppAACDecoderConfig			aacdecConfig;
	IppBitstream					srcBitStream;
	IppSound						sound;
	IppCodecStatus				retCode;

	//buffer
	codecbuff curr_buf;



	guint64						lastTS;
	guint64						nextTS;

	guint64						decodedBytes;
	guint64						decodedTime;

	guint64						gTotalDuration; // duration of aac adts stream in ns, to solve GST aac adts couldn't accurate seek issue
	guint64						gTotalLength;   // length of aac adts stream in byte, to solve GST aac adts couldn't accurate seek issue

	MiscGeneralCallbackTable		*pCBTable;
	void							*pAACDecState;

	/* GstBuffer for input repack */
	GstBuffer 					*tempbuf;

	/* buffer and counter for output multi-frame */
	GstBuffer	*outBuf;
	GstBuffer	*lastBuf;
	void		*pcmData;
	int		framecnt;

	int							samplingIndex;
	
	int							totalFrames;

	int							version;				/* version detected automatically */
	int 							version_config;		/* version configured by user */

	gint64 							codec_time;			/* codec time used for performance testing */
	

	gint    error_count;

	gboolean						streamBegin;

	gboolean 					raw; 
	gboolean						seek;
	gboolean						seek_support; 	/*ADIF do not have sync code, we do not support*/

	gboolean						ltp;

	int							iID3TagLen;

	int							bFirstPushDownDataAfterNewseg;

	gboolean					InputFramed;	//FALSE means not framed or unknown
	int							ADTS_judge;		//-1: not judged, 0: not ADTS, 1: ADTS
	GstBuffer*					ADTS_repackGstBuf;
	int							iIsInEOS;

	int							bIsMultiChanDownMix;
	int							bIsADIF;
};


struct _GstAACDecClass 
{
	GstElementClass parent_class;
};

GType gst_aacdec_get_type (void);

G_END_DECLS


#endif /* __GST_AACDEC_H__ */

/* EOF */
