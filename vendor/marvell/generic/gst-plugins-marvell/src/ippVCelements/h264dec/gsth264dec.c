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


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <string.h> //to include memcpy, memset()
#include <dlfcn.h>
#include <unistd.h>
#include "gsth264dec.h"
#include "ippGSTdefs.h"

#ifdef DEBUG_PERFORMANCE
#include <sys/time.h>
#include <time.h>
#endif

#define BMMBINARY_NAME "libbmm.so"
//#define BMMBINARY_NAME "libbmmpesudo.so"	//only for debug

//#include "bmm_lib.h"	//to include BMM_ATTR_DEFAULT
#ifndef BMM_ATTR_DEFAULT
#define BMM_ATTR_DEFAULT	0
#endif
#define BMMBUFFER_ATTR_FORGST	BMM_ATTR_DEFAULT



GST_DEBUG_CATEGORY_STATIC (h264dec_debug);
#define GST_CAT_DEFAULT h264dec_debug

/******************************************************************************
// IPP H.264 decoder related modules
******************************************************************************/

typedef struct _TsDuPair {
	GstClockTime	ts;
	GstClockTime	du;
} TsDuPair;

typedef struct _FrameWaitPaint {
	IppPicture*	pPic;
	int			NominalPoc;
	TsDuPair*	pTsDu;
} FrameWaitPaint;


#define GSTH264DEC_PROFILE_UNKNOWN		0
#define GSTH264DEC_PROFILE_BASELINE		1


#if 0
#define assist_myecho(...)	printf(__VA_ARGS__)
#else
#define assist_myecho(...)
#endif

//#define DUMP_TS_FILE	"OutTs.txt"
#ifdef DUMP_TS_FILE
static void * fpOutTs = NULL;
#endif

//#define USE_ATOM_FOR_QOS	//it seems there is no difference between using atomic operation or not
#ifdef USE_ATOM_FOR_QOS
#define GET_INT32_NominalAtomic(x)		g_atomic_int_get(&(x))
#define SET_INT32_NominalAtomic(x, val)	g_atomic_int_set(&(x), val)
#define INC_INT32_NominalAtomic(x)		g_atomic_int_inc(&(x))
#else
#define GET_INT32_NominalAtomic(x)		(x)
#define SET_INT32_NominalAtomic(x, val)	((x)=(val))
#define INC_INT32_NominalAtomic(x)		((x)++)
#endif

#define SMARTSKIPMODE_NORMALDECODE					0
#define SMARTSKIPMODE_PERFORMANCE_PREFER_LITTLE		1		//decoder do fast deblock
#define SMARTSKIPMODE_PERFORMANCE_PREFER_MID		2		//non deblock
#define SMARTSKIPMODE_PERFORMANCE_PREFER_BIG		3		//decoder drop 1/2 non-reference frame
#define SMARTSKIPMODE_PERFORMANCE_PREFER_LARGE		4		//decoder drop all non-reference frame
#define SMARTSKIPMODE_PERFORMANCE_PREFER_MOST		5		//gst rough skip some compressed data fragment

#define SHORTWND_QOSWATCHLEN_ADJUST				3
#define LONGWND_QOSWATCHLEN_ADJUST				1
#define PERFORMANCE_LACK						1
#define PERFORMANCE_LACK_MUCH					DROP_REC_SHORTWND_LEN
#define PERFORMANCE_RICH						0
#define ROUGH_SKIPDATA_PERIOD_MIN				2
#define ROUGH_SKIPDATA_PERIOD_MAX				4

#define QOSLOG_PROTECT_FORNEWSEG				3

/******************************************************************************
// IPP-powered GStreamer H.264 Decode Plug-in Implementations
******************************************************************************/

enum {

	/* fill above with user defined signals */
    LAST_SIGNAL
};

enum {
    ARG_0,

	/* fill below with user defined arguments */
	ARG_MORE_SINKBUFFER,
	ARG_REORDERDELAY,
	ARG_FORBID_SI42,
	ARG_COPYSTREAMBUF,
	ARG_NOTREORDERBASELINE,

	/* frame skip strategy during the decoding */
	ARG_SKIP_STRATEGY,
	ARG_FORBID_ROUGHSKIP,
	ARG_SKIP_LOGFILE,

#ifdef DEBUG_PERFORMANCE
	ARG_TOTALFRAME,
	ARG_CODECTIME,
#endif
};

#define GSTH264DEC_SKIPSTRATEGY_MIN		0
#define GSTH264DEC_SKIPSTRATEGY_MAX		6

#define GSTH264DEC_REORDERDELAY_UNDECIDED	17

#define DOWNBUF_TYPE_UNDECIDED	0
#define DOWNBUF_TYPE_I420		1
#define DOWNBUF_TYPE_sI42		2

#define GST_IPPH264DEC_SELFSTREAMBUF_MINSZ	(4*1024)

//#define DEBUG_PERFORMANCE_GETMEMCPYTIME
#ifdef DEBUG_PERFORMANCE_GETMEMCPYTIME
static int memcpy_time = 0;
#endif

#ifndef DEBUG_DOWN_IS_YV12	//DEBUG_DOWN_IS_YV12 is only defined when debuging, it change I420 to YV12
#define H264DEC_4CC_I420STR	"I420"
#else
#define H264DEC_4CC_I420STR	"YV12"
#endif
#define H264DEC_4CC_sI42STR	"sI42"


static GstStaticPadTemplate sink_factory = 
	GST_STATIC_PAD_TEMPLATE ("sink", GST_PAD_SINK, GST_PAD_ALWAYS, 
	GST_STATIC_CAPS ("video/x-h264, " 
					 "width = (int) [ 16, 2048 ], " 
					 "height = (int) [ 16, 2048 ], " 
					 "framerate = (fraction) [ 0, MAX ]")
					 );

static GstStaticPadTemplate src_factory = 
	GST_STATIC_PAD_TEMPLATE ("src", GST_PAD_SRC, GST_PAD_ALWAYS, 
	GST_STATIC_CAPS ("video/x-raw-yuv," 
					 "format = (fourcc) { " H264DEC_4CC_I420STR ", " H264DEC_4CC_sI42STR " }, " 
					 "width = (int) [ 16, 2048 ], " 
					 "height = (int) [ 16, 2048 ], " 
					 "framerate = (fraction) [ 0, MAX ]")
					 );

GST_BOILERPLATE (GstH264Dec, gst_h264dec, GstElement, GST_TYPE_ELEMENT);

//a subclass from GstBuffer to hook GstBuffer finalize
typedef struct {
	GstBuffer buffer;
}GstVDECFrameBuffer;

#define GST_TYPE_VDEC_BUFFER (gst_vdec_buffer_get_type())
#define GST_IS_VDEC_BUFFER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_VDEC_BUFFER))
#define GST_VDEC_BUFFER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GST_TYPE_VDEC_BUFFER, GstVDECFrameBuffer))
#define GST_VDEC_BUFFER_CAST(obj) ((GstVDECFrameBuffer *)(obj))

static void gst_vdec_buffer_class_init (gpointer g_class, gpointer class_data);

static void gst_vdec_buffer_init (GTypeInstance * instance, gpointer g_class);

GType
gst_vdec_buffer_get_type(void)
{
	static GType vdec_buffer_type;

	if (G_UNLIKELY (vdec_buffer_type == 0)) {
		static const GTypeInfo vdec_buffer_info = {
		sizeof(GstBufferClass),
		NULL,
		NULL,
		gst_vdec_buffer_class_init,
		NULL,
		NULL,
		sizeof(GstVDECFrameBuffer),
		0,
		gst_vdec_buffer_init,
		NULL
	};
	vdec_buffer_type = g_type_register_static (GST_TYPE_BUFFER, "GstVDECFrameBuffer", &vdec_buffer_info, 0);
	}
	return vdec_buffer_type;
}

static GstMiniObjectClass *vdec_buffer_parent_class = NULL;
static void gst_vdec_buffer_finalize (GstVDECFrameBuffer * vdec_buffer);

static void
gst_vdec_buffer_class_init (gpointer g_class, gpointer class_data)
{
	GstMiniObjectClass *mini_object_class = GST_MINI_OBJECT_CLASS (g_class);

	vdec_buffer_parent_class = g_type_class_peek_parent (g_class);

	mini_object_class->finalize = (GstMiniObjectFinalizeFunction)gst_vdec_buffer_finalize;

	return;
}


static void
gst_vdec_buffer_init (GTypeInstance * instance, gpointer g_class)
{
	return;
}

static void gst_h264dec_finalize_res(GstH264Dec *h264dec);

static void
gst_vdec_buffer_finalize (GstVDECFrameBuffer * vdec_buffer)
{
	if(GST_BUFFER_DATA(vdec_buffer) == NULL) {
		//this is ResFinalizer's finalize
		GstH264Dec* h264dec = (GstH264Dec*)IPPGST_BUFFER_CUSTOMDATA(vdec_buffer);
		gst_h264dec_finalize_res(h264dec);
		h264dec->ResFinalizer = NULL;
		gst_object_unref(h264dec);	//since the resource is released, release h264dec
		return;
	}

	GST_sI42Buf_SpecialInfo* sI42Info = (GST_sI42Buf_SpecialInfo*) IPPGST_BUFFER_CUSTOMDATA(vdec_buffer);
	((IppPicture*)(sI42Info->pDecoderData0))->picStatus |= 1;
	((GstH264Dec*)(sI42Info->pDecoderData1))->iRawDecoderFrameFreedFlag++;

	((IppPicture*)(sI42Info->pDecoderData0))->picPlaneStep[IPP_MAXPLANENUM-1] = 0;	//picPlaneStep[IPP_MAXPLANENUM-1] hasn't been accessed by h264 decoder until 2009.11.5
	//printf("-----comm sI42 buffer is finalized, uaddr is %x\n", ((IppPicture*)(sI42Info->pDecoderData0))->ppPicPlane[1]);

	//this operation probably cause h264dec resource finalization, so the h264dec correlative operation should be done before this operation
	gst_buffer_unref(((GstH264Dec*)(sI42Info->pDecoderData1))->ResFinalizer);
	g_slice_free(GST_sI42Buf_SpecialInfo, sI42Info);
	

	vdec_buffer_parent_class->finalize(GST_MINI_OBJECT_CAST(vdec_buffer));
	return;
}


static inline GstVDECFrameBuffer *
gst_vdecbuffer_new()
{
	return (GstVDECFrameBuffer *)gst_mini_object_new(GST_TYPE_VDEC_BUFFER);
}
//end of a subclass from GstBuffer to hook GstBuffer finalize


static void
CopyPictureContent_FromIPPH264(IppPicture *pSrcPicture, unsigned char * pDst, int iDstYpitch, int iDstUVpitch, int Uoffset, int Voffset)
{
	int i;
	Ipp8u *pSrcY = pSrcPicture->ppPicPlane[0] + pSrcPicture->picPlaneStep[0] * pSrcPicture->picROI.y + pSrcPicture->picROI.x;
	Ipp8u *pSrcU = pSrcPicture->ppPicPlane[1] + pSrcPicture->picPlaneStep[1] * (pSrcPicture->picROI.y >> 1) + (pSrcPicture->picROI.x >> 1);
	Ipp8u *pSrcV = pSrcPicture->ppPicPlane[2] + pSrcPicture->picPlaneStep[2] * (pSrcPicture->picROI.y >> 1) + (pSrcPicture->picROI.x >> 1);

	int ystep = pSrcPicture->picPlaneStep[0];
	int ustep = pSrcPicture->picPlaneStep[1];
	int vstep = pSrcPicture->picPlaneStep[2];

	int w = pSrcPicture->picROI.width;
	int h = pSrcPicture->picROI.height;

	unsigned char* pTarget = pDst;

#ifdef DEBUG_PERFORMANCE_GETMEMCPYTIME
	struct timeval clk0, clk1;
	gettimeofday(&clk0,NULL);
#endif

	for(i = 0; i < h; i++) {
		memcpy(pTarget, pSrcY, w);
		pSrcY += ystep;
		pTarget += iDstYpitch;
	}

	pTarget = pDst + Uoffset;
	for(i = 0; i < (h >> 1); i++) {
		memcpy(pTarget, pSrcU, w>>1);
		pSrcU += ustep;
		pTarget += iDstUVpitch;
	}

	pTarget = pDst + Voffset;
	for(i = 0; i < (h >> 1); i++) {
		memcpy(pTarget, pSrcV, w>>1);
		pSrcV += vstep;
		pTarget += iDstUVpitch;
	}

#ifdef DEBUG_PERFORMANCE_GETMEMCPYTIME
	gettimeofday(&clk1,NULL);
	memcpy_time += (clk1.tv_sec - clk0.tv_sec)*1000000 + (clk1.tv_usec - clk0.tv_usec);
#endif

	return;
}

static gint compare_frame_is_in_pool
(gconstpointer a, gconstpointer b)
{
	if(((FrameWaitPaint*)a)->pPic == (IppPicture*)b) {
		return 0;
	}else{
		return -1;
	}
}

static inline int
is_frame_already_in_reorderpool(GSList* pool, IppPicture* pPic)
{
	if(NULL == g_slist_find_custom(pool, pPic, compare_frame_is_in_pool)) {
		return 0;
	}else{
		return 1;
	}
}

static gboolean
_h264_set_skiplogfilename(GstH264Dec *h264dec, const gchar* location)
{
	if(h264dec->skip_logfile) {
		goto SKIPLOGFILE_WAS_OPEN;
	}

	g_free(h264dec->skip_logfilename);

	if(G_LIKELY(location != NULL)) {
		h264dec->skip_logfilename = g_strdup(location);
	}else{
		h264dec->skip_logfilename = NULL;
	}

	return TRUE;

	/* ERRORS */
SKIPLOGFILE_WAS_OPEN:
	g_warning ("Changing the skip_logfile property when a skip log file is open not supported.");
	return FALSE;

}

static void 
gst_h264dec_set_property(GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	GstH264Dec *h264dec = GST_H264DEC(object);
	switch (prop_id)
	{
	case ARG_MORE_SINKBUFFER:
		{
			int moresinkbuffer = g_value_get_int(value);
			if(moresinkbuffer != 0 && moresinkbuffer != 1) {
				GST_ERROR_OBJECT(h264dec, "more-sinkbuffer %d exceed range", moresinkbuffer);
			}else{
				h264dec->bReorderOnSinkBuffers = moresinkbuffer;
			}
		}
		break;
	case ARG_REORDERDELAY:
		{
			int reorderdelay_set = g_value_get_int(value);
			if(reorderdelay_set < -1 || reorderdelay_set > 17) {
				GST_ERROR_OBJECT(h264dec, "force-reorder-delay %d exceed range", reorderdelay_set);
			}else{
				h264dec->iReorderDelayCustomSet = reorderdelay_set;
			}
		}
		break;
	case ARG_FORBID_SI42:
		{
			int forbidsI42 = g_value_get_int(value);
			if(forbidsI42 != 0 && forbidsI42 != 1) {
				GST_ERROR_OBJECT(h264dec, "forbid-sI42 %d exceed range", forbidsI42);
			}else{
				h264dec->bForbid_sI42 = forbidsI42;
			}
		}
		break;
	case ARG_COPYSTREAMBUF:
		{
			int cpbuf = g_value_get_int(value);
			if(cpbuf != 0 && cpbuf != 1) {
				GST_ERROR_OBJECT(h264dec, "copy-streambuf %d exceed range", cpbuf);
			}else{
				h264dec->ForceCopyStreamBuf = cpbuf;
			}
		}
		break;
	case ARG_NOTREORDERBASELINE:
		{
			int notr = g_value_get_int(value);
			if(notr != 0 && notr != 1) {
				GST_ERROR_OBJECT(h264dec, "notreorder-baseline %d exceed range", notr);
			}else{
				h264dec->bNotReorderForBaseline = notr;
			}
		}
		break;
	case ARG_SKIP_STRATEGY:
		{
			h264dec->skip_strategy = g_value_get_int(value);
			if(h264dec->skip_strategy > GSTH264DEC_SKIPSTRATEGY_MAX || h264dec->skip_strategy < GSTH264DEC_SKIPSTRATEGY_MIN) {
				h264dec->skip_strategy = GSTH264DEC_SKIPSTRATEGY_MIN;
				GST_ERROR_OBJECT(h264dec, "skip strategy exceed range, reset to %d", GSTH264DEC_SKIPSTRATEGY_MIN);
			}
		}
		break;
	case ARG_FORBID_ROUGHSKIP:
		{
			int forbidrough = g_value_get_int(value);
			if(forbidrough != 0 && forbidrough != 1) {
				GST_ERROR_OBJECT(h264dec, "forbid-roughskip %d exceed range", forbidrough);
			}else{
				h264dec->bForbid_RoughSkip = forbidrough;
			}
		}
		break;
	case ARG_SKIP_LOGFILE:
		_h264_set_skiplogfilename(h264dec, g_value_get_string(value));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}

	return;
}


static void 
gst_h264dec_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	GstH264Dec *h264dec = GST_H264DEC(object);

	switch (prop_id)
	{
	case ARG_MORE_SINKBUFFER:
		g_value_set_int (value, h264dec->bReorderOnSinkBuffers);
		break;
	case ARG_REORDERDELAY:
		g_value_set_int (value, h264dec->iReorderDelayCustomSet);
		break;
	case ARG_FORBID_SI42:
		g_value_set_int (value, h264dec->bForbid_sI42);
		break;
	case ARG_COPYSTREAMBUF:
		g_value_set_int (value, h264dec->ForceCopyStreamBuf);
		break;
	case ARG_NOTREORDERBASELINE:
		g_value_set_int (value, h264dec->bNotReorderForBaseline);
		break;
	case ARG_SKIP_STRATEGY:
		g_value_set_int (value, h264dec->skip_strategy);
		break;
	case ARG_FORBID_ROUGHSKIP:
		g_value_set_int (value, h264dec->bForbid_RoughSkip);
		break;
	case ARG_SKIP_LOGFILE:
		g_value_set_string(value, h264dec->skip_logfilename);
		break;

#ifdef DEBUG_PERFORMANCE
	case ARG_TOTALFRAME:
		g_value_set_int (value, h264dec->totalFrames);
		break;
	case ARG_CODECTIME:
		g_value_set_int64 (value, h264dec->codec_time);
		break;
#endif
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}

	return;
}

static gboolean 
gst_h264dec_setcaps (GstPad *pad, GstCaps *caps)
{
	GstH264Dec *h264dec = GST_H264DEC (GST_OBJECT_PARENT (pad));

	int i = 0;
	const gchar *name;
	GstStructure *str;
	const GValue *value;


	int stru_num = gst_caps_get_size (caps);
	if(stru_num != 1) {
		gchar* sstr;
		g_warning("Multiple MIME stream type in sinkpad caps, only select the first item.");
		for(i=0; i<stru_num; i++) {
			str = gst_caps_get_structure(caps, i);
			sstr = gst_structure_to_string(str);
			g_warning("struture %d is %s.", i, sstr);
			g_free(sstr);
		}
	}


	str = gst_caps_get_structure(caps, 0);
	name = gst_structure_get_name(str);
	if(strcmp(name, "video/x-h264") != 0) {
		g_warning("Unsupported stream MIME type %s.", name);
		return FALSE;
	}
	

	if(G_UNLIKELY(FALSE == gst_structure_get_fraction(str, "framerate", &h264dec->iFRateNum, &h264dec->iFRateDen))) {
		h264dec->iFRateNum = 0;		//if this information isn't in caps, we assume it is 0
		h264dec->iFRateDen = 1;
	}
	GST_INFO_OBJECT (h264dec, "h264dec->iFRateNum=%d,h264dec->iFRateDen=%d\n",h264dec->iFRateNum,h264dec->iFRateDen);
    if(h264dec->iFRateNum != 0) {
    	h264dec->iFixedFRateDuration = gst_util_uint64_scale_int(GST_SECOND, h264dec->iFRateDen, h264dec->iFRateNum);
	    h264dec->iFixedFRateDuration_5_8 = (h264dec->iFixedFRateDuration >> 1) + (h264dec->iFixedFRateDuration >> 3);
    }else{
        h264dec->iFixedFRateDuration = 0;
        h264dec->iFixedFRateDuration_5_8 = 0;
        assist_myecho("The frame rate information seems not correctly, force fixed duration to 0\n");
    }

	h264dec->is_avcC = FALSE;	//set the default value of is_avcC
	if ((value = gst_structure_get_value (str, "codec_data"))) {
		guint8 *cdata;
		guint csize;
		GstBuffer *buf;

		int offset = 4;
		int cnt_sps = 0;
		int cnt_pps = 0;
		guint16 len = 0;


		/* h264 codec data is avcC data, contains sps and pps, start from offset =5 */
		buf = gst_value_get_buffer(value);

		cdata = GST_BUFFER_DATA (buf);
		csize = GST_BUFFER_SIZE (buf);

		//DecodeFrame_H264Video() probably modify the data, therefore, copy data to self buffer
		if(G_LIKELY(h264dec->pSelfStreamBuf == NULL)) {
			h264dec->iSelfStreamBufSz = csize < GST_IPPH264DEC_SELFSTREAMBUF_MINSZ ? GST_IPPH264DEC_SELFSTREAMBUF_MINSZ : csize;
			h264dec->pSelfStreamBuf = g_malloc(h264dec->iSelfStreamBufSz);
			assist_myecho("Create internal stream buffer %x, size %d\n", h264dec->pSelfStreamBuf, h264dec->iSelfStreamBufSz);
		}else if(h264dec->iSelfStreamBufSz < csize) {
			g_free(h264dec->pSelfStreamBuf);
			h264dec->pSelfStreamBuf = g_malloc(csize);
			h264dec->iSelfStreamBufSz = csize;
			assist_myecho("Re-Create internal stream buffer %x, size %d\n", h264dec->pSelfStreamBuf, h264dec->iSelfStreamBufSz);
		}
		cdata = memcpy(h264dec->pSelfStreamBuf, cdata, csize);

	
		//if have codec_data, we assume it contains one SPS and one PPS at least
		if(csize < 15){//each sync code 3 bytes, SPS NAL>= 6 bytes, PPS NAL>= 3 bytes
			GST_ERROR_OBJECT (h264dec, "do not have enough data in code_data");
			return FALSE;
		}

		//ISO 14496-15: sec 5.2.4.1, sec 5.3.4.1.2
		if(csize >= 20 && cdata && 1 == cdata[0]){
#ifdef DEBUG_PERFORMANCE
			struct timeval clk0, clk1;
#endif
			unsigned int AVCProfileIndication = cdata[1];
			unsigned int profile_compatibility = cdata[2];
			IppCodecStatus codecret;
			h264dec->is_avcC = TRUE;

			if(AVCProfileIndication == 66) {
				h264dec->iNominalProfile = GSTH264DEC_PROFILE_BASELINE;
			}else{
				h264dec->iNominalProfile = GSTH264DEC_PROFILE_UNKNOWN;
			}
			if(AVCProfileIndication != 66 && 0 == (profile_compatibility & 0x80)) {
				GST_INFO_OBJECT(h264dec, "AVCProfileIndication in avcC is %d, profile_compatibility in avcC is %d, IPP 264 decoder probably couldn't support this profile.", AVCProfileIndication, profile_compatibility);
			}
			

			/*parse sps from avcC*/
			cdata += offset;					/* start form 4 */
			
			h264dec->nal_length_size = (*(cdata++) & 0x03)+1;
			if(h264dec->nal_length_size == 3) {
				//the nal_length_size shouldn't be 3, force it to be 4
				g_warning("nal_length_size in avcC isn't right, force it to be 4");
				h264dec->nal_length_size = 4;
			}
			assist_myecho("h264dec->nal_length_size=%d\n",h264dec->nal_length_size);
			GST_INFO_OBJECT (h264dec, "h264dec->nal_length_size=%d\n",h264dec->nal_length_size);
			
			cnt_sps = *(cdata++) & 0x1f;		/* sps cnt */
			/* some stream may be parsed with cnt_sps = 0, we force it as 1 to init codec */
			cnt_sps = cnt_sps > 0 ? cnt_sps : 1;

			for (i = 0; i < cnt_sps; i++) {
				len = *(cdata++)<<8;
				len |= *(cdata++);		/* one sps len 2 bytes*/


				/* deocde sps */
				h264dec->srcBitStream.pBsBuffer = cdata;
				h264dec->srcBitStream.pBsCurByte = h264dec->srcBitStream.pBsBuffer;
				h264dec->srcBitStream.bsByteLen = len;
				h264dec->srcBitStream.bsCurBitOffset = 0;


#ifdef DEBUG_PERFORMANCE
				gettimeofday(&clk0, NULL);
#endif
				assist_myecho("sps len is %d\n",len);
                codecret = DecodeFrame_H264Video(&(h264dec->srcBitStream), &(h264dec->pDstPicList), h264dec->pH264DecState, &(h264dec->nAvailFrames));
#ifdef DEBUG_PERFORMANCE
				gettimeofday(&clk1, NULL);
				h264dec->codec_time += (clk1.tv_sec - clk0.tv_sec)*1000000 + (clk1.tv_usec - clk0.tv_usec);
#endif
				assist_myecho("In setcaps(), DecodeFrame_H264Video() return %d for sps\n", codecret);
				if(codecret != IPP_STATUS_NOERR) {
					g_warning("In setcaps(), DecodeFrame_H264Video() return %d for parsing sps", codecret);
					if(codecret == IPP_STATUS_NOTSUPPORTED_ERR) {
						g_warning("In setcaps(), DecodeFrame_H264Video() return %d for parsing sps, it's fatal error, stop decoding", codecret);
						GST_ERROR_OBJECT(h264dec, "In setcaps(), DecodeFrame_H264Video() return %d for parsing sps, it's fatal error, stop decoding", codecret);
						return FALSE;
					}
				} 
				cdata += len;
			}

			/*parse pps from avcC*/
			cnt_pps= *(cdata++);				/* pps cnt*/
			/* some stream may be parsed with cnt_pps = 0, we force it as 1 to init codec */
			cnt_pps = cnt_pps > 0 ? cnt_pps : 1;

			for (i = 0; i < cnt_pps; i++) {
				len = *(cdata++)<<8;
				len |= *(cdata++);			/*one pps len */

				/* decode pps */				

				h264dec->srcBitStream.pBsBuffer = cdata;
				h264dec->srcBitStream.pBsCurByte = h264dec->srcBitStream.pBsBuffer;
				h264dec->srcBitStream.bsByteLen = len;
				h264dec->srcBitStream.bsCurBitOffset = 0;


#ifdef DEBUG_PERFORMANCE
				gettimeofday(&clk0, NULL);
#endif
				assist_myecho("pps len is %d\n",len);
				codecret = DecodeFrame_H264Video(&(h264dec->srcBitStream), &(h264dec->pDstPicList), h264dec->pH264DecState, &(h264dec->nAvailFrames));
#ifdef DEBUG_PERFORMANCE
				gettimeofday(&clk1, NULL);
				h264dec->codec_time += (clk1.tv_sec - clk0.tv_sec)*1000000 + (clk1.tv_usec - clk0.tv_usec);
#endif
				assist_myecho("In setcaps(), DecodeFrame_H264Video() return %d for pps\n", codecret);
				if(codecret != IPP_STATUS_NOERR) {
					g_warning("In setcaps(), DecodeFrame_H264Video() return %d for parsing pps", codecret);
					if(codecret == IPP_STATUS_NOTSUPPORTED_ERR) {
						g_warning("In setcaps(), DecodeFrame_H264Video() return %d for parsing pps, it's fatal error, stop decoding", codecret);
						GST_ERROR_OBJECT(h264dec, "In setcaps(), DecodeFrame_H264Video() return %d for parsing pps, it's fatal error, stop decoding", codecret);
						return FALSE;
					}
				} 

				cdata += len;

			}	
		}else{
			h264dec->is_avcC = FALSE;
			GST_INFO_OBJECT(h264dec, "No codec_data provided by demuxer, suppose the stream is h.264 byte stream format, and sps+pps are combined into element stream");
		}
	}
	return TRUE;
}


static inline int
update_h264dec_srcpad_cap(GstH264Dec* h264dec)
{
	assist_myecho("cap updated, w %d, h %d, framerate %d %d\n", h264dec->iDownFrameWidth, h264dec->iDownFrameHeight, h264dec->iFRateNum, h264dec->iFRateDen);
	GstCaps *Tmp;
	if(h264dec->DownStreamBufType != DOWNBUF_TYPE_sI42) {
		Tmp = gst_caps_new_simple ("video/x-raw-yuv", 
		"format", GST_TYPE_FOURCC, GST_STR_FOURCC ( H264DEC_4CC_I420STR ), 
		"width", G_TYPE_INT, h264dec->iDownFrameWidth, 
		"height", G_TYPE_INT, h264dec->iDownFrameHeight, 
		"framerate", GST_TYPE_FRACTION,  h264dec->iFRateNum, h264dec->iFRateDen,
		NULL);
	}else{
		Tmp = gst_caps_new_simple ("video/x-raw-yuv", 
		"format", GST_TYPE_FOURCC, GST_STR_FOURCC (H264DEC_4CC_sI42STR), 
		"width", G_TYPE_INT, h264dec->iDownFrameWidth, 
		"height", G_TYPE_INT, h264dec->iDownFrameHeight, 
		"framerate", GST_TYPE_FRACTION,  h264dec->iFRateNum, h264dec->iFRateDen,
		NULL);
	}

	gst_pad_set_caps (h264dec->srcpad, Tmp);
	gst_caps_unref(Tmp);
	
	return 0;
}

#define IPPPIC_IS_DUMMYPIC(pPic)				(((IppPicture*)(pPic))->ppPicPlane[0] == NULL)
#define IPPPIC_IS_ATTACHGSTBUF(pPic)			(((IppPicture*)(pPic))->ppPicPlane[1] == NULL)

static void
_h264_clean_ReorderPool(GstH264Dec* h264dec)
{
	FrameWaitPaint* pFrame;

	for(; h264dec->iDecodedFramesReorderPoolLen!=0; ) {
		pFrame = (FrameWaitPaint*) h264dec->DecodedFramesReorderPool->data;
		h264dec->DecodedFramesReorderPool = g_slist_delete_link(h264dec->DecodedFramesReorderPool, h264dec->DecodedFramesReorderPool);
#ifdef REORDER_BASEONPOC
		pFrame->pTsDu = (TsDuPair*) h264dec->TSReorderPool->data;
		h264dec->TSReorderPool = g_slist_delete_link(h264dec->TSReorderPool, h264dec->TSReorderPool);
#endif
		h264dec->iDecodedFramesReorderPoolLen--;
		g_slice_free(TsDuPair, pFrame->pTsDu);
		if(pFrame->pPic != NULL) {
			pFrame->pPic->picStatus |= 1;
			if(IPPPIC_IS_ATTACHGSTBUF(pFrame->pPic)) {
				gst_buffer_unref(pFrame->pPic->ppPicPlane[0]);
				g_slice_free(IppPicture, pFrame->pPic);
			}
		}
		g_slice_free(FrameWaitPaint, pFrame);
	}
	return;
}

static inline void
_h264_clean_inTsQueue(GstH264Dec* h264dec)
{
	TsDuPair* pTmp;
	for(;;){
		pTmp = g_queue_pop_head(&h264dec->inTsQueue);
		if(pTmp == NULL) {
			break;
		}
		g_slice_free(TsDuPair, pTmp);
	}
	return;
}

static inline int
calculate_GSTstandard_I420_size(int w, int h)
{
	//gstreamer defined standard I420 layout, see <gst/video/video.h>, gstvideo and gst_video_format_get_size()
	return (GST_ROUND_UP_4(w) + GST_ROUND_UP_4(GST_ROUND_UP_2(w)>>1)) * GST_ROUND_UP_2(h);
}

#define I420_standardLayout_sameas_compactLayout(w, h)	(((w)&7)==0 && ((h)&1)==0)	//if width is 8 align and height is 2 align, the GST standard I420 layout is the same as compact I420 layout

static void
update_downbuf_layout(int w, int h, int category, GstH264Dec* h264dec)
{
	if(category == 0) {
		//we defined compact I420 layout
		h264dec->iDownbufYpitch = w;
		h264dec->iDownbufUVpitch = w>>1;
		h264dec->iDownbufUoffset = h264dec->iDownbufYpitch * h;
		h264dec->iDownbufVoffset = h264dec->iDownbufUoffset + h264dec->iDownbufUVpitch * (h>>1);
		h264dec->iDownbufSz = h264dec->iDownbufVoffset + h264dec->iDownbufUVpitch * (h>>1);
	}else if(category == 1) {
		//gstreamer defined standard I420 layout, see <gst/video/video.h>, gstvideo and gst_video_format_get_size()
		h264dec->iDownbufYpitch = GST_ROUND_UP_4(w);
		h264dec->iDownbufUVpitch = GST_ROUND_UP_4(GST_ROUND_UP_2(w)>>1);
		h264dec->iDownbufUoffset = h264dec->iDownbufYpitch * GST_ROUND_UP_2(h);
		h264dec->iDownbufVoffset = h264dec->iDownbufUoffset + h264dec->iDownbufUVpitch * (GST_ROUND_UP_2(h)>>1);
		h264dec->iDownbufSz = h264dec->iDownbufVoffset + h264dec->iDownbufUVpitch * (GST_ROUND_UP_2(h)>>1);
	}
	return;
}

static inline GstFlowReturn
require_downframebuf(GstH264Dec* h264dec, GstBuffer** ppGstBuf)
{
	GstFlowReturn ret = GST_FLOW_OK;
	ret = gst_pad_alloc_buffer_and_set_caps (h264dec->srcpad, 0, h264dec->iDownbufSz, GST_PAD_CAPS (h264dec->srcpad), ppGstBuf);
	if(ret == GST_FLOW_OK) {
		//Gstreamer defined I420/YV12 format require pitch should be 4 aligned(see <gst/video/video.h>, gstvideo and gst_video_format_get_size()). xvimagesink and ffdec always obey this rule.
		//However, filesink and fakesink don't require this limit because they don't care the format/layout of buffer.
		//Therefore, we output different buffer layout for different sink.
		if(!I420_standardLayout_sameas_compactLayout(h264dec->iDownFrameWidth, h264dec->iDownFrameHeight) && GST_BUFFER_SIZE(*ppGstBuf) != (guint)h264dec->iDownbufSz) {
			if((int)GST_BUFFER_SIZE(*ppGstBuf) == calculate_GSTstandard_I420_size(h264dec->iDownFrameWidth, h264dec->iDownFrameHeight)) {
				update_downbuf_layout(h264dec->iDownFrameWidth, h264dec->iDownFrameHeight, 1, h264dec);
			}
		}
	}
	return ret;
}



static GstFlowReturn
_h264_consume_1frame_in_ReorderPool(GstH264Dec* h264dec, int* bFreed1DecNonDummyFrame)
{
	FrameWaitPaint* pFrame;
	GstFlowReturn ret = GST_FLOW_OK;
	GstBuffer *outBuf = NULL;

	pFrame = (FrameWaitPaint*) h264dec->DecodedFramesReorderPool->data;
	h264dec->DecodedFramesReorderPool = g_slist_delete_link(h264dec->DecodedFramesReorderPool, h264dec->DecodedFramesReorderPool);
#ifdef REORDER_BASEONPOC
	pFrame->pTsDu = (TsDuPair*) h264dec->TSReorderPool->data;
	h264dec->TSReorderPool = g_slist_delete_link(h264dec->TSReorderPool, h264dec->TSReorderPool);
#endif
	h264dec->iDecodedFramesReorderPoolLen--;


	if(G_LIKELY(! IPPPIC_IS_DUMMYPIC(pFrame->pPic) )) {
		if(h264dec->DownStreamBufType != DOWNBUF_TYPE_sI42) {
			if(! IPPPIC_IS_ATTACHGSTBUF(pFrame->pPic)) {
				if(pFrame->pPic->picROI.width != h264dec->iDownFrameWidth || pFrame->pPic->picROI.height != h264dec->iDownFrameHeight) {
					//at first, we try our defined compact I420 layout, not standard gstreamer defined I420 layout in <gst/video/video.h>
					h264dec->iDownFrameWidth = pFrame->pPic->picROI.width;
					h264dec->iDownFrameHeight = pFrame->pPic->picROI.height;
					update_h264dec_srcpad_cap(h264dec);
					update_downbuf_layout(h264dec->iDownFrameWidth, h264dec->iDownFrameHeight, 0, h264dec);
				}
				ret = require_downframebuf(h264dec, &outBuf);
				if(G_LIKELY(ret == GST_FLOW_OK)) {
					GST_BUFFER_TIMESTAMP(outBuf) = pFrame->pTsDu->ts;
					GST_BUFFER_DURATION(outBuf) = pFrame->pTsDu->du;
					/* pack up picture's ROI to GstBuffer */
#ifndef DEBUG_DOWN_IS_YV12
					CopyPictureContent_FromIPPH264(pFrame->pPic, GST_BUFFER_DATA(outBuf), h264dec->iDownbufYpitch, h264dec->iDownbufUVpitch, h264dec->iDownbufUoffset, h264dec->iDownbufVoffset);
#else
					CopyPictureContent_FromIPPH264(pFrame->pPic, GST_BUFFER_DATA(outBuf), h264dec->iDownbufYpitch, h264dec->iDownbufUVpitch, h264dec->iDownbufVoffset, h264dec->iDownbufUoffset);
#endif
				}
				pFrame->pPic->picStatus |= 1;
				*bFreed1DecNonDummyFrame = 1;
				g_slice_free(TsDuPair, pFrame->pTsDu);
				g_slice_free(FrameWaitPaint, pFrame);

				if(ret != GST_FLOW_OK) {
					if(ret != GST_FLOW_WRONG_STATE) {					
						//during seeking, sink element often return GST_FLOW_WRONG_STATUS. No need to echo this message
						g_warning("h264dec can not malloc buffer from downstream, ret %d, wanted width %d, height %d, wanted sz %d", ret, h264dec->iDownFrameWidth, h264dec->iDownFrameHeight, h264dec->iDownbufSz);
					}
					GST_WARNING_OBJECT(h264dec, "h264dec can not malloc buffer from downstream, ret %d, wanted width %d, height %d, wanted sz %d", ret, h264dec->iDownFrameWidth, h264dec->iDownFrameHeight, h264dec->iDownbufSz);
					return ret;
				}
			}else{
				outBuf = (GstBuffer*)(pFrame->pPic->ppPicPlane[0]);
				GST_BUFFER_TIMESTAMP(outBuf) = pFrame->pTsDu->ts;
				GST_BUFFER_DURATION(outBuf) = pFrame->pTsDu->du;
			
				*bFreed1DecNonDummyFrame = 1;
				g_slice_free(TsDuPair, pFrame->pTsDu);
				g_slice_free(IppPicture, pFrame->pPic);
				g_slice_free(FrameWaitPaint, pFrame);
			}
		}else{
			GST_sI42Buf_SpecialInfo* sI42Info;

			if(pFrame->pPic->picROI.width != h264dec->iDownFrameWidth || pFrame->pPic->picROI.height != h264dec->iDownFrameHeight) {
				h264dec->iDownFrameWidth = pFrame->pPic->picROI.width;
				h264dec->iDownFrameHeight = pFrame->pPic->picROI.height;
				update_h264dec_srcpad_cap(h264dec);
			}

			sI42Info = g_slice_new(GST_sI42Buf_SpecialInfo);
			sI42Info->pY_vAddr = (unsigned char*)(pFrame->pPic->ppPicPlane[0]) + pFrame->pPic->picROI.x + pFrame->pPic->picROI.y*pFrame->pPic->picPlaneStep[0];
			sI42Info->pU_vAddr = (unsigned char*)(pFrame->pPic->ppPicPlane[1]) + (pFrame->pPic->picROI.x>>1) + (pFrame->pPic->picROI.y>>1)*pFrame->pPic->picPlaneStep[1];
			sI42Info->pV_vAddr = (unsigned char*)(pFrame->pPic->ppPicPlane[2]) + (pFrame->pPic->picROI.x>>1) + (pFrame->pPic->picROI.y>>1)*pFrame->pPic->picPlaneStep[2];

			sI42Info->iYPitch = pFrame->pPic->picPlaneStep[0];
			sI42Info->iUPitch = pFrame->pPic->picPlaneStep[1];
			sI42Info->iVPitch = pFrame->pPic->picPlaneStep[2];

#define GST_IPPH264DEC_LUM_PAD_LENGTH		IPP_H264_LUM_PAD_WIDTH	//IPP_H264_LUM_PAD_WIDTH defined in codecVC.h
			//calculate the physical address
			sI42Info->pBase_vAddr = (unsigned char*)((unsigned int)pFrame->pPic->ppPicPlane[0] - (GST_IPPH264DEC_LUM_PAD_LENGTH*sI42Info->iYPitch + GST_IPPH264DEC_LUM_PAD_LENGTH));
			//NOTE: for bmm, only the original virtual address could be converted physical address through bmm_get_paddr()
			sI42Info->pBase_pAddr = (unsigned char*)h264dec->pfunBmmGetPaddr(sI42Info->pBase_vAddr);
			sI42Info->pY_pAddr = sI42Info->pBase_pAddr + (sI42Info->pY_vAddr - sI42Info->pBase_vAddr);
			sI42Info->pU_pAddr = sI42Info->pBase_pAddr + (sI42Info->pU_vAddr - sI42Info->pBase_vAddr);
			sI42Info->pV_pAddr = sI42Info->pBase_pAddr + (sI42Info->pV_vAddr - sI42Info->pBase_vAddr);
			
			sI42Info->iXoffset = GST_IPPH264DEC_LUM_PAD_LENGTH + pFrame->pPic->picROI.x;
			sI42Info->iYoffset = GST_IPPH264DEC_LUM_PAD_LENGTH + pFrame->pPic->picROI.y;
			sI42Info->iExpandedWidth = pFrame->pPic->picWidth + (GST_IPPH264DEC_LUM_PAD_LENGTH<<1);	//it should equal to sI42Info->iYPitch
			sI42Info->iExpandedHeight = pFrame->pPic->picHeight + (GST_IPPH264DEC_LUM_PAD_LENGTH<<1);
#if 0
			if(sI42Info->iExpandedWidth != sI42Info->iYPitch) {
				g_warning("Expanded width %d =/= Y pitch %d", sI42Info->iExpandedWidth, sI42Info->iYPitch);
			}
#endif


			sI42Info->pDecoderData0 = (void*) pFrame->pPic;
			gst_buffer_ref(h264dec->ResFinalizer);
			sI42Info->pDecoderData1 = (void*) h264dec;
			
			pFrame->pPic->picPlaneStep[IPP_MAXPLANENUM-1] = 1;	//indicate this IppPicture is using by display sink

			outBuf = (GstBuffer*)gst_vdecbuffer_new();

			GST_BUFFER_DATA(outBuf) = sI42Info->pY_vAddr;
			//GST_BUFFER_SIZE(outBuf) = h264dec->picDataSize;
			GST_BUFFER_SIZE(outBuf) = sI42Info->pV_vAddr + sI42Info->iVPitch * (h264dec->iDownFrameHeight>>1) - sI42Info->pY_vAddr;

			//printf("+++push sI42 buffer to sink, uaddr %x\n", sI42Info->pUAddr);



			IPPGST_BUFFER_CUSTOMDATA(outBuf) = (gpointer)sI42Info;

			gst_buffer_set_caps(outBuf, GST_PAD_CAPS(h264dec->srcpad));

			GST_BUFFER_TIMESTAMP(outBuf) = pFrame->pTsDu->ts;
			GST_BUFFER_DURATION(outBuf) = pFrame->pTsDu->du;

			g_slice_free(TsDuPair, pFrame->pTsDu);
			g_slice_free(FrameWaitPaint, pFrame);
			*bFreed1DecNonDummyFrame = 0;
		}


#ifdef DUMP_TS_FILE
		fprintf(fpOutTs, "Out Ts %lld, Du %lld\n", GST_BUFFER_TIMESTAMP(outBuf), GST_BUFFER_DURATION(outBuf));
#endif
		if(h264dec->bNewSegReceivedAfterflush) {
			ret = gst_pad_push(h264dec->srcpad, outBuf);
		}else{
			//If new segment event hasn't received, shouldn't push buffer to sink. Otherwise, sink will throw warnings and work abnormal.
			//Sometimes, user seeking position exceeds the duration of file, then demuxer only fire flush event without newseg event. And demuxer also fire EOS event. At EOS, decoder probably still output some frames, but those frames shouldn't be pushed to sink because no newseg event.
			gst_buffer_unref(outBuf);
			ret = GST_FLOW_OK;
		}
		if (GST_FLOW_OK != ret) {
			g_warning("calling gst_pad_push() to push frame to downstream fail, ret is %d", ret);
			GST_WARNING_OBJECT(h264dec,"calling gst_pad_push() to push frame to downstream fail, ret is %d", ret);
			return ret;
		}
		h264dec->frame_suc_pushed2sink_cnt++;
	}else{//it's a dummy frame, just free some structure
		g_slice_free(TsDuPair, pFrame->pTsDu);
		g_slice_free(FrameWaitPaint, pFrame);
	}

	return ret;
}

static inline void
_h264_align_ts_to_frame(int framecnt, GstH264Dec* h264dec)
{
	int tscnt = h264dec->inTsQueue.length;
	if(G_LIKELY(tscnt == framecnt)) {
		return;
	}
	if(tscnt > framecnt) {
#define TSALIGN_ADJUST_POS	0
		int remove_tscnt = tscnt-framecnt-TSALIGN_ADJUST_POS;	//ADJUST_TSALIGN_POS is used to align the latest frame to the latest TS or the 2nd latest TS. ADJUST_TSALIGN_POS's range is 0~1.
		TsDuPair* pTmp;
		assist_myecho("--TS(%d) is more than Frame(%d) at input frag %d--\n", tscnt, framecnt, h264dec->demuxer_datafrag_cnt);
		for(; remove_tscnt != 0; remove_tscnt--) {
			pTmp = g_queue_pop_head(&h264dec->inTsQueue);
			g_slice_free(TsDuPair, pTmp);
		}
	}else{
		int append_tscnt = framecnt-tscnt;
		TsDuPair* pTmp;
		assist_myecho("--TS(%d) is less than Frame(%d) at input frag %d--\n", tscnt, framecnt, h264dec->demuxer_datafrag_cnt);
		for(; append_tscnt !=0; append_tscnt--) {
			pTmp = g_slice_new(TsDuPair);
			pTmp->ts = GST_CLOCK_TIME_NONE;		//use GST_CLOCK_TIME_NONE for those missing TS
			pTmp->du = GST_CLOCK_TIME_NONE;
			g_queue_push_head(&h264dec->inTsQueue, pTmp);
		}
	}

	return;
}

static gint
compareFrameInDisplayOrder(gconstpointer a, gconstpointer b)
{
#ifndef REORDER_BASEONPOC
	GstClockTime TsA = ((FrameWaitPaint*)a)->pTsDu->ts;
	GstClockTime TsB = ((FrameWaitPaint*)b)->pTsDu->ts;
	if(G_LIKELY(TsA != TsB && TsA != GST_CLOCK_TIME_NONE && TsB != GST_CLOCK_TIME_NONE)) {
		if(TsA > TsB) {
			return 1;
		}else{
			return -1;
		}
	}else{
		return ((FrameWaitPaint*)a)->NominalPoc >= ((FrameWaitPaint*)b)->NominalPoc ? 1 : -1;
	}
#else
	return ((FrameWaitPaint*)a)->NominalPoc > ((FrameWaitPaint*)b)->NominalPoc ? 1 : -1;
#endif
}

#ifdef REORDER_BASEONPOC
static void
degrade_poc_in_reorderpool(GSList* reorderlist, int newPoc, int additional_delta)
{
	GSList* lastItem = g_slist_last(reorderlist);
	if(lastItem != NULL) {
		int maxPoc = ((FrameWaitPaint*)(lastItem->data))->NominalPoc;
		int delta = newPoc  - maxPoc + additional_delta;
		while(reorderlist != NULL) {
			((FrameWaitPaint*)(reorderlist->data))->NominalPoc += delta;
			reorderlist = reorderlist->next;
		}
	}
	return;
}
static gint
compareTsInDisplayOrder(gconstpointer a, gconstpointer b)
{
	//let early timestamp at the head of list, if two ts is equal, the more early inserted is at the head of list
	if( (gint64)(((TsDuPair*)a)->ts) >= (gint64)(((TsDuPair*)b)->ts) ) {
		//convert to signed int64, let GST_CLOCK_TIME_NONE is the most early timestamp
		return 1;
	}else{
		return -1;
	}
}
#endif

static int
_h264_process_decodedframes(GstH264Dec* h264dec, int iFrameCnt, IppH264PicList* DecFrameList)
{
	FrameWaitPaint* pFrame;
	_h264_align_ts_to_frame(iFrameCnt, h264dec);
	for(; iFrameCnt>0; iFrameCnt--) {
		pFrame = g_slice_new(FrameWaitPaint);
		pFrame->pPic = DecFrameList->pPic;
		pFrame->NominalPoc = DecFrameList->pPic->picOrderCnt;
		DecFrameList = DecFrameList->pNextPic;
		
		pFrame->pTsDu = g_queue_pop_head(&h264dec->inTsQueue);
	
		if(h264dec->DownStreamBufType != DOWNBUF_TYPE_sI42) {
			if(h264dec->bReorderOnSinkBuffers == 1) {
				if(! IPPPIC_IS_DUMMYPIC(pFrame->pPic)) {
					//it isn't a dummy frame
					GstFlowReturn ret;
					GstBuffer * GstBufFromSink;
					if(pFrame->pPic->picROI.width != h264dec->iDownFrameWidth || pFrame->pPic->picROI.height != h264dec->iDownFrameHeight) {
						//at first, we try our defined compact I420 layout, not standard gstreamer defined I420 layout in <gst/video/video.h>
						h264dec->iDownFrameWidth = pFrame->pPic->picROI.width;
						h264dec->iDownFrameHeight = pFrame->pPic->picROI.height;
						update_h264dec_srcpad_cap(h264dec);
						update_downbuf_layout(h264dec->iDownFrameWidth, h264dec->iDownFrameHeight, 0, h264dec);
					}

					ret = require_downframebuf(h264dec, &GstBufFromSink);
					if(G_LIKELY(ret == GST_FLOW_OK)) {
						/* pack up picture's ROI to GstBuffer */
						IppPicture * pPicGstbuf = g_slice_new(IppPicture);
#ifndef DEBUG_DOWN_IS_YV12
						CopyPictureContent_FromIPPH264(pFrame->pPic, GST_BUFFER_DATA(GstBufFromSink), h264dec->iDownbufYpitch, h264dec->iDownbufUVpitch, h264dec->iDownbufUoffset, h264dec->iDownbufVoffset);
#else
						CopyPictureContent_FromIPPH264(pFrame->pPic, GST_BUFFER_DATA(GstBufFromSink), h264dec->iDownbufYpitch, h264dec->iDownbufUVpitch, h264dec->iDownbufVoffset, h264dec->iDownbufUoffset);
#endif
						pFrame->pPic->picStatus |= 1;
						pPicGstbuf->ppPicPlane[0] = (unsigned char*)GstBufFromSink;
						pPicGstbuf->ppPicPlane[1] = NULL;	//ppPicPlane[1] == 0 means this IppPicture is attached with a GstBuffer from downstream
						pPicGstbuf->picROI.width = pFrame->pPic->picROI.width;
						pPicGstbuf->picROI.height = pFrame->pPic->picROI.height;
						pFrame->pPic = pPicGstbuf;
					}
					pFrame->pPic->picStatus |= 1;
					if(G_UNLIKELY(ret != GST_FLOW_OK)) {
						if(ret != GST_FLOW_WRONG_STATE) {
							//during seeking, sink element often return GST_FLOW_WRONG_STATUS. No need to echo this message
							g_warning("h264dec can not malloc buffer from downstream, ret %d, wanted width %d, height %d, wanted sz %d", ret, h264dec->iDownFrameWidth, h264dec->iDownFrameHeight, h264dec->iDownbufSz);
						}
						GST_WARNING_OBJECT(h264dec, "h264dec can not malloc buffer from downstream, ret %d, wanted width %d, height %d, wanted sz %d", ret, h264dec->iDownFrameWidth, h264dec->iDownFrameHeight, h264dec->iDownbufSz);
						//use a dummy frame
						pFrame->pPic = &h264dec->DummyPic;
					}
				}
			}
		}

		//insert FrameWaitPaint to pool, implement reorder
#ifdef REORDER_BASEONPOC
#define POC_PROTECTGAP	1000
		if(pFrame->NominalPoc == 0) {
			degrade_poc_in_reorderpool(h264dec->DecodedFramesReorderPool, 0, -(POC_PROTECTGAP+1));	//in decoding order, it's possible that there is one B frame whose poc < 0 after a I frame whose  poc is 0. Therefore, must have a gap.
		}
		if(IPPPIC_IS_DUMMYPIC(pFrame->pPic)) {
			//a skip dummy picture
			GSList* lastItem = g_slist_last(h264dec->DecodedFramesReorderPool);
			//let the dummy picture's poc equal to the last picture in reorder pool
			if(lastItem != NULL) {
				pFrame->NominalPoc = ((FrameWaitPaint*)(lastItem->data))->NominalPoc;
			}else{
				pFrame->NominalPoc = -(POC_PROTECTGAP+1);
			}
		}
#endif
		h264dec->DecodedFramesReorderPool = g_slist_insert_sorted(h264dec->DecodedFramesReorderPool, pFrame, compareFrameInDisplayOrder);
#ifdef REORDER_BASEONPOC
		h264dec->TSReorderPool = g_slist_insert_sorted(h264dec->TSReorderPool, pFrame->pTsDu, compareTsInDisplayOrder);
		pFrame->pTsDu = NULL;
#endif

		h264dec->iDecodedFramesReorderPoolLen ++;
		//push frame to downstream in display order
		if(h264dec->iDecodedFramesReorderPoolLen == h264dec->iReorderDelay + 1) {
			int bDec1NonDummyFrameFreed = 0;
			_h264_consume_1frame_in_ReorderPool(h264dec, &bDec1NonDummyFrameFreed);
		}
	}
	return 0;
}


static gboolean 
gst_h264dec_sinkpad_event (GstPad *pad, GstEvent *event)
{
	GstH264Dec *h264dec = GST_H264DEC (GST_OBJECT_PARENT (pad));
	gboolean eventRet;

	switch(GST_EVENT_TYPE (event))
	{
	case GST_EVENT_NEWSEGMENT:
		assist_myecho("New segment event received by h264dec GST plugin\n");
		//clean all frames in Decoded Frame Pool, all Ts
		_h264_clean_ReorderPool(h264dec);
		_h264_clean_inTsQueue(h264dec);

		//reset qos mechanism
		memset(h264dec->DropRecordWnd_S, 0, sizeof(h264dec->DropRecordWnd_S));
		memset(h264dec->DropRecordWnd_L, 0, sizeof(h264dec->DropRecordWnd_L));
		h264dec->iShortTermDropRate = 0;
		h264dec->iLongTermDropRate = 0;
		h264dec->iNewQosReportedCnt = 0;
		h264dec->QosProtect_forNewSeq = QOSLOG_PROTECT_FORNEWSEG;

		eventRet = gst_pad_push_event(h264dec->srcpad, event);
		h264dec->bNewSegReceivedAfterflush = 1;
		break;
	case GST_EVENT_EOS:
		h264dec->is_playing = 0;
		{
			int bDec1NonDummyFrameFreed;
#define WORKROUND_QTDEMUX_BFRAME_LASTFRAME
#ifdef WORKROUND_QTDEMUX_BFRAME_LASTFRAME
			//For B frame h264 stream, the last frame's timestamp provided by qtdemuxer isn't less than the stop time of current segment data provided by qtdemxer. Therefore, the basesink thinks the last frame exceed the current segment data's range, and ignore the last frame.
			//To fix it, decrease the last frame's timestamp a little. 2009.09.25, decrease a little is useless for some stream, for those streams, the last frame's timestamp exceed the segment's duration a lot(those stream is edited by some edit tool, therefore, before the last frame, some frame are eliminated. however, the segment's duration is calculated through the frame count in the segment). To work round, force the last frame's timestamp to GST_CLOCK_TIME_NONE.
			//Qtdemuxer 0.10.14 has this issue. ffdemuxer hasn't this issue. But ffdemuxer(0.10.6.2) couldn't provide right framerate, it always provides field rate.
#ifndef REORDER_BASEONPOC
			GSList* LastFrameList = g_slist_last(h264dec->DecodedFramesReorderPool);
			if(LastFrameList != NULL) {
				FrameWaitPaint* pFrame = (FrameWaitPaint*)LastFrameList->data;
				if(pFrame->pTsDu->ts != GST_CLOCK_TIME_NONE) {
					//pFrame->pTsDu->ts = pFrame->pTsDu->ts - 100;
					pFrame->pTsDu->ts = GST_CLOCK_TIME_NONE;
				}
			}
#else
			GSList* LastTsList = g_slist_last(h264dec->TSReorderPool);
			if(LastTsList != NULL) {
				((TsDuPair*)LastTsList->data)->ts = GST_CLOCK_TIME_NONE;
			}
#endif
#endif
			for(;h264dec->iDecodedFramesReorderPoolLen != 0;) {
				bDec1NonDummyFrameFreed = 0;
				_h264_consume_1frame_in_ReorderPool(h264dec, &bDec1NonDummyFrameFreed);
			}
		}

		eventRet = gst_pad_push_event(h264dec->srcpad, event);
		break;
	case GST_EVENT_FLUSH_STOP:
		h264dec->bNewSegReceivedAfterflush = 0;
	default:
		eventRet = gst_pad_event_default(pad, event);
		break;
	}

	return eventRet;
}

static void
log_droprecord_and_calculate_droprate(GstH264Dec *h264dec, int bNominalDropped)
{
	gint iSTRate = h264dec->iShortTermDropRate, iLTRate = h264dec->iLongTermDropRate;
	
	//using MA filter to calculate average drop rate
	iSTRate -= h264dec->DropRecordWnd_S[h264dec->iNextRecordIdx_S];
	iLTRate -= h264dec->DropRecordWnd_L[h264dec->iNextRecordIdx_L];
	h264dec->DropRecordWnd_S[h264dec->iNextRecordIdx_S++] = bNominalDropped;
	iSTRate += bNominalDropped;
	h264dec->DropRecordWnd_L[h264dec->iNextRecordIdx_L++] = bNominalDropped;
	iLTRate += bNominalDropped;

	//ring buffer to implement moving window
	if(h264dec->iNextRecordIdx_S == DROP_REC_SHORTWND_LEN) {
		h264dec->iNextRecordIdx_S = 0;
	}
	if(h264dec->iNextRecordIdx_L == DROP_REC_LONGWND_LEN) {
		h264dec->iNextRecordIdx_L = 0;
	}

	//using atomic operation because this event comes from basesink data processing function, it probably in another thread when queue exists in pipeline.
	SET_INT32_NominalAtomic(h264dec->iShortTermDropRate, iSTRate);
	SET_INT32_NominalAtomic(h264dec->iLongTermDropRate, iLTRate);
	INC_INT32_NominalAtomic(h264dec->iNewQosReportedCnt);

	return;
}

static gboolean	
gst_h264dec_srcpad_event (GstPad *pad, GstEvent *event)
{
	gboolean eventRet;
	switch(GST_EVENT_TYPE (event))
	{
	case GST_EVENT_QOS:
		{
			GstH264Dec *h264dec = GST_H264DEC (GST_OBJECT_PARENT (pad));
			//only gather qos information when skip_strategy==1
			if(h264dec->skip_strategy == 1) {
			//{
				if(h264dec->QosProtect_forNewSeq <= 0 && h264dec->is_playing == 1) {
					GstClockTimeDiff diff;
					int NominalDropRecord;
					//GstClockTime timestamp;
					//gst_event_parse_qos(event, &proportion, &diff, &timestamp);
					gst_event_parse_qos(event, NULL, &diff, NULL);
					if(G_UNLIKELY(diff == 0)) {
						//no frame Drop in sink
						NominalDropRecord = 0;	//when video sink's sync==0, the diff in QoS event reported by video sink is equal to 0
					}else{
#if 1
						if(h264dec->iCurSmartSkipMode <= SMARTSKIPMODE_PERFORMANCE_PREFER_MID) {
							NominalDropRecord = diff < -18000000 ? 0 : 1;	//before SMARTSKIPMODE_PERFORMANCE_PREFER_MID, if the frame arrive at the sink only a little precede it's timestamp, we still think it is dropped. Therefore, we could prevent the real frame drop by changing decoder mode in advance.
						// No obvious effect, the most case is a scene changing result in the decoding time increase a lot. Before that scene changing, no premonitor.
						}else{
							NominalDropRecord = diff < 0 ? 0 : 1;
						}
#else
						NominalDropRecord = diff < -15000000 ? 0 : 1;
#endif
					}
					log_droprecord_and_calculate_droprate(h264dec, NominalDropRecord);
					//assist_myecho("qos jitter %lldns, s_rate %d, l_rate %d, qos cnt %d\n", diff, h264dec->iShortTermDropRate, h264dec->iLongTermDropRate, h264dec->iNewQosReportedCnt);
					//printf("qos jitter %lldns, s_rate %d, l_rate %d, qos cnt %d\n", diff, h264dec->iShortTermDropRate, h264dec->iLongTermDropRate, h264dec->iNewQosReportedCnt);
				}else{
					//after new seg, we ignore some qos event
					h264dec->QosProtect_forNewSeq--;
				}
			}	
			eventRet = gst_pad_push_event(h264dec->sinkpad, event);
		}
		
		break;

	default:
		eventRet = gst_pad_event_default(pad, event);
		break;
	}

	return eventRet;
}



static void
_h264_log_frameskip(GstH264Dec* h264dec, int whichskip, int bSkipped)
{
	if(whichskip == 0) {
		//decoder skip
		fprintf(h264dec->skip_logfile, "---raw decoder skipped %d, cur smart mode %d\n", bSkipped, h264dec->iCurSmartSkipMode);
	}else{
		//gst rough skip
		fprintf(h264dec->skip_logfile, "++++data fragment cnt %d, gst rough skipped %d, cur smart mode %d\n", h264dec->demuxer_datafrag_cnt, bSkipped, h264dec->iCurSmartSkipMode);
	}
	return;
}


static int
set_thresholds(GstH264Dec* h264dec, int w, int h)
{
	int WxH = w*h;
	assist_myecho("set_thresholds, w %d, h %d\n", w, h);
#if 1
	//for 800Mhz cpu
	if(WxH <= 640*480) {
		h264dec->iSmartModeByDimension = SMARTSKIPMODE_NORMALDECODE;
	}else if(WxH <= 720*576) {
		h264dec->iSmartModeByDimension = SMARTSKIPMODE_PERFORMANCE_PREFER_LITTLE;
	}else{
		h264dec->iSmartModeByDimension = SMARTSKIPMODE_PERFORMANCE_PREFER_MID;
	}
#else
	//for 1.2G cpu
	if(WxH <= 720*576) {
		h264dec->iSmartModeByDimension = SMARTSKIPMODE_NORMALDECODE;
	}else{
		h264dec->iSmartModeByDimension = SMARTSKIPMODE_PERFORMANCE_PREFER_MID;
	}
#endif

#define VIDEO_MINSZ_FOR_ROUGHSKIPENABLE		(720*480)
	if(WxH <= VIDEO_MINSZ_FOR_ROUGHSKIPENABLE) {	//for small stream, rough skip always be forbidden.
		h264dec->bForbid_RoughSkip = 1;
	}

	return WxH;
}

static int
set_new_smartskipmode(GstH264Dec *h264dec, int smartmode)
{
	IppCodecStatus codecRt;
	IppSkipMode decodermode;
	switch(smartmode) {
	case SMARTSKIPMODE_NORMALDECODE:
		decodermode = IPPVC_SKIPMODE_0;
		break;
	case SMARTSKIPMODE_PERFORMANCE_PREFER_LITTLE:
		decodermode = IPPVC_SKIPMODE_1;
		break;
	case SMARTSKIPMODE_PERFORMANCE_PREFER_MID:
		decodermode = IPPVC_SKIPMODE_2;
		break;
	case SMARTSKIPMODE_PERFORMANCE_PREFER_BIG:
		decodermode = IPPVC_SKIPMODE_2_5;
		break;
	case SMARTSKIPMODE_PERFORMANCE_PREFER_LARGE:
		decodermode = IPPVC_SKIPMODE_3;
		break;
	case SMARTSKIPMODE_PERFORMANCE_PREFER_MOST:
		decodermode = IPPVC_SKIPMODE_3;
		//reset rough skip information
		h264dec->iCurRoughSkipDataPeriod = ROUGH_SKIPDATA_PERIOD_MAX;
		h264dec->iRoughSkipDataIndicator = ROUGH_SKIPDATA_PERIOD_MAX-1;
		break;
	default:
		g_warning("Un supported smart skip mode %d", smartmode);
		return -1;
	}
	assist_myecho("Set decoder mode to %d, GST decoder mode to %d, at in fragment %d\n", decodermode, smartmode, h264dec->demuxer_datafrag_cnt);
	codecRt = DecodeSendCmd_H264Video(IPPVC_SET_SKIPMODE, &decodermode, NULL, h264dec->pH264DecState);
	if(codecRt != IPP_STATUS_NOERR) {
		g_warning("Set decoder skip mode fail, decoder return %d.", codecRt);
		GST_ERROR_OBJECT(h264dec, "Set decoder skip mode fail, decoder return %d.", codecRt);
		return -1;
	}
	h264dec->iCurSmartSkipMode = smartmode;
	h264dec->smartmode_switchcnt++;

	return 0;
}

static int
switch_smartskipmode(GstH264Dec *h264dec)
{
	int newmode;
	//Qos report may be thrown in another thread, using atomic method. In fact, using mutex is more safe.
	if(GET_INT32_NominalAtomic(h264dec->iNewQosReportedCnt) > (DROP_REC_SHORTWND_LEN+SHORTWND_QOSWATCHLEN_ADJUST) && GET_INT32_NominalAtomic(h264dec->iShortTermDropRate) >= PERFORMANCE_LACK) {
		//increase the smart mode
		if(h264dec->iCurSmartSkipMode < SMARTSKIPMODE_PERFORMANCE_PREFER_LARGE) {
			newmode = h264dec->iCurSmartSkipMode + 1;
			assist_myecho("performance is lack, increase mode from %d to %d\n",h264dec->iCurSmartSkipMode, newmode);
			set_new_smartskipmode(h264dec, newmode);
			SET_INT32_NominalAtomic(h264dec->iNewQosReportedCnt, 0);
		}else{
			//transfer to mode SMARTSKIPMODE_PERFORMANCE_PREFER_MOST should be limited
			if(GET_INT32_NominalAtomic(h264dec->iShortTermDropRate) >= PERFORMANCE_LACK_MUCH) {
				if(h264dec->iCurSmartSkipMode == SMARTSKIPMODE_PERFORMANCE_PREFER_LARGE) {
					newmode = SMARTSKIPMODE_PERFORMANCE_PREFER_LARGE + 1;
					assist_myecho("performance is lack, increase mode from %d to %d\n",h264dec->iCurSmartSkipMode, newmode);
					set_new_smartskipmode(h264dec, newmode);
				}else{
					if(h264dec->iCurRoughSkipDataPeriod > ROUGH_SKIPDATA_PERIOD_MIN) {
						h264dec->iRoughSkipDataIndicator = h264dec->iCurRoughSkipDataPeriod;
						h264dec->iCurRoughSkipDataPeriod--;
						assist_myecho("performance is lack, decrease rough skip period to %d\n",h264dec->iCurRoughSkipDataPeriod);
					}
				}
				SET_INT32_NominalAtomic(h264dec->iNewQosReportedCnt, 0);
			}
		}
	}else if(GET_INT32_NominalAtomic(h264dec->iNewQosReportedCnt) > (DROP_REC_LONGWND_LEN+LONGWND_QOSWATCHLEN_ADJUST) && GET_INT32_NominalAtomic(h264dec->iLongTermDropRate) <= PERFORMANCE_RICH) {
		//decrease the smart mode
		if(h264dec->iCurSmartSkipMode == SMARTSKIPMODE_PERFORMANCE_PREFER_MOST && h264dec->iCurRoughSkipDataPeriod < ROUGH_SKIPDATA_PERIOD_MAX) {
			h264dec->iRoughSkipDataIndicator = h264dec->iCurRoughSkipDataPeriod;
			h264dec->iCurRoughSkipDataPeriod++;
			SET_INT32_NominalAtomic(h264dec->iNewQosReportedCnt, 0);
			assist_myecho("performance is rich, increase rough skip period to %d\n",h264dec->iCurRoughSkipDataPeriod);
		}else if(h264dec->iCurSmartSkipMode > SMARTSKIPMODE_NORMALDECODE) {
			newmode = h264dec->iCurSmartSkipMode - 1;
			assist_myecho("performance is rich, decrease mode from %d to %d\n",h264dec->iCurSmartSkipMode, newmode);
			set_new_smartskipmode(h264dec, newmode);
			SET_INT32_NominalAtomic(h264dec->iNewQosReportedCnt, 0);
		}
	}
	return 0;
}

static inline int
get_first_nal_type(GstH264Dec *h264dec, const unsigned char* pData, int len)
{
	if(h264dec->is_avcC != 0) {
		return pData[h264dec->nal_length_size] & 0x1f;
	}else{
		if(len >= 4) {
			int code = (pData[0]<<16) | (pData[1]<<8) | pData[2] ;
			pData += 3;
			while( (code&0xffffff) != 0x000001 && len >= 4 ) {
				code = (*pData++) | (code<<8) ;
				len--;
			}
			if(len >= 4) {
				return pData[0] & 0x1f;
			}
		}
		return -1;
	}
}

static int 
should_roughskip_datafrag(GstH264Dec *h264dec, const unsigned char* pData, int len)
{
	if(h264dec->bForbid_RoughSkip != 0) {
		return 0;
	}
	if(get_first_nal_type(h264dec, pData, len) == H264_IDR_SLICE_NALU) {
		//reset indicator, so the rough skipping won't happen just following IDR frame
		h264dec->iRoughSkipDataIndicator = h264dec->iCurRoughSkipDataPeriod-1;
		return 0;
	}
	if(h264dec->iRoughSkipDataIndicator != 0) {
		h264dec->iRoughSkipDataIndicator--;
		return 0;
	}else{
		h264dec->iRoughSkipDataIndicator = h264dec->iCurRoughSkipDataPeriod-1;
		return 1;
	}
}


static inline int
getDPBSizeFromDecoder(GstH264Dec* h264dec)
{
	int DBPsize;
	IppCodecStatus codecRt;
	codecRt = DecodeSendCmd_H264Video(IPPVC_GET_DPBSIZE, NULL, &DBPsize, h264dec->pH264DecState);
	if(codecRt != IPP_STATUS_NOERR) {
		g_warning("get DPB size from decoder fail, decoder return %d.", codecRt);
		GST_ERROR_OBJECT(h264dec, "get DPB size from decoder fail, decoder return %d.", codecRt);
		return -1;
	}
	return DBPsize;
}

static inline int
getPocTypeFromDecoder(GstH264Dec* h264dec)
{
	int POCtype;
	IppCodecStatus codecRt;
	codecRt = DecodeSendCmd_H264Video(IPPVC_GET_POCTYPE, NULL, &POCtype, h264dec->pH264DecState);
	if(codecRt != IPP_STATUS_NOERR) {
		g_warning("get poc type from decoder fail, decoder return %d.", codecRt);
		GST_ERROR_OBJECT(h264dec, "get poc type from decoder fail, decoder return %d.", codecRt);
		return -1;
	}
	return POCtype;

}

static void
_h264_gather_decoder_info(GstH264Dec* h264dec)
{
	int DPBSize = -1, PocType = -1;

	DPBSize = getDPBSizeFromDecoder(h264dec);
	PocType = getPocTypeFromDecoder(h264dec);

	if(PocType == 2 || (h264dec->iNominalProfile == GSTH264DEC_PROFILE_BASELINE && h264dec->bNotReorderForBaseline) ) {
		//for PocType == 2, the display order is the same as coded order
		//we assume for baseline h264, the display order is the same as coded order(it's not compatible with h264 spec.)
		h264dec->iReorderDelay = 0;
	}else{
		h264dec->iReorderDelay = DPBSize >= 0? DPBSize : 16;
	}

	if(h264dec->iReorderDelayCustomSet != -1) {
		assist_myecho("User defined reoder delay %d is adopted.\n", h264dec->iReorderDelayCustomSet);
		h264dec->iReorderDelay = h264dec->iReorderDelayCustomSet;
	}

	assist_myecho("+++++ DPBsize is %d, POC type is %d, reorderdelay is %d ++++++\n", DPBSize, PocType, h264dec->iReorderDelay);

	return;
}


#define IPPH264DEC_FRAMEBUF_MAXSZ_LIMIT			(60*1024*1024)		//60M byte
#define IPPH264DEC_FRAMEBUF_MAXCNT_LIMIT		40

static inline int
should_decoder_fire_add1buf_cmd(GstH264Dec* h264dec)
{
	return (h264dec->iRawDecoder_FrameBufSize < IPPH264DEC_FRAMEBUF_MAXSZ_LIMIT && h264dec->iRawDecoder_FrameBufCnt < IPPH264DEC_FRAMEBUF_MAXCNT_LIMIT)? 1 : 0;
}

static inline int
decoder_fire_add1buf_cmd(GstH264Dec* h264dec)
{
	assist_myecho("Fire a command to decoder to increase 1 frame buffer\n");
	return DecodeSendCmd_H264Video(IPPVC_MORE_FRAMEBUFFUER, NULL, NULL, h264dec->pH264DecState) == IPP_STATUS_NOERR ? 0 : -1;
}

static int
wait_sink_free_oneframe(GstH264Dec* h264dec)
{
#define WAITSINK_SLEEP_TIME		10000
	unsigned int totalsleeptime = 0;
	while(totalsleeptime <= 500000) {	//half second
		//clear the flag
		h264dec->iRawDecoderFrameFreedFlag = 0;
		//sleep
		sleep(WAITSINK_SLEEP_TIME);
		totalsleeptime += WAITSINK_SLEEP_TIME;
		//check the flag
		if(h264dec->iRawDecoderFrameFreedFlag > 0) {
			return 0;
		}
	}
	return -1;
}


//consume_nals() return < 0, fatal error happend, return = 0 , normal decoding, return > 0, common error happen
static int
consume_nals(GstH264Dec* h264dec, unsigned char* pData, int len, int* iDecoderOutFrame)
{
#ifdef DEBUG_PERFORMANCE
	struct timeval clk0, clk1;
#endif

	h264dec->srcBitStream.pBsBuffer = pData;
	h264dec->srcBitStream.bsCurBitOffset = 0;
	IppCodecStatus	codecret;
	int consumedlen;
	while(len > 0) {
		h264dec->srcBitStream.bsByteLen = len;
		h264dec->srcBitStream.pBsCurByte = h264dec->srcBitStream.pBsBuffer;

		
#ifdef DEBUG_PERFORMANCE
		gettimeofday(&clk0, NULL);
#endif
		codecret = DecodeFrame_H264Video(&(h264dec->srcBitStream), &(h264dec->pDstPicList), h264dec->pH264DecState, &(h264dec->nAvailFrames));
#ifdef DEBUG_PERFORMANCE
		gettimeofday(&clk1, NULL);
		h264dec->codec_time += (clk1.tv_sec - clk0.tv_sec)*1000000 + (clk1.tv_usec - clk0.tv_usec);
#endif
		
		consumedlen = h264dec->srcBitStream.pBsCurByte - h264dec->srcBitStream.pBsBuffer;
		len -= consumedlen;
		h264dec->srcBitStream.pBsBuffer = h264dec->srcBitStream.pBsCurByte;
		switch(codecret) {
			case IPP_STATUS_BUFFER_FULL:
				assist_myecho("Decoder return IPP_STATUS_BUFFER_FULL\n");
				if(h264dec->DownStreamBufType != DOWNBUF_TYPE_sI42) {
					if(should_decoder_fire_add1buf_cmd(h264dec) && (decoder_fire_add1buf_cmd(h264dec)==0)) {
						assist_myecho("Send command to decoder to increase frame buffer successfully!");
					}else{
						//fatal error happen
						g_warning("Decoder return buffer full, but increase a frame buffer fail and raw IPP h264 decoder has allocated %d frames.", h264dec->iRawDecoder_FrameBufCnt);
						GST_ERROR_OBJECT(h264dec, "Decoder return buffer full, but increase a frame buffer fail and raw IPP h264 decoder has allocated %d frames.", h264dec->iRawDecoder_FrameBufCnt);
						return -1;
					}
				}else{
					int bAddedFrame = 0;
					if(should_decoder_fire_add1buf_cmd(h264dec)) {
						bAddedFrame = ! decoder_fire_add1buf_cmd(h264dec);
					}
					if(bAddedFrame == 0){
						if(wait_sink_free_oneframe(h264dec) != 0) {
							assist_myecho("Sink couldn't free any frame.\n");
							g_warning("Sink couldn't free any frame.");
							GST_WARNING_OBJECT(h264dec, "Sink couldn't free any frame.");
							//give up decoding those data
							return 1; 
						}
					}
				}
				break;
			case IPP_STATUS_FRAME_COMPLETE:

				if(G_LIKELY(h264dec->nAvailFrames != 0)) {
					if(is_frame_already_in_reorderpool(h264dec->DecodedFramesReorderPool, h264dec->pDstPicList->pPic)) {
						g_warning("Decoder output repeated frames in reorder pool!!!");
						GST_ERROR_OBJECT(h264dec, "Decoder output repeated frames in reorder pool!!!");
						return -5;
					}else if(h264dec->pDstPicList->pPic->picPlaneStep[IPP_MAXPLANENUM-1] == 1) {
						g_warning("Decoder output repeated frames in disp pool!!!");
						GST_ERROR_OBJECT(h264dec, "Decoder output repeated frames in disp pool!!!");
						return -4;
					}else if((h264dec->pDstPicList->pPic->picStatus & 0x100) != 0) {
						//it's the end of video sequence, and the new video sequence will be started.
						//There are some repeated frames in pDstPicList, because those frame isn't freed in reorder pool or display queue
						g_warning("Decoder output repeated frames before new video sequence begin. Plugin don't support it currently!!!");
						GST_ERROR_OBJECT(h264dec, "Decoder output repeated frames before new video sequence begin. Plugin don't support it currently!!!");
						return -6;
					}else if(consumedlen == 0) {
						//for error bit stream, decoder probably output the decoded frame only after meet a new compressed frame data but don't consume any bits of the new frame data
						assist_myecho("raw decoder output %d frames without consuming any bits!!!!!!!\n", h264dec->nAvailFrames);
						while(h264dec->nAvailFrames > 0) {
							//ignore those corrupted frames
							h264dec->pDstPicList->pPic->picStatus |= 1;
							h264dec->nAvailFrames--;
							h264dec->pDstPicList = h264dec->pDstPicList->pNextPic;
						}
					}else{
#ifdef DEBUG_PERFORMANCE
						h264dec->totalFrames += h264dec->nAvailFrames;
#endif
						*iDecoderOutFrame = *iDecoderOutFrame + h264dec->nAvailFrames;

						h264dec->decoded_frame_cnt += h264dec->nAvailFrames;
						_h264_process_decodedframes(h264dec, h264dec->nAvailFrames, h264dec->pDstPicList);
					}
				}else{

				}

				h264dec->nAvailFrames = 0;
				h264dec->pDstPicList = NULL;
				break;
			case IPP_STATUS_NEW_VIDEO_SEQ:
				assist_myecho("Decoder return New video seq\n");
				//give up all existing frames
				_h264_clean_ReorderPool(h264dec);

				_h264_gather_decoder_info(h264dec);
				if(h264dec->iSmartModeByDimension == -1) {
					IppiSize frameWH;
					int dim;
					if(IPP_STATUS_NOERR != DecodeSendCmd_H264Video(IPPVC_GET_PICSIZE, NULL, &frameWH, h264dec->pH264DecState)) {
						//better to stop decoding because the decoder state isn't correct
						g_warning("Send command IPPVC_GET_PICSIZE fail.");
						GST_ERROR_OBJECT(h264dec, "Send command IPPVC_GET_PICSIZE fail.");
						return -3;
					}
					dim = set_thresholds(h264dec, frameWH.width, frameWH.height);
					if(h264dec->skip_strategy == 2 && h264dec->iSmartModeByDimension != h264dec->iCurSmartSkipMode) {
						set_new_smartskipmode(h264dec, h264dec->iSmartModeByDimension);
					}else if(h264dec->skip_strategy == 1 && dim >= 1280*720) {
						//for big stream, set the init smartskipmode to SMARTSKIPMODE_PERFORMANCE_PREFER_LITTLE
						set_new_smartskipmode(h264dec, SMARTSKIPMODE_PERFORMANCE_PREFER_LITTLE);
					}
				}

				break;
			case IPP_STATUS_NOERR:
				//no special handling
				break;
			case IPP_STATUS_NOTSUPPORTED_ERR:
				g_warning("DecodeFrame_H264Video() return not supported(%d), stop decoding.\n", codecret);
				GST_ERROR_OBJECT(h264dec, "DecodeFrame_H264Video() return not supported(%d), stop decoding.\n", codecret);
				return -7;
			case IPP_STATUS_SYNCNOTFOUND_ERR:
				//break;
			case IPP_STATUS_BITSTREAM_ERR:
			case IPP_STATUS_INPUT_ERR:
				assist_myecho("Bitstream error occur, DecodeFrame_H264Video() ret %d, give up decoding this data fragment.\n", codecret);
				//stream error, give up decoding those data
				return 1;
			default:
				//fatal error happen, it's better to finish decoding
				g_warning("Decoder return an undefined error %d.", codecret);
				GST_ERROR_OBJECT(h264dec, "Decoder return an undefined error %d.", codecret);
				return -2;
		}

	}
	return 0;
}

static int 
check_structure_hold_somefourcc(GstStructure* CapStruct, guint32 somefourcc) {
	if(CapStruct == NULL) {
		return 0;
	}
	if(! gst_structure_has_field(CapStruct, "format")) {
		return 0;
	}else{
		const GValue* format_value = gst_structure_get_value(CapStruct, "format");
		if(format_value == NULL) {
			return 0;
		}else{
			guint32 forcc;
			if(GST_VALUE_HOLDS_LIST(format_value)) {
				//fourcc list
				int cnt = gst_value_list_get_size(format_value);
				int i;
				for(i=0; i<cnt; i++) {
					forcc = gst_value_get_fourcc(gst_value_list_get_value(format_value, i));
					if(forcc == somefourcc) {
						return 1;
					}
				}
			}else{
				forcc = gst_value_get_fourcc(format_value);
				if(forcc == somefourcc) {
					return 1;
				}
			}
		}
	}
	return 0;
}

static int 
check_downpeercaps_contain_sI42(GstH264Dec *h264dec)
{
	int bContain_sI42 = 0;
	GstCaps* DownPeerCaps = gst_pad_peer_get_caps(h264dec->srcpad);
	if(DownPeerCaps != NULL){
		int cnt = gst_caps_get_size(DownPeerCaps);
		int i;
		for(i=0; i<cnt; i++) {
			bContain_sI42 = check_structure_hold_somefourcc(gst_caps_get_structure(DownPeerCaps, i), GST_STR_FOURCC (H264DEC_4CC_sI42STR));
			if(bContain_sI42 != 0) {
				break;
			}
		}
		gst_caps_unref(DownPeerCaps);
	}
	assist_myecho("Down peer plug-in !!! %s !!! support sI42.\n", bContain_sI42==0 ? "DOES NOT":"DOES");
	return bContain_sI42;
}

static void
closebmmlib(GstH264Dec* h264dec)
{
	const char* dlmsg;
	dlclose(h264dec->BmmLibHandle);
	h264dec->BmmLibHandle = NULL;
	dlmsg = dlerror();
	if(dlmsg != NULL) {
		g_warning("dlclose is fail, ret string is %s", dlmsg);
		GST_WARNING_OBJECT(h264dec, "dlclose is fail, ret string is %s", dlmsg);
	}
	return;
}

static int
openbmmlib(GstH264Dec* h264dec)
{
	const char * pErrMsg;
	h264dec->BmmLibHandle = dlopen(BMMBINARY_NAME, RTLD_LAZY);
	pErrMsg = dlerror();
	if(pErrMsg != NULL) {
		assist_myecho("open %s fail, return string %s\n", BMMBINARY_NAME, pErrMsg);
	}
	if(h264dec->BmmLibHandle == NULL) {
		return -1;
	}
	h264dec->pfunBmmMalloc = dlsym(h264dec->BmmLibHandle, "bmm_malloc");
	pErrMsg = dlerror();
	if(pErrMsg != NULL) {
		assist_myecho("get bmm_malloc fail, return string %s\n", pErrMsg);
	}
	h264dec->pfunBmmFree = dlsym(h264dec->BmmLibHandle, "bmm_free");
	pErrMsg = dlerror();
	if(pErrMsg != NULL) {
		assist_myecho("get bmm_free fail, return string %s\n", pErrMsg);
	}

	h264dec->pfunBmmGetPaddr = dlsym(h264dec->BmmLibHandle, "bmm_get_paddr");
	pErrMsg = dlerror();
	if(pErrMsg != NULL) {
		assist_myecho("get bmm_get_paddr fail, return string %s\n", pErrMsg);
	}

	if(h264dec->pfunBmmMalloc == NULL || h264dec->pfunBmmFree == NULL || h264dec->pfunBmmGetPaddr == NULL) {
		assist_myecho("get bmm_malloc/free/get_paddr function fail\n");
		h264dec->pfunBmmMalloc = NULL;
		h264dec->pfunBmmFree = NULL;
		h264dec->pfunBmmGetPaddr = NULL;
		
		dlclose(h264dec->BmmLibHandle);
		dlerror();
		h264dec->BmmLibHandle = NULL;
		return -1;
	}
	return 0;
}



static void*
h264_rawdecoder_frameMalloc(int size, int alignment, void* pUsrData)
{
	assist_myecho("IPP h264 raw decoder frame malloc is called!-----------\n");
	GstH264Dec* h264dec = (GstH264Dec*)pUsrData;
	void * pointer = NULL;
	if(h264dec->DownStreamBufType == DOWNBUF_TYPE_sI42) {
		assist_myecho("IPP h264 raw decoder frame malloc is using bmm!\n");
		pointer = h264dec->pfunBmmMalloc(size, BMMBUFFER_ATTR_FORGST);
		if(pointer && (((unsigned int)pointer & (alignment-1)) != 0)) {
			g_warning("the bmm allocated memory(addr:%x) isn't %d aligned", (unsigned int)pointer, alignment);
			GST_ERROR_OBJECT(h264dec, "the bmm allocated memory(addr:%x) isn't %d aligned", (unsigned int)pointer, alignment);
			h264dec->pfunBmmFree(pointer);
			pointer = NULL;
		}
	}else{
		h264dec->pCBTable->fMemMalloc(&pointer, size, alignment);
	}

	if(pointer) {
		h264dec->iRawDecoder_FrameBufSize += size;
		h264dec->iRawDecoder_FrameBufCnt ++;
	}else{
		g_warning("IPP h264 raw decoder frame malloc fail!!!");
		GST_ERROR_OBJECT(h264dec, "IPP h264 raw decoder frame malloc fail!!!");
	}
	return pointer;

}

static void
h264_rawdecoder_frameFree(void* pointer, void* pUsrData)
{
	assist_myecho("IPP h264 raw decoder frame free is called!-----------\n");
	GstH264Dec* h264dec = (GstH264Dec*)pUsrData;
	if(h264dec->DownStreamBufType == DOWNBUF_TYPE_sI42) {
		assist_myecho("IPP h264 raw decoder frame free is using bmm!\n");
		h264dec->pfunBmmFree(pointer);
	}else{
		h264dec->pCBTable->fMemFree(&pointer);
	}
	return;
}

static int
consume_inputbuf_ts(GstH264Dec* h264dec, GstBuffer* buf)
{
	int bValidTs = 0;

	//judge whether the TS is valid
	if(G_LIKELY(GST_BUFFER_TIMESTAMP(buf) != GST_CLOCK_TIME_NONE)) {
		if(G_LIKELY(h264dec->LastInputValidTs != GST_CLOCK_TIME_NONE)) {
			guint64 diff;
			if(GST_BUFFER_TIMESTAMP(buf) > h264dec->LastInputValidTs) {
				diff = GST_BUFFER_TIMESTAMP(buf) - h264dec->LastInputValidTs;
			}else{
				diff = h264dec->LastInputValidTs - GST_BUFFER_TIMESTAMP(buf);
			}
			if(diff >= h264dec->iFixedFRateDuration_5_8) {
				bValidTs = 1;
			}else{
                assist_myecho("TS isn't valid, diff is %lld, FRDu_5_8 is %lld\n", diff, h264dec->iFixedFRateDuration_5_8);
            }
		}else{
			bValidTs = 1;
		}
 	}
	//store the TS in the queue
	if(bValidTs != 0) {
		TsDuPair* pTsDu = g_slice_new(TsDuPair);
		h264dec->LastInputValidTs = GST_BUFFER_TIMESTAMP(buf);
		pTsDu->ts = GST_BUFFER_TIMESTAMP(buf);
		pTsDu->du = GST_BUFFER_DURATION(buf);
		g_queue_push_tail(&h264dec->inTsQueue, pTsDu);
		return 1;
	}else{
        assist_myecho("input TS %lld isn't valid\n", GST_BUFFER_TIMESTAMP(buf));
		return 0;
    }
}

static GstFlowReturn
gst_h264dec_chain(GstPad *pad, GstBuffer *buf)
{
	GstH264Dec* h264dec = GST_H264DEC (GST_OBJECT_PARENT (pad));

	int i;
	guint8 *data;
	int data_size;
	int Ret = 0;
	int nalslen;
	int iDecOutFrameIn1Input = 0;
	int	bInputDataContainFrame = 0;

	//query downstream cap and decide down buffer type
	if(G_UNLIKELY(h264dec->DownStreamBufType == DOWNBUF_TYPE_UNDECIDED)) {
		h264dec->DownStreamBufType = DOWNBUF_TYPE_I420;
		assist_myecho("Current bForbid_sI42 has been set to %d.\n", h264dec->bForbid_sI42);
		if(h264dec->bForbid_sI42 == 0 && check_downpeercaps_contain_sI42(h264dec) != 0) {
			if(openbmmlib(h264dec) == 0) {
				int delay_disable = 1;
				if(IPP_STATUS_NOERR == DecodeSendCmd_H264Video(IPPVC_SET_OUTPUTDELAYDISABLE, &(delay_disable), NULL, h264dec->pH264DecState)) {
					h264dec->DownStreamBufType = DOWNBUF_TYPE_sI42;
				}else{
					closebmmlib(h264dec);
				}
			}
		}
		assist_myecho("Down buf type is %s\n", h264dec->DownStreamBufType == DOWNBUF_TYPE_sI42 ? H264DEC_4CC_sI42STR : H264DEC_4CC_I420STR);
	}


	data = GST_BUFFER_DATA(buf);
	data_size  = GST_BUFFER_SIZE(buf);

    //assist_myecho("input TS is %lld, du is %lld, fragidx %d\n", GST_BUFFER_TIMESTAMP(buf),GST_BUFFER_DURATION(buf), h264dec->demuxer_datafrag_cnt);
	h264dec->demuxer_datafrag_cnt++;

	if(h264dec->is_playing == 1) {
		if(h264dec->skip_strategy == 1) {
			switch_smartskipmode(h264dec);
		}

		if(G_UNLIKELY(h264dec->iCurSmartSkipMode == SMARTSKIPMODE_PERFORMANCE_PREFER_MOST)) {
			int bRoughSkipData = should_roughskip_datafrag(h264dec, data, data_size);
			if(bRoughSkipData != 0) {
				//rough skip
				bInputDataContainFrame = consume_inputbuf_ts(h264dec, buf);	//if the TS is valid and accepted by consume_inputbuf_ts, we consider the input data contains at least one frame. If inputdata only contain SPS and PPS nal, the timestamp is GST_CLOCK_TIME_NONE, therefore, bInputDataContainFrame = 0.
				gst_buffer_unref(buf);
				h264dec->datafrag_roughskipped_cnt++;
				if(h264dec->skip_logfile != NULL) {
					_h264_log_frameskip(h264dec, 1, 1);
				}
				if(bInputDataContainFrame) {
					//the current frame is rough skipped by decoder, fill one dummy decoded frame
					_h264_process_decodedframes(h264dec, 1, &h264dec->DummyPicList);
				}
				return GST_FLOW_OK;
			}
		}
	}


	//log this data fragment isn't rough skipped
	if(h264dec->skip_logfile != NULL) {
		_h264_log_frameskip(h264dec, 1, 0);
	}

	bInputDataContainFrame = consume_inputbuf_ts(h264dec, buf);
	
	if(G_LIKELY(data_size>0)) {
		if(h264dec->ForceCopyStreamBuf || !gst_buffer_is_writable(buf)) {
			//IPP h264 decoder probably modify the data in stream buffer, therefore, make sure the stream buffer is writable
			if(h264dec->pSelfStreamBuf == NULL) {
				h264dec->iSelfStreamBufSz = data_size < GST_IPPH264DEC_SELFSTREAMBUF_MINSZ ? GST_IPPH264DEC_SELFSTREAMBUF_MINSZ : data_size;
				h264dec->pSelfStreamBuf = g_malloc(h264dec->iSelfStreamBufSz);
				assist_myecho("Create internal stream buffer %x, size %d\n", h264dec->pSelfStreamBuf, h264dec->iSelfStreamBufSz);
			}else if(h264dec->iSelfStreamBufSz < data_size) {
				g_free(h264dec->pSelfStreamBuf);
				h264dec->pSelfStreamBuf = g_malloc(data_size);
				h264dec->iSelfStreamBufSz = data_size;
				assist_myecho("Re-Create internal stream buffer %x, size %d\n", h264dec->pSelfStreamBuf, h264dec->iSelfStreamBufSz);
			}
			data = memcpy(h264dec->pSelfStreamBuf, data, data_size);
		}
	}


	while (data_size > 0){

		if(h264dec->is_avcC == TRUE) {
			if(data_size <= h264dec->nal_length_size) {
				//error bitstream
				break;
			}
			nalslen = 0;
			for(i=0; i<h264dec->nal_length_size; i++){
				nalslen = nalslen <<8 | *data++;				
			}
			data_size -= h264dec->nal_length_size;
		}else{
			//h264 byte stream format(h.264 annex B) with sync code
			nalslen = data_size;
		}

		if(G_UNLIKELY(nalslen > data_size)) {
			//for error stream, probably nalslen > data_size
			nalslen = data_size;
		}
		Ret = consume_nals(h264dec, data, nalslen, &iDecOutFrameIn1Input);
		data += nalslen;
		data_size -= nalslen;
		if(Ret < 0) {
			//consume_nals return a fatal error
			goto CHAIN_END;
		}
	}

CHAIN_END:
	gst_buffer_unref(buf);
	if(h264dec->skip_logfile && bInputDataContainFrame) {
		_h264_log_frameskip(h264dec, 0, !iDecOutFrameIn1Input);
	}
	if(iDecOutFrameIn1Input == 0 && bInputDataContainFrame) {
		h264dec->decoder_frameskipped_cnt++;
		//the current frame is skipped by decoder, fill one dummy decoded frame
		_h264_process_decodedframes(h264dec, 1, &h264dec->DummyPicList);
	}
	if(iDecOutFrameIn1Input > 1) {
		assist_myecho("Wrong: decoded multiple frame %d during one input fragment %d\n", iDecOutFrameIn1Input, h264dec->demuxer_datafrag_cnt);
	}

	if(G_LIKELY(Ret >= 0)) {
		return GST_FLOW_OK;
	}else{
		g_warning("!!!!! h264dec in chain() fatal error");
		GST_ELEMENT_ERROR(h264dec, STREAM, DECODE, (NULL), ("%s() return un-accepted error, reason %d", __FUNCTION__, Ret));

		return GST_FLOW_ERROR;
	}

}

static inline void
gst_h264dec_init_members(GstH264Dec *h264dec)
{
	assist_myecho("enter %s begin\n", __FUNCTION__);
    h264dec->pCBTable = NULL;
	h264dec->pH264DecState = NULL;
	h264dec->pDstPicList = NULL;

	h264dec->nAvailFrames = 0;

	h264dec->iDownFrameWidth = 0;
	h264dec->iDownFrameHeight = 0;
	h264dec->iFRateNum = 0;
	h264dec->iFRateDen = 0;
	h264dec->iFixedFRateDuration = 0;
	h264dec->iFixedFRateDuration_5_8 = 0;
	h264dec->LastInputValidTs = GST_CLOCK_TIME_NONE;

	h264dec->is_avcC = FALSE;
	h264dec->nal_length_size = 0;

	memset(&h264dec->srcBitStream, 0, sizeof(IppBitstream));

	h264dec->iNominalProfile = GSTH264DEC_PROFILE_UNKNOWN;
	g_queue_init(&h264dec->inTsQueue);
	h264dec->DecodedFramesReorderPool = NULL;
#ifdef REORDER_BASEONPOC
	h264dec->TSReorderPool = NULL;
#endif
	h264dec->iDecodedFramesReorderPoolLen = 0;
	h264dec->iReorderDelay = GSTH264DEC_REORDERDELAY_UNDECIDED;
	h264dec->iReorderDelayCustomSet = -1;
	h264dec->bNotReorderForBaseline = 1;

	h264dec->bReorderOnSinkBuffers = 1;

	h264dec->skip_strategy = 1;

	h264dec->is_playing = 0;

	h264dec->iCurSmartSkipMode = SMARTSKIPMODE_NORMALDECODE;
	h264dec->iSmartModeByDimension = -1;
	h264dec->bForbid_RoughSkip = 0;
	h264dec->iCurRoughSkipDataPeriod = ROUGH_SKIPDATA_PERIOD_MAX;
	h264dec->iRoughSkipDataIndicator = ROUGH_SKIPDATA_PERIOD_MAX-1;

	h264dec->QosProtect_forNewSeq = QOSLOG_PROTECT_FORNEWSEG;
	h264dec->iNewQosReportedCnt = 0;
	h264dec->iShortTermDropRate = 0;
	h264dec->iLongTermDropRate = 0;
	h264dec->iNextRecordIdx_S = 0;
	h264dec->iNextRecordIdx_L = 0;
	memset(h264dec->DropRecordWnd_S, 0, sizeof(h264dec->DropRecordWnd_S));
	memset(h264dec->DropRecordWnd_L, 0, sizeof(h264dec->DropRecordWnd_L));


	h264dec->DummyPic.ppPicPlane[0] = NULL;		//ppPicPlane[0] == 0 means it's a dummy picture
	h264dec->DummyPic.ppPicPlane[1] = (void*)1;	//just indicator, to be different from picture attached with gstbuffer
	h264dec->DummyPic.ppPicPlane[2] = (void*)1;
	h264dec->DummyPic.picOrderCnt = 0x80000001;
	h264dec->DummyPicList.pPic = &(h264dec->DummyPic);
	h264dec->DummyPicList.pNextPic = NULL;

	h264dec->skip_logfilename = NULL;
	h264dec->skip_logfile = NULL;

	h264dec->demuxer_datafrag_cnt = 0;
	h264dec->datafrag_roughskipped_cnt = 0;
	h264dec->decoder_frameskipped_cnt = 0;
	h264dec->decoded_frame_cnt = 0;
	h264dec->frame_suc_pushed2sink_cnt = 0;
	h264dec->smartmode_switchcnt = 0;

	h264dec->iRawDecoder_FrameBufSize = 0;
	h264dec->iRawDecoder_FrameBufCnt = 0;

	h264dec->bForbid_sI42 = 1;	//sI42 is legacy on Aspen, not support it on other platform currently 2011.03. Therefore, forbid sI42 by default.
	h264dec->DownStreamBufType = DOWNBUF_TYPE_UNDECIDED;
	h264dec->BmmLibHandle = NULL;

	h264dec->iRawDecoderFrameFreedFlag = 0;

	h264dec->ResFinalizer = NULL;

#ifndef GST_IPPH264DEC_FORCE_COPYSTREAMBUF
	h264dec->ForceCopyStreamBuf = 0;
#else
	h264dec->ForceCopyStreamBuf = 1;
#endif
	h264dec->pSelfStreamBuf = NULL;
	h264dec->iSelfStreamBufSz = 0;

	h264dec->bNewSegReceivedAfterflush = 0;

#ifdef DEBUG_PERFORMANCE
	h264dec->totalFrames = 0;
	h264dec->codec_time = 0;
#endif

	assist_myecho("enter %s end\n", __FUNCTION__);
	return;
}

static gboolean
gst_h264dec_null2ready(GstH264Dec *h264dec)
{
	IppCodecStatus codecret;

	int delay_disable;

	delay_disable= 1;
	if(h264dec->bReorderOnSinkBuffers == 1) {
		//when raw decoder is working under delay_disable=2 mode, it won't hold any redundant buffer to do re-order.
		delay_disable = 2;
	}
	assist_myecho("Set the decoder working delay_disable mode %d\n", delay_disable);

	if(h264dec->pCBTable == NULL) {
		miscInitGeneralCallbackTable(&(h264dec->pCBTable));
	}

	if(h264dec->pH264DecState == NULL) {
		
		codecret = DecoderInitAlloc_H264Video(h264dec->pCBTable, &(h264dec->pH264DecState));
		
		if (codecret != IPP_STATUS_NOERR) {
			g_warning("Init h264 decoder fail, ret is %d\n", codecret);
			GST_ERROR_OBJECT(h264dec, "Init h264 decoder fail, ret is %d\n", codecret);
			return FALSE;
		} else {
			int NSCCheckDisable = 1;
			FrameMemOpSet MemOpsObj;
			
			h264dec->iCurSmartSkipMode = SMARTSKIPMODE_NORMALDECODE;	//the default mode
			/*disable codec seek next sync code, for we are using parser*/
			codecret = DecodeSendCmd_H264Video(IPPVC_SET_NSCCHECKDISABLE, &NSCCheckDisable, NULL, h264dec->pH264DecState);
			if(codecret != IPP_STATUS_NOERR) {
				g_warning("Send IPPVC_SET_NSCCHECKDISABLE to h264 decoder fail, ret is %d", codecret);
				GST_ERROR_OBJECT(h264dec, "Send IPPVC_SET_NSCCHECKDISABLE to h264 decoder fail, ret is %d", codecret);
				return FALSE;
			}

			codecret = DecodeSendCmd_H264Video(IPPVC_SET_OUTPUTDELAYDISABLE, &(delay_disable), NULL, h264dec->pH264DecState);
			if(codecret != IPP_STATUS_NOERR) {
				g_warning("Send IPPVC_SET_OUTPUTDELAYDISABLE to h264 decoder fail, ret is %d", codecret);
				GST_ERROR_OBJECT(h264dec, "Send IPPVC_SET_OUTPUTDELAYDISABLE to h264 decoder fail, ret is %d", codecret);
				return FALSE;
			}

			h264dec->iRawDecoder_FrameBufSize = 0;
			h264dec->iRawDecoder_FrameBufCnt = 0;
			MemOpsObj.fMallocFrame = h264_rawdecoder_frameMalloc;
			MemOpsObj.fFreeFrame = h264_rawdecoder_frameFree;
			MemOpsObj.pUsrObj = (void*)h264dec;
			codecret = DecodeSendCmd_H264Video(IPPVC_SET_FRAMEMEMOP, &MemOpsObj, NULL, h264dec->pH264DecState);
			if(codecret != IPP_STATUS_NOERR) {
				g_warning("Send IPPVC_SET_FRAMEMEMOP to h264 decoder fail, ret is %d", codecret);
				GST_ERROR_OBJECT(h264dec, "Send IPPVC_SET_FRAMEMEMOP to h264 decoder fail, ret is %d", codecret);
				return FALSE;
			}

		}

	}

	//create resource finalizer
	if(h264dec->ResFinalizer == NULL) {
		h264dec->ResFinalizer = (void*)gst_vdecbuffer_new();
	}
	GST_BUFFER_DATA(h264dec->ResFinalizer) = NULL;
	IPPGST_BUFFER_CUSTOMDATA(h264dec->ResFinalizer) = (gpointer)gst_object_ref(h264dec);	//ref h264dec, so the ResFinalizer should be finalized before h264dec



#ifdef DUMP_TS_FILE
	fpOutTs = fopen(DUMP_TS_FILE, "wt");
#endif

	return TRUE;
}

static void
gst_h264dec_finalize_res(GstH264Dec *h264dec)
{
	if(h264dec->pH264DecState != NULL) {
		DecoderFree_H264Video(&(h264dec->pH264DecState));
		h264dec->pH264DecState = NULL;
	}
	if(h264dec->pCBTable != NULL) {
		miscFreeGeneralCallbackTable(&(h264dec->pCBTable));
		h264dec->pCBTable = NULL;
	}
	if(h264dec->BmmLibHandle != NULL) {
		closebmmlib(h264dec);
	}
	assist_myecho("Gsth264dec resource is finalized!!!!\n");
	return;
}

static gboolean
gst_h264dec_ready2null(GstH264Dec *h264dec)
{
#ifdef DUMP_TS_FILE
	fclose(fpOutTs);
#endif
	if(h264dec->pSelfStreamBuf) {
		assist_myecho("Free internal stream buffer %x, size %d\n", h264dec->pSelfStreamBuf, h264dec->iSelfStreamBufSz);
		g_free(h264dec->pSelfStreamBuf);
		h264dec->pSelfStreamBuf = NULL;
		h264dec->iSelfStreamBufSz = 0;
	}

	//smart release resource based on refcount
	gst_buffer_unref(h264dec->ResFinalizer);

	if(h264dec->skip_logfile != NULL) {
		fprintf(h264dec->skip_logfile, "\n--------frame skip information summary-----------\n");
		fprintf(h264dec->skip_logfile, "Demuxer provided %d data fragment.\n", h264dec->demuxer_datafrag_cnt);
		fprintf(h264dec->skip_logfile, "Decoder plugin rough skipped %d data fragment.\n", h264dec->datafrag_roughskipped_cnt);
		fprintf(h264dec->skip_logfile, "Raw decoder frame skipping happened %d.\n", h264dec->decoder_frameskipped_cnt);
		fprintf(h264dec->skip_logfile, "Raw decoder output %d frames.\n", h264dec->decoded_frame_cnt);
		fprintf(h264dec->skip_logfile, "Docoder plugin pushed %d frames to sink.\n", h264dec->frame_suc_pushed2sink_cnt);
		fprintf(h264dec->skip_logfile, "The skip strategy is %d, latest decoder skip mode is %d, mode switch about occur %d.\n", h264dec->skip_strategy, h264dec->iCurSmartSkipMode, h264dec->smartmode_switchcnt);
		fclose(h264dec->skip_logfile);
		h264dec->skip_logfile = NULL;
	}

#if 1
	assist_myecho("\n--------frame skip information summary-----------\n");
	assist_myecho("Demuxer provided %d data fragment.\n", h264dec->demuxer_datafrag_cnt);
	assist_myecho("Decoder plugin rough skipped %d data fragment.\n", h264dec->datafrag_roughskipped_cnt);
	assist_myecho("Raw decoder frame skipping happened %d.\n", h264dec->decoder_frameskipped_cnt);
	assist_myecho("Raw decoder output %d frames.\n", h264dec->decoded_frame_cnt);
	assist_myecho("Docoder plugin pushed %d frames to sink.\n", h264dec->frame_suc_pushed2sink_cnt);
	assist_myecho("The skip strategy is %d, latest decoder skip mode is %d, mode switch about occur %d.\n", h264dec->skip_strategy, h264dec->iCurSmartSkipMode, h264dec->smartmode_switchcnt);
	assist_myecho("Raw IPP h264 decoder totally allocated %d frame buffers(%d bytes).\n", h264dec->iRawDecoder_FrameBufCnt, h264dec->iRawDecoder_FrameBufSize);
#else
	printf("\n--------frame skip information summary-----------\n");
	printf("Demuxer provided %d data fragment.\n", h264dec->demuxer_datafrag_cnt);
	printf("Decoder plugin rough skipped %d data fragment.\n", h264dec->datafrag_roughskipped_cnt);
	printf("Raw decoder frame skipping happened %d.\n", h264dec->decoder_frameskipped_cnt);
	printf("Raw decoder output %d frames.\n", h264dec->decoded_frame_cnt);
	printf("Docoder plugin pushed %d frames to sink.\n", h264dec->frame_suc_pushed2sink_cnt);
	printf("The skip strategy is %d, latest decoder skip mode is %d, mode switch about occur %d.\n", h264dec->skip_strategy, h264dec->iCurSmartSkipMode, h264dec->smartmode_switchcnt);
	printf("Raw IPP h264 decoder totally allocated %d frame buffers(%d bytes).\n", h264dec->iRawDecoder_FrameBufCnt, h264dec->iRawDecoder_FrameBufSize);
#endif


#ifdef DEBUG_PERFORMANCE
	printf("codec system time %lld usec, frame number %d\n", h264dec->codec_time, h264dec->totalFrames);
#endif

#ifdef DEBUG_PERFORMANCE_GETMEMCPYTIME
	printf("frame memcpy system time %d usec\n", memcpy_time);
#endif
	return TRUE;
}

static gboolean
gst_h264dec_ready2pause(GstH264Dec *h264dec)
{
	if(h264dec->skip_logfile==NULL && h264dec->skip_logfilename != NULL && h264dec->skip_logfilename[0] != '\0') {
		h264dec->skip_logfile = fopen(h264dec->skip_logfilename, "wt");
	}
	//prepare to decode new stream
	h264dec->iNominalProfile = GSTH264DEC_PROFILE_UNKNOWN;

	h264dec->iReorderDelay = GSTH264DEC_REORDERDELAY_UNDECIDED;

	h264dec->LastInputValidTs = GST_CLOCK_TIME_NONE;

	_h264_clean_ReorderPool(h264dec);
	_h264_clean_inTsQueue(h264dec);

	h264dec->iDownFrameWidth = 0;
	h264dec->iDownFrameHeight = 0;

	//reset smart skip mode
	h264dec->iSmartModeByDimension = -1;
	{
		//For skip strategy 0,1,2, set decoder smart mode to normal decoding first.
		int newSmartMode = h264dec->skip_strategy<=2 ? SMARTSKIPMODE_NORMALDECODE : h264dec->skip_strategy-2;
		assist_myecho("During ready to pause, set decoder mode, skip-strategy is %d\n", h264dec->skip_strategy);
		if(newSmartMode != h264dec->iCurSmartSkipMode) {
			set_new_smartskipmode(h264dec, newSmartMode);
		}
	}


	return TRUE;
}

static GstStateChangeReturn
gst_h264dec_change_state(GstElement *element, GstStateChange transition)
{
	GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;
	GstH264Dec *h264dec = GST_H264DEC (element);
    assist_myecho("begin h264dec state change from %d to %d\n", transition>>3, transition&7);
	//only after all element in the pipeline changed their state to playing, we set the is_playing to 1. By default, set it to 0.
	h264dec->is_playing = 0;

	switch (transition)
	{
	case GST_STATE_CHANGE_NULL_TO_READY:
		GST_INFO_OBJECT (h264dec, "h264dec GST_STATE_CHANGE_NULL_TO_READY");	

		if(!gst_h264dec_null2ready(h264dec)){
			goto statechange_failed;
		}	
		
		break;
	case GST_STATE_CHANGE_READY_TO_PAUSED:
		gst_h264dec_ready2pause(h264dec);
		break;
	
	default:
		break;
	}

	ret = parent_class->change_state (element, transition);
	if (ret == GST_STATE_CHANGE_FAILURE){
		return ret;
	}

	switch (transition)
	{
	case GST_STATE_CHANGE_READY_TO_NULL:
		GST_INFO_OBJECT (h264dec, "h264dec GST_STATE_CHANGE_READY_TO_NULL");
		
		if(!gst_h264dec_ready2null(h264dec)){
			goto statechange_failed;
		}
		break;

	//only after all element in the pipeline changed their state to playing, we set the is_playing to 1.
	case GST_STATE_CHANGE_PAUSED_TO_PLAYING:
		h264dec->is_playing = 1;
		break;

	default:
		break;
	}

    assist_myecho("end h264dec state change from %d to %d\n", transition>>3, transition&7);
	return ret;

statechange_failed:
	{
		/* subclass must post a meaningfull error message */
		GST_ERROR_OBJECT (h264dec, "state change failed");
		return GST_STATE_CHANGE_FAILURE;
	}
}


static void
gst_h264dec_finalize(GObject * object)
{
	G_OBJECT_CLASS(parent_class)->finalize(object);
#if 1
	assist_myecho("Gsth264dec instance is finalized!!!\n");
#else
	printf("Gsth264dec instance is finalized!!!\n");
#endif
	return;
}

static void
gst_h264dec_base_init(gpointer g_klass)
{
	static GstElementDetails h264dec_details = {
        "H.264 Video Decoder", 
        "Codec/Decoder/Video", 
        "H.264 Video Decoder based-on IPP & CODEC", 
        ""
	};
	GstElementClass *element_class = GST_ELEMENT_CLASS (g_klass);

	/* create pad templates */
	gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&sink_factory));
	gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&src_factory));

	/* set H264DEC element's details */
	gst_element_class_set_details (element_class, &h264dec_details);
}

static void
gst_h264dec_class_init(GstH264DecClass *klass)
{
	GObjectClass *gobject_class = (GObjectClass *) klass;
	GstElementClass *gstelement_class = (GstElementClass *) klass;

	gobject_class->set_property = gst_h264dec_set_property;
	gobject_class->get_property = gst_h264dec_get_property;

	g_object_class_install_property (gobject_class, ARG_MORE_SINKBUFFER, \
		g_param_spec_int ("more-sinkbuffer", "set re-order pool memory management", \
		"set doing frame re-order on sink allocated buffers or not(this property is only valid for I420, could be 0<do frame re-order on decoder internal buffers, it probably affects decoding speed and require decoder provide enough buffers>, 1<do frame re-order on downstream plug-in(e.g. video sink) supplied buffers, it requires sink element could provide enough buffers>", \
		0/* range_MIN */, 1/* range_MAX */, 1/* default_INIT */, G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, ARG_REORDERDELAY, \
		g_param_spec_int ("force-reorder-delay", "customer set re-order delay", \
		"Set reorder delay. Don't access this property unless you are very clear what you are doing. It could be -1<auto set re-order delay according to stream embedded information> or 0~16", \
		-1/* range_MIN */, 16/* range_MAX */, -1/* default_INIT */, G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, ARG_FORBID_SI42, \
		g_param_spec_int ("forbid-sI42", "forbit using sI42 format", \
		"Forbit sI42 format for decoded yuv data. It could be 0<not forbit sI42> or 1<forbit sI42>", \
		0/* range_MIN */, 1/* range_MAX */, 0/* default_INIT */, G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, ARG_COPYSTREAMBUF, \
		g_param_spec_int ("copy-streambuf", "copy upstream plug-in pushed buffer", \
		"Copy demuxer provided buffer data to internal buffer. It could be 0<directly use buffer from demuxer> or 1<copy>. Don't access this property unless you are clear what you are doing.", \
		0/* range_MIN */, 1/* range_MAX */, 0/* default_INIT */, G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, ARG_NOTREORDERBASELINE, \
		g_param_spec_int ("notreorder-baseline", "do not reorder for baseline h264", \
		"Not do auto reorder for baseline h264. It could be 1<not do reorder for baseline stream> or 0<do reorder for baseline stream>. If force-reorder-delay is set, notreorder-baseline is invalid.", \
		0/* range_MIN */, 1/* range_MAX */, 1/* default_INIT */, G_PARAM_READWRITE));


	/* add skip-frame arguments to the element */
	g_object_class_install_property (gobject_class, ARG_SKIP_STRATEGY, \
		g_param_spec_int ("frameskip-strategy", "set strategy of frame skipping", \
		"frame skipping strategy index (could be 0<no skip, normal decoding>, 1<dynamical skip mode based on realtime measure>, 2<auto select one fixed skip mode based on frame size>, 3<fixed skip mode: fast-deblock>, 4<fixed skip mode: non-deblock>, 5<fixed skip mode: skip 1/2 non-reference frames>, 6<fixed skip mode: skip all non-reference frames>", \
		GSTH264DEC_SKIPSTRATEGY_MIN/* range_MIN */, GSTH264DEC_SKIPSTRATEGY_MAX/* range_MAX */, GSTH264DEC_SKIPSTRATEGY_MIN/* default_INIT */, G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, ARG_FORBID_ROUGHSKIP, \
		g_param_spec_int ("forbid-roughskip", "forbit rough skipping", \
		"Forbid rough skipping. Valid only when frameskip-strategy=1. Rough skipping means not only skip B frame, but also skip I/P frame. It could be 0<not forbit> or 1<forbit>. If video size <= " G_STRINGIFY(VIDEO_MINSZ_FOR_ROUGHSKIPENABLE) ", it's always 1.", \
		0/* range_MIN */, 1/* range_MAX */, 0/* default_INIT */, G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, ARG_SKIP_LOGFILE,
		g_param_spec_string ("skip-logfile", "Skip Log File Location",
		"Location of the skip log file to write", NULL,
		G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

#ifdef DEBUG_PERFORMANCE
	g_object_class_install_property (gobject_class, ARG_TOTALFRAME,
		g_param_spec_int ("totalframes", "Number of frame",
		"Number of total decoded frames for all tracks", 0, G_MAXINT, 0, G_PARAM_READABLE));
	g_object_class_install_property (gobject_class, ARG_CODECTIME,
		g_param_spec_int64 ("codectime", "codec time (microsecond)",
		"Total pure decoding spend system time for all tracks (microsecond)", 0, G_MAXINT64, 0, G_PARAM_READABLE));
#endif

	gobject_class->finalize = gst_h264dec_finalize;
	gstelement_class->change_state = GST_DEBUG_FUNCPTR(gst_h264dec_change_state);

	GST_DEBUG_CATEGORY_INIT (h264dec_debug, "h264dec", 0, "H.264 Decode Element");
}


static void
gst_h264dec_init(GstH264Dec *h264dec, GstH264DecClass *h264dec_klass)
{
	GstElementClass *klass = GST_ELEMENT_CLASS (h264dec_klass);
	
	h264dec->sinkpad = gst_pad_new_from_template (\
		gst_element_class_get_pad_template (klass, "sink"), "sink");

	gst_pad_set_setcaps_function (h264dec->sinkpad, GST_DEBUG_FUNCPTR (gst_h264dec_setcaps));
	gst_pad_set_event_function (h264dec->sinkpad, GST_DEBUG_FUNCPTR (gst_h264dec_sinkpad_event));
	gst_pad_set_chain_function (h264dec->sinkpad, GST_DEBUG_FUNCPTR (gst_h264dec_chain));

	gst_element_add_pad (GST_ELEMENT (h264dec), h264dec->sinkpad);	


	h264dec->srcpad = gst_pad_new_from_template (
		gst_element_class_get_pad_template (klass, "src"), "src");

	gst_pad_set_event_function(h264dec->srcpad, GST_DEBUG_FUNCPTR (gst_h264dec_srcpad_event));
	gst_element_add_pad (GST_ELEMENT (h264dec), h264dec->srcpad);

	gst_h264dec_init_members(h264dec);

	return;
}

static gboolean 
plugin_init(GstPlugin *plugin)
{	
	return gst_element_register (plugin, "h264dec", GST_RANK_PRIMARY+2, GST_TYPE_H264DEC);
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR, 
                   GST_VERSION_MINOR, 
                   "mvl_h264dec", 
                   "h264 decoder based on IPP, "__DATE__,
                   plugin_init, 
                   VERSION, 
                   "LGPL",
                   "", 
                   "");


/* EOF */
