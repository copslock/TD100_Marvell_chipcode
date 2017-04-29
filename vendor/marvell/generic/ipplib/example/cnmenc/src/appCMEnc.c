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

#include "codecCNM.h"
#include "misc.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "ippLV.h"

#ifdef _IPP_X86
#include "ctype.h"
#define cnm_os_api_dma_malloc(ppAddr,iSize, align)         IPP_MemMalloc(ppAddr,iSize, align)
#define cnm_os_api_dma_free(ppAddr)                        IPP_MemFree(ppAddr);
#define cnm_os_api_dma_get_pa(x)                                (x)
#define cnm_os_api_dma_get_va(x)                                (x)

#elif defined _IPP_LINUX
#include "cnm_bridge.h"



#elif defined _IPP_PPC
#error for PPC malloc non cache
#endif

/*Parsing Cmd line for MPEG4 Encoder.*/
#define TOKEN_SIZE 4096
#define ERES_OK 0
#define ERES_PARAMS 1
#define ERES_NOOBJ 2
#define ERES_MEMORY 3
#define ERES_EOF 4
#define ERES_FORMAT 5

#define    EOS_LEN    4
#define    EOS1       0       /* EOS data, byte 1,2,3,4 */
#define    EOS2       0
#define    EOS3       0x01
#define    EOS4       0xb6

#define VPU_LAST_FRAME            (0x10)/*indicat it's stream end of input.*/
#define VPU_BUFFER_USED           (0x1)/*buffer used flag*/
#define VPU_BUFFER_FULLY          (0x2)/*fully buffer, application only pull it again to encoder, rather than feed data*/

#define _Max_Num_                   (2)/*buffer numbers.*/

#define MAX_PAR_NAME_LEN    1024
#define MAX_PAR_VALUE_LEN   2048
#define MAX_BUF_SIZE        512*1024     /*temp bitstream buffer size*/
#define STRNCPY(dst, src, len) \
{\
    IPP_Strncpy((dst), (src), (len));\
    (dst)[(len)] = '\0';\
}

//NOTE: currently, IPP_Printf only support the string whose length < 1024
static char scHelpCodaDx8Enc[] = {
    "Marvell CodaDx8Enc Usage1: \n"
	"./App <-fmt:n> <-i:infilename> [-o:outfilename] [-l:logfilename] [-p:configfile] [-fmt:formant] [-pollingmode:0|1] [-dynaclk:0|1] [-pingpangmode:0|1] [-multiinstancesync:0|1] [-inputyuvmode:0|1]\n"
    "for fmt:\n(__MPG1_NoSupport  = 0)\n"
    "(__MPG2_NoSupport  = 1)\n"
    "(__MPG4_Support    = 2)\n"
    "(__H261_NoSupport  = 3)\n"
    "(__H263_Support    = 4)\n"
    "(__H264_Support    = 5)\n"
    "(__VC1_NoSupport   = 6)\n"
    "(__JPEG_NoSupport  = 7)\n"
    "(__MJPG_NoSupport  = 8)\n"
    "(__VP6_NoSupport   = 9)\n"
	"Marvell CodaDx8Enc Usage2: \n"
	"./App \"<-fmt:n> <-i:infilename> [-o:outfilename] [-l:logfilename] [-p:configfile] [-fmt:formant] [-pollingmode:0|1] [-dynaclk:0|1] [-pingpangmode:0|1] [-multiinstancesync:0|1] [-inputyuvmode:0|1]\"\n"
	"Sample1: ./App -fmt:5 -i:input.yuv -o:output.264 -p:config.par\n"
	"Sample2: ./App \"-i:input.yuv -l:mylog.log -fmt:5 -o:output.264 -p:config.par\"\n"
};

static char scformat[][8] = {
    "MPG1","MPG2","MPG4","H261","H263","H264","VC1","JPEG","MJPG","VP6"
};

#define Display(x)   IPP_Printf("       ["#x"]: == [%d]\n", x);

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
	ret = GetLibVersion_CNMENC(libversion,sizeof(libversion));
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

void OutputPara(IppVPUEncParSet *pParInfo)
{
	Display(pParInfo->iFormat);    
	Display(pParInfo->iWidth);
    Display(pParInfo->iHeight);
    Display(pParInfo->iFrameRate);
    Display(pParInfo->iGopSize);
    Display(pParInfo->iRCBitRate);
    Display(pParInfo->iRCMaxBitRate);	
	Display(pParInfo->iInitQp);
	Display(pParInfo->iDelayLimit);

	//Display(pParInfo->dynamicAllocEnable);
	//Display(pParInfo->ringBufferEnable);

    switch(pParInfo->iFormat){
        case IPP_VIDEO_STRM_FMT_MPG4:
            Display(pParInfo->IppEncStdParam.mp4Param.mp4_dataPartitionEnable);
            Display(pParInfo->IppEncStdParam.mp4Param.mp4_hecEnable);
            Display(pParInfo->IppEncStdParam.mp4Param.mp4_intraDcVlcThr);
            Display(pParInfo->IppEncStdParam.mp4Param.mp4_reversibleVlcEnable);
            Display(pParInfo->IppEncStdParam.mp4Param.mp4_verid);
            break;

        case IPP_VIDEO_STRM_FMT_H263:
            Display(pParInfo->IppEncStdParam.h263Param.h263_annexJEnable);
            Display(pParInfo->IppEncStdParam.h263Param.h263_annexKEnable);
            Display(pParInfo->IppEncStdParam.h263Param.h263_annexTEnable);
            break;

        case IPP_VIDEO_STRM_FMT_H264:
            Display(pParInfo->IppEncStdParam.avcParam.avc_bDeblockDisable)            
            break;
        default:
            break;
    }
}

/***************************************************************************
// Name:             UpdateStandardPar_VPUEnc
// Description:      default set for Encoder Configuration
//
// Input Arguments:
//      pPar         -- Pointer to parset information.
// Output Arguments:
//
//	Returns:	
*****************************************************************************/
void UpdateStandardPar_VPUEnc(IppVPUEncParSet *pPar)
{
    pPar->iWidth     = 48;
	pPar->iHeight    = 32;
	pPar->iFrameRate = 30;
	pPar->iGopSize   = 0;
	pPar->bRCEnable  = 1;
	pPar->iInitQp    = 10;
	pPar->iRCBitRate = 1024;
	pPar->iRCMaxBitRate = 1024*3/2;	
	pPar->iDelayLimit = 1000;
	pPar->bPollingMode = 0;
	pPar->bDynaClkEnable = 0;
	pPar->bPingPangMode  = 0;
	pPar->iMultiInstanceSync = 0;	
	pPar->iInputYUVMode = 0;
    return ;
}

/***************************************************************************
// Name:             parSkipSpace
// Description:      Skip the space in the input file
//
// Input Arguments:
//      m_fp		 Pointer to the input parameter file stream buffer
// Output Arguments:
//      m_fp		 Pointer to the updated input parameter file stream buffer
//
//	Returns:	
*****************************************************************************/
int parSkipSpace(FILE *m_fp)
{
	int ch;
	do {
		ch = getc(m_fp);
		if(ch==EOF)
			return ERES_EOF;
	} while(isspace(ch));

	ungetc(ch, m_fp);
	return ERES_OK;

}

/***************************************************************************
// Name:             parSkipComment
// Description:      Skip the comments in the input file
//
// Input Arguments:
//      m_fp		 Pointer to the input parameter file stream buffer
// Output Arguments:
//      m_fp		 Pointer to the updated input parameter file stream buffer
//
//	Returns:	
*****************************************************************************/
int parSkipComment(FILE *m_fp)		
{
	int ch;
	do {
		ch = getc(m_fp);
		if(ch==EOF)
			return ERES_EOF;

		if(ch!='/' && ch!='*')
		{
			ungetc(ch, m_fp);
			return ERES_NOOBJ;
		}

		if(ch=='/')
		{
			do {
				ch = getc(m_fp);
				if(ch==EOF)
					return ERES_EOF;
			} while(ch!='\n');
		}
		else if(ch=='*')
		{
			int iState = 0;			
			do {
				ch = getc(m_fp);
				if(ch==EOF)
					return ERES_EOF;
				if(iState==0 && ch=='*')
					iState = 1;
				else if(iState==1)
				{
					if(ch=='/')
						iState = 2;
					else if(ch!='*')
						iState = 0;
				}
			} while(iState!=2);
		}
		if(parSkipSpace(m_fp)==ERES_EOF) {
			return ERES_EOF;
		}
		ch = getc(m_fp);
		if(ch==EOF) {
			return ERES_EOF;
		}

	} while(ch=='/');
	
	ungetc(ch, m_fp);
	return ERES_OK;
}


/***************************************************************************
// Name:             parGetToken
// Description:      Get token to output buffer from input file
//
// Input Arguments:
//      m_fp		 Pointer to the input parameter file stream buffer
//		pchBuf		 Pointer to the output char buffer
// Output Arguments:
//      m_fp		 Pointer to the updated input parameter file stream buffer
//
//	Returns:	
*****************************************************************************/
int parGetToken(FILE *m_fp, char *pchBuf)   
{
	int ch;	
	int i=1,j=0;
	parSkipSpace(m_fp);
	ch = getc(m_fp);
	if(ch==EOF) {
		return ERES_EOF;
	}

	if(ch=='/')	{
		parSkipComment(m_fp);
		ch = getc(m_fp);
		if (ch==EOF)
			return ERES_EOF;
	} 
	if(ch=='=')
	{	
		pchBuf[0] = (char)ch;
		pchBuf[1] = '\0';
		return ERES_OK;
	}

	pchBuf[0] = (char)ch;

	if(ch=='\"')	{	
		ch = getc(m_fp);
		while (ch != '\"'){
			pchBuf[j] = (char)ch;
			ch = getc(m_fp);
			j++;
		}
		pchBuf[j] = '\0';
		return ERES_OK;
	} else{
		for(i=1;i<TOKEN_SIZE - 1;i++)
		{
			ch = getc(m_fp);
			if(ch==EOF) {
				pchBuf[i] = '\0';
				return ERES_EOF;
			}
			if(isspace(ch)||ch == '=')
			{
				pchBuf[i] = '\0';
				ungetc(ch, m_fp);
				return ERES_OK;
			}

			if(ch=='/' && parSkipComment(m_fp)!=ERES_NOOBJ)
			{
				pchBuf[i] = '\0';
				return ERES_OK;
			}
			pchBuf[i] = (char)ch;
		}
	}
	pchBuf[i] = '\0';
	return ERES_OK;

}

#define TEST

#ifdef TEST
#define _Ipp_CheckStd(x, y/*std*/, z)   {}
#else
#define _Ipp_CheckStd(x, y/*std*/, z)                           \
{                                                               \
    if(x != y ){                                                \
        IPP_Printf(#z" Is Not Supported by %s\n", scformat[x]); \
        er = ERES_FORMAT;                                       \
        goto EndOfParsing;                                      \
    }                                                           \
}
#endif
/***************************************************************************
// Name:             GetPar_VPUEnc
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
int GetPar_VPUEnc(IppVPUEncParSet *pParInfo, char *pParName)
{
    /* all the parameters to the encoder*/
	FILE *m_fp;
	
	int  uiFrmWidth=0;
	int  uiFrmHeight=0;
	char pchBuf[TOKEN_SIZE], pchName[TOKEN_SIZE];
	
	int  er;
	char chBuf[80], *pch, *pchEnd;
    FILE *pfPara = NULL;
	/*Set Default Value*/

    /*Open Par file*/
    pfPara = IPP_Fopen(pParName, "rb");
    if ( !pfPara ) {
        IPP_Printf("Error: IPP_Fopen %s \n", pParName);
        return -1;
    }

    er = ERES_OK;
	m_fp = pfPara;
	while (!(parGetToken(m_fp, pchBuf)==ERES_EOF)){

		IPP_Strcpy(pchName, pchBuf);

		if(parGetToken(m_fp, pchBuf)==ERES_EOF)
			er = ERES_FORMAT;

		if(pchBuf[0]!='=')
			er = ERES_FORMAT;

		if(parGetToken(m_fp, pchBuf)==ERES_EOF)
			er = ERES_FORMAT;

        /*Common Setting for Encoder*/
        if(IPP_Strcmp(pchName, "iFormat") == 0){
			pParInfo->iFormat = strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "iWidth") == 0){
			pParInfo->iWidth = strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "iHeight") == 0){
			pParInfo->iHeight = strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "iFrameRate") == 0){
			pParInfo->iFrameRate = strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "bRCEnable") == 0){
			pParInfo->bRCEnable = strtol(pchBuf, &pchEnd, 10);
	    }else if(IPP_Strcmp(pchName, "iRCBitRate") == 0){
			pParInfo->iRCBitRate = strtol(pchBuf, &pchEnd, 10);
		}else if(IPP_Strcmp(pchName, "iRCMaxBitRate") == 0){
			pParInfo->iRCMaxBitRate = strtol(pchBuf, &pchEnd, 10);
	    }else if(IPP_Strcmp(pchName, "iDelayLimit") == 0){
            pParInfo->iDelayLimit = strtol(pchBuf, &pchEnd, 10);
        }else if(IPP_Strcmp(pchName, "iGopSize") == 0){
            /*0: Only One I , 1: All I, 2: IPIP, 3: IPPIPP*/
            pParInfo->iGopSize = strtol(pchBuf, &pchEnd, 10);      
        }else if(IPP_Strcmp(pchName, "iInitQp") == 0){
            pParInfo->iInitQp = strtol(pchBuf, &pchEnd, 10);
        }else if(IPP_Strcmp(pchName, "iRCType") == 0){
            pParInfo->iRCType = strtol(pchBuf, &pchEnd, 10);

        /*H264 Specific Features.*/
        }else if(IPP_Strcmp(pchName, "bDeblockEnable") == 0){
            pParInfo->IppEncStdParam.avcParam.avc_bDeblockDisable = !(strtol(pchBuf, &pchEnd, 10));
            _Ipp_CheckStd(pParInfo->iFormat, IPP_VIDEO_STRM_FMT_H264/*std*/, bDeblockEnable);
     
        /*H263 Specific Features: Annex I is not supportted.*/
        }else if(IPP_Strcmp(pchName, /*"h263_annexJEnable"*/"bDFEnable") == 0){ /* Deblock Filter Mode*/
            pParInfo->IppEncStdParam.h263Param.h263_annexJEnable = strtol(pchBuf, &pchEnd, 10);
            _Ipp_CheckStd(pParInfo->iFormat , IPP_VIDEO_STRM_FMT_H263/*std*/, h263_annexJEnable);
        }else if(IPP_Strcmp(pchName, /*"h263_annexKEnable"*/"bSSEnable") == 0){ /* Slice Structured Mode:RS=0, ASO=0*/
            pParInfo->IppEncStdParam.h263Param.h263_annexKEnable = strtol(pchBuf, &pchEnd, 10);
            _Ipp_CheckStd(pParInfo->iFormat, IPP_VIDEO_STRM_FMT_H263/*std*/, h263_annexKEnable);
        }else if(IPP_Strcmp(pchName, /*"h263_annexTEnable"*/"bMQEnable") == 0){ /* Modified Quantization Mode*/
            pParInfo->IppEncStdParam.h263Param.h263_annexTEnable = strtol(pchBuf, &pchEnd, 10);
            _Ipp_CheckStd(pParInfo->iFormat, IPP_VIDEO_STRM_FMT_H263/*std*/, h263_annexTEnable);
        
            /*MPEG4 Specific Features.*/
        }else if(IPP_Strcmp(pchName, "bDataPartitioned") == 0){
            pParInfo->IppEncStdParam.mp4Param.mp4_dataPartitionEnable = strtol(pchBuf, &pchEnd, 10);
            _Ipp_CheckStd(pParInfo->iFormat , IPP_VIDEO_STRM_FMT_MPG4/*std*/, bDataPartitioned);
        }else if(IPP_Strcmp(pchName, "bReverseVLC") == 0){
            pParInfo->IppEncStdParam.mp4Param.mp4_reversibleVlcEnable = strtol(pchBuf, &pchEnd, 10);
            _Ipp_CheckStd(pParInfo->iFormat, IPP_VIDEO_STRM_FMT_MPG4/*std*/, bReverseVLC);
        }else if(IPP_Strcmp(pchName, "iIntraDcVlcThr") == 0){
            /*Meaning of intra_dc_vlc_thr: 0 -7*/
            pParInfo->IppEncStdParam.mp4Param.mp4_intraDcVlcThr = strtol(pchBuf, &pchEnd, 10);
            _Ipp_CheckStd(pParInfo->iFormat, IPP_VIDEO_STRM_FMT_MPG4/*std*/, iIntraDcVlcThr);
        }else if(IPP_Strcmp(pchName, "bHecEnable") == 0){/*0.1 for externsion code of VOP type, time increment*/
            pParInfo->IppEncStdParam.mp4Param.mp4_hecEnable = strtol(pchBuf, &pchEnd, 10);
            _Ipp_CheckStd(pParInfo->iFormat, IPP_VIDEO_STRM_FMT_MPG4/*std*/, bHecEnable);
        }else if(IPP_Strcmp(pchName, "iVolVerID") == 0){/*1.2 version*/
            pParInfo->IppEncStdParam.mp4Param.mp4_verid = strtol(pchBuf, &pchEnd, 10);
            _Ipp_CheckStd(pParInfo->iFormat, IPP_VIDEO_STRM_FMT_MPG4/*std*/, iVolVerID);		
		}else{
			//er = ERES_FORMAT;
            IPP_Printf("[%s] is not useful for CodaDx8 \n", pchName);
		}
	};

	if(er!=ERES_OK) {
		IPP_Printf("error %d at some line of parameter file\n", er );
		return IPP_STATUS_INPUT_ERR;
	}

EndOfParsing:	
	
    /*dump setting.*/
    OutputPara(pParInfo);

    if(pfPara){
        IPP_Fclose(pfPara);
    }

    return er;
}

/***************************************************************************
// Name:             VpuEnc_DumpCompressedData
// Description:      Dump compressed data to file.
//
// Input Arguments:
//      pBs          - Pointer to bitstream buffer
//      outFile      - Pointer to output file.
// 
// Output Arguments:
//
// Returns:         
//        N/A
*****************************************************************************/
int VpuEnc_DumpCompressedData(IppVPUBitstream *pBs, FILE *outFile, int *pTotalBits)
{
    int ret = IPP_OK;
    int iLen = 0;
	
    if(pBs == NULL){
        return IPP_FAIL;
    }
	
    if(pBs->bsByteLen == 0){
        /*No Data.*/
        IPP_Printf("Warning: No More Data Flush Out\n");
        pBs->status &= (~VPU_BUFFER_USED);
        IPP_Printf("[SampleCode]: Unlock %d bitstream buffer \n", pBs->idx);
        return IPP_OK;
    }
    /*Dump Here.*/
	if (outFile != NULL) {
		iLen = IPP_Fwrite(pBs->pBsRead, 1, pBs->bsByteLen,outFile);
		if(iLen != pBs->bsByteLen){
			IPP_Printf("iLen = %d, pBs->bsByteLen = %d\n", iLen, pBs->bsByteLen);
			return IPP_FAIL;
		}
		IPP_Printf("[SampleCode]: Dumped %d bytes \n", pBs->bsByteLen);		
	}

	*pTotalBits += pBs->bsByteLen * 8;
    pBs->bsByteLen = 0;
    pBs->status &= (~VPU_BUFFER_USED);
    //IPP_Printf("[SampleCode]: Unlock %d bitstream buffer \n", pBs->idx);
    return ret;
}

/***************************************************************************
// Name:             VpuEnc_LoadingData
// Description:      Loading YUV data from file to buffer.
//
// Input Arguments:
//      pBs          - Pointer to bitstream buffer
//      outFile      - Pointer to output file.
// 
// Output Arguments:
//
// Returns:         
//        N/A
*****************************************************************************/
int VpuEnc_LoadingData(IppVPUPicture *pPic, FILE *infile, int inputYUVmode)
{
    Ipp8u	*tempPtr1;	
    IppPicture *pPicture = &pPic->pic;
    int     j = 0;

    if((pPicture == NULL) || (infile == NULL)){
        return IPP_FAIL;
    }
    if(pPic->pic.picStatus & VPU_BUFFER_FULLY){
        return IPP_STATUS_NOERR;
    }
    /*Loading Here.*/
	tempPtr1 = pPicture->ppPicPlane[0];
	/* Reload YUV plane */
	for(j = 0; j < pPicture->picHeight; j++) {
		if ((pPicture->picWidth) != IPP_Fread(tempPtr1,1,pPicture->picWidth,infile) ) {
			return IPP_FAIL;
		}
		tempPtr1 += pPicture->picPlaneStep[0];
	}
	if (inputYUVmode==0) {
		tempPtr1 = pPicture->ppPicPlane[1];
		for(j = 0; j < pPicture->picHeight/2; j++) {
			if ((pPicture->picWidth/2) != IPP_Fread(tempPtr1,1,pPicture->picWidth/2, infile) ) {
				return IPP_FAIL;
			}		
			tempPtr1 += pPicture->picPlaneStep[1];
		}

		tempPtr1 = pPicture->ppPicPlane[2];
		for(j = 0; j < pPicture->picHeight/2; j++) {
			if ((pPicture->picWidth/2) != IPP_Fread(tempPtr1,1,pPicture->picWidth/2, infile) ) {
				return IPP_FAIL;
			}		
			tempPtr1 += pPicture->picPlaneStep[2];
		}
	} else {
	    /*YUV420SP*/
		tempPtr1 = pPicture->ppPicPlane[1];
		for(j = 0; j < pPicture->picHeight/2; j++) {
			if ((pPicture->picWidth) != IPP_Fread(tempPtr1,1,pPicture->picWidth, infile) ) {
				return IPP_FAIL;
			}		
			tempPtr1 += pPicture->picPlaneStep[1];
		}	
	}

	return IPP_OK;
}

/***************************************************************************
// Name:             VpuEnc_AllocInputFrameBufs
// Description:      Malloc Input frame buffers.
//
// Input Arguments:
//        srcPic  -- frame buffer array , two elements
//        pPar    -- pointer to IppVPUEncParSet
// Output Arguments:
//
// Returns:         
//        N/A
*****************************************************************************/
int VpuEnc_AllocInputFrameBufs(IppVPUPicture srcPic[2], IppVPUEncParSet *pPar, PhyContMemInfo PicBufMemArr[])
{
    int     retCode = IPP_OK;
    Ipp8u   *pTmp = NULL;
    int     i = 0;
    int     iWidth , iHeight , iSizeY, iSizeUV;

    if(!pPar){
        return IPP_FAIL;
    }

    iWidth = pPar->iWidth;
    iHeight = pPar->iHeight;
    if((iWidth <16) || (iWidth> 1280)){
        return IPP_FAIL;
    }
    if((iHeight <16) || (iHeight> 720)){
        return IPP_FAIL;
    }    
    iSizeY = (iWidth * iHeight + 4095) & (~0xfff);/*I420, chroma seperate mode*/
    iSizeUV = ((iWidth * iHeight>>2) + 4095) & (~0xfff);
    
	/*allocate*/
	if (pPar->iInputYUVMode == 0) {
		for(i = 0;i <= pPar->bPingPangMode; i++){
			//cnm_os_api_dma_malloc(&pTmp, iSizeY + iSizeUV*2 , 8);
			PicBufMemArr[i].iSize = iSizeY + iSizeUV*2;
			PicBufMemArr[i].iAlign = 8;
			if(0 != cnm_os_api_pcontmem_alloc(&PicBufMemArr[i])) {
				return IPP_STATUS_NOMEM_ERR;
			}

			pTmp = PicBufMemArr[i].virAddr;

			srcPic[i].pic.ppPicPlane[0] = pTmp ;
			srcPic[i].pic.ppPicPlane[1] = (Ipp8u*)srcPic[i].pic.ppPicPlane[0] + iSizeY;
			srcPic[i].pic.ppPicPlane[2] = (Ipp8u*)srcPic[i].pic.ppPicPlane[1] + iSizeUV;
			srcPic[i].phyLuma = PicBufMemArr[i].phyAddr;//(Ipp32u)cnm_os_api_dma_get_pa(pTmp);
			srcPic[i].phyCb   = srcPic[i].phyLuma  + iSizeY;
			srcPic[i].phyCr   = srcPic[i].phyCb + iSizeUV;
			srcPic[i].pic.picWidth  = iWidth;
			srcPic[i].pic.picHeight = iHeight;
			srcPic[i].pic.picPlaneStep[0] = iWidth;
			srcPic[i].pic.picPlaneStep[1] = iWidth>>1;
			srcPic[i].pic.picPlaneStep[2] = iWidth>>1;
			srcPic[i].pic.picStatus       = 0;
		}
	} else {
	    /*YUV420SP*/
		for(i = 0;i <= pPar->bPingPangMode; i++){
			//cnm_os_api_dma_malloc(&pTmp, iSizeY + iSizeUV*2 , 8);
			PicBufMemArr[i].iSize = iSizeY + iSizeUV*2;
			PicBufMemArr[i].iAlign = 8;
			if(0 != cnm_os_api_pcontmem_alloc(&PicBufMemArr[i])) {
				return IPP_STATUS_NOMEM_ERR;
			}

			pTmp = PicBufMemArr[i].virAddr;

			srcPic[i].pic.ppPicPlane[0] = pTmp ;
			srcPic[i].pic.ppPicPlane[1] = (Ipp8u*)srcPic[i].pic.ppPicPlane[0] + iSizeY;
			srcPic[i].pic.ppPicPlane[2] = srcPic[i].pic.ppPicPlane[1];
			srcPic[i].phyLuma = PicBufMemArr[i].phyAddr;//(Ipp32u)cnm_os_api_dma_get_pa(pTmp);
			srcPic[i].phyCb   = srcPic[i].phyLuma  + iSizeY;
			srcPic[i].phyCr   = srcPic[i].phyCb;
			srcPic[i].pic.picWidth  = iWidth;
			srcPic[i].pic.picHeight = iHeight;
			srcPic[i].pic.picPlaneStep[0] = iWidth;
			srcPic[i].pic.picPlaneStep[1] = iWidth;
			srcPic[i].pic.picPlaneStep[2] = iWidth;
			srcPic[i].pic.picStatus       = 0;
		}	
	}
    return retCode;
}

/***************************************************************************
// Name:             VpuEnc_FreeInputFrameBufs
// Description:      free Input frame buffers.
//
// Input Arguments:
//        pPicArray  -- frame buffer array , two elements
// Output Arguments:
//
// Returns:         
//        N/A
*****************************************************************************/
int VpuEnc_FreeInputFrameBufs(IppVPUPicture *pPicArray, PhyContMemInfo PicBufMemArr[])
{
    int ret = IPP_OK;
    int i = 0;
    Ipp8u *pTmp = NULL;
    if(!pPicArray){
        return IPP_FAIL;
    }
    for(i = 0; i < _Max_Num_; i ++){
        pTmp = pPicArray[i].pic.ppPicPlane[0];
        if(pTmp){
			cnm_os_api_pcontmem_free(&PicBufMemArr[i]);
            //cnm_os_api_dma_free(&pTmp);
        }
    }
    return ret;
}
/***************************************************************************
// Name:             VpuEnc_AllocOutputStreamBufs
// Description:      Malloc bitstream buffers.
//
// Input Arguments:
//        bitStream  -- bitstream buffers, two elements
//        pPar       -- pointer to IppVPUEncParSet
// Output Arguments:
//
// Returns:         
//        N/A
*****************************************************************************/
int VpuEnc_AllocOutputStreamBufs(IppVPUBitstream bitStream[2], IppVPUEncParSet *pPar, PhyContMemInfo StmBufMemArr[])
{
    int     retCode = IPP_OK;
	int     iSizeEach;
	int     i;

    if(!bitStream || !pPar){
        return IPP_FAIL;
    }

	iSizeEach = ((pPar->iWidth*pPar->iHeight*3)/2 + 1023)/1024 *1024;

	/*allocate*/
	for(i = 0;i <= pPar->bPingPangMode; i++){	
		StmBufMemArr[i].iSize = iSizeEach;
		StmBufMemArr[i].iAlign = 8;
		//cnm_os_api_dma_malloc((void**)&(bitStream[i].pBsBuffer),iSizeEach, 8);

		if(0 != cnm_os_api_pcontmem_alloc(&StmBufMemArr[i])){
			return IPP_STATUS_NOMEM_ERR;
		}

		bitStream[i].pBsBuffer = StmBufMemArr[i].virAddr;

		/*bitstream buffer*/
		bitStream[i].pBsRead = bitStream[i].pBsWrite = bitStream[i].pBsBuffer ;
		bitStream[i].bsByteLen = 0;
		bitStream[i].bsBufSize = iSizeEach;
		bitStream[i].phyAddr = StmBufMemArr[i].phyAddr;//(Ipp32u)cnm_os_api_dma_get_pa(bitStream[i].pBsBuffer);
		bitStream[i].idx     = i;
	}

    return retCode;
}
/***************************************************************************
// Name:             VpuEnc_FreeOutputStreamBufs
// Description:      Free output bitstream buffer.
//
// Input Arguments:
//        pBitStream  -- pointer to IppVPUBitstream
// Output Arguments:
//
// Returns:         
//        N/A
*****************************************************************************/
int VpuEnc_FreeOutputStreamBufs(IppVPUBitstream *pBSArray, PhyContMemInfo StmBufMemArr[])
{
    int   retCode = IPP_OK;
    int   i = 0;
    Ipp8u *pTmp = NULL;

    if(!pBSArray){
        return IPP_FAIL;
    }

    for(i = 0; i < _Max_Num_; i ++){
        pTmp = pBSArray[i].pBsBuffer;
        if(pTmp){
            //cnm_os_api_dma_free(&pTmp);
			cnm_os_api_pcontmem_free(&(StmBufMemArr[i]));
        }
    }

    return retCode;
}
/***************************************************************************
// Name:             VpuEnc_GetInputFrameBuf
// Description:      Find a new frame buffer for encoding.
//
// Input Arguments:
//      ppPic        - Pointer to pointer of frame buffer.
// 
// Output Arguments:
//      ppPic        - return out a unused frame buffer
// Returns:         
//        N/A
*****************************************************************************/
//int VpuEnc_GetInputFrameBuf(IppVPUPicture **ppPic, IppVPUPicture *pPicArray)
//{
//    int retCode = IPP_OK;
//    int i = 0;
//    for(i = 0; i < _Max_Num_; i++){
//        if(pPicArray[i].pic.picStatus & VPU_BUFFER_USED){
//            continue;
//        }
//        *ppPic = &pPicArray[i];
//        //IPP_Printf("[SampeCode]: Get [0x%x] Free Frame Buffer \n", pPicArray[i].pic.ppPicPlane[0]);
//        break;
//    }
//    if(*ppPic == NULL){
//        return IPP_FAIL;
//    }
//    return retCode;
//}

/***************************************************************************
// Name:             CodaDx8encoder
// Description:      Encoding entry for CodaDx8 decoder, support: H264/BP, 
//                   MPEG4/SP, VC1/MP, H263/P3, MPEG2/MP.
//
// Input Arguments:
//      pParInfo     - Pointer to para setting for encoder.
//      inputFile    - Pointer to input file.
//      outFile      - Pointer to output file.
// 
// Output Arguments:
//        pFrameNumber  Overall decoded frame numbers
//
// Returns:         
//        IPP Codec Status Code
*****************************************************************************/
int CodaDx8Enc(FILE *inputFile, FILE * outFile, unsigned long *pFrameNumber, 
               char *logfile, IppVPUEncParSet* pParInfo)
{
    int       retCode           = IPP_OK;
    char      *pLog             = logfile;
    void      *pEncoderState    = NULL;
    int       iTotal            = 0;

    IppVPUPicture    srcPicture[2]   = {0};//for pingpang mode
	PhyContMemInfo	 srcPictureMem[2] = {0};
	IppVPUBitstream  dstBitStream[2] = {0};
	PhyContMemInfo	 dstBitStreamMem[2] = {0};
	IppVPUPicture    *pPic = &srcPicture[0];//default point to 1st buffer
	IppVPUBitstream  tmpBitStream, *pBs;
	int idx = 0;

    MiscGeneralCallbackTable *pSrcCBTable = NULL;
	int                 encoding_perf_index;
	int                 yuvloading_perf_index;
    int                 nTotalTime = 0;
	int                 nTotalBits = 0;
	int                 nTotalTime1 = 0;

    IPP_GetPerfCounter(&encoding_perf_index, DEFAULT_TIMINGFUNC_START, DEFAULT_TIMINGFUNC_STOP);
    IPP_ResetPerfCounter(encoding_perf_index);
    IPP_GetPerfCounter(&yuvloading_perf_index, DEFAULT_TIMINGFUNC_START, DEFAULT_TIMINGFUNC_STOP);
    IPP_ResetPerfCounter(yuvloading_perf_index);
    /*Setting parameters*/
    retCode = miscInitGeneralCallbackTable(&pSrcCBTable);
    if(retCode != IPP_OK){
        IPP_Log(pLog,"a", "Error: miscInitGeneralCallbackTable \n");
        return retCode;
    }
    /*Malloc Input YUV buffer*/
    retCode = VpuEnc_AllocInputFrameBufs(srcPicture, pParInfo, srcPictureMem);
    if(retCode != IPP_OK){
        IPP_Log(pLog,"a", "Error: Input YUV buffer \n");
        return retCode;
    }
    /*Malloc Output Bitstream buffer*/
	retCode = VpuEnc_AllocOutputStreamBufs(dstBitStream, pParInfo, dstBitStreamMem);
    if(retCode != IPP_OK){
        IPP_Log(pLog,"a", "Error: Output Bitstream buffer \n");
        return retCode;
    }
	idx = 0;
	tmpBitStream = dstBitStream[idx];
	pBs = &tmpBitStream;
	IPP_Printf("Ping Pang Buffer Enable: %d\n", pParInfo->bPingPangMode);

    /*Init Encoder, and Get bitstream buffer*/
    retCode = EncoderInitAlloc_VPU(pParInfo, pSrcCBTable, pBs, NULL/*external allocate*/, &pEncoderState);
    if(IPP_STATUS_NOERR != retCode){
        IPP_Log(pLog,"a", "Error: EncoderInitAlloc_VPU %d \n", retCode);
        IPP_Printf("Error: EncoderInitAlloc_VPU %d \n", retCode);
        goto _EndOfVPUEnc_;
    }

    /*dumping compressed data out: such as SPS, and PPS*/
    retCode = VpuEnc_DumpCompressedData(pBs, outFile, &nTotalBits);
    if(IPP_STATUS_NOERR != retCode){
        IPP_Log(pLog,"a", "Error: VpuEnc_DumpCompressedData \n");
        goto _EndOfVPUEnc_;
    }

    do {
        //IPP_Printf("\nStart Encoding Frame ---------------\n");
        //retCode = VpuEnc_GetInputFrameBuf(&pPic, srcPicture);
        //if(IPP_STATUS_NOERR != retCode){
        //    IPP_Log(pLog,"a", "Error: VpuEnc_GetInputFrameBuf \n");
        //    goto _EndOfVPUEnc_;
        //}
        /*Loading YUV data to frame buffer*/
        IPP_StartPerfCounter(yuvloading_perf_index);
        retCode = VpuEnc_LoadingData(pPic, inputFile, pParInfo->iInputYUVMode);
		IPP_StopPerfCounter(yuvloading_perf_index);
        if(IPP_STATUS_NOERR != retCode){
			IPP_Printf("end of file!\n");
			/*reset retCode, since end of file is normal*/
			retCode = IPP_OK;
			if (pParInfo->bPingPangMode) {
				pPic->pic.picStatus = VPU_LAST_FRAME;			
			} else {
				break;
			}
        }
        //IPP_Printf("\nLoad YUV Frame ---------------\n");
        /*kick off one frame encoding*/
		nTotalTime1 = (int)IPP_GetPerfData(encoding_perf_index);
        IPP_StartPerfCounter(encoding_perf_index);
        retCode = EncodeFrame_VPU(pBs, pPic, pEncoderState);
		IPP_StopPerfCounter(encoding_perf_index);
		nTotalTime = (int)IPP_GetPerfData(encoding_perf_index);
        if(IPP_STATUS_NOERR != retCode){
            IPP_Log(pLog,"a", "Error: EncodeFrame_VPU \n");
			IPP_Printf("Error: EncodeFrame_VPU %d\n", retCode);
            goto _EndOfVPUEnc_;
        }
		
		/* performance check for real time encoding*/
		IPP_Printf("Encoding time is %dms\n", (nTotalTime-nTotalTime1)/1000);
		if (((nTotalTime-nTotalTime1)/1000) > (1000/pParInfo->iFrameRate)) {
			IPP_Printf("Warning: insufficient performance!!!!!!!!\n");
		}

        /*dumping compressed data out.*/
        retCode = VpuEnc_DumpCompressedData(pBs, outFile, &nTotalBits);
        if(IPP_STATUS_NOERR != retCode){
            IPP_Log(pLog,"a", "Error: VpuEnc_DumpCompressedData \n");
            goto _EndOfVPUEnc_;
        }

		if (pParInfo->bPingPangMode && pPic->pic.picStatus == VPU_LAST_FRAME) {
			break;
		}

		IPP_Printf("Frame %d is encoded!\n", iTotal++);
	
		if (pParInfo->bPingPangMode) {
			idx  = (idx + 1)%2;
			pPic = &srcPicture[idx];
			tmpBitStream = dstBitStream[idx];
			pBs = &tmpBitStream;
			IPP_Printf("Buffer index %d\n", idx);
		}
    } while (!IPP_Feof(inputFile));

    /*dumping compressed data out.*/
	nTotalTime = (int)IPP_GetPerfData(encoding_perf_index);
    g_Tot_Time[IPP_VIDEO_INDEX]     = nTotalTime;
    g_Frame_Num[IPP_VIDEO_INDEX]    = iTotal;

	IPP_Printf("\nPERFORMANCE: Total Frame: %d, Total Time: %d(ms), Source Frame Rate: %d, Encodin FPS: %f\n", iTotal, nTotalTime/1000, 	pParInfo->iFrameRate, (float)(1000.0 * 1000.0 * iTotal / nTotalTime));
	IPP_Printf("Total Bits: %d (bits), Average Rate: %.2f kb/s\n", nTotalBits, (float)nTotalBits * pParInfo->iFrameRate / (float)(iTotal) / 1000.0);
	
	IPP_Printf("Total YUV Loading Time is %d(ms)\n", (int)IPP_GetPerfData(yuvloading_perf_index)/1000);	
	IPP_Printf("YUV Loading Time/Frame Encoding Time ratio is %.2f\n", (int)IPP_GetPerfData(yuvloading_perf_index)/(float)nTotalTime);

_EndOfVPUEnc_:
    /*Free Encoder*/
    EncoderFree_VPU(&pEncoderState);

    /*free callback*/
    miscFreeGeneralCallbackTable(&pSrcCBTable);

    /*Free Input YUV buffer*/
    VpuEnc_FreeInputFrameBufs(srcPicture, srcPictureMem);

	/*Free Output Stream buffer*/
	VpuEnc_FreeOutputStreamBufs(dstBitStream, dstBitStreamMem);

    IPP_FreePerfCounter(encoding_perf_index);
    IPP_FreePerfCounter(yuvloading_perf_index);
	//add for physical continuous memory leak.
	IPP_PysicalMemTest();

    return retCode;
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
int ParseCmd_CodaDx8Enc(char *pCmdLine,   char *pSrcFileName, 
                        char *pDstFileName, char *pLogFileName, 
                        IppVPUEncParSet *pPar)
{
    char *pCur, *pEnd;
    char par_name[MAX_PAR_NAME_LEN] = {0};
    char par_value[MAX_PAR_VALUE_LEN] = {0};
    char *p1 = NULL, *p2 = NULL, *p3 = NULL;

    int  par_name_len         = 0;
    int  par_value_len        = 0;
    int  v3                   = 0;/*std of compressed*/
  
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
        } else if (0 == IPP_Strcmp(par_name, "fmt")) {
            STRNCPY(par_value, p2 + 1, par_value_len);
			pPar->iFormat = IPP_Atoi(par_value);
		} else if (0 == IPP_Strcmp(par_name, "pollingmode")){
            STRNCPY(par_value, p2 + 1, par_value_len);
			pPar->bPollingMode = IPP_Atoi(par_value);	
			if ((pPar->bPollingMode != 0) && (pPar->bPollingMode != 1)) {
				IPP_Printf("cmd input error, should be: -pollingmode:0|1 !\n");
				return IPP_FAIL;	
			}
		} else if (0 == IPP_Strcmp(par_name, "dynaclk")){
            STRNCPY(par_value, p2 + 1, par_value_len);
			pPar->bDynaClkEnable = IPP_Atoi(par_value);	
			if ((pPar->bDynaClkEnable != 0) && (pPar->bDynaClkEnable != 1)) {
				IPP_Printf("cmd input error, should be: -pollingmode:0|1 !\n");
				return IPP_FAIL;	
			}
		} else if (0 == IPP_Strcmp(par_name, "pingpangmode")) {
            STRNCPY(par_value, p2 + 1, par_value_len);
			pPar->bPingPangMode = IPP_Atoi(par_value);	
			if ((pPar->bPingPangMode != 0) && (pPar->bPingPangMode != 1)) {
				IPP_Printf("cmd input error, should be: -pingpangmode:0|1 !\n");
				return IPP_FAIL;	
			}		
		} else if (0 == IPP_Strcmp(par_name, "multiinstancesync")) {
            STRNCPY(par_value, p2 + 1, par_value_len);
			pPar->iMultiInstanceSync = IPP_Atoi(par_value);	
			if ((pPar->iMultiInstanceSync != 0) && (pPar->iMultiInstanceSync != 1)) {
				IPP_Printf("cmd input error, should be: -multiinstancesync:0|1 !\n");
				return IPP_FAIL;	
			}		
		} else if (0 == IPP_Strcmp(par_name, "inputyuvmode")) {
            STRNCPY(par_value, p2 + 1, par_value_len);
			pPar->iInputYUVMode = IPP_Atoi(par_value);	
			if ((pPar->iInputYUVMode != 0) && (pPar->iInputYUVMode != 1)) {
				IPP_Printf("cmd input error, should be: -inputyuvmode:0|1 !\n");
				return IPP_FAIL;	
			}		
		} else if ((0 == IPP_Strcmp(par_name, "p")) || (0 == IPP_Strcmp(par_name, "P"))) {
			/*par file*/
			STRNCPY(par_name, p2 + 1, par_value_len);
            ret = GetPar_VPUEnc(pPar, par_name);
            if(0 != ret){
                IPP_Printf("    Error: GetPar_VPUEnc \n");
                return IPP_FAIL;
            }

        } else {
			IPP_Printf("cmd input error: undefined par: -%s\n", par_name);
			return IPP_FAIL;
        }
        
        pCur = p3;
    }

    return ret;
}

/***************************************************************************
// Name:             main
// Description:      Entry function for CodaDx8 decoder
//
// Returns:          0:       decoding okay
//                   others:  decoding fail
*****************************************************************************/
int CodecTest(int argc, char *argv[])
{
	int   ret = IPP_OK;
	unsigned long ulFrameNumber;

	char    input_file_name[512]     = {'\0'}; 
	char    output_file_name[512]    = {'\0'};
	char    log_file_name[512]       = {'\0'};

	FILE    *fp_cmp        = NULL;
	FILE    *fp_yuv        = NULL;

	IppVPUEncParSet         parInfo;

    IPP_Memset(&parInfo, 0 , sizeof(IppVPUEncParSet));
	UpdateStandardPar_VPUEnc(&parInfo);

	if (2 > argc) {
        IPP_Printf("%s\n", &scHelpCodaDx8Enc);
        return IPP_FAIL;
    } else if (2 == argc){
        /*for validation*/
        IPP_Printf("Input command line: %s\n", argv[1]);
        if (IPP_OK != ParseCmd_CodaDx8Enc(argv[1], input_file_name, output_file_name ,log_file_name, &parInfo)){
			IPP_Printf("command line is wrong! %s\n\n", argv[1]);
            IPP_Log(log_file_name, "a", "command line is wrong! %s\n\n", argv[1]);
            return IPP_FAIL;
        }
    } else {
		int i;
		for(i=1;i<argc;i++) {
			if(IPP_OK != ParseCmd_CodaDx8Enc(argv[i], input_file_name, output_file_name ,log_file_name, &parInfo)) {
				IPP_Printf("some command line is mistake! %s\n\n", argv[i]);
				IPP_Log(log_file_name, "a", "some command line is mistake! %s\n\n", argv[i]);
				return IPP_FAIL;
			}
			
		}
    }

    if (0 == IPP_Strcmp(input_file_name, "\0")) {
        IPP_Log(log_file_name, "a", "input file name is null!\n");
        return IPP_FAIL;
    } else {
        fp_yuv = IPP_Fopen(input_file_name, "rb");
        if (!fp_yuv) {
            IPP_Log(log_file_name, "a", "Fails to open file %s!\n", input_file_name);
            return IPP_FAIL;
        }
    }
    
    if (0 == IPP_Strcmp(output_file_name, "\0")) {
		IPP_Log(log_file_name, "a", "output file name is null!\n");
		//sometimes, we don't want to dump decoded data, such as performance test. therefore, not return IPP_FAIL here.
    } else {
        fp_cmp = IPP_Fopen(output_file_name, "wb");
        if (!fp_cmp) {
            IPP_Log(log_file_name, "a", "Fails to open file %s\n", output_file_name);
            if (fp_yuv) {
                IPP_Fclose(fp_yuv);
            }
            return IPP_FAIL;
        }
    }

	switch(parInfo.iFormat) {
	case 2:
	case 4:
	case 5:
		IPP_Printf("Input video format is supported: [%s]\n", scformat[parInfo.iFormat]);
		break;
	default:
		IPP_Printf("Input video format(%d) is un-supported.\n", parInfo.iFormat);
		IPP_Log(log_file_name, "a", "Input video format(%d) is un-supported.\n", parInfo.iFormat);
                if (fp_cmp) {
                    IPP_Fclose(fp_cmp);
                }
                if (fp_yuv) {
                    IPP_Fclose(fp_yuv);
                }
		return IPP_FAIL;
	}

	if ((parInfo.bPollingMode != 0) && (parInfo.bPollingMode != 1)) {
		IPP_Printf("cmd input error, should be: -reorderchoice:0|1|2 !\n");
		IPP_Log(log_file_name, "a", "cmd input error, should be: -reorderchoice:0|1|2 !\n");
                if (fp_cmp) {
                    IPP_Fclose(fp_cmp);
                }
                if (fp_yuv) {
                    IPP_Fclose(fp_yuv);
                }
		return IPP_FAIL;	
	}

    IPP_Log(log_file_name, "a", "input file  : %s\n", input_file_name);
    IPP_Log(log_file_name, "a", "output file : %s\n", output_file_name);
    IPP_Log(log_file_name, "a", "begin to decode\n");

    DisplayLibVersion();
    ret = CodaDx8Enc(fp_yuv, fp_cmp, &ulFrameNumber, log_file_name, &parInfo);

	switch (ret)
	{
	case IPP_OK:
		IPP_Log(log_file_name, "a", "Everything is Okay.\n");
		break;
	default:
		IPP_Log(log_file_name, "a", "Something wrong, CodaDx8Enc() ret %d\n", ret);
		break;
	}

	if ( fp_cmp ){
		IPP_Fclose (fp_cmp);
	}

	if ( fp_yuv ){
		IPP_Fclose (fp_yuv);
	}

    return ret;

}