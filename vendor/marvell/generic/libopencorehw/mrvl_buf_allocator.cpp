/*
 * (C) Copyright 2010 Marvell International Ltd.
 * All Rights Reserved
 */
/* ------------------------------------------------------------------
 * Copyright (C) 2009 Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 * -------------------------------------------------------------------
 */

#include "mrvl_buf_allocator.h"
#define LOG_TAG "MrvlBufferAlloc"
#include <utils/Log.h>

MrvlBufferAlloc::MrvlBufferAlloc(uint32 size, uint32 buffers, bool bIsPhyCont): refCount(0), bufferSize(size), maxBuffers(buffers), numAllocated(0), bIsPhyCont(bIsPhyCont)
{
}

MrvlBufferAlloc::~MrvlBufferAlloc()
{
    LOGD("Destroy MrvlBufferAlloc");
}

void MrvlBufferAlloc::addRef()
{
    ++refCount;
}

void MrvlBufferAlloc::removeRef()
{
    --refCount;
    if (refCount <= 0)
    {
        this->~MrvlBufferAlloc();
        OSCL_DELETE(this);
    }
}


OsclAny* MrvlBufferAlloc::allocate()
{
    if (numAllocated < maxBuffers)
    {
	OsclAny *ptr = NULL;
	if (!bIsPhyCont){
            ptr = oscl_malloc(bufferSize);
	} else {
	    ptr = phy_cont_malloc(bufferSize, PHY_CONT_MEM_ATTR_NONCACHED);
	}
        if (ptr) 
	{
	    ++numAllocated;
	    LOGD("allocate(): %s numAllocated=%d size=%d", (bIsPhyCont)?"phy_cont_malloc":"oscl_malloc", numAllocated, bufferSize);
            return ptr;
	}
	else
	    LOGD("allocate() failed: numAllocated=%d max=%d", numAllocated, maxBuffers);
    }
    LOGD("allocate() reaches max num of buffers");
    return NULL;
}

void MrvlBufferAlloc::deallocate(OsclAny* ptr)
{
    if (ptr)
    {
	if (!bIsPhyCont){
            oscl_free(ptr);
	} else {
	    phy_cont_free(ptr);
	}
        --numAllocated;
	LOGD("deallocate(): %s memory fred", (bIsPhyCont)?"phy_cont_free":"oscl_free");
    }
}

uint32 MrvlBufferAlloc::getBufferSize()
{
    return bufferSize;
}

uint32 MrvlBufferAlloc::getNumBuffers()
{
    return maxBuffers;
}


bool MrvlBufferAlloc::queryInterface(const PVUuid& uuid, PVInterface*& aInterface)
{
    aInterface = NULL; // initialize aInterface to NULL in case uuid is not supported

    if (PVMFFixedSizeBufferAllocUUID == uuid)
    {
        // Send back ptr to the allocator interface object
        PVMFFixedSizeBufferAlloc* myInterface   = OSCL_STATIC_CAST(PVMFFixedSizeBufferAlloc*, this);
        refCount++; // increment interface refcount before returning ptr
        aInterface = OSCL_STATIC_CAST(PVInterface*, myInterface);
        return true;
    }

    return false;
}

