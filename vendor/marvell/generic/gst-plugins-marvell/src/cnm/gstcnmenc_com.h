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

#ifndef __GST_CNMENC_COM_H__
#define __GST_CNMENC_COM_H__

#include <gst/gst.h>
#include "codecCNM.h"
#include "cnm_bridge.h"
#include "gcnmcodec_com.h"

G_BEGIN_DECLS

enum {
	ARG_0,

	/* fill below with user defined arguments */
	ARG_GOPSIZE,
	ARG_INITQP,
	ARG_RCENABLE,
	ARG_RCTYPE,
	ARG_RCBITRATE,
	ARG_RCMAXBITRATE,
	ARG_DELAYLIMIT,
	ARG_SUPPORTMULTIINS,
	ARG_ENABLEDYNACLK,
	ARG_FORCE_STARTTIME,
#ifdef DEBUG_PERFORMANCE
	ARG_TOTALFRAME,
	ARG_CODECTIME,
#endif
};
#ifdef DEBUG_PERFORMANCE
#define CNMENC_COMPROP_NUM	12
#else
#define CNMENC_COMPROP_NUM	10
#endif

#define IPPGSTCNMENC_STMFORMAT_H264			1
#define IPPGSTCNMENC_STMFORMAT_MPEG4		2
#define IPPGSTCNMENC_STMFORMAT_H263			3

#define IPPPSEUDO_GST_ERROR_OBJECT(shareobj, args...)		shareobj->GSTobj_echo_fun(shareobj->gst_ele, GST_LEVEL_ERROR, ##args)	//Because it use callback function and there is sprintf in callback, there is some additional cost to use this echo system. Therefore, it's better not to use it in each frame operation like _chain function.
#define IPPPSEUDO_GST_WARNING_OBJECT(shareobj, args...)		shareobj->GSTobj_echo_fun(shareobj->gst_ele, GST_LEVEL_WARNING, ##args)
#define IPPPSEUDO_GST_INFO_OBJECT(shareobj, args...)		shareobj->GSTobj_echo_fun(shareobj->gst_ele, GST_LEVEL_INFO, ##args)
#define IPPPSEUDO_GST_DEBUG_OBJECT(shareobj, args...)		shareobj->GSTobj_echo_fun(shareobj->gst_ele, GST_LEVEL_DEBUG, ##args)
#define IPPPSEUDO_GST_LOG_OBJECT(shareobj, args...)			shareobj->GSTobj_echo_fun(shareobj->gst_ele, GST_LEVEL_LOG, ##args)

typedef void (*pfun_PSEUDO_GST_XXX_OBJECT)(void*, GstDebugLevel, const char*, ...);

typedef struct{
	void*	gst_ele;
	pfun_PSEUDO_GST_XXX_OBJECT GSTobj_echo_fun;
	GstPad*	sinkpad;
	GstPad*	srcpad;

	void*						cnmenc_handle;
	MiscGeneralCallbackTable*	pSrcCBTable;
	PhyContMemInfo				StmBufMem;
	PhyContMemInfo				FrameBufMem;
	IppVPUBitstream				BsBufTemplate;
	int							iDataLen_FrameBufForFileSrc;
	unsigned char*				SeqHdrBuf;
	int							SeqHdrLen;
	int							iWholeFrameInputModeFlag;

	//resolution information
	int							iWidth;
	int							iHeight;
	int							iFrameSize;
	int							iUoffset;
	int							iVoffset;

	//fps, TS information
	int							iAllowFractionFps;
	int							iWillScaleBps;
	int							iInputFpsNum;
	int							iInputFpsDen;
	int							iRoundedFrameRate;
	guint64						u64FrameFixedDuration;
	GQueue						StmTSCandidateQueue;
	GstClockTime				AutoIncTimestamp;

	//Property
	int							PseudoProp_streamspec;
	int							Prop_iGopSize;
	int							Prop_iInitQp;
	int							Prop_bRCEnable;
	int							Prop_iRCType;
	int							Prop_iRCBitRate;
	int							Prop_iRCMaxBitRate;
	int							Prop_iDelayLimit;
	IppEncAvcParam				Prop_h264Par;
	IppEncH263Param				Prop_h263Par;
	IppEncMp4Param				Prop_mpeg4Par;

	int							Prop_iMultiInstanceSync;
	int							Prop_bDynaClkEnable;
	gint64						Prop_ForceStartTimestamp;

	int							Prop_bInsertSeqHdr;

	int							iEncProducedCompressedFrameCnt;
	guint64						iEncProducedByteCnt;
	gint64						codec_time;
}IppGSTCnMEncCOM;

GstFlowReturn _cnmenccom_chain(GstBuffer* buf, IppGSTCnMEncCOM* cnmcom, int* pbTerminate, int* pErrReason);
GstStateChangeReturn _cnmenccom_change_state(GstElement *element, GstStateChange transition, IppGSTCnMEncCOM* cnmcom, GstElementClass* father_class);
gboolean _cnmenccom_sinkpad_event(GstPad* pad, GstEvent* event, IppGSTCnMEncCOM* cnmcom);
gboolean _cnmenccom_sinkpad_setcaps(GstPad* pad, GstCaps* caps, IppGSTCnMEncCOM* cnmcom);
int _cnmenccom_set_com_prop(GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec, IppGSTCnMEncCOM* cnmcom);
int _cnmenccom_get_prop(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec, IppGSTCnMEncCOM* cnmcom);
void _cnmenccom_init_members(IppGSTCnMEncCOM* cnmcom, int ippgstcnmenc_stmfmt, int initqp, void* gst_element, pfun_PSEUDO_GST_XXX_OBJECT echofun, GstPad* sinkpad, GstPad* srcpad);
GstFlowReturn _cnmenccom_sinkpad_allocbuf(GstPad *pad, guint64 offset, guint size, GstCaps *caps, GstBuffer **buf, IppGSTCnMEncCOM* cnmcom);
void _cnmenccom_class_install_com_prop(GObjectClass* gobject_class);

G_END_DECLS

#endif
