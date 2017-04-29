/*****************************************************************************
*(C) Copyright [2010 - 2011] Marvell International Ltd.
* All Rights Reserved
******************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "cam_extisp_ppu.h"
#include "cam_extisp_eng.h"

#include "cam_log.h"
#include "cam_utility.h"


#if defined( BUILD_OPTION_STRATEGY_ENABLE_CSC_CBYCRY_RGB )
#if ( PLATFORM_GRAPHICS_VALUE < PLATFORM_GRAPHICS_GC200 ) || ( PLATFORM_GRAPHICS_VALUE > PLATFORM_GRAPHICS_GC2000 )
#error No suitable GCU. Pls undefine the following macro: BUILD_OPTION_STRATEGY_ENABLE_CSC_CBYCRY_RGB
#endif
#include "gcu.h"
#define GCU_SUBSAMPLE_THRESHOLD		1.5
#endif

#include "ippdefs.h"
#include "ippIP.h"
#include "misc.h"
#include "codecDef.h"
#include "codecJP.h"

#define IPP_JPEG_SRCFMT CAM_IMGFMT_YCbCr420P

static void _ppu_retrieve_format( CAM_ImageFormat eFmt, CAM_ImageFormat *pFmtCap, CAM_Int32s iFmtCapCnt, CAM_Int32s *pIndex );

#define PPU_DUMP_FRAME( sFileName, pImgBuf ) do {\
                                                 FILE *fp = NULL;\
                                                 fp = fopen( sFileName, "wb" );\
                                                 fwrite( pImgBuf->pBuffer[0], pImgBuf->iFilledLen[0], 1, fp );\
                                                 fflush( fp );\
                                                 fclose( fp );\
                                                 fp = NULL;\
                                                } while ( 0 )
                                                                                           

// API
// create a post-processing handler for use
CAM_Error _ppu_create_handle( void **phHandle )
{
#if defined( BUILD_OPTION_STRATEGY_ENABLE_CSC_CBYCRY_RGB )
	{
		GCU_INIT_DATA    stInitData;
		GCU_CONTEXT_DATA stContextData;

		memset( &stInitData, 0, sizeof(stInitData) );
		// stInitData.debug = GCU_TRUE;
		gcuInitialize( &stInitData );

		memset( &stContextData, 0, sizeof(stContextData) );
		*phHandle = (void*)gcuCreateContext( &stContextData );
		TRACE( CAM_INFO, "Info: ppu handle:%d\n", *phHandle );

		ASSERT( *phHandle != NULL );
	}
#else
	*phHandle = NULL;
#endif

	return CAM_ERROR_NONE;
}

CAM_Error _ppu_destroy_handle( void **phHandle )
{
#if defined( BUILD_OPTION_STRATEGY_ENABLE_CSC_CBYCRY_RGB )
	if ( *phHandle != NULL )
	{
		gcuDestroyContext( *phHandle );
	}

	gcuTerminate();

	*phHandle = NULL;
#endif

	return CAM_ERROR_NONE;
}

CAM_Error _ppu_query_jpegenc_cap( CAM_ImageFormat *pBaseFmtCap, CAM_Int32s iBaseFmtCapCnt, CAM_JpegCapability *pJpegEncCap )
{
	memset( pJpegEncCap, 0, sizeof( CAM_JpegCapability ) );
#if defined( BUILD_OPTION_STARTEGY_ENABLE_JPEGENCODER )
	{
		CAM_Int32s iIndex = -1;
		_ppu_retrieve_format( IPP_JPEG_SRCFMT, pBaseFmtCap, iBaseFmtCapCnt, &iIndex );
		if ( iIndex < 0 )
		{
			pJpegEncCap->bSupportJpeg      = CAM_FALSE;
		}
		else
		{
			pJpegEncCap->bSupportJpeg      = CAM_TRUE;
			pJpegEncCap->iMinQualityFactor = 5;
			pJpegEncCap->iMaxQualityFactor = 95;
			pJpegEncCap->bSupportExif      = CAM_FALSE;
			pJpegEncCap->bSupportThumb     = CAM_FALSE;
		}
	}
#else			
	pJpegEncCap->bSupportJpeg      = CAM_FALSE;
#endif			
	return CAM_ERROR_NONE;
}

CAM_Error _ppu_query_csc_cap( CAM_ImageFormat *pBaseFmtCap, CAM_Int32s iBaseFmtCapCnt, CAM_ImageFormat *pAddOnFmtCap, CAM_Int32s *pAddOnFmtCapCnt )
{
	CAM_Int32s cnt = 0;
	CAM_Int32s i   = 0;

#if defined( BUILD_OPTION_STRATEGY_ENABLE_CSC_CBYCRY_RGB )
	// GCU can do UYVY --> BGR565 / BGRA8888 conversion
	// add BGR565 and BGRA8888 support if it is not natively supported
	{
		CAM_Bool bSupportCbYCrY = CAM_FALSE, bSupportBGR565 = CAM_FALSE, bSupportBGRA8888 = CAM_FALSE;

		for ( i = 0; i < iBaseFmtCapCnt; i++ )
		{
			if ( pBaseFmtCap[i] == CAM_IMGFMT_CbYCrY )
			{
				bSupportCbYCrY = CAM_TRUE;
			}
			if ( pBaseFmtCap[i] == CAM_IMGFMT_BGR565 )
			{
				bSupportBGR565 = CAM_TRUE;
			}
			if ( pBaseFmtCap[i] == CAM_IMGFMT_BGRA8888 )
			{
				bSupportBGRA8888 = CAM_TRUE;
			}
		}

		if ( bSupportCbYCrY && !bSupportBGR565 )
		{
			pAddOnFmtCap[cnt++] = CAM_IMGFMT_BGR565;
		}
		if ( bSupportCbYCrY && !bSupportBGRA8888 )
		{
			pAddOnFmtCap[cnt++] = CAM_IMGFMT_BGRA8888;
		}
	}
#endif

#if defined( BUILD_OPTION_STRATEGY_ENABLE_CSC_YCC420P_YCC420SP )
	{
		CAM_Bool bSupportYCbCr420P  = CAM_FALSE;
		CAM_Bool bSupportYCbCr420SP = CAM_FALSE, bSupportYCrCb420SP = CAM_FALSE;

		for ( i = 0; i < iBaseFmtCapCnt; i++ )
		{
			if ( pBaseFmtCap[i] == CAM_IMGFMT_YCbCr420P )
			{
				bSupportYCbCr420P = CAM_TRUE;
			}
			if ( pBaseFmtCap[i] == CAM_IMGFMT_YCbCr420SP )
			{
				bSupportYCbCr420SP = CAM_TRUE;
			}
			if ( pBaseFmtCap[i] == CAM_IMGFMT_YCrCb420SP )
			{
				bSupportYCrCb420SP = CAM_TRUE;
			}
		}
		if ( bSupportYCbCr420P && !bSupportYCbCr420SP )
		{
			pAddOnFmtCap[cnt++] = CAM_IMGFMT_YCbCr420SP;
		}
		if ( bSupportYCbCr420P && !bSupportYCrCb420SP )
		{
			pAddOnFmtCap[cnt++] = CAM_IMGFMT_YCrCb420SP;
		}
	}
#endif

	*pAddOnFmtCapCnt = cnt;

	return CAM_ERROR_NONE;
}

CAM_Error _ppu_query_rotator_cap( CAM_FlipRotate *pRotCap, CAM_Int32s *pRotCnt )
{
#if defined ( BUILD_OPTION_STRATEGY_ENABLE_ROTATOR )
	pRotCap[0] = CAM_FLIPROTATE_NORMAL;
	pRotCap[1] = CAM_FLIPROTATE_90L;
	pRotCap[2] = CAM_FLIPROTATE_90R;
	pRotCap[3] = CAM_FLIPROTATE_180;
	*pRotCnt   = 4;
#else
	pRotCap[0] = CAM_FLIPROTATE_NORMAL;
	*pRotCnt   = 1;
#endif

	return CAM_ERROR_NONE;
}

CAM_Error _ppu_query_resizer_cap( CAM_Bool *pbIsArbitrary, CAM_Size *pMin, CAM_Size *pMax )
{
#if defined( BUILD_OPTION_STRATEGY_ENABLE_RESIZER )
	*pbIsArbitrary = CAM_TRUE;
	pMin->iWidth   = 64;
	pMin->iHeight  = 64;
	pMax->iWidth   = 1920;
	pMax->iHeight  = 1920;
#else
	*pbIsArbitrary = CAM_FALSE;
#endif
	return CAM_ERROR_NONE;
}

CAM_Error _ppu_negotiate_format( CAM_ImageFormat eDstFmt, CAM_ImageFormat *pSrcFmt, CAM_ImageFormat *pFmtCap, CAM_Int32s iFmtCapCnt )
{
	CAM_Int32s      iIndex = -1;
	CAM_Error       error  = CAM_ERROR_NONE;
	CAM_ImageFormat eSrcFmt; 

	switch ( eDstFmt )
	{
	case CAM_IMGFMT_JPEG:
		eSrcFmt = IPP_JPEG_SRCFMT;
		break;

	case CAM_IMGFMT_BGR565:
	case CAM_IMGFMT_BGRA8888:
		eSrcFmt = CAM_IMGFMT_CbYCrY;
		break;

	case CAM_IMGFMT_CbYCrY:
		eSrcFmt = CAM_IMGFMT_YCbCr420P;
		break;

	case CAM_IMGFMT_YCbCr420SP:
	case CAM_IMGFMT_YCrCb420SP:
		eSrcFmt = CAM_IMGFMT_YCbCr420P;
		break;

	default:
		ASSERT( 0 );
		break;
	}

	_ppu_retrieve_format( eSrcFmt, pFmtCap, iFmtCapCnt, &iIndex );
	if ( iIndex < 0 )
	{
		TRACE( CAM_ERROR, "Error: ppu cannot handle generate image to format[%d] ( %s, %s, %d )\n", eDstFmt, __FILE__, __FUNCTION__, __LINE__ );
		error = CAM_ERROR_NOTSUPPORTEDARG;
	}
	else
	{
		*pSrcFmt = eSrcFmt;
		error = CAM_ERROR_NONE;
	}

	return error;
}

CAM_Error _ppu_get_bufreq( void *hHandle, _CAM_ImageInfo *pImgInfo, CAM_ImageBufferReq *pBufReq )
{
	void   *pUserData = NULL;
	
	// BAC check
	_CHECK_BAD_POINTER( pImgInfo );
	_CHECK_BAD_POINTER( pBufReq );

	pBufReq->eFormat = pImgInfo->eFormat;
	if ( pImgInfo->eRotation == CAM_FLIPROTATE_90L || pImgInfo->eRotation == CAM_FLIPROTATE_90R ||
	     pImgInfo->eRotation == CAM_FLIPROTATE_DFLIP || pImgInfo->eRotation == CAM_FLIPROTATE_ADFLIP )
	{
		pBufReq->iWidth  = pImgInfo->iHeight;
		pBufReq->iHeight = pImgInfo->iWidth;
	}
	else
	{
		pBufReq->iWidth  = pImgInfo->iWidth;
		pBufReq->iHeight = pImgInfo->iHeight;
	}

	// ipp lib, gcu requirement
	pBufReq->iAlignment[0] = 4096;	
	if ( pImgInfo->eFormat == CAM_IMGFMT_YCbCr444P || pImgInfo->eFormat == CAM_IMGFMT_YCbCr422P || pImgInfo->eFormat == CAM_IMGFMT_YCbCr420P )
	{
		// 32 byte alignment is reqired by DMA
		pBufReq->iAlignment[1] = 32;
		pBufReq->iAlignment[2] = 32;	
	}
	else if ( pImgInfo->eFormat == CAM_IMGFMT_YCbCr420SP || pImgInfo->eFormat == CAM_IMGFMT_YCrCb420SP )
	{
		// 32 byte alignment is reqired by DMA
		pBufReq->iAlignment[1] = 32;
		pBufReq->iAlignment[2] = 0;	
	}
	else
	{
		pBufReq->iAlignment[1] = 0;
		pBufReq->iAlignment[2] = 0;
	}

	pBufReq->iRowAlign[0] = 1;
	if ( pImgInfo->eFormat == CAM_IMGFMT_YCbCr444P || pImgInfo->eFormat == CAM_IMGFMT_YCbCr422P || pImgInfo->eFormat == CAM_IMGFMT_YCbCr420P )
	{
		pBufReq->iRowAlign[0] = 8;
		pBufReq->iRowAlign[1] = 8;
		pBufReq->iRowAlign[2] = 8;
	}
	else if ( pImgInfo->eFormat == CAM_IMGFMT_YCbCr420SP || pImgInfo->eFormat == CAM_IMGFMT_YCrCb420SP )
	{
		pBufReq->iRowAlign[0] = 8;
		pBufReq->iRowAlign[1] = 8;
		pBufReq->iRowAlign[2] = 0;
	}
	else
	{
		pBufReq->iRowAlign[1] = 0;
		pBufReq->iRowAlign[2] = 0;
	}

	pBufReq->iFlagOptimal = BUF_FLAG_PHYSICALCONTIGUOUS |
	                        BUF_FLAG_L1CACHEABLE | BUF_FLAG_L2CACHEABLE | BUF_FLAG_BUFFERABLE |
	                        BUF_FLAG_YUVBACKTOBACK | BUF_FLAG_FORBIDPADDING;

	pBufReq->iFlagAcceptable = pBufReq->iFlagOptimal | BUF_FLAG_L1NONCACHEABLE | BUF_FLAG_L2NONCACHEABLE | BUF_FLAG_UNBUFFERABLE;

	pBufReq->iMinBufCount = 3;
	_calcstep( pBufReq->eFormat, pBufReq->iWidth, pBufReq->iRowAlign, pBufReq->iMinStep );

	pUserData = NULL;
	// print JPEG buffer size infomation
	if ( pBufReq->eFormat == CAM_IMGFMT_JPEG )
	{
		pUserData = pImgInfo->pJpegParam;
		ASSERT( pUserData != NULL );

		TRACE( CAM_INFO, "Info: JPEG iWidth: %d, iHeight: %d, quality: %d\n", pBufReq->iWidth, pBufReq->iHeight, 
		       pImgInfo->pJpegParam->iQualityFactor );
	}

	(void)_calcbuflen( pBufReq->eFormat, pBufReq->iWidth, pBufReq->iHeight, pBufReq->iMinStep, pBufReq->iMinBufLen, pUserData );

	/*
	CELOG( "Info: format = %d, width = %d, height = %d, align = %d, step = %d, %d, %d, size = %d, %d, %d\n",
	       pBufReq->eFormat, pBufReq->iWidth, pBufReq->iHeight, pBufReq->iRowAlign,
	       pBufReq->iMinStep[0], pBufReq->iMinStep[1], pBufReq->iMinStep[2],
	       pBufReq->iMinBufLen[0], pBufReq->iMinBufLen[1], pBufReq->iMinBufLen[2] );
	*/

	return CAM_ERROR_NONE;

}


/* all Camera Engine's post-processing in included in this function, currently features include:
 *  1. image rotate
 *  2. image resize
 * *3. color space conversion( recommend use Marvell's CCIC to do it in camera driver if possible )
 *  4. JPEG encoder + rotate
*/
static CAM_Error _gcu_yuv2rgb_rszrot_roi( void *hHandle, CAM_ImageBuffer *pSrcImg, CAM_ImageBuffer *pDstImg, CAM_Rect *pROI, CAM_FlipRotate eRotate );
static CAM_Error _ipp_yuv_csc_rszrot_roi( CAM_ImageBuffer *pSrcImg, CAM_ImageBuffer *pDstImg, CAM_Rect *pROI, CAM_FlipRotate eRotate, CAM_Bool bIsInPlace );
static CAM_Error _ipp_jpegdec_rszrot_roi( CAM_ImageBuffer *pSrcImg, CAM_ImageBuffer *pDstImg, CAM_Rect *pROI, CAM_FlipRotate eRotate );
static CAM_Error _ipp_tojpeg_rszrot_roi( CAM_ImageBuffer *pSrcImg, CAM_ImageBuffer *pDstImg, CAM_Rect *pROI, CAM_FlipRotate eRotate, CAM_JpegParam *pJpegParam );

static CAM_Error _ipp_yuv420p_rszrot_roi( CAM_ImageBuffer *pSrcImg, CAM_ImageBuffer *pDstImg, CAM_Rect *pROI, CAM_FlipRotate eRotate );
static CAM_Error _ipp_yuv422p_rszrot_roi( CAM_ImageBuffer *pSrcImg, CAM_ImageBuffer *pDstImg, CAM_Rect *pROI, CAM_FlipRotate eRotate );
static CAM_Error _ipp_uyvy_rszrot_roi( CAM_ImageBuffer *pSrcImg, CAM_ImageBuffer *pDstImg, CAM_Rect *pROI, CAM_FlipRotate eRotate );
static CAM_Error _ipp_yuv420p2sp_rszrot_roi_swap( CAM_ImageBuffer *pSrcImg, CAM_ImageBuffer *pDstImg, CAM_Rect *pROI, CAM_FlipRotate eRotate, CAM_Bool bIsInPlace );
static CAM_Error _ipp_yuv420p2uyvy_rszrot_roi( CAM_ImageBuffer *pSrcImg, CAM_ImageBuffer *pDstImg, CAM_Rect *pROI, CAM_FlipRotate eRotate );
static CAM_Error _ipp_jpeg_rotation( CAM_ImageBuffer *pSrcImg, CAM_ImageBuffer *pDstImg, CAM_FlipRotate eRotate );
static CAM_Error _ipp_yuv2jpeg( CAM_ImageBuffer *pSrcImg, CAM_ImageBuffer *pDstImg, CAM_JpegParam *pJpegParam );
static CAM_Error _ipp_jpeg_roidec( CAM_ImageBuffer *pSrcImg, CAM_ImageBuffer *pDstImg, CAM_Rect *pROI );

static IppStatus ippiYCbCr420ToCbYCrY_8u_P3C2R_C( Ipp8u *pSrc[3], int *pSrcStep, IppiSize stSrcSize, Ipp8u *pDst[3], int *pDstStep, IppiSize stDstSize );
static IppStatus ippiYCbCr422RszRot_8u_C2R_C( const Ipp8u *pSrc, int srcStep, 
                                              IppiSize srcSize, Ipp8u *pDst, int dstStep, IppiSize dstSize, 
                                              IppCameraInterpolation interpolation, IppCameraRotation rotation,
                                              int rcpRatiox, int rcpRatioy );
static void      _ppu_copy_image( Ipp8u *pSrc, int iSrcStep, Ipp8u *pDst, int iDstStep, int iBytesPerLine, int iLines );

CAM_Error _ppu_image_process( void *hHandle, CAM_ImageBuffer *pSrcImg, CAM_ImageBuffer *pDstImg, _CAM_PostProcessParam *pPostProcessParam )
{
	CAM_Error      error         = CAM_ERROR_NONE;
	CAM_FlipRotate eRotate;
	CAM_Rect       stCrop, stROI;

	eRotate = pPostProcessParam->eRotation;

	if ( pPostProcessParam->bFavorAspectRatio )
	{
		CAM_Int32s iWidthBeforeRotate, iHeightBeforeRotate;

		if( eRotate == CAM_FLIPROTATE_90L || eRotate == CAM_FLIPROTATE_90R || eRotate == CAM_FLIPROTATE_DFLIP || eRotate == CAM_FLIPROTATE_ADFLIP )
		{
			iWidthBeforeRotate  = pDstImg->iHeight;
			iHeightBeforeRotate = pDstImg->iWidth;
		}
		else
		{
			iWidthBeforeRotate  = pDstImg->iWidth;
			iHeightBeforeRotate = pDstImg->iHeight;
		}

		_calccrop( pSrcImg->iWidth, pSrcImg->iHeight, iWidthBeforeRotate, iHeightBeforeRotate, &stCrop );
	}
	else
	{
		stCrop.iTop    = 0;
		stCrop.iLeft   = 0;
		stCrop.iHeight = pSrcImg->iHeight;
		stCrop.iWidth  = pSrcImg->iWidth;
	}

	// calc zoom ROI based on the crop
	stROI.iWidth  = _ROUND_TO( ( (CAM_Int64s)stCrop.iWidth << 16 ) / pPostProcessParam->iPpuDigitalZoomQ16, 32 );
	stROI.iHeight = ( (CAM_Int64s)stCrop.iHeight << 16 ) / pPostProcessParam->iPpuDigitalZoomQ16;

	stROI.iWidth  = _MIN( stROI.iWidth, stCrop.iWidth );
	stROI.iHeight = _MIN( stROI.iHeight, stCrop.iHeight );

	stROI.iTop    = stCrop.iTop  + ( stCrop.iHeight - stROI.iHeight ) / 2;
	stROI.iLeft   = _ROUND_TO( stCrop.iLeft + ( stCrop.iWidth- stROI.iWidth ) / 2, 16 );


	// gcu yuv->rgb resize/rotate/zoom
	if ( CAM_IMGFMT_CbYCrY == pSrcImg->eFormat && 
	     ( CAM_IMGFMT_BGR565 == pDstImg->eFormat || CAM_IMGFMT_BGRA8888 == pDstImg->eFormat ) )
	{
		ASSERT( pPostProcessParam->bIsInPlace == CAM_FALSE );
		error = _gcu_yuv2rgb_rszrot_roi( hHandle, pSrcImg, pDstImg, &stROI, eRotate ); 
	}
	// ipp yuv data resize/rotate/zoom
	else if ( CAM_IMGFMT_JPEG != pSrcImg->eFormat && CAM_IMGFMT_JPEG != pDstImg->eFormat )
	{
		error = _ipp_yuv_csc_rszrot_roi( pSrcImg, pDstImg, &stROI, eRotate, pPostProcessParam->bIsInPlace );
	}
	// JPEG snapshot generator
	else if ( CAM_IMGFMT_JPEG == pSrcImg->eFormat && CAM_IMGFMT_JPEG != pDstImg->eFormat )
	{
		ASSERT( pPostProcessParam->bIsInPlace == CAM_FALSE );
		error = _ipp_jpegdec_rszrot_roi( pSrcImg, pDstImg, &stROI, eRotate );
	}
	// YUV/JPEG to JPEG
	else if ( pDstImg->eFormat == CAM_IMGFMT_JPEG )
	{
		ASSERT( pPostProcessParam->bIsInPlace == CAM_FALSE );
		ASSERT( eRotate == CAM_FLIPROTATE_NORMAL); 
		error = _ipp_tojpeg_rszrot_roi( pSrcImg, pDstImg, &stROI, eRotate, pPostProcessParam->pJpegParam );
	}
	else
	{
		TRACE( CAM_ERROR, "Error: unsupported conversion: src format = %d, dst format = %d ( %s, %d )\n", \
		       pSrcImg->eFormat, pDstImg->eFormat, __FILE__, __LINE__ );
		error = CAM_ERROR_NOTSUPPORTEDARG;
	}

	return error;
}

static CAM_Error _gcu_yuv2rgb_rszrot_roi( void *hHandle, CAM_ImageBuffer *pSrcImg, CAM_ImageBuffer *pDstImg, CAM_Rect *pROI, CAM_FlipRotate eRotate )
{
	CAM_Error  error       = CAM_ERROR_NONE;
#if defined( BUILD_OPTION_STRATEGY_ENABLE_CSC_CBYCRY_RGB )
	_CHECK_BAD_POINTER( hHandle );
	GCUContext pGCUContext = ( GCUContext )hHandle;

	if ( !((int)pSrcImg->pBuffer[0] & 31) && !(pSrcImg->iStep[0] & 31) &&
	     !((int)pDstImg->pBuffer[0] & 31) && !(pDstImg->iStep[0] & 31) )
	{
		GCU_BLT_DATA stBltData;
		GCU_RECT     stSrcRect, stDstRect;

		memset( &stBltData, 0, sizeof( GCU_BLT_DATA ) );
		if ( pROI->iHeight >= pDstImg->iHeight * GCU_SUBSAMPLE_THRESHOLD )
		{
			stBltData.pSrcSurface = _gcuCreatePreAllocBuffer( pGCUContext, pSrcImg->iStep[0], pSrcImg->iHeight / 2, GCU_FORMAT_UYVY,
			                                                  GCU_TRUE, pSrcImg->pBuffer[0], pSrcImg->iPhyAddr[0] == 0 ? GCU_FALSE : GCU_TRUE, 
			                                                  pSrcImg->iPhyAddr[0] );
			stSrcRect.left   = pROI->iLeft;
			stSrcRect.top    = pROI->iTop / 2;
			stSrcRect.right  = pROI->iWidth;
			stSrcRect.bottom = pROI->iHeight / 2;
		}
		else
		{
			stBltData.pSrcSurface = _gcuCreatePreAllocBuffer( pGCUContext, pSrcImg->iStep[0] >> 1, pSrcImg->iHeight, GCU_FORMAT_UYVY,
			                                                  GCU_TRUE, pSrcImg->pBuffer[0], pSrcImg->iPhyAddr[0] == 0 ? GCU_FALSE : GCU_TRUE, 
			                                                  pSrcImg->iPhyAddr[0] );
			stSrcRect.left     = pROI->iLeft;
			stSrcRect.top      = pROI->iTop;
			stSrcRect.right    = pROI->iWidth;
			stSrcRect.bottom   = pROI->iHeight;
		}
		stBltData.pSrcRect = &stSrcRect;

		if ( CAM_IMGFMT_BGR565 == pDstImg->eFormat )
		{
			stBltData.pDstSurface = _gcuCreatePreAllocBuffer( pGCUContext, pDstImg->iStep[0] >> 1, pDstImg->iHeight, GCU_FORMAT_RGB565,
			                                                  GCU_TRUE, pDstImg->pBuffer[0], pDstImg->iPhyAddr[0] == 0 ? GCU_FALSE : GCU_TRUE, 
			                                                  pDstImg->iPhyAddr[0] );
		}
		else if ( CAM_IMGFMT_BGRA8888 == pDstImg->eFormat )
		{
			stBltData.pDstSurface = _gcuCreatePreAllocBuffer( pGCUContext, pDstImg->iStep[0] >> 2, pDstImg->iHeight, GCU_FORMAT_ARGB8888,
			                                                  GCU_TRUE, pDstImg->pBuffer[0], pDstImg->iPhyAddr[0] == 0 ? GCU_FALSE : GCU_TRUE, 
			                                                  pDstImg->iPhyAddr[0] );
		}

		stDstRect.left   = 0;
		stDstRect.top    = 0;
		stDstRect.right  = pDstImg->iWidth;
		stDstRect.bottom = pDstImg->iHeight;
		stBltData.pDstRect = &stDstRect;

		switch ( eRotate )
		{
		case CAM_FLIPROTATE_NORMAL:
			stBltData.rotation = GCU_ROTATION_0;
			break;

		case CAM_FLIPROTATE_90L:
			stBltData.rotation = GCU_ROTATION_90;
			break;

		case CAM_FLIPROTATE_90R:
			stBltData.rotation = GCU_ROTATION_270;
			break;

		case CAM_FLIPROTATE_180:
			stBltData.rotation = GCU_ROTATION_180;
			break;

		default:
			TRACE( CAM_ERROR, "Error: unsupported rotate[%d]( %s, %s, %d )\n", eRotate, __FILE__, __FUNCTION__, __LINE__ );
			ASSERT( 0 );
			break;
		}
		/**********************************************************************************************************
		 * normal quality is two steps solution ( 1. horizontal blit + vertical filter blit; 2. rotate )
		 * high quality is three steps solution ( 1. horizontal filter blit; 2. vertical filter blit; 3. rotate )
		 * We suggest always use the normal quality for performance reason
		***********************************************************************************************************/
		// gcuSet( pGCUContext, GCU_QUALITY, GCU_QUALITY_HIGH );

		gcuBlit( pGCUContext, &stBltData );
		gcuFinish( pGCUContext );

		pDstImg->iFilledLen[0] = pDstImg->iHeight * pDstImg->iStep[0];
		pDstImg->iFilledLen[1] = 0;
		pDstImg->iFilledLen[2] = 0;

		error = CAM_ERROR_NONE;
	}
	else
	{
		TRACE( CAM_ERROR, "Error: GCU resize/rotate requires 32 bytes align in starting address/row size( %s, %s, %d )\n", 
		       __FILE__, __FUNCTION__, __LINE__ );
		
		pDstImg->iFilledLen[0] = 0;
		pDstImg->iFilledLen[1] = 0;
		pDstImg->iFilledLen[2] = 0;

		error = CAM_ERROR_NOTSUPPORTEDARG;
	}
#else
	TRACE( CAM_ERROR, "Error: Pls open macro BUILD_OPTION_STRATEGY_ENABLE_CSC_CBYCRY_RGB to enable converter to BGR565/BGRA8888( %s, %d )\n",
 	       __FUNCTION__, __LINE__ );
	error = CAM_ERROR_NOTSUPPORTEDARG;
#endif

	return error;
}

static CAM_Error _ipp_yuv_csc_rszrot_roi( CAM_ImageBuffer *pSrcImg, CAM_ImageBuffer *pDstImg, CAM_Rect *pROI, CAM_FlipRotate eRotate, CAM_Bool bIsInPlace )
{
	CAM_Error error = CAM_ERROR_NONE;

	// from yuv420 planar
	if ( CAM_IMGFMT_YCbCr420P == pSrcImg->eFormat )
	{
		if ( CAM_IMGFMT_YCbCr420P == pDstImg->eFormat )
		{
			ASSERT( bIsInPlace == CAM_FALSE );
			error = _ipp_yuv420p_rszrot_roi( pSrcImg, pDstImg, pROI, eRotate );
		}
		else if ( CAM_IMGFMT_YCbCr420SP == pDstImg->eFormat || CAM_IMGFMT_YCrCb420SP == pDstImg->eFormat )
		{
			error = _ipp_yuv420p2sp_rszrot_roi_swap( pSrcImg, pDstImg, pROI, eRotate, bIsInPlace );
		}
		else if ( CAM_IMGFMT_CbYCrY == pDstImg->eFormat )
		{
			ASSERT( bIsInPlace == CAM_FALSE );
			error = _ipp_yuv420p2uyvy_rszrot_roi( pSrcImg, pDstImg, pROI, eRotate );
		}
		else
		{
			TRACE( CAM_ERROR, "Error: IPP primitives don't support: src format = %d -> dst format = %d ( %s, %d )\n", \
			       pSrcImg->eFormat, pDstImg->eFormat, __FILE__, __LINE__ );
			error = CAM_ERROR_NOTSUPPORTEDCMD;
		}
	}
	// from yuv422 planar
	else if ( CAM_IMGFMT_YCbCr422P == pSrcImg->eFormat )
	{
		if ( CAM_IMGFMT_YCbCr422P == pDstImg->eFormat )
		{
			ASSERT( bIsInPlace == CAM_FALSE );
			error = _ipp_yuv422p_rszrot_roi( pSrcImg, pDstImg, pROI, eRotate );
		}
		else
		{
			TRACE( CAM_ERROR, "Error: IPP primitives don't support: src format = %d -> dst format = %d ( %s, %d )\n", \
			       pSrcImg->eFormat, pDstImg->eFormat, __FILE__, __LINE__ );
			error = CAM_ERROR_NOTSUPPORTEDCMD;
		}
	}
	// from uyvy
	else if ( CAM_IMGFMT_CbYCrY == pSrcImg->eFormat )
	{
		if ( CAM_IMGFMT_CbYCrY == pDstImg->eFormat )
		{
			ASSERT( bIsInPlace == CAM_FALSE );
			error = _ipp_uyvy_rszrot_roi( pSrcImg, pDstImg, pROI, eRotate );
		}
		else
		{
			TRACE( CAM_ERROR, "Error: IPP primitives don't support: src format = %d -> dst format = %d ( %s, %d )\n", \
			       pSrcImg->eFormat, pDstImg->eFormat, __FILE__, __LINE__ );
			error = CAM_ERROR_NOTSUPPORTEDCMD;
		}
	}
	else
	{
		TRACE( CAM_ERROR, "Error: IPP primitives don't support: src format = %d -> dst format = %d ( %s, %d )\n", \
		       pSrcImg->eFormat, pDstImg->eFormat, __FILE__, __LINE__ );
		error = CAM_ERROR_NOTSUPPORTEDCMD;
	}

	return error;

}

static CAM_Error _ipp_jpegdec_rszrot_roi( CAM_ImageBuffer *pSrcImg, CAM_ImageBuffer *pDstImg, CAM_Rect *pROI, CAM_FlipRotate eRotate )
{
	CAM_Error error = CAM_ERROR_NONE;

	ASSERT( pSrcImg->pBuffer[0] != NULL );

#if 0
	if ( pSrcImg->iFlag & BUF_FLAG_L1NONCACHEABLE )
	{
		TRACE( CAM_WARN, "Warning: perform software jpeg snapshot generating on non-cacheable source buffer results in low performance!\n" );
	}
#endif

	if ( pDstImg->eFormat != CAM_IMGFMT_CbYCrY && pDstImg->eFormat != CAM_IMGFMT_YCbCr420SP &&
	     pDstImg->eFormat != CAM_IMGFMT_YCrCb420SP && eRotate == CAM_FLIPROTATE_NORMAL )
	{
		error = _ipp_jpeg_roidec( pSrcImg, pDstImg, pROI );
	}
	else
	{
		CAM_ImageBuffer     stTmpImg;
		CAM_Rect            stROIAfterZoom;
		CAM_Int8u           *pHeap          = NULL;

		memset( &stTmpImg, 0, sizeof( CAM_ImageBuffer ) );

		pHeap = ( CAM_Int8u* )malloc( pDstImg->iHeight * pDstImg->iWidth * 2 + 128 );
		if ( pHeap == NULL )
		{
			TRACE( CAM_ERROR, "Error: no enough memory afford ipp image post processing ( %s, %s, %d )\n", __FILE__, __FUNCTION__, __LINE__ );
			return CAM_ERROR_OUTOFMEMORY;
		}

		stTmpImg.iWidth  = pDstImg->iWidth;
		stTmpImg.iHeight = pDstImg->iHeight;

#if 0
		CELOG( "Info:\
		        the pROI->iWidth:%d, pROI->iHeight:%d, pDstImg->iWidth:%d, pDstImg->iHeight:%d, \n\
		        stTmpImg.iWidth:%d, stTmpImg.iHeight:%d \n",
		        pROI->iWidth, pROI->iHeight, pDstImg->iWidth, pDstImg->iHeight,
		        stTmpImg.iWidth, stTmpImg.iHeight );
#endif

		stTmpImg.eFormat  = CAM_IMGFMT_YCbCr420P;
		stTmpImg.iStep[0] = _ALIGN_TO( stTmpImg.iWidth, 8 );
		stTmpImg.iStep[1] = _ALIGN_TO( stTmpImg.iWidth >> 1, 8 );
		stTmpImg.iStep[2] = _ALIGN_TO( stTmpImg.iWidth >> 1, 8 );

		stTmpImg.pBuffer[0] = (CAM_Int8u*)_ALIGN_TO( pHeap, 8 );
		stTmpImg.pBuffer[1] = stTmpImg.pBuffer[0] + stTmpImg.iStep[0] * stTmpImg.iHeight;
		stTmpImg.pBuffer[2] = stTmpImg.pBuffer[1] + stTmpImg.iStep[1] * ( stTmpImg.iHeight >> 1 );

		stTmpImg.iFilledLen[0] = stTmpImg.iStep[0] * stTmpImg.iHeight;
		stTmpImg.iFilledLen[1] = stTmpImg.iStep[1] * ( stTmpImg.iHeight >> 1 );
		stTmpImg.iFilledLen[2] = stTmpImg.iStep[2] * ( stTmpImg.iHeight >> 1 );

		// 1. firset decoder to a same size YCbCr420P with dst image.
		error = _ipp_jpeg_roidec( pSrcImg, &stTmpImg, pROI );
		if ( error != CAM_ERROR_NONE )
		{
			free( pHeap );
			pHeap = NULL;

			return error;
		}

		stROIAfterZoom.iLeft   = 0;
		stROIAfterZoom.iTop    = 0;
		stROIAfterZoom.iHeight = stTmpImg.iHeight;
		stROIAfterZoom.iWidth  = stTmpImg.iWidth;

		// 2. second to do rotation and color conversion
		error = _ipp_yuv_csc_rszrot_roi( &stTmpImg, pDstImg, &stROIAfterZoom, eRotate, CAM_FALSE );
		free( pHeap );
		pHeap = NULL;
	}

	return error;

}

static CAM_Error _ipp_tojpeg_rszrot_roi( CAM_ImageBuffer *pSrcImg, CAM_ImageBuffer *pDstImg, CAM_Rect *pROI, CAM_FlipRotate eRotate, CAM_JpegParam *pJpegParam )
{
	CAM_Error       error = CAM_ERROR_NONE;
	CAM_ImageBuffer stImgBeforeZoom, stImgAfterZoom;
	CAM_Int8u       *pHeapBeforeZoom = NULL, *pHeapAfterZoom = NULL;
	CAM_Rect        stZoomROI;

	memset( &stImgBeforeZoom, 0, sizeof( CAM_ImageBuffer ) );
	memset( &stImgAfterZoom, 0, sizeof( CAM_ImageBuffer ) );
	
	if ( pSrcImg->eFormat == CAM_IMGFMT_JPEG )
	{
		// JPEG rotation case
		if ( pROI->iHeight == pDstImg->iHeight && pROI->iWidth == pDstImg->iWidth &&
		     pROI->iTop == 0 && pROI->iLeft == 0 )
		{
			ASSERT( eRotate != CAM_FLIPROTATE_NORMAL );
			error = _ipp_jpeg_rotation( pSrcImg, pDstImg, eRotate );
			return error;
		}
		// need use ROI decoder to get raw picture 
		else
		{			
			stImgBeforeZoom.iHeight = pROI->iHeight;
			stImgBeforeZoom.iWidth  = pROI->iWidth;
			
			stImgBeforeZoom.eFormat  = CAM_IMGFMT_YCbCr422P;
			stImgBeforeZoom.iStep[0] = _ALIGN_TO( stImgBeforeZoom.iWidth, 8 );
			stImgBeforeZoom.iStep[1] = _ALIGN_TO( stImgBeforeZoom.iWidth >> 1, 8 );
			stImgBeforeZoom.iStep[2] = _ALIGN_TO( stImgBeforeZoom.iWidth >> 1, 8 );

			pHeapBeforeZoom = ( CAM_Int8u* )malloc( pROI->iHeight * ( stImgBeforeZoom.iStep[0] + stImgBeforeZoom.iStep[1] + stImgBeforeZoom.iStep[2] ) + 128 );
			if ( pHeapBeforeZoom == NULL )
			{	
				TRACE( CAM_ERROR, "Error: no enough memory afford ipp image post processing ( %s, %s, %d )!\n", 
				       __FILE__, __FUNCTION__, __LINE__ );
				return CAM_ERROR_OUTOFMEMORY;
			}

			stImgBeforeZoom.pBuffer[0] = (CAM_Int8u*)_ALIGN_TO( pHeapBeforeZoom, 8 );
			stImgBeforeZoom.pBuffer[1] = stImgBeforeZoom.pBuffer[0] + stImgBeforeZoom.iStep[0] * stImgBeforeZoom.iHeight;
			stImgBeforeZoom.pBuffer[2] = stImgBeforeZoom.pBuffer[1] + stImgBeforeZoom.iStep[1] * stImgBeforeZoom.iHeight;

			error = _ipp_jpeg_roidec( pSrcImg, &stImgBeforeZoom, pROI );
			ASSERT( error == CAM_ERROR_NONE );

			stZoomROI.iTop    = 0;
			stZoomROI.iLeft   = 0;
			stZoomROI.iHeight = pROI->iHeight;
			stZoomROI.iWidth  = pROI->iWidth;
		}
	}
	else
	{
		stImgBeforeZoom = *pSrcImg;
		stZoomROI       = *pROI;
	}

	// resize/rotate
	if ( stZoomROI.iHeight == pDstImg->iHeight && stZoomROI.iWidth == pDstImg->iWidth && 
	     stZoomROI.iTop == 0 && stZoomROI.iLeft == 0 && eRotate == CAM_FLIPROTATE_NORMAL )
	{
		stImgAfterZoom = stImgBeforeZoom;
	}
	else
	{
		stImgAfterZoom.iHeight = pDstImg->iHeight;
		stImgAfterZoom.iWidth  = pDstImg->iWidth;

		stImgAfterZoom.eFormat  = stImgBeforeZoom.eFormat;
		stImgAfterZoom.iStep[0] = _ALIGN_TO( stImgAfterZoom.iWidth, 8 );
		stImgAfterZoom.iStep[1] = _ALIGN_TO( stImgAfterZoom.iWidth >> 1, 8 );
		stImgAfterZoom.iStep[2] = _ALIGN_TO( stImgAfterZoom.iWidth >> 1, 8 );

		pHeapAfterZoom = ( CAM_Int8u* )malloc( pDstImg->iHeight * ( stImgAfterZoom.iStep[0] + stImgAfterZoom.iStep[1] + stImgAfterZoom.iStep[2] ) + 128 );
		if ( pHeapAfterZoom == NULL )
		{	
			TRACE( CAM_ERROR, "Error: no enough memory afford ipp image post processing ( %s, %s, %d )!\n", __FILE__, __FUNCTION__, __LINE__ );
			return CAM_ERROR_OUTOFMEMORY;
		}

		stImgAfterZoom.pBuffer[0] = (CAM_Int8u*)_ALIGN_TO( pHeapAfterZoom, 8 );
		stImgAfterZoom.pBuffer[1] = stImgAfterZoom.pBuffer[0] + stImgAfterZoom.iStep[0] * stImgAfterZoom.iHeight;
		stImgAfterZoom.pBuffer[2] = stImgAfterZoom.pBuffer[1] + stImgAfterZoom.iStep[1] * stImgAfterZoom.iHeight;

		error = _ipp_yuv_csc_rszrot_roi( &stImgBeforeZoom, &stImgAfterZoom, &stZoomROI, eRotate, CAM_FALSE );
		ASSERT( error == CAM_ERROR_NONE );
	}
	
	// JPEG encoder
	error = _ipp_yuv2jpeg( &stImgAfterZoom, pDstImg, pJpegParam );
	
	if ( pHeapBeforeZoom != NULL )
	{
		free( pHeapBeforeZoom );
		pHeapBeforeZoom = NULL;
	}
	if ( pHeapAfterZoom != NULL )
	{
		free( pHeapAfterZoom );
		pHeapAfterZoom = NULL;
	}

	return error;
}

static CAM_Error _ipp_yuv420p_rszrot_roi( CAM_ImageBuffer *pSrcImg, CAM_ImageBuffer *pDstImg, CAM_Rect *pROI, CAM_FlipRotate eRotate )
{
	CAM_Error error       = CAM_ERROR_NONE;
	CAM_Int8u *pSrcBuf[3] = { NULL, NULL, NULL };
#if 0
	if ( pSrcImg->iFlag & BUF_FLAG_L1NONCACHEABLE )
	{
		TRACE( CAM_WARN, "Warning: perform software resize/memcpy on non-cacheable source buffer results in low performance!\n" );
	}
#endif

	pSrcBuf[0] = (CAM_Int8u*)( (CAM_Int32s)pSrcImg->pBuffer[0] + pROI->iTop * pSrcImg->iStep[0] + pROI->iLeft );
	pSrcBuf[1] = (CAM_Int8u*)( (CAM_Int32s)pSrcImg->pBuffer[1] + ( pROI->iTop >> 1 ) * pSrcImg->iStep[1] + ( pROI->iLeft >> 1 ) );
	pSrcBuf[2] = (CAM_Int8u*)( (CAM_Int32s)pSrcImg->pBuffer[2] + ( pROI->iTop >> 1 ) * pSrcImg->iStep[2] + ( pROI->iLeft >> 1 ) );

	if ( pROI->iWidth == pDstImg->iWidth && pROI->iHeight == pDstImg->iHeight && eRotate == CAM_FLIPROTATE_NORMAL )
	{
		TRACE( CAM_WARN, "Warning: redundant memory copy( %s, %s, %d )\n", __FILE__, __FUNCTION__, __LINE__ );

		// just crop here
		_ppu_copy_image( pSrcImg->pBuffer[0], pSrcImg->iStep[0], pDstImg->pBuffer[0], pDstImg->iStep[0], pROI->iWidth, pROI->iHeight );
		_ppu_copy_image( pSrcImg->pBuffer[1], pSrcImg->iStep[1], pDstImg->pBuffer[1], pDstImg->iStep[1], pROI->iWidth >> 1, pROI->iHeight >> 1 );
		_ppu_copy_image( pSrcImg->pBuffer[2], pSrcImg->iStep[2], pDstImg->pBuffer[2], pDstImg->iStep[2], pROI->iWidth >> 1, pROI->iHeight >> 1 );

		error = CAM_ERROR_NONE;
	}
	else if ( !( pSrcImg->iStep[0] & 7 ) && !( pSrcImg->iStep[1] & 7 ) && !( pSrcImg->iStep[2] & 7 ) &&
	          !( pDstImg->iStep[0] & 3 ) && !( pDstImg->iStep[1] & 1 ) && !( pDstImg->iStep[2] & 1 )
			 )
	{
		IppStatus         eIppRet;
		IppiSize          stSrcSize, stDstSize;
		IppCameraRotation eIppRotate;
		CAM_Int32s        rcpRatioxQ16, rcpRatioyQ16;

		IppCameraInterpolation eInterpolation = ippCameraInterpBilinear;

		switch ( eRotate )
		{
		case CAM_FLIPROTATE_NORMAL:
			eIppRotate = ippCameraRotateDisable;
			stDstSize.width  = pDstImg->iWidth;
			stDstSize.height = pDstImg->iHeight; 
			break;

		case CAM_FLIPROTATE_90L:
			eIppRotate = ippCameraRotate90L;
			stDstSize.width  = pDstImg->iHeight;
			stDstSize.height = pDstImg->iWidth; 
			break;

		case CAM_FLIPROTATE_90R:
			eIppRotate = ippCameraRotate90R;
			stDstSize.width  = pDstImg->iHeight;
			stDstSize.height = pDstImg->iWidth;
			break;

		case CAM_FLIPROTATE_180:
			eIppRotate = ippCameraRotate180;
			stDstSize.width  = pDstImg->iWidth;
			stDstSize.height = pDstImg->iHeight; 
			break;

		case CAM_FLIPROTATE_HFLIP:
			eIppRotate = ippCameraFlipHorizontal; 
			stDstSize.width  = pDstImg->iWidth;
			stDstSize.height = pDstImg->iHeight; 
			break;

		case CAM_FLIPROTATE_VFLIP:
			eIppRotate = ippCameraFlipVertical; 
			stDstSize.width  = pDstImg->iWidth;
			stDstSize.height = pDstImg->iHeight; 
			break;

		default:
			TRACE( CAM_ERROR, "Error: unsupported rotate[%d]( %s, %s, %d )\n", eRotate, __FILE__, __FUNCTION__, __LINE__ );
			ASSERT( 0 );
			break;
		}

		stSrcSize.width  = pROI->iWidth;
		stSrcSize.height = pROI->iHeight;

		rcpRatioxQ16 = ( ( ( stSrcSize.width & ~1 ) - 1 ) << 16 ) / ( ( stDstSize.width & ~1 ) - 1 );
		rcpRatioyQ16 = ( ( ( stSrcSize.height & ~1 ) - 1 ) << 16 ) / ( ( stDstSize.height & ~1 ) - 1 );


		/*
		TRACE( CAM_INFO, "SrcStep = {%d, %d, %d}, srcSize = (%d x %d),\nDstStep = {%d, %d, %d}, dstSize = (%d x %d),\nrotation = %d, \
		       scale = (%d, %d)\n",
		       pSrcImg->iStep[0], pSrcImg->iStep[1], pSrcImg->iStep[2],
		       stSrcSize.width, stSrcSize.height,
		       pDstImg->iStep[0], pDstImg->iStep[1], pDstImg->iStep[2],
		       stDstSize.width, stDstSize.height,
		       eIppRotate, rcpRatioxQ16, rcpRatioyQ16 );
		*/

		eIppRet = ippiYCbCr420RszRot_8u_P3R( (const Ipp8u**)pSrcBuf, pSrcImg->iStep, stSrcSize,
		                                      pDstImg->pBuffer, pDstImg->iStep, stDstSize,
		                                      eInterpolation, eIppRotate, rcpRatioxQ16, rcpRatioyQ16 );

		ASSERT( eIppRet == ippStsNoErr );

		error = CAM_ERROR_NONE;
	}
	else
	{
		TRACE( CAM_ERROR, "Error: Marvell IPP yuv420planar primitive requires buffer address/step 8 bytes align( %s, %s, %d )", 
		       __FILE__, __FUNCTION__, __LINE__ );
		error = CAM_ERROR_NOTSUPPORTEDARG;
	}

	if ( error == CAM_ERROR_NONE )
	{
		pDstImg->iFilledLen[0] = pDstImg->iStep[0] * pDstImg->iHeight;
		pDstImg->iFilledLen[1] = pDstImg->iStep[1] * ( pDstImg->iHeight >> 1 );
		pDstImg->iFilledLen[2] = pDstImg->iStep[2] * ( pDstImg->iHeight >> 1 );
	}
	else
	{
		pDstImg->iFilledLen[0] = 0;
		pDstImg->iFilledLen[1] = 0;
		pDstImg->iFilledLen[2] = 0;
	}

	return error;

}

static CAM_Error _ipp_yuv422p_rszrot_roi( CAM_ImageBuffer *pSrcImg, CAM_ImageBuffer *pDstImg, CAM_Rect *pROI, CAM_FlipRotate eRotate )
{
	CAM_Error error       = CAM_ERROR_NONE;
	CAM_Int8u *pSrcBuf[3] = { NULL, NULL, NULL };

	pSrcBuf[0] = (CAM_Int8u*)( (CAM_Int32s)pSrcImg->pBuffer[0] + pROI->iTop * pSrcImg->iStep[0] + pROI->iLeft );
	pSrcBuf[1] = (CAM_Int8u*)( (CAM_Int32s)pSrcImg->pBuffer[1] + pROI->iTop * pSrcImg->iStep[1] + ( pROI->iLeft >> 1 ) );
	pSrcBuf[2] = (CAM_Int8u*)( (CAM_Int32s)pSrcImg->pBuffer[2] + pROI->iTop * pSrcImg->iStep[2] + ( pROI->iLeft >> 1 ) );
#if 0
	if ( pSrcImg->iFlag & BUF_FLAG_L1NONCACHEABLE )
	{
		TRACE( CAM_WARN, "Warning: perform software resize/memcpy on non-cacheable source buffer results in low performance!\n" );
	}
#endif
	if ( pROI->iWidth == pDstImg->iWidth && pROI->iHeight == pDstImg->iHeight && eRotate == CAM_FLIPROTATE_NORMAL )
	{
		TRACE( CAM_WARN, "Warning: redundant memory copy( %s, %s, %d )\n", __FILE__, __FUNCTION__, __LINE__ );

		// just crop here
		_ppu_copy_image( pSrcImg->pBuffer[0], pSrcImg->iStep[0], pDstImg->pBuffer[0], pDstImg->iStep[0], pROI->iWidth, pROI->iHeight );
		_ppu_copy_image( pSrcImg->pBuffer[1], pSrcImg->iStep[1], pDstImg->pBuffer[1], pDstImg->iStep[1], pROI->iWidth >> 1, pROI->iHeight );
		_ppu_copy_image( pSrcImg->pBuffer[2], pSrcImg->iStep[2], pDstImg->pBuffer[2], pDstImg->iStep[2], pROI->iWidth >> 1, pROI->iHeight );

		error = CAM_ERROR_NONE;
	}
	else if ( !( pSrcImg->iStep[0] & 7 ) && !( pSrcImg->iStep[1] & 7 ) && !( pSrcImg->iStep[2] & 7 ) &&
	          !( pDstImg->iStep[0] & 7 ) && !( pDstImg->iStep[1] & 7 ) && !( pDstImg->iStep[2] & 7 ) )
	{
		IppStatus              eIppRet;
		IppCameraRotation      eIppRotate;
		IppiSize               stSrcSize, stDstSize;
		CAM_Int32s             rcpRatioxQ16, rcpRatioyQ16;
		IppCameraInterpolation eInterpolation = ippCameraInterpBilinear;

		switch ( eRotate )
		{
		case CAM_FLIPROTATE_NORMAL:
			eIppRotate = ippCameraRotateDisable;
			stDstSize.width  = pDstImg->iWidth;
			stDstSize.height = pDstImg->iHeight; 
			break;

		case CAM_FLIPROTATE_90L:
			eIppRotate = ippCameraRotate90L;
			stDstSize.width  = pDstImg->iHeight;
			stDstSize.height = pDstImg->iWidth; 
			break;

		case CAM_FLIPROTATE_90R:
			eIppRotate = ippCameraRotate90R;
			stDstSize.width  = pDstImg->iHeight;
			stDstSize.height = pDstImg->iWidth;
			break;

		case CAM_FLIPROTATE_180:
			eIppRotate = ippCameraRotate180;
			stDstSize.width  = pDstImg->iWidth;
			stDstSize.height = pDstImg->iHeight; 
			break;

		case CAM_FLIPROTATE_HFLIP:
			eIppRotate = ippCameraFlipHorizontal; 
			stDstSize.width  = pDstImg->iWidth;
			stDstSize.height = pDstImg->iHeight; 
			break;

		case CAM_FLIPROTATE_VFLIP:
			eIppRotate = ippCameraFlipVertical; 
			stDstSize.width  = pDstImg->iWidth;
			stDstSize.height = pDstImg->iHeight; 
			break;

		default:
			TRACE( CAM_ERROR, "Error: unsupported rotate[%d]( %s, %s, %d )\n", eRotate, __FILE__, __FUNCTION__, __LINE__ );
			ASSERT( 0 );
			break;
		}

		stSrcSize.width  = pROI->iWidth;
		stSrcSize.height = pROI->iHeight;

		rcpRatioxQ16 = ( ( ( stSrcSize.width & ~1 ) - 1 ) << 16 ) / ( ( stDstSize.width & ~1 ) - 1 );
		rcpRatioyQ16 = ( ( ( stSrcSize.height & ~1 ) - 1 ) << 16 ) / ( ( stDstSize.height & ~1 ) - 1 );

		eIppRet = ippiYCbCr422RszRot_8u_P3R( (const Ipp8u**)pSrcBuf, pSrcImg->iStep, stSrcSize,
		                                      pDstImg->pBuffer, pDstImg->iStep, stDstSize,
		                                      eInterpolation, eIppRotate, rcpRatioxQ16, rcpRatioyQ16 );
		ASSERT( eIppRet == ippStsNoErr );

		error = CAM_ERROR_NONE;
	}
	else
	{
		TRACE( CAM_ERROR, "Error: Marvell IPP yuv422planar primitive requires address/step 8 bytes align, only 90L/90R are supported and no resize\
		       ( %s, %s, %d )", __FILE__, __FUNCTION__, __LINE__  );
		error = CAM_ERROR_NOTSUPPORTEDARG;
	}

	if ( error == CAM_ERROR_NONE )
	{
		pDstImg->iFilledLen[0] = pDstImg->iStep[0] * pDstImg->iHeight;
		pDstImg->iFilledLen[1] = pDstImg->iStep[1] * pDstImg->iHeight;
		pDstImg->iFilledLen[2] = pDstImg->iStep[1] * pDstImg->iHeight;
	}
	else
	{
		pDstImg->iFilledLen[0] = 0;
		pDstImg->iFilledLen[1] = 0;
		pDstImg->iFilledLen[2] = 0;
	}

	return error;
}

static CAM_Error _ipp_uyvy_rszrot_roi( CAM_ImageBuffer *pSrcImg, CAM_ImageBuffer *pDstImg, CAM_Rect *pROI, CAM_FlipRotate eRotate )
{
	CAM_Error  error          = CAM_ERROR_NONE;
	CAM_Int8u  *pSrcBuf       = NULL;

	pDstImg->iFilledLen[0] = pDstImg->iStep[0] * pDstImg->iHeight;
	pDstImg->iFilledLen[1] = 0;
	pDstImg->iFilledLen[2] = 0;
#if 0
	if ( pSrcImg->iFlag & BUF_FLAG_L1NONCACHEABLE )
	{
		TRACE( CAM_WARN, "Warning: perform software resize/memcpy on non-cacheable source buffer will result in low performance!\n" );
	}
#endif
	pSrcBuf = (CAM_Int8u*)( (CAM_Int32s)pSrcImg->pBuffer[0] + pROI->iTop * pSrcImg->iStep[0] + pROI->iLeft );

	if ( pROI->iWidth == pDstImg->iWidth && pROI->iHeight == pDstImg->iHeight && eRotate == CAM_FLIPROTATE_NORMAL )
	{
		TRACE( CAM_WARN, "Warning: redundant memory copy( %s, %s, %d )\n", __FILE__, __FUNCTION__, __LINE__ );

		// just crop here
		_ppu_copy_image( pSrcImg->pBuffer[0], pSrcImg->iStep[0], pDstImg->pBuffer[0], pDstImg->iStep[0], pROI->iWidth << 2, pROI->iHeight );

		error = CAM_ERROR_NONE;
	}
	else if ( ( pSrcImg->iStep[0] & 7 ) == 0 && ( pDstImg->iStep[0] & 7 ) == 0 &&
	          ( eRotate == CAM_FLIPROTATE_90L || eRotate == CAM_FLIPROTATE_90R ) &&
	          ( pROI->iWidth == pDstImg->iHeight && pROI->iHeight == pDstImg->iWidth )
	         )
	{
		IppStatus         eIppRet;
		IppCameraRotation eIppRotation = ( eRotate == CAM_FLIPROTATE_90L ) ? ippCameraRotate90L : ippCameraRotate90R;
		IppiSize          stROISize    = { pROI->iWidth, pROI->iHeight };

		eIppRet = ippiYCbCr422Rotate_8u_C2R( pSrcBuf, pSrcImg->iStep[0],
		                                     pDstImg->pBuffer[0], pDstImg->iStep[0],
		                                     stROISize, eIppRotation );
		ASSERT( eIppRet == ippStsNoErr );

		error = CAM_ERROR_NONE;
	}
	else if ( ( pSrcImg->iStep[0] & 7 ) == 0 && ( pDstImg->iStep[0] & 7 ) == 0 &&
	          eRotate == CAM_FLIPROTATE_NORMAL )
	{
		IppStatus              eIppRet;
		IppiSize               stSrcSize, stDstSize;
		CAM_Int32s             rcpRatioxQ16, rcpRatioyQ16;
		IppCameraInterpolation eInterpolation = ippCameraInterpBilinear;
		
		stSrcSize.width  = pROI->iWidth;
		stSrcSize.height = pROI->iHeight;
		
		stDstSize.width  = pDstImg->iWidth;
		stDstSize.height = pDstImg->iHeight; 

		rcpRatioxQ16 = ( ( ( stSrcSize.width & ~1 ) - 1 ) << 16 ) / ( ( stDstSize.width & ~1 ) - 1 );
		rcpRatioyQ16 = ( ( ( stSrcSize.height & ~1 ) - 1 ) << 16 ) / ( ( stDstSize.height & ~1 ) - 1 );

		eIppRet = ippiYCbCr422RszRot_8u_C2R_C ( (const Ipp8u*) pSrcBuf, pSrcImg->iStep[0], 
                                                stSrcSize, pDstImg->pBuffer[0], pDstImg->iStep[0], stDstSize, 
                                                eInterpolation, ippCameraRotateDisable,
                                                rcpRatioxQ16, rcpRatioyQ16 );
											   
		ASSERT( eIppRet == ippStsNoErr );

		error = CAM_ERROR_NONE;
	}
	else
	{
		TRACE( CAM_ERROR, "Error: Marvell IPP uyvy primitive requires address/step 8 bytes align, only 90L/90R or resize are supported\
		       ( %s, %s, %d )\n", __FILE__, __FUNCTION__, __LINE__  );
		error = CAM_ERROR_NOTSUPPORTEDARG;
	}

	return CAM_ERROR_NONE;

}

static CAM_Error _ipp_yuv420p2sp_rszrot_roi_swap( CAM_ImageBuffer *pSrcImg, CAM_ImageBuffer *pDstImg, CAM_Rect *pROI, CAM_FlipRotate eRotate, CAM_Bool bIsInPlace )
{
	CAM_Error  error   = CAM_ERROR_NONE;  
	IppStatus  eIppRet;
	CAM_Int8u  *pSrcBuf[3], *pDstBuf[3]; 
	CAM_Int32s pSrcStep[3], pDstStep[3];
	IppiSize   stROISize;
	CAM_Int8u  *pSrcHeap = NULL, *pDstHeap = NULL;
#if 0
	if ( pSrcImg->iFlag & BUF_FLAG_L1NONCACHEABLE )
	{
		TRACE( CAM_WARN, "Warning: perform software resize/memcpy on non-cacheable source buffer results in low performance!\n" );
	}
#endif
	if ( pROI->iWidth == pDstImg->iWidth && pROI->iHeight == pDstImg->iHeight && eRotate == CAM_FLIPROTATE_NORMAL )
	{
		pSrcBuf[0]  = pSrcImg->pBuffer[0] + pROI->iTop * pSrcImg->iStep[0] + pROI->iLeft;
		pSrcStep[0] = pSrcImg->iStep[0];

		if ( pDstImg->eFormat == CAM_IMGFMT_YCbCr420SP )
		{
			pSrcBuf[1]  = pSrcImg->pBuffer[1] + ( pROI->iTop >> 1 ) * pSrcImg->iStep[1] + ( pROI->iLeft >> 1 );
			pSrcBuf[2]  = pSrcImg->pBuffer[2] + ( pROI->iTop >> 1 ) * pSrcImg->iStep[2] + ( pROI->iLeft >> 1 );

			pSrcStep[1] = pSrcImg->iStep[1];
			pSrcStep[2] = pSrcImg->iStep[2];
		}
		else
		{
			// sawp u/v plane
			pSrcBuf[1]  = pSrcImg->pBuffer[2] + ( pROI->iTop >> 1 ) * pSrcImg->iStep[2] + ( pROI->iLeft >> 1 );
			pSrcBuf[2]  = pSrcImg->pBuffer[1] + ( pROI->iTop >> 1 ) * pSrcImg->iStep[1] + ( pROI->iLeft >> 1 );

			pSrcStep[1] = pSrcImg->iStep[2];
			pSrcStep[2] = pSrcImg->iStep[1];
		}

		if ( bIsInPlace == CAM_FALSE )
		{
			pDstBuf[0]  = pDstImg->pBuffer[0];
			pDstBuf[1]  = pDstImg->pBuffer[1];
			pDstStep[0] = pDstImg->iStep[0];
			pDstStep[1] = pDstImg->iStep[1];
		}
		else
		{
			pDstHeap = ( CAM_Int8u* )malloc( pDstImg->iStep[1] * ( pDstImg->iHeight >> 1 ) + 8 );
			if ( pDstHeap == NULL )
			{	
				TRACE( CAM_ERROR, "Error: no enough memory to do 420P->420SP conversion ( %s, %s, %d )\n", __FILE__, __FUNCTION__, __LINE__ );
				return CAM_ERROR_OUTOFMEMORY;
			}
			pDstBuf[0]  = pDstImg->pBuffer[0];
			pDstBuf[1]  = (CAM_Int8u*)_ALIGN_TO( pDstHeap, 8 );		
			pDstStep[0] = pDstImg->iStep[0];
			pDstStep[1] = pDstImg->iStep[1];
		}
	}
	else
	{
		IppStatus              eRetCode;
		IppiSize               stSrcSize, stDstSize;
		IppCameraRotation      eIppRotate;
		CAM_Int32s             rcpRatioxQ16, rcpRatioyQ16;
		CAM_Int8u              *pBufBeforeZoom[3], *pBufAfterZoom[3];
		CAM_Int32s             pStepAfterZoom[3];
		IppCameraInterpolation eInterpolation = ippCameraInterpBilinear;
		
		if ( !( pSrcImg->iStep[0] & 7 ) && !( pSrcImg->iStep[1] & 7 ) && !( pSrcImg->iStep[2] & 7 ) )
		{
			pBufBeforeZoom[0] = pSrcImg->pBuffer[0] + pROI->iTop * pSrcImg->iStep[0] + pROI->iLeft;
			pBufBeforeZoom[1] = pSrcImg->pBuffer[1] + ( pROI->iTop >> 1 ) * pSrcImg->iStep[1] + ( pROI->iLeft >> 1 );
			pBufBeforeZoom[2] = pSrcImg->pBuffer[2] + ( pROI->iTop >> 1 ) * pSrcImg->iStep[2] + ( pROI->iLeft >> 1 );

			pSrcHeap = ( CAM_Int8u* )malloc( pDstImg->iWidth * pDstImg->iHeight * 3 / 2 + 24 );
			if ( pSrcHeap == NULL )
			{	
				TRACE( CAM_ERROR, "Error: no enough memory afford ipp image post processing ( %s, %s, %d )\n", __FILE__, __FUNCTION__, __LINE__ );
				return CAM_ERROR_OUTOFMEMORY;
			}

			pStepAfterZoom[0] = _ALIGN_TO( pDstImg->iWidth, 8 );
			pStepAfterZoom[1] = _ALIGN_TO( pDstImg->iWidth >> 1, 8 );
			pStepAfterZoom[2] = _ALIGN_TO( pDstImg->iWidth >> 1, 8 );

			pBufAfterZoom[0] = (CAM_Int8u*)_ALIGN_TO( pSrcHeap, 8 );
			pBufAfterZoom[1] = pBufAfterZoom[0] + pStepAfterZoom[0] * pDstImg->iHeight;
			pBufAfterZoom[2] = pBufAfterZoom[1] + pStepAfterZoom[1] * ( pDstImg->iHeight >> 1 );

			switch ( eRotate )
			{
			case CAM_FLIPROTATE_NORMAL:
				eIppRotate = ippCameraRotateDisable;
				stDstSize.width  = pDstImg->iWidth;
				stDstSize.height = pDstImg->iHeight; 
				break;

			case CAM_FLIPROTATE_90L:
				eIppRotate = ippCameraRotate90L;
				stDstSize.width  = pDstImg->iHeight;
				stDstSize.height = pDstImg->iWidth; 
				break;

			case CAM_FLIPROTATE_90R:
				eIppRotate = ippCameraRotate90R;
				stDstSize.width  = pDstImg->iHeight;
				stDstSize.height = pDstImg->iWidth;
				break;

			case CAM_FLIPROTATE_180:
				eIppRotate = ippCameraRotate180;
				stDstSize.width  = pDstImg->iWidth;
				stDstSize.height = pDstImg->iHeight; 
				break;

			case CAM_FLIPROTATE_HFLIP:
				eIppRotate = ippCameraFlipHorizontal; 
				stDstSize.width  = pDstImg->iWidth;
				stDstSize.height = pDstImg->iHeight; 
				break;

			case CAM_FLIPROTATE_VFLIP:
				eIppRotate = ippCameraFlipVertical; 
				stDstSize.width  = pDstImg->iWidth;
				stDstSize.height = pDstImg->iHeight; 
				break;

			default:
				TRACE( CAM_ERROR, "Error: unsupported rotate[%d]( %s, %s, %d )\n", eRotate, __FILE__, __FUNCTION__, __LINE__ );
				ASSERT( 0 );
				break;
			}

			stSrcSize.width  = pROI->iWidth;
			stSrcSize.height = pROI->iHeight;

			rcpRatioxQ16 = ( ( ( stSrcSize.width & ~1 ) - 1 ) << 16 ) / ( ( stDstSize.width & ~1 ) - 1 );
			rcpRatioyQ16 = ( ( ( stSrcSize.height & ~1 ) - 1 ) << 16 ) / ( ( stDstSize.height & ~1 ) - 1 );

#if 0
			TRACE( CAM_INFO, "SrcStep = {%d, %d, %d}, srcSize = (%d x %d),\n DstStep = {%d, %d, %d}, dstSize = (%d x %d),\nrotation = %d, scale = (%d, %d)\n",
			       pSrcImg->iStep[0], pSrcImg->iStep[1], pSrcImg->iStep[2],
			       stSrcSize.width, stSrcSize.height,
			       pDstImg->iStep[0], pDstImg->iStep[1], pDstImg->iStep[2],
			       stDstSize.width, stDstSize.height,
			       eIppRotate, rcpRatioxQ16, rcpRatioyQ16 );
#endif			

			eIppRet = ippiYCbCr420RszRot_8u_P3R( (const Ipp8u**)pBufBeforeZoom, pSrcImg->iStep, stSrcSize,
			                                      pBufAfterZoom, pStepAfterZoom, stDstSize,
			                                      eInterpolation, eIppRotate, rcpRatioxQ16, rcpRatioyQ16 );

			ASSERT( eIppRet == ippStsNoErr );

			pSrcBuf[0] = pBufAfterZoom[0];
			pSrcStep[0] = pStepAfterZoom[0];

			if ( pDstImg->eFormat == CAM_IMGFMT_YCbCr420SP )
			{
				pSrcBuf[1]  = pBufAfterZoom[1];
				pSrcBuf[2]  = pBufAfterZoom[2];
				pSrcStep[1] = pStepAfterZoom[1];
				pSrcStep[2] = pStepAfterZoom[2];
			}
			else
			{
				// sawp u/v plane
				pSrcBuf[1]  = pBufAfterZoom[2];
				pSrcBuf[2]  = pBufAfterZoom[1];
				pSrcStep[1] = pStepAfterZoom[2];
				pSrcStep[2] = pStepAfterZoom[1];
			}

			pDstBuf[0]  = pDstImg->pBuffer[0];
			pDstBuf[1]  = pDstImg->pBuffer[1];
			pDstStep[0] = pDstImg->iStep[0];
			pDstStep[1] = pDstImg->iStep[1];
		}
		else
		{
			TRACE( CAM_ERROR, "Error: Marvell IPP yuv420 planar primitive requires address/step 8 bytes align\
			       ( %s, %s, %d )\n", __FILE__, __FUNCTION__, __LINE__  );
			return CAM_ERROR_NOTSUPPORTEDARG;
		}
	}

	stROISize.height = pDstImg->iHeight;
	stROISize.width  = pDstImg->iWidth;

	eIppRet = ippiYCbCr420PToYCbCr420SP_8u_P3P2R( (const Ipp8u**)pSrcBuf, pSrcStep, pDstBuf, pDstStep, stROISize );
	if ( pSrcHeap != NULL )
	{
		free( pSrcHeap );
		pSrcHeap = NULL;
	}
	if ( pDstHeap != NULL )
	{
		memmove( pDstImg->pBuffer[1], pDstBuf[1], pDstImg->iStep[1] * ( pDstImg->iHeight >> 1 ) );
		free( pDstHeap );
		pDstHeap = NULL;
	}
	ASSERT( eIppRet == ippStsNoErr );

	pDstImg->iFilledLen[0] = pDstImg->iStep[0] * pDstImg->iHeight;
	pDstImg->iFilledLen[1] = pDstImg->iStep[1] * ( pDstImg->iHeight >> 1 );
	pDstImg->iFilledLen[2] = 0; 

	return CAM_ERROR_NONE;

}

static CAM_Error _ipp_yuv420p2uyvy_rszrot_roi( CAM_ImageBuffer *pSrcImg, CAM_ImageBuffer *pDstImg, CAM_Rect *pROI, CAM_FlipRotate eRotate )
{
	CAM_Error  error   = CAM_ERROR_NONE;  
	IppStatus  eIppRet = ippStsNoErr;
	CAM_Int8u  *pBufAfterZoom[3]; 
	CAM_Int32s pStepAfterZoom[3];
	IppiSize   stROISize;
	CAM_Int8u  *pHeap = NULL;
#if 0
	if ( pSrcImg->iFlag & BUF_FLAG_L1NONCACHEABLE )
	{
		TRACE( CAM_WARN, "Warning: perform software resize/memcpy on non-cacheable source buffer results in low performance!\n" );
	}
#endif
	if ( pROI->iWidth == pDstImg->iWidth && pROI->iHeight == pDstImg->iHeight && eRotate == CAM_FLIPROTATE_NORMAL )
	{
		pBufAfterZoom[0] = pSrcImg->pBuffer[0] + pROI->iTop * pSrcImg->iStep[0] + pROI->iLeft ;
		pBufAfterZoom[1] = pSrcImg->pBuffer[1] + ( pROI->iTop >> 1 ) * pSrcImg->iStep[1] + ( pROI->iLeft >> 1 );
		pBufAfterZoom[2] = pSrcImg->pBuffer[2] + ( pROI->iTop >> 1 ) * pSrcImg->iStep[2] + ( pROI->iLeft >> 1 );

		pStepAfterZoom[0] = pSrcImg->iStep[0];
		pStepAfterZoom[1] = pSrcImg->iStep[1];
		pStepAfterZoom[2] = pSrcImg->iStep[2];
	}
	else
	{
		IppiSize          stSrcSize, stDstSize;
		IppCameraRotation eIppRotate;
		CAM_Int32s        rcpRatioxQ16, rcpRatioyQ16;
		CAM_Int8u         *pBufBeforeZoom[3];
		
		IppCameraInterpolation eInterpolation = ippCameraInterpBilinear;

		// NOTE: IPP primitives require step 8 byte align, if not, the result will be UNEXPECTED!!!
		if ( !( pSrcImg->iStep[0] & 7 ) && !( pSrcImg->iStep[1] & 7 ) && !( pSrcImg->iStep[2] & 7 ) )
		{
			pBufBeforeZoom[0] = pSrcImg->pBuffer[0] + pROI->iTop * pSrcImg->iStep[0] + pROI->iLeft;
			pBufBeforeZoom[1] = pSrcImg->pBuffer[1] + ( pROI->iTop >> 1 ) * pSrcImg->iStep[1] + ( pROI->iLeft >> 1 );
			pBufBeforeZoom[2] = pSrcImg->pBuffer[2] + ( pROI->iTop >> 1 ) * pSrcImg->iStep[2] + ( pROI->iLeft >> 1 );

			pHeap = ( CAM_Int8u* )malloc( pDstImg->iWidth * pDstImg->iHeight * 3 / 2 + 8 );
			if ( pHeap == NULL )
			{	
				TRACE( CAM_ERROR, "Error: no enough memory afford ipp image post processing ( %s, %s, %d )\n", __FILE__, __FUNCTION__, __LINE__ );
				return CAM_ERROR_OUTOFMEMORY;
			}

			pStepAfterZoom[0] = _ALIGN_TO( pDstImg->iWidth, 8 );
			pStepAfterZoom[1] = _ALIGN_TO( pDstImg->iWidth >> 1, 8 );
			pStepAfterZoom[2] = _ALIGN_TO( pDstImg->iWidth >> 1, 8 );

			pBufAfterZoom[0] = (CAM_Int8u*)_ALIGN_TO( pHeap, 8 );
			pBufAfterZoom[1] = pBufAfterZoom[0] + pStepAfterZoom[0] * pDstImg->iHeight;
			pBufAfterZoom[2] = pBufAfterZoom[1] + pStepAfterZoom[1] * ( pDstImg->iHeight >> 1 );

			switch ( eRotate )
			{
			case CAM_FLIPROTATE_NORMAL:
				eIppRotate       = ippCameraRotateDisable;
				stDstSize.width  = pDstImg->iWidth;
				stDstSize.height = pDstImg->iHeight; 
				break;

			case CAM_FLIPROTATE_90L:
				eIppRotate       = ippCameraRotate90L;
				stDstSize.width  = pDstImg->iHeight;
				stDstSize.height = pDstImg->iWidth; 
				break;

			case CAM_FLIPROTATE_90R:
				eIppRotate       = ippCameraRotate90R;
				stDstSize.width  = pDstImg->iHeight;
				stDstSize.height = pDstImg->iWidth;
				break;

			case CAM_FLIPROTATE_180:
				eIppRotate       = ippCameraRotate180;
				stDstSize.width  = pDstImg->iWidth;
				stDstSize.height = pDstImg->iHeight; 
				break;

			case CAM_FLIPROTATE_HFLIP:
				eIppRotate       = ippCameraFlipHorizontal; 
				stDstSize.width  = pDstImg->iWidth;
				stDstSize.height = pDstImg->iHeight; 
				break;

			case CAM_FLIPROTATE_VFLIP:
				eIppRotate       = ippCameraFlipVertical; 
				stDstSize.width  = pDstImg->iWidth;
				stDstSize.height = pDstImg->iHeight; 
				break;

			default:
				TRACE( CAM_ERROR, "Error: unsupported rotate[%d]( %s, %s, %d )\n", eRotate, __FILE__, __FUNCTION__, __LINE__ );
				ASSERT( 0 );
				break;
			}

			stSrcSize.width  = pROI->iWidth;
			stSrcSize.height = pROI->iHeight;

			rcpRatioxQ16 = ( ( ( stSrcSize.width & ~1 ) - 1 ) << 16 ) / ( ( stDstSize.width & ~1 ) - 1 );
			rcpRatioyQ16 = ( ( ( stSrcSize.height & ~1 ) - 1 ) << 16 ) / ( ( stDstSize.height & ~1 ) - 1 );


			/*
			TRACE( CAM_INFO, "SrcStep = {%d, %d, %d}, srcSize = (%d x %d),\nDstStep = {%d, %d, %d}, dstSize = (%d x %d),\nrotation = %d, scale = (%d, %d)\n",
			       pSrcImg->iStep[0], pSrcImg->iStep[1], pSrcImg->iStep[2],
			       stSrcSize.width, stSrcSize.height,
			       pDstImg->iStep[0], pDstImg->iStep[1], pDstImg->iStep[2],
			       stDstSize.width, stDstSize.height,
			       eIppRotate, rcpRatioxQ16, rcpRatioyQ16 );
			*/

			eIppRet = ippiYCbCr420RszRot_8u_P3R( (const Ipp8u**)pBufBeforeZoom, pSrcImg->iStep, stSrcSize,
			                                      pBufAfterZoom, pStepAfterZoom, stDstSize,
			                                      eInterpolation, eIppRotate, rcpRatioxQ16, rcpRatioyQ16 );

			ASSERT( eIppRet == ippStsNoErr );

		}
		else
		{
			TRACE( CAM_ERROR, "Error: Marvell IPP yuv422 planar primitive requires address/step 8 bytes align\
			       ( %s, %s, %d )\n", __FILE__, __FUNCTION__, __LINE__  );
			return CAM_ERROR_NOTSUPPORTEDARG;
		}
	}

	stROISize.height = pDstImg->iHeight;
	stROISize.width  = pDstImg->iWidth;

	ippiYCbCr420ToCbYCrY_8u_P3C2R_C( (Ipp8u**)pBufAfterZoom, pStepAfterZoom, stROISize, pDstImg->pBuffer, pDstImg->iStep, stROISize );

	if ( pHeap != NULL )
	{
		free( pHeap );
		pHeap = NULL;
	}
	ASSERT( eIppRet == ippStsNoErr );

	pDstImg->iFilledLen[0] = pDstImg->iStep[0] * pDstImg->iHeight;
	pDstImg->iFilledLen[1] = pDstImg->iStep[1] * ( pDstImg->iHeight >> 1 );
	pDstImg->iFilledLen[2] = 0; 

	return CAM_ERROR_NONE;

}


static CAM_Error _ipp_jpeg_rotation( CAM_ImageBuffer *pSrcImg, CAM_ImageBuffer *pDstImg, CAM_FlipRotate eRotate )
{
#if 0
	void                     *pDecoderState  = NULL;
	MiscGeneralCallbackTable *pCallBackTable = NULL;
	IppJPEGDecoderParam      stDecoderPar;
	IppBitstream             stSrcBitStream;  
	IppBitstream             stDstBitStream; 
	IppPicture               stDstPicture;	
	IppCodecStatus           eRetCode;
	CAM_Int8u                cImgEnd[2]      = { 0 };

	memset( &stDecoderPar, 0, sizeof( IppJPEGDecoderParam ) );
	memset( &stDstPicture, 0, sizeof( IppPicture ) );

	pDstImg->iFilledLen[0] = 0;
	pDstImg->iFilledLen[1] = 0;
	pDstImg->iFilledLen[2] = 0;

	stDecoderPar.nModeFlag = IPP_JPEG_ROTATE;

	switch( eRotate )
	{
	case CAM_FLIPROTATE_90L:
		stDecoderPar.UnionParamJPEG.rotParam.RotMode = IPP_JP_90L;
		break;

	case CAM_FLIPROTATE_90R:
		stDecoderPar.UnionParamJPEG.rotParam.RotMode = IPP_JP_90R;
		break;

	case CAM_FLIPROTATE_180:
		stDecoderPar.UnionParamJPEG.rotParam.RotMode = IPP_JP_180;
		break;

	default:
		TRACE( CAM_ERROR, "Error: do not support JPEG rotation other than 90/180/270( %s, %s, %d )\n", __FILE__, __FUNCTION__, __LINE__  );
		ASSERT( 0 );
		break;
	}

	stDecoderPar.UnionParamJPEG.rotParam.pStreamHandler = NULL;

	// init callback table
	if ( miscInitGeneralCallbackTable( &pCallBackTable ) != 0 )
	{
		TRACE( CAM_ERROR, "Error: init JPEG rotation failed( %s, %d )!\n", __FUNCTION__, __LINE__ );
		return CAM_ERROR_PPUFAILED; 
	}

	stSrcBitStream.pBsBuffer      = pSrcImg->pBuffer[0];
	stSrcBitStream.bsByteLen      = pSrcImg->iFilledLen[0];
	stSrcBitStream.pBsCurByte     = pSrcImg->pBuffer[0];
	stSrcBitStream.bsCurBitOffset = 0;

	/* XXX: why I do this? Oh, it's for robustness consideration, if a JPEG file
	 *      is damaged, maybe we need not frustrated, some decoders also can decode it
	 *      although imcomplete, but "half a bread is better than none", you know.IPP JPEG
	 *      decoder can decode a damaged JPEG at its best, but the presumption is that
	 *      you should give it a 0xFFD9 to remind it, the file is finished.
	 */
	cImgEnd[0] = pSrcImg->pBuffer[0][stSrcBitStream.bsByteLen - 2];
	cImgEnd[1] = pSrcImg->pBuffer[0][stSrcBitStream.bsByteLen - 1];
	pSrcImg->pBuffer[0][stSrcBitStream.bsByteLen - 1] = 0xD9;
	pSrcImg->pBuffer[0][stSrcBitStream.bsByteLen - 2] = 0xFF;

	eRetCode = DecoderInitAlloc_JPEG( &stSrcBitStream, &stDstPicture, pCallBackTable, &pDecoderState );

	if ( IPP_STATUS_NOERR != eRetCode )
	{
#if defined( ANDROID )
		char sFileName[256] = "/data/badjpeg_ppu1.dat";
#else
		char sFileName[256] = "badjpeg_ppu1.dat";
#endif
		PPU_DUMP_FRAME( sFileName, pSrcImg );

		TRACE( CAM_ERROR, "Error: init JPEG decoder failed, the input JPEG data dumped to: %s( %s, %d )!\n", sFileName, __FUNCTION__, __LINE__ );
		miscFreeGeneralCallbackTable( &pCallBackTable );

		return CAM_ERROR_PPUFAILED;
	}

	stDstBitStream.pBsBuffer      = stDstBitStream.pBsCurByte = pDstImg->pBuffer[0];
	stDstBitStream.bsByteLen      = pDstImg->iAllocLen[0];
	stDstBitStream.bsCurBitOffset = 0;

	// call the core JPEG decoder function
	eRetCode = Decode_JPEG( ( void* )&stSrcBitStream, &stDstBitStream, NULL, &stDecoderPar, pDecoderState );
	if ( IPP_STATUS_JPEG_EOF != eRetCode && IPP_STATUS_INPUT_ERR != eRetCode )
	{
		TRACE( CAM_ERROR, "Error: JPEG rotation failed, error code[%d]( %s, %d )!\n", eRetCode, __FUNCTION__, __LINE__ );

		DecoderFree_JPEG( &pDecoderState );
		miscFreeGeneralCallbackTable( &pCallBackTable );
		pSrcImg->pBuffer[0][stSrcBitStream.bsByteLen - 1] = cImgEnd[1];
		pSrcImg->pBuffer[0][stSrcBitStream.bsByteLen - 2] = cImgEnd[0];

		return CAM_ERROR_PPUFAILED;
	}
	if ( IPP_STATUS_INPUT_ERR == eRetCode )
	{
		TRACE( CAM_WARN, "Warning: Input JPEG file is damaged, Camera Enigne/IPP lib will do our best to handle it, \
		       but also pls check the data source( %s, %d )\n", __FUNCTION__, __LINE__ );
	}

	DecoderFree_JPEG( &pDecoderState );
	miscFreeGeneralCallbackTable( &pCallBackTable );
	pSrcImg->pBuffer[0][stSrcBitStream.bsByteLen - 1] = cImgEnd[1];
	pSrcImg->pBuffer[0][stSrcBitStream.bsByteLen - 2] = cImgEnd[0];

	pDstImg->iFilledLen[0] = ( int )( stDstBitStream.pBsCurByte - stDstBitStream.pBsBuffer );
	pDstImg->iFilledLen[1] = 0;
	pDstImg->iFilledLen[2] = 0;
	return CAM_ERROR_NONE;
#else
	TRACE( CAM_ERROR, "cannot support JPEG rotate in this platform( %s, %d )\n", __FILE__, __LINE__ );
	return CAM_ERROR_NOTSUPPORTEDARG;
#endif
	  
}

static CAM_Error _ipp_jpeg_roidec( CAM_ImageBuffer *pSrcImg, CAM_ImageBuffer *pDstImg, CAM_Rect *pROI )
{
	CAM_Error                error           = CAM_ERROR_NONE;
	MiscGeneralCallbackTable *pCallBackTable = NULL;
	void                     *pDecoderState  = NULL;
	IppJPEGDecoderParam      stDecoderPar;
	IppPicture               stDstPicture;
	IppBitstream             stSrcBitStream;  
	IppCodecStatus           eRetCode;
	CAM_Int8u                cImgEnd[2]      = { 0 };

	memset( &stDecoderPar, 0, sizeof( IppJPEGDecoderParam ) );
	memset( &stDstPicture, 0, sizeof( IppPicture ) );

	ASSERT( pSrcImg->pBuffer[0] != NULL );
#if 0
	if ( pSrcImg->iFlag & BUF_FLAG_L1NONCACHEABLE )
	{
		TRACE( CAM_WARN, "Warning: perform software jpeg snapshot generating on non-cacheable source buffer results in low performance!\n" );
	}
#endif
	// init callback table
	if ( miscInitGeneralCallbackTable( &pCallBackTable ) != 0 )
	{
		TRACE( CAM_ERROR, "Error: init JPEG decoder failed( %s, %d )!\n", __FUNCTION__, __LINE__ );
		return CAM_ERROR_PPUFAILED; 
	}

	// no need to read image from file
	pCallBackTable->fFileRead     = NULL;
	stSrcBitStream.pBsCurByte     = stSrcBitStream.pBsBuffer = pSrcImg->pBuffer[0];
	stSrcBitStream.bsByteLen      = pSrcImg->iFilledLen[0];
	stSrcBitStream.bsCurBitOffset = 0;

	/* XXX: why I do this? Oh, it's for robustness consideration, if a JPEG file
	 *      is damaged, maybe we need not frustrated, some decoders also can decode it
	 *      although imcomplete, but "half a bread is better than none", you know.IPP JPEG
	 *      decoder can decode a damaged JPEG at its best, but the presumption is that
	 *      you should give it a 0xFFD9 to remind it, the file is finished.
	 */
	cImgEnd[0] = pSrcImg->pBuffer[0][stSrcBitStream.bsByteLen - 2];
	cImgEnd[1] = pSrcImg->pBuffer[0][stSrcBitStream.bsByteLen - 1];
	pSrcImg->pBuffer[0][stSrcBitStream.bsByteLen - 1] = 0xD9;
	pSrcImg->pBuffer[0][stSrcBitStream.bsByteLen - 2] = 0xFF;

#if 0
	CELOG( "stSrcBitStream.pBsCurByte: %p\n stSrcBitStream.pBsBuffer: %p\n\
	        stSrcBitStream.bsByteLen: %d\n, stSrcBitStream.bsCurBitOffset :%d\n",
	        stSrcBitStream.pBsCurByte, stSrcBitStream.pBsBuffer, stSrcBitStream.bsByteLen,
	        stSrcBitStream.bsCurBitOffset );
#endif

	eRetCode = DecoderInitAlloc_JPEG( &stSrcBitStream, &stDstPicture, pCallBackTable, &pDecoderState, (void *)NULL );
	if ( IPP_STATUS_NOERR != eRetCode )
	{
#if defined( ANDROID )
		char sFileName[256] = "/data/badjpeg_ppu2.dat";
#else
		char sFileName[256] = "badjpeg_ppu2.dat";
#endif
		PPU_DUMP_FRAME( sFileName, pSrcImg );

		TRACE( CAM_ERROR, "Error: init JPEG decoder failed, the input JPEG data dumped to: %s( %s, %d )!\n", sFileName, __FUNCTION__, __LINE__ );
		miscFreeGeneralCallbackTable( &pCallBackTable );
		return CAM_ERROR_PPUFAILED;
	}

	switch ( pDstImg->eFormat )
	{
	case CAM_IMGFMT_YCbCr420P:
		pDstImg->iFilledLen[0] = pDstImg->iStep[0] * pDstImg->iHeight;
		pDstImg->iFilledLen[1] = pDstImg->iStep[1] * ( pDstImg->iHeight >> 1);
		pDstImg->iFilledLen[2] = pDstImg->iStep[2] * ( pDstImg->iHeight >> 1);
		stDstPicture.picPlaneNum = 3;
		stDecoderPar.nDesiredColor = JPEG_YUV411;
		break;

	case CAM_IMGFMT_YCbCr422P:
		pDstImg->iFilledLen[0] = pDstImg->iStep[0] * pDstImg->iHeight;
		pDstImg->iFilledLen[1] = pDstImg->iStep[1] * pDstImg->iHeight;
		pDstImg->iFilledLen[2] = pDstImg->iStep[2] * pDstImg->iHeight;
		stDstPicture.picPlaneNum = 3;
		stDecoderPar.nDesiredColor = JPEG_YUV422;
		break;

	case CAM_IMGFMT_YCbCr444P:
		pDstImg->iFilledLen[0] = pDstImg->iStep[0] * pDstImg->iHeight;
		pDstImg->iFilledLen[1] = pDstImg->iStep[1] * pDstImg->iHeight;
		pDstImg->iFilledLen[2] = pDstImg->iStep[2] * pDstImg->iHeight;
		stDstPicture.picPlaneNum = 3;
		stDecoderPar.nDesiredColor = JPEG_YUV444;
		break;

	case CAM_IMGFMT_RGB565:
		pDstImg->iFilledLen[0] = pDstImg->iStep[0] * pDstImg->iHeight;
		pDstImg->iFilledLen[1] = 0;
		pDstImg->iFilledLen[2] = 0;
		stDstPicture.picPlaneNum = 1;
		stDecoderPar.nDesiredColor = JPEG_RGB565;
		break;

	case CAM_IMGFMT_BGR565:
		pDstImg->iFilledLen[0] = pDstImg->iStep[0] * pDstImg->iHeight;
		pDstImg->iFilledLen[1] = 0;
		pDstImg->iFilledLen[2] = 0;
		stDstPicture.picPlaneNum = 1;
		stDecoderPar.nDesiredColor = JPEG_BGR565;
		break;

	default:
		TRACE( CAM_ERROR, "Error: the given format[%d] is not supported by JPEG ROI decoder!\n", pDstImg->eFormat );
		ASSERT( 0 );
		break;
	}

	stDstPicture.picWidth  =  pDstImg->iWidth;
	stDstPicture.picHeight =  pDstImg->iHeight;

	stDstPicture.picPlaneStep[0] = pDstImg->iStep[0];
	stDstPicture.picPlaneStep[1] = pDstImg->iStep[1];
	stDstPicture.picPlaneStep[2] = pDstImg->iStep[2];

	stDstPicture.ppPicPlane[0]   = pDstImg->pBuffer[0];
	stDstPicture.ppPicPlane[1]   = pDstImg->pBuffer[1];
	stDstPicture.ppPicPlane[2]   = pDstImg->pBuffer[2];

	stDecoderPar.srcROI.x        = pROI->iLeft;
	stDecoderPar.srcROI.y        = pROI->iTop;
	stDecoderPar.srcROI.width    = pROI->iWidth;
	stDecoderPar.srcROI.height   = pROI->iHeight;

	stDecoderPar.nScale = 0; // decoder will self calc the Scale factor

#if 0
	CELOG( "Info:\
	        stDstPicture.picWidth=%d,\n\
	        stDstPicture.picHeight=%d,\n\
	        stDstPicture.picPlaneStep[0]=%d,\n\
	        stDstPicture.picPlaneStep[1]=%d,\n\
	        stDstPicture.picPlaneStep[2]=%d,\n\
	        stDstPicture.ppPicPlane[0]=%p,\n\
	        stDstPicture.ppPicPlane[1]=%p,\n\
	        stDstPicture.ppPicPlane[2]=%p,\n\
	        stDecoderPar.nDesiredColor=%d,\n\
	        stDecoderPar.srcROI.width=%d,\n\
	        stDecoderPar.srcROI.height=%d,\n\
	        stDecoderPar.srcROI.x=%d,\n\
	        stDecoderPar.srcROI.y=%d,\n\
	        stDecoderPar.nScale=%d\n",
	        stDstPicture.picWidth,
	        stDstPicture.picHeight,
	        stDstPicture.picPlaneStep[0],
	        stDstPicture.picPlaneStep[1],
	        stDstPicture.picPlaneStep[2],
	        stDstPicture.ppPicPlane[0],
	        stDstPicture.ppPicPlane[1],
	        stDstPicture.ppPicPlane[2],
	        stDecoderPar.nDesiredColor,
	        stDecoderPar.srcROI.width,
	        stDecoderPar.srcROI.height,
	        stDecoderPar.srcROI.x,
	        stDecoderPar.srcROI.y,
	        stDecoderPar.nScale );
#endif

	// call the core JPEG decoder function
	eRetCode = Decode_JPEG( &stDstPicture, &stDecoderPar, pDecoderState );
	if ( IPP_STATUS_NOERR != eRetCode )
	{
		TRACE( CAM_ERROR, "Error: JPEG_ROIDec Failed, error code[%d]( %s, %d )!\n", eRetCode, __FUNCTION__, __LINE__ );
		error = CAM_ERROR_PPUFAILED;
	}
	else
	{
		error = CAM_ERROR_NONE;
	}

	DecoderFree_JPEG( &pDecoderState );
	miscFreeGeneralCallbackTable( &pCallBackTable );
	pSrcImg->pBuffer[0][stSrcBitStream.bsByteLen - 1] = cImgEnd[1];
	pSrcImg->pBuffer[0][stSrcBitStream.bsByteLen - 2] = cImgEnd[0];

	if ( error != CAM_ERROR_NONE )
	{
		pDstImg->iFilledLen[0] = 0;
		pDstImg->iFilledLen[1] = 0;
		pDstImg->iFilledLen[2] = 0;
	}

	return error;
}

static CAM_Error _ipp_yuv2jpeg( CAM_ImageBuffer *pSrcImg, CAM_ImageBuffer *pDstImg, CAM_JpegParam *pJpegParam )
{
	IppPicture                  stSrcPic;
	IppBitstream                stDstStream;
	IppJPEGEncoderParam         stEncoderPar;
	IppCodecStatus              eRetCode;
	void                        *pEncoderState  = NULL;
	MiscGeneralCallbackTable    *pCallBackTable = NULL;

	CAM_Error                   error           = CAM_ERROR_NONE;

	if ( pDstImg->iWidth != pSrcImg->iWidth || pDstImg->iHeight != pSrcImg->iHeight )
	{
		TRACE( CAM_ERROR, "Error: JPEG encoder don't support input size and output size differ( %s, %d )\n", __FILE__, __LINE__ );
		return CAM_ERROR_NOTSUPPORTEDARG;
	}

	stSrcPic.picWidth            = pSrcImg->iWidth;
	stSrcPic.picHeight           = pSrcImg->iHeight;
	stSrcPic.picFormat           = ( pSrcImg->eFormat == CAM_IMGFMT_YCbCr420P ? JPEG_YUV411 : JPEG_YUV422 );
	stSrcPic.picChannelNum       = 3;
	stSrcPic.picPlaneNum         = 3;
	stSrcPic.picPlaneStep[0]     = pSrcImg->iStep[0];
	stSrcPic.picPlaneStep[1]     = pSrcImg->iStep[1];
	stSrcPic.picPlaneStep[2]     = pSrcImg->iStep[2];
	stSrcPic.ppPicPlane[0]       = pSrcImg->pBuffer[0];
	stSrcPic.ppPicPlane[1]       = pSrcImg->pBuffer[1];
	stSrcPic.ppPicPlane[2]       = pSrcImg->pBuffer[2];
	stSrcPic.picROI.x            = 0;
	stSrcPic.picROI.y            = 0;
	stSrcPic.picROI.width        = pSrcImg->iWidth;
	stSrcPic.picROI.height       = pSrcImg->iHeight;

	stDstStream.pBsBuffer        = ( Ipp8u* )pDstImg->pBuffer[0];
	stDstStream.bsByteLen        = pDstImg->iAllocLen[0];
	stDstStream.pBsCurByte       = ( Ipp8u* )pDstImg->pBuffer[0];
	stDstStream.bsCurBitOffset   = 0;

	stEncoderPar.nQuality          = pJpegParam->iQualityFactor;
	stEncoderPar.nRestartInterval  = 0;
	stEncoderPar.nJPEGMode         = JPEG_BASELINE;
	stEncoderPar.nSubsampling      = ( stSrcPic.picFormat == JPEG_YUV411 ? JPEG_411 : JPEG_422 );
	stEncoderPar.nBufType          = JPEG_INTEGRATEBUF;
	stEncoderPar.pSrcPicHandler    = (void*)NULL;
	stEncoderPar.pStreamHandler    = (void*)NULL;

#if 0
	TRACE( CAM_INFO, "stEncoderPar.nBufType = %d\n stEncoderPar.nJPEGMode = %d\n stEncoderPar.nQuality = %d\n \
	       stEncoderPar.nRestartInterval = %d\n stEncoderPar.nSubsampling = %d\n \
	       stEncoderPar.pSrcPicHandler = %p\n stEncoderPar.pStreamHandler = %p\n \
	       stSrcPic.picWidth = %d\n stSrcPic.picHeight = %d\n stSrcPic.picFormat = %d\n \
	       stSrcPic.picChannelNum = %d\n, stSrcPic.picPlaneNum = %d\n stSrcPic.picPlaneStep[0] = %d\n \
	       stSrcPic.picPlaneStep[1] = %d\n stSrcPic.picPlaneStep[2] = %d\n stSrcPic.ppPicPlane[0] = %p\n \
	       stSrcPic.ppPicPlane[1] = %p\n stSrcPic.ppPicPlane[2] = %p\n stSrcPic.picROI.x = %d\n \
	       stSrcPic.picROI.y = %d\n stSrcPic.picROI.width = %d\n stSrcPic.picROI.height = %d\n \
	       stSrcPic.picStatus = %d\n stSrcPic.picTimeStamp = %d\n stSrcPic.picOrderCnt = %d\n ",\
	       stEncoderPar.nBufType, stEncoderPar.nJPEGMode, stEncoderPar.nQuality, \
	       stEncoderPar.nRestartInterval, stEncoderPar.nSubsampling, stEncoderPar.pSrcPicHandler, \
	       stEncoderPar.pStreamHandler, srcPic.picWidth, srcPic.picHeight, srcPic.picFormat, \
	       stSrcPic.picChannelNum, stSrcPic.picPlaneNum, stSrcPic.picPlaneStep[0], stSrcPic.picPlaneStep[1], \
	       stSrcPic.picPlaneStep[2], stSrcPic.ppPicPlane[0], stSrcPic.ppPicPlane[1], stSrcPic.ppPicPlane[2], \
	       stSrcPic.picROI.x, stSrcPic.picROI.y, stSrcPic.picROI.width, stSrcPic.picROI.height, \
	       stSrcPic.picStatus, (CAM_Int32s)stSrcPic.picTimeStamp, stSrcPic.picOrderCnt );
#endif

	if ( miscInitGeneralCallbackTable( &pCallBackTable ) != 0 ) 
	{
		return CAM_ERROR_OUTOFMEMORY;
	}

	eRetCode = EncoderInitAlloc_JPEG( &stEncoderPar, &stSrcPic, &stDstStream, pCallBackTable, &pEncoderState );

	if ( IPP_STATUS_NOERR != eRetCode )
	{
		return CAM_ERROR_BADARGUMENT;
	}

	eRetCode = Encode_JPEG( &stSrcPic, &stDstStream, pEncoderState );
	if ( IPP_STATUS_NOERR != eRetCode ) 
	{
		error = CAM_ERROR_PPUFAILED;
	}


	EncoderFree_JPEG( &pEncoderState );
	miscFreeGeneralCallbackTable( &pCallBackTable );
	if ( error == CAM_ERROR_NONE )
	{
		pDstImg->iFilledLen[0] = (CAM_Int32s)stDstStream.pBsCurByte - (CAM_Int32s)stDstStream.pBsBuffer;
		pDstImg->iFilledLen[1] = 0;
		pDstImg->iFilledLen[2] = 0;
	}

	return error;

}

static IppStatus ippiYCbCr420ToCbYCrY_8u_P3C2R_C( Ipp8u *pSrc[3], int *pSrcStep, IppiSize stSrcSize, Ipp8u *pDst[3], int *pDstStep, IppiSize stDstSize )
{
		int i, j;
		Ipp8u *pSrcYOdd = NULL, *pSrcYEven = NULL, *pSrcU = NULL, *pSrcV = NULL, *pDstOdd = NULL, *pDstEven = NULL;


		ASSERT( ( stSrcSize.width == stDstSize.width ) && ( stSrcSize.height == stDstSize.height ) );

		pSrcYOdd  = pSrc[0];
		pSrcYEven = pSrc[0] + pSrcStep[0];
		pSrcU     = pSrc[1];
		pSrcV     = pSrc[2];
		pDstOdd   = pDst[0];
		pDstEven  = pDst[0] + pDstStep[0];
		for ( i = 0; i < stSrcSize.height; i += 2 )
		{
			for ( j = 0; j < stSrcSize.width; j += 2 )
			{
				*pDstOdd++ = *pSrcU;
				*pDstOdd++ = *pSrcYOdd++;
				*pDstOdd++ = *pSrcV;
				*pDstOdd++ = *pSrcYOdd++;

				*pDstEven++ = *pSrcU++;
				*pDstEven++ = *pSrcYEven++;
				*pDstEven++ = *pSrcV++;
				*pDstEven++ = *pSrcYEven++;
			}

			pSrcYOdd += ( pSrcStep[0] << 1 )  - stSrcSize.width;
			pSrcYEven += ( pSrcStep[0] << 1 ) - stSrcSize.width;

			pSrcU += pSrcStep[1] - ( stSrcSize.width >> 1 );
			pSrcV += pSrcStep[2] - ( stSrcSize.width >> 1 );

			pDstOdd  += ( pDstStep[0] << 1 ) - ( stDstSize.width << 1 );
			pDstEven += ( pDstStep[0] << 1 ) - ( stDstSize.width << 1 );
		}

		return ippStsNoErr;
}

/*********************************************************************************
; Name:             ippiYCbCr422RszRot_8u_C2R_C
; Description:      This function combine resizing, color space conversion and 
;					rotation into one function to reduce memory access
; Input Arguments:  pSrc    	    - A Pointer to source image plane
;                   srcStep         - A 1-element vector containing the 
;                                     distance, in bytes, between the start of 
;                                     lines in each of the input image plane.
;					srcSize			- size of Source image
;                   pDst            - A 1-element vector containing pointers to 
;									  the start of each of the destination plane
;                   dstStep         - A 1-element vector containing the distance,
;									  in bytes, between the start of lines in each
;									  of the output image plane.
;                   dstSize         - size of output image.
;                   interpolation   - type of interpolation
;                                     to perform for resampling of the input image.
;                                     The following are currently supported.
;									  ippCameraInterpNearearest Nearest neighboring
;                                     ippCameraInterpBilinear   Bilinear interpolation.     
;                   rotation        - Rotation control parameter; must be of the
;                                     following pre-defined values:
;                                     ippCameraRotate90L, ippCameraRotate90R,
;                                     ippCameraRotate180, ippCameraRotateDisable
;									  ippCameraFlipHrz or ippiCameraFlipVt
; Output Arguments:pDst     	    - Pointer to the vector to be initialized.  
; Notes:    prototype:
;		    IppStatus ippiYCbCr422RszRot_8u_C2R_C (const Ipp8u *pSrc, 
;						int srcStep, IppiSize srcSize, Ipp8u *pDst, 
;						int dstStep, IppiSize dstSize, IppCameraInterpolation
;						interpolation, 	IppCameraRotation rotation, int rcpRatiox,
;						int rcpRatioy)
;**********************************************************************************/

static IppStatus ippiYCbCr422RszRot_8u_C2R_C ( const Ipp8u *pSrc, int srcStep, 
                                               IppiSize srcSize, Ipp8u *pDst, int dstStep, IppiSize dstSize, 
                                               IppCameraInterpolation interpolation, IppCameraRotation rotation,
                                               int rcpRatiox, int rcpRatioy )
{
	int x, y;
	Ipp32s ay, fx, fx1, ay1, fx2;
	int dstWidth, dstHeight;
	Ipp32s xLimit, yLimit;
	Ipp8u *pSrcY1, *pSrcY2, *pSrcCb1, *pSrcCr1, *pSrcCb2, *pSrcCr2;
	Ipp8u *pDstY, *pDstCb, *pDstCr;
	Ipp8u *pDstY1, *pDstY2, *pDstY3, *pDstY4;
	Ipp8u *pDstCb1, *pDstCb2, *pDstCr1, *pDstCr2;
	int stepX, stepY, stepX1, stepX2, stepY1, stepY2;
	Ipp32s yLT, yRT, yLB, yRB, yT, yB, yR;
	Ipp8u Y1, Y2, Y3, Y4, Cb1, Cb2, Cr1, Cr2;
	int col, dcol, drow;
	int xNum, yNum;

	ASSERT(pSrc && pDst);

	dstWidth = dstSize.width;
	dstHeight = dstSize.height;
	
	xLimit = (srcSize.width-1) << 16;
	yLimit = (srcSize.height-1) << 16;
	xNum = dstSize.width;
	yNum = dstSize.height;

	ay = 0;
	ay1 = rcpRatioy;
	pSrcY1  = (Ipp8u*)(pSrc + 1);
	pSrcY2  = (Ipp8u*)(pSrcY1 + (ay1 >> 16) * srcStep);			
	pSrcCb1 = (Ipp8u*)pSrc;
	pSrcCb2 = (Ipp8u*)(pSrcCb1 + ((ay1 + 32768) >> 16) * srcStep);
	pSrcCr1 = (Ipp8u*)(pSrc + 2);
	pSrcCr2 = (Ipp8u*)(pSrcCr1 + ((ay1 + 32768) >> 16) * srcStep);
	pDstY   = (Ipp8u*)(pDst + 1);
	pDstCb  = (Ipp8u*)pDst;
	pDstCr  = (Ipp8u*)(pDst + 2);

	switch (rotation) {
	case ippCameraRotateDisable:
		pDstY1 = pDstY;
		pDstY2 = pDstY + 2;
		pDstY3 = pDstY + dstStep;
		pDstY4 = pDstY3 + 2;
		stepX  = 4;
		stepX1 = 4;
		stepX2 = 4;
		stepY  = 2 * dstStep - dstSize.width * 2;
		stepY1 = 2 * dstStep - dstSize.width * 2;
		stepY2 = 2 * dstStep - dstSize.width * 2;
		pDstCb1 = pDstCb;
		pDstCb2 = pDstCb + dstStep;
		pDstCr1 = pDstCr;
		pDstCr2 = pDstCr + dstStep;
		break;
	default:
		return ippStsBadArgErr;
	};

	if ( interpolation == ippCameraInterpBilinear ) {
		for(y = 0; y < yNum; y += 2) {
			fx = 0;
			fx1 = rcpRatiox;
			fx2 = 32768;
			for(x = 0; x < xNum; x+= 2) {
				/* 1. Resize for Y */
				col = fx >> 16;
				col = col * 2;
				dcol = fx & 0xffff;
				drow = ay & 0xffff;
				yLT = pSrcY1[col];
				yRT = pSrcY1[col + 2];
				yLB = pSrcY1[col + srcStep];
				yRB = pSrcY1[col + srcStep + 2];

				/*if (fx == xLimit){
					yRT = pSrcY1[col];
					yRB = pSrcY1[col + srcStep[0]];
				}else{
					yRT = pSrcY1[col + 1];
					yRB = pSrcY1[col + srcStep[0] + 1];
				}*/

				yT = ((dcol * (yRT - yLT)) >> 16) + yLT;
				yB = ((dcol * (yRB - yLB)) >> 16) + yLB;
				yR = ((drow * (yB - yT)) >> 16) + yT;
				Y1 = (Ipp8u)yR;
	
				col = fx1 >> 16;
				col = col * 2;
				dcol = fx1 & 0xffff;
				drow = ay & 0xffff;
				yLT = pSrcY1[col];
				//	yRT = pSrcY1[col + 1];
				yLB = pSrcY1[col + srcStep];
				//	yRB = pSrcY1[col + srcStep[0] + 1];

				if (fx1 >= xLimit){
					yRT = pSrcY1[col];
					yRB = pSrcY1[col + srcStep];
				}else{
					yRT = pSrcY1[col + 2];
					yRB = pSrcY1[col + srcStep + 2];
				}/**/

				yT = ((dcol * (yRT - yLT)) >> 16) + yLT;
				yB = ((dcol * (yRB - yLB)) >> 16) + yLB;
				yR = ((drow * (yB - yT)) >> 16) + yT;
				Y2 = (Ipp8u)yR;

				col = fx >> 16;
				col = col * 2;
				dcol = fx & 0xffff;
				drow = ay1 & 0xffff;
				yLT = pSrcY2[col];
				yRT = pSrcY2[col + 2];
				yLB = pSrcY2[col + srcStep];
				yRB = pSrcY2[col + srcStep + 2];

				/*	if (fx == xLimit){
					yRT = pSrcY2[col];
					yRB = pSrcY2[col + srcStep[0]];
				}else{
					yRT = pSrcY2[col + 1];
					yRB = pSrcY2[col + srcStep[0] + 1];
				}*/
				yT = ((dcol * (yRT - yLT)) >> 16) + yLT;
				yB = ((dcol * (yRB - yLB)) >> 16) + yLB;
				yR = ((drow * (yB - yT)) >> 16) + yT;
				Y3 = (Ipp8u)yR;

				col = fx1 >> 16;
				col = col * 2;
				dcol = fx1 & 0xffff;
				drow = ay1 & 0xffff;
				yLT = pSrcY2[col];
				//	yRT = pSrcY2[col + 2];
				yLB = pSrcY2[col + srcStep];
				//	yRB = pSrcY2[col + srcStep + 2];

				if (fx1 >= xLimit){
					yRT = pSrcY2[col];
					yRB = pSrcY2[col + srcStep];
				}else{
					yRT = pSrcY2[col + 2];
					yRB = pSrcY2[col + srcStep + 2];
				}/**/

				yT = ((dcol * (yRT - yLT)) >> 16) + yLT;
				yB = ((dcol * (yRB - yLB)) >> 16) + yLB;
				yR = ((drow * (yB - yT)) >> 16) + yT;
				Y4 = (Ipp8u) yR;

				/* 2. Resize for Cb and Cr */	
				col = fx2 >> 16;
				col = col * 4;
				Cb1 = pSrcCb1[col];
				Cb2 = pSrcCb2[col];
				Cr1 = pSrcCr1[col];
				Cr2 = pSrcCr2[col];
			
				fx += rcpRatiox << 1;
				fx1 += rcpRatiox << 1;
				/*	if(fx1 >= xLimit) {
					fx1 = xLimit - 1;
				}*/
				fx2 += rcpRatiox;
				/*	if(fx2 >= (xLimit >> 1)) {
					fx2 = (xLimit >> 1) - 1;
				}
				*/
				/* 2. Rotation storage */
				*pDstY1 = Y1;
				pDstY1 += stepX;
				*pDstY2 = Y2;
				pDstY2 += stepX;
				*pDstY3 = Y3;
				pDstY3 += stepX;
				*pDstY4 = Y4;
				pDstY4 += stepX;
				*pDstCb1 = Cb1;
				pDstCb1 += stepX1;
				*pDstCb2 = Cb2;
				pDstCb2 += stepX1;
				*pDstCr1 = Cr1;
				pDstCr1 += stepX2;
				*pDstCr2 = Cr2;
				pDstCr2 += stepX2;
			}
			pDstY1 += stepY;
			pDstY2 += stepY;
			pDstY3 += stepY;
			pDstY4 += stepY;
			pDstCb1 += stepY1;
			pDstCb2 += stepY1;
			pDstCr1 += stepY2;
			pDstCr2 += stepY2;

			ay += 2 * rcpRatioy;
			ay1 += 2 * rcpRatioy;
			if(ay1 >= yLimit) {
				ay1 = yLimit - 1;
			}

			pSrcY1 = (Ipp8u*)(pSrc + 1 + (ay >> 16) * srcStep);
			pSrcY2 = (Ipp8u*)(pSrc + 1 + (ay1 >> 16) * srcStep);
			pSrcCb1 = (Ipp8u*)(pSrc + ((ay+32768) >> 16) * srcStep);
			pSrcCb2 = (Ipp8u*)(pSrc + ((ay1+32768) >> 16) * srcStep);
			pSrcCr1 = (Ipp8u*)(pSrc + 2 + ((ay+32768) >> 16) * srcStep);
			pSrcCr2 = (Ipp8u*)(pSrc + 2 + ((ay1+32768) >> 16) * srcStep);
		}
	}
	else {
		return ippStsBadArgErr;
	}

	return ippStsNoErr;
}


static void _ppu_retrieve_format( CAM_ImageFormat eFmt, CAM_ImageFormat *pFmtCap, CAM_Int32s iFmtCapCnt, CAM_Int32s *pIndex )
{
	int j;

	*pIndex = -1;

	for ( j = 0; j < iFmtCapCnt; j++ )
	{
		if ( pFmtCap[j] == eFmt )
		{
			*pIndex = j;
			break;
		}
	}

	return;
}

static void _ppu_copy_image( Ipp8u *pSrc, int iSrcStep, Ipp8u *pDst, int iDstStep, int iBytesPerLine, int iLines )
{
	int   i, j;
	Ipp8u *pSrcBuf = NULL, *pDstBuf = NULL;

	pSrcBuf = pSrc;
	pDstBuf = pDst;

	for ( i = 0; i < iLines; i++ )
	{
		memmove( pDstBuf, pSrcBuf, iBytesPerLine );
		pSrcBuf += iSrcStep;
		pDstBuf += iDstStep;
	}

	return;
}
