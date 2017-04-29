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

#include <sys/mman.h>

#include <dlfcn.h>

#include <cutils/ashmem.h>
#include <cutils/log.h>

#include <hardware/hardware.h>
#include <hardware/gralloc.h>

#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdlib.h>

#include <cutils/log.h>
#include <cutils/atomic.h>
#include <cutils/properties.h>

#include <EGL/egl.h>

#if HAVE_ANDROID_OS
#include <linux/fb.h>
#endif

#include "gralloc_priv.h"
#include "gr.h"

// Special ioctl for HDMI mirror rotation
#define FB_IOCTL_ROTATE_ADDR    0xe9

/*****************************************************************************/



enum {
    PAGE_FLIP = 0x00000001,
    LOCKED = 0x00000002
};

struct fb_context_t {
    framebuffer_device_t  device;
};

/*****************************************************************************/

static unsigned int g_fb_bpp = 16;

#define FB_IOC_MAGIC                        'm'
#define FB_IOCTL_WAIT_VSYNC_ON                 _IO(FB_IOC_MAGIC, 20)
#define FB_IOCTL_WAIT_VSYNC_OFF                _IO(FB_IOC_MAGIC, 21)

static int fb_setSwapInterval(struct framebuffer_device_t* dev,
            int interval)
{
    fb_context_t* ctx = (fb_context_t*)dev;
    if (interval < dev->minSwapInterval || interval > dev->maxSwapInterval)
        return -EINVAL;

    private_module_t* m = reinterpret_cast<private_module_t*>(
            dev->common.module);
    if(interval > 0)
      {
        if (ioctl(m->framebuffer->fd, FB_IOCTL_WAIT_VSYNC_ON, &m->info) == -1) {
            LOGE("FBIOPUT_VSCREENINFO failed");
            return -errno;
        }
      }
    else
      {
        if (ioctl(m->framebuffer->fd, FB_IOCTL_WAIT_VSYNC_OFF, &m->info) == -1) {
            LOGE("FBIOPUT_VSCREENINFO failed");
            return -errno;
        }
      }

    return 0;
}

static int fb_setUpdateRect(struct framebuffer_device_t* dev,
        int l, int t, int w, int h)
{
    if (((w|h) <= 0) || ((l|t)<0))
        return -EINVAL;
        
    fb_context_t* ctx = (fb_context_t*)dev;
    private_module_t* m = reinterpret_cast<private_module_t*>(
            dev->common.module);
    m->info.reserved[0] = 0x54445055; // "UPDT";
    m->info.reserved[1] = (uint16_t)l | ((uint32_t)t << 16);
    m->info.reserved[2] = (uint16_t)(l+w) | ((uint32_t)(t+h) << 16);
    return 0;
}

static int fb_post(struct framebuffer_device_t* dev, buffer_handle_t buffer)
{
    if (private_handle_t::validate(buffer) < 0)
        return -EINVAL;

    fb_context_t* ctx = (fb_context_t*)dev;

    private_handle_t const* hnd = reinterpret_cast<private_handle_t const*>(buffer);
    private_module_t* m = reinterpret_cast<private_module_t*>(
            dev->common.module);
    
    if (m->currentBuffer) {
        m->base.unlock(&m->base, m->currentBuffer);
        m->currentBuffer = 0;
    }

    if (hnd->flags & private_handle_t::PRIV_FLAGS_FRAMEBUFFER) {

        m->base.lock(&m->base, buffer, 
                private_module_t::PRIV_USAGE_LOCKED_FOR_POST, 
                0, 0, m->info.xres, ALIGN(m->info.yres, 16), NULL);


        const size_t offset = hnd->base - m->framebuffer->base;

        if(m->m_gcu_inited)
        {

            char    prop_value[16];
            GCUbool mirror_enable = false;
            memset(prop_value, 0, sizeof(prop_value));
            property_get("service.fb_mirror", prop_value, "0");

            if(strcmp(prop_value, "1") == 0)
            {
                mirror_enable = true;
            }

            if(mirror_enable)
            {
                GCU_ROTATION rotation = GCU_ROTATION_90;

		property_get("service.screen.rotation", prop_value, "0");
		// service.screen.rotation = 1 : rotate 270
		// service.screen.rotation = 3 : rotate 90
                if( !strcmp(prop_value, "1") ) {
		    rotation = GCU_ROTATION_270;
		}
		_gcuUpdatePreAllocBuffer(m->m_gcu_context, m->m_gcu_src_surface,
                                         GCU_TRUE, (void*)(hnd->base),
                                         GCU_TRUE,  m->finfo.smem_start + offset);

                _gcuUpdatePreAllocBuffer(m->m_gcu_context, m->m_gcu_dst_surface,
                                         GCU_TRUE, (char*)m->m_gcu_tmp_vaddr + offset,
                                         GCU_TRUE,  m->m_gcu_tmp_paddr + offset);

                GCU_BLT_DATA bltData;
                memset(&bltData, 0, sizeof(bltData));
                bltData.pSrcSurface = m->m_gcu_src_surface;
                bltData.pDstSurface = m->m_gcu_dst_surface;
                bltData.pSrcRect    = NULL;
                //bltData.pDstRect    = NULL;

                GCU_RECT dstRect;
                dstRect.left = 0;
                dstRect.top = 0;
                dstRect.right = m->info.yres;
                dstRect.bottom = m->info.xres;
                bltData.pDstRect = &dstRect;

                bltData.rotation    = rotation;
                gcuBlit(m->m_gcu_context, &bltData);
                gcuFinish(m->m_gcu_context);

                // _gcuDumpSurface(m->m_gcu_context, m->m_gcu_src_surface, "/data/m_gcu_src");
                // _gcuDumpSurface(m->m_gcu_context, m->m_gcu_dst_surface, "/data/m_gcu_dst");

                ioctl(m->framebuffer->fd, FB_IOCTL_ROTATE_ADDR, m->m_gcu_tmp_paddr + offset);
            }

            if(mirror_enable != m->m_gcu_mirror_enable)
            {
                if(mirror_enable)
                {
                     // ioctl(m->framebuffer->fd, FB_IOCTL_ROTATE_ADDR, m->m_gcu_tmp_paddr);
                }
                else
                {
                     ioctl(m->framebuffer->fd, FB_IOCTL_ROTATE_ADDR, 0);
                }

                m->m_gcu_mirror_enable = mirror_enable;
            }
        }

        m->info.activate = FB_ACTIVATE_VBL;
        m->info.yoffset = offset / m->finfo.line_length;
        if (ioctl(m->framebuffer->fd, FBIOPAN_DISPLAY, &m->info) == -1) {
            LOGE("FBIOPUT_VSCREENINFO failed");
            m->base.unlock(&m->base, buffer); 
            return -errno;
        }

        m->currentBuffer = buffer;
        
    } else {
        // If we can't do the page_flip, just copy the buffer to the front 
        // FIXME: use copybit HAL instead of memcpy
        
        void* fb_vaddr;
        void* buffer_vaddr;
        
        m->base.lock(&m->base, m->framebuffer, 
                GRALLOC_USAGE_SW_WRITE_RARELY, 
                0, 0, m->info.xres, m->info.yres,
                &fb_vaddr);

        m->base.lock(&m->base, buffer, 
                GRALLOC_USAGE_SW_READ_RARELY, 
                0, 0, m->info.xres, m->info.yres,
                &buffer_vaddr);

        memcpy(fb_vaddr, buffer_vaddr, m->finfo.line_length * m->info.yres);
        
        m->base.unlock(&m->base, buffer); 
        m->base.unlock(&m->base, m->framebuffer); 
    }
    
    return 0;
}


static  int fb_compositionComplete(struct framebuffer_device_t* dev)
{
    // do nothing, just make sure GL completion now
    eglWaitClient();
    return 0;
}

/*****************************************************************************/

int mapFrameBufferLocked(struct private_module_t* module)
{
    // already initialized...
    if (module->framebuffer) {
        return 0;
    }
        
    char const * const device_template[] = {
            "/dev/graphics/fb%u",
            "/dev/fb%u",
            0 };

    int fd = -1;
    int i=0;
    char name[64];

    while ((fd==-1) && device_template[i]) {
        snprintf(name, 64, device_template[i], 0);
        fd = open(name, O_RDWR, 0);
        i++;
    }
    if (fd < 0)
        return -errno;

    struct fb_fix_screeninfo finfo;
    if (ioctl(fd, FBIOGET_FSCREENINFO, &finfo) == -1)
        return -errno;

    struct fb_var_screeninfo info;
    if (ioctl(fd, FBIOGET_VSCREENINFO, &info) == -1)
        return -errno;

    char prop_value[16];    
    memset(prop_value, 0, sizeof(prop_value));
    property_get("marvell.graphics.fb_bpp", prop_value, "16");

    if(strcmp(prop_value, "32") == 0)
    {
        g_fb_bpp = 32;        
    }
    else if(strcmp(prop_value, "24") == 0)
    {
        g_fb_bpp = 24;        
    }
    else
    {
        g_fb_bpp = 16;
    }        

    info.reserved[0] = 0;
    info.reserved[1] = 0;
    info.reserved[2] = 0;
    info.xoffset = 0;
    info.yoffset = 0;
    info.activate = FB_ACTIVATE_NOW;

    if(g_fb_bpp == 32)
    {
        info.bits_per_pixel = 32;
    // Force config framebuffer into ARGB,
    // walkaround for color reverse issue on honeycomb
#if 1/*def MRVL_BGRA_HACK*/
        info.red.offset = 16;
        info.red.length = 8;
        info.green.offset = 8;
        info.green.length = 8;
        info.blue.offset = 0;
        info.blue.length = 8;
        info.transp.offset = 24;
        info.transp.length = 0;        
#else
        /* Explicitly request RGBA */
        info.red.offset = 0;
        info.red.length = 8;
        info.green.offset = 8;
        info.green.length = 8;
        info.blue.offset = 16;
        info.blue.length = 8;
        info.transp.offset = 24;
        info.transp.length = 0;        
#endif        

    }
    else if (g_fb_bpp == 24)
    {
        info.bits_per_pixel = 24;
        /* Explicitly request RGB */
        info.red.offset = 0;
        info.red.length = 8;
        info.green.offset = 8;
        info.green.length = 8;
        info.blue.offset = 16;
        info.blue.length = 8;
        info.transp.offset = 0;
        info.transp.length = 0;        
    }
    else
    {
        /* default is 16bpp */
        
        /*
         * Explicitly request 5/6/5
         */
        info.bits_per_pixel = 16;
        info.red.offset     = 11;
        info.red.length     = 5;
        info.green.offset   = 5;
        info.green.length   = 6;
        info.blue.offset    = 0;
        info.blue.length    = 5;
        info.transp.offset  = 0;
        info.transp.length  = 0;        
    }
    
    /*
     * Request NUM_BUFFERS screens (at lest 2 for page flipping)
     * if support triple buffer (fix gingerbread bypass mode performance issue)
     *    use triple buffer
     * else
     *    use double buffer
     */
    info.yres_virtual = ALIGN(info.yres, 16) * 3;

    uint32_t flags = PAGE_FLIP;
    if (ioctl(fd, FBIOPUT_VSCREENINFO, &info) == -1) {
        info.yres_virtual = ALIGN(info.yres, 16) * 2;
        if (ioctl(fd, FBIOPUT_VSCREENINFO, &info) == -1) {
            flags &= ~PAGE_FLIP;
            LOGW("FBIOPUT_VSCREENINFO failed, page flipping not supported");
        }
    }

    if (info.yres_virtual < ALIGN(info.yres, 16) * 2) {
        // we need at least 2 for page-flipping
        info.yres_virtual = ALIGN(info.yres, 16);
        flags &= ~PAGE_FLIP;
        LOGW("page flipping not supported (yres_virtual=%d, requested=%d)",
                info.yres_virtual, ALIGN(info.yres, 16)*2);
    }

    if (ioctl(fd, FBIOGET_VSCREENINFO, &info) == -1)
        return -errno;

    int refreshRate = 1000000000000000LLU /
    (
            uint64_t( info.upper_margin + info.lower_margin + info.yres )
            * ( info.left_margin  + info.right_margin + info.xres )
            * info.pixclock
    );

    if (refreshRate == 0) {
        // bleagh, bad info from the driver
        refreshRate = 60*1000;  // 60 Hz
    }

    if (int(info.width) <= 0 || int(info.height) <= 0) {
        // the driver doesn't return that information
        // default to 160 dpi
        info.width  = ((info.xres * 25.4f)/160.0f + 0.5f);
        info.height = ((info.yres * 25.4f)/160.0f + 0.5f);
    }

    float xdpi = (info.xres * 25.4f) / info.width;
    float ydpi = (info.yres * 25.4f) / info.height;
    float fps  = refreshRate / 1000.0f;

    LOGI(   "using (fd=%d)\n"
            "id           = %s\n"
            "xres         = %d px\n"
            "yres         = %d px\n"
            "xres_virtual = %d px\n"
            "yres_virtual = %d px\n"
            "bpp          = %d\n"
            "r            = %2u:%u\n"
            "g            = %2u:%u\n"
            "b            = %2u:%u\n",
            fd,
            finfo.id,
            info.xres,
            info.yres,
            info.xres_virtual,
            info.yres_virtual,
            info.bits_per_pixel,
            info.red.offset, info.red.length,
            info.green.offset, info.green.length,
            info.blue.offset, info.blue.length
    );

    LOGI(   "width        = %d mm (%f dpi)\n"
            "height       = %d mm (%f dpi)\n"
            "refresh rate = %.2f Hz\n",
            info.width,  xdpi,
            info.height, ydpi,
            fps
    );


    if (ioctl(fd, FBIOGET_FSCREENINFO, &finfo) == -1)
        return -errno;

    if (finfo.smem_len <= 0)
        return -errno;


    module->flags = flags;
    module->info = info;
    module->finfo = finfo;
    module->xdpi = xdpi;
    module->ydpi = ydpi;
    module->fps = fps;

    /*
     * map the framebuffer
     */

    int err;
    size_t fbSize = roundUpToPageSize(finfo.line_length * info.yres_virtual);
    module->framebuffer = new private_handle_t(dup(fd), fbSize,
            private_handle_t::PRIV_FLAGS_USES_PMEM);
    if (module->framebuffer == NULL) {
        return -ENOMEM;
    }

    module->numBuffers = info.yres_virtual / info.yres;
    module->bufferMask = 0;

    void* vaddr = mmap(0, fbSize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (vaddr == MAP_FAILED) {
        LOGE("Error mapping the framebuffer (%s)", strerror(errno));
        return -errno;
    }
    module->framebuffer->base = intptr_t(vaddr);
    memset(vaddr, 0, fbSize);

    // Init GCU for mirror display
    memset(prop_value, 0, sizeof(prop_value));
    property_get("marvell.graphics.fb_mirror", prop_value, "0");
    if(strcmp(prop_value, "1") == 0)
    {
        GCUbool inited = GCU_FALSE;

        GCU_INIT_DATA initData;
        memset(&initData, 0, sizeof(initData));
        initData.version = GCU_VERSION_2_0;
        //initData.debug = 1;
        if(gcuInitialize(&initData))
        {
            GCU_CONTEXT_DATA ctxData;
            memset(&ctxData, 0, sizeof(ctxData));
            module->m_gcu_context = gcuCreateContext(&ctxData);
            if(module->m_gcu_context)
            {

                module->m_gcu_src_surface = _gcuCreatePreAllocBuffer(module->m_gcu_context,
                                                                     info.xres, info.yres,
                                                                     (g_fb_bpp == 16) ? GCU_FORMAT_RGB565 : GCU_FORMAT_ARGB8888,
                                                                     GCU_TRUE, vaddr,
                                                                     GCU_TRUE, finfo.smem_start);

                module->m_gcu_tmp_surface = _gcuCreateBuffer( module->m_gcu_context,
                                                             info.yres_virtual, info.xres,
                                                              (g_fb_bpp == 16) ? GCU_FORMAT_RGB565 : GCU_FORMAT_ARGB8888,
                                                              &(module->m_gcu_tmp_vaddr),
                                                              &(module->m_gcu_tmp_paddr));
                GCU_FILL_DATA fillData;
                memset(&fillData, 0, sizeof(fillData));
                fillData.pSurface = module->m_gcu_tmp_surface;
                fillData.color = 0xff000000;
                fillData.bSolidColor = GCU_TRUE;
                fillData.pRect = NULL;
                gcuFill(module->m_gcu_context, &fillData);
                gcuFinish(module->m_gcu_context);
                module->m_gcu_dst_surface = _gcuCreatePreAllocBuffer(module->m_gcu_context,
                                                                     ALIGN(info.yres, 16), info.xres,
                                                                     (g_fb_bpp == 16) ? GCU_FORMAT_RGB565 : GCU_FORMAT_ARGB8888,
                                                                     GCU_TRUE, module->m_gcu_tmp_vaddr,
                                                                     GCU_TRUE, module->m_gcu_tmp_paddr);

                if(module->m_gcu_src_surface && module->m_gcu_tmp_surface && module->m_gcu_dst_surface)
                {
                    inited = GCU_TRUE;
                }
            }

            if(inited)
            {
                // Set to none mirror mode first
                ioctl(fd, FB_IOCTL_ROTATE_ADDR, 0);
                module->m_gcu_mirror_enable = GCU_FALSE;
                module->m_gcu_inited = GCU_TRUE;
            }
            else
            {
                // Roll back if has error
                if(module->m_gcu_context)
                {
                    if(module->m_gcu_dst_surface)
                    {
                        _gcuDestroyBuffer(module->m_gcu_context, module->m_gcu_dst_surface);
                        module->m_gcu_dst_surface    = NULL;
                    }

                    if(module->m_gcu_tmp_surface)
                    {
                        _gcuDestroyBuffer(module->m_gcu_context, module->m_gcu_tmp_surface);
                        module->m_gcu_tmp_vaddr      = NULL;
                        module->m_gcu_tmp_paddr      = 0;
                        module->m_gcu_tmp_surface    = NULL;
                    }

                    if(module->m_gcu_src_surface)
                    {
                        _gcuDestroyBuffer(module->m_gcu_context, module->m_gcu_src_surface);
                        module->m_gcu_src_surface    = NULL;
                    }

                    gcuDestroyContext(module->m_gcu_context);
                    module->m_gcu_context = NULL;
                }
                gcuTerminate();

                module->m_gcu_mirror_enable = GCU_FALSE;
                module->m_gcu_inited = GCU_FALSE;
            }
        }

    }

    return 0;
}

static int mapFrameBuffer(struct private_module_t* module)
{
    pthread_mutex_lock(&module->lock);
    int err = mapFrameBufferLocked(module);
    pthread_mutex_unlock(&module->lock);
    return err;
}

/*****************************************************************************/

static int fb_close(struct hw_device_t *dev)
{
    fb_context_t* ctx = (fb_context_t*)dev;
    if (ctx) {
        private_module_t* m = reinterpret_cast<private_module_t*>(dev->module);
        if(m->m_gcu_inited)
        {
            if(m->m_gcu_context)
            {
                if(m->m_gcu_dst_surface)
                {
                    _gcuDestroyBuffer(m->m_gcu_context, m->m_gcu_dst_surface);
                    m->m_gcu_dst_surface    = NULL;
                }

                if(m->m_gcu_tmp_surface)
                {
                    _gcuDestroyBuffer(m->m_gcu_context, m->m_gcu_tmp_surface);
                    m->m_gcu_tmp_vaddr      = NULL;
                    m->m_gcu_tmp_paddr      = 0;
                    m->m_gcu_tmp_surface    = NULL;
                }

                if(m->m_gcu_src_surface)
                {
                    _gcuDestroyBuffer(m->m_gcu_context, m->m_gcu_src_surface);
                    m->m_gcu_src_surface    = NULL;
                }

                gcuDestroyContext(m->m_gcu_context);
                m->m_gcu_context = NULL;
            }

            gcuTerminate();

            m->m_gcu_mirror_enable = GCU_FALSE;
            m->m_gcu_inited = GCU_FALSE;
        }

        free(ctx);
    }
    return 0;
}

int fb_device_open(hw_module_t const* module, const char* name,
        hw_device_t** device)
{

    int status = -EINVAL;
    if (!strcmp(name, GRALLOC_HARDWARE_FB0)) {
        alloc_device_t* gralloc_device;
        status = gralloc_open(module, &gralloc_device);
        if (status < 0)
            return status;

        /* initialize our state here */
        fb_context_t *dev = (fb_context_t*)malloc(sizeof(*dev));
        memset(dev, 0, sizeof(*dev));

        /* initialize the procs */
        dev->device.common.tag = HARDWARE_DEVICE_TAG;
        dev->device.common.version = 0;
        dev->device.common.module = const_cast<hw_module_t*>(module);
        dev->device.common.close = fb_close;
        dev->device.setSwapInterval = fb_setSwapInterval;
        dev->device.post            = fb_post;
        dev->device.compositionComplete = fb_compositionComplete;
        dev->device.setUpdateRect = 0;

        private_module_t* m = (private_module_t*)module;
        status = mapFrameBuffer(m);
        if (status >= 0) {
            int stride = m->finfo.line_length / (m->info.bits_per_pixel >> 3);
            /* MRVL change, set frame buffer flag to 1 */                        
            const_cast<uint32_t&>(dev->device.flags) = 1;               
            const_cast<uint32_t&>(dev->device.width) = m->info.xres;
            const_cast<uint32_t&>(dev->device.height) = m->info.yres;
            const_cast<int&>(dev->device.stride) = stride;

            if(g_fb_bpp == 32)
            {
                const_cast<int&>(dev->device.format) = HAL_PIXEL_FORMAT_RGBA_8888;
            }
            else if(g_fb_bpp == 24)
            {
                const_cast<int&>(dev->device.format) = HAL_PIXEL_FORMAT_RGB_888;
            }
            else
            {
                /* default is 16bpp */
                const_cast<int&>(dev->device.format) = HAL_PIXEL_FORMAT_RGB_565;
            }
            
            const_cast<float&>(dev->device.xdpi) = m->xdpi;
            const_cast<float&>(dev->device.ydpi) = m->ydpi;
            const_cast<float&>(dev->device.fps) = m->fps;
            const_cast<int&>(dev->device.minSwapInterval) = 0;
            const_cast<int&>(dev->device.maxSwapInterval) = 1;
            *device = &dev->device.common;
        } else {
            free(dev);
        }
    }
    return status;
}
