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
#include "dib.h"
#include <stddef.h>	// required by bmm
#include "bmm_lib.h"
#include "ippIE.h"
#include "gcu.h"

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
	ret = GetLibVersion_IppIE(libversion,sizeof(libversion));
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
// Name:			ParseUsmfilterCmd
// Description:			Parse the user command and update parameter structure.
// Input Arguments:
//		pCmd:	Pointer to the input cmd buffer
// Output Arguments:	
//		pSrcFileName: Pointer to current src file name
//		pDstFileName: Pointer to dst file name
//		pLogFileName: Pointer to log file name
//		pSharpeningPar:   Pointer to parameter structure
// Returns:
//     [Success]		General IPP return code
//     [Failure]		General IPP return code					
******************************************************************************/

static IppStatus ParseUsmfilterCmd(char *pNextCmd, char **ppSrcFileName, char **ppDstFileName, 
                               char **ppLogFileName, IppUsmfilterParam *pSharpeningPar)
{
    int iLength, i = 0;
    char *pIdx, *pCurPos, *pEnd;
    char *pSrcFileName = NULL;
    char *pDstFileName = NULL;
    char *pLogFileName = NULL;


    /* Parse comments */
    if(pNextCmd[0] == '#' || pNextCmd[0] == '\t'|| pNextCmd[0] == ' ' || pNextCmd[0] == '\0') {
        return ippStsErr;
    }

    /* set the default value */
    pSharpeningPar->fAmount = 5.0;
    pSharpeningPar->fRadius = 0.5;
    pSharpeningPar->iThreshold = 0;

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
            iLength = pEnd - pIdx - 3;
        } else {
            iLength = pEnd - pIdx - 3;		
        }


        switch(pIdx[1]) {
        case 'i':
        case 'I':
            if(*ppSrcFileName==NULL){
                IPP_MemCalloc((void **)ppSrcFileName, 512, 4);
                if(*ppSrcFileName==NULL){
                    IPP_Printf ("Memory allocation for src file name error!\n");
                    return ippStsMemAllocErr;
                }
            }
            pSrcFileName = *ppSrcFileName;
            /* If new image, state re-initialization is needed */
            IPP_Strncpy(pSrcFileName, pIdx + 3, iLength);
            pSrcFileName [iLength] = '\0';
            break;
        case 'o':
        case 'O':
            if(*ppDstFileName==NULL){
                IPP_MemCalloc((void **)ppDstFileName, 512, 4);
                if(*ppDstFileName==NULL){
                    IPP_Printf ("Memory allocation for dst file name error!\n");
                    return ippStsMemAllocErr;
                }
            }
            pDstFileName = *ppDstFileName;
            IPP_Strncpy(pDstFileName, pIdx + 3, iLength);
            pDstFileName [iLength] = '\0';
            break;
        case 'l':
        case 'L':
            if(*ppLogFileName==NULL){
                IPP_MemCalloc((void **)ppLogFileName, 512, 4);
                if(*ppLogFileName==NULL){
                    IPP_Printf ("Memory allocation for log file name error!\n");
                    return ippStsMemAllocErr;
                }
            }
            pLogFileName = *ppLogFileName;
            IPP_Strncpy(pLogFileName, pIdx + 3, iLength);
            pLogFileName [iLength] = '\0';
            break;
        case 'a':
        case 'A':
            pSharpeningPar->fAmount = IPP_Atof(pIdx+3);
            if( (pSharpeningPar->fAmount <= 0) || (pSharpeningPar->fAmount >10.0) ){
                IPP_Printf ("Amount shoud be in the range (0,10.0] !\n");
                return ippStsErr;
            }
            break;
        case 'r':
        case 'R':
            pSharpeningPar->fRadius = IPP_Atof(pIdx+3);
            if( (pSharpeningPar->fRadius <= 0) || (pSharpeningPar->fRadius >3.5) ){
                IPP_Printf ("Radius shoud be in the range (0,3.5] !\n");
                return ippStsErr;
            }
            break;
        case 't':
        case 'T':
            pSharpeningPar->iThreshold = IPP_Atoi(pIdx+3);		
            if( (pSharpeningPar->iThreshold < 0) || (pSharpeningPar->iThreshold >255) ){
                IPP_Printf ("Threshold shoud be in the range [0,255] !\n");
                return ippStsErr;
            }
            break;
        default:
            return ippStsErr;
        }

        pCurPos = pEnd;
    }
    return ippStsNoErr;
}



/******************************************************************************
// Name:				Usmfilter
// Description:			Main entry for Usmfilter
// Input Arguments:
//     pCmd  - Pointer to the command line
// Output Arguments:
// Returns:
//     None
******************************************************************************/
int Usmfilter(char *pCmd)
{
    IPP_FILE *srcFile = NULL, *dstFile = NULL;
    MDIBSPEC dibSpec;
    MiscGeneralCallbackTable *pCallBackTable = NULL;
    IppUsmfilterParam UsmfilterPar;
    char *pSrcFileName = NULL;
    char *pDstFileName = NULL;
    char *pLogFileName = NULL;
    IppStatus rtCode; 
    int ret = IPP_OK;
    Ipp8u *pSrcRGB = NULL, *pDstRGB = NULL, *pTemp = NULL;
    int iWidth, iHeight, iBpp, nClrMode;
    unsigned int iPhAddrSrcRGB, iPhAddrDstRGB;
    int i,j;
    GCU_INIT_DATA       initData;


    int total_perf_index = 0;
    long long nTotTime = 0;

    IPP_GetPerfCounter(&total_perf_index, DEFAULT_TIMINGFUNC_START, DEFAULT_TIMINGFUNC_STOP);
    IPP_ResetPerfCounter(total_perf_index);

    IPP_Printf("Usmfilter start\n");

    /* Commamd line check */
    if( (IPP_Strlen(pCmd)) <= 1) {
        IPP_Log(pLogFileName,"a","IPP Error:command error\n");  
        IPP_Printf("Usmfilter command line is too short\n");
        return IPP_FAIL;
    }

    /* Parse Command Line */
    if (ippStsNoErr!=ParseUsmfilterCmd(pCmd, &pSrcFileName, &pDstFileName, &pLogFileName, &UsmfilterPar) ) {
        IPP_Log(pLogFileName,"a","IPP Error:command error\n");  
        IPP_Printf("Usmfilter parse cmd line Failed\n");
        ret = IPP_FAIL;
        goto Usmfilter_Done;   
    }

    IPP_Printf("amout = %f, radius = %f, threshold = %d\n", UsmfilterPar.fAmount, UsmfilterPar.fRadius, UsmfilterPar.iThreshold);

    /* Open Src and Dst File */
    if(NULL==pSrcFileName){
        IPP_Log(pLogFileName,"a","IPP Error: input source file is NULL\n");
        IPP_Printf("Usmfilter Failed:input source file is NULL\n");
        ret = IPP_FAIL;
        goto Usmfilter_Done;
    }else{
        IPP_Printf ("Source File:      %s \n", pSrcFileName);
        srcFile=IPP_Fopen(pSrcFileName, "rb");
        if( NULL==srcFile ){
            IPP_Log(pLogFileName,"a","IPP Error:can not open src file :%s\n",pSrcFileName);  
            IPP_Printf("Usmfilter Failed:can not open src file\n");
            ret = IPP_FAIL;
            goto Usmfilter_Done;
        }
    }

    if(NULL==pDstFileName){
        IPP_Printf ("Destination File: NULL \n");
        IPP_Log(pLogFileName,"a","IPP Status: dst file is NULL\n");  
    }else{
        IPP_Printf ("Destination File: %s \n", pDstFileName);
        dstFile=IPP_Fopen(pDstFileName, "wb");
        if( NULL==dstFile ){
            IPP_Log(pLogFileName,"a","IPP Status:can not open dst file :%s\n",pDstFileName);  
        }
    }

    /* Read bitmap file */   
    IPP_Memset(&dibSpec, 0, sizeof(MDIBSPEC));
    rtCode = ReadDIBFile(&dibSpec, srcFile);
    if (ippStsNoErr!=rtCode) {
        IPP_Log(pLogFileName,"a","IPP Error:read dib file, error\n");  
        ret = IPP_FAIL;
        goto Usmfilter_Done;
    }

    /* Alloc src and dst RGB buffes */
    iWidth = dibSpec.nWidth;
    iHeight = dibSpec.nHeight;
    nClrMode = dibSpec.nClrMode;

    /* show bitmap info */
    IPP_Printf("width = %d, height = %d\n", iWidth, iHeight);  

    /* SRC IMAGE FORMAT CHECK */
    if( (FORMAT_BGR888 != nClrMode) && (FORMAT_BGRA != nClrMode) ){
            if(NULL!=dibSpec.pBitmapData){
                IPP_MemFree((void**)&dibSpec.pBitmapData);
                dibSpec.pBitmapData = NULL;
            }
            IPP_Log(pLogFileName,"a","IPP Error:not supported format\n");
            IPP_Printf("Usmfilter not support format = %d\n", nClrMode);
            ret = IPP_FAIL;
            goto Usmfilter_Done; 
    }
    if( (iWidth%16) != 0 ){
        if(NULL!=dibSpec.pBitmapData){
                IPP_MemFree((void**)&dibSpec.pBitmapData);
                dibSpec.pBitmapData = NULL;
            }
            IPP_Log(pLogFileName,"a","IPP Error:not supported width \n");
            IPP_Printf("Usmfilter not support width = %d\n", iWidth);
            ret = IPP_FAIL;
            goto Usmfilter_Done; 
    }

    pTemp = (Ipp8u*)bmm_malloc(iWidth * iHeight * 4, BMM_ATTR_DEFAULT);        
    pSrcRGB = (Ipp8u*)bmm_malloc(iWidth * iHeight * 4, BMM_ATTR_DEFAULT);
    pDstRGB = (Ipp8u*)bmm_malloc(iWidth * iHeight * 4, BMM_ATTR_DEFAULT);
    iPhAddrSrcRGB = (unsigned int)bmm_get_paddr(pSrcRGB);
    iPhAddrDstRGB = (unsigned int)bmm_get_paddr(pDstRGB);
    if( (NULL==pTemp) || (NULL==pSrcRGB) || (NULL==pDstRGB) ){
        IPP_Log(pLogFileName,"a","IPP Error:failed to alloc src and dst rgb buffers\n");
        IPP_Printf("Usmfilter alloc src and dst rgb buffers Failed\n");
        ret = IPP_FAIL;
        goto Usmfilter_Done; 
    }
    IPP_Memset(pSrcRGB, 0, iWidth * iHeight * 4);
    IPP_Memset(pDstRGB, 0, iWidth * iHeight * 4);


    IPP_Printf("read BGR data\n");
    if(NULL!=dibSpec.pBitmapData){
        /* Other format already convert to BGR888 in dib.c*/
        if(FORMAT_BGR888==nClrMode){
            Ipp8u *pBitmapData = dibSpec.pBitmapData;
            int nStep = dibSpec.nStep;
            for (i=0 ; i<iHeight ; i++){   
                for (j=0 ; j<iWidth ; j++){
                    pSrcRGB[(i*iWidth + j) * 4 + 0] = pBitmapData[i*nStep+j * 3 + 0];
                    pSrcRGB[(i*iWidth + j) * 4 + 1] = pBitmapData[i*nStep+j * 3 + 1];
                    pSrcRGB[(i*iWidth + j) * 4 + 2] = pBitmapData[i*nStep+j * 3 + 2];
                    pSrcRGB[(i*iWidth + j) * 4 + 3] = 0x00;
                }
            }
        }else if(FORMAT_BGRA==nClrMode){
            Ipp8u *pBitmapData = dibSpec.pBitmapData;
            int nStep = dibSpec.nStep;
            for (i=0 ; i<iHeight ; i++){   
                for (j=0 ; j<iWidth ; j++){
                    pSrcRGB[(i*iWidth + j) * 4 + 0] = pBitmapData[i*nStep+j * 4 + 0];
                    pSrcRGB[(i*iWidth + j) * 4 + 1] = pBitmapData[i*nStep+j * 4 + 1];
                    pSrcRGB[(i*iWidth + j) * 4 + 2] = pBitmapData[i*nStep+j * 4 + 2];
                    pSrcRGB[(i*iWidth + j) * 4 + 3] = pBitmapData[i*nStep+j * 4 + 3];
                }
            }
        } 
        IPP_MemFree((void**)&dibSpec.pBitmapData);
        dibSpec.pBitmapData = NULL;
    }else{
        if(FORMAT_BGR888==nClrMode){
            Ipp32u iPaddedWidth = IIP_WIDTHBYTES_4B(iWidth*3*8)-iWidth*3;
            Ipp8u tmpBuf[4];
            for (i=0 ; i< iHeight ; i++){
                if( (iWidth * 3) != IPP_Fread(pTemp + iWidth * 3 * i, 1, iWidth * 3, srcFile)){
                    IPP_Log(pLogFileName,"a","IPP Error:load image failed\n");
                    IPP_Printf("Usmfilter load image failed\n");
                    ret = IPP_FAIL;
                    goto Usmfilter_Done;
                }
                /* BGR888 to BGRA */
                for (j=0 ; j<iWidth ; j++){
                    pSrcRGB[(i*iWidth + j) * 4 + 0] = pTemp[(i*iWidth+j) * 3 + 0];
                    pSrcRGB[(i*iWidth + j) * 4 + 1] = pTemp[(i*iWidth+j) * 3 + 1];
                    pSrcRGB[(i*iWidth + j) * 4 + 2] = pTemp[(i*iWidth+j) * 3 + 2];
                    pSrcRGB[(i*iWidth + j) * 4 + 3] = 0x00;
                }
                IPP_Fseek(srcFile, iPaddedWidth, IPP_SEEK_CUR); 
            }                
        }else if(FORMAT_BGRA==nClrMode){
            Ipp32u iPaddedWidth = IIP_WIDTHBYTES_4B(iWidth*4*8)-iWidth*4;
            Ipp8u tmpBuf[4];
            for (i=0 ; i< iHeight ; i++){
                if( (iWidth * 4) != IPP_Fread(pSrcRGB + iWidth * 4 * i, 1, iWidth * 4, srcFile)){
                    IPP_Log(pLogFileName,"a","IPP Error:load image failed\n");
                    IPP_Printf("Usmfilter load image failed\n");
                    ret = IPP_FAIL;
                    goto Usmfilter_Done;
                }
                IPP_Fseek(srcFile, iPaddedWidth, IPP_SEEK_CUR); 
            }
        }else{
            IPP_Log(pLogFileName,"a","IPP Status:not supported format\n");
            IPP_Printf("Usmfilter not support format\n");
            ret = IPP_FAIL;
            goto Usmfilter_Done;
        }
    }

    /* GCU init */
	IPP_Memset(&initData, 0, sizeof(initData));
	initData.version = GCU_VERSION_1_0;
	gcuInitialize(&initData);

    IPP_Printf("init GCU\n");

    IPP_StartPerfCounter(total_perf_index);
    /* USM Sharpen */
    rtCode = ippiUSMSharpen(pSrcRGB, iPhAddrSrcRGB, iWidth * 4, FORMAT_BGRA,
        pDstRGB, iPhAddrDstRGB, iWidth * 4, FORMAT_BGRA,
        iWidth, iHeight, UsmfilterPar.fRadius, UsmfilterPar.iThreshold, UsmfilterPar.fAmount, NULL, 0, NULL);    
    if(ippStsNoErr != rtCode){
        IPP_Log(pLogFileName,"a","IPP Error:Usmfilter Sharpen failed\n");  
        IPP_Printf("Usmfilter Sharpen failed\n");
        switch(rtCode){
            case ippStsBadArgErr:
                IPP_Printf("bad argument\n");
                break;
            case ippStsNullPtrErr:
                IPP_Printf("input NULL pointer\n");
                break;
            case ippStsNoMemErr:
                IPP_Printf("no memory\n");
                break;
            default:
                IPP_Printf("unspecific error\n");
                break;
        }
        ret = IPP_FAIL;
        goto Usmfilter_Done;
    }
    IPP_StopPerfCounter(total_perf_index);

    IPP_Printf("finish USM filtering\n");

    /* GCU terminate */
    gcuTerminate();

    IPP_Printf("GCU  terminate\n");

    nTotTime = IPP_GetPerfData(total_perf_index);
    g_Tot_Time[IPP_VIDEO_INDEX] = nTotTime;
    g_Frame_Num[IPP_VIDEO_INDEX]  = 1;

    IPP_Printf("Total Usmfilter Time:");
    IPP_Printf("%lld\n", nTotTime);

    /* write output file */
    if(NULL!=dstFile){            
        Ipp32u iPaddedWidth = IIP_WIDTHBYTES_4B(iWidth*3*8)-iWidth*3;
        /* Padding */
        for (i=0 ; i<iHeight ; i++){   
            for (j=0 ; j<iWidth ; j++)
            {
                pTemp[(i*iWidth+j) * 3 + 0] = pDstRGB[(i*iWidth + j) * 4 + 0];
                pTemp[(i*iWidth+j) * 3 + 1] = pDstRGB[(i*iWidth + j) * 4 + 1];
                pTemp[(i*iWidth+j) * 3 + 2] = pDstRGB[(i*iWidth + j) * 4 + 2];
            }
            for(j=0; j<iPaddedWidth; j++){
                pTemp[iWidth*3+j] = 0;
            }
        }

        /* IPP_Printf("Usmfilter output ready\n"); */
        dibSpec.nClrMode = FORMAT_BGR888;
        dibSpec.nBitsPerpixel = 24;
        dibSpec.nNumComponent = 3;
        dibSpec.pBitmapData = pTemp;
        dibSpec.nDataSize = IIP_WIDTHBYTES_4B(iWidth*3*8) * iHeight;
        if (IPP_STATUS_NOERR != WriteDIBFile(&dibSpec, dstFile)) {
            IPP_Log(pLogFileName,"a","IPP Error:destination file write error\n");  
            ret = IPP_FAIL;
            dibSpec.pBitmapData = NULL;
            goto Usmfilter_Done;
        }
        dibSpec.pBitmapData = NULL;
        IPP_Printf("write output to file\n");
    }

Usmfilter_Done:    
    /* Free stream read perf index */
    IPP_FreePerfCounter(total_perf_index);

    //IPP_Printf("free perf counter\n");

    /* close files */
    if(NULL!=srcFile){
        IPP_Fclose(srcFile);
        srcFile = NULL;
    }
    if(NULL!=dstFile){
        IPP_Fclose(dstFile);
        dstFile = NULL;
    }

    //IPP_Printf("close files\n");

    /* free memory */
    if(NULL != pSrcFileName ){
        IPP_MemFree((void**)&pSrcFileName);
        pSrcFileName = NULL;
    }
    if(NULL != pDstFileName ){
        IPP_MemFree((void**)&pDstFileName);
        pDstFileName = NULL;
    }
    if(NULL != pLogFileName ){
        IPP_MemFree((void**)&pLogFileName);
        pLogFileName = NULL;
    } 

    //IPP_Printf("free file name memory\n");

    if(NULL!=pTemp){
        bmm_free(pTemp);        
        pTemp = NULL;
    }
    if(NULL!=pSrcRGB){
        bmm_free(pSrcRGB);
        pSrcRGB = NULL;
    }
    if(NULL!=pDstRGB){
        bmm_free(pDstRGB);
        pDstRGB = NULL;
    }

    //IPP_Printf("free bgr memory\n");

    /* final result */
    if(IPP_OK!=ret){
        IPP_Printf("Usmfilter Failed\n");
    }else{
        IPP_Printf("Usmfilter Success\n");
    }

    return ret;
}


/******************************************************************************
// Name:  CodecTest
// Description:			Main entry for Usmfilter test
// Input Arguments:
//      N/A
// Output Arguments:	
//      N/A
// Returns:
//      IPP_OK   - Run Usmfilter
//      IPP_FAIL - Show Usage
******************************************************************************/
int CodecTest(int argc, char** argv)
{
    int ret = IPP_OK;
	DisplayLibVersion();
    if(argc == 2){       
        ret = Usmfilter(argv[1]);
        return ret;
    }else{
        IPP_Log(NULL,"w",
            "Command is incorrect! \n\
            Usage:appSharpening.exe \"-i:test.bmp -o:test.bmp -l:test.log -a:5.0 -r:0.5 -t:1.0\"\n\
            -i Input file\n\
            -o Output file\n\
            -l Log file\n\
            -a amount, range (0,10.0]\n \
            -r radius, range (0,3.5]\n \
            -t threshold, range [0,255]\n\
            Note:the width of input image should be multiple of 4, as GCU needs the step(width*4) to be multiple of 16\n\
            ");
        return IPP_FAIL;
    }    

}
/* EOF */


