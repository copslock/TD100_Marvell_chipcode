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


#include "v263euser.h"
extern int iGroRCMaxBitRate;

/***************************************************************************
// Name:             OutputPara
// Description:      Display H263 Encoder Parameters
//
// Input Arguments:
//      pfPara		 Pointer to the input patamete file stream buffer
// Output Arguments:
//  
//	Returns:	
//		IPP Codec Status Code
*****************************************************************************/

#define    displayValue(x)              IPP_Printf(#x"\t\t\t= %d \n", x);
#define    displayValueS(x)             IPP_Printf(#x"\t\t\t= %s \n", x);

void OutputPara(IppH263ParSet *pParInfo)
{
    IPP_Printf("\n======SETTING============\t\t\t\t=======================\n");
    displayValue(pParInfo->bAlgorithmFromCustomer);
    displayValueS(pParInfo->InputRawDataFileName);
    displayValueS(pParInfo->OutputCompressedFileName);
    displayValue(pParInfo->iSourceFormat);
    displayValue(pParInfo->iPicWidth);
    displayValue(pParInfo->iPicHeight);
    displayValue(pParInfo->iSrcFrmRate);
    displayValue(pParInfo->iSrcSamRate);
    displayValue(pParInfo->iColorFormat);
    displayValue(pParInfo->bRCEnable);
    displayValue(pParInfo->iRCBitRate);
	//RC enhancement for maximum bitrate constrain
	displayValue(iGroRCMaxBitRate);
	//RC enhancement for maximum bitrate constrain
    displayValue(pParInfo->bHWMEEnable);

    displayValue(pParInfo->bRoundingCtrlEnable);
    displayValue(pParInfo->GOBHeaderPresent);
    displayValue(pParInfo->iPBetweenI);
    displayValue(pParInfo->iIntraQuant);
    displayValue(pParInfo->iInterQuant);
    displayValue(pParInfo->optUMV);
    displayValue(pParInfo->optSAC);
    displayValue(pParInfo->optAP);
    displayValue(pParInfo->optPBframes);

    displayValue(pParInfo->optAIC);
    displayValue(pParInfo->optDF);
    displayValue(pParInfo->optSS);
    displayValue(pParInfo->BitsPerSlice);
    displayValue(pParInfo->optRPS);
    displayValue(pParInfo->optISD);
    displayValue(pParInfo->optAIV);
    displayValue(pParInfo->optMQ);

    displayValue(pParInfo->optRPR);
    displayValue(pParInfo->optRRU);

    displayValue(pParInfo->iRCType);
    displayValue(pParInfo->iSearchRange);
    displayValue(pParInfo->iVbvBitRate);
    displayValue(pParInfo->iVbvBufferSize);
    displayValue(pParInfo->iVbvOccupancy);
    displayValue(pParInfo->iMotionActivity);
    displayValue(pParInfo->iSlopeDelta);
    displayValue(pParInfo->iInflexionDelta);
    displayValue(pParInfo->iIPictureQPDelta);
    displayValue(pParInfo->iMaxQP);
    displayValue(pParInfo->iMinQP);
    displayValue(pParInfo->iDelayMaxFactor);
    displayValue(pParInfo->iDelayMinFactor);

    displayValue(pParInfo->iModelK);
    displayValue(pParInfo->iModelC);

    displayValue(pParInfo->iMEAlgorithm);
    displayValue(pParInfo->iMEStrategy);
    IPP_Printf("========================\t\t\t\t=======================\n");
    
}

/***************************************************************************
// Name:             GetPar_H263Enc
// Description:      Get parameters from input PAR file to parameter structure
//
// Input Arguments:
//      pfPara		 Pointer to the input patamete file stream buffer
// Output Arguments:
//      pParInfo     Pointer to the updated input parameter structure
//      pISConfig    Pointer to the image stabilizer plug-in configuration structure
//
//	Returns:	
//		IPP Codec Status Code
*****************************************************************************/

IPPCODECFUN(IppCodecStatus, GetPar_H263Enc)(IppH263ParSet *pParInfo, FILE *pfPara)
{
/* all the parameters to the encoder*/
	FILE *m_fp;
	
	int  uiFrmWidth=0;
	int  uiFrmHeight=0;

	char *pchRawDataFileName, *pchCompressedDataFileName, *pchInputDir, *pchOutputDir;
	//char pchSrcRaw[100], pchDstCompressed[100];

	char pchBuf[TOKEN_SIZE], pchName[TOKEN_SIZE];
	
	int  rguiBitsBudget;
	int  rgdFrameFrequency;	
	int	 iSamplingRate;
	int  ColorFormat;
	int  SourceFormat;
	int	 bRateControl;
	int  iLen, er;
	char chBuf[80], *pch, *pchEnd;
	
	/*Set Default Value*/
	pchInputDir = NULL;
	pchOutputDir = NULL;
	pchRawDataFileName = NULL;
	pchCompressedDataFileName = NULL;
	rgdFrameFrequency = 30;
	iSamplingRate = 1;
	SourceFormat = IPP_VIDEO_QCIF;
	rguiBitsBudget = 300000;
	bRateControl = 0;
	ColorFormat = IPP_YCbCr411;

    /*Update ParSeting.*/
    UpdateStandardPar_H263(pParInfo);

    er = ERES_OK;
	pch = IPP_Fgets(chBuf, 80, pfPara);
	if( IPP_Strncmp("!!!INTEL!!!", chBuf, 11)!=0 ){
		IPP_Printf("Not a valid parameter file!");	
		er = ERES_OK;
	}
	m_fp = pfPara;
	while (!(parGetToken(m_fp, pchBuf)==ERES_EOF)){

		IPP_Strcpy(pchName, pchBuf);

		if(parGetToken(m_fp, pchBuf)==ERES_EOF)
			er = ERES_FORMAT;

		if(pchBuf[0]!='=')
			er = ERES_FORMAT;

		if(parGetToken(m_fp, pchBuf)==ERES_EOF)
			er = ERES_FORMAT;

		if (IPP_Strcmp(pchName, "Source.Directory") == 0){				
			iLen = strlen(pchBuf) + 1;
            IPP_MemMalloc(&pchInputDir, (iLen*sizeof(char)) , 8);
			memcpy(pchInputDir, pchBuf, iLen);

		}else if(IPP_Strcmp(pchName, "Source.RawDataFile") == 0){
			iLen = strlen(pchBuf) + 1;
            IPP_MemMalloc(&pchRawDataFileName, (iLen*sizeof(char)), 8 );
			memcpy(pchRawDataFileName, pchBuf, iLen);

		}else if(IPP_Strcmp(pchName, "Source.width") == 0){
			uiFrmWidth = IPP_Strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "Source.height") == 0){
			uiFrmHeight = IPP_Strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "Output.Directory") == 0){
			iLen = strlen(pchBuf) + 1;
            IPP_MemMalloc(&pchOutputDir, (iLen*sizeof(char)), 8);
			memcpy(pchOutputDir, pchBuf, iLen);

		}else if(IPP_Strcmp(pchName, "Output.CompressedBitstream") == 0){
			iLen = strlen(pchBuf) + 1;
            IPP_MemMalloc(&pchCompressedDataFileName, (iLen*sizeof(char)), 8);
			memcpy(pchCompressedDataFileName, pchBuf, iLen);

		}else if(IPP_Strcmp(pchName, "Source.FrameRate") == 0){
			rgdFrameFrequency = IPP_Strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "Source.SamplingRate") == 0){
			iSamplingRate = IPP_Strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "Source.Format") == 0){
			if (IPP_Strcmp(pchBuf, "SUB_QCIF") == 0){
				SourceFormat = IPP_VIDEO_SUBQCIF;
				uiFrmWidth = 128;
				uiFrmHeight = 96;
			}else if(IPP_Strcmp(pchBuf, "QCIF") == 0){
				SourceFormat = IPP_VIDEO_QCIF;
				uiFrmWidth = 176;
				uiFrmHeight = 144;
			}else if(IPP_Strcmp(pchBuf, "CIF") == 0){
				SourceFormat = IPP_VIDEO_CIF;
				uiFrmWidth = 352;
				uiFrmHeight = 288;
			}else if(IPP_Strcmp(pchBuf, "4CIF") == 0){
				SourceFormat = IPP_VIDEO_4CIF;
				uiFrmWidth = 704;
				uiFrmHeight = 576;
			}else if(IPP_Strcmp(pchBuf, "16CIF") == 0){
				SourceFormat = IPP_VIDEO_16CIF;
				uiFrmWidth = 1408;
				uiFrmHeight = 1152;			
			}else if(IPP_Strcmp(pchBuf, "CPF") == 0){
				SourceFormat = IPP_VIDEO_CUSTOM;
				
			}else{
				IPP_Printf("Not supported Source Format!\n");
				er = ERES_FORMAT;
			}
		}else if(IPP_Strcmp(pchName, "RateControl.BitRate") == 0){
			rguiBitsBudget = IPP_Strtol(pchBuf, &pchEnd, 10);
		//RC enhancement for maximum bitrate constrain
		}else if(IPP_Strcmp(pchName, "RateControl.MaxBitRate") == 0){
			iGroRCMaxBitRate = IPP_Strtol(pchBuf, &pchEnd, 10);
		//RC enhancement for maximum bitrate constrain
		}else if(IPP_Strcmp(pchName, "RateControl.Enable") == 0){
			bRateControl = IPP_Strtol(pchBuf, &pchEnd, 10);
				if ( 0== bRateControl ) {
		           pParInfo->iRCType = RC_NULL;
				}
		}else if(IPP_Strcmp(pchName, "Source.ColorFormat") == 0){
			if(IPP_Strcmp(pchBuf, "YUV420")==0)
				ColorFormat = IPP_YCbCr411; 
			else if(IPP_Strcmp(pchBuf, "RGB888")==0)
				ColorFormat  = IPP_BGR888;     
			else {
				IPP_Printf("Not supported Color Format!\n");
				er = ERES_FORMAT;
			}
        }else if(IPP_Strcmp(pchName, "iInterQuant") == 0){
            pParInfo->iInterQuant = IPP_Strtol(pchBuf, &pchEnd, 10);
        }else if(IPP_Strcmp(pchName, "iIntraQuant") == 0){
            pParInfo->iIntraQuant = IPP_Strtol(pchBuf, &pchEnd, 10);
        }else if(IPP_Strcmp(pchName, "GOBHeaderPresent") == 0){
            pParInfo->GOBHeaderPresent = IPP_Strtol(pchBuf, &pchEnd, 10);
        }else if(IPP_Strcmp(pchName, "bRoundingCtrlEnable") == 0){
            pParInfo->bRoundingCtrlEnable = IPP_Strtol(pchBuf, &pchEnd, 10);
        }else if(IPP_Strcmp(pchName, "iPBetweenI") == 0){
            pParInfo->iPBetweenI = IPP_Strtol(pchBuf, &pchEnd, 10);
        }else if(IPP_Strcmp(pchName, "optUMV") == 0){
            pParInfo->optUMV = IPP_Strtol(pchBuf, &pchEnd, 10);
        }else if(IPP_Strcmp(pchName, "optDF") == 0){
            pParInfo->optDF = IPP_Strtol(pchBuf, &pchEnd, 10);
        }else if(IPP_Strcmp(pchName, "optSS") == 0){
            pParInfo->optSS = IPP_Strtol(pchBuf, &pchEnd, 10);
        }else if(IPP_Strcmp(pchName, "iRCType") == 0){
            pParInfo->iRCType = IPP_Strtol(pchBuf, &pchEnd, 10);
        }else if(IPP_Strcmp(pchName, "iSearchRange") == 0){
            pParInfo->iSearchRange = IPP_Strtol(pchBuf, &pchEnd, 10);
        }else if(IPP_Strcmp(pchName, "iMotionActivity") == 0){
            pParInfo->iMotionActivity = IPP_Strtol(pchBuf, &pchEnd, 10);
        }else if(IPP_Strcmp(pchName, "iSlopeDelta") == 0){
            pParInfo->iSlopeDelta = IPP_Strtol(pchBuf, &pchEnd, 10);
        }else if(IPP_Strcmp(pchName, "iIPictureQPDelta") == 0){
            pParInfo->iIPictureQPDelta = IPP_Strtol(pchBuf, &pchEnd, 10);
        }else if(IPP_Strcmp(pchName, "iMEAlgorithm") == 0){
            pParInfo->iMEAlgorithm = IPP_Strtol(pchBuf, &pchEnd, 10);
        }else if(IPP_Strcmp(pchName, "iMEStrategy") == 0){
            pParInfo->iMEStrategy = IPP_Strtol(pchBuf, &pchEnd, 10);
            /*add for QA parfile*/
        }else if(IPP_Strcmp(pchName, "bAlgorithmFromCustomer") == 0){
            pParInfo->bAlgorithmFromCustomer = IPP_Strtol(pchBuf, &pchEnd, 10);
        }else if(IPP_Strcmp(pchName, "BitsPerSlice") == 0){
            pParInfo->BitsPerSlice = IPP_Strtol(pchBuf, &pchEnd, 10);
        }else if(IPP_Strcmp(pchName, "optAIC") == 0) {
  	   pParInfo->optAIC = IPP_Strtol(pchBuf, &pchEnd, 10);
	}else if(IPP_Strcmp(pchName, "optMQ") == 0) {
	   pParInfo->optMQ = IPP_Strtol(pchBuf, &pchEnd, 10);
        }else{
			er = ERES_FORMAT;
		}
	};

	if(er!=ERES_OK) {
		IPP_Printf("error %d at some line of parameter file\n", er );
		return IPP_STATUS_INPUT_ERR;
	}
	
	/* set pInfo values */
	pParInfo->iPicWidth = uiFrmWidth;
	pParInfo->iPicHeight = uiFrmHeight;
	pParInfo->iSourceFormat = SourceFormat;
	pParInfo->iSrcFrmRate = rgdFrameFrequency;
	pParInfo->iSrcSamRate = iSamplingRate;

	pParInfo->bRCEnable = bRateControl;
	pParInfo->iRCBitRate = rguiBitsBudget;
	pParInfo->iColorFormat = ColorFormat;
	
    /*update ratecontrol parameters*/
    if(IPP_STATUS_NOERR != UpdateAlgorithmPar_External_H263(pParInfo)){
        return IPP_STATUS_ERR;
    };

    /*dump setting.*/
    //OutputPara(pParInfo);

    if(pchInputDir){
	    IPP_MemFree(&pchInputDir);
    }

    if(pchOutputDir){
	    IPP_MemFree(&pchOutputDir);
    }

    if(pchRawDataFileName){
	    IPP_MemFree(&pchRawDataFileName);
    }

    if(pchCompressedDataFileName){
	    IPP_MemFree(&pchCompressedDataFileName);	
    }

	return er;
}

/*Parsing Cmd line for H263 Encoder.*/
#define MAX_PAR_NAME_LEN    1024
#define MAX_PAR_VALUE_LEN   2048
#define STRNCPY(dst, src, len) \
{\
    IPP_Strncpy((dst), (src), (len));\
    (dst)[(len)] = '\0';\
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
IPPCODECFUN(int,ParseCmd_H263Enc)(char *pCmdLine, 
                                  char *pSrcFileName, 
                                  char *pDstFileName, 
                                  char *pLogFileName, 
                                  /*IppParSet*/void *pParInfo
                                  )
{
    char *pCur, *pEnd;
    char par_name[MAX_PAR_NAME_LEN];
    char *p1 = NULL, *p2 = NULL, *p3 = NULL;

    int  par_name_len         = 0;
    int  par_value_len        = 0;
  
    IppCodecStatus   ret      = IPP_STATUS_NOERR;

    /*Par file*/
    char pParFileName[128]    = {"\0"};
    FILE *fpar                = NULL;
    char cTmp[64]             = {"\0"};
    IppH263ParSet  *pPar          = (IppH263ParSet*)pParInfo;

    /*Update ParSeting.*/
    UpdateStandardPar_H263(pParInfo);

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
        } else if ((0 == IPP_Strcmp(par_name, "p")) || (0 == IPP_Strcmp(par_name, "P"))) {
            /*par file*/
            /*parse par file to fill pParSet*/
            STRNCPY(pParFileName, p2 + 1, par_value_len);

            fpar = IPP_Fopen(pParFileName, "rb");
            if(!fpar){
                return IPP_STATUS_ERR;
            }

            GetPar_H263Enc(pParInfo, fpar);
            if(fpar){
                IPP_Fclose(fpar);
            }

            /*1st Level Parameter*/
        } else if ((0 == IPP_Strcmp(par_name, "Width"))){
            STRNCPY(cTmp, p2 + 1, par_value_len);
            pPar->iPicWidth = IPP_Atoi(cTmp);
        } else if ((0 == IPP_Strcmp(par_name, "Height"))){
            /*parse other parameters for encoder*/
            STRNCPY(cTmp, p2 + 1, par_value_len);
            //pPar->iPicWidth = IPP_Atoi(cTmp);
	    pPar->iPicHeight = IPP_Atoi(cTmp);
        } else if ((0 == IPP_Strcmp(par_name, "FrameRate"))){ 
            /*parse other parameters for encoder*/
            STRNCPY(cTmp, p2 + 1, par_value_len);
            pPar->iSrcFrmRate = IPP_Atoi(cTmp);
        } else if ((0 == IPP_Strcmp(par_name, "SamplingRate"))){ 
            /*parse other parameters for encoder*/
            STRNCPY(cTmp, p2 + 1, par_value_len);
            pPar->iSrcSamRate = IPP_Atoi(cTmp);
        } else if ((0 == IPP_Strcmp(par_name, "bRCEnable"))){ 
            /*parse other parameters for encoder*/
            STRNCPY(cTmp, p2 + 1, par_value_len);
            pPar->bRCEnable = IPP_Atoi(cTmp);
        } else if ((0 == IPP_Strcmp(par_name, "BitRate"))){ 
            /*parse other parameters for encoder*/
            STRNCPY(cTmp, p2 + 1, par_value_len);
            pPar->iRCBitRate = IPP_Atoi(cTmp);

            /*2nd Level Parameter*/
		//RC enhancement for maximum bitrate constrain
        } else if ((0 == IPP_Strcmp(par_name, "MaxBitRate"))){ 
            /*parse other parameters for encoder*/
            STRNCPY(cTmp, p2 + 1, par_value_len);
			iGroRCMaxBitRate = IPP_Atoi(cTmp);
		//RC enhancement for maximum bitrate constrain
        }else if ((0 == IPP_Strcmp(par_name, "GOBHeaderPresent"))){ 
            /*parse other parameters for encoder*/
            STRNCPY(cTmp, p2 + 1, par_value_len);
            pPar->GOBHeaderPresent = IPP_Atoi(cTmp);
        } else if ((0 == IPP_Strcmp(par_name, "bRoundingCtrlEnable"))){ 
            /*parse other parameters for encoder*/
            STRNCPY(cTmp, p2 + 1, par_value_len);
            pPar->bRoundingCtrlEnable = IPP_Atoi(cTmp);
        } else if ((0 == IPP_Strcmp(par_name, "iPBetweenI"))){ 
            /*parse other parameters for encoder*/
            STRNCPY(cTmp, p2 + 1, par_value_len);
            pPar->iPBetweenI = IPP_Atoi(cTmp);

        } else if ((0 == IPP_Strcmp(par_name, "iIntraQuant"))){ 
            /*parse other parameters for encoder*/
            STRNCPY(cTmp, p2 + 1, par_value_len);
            pPar->iIntraQuant = (Ipp8u)(IPP_Atoi(cTmp));
        } else if ((0 == IPP_Strcmp(par_name, "iInterQuant"))){ 
            /*parse other parameters for encoder*/
            STRNCPY(cTmp, p2 + 1, par_value_len);
            pPar->iInterQuant = (Ipp8u)(IPP_Atoi(cTmp));
        } else if ((0 == IPP_Strcmp(par_name, "optUMV"))){ 
            /*parse other parameters for encoder*/
            STRNCPY(cTmp, p2 + 1, par_value_len);
            pPar->optUMV = (Ipp8u)(IPP_Atoi(cTmp));
        } else if ((0 == IPP_Strcmp(par_name, "optDF"))){ 
            /*parse other parameters for encoder*/
            STRNCPY(cTmp, p2 + 1, par_value_len);
            pPar->optDF = IPP_Atoi(cTmp);
        } else if ((0 == IPP_Strcmp(par_name, "optSS"))){ 
            /*parse other parameters for encoder*/
            STRNCPY(cTmp, p2 + 1, par_value_len);
            pPar->optSS = IPP_Atoi(cTmp);

        } else if ((0 == IPP_Strcmp(par_name, "iRCType"))){ 
            /*parse other parameters for encoder*/
            STRNCPY(cTmp, p2 + 1, par_value_len);
            pPar->iRCType = IPP_Atoi(cTmp);
        } else if ((0 == IPP_Strcmp(par_name, "iSearchRange"))){ 
            /*parse other parameters for encoder*/
            STRNCPY(cTmp, p2 + 1, par_value_len);
            pPar->iSearchRange = IPP_Atoi(cTmp);

        } else if ((0 == IPP_Strcmp(par_name, "iMotionActivity"))){ 
            /*parse other parameters for encoder*/
            STRNCPY(cTmp, p2 + 1, par_value_len);
            pPar->iMotionActivity = IPP_Atoi(cTmp);

        } else if ((0 == IPP_Strcmp(par_name, "iIPictureQPDelta"))){ 
            /*parse other parameters for encoder*/
            STRNCPY(cTmp, p2 + 1, par_value_len);
            pPar->iIPictureQPDelta = IPP_Atoi(cTmp);

        /*3rd Level Parameter*/
        } else if ((0 == IPP_Strcmp(par_name, "iMEAlgorithm"))){ 
            /*parse other parameters for encoder*/
            STRNCPY(cTmp, p2 + 1, par_value_len);
            pPar->iMEAlgorithm = IPP_Atoi(cTmp);

        } else if ((0 == IPP_Strcmp(par_name, "iMEStrategy"))){ 
            /*parse other parameters for encoder*/
            STRNCPY(cTmp, p2 + 1, par_value_len);
            pPar->iMEStrategy = IPP_Atoi(cTmp);

        } else if ((0 == IPP_Strcmp(par_name, "optAIC"))) {
		    STRNCPY(cTmp, p2 + 1, par_value_len);
		    pPar->optAIC = IPP_Atoi(cTmp);
	    } else if ((0 == IPP_Strcmp(par_name, "optMQ"))) {
		    STRNCPY(cTmp, p2 + 1, par_value_len);
		    pPar->optMQ = IPP_Atoi(cTmp);
	    } else {
            /*adding no such  option*/
            STRNCPY(cTmp, p2 + 1, par_value_len);
            IPP_Printf("No Such Optional: %s:::%s, Skipped\n", par_name, cTmp);
            //return IPP_STATUS_ERR;
        }

        pCur = p3;
    }

     /*update ratecontrol parameters*/
    if(IPP_STATUS_NOERR != UpdateAlgorithmPar_External_H263(pParInfo)){
        return IPP_STATUS_ERR;
    }

    OutputPara(pParInfo);
    return ret;
}

/*EOF*/
