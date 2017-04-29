/*
Copyright (c) 2010, Marvell International Ltd.
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
/* gstcnmdec_wmv3.h */

#ifndef __GST_CNMDEC_WMV3_H__
#define __GST_CNMDEC_WMV3_H__


/*****************************************************************************/
/* IPP codec interface */
#include "codecCNM.h"
/*****************************************************************************/

#include <gst/gst.h>
#include "gcnmcodec_com.h"


G_BEGIN_DECLS

#define GST_TYPE_CNMWMVDEC  	(gst_cnmwmvdec_get_type())
#define GST_CNMWMVDEC(obj)  	(G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_CNMWMVDEC,Gstcnmwmvdec))
#define GST_CNMWMVDEC_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_CNMWMVDEC,GstcnmwmvdecClass))
#define GST_IS_CNMWMVDEC(obj)   	(G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_CNMWMVDEC))
#define GST_IS_CNMWMVDEC_CLASS(obj) (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_CNMWMVDEC))


GType gst_cnmwmvdec_get_type (void);

typedef struct _Gstcnmwmvdec Gstcnmwmvdec;
typedef struct _GstcnmwmvdecClass GstcnmwmvdecClass;

#define GSTCNMDEC_WMV3_STMPOOLSZ	6

struct _Gstcnmwmvdec
{
	GstElement element;
	GstPad  				*sinkpad;
	GstPad  				*srcpad;

	void					*pWMVDecoderState;
	gint					num;
	gint					den;
	int 					bCodecSucLaunched;
	int 					b1stFrame_forFileplay;
	int 					iCodecErr;
	int						iMemErr;


	IppCMVpuDecParsedSeqInfo	SeqInfo;
	IppCMVpuDecFrameAddrInfo	DecFramesInfoArr[32];   //32 is enough
	int 						iDecFrameBufArrLen;
	IppCMDecFilePlayStmBufInfo  FilePlay_StmBuf;
	IppCMVpuDecOutputInfo   	DecOutInfo;
	int 						iAlignedBufSz;
	CNMDec_ShareFrameManager	ShareBufManager;
	SkipFrameBufRepo			SFBufRepo;
	CNMDecEleState				ElementState;

	unsigned char   			seqMeta[40]; //SMPTE 421M, Annex L. In spec. 36 bytes for seqMeta[] is ok, but for some legacy stream, the STRUCT_C are 5 bytes, even 6 bytes instead of 4 bytes. Therefore, let the seqMeta[] big enough.
	int 						seqMetaLen;

	GCNMDec_StmBuf  		StmBufPool[GSTCNMDEC_WMV3_STMPOOLSZ];
	GCNMDec_DownBufLayout   DownFrameLayout;
	GCNMDec_TSDUManager 	TsDuManager;

	int 					iI420LayoutKind;
	int 					iShareFrameBuf;
	int						iTimeoutForDisp1Frame;	//in usecond
	int 					iSupportMultiIns;
	int						bEnableHibernateAtPause;
	int						bEnableDynaClk;

	int						bNewSegReceivedAfterflush;

	GMutex*					cnmCodecMtx;
	int						bCnmInDream;

	int						wmvversion;
	void*					lastPushedShareBufVaddr;		//only for shareframe case

	guint64					adjustTsForNewSegGap;
	int						b1stChainAfterNewSeg;

	int 					totalFrames;
	gint64  				codec_time;
	int 					iDirectMVReformedFrameCnt;
	gpointer				pProbeData;

};

struct _GstcnmwmvdecClass
{
	GstElementClass parent_class;
};

G_END_DECLS


#endif

/* EOF */
