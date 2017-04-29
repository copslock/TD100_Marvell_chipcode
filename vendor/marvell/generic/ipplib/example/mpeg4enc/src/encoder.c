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

/******************************************************************************
//  Description:    Preview video raw data and encodes it as MPEG-4 bitstream file
//	Function list:
//		MPEG4encoder
//
//  History:
//      Date          Author					 Changes
//      2001/09		  Rio Wu					 Created.
******************************************************************************/
#include "vmp4para.h"
#include "vguser.h"
#include "misc.h"

#include "ippLV.h"

int		iGroRCMaxBitRate = 0;


/*Max File Name Length*/
#define iMaxLen         128
static char scHelpMpeg4Enc[] = {
    "Marvell MPEG4Enc Usage: \n"
    "     IPP Unit Test    :  ./App Input.yuv Output.cmp Parfile\n"
    "     Integration Test :  ./App \"-i:input.yuv -o:output.cmp -l:logfile -p:parfile [-option:value]\" \n"
    "               Option :  -Width, -Height,-FrameRate,-SamplingRate,-bRCEnable,-BitRate -iMaxBitRate,-iIntraDcVlcThr,-bRoundingCtrlEnable \n"
    "                      :  -iPBetweenI, -iIVOPQuantStep, -iPVOPQuantStep,-bQuantType, -bIntraQMatrixDefault \n"
    "                      :  -bInterQMatrixDefault,-bResyncDisable,-bDataPartitioned,-bReverseVLC,-iVideoPacketLength \n"
    "                      :  -bAdapativeIntraRefreshEnable,-iAdapativeIntraRefreshRate,-iPictureComplexity, -iSearchRange \n"
    "                      :  -iVbvBufferSize, -iVbvOccupancy, -iMEStrategy \n"
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
	ret = GetLibVersion_MPEG4ENC(libversion,sizeof(libversion));
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

/***************************************************************************
// Name:             appiOutputRec_MPEG4Enc
// Description:      Dump Reconstruct YUV frame to File
//
// Input Arguments:
//      pmp4EncState  pointer to encoder state
//
// Output Arguments:
//		outYUVFile    output file handle
//
// Returns:         
//		NULL
*****************************************************************************/
void appiOutputRec_MPEG4Enc(void *pmp4EncState, FILE * outYUVFile)
{
#if 0
	Ipp8u *pResultY, *pResultU, *pResultV;
	int step;

    pResultY = ((IppMPEG4VideoEncoderState*)pmp4EncState)->FwdRefRecFrame.pYPtr;
    pResultU = ((IppMPEG4VideoEncoderState*)pmp4EncState)->FwdRefRecFrame.pCbPtr;
	pResultV = ((IppMPEG4VideoEncoderState*)pmp4EncState)->FwdRefRecFrame.pCrPtr;
				
	step = ((IppMPEG4VideoEncoderState*)pmp4EncState)->FwdReconVop.stepSet.YStep;
	for(i = 0; i<((IppMPEG4VideoEncoderState*)pmp4EncState)->iVolDisplayHeight; i++) {
		IPP_Fwrite(pResultY, sizeof( char ), ((IppMPEG4VideoEncoderState*)pmp4EncState)->iVolDisplayWidth, outYUVFile);
		pResultY += step;			
	}
			
	step = ((IppMPEG4VideoEncoderState*)pmp4EncState)->FwdReconVop.stepSet.CbStep;
	for(i = 0; i<((IppMPEG4VideoEncoderState*)pmp4EncState)->iVolDisplayHeight/2; i++) {
		IPP_Fwrite(pResultU, sizeof( char ),((IppMPEG4VideoEncoderState*)pmp4EncState)->iVolDisplayWidth/2, outYUVFile);
		pResultU += step;
	}
				
	step = ((IppMPEG4VideoEncoderState*)pmp4EncState)->FwdReconVop.stepSet.CrStep;
	for(i = 0; i<((IppMPEG4VideoEncoderState*)pmp4EncState)->iVolDisplayHeight/2; i++) {
	   	IPP_Fwrite(pResultV, sizeof( char ), ((IppMPEG4VideoEncoderState*)pmp4EncState)->iVolDisplayWidth/2, outYUVFile);
		pResultV += step;
	}
#else
    pmp4EncState = pmp4EncState;
    outYUVFile   = outYUVFile;
#endif
    return ;
}

/***************************************************************************
// Name:             MPEG4encoder
// Description:      Encoding entry for MPEG4 baseline encoder
//
// Input Arguments:
//      inParFile	  Pointer to input parameter file stream buffer
//		outYUVFile	  Pointer to reconstruction YUV file stream buffer.
//
// Output Arguments:
//		pFrameNumber  Overall encoded frame numbers
//
// Returns:         
//		IPP Codec Status Code
*****************************************************************************/
int MPEG4encoder(IppParSet *pParInfo, IppISConfig *pISConfig, int *pISEnable, unsigned long *pFrameNumber, char* logfile)
{

	int		i, iLoadTag, retcode, size, ret = 0;
	void                *pmp4EncState           = NULL;

    IppBitstream		*pbufInfo               = NULL;
	IppPicture			*pPic                   = NULL;

	MiscGeneralCallbackTable   *pCallBackTable  = NULL;

	IppPicture			*pExtPic                = NULL;
	Ipp8u				*pRawDataBuffer         = NULL;
	int					pos;
	int					iBorderExt;
	int					bISEnable;
	IppISDynamicContrl	ISDynamicControl;
    FILE                *outYUVFile             = NULL;
    FILE                *fpin                   = NULL;
	FILE				*fcmp                   = NULL;
    DISPLAY_CB          hDispCB          ;
	//print information RC_VT
	Ipp8u					pictureType;
	//print information
 
    /*Performance counter*/
    int                  perf_index             = 0;
    long long            llTotaltime            = 0;

    IPP_GetPerfCounter(&perf_index, DEFAULT_TIMINGFUNC_START, DEFAULT_TIMINGFUNC_STOP);
    IPP_ResetPerfCounter(perf_index);

	*pFrameNumber = 0;

	bISEnable = *pISEnable;

    /*Init Callback*/
	if ( miscInitGeneralCallbackTable(&pCallBackTable) != 0 ) {
        IPP_Log(logfile,"a", "Error: miscInitGeneralCallbackTable \n");
		return IPP_STATUS_NOMEM_ERR; 
	}
    /*Init output Bitstream buffer*/
	IPP_MemCalloc(&pbufInfo, sizeof(IppBitstream), 4);
	if ( !pbufInfo ) {
        IPP_Log(logfile,"a", "Error: IPP_MemCalloc \n");
        ret = IPP_STATUS_ERR;
        goto  POST_DEAL2;
	}

    /*Init input Picture structure.*/
	IPP_MemCalloc(&pPic, sizeof(IppPicture), 4);
    if(!pPic){
        IPP_Log(logfile,"a", "Error: IPP_MemCalloc pPic\n");
        ret = IPP_STATUS_ERR;
        goto  POST_DEAL2;
    }

	IPP_MemCalloc(&pExtPic, sizeof(IppPicture), 4);
    if (!pExtPic ) {
        IPP_Log(logfile,"a", "Error: IPP_MemCalloc pExtPic\n");
        ret = IPP_STATUS_ERR;
        goto  POST_DEAL2;
	} 

    /*Open the Input & Output & LogFile*/
    fpin = IPP_Fopen(pParInfo->InputRawDataFileName, "rb");
    if (!fpin) {
        IPP_Log(logfile,"a", "Error: IPP_Fopen %s \n", pParInfo->InputRawDataFileName);
        ret = IPP_STATUS_ERR;
        goto  POST_DEAL2;
    }

    fcmp = IPP_Fopen(pParInfo->OutputCompressedFileName, "wb");
    if (!fcmp) {
        IPP_Log(logfile,"a", "Warning: IPP_Fopen %s \n", pParInfo->OutputCompressedFileName);
        //ret = IPP_STATUS_ERR;
        //goto  POST_DEAL2;
    }

	pParInfo->pStreamHandler = (void*)fcmp;

	if ( !init_cmp_output_buf(pbufInfo, (pParInfo->iVolWidth * pParInfo->iVolHeight) >> 3)) {
        IPP_Log(logfile,"a", "Error: init_cmp_output_buf \n");
        ret = IPP_STATUS_ERR;
		goto  POST_DEAL2;
	}

        /*Init Display for Manchac*/
    if(IPP_STATUS_NOERR != display_open(&hDispCB, pParInfo->iVolWidth, pParInfo->iVolHeight)){
        IPP_Log(logfile,"a", "Error: display_open \n");
        ret = IPP_STATUS_ERR;
		goto  POST_DEAL2;
    }
	/* Allocate input video frame buffer */
	pPic->picWidth			= pParInfo->iVolWidth;
	pPic->picHeight			= pParInfo->iVolHeight; /* must be even */
	pPic->picPlaneNum		= 3;
	pPic->picChannelNum		= 3;
	pPic->picFormat			= IPP_YCbCr411;

	iBorderExt				= pISConfig->iBorderExtension;
	pExtPic->picWidth		= pParInfo->iVolWidth + iBorderExt*2;
	pExtPic->picHeight		= pParInfo->iVolHeight + iBorderExt*2; /* must be even */
	pExtPic->picPlaneNum	= 3;
	pExtPic->picChannelNum	= 3;
	pExtPic->picFormat		= IPP_YCbCr411;

	/* 8 multiple step is required */
	pExtPic->picPlaneStep[0] = ((pExtPic->picWidth + 15) >> 4) << 4;
	pExtPic->picPlaneStep[1] = pExtPic->picPlaneStep[0] >> 1;
	pExtPic->picPlaneStep[2] = pExtPic->picPlaneStep[0] >> 1;

	pPic->picPlaneStep[0] = pExtPic->picPlaneStep[0];
	pPic->picPlaneStep[1] = pExtPic->picPlaneStep[1];
	pPic->picPlaneStep[2] = pExtPic->picPlaneStep[2];

	size = pExtPic->picPlaneStep[0] * (((pExtPic->picHeight + 15) >> 4) << 4);
	pCallBackTable->fMemMalloc((void**)&pRawDataBuffer, 24 + ((size * 3) >> 1), 1);
	if (!pRawDataBuffer) {
        IPP_Log(logfile,"a", "Error: pCallBackTable->fMemMalloc \n");
		ret = IPP_STATUS_ERR;
		goto  POST_DEAL2;
	}
	/* 8 byte alignment is required */
	pos = (int)pRawDataBuffer + iBorderExt * pExtPic->picPlaneStep[0] + iBorderExt;
	pPic->ppPicPlane[0] = (Ipp8u*) (((pos + 7) >> 3) << 3);
	pExtPic->ppPicPlane[0] = (Ipp8u*)pPic->ppPicPlane[0] -
		(iBorderExt * pExtPic->picPlaneStep[0] + iBorderExt);

	pos = (int)pExtPic->ppPicPlane[0] + size +
		(iBorderExt>>1) * pExtPic->picPlaneStep[1] + (iBorderExt>>1);
	pPic->ppPicPlane[1] = (Ipp8u*) (((pos + 7) >> 3) << 3);
	pExtPic->ppPicPlane[1] = (Ipp8u*)pPic->ppPicPlane[1] -
		((iBorderExt>>1) * pExtPic->picPlaneStep[1] + (iBorderExt>>1));

	pos = (int)pExtPic->ppPicPlane[1] + (size>>2) +
		(iBorderExt>>1) * pExtPic->picPlaneStep[2] + (iBorderExt>>1);
	pPic->ppPicPlane[2] = (Ipp8u*) (((pos + 7) >> 3) << 3);
	pExtPic->ppPicPlane[2] = (Ipp8u*)pPic->ppPicPlane[2] -
		((iBorderExt>>1) * pExtPic->picPlaneStep[2] + (iBorderExt>>1));

	retcode = EncoderInitAlloc_MPEG4Video (pbufInfo, pParInfo, 
		pCallBackTable,	NULL, &pmp4EncState);

	if ( retcode != IPP_STATUS_NOERR ) {
        IPP_Log(logfile,"a", "Error: EncoderInitAlloc_MPEG4Video, ret %d \n", retcode);
        ret = IPP_STATUS_ERR;
		goto  POST_DEAL;
	}

	/* Enable the image stabilizer */
	if ( bISEnable ) {
		pISConfig->iVolWidth		= pParInfo->iVolWidth;
		pISConfig->iVolHeight		= pParInfo->iVolHeight;
		pISConfig->iSearchRes		= 1;
		pISConfig->iMaxKBNum		= (pParInfo->iVolWidth*pParInfo->iVolHeight>>8) / 10;
		pISConfig->iMEAlgorithm	    = 1;
		pISConfig->iNoiseSAD		= 500;
		pISConfig->pJMV			    = NULL;
		pISConfig->pGMV			    = NULL;
		pISConfig->fMemAlloc		= pCallBackTable->fMemCalloc;
		pISConfig->fMemFree		    = pCallBackTable->fMemFree;
		
		ISDynamicControl.pISConfig		= pISConfig;
		ISDynamicControl.fISInitAlloc	= appiInitAlloc_IS;
		ISDynamicControl.fISFree		= appiFree_IS;
		ISDynamicControl.fISPreprocess	= appiPreprocess_IS;
		ISDynamicControl.fISPostprocess	= appiPostprocess_IS;
		
		retcode = EncodeSendCmd_MPEG4Video(IPPVC_ENABLE_IS, &ISDynamicControl, NULL, pmp4EncState);

		if ( retcode != IPP_STATUS_NOERR ) {
            IPP_Log(logfile,"a", "Error: EncodeSendCmd_MPEG4Video, ret %d \n", retcode);
            ret = IPP_STATUS_ERR;
		    goto  POST_DEAL;
		}
	}

	//RC_VT
	if (iGroRCMaxBitRate) {
		retcode = EncodeSendCmd_MPEG4Video(IPPVC_SET_MAXIBITRATES, &iGroRCMaxBitRate, NULL, pmp4EncState);

		if ( retcode != IPP_STATUS_NOERR ) {
			IPP_Log(logfile,"a", "Error: EncodeSendCmd_MPEG4Video, ret %d \n", retcode);
			ret = IPP_STATUS_ERR;
			goto  POST_DEAL;
		}
	}
	//RC_VT

#ifdef _MPEG4AIRCIRENABLED
	//for AIR/CIR
	{
		IppAIRCIRControl	AIRCIRControl = {0};
		AIRCIRControl.iAIRCIRStrategy = 0;
		AIRCIRControl.bAIREnable = 1;
		AIRCIRControl.iAIRMBIntraCodedPerFrame = 3;	//a typical value for qcif
		AIRCIRControl.nAIRRefreshTime = 1;
		AIRCIRControl.iAIRSAD00th_InitVal = 0;
		AIRCIRControl.bCIREnable = 0;
		AIRCIRControl.iCIRMBIntraCodedPerFrame = 4;	//a typical value for qcif
		retcode = EncodeSendCmd_MPEG4Video(IPPVC_SETAIRCIRSTRATEGY, &AIRCIRControl, NULL, pmp4EncState);
		if(retcode != IPP_STATUS_NOERR) {
            ret = IPP_STATUS_ERR;
		    goto  POST_DEAL;
		}
	}
#endif

    /*Dump compressed stream out to Output file.*/
	if (fcmp) {
		if ( ( pbufInfo->pBsCurByte - pbufInfo->pBsBuffer > 0 ) ){
			output_bitstream (pbufInfo, fcmp);
		} 
	} else {
		pbufInfo->pBsCurByte = pbufInfo->pBsBuffer;	
	}

    /*Load One frame YUV data from input File*/
	iLoadTag = LoadExtendPlane(fpin, pExtPic);

    display_frame(&hDispCB, pExtPic);/*Display on Manchanc*/
		
	while (iLoadTag == IPP_STATUS_NOERR) {	

        IPP_StartPerfCounter(perf_index);

        /*Encoding One VOP of MPEG4 Simple Profile*/
        retcode = Encode_MPEG4Video(pbufInfo, pPic, pmp4EncState);

        IPP_StopPerfCounter(perf_index);

		if ( IPP_STATUS_NOERR != retcode ) {
			if ( IPP_STATUS_ERR == retcode ) {
				continue;
			} else {
                IPP_Log(logfile,"a", "Error: Encode_MPEG4Video, ret %d \n", retcode);
                ret = IPP_STATUS_ERR;
				goto POST_DEAL;
			}
		}

		if(outYUVFile) {
            /*Dump reconstruct YUV : default is OFF*/
            appiOutputRec_MPEG4Enc(pmp4EncState, outYUVFile);
		}

		//print information
		retcode = EncodeSendCmd_MPEG4Video(IPPVC_GET_PICTURETYPE, NULL, &pictureType, pmp4EncState);

		if ( retcode != IPP_STATUS_NOERR ) {
            IPP_Log(logfile,"a", "Error: EncodeSendCmd_MPEG4Video, ret %d \n", retcode);
            ret = IPP_STATUS_ERR;
		    goto  POST_DEAL;
		}
		IPP_Printf("picture type[ %d ]	:  %d\n", (*pFrameNumber) ,pictureType);
		//print information RC_VT

	    (*pFrameNumber)++;		

        //IPP_Printf("Encoder %d VOP\n", *pFrameNumber);

        /*Dump compressed stream out*/
	    if (fcmp) {
			if ( ( pbufInfo->pBsCurByte - pbufInfo->pBsBuffer > 0/*pParInfo->iVolWidth*pParInfo->iVolHeight*/) ){
				output_bitstream (pbufInfo, fcmp);
			} 
		} else {
			pbufInfo->pBsCurByte = pbufInfo->pBsBuffer;	
		}

        /*Re-sample for Input YUV stream.*/
		for( i = 1; i < pParInfo->iSrcSamRate; i ++) {
			if ( 0 != fseek(fpin, ( size * 3 ) >> 1, 1) ) {
				break;
			}				
		}
		
        /*Load One frame YUV data from input File*/
        iLoadTag = LoadExtendPlane(fpin, pExtPic);

        display_frame(&hDispCB, pExtPic);/*Display on Manchanc*/
					
	}	

POST_DEAL:

    /*DeInit Display*/
    display_close();

    llTotaltime = IPP_GetPerfData(perf_index);
    g_Frame_Num[IPP_VIDEO_INDEX] = *pFrameNumber;
    g_Tot_Time[IPP_VIDEO_INDEX] = llTotaltime;

    IPP_Printf("Total Time %.2f (s), FPS is %.2f fps\n ", llTotaltime/(1000.0*1000.0), (*pFrameNumber) * 1000.0 *1000.0/llTotaltime);

    /*Dump compressed stream out to Output File*/
	if (fcmp) {
	    output_bitstream (pbufInfo, fcmp);
	}
	
    /*Free Codec context*/
	retcode = EncoderFree_MPEG4Video(&pmp4EncState);	
    if(IPP_STATUS_NOERR != retcode){
        IPP_Log(logfile,"a", "Error: EncoderFree_MPEG4Video, ret %d \n", retcode);
        ret = IPP_STATUS_ERR;
    }

POST_DEAL2:

	if(pbufInfo->pBsBuffer) { 
		 IPP_MemFree(&pbufInfo->pBsBuffer);
     }

    if(pbufInfo){
	    IPP_MemFree(&pbufInfo);
    }

    if(pRawDataBuffer){
	    pCallBackTable->fMemFree((void**)&pRawDataBuffer);
    }

    if(pPic){
	    IPP_MemFree(&pPic);
    }

    if(pExtPic){
	    IPP_MemFree(&pExtPic);
    }

    /*Performance Data*/
    IPP_FreePerfCounter(perf_index);

	miscFreeGeneralCallbackTable(&pCallBackTable);

    if(fpin)  IPP_Fclose (fpin);
	if(fcmp) IPP_Fclose (fcmp);

	return ret;
}

/***************************************************************************
// Name:             main
// Description:      Entry function for MPEG4 baseline encoder
//
// Returns:          0:		  encoding okay
//                   others:  encoding fail
*****************************************************************************/
/*
int CodecTest(int argc, char **argv)
*/
/*
int main (int argc, char *argv[])
*/
int CodecTest (int argc, char *argv[])
{
    int   ret                       =   0;

    char *pInputFile                =   NULL;
    char *pOutputFile               =   NULL;
    char log_file_name[iMaxLen]     =   {'\0'};
    char par_file_name[iMaxLen]     =   {'\0'};

    /* 0 means use default value , reserved for future */
    unsigned long ulFrameNumber     =   0;

    FILE	    *fpin               =   NULL; 
    FILE	    *fpout              =   NULL;
    FILE        *fpar               =   NULL;
    IppParSet   ParInfo;
    IppISConfig ISConfig;
    int         bISEnable           =   0;

    char        *pLog               = NULL;
    int         len                 =   0;

    /*Usage:*/
    IPP_Printf("%s\n", &scHelpMpeg4Enc);
	
	DisplayLibVersion();

    IPP_Memset(&ParInfo, 0x0, sizeof(IppParSet));

    pInputFile  = ParInfo.InputRawDataFileName;
    pOutputFile = ParInfo.OutputCompressedFileName;

    /*Parse cmd line and parfile to pParInfo State*/
    if((argc == 2) && strchr(argv[1], ':') ){

        /*Parse QA Cmd Line and parsr Parfile to state*/
        //IPP_Printf("Parsing QA Cmd Line \n");
        ret = ParseCmd_MPEGEnc(argv[1], pInputFile, pOutputFile, log_file_name, &ParInfo, &ISConfig, &bISEnable);

        if(0 != ret){
            IPP_Log(pLog,"a", "Error: ParseCmd_MPEGEnc \n");
            return -1;
        }

    }else if(argc == 4){
        /*Parse IPP Cmd line*/
        IPP_Printf("Parsing IPP Cmd Line \n");
        IPP_Strcpy(pInputFile, argv[1]);
        IPP_Strcpy(pOutputFile, argv[2]);
        IPP_Strcpy(par_file_name, argv[3]);

        IPP_Printf("Input: %s, Output: %s, ParFile: %s\n",pInputFile, pOutputFile, par_file_name );

        fpar = IPP_Fopen(par_file_name, "rb");
        if(!fpar){
            ret = IPP_STATUS_ERR;
            goto EndOFMpeg4Enc;
        }

        /*Parse ParFile */
        ret = GetPar_MPEG4Enc(&ParInfo, &ISConfig, &bISEnable, fpar);
        if(0 != ret){
            ret = IPP_STATUS_ERR;
            goto EndOFMpeg4Enc;
        }

    }else{
        return -1;
    }

    len = IPP_Strlen(log_file_name);
    if(len){
        IPP_MemMalloc(&pLog, len ,8);
        IPP_Memcpy(pLog, log_file_name, len);
    }

    /*pass parInfo to main encoder API*/
    ret = MPEG4encoder(&ParInfo, &ISConfig, &bISEnable, &ulFrameNumber, pLog);

    switch (ret)
    {
    case IPP_STATUS_NOERR:
        //IPP_Printf("Everything is Okay\n");
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

EndOFMpeg4Enc:

    if ( fpin)     IPP_Fclose (fpin);
    if ( fpout)	   IPP_Fclose (fpout); 
    if ( fpar)     IPP_Fclose (fpar);

    if(pLog){
        IPP_MemFree(&pLog);
    }

    return ret ;
}

/*EOF*/