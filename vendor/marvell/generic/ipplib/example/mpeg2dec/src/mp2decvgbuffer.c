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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codecVC.h"
#include "mp2decocder.h"

/***************************************************************************
// Name:             videoInitBuffer
// Description:      Initialize the input bitstream buffer for MPEG4 decoder
//
// Input Arguments:
//      pBufInfo  Pointer to decoder input bitstream buffer
//
// Output Arguments:
//      pBufInfo  Pointer to updated decoder bitstream buffer
//
// Returns:
//     IPP_STATUS_NOERR		--		No Error
*****************************************************************************/
IppCodecStatus videoInitBuffer (IppBitstream *pBufInfo)
{
	/*
	// Initialize IppBitstream
	// at least big enough to store 2 frame data for less reload 
	*/
	pBufInfo->pBsBuffer = (Ipp8u*)malloc(DATA_BUFFER_SIZE+8);
	//additional 8 bytes is used to avoid bit stream overflow when bit error happen in stream.
	//it's not must to have, it's better to have.
	pBufInfo->pBsBuffer[DATA_BUFFER_SIZE] = 0;
	pBufInfo->pBsBuffer[DATA_BUFFER_SIZE+1] = 0;
	pBufInfo->pBsBuffer[DATA_BUFFER_SIZE+2] = 0;
	pBufInfo->pBsBuffer[DATA_BUFFER_SIZE+3] = 0;
	pBufInfo->pBsBuffer[DATA_BUFFER_SIZE+4] = 0;
	pBufInfo->pBsBuffer[DATA_BUFFER_SIZE+5] = 0;
	pBufInfo->pBsBuffer[DATA_BUFFER_SIZE+6] = 1;
	pBufInfo->pBsBuffer[DATA_BUFFER_SIZE+7] = 0xb7;


	if (pBufInfo->pBsBuffer == NULL) {
		return IPP_STATUS_NOMEM_ERR;
	}

	if ( pBufInfo->pBsBuffer ) {
		memset (pBufInfo->pBsBuffer, 0, DATA_BUFFER_SIZE);
	}

	pBufInfo->pBsCurByte = pBufInfo->pBsBuffer;
	pBufInfo->bsByteLen = 0;
	pBufInfo->bsCurBitOffset = 0;
	return IPP_STATUS_NOERR;
}

/***************************************************************************
// Name:             videoReloadBuffer
// Description:      Reload the input bitstream buffer for MPEG4 decoder
//
// Input Arguments:
//      pBufInfo  Pointer to decoder input bitstream buffer
//		stream    Pointer to the input file stream buffer
//
// Output Arguments:
//      pBufInfo  Pointer to updated decoder bitstream buffer
//
// Returns:
//		IPP_STATUS_NOERR		--		No Error
//		IPP_STATUS_ERR			--      fread return 0  		
*****************************************************************************/
IppCodecStatus videoReloadBuffer (IppBitstream *pBufInfo, FILE *stream)
{
	int offset;	
	int	remain_data;
	int readNum;
	offset = (int)(pBufInfo->pBsCurByte - pBufInfo->pBsBuffer);
	if(pBufInfo->bsByteLen > 0 && offset == 0) {
		//no empty space to fill new data
		return IPP_STATUS_NOERR;
	}
	//remain_data = DATA_BUFFER_SIZE - offset;
	remain_data = pBufInfo->bsByteLen - offset;
	if ( remain_data != 0 ) {
		//use memmove instead of memcpy to ensure the right behavior when overlap exist between src and dst
		memmove ( pBufInfo->pBsBuffer, pBufInfo->pBsCurByte, remain_data );
	}
	pBufInfo->pBsCurByte = pBufInfo->pBsBuffer;	
	readNum = (int)fread( pBufInfo->pBsBuffer + remain_data, 1, DATA_BUFFER_SIZE - remain_data, stream );
	pBufInfo->bsByteLen = remain_data +  readNum;
	if (  readNum < DATA_BUFFER_SIZE - remain_data ) {
		return IPP_STATUS_ERR;
	}
	
	return IPP_STATUS_NOERR;
}

	
/***************************************************************************
// Name:             videoFreeBuffer
// Description:      Free input bitstream buffer for MPEG4 decoder
//
// Input Arguments:
//      pBufInfo  Pointer to decoder input bitstream buffer
//
// Output Arguments:
//      pBufInfo  Pointer to updated decoder input bitstream buffer
//
// Returns:
//     IPP_STATUS_NOERR		--		No Error
*****************************************************************************/
IppCodecStatus videoFreeBuffer (IppBitstream *pBufInfo)
{
	if ( pBufInfo->pBsBuffer ) {
		free (pBufInfo->pBsBuffer);
		pBufInfo->pBsBuffer = NULL;
	}
		
	return IPP_STATUS_NOERR;
}


/***********************************************************
// Name:			outfile_frame
// Description:		Write the decoded yuv plane to file, while the outfile is appointed.
//
// Input Parameters:
//		pPicture	Pointer to the output bitstream buffer structure of MPEG-4 decoder
// Output Parameters:
//		outfile		The updated output file stream pointer.
//
// Note:
//   the outfile pointer shouldn't be NULL.
//
*************************************************************/
void outfile_frame (FILE *outfile,IppPicture *pPicture) 
{
	int i,step;
	Ipp8u *pResultY, *pResultU, *pResultV;
	int width, height;
	pResultY = pPicture->ppPicPlane[0];
	pResultU = pPicture->ppPicPlane[1];
	pResultV = pPicture->ppPicPlane[2];

	
	width = pPicture->picWidth;
	height = pPicture->picHeight;

	step = pPicture->picPlaneStep[0];
	for(i = 0; i<height; i++) {
		fwrite(pResultY, sizeof( char ), width, outfile);
		pResultY += step;			
	}


	step = pPicture->picPlaneStep[1];
	for(i = 0; i<(height>>1); i++) {
		fwrite(pResultU, sizeof( char ),width>>1, outfile);
		pResultU += step;
	}

	
	step = pPicture->picPlaneStep[2];
	for(i = 0; i<(height>>1); i++) {
		fwrite(pResultV, sizeof( char ), width>>1, outfile);
		pResultV += step;
	}
	
	return;
}


IppCodecStatus InitCheckVideoInput(FILE* inputCmpFile, IppBitstream* pVideoStream, IppBitstream* pSysStream, Ipp32u* pCurPacketLength, int* pbSystemInfo)
{
	Ipp32u	code;
	Ipp32u  offset=0;
	int		bSystemInfo = 0;
	IppBitstream stream={0};

	if( IPP_STATUS_NOERR != videoInitBuffer(&stream) ) {
		return IPP_STATUS_ERR;
	}
	videoReloadBuffer(&stream, inputCmpFile);
	//  Check the transport stream
    GETNBYTES(&stream,code,1);
	if ( code == 0x47 )
    {
		printf("Transport stream is not supported.\n");
        videoFreeBuffer(&stream);
        return IPP_STATUS_ERR;
    }

    //  Check whether system level info is contained in the stream
    SeekSyncCode(&stream,&offset);
    stream.pBsCurByte += offset>>3;

    GETNBYTES(&stream,code,4);
    switch( code )
    {
    case MPEG2_SEQ_HEADER_SYNC:
        break;
    case MPEG2_PACK_START_CODE:
        bSystemInfo = 1;
        break;
    case MPEG2_VIDEO_ESTREAM:
        bSystemInfo = 1;
        break;
    default:
		printf("Unknown stream!\n");
		videoFreeBuffer(&stream);
		return IPP_STATUS_ERR;
	}

    //  Decode the stream with system level info
    if ( bSystemInfo )
    {
		pSysStream->pBsBuffer = stream.pBsBuffer;
		pSysStream->pBsCurByte = stream.pBsBuffer;
		pSysStream->bsByteLen = 0;
		pSysStream->bsCurBitOffset = 0;
		memset(pSysStream->pBsBuffer, 0, DATA_BUFFER_SIZE);
		
		videoInitBuffer(pVideoStream);

		//reset videostream to fit for videoReloadBufferFromPES_MPEG2()
		pVideoStream->bsByteLen = DATA_BUFFER_SIZE;
        pVideoStream->pBsCurByte = pVideoStream->pBsBuffer + DATA_BUFFER_SIZE;
		
		*pCurPacketLength = 0;
	}else{
		pVideoStream->pBsBuffer = stream.pBsBuffer;
		pVideoStream->pBsCurByte = stream.pBsBuffer;
		pVideoStream->bsByteLen = 0;
		pVideoStream->bsCurBitOffset = 0;
	}
	
	*pbSystemInfo = bSystemInfo;
	fseek(inputCmpFile, 0, SEEK_SET);
	return IPP_STATUS_NOERR;
}


IppCodecStatus RepackFillVideoInput(FILE* inputCmpFile, IppBitstream* pVideoStream, IppBitstream* pSysStream, Ipp32u* pCurPacketLength, int bSysStm)
{
	if(pVideoStream->bsByteLen > 0 && pVideoStream->pBsCurByte == pVideoStream->pBsBuffer) {
		//no old data was used, so no space to fill new data
		return IPP_STATUS_BUFOVERFLOW_ERR;
	}
	if(bSysStm) {
		return videoReloadBufferFromPES_MPEG2(pSysStream,pVideoStream,inputCmpFile,pCurPacketLength);
	}else{
		return videoReloadBuffer(pVideoStream,inputCmpFile);
	}
}

IppCodecStatus FreeVideoInput(IppBitstream* pVideoStream, IppBitstream* pSysStream)
{
	if(pVideoStream) {
		videoFreeBuffer(pVideoStream);
	}
	if(pSysStream) {
		videoFreeBuffer(pSysStream);
	}
	return IPP_STATUS_NOERR;
}



//for mpeg2 PS stream
IppCodecStatus videoReloadBufferFromPES_MPEG2(
	IppBitstream *pSystemBitstream,
	IppBitstream *pVideostream,
	FILE *stream, 
	Ipp32u* puCurPacket_length)
{
	unsigned int offset;	
	unsigned	remain_data;
	unsigned int tp32u;
	IppCodecStatus ret;

	//	Deal with remain bitstream
	offset = (unsigned int)(pVideostream->pBsCurByte - pVideostream->pBsBuffer);
	if(pVideostream->bsByteLen > 0 && offset == 0) {
		//no empty space to fill new data
		return IPP_STATUS_NOERR;
	}
	remain_data = pVideostream->bsByteLen - offset;
	if ( remain_data != 0 )
	{
		//memcpy( pVideostream->pBsBuffer,pVideostream->pBsCurByte, remain_data );
		memmove( pVideostream->pBsBuffer,pVideostream->pBsCurByte, remain_data );
	}
	pVideostream->bsByteLen = remain_data;
	pVideostream->pBsCurByte = pVideostream->pBsBuffer;

	//	Fill buffer from PES packets
	while( offset > 0 )  {
		if ( offset <= *puCurPacket_length )
		{
			//	Copy needed bitstream from current existing PES packet
			if ( offset > (unsigned int)(pSystemBitstream->pBsBuffer - pSystemBitstream->pBsCurByte + pSystemBitstream->bsByteLen) )
			{
				videoReloadBuffer(pSystemBitstream,stream);
			}
			memcpy( pVideostream->pBsBuffer + remain_data, pSystemBitstream->pBsCurByte, offset );
			pVideostream->bsByteLen += offset;
			//if(offset > pSystemBitstream->bsByteLen - (pSystemBitstream->pBsCurByte-pSystemBitstream->pBsBuffer))
			//{
			//	offset = pSystemBitstream->bsByteLen - (pSystemBitstream->pBsCurByte-pSystemBitstream->pBsBuffer);
			//}
			pSystemBitstream->pBsCurByte += offset;
			*puCurPacket_length -= offset;
			return IPP_STATUS_NOERR;
		}else{
			if ( *puCurPacket_length > 0 )
			{
				//	Copy all the left bitstream in current PES packet to video stream
				if ( *puCurPacket_length >(unsigned int)(pSystemBitstream->pBsBuffer - pSystemBitstream->pBsCurByte + pSystemBitstream->bsByteLen) )
				{
					videoReloadBuffer(pSystemBitstream,stream);
				}
				memcpy(pVideostream->pBsBuffer + remain_data,pSystemBitstream->pBsCurByte,*puCurPacket_length);
				pVideostream->bsByteLen += *puCurPacket_length;
				remain_data += *puCurPacket_length;
				offset -= *puCurPacket_length;
				pSystemBitstream->pBsCurByte += *puCurPacket_length;
				*puCurPacket_length = 0;
				tp32u = 0;
			}
			//	Get next packet
			ret = Seek_Next_Packet(pSystemBitstream,stream,puCurPacket_length);
			if ( IPP_STATUS_ERR == ret )
				return IPP_STATUS_ERR;

			tp32u = (unsigned int)(pSystemBitstream->pBsBuffer - pSystemBitstream->pBsCurByte) + pSystemBitstream->bsByteLen;
			tp32u = tp32u > offset? offset:tp32u;
			tp32u = tp32u > *puCurPacket_length? *puCurPacket_length:tp32u;
			//	Copy the needed data from current packet
			memcpy(pVideostream->pBsBuffer + remain_data,pSystemBitstream->pBsCurByte,tp32u);
			pVideostream->bsByteLen += tp32u;
			remain_data += tp32u;
			pSystemBitstream->pBsCurByte += tp32u;
			*puCurPacket_length -= tp32u;
			offset -= tp32u;			
		}
	}
	return IPP_STATUS_NOERR;
}

IppCodecStatus Seek_Next_Packet(IppBitstream *pBitstream,FILE * fpStream,Ipp32u *puPacketLength)
{
	unsigned int code;
	unsigned int tp32u = 0;
	IppCodecStatus ret;
	Ipp8u * pPrevByte;
	int i=0;

	for(;;)
	{
		i++;
		tp32u = 0;
		//	Seek next synccode prefix
		ret = SeekSyncCode(pBitstream,&tp32u);
		if ( IPP_STATUS_ERR == ret ) {
			videoReloadBuffer(pBitstream,fpStream);
			tp32u = 0;
			ret = SeekSyncCode(pBitstream,&tp32u);
			if ( IPP_STATUS_ERR == ret )
				return ret;
		}else{
			//sometimes, the bits left in pBitstream are not enough to do following parse.
			//therefore, do loading to append data.
			videoReloadBuffer(pBitstream,fpStream);
		}

		pBitstream->pBsCurByte += tp32u>>3;
		INITCODE(pBitstream,code);
		pBitstream->pBsCurByte += 4;

		switch(code)
		{
		case MPEG2_PACK_START_CODE:
			//	Packet header
			break;
		case MPEG2_VIDEO_ESTREAM:   
			//	Parse elementary video stream packet data length
			GETNBYTES(pBitstream,(*puPacketLength),2);
			pBitstream->pBsCurByte += 2;
			pPrevByte = pBitstream->pBsCurByte;
			GETNBYTES(pBitstream,code,1);
			pBitstream->pBsCurByte++;
			if((code>>6)==0x02)
			{
				pBitstream->pBsCurByte++;
				//	Parse PES header data length
				GETNBYTES(pBitstream,tp32u,1);
				//	Advance current byte to packet data beginning
				pBitstream->pBsCurByte += tp32u + 1;
				*puPacketLength -= (Ipp32u)(pBitstream->pBsCurByte - pPrevByte);
				printf("MPEG-2 PES packet\n");
				return IPP_STATUS_NOERR;
			}
			else if(code==0xff)
			{
				// Parse MPEG-1 packet header
				while((pBitstream->pBsCurByte[0])== 0xFF){
					pBitstream->pBsCurByte++;
				};
				GETNBYTES(pBitstream,code,1);
			}

			// Stuffing bytes
			if(code>=0x40)
			{
				if(code>=0x80)
				{
					printf("Error in packet header\n");
					return IPP_STATUS_ERR;
				}
				// Skip STD_buffer_scale
				pBitstream->pBsCurByte++;
				GETNBYTES(pBitstream,code,1);
				pBitstream->pBsCurByte++;
			}

			if(code>=0x30)
			{
				if(code>=0x40)
				{
					printf("Error in packet header\n");
					return IPP_STATUS_ERR;
				}
				// Skip presentation and decoding time stamps
				pBitstream->pBsCurByte += 9;
			}
			else if(code>=0x20)
			{
				// skip presentation time stamps
				pBitstream->pBsCurByte += 4;
			}
			else if(code!=0x0f)
			{
				printf("Error in packet header\n");
				return IPP_STATUS_ERR;
			}
			return IPP_STATUS_NOERR;
		case MPEG2_ISO_END_CODE: 
			return IPP_STATUS_NEW_VIDEO_SEQ;
		default:
			if(code>=0xBB)
			{
				// Skip system headers and non-video packets
				GETNBYTES(pBitstream,code,2);
				pBitstream->pBsCurByte += 2;
				if ( code > (unsigned int)(pBitstream->pBsBuffer - pBitstream->pBsCurByte + pBitstream->bsByteLen) )
				{
					videoReloadBuffer(pBitstream,fpStream);
				}
				pBitstream->pBsCurByte += code;
			}
			else
			{
				printf("Unexpected startcode %08x in system layer\n",code);
				return IPP_STATUS_ERR;
			}
			break;
		}
	}
}

//	Fast Kunth string matching algorithm for string "0x0000 001xx"
IppCodecStatus	SeekSyncCode(
			IppBitstream				*pSrcBitstream,
			Ipp32u						*pOffset)
{
	Ipp32u code = 0;
	Ipp32u	syncCode = 0x00000100;
	Ipp8u *pByte = pSrcBitstream->pBsCurByte;

	if ((pSrcBitstream->bsCurBitOffset&0x7) != 0 )
	{
		*pOffset = *pOffset + (8-(pSrcBitstream->bsCurBitOffset&0x7));
		pByte++;
	}

	code |= ((pByte[0])<<24);
	code |= ((pByte[1])<<16);
	code |= ((pByte[2])<<8);
	code |= ((pByte[3]));
	
	for (;;)
	{
		if ( (pByte - pSrcBitstream->pBsBuffer) + 4 > pSrcBitstream->bsByteLen )
			return IPP_STATUS_ERR;
		code = 	(pByte[1])<<16;
		if (code == 0)
		{
			code |= ((pByte[0])<<24);
			if (code)
			{
				code <<= 8;
				code |= ((pByte[2])<<16);
				code |= ((pByte[3])<<8);

				if (code != syncCode)
				{
					*pOffset = *pOffset + 16;
					pByte += 2;
				} else {
					*pOffset = *pOffset + 8;
					pByte++;
					break;		//000001 has been found
				}
			}
			else
			{
				code |= ((pByte[2])<<8);

				if (code != syncCode)
				{
					*pOffset = *pOffset + 8;
					pByte++;
				} else{
					break;	//000001 has been found
				}
			}
		}
		else
		{ 
			*pOffset = *pOffset + 16;
			pByte += 2;
		}
	}
	if ( (pByte - pSrcBitstream->pBsBuffer) + 4 > pSrcBitstream->bsByteLen ) {
		return IPP_STATUS_ERR;
	}else{
		return IPP_STATUS_NOERR;
	}
}


