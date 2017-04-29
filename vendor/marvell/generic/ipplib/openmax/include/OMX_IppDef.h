/******************************************************************************
// (C) Copyright 2007 Marvell International Ltd.
// All Rights Reserved
// INTEL CONFIDENTIAL
// Copyright 2000 ~ 2006 Intel Corporation All Rights Reserved. 
// The source code contained or described herein and all documents related to 
// the source code (¡°Material¡±) are owned by Intel Corporation or its 
// suppliers or licensors. Title to the Material remains with Intel Corporation
// or its suppliers and licensors. The Material contains trade secrets and 
// proprietary and confidential information of Intel or its suppliers and 
// licensors. The Material is protected by worldwide copyright and trade secret 
// laws and treaty provisions. No part of the Material may be used, copied, 
// reproduced, modified, published, uploaded, posted, transmitted, distributed, 
// or disclosed in any way without Intel¡¯s prior express written permission.
//
// No license under any patent, copyright, trade secret or other intellectual 
// property right is granted to or conferred upon you by disclosure or delivery 
// of the Materials, either expressly, by implication, inducement, estoppel or 
// otherwise. Any license under such intellectual property rights must be 
// express and approved by Intel in writing.
//
******************************************************************************/

/** @file OMX_IppDef.h
 *  The structures needed by IppDef components to exchange
 *  parameters and configuration data with the componenmilts.
 */

#ifndef OMX_IppDef_h
#define OMX_IppDef_h

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* Each OMX header must include all required header files to allow the
 *  header to compile without errors.  The includes below are required
 *  for this header file to compile successfully 
 */
#include <OMX_Types.h>
#include <OMX_Core.h>



// method
#define MARCO_MAX(_a_,_b_) (_a_>_b_)?_a_:_b_



// Indexes
#define		OMX_IndexIndexMarvellStartUnused		0xFF000000
#define		OMX_IndexParamMarvellH264Enc			0xFF000005	/**< reference: OMX_VIDEO_PARAM_MARVELL_H264ENC */
#define     OMX_IndexParamMarvellH264Dec			0xFF000006  /**< reference: OMX_VIDEO_PARAM_MARVELL_H264DEC */
#define		OMX_IndexParamMarvellH263Enc			0xFF000007	/**< reference: OMX_VIDEO_PARAM_MARVELL_H263ENC */
#define		OMX_IndexParamMarvellWMVDec			    0xFF00000F /**< reference: OMX_VIDEO_PARAM_MARVELL_WMVDEC */
#define		OMX_IndexParamMarvellWMADec			    0xFF000010 /**< reference: OMX_VIDEO_PARAM_MARVELL_WMADEC */
#define     OMX_IndexParamMarvellCodaDx8Dec         0xFF000011 /**< reference: OMX_VIDEO_PARAM_MARVELL_CODADX8DEC */
#define     OMX_IndexParamMarvellVmetaDec           0xFF000012  /**< reference: OMX_VIDEO_PARAM_MARVELL_VMETADEC */
#define     OMX_IndexParamMarvellVmetaEnc           0xFF000013  /**< reference: OMX_VIDEO_PARAM_MARVELL_VMETAENC */
#define     OMX_IndexParamMarvellCodaDx8Enc         0xFF000014 /**< reference: OMX_VIDEO_PARAM_MARVELL_CODADX8ENC */
#define     OMX_IndexParamMarvellAACDec             0xFF000015 /**< reference: OMX_VIDEO_PARAM_MARVELL_AACENC */
#define     OMX_IndexParamMarvellAACEnc             0xFF000016 /**< reference: OMX_VIDEO_PARAM_MARVELL_AACENC */


#define		OMX_IndexParamMarvellInputMode			0xFF100000	/**< reference: OMX_OTHER_PARAM_MARVELL_INPUTMODETYPE */
#define		OMX_IndexParamMarvellUseSourceBuffer	0xFF100001/**< reference: OMX_OTHER_PARAM_MARVELL_USESOURCEBUFFERTYPE */
#define     OMX_IndexParamMarvellCapabilty          0xFF100002 /**< reference: OMX_OTHER_PARAM_MARVELL_CAPABILITYTYPE */

#define		OMX_IndexConfigTimeDuration				0x09FFFFFF  /**< reference: OMX_TIME_CONFIG_TIMESTAMPTYPE */


// define for WMA
#define WMA_DECOPT_CHANNEL_DOWNMIXING	            0x00000001
#define WMA_DECOPT_REQUANTTO16			            0x00000040
#define WMA_DECOPT_DOWNSAMPLETO44OR48	            0x00000080

#define IPP_WMA_MAX_CBSIZE 32


// Extension index parameter structure


typedef enum IPPOMX_INPUTMODE
{
	ONE_ENTIRE_FRAME				= 0,
	PARTIAL_END_OF_FRAME	= 1,
	STREAM_IN								= 2,
	INPUT_MODE_MAX		= 0x7FFFFFFF
}IPPOMX_INPUTMODE;

/* Input Mode Parameter Structure Name is "OMX.Marvell.index.param.inputMode" */
/*	Input Mode  Specific Parameter Structure	*/
typedef struct OMX_OTHER_PARAM_MARVELL_INPUTMODETYPE {
	OMX_U32				nSize;
	OMX_VERSIONTYPE		nVersion;
	OMX_U32				nPortIndex;
	IPPOMX_INPUTMODE	inputMode;
}OMX_OTHER_PARAM_MARVELL_INPUTMODETYPE;


/* Whether iv Render To Use Source Buffer Parameter Structure Name is "OMX.Marvell.index.param.useSourceBuffer" */
/*	Use Source Buffer Specific Parameter Structure	*/
typedef struct OMX_OTHER_PARAM_MARVELL_USESOURCEBUFFERTYPE {
	OMX_U32				nSize;
	OMX_VERSIONTYPE		nVersion;
	OMX_U32				nPortIndex;
	OMX_BOOL			bUseSrcBuf;
}OMX_OTHER_PARAM_MARVELL_USESOURCEBUFFERTYPE;


/* H.264 Encoder Parameter Structure Name is "OMX.Marvell.index.param.H264Encoder" */
/*	H.264 Encoder Specific Parameter Structure	*/
typedef struct OMX_VIDEO_PARAM_MARVELL_H264ENC{
    OMX_U32			nSize;
    OMX_VERSIONTYPE	nVersion;
    OMX_U32			nPortIndex;
    //expose
    int           iQpFirstFrame;
}OMX_VIDEO_PARAM_MARVELL_H264ENC;

/* H.264 Decoder Parameter Structure Name is "OMX.Marvell.index.param.H264Decoder" */
/* H.264 Decoder Specific Parameter Structure	*/
typedef struct OMX_VIDEO_PARAM_MARVELL_H264DEC{
	OMX_U32			nSize;
	OMX_VERSIONTYPE nVersion;
	OMX_U32			nPortIndex;
	OMX_BOOL		bReorder;	
}OMX_VIDEO_PARAM_MARVELL_H264DEC;

/* H.263 Encoder Parameter Structure Name is "OMX.Marvell.index.param.H263Encoder" */
/*	H.263 Encoder Specific Parameter Structure	*/
typedef struct OMX_VIDEO_PARAM_MARVELL_H263ENC {
    OMX_U32			nSize;
    OMX_VERSIONTYPE	nVersion;
    OMX_U32			nPortIndex;
    int             iIntraQuant;
    int             iInterQuant;
    int				optDF;
    int				optSS;
    int				BitsPerSlice;
    int             iRCType;
}OMX_VIDEO_PARAM_MARVELL_H263ENC;

/* WMV Decoder Parameter Structure Name is "OMX.Marvell.index.param.WMVDecoder" */
/*	WMV Decoder Specific Parameter Structure	*/
typedef struct OMX_VIDEO_PARAM_MARVELL_WMVDEC{
	OMX_U32			nSize;
	OMX_VERSIONTYPE	nVersion;
	OMX_U32			nPortIndex;
	//expose
	int           codec_version;
}OMX_VIDEO_PARAM_MARVELL_WMVDEC;

/* WMA Decoder Parameter Structure Name is "OMX.Marvell.index.param.WMADecoder" */
/*	WMA Decoder Specific Parameter Structure	*/
typedef struct OMX_AUDIO_PARAM_MARVELL_WMADEC{
	OMX_U32			nSize;
	OMX_VERSIONTYPE	nVersion;
	OMX_U32			nPortIndex;
	//expose
	OMX_U16 wFormatTag;			/* format type */
//	OMX_S16 nChannels;			/* number of channels (i.e. mono, stereo...) */
//	OMX_S32 nSamplesPerSec;		/* sample rate */
	OMX_S32 nAvgBytesPerSec;		/* for buffer estimation */
	OMX_S16 nBlockAlign;			/* block size of data */
	OMX_S16 wBitsPerSample;		/* number of bits per sample of mono data */
	OMX_S16 cbSize;				/* the count in bytes of the size of */
	/* extra information (after cbSize) */
	OMX_U8 pData[IPP_WMA_MAX_CBSIZE];
	/* extra information data */
	OMX_S16 iRmsAmplitudeRef;
	OMX_S16 iRmsAmplitudeTarget;
	OMX_S16 iPeakAmplitudeRef;
	OMX_S16 iPeakAmplitudeTarget;

	/* Additional parameters set by user */
	OMX_U32 iDecoderFlags;			/* Flag for decoding, could be multiple set:
								IPP_DECOPT_CHANNEL_DOWNMIXING, downmixing to two channels
								IPP_DECOPT_REQUANTTO16, requant to 16 bit
								IPP_DECOPT_DOWNSAMPLETO44OR48, downsample to 44.1 or 48 kHz
								*/
}OMX_AUDIO_PARAM_MARVELL_WMADEC;

/*ChipMedia Codadx8 Decoder Parameter Structure Name is "OMX.Marvell.index.param.CodaDx8Decoder"*/
/*ChipMedia Codadx8 Decoder Specific Parameter*/
typedef struct OMX_VIDEO_PARAM_MARVELL_CODADX8DEC{
	OMX_U32			nSize;
	OMX_VERSIONTYPE	nVersion;
	OMX_U32			nPortIndex;
	
	OMX_BOOL bRotation;
	OMX_U32  nRotationAngle; /*0,90,180,270*/
	OMX_BOOL bMirror;
	OMX_U32  nMirrorDir; /*0, 1(VER), 2(HOR), 3(VER_HOR)*/
	OMX_BOOL bEnableMultiInstances;

}OMX_VIDEO_PARAM_MARVELL_CODADX8DEC;
/*ChipMedia Codadx8 Encoder Parameter Structure Name is "OMX.Marvell.index.param.CodaDx8Encoder"*/
typedef struct OMX_VIDEO_PARAM_MARVELL_CODADX8ENC{
	OMX_U32			nSize;
	OMX_VERSIONTYPE	nVersion;
	OMX_U32			nPortIndex;
	
	OMX_BOOL bEnableMultiInstances;
	OMX_BOOL bShareInputBuffer;
}OMX_VIDEO_PARAM_MARVELL_CODADX8ENC;

/* Capability Parameter Structure Name is "OMX.Marvell.index.param.capability" */
/*	Capability  Specific Parameter Structure	*/
typedef struct OMX_OTHER_PARAM_MARVELL_CAPABILITYTYPE {
	OMX_U32				nSize;
	OMX_VERSIONTYPE		nVersion;

	OMX_BOOL bComponentMultiThreaded;
    OMX_BOOL bSupportsEntireFrames;
    OMX_BOOL bSupportsPartialFrames;
    OMX_BOOL bSupportsStreamIn;
	
}OMX_OTHER_PARAM_MARVELL_CAPABILITYTYPE;

/* Vmeta Decoder Parameter Structure Name is "OMX.Marvell.index.param.VmetaDecoder" */
/*Vmeta Decoder Specific Parameter Structure*/
#define VC1_SIMPLE_PROFILE                  0
#define VC1_MAIN_PROFILE                    4
#define VC1_ADVANCED_PROFILE                12
#define ENABLE_ADVANAVSYNC_1080P            0x1
#define ENABLE_ADVANAVSYNC_720P             0x2
#define ENABLE_POWEROPT                     (1<<31)
typedef struct OMX_VIDEO_PARAM_MARVELL_VMETADEC{
    OMX_U32         nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32         nPortIndex;
    OMX_BOOL        bReorder;       /*OMX_TRUE or OMX_FALSE*/
    OMX_U32         nVC1Profile;    /*VC1_SIMPLE_PROFILE, or VC1_MAIN_PROFILE, or VC1_ADVANCED_PROFILE*/
    OMX_BOOL        bMultiIns;      /*OMX_TRUE or OMX_FALSE*/
    OMX_U32         nAdvanAVSync;   /*0x1: enable advanced av sync only for 1080p, 0x3 enable for 1080p&720p*/
    OMX_S32         nThreOffset;    /*advanced av sync parameter, [-3, 0], nThreOffset+FrameRate is target fps*/
} OMX_VIDEO_PARAM_MARVELL_VMETADEC;

/* Vmeta Encoder Parameter Structure Name is "OMX.Marvell.index.param.VmetaEncoder" */
/*Vmeta Encoder Specific Parameter Structure*/
typedef struct OMX_VIDEO_PARAM_MARVELL_VMETAENC{
    OMX_U32         nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32         nPortIndex;
    OMX_BOOL        bMultiIns;      /*OMX_TRUE or OMX_FALSE*/
    OMX_S32         nRCType;        /* valid when rc enable
                                    // 0: Best visual quality
                                    // 1: Median visual quality, median bit-rate control
                                    // 2: Best bit-rate control*/
    OMX_S32         nMaxBitRate;    /* (b/s), valid when rc enable and nRCType=2*/
} OMX_VIDEO_PARAM_MARVELL_VMETAENC;

/* AAC Encoder Parameter Structure Name is "OMX.Marvell.index.param.AACEncoder" */
/*AAC Encoder Specific Parameter Structure*/
typedef struct _OMX_AUDIO_PARAM_MARVELL_AACENC{
    OMX_U32         nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32         nPortIndex;
    OMX_BOOL        bOutputSpecificData;      /*OMX_TRUE or OMX_FALSE*/
} OMX_AUDIO_PARAM_MARVELL_AACENC;

// Color formats
#define		OMX_COLOR_FormatMarvellStartUnused	0x7f000000
#define		OMX_COLOR_FormatYV12				( (OMX_COLOR_FORMATTYPE)(OMX_COLOR_FormatMarvellStartUnused + 1) )
#define		OMX_COLOR_Format15bitRGB555			( (OMX_COLOR_FORMATTYPE)(OMX_COLOR_FormatMarvellStartUnused + 2) )
#define		OMX_COLOR_Format16bitBGR555			( (OMX_COLOR_FORMATTYPE)(OMX_COLOR_FormatMarvellStartUnused + 3) )
#define		OMX_COLOR_FormatGRAY8		        ( (OMX_COLOR_FORMATTYPE)(OMX_COLOR_FormatMarvellStartUnused + 4) )
#define		OMX_COLOR_FormatYUV444Planar		( (OMX_COLOR_FORMATTYPE)(OMX_COLOR_FormatMarvellStartUnused + 5) )
#define		OMX_COLOR_FormatIppPicture			( (OMX_COLOR_FORMATTYPE)(OMX_COLOR_FormatMarvellStartUnused + 6) )

// Flag
#define OMX_BUFFERFLAG_NEWSEGMENT 0x7F000000
/** 
 * Rotation
 *
 * ENUMS
 * 
 */

typedef enum OMX_IMAGE_INTERPOLATIONTYPE {
    OMX_IMAGE_InterpolationNearest = 0,
    OMX_IMAGE_InterpolationBilinear = 1,
    OMX_IMAGE_InterpolationCubical = 2,
    OMX_IMAGE_InterpolationMedian = 3,
    OMX_IMAGE_InterpolationNearlinear = 4,
    OMX_IMAGE_InterpolationMax = 0x7FFFFFFF
} OMX_IMAGE_INTERPOLATIONTYPE;


// video define
#define     VIDEO_CIF_SIZE		(352*288*3/2)     
#define     VIDEO_QCIF_SIZE		(176*144*3/2)     
#define     MAX_VIDEOSTREAM_LEN_QCIF		(64*1024/8)     
#define		MAX_VIDEOSTREAM_LEN_CIF			(1024*1024/8)
#define		VIDEOSTREAM_BUFFER_SIZE			(MAX_VIDEOSTREAM_LEN_CIF)


#define IPP_OMX_BUFFERFLAG_INTERLACEFRAME        0x00000001
typedef struct _OMX_BUFFERHEADERTYPE_IPPEXT {
    OMX_BUFFERHEADERTYPE    bufheader;
    OMX_U32                 nPhyAddr;
    OMX_BOOL                bAllocBufInternal;      /*to differentiate internally or externally allocating buffer*/
    OMX_U32                 nExtFlags;              /*extension flags*/
}OMX_BUFFERHEADERTYPE_IPPEXT;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
/* File EOF */
