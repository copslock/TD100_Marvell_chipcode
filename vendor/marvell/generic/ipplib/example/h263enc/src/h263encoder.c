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

#include	"v263euser.h"
#include "ippLV.h"

int		iGroRCMaxBitRate = 0;

/*Max File Name Length*/
#define iMaxLen         128
static char scHelpH263Enc[] = {
    "Marvell H263Enc Usage: \n"
    "     IPP Unit Test    :  ./App Input.yuv Output.cmp Parfile\n"
    "     Integration Test :  ./App \"-i:input.yuv -o:output.cmp -l:logfile -p:parfile [-option:value]\" \n"
    "               Option :  -Width, -Height,-FrameRate,-SamplingRate,-bRCEnable,-BitRate, -MaxBitRate \n"
    "                      :  -GOBHeaderPresent,-bRoundingCtrlEnable \n"
    "                      :  -iPBetweenI, -iIntraQuant, -iInterQuant,-optUMV\n"
    "                      :  -optDF,-optSS,-iRCType,-iSearchRange,-iMotionActivity \n"
    "                      :  -iMEAlgorithm, -iMEStrategy \n"
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
	ret = GetLibVersion_H263ENC(libversion,sizeof(libversion));
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
// Name:             H263encoder
// Description:      Encoding entry for H263 baseline encoder
//
// Input Arguments:
//      inParFile	  Pointer to input parameter file stream buffer
//
// Output Arguments:
//
// Returns:         
//		IPP Codec Status Code
*****************************************************************************/
int H263encoder(IppH263ParSet *pParInfo, unsigned long * pFrameNumber, char *pLog)
{

    FILE	*infile = NULL;
    int		i, iLoadTag, retcode = IPP_STATUS_NOERR;
    Ipp32u   pictureType;

    void	*ph263EncState = NULL;
    IppBitstream	*pbufInfo = NULL;
    IppPicture		*pPic = NULL;
    FILE			*fcmp = NULL;
    MiscGeneralCallbackTable   *pCallBackTable = NULL;
    char    *logfile   = pLog;
    DISPLAY_CB          hDispCB;

    /*Performance counter*/
    int                  perf_index             = 0;
    long long            llTotaltime            = 0;

	DisplayLibVersion();

    IPP_GetPerfCounter(&perf_index, DEFAULT_TIMINGFUNC_START, DEFAULT_TIMINGFUNC_STOP);
    IPP_ResetPerfCounter(perf_index);

    *pFrameNumber = 0;

    if ( miscInitGeneralCallbackTable(&pCallBackTable) != 0 ) {
        IPP_Log(logfile,"a", "Error: miscInitGeneralCallbackTable \n");
        goto EndOfH263Enc; 
    }
  
    IPP_MemMalloc(&pbufInfo, sizeof(IppBitstream), 8);
    if ( !pbufInfo ) {
        IPP_Log(logfile,"a", "Error: IPP_MemMalloc pbufInfo \n");
        goto EndOfH263Enc;
    } else {
        memset (pbufInfo, 0, sizeof(IppBitstream) );
    }

    IPP_MemMalloc(&pPic, sizeof(IppPicture), 8);
    if ( !pPic ) {
        IPP_Log(logfile,"a", "Error: IPP_MemMalloc pPic \n");
        goto EndOfH263Enc;
    } else {
        memset (pPic, 0, sizeof(IppPicture) );
    }

    infile = IPP_Fopen(pParInfo->InputRawDataFileName,"rb");
    if(infile == NULL) {
        IPP_Log(logfile,"a", "Error: IPP_Fopen Input \n");
        goto EndOfH263Enc;
    }

    fcmp = IPP_Fopen (pParInfo->OutputCompressedFileName, "wb+");
    if ( !fcmp ) {
        IPP_Log(logfile,"a", "Error: IPP_Fopen Output \n");
        //goto EndOfH263Enc;
    }

    /* Allocate input video frame buffer */
    pPic->picWidth      = pParInfo->iPicWidth;
    pPic->picHeight     = pParInfo->iPicHeight;
    pPic->picPlaneNum   = 3;
    pPic->picChannelNum = 3;
    pPic->picFormat     = IPP_YCbCr411;

        /*Init Display for Manchac*/
    if(IPP_STATUS_NOERR != display_open(&hDispCB, pParInfo->iPicWidth, pParInfo->iPicHeight)){
        IPP_Log(logfile,"a", "Error: display_open \n");
        retcode = IPP_STATUS_ERR;
		goto  EndOfH263Enc;
    }

    pParInfo->pStreamHandler = NULL;
	if (!init_cmp_output_buf (pbufInfo, (pParInfo->iPicWidth*pParInfo->iPicHeight) << 3)) {
        IPP_Log(logfile,"a", "Error: init_cmp_output_buf \n");
        return IPP_STATUS_NOMEM_ERR;
    }

    retcode = EncoderInitAlloc_H263Video (pParInfo, 
        pCallBackTable,	pPic, &ph263EncState);
    if ( retcode != IPP_STATUS_NOERR ) {
        IPP_Log(logfile,"a", "Error: EncoderInitAlloc_H263Video \n");
        goto POST_DEAL;
    }

	if ( iGroRCMaxBitRate ) {
		retcode = EncodeSendCmd_H263Video(IPPVC_SET_MAXIBITRATES, &iGroRCMaxBitRate, NULL, ph263EncState);
        if ( retcode != IPP_STATUS_NOERR ) {
            IPP_Log(logfile,"a", "Error: EncodeSendCmd_H263Video \n");
            goto POST_DEAL;
        }
	}

    if (fcmp) {
        if ( ( pbufInfo->pBsCurByte - pbufInfo->pBsBuffer > pParInfo->iPicWidth *pParInfo->iPicHeight / 2 ) ){
            output_bitstream (pbufInfo, fcmp);
        } 
    } else {
        pbufInfo->pBsCurByte = pbufInfo->pBsBuffer;	
    }

    if ( IPP_BGR888 == pParInfo->iColorFormat ) {
        iLoadTag = IPP_STATUS_INPUT_ERR;
    } else if ( IPP_BGR565 == pParInfo->iColorFormat ) {
        iLoadTag = IPP_STATUS_INPUT_ERR;
    } else {
        iLoadTag = LoadExtendPlane(infile, pPic);
        display_frame(&hDispCB, pPic);/*Display on Manchanc*/
    }

    while (iLoadTag == IPP_STATUS_NOERR) {	
        /*************************************************************
        // Preview sub-sampled raw data
        *************************************************************/

        IPP_StartPerfCounter(perf_index);

        retcode = Encode_H263Video(pbufInfo, pPic, ph263EncState);
		
        IPP_StopPerfCounter(perf_index);

        if ( IPP_STATUS_NOERR != retcode ) {
            if ( IPP_STATUS_ERR == retcode ) {
                continue;
            } else {
                IPP_Log(logfile,"a", "Error: Encode_H263Video \n");
                goto POST_DEAL;
            }
        }

		//remove freeze to example code
		retcode = Encode_FillFreezeH263Video(pbufInfo, ph263EncState);
		 if ( IPP_STATUS_NOERR != retcode ) {
			 IPP_Log(logfile,"a", "Error: Encode_FillFreezeH263Video \n");
		 }
		//remove freeze to example code

        retcode = EncodeSendCmd_H263Video(IPPVC_GET_PICTURETYPE, NULL, &pictureType, ph263EncState);
        if ( retcode != IPP_STATUS_NOERR ) {
            IPP_Log(logfile,"a", "Error: EncodeSendCmd_H263Video \n");
            goto POST_DEAL;
        }

        IPP_Printf("picture type[ %d ]	: %d\n", (*pFrameNumber) ,pictureType);

        (*pFrameNumber)++;
        if (fcmp) {
			output_bitstream (pbufInfo, fcmp);
        } else {
            pbufInfo->pBsCurByte = pbufInfo->pBsBuffer;	
        }

        for( i = 1; i < pParInfo->iSrcSamRate; i ++) {
            if ( 0 != IPP_Fseek(infile, ( pParInfo->iPicWidth * pParInfo->iPicHeight * 3 ) >> 1, 1) ) {
                break;
            }				
        }

        if ( IPP_BGR888 == pParInfo->iColorFormat ||  (IPP_BGR565 == pParInfo->iColorFormat)) {
            IPP_Log(logfile,"a", "Error: ColorFormat \n");
            goto POST_DEAL;
        } else {
            iLoadTag = LoadExtendPlane(infile, pPic);
            display_frame(&hDispCB, pPic);/*Display on Manchanc*/
        }
    }	

    if (fcmp) {
        output_bitstream (pbufInfo, fcmp);
    }

    llTotaltime = IPP_GetPerfData(perf_index);
    g_Frame_Num[IPP_VIDEO_INDEX] = *pFrameNumber;
    g_Tot_Time[IPP_VIDEO_INDEX] = llTotaltime;

    IPP_Printf("Total Time %.2f (s), FPS is %.2f fps\n ", llTotaltime/(1000.0*1000.0), (*pFrameNumber) * 1000.0 *1000.0/llTotaltime);

POST_DEAL:

    /*DeInit Display*/
    display_close();
    IPP_FreePerfCounter(perf_index);

    EncoderFree_H263Video(&ph263EncState);

    free_cmp_output_buf(pbufInfo);

EndOfH263Enc:

    if(pbufInfo){
        IPP_MemFree(&pbufInfo);
    }

    if(pPic){
        IPP_MemFree(&pPic);
    }

    miscFreeGeneralCallbackTable(&pCallBackTable);

    if(infile){
        IPP_Fclose (infile);
    }

    if (fcmp) {
        IPP_Fclose (fcmp);
    }

    return retcode;
}


/***************************************************************************
// Name:             CodecTest
// Description:      Entry function for H263 baseline encoder
//
// Returns:          0:		  encoding okay
//                   others:  encoding fail
*****************************************************************************/
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
    IppH263ParSet   ParInfo;
    char        *pLog               = NULL;
    int         len                 =   0;

    /*Usage:*/
    IPP_Printf("%s\n", &scHelpH263Enc);

    IPP_Memset(&ParInfo, 0x0, sizeof(IppH263ParSet));

    pInputFile  = ParInfo.InputRawDataFileName;
    pOutputFile = ParInfo.OutputCompressedFileName;

    /*Parse cmd line and parfile to pParInfo State*/
    if((argc == 2) && IPP_Strstr(argv[1], ":") ){

        /*Parse QA Cmd Line and parsr Parfile to state*/
        //IPP_Printf("Parsing QA Cmd Line \n");
        ret = ParseCmd_H263Enc(argv[1], pInputFile, pOutputFile, log_file_name, &ParInfo);

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
            goto EndOFH263Enc;
        }

        /*Parse ParFile */
        ret = GetPar_H263Enc(&ParInfo, fpar);
        if(0 != ret){
            ret = IPP_STATUS_ERR;
            goto EndOFH263Enc;
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
    ret = H263encoder(&ParInfo, &ulFrameNumber, pLog);

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

EndOFH263Enc:

    if ( fpin)     IPP_Fclose (fpin);
    if ( fpout)	   IPP_Fclose (fpout); 
    if ( fpar)     IPP_Fclose (fpar);

    if(pLog){
        IPP_MemFree(&pLog);
    }

    return ret ;
}
