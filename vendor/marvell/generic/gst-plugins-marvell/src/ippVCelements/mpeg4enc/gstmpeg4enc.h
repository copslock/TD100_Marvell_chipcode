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



#ifndef __GST_MPEG4ENC_H__
#define __GST_MPEG4ENC_H__


#include <gst/gst.h>

#include "misc.h"			/* IPP memory management header file */
#include "codecVC.h"		/* IPP based video CODEC header file */
#include "ippGSTdefs.h"		/* IPP based MMF for Linux defines file */


G_BEGIN_DECLS

#define GST_TYPE_MPEG4ENC \
	(gst_mpeg4enc_get_type())
#define GST_MPEG4ENC(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_MPEG4ENC,GstMPEG4Enc))
#define GST_MPEG4ENC_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_MPEG4ENC,GstMPEG4EncClass))
#define GST_IS_MPEG4ENC(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_MPEG4ENC))
#define GST_IS_MPEG4ENC_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_MPEG4ENC))


typedef struct _GstMPEG4Enc      GstMPEG4Enc;
typedef struct _GstMPEG4EncClass GstMPEG4EncClass;


struct _GstMPEG4Enc
{
	GstElement					element;
	GstPad						*sinkpad;
	GstPad						*srcpad;
	GstClockTime					ts;
	GstClockTime					dur;

/* GstBuffer for input repack */
	GstBuffer 					*tempbuf;


	/* IPP CODEC variables */
	MiscGeneralCallbackTable		*pCBTable;
	void							*pMPEG4EncState;
	IppParSet					mpeg4EncPar;
	IppPicture					srcPicture;
	IppBitstream					dstBitStream;
	IppCodecStatus				retCode;

	/* the pixel number of a frame */
	int							picSize;
	int							picDataSize;
	int							picquartSize;
	int							totalFrames;		
	int							frameWidth;
	int							frameHeight;
	int							frameRate;
	int							bitRate;
	gint64						codec_time;

	gboolean					rateControl;
	gboolean					streamBegin;

	
};

struct _GstMPEG4EncClass 
{
	GstElementClass parent_class;
};

GType gst_mpeg4enc_get_type (void);

G_END_DECLS


#endif /* __GST_MPEG4ENC_H__ */

/* EOF */
