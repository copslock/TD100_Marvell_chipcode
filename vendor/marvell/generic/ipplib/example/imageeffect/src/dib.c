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



#include "ippIE.h"
#include "dib.h"


/******************************************************************************
// Name:				DestroyDIB
// Description:			Reset MDIBSPEC structure and free data buffer
// Input Arguments:
//     pDIBSpec  - Pointer to MDIBSPEC structure
// Output Arguments:
//     None
// Returns:
//     None
******************************************************************************/
void DestroyDIB(MDIBSPEC *pDIBSpec)
{
    if (pDIBSpec->pBitmapData) {
        IPP_MemFree((void **) &(pDIBSpec->pBitmapData) );
        pDIBSpec->pBitmapData = NULL;
    }
    pDIBSpec->nBitsPerpixel=0;
    pDIBSpec->nClrMode=0;
    pDIBSpec->nDataSize=0;
    pDIBSpec->nHeight=0;
    pDIBSpec->nNumComponent=0;
    pDIBSpec->nPrecision=0;
    pDIBSpec->nStep=0;
    pDIBSpec->nWidth=0;
}

/******************************************************************************
// Name:				ToJPEGClrMode
// Description:			Process special dib color format, such as bit count is 4, 8, 32, compression is RLE.
// Input Arguments:
//     pDIBSpec      - Pointer to MDIBSPEC structure
//     file          - Pointer to the input image file
//     biCompression - Compression mode, RLE4 , RLE8
//     iRgbQuadSize  - Size of Rgb quad table
//     cClrEntrySize - Size of Rgb quad structure, 4 bytes or 3 bytes
// Output Arguments:
//     None
// Returns:
//     [Success] - ippStsNoErr
//     [Fail]    - ippStsMemAllocErr
//               - ippStsErr
******************************************************************************/
static IppStatus ToJPEGClrMode(MDIBSPEC *pDIBSpec, IPP_FILE *file, 
                             Ipp32u biCompression, Ipp32u iRgbQuadSize, Ipp8u cClrEntrySize)
{
    Ipp8u *pRgbQuad = NULL;
    Ipp8u *pTransformedData = NULL; 
    Ipp8u *pBitmapData = NULL;
    int x, y, j;
    Ipp8u cByte, cFirstByte, cSecByte, clrIndex = 0;
    IppStatus rtCode = ippStsNoErr;
    Ipp32u iPaddedWidth = IIP_WIDTHBYTES_4B(pDIBSpec->nWidth*3*8);

    
    IPP_MemCalloc((void **)&pRgbQuad, 4*256, 1);

    if(NULL==pRgbQuad){
        rtCode = ippStsMemAllocErr;
        goto ToJPEGClrMode_FuncRt;
    }

    pBitmapData = NULL;        

    switch(pDIBSpec->nBitsPerpixel){
        case 1:
            if(BI_RGB==biCompression){  
                Ipp32u iOddBitUnit = pDIBSpec->nWidth - ((pDIBSpec->nWidth>>3)<<3);

                /* Allocate memory for bitmap data */
                IPP_MemCalloc((void **)&pBitmapData, pDIBSpec->nDataSize, 4);
                if (NULL == pBitmapData) {
                    rtCode =  ippStsMemAllocErr;
                    goto ToJPEGClrMode_FuncRt;
                }
                
                IPP_MemCalloc((void **)&pTransformedData, pDIBSpec->nHeight *iPaddedWidth , 4);
                if (NULL == pTransformedData) {
                    rtCode = ippStsMemAllocErr;
                    goto ToJPEGClrMode_FuncRt;
                }

                if(1024<iRgbQuadSize){
                    IPP_Fread(pRgbQuad, 1, 1024, file);
                    IPP_Fseek(file, iRgbQuadSize-1024, IPP_SEEK_CUR); 
                }else{                         
                    if( iRgbQuadSize!= IPP_Fread(pRgbQuad, 1, iRgbQuadSize, file) ){
                        rtCode = ippStsErr;
                        goto ToJPEGClrMode_FuncRt;
                    }           
                }

                if( pDIBSpec->nDataSize != IPP_Fread(pBitmapData, 1, pDIBSpec->nDataSize, file) ){
                    rtCode = ippStsErr;
                    goto ToJPEGClrMode_FuncRt;
                }
                pDIBSpec->pBitmapData = pTransformedData;

                for(y=0; y<pDIBSpec->nHeight; y++){
                    for(x=0; x<(pDIBSpec->nWidth>>3); x++){                        
                        cByte = *(pBitmapData++);                        
                        for(j=7; j>=0; j--){
                            clrIndex = (Ipp8u)((cByte >>j)&0x1);
                            *(pTransformedData++) = pRgbQuad[ clrIndex*cClrEntrySize ];     /*ctBlue*/
                            *(pTransformedData++) = pRgbQuad[ clrIndex*cClrEntrySize+1 ]; /*ctGreen*/
                            *(pTransformedData++) = pRgbQuad[ clrIndex*cClrEntrySize+2 ]; /*ctRed*/
                        }
                    }

                    if(0!=iOddBitUnit){
                        cByte = *pBitmapData;  
                        for(j=0; (Ipp32u)j<iOddBitUnit; j++  ){
                            clrIndex =(Ipp8u) ( ( cByte >> (7-j) )&0x1 );
                            *(pTransformedData++) = pRgbQuad[ clrIndex*cClrEntrySize ];     /*ctBlue*/
                            *(pTransformedData++) = pRgbQuad[ clrIndex*cClrEntrySize+1 ]; /*ctGreen*/
                            *(pTransformedData++) = pRgbQuad[ clrIndex*cClrEntrySize+2 ]; /*ctRed*/
                        }
                    }

                    /* Padded bytes */
                    pBitmapData += ( pDIBSpec->nStep - (pDIBSpec->nWidth>>3) );

                    for(j=0; (Ipp32u)j<( iPaddedWidth  - (Ipp32u)(pDIBSpec->nWidth*3) ); j++){
                        *(pTransformedData++) = 0;
                    }
                }
                pBitmapData -= pDIBSpec->nDataSize;
                pDIBSpec->nDataSize = pDIBSpec->nHeight * iPaddedWidth;
                pDIBSpec->nStep = iPaddedWidth;
                
            }
            break;
        case 4:                           
            if(BI_RGB==biCompression){ 
                Ipp32u iOddBitUnit = pDIBSpec->nWidth - ((pDIBSpec->nWidth>>1)<<1);
                
                /* Allocate memory for bitmap data */
                IPP_MemCalloc((void **)&pBitmapData, pDIBSpec->nDataSize, 4);
                if (NULL == pBitmapData) {
                    rtCode =  ippStsMemAllocErr;
                    goto ToJPEGClrMode_FuncRt;
                }
                IPP_MemCalloc((void **)&pTransformedData, pDIBSpec->nHeight *iPaddedWidth , 4);
                if (NULL == pTransformedData) {
                    rtCode = ippStsMemAllocErr;
                    goto ToJPEGClrMode_FuncRt;
                }
                if(1024<iRgbQuadSize){
                    IPP_Fread(pRgbQuad, 1, 1024, file);
                    IPP_Fseek(file, iRgbQuadSize-1024, IPP_SEEK_CUR); 
                }else{                         
                    if( iRgbQuadSize!= IPP_Fread(pRgbQuad, 1, iRgbQuadSize, file) ){
                        rtCode = ippStsErr;
                        goto ToJPEGClrMode_FuncRt;
                    }           
                }
                if( pDIBSpec->nDataSize != IPP_Fread(pBitmapData, 1, pDIBSpec->nDataSize, file) ){
                    rtCode = ippStsErr;
                    goto ToJPEGClrMode_FuncRt;
                }
                pDIBSpec->pBitmapData = pTransformedData;

                for(y=0; y<pDIBSpec->nHeight; y++){
                    for(x=0; x<(pDIBSpec->nWidth>>1); x++){
                        cByte = *(pBitmapData++);
                        clrIndex = cByte >>4;
                        *(pTransformedData++) = pRgbQuad[ clrIndex*cClrEntrySize ];     /*ctBlue;*/
                        *(pTransformedData++) = pRgbQuad[ clrIndex*cClrEntrySize+1 ]; /*ctGreen;*/
                        *(pTransformedData++) = pRgbQuad[ clrIndex*cClrEntrySize+2 ]; /*ctRed;*/
                        clrIndex = cByte & 0xF;
                        *(pTransformedData++) = pRgbQuad[ clrIndex*cClrEntrySize ];     /*ctBlue;*/
                        *(pTransformedData++) = pRgbQuad[ clrIndex*cClrEntrySize+1 ]; /*ctGreen;*/
                        *(pTransformedData++) = pRgbQuad[ clrIndex*cClrEntrySize+2 ]; /*ctRed;*/                        
                    }

                    if(0!=iOddBitUnit){
                        clrIndex = (*pBitmapData) >>4;
                        *(pTransformedData++) = pRgbQuad[ clrIndex*cClrEntrySize ];     /*ctBlue*/
                        *(pTransformedData++) = pRgbQuad[ clrIndex*cClrEntrySize+1 ]; /*ctGreen*/
                        *(pTransformedData++) = pRgbQuad[ clrIndex*cClrEntrySize+2 ]; /*ctRed*/
                    }

                    /* Padded bytes */
                    pBitmapData += ( pDIBSpec->nStep - (pDIBSpec->nWidth>>1) );
                    for(j=0; (Ipp32u)j<( iPaddedWidth  - (Ipp32u)(pDIBSpec->nWidth*3) ); j++){
                        *(pTransformedData++) = 0;
                    }
                }

                pBitmapData -= pDIBSpec->nDataSize;
                pDIBSpec->nDataSize = pDIBSpec->nHeight * iPaddedWidth;
                pDIBSpec->nStep = iPaddedWidth;

            }else if(BI_RLE4==biCompression){
                int iDataLen = pDIBSpec->nDataSize;
                Ipp32u iLineNum = 0;
                Ipp8u * pCur = NULL;                

                /* Allocate memory for bitmap data */
                IPP_MemCalloc((void **)&pBitmapData, pDIBSpec->nDataSize, 4);
                if (NULL == pBitmapData) {
                    rtCode =  ippStsMemAllocErr;
                    goto ToJPEGClrMode_FuncRt;
                }
                iPaddedWidth = IIP_WIDTHBYTES_4B((pDIBSpec->nWidth + (pDIBSpec->nWidth&0x1)) * 3*8);                
                IPP_MemCalloc((void **)&pTransformedData, pDIBSpec->nHeight *iPaddedWidth , 4);
                if (NULL == pTransformedData) {
                    rtCode = ippStsMemAllocErr;
                    goto ToJPEGClrMode_FuncRt;
                }
                if(1024<iRgbQuadSize){
                    IPP_Fread(pRgbQuad, 1, 1024, file);
                    IPP_Fseek(file, iRgbQuadSize-1024, IPP_SEEK_CUR); 
                }else{                         
                    if( iRgbQuadSize!= IPP_Fread(pRgbQuad, 1, iRgbQuadSize, file) ){
                        rtCode = ippStsErr;
                        goto ToJPEGClrMode_FuncRt;
                    }           
                }
                if( pDIBSpec->nDataSize != IPP_Fread(pBitmapData, 1, pDIBSpec->nDataSize, file) ){
                    rtCode = ippStsErr;
                    goto ToJPEGClrMode_FuncRt;
                }
                pDIBSpec->pBitmapData = pTransformedData;

                pCur = pTransformedData;

                while(iDataLen>1){   
                    /* First byte in the mode */
                    cFirstByte = *pBitmapData;
                    if(0==cFirstByte){
                        cSecByte = *(pBitmapData+1);
                        if( cSecByte < 0x03){
                            /* Encode Mode */
                            switch(cSecByte){
                            case 0x00:
                                /* End of line */
                                pBitmapData += 2;
                                iLineNum++;
                                pCur =  pTransformedData +(iLineNum* iPaddedWidth);
                                break;
                            case 0x01:
                                /* End of bitmap */
                                pBitmapData += 2;
                                pBitmapData -= pDIBSpec->nDataSize;
                                pDIBSpec->nDataSize = pDIBSpec->nHeight * iPaddedWidth;
                                pDIBSpec->nStep = iPaddedWidth;
                                rtCode = ippStsNoErr;
                                goto ToJPEGClrMode_FuncRt;
                            case 0x02:
                                if(iDataLen<4){
                                    rtCode = ippStsErr;
                                    goto ToJPEGClrMode_FuncRt;
                                }
                                pBitmapData += 2;
                                iDataLen -= 2;
                                pCur += ( (*(pBitmapData++)) * 3 ); /* Horizontal offset */
                                pCur += ( (*(pBitmapData++)) * iPaddedWidth); /* Vertical offset */
                                break;
                            }
                        }else{
                            /* Absolute Mode */
                            Ipp8u nDataBytes = (cSecByte+1)/2;
                            Ipp8u nPaddedBytes = ( (0==(nDataBytes&0x1)) ? 0 : (nDataBytes&0x1) );
                            if( (iDataLen-2)< (nDataBytes+nPaddedBytes) ){
                                rtCode = ippStsErr;
                                goto ToJPEGClrMode_FuncRt;
                            }
                            pBitmapData += 2;

                            for(j=0; j< cSecByte; j++){                               
                                if( 0==(j&0x1) ){
                                    clrIndex = *(pBitmapData++);
                                    *(pCur++) = pRgbQuad[ (clrIndex >>4)*cClrEntrySize     ]; /*ctBlue*/
                                    *(pCur++) = pRgbQuad[ (clrIndex >>4)*cClrEntrySize+1 ]; /*ctGreen*/
                                    *(pCur++) = pRgbQuad[ (clrIndex >>4)*cClrEntrySize+2 ]; /*ctRed*/
                                }else{
                                    *(pCur++) = pRgbQuad[ (clrIndex & 0x0F)*cClrEntrySize     ]; /*ctBlue*/
                                    *(pCur++) = pRgbQuad[ (clrIndex & 0x0F)*cClrEntrySize+1 ]; /*ctGreen*/
                                    *(pCur++) = pRgbQuad[ (clrIndex & 0x0F)*cClrEntrySize+2 ]; /*ctRed*/
                                }
                            }
                            pBitmapData += nPaddedBytes;
                            iDataLen -= (nDataBytes+nPaddedBytes);
                        }
                    }else{
                        /* Encode Mode, First Byte is non-zero */
                        clrIndex = *(pBitmapData+1);
                        for(j=0; j< cFirstByte; j++){
                            if( 0==(j&0x1) ){
                                *(pCur++) = pRgbQuad[ ((clrIndex & 0xF0)>>4)*cClrEntrySize     ]; /*ctBlue*/
                                *(pCur++) = pRgbQuad[ ((clrIndex & 0xF0)>>4)*cClrEntrySize+1 ]; /*ctGreen*/
                                *(pCur++) = pRgbQuad[ ((clrIndex & 0xF0)>>4)*cClrEntrySize+2 ]; /*ctRed*/
                            }else{
                                *(pCur++) = pRgbQuad[ (clrIndex & 0x0F)*cClrEntrySize     ]; /*ctBlue*/
                                *(pCur++) = pRgbQuad[ (clrIndex & 0x0F)*cClrEntrySize+1 ]; /*ctGreen*/
                                *(pCur++) = pRgbQuad[ (clrIndex & 0x0F)*cClrEntrySize+2 ]; /*ctRed*/
                            }
                        }      
                        pBitmapData += 2;
                    }

                    iDataLen -= 2;
                } /* end of while(iDataLen)*/

                pBitmapData -= pDIBSpec->nDataSize;
                pDIBSpec->nDataSize = pDIBSpec->nHeight * iPaddedWidth;
                pDIBSpec->nStep = iPaddedWidth;
            }

            break;
        case 8:
            if(BI_RLE8==biCompression){
                int iDataLen = pDIBSpec->nDataSize;
                Ipp32u iLineNum = 0;
                Ipp8u * pCur = NULL;                 

                /* BGR888 */
                /* Allocate memory for bitmap data */
                IPP_MemCalloc((void **)&pBitmapData, pDIBSpec->nDataSize, 4);
                if (NULL == pBitmapData) {
                    rtCode =  ippStsMemAllocErr;
                    goto ToJPEGClrMode_FuncRt;
                }
                IPP_MemCalloc((void **)&pTransformedData, pDIBSpec->nHeight *iPaddedWidth , 4);
                if (NULL == pTransformedData) {
                    rtCode = ippStsMemAllocErr;
                    goto ToJPEGClrMode_FuncRt;
                }
                pCur = pTransformedData;
                if(1024<iRgbQuadSize){
                    IPP_Fread(pRgbQuad, 1, 1024, file);
                    IPP_Fseek(file, iRgbQuadSize-1024, IPP_SEEK_CUR); 
                }else{                         
                    if( iRgbQuadSize!= IPP_Fread(pRgbQuad, 1, iRgbQuadSize, file) ){
                        rtCode = ippStsErr;
                        goto ToJPEGClrMode_FuncRt;
                    }           
                }
                if( iDataLen != IPP_Fread(pBitmapData, 1, iDataLen, file) ){
                    rtCode = ippStsErr;
                    goto ToJPEGClrMode_FuncRt;
                }
                pDIBSpec->pBitmapData = pTransformedData;                

                while(iDataLen>1){   
                    /* First byte in the mode */
                    cFirstByte = *pBitmapData;
                    if(0==cFirstByte){
                        cSecByte = *(pBitmapData+1);                    
                        if( cSecByte < 0x03){                        
                            /* Encode Mode */
                            switch(cSecByte){
                            case 0x00:
                                /* End of line */
                                pBitmapData += 2;
                                iLineNum++;
                                pCur =  pTransformedData + (iLineNum* iPaddedWidth);
                                break;
                            case 0x01:
                                /* End of bitmap */
                                pBitmapData += 2;
                                pBitmapData -= pDIBSpec->nDataSize;
                                pDIBSpec->nDataSize = pDIBSpec->nHeight * iPaddedWidth;
                                pDIBSpec->nStep = iPaddedWidth;
                                rtCode =  ippStsNoErr;
                                goto ToJPEGClrMode_FuncRt;
                            case 0x02:                                
                                if(iDataLen<4){
                                    rtCode = ippStsErr;
                                    goto ToJPEGClrMode_FuncRt;
                                }
                                pBitmapData += 2;
                                iDataLen -= 2;
                                pCur += ( (*(pBitmapData++)) * 3 ); /* Horizontal offset */
                                pCur += ( (*(pBitmapData++)) * iPaddedWidth); /* Vertical offset */
                                break;
                            }
                        }else{
                            /* Absolute Mode */
                            Ipp8u nPaddedBytes = ( (cSecByte&0x1)==0 ? 0 : (cSecByte&0x1 ) );
                            if( (iDataLen-2) < (cSecByte+nPaddedBytes)  ){
                                rtCode = ippStsErr;
                                goto ToJPEGClrMode_FuncRt;
                            }
                            pBitmapData += 2;

                            for(j=0; j< cSecByte; j++){
                                clrIndex = *(pBitmapData++);
                                *(pCur++) = pRgbQuad[ clrIndex*cClrEntrySize     ]; /*ctBlue*/
                                *(pCur++) = pRgbQuad[ clrIndex*cClrEntrySize+1 ]; /*ctGreen*/
                                *(pCur++) = pRgbQuad[ clrIndex*cClrEntrySize+2 ]; /*ctRed*/
                            }
                            iDataLen -= (cSecByte + nPaddedBytes);
                            pBitmapData += nPaddedBytes;
                        }
                    }else{
                        /* Encode Mode, First Byte is non-zero */
                        clrIndex = *(pBitmapData+1);
                        for(j=0; j< cFirstByte; j++){
                            *(pCur++) = pRgbQuad[ clrIndex*cClrEntrySize     ]; /*ctBlue*/
                            *(pCur++) = pRgbQuad[ clrIndex*cClrEntrySize+1 ]; /*ctGreen*/
                            *(pCur++) = pRgbQuad[ clrIndex*cClrEntrySize+2 ]; /*ctRed*/
                        }   
                        pBitmapData += 2;
                    }

                    iDataLen -= 2;
                } /* end of while(iDataLen)*/

                pBitmapData -= pDIBSpec->nDataSize;
                pDIBSpec->nDataSize = pDIBSpec->nHeight * iPaddedWidth;
                pDIBSpec->nStep = iPaddedWidth;
            }else{
                int iDataLen = pDIBSpec->nDataSize;
                Ipp8u * pCur = NULL; 

                if(1024<iRgbQuadSize){
                    IPP_Fread(pRgbQuad, 1, 1024, file);
                    IPP_Fseek(file, iRgbQuadSize-1024, IPP_SEEK_CUR); 
                }else{                         
                    if( iRgbQuadSize!= IPP_Fread(pRgbQuad, 1, iRgbQuadSize, file) ){
                        rtCode = ippStsErr;
                        goto ToJPEGClrMode_FuncRt;
                    }           
                }

                for(j=0; (Ipp32u)j<(iRgbQuadSize/cClrEntrySize); j++){
                    if( (pRgbQuad[0+j*cClrEntrySize]!=j)
                        || (pRgbQuad[1+j*cClrEntrySize]!=j)
                        || (pRgbQuad[2+j*cClrEntrySize]!=j) ){
                            break;
                    }
                }                

                if( j==(iRgbQuadSize/cClrEntrySize) )
                {
                    pDIBSpec->nNumComponent=1;
                    pDIBSpec->nPrecision=8;
                    pDIBSpec->nClrMode=FORMAT_GRAY8;
                }else{
                    pDIBSpec->nNumComponent=3;
                    pDIBSpec->nPrecision=8;
                    pDIBSpec->nClrMode=FORMAT_BGR888;

                    /* Allocate memory for bitmap data */
                    IPP_MemCalloc((void **)&pBitmapData, pDIBSpec->nDataSize, 4);
                    if (NULL == pBitmapData) {
                        rtCode =  ippStsMemAllocErr;
                        goto ToJPEGClrMode_FuncRt;
                    }
                    IPP_MemCalloc((void **)&pTransformedData, pDIBSpec->nHeight *iPaddedWidth , 4);
                    if (NULL == pTransformedData) {
                        rtCode = ippStsMemAllocErr;
                        goto ToJPEGClrMode_FuncRt;
                    }
                    pCur = pTransformedData;

                    if( iDataLen != IPP_Fread(pBitmapData, 1, iDataLen, file) ){
                        rtCode = ippStsErr;
                        goto ToJPEGClrMode_FuncRt;
                    }
                    pDIBSpec->pBitmapData = pTransformedData;   

                    for(y=0; y<pDIBSpec->nHeight; y++){
                        for(x=0; x<pDIBSpec->nWidth; x++){
                            clrIndex = *(pBitmapData++);
                            *(pCur++) = pRgbQuad[ clrIndex*cClrEntrySize     ]; /*ctBlue*/
                            *(pCur++) = pRgbQuad[ clrIndex*cClrEntrySize+1 ]; /*ctGreen*/
                            *(pCur++) = pRgbQuad[ clrIndex*cClrEntrySize+2 ]; /*ctRed*/
                        }                    

                        /* Padded bytes */
                        for(j=0; (Ipp32u)j<( iPaddedWidth  - (Ipp32u)(pDIBSpec->nWidth*3) ); j++){
                            *(pCur++) = 0;
                        }
                        pBitmapData += (pDIBSpec->nStep - pDIBSpec->nWidth);

                    }

                    pBitmapData -= pDIBSpec->nDataSize;
                    pDIBSpec->nDataSize = pDIBSpec->nHeight * iPaddedWidth;
                    pDIBSpec->nStep = iPaddedWidth;  
                }
            }
            break;
#if 0
        case 32:
            {
                int iDataLen = pDIBSpec->nDataSize;
                Ipp8u * pCur = NULL; 
                
                /* Allocate memory for bitmap data */
                IPP_MemCalloc((void **)&pBitmapData, pDIBSpec->nDataSize, 4);
                if (NULL == pBitmapData) {
                    rtCode =  ippStsMemAllocErr;
                    goto ToJPEGClrMode_FuncRt;
                }
                IPP_MemCalloc((void **)&pTransformedData, pDIBSpec->nHeight *iPaddedWidth , 4);
                if (NULL == pTransformedData) {
                    rtCode = ippStsMemAllocErr;
                    goto ToJPEGClrMode_FuncRt;
                }
                //memset(pTransformedData, 0, pDIBSpec->nHeight*iPaddedWidth);
                pCur = pTransformedData;
                if( iDataLen != IPP_Fread(pBitmapData, 1, iDataLen, file) ){
                    rtCode = ippStsErr;
                    goto ToJPEGClrMode_FuncRt;
                }
                pDIBSpec->pBitmapData = pTransformedData;   

                for(y=0; y<pDIBSpec->nHeight; y++){
                    for(x=0; x<pDIBSpec->nWidth; x++){
                        *(pCur++) = *(pBitmapData++);
                        *(pCur++) = *(pBitmapData++);
                        *(pCur++) = *(pBitmapData++);  
                        pBitmapData++;
                    }                    

                    /* Padded bytes */
                    for(j=0; (Ipp32u)j<( iPaddedWidth  - (Ipp32u)(pDIBSpec->nWidth*3) ); j++){
                        *(pCur++) = 0;
                    }
                }

                pBitmapData -= pDIBSpec->nDataSize;
                pDIBSpec->nDataSize = pDIBSpec->nHeight * iPaddedWidth;
                pDIBSpec->nStep = iPaddedWidth;
            }
            break;
#endif
        default:
            break;
    }


ToJPEGClrMode_FuncRt:
    if(NULL!=pRgbQuad){
        IPP_MemFree((void **)&pRgbQuad);
        pRgbQuad = NULL;
    }

    if(NULL!=pBitmapData){
        IPP_MemFree((void **)&pBitmapData);
        pBitmapData = NULL;
    }
    return rtCode;
}

/******************************************************************************
// Name:				ReadDIBFile
// Description:			Parse header info of the input image file
// Input Arguments:
//     pDIBSpec  - Pointer to MDIBSPEC structure
//     file      - Pointer to the input image file
// Output Arguments:
//     pDIBSpec  - Pointer to MDIBSPEC structure
// Returns:
//     [Success] - ippStsNoErr
//     [Fail]    - ippStsMemAllocErr
//               - ippStsNotSupportedModeErr
//               - ippStsErr
******************************************************************************/
IppStatus ReadDIBFile(MDIBSPEC *pDIBSpec, IPP_FILE *file)
{
    Ipp8u   anData8[16];
    Ipp16u  data16 = 0;
    Ipp32u  data32 = 0;
    Ipp32s  biHeight = 0;
    Ipp32u  bfSize = 0;    
    Ipp32u  bfOffBits = 0;
    Ipp32s  biSize = 0;
    Ipp32u  biSizeImage = 0;
    Ipp32u  biCompression = BI_RGB;
    Ipp32u  anMask[3];
    Ipp32u  biClrUsed = 0;
    Ipp32u iRgbQuadSize = 0;
    Ipp8u   cClrEntrySize = 4;
    IppStatus rtCode = ippStsNoErr;

    /* Read DIB Header information */
    
    /* bfType - 2;             */
    if ( (IPP_Fread(anData8, 1, 2, file)!=2)||(anData8[0]!='B')||(anData8[1]!='M') ) {
        return ippStsErr;
    }

    /* bfSize - 4;             */
    if ( (IPP_Fread(&bfSize, 4, 1, file)!=1) ) {
        return ippStsErr;
    }

    /* bfReserved1 - 2;        */
    /* bfReserved2 - 2;        */
    IPP_Fseek(file, 4, IPP_SEEK_CUR);

    /* bfOffBits - 4;          */
    if ( (IPP_Fread(&bfOffBits, 4, 1, file)!=1) ) {
        return ippStsErr;
    }    

    /* biSize - 4 */
    if ( (IPP_Fread(&biSize, 4, 1, file)!=1)||(0==biSize) ) {
        return ippStsErr;
    }

    if(biSize>=40){
        /* BITMAPINFOHEADER */

        /* biWidth - 4 */
        if ( (IPP_Fread(&(pDIBSpec->nWidth), 4, 1, file)!=1)||(0==pDIBSpec->nWidth) ) {
            return ippStsErr;
        }

        /* biHeight - 4 */
        if ( (IPP_Fread(&biHeight, 4, 1, file)!=1)||(0==biHeight) ) {
            return ippStsErr;
        }

        pDIBSpec->nHeight=biHeight;

        /* Add for BITMAPV4HEADER or BITMAPV5HEADER
        // Height is negative: indicating a top-down DIB
        // Height is positive:  indicating a bottom-up DIB*/
        if(biHeight<0){
            pDIBSpec->nHeight = -biHeight;
        }
        /* End add, @20081117, Yajun Zeng*/


        /* biPlanes - 2 */
        if ( (IPP_Fread(&data16, 2, 1, file)!=1) || (1!=data16) ) {
            return ippStsErr;
        }

        /* biBitCount - 2 */
        if ( (IPP_Fread(&(pDIBSpec->nBitsPerpixel), 2, 1, file)!=1) ) {
            return ippStsErr;
        }

        /* biCompression - 4 */
        if ( (IPP_Fread(&biCompression, 4, 1, file)!=1) ) {
            return ippStsErr;
        }

        if ( (BI_JPEG==biCompression) || (BI_PNG==biCompression) ) {
            return ippStsNotSupportedModeErr;
        }

        /* biSizeImage - 4 */
        if (IPP_Fread(&biSizeImage, 4, 1, file)!=1) {
            return ippStsErr;
        }

        /* biXPelsPerMeter - 4 */
        if ( IPP_Fread(&data32, 4, 1, file)!=1 ) {
            return ippStsErr;
        }

        /* biYPelsPerMeter - 4 */
        if ( IPP_Fread(&data32, 4, 1, file)!=1 ) {
            return ippStsErr;
        }

        /* biClrUsed - 4 */
        if ( IPP_Fread(&biClrUsed, 4, 1, file)!=1 ) {
            return ippStsErr;
        }

        /* biClrImportant - 4 */
        if ( IPP_Fread(&data32, 4, 1, file)!=1 ) {
            return ippStsErr;
        }
    }else{
        /* BITMAPCOREHEADER */

            /* biWidth - 2 */
            if ( (IPP_Fread(&(pDIBSpec->nWidth), 2, 1, file)!=1)||(0==pDIBSpec->nWidth) ) {
                return ippStsErr;
            }

            /* biHeight - 2 */
            if ( (IPP_Fread(&(pDIBSpec->nHeight), 2, 1, file)!=1)||(0==pDIBSpec->nHeight) ) {
                return ippStsErr;
            }

            /* biPlanes - 2 */
            if ( (IPP_Fread(&data16, 2, 1, file)!=1) || (1!=data16) ) {
                return ippStsErr;
            }

            /* biBitCount - 2 */
            if ( (IPP_Fread(&(pDIBSpec->nBitsPerpixel), 2, 1, file)!=1) ) {
                return ippStsErr;
            }
    }

    pDIBSpec->nStep     = IIP_WIDTHBYTES_4B((pDIBSpec->nWidth)*(pDIBSpec->nBitsPerpixel));
    pDIBSpec->nDataSize = pDIBSpec->nStep*pDIBSpec->nHeight;

    /* Allocate memory for bitmap data */
    pDIBSpec->pBitmapData = NULL;
       
    /* Get Color Mode */
    switch (pDIBSpec->nBitsPerpixel) {
        case 8:
            IPP_Printf("src image format: GRAY8\n");
            if(BI_RLE8==biCompression){
                IPP_Fseek(file, (14+biSize), IPP_SEEK_SET);                
                pDIBSpec->nDataSize = biSizeImage;    
                iRgbQuadSize = bfOffBits-14-biSize;
                /* Count cClrEntrySize, it has only two values - 4 or 3 */
                if(0!=iRgbQuadSize){              
                    if(12==biSize){
                        cClrEntrySize = 3;
                    }else if( (iRgbQuadSize==(biClrUsed*3)) || (iRgbQuadSize==((1<<pDIBSpec->nBitsPerpixel) *3)) ){
                        cClrEntrySize = 3;
                    }               
                }
                if(ippStsNoErr
                    !=  (rtCode = ToJPEGClrMode(pDIBSpec, file, biCompression, bfOffBits-14-biSize, cClrEntrySize)) ){
                        return rtCode;
                }
                pDIBSpec->nNumComponent=3;
                pDIBSpec->nPrecision=8;
                pDIBSpec->nClrMode=FORMAT_BGR888;
            }else{
                iRgbQuadSize = bfOffBits-14-biSize;
                if(0!=iRgbQuadSize){                    
                    IPP_Fseek(file, (14+biSize), IPP_SEEK_SET);   
                    /* Count cClrEntrySize, it has only two values - 4 or 3 */
                    if(12==biSize){
                        cClrEntrySize = 3;
                    }else if( (iRgbQuadSize==(biClrUsed*3)) || (iRgbQuadSize==((1<<pDIBSpec->nBitsPerpixel) *3)) ){
                        cClrEntrySize = 3;
                    } 

                    if(0!=biClrUsed){
                        if( iRgbQuadSize<(biClrUsed*cClrEntrySize) ){
                            return ippStsErr;
                        }
                    }else if( iRgbQuadSize<((Ipp32u)(1<<((Ipp32u)pDIBSpec->nBitsPerpixel) )*cClrEntrySize) ){
                        return ippStsErr;
                    }

                    if(ippStsNoErr
                        !=  (rtCode = ToJPEGClrMode(pDIBSpec, file, biCompression, iRgbQuadSize, cClrEntrySize)) ){
                            return rtCode;
                    }
                }
                else{                  
                    pDIBSpec->nNumComponent=1;
                    pDIBSpec->nPrecision=8;
                    pDIBSpec->nClrMode=FORMAT_GRAY8;
                }
            }
            break;
        case 24:
            IPP_Printf("src image format: BGR888\n");
            pDIBSpec->nNumComponent=3;
            pDIBSpec->nPrecision=8;
            pDIBSpec->nClrMode=FORMAT_BGR888;
            break;
        case 16:
            pDIBSpec->nNumComponent=3;
            pDIBSpec->nPrecision=8;
            if ( BI_BITFIELDS==biCompression ) {
                if ( IPP_Fread(&anMask, 4, 3, file)!=3 ) {
                    return ippStsErr;
                }
                if ( (0x001F==anMask[2])&&(0x03E0==anMask[1])&&(0x7C00==anMask[0]) ) {
                    pDIBSpec->nClrMode=FORMAT_BGR555;
                    IPP_Printf("src image format: BGR555\n");
                } else if ( (0x001F==anMask[2])&&(0x07E0==anMask[1])&&(0xF800==anMask[0]) ) {
                    pDIBSpec->nClrMode=FORMAT_BGR565;
                    IPP_Printf("src image format: BGR565\n");
                } else {
                    return ippStsNotSupportedModeErr;
                }
            }else{
                 pDIBSpec->nClrMode=FORMAT_BGR555;
            }
            break;
        case 32:
            IPP_Printf("src image format: BGRA\n");
            pDIBSpec->nNumComponent=4;
            pDIBSpec->nPrecision=8;
            pDIBSpec->nClrMode=FORMAT_BGRA;
            IPP_Fseek(file, bfOffBits, IPP_SEEK_SET);
            break;
        default:
            IPP_Printf("src image format: OTHERS\n");
            pDIBSpec->nNumComponent=3;
            pDIBSpec->nPrecision=8;
            pDIBSpec->nClrMode=FORMAT_BGR888;
            //return ippStsNotSupportedModeErr;
            /* Skip the other INFOHEADER syntax of V4/V5HEADER */
            IPP_Fseek(file, (14+biSize), IPP_SEEK_SET);
            if(BI_RLE4==biCompression){
                pDIBSpec->nDataSize = biSizeImage;
            }

            iRgbQuadSize = bfOffBits-14-biSize;

            /* Count cClrEntrySize, it has only two values - 4 or 3 */
            if(0!=iRgbQuadSize){   
                if(12==biSize){
                    cClrEntrySize = 3;
                }else if( (iRgbQuadSize==(biClrUsed*3)) || (iRgbQuadSize==((1<<pDIBSpec->nBitsPerpixel) *3)) ){
                    cClrEntrySize = 3;
                }               
            }
            
            if(ippStsNoErr
                !=  (rtCode = ToJPEGClrMode(pDIBSpec, file, biCompression, iRgbQuadSize, cClrEntrySize)) ){
                    return rtCode;
            }
            break;
    }    

    IPP_Fseek(file, bfOffBits, IPP_SEEK_SET);

    if(biHeight<0){
        pDIBSpec->nStep = -pDIBSpec->nStep;
    }

    return ippStsNoErr;
}


/******************************************************************************
// Name:				WriteDIBFile
// Description:			Write DIB bit stream to output file
// Input Arguments:
//     pDIBSpec  - Pointer to MDIBSPEC structure
//     file      - Pointer to the output file
// Output Arguments:
//     None
// Returns:
//     [Success] - ippStsNoErr
//     [Fail]    - IPP_SATUS_ERR
******************************************************************************/
IppStatus WriteDIBFile(MDIBSPEC *pDIBSpec, IPP_FILE *file)
{
    Ipp8u   anData8[16];
    Ipp16u  data16;
    Ipp32u  data32;
    Ipp32u  bfSize;    
    Ipp32u  bfOffBits;
    Ipp32u  biCompression;
    Ipp32u  anMask[3];

    /* Read DIB Header information */
    
    /* bfType - 2;             */
    anData8[0] = 'B';
    anData8[1] = 'M';

    if ( (IPP_Fwrite(&anData8, 1, 2, file)!=2) ) {
        return ippStsErr;
    }

    /* bfSize - 4;             */
    bfSize = pDIBSpec->nDataSize + 54;
    if ( (IPP_Fwrite(&bfSize, 4, 1, file)!=1) ) {
        return ippStsErr;
    }

    /* bfReserved1 - 2;        */
    /* bfReserved2 - 2;        */
    IPP_Fseek(file, 4, IPP_SEEK_CUR);

    /* bfOffBits - 4;          */
    bfOffBits = 54;
    if ( (IPP_Fwrite(&bfOffBits, 4, 1, file)!=1) ) {
        return ippStsErr;
    }

    /* BITMAPINFOHEADER */

    /* biSize - 4 */
    data32 = 40;
    if ( (IPP_Fwrite(&data32, 4, 1, file)!=1) ) {
        return ippStsErr;
    }

    /* biWidth - 4 */
    data32=pDIBSpec->nWidth;
    if ( (IPP_Fwrite(&data32, 4, 1, file)!=1) ) {
        return ippStsErr;
    }

    /* biHeight - 4 */
    data32=pDIBSpec->nHeight;
    if ( (IPP_Fwrite(&data32, 4, 1, file)!=1) ) {
        return ippStsErr;
    }

    /* biPlanes - 2 */
    data16=1;
    if ( IPP_Fwrite(&data16, 2, 1, file)!=1 ) {
        return ippStsErr;
    }

    /* biBitCount - 2 */
    data16=(Ipp16u)pDIBSpec->nBitsPerpixel;
    if ( (IPP_Fwrite(&data16, 2, 1, file)!=1) ) {
        return ippStsErr;
    }

    /* biCompression - 4 */
    biCompression = (pDIBSpec->nClrMode==FORMAT_BGR555 || pDIBSpec->nClrMode==FORMAT_BGR565) ? BI_BITFIELDS : BI_RGB;
    if ( IPP_Fwrite(&biCompression, 4, 1, file)!=1 ) {
        return ippStsErr;
    }

    /* biSizeImage - 4 */
    data32 = 0;
    if ( (IPP_Fwrite(&data32, 4, 1, file)!=1) ) {
        return ippStsErr;
    }

    /* biXPelsPerMeter - 4 */
    data32 = 3780;
    if ( IPP_Fwrite(&data32, 4, 1, file)!=1 ) {
        return ippStsErr;
    }

    /* biYPelsPerMeter - 4 */
    data32 = 3780;
    if ( IPP_Fwrite(&data32, 4, 1, file)!=1 ) {
        return ippStsErr;
    }

    /* biClrUsed - 4 */
    data32 = 0;
    if ( IPP_Fwrite(&data32, 4, 1, file)!=1 ) {
        return ippStsErr;
    }

    /* biClrImportant - 4 */
    data32 = 0;
    if ( IPP_Fwrite(&data32, 4, 1, file)!=1 ) {
        return ippStsErr;
    }
    
    /* Get Color Mode */
    if (FORMAT_BGR555==pDIBSpec->nClrMode) {
        anMask[2]=0x001F;
        anMask[1]=0x03E0;
        anMask[0]=0x7C00;
        if ( IPP_Fwrite(&anMask, 4, 3, file)!=3 ) {
            return ippStsErr;
        }
    } else if (FORMAT_BGR565==pDIBSpec->nClrMode) {
        anMask[2]=0x001F;
        anMask[1]=0x07E0;
        anMask[0]=0xF800;
        if ( IPP_Fwrite(&anMask, 4, 3, file)!=3 ) {
            return ippStsErr;
        }
    } else if (FORMAT_GRAY8==pDIBSpec->nClrMode) {
        unsigned int i;
        for (i=0; i<=255; i++) {
            data32 = (i<<16)|(i<<8)|i;
            IPP_Fwrite(&data32, 4, 1, file);
        }
    }

    /* bfOffBits -4 */
    data32=IPP_Ftell(file);
    IPP_Fseek(file, 10, IPP_SEEK_SET);
    if ( IPP_Fwrite(&data32, 4, 1, file)!=1 ) {
        return ippStsErr;
    }

    IPP_Fseek(file, data32, IPP_SEEK_SET);

    data32 = IPP_Fwrite(pDIBSpec->pBitmapData, 1, pDIBSpec->nDataSize, file);

    if (  data32 != (Ipp32u)(pDIBSpec->nDataSize) ) {
        return ippStsErr;
    }

    /* bfOffBits -4 */
    data32=IPP_Ftell(file);
    IPP_Fseek(file, 2, IPP_SEEK_SET);
    if ( IPP_Fwrite(&data32, 4, 1, file)!=1 ) {
        return ippStsErr;
    }

    IPP_Fseek(file, 0, IPP_SEEK_END);
    return ippStsNoErr;
}

/* EOF */
