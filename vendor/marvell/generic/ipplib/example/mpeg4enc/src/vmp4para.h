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

#ifndef __VMP4PARA_H_
#define __VMP4PARA_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "codecVC.h"

/* Fix PVCS Defect #102 */
/* PVCS Defect #102 fixed */

#define TOKEN_SIZE 4096

#define ERES_OK 0
#define ERES_PARAMS 1
#define ERES_NOOBJ 2
#define ERES_MEMORY 3
#define ERES_EOF 4
#define ERES_FORMAT 5

typedef int Bool; 

int	 parGetToken(FILE *m_fp, char *pchBuf);
int	 parSkipComment(FILE *m_fp);
int	 parSkipSpace(FILE *m_fp);

IPPCODECAPI(IppCodecStatus, GetPar_MPEG4Enc, (IppParSet *pParInfo, IppISConfig *pISConfig, int *pISEnable, FILE *fp));
IPPCODECAPI(IppCodecStatus, UpdateStandardPar_MPEG4, (void *pParInfo));
IPPCODECAPI(IppCodecStatus, UpdateAlgorithmPar_External_MPEG4, (void *pParInfo));

int ParseCmd_MPEGEnc(char *pCmdLine, 
                     char *pSrcFileName, 
                     char *pDstFileName, 
                     char *pLogFileName, 
                     /*IppParSet*/void *pParInfo, 
                     /*IppISConfig*/void *pISConfig,
                     int   *pISEnable);

#endif	//end of file
