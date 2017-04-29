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



#include "codecJP.h"
#include "dib.h"
#include "misc.h"

#include "ippLV.h"


#define BMPFILE     1
#define JPGFILE     2
#define YUVFILE     3

#define BMP2JPG     1
#define YUV2JPG     2
#define JPG2BMP     3
#define JPG2YUV     4

#define VERSION     "4.0 Beta"
#define JPEG_YUV    0x00000080



typedef struct _CMDPARAMETERS
{   
    int nQuality;
    int nInterval;
    int nJPEGMode;
    int nSubsampling;
    int nScanMode;
    int nDesiredColor;
    int nConvertMode;
    int nWidth;
    int nHeight;
    int nFormat;
    int nBufType;
    char SrcFileName[100]; /* input raw data file name */
    char DstFileName[100];
} CMDPARAMETERS;


/******************************************************************************
// Name:			 DisplayLibVersion
//
// Description:		 Display library build information on the text console
//
// Input Arguments:  None.
//
// Output Arguments: None				
//
// Returns:			 None				
*******************************************************************************/
void DisplayLibVersion()
{
	char libversion[128]={'\0'};
	IppCodecStatus ret;
	ret = GetLibVersion_JPEGENC(libversion,sizeof(libversion));
	if(0 == ret){
		IPP_Printf("\n*****************************************************************\n");
	    IPP_Printf("This library is built from %s\n",libversion);
		IPP_Printf("*****************************************************************\n");
	}else{
		IPP_Printf("\n*****************************************************************\n");
		IPP_Printf("Can't find this library version information\n");
		IPP_Printf("*****************************************************************\n");
	}
}

/******************************************************************************
// Name:				ShowImageAttr
// Description:			Show the image attribute.
// Input Arguments:
//		pDIBSpec:	Pointer to the MDIBSPEC structure
// Output Arguments:	
//		None
// Returns:
//     None				
******************************************************************************/
void ShowImageAttr(MDIBSPEC *pDIBSpec)
{
    IPP_Printf ("Source Image Attributes:\n");
    IPP_Printf ("Image Width:   %d\n", pDIBSpec->nWidth);
    IPP_Printf ("Image Height:  %d\n", pDIBSpec->nHeight);
    switch (pDIBSpec->nClrMode) {
        case JPEG_GRAY8:
            IPP_Printf ("Image Type:    %s\n", "Gray8");
            break;
        case JPEG_BGR888:
            IPP_Printf ("Image Type:    %s\n", "RGB888");
            break;
        case JPEG_BGR555:
            IPP_Printf ("Image Type:    %s\n", "RGB555");
            break;
        case JPEG_BGR565:
            IPP_Printf ("Image Type:    %s\n", "RGB565");
            break;
        case JPEG_ABGR:
            IPP_Printf ("Image Type:    %s\n", "ABGR");
            break;
        default:
            IPP_Printf ("Image Type:    %s\n", "unknown");
            break;
    }
    IPP_Printf ("Image Size:    %d\n", pDIBSpec->nDataSize);
}

/******************************************************************************
// Name:                        ReadStreamFromFile
// Description:                 Read stream from a file
//
// Input Arguments:
//                iSize:        Item size in bytes
//               iCount:        Maximum number of items to be read
//             pHandler:    	Point to the FILE structure
//
// Output Arguments:
//             ppDstBuf:        Pointer to the pointer to the dst buffer
//
// Returns:
//        Return the actual read data size in byte
// ******************************************************************************/
int       gPerfFileReadIndex;
int ReadStreamFromFile(void **ppDstBuf, int iSize, int iCount, void *pHandler)
{
	IPP_FILE *fp = (IPP_FILE *)pHandler;
	int ret;

	/* Read data from FILE */
	IPP_StartPerfCounter(gPerfFileReadIndex);
	ret = IPP_Fread(*ppDstBuf, iSize, iCount, fp);
	IPP_StopPerfCounter(gPerfFileReadIndex);
	return (ret*iSize);
}

/******************************************************************************
// Name:             BMPtoJPEG
// Description:      Compress image raw data from BMP to JPEG file
//
// Input Arguments:
//         nQuality: Required quality of compression between 0 and 100. 100 is
//                   the best
// nRestartInterval: Required restart intervals for Huffman coding
//        nJPEGMode: Required encoding mode (JPEG_BASELINE or JPEG_PROGRESSIVE)
//     nSubsampling: Required subsampling of YCbCr (JPEG_411, JPEG_444 or 
//                   JPEG_422)
//         pDIBSpec: Pointer to the specification of input BMP file
//
// Output Arguments:
//          dstFile: Pointer to output JPEG file
//
// Returns:
//             IPP_STATUS_NOERR - OK
//  IPP_STATUS_NOTSUPPORTED_ERR - Not supported BMP format
//         IPP_STATUS_NOMEM_ERR - Memory allocation error
//               IPP_STATUS_ERR - General errors
******************************************************************************/
int BMPtoJPEG ( int      nQuality,
               int      nRestartInterval,
               int      nJPEGMode,
               int      nSubsampling,
               int      nBufType,
               MDIBSPEC *pDIBSpec,
               IPP_FILE	 *srcFile,
               IPP_FILE     *dstFile)
{
    MiscGeneralCallbackTable   *pCallBackTable = NULL;
    IppJPEGEncoderParam encoderPar;
    IppPicture          srcPicture;
	IppBitstream        dstBitstream = {0};
    void                *pEncoderState = NULL;
    int nMCUHeight;
    int nSrcBuffersize;

    int perf_index;    
    long long TotTime;
    int ret = IPP_STATUS_NOERR;

    IPP_GetPerfCounter(&perf_index, DEFAULT_TIMINGFUNC_START, DEFAULT_TIMINGFUNC_STOP);
    IPP_ResetPerfCounter(perf_index);
    
    IPP_GetPerfCounter(&gPerfFileReadIndex, IPP_TimeGetTickCount, IPP_TimeGetTickCount);
    IPP_ResetPerfCounter(gPerfFileReadIndex);

    /* Prepare encode parameters */
    encoderPar.nQuality         = nQuality;
    encoderPar.nRestartInterval = nRestartInterval;
    encoderPar.nJPEGMode        = nJPEGMode;
    encoderPar.nSubsampling     = nSubsampling;
    encoderPar.nBufType			= nBufType;

    encoderPar.pSrcPicHandler	= srcFile;
    encoderPar.pStreamHandler   = dstFile;

    if (8!=pDIBSpec->nPrecision) {
        return IPP_STATUS_NOTSUPPORTED_ERR;
    }

    /* Prepare picture parameters */
    srcPicture.picWidth         = pDIBSpec->nWidth;
    srcPicture.picHeight        = pDIBSpec->nHeight;
    srcPicture.picChannelNum    = pDIBSpec->nNumComponent;
    srcPicture.picPlaneNum      = 1;

    /* BMP in Windows is stored from down to top */
    srcPicture.picPlaneStep[0]  = -(pDIBSpec->nStep);
    srcPicture.picFormat        = pDIBSpec->nClrMode;

    /* Prepare bitstream structure 
    // Notes:
    // Please be aware that the more buffer you allocate for output stream,
    // the less flush times are needed, and the better performance you get.
    // Be careful to set an optimal size for performance.
    */
    if (10240>pDIBSpec->nDataSize) {
        dstBitstream.bsByteLen      = 10240;
    } else {
        /* demonstration purpose only */
        dstBitstream.bsByteLen      = 1024;//pDIBSpec->nDataSize;
    }

    IPP_MemCalloc((void **)&(dstBitstream.pBsBuffer), dstBitstream.bsByteLen, 1);
    if (NULL == dstBitstream.pBsBuffer) {
        return IPP_STATUS_NOMEM_ERR;
    }

    /* Init callback table */
    if ( miscInitGeneralCallbackTable(&pCallBackTable) != 0 ) {
        IPP_MemFree((void **) &(dstBitstream.pBsBuffer) );
        return IPP_STATUS_NOMEM_ERR; 
    }
    
    /* Override the fFileRead callback function */
    pCallBackTable->fFileRead = ReadStreamFromFile;

    if(	encoderPar.nBufType	== JPEG_INTEGRATEBUF){
        if(NULL == pDIBSpec->pBitmapData){
            IPP_MemCalloc( (void **) &(pDIBSpec->pBitmapData), pDIBSpec->nDataSize, 4);
            if (NULL == pDIBSpec->pBitmapData){
                ret = IPP_STATUS_NOMEM_ERR;
                goto BMP2JPEG_End;
            }
            IPP_Fread(pDIBSpec->pBitmapData, 1, pDIBSpec->nDataSize, srcFile);
        }
        if(srcPicture.picPlaneStep[0]<0){
            srcPicture.ppPicPlane[0] = (Ipp8u *)pDIBSpec->pBitmapData + 
                (-srcPicture.picPlaneStep[0]) * (srcPicture.picHeight - 1);
        }else{
            srcPicture.ppPicPlane[0] = (Ipp8u *)pDIBSpec->pBitmapData;
        }
    }else{        
        if (JPEG_GRAY8 == srcPicture.picFormat){
            nMCUHeight      = 8;
        }
        else{
            switch (encoderPar.nSubsampling) {
            case JPEG_444:
                nMCUHeight  = 8;
                break;
            case JPEG_422:
                nMCUHeight  = 8;
                break;
            case  JPEG_411:
                nMCUHeight  = 16;                
                break;
            default:
                ret = IPP_STATUS_NOTSUPPORTED_ERR;
                goto BMP2JPEG_End;
            }
        }
        if(srcPicture.picPlaneStep[0]<0){
            nSrcBuffersize = (-(srcPicture.picPlaneStep[0])) * nMCUHeight;
            IPP_MemCalloc((void **)&(pDIBSpec->pBitmapData), nSrcBuffersize, 4);
            if (NULL == pDIBSpec->pBitmapData){
                ret = IPP_STATUS_NOMEM_ERR;
                goto BMP2JPEG_End;
            }
            srcPicture.ppPicPlane[0] = pDIBSpec->pBitmapData;
        }else{
            nSrcBuffersize = (srcPicture.picPlaneStep[0]) * nMCUHeight;
            IPP_MemCalloc((void **)&(pDIBSpec->pBitmapData), nSrcBuffersize, 4);
            if (NULL == pDIBSpec->pBitmapData){
                ret= IPP_STATUS_NOMEM_ERR;
                goto BMP2JPEG_End;
            }
            srcPicture.ppPicPlane[0] = pDIBSpec->pBitmapData;
        }
    }

    if (IPP_STATUS_NOERR != EncoderInitAlloc_JPEG  (&encoderPar,
        &srcPicture,
        &dstBitstream,
        pCallBackTable,
        &pEncoderState)) { 
            ret = IPP_STATUS_ERR;
            goto BMP2JPEG_End;
    }

	IPP_StartPerfCounter(perf_index);
    if (IPP_STATUS_NOERR != Encode_JPEG (&srcPicture,
        &dstBitstream,
        pEncoderState)) {
            ret = IPP_STATUS_ERR;
            goto BMP2JPEG_End;

    }
    IPP_StopPerfCounter(perf_index);
    TotTime = IPP_GetPerfData(perf_index);
    TotTime -= IPP_GetPerfData(gPerfFileReadIndex);
    g_Tot_Time[IPP_VIDEO_INDEX] = TotTime;
    
    IPP_Printf("jpeg enc consume %lld us \n",TotTime); 

BMP2JPEG_End:
    IPP_FreePerfCounter(perf_index);
    IPP_FreePerfCounter(gPerfFileReadIndex);

    EncoderFree_JPEG(&pEncoderState);

    if(NULL!=dstBitstream.pBsBuffer){

    if(NULL!=dstFile){
        int count = (int)(dstBitstream.pBsCurByte
            - dstBitstream.pBsBuffer);
        IPP_Fwrite(dstBitstream.pBsBuffer, 1, (int)dstBitstream.pBsCurByte
            - (int)dstBitstream.pBsBuffer, dstFile);
    }

    IPP_MemFree((void **) &(dstBitstream.pBsBuffer) );
    }
    if(NULL!=pCallBackTable){
    miscFreeGeneralCallbackTable(&pCallBackTable);
    }

    return ret;

}

/******************************************************************************
// Name:             YUVtoJPEG
// Description:      Compress image raw data from YUV to JPEG file
//
// Input Arguments:
//         nQuality: Required quality of compression between 0 and 100. 100 is
//                   the best
// nRestartInterval: Required restart intervals for Huffman coding
//        nJPEGMode: Required encoding mode (JPEG_BASELINE or JPEG_PROGRESSIVE)
//     nSubsampling: Required subsampling of YCbCr, ignored in this function.
//         pYUVSpec: Pointer to the specification of input YUV raw data
//
// Output Arguments:
//          dstFile: Pointer to output JPEG file
//
// Returns:
//             IPP_STATUS_NOERR - OK
//         IPP_STATUS_NOMEM_ERR - Memory allocation error
//               IPP_STATUS_ERR - General errors
******************************************************************************/
int YUVtoJPEG (int      nQuality,
               int      nRestartInterval,
               int      nJPEGMode,
               int      nSubsampling,
               int      nBufType,
               MYUVSPEC *pYUVSpec,
               IPP_FILE		*srcFile,
               IPP_FILE     *dstFile)
{
    MiscGeneralCallbackTable   *pCallBackTable = NULL;
    IppJPEGEncoderParam encoderPar;
    IppPicture          srcPicture;
    IppBitstream        dstBitstream;
    void                *pEncoderState;
    int                 nWidth = pYUVSpec->nWidth;
    int                 nHeight= pYUVSpec->nHeight;
    int					nLen1, nLen2;

    int perf_index;    
    long long TotTime;

    IPP_GetPerfCounter(&perf_index, DEFAULT_TIMINGFUNC_START, DEFAULT_TIMINGFUNC_STOP);
    IPP_GetPerfCounter(&gPerfFileReadIndex, IPP_TimeGetTickCount, IPP_TimeGetTickCount);
    IPP_ResetPerfCounter(perf_index);
    IPP_ResetPerfCounter(gPerfFileReadIndex);

    /* Prepare encode parameters */
    encoderPar.nQuality         = nQuality;
    encoderPar.nRestartInterval = nRestartInterval;
    encoderPar.nJPEGMode        = nJPEGMode;
    encoderPar.nSubsampling     = nSubsampling;
    encoderPar.nBufType			= nBufType;
    encoderPar.pStreamHandler   = dstFile;
    encoderPar.pSrcPicHandler   = srcFile;

    /* Prepare source picture parameters */
    srcPicture.picWidth         = nWidth;
    srcPicture.picHeight        = nHeight;
    /* Assume three components existent in picture always: Y, Cb, Cr */
    srcPicture.picChannelNum    = 3;
    /* Assume three planes Y, Cb, Cr are contained in source picture */
    srcPicture.picPlaneNum      = 3;
    srcPicture.picFormat        = pYUVSpec->nSamplingFormat;

    switch (srcPicture.picFormat) {
    case JPEG_YUV444:
        srcPicture.picPlaneStep[0] = nWidth;
        srcPicture.picPlaneStep[1] = nWidth;
        srcPicture.picPlaneStep[2] = nWidth;
        nLen1 = nWidth * nHeight;
        nLen2 = nLen1;
        break;
    case JPEG_YUV422:
        srcPicture.picPlaneStep[0] = nWidth;
        srcPicture.picPlaneStep[1] = (nWidth >> 1);
        srcPicture.picPlaneStep[2] = (nWidth >> 1);
        nLen1 = nWidth * nHeight;
        nLen2 = nLen1 >> 1;
        break;
    case JPEG_YUV411:
        srcPicture.picPlaneStep[0] = nWidth;
        srcPicture.picPlaneStep[1] = (nWidth >> 1);
        srcPicture.picPlaneStep[2] = (nWidth >> 1);
        nLen1 = nWidth * nHeight;
        nLen2 = nLen1 >> 2;
        break;
    default:
        return IPP_STATUS_INPUT_ERR;
    }    

    /* Prepare bitstream structure 
    // Notes:
    // Please be aware that the more buffer you allocate for output stream,
    // the less flush times are needed, and the better performance you get.
    // Be careful to set an optimal size for performance.
    */
    if (nWidth * nHeight < 1024) {
        dstBitstream.bsByteLen = 3072;
    } else {
        /* demonstration purpose only */
        dstBitstream.bsByteLen = 1024;//3 * nWidth * nHeight;
    }

    IPP_MemCalloc((void **)&(dstBitstream.pBsBuffer), dstBitstream.bsByteLen, 1);
    if (NULL == dstBitstream.pBsBuffer) {
        return IPP_STATUS_NOMEM_ERR;
    }

    /* Init callback table */
    if ( miscInitGeneralCallbackTable(&pCallBackTable) != 0 ) {
        IPP_MemFree((void **) &(dstBitstream.pBsBuffer) );
        return IPP_STATUS_NOMEM_ERR; 
    }

     /* Override the fFileRead callback function */
    pCallBackTable->fFileRead = ReadStreamFromFile;

    if(	encoderPar.nBufType	== JPEG_INTEGRATEBUF){
        IPP_MemCalloc((void **)&(pYUVSpec->pData), pYUVSpec->nDataSize, 1);
        if (NULL == pYUVSpec){
            IPP_MemFree((void **) &(dstBitstream.pBsBuffer) );
            miscFreeGeneralCallbackTable(&pCallBackTable);
            return IPP_STATUS_NOMEM_ERR;
        }
        IPP_Fread(pYUVSpec->pData, 1, pYUVSpec->nDataSize, srcFile);
        srcPicture.ppPicPlane[0] = pYUVSpec->pData;
        srcPicture.ppPicPlane[1] = (Ipp8u *)srcPicture.ppPicPlane[0] + nLen1;
        srcPicture.ppPicPlane[2] = (Ipp8u *)srcPicture.ppPicPlane[1] + nLen2;
    }else{
        /*added by yiwang*/
        int nSrcBuffersize;
        int nLen1 = 0, nLen2 = 0;

        nSrcBuffersize = 8;	
        switch (srcPicture.picFormat) {
        case JPEG_YUV444:
            nLen1 = nSrcBuffersize * srcPicture.picPlaneStep[0];
            nLen2 = nLen1;
            nSrcBuffersize = nLen1 * 3;
            break;
        case JPEG_YUV422:
            nLen1 = nSrcBuffersize * srcPicture.picPlaneStep[0];
            nLen2 = nLen1 >> 1;
            nSrcBuffersize = nLen1 * 2 ;
            break;
        case JPEG_YUV411:
            nLen1 = nSrcBuffersize * srcPicture.picPlaneStep[0] * 2;
            nLen2 = nLen1 >> 2;
            nSrcBuffersize = nLen1 + (nLen2 << 1);
            break;
        }
        IPP_MemCalloc((void **)&(pYUVSpec->pData),nSrcBuffersize, 1);
        if (NULL == pYUVSpec->pData){
            IPP_MemFree((void **) &(dstBitstream.pBsBuffer) );
            miscFreeGeneralCallbackTable(&pCallBackTable);
            return IPP_STATUS_NOMEM_ERR;
        }
        srcPicture.ppPicPlane[0] = pYUVSpec->pData;
        srcPicture.ppPicPlane[1] = (Ipp8u *)srcPicture.ppPicPlane[0] + nLen1;
        srcPicture.ppPicPlane[2] = (Ipp8u *)srcPicture.ppPicPlane[1] + nLen2;
    }

    /* init */
    if (IPP_STATUS_NOERR != EncoderInitAlloc_JPEG  (&encoderPar,
        &srcPicture,
        &dstBitstream,
        pCallBackTable,
        &pEncoderState)) {
            IPP_MemFree((void **)&(dstBitstream.pBsBuffer));
            IPP_MemFree((void **)&(pYUVSpec->pData));
            EncoderFree_JPEG(&pEncoderState);
            miscFreeGeneralCallbackTable(&pCallBackTable);
            return IPP_STATUS_ERR;
    }


	IPP_StartPerfCounter(perf_index);
    if (IPP_STATUS_NOERR != Encode_JPEG (&srcPicture,
        &dstBitstream,
        pEncoderState)) {
            IPP_MemFree((void **)&(dstBitstream.pBsBuffer));
            IPP_MemFree((void **)&(pYUVSpec->pData));
            EncoderFree_JPEG(&pEncoderState);
            miscFreeGeneralCallbackTable(&pCallBackTable);
            return IPP_STATUS_ERR;
    }

    IPP_StopPerfCounter(perf_index);
    TotTime = IPP_GetPerfData(perf_index);
    TotTime -=IPP_GetPerfData(gPerfFileReadIndex);
    g_Tot_Time[IPP_VIDEO_INDEX] = TotTime;

    IPP_Printf("jpeg enc consume %lld us \n",TotTime);

    IPP_FreePerfCounter(perf_index);
    IPP_FreePerfCounter(gPerfFileReadIndex);

    /* dump compressed bitstream to output file*/ 
    if(NULL!=dstFile){
        IPP_Fwrite(dstBitstream.pBsBuffer, 1, (int)dstBitstream.pBsCurByte
            - (int)dstBitstream.pBsBuffer, dstFile);
    }

    IPP_MemFree((void **)&(dstBitstream.pBsBuffer));
    IPP_MemFree((void **)&(pYUVSpec->pData));
    EncoderFree_JPEG(&pEncoderState);
    miscFreeGeneralCallbackTable(&pCallBackTable);

    return IPP_STATUS_NOERR;
}

/******************************************************************************
// Name:				ParseJPEGEncCmd
// Description:			Parse the user command and update the encoder paramter structure.
// Input Arguments:
//		pCmd:	Pointer to the input cmd buffer
// Output Arguments:	
//		ppSrcFileName: Pointer to pointer tocurrent src file name
//		ppDstFileName: Pointer to pointer to dst file name
//		ppLogFileName: Pointer to pointer to log file name
//		pParameters:   Pointer to paramter structure
// Returns:
//     [Success]   General IPP return code
//     [Failure]   General IPP return code					
******************************************************************************/

IppCodecStatus ParseJPEGEncCmd(char *pNextCmd, char **ppSrcFileName, char **ppDstFileName, 
                               char **ppLogFileName, CMDPARAMETERS *pParameters)                            
{
    int iLength, i = 0;
    char *pIdx, *pCurPos, *pEnd;
    int nSrcType = -1, nDstType = -1;
    char *pSrcFileName = NULL;
    char *pDstFileName = NULL;
    char *pLogFileName = NULL;


    /* Parse comments */
    if(pNextCmd[0] == '#' || pNextCmd[0] == '\t'|| pNextCmd[0] == ' ' || pNextCmd[0] == '\0') {
        return IPP_STATUS_ERR;
    }

    /* Replace the '\t' with ' 'in the cmd string */
    while(pNextCmd[i]) {
        if(pNextCmd[i] == '\t') {
            pNextCmd[i] = ' ';
        }
        i++;
    }

    pCurPos = pNextCmd;

    while((pIdx = IPP_Strstr(pCurPos, "-"))) {

        pEnd = IPP_Strstr(pIdx+1, " -");

        if(!pEnd) {
            pEnd = pNextCmd + IPP_Strlen(pNextCmd);
            iLength = pEnd - pIdx - 3;
        } else {
            iLength = pEnd - pIdx - 3;		
        }


        switch(pIdx[1]) {
        case 'i':
        case 'I':
            if(*ppSrcFileName==NULL){
                IPP_MemCalloc((void **)ppSrcFileName, 512, 4);
                if(*ppSrcFileName==NULL){
                    IPP_Printf ("Memory allocation for src file name error!\n");
                    return IPP_STATUS_ERR;
                }
            }
            pSrcFileName = *ppSrcFileName;
            /* If new image, state re-initialization is needed */
            IPP_Strncpy(pSrcFileName, pIdx + 3, iLength);
            pSrcFileName [iLength] = '\0';
            /* Get source file type */
            if ( ((pSrcFileName[iLength-3]=='b') ||(pSrcFileName[iLength-3]=='B'))
                && ((pSrcFileName[iLength-2]=='m') || (pSrcFileName[iLength-2]=='M'))
                && ((pSrcFileName[iLength-1]=='p') || (pSrcFileName[iLength-1]=='P')) ) {
                    nSrcType=BMPFILE;
            } else  if (((pSrcFileName[iLength-3]=='j') || (pSrcFileName[iLength-3]=='J')) 
                &&  ((pSrcFileName[iLength-2]=='p') || (pSrcFileName[iLength-2]=='P'))
                && ((pSrcFileName[iLength-1]=='g') || (pSrcFileName[iLength-1]=='G'))) {
                    nSrcType=JPGFILE;
            } else if (((pSrcFileName[iLength-3] == 'y') ||  (pSrcFileName[iLength-3] == 'Y')) 
                && ((pSrcFileName[iLength-2] == 'u')  ||(pSrcFileName[iLength-2] == 'U')) 
                && ((pSrcFileName[iLength-1] == 'v')  ||(pSrcFileName[iLength-1] == 'V'))) {
                    nSrcType = YUVFILE;
            }
            break;
        case 'o':
        case 'O':
            if(*ppDstFileName==NULL){
                IPP_MemCalloc((void **)ppDstFileName, 512, 4);
                if(*ppDstFileName==NULL){
                    IPP_Printf ("Memory allocation for dst file name error!\n");
                    return IPP_STATUS_ERR;
                }
            }
            pDstFileName = *ppDstFileName;
            IPP_Strncpy(pDstFileName, pIdx + 3, iLength);
            pDstFileName [iLength] = '\0';
            /* Get destination file type */
            if (((pDstFileName[iLength-3]=='b') || (pDstFileName[iLength-3]=='B'))
                && ((pDstFileName[iLength-2]=='m') || (pDstFileName[iLength-2]=='M'))
                && ((pDstFileName[iLength-1]=='p') || (pDstFileName[iLength-1]=='P'))) {
                    nDstType=BMPFILE;
            } else  if (((pDstFileName[iLength-3]=='j') ||  (pDstFileName[iLength-3]=='J'))
                && ((pDstFileName[iLength-2]=='p') || (pDstFileName[iLength-2]=='P'))
                && ((pDstFileName[iLength-1]=='g') || (pDstFileName[iLength-1]=='G'))) {
                    nDstType=JPGFILE;
            } else if (((pDstFileName[iLength-3] == 'y') || (pDstFileName[iLength-3] == 'Y')) 
                && ((pDstFileName[iLength-2] == 'u') || (pDstFileName[iLength-2] == 'U')) 
                && ((pDstFileName[iLength-1] == 'v') || (pDstFileName[iLength-1] == 'V'))) {
                    nDstType = YUVFILE;
            }
            break;
        case 'l':
        case 'L':
            if(*ppLogFileName==NULL){
                IPP_MemCalloc((void **)ppLogFileName, 512, 4);
                if(*ppLogFileName==NULL){
                    IPP_Printf ("Memory allocation for log file name error!\n");
                    return IPP_STATUS_ERR;
                }
            }
            pLogFileName = *ppLogFileName;
            IPP_Strncpy(pLogFileName, pIdx + 3, iLength);
            pLogFileName [iLength] = '\0';
            break;
        case 'b':
        case 'B':
            pParameters->nBufType = IPP_Atoi(pIdx+3);
            if ((JPEG_SLICEBUF != pParameters->nBufType) && 
                (JPEG_INTEGRATEBUF != pParameters->nBufType)){
                    pParameters->nBufType = JPEG_SLICEBUF;
                    IPP_Printf ("-b error, using defualt value (JPEG_SLICEBUF).\n");
            }
            break;
        case 'q':
        case 'Q':
            pParameters->nQuality = IPP_Atoi(pIdx+3);
            if (0>=pParameters->nQuality||pParameters->nQuality>100) {
                pParameters->nQuality = 50;
                IPP_Printf ("-q error, using defualt value (50).\n");
            }
            break;
        case 'v':
        case 'V':
            pParameters->nInterval = IPP_Atoi(pIdx+3);
            if ( (0>pParameters->nInterval) ||(65535<pParameters->nInterval) ){
                pParameters->nInterval = 0;
                IPP_Printf ("-v error, using defualt value (0).\n");
            }
            break;
        case 'm':
        case 'M':
            switch (IPP_Atoi(pIdx+3)) {
            case 0:
                pParameters->nJPEGMode=JPEG_BASELINE;
                break;
            case 1:
                pParameters->nJPEGMode=JPEG_PROGRESSIVE;
                break;
            default:
                pParameters->nJPEGMode=JPEG_BASELINE;
                IPP_Printf ("-m error, using defualt value (0).\n");
                break;
            }
            break;
        case 's':
        case 'S':
            switch (IPP_Atoi(pIdx+3)) {
            case 444:
                pParameters->nSubsampling = JPEG_444;
                break;
            case 411:
                pParameters->nSubsampling = JPEG_411;
                break;
            case 422:
                pParameters->nSubsampling = JPEG_422;
                break;
            default:
                pParameters->nSubsampling = JPEG_411;
                IPP_Printf ("-s error, using defualt value (411).\n");
                break;
            }
            break;
        case 'w':
        case 'W':
            pParameters->nWidth = IPP_Atoi(pIdx+3);
            break;
        case 'h':
        case 'H':
            pParameters->nHeight = IPP_Atoi(pIdx+3);
            break;
        case 'f':
        case 'F':
            switch (IPP_Atoi(pIdx+3)) {
            case 444:
                pParameters->nFormat = JPEG_YUV444;
                break;
            case 422:
                pParameters->nFormat = JPEG_YUV422;
                break;
            case 411:
                pParameters->nFormat = JPEG_YUV411;
                break;
            default:
                pParameters->nFormat = JPEG_YUV411;
                IPP_Printf ("-f error, using defualt value (411).\n");
                break;
            }
            break;
        case 'd':
        case 'D':
            switch (IPP_Atoi(pIdx+3)) {
            case 0:
                pParameters->nDesiredColor = JPEG_GRAY8|JPEG_BGR888;
                break;
            case 1:
                pParameters->nDesiredColor = JPEG_GRAY8|JPEG_BGR555;
                break;
            case 2:
                pParameters->nDesiredColor = JPEG_GRAY8|JPEG_BGR565;
                break;
            default:
                pParameters->nDesiredColor = JPEG_GRAY8|JPEG_BGR888;
                IPP_Printf ("-d error, using defualt value (0).\n");
                break;
            }
            break;        
        default:
            return IPP_STATUS_ERR;
        }

        pCurPos = pEnd;
    }

	if (BMPFILE==nSrcType) {
        pParameters->nConvertMode=BMP2JPG;
    } else if (YUVFILE==nSrcType) {
        pParameters->nConvertMode = YUV2JPG;
    } else {
        pParameters->nConvertMode = -1;
        return IPP_STATUS_ERR;
    }

    if (nSrcType == YUVFILE) {
        /* up/downsampling is not supported for YUV input currently */
        switch (pParameters->nFormat) {
        case JPEG_YUV444:
            pParameters->nSubsampling = JPEG_444;
            break;
        case JPEG_YUV422:
            pParameters->nSubsampling = JPEG_422;
            break;
        case JPEG_YUV411:
            pParameters->nSubsampling = JPEG_411;
            break;
        default:
            break;
        }
    }

    if (nDstType == YUVFILE) {
        /* up/downsampling is not supported for YUV output currently */
        pParameters->nDesiredColor = JPEG_YUV;
    }

    return IPP_STATUS_NOERR;
}

/******************************************************************************
// Name:				JPEGEnc
// Description:			Main entry for JPEG encoder
// Input Arguments:
//     pCmd  - Pointer to the command line
// Output Arguments:
//     None
// Returns:
//     IPP_OK   - encoder success
//     IPP_FAIL - encoder fail
******************************************************************************/
int JPEGEnc(char* pCmd)
{
    IPP_FILE *srcFile = NULL; 
    IPP_FILE *dstFile = NULL;
    int nSrcFileLen;
    int nDstFileLen;
    CMDPARAMETERS  parameters;
    IppCodecStatus rtCode;
    char *pSrcFileName = NULL;
    char *pDstFileName = NULL;
    char *pLogFileName = NULL;
    int ret = IPP_OK;
    
    parameters.nQuality      = 50;
    parameters.nInterval     = 0;
    parameters.nJPEGMode     = JPEG_BASELINE;
    parameters.nSubsampling  = JPEG_411;
    parameters.nDesiredColor = JPEG_GRAY8|JPEG_BGR888;
    parameters.nConvertMode  = -1;
    parameters.nWidth        = 176;
    parameters.nHeight       = 144;
    parameters.nFormat       = JPEG_YUV411;
    parameters.nBufType      = JPEG_SLICEBUF;

    if (IPP_STATUS_NOERR!=ParseJPEGEncCmd(pCmd, &pSrcFileName, &pDstFileName, &pLogFileName, &parameters)) {
        IPP_Log(pLogFileName,"a","IPP Error:command error\n");  
        IPP_Printf("JPEG Encode Failed\n");
        ret = IPP_FAIL;
        goto Encode_Done;
    }

    IPP_Printf ("Start Converting...\n");
    if(NULL==pSrcFileName){
        IPP_Log(pLogFileName,"a","IPP Error: input source file is NULL\n");
        IPP_Printf("JPEG Encode Failed\n");
        ret = IPP_FAIL;
        goto Encode_Done;
    }else{
        IPP_Printf ("Source File:      %s \n", pSrcFileName);
        srcFile=IPP_Fopen(pSrcFileName, "rb");
    }
    if(NULL==pDstFileName){
        IPP_Printf ("Destination File: NULL \n");
    }else{
        IPP_Printf ("Destination File: %s \n", pDstFileName);
        dstFile=IPP_Fopen(pDstFileName, "wb");
    }

    if( NULL==srcFile ){
        IPP_Log(pLogFileName,"a","IPP Error:can not open src file :%s\n",pSrcFileName);  
        IPP_Printf("JPEG Encode Failed\n");
        ret = IPP_FAIL;
        goto Encode_Done;
    }
    if( NULL==dstFile ){
        if(NULL==pDstFileName){
            IPP_Log(pLogFileName,"a","IPP Status: dst file is NULL\n");  
        }else{
            IPP_Log(pLogFileName,"a","IPP Status:can not open dst file :%s\n",pDstFileName);  
        }
    }

    switch(parameters.nConvertMode) {
    case BMP2JPG: 
        {
            MDIBSPEC       dibSpec;
            IPP_Memset(&dibSpec, 0, sizeof(MDIBSPEC));

            /* 1. Read bitmap file */
            rtCode = ReadDIBFile(&dibSpec, srcFile);
            if (IPP_STATUS_NOERR!=rtCode) {
                if (IPP_STATUS_NOMEM_ERR==rtCode) {
                    IPP_Log(pLogFileName,"a","IPP Error:read dib file, no memory error\n");  
                }else if (IPP_STATUS_NOTSUPPORTED_ERR==rtCode) {
                    IPP_Log(pLogFileName,"a","IPP Error:read dib file, non-supported format of dib file\n");  
                }else if(IPP_STATUS_BITSTREAM_ERR==rtCode){
                    IPP_Log(pLogFileName,"a","IPP Error:read dib file, bit stream error\n");  
                }else if (IPP_STATUS_ERR==rtCode) {
                    IPP_Log(pLogFileName,"a","IPP Error:read dib file, error\n");  
                }
                DestroyDIB(&dibSpec);
                IPP_Printf("JPEG Encode Failed\n");
                ret = IPP_FAIL;
                goto Encode_Done;
            }


            if(NULL!=dibSpec.pBitmapData){
                parameters.nBufType = JPEG_INTEGRATEBUF;                
            }
            
            ShowImageAttr(&dibSpec);

            /* 3. Encoding */
            if (IPP_STATUS_NOERR!=BMPtoJPEG(parameters.nQuality, 
                parameters.nInterval,
                parameters.nJPEGMode,
                parameters.nSubsampling,
                parameters.nBufType,
                &dibSpec, 
                srcFile,
                dstFile)){
                    IPP_Log(pLogFileName,"a","IPP Error:bmp to jpeg, compress error\n");  
                    DestroyDIB(&dibSpec);
                    IPP_Printf("JPEG Encode Failed\n");
                    ret = IPP_FAIL;
                    goto Encode_Done;
            }
            DestroyDIB(&dibSpec);

            /* 3. print decoding ratio */
            if(NULL!=srcFile){
                nSrcFileLen=IPP_Ftell(srcFile);
                IPP_Printf("Source file length:       %d\n", nSrcFileLen);
            }
            if(NULL!=dstFile){
                nDstFileLen=IPP_Ftell(dstFile);
                IPP_Printf("Destination file length:  %d\n", nDstFileLen);
            }                               
            IPP_Printf ("BMP -> JPEG Ok.\n");
            break;
        }
    case YUV2JPG:
        {
            /* 1. Prepare yuv spec */
            MYUVSPEC yuvSpec;
            yuvSpec.nWidth          = parameters.nWidth;
            yuvSpec.nHeight         = parameters.nHeight;
            yuvSpec.nSamplingFormat = parameters.nFormat;
            yuvSpec.nStorageFormat  = YUV_PLANAR;

            IPP_Fseek(srcFile, 0, IPP_SEEK_END);
            yuvSpec.nDataSize = IPP_Ftell(srcFile);
            IPP_Fseek(srcFile, 0, IPP_SEEK_SET);

            /* 2. Load YUV file skipped */ 
            /* 3. encoding */
            if (IPP_STATUS_NOERR != YUVtoJPEG(parameters.nQuality,
                parameters.nInterval,  
                parameters.nJPEGMode,
                parameters.nSubsampling,
                parameters.nBufType,
                &yuvSpec,
                srcFile,
                dstFile)){
                    IPP_Log(pLogFileName,"a","IPP Error:yuv to jpeg, compress error\n");  
                    IPP_Printf("JPEG Encode Failed\n");
                    ret = IPP_FAIL;
                    goto Encode_Done;
            }

            /* 4. print encoding ratio */
            if(NULL!=srcFile){
                nSrcFileLen=IPP_Ftell(srcFile);
                IPP_Printf("Source file length:       %d\n", nSrcFileLen);
            }
            if(NULL!=dstFile){
                nDstFileLen=IPP_Ftell(dstFile);
                IPP_Printf("Destination file length:  %d\n", nDstFileLen);
            }    
            IPP_Printf ("YUV -> JPG Ok.\n");
            break;
        }
    default:        
       IPP_Log(pLogFileName,"a","IPP Error:command error\n");  
        IPP_Printf("JPEG Encode Failed\n");
        ret = IPP_FAIL;
        goto Encode_Done;
    }

    g_Frame_Num[IPP_VIDEO_INDEX]  = 1;

Encode_Done: 
    if(NULL!=srcFile){
        IPP_Fclose(srcFile);
        srcFile = NULL;
    }
    if(NULL!=dstFile){
        IPP_Fclose(dstFile);
        dstFile = NULL;
    }

    if(NULL != pSrcFileName ){
        IPP_MemFree((void **)(void**)&pSrcFileName);
        pSrcFileName = NULL;
    }
    if(NULL != pDstFileName ){
        IPP_MemFree((void **)(void**)&pDstFileName);
        pDstFileName = NULL;
    }
    if(NULL != pLogFileName ){
        IPP_MemFree((void **)(void**)&pLogFileName);
        pLogFileName = NULL;
    }

    if(IPP_OK!=ret){
        IPP_Printf("JPEG Encode Failed\n");
    }else{
        IPP_Printf("JPEG Encode Success\n");
    }
    return ret;

}




/******************************************************************************
// Name:  CodecTest
// Description:			Main entry for JPEG encoder test
// Input Arguments:
//      N/A
// Output Arguments:	
//      N/A
// Returns:
//      IPP_OK   - Run JPEGEnc
//      IPP_FAIL - Show Usage
******************************************************************************/
int CodecTest(int argc, char** argv)
{
    if(argc == 2 ){   
		DisplayLibVersion();
        return JPEGEnc(argv[1]);
    }else{
        IPP_Log(NULL,"w",
            "Command is incorrect!\nUsage:appJPEGEnc.exe \"-i:srcfile -o:dstfile [-l:logfile] [-b:0] [-q:50]  [-v:0]   [-m:0]   [-s:411] [-w:176] [-h:144] [-f:411] [-d:0]\"\n \
             -i         Input file\n                                                               \
            -o         Output file\n                                                              \
            -l          Log file\n                                                                 \
            Compression command options:\n  \
            -b:       Buffer type     (0: JPEG_SLICEBUF,  1: JPEG_INTEGRATEBUF) \n \
            -q         Quality          [0, 100]\n \
            -v         Restart Interval [0, 6]\n \
            -m        JPEG Mode        (0: Baseline, 1: Progressive)\n \
            -s         Subsampling      (444, 422, 411)for RGB input only\n \
            -w        Image width                     for YUV input only\n \
            -h         Image height                    for YUV input only\n \
            -f          Image format     (444, 422, 411)for YUV input only\n \
            \nDecompression command options:\n \
            -d         Desired Color    (0: RGB, 1: RGB555, 2: RGB565) for RGB output only\n \
            ");
		return IPP_FAIL;
    }

    
}
/* EOF */
