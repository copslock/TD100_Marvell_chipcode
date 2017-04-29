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

IPPCODECFUN(IppCodecStatus, UpdateStandardPar_H263)(void *pParInfo)
{

	IppH263ParSet * pH263EncoderPar = (IppH263ParSet *) pParInfo;
	
	/* Update standard defined codec configuration */
	pH263EncoderPar->iRCType = RC_TMN8;

	pH263EncoderPar->iPBetweenI = 20;
		/* Positive integer indicates number of P-frame between two neighbouring I-frames, 
		   -1 indicates just one I-frame in starting point, no more I-frame */
	
	pH263EncoderPar->iIntraQuant = 12;
		/* Legal Values:  1 - 31
		   If RC enable, it indicates the initialized QP of Intra-picture,
		   else it indicates the QP of all Intra-pictures
		*/

	pH263EncoderPar->iInterQuant = 14;
		/* Legal Values:  1 - 31
		   If RC enable, it indicates the initialized QP of Inter-picture,
		   else it indicates the QP of all Inter-pictures
		*/

	pH263EncoderPar->bRoundingCtrlEnable = 0;
		/* 2 Option: 1 -> Enable; 0 -> Disable; */

	pH263EncoderPar->GOBHeaderPresent = 0;
		/* 2 Option: 1 -> Enable; 0 -> Disable; */
		/* If enabled in GOB mode, every encoded GOB will have a GOBheader
		   This flag has no influence in Slice Structured mode
		*/

/* Annex Options */
	pH263EncoderPar->optUMV = 0;
		/* 2 Option: 1 -> Enable; 0 -> Disable; */

	pH263EncoderPar->optSAC = 0;
		/* 2 Option: 1 -> Enable; 0 -> Disable; */

	pH263EncoderPar->optAP = 0;
		/* 2 Option: 1 -> Enable; 0 -> Disable; */

	pH263EncoderPar->optPBframes = 0;
		/* 2 Option: 1 -> Enable; 0 -> Disable; */

	pH263EncoderPar->optAIC = 0;
		/* 2 Option: 1 -> Enable; 0 -> Disable; */

	/* RC_VT for Borqs case, sync. between OpenMAX and lib */
	//pH263EncoderPar->optDF = 1;
	pH263EncoderPar->optDF = 0;
		/* 2 Option: 1 -> Enable; 0 -> Disable; */

	pH263EncoderPar->optSS = 0;
		/* 2 Option: 1 -> Enable; 0 -> Disable; */
	pH263EncoderPar->BitsPerSlice = 2000;
		/* Threshold of Bits per slice when in SS mode */

	pH263EncoderPar->optRPS = 0;
		/* 2 Option: 1 -> Enable; 0 -> Disable; */
	
	pH263EncoderPar->optISD = 0;
		/* 2 Option: 1 -> Enable; 0 -> Disable; */

	pH263EncoderPar->optAIV = 0;
		/* 2 Option: 1 -> Enable; 0 -> Disable; */

	/* RC_VT for Borqs case, sync. between OpenMAX and lib */
	//pH263EncoderPar->optMQ = 1;
	pH263EncoderPar->optMQ = 0;
		/* 2 Option: 1 -> Enable; 0 -> Disable; */

	pH263EncoderPar->optRPR = 0;
		/* 2 Option: 1 -> Enable; 0 -> Disable; */

	pH263EncoderPar->optRRU = 0;
		/* 2 Option: 1 -> Enable; 0 -> Disable; */

	pH263EncoderPar->iSearchRange = 15;
		/* Positive Integer, Search Range of Motion Estimation */

	pH263EncoderPar->iMEAlgorithm = 0;
		/* Motion Estimation Algorithm Selection
		   2 Option:	0 -> MVFAST;
						1 -> SEA;
		*/

    pH263EncoderPar->iMEStrategy = ME_QUALITY_BIAS;

    pH263EncoderPar->iMaxQP = 31;
    pH263EncoderPar->iMinQP = 1; // sync. between codec and OMXIL

		
	return IPP_STATUS_NOERR;

}


IPPCODECFUN(IppCodecStatus, UpdateAlgorithmPar_External_H263)(void *pParInfo)
{

	IppH263ParSet *pH263EncoderPar = (IppH263ParSet *) pParInfo;

	//pH263EncoderPar->bAlgorithmFromCustomer = 1;
	//	/* 2 Option: 1 -> Enable; 0 -> Disable; */	

    if(pH263EncoderPar->iIPictureQPDelta > 31 || (pH263EncoderPar->iIPictureQPDelta < -31)){
        return IPP_STATUS_ERR;
    }

	if(1 == pH263EncoderPar->bRCEnable) {
		pH263EncoderPar->iVbvBufferSize  = 50;
			/* 
			// Percentage value. must be ">0"  
			// real buffer = pMP4EncoderPar->iVbvBufferSize * pMP4EncoderPar->iRCBitRate / 100
			// 50, 100	
			// Recommend: 50  <--->  0.5 second delay  
			*/	
		pH263EncoderPar->iVbvOccupancy   = 0;
			/*
			// Percentage value, valid [0, 100].  
			// real occupancy = pMP4EncoderPar->iVbvOccupancy * real buffer / 100
			// 0, 50 or 200/3 
			// Corresponding buffer position: 0, 1/2, 2/3 
			// Recommend: 0 
			*/	

		pH263EncoderPar->iMotionActivity = MOTION_MEDIUM;
		pH263EncoderPar->iSlopeDelta = 0;
			/*
			// Valid value:  [-52428, 52428]
			*/
		
		pH263EncoderPar->iInflexionDelta = 0;  
			/*
			// Valid value:  [-PBetweenI/2, PBetweenI/2]
			*/
		pH263EncoderPar->iIPictureQPDelta = 0;
			/*
			// Valid value:  [-31, 31]
			*/

		pH263EncoderPar->iMaxQP = 31;
		pH263EncoderPar->iMinQP = 1;
			/*
			// Valid value:  [1, 31] and iMaxQP shoud be larger than iMinQP
			*/

		pH263EncoderPar->iDelayMaxFactor = 90;  /* means 90% of whole buffer */ 
		pH263EncoderPar->iDelayMinFactor = 10;  /* means 10% of whole buffer */
			/*
			// Valid value:  [0, 100] and iDelayMaxFactor should be larger than iDelayMinFactor
			*/

		pH263EncoderPar->iModelK = 32768;  /*  0.5 << 16, real value is 0.5 */ 
		pH263EncoderPar->iModelC = 0;      /*    0 << 16  real value is 0   */
			/*
			// standard recommended value.
			*/
	}
 
	/*********************************************
     *    Motion Estimation Strategy selection   *
     *    ME_QUALITY_BIAS  prefer quality        *
     *    ME_PERFORMANCE_BIAS prefer performance *
     *********************************************/

    if (pH263EncoderPar->optDF || pH263EncoderPar->optUMV || pH263EncoderPar->optAP){
 		/* 4 MV */
		if (ME_PERFORMANCE_BIAS == pH263EncoderPar->iMEStrategy) {
			pH263EncoderPar->MEFlowContrl.bBlockSearchDisabled      = 1; 
			pH263EncoderPar->MEFlowContrl.iEarlyExitThresForZMD    = 1024;
			pH263EncoderPar->MEFlowContrl.iFavorIntra              = -512;
			pH263EncoderPar->MEFlowContrl.iEarlyExitThresInt16     = 1024;
			pH263EncoderPar->MEFlowContrl.iEarlyExitThresInt8      = 0;
			pH263EncoderPar->MEFlowContrl.iEarlyExitThresHalf16    = 0;
		} else {
			pH263EncoderPar->MEFlowContrl.bBlockSearchDisabled      = 0; 
			pH263EncoderPar->MEFlowContrl.iEarlyExitThresForZMD    = 1024;
			pH263EncoderPar->MEFlowContrl.iFavorIntra              = -512;            
			pH263EncoderPar->MEFlowContrl.iEarlyExitThresInt16     = 1024;
			pH263EncoderPar->MEFlowContrl.iEarlyExitThresInt8      = 0;
			pH263EncoderPar->MEFlowContrl.iEarlyExitThresHalf16    = 0;    
		} 
    } else {
		/* 1 MV */
		pH263EncoderPar->MEFlowContrl.bBlockSearchDisabled      = 1; 
		pH263EncoderPar->MEFlowContrl.iEarlyExitThresForZMD    = 1024;
		pH263EncoderPar->MEFlowContrl.iFavorIntra              = -512;
		pH263EncoderPar->MEFlowContrl.iEarlyExitThresInt16     = 1024;
		pH263EncoderPar->MEFlowContrl.iEarlyExitThresInt8      = 0;
		pH263EncoderPar->MEFlowContrl.iEarlyExitThresHalf16    = 0;
    }

	return IPP_STATUS_NOERR;

}
