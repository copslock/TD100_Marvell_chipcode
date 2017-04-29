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
/* Standard IPP headers */
#include "misc.h"
#include "codecWMA.h"
#include "asfparser.h"

#include "ippLV.h"

/****************
 True and false 
 constant
****************/ 
#define	FALSE			0	
#define	TRUE			1
#define MAX_PAR_NAME_LEN    1024
#define MAX_PAR_VALUE_LEN   2048

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
	ret = GetLibVersion_WMADEC(libversion,sizeof(libversion));
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

int WMADec(char* pSrcFileName, char* pDstFileName, char* pLogFileName, int decoderflag)
{
	IppBitstream bitStream;				/* wma bit stream */
	IppSound sound;
	IppWMADecoderConfig decoderConfig;
												
	//FILE *fplog;
	IPP_FILE *fpi;
	IPP_FILE *fpo;						/* Input and output file pointers */
	int frameNum = 0;					/* Frame counter */	
	int dataSize = 0;
	int iResult;
	char *pOutBuf;						// output buffer 
	void *pDecoderState = NULL;			// pointer to decoder state structure
	int i=0, done=0;
	int iMaxFrameSize, iPackets=0;
	MiscGeneralCallbackTable   *pCallBackTable = NULL;
	int perf_index;
	long long TotTime;


	DisplayLibVersion();

	IPP_Memset(&decoderConfig, 0x00, sizeof(decoderConfig));

	if ( miscInitGeneralCallbackTable(&pCallBackTable) != 0 ) 
	{
		return IPP_FAIL; 
	}

	fpi = IPP_Fopen(pSrcFileName, "rb");
	if(!fpi){
		IPP_Log(pLogFileName,"a","input file is NULL!");
		miscFreeGeneralCallbackTable(&pCallBackTable);
		return IPP_FAIL;
	}
	
	fpo = IPP_Fopen(pDstFileName,"wb");

	if(fpo == NULL ){
			IPP_Log(pLogFileName,"a","output file is NULL!");
	}	
	
	iResult = ParseASFHeader(fpi, &decoderConfig);
    if(iResult != 0)
	{
		if(fpi != NULL) 	IPP_Fclose(fpi);
		if(fpo != NULL) 	IPP_Fclose(fpo);
		miscFreeGeneralCallbackTable(&pCallBackTable);
		
		return IPP_FAIL;
	}

	/* Set additional user config.*/
	/*		  
    // downmix to two channels
	decoderConfig.iDecoderFlags |= IPP_DECOPT_CHANNEL_DOWNMIXING;
	// resample to 16 bits, -pcm 16 
    decoderConfig.iDecoderFlags |= IPP_DECOPT_REQUANTTO16;
	// downsample to 44.1 or 48 kHZ 
    decoderConfig.iDecoderFlags |= IPP_DECOPT_DOWNSAMPLETO44OR48;
	*/
	decoderConfig.iDecoderFlags = decoderflag;

	/* Initialize the decoder state */
	iResult = DecoderInitAlloc_WMA(&bitStream,&decoderConfig,pCallBackTable, 
		&iMaxFrameSize, &pDecoderState);

	if(iResult != IPP_STATUS_INIT_OK)
	{
		if(fpi != NULL) 	IPP_Fclose(fpi);
		if(fpo != NULL) 	IPP_Fclose(fpo);
		DecoderFree_WMA(&pDecoderState);
		miscFreeGeneralCallbackTable(&pCallBackTable);
		
		IPP_Log(pLogFileName,"a","Decoder initialization failed!\n");
		return IPP_FAIL;
	}
#ifndef RAWPCM
	iResult =IPP_WriteWaveHeaders(fpo, &decoderConfig);
	if(iResult != IPP_OK)
	{
		IPP_Log(pLogFileName,"a","Wave file write failed!\n");
	}
#endif //RAWPCM
	/* Assign memory for input buffer by user */ 
	if(decoderConfig.wFormatTag == WAVE_FORMAT_WMSP1)
		iResult = IPP_MemMalloc(&bitStream.pBsBuffer,decoderConfig.dwPacketSize, 4);
	else
		iResult = IPP_MemMalloc(&bitStream.pBsBuffer,decoderConfig.nBlockAlign, 4);
	if(iResult != 0 ){
			IPP_Log(pLogFileName,"a","Input buffer allocation failed!\n");
	}

	/* Assign memory for output buffer by user */
	iResult = IPP_MemMalloc(&pOutBuf, iMaxFrameSize, 8);
	if(iResult != 0 ){
			IPP_Log(pLogFileName,"a","Output buffer allocation failed!\n");
	}
	sound.pSndFrame =(Ipp16s *) pOutBuf;
	

	/* Initialize output buffer by user */
	sound.sndLen =0;
		
	/* Initialize input buffer by user */
	bitStream.pBsCurByte = bitStream.pBsBuffer;
	bitStream.bsCurBitOffset =0;
	bitStream.bsByteLen = decoderConfig.nBlockAlign;

	/* Parse the data object header */
	iResult = ParseDataObjectHead(fpi, &iPackets);
	if(iResult != IPP_OK)
	{
		IPP_Log(pLogFileName,"a","ParseDataObjectHead failed!\n");
	}

	if(decoderConfig.wFormatTag == WAVE_FORMAT_WMSP1)
		iPackets *=2;

	/* read the first payload */
	iResult = ParseNextPacketAndGetPayload(fpi,&decoderConfig,&bitStream);
	if( IPP_OK != iResult )
        {
		if(fpi != NULL) 	IPP_Fclose(fpi);
		if(fpo != NULL) 	IPP_Fclose(fpo);
		IPP_MemFree(&(bitStream.pBsBuffer));
		IPP_MemFree(&pOutBuf);
		DecoderFree_WMA(&pDecoderState);
		miscFreeGeneralCallbackTable(&pCallBackTable);
		
		IPP_Log(pLogFileName,"a","WMA Payload Parser failed!\n");
		return IPP_FAIL;
        }

	/* initialize for performance data collection */
	IPP_GetPerfCounter(&perf_index, DEFAULT_TIMINGFUNC_START, DEFAULT_TIMINGFUNC_STOP);
	IPP_ResetPerfCounter(perf_index);

	while (i<=iPackets && !done)
	{
	/* Note: If the output PCM format is 24 bit, sound->pSndFrame
		would be the starting address of the output buffer. The user should 
		typecast the pointer type from Ipp16s* to the desired type, such as Ipp8u*. */
		IPP_StartPerfCounter(perf_index);
		iResult = Decode_WMA(&bitStream, &sound, pDecoderState);
		IPP_StopPerfCounter(perf_index);

		switch( iResult )
		{
		case IPP_STATUS_FRAME_COMPLETE:
			frameNum++;
			if(frameNum == 1){
				audio_open(decoderConfig.nSamplesPerSec, decoderConfig.nChannels);
			}
			audio_render(sound.pSndFrame, sound.sndLen, decoderConfig.nChannels);
			if (fpo!=NULL) {
				IPP_Fwrite(sound.pSndFrame, 1,sound.sndLen, fpo);
				dataSize += sound.sndLen;
			}
			break;
			
		case IPP_STATUS_BUFFER_UNDERRUN:
			i++;
			if(i<iPackets)
			{
				iResult = ParseNextPacketAndGetPayload(fpi,&decoderConfig,&bitStream);
                		if( IPP_OK != iResult )
                		{
                       			done =1;
                        		break;
                		}
			}
			else
			{
				bitStream.bsByteLen = 0;
				bitStream.pBsCurByte = bitStream.pBsBuffer;
				bitStream.bsCurBitOffset = 0;
			}
			
			break;
			
		case IPP_STATUS_FRAME_ERR:
			IPP_Log(pLogFileName,"a","frame %d skipped!\n", frameNum); 
			/* Skip this frame or other process here */
			break;
		case IPP_STATUS_NOTSUPPORTED:
			done =1;
			IPP_Log(pLogFileName,"a","The bitstream is not supported, programe terminate!");
			break;
		case IPP_STATUS_BS_END:
			done = 1;
			IPP_Log(pLogFileName,"a","Bitstream comes to the end!\n");
			break;
		default:
			done = 1;
			IPP_Log(pLogFileName,"a","unknown error encountered, and exit.\n");
			break;
		}
	}
	
	IPP_Log(pLogFileName,"a","Finish decoding!\n");

	/* log codec performance and exit performance counting */
	TotTime = IPP_GetPerfData(perf_index);
	
	IPP_FreePerfCounter(perf_index);

	audio_close();

	printf("decode cost %llds\n", TotTime);
#ifndef RAWPCM
	iResult = IPP_UpdateWaveHeaders(fpo, decoderConfig.cbSize, dataSize);
	if(iResult != IPP_OK)
	{
		IPP_Log(pLogFileName,"a","IPP_UpdateWaveHeaders failed!\n");
	}
#endif //RAWPCM

	g_Frame_Num[IPP_AUDIO_INDEX] = frameNum;
	g_Tot_Time[IPP_AUDIO_INDEX] = TotTime;
	/* Close I/O devices */	
	if(fpi != NULL) 	IPP_Fclose(fpi);
	if(fpo != NULL) 	IPP_Fclose(fpo);
	IPP_MemFree(&(bitStream.pBsBuffer));
	IPP_MemFree(&pOutBuf);
	DecoderFree_WMA(&pDecoderState);
	miscFreeGeneralCallbackTable(&pCallBackTable);
	
	//fclose(fplog);
	return IPP_OK;
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
int ParseWMADecCmd(  char *pCmdLine, 
                     char *pSrcFileName, 
                     char *pDstFileName, 
                     char *pLogFileName,
					 int  *decoderflag)
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
            /*input file*/
            IPP_Strncpy(pSrcFileName, p2 + 1, par_value_len);
			  pSrcFileName[par_value_len] = '\0';
        } else if ((0 == IPP_Strcmp(par_name, "o")) || (0 == IPP_Strcmp(par_name, "O"))) {
            /*output file*/
            IPP_Strncpy(pDstFileName, p2 + 1, par_value_len);
			 pDstFileName[par_value_len] = '\0';
        } else if ((0 == IPP_Strcmp(par_name, "l")) || (0 == IPP_Strcmp(par_name, "L"))) {
            /*log file*/
            IPP_Strncpy(pLogFileName, p2 + 1, par_value_len);
			  pLogFileName[par_value_len] = '\0';
        }else if ((0 == IPP_Strcmp(par_name, "c")) || (0 == IPP_Strcmp(par_name, "C"))) {
            /*decoder flag*/
			p2 = IPP_Strstr(p2+1, "=");
			if (NULL == p2) continue;

			*decoderflag = IPP_Atoi(p2+1);
        }else if ((0 == IPP_Strcmp(par_name, "p")) || (0 == IPP_Strcmp(par_name, "P"))) {
            /*par file*/
            /*parse par file to fill pParSet*/
		} 

        pCur = p3;
    }

    return IPP_OK;
}

/******************************************************************************
// Name:                CodecTest
// Description:         Main entry for AAC decoder test
//
// Input Arguments:
//                      N/A
// Output Arguments:
//                      N/A
// Returns:
******************************************************************************/
int CodecTest(int argc, char** argv)
{
    char pSrcFileName[256];
    char pDstFileName[256];
    char pLogFileName[256];
	int  decoderflag=0;
	
	IPP_Memset(pSrcFileName, 0x0, 256);
	IPP_Memset(pDstFileName, 0x0, 256);
	IPP_Memset(pLogFileName, 0x0, 256);
   
    if(argc == 2 && ParseWMADecCmd(argv[1], pSrcFileName, pDstFileName, pLogFileName, &decoderflag) == 0){
           return(WMADec(pSrcFileName, pDstFileName,pLogFileName, decoderflag));
    }else{
        IPP_Printf("Command is incorrect! \n \
		Usage:appWMADec.exe \"-i:test.aac -o:test.wav -l:test.log \"\n\
		-i input file \n \
		-o output file \n \
		-l log file \n \
		-c flag=7,3,1 or 0 \n "); 
		return IPP_FAIL;
	}
    
}
/* EOF */
