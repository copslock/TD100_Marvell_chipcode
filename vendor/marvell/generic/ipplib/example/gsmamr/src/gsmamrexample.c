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


/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Standard IPP definitions */
#include <ippdefs.h>
#include <ippSC.h>

/* IPP codec interface */
#include <codecSC.h>
#include <misc.h>


#include "ippLV.h"

///////////////////////////////////////////////////////////////////////////////
// GSMAMR API that not exposed by ippCodec.h
///////////////////////////////////////////////////////////////////////////////
IppStatus appsUnpackTestVector_GSMAMR_8u16s(const Ipp8u *pSrcBitstream,
											Ipp16s *pDstTestVector);
IppStatus appsPackTestVector_GSMAMR_8u16s(const Ipp16s *pSrcTestVector,
										  Ipp8u *pDstPackedBitstream);


/* enum used in example */
typedef	enum {
	RUN_BOTH, RUN_ENCODE, RUN_DECODE
} run_type;

typedef	enum {
	IF1, IF2, RAWIF1, COMPLIANCE
} bitstreawm_type;

/* Command line paramter structure*/
typedef	struct {
	char 	szFileNameInputPCM[256];
	char 	szFileNameBitstream[256];
	char 	szFileNameOutputPCM[256];
    char  szFileNameLog[256];
	char  szFileNameMode[256];
	int		nDtxState;
	int		nVadModel;
	int		nBitRate;
	run_type		nCodecType;
	bitstreawm_type	formatInd;
} CmdLineOptions;

#define MAX_PAR_NAME_LEN    1024
#define MAX_PAR_VALUE_LEN   2048

/* Supporting routines*/
int ParseCmdLine(char *pCmdLine, CmdLineOptions *options);
void Usage();
int RunCodec(CmdLineOptions *options);
int RunBoth(CmdLineOptions *options);
int RunEncoder(CmdLineOptions *options);
int RunDecoder(CmdLineOptions *options);

int	ReadMode(IPP_FILE *mode_file,int *mode,char *pLogFile);
int SelectMode(int mode,char*pLogFile);

int GetFrameLengthByFirstStreamByte(unsigned char uFirstStreamByte, IppGSMAMRFormatConfig 
									*pConfig, char* pLogFile);
int GetFrameLength(int nFrameType,char *pLogFile);
int Least4BitsReversed(int val);
void DisplayEncLibVersion();
void DisplayDecLibVersion();


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
	ret = GetLibVersion_GSMAMRENC(libversion,sizeof(libversion));
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
	ret = GetLibVersion_GSMAMRDEC(libversion,sizeof(libversion));
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
// Name:			 ParseCmdLine
//
// Description:		 Parse commmand line, and set options in CmdLineOptions structure
//
// Input Arguments:   pCmdLine    :   Pointer to the input command line
//
// Output Arguments: options		- Pointer to the CmdLineOptions structure, store the options				
//
// Returns:			 Linux style error return code				
**********************************************************************************************/
int ParseCmdLine(char *pCmdLine, CmdLineOptions *options)
{

#define STRNCPY(dst, src, len) \
{\
    IPP_Strncpy((dst), (src), (len));\
    (dst)[(len)] = '\0';\
}

	int tmp;
	
	char *pCur, *pEnd;
    char par_name[MAX_PAR_NAME_LEN];
    int  par_name_len;
    int  par_value_len;
    char *p1, *p2, *p3;

	options->formatInd = COMPLIANCE;
	options->nBitRate = IPP_SPCHBR_4750;
	options->nCodecType = RUN_BOTH;
	options->nDtxState = 0;
	options->nVadModel = 1;

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
			if(options->nCodecType == RUN_DECODE){
				STRNCPY(options->szFileNameBitstream, p2 + 1, par_value_len);
			}else{
				STRNCPY(options->szFileNameInputPCM, p2 + 1, par_value_len);
			}
        } else if ((0 == IPP_Strcmp(par_name, "o")) || (0 == IPP_Strcmp(par_name, "O"))) {
            /*output file*/
			if(options->nCodecType == RUN_DECODE){
				STRNCPY(options->szFileNameOutputPCM, p2 + 1, par_value_len);
			}else{
				STRNCPY(options->szFileNameBitstream, p2 + 1, par_value_len);
			}
        } else if ((0 == IPP_Strcmp(par_name, "i2")) || (0 == IPP_Strcmp(par_name, "I2"))) {
            /*mode file*/
			STRNCPY(options->szFileNameMode, p2 + 1, par_value_len);
        } else if ((0 == IPP_Strcmp(par_name, "l")) || (0 == IPP_Strcmp(par_name, "L"))) {
            /*log file*/
			STRNCPY(options->szFileNameLog, p2 + 1, par_value_len);
         } else if ((0 == IPP_Strcmp(par_name, "d")) || (0 == IPP_Strcmp(par_name, "D"))) {
			 if (0 == IPP_Strncmp(p2+1,"on",2)){
				 options->nDtxState = 1;
			 }else if (0 == IPP_Strncmp(p2+1,"off",3)){
				 options->nDtxState = 0;
			 }else{
				 IPP_Log(options->szFileNameLog,"a","dtx state not valid, set to default[off]\n");
				options->nDtxState = 0;
			 }
		}else if ((0 == IPP_Strcmp(par_name, "v")) || (0 == IPP_Strcmp(par_name, "V"))) {
			if ( 0 == IPP_Strncmp(p2+1,"vad1",4)) {
				options->nVadModel = 1;
			}else if ( 0 == IPP_Strncmp(p2+1,"vad2",4)) {
				options->nVadModel = 2;
			}else {
				IPP_Log(options->szFileNameLog,"a","vad state not valid, set to default[vad1]\n");
				options->nVadModel = 1;
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
			tmp = IPP_Atoi(p2+1);
			switch( tmp ) {
			case 122:
				options->nBitRate = IPP_SPCHBR_12200;
				break;
			case 102:
				options->nBitRate = IPP_SPCHBR_10200;
				break;
			case 795:
				options->nBitRate = IPP_SPCHBR_7950;
				break;
			case 74:
				options->nBitRate = IPP_SPCHBR_7400;
				break;
			case 67:
				options->nBitRate = IPP_SPCHBR_6700;
				break;
			case 59:
				options->nBitRate = IPP_SPCHBR_5900;
				break;
			case 515:
				options->nBitRate = IPP_SPCHBR_5150;
				break;
			case 475:
				options->nBitRate = IPP_SPCHBR_4750;
				break;
			default:
				options->nBitRate = IPP_SPCHBR_4750;
				break;
			}
         } else if ((0 == IPP_Strcmp(par_name, "f")) || (0 == IPP_Strcmp(par_name, "F"))) {
			 if ( !IPP_Strncmp((p2+1),"IF1",3)) {
				 options->formatInd = IF1;
			 } else if (!IPP_Strncmp((p2+1),"IF2",3)) {
				 options->formatInd = IF2;
			 } else if (!IPP_Strncmp((p2+1), "RAWIF1",6)) {
				 options->formatInd = RAWIF1;
			 } else if (!IPP_Strncmp((p2+1), "COMPLIANCE",10)) {
				 options->formatInd = COMPLIANCE;
			 }else{
				 options->formatInd = COMPLIANCE;
			 }
         } 
        pCur = p3;
    }
    return IPP_OK;
}


/*********************************************************************************************
// Name:			 Usage
//
// Description:		 Display Usage
//
**********************************************************************************************/
void Usage()
{
	IPP_Printf("\n\n\n");
	IPP_Printf("GSMAMR codec example v0.0.2\n");
	IPP_Printf("Description: perform GSMAMR encode/decode on 8kHz 16bits PCM files\n"); 
	IPP_Printf("             encoded stream file format complies GSMAMR standard\n");
	IPP_Printf("Usage: gmrexample [option]\n");
	IPP_Printf("Options:\n");
	IPP_Printf("-t <both|encode|decode>                      select do encode,decode or both\n");
	IPP_Printf("-i <input pcm file>                                  source PCM file\n");
	IPP_Printf("-o <output pcm file>                              encoded/decoded PCM file\n");
//	IPP_Printf("-c <stream file>                                     i/o encoded stream file, with ETSI standard format\n");
	IPP_Printf("-d <on|off>                                           dtx state\n");
	IPP_Printf("-v <vad1|vad2>                                     vad model\n");
	IPP_Printf("-b <122|102|795|74|67|59|515|475>  compress bit rate\n");
	IPP_Printf("-i2 <bit rate mode file>                           this option will disable -b option\n");
	IPP_Printf("-f <IF1|IF2|RAWIF1|COMPLIANCE>         compressed format bitstream.\n");
}

/*********************************************************************************************
// Name:			 RunCodec
//
// Description:		 run GSMAMR codec with the options, select to run encode/decode/both
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
		IPP_Log(options->szFileNameLog,"a", "options->nCodecType not valid, abort.\n");
		return IPP_FAIL;
	}
	return rt;
}


/*********************************************************************************************
// Name:			 RunBoth
//
// Description:		 run GSMAMR codec with the options
//
**********************************************************************************************/
int RunBoth(CmdLineOptions *options)
{

	/* GSMAMR related structures and buffers */
	void *pDecoderState;		/* Test vector decoder state */
	void *pEncoderState;				/* Test vector encoder state */
	Ipp16s				InputBuffer[IPP_GSMAMR_FRAME_LEN];	  /* One frame input pcm data */
	Ipp16s				OutputBuffer[IPP_GSMAMR_FRAME_LEN];	  /* One frame output pcm data */
	Ipp8u				CompressedSpeech[IPP_GSMAMR_BITSTREAM_LEN_122];  /* One frame codec bit stream buffer
																		  We allocate the largest buffer*/
	Ipp16s				ParamBuffer[IPP_GSMAMR_DECODER_TV_LEN];			 /*GSMAMR compliance test file format buffer*/

	IPP_FILE* in, *out, *cod, *mode;
	int cnt_in, total_in;
	int cnt_out, total_out;
	
	int cnt_cod, total_cod;
	int frame,iResult;

	IppSound speech_in, speech_out;
	IppBitstream bitstream;
	IppGSMAMRFormatConfig config;
	MiscGeneralCallbackTable *pCallbackTable = NULL;

	int nMode=0;
	int rt;

	int nFrameLen;
	
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
			IPP_Log(options->szFileNameLog,"a","cannot open %s, abort.\n",options->szFileNameOutputPCM);
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
			IPP_Log(options->szFileNameLog,"cannot open %s, abort.\n",options->szFileNameMode);
			return IPP_FAIL;
		}
    } 

	if ( miscInitGeneralCallbackTable(&pCallbackTable) != 0 ) {
		IPP_Log(options->szFileNameLog,"a","Cannot initialize CallbackTable,abort.\n");
		return IPP_FAIL; 
	}

	iResult = EncoderInitAlloc_GSMAMR(&pEncoderState,options->nDtxState,options->nVadModel,
		pCallbackTable);	
	if(iResult != 0){
		IPP_Log(options->szFileNameLog,"a","Cannot initialize Encoder,abort.\n");
		return IPP_FAIL; 
	}
	
	iResult = DecoderInitAlloc_GSMAMR(&pDecoderState, pCallbackTable);
	if (iResult != 0){
		IPP_Log(options->szFileNameLog,"a","Cannot initialize Decoder,abort.\n");
		return IPP_FAIL; 
	}

	/*Ted's unpack API does not zero the unused tail, so we do that to fix this*/
	IPP_Memset(ParamBuffer,0,sizeof(Ipp16s)*IPP_GSMAMR_DECODER_TV_LEN); 
	
	speech_in.pSndFrame = InputBuffer;
	speech_out.pSndFrame = OutputBuffer;

	/* Set the config information */
	config.formatInd = options->formatInd;

	if(config.formatInd != TV_COMPLIANCE_FORMAT) {
		bitstream.pBsBuffer = CompressedSpeech;
	} else {
		bitstream.pBsBuffer = (Ipp8u *)ParamBuffer;
	}

	total_in = 0;
	total_out = 0;
	total_cod = 0;
	frame = 0;

	if(config.formatInd == HEADERLESS_IF1_FORMAT && cod != NULL) {
		char buf[6] = {0x23, 0x21, 0x41, 0x4d, 0x52, 0x0a};
		IPP_Fwrite(buf, 1, 6, cod);
	}

	while(!IPP_Feof(in)) {
		cnt_in = IPP_Fread(InputBuffer,sizeof(Ipp16s),IPP_GSMAMR_FRAME_LEN,in);
		if ( cnt_in != IPP_GSMAMR_FRAME_LEN ) {
			if ( cnt_in != 0 ) {
				IPP_Log(options->szFileNameLog,"a", "Input PCM frame length[%d] not equal to %d, abort.\n",cnt_in,IPP_GSMAMR_FRAME_LEN);
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
		}else {
			config.bitRate = options->nBitRate;
		}

		iResult = Encode_GSMAMR(&speech_in, &bitstream, pEncoderState, &config);		
		if (IPP_STATUS_NOERR != iResult){
			IPP_Log(options->szFileNameLog,"a","Encoder failed,abort.\n");
			return IPP_FAIL; 
		}

		if(config.formatInd == HEADERLESS_IF1_FORMAT && cod != NULL) {
			unsigned char head;
			head = (unsigned char)(config.frameType << 3);
			head |= 4;
			cnt_cod = IPP_Fwrite(&head, sizeof(Ipp8u),1,cod);
		}

		if ( cod != NULL ) {
			nFrameLen = bitstream.bsByteLen;
			cnt_cod = IPP_Fwrite(bitstream.pBsBuffer, sizeof(Ipp8u),nFrameLen,cod);

			total_cod += cnt_cod;
		}
		
		iResult = Decode_GSMAMR(&bitstream,&speech_out,pDecoderState, &config);
		if (IPP_STATUS_NOERR != iResult){
			IPP_Log(options->szFileNameLog,"a","Decoder failed,abort.\n");
			return IPP_FAIL; 
		}
		
		if ( out != NULL) {
			cnt_out = IPP_Fwrite(OutputBuffer,sizeof(Ipp16s),IPP_GSMAMR_FRAME_LEN,out);
			total_out += cnt_out;
		}
		
		total_in += cnt_in;
		frame ++;
		if ( frame%100==0 ) IPP_Printf("Current running frame %d ......\r", frame);
	}
	
	EncoderFree_GSMAMR(&pEncoderState, pCallbackTable);
	DecoderFree_GSMAMR(&pDecoderState, pCallbackTable);
	miscFreeGeneralCallbackTable(&pCallbackTable);	

	if ( cod ) 
		IPP_Fclose(cod);
	if ( out ) 
		IPP_Fclose(out);
	if ( in ) 
		IPP_Fclose(in);

	return IPP_OK;
}


/*********************************************************************************************
// Name:			 RunEncoder
//
// Description:		 run GSMAMR encoder with the options
//
**********************************************************************************************/
int RunEncoder(CmdLineOptions *options)
{
	/* GSMAMR related structures and buffers */
	void *pEncoderState;			/* Test vector encoder state */
	Ipp16s				InputBuffer[IPP_GSMAMR_FRAME_LEN];	  /* One frame input pcm data */
	Ipp8u				CompressedSpeech[IPP_GSMAMR_BITSTREAM_LEN_122];  /* One frame codec bit stream buffer
																		  We allocate the largest buffer*/
	Ipp16s				ParamBuffer[IPP_GSMAMR_DECODER_TV_LEN];			 /*GSMAMR compliance test file format buffer*/

	IPP_FILE* in, *cod, *mode;
	int cnt_in, total_in;
	int cnt_cod, total_cod;
	int frame,iResult;

	IppSound speech_in;
	IppBitstream bitstream;
	IppGSMAMRFormatConfig config;
	MiscGeneralCallbackTable *pCallbackTable = NULL;	

	int perf_index;
	long long TotTime;
		
	int nMode=0;
	int rt;

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

	/* Set the config information */
	config.formatInd = options->formatInd;
	
	if ( miscInitGeneralCallbackTable(&pCallbackTable) != 0 ) {
		IPP_Log(options->szFileNameLog,"a","Cannot initialize CallbackTable,abort.\n");
		return IPP_FAIL; 
	}

	iResult = EncoderInitAlloc_GSMAMR(&pEncoderState,options->nDtxState,options->nVadModel,
		pCallbackTable);	
	if(iResult != 0){
		IPP_Log(options->szFileNameLog,"a","Cannot initialize Encoder,abort.\n");
		return IPP_FAIL; 
	}

	/*Ted's unpack API does not zero the unused tail, so we do that to fix this*/
	IPP_Memset(ParamBuffer,0,sizeof(Ipp16s)*IPP_GSMAMR_DECODER_TV_LEN); 

	speech_in.pSndFrame = InputBuffer;

	if(config.formatInd != TV_COMPLIANCE_FORMAT) {
		bitstream.pBsBuffer = CompressedSpeech;
	} else {
		bitstream.pBsBuffer = (Ipp8u *)ParamBuffer;
	}

	if(config.formatInd == HEADERLESS_IF1_FORMAT && cod != NULL) {
		char buf[6] = {0x23, 0x21, 0x41, 0x4d, 0x52, 0x0a};
		IPP_Fwrite(buf, 1, 6, cod);
	}
	
	total_in = 0;
	total_cod = 0;
	frame = 0;

	/* initialize for performance data collection */
	IPP_GetPerfCounter(&perf_index, DEFAULT_TIMINGFUNC_START, DEFAULT_TIMINGFUNC_STOP);
	IPP_ResetPerfCounter(perf_index);

	while(!IPP_Feof(in)) {
		cnt_in = IPP_Fread(InputBuffer,sizeof(Ipp16s),IPP_GSMAMR_FRAME_LEN,in);
		if ( cnt_in != IPP_GSMAMR_FRAME_LEN ) {
			if ( cnt_in != 0 ) {
				IPP_Log(options->szFileNameLog,"a","Input PCM frame length[%d] not equal to %d, abort.\n",cnt_in,IPP_GSMAMR_FRAME_LEN);
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
		}else {
			config.bitRate = options->nBitRate;
		}

		IPP_StartPerfCounter(perf_index);
		iResult = Encode_GSMAMR(&speech_in, &bitstream, pEncoderState, &config);		
		IPP_StopPerfCounter(perf_index);

		if (IPP_STATUS_NOERR != iResult){
			IPP_Log(options->szFileNameLog,"a","Encoder failed,abort.\n");
			return IPP_FAIL; 
		}

		if(config.formatInd == HEADERLESS_IF1_FORMAT && cod != NULL) {
			unsigned char head;
			head = (unsigned char)(config.frameType << 3);
			head |= 4;
			cnt_cod = IPP_Fwrite(&head, sizeof(Ipp8u),1,cod);
		}

		if ( cod != NULL ) {
			cnt_cod = IPP_Fwrite(bitstream.pBsBuffer,sizeof(Ipp8u),bitstream.bsByteLen,cod);
			total_cod += cnt_cod;
		} 
		
		total_in += cnt_in;
		frame ++;
		if ( frame%100==0 ) IPP_Printf("Current running frame %d ......\r", frame);
	}
	
	IPP_Log(options->szFileNameLog,"a","Finish encoding!\n");
		
	/* log codec performance and exit performance counting */
	TotTime = IPP_GetPerfData(perf_index);
	IPP_FreePerfCounter(perf_index);
	g_Tot_Time[IPP_AUDIO_INDEX] = TotTime;
	g_Frame_Num[IPP_AUDIO_INDEX] = frame;
	
	EncoderFree_GSMAMR(&pEncoderState, pCallbackTable);	
	miscFreeGeneralCallbackTable(&pCallbackTable);	

	if ( cod ) 
		IPP_Fclose(cod);
	if ( in ) 
		IPP_Fclose(in);

	return IPP_OK;
}


/*********************************************************************************************
// Name:			 RunDecoder
//
// Description:		 run GSMAMR decoder with the options
//
**********************************************************************************************/
int RunDecoder(CmdLineOptions *options)
{
	/* GSMAMR related structures and buffers */
	void *pDecoderState;		/* Test vector decoder state */
	Ipp16s				OutputBuffer[IPP_GSMAMR_FRAME_LEN];	  /* One frame output pcm data */
	Ipp8u				CompressedSpeech[IPP_GSMAMR_BITSTREAM_LEN_122];  /* One frame codec bit stream buffer
																		  We allocate the largest buffer*/
	Ipp16s				ParamBuffer[IPP_GSMAMR_DECODER_TV_LEN];			 /*GSMAMR compliance test file format buffer*/

	IPP_FILE *out, *cod;
	int cnt_out, total_out;
	int cnt_cod, total_cod;
	int frame,iResult;

	IppSound speech_out;
	IppBitstream bitstream;
	
	unsigned char uFirstByte;
	int nFrameLen;
	IppGSMAMRFormatConfig config;
	MiscGeneralCallbackTable *pCallbackTable = NULL;	

	int perf_index;
	long long TotTime;
	
	out = cod = NULL;	

	if ( *options->szFileNameBitstream != '\0' ) {
		cod = IPP_Fopen(options->szFileNameBitstream,"rb");
		if ( !cod ) {
			IPP_Fprintf(options->szFileNameLog,"a","cannot open %s, abort.\n",options->szFileNameBitstream);
			return IPP_FAIL;
		}
    } else {
		IPP_Log(options->szFileNameLog,"a","%s isn't valid, abort.\n",options->szFileNameBitstream);
		return IPP_FAIL;
    }

	if ( *options->szFileNameOutputPCM != '\0' ) {
		out = IPP_Fopen(options->szFileNameOutputPCM,"wb");
		if ( !out ) {
			IPP_Log(options->szFileNameLog,"a","cannot open %s, abort.\n",options->szFileNameOutputPCM);
			return IPP_FAIL;
		}
    } else {
		IPP_Log(options->szFileNameLog,"a","%s isn't valid, abort.\n",options->szFileNameOutputPCM);
//		return IPP_FAIL;
    }

	/* Set the config information */
	config.formatInd = options->formatInd;
	
	if ( miscInitGeneralCallbackTable(&pCallbackTable) != 0 ) {
		IPP_Log(options->szFileNameLog,"a","Cannot initialize CallbackTable,abort.\n");
		return IPP_FAIL; 
	}

	iResult = DecoderInitAlloc_GSMAMR(&pDecoderState, pCallbackTable);
	if (iResult != 0){
		IPP_Log(options->szFileNameLog,"a","Cannot initialize Decoder,abort.\n");
		return IPP_FAIL; 
	}

	IPP_Memset(ParamBuffer,0,sizeof(Ipp16s)*IPP_GSMAMR_DECODER_TV_LEN); 
	IPP_Memset(CompressedSpeech,0,sizeof(Ipp8u)*IPP_GSMAMR_BITSTREAM_LEN_122);	

	speech_out.pSndFrame = OutputBuffer;

	if(config.formatInd != TV_COMPLIANCE_FORMAT) {
		bitstream.pBsBuffer = CompressedSpeech;
	} else {
		bitstream.pBsBuffer = (Ipp8u *)ParamBuffer;
	}
	bitstream.bsByteLen = 0;
	bitstream.bsCurBitOffset = 0;
	bitstream.pBsCurByte = bitstream.pBsBuffer;

	if(config.formatInd == HEADERLESS_IF1_FORMAT) {
		IPP_Fread(CompressedSpeech, 1, 6, cod);		/* Skip the first 6 byte */
	}

	total_out = 0;
	total_cod = 0;
	frame = 0;

	/* initialize for performance data collection */
	IPP_GetPerfCounter(&perf_index, DEFAULT_TIMINGFUNC_START, DEFAULT_TIMINGFUNC_STOP);
	IPP_ResetPerfCounter(perf_index);

	while(!IPP_Feof(cod)) {
		if(config.formatInd != TV_COMPLIANCE_FORMAT && 
			config.formatInd != HEADERLESS_IF1_FORMAT) {
			cnt_cod = IPP_Fread(&uFirstByte,sizeof(Ipp8u),1,cod);
			if ( cnt_cod != 1 ) {
				if ( cnt_cod != 0 ) {
					IPP_Log(options->szFileNameLog,"a","Bitstream frame head not exist, abort.\n");
				}
				break;
			}			

			CompressedSpeech[0] = uFirstByte;
			nFrameLen = GetFrameLengthByFirstStreamByte(uFirstByte, &config,options->szFileNameLog);

			if(nFrameLen >= 1) {
				cnt_cod = IPP_Fread(&CompressedSpeech[1],sizeof(Ipp8u),nFrameLen-1,cod);

				if ( cnt_cod != nFrameLen - 1 ) {
					if ( cnt_cod != 0 ) {
						IPP_Log(options->szFileNameLog,"a","Bitstream frame len[%d] not equal to %d, abort.\n",cnt_cod + 1,nFrameLen);
					}
					break;
				}			
			}
			bitstream.bsByteLen = cnt_cod+1;
		} else if(config.formatInd == HEADERLESS_IF1_FORMAT) {
			cnt_cod = IPP_Fread(&uFirstByte,sizeof(Ipp8u),1,cod);
			if ( cnt_cod != 1 ) {
				if ( cnt_cod != 0 ) {
					IPP_Log(options->szFileNameLog,"a","Bitstream frame head not exist, abort.\n");
				}
				break;
			}	
			config.frameType = (uFirstByte >> 3) & 0xf;

			nFrameLen = GetFrameLength(config.frameType,options->szFileNameLog) - 3;

			if(nFrameLen >= 0) {
				cnt_cod = IPP_Fread(&CompressedSpeech[0],sizeof(Ipp8u),nFrameLen,cod);
				if ( cnt_cod != nFrameLen) {
					if ( cnt_cod != 0 ) {
						IPP_Log(options->szFileNameLog,"a","Bitstream frame len[%d] not equal to %d, abort.\n",cnt_cod + 1,nFrameLen);
					}
					break;
				}			
			}
			bitstream.bsByteLen = cnt_cod;
		} else {
			nFrameLen = 250;
			cnt_cod = IPP_Fread(ParamBuffer,sizeof(Ipp16s),nFrameLen,cod);
			if ( cnt_cod != nFrameLen) {
				if ( cnt_cod != 0 ) {
					IPP_Log(options->szFileNameLog,"a","Bitstream frame len[%d] not equal to %d, abort.\n",cnt_cod + 1,nFrameLen);
				}
				break;
			}
			bitstream.bsByteLen = cnt_cod*2;
		}

		bitstream.pBsCurByte = bitstream.pBsBuffer;
		IPP_StartPerfCounter(perf_index);
		iResult = Decode_GSMAMR(&bitstream,&speech_out,pDecoderState, &config);
        IPP_StopPerfCounter(perf_index);

		if (IPP_STATUS_NOERR != iResult){
			IPP_Log(options->szFileNameLog,"a","Decoding frame %d failed,continue.\n");
			//IPP_Log(options->szFileNameLog,"a","Decoder failed,abort.\n");
			//return IPP_FAIL; 
		}
		
		if ( out != NULL) {
			cnt_out = IPP_Fwrite(OutputBuffer,sizeof(Ipp16s),IPP_GSMAMR_FRAME_LEN,out);
			total_out += cnt_out;
		}
		
		total_cod += cnt_cod;
		frame ++;

		if ( frame%100==0 ) IPP_Printf("Current running frame %d ......\r", frame);
	}
	
	IPP_Log(options->szFileNameLog,"a","Finish decoding!\n");
		
	/* log codec performance and exit performance counting */
	TotTime = IPP_GetPerfData(perf_index);
	IPP_FreePerfCounter(perf_index);
	g_Tot_Time[IPP_AUDIO_INDEX] = TotTime;
	g_Frame_Num[IPP_AUDIO_INDEX] = frame;

	DecoderFree_GSMAMR(&pDecoderState, pCallbackTable);
	miscFreeGeneralCallbackTable(&pCallbackTable);	

	if ( cod ) 
		IPP_Fclose(cod);
	if ( out ) 
		IPP_Fclose(out);
	return IPP_OK;
}


/*********************************************************************************************
// Name:			 ReadMode
//
// Description:		 read text based mode file in format "MR122\r\nMR102\r\n..."
//
**********************************************************************************************/
#define	MAX_DIGIT	10
int	ReadMode(IPP_FILE *mode_file, int *mode,char* pLogFile)
{
	char szMode[MAX_DIGIT];
	char ch;
	int state; // state: 0=/r/n, 1= M 2=R, 3=digital follow MR
	int cnt;

	cnt = 0;
	state = 0;
	while(!IPP_Feof(mode_file)) {
		ch = IPP_Fgetc(mode_file);
		if ( state == 0 && ch == 'M' ) {
			state = 1;
			continue;
		}
		if ( state == 1 && ch == 'R' ) {
			state = 2;
			continue;
		}
		if ( state == 2 && isdigit(ch) ) {
			state = 3;
			szMode[cnt] = ch;
			cnt ++;
			continue;
		}
		if ( state == 3 && isdigit(ch) ) {
			szMode[cnt] = ch;
			cnt ++;
			if ( cnt == MAX_DIGIT-1 ) {
				state = 0;
				szMode[cnt] = '\0';
				*mode = SelectMode(IPP_Atoi(szMode),pLogFile);
				return 0;
			}
			continue;
		}
		if ( state == 3 && (ch == '\r' || ch == '\n') ) {
			state = 0;
			szMode[cnt] = '\0';
			*mode = SelectMode(IPP_Atoi(szMode),pLogFile);
			return 0;
		}
	}
	return -1;
}
#undef	MAX_DIGIT



/*********************************************************************************************
// Name:			 SelectMode
//
// Description:		 Translate mode from 122 to IPP_SPCHBR_12200, and etc.
//
**********************************************************************************************/
int SelectMode(int mode,char *pLogFile ) 
{
	switch( mode ) {
	case 122:
		return IPP_SPCHBR_12200;
	case 102:
		return IPP_SPCHBR_10200;
	case 795:
		return IPP_SPCHBR_7950;
	case 74:
		return IPP_SPCHBR_7400;
	case 67:
		return IPP_SPCHBR_6700;
	case 59:
		return IPP_SPCHBR_5900;
	case 515:
		return IPP_SPCHBR_5150;
	case 475:
		return IPP_SPCHBR_4750;
	default:
		IPP_Log(pLogFile,"a","Mode invalid, set to default[IPP_SPCHBR_4750]\n");
		return IPP_SPCHBR_4750;
	}

}



/*********************************************************************************************
// Name:			 Least4BitsReverse
//
// Description:		 Reverse the least significant 4 bits
//
**********************************************************************************************/
int Least4BitsReversed(int val)
{
	int rt;
	rt = (val & 0x01 ) << 3 |
		 ( val & 0x02 ) << 1 |
		 ( val & 0x04 ) >> 1 |
		 ( val & 0x08 ) >> 3;
	return rt;
}

/*********************************************************************************************
// Name:			 GetFrameLength
//
// Description:		 Get the length of a frame in type defined, counted in byte
//
**********************************************************************************************/
int GetFrameLength(int nFrameType,char* pLogFile)
{
	int nFrameLen;
	switch(nFrameType)
	{
		case IPP_SPCHBR_4750:
			nFrameLen = IPP_GSMAMR_BITSTREAM_LEN_475;
			break;
		case IPP_SPCHBR_5150:
			nFrameLen = IPP_GSMAMR_BITSTREAM_LEN_515;
			break;
		case IPP_SPCHBR_5900:
			nFrameLen = IPP_GSMAMR_BITSTREAM_LEN_59;
			break;
		case IPP_SPCHBR_6700:
			nFrameLen = IPP_GSMAMR_BITSTREAM_LEN_67;
			break;
		case IPP_SPCHBR_7400:
			nFrameLen = IPP_GSMAMR_BITSTREAM_LEN_74;
			break;
		case IPP_SPCHBR_7950:
			nFrameLen = IPP_GSMAMR_BITSTREAM_LEN_795;
			break;
		case IPP_SPCHBR_10200:
			nFrameLen = IPP_GSMAMR_BITSTREAM_LEN_102;
			break;
		case IPP_SPCHBR_12200:
			nFrameLen = IPP_GSMAMR_BITSTREAM_LEN_122;
			break;
		case IPP_SPCHBR_DTX:
			nFrameLen = IPP_GSMAMR_BITSTREAM_LEN_DTX;
			break;
		case 15:		/* 15 is the no data type */
			nFrameLen = 1;
			break;
		default:
			IPP_Log(pLogFile,"a","Invalid bitrate in GetFrameLength, abort\n");
			exit(13);
			break;
	}
	return nFrameLen;
}


/*********************************************************************************************
// Name:			 GetFrameLength
//
// Description:		 Get the length of a frame by the first byte in frame of bitstream
//                   The length is counted in byte
**********************************************************************************************/
int GetFrameLengthByFirstStreamByte(unsigned char uFirstStreamByte, IppGSMAMRFormatConfig 
									*pConfig, char*pLogFile)
{
	int nFrameType;
	int nFrameLen = 0;
	int bitNum[9] = {95, 103, 118, 134, 148, 159, 204, 244, 39};

	if(pConfig->formatInd == IF1_FORMAT) {
		nFrameType = ((int)uFirstStreamByte >> 4) & 0xf;
		nFrameLen = GetFrameLength(nFrameType,pLogFile);

	} else if(pConfig->formatInd == IF2_FORMAT) {
		nFrameType = (int)uFirstStreamByte & 0xf;
		if(nFrameType <= 8) {
			nFrameLen = (bitNum[nFrameType] - 4) >> 3;
			nFrameLen ++;
			if((bitNum[nFrameType] - 4) & 0x7) {
				nFrameLen += 1;
			}
		} else {
			nFrameLen = 1;
		}
	} 

	return nFrameLen;
}

/******************************************************************************
// Name:				CodecTest
// Description:			Main entry for GSMAMR codec test
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
				   Usage:appgsmamr.exe \"-t:encode -i:in.wav -o:out.cod -l:log.txt -d:on -v:vad1 -b:122 -f:COMPLIANCE\"	\n \
				    or       appgsmamr.exe \"-t:decode -i:out.cod -o:out.wav -l:log.txt -d:on -v:vad1 -b:122 -f:COMPLIANCE\"	\n \
				    -t:<encode>                                encode only, output (after -o ) will be a packet file\n	\
				    -t:<decode>                                decode only, input must be a packet file\n	\
					-i :<input pcm file>              source PCM file\n \
				   -o:<output pcm file>           encoded/decoded PCM file \n \
			        -l:<log file>                         is the log information file \n			\
					-d:<on|off>                        dtx state\n	\
					-v:<vad1|vad2>                 vad model\n  \
					-b:<122|102|795|74|67|59|515|475>   compress bit rate\n \
					-i2: <bit rate mode file>      this option will disable -b option\n \
					-f: <IF1|IF2|RAWIF1|COMPLIANCE>          compressed format bitstream.\n \
		");
		return IPP_FAIL;
	}
}
/* EOF */
