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
#include "mp2decocder.h"

//#define outfile_frame	//define this macro to skip write frame into file, it's just for debug

#define STMLEFTBYTES(pStm)	((pStm)->bsByteLen-((pStm)->pBsCurByte+(((pStm)->bsCurBitOffset+7)>>3)-(pStm)->pBsBuffer))
#define	ESTIMATED_ONEPICSIZE		800	//estimated one compressed picture's size, if user couldn't evaluate it, use its default value: 800
#define MORESAFE_DEC_FORERR			//This macro is not must to have, it's better to have.

/* INTERNAL_NSC_DISABLE determine which input mode is selected */
/*
// If internal NSC is disabled which is the default setting for decoder, the upper layer must ensure the data fragment sent 
// to Decode_MPEG2Video() must contain one whole picture data or at least one whole slice data. If the macro SLICEINPUT is defined, 
// the input data fragment must contain at least one whole slice data, and the fragment's end position must be at the border of slice. 
// Otherwise, the input fragment must contain one integrated compressed picture data.
*/
/*
// If internal NSC is enabled, there is no restrict for input fragment which is sent to Decode_MPEG2Video(). But the decoding delay maybe 
// increased(not performance drop). For example, when current input fragment already contains a whole picture just right, Decode_MPEG2Video() maybe still 
// return IPP_STATUS_NEED_INPUT to ask for more input data to finish decoding current picture. If the length of one compressed MB data is 
// very large(larger than 580 byte, 580 bytes is almost the upper limitation according to MPEG2 spec), decoder maybe regard it as corrupt stream and return error code. 
*/
/* Please refer to user guide for more detail */
#define INTERNAL_NSC_DISABLE	//define this macro to select NSC disable input mode, which is also called restricted input mode.

/* PIC_INPUT_MODE determine which sub input mode is selected when NSC disable input mode is chosen. */
// picture input and slice input are sub-modes of restricted input mode.
// For error stream, picture input mode is more safe and compatible then slice input mode, and it's recommendatory mode.
/* Please refer to user guide for more detail */
#define PIC_INPUT_MODE			//define this macro to select picture input mode, otherwise slice input mode is chosen


void reportFun(int DecRet, int DecFrameNum, char logfilename[], int mode, char flagfilename[]);

IppCodecStatus MPEG2decoder(FILE* inputCmpFile, FILE* outYUVFile,unsigned long * pFrameNumber)
{
	int DecRet;
	int mode;
#ifdef INTERNAL_NSC_DISABLE
	mode = 0;
#else
	mode = 1;
#endif

	if(mode == 0) {
		DecRet = MPEG2decoder_interNSCdisabled(inputCmpFile,outYUVFile,pFrameNumber);
	}else{
		DecRet = MPEG2decoder_interNSCenabled(inputCmpFile,outYUVFile,pFrameNumber);
	}

	//reportFun() is just used to log the information of decoder. In real application environment, it's not needed.
	reportFun(DecRet, *pFrameNumber, "MP2Declog.txt", mode, "NoCrashFlag.dat");
	if(DecRet == 0) {
		return IPP_STATUS_NOERR;
	}else{
		return IPP_STATUS_ERR;
	}
};

int MPEG2decoder_interNSCenabled(FILE* inputCmpFile, FILE* outYUVFile,unsigned long * pFrameNumber)
{
	MiscGeneralCallbackTable   *pCallBackTable = NULL;
	IppCodecStatus  ret = IPP_STATUS_NOERR, init_ret, readdata_ret;
	int retfun=0;	//only used by reportFun()
	IppPicture    DstPicture = {0};
	void * pDecoderState = NULL;
	IppBitstream  StmInputVideo={0}, StmInputSys={0};
	Ipp32u	uCurPacketLength = 0;
	int		bSysStm;
	int		bCodecEnded = 0;
	int		bSeriousErr = 0;
	int		bNoMore = 0;
	int		j=-1;
	
	*pFrameNumber = 0;

	if (miscInitGeneralCallbackTable(&pCallBackTable)) {
		printf("No memory error!\n");
		retfun = -4;
		goto MPEG2DECSAMPLE_FREERES_NSCENABLE;
	}
	if( IPP_STATUS_NOERR != InitCheckVideoInput(inputCmpFile, &StmInputVideo, &StmInputSys, &uCurPacketLength, &bSysStm) ) {
		printf("Init and check vide input error!\n");
		retfun = -1;
		goto MPEG2DECSAMPLE_FREERES_NSCENABLE;
	}

	while(1) {
		//repack and fill input data
		if( bNoMore==0 ) {
			readdata_ret = RepackFillVideoInput(inputCmpFile, &StmInputVideo, &StmInputSys, &uCurPacketLength, bSysStm);
			if(readdata_ret==IPP_STATUS_ERR) {
				//all data read
				bNoMore = 1;
			}else if(readdata_ret==IPP_STATUS_BUFOVERFLOW_ERR) {
				printf("There is no space to fill data!\n");
				bSeriousErr = 1;
				retfun = -2;
				break;
			}
		}


		//For error stream, it's better to append 0x00000000000001b7 at the end. Because the exact input buffer's size is larger than the StmInputVideo.bsByteLen, it's no risk to append 8 bytes data.
		//it's not must to have, it's better to have.
#ifdef MORESAFE_DEC_FORERR
		if(bNoMore == 1) {
			StmInputVideo.pBsBuffer[StmInputVideo.bsByteLen] = 0;
			StmInputVideo.pBsBuffer[StmInputVideo.bsByteLen+1] = 0;
			StmInputVideo.pBsBuffer[StmInputVideo.bsByteLen+2] = 0;
			StmInputVideo.pBsBuffer[StmInputVideo.bsByteLen+3] = 0;
			StmInputVideo.pBsBuffer[StmInputVideo.bsByteLen+4] = 0;
			StmInputVideo.pBsBuffer[StmInputVideo.bsByteLen+5] = 0;
			StmInputVideo.pBsBuffer[StmInputVideo.bsByteLen+6] = 1;
			StmInputVideo.pBsBuffer[StmInputVideo.bsByteLen+7] = 0xb7;
		}
#endif


		if(pDecoderState == NULL) {
			int bDisNSC = 0;
			//the first input data should be long enough to init decoder
			init_ret = DecoderInitAlloc_MPEG2Video(pCallBackTable,&pDecoderState,&StmInputVideo);
			if ( IPP_STATUS_NOERR != init_ret ) {
				printf("Initialize decoder failed.\n");
				if( IPP_STATUS_BADARG_ERR != init_ret) {
					DecoderFree_MPEG2Video(&pDecoderState);		//if DecoderInitAlloc_MPEG2Video failed, always should call DecoderFree_MPEG2Video() except the return code is IPP_STATUS_BADARG_ERR. Calling DecoderFree_MPEG2Video() more one time has no side-effect.
				}
				pDecoderState = NULL;
				if( IPP_STATUS_NOMEM_ERR == init_ret ) {
					printf("No memory error!\n");
					retfun = -4;
					goto MPEG2DECSAMPLE_FREERES_NSCENABLE;
				}else if(IPP_STATUS_FATAL_ERR == init_ret) {
					printf("fatal error happen during init!\n");
					retfun = -3;
					goto MPEG2DECSAMPLE_FREERES_NSCENABLE;
				}else{//other errors are all caused by stream error
					if(bNoMore == 1) {
						if(init_ret == IPP_STATUS_BUFOVERFLOW_ERR || STMLEFTBYTES(&StmInputVideo) < ESTIMATED_ONEPICSIZE) {
							//the left stream isn't enough to initialize or just little stream bits left, finish decoding procedure
							retfun = -1;
							goto MPEG2DECSAMPLE_FREERES_NSCENABLE;
						}
					}
					continue;	//continue the loop to try to initialize the decoder again.
				}
			}
			DecodeSendCmd_MPEG2Video(IPPVC_SET_NSCCHECKDISABLE, &bDisNSC, 0, pDecoderState);
		}


		//consume the bits in StmInputVideo until need more bits
		while(1) {
			ret = Decode_MPEG2Video(&StmInputVideo,&DstPicture, bNoMore, pDecoderState);
			if( ret == IPP_STATUS_NOERR || ret == IPP_STATUS_BS_END ) {
				j++;
				if(j>0) {
					outfile_frame(outYUVFile,&DstPicture);
					printf("Frame %d finished. %s\n",j,(DstPicture.picStatus&0xff000000)==0?"no image err":"image err");
				}
				if(ret == IPP_STATUS_BS_END) {
					if(bNoMore == 0 || STMLEFTBYTES(&StmInputVideo) > ESTIMATED_ONEPICSIZE) {
						//the sequence_end_code detected by decoder maybe caused by error bit, continue to decode
						printf("the sequence end code is detected, because still input exits, continue to decode\n");
						//reset the decoder state
						DecodeSendCmd_MPEG2Video(IPPVC_SET_DECODENEWPIC, 0, 0, pDecoderState);
					}else{
						printf("Decoder successfully ended!\n");
						bCodecEnded = 1;
						break;
					}
				}
				continue;
			}else if(ret == IPP_STATUS_FIELD_PICTURE_TOP || ret == IPP_STATUS_FIELD_PICTURE_BOTTOM) {
				continue;
			}else if(ret == IPP_STATUS_NEED_INPUT) {
				break;	//need more data
			}else if(ret == IPP_STATUS_FATAL_ERR) {
				printf("fatal error happen, finish decoding!\n");
				retfun = -3;
				goto MPEG2DECSAMPLE_FREERES_NSCENABLE;
			}else{
				printf("Decoding Common Err.\n");

				//reset the decoder state
				DecodeSendCmd_MPEG2Video(IPPVC_SET_DECODENEWPIC, 0, 0, pDecoderState);

				//suggest to execute following action to improve the stability of codec and prevent dead looping
				if(bNoMore != 0 && STMLEFTBYTES(&StmInputVideo) < ESTIMATED_ONEPICSIZE) {
					printf("error happened during NoMore data\n");
					bSeriousErr = 1;		//It's better to not ignore the error when no more data, terminate the decoding
				}

				break;
			}	
		}

		if(bSeriousErr != 0 || bCodecEnded != 0) {
			break;	//end loop
		}

	}

	if(bSeriousErr == 1){
		//error happened, let StmInputvideo.pBsCurByte point to the end of stream to get the last frame
		StmInputVideo.pBsCurByte = StmInputVideo.pBsBuffer + StmInputVideo.bsByteLen;
		ret = Decode_MPEG2Video(&StmInputVideo,&DstPicture, 1, pDecoderState);
		j++;
		if(j>0) {
			outfile_frame(outYUVFile,&DstPicture);
			printf("Get last Frame %d finished. %s\n",j,(DstPicture.picStatus&0xff000000)==0?"no image err":"image err");
		}
		printf("some error detected!\n");
	}

	*pFrameNumber = j;	//the decoded frame's amount, some image error maybe exist in some frame

MPEG2DECSAMPLE_FREERES_NSCENABLE:
	FreeVideoInput(&StmInputVideo, &StmInputSys);
	DecoderFree_MPEG2Video(&pDecoderState);
	miscFreeGeneralCallbackTable(&pCallBackTable);

	return retfun;
}

int MPEG2decoder_interNSCdisabled(FILE* inputCmpFile, FILE* outYUVFile,unsigned long * pFrameNumber)
{
	MiscGeneralCallbackTable   *pCallBackTable = NULL;
	IppCodecStatus  ret = IPP_STATUS_NOERR, readdata_ret, cmd_ret, init_ret;
	int retfun = 0;		//only used by reportFun()
	IppPicture    DstPicture = {0};
	void * pDecoderState = NULL;
	IppBitstream StmInputVideo={0}, StmInputSys={0};
	Ipp32u	uCurPacketLength = 0;
	int		bSysStm;
	int		bCodecEnded = 0;
	int		bSeriousErr = 0;
	int		bNoMore = 0;
	int		j=-1;
	Ipp8u*	NextUnitStartPos;
	IppPosition     nextSNCPos;
	IppCommand	cmd;
#ifndef PIC_INPUT_MODE		//slice input mode
	int		TmpLen, bTmpNoMore;
#endif

	*pFrameNumber = 0;

	if (miscInitGeneralCallbackTable(&pCallBackTable)) {
		printf("No memory error!\n");
		retfun = -4;
        goto MPEG2DECSAMPLE_FREERES_NSCDISABLE;
	}
	if(IPP_STATUS_NOERR != InitCheckVideoInput(inputCmpFile, &StmInputVideo, &StmInputSys, &uCurPacketLength, &bSysStm)) {
		printf("Init and check vide input error!\n");
		retfun = -1;
		goto MPEG2DECSAMPLE_FREERES_NSCDISABLE;
	}

	while(1) {
		//repack and fill data directly on input stream
		if( bNoMore==0) {
			readdata_ret = RepackFillVideoInput(inputCmpFile, &StmInputVideo, &StmInputSys, &uCurPacketLength, bSysStm);
			if(readdata_ret==IPP_STATUS_ERR) {
				//all data read
				bNoMore = 1;
			}else if(readdata_ret==IPP_STATUS_BUFOVERFLOW_ERR) {
				printf("There is no space to fill data!\n");
				bSeriousErr = 1;
				retfun = -2;
				break;
			}
		}

		//For error stream, it's better to append 0x00000000000001b7 at the end. Because the exact input buffer's size is larger than the StmInputVideo.bsByteLen, it's no risk to append 8 bytes data.
		//it's not must to have, it's better to have.
#ifdef MORESAFE_DEC_FORERR
		if(bNoMore == 1) {
			StmInputVideo.pBsBuffer[StmInputVideo.bsByteLen] = 0;
			StmInputVideo.pBsBuffer[StmInputVideo.bsByteLen+1] = 0;
			StmInputVideo.pBsBuffer[StmInputVideo.bsByteLen+2] = 0;
			StmInputVideo.pBsBuffer[StmInputVideo.bsByteLen+3] = 0;
			StmInputVideo.pBsBuffer[StmInputVideo.bsByteLen+4] = 0;
			StmInputVideo.pBsBuffer[StmInputVideo.bsByteLen+5] = 0;
			StmInputVideo.pBsBuffer[StmInputVideo.bsByteLen+6] = 1;
			StmInputVideo.pBsBuffer[StmInputVideo.bsByteLen+7] = 0xb7;
		}
#endif


		if(pDecoderState == NULL) {
			//we assume the first input data is long enough to init decoder
			init_ret = DecoderInitAlloc_MPEG2Video(pCallBackTable,&pDecoderState,&StmInputVideo);
			if ( IPP_STATUS_NOERR !=  init_ret ) {
				printf("Initialize decoder failed.\n");
				if( IPP_STATUS_BADARG_ERR != init_ret) {
					DecoderFree_MPEG2Video(&pDecoderState);		//if DecoderInitAlloc_MPEG2Video failed, always should call DecoderFree_MPEG2Video() except the return code is IPP_STATUS_BADARG_ERR. Calling DecoderFree_MPEG2Video() more one time has no side-effect.
				}
				pDecoderState = NULL;
				if(IPP_STATUS_NOMEM_ERR == init_ret) {
					printf("No memory error!\n");
					retfun = -4;
					goto MPEG2DECSAMPLE_FREERES_NSCDISABLE;
				}else if(IPP_STATUS_FATAL_ERR == init_ret) {
					printf("fatal error happen during init!\n");
					retfun = -3;
					goto MPEG2DECSAMPLE_FREERES_NSCDISABLE;
				}else{//other errors are all caused by stream error
					if(bNoMore == 1) {
						if(init_ret == IPP_STATUS_BUFOVERFLOW_ERR || STMLEFTBYTES(&StmInputVideo) < ESTIMATED_ONEPICSIZE) {
							//the left stream isn't enough to initialize or just little stream bits left, finish decoding procedure
							retfun = -1;
							goto MPEG2DECSAMPLE_FREERES_NSCDISABLE;
						}
					}
					continue;	//continue the loop to try to initialize the decoder again.
				}
			}
		}


		//consume the bits in StmInputVideo until need more bits
		while(1) {
			NextUnitStartPos = NULL;

#ifdef PIC_INPUT_MODE
			cmd = IPPVC_SEEK_NEXTSYNCCODE;
#else
			cmd = IPPVC_SEEK_LASTSLICEENDPOS;
#endif
			//If parser could ensure the current input fragment hold a whole picture data, it's no need to call DecodeSendCmd_MPEG2Video() to check whether current data hold a whole picture
			cmd_ret = DecodeSendCmd_MPEG2Video(cmd,(void*)&StmInputVideo,(void*)&nextSNCPos,(void*)pDecoderState);
			if(cmd_ret != IPP_STATUS_NOERR) {
				if(bNoMore == 0) {
					break;	//get more input data
				}
			}else{
				//If no bit error in stream, it's not necessary to record the end position of current picture in compressed stream.
				NextUnitStartPos = nextSNCPos.ptr;
			}
			
#ifndef PIC_INPUT_MODE		//slice input mode
			TmpLen = StmInputVideo.bsByteLen;
			bTmpNoMore = bNoMore;
			if(NextUnitStartPos != NULL) {
				//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				//Note: for slice input, continuous 23 zero bit must appear at the end of input data for Decode_MPEG2Video(). Otherwise, Decode_MPEG2Video() couldn't run normally.
				//And the start code of next unit couldn't appear at the end of input data.
				//Therefore, just + 3, not + 4
				StmInputVideo.bsByteLen = NextUnitStartPos - StmInputVideo.pBsBuffer + 3;
				if(StmInputVideo.bsByteLen < TmpLen) {
					bNoMore = 0;
				}
			}
#endif
			ret = Decode_MPEG2Video(&StmInputVideo,&DstPicture, bNoMore, pDecoderState);

#ifndef PIC_INPUT_MODE		//slice input mode
			StmInputVideo.bsByteLen = TmpLen;
			bNoMore = bTmpNoMore;
#endif
			
			if(ret == IPP_STATUS_NOERR || ret == IPP_STATUS_BS_END) {
				j++;
				if(j>0) {
					outfile_frame(outYUVFile,&DstPicture);
					printf("Frame %d finished. %s\n",j,(DstPicture.picStatus&0xff000000)==0?"no image err":"image err");
				}
				if( ret == IPP_STATUS_BS_END ) {
					if(bNoMore == 0 || STMLEFTBYTES(&StmInputVideo) > ESTIMATED_ONEPICSIZE) {
						//the sequence_end_code detected by decoder maybe caused by error bit, continue to decode
						printf("the sequence end code is detected, because still input exits, continue to decode\n");
						//reset the decoder state
						DecodeSendCmd_MPEG2Video(IPPVC_SET_DECODENEWPIC, 0, 0, pDecoderState);
					}else{
						printf("Decoder successfully ended!\n");
						bCodecEnded = 1;
						break;
					}
				}
				continue;
			}else if(ret == IPP_STATUS_FIELD_PICTURE_TOP || ret == IPP_STATUS_FIELD_PICTURE_BOTTOM ) {
				continue;
			}else if(ret == IPP_STATUS_NEED_INPUT) {
				break;	//need more data
			}else if(ret == IPP_STATUS_FATAL_ERR) {
				printf("fatal error happen, finish decoding!\n");
				retfun = -3;
				goto MPEG2DECSAMPLE_FREERES_NSCDISABLE;
			}else{
				printf("Decoding Common Err.\n");
				//error occur, current picture data isn't decodable.
				//suggest to execute following action to improve the stability of codec and prevent dead looping
				//reset the decoder state
				DecodeSendCmd_MPEG2Video(IPPVC_SET_DECODENEWPIC, 0, 0, pDecoderState);
				if(bNoMore != 0 && STMLEFTBYTES(&StmInputVideo) < ESTIMATED_ONEPICSIZE) {
					printf("error happened during NoMore data\n");
					bSeriousErr = 1;	//It's more safe to end the decoding if error happened when no more data input
				}
#ifdef MORESAFE_DEC_FORERR
				if(NextUnitStartPos) {
#ifdef PIC_INPUT_MODE	//if the mode is slice input mode, it's better not to execute following code in order to avoid skipping correct picture data. It's more compatibly but less safe.
					//for picture input mode, following code is not must to have but better to have.
					//skip the current input data fragment
					StmInputVideo.pBsCurByte = NextUnitStartPos;
					StmInputVideo.bsCurBitOffset = 0;
#endif
				}
#endif
				break;
			}
		}

		if(bSeriousErr != 0 || bCodecEnded != 0) {
			break;	//end loop
		}

	}

	if(bSeriousErr == 1){
		//error happened, let StmInputvideo.pBsCurByte point to the end of stream to get the last frame
		StmInputVideo.pBsCurByte = StmInputVideo.pBsBuffer + StmInputVideo.bsByteLen;
		ret = Decode_MPEG2Video(&StmInputVideo,&DstPicture, 1, pDecoderState);
		j++;
		if(j>0) {
			outfile_frame(outYUVFile,&DstPicture);
			printf("Get last Frame %d finished. %s\n",j,(DstPicture.picStatus&0xff000000)==0?"no image err":"image err");
		}
		printf("some error detected!\n");
	}

	*pFrameNumber = j;	//the decoded frame's amount, some image error maybe exist in some frame

MPEG2DECSAMPLE_FREERES_NSCDISABLE:
	FreeVideoInput(&StmInputVideo, &StmInputSys);
	DecoderFree_MPEG2Video(&pDecoderState);
	miscFreeGeneralCallbackTable(&pCallBackTable);

	return retfun;
}

//reportFun() is just used to log the information of decoder. In real application environment, it's not needed.
void reportFun(int DecRet, int DecFrameNum, char logfilename[], int mode, char flagfilename[])
{
	FILE *fp;
	fp = fopen(logfilename,"at");
	if(fp == NULL) {
		printf("open file error\n");
		return;
	}
	fprintf(fp, "Decoded\t%d\tframes.\t%s.\t", DecFrameNum, mode==0?"internal NSC disable":"internal NSC enable");
	if(DecRet == -2) {
		fprintf(fp, "ret code %2d, The input buffer isn't enough to hold one picture/slice!\n", DecRet);
	}else if(DecRet == -1){
		fprintf(fp, "ret code %2d, The dec finished fail because of generic error\n", DecRet);
	}else if(DecRet == -3){
		fprintf(fp, "ret code %2d, The dec finished fail! fatal error happen!\n", DecRet);
	}else if(DecRet == 0){
		fprintf(fp, "ret code %2d, The dec finished Suc!\n", DecRet);
	}else if(DecRet == -4){
		fprintf(fp, "ret code %2d, The dec finished fail because of no memory!\n", DecRet);
	}else{
		fprintf(fp, "ret code %2d, The dec finished fail!\n", DecRet);
	}
	fclose(fp);

	//new a file as a flag
	if(DecRet != -3 && DecRet != -4){	//we consider no memory and fatal error are similar to crash
		fp = fopen(flagfilename, "wb");
		fclose(fp);
	}

	return;
}



