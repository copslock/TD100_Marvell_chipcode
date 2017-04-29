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

#ifndef __CODECCNM__H__
#define __CODECCNM__H__

#include "codecVC.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _IppCMVpuDecFrameAddrInfo{
	Ipp32u	phyY;	//must 8 byte align, physical address of physical continuous memory
	Ipp32u	phyU;	//must 8 byte align, physical address of physical continuous memory
	Ipp32u	phyV;	//must 8 byte align, physical address of physical continuous memory

	//reserved for application use, CMVpu APIs won't access this member
	void*	pUserData0;
	void*	pUserData1;
	void*	pUserData2;
	void*	pUserData3;
}IppCMVpuDecFrameAddrInfo;

typedef struct _IppCMDecFilePlayStmBufInfo{
	Ipp32u	phyAddrBitstreamBuf;	//must 4 align, must be physical address of physical continuous memory
	int		DataOffset;				//0~3
	int		DataLen;
}IppCMDecFilePlayStmBufInfo;

typedef struct _IppCMVpuDecParsedSeqInfo{
	int		FrameAlignedWidth;	//unit in pixel, always 16 align, it called CodedFrameWidth in previous version
	int		FrameAlignedHeight;	//always 16 align, for mpeg2, probably 32 align
	IppiRect	FrameROI;	//display area ROI
	int		FrameRateNumerator;
	int		FrameRateDenominator;	//if FrameRateNumerator:FrameRateDenominator == 0:1, it means CM couldn't get frame rate information from sequence header data

	int		MinFrameBufferCount;	//min count of buffer needed to be registered to decoder
	int		MaxReorderDelay;
	int		MaxNumOfFrameBufUsedForNextDecoded;

	int		Mp4_dataPartitionEnableReport;
	int		Mp4_reversibleVlcEnableReport;
	int		Mp4_shortVideoHeaderReport;
	int		H263_annexJEnableReport;
	int		Mp2_progressive_sequence_Report;

	//reserved for future use
	void*	Reserved[4];
}IppCMVpuDecParsedSeqInfo;

typedef struct _IppCMVpuDecOutputInfo{
	int		idxFrameOutput;			//index of frame buffer that contained outputted frame data, this frame should be display. If some error happen, this variable maybe < 0.
	int		idxFrameDecoded;		//index of frame buffer that contained decoded frame data. Under re-order case, this frame shouldn't be display. Application should use the frame indicated by idxFrameOutput to display.
	int		FrameDecodedType;		//0: I frame, 1: P frame, 2: B frame, 4: VC1 skip frame
	int		OneFrameUnbrokenDecoded;//indicate whether the frame is decoded normally

	int		bVPUonDecodingStage;	//indicate the decoder state

	int		NumOfErrMbs;			//the error MB number for the frame--idxFrameDecoded
	int		idxFrameBufUsedForNextDecoded[3];	//indicates the frame buffer will be used for next frame decoding, only valid for VC1

	int		VC1MPDirectModeMVUsed;	//for VC1 MP, if the current decoded B frame is using direct mode MV, this var is set to 1. When direct mode is using, the decoded B frame content is probably corrupted, it's C&M known issue.

	//reserved for future use
	void*	Reserved[3];
}IppCMVpuDecOutputInfo;

#define IPPCMDEC_STATICSTMBUF_PROTECTSPACE	1	//when static stream buffer works as ring buffer, even it's totally empty, its usable spare space is less than its buffer size because of protect space existing

#define IPPCMDEC_DECFRAMEBUF_MAX_H264				18	//for H264, at most, application could register 18 frame buffers to decoder by calling DecoderRegisterFrameBufs_CMVpu()
#define IPPCMDEC_DECFRAMEBUF_MAX_VC1				32	//for VC1, at most, application could register 32 frame buffers to decoder by calling DecoderRegisterFrameBufs_CMVpu()
#define IPPCMDEC_DECFRAMEBUF_MAX_H263MPEG2MPEG4		8	//for h263, mpeg2, mpeg4, at most, application could register 8 frame buffers to decoder by calling DecoderRegisterFrameBufs_CMVpu()

typedef struct _IppCMVpuDecInitPar{
	int						ReorderEnable;
	int						FilePlayEnable;

	Ipp32u					phyAddrStaticBitstreamBuf;	//must 4 byte align, must be physical address of physical continuous memory
	Ipp32u					StaticBitstreamBufSize;		//must 1024 align, and should >=2*1024 && < 16383*1024
	IppVideoStreamFormat	VideoStreamFormat;		/*stream format, h264, mpeg2, etc*/
	
	int						PollingMode;			//0 interrupt mode, 1 build-in polling, 2 custom polling.
	void*					PollingModeUserData;	//valid only when PollingMode = 2
	int (*PollingModeWaitFun)(void* pUserData, int bNoMoreData, int reserved);	//valid only when PollingMode = 2, parameter pUserData will equal to PollingModeUserData.

	int						DynamicClkEnable;		//0: VPU clock always on during decoding. 1: VPU clock is closed after decoding one frame, and will be opened before decoding next frame.

	int						MultiInstanceSync;		//0: Won't enable synchronous mechanism between multiple instance, for single instance usage case, let it be 0.  1: Enable synchronous mechanism, synchronous mechanism ensures that only one instance could use VPU at any time. For multiple instance usage case, let it be 1.

	int						PostDBKEnable;			//0: not-enable h263/mpeg4 post-processing de-blocking. 1: enable h263/mpeg4 post-processing de-blocking. This flag could be set to 1 only for h263 and mpeg4.

	int						YUVFormat;				//0: YUV 420 plannar, 1: YUV 420 SP(For YUV 420SP, Y is plannar, UV is interleave)

	//reserved, let it be 0
	void*					Reserved[2];

}IppCMVpuDecInitPar;


IPPCODECAPI( IppCodecStatus, DecoderInitAlloc_CMVpu, (
	IppCMVpuDecInitPar *pDecInitParSet,
	MiscMallocCallback pfunMalloc,
	MiscFreeCallback pfunFree,
	void **ppDecoderObj
))

IPPCODECAPI( IppCodecStatus, DecodeFrame_CMVpu, (
	void *pDecoderObj,
	IppCMDecFilePlayStmBufInfo *pStmBuf,
	IppCMVpuDecOutputInfo *pOutputInfo,
	int bNoMoreData
))

IPPCODECAPI( IppCodecStatus, DecoderFree_CMVpu, (
	void **ppDecoderObj
))

IPPCODECAPI( IppCodecStatus, DecoderParseSeqHeader_CMVpu, (
	void *pDecoderObj,
	IppCMVpuDecParsedSeqInfo* pSeqInfo
))

IPPCODECAPI( IppCodecStatus, DecoderRegisterFrameBufs_CMVpu, (
	void *pDecoderObj,
	IppCMVpuDecFrameAddrInfo FrameInfoArr[],
	int iArrSz,
	int iYStride	//must be 8 align
))

IPPCODECAPI( IppCodecStatus, DecoderFrameDisplayed_CMVpu, (
	void *pDecoderObj,
	int frameidx
))


//NOTE: Because there is some pretect space left, even application hasn't fill any data into the static stream buffer, the size of waitforfillspace is still < buffer size of static stream buffer
//NOTE: when eos or no more data, application shouldn't call DecoderGetStaticStmBufInfo_CMVpu(), it returned information probably is mistake.
//NOTE: when no more data, once VPU decoded the last frame in static stream buffer, the *pPhyAddrVPUWillReadSpace will stay at the next 512 aligned address. Therefore, the *pPhyAddrWaitForFillSpace probably exceeds the *pPhyAddrWaitForFillSpace and DecoderGetStaticStmBufInfo_CMVpu() returned *pWaitForFillSpaceSz are mistake.
IPPCODECAPI( IppCodecStatus, DecoderGetStaticStmBufInfo_CMVpu, (
	void *pDecoderObj,
	Ipp32u* pPhyAddrWaitForFillSpace,
	int* pWaitForFillSpaceSz,
	Ipp32u* pPhyAddrVPUWillReadSpace
))

IPPCODECAPI( IppCodecStatus, DecoderStaticStmBufDataFilled_CMVpu, (
	void *pDecoderObj,
	int iDataFilledLen
))

IPPCODECAPI( IppCodecStatus, DecoderStopAndClearStaticStmBuf_CMVpu, (
	void *pDecoderObj
))

IPPCODECAPI( IppCodecStatus, DecoderSetRotMir_CMVpu, (
	void *pDecoderObj,
	int bRotationEnable,
	int bMirrorEnable,
	int iRotationDir,	//only could be 0,1,2,3, correspond to 0, 90, 180, 270 degree, anti-clockwise direction
	int iMirrorDir,		//only could be 0,1,2,3, correspond to no mirror, vertical mirror(top to down), horizontal mirror, horizontal-vertical mirror
	IppCMVpuDecFrameAddrInfo*	pFrameAddrInfo,
	int	iYStride		//must be 8 align
))

IPPCODECAPI( IppCodecStatus, DecoderChangeRotMirBuf_CMVpu, (
	void *pDecoderObj,
	IppCMVpuDecFrameAddrInfo*	pFrameAddrInfo
))

//for post-processing de-blocking buffer, its layout should be same as the buffer which are registered to decoder through DecoderRegisterFrameBufs_CMVpu()
IPPCODECAPI( IppCodecStatus, DecoderSetPostDBKBuf_CMVpu, (
	void *pDecoderObj,
	IppCMVpuDecFrameAddrInfo*	pFrameAddrInfo
))

IPPCODECAPI( IppCodecStatus, DecoderHibernate_CMVpu, (
	void *pDecoderObj,
	int	iHibernateLevel	//1: only let VPU sleep; 2: let VPU sleep and close clock; 3: let VPU sleep, close clock and power off
))

IPPCODECAPI( IppCodecStatus, DecoderWakeup_CMVpu, (
	void *pDecoderObj
))


//C&M couldn't decode some(not all) VC1 MP frame which adopts DirectModeMV. Therefore, application could use DecoderReformVC1MPDirectMVFrame_CMVpu() to workaround.
//DecoderReformVC1MPDirectMVFrame_CMVpu() only could be called one time before any calling of DecodeFrame_CMVpu().
//If DecoderReformVC1MPDirectMVFrame_CMVpu() is called successfully, the decoder won't ouput any VC1 MP frame which adopts DirectModeMV, but output the previous frame. In other words, decoder output a repeated frame, just like a skip frame, to replace the DirectModeMV frame.
//If application doesn't call DecoderReformVC1MPDirectMVFrame_CMVpu(), DirectModeMV frame is outputted just like normal frame, but the content of the frame probably is corrupted.
//Whether DecoderReformVC1MPDirectMVFrame_CMVpu is called or not, IppCMVpuDecOutputInfo.VC1MPDirectModeMVUsed indicates whether current decoded frame is DirectModeMV frame.
IPPCODECAPI( IppCodecStatus, DecoderReformVC1MPDirectMVFrame_CMVpu, (
	void *pDecoderObj
))



/********************** below is for C&M encoder *********************/

typedef struct _IppVPUBitstream{
	Ipp8u             *pBsBuffer;            /*start pointer of buffer */
	Ipp8u             *pBsRead;              /*read pointer for wrapping-around*/
	Ipp8u             *pBsWrite;             /*write pointer for wrapping-around*/
	Ipp32u             bsBufSize;            /*bitstream buffer size*/
	Ipp32u             bsByteLen;            /*bitstream buffer size, zero value for flush reorder buffer*/
    Ipp32u             phyAddr;              /*phyAddr*/
    Ipp32u             bsByteAvail;          /*available space in bitstream buffer*/
    Ipp32u             status;               /*buf status*/
    Ipp32u             idx;                  /*buf idx*/
}IppVPUBitstream;

/* physical address of Y.U.V.*/
typedef struct _IppVPUPicture{
    Ipp32u     phyLuma;
    Ipp32u     phyCb;
    Ipp32u     phyCr;
    IppPicture pic;
}IppVPUPicture;

typedef struct {
    int     mp4_dataPartitionEnable;
    int     mp4_reversibleVlcEnable;
    int     mp4_intraDcVlcThr;
    int	    mp4_hecEnable;
    int     mp4_verid;
} IppEncMp4Param;

typedef struct {
	int     h263_annexJEnable;  /* Deblock Filter Mode*/
	int     h263_annexKEnable;  /* Slice Structured Mode*/
	int     h263_annexTEnable;  /* Modified Quantization Mode*/
} IppEncH263Param;

typedef struct {
	int     avc_bDeblockDisable;
} IppEncAvcParam;

typedef struct _IppVPUEncParSet{

	int     iFormat;	
	int     iWidth;
	int     iHeight;
	int     iFrameRate;
	int     iGopSize;                   /*0 每 Only first picture is I, 1每all I pictures, 2每IPIP＃, 3每IPPIPP＃ */
    int     iInitQp;                    /*when bRCEnable=0 it is valid for all frame, otherwise when bRCEnable=1, it is ignored, first QP is selected internally*/
    int     bRCEnable;                  /*1: enable, 0: disable*/
	int     iRCType;                    /*rate control type for different usage scenarios */
	int     iRCBitRate;                 /*(bits/second), valid when bRCEnable=1 */
	int     iRCMaxBitRate;              /*(bits/second), valid when bRCEnable=1, >= 1.5*iRCBitRate*/
	int     iDelayLimit;                /*Time delay (in mili-seconds) to reach initial occupancy*/

    union {
        IppEncMp4Param  mp4Param;
        IppEncH263Param h263Param;
        IppEncAvcParam  avcParam;			
    } IppEncStdParam;

	/*C&M working mode*/
	int     bPollingMode;				/*0: interrupt mode; 1: polling mode*/
	int     bDynaClkEnable;
	int     bPingPangMode;
	int     iMultiInstanceSync;          /*0: diasable; 1: enable; >1 reserved */
	int     iInputYUVMode;               /*0: yuv420planar; 1: yuv420semiplanar; >1 reserved */

}IppVPUEncParSet;

/*****************************************************************************
//     Name:                    EncoderInitAlloc_VPU
//     Description:   handle initialize process of VPU Encoder, including:  
//                    Search memory setting, frame buffer allocation,
//                    and basic setting of VPU encoder, including:
//                    resolution input, rate control, basic parameters 
//                    bitstream format,dynamicalloation, reorder,etc.
//                    
//     Output:        PPS/SPS, VO/VOL. etc.
//     NOTE:          
******************************************************************************/
IPPCODECAPI( IppCodecStatus, EncoderInitAlloc_VPU, (
	    IppVPUEncParSet          *pVPUEncParSet,
        MiscGeneralCallbackTable *pCallbackTable,
        IppVPUBitstream          *pBitstream,       /*Output*/
	    IppVPUPicture            *pSrcPicture,     /*NULL, allocated outside*/
	    void                     ** ppEncoderState /*Output*/
))

/*****************************************************************************
//     Name:                    EncoderFree_VPU
//     Description:   close the VPU and free momery 
//
//     NOTE:          
******************************************************************************/
IPPCODECAPI( IppCodecStatus, EncoderFree_VPU, (
	    void **ppEncoderState
))

/*****************************************************************************
//     Name:                    EncodeFrame_VPU
//     Description:   load YUV data, kickoff one frame encoding, and output info.
//
//     NOTE:          loading bitstream and frame encoding can be parallelly organized.    
******************************************************************************/
IPPCODECAPI( IppCodecStatus, EncodeFrame_VPU, (
        IppVPUBitstream          *pBitstream,     /*Output*/
	    IppVPUPicture            *pSrcPicture,     /*Input*/ 
	    void                     *pEncoderState
))

/****************************************************************************
//     Name:                    EncodeSendCmd_VPU
//     Description:   reserve this APIs for VPU controls , such as PP, etc.
//
//
//     NOTE:          
******************************************************************************/
IPPCODECAPI (IppCodecStatus, EncodeSendCmd_VPU, (
    	int   cmd,
	    void *pInParam,
    	void *pOutParam,
    	void *pEncoderState
))

#ifdef __cplusplus
}
#endif

#endif
