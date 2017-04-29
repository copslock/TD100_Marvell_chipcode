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


#include "vguser.h"


/***************************************************************************
// Name:             QCIFYTo64x48
// Description:      Luminance component subsampling from QCIF to 64x48, the subsampled width is 64 pixels,
//					 and the height is 48 pixels
//
// Input Arguments:
//      pSrc		Pointer to the input raw data
//      srcStep		The step size for input raw data
//		dstStep		The step size for output data
// Output Arguments:
//      pDst		Pointer to the output subsamped data
//
// Returns:          0:  subsample okay
*****************************************************************************/
int QCIFYTo64x48(const Ipp8u * pSrc, int srcStep, Ipp8u * pDst, int dstStep)
{
	int ix, isrcx, iy;


	for (iy = 0; iy < 48; iy ++, pDst += dstStep, pSrc += 3 * srcStep) {
		for (ix = 0, isrcx = 0; ix < 48; ix += 8, isrcx += 24) {
			pDst[ix] = pSrc[isrcx];
			pDst[ix + 1] = pSrc[isrcx + 3];
			pDst[ix + 2] = pSrc[isrcx + 6];
			pDst[ix + 3] = pSrc[isrcx + 9];
			pDst[ix + 4] = pSrc[isrcx + 12];
			pDst[ix + 5] = pSrc[isrcx + 15];
			pDst[ix + 6] = pSrc[isrcx + 18];
			pDst[ix + 7] = pSrc[isrcx + 21];
		}
		for (ix = 48, isrcx = 144; ix < 64; ix += 8, isrcx += 16) {
			pDst[ix] = pSrc[isrcx];
			pDst[ix + 1] = pSrc[isrcx + 2];
			pDst[ix + 2] = pSrc[isrcx + 4];
			pDst[ix + 3] = pSrc[isrcx + 6];
			pDst[ix + 4] = pSrc[isrcx + 8];
			pDst[ix + 5] = pSrc[isrcx + 10];
			pDst[ix + 6] = pSrc[isrcx + 12];
			pDst[ix + 7] = pSrc[isrcx + 14];
		}
	}

	return  0;
}

/***************************************************************************
// Name:             QCIFYTTo48x64
// Description:      Luminance component subsampling from QCIF to 48x64, the subsampled width is 48 pixels,
//					 and the height is 64 pixels
//
// Input Arguments:
//      pSrc		Pointer to the input raw data
//      srcStep		The step size for input raw data
//		dstStep		The step size for output data
// Output Arguments:
//      pDst		Pointer to the output subsamped data
//
// Returns:          0:  subsample okay
*****************************************************************************/
int QCIFYTTo48x64(const Ipp8u * pSrc, int srcStep, Ipp8u * pDst, int dstStep)
{
	int ix, isrcx, iy;

	for (iy = 0; iy < 48; iy ++, pDst += dstStep, pSrc += 3 * srcStep) {
		for (ix = 0, isrcx = 0; ix < 48; ix += 8, isrcx += 24) {
			pDst[ix] = pSrc[isrcx];
			pDst[ix + 1] = pSrc[isrcx + 3];
			pDst[ix + 2] = pSrc[isrcx + 6];
			pDst[ix + 3] = pSrc[isrcx + 9];
			pDst[ix + 4] = pSrc[isrcx + 12];
			pDst[ix + 5] = pSrc[isrcx + 15];
			pDst[ix + 6] = pSrc[isrcx + 18];
			pDst[ix + 7] = pSrc[isrcx + 21];
		}
	}
	for (iy = 48; iy < 64; iy ++, pDst += dstStep, pSrc += 2 * srcStep) {
		for (ix = 0, isrcx = 0; ix < 48; ix += 8, isrcx += 24) {
			pDst[ix] = pSrc[isrcx];
			pDst[ix + 1] = pSrc[isrcx + 3];
			pDst[ix + 2] = pSrc[isrcx + 6];
			pDst[ix + 3] = pSrc[isrcx + 9];
			pDst[ix + 4] = pSrc[isrcx + 12];
			pDst[ix + 5] = pSrc[isrcx + 15];
			pDst[ix + 6] = pSrc[isrcx + 18];
			pDst[ix + 7] = pSrc[isrcx + 21];
		}
	}

	return  0;
}


/***************************************************************************
// Name:             QCIFCTo64x48
// Description:      Chrominance component subsampling from QCIF to 64x48, the subsampled width is 32 pixels,
//					 and the height is 24 pixels
//
// Input Arguments:
//      pSrc		Pointer to the input raw data
//      srcStep		The step size for input raw data
//		dstStep		The step size for output data
// Output Arguments:
//      pDst		Pointer to the output subsamped data
//
// Returns:          0:  subsample okay
*****************************************************************************/
int QCIFCTo64x48(const Ipp8u * pSrc, int srcStep, Ipp8u * pDst, int dstStep)
{
	int ix, isrcx, iy;

	for (iy = 0; iy < 24; iy ++, pDst += dstStep, pSrc += 3 * srcStep) {
		for (ix = 0, isrcx = 0; ix < 24; ix += 8, isrcx += 24) {
			pDst[ix] = pSrc[isrcx];
			pDst[ix + 1] = pSrc[isrcx + 3];
			pDst[ix + 2] = pSrc[isrcx + 6];
			pDst[ix + 3] = pSrc[isrcx + 9];
			pDst[ix + 4] = pSrc[isrcx + 12];
			pDst[ix + 5] = pSrc[isrcx + 15];
			pDst[ix + 6] = pSrc[isrcx + 18];
			pDst[ix + 7] = pSrc[isrcx + 21];
		}
		
			pDst[24] = pSrc[72];
			pDst[25] = pSrc[74];
			pDst[26] = pSrc[76];
			pDst[27] = pSrc[78];
			pDst[28] = pSrc[80];
			pDst[29] = pSrc[82];
			pDst[30] = pSrc[84];
			pDst[31] = pSrc[86];		
	}

	return  0;
}


/***************************************************************************
// Name:             QCIFCTTo48x64
// Description:      Chrominance component subsampling from QCIF to 48x64, the subsampled width is 24 pixels,
//					 and the height is 32 pixels
//
// Input Arguments:
//      pSrc		Pointer to the input raw data
//      srcStep		The step size for input raw data
//		dstStep		The step size for output data
// Output Arguments:
//      pDst		Pointer to the output subsamped data
//
// Returns:          0:  subsample okay
*****************************************************************************/
int QCIFCTTo48x64(const Ipp8u * pSrc, int srcStep, Ipp8u * pDst, int dstStep)
{
	int ix, isrcx, iy;

	for (iy = 0; iy < 24; iy ++, pDst += dstStep, pSrc += 3 * srcStep) {
		for (ix = 0, isrcx = 0; ix < 24; ix += 8, isrcx += 24) {
			pDst[ix] = pSrc[isrcx];
			pDst[ix + 1] = pSrc[isrcx + 3];
			pDst[ix + 2] = pSrc[isrcx + 6];
			pDst[ix + 3] = pSrc[isrcx + 9];
			pDst[ix + 4] = pSrc[isrcx + 12];
			pDst[ix + 5] = pSrc[isrcx + 15];
			pDst[ix + 6] = pSrc[isrcx + 18];
			pDst[ix + 7] = pSrc[isrcx + 21];
		}
	}

	for (iy = 24; iy < 32; iy ++, pDst += dstStep, pSrc += 2 * srcStep) {
		for (ix = 0, isrcx = 0; ix < 24; ix += 8, isrcx += 24) {
			pDst[ix] = pSrc[isrcx];
			pDst[ix + 1] = pSrc[isrcx + 3];
			pDst[ix + 2] = pSrc[isrcx + 6];
			pDst[ix + 3] = pSrc[isrcx + 9];
			pDst[ix + 4] = pSrc[isrcx + 12];
			pDst[ix + 5] = pSrc[isrcx + 15];
			pDst[ix + 6] = pSrc[isrcx + 18];
			pDst[ix + 7] = pSrc[isrcx + 21];
		}
	}

	return  0;
}

