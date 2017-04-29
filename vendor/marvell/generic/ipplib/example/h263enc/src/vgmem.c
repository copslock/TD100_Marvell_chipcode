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
// Name:             malloc_align_64u
// Description:      Malloc a buffer whose start address is 64-bit aligned
//
// Input Arguments:
//      ppAdress     Pointer to the pointer to the adress
//      bufSize		 Buffer size to be allocated
// Output Arguments:
//      ppAdress     Pointer to the pointer to the start address of the allocated buffer,
//					 which is 64-bit aligned
//
// Returns:          0:  malloc failed
//                   1:  malloc ok
*****************************************************************************/
int malloc_align_64u(Ipp8u ** ppAddress, int bufSize)
{
	int tmp;
	Ipp8u * pAddress;

	pAddress = *ppAddress;

	pAddress = (Ipp8u *)malloc( (bufSize) * sizeof(char) + 8);
	if (!pAddress) {
		return 0; 
	}

	
	(tmp)  =(long)(pAddress) & 0x7;							
	*((pAddress) + 7 - tmp) = (unsigned char)(8 - tmp);

	/* get 64 bit-aligned pointer */
	(pAddress) = ((pAddress) + 8) - (long) (pAddress) % 8;	

	*ppAddress = pAddress;

	return 1;
}


/***************************************************************************
// Name:             malloc_align_32u
// Description:      Malloc a buffer whose start address is 32-bit aligned
// Input Arguments:
//      ppAdress     Pointer to the pointer to the adress
//      bufSize		 Buffer size to be allocated
// Output Arguments:
//      ppAdress     Pointer to the pointer to the start address of the allocated buffer,
//					 which is 32-bit aligned
// Returns:          0:  malloc failed
//                   1:  malloc ok
*****************************************************************************/
int malloc_align_32u(Ipp8u ** ppAddress, int bufSize)
{

	int tmp;
	Ipp8u * pAddress;

	pAddress = *ppAddress;

	pAddress = (unsigned char *)malloc( (bufSize) * sizeof(char) + 4);
	if ( !pAddress ) {
		return 0; 
	}

	/* save the offset */
	(tmp)  =(long)(pAddress) & 0x3;							
	*((pAddress) + 3 - tmp) = (unsigned char)(4 - tmp);

	/* get 32 bit-aligned pointer */
	(pAddress) = ((pAddress) + 4) - (long) (pAddress) % 4;	

	*ppAddress = pAddress;

	return 1;
}


/***************************************************************************
// Name:             free_align
// Description:      Free a 32bit or 64 bit aligned buffer allocated by function allocate_align
//
// Input Arguments:
//      ppAdress     Pointer to the pointer to the adress
// Output Arguments:
//      ppAdress     Pointer to the updated pointer to the adress
//
// Returns:          none
*****************************************************************************/
void free_align(Ipp8u * pAddress)
{
	/* get original pointer */
	if(pAddress) {
		pAddress = (pAddress) - *((pAddress)-1);
		free (pAddress);				
	}
}

