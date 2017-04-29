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
#include "mjpegdec.h"

#include "ippLV.h"

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
	ret = GetLibVersion_JPEGDEC(libversion,sizeof(libversion));
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

/*************************************************************************************************************
// Name:  MJPEGFmAVIDec
// Description:
//      Entry of MPJEG Decoder 
// Input Arguments:
//      pSrcFileName  - Pointer to the source file
// Output Arguments:
//      pDstFileName  - Pointer to the output file
//      pLogFileName  - Pointer to the log file
// Returns:
//      None
// Others:
//      None
*************************************************************************************************************/
int MJPEGFmAVIDec(char *pSrcFileName, char *pDstFileName, char* pLogFileName)
{
    IPP_FILE *srcFile = NULL;
    IPP_FILE *dstFile = NULL;
    IPP_FILE *logFile = NULL;
    unsigned int iFileReadSize = 0;
    MiscGeneralCallbackTable    *pCallBackTable = NULL;
    IppPicture      DstPicture;
    IppBitstream  SrcBitStream;  
    void *pDecoderState = NULL;	
    IppCodecStatus	 rtCode = IPP_STATUS_NOERR;
    unsigned char allFrameDecDone = 0;
    char err[256];
    int iPicNum = 0;
    int ret = IPP_OK;
    Ipp32u iFourCCID = 0;
    unsigned char cByte0, cByte1, cByte2, cByte3;

    int perf_index;
    DISPLAY_CB DisplayCB;
    long long TotTime;

    /* Open input and output files*/    
    if(NULL!=pDstFileName){
        if( NULL == (dstFile = IPP_Fopen(pDstFileName, "w+b"))){
            IPP_Log(pLogFileName,"a","IPP Error:can not open dst file :%s\n",pDstFileName);            
        }
    }

    if( NULL == (srcFile = IPP_Fopen(pSrcFileName, "r+b"))){
        IPP_Log(pLogFileName,"a","IPP Error:can not open src file :%s\n",pSrcFileName);
        if(NULL!=dstFile){
            IPP_Fclose(dstFile);
            dstFile = NULL;
        }
        IPP_Printf("MJPEG Decode Failed\n");
        return IPP_FAIL;
    }

    /* Init callback table */
    if ( miscInitGeneralCallbackTable(&pCallBackTable) != 0 ) {
        IPP_Log(pLogFileName,"a","IPP Error:init callback table failed\n");
        if(NULL!=dstFile){
            IPP_Fclose(dstFile);        
            dstFile = NULL;
        }
        if(NULL!=srcFile){
            IPP_Fclose(srcFile);        
            srcFile = NULL;
        }
        IPP_Printf("MJPEG Decode Failed\n");
        return IPP_FAIL; 
    }

    /* Allocate the input stream buffer */
    pCallBackTable->fMemCalloc(&SrcBitStream.pBsBuffer, IPP_MJPEGFMAVI_INP_STREAM_BUF_LEN, 4);
    if (NULL == SrcBitStream.pBsBuffer) {
        IPP_Log(pLogFileName,"a","IPP Error:malloc SrcBitStream failed\n");
        if(NULL!=pCallBackTable){
            miscFreeGeneralCallbackTable(&pCallBackTable);
            pCallBackTable = NULL;
        }
       if(NULL!=dstFile){
            IPP_Fclose(dstFile);        
            dstFile = NULL;
        }
        if(NULL!=srcFile){
            IPP_Fclose(srcFile);        
            srcFile = NULL;
        }
        IPP_Printf("MJPEG Decode Failed\n");
        return IPP_FAIL;
    }
    SrcBitStream.bsByteLen = IPP_MJPEGFMAVI_INP_STREAM_BUF_LEN;
    SrcBitStream.bsCurBitOffset = 0;
    SrcBitStream.pBsCurByte = SrcBitStream.pBsBuffer + IPP_MJPEGFMAVI_INP_STREAM_BUF_LEN;

    /* Init Decoder*/
    if(IPP_STATUS_NOERR != DecoderInitAlloc_MJPEGFmAVI(
        &DstPicture, &pDecoderState, pCallBackTable, (void *)srcFile)){ 
            IppStatusMessage(rtCode,err);
            IPP_Log(pLogFileName,"a","IPP Error:DecoderInitAlloc_MJPEGFmAVI failed, %s\n", err);
            ret = IPP_FAIL;
            goto END;
    }

    IPP_GetPerfCounter(&perf_index, DEFAULT_TIMINGFUNC_START, DEFAULT_TIMINGFUNC_STOP);
    IPP_ResetPerfCounter(perf_index); 

    IPP_StartPerfCounter(perf_index);
    /* chech whether avi source file */
    /* RIFF */
    if(4!= IPP_Fread((void*)SrcBitStream.pBsBuffer, 1, 4, srcFile) ){
        IppStatusMessage(IPP_STATUS_ERR,err);
        IPP_Log(pLogFileName,"a","IPP Error:Parse_MJPEGFmAVI failed, %s\n", err);
        ret = IPP_FAIL;
        goto END;
    }
    SrcBitStream.pBsCurByte = SrcBitStream.pBsBuffer;
    GETFOURCC(SrcBitStream.pBsCurByte, iFourCCID)
    if( AVI_RIFF !=iFourCCID){
        #ifdef _IPP_DEBUG
        IPP_Printf("error: not AVI file\n");
        #endif
        IPP_Log(pLogFileName,"a","IPP Error:Parse_MJPEGFmAVI failed, the input file is not AVI file\n");
        ret = IPP_FAIL;
        goto END;
    }
    /* skip size syntax */
    if( 0!= IPP_Fseek(srcFile, 4, IPP_SEEK_CUR) ){
        IppStatusMessage(IPP_STATUS_ERR,err);
        IPP_Log(pLogFileName,"a","IPP Error:Parse_MJPEGFmAVI failed, %s\n", err);
        ret = IPP_FAIL;
        goto END;
    }
    /* 'AVI' */
    if(4!= IPP_Fread((void*)SrcBitStream.pBsBuffer, 1, 4, srcFile) ){
        IppStatusMessage(IPP_STATUS_ERR,err);
        IPP_Log(pLogFileName,"a","IPP Error:Parse_MJPEGFmAVI failed, %s\n", err);
        ret = IPP_FAIL;
        goto END;
    }
    SrcBitStream.pBsCurByte = SrcBitStream.pBsBuffer;
    GETFOURCC(SrcBitStream.pBsCurByte, iFourCCID)
    if( AVI_AVI !=iFourCCID){
        #ifdef _IPP_DEBUG
        IPP_Printf("error: not AVI file\n");
        #endif
        IPP_Log(pLogFileName,"a","IPP Error:Parse_MJPEGFmAVI failed, the input file is not AVI file\n");
        ret = IPP_FAIL;
        goto END;
    }
    /* SEEK to the beginning of the file */
    if( 0!= IPP_Fseek(srcFile, 0, IPP_SEEK_SET) ){
        IppStatusMessage(IPP_STATUS_ERR,err);
        IPP_Log(pLogFileName,"a","IPP Error:Parse_MJPEGFmAVI failed, %s\n", err);
        ret = IPP_FAIL;
        goto END;
    }
    
    while(!allFrameDecDone){  
        rtCode = Parse_MJPEGFmAVI(&pDecoderState, &SrcBitStream, &DstPicture);

        switch(rtCode){             
             case IPP_STATUS_NOERR:
                 allFrameDecDone = 1;
                 IPP_Log(pLogFileName,"a","IPP Status:Parse_MJPEGFmAVI Success!\n");
                 break;
             case IPP_STATUS_FRAME_COMPLETE:
                 iPicNum++;
                 if(SrcBitStream.pBsCurByte!=SrcBitStream.pBsBuffer){ /* exclude chunk size is 0*/
                     if(IPP_STATUS_NOERR
                         !=(rtCode=appiDecodeJPEGFromAVI(SrcBitStream.pBsBuffer, 
                         (Ipp32u)(SrcBitStream.pBsCurByte-SrcBitStream.pBsBuffer), &DstPicture, dstFile))){
                             IppStatusMessage(rtCode,err);
                             IPP_Log(pLogFileName,"a","IPP Error:Picture Num.%d JPEG Decoding failed, %s\n", iPicNum, err);
                             ret = IPP_FAIL;
                             goto END;
                     }else{
                         IPP_Log(pLogFileName,"a","IPP Status:Picture Num.%d JPEG Decoding success\n", iPicNum);
                         /* display one frame*/    
                         display_open(&DisplayCB, DstPicture.picWidth,DstPicture.picHeight);
                         display_frame(&DisplayCB, &DstPicture);
                         display_close();
                     }
                 }
                 break;
             case IPP_STATUS_BADARG_ERR:
             case IPP_STATUS_NOMEM_ERR:
             case IPP_STATUS_BITSTREAM_ERR:
             case IPP_STATUS_NOTSUPPORTED_ERR:
             case IPP_STATUS_ERR:
             default:
                 IppStatusMessage(rtCode,err);
                 IPP_Log(pLogFileName,"a","IPP Error:Parse_MJPEGFmAVI failed, %s\n", err);
                 ret = IPP_FAIL;
                 goto END;
        }

    }    
    IPP_StopPerfCounter(perf_index);    


    TotTime = IPP_GetPerfData(perf_index);
    g_Tot_Time[IPP_VIDEO_INDEX] = TotTime;
    g_Frame_Num[IPP_VIDEO_INDEX]  = iPicNum;
    IPP_FreePerfCounter(perf_index);

END:    
    if(NULL!=pDecoderState){
        DecoderFree_MJPEGFmAVI(&pDecoderState);
        pDecoderState = NULL;
    }

    if (NULL!=SrcBitStream.pBsBuffer) {
        pCallBackTable->fMemFree(&(SrcBitStream.pBsBuffer));
        SrcBitStream.pBsBuffer = NULL;
    }

    if (NULL!=srcFile) {
        IPP_Fclose(srcFile); 
        srcFile = NULL;
    }
    if (NULL!=dstFile) {
        IPP_Fclose(dstFile); 
        dstFile = NULL;
    }
    if(NULL!=pCallBackTable){
        miscFreeGeneralCallbackTable(&pCallBackTable); 
        pCallBackTable = NULL;
    }
    if(IPP_OK!=ret){
        IPP_Printf("MJPEG Decode Failed\n");
    }else{
        IPP_Printf("MJPEG Decode Success\n");
    }
    return ret;
}


/******************************************************************************
// Name:  ParseMJPEGDecCmd
// Description:			Parse the user command 
// Input Arguments:
//      pNextCmd      - Pointer to the input cmd buffer
// Output Arguments:	
//      ppSrcFileName - Pointer to the pointer to src file name
//      ppDstFileName - Pointer to the pointer to dst file name
//      ppLogFileName - Pointer to the pointer to log file name
// Returns:
//      [Success]		General IPP return code
//      [Failure]		General IPP return code					
******************************************************************************/
int ParseMJPEGDecCmd(char *pNextCmd, char **ppSrcFileName, char **ppDstFileName, char** ppLogFileName)
{
    int iLength = 0;
    int i = 0;
    char *pIdx, *pCurPos, *pEnd;
    char *pSrcFileName = NULL;
    char *pDstFileName = NULL;
    char *pLogFileName = NULL;

    /* Parse comments */
    if( '#'== pNextCmd[0] || '\t'== pNextCmd[0] || ' '== pNextCmd[0] || '\0' == pNextCmd[0] ) {
        return IPP_FAIL;
    }

    /* Replace the '\t' with ' 'in the cmd string */
    while(pNextCmd[i]) {
        if(pNextCmd[i] == '\t') {
            pNextCmd[i] = ' ';
        }
        i++;
    }

    pCurPos = pNextCmd;
    
    while((pIdx = IPP_Strstr(pCurPos, "-"))) {

        pEnd = IPP_Strstr(pIdx+1, " -");

        if(!pEnd) {
            pEnd = pNextCmd + IPP_Strlen(pNextCmd);
            iLength = (int)(pEnd - pIdx) - 3;
        } else {
            iLength = (int)(pEnd - pIdx) - 3;		
        }

        switch(pIdx[1]) {
        case 'i':
        case 'I':
            if(*ppSrcFileName==NULL){
                IPP_MemMalloc((void **)ppSrcFileName, 512, 4);
                if(*ppSrcFileName==NULL){
                    IPP_Printf ("Memory allocation for src file name error!\n");
                    return IPP_STATUS_ERR;
                }
            }
            pSrcFileName = *ppSrcFileName;
            IPP_Strncpy(pSrcFileName, pIdx + 3, iLength);
            pSrcFileName [iLength] = '\0';            
            break;
        case 'o':
        case 'O':
            if(*ppDstFileName==NULL){
                IPP_MemMalloc((void **)ppDstFileName, 512, 4);
                if(*ppDstFileName==NULL){
                    IPP_Printf ("Memory allocation for dst file name error!\n");
                    return IPP_STATUS_ERR;
                }
            }
            pDstFileName = *ppDstFileName;
            IPP_Strncpy(pDstFileName, pIdx + 3, iLength);
            pDstFileName [iLength] = '\0';            
            break;
        case 'l':
        case 'L':
            if(*ppLogFileName==NULL){
                IPP_MemMalloc((void **)ppLogFileName, 512, 4);
                if(*ppLogFileName==NULL){
                    IPP_Printf ("Memory allocation for log file name error!\n");
                    return IPP_STATUS_ERR;
                }
            }
            pLogFileName = *ppLogFileName;
            IPP_Strncpy(pLogFileName, pIdx + 3, iLength);
            pLogFileName [iLength] = '\0';
            break;
        default:
            return IPP_FAIL;
        }

        pCurPos = pEnd;
    }

    return IPP_OK;
}


/******************************************************************************
// Name:  CodecTest
// Description:			Main entry for MJPEG from AVI Container decoder test
// Input Arguments:
//      N/A
// Output Arguments:	
//      N/A
// Returns:
//      IPP_OK   - Run MJPEGDec
//      IPP_FAIL - Show Usage
******************************************************************************/
int CodecTest(int argc, char** argv)
{
    char *pSrcFileName = NULL;
    char *pDstFileName = NULL;
    char *pLogFileName = NULL;
	int rtCode = IPP_OK;

	DisplayLibVersion();

    if(argc == 2 && ParseMJPEGDecCmd(argv[1], &pSrcFileName, &pDstFileName, &pLogFileName) == 0){       
        rtCode = MJPEGFmAVIDec(pSrcFileName, pDstFileName,pLogFileName == NULL ? NULL : pLogFileName);
    }else{
        IPP_Log(pLogFileName,"w",
            "Command is incorrect!\nUsage:appMJPEGDec.exe \"-i:test.avi -o:test.yuv -l:test.log\"\n-i input file\n-o output file\n-l log file\n");
		rtCode = IPP_FAIL;
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
	return rtCode;

}
/* EOF */