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

#include "vgusr.h"

/***************************************************************************
// Name:             videoInitBuffer
// Description:      Initialize the input bitstream buffer for decoder
//
// Input Arguments:
//      pBufInfo  Pointer to decoder input bitstream buffer
//
// Output Arguments:
//      pBufInfo  Pointer to updated decoder bitstream buffer
//
// Returns:
//     IPP_OK               No Error
       IPP_FAIL             malloc memory fail
*****************************************************************************/
int videoInitBuffer (IppBitstream* pBufInfo)
{
     IPP_MemMalloc((void **)&(pBufInfo->pBsBuffer), DATA_BUFFER_SIZE, 4);

    if (pBufInfo->pBsBuffer == NULL) {
        return IPP_FAIL;
    }

    if ( pBufInfo->pBsBuffer ) {
        IPP_Memset (pBufInfo->pBsBuffer, 0, DATA_BUFFER_SIZE);
    }

    /*
    //no read data at beginning
    //set current pointer to the end of buffer
    */

    pBufInfo->pBsCurByte = pBufInfo->pBsBuffer + DATA_BUFFER_SIZE;
    pBufInfo->bsCurBitOffset = 0;
    pBufInfo->bsByteLen = DATA_BUFFER_SIZE;

    return IPP_OK;
}

/***************************************************************************
// Name:             videoReloadBuffer
// Description:      Reload the input bitstream buffer for decoder
//
// Input Arguments:
//      pBufInfo  Pointer to decoder input bitstream buffer
//      stream    Pointer to the input file stream buffer
//
// Output Arguments:
//      pBufInfo  Pointer to updated decoder bitstream buffer
//
// Returns:
//      IPP_OK              No Error
//      IPP_FAIL           fread return 0
*****************************************************************************/
int videoReloadBuffer (IppBitstream *pBufInfo, IPP_FILE *srcFile)
{
    int    offset;
    int remain_data,nReadByte;
    
    offset = pBufInfo->pBsCurByte - pBufInfo->pBsBuffer;
    remain_data = pBufInfo->bsByteLen - offset;
    if ( remain_data != 0 ) {
        IPP_Memmove( pBufInfo->pBsBuffer, pBufInfo->pBsCurByte, remain_data );
    }
    nReadByte = (int)IPP_Fread(pBufInfo->pBsBuffer+remain_data, 1, DATA_BUFFER_SIZE - remain_data, (void*)srcFile);

    pBufInfo->bsByteLen = remain_data + nReadByte;

    pBufInfo->pBsCurByte = pBufInfo->pBsBuffer;
    
    /*read all data of the file,there is no more data can be readed from the file*/
    if ( nReadByte<(DATA_BUFFER_SIZE - remain_data) ){
        return 1;
    }
    
    return 0;
}

    
/***************************************************************************
// Name:             videoFreeBuffer
// Description:      Free input bitstream buffer for decoder
//
// Input Arguments:
//      pBufInfo  Pointer to decoder input bitstream buffer

// No Returns:
*****************************************************************************/
void videoFreeBuffer (IppBitstream *pBufInfo)
{
    if ( pBufInfo->pBsBuffer ) {
        IPP_MemFree ((void**)&pBufInfo->pBsBuffer);
        pBufInfo->pBsBuffer = NULL;
    }
        
 }
/***************************************************************************
// Name:             outfile_frame
// Description:      write input picture frame buffer to file
//
// Input Arguments:
//      pPicture  Pointer to decoder output frame 
//      outfile   Pointer to output file
// Returns:
//      IPP_OK    write file success
//      IPP_FAIL  write file fail
*****************************************************************************/
int outfile_frame ( IPP_FILE *outfile,IppPicture *pPicture) 
{
    void *pResultY, *pResultU, *pResultV;
    int width, height,size;
    int ret = IPP_OK;
    pResultY = pPicture->ppPicPlane[0];
    pResultU = pPicture->ppPicPlane[1];
    pResultV = pPicture->ppPicPlane[2];
    
    width = pPicture->picWidth;
    height = pPicture->picHeight;
    size =  width*height;

    if( IPP_Fwrite(pResultY,sizeof(Ipp8u),size, outfile) != size){
         ret = IPP_FAIL;
    }
    size >>= 2;
    if( IPP_Fwrite(pResultU,sizeof(Ipp8u), size, outfile) != size){
         ret = IPP_FAIL;
    }
    if( IPP_Fwrite(pResultV,sizeof(Ipp8u), size, outfile) != size){
         ret = IPP_FAIL;
    }
    
    return ret;
}
/***************************************************************************
// Name:             CheckStreamSpace
// Description:      check whether there is space available in the IppBitstream
//
// Input Arguments:
//      pVideoStream  pointer to IppBitstream which need to be checked
// Returns:
//      IPP_OK         there is space available
//      IPP_FAIL       there is no space available

*****************************************************************************/
int CheckStreamSpace(IppBitstream *pVideoStream)
{
    if(pVideoStream->bsByteLen > 0 && pVideoStream->pBsCurByte == pVideoStream->pBsBuffer) {
        return IPP_FAIL;
    }else{
        return IPP_OK;
    }

}
