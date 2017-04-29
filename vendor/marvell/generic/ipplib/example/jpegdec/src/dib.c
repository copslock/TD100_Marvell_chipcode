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
        IPP_MemFree( &(pDIBSpec->pBitmapData) );
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
    pDIBSpec->pBitmapData=0;
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
//     [Success] - IPP_STATUS_NOERR
//     [Fail]    - IPP_STATUS_NOMEM_ERR
//               - IPP_STATUS_NOTSUPPORTED_ERR
//               - IPP_SATUS_ERR
******************************************************************************/
IppCodecStatus ReadDIBFile(MDIBSPEC *pDIBSpec, IPP_FILE *file)
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
    if ( (IPP_Fread(anData8, 1, 2, file)!=2)||(anData8[0]!='B')||(anData8[1]!='M') ) {
        return IPP_STATUS_ERR;
    }

    /* bfSize - 4;             */
    if ( (IPP_Fread(&bfSize, 4, 1, file)!=1) ) {
        return IPP_STATUS_ERR;
    }

    /* bfReserved1 - 2;        */
    /* bfReserved2 - 2;        */
    IPP_Fseek(file, 4, IPP_SEEK_CUR);

    /* bfOffBits - 4;          */
    if ( (IPP_Fread(&bfOffBits, 4, 1, file)!=1) ) {
        return IPP_STATUS_ERR;
    }

    /* BITMAPINFOHEADER */

    /* biSize - 4 */
    if ( (IPP_Fread(&data32, 4, 1, file)!=1)||(0==data32) ) {
        return IPP_STATUS_ERR;
    }

    /* biWidth - 4 */
    if ( (IPP_Fread(&data32, 4, 1, file)!=1)||(0==data32) ) {
        return IPP_STATUS_ERR;
    }
    pDIBSpec->nWidth=data32;

    /* biHeight - 4 */
    if ( (IPP_Fread(&data32, 4, 1, file)!=1)||(0==data32) ) {
        return IPP_STATUS_ERR;
    }
    pDIBSpec->nHeight=data32;

    /* biPlanes - 2 */
    if ( IPP_Fread(&data16, 2, 1, file)!=1 ) {
        return IPP_STATUS_ERR;
    }

    /* biBitCount - 2 */
    if ( (IPP_Fread(&data16, 2, 1, file)!=1) ) {
        return IPP_STATUS_ERR;
    }
    pDIBSpec->nBitsPerpixel=data16;

    pDIBSpec->nStep     = IIP_WIDTHBYTES_4B((pDIBSpec->nWidth)*(pDIBSpec->nBitsPerpixel));
    pDIBSpec->nDataSize = pDIBSpec->nStep*pDIBSpec->nHeight;

    /* Allocate memory for bitmap data */
    pDIBSpec->pBitmapData = NULL;

    /* biCompression - 4 */
    if ( (IPP_Fread(&biCompression, 4, 1, file)!=1) ) {
        return IPP_STATUS_ERR;
    }

    if ( (BI_RGB!=biCompression) && (BI_BITFIELDS!=biCompression) ) {
        return IPP_STATUS_NOTSUPPORTED_ERR;
    }

    /* biSizeImage - 4 */
    if (IPP_Fread(&data32, 4, 1, file)!=1) {
        return IPP_STATUS_ERR;
    }

    /* biXPelsPerMeter - 4 */
    if ( IPP_Fread(&data32, 4, 1, file)!=1 ) {
        return IPP_STATUS_ERR;
    }

    /* biYPelsPerMeter - 4 */
    if ( IPP_Fread(&data32, 4, 1, file)!=1 ) {
        return IPP_STATUS_ERR;
    }

    /* biClrUsed - 4 */
    if ( IPP_Fread(&data32, 4, 1, file)!=1 ) {
        return IPP_STATUS_ERR;
    }

    /* biClrImportant - 4 */
    if ( IPP_Fread(&data32, 4, 1, file)!=1 ) {
        return IPP_STATUS_ERR;
    }
    
    /* Get Color Mode */
    switch (pDIBSpec->nBitsPerpixel) {
        case 8:
            pDIBSpec->nNumComponent=1;
            pDIBSpec->nPrecision=8;
            pDIBSpec->nClrMode=JPEG_GRAY8;
            break;
        case 24:
            pDIBSpec->nNumComponent=3;
            pDIBSpec->nPrecision=8;
            pDIBSpec->nClrMode=JPEG_BGR888;
            break;
        case 16:
            pDIBSpec->nNumComponent=3;
            pDIBSpec->nPrecision=8;
            if ( BI_BITFIELDS==biCompression ) {
                if ( IPP_Fread(&anMask, 4, 3, file)!=3 ) {
                    return IPP_STATUS_ERR;
                }
                if ( (0x001F==anMask[0])||(0x03E0==anMask[1])||(0x7C00==anMask[2]) ) {
                    pDIBSpec->nClrMode=JPEG_BGR555;
                } else if ( (0x001F==anMask[0])||(0x07E0==anMask[1])||(0xF800==anMask[2]) ) {
                    pDIBSpec->nClrMode=JPEG_BGR565;
                } else {
                    return IPP_STATUS_NOTSUPPORTED_ERR;
                }
            }
            break;
        default:
            return IPP_STATUS_NOTSUPPORTED_ERR;
    }

    IPP_Fseek(file, bfOffBits, IPP_SEEK_SET);

    return IPP_STATUS_NOERR;
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
//     [Success] - IPP_STATUS_NOERR
//     [Fail]    - IPP_SATUS_ERR
******************************************************************************/
IppCodecStatus WriteDIBFile(MDIBSPEC *pDIBSpec, IPP_FILE *file)
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
        return IPP_STATUS_ERR;
    }

    /* bfSize - 4;             */
    if ( (IPP_Fwrite(&bfSize, 4, 1, file)!=1) ) {
        return IPP_STATUS_ERR;
    }

    /* bfReserved1 - 2;        */
    /* bfReserved2 - 2;        */
    IPP_Fseek(file, 4, IPP_SEEK_CUR);

    /* bfOffBits - 4;          */
    if ( (IPP_Fwrite(&bfOffBits, 4, 1, file)!=1) ) {
        return IPP_STATUS_ERR;
    }

    /* BITMAPINFOHEADER */

    /* biSize - 4 */
    data32 = 40;
    if ( (IPP_Fwrite(&data32, 4, 1, file)!=1) ) {
        return IPP_STATUS_ERR;
    }

    /* biWidth - 4 */
    data32=pDIBSpec->nWidth;
    if ( (IPP_Fwrite(&data32, 4, 1, file)!=1) ) {
        return IPP_STATUS_ERR;
    }

    /* biHeight - 4 */
    data32=pDIBSpec->nHeight;
    if ( (IPP_Fwrite(&data32, 4, 1, file)!=1) ) {
        return IPP_STATUS_ERR;
    }

    /* biPlanes - 2 */
    data16=1;
    if ( IPP_Fwrite(&data16, 2, 1, file)!=1 ) {
        return IPP_STATUS_ERR;
    }

    /* biBitCount - 2 */
    data16=(Ipp16u)pDIBSpec->nBitsPerpixel;
    if ( (IPP_Fwrite(&data16, 2, 1, file)!=1) ) {
        return IPP_STATUS_ERR;
    }

    /* biCompression - 4 */
    biCompression = (pDIBSpec->nClrMode==JPEG_BGR555 || pDIBSpec->nClrMode==JPEG_BGR565) ? BI_BITFIELDS : BI_RGB;
    if ( IPP_Fwrite(&biCompression, 4, 1, file)!=1 ) {
        return IPP_STATUS_ERR;
    }

    /* biSizeImage - 4 */
    data32 = pDIBSpec->nDataSize;
    if ( (IPP_Fwrite(&data32, 4, 1, file)!=1) ) {
        return IPP_STATUS_ERR;
    }

    /* biXPelsPerMeter - 4 */
    data32 = 72;
    if ( IPP_Fwrite(&data32, 4, 1, file)!=1 ) {
        return IPP_STATUS_ERR;
    }

    /* biYPelsPerMeter - 4 */
    data32 = 72;
    if ( IPP_Fwrite(&data32, 4, 1, file)!=1 ) {
        return IPP_STATUS_ERR;
    }

    /* biClrUsed - 4 */
    data32 = 0;
    if ( IPP_Fwrite(&data32, 4, 1, file)!=1 ) {
        return IPP_STATUS_ERR;
    }

    /* biClrImportant - 4 */
    data32 = 0;
    if ( IPP_Fwrite(&data32, 4, 1, file)!=1 ) {
        return IPP_STATUS_ERR;
    }
    
    /* Get Color Mode */
    if (JPEG_BGR555==pDIBSpec->nClrMode) {
        anMask[2]=0x001F;
        anMask[1]=0x03E0;
        anMask[0]=0x7C00;
        if ( IPP_Fwrite(&anMask, 4, 3, file)!=3 ) {
            return IPP_STATUS_ERR;
        }
    } else if (JPEG_BGR565==pDIBSpec->nClrMode) {
        anMask[2]=0x001F;
        anMask[1]=0x07E0;
        anMask[0]=0xF800;
        if ( IPP_Fwrite(&anMask, 4, 3, file)!=3 ) {
            return IPP_STATUS_ERR;
        }
    } else if (JPEG_GRAY8==pDIBSpec->nClrMode) {
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
        return IPP_STATUS_ERR;
    }

    IPP_Fseek(file, data32, IPP_SEEK_SET);

    data32 = IPP_Fwrite(pDIBSpec->pBitmapData, 1, pDIBSpec->nDataSize, file);

    if (  data32 != (Ipp32u)(pDIBSpec->nDataSize) ) {
        return IPP_STATUS_ERR;
    }

    /* bfOffBits -4 */
    data32=IPP_Ftell(file);
    IPP_Fseek(file, 2, IPP_SEEK_SET);
    if ( IPP_Fwrite(&data32, 4, 1, file)!=1 ) {
        return IPP_STATUS_ERR;
    }

    IPP_Fseek(file, 0, IPP_SEEK_END);
    return IPP_STATUS_NOERR;
}

/* EOF */
