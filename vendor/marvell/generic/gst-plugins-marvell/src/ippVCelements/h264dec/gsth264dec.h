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


#ifndef __GST_H264DEC_H__
#define __GST_H264DEC_H__



#include "misc.h"			/* IPP memory management header file */
#include "codecVC.h"		/* IPP based video CODEC header file */

#include <gst/gst.h>



G_BEGIN_DECLS

typedef void* (*BmmMalloc_Fun_T)(unsigned long size, int attr);
typedef void (*BmmFree_Fun_T)(void* vaddr);
typedef void* (*BmmGetPaddr_Fun_T)(void* vaddr);

#define GST_TYPE_H264DEC	(gst_h264dec_get_type())
#define GST_H264DEC(obj)	(G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_H264DEC,GstH264Dec))
#define GST_H264DEC_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_H264DEC,GstH264DecClass))
#define GST_IS_H264DEC(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_H264DEC))
#define GST_IS_H264DEC_CLASS(obj)	(G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_H264DEC))


/* definition the structure and class of GST_H264DEC */
typedef struct _GstH264Dec GstH264Dec;
typedef struct _GstH264DecClass GstH264DecClass;

#define DROP_REC_SHORTWND_LEN	4
#define DROP_REC_LONGWND_LEN	15

#define REORDER_BASEONPOC		//to support B frame h264 in avi/flv file, in those files, demuxer only provide decoding timestamp not presentation timestamp

struct _GstH264Dec
{
	GstElement					element;
	GstPad						*sinkpad;
	GstPad						*srcpad;


	/* IPP H.264 decoder variables */

	MiscGeneralCallbackTable	*pCBTable;
	void						*pH264DecState;
	IppH264PicList				*pDstPicList;

	int							nAvailFrames;

	int							iDownFrameWidth;
	int							iDownFrameHeight;
	int							iDownbufYpitch;
	int							iDownbufUVpitch;
	int							iDownbufUoffset;
	int							iDownbufVoffset;
	int							iDownbufSz;

	int							iFRateNum;
	int							iFRateDen;
	guint64						iFixedFRateDuration;		//in ns
	guint64						iFixedFRateDuration_5_8;	//5/8 of frame duration, in ns
	GstClockTime				LastInputValidTs;

	gboolean					is_avcC;
	int							nal_length_size; ///< Number of bytes used to store nal length (1, 2 or 4) in avcC


	IppBitstream				srcBitStream;

	int							iNominalProfile;

	GQueue						inTsQueue;	//store input Ts in coded order
	GSList*						DecodedFramesReorderPool;	//store decoded frame in display order
#ifdef REORDER_BASEONPOC
	GSList*						TSReorderPool;	//store decoded frame TS in display order
#endif
	int							iDecodedFramesReorderPoolLen;
	int							iReorderDelay;
	int							iReorderDelayCustomSet;
	int							bNotReorderForBaseline;

	/*
	When bReorderOnSinkBuffers is 1, once decoder decoded a frame, copy this frame to sink allocated buffer.
	Otherwise, copy decoder's frame to sink allocated buffer when we are going to fire the frame to video sink plug-in.
	There is a re-order delay between decoder decoded a frame and firing this frame to video sink.
	Therefore, if bReorderOnSinkBuffers is 1, doing frame reorder on video sink allocated buffers, otherwise doing frame reorder on decoder internal buffers.
	If bReorderOnSinkBuffers is 1, video sink probably report memory lacking if it hasn't enough frame memory.
	If bReorderOnSinkBuffers is 0, decoder probably report memory lacking(report IPP_STATUS_BUFFER_FULL or output repeated frames), and it affects the plug-in's decoding speed.
	*/
	int							bReorderOnSinkBuffers;

	//for frame skip
	int							skip_strategy;

	int							is_playing;
	int							iCurSmartSkipMode;
	int							iSmartModeByDimension;
	int							bForbid_RoughSkip;
	int							iCurRoughSkipDataPeriod;
	int							iRoughSkipDataIndicator;

	int							QosProtect_forNewSeq;
	gint						iNewQosReportedCnt;
	gint						iShortTermDropRate;
	gint						iLongTermDropRate;
	int							iNextRecordIdx_S;	//for short term
	int							iNextRecordIdx_L;
	int							DropRecordWnd_S[DROP_REC_SHORTWND_LEN];
	int							DropRecordWnd_L[DROP_REC_LONGWND_LEN];


	IppPicture					DummyPic;
	IppH264PicList				DummyPicList;

	gchar*						skip_logfilename;
	FILE*						skip_logfile;

//log information
	int							demuxer_datafrag_cnt;
	int							datafrag_roughskipped_cnt;
	int							decoder_frameskipped_cnt;
	int							decoded_frame_cnt;
	int							frame_suc_pushed2sink_cnt;
	int							smartmode_switchcnt;

	int							iRawDecoder_FrameBufSize;
	int							iRawDecoder_FrameBufCnt;

	//for sI42
	int							bForbid_sI42;
	int							DownStreamBufType;
	void*						BmmLibHandle;
	BmmMalloc_Fun_T				pfunBmmMalloc;
	BmmFree_Fun_T				pfunBmmFree;
	BmmGetPaddr_Fun_T			pfunBmmGetPaddr;
	int							iRawDecoderFrameFreedFlag;

	void*						ResFinalizer;

	int							ForceCopyStreamBuf;
	unsigned char*				pSelfStreamBuf;
	int							iSelfStreamBufSz;

	int							bNewSegReceivedAfterflush;

#ifdef DEBUG_PERFORMANCE
	int							totalFrames;	//only count the decoded frame for all streams
	gint64						codec_time;
#endif

};

struct _GstH264DecClass
{
	GstElementClass parent_class;
};


GType gst_h264dec_get_type (void);

G_END_DECLS


#endif	/* __GST_H264DEC_H__ */

/* EOF */
