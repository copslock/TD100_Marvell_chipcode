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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "misc.h"
#include "codecPNG.h"
#include "dib.h"

#include "ippLV.h"  

#if defined(_IPP_X86) || defined(_IPP_WINCE)
#include <windows.h>
#endif /*_IPP_X86 || _IPP_WINCE*/

#ifdef _IPP_LINUX
#include <sys/time.h>
#endif /*_IPP_LINUX*/

#define PNG_INP_STREAM_BUF_LEN 20480
#define PNG_OUT_STREAM_BUF_LEN 20480
#define EXIT_MEMFREE \
	if (srcFile && errFile) {\
	fprintf(errFile,"encounter error when decode file: %s\n", pSrcFileName);\
	}\
	if (srcFile) {\
	fclose(srcFile); srcFile = NULL;\
	}\
	if (dstFile) {\
	fclose(dstFile); dstFile = NULL;\
	}\
	if (logFile) {\
	fclose(logFile); logFile = NULL;\
	}\
	if (alphaFile) {\
	fclose(alphaFile); dstFile = NULL;\
	}\
	if (pDecoderState) {\
	DecoderFree_PNG(&pDecoderState);\
	}\
	if (pBasePtr) {\
	free(pBasePtr); pBasePtr = NULL;\
	}\
	if (pBaseAlpha) {\
	free(pBaseAlpha); pBaseAlpha = NULL;\
	}\
	

#define EXIT_MEMFREE_ALL  EXIT_MEMFREE ;\
	if (SrcBitStream.pBsBuffer) {\
	free(SrcBitStream.pBsBuffer); SrcBitStream.pBsBuffer = NULL;\
	}\
	if (parFile) {\
	fclose(parFile); parFile = NULL;\
	}\
	if (perfFile) {\
	fclose(perfFile); perfFile = NULL;\
	}\
	if (errFile) {\
	fclose(errFile); errFile = NULL;\
	}\
	miscFreeGeneralCallbackTable(&pCallBackTable); pCallBackTable = NULL
	
	

#define ENCOUNTER_BAD_STREAM \
	SrcBitStream.bsByteLen = PNG_INP_STREAM_BUF_LEN;\
	continue
#define ENCOUNTER_BAD_FILE ENCOUNTER_BAD_STREAM

	
int PNGClrFmt[] = {
    IPP_BGR888,
    IPP_BGR555,
    IPP_BGR565,
    IPP_BGRA8888
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
	ret = GetLibVersion_PNGDEC(libversion,sizeof(libversion));
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
// Name:				_initAncillary
// Description:			Initialize ancillary information, including content  
//                      about ancillary chunk and its availability
//
// Input Arguments:
//		pAncillary:	    Pointer to ancillary info structure
//
// Output Arguments:	
//		pAncillary:	    Pointer to initialized ancillary info structure
// Returns:
//        [NONE]			
******************************************************************************/

void _initAncillary( IppPNGAncillaryInfo *pAncillary)
{
	pAncillary->gama                =   0;
    pAncillary->pAlpha              =   NULL;  /* we assume no alpha info */
    pAncillary->greyBkgd            =   0;
    pAncillary->RGBBkgd[0]          =   0;
    pAncillary->RGBBkgd[1]          =   0;
    pAncillary->RGBBkgd[2]          =   0;
    pAncillary->plteBkgd            =   0;
    pAncillary->RGBTrans[0]         =   0;
    pAncillary->RGBTrans[1]         =   0;
    pAncillary->RGBTrans[2]         =   0;
    pAncillary->greyTrans           =   0;
    pAncillary->sRGB                =   0;
    pAncillary->AncillaryInfoFlag   =   0;
}

/******************************************************************************
// Name:				ParseNextCmd
// Description:			Parse the user command and update the decoder config
//						structure.
//
// Input Arguments:
//			pNextCmd:	Pointer to the input cmd buffer
//		pSrcFileName:	Pointer to last src file name
//
// Output Arguments:	
//		pSrcFileName:	Pointer to current src file name
//		pDstFileName:	Pointer to dst file name
//		pDecoderPar:	Pointer to updated decoder config structure
// Returns:
//        [Success]		General IPP return code
//        [Failure]		General IPP return code					
******************************************************************************/

IppCodecStatus ParseNextCmd(char *pNextCmd, char *pSrcFileName, char *pDstFileName, 
							IppPNGDecoderParam *pDecoderPar)
{
	int iLength;
	char *pIdx, *pCurPos, *pEnd, *pMaskStrEnd, pFormatStr[13];
	char pMaskStrFmt[] = "PNG_BGR888 PNG_BGR555 PNG_BGR565 PNG_ABGR";

	/* Parse comments */
	if(pNextCmd[0] == '#' || pNextCmd[0] == '\t'|| pNextCmd[0] == ' ' || pNextCmd[0] == '\0') {
		return IPP_STATUS_ERR;
	}

	pCurPos = pNextCmd;

	while((pIdx = strstr(pCurPos, "-"))) {
		
		pEnd = strstr(pIdx+1, "-");
		if(!pEnd) {
			pEnd = pNextCmd + strlen(pNextCmd);
			iLength = (int)(pEnd - pIdx) - 3;
		} else {
			iLength = (int)(pEnd - pIdx) - 4;
		}


		switch(pIdx[1]) {
		case 'i':
        case 'I':
			strncpy(pSrcFileName, pIdx + 3, iLength);
			pSrcFileName [iLength] = '\0';
			
			break;
		case 'o':
        case 'O':
			strncpy(pDstFileName, pIdx + 3, iLength);
			pDstFileName [iLength] = '\0';
			
			break;
		case 'w':
        case 'W':
			pDecoderPar->nDesiredImageSize.width = atoi(pIdx+3);

			break;
		case 'h':
        case 'H':
			pDecoderPar->nDesiredImageSize.height = atoi(pIdx+3);

			break;
		case 'f':
        case 'F':
            /* Output format */
			strncpy(pFormatStr, pIdx + 3, iLength);
			pFormatStr [iLength] = '\0';
			pMaskStrEnd = strstr(pMaskStrFmt, pFormatStr);
			
			pDecoderPar->nDesiredClrMd = PNGClrFmt[(pMaskStrEnd-pMaskStrFmt)/11];

			if(pDecoderPar->nDesiredClrMd < IPP_BGR888 || pDecoderPar->nDesiredClrMd > IPP_BGR555) {
				return IPP_STATUS_ERR;
			}
			break;
		case 'a':
        case 'A':
            /* default alpha value, if we don't have alpha channel in image */
			pDecoderPar->nAlphaValue = (Ipp8u)atoi(pIdx+3);

			break;

		default:
			return IPP_STATUS_ERR;
		}

		pCurPos = pEnd;
	}

	return IPP_STATUS_NOERR;
}

/******************************************************************************
// Name:				wmain
// Description:			Main entry for PNG decoder
//
// Input Arguments:
//						N/A
// Output Arguments:	
//						N/A
// Returns:
******************************************************************************/
#ifndef WINCE
int main()
#else
int wmain()
#endif
{
	FILE *srcFile = NULL, *dstFile = NULL, *parFile = NULL, *errFile = NULL;
	FILE *logFile = NULL, *alphaFile = NULL, *perfFile = NULL;
	MDIBSPEC dibSpec;
	MiscGeneralCallbackTable    *pCallBackTable;
    IppPNGDecoderParam          DecoderPar;
    IppPicture                  DstPicture;
    IppBitstream                SrcBitStream;  
    void *pDecoderState = NULL;	
	char pNextCmd[256], pSrcFileName[256], pDstFileName[256], pLogFileName[256], pAlphaFileName[256];
	int   iLength;
	Ipp8u *pBasePtr = NULL,     *pBaseAlpha = NULL, *pAlpha = NULL;
    IppPNGAncillaryInfo         ancillaryInfo;
	int i, j, nBytesLeft, nReadByte;
	int Done;
	IppCodecStatus rtCode; 
    unsigned long ts, TotTime;

#ifdef _IPP_LINUX
    struct timeval tv1, tv2;
    struct timezone tz;	
    unsigned long total_time=0;
#endif /*_IPP_LINUX*/

	int bErStream = 0; /*use for encountering bad stream, to quit the for loop*/	
	char *pIdx = NULL;
	int length;
	int data32;
	int AncillaryInfoFlag;

	DisplayLibVersion();

	/* Init callback table */
	if ( miscInitGeneralCallbackTable(&pCallBackTable) != 0 ) {
		return IPP_STATUS_ERR; 
	}

	/* Allocate the input stream buffer */
    SrcBitStream.pBsBuffer = (Ipp8u *)malloc( PNG_INP_STREAM_BUF_LEN);
    SrcBitStream.bsByteLen = PNG_INP_STREAM_BUF_LEN;
    SrcBitStream.bsCurBitOffset = 0;
    if (NULL == SrcBitStream.pBsBuffer) {
		return IPP_STATUS_NOMEM_ERR;
	}
    
	if( NULL == (parFile = fopen("pngdec.cfg", "rt"))) {
        free(SrcBitStream.pBsBuffer);
		return -1;
	}

	if( NULL == (perfFile = fopen("performanceLog.txt", "wt"))) {
        free(SrcBitStream.pBsBuffer);
		fclose(parFile);
		return -1;
	}
	
	if (NULL == (errFile = fopen("error.log", "wt"))) {
		free(SrcBitStream.pBsBuffer);
		fclose(parFile);
		fclose(perfFile);
		return -1;
	}

	while (!feof(parFile)) {
        Done = 0;
        TotTime = 0;
		bErStream = 0;
		DecoderPar.nDesiredImageSize.height = -1;
		DecoderPar.nDesiredImageSize.width = -1;

		/* Get next operation */
		if(NULL == fgets(pNextCmd, 256, parFile)) {
			break;
		}
		
		/* Remove \r\n */
		iLength = (int)strlen(pNextCmd);
		
		if(iLength <= 1) {
			continue;
		}

		if(pNextCmd[iLength-2] == '\r') {
			pNextCmd[iLength-2] = '\0';
		} else {
			pNextCmd[iLength-1] = '\0';
		}

		if (ParseNextCmd(pNextCmd, pSrcFileName, pDstFileName, &DecoderPar) ) {
			continue;
		}
		
		if (NULL == (srcFile = fopen(pSrcFileName, "rb"))) {			
			EXIT_MEMFREE ;
			ENCOUNTER_BAD_FILE ;
            return -1;
        }
        if (NULL == (dstFile = fopen(pDstFileName, "wb"))) {
			EXIT_MEMFREE ;
			ENCOUNTER_BAD_FILE;
            return -1;
        }


        /* Read source stream */
        nReadByte = (int)fread(SrcBitStream.pBsBuffer, 1, SrcBitStream.bsByteLen, srcFile);
	    if(!nReadByte) {
			EXIT_MEMFREE ;			
			ENCOUNTER_BAD_FILE ;
		    return IPP_STATUS_ERR;
	    }
	    SrcBitStream.pBsCurByte = SrcBitStream.pBsBuffer;
	    SrcBitStream.bsByteLen = nReadByte;
	    SrcBitStream.bsCurBitOffset = 0;

		/*open logFile name*/
		pIdx = strstr(pDstFileName, ".");
		length = pIdx - pDstFileName;
		strncpy(pLogFileName, pDstFileName, length);
		pLogFileName[length]  = '\0';
		strcat(pLogFileName, "Log.txt");
		if( NULL == (logFile = fopen(pLogFileName, "wb"))) {
			EXIT_MEMFREE ;			
			ENCOUNTER_BAD_FILE ;
		}	

        /* Parse header of image and get basic info about it */
        rtCode = DecoderInitAlloc_PNG(&SrcBitStream,
			                           &DstPicture,
			                           &pDecoderState,
			                           pCallBackTable);
        if ( rtCode != IPP_STATUS_NOERR ) { 
			EXIT_MEMFREE; 
			ENCOUNTER_BAD_STREAM;	
        }

		/*after DecoderInitAlloc_PNG, log information about: source picture Width&Height, 
		whether alpha info exists and format*/
		fprintf(logFile,"%s size:%d X %d, format:%d\n", pSrcFileName,
			DstPicture.picWidth, DstPicture.picHeight, 	DstPicture.picFormat);

		/* Set output picture attributes */
        // desiredWidth and height must be changed by user,  
        // based on source image attributte we get
        /*DecoderPar.nDesiredImageSize.width = 640;
        DecoderPar.nDesiredImageSize.height= 480;*/
		
        DecoderPar.nDesiredImageSize.width  = dibSpec.nWidth = DecoderPar.nDesiredImageSize.width > 0 
			? DecoderPar.nDesiredImageSize.width
			: DstPicture.picWidth;
	    DecoderPar.nDesiredImageSize.height = dibSpec.nHeight = DecoderPar.nDesiredImageSize.height > 0
			? DecoderPar.nDesiredImageSize.height
			: DstPicture.picHeight;
	    dibSpec.nPrecision    = 8;                                 /* not support 16-bits precision currently */
	    dibSpec.nClrMode      = DecoderPar.nDesiredClrMd;
        switch (DecoderPar.nDesiredClrMd) {
            case IPP_BGR888:
                dibSpec.nBitsPerpixel = 24;
                break;
            case IPP_BGRA8888:
                dibSpec.nBitsPerpixel = 32;
                break;
            case IPP_BGR565:
            case IPP_BGR555:
                dibSpec.nBitsPerpixel = 16;
                break;
            default:
				EXIT_MEMFREE;
				ENCOUNTER_BAD_STREAM;
                return IPP_STATUS_NOTSUPPORTED_ERR;
        }
	    dibSpec.nStep         = IIP_WIDTHBYTES_4B((dibSpec.nWidth) * (dibSpec.nBitsPerpixel));
	    dibSpec.nDataSize     = (dibSpec.nStep) * (dibSpec.nHeight);        

		if (pBasePtr) {
			free(pBasePtr);
			pBasePtr = NULL;
		}

		/* Allocate the output picture buffer */		
		pBasePtr = (Ipp8u *) malloc(dibSpec.nDataSize + 8);
		if (NULL == pBasePtr) {
			EXIT_MEMFREE;			
			continue;						
		}
		memset(pBasePtr, 0, dibSpec.nDataSize + 8);

		/* Force 8-byte aligned */
		dibSpec.pBitmapData = (Ipp8u*)(((unsigned int)pBasePtr & 0xFFFFFFF8) + 8);

		/* Only bottom-up BMP is supported here */
		DstPicture.picPlaneStep[0]  = -(dibSpec.nStep);
		DstPicture.ppPicPlane[0]    = &dibSpec.pBitmapData[dibSpec.nStep*(dibSpec.nHeight-1)];

        _initAncillary( &ancillaryInfo );
        /* Have alpha info */
		if ( IPP_PNG_GREY_A == DstPicture.picFormat || IPP_PNG_TRUECOLOR_A == DstPicture.picFormat) {
			if (pBaseAlpha) {
				free(pBaseAlpha);
				pBaseAlpha = NULL;
			}

			pBaseAlpha = (Ipp8u *)malloc(dibSpec.nWidth * dibSpec.nHeight);
			if ( NULL == pBaseAlpha ) {
				EXIT_MEMFREE;
				continue;
			}
			memset(pBaseAlpha, 0, dibSpec.nWidth * dibSpec.nHeight);
		    /*bottom up*/
            ancillaryInfo.AncillaryInfoFlag |= 0x40; //indicate that alpha info is available
		    ancillaryInfo.pAlpha = pBaseAlpha + dibSpec.nWidth * (dibSpec.nHeight - 1);
            DstPicture.picPlaneStep[1] = -dibSpec.nWidth;			
        }

		/* Call the core PNG decoder function */
		while(!Done && !bErStream) {

#if defined(_IPP_X86) || defined(_IPP_WINCE)
			ts = GetTickCount();            
#endif /*_IPP_X86 || _IPP_WINCE*/
			
#ifdef _IPP_LINUX
            gettimeofday(&tv1, &tz);
#endif /*_IPP_LINUX*/
			
			rtCode = Decode_PNG (&SrcBitStream, &DecoderPar, &DstPicture, &ancillaryInfo, pDecoderState);

#ifdef _IPP_LINUX
            gettimeofday(&tv2, &tz);
            TotTime += (tv2.tv_sec - tv1.tv_sec) * 1000+ (tv2.tv_usec - tv1.tv_usec)/1000;
#endif /*_IPP_LINUX*/

#if defined(_IPP_X86) || defined(_IPP_WINCE)
            TotTime += GetTickCount() - ts;
#endif /*_IPP_X86 || _IPP_WINCE*/				

			switch(rtCode) {
			case IPP_STATUS_NEED_INPUT:
				nBytesLeft = SrcBitStream.bsByteLen - (SrcBitStream.pBsCurByte - SrcBitStream.pBsBuffer);
				/* move data left in last decode to the beginning of buffer */
				for (i = 0; i < nBytesLeft; i++) {
					*(SrcBitStream.pBsBuffer + i) = *SrcBitStream.pBsCurByte++;
				}

				nReadByte = (int)fread((void*)(SrcBitStream.pBsBuffer + i), 1, 
						SrcBitStream.bsByteLen - nBytesLeft, srcFile);
				if(!nReadByte) {
					bErStream = 1;
					ENCOUNTER_BAD_STREAM;					
					return IPP_STATUS_ERR;
				}
				SrcBitStream.pBsCurByte = SrcBitStream.pBsBuffer;
				SrcBitStream.bsByteLen = nReadByte + i;
				SrcBitStream.bsCurBitOffset = 0;
				break;

			case IPP_STATUS_NOERR:
				Done = 1;
				break;

			case IPP_STATUS_BADARG_ERR:
			case IPP_STATUS_NOMEM_ERR:
			case IPP_STATUS_BITSTREAM_ERR:
			case IPP_STATUS_NOTSUPPORTED_ERR:
			case IPP_STATUS_ERR:
			default:
				bErStream = 1;				
				ENCOUNTER_BAD_STREAM;
				return -1;
			}
		}

		if (bErStream) {
			EXIT_MEMFREE;
			ENCOUNTER_BAD_STREAM;
		}
        
		/* Output the BMP data */
		if (IPP_STATUS_NOERR != WriteDIBFile(&dibSpec, dstFile)) {
			EXIT_MEMFREE ;      
			ENCOUNTER_BAD_STREAM;
		}

		/*after decode, log PNG info*/
		fprintf(perfFile,"%s cost %ld ms\n", pSrcFileName, TotTime);
		fprintf(logFile,"%s cost %ld ms\n", pSrcFileName, TotTime);
		AncillaryInfoFlag = ancillaryInfo.AncillaryInfoFlag ;
		if (AncillaryInfoFlag&(1 << 7)) {
			fprintf(logFile,"has Gama: %d\n", ancillaryInfo.gama);
		}
		if (AncillaryInfoFlag&(1 << 6)) {
			fprintf(logFile,"has alpha value \n");
			/*open alpha value logfile*/
			pIdx = strstr(pDstFileName, ".");
			length = pIdx - pDstFileName;
			strncpy(pAlphaFileName, pDstFileName, length);
			pAlphaFileName[length]  = '\0';		
			strcat(pAlphaFileName, "Alpha.dat");
			if( NULL == (alphaFile = fopen(pAlphaFileName, "wb"))) {
				EXIT_MEMFREE ;			
				ENCOUNTER_BAD_FILE ;
			}	

			//dump alpha info
			pAlpha = pBaseAlpha;
			for (i=0; i<dibSpec.nWidth; i++) {
				for (j=0; j<dibSpec.nHeight;j++) {
					fprintf(alphaFile, "%d\n", *pAlpha);
					pAlpha++;
				}
			}
			fclose(alphaFile); alphaFile = NULL;
		}
		if (AncillaryInfoFlag&(1 << 5)) {
			fprintf(logFile,"has greybkgd: %d\n", ancillaryInfo.greyBkgd);
		}
		if (AncillaryInfoFlag&(1 << 4)) {
			fprintf(logFile,"has RGBbkgd: %d %d %d\n", ancillaryInfo.RGBBkgd[0],
				ancillaryInfo.RGBBkgd[1], ancillaryInfo.RGBBkgd[2]);
		}
		if (AncillaryInfoFlag&(1 << 3)) {
			fprintf(logFile,"has PLTEbkgd: %d\n", ancillaryInfo.plteBkgd);
		}
		if (AncillaryInfoFlag&(1 << 2)) {
			fprintf(logFile,"has greyTrans: %d\n", ancillaryInfo.greyTrans);
		}
		if (AncillaryInfoFlag&(1 << 1)) {
			fprintf(logFile,"has RGBTrans: %d %d %d\n", ancillaryInfo.RGBTrans[0],
				ancillaryInfo.RGBTrans[1], ancillaryInfo.RGBTrans[2]);
		}
		if (AncillaryInfoFlag&(1)) {
			fprintf(logFile,"has sRGB: %d\n", ancillaryInfo.sRGB);
		}
		fprintf(logFile,"\n");
		fclose(logFile);
		logFile = NULL;

        DecoderFree_PNG(&pDecoderState);
        /* Re-init bsBypteLen var, because it may be modified in this decode process */
        SrcBitStream.bsByteLen = PNG_INP_STREAM_BUF_LEN;
		
		/* Close file handler */
		fclose(srcFile);		
		fclose(dstFile);
		srcFile = dstFile = NULL;
	}

	EXIT_MEMFREE_ALL ; 	
	return 0;
}
/* EOF */
