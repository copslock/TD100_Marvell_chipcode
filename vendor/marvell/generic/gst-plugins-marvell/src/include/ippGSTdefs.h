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

#ifndef __IPP_GSTDEFS_H__
#define __IPP_GSTDEFS_H__

G_BEGIN_DECLS

/**
 * PERFORMANCE_TEST_ONLY:
 * @0: Normal way, processed the stream and push out the data
 * @1: Performance test only, processed the stream but do not push out the data
 *
 * Change the definition of this MACRO will make this element push out 
 * the processed data or free it for performance test purpose.
 */
#define PERFORMANCE_TEST_ONLY 0


#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif


/******************************************************************************
// ippVCelements plug-in enumerations
******************************************************************************/

typedef enum {
	GST_H263ENC_SUBQCIF,			/* 128 x 96 */
	GST_H263ENC_QCIF,				/* 176 x 144 */
	GST_H263ENC_CIF,				/* 352 x 288 */
	GST_H263ENC_4CIF,				/* 704 x 576 */
	GST_H263ENC_16CIF				/* 1408 x 1152 */
} GstH263EncFrameSize;


/******************************************************************************
// ippACelements plug-in enumerations
******************************************************************************/

#define GST_AUDIO_FRAMES_TO_TIME(frameSamples, sampleRate) \
	((GstClockTime)(((gdouble)frameSamples / sampleRate) * GST_SECOND))

#define GST_AUDIO_TIME_TO_FRAMES(timeStamp, sampleRate) \
	((gint64)((gst_guint64_to_gdouble(timeStamp) / GST_SECOND) * sampleRate))

/**
 *
 *
 *
 */
typedef enum {
	GST_AACDEC_MPEG2_LC_ADIF_FORMAT, 
	GST_AACDEC_MPEG2_LC_ADTS_FORMAT, 
	GST_AACDEC_MPEG4_LC_RAW_FORMAT, 
	GST_AACDEC_MPEG4_LTP_FORMAT, 
	GST_AACDEC_MPEG4_LC_ADTS_FORMAT, 
	GST_AACDEC_MPEG4_HE_ADTS_FORMAT, 
	GST_AACDEC_MPEG4_HEV2_ADTS_FORMAT,
	GST_AACDEC_MPEG4_HEV2_RAW_FORMAT
} GstAACDecStreamFormat;

/**
 *
 *
 *
 */
typedef enum {
	GST_AACDEC_MONO_CHANNEL, 
	GST_AACDEC_STEREO_CHANNEL
} GstAACDecChannelConfig;

/**
 *
 *
 *
 */
typedef enum {
	GST_AACDEC_1024BYTE_FRAME,
	GST_AACDEC_960BYTE_FRAME
} GstAACDecFrameBytes;


/******************************************************************************
// ippJPelements plug-in enumerations
******************************************************************************/

#define IIP_WIDTHBYTES_4B(x) ((((x)+31)&(~31))>>3)

#define BM				0x424D
#define BI_RGB			0L
#define BI_RLE8			1L
#define BI_RLE4			2L
#define BI_BITFIELDS	3L


#define WRITE_INT8(value, offset, stream) \
{\
	(stream)[offset] = (Ipp8u)(value);\
}

#define WRITE_INT16(value, offset, stream) \
{\
	register int nTemp = (value);\
	(stream)[offset+1] = (Ipp8u)((nTemp>>8) & 0xff);\
	(stream)[offset] = (Ipp8u)(nTemp & 0xff);\
}

#define WRITE_INT32(value, offset, stream) \
{\
	register int nTemp = (value);\
	(stream)[offset+3] = (Ipp8u)((nTemp>>24) & 0xff);\
	(stream)[offset+2] = (Ipp8u)((nTemp>>16) & 0xff);\
	(stream)[offset+1] = (Ipp8u)((nTemp>>8) & 0xff);\
	(stream)[offset] = (Ipp8u)(nTemp & 0xff);\
}


#define READ_INT8(offset, stream) \
	( (Ipp8u)(stream)[offset] )

#define READ_INT16(offset, stream) \
	( (Ipp16u)(stream)[offset+1]|((Ipp16u)(stream)[offset]<<8) )

#define READ_INT32(offset, stream) \
	( (((Ipp32u)(stream)[offset])&0xff)|((((Ipp32u)(stream)[offset+1])<<8)&0xff00)|\
	((((Ipp32u)(stream)[offset+2])<<16)&0xff0000)|((((Ipp32u)(stream)[offset+3])<<24)&0xff000000) )


/**
 *
 *
 *
 */
typedef enum {
	GST_JPEGDEC_OUTPUT_RGB555,
	GST_JPEGDEC_OUTPUT_RGB565,
	GST_JPEGDEC_OUTPUT_RGB888,
	GST_JPEGDEC_OUTPUT_ABGR,
	GST_JPEGDEC_OUTPUT_GRAY8,
	GST_JPEGDEC_OUTPUT_YUV,
	GST_JPEGDEC_OUTPUT_YUV444,
	GST_JPEGDEC_OUTPUT_YUV422,
	GST_JPEGDEC_OUTPUT_YUV422I,
	GST_JPEGDEC_OUTPUT_YUV411
} GstJPEGDecOutputFormat;

/**
 *
 *
 *
 */
typedef enum {
	GST_JPEGENC_JPEG_TO_BMP,
	GST_JPEGENC_JPEG_TO_YUV
} GstJPEGDecConvertMode;


/**
 *
 *
 *
 */
typedef enum {
	GST_JPEGENC_INPUT_BMP,
	GST_JPEGENC_INPUT_YUV411,
	GST_JPEGENC_INPUT_YUV422,
	GST_JPEGENC_INPUT_YUV444
} GstJPEGEncInputFormat;

/**
 *
 *
 *
 */
typedef enum {
	GST_JPEGENC_JPEG_BASELINE,
	GST_JPEGENC_JPEG_PROGRESSIVE
} GstJPEGEncJPEGMode;


/**
 *
 *
 *
 */
typedef enum {
	GST_JPEGENC_JPEG_411,
	GST_JPEGENC_JPEG_422,
	GST_JPEGENC_JPEG_444
} GstJPEGEncSubSampling;

/**
 *
 *
 *
 */
typedef enum {
	GST_JPEGENC_BMP_TO_JPEG,
	GST_JPEGENC_YUV_TO_JPEG
} GstJPEGEncConvertMode;

/**
 *
 *
 *
 */
typedef enum {
	YUV_PLANAR,
	YUV_PACKED
} IppYUVStorage;


/**
 *
 *
 *
 */
typedef struct _MDIBSPEC
{
	int       nWidth;
	int       nHeight;
	int       nStep;
	int       nPrecision;
	int       nBitsPerpixel;
	int       nNumComponent;
	int       nClrMode;
	int       nDataSize;
	unsigned char *pBitmapData;
} MDIBSPEC;

/**
 *
 *
 *
 */
typedef struct _MYUVSPEC
{
	int             nWidth;
	int             nHeight;
	int             nSamplingFormat;
	IppYUVStorage   nStorageFormat;
	int             nDataSize;
	unsigned char *pData;
} MYUVSPEC;


/******************************************************************************
// ippPARelements plug-in enumerations
******************************************************************************/

/**
 *
 *
 *
 */
/* rotate degree for interface dynamic rotate parameter, it should be aligned with m2d-lib.h */
typedef enum {
       ROTATE_NONE,
       ROTATE_90,
       ROTATE_180,
       ROTATE_270,
       ROTATE_NO_CHANGE,
} overlay1_rot_deg;

typedef struct _WindowRectangle {
  gint x;
  gint y;
  gint w;
  gint h;
  overlay1_rot_deg rotate;
}windowRectangle;

typedef enum {
	AMR_BITRATE_4750,
	AMR_BITRATE_5150,
	AMR_BITRATE_5900,
	AMR_BITRATE_6700,
	AMR_BITRATE_7400,
	AMR_BITRATE_7950,
	AMR_BITRATE_10200,
	AMR_BITRATE_12200
} amrnb_bit_rate;

typedef enum {
	AMR_BITRATE_6600,
	AMR_BITRATE_8850,
	AMR_BITRATE_12650,
	AMR_BITRATE_14250,
	AMR_BITRATE_15850,
	AMR_BITRATE_18250,
	AMR_BITRATE_19850,
	AMR_BITRATE_23050,
	AMR_BITRATE_23850	
} amrwb_bit_rate;

/* codecbuff */
typedef struct _codecbuff {
    guint8 * pdata;
    guint    size;
    guint    offset;
    guint    left;
} codecbuff;

typedef struct _GST_sI42Buf_SpecialInfo {
	unsigned char*	pY_vAddr;		//the virtual address to the 1st pixel(on the top-left) should be displayed
	unsigned char*	pU_vAddr;
	unsigned char*	pV_vAddr;
	unsigned char*	pY_pAddr;		//the physical address to the 1st pixel should be displayed
	unsigned char*	pU_pAddr;
	unsigned char*	pV_pAddr;
	unsigned char*	pBase_vAddr;	//the start virtual address of expanded Y plannar, pY_vAddr = pBase_vAddr + iXoffset + iYoffset*iYPitch
	unsigned char*	pBase_pAddr;	//the start physical address of expanded Y plannar

	int			iYPitch;
	int			iUPitch;
	int			iVPitch;

	int			iXoffset;			//it should >= 0 and should be even, the x offset of display rectangle in the expanded rectangle, in Y plannar
	int			iYoffset;			//it should >= 0 and should be even, the y offset of display rectangle in the expanded rectangle, in Y plannar
	int			iExpandedWidth;		//it should >= 0 and should be even, in fact, it shoule equal to iYPitch
	int			iExpandedHeight;	//it should >= 0 and should be even

	void*		pDecoderData0;
	void*		pDecoderData1;
}GST_sI42Buf_SpecialInfo;
#ifndef IPPGST_BUFFER_CUSTOMDATA
#define IPPGST_BUFFER_CUSTOMDATA(buf)	(((GstBuffer*)buf)->_gst_reserved[sizeof(((GstBuffer*)buf)->_gst_reserved)/sizeof(((GstBuffer*)buf)->_gst_reserved[0]) - 1])
#endif
#define IPPGST_sI42_MAGICDATA	3

G_END_DECLS

#endif /* __IPP_GSTDEFS_H__ */

/* EOF */
