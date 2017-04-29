/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <limits.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#include <cutils/log.h>
#include <cutils/atomic.h>

#include <hardware/hardware.h>
#include <hardware/gralloc.h>

#if HAVE_ANDROID_OS
#include <linux/android_pmem.h>
#endif

#include "gralloc_priv.h"

#include <bmm_lib.h>

// we need this for now because pmem cannot mmap at an offset
#define PMEM_HACK   1

/* desktop Linux needs a little help with gettid() */
#if defined(ARCH_X86) && !defined(HAVE_ANDROID_OS)
#define __KERNEL__
# include <linux/unistd.h>
pid_t gettid() { return syscall(__NR_gettid);}
#undef __KERNEL__
#endif

/*****************************************************************************/

static int gralloc_map(gralloc_module_t const* module,
        buffer_handle_t handle,
        void** vaddr)
{
    private_handle_t* hnd = (private_handle_t*)handle;
    // bypass full screen ap need map frame buffer in ap thread not surfaceflinger thread
    if (!((hnd->flags & private_handle_t::PRIV_FLAGS_FRAMEBUFFER) && (hnd->pid == getpid()))) {
        size_t size = hnd->size;
#if PMEM_HACK
        size += hnd->offset;
#endif
        void* mappedAddress = mmap(0, size,
                PROT_READ|PROT_WRITE, MAP_SHARED, hnd->fd, 0);
        if (mappedAddress == MAP_FAILED) {
            LOGE("Could not mmap %s", strerror(errno));
            return -errno;
        }
        hnd->base = intptr_t(mappedAddress) + hnd->offset;
        //LOGD("gralloc_map() succeeded fd=%d, off=%d, size=%d, vaddr=%p", 
        //        hnd->fd, hnd->offset, hnd->size, mappedAddress);
    }
    *vaddr = (void*)hnd->base;
    return 0;
}

static int gralloc_unmap(gralloc_module_t const* module,
        buffer_handle_t handle)
{
    private_handle_t* hnd = (private_handle_t*)handle;
    // bypass full screen ap need map frame buffer in ap thread not surfaceflinger thread
    if (!((hnd->flags & private_handle_t::PRIV_FLAGS_FRAMEBUFFER) && (hnd->pid == getpid()))) {
        void* base = (void*)hnd->base;
        size_t size = hnd->size;
#if PMEM_HACK
        base = (void*)(intptr_t(base) - hnd->offset);
        size += hnd->offset;
#endif
        //LOGD("unmapping from %p, size=%d", base, size);
        if (munmap(base, size) < 0) {
            LOGE("Could not unmap %s", strerror(errno));
        }
    }
    hnd->base = 0;
    return 0;
}

/*****************************************************************************/

static pthread_mutex_t sMapLock = PTHREAD_MUTEX_INITIALIZER; 

/*****************************************************************************/

int gralloc_register_buffer(gralloc_module_t const* module,
        buffer_handle_t handle)
{
    if (private_handle_t::validate(handle) < 0)
        return -EINVAL;

    // In this implementation, we don't need to do anything here

    /* NOTE: we need to initialize the buffer as not mapped/not locked
     * because it shouldn't when this function is called the first time
     * in a new process. Ideally these flags shouldn't be part of the
     * handle, but instead maintained in the kernel or at least 
     * out-of-line
     */ 

    // if this handle was created in this process, then we keep it as is.
    private_handle_t* hnd = (private_handle_t*)handle;
    if (hnd->pid != getpid()) {
        hnd->base = 0;
        hnd->lockState  = 0;
        hnd->writeOwner = 0;

        // Initialize fakeBase, format and lock rectangle
        hnd->fakeBase = 0;
        hnd->fakeFree = 0;
        hnd->left = 0;
        hnd->top = 0;
        hnd->width = 0;
        hnd->height = 0;
    }

    return 0;
}

int gralloc_unregister_buffer(gralloc_module_t const* module,
        buffer_handle_t handle)
{
    if (private_handle_t::validate(handle) < 0)
        return -EINVAL;

    /*
     * If the buffer has been mapped during a lock operation, it's time
     * to un-map it. It's an error to be here with a locked buffer.
     * NOTE: the framebuffer is handled differently and is never unmapped.
     */

    private_handle_t* hnd = (private_handle_t*)handle;
    
    LOGE_IF(hnd->lockState & private_handle_t::LOCK_STATE_READ_MASK,
            "[unregister] handle %p still locked (state=%08x)",
            hnd, hnd->lockState);

    // never unmap buffers that were created in this process
    if (hnd->pid != getpid()) {
        if (hnd->lockState & private_handle_t::LOCK_STATE_MAPPED) {
            gralloc_unmap(module, handle);
        }
        hnd->base = 0;
        hnd->lockState  = 0;
        hnd->writeOwner = 0;

        // Free "fake" buffer if it exists
        if (hnd->fakeFree)
        {
            free((void*) hnd->fakeFree);
            hnd->fakeFree = 0;
            hnd->fakeBase= 0;
        }

        // Reset format and lock rectangle
        hnd->left = 0;
        hnd->top = 0;
        hnd->width = 0;
        hnd->height = 0;
    }

    return 0;
}

int terminateBuffer(gralloc_module_t const* module,
        private_handle_t* hnd)
{
    /*
     * If the buffer has been mapped during a lock operation, it's time
     * to un-map it. It's an error to be here with a locked buffer.
     */

    LOGE_IF(hnd->lockState & private_handle_t::LOCK_STATE_READ_MASK,
            "[terminate] handle %p still locked (state=%08x)",
            hnd, hnd->lockState);
    
    if (hnd->lockState & private_handle_t::LOCK_STATE_MAPPED) {
        // this buffer was mapped, unmap it now
	gralloc_unmap(module, hnd);
    }

    // Free "fake" buffer if it exists
    if (hnd->fakeFree)
    {
        free((void*) hnd->fakeFree);
        hnd->fakeFree = 0;
        hnd->fakeBase = 0;
    }

    // Reset format and lock rectangle
    hnd->left = 0;
    hnd->top = 0;
    hnd->width = 0;
    hnd->height = 0;
    return 0;
}

int gralloc_lock(gralloc_module_t const* module,
        buffer_handle_t handle, int usage,
        int l, int t, int w, int h,
        void** vaddr)
{
    if (private_handle_t::validate(handle) < 0)
        return -EINVAL;

    int err = 0;
    private_handle_t* hnd = (private_handle_t*)handle;
    int32_t current_value, new_value;
    int retry;

    do {
        current_value = hnd->lockState;
        new_value = current_value;

        if (current_value & private_handle_t::LOCK_STATE_WRITE) {
            // already locked for write 
            LOGE("handle %p already locked for write", handle);
            return -EBUSY;
        } else if (current_value & private_handle_t::LOCK_STATE_READ_MASK) {
            // already locked for read
            if (usage & (GRALLOC_USAGE_SW_WRITE_MASK | GRALLOC_USAGE_HW_RENDER)) {
                LOGE("handle %p already locked for read", handle);
                return -EBUSY;
            } else {
                // this is not an error
                //LOGD("%p already locked for read... count = %d", 
                //        handle, (current_value & ~(1<<31)));
            }
        }

        // not currently locked
        if (usage & (GRALLOC_USAGE_SW_WRITE_MASK | GRALLOC_USAGE_HW_RENDER)) {
            // locking for write
            new_value |= private_handle_t::LOCK_STATE_WRITE;
        }
        new_value++;

        retry = android_atomic_cmpxchg(current_value, new_value, 
                (volatile int32_t*)&hnd->lockState);
    } while (retry);

    if (new_value & private_handle_t::LOCK_STATE_WRITE) {
        // locking for write, store the tid
        hnd->writeOwner = gettid();
    }


    // If SW rendering to PMEM & ashmem instead of Frambuffer, we need to flush on unlock
    if ( (usage & GRALLOC_USAGE_SW_WRITE_MASK) &&
         !(hnd->flags & private_handle_t::PRIV_FLAGS_FRAMEBUFFER))
    {
        hnd->flags |= private_handle_t::PRIV_FLAGS_NEEDS_FLUSH;
        hnd->top = t;
        hnd->height = h;
    }
    //
    //  TODO & Caution !!!
    //  For ashmem buffer, we also need to flush it.
    //  But how to ? PMEM needs to provide flush & invalidate API for normal memory.
    // Add HW usage for virtual addr map.
    if (usage & (GRALLOC_USAGE_SW_READ_MASK | GRALLOC_USAGE_SW_WRITE_MASK
	       | GRALLOC_USAGE_HW_TEXTURE | GRALLOC_USAGE_HW_RENDER)) {
        if (!(current_value & private_handle_t::LOCK_STATE_MAPPED)) {
            // we need to map for real
            pthread_mutex_t* const lock = &sMapLock;
            pthread_mutex_lock(lock);
            if (!(hnd->lockState & private_handle_t::LOCK_STATE_MAPPED)) {
                err = gralloc_map(module, handle, vaddr);
                if (err == 0) {
                    android_atomic_or(private_handle_t::LOCK_STATE_MAPPED,
                            (volatile int32_t*)&(hnd->lockState));
                }
            }
            pthread_mutex_unlock(lock);
        }
        *vaddr = (void*)hnd->base;
    }

    // If buffer is for SW rendering and format is 8888, we need return a fake buffer
    if ( (usage & (GRALLOC_USAGE_SW_WRITE_MASK | GRALLOC_USAGE_SW_READ_MASK)) &&
         hnd->bBGRAHack && (hnd->bpp == 4) &&
         !(hnd->flags & private_handle_t::PRIV_FLAGS_FRAMEBUFFER))
    {
        // We need create a "fake" buffer
        if (!hnd->fakeBase)
        {
            hnd->fakeFree = (int) malloc(hnd->size+63);
            if (hnd->fakeFree == 0)
            {
                LOGE("handle %p malloc fakeBuffer failed", handle);
                return err;
            }
            hnd->fakeBase = (int)(hnd->fakeFree + 63) & ~63;
        }

        // Update lock rectangle
        hnd->left = l;
        hnd->top = t;
        hnd->width = w;
        hnd->height = h;

        // Return fake buffer for SW rendering
        *vaddr = (void*)hnd->fakeBase;
    }

    // Save usage for skip copy check in gralloc-unlock
    hnd->usage = usage;

    return err;
}

/* WMMX ARGB8888ToABGR8888 transform */
extern "C" void ARGB8888ToABGR8888_MRVL(unsigned char * pSrc, int srcStride, unsigned int srcWidth, unsigned int srcHeight,
                             unsigned char * pDst, int dstStride);

int gralloc_unlock(gralloc_module_t const* module, 
        buffer_handle_t handle)
{
    if (private_handle_t::validate(handle) < 0)
        return -EINVAL;

    private_handle_t* hnd = (private_handle_t*)handle;
    int32_t current_value, new_value;

    // If exists fake buffer, we need do transform from fake buffer(BGRA)
    // into real buffer(RGBA)
    if (hnd->bBGRAHack && hnd->fakeBase && (hnd->usage & GRALLOC_USAGE_SW_WRITE_MASK))
    {
        ARGB8888ToABGR8888_MRVL(((unsigned char*)hnd->fakeBase + hnd->top * hnd->stride + hnd->left * hnd->bpp), hnd->stride>>2, hnd->width, hnd->height,
                                ((unsigned char*)hnd->base + hnd->top * hnd->stride + hnd->left * hnd->bpp), hnd->stride>>2);
    }

    // Flush CPU after SW rendering
    if (hnd->flags & private_handle_t::PRIV_FLAGS_NEEDS_FLUSH)
    {
        struct pmem_region region;
        int err;

        // Flush buffer from pmem
        if (hnd->flags & private_handle_t::PRIV_FLAGS_USES_PMEM)
        {
            if(hnd->bpr > 0)
            {
                // Partial cache flush for normal gralloc buffer
                int offset = hnd->top * hnd->bpr;
                int len = hnd->height * hnd->bpr;
                int left = hnd->size - offset;

                len = (len >= 0) ? len : 0;
                len = (len <= left) ? len : left;

                region.offset   = hnd->offset + offset;
                region.len      = len;
            }
            else
            {
                // Full cache flush for bpr is 0 case, like framebuffer and yuv
                region.offset   = hnd->offset;
                region.len      = hnd->size;
            }

            if (hnd->fd >= 0)
            {
                int flush_fd = (hnd->pid != getpid()) ? hnd->fd : hnd->master_fd;
                err = ioctl(flush_fd, PMEM_CACHE_FLUSH, &region);
                LOGE_IF(err < 0, "cannot flush handle %p (offs=%x len=%x)\n",
                        hnd, hnd->offset, hnd->size);
            }
        }
        // Flush buffer from ashmem
        else
        {
            if (hnd->base)
            {
                LOGD("%s : %d : BMM cache flush for ASHMEM memory", __FUNCTION__, __LINE__);
                bmm_flush_user((void*)hnd->base, hnd->size, BMM_DMA_BIDIRECTIONAL);
            }
        }

        hnd->flags &= ~private_handle_t::PRIV_FLAGS_NEEDS_FLUSH;
    }

    do {
        current_value = hnd->lockState;
        new_value = current_value;

        if (current_value & private_handle_t::LOCK_STATE_WRITE) {
            // locked for write
            if (hnd->writeOwner == gettid()) {
                hnd->writeOwner = 0;
                new_value &= ~private_handle_t::LOCK_STATE_WRITE;
            }
        }

        if ((new_value & private_handle_t::LOCK_STATE_READ_MASK) == 0) {
            LOGE("handle %p not locked", handle);
            return -EINVAL;
        }

        new_value--;

    } while (android_atomic_cmpxchg(current_value, new_value, 
            (volatile int32_t*)&hnd->lockState));

    return 0;
}

/*****************************************************************************/

int gralloc_perform(struct gralloc_module_t const* module,
        int operation, ... )
{
    int res = -EINVAL;
    va_list args;
    va_start(args, operation);

    switch (operation) {
        case GRALLOC_MODULE_PERFORM_CREATE_HANDLE_FROM_BUFFER: {
            int fd = va_arg(args, int);
            size_t size = va_arg(args, size_t);
            size_t offset = va_arg(args, size_t);
            void* base = va_arg(args, void*);
            native_handle_t** handle = va_arg(args, native_handle_t**);
            private_handle_t* hnd = (private_handle_t*)native_handle_create(
                    private_handle_t::sNumFds, private_handle_t::sNumInts);
            hnd->magic = private_handle_t::sMagic;
            hnd->fd = fd;
            hnd->flags = private_handle_t::PRIV_FLAGS_USES_PMEM;
            hnd->size = size;
            hnd->offset = offset;
            hnd->base = intptr_t(base) + offset;
            hnd->lockState = private_handle_t::LOCK_STATE_MAPPED;
            *handle = (native_handle_t *)hnd;
            res = 0;
            break;
        }
        
        // MRVL changes for optimization
    	case GRALLOC_MODULE_PERFORM_GET_BUFFER_FROM_HANDLE: {
            const private_module_t * m = reinterpret_cast<const private_module_t*>(module);
            native_handle_t* handle = va_arg(args, native_handle_t*);
    	    private_handle_t* hnd = (private_handle_t*)handle;
    		int *master_fd = va_arg(args, int *);
    	    int *fd = va_arg(args, int*);
                size_t *size = va_arg(args, size_t*);
                size_t *offset = va_arg(args, size_t*);

            *master_fd = hnd->fd;
    	    *fd = hnd->fd;
    	    *size = hnd->size;
    	    *offset = hnd->offset;
    	    res = 0;
                break;
    	}
        case GRALLOC_MODULE_PERFORM_GET_BUFFER_MAPFLAG: {
            const private_module_t * m = reinterpret_cast<const private_module_t*>(module);
            native_handle_t* handle = va_arg(args, native_handle_t*);
            private_handle_t* hnd = (private_handle_t*)handle;
            size_t *bMapped    = va_arg(args, size_t*);
            *bMapped    = hnd->bMapped;
            res = 0;
            break;
        }

        case GRALLOC_MODULE_PERFORM_SET_BUFFER_MAPFLAG: {
             const private_module_t * m = reinterpret_cast<const private_module_t*>(module);
             native_handle_t* handle = va_arg(args, native_handle_t*);
             private_handle_t* hnd = (private_handle_t*)handle;
             size_t *bMapped    = va_arg(args, size_t*);
             hnd->bMapped = *bMapped;
             res = 0;
             break;

        }
    }

    va_end(args);
    return res;
}
