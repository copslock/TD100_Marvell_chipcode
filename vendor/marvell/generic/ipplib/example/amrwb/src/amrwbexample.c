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
#include <string.h>
#include "codecSC.h"
#include "misc.h"
#include "ippLV.h"

#define AMRWB_SID_MODE 9
#define MAX_PAR_NAME_LEN    1024
#define MAX_PAR_VALUE_LEN   2048

/* enum used in example */
typedef	enum {
	RUN_BOTH, RUN_ENCODE, RUN_DECODE
} run_type;
/* Command line paramter structure */

typedef	struct {
	char 	szFileNameInputPCM[256];
	char 	szFileNameBitstream[256];
	char 	szFileNameOutputPCM[256];
	char  szFileNameMode[256];
	char  szFileNameLog[256];
	int		nBitRate;
	int		dtxEnable;
	run_type		nCodecType;
	int		formatInd;
} CmdLineOptions;


const Ipp16s AMRWB_Tbl_ModeBits[] = {
	132, 177, 253, 285, 317, 365, 397, 461, 477, 35
};

const Ipp16s AMRWB_Tbl_PackedSize[] = {
	17, 23, 32, 36, 40, 46, 50, 58, 60,  5,  0,  0,  0,  0,  0,  0
};

const Ipp16s AMRWB_Tbl_IF2_PackedSize[] = {
	17, 22, 32, 36, 40, 46, 50, 58, 60,  5,  0,  0,  0,  0,  0,  0
};

int ParseCmdLine(char *pCmdLine, CmdLineOptions *options);
int RunCodec(CmdLineOptions *options);
int RunBoth(CmdLineOptions *options);
int RunEncoder(CmdLineOptions *options);
int RunDecoder(CmdLineOptions *options);
void Usage();
Ipp32s ReadPacket(IPP_FILE * fIn, IppBitstream *pStream, IppAMRWBCodecConfig *pConfig);
int	ReadMode(IPP_FILE *mode_file, int *mode,char * pLogFile);
int SelectMode(int mode,char*pLogFile);
void	DisplayEncLibVersion(void);
void	DisplayDecLibVersion(void);

/******************************************************************************
// Name:			 DisplayEncLibVersion
//
// Description:		 Display library build information on the text console
//
// Input Arguments:  None.
//
// Output Arguments: None				
//
// Returns:			 None				
*******************************************************************************/
void DisplayEncLibVersion()
{
	char libversion[128]={'\0'};
	IppCodecStatus ret;
	ret = GetLibVersion_AMRWBENC(libversion,sizeof(libversion));
	if(0 == ret){
		IPP_Printf("\n*****************************************************************\n");
	    IPP_Printf("This encoder library is built from %s\n",libversion);
		IPP_Printf("*****************************************************************\n");
	}else{
		IPP_Printf("\n*****************************************************************\n");
		IPP_Printf("Can't find this library version information\n");
		IPP_Printf("*****************************************************************\n");
	}
}

/******************************************************************************
// Name:			 DisplayDecLibVersion
//
// Description:		 Display library build information on the text console
//
// Input Arguments:  None.
//
// Output Arguments: None				
//
// Returns:			 None				
*******************************************************************************/
void DisplayDecLibVersion()
{
	char libversion[128]={'\0'};
	IppCodecStatus ret;
	ret = GetLibVersion_AMRWBDEC(libversion,sizeof(libversion));
	if(0 == ret){
		IPP_Printf("\n*****************************************************************\n");
	    IPP_Printf("This decoder library is built from %s\n",libversion);
		IPP_Printf("*****************************************************************\n");
	}else{
		IPP_Printf("\n*****************************************************************\n");
		IPP_Printf("Can't find this library version information\n");
		IPP_Printf("*****************************************************************\n");
	}
}

/*********************************************************************************************
// Name:			 RunCodec
//
// Description:	 run AMRWB codec with the options, select to run encode/decode/both
//
**********************************************************************************************/
int RunCodec(CmdLineOptions *options)
{
	int rt;

	switch (options->nCodecType) {
	case RUN_BOTH:
    	DisplayEncLibVersion();
	    DisplayDecLibVersion();

		rt = RunBoth(options);
		break;
	case RUN_ENCODE:
		DisplayEncLibVersion();

		rt = RunEncoder(options);
		break;
	case RUN_DECODE:
		DisplayDecLibVersion();

		rt = RunDecoder(options);
		break;
	default:
		IPP_Log(options->szFileNameLog,"a","options->nCodecType not valid, abort.\n");
		return IPP_FAIL;
	}
	return rt;
}

/*********************************************************************************************
// Name:			 RunBoth
//
// Description:	 run GSMAMR codec with the options
//
**********************************************************************************************/
int RunBoth(CmdLineOptions *options)
{

	/* GSMAMR related structures and buffers */
	void *pDecoderState;		/* Test vector decoder state */
	void *pEncoderState;			/* Test vector encoder state */
	Ipp16s				InputBuffer[320];	  /* One frame input pcm data */
	Ipp16s				OutputBuffer[320];	  /* One frame output pcm data */

	IPP_FILE * in, *out, *cod, *mode;
	int frame;

	IppSound speech_in, speech_out;
	IppBitstream bitstream;
	IppAMRWBCodecConfig config;
	Ipp8u *pParaBuf;
	int cnt_in;
	int rt;
	int nMode = 0;

	int nFrameLen,iResult;

	MiscGeneralCallbackTable   *pCallbackTable = NULL;

	in = out = cod = mode = NULL;

	if ( *options->szFileNameInputPCM != '\0' ) {
		in = IPP_Fopen(options->szFileNameInputPCM,"rb");
		if ( !in ) {
			IPP_Log(options->szFileNameLog,"a","cannot open %s, abort.\n",options->szFileNameInputPCM);
			return IPP_FAIL;
		}
    } else {
		IPP_Log(options->szFileNameLog,"a","%s isn't valid, abort.\n",options->szFileNameInputPCM);
		return IPP_FAIL;
    }

	if ( *options->szFileNameOutputPCM != '\0' ) {
		out = IPP_Fopen(options->szFileNameOutputPCM,"wb");
		if ( !out ) {
			IPP_Log(options->szFileNameLog,"a", "cannot open %s, abort.\n",options->szFileNameOutputPCM);
			return IPP_FAIL;
		}
    } else {
		IPP_Log(options->szFileNameLog,"a","%s isn't valid, abort.\n",options->szFileNameOutputPCM);
//		return IPP_FAIL;
    }

	if ( *options->szFileNameBitstream != '\0' ) {
		cod = IPP_Fopen(options->szFileNameBitstream,"wb");
		if ( !cod ) {
			IPP_Log(options->szFileNameLog,"a","cannot open %s, abort.\n",options->szFileNameBitstream);
			return IPP_FAIL;
		}
    } else {
		IPP_Log(options->szFileNameLog,"a","%s isn't valid, abort.\n",options->szFileNameBitstream);
//		return IPP_FAIL;
    }

	if ( *options->szFileNameMode != '\0' ) {
		mode = IPP_Fopen(options->szFileNameMode,"rt");
		if ( !mode ) {
			IPP_Log(options->szFileNameLog,"a","cannot open %s, abort.\n",options->szFileNameMode);
			return IPP_FAIL;
		}
    } 

	speech_in.pSndFrame = InputBuffer;
	speech_out.pSndFrame = OutputBuffer;

	config.bitstreamFormatId = options->formatInd;
	config.bitRate = options->nBitRate;

	if(AMRWB_IF1_FORMAT == config.bitstreamFormatId) {
		IPP_MemCalloc(&pParaBuf,63,8);
	} else if (AMRWB_IF2_FORMAT == config.bitstreamFormatId) {
		IPP_MemCalloc(&pParaBuf,61,8);
	} else if (AMRWB_MIME_FILE_FORMAT == config.bitstreamFormatId) {
		IPP_MemCalloc(&pParaBuf,61,8);
	}else {
		IPP_MemCalloc(&pParaBuf,477 * 2 + 7,8);
	}

	speech_in.pSndFrame = InputBuffer;
	bitstream.pBsBuffer = pParaBuf;

	if(AMRWB_TV_COMPLIANCE_FORMAT == config.bitstreamFormatId) {
		if((int)bitstream.pBsBuffer & 0x1) {
			bitstream.pBsBuffer += 1;
		}
	}

	if(config.bitstreamFormatId == AMRWB_MIME_FILE_FORMAT && cod != NULL) {
		char buf[9] = {0x23, 0x21, 0x41, 0x4d, 0x52, 0x2d, 0x57, 0x42, 0x0a};
		if(cod != NULL) {			
			IPP_Fwrite(buf, 1, 9, cod);
		}
	}
	
	if ( miscInitGeneralCallbackTable(&pCallbackTable) != 0 ) {
		IPP_Log(options->szFileNameLog,"a","Cannot initialize CallbackTable,abort.\n");
		return IPP_FAIL; 
	}

	if( 0 != EncoderInitAlloc_AMRWB(&pEncoderState, options->dtxEnable, pCallbackTable)){
		IPP_Log(options->szFileNameLog,"a","Cannot initialize Encoder,abort.\n");
		return IPP_FAIL; 
	}

	if ( 0 != DecoderInitAlloc_AMRWB(&pDecoderState, pCallbackTable)){
		IPP_Log(options->szFileNameLog,"a","Cannot initialize Decoder,abort.\n");
		return IPP_FAIL; 
	}


	frame = 0;

	while(!IPP_Feof(in)) {
		cnt_in = IPP_Fread(InputBuffer,sizeof(Ipp16s),320,in);
		if ( cnt_in != 320 ) {
			IPP_Log(options->szFileNameLog,"a", "Encoding completed for this frame\n");
			break;
		}
		if ( mode ) {
			rt = ReadMode(mode,&nMode,options->szFileNameLog);
			if ( 0 != rt ) {
				IPP_Log(options->szFileNameLog,"a","Failed to parse mode file, abort.\n");
				return IPP_FAIL;
			}
			config.bitRate = nMode;
		}
		else {
			config.bitRate = options->nBitRate;
		}

		iResult = Encode_AMRWB(&speech_in, &bitstream, &config, pEncoderState);		
		if (IPP_STATUS_NOERR != iResult){
			IPP_Log(options->szFileNameLog,"a","Encoder failed,abort.\n");
			return IPP_FAIL; 
		}

		if ( cod != NULL ) {
			nFrameLen = bitstream.bsByteLen;
			IPP_Fwrite(bitstream.pBsBuffer, sizeof(Ipp8u),nFrameLen,cod);
		}	

		iResult = Decode_AMRWB(&bitstream,&speech_out,pDecoderState, &config);
		if (IPP_STATUS_NOERR != iResult){
			IPP_Log(options->szFileNameLog,"a","Decoder failed,abort.\n");
			return IPP_FAIL; 
		}

		if ( out != NULL) {
			 IPP_Fwrite(OutputBuffer,sizeof(Ipp16s), 320,out);
		}
		
		frame ++;
		if ( frame%50==0 ) 
			IPP_Printf("Current running frame %d ......\n", frame);		
	}

	EncoderFree_AMRWB(&pEncoderState, pCallbackTable);
	DecoderFree_AMRWB(&pDecoderState, pCallbackTable);
	miscFreeGeneralCallbackTable(&pCallbackTable);
	
	IPP_MemFree(&pParaBuf);

	if ( cod ) 
		IPP_Fclose(cod);
	if ( out ) 
		IPP_Fclose(out);
	if ( in ) 
		IPP_Fclose(in);

	return 0;
}


/******************************************************************************
// Name:			 RunEncoder
//
// Description:	 run AMRWB encoder with the options
//
*******************************************************************************/
int RunEncoder(CmdLineOptions *options)
{
	/* AMR_WB related structures and buffers */
	void  *pEncoderState;			/* Encoder state */
	Ipp16s	InputBuffer[IPP_AMRWB_FRAMELENGTH];	  /* One frame input pcm data */
	Ipp8u	*pParaBuf;

	IPP_FILE * in, *cod, *mode;
	int cnt_in, total_in;
	int cnt_cod, total_cod;
	int frame;

	IppSound speech_in;
	IppBitstream bitstream;
	IppAMRWBCodecConfig config;
	
	int rt,iResult;
	int nMode;
	
	MiscGeneralCallbackTable *pCallbackTable = NULL;

	int perf_index;
	long long TotTime;

	in = cod = mode = NULL;
	
	if ( *options->szFileNameInputPCM != '\0' ) {
		in = IPP_Fopen(options->szFileNameInputPCM,"rb");
		if ( !in ) {
			IPP_Log(options->szFileNameLog,"a","cannot open %s, abort.\n",options->szFileNameInputPCM);
			return IPP_FAIL;
		}
    } else {
		IPP_Log(options->szFileNameLog,"a","%s isn't valid, abort.\n",options->szFileNameInputPCM);
		return IPP_FAIL;
    }

	if ( *options->szFileNameBitstream != '\0' ) {
		cod = IPP_Fopen(options->szFileNameBitstream,"wb");
		if ( !cod ) {
			IPP_Log(options->szFileNameLog,"a", "cannot open %s, abort.\n",options->szFileNameBitstream);
			return IPP_FAIL;
		}
    } else {
		IPP_Log(options->szFileNameLog,"a","%s isn't valid, abort.\n",options->szFileNameBitstream);
//		return IPP_FAIL;
    }

	if ( *options->szFileNameMode != '\0' ) {
		mode = IPP_Fopen(options->szFileNameMode,"rt");
		if ( !mode ) {
			IPP_Log(options->szFileNameLog,"a", "cannot open %s, abort.\n",options->szFileNameMode);
			return IPP_FAIL;
		}
    } 

	/* Set the config information */
	config.bitstreamFormatId = options->formatInd;
	
	if(AMRWB_IF1_FORMAT == config.bitstreamFormatId) {
		IPP_MemCalloc(&pParaBuf,63,8);
	} else if (AMRWB_IF2_FORMAT == config.bitstreamFormatId) {
		IPP_MemCalloc(&pParaBuf,61,8);
	} else if (AMRWB_MIME_FILE_FORMAT == config.bitstreamFormatId) {
		IPP_MemCalloc(&pParaBuf,61,8);
	}else {
		IPP_MemCalloc(&pParaBuf,477 * 2 + 7,8);
	}

	speech_in.pSndFrame = InputBuffer;
	bitstream.pBsBuffer = pParaBuf;

	if(AMRWB_TV_COMPLIANCE_FORMAT == config.bitstreamFormatId) {
		if((int)bitstream.pBsBuffer & 0x1) {
			bitstream.pBsBuffer += 1;
		}
	}
	
	if ( miscInitGeneralCallbackTable(&pCallbackTable) != 0 ) {
		IPP_Log(options->szFileNameLog,"a","Cannot initialize CallbackTable,abort.\n");
		return IPP_FAIL; 
	}

	iResult = EncoderInitAlloc_AMRWB(&pEncoderState, options->dtxEnable, pCallbackTable);	
	if( 0 != iResult){
		IPP_Log(options->szFileNameLog,"a","Cannot initialize Encoder,abort.\n");
		return IPP_FAIL; 
	}

	/* initialize for performance data collection */
	IPP_GetPerfCounter(&perf_index, DEFAULT_TIMINGFUNC_START, DEFAULT_TIMINGFUNC_STOP);
	IPP_ResetPerfCounter(perf_index);
	
	total_in = 0;
	total_cod = 0;
	frame = 0;

	if(config.bitstreamFormatId == AMRWB_MIME_FILE_FORMAT && cod != NULL) {
		char buf[9] = {0x23, 0x21, 0x41, 0x4d, 0x52, 0x2d, 0x57, 0x42, 0x0a};
		IPP_Fwrite(buf, 1, 9, cod);
	}

	while(!IPP_Feof(in)) {
		cnt_in = IPP_Fread(InputBuffer,sizeof(Ipp16s),IPP_AMRWB_FRAMELENGTH,in);
		if ( cnt_in != IPP_AMRWB_FRAMELENGTH ) {
			if ( cnt_in != 0 ) {
				IPP_Log(options->szFileNameLog,"a","Input PCM frame length[%d] not equal to %d, \
					abort.\n",cnt_in,IPP_AMRWB_FRAMELENGTH);
			}
			break;
		}

		if ( mode ) {
			rt = ReadMode(mode,&nMode,options->szFileNameLog);
			if ( 0 != rt ) {
				IPP_Log(options->szFileNameLog,"a","Failed to parse mode file, abort.\n");
				return IPP_FAIL;
			}
			config.bitRate = nMode;
		} else {
			config.bitRate = options->nBitRate;
		}

		IPP_StartPerfCounter(perf_index);    
		iResult = Encode_AMRWB(&speech_in, &bitstream, &config, pEncoderState);		
		IPP_StopPerfCounter(perf_index);

		if (IPP_STATUS_NOERR != iResult){
			IPP_Log(options->szFileNameLog,"a","Encoder failed,abort.\n");
			return IPP_FAIL; 
		}

		if ( cod != NULL ) {
			cnt_cod = IPP_Fwrite(bitstream.pBsBuffer,sizeof(Ipp8u),bitstream.bsByteLen,cod);
		} 
		
		frame ++;
		if ( frame%20==0 ) IPP_Printf("Current running frame %d ......\r", frame);
		
	}
	
	IPP_Log(options->szFileNameLog,"a","Finish encoding!\n");
		
	/* log codec performance and exit performance counting */
	TotTime = IPP_GetPerfData(perf_index);
    IPP_FreePerfCounter(perf_index);
	g_Tot_Time[IPP_AUDIO_INDEX] = TotTime;
	g_Frame_Num[IPP_AUDIO_INDEX] = frame;

	EncoderFree_AMRWB(&pEncoderState, pCallbackTable);
	miscFreeGeneralCallbackTable(&pCallbackTable);
	
	IPP_MemFree(&pParaBuf);
		
	if ( cod ) 
		IPP_Fclose(cod);
	if ( in ) 
		IPP_Fclose(in);

	return 0;
}


/*********************************************************************************************
// Name:			 RunDecoder
//
// Description:		 run AMRWB decoder with the options
//
**********************************************************************************************/
int RunDecoder(CmdLineOptions *options)
{
	/* GSMAMR related structures and buffers */
	void *pDecoderState;		/* Test vector decoder state */
	Ipp16s	OutputBuffer[320];	  /* One frame output pcm data */
	Ipp8u *pParaBuf;
												
	IPP_FILE *out, *cod;
	int frame,iResult;

	IppSound speech_out;
	IppBitstream bitstream;
	
	IppAMRWBCodecConfig config;
	int done = 0;
	int readFlag;
	
	MiscGeneralCallbackTable *pCallbackTable = NULL;

	int perf_index;
	long long TotTime;

	out = cod = NULL;	

	if ( *options->szFileNameBitstream != '\0' ) {
		cod = IPP_Fopen(options->szFileNameBitstream,"rb");
		if ( !cod ) {
			IPP_Log(options->szFileNameLog,"a", "cannot open %s, abort.\n",options->szFileNameBitstream);
			return IPP_FAIL;
		}
    } else {
		IPP_Log(options->szFileNameLog,"a","%s isn't valid, abort.\n",options->szFileNameBitstream);
		return IPP_FAIL;
    }

	if ( *options->szFileNameOutputPCM != '\0' ) {
		out = IPP_Fopen(options->szFileNameOutputPCM,"wb");
		if ( !out ) {
			IPP_Log(options->szFileNameLog,"a", "cannot open %s, abort.\n",options->szFileNameOutputPCM);
			return IPP_FAIL;
		}
    } else {
		IPP_Log(options->szFileNameLog,"a","%s isn't valid, abort.\n",options->szFileNameOutputPCM);
//		return IPP_FAIL;
    }

	/* Set the config information */
	config.bitstreamFormatId = options->formatInd;
	
	if(AMRWB_IF1_FORMAT == config.bitstreamFormatId) {
        IPP_MemCalloc(&pParaBuf,63,8);
	} else if (AMRWB_IF2_FORMAT == config.bitstreamFormatId) {
		IPP_MemCalloc(&pParaBuf,61,8);
	} else if (AMRWB_MIME_FILE_FORMAT == config.bitstreamFormatId) {
		IPP_MemCalloc(&pParaBuf,61,8);
	}else {
		IPP_MemCalloc(&pParaBuf,477 * 2 + 6,8);
	}

	speech_out.pSndFrame = OutputBuffer;
	bitstream.pBsBuffer = pParaBuf;
	bitstream.pBsCurByte = pParaBuf;

	if(AMRWB_MIME_FILE_FORMAT == config.bitstreamFormatId) {
		IPP_Fread(pParaBuf, 1, 9, cod);		/* Skip the first 9 byte */
	}
	
	if ( miscInitGeneralCallbackTable(&pCallbackTable) != 0 ) {
		IPP_Log(options->szFileNameLog,"a","Cannot initialize CallbackTable,abort.\n");
		return IPP_FAIL; 
	}

	if ( 0 != DecoderInitAlloc_AMRWB(&pDecoderState, pCallbackTable)){
		IPP_Log(options->szFileNameLog,"a","Cannot initialize Decoder,abort.\n");
		return IPP_FAIL; 
	}
	/* initialize for performance data collection */
	IPP_GetPerfCounter(&perf_index, DEFAULT_TIMINGFUNC_START, DEFAULT_TIMINGFUNC_STOP);
	IPP_ResetPerfCounter(perf_index);

	frame = 0;
	while(!done) {

		readFlag = ReadPacket(cod, &bitstream, &config);

		if(readFlag == 0) {
			done = 1;
			continue;
		}

		IPP_StartPerfCounter(perf_index);
		iResult = Decode_AMRWB(&bitstream, &speech_out,pDecoderState, &config);
		IPP_StopPerfCounter(perf_index);

		if (IPP_STATUS_NOERR != iResult){
			IPP_Log(options->szFileNameLog,"a","Decoder failed,abort.\n");
			return IPP_FAIL; 
		}
		
		if ( out != NULL) {
			IPP_Fwrite(OutputBuffer,sizeof(Ipp16s), 320, out);
		}
		
		frame ++;
		if ( frame%20==0 ) IPP_Printf("Current running frame %d ......\r", frame);
	}

	IPP_Log(options->szFileNameLog,"a","Finish decoding!\n");
		
	/* log codec performance and exit performance counting */
	TotTime = IPP_GetPerfData(perf_index);
    IPP_FreePerfCounter(perf_index);

	g_Tot_Time[IPP_AUDIO_INDEX] = TotTime;
	g_Frame_Num[IPP_AUDIO_INDEX] = frame;

	DecoderFree_AMRWB(&pDecoderState, pCallbackTable);
	
	miscFreeGeneralCallbackTable(&pCallbackTable);

	if ( cod ) 
		IPP_Fclose(cod);
	if ( out ) 
		IPP_Fclose(out);
	IPP_MemFree(&pParaBuf);

	return 0;
}

/****************************************************************************
// Name:			 Usage
//
// Description:		 Display Usage
//
******************************************************************************/
void Usage()
{
	IPP_Printf("\n\n\n");
	IPP_Printf("AMR-WB codec example v0.0.1\n");
	IPP_Printf("Description: perform AMR-WB encode/decode on 16kHz 16bits PCM files\n"); 
	IPP_Printf("             encoded stream file format complies GSMAMR standard\n");
	IPP_Printf("Usage: gmrexample [option]\n");
	IPP_Printf("Options:\n");
	IPP_Printf("-t <both|encode|decode>                select do encode,decode or both\n");
	IPP_Printf("-i <input pcm file>                           source PCM file\n");
	IPP_Printf("-o <output pcm file>                       encoded/decoded PCM file\n");
//	IPP_Printf("-c <stream file>                               i/o encoded stream file, with ETSI standard format\n");
    IPP_Printf("-d:<on|off>                                     on means turn DTX on, off means turn DTX off \n");
	IPP_Printf("-b <0|1|2|3|4|5|6|7>                     compress bit rate\n");
	IPP_Printf("-i2 <bit rate mode file>                     this option will disable -b option\n");
	IPP_Printf("-f <IF1|IF1|MIME|3GPPCOMPLIANCE>	select the AMR-WB bistream format\n");
}

/*********************************************************************************************
// Name:			 ParseCmdLine
//
// Description:		 Parse commmand line, and set options in CmdLineOptions structure
//
// Input Arguments:  
//					            pCmdLine     - Pointer to the input command line
//
// Output Arguments: options		- Pointer to the CmdLineOptions structure, store the options				
//
// Returns:			 Linux style error return code				
**********************************************************************************************/
int ParseCmdLine(char *pCmdLine, CmdLineOptions *options)
{
	int tmp;
	
	char *pCur, *pEnd;
    char par_name[MAX_PAR_NAME_LEN];
    int  par_name_len;
    int  par_value_len;
    char *p1, *p2, *p3;

	options->dtxEnable = 0;
	options->formatInd = 3;  //default
	options->nBitRate = IPP_SPCHBR_23850;
	options->nCodecType = RUN_DECODE;

	pCur = pCmdLine;
    pEnd = pCmdLine + IPP_Strlen(pCmdLine);
	
    while((p1 = IPP_Strstr(pCur, "-"))){
        p2 = IPP_Strstr(p1, ":");
        if (NULL == p2) {
            return IPP_FAIL;
        }
        p3 = IPP_Strstr(p2, " "); /*one space*/
        if (NULL == p3) {
            p3 = pEnd;
        }

        par_name_len    = p2 - p1 - 1;
        par_value_len   = p3 - p2 - 1;

        if ((0 >= par_name_len)  || (MAX_PAR_NAME_LEN <= par_name_len) ||
            (0 >= par_value_len) || (MAX_PAR_VALUE_LEN <= par_value_len)) {
                return IPP_FAIL;
        }

        IPP_Strncpy(par_name, p1 + 1, par_name_len);
		par_name[par_name_len] = '\0';

        if ((0 == IPP_Strcmp(par_name, "h")) || (0 == IPP_Strcmp(par_name, "H"))) {
			Usage();
		}else if ((0 == IPP_Strcmp(par_name, "i")) || (0 == IPP_Strcmp(par_name, "I"))) {
            /*input file*/
			if (options->nCodecType == RUN_DECODE){
				IPP_Strncpy(options->szFileNameBitstream,p2 + 1,par_value_len);
				options->szFileNameBitstream[par_value_len] = '\0';
			}else{
				IPP_Strncpy(options->szFileNameInputPCM, p2 + 1, par_value_len);
				options->szFileNameInputPCM[par_value_len] = '\0';
			}
        } else if ((0 == IPP_Strcmp(par_name, "o")) || (0 == IPP_Strcmp(par_name, "O"))) {
            /*output file*/
			if (options->nCodecType == RUN_DECODE){
				IPP_Strncpy(options->szFileNameOutputPCM, p2 + 1,par_value_len);
				options->szFileNameOutputPCM[par_value_len] = '\0';
			}else{
				IPP_Strncpy(options->szFileNameBitstream, p2 + 1,par_value_len);
				options->szFileNameBitstream[par_value_len] = '\0';
			}
        } else if ((0 == IPP_Strcmp(par_name, "i2")) || (0 == IPP_Strcmp(par_name, "I2"))) {
            /*mode file*/
			IPP_Strncpy(options->szFileNameMode,p2 + 1, par_value_len);
			options->szFileNameMode[par_value_len] = '\0';
        } else if ((0 == IPP_Strcmp(par_name, "l")) || (0 == IPP_Strcmp(par_name, "L"))) {
            /*log file*/
			IPP_Strncpy(options->szFileNameLog,p2 + 1,par_value_len);
			options->szFileNameLog[par_value_len] = '\0';
         } else if ((0 == IPP_Strcmp(par_name, "d")) || (0 == IPP_Strcmp(par_name, "D"))) {
			 if (0 == IPP_Strncmp(p2+1,"on",2)){
				 options->dtxEnable = 1;
			 }else if (0 == IPP_Strncmp((p2+1),"off",3)){
				 options->dtxEnable = 0;
		     }
         } else if ((0 == IPP_Strcmp(par_name, "t")) || (0 == IPP_Strcmp(par_name, "T"))) {  
			 if (0 == IPP_Strncmp(p2+1,"encode",6)){
				 options->nCodecType = RUN_ENCODE;
			 }else if (0 == IPP_Strncmp(p2+1,"decode",6)){
				 options->nCodecType = RUN_DECODE;
			 }else if (0 == IPP_Strncmp(p2+1,"both",4)){
				 options->nCodecType = RUN_BOTH;
			 }else{
				 options->nCodecType = RUN_BOTH;
			 }
         } else if ((0 == IPP_Strcmp(par_name, "b")) || (0 == IPP_Strcmp(par_name, "B"))) {
			 tmp = IPP_Atoi(p2 + 1);
			 switch( tmp ) {
			 case 0:
				 options->nBitRate = IPP_SPCHBR_6600;
				 break;
			 case 1:
				 options->nBitRate = IPP_SPCHBR_8850;
				 break;
			 case 2:
				 options->nBitRate = IPP_SPCHBR_12650;
				 break;
			 case 3:
				 options->nBitRate = IPP_SPCHBR_14250;
				 break;
			case 4:
				options->nBitRate = IPP_SPCHBR_15850;
				break;
			case 5:
				options->nBitRate = IPP_SPCHBR_18250;
				break;
			case 6:
				options->nBitRate = IPP_SPCHBR_19850;
				break;
			case 7:
				options->nBitRate = IPP_SPCHBR_23050;
				break;
			case 8:
				options->nBitRate = IPP_SPCHBR_23850;
				break;
			default:
				options->nBitRate = IPP_SPCHBR_23850;
				IPP_Log(options->szFileNameLog,"a","codec rate not valid, set to 23.85 Kbps\n");
			}
         } else if ((0 == IPP_Strcmp(par_name, "f")) || (0 == IPP_Strcmp(par_name, "F"))) {
			 if ( !IPP_Strncmp(p2+1,"IF1",3)) {
				 options->formatInd = 0;
			 } else if (!IPP_Strncmp(p2+1,"IF2",3)) {
				 options->formatInd = 1;
			 } else if (!IPP_Strncmp(p2+1, "MIME",4)) {
				 options->formatInd = 2;
			 } else if (!IPP_Strncmp(p2+1, "DEFAULT",7)) {
				 options->formatInd = 3;
			 }
         } 
        pCur = p3;
    }

    return IPP_OK;
}

Ipp32s ReadPacket(IPP_FILE * fIn, IppBitstream *pStream, IppAMRWBCodecConfig *pConfig)
{
   Ipp32s frameType, mode;	
   Ipp8u  tmp, *pPacket;
   Ipp16s *pBitstream;

   if(pConfig->bitstreamFormatId == AMRWB_TV_COMPLIANCE_FORMAT)	{	   
	   if(IPP_Fread(pStream->pBsBuffer, sizeof(Ipp16s), 3, fIn) < 3) {
		   return 0;
	   }
	   pBitstream = (Ipp16s *)(pStream->pBsBuffer);
	   frameType = pBitstream[1];
	   mode = pBitstream[2];

	   if (frameType != TX_SPEECH) {
		   mode = AMRWB_SID_MODE;
	   }

	   if(mode>=0 && mode<=9){
		   if((Ipp16s)IPP_Fread(pBitstream + 3, sizeof(Ipp16s), 
			   AMRWB_Tbl_ModeBits[mode], fIn) != AMRWB_Tbl_ModeBits[mode]) {
				   return 0; 
		   }
	   }
	   return 1;

   } else if (pConfig->bitstreamFormatId == AMRWB_MIME_FILE_FORMAT) {	
		/* MIME/storage file format */   
		 pPacket = (Ipp8u*)pStream->pBsBuffer;
		 if(IPP_Fread(pPacket, sizeof(Ipp8u), 1, fIn) < 1) {
			 return 0;
		 }	 		 
	     mode = (pPacket[0] >> 3) & 0x0F;
		 if(mode>= 0 && mode <= 9) {
			if ((Ipp16s)IPP_Fread(pPacket + 1, sizeof(Ipp8u), 
				 AMRWB_Tbl_PackedSize[mode], fIn)  != AMRWB_Tbl_PackedSize[mode]) {
				 return 0;
			}
		 }		 
	    return 1;
   } else if (pConfig->bitstreamFormatId == AMRWB_IF1_FORMAT) { /* IF1 format */
		pPacket = (Ipp8u*)pStream->pBsBuffer;
		if(IPP_Fread(pPacket, sizeof(Ipp8u), 1, fIn) < 1) {
			return 0;
		}
		tmp = pPacket[0];
		mode = (Ipp8u)((tmp >> 4) & 0xf);		
		if (mode>=0 && mode <= 9) {
			IPP_Fread(pPacket + 1, sizeof(Ipp8u), 2, fIn);			
			if ((Ipp16s)IPP_Fread(pPacket + 3, sizeof(Ipp8u), 
			   AMRWB_Tbl_PackedSize[mode], fIn) != AMRWB_Tbl_PackedSize[mode]) {
			   return 0;
			}
		}				 
	   return 1;

   } else if (pConfig->bitstreamFormatId == AMRWB_IF2_FORMAT) { /* IF2 format */
		pPacket = (Ipp8u*)pStream->pBsBuffer;
		if(IPP_Fread(pPacket, sizeof(Ipp8u), 1, fIn) != 1) {
			return 0;
		}
		tmp = pPacket[0];
		mode = (Ipp8u)((tmp >> 4) & 0xf);		
		if(mode>=0 && mode <= 9) {			
			if ((Ipp16s)IPP_Fread(pPacket + 1, sizeof(Ipp8u), 
				AMRWB_Tbl_IF2_PackedSize[mode], fIn) 
				!= AMRWB_Tbl_IF2_PackedSize[mode]) {
			   return 0;
			}
		}				 
	   return 1;
   }
   return 0;
}

/*********************************************************************************************
// Name:			 SelectMode
//
// Description:		 Translate mode from 122 to IPP_SPCHBR_12200, and etc.
//
**********************************************************************************************/
int SelectMode(int mode,char * pLogFile) 
{
	switch( mode ) {
	case 8:
		return IPP_SPCHBR_23850;
	case 7:
		return IPP_SPCHBR_23050;
	case 6:
		return IPP_SPCHBR_19850;
	case 5:
		return IPP_SPCHBR_18250;
	case 4:
		return IPP_SPCHBR_15850;
	case 3:
		return IPP_SPCHBR_14250;
	case 2:
		return IPP_SPCHBR_12650;
	case 1:
		return IPP_SPCHBR_8850;
	case 0:
		return IPP_SPCHBR_6600;
	default:
		IPP_Log(pLogFile,"a","Mode invalid, set to default[IPP_SPCHBR_6600]\n");
		return IPP_SPCHBR_6600;
	}

}


#define	MAX_DIGIT	10

int	ReadMode(IPP_FILE *mode_file, int *mode,char * LogFile)
{
	char szMode[MAX_DIGIT];
	int ch;
	int state;
	int cnt;
	cnt = 0;
	state = 0;
	while(!IPP_Feof(mode_file)) {
		ch = IPP_Fgetc(mode_file);
		szMode[0] = (char)ch;
		szMode[1] = '\0';
		*mode = SelectMode(IPP_Atoi(szMode),LogFile);
		ch = IPP_Fgetc(mode_file);
		return 0;
	}

	return IPP_FAIL;
}
#undef	MAX_DIGIT

/******************************************************************************
// Name:				CodecTest
// Description:			Main entry for AMR-WB codec test
//
// Input Arguments:
//						N/A
// Output Arguments:	
//						N/A
// Returns:
******************************************************************************/
int CodecTest(int argc, char** argv)
{
    int ret;	
    CmdLineOptions options;
    IPP_Memset(&options, 0, sizeof(CmdLineOptions));

	if(argc == 2 && ParseCmdLine(argv[1],&options) == 0){
		ret = RunCodec(&options);
		return (ret);
	}else{
		IPP_Printf("Command is incorrect!\n         \
		   Usage:appamrwb.exe \"-t:encode -i:in.wav -o:out.cod -l:log.txt -d:off -b:0 -f:default\"	\n \
			   or    appamrwb.exe \"-t:decode -i:out.cod -o:out.wav -l:log.txt -d:off -b:0 -f:default\"	\n \
			   -t: <both|encode|decode>   select do encode,decode or both\n	\
			   -i: <input pcm file>                 source PCM file\n\
			  -o: <output pcm file>             encoded/decoded PCM file \n \
			  -c: <stream file>                    i/o encoded stream file, with ETSI standard format \n \
			   -l: <log file>                          is the log information file \n			\
			   -d:<on|off>                          on means turn DTX on, off means turn DTX off \n \
			   -b:<0|1|2|3|4|5|6|7|8>      compress bit rate\n  \
			  -i2:<bit rate mode file>          this option will disable -b option\n \
			    -f:<IF1|IF2|MIME|DEFAULT>	select the AMR-WB bistream format\n	\
		");
		return IPP_FAIL;
	}
}
/* EOF */
