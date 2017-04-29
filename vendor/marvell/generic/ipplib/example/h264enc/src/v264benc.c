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


#include "codecVC.h"
#include "misc.h"

#include "ippLV.h"

#define H264_ENC_PARA_NUM 36

typedef struct {
    char *name;
    void *p;
} ParaTable;

static H264EncoderParSet g_H264EncParSet;

static ParaTable Map[H264_ENC_PARA_NUM]={
    {"iWidth",                       &g_H264EncParSet.iWidth                        },
    {"iHeight",                      &g_H264EncParSet.iHeight                       },
    {"iQpFirstFrame",                &g_H264EncParSet.iQpFirstFrame                 },
    {"iPBetweenI",                   &g_H264EncParSet.iPBetweenI                    },
    {"iQcIndexOffset",               &g_H264EncParSet.iQcIndexOffset                },
    {"nNumberReferenceFrames",       &g_H264EncParSet.nNumberReferenceFrames        },
    {"profileIdc",                   &g_H264EncParSet.profileIdc                    },
    {"levelIdc",                     &g_H264EncParSet.levelIdc                      },
    {"bRCEnable",                    &g_H264EncParSet.bRCEnable                     },
    {"iFrameRate",                   &g_H264EncParSet.iFrameRate                    },
    {"iRCBitRate",                   &g_H264EncParSet.iRCBitRate                    },
    {"iDelayLimit",                  &g_H264EncParSet.iDelayLimit                   },
    {"iRCType",                      &g_H264EncParSet.iRCType                       },
    {"bDeblockEnable",               &g_H264EncParSet.bDeblockEnable                },
    {"iLoopFilterAlphaC0OffsetDiv2", &g_H264EncParSet.iLoopFilterAlphaC0OffsetDiv2  },
    {"iLoopFilterBetaOffsetDiv2",    &g_H264EncParSet.iLoopFilterBetaOffsetDiv2     },
    {"disableDeblockFilterIdc",      &g_H264EncParSet.disableDeblockFilterIdc       },
    {"bConstrainedIntraPredFlag",    &g_H264EncParSet.bConstrainedIntraPredFlag     },
    {"bIntra4x4PredModeEnable",      &g_H264EncParSet.bIntra4x4PredModeEnable       },
    {"bHardmardEnable",              &g_H264EncParSet.bHardmardEnable               },
    {"b8x8BlockSplitEnable",         &g_H264EncParSet.b8x8BlockSplitEnable          },
    {"b4x4BlockSplitEnable",         &g_H264EncParSet.b4x4BlockSplitEnable          },
    {"bHalfPixelSearchEnable",       &g_H264EncParSet.bHalfPixelSearchEnable        },
    {"bQuarterPixelSearchEnable",    &g_H264EncParSet.bQuarterPixelSearchEnable     },
    {"bSubPixelSearchFastMode",      &g_H264EncParSet.bSubPixelSearchFastMode       },
    {"i16x16SearchRange",            &g_H264EncParSet.i16x16SearchRange             },
    {"i8x8SearchRange",              &g_H264EncParSet.i8x8SearchRange               },
    {"i4x4SearchRange",              &g_H264EncParSet.i4x4SearchRange               },
    {"iSliceLength",                 &g_H264EncParSet.iSliceLength                  },
    {"nNumSliceGroups",              &g_H264EncParSet.nNumSliceGroups               },
    {"nFMOType",                     &g_H264EncParSet.nFMOType                      },
    {"iPictureComplexity",           &g_H264EncParSet.iPictureComplexity            },
    {"nQualityLevel",                &g_H264EncParSet.nQualityLevel                 },
    {"nSubPixelRefineLevel",         &g_H264EncParSet.nSubPixelRefineLevel          },
    {"nModeDecisionEarlyStopEnable", &g_H264EncParSet.nModeDecisionEarlyStopEnable  },
    {"iRCMaxBitRate",				 &g_H264EncParSet.iRCMaxBitRate                 }
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
	ret = GetLibVersion_H264ENC(libversion,sizeof(libversion));
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
// Name:             ParseItem
// Description:      parse a item from input command line
//
// Input Arguments:	
//      content		    - command line
//      pIdx		    - current position
//      len		        - the length of command line
//
//	Output Arguments:
//      item            - parsed item
//
// Returns:
//      int             - parameter index
******************************************************************************/
int ParseItem(char *content, int *pIdx, int len, char *item)
{
    int i;
    int start = 0;
    i = 0;
    while (*pIdx < len){
        if ((content[*pIdx] != ' ') && (content[*pIdx] != '\t')){
            item[i++] = content[(*pIdx)];
            (*pIdx)++;
            start = 1;
        } else {
            (*pIdx)++;
            if (1 == start){
                break;
            }
        }
    }
    item[i] = '\0';

    return start;
}

/******************************************************************************
// Name:             GetParaIndex
// Description:      get the parameter index according to parameter name
//
// Input Arguments:	
//      ParaName		- parameter name
//
//	Output Arguments:
//
// Returns:
//      int             - parameter index
******************************************************************************/
int GetParaIndex(char *ParaName)
{
    int i;

    for (i = 0; i < H264_ENC_PARA_NUM; i++){
        if (0 == IPP_Strcmp(ParaName, Map[i].name)){
            return i;
        }
    }
    return i;
}

/******************************************************************************
// Name:             ParaConfig
// Description:      parse config file to get encoding parameters
//
// Input Arguments:	
//      f		        - Config file
//
//	Output Arguments:
//		pPar	        - Encoding parameter
//
// Returns:
//      1               - success
******************************************************************************/
int ReadConfigFile(IPP_FILE *f)
{
    int rtCode;
    char content[2048];
    char name[256];
    char value[256];
    int len;
    int nIdx, nParaIdx;
    int *p;
    char *rt;
    while (!IPP_Feof(f)){
        rt = IPP_Fgets(content, 512, f);
        if (NULL != rt){
            if (content[0] == '\0' || content[0] == '#' || content[0] == '\n'){
                /*comments or useless content*/
            } else {
                len = IPP_Strlen(content);
                nIdx = 0;
                rtCode = ParseItem(content, &nIdx, len, name);
                if (0 != rtCode){
                    nParaIdx = GetParaIndex(name);
                    if (nParaIdx < H264_ENC_PARA_NUM){
                        /* '=' */
                        ParseItem(content, &nIdx, len, value);
                        /* value */
                        rtCode = ParseItem(content, &nIdx, len, value);
                        if (0 != rtCode ){
                            p = (int *)Map[nParaIdx].p;
                            *p = IPP_Atoi(value);
                        }
                    }
                }
            }
        }
    }

    return 1;
}

/******************************************************************************
// Name:             InitPara
// Description:      initialize encoding parameter
//
// Input Arguments:	
//		pPar	        - Encoding parameter
//
//	Output Arguments:
//
// Returns:
//   
******************************************************************************/
void InitPara(H264EncoderParSet *pPar)
{
    IPP_Memset(pPar, 0, sizeof(H264EncoderParSet));
    pPar->iWidth                            = 176;
    pPar->iHeight                           = 144;
    pPar->iFrameRate                        = 30;
    pPar->iQpFirstFrame                     = 30;
    pPar->iPBetweenI                        = 29;
    pPar->iQcIndexOffset                    = 0;
    pPar->nNumberReferenceFrames            = 1;
    pPar->profileIdc                        = H264_BASELINE_PROFILE;
    pPar->levelIdc                          = 30;
    pPar->bRCEnable                         = 1;
    pPar->iRCBitRate                        = 512000;
    pPar->iRCMaxBitRate                     = 512000*3/2;
    pPar->iDelayLimit                       = 500;
    pPar->iRCType                           = 0;
    pPar->bDeblockEnable                    = 1;
    pPar->iLoopFilterAlphaC0OffsetDiv2      = 0;
    pPar->iLoopFilterBetaOffsetDiv2         = 0;
    pPar->disableDeblockFilterIdc           = 0;
    pPar->bConstrainedIntraPredFlag         = 0;
    pPar->bIntra4x4PredModeEnable           = 1;
    pPar->bHardmardEnable                   = 1;
    pPar->b8x8BlockSplitEnable              = 1;
    pPar->b4x4BlockSplitEnable              = 1;
    pPar->bHalfPixelSearchEnable            = 1;
    pPar->bQuarterPixelSearchEnable         = 1;
    pPar->bSubPixelSearchFastMode           = 1;
    pPar->i16x16SearchRange                 = 16;
    pPar->i8x8SearchRange                   = 16;
    pPar->i4x4SearchRange                   = 16;
    pPar->iSliceLength                      = 0;
    pPar->nNumSliceGroups                   = 0;
    pPar->nFMOType                          = 0;
    pPar->iPictureComplexity                = 0;

    pPar->nQualityLevel                     = 0;
    pPar->nSubPixelRefineLevel              = 2;
    pPar->nModeDecisionEarlyStopEnable      = 1;
}

/******************************************************************************
// Name:             OutputPara
// Description:      display encoding setting
//
// Input Arguments:	
//		pPar	        - Input parameters
//
//	Output Arguments:
//
// Returns:
//   
******************************************************************************/
void OutputPara(H264EncoderParSet *pPar)
{
    IPP_Printf("#############################################################\n");
    IPP_Printf("#            Encoding Parameter Setting:                    #\n");
    IPP_Printf("#############################################################\n");
    IPP_Printf("Width                           %d\n", pPar->iWidth);
    IPP_Printf("Height                          %d\n", pPar->iHeight);
    IPP_Printf("FrameRate                       %d\n", pPar->iFrameRate);
    IPP_Printf("QpFirstFrame                    %d\n", pPar->iQpFirstFrame);
    IPP_Printf("PBetweenI                       %d\n", pPar->iPBetweenI);
    IPP_Printf("QcIndexOffset                   %d\n", pPar->iQcIndexOffset);
    IPP_Printf("NumberReferenceFrames           %d\n", pPar->nNumberReferenceFrames);
    IPP_Printf("profileIdc                      %d\n", pPar->profileIdc);
    IPP_Printf("levelIdc                        %d\n", pPar->levelIdc);
    IPP_Printf("RCEnable                        %d\n", pPar->bRCEnable);
    IPP_Printf("RCBitRate                       %d\n", pPar->iRCBitRate);
	IPP_Printf("RCMaxBitRate                    %d\n", (int)((pPar->iRCMaxBitRate) < (pPar->iRCBitRate*3/2) ? pPar->iRCBitRate*3/2 : pPar->iRCMaxBitRate));
    IPP_Printf("DelayLimit                      %d\n", pPar->iDelayLimit);
    IPP_Printf("RCType                          %d\n", pPar->iRCType);
    IPP_Printf("DeblockEnable                   %d\n", pPar->bDeblockEnable);
    IPP_Printf("LoopFilterAlphaC0OffsetDiv2     %d\n", pPar->iLoopFilterAlphaC0OffsetDiv2);
    IPP_Printf("LoopFilterBetaOffsetDiv2        %d\n", pPar->iLoopFilterBetaOffsetDiv2);
    IPP_Printf("disableDeblockFilterIdc         %d\n", pPar->disableDeblockFilterIdc);
    IPP_Printf("ConstrainedIntraPredFlag        %d\n", pPar->bConstrainedIntraPredFlag);
    IPP_Printf("Intra4x4PredModeEnable          %d\n", pPar->bIntra4x4PredModeEnable);
    IPP_Printf("HardmardEnable                  %d\n", pPar->bHardmardEnable);
    IPP_Printf("8x8BlockSplitEnable             %d\n", pPar->b8x8BlockSplitEnable);
    IPP_Printf("4x4BlockSplitEnable             %d\n", pPar->b4x4BlockSplitEnable);
    IPP_Printf("HalfPixelSearchEnable           %d\n", pPar->bHalfPixelSearchEnable);
    IPP_Printf("QuarterPixelSearchEnable        %d\n", pPar->bQuarterPixelSearchEnable);
    IPP_Printf("SubPixelSearchFastMode          %d\n", pPar->bSubPixelSearchFastMode);
    IPP_Printf("16x16SearchRange                %d\n", pPar->i16x16SearchRange);
    IPP_Printf("8x8SearchRange                  %d\n", pPar->i8x8SearchRange);
    IPP_Printf("4x4SearchRange                  %d\n", pPar->i4x4SearchRange);
    IPP_Printf("SliceLength                     %d\n", pPar->iSliceLength);
    IPP_Printf("NumSliceGroups                  %d\n", pPar->nNumSliceGroups);
    IPP_Printf("FMOType                         %d\n", pPar->nFMOType);
    IPP_Printf("PictureComplexity               %d\n", pPar->iPictureComplexity);
    IPP_Printf("QualityLevel                    %d\n", pPar->nQualityLevel);
    IPP_Printf("SubPixelRefineLevel             %d\n", pPar->nSubPixelRefineLevel);
    IPP_Printf("ModeDecisionEarlyStopEnable     %d\n", pPar->nModeDecisionEarlyStopEnable);
    IPP_Printf("#############################################################\n");
    IPP_Printf("#############################################################\n");
}


/******************************************************************************
// Name:             OutputRecPicture
// Description:      output reconstruction picture
//
// Input Arguments:	
//      fprec		    - Reconstruction file
//		pRecPicture	    - Reconstruction picture
//
//	Output Arguments:
//
// Returns:
//   
******************************************************************************/
void OutputRecPicture(IPP_FILE *fprec, IppPicture *pRecPicture)
{
    Ipp8u *pRecData;
    int nLine;
    pRecData = pRecPicture->ppPicPlane[0];
    for (nLine = 0; nLine < pRecPicture->picHeight; nLine++) {
        IPP_Fwrite(pRecData, 1, pRecPicture->picWidth, fprec);
        pRecData += pRecPicture->picPlaneStep[0];
    }

    pRecData = pRecPicture->ppPicPlane[1];
    for (nLine = 0; nLine < (pRecPicture->picHeight / 2); nLine++) {
        IPP_Fwrite(pRecData, 1, pRecPicture->picWidth/2, fprec);
        pRecData += pRecPicture->picPlaneStep[1];
    }

    pRecData = pRecPicture->ppPicPlane[2];
    for (nLine = 0; nLine < (pRecPicture->picHeight / 2); nLine++) {
        IPP_Fwrite(pRecData, 1, pRecPicture->picWidth/2, fprec);
        pRecData += pRecPicture->picPlaneStep[2];
    }
}

/******************************************************************************
// Name:             CalculatePSNRY
// Description:      calculate PSNR
//
// Input Arguments:	
//      pRecPicture		- Reconstruction picture
//		pSrcPicture	    - Original picture
//
//	Output Arguments:
//
// Returns:
//      PSNR
******************************************************************************/
double CalculatePSNRY(IppPicture *pRecPicture, IppPicture *pSrcPicture)
{
    double dis;
    Ipp8u  *pSrcData, *pRecData;
    int i, nLine;
    double dPsnrY;
    dis = 0;   
    pRecData = pRecPicture->ppPicPlane[0];
    pSrcData = pSrcPicture->ppPicPlane[0];
    for (nLine = 0; nLine < pRecPicture->picHeight; nLine++)
    {   
        for (i = 0; i < pRecPicture->picWidth; i++){
            dis += (pRecData[i] - pSrcData[i]) * (pRecData[i] - pSrcData[i]);
        }
        pSrcData += pSrcPicture->picPlaneStep[0];
        pRecData += pRecPicture->picPlaneStep[0];
    }
    dPsnrY = 10 * IPP_Log10(65025.0 * pRecPicture->picHeight * pRecPicture->picWidth / dis);
    return dPsnrY;
}

/******************************************************************************
// Name:             H264Encoder
// Description:      real H.264 encoding function 
//
// Input Arguments:	
//      fpin		    - Input yuv file
//		fpout	        - Output stream file
//      fprec	        - Output reconstruction yuv
//      pEncPar	        - Encoding parameter
//
//	Output Arguments:
//      pAvgPsnrY		- Average PSNR
//      pAvgBitrate	    - Average bitrate
//      pFrameNum	    - The total number of encoded frames
//      pAvgFps	        - frame per second
//      StatEnalbe	    - flag for enable or disable PSNR caculation
//
// Returns:
//      0               - success
//      1               - failure
******************************************************************************/
int H264Encoder(IPP_FILE *fpin, IPP_FILE *fpout, char *log_file_name, IPP_FILE *fprec, H264EncoderParSet *pEncPar)
{
    IppPicture                  srcPicture;
    IppBitstream                dstBitStream;
    IppPicture                  *pRecPicture;
    void                        *pH264EncoderState = NULL;
    H264EncoderCBTable          *pCBTable  = NULL;
    H264EncoderCBTable          CallBackTable;
    IppCodecStatus              rtCode;

    int                         nBufSize = 0, nPicArea = 0;
    Ipp8u						pictureType;
    int                         QP;
    H264NALUnitList             *pNAL;

    int nTotalFrames=0;
    int nFrameTime, nTotalTime;
    int nFrameBits, nTotalBits;
    double dPsnrY, dTotalPsnrY;
    int nReadNum;

    //int StatEnalbe = 1;

    /*stat rate control performance*/
    float fVar;
    float fMaxVar;
    int   nMaxIdx;
    int   nSecondBitsArray[1000]; // for each second
    int   nSecondBits;
    int   nSecondFrames;
    int   nSecond;
    int   i;

    int                         perf_index;
    DISPLAY_CB                  DispCB;
    DISPLAY_CB                  *hDispCB = &DispCB;

    int                         rtFlag = IPP_OK;
	int  nSkipFrameNum = 0;

    IPP_GetPerfCounter(&perf_index, DEFAULT_TIMINGFUNC_START, DEFAULT_TIMINGFUNC_STOP);
    IPP_ResetPerfCounter(perf_index);
    display_open(hDispCB, pEncPar->iWidth, pEncPar->iHeight);

    /**************************************************************************
    // 1. Initialize callback function table.                                //
    **************************************************************************/
    pCBTable = &CallBackTable;
    pCBTable->h264MemCalloc = (MiscCallocCallback)IPP_MemCalloc;
    pCBTable->h264MemFree   = (MiscFreeCallback)IPP_MemFree;

    /**************************************************************************
    // 2. Allocate output bitstream buffer.                                  //
    **************************************************************************/
    nBufSize = (pEncPar->iWidth * pEncPar->iHeight) / 10;
    IPP_MemMalloc((void**)(&dstBitStream.pBsBuffer), nBufSize, 4);
    if (NULL == dstBitStream.pBsBuffer) {
        IPP_Log(log_file_name, "a", "error: no memory!\n");
        return IPP_FAIL;
    }
    dstBitStream.pBsCurByte     = dstBitStream.pBsBuffer;
    dstBitStream.bsCurBitOffset = 0;
    dstBitStream.bsByteLen      = nBufSize;

    /**************************************************************************
    // 3. Allocate input picture buffer.                                     //
    **************************************************************************/
    srcPicture.picPlaneNum = 3;
    srcPicture.picWidth    = pEncPar->iWidth;
    srcPicture.picHeight   = pEncPar->iHeight;
    nPicArea               = pEncPar->iWidth * pEncPar->iHeight;
    IPP_MemMalloc((void**)(&srcPicture.ppPicPlane[0]), (nPicArea + (nPicArea >> 1)), 8);
    if (NULL == srcPicture.ppPicPlane[0]) {
        IPP_MemFree((void**)(&srcPicture.ppPicPlane[0]));
        IPP_Log(log_file_name, "a", "error: no memory!\n");
        return IPP_FAIL;
    }
    srcPicture.ppPicPlane[1] = ((Ipp8u*)srcPicture.ppPicPlane[0] + nPicArea);
    srcPicture.ppPicPlane[2] = ((Ipp8u*)srcPicture.ppPicPlane[1] + (nPicArea >> 2));
    srcPicture.picPlaneStep[0] = pEncPar->iWidth;
    srcPicture.picPlaneStep[1] = pEncPar->iWidth >> 1;
    srcPicture.picPlaneStep[2] = pEncPar->iWidth >> 1;
    nPicArea += nPicArea >> 1;

    /*stat rate control performance*/
    if (pEncPar->bRCEnable) {
        nSecondBits     = 0;
        nSecondFrames   = 0;
        nSecond         = 0;
        for (i = 0; i < 1000; i++) {
            nSecondBitsArray[i] = 0;
        }
    }    

    QP          = 0;
    nTotalFrames    = 0;
    nTotalBits  = 0;
    nTotalTime  = 0;
    dTotalPsnrY = 0;
    dPsnrY      = 0;
    /**************************************************************************
    // 4. Initialize encoder.                                                //
    **************************************************************************/
    rtCode = EncoderInitAlloc_H264Video(pCBTable, pEncPar, &dstBitStream, &pH264EncoderState, &pNAL);
    if (IPP_STATUS_NOERR != rtCode) {
        IPP_Log(log_file_name, "a", "error: encoder init fail! error code %d\n", rtCode);
        rtFlag = IPP_FAIL;
        goto ENCODE_END;
    }
    /*sps and pps bits*/
    nTotalBits += (dstBitStream.pBsCurByte - dstBitStream.pBsBuffer) * 8;
    if (fpout) {
        IPP_Fwrite(dstBitStream.pBsBuffer, 1, dstBitStream.pBsCurByte - dstBitStream.pBsBuffer, fpout);
    }
    dstBitStream.pBsCurByte     = dstBitStream.pBsBuffer;
    dstBitStream.bsCurBitOffset = 0;

    IPP_Printf("\n");
    IPP_Printf("  No Type  QP  PSNRY(dB)   Rate(bit)   Time(us)\n");

    /**************************************************************************
    // 5. Encode frame by frame.                                             //
    **************************************************************************/
    do {
        /**************************************************************************
        // 5.1 Read original picture into srcPicture.                            //
        **************************************************************************/
        nReadNum = IPP_Fread(srcPicture.ppPicPlane[0], 1, nPicArea, fpin);
        if (nReadNum != nPicArea) {
            break;
        }
        display_frame(hDispCB, &srcPicture);

        /**************************************************************************
        // 5.2 Encode one frame                                                  //
        **************************************************************************/
        nFrameTime = (int)IPP_GetPerfData(perf_index);
        nFrameBits = 0;
        while (1) {
            IPP_StartPerfCounter(perf_index);
            rtCode = EncodeFrame_H264Video(pCBTable, NULL, &dstBitStream, &srcPicture, pH264EncoderState, NULL, &pRecPicture);
            IPP_StopPerfCounter(perf_index); 
            if ((IPP_STATUS_OUTPUT_DATA == rtCode) || (IPP_STATUS_NOERR == rtCode)) {
                nFrameBits += (dstBitStream.pBsCurByte - dstBitStream.pBsBuffer) * 8;
                if (fpout) {
                    IPP_Fwrite(dstBitStream.pBsBuffer, 1, dstBitStream.pBsCurByte - dstBitStream.pBsBuffer, fpout);
                }
                dstBitStream.pBsCurByte = dstBitStream.pBsBuffer;
                if (IPP_STATUS_NOERR == rtCode) {
                    break;
                }
            } else {
                /*error*/
                IPP_Log(log_file_name, "a", "error: encoding frame %d fail! error code %d\n", nTotalFrames, rtCode);
                rtFlag = IPP_FAIL;
                goto ENCODE_END;
            }
        }
        nFrameTime = (int)(IPP_GetPerfData(perf_index) - nFrameTime);
        
        nTotalBits += nFrameBits;


        /**************************************************************************
        // 5.3 Output reconstructed picture and calculate PSNR                   //
        **************************************************************************/
        if (fprec){
            OutputRecPicture(fprec, pRecPicture); 
        }

        /*calculate psnr*/
        //if (StatEnalbe){
		if (pEncPar->bRCEnable) {
			/* ignore skipped frame's PSNR */
			if (nFrameBits > (float)pEncPar->iRCBitRate/(pEncPar->iFrameRate * 100.0)) {
				dPsnrY = CalculatePSNRY(pRecPicture, &srcPicture);
				dTotalPsnrY += dPsnrY;	
			} else {
				nSkipFrameNum++;
			}	
		} else {
            dPsnrY = CalculatePSNRY(pRecPicture, &srcPicture);
            dTotalPsnrY += dPsnrY;			
		}
		//}

        rtCode = EncodeSendCmd_H264Video(IPPVC_GET_PICTURETYPE, NULL, &pictureType, pH264EncoderState);//seek
        if ( rtCode != IPP_STATUS_NOERR ) {
            IPP_Printf("picture type command error\n");
        }

        IPP_Printf("%4d %4d  %2d   %2.3f      %6d     %6d\n", nTotalFrames, pictureType, QP, dPsnrY, nFrameBits, nFrameTime);
        nTotalFrames++; 

        /*stat rate control performance*/
        if (pEncPar->bRCEnable) {
            nSecondBits += nFrameBits;
            nSecondFrames++;
            if (nSecondFrames == pEncPar->iFrameRate) {
                nSecondBitsArray[nSecond] = nSecondBits;
                IPP_Printf("Second:%d TargetBitrate: %d RealBitrate: %d\n", nSecond, pEncPar->iRCBitRate, nSecondBitsArray[nSecond]);
                nSecondFrames   = 0;
                nSecondBits     = 0;
                nSecond++;
            }
        }
    } while (!IPP_Feof(fpin));

    /*stat rate control performance*/
    if (pEncPar->bRCEnable) {
        if (0 != nSecondFrames) {
            nSecondBitsArray[nSecond] = (int)((float)nSecondBits / nSecondFrames * pEncPar->iFrameRate);
            IPP_Printf("Second:%d TargetBitrate: %d RealBitrate: %d\n", nSecond, pEncPar->iRCBitRate, nSecondBitsArray[nSecond]);
            nSecondFrames   = 0;
            nSecondBits     = 0;
            nSecond++;
        }
    }

    nTotalTime = (int)IPP_GetPerfData(perf_index);
    IPP_Printf("\n#############################################################\n\n");

	IPP_Printf("FrameNum: %d   Skipped Frame Num: %d, Total Bits: %d (bit)\n", nTotalFrames, nSkipFrameNum, nTotalBits);
    IPP_Printf("Total Time: %d (ms)  fps: %.2f (frame/s)\n", nTotalTime/1000, (float)(1000.0 * 1000.0 * nTotalFrames / nTotalTime));
    IPP_Printf("FrameRate: %d frame/s\n", pEncPar->iFrameRate);
    IPP_Printf("Average Rate: %.2f kb/s\n", (float)nTotalBits * pEncPar->iFrameRate / (float)nTotalFrames / 1000.0);
    IPP_Printf("Average PSNR: %.2f dB\n", dTotalPsnrY / (nTotalFrames-nSkipFrameNum));


    g_Tot_Time[IPP_VIDEO_INDEX]     = nTotalTime;
    g_Frame_Num[IPP_VIDEO_INDEX]    = nTotalFrames;

    /*stat rate control performance*/
    if (pEncPar->bRCEnable) {
        fVar    = 0;
        fMaxVar = 0;
        nMaxIdx = 0;
        for (i = 0; i < nSecond; i++) {
            if (nSecondBitsArray[i] > pEncPar->iRCBitRate) {
                fVar += (((float)nSecondBitsArray[i] / pEncPar->iRCBitRate - 1) );
                if (fMaxVar < (nSecondBitsArray[i] - pEncPar->iRCBitRate)) {
                    fMaxVar = (float)(nSecondBitsArray[i] - pEncPar->iRCBitRate);
                    nMaxIdx = i;
                }
            } else {
                fVar -= (((float)nSecondBitsArray[i] / pEncPar->iRCBitRate - 1) );
                if (fMaxVar < (pEncPar->iRCBitRate - nSecondBitsArray[i])) {
                    fMaxVar = (float)(pEncPar->iRCBitRate - nSecondBitsArray[i]);
                    nMaxIdx = i;
                }
            }
        }
        fVar    /= nSecond;
        fVar    *= 100;
        fMaxVar /= pEncPar->iRCBitRate;
        fMaxVar *= 100;
        IPP_Printf("the average inaccuracy of rate control is : %.2f%%\n", fVar);
        IPP_Printf("the maximum inaccuracy of rate control is : %.2f%% Time: %d Bitrate: %d\n", fMaxVar, nMaxIdx, nSecondBitsArray[nMaxIdx]);
    }


ENCODE_END:
    /**************************************************************************
    // 6. Free allocated buffers.                                            //
    **************************************************************************/
    rtCode = EncoderFree_H264Video(pCBTable, &pH264EncoderState, NULL);
    if (IPP_STATUS_NOERR != rtCode) {
        IPP_Log(log_file_name, "a", "error: encoder free fail, error code %d!\n", rtCode);
        rtFlag = IPP_FAIL;
    }

    if (dstBitStream.pBsBuffer) {
        IPP_MemFree((void**)(&dstBitStream.pBsBuffer));
    }
    if (srcPicture.ppPicPlane[0]) {
        IPP_MemFree((void**)(&srcPicture.ppPicPlane[0]));
    }
    
    /**************************************************************************
    // 7. Output information.                                                //
    **************************************************************************/

    IPP_FreePerfCounter(perf_index);
    display_close();
    return rtFlag;
}

#define MAX_PAR_NAME_LEN    1024
#define MAX_PAR_VALUE_LEN   2048
#define STRNCPY(dst, src, len) \
{\
    IPP_Strncpy((dst), (src), (len));\
    (dst)[(len)] = '\0';\
}
/******************************************************************************
// Name:                ParseH264DecCmd
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

int ParseH264EncCmd(char *pCmdLine, 
                    char *pSrcFileName, 
                    char *pDstFileName, 
                    char *pLogFileName, 
                    char *pRecFileName,
                    void *pParSet)
{

    char *pCur, *pEnd;
    char par_name[MAX_PAR_NAME_LEN];
    char par_value[MAX_PAR_VALUE_LEN];
    int  par_name_len;
    int  par_value_len;
    char *p1, *p2, *p3;

    int rtCode;
    int nParaIdx;
    int *p;

    H264EncoderParSet *pH264EncParSet = (H264EncoderParSet *)pParSet;
    IPP_FILE *fConfig;

    /*step1: initialize parameters and set default value*/
    InitPara(pH264EncParSet);

    /*step 2: read parameters setting from config file*/
    fConfig = IPP_Fopen("EncConfig.cfg", "r");
    if (NULL == fConfig){
        IPP_Printf("Can't open default config file: %s\n", "EncConfig.cfg");
    } else {
        rtCode = ReadConfigFile(fConfig);
        if (0 == rtCode){
            IPP_Printf("Incorrect parameter setting in default config file\n");
            return IPP_FAIL;
        }
        IPP_Fclose(fConfig);
    }

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
        } else if ((0 == IPP_Strcmp(par_name, "r")) || (0 == IPP_Strcmp(par_name, "r"))) { 
            /*rec file*/
            STRNCPY(pRecFileName, p2 + 1, par_value_len);
        } else if ((0 == IPP_Strcmp(par_name, "p")) || (0 == IPP_Strcmp(par_name, "P"))) { 
            /*parameter file*/
            STRNCPY(par_value, p2 + 1, par_value_len);
            fConfig = IPP_Fopen(par_value, "r");
            if (NULL == fConfig){
                IPP_Printf("Can't open config file: %s\n", par_value);
                return IPP_FAIL;
            } else {
                rtCode = ReadConfigFile(fConfig);
                if (0 == rtCode){
                    IPP_Printf("Incorrect parameter setting in config file\n");
                    IPP_Fclose(fConfig);
                    return IPP_FAIL;
                }
                IPP_Fclose(fConfig);
                IPP_Printf("use user-defined config file: %s\n", par_value);
            }
        } else {
            nParaIdx = GetParaIndex(par_name);
            if (H264_ENC_PARA_NUM > nParaIdx){
                STRNCPY(par_value, p2 + 1, par_value_len);
                p = (int *)Map[nParaIdx].p;
                *p = IPP_Atoi(par_value);
            } else {
                return IPP_FAIL;
            }
        }

        pCur = p3;
    }

    /*step 5: output parameter config*/
    OutputPara(pH264EncParSet);
    return IPP_OK;
}


/*Interface for IPP sample code template*/
int CodecTest(int argc, char **argv)
{
    IPP_FILE *fpin = NULL, *fpout = NULL;
    char input_file_name[2048]  = {'\0'};
    char output_file_name[2048] = {'\0'};
    char log_file_name[2048]    = {'\0'};
    char rec_file_name[2048]    = {'\0'};
    H264EncoderParSet *pH264EncParSet = &g_H264EncParSet;
    IPP_FILE *fprec = NULL;
    int rtFlag;

	DisplayLibVersion();

    if (2 > argc) {
        IPP_Printf("Usage: h264enc.exe -i:input.yuv -o:output.cmp -l:enc.log -r:rec.yuv -p:parfile!\n");
        return IPP_FAIL;
    } else if (2 == argc){
        /*for validation*/
        if (IPP_OK != ParseH264EncCmd(argv[1], input_file_name, output_file_name, log_file_name, rec_file_name, pH264EncParSet)){
            IPP_Log(log_file_name, "w", 
                "command line is wrong! %s\nUsage: h264enc.exe -i:input.cmp -o:output.cmp -l:enc.log!\n", argv[1]);
            return IPP_FAIL;
        }
    } else {
        /*for internal debug*/
        ParseH264EncCmd(argv[1], input_file_name, output_file_name, log_file_name, rec_file_name, pH264EncParSet);
        IPP_Strcpy(input_file_name, argv[1]);
        IPP_Strcpy(output_file_name, argv[2]);
        IPP_Strcpy(rec_file_name, "h264rec.yuv");
    }

    if (0 == IPP_Strcmp(input_file_name, "\0")) {
        IPP_Log(log_file_name, "a", "input file name is null!\n");
        return IPP_FAIL;
    } else {
        fpin = IPP_Fopen(input_file_name, "rb");
        if (!fpin) {
            IPP_Log(log_file_name, "a", "Fails to open file %s!\n", input_file_name);
            return IPP_FAIL;
        }
    }

    if (0 == IPP_Strcmp(output_file_name, "\0")) {
        IPP_Log(log_file_name, "a", "output file name is null!\n");
    } else {
        fpout = IPP_Fopen(output_file_name, "wb");
        if (!fpout) {
            IPP_Log(log_file_name, "a", "Fails to open file %s\n", output_file_name);
            return IPP_FAIL;
        }
    }

    if (0 == IPP_Strcmp(rec_file_name, "\0")) {
        IPP_Printf("rec file name is null!\n");
    } else {
        fprec = IPP_Fopen(rec_file_name, "wb");
        if (!fprec) {
            IPP_Log(log_file_name, "a", "Fails to open file %s\n", rec_file_name);
            return IPP_FAIL;
        }
    }
    
    IPP_Log(log_file_name, "a", "command line: %s\n", argv[1]);
    IPP_Log(log_file_name, "a", "input file  : %s\n", input_file_name);
    IPP_Log(log_file_name, "a", "output file : %s\n", output_file_name);
    IPP_Log(log_file_name, "a", "rec file    : %s\n", rec_file_name);
    IPP_Log(log_file_name, "a", "begin to encode\n");
    rtFlag = H264Encoder(fpin, fpout, log_file_name, fprec, pH264EncParSet);
    if (IPP_OK == rtFlag) {
        IPP_Log(log_file_name, "a", "everything is OK!\n");
    } else {
        IPP_Log(log_file_name, "a", "encoding fail!\n");
    }

    if (fpin) {
        IPP_Fclose(fpin);
    }

    if (fpout) {
        IPP_Fclose(fpout);
    }

    if (fprec) {
        IPP_Fclose(fprec);
    }

    return rtFlag;
}


/*EOF*/