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


#include <string.h>
#include "misc.h"
#include "codecVC.h"
#include "vgusr.h"

#include "ippLV.h"

#define FALSE 0
#define TRUE 1

#define STMLEFTBYTES(pStm)  ((pStm)->bsByteLen-((pStm)->pBsCurByte+(((pStm)->bsCurBitOffset+7)>>3)-(pStm)->pBsBuffer)) /* compute the left available bits in a IppStream */
#define	ESTIMATED_ONEPICSIZE  800/*estimated one compressed picture's size, if user couldn't evaluate it, use its default value: 800 */

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
	ret = GetLibVersion_MPEG2DEC(libversion,sizeof(libversion));
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
// Name:                ParseNextCmd
// Description:         Parse the user command 
//
// Input Arguments:
//      pNextCmd:   Pointer to the input cmd buffer
//
// Output Arguments:
//      pSrcFileName:   Pointer to src file name
//      pDstFileName:   Pointer to dst file name
//      pLogFileName:   Pointer to log file name
// Returns:
//        [Success]     IPP_OK
//        [Failure]     IPP_FAIL
******************************************************************************/

int ParseMpeg2Cmd(char *pNextCmd, char **pSrcFileName, char **pDstFileName, char** pLogFileName)
{
    int iLength;
    char *pIdx, *pCurPos, *pEnd;

    /* Parse comments */
    if( '#'== pNextCmd[0] || '\t'== pNextCmd[0] || ' '== pNextCmd[0] || '\0' == pNextCmd[0] ) {
        return IPP_FAIL;
    }

    pCurPos = pNextCmd;

    while((pIdx = IPP_Strstr(pCurPos, "-"))){

        pEnd = pIdx;

        for(;;){
            pEnd = IPP_Strstr(pEnd+1, "-");
            if(pEnd == NULL || pEnd[2] == ':'){
                break;
            }
        }

        if(!pEnd) {
            pEnd = pNextCmd + IPP_Strlen(pNextCmd);
            iLength = (int)(pEnd - pIdx) - 3;
        } else {
            iLength = (int)(pEnd - pIdx) - 4;
        }

        switch(pIdx[1]) {
        case 'i':
        case 'I':
            IPP_MemMalloc((void **)pSrcFileName, 512, 4);
            IPP_Strncpy(*pSrcFileName, pIdx + 3, iLength);
            *(*pSrcFileName+iLength) = '\0';
            
            break;
        case 'o':
        case 'O':
            IPP_MemMalloc((void **)pDstFileName, 512, 4);
            IPP_Strncpy(*pDstFileName, pIdx + 3, iLength);
            *(*pDstFileName+iLength) = '\0';
            
            break;
        case 'l':
        case 'L':
            IPP_MemMalloc((void **)pLogFileName, 512, 4);
            IPP_Strncpy(*pLogFileName, pIdx + 3, iLength);
            *(*pLogFileName+iLength) = '\0';
            
            break;
        default:
            return IPP_FAIL;
        }

        pCurPos = pEnd;
    }

    return IPP_OK;
}

/******************************************************************************
// Name:                Mpeg2Dec
// Description:         Decode Mpeg2 video stream 
//
// Input Arguments:
//      pSrcFileName:   Pointer to src file name
//      pDstFileName:   Pointer to dst file name
//      pLogFileName:   Pointer to log file name
//
// Output Arguments:    No
//
// Returns:
//        [Success]     IPP_OK
//        [Failure]     IPP_FAIL
******************************************************************************/

int Mpeg2Dec(char *pSrcFileName, char *pDstFileName, char* pLogFileName)
{
    IPP_FILE *srcFile = NULL, *dstFile = NULL;
    MiscGeneralCallbackTable    *pCallBackTable;

    IppPicture                  DstPicture;
    IppBitstream                SrcBitStream;
    void* pDecoderState = NULL;
    char *index;
//    char pPerLogFileName[256];
    DISPLAY_CB DisplayCB;   

    IppCodecStatus rtCode;
    int perf_index;
    long long TotTime;

    IppCommand cmd;
    IppPosition     nextSNCPos;
    Ipp8u* NextUnitStartPos;
    int bNoMore = FALSE;
    int bCodecEnded = FALSE;
    int frame_num = -1;
	int ret = IPP_OK;
    // initialize global variable which are used to counter decoded frame number and decoding time.
    g_Frame_Num[IPP_VIDEO_INDEX]=0;
    g_Tot_Time[IPP_VIDEO_INDEX]=0;

    /* Open input and output files */
    if( NULL != pDstFileName ){
        if (NULL == (dstFile = IPP_Fopen(pDstFileName, "w+b"))) {
          IPP_Log(pLogFileName,"a","IPP Error:can not open dst file :%s.\n",pDstFileName);
          return IPP_FAIL;
        }
    }

    if (NULL == (srcFile = IPP_Fopen(pSrcFileName, "r+b"))) {
        IPP_Log(pLogFileName,"a","IPP Error:can not open src file :%s.\n",pSrcFileName);
        if( NULL != dstFile ){
            IPP_Fclose(dstFile);
        }
        return IPP_FAIL;
    }

    /* Init callback table */
    if ( 0 != miscInitGeneralCallbackTable(&pCallBackTable) ) {
        IPP_Log(pLogFileName,"a","IPP Error:init callback table failed.\n");
        IPP_Fclose(srcFile);
        if( NULL != dstFile ){
            IPP_Fclose(dstFile);
        }
        return IPP_FAIL;
    }


    /* Allocate the input stream buffer */
    if( IPP_FAIL == videoInitBuffer(&SrcBitStream) ) {
        IPP_Log(pLogFileName,"a","IPP Error:malloc SrcBitStream failed.\n");
        IPP_Fclose(dstFile);
        IPP_Fclose(srcFile);
        return IPP_FAIL;
    }

    IPP_GetPerfCounter(&perf_index,DEFAULT_TIMINGFUNC_START,DEFAULT_TIMINGFUNC_STOP);
    IPP_ResetPerfCounter(perf_index);
    
    while(1) {
            if( FALSE == bNoMore ) {
                if(IPP_FAIL == CheckStreamSpace(&SrcBitStream)) {
                    IPP_Log(pLogFileName,"a","IPP Error:There is no space to fill data!\n");
                    ret = IPP_FAIL;
                    goto END;
                }
                if( 1 == videoReloadBuffer(&SrcBitStream,srcFile)) {
                      /* all data read */
                     bNoMore = TRUE;
                } 
            }

           if( NULL == pDecoderState ) {
                /* we assume the first input data is long enough to init decoder */
                rtCode = DecoderInitAlloc_MPEG2Video(pCallBackTable,&pDecoderState,&SrcBitStream);
                if ( IPP_STATUS_NOERR !=  rtCode ) {
                    IPP_Log(pLogFileName,"a","IPP Error:Initialize decoder failed.\n");
                    if( IPP_STATUS_BADARG_ERR != rtCode) {
                          /*if DecoderInitAlloc_MPEG2Video failed, always should call DecoderFree_MPEG2Video() except the return code is IPP_STATUS_BADARG_ERR. Calling DecoderFree_MPEG2Video() more one time has no side-effect.*/
                         DecoderFree_MPEG2Video(&pDecoderState);
                    }

                    pDecoderState = NULL;
                    if(IPP_STATUS_NOMEM_ERR == rtCode) {
                        IPP_Log(pLogFileName,"a","IPP Error:Initialize no memory error!\n");
						ret = IPP_FAIL;
                        goto END;
                    }else if(IPP_STATUS_FATAL_ERR == rtCode) {
                        IPP_Log(pLogFileName,"a","IPP Error:Fatal error happen during init!\n");
						ret = IPP_FAIL;
                        goto END;
                    }else{      /*other errors are all caused by stream error*/
                        if( TRUE == bNoMore) {
                            if( IPP_STATUS_BUFOVERFLOW_ERR == rtCode || STMLEFTBYTES(&SrcBitStream) < ESTIMATED_ONEPICSIZE) {
                                /*the left stream isn't enough to initialize or just little stream bits left, finish decoding procedure */
                                IPP_Log(pLogFileName,"a","IPP Error:No bits to initialize!\n");
								ret = IPP_FAIL;
                                goto END;
                            }
                        }
                        /*continue the loop to try to initialize the decoder again.*/
                        continue;
                   }
                }
           }
          /*consume the bits in StmInputVideo until need more bits */

          while(1) {
              NextUnitStartPos = NULL;
              cmd = IPPVC_SEEK_NEXTSYNCCODE;
               /*If parser could ensure the current input fragment hold a whole picture data, it's no need to call DecodeSendCmd_MPEG2Video() to check whether current data hold a whole picture */
               rtCode = DecodeSendCmd_MPEG2Video(cmd,(void*)&SrcBitStream,(void*)&nextSNCPos,(void*)pDecoderState);
               if( IPP_STATUS_NOERR != rtCode ) {
                   if( FALSE == bNoMore ) {
                       break;	/*get more input data*/
                   }
               }else{
                   /*If no bit error in stream, it's not necessary to record the end position of current picture in compressed stream.*/
                   NextUnitStartPos = nextSNCPos.ptr;
               }
               IPP_StartPerfCounter(perf_index);
               rtCode = Decode_MPEG2Video(&SrcBitStream,&DstPicture, bNoMore, pDecoderState);
               IPP_StopPerfCounter(perf_index);

               if( IPP_STATUS_NOERR == rtCode ||  IPP_STATUS_BS_END == rtCode ) {
                    frame_num++;
                    if(frame_num>0) {
                          if( NULL != dstFile ){
                               if(IPP_FAIL == outfile_frame(dstFile,&DstPicture)) {
                                   IPP_Log(pLogFileName,"a","Err:write output file error\n");
                               }
                           }
                           /* display one frame*/
                            if(1 == frame_num){
                                 display_open(&DisplayCB,DstPicture.picWidth,DstPicture.picHeight);
                            }
                            display_frame(&DisplayCB, &DstPicture);

                          // IPP_Printf("Frame %d finished.\r",frame_num);
                    }
                    if( IPP_STATUS_BS_END == rtCode ) {
                        if( FALSE == bNoMore || STMLEFTBYTES(&SrcBitStream) > ESTIMATED_ONEPICSIZE ) {
                            /*the sequence_end_code detected by decoder maybe caused by error bit, continue to decode */
                            IPP_Log(pLogFileName,"a","IPP :the sequence end code is detected, because still input exits, continue to decode\n");
                            /*reset the decoder state */
                            //DecodeSendCmd_MPEG2Video(IPPVC_SET_DECODENEWPIC, 0, 0, pDecoderState);
                            DecodeSendCmd_MPEG2Video(1, 0, 0, pDecoderState);
                            DecoderFree_MPEG2Video(&pDecoderState);
                            pDecoderState = NULL;
                            g_Frame_Num[IPP_VIDEO_INDEX] += frame_num;
                            frame_num = -1;
                        }else{
                             bCodecEnded = TRUE;
                        }
                        display_close ();
                        break;
                    }
                    continue;
               }else if( IPP_STATUS_FIELD_PICTURE_TOP == rtCode || IPP_STATUS_FIELD_PICTURE_BOTTOM == rtCode) {
                    continue;
               }else if( IPP_STATUS_NEED_INPUT == rtCode) {
                    break;  /*need more data */
               }else if( IPP_STATUS_FATAL_ERR == rtCode) {
                    IPP_Log(pLogFileName,"a","IPP Error:fatal error happen, finish decoding!\n");
					ret = IPP_FAIL;
                    goto END;
               }else{
                    IPP_Log(pLogFileName,"a","IPP Error:Decoding Common Err.\n");
                    /*error occur, current picture data isn't decodable.
                    //suggest to execute following action to improve the stability of codec and prevent dead looping
                    //reset the decoder state
                    */
                   // DecodeSendCmd_MPEG2Video(IPPVC_SET_DECODENEWPIC, 0, 0, pDecoderState);
                    DecodeSendCmd_MPEG2Video(1, 0, 0, pDecoderState);
                    if( FALSE != bNoMore && STMLEFTBYTES(&SrcBitStream) < ESTIMATED_ONEPICSIZE) {
                        IPP_Log(pLogFileName,"a", "IPP Error:error happened during No More data\n");
						ret = IPP_FAIL;
                        goto END;   /*It's more safe to end the decoding if error happened when no more data input */
                    }

                    if(NextUnitStartPos) {
                        /*skip the current input data fragment*/
                        SrcBitStream.pBsCurByte = NextUnitStartPos;
                        SrcBitStream.bsCurBitOffset = 0;
                    }

                    break;
               }
          }

        if( TRUE == bCodecEnded ) {
            break;	/*end loop */
        }
    }

    g_Frame_Num[IPP_VIDEO_INDEX] += frame_num;

    TotTime = IPP_GetPerfData(perf_index);
    g_Tot_Time[IPP_VIDEO_INDEX] = TotTime;

    IPP_FreePerfCounter(perf_index);

END:
    
    videoFreeBuffer(&SrcBitStream);
    DecoderFree_MPEG2Video(&pDecoderState);
    miscFreeGeneralCallbackTable(&pCallBackTable); 

    /* Close file handler */
    IPP_Fclose(srcFile);
    if( NULL != dstFile ) {
        IPP_Fclose(dstFile);
    }
    return ret;
}

/******************************************************************************
// Name:                CodecTest
// Description:         Main entry for MPEG2 decoder test
//
// Input Arguments:
//                      N/A
// Output Arguments:
//                      N/A
// Returns:
******************************************************************************/
int CodecTest(int argc, char** argv)
{
    char *pSrcFileName = NULL;
    char *pDstFileName = NULL;
    char *pLogFileName = NULL;
	int ret;

	DisplayLibVersion();
   
    if(argc == 2 && ParseMpeg2Cmd(argv[1], &pSrcFileName, &pDstFileName, &pLogFileName) == 0){
		ret = Mpeg2Dec(pSrcFileName, pDstFileName,pLogFileName);
    }else{
        IPP_Log(pLogFileName,"w",
            "Command is incorrect! \n                                                       \
            Usage:appMpeg2Dec.exe \"-i:test.mpeg2 -o:test.yuv -l:test.log\"\n \
                -i input file                                                               \
                -o output file                                                              \
                -l log file                                                                 \
        ");
		return IPP_FAIL;
    }
   
    if(NULL != pSrcFileName ){
        IPP_MemFree((void**)&pSrcFileName);
    }
    if(NULL != pDstFileName ){
        IPP_MemFree((void**)&pDstFileName);
    }
    if(NULL != pLogFileName ){
        IPP_MemFree((void**)&pLogFileName);
    }
    
	return ret;
}
/* EOF */
