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
#include "misc.h"
#include "dib.h"
#include "jpqueue.h"
#include "ijedcmd.h"

#include "ippLV.h"



#ifdef MULTI_THREAD
void * hEventBufFull;
void * hEventBufEmpty;
JpegBufQueue bufQFull;
JpegBufQueue bufQEmpty;
int bExit=0;
#endif

int flagEos = 0;
int g_stream_read_perf_index;

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
	 24, /* JPEG_RGB888 */
    16, /* JPEG_RGB555 */
    16, /* JPEG_RGB565 */
    32, /* JPEG_ARGB */
};

const IppJPEGColorFormat JPEGYUVFormat[] = 
{
    JPEG_YUV444, JPEG_YUV422, JPEG_YUV422I, JPEG_YUV411, JPEG_GRAY8
};


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
	ret = GetLibVersion_JPEGDEC(libversion,sizeof(libversion));
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
// Name:		ReadStreamFromFile
// Description:		Read stream from a file
//
// Input Arguments:
//	  iSize:	Item size in bytes
//	 iCount:	Maximum number of items to be read
//	 pHandler:	Point to the FILE structure
//
// Output Arguments:	
//	pDstBuf:	Pointer to the dst buffer
//
// Returns:
//        Return the actual read data size in byte
 ******************************************************************************/
#ifdef MULTI_THREAD

int ReadStreamFromFile(void **pDstBuf, int iSize, int iCount, void *pHandler)
{
	int ret;
	int bTimeOut;
	int size;

	if (NULL == *pDstBuf){
		IPP_EventWait(hEventBufFull, INFINITE_WAIT, &bTimeOut);
		JPEGBufQueue_DeQueue(&bufQFull, pDstBuf, &size);
		IPP_EventReset(hEventBufFull);
		ret = size;
	} else {
		JPEGBufQueue_EnQueue(&bufQEmpty, *pDstBuf, iSize*iCount);
		IPP_EventSet(hEventBufEmpty);
		IPP_EventWait(hEventBufFull, INFINITE_WAIT, &bTimeOut);
		JPEGBufQueue_DeQueue(&bufQFull, pDstBuf, &size);
		IPP_EventReset(hEventBufFull);
		ret = size;
	}
	if (!JPEG_IsBufQueue_Empty(&bufQFull)){
		IPP_EventSet(hEventBufFull);
	}
	return size;
}


int readThread(void *par){
	IPP_FILE *fp = (IPP_FILE *)par;
	char *pBuffer;
	int bTimeOut;
	int ret;
	jpegBufType *bufType;
	int size;
	void * pDstBuf;

	for (;;){
		IPP_StartPerfCounter(g_stream_read_perf_index);
		while((!JPEG_IsBufQueue_Empty(&bufQEmpty)) && (!bExit)){
			JPEGBufQueue_DeQueue(&bufQEmpty, &pDstBuf, &size);
			IPP_EventReset(hEventBufEmpty);

			/* Read data from FILE */
			ret = IPP_Fread(pDstBuf, 1, size, fp);
			//IPP_Printf("Read %d bytes from source.\n", ret);
			/* Insert EOI if encouting end-of-file */
			if((!flagEos) && (IPP_Feof(fp))) {
				pBuffer = (char *)pDstBuf;
				pBuffer[ret] = 0xFF;
				pBuffer[ret+1] = 0xD9;
				flagEos = 1;
				ret = ret+2;
			}
			JPEGBufQueue_EnQueue(&bufQFull, pDstBuf, ret);
			IPP_EventSet(hEventBufFull);
		}
		IPP_StopPerfCounter(g_stream_read_perf_index);
		if((ret == 0) || bExit){
			break;
		}
		IPP_EventWait(hEventBufEmpty, INFINITE_WAIT, &bTimeOut);
	}
    return 0;
}
#else
int ReadStreamFromFile(void **pDstBuf, int iSize, int iCount, void *pHandler)
{
	IPP_FILE *fp = (IPP_FILE *)pHandler;
	char *pBuffer;
	int ret;

	IPP_StartPerfCounter(g_stream_read_perf_index);
	/* Read data from FILE */
	ret = IPP_Fread(*pDstBuf, iSize, iCount, fp);
	/* Insert EOI if encouting end-of-file */
	if((!flagEos) && (IPP_Feof(fp))) {
		pBuffer = (char *)(*pDstBuf);
		pBuffer[ret*iSize  ] = 0xFF;
		pBuffer[ret*iSize+1] = 0xD9;

		flagEos = 1;

		IPP_StopPerfCounter(g_stream_read_perf_index);
		return (ret*iSize+2);
	}

	IPP_StopPerfCounter(g_stream_read_perf_index);
	return (ret*iSize);
}
#endif

/******************************************************************************
// Name:			ParseJPEGDecCmd
// Description:			Parse the user command and update the decoder config structure.
// Input Arguments:
//		pCmd:	Pointer to the input cmd buffer
// Output Arguments:	
//		pSrcFileName: Pointer to current src file name
//		pDstFileName: Pointer to dst file name
//		pLogFileName: Pointer to log file name
//		pDecoderPar:   Pointer to updated decoder config structure
//		pReInit:       Pointer to re-initialization flag
//     pStreanInsize: pointer to the size of input stream
// Returns:
//     [Success]		General IPP return code
//     [Failure]		General IPP return code					
******************************************************************************/

IppCodecStatus ParseJPEGDecCmd(char *pNextCmd, char *pSrcFileName, char *pDstFileName, char *pLogFileName, 
                            IppJPEGDecoderParam *pROIDecPar, int *pStreamInSize)
{
    int iLength, i = 0;
    char *pIdx, *pCurPos, *pEnd, *pMaskStrEnd, pFormatStr[13];
	 char pMaskStrFmt[] = "JPEG_BGR888 JPEG_BGR555 JPEG_BGR565 JPEG_ABGR   JPEG_RGB888 JPEG_RGB555 JPEG_RGB565 JPEG_ARGB   JPEG_YUV444 JPEG_YUV422 JPEG_YUV422IJPEG_YUV411 JPEG_GRAY8";    
	char pMaskStrRot[] = "IPP_JP_90LIPP_JP_180IPP_JP_90R";
	unsigned char bLPCutFreqFlag = 0;
	int temp;




    /* Parse comments */
    if(pNextCmd[0] == '#' || pNextCmd[0] == '\t'|| pNextCmd[0] == ' ' || pNextCmd[0] == '\0') {
        return IPP_STATUS_ERR;
    }

    /* set the default value */
    pROIDecPar->nAlphaValue = 0x80;

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
            IPP_Strncpy(pSrcFileName, pIdx + 3, iLength);
            pSrcFileName [iLength] = '\0';
            break;
        case 'o':
        case 'O':
            IPP_Strncpy(pDstFileName, pIdx + 3, iLength);
            pDstFileName [iLength] = '\0';
            break;
        case 'l':
        case 'L':
            IPP_Strncpy(pLogFileName, pIdx + 3, iLength);
            pLogFileName [iLength] = '\0';
            break;
        case 'r':
        case 'R':
            pROIDecPar->srcROI.x = IPP_Atoi(pIdx+5+1);
            pCurPos = pIdx+5+1;
            if(pIdx = IPP_Strstr(pCurPos, ", ")){
                pROIDecPar->srcROI.y = IPP_Atoi(pIdx+2);
            }
            pCurPos = pIdx+1;
            if(pIdx = IPP_Strstr(pCurPos, ", ")){
                pROIDecPar->srcROI.width = IPP_Atoi(pIdx+2);
            }
            pCurPos = pIdx+1;
            if(pIdx = IPP_Strstr(pCurPos, ", ")){
                pROIDecPar->srcROI.height = IPP_Atoi(pIdx+2);
            }
            break;
        case 'w':
        case 'W':
            //pROIDecPar->nDstWidth = IPP_Atoi(pIdx+7);
				
            break;
        case 'h':
        case 'H':
          //  pROIDecPar->nDstHeight = IPP_Atoi(pIdx+8);
            break;
        case 'f':
        case 'F':
            IPP_Strncpy(pFormatStr, pIdx + 3, iLength);
            pFormatStr [iLength] = '\0';
            pMaskStrEnd = IPP_Strstr(pMaskStrFmt, pFormatStr);
            pROIDecPar->nDesiredColor = (IppJPEGColorFormat)((pMaskStrEnd-pMaskStrFmt)/12);
            if(pROIDecPar->nDesiredColor < JPEG_BGR888 || pROIDecPar->nDesiredColor > JPEG_GRAY8) {
                return IPP_STATUS_ERR;
            }
            break;
        case 'a':
        case 'A':
            pROIDecPar->nAlphaValue = IPP_Atoi(pIdx+3);
            break;
        case 'b':
        case 'B':
            *pStreamInSize = IPP_Atoi(pIdx+3);
            break;
		 case 's':
        case 'S':
            pROIDecPar->nScale = IPP_Atoi(pIdx+3);
            break;
        default:
            break;
        }

        pCurPos = pEnd;
    }
	
    return IPP_STATUS_NOERR;
}



/******************************************************************************
// Name:				JPEGDec
// Description:			Main entry for JPEG decoder
// Input Arguments:
//     pCmd  - Pointer to the command line
// Output Arguments:
// Returns:
//     None
******************************************************************************/
int JPEGDec(char *pCmd)
{
	IPP_FILE *srcFile = NULL, *dstFile = NULL, *streamHandler=NULL;
	MDIBSPEC dibSpec;
	MiscGeneralCallbackTable *pCallBackTable = NULL;
	IppJPEGDecoderParam DecoderPar;
	IppPicture DstPicture;
	IppBitstream SrcBitStream, DstBitStream;  
	void *pDecoderState = NULL;
	char pSrcFileName[512];
	char pDstFileName[512];
	char pLogFileName[512];
	int iOutBufSize = 0, iSizeOut = 0, bReInit, iLength, bCloseFlag = 0;
	int iSizeIn, nDstWidth, nDstHeight, nChromaSize = 0;
	int iStreamInSize = 0;
	Ipp8u *pBasePtr = NULL, *ptr;
	IppCodecStatus rtCode; 
	int ret = IPP_OK;
	Ipp8u *pBaseStrPtr = NULL;
	int mode;

	int decode_perf_index, total_perf_index;
	DISPLAY_CB DisplayCB;
	long long nTotTime, nStreamReadTime, nDecodeTime;
#ifdef MULTI_THREAD
	int i;
	int bufferCount = 0;
	Ipp8u *pMTBuffer = NULL;
	int err;
	int size;
	int hThread = 0;
	bExit = 0;
#endif

	IPP_Memset(pSrcFileName, 0x00, sizeof(pSrcFileName));
	IPP_Memset(pDstFileName, 0x00, sizeof(pDstFileName));
	IPP_Memset(pLogFileName, 0x00, sizeof(pLogFileName));
	IPP_Memset(&DecoderPar, 0x00, sizeof(DecoderPar));
	DecoderPar.nScale = (8 << 16);
	iSizeIn = 1024*16;
	if (IPP_STATUS_NOERR!=ParseJPEGDecCmd(pCmd, pSrcFileName, pDstFileName, pLogFileName, &DecoderPar, &iSizeIn) ) {
		IPP_Log(pLogFileName,"a","IPP Error:command error\n");  
		IPP_Printf("JPEG parse cmd line Failed\n");
		return IPP_FAIL;    
	}

	if ( miscInitGeneralCallbackTable(&pCallBackTable) != 0 ) {
		IPP_Log(pLogFileName,"a","IPP Error:init callback table failed\n");
		IPP_Printf("JPEG init callback table Failed\n");
		return IPP_FAIL; 
	}
	/* Override the fFileRead callback function */
//	pCallBackTable->fFileRead = ReadStreamFromFile;

	if((pDstFileName[0])){
		dstFile = IPP_Fopen(pDstFileName, "wb");
		if(NULL == dstFile){
			IPP_Log(pLogFileName,"a","IPP Status:can not open dst file :%s\n",pDstFileName);              
		}
	}

	if((pSrcFileName[0])){
		srcFile = IPP_Fopen(pSrcFileName, "rb");
		if(NULL == srcFile){
			IPP_Log(pLogFileName,"a","IPP Status:can not open src file :%s\n",pSrcFileName);              
		}
	}

	
#ifdef MULTI_THREAD
	bufferCount = 2;
	JPEGBufQueue_Init(&bufQFull);
	JPEGBufQueue_Init(&bufQEmpty);
	err = IPP_EventCreate(&hEventBufEmpty);
	if (0 != err){
		ret = IPP_FAIL;
		goto Decode_Done;
	}
	err = IPP_EventCreate(&hEventBufFull);
	if (0 != err){
		ret = IPP_FAIL;
		goto Decode_Done;
	}
	IPP_EventReset(hEventBufEmpty);
	IPP_EventReset(hEventBufFull);
	if (JPEG_BUFQUEUE_MAX < bufferCount){
		bufferCount = JPEG_BUFQUEUE_MAX;
	}
	for (i=0;i<bufferCount;i++){
		pCallBackTable->fMemCalloc(&pMTBuffer, iSizeIn, 4);
		if (NULL == pMTBuffer){
			ret = IPP_FAIL;
			goto Decode_Done;
		} else {
			JPEGBufQueue_EnQueue(&bufQEmpty, pMTBuffer, iSizeIn);
		}
	}
	SrcBitStream.pBsBuffer = NULL;
	SrcBitStream.bsByteLen = iSizeIn;
	SrcBitStream.bsCurBitOffset = 0;
#else
	pCallBackTable->fMemCalloc(&pBaseStrPtr, iSizeIn, 1);
	if (NULL == pBaseStrPtr) {
		IPP_Log(pLogFileName,"a","IPP Error:malloc SrcBitStream failed\n");
		miscFreeGeneralCallbackTable(&pCallBackTable);
		IPP_Printf("JPEG malloc Failed\n");
		return IPP_FAIL;
	}
	SrcBitStream.pBsBuffer = pBaseStrPtr;
	SrcBitStream.bsByteLen = iSizeIn;
	SrcBitStream.bsCurBitOffset = 0;
#endif

	/* Reset the stream read performance index */
	/* Total time, get absolute time */
        IPP_GetPerfCounter(&total_perf_index, DEFAULT_TIMINGFUNC_START, DEFAULT_TIMINGFUNC_STOP);
        IPP_ResetPerfCounter(total_perf_index);

#ifdef MULTI_THREAD
		/* Decode time, thread time */
        IPP_GetPerfCounter(&decode_perf_index, IPP_TimeGetThreadTime, IPP_TimeGetThreadTime);
        IPP_ResetPerfCounter(decode_perf_index);

	/* Stream read time, thread time */
	IPP_GetPerfCounter(&g_stream_read_perf_index, IPP_TimeGetThreadTime, IPP_TimeGetThreadTime);
	IPP_ResetPerfCounter(g_stream_read_perf_index);    
#else
		/* Decode time, thread time */
        IPP_GetPerfCounter(&decode_perf_index, DEFAULT_TIMINGFUNC_START, DEFAULT_TIMINGFUNC_STOP);
        IPP_ResetPerfCounter(decode_perf_index);

	/* Stream read time, thread time */
	IPP_GetPerfCounter(&g_stream_read_perf_index, DEFAULT_TIMINGFUNC_START, DEFAULT_TIMINGFUNC_STOP);
	IPP_ResetPerfCounter(g_stream_read_perf_index);    
#endif

#ifdef MULTI_THREAD
		err = IPP_ThreadCreate(&hThread, 0, readThread, srcFile);
		if (0!=err){
			ret = IPP_FAIL;
			goto Decode_Done;
		}
#endif
		IPP_StartPerfCounter(total_perf_index);
		rtCode = DecoderInitAlloc_JPEG(&SrcBitStream,&DstPicture, pCallBackTable, &pDecoderState,srcFile);
		if(rtCode) {
			IPP_Log(pLogFileName,"a","IPP Error:initialize decoder failed\n");  
			ret = IPP_FAIL;
#ifdef MULTI_THREAD
				JPEGBufQueue_EnQueue(&bufQEmpty, SrcBitStream.pBsBuffer, SrcBitStream.bsByteLen);
#endif			
			goto Decode_Done;
		}
	

		if((DecoderPar.srcROI.width == 0) || (DecoderPar.srcROI.height == 0)){
			DecoderPar.srcROI.x = 0;
			DecoderPar.srcROI.y = 0;
            DecoderPar.srcROI.width = DstPicture.picWidth;
			DecoderPar.srcROI.height = DstPicture.picHeight;		
		}
		

		//DecoderPar.nScale = 8;

		nDstWidth = DstPicture.picWidth = ((DecoderPar.srcROI.width << 16) + DecoderPar.nScale - 1)/DecoderPar.nScale;
		nDstHeight = DstPicture.picHeight = ((DecoderPar.srcROI.height << 16) + DecoderPar.nScale - 1)/DecoderPar.nScale;

        /* RGB space */
        if(DecoderPar.nDesiredColor < JPEG_YUV444) {
            /* Set output picture attributes */
            dibSpec.nWidth			= nDstWidth;
            dibSpec.nHeight			= nDstHeight;
            dibSpec.nPrecision		= 8;
            dibSpec.nNumComponent	= DstPicture.picChannelNum;
            dibSpec.nClrMode		= DecoderPar.nDesiredColor;
            dibSpec.nBitsPerpixel	= JPEGBitsPerPixel[DecoderPar.nDesiredColor];	
            dibSpec.nStep			= IIP_WIDTHBYTES_4B((dibSpec.nWidth)*(dibSpec.nBitsPerpixel));
            dibSpec.nDataSize		= dibSpec.nStep * dibSpec.nHeight;

            /* Allocate the output picture buffer */
            pCallBackTable->fMemCalloc(&pBasePtr, dibSpec.nDataSize, 8);
            if(NULL==pBasePtr){
                IPP_Log(pLogFileName,"a","IPP Error: alloc roi bgr decoding pBasePtr failed\n");
                ret = IPP_FAIL;
                goto Decode_Done;
            }

            /* Force 8-byte aligned */
            dibSpec.pBitmapData = pBasePtr;
            /* Only bottom-up BMP is supported here */
            DstPicture.picPlaneStep[0]  =-(dibSpec.nStep);
            DstPicture.ppPicPlane[0]    = &dibSpec.pBitmapData[dibSpec.nStep*(dibSpec.nHeight-1)];
            DstPicture.picPlaneNum = 1;

        } else { /* YUV space */

			mode = DecoderPar.nDesiredColor - JPEG_YUV444;
			if((DecoderPar.nDesiredColor == JPEG_YUV422) || (DecoderPar.nDesiredColor == JPEG_YUV411)){
				nDstWidth = ((nDstWidth >> 1) << 1);
				DstPicture.picWidth = nDstWidth;
			}

			if((DecoderPar.nDesiredColor == JPEG_YUV422I) || (DecoderPar.nDesiredColor == JPEG_YUV411)){
				nDstHeight = ((nDstHeight >> 1) << 1);
				DstPicture.picHeight = nDstHeight;
			}
            if(DstPicture.picChannelNum==3) {
                nChromaSize = (nDstWidth>>JPEGSubSamplingTab[mode][0]) * (nDstHeight>>JPEGSubSamplingTab[mode][1]);
            } else {
                nChromaSize = 0;
            }

            iOutBufSize = nDstWidth*nDstHeight + nChromaSize*2;

            pCallBackTable->fMemCalloc(&pBasePtr, iOutBufSize, 8);
            if(NULL==pBasePtr){
                IPP_Log(pLogFileName,"a","IPP Error: alloc roi yuv decoding pBasePtr failed\n");
                ret = IPP_FAIL;
                goto Decode_Done;
            }       
            /* Force 8-byte aligned */
            ptr = pBasePtr;
            DstPicture.picPlaneNum = 3;
            DstPicture.picPlaneStep[0] = nDstWidth;
            DstPicture.ppPicPlane[0]   = ptr;
            DstPicture.picPlaneStep[1] = ((nDstWidth -1 + (1 << JPEGSubSamplingTab[mode][0]))>>JPEGSubSamplingTab[mode][0]);
            DstPicture.picPlaneStep[2] = ((nDstWidth - 1 +(1 << JPEGSubSamplingTab[mode][0]))>>JPEGSubSamplingTab[mode][0]);
            DstPicture.ppPicPlane[1]   = (Ipp8u*)DstPicture.ppPicPlane[0] + nDstWidth*nDstHeight;
            DstPicture.ppPicPlane[2]   = (Ipp8u*)DstPicture.ppPicPlane[1] + nChromaSize; 

		}

        IPP_StartPerfCounter(decode_perf_index);
        /* Call the core JPEG decoder function */
        rtCode = Decode_JPEG(&DstPicture, &DecoderPar, pDecoderState);

		IPP_StopPerfCounter(decode_perf_index);
        IPP_StopPerfCounter(total_perf_index);        
        
#ifdef MULTI_THREAD
				JPEGBufQueue_EnQueue(&bufQEmpty, SrcBitStream.pBsBuffer, SrcBitStream.bsByteLen);
#endif
        
        if(rtCode) {
            IPP_Log(pLogFileName,"a","IPP Error:roi decoding error\n");  
            ret = IPP_FAIL;
            goto Decode_Done;
        }
				
        /* Save the output */
        if(NULL!=dstFile){
            if(DecoderPar.nDesiredColor < JPEG_YUV444) {
                /* Output the BMP data */
                if (IPP_STATUS_NOERR != WriteDIBFile(&dibSpec, dstFile)) {
                    IPP_Log(pLogFileName,"a","IPP Error:destination file write error\n");  
                    ret = IPP_FAIL;
                    goto Decode_Done;
                }
            } else {
                /* Output the YUV data */
                IPP_Fwrite(DstPicture.ppPicPlane[0], 1, nDstWidth*nDstHeight + nChromaSize*2, dstFile);
            }
        }
        /* display one frame*/    
        display_open(&DisplayCB, nDstWidth,nDstHeight);
        display_JPEG(&DisplayCB, &DstPicture);
        display_close();
    

    nTotTime 		= IPP_GetPerfData(total_perf_index);
    nDecodeTime 	= IPP_GetPerfData(decode_perf_index);
    nStreamReadTime 	= IPP_GetPerfData(g_stream_read_perf_index);

    g_Tot_Time[IPP_VIDEO_INDEX] = nDecodeTime;
    g_Frame_Num[IPP_VIDEO_INDEX]  = 1;

    IPP_Printf("Stream name, Total App Time, Decode Time, File Load Time\n");
    IPP_Printf("%s, %lld, %lld, %lld\n", pDstFileName, nTotTime, nDecodeTime, nStreamReadTime);
    
Decode_Done:    
    /* Free stream read perf index */
    IPP_FreePerfCounter(total_perf_index);
    IPP_FreePerfCounter(decode_perf_index);
    IPP_FreePerfCounter(g_stream_read_perf_index);

    if(NULL!=srcFile){
        IPP_Fclose(srcFile);
        srcFile = NULL;
    }
    if(NULL!=dstFile){
        IPP_Fclose(dstFile);
        dstFile = NULL;
    }

    /* JPEG Free */
#ifdef MULTI_THREAD

	bExit = 1;
	for (i=0;i<bufferCount;i++){
		if (0==JPEGBufQueue_DeQueue(&bufQEmpty, &pMTBuffer, &size)){
			char * pTmp;
			pTmp = (int)pMTBuffer;
			pCallBackTable->fMemFree(&pTmp);
		}
	}
	/* make sure there is no in bufQFull, it should no buffer in bufQFull */
	for (i=0;i<bufferCount;i++){
		if (0==JPEGBufQueue_DeQueue(&bufQFull, &pMTBuffer, &size)){
			char * pTmp;
			pTmp = (int)pMTBuffer;
			pCallBackTable->fMemFree(&pTmp);
		}
	}

	JPEGBufQueue_Deinit(&bufQFull);
	JPEGBufQueue_Deinit(&bufQEmpty);
	
	IPP_EventSet(hEventBufEmpty);
	
	IPP_ThreadDestroy(&hThread, 1);
	
	IPP_EventDestroy(hEventBufEmpty);
	IPP_EventDestroy(hEventBufFull);
	
	

#else
    if(NULL!=SrcBitStream.pBsBuffer){
        SrcBitStream.pBsBuffer = NULL;
    }
#endif

		if(NULL!=pBasePtr){
        pCallBackTable->fMemFree(&pBasePtr);
        pBasePtr = NULL;
    }
    
    if(NULL!=pBaseStrPtr){
        pCallBackTable->fMemFree(&pBaseStrPtr);
        pBaseStrPtr = NULL;
    }
    
    if(NULL!=pDecoderState){
        DecoderFree_JPEG(&pDecoderState);
        pDecoderState = NULL;
    }
    if(NULL != pCallBackTable){
        miscFreeGeneralCallbackTable(&pCallBackTable);
        pCallBackTable = NULL;
    }

    
    pCallBackTable = NULL;

    if(IPP_OK!=ret){
        IPP_Printf("JPEG Decode Failed\n");
    }else{
        IPP_Printf("JPEG Decode Success\n");
    }

    return ret;
}


/******************************************************************************
// Name:  CodecTest
// Description:			Main entry for JPEG decoder test
// Input Arguments:
//      N/A
// Output Arguments:	
//      N/A
// Returns:
//      IPP_OK   - Run JPEGDec
//      IPP_FAIL - Show Usage
******************************************************************************/
//extern int nCodeLength[];
//extern int nTotalCoeff;
int CodecTest(int argc, char** argv)
{
    int ret = IPP_OK;
    if(argc == 2){    

		DisplayLibVersion();
		
		//int i, total=0;
        ret = JPEGDec(argv[1]);
		//IPP_Printf("Total %d coeff huffman decoded.\n", nTotalCoeff);
		/*
		for(i=0;i<16;i++) {
			total += nCodeLength[i];
		}
		IPP_Printf("Total %d DC decoded.\n", total);
		for(i=0;i<16;i++) {
			IPP_Printf("DC length %2d	%d	%.2f%%\n", i, nCodeLength[i], (float)nCodeLength[i]/total*100);
		}
		*/
		return ret;
    }else{
        IPP_Log(NULL,"w",
            "Command is incorrect! \n                                                       \
            Usage:appJPEGDec.exe \"-i:test.jpg -o:test.yuv -l:test.log -m:dec_mode -roi:[x_offset, y_offset, width, height] -xration:Q16 -yratio:Q16 -f:format -a:alpha -d:rotate -b:size\"\n \
            -i          Input file\n                                                               \
            -o         Output file\n                                                              \
            -l          Log file\n                                                                 \
            -roi       Src ROI region for ROI decoding, [x_offset, y_offset, width, height]\n \
            -s         Resizing ratio in x/y direction, Q16 format\n        \
            -f          Output image format, YUV or RGB space\n       \
            -a         Alpha value, only valid for JPEG_ABGR format\n \
            -b         Streaming input buffer size\n?                            \
            ");
		return IPP_FAIL;
    }    

}
/* EOF */


