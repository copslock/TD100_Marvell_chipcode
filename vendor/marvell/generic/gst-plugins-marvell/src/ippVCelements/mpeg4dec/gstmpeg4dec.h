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


#ifndef __GST_MPEG4DEC_H__
#define __GST_MPEG4DEC_H__


#include <gst/gst.h>

#include "misc.h"			/* IPP memory management header file */
#include "codecVC.h"		/* IPP based video CODEC header file */
#include "ippGSTdefs.h"		/* IPP based MMF for Linux defines file */

/*Enable MPEG4 Frame Skip with MA filter*/
//#define MPEG4DECGST_FRAMESKIP_ENABLE

G_BEGIN_DECLS

#define GST_TYPE_MPEG4DEC \
	(gst_mpeg4dec_get_type())
#define GST_MPEG4DEC(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_MPEG4DEC,GstMPEG4Dec))
#define GST_MPEG4DEC_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_MPEG4DEC,GstMPEG4DecClass))
#define GST_IS_MPEG4DEC(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_MPEG4DEC))
#define GST_IS_MPEG4DEC_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_MPEG4DEC))


typedef struct _GstMPEG4Dec      GstMPEG4Dec;
typedef struct _GstMPEG4DecClass GstMPEG4DecClass;

#ifdef MPEG4DECGST_FRAMESKIP_ENABLE

#define DROP_REC_SHORTWND_LEN   				4
#define DROP_REC_LONGWND_LEN    				13
#define SHORTWND_QOSWATCHLEN_ADJUST                             4
#define LONGWND_QOSWATCHLEN_ADJUST                              1
#define QOSLOG_PROTECT_FORNEWSEG                                4
#define SMARTSKIPMODE_NORMALDECODE                                      0
#define SMARTSKIPMODE_PERFORMANCE_PREFER_LITTLE         1               //decoder do fast deblock
#define SMARTSKIPMODE_PERFORMANCE_PREFER_MID            2               //non deblock

#define GET_INT32_NominalAtomic(x)              (x)
#define SET_INT32_NominalAtomic(x, val) ((x)=(val))
#define INC_INT32_NominalAtomic(x)              ((x)++)
#define PERFORMANCE_LACK 1
#define PERFORMANCE_RICH 0


#endif


struct _GstMPEG4Dec
{
	GstElement					element;
	GstPad						*sinkpad;
	GstPad						*srcpad;

	/* IPP CODEC variables */
	MiscGeneralCallbackTable	*pCBTable;
	void						*pMPEG4DecState;
	IppBitstream				srcBitStream;
	IppPicture					decPicture;
	
	GSList*						TsDuList;
	int							iTsDuListLen;
	GstClockTime				LastOutputTs;
	GstClockTime				LastOutputDu;
	GstClockTime				FrameFixPeriod;

	int							PackedFrameOccur;

	int							NSCCheckDisable;

	gint						num;
	gint						den;
	int							iDownFrameWidth;
	int							iDownFrameHeight;
	int							iDownbufYpitch;
	int							iDownbufUVpitch;
	int							iDownbufUoffset;
	int							iDownbufVoffset;
	int							iDownbufSz;

	int							iTimeIncBits;

	int							totalFrames;
	gint64						codec_time;
	int							errcnt;

	gboolean 					firsttime;
	gboolean					dec_error;

	int							bNewSegReceivedAfterflush;
	int							bDropReorderDelayedFrame;

#ifdef MPEG4DECGST_FRAMESKIP_ENABLE
	/*moving average filter for frame droping*/
	int                                             iCurSmartSkipMode; 
	int                                             QosProtect_forNewSeq;
        gint                                            iNewQosReportedCnt;
        gint                                            iShortTermDropRate;
        gint                                            iLongTermDropRate;
        int                                             iNextRecordIdx_S;       //for short term
        int                                             iNextRecordIdx_L;
        int                                             DropRecordWnd_S[DROP_REC_SHORTWND_LEN];
        int                                             DropRecordWnd_L[DROP_REC_LONGWND_LEN];
#endif
	
};

struct _GstMPEG4DecClass 
{
	GstElementClass parent_class;
};

GType gst_mpeg4dec_get_type (void);

G_END_DECLS


#endif /* __GST_MPEG4DEC_H__ */

/* EOF */
