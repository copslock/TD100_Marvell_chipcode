/*****************************************************************************
*(C) Copyright [2010 - 2011] Marvell International Ltd.
* All Rights Reserved
******************************************************************************/

#ifndef __CAM_ENGINE_EXTISP_POSTPROCESSING_H_
#define __CAM_ENGINE_EXTISP_POSTPROCESSING_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "CameraEngine.h"

typedef struct
{
    CAM_Bool        bFavorAspectRatio;
	CAM_Int32s      iPpuDigitalZoomQ16;
	CAM_FlipRotate	eRotation;
	CAM_JpegParam   *pJpegParam;
	CAM_Bool        bIsInPlace;
	void            *pUsrData;         // reserved for user specific usage case
}_CAM_PostProcessParam;

typedef struct
{
	CAM_Int32s       iWidth;  // width before rotate
	CAM_Int32s       iHeight; // height before rotate
	CAM_ImageFormat  eFormat;
	CAM_FlipRotate	 eRotation;
	CAM_JpegParam    *pJpegParam;
} _CAM_ImageInfo;


// API
CAM_Error _ppu_create_handle( void **phHandle );
CAM_Error _ppu_destroy_handle( void **phHandle );

CAM_Error _ppu_query_csc_cap( CAM_ImageFormat *pBaseFmtCap, CAM_Int32s iBaseFmtCapCnt, CAM_ImageFormat *pAddOnFmtCap, CAM_Int32s *pAddOnFmtCapCnt );
CAM_Error _ppu_query_rotator_cap( CAM_FlipRotate *pRotCap, CAM_Int32s *pRotCnt );
CAM_Error _ppu_query_resizer_cap( CAM_Bool *pbIsArbitrary, CAM_Size *pMin, CAM_Size *pMax );
CAM_Error _ppu_query_jpegenc_cap( CAM_ImageFormat *pBaseFmtCap, CAM_Int32s iBaseFmtCapCnt, CAM_JpegCapability *pJpegEncCap );
CAM_Error _ppu_negotiate_format( CAM_ImageFormat eDstFmt, CAM_ImageFormat *pSrcFmt, CAM_ImageFormat *pFmtCap, CAM_Int32s iFmtCapCnt );

CAM_Error _ppu_get_bufreq( void *hHandle, _CAM_ImageInfo *pImgInfo, CAM_ImageBufferReq *pBufReq );
CAM_Error _ppu_image_process( void *hHandle, CAM_ImageBuffer *pSrcImg, CAM_ImageBuffer *pDstImg, _CAM_PostProcessParam *pPostProcessParam );

#ifdef __cplusplus
}
#endif

#endif // __CAM_ENGINE_EXTISP_POSTPROCESSING_H_