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


/* Standard IPP headers */
#include <stdio.h>
#include <string.h>
#include "codecVC.h"
#include "codecWMV.h"
#include "misc.h"

#include "ippLV.h"

#define _OUTPUTYUV

/*RIM Specified RCV File Format*/
#define _RIM_RCV_

int main_WMVDec (char *cmpName, char* output, char *logfile);
int output_frame(IppPicture *dst, FILE* outputFile);
void	DisplayLibVersion(void);

#ifdef _IPP_LINUX
#include <sys/time.h>
struct timeval tv1, tv2;
struct timezone tz;
struct timespec start, end;
/*System time*/
unsigned long total_time;
/*Process time*/
unsigned long total_cpu; 

/*Process time*/
#define _NR_SYS_CLOCK_GETTIME 263
#define CLOCK_PROCESS_CPUTIME_ID 2
/*System time*/
#define GetTickCountStart()                                                           \
{   /*System time*/                                                                   \
    gettimeofday(&tv1, &tz);                                                          \
    syscall(_NR_SYS_CLOCK_GETTIME, CLOCK_PROCESS_CPUTIME_ID, &start); /*ProcessTime*/ \
}


#define GetTickCountEnd()                                                             \
{                                                                                     \
    gettimeofday(&tv2, &tz);                                                          \
    syscall(_NR_SYS_CLOCK_GETTIME, CLOCK_PROCESS_CPUTIME_ID, &end);/*Process timt*/   \
    total_time += (tv2.tv_sec - tv1.tv_sec) * 1000000 + (tv2.tv_usec - tv1.tv_usec);  \
    total_cpu += (end.tv_sec*1000000 + end.tv_nsec/1000- start.tv_sec*1000000 - start.tv_nsec/1000);   \
}

#define GetTickCountProfile(totalframe)                                                                \
{                                                                                                      \
    IPP_Printf("=====================Benchmark Data!=========================\n");                         \
    IPP_Printf("[Codec:] Frame number is : [%d] frames\n", (totalframe));                                  \
    IPP_Printf("[Codec:] Decoding System Time is : [%d] us\n", total_time);                                \
    IPP_Printf("[Codec:] Average FPS is : [%f] fps \n", (1000000.0 * (totalframe))/ total_time);             \
    IPP_Printf("[Codec:] CPU Time is : [%d] us\n", total_cpu);                                             \
    IPP_Printf("[Codec:] CPU%% is : [%f]\n", (100.0 * total_cpu) / total_time);                              \
    IPP_Printf("=====================Benchmark Data!=========================\n");                         \
}

#else /*No Profiling Here */
/*X86 Version*/
#define GetTickCountStart()
#define GetTickCountEnd()
#define GetTickCountProfile(totalframe)
#endif /*End of Profiling*/


/*Parsing Cmd line for MPEG4 Encoder.*/
#define MAX_PAR_NAME_LEN    1024
#define MAX_PAR_VALUE_LEN   2048
#define STRNCPY(dst, src, len) \
{\
    IPP_Strncpy((dst), (src), (len));\
    (dst)[(len)] = '\0';\
}

static char scHelpWMV9Dec[] = {
    "Marvell WMV9Dec Usage: \n"
    "     IPP Unit Test    :  ./App Input.cmp Output.yuv\n"
    "     Integration Test :  ./App \"-i:input.cmp -o:output.yuv -l:logfile\" \n"
};

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
	ret = GetLibVersion_VC1DEC(libversion,sizeof(libversion));
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
int ParseCmd_WMV9Dec(char *pCmdLine,    char *pSrcFileName, 
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
// Name:             main_WMVDec
// Description:      Entry function for WMV Simple Profile decoder
//
// Returns:          0:          decoding okay
//                   others:  decoding fail
*****************************************************************************/
int CodecTest(int argc, char ** argv)
{

    int     ret             = 0;
    char    fInput[128]     = {'\0'}; 
    char    fOutput[128]    = {'\0'};
    char    fLog[128]       = {'\0'};

    char    *pLog           = NULL;
    int     len             = NULL;

    IPP_Printf("Marvell WMV9 Decoder(Simple/Main), Be compatible with VC-1\n\n");

    /*Usage:*/
    IPP_Printf("%s\n", &scHelpWMV9Dec);

	DisplayLibVersion();

    if((argc == 2) && strchr(argv[1], ':') ){
        /*QA Cmd Input.*/
        ret = ParseCmd_WMV9Dec(argv[1], fInput, fOutput, fLog);
        if(0 != ret){
            IPP_Log(pLog,"a", "Error: ParseCmd_WMV9Dec \n");
            return ret;
        }
    }else if(argc == 3){
        /*Unit Test*/
        IPP_Strcpy(fInput, argv[1]);
        IPP_Strcpy(fOutput, argv[2]);

#ifdef _IPP_LINUX
    }else if(argc == 2){
        /*Manchanc on Linux*/
        IPP_Strcpy(fInput, argv[1]);
        IPP_Strcpy(fOutput, "/dev/null");
#endif
    }else{
        return -1;
    }

    len = IPP_Strlen(fLog);
    if(len){
        /*Init Log file*/
        IPP_MemMalloc(&pLog, len ,8);
        IPP_Memcpy(pLog, fLog, len);
    }

    if(0 != main_WMVDec(fInput, fOutput, pLog)){
        return -1;
    }

    if(pLog){
        IPP_MemFree(&pLog);
    }

    return 0;

}

int main_WMVDec (char *cmpName, char* output, char *logfile)
{
    IPP_FILE                    *fpin = NULL, *fpout = NULL;
    void                        *pWMVDecoderState = NULL;
    IppBitstream                srcBitStream;
    MiscGeneralCallbackTable    *pSrcCBTable = NULL;
    IppPicture                  dstPicture;
    IppCodecStatus              retCode = IPP_STATUS_NOERR, rc = IPP_STATUS_NOERR;   
    Ipp8u                       *pReadBuf= NULL;
    Ipp8u                       *pRead = NULL;
    Ipp32u                      iValue;

    int     dwNumFrames,rcv_version,codec_version,hdrext,ext_size,dwHeight, dwWidth;
    int     rcv_additional_header_size, pre_roll, bitrate, framerate, VBR, two_pass;
    int     framelen, dwMaxFrameSize, keyframe;
    int     i;

    int     frameNum =0;
    int     iReadSize = 0;
    Ipp8u * pHdrExt = NULL;

    /*Performance Profiling*/
    int             perf_index       = 0; 
    long long       llTotaltime      = 0;
    DISPLAY_CB      hDispCB          ;

    /*Performance and Display */
    IPP_GetPerfCounter(&perf_index, DEFAULT_TIMINGFUNC_START, DEFAULT_TIMINGFUNC_STOP);
    IPP_ResetPerfCounter(perf_index);

    fpin = IPP_Fopen(cmpName, "rb");
    if (!fpin) {
        IPP_Log(logfile,"a", "Error: IPP_Fopen %s \n", cmpName);
        return -1;
    }

    fpout = IPP_Fopen(output, "wb");
    if (!fpout) {
        IPP_Log(logfile,"a", "Warning: IPP_Fopen %s \n", output);
        //return -1;
    }

    srcBitStream.pBsBuffer = NULL;

    IPP_MemMalloc(&pReadBuf, 4, 8);
    pRead = pReadBuf;

    IPP_Fread(pReadBuf, 1, 4, fpin);
    iValue = (pRead[3] << 24)|(pRead[2] << 16)|(pRead[1] << 8)|(pRead[0]);
    dwNumFrames = iValue;

    rcv_version = (dwNumFrames>>30) & 0x1;

    codec_version = dwNumFrames>>24;

    dwNumFrames &= 0xffffff;

    hdrext=(codec_version>>7)&0x1;

    codec_version &= 0x3f;

    if(hdrext!=0) 
    {
        IPP_Fread(pReadBuf, 1, 4, fpin);
        iValue = (pRead[3] << 24)|(pRead[2] << 16)|(pRead[1] << 8)|(pRead[0]);
        ext_size = iValue;

        IPP_MemMalloc(&pHdrExt, ext_size, 8);
        if (pHdrExt == NULL){
            if(pReadBuf != NULL){
                IPP_MemFree(&pReadBuf);
            }
            IPP_Log(logfile,"a", "Error: IPP_MemMalloc Line %d\n", __LINE__);
            return -1;
        }

        IPP_Fread(pHdrExt, 1, ext_size, fpin);
    }

    /*
    //Get the height of the frames
    */
    IPP_Fread(pReadBuf, 1, 4, fpin);
    iValue = (pRead[3] << 24)|(pRead[2] << 16)|(pRead[1] << 8)|(pRead[0]);
    dwHeight = iValue;

    /*
    //Get the width of the frames
    */
    IPP_Fread(pReadBuf, 1, 4, fpin);
    iValue = (pRead[3] << 24)|(pRead[2] << 16)|(pRead[1] << 8)|(pRead[0]);
    dwWidth = iValue;

    /* 
    //side effect of using MyReadFile here is to preread the file before profiling starts below
    */
    if (rcv_version == 1) 
    {
        IPP_Fread(pReadBuf, 1, 4, fpin);
        iValue = (pRead[3] << 24)|(pRead[2] << 16)|(pRead[1] << 8)|(pRead[0]);
        rcv_additional_header_size = iValue;

        IPP_Fread(pReadBuf, 1, 4, fpin);
        iValue = (pRead[3] << 24)|(pRead[2] << 16)|(pRead[1] << 8)|(pRead[0]);
        pre_roll = iValue;

        VBR = (pre_roll>>28)&0x1;
        two_pass = (pre_roll>>29)&0x1;
        pre_roll &= 0x0fffffff;

        IPP_Fread(pReadBuf, 1, 4, fpin);
        iValue = (pRead[3] << 24)|(pRead[2] << 16)|(pRead[1] << 8)|(pRead[0]);
        bitrate = iValue;

        IPP_Fread(pReadBuf, 1, 4, fpin);
        iValue = (pRead[3] << 24)|(pRead[2] << 16)|(pRead[1] << 8)|(pRead[0]);
        framerate = iValue;
    } else 
    {
        bitrate   = 300;
    }

    /*Init Display for Manchac*/
    if(IPP_STATUS_NOERR != display_open(&hDispCB, dwWidth, dwHeight)){
        IPP_Log(logfile,"a", "Error: display \n");
        rc = IPP_STATUS_ERR;
		goto  EndOfDec2;
    }

    retCode = miscInitGeneralCallbackTable(&pSrcCBTable);
    if(IPP_STATUS_NOERR != retCode ){
        IPP_Log(logfile,"a", "Error: miscInitGeneralCallbackTable return Err %d \n", retCode);
        rc = retCode;
        goto EndOfDec2;
    }

    retCode = DecoderInitAlloc_WMVVideo(dwWidth, dwHeight, codec_version, 1, pHdrExt, ext_size, pSrcCBTable, &pWMVDecoderState);
    if(IPP_STATUS_INIT_OK != retCode ){
        IPP_Log(logfile,"a", "Error: DecoderInitAlloc_WMVVideo return Err %d \n", retCode);
        rc = retCode;
        goto EndOfDec;
    }

    /*Allocate Bitstream buffer*/
    IPP_MemMalloc(&srcBitStream.pBsBuffer, ((dwWidth+15)&~15) * ((dwHeight+15)&~15) * 3 /2, 8);
    dwMaxFrameSize = ( (dwWidth+15)&~15) * ((dwHeight+15)&~15) * 3 /2 ;

    for(i = 0; i < dwNumFrames; i++)
    {
        if(IPP_Feof(fpin)){
            goto EndOfDec;
        }

        IPP_Fread(pReadBuf, 1, 4, fpin);
        iValue = (pRead[3] << 24)|(pRead[2] << 16)|(pRead[1] << 8)|(pRead[0]);
        framelen = iValue;

        if (rcv_version == 1) 
        {
            keyframe = framelen>>31; 
            framelen &= 0x0fffffff;

            IPP_Fread(pReadBuf, 1, 4, fpin);
            iValue = (pRead[3] << 24)|(pRead[2] << 16)|(pRead[1] << 8)|(pRead[0]);

            if (framelen > dwMaxFrameSize){
                IPP_Log(logfile,"a", "Error: IPP_Fread FrameLen \n");
                rc = -2;
		        goto EndOfDec;
            }
        }
#ifdef _RIM_RCV_
        else{
            framelen &= 0x0fffffff;
        }
#endif

        iReadSize = IPP_Fread(srcBitStream.pBsBuffer, 1, framelen, fpin);
        if(iReadSize != framelen){
            IPP_Log(logfile,"a", "Warning: IPP_Fread Line: %d \n", __LINE__);
        }
        srcBitStream.pBsCurByte = srcBitStream.pBsBuffer;
        srcBitStream.bsByteLen = srcBitStream.bsCurBitOffset = 0;
        srcBitStream.bsByteLen = framelen;

        if(0){
            /*Get Next Picture Type*/
            int pType = 0;
            retCode = DecodeSendCmd_WMVVideo(IPPVC_GET_PICTURETYPE, &srcBitStream, &pType, pWMVDecoderState);
            if(IPP_STATUS_NOERR != retCode){
                IPP_Log(logfile,"a", "Error: DecodeSendCmd_WMVVideo: %d \n", retCode);
                rc = retCode;
                goto EndOfDec;
            }

            switch(pType)
            {
            case 0:
                IPP_Printf("pType IVOP\n");
                break;
            case 1:
                IPP_Printf("pType PVOP\n");
                break;
            case 2:
                IPP_Printf("pType BVOP\n");
                break;
            case 4:
                IPP_Printf("pType BIVOP\n");
                break;
            default:
                IPP_Printf("Error In Get Picture Type[%d]\n", pType);
                getchar();
                goto EndOfDec;
            }

        }

        /*
        //Decoding one frame
        */

        GetTickCountStart();
        IPP_StartPerfCounter(perf_index);

        retCode = Decode_WMVVideo(&srcBitStream, &dstPicture, pWMVDecoderState,0);

        IPP_StopPerfCounter(perf_index);
        GetTickCountEnd();

        if(IPP_STATUS_FRAME_COMPLETE != retCode){
            IPP_Log(logfile,"a", "Error: Decode_WMVVideo return %d \n", retCode);
            rc = retCode;
            goto EndOfDec;
        }

        frameNum ++;

        //IPP_Printf("Display [%d] %d.%d\n",frameNum, dwWidth,dwHeight );

        display_frame(&hDispCB, &dstPicture);/*Display on Manchanc*/

        if(0 != output_frame(&dstPicture, fpout)){
            IPP_Log(logfile,"a", "Error: output_frame \n");
            rc = retCode;
            goto EndOfDec;
        }
    }

    /*
    //Output Last Frame when it enable B frame in stream.
    */
    GetTickCountStart();
    IPP_StartPerfCounter(perf_index);

    retCode = Decode_WMVVideo(&srcBitStream, &dstPicture, pWMVDecoderState,1);

    IPP_StopPerfCounter(perf_index);
    GetTickCountEnd();

    if(IPP_STATUS_FRAME_COMPLETE == retCode){
        output_frame(&dstPicture, fpout);
        display_frame(&hDispCB, &dstPicture);/*Display on Manchanc*/
    }

    /*DeInit Display*/
    display_close();

    llTotaltime = IPP_GetPerfData(perf_index);
    g_Frame_Num[IPP_VIDEO_INDEX] = frameNum;
    g_Tot_Time[IPP_VIDEO_INDEX] = llTotaltime;

EndOfDec:

    GetTickCountProfile(frameNum);

    retCode = DecoderFree_WMVVideo(pWMVDecoderState);
    if(IPP_STATUS_NOERR != retCode){
        IPP_Log(logfile,"a", "Error: DecoderFree_WMVVideo return %d \n", retCode);
        rc = retCode;
    }

EndOfDec2:

    if(srcBitStream.pBsBuffer != NULL){
        IPP_MemFree(&srcBitStream.pBsBuffer);
    }
    if(pSrcCBTable != NULL){
        miscFreeGeneralCallbackTable(&pSrcCBTable);
    }

    if(pHdrExt != NULL){
        IPP_MemFree(&pHdrExt);
    }

    if(pReadBuf != NULL){
        IPP_MemFree(&pReadBuf);
    }

    /*Performance Data*/
    IPP_FreePerfCounter(perf_index);

    if(fpin){
        IPP_Fclose(fpin);
    }

    if(fpout){
        IPP_Fclose(fpout);
    }

    return rc;
}

int output_frame(IppPicture *dst, FILE* outputFile)
{
#ifdef _OUTPUTYUV
    int j = 0;
    char *yPlane, *uPlane, *vPlane;

    if(outputFile == NULL){
        return 0;
    }
    if(dst == NULL){
        //IPP_Printf("Error in output \n");
        return -1;
    }

    yPlane = dst->ppPicPlane[0];
    uPlane = dst->ppPicPlane[1];
    vPlane = dst->ppPicPlane[2];

    for(j = 0; j < dst->picHeight; j++){
        IPP_Fwrite(yPlane, 1, dst->picWidth, outputFile);
        yPlane += dst->picPlaneStep[0];
    }

    for(j = 0; j < dst->picHeight/2; j++){
        IPP_Fwrite(uPlane, 1, dst->picWidth/2, outputFile);
        uPlane += dst->picPlaneStep[1];
    }

    for(j = 0; j < dst->picHeight/2; j++){
        IPP_Fwrite(vPlane, 1, dst->picWidth/2, outputFile);
        vPlane += dst->picPlaneStep[2];
    }
#endif
    return 0;
}
