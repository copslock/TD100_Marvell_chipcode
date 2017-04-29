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


#include "misc.h"
#include "vguser.h"
#include "codecVC.h"

#include "ippLV.h"

/*Parsing Cmd line for MPEG4 Encoder.*/
#define MAX_PAR_NAME_LEN    1024
#define MAX_PAR_VALUE_LEN   2048
#define STRNCPY(dst, src, len) \
{\
    IPP_Strncpy((dst), (src), (len));\
    (dst)[(len)] = '\0';\
}

static char scHelpMpeg4Dec[] = {
    "Marvell MPEG4Dec Usage: \n"
    "     IPP Unit Test    :  ./App Input.cmp Output.yuv\n"
    "     Integration Test :  ./App \"-i:input.cmp -o:output.yuv -l:logfile\" \n"
};

int InsertSyncCode_MPEGDec(IppBitstream *pBs, int flag/*h263 (1) or mpeg4 (0)*/)
{
    Ipp8u *pTmp = pBs->pBsBuffer + pBs->bsByteLen - 4;

    if(flag){
        /*H263 Start Code, 0000 0000 0000 0000 1 00000*/
        *pTmp ++ = 0x00; *pTmp ++ = 0x00;*pTmp ++ = 0x80;
        
    }else {
        /*MPEG4 Start Code*/
        /*check sync code for MPEG4, and insert one sync code*/
        *pTmp ++ = 0x00; *pTmp ++ = 00;*pTmp ++ = 0x01;
    }
    return 0;
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
	ret = GetLibVersion_MPEG4DEC(libversion,sizeof(libversion));
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
int ParseCmd_MPEGDec(char *pCmdLine,    char *pSrcFileName, 
                     char *pDstFileName, char *pLogFileName
                     )
{
    char *pCur, *pEnd;
    char par_name[MAX_PAR_NAME_LEN];
    char *p1 = NULL, *p2 = NULL, *p3 = NULL;

    int  par_name_len         = 0;
    int  par_value_len        = 0;
  
    IppCodecStatus   ret      = IPP_STATUS_NOERR;
    char cTmp[64]             = {"\0"};


    pCur = pCmdLine;
    pEnd = pCmdLine + IPP_Strlen(pCmdLine);

    while((p1 = IPP_Strstr(pCur, "-"))){
        p2 = IPP_Strstr(p1, ":");
        if (NULL == p2) {
            return IPP_STATUS_ERR;
        }
        p3 = IPP_Strstr(p2, " "); /*one space*/
        if (NULL == p3) {
            p3 = pEnd;
        }

        par_name_len    = p2 - p1 - 1;
        par_value_len   = p3 - p2 - 1;

        if ((0 >= par_name_len)  || (MAX_PAR_NAME_LEN <= par_name_len) ||
            (0 >= par_value_len) || (MAX_PAR_VALUE_LEN <= par_value_len)) {
                return IPP_STATUS_ERR;
        }

        STRNCPY(par_name, p1 + 1, par_name_len);
        if ((0 == IPP_Strcmp(par_name, "i")) || (0 == IPP_Strcmp(par_name, "I"))) {
            /*input file*/
            STRNCPY(pSrcFileName, p2 + 1, par_value_len);
        } else if ((0 == IPP_Strcmp(par_name, "o")) || (0 == IPP_Strcmp(par_name, "O"))) {
            /*output file*/
            STRNCPY(pDstFileName, p2 + 1, par_value_len);
        } else if ((0 == IPP_Strcmp(par_name, "l")) || (0 == IPP_Strcmp(par_name, "L"))) {
            /*log file*/
            STRNCPY(pLogFileName, p2 + 1, par_value_len);
        } else {
            /*adding no such  option*/
            STRNCPY(cTmp, p2 + 1, par_value_len);
            IPP_Printf("No Such Optional: %s:::%s, Skipped\n", par_name, cTmp);
            //return IPP_STATUS_ERR;
        }
        
        pCur = p3;
    }

    return ret;
}

/***************************************************************************
// Name:             MPEG4Coredecoder
// Description:      Decoding entry for MPEG4 BVOP decoder
//
// Input Arguments:
//      inputCmpFile  Pointer to input file stream buffer
//		outYUVFile	  Pointer to output YUV file stream buffer.
//		iWidth		  Decoding picture width,  only for shape mode
//		iHeight		  Decoding picture height, only for shape mode
//
// Output Arguments:
//		pFrameNumber  Overall decoded frame numbers
//
// Returns:         
//		IPP Codec Status Code
*****************************************************************************/
int MPEG4Coredecoder(FILE *inputCmpFile, FILE * outYUVFile, int iWidth,
					 int iHeight, unsigned long *pFrameNumber, char *logfile, int para)
{
	void *pmp4DecState  = NULL;
	void *ph263DecState = NULL;
	MiscGeneralCallbackTable   *pCallBackTable = NULL;
	IppBitstream	streamBuffer;	
	IppPicture		Pic;
	IppPosition		NextSyncCode;
	int				NSCCheckDisable = 1;
	int				bEnd = 0;					/* flag for end of decode */
	int				bLastOutputFrame = 0;	
	IppCodecStatus	rc = IPP_STATUS_NOERR, rc1, ret = IPP_STATUS_NOERR;	/* return value */
    Ipp8u           u8Flv = 0;

    /*Performance Profiling*/
    int             perf_index       = 0; 
    long long       llTotaltime      = 0;
    DISPLAY_CB      hDispCB          ;
    Ipp32u          u32SkipMode      = IPPVC_SKIPMODE_0;

	memset (&streamBuffer, 0, sizeof(IppBitstream) );
	memset (&Pic, 0, sizeof(IppPicture) );

	if ( miscInitGeneralCallbackTable(&pCallBackTable) != 0 ) {
        IPP_Log(logfile,"a", "Error: miscInitGeneralCallbackTable \n");
		return IPP_STATUS_NOMEM_ERR; 
	}	
	
	/* initialize data buffer and load data */
	if ( videoInitBuffer (&streamBuffer) != IPP_STATUS_NOERR) {	
        IPP_Log(logfile,"a", "Error: videoInitBuffer \n");
		return IPP_STATUS_NOMEM_ERR;
	}

    if(videoReloadBuffer(&streamBuffer, inputCmpFile) != IPP_STATUS_NOERR){
        IPP_Log(logfile,"a", "Error: videoReloadBuffer \n");
        videoFreeBuffer(&streamBuffer);
        return IPP_STATUS_NOMEM_ERR;
    }
	
	/* ensure VO & VOL header has been completely loaded in the stream buffer */
	rc1 = DecoderInitAlloc_MPEG4Video(&streamBuffer, &iWidth, &iHeight,
		pCallBackTable, &pmp4DecState);
	
	if ( rc1 != IPP_STATUS_NOERR && rc1 != IPP_STATUS_MP4_SHORTHEAD) {
		/* IPP_Free the allocated memory */	
        IPP_Log(logfile,"a", "Error: DecoderInitAlloc_MPEG4Video \n");
		ret = IPP_STATUS_ERR;
		goto  EndOfMpeg4Dec;
	}

    /*Performance and Display */
    IPP_GetPerfCounter(&perf_index, DEFAULT_TIMINGFUNC_START, DEFAULT_TIMINGFUNC_STOP);
    IPP_ResetPerfCounter(perf_index);

    /*Init Display for Manchac*/
    if(IPP_STATUS_NOERR != display_open(&hDispCB, iWidth, iHeight)){
        IPP_Log(logfile,"a", "Error: display_open \n");
        ret = IPP_STATUS_ERR;
		goto  EndOfMpeg4Dec;
    }

	if (IPP_STATUS_MP4_SHORTHEAD == rc1) {
		rc = DecoderInitAlloc_H263Video(&streamBuffer, pCallBackTable,
			&ph263DecState/*,u8Flv*/);
		if ( rc != IPP_STATUS_NOERR) {
			/* IPP_Free the allocated memory */	
            IPP_Log(logfile,"a", "Error: DecoderInitAlloc_H263Video \n");
		    ret = IPP_STATUS_ERR;
		    goto  EndOfMpeg4Dec;
		}
	}

	/* send command to disable next sync code search inside codec */
	if (IPP_STATUS_MP4_SHORTHEAD == rc1) {
        NSCCheckDisable = 1;
		rc = DecodeSendCmd_H263Video (IPPVC_SET_NSCCHECKDISABLE,
			(&NSCCheckDisable), NULL, ph263DecState);
		if ( rc != IPP_STATUS_NOERR ) {
            IPP_Log(logfile,"a", "Error: DecodeSendCmd_H263Video \n");
		    ret = IPP_STATUS_ERR;
		    goto  EndOfMpeg4Dec;
		}
	} else {
		rc = DecodeSendCmd_MPEG4Video (IPPVC_SET_NSCCHECKDISABLE,
			(&NSCCheckDisable), NULL, pmp4DecState);
		if ( rc != IPP_STATUS_NOERR ) {
            IPP_Log(logfile,"a", "Error: DecodeSendCmd_MPEG4Video \n");
		    ret = IPP_STATUS_ERR;
		    goto  EndOfMpeg4Dec;
		}

        /*Drop B: [0-4]
        0 == [0%]
        1 == [25%]
        2 == [50%]
        3 == [75%]
        4 == [100%]
        */
        u32SkipMode = para;

        /*FrameSkip Mode: [IPPVC_SET_SKIPMODE]*/
        rc = DecodeSendCmd_MPEG4Video (IPPVC_SET_SKIPMODE,
			(&u32SkipMode), NULL, pmp4DecState);
		if ( rc != IPP_STATUS_NOERR ) {
            IPP_Log(logfile,"a", "Error: DecodeSendCmd_MPEG4Video \n");
		    ret = IPP_STATUS_ERR;
		    goto  EndOfMpeg4Dec;
		}
	}

	/* decode frame in loop */
	*pFrameNumber = 0;
	while (!bEnd) {
		/* search sync codes to ensure one VOP has been completely loaded
		// in the stream buffer */
		if (IPP_STATUS_MP4_SHORTHEAD == rc1) {
			rc = DecodeSendCmd_H263Video (IPPVC_SEEK_NEXTSYNCCODE,
				(&streamBuffer), (&NextSyncCode), ph263DecState);
			if ((rc != IPP_STATUS_NOERR) && (rc != IPP_STATUS_SYNCNOTFOUND_ERR)) {
                IPP_Log(logfile,"a", "Error: DecodeSendCmd_H263Video \n");
		        ret = IPP_STATUS_ERR;
		        goto  EndOfMpeg4Dec;
			}
		} else {
			rc = DecodeSendCmd_MPEG4Video (IPPVC_SEEK_NEXTSYNCCODE,
				(&streamBuffer), (&NextSyncCode), pmp4DecState);
			if ((rc != IPP_STATUS_NOERR) && (rc != IPP_STATUS_SYNCNOTFOUND_ERR)) {
                IPP_Log(logfile,"a", "Error: DecodeSendCmd_MPEG4Video \n");
		        ret = IPP_STATUS_ERR;
		        goto  EndOfMpeg4Dec;
			}
		}

		/* decode frame */
		if (IPP_STATUS_SYNCNOTFOUND_ERR != rc) {

            IPP_StartPerfCounter(perf_index);

			if (IPP_STATUS_MP4_SHORTHEAD == rc1) {
				rc = Decode_H263Video(&streamBuffer, &Pic, ph263DecState);
			} else {
				rc = Decode_MPEG4Video(&streamBuffer, &Pic, pmp4DecState,
					bLastOutputFrame);
			} 

            IPP_StopPerfCounter(perf_index);
			/* if decoded successfully */
			if (rc == IPP_STATUS_NOERR) {
                //IPP_Printf("Decoded VOP %d \n", *pFrameNumber);
				(*pFrameNumber)++;
				/*if(outYUVFile)*/ {
					if (IPP_STATUS_MP4_SHORTHEAD == rc1) {
						outfile_frame (outYUVFile,&Pic);
                        display_frame(&hDispCB, &Pic);/*Display on Manchanc*/
					} else {
						if ((*pFrameNumber) > 1) {
							outfile_frame (outYUVFile, &Pic);
                            /*Display on Manchanc*/
                            display_frame(&hDispCB, &Pic);
						}
					}
				}
                
            } else if (IPP_STATUS_SYNCNOTFOUND_ERR == rc){
                continue;
            } else if(rc != IPP_STATUS_ERR) {
                IPP_Log(logfile,"a", "Error:Decode_MPEG4Video !\n");
				break;		// exit
			}
		} else {
			if (IPP_Feof(inputCmpFile)) {
				/* last VOP and needn't search the next sync code */
                IPP_StartPerfCounter(perf_index);

                if(NSCCheckDisable == 0){
                    InsertSyncCode_MPEGDec(&streamBuffer, (IPP_STATUS_MP4_SHORTHEAD == rc1));              
                }
				if (IPP_STATUS_MP4_SHORTHEAD == rc1) {
					rc = Decode_H263Video(&streamBuffer, &Pic, ph263DecState);
				} else {
					rc = Decode_MPEG4Video(&streamBuffer, &Pic, pmp4DecState,
						bLastOutputFrame);
					bLastOutputFrame = 1;
				}

                IPP_StopPerfCounter(perf_index);
 
				/* if decoded successfully */
				if (rc == IPP_STATUS_NOERR) {
                    //IPP_Printf("Decoded VOP %d \n", *pFrameNumber);
					(*pFrameNumber)++;
					/*if(outYUVFile) */{
						if (IPP_STATUS_MP4_SHORTHEAD == rc1) {
							outfile_frame (outYUVFile,&Pic);
                            display_frame(&hDispCB, &Pic);/*Display on Manchanc*/
						} else {
							if ((*pFrameNumber) > 1) {
								outfile_frame (outYUVFile, &Pic);
                                display_frame(&hDispCB, &Pic);/*Display on Manchanc*/
							}
						}
					}
				} else if (rc != IPP_STATUS_ERR) {
                    IPP_Log(logfile,"a", "Error:Decode_MPEG4Video !\n");
					break;		// exit
				}
				bEnd = 1;
			} else {
				if (videoReloadBuffer (&streamBuffer, inputCmpFile)) {
                    IPP_Log(logfile,"a", "Warning:Fails to fill one VOP in stream buffer!\n");
					bEnd = 1;
				}
			}
		}
	}

    /*Flush the reserver frame .*/
	if (rc1 != IPP_STATUS_MP4_SHORTHEAD) {

        if (bLastOutputFrame) {
            IPP_StartPerfCounter(perf_index);

			rc = Decode_MPEG4Video(&streamBuffer, &Pic, pmp4DecState,
				bLastOutputFrame);

            IPP_StopPerfCounter(perf_index);

			if ((rc == IPP_STATUS_NOERR) /*&& outYUVFile*/)  {
                outfile_frame (outYUVFile, &Pic);
                display_frame(&hDispCB, &Pic);/*Display on Manchanc*/
                
			}
		}
	}

    /*DeInit Display*/
    display_close();

    llTotaltime = IPP_GetPerfData(perf_index);
    g_Frame_Num[IPP_VIDEO_INDEX] = *pFrameNumber;
    g_Tot_Time[IPP_VIDEO_INDEX] = llTotaltime;

    IPP_Printf("Total Time %.2f (s), FPS is %.2f fps\n ", llTotaltime/(1000.0*1000.0), (*pFrameNumber) * 1000.0 *1000.0/llTotaltime);

EndOfMpeg4Dec:

	videoFreeBuffer(&streamBuffer);

	if (IPP_STATUS_MP4_SHORTHEAD == rc1) {
		rc = DecoderFree_H263Video(&ph263DecState);
	} else {			
		rc = DecoderFree_MPEG4Video(&pmp4DecState);
	}	
    if(IPP_STATUS_NOERR != rc){
        IPP_Log(logfile,"a", "Error: DecoderFree_MPEG4Video \n");
    }

    /*Performance Data*/
    IPP_FreePerfCounter(perf_index);

	miscFreeGeneralCallbackTable(&pCallBackTable);	
	
	return rc;
}
	

/***************************************************************************
// Name:             main
// Description:      Entry function for MPEG4 BVOP decoder
//
// Returns:          0:		  decoding okay
//                   others:  decoding fail
*****************************************************************************/
int CodecTest (int argc, char *argv[])
{
	int   ret = 0;
	int   width=176, height=144; 
	unsigned long ulFrameNumber;

    char    fInput[512]     = {'\0'}; 
    char    fOutput[512]    = {'\0'};
    char    fLog[512]       = {'\0'};

	FILE	*outfile        = NULL;
	FILE    *fcmp           = NULL;

    char    *pLog           = NULL;
    int     len             = NULL;
    int     v3              = 0;

    /*Usage:*/
    IPP_Printf("%s\n", &scHelpMpeg4Dec);

	DisplayLibVersion();

    if((argc == 2) && strchr(argv[1], ':') ){

        ret = ParseCmd_MPEGDec(argv[1], fInput, fOutput ,fLog);
        if(ret != IPP_STATUS_NOERR){
            IPP_Log(pLog,"a", "Error: ParseCmd_MPEGDec \n");
            return ret;
        }
    }else if(argc >= 3){
        IPP_Strcpy(fInput, argv[1]);
        IPP_Strcpy(fOutput, argv[2]);

		if(argv[3] != NULL){
			v3 = atoi(argv[3]);
		}

    }else if(argc == 2){
        /*Manchanc on Linux*/

        IPP_Strcpy(fInput, argv[1]);
        //IPP_Strcpy(fOutput, "/dev/null");
        fOutput[0] = '+';
        
    }else{
        return -1;
    }

    //IPP_Printf("    Input: %s \n    Output: %s \n", fInput, fOutput);
	
    width  = 8;
	height = 8;

    len = IPP_Strlen(fLog);
    if(len){
        IPP_MemMalloc(&pLog, len ,8);
        IPP_Memcpy(pLog, fLog, len);
    }
 
	fcmp = IPP_Fopen (fInput, "rb");
	if ( !fcmp ) {
		IPP_Log(pLog,"a", "Error: IPP_Fopen %s \n", fInput);
		if(pLog){
			IPP_MemFree(&pLog);
		}
		return -1;
	}

    if(fOutput[0] != '+'){
        outfile = IPP_Fopen (fOutput, "wb");
	    if ( !outfile ) {
	        IPP_Log(pLog,"a", "Warbubg: IPP_Fopen %s \n", fOutput);
	        //return -1;
	    }  
    }else{
        outfile = NULL;
    }


	ret = MPEG4Coredecoder(fcmp, outfile, width, height, &ulFrameNumber, pLog, v3);

	switch (ret)
	{
	case IPP_STATUS_NOERR:
        //IPP_Log(pLog,"a", "Everything is Okay \n");
		break;
	case IPP_STATUS_ERR:
        IPP_Log(pLog,"a", "Unknown/Unspecified Error \n");
		break;
	case IPP_STATUS_NOMEM_ERR:
        IPP_Log(pLog,"a", "Memory allocation Failed \n");
		break;
	case IPP_STATUS_BADARG_ERR:
        IPP_Log(pLog,"a", "Bad Argument Error \n");
		break;
	case IPP_STATUS_SYNCNOTFOUND_ERR:
        IPP_Log(pLog,"a", "Miss Synchronize code \n");
		break;
	case IPP_STATUS_INPUT_ERR:
        IPP_Log(pLog,"a", "Wrong input parameter \n");
		break;
	case IPP_STATUS_NOTSUPPORTED_ERR:
        IPP_Log(pLog,"a", "Not support in current version yet \n");
		break;
	default:
        IPP_Log(pLog,"a", "Out of control \n");
		break;
	}
	
	if ( outfile )	  IPP_Fclose (outfile); 
	if ( fcmp )	  IPP_Fclose (fcmp); 
 
    if(pLog){
        IPP_MemFree(&pLog);
    }

	return ret ? -1 : 0;

}
