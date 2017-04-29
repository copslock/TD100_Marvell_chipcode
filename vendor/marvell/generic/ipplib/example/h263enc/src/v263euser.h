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

#ifndef	__V263EUSER_H__
#define	__V263EUSER_H__

#include "ippdefs.h"
#include "codecVC.h"
#include "vguser.h"
#include "misc.h"


#ifdef __cplusplus
extern "C" {
#endif


extern int H263encoder(IppH263ParSet *pParInfo, unsigned long * pFrameNumber, char *pLog);

IPPCODECAPI(IppCodecStatus, GetPar_H263, (IppH263ParSet *pParInfo, FILE *fp));
IPPCODECAPI(IppCodecStatus, UpdateStandardPar_H263, (void *pParInfo));
IPPCODECAPI(IppCodecStatus, UpdateAlgorithmPar_External_H263, (void *pParInfo));

IPPCODECAPI(IppCodecStatus, GetPar_H263Enc,(IppH263ParSet *pParInfo, FILE *pfPara));

IPPCODECAPI(int, ParseCmd_H263Enc,(char *pCmdLine, char *pSrcFileName, char *pDstFileName, 
                                   char *pLogFileName, /*IppParSet*/void *pParInfo));


#ifdef __cplusplus
}
#endif


#endif
