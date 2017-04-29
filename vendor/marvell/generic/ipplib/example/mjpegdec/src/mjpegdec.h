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


#ifndef _MJPEGFMAVI_H_
#define _MJPEGFMAVI_H_

#include "codecDef.h"
#include "codecJP.h"
#include "misc.h"

#define IPP_MJPEGFMAVI_INP_STREAM_BUF_LEN  20480 
#define IPP_MAX_STREAM_NUM 2

#define GETFOURCC(pCurByte, iFourCC)   \
    cByte0 = *((pCurByte)++);          \
    cByte1 = *((pCurByte)++);          \
    cByte2 = *((pCurByte)++);          \
    cByte3 = *((pCurByte)++);          \
    (iFourCC) =                        \
        ( (cByte0<<24)+(cByte1<<16)+(cByte2<<8)+cByte3 ); 

#define GETTWOCC(pCurByte, sTWOCC)     \
    cByte0 = *((pCurByte)++);          \
    cByte1 = *((pCurByte)++);          \
    (sTWOCC) =                         \
        ( (cByte0<<8)+cByte1 ); 

#define TOFOURCC(cA, cB, cC, cD)       \
    ( ((cA)<<24) | ((cB)<<16) | ((cC)<<8) | (cD) )

#define TOTWOCC(cA, cB)                \
    ( ((cA)<<8) | (cB) )

#define GET4BYTES(pCurByte, iValue )   \
    cByte0 = *((pCurByte)++);          \
    cByte1 = *((pCurByte)++);          \
    cByte2 = *((pCurByte)++);          \
    cByte3 = *((pCurByte)++);          \
    (iValue) =                         \
        ( (cByte3<<24)+(cByte2<<16)+(cByte1<<8)+cByte0 ); 

#define GET2BYTES( pCurByte, iValue )  \
    cByte0 = *((pCurByte)++);          \
    cByte1 = *((pCurByte)++);          \
    (iValue) =                         \
        ( (cByte1<<8)+cByte0 ); 

#define SKIPBYTES( pCurByte, iNum )    \
    (pCurByte) += (iNum);

#define AVI_RIFF   TOFOURCC('R', 'I', 'F', 'F')
#define AVI_AVI    TOFOURCC('A', 'V', 'I', ' ')
#define AVI_LIST   TOFOURCC('L', 'I', 'S', 'T')
#define AVI_INFO   TOFOURCC('I', 'N', 'F', 'O')
#define AVI_MOVI   TOFOURCC('m', 'o', 'v', 'i')
#define AVI_IDX1   TOFOURCC('i', 'd', 'x', '1')
#define AVI_HDRL   TOFOURCC('h', 'd', 'r', 'l')
#define AVI_AVIH   TOFOURCC('a', 'v', 'i', 'h')
#define AVI_STRL   TOFOURCC('s', 't', 'r', 'l')
#define AVI_STRH   TOFOURCC('s', 't', 'r', 'h')
#define AVI_STRF   TOFOURCC('s', 't', 'r', 'f')
#define AVI_STRD   TOFOURCC('s', 't', 'r', 'd')
#define AVI_STRN   TOFOURCC('s', 't', 'r', 'n')
#define AVI_JUNK   TOFOURCC('J', 'U', 'N', 'K')
#define AVI_VIDS   TOFOURCC('v', 'i', 'd', 's')    /* video stream*/
#define AVI_AUDS   TOFOURCC('a', 'u', 'd', 's')    /* audio stream */

#define AVI_MOVICK_00DB   TOFOURCC('0', '0', 'd', 'b')     /* Uncompressed video frame */
#define AVI_MOVICK_00DC   TOFOURCC('0', '0', 'd', 'c')     /* Compressed video frame */
#define AVI_MOVICK_00PC   TOFOURCC('0', '0', 'p', 'c')     /* Palette change */
#define AVI_MOVICK_01WB   TOFOURCC('0', '0', 'w', 'b')     /* Audio data */
#define AVI_MOVICK_REC    TOFOURCC('r', 'e', 'c', ' ')     /* 'rec' */

#define AVI_MOVICK_DB       TOTWOCC('d', 'b')   /* Uncompressed video frame */
#define AVI_MOVICK_DC       TOTWOCC('d', 'c')   /* Compressed video frame */
#define AVI_MOVICK_PC       TOTWOCC('p', 'c')   /* Palette change */
#define AVI_MOVICK_WB       TOTWOCC('w', 'b')   /* Audio data */
#define AVI_MOVICK_STREAM0  TOTWOCC('0', '0')   /* Num.0 stream, usually video stream */
#define AVI_MOVICK_STREAM1  TOTWOCC('0', '1')   /* Num.1 stream, usually audio stream */

typedef enum{
    AVIF_HASINDEX       = 0x00000010,	
    AVIF_MUSTUSEINDEX   = 0x00000020,
    AVIF_ISINTERLEAVED  = 0x00000100,
    AVIF_TRUSTCKTYPE    = 0x00000800,	
    AVIF_WASCAPTUREFILE = 0x00010000,
    AVIF_COPYRIGHTED    = 0x00020000,
}IppAVIHeadeFlags;

typedef enum{
    AVISF_DISABLED         = 0x00000001,
    AVISF_VIDEO_PALCHANGES = 0x00010000,
}IppAVIStreamFlags;

typedef enum{
    AVIIF_LIST       = 0x00000001,   
    AVIIF_KEYFRAME   = 0x00000010,   
    AVIIF_FIRSTPART  = 0x00000020,  
    AVIIF_LASTPART   = 0x00000040,  
    AVIIF_MIDPART    = 0x00000060,   
    AVIIF_NOTIME     = 0x00000100,
    AVIIF_COMPUSE    = 0x0FFF0000,
}IppAVIIdxFlags;

typedef enum{
    WAVE_FORMAT_PCM  = 1,
}IppWaveFormat;

typedef struct _IppBMPInfoHeader{
    Ipp32u  ihSize;           /* Size of struct IppBMPInfoHeader, 0x28 for Windows OS */
    Ipp32u  ihWidth;          /* Width of the bitmap, in pixels */
    Ipp32s  ihHeight;         /* Height of the bitmap, in pixels */
    Ipp16u  ihPlanes;         /* Number of planes for the target device, set to 1*/
    Ipp16u  ihBitCount;       /* Number of bits per pixel, can be 1/4/8/16/24/32 */
    Ipp32u  ihCompression;    /* Compression Type for a compressed bitmap, can be BI_RGB / BI_RLE8 / BI_RLE4 */
    Ipp32u  ihSizeImage;      /* Size of the image */
    Ipp32u  ihHResolution;    /* Horizontal resolution of the target device, in pixels per meter */
    Ipp32u  ihVResolution;    /* Vertival resolution of the target device, in pixels per meter */
    Ipp32u  ihClrUsed;        /* Number of actually used color indexes in the color table */
    Ipp32u  ihClrImportant;   /* Number of important color indexes */
}IppBMPInfoHeader;

typedef struct _IppBMPRgbQuad{
    Ipp8u  ctBlue;            /* intensity of blue in the color */
    Ipp8u  ctGreen;           /* intensity of green in the color */
    Ipp8u  ctRed;             /* intensity of red in the color */
    Ipp8u  ctReserve;         /* not used, set to zero */
}IppBMPRgbQuad;

typedef struct _IppWaveFormatEx{
    Ipp16u   sFormatType;     /* Format type */
    Ipp16u   sNumChannel;     /* Number of channels (i.e. mono, stereo...) */
    Ipp32u   iSamplesPerSec;  /* Sample rate */
    Ipp32u   iAvgBytesPerSec; /* Average data rate */
    Ipp16u   sBlockAlign;     /* Block alignment */
    Ipp16u   sBitsPerSample;  /* Number of bits per sample of mono data */
    Ipp16u   sExtraInfoSize;  /* The count in bytes of the extra info */
}IppWaveFormatEx;

typedef struct _IppAVIStreamFormat{
    IppWaveFormatEx  pWaveFormatEx;
    IppBMPInfoHeader pBInfoHeader;
    IppBMPRgbQuad    aColors[256];
}IppAVIStreamFormat;

typedef struct _IppAVIHeader{
    Ipp32u  iMicroSecPerFrame;  /* Periods between video frames */
    Ipp32u  iMaxBytesPerSec;    /* Max data rate */
    Ipp32u  iPaddGranurity;     /* Alignment */
    Ipp32u  iAviHFlags;         /* Flags in the file */
    Ipp32u  iTotalFrames;       /* Total number of frames */
    Ipp32u  iAviHInitialFrames; /* For interleaved files. specify the number of frames in the file prior to the initial frame of the AVI sequence in this field*/
    Ipp32u  iStreams;           /* Number of streams */
    Ipp32u  iAviHSugBufferSize; /* Suggested buffer size to contain the largest chunk in the file */
    Ipp32u  iWidth;             /* Width of the AVI file in pixels */
    Ipp32u  iHeight;            /* Height of the AVI file in pixels */
    Ipp32u  iReserve[4];        /* Reserved, set to 0 */

}IppAVIHeader;

typedef struct _IppAVIStreamHeader{
    Ipp32u  iStreamType;          /* Stream type */
    Ipp32u  iStreamHandler;       /* Data handler */
    Ipp32u  iStreamFlags;         /* Flags in the stream*/
    Ipp16u  sPriority;            /* Priority of a stream type */
    Ipp16u  sLanguage;            /* Language tag */
    Ipp32u  iStreamInitialFrame;  /* the time audio data is skewed ahead fo the video frames in interleaved file */
    Ipp32u  iStreamScale;         /* Time Scale fo the stream*/
    Ipp32u  iStreamRate;          /* Time Scale fo the stream*/
    Ipp32u  iStreamStart;         /* Starting time fo the AVI file */
    Ipp32u  iStreamLength;        /* length of the stream */
    Ipp32s  iStreamSugBufferSize; /* Suggested buffer size to contain the largest chunk in the stream */
    Ipp32s  iQuality;             /* Quality of the data in the stream */    
    Ipp32u  iSampleSize;          /* Size of a single sample of data */
    struct {                      /* destination rectangle for a text or video stream  */
         short int left;          /* within the movie rectangle specified by the dwWidth and dwHeight*/
         short int top;
         short int right;
         short int bottom;
     }  rcFrame;
}IppAVIStreamHeader;

typedef struct _IppAVIdxEntry{
    Ipp32u  iChunkID;         /* Chunk ID */
    Ipp32u  iIdxFlags;        /* Flags */
    Ipp32u  iChunkOff;        /* Position in the file of the specified chunk */
    Ipp32u  iChunklength;     /* Length of the specified chunk */
}IppAVIIdxEntry;


typedef struct _IppMJPEGFmAVIDecoderState{
    IppAVIHeader        *pAVIHeader;
    IppAVIStreamHeader  *pAVIStreamHeader[IPP_MAX_STREAM_NUM];
    IppAVIStreamFormat  *pAVIStreamFormat[IPP_MAX_STREAM_NUM];
    IppAVIIdxEntry      *pAVIIdxEntry;    

    Ipp32u  iAviRiffSize;     /* Size of the chunk with chunk ID 'RIFF', format type 'AVI' */
    Ipp32u  iHdrlListSize;    /* Size of the chunk with list type 'hdrl' */
    Ipp32u  iMoviListSize;    /* Size of the chunk with list type 'movi' */
    Ipp32u  iIdx1ChunkSize;   /* Size of the chunk with chunk ID 'idx1' */
    Ipp32u  iAvihChunkSize;   /* Size of the chunk with chunk ID 'avih' */
    Ipp32u  aStrlListSize[IPP_MAX_STREAM_NUM];  /* Size of the chunk with list type 'strl' */
    Ipp32u  aStrhChunkSize[IPP_MAX_STREAM_NUM]; /* Size of the chunk with chunk ID 'strh' */
    Ipp32u  aStrfChunkSize[IPP_MAX_STREAM_NUM]; /* Size of the chunk with chunk ID 'strf' */
    Ipp8u   cStrlListIdx;     /* Index for aStrlSize array */

    /* callback functions */
    MiscMallocCallback     iaxMemAlloc;
    MiscFreeCallback       iaxMemFree;
    MiscFileSeekCallback   iaxFileSeek;
    MiscFileReadCallback   iaxFileRead;

    void *pStreamHandler;  /* the bitstream file */
}IppMJPEGFmAVIDecoderState;

/*************************************************************************************************************
// Name:  appiAVIParseChunkTypeAndSize
// Description:
//      Parse chunk type and chunk size
// Input Arguments:
//      pSrcBitStream - Pointer to the bit stream structure
// Output Arguments:
//      pChunkID      - Pointer to the FOURCC chunkID
//      pChunkSize    - Pointer to the chunk size
// Returns:
//      IPP_STATUS_NOERR	- OK
// Others:
//      None
*************************************************************************************************************/
IppCodecStatus appiAVIParseChunkTypeAndSize
    (IppBitstream   *pSrcBitStream,
    Ipp32u *pChunkID,
    Ipp32u *pChunkSize) ;

/*************************************************************************************************************
// Name:  appiAVIParseAviHeader
// Description:
//      Parse 'avih' chunk
// Input Arguments:
//      pSrcBitStream  - Pointer to the bit stream structure
// Output Arguments:
//      ppDecoderState - Pointer to a initialized decoder state structure .
// Returns:
//      IPP_STATUS_NOERR	- OK
// Others:
//      None
*************************************************************************************************************/
IppCodecStatus appiAVIParseAviHeader
    (IppBitstream   *pSrcBitStream,
    IppMJPEGFmAVIDecoderState *pDecoderState);

/*************************************************************************************************************
// Name:  appiAVIParseStrh
// Description:
//      Parse 'strh' chunk
// Input Arguments:
//      pSrcBitStream  - Pointer to the bit stream structure
// Output Arguments:
//      ppDecoderState - Pointer to a initialized decoder state structure .
// Returns:
//      IPP_STATUS_NOERR	    - OK
//      IPP_STATUS_NOMEM_ERR - no memory
// Others:
//      None
*************************************************************************************************************/
IppCodecStatus appiAVIParseStrh
    (IppBitstream   *pSrcBitStream,
    IppMJPEGFmAVIDecoderState *pDecoderState) ;

/*************************************************************************************************************
// Name:  appiAVIParseStrf
// Description:
//      Parse 'strf' chunk
// Input Arguments:
//	     pSrcBitStream  - Pointer to the bit stream structure
// Output Arguments:
//	     ppDecoderState - Pointer to a initialized decoder state structure .
// Returns:
//      IPP_STATUS_NOERR	- OK
// Others:
//      None
*************************************************************************************************************/
IppCodecStatus appiAVIParseStrf
    (IppBitstream   *pSrcBitStream,
    IppMJPEGFmAVIDecoderState *pDecoderState) ;

/*************************************************************************************************************
// Name:  appiAVIParseStrlList
// Description:
//      Decoder 'strl' list
// Input Arguments:
//      pSrcBitStream      - Pointer to the bit stream structure
//      iStrlListDataSize  - Length of the 'strl' list
// Output Arguments:
//      ppDecoderState     - Pointer to a initialized decoder state structure .
//      pDstPicture        - Pointer to an IppPicture, include info about origin AVI image
// Returns:
//      IPP_STATUS_NOERR - OK
//      IPP_STATUS_ERR   - Error  
// Others:
//      None
*************************************************************************************************************/
IppCodecStatus appiAVIParseStrlList
    (IppBitstream   *pSrcBitStream,
    IppMJPEGFmAVIDecoderState *pDecoderState,
    Ipp32s iStrlListDataSize);

/*************************************************************************************************************
// Name:  appiAVIParseHdrlList
// Description:
//      Decoder 'hdrl' list, including 'avih' and 'strl'
// Input Arguments:
//      pSrcBitStream      - Pointer to the bit stream structure
//      iHdrlListDataSize  - Length of the 'hdrl' list
// Output Arguments:
//      ppDecoderState     - Pointer to a initialized decoder state structure .
//      pDstPicture        - Pointer to an IppPicture, include info about origin AVI image
// Returns:
//      IPP_STATUS_NOERR	- OK
//      IPP_STATUS_ERR		- Error 
// Others:
//      None
*************************************************************************************************************/
IppCodecStatus appiAVIParseHdrlList
    (IppBitstream   *pSrcBitStream,
    IppMJPEGFmAVIDecoderState *pDecoderState,
    IppPicture *pDstPicture,
    Ipp32s iHdrlListDataSize);

/*************************************************************************************************************
// Name:  DecoderInitAlloc_MJPEGFmAVI
// Description:
//      Init function for AVI decoding, it builds the IppMJPEGFmAVIDecoderState 
// Input Arguments:
//      pInCbTbl       - Pointer to the memory and file related call back functions table
//      fStream        - Pointer to the input AVI file
// Output Arguments:
//      ppDecoderState - Pointer to a initialized AVI decoder structure .
//      pDstPicture    - Pointer to an IppPicture, include info about origin AVI image
// Returns:
//      IPP_STATUS_NOERR      - OK
//      IPP_STATUS_BADARG_ERR	- Bad arguments
//      IPP_STATUS_NOMEM_ERR  - Error because of memory allocate failure 
// Others:
//      None
*************************************************************************************************************/
IppCodecStatus DecoderInitAlloc_MJPEGFmAVI 
    (IppPicture  *pDstPicture,
    void         **ppDecoderState,
    MiscGeneralCallbackTable  *pInCbTbl,
    void * fStream) ;

/*******************************************************************************************************************
// Name:  DecoderFree_MJPEGFmAVI
// Description:
//      Release the allocated working buffer and free the decoder state.
// Input Arguments:
//      ppDecoderState	Pointer to the AVI decoder state structure .
// Output Arguments:
//      None
// Returns:
//      IPP_STATUS_NOERR      - OK
//      IPP_STATUS_BADARG_ERR	- Bad arguments
// Others:
//      None
*********************************************************************************************************************/
IppCodecStatus DecoderFree_MJPEGFmAVI
    (void  **ppDecoderState) ;


/*************************************************************************************************************
// Name:  Parse_MJPEGFmAVI
// Description:
//      Parse the AVI Data
// Input Arguments:
//      pSrcBitStream  - Pointer to a IppBitstream structure
// Output Arguments:
//      ppDecoderState - Pointer to a the AVI decoder state structure .
//      pDstPicture    - Pointer to an IppPicture, include info about origin AVI image
// Returns:
//      IPP_STATUS_NOERR - OK
//      IPP_STATUS_ERR   - Feature Conditions not meeted
// Others:
//      None
*************************************************************************************************************/
IppCodecStatus Parse_MJPEGFmAVI 
    (void  **ppDecoderState, 
    IppBitstream   *pSrcBitStream,
    IppPicture *pDstPicture);

/*************************************************************************************************************
// Name:      appiDecodeJPEGFromAVI
// Description:
//      Decoder JPEG bit streams
// Input Arguments:
//      pBsStart        - Pointer to the start of the bit stream
//      pDstAVIPicture  - Pointer to the AVI picture structure
//      iBsLen          - Length of the bit stream
//      dstFile         - Pointer to the output file 
// Output Arguments:
//      None
// Returns:
//      IPP_STATUS_NOERR     - OK
//      IPP_STATUS_ERR       - Error  
//      IPP_STATUS_NOMEM_ERR - Memory alloc error
// Others:
//      None
*************************************************************************************************************/
IppCodecStatus appiDecodeJPEGFromAVI
    ( Ipp8u * pBsStart,
    Ipp32u iBsLen,
    IppPicture *pDstAVIPicture,
    void * dstFile) ;
#endif