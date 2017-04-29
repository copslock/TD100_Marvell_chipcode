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

/* gstmpeg2dec.c */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h> //to include memcpy, memset()
#include <strings.h>	//to include strcasecmp()
#include "gstmpeg2dec.h"
#include "misc.h"

#ifdef DEBUG_PERFORMANCE
#include <sys/time.h>
#include <time.h>
#endif


GST_DEBUG_CATEGORY_STATIC (ippmpeg2dec_debug);
#define GST_CAT_DEFAULT ippmpeg2dec_debug

enum {

	/* fill above with user defined signals */
	LAST_SIGNAL
};

enum {
	ARG_0,
	/* fill below with user defined arguments */
	ARG_IGNORE_FRAMESBEFOREI,
	ARG_ENABLE_DYNAMICDEINTERLACE,

#ifdef DEBUG_PERFORMANCE
	ARG_TOTALFRAME,
	ARG_CODECTIME,
#endif
};

#define IPPGST_FORCEBLOCK_MPEG1

#ifndef DEBUG_DOWN_IS_YV12	//DEBUG_DOWN_IS_YV12 is only defined when debuging, it change I420 to YV12
#define MPEG2DEC_4CC_I420STR	"I420"
#else
#define MPEG2DEC_4CC_I420STR	"YV12"
#endif


#ifndef IPPGST_FORCEBLOCK_MPEG1
#define IPPGST_IPPMPEG2_SINKCAPTEMPLATE	"video/mpeg, mpegversion = (int)2, systemstream = (boolean) FALSE"
#else
#define IPPGST_IPPMPEG2_SINKCAPTEMPLATE	"video/mpeg, mpegversion = (int){1,2}, systemstream = (boolean) FALSE"
#endif

static GstStaticPadTemplate sink_factory =
	GST_STATIC_PAD_TEMPLATE ("sink", GST_PAD_SINK, GST_PAD_ALWAYS,
	GST_STATIC_CAPS (IPPGST_IPPMPEG2_SINKCAPTEMPLATE));

static GstStaticPadTemplate src_factory =
	GST_STATIC_PAD_TEMPLATE ("src", GST_PAD_SRC, GST_PAD_ALWAYS,
	GST_STATIC_CAPS ("video/x-raw-yuv,"
					 "format = (fourcc)" MPEG2DEC_4CC_I420STR ", "
					 "width = (int) [ 16, 2048 ], "
					 "height = (int) [ 16, 2048 ], "
					 "framerate = (fraction) [ 0, MAX ]"
					));



GST_BOILERPLATE (Gstippmpeg2dec, gst_ippmpeg2dec, GstElement, GST_TYPE_ELEMENT);


static const gint fpss[][2] = {
{0, 1},
{24000, 1001},
{24, 1},
{25, 1},
{30000, 1001},
{30, 1},
{50, 1},
{60000, 1001},
{60, 1}
};

#define MPEG2DEC_GST_ERR_FATALERR		1
#define MPEG2DEC_GST_ERR_LACKINITMEM	2
#define MPEG2DEC_GST_ERR_COMMON			4
#define MPEG2DEC_GST_SERIOUS_ERRS		(MPEG2DEC_GST_ERR_FATALERR|MPEG2DEC_GST_ERR_LACKINITMEM)

#define ELEMENT_IS_UNDECIDED			0
#define ELEMENT_IS_DEMUX_FFMPEGPS		1
#define ELEMENT_IS_DEMUX_FFMPEGTS		2
#define ELEMENT_IS_COMMON		0xffffffff		//COMMON means the species of element is unknown. The element still probably is one of above, like ELEMENT_IS_DEMUX_FLUMPEGPS.


#if 0
#define assist_myecho(...)	printf(__VA_ARGS__)
#else
#define assist_myecho(...)
#endif

//frame coded image could be progress or interlace image
//usage: if(MPEG2IMAGE_IS_INTERLACE(pIppPicture->picStatus)) { //interlace image, ....}
#define MPEG2IMAGE_IS_INTERLACE(PicStatus)			(((PicStatus)&0x80) == 0)
#define MPEG2IMAGE_IS_FRAMECODED(PicStatus)			(((PicStatus)&0x18) == 0x18)

void _Deinterlace2(unsigned char *dst, int dst_wrap, const unsigned char *src, int src_wrap, int width, int height);	//this function is implemented in ARM WMMX assembly and in .s file

static void _Deinterlace2_Cversion(unsigned char *dst, int dst_wrap, const unsigned char *src, int src_wrap, int width, int height);	//C version
//C version of _Deinterlace2_Cversion has no alignment requirement, and _Deinterlace2 ask each src line start address and dst line start address must be 8 aligned

#define IS_8ALIGNED(x)	( (((unsigned int)(x)) & 7) == 0 )
static void
copy_framedata_to_gstbuffer_sw_interlace(IppPicture *pSrcFrame, unsigned char * pDst, int iDstYpitch, int iDstUVpitch, int iDstUoffset, int iDstVoffset)
{
	Ipp8u* pSrc = pSrcFrame->ppPicPlane[0];
	int SrcStep = pSrcFrame->picPlaneStep[0];
	Ipp8u* pTarget = pDst;

	//copy and deinterlace Y planner
#ifdef FORBID_WMMX_DEINTERLACE
	_Deinterlace2_Cversion(pTarget, iDstYpitch, pSrc, SrcStep, pSrcFrame->picWidth, pSrcFrame->picHeight);
#else
	//check align, Src is always 8 aligned because the decoder guarantee it.
	if(G_LIKELY(IS_8ALIGNED(pTarget) && IS_8ALIGNED(iDstYpitch))) {
		_Deinterlace2(pTarget, iDstYpitch, pSrc, SrcStep, pSrcFrame->picWidth, pSrcFrame->picHeight);
	}else{
		_Deinterlace2_Cversion(pTarget, iDstYpitch, pSrc, SrcStep, pSrcFrame->picWidth, pSrcFrame->picHeight);
	}
#endif

	//copy and deinterlace U planner
	pSrc = pSrcFrame->ppPicPlane[1];
	SrcStep = pSrcFrame->picPlaneStep[1];
	pTarget = pDst + iDstUoffset;
#ifdef FORBID_WMMX_DEINTERLACE
	_Deinterlace2_Cversion(pTarget, iDstUVpitch, pSrc, SrcStep, pSrcFrame->picWidth>>1, pSrcFrame->picHeight>>1);
#else
	if(G_LIKELY(IS_8ALIGNED(pTarget) && IS_8ALIGNED(iDstUVpitch))) {
		_Deinterlace2(pTarget, iDstUVpitch, pSrc, SrcStep, pSrcFrame->picWidth>>1, pSrcFrame->picHeight>>1);
	}else{
		_Deinterlace2_Cversion(pTarget, iDstUVpitch, pSrc, SrcStep, pSrcFrame->picWidth>>1, pSrcFrame->picHeight>>1);
	}
#endif

	//copy and deinterlace U planner
	pSrc = pSrcFrame->ppPicPlane[2];
	SrcStep = pSrcFrame->picPlaneStep[2];
	pTarget = pDst + iDstVoffset;
#ifdef FORBID_WMMX_DEINTERLACE
	_Deinterlace2_Cversion(pTarget, iDstUVpitch, pSrc, SrcStep, pSrcFrame->picWidth>>1, pSrcFrame->picHeight>>1);
#else
	if(G_LIKELY(IS_8ALIGNED(pTarget) && IS_8ALIGNED(iDstUVpitch))) {
		_Deinterlace2(pTarget, iDstUVpitch, pSrc, SrcStep, pSrcFrame->picWidth>>1, pSrcFrame->picHeight>>1);
	}else{
		_Deinterlace2_Cversion(pTarget, iDstUVpitch, pSrc, SrcStep, pSrcFrame->picWidth>>1, pSrcFrame->picHeight>>1);
	}
#endif

	return;
}

/* filter parameters: [-1 4 2 4 -1] // 8 */
static void
deinterlace_line(unsigned char *dst, const unsigned char *lum_m4, const unsigned char *lum_m3, const unsigned char *lum_m2, const unsigned char *lum_m1, const unsigned char *lum, int size)
{
    int sum;

    for(;size > 0;size--) {
        sum = -lum_m4[0];
        sum += lum_m3[0] << 2;
        sum += lum_m2[0] << 1;
        sum += lum_m1[0] << 2;
        sum += -lum[0];
#if 0
 		if(((sum + 4) >> 3) >= 256) {
			dst[0] = 255;
		}else if(((sum + 4) >> 3) < 0) {
			dst[0] = 0;
		}else{
			dst[0] = ((sum + 4) >> 3);
		}
#else
		if(sum >= (256<<3)-4) {
			dst[0] = 255;
		}else if(sum < -4) {
			dst[0] = 0;
		}else{
			dst[0] = ((sum + 4) >> 3);
		}
#endif
        lum_m4++;
        lum_m3++;
        lum_m2++;
        lum_m1++;
        lum++;
        dst++;
    }
	return;
}

static void
_Deinterlace2_Cversion(unsigned char *dst, int dst_wrap, const unsigned char *src, int src_wrap, int width, int height)
{
    const unsigned char *src_m2, *src_m1, *src_0, *src_p1, *src_p2;
    int y;
	assist_myecho("Cversion deinterlace is called\n");

    src_m2 = src;
    src_m1 = &src_m2[src_wrap];
    src_0  =&src_m1[src_wrap];
    src_p1 =&src_0[src_wrap];
    src_p2 =&src_p1[src_wrap];

    /* copy the first two lines */
    memcpy(dst,src_m2,width);
    dst += dst_wrap;
    memcpy(dst,src_m1,width);
    dst += dst_wrap;

    src_m2 = src_m1;
    src_m1 = src_0;
    src_0  = src_p1;
    src_p1 = src_p2;
    src_p2 += src_wrap;

    for(y=0;y<(height-4);y+=2) {
        memcpy(dst,src_m1,width);
        dst += dst_wrap;
        deinterlace_line(dst,src_m2,src_m1,src_0,src_p1,src_p2,width);

        src_m2 = src_0;
        src_m1 = src_p1;
        src_0 = src_p2;
        src_p1 += 2*src_wrap;
        src_p2 += 2*src_wrap;

        dst += dst_wrap;
    }

    /* copy the last two lines */
    memcpy(dst,src_m1,width);
    dst += dst_wrap;
    memcpy(dst,src_0,width);

	return;
}

static void
copy_framedata_to_gstbuffer(IppPicture *pSrcFrame, unsigned char * pDst, int iDstYpitch, int iDstUVpitch, int iDstUoffset, int iDstVoffset)
{
	int i;
	Ipp8u* pSrc = pSrcFrame->ppPicPlane[0];
	int SrcStep = pSrcFrame->picPlaneStep[0];
	Ipp8u* pTarget = pDst;

	//copy Y planner
	if(G_LIKELY(SrcStep == iDstYpitch)) {
		memcpy(pTarget, pSrc, SrcStep * pSrcFrame->picHeight);
	}else{
		for(i=0;i<pSrcFrame->picHeight;i++) {
			memcpy(pTarget, pSrc, pSrcFrame->picWidth);
			pTarget += iDstYpitch;
			pSrc += SrcStep;
		}
	}

	//copy U planner
	pSrc = pSrcFrame->ppPicPlane[1];
	SrcStep = pSrcFrame->picPlaneStep[1];
	pTarget = pDst + iDstUoffset;
	if(G_LIKELY(SrcStep == iDstUVpitch)) {
		memcpy(pTarget, pSrc, SrcStep * (pSrcFrame->picHeight>>1));
	}else{
		for(i=0;i<(pSrcFrame->picHeight>>1);i++) {
			memcpy(pTarget, pSrc, pSrcFrame->picWidth>>1);
			pTarget += iDstUVpitch;
			pSrc += SrcStep;
		}
	}

	//copy V planner
	pSrc = pSrcFrame->ppPicPlane[2];
	SrcStep = pSrcFrame->picPlaneStep[2];
	pTarget = pDst + iDstVoffset;
	if(G_LIKELY(SrcStep == iDstUVpitch)) {
		memcpy(pTarget, pSrc, SrcStep * (pSrcFrame->picHeight>>1));
	}else{
		for(i=0;i<(pSrcFrame->picHeight>>1);i++) {
			memcpy(pTarget, pSrc, pSrcFrame->picWidth>>1);
			pTarget += iDstUVpitch;
			pSrc += SrcStep;
		}
	}

	return;
}


#define FFMPEGPSDEMUX_TYPENAME		"ffdemux_mpeg"
#define FFMPEGTSDEMUX_TYPENAME		"ffdemux_mpegts"

#define DEMUXER_IS_FFMPEG_PSTS(mpeg2)	(query_nonqueue_upstreamelement_species(mpeg2)==ELEMENT_IS_DEMUX_FFMPEGPS || query_nonqueue_upstreamelement_species(mpeg2)==ELEMENT_IS_DEMUX_FFMPEGTS)

static int
query_nonqueue_upstreamelement_species(Gstippmpeg2dec* mpeg2)
{

	if(mpeg2->UpAdjacentNonQueueEle != ELEMENT_IS_UNDECIDED) {
		return mpeg2->UpAdjacentNonQueueEle;
	}else{
		char* padname = "sink";
		GstElement* ele = GST_ELEMENT_CAST(mpeg2);
		GstPad* pad;
		GstPad* peerPad;
		GstObject* peerEleObj;
		GstElement* peerEle;
		const gchar* peerEleTname = NULL;
		int eleIsQueue = 0;
		GstIterator *iter = NULL;


		do{
			if(eleIsQueue != 2) {
				pad = gst_element_get_static_pad(ele, padname);
				peerPad = gst_pad_get_peer(pad);
				gst_object_unref(pad);
			}else{
				iter = gst_element_iterate_sink_pads(ele);
				if(GST_ITERATOR_OK != gst_iterator_next(iter, &pad)) {
					gst_iterator_free(iter);
					mpeg2->UpAdjacentNonQueueEle = ELEMENT_IS_COMMON;
					return ELEMENT_IS_COMMON;
				}
				peerPad = gst_pad_get_peer(pad);
				gst_object_unref(pad);
				gst_iterator_free(iter);
			}
			if(peerPad == NULL) {
				mpeg2->UpAdjacentNonQueueEle = ELEMENT_IS_COMMON;
				return ELEMENT_IS_COMMON;
			}
			peerEleObj = GST_OBJECT_PARENT(peerPad);
			if(! GST_IS_ELEMENT(peerEleObj)) {	//sometimes, the peerPad's parent isn't an element, it may be ghostpad or something else.
				gst_object_unref(peerPad);
				mpeg2->UpAdjacentNonQueueEle = ELEMENT_IS_COMMON;
				return ELEMENT_IS_COMMON;
			}
			peerEle = GST_ELEMENT(peerEleObj);
			gst_object_unref(peerPad);
			peerEleTname = G_OBJECT_TYPE_NAME(peerEle);
			ele = peerEle;
			if(strcmp(peerEleTname, "GstQueue") == 0) {
				eleIsQueue = 1;
			}else if(strcmp(peerEleTname, "GstMultiQueue") == 0) {
				eleIsQueue = 2;
			}else{
				eleIsQueue = 0;
			}
			assist_myecho("meet %s\n", peerEleTname == NULL ? "NULL" : peerEleTname);
		}while(eleIsQueue != 0);

		if(strcmp(peerEleTname, FFMPEGPSDEMUX_TYPENAME)==0) {
			mpeg2->UpAdjacentNonQueueEle = ELEMENT_IS_DEMUX_FFMPEGPS;
		}else if(strcmp(peerEleTname, FFMPEGTSDEMUX_TYPENAME)==0) {
			mpeg2->UpAdjacentNonQueueEle = ELEMENT_IS_DEMUX_FFMPEGTS;
		}else{
			mpeg2->UpAdjacentNonQueueEle = ELEMENT_IS_COMMON;
		}
		assist_myecho("Adjacent element at upstream is %s(id %d).\n", peerEleTname, mpeg2->UpAdjacentNonQueueEle);
		return mpeg2->UpAdjacentNonQueueEle;
	}

}

static void
gst_ippmpeg2dec_set_property(GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
	Gstippmpeg2dec* mpeg2 = GST_IPPMPEG2DEC (object);
	switch (prop_id)
	{
	case ARG_IGNORE_FRAMESBEFOREI:
		{
			int notdisp = g_value_get_int(value);
			if(notdisp != 0 && notdisp != 1) {
				GST_ERROR_OBJECT(mpeg2, "notdisp-BeforeI %d exceed range", notdisp);
			}else{
				mpeg2->bNotDisplayFramesBeforeI = notdisp;
			}
		}
		break;
	case ARG_ENABLE_DYNAMICDEINTERLACE:
		{
			int enable = g_value_get_int(value);
			if(enable != 0 && enable != 1) {
				GST_ERROR_OBJECT(mpeg2, "enable-DynamicDeinterlace %d exceed range", enable);
			}else{
				mpeg2->bEnableDynamicDeinterlace = enable;
			}
		}
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}

	return;
}

static void
gst_ippmpeg2dec_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
	Gstippmpeg2dec *mpeg2 = GST_IPPMPEG2DEC (object);

	switch (prop_id)
	{
	case ARG_IGNORE_FRAMESBEFOREI:
		g_value_set_int(value, mpeg2->bNotDisplayFramesBeforeI);
		break;
	case ARG_ENABLE_DYNAMICDEINTERLACE:
		g_value_set_int(value, mpeg2->bEnableDynamicDeinterlace);
		break;
#ifdef DEBUG_PERFORMANCE
	case ARG_TOTALFRAME:
		g_value_set_int(value, mpeg2->totalFrames);
		break;
	case ARG_CODECTIME:
		g_value_set_int64(value, mpeg2->codec_time);
		break;
#endif
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
	return;
}

static gboolean
gst_ippmpeg2dec_sinkpad_setcaps (GstPad *pad, GstCaps *caps)
{
	Gstippmpeg2dec* mpeg2 = GST_IPPMPEG2DEC (GST_OBJECT_PARENT (pad));

	int i = 0;
	gint mpegversion;
	const gchar *name;
	GstStructure *str;
	int stru_num = gst_caps_get_size (caps);
	if(stru_num != 1) {
		gchar* sstr;
		g_warning("Multiple MIME stream type in sinkpad caps, only select the first item.");
		for(i=0; i<stru_num; i++) {
			str = gst_caps_get_structure(caps, i);
			sstr = gst_structure_to_string(str);
			g_warning("struture %d is %s\n", i, sstr);
			g_free(sstr);
		}
	}


	str = gst_caps_get_structure(caps, 0);
	name = gst_structure_get_name(str);
	if(strcmp(name, "video/mpeg") != 0) {
		g_warning("Unsupported stream MIME type %s.", name);
		return FALSE;
	}

	if(G_LIKELY(TRUE == gst_structure_get_int(str, "mpegversion", &mpegversion))) {
		mpeg2->MpegVersion = mpegversion;
		GST_DEBUG_OBJECT(mpeg2, "mpeg video version is %d", mpegversion);
		if(mpegversion != 2) {
			g_warning("Unsupported mpeg video version %d.", mpegversion);
			return TRUE;	//will return error in _chain() if not mpeg2
		}
	}

	if(G_UNLIKELY(FALSE == gst_structure_get_fraction(str, "framerate", &mpeg2->iFRateNum, &mpeg2->iFRateDen))) {
		mpeg2->bGetFRinfoFromDemuxer = 0;
		mpeg2->iFRateNum = 0;	//if this information isn't in caps, we assume it is 0 at first, and in the chain function, it will be parsed from stream data
		mpeg2->iFRateDen = 1;
	}else{
#if 0
		if(! DEMUXER_IS_FFMPEG_PSTS(mpeg2)) {
			mpeg2->bGetFRinfoFromDemuxer = 1;
		}else{
			mpeg2->bGetFRinfoFromDemuxer = 0;	//not believe frame rate from ffmpeg PS/TS demuxer, it seems it provide field rate not frame rate for interlace stream
		}
#else
		mpeg2->bGetFRinfoFromDemuxer = 0;
#endif
		assist_myecho("Parse framerate %d %d from demuxer\n",mpeg2->iFRateNum, mpeg2->iFRateDen);
	}
	if(mpeg2->iFRateNum != 0) {
		mpeg2->FrameFixDuration = gst_util_uint64_scale_int(GST_SECOND, mpeg2->iFRateDen, mpeg2->iFRateNum);
	}else{
		mpeg2->FrameFixDuration = GST_CLOCK_TIME_NONE;
	}



	if(TRUE == gst_structure_has_field(str, "codec_data")) {
		const GValue *value = gst_structure_get_value(str, "codec_data");
		GstBuffer* buf = gst_value_get_buffer(value);
		if(mpeg2->LeftData != NULL) {
			gst_buffer_unref(mpeg2->LeftData);
			mpeg2->LeftData = NULL;
		}
		mpeg2->LeftData = gst_buffer_copy(buf);
	}
	return TRUE;
}

static inline int
update_mpeg2dec_srcpad_cap(Gstippmpeg2dec* mpeg2)
{
	assist_myecho("cap updated, w %d, h %d, framerate %d %d\n", mpeg2->iDownFrameWidth, mpeg2->iDownFrameHeight, mpeg2->iFRateNum, mpeg2->iFRateDen);
	GstCaps *Tmp = gst_caps_new_simple ("video/x-raw-yuv",
		"format", GST_TYPE_FOURCC, GST_STR_FOURCC (MPEG2DEC_4CC_I420STR),
		"width", G_TYPE_INT, mpeg2->iDownFrameWidth,
		"height", G_TYPE_INT, mpeg2->iDownFrameHeight,
		"framerate", GST_TYPE_FRACTION,  mpeg2->iFRateNum, mpeg2->iFRateDen,
		NULL);

	if(TRUE != gst_pad_set_caps(mpeg2->srcpad, Tmp)) {
		gchar* cap_string = gst_caps_to_string(Tmp);
		g_warning("set mpeg2 src cap fail, cap is %s",cap_string);
		g_free(cap_string);
	}
	gst_caps_unref(Tmp);

	return 0;
}

#define I420_standardLayout_sameas_compactLayout(w, h)	(((w)&7)==0 && ((h)&1)==0)	//if width is 8 align and height is 2 align, the GST standard I420 layout is the same as compact I420 layout

static void
update_downbuf_layout(int w, int h, int category, Gstippmpeg2dec* RawDecObj)
{
	if(category == 0) {
		//we defined compact I420 layout
		RawDecObj->iDownbufYpitch = w;
		RawDecObj->iDownbufUVpitch = w>>1;
		RawDecObj->iDownbufUoffset = RawDecObj->iDownbufYpitch * h;
		RawDecObj->iDownbufVoffset = RawDecObj->iDownbufUoffset + RawDecObj->iDownbufUVpitch * (h>>1);
		RawDecObj->iDownbufSz = RawDecObj->iDownbufVoffset + RawDecObj->iDownbufUVpitch * (h>>1);

	}else if(category == 1) {
		//gstreamer defined standard I420 layout, see <gst/video/video.h>, gstvideo and gst_video_format_get_size()
		RawDecObj->iDownbufYpitch = GST_ROUND_UP_4(w);
		RawDecObj->iDownbufUVpitch = GST_ROUND_UP_4(GST_ROUND_UP_2(w)>>1);
		RawDecObj->iDownbufUoffset = RawDecObj->iDownbufYpitch * GST_ROUND_UP_2(h);
		RawDecObj->iDownbufVoffset = RawDecObj->iDownbufUoffset + RawDecObj->iDownbufUVpitch * (GST_ROUND_UP_2(h)>>1);
		RawDecObj->iDownbufSz = RawDecObj->iDownbufVoffset + RawDecObj->iDownbufUVpitch * (GST_ROUND_UP_2(h)>>1);
	}
	return;
}

static inline int
calculate_GSTstandard_I420_size(int w, int h)
{
	//gstreamer defined standard I420 layout, see <gst/video/video.h>, gstvideo and gst_video_format_get_size()
	return (GST_ROUND_UP_4(w) + GST_ROUND_UP_4(GST_ROUND_UP_2(w)>>1)) * GST_ROUND_UP_2(h);
}

#define MPEG2IMAGE_IS_I(PicStatus)			(((PicStatus)&7) == 1 || ((PicStatus)&(7<<8)) == (1<<8))
#define MPEG2IMAGE_IS_B(PicStatus)			(((PicStatus)&7) == 3)
#define MPEG2IMAGE_FRAMERATECODE(PicStatus)	((PicStatus>>18)&0xf)
static GstFlowReturn
mpeg2_push_data(Gstippmpeg2dec *mpeg2, IppPicture* pFrame)
{
    GstFlowReturn ret = GST_FLOW_OK;
	GstBuffer *down_buf = NULL;
	int iDstUplanarOffset, iDstVplanarOffset;
	int fr_num, fr_den;

	//update TS
	//TsLattermost indicates the lattermost TS which are received
	//below TS method isn't accurate, but could work
	GstClockTime TSpushdown;
	if(mpeg2->InputTs <= mpeg2->LattermostTs) {
		TSpushdown = mpeg2->InputTs;
	}else{
		TSpushdown = mpeg2->LattermostTs;
		mpeg2->LattermostTs = mpeg2->InputTs;
	}

	if(mpeg2->bMetIFrameIn1Seg == 0) {
		if(MPEG2IMAGE_IS_I(pFrame->picStatus)) {
			mpeg2->bMetIFrameIn1Seg = 1;
			mpeg2->bBBeforeI_CouldBeHealth = 0;
		}else{
			if(mpeg2->bNotDisplayFramesBeforeI) {
				if( mpeg2->bBBeforeI_CouldBeHealth && MPEG2IMAGE_IS_B(pFrame->picStatus) ) {
					//in closed_gop, B before I(in display order) still could be health and displayable
				}else{
					//ignore P/B frames before I frame in one new segment
					return GST_FLOW_CUSTOM_SUCCESS;
				}
			}
		}
	}

	unsigned int frame_rate_code = MPEG2IMAGE_FRAMERATECODE(pFrame->picStatus);
	if(G_LIKELY(mpeg2->bGetFRinfoFromDemuxer == 0 && frame_rate_code < sizeof(fpss)/sizeof(fpss[0]))) {
		fr_num = fpss[frame_rate_code][0];
		fr_den = fpss[frame_rate_code][1];
	}else{
		fr_num = mpeg2->iFRateNum;
		fr_den = mpeg2->iFRateDen;
	}

	if(mpeg2->iDownFrameWidth != pFrame->picWidth || mpeg2->iDownFrameHeight != pFrame->picHeight || fr_num != mpeg2->iFRateNum || fr_den != mpeg2->iFRateDen) {
		if( (pFrame->picWidth & 1) || (pFrame->picHeight & 1) ) {
			g_warning("The display content area of decoded frame's dimension isn't even, width is %d, height %d",pFrame->picWidth, pFrame->picHeight);
			return GST_FLOW_ERROR;
		}

		mpeg2->iDownFrameWidth = pFrame->picWidth;
		mpeg2->iDownFrameHeight = pFrame->picHeight;
		mpeg2->iFRateNum = fr_num;
		mpeg2->iFRateDen = fr_den;
		if(mpeg2->iFRateNum != 0) {
			mpeg2->FrameFixDuration = gst_util_uint64_scale_int(GST_SECOND, mpeg2->iFRateDen, mpeg2->iFRateNum);
		}else{
			mpeg2->FrameFixDuration = GST_CLOCK_TIME_NONE;
		}

		update_mpeg2dec_srcpad_cap(mpeg2);

		//at first, we try our defined compact I420 layout, not standard gstreamer defined I420 layout in <gst/video/video.h>
		update_downbuf_layout(mpeg2->iDownFrameWidth, mpeg2->iDownFrameHeight, 0, mpeg2);
	}

	ret = gst_pad_alloc_buffer_and_set_caps(mpeg2->srcpad, 0, mpeg2->iDownbufSz, GST_PAD_CAPS(mpeg2->srcpad), &down_buf);
	if(ret != GST_FLOW_OK) {
		if(ret != GST_FLOW_WRONG_STATE) {
			//during seeking, sink element often return GST_FLOW_WRONG_STATUS. No need to echo this message
			g_warning("mpeg2 can not malloc buffer from down element, ret %d", ret);
		}
		return ret;
	}else{
		//Gstreamer defined I420/YV12 format require pitch should be 4 aligned(see <gst/video/video.h>, gstvideo and gst_video_format_get_size()). xvimagesink and ffdec always obey this rule.
		//However, filesink and fakesink don't require this rule because they don't care the format/layout of buffer.
		//Therefore, we output different buffer layout for different sink.
		if(!I420_standardLayout_sameas_compactLayout(mpeg2->iDownFrameWidth, mpeg2->iDownFrameHeight) && GST_BUFFER_SIZE(down_buf) != (guint)mpeg2->iDownbufSz) {
			if((int)GST_BUFFER_SIZE(down_buf) == calculate_GSTstandard_I420_size(mpeg2->iDownFrameWidth, mpeg2->iDownFrameHeight)) {
				update_downbuf_layout(mpeg2->iDownFrameWidth, mpeg2->iDownFrameHeight, 1, mpeg2);
				assist_myecho("Updated layout w %d, h %d, sz %d, y %d, uv %d, off %d, %d\n", mpeg2->iDownFrameWidth, mpeg2->iDownFrameHeight, mpeg2->iDownbufSz, mpeg2->iDownbufYpitch, mpeg2->iDownbufUVpitch, mpeg2->iDownbufUoffset, mpeg2->iDownbufVoffset);
			}
		}
	}


#ifndef DEBUG_DOWN_IS_YV12
	iDstUplanarOffset = mpeg2->iDownbufUoffset;
	iDstVplanarOffset = mpeg2->iDownbufVoffset;
#else
	iDstUplanarOffset = mpeg2->iDownbufVoffset;
	iDstVplanarOffset = mpeg2->iDownbufUoffset;
#endif

	if(mpeg2->bEnableDynamicDeinterlace != 0 && MPEG2IMAGE_IS_INTERLACE(pFrame->picStatus)){
		assist_myecho("Interlaced Frame, and Deinteralcing\n");
		copy_framedata_to_gstbuffer_sw_interlace(pFrame, GST_BUFFER_DATA(down_buf), mpeg2->iDownbufYpitch, mpeg2->iDownbufUVpitch, iDstUplanarOffset, iDstVplanarOffset);
	}else{
		copy_framedata_to_gstbuffer(pFrame, GST_BUFFER_DATA(down_buf), mpeg2->iDownbufYpitch, mpeg2->iDownbufUVpitch, iDstUplanarOffset, iDstVplanarOffset);
	}


	GST_BUFFER_DURATION(down_buf) = mpeg2->FrameFixDuration;
	GST_BUFFER_TIMESTAMP(down_buf) = TSpushdown;

	if(mpeg2->bNewSegReceivedAfterflush) {
		ret = gst_pad_push(mpeg2->srcpad, down_buf);
	}else{
		//If new segment event hasn't received, shouldn't push buffer to sink. Otherwise, sink will throw warnings and work abnormal.
		//Sometimes, user seeking position exceeds the duration of file, then demuxer only fire flush event without newseg event. And demuxer also fire EOS event. At EOS, decoder probably still output some frames, but those frames shouldn't be pushed to sink because no newseg event.
		gst_buffer_unref(down_buf);
		ret = GST_FLOW_OK;
	}
	if(ret != GST_FLOW_OK) {
		if(ret != GST_FLOW_WRONG_STATE) {
			//during seeking, sink element often return GST_FLOW_WRONG_STATUS. No need to echo this message
			g_warning("Pad push buffer to down stream fail, ret %d!", ret);
		}
	}
	return ret;
}

#define MPEG2DECODING_UNTIL_EOS			0
#define MPEG2DECODING_UNTIL_LACKDATA	1	//until all current data are not enough to continue decoding
#define MPEG2DECODING_UNTIL_FRAME		2	//until one frame decoded
#define MPEG2DECODING_UNTIL_PICTURE		3	//until one picture decoded
static int
decoding_stream(Gstippmpeg2dec* mpeg2, IppBitstream* pStmInput, IppPicture* pOutFrame, int iDecodingStopLevel)
{
#ifdef DEBUG_PERFORMANCE
	struct timeval clk0, clk1;
#endif

	IppCodecStatus ret;
	int bEndOfSegment = (iDecodingStopLevel == MPEG2DECODING_UNTIL_EOS ? 1 : 0);

	while(pStmInput->bsByteLen-(pStmInput->pBsCurByte-pStmInput->pBsBuffer) > 0) {
#ifdef DEBUG_PERFORMANCE
		gettimeofday(&clk0, NULL);
#endif
		ret = Decode_MPEG2Video(pStmInput, pOutFrame, bEndOfSegment, mpeg2->pMPEG2DecObj);
#ifdef DEBUG_PERFORMANCE
		gettimeofday(&clk1, NULL);
		mpeg2->codec_time += (clk1.tv_sec - clk0.tv_sec)*1000000 + (clk1.tv_usec - clk0.tv_usec);
#endif

		if( ret == IPP_STATUS_NOERR || ret == IPP_STATUS_BS_END ) {
			if(mpeg2->bFirstDummyFrame == 0) {
				if(bEndOfSegment == 0 || mpeg2->iLastFrameTemporalReference != pOutFrame->picOrderCnt) {
					//during the end of stream, it should check whether the frame has been outputed repeatedly
#ifdef DEBUG_PERFORMANCE
					mpeg2->totalFrames++;
#endif
					mpeg2_push_data(mpeg2, pOutFrame);
				}
				mpeg2->iLastFrameTemporalReference = pOutFrame->picOrderCnt;
			}else{
				mpeg2->bFirstDummyFrame = 0;
			}
			if(ret == IPP_STATUS_BS_END) {
				//the sequence_end_code detected by decoder probably caused by error bit, continue to decode
				assist_myecho("the sequence end code is detected, because still input exits, continue to decode\n");
				//reset the decoder state
				DecodeSendCmd_MPEG2Video(IPPVC_SET_DECODENEWPIC, 0, 0, mpeg2->pMPEG2DecObj);
			}
			if(iDecodingStopLevel >= MPEG2DECODING_UNTIL_FRAME) {
				break;
			}else{
				continue;
			}
		}else if(ret == IPP_STATUS_FIELD_PICTURE_TOP || ret == IPP_STATUS_FIELD_PICTURE_BOTTOM) {
			if(iDecodingStopLevel >= MPEG2DECODING_UNTIL_PICTURE) {
				break;
			}else{
				continue;
			}
		}else if(ret == IPP_STATUS_NEED_INPUT) {
			break;	//need more data
		}else if(ret == IPP_STATUS_FATAL_ERR) {
			mpeg2->iError_in1stream |= MPEG2DEC_GST_ERR_FATALERR;
			g_warning("Fatal error happen during mpeg2 decoding\n");
			return -1;
		}else{
			assist_myecho("Decoding Common Error Occur %d.\n", ret);
			mpeg2->iError_in1stream |= MPEG2DEC_GST_ERR_COMMON;
			//reset the decoder state
			DecodeSendCmd_MPEG2Video(IPPVC_SET_DECODENEWPIC, 0, 0, mpeg2->pMPEG2DecObj);
			break;
		}
	}
	return 0;
}

static unsigned char*
reverseseek_SC(unsigned char* pEnd, int len, unsigned int SC)
{
	if(len < 4) {
		return NULL;
	}else{
		unsigned int code = (*(pEnd-1)<<16) | (*(pEnd-2)<<8) | *(pEnd-3);
		pEnd -= 4;
		SC = (SC>>24) | ((SC&0xff0000)>>8) | ((SC&0xff00)<<8) | (SC<<24);
		while(len>0){
			code = *pEnd-- | (code<<8);
			len--;
			if(code == SC) {
				return pEnd+1;
			}
		}
		return NULL;
	}
}

static GstFlowReturn
gst_ippmpeg2dec_chain(GstPad *pad, GstBuffer *buf)
{
	Gstippmpeg2dec *mpeg2 = GST_IPPMPEG2DEC (GST_OBJECT_PARENT (pad));
	IppBitstream  StmInputVideo;
	IppPicture    DstPicture;
	int	iUsedLen;
	if(mpeg2->MpegVersion != 2) {
		gst_buffer_unref(buf);
		GST_ELEMENT_ERROR(mpeg2, STREAM, DECODE, (NULL), ("%s() return flow-error because IPP not support mpeg%d video", __FUNCTION__, mpeg2->MpegVersion));
		return GST_FLOW_ERROR;
	}

	if(mpeg2->iError_in1stream & MPEG2DEC_GST_SERIOUS_ERRS) {
		goto MPEG2DEC_GSTCHAIN_FAIL;
	}

	//TS
	if(GST_BUFFER_TIMESTAMP(buf) != GST_CLOCK_TIME_NONE) {
		mpeg2->InputTs = GST_BUFFER_TIMESTAMP(buf);
		if(mpeg2->LattermostTs == GST_CLOCK_TIME_NONE) {
			mpeg2->LattermostTs = GST_BUFFER_TIMESTAMP(buf);
		}
	}

	//join buf
	if(mpeg2->LeftData != NULL) {
		buf = gst_buffer_join(mpeg2->LeftData, buf);
		mpeg2->LeftData = NULL;
	}


	StmInputVideo.pBsBuffer = GST_BUFFER_DATA(buf);
	StmInputVideo.bsByteLen = GST_BUFFER_SIZE(buf);
	StmInputVideo.pBsCurByte = StmInputVideo.pBsBuffer;
	StmInputVideo.bsCurBitOffset = mpeg2->bsCurBitOffset;

	//init decoder
	if(mpeg2->pMPEG2DecObj == NULL && 0 == (mpeg2->iError_in1stream & MPEG2DEC_GST_SERIOUS_ERRS)) {
		mpeg2->bDecoderDisNSC = 0;
		if(DEMUXER_IS_FFMPEG_PSTS(mpeg2)) {
			mpeg2->bDecoderDisNSC = 1;
			assist_myecho("up stream demuxer is ffdemux_mpeg or ffdemux_mpegts\n");
		}

		//the first input data should be long enough to init decoder
		IppCodecStatus init_ret = DecoderInitAlloc_MPEG2Video(mpeg2->pCbTable,&mpeg2->pMPEG2DecObj,&StmInputVideo);
		if ( IPP_STATUS_NOERR != init_ret ) {
			assist_myecho("Calling DecoderInitAlloc_MPEG2Video() not succeed, ret %d!\n", init_ret);
			if( IPP_STATUS_BADARG_ERR != init_ret) {
				DecoderFree_MPEG2Video(&mpeg2->pMPEG2DecObj);		//if DecoderInitAlloc_MPEG2Video failed, always should call DecoderFree_MPEG2Video() except the return code is IPP_STATUS_BADARG_ERR. Calling DecoderFree_MPEG2Video() more one time has no side-effect.
			}
			mpeg2->pMPEG2DecObj = NULL;

			if( IPP_STATUS_NOMEM_ERR == init_ret ) {
				mpeg2->iError_in1stream |= MPEG2DEC_GST_ERR_LACKINITMEM;
				g_warning("Calling DecoderInitAlloc_MPEG2Video() memory isn't enough\n");
				goto MPEG2DEC_GSTCHAIN_FAIL;
			}else if(IPP_STATUS_FATAL_ERR == init_ret) {
				mpeg2->iError_in1stream |= MPEG2DEC_GST_ERR_FATALERR;
				g_warning("Calling DecoderInitAlloc_MPEG2Video() fatal error happend\n");
				goto MPEG2DEC_GSTCHAIN_FAIL;
			}else{
				//other errors are all caused by stream error, no special handling
			}
		}else{
			if(mpeg2->bNotDisplayFramesBeforeI) {
				//seek GOP header
				unsigned char* pGOPHeader = reverseseek_SC(StmInputVideo.pBsCurByte, StmInputVideo.pBsCurByte - StmInputVideo.pBsBuffer, 0x000001b8);
				if(pGOPHeader && pGOPHeader - StmInputVideo.pBsBuffer <= StmInputVideo.bsByteLen - 8 ) {
					unsigned char* pSeqHeader = reverseseek_SC(StmInputVideo.pBsCurByte, StmInputVideo.pBsCurByte - StmInputVideo.pBsBuffer, 0x000001b3);
					if(pSeqHeader && (unsigned int)pGOPHeader > (unsigned int)pSeqHeader) {
						//ensure the GOP is the latest GOP after sequence header
						int closed_gop = (pGOPHeader[7] >> 6) & 1;
						if(closed_gop) {
							mpeg2->bBBeforeI_CouldBeHealth = 1;
						}
					}
				}
			}
		}

		if(mpeg2->pMPEG2DecObj != NULL) {
			//if demuxer isn't ffmpeg ps demuxer, enable non-restricted input mode of ippmpeg2dec, the input data segment could be arbitrary
			DecodeSendCmd_MPEG2Video(IPPVC_SET_NSCCHECKDISABLE, &mpeg2->bDecoderDisNSC, 0, mpeg2->pMPEG2DecObj);
		}else{
			StmInputVideo.bsCurBitOffset = 0;
#define DATA_FOR_INITDECODER_MAXSIZE	(1024*1024*2)	//2M byte
			if(StmInputVideo.bsByteLen > DATA_FOR_INITDECODER_MAXSIZE) {
				//the accumulated data is too long, give up previous 1/2 data
				StmInputVideo.pBsCurByte = StmInputVideo.pBsBuffer + DATA_FOR_INITDECODER_MAXSIZE/2;
			}else{
				//accumulate data
				StmInputVideo.pBsCurByte = StmInputVideo.pBsBuffer;
			}
		}

		mpeg2->bsCurBitOffset = StmInputVideo.bsCurBitOffset;
	}

	if(mpeg2->pMPEG2DecObj != NULL && 0 == (mpeg2->iError_in1stream & MPEG2DEC_GST_SERIOUS_ERRS)) {
		int ret;
		if(DEMUXER_IS_FFMPEG_PSTS(mpeg2)) {
			//ffmpeg ps/ts demux provide whole picture each time, those bits left in the end of current compressed data are stuffing bits, should be ignored
			ret = decoding_stream(mpeg2, &StmInputVideo, &DstPicture, MPEG2DECODING_UNTIL_PICTURE);
			StmInputVideo.pBsCurByte = StmInputVideo.pBsBuffer + StmInputVideo.bsByteLen;
		}else{
			ret = decoding_stream(mpeg2, &StmInputVideo, &DstPicture, MPEG2DECODING_UNTIL_LACKDATA);
		}
		mpeg2->bsCurBitOffset = StmInputVideo.bsCurBitOffset;
		if(ret == -1) {
			goto MPEG2DEC_GSTCHAIN_FAIL;
		}
	}

	//store left data for next _chain()
	iUsedLen = StmInputVideo.pBsCurByte-StmInputVideo.pBsBuffer;
	if(StmInputVideo.bsByteLen-iUsedLen > 0) {
		mpeg2->LeftData = gst_buffer_create_sub(buf, iUsedLen, StmInputVideo.bsByteLen-iUsedLen);
	}


	gst_buffer_unref(buf);
	return GST_FLOW_OK;

MPEG2DEC_GSTCHAIN_FAIL:
	mpeg2->bsCurBitOffset = 0;
	gst_buffer_unref(buf);
	GST_ELEMENT_ERROR(mpeg2, STREAM, DECODE, (NULL), ("%s() return un-accepted error %d", __FUNCTION__, GST_FLOW_ERROR));

	return GST_FLOW_ERROR;

}

static gboolean
gst_ippmpeg2dec_null2ready(Gstippmpeg2dec *mpeg2)
{

	if( 0 != miscInitGeneralCallbackTable(&(mpeg2->pCbTable)) ) {
		mpeg2->iError_in1stream |= MPEG2DEC_GST_ERR_LACKINITMEM;
		g_warning("ippmpeg2dec init call back table error");
		GST_ERROR_OBJECT(mpeg2, "ippmpeg2dec init call back table error");
        return FALSE;
    }

	return TRUE;
}


static gboolean
gst_ippmpeg2dec_ready2null(Gstippmpeg2dec *mpeg2)
{
	if(mpeg2->LeftData != NULL) {
		gst_buffer_unref(mpeg2->LeftData);
		mpeg2->LeftData = NULL;
	}

	if(mpeg2->pMPEG2DecObj != NULL) {
		DecoderFree_MPEG2Video(&mpeg2->pMPEG2DecObj);
		mpeg2->pMPEG2DecObj = NULL;
	}

	if(mpeg2->pCbTable != NULL) {
		miscFreeGeneralCallbackTable(&mpeg2->pCbTable);
		mpeg2->pCbTable = NULL;
	}

	if(mpeg2->iError_in1stream != 0) {
		g_warning("Some error (id=%d) happen during decoding", mpeg2->iError_in1stream);
	}

#ifdef DEBUG_PERFORMANCE
	printf("codec system time %lld usec, frame number %d\n", mpeg2->codec_time, mpeg2->totalFrames);
#endif

	return TRUE;
}

static inline gboolean
gst_ippmpeg2dec_dec1stream_ready2pause(Gstippmpeg2dec* mpeg2)
{
	mpeg2->UpAdjacentNonQueueEle = ELEMENT_IS_UNDECIDED;

	//because it's possible to decode a new stream, stream correlative information should be reset
	mpeg2->iDownFrameWidth = 0;
	mpeg2->iDownFrameHeight = 0;
	mpeg2->iDownbufSz = 0;

	mpeg2->bGetFRinfoFromDemuxer = 0;
	mpeg2->iFRateNum = 0;
	mpeg2->iFRateDen = 1;
	mpeg2->InputTs = GST_CLOCK_TIME_NONE;
	mpeg2->LattermostTs = GST_CLOCK_TIME_NONE;
	mpeg2->FrameFixDuration = GST_CLOCK_TIME_NONE;

	mpeg2->bFirstDummyFrame = 1;
	mpeg2->iLastFrameTemporalReference = -1;

	mpeg2->iError_in1stream = 0;

	if(mpeg2->LeftData != NULL) {
		gst_buffer_unref(mpeg2->LeftData);
		mpeg2->LeftData = NULL;
	}
	mpeg2->bsCurBitOffset = 0;

	if(mpeg2->pMPEG2DecObj != NULL) {
		DecoderFree_MPEG2Video(&mpeg2->pMPEG2DecObj);
		mpeg2->pMPEG2DecObj = NULL;
	}

	return TRUE;
}

static inline gboolean
gst_ippmpeg2dec_dec1stream_pause2ready(Gstippmpeg2dec* mpeg2)
{
	//delay clean work to "Ready To Paused", because in early GST version, sometimes the finish of EOS for whole pipeline is later than decoder plugin's "Paused to Ready"
	return TRUE;
}

static GstStateChangeReturn
gst_ippmpeg2dec_change_state(GstElement *element, GstStateChange transition)
{
	GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;
	Gstippmpeg2dec *mpeg2 = GST_IPPMPEG2DEC(element);

    assist_myecho("GST ippmpeg2dec state change trans from %d to %d\n", transition>>3, transition&7);

	switch (transition)
	{
	case GST_STATE_CHANGE_NULL_TO_READY:
		assist_myecho("GST ippmpeg2dec state change: NULL to Ready\n");
		if(!gst_ippmpeg2dec_null2ready(mpeg2)){
			goto MPEG2GST_STATECHANGE_ERR;
		}
		break;
	case GST_STATE_CHANGE_READY_TO_PAUSED:
		gst_ippmpeg2dec_dec1stream_ready2pause(mpeg2);
		break;
	default:
		break;
	}
	ret = GST_ELEMENT_CLASS (parent_class)->change_state (element, transition);
	if (ret == GST_STATE_CHANGE_FAILURE) {
		g_warning("GST ippmpeg2dec parent class state change fail!");
		return ret;
	}

	switch (transition)
	{
	case GST_STATE_CHANGE_PAUSED_TO_READY:
		gst_ippmpeg2dec_dec1stream_pause2ready(mpeg2);
		break;
	case GST_STATE_CHANGE_READY_TO_NULL:
		assist_myecho("GST ippmpeg2dec state change: Ready to NULL\n");
		if(!gst_ippmpeg2dec_ready2null(mpeg2)){
			goto MPEG2GST_STATECHANGE_ERR;
		}
		break;
	default:
		break;
	}

	return ret;

	 /* ERRORS */
MPEG2GST_STATECHANGE_ERR:
	{
		g_warning("ippmpeg2dec state change failed");
		/* subclass must post a meaningfull error message */
		GST_ERROR_OBJECT (mpeg2, "state change failed");
		return GST_STATE_CHANGE_FAILURE;
	}
}

static gboolean
gst_ippmpeg2dec_sinkpad_event(GstPad *pad, GstEvent *event)
{
	Gstippmpeg2dec *mpeg2 = GST_IPPMPEG2DEC (GST_OBJECT_PARENT (pad));
	gboolean eventRet;

	assist_myecho("GST ippmpeg2dec get event from upstream element's pad\n");

	switch (GST_EVENT_TYPE (event))
	{
	case GST_EVENT_NEWSEGMENT:
	{
		assist_myecho("GST ippmpeg2dec !new segment! event received\n");
#if 1
		//simply give up the left data is also OK.
#else
		if(mpeg2->LeftData != NULL && mpeg2->pMPEG2DecObj != NULL && 0 == (mpeg2->iError_in1stream & MPEG2DEC_GST_SERIOUS_ERRS)) {
			IppBitstream  StmInputVideo;
			IppPicture    DstPicture;
			int ret;
			StmInputVideo.pBsBuffer = GST_BUFFER_DATA(mpeg2->LeftData);
			StmInputVideo.bsByteLen = GST_BUFFER_SIZE(mpeg2->LeftData);
			StmInputVideo.pBsCurByte = StmInputVideo.pBsBuffer;
			StmInputVideo.bsCurBitOffset = mpeg2->bsCurBitOffset;

			//let output gst buffer to sink has the timestamp--"GST_CLOCK_TIME_NONE", otherwise, mpegpsdemux/flupsdemux probably couldn't work normally after seeking
			mpeg2->InputTs = GST_CLOCK_TIME_NONE;
			mpeg2->LattermostTs = GST_CLOCK_TIME_NONE;

			ret = decoding_stream(mpeg2, &StmInputVideo, &DstPicture, MPEG2DECODING_UNTIL_EOS);
			mpeg2->bsCurBitOffset = StmInputVideo.bsCurBitOffset;
			if(-1 != ret) {
				DecodeSendCmd_MPEG2Video(IPPVC_SET_DECODENEWFRAME, 0, 0, mpeg2->pMPEG2DecObj);
			}
		}
#endif
		//reset LeftData
		if(mpeg2->LeftData != NULL) {
			gst_buffer_unref(mpeg2->LeftData);
			mpeg2->LeftData = NULL;
		}

		eventRet = gst_pad_push_event(mpeg2->srcpad, event);

		mpeg2->bNewSegReceivedAfterflush = 1;

		//prepare to decoding new segment
		//reset TS
		mpeg2->InputTs = GST_CLOCK_TIME_NONE;
		mpeg2->LattermostTs = GST_CLOCK_TIME_NONE;
		mpeg2->bsCurBitOffset = 0;
		DecodeSendCmd_MPEG2Video(IPPVC_SET_DECODENEWFRAME, 0, 0, mpeg2->pMPEG2DecObj);
		mpeg2->bMetIFrameIn1Seg = 0;
		break;
	}
	case GST_EVENT_EOS:
	{
		assist_myecho("GST ippmpeg2dec !EOS! event received.\n");

		if(mpeg2->pMPEG2DecObj != NULL && 0 == (mpeg2->iError_in1stream & MPEG2DEC_GST_SERIOUS_ERRS)) {
#ifdef DEBUG_PERFORMANCE
			struct timeval clk0, clk1;
#endif
			IppBitstream  StmInputVideo;
			IppPicture    DstPicture;

			if(mpeg2->LeftData != NULL ) {
				StmInputVideo.pBsBuffer = GST_BUFFER_DATA(mpeg2->LeftData);
				StmInputVideo.bsByteLen = GST_BUFFER_SIZE(mpeg2->LeftData);
				StmInputVideo.pBsCurByte = StmInputVideo.pBsBuffer;
				StmInputVideo.bsCurBitOffset = mpeg2->bsCurBitOffset;
				decoding_stream(mpeg2, &StmInputVideo, &DstPicture, MPEG2DECODING_UNTIL_EOS);
			}
			//To get the last delay frame
			StmInputVideo.pBsBuffer = StmInputVideo.pBsCurByte = NULL;
			StmInputVideo.bsByteLen = 0;
			StmInputVideo.bsCurBitOffset = 0;

#ifdef DEBUG_PERFORMANCE
			gettimeofday(&clk0, NULL);
#endif
			Decode_MPEG2Video(&StmInputVideo, &DstPicture, 1, mpeg2->pMPEG2DecObj);
#ifdef DEBUG_PERFORMANCE
			gettimeofday(&clk1, NULL);
			mpeg2->codec_time += (clk1.tv_sec - clk0.tv_sec)*1000000 + (clk1.tv_usec - clk0.tv_usec);
#endif

			if(mpeg2->bFirstDummyFrame == 0 && mpeg2->iLastFrameTemporalReference != DstPicture.picOrderCnt) {
				//during the end of stream, it should check whether the frame has been outputed repeatedly
#ifdef DEBUG_PERFORMANCE
				mpeg2->totalFrames++;
#endif
				mpeg2_push_data(mpeg2, &DstPicture);

				mpeg2->iLastFrameTemporalReference = DstPicture.picOrderCnt;
			}

		}
		if(mpeg2->LeftData != NULL) {
			gst_buffer_unref(mpeg2->LeftData);
			mpeg2->LeftData = NULL;
		}
		mpeg2->bsCurBitOffset = 0;

		eventRet = gst_pad_push_event(mpeg2->srcpad, event);
		assist_myecho("GST ippmpeg2dec !EOS! event push returned\n");
		break;
	}

	case GST_EVENT_FLUSH_STOP:
		mpeg2->bNewSegReceivedAfterflush = 0;
	default:
		eventRet = gst_pad_event_default (pad, event);
		break;
	}

	return eventRet;
}



static void
gst_ippmpeg2dec_base_init (gpointer klass)
{
	static GstElementDetails ippmpeg2dec_details = {
		"MPEG-2 Video Decoder(Marvell IPP codec)",
		"Codec/Decoder/Video",
		"MPEG-2 Video Decoder based on IPP codec",
		""
	};

	GstElementClass *element_class = GST_ELEMENT_CLASS (klass);

	gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&src_factory));
	gst_element_class_add_pad_template (element_class, gst_static_pad_template_get (&sink_factory));

	gst_element_class_set_details (element_class, &ippmpeg2dec_details);

	assist_myecho("ippmpeg2dec base inited\n");
}

static void
gst_ippmpeg2dec_class_init (Gstippmpeg2decClass *klass)
{
	GstElementClass *gstelement_class = (GstElementClass*) klass;

	GObjectClass *gobject_class  = (GObjectClass*) klass;

	//parent_class = g_type_class_peek_parent(klass);	//parent_class has been declared in GST_BOILERPLATE, and this opration has been done in GST_BOILERPLATE_FULL

	gobject_class->set_property = gst_ippmpeg2dec_set_property;
	gobject_class->get_property = gst_ippmpeg2dec_get_property;

	g_object_class_install_property (gobject_class, ARG_IGNORE_FRAMESBEFOREI,
		g_param_spec_int ("notdisp-BeforeI", "ignore frames before I frame",
		"Not display P/B frames before I frame in one new segment, could be 1<not display> or 0<display>", 0, 1, 1, G_PARAM_READWRITE));

	g_object_class_install_property (gobject_class, ARG_ENABLE_DYNAMICDEINTERLACE,
		g_param_spec_int ("enable-DynamicDeinterlace", "enable dynamic deinterlace",
		"Enable dynamic deinterlace, could be 1<do deinterlace if image is interlace> or 0<not do deinterlace>", 0, 1, 1, G_PARAM_READWRITE));

#ifdef DEBUG_PERFORMANCE
	g_object_class_install_property (gobject_class, ARG_TOTALFRAME,
		g_param_spec_int ("totalframes", "Number of frame",
		"Number of total decoded frames for all tracks", 0, G_MAXINT, 0, G_PARAM_READABLE));
	g_object_class_install_property (gobject_class, ARG_CODECTIME,
		g_param_spec_int64 ("codectime", "codec time (microsecond)",
		"Total pure decoding spend system time for all tracks (microsecond)", 0, G_MAXINT64, 0, G_PARAM_READABLE));
#endif

	gstelement_class->change_state = GST_DEBUG_FUNCPTR (gst_ippmpeg2dec_change_state);

	GST_DEBUG_CATEGORY_INIT(ippmpeg2dec_debug, "ippmpeg2dec", 0, "IPP MPEG2Video GST Decoder Element");	//after this, GST_DEBUG(), GST_DEBUG_OBJECT() and ... could work
	assist_myecho("ippmpeg2dec class inited\n");
}

static inline void
mpeg2dec_init_members(Gstippmpeg2dec *mpeg2)
{
	mpeg2->pCbTable = NULL;
	mpeg2->pMPEG2DecObj = NULL;
	mpeg2->bDecoderDisNSC = 0;

	mpeg2->iDownFrameWidth = 0;
	mpeg2->iDownFrameHeight = 0;
	mpeg2->iDownbufSz = 0;
	mpeg2->bGetFRinfoFromDemuxer = 0;
	mpeg2->iFRateNum = 0;
	mpeg2->iFRateDen = 1;
	mpeg2->InputTs = GST_CLOCK_TIME_NONE;
	mpeg2->LattermostTs = GST_CLOCK_TIME_NONE;
	mpeg2->FrameFixDuration = GST_CLOCK_TIME_NONE;

	mpeg2->bFirstDummyFrame = 1;
	mpeg2->iLastFrameTemporalReference = -1;

	mpeg2->UpAdjacentNonQueueEle = ELEMENT_IS_UNDECIDED;

	mpeg2->LeftData = NULL;
	mpeg2->bsCurBitOffset = 0;
	mpeg2->iError_in1stream = 0;

	mpeg2->bMetIFrameIn1Seg = 0;
	mpeg2->bBBeforeI_CouldBeHealth = 0;
	mpeg2->bNotDisplayFramesBeforeI = 1;
	mpeg2->bEnableDynamicDeinterlace = 0;

	mpeg2->bNewSegReceivedAfterflush = 0;
	mpeg2->MpegVersion = 0;
#ifdef DEBUG_PERFORMANCE
	mpeg2->totalFrames = 0;
	mpeg2->codec_time = 0;
#endif

	return;
}

static void
gst_ippmpeg2dec_init(Gstippmpeg2dec *mpeg2, Gstippmpeg2decClass *klass)
{
	mpeg2dec_init_members(mpeg2);

	mpeg2->sinkpad = gst_pad_new_from_template (
		gst_element_class_get_pad_template ((GstElementClass*)klass, "sink"), "sink");


	gst_pad_set_setcaps_function (mpeg2->sinkpad, GST_DEBUG_FUNCPTR (gst_ippmpeg2dec_sinkpad_setcaps));
	gst_pad_set_chain_function (mpeg2->sinkpad, GST_DEBUG_FUNCPTR (gst_ippmpeg2dec_chain));
	gst_pad_set_event_function (mpeg2->sinkpad, GST_DEBUG_FUNCPTR (gst_ippmpeg2dec_sinkpad_event));

	mpeg2->srcpad = gst_pad_new_from_template (
		gst_element_class_get_pad_template ((GstElementClass*)klass, "src"), "src");

	gst_element_add_pad (GST_ELEMENT(mpeg2), mpeg2->sinkpad);
	gst_element_add_pad (GST_ELEMENT(mpeg2), mpeg2->srcpad);

    assist_myecho("ippmpeg2dec instance inited\n");
    return;
}

static gboolean
plugin_init (GstPlugin *plugin)
{
	assist_myecho("ippmpeg2dec plugin inited\n");
	return gst_element_register (plugin, "ippmpeg2dec", GST_RANK_PRIMARY+2, GST_TYPE_IPPMPEG2DEC);
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
                   GST_VERSION_MINOR,
                   "mvl_mpeg2dec",
                   "MPEG2 video decoder based on IPP codec, "__DATE__,
                   plugin_init,
                   VERSION,
                   "LGPL",
                   "",
                   "");



/* EOF */
