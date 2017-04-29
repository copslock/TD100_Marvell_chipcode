#include "ippExif.h"

#include "codecJP.h"
#include "ippIP.h"
#include "misc.h"

#include "CameraEngine.h"//for CAM_ImageBuffer, CAM_ImageFormat
#include "CameraSetting.h"//for CameraSetting

#include <cmath>
#include <stdio.h>
#include <stdlib.h>

#include <binder/MemoryBase.h>
#include <binder/MemoryHeapBase.h>
#include <binder/MemoryHeapPmem.h>
#include <linux/android_pmem.h>

#include "cam_log_mrvl.h"
#include "exif_helper.h"

namespace android {

#define   _ALIGN_TO(x,iAlign) ( (((Ipp32s)(x)) + (iAlign) - 1) & (~((iAlign) - 1)) )

    static IppStatus _ipp_jpeg_thumbnail(const CAM_ImageBuffer *pSrcImgBuf, CAM_ImageBuffer *pDstImgBuf, Ipp32s iQualityFactor )
    {
        MiscGeneralCallbackTable *pCallBackTable = NULL;

        IppJPEGEncoderParam       stEncoderPar;
        IppBitstream              stDstBitStream;
        IppPicture                stSrcPicture;
        void                      *pEncoderState = NULL;

        IppJPEGDecoderParam      stDecoderPar;
        IppPicture               stDstPicture;
        IppBitstream             stSrcBitStream;
        void                     *pDecoderState = NULL;

        IppCodecStatus           eRetCode;
        Ipp32u                   iWidthScale = 0, iHeightScale = 0;

        Ipp8u                    *pHeap = NULL, *pDstBuf = NULL;
        CAM_ImageBuffer          stTempBuffer;

        Ipp8u                    cImgEnd[2] = {0, 0};

        memset( &stDecoderPar, 0, sizeof(IppJPEGDecoderParam) );
        memset( &stDstPicture, 0, sizeof(IppPicture) );
        memset( &stTempBuffer, 0, sizeof(CAM_ImageBuffer) );

        if ( pSrcImgBuf->pBuffer[0] == NULL )
        {
            TRACE_E( "Error: memory pointer is NULL !\n");
            return ippStsNoMemErr;
        }

        if (CAM_IMGFMT_JPEG == pSrcImgBuf->eFormat){

            TRACE_D("%s: thumbnail src image is jpeg",__FUNCTION__);
            // Init callback table
            if ( miscInitGeneralCallbackTable( &pCallBackTable ) != 0 )
            {
                TRACE_E( "Error: init JPEG decoder failed!\n");
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

            TRACE_V( "stSrcBitStream.pBsCurByte: %p\n stSrcBitStream.pBsBuffer: %p\n\
                    stSrcBitStream.bsByteLen: %d\n, stSrcBitStream.bsCurBitOffset :%d\n",
                    stSrcBitStream.pBsCurByte, stSrcBitStream.pBsBuffer, stSrcBitStream.bsByteLen,
                    stSrcBitStream.bsCurBitOffset );

            eRetCode = DecoderInitAlloc_JPEG( &stSrcBitStream, &stDstPicture, pCallBackTable, &pDecoderState, (void *)NULL );
            if ( IPP_STATUS_NOERR != eRetCode )
            {
                miscFreeGeneralCallbackTable( &pCallBackTable );
                TRACE_E( "Error: init JPEG decoder failed!\n");
                return ippStsErr;
            }

            stDstPicture.picWidth  =  pDstImgBuf ->iWidth;
            stDstPicture.picHeight =  pDstImgBuf ->iHeight;

            stDecoderPar.srcROI.x = 0;
            stDecoderPar.srcROI.y = 0;
            stDecoderPar.srcROI.width = pSrcImgBuf ->iWidth;
            stDecoderPar.srcROI.height = pSrcImgBuf ->iHeight;

            TRACE_D("Info: snapshot: %d x %d \n", stDstPicture.picWidth, stDstPicture.picHeight);

            iWidthScale = ( stDecoderPar.srcROI.width << 16 ) / stDstPicture.picWidth;  //Q16
            iHeightScale = ( stDecoderPar.srcROI.height << 16 ) / stDstPicture.picHeight;  //Q16

            if ( iWidthScale != iHeightScale )
            {
                TRACE_E("Currently ipp jpeg deocder lib do not support widthScale not equal to heightScale\n");
                return ippStsBadArgErr;
            }

            stDecoderPar.nScale = iWidthScale;

            if( CAM_IMGFMT_JPEG == pDstImgBuf->eFormat)
            {
                pHeap = (Ipp8u*)malloc( pDstImgBuf ->iWidth * pDstImgBuf ->iHeight * 2 + 128 );

                if (NULL == pHeap)
                {
                    TRACE_E( "Error: no enough memory !\n");
                    return ippStsNoMemErr;
                }

                stTempBuffer.eFormat  = CAM_IMGFMT_YCbCr422P;
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

            // map format between Camera Engine and ipp lib

            switch ( pDstImgBuf->eFormat )
            {
                case CAM_IMGFMT_YCbCr420P:

                    stDecoderPar.nDesiredColor = JPEG_YUV411;
                    stDstPicture.picPlaneNum     = 3;
                    stDstPicture.picPlaneStep[0] = pDstImgBuf->iStep[0];
                    stDstPicture.picPlaneStep[1] = pDstImgBuf->iStep[1];
                    stDstPicture.picPlaneStep[2] = pDstImgBuf->iStep[2];
                    stDstPicture.ppPicPlane[0]   = pDstImgBuf->pBuffer[0];
                    stDstPicture.ppPicPlane[1]   = pDstImgBuf->pBuffer[1];
                    stDstPicture.ppPicPlane[2]   = pDstImgBuf->pBuffer[2];
                    break;

                case CAM_IMGFMT_YCbCr422P:

                    stDecoderPar.nDesiredColor = JPEG_YUV422;
                    stDstPicture.picPlaneNum     = 3;
                    stDstPicture.picPlaneStep[0] = pDstImgBuf->iStep[0];
                    stDstPicture.picPlaneStep[1] = pDstImgBuf->iStep[1];
                    stDstPicture.picPlaneStep[2] = pDstImgBuf->iStep[2];
                    stDstPicture.ppPicPlane[0]   = pDstImgBuf->pBuffer[0];
                    stDstPicture.ppPicPlane[1]   = pDstImgBuf->pBuffer[1];
                    stDstPicture.ppPicPlane[2]   = pDstImgBuf->pBuffer[2];
                    break;

                case CAM_IMGFMT_RGB888 :

                    stDecoderPar.nDesiredColor = JPEG_RGB888;
                    stDstPicture.picPlaneNum     = 1;
                    stDstPicture.picPlaneStep[0] = pDstImgBuf->iStep[0];
                    stDstPicture.ppPicPlane[0]   = pDstImgBuf->pBuffer[0];
                    break;

                case CAM_IMGFMT_JPEG :

                    //first generate yuv 422 temp format
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
                    TRACE_E("Error: the given format[%d] is not supported by JPEG ROI decoder!\n", pDstImgBuf->eFormat );
                    return ippStsNotSupportedModeErr;
                    break;
            }


            TRACE_V( "Info:\
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


            // call the core JPEG decoder function

            eRetCode = Decode_JPEG( &stDstPicture, &stDecoderPar, pDecoderState);

            // recover back pBuffer last two bytes
            pSrcImgBuf->pBuffer[0][stSrcBitStream.bsByteLen - 1] = cImgEnd[1];
            pSrcImgBuf->pBuffer[0][stSrcBitStream.bsByteLen - 2] = cImgEnd[0];

            if ( IPP_STATUS_NOERR != eRetCode )
            {
                TRACE_E( "Error: JPEG_ROIDec Failed, error code[%d]( %s, %d )!\n", eRetCode, __FUNCTION__, __LINE__ );
                DecoderFree_JPEG( &pDecoderState );
                miscFreeGeneralCallbackTable( &pCallBackTable );
                return ippStsErr;
            }

            DecoderFree_JPEG( &pDecoderState );

            miscFreeGeneralCallbackTable( &pCallBackTable );
        }
        else if(CAM_IMGFMT_YCbCr422P == pSrcImgBuf->eFormat){
            TRACE_D("%s: thumbnail src image is 422P",__FUNCTION__);
            //for jpeg interleave input, we need do: yuv->jpeg encoder
#if 1
            //TODO: HERE we assume all yuv components in jpeg-interleave is 8 bytes align
            memcpy(&stTempBuffer, pSrcImgBuf, sizeof(CAM_ImageBuffer));
#else    //only for debug
            stTempBuffer.eFormat  = CAM_IMGFMT_YCbCr422P;
            stTempBuffer.iWidth   = pSrcImgBuf->iWidth;
            stTempBuffer.iHeight  = pSrcImgBuf->iHeight;
            stTempBuffer.iStep[0] = pSrcImgBuf->iStep[0];
            stTempBuffer.iStep[1] = pSrcImgBuf->iStep[1];
            stTempBuffer.iStep[2] = pSrcImgBuf->iStep[2];

            //TODO: HERE we assume all yuv components in jpeg-interleave is 8 bytes align
            stTempBuffer.pBuffer[0] = (Ipp8u *)pSrcImgBuf->pBuffer[0];
            stTempBuffer.pBuffer[1] = (Ipp8u *)pSrcImgBuf->pBuffer[1];
            stTempBuffer.pBuffer[2] = (Ipp8u *)pSrcImgBuf->pBuffer[2];

            stTempBuffer.iFilledLen[0] = stTempBuffer.iStep[0] * stTempBuffer.iHeight;
            stTempBuffer.iFilledLen[1] = stTempBuffer.iStep[1] * stTempBuffer.iHeight;
            stTempBuffer.iFilledLen[2] = stTempBuffer.iStep[2] * stTempBuffer.iHeight;
#endif
        }
        else if(CAM_IMGFMT_YCbCr420P == pSrcImgBuf->eFormat){
            TRACE_D("%s: thumbnail src image is 420P",__FUNCTION__);
            //TODO: HERE we assume all yuv components in jpeg-interleave is 8 bytes align
            memcpy(&stTempBuffer, pSrcImgBuf, sizeof(CAM_ImageBuffer));
        }

        //convert form yuv 422 to JPEG
        if( CAM_IMGFMT_JPEG == pDstImgBuf->eFormat)
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

            stDstBitStream.pBsBuffer        = (Ipp8u*)pDstImgBuf->pBuffer[0];
            stDstBitStream.bsByteLen        = pDstImgBuf->iAllocLen[0];
            stDstBitStream.pBsCurByte       = (Ipp8u*)pDstImgBuf->pBuffer[0];
            stDstBitStream.bsCurBitOffset   = 0;

            stEncoderPar.nQuality          = iQualityFactor;
            stEncoderPar.nRestartInterval  = 0;
            stEncoderPar.nJPEGMode         = JPEG_BASELINE;
            stEncoderPar.nSubsampling      = JPEG_422 ;
            stEncoderPar.nBufType          = JPEG_INTEGRATEBUF;
            stEncoderPar.pSrcPicHandler    = (void*)NULL;
            stEncoderPar.pStreamHandler    = (void*)NULL;

            if ( miscInitGeneralCallbackTable(&pCallBackTable) != 0 )
            {
                TRACE_E("miscInitGeneralCallbackTable failed");
                return ippStsNoMemErr;
            }

            eRetCode = EncoderInitAlloc_JPEG( &stEncoderPar, &stSrcPicture, &stDstBitStream, pCallBackTable, &pEncoderState );

            if ( IPP_STATUS_NOERR != eRetCode )
            {
                TRACE_E("EncoderInitAlloc_JPEG failed, %d",eRetCode);
                return ippStsErr;
            }

            eRetCode = Encode_JPEG( &stSrcPicture, &stDstBitStream, pEncoderState );

            if ( IPP_STATUS_NOERR != eRetCode )
            {
                TRACE_E("Encode_JPEG failed, %d",eRetCode);
                EncoderFree_JPEG( &pEncoderState );
                miscFreeGeneralCallbackTable( &pCallBackTable );
                return ippStsErr;
            }

            pDstImgBuf->iFilledLen[0] = (Ipp32s)stDstBitStream.pBsCurByte - (Ipp32s)stDstBitStream.pBsBuffer;
            pDstImgBuf->iFilledLen[1] = 0;
            pDstImgBuf->iFilledLen[2] = 0;

            TRACE_D("THE THUMBNAIL JPEG LENGTH IS %d\n",pDstImgBuf->iFilledLen[0] );
            EncoderFree_JPEG( &pEncoderState );
            miscFreeGeneralCallbackTable( &pCallBackTable );

        }
        if(NULL != pHeap){
            free(pHeap);
            pHeap = NULL;
        }

        switch ( pDstImgBuf->eFormat )
        {
            case CAM_IMGFMT_YCbCr420P:

                pDstImgBuf->iFilledLen[0] = pDstImgBuf->iStep[0] * pDstImgBuf->iHeight;
                pDstImgBuf->iFilledLen[1] = pDstImgBuf->iStep[1] * pDstImgBuf->iHeight >> 1;
                pDstImgBuf->iFilledLen[2] = pDstImgBuf->iStep[2] * pDstImgBuf->iHeight >> 1;
                break;

            case CAM_IMGFMT_YCbCr422P:

                pDstImgBuf->iFilledLen[0] = pDstImgBuf->iStep[0] * pDstImgBuf->iHeight;
                pDstImgBuf->iFilledLen[1] = pDstImgBuf->iStep[1] * pDstImgBuf->iHeight;
                pDstImgBuf->iFilledLen[2] = pDstImgBuf->iStep[2] * pDstImgBuf->iHeight;
                break;

            case CAM_IMGFMT_RGB888 :

                pDstImgBuf->iFilledLen[0] = pDstImgBuf->iStep[0] * pDstImgBuf->iHeight ;
                pDstImgBuf->iFilledLen[1] = 0;
                pDstImgBuf->iFilledLen[2] = 0;
                break;

            default:
                break;
        }

        return ippStsNoErr;
    }

    bool updateExifInfo(IppExifInfo *pExifInfo,const CameraSetting& setting,
            const CAM_ImageBuffer* const pImgBuf_still, const CAM_ImageBuffer* const pThumbnailBuf)
    {
        IppExif32uRational  urXResolutionDefault = {72, 1};
        IppExif32uRational  urYResolutionDefault = {72, 1};
        IppExif32uRational  urFocalLength        = {400,4000,}; //
        int                 iExifW               = 0;
        int                 iExifH               = 0;
        CAM_ShotModeCapability cShotModeCapability = setting.getSupportedSceneModeCap();

        time_t rawtime;
        struct tm *timeinfo;
        time(&rawtime);
        timeinfo = localtime(&rawtime);

        setting.getPictureSize(&iExifW, &iExifH);

        // pExifInfo->eCPUEndian = ippExifLittleEndian;
        strncpy((char *)pExifInfo->stTiffInfo.ucImageDescription,"tiff infomation", MAX_IMAGE_NAME);
        strncpy((char *)pExifInfo->stTiffInfo.ucMake,"marvell", MAX_MAKE_NAME);
        strncpy((char *)pExifInfo->stTiffInfo.ucModel, setting.getProductName(), MAX_MODEL_NAME);

        pExifInfo->stTiffInfo.urXResolution     = urXResolutionDefault;
        pExifInfo->stTiffInfo.urYResolution     = urYResolutionDefault;
        pExifInfo->stTiffInfo.eResolutionUnit   = ippExifResolutionUnitInch;
        pExifInfo->stTiffInfo.eYCbCrPosition    = ippExifYCbCrPosCentral;

        strncpy((char *)pExifInfo->stTiffInfo.ucSoftware, "N/A", MAX_SOFTWARE_LEN);

        strftime((char *)pExifInfo->stTiffInfo.ucDataTime, TIME_DEFAULT_LEN, "%Y:%m:%d %H:%M:%S", timeinfo);

        pExifInfo->stDCFInteropbInfo.eInteropbIndex = ippExifInteropbR98;
        pExifInfo->stDCFInteropbInfo.eInteropbVersion = ippDCFInteropbVersion1;

        strftime((char *)pExifInfo->stExifPrivateTagInfo.ucDateTimeOriginal, TIME_DEFAULT_LEN, "%Y:%m:%d %H:%M:%S", timeinfo);
        strftime((char *)pExifInfo->stExifPrivateTagInfo.ucDateTimeDigitized, TIME_DEFAULT_LEN, "%Y:%m:%d %H:%M:%S", timeinfo);
        pExifInfo->stExifPrivateTagInfo.eComponentConfiguration = ippExifComponentConfigYCbCr;
        pExifInfo->stExifPrivateTagInfo.eColorSpace             = ippExifColorSpaceUnCalibrated; //default
        pExifInfo->stExifPrivateTagInfo.urFocalLength           = urFocalLength;
        pExifInfo->stExifPrivateTagInfo.uiPixelXDimention       = iExifW;
        pExifInfo->stExifPrivateTagInfo.uiPixelYDimention       = iExifH;
        pExifInfo->stExifPrivateTagInfo.eExposureMode           = ippExifExposureModeManual;
        //strcpy(pExifInfo->stExifPrivateTagInfo.cExifVersion,"0220");//default
        //strcpy(pExifInfo->stExifPrivateTagInfo.cFlashPixelVersion,"0100");//default
        pExifInfo->stExifPrivateTagInfo.eCustomRendered        = ippExifCustomRenderedNormal;
        pExifInfo->stExifPrivateTagInfo.eSubjectDistanceRange  = ippExifSDRUnknow;

        pExifInfo->stExifPrivateTagInfo.eSceneType              = ippExifSceneTypeDirectly;
        pExifInfo->stExifPrivateTagInfo.eFileSource             = ippExifFileSourceDirectly;
        pExifInfo->stExifPrivateTagInfo.eLightSource            = ippExifLightSourceUnKnown;
        pExifInfo->stExifPrivateTagInfo.usFocalLenIn35mmFilm    = 0;  // FIXME: pImgBuf_still->stShotInfo.iFocalLen35mm;
        pExifInfo->stExifPrivateTagInfo.eSensingMethod          = ippExifSMOneSensor;

        int iZoom = setting.getInt(CameraParameters::KEY_ZOOM);
        pExifInfo->stExifPrivateTagInfo.urDigitalZoomRatio.numerator    = cShotModeCapability.iMinDigZoomQ16 + iZoom * cShotModeCapability.iDigZoomStepQ16;
        pExifInfo->stExifPrivateTagInfo.urDigitalZoomRatio.denominal    = 65536;
        TRACE_V("EXIF information: DigitalZoomRatio: %.2f", (cShotModeCapability.iMinDigZoomQ16 + iZoom * cShotModeCapability.iDigZoomStepQ16) / 65536.0);

        char  pMakerNote[] = "none";
        pExifInfo->stExifPrivateTagInfo.pMakerNote               =(Ipp8u *) pMakerNote;
        pExifInfo->stExifPrivateTagInfo.uiMakerNoteLen           = strlen(pMakerNote );

        // spec2.2, annex C, APEX is a convenient unit for expressing exposure (Ev).
        // ApertureValue (Av) = 2 log2 (F number)
        // ShutterSpeedValue (Tv) = - log2 (exposure time)
        // BrightnessValue (Bv) = log2 ( B/NK ) Note that: B:cd/cm2, N,K: constant
        // Film sensitivity (Sv) = log2 ( ASA / 3.125 )
        // Ev = Av + Tv = Bv + Sv

        if (pImgBuf_still->bEnableShotInfo)
        {

            //EXIF exporsure time
            pExifInfo->stExifPrivateTagInfo.urExposureTime.numerator        = pImgBuf_still->stShotInfo.iExposureTimeQ16;
            pExifInfo->stExifPrivateTagInfo.urExposureTime.denominal        = 65536;

            //IppExifPrivateTagInfo.urFNumber
            pExifInfo->stExifPrivateTagInfo.urFNumber.numerator             = pImgBuf_still->stShotInfo.iFNumQ16;
            pExifInfo->stExifPrivateTagInfo.urFNumber.denominal             = 65536;

            uint32_t Av, Tv, Bv, Sv, Ev;

            double Fnumber              = (double)pExifInfo->stExifPrivateTagInfo.urFNumber.numerator / pExifInfo->stExifPrivateTagInfo.urFNumber.denominal;
            double ExposureTime         = (double)pExifInfo->stExifPrivateTagInfo.urExposureTime.numerator / pExifInfo->stExifPrivateTagInfo.urExposureTime.denominal; 
            uint16_t IsoSpeedRating     = 100;

            Ipp16u pISOSpeed[]                    = {100}; //FIXME: pImgBuf_still->stShotInfo.iISOSpeed;
            pExifInfo->stExifPrivateTagInfo.pISOSpeedRating        = pISOSpeed;
            pExifInfo->stExifPrivateTagInfo.uiISOSpeedRatingCnt    = sizeof(pISOSpeed)/sizeof(Ipp16u);
#define _LOG2(x) (log((double)(x)) / log(2.0))
            Av = (int)(2 * _LOG2((double)(Fnumber)) + 0.5);
            Tv = ExposureTime >= 1 ? (int)(-_LOG2(ExposureTime) - 0.5) : (int)(-_LOG2(ExposureTime) + 0.5);
            Sv = ((int)(_LOG2((IsoSpeedRating) / 3.125) + 0.5));  // Film sensitivity (Sv) = log2 ( ASA / 3.125 )
            Bv = Av + Tv - Sv;
            Ev = Av + Tv;

            TRACE_D("%s: EXIF information: Ev : %d, Av : %d, Tv : %d",__FUNCTION__, Ev, Av, Tv);


            // ApertureValue (Av) = 2 log2 (F number)
            pExifInfo->stExifPrivateTagInfo.urApertureValue.numerator       = Av * 65536;
            pExifInfo->stExifPrivateTagInfo.urApertureValue.denominal       = 65536;

            // ShutterSpeedValue (Tv) = - log2 (exposure time)
            pExifInfo->stExifPrivateTagInfo.srShutterSpeed.inumerator       = Tv * 65536;
            pExifInfo->stExifPrivateTagInfo.srShutterSpeed.idenominal       = 65536;
            //ShutterSpeedValue

            // BrightnessValue (Bv) = log2 ( B/NK )
            pExifInfo->stExifPrivateTagInfo.srBrightnessValue.inumerator  = Bv * 65536;
            pExifInfo->stExifPrivateTagInfo.srBrightnessValue.idenominal  = 65536;

            //IppExifPrivateTagInfo.srExposureBiaValue  // SCENE_MODE_BEACH_SNOW ---> 65536/65536
            // Ev = Av + Tv = Bv + Sv
            pExifInfo->stExifPrivateTagInfo.srExposureBiaValue.inumerator   = 0; // TBD
            pExifInfo->stExifPrivateTagInfo.srExposureBiaValue.idenominal   = 65536;

            pExifInfo->stExifPrivateTagInfo.urMaxApertureValue.numerator    = (int)(2 * _LOG2((double)(cShotModeCapability.iMinFNumQ16 >> 16)) + 0.5) << 16; // FAKE: 194698;
            pExifInfo->stExifPrivateTagInfo.urMaxApertureValue.denominal    = 65536;
            TRACE_V("EXIF information: MaxApertureValue: %.2f", pExifInfo->stExifPrivateTagInfo.urMaxApertureValue.numerator / 65536.0);

            pExifInfo->stExifPrivateTagInfo.eExposureProgram                = ippExifExposureProgramNotDefined;
            pExifInfo->stExifPrivateTagInfo.eMeteringMode                   = ippExifMeteringModeCenterWeightedAverage; //
            pExifInfo->stExifPrivateTagInfo.eFlash              = ippExifFlashNotFired;
            pExifInfo->stExifPrivateTagInfo.eContrast           = ippExifContrastNormal;
            pExifInfo->stExifPrivateTagInfo.eSaturation         = ippExifSaturationNormal;
            pExifInfo->stExifPrivateTagInfo.eSharpness          = ippExifSharpnessNormal;
            pExifInfo->stExifPrivateTagInfo.eSceneCaptureType   = ippExifSceneCaptureTypeStandard;
        }

        //for GPS information
        float latitude      = setting.getFloat(CameraParameters::KEY_GPS_LATITUDE);
        float longitude     = setting.getFloat(CameraParameters::KEY_GPS_LONGITUDE);
        int altitude        = setting.getInt(CameraParameters::KEY_GPS_ALTITUDE);
        int timestamp       = setting.getInt(CameraParameters::KEY_GPS_TIMESTAMP);

        const char *gpsvalue  = setting.get(CameraParameters::KEY_GPS_LATITUDE);
        const char *gpstmp    = gpsvalue;

        if (0 != gpsvalue)
        {
            memmove( pExifInfo->stGPSInfo.ucGPSMapDatum, "TOKYO", strlen("TOKYO") + 1);
            /*
               D = TRUNC(d)
               M = TRUNC((d - D) * 60)
               s = (d - D - M/60) * 3600 = (d - D) * 3600 - M * 60
               */
            pExifInfo->stGPSInfo.eGPSLatituteRef            = (latitude > 0 ) ? ippExifGPSLatituteRefNorth : ippExifGPSLatituteRefSouth;
            pExifInfo->stGPSInfo.eGPSLongtituteRef          = (longitude > 0) ? ippExifGPSLongtituteRefEast : ippExifGPSLongtituteRefWest;
	     pExifInfo->stGPSInfo.eGPSAltitudeRef            = (altitude > 0) ? ippExifAltitudeRefSeaLevel : ippExifAltitudeRefSeaLevelNeg;

            double fLatitude    = setting.getFloat(CameraParameters::KEY_GPS_LATITUDE);
            long lLatitude      = (long)(fLatitude * 10000 / 1);
            double fLongitude   = setting.getFloat(CameraParameters::KEY_GPS_LONGITUDE);
            long lLongitude     = (long)(fLongitude * 10000 / 1);
            double fAltitude    = setting.getFloat(CameraParameters::KEY_GPS_ALTITUDE);
            long lAltitude      = (long)(fAltitude * 100 / 1);
            double latitude     = fabs(lLatitude / 10000.0);
            double longitude    = fabs(lLongitude / 10000.0);
            double altitude     = fabs(lAltitude / 100.0);

            pExifInfo->stGPSInfo.urGPSLatitute[0].numerator     = (uint32_t)latitude;
            pExifInfo->stGPSInfo.urGPSLatitute[0].denominal     = 1;
            pExifInfo->stGPSInfo.urGPSLatitute[1].numerator     = (uint32_t)((latitude - pExifInfo->stGPSInfo.urGPSLatitute[0].numerator) * 60);
            pExifInfo->stGPSInfo.urGPSLatitute[1].denominal     = 1;
            pExifInfo->stGPSInfo.urGPSLatitute[2].numerator     = (uint32_t)((((latitude - pExifInfo->stGPSInfo.urGPSLatitute[0].numerator) * 60)
                        - pExifInfo->stGPSInfo.urGPSLatitute[1].numerator) * 60);
            pExifInfo->stGPSInfo.urGPSLatitute[2].denominal     = 1;

            pExifInfo->stGPSInfo.urGPSLongtitute[0].numerator   = (uint32_t)longitude;
            pExifInfo->stGPSInfo.urGPSLongtitute[0].denominal   = 1;
            pExifInfo->stGPSInfo.urGPSLongtitute[1].numerator   = (uint32_t)((longitude - pExifInfo->stGPSInfo.urGPSLongtitute[0].numerator) * 60);
            pExifInfo->stGPSInfo.urGPSLongtitute[1].denominal   = 1;
            pExifInfo->stGPSInfo.urGPSLongtitute[2].numerator   = (uint32_t)((((longitude - pExifInfo->stGPSInfo.urGPSLongtitute[0].numerator) * 60)
                        - pExifInfo->stGPSInfo.urGPSLongtitute[1].numerator) * 60);
            pExifInfo->stGPSInfo.urGPSLongtitute[2].denominal   = 1;

            pExifInfo->stGPSInfo.urGPSAltitude.numerator     = (uint32_t)altitude;
            pExifInfo->stGPSInfo.urGPSAltitude.denominal     = 1;

            //time stamp
            struct tm tmTimestamp;
            const char *sTimestamp = setting.get(CameraParameters::KEY_GPS_TIMESTAMP);
            long lTimestamp = atol(sTimestamp);
            gmtime_r(&lTimestamp, &tmTimestamp);
            pExifInfo->stGPSInfo.urGPSTimeStamp[0].numerator = tmTimestamp.tm_hour;
            pExifInfo->stGPSInfo.urGPSTimeStamp[0].denominal = 1;
            pExifInfo->stGPSInfo.urGPSTimeStamp[1].numerator = tmTimestamp.tm_min;
            pExifInfo->stGPSInfo.urGPSTimeStamp[1].denominal = 1;
            pExifInfo->stGPSInfo.urGPSTimeStamp[2].numerator = tmTimestamp.tm_sec;
            pExifInfo->stGPSInfo.urGPSTimeStamp[2].denominal = 1;
            snprintf((char*)pExifInfo->stGPSInfo.urGPSTimeStamp, sizeof(pExifInfo->stGPSInfo.urGPSTimeStamp),
                    "%04d:%02d:%02d", tmTimestamp.tm_year, tmTimestamp.tm_mon, tmTimestamp.tm_mday);

            //process method
            const char *tmpProcessMethod = setting.get(CameraParameters::KEY_GPS_PROCESSING_METHOD);
            pExifInfo->stGPSInfo.pGPSProcessMethod =(Ipp8u*) tmpProcessMethod;
            pExifInfo->stGPSInfo.uiGPSProcessMethodLen = strlen(tmpProcessMethod);
            //strncpy((char *)pExifInfo->stGPSInfo.cGPSProcessMethod, tmpProcessMethod, sizeof(pExifInfo->stGPSInfo.cGPSProcessMethod));   // MAX_GPS_PROCESS_METHOD

            //date stamp
            time_t rawtime;
            struct tm *timeinfo;
            time(&rawtime);
            timeinfo = localtime(&rawtime);
            strftime((char *)pExifInfo->stGPSInfo.ucGPSDateStamp, 11, "%Y:%m:%d %H:%M:%S", timeinfo);
        }

        //map exif rotation
        const char* v = setting.get(CameraParameters::KEY_ROTATION);
        if (0 != v){
            TRACE_D("%s:KEY_ROTATION: %s", __FUNCTION__, v);
            int rotation = setting.map_andr2ce(CameraSetting::map_exifrotation, v);
            if (-1 != rotation){
                pExifInfo->stTiffInfo.eOrientation = (IppExifOrientation)rotation;
                TRACE_D("%s:Exif eOrientation: %d", __FUNCTION__, rotation);
            }
        }

        //map exif whitebalance
        v = (char *)setting.get(CameraParameters::KEY_WHITE_BALANCE);
        if (0 != v){
            TRACE_D("%s:KEY_WHITE_BALANCE: %s", __FUNCTION__, v);
            int whitebalance = setting.map_andr2ce(CameraSetting::map_exifwhitebalance, v);
            if (-1 != whitebalance){
                pExifInfo->stExifPrivateTagInfo.eWhiteBalance = (IppExifWhiteBalance)whitebalance;
                TRACE_D("%s:Exif eWhiteBalance: %d", __FUNCTION__, whitebalance);
            }
        }

        if (0 == strcmp(setting.getSensorName(), "ov5642"))
        {
            // override the following values depends on diffenrent sensor.
#if 0
            IppExif32uRational  urXResolutionDefault = {72, 1};
            IppExif32uRational  urYResolutionDefault = {72, 1};
            IppExif32uRational  urFocalLength={400,4000,}; //
            pExifInfo->stTiffInfo.urXResolution = urXResolutionDefault;
            pExifInfo->stTiffInfo.urYResolution = urYResolutionDefault;
            pExifInfo->stExifPrivateTagInfo.urFocalLength = urFocalLength;
#endif
        }


        /*
         * **************************************************
         * Finally, update some thumbnail related info.
         * **************************************************
         */
        // not effected by stTiffInfo.eOrientation
        if(NULL ==pThumbnailBuf){
            TRACE_E("%s:NO thumbnail info available, skip thumbnail info in exif",__FUNCTION__);
        }
        else{
            pExifInfo->stThumbnailInfo.eOrientation = ippExifOrientationNormal;
            pExifInfo->stThumbnailInfo.pData = pThumbnailBuf->pBuffer[0];
            pExifInfo->stThumbnailInfo.iDataLen = pThumbnailBuf->iFilledLen[0] +
                pThumbnailBuf->iFilledLen[1] + pThumbnailBuf->iFilledLen[2];

            switch( pThumbnailBuf->eFormat )
            {
                case CAM_IMGFMT_YCbCr420P:

                    pExifInfo->stThumbnailInfo.eCompression = ippExifCompressionUnCompressed;
                    pExifInfo->stThumbnailInfo.ePhotometricInterpretation = ippExifPhotometricInterpretationYCC;
                    pExifInfo->stThumbnailInfo.eResolutionUnit = ippExifResolutionUnitInch;
                    pExifInfo->stThumbnailInfo.stRawThumbAttribute.uiHeight = pThumbnailBuf->iHeight;
                    pExifInfo->stThumbnailInfo.stRawThumbAttribute.uiWidth = pThumbnailBuf->iWidth;
                    pExifInfo->stThumbnailInfo.stRawThumbAttribute.ePlanarConfig = ippExifPlanarConfigPlanar;
                    pExifInfo->stThumbnailInfo.stRawThumbAttribute.eYCCSubSampling = ippExifYCCSubSampling420;
                    break;

                case CAM_IMGFMT_YCbCr422P:

                    pExifInfo->stThumbnailInfo.eCompression = ippExifCompressionUnCompressed;
                    pExifInfo->stThumbnailInfo.ePhotometricInterpretation = ippExifPhotometricInterpretationYCC;
                    pExifInfo->stThumbnailInfo.eResolutionUnit = ippExifResolutionUnitInch ;
                    pExifInfo->stThumbnailInfo.stRawThumbAttribute.uiHeight = pThumbnailBuf->iHeight;
                    pExifInfo->stThumbnailInfo.stRawThumbAttribute.uiWidth = pThumbnailBuf->iWidth;
                    pExifInfo->stThumbnailInfo.stRawThumbAttribute.ePlanarConfig = ippExifPlanarConfigPlanar;
                    pExifInfo->stThumbnailInfo.stRawThumbAttribute.eYCCSubSampling = ippExifYCCSubSampling422;
                    break;

                case CAM_IMGFMT_JPEG:

                    pExifInfo->stThumbnailInfo.eCompression = ippExifCompressionThumbnailJPEG;
                    break;

                case CAM_IMGFMT_RGB888 :

                    pExifInfo->stThumbnailInfo.eCompression = ippExifCompressionUnCompressed;
                    pExifInfo->stThumbnailInfo.ePhotometricInterpretation = ippExifPhotometricInterpretationRGB;
                    pExifInfo->stThumbnailInfo.eResolutionUnit = ippExifResolutionUnitInch ;
                    pExifInfo->stThumbnailInfo.stRawThumbAttribute.uiHeight = pThumbnailBuf->iHeight;
                    pExifInfo->stThumbnailInfo.stRawThumbAttribute.uiWidth = pThumbnailBuf->iWidth;
                    pExifInfo->stThumbnailInfo.stRawThumbAttribute.ePlanarConfig = ippExifPlanarConfigChunky;
                    break;

                default:
                    break;
            }
        }
        return NO_ERROR;
    }


    //ThumbnailBuf: which will contain the pointer to thumbnail buffer
    //thumbmem: the external allocated buffer to store thumbnail
    //thumblen: buffer length of thumbmem
    //pThumbnail_SrcImg: raw jpeg image
    status_t getThumbnail(CAM_ImageBuffer& ThumbnailBuf, unsigned char* const thumbmem, int thumblen,
            const CAM_ImageBuffer& Thumbnail_SrcImg, const CameraSetting& setting)
    {
        int minlen=ThumbnailBuf.iWidth * ThumbnailBuf.iHeight * 3 + 128;
        if(thumblen < minlen){
            TRACE_E("%s:ERROR: memlen NOT enough, input=%d, minlen=%d",__FUNCTION__, thumblen, minlen);
            return BAD_VALUE;
        }

        Ipp32s iQualityFactor = setting.getParameters().getInt(
                CameraParameters::KEY_JPEG_THUMBNAIL_QUALITY);

        status_t error = NO_ERROR;

        switch( ThumbnailBuf.eFormat )
        {
            case CAM_IMGFMT_YCbCr420P:
                ThumbnailBuf.iStep[0] = ThumbnailBuf.iWidth;
                ThumbnailBuf.iStep[1] = ThumbnailBuf.iWidth >> 1;
                ThumbnailBuf.iStep[2] = ThumbnailBuf.iWidth >> 1;

                ThumbnailBuf.pBuffer[0] = (Ipp8u *)_ALIGN_TO( thumbmem, 8 );
                ThumbnailBuf.pBuffer[1] = ThumbnailBuf.pBuffer[0] +
                    ThumbnailBuf.iStep[0] * ThumbnailBuf.iHeight;
                ThumbnailBuf.pBuffer[2] = ThumbnailBuf.pBuffer[1] +
                    ThumbnailBuf.iStep[1] * ( ThumbnailBuf.iHeight >> 1 );
                break;

            case CAM_IMGFMT_YCbCr422P:
                ThumbnailBuf.iStep[0] = ThumbnailBuf.iWidth;
                ThumbnailBuf.iStep[1] = ThumbnailBuf.iWidth >> 1;
                ThumbnailBuf.iStep[2] = ThumbnailBuf.iWidth >> 1;

                ThumbnailBuf.pBuffer[0] = (Ipp8u *)_ALIGN_TO( thumbmem, 8 );
                ThumbnailBuf.pBuffer[1] = ThumbnailBuf.pBuffer[0] +
                    ThumbnailBuf.iStep[0] * ThumbnailBuf.iHeight;
                ThumbnailBuf.pBuffer[2] = ThumbnailBuf.pBuffer[1] +
                    ThumbnailBuf.iStep[1] * ThumbnailBuf.iHeight;
                break;

            case CAM_IMGFMT_JPEG:
                ThumbnailBuf.pBuffer[0] = (Ipp8u *)_ALIGN_TO( thumbmem, 8 );
                ThumbnailBuf.iAllocLen[0] = ThumbnailBuf.iWidth * ThumbnailBuf.iHeight * 3 ;
                iQualityFactor  = 60;

                //TODO: delete JPEG thumbnail APP tag.
                /*
                 * the exif thumnail need pure bit stream ,do not contain any APP tag,
                 * just from FFD8 to FFD9, but ipp_jpeg_thumbnail function generate full jpeg image,
                 * so if want to be precise, need to delete jpeg thumbnail APP tag
                 */
                break;

            case CAM_IMGFMT_RGB888 :
                ThumbnailBuf.iStep[0] = ThumbnailBuf.iWidth * 3;
                ThumbnailBuf.pBuffer[0] = (Ipp8u *)_ALIGN_TO( thumbmem, 8 );
                break;
            default:
                TRACE_E("%s:ERROR:unsupported thumbnail format:%d",__FUNCTION__, ThumbnailBuf.eFormat);
                return BAD_VALUE;
        }
        IppStatus ret = _ipp_jpeg_thumbnail(&Thumbnail_SrcImg, &ThumbnailBuf, iQualityFactor);
        if (ippStsNoErr != ret){
            TRACE_E("%s:ERROR:_ipp_jpeg_thumbnail failed, %d",__FUNCTION__,ret);
            return UNKNOWN_ERROR;
        }
        return NO_ERROR;
    }


    sp<MemoryBase> ExifGenerator(const CAM_ImageBuffer* const pImgBuf_still, const CameraSetting& setting)
    {

        const CAM_ImageBuffer* pThumbnail_SrcImg;

#undef SUPPORT_INTERLEAVE_JPEG
#ifdef SUPPORT_INTERLEAVE_JPEG

#if 0
        //for debug purpose only
        //use jpeg-yuv-interleave image.
        CAM_ImageBuffer* inputimg = const_cast<CAM_ImageBuffer*>(pImgBuf_still);
        CAM_ImageBuffer interleaveimg;

        int w = 176, h = 144;
        memset(&interleaveimg, 0, sizeof(CAM_ImageBuffer));

        interleaveimg.eFormat  = (CAM_ImageFormat)CAM_IMGFMT_YCbCr422P;

        interleaveimg.iWidth   = w;
        interleaveimg.iHeight  = h;

        interleaveimg.iStep[0] = w;
        interleaveimg.iStep[1] = w >> 1;
        interleaveimg.iStep[2] = w >> 1;
        interleaveimg.iAllocLen[0] = h * interleaveimg.iStep[0];
        interleaveimg.iAllocLen[1] = h * interleaveimg.iStep[1];
        interleaveimg.iAllocLen[2] = h * interleaveimg.iStep[2];
        interleaveimg.iFilledLen[0] = h * interleaveimg.iStep[0];
        interleaveimg.iFilledLen[1] = h * interleaveimg.iStep[1];
        interleaveimg.iFilledLen[2] = h * interleaveimg.iStep[2];

        interleaveimg.pBuffer[0] = inputimg->pBuffer[1];
        interleaveimg.pBuffer[1] = interleaveimg.pBuffer[0] + interleaveimg.iFilledLen[0];
        interleaveimg.pBuffer[2] = interleaveimg.pBuffer[1] + interleaveimg.iFilledLen[1];

        pThumbnail_SrcImg = &interleaveimg;
#endif

        TRACE_D("%s:format=%d, interleave=%d",__FUNCTION__, pImgBuf_still->eFormat, pImgBuf_still->bEnableInterleave);
        if(CAM_IMGFMT_JPEG == pImgBuf_still->eFormat && CAM_TRUE == pImgBuf_still->bEnableInterleave){
            pThumbnail_SrcImg = static_cast<const CAM_ImageBuffer*>(pImgBuf_still->pInterleaveImgBuf);
            TRACE_D("%s:use interleave image for thumbnail",__FUNCTION__);
        }
        else{
#endif
            pThumbnail_SrcImg = pImgBuf_still;
            TRACE_D("%s:use origin image for thumbnail",__FUNCTION__);
#ifdef SUPPORT_INTERLEAVE_JPEG
        }
#endif

        TRACE_V("fmt=%d,w=%d,h=%d,step=%d,%d,%d,alloclen=%d,%d,%d,filledlen=%d,%d,%d,pBuffer=%x,%x,%x",
                pThumbnail_SrcImg->eFormat,pThumbnail_SrcImg->iWidth,pThumbnail_SrcImg->iHeight,
                pThumbnail_SrcImg->iStep[0],pThumbnail_SrcImg->iStep[1],pThumbnail_SrcImg->iStep[2],
                pThumbnail_SrcImg->iAllocLen[0],pThumbnail_SrcImg->iAllocLen[1],pThumbnail_SrcImg->iAllocLen[2],
                pThumbnail_SrcImg->iFilledLen[0],pThumbnail_SrcImg->iFilledLen[1],pThumbnail_SrcImg->iFilledLen[2],
                pThumbnail_SrcImg->pBuffer[0],pThumbnail_SrcImg->pBuffer[1],pThumbnail_SrcImg->pBuffer[2]);

        //step 3:prepare to generate thumbnail
        //source image buffer which contains raw jpeg pic, without exif info
        int srcImgBuf_len = pImgBuf_still->iFilledLen[0]; //the src raw jpeg length
        //+ pImgBuf_still->iFilledLen[1] + pImgBuf_still->iFilledLen[2];

        //step 1: generate thumbnail
        CAM_ImageBuffer ThumbnailBuf;//contain info for thubmnail buffer
        memset(&ThumbnailBuf, 0, sizeof(CAM_ImageBuffer));

        //TODO:here we simply hardcoding to JPEG format.
        ThumbnailBuf.eFormat = CAM_IMGFMT_JPEG;
#if 1
        ThumbnailBuf.iWidth = setting.getParameters().getInt(
                CameraParameters::KEY_JPEG_THUMBNAIL_WIDTH);
        ThumbnailBuf.iHeight = setting.getParameters().getInt(
                CameraParameters::KEY_JPEG_THUMBNAIL_HEIGHT);
#else //hardcoding for debug
        ThumbnailBuf.iWidth = 176;
        ThumbnailBuf.iHeight = 144;
#endif

        Ipp32s iQualityFactor = setting.getParameters().getInt(
                CameraParameters::KEY_JPEG_THUMBNAIL_QUALITY);

        Ipp8u* thumbmem = NULL;//ptr to thumbnail image
        bool thumbinfo_valid = true;

        TRACE_D("%s:Thumbnail parameters: w=%d,h=%d,Q=%d",__FUNCTION__,ThumbnailBuf.iWidth, ThumbnailBuf.iHeight, iQualityFactor);
        if(ThumbnailBuf.iWidth <= 0 || ThumbnailBuf.iHeight <= 0 ||
                iQualityFactor <= 0 || iQualityFactor > 100){
            thumbinfo_valid = false;
            TRACE_E("%s:Invalid thumbnail parameters, skip thumbnail",__FUNCTION__);
        }
        else{
            //prepare buffer to contain thumbnail
            //TODO:here we assume required buflen less than (w*h*3+128)
            //Though it's a little ugle, but it could make the code more clear
            int thumblen=ThumbnailBuf.iWidth * ThumbnailBuf.iHeight * 3 + 128;
            thumbmem = (Ipp8u *)malloc(thumblen);
            if ( thumbmem == NULL ){
                thumbinfo_valid = false;
                TRACE_E("ERROR: fail to alloc thumbnail buffers");
            }
            else{
                status_t res = getThumbnail(ThumbnailBuf, thumbmem, thumblen, *pThumbnail_SrcImg, setting);
                if(NO_ERROR != res ){
                    thumbinfo_valid = false;
                    TRACE_E("%s:ERROR:getThumbnail fail",__FUNCTION__);
                }
            }
        }

        //step 2: update exif info, to cExifInfo structure
        //initialize default exif info
        IppExifInfo cExifInfo = IPPExif_Default_ExifInfo;

        if( true == thumbinfo_valid ){
            updateExifInfo(&cExifInfo, setting, pImgBuf_still, &ThumbnailBuf);
        }
        else{
            updateExifInfo(&cExifInfo, setting, pImgBuf_still, NULL);
        }

        //step 3:update buffer requirement with exifinfo and thumbnail
        IppExifBufReq cBufReq;
        if( ippStsNoErr != ippExifGetBufReq(&cExifInfo, &cBufReq)){
            TRACE_E("%s:ERROR:ippExifGetBufReq fail",__FUNCTION__);
            if(NULL != thumbmem){
                free(thumbmem);
                thumbmem = NULL;
            }
            return NULL;
        }

        sp<MemoryHeapBase> exif_heap=NULL;
        sp<MemoryBase> exif_mem=NULL;
        ssize_t offset = 0;
        size_t size = 0;
        int heap_size = 0;

        //prepare dest buffer for Camera Image Callback
        exif_heap = new MemoryHeapBase((size_t)cBufReq.iMinBufLen + srcImgBuf_len);
        heap_size = (int)exif_heap->getSize();
        exif_mem = new MemoryBase(exif_heap, 0, heap_size);

        exif_mem->getMemory(&offset, &size);
        uint8_t* ptr_pic = (uint8_t*)exif_heap->base()+offset;
        memset(ptr_pic,0,size);

        //exif dest buffer info
        IppExifImgBuf cDesImg;
        cDesImg.pBuffer=ptr_pic;
        cDesImg.iFilledLen=cBufReq.iMinBufLen + srcImgBuf_len;

        //exif src buffer info
        IppExifImgBuf cSrcImg;
        cSrcImg.pBuffer = pImgBuf_still->pBuffer[0];
        cSrcImg.iFilledLen = srcImgBuf_len;

        if (ippStsNoErr != ippExifWrite(&cSrcImg, &cDesImg, &cExifInfo)){
            TRACE_E("ippExifWrite fail");
            if(NULL != thumbmem){
                free(thumbmem);
                thumbmem = NULL;
            }
            return NULL;
        }

        if(NULL != thumbmem){
            free(thumbmem);
            thumbmem = NULL;
        }
        return exif_mem;
    }

};

