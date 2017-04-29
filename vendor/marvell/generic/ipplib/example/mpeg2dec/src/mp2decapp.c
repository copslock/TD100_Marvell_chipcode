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


#include "mp2decocder.h"

int _tmain(int argc,_TCHAR *argv[]) 
{
	return MP2DecApp(argc,argv);
}

int MP2DecApp(int argc, _TCHAR *argv[])
{
	FILE * fpInput =NULL ,*fpOutput = NULL; 
	unsigned long	FrameNumber;

	if ( argc < 3 )	{
		_tprintf(_T("Parameter error!\n"));
		_tprintf(_T("Usage: xx.exe <input stream> <output>\n"));
		_tprintf(_T("Example: xx.exe a.m2v b.yuv\n"));
		return 0;
	}

	if (CmdOptionsParser(argc,argv,&fpInput)<0) {
		return -1;
	}

	fpOutput = _tfopen(argv[2],_T("wb"));
	if(fpOutput == NULL) {
		fclose(fpInput);
		return -1;
	}

	MPEG2decoder(fpInput,fpOutput,&FrameNumber);

    fclose(fpInput);
    fclose(fpOutput);

	return 0;
};

int CmdOptionsParser(int argCnt, _TCHAR *argVal[], FILE **ppFile)
{
	*ppFile = _tfopen(argVal[1],_T("rb"));
	if (argCnt<2||!(*ppFile))
	{
		_tprintf(_T("Invalid Arguments!\n %s\n"),argVal[1]);
		return -1;
	}
	return 1;
};

