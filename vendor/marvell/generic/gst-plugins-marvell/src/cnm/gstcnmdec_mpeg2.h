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
/* gstcnmdec_mpeg2.h */

#ifndef __GST_CNMDEC_MPEG2_H__
#define __GST_CNMDEC_MPEG2_H__


/*****************************************************************************/
/* IPP codec interface */
#include "codecCNM.h"
/*****************************************************************************/

#include <gst/gst.h>
#include "gcnmcodec_com.h"


G_BEGIN_DECLS

#define GST_TYPE_CNMMPEG2DEC		(gst_cnmmpeg2dec_get_type())
#define GST_CNMMPEG2DEC(obj)		(G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_CNMMPEG2DEC,Gstcnmmpeg2dec))
#define GST_CNMMPEG2DEC_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_CNMMPEG2DEC,Gstcnmmpeg2decClass))
#define GST_IS_CNMMPEG2DEC(obj) 		(G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_CNMMPEG2DEC))
#define GST_IS_CNMMPEG2DEC_CLASS(obj)   (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_CNMMPEG2DEC))


GType gst_cnmmpeg2dec_get_type (void);

typedef struct _Gstcnmmpeg2dec Gstcnmmpeg2dec;
typedef struct _Gstcnmmpeg2decClass Gstcnmmpeg2decClass;

#define GSTCNMDEC_MPEG2_STMPOOLSZ   6

#define IPPGST_QUICKPARSER	//quick parse method is more efficient specially for high bps mpeg2 stream

typedef struct{
	unsigned char* pBuf;
	int bufSz;
	unsigned char* pParsed;
	unsigned char* pWriter;
}WorkingDataBuf;

typedef struct{
	WorkingDataBuf databuf;
	int	wantstate;
	unsigned char* pFrameBeg;
	unsigned char* pFrameEnd;
	unsigned char* pPicBeg[2];
	unsigned int pic_type[2];
	unsigned int temporal_reference[2];
	unsigned int pic_struct[2];
	int sliceinPicFound[2];
}IPP_MPEG2Parse;

struct _Gstcnmmpeg2dec
{
	GstElement element;
	GstPad  				*sinkpad;
	GstPad  				*srcpad;

	void					*pMPEG2DecoderState;
	gint					num;
	gint					den;
	int 					bCodecSucLaunched;
	int 					b1stFrame_forFileplay;
	int 					bSeqHdrSeeked;
	int 					iCodecErr;
	int						iMemErr;

	IppCMVpuDecParsedSeqInfo	SeqInfo;
	IppCMVpuDecFrameAddrInfo	DecFramesInfoArr[32];   //32 is enough
	int 						iDecFrameBufArrLen;
	IppCMDecFilePlayStmBufInfo  FilePlay_StmBuf;
	IppCMVpuDecOutputInfo   	DecOutInfo;
	int 						iAlignedBufSz;
	CNMDec_ShareFrameManager	ShareBufManager;
	CNMDecEleState				ElementState;


	unsigned char*  pActiveBuf;
	int 			iActiveBufSz;
	int 			iActiveDataLen;
	IPP_MPEG2Parse	stm_parser;

	int 			bLetDataBeginFromBorder;
	int 			bNotOutputPBframe;
	int 			bNotInputPBframe;

	GstClockTime last_inTs;
	guint64 TS_judgement;

	int						iIgnLastSegDelayedFrames;

	GCNMDec_StmBuf  		StmBufPool[GSTCNMDEC_MPEG2_STMPOOLSZ];
	GCNMDec_DownBufLayout   DownFrameLayout;
	GCNMDec_TSDUManager 	TsDuManager;

	int 					iIgnorePBBeforeI;
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

struct _Gstcnmmpeg2decClass
{
	GstElementClass parent_class;
};

G_END_DECLS


#endif

/* EOF */
