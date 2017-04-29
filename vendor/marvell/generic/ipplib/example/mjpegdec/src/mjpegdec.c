/***************************************************************************************** 
Copyright (c) 2009, Marvell International Ltd. 
All Rights Reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Marvell nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY MARVELL ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL MARVELL BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************************/


#include "mjpegdec.h"
#include "misc.h"
#include "codecJP.h"


int JPEGSubSamplingTab[][2] = 
{
	/* width, height */
	{0, 0}, /* 444  */
	{1, 0}, /* 422, horizontal sub-sampling  */
	{0, 1}, /* 422I,vertical sub-sampling */
	{1, 1}, /* 411  */
};

int JPEGBitsPerPixel[] = 
{
    24, /* JPEG_BGR888 */
    16, /* JPEG_BGR555 */
    16, /* JPEG_BGR565 */
    32, /* JPEG_ABGR */
};


/*************************************************************************************************************
// Name:      appiDecodeJPEGFromAVI
// Description:
//      Decoder JPEG bit streams
// Input Arguments:
//      pBsStart        - Pointer to the start of the bit stream
//      pDstAVIPicture  - Pointer to the AVI picture structure
//      iBsLen          - Length of the bit stream
//      dstFile         - Pointer to the output file 
// Output Arguments:
//      None
// Returns:
//      IPP_STATUS_NOERR     - OK
//      IPP_STATUS_ERR       - Error  
//      IPP_STATUS_NOMEM_ERR - Memory alloc error
// Others:
//      None
*************************************************************************************************************/
IppCodecStatus appiDecodeJPEGFromAVI
                            ( Ipp8u * pBsStart,
                            Ipp32u iBsLen,
                            IppPicture *pDstAVIPicture,
                            void * dstFile)
{
    IppPicture DstPicture;
    IppBitstream SrcBitStream;  
    void * pDecoderState = NULL;
    MiscGeneralCallbackTable *pCallBackTable = NULL;
    int iOutBufSize;
    int nDstWidth, nDstHeight, nChromaSize = 0;
    Ipp8u *pYUVPtr = NULL;
    IppCodecStatus rtCode = IPP_STATUS_NOERR;
    IppJPEGDecoderParam DecoderPar;
    Ipp8u cDHTFlag = 0;

    /* Init callback table */
    if ( miscInitGeneralCallbackTable(&pCallBackTable) != 0 ) {
        return IPP_STATUS_ERR; 
    }

    SrcBitStream.pBsBuffer = pBsStart;
    SrcBitStream.bsByteLen = iBsLen;
    SrcBitStream.pBsCurByte = pBsStart;
    SrcBitStream.bsCurBitOffset = 0;

    DecoderPar.nModeFlag = IPP_JPEG_ROIDEC_STD;
	 DecoderPar.UnionParamJPEG.roiDecParam.iLPCutFreq = IPP_INVALID_LPCUTFREQ;
    DecoderPar.UnionParamJPEG.roiDecParam.srcROI.x = 0;
    DecoderPar.UnionParamJPEG.roiDecParam.srcROI.y = 0;
    DecoderPar.UnionParamJPEG.roiDecParam.srcROI.width = pDstAVIPicture->picWidth;
    DecoderPar.UnionParamJPEG.roiDecParam.srcROI.height = pDstAVIPicture->picHeight;
    DecoderPar.UnionParamJPEG.roiDecParam.nDstWidth = pDstAVIPicture->picWidth;
    DecoderPar.UnionParamJPEG.roiDecParam.nDstHeight = pDstAVIPicture->picHeight;
    DecoderPar.UnionParamJPEG.roiDecParam.nDesiredColor = JPEG_YUV411;
    DecoderPar.UnionParamJPEG.roiDecParam.nAlphaValue = 8;
    DecoderPar.UnionParamJPEG.rotParam.RotMode = IPP_JP_90L;
    DecoderPar.UnionParamJPEG.rotParam.pStreamHandler = NULL;

    /* Init JPEG Decoder */
    rtCode = DecoderInitAlloc_JPEG (&SrcBitStream, &DstPicture, pCallBackTable, &pDecoderState);

    switch(rtCode){			
            case IPP_STATUS_NOERR:
                break;
            case IPP_STATUS_BADARG_ERR:
            case IPP_STATUS_NOMEM_ERR:
            case IPP_STATUS_BITSTREAM_ERR:
            case IPP_STATUS_NOTSUPPORTED_ERR:
            case IPP_STATUS_ERR:
            default:
                if(NULL!=pDecoderState){
                    DecoderFree_JPEG(&pDecoderState);
                    pDecoderState = NULL;
                }
                if(NULL!=pCallBackTable){
                    miscFreeGeneralCallbackTable(&pCallBackTable);
                    pCallBackTable = NULL;
                }
                return IPP_STATUS_ERR;
    }

    switch(DstPicture.picFormat ){
        case JPEG_444:
            DecoderPar.UnionParamJPEG.roiDecParam.nDesiredColor = JPEG_YUV444;
            break;
        case JPEG_422:
            DecoderPar.UnionParamJPEG.roiDecParam.nDesiredColor = JPEG_YUV422;
            break;  
        case JPEG_422I:
            DecoderPar.UnionParamJPEG.roiDecParam.nDesiredColor = JPEG_YUV422I;
            break;
        case JPEG_411  :
            DecoderPar.UnionParamJPEG.roiDecParam.nDesiredColor = JPEG_YUV411;
            break;
    }

    /* JPEG ROI decoder, IPP_JPEG_ROIDEC_STD or IPP_JPEG_ROIDEC_INT */
    if(DecoderPar.nModeFlag == IPP_JPEG_ROIDEC_STD){
        /* It should be rounding to the floor. */
        nDstWidth  = DecoderPar.UnionParamJPEG.roiDecParam.nDstWidth;  
        nDstHeight = DecoderPar.UnionParamJPEG.roiDecParam.nDstHeight;

        /* YUV space */
        if(DstPicture.picChannelNum==3) {
            nChromaSize = (nDstWidth >>JPEGSubSamplingTab[DstPicture.picFormat][0]) * 
                (nDstHeight>>JPEGSubSamplingTab[DstPicture.picFormat][1]);
        } else {
            nChromaSize = 0;
        }

        iOutBufSize = nDstWidth*nDstHeight + nChromaSize*2;

        if(NULL==pYUVPtr ){
            pCallBackTable->fMemCalloc(&pYUVPtr, iOutBufSize, 8);
            if(NULL==pYUVPtr){
                if(NULL!=pDecoderState){
                    DecoderFree_JPEG(&pDecoderState);
                    pDecoderState = NULL;
                }
                if(NULL!=pCallBackTable){
                    miscFreeGeneralCallbackTable(&pCallBackTable);
                    pCallBackTable = NULL;
                }
                return IPP_STATUS_NOMEM_ERR;
            }
        }

        DstPicture.picWidth  = nDstWidth;
        DstPicture.picHeight = nDstHeight;
        DstPicture.picPlaneStep[0] = nDstWidth;
        DstPicture.ppPicPlane[0]   = pYUVPtr;
        if(DstPicture.picChannelNum==3) {
            DstPicture.picPlaneStep[1] = (nDstWidth>>JPEGSubSamplingTab[DstPicture.picFormat][0]);
            DstPicture.picPlaneStep[2] = (nDstWidth>>JPEGSubSamplingTab[DstPicture.picFormat][0]);
            DstPicture.ppPicPlane[1]   = (Ipp8u*)DstPicture.ppPicPlane[0] + nDstWidth*nDstHeight;
            DstPicture.ppPicPlane[2]   = (Ipp8u*)DstPicture.ppPicPlane[1] + nChromaSize;  
        }



        /* Call the JPEG decoder function */      
        rtCode = Decode_JPEG (&SrcBitStream, NULL, &DstPicture, &DecoderPar, pDecoderState);

        switch(rtCode) {
            case IPP_STATUS_OUTPUT_DATA:
                /* Not supported in platform release */
                break;					
            case IPP_STATUS_JPEG_EOF:
                break;
            case IPP_STATUS_NEED_INPUT:
            case IPP_STATUS_BADARG_ERR:
            case IPP_STATUS_NOMEM_ERR:
            case IPP_STATUS_BITSTREAM_ERR:
            case IPP_STATUS_NOTSUPPORTED_ERR:
            case IPP_STATUS_ERR:
            default:
                if(NULL!=pYUVPtr){
                    pCallBackTable->fMemFree(&pYUVPtr);
                    pYUVPtr = NULL;
                }
                if(NULL!=pDecoderState){
                    DecoderFree_JPEG(&pDecoderState);
                    pDecoderState = NULL;
                }
                if(NULL!=pCallBackTable){
                    miscFreeGeneralCallbackTable(&pCallBackTable);
                    pCallBackTable = NULL;
                }
                return rtCode;
        }

        /* Output the YUV data */
        if(NULL!=dstFile){
            IPP_Fwrite(DstPicture.ppPicPlane[0], 1, nDstWidth*nDstHeight + nChromaSize*2, dstFile);
        }

    }

    /* JPEG Free */
    if(NULL!=pYUVPtr){
        pCallBackTable->fMemFree(&pYUVPtr);
        pYUVPtr = NULL;
    }
    if(NULL!=pDecoderState){
        DecoderFree_JPEG(&pDecoderState);
        pDecoderState = NULL;
    }
    if(NULL!=pCallBackTable){
        miscFreeGeneralCallbackTable(&pCallBackTable);
        pCallBackTable = NULL;
    }

    return IPP_STATUS_NOERR;
}

/* EOF */