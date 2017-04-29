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
#include "ippdefs.h"
#include "codecDef.h"
#include "codecWMA.h"
#include "asfparser.h"

/* Header Object */
const IPP_GUID_WMA guidHeaderObject = {0x75b22630,0x668e,0x11cf,0xa6,0xd9,0x00,0xaa,0x00,0x62,0xce,0x6c};

/* Header Extension Object */
const IPP_GUID_WMA guidHeaderExtensionObject = {0x5fbf03b5, 0xa92e, 0x11cf, 0x8e, 0xe3, 0x0, 0xc0, 0xc, 0x20, 0x53, 0x65};

/* Stream Properties Object */
const IPP_GUID_WMA guidStreamPropertiesObject = {0xb7dc0791, 0xa9b7, 0x11cf, 0x8e, 0xe6, 0x0, 0xc0, 0xc, 0x20, 0x53, 0x65};

/* Metadata Object */
const IPP_GUID_WMA guidMetaDataObject = {0xc5f8cbea, 0x5baf, 0x4877, 0x84, 0x67, 0xaa, 0x8c, 0x44, 0xfa, 0x4c, 0xca};

/* Data Object */
const IPP_GUID_WMA guidDataObject = {0x75b22636,0x668e,0x11cf, 0xa6,0xd9,0x00,0xaa,0x00,0x62,0xce,0x6c};

/* Stream Bitrate Properties Object*/
const IPP_GUID_WMA guidStreamBitratePropertiesObject = {0x7bf875ce, 0x468d, 0x11d1, 0x8d, 0x82, 0x00, 0x60, 0x97, 0xc9, 0xa2, 0xb2};

/* File properties Object */
const IPP_GUID_WMA guidFilePropertiesObjectV2 = {0x8cabdca1, 0xa947, 0x11cf, 0x8e, 0xe4, 0x0, 0xc0, 0xc, 0x20, 0x53, 0x65};

/*********************************************************************************************
// Name:			 IPP_WriteWaveHeaders
//
// Description:		 Write Wave file format to output file
//
// Input Arguments:  pConfig	- Pointer to the WMA decoder config structure
//
// Output Arguments: pwfio		- Pointer to the output file stream
//
// Returns:			 IPP_OK		- Wave file write succeeded
//					 IPP_FAIL   - Wave file write failed
**********************************************************************************************/
IPPCODECFUN(IppCodecStatus, IPP_WriteWaveHeaders)(IPP_FILE *pwfio,
                 IppWMADecoderConfig *pConfig)
{

  unsigned int ret;
  unsigned int offset = 0;

  IPP_WAVEFORMATEXTENSIBLE waveFmt;

  unsigned int dwTemp;

  if((pwfio == NULL) || (pConfig == NULL))
	  return IPP_FAIL;
	IPP_Memset(&waveFmt, 0x0, sizeof(waveFmt));

	if((pConfig->iDecoderFlags & IPP_DECOPT_CHANNEL_DOWNMIXING) && (pConfig->nChannels > 2)){
		waveFmt.Format.nChannels = 2;
	}else{
		waveFmt.Format.nChannels = pConfig->nChannels;
	}

	waveFmt.Format.nSamplesPerSec = pConfig->nSamplesPerSec;
	if(pConfig->iDecoderFlags & IPP_DECOPT_DOWNSAMPLETO44OR48){
		if (pConfig->nSamplesPerSec % 44100 == 0)
            waveFmt.Format.nSamplesPerSec = 44100;
        if (pConfig->nSamplesPerSec % 48000 == 0)
            waveFmt.Format.nSamplesPerSec = 48000;
	}

	if(pConfig->iDecoderFlags & IPP_DECOPT_REQUANTTO16){
		waveFmt.Format.wBitsPerSample = 16;
	}else{
		waveFmt.Format.wBitsPerSample = pConfig->wBitsPerSample;
	}

	waveFmt.Format.nBlockAlign = waveFmt.Format.nChannels * ((waveFmt.Format.wBitsPerSample + 7) / 8);
	waveFmt.Format.nAvgBytesPerSec = waveFmt.Format.nBlockAlign * waveFmt.Format.nSamplesPerSec;

	if( (pConfig->wFormatTag == WAVE_FORMAT_WMAUDIO3) ||
		(pConfig->wFormatTag == WAVE_FORMAT_WMAUDIO_LOSSLESS)){

		waveFmt.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
		waveFmt.Samples.wValidBitsPerSample = waveFmt.Format.wBitsPerSample;
		if( (pConfig->iDecoderFlags & IPP_DECOPT_CHANNEL_DOWNMIXING) && (waveFmt.Format.nChannels == 2)){
			waveFmt.dwChannelMask = 3;
		}else{
			waveFmt.dwChannelMask =  *((Ipp16u *)(pConfig->pData) + 1);
		}

		waveFmt.SubFormat.Data1       = 0x01;
		waveFmt.SubFormat.Data2       = 0x00;
		waveFmt.SubFormat.Data3       = 0x10;
		waveFmt.SubFormat.Data4[0] = 0x80;
		waveFmt.SubFormat.Data4[1] = 0x00;
		waveFmt.SubFormat.Data4[2] = 0x00;
		waveFmt.SubFormat.Data4[3] = 0xaa;
		waveFmt.SubFormat.Data4[4] = 0x00;
		waveFmt.SubFormat.Data4[5] = 0x38;
		waveFmt.SubFormat.Data4[6] = 0x9b;
		waveFmt.SubFormat.Data4[7] = 0x71;
	}else if ((pConfig->wFormatTag == WAVE_FORMAT_WMAUDIO2) || (pConfig->wFormatTag == WAVE_FORMAT_MSAUDIO1)){

		waveFmt.Format.wFormatTag = WAVE_FORMAT_PCM;
		waveFmt.Samples.wSamplesPerBlock = *((Ipp16u *)(pConfig->pData));
		if(pConfig->nChannels == 6){
			waveFmt.dwChannelMask = (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT |
						  SPEAKER_FRONT_CENTER | SPEAKER_BACK_LEFT |
						  SPEAKER_BACK_RIGHT | SPEAKER_LOW_FREQUENCY);
		}
	}

	if(waveFmt.Format.nChannels > 2 ||
			  (0 != (waveFmt.Format.wBitsPerSample % 8)) ||
			  waveFmt.Format.wBitsPerSample > 16){
		waveFmt.Format.cbSize = 22;
		waveFmt.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
	}else{
		waveFmt.Format.cbSize = 0;
		waveFmt.Format.wFormatTag = WAVE_FORMAT_PCM;
	}

	pConfig->cbSize = waveFmt.Format.cbSize;

  /* write "RIFF" */

  dwTemp = (unsigned int)'R' | ((unsigned int)'I' << 8) | ((unsigned int)'F' << 16) | ((unsigned int)'F' << 24);
  ret = IPP_Fwrite(&dwTemp, 1, sizeof(unsigned int), pwfio);
  if(ret != sizeof(unsigned int))
  {
    return IPP_FAIL;
  }
  offset += ret;

  /* write "RIFF" length, temporarily */

  dwTemp = 0;   /* temporary */
  ret = IPP_Fwrite(&dwTemp, 1, sizeof(unsigned int), pwfio);
  if(ret != sizeof(unsigned int))
  {
  	return IPP_FAIL;
  }
  offset += ret;

  /* write "WAVE" */

  dwTemp = (unsigned int)'W' | ((unsigned int)'A' << 8) | ((unsigned int)'V' << 16) | ((unsigned int)'E' << 24);
  ret = IPP_Fwrite(&dwTemp, 1, sizeof(unsigned int), pwfio);
  if(ret != sizeof(unsigned int))
  {
    return IPP_FAIL;
  }
  offset += ret;

  /* -----------------------------------------------------------------------
   * create FMT chunk
   */

  /* write "fmt " */

   dwTemp = (unsigned int)'f' | ((unsigned int)'m' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)' ' << 24);
  ret = IPP_Fwrite(&dwTemp, 1, sizeof(unsigned int), pwfio);
  if(ret != sizeof(unsigned int))
    {
      return IPP_FAIL;
    }
  offset += ret;

  /* write "fmt " length */

  if (waveFmt.Format.cbSize == 0)
  {
    /* Traditional WAVE_FORMAT_PCM etc */
    dwTemp = 18;
  }else{
	dwTemp = 40;
  }

  ret = IPP_Fwrite(&dwTemp, 1, sizeof(unsigned int), pwfio);
  if(ret != sizeof(unsigned int))
    {
      return IPP_FAIL;
    }
  offset += ret;

  /* write actual format */
  ret = IPP_Fwrite(&(waveFmt.Format), 1, 18, pwfio);
  offset += ret;
  if(waveFmt.Format.cbSize > 0){
	ret = IPP_Fwrite(&(waveFmt.Samples), 1, sizeof(waveFmt.Samples), pwfio);
	offset += ret;
	ret = IPP_Fwrite(&(waveFmt.dwChannelMask), 1, sizeof(waveFmt.dwChannelMask), pwfio);
	offset += ret;
	ret = IPP_Fwrite(&(waveFmt.SubFormat), 1, sizeof(waveFmt.SubFormat), pwfio);
	offset += ret;
  }

  /* -----------------------------------------------------------------------
   * create DATA chunk
   */

  /* write "data" */

  dwTemp = (unsigned int)'d' | ((unsigned int)'a' << 8) | ((unsigned int)'t' << 16) | ((unsigned int)'a' << 24);
  ret = IPP_Fwrite(&dwTemp, 1, sizeof(unsigned int), pwfio);
  if(ret != sizeof(unsigned int))
    {
     return IPP_FAIL;
    }
  offset += ret;

  /* write "data" length */

  dwTemp = 0;
  ret = IPP_Fwrite(&dwTemp, 1, sizeof(unsigned int), pwfio);
  if(ret != sizeof(unsigned int))
	{
	  return IPP_FAIL;
	}

  return IPP_OK;
}
/*********************************************************************************************
// Name:			 IPP_UpdateWaveHeaders
//
// Description:		 Update Wave file chunk data size
//
// Input Arguments:  cbSize     - the count in bytes of the size of Wave file
//					 dataSize   - the whole length of output stream
//
// Output Arguments: pwfio		- Pointer to the output file stream
//
// Returns:			 IPP_OK		- Wave file write succeeded
//					 IPP_FAIL   - Wave file write failed
**********************************************************************************************/
IPPCODECFUN(IppCodecStatus, IPP_UpdateWaveHeaders)(IPP_FILE *pwfio,
                 unsigned int cbSize, int dataSize)
{

	unsigned int length, ret;

	if (pwfio == NULL) return IPP_FAIL;
	length = dataSize + 38 + cbSize;
	IPP_Fseek(pwfio, 4, IPP_SEEK_SET);
	ret = IPP_Fwrite(&length, sizeof(unsigned int), 1, pwfio);
	if ( 1 != ret ) return IPP_FAIL;

	IPP_Fseek(pwfio, 46-4+cbSize, IPP_SEEK_SET);
	ret = IPP_Fwrite(&dataSize, sizeof(unsigned int), 1, pwfio);
	if ( 1 != ret ) return IPP_FAIL;

	return 	IPP_OK;
}

/*********************************************************************************************
// Name:			 ParseASFHeader
//
// Description:		 Parse header information of ASF file
//
// Input Arguments:  fpi		- Pointer to the input file stream
//
// Output Arguments: pConfig	- Pointer to the WMA decoder config structure
//
// Returns:			 IPP_OK		- Parse header succeeded
//					 IPP_FAIL   - Parse header failed
**********************************************************************************************/
IPPCODECFUN(IppCodecStatus,ParseASFHeader)(IPP_FILE *fpi, IppWMADecoderConfig *pConfig)
{
	IPP_GUID_WMA objectId;
	Ipp64u qwSize, qwSizeExt, qwDummyBit, dwObjectSize;
	Ipp32u cHeaders, wExHeaderSize, i, j, dwMinPacketSize;
	Ipp16u cExtHeaders, cBitrateRecords;
	Ipp32s iResult;
	Ipp8u pHeaderObjectTag[_ASF_HEADER_OBJECT_TAG_SIZE], *pBuffer = NULL, *pCurBuffer, *p;
	Ipp8u bfobj[3]={0};
	appWAVEFORMATEX wfx;
	if((fpi == NULL) || (pConfig == NULL))
	{
		return IPP_FAIL;
	}

	if(_ASF_HEADER_OBJECT_TAG_SIZE != IPP_Fread((void *)pHeaderObjectTag,
		1, _ASF_HEADER_OBJECT_TAG_SIZE, fpi))
	{
		return IPP_FAIL;
	}

	pBuffer = (Ipp8u *)pHeaderObjectTag;
	_IPP_LOAD_GUID( objectId, pBuffer );
    _IPP_LOAD_QWORD( dwObjectSize, pBuffer );
	_IPP_LOAD_DWORD( cHeaders, pBuffer );

	if(IPP_Memcmp(&guidHeaderObject, &objectId, sizeof(IPP_GUID_WMA)))
	{
		/* Invalid ASF file */
		return IPP_FAIL;
	}

	/* Remaining header object bitstream */
	iResult = IPP_MemMalloc(&pBuffer, (int)dwObjectSize - _ASF_HEADER_OBJECT_TAG_SIZE, 4);
	if(iResult != 0)
    {
        return IPP_FAIL;
    }
	p = pBuffer;
	if(dwObjectSize - _ASF_HEADER_OBJECT_TAG_SIZE != IPP_Fread((void *)pBuffer,
		1, (int)dwObjectSize - _ASF_HEADER_OBJECT_TAG_SIZE, fpi))
	{
		if(p != NULL)
			IPP_MemFree(&p);
		return IPP_FAIL;
	}

	/* Init config parameter */
	pConfig->iRmsAmplitudeRef = 0;
	pConfig->iRmsAmplitudeTarget = 0;
	pConfig->iPeakAmplitudeRef = 0;
	pConfig->iPeakAmplitudeTarget = 0;

	for (i=0; i<cHeaders; i++)
	{
		_IPP_LOAD_GUID( objectId, pBuffer );
		_IPP_LOAD_QWORD( qwSize, pBuffer );
		if (dwObjectSize <= qwSize)
		{
			if(p != NULL)
				IPP_MemFree(&p);
			return IPP_FAIL;
		}

		if(!IPP_Memcmp(&guidHeaderExtensionObject, &objectId, sizeof(IPP_GUID_WMA)))
		{
		        bfobj[2]=1;
			pCurBuffer = pBuffer;
			_IPP_LOAD_QWORD( qwDummyBit, pCurBuffer ); /* Load dummy bit */
			_IPP_LOAD_QWORD( qwDummyBit, pCurBuffer ); /* Load dummy bit */
			_IPP_LOAD_WORD( cExtHeaders, pCurBuffer );
			_IPP_LOAD_DWORD( wExHeaderSize, pCurBuffer );

			if(wExHeaderSize)
			{
				for (j=0; j<cExtHeaders; j++)
				{
					if((pCurBuffer - pBuffer) > wExHeaderSize)
						break;

					_IPP_LOAD_GUID( objectId, pCurBuffer );
					_IPP_LOAD_QWORD( qwSizeExt, pCurBuffer );

					if(!IPP_Memcmp(&guidMetaDataObject, &objectId, sizeof(IPP_GUID_WMA)))
					{
						ParseMetadataObject(pCurBuffer, pConfig);
					}
					pCurBuffer += qwSizeExt-sizeof(IPP_GUID_WMA)-sizeof(Ipp64u);
				}

				pCurBuffer = pBuffer + wExHeaderSize;
     		}
		}
		if(!IPP_Memcmp(&guidStreamPropertiesObject, &objectId, sizeof(IPP_GUID_WMA)))
		{
			bfobj[1]=1;
			pCurBuffer = pBuffer;
			pCurBuffer += 54; /* stuff bit before stream type, maybe update in the future! */
			IPP_Memcpy(&wfx, pCurBuffer, 18/*sizeof(appWAVEFORMATEX)*/);
			pConfig->wFormatTag = wfx.wFormatTag;
			pConfig->nChannels = wfx.nChannels;
			pConfig->nSamplesPerSec = wfx.nSamplesPerSec;
			pConfig->nAvgBytesPerSec = wfx.nAvgBytesPerSec;
			pConfig->nBlockAlign = wfx.nBlockAlign;
			pConfig->wBitsPerSample = wfx.wBitsPerSample;
			pConfig->cbSize = wfx.cbSize;

			IPP_Memcpy(pConfig->pData, pCurBuffer + 18/*sizeof(appWAVEFORMATEX)*/, wfx.cbSize);
		}
	    if(!IPP_Memcmp(&guidStreamBitratePropertiesObject, &objectId, sizeof(IPP_GUID_WMA)))
		{
			IPP_Memcpy(&cBitrateRecords, pBuffer, 2);
			if(cBitrateRecords > 1) /* MBR */
			{
				IPP_Printf("This is a MultiBitRate(MBR) stream which contains multiple bitstreams of different bit rate. Please split it into single streams!\n");
				if(p != NULL)
				IPP_MemFree(&p);
				return IPP_FAIL;
			}
		}
		if(!IPP_Memcmp(&guidFilePropertiesObjectV2, &objectId, sizeof(IPP_GUID_WMA)))
		{
			bfobj[0]=1;
			pCurBuffer = pBuffer;
			pCurBuffer += 32;
			_IPP_LOAD_QWORD( qwSizeExt, pCurBuffer );
			pCurBuffer += 28;
			// Min data packet size
			_IPP_LOAD_DWORD( dwMinPacketSize, pCurBuffer );
            // Max data packet size
			_IPP_LOAD_DWORD( pConfig->dwPacketSize, pCurBuffer );
            // skip Max bit rate
			pCurBuffer += 4;
			if(dwMinPacketSize != pConfig->dwPacketSize
					|| (qwSizeExt == 0))
			{
				if(p != NULL)
					IPP_MemFree(&p);
				return IPP_FAIL;
			}
		}

		pBuffer += qwSize-sizeof(IPP_GUID_WMA)-sizeof(Ipp64u);
	}
	if( (!(bfobj[0])) || (!(bfobj[1])) || (!(bfobj[2])) || ( pConfig->dwPacketSize < pConfig->nBlockAlign ) )
	{
		IPP_MemFree(&p);
		return -1;
	}
	if(p != NULL)
	IPP_MemFree(&p);
	return IPP_OK;
}

/*********************************************************************************************
// Name:			 ParseMetadataObject
//
// Description:		 Parse Metadata Object in ASF header
//
// Input Arguments:  pBuffer	- Pointer to the input stream buffer
//
// Output Arguments: pConfig	- Pointer to the WMA decoder config structure
//
// Returns:			 IPP_OK		- Parse Metadata Object succeeded
//					 IPP_FAIL   - Parse Metadata Object failed
**********************************************************************************************/
IPPCODECFUN(IppCodecStatus,ParseMetadataObject)(const Ipp8u *pBuffer, IppWMADecoderConfig *pConfig)
{
	Ipp8u *pCurBuffer;
	Ipp32u cbData;
	int i;
	Ipp16u wEntryNum, wReserved, wStreamID, cbName, wDataType, cbDataType;

	Ipp16u PeakRef[] = {'W','M','/','W','M','A','D','R','C','P','e','a','k','R','e','f','e','r','e','n','c','e',0x00};
    Ipp16u PeakTgt[] = {'W','M','/','W','M','A','D','R','C','P','e','a','k','T','a','r','g','e','t',0x00};
    Ipp16u RmsRef[]  = {'W','M','/','W','M','A','D','R','C','A','v','e','r','a','g','e','R','e','f','e','r','e','n','c','e',0x00};
    Ipp16u RmsTgt[]  = {'W','M','/','W','M','A','D','R','C','A','v','e','r','a','g','e','T','a','r','g','e','t',0x00};

	pCurBuffer = (Ipp8u *)pBuffer;

	_IPP_LOAD_WORD( wEntryNum, pCurBuffer );

	if(wEntryNum != 6)
	{
		pConfig->iPeakAmplitudeRef = 0;
		pConfig->iPeakAmplitudeTarget = 0;
		pConfig->iRmsAmplitudeRef = 0;
		pConfig->iRmsAmplitudeTarget = 0;

		return IPP_OK;
	}

	for (i=0; i<wEntryNum; i++)
	{
		/* Load Reserved field */
		_IPP_LOAD_WORD(wReserved, pCurBuffer);
		/* Load Stream Number field */
		_IPP_LOAD_WORD(wStreamID, pCurBuffer);
		/* Load Name Length field */
		_IPP_LOAD_WORD(cbName, pCurBuffer);
		/* Load Data Type field */
		_IPP_LOAD_WORD(wDataType, pCurBuffer);

		switch (wDataType)
		{
		    case 0:  cbDataType = 0; break; /* Unicode String */
		    case 1:  cbDataType = 1; break; /* BYTE array */
			case 2:  cbDataType = 2; break; /* BOOL */
			case 3:  cbDataType = 4; break; /* DWORD */
		    case 4:  cbDataType = 8; break; /* QWORD */
			case 5:  cbDataType = 2; break; /* WORD */
			default: return IPP_FAIL;
		}

		/* Load Data Length field */
		_IPP_LOAD_DWORD(cbData, pCurBuffer);

		if(IPP_Memcmp(PeakRef, pCurBuffer, cbName)==0)
		{
			pCurBuffer += cbName;
			_IPP_LOAD_DWORD(pConfig->iPeakAmplitudeRef, pCurBuffer);
		}
		else if(IPP_Memcmp(PeakTgt, pCurBuffer, cbName)==0)
		{
			pCurBuffer += cbName;
			_IPP_LOAD_DWORD(pConfig->iPeakAmplitudeTarget, pCurBuffer);
		}
		else if(IPP_Memcmp(RmsRef, pCurBuffer, cbName)==0)
		{
			pCurBuffer += cbName;
			_IPP_LOAD_DWORD(pConfig->iRmsAmplitudeRef, pCurBuffer);
		}
		else if(IPP_Memcmp(RmsTgt, pCurBuffer, cbName)==0)
		{
			pCurBuffer += cbName;
			_IPP_LOAD_DWORD(pConfig->iRmsAmplitudeTarget, pCurBuffer);
		}
		else
		{
			pCurBuffer += (cbName+cbData);
		}
	}

	return IPP_OK;
}

/*********************************************************************************************
// Name:			 ParseDataObjectHead
//
// Description:		 Parse Data Object Head
//
// Input Arguments:  fpi		- Pointer to the input file stream
//
// Output Arguments: pConfig	- Pointer to the WMA decoder config structure
//
// Returns:			 IPP_OK		- Parse Data Object succeeded
//					 IPP_FAIL   - Parse Data Object failed
**********************************************************************************************/
IPPCODECFUN(IppCodecStatus,ParseDataObjectHead)(IPP_FILE *fpi, int *pPackets)
{
	IPP_GUID_WMA objectId;
	Ipp8u pBuffer[_ASF_DATA_OBJECT_TAG_SIZE], *pCurBuffer;
	Ipp64u size;
	Ipp16u wReserved;
    if((fpi == NULL) || (pPackets == NULL))
		return IPP_FAIL;

	IPP_Fread(pBuffer, 1, _ASF_DATA_OBJECT_TAG_SIZE, fpi);

	pCurBuffer = pBuffer;

	_IPP_LOAD_GUID( objectId, pCurBuffer);

	if(IPP_Memcmp(&guidDataObject, &objectId, sizeof(IPP_GUID_WMA)))
	{
		return IPP_FAIL;
	}

	_IPP_LOAD_QWORD(size, pCurBuffer);		/* Object size */
	_IPP_LOAD_GUID( objectId, pCurBuffer);	/* File ID */
	_IPP_LOAD_QWORD(size, pCurBuffer);		/* Total data packets */
	_IPP_LOAD_WORD(wReserved, pCurBuffer);	/* Reserved field */

	*pPackets = (int)size;

	return IPP_OK;
}


/*********************************************************************************************
// Name:			 ParseNextPacketAndGetPayload
//
// Description:		 Parse next packet and get the payload data
//
// Input Arguments:  fpi				- Pointer to the input file stream
//                   iInputBufSize		- input buffer size
//                   pBitStream			- Pointer to the bitstream structure
//
// Output Arguments: pBitStream			- Pointer to the bitstream structure
//
// Returns:			 IPP_OK				- Succeeded
//					 IPP_FAIL			- Failed
**********************************************************************************************/
IPPCODECFUN(IppCodecStatus,ParseNextPacketAndGetPayload)(IPP_FILE *fpi, IppWMADecoderConfig *pConfig, IppBitstream *pBitStream)
{
	int iReadLen, iReplicatedDataLen = 0, iPaddingLen = 0;
	Ipp32u readDWord, dwPayLenType, dwPayloads, dWPacketLen=0, dwPayloadDataSize = 0, dwByteLen = 0;
	Ipp8u bEccFlag, bLenTypeFlag, bPropertyFlag;
	int iPacksize = pConfig->dwPacketSize;
	Ipp8u *pTemp;

	int payloadOffset,i;

	Ipp8u bMultiplePayload = 0;
    int iInputBufSize;

	if((fpi == NULL) || (pConfig == NULL) || (pBitStream == NULL))
		return IPP_FAIL;
	iInputBufSize = pConfig->nBlockAlign;
    // ======== If we still have data in the buffer, just use it ========
	if(pBitStream->bsCurBitOffset >= iInputBufSize)
	{
		pBitStream->pBsCurByte = pBitStream->pBsBuffer + pBitStream->bsCurBitOffset;
		pBitStream->bsByteLen = iInputBufSize;
		return IPP_OK;
	}

	/* Error correction flags */
	if(1 != (iReadLen=IPP_Fread(&bEccFlag, 1, 1, fpi))) /* EOF */
	{
		pBitStream->bsByteLen = 0;
		pBitStream->pBsCurByte = pBitStream->pBsBuffer;
		pBitStream->bsCurBitOffset = 0;
		return IPP_FAIL;
	}

	/* Error correction present */
	if(0x80 == ( bEccFlag & 0x80 ) )
	{
		if(bEccFlag & 0x10)
		{
			/* Opaque Data Present */
			return IPP_FAIL;
		}
		if(bEccFlag & 0x60)
		{
			return IPP_FAIL;
		}
		if( (bEccFlag & 0x0f) != 2)
		{
			return IPP_FAIL;
		}
		if(!(bEccFlag &0x60))
		{
			iReadLen += IPP_Fread(&readDWord, 1, 2, fpi);
		}
		iReadLen += IPP_Fread(&bLenTypeFlag, 1, 1, fpi);
	}
	else if(pConfig->wFormatTag == WAVE_FORMAT_WMSP1){
		bLenTypeFlag = bEccFlag;
	}else{
		pBitStream->bsByteLen = 0;
		pBitStream->pBsCurByte = pBitStream->pBsBuffer;
		pBitStream->bsCurBitOffset = 0;
		return IPP_FAIL;
	}

	if(bLenTypeFlag&0x1)//Multi-payload stream
	{
		bMultiplePayload = 1;
	}

	/* Property flags */
	iReadLen += IPP_Fread(&bPropertyFlag, 1, 1, fpi);

	/* Packet length */
	iReadLen += IPP_Fread(&readDWord, 1, ((bLenTypeFlag &0x60)>>5), fpi);

	/* Sequence */
	iReadLen += IPP_Fread(&readDWord, 1, ((bLenTypeFlag &0x6)>>1), fpi);

	/* Padding length */
	iReadLen += IPP_Fread(&iPaddingLen, 1, ((bLenTypeFlag &0x18)>>3), fpi);

	/* Sent time */
	iReadLen += IPP_Fread(&readDWord, 1, 4, fpi);

	/* Duration */
	iReadLen += IPP_Fread(&readDWord, 1, 2, fpi);


	if(bMultiplePayload){
		iReadLen += IPP_Fread(&readDWord, 1, 1, fpi); //read payloads number
		dwPayLenType = ((readDWord & 0xc0)>>6);
		if((dwPayLenType != 1) && (dwPayLenType != 2)){
			return IPP_FAIL;
		}
		dwPayloads = readDWord & 0x3f;
	}else{
		dwPayloads = 1;
	}

	payloadOffset = 0;
	for( i=0; i<dwPayloads; i++){
		/* Stream number */
		iReadLen += IPP_Fread(&readDWord, 1, ((bPropertyFlag &0xc0)>>6), fpi);

		/* Media object number */
		iReadLen += IPP_Fread(&readDWord, 1, ((bPropertyFlag &0x30)>>4), fpi);

		/* Offset into media object */
		iReadLen += IPP_Fread(&readDWord, 1, ((bPropertyFlag &0xc)>>2), fpi);

		/* Replicated data length */
		iReadLen += IPP_Fread(&readDWord, 1, (bPropertyFlag &0x3), fpi);

		/* Read replicated data length */
		iReadLen += IPP_Fread(&iReplicatedDataLen, 1, 1, fpi);

		if(iReplicatedDataLen != 1)
		{
				iReadLen += IPP_Fread(pBitStream->pBsBuffer, 1, iReplicatedDataLen, fpi);

				dwPayloadDataSize = *(Ipp32u*)pBitStream->pBsBuffer;

				if(bMultiplePayload){ //read sub-payload length
					iReadLen += IPP_Fread(&dwPayloadDataSize, 1, dwPayLenType, fpi);
					iReadLen += IPP_Fread(&dwPayloadDataSize, 1, 1, fpi);
				}
		}
		else /* Has sub-payload */
		{
			/* Read presentation time delta */
			iReadLen += IPP_Fread(pBitStream->pBsBuffer, 1, 1, fpi);

			if(bMultiplePayload){ //read sub-payload length
				iReadLen += IPP_Fread(&dwPayloadDataSize, 1, dwPayLenType, fpi);
			}
			iReadLen += IPP_Fread(&dwPayloadDataSize, 1, 1, fpi);
		}
			//while (iReadLen < (int)dwPacketLen)
		{

			/* Read sub-payload #ith data length */
			if((dwPayloadDataSize+payloadOffset)>pBitStream->bsByteLen)
			{
				//pBitStream->pBsBuffer = realloc(pBitStream->pBsBuffer, dwPayloadDataSize);
				IPP_MemMalloc(&pTemp, (dwPayloadDataSize+payloadOffset), 4);
				IPP_Memcpy(pTemp, pBitStream->pBsBuffer, pBitStream->bsByteLen);
				IPP_MemFree(&pBitStream->pBsBuffer);
				pBitStream->pBsBuffer = pTemp;

				pBitStream->bsByteLen = payloadOffset + dwPayloadDataSize ;
			}
			iReadLen += IPP_Fread(pBitStream->pBsBuffer+payloadOffset, 1, dwPayloadDataSize, fpi);
			payloadOffset += dwPayloadDataSize;

		}

	}

	/* Read the padding data */
	if( iPacksize > (iReadLen+iPaddingLen))
		iPaddingLen = iPacksize - iReadLen;

	IPP_Fseek(fpi, iPaddingLen, IPP_SEEK_CUR);
	iReadLen += iPaddingLen;


	/* Init the bitstream */
	if (pConfig->wFormatTag == WAVE_FORMAT_WMSP1)
	{
		pBitStream->bsByteLen = iInputBufSize; //dwPayloadDataSize;
		pBitStream->pBsCurByte = pBitStream->pBsBuffer;
		pBitStream->bsCurBitOffset = payloadOffset - 2*iInputBufSize;
	}
	else
	{
		pBitStream->bsByteLen = payloadOffset; //dwPayloadDataSize;
		pBitStream->pBsCurByte = pBitStream->pBsBuffer;
		pBitStream->bsCurBitOffset = 0;
	}
	return IPP_OK;
}

/* EOF */
