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


#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#if 0
#include	"v263euser.h"

int CodecTestttt(int argc, char *argv[])
{
	FILE	*infile = NULL;
	FILE	*outfile = NULL;
	int		ret = 0;
	unsigned long ulFrameNumber;


	if(argc < 2){
		IPP_Printf("Usage: h263Enc.exe input_par_file \n");
		return -1;
	}
	
	infile = fopen(argv[1], "r");
	/* Read Par file */
	if (!infile){
		IPP_Printf("Can't open input Par file! \n");
		return -1;
	}

	if (argc > 2){
		outfile = fopen(argv[2], "wb");
		if (!outfile){
			IPP_Printf("Can't creat reconstructed file \n");
			return -1;
		}
	}
	
	ret = H263encoder(infile, &ulFrameNumber);

	switch (ret)
	{
	case IPP_STATUS_NOERR:
		IPP_Printf("Everything is Okay\n");
		break;
	case IPP_STATUS_ERR:
		IPP_Printf("Unknown/Unspecified Error\n");
		break;
	case IPP_STATUS_NOMEM_ERR:
		IPP_Printf("Memory allocation Failed\n");
		break;
	case IPP_STATUS_BADARG_ERR:
		IPP_Printf("Bad Argument Error \n");
		break;
	case IPP_STATUS_SYNCNOTFOUND_ERR:
		IPP_Printf("Miss Synchronize code\n");
		break;
	case IPP_STATUS_INPUT_ERR:
		IPP_Printf("Wrong input parameter\n");
		break;
	case IPP_STATUS_NOTSUPPORTED_ERR:
		IPP_Printf("Not support in current version yet\n");
		break;
	case IPP_STATUS_BUFOVERFLOW_ERR:
		IPP_Printf("Output Bistream Buffer Overflow\n");
		break;
	default:
		IPP_Printf("Out of control\n");
		break;
	}

	if ( infile ){
	  fclose (infile);
	}
	if ( outfile ){
	  fclose (outfile); 
	}
	return ret ? -1 : 0;

}




#endif
