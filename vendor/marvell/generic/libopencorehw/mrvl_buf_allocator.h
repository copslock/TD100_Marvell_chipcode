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
#ifndef MRVL_BUF_ALLOCATOR_H
#define MRVL_BUF_ALLOCATOR_H

#include "pvmf_fixedsize_buffer_alloc.h"
#include "pv_interface.h"
#include "oscl_mem.h"
#include "phycontmem.h"

class PVInterface;
class PVMFFixedSizeBufferAlloc;

class MrvlBufferAlloc : public PVInterface, public PVMFFixedSizeBufferAlloc
{
    public:

        MrvlBufferAlloc(uint32 size, uint32 buffers, bool bIsPhyCont);

        ~MrvlBufferAlloc();

        void addRef();

        void removeRef();

        bool queryInterface(const PVUuid& uuid, PVInterface*& aInterface) ;

        OsclAny* allocate();

        void deallocate(OsclAny* ptr) ;

        uint32 getBufferSize() ;

        uint32 getNumBuffers() ;

    private:
        int32 refCount;
        int32 bufferSize;
        int32 maxBuffers;
        int32 numAllocated;
	bool bIsPhyCont;

};

#endif
