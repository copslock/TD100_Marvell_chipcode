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

/************
Header files
*************/
#include "codecAC.h"
#include "misc.h"
#include "ippdefs.h"
#include "ippLV.h"

/****************
 True and false 
 constant
****************/ 
#define	FALSE	0		
#define	TRUE	1

/*************************
 Fill buffer result
 ************************/
#define	NO_NEWDATA			2
#define	LESS_NEWDATA		1
#define	MORE_NEWDATA		0
#define MAX_PAR_NAME_LEN    1024
#define MAX_PAR_VALUE_LEN   2048

#define MIN_READ_SIZE		333

/*************
 Prototypes 
**************/
void	DisplayConsoleStartupBanner(void);
void	DisplayLibVersion(void);
int		ShiftAndFillStreamBuf(IPP_FILE *fp, IppBitstream *pBitStream);
int		GetSampleIndex(int samplerate);
int		WritePcmFile(void *pDecoderState, IppSound sound, char *pDstFileName, int pcmFmt, IPP_FILE* fpo[]);

/******************************************************************************
// Name:			 AACDec
//
// Description:		 AAC player sample code
//					 Demonstrates the Marvell Integrated Performance Primitives 
//					 (IPP) AAC API. Compatible with streams generated by the 
//					 following encoder types:
//						1) ISO/IEC 13818-7, "MPEG-2" AAC 
//						** Low Complexity profile and 1, 2-channel modes only
//						2) ISO/IEC 14496-3, "MPEG-4" AAC
//						** Low complexity profile and 1, 2-channel modes only
//						3) ISO/IEC 14496-3, "MPEG-4" AAC
//						** LTP Object, 1 and 2-channels modes only
//				
// Input Arguments:  pSrcFileName	- Input AAC file name
//					 pDstFileName	- Output PCM file name
//					 pLogFileName	- Log file name
//					 format			- Specify the AAC stream format RAW/ADTS/ADIF
//					 channal		- Specify channel number for RAW format
//					 samplerate		- Specify sample rate for RAW format
//					 profile		- Specify decoder capability LC/LTP/HE/HEv2
//					 delay			- Specify if delay 1 frame
//					 pcmFmt			- Specify PCM format
//					 dmMode			- Specify downmix mode for multi-channel
//
// Output Arguments: None			
//									
// Returns:			 None.
//
******************************************************************************/
int AACDec(char* pSrcFileName, char* pDstFileName, char* pLogFileName, int format, int channal, 
		   int samplerate, int profile, int delay, int pcmFmt, int dmMode)
{
	IppBitstream bitStream;						/* AAC decoder input structure */
	IppSound sound;								/* AAC decoder output structure */
	IppAACDecoderConfig decoderConfig;			/* AAC decoding config structure */
	void *pDecoderState = NULL;					/* AAC decoder state structure pointer */
	MiscGeneralCallbackTable   *pCallBackTable = NULL;	/* Misc callback table */

	/* input/output buffer pointer */
	char *pInputBuf;		
	char *pOutBuf;			

	/* input/output file pointer */
	IPP_FILE* fpi;	
	IPP_FILE* fpo[IPP_AACD_MAX_CH_NUM];

	int i, cbID3Size;
	int frameNum = 0;		/* Frame counter */
	int done = FALSE;		/* Main loop boolean status */			
	int lastEnd = FALSE;
	int iResult;
	int outputBufSize = 0;
	int seekOffset = 0;

	/* performance parameters */
	int perf_index;
	long long TotTime;

	/* Startup banner */
	DisplayConsoleStartupBanner();	
	DisplayLibVersion();

	if ( miscInitGeneralCallbackTable(&pCallBackTable) != 0 ) {
		return IPP_FAIL; 
	}

	/* open input AAC file */
	fpi = IPP_Fopen(pSrcFileName, "rb");
	if(fpi == NULL){
		IPP_Log(pLogFileName,"a","can not open input file\n");
		return IPP_FAIL;
	}
	for(i=0; i<IPP_AACD_MAX_CH_NUM; i++) {
		fpo[i] = NULL;
	}
    
    /* USR UPDATE: configure decoder type */
	IPP_Memset(&decoderConfig, 0, sizeof(IppAACDecoderConfig));
	decoderConfig.profileType = profile;
	decoderConfig.streamFmt = format;
	decoderConfig.pcmFmt = pcmFmt;
	if ( AAC_SF_RAW == decoderConfig.streamFmt ) {
		/* following parameters need to be updated only if input stream is raw format */
		decoderConfig.samplingFrequencyIndex = GetSampleIndex(samplerate);
		decoderConfig.channelConfiguration = channal;
	}

	IPP_MemMalloc(&pInputBuf, INPUT_BUF_SIZE_AAC+1, 2);	
	bitStream.pBsBuffer = pInputBuf;
	bitStream.pBsCurByte = bitStream.pBsBuffer;
	bitStream.bsCurBitOffset =0;

	/* Init output structure - output buffer must be 4 byte aligned */
	if((AAC_AOT_HE == decoderConfig.profileType) || (AAC_AOT_HE_PS == decoderConfig.profileType)) {
		outputBufSize = OUTPUT_BUF_SIZE_AAC * 4 * 6;
    } else {
		outputBufSize = OUTPUT_BUF_SIZE_AAC * 2 * 6;
    }
	IPP_MemMalloc(&pOutBuf, outputBufSize + 3, 4);

	sound.pSndFrame = (Ipp16s*)pOutBuf;
	sound.sndLen =0;		

	/* read input file for initialization */
	i = IPP_Fread(bitStream.pBsCurByte, 1, INPUT_BUF_SIZE_AAC, fpi);	
	if(i < MIN_READ_SIZE) {
		IPP_Log(pLogFileName,"a","Pls read more AAC data for intialization!\n");
		if(fpi != NULL) 	IPP_Fclose(fpi);;
		IPP_MemFree(&pInputBuf);
		IPP_MemFree(&pOutBuf);
		return IPP_FAIL;
	}

	/* parse & skip ID3v2 tag */
    cbID3Size = 0;
	if(!IPP_Strncmp(bitStream.pBsCurByte, "ID3", 3))
	{
		cbID3Size = ((int)(bitStream.pBsCurByte[6])<<21) | ((int)(bitStream.pBsCurByte[7])<<14) |
					((int)(bitStream.pBsCurByte[8])<<7)  | ((int)(bitStream.pBsCurByte[9])<<0);
		cbID3Size += 10;
		IPP_Fseek(fpi, cbID3Size, IPP_SEEK_SET);
		/* read input file for initialization */
		i = IPP_Fread(bitStream.pBsCurByte, 1, INPUT_BUF_SIZE_AAC, fpi);	
		if(i < MIN_READ_SIZE) {
			IPP_Log(pLogFileName,"a","Pls read more AAC data for intialization!\n");
			if(fpi != NULL) 	IPP_Fclose(fpi);;
			IPP_MemFree(&pInputBuf);
			IPP_MemFree(&pOutBuf);
			return IPP_FAIL;
		}
	}
	bitStream.bsByteLen = i;

	/*######################################################################	 
	 IPP AAC high-level decoder function #1: Initialize AAC decoder
	########################################################################*/
	iResult = DecoderInitAlloc_AAC(&bitStream, &decoderConfig,pCallBackTable,&pDecoderState);
	if(iResult != IPP_STATUS_INIT_OK) {
		if(fpo[0] == NULL && pDstFileName != NULL && pDstFileName[0] != '\0') {
			fpo[0] = IPP_Fopen(pDstFileName, "wb");
		}
		if (fpo[0]!=NULL)
		{
			IPP_Fclose(fpo[0]);
		}
		switch (iResult) {
			case IPP_STATUS_BADARG_ERR:
                IPP_Log(pLogFileName,"a","Bad decoder configuration arguments\n");			    
				break;
			case IPP_STATUS_NOMEM_ERR:
		        IPP_Log(pLogFileName,"a","Memory allocation failed\n");			    
				break;
			case IPP_STATUS_INIT_ERR:
			    IPP_Log(pLogFileName,"a","The bitstream doesn't have correct format, program exits\n");
				break;
			default:
				break;
		}
		IPP_MemFree(&pInputBuf);
		IPP_MemFree(&pOutBuf);
		DecoderFree_AAC(&pDecoderState);
		miscFreeGeneralCallbackTable(&pCallBackTable);
		if(fpi != NULL) 	IPP_Fclose(fpi);;
		return IPP_FAIL;
	} /* End of checking initialization result */

	/* initialize for performance data collection */
	IPP_GetPerfCounter(&perf_index, DEFAULT_TIMINGFUNC_START, DEFAULT_TIMINGFUNC_STOP);
	IPP_ResetPerfCounter(perf_index);

	/*###################################################################	 
	IPP AAC high-level decoder function #2: Send command to AAC decoder
	#####################################################################*/ 
	DecodeSendCmd_AACAudio(IPPAC_AAC_SET_DOWNMIX_MODE, &dmMode, NULL, pDecoderState);

	/**************************************************************************
	Invoke frame decoder until input file empty
	The following frame decoder outcomes are possible:

		1. IPP_STATUS_FRAME_COMPLETE
									- Sufficient data was present in the buffer 
									  to decode 1 frame, no frame abnormalities 
									  were detected.  Upon return, PCM samples 
									  are present in the PCM output buffer.

		2. IPP_STATUS_BUFFER_UNDERRUN
		3. IPP_STATUS_SYNCNOTFOUND_ERR
									- Insufficient data present in the buffer to 
									  decode 1 frame; application loads additional 
									  stream bytes into the ring buffer and 
									  invokes the decoder once again. 


		4. IPP_STATUS_NOTSUPPORTED	- The bitstream is not supported in this 
									  decoder

		5. IPP_STATUS_FRAME_ERR		- The bitstream has error, and the frame
									  cannot be decoded correctly.
		6. IPP_STATUS_BS_END		- The bitstream goes to the end.
									
	**************************************************************************/
	while (!done)
	{
		IPP_StartPerfCounter(perf_index);
		/*##################################################################
		 IPP AAC high-level decoder function #3: Decode one AAC frame
		####################################################################*/ 
		iResult = Decode_AAC(&bitStream, &sound,pDecoderState);   
		IPP_StopPerfCounter(perf_index);
		
		switch( iResult )
		{
			case IPP_STATUS_FRAME_COMPLETE:	
				frameNum++;
				if(frameNum == 1){
					/* first frame is decoded successfully, open audio device */
					audio_open(sound.sndSampleRate, sound.sndChannelNum);
				}
				audio_render(sound.pSndFrame, sound.sndLen, sound.sndChannelNum);
				if ( delay < frameNum  && pDstFileName != NULL && pDstFileName[0] != '\0') {
					WritePcmFile(pDecoderState, sound, pDstFileName, pcmFmt, fpo);
				}
				break;

			case IPP_STATUS_BUFFER_UNDERRUN:
			case IPP_STATUS_SYNCNOTFOUND_ERR:
				if(lastEnd) {
					done = TRUE;
					break;
				}
				iResult = ShiftAndFillStreamBuf(fpi,&bitStream);
				if ( iResult ) {
					lastEnd = TRUE;
				}
				break;

			case IPP_STATUS_FRAME_ERR:
				IPP_Log(pLogFileName,"a","frame %d skipped.\n",++frameNum);
#ifdef AACDEC_SKIP_ERR_ADTS_FRAME
				/* For ADTS format skip this frame and find the next frame's sync word */
				iResult = DecodeSendCmd_AACAudio(IPPAC_AAC_FINDNEXT_SYNCWORD, &bitStream, &seekOffset, NULL);
				if(iResult == IPP_STATUS_NOERR) {
					bitStream.pBsCurByte += seekOffset;
					bitStream.bsCurBitOffset = 0;
				} else if(iResult == IPP_STATUS_BUFFER_UNDERRUN) {
					iResult = ShiftAndFillStreamBuf(fpi,&bitStream);
					if ( iResult ) {
						done = TRUE;
					}
				}
#endif
				break;
			case IPP_STATUS_NOTSUPPORTED:
				IPP_Log(pLogFileName,"a"," frame %d is not supported\n", ++frameNum);
#ifdef AACDEC_SKIP_ERR_ADTS_FRAME
				/* For ADTS format skip this frame and find the next frame's sync word */
				iResult = DecodeSendCmd_AACAudio(IPPAC_AAC_FINDNEXT_SYNCWORD, &bitStream, &seekOffset, NULL);
				if(iResult == IPP_STATUS_NOERR) {
					bitStream.pBsCurByte += seekOffset;
					bitStream.bsCurBitOffset = 0;
				} else if(iResult == IPP_STATUS_BUFFER_UNDERRUN) {
					iResult = ShiftAndFillStreamBuf(fpi,&bitStream);
					if ( iResult ) {
						done = TRUE;
					}
				}
#else
				done = TRUE;
#endif
				break;
			case IPP_STATUS_NOMEM_ERR:
				IPP_Log(pLogFileName,"a"," Memory allocation failed\n");			    
				done = TRUE;
				break;				
			case IPP_STATUS_BS_END:
				IPP_Log(pLogFileName,"a"," This frame comes to end for the first ID.\n");
				done = TRUE;
				break;
			default:
				done = TRUE;
				break;
		}
	} 	

	if(fpo[0] == NULL  && pDstFileName != NULL && pDstFileName[0] != '\0') {
		fpo[0] = IPP_Fopen(pDstFileName, "wb");
	}

	audio_close();

	/* log codec performance and exit performance counting */
	TotTime = IPP_GetPerfData(perf_index);
	g_Frame_Num[IPP_AUDIO_INDEX] = frameNum;
	g_Tot_Time[IPP_AUDIO_INDEX] = TotTime;
	IPP_FreePerfCounter(perf_index);

	/* close I/O devices */
	if(fpi != NULL) 	IPP_Fclose(fpi);
	for(i=0; i<IPP_AACD_MAX_CH_NUM; i++) {
		if(fpo[i] != NULL)		IPP_Fclose(fpo[i]);
	}

	/* free input & output buffer */
	IPP_MemFree(&pInputBuf);
	IPP_MemFree(&pOutBuf);

	/*###################################################################
	 IPP AAC high-level decoder function #4: Free AAC decoder
	#####################################################################*/ 
	DecoderFree_AAC(&pDecoderState);
	miscFreeGeneralCallbackTable(&pCallBackTable);
	
	return IPP_OK;
} /* End of AACDec() */

int WritePcmFile(void *pDecoderState, IppSound sound, char *pDstFileName, int pcmFmt, IPP_FILE* fpo[]) 
{
	int i;
	int chPos;
	char *pChar = NULL;
	char filename[512];

	if((pcmFmt == IPP_PCM_16B_INTLVD) || (pcmFmt == IPP_PCM_32B_INTLVD)) {
		/* interleave mode, output 1 channel interleaved pcm file */
		if(fpo[0] == NULL) {
			fpo[0] = IPP_Fopen(pDstFileName, "wb");
			if(fpo[0] == NULL) return -1;
		}
		IPP_Fwrite(sound.pSndFrame, 1, sound.sndLen, fpo[0]);
	} else {
		/* non-interleave mode, write each channel to different PCM files */
		pChar = IPP_Strstr(pDstFileName, ".pcm");
		for ( i = 0; i < sound.sndChannelNum; i++ ) {
			IPP_Strncpy(filename, pDstFileName, (pChar - pDstFileName));
			filename[pChar - pDstFileName] = '\0';
			DecodeSendCmd_AACAudio(IPPAC_AAC_GET_CHANNEL_POSITION, &i, &chPos, pDecoderState);
			switch(chPos) {
			case IPP_FRONT_CENTER_CHANNEL:
				IPP_Strcat(filename, "_fc");
				break;
			case IPP_FRONT_LEFT_CHANNEL:
				IPP_Strcat(filename, "_fl");
				break;
			case IPP_FRONT_RIGHT_CHANNEL:
				IPP_Strcat(filename, "_fr");
				break;
			case IPP_REAR_SURROUND_CHANNEL:
				IPP_Strcat(filename, "_rs");
				break;
			case IPP_SIDE_LEFT_CHANNEL:
				IPP_Strcat(filename, "_sl");
				break;
			case IPP_SIDE_RIGHT_CHANNEL:
				IPP_Strcat(filename, "_sr");
				break;
			case IPP_BACK_LEFT_CHANNEL:
				IPP_Strcat(filename, "_bl");
				break;
			case IPP_BACK_RIGHT_CHANNEL:
				IPP_Strcat(filename, "_br");
				break;
			case IPP_FRONT_LFE_CHANNEL:
				IPP_Strcat(filename, "_lfe");
				break;
			default:
				break;
			}
			IPP_Strcat(filename, ".pcm");
			if(fpo[i] == NULL) {
				fpo[i] = IPP_Fopen(filename, "wb");
			}
			IPP_Fwrite(sound.pSndFrame + i * (sound.sndLen/sound.sndChannelNum/sizeof(Ipp16s)), 1,sound.sndLen/sound.sndChannelNum, fpo[i]);
		}
	}
	return 0;
}

/******************************************************************************
// Name:			 ShiftAndFillStreamBuf
//
// Description:		 Bitstream buffer manager.
//					 Loads AAC stream bytes into the bitstream buffer required 
//					 by the IPP AAC API.  Maitains the buffer pointers.
//
// Input Arguments:  fp				- AAC input stream pointer
//					 pBitStream		- Bitstream ring buffer structure pointer
//
// Output Arguments: None				
//
// Returns:			 None
//
******************************************************************************/
int ShiftAndFillStreamBuf(IPP_FILE *fp, IppBitstream *pBitStream)
{

	int i, shiftLength, fillLength;
	int readByte;
	int val;
	Ipp8u *pFillPos, *pShiftPos;

	shiftLength = pBitStream->bsByteLen;
	shiftLength = shiftLength - (pBitStream->pBsCurByte - pBitStream->pBsBuffer);
	pShiftPos = pBitStream->pBsCurByte;

	for(i=0; i<shiftLength; i++) {
		pBitStream->pBsBuffer[i] = pBitStream->pBsCurByte[i];
	}

	pFillPos = &(pBitStream->pBsBuffer[shiftLength]);
	pBitStream->pBsCurByte = pFillPos;
	fillLength = INPUT_BUF_SIZE_AAC - shiftLength;

	readByte = IPP_Fread(pFillPos, 1, fillLength, fp);

	if(readByte < fillLength) {	// End of file 
		if(readByte == 0) {		// Last frame is the end frame 
			val = NO_NEWDATA;
		} else {
			val = LESS_NEWDATA;
		}
	} else {
		val = MORE_NEWDATA;
	}

	pBitStream->bsByteLen = readByte + (pBitStream->pBsCurByte - 
		pBitStream->pBsBuffer);

	pBitStream->pBsCurByte = pBitStream->pBsBuffer;

	return val;

} /* End of ShiftAndFillStreamBuf() */


/******************************************************************************
// Name:			 DisplayConsoleStartupBanner
//
// Description:		 Display startup message on the text console
//
// Input Arguments:  None.
//
// Output Arguments: None				
//
// Returns:			 None				
*******************************************************************************/
void DisplayConsoleStartupBanner()
{
	int i;
	for(i=0;i<2;i++) IPP_Printf("\n");
	IPP_Printf("*****************************************************************");
	IPP_Printf("\nMarvell Integrated Performance Primitives (IPP)\nfor the Marvell \
		XScale* Microprocessor.\n");
	IPP_Printf("AAC Decoder Demonstration, Version 0.2\n");
	IPP_Printf("ISO/IEC 13818-7 MPEG-2 AAC LC profile\n");
	IPP_Printf("ISO/IEC 14496-3	MPEG-4 AAC LC/LTP profile\n");
	IPP_Printf("ISO/IEC 14496-3 Amd 1 HE-AAC profile\n");
	IPP_Printf("ISO/IEC 14496-3 Amd 2 HE-AACv2 profile\n");
	IPP_Printf("Created by Marvell IPP Team\n");
	IPP_Printf("Copyright (c) 2009, Marvell International Ltd.  All Rights Reserved.\n");
	IPP_Printf("*****************************************************************\n\n");
}

/******************************************************************************
// Name:			 DisplayLibVersion
//
// Description:		 Display library build information on the text console
//
// Input Arguments:  None.
//
// Output Arguments: None				
//
// Returns:			 None				
*******************************************************************************/
void DisplayLibVersion()
{
	char libversion[128]={'\0'};
	IppCodecStatus ret;
	ret = GetLibVersion_AACDEC(libversion,sizeof(libversion));
	if(0 == ret){
		IPP_Printf("\n*****************************************************************\n");
	    IPP_Printf("This library is built from %s\n",libversion);
		IPP_Printf("*****************************************************************\n");
	}else{
		IPP_Printf("\n*****************************************************************\n");
		IPP_Printf("Can't find this library version information\n");
		IPP_Printf("*****************************************************************\n");
	}
}

/******************************************************************************
// Name:                ParseCmdTemplate
// Description:         Parse the user command 
//
// Input Arguments:
//      pCmdLine    :   Pointer to the input command line
//
// Output Arguments:
//      pSrcFileName:   Pointer to src file name
//      pDstFileName:   Pointer to dst file name
//      pLogFileName:   Pointer to log file name
//      pParSet     :   Pointer to codec parameter set
// Returns:
//        [Success]     IPP_OK
//        [Failure]     IPP_FAIL
******************************************************************************/
int ParseAACDecCmd(char *pCmdLine, 
					char *pSrcFileName, 
					char *pDstFileName, 
					char *pLogFileName, 
					int* format,
					int* channal,
					int* samplerate,
					int* profile,
					int* delay, 
					int* pcmFmt,
					int* dmMode)
{
    char *pCur, *pEnd;
    char par_name[MAX_PAR_NAME_LEN];
    int  par_name_len;
    int  par_value_len;
    char *p1, *p2, *p3;

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
        if ((0 == IPP_Strcmp(par_name, "i")) || (0 == IPP_Strcmp(par_name, "I"))) {            
			IPP_Strncpy(pSrcFileName, p2 + 1, par_value_len);	/* input file */			
			pSrcFileName[par_value_len] = '\0';
        } else if ((0 == IPP_Strcmp(par_name, "o")) || (0 == IPP_Strcmp(par_name, "O"))) {
			IPP_Strncpy(pDstFileName, p2 + 1, par_value_len);	/* output file */			 
			pDstFileName[par_value_len] = '\0';
        } else if ((0 == IPP_Strcmp(par_name, "l")) || (0 == IPP_Strcmp(par_name, "L"))) {
            IPP_Strncpy(pLogFileName, p2 + 1, par_value_len);	/* log file */
			pLogFileName[par_value_len] = '\0';
        } else if ((0 == IPP_Strcmp(par_name, "p")) || (0 == IPP_Strcmp(par_name, "P"))) {
            *profile = IPP_Atoi(p2 + 1);		/* profile type */
         } else if ((0 == IPP_Strcmp(par_name, "f")) || (0 == IPP_Strcmp(par_name, "F"))) {
            *format = IPP_Atoi(p2 + 1);			/* AAC stream format */
        } else if ((0 == IPP_Strcmp(par_name, "n")) || (0 == IPP_Strcmp(par_name, "N"))) {
            *channal = IPP_Atoi(p2 + 1);		/* channel number for raw format */
        } else if ((0 == IPP_Strcmp(par_name, "r")) || (0 == IPP_Strcmp(par_name, "R"))) {
            *samplerate = IPP_Atoi(p2 + 1);		/* sample rate for raw format */
        } else if ((0 == IPP_Strcmp(par_name, "s")) || (0 == IPP_Strcmp(par_name, "S"))) {
            *delay = IPP_Atoi(p2 + 1);			/* output delay in frame */
        } else if ((0 == IPP_Strcmp(par_name, "m")) || (0 == IPP_Strcmp(par_name, "M"))) {
            *pcmFmt = IPP_Atoi(p2 + 1);			/* output pcm format */
        } else if ((0 == IPP_Strcmp(par_name, "d")) || (0 == IPP_Strcmp(par_name, "D"))) {
            *dmMode = IPP_Atoi(p2 + 1);			/* downmix mode for multichannel */
		}
        pCur = p3;
    }

    return IPP_OK;
}


int GetSampleIndex(int samplerate)
{
	int pSampleRateMapTbl[12] = {96000,	88200,	64000,	48000,	44100, 32000,	24000,	22050,	16000,	12000, 11025,	8000};
	int i;
	for(i=0; i<12; i++) {
		if(samplerate == pSampleRateMapTbl[i]) {
			return i;
		}
	}
	return 0;
}

/******************************************************************************
// Name:			 CodecTest
//
// Description:		 Main entry for AAC decoder test
//
//		Usage:		 appAACDec.exe "-i:test.aac -o:test.pcm -l:test.log -f:format 
//					 -c: channal -s: samplerate\" 
//				
// Input Arguments:  argc				- Standard C argument count
//					 argv				- Standard C argument list
//
// Output Arguments: None				- Pointer to the PCM audio output 
//										  buffer
//
// Returns:			 None.
//
******************************************************************************/
int CodecTest(int argc, char** argv)
{
    char pSrcFileName[256];
    char pDstFileName[256];
    char pLogFileName[256];
	int		format = AAC_SF_MP4ADTS;
	int		channal = 1;
	int		samplerate = 16000;
	int		profile = AAC_AOT_HE;
	int		delay = 0;
	int		pcmFmt = IPP_PCM_16B_INTLVD;
	int		dmMode = AAC_NO_DOWNMIX;

	IPP_Memset(pSrcFileName, 0x0, 256);
	IPP_Memset(pDstFileName, 0x0, 256);
	IPP_Memset(pLogFileName, 0x0, 256);
   
    if(argc == 2 && ParseAACDecCmd(argv[1], pSrcFileName, pDstFileName, pLogFileName, &format, &channal, &samplerate, &profile, &delay, &pcmFmt, &dmMode) == 0){
           return(AACDec(pSrcFileName, pDstFileName,pLogFileName, format, channal, samplerate, profile, delay, pcmFmt, dmMode));
    }else{
        IPP_Printf("Command is incorrect! \n \
		Usage:appAACDec.exe \"-i:test.aac -o:test.pcm -l:test.log [options]\"\n\
		-i input AAC(ADTS/ADIF/RAW) file \n \
		-o output PCM file \n \
		-l log file \n \
		-p profile (default is HE_PS, 2 is LC, 5 is HEAAC) \n \
		-f format (defatult is MP4_ADTS, 6 is RAW_FORMAT) \n \
		-n channal, affect only in raw data format (max 6 channels) \n \
		-r sample rate, affect only in raw data format  \n \
		-s delay frame, the first frame of output will be ignored if it's set  \n \
		-m pcm format, default is 16 bit interleave mode  \n \
		-d downmix mode, affect only for multichannel decoding"); 
		return IPP_FAIL;
    }
}


/* EOF */

