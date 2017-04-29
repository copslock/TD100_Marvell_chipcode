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

#include "jpqueue.h"
#include "misc.h"

//////////////////////////////////////////////////////////////////////
// Buffer Queue
//////////////////////////////////////////////////////////////////////
int JPEGBufQueue_Init(JpegBufQueue *this)
{
	int rt = IPP_MutexCreate(&this->hMutex);
	this->headIndex = 0;
	this->count = 0;
	return rt;
}

void JPEGBufQueue_Deinit(JpegBufQueue *this)
{
	if (this->hMutex){
		IPP_MutexDestroy(this->hMutex);
		this->hMutex = NULL;
	}
}

int JPEGBufQueue_EnQueue(JpegBufQueue *this,  jpegBufType *pBufferHeader, int size)
{
	int ret;
		
	IPP_MutexLock(this->hMutex, INFINITE_WAIT, NULL);
	if (JPEG_BUFQUEUE_MAX < this->count + 1){
		ret = -1;
	}
	else {
		unsigned int next = (this->headIndex + this->count) % JPEG_BUFQUEUE_MAX;
		this->buffers[next] = pBufferHeader;
		this->bufSize[next] = size;
		this->count++;
		ret = 0;
	}
		
	IPP_MutexUnlock(this->hMutex);
	return ret;
}

int JPEGBufQueue_DeQueue(JpegBufQueue *this,  jpegBufType **ppBufferHeader, int *pSize)
{
	int ret;
		
	IPP_MutexLock(this->hMutex, INFINITE_WAIT, NULL);
	if (0 >= this->count) {
		ret = -1;
	} else {
		*ppBufferHeader = this->buffers[this->headIndex];
		*pSize = this->bufSize[this->headIndex];
		this->headIndex = (this->headIndex + 1) % JPEG_BUFQUEUE_MAX;
		this->count--;
		ret = 0;
	}
	IPP_MutexUnlock(this->hMutex);
	return ret;
}

void JPEGBufQueue_Flush(JpegBufQueue *this)
{
	IPP_MutexLock(this->hMutex, INFINITE_WAIT, NULL);
	this->headIndex = 0;
	this->count = 0;
	IPP_MutexUnlock(this->hMutex);
}


int JPEG_IsBufQueue_Empty(JpegBufQueue *this)
{
	return (0 == this->count);
}

/* EOF */

