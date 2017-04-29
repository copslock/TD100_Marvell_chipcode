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

/*************************************************************************************************************
// Name:  appiAVIParseChunkTypeAndSize
// Description:
//      Parse chunk type and chunk size
// Input Arguments:
//      pSrcBitStream - Pointer to the bit stream structure
// Output Arguments:
//      pChunkID      - Pointer to the FOURCC chunkID
//      pChunkSize    - Pointer to the chunk size
// Returns:
//      IPP_STATUS_NOERR	- OK
// Others:
//      None
*************************************************************************************************************/
IppCodecStatus appiAVIParseChunkTypeAndSize
    (IppBitstream   *pSrcBitStream,
    Ipp32u *pChunkID,
    Ipp32u *pChunkSize)
{
    unsigned char cByte0, cByte1, cByte2, cByte3;

    GETFOURCC(pSrcBitStream->pBsCurByte, *pChunkID)
    GET4BYTES(pSrcBitStream->pBsCurByte, *pChunkSize)

    return IPP_STATUS_NOERR;
}

/*************************************************************************************************************
// Name:  appiAVIParseAviHeader
// Description:
//      Parse 'avih' chunk
// Input Arguments:
//      pSrcBitStream  - Pointer to the bit stream structure
// Output Arguments:
//      ppDecoderState - Pointer to a initialized decoder state structure .
// Returns:
//      IPP_STATUS_NOERR	- OK
// Others:
//      None
*************************************************************************************************************/
IppCodecStatus appiAVIParseAviHeader
    (IppBitstream   *pSrcBitStream,
    IppMJPEGFmAVIDecoderState *pDecoderState)
{
    unsigned char cByte0, cByte1, cByte2, cByte3;

    GET4BYTES(pSrcBitStream->pBsCurByte, pDecoderState->pAVIHeader->iMicroSecPerFrame)
    GET4BYTES(pSrcBitStream->pBsCurByte, pDecoderState->pAVIHeader->iMaxBytesPerSec)
    GET4BYTES(pSrcBitStream->pBsCurByte, pDecoderState->pAVIHeader->iPaddGranurity)    
    GET4BYTES(pSrcBitStream->pBsCurByte, pDecoderState->pAVIHeader->iAviHFlags)
    GET4BYTES(pSrcBitStream->pBsCurByte, pDecoderState->pAVIHeader->iTotalFrames)
    GET4BYTES(pSrcBitStream->pBsCurByte, pDecoderState->pAVIHeader->iAviHInitialFrames)
    GET4BYTES(pSrcBitStream->pBsCurByte, pDecoderState->pAVIHeader->iStreams)
    GET4BYTES(pSrcBitStream->pBsCurByte, pDecoderState->pAVIHeader->iAviHSugBufferSize)
    GET4BYTES(pSrcBitStream->pBsCurByte, pDecoderState->pAVIHeader->iWidth)
    GET4BYTES(pSrcBitStream->pBsCurByte, pDecoderState->pAVIHeader->iHeight)
    SKIPBYTES(pSrcBitStream->pBsCurByte, 16)

    return IPP_STATUS_NOERR;
}

/*************************************************************************************************************
// Name:  appiAVIParseStrh
// Description:
//      Parse 'strh' chunk
// Input Arguments:
//      pSrcBitStream  - Pointer to the bit stream structure
// Output Arguments:
//      ppDecoderState - Pointer to a initialized decoder state structure .
// Returns:
//      IPP_STATUS_NOERR	    - OK
//      IPP_STATUS_NOMEM_ERR - no memory
// Others:
//      None
*************************************************************************************************************/
IppCodecStatus appiAVIParseStrh
    (IppBitstream   *pSrcBitStream,
    IppMJPEGFmAVIDecoderState *pDecoderState)
{
    unsigned char cByte0, cByte1, cByte2, cByte3;
    IppAVIStreamHeader  *pAVIStreamHeader = pDecoderState->pAVIStreamHeader[pDecoderState->cStrlListIdx];

    GETFOURCC(pSrcBitStream->pBsCurByte, pAVIStreamHeader->iStreamType)
    GET4BYTES(pSrcBitStream->pBsCurByte, pAVIStreamHeader->iStreamHandler)
    GET4BYTES(pSrcBitStream->pBsCurByte, pAVIStreamHeader->iStreamFlags)
    GET2BYTES(pSrcBitStream->pBsCurByte, pAVIStreamHeader->sPriority)
    GET2BYTES(pSrcBitStream->pBsCurByte, pAVIStreamHeader->sLanguage)
    GET4BYTES(pSrcBitStream->pBsCurByte, pAVIStreamHeader->iStreamInitialFrame)
    GET4BYTES(pSrcBitStream->pBsCurByte, pAVIStreamHeader->iStreamScale)
    GET4BYTES(pSrcBitStream->pBsCurByte, pAVIStreamHeader->iStreamRate)
    GET4BYTES(pSrcBitStream->pBsCurByte, pAVIStreamHeader->iStreamStart)
    GET4BYTES(pSrcBitStream->pBsCurByte, pAVIStreamHeader->iStreamLength)
    GET4BYTES(pSrcBitStream->pBsCurByte, pAVIStreamHeader->iStreamSugBufferSize)
    GET4BYTES(pSrcBitStream->pBsCurByte, pAVIStreamHeader->iQuality)
    GET4BYTES(pSrcBitStream->pBsCurByte, pAVIStreamHeader->iSampleSize)
    GET2BYTES(pSrcBitStream->pBsCurByte, pAVIStreamHeader->rcFrame.left)
    GET2BYTES(pSrcBitStream->pBsCurByte, pAVIStreamHeader->rcFrame.top)
    GET2BYTES(pSrcBitStream->pBsCurByte, pAVIStreamHeader->rcFrame.right)
    GET2BYTES(pSrcBitStream->pBsCurByte, pAVIStreamHeader->rcFrame.bottom)


    if(AVI_VIDS==pAVIStreamHeader->iStreamType){
        if(pSrcBitStream->bsByteLen<pAVIStreamHeader->iStreamSugBufferSize){
            /* the stream buffer is not large enough */
            pDecoderState->iaxMemFree(&pSrcBitStream->pBsBuffer);
            pSrcBitStream->pBsBuffer = NULL;
            pDecoderState->iaxMemAlloc(&pSrcBitStream->pBsBuffer, pAVIStreamHeader->iStreamSugBufferSize, 4);
            if(NULL == pSrcBitStream->pBsBuffer){
                return IPP_STATUS_NOMEM_ERR;
            }
            pSrcBitStream->bsByteLen = pAVIStreamHeader->iStreamSugBufferSize;
        }
    }

    return IPP_STATUS_NOERR;
}

/*************************************************************************************************************
// Name:  appiAVIParseStrf
// Description:
//      Parse 'strf' chunk
// Input Arguments:
//	     pSrcBitStream  - Pointer to the bit stream structure
// Output Arguments:
//	     ppDecoderState - Pointer to a initialized decoder state structure .
// Returns:
//      IPP_STATUS_NOERR	- OK
// Others:
//      None
*************************************************************************************************************/
IppCodecStatus appiAVIParseStrf
    (IppBitstream   *pSrcBitStream,
    IppMJPEGFmAVIDecoderState *pDecoderState)
{
    unsigned char cByte0, cByte1, cByte2, cByte3;
    IppAVIStreamFormat *pCurStreamFormat = pDecoderState->pAVIStreamFormat[pDecoderState->cStrlListIdx];

    switch(pDecoderState->pAVIStreamHeader[pDecoderState->cStrlListIdx]->iStreamType){
        case AVI_VIDS:
            /* BITMAPINFO, including BITMAPINFOHEADER and RGBQUAD */
            GET4BYTES(pSrcBitStream->pBsCurByte, pCurStreamFormat->pBInfoHeader.ihSize)
            GET4BYTES(pSrcBitStream->pBsCurByte, pCurStreamFormat->pBInfoHeader.ihWidth)
            GET4BYTES(pSrcBitStream->pBsCurByte, pCurStreamFormat->pBInfoHeader.ihHeight)
            GET2BYTES(pSrcBitStream->pBsCurByte, pCurStreamFormat->pBInfoHeader.ihPlanes)
            GET2BYTES(pSrcBitStream->pBsCurByte, pCurStreamFormat->pBInfoHeader.ihBitCount)
            GET4BYTES(pSrcBitStream->pBsCurByte, pCurStreamFormat->pBInfoHeader.ihCompression)
            GET4BYTES(pSrcBitStream->pBsCurByte, pCurStreamFormat->pBInfoHeader.ihSizeImage)
            GET4BYTES(pSrcBitStream->pBsCurByte, pCurStreamFormat->pBInfoHeader.ihHResolution)
            GET4BYTES(pSrcBitStream->pBsCurByte, pCurStreamFormat->pBInfoHeader.ihVResolution)
            GET4BYTES(pSrcBitStream->pBsCurByte, pCurStreamFormat->pBInfoHeader.ihClrUsed)
            GET4BYTES(pSrcBitStream->pBsCurByte, pCurStreamFormat->pBInfoHeader.ihClrImportant)
            {   /* RGBQUAD */
                Ipp32u i;
                for(i=0; i<(pCurStreamFormat->pBInfoHeader.ihSize - 40); i+=4){
                    pCurStreamFormat->aColors[i>>2].ctBlue    = *(pSrcBitStream->pBsCurByte++);
                    pCurStreamFormat->aColors[i>>2].ctGreen = *(pSrcBitStream->pBsCurByte++);
                    pCurStreamFormat->aColors[i>>2].ctRed     = *(pSrcBitStream->pBsCurByte++);
                    pCurStreamFormat->aColors[i>>2].ctReserve = *(pSrcBitStream->pBsCurByte++);
                }
            }
            break;
        case AVI_AUDS:
            /* WAVEFORMATEX  */
            GET2BYTES(pSrcBitStream->pBsCurByte, pCurStreamFormat->pWaveFormatEx.sFormatType)
            GET2BYTES(pSrcBitStream->pBsCurByte, pCurStreamFormat->pWaveFormatEx.sNumChannel)
            GET4BYTES(pSrcBitStream->pBsCurByte, pCurStreamFormat->pWaveFormatEx.iSamplesPerSec)
            GET4BYTES(pSrcBitStream->pBsCurByte, pCurStreamFormat->pWaveFormatEx.iAvgBytesPerSec)
            GET2BYTES(pSrcBitStream->pBsCurByte, pCurStreamFormat->pWaveFormatEx.sBlockAlign)
            GET2BYTES(pSrcBitStream->pBsCurByte, pCurStreamFormat->pWaveFormatEx.sBitsPerSample)
            if(WAVE_FORMAT_PCM != pCurStreamFormat->pWaveFormatEx.sFormatType){
                GET2BYTES(pSrcBitStream->pBsCurByte, pCurStreamFormat->pWaveFormatEx.sExtraInfoSize)
            }
            break;
    }
    return IPP_STATUS_NOERR;
}

/*************************************************************************************************************
// Name:  appiAVIParseStrlList
// Description:
//      Decoder 'strl' list
// Input Arguments:
//      pSrcBitStream      - Pointer to the bit stream structure
//      iStrlListDataSize  - Length of the 'strl' list
// Output Arguments:
//      ppDecoderState     - Pointer to a initialized decoder state structure .
//      pDstPicture        - Pointer to an IppPicture, include info about origin AVI image
// Returns:
//      IPP_STATUS_NOERR - OK
//      IPP_STATUS_ERR   - Error  
// Others:
//      None
*************************************************************************************************************/
IppCodecStatus appiAVIParseStrlList
    (IppBitstream   *pSrcBitStream,
    IppMJPEGFmAVIDecoderState *pDecoderState,
    Ipp32s iStrlListDataSize)
{
    Ipp32u iChunkID = AVI_STRH;
    Ipp32u iChunkSize = 0;
    IPP_FILE *fStream = (IPP_FILE *)pDecoderState->pStreamHandler;
    IppCodecStatus rtCode = IPP_STATUS_NOERR;

    while(iStrlListDataSize>0){
        if(8!=IPP_Fread((void*)pSrcBitStream->pBsBuffer, 1, 8, fStream)){
            return IPP_STATUS_ERR;
        }
        pSrcBitStream->pBsCurByte = pSrcBitStream->pBsBuffer;
        appiAVIParseChunkTypeAndSize(pSrcBitStream, &iChunkID, &iChunkSize);
        iStrlListDataSize -= 8;
        switch(iChunkID){
            case AVI_STRH:
                if(iChunkSize!=IPP_Fread((void*)pSrcBitStream->pBsBuffer, 1, iChunkSize, fStream)){
                    return IPP_STATUS_ERR;
                }
                pSrcBitStream->pBsCurByte = pSrcBitStream->pBsBuffer;
                if(IPP_STATUS_NOERR
                    != (rtCode = appiAVIParseStrh(pSrcBitStream, pDecoderState)) ){
                    return rtCode;
                }
                iStrlListDataSize -= iChunkSize;
                pDecoderState->aStrhChunkSize[pDecoderState->cStrlListIdx] = iChunkSize;
                #ifdef _IPP_DEBUG
                IPP_Printf("            strh [ %d Bytes ]\n", iChunkSize);
                #endif
                break;
            case AVI_STRF:
                if(iChunkSize!=IPP_Fread((void*)pSrcBitStream->pBsBuffer, 1, iChunkSize, fStream)){
                    return IPP_STATUS_ERR;
                }
                pSrcBitStream->pBsCurByte = pSrcBitStream->pBsBuffer;
                appiAVIParseStrf(pSrcBitStream, pDecoderState);
                iStrlListDataSize -= iChunkSize;
                pDecoderState->aStrfChunkSize[pDecoderState->cStrlListIdx] = iChunkSize;
                #ifdef _IPP_DEBUG
                IPP_Printf("            strf [ %d Bytes ]\n", iChunkSize);
                #endif
                break;
            case AVI_STRD:
            case AVI_STRN:
            default:            
                pDecoderState->iaxFileSeek(fStream, (iChunkSize+(iChunkSize&0x1)), IPP_SEEK_CUR);
                iStrlListDataSize -= (iChunkSize+(iChunkSize&0x1) );
                #ifdef _IPP_DEBUG
                IPP_Printf("            %c%c%c%c [ %d Bytes ]\n", (iChunkID>>24), ((iChunkID&0x00FF0000)>>16), 
                    ((iChunkID&0x0000FF00)>>8), (iChunkID&0x000000FF), iChunkSize);
                #endif
                break;
        }
    }

    return IPP_STATUS_NOERR;
}

/*************************************************************************************************************
// Name:  appiAVIParseHdrlList
// Description:
//      Decoder 'hdrl' list, including 'avih' and 'strl'
// Input Arguments:
//      pSrcBitStream      - Pointer to the bit stream structure
//      iHdrlListDataSize  - Length of the 'hdrl' list
// Output Arguments:
//      ppDecoderState     - Pointer to a initialized decoder state structure .
//      pDstPicture        - Pointer to an IppPicture, include info about origin AVI image
// Returns:
//      IPP_STATUS_NOERR	- OK
//      IPP_STATUS_ERR		- Error 
// Others:
//      None
*************************************************************************************************************/
IppCodecStatus appiAVIParseHdrlList
    (IppBitstream   *pSrcBitStream,
    IppMJPEGFmAVIDecoderState *pDecoderState,
    IppPicture *pDstPicture,
    Ipp32s iHdrlListDataSize)
{
    Ipp32u iFourCCID = AVI_HDRL;
    Ipp32u iChunkSize = 0;
    Ipp32u iListSize = 0;
    Ipp8u   cListFlag = 0;
    IPP_FILE *fStream = (IPP_FILE *)pDecoderState->pStreamHandler;
    IppCodecStatus rtCode = IPP_STATUS_NOERR;

    while(iHdrlListDataSize>0){
        unsigned char cByte0, cByte1, cByte2, cByte3;
        if(4!=IPP_Fread((void*)pSrcBitStream->pBsBuffer, 1, 4, fStream)){
            return IPP_STATUS_ERR;
        }
        pSrcBitStream->pBsCurByte = pSrcBitStream->pBsBuffer;
        GETFOURCC(pSrcBitStream->pBsCurByte, iFourCCID)

        switch(iFourCCID){
            case AVI_AVIH:
                if(1!=IPP_Fread((void*)&iChunkSize, 4, 1, fStream)){
                    return IPP_STATUS_ERR;
                }
                #ifdef _IPP_DEBUG
                IPP_Printf("        avih [ %d Bytes ]\n", iChunkSize);
                #endif
                if(iChunkSize!=IPP_Fread((void*)pSrcBitStream->pBsBuffer, 1, iChunkSize, fStream)){
                    return IPP_STATUS_ERR;
                }
                pSrcBitStream->pBsCurByte = pSrcBitStream->pBsBuffer;
                appiAVIParseAviHeader(pSrcBitStream, pDecoderState);      
                iHdrlListDataSize -= (iChunkSize + 8);
                pDecoderState->iAvihChunkSize = iChunkSize;
                pDstPicture->picWidth = pDecoderState->pAVIHeader->iWidth;
                pDstPicture->picHeight = pDecoderState->pAVIHeader->iHeight;                
                break;
            case AVI_LIST:
                iHdrlListDataSize -= 8;
                cListFlag = 1;
                if(1!=IPP_Fread((void*)&iListSize, 4, 1, fStream)){
                    return IPP_STATUS_ERR;
                }
                break;
            case AVI_STRL:
                cListFlag = 0;
                #ifdef _IPP_DEBUG
                IPP_Printf("        LIST_strl [ %d Bytes ]\n", iListSize);
                #endif
                if(IPP_STATUS_NOERR
                    != ( rtCode = appiAVIParseStrlList(pSrcBitStream, pDecoderState, iListSize-4)) ){
                    return rtCode;
                }
                iHdrlListDataSize -= iListSize;
                pDecoderState->aStrlListSize[pDecoderState->cStrlListIdx] = iListSize;
                pDecoderState->cStrlListIdx++;                
                break;  
            default:
                if(1==cListFlag){
                    pDecoderState->iaxFileSeek(fStream, (iListSize-4), IPP_SEEK_CUR);
                    #ifdef _IPP_DEBUG
                    IPP_Printf("        LIST_%c%c%c%c [ %d Bytes ]\n", (iFourCCID>>24), ((iFourCCID&0x00FF0000)>>16), 
                        ((iFourCCID&0x0000FF00)>>8), (iFourCCID&0x000000FF), iListSize);
                    #endif
                    cListFlag = 0;
                    iHdrlListDataSize -= iListSize;
                }else{
                    if(1!=IPP_Fread((void*)&iChunkSize, 4, 1, fStream)){
                        return IPP_STATUS_ERR;
                    }
                    pDecoderState->iaxFileSeek(fStream, (iChunkSize+(iChunkSize&0x1)), IPP_SEEK_CUR);
                    #ifdef _IPP_DEBUG
                    IPP_Printf("        %c%c%c%c [ %d Bytes ]\n", (iFourCCID>>24), ((iFourCCID&0x00FF0000)>>16), 
                        ((iFourCCID&0x0000FF00)>>8), (iFourCCID&0x000000FF), iChunkSize);
                    #endif
                    iHdrlListDataSize -= (iChunkSize + 8+(iChunkSize&0x1));
                }
                break;
        }
    }
    return IPP_STATUS_NOERR;
}

/*************************************************************************************************************
// Name:  DecoderInitAlloc_MJPEGFmAVI
// Description:
//      Init function for AVI decoding, it builds the IppMJPEGFmAVIDecoderState 
// Input Arguments:
//      pInCbTbl       - Pointer to the memory and file related call back functions table
//      fStream        - Pointer to the input AVI file
// Output Arguments:
//      ppDecoderState - Pointer to a initialized AVI decoder structure .
//      pDstPicture    - Pointer to an IppPicture, include info about origin AVI image
// Returns:
//      IPP_STATUS_NOERR      - OK
//      IPP_STATUS_BADARG_ERR	- Bad arguments
//      IPP_STATUS_NOMEM_ERR  - Error because of memory allocate failure 
// Others:
//      None
*************************************************************************************************************/
IppCodecStatus DecoderInitAlloc_MJPEGFmAVI
    (IppPicture    *pDstPicture,
    void    **ppDecoderState,			 
    MiscGeneralCallbackTable    *pInCbTbl,
    void    *fStream)
{
    IppMJPEGFmAVIDecoderState *pDecoderState = NULL;	
    int i;

    /* 0.0 Check input argument */
    if ( !(ppDecoderState
        && pDstPicture && pInCbTbl) ) {
        return IPP_STATUS_BADARG_ERR;
    }    

    /* 1.0 Allocate structure memory for pDecoderState, and initialize it */
    pInCbTbl->fMemCalloc(&pDecoderState, sizeof(IppMJPEGFmAVIDecoderState), 4);
    if (NULL==pDecoderState) {
        return IPP_STATUS_NOMEM_ERR;
    }

    pInCbTbl->fMemCalloc(&pDecoderState->pAVIHeader, sizeof(IppAVIHeader), 4);
    if (NULL==pDecoderState->pAVIHeader) {
        return IPP_STATUS_NOMEM_ERR;
    }

    for(i=0; i<IPP_MAX_STREAM_NUM; i++){
        pInCbTbl->fMemCalloc(&(pDecoderState->pAVIStreamHeader[i]), sizeof(IppAVIStreamHeader), 4);
        if (NULL==pDecoderState->pAVIStreamHeader[i]) {
            return IPP_STATUS_NOMEM_ERR;
        }
    }

    for(i=0; i<IPP_MAX_STREAM_NUM; i++){
        pInCbTbl->fMemCalloc(&(pDecoderState->pAVIStreamFormat[i]),sizeof(IppAVIStreamFormat), 4);
        if (NULL==pDecoderState->pAVIStreamFormat[i]) {
            return IPP_STATUS_NOMEM_ERR;
        }
    }

    pInCbTbl->fMemCalloc(&pDecoderState->pAVIIdxEntry, sizeof(IppAVIIdxEntry), 4);
    if (NULL==pDecoderState->pAVIIdxEntry) {
        return IPP_STATUS_NOMEM_ERR;
    }	

    /* 1.1 Initial Decoder State structure */
    pDecoderState->iaxMemAlloc = pInCbTbl->fMemCalloc;
    pDecoderState->iaxMemFree  = pInCbTbl->fMemFree;
    pDecoderState->iaxFileSeek = pInCbTbl->fFileSeek;
    pDecoderState->iaxFileRead = pInCbTbl->fFileRead;

    if(NULL!=fStream){
        pDecoderState->pStreamHandler = fStream;
    }else{
        pDecoderState->pStreamHandler = NULL;
    }

    /* 1.2 Initialize attributes of source image */
    pDstPicture->picWidth = 0;
    pDstPicture->picHeight= 0; 

    *ppDecoderState = pDecoderState;	

	return IPP_STATUS_NOERR;
}


/*******************************************************************************************************************
// Name:  DecoderFree_MJPEGFmAVI
// Description:
//      Release the allocated working buffer and free the decoder state.
// Input Arguments:
//      ppDecoderState	Pointer to the AVI decoder state structure .
// Output Arguments:
//      None
// Returns:
//      IPP_STATUS_NOERR      - OK
//      IPP_STATUS_BADARG_ERR	- Bad arguments
// Others:
//      None
*********************************************************************************************************************/
IppCodecStatus DecoderFree_MJPEGFmAVI
    (void  **ppDecoderState)
{
    IppMJPEGFmAVIDecoderState *pDecoderState = (IppMJPEGFmAVIDecoderState*)(*ppDecoderState);
    MiscFreeCallback cbMemFree;  

    if (ppDecoderState==NULL) {
        return IPP_STATUS_BADARG_ERR;
    }

    if ( pDecoderState != NULL ) {
        cbMemFree = pDecoderState->iaxMemFree;

        /* Free AVI header structure */
        if (pDecoderState->pAVIHeader != NULL) {
            cbMemFree(&(pDecoderState->pAVIHeader));
            pDecoderState->pAVIHeader = NULL;
        }

        /* Free AVI stream header structure */
        if ( pDecoderState->pAVIStreamHeader != NULL ) {
            int i;
            for(i=0;  i<IPP_MAX_STREAM_NUM; i++){
                if ( pDecoderState->pAVIStreamHeader[i] != NULL ) {
                    cbMemFree(&(pDecoderState->pAVIStreamHeader[i]));
                    pDecoderState->pAVIStreamHeader[i] = NULL;
                }
            }
        }

        /* Free AVI stream format structure */
        if ( pDecoderState->pAVIStreamFormat != NULL ) {
            int i;
            for(i=0;  i<IPP_MAX_STREAM_NUM; i++){
                if ( pDecoderState->pAVIStreamFormat[i] != NULL ) {
                    cbMemFree(&(pDecoderState->pAVIStreamFormat[i]));
                    pDecoderState->pAVIStreamFormat[i] = NULL;
                }
            }
        }

        /* Free AVI index entry */
        if ( pDecoderState->pAVIIdxEntry != NULL ) {
            cbMemFree(&(pDecoderState->pAVIIdxEntry));
            pDecoderState->pAVIIdxEntry = NULL;
        }


        /* Free DecoderState and reset it */
        cbMemFree(ppDecoderState);
        *ppDecoderState = NULL;
    }

    return IPP_STATUS_NOERR;
}

/*************************************************************************************************************
// Name:  Parse_MJPEGFmAVI
// Description:
//      Parse the AVI Data
// Input Arguments:
//      pSrcBitStream  - Pointer to a IppBitstream structure
// Output Arguments:
//      ppDecoderState - Pointer to a the AVI decoder state structure .
//      pDstPicture    - Pointer to an IppPicture, include info about origin AVI image
// Returns:
//      IPP_STATUS_NOERR - OK
//      IPP_STATUS_ERR   - Feature Conditions not meeted
// Others:
//      None
*************************************************************************************************************/
IppCodecStatus Parse_MJPEGFmAVI
    (void  **ppDecoderState, 
    IppBitstream   *pSrcBitStream,
    IppPicture *pDstPicture)
{
    IppMJPEGFmAVIDecoderState *pDecoderState = (IppMJPEGFmAVIDecoderState *)(*ppDecoderState);
    Ipp32u iFourCCID = AVI_RIFF;
    Ipp32u iListSize = 0;
    Ipp32u iChunkSize = 0;
    Ipp8u   cListFlag = 0;
    IPP_FILE *fStream = (IPP_FILE *)pDecoderState->pStreamHandler;
    IppCodecStatus rtCode = IPP_STATUS_NOERR;
    unsigned char cByte0, cByte1, cByte2, cByte3;
    int iSizeFRead = 0;


    if(NULL==fStream){
        return IPP_STATUS_ERR;
    }

    while( (iSizeFRead = IPP_Fread((void*)pSrcBitStream->pBsBuffer, 1, 4, fStream) )
        && !IPP_Feof(fStream)){
        
        if(4!=iSizeFRead){
            return IPP_STATUS_ERR;
        }
        pSrcBitStream->pBsCurByte = pSrcBitStream->pBsBuffer;
        GETFOURCC(pSrcBitStream->pBsCurByte, iFourCCID)

        switch(iFourCCID){
        case AVI_RIFF:
            if(1!=IPP_Fread((void*)&(pDecoderState->iAviRiffSize), 4, 1, fStream)){
                return IPP_STATUS_ERR;
            }
            break;
        case AVI_AVI:
            #ifdef _IPP_DEBUG
            IPP_Printf("RIFF_AVI [ %d Bytes ]\n", pDecoderState->iAviRiffSize);
            #endif
            break;
        case AVI_LIST:
            cListFlag = 1;
            if(1!=IPP_Fread((void*)&iListSize, 4, 1, fStream)){
                return IPP_STATUS_ERR;
            }
            break;
        case AVI_HDRL:
            cListFlag = 0;
            #ifdef _IPP_DEBUG
            IPP_Printf("    LIST_hdrl [ %d Bytes ]\n", iListSize);
            #endif
            pDecoderState->iHdrlListSize = iListSize;
            if(IPP_STATUS_NOERR 
                != ( rtCode = appiAVIParseHdrlList(pSrcBitStream, pDecoderState, pDstPicture, iListSize-4)) ){
                    return rtCode;
            }
            break;        
        case AVI_MOVI:
            cListFlag = 0;
            #ifdef _IPP_DEBUG
            IPP_Printf("    LIST_movi [ %d Bytes ]\n", iListSize);
            #endif
            pDecoderState->iMoviListSize = iListSize;
            break;
        case AVI_IDX1:                        
            if(1!=IPP_Fread((void*)&(pDecoderState->iIdx1ChunkSize), 4, 1, fStream)){
                return IPP_STATUS_ERR;
            }
            pDecoderState->iaxFileSeek(fStream, (pDecoderState->iIdx1ChunkSize+(pDecoderState->iIdx1ChunkSize&0x1)), IPP_SEEK_CUR);
            #ifdef _IPP_DEBUG
            IPP_Printf("    idx1 [ %d Bytes ]\n", pDecoderState->iIdx1ChunkSize);
            #endif
            break;
        default:
            /* Skip these lists or chunks, or return the JPEG bit stream*/
            if(1==cListFlag){
                cListFlag = 0;
                pDecoderState->iaxFileSeek(fStream, (iListSize-4), IPP_SEEK_CUR);
                #ifdef _IPP_DEBUG
                IPP_Printf("        LIST_%c%c%c%c [ %d Bytes ]\n", (iFourCCID>>24), ((iFourCCID&0x00FF0000)>>16), 
                    ((iFourCCID&0x0000FF00)>>8), (iFourCCID&0x000000FF), iListSize);
                #endif                
            }else{                
                if(1!=IPP_Fread((void*)&iChunkSize, 4, 1, fStream)){
                    return IPP_STATUS_ERR;
                }
                #ifdef _IPP_DEBUG
                IPP_Printf("        %c%c%c%c [ %d Bytes ]\n", (iFourCCID>>24), ((iFourCCID&0x00FF0000)>>16), 
                    ((iFourCCID&0x0000FF00)>>8), (iFourCCID&0x000000FF), iChunkSize);
                #endif
                if(AVI_MOVICK_DC==((cByte2<<8) | cByte3) ){
                    Ipp32u iPaddedBytes = 0;
                    Ipp16u eoiFlag = 0;

                    if(((int)iChunkSize)>pSrcBitStream->bsByteLen){
                        /* 
                        // the JPEG bit stream is larger than the input bitstream buffer 
                        // alloc a new one 
                        */
                        pDecoderState->iaxMemFree(&pSrcBitStream->pBsBuffer);
                        pSrcBitStream->pBsBuffer = NULL;
                        pDecoderState->iaxMemAlloc(&pSrcBitStream->pBsBuffer, iChunkSize, 4);
                        if(NULL == pSrcBitStream->pBsBuffer){
                            return IPP_STATUS_NOMEM_ERR;
                        }
                        pSrcBitStream->bsByteLen = iChunkSize;
                    }

                    if(0!=iChunkSize){
                        if(iChunkSize!=IPP_Fread((void*)pSrcBitStream->pBsBuffer, 1, iChunkSize, fStream)){
                            return IPP_STATUS_ERR;
                        }

                        /* check whether JPEG bit stream */
                        if( (0xff!=pSrcBitStream->pBsBuffer[0]) || (0xd8!=pSrcBitStream->pBsBuffer[1]) ){
                            return IPP_STATUS_NOTSUPPORTED_ERR;
                        }
                        pSrcBitStream->pBsCurByte = pSrcBitStream->pBsBuffer + iChunkSize;

                        while(0xd9ff !=(eoiFlag&0xFFFF)){
                            eoiFlag = (eoiFlag << 8) | (pSrcBitStream->pBsCurByte[-iPaddedBytes-1]);
                            iPaddedBytes++;
                        }            
                        pSrcBitStream->pBsCurByte -= (iPaddedBytes-2);
                        pDecoderState->iaxFileSeek(fStream, (iChunkSize&0x1), IPP_SEEK_CUR); 
                    }else{
                        pSrcBitStream->pBsCurByte = pSrcBitStream->pBsBuffer;
                    }
                    return IPP_STATUS_FRAME_COMPLETE;
                }
                pDecoderState->iaxFileSeek(fStream, iChunkSize + (iChunkSize&0x1), IPP_SEEK_CUR); 

            }
            break;
        }
    }  

    return IPP_STATUS_NOERR;
}
/* EOF */