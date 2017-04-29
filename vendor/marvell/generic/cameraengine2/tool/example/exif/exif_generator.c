#include "ippExif.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "misc.h"
#include "codecJP.h"
#include "ippIP.h"

#define   JPEG_IMAGE_SIZE         310171 
#define   ECHO	                  printf("%s, %d\n", __FUNCTION__, __LINE__);
#define   EXIFLOG(...)            do{ printf( __VA_ARGS__ ); fflush( NULL ); }while( 0 ) 
#define   _ALIGN_TO(x,iAlign)     ( (((Ipp32s)(x)) + (iAlign) - 1) & (~((iAlign) - 1)) )

#define ASSERT_ERROR( err )\
	do {\
		switch ( (err) )\
		{\
			case ippStsDataTypeErr:\
				EXIFLOG( "Bad input src image, do not having FFD8 in the head ( %s, %d )\n", __FILE__, __LINE__ );\
					break;\
			case ippStsNotSupportedModeErr:\
				EXIFLOG( "Exif info len is over 64K, currently do not supported writing out of App1 ( %s, %d )\n", __FILE__, __LINE__ );\
					break;\
			case ippStsBadArgErr:\
				EXIFLOG( "Bad input argment( %s, %d )\n", __FILE__, __LINE__ );\
					break;\
			case ippStsOutOfRangeErr:\
				EXIFLOG( "Input info is out of rang defined in Exif spec 2.2( %s, %d )\n", __FILE__, __LINE__ );\
					break;\
			case ippStsNoMemErr:\
				EXIFLOG( "No memory while reserved JIFI info( %s, %d )\n", __FILE__, __LINE__ );\
					break;\
			case ippStsNoErr:\
					break;\
			default:\
				EXIFLOG( "Unknown Error - %d, %s - %d\n", err, __FILE__, __LINE__ );\
					break;\
		}\
			if ( err != ippStsNoErr )\
				{ return -1;}\
	} while (0)

typedef enum
{
	// Low->high  R,G,B
	ippExif_IMGFMT_RGB888  = 2000,

	ippExif_IMGFMT_YCC420P = 3000,
	ippExif_IMGFMT_YCC422P,

	ippExif_IMGFMT_JPEG    = 4000,
}Exif_ImageFormat;

typedef struct
{
	Exif_ImageFormat    eFormat;
	Ipp32s              iWidth;
	Ipp32s              iHeight;
	Ipp32s              iStep[3];
	Ipp32s              iAllocLen[3];
	Ipp32s              iFilledLen[3];
	Ipp8u               *pBuffer[3];
} Exif_ImageBuffer;

static IppStatus _ipp_jpeg_thumbnail( Exif_ImageBuffer *pSrcImgBuf, Exif_ImageBuffer *pDstImgBuf, Ipp32s iQualityFactor );
static void _exif_display_lib_verson( );

int main( void )
{
	IppExifInfo stExifInfo                = IPPExif_Default_ExifInfo;
	IppExifInfo *pExifInfo                = &stExifInfo;
	IppExif32uRational urGPSTimeStamp[3]  = {{4, 1}, { 12, 1}, { 20, 1}};
	Ipp8u GPSProcessMethod[100]           = "gps process method";

	Ipp32u     ret             = 0;
	Ipp8u      *pHeap          = NULL;
	IppStatus  error           = ippStsNoErr;
	Ipp32s     iQualityFactor  = 0;
	Exif_ImageBuffer stPrimaryImgBuf, stThumbnailImgBuf;

	FILE          *pInputFile  = NULL;
	FILE          *pOutputFile = NULL;

	long long llt0 = 0;

	IppExifImgBuf stSrcImg     = {0};
	IppExifImgBuf stDstImg     = {0};
	IppExifBufReq stBufReq     = {0};

	memset( &stPrimaryImgBuf, 0 ,sizeof( Exif_ImageBuffer ));
	memset( &stThumbnailImgBuf, 0 ,sizeof( Exif_ImageBuffer ));

	stSrcImg.pBuffer = (Ipp8u*)malloc( JPEG_IMAGE_SIZE * sizeof(Ipp8u) );
	if( NULL == stSrcImg.pBuffer )
	{
		EXIFLOG( "Error: failed to allocate  memory \n" );
		return -1;
	}

	if( ( pInputFile = fopen( "hand.jpg", "rb" ) ) == NULL )
	{
		EXIFLOG( "failed to open the input file!\n" );
		free( stSrcImg.pBuffer );
		stSrcImg.pBuffer = NULL;
		return -1;
	}

	stSrcImg.iFilledLen = fread( stSrcImg.pBuffer, sizeof(Ipp8u), JPEG_IMAGE_SIZE, pInputFile );

	/* get ready to generate thumbnail */
	stPrimaryImgBuf.iWidth          = 2592;
	stPrimaryImgBuf.iHeight         = 1944;
	stPrimaryImgBuf.pBuffer[0]      = stSrcImg.pBuffer;
	stPrimaryImgBuf.iFilledLen[0]   = stSrcImg.iFilledLen;

	stThumbnailImgBuf.eFormat       = ippExif_IMGFMT_JPEG;
	stThumbnailImgBuf.iWidth        = 160;
	stThumbnailImgBuf.iHeight       = 120;

	switch( stThumbnailImgBuf.eFormat )
	{
	case ippExif_IMGFMT_YCC420P:
		pHeap = (Ipp8u*)malloc( stThumbnailImgBuf.iWidth * stThumbnailImgBuf.iHeight * 3 / 2 + 128 );
		if ( pHeap == NULL )
		{
			EXIFLOG( "Error: no enough memory !\n");
			free( stSrcImg.pBuffer );
			stSrcImg.pBuffer = NULL;
			return -1;
		}

		stThumbnailImgBuf.iStep[0] = stThumbnailImgBuf.iWidth;
		stThumbnailImgBuf.iStep[1] = stThumbnailImgBuf.iWidth >> 1;
		stThumbnailImgBuf.iStep[2] = stThumbnailImgBuf.iWidth >> 1;

		stThumbnailImgBuf.pBuffer[0] = (Ipp8u*)_ALIGN_TO( pHeap, 8 );
		stThumbnailImgBuf.pBuffer[1] = stThumbnailImgBuf.pBuffer[0] + stThumbnailImgBuf.iStep[0] * stThumbnailImgBuf.iHeight;
		stThumbnailImgBuf.pBuffer[2] = stThumbnailImgBuf.pBuffer[1] + stThumbnailImgBuf.iStep[1] * ( stThumbnailImgBuf.iHeight >> 1 );

		error = _ipp_jpeg_thumbnail( &stPrimaryImgBuf, &stThumbnailImgBuf, iQualityFactor );
		break;

	case ippExif_IMGFMT_YCC422P:
		pHeap = (Ipp8u*)malloc( stThumbnailImgBuf.iWidth * stThumbnailImgBuf.iHeight *  2 + 128 );
		if ( pHeap == NULL )
		{
			EXIFLOG( "Error: no enough memory !\n");
			free( stSrcImg.pBuffer );
			stSrcImg.pBuffer = NULL;
			return -1;
		}

		stThumbnailImgBuf.iStep[0] = stThumbnailImgBuf.iWidth;
		stThumbnailImgBuf.iStep[1] = stThumbnailImgBuf.iWidth >> 1;
		stThumbnailImgBuf.iStep[2] = stThumbnailImgBuf.iWidth >> 1;

		stThumbnailImgBuf.pBuffer[0] = (Ipp8u*)_ALIGN_TO( pHeap, 8 );
		stThumbnailImgBuf.pBuffer[1] = stThumbnailImgBuf.pBuffer[0] + stThumbnailImgBuf.iStep[0] * stThumbnailImgBuf.iHeight;
		stThumbnailImgBuf.pBuffer[2] = stThumbnailImgBuf.pBuffer[1] + stThumbnailImgBuf.iStep[1] * stThumbnailImgBuf.iHeight;

		error = _ipp_jpeg_thumbnail( &stPrimaryImgBuf, &stThumbnailImgBuf, iQualityFactor );
		break;

	case ippExif_IMGFMT_JPEG:
		pHeap = (Ipp8u*)malloc( stThumbnailImgBuf.iWidth * stThumbnailImgBuf.iHeight * 3  + 128 );
		if ( pHeap == NULL )
		{
			EXIFLOG( "Error: no enough memory !\n" );
			free( stSrcImg.pBuffer );
			stSrcImg.pBuffer = NULL;
			return -1;
		}

		stThumbnailImgBuf.pBuffer[0]   = (Ipp8u*)_ALIGN_TO( pHeap, 8 );
		stThumbnailImgBuf.iAllocLen[0] = stThumbnailImgBuf.iWidth * stThumbnailImgBuf.iHeight * 3 ;
		iQualityFactor                 = 60;
		error  = _ipp_jpeg_thumbnail( &stPrimaryImgBuf, &stThumbnailImgBuf, iQualityFactor );
		break;

	case ippExif_IMGFMT_RGB888:
		pHeap = (Ipp8u*)malloc( stThumbnailImgBuf.iWidth * stThumbnailImgBuf.iHeight * 3 + 128 );
		if ( pHeap == NULL )
		{
			EXIFLOG( "Error: no enough memory !\n");
			free( stSrcImg.pBuffer );
			stSrcImg.pBuffer = NULL;
			return -1;
		}

		stThumbnailImgBuf.iStep[0]   = stThumbnailImgBuf.iWidth * 3;
		stThumbnailImgBuf.pBuffer[0] = (Ipp8u*)_ALIGN_TO( pHeap, 8 );
		error = _ipp_jpeg_thumbnail( &stPrimaryImgBuf, &stThumbnailImgBuf, iQualityFactor );
		break;

	default:
		EXIFLOG( "Error: Unsupported thumbnail format\n");
		error = ippStsNotSupportedModeErr;
		break;
	}
	
	if( ippStsNoErr != error )
	{
		EXIFLOG( "Error: generate thumbnail faied %d!\n" , error);
		free( stSrcImg.pBuffer );
		free ( pHeap );
		stSrcImg.pBuffer = NULL;
		pHeap = NULL;
		return -1;
	}

	/* assign the exif info of primary image */
	memmove( pExifInfo->stTiffInfo.ucImageDescription, "hand_exif", strlen("hand_exif") + 1 );
	memmove( pExifInfo->stTiffInfo.ucMake, "marvell2010", strlen("marvell") + 1 );
	memmove( pExifInfo->stTiffInfo.ucModel, "dkb", strlen("dkb_brownstone_mg1") + 1 );

   // just for test
   // memmove( pExifInfo->stExifPrivateTagInfo.cExifVersion, "0110", strlen("0110") );

	pExifInfo->stTiffInfo.eOrientation = ippExifOrientationNormal;
	pExifInfo->stTiffInfo.eResolutionUnit = ippExifResolutionUnitCentimeters;
	pExifInfo->stTiffInfo.eYCbCrPosition = ippExifYCbCrPosCentral;

	ConvertTime( 1986, 6, 18, 12, 30, 57, pExifInfo->stExifPrivateTagInfo.ucDateTimeOriginal );
	ConvertTime( 1986, 6, 18, 12, 32, 0, pExifInfo->stExifPrivateTagInfo.ucDateTimeDigitized );
	ConvertTime( 1986, 6, 18, 12, 32, 0, pExifInfo->stTiffInfo.ucDataTime );
	pExifInfo->stExifPrivateTagInfo.eComponentConfiguration = ippExifComponentConfigYCbCr;
	ToRational( 1.5, pExifInfo->stExifPrivateTagInfo.urFocalLength );
	pExifInfo->stExifPrivateTagInfo.eColorSpace       = ippExifColorSpaceUnCalibrated;
	pExifInfo->stExifPrivateTagInfo.uiPixelXDimention = 2592;
	pExifInfo->stExifPrivateTagInfo.uiPixelYDimention = 1944;
	pExifInfo->stExifPrivateTagInfo.eExposureMode     = ippExifExposureModeManual;
	pExifInfo->stExifPrivateTagInfo.eWhiteBalance     = ippExifWhiteBalanceManual;

	pExifInfo->stGPSInfo.eGPSLatituteRef = ippExifGPSLatituteRefNorth;
	pExifInfo->stGPSInfo.eGPSLongtituteRef = ippExifGPSLongtituteRefWest;

	pExifInfo->stGPSInfo.eGPSAltitudeRef   = ippExifAltitudeRefSeaLevel;
	ToRational( 24.67, pExifInfo->stGPSInfo.urGPSAltitude );

	Decimalto60( 39.20567, pExifInfo->stGPSInfo.urGPSLatitute );
	Decimalto60( 15.6789, pExifInfo->stGPSInfo.urGPSLongtitute );
	memmove( pExifInfo->stGPSInfo.urGPSTimeStamp, urGPSTimeStamp, sizeof( urGPSTimeStamp ));

	pExifInfo->stGPSInfo.pGPSProcessMethod     = GPSProcessMethod;
	pExifInfo->stGPSInfo.uiGPSProcessMethodLen = strlen( (char*)GPSProcessMethod );
	memmove( pExifInfo->stGPSInfo.ucGPSDateStamp, "2011:03:02", strlen("2011:03:02") + 1);

	ToSRational( -99.99, pExifInfo->stExifPrivateTagInfo.srExposureBiaValue );
	ToSRational( -3.14, pExifInfo->stExifPrivateTagInfo.srBrightnessValue );
	ToRational( 1.2, pExifInfo->stExifPrivateTagInfo.urApertureValue );
	ToRational( 1.2, pExifInfo->stExifPrivateTagInfo.urMaxApertureValue );
	ToRational( 0.8, pExifInfo->stExifPrivateTagInfo.urFNumber );

	pExifInfo->stExifPrivateTagInfo.eExposureProgram       = ippExifExposureProgramNotDefined;
	pExifInfo->stExifPrivateTagInfo.eMeteringMode          = ippExifMeteringModeUnknown;
	pExifInfo->stExifPrivateTagInfo.eContrast              = ippExifContrastNormal;
	pExifInfo->stExifPrivateTagInfo.eSaturation            = ippExifSaturationNormal;
	pExifInfo->stExifPrivateTagInfo.eSharpness             = ippExifSharpnessHard;
	pExifInfo->stExifPrivateTagInfo.eLightSource           = ippExifLightSourceUnKnown;
	pExifInfo->stExifPrivateTagInfo.eSceneCaptureType      = ippExifSceneCaptureTypeStandard;
	pExifInfo->stExifPrivateTagInfo.eFlash                 = ippExifFlashFired_CFMODE_SRLD;

	pExifInfo->stExifPrivateTagInfo.eCustomRendered        = ippExifCustomRenderedNormal;
	pExifInfo->stExifPrivateTagInfo.eSubjectDistanceRange  = ippExifSDRUnknow;
	memmove( pExifInfo->stGPSInfo.ucGPSMapDatum, "TOKYO", strlen("TOKYO") + 1);
	/* assign the exif info of thumbnail image */
	pExifInfo->stThumbnailInfo.pData    = stThumbnailImgBuf.pBuffer[0];
	pExifInfo->stThumbnailInfo.iDataLen = stThumbnailImgBuf.iFilledLen[0] + stThumbnailImgBuf.iFilledLen[1] + stThumbnailImgBuf.iFilledLen[2];

	switch( stThumbnailImgBuf.eFormat )
	{
	case ippExif_IMGFMT_YCC420P: 
		pExifInfo->stThumbnailInfo.eCompression                        = ippExifCompressionUnCompressed;
		pExifInfo->stThumbnailInfo.ePhotometricInterpretation          = ippExifPhotometricInterpretationYCC;
		pExifInfo->stThumbnailInfo.eResolutionUnit                     = ippExifResolutionUnitInch;
		pExifInfo->stThumbnailInfo.stRawThumbAttribute.uiHeight        = stThumbnailImgBuf.iHeight;
		pExifInfo->stThumbnailInfo.stRawThumbAttribute.uiWidth         = stThumbnailImgBuf.iWidth;
		pExifInfo->stThumbnailInfo.stRawThumbAttribute.ePlanarConfig   = ippExifPlanarConfigPlanar;
		pExifInfo->stThumbnailInfo.stRawThumbAttribute.eYCCSubSampling = ippExifYCCSubSampling420;
		break;

	case  ippExif_IMGFMT_YCC422P:
		pExifInfo->stThumbnailInfo.eCompression                        = ippExifCompressionUnCompressed;
		pExifInfo->stThumbnailInfo.ePhotometricInterpretation          = ippExifPhotometricInterpretationYCC;
		pExifInfo->stThumbnailInfo.eResolutionUnit                     = ippExifResolutionUnitInch;
		pExifInfo->stThumbnailInfo.stRawThumbAttribute.uiHeight        = stThumbnailImgBuf.iHeight;
		pExifInfo->stThumbnailInfo.stRawThumbAttribute.uiWidth         = stThumbnailImgBuf.iWidth;
		pExifInfo->stThumbnailInfo.stRawThumbAttribute.ePlanarConfig   = ippExifPlanarConfigPlanar;
		pExifInfo->stThumbnailInfo.stRawThumbAttribute.eYCCSubSampling = ippExifYCCSubSampling422;
		break;

	case ippExif_IMGFMT_JPEG:
		pExifInfo->stThumbnailInfo.eCompression                       = ippExifCompressionThumbnailJPEG;
		break;

	case ippExif_IMGFMT_RGB888 :
		pExifInfo->stThumbnailInfo.eCompression                      = ippExifCompressionUnCompressed;
		pExifInfo->stThumbnailInfo.ePhotometricInterpretation        = ippExifPhotometricInterpretationRGB;
		pExifInfo->stThumbnailInfo.eResolutionUnit                   = ippExifResolutionUnitInch;
		pExifInfo->stThumbnailInfo.stRawThumbAttribute.uiHeight      = stThumbnailImgBuf.iHeight;
		pExifInfo->stThumbnailInfo.stRawThumbAttribute.uiWidth       = stThumbnailImgBuf.iWidth;
		pExifInfo->stThumbnailInfo.stRawThumbAttribute.ePlanarConfig = ippExifPlanarConfigChunky;
		break;

	default:
		EXIFLOG( "Error: Unsupported thumbnail format\n" );
		break;
	}

	/* dump thumbnail info to temp file*/
	if ( ippExif_IMGFMT_JPEG == stThumbnailImgBuf.eFormat )
	{
		FILE *pThumbnailFile = NULL;
		if( ( pThumbnailFile = fopen("/data/thumbnail.jpg", "w" ) ) == NULL )
		{
			EXIFLOG( "failed to open the thumbnail file!\n" );
			free( stSrcImg.pBuffer );
			free ( pHeap );
			stSrcImg.pBuffer = NULL;
			pHeap = NULL;
			return -1;
		}
		ret = fwrite( pExifInfo->stThumbnailInfo.pData, sizeof(Ipp8s), pExifInfo->stThumbnailInfo.iDataLen, pThumbnailFile );
		if( (int)NULL == ret )
		{
			EXIFLOG( "failded to dump thumbnail image\n" );
			free( stSrcImg.pBuffer );
			free ( pHeap );
			stSrcImg.pBuffer = NULL;
			pHeap = NULL;
			return -1;
		}
		fclose( pThumbnailFile);
	}

	_exif_display_lib_verson( );

	error = ippExifGetBufReq( pExifInfo, &stBufReq );
	ASSERT_ERROR( error );
	
	stDstImg.iAllocLen = stBufReq.iMinBufLen + JPEG_IMAGE_SIZE;
	stDstImg.pBuffer   = (Ipp8u*)malloc( stDstImg.iAllocLen * sizeof(Ipp8u) );
	if( NULL == stDstImg.pBuffer )
	{
		EXIFLOG( "Error: failed to allocate Dst memory \n" );
		free( stSrcImg.pBuffer );
		free ( pHeap );
		stSrcImg.pBuffer = NULL;
		pHeap = NULL;
		return -1;
	}

#ifdef EXIF_PERF
	llt0 = -IPP_TimeGetTickCount();
#endif

	error = ippExifWrite( &stSrcImg, &stDstImg, pExifInfo );
	ASSERT_ERROR( error );

#ifdef EXIF_PERF
	llt0 += IPP_TimeGetTickCount();
	EXIFLOG("the running time of ippExifWriter is %.4f ms! \n", llt0 / 1000.0 );
#endif

	if( ( pOutputFile = fopen("hand_exiftest.jpg", "w" ) ) == NULL )
	{
		EXIFLOG( "failed to open the outnput file!\n" );
		free( stSrcImg.pBuffer );
		free( stDstImg.pBuffer );
		free( pHeap );
		stDstImg.pBuffer = NULL;
		stSrcImg.pBuffer = NULL;
		pHeap = NULL;
		return -1;
	}
	ret = fwrite( stDstImg.pBuffer, sizeof(Ipp8u), stDstImg.iFilledLen, pOutputFile );
	if( (int)NULL == ret )
	{
		EXIFLOG( "failded write exif info to image\n" );
		free( stSrcImg.pBuffer );
		free( stDstImg.pBuffer );
		free( pHeap );
		stDstImg.pBuffer = NULL;
		stSrcImg.pBuffer = NULL;
		pHeap = NULL;
		return -1;
	}

	free( stDstImg.pBuffer );
	free( stSrcImg.pBuffer );
	free( pHeap );
	pHeap = NULL;
	stDstImg.pBuffer = NULL;
	stSrcImg.pBuffer = NULL;
	fclose( pInputFile );
	fclose( pOutputFile );
	return 0;
}



static IppStatus _ipp_jpeg_thumbnail( Exif_ImageBuffer *pSrcImgBuf, Exif_ImageBuffer *pDstImgBuf, Ipp32s iQualityFactor )
{
	MiscGeneralCallbackTable *pCallBackTable = NULL;

	IppJPEGEncoderParam       stEncoderPar;
	IppBitstream              stDstBitStream;
	IppPicture                stSrcPicture;
	void                      *pEncoderState = NULL;

	IppJPEGDecoderParam       stDecoderPar;
	IppPicture                stDstPicture;
	IppBitstream              stSrcBitStream;  
	void                      *pDecoderState = NULL;
	
	IppCodecStatus            eRetCode;
	Ipp8u                     *pHeap = NULL;
	Exif_ImageBuffer          stTempBuffer;
	Ipp8u                     cImgEnd[2] = { 0 };

	memset( &stDecoderPar, 0, sizeof(IppJPEGDecoderParam) );
	memset( &stEncoderPar, 0, sizeof(IppJPEGEncoderParam) );

	memset( &stDstPicture, 0, sizeof(IppPicture) );
	memset( &stSrcPicture, 0, sizeof(IppPicture) );

	memset( &stSrcBitStream, 0, sizeof(IppBitstream) );
	memset( &stDstBitStream, 0, sizeof(IppBitstream) );
	memset( &stTempBuffer, 0, sizeof(Exif_ImageBuffer) );

	if ( pSrcImgBuf->pBuffer[0] == NULL )
	{
		EXIFLOG( "Error: bad memory pointer !\n" );
		return ippStsNullPtrErr;
	}

	// Init callback table
	if ( miscInitGeneralCallbackTable( &pCallBackTable ) != 0 )
	{
		EXIFLOG( "Error: JPEG init callback table Failed\n" );
		return ippStsErr;
	}
	// dummy fFileRead callback function
	pCallBackTable->fFileRead = NULL;
	stSrcBitStream.pBsCurByte = stSrcBitStream.pBsBuffer = pSrcImgBuf->pBuffer[0];
	stSrcBitStream.bsByteLen = pSrcImgBuf->iFilledLen[0];
	stSrcBitStream.bsCurBitOffset = 0;

	/*  XXX: why I do this? Oh, it's for robustness consideration, if a JPEG file
	 *      is damaged, maybe we need not frustrated, some decoders also can decode it
	 *      although imcomplete, but "half a bread is better than none", you know.IPP JPEG
	 *      decoder can decode a damaged JPEG at its best, but the presumption is that
	 *     you should give it a 0xFFD9 to remind it, the file is finished. 
	 */
	cImgEnd[0] = pSrcImgBuf->pBuffer[0][stSrcBitStream.bsByteLen - 2];
	cImgEnd[1] = pSrcImgBuf->pBuffer[0][stSrcBitStream.bsByteLen - 1];
	pSrcImgBuf->pBuffer[0][stSrcBitStream.bsByteLen - 1] = 0xD9;
	pSrcImgBuf->pBuffer[0][stSrcBitStream.bsByteLen - 2] = 0xFF;

#if 0

	EXIFLOG( "stSrcBitStream.pBsCurByte: %p\n stSrcBitStream.pBsBuffer: %p\n\
		   stSrcBitStream.bsByteLen: %d\n, stSrcBitStream.bsCurBitOffset :%d\n",
		   stSrcBitStream.pBsCurByte, stSrcBitStream.pBsBuffer, stSrcBitStream.bsByteLen,
		   stSrcBitStream.bsCurBitOffset );

#endif

	eRetCode = DecoderInitAlloc_JPEG( &stSrcBitStream, &stDstPicture, pCallBackTable, &pDecoderState, (void *)NULL );
	if ( IPP_STATUS_NOERR != eRetCode )
	{
		miscFreeGeneralCallbackTable( &pCallBackTable );
		EXIFLOG( "Error: init JPEG decoder failed!\n" );
		return ippStsErr;
	}

	// assign the dst picture width and height
	stDstPicture.picWidth  =  pDstImgBuf ->iWidth;
	stDstPicture.picHeight =  pDstImgBuf ->iHeight;

	// assign the DecoderPar
	if((stDecoderPar.srcROI.width == 0) || (stDecoderPar.srcROI.height == 0)){
		stDecoderPar.srcROI.x = 0;
		stDecoderPar.srcROI.y = 0;
		stDecoderPar.srcROI.width = pSrcImgBuf->iWidth; 
		stDecoderPar.srcROI.height = pSrcImgBuf->iHeight; 
		}

	stDecoderPar.nScale = 0;

	if( ippExif_IMGFMT_JPEG == pDstImgBuf->eFormat)
	{
		pHeap = (Ipp8u*)malloc( pDstImgBuf ->iWidth * pDstImgBuf ->iHeight * 2 + 128 );

		if ( pHeap == NULL )
		{	
			EXIFLOG( "Error: no enough memory !\n");
			return ippStsNoMemErr;
		}

		stTempBuffer.eFormat  = ippExif_IMGFMT_YCC422P;
		stTempBuffer.iWidth   = pDstImgBuf ->iWidth;
		stTempBuffer.iHeight  = pDstImgBuf ->iHeight;
		stTempBuffer.iStep[0] = pDstImgBuf ->iWidth;
		stTempBuffer.iStep[1] = pDstImgBuf ->iWidth >> 1;
		stTempBuffer.iStep[2] = pDstImgBuf ->iWidth >> 1;

		stTempBuffer.pBuffer[0] = (Ipp8u *)_ALIGN_TO( pHeap, 8 );
		stTempBuffer.pBuffer[1] = stTempBuffer.pBuffer[0] + stTempBuffer.iStep[0] * stTempBuffer.iHeight;
		stTempBuffer.pBuffer[2] = stTempBuffer.pBuffer[1] + stTempBuffer.iStep[1] * stTempBuffer.iHeight; 

		stTempBuffer.iFilledLen[0] = stTempBuffer.iStep[0] * stTempBuffer.iHeight;
		stTempBuffer.iFilledLen[1] = stTempBuffer.iStep[1] * stTempBuffer.iHeight;
		stTempBuffer.iFilledLen[2] = stTempBuffer.iStep[2] * stTempBuffer.iHeight;
	}

	switch ( pDstImgBuf->eFormat )
	{
	case ippExif_IMGFMT_YCC420P :
		stDecoderPar.nDesiredColor = JPEG_YUV411;
		stDstPicture.picPlaneNum     = 3;
		stDstPicture.picPlaneStep[0] = pDstImgBuf->iStep[0];
		stDstPicture.picPlaneStep[1] = pDstImgBuf->iStep[1];
		stDstPicture.picPlaneStep[2] = pDstImgBuf->iStep[2];
		stDstPicture.ppPicPlane[0]   = pDstImgBuf->pBuffer[0];
		stDstPicture.ppPicPlane[1]   = pDstImgBuf->pBuffer[1];
		stDstPicture.ppPicPlane[2]   = pDstImgBuf->pBuffer[2];
		break;
	case ippExif_IMGFMT_YCC422P:
		stDecoderPar.nDesiredColor = JPEG_YUV422;
		stDstPicture.picPlaneNum     = 3;
		stDstPicture.picPlaneStep[0] = pDstImgBuf->iStep[0];
		stDstPicture.picPlaneStep[1] = pDstImgBuf->iStep[1];
		stDstPicture.picPlaneStep[2] = pDstImgBuf->iStep[2];
		stDstPicture.ppPicPlane[0]   = pDstImgBuf->pBuffer[0];
		stDstPicture.ppPicPlane[1]   = pDstImgBuf->pBuffer[1];
		stDstPicture.ppPicPlane[2]   = pDstImgBuf->pBuffer[2];
		break;

	case ippExif_IMGFMT_RGB888 :
		stDecoderPar.nDesiredColor = JPEG_RGB888;
		stDstPicture.picPlaneNum     = 1;
		stDstPicture.picPlaneStep[0] = pDstImgBuf->iStep[0];
		stDstPicture.ppPicPlane[0]   = pDstImgBuf->pBuffer[0];
		break;

	case ippExif_IMGFMT_JPEG :
		/* first generate yuv 422 temp format */
		stDecoderPar.nDesiredColor = JPEG_YUV422;
		stDstPicture.picPlaneNum     = 3;
		stDstPicture.picPlaneStep[0] = stTempBuffer.iStep[0];
		stDstPicture.picPlaneStep[1] = stTempBuffer.iStep[1];
		stDstPicture.picPlaneStep[2] = stTempBuffer.iStep[2];
		stDstPicture.ppPicPlane[0]   = stTempBuffer.pBuffer[0];
		stDstPicture.ppPicPlane[1]   = stTempBuffer.pBuffer[1];
		stDstPicture.ppPicPlane[2]   = stTempBuffer.pBuffer[2];
		break;

	default:
		EXIFLOG("Error: the given format[%d] is not supported by JPEG ROI decoder!\n", pDstImgBuf->eFormat );
		return ippStsNotSupportedModeErr;
		break;
	}


#if 0

	EXIFLOG( "Info:\
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
		   stDecoderPar.srcROI.y=%d, \n\
		   stDecoderPar.nScale =%d \n",
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
		   stDecoderPar.nScale);

#endif

	eRetCode = Decode_JPEG( &stDstPicture, &stDecoderPar, pDecoderState );
	if ( IPP_STATUS_NOERR != eRetCode )
	{
		EXIFLOG( "Error: JPEG_ROIDec Failed, error code[%d]( %s, %d )!\n", eRetCode, __FUNCTION__, __LINE__ );
		DecoderFree_JPEG( &pDecoderState );
		miscFreeGeneralCallbackTable( &pCallBackTable );
		pSrcImgBuf->pBuffer[0][stSrcBitStream.bsByteLen - 1] = cImgEnd[1];
		pSrcImgBuf->pBuffer[0][stSrcBitStream.bsByteLen - 2] = cImgEnd[0];
		free( pHeap );
		pHeap = NULL;
		return  ippStsErr;
	}

	DecoderFree_JPEG( &pDecoderState );

	miscFreeGeneralCallbackTable( &pCallBackTable );
	pSrcImgBuf->pBuffer[0][stSrcBitStream.bsByteLen - 1] = cImgEnd[1];
	pSrcImgBuf->pBuffer[0][stSrcBitStream.bsByteLen - 2] = cImgEnd[0];

#if 0
	{
		FILE *fp = NULL;
		fp = fopen("/data/422.yuv", "wb");
		fwrite (stTempBuffer.pBuffer[0], 1, stTempBuffer.iFilledLen[0], fp);
		fwrite (stTempBuffer.pBuffer[1], 1, stTempBuffer.iFilledLen[1], fp);
		fwrite (stTempBuffer.pBuffer[2], 1, stTempBuffer.iFilledLen[2], fp);
		fclose(fp);
	}
#endif

	/* convert form yuv 422 to JPEG */
	if( ippExif_IMGFMT_JPEG == pDstImgBuf->eFormat)
	{

		stSrcPicture.picWidth            = stTempBuffer.iWidth;
		stSrcPicture.picHeight           = stTempBuffer.iHeight;
		stSrcPicture.picFormat           = JPEG_YUV422;
		stSrcPicture.picChannelNum       = 3;
		stSrcPicture.picPlaneNum         = 3;
		stSrcPicture.picPlaneStep[0]     = stTempBuffer.iStep[0];
		stSrcPicture.picPlaneStep[1]     = stTempBuffer.iStep[1];
		stSrcPicture.picPlaneStep[2]     = stTempBuffer.iStep[2];
		stSrcPicture.ppPicPlane[0]       = stTempBuffer.pBuffer[0];
		stSrcPicture.ppPicPlane[1]       = stTempBuffer.pBuffer[1];
		stSrcPicture.ppPicPlane[2]       = stTempBuffer.pBuffer[2];
		stSrcPicture.picROI.x            = 0;
		stSrcPicture.picROI.y            = 0;
		stSrcPicture.picROI.width        = stTempBuffer.iWidth;
		stSrcPicture.picROI.height       = stTempBuffer.iHeight;
		stDstBitStream.pBsBuffer         = (Ipp8u*)pDstImgBuf->pBuffer[0];
		stDstBitStream.bsByteLen         = pDstImgBuf->iAllocLen[0];
		stDstBitStream.pBsCurByte        = (Ipp8u*)pDstImgBuf->pBuffer[0];
		stDstBitStream.bsCurBitOffset    = 0;

		stEncoderPar.nQuality            = iQualityFactor;
		stEncoderPar.nRestartInterval    = 0;
		stEncoderPar.nJPEGMode           = JPEG_BASELINE;
		stEncoderPar.nSubsampling        = JPEG_422 ;
		stEncoderPar.nBufType            = JPEG_INTEGRATEBUF;
		stEncoderPar.pSrcPicHandler      = (void*)NULL;
		stEncoderPar.pStreamHandler      = (void*)NULL;

		if ( miscInitGeneralCallbackTable(&pCallBackTable) != 0 ) 
		{
			free( pHeap );
			pHeap = NULL;
			return ippStsNoMemErr;
		}

		eRetCode = EncoderInitAlloc_JPEG( &stEncoderPar, &stSrcPicture, &stDstBitStream, pCallBackTable, &pEncoderState );

		if ( IPP_STATUS_NOERR != eRetCode )
		{
			miscFreeGeneralCallbackTable( &pCallBackTable );
			free( pHeap );
			pHeap = NULL;
			return ippStsErr;
		}

		eRetCode = Encode_JPEG( &stSrcPicture, &stDstBitStream, pEncoderState );

		if ( IPP_STATUS_NOERR != eRetCode ) 
		{
			EncoderFree_JPEG( &pEncoderState );
			miscFreeGeneralCallbackTable( &pCallBackTable );
			free( pHeap );
			pHeap = NULL;
			return ippStsErr;
		}
		pDstImgBuf->iFilledLen[0] = (Ipp32s)stDstBitStream.pBsCurByte - (Ipp32s)stDstBitStream.pBsBuffer;
		pDstImgBuf->iFilledLen[1] = 0;
		pDstImgBuf->iFilledLen[2] = 0;

#if 0
	{
		FILE *fp = NULL;
		fp = fopen("/data/thumbnail.jpg", "wb");
		fwrite (pDstImgBuf->pBuffer[0], 1, pDstImgBuf->iFilledLen[0], fp);
		fclose(fp);
	}
#endif

		EncoderFree_JPEG( &pEncoderState );
		miscFreeGeneralCallbackTable( &pCallBackTable );

		free(pHeap);
		pHeap = NULL;
	}

	switch ( pDstImgBuf->eFormat )
	{
	case ippExif_IMGFMT_YCC420P :
		pDstImgBuf->iFilledLen[0] = pDstImgBuf->iStep[0] * pDstImgBuf->iHeight;
		pDstImgBuf->iFilledLen[1] = pDstImgBuf->iStep[1] * pDstImgBuf->iHeight >> 1;
		pDstImgBuf->iFilledLen[2] = pDstImgBuf->iStep[2] * pDstImgBuf->iHeight >> 1;
		break;

	case ippExif_IMGFMT_YCC422P:
		pDstImgBuf->iFilledLen[0] = pDstImgBuf->iStep[0] * pDstImgBuf->iHeight;
		pDstImgBuf->iFilledLen[1] = pDstImgBuf->iStep[1] * pDstImgBuf->iHeight;
		pDstImgBuf->iFilledLen[2] = pDstImgBuf->iStep[2] * pDstImgBuf->iHeight;
		break;

	case ippExif_IMGFMT_RGB888:
		pDstImgBuf->iFilledLen[0] = pDstImgBuf->iStep[0] * pDstImgBuf->iHeight ;
		pDstImgBuf->iFilledLen[1] = 0;
		pDstImgBuf->iFilledLen[2] = 0;
		break;

	default:
		break;
	}

	return ippStsNoErr;
}

static void _exif_display_lib_verson( )
{
	char libversion[128]={0};
	IppStatus error;
	error = ippExifGetLibVersion( libversion, sizeof(libversion));
	if( ippStsNoErr == error )
	{
		EXIFLOG( "\n*****************************************************************\n" );
		EXIFLOG( "This library is built from %s\n", libversion );
		EXIFLOG( "*****************************************************************\n" );
	}
	else
	{
		EXIFLOG( "\n*****************************************************************\n" );
		EXIFLOG( "Can't find this library version information\n" );
		EXIFLOG( "*****************************************************************\n" );
	}
}
