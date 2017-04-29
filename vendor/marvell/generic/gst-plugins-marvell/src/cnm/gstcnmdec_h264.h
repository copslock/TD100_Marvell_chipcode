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
/* gstcnmdec_h264.h */

#ifndef __GST_CNMDEC_H264_H__
#define __GST_CNMDEC_H264_H__


/*****************************************************************************/
/* IPP codec interface */
#include "codecCNM.h"
/*****************************************************************************/

#include <gst/gst.h>
#include "gcnmcodec_com.h"


G_BEGIN_DECLS

#define GST_TYPE_CNMH264DEC 	(gst_cnmh264dec_get_type())
#define GST_CNMH264DEC(obj) 	(G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_CNMH264DEC,Gstcnmh264dec))
#define GST_CNMH264DEC_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_CNMH264DEC,Gstcnmh264decClass))
#define GST_IS_CNMH264DEC(obj)  		(G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_CNMH264DEC))
#define GST_IS_CNMH264DEC_CLASS(obj)	(G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_CNMH264DEC))


GType gst_cnmh264dec_get_type (void);

typedef struct _Gstcnmh264dec Gstcnmh264dec;
typedef struct _Gstcnmh264decClass Gstcnmh264decClass;

#define GSTCNMDEC_H264_STMPOOLSZ	6

struct _Gstcnmh264dec
{
	GstElement element;
	GstPad  				*sinkpad;
	GstPad  				*srcpad;

	void					*pH264DecoderState;
	gint					num;
	gint					den;
	int 					bCodecSucLaunched;
	int 					b1stFrame_forFileplay;
	int 					iCodecErr;
	int						iMemErr;

	gboolean				is_avcC;
	int 					nalLenFieldSize;

	IppCMVpuDecParsedSeqInfo	SeqInfo;
	IppCMVpuDecFrameAddrInfo	DecFramesInfoArr[32];   //32 is enough
	int 						iDecFrameBufArrLen;
	IppCMDecFilePlayStmBufInfo  FilePlay_StmBuf;
	IppCMVpuDecOutputInfo   	DecOutInfo;
	int 						iAlignedBufSz;
	CNMDec_ShareFrameManager	ShareBufManager;
	CNMDecEleState				ElementState;

	unsigned char   			*seqMeta;
	int 						seqMetaBufSz;
	int 						seqMetaLen;

	GCNMDec_StmBuf  		StmBufPool[GSTCNMDEC_H264_STMPOOLSZ];
	GCNMDec_DownBufLayout   DownFrameLayout;
	GCNMDec_TSDUManager 	TsDuManager;

	int						bSPSReceivedForNonAVCC;

	int 					bNotReorderForBaseline;
	int 					iI420LayoutKind;
	int 					iShareFrameBuf;
	int						iTimeoutForDisp1Frame;	//in usecond
	int 					iSupportMultiIns;
	int						bEnableHibernateAtPause;
	int						bEnableDynaClk;
	int						bNewSegReceivedAfterflush;

	GMutex*					cnmCodecMtx;
	int						bCnmInDream;

	int 					totalFrames;
	gint64  				codec_time;

	gpointer				pProbeData;

};

struct _Gstcnmh264decClass
{
	GstElementClass parent_class;
};

G_END_DECLS


#endif

/* EOF */
