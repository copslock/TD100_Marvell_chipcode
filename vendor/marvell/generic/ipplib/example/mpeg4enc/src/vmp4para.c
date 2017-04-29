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
//  Description:    encoder parameter process functions
//	Function list:
//		GetPar_MPEG4
//		parSkipSpace
//		parSkipComment
//		parGetToken
//
******************************************************************************/

#include "codecVC.h"
#include "vguser.h"
#include "vmp4para.h"
extern int iGroRCMaxBitRate;


unsigned char appCustomerIntraQMatrix [64] = {
	(unsigned char)8,	17,	18,	19,	21,	23,	25,	27,
	(unsigned char)17,	18,	19,	21,	23,	25,	27,	28,
	(unsigned char)20,	21,	22,	23,	24,	26,	28,	30,
	(unsigned char)21,	22,	23,	24,	26,	28,	30,	32,
	(unsigned char)22,	23,	24,	26,	28,	30,	32,	35,
	(unsigned char)23,	24,	26,	28,	30,	32,	35,	38,
	(unsigned char)25,	26,	28,	30,	32,	35,	38,	41,
	(unsigned char)27,	28,	30,	32,	35,	38,	41,	45
};

unsigned char appCustomerInterQMatrix [64] = {
	16,	17,	18, 19, 20,	21,	22,	23,
	17,	18,	19,	20,	21,	22,	23,	24,
	18,	19,	20,	21,	22,	23,	24,	25,
	19,	20,	21,	22,	23,	24,	26,	27,
	20,	21,	22,	23,	25,	26,	27,	28,
	21,	22,	23,	24,	26,	27,	28,	30,
	22,	23,	24,	26,	27,	28,	30,	31,
	23,	24,	25,	27,	28,	30,	31,	33
};

IPPCODECFUN(IppCodecStatus, UpdateStandardPar_MPEG4)(void *pParInfo)
{

	IppParSet * pMP4EncoderPar = (IppParSet *) pParInfo;
	
	/* Update standard defined codec configuration */

	pMP4EncoderPar->bObjectLayerIdentifier = 1;
		/* 2 Option: 1 -> version identification and priority is specified for the visual object layer
					 0 -> no version identification or priority needs to be specified
		*/

	pMP4EncoderPar->iVolVerID = 2;			
		/* 2 Option: 1 -> Version 1; 2 -> Version 2 */

	pMP4EncoderPar->iVolPriority = 1;
		/* 7 Option: 1 ~ 7; 
					 1 - Highest Priority
					 7 - Lowest Priority
		*/

	pMP4EncoderPar->bVolRandomAcess = 0;
		/* 2 Option: 1 -> every VOP in this VOL is individually decodable
					 0 -> any of the VOPs in the VOL are non-intra coded
		*/

	pMP4EncoderPar->iVideoObjectTypeID = 0x01;
		/* Video Object Type Identification 
			Reserved						0x00
			Simple Object Type				0x01	(Supported)
			Simple Scalable Object Type		0x02	(Not Supported)
			Core Object Type				0x03	(Not Supported)
			Main Object Type				0x04	(Not Supported)
			N-bit Object Type				0x05	(Not Supported)
			Basic Anim.2D Texture			0x06	(Not Supported)
			Anim.2D Mesh					0x07	(Not Supported)
			Simple Face						0x08	(Not Supported)
			Still Scalable Texture			0x09	(Not Supported)
			Advanced Real Time Simple		0x0a	(Not Supported)
			Core Scalable					0x0b	(Not Supported)
			Advanced Coding Efficiency		0x0c	(Not Supported)
			Advanced Scalable Texture		0x0d	(Not Supported)
			Simple FBA						0x0e	(Not Supported)
			Reserved						0x0f ~ 0xff
		*/

	pMP4EncoderPar->bQuantType = Q_MPEG4;
		/* 2 Option:	Q_H263, Q_MPEG4 */

	pMP4EncoderPar->bIntraQMatrixDefault = 1;
		/* 2 Option:	1 -> Default Matrix; 0 -> Customer Input Matrix */

	pMP4EncoderPar->bInterQMatrixDefault = 1;
		/* 2 Option:	1 -> Default Matrix; 0 -> Customer Input Matrix */

	pMP4EncoderPar->IntraQMatrix = appCustomerIntraQMatrix;
		/* Customer Input Intra Quantization Maxtrix, 64 sub-values */

	pMP4EncoderPar->InterQMatrix = appCustomerInterQMatrix;
		/* Customer Input Inter Quantization Maxtrix, 64 sub-values */

	pMP4EncoderPar->iPBetweenI = 20;
		/* Positive integer indicates number of P-frame between two neighbouring I-frames, 
		   -1 indicates just one I-frame in starting point, no more I-frame */
	
	pMP4EncoderPar->iIntraDcVlcThr = 3;
		/* 8 Option:  0, 1, 2, 3, 4, 5, 6, 7 */

	pMP4EncoderPar->iIVOPQuantStep = 12;
		/* Legal Values:  1 - 31
		   If RC enable, it indicates the initialized QP of I-VOP,
		   else it indicates the QP of all I-VOPs
		*/

	pMP4EncoderPar->iPVOPQuantStep = 14;
		/* Legal Values:  1 - 31
		   If RC enable, it indicates the initialized QP of P-VOP,
		   else it indicates the QP of all P-VOPs
		*/

	pMP4EncoderPar->bRoundingCtrlEnable = 1;
		/* 2 Option: 1 -> Enable; 0 -> Disable; */

	pMP4EncoderPar->iRoundingCtrlInit = 0;
		/* 2 Option: 0, 1 */

	
	pMP4EncoderPar->bComplexEstDisable = 1;
		/* 2 Option: 1 -> Disable; 0 -> Enable; */

	pMP4EncoderPar->bResyncDisable = 1;
		/* 2 Option: 1 -> Disable; 0 -> Enable; */	
	pMP4EncoderPar->bDataPartitioned = 0;
		/* 2 Option: 1 -> Enable; 0 -> Disable; */	
	pMP4EncoderPar->iVideoPacketLength = 500;
		/* Posititve Integer indicates the target number of bit in packet */	

	pMP4EncoderPar->bReverseVLC = 0;
		/* 2 Option: 1 -> Enable; 0 -> Disable; */

	pMP4EncoderPar->bAdapativeIntraRefreshEnable = 0;
		/* 2 Option: 1 -> Adapative Intra Macroblock Refreshment Enable 
					 0 -> Adapative Intra Macroblock Refreshment Disable
		*/
    pMP4EncoderPar->iAdapativeIntraRefreshRate = 0;
		/* Positive Integer indicates the number of VOPs in which all macroblocks refreshed a round */

	
	pMP4EncoderPar->bAdapativeIntraRefreshEnable = 0;	
		/* 2 Option: 1 -> Enable; 0 -> Disable; */

	pMP4EncoderPar->iAdapativeIntraRefreshRate = 0;
		/* Posititve Integer indicates the number of VOPs in which all macroblocks refreshed a round */	

		/* add for VBV&VOL control support */
	pMP4EncoderPar->bVolControlPara = 0;
	pMP4EncoderPar->bVbvParaEnable  = 0;
		/*
		// To enable VBV setting, make sure (bVolControlPara == 1 && bVbvParaEnable ==1)
		// iVbvBitRate:    It is maximum bit rate, recommend value = Max_BitPerFrame x FrameRate
		// iVbvBufferSize: Practical used buffer size for encoder
		// iVbvOccupancy:  Practical used occupancy
		*/
	pMP4EncoderPar->iVbvBitRate     = pMP4EncoderPar->iRCBitRate*3;
		/* Maximum_bits_per_frame * Frame_Rate */	
		/* Recommend: pMP4EncoderPar->iRCBitRate*3 */	
	pMP4EncoderPar->iVbvBufferSize  = 50;
		/* 
		// Percentage value. must be ">0"  
		// real buffer = pMP4EncoderPar->iVbvBufferSize * pMP4EncoderPar->iRCBitRate / 100
		// 50, 100	
		// Recommend: 50  <--->  0.5 second delay  
	    */	
	pMP4EncoderPar->iVbvOccupancy   = 0;
		/*
		// Percentage value, valid [0, 100].  
		// real occupancy = pMP4EncoderPar->iVbvOccupancy * real buffer / 100
		// 0, 50 or 200/3 
	    // Corresponding buffer position: 0, 1/2, 2/3 
		// Recommend: 0 
		*/	

	pMP4EncoderPar->iPictureComplexity = 1;
		/* Complexity of Clips, 3 Option: 
			0 -> Low Complexity Clips;
			1 -> Median Complexity Clips;
			2 -> High Complexity Clips;
		*/

	pMP4EncoderPar->iSearchRange = 15;
		/* Positive Integer, Search Range of Motion Estimation */

	pMP4EncoderPar->bUseSrcME = 0;
		/*   "0" means that motion estimation is performed on reconstructed referenece plane directly
		      "Option: 1" is deprecated from 4.1 beta.
		*/

	pMP4EncoderPar->iMEAlgorithm = 0;
		/* Motion Estimation Algorithm Selection
		   3 Option:	0 -> MVFAST;
						1 -> SEA;
						2 -> SEA_CONCAN;
		*/

    pMP4EncoderPar->iMEStrategy = ME_PERFORMANCE_BIAS;

	return IPP_STATUS_NOERR;

}


/***************************************************************************
// Name:             OutputPara
// Description:      Display MPEG4 Encoder Parameters
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

void OutputPara(IppParSet *pParInfo)
{
    IPP_Printf("\n======SETTING============\t\t\t\t=======================\n");
    displayValue(pParInfo->bAlgorithmFromCustomer);
    displayValueS(pParInfo->InputRawDataFileName);
    displayValueS(pParInfo->OutputCompressedFileName);
    displayValue(pParInfo->iVolWidth);
    displayValue(pParInfo->iVolHeight);
    displayValue(pParInfo->iSrcFrmRate);
    displayValue(pParInfo->iSrcSamRate);
    displayValue(pParInfo->iColorFormat);
    displayValue(pParInfo->bRCEnable);
    displayValue(pParInfo->iRCBitRate);
    displayValue(pParInfo->bHWMEEnable);
    displayValue(pParInfo->bObjectLayerIdentifier);
    displayValue(pParInfo->iVolVerID);
    displayValue(pParInfo->iVolPriority);
    displayValue(pParInfo->bVolRandomAcess);
    displayValue(pParInfo->iVideoObjectTypeID);
    displayValue(pParInfo->iIntraDcVlcThr);
    displayValue(pParInfo->bRoundingCtrlEnable);
    displayValue(pParInfo->iRoundingCtrlInit);
    displayValue(pParInfo->iPBetweenI);
    displayValue(pParInfo->iClockRate);
    displayValue(pParInfo->iNumBitsTimeIncr);
    displayValue(pParInfo->bComplexEstDisable);
    displayValue(pParInfo->iIVOPQuantStep);
    displayValue(pParInfo->iPVOPQuantStep);
    displayValue(pParInfo->bQuantType);
    displayValue(pParInfo->bIntraQMatrixDefault);
    displayValue(pParInfo->bInterQMatrixDefault);
    displayValue(pParInfo->bResyncDisable);
    displayValue(pParInfo->bDataPartitioned);
    displayValue(pParInfo->bReverseVLC);
    displayValue(pParInfo->iVideoPacketLength);
    displayValue(pParInfo->bAdapativeIntraRefreshEnable);
    displayValue(pParInfo->iAdapativeIntraRefreshRate);
    displayValue(pParInfo->iPictureComplexity);
    displayValue(pParInfo->iRCType);
    displayValue(pParInfo->iSearchRange);
    displayValue(pParInfo->bUseSrcME);
    displayValue(pParInfo->iMEAlgorithm);
    displayValue(pParInfo->bVolControlPara);
    displayValue(pParInfo->bVbvParaEnable);
    displayValue(pParInfo->iVbvBitRate);
    displayValue(pParInfo->iVbvBufferSize);
    displayValue(pParInfo->iVbvOccupancy);
    displayValue(pParInfo->iMotionActivity);
    displayValue(pParInfo->iSlopeDelta);
    displayValue(pParInfo->iInflexionDelta);
    displayValue(pParInfo->iIVOPQPDelta);
    displayValue(pParInfo->iMaxQP);
    displayValue(pParInfo->iMinQP);
    displayValue(pParInfo->iDelayMaxFactor);
    displayValue(pParInfo->iDelayMinFactor);
    displayValue(pParInfo->iModelK);
    displayValue(pParInfo->iModelC);
    displayValue(pParInfo->iMEStrategy);
    IPP_Printf("========================\t\t\t\t=======================\n");
    
}

/***************************************************************************
// Name:             GetPar_MPEG4Enc
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

IPPCODECFUN(IppCodecStatus, GetPar_MPEG4Enc)(IppParSet *pParInfo, IppISConfig *pISConfig, int* pISEnable, FILE *pfPara)
{
	/* all the parameters to the encoder*/
	FILE *m_fp;
	int  uiFrmWidth, uiFrmHeight;
	char *pchRawDataFileName, *pchCompressedDataFileName, *pchInputDir, *pchOutputDir;
	char pchBuf[TOKEN_SIZE], pchName[TOKEN_SIZE];
	int  rguiBitsBudget;
	int  rgdFrameFrequency;	
	int	 iSamplingRate;
	int  ColorFormat;
	int	 bRateControl;
	int  iBorderExt;
	int  iMaxJitter;
	int  iPanningType;
	int  iLen, er = IPP_STATUS_NOERR;
	char chBuf[80], *pch, *pchEnd;
	
	//Set Default Value
	pchInputDir = NULL;
	pchOutputDir = NULL;
	pchRawDataFileName = NULL;
	pchCompressedDataFileName = NULL;
	rgdFrameFrequency = 1;
	iSamplingRate = 1;
	uiFrmWidth = 176;
	uiFrmHeight = 144;
	rguiBitsBudget = 300000;
	bRateControl = 0;
	ColorFormat = IPP_YCbCr411;
	iBorderExt = 0;
	iMaxJitter = 10;
	iPanningType = 0;

    er = ERES_OK;

    /*Init MPEG4 Encoder Par*/
	UpdateStandardPar_MPEG4(pParInfo);
	
    /*Get Parameters from Par file*/
	pch = IPP_Fgets(chBuf, 80, pfPara);
	if( strncmp("!!!INTEL!!!", chBuf, 11)!=0 ){
		IPP_Printf("Not a valid parameter file!");	
		er = ERES_OK;
	}
	m_fp = pfPara;
	while (!(parGetToken(m_fp, pchBuf)==ERES_EOF)){

		strcpy(pchName, pchBuf);
		if(parGetToken(m_fp, pchBuf)==ERES_EOF)
			er = ERES_FORMAT;

		if(pchBuf[0]!='=')
			er = ERES_FORMAT;

		if(parGetToken(m_fp, pchBuf)==ERES_EOF)
			er = ERES_FORMAT;

		if (IPP_Strcmp(pchName, "Source.Directory") == 0){				
			iLen = strlen(pchBuf) + 1;
            if(pchInputDir){
                goto EndofPara_MPEG4Enc;
            }
            /*
            SKIP
			IPP_MemCalloc(&pchInputDir, iLen*sizeof(char), 4);
			memcpy(pchInputDir, pchBuf, iLen);
            */

		}else if(IPP_Strcmp(pchName, "Source.RawDataFile") == 0){
			iLen = strlen(pchBuf) + 1;
            if(pchRawDataFileName){
                goto EndofPara_MPEG4Enc;
            }
            /*
            SKIP
			IPP_MemCalloc(&pchRawDataFileName, iLen*sizeof(char), 4);
			memcpy(pchRawDataFileName, pchBuf, iLen);
            */

		}else if(IPP_Strcmp(pchName, "Output.Directory") == 0){
			iLen = strlen(pchBuf) + 1;

            if(pchOutputDir){
                goto EndofPara_MPEG4Enc;
            }
            /*
            SKIP
			IPP_MemCalloc(pchOutputDir, iLen*sizeof(char), 4);
			memcpy(pchOutputDir, pchBuf, iLen);
            */

		}else if(IPP_Strcmp(pchName, "Output.CompressedBitstream") == 0){
			iLen = strlen(pchBuf) + 1;
            if(pchCompressedDataFileName){
                goto EndofPara_MPEG4Enc;
            }
            /*
            SKIP
			IPP_MemCalloc(&pchCompressedDataFileName, iLen*sizeof(char), 4);
			memcpy(pchCompressedDataFileName, pchBuf, iLen);
            */

		}else if(IPP_Strcmp(pchName, "Source.FrameRate") == 0){
			rgdFrameFrequency = strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "Source.SamplingRate") == 0){
			iSamplingRate = strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "Source.Width") == 0){
			uiFrmWidth = strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "Source.Height") == 0){
			uiFrmHeight = strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "RateControl.BitRate") == 0){
			rguiBitsBudget = strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "RateControl.MaxBitRate") == 0){ //RC_VT
			iGroRCMaxBitRate = strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "RateControl.Enable") == 0){
			bRateControl = strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "Stabilizer.Enable") == 0){
			*pISEnable = strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "Stabilizer.BorderExtend") == 0){
			iBorderExt = strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "Stabilizer.MaxJitter") == 0){
			iMaxJitter = strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "Stabilizer.PanningType") == 0){
			if(IPP_Strcmp(pchBuf, "STILL")==0)
				iPanningType = 0;
			else if(IPP_Strcmp(pchBuf, "LOW")==0)
				iPanningType = 1;
			else if(IPP_Strcmp(pchBuf, "VARIANT")==0)
				iPanningType = 2;
			else {
				IPP_Printf("Not supported Paning Type!\n");
				er = ERES_FORMAT;
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
		}else if(IPP_Strcmp(pchName, "iPictureComplexity") == 0){
			pParInfo->iPictureComplexity = strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "bObjectLayerIdentifier") == 0){
			pParInfo->bObjectLayerIdentifier = strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "iVolVerID") == 0){
			pParInfo->iVolVerID = strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "iVolPriority") == 0){
			pParInfo->iVolPriority = strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "bVolRandomAcess") == 0){
			pParInfo->bVolRandomAcess = strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "iVideoObjectTypeID") == 0){
			pParInfo->iVideoObjectTypeID = strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "bQuantType") == 0){
			pParInfo->bQuantType = strtol(pchBuf, &pchEnd, 10);
			if (pParInfo->bQuantType != 1) {
				pParInfo->bQuantType = 0;
			}
			
		}else if(IPP_Strcmp(pchName, "bIntraQMatrixDefault") == 0){
			pParInfo->bIntraQMatrixDefault = strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "bInterQMatrixDefault") == 0){
			pParInfo->bInterQMatrixDefault = strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "bResyncDisable") == 0){
			pParInfo->bResyncDisable = strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "bDataPartitioned") == 0){
			pParInfo->bDataPartitioned = strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "iVideoPacketLength") == 0){
			pParInfo->iVideoPacketLength = strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "bReverseVLC") == 0){
			pParInfo->bReverseVLC = strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "bAdapativeIntraRefreshEnable") == 0){
			pParInfo->bAdapativeIntraRefreshEnable = strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "iAdapativeIntraRefreshRate") == 0){
			pParInfo->iAdapativeIntraRefreshRate = strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "bVolControlPara") == 0){
			pParInfo->bVolControlPara = strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "bVbvParaEnable") == 0){
			pParInfo->bVbvParaEnable = strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "iVbvBitRate") == 0){
			pParInfo->iVbvBitRate = strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "iVbvBufferSize") == 0){
			pParInfo->iVbvBufferSize = strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "iVbvOccupancy") == 0){
			pParInfo->iVbvOccupancy = strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "iRCType") == 0){
			if(IPP_Strcmp(pchBuf, "NONE")==0){
				pParInfo->iRCType = RC_NULL;
			}else if(IPP_Strcmp(pchBuf, "MP4") == 0){
				pParInfo->iRCType = RC_MPEG4;
			}else if(IPP_Strcmp(pchBuf, "TMN8") == 0){
				pParInfo->iRCType = RC_TMN8;
			}else if(IPP_Strcmp(pchBuf, "NEWTONRAPSON") == 0){
				pParInfo->iRCType = RC_NEWTONRAPSON;
			}else if(IPP_Strcmp(pchBuf, "VT") == 0){
				pParInfo->iRCType = RC_VT; //RC_VT
			}else {
				IPP_Printf("Unknown RCType !\n");
			}
		}else if(IPP_Strcmp(pchName, "iSearchRange") == 0){
			pParInfo->iSearchRange = strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "bUseSrcME") == 0){
			pParInfo->bUseSrcME = strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "iMEAlgorithm") == 0){
			pParInfo->iMEAlgorithm = strtol(pchBuf, &pchEnd, 10);
			if(IPP_Strcmp(pchBuf, "MVFAST")==0)
				pParInfo->iMEAlgorithm = 0;
			else if(IPP_Strcmp(pchBuf, "SEA")==0)
				pParInfo->iMEAlgorithm = 1;
			else if(IPP_Strcmp(pchBuf, "SEA_CONCAN")==0)
				pParInfo->iMEAlgorithm = 2;
			else {
				IPP_Printf("Unknown ME Algorithm Type\n");
			}
		}else if(IPP_Strcmp(pchName, "iMEStrategy") == 0){
			pParInfo->iMEStrategy = strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "iPBetweenI") == 0){
			pParInfo->iPBetweenI = strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "iIntraDcVlcThr") == 0){
			pParInfo->iIntraDcVlcThr = strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "iIVOPQuantStep") == 0){
			pParInfo->iIVOPQuantStep = (Ipp8u)strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "iPVOPQuantStep") == 0){
			pParInfo->iPVOPQuantStep = (Ipp8u)strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "bRoundingCtrlEnable") == 0){
			pParInfo->bRoundingCtrlEnable = strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "bComplexEstDisable") == 0){
			pParInfo->bComplexEstDisable = strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "iRoundingCtrlInit") == 0){
			pParInfo->iRoundingCtrlInit = strtol(pchBuf, &pchEnd, 10);
		}else{
			er = ERES_FORMAT;
		}
	};

	if(er != ERES_OK) {
		IPP_Printf("error %d at some line of parameter file\n", er );
		return IPP_STATUS_INPUT_ERR;
	}
	
	/* set pInfo values */

	pParInfo->iVolWidth = uiFrmWidth;
    
    pParInfo->iVolHeight = uiFrmHeight;	

   	pParInfo->iSrcFrmRate = rgdFrameFrequency;
	if (pParInfo->iSrcFrmRate < 0) {
		pParInfo->iSrcFrmRate = 20;
	}
	pParInfo->iSrcSamRate = iSamplingRate;

    /* rate control algorithm 
		Currently, only NEWTON-RAPHSON is supported, so
		this value should be fixed to 0 */
    pParInfo->bRCEnable = bRateControl;
 //   if (pParInfo->bRCEnable) {
 //       pParInfo->iRCType = RC_NEWTONRAPSON;
	//} 

	//RC_VT
	if (!pParInfo->bRCEnable) {
		pParInfo->iRCType = RC_NULL;
	} else {
		if (RC_VT != pParInfo->iRCType) {
			pParInfo->iRCType = RC_NEWTONRAPSON;
		}
	}
	//RC_VT

	pParInfo->iRCBitRate = rguiBitsBudget;
	pParInfo->iColorFormat = ColorFormat;
    if(IPP_YCbCr411 != pParInfo->iColorFormat){
        er = IPP_STATUS_INPUT_ERR;
        goto EndofPara_MPEG4Enc;
    }

	pISConfig->iBorderExtension	= iBorderExt;
	pISConfig->iMaxJitter		= iMaxJitter;

	switch ( iPanningType ) {
	    case 0: // camera is put at fixed position
		    pISConfig->iPanningVar = 0;
		    pISConfig->iTrackSpeed = 0;
		    pISConfig->iTrackErrThreshold	= 0;
		    break;
	    case 1: // camera has slow panning
		    pISConfig->iPanningVar = 1;
		    pISConfig->iTrackSpeed = 200;
		    pISConfig->iTrackErrThreshold	= 8;
		    break;
	    case 2: // camera has variant panning
	    default:
		    pISConfig->iPanningVar = 3;
		    pISConfig->iTrackSpeed = 500;
		    pISConfig->iTrackErrThreshold	= 8;
		    break;
	}
   
    if (ME_PERFORMANCE_BIAS == pParInfo->iMEStrategy) {
        pParInfo->MEFlowContrl.bBlockSearchDisabled      = 1;    
        pParInfo->MEFlowContrl.iEarlyExitThresForZMD    = 1024;
        pParInfo->MEFlowContrl.iFavorIntra              = -512;
        pParInfo->MEFlowContrl.iEarlyExitThresInt16     = 0;
        pParInfo->MEFlowContrl.iEarlyExitThresInt8      = 0;
        pParInfo->MEFlowContrl.iEarlyExitThresHalf16    = 0;
    } else {
        pParInfo->MEFlowContrl.bBlockSearchDisabled      = 0;    
        pParInfo->MEFlowContrl.iEarlyExitThresForZMD    = 1024;
        pParInfo->MEFlowContrl.iFavorIntra              = -512;
        pParInfo->MEFlowContrl.iEarlyExitThresInt16     = 0;
        pParInfo->MEFlowContrl.iEarlyExitThresInt8      = 0;
        pParInfo->MEFlowContrl.iEarlyExitThresHalf16    = 0;    
    } 

EndofPara_MPEG4Enc:

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
    
    /*Debug Show*/
    OutputPara(pParInfo);

	return er;
}

/*Parsing Cmd line for MPEG4 Encoder.*/
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
int ParseCmd_MPEGEnc(char *pCmdLine, 
                     char *pSrcFileName, 
                     char *pDstFileName, 
                     char *pLogFileName, 
                     /*IppParSet*/void *pParInfo, 
                     /*IppISConfig*/void *pISConfig,
                     int   *pISEnable)
{
    char *pCur, *pEnd;
    char par_name[MAX_PAR_NAME_LEN];
//    char par_value[MAX_PAR_VALUE_LEN];
    char *p1 = NULL, *p2 = NULL, *p3 = NULL;

    int  par_name_len         = 0;
    int  par_value_len        = 0;
  
    IppCodecStatus   ret      = IPP_STATUS_NOERR;

    /*Par file*/
    char pParFileName[128]    = {"\0"};
    FILE *fpar                = NULL;
    char cTmp[64]             = {"\0"};
    IppParSet  *pPar          = (IppParSet*)pParInfo;

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

            GetPar_MPEG4Enc(pParInfo, pISConfig, pISEnable, fpar);

            if(fpar){
                IPP_Fclose(fpar);
            }

            /*1st Level Parameter*/
        } else if ((0 == IPP_Strcmp(par_name, "Width"))){
            STRNCPY(cTmp, p2 + 1, par_value_len);
            pPar->iVolWidth = IPP_Atoi(cTmp);
        } else if ((0 == IPP_Strcmp(par_name, "Height"))){
            /*parse other parameters for encoder*/
            STRNCPY(cTmp, p2 + 1, par_value_len);
            pPar->iVolHeight = IPP_Atoi(cTmp);
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
        } else if ((0 == IPP_Strcmp(par_name, "iMaxBitRate"))){ 
            /*parse other parameters for encoder*/
            STRNCPY(cTmp, p2 + 1, par_value_len);
            iGroRCMaxBitRate = IPP_Atoi(cTmp);
        } else if ((0 == IPP_Strcmp(par_name, "iIntraDcVlcThr"))){ 
            /*parse other parameters for encoder*/
            STRNCPY(cTmp, p2 + 1, par_value_len);
            pPar->iIntraDcVlcThr = IPP_Atoi(cTmp);
        } else if ((0 == IPP_Strcmp(par_name, "bRoundingCtrlEnable"))){ 
            /*parse other parameters for encoder*/
            STRNCPY(cTmp, p2 + 1, par_value_len);
            pPar->bRoundingCtrlEnable = IPP_Atoi(cTmp);
        } else if ((0 == IPP_Strcmp(par_name, "iPBetweenI"))){ 
            /*parse other parameters for encoder*/
            STRNCPY(cTmp, p2 + 1, par_value_len);
            pPar->iPBetweenI = IPP_Atoi(cTmp);

        } else if ((0 == IPP_Strcmp(par_name, "iIVOPQuantStep"))){ 
            /*parse other parameters for encoder*/
            STRNCPY(cTmp, p2 + 1, par_value_len);
            pPar->iIVOPQuantStep = (Ipp8u)(IPP_Atoi(cTmp));
        } else if ((0 == IPP_Strcmp(par_name, "iPVOPQuantStep"))){ 
            /*parse other parameters for encoder*/
            STRNCPY(cTmp, p2 + 1, par_value_len);
            pPar->iPVOPQuantStep = (Ipp8u)(IPP_Atoi(cTmp));
        } else if ((0 == IPP_Strcmp(par_name, "bQuantType"))){ 
            /*parse other parameters for encoder*/
            STRNCPY(cTmp, p2 + 1, par_value_len);
            if(IPP_Strcmp(cTmp, "MPEG4") == 0){
                pPar->bQuantType = 1;
            }else{
                pPar->bQuantType = 0;
            }
        } else if ((0 == IPP_Strcmp(par_name, "bIntraQMatrixDefault"))){ 
            /*parse other parameters for encoder*/
            STRNCPY(cTmp, p2 + 1, par_value_len);
            pPar->bIntraQMatrixDefault = IPP_Atoi(cTmp);
        } else if ((0 == IPP_Strcmp(par_name, "bInterQMatrixDefault"))){ 
            /*parse other parameters for encoder*/
            STRNCPY(cTmp, p2 + 1, par_value_len);
            pPar->bInterQMatrixDefault = IPP_Atoi(cTmp);

        } else if ((0 == IPP_Strcmp(par_name, "bResyncDisable"))){ 
            /*parse other parameters for encoder*/
            STRNCPY(cTmp, p2 + 1, par_value_len);
            pPar->bResyncDisable = IPP_Atoi(cTmp);
        } else if ((0 == IPP_Strcmp(par_name, "bDataPartitioned"))){ 
            /*parse other parameters for encoder*/
            STRNCPY(cTmp, p2 + 1, par_value_len);
            pPar->bDataPartitioned = IPP_Atoi(cTmp);

        } else if ((0 == IPP_Strcmp(par_name, "bReverseVLC"))){ 
            /*parse other parameters for encoder*/
            STRNCPY(cTmp, p2 + 1, par_value_len);
            pPar->bReverseVLC = IPP_Atoi(cTmp);

        } else if ((0 == IPP_Strcmp(par_name, "iVideoPacketLength"))){ 
            /*parse other parameters for encoder*/
            STRNCPY(cTmp, p2 + 1, par_value_len);
            pPar->iVideoPacketLength = IPP_Atoi(cTmp);
        } else if ((0 == IPP_Strcmp(par_name, "bAdapativeIntraRefreshEnable"))){ 
            /*parse other parameters for encoder*/
            STRNCPY(cTmp, p2 + 1, par_value_len);
            pPar->bAdapativeIntraRefreshEnable = IPP_Atoi(cTmp);
        } else if ((0 == IPP_Strcmp(par_name, "iAdapativeIntraRefreshRate"))){ 
            /*parse other parameters for encoder*/
            STRNCPY(cTmp, p2 + 1, par_value_len);
            pPar->iAdapativeIntraRefreshRate = IPP_Atoi(cTmp);
        } else if ((0 == IPP_Strcmp(par_name, "iPictureComplexity"))){ 
            /*parse other parameters for encoder*/
            STRNCPY(cTmp, p2 + 1, par_value_len);
            pPar->iPictureComplexity = IPP_Atoi(cTmp);


        /*3rd Level Parameter*/
        } else if ((0 == IPP_Strcmp(par_name, "iSearchRange"))){ 
            /*parse other parameters for encoder*/
            STRNCPY(cTmp, p2 + 1, par_value_len);
            pPar->iSearchRange = IPP_Atoi(cTmp);
        } else if ((0 == IPP_Strcmp(par_name, "iVbvBitRate"))){ 
            /*parse other parameters for encoder*/
            STRNCPY(cTmp, p2 + 1, par_value_len);
            pPar->iVbvBitRate = IPP_Atoi(cTmp);
        } else if ((0 == IPP_Strcmp(par_name, "iVbvBufferSize"))){ 
            /*parse other parameters for encoder*/
            STRNCPY(cTmp, p2 + 1, par_value_len);
            pPar->iVbvBufferSize = IPP_Atoi(cTmp);
        } else if ((0 == IPP_Strcmp(par_name, "iVbvOccupancy"))){ 
            /*parse other parameters for encoder*/
            STRNCPY(cTmp, p2 + 1, par_value_len);
            pPar->iVbvOccupancy = IPP_Atoi(cTmp);
        } else if ((0 == IPP_Strcmp(par_name, "iMEStrategy"))){ 
            /*parse other parameters for encoder*/
            STRNCPY(cTmp, p2 + 1, par_value_len);
            pPar->iMEStrategy = IPP_Atoi(cTmp);

        } else {
            /*adding no such  option*/
            STRNCPY(cTmp, p2 + 1, par_value_len);
            IPP_Printf("No Such Optional: %s:::%s, Skipped\n", par_name, cTmp);
            //return IPP_STATUS_ERR;
        }

        pCur = p3;
    }

 //   if (pPar->bRCEnable) {
 //       pPar->iRCType = RC_NEWTONRAPSON;
	//} 

	//RC_VT
	if (!pPar->bRCEnable) {
		pPar->iRCType = RC_NULL;
	} else {
		if (RC_VT != pPar->iRCType) {
			pPar->iRCType = RC_NEWTONRAPSON;
		}
	}
	//RC_VT

    if (ME_PERFORMANCE_BIAS == pPar->iMEStrategy) {
        pPar->MEFlowContrl.bBlockSearchDisabled      = 1;    
        pPar->MEFlowContrl.iEarlyExitThresForZMD    = 1024;
        pPar->MEFlowContrl.iFavorIntra              = -512;
        pPar->MEFlowContrl.iEarlyExitThresInt16     = 0;
        pPar->MEFlowContrl.iEarlyExitThresInt8      = 0;
        pPar->MEFlowContrl.iEarlyExitThresHalf16    = 0;
    } else {
        pPar->MEFlowContrl.bBlockSearchDisabled      = 0;    
        pPar->MEFlowContrl.iEarlyExitThresForZMD    = 1024;
        pPar->MEFlowContrl.iFavorIntra              = -512;
        pPar->MEFlowContrl.iEarlyExitThresInt16     = 0;
        pPar->MEFlowContrl.iEarlyExitThresInt8      = 0;
        pPar->MEFlowContrl.iEarlyExitThresHalf16    = 0;    
    } 

    return ret;
}

/*EOF*/