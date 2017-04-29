/******************************************************************************
// (C) Copyright 2009 Marvell International Ltd.
// All Rights Reserved
******************************************************************************/
#include "OMX_Core.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


// Entry Point Declaration

// Clock component
OMX_API OMX_ERRORTYPE OMX_APIENTRY CIppOmxClock2Comp_CreateInstance(OMX_IN OMX_HANDLETYPE hComponent);

// Source components
OMX_API OMX_ERRORTYPE OMX_APIENTRY CIppOmxCameraSrcComp_CreateInstance(OMX_IN OMX_HANDLETYPE hComponent);

// Demuxer components
OMX_API OMX_ERRORTYPE OMX_APIENTRY CIppOmxMp4DemuxerComp_CreateInstance(OMX_IN OMX_HANDLETYPE hComponent);

// Decoder / Encoder components
// Video
OMX_API OMX_ERRORTYPE OMX_APIENTRY CIppOmxMpeg4AspDecComp_CreateInstance(OMX_IN OMX_HANDLETYPE hComponent);
OMX_API OMX_ERRORTYPE OMX_APIENTRY CIppOmxMpeg4EncComp_CreateInstance(OMX_IN OMX_HANDLETYPE hComponent);
OMX_API OMX_ERRORTYPE OMX_APIENTRY CIppOmxH263DecComp_CreateInstance(OMX_IN OMX_HANDLETYPE hComponent);
OMX_API OMX_ERRORTYPE OMX_APIENTRY CIppOmxH263EncComp_CreateInstance(OMX_IN OMX_HANDLETYPE hComponent);
OMX_API OMX_ERRORTYPE OMX_APIENTRY CIppOmxH264DecComp_CreateInstance(OMX_IN OMX_HANDLETYPE hComponent);
OMX_API OMX_ERRORTYPE OMX_APIENTRY CIppOmxH264EncComp_CreateInstance(OMX_IN OMX_HANDLETYPE hComponent);
OMX_API OMX_ERRORTYPE OMX_APIENTRY CIppOmxWmvDecComp_CreateInstance(OMX_IN OMX_HANDLETYPE hComponent);
OMX_API OMX_ERRORTYPE OMX_APIENTRY CIppOmxVmetaDecComp_CreateInstance(OMX_IN OMX_HANDLETYPE hComponent);
OMX_API OMX_ERRORTYPE OMX_APIENTRY CIppOmxCodaDx8DecComp_CreateInstance(OMX_IN OMX_HANDLETYPE hComponent);
OMX_API OMX_ERRORTYPE OMX_APIENTRY CIppOmxCodaDx8EncComp_CreateInstance(OMX_IN OMX_HANDLETYPE hComponent);
// Audio
OMX_API OMX_ERRORTYPE OMX_APIENTRY CIppOmxMp3DecComp_CreateInstance(OMX_IN OMX_HANDLETYPE hComponent);
OMX_API OMX_ERRORTYPE OMX_APIENTRY CIppOmxAacDecComp_CreateInstance(OMX_IN OMX_HANDLETYPE hComponent);
OMX_API OMX_ERRORTYPE OMX_APIENTRY CIppOmxAacEncComp_CreateInstance(OMX_IN OMX_HANDLETYPE hComponent);
OMX_API OMX_ERRORTYPE OMX_APIENTRY CIppOmxGsmamrDecComp_CreateInstance(OMX_IN OMX_HANDLETYPE hComponent);
OMX_API OMX_ERRORTYPE OMX_APIENTRY CIppOmxGsmamrEncComp_CreateInstance(OMX_IN OMX_HANDLETYPE hComponent);
OMX_API OMX_ERRORTYPE OMX_APIENTRY CIppOmxAmrwbDecComp_CreateInstance(OMX_IN OMX_HANDLETYPE hComponent);
OMX_API OMX_ERRORTYPE OMX_APIENTRY CIppOmxWmaDecComp_CreateInstance(OMX_IN OMX_HANDLETYPE hComponent);
OMX_API OMX_ERRORTYPE OMX_APIENTRY CIppOmxAmrwbEncComp_CreateInstance(OMX_IN OMX_HANDLETYPE hComponent);
OMX_API OMX_ERRORTYPE OMX_APIENTRY CIppOmxQcelpDecComp_CreateInstance(OMX_IN OMX_HANDLETYPE hComponent);
OMX_API OMX_ERRORTYPE OMX_APIENTRY CIppOmxQcelpEncComp_CreateInstance(OMX_IN OMX_HANDLETYPE hComponent);
// Image
OMX_API OMX_ERRORTYPE OMX_APIENTRY CIppOmxRszRotCscComp_CreateInstance(OMX_IN OMX_HANDLETYPE hComponent);
OMX_API OMX_ERRORTYPE OMX_APIENTRY CIppOmxJpegDecComp_CreateInstance(OMX_IN OMX_HANDLETYPE hComponent);
OMX_API OMX_ERRORTYPE OMX_APIENTRY CIppOmxJpegEncComp_CreateInstance(OMX_IN OMX_HANDLETYPE hComponent);
OMX_API OMX_ERRORTYPE OMX_APIENTRY CIppOmxPngDecComp_CreateInstance(OMX_IN OMX_HANDLETYPE hComponent);
OMX_API OMX_ERRORTYPE OMX_APIENTRY CIppOmxGifDecComp_CreateInstance(OMX_IN OMX_HANDLETYPE hComponent);

// Render components
OMX_API OMX_ERRORTYPE OMX_APIENTRY CIppOmxAudioRendererPCMComp_CreateInstance(OMX_IN OMX_HANDLETYPE hComponent);
OMX_API OMX_ERRORTYPE OMX_APIENTRY CIppOmxIVRendererYUVOverlayComp_CreateInstance(OMX_IN OMX_HANDLETYPE hComponent);



OMX_API OMX_ERRORTYPE OMX_APIENTRY CIppOmxMpeg4AspDecMVEDComp_CreateInstance(OMX_IN OMX_HANDLETYPE hComponent);
OMX_API OMX_ERRORTYPE OMX_APIENTRY CIppOmxMpeg4EncMVEDComp_CreateInstance(OMX_IN OMX_HANDLETYPE hComponent);
OMX_API OMX_ERRORTYPE OMX_APIENTRY CIppOmxH264DecMVEDComp_CreateInstance(OMX_IN OMX_HANDLETYPE hComponent);
OMX_API OMX_ERRORTYPE OMX_APIENTRY CIppOmxH264EncMVEDComp_CreateInstance(OMX_IN OMX_HANDLETYPE hComponent);
OMX_API OMX_ERRORTYPE OMX_APIENTRY CIppOmxWmvDecMVEDComp_CreateInstance(OMX_IN OMX_HANDLETYPE hComponent);

// Entry Table
OMX_COMPONENTREGISTERTYPE OMX_ComponentRegistered[] = {

// Clock component
#ifdef _MARVELL_OTHER_CLOCK2
	{"OMX.MARVELL.OTHER.CLOCK", CIppOmxClock2Comp_CreateInstance},
#endif

// Source components
#ifdef _MARVELL_OTHER_CAMERASRC
	{"OMX.MARVELL.OTHER.CAMERASRC", CIppOmxCameraSrcComp_CreateInstance},
#endif

// Demuxer components
#ifdef _MARVELL_OTHER_MP4DEMUXER
	{"OMX.MARVELL.OTHER.MP4DEMUXER", CIppOmxMp4DemuxerComp_CreateInstance},
#endif

// Decoder / Encoder components
// Video
#ifdef _MARVELL_VIDEO_CODADX8DECODER
	{"OMX.MARVELL.VIDEO.CODADX8DECODER",CIppOmxCodaDx8DecComp_CreateInstance},
#endif

#ifdef _MARVELL_VIDEO_CODADX8ENCODER
        {"OMX.MARVELL.VIDEO.CODADX8ENCODER",CIppOmxCodaDx8EncComp_CreateInstance},
#endif

#ifdef _MARVELL_VIDEO_MPEG4ASPDECODER
	{"OMX.MARVELL.VIDEO.MPEG4ASPDECODER", CIppOmxMpeg4AspDecComp_CreateInstance},
#endif

#ifdef _MARVELL_VIDEO_MPEG4ASPDECODERMVED
    {"OMX.MARVELL.VIDEO.MPEG4ASPDECODERMVED", CIppOmxMpeg4AspDecMVEDComp_CreateInstance},
#endif
	
#ifdef _MARVELL_VIDEO_MPEG4ENCODER
	{"OMX.MARVELL.VIDEO.MPEG4ENCODER", CIppOmxMpeg4EncComp_CreateInstance},
#endif

#ifdef _MARVELL_VIDEO_MPEG4ENCODERMVED
    {"OMX.MARVELL.VIDEO.MPEG4ENCODERMVED", CIppOmxMpeg4EncMVEDComp_CreateInstance},
#endif

#ifdef _MARVELL_VIDEO_H263DECODER
	{"OMX.MARVELL.VIDEO.H263DECODER", CIppOmxH263DecComp_CreateInstance},
#endif

#ifdef _MARVELL_VIDEO_H263ENCODER
	{"OMX.MARVELL.VIDEO.H263ENCODER", CIppOmxH263EncComp_CreateInstance},
#endif

#ifdef _MARVELL_VIDEO_H264DECODER
	{"OMX.MARVELL.VIDEO.H264DECODER", CIppOmxH264DecComp_CreateInstance},
#endif


#ifdef _MARVELL_VIDEO_H264DECODERMVED
    {"OMX.MARVELL.VIDEO.H264DECODERMVED", CIppOmxH264DecMVEDComp_CreateInstance},
#endif


#ifdef _MARVELL_VIDEO_H264ENCODER
	{"OMX.MARVELL.VIDEO.H264ENCODER", CIppOmxH264EncComp_CreateInstance},
#endif

#ifdef _MARVELL_VIDEO_H264ENCODERMVED
    {"OMX.MARVELL.VIDEO.H264ENCODERMVED", CIppOmxH264EncMVEDComp_CreateInstance},
#endif

#ifdef _MARVELL_VIDEO_VMETADECODER
    {"OMX.MARVELL.VIDEO.VMETADECODER", CIppOmxVmetaDecComp_CreateInstance},
#endif

#ifdef _MARVELL_VIDEO_WMVDECODER
	{"OMX.MARVELL.VIDEO.WMVDECODER", CIppOmxWmvDecComp_CreateInstance},
#endif

#ifdef _MARVELL_VIDEO_WMVDECODERMVED
    {"OMX.MARVELL.VIDEO.WMVDECODERMVED", CIppOmxWmvDecMVEDComp_CreateInstance},
#endif

//Audio
#ifdef _MARVELL_AUDIO_MP3DECODER
	{"OMX.MARVELL.AUDIO.MP3DECODER", CIppOmxMp3DecComp_CreateInstance},
#endif

#ifdef _MARVELL_AUDIO_AACDECODER
	{"OMX.MARVELL.AUDIO.AACDECODER", CIppOmxAacDecComp_CreateInstance},
#endif

#ifdef _MARVELL_AUDIO_AACENCODER
	{"OMX.MARVELL.AUDIO.AACENCODER", CIppOmxAacEncComp_CreateInstance},
#endif

#ifdef _MARVELL_AUDIO_AMRNBDECODER
	{"OMX.MARVELL.AUDIO.AMRNBDECODER", CIppOmxGsmamrDecComp_CreateInstance},
#endif

#ifdef _MARVELL_AUDIO_AMRNBENCODER
	{"OMX.MARVELL.AUDIO.AMRNBENCODER", CIppOmxGsmamrEncComp_CreateInstance},
#endif

#ifdef _MARVELL_AUDIO_AMRWBDECODER
	{"OMX.MARVELL.AUDIO.AMRWBDECODER", CIppOmxAmrwbDecComp_CreateInstance},
#endif

#ifdef _MARVELL_AUDIO_WMADECODER
	{"OMX.MARVELL.AUDIO.WMADECODER", CIppOmxWmaDecComp_CreateInstance},
#endif

#ifdef _MARVELL_AUDIO_AMRWBENCODER
	{"OMX.MARVELL.AUDIO.AMRWBENCODER", CIppOmxAmrwbEncComp_CreateInstance},
#endif

#ifdef _MARVELL_AUDIO_QCELPDECODER
	{"OMX.MARVELL.AUDIO.QCELPDECODER", CIppOmxQcelpDecComp_CreateInstance},
#endif
	
#ifdef _MARVELL_AUDIO_QCELPENCODER
	{"OMX.MARVELL.AUDIO.QCELPENCODER", CIppOmxQcelpEncComp_CreateInstance},
#endif

// Image
#ifdef _MARVELL_IMAGE_RSZROTCSC
	{"OMX.MARVELL.IMAGE.RSZROTCSC", CIppOmxRszRotCscComp_CreateInstance},
#endif

#ifdef _MARVELL_IMAGE_PNGDECODER
	{"OMX.MARVELL.IMAGE.PNGDECODER", CIppOmxPngDecComp_CreateInstance},
#endif
	
#ifdef _MARVELL_IMAGE_GIFDECODER
	{"OMX.MARVELL.IMAGE.GIFDECODER", CIppOmxGifDecComp_CreateInstance},
#endif

#ifdef _MARVELL_IMAGE_JPEGDECODER
	{"OMX.MARVELL.IMAGE.JPEGDECODER", CIppOmxJpegDecComp_CreateInstance},
#endif

#ifdef _MARVELL_IMAGE_JPEGENCODER
	{"OMX.MARVELL.IMAGE.JPEGENCODER", CIppOmxJpegEncComp_CreateInstance},
#endif

// Render components
#ifdef _MARVELL_AUDIO_RENDERERPCM
	{"OMX.MARVELL.AUDIO.RENDERERPCM", CIppOmxAudioRendererPCMComp_CreateInstance},
#endif

#ifdef _MARVELL_OTHER_IVRENDERERYUVOVERLAY
	{"OMX.MARVELL.OTHER.IVRENDERERYUVOVERLAY", CIppOmxIVRendererYUVOverlayComp_CreateInstance},
#endif
};

int _nMaxComponentNum = ( sizeof(OMX_ComponentRegistered) / sizeof(OMX_ComponentRegistered[0]) );


#ifdef __cplusplus
}
#endif /* __cplusplus */

