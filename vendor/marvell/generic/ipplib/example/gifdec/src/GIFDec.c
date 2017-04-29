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

#include "misc.h"
#include "codecGIF.h"

#include "ippLV.h"

#if defined(_IPP_X86) || defined(_IPP_WINCE)
#include <windows.h>
#endif /*_IPP_X86 || _IPP_WINCE*/

#ifdef _IPP_LINUX
#include <sys/time.h>
#endif /*_IPP_LINUX*/

//#include "time.h"
#include "dib.h"


#define GIF_FIXED_BUFLEN 800

#define EXIT_MEMFREE \
	if (srcFile && logFile) {\
	fprintf(logFile,"encounter error when decode file: %s\n", pSrcFileName);\
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
	if (pDecoderState) {\
	DecoderFree_GIF(&pDecoderState);\
	}\
	if (pBasePtr) {\
	free(pBasePtr); pBasePtr = NULL;\
	}\
	

#define EXIT_MEMFREE_ALL  EXIT_MEMFREE ;\
	if (SrcBitStream.pBsBuffer) {\
	free(SrcBitStream.pBsBuffer); SrcBitStream.pBsBuffer = NULL;\
	}\
	if (parFile) {\
	fclose(parFile); parFile = NULL;\
	}\
	if (decTimeLog) {\
	fclose(decTimeLog); decTimeLog = NULL;\
	}\
	miscFreeGeneralCallbackTable(&pCallBackTable); pCallBackTable = NULL


#define ENCOUNTER_BAD_STREAM \
	fprintf(decTimeLog , "ENCOUNTER_BAD_STREAM \n");\
	SrcBitStream.bsByteLen = GIF_FIXED_BUFLEN;\
	continue
#define ENCOUNTER_BAD_FILE ENCOUNTER_BAD_STREAM

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
	ret = GetLibVersion_GIFDEC(libversion,sizeof(libversion));
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
							IppGIFDecoderParam *pDecoderPar)
{
	int iLength;
	char *pIdx, *pCurPos, *pEnd, *pMaskStrEnd, pFormatStr[13];
	char pMaskStrFmt[] = "GIF_BGR888 GIF_BGR555 GIF_BGR565 GIF_ABGR";
	int GIFClrFmt[] = {
		IPP_BGR888,
		IPP_BGR555,
		IPP_BGR565,
		IPP_BGRA8888
	};


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
		/*case 'w':  just for future enhauncement
        case 'W':
			pDecoderPar->nDesiredImageSize.width = atoi(pIdx+3);

			break;
		case 'h':
        case 'H':
			pDecoderPar->nDesiredImageSize.height = atoi(pIdx+3);

			break;*/
		case 'f':
        case 'F':
            /* Output format */
			strncpy(pFormatStr, pIdx + 3, iLength);
			pFormatStr [iLength] = '\0';
			pMaskStrEnd = strstr(pMaskStrFmt, pFormatStr);
			
			pDecoderPar->nDesiredClrMd = GIFClrFmt[(pMaskStrEnd-pMaskStrFmt)/11];

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
// Description:			Main entry for GIF decoder
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
	FILE *srcFile = NULL, *dstFile = NULL, *parFile = NULL, *decTimeLog = NULL,
		*logFile = NULL;
	MDIBSPEC dibSpec;
	MiscGeneralCallbackTable    *pCallBackTable = NULL;
	IppGIFDecoderParam          DecoderPar = {0};
    IppPicture                  DstPicture;
    IppBitstream                SrcBitStream;  
    void *pDecoderState = NULL;
	char pNextCmd[256], pSrcFileName[256], pDstFileName[256], pLogFileName[256],outFileName[256];
	int  iLength;
	Ipp8u *pBasePtr = NULL;   
	IppGIFPictureInfo GIFInfo = {0};
	int i, nBytesLeft, nReadByte;
	int Done;
	IppCodecStatus rtCode; 
	/*for record performance*/
    unsigned long ts, TotTime;
	unsigned long dwFileSz;

	int bErStream = 0;
	char *pIdx = NULL;
	int length;

#ifdef _IPP_LINUX
    struct timeval tv1, tv2;
    struct timezone tz;	
    unsigned long total_time=0;
#endif /*_IPP_LINUX*/

	DisplayLibVersion();

	/*this file just for record decoding time*/
	decTimeLog = fopen("gifDecTmLog.dat", "wb");

	/* Init callback table */
	if ( miscInitGeneralCallbackTable(&pCallBackTable) != 0 ) {
		return IPP_STATUS_ERR; 
	}

	/*memset DecoderPar, in fact, user should set the right GlbClrTbl[] table and nClrTblSz*/
	memset(DecoderPar.GlbClrTbl, 0, 256);
	DecoderPar.nClrTblSz = 256;

	/* Allocate the input stream buffer */
    SrcBitStream.pBsBuffer = (Ipp8u *)malloc( GIF_FIXED_BUFLEN);
    SrcBitStream.bsByteLen = GIF_FIXED_BUFLEN;
    SrcBitStream.bsCurBitOffset = 0;
    if (NULL == SrcBitStream.pBsBuffer) {		
		EXIT_MEMFREE_ALL ;
		return IPP_STATUS_NOMEM_ERR;
	}
    
	if( NULL == (parFile = fopen("gifdec.cfg", "rt"))) {        
		EXIT_MEMFREE_ALL;
		return -1;
	}


    while (!feof(parFile)) {
        Done = 0;
        TotTime = 0;
		bErStream = 0;
		

		/* Get next operation */
		if (NULL == fgets(pNextCmd, 256, parFile)) {
			break;
		}
		
		/* Remove \r\n */
		iLength = (int)strlen(pNextCmd);
		
		if (iLength <= 1) {
			continue;
		}

		if (pNextCmd[iLength-2] == '\r') {
			pNextCmd[iLength-2] = '\0';
		} else {
			pNextCmd[iLength-1] = '\0';
		}


		if (ParseNextCmd(pNextCmd, pSrcFileName, pDstFileName, &DecoderPar) ) {
			continue;
		}		
     
        if (NULL == (srcFile = fopen(pSrcFileName, "rb"))) {
			ENCOUNTER_BAD_FILE;
            return -1;
        }

		/*Get file size*/
		fseek(srcFile, 0, SEEK_END);
		dwFileSz = ftell(srcFile);
		fseek(srcFile, 0, SEEK_SET);

        /* Read source stream */
        nReadByte = (int)fread(SrcBitStream.pBsBuffer, 1, GIF_FIXED_BUFLEN, srcFile);
	    if (!nReadByte) {
			EXIT_MEMFREE ;
			ENCOUNTER_BAD_FILE ;
		    return IPP_STATUS_ERR;
	    }
	    SrcBitStream.pBsCurByte = SrcBitStream.pBsBuffer;
	    SrcBitStream.bsByteLen = nReadByte;
	    SrcBitStream.bsCurBitOffset = 0;

		/*open logFile name*/
		/*pIdx = strstr(pDstFileName, ".");
		length = pIdx - pDstFileName;
		strncpy(pLogFileName, pDstFileName, length);
		pLogFileName[length]  = '\0';		*/
		sprintf(pLogFileName, "%sLog.txt", pDstFileName);
		if( NULL == (logFile = fopen(pLogFileName, "wb"))) {
			EXIT_MEMFREE ;			
			ENCOUNTER_BAD_FILE ;
		}
		

        /* Parse header of image and get basic info about it */
		rtCode = DecoderInitAlloc_GIF(  &DecoderPar,
										&SrcBitStream,
										&DstPicture, 
										pCallBackTable,
										&pDecoderState);  
        if ( rtCode != IPP_STATUS_NOERR ) { 
			EXIT_MEMFREE; 
			ENCOUNTER_BAD_STREAM;
			return -1;
        }

		/*after DecoderInitAlloc_PNG, log information about :global color table 
		and background color*/
		fprintf(logFile,"global color table size:%d    backgound color is:%d \n", 
			DecoderPar.nClrTblSz, DecoderPar.nBkgndClrIdx);
		for (i = 0; i < DecoderPar.nClrTblSz; i++) {
			fprintf(logFile,"index i blue:%d  green:%d  red:%d   ", 
				i, DecoderPar.GlbClrTbl[i].blue, DecoderPar.GlbClrTbl[i].green, DecoderPar.GlbClrTbl[i].red);
			if (9 == i%10) {
				fprintf(logFile,"\n");
			}			
		}


		/* Set output picture attributes */
        // desiredWidth and height must be changed by user,  
        // based on source image attributte we get
		//but now, we don't support risize yet, so we set this size to source image width&height*/
        DecoderPar.nDesiredImageSize.width = DstPicture.picWidth;/*if resize, could be arbitrary value*/
        DecoderPar.nDesiredImageSize.height= DstPicture.picHeight;/*if resize, could be arbitrary value*/
        dibSpec.nWidth        = DecoderPar.nDesiredImageSize.width;
	    dibSpec.nHeight       = DecoderPar.nDesiredImageSize.height;
	    dibSpec.nPrecision    = 8;                                 /* not support 16-bits precision currently */
	    dibSpec.nNumComponent = DstPicture.picChannelNum;
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
                return IPP_STATUS_NOTSUPPORTED_ERR;
        }
	    dibSpec.nStep         = IIP_WIDTHBYTES_4B((dibSpec.nWidth) * (dibSpec.nBitsPerpixel));
	    dibSpec.nDataSize     = (dibSpec.nStep) * (dibSpec.nHeight);        

		/*size check*/
		if (dibSpec.nDataSize < 0) {
			EXIT_MEMFREE ;
			ENCOUNTER_BAD_STREAM;					
		}

		/* Allocate the output picture buffer */
		if (pBasePtr) {
			free(pBasePtr);
			pBasePtr = NULL;
		}
		pBasePtr = (Ipp8u *) malloc(dibSpec.nDataSize + 8);
		if ( NULL == pBasePtr) {
			EXIT_MEMFREE;
			ENCOUNTER_BAD_STREAM;						
		}
		memset(pBasePtr, 0, dibSpec.nDataSize + 8);
		
		/* Force 8-byte aligned */
		dibSpec.pBitmapData = (Ipp8u*)(((unsigned int)pBasePtr & 0xFFFFFFF8) + 8);

		/* Only bottom-up BMP is supported here */
		DstPicture.picHeight = dibSpec.nHeight; /*adjust picture attribute*/ 
		DstPicture.picWidth = dibSpec.nWidth;   /*according to users input*/
		DstPicture.picPlaneStep[0]  = -(dibSpec.nStep);
		DstPicture.ppPicPlane[0]    = &dibSpec.pBitmapData[dibSpec.nStep*(dibSpec.nHeight-1)];
 
		GIFInfo.nCurFrame = 0;
		/* Call the core GIF decoder function */
		while(!Done && !bErStream) {

#if defined(_IPP_X86) || defined(_IPP_WINCE)
			ts = GetTickCount();            
#endif /*_IPP_X86 || _IPP_WINCE*/
			
#ifdef _IPP_LINUX
            gettimeofday(&tv1, &tz);
#endif /*_IPP_LINUX*/

			rtCode = Decode_GIF(&SrcBitStream, &DstPicture, &GIFInfo, pDecoderState);

#ifdef _IPP_LINUX
            gettimeofday(&tv2, &tz);
            TotTime += (tv2.tv_sec - tv1.tv_sec) * 1000 + (tv2.tv_usec - tv1.tv_usec)/1000;
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
					EXIT_MEMFREE ;
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

			case IPP_STATUS_GIF_MORE: /*write DIB file*/
				sprintf(outFileName, "%s_%d.bmp", pDstFileName, ++GIFInfo.nCurFrame);
				if (NULL == (dstFile = fopen(outFileName, "wb"))) {
					printf("Destination file open error.\n");					
					bErStream = 1;					
					ENCOUNTER_BAD_FILE;
					return -1;
				}			
				/* Output the BMP data */
				if (IPP_STATUS_NOERR != WriteDIBFile(&dibSpec, dstFile)) {
					printf("Destination file write error.\n");					
					bErStream = 1;
					ENCOUNTER_BAD_STREAM;	
					return -1;
				}
				fclose(dstFile);	
				dstFile = NULL;
				break;

			case IPP_STATUS_BADARG_ERR:
			case IPP_STATUS_BITSTREAM_ERR:
			case IPP_STATUS_NOTSUPPORTED_ERR:
			case IPP_STATUS_ERR:
			default:				
				bErStream = 1;
				ENCOUNTER_BAD_STREAM;	
				return rtCode;
			}
		}

		if (bErStream) {
			EXIT_MEMFREE;
			ENCOUNTER_BAD_STREAM;
		}
		printf("%s contain %d images,total cost %ld ms, average time:%ld ms\n", pSrcFileName,
			GIFInfo.nCurFrame, TotTime, TotTime/GIFInfo.nCurFrame);
		fprintf(decTimeLog,"%s size:%d X %d,%d byte  contain %d images,total cost %ld ms, average time:%f ms\n", pSrcFileName,
			DstPicture.picWidth, DstPicture.picHeight,dwFileSz,
			GIFInfo.nCurFrame, TotTime, TotTime*1.0/GIFInfo.nCurFrame);
        DecoderFree_GIF(&pDecoderState);

		/*record info after decoding Log:*/
		fprintf(logFile,"nDisposalMethod:%d \n",GIFInfo.nDisposalMethod); 
		fprintf(logFile,"UserInput:%d \n",GIFInfo.bUserInput);
		fprintf(logFile,"Transparency Color Flag:%d \n",GIFInfo.bTrnsClrFlg);
		fprintf(logFile,"Transparency index:%d \n",GIFInfo.nTrnsClrIdx);
		fprintf(logFile,"Picture stamp index:%d \n",GIFInfo.nPicTimeStamp);			


		/* Close file handler */
		fclose(srcFile);
		srcFile = NULL;
		fclose(logFile);
		logFile = NULL;
	}
	EXIT_MEMFREE_ALL;
	return 0;
}
/* EOF */
