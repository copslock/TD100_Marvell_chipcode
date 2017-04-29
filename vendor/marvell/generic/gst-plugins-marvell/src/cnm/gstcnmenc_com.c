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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h> //to include memset(),...
#include "misc.h"
#include "ippGST_sideinfo.h"
#include "gstcnmenc_com.h"

#ifdef DEBUG_PERFORMANCE
#include <sys/time.h>
#include <time.h>
#endif

//a subclass from GstBuffer
typedef struct {
	GstBuffer						gstbuf;
	PhyContMemInfo					phyMem;
}GstCNMEncFrameBuffer;

#define GST_TYPE_CNMENC_BUFFER		(gst_cnmenc_buffer_get_type())
#define GST_IS_CNMENC_BUFFER(obj)	(G_TYPE_CHECK_INSTANCE_TYPE((obj), GST_TYPE_CNMENC_BUFFER))
#define GST_CNMENC_BUFFER(obj)		(G_TYPE_CHECK_INSTANCE_CAST((obj), GST_TYPE_CNMENC_BUFFER, GstCNMEncFrameBuffer))
#define GST_CNMENC_BUFFER_CAST(obj)	((GstCNMEncFrameBuffer *)(obj))

static GstMiniObjectClass* cnmenc_buffer_parent_class = NULL;

static void
gst_cnmenc_buffer_finalize(GstCNMEncFrameBuffer * buf)
{
	if(buf->phyMem.iSize != 0) {
		cnm_os_api_pcontmem_free(&buf->phyMem);
		buf->phyMem.iSize = 0;
	}
	cnmenc_buffer_parent_class->finalize(GST_MINI_OBJECT_CAST(buf));
	return;
}

static void
gst_cnmenc_buffer_class_init(gpointer g_class, gpointer class_data)
{
	GstMiniObjectClass *mini_object_class = GST_MINI_OBJECT_CLASS(g_class);
	cnmenc_buffer_parent_class = g_type_class_peek_parent(g_class);
	mini_object_class->finalize = (GstMiniObjectFinalizeFunction)gst_cnmenc_buffer_finalize;
	return;
}

static void
gst_cnmenc_buffer_init(GTypeInstance * instance, gpointer g_class)
{
	//memset(&((GstCNMEncFrameBuffer)instance->phyMem), 0, sizeof(PhyContMemInfo));	//in parent class init, the instance has been clear to 0. therefore, no need to clear it again.
	return;
}

GType
gst_cnmenc_buffer_get_type(void)
{
	static GType buffer_type;

	if (G_UNLIKELY(buffer_type == 0)) {
		static const GTypeInfo buffer_info = {
			sizeof(GstBufferClass),
			NULL,
			NULL,
			gst_cnmenc_buffer_class_init,
			NULL,
			NULL,
			sizeof(GstCNMEncFrameBuffer),
			0,
			gst_cnmenc_buffer_init,
			NULL
		};
		buffer_type = g_type_register_static(GST_TYPE_BUFFER, "GstCNMEncFrameBuffer", &buffer_info, 0);
	}
	return buffer_type;
}

static __inline GstCNMEncFrameBuffer *
gst_CNMEncBuffer_new()
{
	return (GstCNMEncFrameBuffer*)gst_mini_object_new(GST_TYPE_CNMENC_BUFFER);
}
//end of a subclass from GstBuffer


#define IPPGSTBUFTYPE_DIRECTUSABLE_ENCPLUGIN_ALLOCATED	0
#define IPPGSTBUFTYPE_DIRECTUSABLE_SRCPLUGIN_ALLOCATED	1
#define IPPGSTBUFTYPE_NOTDIRECTUSABLE					-1
static int IPP_DistinguishGstBufFromUpstream(IppGSTCnMEncCOM* cnmcom, GstBuffer* buf, int* pnPhyAddr)
{
	if(GST_IS_CNMENC_BUFFER(buf)) {
		*pnPhyAddr = ((GstCNMEncFrameBuffer*)buf)->phyMem.phyAddr;
		return IPPGSTBUFTYPE_DIRECTUSABLE_ENCPLUGIN_ALLOCATED;
	}else{
		unsigned int PhyAddr = 0;
		if(IPPGST_BUFFER_CUSTOMDATA(buf) != 0) {
			IPPGSTDecDownBufSideInfo* sideinfo = IPPGST_BUFFER_CUSTOMDATA(buf);
			if(sideinfo->x_off == 0 && sideinfo->y_off == 0 && sideinfo->x_stride == cnmcom->iWidth && sideinfo->y_stride == cnmcom->iHeight) {
				//upstream plug-in probably is cnm decoder plug-in. And encoder only support the case ROI is the same as entire picture
				PhyAddr = sideinfo->phyAddr;
			}
		}
		if(PhyAddr != 0) {
			*pnPhyAddr = (int)PhyAddr;
			return IPPGSTBUFTYPE_DIRECTUSABLE_SRCPLUGIN_ALLOCATED;
		}
	}
	return IPPGSTBUFTYPE_NOTDIRECTUSABLE;
}


#define IPPGSTCNMENC_INITQP_MIN				1
#define IPPGSTCNMENC_INITQP_MAX				51
#define IPPGSTCNMENC_INITQP_DEF				15

#define IPPGSTCNMENC_BITRATE_MAX			(20*1024*1024)
#define IPPGSTCNMENC_BITRATE_DEF			(45000)
#define IPPGSTCNMENC_MAXBITRATE_MAX			((int)(IPPGSTCNMENC_BITRATE_MAX*1.5))
#define IPPGSTCNMENC_MAXBITRATE_DEF			((int)(IPPGSTCNMENC_BITRATE_DEF*1.5))

#define IPPGSTCNMENC_DELAYLIMIT_MIN			500
#define IPPGSTCNMENC_DELAYLIMIT_MAX			2000
#define IPPGSTCNMENC_DELAYLIMIT_DEF			1000

static __inline int trans_format_gst2enc(IppVPUEncParSet* pPar, int gstformat)
{
	if(gstformat == IPPGSTCNMENC_STMFORMAT_H264) {
		pPar->iFormat = 5;
	}else if(gstformat == IPPGSTCNMENC_STMFORMAT_H263) {
		pPar->iFormat = 4;
	}else if(gstformat == IPPGSTCNMENC_STMFORMAT_MPEG4) {
		pPar->iFormat = 2;
	}else{
		return -1;
	}
	return 0;
}

static __inline int trans_format_enc2gst(IppVPUEncParSet* pPar, int* gstformat)
{
	if(pPar->iFormat == 5) {
		*gstformat = IPPGSTCNMENC_STMFORMAT_H264;
	}else if(pPar->iFormat == 4) {
		*gstformat = IPPGSTCNMENC_STMFORMAT_H263;
	}else if(pPar->iFormat == 2) {
		*gstformat = IPPGSTCNMENC_STMFORMAT_MPEG4;
	}else{
		return -1;
	}
	return 0;
}

static __inline int scaleBps(IppGSTCnMEncCOM* cnmcom, int bps)
{
	if(cnmcom->iWillScaleBps) {
		return (int)gst_util_uint64_scale_int((guint64)bps * cnmcom->iRoundedFrameRate, cnmcom->iInputFpsDen, cnmcom->iInputFpsNum);
	}
	return bps;
}

static __inline int revertScaleBps(IppGSTCnMEncCOM* cnmcom, int scaledBps)
{
	if(cnmcom->iWillScaleBps) {
		return (int)gst_util_uint64_scale(scaledBps, cnmcom->iInputFpsNum, (guint64)cnmcom->iInputFpsDen * cnmcom->iRoundedFrameRate);
	}
	return scaledBps;
}

static int SetEncPar(IppVPUEncParSet* pPar, IppGSTCnMEncCOM* cnmcom)
{
	trans_format_gst2enc(pPar, cnmcom->PseudoProp_streamspec);
	pPar->iWidth = cnmcom->iWidth;
	pPar->iHeight = cnmcom->iHeight;
	pPar->iFrameRate = cnmcom->iRoundedFrameRate;
	pPar->iGopSize = cnmcom->Prop_iGopSize;
	pPar->iInitQp = cnmcom->Prop_iInitQp;
	pPar->bRCEnable = cnmcom->Prop_bRCEnable;
	pPar->iRCType = cnmcom->Prop_iRCType;
	pPar->iRCBitRate = scaleBps(cnmcom, cnmcom->Prop_iRCBitRate);
	pPar->iRCMaxBitRate = scaleBps(cnmcom, cnmcom->Prop_iRCMaxBitRate);
	pPar->iDelayLimit = cnmcom->Prop_iDelayLimit;

	if(cnmcom->PseudoProp_streamspec == IPPGSTCNMENC_STMFORMAT_H263) {
		pPar->IppEncStdParam.h263Param = cnmcom->Prop_h263Par;
	}else if(cnmcom->PseudoProp_streamspec == IPPGSTCNMENC_STMFORMAT_MPEG4) {
		pPar->IppEncStdParam.mp4Param = cnmcom->Prop_mpeg4Par;
	}else{
		pPar->IppEncStdParam.avcParam = cnmcom->Prop_h264Par;
	}

	/*C&M working mode*/
	pPar->bPollingMode = 0;
	pPar->bDynaClkEnable = cnmcom->Prop_bDynaClkEnable;
	pPar->bPingPangMode = 0;
	pPar->iMultiInstanceSync = cnmcom->Prop_iMultiInstanceSync;
	pPar->iInputYUVMode = 0;

	return 0;
}

static int Par2Prop(IppVPUEncParSet* pPar, IppGSTCnMEncCOM* cnmcom)
{
	trans_format_enc2gst(pPar, &cnmcom->PseudoProp_streamspec);
	cnmcom->Prop_iGopSize = pPar->iGopSize;
	cnmcom->Prop_iInitQp = pPar->iInitQp;
	cnmcom->Prop_bRCEnable = pPar->bRCEnable;
	cnmcom->Prop_iRCType = pPar->iRCType;
	if(pPar->iRCBitRate != scaleBps(cnmcom, cnmcom->Prop_iRCBitRate)) {
		//pPar->iRCBitRate is changed, need update Prop_iRCBitRate
		cnmcom->Prop_iRCBitRate = revertScaleBps(cnmcom, pPar->iRCBitRate);
	}
	if(pPar->iRCMaxBitRate != scaleBps(cnmcom, cnmcom->Prop_iRCMaxBitRate)) {
		//pPar->iRCMaxBitRate is changed, need update Prop_iRCMaxBitRate
		cnmcom->Prop_iRCMaxBitRate = revertScaleBps(cnmcom, pPar->iRCMaxBitRate);
	}
	if(cnmcom->Prop_iRCBitRate > IPPGSTCNMENC_BITRATE_MAX) {
		cnmcom->Prop_iRCBitRate = IPPGSTCNMENC_BITRATE_MAX;
	}
	if(cnmcom->Prop_iRCMaxBitRate > IPPGSTCNMENC_MAXBITRATE_MAX) {
		cnmcom->Prop_iRCMaxBitRate = IPPGSTCNMENC_MAXBITRATE_MAX;
	}
	cnmcom->Prop_iDelayLimit = pPar->iDelayLimit;

	if(cnmcom->PseudoProp_streamspec == IPPGSTCNMENC_STMFORMAT_H263) {
		cnmcom->Prop_h263Par = pPar->IppEncStdParam.h263Param;
	}else if(cnmcom->PseudoProp_streamspec == IPPGSTCNMENC_STMFORMAT_MPEG4) {
		cnmcom->Prop_mpeg4Par = pPar->IppEncStdParam.mp4Param;
	}else{
		cnmcom->Prop_h264Par = pPar->IppEncStdParam.avcParam;
	}

	/*C&M working mode*/
	cnmcom->Prop_bDynaClkEnable = pPar->bDynaClkEnable;
	cnmcom->Prop_iMultiInstanceSync = pPar->iMultiInstanceSync;

	return 0;
}

static GstFlowReturn _cnmenc_push_data(IppGSTCnMEncCOM* cnmcom, IppVPUBitstream * pBs, GstClockTime* pTS)
{
	GstFlowReturn FlowRt = GST_FLOW_OK;
	GstBuffer *outBuf = NULL;
	unsigned char* pDst;
	int downBufSz;

	if(cnmcom->SeqHdrLen != 0 && (pBs->status&0x80000000)) {
		downBufSz = cnmcom->SeqHdrLen + pBs->bsByteLen;
	}else{
		downBufSz = pBs->bsByteLen;
	}

	//one frame is encoded, push data to down stream
	outBuf= gst_buffer_new_and_alloc(downBufSz);
	if(G_LIKELY(outBuf != NULL)) {
		pDst = GST_BUFFER_DATA(outBuf);
		if(cnmcom->SeqHdrLen != 0 && (pBs->status&0x80000000)) {
			memcpy(pDst, cnmcom->SeqHdrBuf, cnmcom->SeqHdrLen);
			pDst += cnmcom->SeqHdrLen;
			if(! cnmcom->Prop_bInsertSeqHdr) {
				//if Not enable inserting seq header feature, we only insert seq header before the 1st I frame. After that, free seq header data.
				g_free(cnmcom->SeqHdrBuf);
				cnmcom->SeqHdrBuf = NULL;
				cnmcom->SeqHdrLen = 0;
			}
		}
		memcpy(pDst, pBs->pBsBuffer, pBs->bsByteLen);
		GST_BUFFER_TIMESTAMP(outBuf) = *pTS;
		GST_BUFFER_DURATION(outBuf) = cnmcom->u64FrameFixedDuration;
		gst_buffer_set_caps(outBuf, GST_PAD_CAPS(cnmcom->srcpad));
	}else{
		IPPPSEUDO_GST_ERROR_OBJECT(cnmcom, "Allocate push down buffer fail(want size %d), will stop encoding!", cnmcom->SeqHdrLen + pBs->bsByteLen);
		return GST_FLOW_ERROR;
	}

	FlowRt = gst_pad_push(cnmcom->srcpad, outBuf);

	return FlowRt;
}

static int _alloc_outputbsmem(IppGSTCnMEncCOM* cnmcom)
{
	gcnm_printf("%s() is called\n", __FUNCTION__);

	cnmcom->StmBufMem.iSize = ((cnmcom->iWidth*cnmcom->iHeight*3>>1) + 1023)&(~1023);//allocate a big enough buffer
	cnmcom->StmBufMem.iAlign = 8;
	if(0 != cnm_os_api_pcontmem_alloc(&cnmcom->StmBufMem)){
		cnmcom->StmBufMem.iSize = 0;
		cnmcom->StmBufMem.virAddr = 0;
		cnmcom->StmBufMem.phyAddr = 0;
		return -1;
	}

	cnmcom->BsBufTemplate.pBsRead = cnmcom->BsBufTemplate.pBsWrite = cnmcom->BsBufTemplate.pBsBuffer = cnmcom->StmBufMem.virAddr;
	cnmcom->BsBufTemplate.bsByteLen = 0;
	cnmcom->BsBufTemplate.bsBufSize = cnmcom->StmBufMem.iSize;
	cnmcom->BsBufTemplate.phyAddr = cnmcom->StmBufMem.phyAddr;
	cnmcom->BsBufTemplate.bsByteAvail = 0;
	cnmcom->BsBufTemplate.status = 0;
	cnmcom->BsBufTemplate.idx = 0;

	return 0;
}

static __inline int _alloc_inputyuvmem(PhyContMemInfo* pMem, int sz)
{
	gcnm_printfmore("%s() is called, sz %d\n", __FUNCTION__, sz);
	pMem->iSize = sz;
	pMem->iAlign = 8;
	if(0 != cnm_os_api_pcontmem_alloc(pMem)){
		pMem->iSize = 0;
		pMem->virAddr = 0;
		pMem->phyAddr = 0;
		return -1;
	}
	return 0;
}


static void set_cnmenc_inputpic(IppVPUPicture* p, unsigned int phyAddr, unsigned char* virAddr, IppGSTCnMEncCOM* cnmcom)
{
	p->phyLuma = phyAddr;
	p->phyCb   = phyAddr + cnmcom->iUoffset;
	p->phyCr   = phyAddr + cnmcom->iVoffset;

	p->pic.ppPicPlane[0] = virAddr ;
	p->pic.ppPicPlane[1] = virAddr + cnmcom->iUoffset;
	p->pic.ppPicPlane[2] = virAddr + cnmcom->iVoffset;
	p->pic.picWidth  = cnmcom->iWidth;
	p->pic.picHeight = cnmcom->iHeight;
	p->pic.picPlaneStep[0] = cnmcom->iWidth;
	p->pic.picPlaneStep[1] = cnmcom->iWidth>>1;
	p->pic.picPlaneStep[2] = cnmcom->iWidth>>1;
	p->pic.picStatus = 0;
	return;
}

//Because cnm encoder are working under "one frame in, one stream out" mode currently(in other word, no delay mode), the TS queue mechanism could be simplified in fact.
static GstClockTime IPP_PopTSFromQueue(IppGSTCnMEncCOM* cnmcom)
{
	GstClockTime* pTs = (GstClockTime*)g_queue_pop_tail(&cnmcom->StmTSCandidateQueue);
	GstClockTime ts;
	if(pTs == NULL) {
		IPPPSEUDO_GST_WARNING_OBJECT(cnmcom, "No timestamp left in TS queue could be pop!");
		return GST_CLOCK_TIME_NONE;
	}else{
		ts = *pTs;
		g_slice_free(GstClockTime, pTs);
		return ts;
	}
}
#define IPPGST_CNMENC_TS_MAX_CAPACITY		1
static void IPP_PushTSToQueue(IppGSTCnMEncCOM* cnmcom, GstClockTime ts)
{
	GstClockTime* pTs = g_slice_new(GstClockTime);
	*pTs = ts;
	g_queue_push_head(&cnmcom->StmTSCandidateQueue, pTs);

	//remove redundant TS to avoid queue overflow
	if(g_queue_get_length(&cnmcom->StmTSCandidateQueue) > IPPGST_CNMENC_TS_MAX_CAPACITY) {
		GstClockTime* pRedundantTs = (GstClockTime*)g_queue_pop_tail(&cnmcom->StmTSCandidateQueue);
		g_slice_free(GstClockTime, pRedundantTs);
	}
	return;
}

static int
IPP_PrepareFrameTSFromFileSrc(IppGSTCnMEncCOM* cnmcom, GstBuffer* buf, int* pLeftLen)
{
	int iSrcLen = *pLeftLen;
	unsigned char* p = GST_BUFFER_DATA(buf) + (GST_BUFFER_SIZE(buf)-*pLeftLen);
	int copylen;
	if(cnmcom->FrameBufMem.iSize == 0) {
		if(0 != _alloc_inputyuvmem(&cnmcom->FrameBufMem, cnmcom->iFrameSize)) {
			IPPPSEUDO_GST_ERROR_OBJECT(cnmcom, "allocate frame memory fail in %s()", __FUNCTION__);
			return -1;
		}
		cnmcom->iDataLen_FrameBufForFileSrc = 0;
	}
	copylen = cnmcom->iFrameSize - cnmcom->iDataLen_FrameBufForFileSrc;
	if(iSrcLen < copylen) {
		copylen = iSrcLen;
	}
	memcpy((unsigned char*)(cnmcom->FrameBufMem.virAddr) + cnmcom->iDataLen_FrameBufForFileSrc, p, copylen);
	cnmcom->iDataLen_FrameBufForFileSrc += copylen;
	*pLeftLen -= copylen;
	if(cnmcom->iDataLen_FrameBufForFileSrc == cnmcom->iFrameSize) {
		cnmcom->iDataLen_FrameBufForFileSrc = 0;
		IPP_PushTSToQueue(cnmcom, cnmcom->AutoIncTimestamp);
		if(cnmcom->u64FrameFixedDuration != GST_CLOCK_TIME_NONE && cnmcom->AutoIncTimestamp != GST_CLOCK_TIME_NONE) {
			cnmcom->AutoIncTimestamp += cnmcom->u64FrameFixedDuration;
		}else{
			cnmcom->AutoIncTimestamp = GST_CLOCK_TIME_NONE;
		}
		return 0;
	}
	return 1;
}

static __inline int
IPP_IsWholeFrameInputMode(IppGSTCnMEncCOM* cnmcom, GstBuffer* buf)
{
	if(G_LIKELY(cnmcom->iWholeFrameInputModeFlag != -1)) {
		return cnmcom->iWholeFrameInputModeFlag;
	}else{
		if((int)GST_BUFFER_SIZE(buf) == cnmcom->iFrameSize) {
			cnmcom->iWholeFrameInputModeFlag = 1;
			return 1;
		}else{
			cnmcom->iWholeFrameInputModeFlag = 0;
			return 0;
		}
	}
}

GstFlowReturn
_cnmenccom_chain(GstBuffer* buf, IppGSTCnMEncCOM* cnmcom, int* pbTerminate, int* pErrReason)
{
	GstFlowReturn FlowRt = GST_FLOW_OK;
	IppCodecStatus codecRt;
	IppVPUBitstream Bs;
	IppVPUPicture Pic;
	int	bOneFrameReady = 0;
	int err_reason = 0;
	int indata_leftlen;
	int direct_useframebuf = 0;
	int EncodedPicType = -1;

#ifdef DEBUG_PERFORMANCE
	struct timeval CNMEncAPI_clk0, CNMEncAPI_clk1;
#endif

	*pbTerminate = 0;
	*pErrReason = 0;
	memset(&Pic, 0, sizeof(Pic));

	if(cnmcom->iFrameSize == 0) {
		IPPPSEUDO_GST_ERROR_OBJECT(cnmcom, "encoder parameter is still unknown in chain()!");
		err_reason = 1;
		goto CNMENC_CHAIN_FATALERR;
	}
	if(cnmcom->pSrcCBTable == NULL) {
		int rt = miscInitGeneralCallbackTable(&cnmcom->pSrcCBTable);
		if(rt != 0) {
			IPPPSEUDO_GST_ERROR_OBJECT(cnmcom, "init misc table fail!");
			err_reason = 2;
			goto CNMENC_CHAIN_FATALERR;
		}
	}
	if(cnmcom->StmBufMem.iSize == 0) {
		int rt = _alloc_outputbsmem(cnmcom);
		if(rt != 0) {
			IPPPSEUDO_GST_ERROR_OBJECT(cnmcom, "allocate output bs memory fail, ret %d!", rt);
			err_reason = 3;
			goto CNMENC_CHAIN_FATALERR;
		}
	}

	if(cnmcom->cnmenc_handle == NULL) {
		IppVPUEncParSet Par;
		SetEncPar(&Par, cnmcom);
		Bs = cnmcom->BsBufTemplate;
		codecRt = EncoderInitAlloc_VPU(&Par, cnmcom->pSrcCBTable, &Bs, NULL/*external allocate*/, &cnmcom->cnmenc_handle);
		IPPPSEUDO_GST_DEBUG_OBJECT(cnmcom, "Call EncoderInitAlloc_VPU() ret %d", codecRt);
		if(codecRt != IPP_STATUS_NOERR) {
			cnmcom->cnmenc_handle = NULL;
			IPPPSEUDO_GST_ERROR_OBJECT(cnmcom, "init codec fail, ret %d", codecRt);
			err_reason = 4;
			goto CNMENC_CHAIN_FATALERR;
		}else{
			//EncoderInitAlloc_VPU() probably modify Par, therefore, should refresh plug-in property
			Par2Prop(&Par, cnmcom);
		}
		if(Bs.bsByteLen != 0) {
			if(cnmcom->SeqHdrLen != 0) {
				g_free(cnmcom->SeqHdrBuf);
				cnmcom->SeqHdrBuf = NULL;
				cnmcom->SeqHdrLen = 0;
			}
			cnmcom->SeqHdrBuf = g_malloc(Bs.bsByteLen);
			if(cnmcom->SeqHdrBuf == NULL) {
				IPPPSEUDO_GST_ERROR_OBJECT(cnmcom, "allocate stream buffer to hold seq hdr data fail!");
				err_reason = 5;
				goto CNMENC_CHAIN_FATALERR;
			}
			memcpy(cnmcom->SeqHdrBuf, Bs.pBsBuffer, Bs.bsByteLen);
			cnmcom->SeqHdrLen = Bs.bsByteLen;
		}

		if(cnmcom->Prop_ForceStartTimestamp != -1) {
			cnmcom->AutoIncTimestamp = cnmcom->Prop_ForceStartTimestamp;
		}else if(! IPP_IsWholeFrameInputMode(cnmcom, buf)) {
			cnmcom->AutoIncTimestamp = 0;
		}
	}

	indata_leftlen = GST_BUFFER_SIZE(buf);
	while(indata_leftlen > 0) {
		bOneFrameReady = 0;
		if(! IPP_IsWholeFrameInputMode(cnmcom, buf)) {
			//stream input
			int ret = IPP_PrepareFrameTSFromFileSrc(cnmcom, buf, &indata_leftlen);
			if(ret < 0) {
				IPPPSEUDO_GST_ERROR_OBJECT(cnmcom, "IPP_PrepareFrameTSFromFileSrc() fail, ret %d!", ret);
				err_reason = 6;
				goto CNMENC_CHAIN_FATALERR;
			}
			if(indata_leftlen == 0) {
				gst_buffer_unref(buf);
				buf = NULL;
			}
			if(ret == 0) {
				set_cnmenc_inputpic(&Pic, cnmcom->FrameBufMem.phyAddr, cnmcom->FrameBufMem.virAddr, cnmcom);
				bOneFrameReady = 1;
			}
		}else{
			//frame input
			if(cnmcom->Prop_ForceStartTimestamp != -1) {
				IPP_PushTSToQueue(cnmcom, cnmcom->AutoIncTimestamp);
				if(cnmcom->u64FrameFixedDuration != GST_CLOCK_TIME_NONE && cnmcom->AutoIncTimestamp != GST_CLOCK_TIME_NONE) {
					cnmcom->AutoIncTimestamp += cnmcom->u64FrameFixedDuration;
				}else{
					cnmcom->AutoIncTimestamp = GST_CLOCK_TIME_NONE;
				}
			}else{
				IPP_PushTSToQueue(cnmcom, GST_BUFFER_TIMESTAMP(buf));
			}
			int nPhyAddr;
			int gstbuftype = IPP_DistinguishGstBufFromUpstream(cnmcom, buf, &nPhyAddr);
			if( gstbuftype == IPPGSTBUFTYPE_DIRECTUSABLE_ENCPLUGIN_ALLOCATED || gstbuftype == IPPGSTBUFTYPE_DIRECTUSABLE_SRCPLUGIN_ALLOCATED) {
				direct_useframebuf = 1;
				set_cnmenc_inputpic(&Pic, nPhyAddr, GST_BUFFER_DATA(buf), cnmcom);
			}else{
				if(cnmcom->FrameBufMem.iSize == 0) {
					if(0 != _alloc_inputyuvmem(&cnmcom->FrameBufMem, cnmcom->iFrameSize)) {
						IPPPSEUDO_GST_ERROR_OBJECT(cnmcom, "allocate frame memory fail");
						err_reason = 7;
						goto CNMENC_CHAIN_FATALERR;
					}
				}
				memcpy(cnmcom->FrameBufMem.virAddr, GST_BUFFER_DATA(buf), cnmcom->iFrameSize);
				set_cnmenc_inputpic(&Pic, cnmcom->FrameBufMem.phyAddr, cnmcom->FrameBufMem.virAddr, cnmcom);
				gst_buffer_unref(buf);
				buf = NULL;
			}
			bOneFrameReady = 1;
			indata_leftlen = 0;
		}

		if(bOneFrameReady) {
			//means one input frame is ready
			Bs = cnmcom->BsBufTemplate;	//reset Bs
#ifdef DEBUG_PERFORMANCE
			gettimeofday(&CNMEncAPI_clk0, NULL);
#endif
			codecRt= EncodeFrame_VPU(&Bs, &Pic, cnmcom->cnmenc_handle);
#ifdef DEBUG_PERFORMANCE
			gettimeofday(&CNMEncAPI_clk1, NULL);
			cnmcom->codec_time += (CNMEncAPI_clk1.tv_sec - CNMEncAPI_clk0.tv_sec)*1000000 + (CNMEncAPI_clk1.tv_usec - CNMEncAPI_clk0.tv_usec);
#endif
			if(codecRt != IPP_STATUS_NOERR) {
				g_warning("EncodeFrame_VPU() fail, ret %d", codecRt);
				IPPPSEUDO_GST_ERROR_OBJECT(cnmcom, "EncodeFrame_VPU() fail, ret %d", codecRt);
				err_reason = 8;
				goto CNMENC_CHAIN_FATALERR;
			}

			EncodeSendCmd_VPU(IPPVC_GET_PICTURETYPE, NULL, &EncodedPicType, cnmcom->cnmenc_handle);
			if(EncodedPicType == 0) {
				//it's I picture
				Bs.status |= 0x80000000;
			}

			if(direct_useframebuf && buf) {
				gst_buffer_unref(buf);
				buf = NULL;
			}
			GstClockTime ts = IPP_PopTSFromQueue(cnmcom);
			if(Bs.bsByteLen > 0) {
				cnmcom->iEncProducedCompressedFrameCnt++;
				cnmcom->iEncProducedByteCnt += Bs.bsByteLen;
				FlowRt = _cnmenc_push_data(cnmcom, &Bs, &ts);
				if(FlowRt == GST_FLOW_ERROR) {
					err_reason = 9;
				}
			}
		}
	}


CNMENC_CHAIN_RET:
	if(buf) {
		gst_buffer_unref(buf);
		buf = NULL;
	}
	if(FlowRt < GST_FLOW_UNEXPECTED) {
		*pbTerminate = 1;
		*pErrReason = err_reason;
	}
	return FlowRt;

CNMENC_CHAIN_FATALERR:
	FlowRt = GST_FLOW_ERROR;
	goto CNMENC_CHAIN_RET;

}


static gboolean
cnmenccom_null2ready(IppGSTCnMEncCOM *cnmcom)
{
	cnmcom->AutoIncTimestamp = GST_CLOCK_TIME_NONE;
	cnmcom->iDataLen_FrameBufForFileSrc = 0;

	cnmcom->iEncProducedCompressedFrameCnt = 0;
	cnmcom->iEncProducedByteCnt = 0;
	cnmcom->codec_time = 0;
	return TRUE;
}


static int rel_resource(IppGSTCnMEncCOM *cnmcom)
{
	int rt = 0;
	/*Free Encoder*/
	if(cnmcom->cnmenc_handle) {
		IppCodecStatus codecRt = EncoderFree_VPU(&(cnmcom->cnmenc_handle));
		IPPPSEUDO_GST_DEBUG_OBJECT(cnmcom, "Call EncoderFree_VPU() ret %d", codecRt);
		if(codecRt != IPP_STATUS_NOERR) {
			rt |= 1;
			IPPPSEUDO_GST_ERROR_OBJECT(cnmcom, "ERROR: EncoderFree_VPU fail, ret %d", codecRt);
			gcnm_printf("ERROR: EncoderFree_VPU fail, ret %d\n", codecRt);
		}
		cnmcom->cnmenc_handle = NULL;
	}

	/*free callback*/
	if(cnmcom->pSrcCBTable){
		miscFreeGeneralCallbackTable(&(cnmcom->pSrcCBTable));
		cnmcom->pSrcCBTable = NULL;
	}

	if(cnmcom->StmBufMem.iSize != 0) {
		cnm_os_api_pcontmem_free(&cnmcom->StmBufMem);
		cnmcom->StmBufMem.iSize = 0;
	}

	if(cnmcom->FrameBufMem.iSize != 0) {
		cnm_os_api_pcontmem_free(&cnmcom->FrameBufMem);
		cnmcom->FrameBufMem.iSize = 0;
	}

	if(cnmcom->SeqHdrLen != 0) {
		g_free(cnmcom->SeqHdrBuf);
		cnmcom->SeqHdrBuf = NULL;
		cnmcom->SeqHdrLen = 0;
	}

	if(g_queue_get_length(&cnmcom->StmTSCandidateQueue) > 0) {
		GstClockTime* pRedundantTs = (GstClockTime*)g_queue_pop_tail(&cnmcom->StmTSCandidateQueue);
		g_slice_free(GstClockTime, pRedundantTs);
	}
	return rt;
}

static gboolean
cnmenccom_ready2null(IppGSTCnMEncCOM *cnmcom)
{
	rel_resource(cnmcom);

#ifdef DEBUG_PERFORMANCE
	printf("cnm encoder totally outputted %d frames(len %lld bytes), spend %lld us\n", cnmcom->iEncProducedCompressedFrameCnt, cnmcom->iEncProducedByteCnt, cnmcom->codec_time);
#endif

	return TRUE;
}



GstStateChangeReturn
_cnmenccom_change_state(GstElement *element, GstStateChange transition, IppGSTCnMEncCOM* cnmcom, GstElementClass* father_class)
{
	gcnm_printf("state change from %d to %d\n", transition>>3, transition&7);

	GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;

	switch (transition)
	{
	case GST_STATE_CHANGE_NULL_TO_READY:
		if(!cnmenccom_null2ready(cnmcom)){
			goto statechange_failed;
		}
		break;
	default:
		break;
	}

	ret = father_class->change_state (element, transition);
	if (ret == GST_STATE_CHANGE_FAILURE) {
		return ret;
	}

	switch (transition)
	{
	case GST_STATE_CHANGE_READY_TO_NULL:
		if(!cnmenccom_ready2null(cnmcom)){
			goto statechange_failed;
		}
		break;
	default:
		break;
	}

	return ret;

	/* ERRORS */
statechange_failed:
	{
		/* subclass must post a meaningfull error message */
		IPPPSEUDO_GST_ERROR_OBJECT (cnmcom, "state change failed");
		return GST_STATE_CHANGE_FAILURE;
	}
}


gboolean
_cnmenccom_sinkpad_event(GstPad *pad, GstEvent *event, IppGSTCnMEncCOM* cnmcom)
{
	gboolean eventRet;
	gcnm_printf("receive event %s on cnmcom sinkpad(%p).\n", GST_EVENT_TYPE_NAME(event), pad);

	switch(GST_EVENT_TYPE (event))
	{
	case GST_EVENT_EOS:
		rel_resource(cnmcom);
		eventRet = gst_pad_push_event(cnmcom->srcpad, event);
		break;
	default:
		eventRet = gst_pad_event_default(pad, event);
		break;
	}

	return eventRet;
}


gboolean
_cnmenccom_sinkpad_setcaps(GstPad *pad, GstCaps *caps, IppGSTCnMEncCOM* cnmcom)
{
	guint stru_num = gst_caps_get_size (caps);
	if(stru_num == 0) {
		g_warning("No content in vmetaenc sinkpad setcaps!");
		IPPPSEUDO_GST_ERROR_OBJECT(cnmcom, "No content in vmetaenc sinkpad setcaps!");
		return FALSE;
	}

	GstStructure *str = gst_caps_get_structure(caps, 0);

	if (FALSE == gst_structure_get_int(str, "width", &(cnmcom->iWidth))) {
		g_warning("Couldn't get src image width in setcaps()!");
		IPPPSEUDO_GST_ERROR_OBJECT(cnmcom, "Couldn't get src image width in setcaps()!");
		return FALSE;
	}

	if (FALSE == gst_structure_get_int(str, "height", &(cnmcom->iHeight))) {
		g_warning("Couldn't get src image height in setcaps()!");
		IPPPSEUDO_GST_ERROR_OBJECT(cnmcom, "Couldn't get src image height in setcaps()!");
		return FALSE;
	}

	if(((cnmcom->iWidth & (16-1)) != 0) || ((cnmcom->iHeight & (16-1)) != 0)) {
		g_warning("Couldn't support non 16 aligned video, w %d, h %d", cnmcom->iWidth, cnmcom->iHeight);
		IPPPSEUDO_GST_ERROR_OBJECT(cnmcom, "Couldn't support non 16 aligned video,  w %d, h %d", cnmcom->iWidth, cnmcom->iHeight);
		return FALSE;
	}

	int iFRateNum = 0, iFRateDen = 0;
	if (FALSE == gst_structure_get_fraction(str, "framerate", &iFRateNum, &iFRateDen)) {
		g_warning("Couldn't get framerate in setcaps()!");
		IPPPSEUDO_GST_ERROR_OBJECT(cnmcom, "Couldn't get framerate in setcaps()!");
		return FALSE;
	}
	if(iFRateNum <= 0 || iFRateDen <= 0) {
		g_warning("frame rate info from caps isn't correct, (%d)/(%d)!", iFRateNum, iFRateDen);
		IPPPSEUDO_GST_ERROR_OBJECT(cnmcom, "frame rate info from caps isn't correct, (%d)/(%d)!", iFRateNum, iFRateDen);
		return FALSE;
	}
	cnmcom->iRoundedFrameRate = iFRateNum/iFRateDen;
	if(cnmcom->iAllowFractionFps == 0 && iFRateDen * cnmcom->iRoundedFrameRate != iFRateNum) {	//current c&m encoder only support interger fps
		g_warning("Couldn't support fractional framerate (%d)/(%d) fps!", iFRateNum, iFRateDen);
		IPPPSEUDO_GST_ERROR_OBJECT(cnmcom, "Couldn't support fractional framerate (%d)/(%d) fps!", iFRateNum, iFRateDen);
		return FALSE;
	}
	if(cnmcom->iAllowFractionFps) {
		if(cnmcom->iRoundedFrameRate == 0) {
			cnmcom->iRoundedFrameRate = 1;
		}
	}
	IPPPSEUDO_GST_DEBUG_OBJECT(cnmcom, "The input fps is (%d)/(%d), rounded fps is %d", iFRateNum, iFRateDen, cnmcom->iRoundedFrameRate);
	cnmcom->iInputFpsNum = iFRateNum;
	cnmcom->iInputFpsDen = iFRateDen;
	if(cnmcom->iAllowFractionFps && iFRateDen * cnmcom->iRoundedFrameRate != iFRateNum) {
		cnmcom->iWillScaleBps = 1;
	}

	cnmcom->u64FrameFixedDuration = gst_util_uint64_scale_int(GST_SECOND, iFRateDen, iFRateNum);
	gcnm_printf("cnmcom->u64FrameFixedDuration is set %lld\n", cnmcom->u64FrameFixedDuration);

	GstCaps *TmpCap = NULL;
	if(cnmcom->PseudoProp_streamspec == IPPGSTCNMENC_STMFORMAT_H264) {
		TmpCap = gst_caps_new_simple ("video/x-h264",
		"width", G_TYPE_INT, cnmcom->iWidth,
		"height", G_TYPE_INT, cnmcom->iHeight,
		"framerate", GST_TYPE_FRACTION, iFRateNum, iFRateDen,
		NULL);
	}else if(cnmcom->PseudoProp_streamspec == IPPGSTCNMENC_STMFORMAT_H263) {
		TmpCap = gst_caps_new_simple ("video/x-h263",
			"width", G_TYPE_INT, cnmcom->iWidth,
			"height", G_TYPE_INT, cnmcom->iHeight,
			"framerate", GST_TYPE_FRACTION, iFRateNum, iFRateDen,
			NULL);
	}else if(cnmcom->PseudoProp_streamspec == IPPGSTCNMENC_STMFORMAT_MPEG4) {
		TmpCap = gst_caps_new_simple ("video/mpeg",
			"width", G_TYPE_INT, cnmcom->iWidth,
			"height", G_TYPE_INT, cnmcom->iHeight,
			"mpegversion", G_TYPE_INT, 4,
			"systemstream", G_TYPE_BOOLEAN, FALSE,
			"framerate", GST_TYPE_FRACTION, iFRateNum, iFRateDen,
			NULL);
	}
	if(!gst_pad_set_caps(cnmcom->srcpad, TmpCap)) {
		gst_caps_unref(TmpCap);
		g_warning("Set caps on srcpad fail!");
		IPPPSEUDO_GST_ERROR_OBJECT(cnmcom, "Set caps on srcpad fail!");
		return FALSE;
	}
	gst_caps_unref(TmpCap);

	cnmcom->iFrameSize = cnmcom->iWidth * cnmcom->iHeight * 3 >> 1;
	cnmcom->iUoffset = cnmcom->iWidth * cnmcom->iHeight;
	cnmcom->iVoffset = cnmcom->iUoffset + (cnmcom->iWidth * cnmcom->iHeight>>2);
	return TRUE;
}


int
_cnmenccom_set_com_prop(GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec, IppGSTCnMEncCOM* cnmcom)
{
	int rt = 0;
	switch (prop_id)
	{
	case ARG_GOPSIZE:
		{
			int iVal = g_value_get_int(value);
			if( iVal<0 ) {
				IPPPSEUDO_GST_ERROR_OBJECT(cnmcom, "gopsize %d exceed range.", iVal);
				iVal= 0;
			}
			cnmcom->Prop_iGopSize = iVal;
		}
		break;
	case ARG_INITQP:
		{
			int iVal = g_value_get_int(value);
			if( (iVal<IPPGSTCNMENC_INITQP_MIN) || (iVal>IPPGSTCNMENC_INITQP_MAX) ) {
				IPPPSEUDO_GST_ERROR_OBJECT(cnmcom, "initqp %d exceed range.", iVal);
				iVal = IPPGSTCNMENC_INITQP_DEF;
			}
			cnmcom->Prop_iInitQp = iVal;
		}
		break;
	case ARG_RCENABLE:
		{
			int iVal = g_value_get_int(value);
			if( (iVal!=0) && (iVal!=1) ) {
				IPPPSEUDO_GST_ERROR_OBJECT(cnmcom, "rc-enable %d exceed range.", iVal);
				iVal= 1;
			}
			cnmcom->Prop_bRCEnable = iVal;
		}
		break;
	case ARG_RCTYPE:
		{
			int iVal = g_value_get_int(value);
			if( (iVal<0) || (iVal>2) ) {
				IPPPSEUDO_GST_ERROR_OBJECT(cnmcom, "rc-type %d not supported.", iVal);
				iVal= 0;
			}
			cnmcom->Prop_iRCType = iVal;
		}
		break;
	case ARG_RCBITRATE:
		{
			int iVal = g_value_get_int(value);
			if( (iVal<0) || (iVal>IPPGSTCNMENC_BITRATE_MAX) ) {
				IPPPSEUDO_GST_ERROR_OBJECT(cnmcom, "rc-bitrate %d exceed range.", iVal);
				iVal= IPPGSTCNMENC_BITRATE_DEF;
			}
			cnmcom->Prop_iRCBitRate = iVal;
		}
		break;
	case ARG_RCMAXBITRATE:
		{
			int iVal = g_value_get_int(value);
			if( (iVal<0) || (iVal>(IPPGSTCNMENC_MAXBITRATE_MAX)) ) {
				IPPPSEUDO_GST_ERROR_OBJECT(cnmcom, "rc-maxbitrate %d exceed range.", iVal);
				iVal= IPPGSTCNMENC_MAXBITRATE_DEF;
			}
			cnmcom->Prop_iRCMaxBitRate = iVal;
		}
		break;
	case ARG_DELAYLIMIT:
		{
			int iVal = g_value_get_int(value);
			if( (iVal<IPPGSTCNMENC_DELAYLIMIT_MIN) || (iVal>IPPGSTCNMENC_DELAYLIMIT_MAX) ) {
				IPPPSEUDO_GST_ERROR_OBJECT(cnmcom, "rc-delaylimit %d exceed range.", iVal);
				iVal= IPPGSTCNMENC_DELAYLIMIT_DEF;
			}
			cnmcom->Prop_iDelayLimit = iVal;
		}
		break;
	case ARG_SUPPORTMULTIINS:
		{
			int iVal = g_value_get_int(value);
			if( (iVal!=0) && (iVal!=1) ) {
				IPPPSEUDO_GST_ERROR_OBJECT(cnmcom, "supportMultiIns %d exceed range.", iVal);
				iVal= 1;
			}
			cnmcom->Prop_iMultiInstanceSync = iVal;
		}
		break;
	case ARG_ENABLEDYNACLK:
		{
			int iVal = g_value_get_int(value);
			if( (iVal!=0) && (iVal!=1) ) {
				IPPPSEUDO_GST_ERROR_OBJECT(cnmcom, "dynamicClock %d exceed range.", iVal);
				iVal= 1;
			}
			cnmcom->Prop_bDynaClkEnable= iVal;
		}
		break;
	case ARG_FORCE_STARTTIME:
		{
			gint64 st = g_value_get_int64(value);
			if(st<(gint64)(-1)) {
				IPPPSEUDO_GST_ERROR_OBJECT(cnmcom, "force-starttime %lld exceed range!", st);
				st = -1;
			}
			cnmcom->Prop_ForceStartTimestamp = st;
		}
		break;
	default:
		rt = -1;
		break;
	}
	return rt;
}

int
_cnmenccom_get_prop(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec, IppGSTCnMEncCOM* cnmcom)
{
	int rt = 0;
	switch (prop_id)
	{
	case ARG_GOPSIZE:
		g_value_set_int(value, cnmcom->Prop_iGopSize);
		break;
	case ARG_INITQP:
		g_value_set_int(value, cnmcom->Prop_iInitQp);
		break;
	case ARG_RCENABLE:
		g_value_set_int(value, cnmcom->Prop_bRCEnable);
		break;
	case ARG_RCTYPE:
		g_value_set_int(value, cnmcom->Prop_iRCType);
		break;
	case ARG_RCBITRATE:
		g_value_set_int(value, cnmcom->Prop_iRCBitRate);
		break;
	case ARG_RCMAXBITRATE:
		g_value_set_int(value, cnmcom->Prop_iRCMaxBitRate);
		break;
	case ARG_DELAYLIMIT:
		g_value_set_int(value, cnmcom->Prop_iDelayLimit);
		break;
	case ARG_SUPPORTMULTIINS:
		g_value_set_int(value, cnmcom->Prop_iMultiInstanceSync);
		break;
	case ARG_ENABLEDYNACLK:
		g_value_set_int(value, cnmcom->Prop_bDynaClkEnable);
		break;
	case ARG_FORCE_STARTTIME:
		g_value_set_int64(value, cnmcom->Prop_ForceStartTimestamp);
		break;

#ifdef DEBUG_PERFORMANCE
	case ARG_TOTALFRAME:
		g_value_set_int(value, cnmcom->iEncProducedCompressedFrameCnt);
		break;
	case ARG_CODECTIME:
		g_value_set_int64(value, cnmcom->codec_time);
		break;
#endif

	default:
		rt = -1;
		break;
	}
	return rt;
}

void _cnmenccom_init_members(IppGSTCnMEncCOM* cnmcom, int ippgstcnmenc_stmfmt, int initqp, void* gst_element, pfun_PSEUDO_GST_XXX_OBJECT echofun, GstPad* sinkpad, GstPad* srcpad)
{
	cnmcom->cnmenc_handle = NULL;
	cnmcom->pSrcCBTable= NULL;
	memset(&cnmcom->StmBufMem, 0, sizeof(cnmcom->StmBufMem));
	memset(&cnmcom->FrameBufMem, 0, sizeof(cnmcom->FrameBufMem));
	memset(&cnmcom->BsBufTemplate, 0, sizeof(cnmcom->BsBufTemplate));
	cnmcom->iDataLen_FrameBufForFileSrc = 0;
	cnmcom->SeqHdrBuf = NULL;
	cnmcom->SeqHdrLen = 0;
	cnmcom->iWholeFrameInputModeFlag = -1;
	cnmcom->iWidth = 0;
	cnmcom->iHeight = 0;
	cnmcom->iFrameSize = 0;
	cnmcom->iUoffset = 0;
	cnmcom->iVoffset = 0;
	cnmcom->iAllowFractionFps = 0;
	cnmcom->iWillScaleBps = 0;
	cnmcom->iInputFpsNum = 0;
	cnmcom->iInputFpsDen = 0;
	cnmcom->iRoundedFrameRate = 0;
	cnmcom->u64FrameFixedDuration = GST_CLOCK_TIME_NONE;
	g_queue_init(&cnmcom->StmTSCandidateQueue);
	cnmcom->AutoIncTimestamp = GST_CLOCK_TIME_NONE;

	cnmcom->PseudoProp_streamspec = ippgstcnmenc_stmfmt;
	cnmcom->Prop_iGopSize = 25;
	cnmcom->Prop_iInitQp = initqp;
	cnmcom->Prop_bRCEnable = 1;
	cnmcom->Prop_iRCType = 0;
	cnmcom->Prop_iRCBitRate = IPPGSTCNMENC_BITRATE_DEF;
	cnmcom->Prop_iRCMaxBitRate = IPPGSTCNMENC_MAXBITRATE_DEF;
	cnmcom->Prop_iDelayLimit = IPPGSTCNMENC_DELAYLIMIT_DEF;
	cnmcom->Prop_h263Par.h263_annexJEnable = 0;
	cnmcom->Prop_h263Par.h263_annexKEnable = 0;
	cnmcom->Prop_h263Par.h263_annexTEnable = 0;
	cnmcom->Prop_mpeg4Par.mp4_dataPartitionEnable = 0;
	cnmcom->Prop_mpeg4Par.mp4_reversibleVlcEnable = 0;
	cnmcom->Prop_mpeg4Par.mp4_intraDcVlcThr = 0;
	cnmcom->Prop_mpeg4Par.mp4_hecEnable = 0;
	cnmcom->Prop_mpeg4Par.mp4_verid = 1;
	cnmcom->Prop_h264Par.avc_bDeblockDisable = 0;
	cnmcom->Prop_iMultiInstanceSync = 1;
	cnmcom->Prop_bDynaClkEnable = 1;
	cnmcom->Prop_ForceStartTimestamp = -1;
	cnmcom->Prop_bInsertSeqHdr = 0;

	cnmcom->iEncProducedCompressedFrameCnt = 0;
	cnmcom->iEncProducedByteCnt = 0;
	cnmcom->codec_time = 0;

	cnmcom->gst_ele = gst_element;
	cnmcom->GSTobj_echo_fun = echofun;
	cnmcom->sinkpad = sinkpad;
	cnmcom->srcpad = srcpad;

	return;
}

GstFlowReturn
_cnmenccom_sinkpad_allocbuf(GstPad *pad, guint64 offset, guint size, GstCaps *caps, GstBuffer **buf, IppGSTCnMEncCOM* cnmcom)
{
	gcnm_printfmore("call %s(), pad(%p), offset(%lld), size(%d)\n", __FUNCTION__, pad, offset, size);
	int realsz = size;
	if(realsz < cnmcom->iFrameSize) {
		realsz = cnmcom->iFrameSize;
	}
	PhyContMemInfo Mem;
	memset(&Mem, 0, sizeof(Mem));
	if(_alloc_inputyuvmem(&Mem, realsz) != 0) {
		IPPPSEUDO_GST_WARNING_OBJECT(cnmcom, "Couldn't allocate physical continue memory(size %d), will allcoate common buf", size);
		*buf = NULL;
		return GST_FLOW_OK;
	}
	GstCNMEncFrameBuffer* CNMEncBuf = gst_CNMEncBuffer_new();
	if(CNMEncBuf == NULL) {
		IPPPSEUDO_GST_WARNING_OBJECT(cnmcom, "Couldn't new GstCNMEncFrameBuffer structure, will allcoate common buf");
		cnm_os_api_pcontmem_free(&Mem);
		Mem.iSize = 0;
		*buf = NULL;
		return GST_FLOW_OK;
	}

	CNMEncBuf->phyMem = Mem;
	GST_BUFFER_DATA(CNMEncBuf) = Mem.virAddr;
	GST_BUFFER_SIZE(CNMEncBuf) = size;
	GST_BUFFER_OFFSET(CNMEncBuf) = offset;
	*buf = (GstBuffer*)CNMEncBuf;
	gst_buffer_set_caps(*buf, caps);
	return GST_FLOW_OK;
}


void _cnmenccom_class_install_com_prop(GObjectClass* gobject_class)
{
	g_object_class_install_property (gobject_class, ARG_GOPSIZE, \
		g_param_spec_int ("gopsize", "The GOP size of encoded stream", \
		"Only valid when rc-enable==0 and just a suggestion to encoder. It indicates the number of P frames between I frame, could be 0<Only first frame is I, others are P>, 1<all I frame>, 2<IPIP...>, 3<IPPIPP...>, ...", \
		0/* range_MIN */, G_MAXINT/* range_MAX */, 25/* default_INIT */, G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, ARG_INITQP, \
		g_param_spec_int ("initqp", "The init QP of encoded stream", \
		"when bRCEnable=0 it is valid for all frame, otherwise when bRCEnable=1, it is ignored, first QP is selected internally. Recommended value: 35 for H.264, 15 for H.263 and MPEG-4", \
		IPPGSTCNMENC_INITQP_MIN/* range_MIN */, IPPGSTCNMENC_INITQP_MAX/* range_MAX */, IPPGSTCNMENC_INITQP_DEF/* default_INIT */, G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, ARG_RCENABLE, \
		g_param_spec_int ("rc-enable", "enable rate control", \
		"Enable encoder rate control mechanism.", \
		0/* range_MIN */, 1/* range_MAX */, 1/* default_INIT */, G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, ARG_RCTYPE, \
		g_param_spec_int ("rc-type", "rate control type for different usage scenarios", \
		"Only valid when rate control is enabled(rc-enable==1). 0 <No frame skipping; for file compression and transcoding scenarios>, 1 <Frame skipping for large amounts of motion; for camcorder scenarios>, 2 <Frame skipping for buffer overflow, large amounts of motion, and maximum bit rate constraint; for video telephony scenarios>", \
		0/* range_MIN */, 2/* range_MAX */, 0/* default_INIT */, G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, ARG_RCBITRATE, \
		g_param_spec_int ("rc-bitrate", "bit rate(bits/second)", \
		"Target bitrate of encoded bitstream, only valid when rate control is enabled(rc-enable==1). Just wanted bitrate, not the real bitrate of compressed data.", \
		0/* range_MIN */, IPPGSTCNMENC_BITRATE_MAX/*20M, range_MAX */, IPPGSTCNMENC_BITRATE_DEF/* default_INIT */, G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, ARG_RCMAXBITRATE, \
		g_param_spec_int ("rc-maxbitrate", "max bit rate(bits/second)", \
		"Max target bitrate. Only valid when rate control is enabled(rc-enable==1) and (rc-type== 2), should >= 1.5*rc-bitrate.", \
		0/* range_MIN */, IPPGSTCNMENC_MAXBITRATE_MAX/*30M, range_MAX */, IPPGSTCNMENC_MAXBITRATE_DEF/* default_INIT */, G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, ARG_DELAYLIMIT, \
		g_param_spec_int ("rc-delaylimit", "initial delay(milliseconds)", \
		"Time delay (in mili-seconds) to reach initial occupancy. Only valid when rate control is enabled(rc-enable==1).", \
		IPPGSTCNMENC_DELAYLIMIT_MIN/* range_MIN */, IPPGSTCNMENC_DELAYLIMIT_MAX/*30M, range_MAX */, IPPGSTCNMENC_DELAYLIMIT_DEF/* default_INIT */, G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, ARG_SUPPORTMULTIINS, \
		g_param_spec_int ("supportMultiIns", "support multiple instance", \
		"Enable codec internal sync mechanism to support multiple instance. Could be 0<not enable>, or 1<support multi-instance>, or 2<reserved>. Don't access this property unless you are clear what you are doing.", \
		0/* range_MIN */, 1/* range_MAX */, 1/* default_INIT */, G_PARAM_READWRITE));
	g_object_class_install_property (gobject_class, ARG_ENABLEDYNACLK, \
		g_param_spec_int ("dynamicClock", "let cnm hardware dynamic close clock", \
		"Let cnm hardware dynamically close clock. Could be 0<not dynamic close clock>, or 1<dynamic close clock>.", \
		0/* range_MIN */, 1/* range_MAX */, 1/* default_INIT */, G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, ARG_FORCE_STARTTIME, \
		g_param_spec_int64 ("force-starttime", "The first frame's timestamp", \
		"Set the first compressed frame's timestamp(unit is nanoseconds). If this value is -1, adopt input frame's timestamp. Otherwise, ignore all timestamp attached on input frame buffer and produce following timestamps according to frame rate.", \
		-1/* range_MIN */, G_MAXINT64/* range_MAX */, -1/* default_INIT */, G_PARAM_READWRITE));


#ifdef DEBUG_PERFORMANCE
	g_object_class_install_property (gobject_class, ARG_TOTALFRAME,
		g_param_spec_int ("totalframes", "Totalframe of cnm encoder outputted",
		"Number of frame of cnm encoder outputted", 0, G_MAXINT, 0, G_PARAM_READABLE));
	g_object_class_install_property (gobject_class, ARG_CODECTIME,
		g_param_spec_int64 ("codectime", "cnm encode time",
		"Pure codec time of calling cnm encoding API(microsecond)", 0, G_MAXINT64, 0, G_PARAM_READABLE));
#endif
	return;
}
